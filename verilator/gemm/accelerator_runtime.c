#include <stdint.h>

//=============================================================================
// 0. 런타임 지원 함수 (Runtime Support for Freestanding Environment)
//=============================================================================

/**
 * @brief 메모리의 특정 영역을 주어진 값으로 채웁니다.
 * @note -nostdlib 환경에서는 C 표준 라이브러리가 링크되지 않으므로,
 * 컴파일러가 최적화를 위해 생성하는 memset 호출을 처리하기 위해
 * 이 함수를 직접 구현해야 합니다.
 */
void* memset(void* dest, int val, unsigned int len) {
    unsigned char* ptr = (unsigned char*)dest;
    while (len-- > 0) {
        *ptr++ = (unsigned char)val;
    }
    return dest;
}


//=============================================================================
// 1. 저수준 하드웨어 제어 함수 (Low-Level Hardware Control)
//=============================================================================

#define SA_BASE       0x80000000u
#define SA_FEED_B     (SA_BASE + 0x0000)
#define SA_FEED_A     (SA_BASE + 0x0100)
#define SA_OUT        (SA_BASE + 0x1000)
#define TILE_SIZE     16

static inline void mmio_w8(uint32_t addr, uint8_t val) {
    *(volatile uint8_t*)addr = val;
}
static inline uint32_t mmio_r32(uint32_t addr) {
    return *(volatile uint32_t*)addr;
}

/**
 * @brief 2D 행렬에서 16x16 B 타일을 가져와 하드웨어에 로드합니다.
 * @note MLIR에서 호출할 수 있도록 함수 이름을 `_mlir_ciface_sa16_load_B`로 변경했습니다.
 */
void _mlir_ciface_sa16_load_B(const uint8_t* b_tile_base, int stride_n) {
    for (int r = 0; r < TILE_SIZE; ++r) {
        for (int c = 0; c < TILE_SIZE; ++c) {
            uint8_t val = b_tile_base[r * stride_n + c];
            mmio_w8(SA_FEED_B + (r * TILE_SIZE + c), val);
        }
    }
}

/**
 * @brief 2D 행렬에서 16x16 A 타일을 가져와 하드웨어에 공급하고 연산을 실행합니다.
 * @note MLIR에서 호출할 수 있도록 함수 이름을 `_mlir_ciface_sa16_run_tile`로 변경했습니다.
 */
void _mlir_ciface_sa16_run_tile(const uint8_t* a_tile_base, int stride_k, uint32_t* out_tile) {
    for (int r = 0; r < TILE_SIZE; ++r) {
        for (int c = 0; c < TILE_SIZE; ++c) {
            uint8_t val = a_tile_base[r * stride_k + c];
            mmio_w8(SA_FEED_A + (r * TILE_SIZE + c), val);
        }
    }

    uint32_t timeout = 2000000u;
    while(timeout--) {
        if (mmio_r32(SA_OUT + 4) != 0) break;
    }

    for (int i = 0; i < TILE_SIZE * TILE_SIZE; ++i) {
        out_tile[i] = mmio_r32(SA_OUT + i * 4);
    }
}


//=============================================================================
// 2. GEMM 오케스트레이션 함수 (GEMM Orchestration)
//=============================================================================

/**
 * @brief 행렬 곱셈 C = A * B 연산을 타일 단위로 수행합니다.
 */
void run_gemm(
    const uint8_t* A_base,
    const uint8_t* B_base,
    uint32_t* C_base,
    int M, int K, int N)
{
    for (int m0 = 0; m0 < M; m0 += TILE_SIZE) {
        for (int n0 = 0; n0 < N; n0 += TILE_SIZE) {
            uint32_t temp_C_tile[TILE_SIZE * TILE_SIZE] = {0};
            for (int k0 = 0; k0 < K; k0 += TILE_SIZE) {
                const uint8_t* a_tile_ptr = A_base + (m0 * K) + k0;
                const uint8_t* b_tile_ptr = B_base + (k0 * N) + n0;

                // 함수 호출 부분을 변경된 이름으로 수정합니다.
                _mlir_ciface_sa16_load_B(b_tile_ptr, N);

                uint32_t single_mul_result[TILE_SIZE * TILE_SIZE];
                // 함수 호출 부분을 변경된 이름으로 수정합니다.
                _mlir_ciface_sa16_run_tile(a_tile_ptr, K, single_mul_result);

                for (int i = 0; i < TILE_SIZE * TILE_SIZE; ++i) {
                    temp_C_tile[i] += single_mul_result[i];
                }
            }

            uint32_t* c_tile_ptr = C_base + (m0 * N) + n0;
            for (int row = 0; row < TILE_SIZE; ++row) {
                for (int col = 0; col < TILE_SIZE; ++col) {
                    c_tile_ptr[row * N + col] = temp_C_tile[row * TILE_SIZE + col];
                }
            }
        }
    }
}