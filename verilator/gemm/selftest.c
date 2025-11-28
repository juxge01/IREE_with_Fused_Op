/**
 * @file selftest.c
 * @brief Systolic Array (SA16)의 선형성 속성을 검증하는 셀프 테스트 스위트.
 *
 * 이 파일은 SA16 가속기가 다음과 같은 기본적인 선형 시스템의 속성을
 * 만족하는지 확인합니다:
 * 1. 결정성 (Determinism): 동일한 입력은 항상 동일한 출력을 보장하는가?
 * 2. 동차성 (Homogeneity/Scaling): 입력에 상수를 곱하면, 출력도 동일한 상수가 곱해지는가?
 * 3. 가산성 (Additivity): 두 입력의 합에 대한 출력이, 각 입력에 대한 출력의 합과 같은가?
 * 4. 제로 입력 (Zero Input): 제로 입력에 대해 제로 출력을 내는가?
 */

#include <stdint.h>

//=============================================================================
// 1. 외부 의존성 및 타입 정의 (External Dependencies & Types)
//=============================================================================

/**
 * @brief MLIR이 생성한 C 인터페이스 함수 (외부 링크).
 */
extern void _mlir_ciface_offload_tile(void* A_desc, void* B_desc, void* O_desc);

/**
 * @brief 저수준 UART 출력 함수 (외부 링크).
 * 이 함수는 main.c 또는 runtime.c에 정의되어 있어야 합니다.
 * stdio.h를 포함하지 않습니다.
 */
extern void putc(char c);

/**
 * @brief MLIR의 1D memref에 해당하는 C 구조체.
 * 주의: offset, sizes, strides 필드는 RV32에서도 64비트(long)여야 함.
 */
typedef struct {
  void* allocated;
  void* aligned;
  int64_t offset;
  int64_t sizes[1];
  int64_t strides[1];
} memref1d_t;


//=============================================================================
// 2. 헬퍼 함수 (Helper Functions)
//=============================================================================

// 디버깅을 위해 시뮬레이터의 특정 메모리 주소에 결과를 덤프하는 주소들
#define DMEM_DUMP0 0x30010000u
#define DMEM_DUMP1 0x30020000u
#define DMEM_DUMP2 0x30030000u
#define DMEM_DUMP3 0x30040000u

/**
 * @brief UART로 문자열을 출력합니다.
 */
static void puts__(const char* s) {
  while (*s) {
    putc(*s++);
  }
}

/**
 * @brief 버퍼의 내용을 특정 메모리 주소에 덤프합니다 (디버깅용).
 */
static void dump_to(uint32_t base, const uint32_t *buf, int n_words) {
  volatile uint32_t *p = (volatile uint32_t*)base;
  for (int i = 0; i < n_words; i++) {
    p[i] = buf[i];
  }
}

/**
 * @brief 두 버퍼의 내용이 동일한지 비교합니다.
 */
static int equals(const uint32_t *x, const uint32_t *y, int n_words) {
  for (int i = 0; i < n_words; i++) {
    if (x[i] != y[i]) return 0;
  }
  return 1;
}

/**
 * @brief 한 버퍼가 다른 버퍼의 스케일된 버전인지 비교합니다 (y == x * k).
 */
static int equals_scaled(const uint32_t *x, const uint32_t *y, uint32_t k, int n_words) {
  for (int i = 0; i < n_words; i++) {
    if (y[i] != x[i] * k) return 0;
  }
  return 1;
}

/**
 * @brief 한 버퍼가 다른 두 버퍼의 합과 같은지 비교합니다 (x == y + z).
 */
static int equals_sum(const uint32_t *x, const uint32_t *y, const uint32_t *z, int n_words) {
  for (int i = 0; i < n_words; i++) {
    if (x[i] != (y[i] + z[i])) return 0;
  }
  return 1;
}


//=============================================================================
// 3. 메인 테스트 함수 (Main Test Function)
//=============================================================================

/**
 * @brief SA16의 선형성 속성 테스트를 수행합니다.
 * @return 모든 테스트를 통과하면 1, 하나라도 실패하면 0.
 */
