// Copyright 2021 The IREE Authors
//
// Licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "compiler/plugins/target/SA16/SA16LinkerTool.h"

#include "compiler/plugins/target/SA16/internal/COFFLinker.h"
#include "compiler/plugins/target/SA16/internal/ELFLinker.h"
#include "compiler/plugins/target/SA16/internal/MachOLinker.h"
#include "compiler/plugins/target/SA16/internal/WasmLinker.h"

namespace mlir::iree_compiler::IREE::HAL {

// static
std::unique_ptr<SA16LinkerTool>
SA16LinkerTool::getForTarget(const llvm::Triple &targetTriple,
                             SA16TargetOptions &targetOptions) {
  if (targetTriple.isWasm()) {
    return std::make_unique<WasmLinker>(targetTriple, targetOptions);
  } else if (targetTriple.isOSBinFormatCOFF()) {
    return std::make_unique<COFFLinker>(targetTriple, targetOptions);
  } else if (targetTriple.isOSBinFormatMachO()) {
    return std::make_unique<MachOLinker>(targetTriple, targetOptions);
  } else if (targetTriple.isOSBinFormatELF()) {
    return std::make_unique<ELFLinker>(targetTriple, targetOptions);
  }
  return nullptr;
}

} // namespace mlir::iree_compiler::IREE::HAL
