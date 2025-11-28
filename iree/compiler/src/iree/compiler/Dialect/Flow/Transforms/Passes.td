// Copyright 2021 The IREE Authors
//
// Licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef IREE_DIALECT_FLOW_PASSES
#define IREE_DIALECT_FLOW_PASSES

include "mlir/Pass/PassBase.td"

// File organization:
// Groups passes that are related under one banner //===....===//. For any new
// pass add it to the relevant group and keep them alphabetical within a group.

//===---------------------------------------------------------------------===//
// General Flow passes
//===---------------------------------------------------------------------===//

def AnnotateDispatchesPass :
    Pass<"iree-flow-annotate-dispatches", "mlir::ModuleOp"> {
  let summary = "Annotates executable dispatches based on their contents.";
}

def CanonicalizePass :
    Pass<"iree-flow-canonicalize", ""> {
  let summary = "Flow specific canonicalization pass";
  let options = [
    Option<"cseConstants", "cse-constants", "bool",
            /*default=*/"true", "Do Not CSE constants on canonicalization">,
    Option<"testConvergence", "test-convergence", "bool",
           /*default=*/"false", "Fails if the patterns fail to converge">
  ];
}

def CaptureDynamicDimsPass :
    Pass<"iree-flow-capture-dynamic-dims", ""> {
  let summary = "Captures dynamic shape dimensions required by dispatch operands/results and control flow operations.";
  let dependentDialects = [
    "IREE::Flow::FlowDialect",
  ];
}

def CleanupTensorShapesPass :
    Pass<"iree-flow-cleanup-tensor-shapes", ""> {
  let summary = "Cleans up any remaining tensor shape metadata after lowering.";
}

