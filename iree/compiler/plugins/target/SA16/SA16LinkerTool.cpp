// Copyright 2020 The IREE Authors
//
// Licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "compiler/plugins/target/SA16/SA16LinkerTool.h"

#include "iree/compiler/Utils/ToolUtils.h"
#include "llvm/Support/Process.h"

#define DEBUG_TYPE "iree-tools"

namespace mlir::iree_compiler::IREE::HAL {

std::string SA16LinkerTool::getSystemToolPath() const {
  // Always use the --iree-sa16-system-linker-path flag when specified as
  // it's explicitly telling us what to use.
  if (!targetOptions.systemLinkerPath.empty()) {
    return targetOptions.systemLinkerPath;
  }

  // Allow users to override the automatic search with an environment variable.
  char *linkerPath = std::getenv("IREE_SA16_SYSTEM_LINKER_PATH");
  if (linkerPath) {
    return std::string(linkerPath);
  }

  // Fallback to other searches as specified by the SA16LinkerTool implementation.
  return "";
}

LogicalResult SA16LinkerTool::runLinkCommand(std::string commandLine,
                                         StringRef env) {
  LLVM_DEBUG(llvm::dbgs() << "Running linker command:\n"
                          << env << " " << commandLine << "\n");
  if (!env.empty()) {
#if defined(_WIN32)
    commandLine = ("set " + env + " && " + commandLine).str();
#else
    commandLine = (env + " " + commandLine).str();
#endif // _WIN32
  } else {
    commandLine = escapeCommandLineComponent(commandLine);
  }
  int exitCode = system(commandLine.c_str());
  if (exitCode == 0)
    return success();
  llvm::errs() << "Linking failed; escaped command line returned exit code "
               << exitCode << ":\n\n"
               << commandLine << "\n\n";
  return failure();
}

} // namespace mlir::iree_compiler::IREE::HAL
