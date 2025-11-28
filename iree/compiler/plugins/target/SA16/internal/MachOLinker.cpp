// Copyright 2021 The IREE Authors
//
// Licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "compiler/plugins/target/SA16/internal/MachOLinker.h"

#include "iree/compiler/Utils/ToolUtils.h"
#include "llvm/ADT/StringExtras.h"
#include "llvm/Support/Path.h"

namespace mlir::iree_compiler::IREE::HAL {

std::string MachOLinker::getSystemToolPath() const {
  // First check for setting the linker explicitly.
  auto toolPath = SA16LinkerTool::getSystemToolPath();
  if (!toolPath.empty())
    return toolPath;

  // No explicit linker specified, search the environment for common tools.
  // On macOS, the standard system linker is `ld`, and it's
  // unconditionally what we want to use.
  toolPath = findToolInEnvironment({"ld"});
  if (!toolPath.empty())
    return toolPath;

  llvm::errs() << "No Mach-O linker tool found in environment.\n";
  return "";
}

std::optional<Artifacts>
MachOLinker::linkDynamicLibrary(StringRef libraryName,
                                ArrayRef<Artifact> objectFiles) {
  Artifacts artifacts;

  // Create the shared object name; if we only have a single input object we
  // can just reuse that.
  if (objectFiles.size() == 1) {
    artifacts.libraryFile =
        Artifact::createVariant(objectFiles.front().path, "dylib");
  } else {
    artifacts.libraryFile = Artifact::createTemporary(libraryName, "dylib");
  }
  artifacts.libraryFile.close();

  SmallVector<std::string, 8> flags = {
      getSystemToolPath(),
      "-o " + artifacts.libraryFile.path,
      "-dylib",
      "-flat_namespace",
      "-L/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/lib",
      "-lSystem",
  };

  // Link all input objects.
  for (auto &objectFile : objectFiles) {
    flags.push_back(objectFile.path);
  }

  auto commandLine = llvm::join(flags, " ");
  if (failed(runLinkCommand(commandLine)))
    return std::nullopt;
  return artifacts;
}

} // namespace mlir::iree_compiler::IREE::HAL


