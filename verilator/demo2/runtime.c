#include <stdint.h>
#include <stddef.h>

#define SA_BASE       0x80000000u
#define SA_FEED_B     (SA_BASE + 0x0000)  // B 64B seed (once)
#define SA_FEED_A     (SA_BASE + 0x0100)  // A 16B feed
#define SA_OUT        (SA_BASE + 0x1000)  // OUT 64 words (read-only)
#define UART_BASE     0x82002000u

// This struct definition MUST match the one in main.c
// and the layout expected by the LLVM-generated code.
typedef struct {
  void   *allocated;
  void   *aligned;
  int64_t offset;
  int64_t sizes[1];
  int64_t strides[1];
} memref1d_i8;

typedef struct {
  void   *allocated;
  void   *aligned;
  int64_t offset;
  int64_t sizes[1];
  int64_t strides[1];
} memref1d_i32;


static inline void mmio_w32(uint32_t a, uint32_t v){ *(volatile uint32_t*)a = v; }
static inline uint32_t mmio_r32(uint32_t a){ return *(volatile uint32_t*)a; }

static inline void putc(char c){ *(volatile uint8_t*)UART_BASE = (uint8_t)c; }

/* ---- 순수 포인터 버전 (내부 구현) ---- */
static void sa16_load_B_ptr(uint8_t *B) {
  // The original loop was writing 32-bit values from an 8-bit array,
  // which is likely incorrect. This now writes each byte correctly.
  volatile uint8_t *p = (volatile uint8_t*)SA_FEED_B;
  for(uint32_t i=0; i<64; i++) {
      p[i] = B[i];
  }
}

static void sa16_run_tile_ptr(uint8_t *A, uint32_t *Out) {
  // A는 feed 영역(0x8000_0100)에 써야 함
  // The original loop was writing 32-bit values from an 8-bit array.
  // This now writes each byte correctly.
  volatile uint8_t *p = (volatile uint8_t*)SA_FEED_A;
  for(uint32_t i=0; i<16; i++) {
      p[i] = A[i];
  }

  // 간단 폴링: OUT[1]이 0이 아니면 결과 도착(BYPASS_SA=0 기준)
  uint32_t timeout = 2000000u;
  while(timeout--){
    if(mmio_r32(SA_OUT + 4) != 0) break;
  }

  // OUT 창(0x8000_1000)에서 64워드 읽기
  for(uint32_t i=0; i<64; i++) {
      Out[i] = mmio_r32(SA_OUT + i*4);
  }
}

/* ---- MLIR C-인터페이스 래퍼 (memref 디스크립터 언팩) ----
   The function now accepts a pointer to the memref descriptor struct,
   matching the LLVM IR calling convention.
*/

// @sa16_load_B(%B: memref<64xi8>)
void _mlir_ciface_sa16_load_B(void* B_desc_ptr)
{
  memref1d_i8 *desc = (memref1d_i8*)B_desc_ptr;
  uint8_t * B_aligned = (uint8_t*)desc->aligned;
  int64_t offset = desc->offset;
  sa16_load_B_ptr(B_aligned + offset);
}

// @sa16_run_tile(%A: memref<16xi8>, %Out: memref<64xi32>)
void _mlir_ciface_sa16_run_tile(void* A_desc_ptr, void* O_desc_ptr)
{
  memref1d_i8 *A_desc = (memref1d_i8*)A_desc_ptr;
  uint8_t * A_aligned = (uint8_t*)A_desc->aligned;
  int64_t A_off = A_desc->offset;

  memref1d_i32 *O_desc = (memref1d_i32*)O_desc_ptr;
  uint32_t * O_aligned = (uint32_t*)O_desc->aligned;
  int64_t O_off = O_desc->offset;

  sa16_run_tile_ptr(A_aligned + A_off, O_aligned + O_off);
}