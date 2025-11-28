#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

// 초기화 / 종료
int sa16_init(void);
void sa16_deinit(void);

// 사이클 수
uint64_t sa16_get_cycles(void);

// MatMul + Bias Add
void sa16_matmul_bias_add_offload(
    const uint8_t* lhs_ptr, const uint8_t* rhs_ptr, const uint8_t* bias_ptr,
    uint32_t M, uint32_t K, uint32_t N);

void sa16_wait_for_completion(uint8_t* out, uint32_t M, uint32_t N);

// Conv / Conv+ReLU
void sa16_conv_offload(const uint8_t* input, const uint8_t* filter,
                       const uint8_t* bias,
                       uint32_t N, uint32_t H, uint32_t W,
                       uint32_t C, uint32_t OC,
                       uint32_t KH, uint32_t KW,
                       uint32_t stride_h, uint32_t stride_w,
                       uint32_t pad_h, uint32_t pad_w);

void sa16_conv_relu_offload(const uint8_t* input, const uint8_t* filter,
                            const uint8_t* bias,
                            uint32_t N, uint32_t H, uint32_t W,
                            uint32_t C, uint32_t OC,
                            uint32_t KH, uint32_t KW,
                            uint32_t stride_h, uint32_t stride_w,
                            uint32_t pad_h, uint32_t pad_w);

void sa16_conv_wait_for_completion(uint8_t* output,
                                   uint32_t N, uint32_t OH, uint32_t OW, uint32_t OC);

#ifdef __cplusplus
}
#endif

