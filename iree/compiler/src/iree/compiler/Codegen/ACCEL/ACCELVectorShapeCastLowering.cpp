// Copyright 2024 The IREE Authors
//
// Licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "iree/compiler/Codegen/ACCEL/Passes.h"
#include "mlir/Dialect/Vector/Transforms/LoweringPatterns.h"
#include "mlir/Dialect/Vector/Transforms/VectorTransforms.h"
#include "mlir/Pass/Pass.h"
#include "mlir/Transforms/GreedyPatternRewriteDriver.h"

#define DEBUG_TYPE "iree-accel-vector-shape-cast-lowering"

namespace mlir::iree_compiler {

#define GEN_PASS_DEF_ACCELVECTORSHAPECASTLOWERINGPASS
#include "iree/compiler/Codegen/ACCEL/Passes.h.inc"

namespace {
class ACCELVectorShapeCastLoweringPass
    : public impl::ACCELVectorShapeCastLoweringPassBase<
          ACCELVectorShapeCastLoweringPass> {
public:
  using impl::ACCELVectorShapeCastLoweringPassBase<
      ACCELVectorShapeCastLoweringPass>::
      ACCELVectorShapeCastLoweringPassBase;

  void getDependentDialects(DialectRegistry &registry) const override {
    registry.insert<vector::VectorDialect>();
  }
  void runOnOperation() override;
};

void ACCELVectorShapeCastLoweringPass::runOnOperation() {
  MLIRContext *ctx = &getContext();
  auto funcOp = getOperation();

  RewritePatternSet patterns(ctx);
  vector::populateVectorShapeCastLoweringPatterns(patterns);
  (void)applyPatternsGreedily(funcOp, std::move(patterns));
}
} // namespace
} // namespace mlir::iree_compiler
