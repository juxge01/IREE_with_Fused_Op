#include <stdint.h>

#define AXI_SA16_BASE      0x80000000UL
#define OUT_WIN_OFFSET     0x00001000UL

#define SA_FEED_ADDR       (AXI_SA16_BASE + 0x00000000UL)   // B 64B 고정 로드
#define SA_A_ADDR          (AXI_SA16_BASE + OUT_WIN_OFFSET) // A 타일, OUT 창

// RAM(0x30000000 ~ +0x01000000) 내부 주소로 변경
#define DMEM_OUT_COPY_BASE 0x30010000UL  // 결과 64워드 복사 위치

static inline void mmio_write32(uintptr_t addr, uint32_t value) {
  *(volatile uint32_t *)addr = value;
}
static inline uint32_t mmio_read32(uintptr_t addr) {
  return *(volatile uint32_t *)addr;
}
static void busy_delay(volatile uint32_t cycles) { while (cycles--) __asm__ volatile ("nop"); }

/* B 64바이트: 값 0..63 */
static void load_B_once(void) {
  for (uint32_t i = 0; i < 64; ++i) mmio_write32(SA_FEED_ADDR, i & 0xFF);
}

/* A 16바이트 타일 하나: base..base+15 */
static void load_A_tile(uint32_t base_val) {
  for (uint32_t i = 0; i < 16; ++i) mmio_write32(SA_A_ADDR, (base_val + i) & 0xFF);
}

/* OUT 창이 채워질 때까지 폴링 */
static int wait_for_results(uint32_t timeout_loops) {
  while (timeout_loops--) {
    uint32_t w1 = mmio_read32(SA_A_ADDR + 0x0);
    uint32_t w2 = mmio_read32(SA_A_ADDR + 0x4);
    uint32_t w3 = mmio_read32(SA_A_ADDR + 0x8);
    if ((w1 | w2 | w3) != 0) return 0;
    busy_delay(200);
  }
  return -1;
}

/* OUT 창(0x80001000)에서 64워드 -> RAM(0x30010000)으로 복사 */
static void dump_results_to_ram(void) {
  volatile uint32_t *dst = (volatile uint32_t *)DMEM_OUT_COPY_BASE;
  for (uint32_t i = 0; i < 64; ++i) dst[i] = mmio_read32(SA_A_ADDR + i * 4);
}

int main(void) {
  load_B_once();                 // 1) B 한 번 로드
  for (uint32_t t = 0; t < 8; ++t) load_A_tile(t * 16u);   // 2) A 타일 8개(0..127)
  (void)wait_for_results(200000);                          // 3) 완료 대기(대충)
  dump_results_to_ram();                                   // 4) 결과를 RAM으로 복사
  while (1) { __asm__ volatile("wfi"); }                   // 5) 대기
}
