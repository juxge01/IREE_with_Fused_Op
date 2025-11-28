// Copyright 2020 The IREE Authors
//
// Licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef IREE_COMPILER_PLUGINS_TARGET_SA16_SA16IRPASSES_H_
#define IREE_COMPILER_PLUGINS_TARGET_SA16_SA16IRPASSES_H_

#include <memory>

#include "compiler/plugins/target/SA16/SA16TargetOptions.h"
#include "llvm/IR/Module.h"
#include "llvm/Target/TargetMachine.h"
#include "mlir/Support/LogicalResult.h"

namespace mlir::iree_compiler::IREE::HAL {

namespace SA16 {
// Creates and runs LLVMIR optimization passes defined in LLVMTargetOptions.
LogicalResult runLLVMIRPasses(const SA16Target &target,
                              llvm::TargetMachine *machine,
                              llvm::Module *module);

// Emits compiled module obj for the target machine.
LogicalResult runEmitObjFilePasses(llvm::TargetMachine *machine,
                                   llvm::Module *module,
                                   llvm::CodeGenFileType fileType,
                                   std::string *objData);
} // namespace SA16

} // namespace mlir::iree_compiler::IREE::HAL

#endif // IREE_COMPILER_PLUGINS_TARGET_SA16_SA16IRPASSES_H_
