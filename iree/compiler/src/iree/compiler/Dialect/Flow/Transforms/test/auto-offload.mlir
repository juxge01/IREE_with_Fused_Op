// RUN: iree-opt %s -pass-pipeline='builtin.module(iree-flow-auto-offload{offload-threshold=1})' | FileCheck %s
module {
  func.func @hot() {
    %c0 = arith.constant 0 : i32
    %c1 = arith.constant 1 : i32
    %0 = arith.addi %c0, %c1 : i32
    return
  }
}
// CHECK: func.func @hot() attributes {{.*}}offload.target = "custom-accel"
