// Copyright 2024 The IREE Authors
//
// Licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "iree/hal/drivers/accel/registration/accel_driver_module.h"

#include <inttypes.h>
#include <stddef.h>

#include "iree/base/api.h"
#include "iree/base/internal/flags.h"
#include "iree/base/tracing.h"
#include "iree/hal/api.h"
#include "iree/hal/drivers/accel/accel_driver.h"
#include "iree/hal/local/loaders/registration/init.h"
#include "iree/hal/local/plugins/registration/init.h"

IREE_FLAG(int32_t, hal_accel_worker_count, 0,
          "Number of worker threads for the accel HAL driver.\n"
          "0 will default to a platform-specific number.");

static iree_status_t iree_hal_accel_driver_factory_enumerate(
    void* self, iree_host_size_t* out_driver_info_count,
    const iree_hal_driver_info_t** out_driver_infos) {
  static const iree_hal_driver_info_t default_driver_info = {
      .driver_name = IREE_SVL("accel"),
      .full_name = IREE_SVL("Cloned accel driver using accel backend"),
  };
  *out_driver_info_count = 1;
  *out_driver_infos = &default_driver_info;
  return iree_ok_status();
}

static iree_status_t iree_hal_accel_driver_create_factory(
    void* self, iree_string_view_t driver_name,
    iree_allocator_t host_allocator, iree_hal_driver_t** out_driver) {
  if (!iree_string_view_equal(driver_name, IREE_SV("accel"))) {
    return iree_make_status(IREE_STATUS_UNAVAILABLE,
                            "no driver '%.*s' is provided by this factory",
                            (int)driver_name.size, driver_name.data);
  }
  IREE_TRACE_ZONE_BEGIN(z0);

  iree_hal_accel_driver_options_t driver_options;
  iree_hal_accel_driver_options_initialize(&driver_options);
  driver_options.default_device_options.worker_count =
      FLAG_hal_accel_worker_count;

  iree_hal_executable_plugin_manager_t* plugin_manager = NULL;
  iree_status_t status = iree_hal_executable_plugin_manager_create_from_flags(
      host_allocator, &plugin_manager);

  // Use a stack-allocated array for the loaders, which is the standard pattern.
  // A fixed-size capacity is used as the number of loaders is small.
  iree_hal_executable_loader_t* loaders[8] = {NULL};
  iree_host_size_t loader_count = 0;
  if (iree_status_is_ok(status)) {
    status = iree_hal_create_all_available_executable_loaders(
        plugin_manager, IREE_ARRAYSIZE(loaders), &loader_count, loaders,
        host_allocator);
  }

  iree_hal_allocator_t* device_allocator = NULL;
  if (iree_status_is_ok(status)) {
    // This function is declared in iree/hal/allocator.h and implemented in
    // allocator_heap.c, which is part of the iree::hal library.
    status = iree_hal_allocator_create_heap(iree_make_cstring_view("accel"),
                                            host_allocator, host_allocator,
                                            &device_allocator);
  }

  if (iree_status_is_ok(status)) {
    status = iree_hal_accel_driver_create(
        // 모듈이 "local" 디바이스를 타겟으로 컴파일되었으므로, 호환성을 위해
        // "local"로 식별되는 디바이스를 생성해야 합니다.
        // 실행 파일 포맷은 여전히 "accel-bytecode-fb"를 사용합니다.
        iree_make_cstring_view("local"), &driver_options, loader_count,
        loaders, device_allocator, host_allocator, out_driver);
  }

  // Release all resources that were created during this factory function.
  iree_hal_allocator_release(device_allocator);
  for (iree_host_size_t i = 0; i < loader_count; ++i) {
    iree_hal_executable_loader_release(loaders[i]);
  }
  // NOTE: loaders array is stack-allocated and does not need to be freed.
  iree_hal_executable_plugin_manager_release(plugin_manager);
  IREE_TRACE_ZONE_END(z0);
  return status;
}

IREE_API_EXPORT iree_status_t iree_hal_accel_driver_module_register(
    iree_hal_driver_registry_t* registry) {
  static const iree_hal_driver_factory_t factory = {
      .self = NULL,
      .enumerate = iree_hal_accel_driver_factory_enumerate,
      .try_create = iree_hal_accel_driver_create_factory,
  };
  return iree_hal_driver_registry_register_factory(registry, &factory);
}
