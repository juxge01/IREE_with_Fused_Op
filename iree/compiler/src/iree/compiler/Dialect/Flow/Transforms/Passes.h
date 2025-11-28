// Copyright 2019 The IREE Authors
//
// Licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef IREE_COMPILER_DIALECT_FLOW_TRANSFORMS_PASSES_H_
#define IREE_COMPILER_DIALECT_FLOW_TRANSFORMS_PASSES_H_

#include <functional>

#include "iree/compiler/Dialect/Flow/IR/FlowOps.h"
#include "llvm/ADT/StringMap.h"
#include "mlir/IR/BuiltinOps.h"
#include "mlir/Pass/Pass.h"
#include "mlir/Pass/PassManager.h"
#include "mlir/Support/LLVM.h"
#include "mlir/Pass/PassOptions.h"  

namespace mlir::iree_compiler::IREE::Flow {

//===----------------------------------------------------------------------===//
// Pipelines
//===----------------------------------------------------------------------===//

/// This is a placeholder for future. We should pass all the options through the
/// struct.
// struct TransformOptions : public PassPipelineOptions<TransformOptions> {};
struct TransformOptions : public PassPipelineOptions<TransformOptions> {
  Option<bool> enableAutoOffload{
      *this, "flow-enable-auto-offload",
      llvm::cl::init(false),
      llvm::cl::desc("Enable AutoOffload pass")};

  Option<unsigned> flowAutoOffloadThreshold{
      *this, "flow-auto-offload-threshold",
      llvm::cl::init(50),
      llvm::cl::desc("Threshold for arith op count")};

  Option<bool> flowAutoOffloadDumpArith{
      *this, "flow-auto-offload-dump-arith",
      llvm::cl::init(false),
      llvm::cl::desc("Dump arith counts instead of attaching attribute")};

  Option<double> topKRatio{
      *this, "flow-top-k-ratio",
      llvm::cl::desc("Selects the top K ratio of functions for offloading "
                     "based on FLOPs for custom fusion."),
      llvm::cl::init(0.1)};
};

// Adds a set of passes to the given pass manager that run the required flow
// transforms in the canonical order.
//
// Most translation code should prefer to use this instead of manually adding
// the passes themselves to ensure that expected pass ordering is observed.
//
// The expected usage is:
//   Input legalization by one of:
//     - Directly passing supported flow plus core ops
//   buildFlowTransformPassPipeline
//   <run conversion from flow to sequencer/hal/vm/etc>
void buildFlowTransformPassPipeline(OpPassManager &passManager,
                                    const TransformOptions &transformOptions);

void buildIreeFlowCustomFusionPassPipeline(
     OpPassManager &pm, const TransformOptions &transformOptions);

void registerFlowTransformPassPipeline();

//===----------------------------------------------------------------------===//
// Register all Passes
//===----------------------------------------------------------------------===//

#define GEN_PASS_DECL
#include "iree/compiler/Dialect/Flow/Transforms/Passes.h.inc" // IWYU pragma: keep

std::unique_ptr<mlir::Pass> createAutoOffloadPass();
std::unique_ptr<mlir::Pass> createAutoOffloadPass(unsigned threshold, bool dump);

std::unique_ptr<Pass> createWrapOffloadFunctionsPass();

#define GEN_PASS_DECL_AUTOFINDSPLITPOINTPASS
std::unique_ptr<Pass> createAutoFindSplitPointPass();

#define GEN_PASS_DECL_AUTOFUSIONCLUSTERPASS
std::unique_ptr<Pass> createAutoFusionClusterPass();

#define GEN_PASS_DECL_FUSIONUNITBUILDERPASS
std::unique_ptr<Pass> createFusionUnitBuilderPass();

#define GEN_PASS_DECL_PROFILINGINSTRUMENTPASS
std::unique_ptr<Pass> createProfilingInstrumentPass();

#define GEN_PASS_DECL_CLUSTERTODISPATCHPASS
std::unique_ptr<Pass> createClusterToDispatchPass();

#define GEN_PASS_DECL_CALCULATEDISPATCHDATATRANSFERPASS
std::unique_ptr<Pass> createCalculateDispatchDataTransferPass();

#define GEN_PASS_DECL_MLIRFLOPSANALYSISPASS
std::unique_ptr<Pass> createMLIRFLOPsAnalysisPass();

#define GEN_PASS_DECL_FLOWTILEFUSEDOPSPASS
std::unique_ptr<Pass> createFlowTileFusedOpsPass();

#define GEN_PASS_DECL_IDENTIFYOFFLOADTARGETPASS
std::unique_ptr<Pass> createIdentifyOffloadTargetPass();

#define GEN_PASS_DECL_ANNOTATEDISPATCHPASS
std::unique_ptr<OperationPass<ModuleOp>> createAnnotateDispatchPass();

// IntelligentFusionPlannerPass factories
std::unique_ptr<mlir::Pass> createIntelligentFusionPlannerPass();          // no-arg (TD constructor가 이걸 씀)
std::unique_ptr<mlir::Pass> createIntelligentFusionPlannerPass(double ratio); // 편의 오버로드

void registerFlowPasses();

} // namespace mlir::iree_compiler::IREE::Flow

#endif // IREE_COMPILER_DIALECT_FLOW_TRANSFORMS_PASSES_H_