def ConvertShardToFlowPass :
    Pass<"iree-convert-shard-to-flow", "mlir::ModuleOp"> {
  let summary = "Convert Shard dialect operations to flow.";
  let description = [{
    Each grid corresponds to a default !flow.channel with the same group name.
    ```
    shard.grid @grid_1(shape = 2x3)
    ```
    ```
    %channel = flow.channel.default "grid_1" : !flow.channel
    ```
    If there is only one grid in the program than the name is omitted and the
    ```
    %channel = flow.channel.default : !flow.channel
    ```

    Each (shard, grid_axes) pair partitions and orders the devices into disjoint
    groups, each corresponding to a !flow.channel to perform a collective
    operation.
    For example
    ```
    shard.grid @grid(shape = 2x3x4x5)
    ...
    %1 = shard.all_reduce on @grid grid_axes = [2, 0] : tensor<10x20xf32>
    ```
    For more information see
    [Shard dialect](https://mlir.llvm.org/docs/Dialects/Shard/#device-groups).

    The shard partition and device ordering determines the values for the
    `color` and `key` in the corresponding `flow.channel.split` operation used
    to create the channel.
    For more information on the meaning of `color` and `key` see
    [MPI_Comm_split](https://www.mpi-forum.org/docs/mpi-4.1/mpi41-report/node188.htm#Node188)
    in the MPI standard.

    Each !flow.channel is wrapped in an IREE `util.global` and its construction
    is done only once with `util.initializer`.
  }];
  let dependentDialects = [
    "mlir::affine::AffineDialect",
    "mlir::linalg::LinalgDialect",
    "mlir::shard::ShardDialect",
    "mlir::tensor::TensorDialect",
    "IREE::Flow::FlowDialect",
  ];
}

def ConvertToFlowPass :
    Pass<"iree-flow-convert-to-flow", ""> {
  let summary = "Convert operations to flow. Currently just a test pass.";
  let description = [{
    Convert operations to `flow` ops. This pass is currently only used for
    testing, since the conversion to Flow ops happens within dispatch region
    formation.
  }];
  let dependentDialects = [
    "mlir::affine::AffineDialect",
    "mlir::linalg::LinalgDialect",
    "mlir::scf::SCFDialect",
    "mlir::tensor::TensorDialect",
    "IREE::Flow::FlowDialect",
  ];
}

def DeduplicateExecutablesPass :
    Pass<"iree-flow-deduplicate-executables", "mlir::ModuleOp"> {
  let summary = "Deduplicates executables that are identical.";
}

def DumpDispatchGraphPass : Pass<"iree-flow-dump-dispatch-graph-pass"> {
  let summary = "Dump visualization of dispatches within the program.";
  let options = [
    Option<
      "outputFile", "output-file",
      "std::string",
      /*default=*/"std::string()",
      "File path to write to; or `` for stderr or `-` for stdout."
    >,
    Option<"maxLabelLen", "max-label-len", "unsigned",
            /*default=*/"20", "Limit attribute/type length to number of chars">,
    Option<"printAttrs", "print-attrs", "bool",
           /*default=*/"true", "Print attributes of operations">,
    Option<"printControlFlowEdges", "print-control-flow-edges", "bool",
           /*default=*/"false", "Print control flow edges">,
    Option<"printDataFlowEdges", "print-data-flow-edges", "bool",
           /*default=*/"true", "Print data flow edges">,
    Option<"printResultTypes", "print-result-types", "bool",
            /*default=*/"true", "Print result types of operations">,
    Option<"emitDispatchBody", "emit-dispatch-body", "bool",
            /*default=*/"false", "Emit dispatch body in label">,
    Option<"emitInitializers", "emit-initializers", "bool",
            /*default=*/"false", "Emit initializers">
  ];
}

def ExportBenchmarkFuncsPass :
    Pass<"iree-flow-export-benchmark-funcs-pass", "mlir::ModuleOp"> {
  let summary = "Exports benchmark functions.";
  let description = [{
    Exports all functions and dispatch executables as `() -> ()` benchmark funcs.
  }];
  let dependentDialects = [
    "mlir::arith::ArithDialect",
    "IREE::Flow::FlowDialect",
    "IREE::HAL::HALDialect",
    "IREE::Util::UtilDialect",
  ];
}

def InitializeEmptyTensorsPass :
    Pass<"iree-flow-initialize-empty-tensors", ""> {
  let summary = "Initialize empty tensors.";
  let options = [
    Option<"zeroFill", "zero-fill", "bool", /*default=*/"false",
           "Fills empty tensors with zeros.">,
  ];
  let description = [{
    Initialize all empty tensors after dispatch formation to zero or
    uninitialized allocations.
  }];
  let dependentDialects = [
    "mlir::arith::ArithDialect",
    "mlir::linalg::LinalgDialect",
    "IREE::Flow::FlowDialect",
  ];
}

def InjectDispatchTracingPass :
    InterfacePass<"iree-flow-inject-dispatch-tracing", "mlir::FunctionOpInterface"> {
  let summary = "Injects tracing markers for dispatch operation tensor inputs and outputs.";
}

def InjectTensorTracingPass :
    InterfacePass<"iree-flow-inject-tensor-tracing", "mlir::FunctionOpInterface"> {
  let summary = "Injects tensor tracing on ops annotated with `iree.tensor.trace`.";
  let dependentDialects = [
    "mlir::arith::ArithDialect",
    "mlir::tensor::TensorDialect",
    "IREE::Flow::FlowDialect",
  ];
}

def InsertDebugTargetAtSymbolPass :
    Pass<"iree-flow-insert-debug-target-at-symbol", "mlir::ModuleOp"> {
  let summary = "Crops and/or traces the program at the specified symbol.";
  let options = [
    Option<"breakDebugTarget", "break-debug-target", "std::string",
           /*default=*/"",
           "Symbol at which to insert a break in the program.">,
    Option<"traceDebugTarget", "trace-debug-target", "std::string",
           /*default=*/"",
           "Symbol to insert iree.flow.trace ops around.">
  ];
  let dependentDialects = [
    "IREE::Flow::FlowDialect",
    "IREE::HAL::HALDialect",
    "IREE::Util::UtilDialect",
  ];
}

def InsertDebugTargetAtOrdinalPass :
    Pass<"iree-flow-insert-debug-target-at-ordinal", "mlir::ModuleOp"> {
  let summary = "Crops and/or traces the program at the specified ordinal.";
  let options = [
    Option<"breakDebugTarget", "break-debug-target", "std::string",
           /*default=*/"",
           "Ordinal at which to insert a break in the program.">,
    Option<"traceDebugTarget", "trace-debug-target", "std::string",
           /*default=*/"",
           "Ordinal to insert iree.flow.trace ops around.">
  ];
  let dependentDialects = [
    "IREE::Flow::FlowDialect",
    "IREE::HAL::HALDialect",
  ];
}

def OutlineConstantsPass :
    Pass<"iree-flow-outline-constants", "mlir::ModuleOp"> {
  let summary = "Outlines tensor constants into util.globals at the module level.";
  let description = [{
    Outlines tensor constants throughout the program into globals initialized
    with stream ops.
  }];
  let dependentDialects = [
    "mlir::arith::ArithDialect",
    "IREE::Flow::FlowDialect",
    "IREE::Util::UtilDialect",
  ];
}

def OutlineDispatchExternsPass :
    Pass<"iree-flow-outline-dispatch-externs", "mlir::ModuleOp"> {
  let summary = "Outlines external dispatches into executables.";
  let dependentDialects = [
    "IREE::Flow::FlowDialect",
    "IREE::HAL::HALDialect",
  ];
}

def OutlineDispatchRegionsPass :
    Pass<"iree-flow-outline-dispatch-regions", "mlir::ModuleOp"> {
  let summary = "Outlines dispatch regions into executables.";
  let dependentDialects = [
    "mlir::func::FuncDialect",
    "IREE::Flow::FlowDialect",
  ];
}

def TopLevelSCFToCFGPass :
    InterfacePass<"iree-top-level-scf-to-cfg", "mlir::FunctionOpInterface"> {
  let summary = "Converts non-nested SCF constructs to CFG (not traversing into opaque operations).";
}

def VerifyInputLegalityPass: Pass<"iree-verify-input-legality", ""> {
  let summary = "Checks the legality of the IR at the start of IREE flow transformation pipeline.";
  let description = [{
    Verifies that the input to the Flow transformation pipeline is legal.
    This includes checking for operations from dialects that are expected
    to be legalized before this pass.
  }];
}

def AutoOffloadPass
    : Pass<"iree-flow-auto-offload", "mlir::ModuleOp"> {
  let summary = "Attach offload.target to hot funcs";
  let description = [{
    Counts arith ops in each func and, if it exceeds a threshold, attaches offload.target.
  }];
  let constructor = "::mlir::iree_compiler::IREE::Flow::createAutoOffloadPass()";
  let options = [
    Option<"offloadThreshold", "offload-threshold", "unsigned", "50",
           "Attach offload if arith count >= threshold">,
    Option<"dumpArith", "dump-arith", "bool", "false",
           "Print per-function arith counts instead of attaching attribute">
  ];
}

def WrapOffloadFunctionsPass : Pass<"iree-flow-wrap-offload-functions", "ModuleOp"> {
  let summary = "Wrap functions with offload.target attribute into a flow.dispatch.region";
  let constructor = "mlir::iree_compiler::IREE::Flow::createWrapOffloadFunctionsPass()";
}

def AutoFindSplitPointPass : Pass<"iree-flow-auto-find-split-point", "ModuleOp"> {
  let summary = "Finds a split point for partitioning based on a simple cost model.";
  let constructor = "mlir::iree_compiler::IREE::Flow::createAutoFindSplitPointPass()";

  let dependentDialects = [
    "mlir::func::FuncDialect",
    "mlir::arith::ArithDialect"
  ];
}

def AutoFusionClusterPass : Pass<"iree-flow-auto-fusion-cluster", "ModuleOp"> {
  let summary = "Finds a split point for partitioning based on a simple cost model.";
  let constructor = "mlir::iree_compiler::IREE::Flow::createAutoFusionClusterPass()";

  let dependentDialects = [
    "mlir::func::FuncDialect",
    "mlir::arith::ArithDialect"
  ];
}

// Build fusion units around offload anchors.
def FusionUnitBuilderPass
    : Pass<"iree-flow-fusion-unit-builder", "mlir::func::FuncOp"> {
  let summary = "Build fusion clusters (fusion units) around `offload.anchor` ops";
  let description = [{
    For each operation tagged with `offload.anchor`, this pass gathers simple
    producer/consumer elementwise neighbors into a cluster and tags them with
    `cluster.id = "fu_X"`.
  }];
  let constructor = "::mlir::iree_compiler::IREE::Flow::createFusionUnitBuilderPass()";
  let dependentDialects = [
    "mlir::func::FuncDialect",
    "mlir::linalg::LinalgDialect",
    "mlir::tensor::TensorDialect",
    "IREE::Flow::FlowDialect",
  ];
}

// Wrap clustered ops into a flow.dispatch.region.
def ClusterToDispatchPass
    : Pass<"iree-flow-cluster-to-dispatch", "mlir::func::FuncOp"> {
  let summary = "Convert ops with the same `cluster.id` into a flow.dispatch.region";
  let description = [{
    Groups operations by `cluster.id`, topologically sorts them, and forms a
    `flow.dispatch.region` per cluster. Propagates `offload.target` from the
    containing function and attaches a `dispatch.debug.flops.total` attribute.
  }];
  let constructor = "::mlir::iree_compiler::IREE::Flow::createClusterToDispatchPass()";
  let dependentDialects = [
    "mlir::func::FuncDialect",
    "IREE::Flow::FlowDialect",
  ];
}

def CalculateDispatchDataTransferPass
  : Pass<"iree-flow-calc-dispatch-data-transfer", "func::FuncOp"> {
  let summary = "Annotate DispatchOp with H2D/D2H byte counts";
  let constructor = "mlir::iree_compiler::IREE::Flow::createCalculateDispatchDataTransferPass()";
  let dependentDialects = ["tensor::TensorDialect", "func::FuncDialect"];
}

// def IntelligentFusionPlannerPass
//   : Pass<"iree-flow-intelligent-fusion-planner", "ModuleOp"> {
//   let summary = "Intelligent fusion/offload planner by FLOPs";
//   let constructor =
//     "mlir::iree_compiler::IREE::Flow::createIntelligentFusionPlannerPass()";

//   let dependentDialects = [
//     "mlir::func::FuncDialect",
//     "IREE::Flow::FlowDialect",
//     "mlir::arith::ArithDialect",
//     "mlir::linalg::LinalgDialect",
//     "mlir::tensor::TensorDialect",
//     "mlir::math::MathDialect",
//   ];
// }

def IntelligentFusionPlannerPass
    : Pass<"iree-flow-intelligent-fusion-planner", "mlir::ModuleOp"> {
  let summary = "Intelligent fusion/offload planner by FLOPs";
  let description = [{
    This pass analyzes functions and attaches `offload.target` attributes to
    the top K% functions based on their arithmetic operation counts.
  }];
  let constructor = "::mlir::iree_compiler::IREE::Flow::createIntelligentFusionPlannerPass()";
  let options = [
    Option<"topKRatio", "top-k-ratio", "double", "0.1",
          "attach offload to top K% functions based on arith count">
  ];

    let dependentDialects = [
    "mlir::func::FuncDialect",
    "IREE::Flow::FlowDialect",
    "mlir::arith::ArithDialect",
    "mlir::linalg::LinalgDialect",
    "mlir::tensor::TensorDialect",
    "mlir::math::MathDialect",
  ];
}

def FlowTileFusedOpsPass : Pass<"iree-flow-tile-fused-ops", "mlir::func::FuncOp"> {
  let summary = "Tiles Linalg ops inside a dispatch region.";

  let constructor = "mlir::iree_compiler::IREE::Flow::createFlowTileFusedOpsPass()";

  let dependentDialects = [
    "mlir::linalg::LinalgDialect",
    "mlir::scf::SCFDialect"
  ];
}

def MLIRFLOPsAnalysisPass : Pass<"iree-flow-analyze-flops", "ModuleOp"> {
  let summary = "Analyzes Linalg ops to estimate and attach FLOPs as an attribute.";

  let constructor = "mlir::iree_compiler::IREE::Flow::createMLIRFLOPsAnalysisPass()";

  let dependentDialects = [
    "mlir::linalg::LinalgDialect"
  ];
}

def AnnotateDispatchPass : Pass<"iree-flow-annotate-dispatch", "ModuleOp"> {
  let summary = "Annotates flow.dispatch regions with target backend information.";
  let description = [{
    This pass analyzes dispatch regions and attaches target backend attributes
    (e.g., `hal.target = "accel"`) based on offloading criteria.
  }];
  let constructor = "mlir::iree_compiler::IREE::Flow::createAnnotateDispatchPass()";
  let dependentDialects = [
    "mlir::iree_compiler::IREE::Flow::FlowDialect"
  ];
}

def IdentifyOffloadTargetPass : Pass<"iree-sa16-identify-offload-target", "ModuleOp"> {
  let summary = "Identifies dispatch regions to offload to the SA16 accelerator";
  let constructor = "mlir::iree_compiler::IREE::Flow::createIdentifyOffloadTargetPass()";
  let dependentDialects = [
    "mlir::iree_compiler::IREE::Flow::FlowDialect"
  ];
}

def ProfilingInstrumentPass : Pass<"iree-flow-profiling-instrument", "ModuleOp"> {
  let summary = "Instruments Linalg ops with runtime timer calls for profiling.";
  let constructor = "mlir::iree_compiler::IREE::Flow::createProfilingInstrumentPass()";
}

#endif  // IREE_DIALECT_FLOW_PASSES
