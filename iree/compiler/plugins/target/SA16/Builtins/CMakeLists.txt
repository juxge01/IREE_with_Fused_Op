# Copyright 2021 The IREE Authors
#
# Licensed under the Apache License v2.0 with LLVM Exceptions.
# See https://llvm.org/LICENSE.txt for license information.
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

iree_cc_library(
  NAME
    Builtins
  SRCS
    "Device.cpp"
    "Musl.cpp"
    "UKernel.cpp"
  HDRS
    "Device.h"
    "Musl.h"
    "UKernel.h"
  DEPS
    # Note: circular dependency is ok here.
    iree::compiler::plugins::target::SA16::SA16_hdrs
    iree::compiler::Dialect::HAL::IR
    iree::compiler::Codegen::Utils
    iree::compiler::Utils
    LLVMBitReader
    LLVMCore
    LLVMSupport
    LLVMTarget
    MLIRIR
  PUBLIC
)