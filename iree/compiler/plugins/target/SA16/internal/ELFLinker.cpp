// Copyright 2021 The IREE Authors
//
// Licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "compiler/plugins/target/SA16/internal/ELFLinker.h"

#include "iree/compiler/Utils/ToolUtils.h"
#include "llvm/ADT/StringExtras.h"
#include "llvm/Support/FileUtilities.h"
#include "llvm/Support/Path.h"

namespace mlir::iree_compiler::IREE::HAL {

std::string ELFLinker::getSystemToolPath() const {
    // First check for setting the linker explicitly.
    auto toolPath = SA16LinkerTool::getSystemToolPath();
    if (!toolPath.empty())
      return toolPath;

    // No explicit linker specified, search the environment for common tools.
    // We want LLD, but will fall back to system `ld`.
    toolPath = findToolInEnvironment({"ld.lld", "ld"});
    if (!toolPath.empty())
      return toolPath;

    llvm::errs() << "No ELF linker tool found in environment; specify one with "
                    "--iree-sa16-system-linker-path=\n";
    return "";
}

std::optional<Artifacts>
ELFLinker::linkDynamicLibrary(StringRef libraryName,
                              ArrayRef<Artifact> objectFiles) {
  // Create a temporary directory to work in.
  llvm::SmallString<128> tempDir;
  if (std::error_code ec =
          llvm::sys::fs::createUniqueDirectory("iree-elf-linker", tempDir)) {
    llvm::errs() << "failed to create temporary directory: " << ec.message();
    return std::nullopt;
  }
  llvm::FileRemover tempDirRemover(tempDir);

  // Link into a shared object.
  // We do this in a temporary directory so that we don't have to worry about
  // cleaning up the intermediate object files.
  Artifacts artifacts;
  artifacts.libraryFile = Artifact::createTemporary(libraryName, ".so");

  SmallVector<std::string> flags = {
     getSystemToolPath(),
      "-shared",
      "-o",
      artifacts.libraryFile.path,
  };
  for (auto &objectFile : objectFiles) {
    flags.push_back(objectFile.path);
  }

  // Strip debug information (only, no relocations) when not requested.
  if (!targetOptions.target.debugSymbols) {
    flags.push_back("--strip-debug");
  }

  auto commandLine = llvm::join(flags, " ");
  if (failed(runLinkCommand(commandLine)))   
    return std::nullopt;
  return artifacts;
}

} // namespace mlir::iree_compiler::IREE::HAL


