// Copyright 2024 The IREE Authors
//
// Licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "iree/hal/drivers/accel/accel_command_buffer.h"

#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <dlfcn.h>
#include <stdbool.h>
#include <stdio.h>
#include <pthread.h>

#include "iree/base/api.h"
#include "iree/base/internal/arena.h"
#include "iree/base/string_view.h"
#include "iree/hal/drivers/accel/accel_executable.h"
#include "iree/hal/buffer.h"
#include "iree/hal/executable.h"
#include "iree/hal/local/executable_environment.h"
#include "iree/hal/local/executable_library.h"
#include "iree/hal/local/local_executable.h"
#include "iree/hal/utils/resource_set.h"
#include "iree/task/affinity_set.h"
#include "iree/task/list.h"
#include "iree/task/submission.h"
#include "iree/task/task.h"

#include "tools/prof/micro_profiler.h"

#define SA16_MAX_TENSOR_BYTES     (4u * 1024u * 1024u)    // 텐서 당 4MB
#define SA16_MAX_WORKSPACE_BYTES (64u * 1024u * 1024u)    // 워크스페이스 추정 64MB

static pthread_mutex_t g_sa16_mu = PTHREAD_MUTEX_INITIALIZER;

// RTLD flag 선택(일부 환경에서 DPI 심볼이 하위 로더에 노출되어야 할 수 있음)
static int accel_use_rtld_global(void) {
  static int inited = 0, enabled = 0;
  if (!inited) {
    const char* e = getenv("ACCEL_RTLD_GLOBAL");
    enabled = (e && *e && strcmp(e, "0") != 0) ? 1 : 0;
    inited = 1;
  }
  return enabled;
}


// --- ACCEL debug switch (env: ACCEL_DEBUG=1) ---
static inline int accel_debug_enabled(void) {
  static int inited = 0, enabled = 0;
  if (!inited) {
    const char* e = getenv("ACCEL_DEBUG");
    enabled = (e && *e && strcmp(e, "0") != 0) ? 1 : 0;
    inited = 1;
  }
  return enabled;
}

// --- ACCEL force CPU (env: ACCEL_FORCE_CPU=1) ---
static inline int accel_force_cpu(void) {
  static int inited = 0, enabled = 0;
  if (!inited) {
    const char* e = getenv("ACCEL_FORCE_CPU");
    enabled = (e && *e && strcmp(e, "0") != 0) ? 1 : 0;
    inited = 1;
  }
  return enabled;
}

// --- Optional force route (env flags) ---
static inline int accel_force_conv(void) {
  static int inited = 0, enabled = 0;
  if (!inited) {
    const char* e = getenv("ACCEL_FORCE_CONV");
    enabled = (e && *e && strcmp(e, "0") != 0) ? 1 : 0;
    inited = 1;
  }
  return enabled;
}

static inline int accel_force_matmul(void) {
  static int inited = 0, enabled = 0;
  if (!inited) {
    const char* e = getenv("ACCEL_FORCE_MATMUL");
    enabled = (e && *e && strcmp(e, "0") != 0) ? 1 : 0;
    inited = 1;
  }
  return enabled;
}

static iree_device_size_t get_effective_len(iree_hal_buffer_ref_t b) {
  if (!b.buffer) return 0;
  if (b.length) return b.length;
  // length가 0이면 버퍼 핸들의 실제 길이를 사용 (API 이름은 백엔드 버전에 맞춰 교체)
  return iree_hal_buffer_byte_length(b.buffer);
}

static void accel_debug_print_dispatch(
    iree_string_view_t ep,
    const iree_hal_dispatch_config_t* cfg,
    const iree_hal_buffer_ref_list_t* bindings) {
  char ep_cstr[128];
  iree_host_size_t n =
      iree_min(ep.size, (iree_host_size_t)(sizeof(ep_cstr) - 1));
  memcpy(ep_cstr, ep.data, n);
  ep_cstr[n] = '\0';

  fprintf(stderr,
          "[ACCEL][DISPATCH] ep='%s' wg_size=(%u,%u,%u) wg_count=(%u,%u,%u) bindings=%zu\n",
          ep_cstr,
          cfg->workgroup_size[0], cfg->workgroup_size[1], cfg->workgroup_size[2],
          cfg->workgroup_count[0], cfg->workgroup_count[1], cfg->workgroup_count[2],
          (size_t)bindings->count);

  for (size_t i = 0; i < bindings->count; ++i) {
      const iree_hal_buffer_ref_t b = bindings->values[i];
      if (b.buffer) {
        iree_device_size_t eff = get_effective_len(b);
        fprintf(stderr, "  [B%zu] buf=%p off=%" PRIu64 " len=%" PRIu64 "\n",
                i, (void*)b.buffer, (uint64_t)b.offset, (uint64_t)eff);
      } else {
        fprintf(stderr, "  [B%zu] buf=(unallocated) off=%" PRIu64 " len=%" PRIu64 "\n",
                i, (uint64_t)b.offset, (uint64_t)b.length);
      }
    }
}

// --- Soft-skip helper: 로그만 찍고 OK로 리턴하여 체인 끊김/행업 방지 ---
#define ACCEL_CONV_SOFT_SKIP(reason_fmt, ...) do {                            \
  if (accel_debug_enabled()) {                                                \
    fprintf(stderr, "[ACCEL][CONV][SKIP] " reason_fmt "\n", ##__VA_ARGS__);   \
  }                                                                           \
  cmd->issued = false;                                                        \
  status = iree_ok_status();                                                  \
  goto conv_cleanup;                                                          \
} while(0)

#ifndef MP_SCOPE
  #define MP_SCOPE(name) do { (void)sizeof(name); } while (0)
#endif

// Verilator 시뮬레이션 라이브러리 핸들
static void* vtop_lib_handle = NULL;
static int (*sa16_init_p)(void) = NULL;
static void (*sa16_deinit_p)(void) = NULL;

#ifdef __cplusplus
extern "C" {
#endif

// ---- DPI symbols (weak-linked) ----
// void (*sa16_matmul_bias_add_offload_p)(const uint8_t*, const uint8_t*,
//                                        const uint8_t*, uint32_t, uint32_t,
//                                        uint32_t) = NULL;
void (*sa16_matmul_bias_add_offload_p)(
    const uint8_t*, const uint8_t*, const uint8_t*,
    uint32_t, uint32_t, uint32_t) = NULL;

void sa16_matmul_bias_add_offload(
    const uint8_t* lhs, const uint8_t* rhs, const uint8_t* bias,
    uint32_t M, uint32_t K, uint32_t N) {
  if (!sa16_matmul_bias_add_offload_p) return;
  pthread_mutex_lock(&g_sa16_mu);
  sa16_matmul_bias_add_offload_p(lhs, rhs, bias, M, K, N);
  pthread_mutex_unlock(&g_sa16_mu);
}

void (*sa16_matmul_bias_add_relu_offload_p)(const uint8_t*, const uint8_t*,
                                            const uint8_t*, uint32_t,
                                            uint32_t, uint32_t) = NULL;

void (*sa16_wait_for_completion_p)(uint8_t*, uint32_t, uint32_t) = NULL;
void (*iree_dpi_sa16_conv_offload_p)(const uint8_t*, const uint8_t*, const uint8_t*,
                            uint32_t, uint32_t, uint32_t, uint32_t, uint32_t,
                            uint32_t, uint32_t, uint32_t, uint32_t, uint32_t,
                            uint32_t) = NULL;
void (*sa16_conv_relu_offload_p)(
    const uint8_t*, const uint8_t*, const uint8_t*, uint32_t, uint32_t,
    uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t,
    uint32_t, uint32_t) = NULL;
void (*iree_dpi_sa16_conv_wait_for_completion_p)(uint8_t*, uint32_t, uint32_t, uint32_t,
                                        uint32_t) = NULL;
uint64_t (*sa16_get_cycles_p)(void) = NULL;
static uint64_t (*sa16_get_cycles_latched_p)(void) = NULL;
static uint64_t (*sa16_get_cycles_live_p)(void) = NULL;

void sa16_matmul_bias_add_relu_offload(const uint8_t* lhs, const uint8_t* rhs,
                                       const uint8_t* bias, uint32_t M,
                                       uint32_t K, uint32_t N) {
  if (!sa16_matmul_bias_add_relu_offload_p) return;
  pthread_mutex_lock(&g_sa16_mu);
  sa16_matmul_bias_add_relu_offload_p(lhs, rhs, bias, M, K, N);
  pthread_mutex_unlock(&g_sa16_mu);
}

void sa16_wait_for_completion(uint8_t* out, uint32_t M, uint32_t N) {
  if (!sa16_wait_for_completion_p) return;
  pthread_mutex_lock(&g_sa16_mu);
  sa16_wait_for_completion_p(out, M, N);
  pthread_mutex_unlock(&g_sa16_mu);
}

void iree_dpi_sa16_conv_offload(const uint8_t* input, const uint8_t* filter,
                       const uint8_t* bias, uint32_t N, uint32_t H, uint32_t W,
                       uint32_t C, uint32_t OC, uint32_t KH, uint32_t KW,
                       uint32_t stride_h, uint32_t stride_w, uint32_t pad_h,
                       uint32_t pad_w) {
  if (!iree_dpi_sa16_conv_offload_p) return;
  pthread_mutex_lock(&g_sa16_mu);
  iree_dpi_sa16_conv_offload_p(input, filter, bias, N,H,W,C,OC,KH,KW,
                               stride_h,stride_w,pad_h,pad_w);
  pthread_mutex_unlock(&g_sa16_mu);
}

void sa16_conv_relu_offload(const uint8_t* input, const uint8_t* filter,
                            const uint8_t* bias, uint32_t N, uint32_t H,
                            uint32_t W, uint32_t C, uint32_t OC, uint32_t KH,
                            uint32_t KW, uint32_t stride_h, uint32_t stride_w,
                            uint32_t pad_h, uint32_t pad_w) {
  if (!sa16_conv_relu_offload_p) return;
  pthread_mutex_lock(&g_sa16_mu);
  sa16_conv_relu_offload_p(input, filter, bias, N, H, W, C, OC, KH, KW,
                           stride_h, stride_w, pad_h, pad_w);
  pthread_mutex_unlock(&g_sa16_mu);
}

void iree_dpi_sa16_conv_wait_for_completion(uint8_t* output, uint32_t N, uint32_t OH,
                                   uint32_t OW, uint32_t OC) {
  if (!iree_dpi_sa16_conv_wait_for_completion_p) return;
  pthread_mutex_lock(&g_sa16_mu);
  iree_dpi_sa16_conv_wait_for_completion_p(output, N, OH, OW, OC);
  pthread_mutex_unlock(&g_sa16_mu);
}

uint64_t sa16_get_cycles(void) {
  if (sa16_get_cycles_p) return sa16_get_cycles_p();
  return 0;
}

#ifdef __cplusplus
}  // extern "C"
#endif

// ============================================================================
// 가속기 라이브러리(libVtop.so) 동적 로딩 및 초기화
// ============================================================================
static bool sa16_is_initialized = false;

