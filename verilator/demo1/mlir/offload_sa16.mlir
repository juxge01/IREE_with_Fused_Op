module {
  // 런타임 C가 제공할 extern (C ABI wrapper를 위해 emit_c_interface)
  func.func private @sa16_load_B(%B: memref<64xi8>) attributes { llvm.emit_c_interface }
  func.func private @sa16_run_tile(%A: memref<16xi8>, %Out: memref<64xi32>) attributes { llvm.emit_c_interface }

  // 한 타일 오프로딩: B 64B 한 번 로드 → A 16B 타일 실행 → OUT 64워드 채워짐
  func.func @offload_tile(%A: memref<16xi8>, %B: memref<64xi8>, %Out: memref<64xi32>)
             attributes { llvm.emit_c_interface } {
    call @sa16_load_B(%B) : (memref<64xi8>) -> ()
    call @sa16_run_tile(%A, %Out) : (memref<16xi8>, memref<64xi32>) -> ()
    return
  }
}

