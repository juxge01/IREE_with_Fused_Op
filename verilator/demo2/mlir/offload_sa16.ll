; ModuleID = 'LLVMDialectModule'
source_filename = "LLVMDialectModule"

define private void @sa16_load_B(ptr %0, ptr %1, i64 %2, i64 %3, i64 %4) {
  %6 = insertvalue { ptr, ptr, i64, [1 x i64], [1 x i64] } poison, ptr %0, 0
  %7 = insertvalue { ptr, ptr, i64, [1 x i64], [1 x i64] } %6, ptr %1, 1
  %8 = insertvalue { ptr, ptr, i64, [1 x i64], [1 x i64] } %7, i64 %2, 2
  %9 = insertvalue { ptr, ptr, i64, [1 x i64], [1 x i64] } %8, i64 %3, 3, 0
  %10 = insertvalue { ptr, ptr, i64, [1 x i64], [1 x i64] } %9, i64 %4, 4, 0
  %11 = alloca { ptr, ptr, i64, [1 x i64], [1 x i64] }, i64 1, align 8
  store { ptr, ptr, i64, [1 x i64], [1 x i64] } %10, ptr %11, align 8
  call void @_mlir_ciface_sa16_load_B(ptr %11)
  ret void
}

declare void @_mlir_ciface_sa16_load_B(ptr)

define private void @sa16_run_tile(ptr %0, ptr %1, i64 %2, i64 %3, i64 %4, ptr %5, ptr %6, i64 %7, i64 %8, i64 %9) {
  %11 = insertvalue { ptr, ptr, i64, [1 x i64], [1 x i64] } poison, ptr %0, 0
  %12 = insertvalue { ptr, ptr, i64, [1 x i64], [1 x i64] } %11, ptr %1, 1
  %13 = insertvalue { ptr, ptr, i64, [1 x i64], [1 x i64] } %12, i64 %2, 2
  %14 = insertvalue { ptr, ptr, i64, [1 x i64], [1 x i64] } %13, i64 %3, 3, 0
  %15 = insertvalue { ptr, ptr, i64, [1 x i64], [1 x i64] } %14, i64 %4, 4, 0
  %16 = alloca { ptr, ptr, i64, [1 x i64], [1 x i64] }, i64 1, align 8
  store { ptr, ptr, i64, [1 x i64], [1 x i64] } %15, ptr %16, align 8
  %17 = insertvalue { ptr, ptr, i64, [1 x i64], [1 x i64] } poison, ptr %5, 0
  %18 = insertvalue { ptr, ptr, i64, [1 x i64], [1 x i64] } %17, ptr %6, 1
  %19 = insertvalue { ptr, ptr, i64, [1 x i64], [1 x i64] } %18, i64 %7, 2
  %20 = insertvalue { ptr, ptr, i64, [1 x i64], [1 x i64] } %19, i64 %8, 3, 0
  %21 = insertvalue { ptr, ptr, i64, [1 x i64], [1 x i64] } %20, i64 %9, 4, 0
  %22 = alloca { ptr, ptr, i64, [1 x i64], [1 x i64] }, i64 1, align 8
  store { ptr, ptr, i64, [1 x i64], [1 x i64] } %21, ptr %22, align 8
  call void @_mlir_ciface_sa16_run_tile(ptr %16, ptr %22)
  ret void
}

declare void @_mlir_ciface_sa16_run_tile(ptr, ptr)

