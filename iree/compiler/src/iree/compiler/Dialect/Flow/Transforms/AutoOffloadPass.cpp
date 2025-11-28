#include "iree/compiler/Dialect/Flow/Transforms/Passes.h"
#include "iree/compiler/Dialect/Flow/IR/FlowOps.h"

#include "mlir/Dialect/Arith/IR/Arith.h"
#include "mlir/Dialect/Func/IR/FuncOps.h"
#include "mlir/IR/BuiltinOps.h"

namespace mlir::iree_compiler::IREE::Flow {

#define GEN_PASS_DEF_AUTOOFFLOADPASS
#include "iree/compiler/Dialect/Flow/Transforms/Passes.h.inc"

struct AutoOffloadPass : public impl::AutoOffloadPassBase<AutoOffloadPass> {
  AutoOffloadPass() = default;
  // 임계치/덤프를 외부에서 넘겨줄 수 있는 생성자(클래스 내부라 protected 옵션 접근 OK)
  AutoOffloadPass(unsigned thr, bool dump) {
    this->offloadThreshold.setValue(thr);
    this->dumpArith.setValue(dump);
  }
  
  void runOnOperation() override {
    ModuleOp module = getOperation();
    const bool justDump = dumpArith;
    const unsigned thr = offloadThreshold;

    for (func::FuncOp fn : module.getOps<func::FuncOp>()) {
      if (fn.isExternal()) continue;

      unsigned arithCnt = 0;
      fn.walk([&](Operation *op) {
        if (auto *d = op->getDialect())
          if (d->getNamespace() == "arith") ++arithCnt;
      });

      if (justDump) {
        llvm::outs() << "[dump] " << fn.getName() << " : " << arithCnt << "\n";
        continue;
      }
      if (arithCnt < thr) continue;
      if (fn->hasAttr("offload.target")) continue;

      fn->setAttr("offload.target",
                  StringAttr::get(fn.getContext(), "custom-accel"));
    }
  }
};

std::unique_ptr<mlir::Pass> createAutoOffloadPass() {
  return std::make_unique<AutoOffloadPass>();
}

std::unique_ptr<mlir::Pass> createAutoOffloadPass(unsigned thr, bool dump) {
  return std::make_unique<AutoOffloadPass>(thr, dump);
}

} // namespace mlir::iree_compiler::IREE::Flow
