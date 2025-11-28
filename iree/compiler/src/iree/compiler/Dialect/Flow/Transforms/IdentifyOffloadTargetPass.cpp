// iree/compiler/src/iree/compiler/Dialect/Flow/Transforms/IdentifyOffloadTargetPass.cpp
#include "iree/compiler/Dialect/Flow/IR/FlowOps.h"
#include "iree/compiler/Dialect/Flow/Transforms/Passes.h"
#include "mlir/Dialect/Func/IR/FuncOps.h"
#include "mlir/IR/BuiltinOps.h"
#include "mlir/Pass/Pass.h"

namespace mlir::iree_compiler::IREE::Flow {

// TableGen이 생성할 Pass의 기본 구조를 포함합니다.
#define GEN_PASS_DEF_IDENTIFYOFFLOADTARGETPASS
#include "iree/compiler/Dialect/Flow/Transforms/Passes.h.inc"

struct IdentifyOffloadTargetPass
    : public impl::IdentifyOffloadTargetPassBase<IdentifyOffloadTargetPass> {
  void runOnOperation() override {
    mlir::ModuleOp moduleOp = getOperation();
    auto mainFunc = moduleOp.lookupSymbol<mlir::func::FuncOp>("main");
    if (!mainFunc) return;

    mainFunc.walk([&](IREE::Flow::DispatchRegionOp regionOp) {
      // 이 블록 안에 오프로드할 연산을 식별하는 로직을 추가합니다.
      // 예: linalg.conv_2d_nhwc_hwcf 연산이 포함된 경우 오프로드
      bool hasTargetOp = false;
      regionOp.walk([&](mlir::linalg::LinalgOp linalgOp) {
        // 여기에 원하는 연산(예: linalg.conv_2d_nhwc_hwcf)을 확인하는 코드를 추가합니다.
        // 지금은 모든 linalg 연산을 오프로드 대상으로 가정합니다.
        hasTargetOp = true;
        return WalkResult::interrupt();
      });

      if (hasTargetOp) {
        llvm::outs() << "[IdentifyOffloadTargetPass] Attaching 'offload.target' to a dispatch region.\n";
        // 'offload.target' 속성을 'sa16'으로 설정합니다.
        // 런타임 드라이버(accel_command_buffer.c)에서 이 값을 확인합니다.
        regionOp->setAttr("offload.target", StringAttr::get(&getContext(), "sa16"));
      }
    });
  }
};

std::unique_ptr<Pass> createIdentifyOffloadTargetPass() {
  return std::make_unique<IdentifyOffloadTargetPass>();
}

} // namespace mlir::iree_compiler::IREE::Flow
