// Copyright 2022 The IREE Authors
//
// Licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef IREE_COMPILER_CODEGEN_ACCEL_TRANSFORMEXTENSIONS_ACCELEXTENSIONS_H_
#define IREE_COMPILER_CODEGEN_ACCEL_TRANSFORMEXTENSIONS_ACCELEXTENSIONS_H_

#include "mlir/Dialect/Transform/IR/TransformDialect.h"

#define GET_OP_CLASSES
#include "iree/compiler/Codegen/ACCEL/TransformExtensions/ACCELExtensionsOps.h.inc"

namespace mlir {
class DialectRegistry;
} // namespace mlir

namespace mlir::iree_compiler {

/// Registers ACCEL transformations that require IREE-specific information
/// into the transform dialect.
void registerTransformDialectACCELExtension(DialectRegistry &registry);

namespace IREE ::transform_dialect {
// Hook to register ACCEL transformations to the transform dialect.
class ACCELExtensions
    : public transform::TransformDialectExtension<ACCELExtensions> {
public:
  ACCELExtensions();
};
} // namespace IREE::transform_dialect

} // namespace mlir::iree_compiler

#endif // IREE_COMPILER_CODEGEN_ACCEL_TRANSFORMEXTENSIONS_ACCELEXTENSIONS_H_
