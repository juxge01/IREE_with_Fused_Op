// Copyright 2021 The IREE Authors
//
// Licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef IREE_COMPILER_PLUGINS_TARGET_SA16_INTERNAL_ELFLINKER_H_
#define IREE_COMPILER_PLUGINS_TARGET_SA16_INTERNAL_ELFLINKER_H_

#include "compiler/plugins/target/SA16/SA16LinkerTool.h"

namespace mlir::iree_compiler::IREE::HAL {

// An ELF linker tool.
class ELFLinker final : public SA16LinkerTool {
public:
  using SA16LinkerTool::SA16LinkerTool;

  std::string getSystemToolPath() const override;

  std::optional<Artifacts>
  linkDynamicLibrary(StringRef libraryName,
                     ArrayRef<Artifact> objectFiles) override;
};

} // namespace mlir::iree_compiler::IREE::HAL

#endif // IREE_COMPILER_PLUGINS_TARGET_SA16_INTERNAL_ELFLINKER_H_


