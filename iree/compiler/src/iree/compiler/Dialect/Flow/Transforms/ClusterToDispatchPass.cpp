// Copyright 2024 The IREE Authors
//
// Licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "iree/compiler/Dialect/Flow/IR/FlowOps.h"
#include "iree/compiler/Dialect/Flow/Transforms/Passes.h"
#include "iree/compiler/Dialect/Flow/Transforms/RegionOpUtils.h"
#include "mlir/Dialect/Func/IR/FuncOps.h"
#include "mlir/IR/Dominance.h"
#include "mlir/Analysis/TopologicalSortUtils.h"
#include "llvm/ADT/MapVector.h"
// #include "mlir/IR/IRRewriter.h"

#define DEBUG_TYPE "iree-flow-cluster-to-dispatch"

namespace mlir::iree_compiler::IREE::Flow {

#define GEN_PASS_DEF_CLUSTERTODISPATCHPASS
#include "iree/compiler/Dialect/Flow/Transforms/Passes.h.inc"

namespace {

struct ClusterToDispatchPass
    : public impl::ClusterToDispatchPassBase<ClusterToDispatchPass> {
  void runOnOperation() override {
    auto funcOp = getOperation();
    // OpBuilder builder(funcOp.getContext());
    mlir::IRRewriter rewriter(funcOp.getContext());

    // 1. Group ops by cluster.id.
    llvm::MapVector<Attribute, SmallVector<Operation *>> clusters;
    funcOp.walk([&](Operation *op) {
      auto clusterIdAttr = op->getAttr("cluster.id");
      if (clusterIdAttr) {
        clusters[clusterIdAttr].push_back(op);
      }
    });

    if (clusters.empty()) {
      return;
    }

    // 2. For each cluster, create a dispatch region.
    for (auto &it : clusters) {
      auto &opsInCluster = it.second;
      if (opsInCluster.empty()) continue;

      // Topologically sort the ops to maintain order.
      mlir::computeTopologicalSorting(opsInCluster);

      // Use the utility to wrap the cluster in a dispatch region.
    //   FailureOr<DispatchRegionOp> regionOpOrErr =
    //       wrapOpsInDispatchRegion(builder, opsInCluster);
      rewriter.setInsertionPoint(opsInCluster.front());
      FailureOr<DispatchRegionOp> regionOpOrErr =
          wrapOpsInDispatchRegion(rewriter, opsInCluster);

      if (failed(regionOpOrErr)) {
        funcOp.emitError("Failed to form dispatch region for a cluster");
        return signalPassFailure();
      }
      DispatchRegionOp regionOp = regionOpOrErr.value();

      // 3. Propagate metadata from the function to the new region.
      // 클러스터의 첫 번째 op에서 offload.target 속성을 가져와 region에 설정합니다.
      if (auto offloadTarget = opsInCluster.front()->getAttr("offload.target")) {
          regionOp->setAttr("offload.target", offloadTarget);
          opsInCluster.front()->removeAttr("offload.target"); // Optional: cleanup
      }

      // 4. Calculate total FLOPs for the region and attach it.
      int64_t totalFlops = 0;
      regionOp.getBody().walk([&](Operation *op) {
        if (auto flopsAttr = op->getAttrOfType<IntegerAttr>("flow.debug.flops")) {
          totalFlops += flopsAttr.getInt();
        }
      });
      if (totalFlops > 0) {
        // regionOp->setAttr("dispatch.debug.flops.total", builder.getI64IntegerAttr(totalFlops));
        regionOp->setAttr("dispatch.debug.flops.total", rewriter.getI64IntegerAttr(totalFlops));
      }
    }
  }
};

} // namespace

std::unique_ptr<Pass> createClusterToDispatchPass() {
  return std::make_unique<ClusterToDispatchPass>();
}

} // namespace mlir::iree_compiler::IREE::Flow