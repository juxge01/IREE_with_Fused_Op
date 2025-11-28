// Copyright 2024 The IREE Authors
//
// Licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef IREE_HAL_DRIVERS_ACCEL_ACCEL_EXECUTABLE_H_
#define IREE_HAL_DRIVERS_ACCEL_ACCEL_EXECUTABLE_H_

#include "iree/hal/local/local_executable.h"
#include "iree/vm/api.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

// HACK: we need to get at the vmvx module state.
typedef struct iree_hal_vmvx_worker_state_t {
  iree_vm_context_t* context;
  iree_vm_module_state_t* vmvx_module_state;
} iree_hal_vmvx_worker_state_t;

// The internal structure of an executable created by accel_module_loader.
// This is shared between the loader and the command buffer to allow the
// command buffer to inspect the executable's contents for offloading.
typedef struct iree_hal_accel_executable_t {
  iree_hal_local_executable_t base;

  iree_vm_module_t* bytecode_module;
  iree_host_size_t worker_capacity;
  iree_hal_vmvx_worker_state_t* worker_states;

  // Resolved entry function export ordinals from the bytecode module.
  iree_host_size_t entry_fn_count;
  uint16_t entry_fn_ordinals[];
} iree_hal_accel_executable_t;

extern const iree_hal_local_executable_vtable_t
    iree_hal_accel_executable_vtable;

// Returns true if the given |executable| is an `accel` executable.
bool iree_hal_accel_executable_isa(iree_hal_executable_t* executable);

#ifdef __cplusplus
}  // extern "C"
#endif  // __cplusplus

#endif  // IREE_HAL_DRIVERS_ACCEL_ACCEL_EXECUTABLE_H_