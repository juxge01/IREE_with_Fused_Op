// Copyright 2022 The IREE Authors
//
// Licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "iree/compiler/Codegen/ACCEL/Passes.h"
#include "mlir/Dialect/Linalg/Utils/Utils.h"
#include "mlir/Pass/Pass.h"

namespace mlir::iree_compiler {

#define GEN_PASS_DEF_ACCELEMITVECTORIZATIONREMARKSPASS
#include "iree/compiler/Codegen/ACCEL/Passes.h.inc"

namespace {
struct ACCELEmitVectorizationRemarksPass
    : impl::ACCELEmitVectorizationRemarksPassBase<
          ACCELEmitVectorizationRemarksPass> {
  void runOnOperation() override;
};
} // namespace

void ACCELEmitVectorizationRemarksPass::runOnOperation() {
  auto funcOp = getOperation();
  bool dump = false;
  funcOp.walk([&](linalg::LinalgOp op) {
    op.emitWarning("op is not vectorized");
    dump = true;
  });
  if (dump) {
    funcOp.emitWarning("found one or more ops not vectorized");
  }
}
} // namespace mlir::iree_compiler
