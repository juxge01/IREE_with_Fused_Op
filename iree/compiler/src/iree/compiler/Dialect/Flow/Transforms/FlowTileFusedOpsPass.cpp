#include "iree/compiler/Dialect/Flow/IR/FlowDialect.h"
#include "iree/compiler/Dialect/Flow/IR/FlowOps.h"
#include "iree/compiler/Dialect/Flow/Transforms/Passes.h"
#include "iree/compiler/Dialect/HAL/IR/HALOps.h"
#include "iree/compiler/Dialect/HAL/IR/HALDialect.h"

#include "mlir/Dialect/Arith/IR/Arith.h"
#include "mlir/Dialect/Func/IR/FuncOps.h"
#include "mlir/Dialect/Linalg/IR/Linalg.h"
#include "mlir/Dialect/SCF/IR/SCF.h"
#include "mlir/Dialect/SCF/Transforms/TileUsingInterface.h"
#include "mlir/Interfaces/TilingInterface.h"
#include "mlir/Dialect/Tensor/IR/Tensor.h"
#include "mlir/IR/PatternMatch.h"
#include "mlir/Pass/Pass.h"

namespace mlir::iree_compiler::IREE::Flow {

#define GEN_PASS_DEF_FLOWTILEFUSEDOPSPASS
#include "iree/compiler/Dialect/Flow/Transforms/Passes.h.inc"

namespace {

struct FlowTileFusedOpsPass
    : public impl::FlowTileFusedOpsPassBase<FlowTileFusedOpsPass> {
  void getDependentDialects(DialectRegistry &registry) const override {
    registry.insert<IREE::HAL::HALDialect, linalg::LinalgDialect,
                    scf::SCFDialect, arith::ArithDialect>();
  }

  void runOnOperation() override {
    // 이 패스는 func::FuncOp에 대해 실행되므로, getOperation()은 func::FuncOp을 반환합니다.
    func::FuncOp funcOp = getOperation();
    MLIRContext *context = &getContext();

    mlir::linalg::MatmulOp matmulOpToTile;
    funcOp.walk([&](mlir::linalg::MatmulOp matmulOp) {
      // 이미 타일링된 루프 안에 있는 matmul은 건너뜁니다.
      if (matmulOp->getParentOfType<scf::ForOp>()) {
        return WalkResult::advance();
      }
      matmulOpToTile = matmulOp;
      return WalkResult::interrupt();
    });

    if (!matmulOpToTile) {
      return;
    }

    PatternRewriter rewriter(context);
    rewriter.setInsertionPoint(matmulOpToTile);

    scf::SCFTilingOptions options;
    SmallVector<OpFoldResult> tileSizes = {rewriter.getIndexAttr(32),
                                           rewriter.getIndexAttr(32),
                                           rewriter.getIndexAttr(0)};
    options.setTileSizes(tileSizes);

    FailureOr<scf::SCFTilingResult> tiledResult = scf::tileUsingSCF(
        rewriter, cast<TilingInterface>(matmulOpToTile.getOperation()),
        options);

    if (failed(tiledResult)) {
      funcOp.emitError("Failed to tile linalg.matmul op");
      signalPassFailure();
      return;
    }
    rewriter.replaceOp(matmulOpToTile, tiledResult->replacements);
  }
};

} // namespace

std::unique_ptr<Pass> createFlowTileFusedOpsPass() {
  return std::make_unique<FlowTileFusedOpsPass>();
}

} // namespace mlir::iree_compiler::IREE::Flow