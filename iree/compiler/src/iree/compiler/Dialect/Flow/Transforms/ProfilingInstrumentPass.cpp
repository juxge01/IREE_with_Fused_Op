// Copyright 2024 The IREE Authors
//
// Licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "iree/compiler/Dialect/Flow/Transforms/Passes.h"
#include "iree/compiler/Dialect/Util/IR/UtilDialect.h"
#include "iree/compiler/Dialect/Util/IR/UtilOps.h"
#include "iree/compiler/Dialect/Util/IR/UtilTypes.h" 
#include "mlir/Dialect/Func/IR/FuncOps.h"
#include "mlir/Dialect/Linalg/IR/Linalg.h"
#include "mlir/IR/SymbolTable.h"
#include "mlir/IR/PatternMatch.h"
#include "mlir/Pass/Pass.h"

#include <utility>

#include "iree/compiler/Dialect/Flow/IR/FlowOps.h"
#include "iree/compiler/Dialect/HAL/IR/HALDialect.h"
#include "iree/compiler/Dialect/HAL/IR/HALOps.h"
#include "llvm/ADT/StringExtras.h"
#include "llvm/Support/Regex.h"
#include "mlir/IR/Builders.h"
#include "mlir/IR/BuiltinTypes.h"
#include "mlir/IR/Diagnostics.h"
#include "mlir/Interfaces/FunctionInterfaces.h" // [수정] FunctionOpInterface를 위해 추가

#include "llvm/Support/Debug.h" // llvm::dbgs()
#include "mlir/Dialect/Arith/IR/Arith.h"

// C++ std::string 사용을 위해 추가
#include <string> 

#define DEBUG_TYPE "iree-flow-profiling-instrument"

namespace mlir::iree_compiler::IREE::Flow {

// .td 파일에서 생성된 Pass Definition을 포함합니다.
#define GEN_PASS_DEF_PROFILINGINSTRUMENTPASS
#include "iree/compiler/Dialect/Flow/Transforms/Passes.h.inc"

namespace {

/// 런타임에 호출할 타이머 함수 선언을 모듈에 삽입하거나 가져옵니다.
static void getOrInsertTimerFunc(ModuleOp &module, OpBuilder &builder,
                                 StringRef funcName) {
  if (module.lookupSymbol(funcName)) return;

  auto *context = module.getContext();
  auto idType = builder.getI32Type(); // i32 타입 사용
  auto funcType = FunctionType::get(context, {idType}, {});

  // Create the FuncOp and insert it into the module symbol table. Using
  // SymbolTable::insert is more robust than relying on the builder insertion
  // point in some pass ordering scenarios.
  auto func = func::FuncOp::create(module.getLoc(), funcName, funcType);
  func.setPrivate();
  SymbolTable symbolTable(module);
  symbolTable.insert(func);
}

/// 연산자 이름을 나타내는 전역 문자열 상수를 만들고 그 주소를 반환합니다.
struct ProfilingInstrumentPass
    : public impl::ProfilingInstrumentPassBase<ProfilingInstrumentPass> {

  void getDependentDialects(DialectRegistry &registry) const override {
    // [추가] Arith Dialect 의존성 추가
    registry.insert<arith::ArithDialect, func::FuncDialect, 
                    linalg::LinalgDialect, IREE::Util::UtilDialect, 
                    IREE::HAL::HALDialect>();
  }

  void runOnOperation() override {
    ModuleOp module = getOperation();

    llvm::dbgs() << "--- [DEBUG] Running ProfilingInstrumentPass ---\n";
    
    OpBuilder builder(module.getContext());
    int opCounter = 0; 

    // 1. 모듈에 런타임 함수 선언 (i32 버전)이 있는지 확인/삽입
    getOrInsertTimerFunc(module, builder, "runtime_timer_start");
    getOrInsertTimerFunc(module, builder, "runtime_timer_end");

    // 2. 모듈 내의 모든 함수를 순회
    for (auto funcOp : module.getOps<FunctionOpInterface>()) {
      if (funcOp.isExternal()) continue;

      SmallVector<Operation *> opsToInstrument;
      funcOp.walk([&](Operation *op) {
        if (isa<linalg::LinalgOp>(op)) {
          opsToInstrument.push_back(op);
        }
      });

      llvm::dbgs() << "  [DEBUG] Instrumenting function: " << funcOp.getName() << "\n";
      llvm::dbgs() << "    [DEBUG] Found " << opsToInstrument.size() 
                   << " linalg ops to instrument.\n";

      if (opsToInstrument.empty()) continue; 

      IRRewriter rewriter(funcOp.getContext());

      for (Operation *op : opsToInstrument) {
        // 5. [수정] 고유 ID를 생성 (디버그 출력용 이름은 유지)
        int currentOpId = opCounter++;
        std::string opName = op->getName().getStringRef().str() + 
                             "_" + std::to_string(currentOpId);
        
        llvm::dbgs() << "      [DEBUG] Instrumenting: " << opName 
                     << " (ID: " << currentOpId << ")\n"; // ID 로그 추가

        // 6. [수정] 전역 문자열 대신 정수 ID (arith.constant)를 생성
        rewriter.setInsertionPoint(op);
        Value idValue = rewriter.create<arith::ConstantOp>(
            op->getLoc(), rewriter.getI32IntegerAttr(currentOpId));
        
        // 7. 연산 *이전*에 @runtime_timer_start(%id) 호출 삽입
        rewriter.create<func::CallOp>(op->getLoc(), "runtime_timer_start", 
                                      TypeRange{}, ValueRange{idValue});

        // 8. 연산 *이후*에 @runtime_timer_end(%id) 호출 삽입
        rewriter.setInsertionPointAfter(op);
        rewriter.create<func::CallOp>(op->getLoc(), "runtime_timer_end", 
                                      TypeRange{}, ValueRange{idValue});
      }
    };
  }
};

} // namespace

/// Pass 공장 함수
std::unique_ptr<Pass> createProfilingInstrumentPass() {
  return std::make_unique<ProfilingInstrumentPass>();
}

} // namespace mlir::iree_compiler::IREE::Flow