int run_sa16_linearity_test(void) {
  puts__("[ST] Starting SA16 linearity test suite...\n");

  // --- 테스트 데이터 준비 ---
  uint8_t  A0[16], A1[16], A2[16], A3[16];
  uint32_t Oa[64], Ob[64], Oc[64], Od[64], Oz[64];
  uint8_t  B_fixed[64]; // 모든 테스트에서 B는 동일한 값으로 고정

  for (int i = 0; i < 16; i++) A0[i] = 0;
  for (int i = 0; i < 16; i++) A1[i] = (uint8_t)(32 + i);
  for (int i = 0; i < 16; i++) A2[i] = (uint8_t)(A1[i] * 2); // For scaling test
  for (int i = 0; i < 16; i++) A3[i] = (uint8_t)(A1[i] + A1[i]); // For additivity test
  for (int i = 0; i < 64; i++) B_fixed[i] = (uint8_t)i;

  // --- memref 디스크립터 준비 ---
  memref1d_t desc_B = {B_fixed, B_fixed, 0, {64}, {1}};
  memref1d_t desc_A0 = {A0, A0, 0, {16}, {1}};
  memref1d_t desc_A1 = {A1, A1, 0, {16}, {1}};
  memref1d_t desc_A2 = {A2, A2, 0, {16}, {1}};
  memref1d_t desc_A3 = {A3, A3, 0, {16}, {1}};
  memref1d_t desc_Oa = {Oa, Oa, 0, {64}, {1}};
  memref1d_t desc_Ob = {Ob, Ob, 0, {64}, {1}};
  memref1d_t desc_Oc = {Oc, Oc, 0, {64}, {1}};
  memref1d_t desc_Od = {Od, Od, 0, {64}, {1}};
  memref1d_t desc_Oz = {Oz, Oz, 0, {64}, {1}};

  // --- 1. 결정성 테스트 (Determinism) ---
  _mlir_ciface_offload_tile(&desc_A1, &desc_B, &desc_Oa); // Oa <- f(A1, B)
  _mlir_ciface_offload_tile(&desc_A1, &desc_B, &desc_Ob); // Ob <- f(A1, B)
  int pass_det = equals(Oa, Ob, 64);
  dump_to(DMEM_DUMP0, Oa, 64);
  dump_to(DMEM_DUMP1, Ob, 64);
  puts__(pass_det ? "[ST] Determinism: PASS\n" : "[ST] Determinism: FAIL\n");

  // --- 2. 동차성/스케일링 테스트 (Scaling) ---
  _mlir_ciface_offload_tile(&desc_A2, &desc_B, &desc_Oc); // Oc <- f(2*A1, B)
  int pass_scl = equals_scaled(Oa, Oc, 2, 64); // Oc == 2 * Oa ?
  dump_to(DMEM_DUMP2, Oc, 64);
  puts__(pass_scl ? "[ST] Scaling:     PASS\n" : "[ST] Scaling:     FAIL\n");

  // --- 3. 가산성 테스트 (Additivity) ---
  _mlir_ciface_offload_tile(&desc_A3, &desc_B, &desc_Od); // Od <- f(A1+A1, B)
  int pass_add = equals_sum(Od, Oa, Oa, 64); // Od == Oa + Oa ?
  dump_to(DMEM_DUMP3, Od, 64);
  puts__(pass_add ? "[ST] Additivity:  PASS\n" : "[ST] Additivity:  FAIL\n");

  // --- 4. 제로 입력 테스트 (Zero-input) ---
  _mlir_ciface_offload_tile(&desc_A0, &desc_B, &desc_Oz); // Oz <- f(0, B)
  int pass_zero = 1;
  for(int i=0; i<64; i++) if(Oz[i] != 0) { pass_zero = 0; break; }
  puts__(pass_zero ? "[ST] Zero-input:  PASS\n" : "[ST] Zero-input:  FAIL\n");

  // --- 최종 요약 ---
  int all_pass = pass_det && pass_scl && pass_add && pass_zero;
  if (all_pass) {
    puts__("[ST] OK: All linearity tests passed.\n");
  } else {
    puts__("[ST] NG: One or more linearity tests failed.\n");
  }

  return all_pass;
}