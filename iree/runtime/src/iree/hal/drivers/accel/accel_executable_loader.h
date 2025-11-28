// Copyright 2024 The IREE Authors
//
// Licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef IREE_HAL_DRIVERS_ACCEL_ACCEL_EXECUTABLE_LOADER_H_
#define IREE_HAL_DRIVERS_ACCEL_ACCEL_EXECUTABLE_LOADER_H_

#include "iree/base/api.h"
#include "iree/hal/local/executable_loader.h"
#include "iree/vm/api.h"

// HACK: vmvx의 worker state 구조체를 가져와야 합니다.
#include "iree/hal/drivers/accel/accel_executable.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

// Creates an executable loader that can load executables from the IREE VM
// FlatBuffer format produced by the `accel` backend.
iree_status_t iree_hal_accel_executable_loader_create(
    iree_vm_instance_t* instance, iree_host_size_t user_module_count,
    iree_vm_module_t** user_modules, iree_allocator_t host_allocator,
    iree_hal_executable_loader_t** out_executable_loader);

void iree_hal_vmvx_worker_state_deinitialize(
    iree_hal_vmvx_worker_state_t* state);

#ifdef __cplusplus
}  // extern "C"
#endif  // __cplusplus

#endif  // IREE_HAL_DRIVERS_ACCEL_ACCEL_EXECUTABLE_LOADER_H_