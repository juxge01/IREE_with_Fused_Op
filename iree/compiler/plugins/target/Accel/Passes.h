// Copyright 2025 The IREE Authors
//
// Licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#pragma once

#include "mlir/Pass/Pass.h"

namespace mlir::iree_compiler::IREE::HAL::Transforms {

// Replace LLVM add/fadd ops with custom RISC-V intrinsics.
std::unique_ptr<mlir::Pass> createConvertToCustomIntrinsic();

}  // namespace mlir::iree_compiler::IREE::HAL::Transforms
