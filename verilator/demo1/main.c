// main.c  — call MLIR entry with proper memref descriptors
#include <stdint.h>
#include <string.h>

// MLIR C-ABI wrapper (generated)
extern void _mlir_ciface_offload_tile(void *A_desc, void *B_desc, void *O_desc);

// 1D memref descriptor layout
typedef struct {
  void   *allocated;
  void   *aligned;
  long    offset;     // i64 even on RV32
  long    sizes[1];   // i64[1]
  long    strides[1]; // i64[1]
} memref1d;

// global buffers (stack 말고 전역으로!)
static uint8_t  A[16];
static uint8_t  B[64];
static uint32_t OUT[64];

// (optional) UART helpers
#define UART_BASE 0x82002000u
static inline void putc(char c){ *(volatile uint8_t*)UART_BASE = (uint8_t)c; }
static void puts_(const char*s){ while(*s) putc(*s++); }
static void puthex32(uint32_t v){
  const char* h="0123456789ABCDEF";
  for(int i=7;i>=0;i--) putc(h[(v>>(i*4))&0xF]);
}

int main(void)
{
  // 예시 데이터: B=0..63, A=48..63
  // for (int i=0; i<64; i++) B[i] = (uint8_t)i;
  // for (int i=0; i<16; i++) A[i] = (uint8_t)(48+i);

  // a_tile0, b_init 과 동일 데이터 입력
  for (int i=0; i<64; i++) B[i] = (uint8_t)(i+1);
  for (int i=0; i<16; i++) A[i] = (uint8_t)(1+i);

  // memref descriptors (sizes/strides/offset MUST be i64)
  // Initialize field-by-field to avoid compiler-generated memcpy
  memref1d A_desc;
  A_desc.allocated = A;
  A_desc.aligned = A;
  A_desc.offset = 0;
  A_desc.sizes[0] = 16;
  A_desc.strides[0] = 1;

  memref1d B_desc;
  B_desc.allocated = B;
  B_desc.aligned = B;
  B_desc.offset = 0;
  B_desc.sizes[0] = 64;
  B_desc.strides[0] = 1;

  memref1d O_desc;
  O_desc.allocated = OUT;
  O_desc.aligned = OUT;
  O_desc.offset = 0;
  O_desc.sizes[0] = 64;
  O_desc.strides[0] = 1;

  // Call MLIR entry: order = (A, B, Out)
  _mlir_ciface_offload_tile(&A_desc, &B_desc, &O_desc);

  // 앞 8워드만 덤프
  puts_("\n[FW] OUT[0..7]:");
  for (int i=0; i<8; i++){ putc(' '); puthex32(OUT[i]); }
  puts_("\n");

  while(1){ __asm__ volatile("wfi"); }
}