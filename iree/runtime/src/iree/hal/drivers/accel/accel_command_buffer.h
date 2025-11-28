// Copyright 2020 The IREE Authors
//
// Licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef IREE_HAL_DRIVERS_ACCEL_ACCEL_COMMAND_BUFFER_H_
#define IREE_HAL_DRIVERS_ACCEL_ACCEL_COMMAND_BUFFER_H_

#include "iree/base/api.h"
#include "iree/base/internal/arena.h"
#include "iree/hal/api.h"
#include "iree/task/scope.h"
#include "iree/task/task.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

// A placeholder for the queue state. This driver is simple and doesn't
// need to track complex state.
typedef struct iree_hal_accel_queue_state_t iree_hal_accel_queue_state_t;

iree_status_t iree_hal_accel_command_buffer_create(
    iree_hal_allocator_t* device_allocator, iree_task_scope_t* scope,
    iree_hal_command_buffer_mode_t mode,
    iree_hal_command_category_t command_categories,
    iree_hal_queue_affinity_t queue_affinity, iree_host_size_t binding_capacity,
    iree_arena_block_pool_t* block_pool, iree_allocator_t host_allocator,
    iree_hal_command_buffer_t** out_command_buffer);

// Returns true if |command_buffer| is a task system command buffer.
bool iree_hal_accel_command_buffer_isa(
    iree_hal_command_buffer_t* command_buffer);

// Issues a recorded command buffer using the serial |queue_state|.
// |queue_state| is used to track the synchronization scope of the queue from
// prior commands such as signaled events and will be mutated as events are
// reset or new events are signaled.
//
// |retire_task| will be scheduled once all commands issued from the command
// buffer retire and can be used as a fence point.
//
// Any new tasks that are allocated as part of the issue operation (such as
// barrier tasks to handle event synchronization) will be acquired from |arena|.
// The lifetime of |arena| must be at least that of |retire_task| ensuring that
// all of the allocated commands issued have completed and their memory in the
// arena can be recycled.
//
// |pending_submission| will receive the ready list of commands and must be
// submitted to the executor (or discarded on failure) by the caller.
iree_status_t iree_hal_accel_command_buffer_issue(
    iree_hal_command_buffer_t* command_buffer,
    iree_hal_accel_queue_state_t* queue_state, iree_task_t* retire_task,
    iree_task_submission_t* pending_submission);

//! These are the C-linkable stubs that call into the accelerator library.
//! The function pointers are resolved at runtime in `sa16_init`.
extern void (*sa16_matmul_bias_add_offload_p)(
    const uint8_t* lhs, const uint8_t* rhs, const uint8_t* bias,
    uint32_t M, uint32_t K, uint32_t N);

// IREE에서 직접 호출할 wrapper 선언
void sa16_matmul_bias_add_offload(
    const uint8_t* lhs, const uint8_t* rhs, const uint8_t* bias,
    uint32_t M, uint32_t K, uint32_t N);

extern void (*sa16_matmul_bias_add_relu_offload_p)(const uint8_t*, const uint8_t*,
                                                   const uint8_t*, uint32_t,
                                                   uint32_t, uint32_t);
extern void (*sa16_wait_for_completion_p)(uint8_t*, uint32_t, uint32_t);
extern void (*iree_dpi_sa16_conv_offload_p)(
    const uint8_t*, const uint8_t*, const uint8_t*, uint32_t, uint32_t,
    uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t,
    uint32_t, uint32_t);
extern void (*sa16_conv_relu_offload_p)(const uint8_t*, const uint8_t*,
                                        const uint8_t*, uint32_t, uint32_t,
                                        uint32_t, uint32_t, uint32_t,
                                        uint32_t, uint32_t, uint32_t,
                                        uint32_t, uint32_t, uint32_t);
extern void (*iree_dpi_sa16_conv_wait_for_completion_p)(
    uint8_t*, uint32_t, uint32_t, uint32_t, uint32_t);
extern uint64_t (*sa16_get_cycles_p)(void);
// void sa16_matmul_bias_add_offload(const uint8_t*, const uint8_t*, const uint8_t*, uint32_t, uint32_t, uint32_t);
void sa16_matmul_bias_add_relu_offload(const uint8_t*, const uint8_t*, const uint8_t*,
                                       uint32_t, uint32_t, uint32_t);
void sa16_wait_for_completion(uint8_t*, uint32_t, uint32_t);
void sa16_conv_wait_for_completion(uint8_t*, uint32_t, uint32_t, uint32_t, uint32_t);
uint64_t sa16_get_cycles(void);

// Returns the total accumulated cycles from all offloaded operations.
uint64_t iree_hal_accel_get_total_cycles(void);

#ifdef __cplusplus
}  // extern "C"
#endif  // __cplusplus

#endif  // IREE_HAL_DRIVERS_ACCEL_ACCEL_COMMAND_BUFFER_H_
