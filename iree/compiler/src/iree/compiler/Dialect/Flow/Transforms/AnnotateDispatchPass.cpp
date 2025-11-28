#include "iree/compiler/Dialect/Flow/IR/FlowOps.h"
#include "iree/compiler/Dialect/Flow/Transforms/Passes.h"
#include "iree/compiler/Dialect/HAL/IR/HALDialect.h"
#include "iree/compiler/Dialect/HAL/IR/HALOps.h"

#include "mlir/IR/Builders.h"
#include "mlir/IR/BuiltinAttributes.h"   // StringAttr 등
#include "mlir/IR/BuiltinOps.h"          // ModuleOp
#include "mlir/Pass/Pass.h"

namespace mlir::iree_compiler::IREE::Flow {
namespace {

class AnnotateDispatchPass
    : public PassWrapper<AnnotateDispatchPass, OperationPass<ModuleOp>> {
 public:
  StringRef getArgument() const override { return "iree-flow-annotate-dispatch"; }
  StringRef getDescription() const override {
    return "Annotates hal.executable.variant targets for the 'accel' backend.";
  }

  // ★ 세그폴트 방지: HALDialect를 반드시 로드
  void getDependentDialects(DialectRegistry &registry) const override {
    registry.insert<IREE::HAL::HALDialect>();
  }

  void runOnOperation() override {
    ModuleOp moduleOp = getOperation();
    auto *ctx = &getContext();
    Builder b(ctx);

    // 'accel' 타깃 속성 구성
    auto targetAttr = IREE::HAL::ExecutableTargetAttr::get(
        ctx,
        b.getStringAttr("accel"),
        b.getStringAttr("accel-bytecode-fb"),
        b.getDictionaryAttr({}));

    // hal.executable.variant 들에 정식 accessor로 설정
    moduleOp.walk([&](IREE::HAL::ExecutableVariantOp variantOp) {
      // 필요하면 기존 타깃 여부 체크 후 덮어쓰기/스킵 선택 가능
      // if (variantOp.getTargetAttr()) return;  // 이미 있으면 스킵하려면 이 줄 사용
      variantOp.setTargetAttr(targetAttr);
    });
  }
};

}  // namespace

std::unique_ptr<OperationPass<ModuleOp>> createAnnotateDispatchPass() {
  return std::make_unique<AnnotateDispatchPass>();
}

}  // namespace mlir::iree_compiler::IREE::Flow
