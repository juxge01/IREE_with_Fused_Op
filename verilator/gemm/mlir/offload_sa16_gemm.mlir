// mlir/offload_sa16_gemm.mlir
module {
  // 외부 런타임 심볼 (C-ABI 래퍼로 우리가 이미 제공하는 것)
  func.func private @sa16_load_B(%B: memref<64xi8>)
  func.func private @sa16_run_tile(%A: memref<16xi8>, %O: memref<64xi32>)

  // entry: A_stream = memref<?xi8> (길이는 16의 배수), B = memref<64xi8>,
  //        Out = memref<?x64xi32> (행 수 = 타일 수)
  func.func @offload_gemm(%A_stream: memref<?xi8>, %B: memref<64xi8>, %Out: memref<?x64xi32>) {
    %c0 = arith.constant 0 : index
    %c16 = arith.constant 16 : index

    // tile_count = dim(A_stream, 0) / 16
    %len = memref.dim %A_stream, 0 : memref<?xi8>
    %tile_count = arith.divsi %len, %c16 : index

    // B 한 번 로드
    call @sa16_load_B(%B) : (memref<64xi8>) -> ()

    // for t in [0 .. tile_count):
    scf.for %t = %c0 to %tile_count step %c1 {
      // A 슬라이스: [t*16 .. t*16+16)
      %offs   = arith.muli %t, %c16 : index
      %A_tile = memref.subview %A_stream[%offs] [16] [1]
                 : memref<?xi8> to memref<16xi8>

      // Out 행 슬라이스: Out[t, :]
      %O_row  = memref.subview %Out[%t, %c0] [1, 64] [1, 1]
                 : memref<?x64xi32> to memref<1x64xi32>
      // run_tile 은 memref<64xi32>를 받으므로 view로 랭크를 낮춤
      %O_1d   = memref.view %O_row[%c0][] : memref<1x64xi32> to memref<64xi32>

      call @sa16_run_tile(%A_tile, %O_1d)
        : (memref<16xi8>, memref<64xi32>) -> ()
    }

    return
  }
}

