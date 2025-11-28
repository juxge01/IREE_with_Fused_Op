#include "iree/compiler/Dialect/Flow/IR/FlowDialect.h"
#include "iree/compiler/Dialect/Flow/IR/FlowOps.h"
#include "iree/compiler/Dialect/Flow/Transforms/Passes.h"

#include "mlir/Dialect/Arith/IR/Arith.h"
#include "mlir/Dialect/Func/IR/FuncOps.h"
#include "mlir/IR/Builders.h"
#include "mlir/IR/BuiltinOps.h"
#include "mlir/IR/BuiltinTypes.h"
#include "mlir/IR/Attributes.h"
#include "mlir/Pass/Pass.h"

// --- 아래 헤더들이 필요합니다 ---
#include "mlir/Dialect/Linalg/IR/Linalg.h"
#include "mlir/Dialect/Tensor/IR/Tensor.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/ADT/SetVector.h"
#include "llvm/Support/raw_ostream.h"

namespace mlir::iree_compiler::IREE::Flow {

#define GEN_PASS_DEF_AUTOFUSIONCLUSTERPASS
#include "iree/compiler/Dialect/Flow/Transforms/Passes.h.inc"

namespace {

struct AutoFusionClusterPass
    : public impl::AutoFusionClusterPassBase<AutoFusionClusterPass> {
  void runOnOperation() override {
    OpBuilder builder(&getContext());
    auto clusterAttrName = builder.getStringAttr("fusion.cluster");

    getOperation()->walk([&](func::FuncOp funcOp) {
        if (funcOp.getBody().empty()) return;

        Operation *anchorOp = nullptr;
        long long maxFlops = 0;
        funcOp.walk([&](Operation *op) {
            if (auto flopsAttr = op->getAttrOfType<IntegerAttr>("FLOPs")) {
                if (flopsAttr.getInt() > maxFlops) {
                    maxFlops = flopsAttr.getInt();
                    anchorOp = op;
                }
            }
        });

        if (!anchorOp) return;

        SmallVector<llvm::SetVector<Operation *>, 4> candidateClusters;

        llvm::SetVector<Operation *> anchorOnlyCluster;
        anchorOnlyCluster.insert(anchorOp);
        candidateClusters.push_back(anchorOnlyCluster);

        llvm::SetVector<Operation *> producerAnchorCluster;
        producerAnchorCluster.insert(anchorOp);
        for (Value operand : anchorOp->getOperands()) {
            if (Operation *definingOp = operand.getDefiningOp()) {
                if (definingOp->getBlock() == anchorOp->getBlock()) {
                    producerAnchorCluster.insert(definingOp);
                }
            }
        }
        candidateClusters.push_back(producerAnchorCluster);

        llvm::SetVector<Operation *> fullCluster = producerAnchorCluster;
        for (Operation *user : anchorOp->getUsers()) {
            if (user->getBlock() == anchorOp->getBlock()) {
                fullCluster.insert(user);
            }
        }
        candidateClusters.push_back(fullCluster);

        llvm::SetVector<Operation *> bestCluster;
        double maxGain = -1.0;

        llvm::errs() << "--- Analyzing Fusion Candidates ---\n";
        for (const auto &cluster : candidateClusters) {
            long long totalFlops = 0;
            for (Operation *op : cluster) {
                if (auto flopsAttr = op->getAttrOfType<IntegerAttr>("FLOPs")) {
                    totalFlops += flopsAttr.getInt();
                } else if (isa<linalg::LinalgOp, tensor::EmptyOp>(op)) {
                    totalFlops += 1;
                }
            }

            SmallVector<Value, 4> regionOutputs;
            for (Operation *opToMove : cluster) {
                for (Value result : opToMove->getResults()) {
                    for (Operation *user : result.getUsers()) {
                        if (cluster.count(user) == 0) {
                            regionOutputs.push_back(result);
                            break;
                        }
                    }
                }
            }
            
            long long totalBytes = 0;
            for (Value output : regionOutputs) {
                if (auto tensorType = mlir::dyn_cast<RankedTensorType>(output.getType())) {
                    long long numElements = 1;
                    for (int64_t dim : tensorType.getShape()) {
                        if (ShapedType::isDynamic(dim)) { numElements = 0; break; }
                        numElements *= dim;
                    }
                    totalBytes += numElements * 4;
                }
            }
            
            double gain = (totalBytes > 0) ? static_cast<double>(totalFlops) / totalBytes : 0.0;

            llvm::errs() << "  - Candidate with " << cluster.size() << " ops | "
                         << "TotalFLOPs: " << totalFlops << " | "
                         << "Gain: " << gain << "\n";
                         
            if (gain > maxGain) {
                maxGain = gain;
                bestCluster = cluster;
            }
        }
        
        if (!bestCluster.empty()) {
            llvm::errs() << "--- Selected Best Cluster (Gain: " << maxGain << ") ---\n";
            auto clusterIDAttr = builder.getStringAttr("cluster_0");
            for (Operation *op : bestCluster) {
                op->setAttr(clusterAttrName, clusterIDAttr);
            }
        }
    });
  }
};
} // namespace

std::unique_ptr<Pass> createAutoFusionClusterPass() {
  return std::make_unique<AutoFusionClusterPass>();
}

} // namespace mlir::iree_compiler::IREE::Flow