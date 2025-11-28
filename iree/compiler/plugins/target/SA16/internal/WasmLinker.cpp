// Copyright 2021 The IREE Authors
//
// Licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "compiler/plugins/target/SA16/internal/WasmLinker.h"

#include "iree/compiler/Utils/ToolUtils.h"
#include "llvm/ADT/StringExtras.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/Support/FormatVariadic.h"

namespace mlir::iree_compiler::IREE::HAL {

static std::string getWasmToolPath(const SA16TargetOptions &targetOptions) {
  // Always use the --iree-sa16-wasm-linker-path flag when specified as
  // it's explicitly telling us what to use.
  if (!targetOptions.wasmLinkerPath.empty()) {
    return targetOptions.wasmLinkerPath;
  }

  // Allow overriding the automatic search with an environment variable.
  char *linkerPath = std::getenv("IREE_SA16_WASM_LINKER_PATH");
  if (linkerPath) {
    return std::string(linkerPath);
  }

  // No explicit linker specified, search the environment (i.e. our own build
  // or install directories) for common tools.
  std::string toolPath = findToolFromExecutableDir(
      {"wasm-ld", "iree-lld", "lld", "ld.lld", "lld-link"});
  if (!toolPath.empty())
    return toolPath;

  llvm::errs() << "No Wasm linker tool specified or discovered\n";
  return "";
}

std::optional<Artifacts>
WasmLinker::linkDynamicLibrary(StringRef libraryName,
                               ArrayRef<Artifact> objectFiles) {
  Artifacts artifacts;

  // Create the wasm binary file name; if we only have a single input object
  // we can just reuse that.
  if (objectFiles.size() == 1) {
    artifacts.libraryFile =
        Artifact::createVariant(objectFiles.front().path, "wasm");
  } else {
    artifacts.libraryFile = Artifact::createTemporary(libraryName, "wasm");
  }
  artifacts.libraryFile.close();

  SmallVector<std::string, 8> flags = {
      getWasmToolPath(targetOptions),

      // Forces LLD to act like wasm ld and produce WebAssembly files.
      // If not specified then lld tries to figure out what it is by progname
      // (ld, ld64, link, etc).
      // NOTE: must be first because lld sniffs argv[1]/argv[2].
      "-flavor wasm",

      // entry symbol not defined (pass --no-entry to suppress): _start
      "--no-entry",

      // Generated a shared object containing position-independent-code.
      "--experimental-pic",
      "--shared",

      "-o " + artifacts.libraryFile.path,
  };

  // Link all input objects. Note that we are not linking whole-archive as we
  // want to allow dropping of unused codegen outputs.
  for (auto &objectFile : objectFiles) {
    flags.push_back(objectFile.path);
  }

  auto commandLine = llvm::join(flags, " ");
  if (failed(runLinkCommand(commandLine)))
    return std::nullopt;
  return artifacts;
}

} // namespace mlir::iree_compiler::IREE::HAL


