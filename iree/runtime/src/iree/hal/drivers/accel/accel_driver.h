// Copyright 2021 The IREE Authors
//
// Licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef IREE_HAL_DRIVERS_ACCEL_ACCEL_DRIVER_H_
#define IREE_HAL_DRIVERS_ACCEL_ACCEL_DRIVER_H_

#include "iree/base/api.h"
#include "iree/hal/api.h"
#include "iree/hal/drivers/accel/accel_device.h"
#include "iree/hal/local/executable_loader.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

// Options for creating a accel driver.
// Must be initialized with iree_hal_accel_driver_options_initialize.
typedef struct iree_hal_accel_driver_options_t {
  // The default parameters for devices created by the driver.
  iree_hal_accel_device_params_t default_device_options;
} iree_hal_accel_driver_options_t;

// Initializes |out_options| to default values.
void iree_hal_accel_driver_options_initialize(
    iree_hal_accel_driver_options_t* out_options);

iree_status_t iree_hal_accel_driver_create(
    iree_string_view_t identifier, const iree_hal_accel_driver_options_t* options,
    iree_host_size_t loader_count, iree_hal_executable_loader_t** loaders,
    iree_hal_allocator_t* device_allocator, iree_allocator_t host_allocator,
    iree_hal_driver_t** out_driver);

#ifdef __cplusplus
}  // extern "C"
#endif  // __cplusplus

#endif  // IREE_HAL_DRIVERS_ACCEL_ACCEL_DRIVER_H_