define void @offload_tile(ptr %0, ptr %1, i64 %2, i64 %3, i64 %4, ptr %5, ptr %6, i64 %7, i64 %8, i64 %9, ptr %10, ptr %11, i64 %12, i64 %13, i64 %14) {
  %16 = insertvalue { ptr, ptr, i64, [1 x i64], [1 x i64] } poison, ptr %10, 0
  %17 = insertvalue { ptr, ptr, i64, [1 x i64], [1 x i64] } %16, ptr %11, 1
  %18 = insertvalue { ptr, ptr, i64, [1 x i64], [1 x i64] } %17, i64 %12, 2
  %19 = insertvalue { ptr, ptr, i64, [1 x i64], [1 x i64] } %18, i64 %13, 3, 0
  %20 = insertvalue { ptr, ptr, i64, [1 x i64], [1 x i64] } %19, i64 %14, 4, 0
  %21 = insertvalue { ptr, ptr, i64, [1 x i64], [1 x i64] } poison, ptr %0, 0
  %22 = insertvalue { ptr, ptr, i64, [1 x i64], [1 x i64] } %21, ptr %1, 1
  %23 = insertvalue { ptr, ptr, i64, [1 x i64], [1 x i64] } %22, i64 %2, 2
  %24 = insertvalue { ptr, ptr, i64, [1 x i64], [1 x i64] } %23, i64 %3, 3, 0
  %25 = insertvalue { ptr, ptr, i64, [1 x i64], [1 x i64] } %24, i64 %4, 4, 0
  %26 = insertvalue { ptr, ptr, i64, [1 x i64], [1 x i64] } poison, ptr %5, 0
  %27 = insertvalue { ptr, ptr, i64, [1 x i64], [1 x i64] } %26, ptr %6, 1
  %28 = insertvalue { ptr, ptr, i64, [1 x i64], [1 x i64] } %27, i64 %7, 2
  %29 = insertvalue { ptr, ptr, i64, [1 x i64], [1 x i64] } %28, i64 %8, 3, 0
  %30 = insertvalue { ptr, ptr, i64, [1 x i64], [1 x i64] } %29, i64 %9, 4, 0
  %31 = extractvalue { ptr, ptr, i64, [1 x i64], [1 x i64] } %30, 0
  %32 = extractvalue { ptr, ptr, i64, [1 x i64], [1 x i64] } %30, 1
  %33 = extractvalue { ptr, ptr, i64, [1 x i64], [1 x i64] } %30, 2
  %34 = extractvalue { ptr, ptr, i64, [1 x i64], [1 x i64] } %30, 3, 0
  %35 = extractvalue { ptr, ptr, i64, [1 x i64], [1 x i64] } %30, 4, 0
  call void @sa16_load_B(ptr %31, ptr %32, i64 %33, i64 %34, i64 %35)
  %36 = extractvalue { ptr, ptr, i64, [1 x i64], [1 x i64] } %25, 0
  %37 = extractvalue { ptr, ptr, i64, [1 x i64], [1 x i64] } %25, 1
  %38 = extractvalue { ptr, ptr, i64, [1 x i64], [1 x i64] } %25, 2
  %39 = extractvalue { ptr, ptr, i64, [1 x i64], [1 x i64] } %25, 3, 0
  %40 = extractvalue { ptr, ptr, i64, [1 x i64], [1 x i64] } %25, 4, 0
  %41 = extractvalue { ptr, ptr, i64, [1 x i64], [1 x i64] } %20, 0
  %42 = extractvalue { ptr, ptr, i64, [1 x i64], [1 x i64] } %20, 1
  %43 = extractvalue { ptr, ptr, i64, [1 x i64], [1 x i64] } %20, 2
  %44 = extractvalue { ptr, ptr, i64, [1 x i64], [1 x i64] } %20, 3, 0
  %45 = extractvalue { ptr, ptr, i64, [1 x i64], [1 x i64] } %20, 4, 0
  call void @sa16_run_tile(ptr %36, ptr %37, i64 %38, i64 %39, i64 %40, ptr %41, ptr %42, i64 %43, i64 %44, i64 %45)
  ret void
}

define void @_mlir_ciface_offload_tile(ptr %0, ptr %1, ptr %2) {
  %4 = load { ptr, ptr, i64, [1 x i64], [1 x i64] }, ptr %0, align 8
  %5 = extractvalue { ptr, ptr, i64, [1 x i64], [1 x i64] } %4, 0
  %6 = extractvalue { ptr, ptr, i64, [1 x i64], [1 x i64] } %4, 1
  %7 = extractvalue { ptr, ptr, i64, [1 x i64], [1 x i64] } %4, 2
  %8 = extractvalue { ptr, ptr, i64, [1 x i64], [1 x i64] } %4, 3, 0
  %9 = extractvalue { ptr, ptr, i64, [1 x i64], [1 x i64] } %4, 4, 0
  %10 = load { ptr, ptr, i64, [1 x i64], [1 x i64] }, ptr %1, align 8
  %11 = extractvalue { ptr, ptr, i64, [1 x i64], [1 x i64] } %10, 0
  %12 = extractvalue { ptr, ptr, i64, [1 x i64], [1 x i64] } %10, 1
  %13 = extractvalue { ptr, ptr, i64, [1 x i64], [1 x i64] } %10, 2
  %14 = extractvalue { ptr, ptr, i64, [1 x i64], [1 x i64] } %10, 3, 0
  %15 = extractvalue { ptr, ptr, i64, [1 x i64], [1 x i64] } %10, 4, 0
  %16 = load { ptr, ptr, i64, [1 x i64], [1 x i64] }, ptr %2, align 8
  %17 = extractvalue { ptr, ptr, i64, [1 x i64], [1 x i64] } %16, 0
  %18 = extractvalue { ptr, ptr, i64, [1 x i64], [1 x i64] } %16, 1
  %19 = extractvalue { ptr, ptr, i64, [1 x i64], [1 x i64] } %16, 2
  %20 = extractvalue { ptr, ptr, i64, [1 x i64], [1 x i64] } %16, 3, 0
  %21 = extractvalue { ptr, ptr, i64, [1 x i64], [1 x i64] } %16, 4, 0
  call void @offload_tile(ptr %5, ptr %6, i64 %7, i64 %8, i64 %9, ptr %11, ptr %12, i64 %13, i64 %14, i64 %15, ptr %17, ptr %18, i64 %19, i64 %20, i64 %21)
  ret void
}

!llvm.module.flags = !{!0}

!0 = !{i32 2, !"Debug Info Version", i32 3}
