// main.c  — call MLIR entry with proper memref descriptors
#include <stdint.h>
#include <string.h>

// MLIR C-ABI wrapper (generated)
extern void _mlir_ciface_offload_tile(void *A_desc, void *B_desc, void *O_desc);
extern int run_sa16_linearity_test(void);

// 1D memref descriptor layout
typedef struct {
  void   *allocated;
  void   *aligned;
  long    offset;     // i64 even on RV32
  long    sizes[1];   // i64[1]
  long    strides[1]; // i64[1]
} memref1d;

// global buffers (stack 말고 전역으로!)
// selftest에서 사용하지 않더라도, 다른 모듈에서 참조할 수 있으므로 유지합니다.
uint8_t  A[16];
uint8_t  B[64];
uint32_t OUT[64];

// (optional) UART helpers
#define UART_BASE 0x82002000u

/**
 * @brief UART로 한 문자를 출력합니다.
 * 'static'을 제거하여 다른 파일(selftest.c)에서도 이 함수를 링크할 수 있도록 합니다.
 */
void putc(char c){
  *(volatile uint8_t*)UART_BASE = (uint8_t)c;
}

static void puts_(const char*s){
  while(*s) putc(*s++);
}

// main.c에서 더 이상 사용하지 않으므로 주석 처리하거나 지워도 됩니다.
// static void puthex32(uint32_t v){
//   const char* h="0123456789ABCDEF";
//   for(int i=7;i>=0;i--) putc(h[(v>>(i*4))&0xF]);
// }

int main(void)
{
  // main 함수의 기존 로직은 selftest가 독립적으로 수행되므로
  // 주석 처리하거나 비워두어도 괜찮습니다.
  // 이 코드는 이제 selftest의 진입점(entry point) 역할만 합니다.

  puts_("[FW] Starting application...\n");

  run_sa16_linearity_test();

  puts_("[FW] All tasks finished.\n");

  while(1){ __asm__ volatile("wfi"); }
}