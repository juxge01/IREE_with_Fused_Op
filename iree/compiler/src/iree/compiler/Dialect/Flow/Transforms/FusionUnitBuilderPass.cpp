// Copyright 2024 The IREE Authors
//
// Licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "iree/compiler/Dialect/Flow/IR/FlowOps.h"
#include "iree/compiler/Dialect/Flow/Transforms/Passes.h"
#include "iree/compiler/Dialect/Flow/Transforms/RegionOpUtils.h"
#include "mlir/Dialect/Func/IR/FuncOps.h"
#include "mlir/Dialect/Linalg/IR/Linalg.h"
#include "mlir/Dialect/Tensor/IR/Tensor.h"
#include "mlir/IR/Dominance.h"
#include "mlir/Interfaces/TilingInterface.h"
#include "llvm/ADT/SetVector.h"

#include "mlir/Dialect/Arith/IR/Arith.h"
#include "mlir/Dialect/Math/IR/Math.h"
#include "mlir/IR/IRMapping.h" 
#include "mlir/IR/PatternMatch.h"


#define DEBUG_TYPE "iree-flow-fusion-unit-builder"

namespace mlir::iree_compiler::IREE::Flow {

#define GEN_PASS_DEF_FUSIONUNITBUILDERPASS
#include "iree/compiler/Dialect/Flow/Transforms/Passes.h.inc"

namespace {

/// Returns true if the given op is a generic op with only parallel iterators.
static bool isElementwiseGeneric(Operation *op) {
  auto genericOp = dyn_cast<linalg::GenericOp>(op);
  if (!genericOp) return false;
  return genericOp.getNumParallelLoops() == genericOp.getNumLoops();
}

/// Returns true if an op is considered fusable with an anchor.
static bool isFusableOp(Operation *op) {
  // Linalg element-wise
  if (isElementwiseGeneric(op)) return true;

  // Standard element-wise ops
  if (isa<arith::AddFOp, arith::SubFOp, arith::MulFOp, arith::DivFOp,
           arith::MaximumFOp, arith::MinimumFOp, arith::SelectOp,
           math::AbsFOp, math::ExpOp, math::LogOp, math::RsqrtOp, 
           math::SqrtOp, math::TanhOp>(op)) {
    return true;
  }
  // Casts
  return isa<tensor::CastOp>(op);
}

/// Checks if all users of the op's results are in the cluster.
static bool allUsersAreInCluster(Operation *op, 
                                 const llvm::SetVector<Operation *> &cluster) {
  for (Value result : op->getResults()) {
    for (Operation *user : result.getUsers()) {
      if (!cluster.contains(user)) {
        return false;
      }
    }
  }
  return true;
}

/// Checks if all operands of the op are defined within the cluster.
static bool allOperandsAreInCluster(Operation *op,
                                    const llvm::SetVector<Operation *> &cluster) {
  for (Value operand : op->getOperands()) {
    Operation *producer = operand.getDefiningOp();
    if (producer && !cluster.contains(producer)) {
      return false;
    }
  }
  return true;
}


struct FusionUnitBuilderPass
    : public impl::FusionUnitBuilderPassBase<FusionUnitBuilderPass> {
  void getDependentDialects(DialectRegistry &registry) const override {
    registry.insert<IREE::Flow::FlowDialect, linalg::LinalgDialect,
                    arith::ArithDialect, math::MathDialect>();
  }
  void runOnOperation() override {
    auto funcOp = getOperation();
    IRRewriter rewriter(funcOp.getContext());

    // 1. Find all anchor ops.
    SmallVector<Operation *> anchors;
    funcOp.walk([&](Operation *op) {
      if (op->hasAttr("offload.anchor")) {
        anchors.push_back(op);
      }
    });

    if (anchors.empty()) {
      return;
    }

    llvm::DenseSet<Operation *> clusteredOps;

    // 2. For each anchor, build a cluster.
    for (Operation *anchor : anchors) {
      if (clusteredOps.contains(anchor)) continue;

      llvm::SetVector<Operation *> cluster;
      cluster.insert(anchor);

      // Fuse producers (Backward)
      SmallVector<Operation *> worklist;
      worklist.push_back(anchor);
      while (!worklist.empty()) {
        Operation *current = worklist.pop_back_val();
        for (Value operand : current->getOperands()) {
          Operation *producer = operand.getDefiningOp();
          if (producer && isFusableOp(producer) && 
              !cluster.contains(producer) &&
              allUsersAreInCluster(producer, cluster)) { // Safety check
            cluster.insert(producer);
            worklist.push_back(producer);
          }
        }
      }

      // Fuse consumers (Forward)
      worklist.clear();
      for(Operation *op : cluster) worklist.push_back(op);

      while (!worklist.empty()) {
        Operation *current = worklist.pop_back_val();
        for (Value result : current->getResults()) {
          for (Operation *user : result.getUsers()) {
            if (isFusableOp(user) && 
                !cluster.contains(user) &&
                allOperandsAreInCluster(user, cluster)) { // Safety check
              cluster.insert(user);
              worklist.push_back(user);
            }
          }
        }
      }

      // 3. Before wrapping, compute cluster-level transfer bytes and benefit.
      if (cluster.size() <= 0) continue;

      // Gather external inputs (Values whose defining op is outside the cluster)
      llvm::DenseSet<Value> externalInputs;
      llvm::DenseSet<Value> externalOutputs;
      for (Operation *op : cluster) {
        for (Value operand : op->getOperands()) {
          Operation *producer = operand.getDefiningOp();
          if (producer && !cluster.contains(producer)) externalInputs.insert(operand);
        }
        for (Value result : op->getResults()) {
          for (Operation *user : result.getUsers()) {
            if (!cluster.contains(user)) externalOutputs.insert(result);
          }
        }
      }

      // Helper: compute element count for a RankedTensorType (static only)
      auto getElems = [&](Type t) -> long long {
        if (auto rt = dyn_cast<RankedTensorType>(t)) {
          if (!rt.hasStaticShape()) return 0;
          long long e = 1;
          for (int64_t d : rt.getShape()) e *= (d == ShapedType::kDynamic ? 1 : d);
          return e;
        }
        return 0;
      };

      auto getElemBytes = [&](Type t) -> long long {
        if (auto ft = dyn_cast<FloatType>(t)) return ft.getWidth() / 8;
        return 4; // default
      };

      // Sum cluster FLOPs from flow.debug.flops attributes when available
      long long clusterFlops = 0;
      for (Operation *op : cluster) {
        if (auto flopsAttr = op->getAttrOfType<IntegerAttr>("flow.debug.flops"))
          clusterFlops += flopsAttr.getInt();
      }

      // Sum external bytes: inputs + outputs (conservative)
      long long clusterBytes = 0;
      for (Value v : externalInputs) {
        clusterBytes += getElems(v.getType()) * getElemBytes(v.getType());
      }
      for (Value v : externalOutputs) {
        clusterBytes += getElems(v.getType()) * getElemBytes(v.getType());
      }

      // Detailed debug logging to help understand why a cluster may be skipped
      LLVM_DEBUG({
        llvm::dbgs() << "FusionUnitBuilder: cluster size=" << cluster.size()
                     << ", clusterFlops=" << clusterFlops
                     << ", clusterBytes(external)=" << clusterBytes << "\n";
        llvm::dbgs() << "  externalInputs.count=" << externalInputs.size()
                     << ", externalOutputs.count=" << externalOutputs.size() << "\n";
        // Print up to a few external inputs/outputs types for inspection
        int idx = 0;
        for (Value v : externalInputs) {
          if (idx++ >= 5) break;
          llvm::dbgs() << "    ext_in[" << idx-1 << "] type=" << v.getType() << "\n";
        }
        idx = 0;
        for (Value v : externalOutputs) {
          if (idx++ >= 5) break;
          llvm::dbgs() << "    ext_out[" << idx-1 << "] type=" << v.getType() << "\n";
        }
      });

      // Simple time model (same constants as planner)
      const double P_cpu = 10e9;       // 10 GFLOP/s (단일 코어)
      const double BW_cpu = 20e9;      // 20 GB/s (메모리 BW)
      const double P_acc = 100e9;      // 100 GFLOP/s (가속기)
      const double BW_acc = 100e9;     // 100 GB/s (가속기 메모리)
      const double BW_h2d = 10e9;      // 10 GB/s (PCIe)
      const double BW_d2h = 10e9;      // 10 GB/s (PCIe)
      const double launch_overhead = 1e-5;  // 10 µs

      double t_cpu = 0.0, t_acc = 0.0;
      if (clusterFlops > 0 || clusterBytes > 0) {
        t_cpu = std::max(clusterFlops / P_cpu, (double)clusterBytes / BW_cpu);
        t_acc = std::max(clusterFlops / P_acc, (double)clusterBytes / BW_acc) +
                (double)clusterBytes / BW_h2d + (double)clusterBytes / BW_d2h + launch_overhead;
      } else {
        // Fallback: nothing to offload
        t_cpu = 0.0;
        t_acc = 0.0;
      }

      double clusterBenefit = t_cpu - t_acc;

      // If cluster-level benefit is non-positive, skip wrapping (avoid harming perf)
      if (clusterBenefit <= 0.0) {
        LLVM_DEBUG(llvm::dbgs() << "Skipping cluster around anchor due to non-positive benefit: "
                                << clusterBenefit << "s\n");
        // mark these ops as processed to avoid reconsideration
        for (Operation *op : cluster) clusteredOps.insert(op);
        continue;
      }

      auto offloadTargetAttr = anchor->getAttr("offload.target");
      if (!offloadTargetAttr) continue;

      rewriter.setInsertionPoint(anchor);

      // Wrap ops in a dispatch region
      FailureOr<DispatchRegionOp> regionOp = wrapOpsInDispatchRegion(rewriter, cluster.getArrayRef());
      if (failed(regionOp)) {
        anchor->emitError("Failed to form dispatch region around anchor op");
        return signalPassFailure();
      }

      // 4. Attach attributes to the new DispatchRegionOp.
      DispatchRegionOp dispatchOp = *regionOp;
      dispatchOp->setAttr("offload.target", offloadTargetAttr);

      // Remove anchor tags from ops inside the cluster
      for (Operation *op : cluster) op->removeAttr("offload.anchor");

      // Track processed ops
      for (Operation *op : cluster) clusteredOps.insert(op);
    }
  }
};

} // namespace

std::unique_ptr<Pass> createFusionUnitBuilderPass() {
  return std::make_unique<FusionUnitBuilderPass>();
}

} // namespace mlir::iree_compiler::IREE::Flow