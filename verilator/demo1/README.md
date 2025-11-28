mlir-opt offload_sa16.mlir --convert-to-llvm --convert-func-to-llvm --reconcile-unrealized-casts | mlir-translate -mlir-to-llvmir > offload_sa16.ll
