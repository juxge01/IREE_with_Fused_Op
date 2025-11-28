#include "iree/compiler/Dialect/Flow/Transforms/Passes.h"

// 필요한 헤더들을 모두 포함합니다.
#include "mlir/Dialect/Arith/IR/Arith.h"
#include "mlir/Dialect/Func/IR/FuncOps.h"
#include "mlir/Dialect/Linalg/IR/Linalg.h"
#include "mlir/Dialect/Math/IR/Math.h"
#include "mlir/IR/Builders.h"
#include "mlir/IR/BuiltinOps.h"
#include "mlir/Pass/Pass.h"
#include "llvm/ADT/TypeSwitch.h"

namespace mlir::iree_compiler::IREE::Flow {

#define GEN_PASS_DEF_MLIRFLOPSANALYSISPASS
#include "iree/compiler/Dialect/Flow/Transforms/Passes.h.inc"

namespace {

// linalg.generic 연산의 몸체(body) 내부의 FLOPs를 계산하는 헬퍼 함수
long long countFlopsInRegion(Region &region) {
  long long flops = 0;
  if (region.empty()) return flops;
  region.walk([&](Operation *op) {
    if (isa<arith::AddFOp, arith::SubFOp, arith::MulFOp, arith::DivFOp,
            arith::MaximumFOp, arith::MinimumFOp>(op)) {
      flops++;
    } else if (isa<math::FmaOp>(op)) {
      flops += 2;
    }
  });
  return flops;
}

struct MLIRFLOPsAnalysisPass
    : public impl::MLIRFLOPsAnalysisPassBase<MLIRFLOPsAnalysisPass> {
  void runOnOperation() override {
    OpBuilder builder(&getContext());

    getOperation()->walk([&](func::FuncOp funcOp) {
      if (funcOp->getParentOfType<IREE::Flow::ExecutableOp>()) {
        return;
      }

      funcOp.walk([&](Operation *op) {
        long long flops = 0;

        // TypeSwitch를 사용하여 op의 종류를 확인하고 FLOPs를 계산합니다.
        TypeSwitch<Operation *>(op)
            .Case<linalg::MatmulOp>([&](auto matmulOp) {
              auto lhsType = dyn_cast<RankedTensorType>(
                  matmulOp.getDpsInputOperand(0)->get().getType());
              auto rhsType = dyn_cast<RankedTensorType>(
                  matmulOp.getDpsInputOperand(1)->get().getType());
              if (!lhsType || !rhsType || !lhsType.hasStaticShape() ||
                  !rhsType.hasStaticShape() || lhsType.getRank() != 2 ||
                  rhsType.getRank() != 2) {
                return;
              }
              auto lhsShape = lhsType.getShape();
              auto rhsShape = rhsType.getShape();
              long long M = lhsShape[0], K = lhsShape[1], N = rhsShape[1];
              flops = 2LL * M * K * N;
            })
            .Case<linalg::GenericOp>([&](auto genericOp) {
              if (!genericOp.hasPureTensorSemantics() ||
                  genericOp.getNumResults() == 0) {
                return;
              }
              auto outputType = dyn_cast<RankedTensorType>(
                  genericOp->getResult(0).getType());
              if (!outputType || !outputType.hasStaticShape()) {
                return;
              }
              long long numIterations = 1;
              for (int64_t dim : outputType.getShape()) {
                numIterations *= dim;
              }
              if (numIterations > 0) {
                long long flopsInBody = countFlopsInRegion(genericOp.getRegion());
                flops = numIterations * flopsInBody;
              }
            })
            .Case<arith::AddFOp, arith::SubFOp, arith::MulFOp, arith::DivFOp>(
                [&](Operation *arithOp) {
                  if (auto tensorType = dyn_cast<RankedTensorType>(
                          arithOp->getResult(0).getType())) {
                    if (!tensorType.hasStaticShape()) return;
                    long long numElements = 1;
                    for (int64_t dim : tensorType.getShape()) {
                      numElements *= dim;
                    }
                    flops = numElements;
                  }
                });

        if (flops > 0) {
          op->setAttr("FLOPs", builder.getI64IntegerAttr(flops));
        }
      });
    });
  }
};
} // namespace

std::unique_ptr<Pass> createMLIRFLOPsAnalysisPass() {
  return std::make_unique<MLIRFLOPsAnalysisPass>();
}

} // namespace mlir::iree_compiler::IREE::Flow