#include "iree/compiler/Dialect/Flow/IR/FlowDialect.h"
#include "iree/compiler/Dialect/Flow/IR/FlowOps.h"
#include "iree/compiler/Dialect/Flow/Transforms/Passes.h"

#include "mlir/Dialect/Arith/IR/Arith.h"
#include "mlir/Dialect/Func/IR/FuncOps.h"
#include "mlir/Dialect/Linalg/IR/Linalg.h"  // ✅ Linalg 필터에 사용
#include "mlir/IR/Builders.h"
#include "mlir/IR/BuiltinOps.h"
#include "mlir/IR/BuiltinTypes.h"
#include "mlir/IR/Attributes.h"
#include "mlir/Pass/Pass.h"

#include "llvm/ADT/STLExtras.h"
#include "llvm/ADT/SetVector.h"

namespace mlir::iree_compiler::IREE::Flow {

#define GEN_PASS_DEF_AUTOFINDSPLITPOINTPASS
#include "iree/compiler/Dialect/Flow/Transforms/Passes.h.inc"

namespace {

static constexpr llvm::StringLiteral kFlopsAttr("flow.debug.flops"); // ✅ 통일
static constexpr llvm::StringLiteral kAnchorAttr("offload.anchor");   // ✅ 후속 패스와 키 맞춤
// static constexpr llvm::StringLiteral kBoundaryAttr("offload.boundary"); // 필요 시 병행 태깅

struct AutoFindSplitPointPass
    : public impl::AutoFindSplitPointPassBase<AutoFindSplitPointPass> {
  void runOnOperation() override {
    OpBuilder builder(&getContext());
    auto unitAttr = builder.getUnitAttr();

    getOperation()->walk([&](func::FuncOp funcOp) {
      if (funcOp.isExternal() || funcOp.getBody().empty()) return;

      // 함수에 FLOPs 달린 Linalg가 하나도 없으면 조용히 스킵 (아웃라인 이후 등)
      bool hasFlopsLinalg = false;
      funcOp.walk([&](Operation *op) {
        if (isa<linalg::LinalgOp>(op) &&
            op->getAttrOfType<IntegerAttr>(kFlopsAttr))
          hasFlopsLinalg = true;
      });
      if (!hasFlopsLinalg) return;

      Operation *splitPointOp = nullptr;
      int64_t maxCumulativeCost = std::numeric_limits<int64_t>::min();
      int64_t currentCumulativeCost = 0;

      // ✅ 함수 전체(모든 블록)를 순회하되,
      //    FLOPs가 있는 "계산 연산"에서만 누적/갱신
      for (Block &block : funcOp.getBody().getBlocks()) {
        for (Operation &op : block) {
          if (op.hasTrait<OpTrait::IsTerminator>()) continue;

          if (!op.hasAttr("offload.anchor")) continue;

          auto flopsAttr  = op.getAttrOfType<IntegerAttr>("FLOPs");
          auto flopsAttr2 = op.getAttrOfType<IntegerAttr>("flow.debug.flops");
          if (!flopsAttr && !flopsAttr2) continue;

          long long thisFlops = flopsAttr ? flopsAttr.getInt() : flopsAttr2.getInt();
          currentCumulativeCost += thisFlops;

          // ✅ '>' 사용: 동일 누적값일 때 뒤쪽 op가 덮어쓰지 않도록
          if (currentCumulativeCost > maxCumulativeCost) {
            maxCumulativeCost = currentCumulativeCost;
            splitPointOp = &op;
          }
        }
      }

      if (!splitPointOp) return;

      llvm::errs() << "Attaching anchor to op with max cumulative FLOPs "
                   << maxCumulativeCost << ": " << *splitPointOp << "\n";

      // ✅ 후속 FusionUnitBuilder가 읽는 키로 통일
      splitPointOp->setAttr(kAnchorAttr, unitAttr);
      // 필요하면 경계 키도 함께 태깅:
      // splitPointOp->setAttr(kBoundaryAttr, unitAttr);
    });
  }
};

}  // namespace

std::unique_ptr<Pass> createAutoFindSplitPointPass() {
  return std::make_unique<AutoFindSplitPointPass>();
}

}  // namespace mlir::iree_compiler::IREE::Flow
