// Copyright 2024 The IREE Authors
//
// Licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "iree/hal/drivers/accel/registration.h"

#include <inttypes.h>
#include <stddef.h>

#include "iree/base/api.h"
#include "iree/hal/drivers/accel/accel_driver.h"
#include "iree/hal/local/loaders/system_library_loader.h"
#include "iree/hal/drivers/accel/accel_executable_loader.h"
#include "iree/hal/local/loaders/vmvx_module_loader.h"
#include "iree/vm/api.h"

#define IREE_HAL_ACCEL_DRIVER_ID 0x4143434C  // 'ACCL'

static iree_status_t iree_hal_accel_driver_factory_enumerate(
    void* self, iree_host_size_t* out_driver_info_count,
    const iree_hal_driver_info_t** out_driver_infos) {
  static const iree_hal_driver_info_t driver_info = {
      .driver_id = IREE_HAL_ACCEL_DRIVER_ID,
      .driver_name = iree_string_view_literal("accel"),
      .full_name = iree_string_view_literal("Custom Accelerator (Simulated)"),
  };
  *out_driver_info_count = 1;
  *out_driver_infos = &driver_info;
  return iree_ok_status();
}

static iree_status_t iree_hal_accel_driver_factory_try_create(
    void* self, iree_hal_driver_id_t driver_id,
    iree_allocator_t host_allocator, iree_hal_driver_t** out_driver) {
  if (driver_id != IREE_HAL_ACCEL_DRIVER_ID) {
    return iree_make_status(IREE_STATUS_UNAVAILABLE,
                            "no driver with ID %016" PRIX64
                            " is available from this factory",
                            driver_id);
  }

  iree_hal_accel_driver_options_t driver_options;
  iree_hal_accel_driver_options_initialize(&driver_options);

  iree_vm_instance_t* instance = NULL;
  IREE_RETURN_IF_ERROR(iree_vm_instance_create(IREE_VM_TYPE_CAPACITY_DEFAULT,
                                               host_allocator, &instance));

  // Create all executable loaders.
  // We need to list them in the order we want them to be tried.
  iree_hal_executable_loader_t* loaders[2] = {0};
  iree_host_size_t loader_count = 0;
  iree_status_t status = iree_ok_status();

  if (iree_status_is_ok(status)) {
    status = iree_hal_accel_executable_loader_create(instance, 0, NULL,
        host_allocator, &loaders[loader_count++]);
  }

  iree_hal_allocator_t* device_allocator = NULL;
  if (iree_status_is_ok(status)) {
    status = iree_hal_allocator_create_heap(iree_make_cstring_view("accel"),
                                            host_allocator, host_allocator,
                                            &device_allocator);
  }

  if (iree_status_is_ok(status)) {
    status = iree_hal_accel_driver_create(
        iree_make_cstring_view("accel"), &driver_options, loader_count,
        loaders, device_allocator, host_allocator, out_driver);
  }

  for (iree_host_size_t i = 0; i < loader_count; ++i) {
    iree_hal_executable_loader_release(loaders[i]);
  }
  iree_hal_allocator_release(device_allocator);
  iree_vm_instance_release(instance);

  return status;
}

IREE_API_EXPORT iree_status_t iree_hal_accel_driver_module_register(
    iree_hal_driver_registry_t* registry) {
  static const iree_hal_driver_factory_t factory = {
      .self = NULL, // no-op
      .enumerate = iree_hal_accel_driver_factory_enumerate,
      .try_create = iree_hal_accel_driver_factory_try_create,
  };
  return iree_hal_driver_registry_register_factory(registry, &factory);
}