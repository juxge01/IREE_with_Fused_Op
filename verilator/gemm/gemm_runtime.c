#include <stdint.h>

//=============================================================================
// 1. 외부 의존성 선언 (External Dependencies)
//=============================================================================

// runtime.c에 정의된, 하드웨어를 직접 제어하는 저수준 함수들입니다.
// 이 함수들을 여기서도 사용하기 위해 extern으로 선언합니다.
// 현재 이 함수들은 1D 벡터용이지만, 곧 2D 타일을 다루도록 수정할 것입니다.
extern void sa16_load_B_ptr(uint8_t *B_tile);
extern void sa16_run_tile_ptr(uint8_t *A_tile, uint32_t *Out_tile);


//=============================================================================
// 2. GEMM 오케스트레이션 함수 (GEMM Orchestration)
//=============================================================================

/**
 * @brief 행렬 곱셈 C = A * B 연산을 타일 단위로 수행합니다.
 *
 * @param A_base M x K 행렬 A의 시작 주소
 * @param B_base K x N 행렬 B의 시작 주소
 * @param C_base M x N 행렬 C의 시작 주소
 * @param M 행렬 A, C의 행(row) 크기
 * @param K 행렬 A의 열(column)이자 행렬 B의 행 크기
 * @param N 행렬 B, C의 열(column) 크기
 *
 * @note 현재 구현은 M, K, N이 타일 크기(16)의 배수라고 가정합니다.
 */
void run_gemm(
    const uint8_t* A_base,
    const uint8_t* B_base,
    uint32_t* C_base,
    int M, int K, int N)
{
    const int TILE_SIZE = 16;

    // 출력 행렬 C의 각 16x16 타일을 순회하는 루프
    for (int m0 = 0; m0 < M; m0 += TILE_SIZE) {
        for (int n0 = 0; n0 < N; n0 += TILE_SIZE) {

            // --- 하나의 C 타일(16x16)을 계산하는 과정 ---
            uint32_t temp_C_tile[TILE_SIZE * TILE_SIZE] = {0}; // 결과를 누적할 임시 버퍼

            // 1. K 차원을 따라 이동하며 A타일과 B타일을 곱하고, 그 결과를 temp_C_tile에 계속 더합니다.
            //    (Dot Product Accumulation)
            for (int k0 = 0; k0 < K; k0 += TILE_SIZE) {

                // 현재 계산에 필요한 A, B 타일의 시작 주소를 계산합니다.
                // A는 (m0, k0) 위치의 타일, B는 (k0, n0) 위치의 타일입니다.
                const uint8_t* a_tile_ptr = A_base + (m0 * K) + k0;
                const uint8_t* b_tile_ptr = B_base + (k0 * N) + n0;

                // *** 중요 ***
                // 저수준 함수들을 호출하여 한 번의 타일 곱셈을 수행합니다.
                // 지금은 이 함수들이 2D 타일을 올바르게 처리한다고 '가정'합니다.
                // 다음 단계에서 이 함수들의 내부를 실제로 수정할 것입니다.
                sa16_load_B_ptr((uint8_t*)b_tile_ptr);

                uint32_t single_mul_result[TILE_SIZE * TILE_SIZE];
                sa16_run_tile_ptr((uint8_t*)a_tile_ptr, single_mul_result);

                // Systolic Array의 곱셈 결과를 누적 버퍼(temp_C_tile)에 더해줍니다.
                for (int i = 0; i < TILE_SIZE * TILE_SIZE; ++i) {
                    temp_C_tile[i] += single_mul_result[i];
                }
            }

            // 2. 누적이 완료된 타일의 최종 결과를 실제 C 행렬의 올바른 위치로 복사합니다.
            uint32_t* c_tile_ptr = C_base + (m0 * N) + n0;
            for (int row = 0; row < TILE_SIZE; ++row) {
                for (int col = 0; col < TILE_SIZE; ++col) {
                    // C 행렬의 전체 레이아웃에 맞게 2D 인덱스를 계산하여 복사합니다.
                    c_tile_ptr[row * N + col] = temp_C_tile[row * TILE_SIZE + col];
                }
            }
        }
    }
}
