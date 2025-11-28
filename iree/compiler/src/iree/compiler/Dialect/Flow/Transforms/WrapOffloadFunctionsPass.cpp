#include "iree/compiler/Dialect/Flow/IR/FlowDialect.h"
#include "iree/compiler/Dialect/Flow/IR/FlowOps.h"
#include "iree/compiler/Dialect/Flow/Transforms/Passes.h"
#include "iree/compiler/Dialect/Flow/Transforms/RegionOpUtils.h"
#include "mlir/Dialect/Arith/IR/Arith.h"
#include "mlir/Dialect/Func/IR/FuncOps.h"
#include "mlir/Dialect/Linalg/IR/Linalg.h"
#include "mlir/Dialect/Tensor/IR/Tensor.h"
#include "mlir/IR/Builders.h"
#include "mlir/IR/Dominance.h"
#include "mlir/Analysis/TopologicalSortUtils.h"
#include "mlir/Pass/Pass.h"
#include "mlir/Transforms/GreedyPatternRewriteDriver.h"
#include "llvm/Support/Debug.h"
#include "llvm/ADT/SetVector.h"

namespace mlir::iree_compiler::IREE::Flow {

#define GEN_PASS_DEF_WRAPOFFLOADFUNCTIONSPASS
#include "iree/compiler/Dialect/Flow/Transforms/Passes.h.inc"

#define DEBUG_TYPE "iree-flow-wrap-offload-functions"

namespace {

struct WrapOffloadFunctionsPass
    : public impl::WrapOffloadFunctionsPassBase<WrapOffloadFunctionsPass> {
  void getDependentDialects(DialectRegistry &registry) const override {
    registry.insert<IREE::Flow::FlowDialect,
                    mlir::func::FuncDialect,
                    mlir::arith::ArithDialect,
                    mlir::tensor::TensorDialect>();
  }

  void runOnOperation() override {
    ModuleOp module = getOperation();

    // 1. `offload.anchor` 속성을 가진 연산을 포함하는 모든 함수를 찾습니다.
    //    수정 중 반복자 무효화를 피하기 위해 먼저 수집합니다.
    SmallVector<func::FuncOp> funcsToWrap;
    for (auto funcOp : module.getOps<func::FuncOp>()) {
      // 이미 flow.executable 내부에 있는 함수는 건너뜁니다.
      if (funcOp->getParentOfType<IREE::Flow::ExecutableOp>()) {
        continue;
      }
      // funcOp.walk(& {
      //   if (op->hasAttr("offload.anchor")) {
      //     funcsToWrap.push_back(funcOp);
      //     return WalkResult::interrupt();
      //   }
      //   return WalkResult::advance();
      // });

      funcOp.walk([&](Operation *op) -> WalkResult {
        if (op->hasAttr("offload.anchor")) {
          funcsToWrap.push_back(funcOp);
          return WalkResult::interrupt(); // 이미 이 함수는 포장 대상이므로 중단
        }
        return WalkResult::advance();
      });
    }

    if (funcsToWrap.empty()) {
      return;
    }

    // 2. Iterate over each found function and wrap its body.
    for (auto funcOp : funcsToWrap) {
      LLVM_DEBUG(llvm::dbgs() << "[WrapOffloadFunctionsPass] Wrapping function: "
                              << funcOp.getName() << "\n");

      // The function must have exactly one block to be wrapped.
      if (!llvm::hasSingleElement(funcOp.getBody())) {
        funcOp.emitError("can only wrap functions with a single block");
        signalPassFailure();
        return;
      }

      Block &block = funcOp.front();
      Operation *terminator = block.getTerminator();
      if (!terminator) {
        funcOp.emitError("function has no terminator; cannot wrap");
        signalPassFailure();
        return;
      }

      OpBuilder builder(funcOp);

      // Compute dynamic dimensions for the results before creating the region.
      // These will become the result_dims of the dispatch region.
      // The dim ops must be created before the region, and they will be moved inside.
      builder.setInsertionPoint(terminator);
      SmallVector<Value> dynamicResultDims;
      for (Value result : terminator->getOperands()) {
        auto rankedType = dyn_cast<RankedTensorType>(result.getType());
        if (!rankedType) continue;
        for (auto it : llvm::enumerate(rankedType.getShape())) {
          if (ShapedType::isDynamic(it.value())) {
            dynamicResultDims.push_back(
                builder.create<tensor::DimOp>(result.getLoc(), result, it.index()));
          }
        }
      }

      // Create the flow.dispatch.region op at the start of the function.
      // builder.setInsertionPointToStart(&block);
      builder.setInsertionPoint(terminator);

      auto regionOp = builder.create<IREE::Flow::DispatchRegionOp>(
          funcOp.getLoc(), terminator->getOperandTypes(), dynamicResultDims,
          /*workload=*/ValueRange{});

      // `offload.anchor`를 가진 연산을 찾아 `offload.target` 속성을 regionOp로 복사합니다.
      // 이 속성은 나중에 OutlineDispatchRegionsPass에서 flow.executable로 전달됩니다.
      // funcOp.walk(& {
      //   if (op->hasAttr("offload.anchor")) {
      //     if (auto targetAttr = op->getAttr(offloadTargetAttrName)) {
      //       regionOp->setAttr(offloadTargetAttrName, targetAttr);
      //       // 앵커의 offload.target을 찾았으므로 더 이상 순회할 필요가 없습니다.
      //       return WalkResult::interrupt();
      //     }
      //   }
      //   return WalkResult::advance();
      // });
      StringRef offloadTargetAttrName = "offload.target";
      funcOp.walk([&](Operation *op) -> WalkResult {
        if (!op->hasAttr("offload.anchor")) return WalkResult::advance();
        if (Attribute targetAttr = op->getAttr(offloadTargetAttrName)) {
          regionOp->setAttr(offloadTargetAttrName, targetAttr);
        }
        return WalkResult::interrupt(); // 하나만 복사하면 충분
      });


      // For debugging purposes, calculate the total FLOPs for the newly created
      // region and attach it as an attribute. This attribute will then be
      // preserved by our modified OutlineDispatchRegionsPass.
      int64_t totalFlops = 0;
      // We walk the original block *before* ops are moved into the region.
      block.walk([&](Operation *op) {
        // Don't double count if we walk into another region.
        if (op->getParentOp() != funcOp) return;
        if (auto flopsAttr = op->getAttrOfType<IntegerAttr>("FLOPs")) {
          totalFlops += flopsAttr.getInt();
        }
      });
      if (totalFlops > 0) {
        regionOp->setAttr("total_flops", builder.getI64IntegerAttr(totalFlops));
      }

      // Create the region's body and remap arguments.
      Region &region = regionOp.getBody();
      region.push_back(new Block);
      Block &regionBlock = region.front();
      regionBlock.addArguments(
          funcOp.getArgumentTypes(),
          SmallVector<Location>(funcOp.getNumArguments(), funcOp.getLoc()));
      for (auto it : llvm::zip(funcOp.getArguments(), regionBlock.getArguments())) {
        std::get<0>(it).replaceAllUsesWith(std::get<1>(it));
      }

      // Move all ops from the function body into the region.
      // The range is all operations *except* the terminator.
      auto &destOps = regionBlock.getOperations();
      auto &srcOps = block.getOperations();
      // destOps.splice(destOps.end(), srcOps, srcOps.begin(), terminator->getIterator());
      destOps.splice(destOps.end(), srcOps, srcOps.begin(), regionOp->getIterator());

      // Replace the original return with flow.return and a new func.return.
      builder.setInsertionPointToEnd(&regionBlock);
      builder.create<IREE::Flow::ReturnOp>(terminator->getLoc(), terminator->getOperands());
      builder.setInsertionPoint(terminator);
      builder.create<func::ReturnOp>(terminator->getLoc(), regionOp.getResults());
      terminator->erase();

      // 이제 `offload.anchor` 속성은 역할을 다했으므로 제거합니다.
      // `offload.target`은 regionOp로 옮겨졌습니다.
      // funcOp.walk(& { op->removeAttr("offload.anchor"); });
      funcOp.walk([&](Operation *op) {
        op->removeAttr("offload.anchor");
      });
    }
  }
};
} // namespace

std::unique_ptr<Pass> createWrapOffloadFunctionsPass() {
  return std::make_unique<WrapOffloadFunctionsPass>();
}
} // namespace mlir::iree_compiler::IREE::Flow