#define RESOLVE_REQ(name) do {                                   \
  dlerror();                                                     \
  *(void**)(&name##_p) = dlsym(vtop_lib_handle, #name);          \
  char* _e = dlerror();                                          \
  if (_e != NULL || !(name##_p)) {                               \
    fprintf(stderr, "Error resolving required '%s': %s\n",       \
            #name, _e ? _e : "(null)");                          \
    dlclose(vtop_lib_handle); vtop_lib_handle = NULL;            \
    return -1;                                                   \
  }                                                              \
} while(0)

#define RESOLVE_OPT(name) do {                                   \
  dlerror();                                                     \
  *(void**)(&name##_p) = dlsym(vtop_lib_handle, #name);          \
  char* _e = dlerror();                                          \
  if (_e != NULL || !(name##_p)) {                               \
    fprintf(stderr, "[ACCEL][OPT] symbol '%s' not found: %s\n",  \
            #name, _e ? _e : "");                                \
  }                                                               \
} while(0)

static void host_sa16_deinit(void) {
  if (vtop_lib_handle) {
    if (sa16_deinit_p) sa16_deinit_p();
    dlclose(vtop_lib_handle);
    vtop_lib_handle = NULL;

    sa16_init_p = NULL; sa16_deinit_p = NULL;
    sa16_matmul_bias_add_offload_p = NULL;
    sa16_matmul_bias_add_relu_offload_p = NULL;
    sa16_wait_for_completion_p = NULL;
    sa16_get_cycles_p = NULL;
    iree_dpi_sa16_conv_offload_p = NULL;
    sa16_get_cycles_latched_p = NULL;
    sa16_get_cycles_live_p = NULL;
    sa16_conv_relu_offload_p = NULL;
    iree_dpi_sa16_conv_wait_for_completion_p = NULL;
    fprintf(stderr, "--- Custom accelerator deinitialized ---\n");
  }
}

static int host_sa16_init(void) {
  const char* lib_path = getenv("VTOP_LIBRARY_PATH");
  if (!lib_path) {
    // lib_path = "/home/jueun/park/renode/renode-latest/renode-dpi-examples/samples/axi_sa16/axi_sa16/build/libVtop.so";
    lib_path = "/home/jueun/park/renode/renode-latest/renode-dpi-examples/samples/axi_sa16/axi_sa16/build_lib/libVtop.so";
  }

  dlerror();
  vtop_lib_handle = dlopen(lib_path,
                           RTLD_NOW | (accel_use_rtld_global() ? RTLD_GLOBAL : RTLD_LOCAL)); 

  if (!vtop_lib_handle) {
    fprintf(stderr, "HAL accel: Error loading '%s': %s\n", lib_path, dlerror());
    return -1;
  }

  fprintf(stderr, "--- Custom accelerator initializing via libVtop.so ---\n");
  fprintf(stderr, "VTOP_LIBRARY_PATH: %s\n", lib_path);

  RESOLVE_OPT(sa16_matmul_bias_add_offload);
  RESOLVE_OPT(sa16_matmul_bias_add_relu_offload);
  RESOLVE_OPT(sa16_wait_for_completion);
  RESOLVE_OPT(sa16_get_cycles);
  *(void**)(&iree_dpi_sa16_conv_offload_p) = dlsym(vtop_lib_handle, "sa16_conv_offload");
  RESOLVE_OPT(sa16_conv_relu_offload);
  *(void**)(&iree_dpi_sa16_conv_wait_for_completion_p) = dlsym(vtop_lib_handle, "sa16_conv_wait_for_completion");
  RESOLVE_OPT(sa16_get_cycles_latched);
  RESOLVE_OPT(sa16_get_cycles_live);

  // fprintf(stderr, "[sa16] matmul=%p wait=%p get_cycles=%p conv=%p conv_relu=%p conv_wait=%p\n",
  //       (void*)sa16_matmul_bias_add_offload_p,
  //       (void*)sa16_wait_for_completion_p,
  //       (void*)sa16_get_cycles_p,
  //       (void*)iree_dpi_sa16_conv_offload_p,
  //       (void*)sa16_conv_relu_offload_p,
  //       (void*)iree_dpi_sa16_conv_wait_for_completion_p);

  dlerror();
  *(void**)(&sa16_init_p) = dlsym(vtop_lib_handle, "sa16_init");
  char* e = dlerror();
  if (!e && sa16_init_p) {
    int rc = sa16_init_p();
    if (rc != 0) {
      fprintf(stderr, "sa16_init returned %d\n", rc);
      dlclose(vtop_lib_handle); vtop_lib_handle = NULL;
      return -1;
    }
  } else {
    void (*sim_init_p)(void) = (void(*)())dlsym(vtop_lib_handle, "sim_init");
    e = dlerror();
    if (!e && sim_init_p) sim_init_p();
  }

  *(void**)(&sa16_deinit_p) = dlsym(vtop_lib_handle, "sa16_deinit");

  fprintf(stderr, "Successfully loaded libVtop.so and resolved DPI symbols.\n");
  atexit(host_sa16_deinit);
  return 0;
}

// ---- Conv 파라미터 추론 유틸(이름 파싱 실패 시 사용) -------------------------
// static bool accel_infer_conv_params_from_lengths(
//     size_t elem_size, uint64_t il, uint64_t fl, uint64_t bl, uint64_t ol,
//     uint32_t N, uint32_t OH, uint32_t OW,
//     uint32_t* KH, uint32_t* KW, uint32_t* C, uint32_t* OC,
//     uint32_t* stride_h, uint32_t* stride_w, uint32_t* pad_h, uint32_t* pad_w) {
//   // 기본 가정
//   uint32_t s_candidates[] = {1, 2};
//   uint32_t p_candidates[] = {0, 1};
//   uint32_t k_candidates[] = {1, 3, 5, 7};

//   // OC: bias 있으면 bias로, 없으면 output 길이로 계산
//   if (bl) {
//     if (bl % elem_size) return false;
//     *OC = (uint32_t)(bl / elem_size);
//   } else {
//     if (ol == 0 || elem_size == 0 || N == 0 || OH == 0 || OW == 0) return false;
//     uint64_t total = ol / elem_size;
//     uint64_t spatial = (uint64_t)N * OH * OW;
//     if (!spatial || (total % spatial)) return false;
//     *OC = (uint32_t)(total / spatial);
//   }

//   // KH,KW,C 후보 대입
//   for (size_t ki = 0; ki < 4; ++ki) {  // IREE_ARRAYSIZE(k_candidates)
//     uint32_t k = k_candidates[ki];
//     uint64_t denom = (uint64_t)elem_size * k * k * (*OC);
//     if (!denom || (fl % denom)) continue;
//     uint32_t c = (uint32_t)(fl / denom);
//     if (!c) continue;
//     for (size_t si = 0; si < 2; ++si) {  // IREE_ARRAYSIZE(s_candidates)
//       uint32_t s = s_candidates[si];
//       for (size_t pi = 0; pi < 2; ++pi) {  // IREE_ARRAYSIZE(p_candidates)
//         uint32_t p = p_candidates[pi];
//         uint32_t H = (OH - 1) * s + k - 2 * p;
//         uint32_t W = (OW - 1) * s + k - 2 * p;
//         if (!H || !W) continue;
//         uint64_t expect_il = (uint64_t)N * H * W * c * elem_size;
//         if (expect_il == il) {
//           *KH = *KW = k; *C = c;
//           *stride_h = *stride_w = s;
//           *pad_h = *pad_w = p;
//           return true;
//         }
//       }
//     }
//   }
//   return false;
// }

// --- NEW: Conv parameter inference helper (when name parsing fails) ---
static bool accel_infer_conv_params_from_lengths(
    size_t elem_size, uint64_t il, uint64_t fl, uint64_t bl, uint64_t ol,
    uint32_t N, uint32_t OH, uint32_t OW,
    uint32_t* KH, uint32_t* KW, uint32_t* C, uint32_t* OC,
    uint32_t* stride_h, uint32_t* stride_w, uint32_t* pad_h, uint32_t* pad_w) {
  // Common candidates
  uint32_t s_candidates[] = {1, 2};
  uint32_t p_candidates[] = {0, 1};
  uint32_t k_candidates[] = {1, 3, 5, 7};

  // Infer OC from bias or output length
  if (bl) {
    if (bl % elem_size) return false;
    *OC = (uint32_t)(bl / elem_size);
  } else {
    if (ol == 0 || elem_size == 0 || N == 0 || OH == 0 || OW == 0) return false;
    uint64_t total = ol / elem_size;
    uint64_t spatial = (uint64_t)N * OH * OW;
    if (!spatial || (total % spatial)) return false;
    *OC = (uint32_t)(total / spatial);
  }

  // Iterate through candidates for KH, KW, C
  for (size_t ki = 0; ki < 4; ++ki) {
    uint32_t k = k_candidates[ki];
    uint64_t denom = (uint64_t)elem_size * k * k * (*OC);
    if (!denom || (fl % denom)) continue;
    uint32_t c = (uint32_t)(fl / denom);
    if (!c) continue;
    for (size_t si = 0; si < 2; ++si) {
      uint32_t s = s_candidates[si];
      for (size_t pi = 0; pi < 2; ++pi) {
        uint32_t p = p_candidates[pi];
        uint32_t H = (OH - 1) * s + k - 2 * p;
        uint32_t W = (OW - 1) * s + k - 2 * p;
        if (!H || !W) continue;
        uint64_t expect_il = (uint64_t)N * H * W * c * elem_size;
        if (expect_il == il) {
          *KH = *KW = k; *C = c;
          *stride_h = *stride_w = s;
          *pad_h = *pad_w = p;
          return true;
        }
      }
    }
  }
  return false;
}


// ============================================================================
// Forward declarations
// ============================================================================
typedef struct iree_hal_accel_command_buffer_t iree_hal_accel_command_buffer_t;

static iree_status_t iree_hal_accel_command_buffer_emit_execution_task(
    iree_hal_accel_command_buffer_t* command_buffer, iree_task_t* task);
static iree_status_t iree_hal_accel_cmd_conv_offload(
    void* user_context, iree_task_t* task,
    iree_task_submission_t* pending_submission);
static iree_status_t iree_hal_accel_cmd_conv_wait(
    void* user_context, iree_task_t* task,
    iree_task_submission_t* pending_submission);
static iree_status_t iree_hal_accel_cmd_conv_relu_offload(
    void* user_context, iree_task_t* task,
    iree_task_submission_t* pending_submission);
static iree_status_t iree_hal_accel_cmd_conv_relu_wait(
    void* user_context, iree_task_t* task,
    iree_task_submission_t* pending_submission);
static iree_status_t iree_hal_accel_cmd_matmul_wait(
    void* user_context, iree_task_t* task,
    iree_task_submission_t* pending_submission);
static iree_status_t iree_hal_accel_cmd_matmul_offload(
    void* user_context, iree_task_t* task,
    iree_task_submission_t* pending_submission);
static iree_status_t iree_hal_accel_cmd_matmul_relu_offload(
    void* user_context, iree_task_t* task,
    iree_task_submission_t* pending_submission);

static void iree_hal_accel_command_buffer_destroy(
    iree_hal_command_buffer_t* base_command_buffer);
static iree_status_t iree_hal_accel_command_buffer_begin(
    iree_hal_command_buffer_t* base_command_buffer);
static iree_status_t iree_hal_accel_command_buffer_end(
    iree_hal_command_buffer_t* base_command_buffer);
static iree_status_t iree_hal_accel_command_buffer_begin_debug_group(
    iree_hal_command_buffer_t* base_command_buffer, iree_string_view_t label,
    iree_hal_label_color_t label_color,
    const iree_hal_label_location_t* location);
static iree_status_t iree_hal_accel_command_buffer_end_debug_group(
    iree_hal_command_buffer_t* base_command_buffer);
static iree_status_t iree_hal_accel_command_buffer_execution_barrier(
    iree_hal_command_buffer_t* base_command_buffer,
    iree_hal_execution_stage_t source_stage_mask,
    iree_hal_execution_stage_t target_stage_mask,
    iree_hal_execution_barrier_flags_t flags,
    iree_host_size_t memory_barrier_count,
    const iree_hal_memory_barrier_t* memory_barriers,
    iree_host_size_t buffer_barrier_count,
    const iree_hal_buffer_barrier_t* buffer_barriers);
static iree_status_t iree_hal_accel_command_buffer_signal_event(
    iree_hal_command_buffer_t* base_command_buffer, iree_hal_event_t* event,
    iree_hal_execution_stage_t source_stage_mask);
static iree_status_t iree_hal_accel_command_buffer_reset_event(
    iree_hal_command_buffer_t* base_command_buffer, iree_hal_event_t* event,
    iree_hal_execution_stage_t source_stage_mask);
static iree_status_t iree_hal_accel_command_buffer_wait_events(
    iree_hal_command_buffer_t* base_command_buffer,
    iree_host_size_t event_count, const iree_hal_event_t** events,
    iree_hal_execution_stage_t source_stage_mask,
    iree_hal_execution_stage_t target_stage_mask,
    iree_host_size_t memory_barrier_count,
    const iree_hal_memory_barrier_t* memory_barriers,
    iree_host_size_t buffer_barrier_count,
    const iree_hal_buffer_barrier_t* buffer_barriers);
static iree_status_t iree_hal_accel_command_buffer_advise_buffer(
    iree_hal_command_buffer_t* base_command_buffer,
    iree_hal_buffer_ref_t buffer_ref, iree_hal_memory_advise_flags_t flags,
    uint64_t arg0, uint64_t arg1);
static iree_status_t iree_hal_accel_command_buffer_fill_buffer(
    iree_hal_command_buffer_t* base_command_buffer,
    iree_hal_buffer_ref_t target_ref, const void* pattern,
    iree_host_size_t pattern_length, iree_hal_fill_flags_t flags);
static iree_status_t iree_hal_accel_command_buffer_update_buffer(
    iree_hal_command_buffer_t* base_command_buffer, const void* source_buffer,
    iree_host_size_t source_offset, iree_hal_buffer_ref_t target_ref,
    iree_hal_update_flags_t flags);
static iree_status_t iree_hal_accel_command_buffer_copy_buffer(
    iree_hal_command_buffer_t* base_command_buffer,
    iree_hal_buffer_ref_t source_ref, iree_hal_buffer_ref_t target_ref,
    iree_hal_copy_flags_t flags);
static iree_status_t iree_hal_accel_command_buffer_collective(
    iree_hal_command_buffer_t* base_command_buffer, iree_hal_channel_t* channel,
    iree_hal_collective_op_t op, uint32_t param, iree_hal_buffer_ref_t send_ref,
    iree_hal_buffer_ref_t recv_ref, iree_device_size_t element_count);
static iree_status_t iree_hal_accel_command_buffer_dispatch(
    iree_hal_command_buffer_t* base_command_buffer,
    iree_hal_executable_t* executable, int32_t entry_point,
    const iree_hal_dispatch_config_t config, iree_const_byte_span_t constants,
    iree_hal_buffer_ref_list_t bindings, iree_hal_dispatch_flags_t flags);

// ============================================================================
// Name parsing helpers
// ============================================================================
static iree_status_t parse_strides_from_name(iree_string_view_t name,
                                             uint32_t* out_stride_h,
                                             uint32_t* out_stride_w) {
  iree_string_view_t strides_str;
  if (!iree_string_view_split(name, 's', NULL, &strides_str)) {
    return iree_ok_status();
  }
  iree_string_view_t stride_h_str, stride_w_str;
  if (!iree_string_view_split(strides_str, 'x', &stride_h_str, &stride_w_str)) {
    return iree_make_status(IREE_STATUS_INVALID_ARGUMENT, "invalid stride");
  }
  if (!iree_string_view_atoi_uint32(stride_h_str, out_stride_h) ||
      !iree_string_view_atoi_uint32(stride_w_str, out_stride_w)) {
    return iree_make_status(IREE_STATUS_INVALID_ARGUMENT, "invalid stride");
  }
  return iree_ok_status();
}

static iree_status_t parse_kernel_size_from_name(iree_string_view_t name,
                                                 uint32_t* out_kh,
                                                 uint32_t* out_kw) {
  iree_string_view_t remainder;
  if (!iree_string_view_split(name, 'k', NULL, &remainder)) {
    *out_kh = 1;
    *out_kw = 1;
    return iree_ok_status();
  }
  iree_string_view_t kernel_str;
  iree_string_view_split(remainder, 's', &kernel_str, &remainder);

  iree_string_view_t kh_str, kw_str;
  if (!iree_string_view_split(kernel_str, 'x', &kh_str, &kw_str)) {
    return iree_make_status(IREE_STATUS_INVALID_ARGUMENT, "invalid kernel size format, expected kKxK");
  }
  if (!iree_string_view_atoi_uint32(kh_str, out_kh) ||
      !iree_string_view_atoi_uint32(kw_str, out_kw)) {
    return iree_make_status(IREE_STATUS_INVALID_ARGUMENT, "invalid kernel size value");
  }
  return iree_ok_status();
}

static iree_status_t parse_padding_from_name(iree_string_view_t name,
                                             uint32_t* out_pad_h,
                                             uint32_t* out_pad_w) {
  iree_string_view_t remainder;
  if (!iree_string_view_split(name, 'p', NULL, &remainder)) {
    *out_pad_h = 0;
    *out_pad_w = 0;
    return iree_ok_status();
  }
  iree_string_view_t padding_str;
  iree_string_view_split(remainder, 'k', &padding_str, &remainder);
  iree_string_view_split(padding_str, 's', &padding_str, &remainder);

  iree_string_view_t pad_h_str, pad_w_str;
  if (!iree_string_view_split(padding_str, 'x', &pad_h_str, &pad_w_str) ||
      !iree_string_view_atoi_uint32(pad_h_str, out_pad_h) ||
      !iree_string_view_atoi_uint32(pad_w_str, out_pad_w)) {
    return iree_make_status(IREE_STATUS_INVALID_ARGUMENT,
                            "invalid padding format, expected pPxP");
  }
  return iree_ok_status();
}

// ============================================================================
// Heuristics
// ============================================================================
static size_t pick_elem_size_u3(uint64_t a, uint64_t b, uint64_t c) {
  if ((a % 4 == 0) && (b % 4 == 0) && (c % 4 == 0)) return 4;
  if ((a % 2 == 0) && (b % 2 == 0) && (c % 2 == 0)) return 2;
  return 1;
}

static bool classify_matmul_with_elem(const iree_hal_buffer_ref_list_t* bindings, size_t elem) {
  if (bindings->count < 3) return false;
  iree_hal_buffer_ref_t lhs = bindings->values[0];
  iree_hal_buffer_ref_t rhs = bindings->values[1];
  // Bias is optional, so it might not be present.
  iree_hal_buffer_ref_t bias = bindings->count >= 4 ? bindings->values[2] : (iree_hal_buffer_ref_t){0};

  if (!lhs.buffer || !rhs.buffer || !bias.buffer) return false;
  uint64_t lhs_b = lhs.length, rhs_b = rhs.length, bias_b = bias.length;

  if (bias_b > 0) { // With bias
    if ((bias_b % elem) != 0) return false;
    uint64_t N = bias_b / elem; if (N == 0) return false;
    if ((rhs_b % (N * elem)) != 0) return false;
    uint64_t K = rhs_b / (N * elem); if (K == 0) return false;
    if ((lhs_b % (K * elem)) != 0) return false;
  } else { // Without bias (bindings->count == 3)
    // This case is ambiguous. Let's assume it's matmul if it's not conv-like.
    // A simple heuristic: if RHS is much smaller than LHS, it's likely not matmul.
    if (rhs_b > 0 && lhs_b > rhs_b * 16) return false;
  }
  return true;
}

static bool try_classify_matmul(const iree_hal_buffer_ref_list_t* bindings) {
  if (bindings->count < 3) return false;
  uint64_t a = bindings->values[0].length;
  uint64_t b = bindings->values[1].length;
  uint64_t c = bindings->values[2].length;
  size_t elem = pick_elem_size_u3(a, b, c);
  return classify_matmul_with_elem(bindings, elem);
}

static bool try_classify_conv_like_4(const iree_hal_buffer_ref_list_t* bindings) {
  if (bindings->count != 4) return false;
  const iree_hal_buffer_ref_t input  = bindings->values[0];
  const iree_hal_buffer_ref_t filter = bindings->values[1];
  const iree_hal_buffer_ref_t bias   = bindings->values[2];
  const iree_hal_buffer_ref_t output = bindings->values[3];
  if (!input.buffer || !filter.buffer || !bias.buffer || !output.buffer) return false;

  uint64_t in_len  = get_effective_len(input);
  uint64_t fil_len = get_effective_len(filter);
  // uint64_t bia_len = get_effective_len(bias);
  uint64_t out_len = get_effective_len(output);
  if (!in_len || !fil_len || !out_len) return false;

  // Conv의 일반적인 특징: 필터와 바이어스는 입력/출력보다 훨씬 작습니다.
  // Matmul의 경우, 두 입력(lhs, rhs)의 크기가 비슷하거나 서로 비례하는 경향이 있습니다.
  bool filter_is_small = (filter.length * 16 < input.length);
  bool bias_is_tiny = (bias.length * 64 < input.length);
  if (filter_is_small && bias_is_tiny) return true;
  return false;
}

// ============================================================================
// Heuristics
// ============================================================================
static bool try_classify_conv_like_3(const iree_hal_buffer_ref_list_t* bindings) {
  if (bindings->count != 3) return false;
  const iree_hal_buffer_ref_t in  = bindings->values[0];
  const iree_hal_buffer_ref_t fil = bindings->values[1];
  const iree_hal_buffer_ref_t o3  = bindings->values[2];
  if (!in.buffer || !fil.buffer || !o3.buffer) return false;

  uint64_t L0 = in.length, L1 = fil.length, L2 = o3.length;

  // 바이어스 후보가 아주 작음(둘 중 하나만 만족해도 true)
  bool bias_is_tiny = (L0 > 0 && L2 * 64 <= L0) || (L1 > 0 && L2 * 64 <= L1);

  // 필터는 입력보다 '상당히' 작음: 4배 → 2배로 완화
  bool filter_much_smaller_than_input = (L0 > 0 && L1 * 2 <= L0);

  // 출력 크기는 입력과 동일하거나 절반 이상 비슷(여유 있게)
  bool output_like_in = (L0 > 0) && (L2 == L0 || L2 >= (L0 / 2));

  bool patternA = filter_much_smaller_than_input && output_like_in && !bias_is_tiny;
  bool patternB = bias_is_tiny; // (input, filter, bias) → in-place

  return patternA || patternB;
}


static bool try_classify_conv_like(const iree_hal_buffer_ref_list_t* bindings) {
  return try_classify_conv_like_4(bindings) || try_classify_conv_like_3(bindings);
}

static bool check_offload_target_attr(iree_hal_executable_t* executable,
                                      int32_t entry_point,
                                      iree_string_view_t target_name) {
  if (!iree_hal_accel_executable_isa(executable)) return false;
  iree_hal_accel_executable_t* accel_executable =
      (iree_hal_accel_executable_t*)executable;
  if (entry_point < 0 || (uint32_t)entry_point >= accel_executable->entry_fn_count) {
    return false;
  }
  iree_vm_function_t entry_fn;
  iree_status_t status = iree_vm_module_lookup_function_by_ordinal(
      accel_executable->bytecode_module, IREE_VM_FUNCTION_LINKAGE_EXPORT,
      accel_executable->entry_fn_ordinals[entry_point], &entry_fn);
  if (!iree_status_is_ok(status)) return false;
  iree_string_view_t attr = iree_vm_function_lookup_attr_by_name(&entry_fn, iree_make_cstring_view("offload.target"));
  return iree_string_view_equal(attr, target_name);
}
// ============================================================================
// Command buffer state/structs
// ============================================================================
typedef struct iree_hal_accel_cmd_conv_t {
  iree_task_call_t offload_task;   // 오프로드 호출
  iree_task_call_t wait_task;      // 완료 대기 호출
  iree_string_view_t entry_point_name;
  iree_hal_buffer_ref_t bindings[4]; // 0:in,1:filter,2:bias(or null),3:out
  uint32_t workgroup_count[3];

  // offload에서 계산하여 wait로 넘길 파라미터
  uint32_t N, OH, OW, OC;
  bool issued; 
  bool force_skip;
  // If bias is not provided, this will point to a malloc'd zero buffer.
  uint8_t* zero_bias;
} iree_hal_accel_cmd_conv_t;

typedef struct iree_hal_accel_cmd_conv_relu_wait_t {
  iree_task_call_t task;
  iree_hal_buffer_ref_t output_binding;
  uint32_t N, OH, OW, OC;
} iree_hal_accel_cmd_conv_relu_wait_t;

typedef struct iree_hal_accel_cmd_matmul_wait_t {
  iree_task_call_t task;
  iree_hal_buffer_ref_t output_binding;
  uint32_t M, N;
} iree_hal_accel_cmd_matmul_wait_t;

typedef struct iree_hal_accel_command_buffer_t {
  iree_hal_command_buffer_t base;
  iree_allocator_t host_allocator;
  iree_task_scope_t* scope;
  iree_arena_allocator_t arena;
  iree_hal_resource_set_t* resource_set;
  iree_task_list_t root_tasks;
  iree_task_list_t leaf_tasks;
  struct {
    iree_task_barrier_t* open_barrier;
    iree_host_size_t open_task_count;
    iree_task_list_t open_tasks;
  } state;
} iree_hal_accel_command_buffer_t;

typedef struct iree_hal_accel_cmd_matmul_t {
  iree_task_call_t task;
  iree_hal_buffer_ref_t bindings[4];
  uint8_t* zero_bias; // For bias-less matmul
  iree_hal_accel_cmd_matmul_wait_t wait_cmd;
} iree_hal_accel_cmd_matmul_t;

typedef struct iree_hal_accel_cmd_conv_relu_t {
  iree_task_call_t task;
  iree_string_view_t entry_point_name;
  iree_hal_buffer_ref_t bindings[4];
  uint32_t workgroup_count[3];
  iree_hal_accel_cmd_conv_relu_wait_t wait_cmd;
} iree_hal_accel_cmd_conv_relu_t;

static const iree_hal_command_buffer_vtable_t
    iree_hal_accel_command_buffer_vtable;

static iree_hal_accel_command_buffer_t* iree_hal_accel_command_buffer_cast(
    iree_hal_command_buffer_t* base_value) {
  IREE_HAL_ASSERT_TYPE(base_value, &iree_hal_accel_command_buffer_vtable);
  return (iree_hal_accel_command_buffer_t*)base_value;
}

// ============================================================================
// Conv(offload)
// ============================================================================
static iree_status_t iree_hal_accel_cmd_conv_offload(
    void* user_context, iree_task_t* task,
    iree_task_submission_t* pending_submission) {
  iree_hal_accel_cmd_conv_t* cmd =
      (iree_hal_accel_cmd_conv_t*)user_context;
  IREE_TRACE_ZONE_BEGIN(z0);
  MP_SCOPE("OFFLOAD_CALL");

  iree_status_t status = iree_ok_status();
  iree_hal_buffer_mapping_t input_mapping, filter_mapping, bias_mapping, output_mapping;
  memset(&input_mapping, 0, sizeof(input_mapping));
  memset(&filter_mapping, 0, sizeof(filter_mapping));
  memset(&bias_mapping, 0, sizeof(bias_mapping));
  memset(&output_mapping, 0, sizeof(output_mapping));

  // 안전 가드
  if (IREE_UNLIKELY(!cmd->bindings[0].buffer || !cmd->bindings[1].buffer || !cmd->bindings[3].buffer)) {
    IREE_TRACE_ZONE_END(z0);
    return iree_make_status(IREE_STATUS_FAILED_PRECONDITION,
                            "conv offload: NULL buffer (in/filter/out)");
  }

  // 맵
  status = iree_hal_buffer_map_range(cmd->bindings[0].buffer, IREE_HAL_MAPPING_MODE_SCOPED,
                                     IREE_HAL_MEMORY_ACCESS_READ, 0, IREE_HAL_WHOLE_BUFFER, &input_mapping);
  if (!iree_status_is_ok(status)) goto conv_cleanup;

  status = iree_hal_buffer_map_range(cmd->bindings[1].buffer, IREE_HAL_MAPPING_MODE_SCOPED,
                                     IREE_HAL_MEMORY_ACCESS_READ, 0, IREE_HAL_WHOLE_BUFFER, &filter_mapping);
  if (!iree_status_is_ok(status)) goto conv_cleanup;

  if (cmd->bindings[2].buffer) {
    status = iree_hal_buffer_map_range(cmd->bindings[2].buffer, IREE_HAL_MAPPING_MODE_SCOPED,
                                       IREE_HAL_MEMORY_ACCESS_READ, 0, IREE_HAL_WHOLE_BUFFER, &bias_mapping);
    if (!iree_status_is_ok(status)) goto conv_cleanup;
  }

  status = iree_hal_buffer_map_range(cmd->bindings[3].buffer, IREE_HAL_MAPPING_MODE_SCOPED,
                                     IREE_HAL_MEMORY_ACCESS_DISCARD_WRITE, 0, IREE_HAL_WHOLE_BUFFER, &output_mapping);
  if (!iree_status_is_ok(status)) goto conv_cleanup;

  // elem 크기 추론
  size_t elem_size = 1;
  uint64_t fl = filter_mapping.contents.data_length;
  uint64_t bl = cmd->bindings[2].buffer ? bias_mapping.contents.data_length : 0;
  uint64_t il = input_mapping.contents.data_length;
  uint64_t ol = output_mapping.contents.data_length;
  if (bl) elem_size = pick_elem_size_u3(fl, bl, fl);
  else if (il && fl && ol) elem_size = pick_elem_size_u3(il, fl, ol);
  else elem_size = (fl % 4 == 0) ? 4 : ((fl % 2 == 0) ? 2 : 1);

  // 출력(OH,OW,N) → OC
  uint32_t N  = cmd->workgroup_count[0];
  uint32_t OH = cmd->workgroup_count[1];
  uint32_t OW = cmd->workgroup_count[2];

  uint32_t OC = 0;
  if (bl > 0) {
    if (bl % elem_size) { status = iree_make_status(IREE_STATUS_INVALID_ARGUMENT, "bias len misaligned"); goto conv_cleanup; }
    OC = (uint32_t)(bl / elem_size);
  } else {
    if (!ol || !elem_size || !N || !OH || !OW) { status = iree_make_status(IREE_STATUS_INVALID_ARGUMENT, "cannot infer OC"); goto conv_cleanup; }
    uint64_t total = ol / elem_size, spatial = (uint64_t)N * OH * OW;
    if (!spatial || (total % spatial)) { status = iree_make_status(IREE_STATUS_INVALID_ARGUMENT, "output not divisible by N*OH*OW"); goto conv_cleanup; }
    OC = (uint32_t)(total / spatial);
  }

  // stride/pad/kernel
  uint32_t stride_h = 1, stride_w = 1, pad_h = 0, pad_w = 0, KH = 1, KW = 1;
  (void)parse_strides_from_name(cmd->entry_point_name, &stride_h, &stride_w);
  (void)parse_padding_from_name(cmd->entry_point_name, &pad_h, &pad_w);
  bool kernel_parsed = iree_status_is_ok(parse_kernel_size_from_name(cmd->entry_point_name, &KH, &KW));

  // C 계산 (fl = elem * KH * KW * C * OC)
  uint32_t C = 0;
  if (kernel_parsed && KH && KW && OC && elem_size) {
    uint64_t denom = (uint64_t)elem_size * KH * KW * OC;
    if (!denom || (fl % denom)) { status = iree_make_status(IREE_STATUS_INVALID_ARGUMENT, "filter len mismatch"); goto conv_cleanup; }
    C = (uint32_t)(fl / denom);
  } else {
    C = 3;
    if (C && OC && elem_size) {
    double kh2 = (double)fl / ((double)elem_size * C * OC);
      KH = KW = (uint32_t)sqrt(kh2);
    }
  }

  // 입력 H,W 역산 (dilation=1)
  uint32_t H = (OH - 1) * stride_h + KH - 2 * pad_h;
  uint32_t W = (OW - 1) * stride_w + KW - 2 * pad_w;

  // wait용 파라미터 보관
  cmd->N  = N;  cmd->OH = OH;  cmd->OW = OW;  cmd->OC = OC;

  // --- bias 포인터 준비: **항상 수명 보장** ---
  const uint8_t* bias_ptr = NULL;

  if (cmd->bindings[2].buffer) {
    // 외부 bias 사용. zero_bias는 반드시 NULL로.
    cmd->zero_bias = NULL;
    bias_ptr = (const uint8_t*)bias_mapping.contents.data;
  } else {
    // bias 없음: 힙에 0 초기화 버퍼를 만들고 wait에서 해제
    size_t need = (size_t)OC * elem_size;
    if (IREE_UNLIKELY(!need)) { status = iree_make_status(IREE_STATUS_INVALID_ARGUMENT, "zero-sized bias"); goto conv_cleanup; }
    cmd->zero_bias = (uint8_t*)malloc(need);
    if (IREE_UNLIKELY(!cmd->zero_bias)) { status = iree_make_status(IREE_STATUS_RESOURCE_EXHAUSTED, "malloc zero-bias"); goto conv_cleanup; }
    memset(cmd->zero_bias, 0, need);
    bias_ptr = cmd->zero_bias;
  }

  // DPI 호출 (비동기일 수 있으므로 bias 수명은 wait까지 유지)
  iree_dpi_sa16_conv_offload(
      (const uint8_t*)input_mapping.contents.data,
      (const uint8_t*)filter_mapping.contents.data,
      bias_ptr,
      N, H, W, C, OC, KH, KW, stride_h, stride_w, pad_h, pad_w);

conv_cleanup:
  if (input_mapping.buffer)  iree_hal_buffer_unmap_range(&input_mapping);
  if (filter_mapping.buffer) iree_hal_buffer_unmap_range(&filter_mapping);
  if (bias_mapping.buffer)   iree_hal_buffer_unmap_range(&bias_mapping);
  if (output_mapping.buffer) iree_hal_buffer_unmap_range(&output_mapping);
  IREE_TRACE_ZONE_END(z0);
  return status;
}

static iree_status_t iree_hal_accel_cmd_conv_wait(
    void* user_context, iree_task_t* task,
    iree_task_submission_t* pending_submission) {
  iree_hal_accel_cmd_conv_t* cmd = (iree_hal_accel_cmd_conv_t*)user_context;
  IREE_TRACE_ZONE_BEGIN(z0);
  MP_SCOPE("WAIT_CALL");

  iree_status_t status = iree_ok_status();
  iree_hal_buffer_mapping_t out = {{0}};

  if (!cmd->bindings[3].buffer) {
    IREE_TRACE_ZONE_END(z0);
    return iree_make_status(IREE_STATUS_FAILED_PRECONDITION, "conv wait: NULL out");
  }

  status = iree_hal_buffer_map_range(cmd->bindings[3].buffer, IREE_HAL_MAPPING_MODE_SCOPED,
                                     IREE_HAL_MEMORY_ACCESS_DISCARD_WRITE, 0, IREE_HAL_WHOLE_BUFFER, &out);
  if (!iree_status_is_ok(status)) goto done;

  iree_dpi_sa16_conv_wait_for_completion((uint8_t*)out.contents.data,
                                         cmd->N, cmd->OH, cmd->OW, cmd->OC);

done:
  if (out.buffer) iree_hal_buffer_unmap_range(&out);

  // 여기서만 zero-bias 해제
  if (cmd->zero_bias) {
    free(cmd->zero_bias);
    cmd->zero_bias = NULL;
  }

  IREE_TRACE_ZONE_END(z0);
  return status;
}

// ============================================================================
// Command buffer lifecycle & barriers
// ============================================================================
static iree_status_t iree_hal_accel_command_buffer_flush_tasks(
    iree_hal_accel_command_buffer_t* command_buffer);

iree_status_t iree_hal_accel_command_buffer_create(
    iree_hal_allocator_t* device_allocator, iree_task_scope_t* scope,
    iree_hal_command_buffer_mode_t mode,
    iree_hal_command_category_t command_categories,
    iree_hal_queue_affinity_t queue_affinity, iree_host_size_t binding_capacity,
    iree_arena_block_pool_t* block_pool, iree_allocator_t host_allocator,
    iree_hal_command_buffer_t** out_command_buffer) {
  IREE_ASSERT_ARGUMENT(out_command_buffer);
  *out_command_buffer = NULL;
  IREE_ASSERT_ARGUMENT(scope);

  IREE_TRACE_ZONE_BEGIN(z0);

  iree_hal_accel_command_buffer_t* command_buffer = NULL;
  iree_host_size_t validation_state_size =
      iree_hal_command_buffer_validation_state_size(mode, binding_capacity);
  iree_status_t status = iree_allocator_malloc(
      host_allocator,
      sizeof(*command_buffer) + validation_state_size,
      (void**)&command_buffer);
  if (iree_status_is_ok(status)) {
    void* validation_state = validation_state_size > 0
                                 ? (uint8_t*)command_buffer + sizeof(*command_buffer)
                                 : NULL;
    iree_hal_command_buffer_initialize(
        device_allocator, mode, command_categories, queue_affinity,
        binding_capacity, validation_state,
        &iree_hal_accel_command_buffer_vtable, &command_buffer->base);
    command_buffer->host_allocator = host_allocator;
    command_buffer->scope = scope;
    iree_arena_initialize(block_pool, &command_buffer->arena);
    iree_task_list_initialize(&command_buffer->root_tasks);
    iree_task_list_initialize(&command_buffer->leaf_tasks);
    memset(&command_buffer->state, 0, sizeof(command_buffer->state));
    status = iree_hal_resource_set_allocate(block_pool,
                                            &command_buffer->resource_set);
  }
  if (iree_status_is_ok(status)) {
    *out_command_buffer = &command_buffer->base;
  } else {
    iree_hal_command_buffer_release(&command_buffer->base);
  }

  IREE_TRACE_ZONE_END(z0);
  return status;
}

static void iree_hal_accel_command_buffer_destroy(
    iree_hal_command_buffer_t* base_command_buffer) {
  iree_hal_accel_command_buffer_t* command_buffer =
      iree_hal_accel_command_buffer_cast(base_command_buffer);
  iree_allocator_t host_allocator = command_buffer->host_allocator;
  IREE_TRACE_ZONE_BEGIN(z0);

  memset(&command_buffer->state, 0, sizeof(command_buffer->state));
  iree_task_list_discard(&command_buffer->root_tasks);
  iree_task_list_discard(&command_buffer->leaf_tasks);
  iree_arena_deinitialize(&command_buffer->arena);
  iree_hal_resource_set_free(command_buffer->resource_set);
  iree_allocator_free(host_allocator, command_buffer);

  IREE_TRACE_ZONE_END(z0);
}

bool iree_hal_accel_command_buffer_isa(
    iree_hal_command_buffer_t* command_buffer) {
  return iree_hal_resource_is(&command_buffer->resource,
                              &iree_hal_accel_command_buffer_vtable);
}

static iree_status_t iree_hal_accel_command_buffer_begin(
    iree_hal_command_buffer_t* base_command_buffer) {
  iree_hal_accel_command_buffer_t* command_buffer =
      iree_hal_accel_command_buffer_cast(base_command_buffer);

  if (!iree_all_bits_set(command_buffer->base.mode,
                         IREE_HAL_COMMAND_BUFFER_MODE_ONE_SHOT)) {
    memset(&command_buffer->state, 0, sizeof(command_buffer->state));
    iree_task_list_discard(&command_buffer->root_tasks);
    iree_task_list_discard(&command_buffer->leaf_tasks);
    iree_arena_reset(&command_buffer->arena);
    iree_hal_resource_set_free(command_buffer->resource_set);
    iree_status_t status = iree_hal_resource_set_allocate(
        command_buffer->arena.block_pool,
        &command_buffer->resource_set);
    IREE_RETURN_IF_ERROR(status, "re-allocating resource set");
  }

  if (!iree_task_list_is_empty(&command_buffer->root_tasks)) {
    return iree_make_status(IREE_STATUS_FAILED_PRECONDITION,
                            "command buffer cannot be re-recorded");
  }
  return iree_ok_status();
}

static iree_status_t iree_hal_accel_command_buffer_end(
    iree_hal_command_buffer_t* base_command_buffer) {
  iree_hal_accel_command_buffer_t* command_buffer =
      iree_hal_accel_command_buffer_cast(base_command_buffer);

  IREE_RETURN_IF_ERROR(
      iree_hal_accel_command_buffer_flush_tasks(command_buffer));

  if (iree_task_list_is_empty(&command_buffer->root_tasks) &&
      !iree_task_list_is_empty(&command_buffer->leaf_tasks)) {
    iree_task_list_move(&command_buffer->leaf_tasks,
                        &command_buffer->root_tasks);
  }

  iree_hal_resource_set_freeze(command_buffer->resource_set);

  return iree_ok_status();
}

static iree_status_t iree_hal_accel_command_buffer_flush_tasks(
    iree_hal_accel_command_buffer_t* command_buffer) {
  iree_task_barrier_t* open_barrier = command_buffer->state.open_barrier;
  if (open_barrier != NULL) {
    iree_task_t* task_head =
        iree_task_list_front(&command_buffer->state.open_tasks);
    iree_host_size_t dependent_task_count =
        command_buffer->state.open_task_count;
    if (dependent_task_count == 1) {
      iree_task_set_completion_task(&open_barrier->header, task_head);
    } else if (dependent_task_count > 1) {
      iree_task_t** dependent_tasks = NULL;
      IREE_RETURN_IF_ERROR(iree_arena_allocate(
          &command_buffer->arena, dependent_task_count * sizeof(iree_task_t*),
          (void**)&dependent_tasks));
      iree_task_t* task = task_head;
      for (iree_host_size_t i = 0; i < dependent_task_count; ++i) {
        dependent_tasks[i] = task;
        task = task->next_task;
      }
      iree_task_barrier_set_dependent_tasks(open_barrier, dependent_task_count,
                                            dependent_tasks);
    }
  }
  command_buffer->state.open_barrier = NULL;

  if (command_buffer->state.open_task_count > 0) {
    iree_task_list_move(&command_buffer->state.open_tasks,
                        &command_buffer->leaf_tasks);
    command_buffer->state.open_task_count = 0;
  }

  return iree_ok_status();
}

static iree_status_t iree_hal_accel_command_buffer_emit_global_barrier(
    iree_hal_accel_command_buffer_t* command_buffer) {
  IREE_RETURN_IF_ERROR(
      iree_hal_accel_command_buffer_flush_tasks(command_buffer));

  iree_task_barrier_t* barrier = NULL;
  IREE_RETURN_IF_ERROR(iree_arena_allocate(&command_buffer->arena,
                                           sizeof(*barrier), (void**)&barrier));
  iree_task_barrier_initialize_empty(command_buffer->scope, barrier);

  for (iree_task_t* task = iree_task_list_front(&command_buffer->leaf_tasks);
       task != NULL; task = task->next_task) {
    iree_task_set_completion_task(task, &barrier->header);
  }

  if (iree_task_list_is_empty(&command_buffer->root_tasks) &&
      !iree_task_list_is_empty(&command_buffer->leaf_tasks)) {
    iree_task_list_move(&command_buffer->leaf_tasks,
                        &command_buffer->root_tasks);
  }

  iree_task_list_t* target_task_list =
      iree_task_list_is_empty(&command_buffer->root_tasks)
          ? &command_buffer->root_tasks
          : &command_buffer->leaf_tasks;
  iree_task_list_initialize(target_task_list);
  iree_task_list_push_back(target_task_list, &barrier->header);

  command_buffer->state.open_barrier = barrier;
  command_buffer->state.open_task_count = 0;

  return iree_ok_status();
}

static iree_status_t iree_hal_accel_command_buffer_begin_debug_group(
    iree_hal_command_buffer_t* base_command_buffer, iree_string_view_t label,
    iree_hal_label_color_t label_color,
    const iree_hal_label_location_t* location) {
  return iree_ok_status();
}

static iree_status_t iree_hal_accel_command_buffer_end_debug_group(
    iree_hal_command_buffer_t* base_command_buffer) {
  return iree_ok_status();
}

static iree_status_t iree_hal_accel_command_buffer_execution_barrier(
    iree_hal_command_buffer_t* base_command_buffer,
    iree_hal_execution_stage_t source_stage_mask,
    iree_hal_execution_stage_t target_stage_mask,
    iree_hal_execution_barrier_flags_t flags,
    iree_host_size_t memory_barrier_count,
    const iree_hal_memory_barrier_t* memory_barriers,
    iree_host_size_t buffer_barrier_count,
    const iree_hal_buffer_barrier_t* buffer_barriers) {
  iree_hal_accel_command_buffer_t* command_buffer =
      iree_hal_accel_command_buffer_cast(base_command_buffer);
  return iree_hal_accel_command_buffer_emit_global_barrier(command_buffer);
}

static iree_status_t iree_hal_accel_command_buffer_signal_event(
    iree_hal_command_buffer_t* base_command_buffer, iree_hal_event_t* event,
    iree_hal_execution_stage_t source_stage_mask) {
  return iree_ok_status();
}

static iree_status_t iree_hal_accel_command_buffer_reset_event(
    iree_hal_command_buffer_t* base_command_buffer, iree_hal_event_t* event,
    iree_hal_execution_stage_t source_stage_mask) {
  return iree_ok_status();
}

static iree_status_t iree_hal_accel_command_buffer_wait_events(
    iree_hal_command_buffer_t* base_command_buffer,
    iree_host_size_t event_count, const iree_hal_event_t** events,
    iree_hal_execution_stage_t source_stage_mask,
    iree_hal_execution_stage_t target_stage_mask,
    iree_host_size_t memory_barrier_count,
    const iree_hal_memory_barrier_t* memory_barriers,
    iree_host_size_t buffer_barrier_count,
    const iree_hal_buffer_barrier_t* buffer_barriers) {
  iree_hal_accel_command_buffer_t* command_buffer =
      iree_hal_accel_command_buffer_cast(base_command_buffer);
  return iree_hal_accel_command_buffer_emit_global_barrier(command_buffer);
}

static iree_status_t iree_hal_accel_command_buffer_advise_buffer(
    iree_hal_command_buffer_t* base_command_buffer,
    iree_hal_buffer_ref_t buffer_ref, iree_hal_memory_advise_flags_t flags,
    uint64_t arg0, uint64_t arg1) {
  return iree_ok_status();
}

// ============================================================================
// Fill/update/copy
// ============================================================================
#define IREE_HAL_ACCEL_CMD_FILL_SLICE_LENGTH (128 * 1024)

typedef struct iree_hal_accel_cmd_fill_buffer_t {
  iree_task_dispatch_t task;
  iree_hal_buffer_ref_t target_ref;
  uint32_t pattern_length;
  uint8_t pattern[8];
} iree_hal_accel_cmd_fill_buffer_t;

static iree_status_t iree_hal_accel_cmd_fill_tile(
    void* user_context, const iree_task_tile_context_t* tile_context,
    iree_task_submission_t* pending_submission) {
  const iree_hal_accel_cmd_fill_buffer_t* cmd =
      (const iree_hal_accel_cmd_fill_buffer_t*)user_context;
  IREE_TRACE_ZONE_BEGIN(z0);

  // --- NEW: fast guards ---
  if (IREE_UNLIKELY(!cmd->target_ref.buffer)) {
    IREE_TRACE_ZONE_END(z0);
    return iree_make_status(IREE_STATUS_INVALID_ARGUMENT, "fill target buffer is NULL");
  }
  if (IREE_UNLIKELY(cmd->pattern_length != 1 &&
                    cmd->pattern_length != 2 &&
                    cmd->pattern_length != 4)) {
    IREE_TRACE_ZONE_END(z0);
    return iree_make_status(IREE_STATUS_INVALID_ARGUMENT, "pattern_length must be 1,2,4");
  }
  if (IREE_UNLIKELY(cmd->target_ref.length == 0)) {
    IREE_TRACE_ZONE_END(z0);
    return iree_ok_status(); // no-op
  }

  iree_device_size_t length_per_slice = tile_context->workgroup_size[0];
  iree_device_size_t slice_offset =
      (iree_device_size_t)tile_context->workgroup_xyz[0] * length_per_slice;
  if (IREE_UNLIKELY(slice_offset >= cmd->target_ref.length)) {
    IREE_TRACE_ZONE_END(z0);
    return iree_ok_status(); // nothing to do
  }
  iree_device_size_t remaining_length = cmd->target_ref.length - slice_offset;
  iree_device_size_t slice_length = iree_min(length_per_slice, remaining_length);

  iree_status_t status = iree_hal_buffer_map_fill(
      cmd->target_ref.buffer, cmd->target_ref.offset + slice_offset,
      slice_length, cmd->pattern, cmd->pattern_length);

  IREE_TRACE_ZONE_END(z0);
  return status;
}

static iree_status_t iree_hal_accel_command_buffer_fill_buffer(
    iree_hal_command_buffer_t* base_command_buffer,
    iree_hal_buffer_ref_t target_ref, const void* pattern,
    iree_host_size_t pattern_length, iree_hal_fill_flags_t flags) {
  iree_hal_accel_command_buffer_t* command_buffer =
      iree_hal_accel_command_buffer_cast(base_command_buffer);

  IREE_RETURN_IF_ERROR(iree_hal_resource_set_insert(
      command_buffer->resource_set, 1, &target_ref.buffer));

  iree_hal_accel_cmd_fill_buffer_t* cmd = NULL;
  IREE_RETURN_IF_ERROR(
      iree_arena_allocate(&command_buffer->arena, sizeof(*cmd), (void**)&cmd));

  const uint32_t workgroup_size[3] = { IREE_HAL_ACCEL_CMD_FILL_SLICE_LENGTH, 1, 1 };
  const uint32_t workgroup_count[3] = {
      (uint32_t)iree_device_size_ceil_div(target_ref.length, workgroup_size[0]),
      1, 1 };
  iree_task_dispatch_initialize(
      command_buffer->scope,
      iree_task_make_dispatch_closure(iree_hal_accel_cmd_fill_tile, (void*)cmd),
      workgroup_size, workgroup_count, &cmd->task);
  cmd->target_ref = target_ref;
  memcpy(cmd->pattern, pattern, pattern_length);
  cmd->pattern_length = (uint32_t)pattern_length;

  return iree_hal_accel_command_buffer_emit_execution_task(command_buffer,
                                                          &cmd->task.header);
}

typedef struct iree_hal_accel_cmd_update_buffer_t {
  iree_task_call_t task;
  iree_hal_buffer_ref_t target_ref;
  uint8_t source_buffer[];
} iree_hal_accel_cmd_update_buffer_t;

static iree_status_t iree_hal_accel_cmd_update_buffer(
    void* user_context, iree_task_t* task,
    iree_task_submission_t* pending_submission) {
  const iree_hal_accel_cmd_update_buffer_t* cmd =
      (const iree_hal_accel_cmd_update_buffer_t*)user_context;
  IREE_TRACE_ZONE_BEGIN(z0);
  iree_status_t status =
      iree_hal_buffer_map_write(cmd->target_ref.buffer, cmd->target_ref.offset,
                                cmd->source_buffer, cmd->target_ref.length);
  IREE_TRACE_ZONE_END(z0);
  return status;
}

static iree_status_t iree_hal_accel_command_buffer_update_buffer(
    iree_hal_command_buffer_t* base_command_buffer, const void* source_buffer,
    iree_host_size_t source_offset, iree_hal_buffer_ref_t target_ref,
    iree_hal_update_flags_t flags) {
  iree_hal_accel_command_buffer_t* command_buffer =
      iree_hal_accel_command_buffer_cast(base_command_buffer);

  IREE_RETURN_IF_ERROR(iree_hal_resource_set_insert(
      command_buffer->resource_set, 1, &target_ref.buffer));

  iree_host_size_t total_cmd_size =
      sizeof(iree_hal_accel_cmd_update_buffer_t) + target_ref.length;

  iree_hal_accel_cmd_update_buffer_t* cmd = NULL;
  IREE_RETURN_IF_ERROR(iree_arena_allocate(&command_buffer->arena,
                                           total_cmd_size, (void**)&cmd));

  iree_task_call_initialize(
      command_buffer->scope,
      iree_task_make_call_closure(iree_hal_accel_cmd_update_buffer, (void*)cmd),
      &cmd->task);
  cmd->target_ref = target_ref;
  memcpy(cmd->source_buffer, (const uint8_t*)source_buffer + source_offset,
         cmd->target_ref.length);

  return iree_hal_accel_command_buffer_emit_execution_task(command_buffer,
                                                          &cmd->task.header);
}

#define IREE_HAL_ACCEL_CMD_COPY_SLICE_LENGTH (128 * 1024)

typedef struct iree_hal_accel_cmd_copy_buffer_t {
  iree_task_dispatch_t task;
  iree_hal_buffer_ref_t source_ref;
  iree_hal_buffer_ref_t target_ref;
} iree_hal_accel_cmd_copy_buffer_t;

static iree_status_t iree_hal_accel_cmd_copy_tile(
    void* user_context, const iree_task_tile_context_t* tile_context,
    iree_task_submission_t* pending_submission) {
  const iree_hal_accel_cmd_copy_buffer_t* cmd =
      (const iree_hal_accel_cmd_copy_buffer_t*)user_context;
  IREE_TRACE_ZONE_BEGIN(z0);

  iree_device_size_t length_per_slice = tile_context->workgroup_size[0];
  iree_device_size_t slice_offset =
      (iree_device_size_t)tile_context->workgroup_xyz[0] * length_per_slice;
  iree_device_size_t remaining_length = cmd->target_ref.length - slice_offset;
  iree_device_size_t slice_length =
      iree_min(length_per_slice, remaining_length);
  IREE_TRACE_ZONE_APPEND_VALUE_I64(z0, (uint64_t)slice_length);

  iree_status_t status = iree_hal_buffer_map_copy(
      cmd->source_ref.buffer, cmd->source_ref.offset + slice_offset,
      cmd->target_ref.buffer, cmd->target_ref.offset + slice_offset,
      slice_length);

  IREE_TRACE_ZONE_END(z0);
  return status;
}

static iree_status_t iree_hal_accel_command_buffer_copy_buffer(
    iree_hal_command_buffer_t* base_command_buffer,
    iree_hal_buffer_ref_t source_ref, iree_hal_buffer_ref_t target_ref,
    iree_hal_copy_flags_t flags) {
  iree_hal_accel_command_buffer_t* command_buffer =
      iree_hal_accel_command_buffer_cast(base_command_buffer);

  const iree_hal_resource_t* buffers[] = {
      (const iree_hal_resource_t*)source_ref.buffer,
      (const iree_hal_resource_t*)target_ref.buffer,
  };
  IREE_RETURN_IF_ERROR(iree_hal_resource_set_insert(
      command_buffer->resource_set, IREE_ARRAYSIZE(buffers),
      (const iree_hal_resource_t* const*)buffers));

  iree_hal_accel_cmd_copy_buffer_t* cmd = NULL;
  IREE_RETURN_IF_ERROR(
      iree_arena_allocate(&command_buffer->arena, sizeof(*cmd), (void**)&cmd));

  const uint32_t workgroup_size[3] = { IREE_HAL_ACCEL_CMD_COPY_SLICE_LENGTH, 1, 1 };
  const uint32_t workgroup_count[3] = {
      (uint32_t)iree_device_size_ceil_div(target_ref.length, workgroup_size[0]),
      1, 1 };
  iree_task_dispatch_initialize(
      command_buffer->scope,
      iree_task_make_dispatch_closure(iree_hal_accel_cmd_copy_tile, (void*)cmd),
      workgroup_size, workgroup_count, &cmd->task);
  cmd->source_ref = source_ref;
  cmd->target_ref = target_ref;

  return iree_hal_accel_command_buffer_emit_execution_task(command_buffer,
                                                          &cmd->task.header);
}

static iree_status_t iree_hal_accel_command_buffer_collective(
    iree_hal_command_buffer_t* base_command_buffer, iree_hal_channel_t* channel,
    iree_hal_collective_op_t op, uint32_t param, iree_hal_buffer_ref_t send_ref,
    iree_hal_buffer_ref_t recv_ref, iree_device_size_t element_count) {
  return iree_make_status(IREE_STATUS_UNIMPLEMENTED,
                          "collectives not yet implemented on the task system");
}

// ============================================================================
/* CPU fallback dispatch scaffolding */
typedef struct iree_hal_accel_cmd_dispatch_t {
  iree_task_dispatch_t task;
  iree_hal_local_executable_t* executable;
  int32_t ordinal;
  iree_string_view_t entry_point_name;
  uint16_t constant_count;
  uint16_t binding_count;
} iree_hal_accel_cmd_dispatch_t;

static iree_status_t iree_hal_accel_cmd_dispatch_tile(
    void* user_context, const iree_task_tile_context_t* tile_context,
    iree_task_submission_t* pending_submission) {
  const iree_hal_accel_cmd_dispatch_t* cmd =
      (const iree_hal_accel_cmd_dispatch_t*)user_context;
  IREE_TRACE_ZONE_BEGIN(z0);

  iree_alignas(64) iree_hal_executable_dispatch_state_v0_t dispatch_state = {
      .workgroup_size_x = tile_context->workgroup_size[0],
      .workgroup_size_y = tile_context->workgroup_size[1],
      .workgroup_size_z = tile_context->workgroup_size[2],
      .constant_count = cmd->constant_count,
      .workgroup_count_x = tile_context->workgroup_count[0],
      .workgroup_count_y = tile_context->workgroup_count[1],
      .workgroup_count_z = tile_context->workgroup_count[2],
      .max_concurrency =
          iree_task_affinity_set_count_ones(cmd->task.header.affinity_set),
      .binding_count = cmd->binding_count,
  };
  uint8_t* cmd_ptr = (uint8_t*)cmd + sizeof(*cmd);
  dispatch_state.constants = (uint32_t*)cmd_ptr;
  cmd_ptr += cmd->constant_count * sizeof(*dispatch_state.constants);
  dispatch_state.binding_ptrs = (void**)cmd_ptr;
  cmd_ptr += cmd->binding_count * sizeof(*dispatch_state.binding_ptrs);
  dispatch_state.binding_lengths = (size_t*)cmd_ptr;
  cmd_ptr += cmd->binding_count * sizeof(*dispatch_state.binding_lengths);

  const iree_alignas(64)
      iree_hal_executable_workgroup_state_v0_t workgroup_state = {
          .workgroup_id_x = tile_context->workgroup_xyz[0],
          .workgroup_id_y = tile_context->workgroup_xyz[1],
          .workgroup_id_z = tile_context->workgroup_xyz[2],
          .reserved = 0,
          .processor_id = tile_context->processor_id,
          .local_memory = tile_context->local_memory.data,
          .local_memory_size = (size_t)tile_context->local_memory.data_length,
      };
  iree_status_t status = iree_hal_local_executable_issue_call(
      cmd->executable, cmd->ordinal, &dispatch_state, &workgroup_state,
      tile_context->worker_id);

  IREE_TRACE_ZONE_END(z0);
  return status;
}

// ============================================================================
// Matmul/Conv+ReLU offload
// ============================================================================
static iree_status_t iree_hal_accel_cmd_matmul_offload(
    void* user_context, iree_task_t* task,
    iree_task_submission_t* pending_submission) {
  iree_hal_accel_cmd_matmul_t* cmd = (iree_hal_accel_cmd_matmul_t*)user_context;
  IREE_TRACE_ZONE_BEGIN(z0);
  MP_SCOPE("OFFLOAD_CALL");

  iree_status_t status = iree_ok_status();
  iree_hal_buffer_mapping_t lhs = {{0}}, rhs = {{0}}, bias = {{0}};

  status = iree_hal_buffer_map_range(cmd->bindings[0].buffer, IREE_HAL_MAPPING_MODE_SCOPED,
                                     IREE_HAL_MEMORY_ACCESS_READ, 0, IREE_HAL_WHOLE_BUFFER, &lhs);
  if (!iree_status_is_ok(status)) goto done;
  status = iree_hal_buffer_map_range(cmd->bindings[1].buffer, IREE_HAL_MAPPING_MODE_SCOPED,
                                     IREE_HAL_MEMORY_ACCESS_READ, 0, IREE_HAL_WHOLE_BUFFER, &rhs);
  if (!iree_status_is_ok(status)) goto done;

  const size_t elem = 4;
  size_t lhs_bytes = lhs.contents.data_length;
  size_t rhs_bytes = rhs.contents.data_length;

  const uint8_t* bias_ptr = NULL;
  size_t bias_bytes = 0;

  if (cmd->bindings[2].buffer) {
    status = iree_hal_buffer_map_range(cmd->bindings[2].buffer, IREE_HAL_MAPPING_MODE_SCOPED,
                                       IREE_HAL_MEMORY_ACCESS_READ, 0, IREE_HAL_WHOLE_BUFFER, &bias);
    if (!iree_status_is_ok(status)) goto done;
    bias_ptr = (const uint8_t*)bias.contents.data;
    bias_bytes = bias.contents.data_length;
    cmd->zero_bias = NULL;
  } else {
    // bias 없음 → K,N 먼저 추정 후 N*elem 크기로 zero-bias 확보
    // rhs_bytes = K * N * elem, lhs_bytes = M * K * elem
    // 가장 간단히: K는 gcd(lhs_bytes, rhs_bytes)/elem 로 추정 (정수 안전)
    size_t g = lhs_bytes;
    size_t t = rhs_bytes;
    while (t) { size_t r = g % t; g = t; t = r; }
    size_t K_times_elem = g;               // 공약수
    if (K_times_elem == 0 || (K_times_elem % elem) != 0) {
      status = iree_make_status(IREE_STATUS_FAILED_PRECONDITION, "cannot infer K");
      goto done;
    }
    uint32_t K = (uint32_t)(K_times_elem / elem);
    if (!K) { status = iree_make_status(IREE_STATUS_FAILED_PRECONDITION, "K=0"); goto done; }
    uint32_t N = (uint32_t)(rhs_bytes / (K * elem));
    uint32_t M = (uint32_t)(lhs_bytes / (K * elem));

    // wait용 저장
    cmd->wait_cmd.M = M;
    cmd->wait_cmd.N = N;

    // zero-bias 준비 (수명 보장)
    bias_bytes = (size_t)N * elem;
    cmd->zero_bias = (uint8_t*)malloc(bias_bytes);
    if (!cmd->zero_bias) { status = iree_make_status(IREE_STATUS_RESOURCE_EXHAUSTED, "malloc zero-bias"); goto done; }
    memset(cmd->zero_bias, 0, bias_bytes);
    bias_ptr = cmd->zero_bias;

    // 오프로딩
    sa16_matmul_bias_add_offload((const uint8_t*)lhs.contents.data,
                                 (const uint8_t*)rhs.contents.data,
                                 bias_ptr, M, K, N);
    goto done; // 이미 M,N 설정 끝
  }

  // 여기로 왔다는 건 bias가 있었던 경로
  if (bias_bytes == 0 || (bias_bytes % elem) != 0) {
    status = iree_make_status(IREE_STATUS_FAILED_PRECONDITION, "invalid bias size");
    goto done;
  }
  {
    uint32_t N = (uint32_t)(bias_bytes / elem);
    if (!N || (rhs_bytes % (N * elem)) != 0) {
      status = iree_make_status(IREE_STATUS_FAILED_PRECONDITION, "cannot infer K from rhs/bias");
      goto done;
    }
    uint32_t K = (uint32_t)(rhs_bytes / (N * elem));
    if (!K || (lhs_bytes % (K * elem)) != 0) {
      status = iree_make_status(IREE_STATUS_FAILED_PRECONDITION, "invalid K from rhs/lhs");
      goto done;
    }
    uint32_t M = (uint32_t)(lhs_bytes / (K * elem));
    cmd->wait_cmd.M = M;
    cmd->wait_cmd.N = N;

    sa16_matmul_bias_add_offload((const uint8_t*)lhs.contents.data,
                                 (const uint8_t*)rhs.contents.data,
                                 bias_ptr, M, K, N);
  }

done:
  if (lhs.buffer)  iree_hal_buffer_unmap_range(&lhs);
  if (rhs.buffer)  iree_hal_buffer_unmap_range(&rhs);
  if (bias.buffer) iree_hal_buffer_unmap_range(&bias);
  IREE_TRACE_ZONE_END(z0);
  return status;
}


static iree_status_t iree_hal_accel_cmd_matmul_relu_offload(
    void* user_context, iree_task_t* task,
    iree_task_submission_t* pending_submission) {
  iree_hal_accel_cmd_matmul_t* cmd =
      (iree_hal_accel_cmd_matmul_t*)user_context;
  IREE_TRACE_ZONE_BEGIN(z0);
  MP_SCOPE("OFFLOAD_CALL");

  // printf("[CPU] Offloading matmul_bias_add_relu to accelerator! <<<<\n");

  iree_status_t status = iree_ok_status();
  iree_hal_buffer_mapping_t lhs_mapping, rhs_mapping, bias_mapping;
  memset(&lhs_mapping, 0, sizeof(lhs_mapping));
  memset(&rhs_mapping, 0, sizeof(rhs_mapping));
  memset(&bias_mapping, 0, sizeof(bias_mapping));

  status = iree_hal_buffer_map_range(cmd->bindings[0].buffer,
                                     IREE_HAL_MAPPING_MODE_SCOPED,
                                     IREE_HAL_MEMORY_ACCESS_READ, 0,
                                     IREE_HAL_WHOLE_BUFFER, &lhs_mapping);
  if (!iree_status_is_ok(status)) goto mm_relu_cleanup;

  status = iree_hal_buffer_map_range(cmd->bindings[1].buffer,
                                     IREE_HAL_MAPPING_MODE_SCOPED,
                                     IREE_HAL_MEMORY_ACCESS_READ, 0,
                                     IREE_HAL_WHOLE_BUFFER, &rhs_mapping);
  if (!iree_status_is_ok(status)) goto mm_relu_cleanup;

  status = iree_hal_buffer_map_range(cmd->bindings[2].buffer,
                                     IREE_HAL_MAPPING_MODE_SCOPED,
                                     IREE_HAL_MEMORY_ACCESS_READ, 0,
                                     IREE_HAL_WHOLE_BUFFER, &bias_mapping);
  if (!iree_status_is_ok(status)) goto mm_relu_cleanup;

  const size_t elem_size = 4;
  size_t lhs_bytes = lhs_mapping.contents.data_length;
  size_t rhs_bytes = rhs_mapping.contents.data_length;
  size_t bias_bytes = bias_mapping.contents.data_length;

  uint32_t N = (uint32_t)(bias_bytes / elem_size);
  uint32_t K = (uint32_t)(rhs_bytes / (N * elem_size));
  uint32_t M = (uint32_t)(lhs_bytes / (K * elem_size));

  cmd->wait_cmd.M = M;
  cmd->wait_cmd.N = N;

  sa16_matmul_bias_add_relu_offload((const uint8_t*)lhs_mapping.contents.data,
                               (const uint8_t*)rhs_mapping.contents.data,
                               (const uint8_t*)bias_mapping.contents.data, M, K, N);

mm_relu_cleanup:
  if (lhs_mapping.buffer) iree_hal_buffer_unmap_range(&lhs_mapping);
  if (rhs_mapping.buffer) iree_hal_buffer_unmap_range(&rhs_mapping);
  if (bias_mapping.buffer) iree_hal_buffer_unmap_range(&bias_mapping);
  IREE_TRACE_ZONE_END(z0);
  return status;
}

static iree_status_t iree_hal_accel_cmd_matmul_wait(
    void* user_context, iree_task_t* task,
    iree_task_submission_t* pending_submission) {
  iree_hal_accel_cmd_matmul_t* cmd = (iree_hal_accel_cmd_matmul_t*)user_context;
  IREE_TRACE_ZONE_BEGIN(z0);
  MP_SCOPE("WAIT_CALL");

  iree_hal_buffer_mapping_t out = {{0}};
  IREE_RETURN_IF_ERROR(iree_hal_buffer_map_range(
      cmd->wait_cmd.output_binding.buffer, IREE_HAL_MAPPING_MODE_SCOPED,
      IREE_HAL_MEMORY_ACCESS_DISCARD_WRITE, 0, IREE_HAL_WHOLE_BUFFER, &out));

  sa16_wait_for_completion((uint8_t*)out.contents.data, cmd->wait_cmd.M, cmd->wait_cmd.N);

  iree_hal_buffer_unmap_range(&out);

  if (cmd->zero_bias) {
    free(cmd->zero_bias);
    cmd->zero_bias = NULL;
  }

  IREE_TRACE_ZONE_END(z0);
  return iree_ok_status();
}

static iree_status_t iree_hal_accel_cmd_conv_relu_offload(
    void* user_context, iree_task_t* task,
    iree_task_submission_t* pending_submission) {
  iree_hal_accel_cmd_conv_relu_t* cmd =
      (iree_hal_accel_cmd_conv_relu_t*)user_context;
  IREE_TRACE_ZONE_BEGIN(z0);
  MP_SCOPE("OFFLOAD_CALL");

  if (!cmd->bindings[0].buffer || !cmd->bindings[1].buffer || !cmd->bindings[2].buffer || !cmd->bindings[3].buffer) {
    IREE_TRACE_ZONE_END(z0);
    return iree_make_status(IREE_STATUS_INVALID_ARGUMENT, "conv_relu requires in/filter/bias/out");
  }

  // printf("[CPU] Offloading conv2d+relu to accelerator! <<<<\n");

  iree_status_t status = iree_ok_status();
  iree_hal_buffer_mapping_t input_mapping, filter_mapping, bias_mapping;
  memset(&input_mapping, 0, sizeof(input_mapping));
  memset(&filter_mapping, 0, sizeof(filter_mapping));
  memset(&bias_mapping, 0, sizeof(bias_mapping));

  if (!cmd->bindings[2].buffer) {
    IREE_TRACE_ZONE_END(z0);
    return iree_make_status(IREE_STATUS_FAILED_PRECONDITION, "matmul offload requires non-null bias buffer");
  }

  status = iree_hal_buffer_map_range(cmd->bindings[0].buffer,
                                     IREE_HAL_MAPPING_MODE_SCOPED,
                                     IREE_HAL_MEMORY_ACCESS_READ, 0,
                                     IREE_HAL_WHOLE_BUFFER, &input_mapping);
  if (!iree_status_is_ok(status)) goto conv_relu_cleanup;

  status = iree_hal_buffer_map_range(cmd->bindings[1].buffer,
                                     IREE_HAL_MAPPING_MODE_SCOPED,
                                     IREE_HAL_MEMORY_ACCESS_READ, 0,
                                     IREE_HAL_WHOLE_BUFFER, &filter_mapping);
  if (!iree_status_is_ok(status)) goto conv_relu_cleanup;

  status = iree_hal_buffer_map_range(cmd->bindings[2].buffer,
                                     IREE_HAL_MAPPING_MODE_SCOPED,
                                     IREE_HAL_MEMORY_ACCESS_READ, 0,
                                     IREE_HAL_WHOLE_BUFFER, &bias_mapping);
  if (!iree_status_is_ok(status)) goto conv_relu_cleanup;

  const size_t elem_size = 4;  // float32
  uint32_t N = cmd->workgroup_count[0];
  uint32_t OH = cmd->workgroup_count[1];
  uint32_t OW = cmd->workgroup_count[2];

  if (bias_mapping.contents.data_length == 0) {
    status = iree_make_status(IREE_STATUS_INVALID_ARGUMENT,
                              "conv bias cannot be zero size");
    goto conv_relu_cleanup;
  }
  uint32_t OC = (uint32_t)(bias_mapping.contents.data_length / elem_size);

  uint32_t stride_h = 1, stride_w = 1, pad_h = 0, pad_w = 0;
  uint32_t KH = 1, KW = 1;
  (void)parse_strides_from_name(cmd->entry_point_name, &stride_h, &stride_w);
  (void)parse_padding_from_name(cmd->entry_point_name, &pad_h, &pad_w);
  bool kernel_parsed =
      iree_status_is_ok(parse_kernel_size_from_name(cmd->entry_point_name, &KH, &KW));

  if (filter_mapping.contents.data_length == 0) {
    status = iree_make_status(IREE_STATUS_INVALID_ARGUMENT,
                              "conv filter cannot be zero size");
    goto conv_relu_cleanup;
  }
  uint32_t C = 0;
  if (kernel_parsed && (KH * KW * OC) != 0) {
    C = (uint32_t)(filter_mapping.contents.data_length / (elem_size * KH * KW * OC));
  } else {
    C = 3;
    if (C * OC * elem_size > 0) {
      double kh2 = (double)filter_mapping.contents.data_length / (elem_size * C * OC);
      KH = (uint32_t)sqrt(kh2);
      KW = KH;
    }
  }

  uint32_t H = (OH - 1) * stride_h + KH - (2 * pad_h);
  uint32_t W = (OW - 1) * stride_w + KW - (2 * pad_w);

  const uint64_t in_bytes  = input_mapping.contents.data_length;
  const uint64_t fil_bytes = filter_mapping.contents.data_length;

  __uint128_t ws_elems = (__uint128_t)N * OH * OW * KH * KW * C;
  __uint128_t ws_bytes = ws_elems * elem_size;

  fprintf(stderr,
          "[ACCEL][CONV-RELU-PARAM] N=%u OH=%u OW=%u C=%u OC=%u KH=%u KW=%u sh/sw=%u/%u ph/pw=%u/%u elem=%zu | "
          "in=%" PRIu64 "B fil=%" PRIu64 "B ws~=%lluMB\n",
          N, OH, OW, C, OC, KH, KW, stride_h, stride_w, pad_h, pad_w, elem_size,
          in_bytes, fil_bytes,
          (unsigned long long)((uint64_t)(ws_bytes / (1024ull*1024ull))));
  
  if (in_bytes  > SA16_MAX_TENSOR_BYTES ||
      fil_bytes > SA16_MAX_TENSOR_BYTES) {
    status = iree_make_status(IREE_STATUS_OUT_OF_RANGE, "tensor too large for sa16");
    goto conv_relu_cleanup;
  }
  if (ws_bytes > SA16_MAX_WORKSPACE_BYTES) {
    status = iree_make_status(IREE_STATUS_RESOURCE_EXHAUSTED, "workspace too large for sa16");
    goto conv_relu_cleanup;
  }

  cmd->wait_cmd.N = N;
  cmd->wait_cmd.OH = OH;
  cmd->wait_cmd.OW = OW;
  cmd->wait_cmd.OC = OC;

  sa16_conv_relu_offload((const uint8_t*)input_mapping.contents.data,
                         (const uint8_t*)filter_mapping.contents.data,
                         (const uint8_t*)bias_mapping.contents.data,
                         N, H, W, C, OC, KH, KW, stride_h, stride_w, pad_h, pad_w);

conv_relu_cleanup:
  if (input_mapping.buffer) iree_hal_buffer_unmap_range(&input_mapping);
  if (filter_mapping.buffer) iree_hal_buffer_unmap_range(&filter_mapping);
  if (bias_mapping.buffer) iree_hal_buffer_unmap_range(&bias_mapping);
  IREE_TRACE_ZONE_END(z0);
  return status;
}

static iree_status_t iree_hal_accel_cmd_conv_relu_wait(
    void* user_context, iree_task_t* task,
    iree_task_submission_t* pending_submission) {
  const iree_hal_accel_cmd_conv_relu_t* cmd =
      (const iree_hal_accel_cmd_conv_relu_t*)user_context;
  IREE_TRACE_ZONE_BEGIN(z0);
  MP_SCOPE("WAIT_CALL");

  iree_hal_buffer_mapping_t output_mapping;
  IREE_RETURN_IF_ERROR(iree_hal_buffer_map_range(
      cmd->wait_cmd.output_binding.buffer, IREE_HAL_MAPPING_MODE_SCOPED,
      IREE_HAL_MEMORY_ACCESS_DISCARD_WRITE, 0, IREE_HAL_WHOLE_BUFFER,
      &output_mapping));

  iree_dpi_sa16_conv_wait_for_completion((uint8_t*)output_mapping.contents.data, cmd->wait_cmd.N,
                                cmd->wait_cmd.OH, cmd->wait_cmd.OW, cmd->wait_cmd.OC);

  // fprintf(stderr, "[VERIFY conv_relu] first 4 f32:\n");
  // float* out_f32 = (float*)output_mapping.contents.data;
  // for (int i = 0; i < 4 && i < (int)(cmd->wait_cmd.N * cmd->wait_cmd.OH * cmd->wait_cmd.OW * cmd->wait_cmd.OC); ++i) {
  //   fprintf(stderr, "  out[%d] = %f\n", i, out_f32[i]);
  // }

  iree_hal_buffer_unmap_range(&output_mapping);
  IREE_TRACE_ZONE_END(z0);
  return iree_ok_status();
}

// ============================================================================
// Submit/issue helpers
// ============================================================================
static iree_status_t iree_hal_accel_command_buffer_emit_execution_task(
    iree_hal_accel_command_buffer_t* command_buffer, iree_task_t* task) {
  if (command_buffer->state.open_barrier == NULL) {
    iree_task_list_push_back(&command_buffer->leaf_tasks, task);
  } else {
    iree_task_list_push_back(&command_buffer->state.open_tasks, task);
    ++command_buffer->state.open_task_count;
  }
  return iree_ok_status();
}

//! A placeholder for the queue state.
typedef struct iree_hal_accel_queue_state_t {} iree_hal_accel_queue_state_t;


 iree_status_t iree_hal_accel_command_buffer_issue(
     iree_hal_command_buffer_t* base_command_buffer,
     iree_hal_accel_queue_state_t* queue_state, iree_task_t* retire_task,
     iree_task_submission_t* pending_submission) {
   iree_hal_accel_command_buffer_t* command_buffer =
       iree_hal_accel_command_buffer_cast(base_command_buffer);
   IREE_ASSERT_TRUE(command_buffer);

   bool has_root_tasks = !iree_task_list_is_empty(&command_buffer->root_tasks);
   if (!has_root_tasks) {
     return iree_ok_status();
   }

   bool has_leaf_tasks = !iree_task_list_is_empty(&command_buffer->leaf_tasks);
   if (has_leaf_tasks) {
     for (iree_task_t* task = command_buffer->leaf_tasks.head; task != NULL;
          task = task->next_task) {
       iree_task_set_completion_task(task, retire_task);
     }
   } else {
     for (iree_task_t* task = command_buffer->root_tasks.head; task != NULL;
          task = task->next_task) {
       iree_task_set_completion_task(task, retire_task);
     }
   }

   // [FIX] 아직 flush되지 않은 open_tasks에도 retire_task를 연결해
   // 측정/수거가 wait 전에 끝나지 않도록 함 (세그폴트 원인 제거)
   if (!iree_task_list_is_empty(&command_buffer->state.open_tasks)) {
     for (iree_task_t* task = command_buffer->state.open_tasks.head;
          task != NULL; task = task->next_task) {
       iree_task_set_completion_task(task, retire_task);
     }
   }

   iree_task_submission_enqueue_list(pending_submission,
                                     &command_buffer->root_tasks);
   iree_task_list_initialize(&command_buffer->leaf_tasks);

   return iree_ok_status();
 }


// ============================================================================
// Dispatch (offload routing)
// ============================================================================
static iree_status_t iree_hal_accel_command_buffer_dispatch(
    iree_hal_command_buffer_t* base_command_buffer,
    iree_hal_executable_t* executable, int32_t entry_point,
    const iree_hal_dispatch_config_t config, iree_const_byte_span_t constants,
    iree_hal_buffer_ref_list_t bindings, iree_hal_dispatch_flags_t flags) {
  // ---- Lazy-init accelerator ----
  if (!sa16_is_initialized) {
    (void)host_sa16_init();             // 실패 시에도 계속 진행(오프로드만 비활성)
    sa16_is_initialized = true;
  }

  iree_hal_accel_command_buffer_t* command_buffer =
      iree_hal_accel_command_buffer_cast(base_command_buffer);

  if (iree_hal_dispatch_uses_custom_arguments(flags)) {
    return iree_make_status(
        IREE_STATUS_UNIMPLEMENTED,
        "direct/indirect arguments are not supported in the task system");
  }

  // NOTE: executable은 accel 변종일 수도/로컬일 수도 있음.
  iree_hal_local_executable_t* local_executable =
      iree_hal_local_executable_cast(executable);

  // Entry point name (없어도 동작은 가능하지만, CPU 폴백 가드에 사용)
  iree_string_view_t entry_point_name = iree_string_view_empty();
  bool is_accel = iree_hal_accel_executable_isa((iree_hal_executable_t*)local_executable);

  iree_vm_function_t entry_fn = (iree_vm_function_t){0};
  bool have_entry_fn = false;

  if (is_accel) {
    iree_hal_accel_executable_t* accel_executable =
        (iree_hal_accel_executable_t*)local_executable;
    if (entry_point >= 0 &&
        (uint32_t)entry_point < accel_executable->entry_fn_count) {
      int32_t vm_ordinal = accel_executable->entry_fn_ordinals[entry_point];
      entry_fn.module = accel_executable->bytecode_module;
      entry_fn.linkage = IREE_VM_FUNCTION_LINKAGE_EXPORT;
      entry_fn.ordinal = vm_ordinal;
      have_entry_fn = true;
      entry_point_name = iree_vm_function_name(&entry_fn);
    }
  }

  char entry_point_cstr[128];
  iree_host_size_t entry_point_len =
      iree_min(entry_point_name.size,
               (iree_host_size_t)(sizeof(entry_point_cstr) - 1));
  memcpy(entry_point_cstr, entry_point_name.data, entry_point_len);
  entry_point_cstr[entry_point_len] = '\0';

  // ---- Heuristic/env switches ----
  bool force_conv = accel_force_conv();
  bool force_mm   = accel_force_matmul();
  bool force_cpu  = accel_force_cpu();

  bool name_hints_conv = false;
  if (entry_point_cstr[0]) {
    const char* s = entry_point_cstr;
    name_hints_conv = (strstr(s, "conv") != NULL) ||
                      (strstr(s, "depthwise") != NULL) ||
                      (strstr(s, "dw") != NULL);
  }

  // ---- Compiler attribute says offload target? ----
  bool has_matmul = !force_conv && (force_mm || try_classify_matmul(&bindings));
  bool has_conv   = !force_mm   && (force_conv || try_classify_conv_like(&bindings) || name_hints_conv);

  bool can_offload_matmul = sa16_matmul_bias_add_offload_p != NULL;
  bool can_offload_conv   = iree_dpi_sa16_conv_offload_p != NULL;

  // 속성으로 지정되었거나, 히유리스틱상 conv/mm이고 해당 DPI 훅이 있으면 오프로딩
  bool should_offload =
      check_offload_target_attr(executable, entry_point, IREE_SV("sa16")) ||
      (can_offload_conv   && has_conv) ||
      (can_offload_matmul && has_matmul);

  if (force_cpu) should_offload = false;
  if (!can_offload_matmul && !can_offload_conv) should_offload = false;

  // ============================
  // Offload: Conv path
  // ============================
  if (should_offload && has_conv && bindings.count >= 3 && can_offload_conv) {
    if (accel_debug_enabled()) {
      fprintf(stderr,
              "[ACCEL][OFFLOAD] Offloading CONV-like op '%s' to sa16 accelerator.\n",
              entry_point_cstr);
    }
    iree_hal_accel_cmd_conv_t* cmd = NULL;
    IREE_RETURN_IF_ERROR(
        iree_arena_allocate(&command_buffer->arena, sizeof(*cmd), (void**)&cmd));
    memset(cmd, 0, sizeof(*cmd));

    iree_task_call_initialize(
        command_buffer->scope,
        iree_task_make_call_closure(iree_hal_accel_cmd_conv_offload, (void*)cmd),
        &cmd->offload_task);

    cmd->entry_point_name = entry_point_name;
    memcpy(cmd->workgroup_count, config.workgroup_count, sizeof(cmd->workgroup_count));

    cmd->bindings[0] = bindings.values[0];
    cmd->bindings[1] = bindings.values[1];

    if (bindings.count >= 4) {
      cmd->bindings[2] = bindings.values[2];
      cmd->bindings[3] = bindings.values[bindings.count - 1];
    } else {
      const iree_hal_buffer_ref_t input_ref = bindings.values[0];
      const iree_hal_buffer_ref_t third_ref = bindings.values[2];
      uint64_t in_len_eff = get_effective_len(input_ref);
      uint64_t t_len_eff  = get_effective_len(third_ref);
      bool is_output_like = (in_len_eff && t_len_eff && t_len_eff >= in_len_eff / 3);
      cmd->bindings[2] = is_output_like ? (iree_hal_buffer_ref_t){0} : bindings.values[2];
      cmd->bindings[3] = is_output_like ? bindings.values[2] : bindings.values[0];
    }

    iree_task_call_initialize(
        command_buffer->scope,
        iree_task_make_call_closure(iree_hal_accel_cmd_conv_wait, (void*)cmd),
        &cmd->wait_task);
    iree_task_set_completion_task(&cmd->offload_task.header, &cmd->wait_task.header);

    IREE_RETURN_IF_ERROR(iree_hal_resource_set_insert_strided(
        command_buffer->resource_set, bindings.count, bindings.values,
        offsetof(iree_hal_buffer_ref_t, buffer),
        sizeof(iree_hal_buffer_ref_t)));

    return iree_hal_accel_command_buffer_emit_execution_task(
        command_buffer, &cmd->offload_task.header);
  }

  // ============================
  // Offload: Matmul path
  // ============================
  if (should_offload && has_matmul && bindings.count >= 3 && can_offload_matmul) {
    if (accel_debug_enabled()) {
      fprintf(stderr,
              "[ACCEL][OFFLOAD] Offloading MATMUL-like op '%s' to sa16 accelerator.\n",
              entry_point_cstr);
    }
    iree_hal_accel_cmd_matmul_t* cmd = NULL;
    IREE_RETURN_IF_ERROR(
        iree_arena_allocate(&command_buffer->arena, sizeof(*cmd), (void**)&cmd));
    memset(cmd, 0, sizeof(*cmd));

    iree_task_call_initialize(
        command_buffer->scope,
        iree_task_make_call_closure(iree_hal_accel_cmd_matmul_offload, (void*)cmd),
        &cmd->task);

    iree_task_call_initialize(
        command_buffer->scope,
        iree_task_make_call_closure(iree_hal_accel_cmd_matmul_wait, (void*)cmd),
        &cmd->wait_cmd.task);
    iree_task_set_completion_task(&cmd->task.header, &cmd->wait_cmd.task.header);

    cmd->bindings[0] = bindings.values[0];
    cmd->bindings[1] = bindings.values[1];
    if (bindings.count >= 4 && bindings.values[2].buffer) {
      cmd->bindings[2] = bindings.values[2];
      cmd->bindings[3] = bindings.values[3];
    } else {
      cmd->bindings[2] = (iree_hal_buffer_ref_t){0};
      cmd->bindings[3] = bindings.values[2];
    }
    cmd->wait_cmd.output_binding = cmd->bindings[3];

    IREE_RETURN_IF_ERROR(iree_hal_resource_set_insert_strided(
        command_buffer->resource_set, bindings.count, bindings.values,
        offsetof(iree_hal_buffer_ref_t, buffer),
        sizeof(iree_hal_buffer_ref_t)));

    return iree_hal_accel_command_buffer_emit_execution_task(
        command_buffer, &cmd->task.header);
  }

  // ============================
  // CPU Fallback (강화된 가드)
  // ============================

  // 만약 바인딩이 하나도 없다면, 실행할 내용이 없는 디스패치이므로
  // CPU 폴백을 시도하지 않고 즉시 성공 처리합니다.
  if (bindings.count == 0) {
    return iree_ok_status();
  }

  // 1) 엔트리 이름이 비어 있으면 CPU 폴백 금지 (accel-only 모듈 가능성 높음)
  //    이러한 디스패치는 실제 연산이 없는 경우가 많아 CPU 폴백 시 오류를 유발할 수 있습니다.
  if (entry_point_name.size == 0) {
    // if (accel_debug_enabled()) {
    //   fprintf(stderr, "[ACCEL][CPU-FB][SKIP] Fallback skipped for dispatch with no entry point name.\n");
    // }
    return iree_ok_status();
  }

  // [로그 추가] 어떤 연산이 CPU로 폴백되는지 확인합니다.
  if (accel_debug_enabled()) {
    fprintf(stderr,
            "[ACCEL][CPU-FB] Fallback for ep='%s'. Reason: "
            "Not matched as conv (name_hints=%d, heuristic=%d, can_offload=%d) or "
            "matmul (heuristic=%d, can_offload=%d).\n",
            entry_point_cstr, name_hints_conv, has_conv, can_offload_conv,
            has_matmul, can_offload_matmul);
  }
  // 2) 바인딩 유효성: CPU 커널은 NULL/0-len 바인딩을 기대하지 않음
  for (iree_host_size_t i = 0; i < bindings.count; ++i) {
    const iree_hal_buffer_ref_t br = bindings.values[i];
    if (!br.buffer) {
      return iree_make_status(
          IREE_STATUS_FAILED_PRECONDITION,
          "CPU fallback: binding %zu is NULL", (size_t)i);
    }
    // length==0 또는 WHOLE → 실제 길이로 대체해도 0이면 금지
    iree_device_size_t eff = br.length;
    if (eff == 0 || eff == IREE_HAL_WHOLE_BUFFER) {
      eff = iree_hal_buffer_byte_length(br.buffer);
      if (br.offset > eff) {
        return iree_make_status(
            IREE_STATUS_FAILED_PRECONDITION,
            "CPU fallback: binding %zu offset beyond buffer size", (size_t)i);
      }
      eff -= br.offset;
    }
    if (eff == 0) {
      return iree_make_status(
          IREE_STATUS_FAILED_PRECONDITION,
          "CPU fallback: binding %zu has zero effective length", (size_t)i);
    }
  }

  // 3) workgroup_size 보정 (0이면 1)
  uint32_t wg_size_fixed[3] = {
      config.workgroup_size[0] ? config.workgroup_size[0] : 1,
      config.workgroup_size[1] ? config.workgroup_size[1] : 1,
      config.workgroup_size[2] ? config.workgroup_size[2] : 1,
  };

  // === 실제 폴백 디스패치 준비 ===
  uint32_t constant_count = constants.data_length / sizeof(uint32_t);
  uint32_t binding_count = bindings.count;

  iree_hal_accel_cmd_dispatch_t* cmd = NULL;
  iree_host_size_t total_cmd_size =
      sizeof(*cmd) + constant_count * sizeof(uint32_t) +
      binding_count * sizeof(void*) + binding_count * sizeof(size_t);
  IREE_RETURN_IF_ERROR(iree_arena_allocate(&command_buffer->arena,
                                           total_cmd_size, (void**)&cmd));

  iree_task_dispatch_initialize(
      command_buffer->scope,
      iree_task_make_dispatch_closure(iree_hal_accel_cmd_dispatch_tile,
                                      (void*)cmd),
      wg_size_fixed, config.workgroup_count, &cmd->task);

  cmd->executable = local_executable;
  cmd->ordinal = entry_point;
  cmd->entry_point_name = entry_point_name;
  cmd->constant_count = (uint16_t)constant_count;
  cmd->binding_count  = (uint16_t)binding_count;

  // 간접 인자 처리
  iree_host_size_t resource_count = 1;
  iree_hal_resource_t* resources[2] = {(iree_hal_resource_t*)executable, NULL};
  if (iree_hal_dispatch_uses_indirect_parameters(flags)) {
    resources[resource_count++] =
        (iree_hal_resource_t*)config.workgroup_count_ref.buffer;

    cmd->task.header.flags |= IREE_TASK_FLAG_DISPATCH_INDIRECT;

    iree_hal_buffer_mapping_t buffer_mapping = {{0}};
    IREE_RETURN_IF_ERROR(iree_hal_buffer_map_range(
        config.workgroup_count_ref.buffer, IREE_HAL_MAPPING_MODE_PERSISTENT,
        IREE_HAL_MEMORY_ACCESS_READ, config.workgroup_count_ref.offset,
        3 * sizeof(uint32_t), &buffer_mapping));
    cmd->task.workgroup_count.ptr =
        (const uint32_t*)buffer_mapping.contents.data;
  }

  IREE_RETURN_IF_ERROR(iree_hal_resource_set_insert(
      command_buffer->resource_set, resource_count,
      (const iree_hal_resource_t* const*)resources));

  // 로컬 메모리 크기 계산 (VM attr + pages + dynamic)
  iree_hal_executable_dispatch_attrs_v0_t dispatch_attrs =
      (iree_hal_executable_dispatch_attrs_v0_t){0};
  if (is_accel) {
    iree_hal_accel_executable_t* accel_exec =
        (iree_hal_accel_executable_t*)local_executable;
    if (accel_exec->base.dispatch_attrs &&
        entry_point >= 0 &&
        (uint32_t)entry_point < accel_exec->entry_fn_count) {
      dispatch_attrs = accel_exec->base.dispatch_attrs[entry_point];
    }
  }

  uint32_t static_from_vm_attr_bytes = 0;
  if (have_entry_fn) {
    iree_string_view_t attr_mem =
        iree_vm_function_lookup_attr_by_name(
            &entry_fn, IREE_SV("workgroup_local_memory_bytes"));
    if (attr_mem.size == 0) {
      attr_mem = iree_vm_function_lookup_attr_by_name(
          &entry_fn, IREE_SV("wg_local_mem"));
    }
    if (attr_mem.size) {
      (void)iree_string_view_atoi_uint32(attr_mem, &static_from_vm_attr_bytes);
    }
  }

  uint32_t pages_bytes = 0;
#ifdef IREE_HAL_EXECUTABLE_WORKGROUP_LOCAL_MEMORY_PAGE_SIZE
  pages_bytes = dispatch_attrs.local_memory_pages *
                IREE_HAL_EXECUTABLE_WORKGROUP_LOCAL_MEMORY_PAGE_SIZE;
#endif

  cmd->task.local_memory_size =
      static_from_vm_attr_bytes + pages_bytes +
      config.dynamic_workgroup_local_memory;

  // Constants 복사
  if (IREE_UNLIKELY((constants.data_length % sizeof(uint32_t)) != 0)) {
    return iree_make_status(IREE_STATUS_FAILED_PRECONDITION,
                            "constants must be 4-byte aligned");
  }
  uint8_t* cmd_ptr = (uint8_t*)cmd + sizeof(*cmd);
  uint32_t* constants_ptr = (uint32_t*)cmd_ptr;
  memcpy(constants_ptr, constants.data,
         constant_count * sizeof(*constants_ptr));
  cmd_ptr += constant_count * sizeof(*constants_ptr);

  // Bindings 매핑 (NULL/0-len은 위에서 이미 차단됨)
  IREE_ASSERT_EQ(bindings.count, binding_count);
  void**  binding_ptrs    = (void**)cmd_ptr;
  cmd_ptr += bindings.count * sizeof(*binding_ptrs);
  size_t* binding_lengths = (size_t*)cmd_ptr;
  cmd_ptr += bindings.count * sizeof(*binding_lengths);

  for (iree_host_size_t i = 0; i < bindings.count; ++i) {
    iree_hal_buffer_mapping_t m = {{0}};
    iree_device_size_t map_len = bindings.values[i].length;
    if (map_len == 0 || map_len == IREE_HAL_WHOLE_BUFFER) {
      map_len = iree_hal_buffer_byte_length(bindings.values[i].buffer);
      map_len -= bindings.values[i].offset;
    }
    IREE_RETURN_IF_ERROR(iree_hal_buffer_map_range(
        bindings.values[i].buffer, IREE_HAL_MAPPING_MODE_PERSISTENT,
        IREE_HAL_MEMORY_ACCESS_ANY, bindings.values[i].offset, map_len, &m));
    binding_ptrs[i]    = m.contents.data;
    binding_lengths[i] = m.contents.data_length;
  }

  // 디버그 출력
  fprintf(stderr,
          "[CPU Fallback] ep='%s' ordinal=%d wg_size=(%u,%u,%u) wg_count=(%u,%u,%u) "
          "constants=%u bindings=%u local_mem=%u\n",
          entry_point_cstr, entry_point,
          wg_size_fixed[0], wg_size_fixed[1], wg_size_fixed[2],
          config.workgroup_count[0], config.workgroup_count[1], config.workgroup_count[2],
          constant_count, binding_count, cmd->task.local_memory_size);
  for (size_t i = 0; i < bindings.count; ++i) {
    fprintf(stderr, "  [B%zu] ptr=%p len=%zu\n", i, binding_ptrs[i], binding_lengths[i]);
  }

  IREE_RETURN_IF_ERROR(iree_hal_resource_set_insert_strided(
      command_buffer->resource_set, bindings.count, bindings.values,
      offsetof(iree_hal_buffer_ref_t, buffer),
      sizeof(iree_hal_buffer_ref_t)));

  return iree_hal_accel_command_buffer_emit_execution_task(
      command_buffer, &cmd->task.header);
}



// ============================================================================
// VTable
// ============================================================================
static const iree_hal_command_buffer_vtable_t
    iree_hal_accel_command_buffer_vtable = {
        .destroy = iree_hal_accel_command_buffer_destroy,
        .begin = iree_hal_accel_command_buffer_begin,
        .end = iree_hal_accel_command_buffer_end,
        .begin_debug_group = iree_hal_accel_command_buffer_begin_debug_group,
        .end_debug_group = iree_hal_accel_command_buffer_end_debug_group,
        .execution_barrier = iree_hal_accel_command_buffer_execution_barrier,
        .signal_event = iree_hal_accel_command_buffer_signal_event,
        .reset_event = iree_hal_accel_command_buffer_reset_event,
        .wait_events = iree_hal_accel_command_buffer_wait_events,
        .advise_buffer = iree_hal_accel_command_buffer_advise_buffer,
        .fill_buffer = iree_hal_accel_command_buffer_fill_buffer,
        .update_buffer = iree_hal_accel_command_buffer_update_buffer,
        .copy_buffer = iree_hal_accel_command_buffer_copy_buffer,
        .collective = iree_hal_accel_command_buffer_collective,
        .dispatch = iree_hal_accel_command_buffer_dispatch,
}; 