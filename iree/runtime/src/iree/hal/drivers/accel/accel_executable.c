// Copyright 2024 The IREE Authors
//
// Licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "iree/hal/drivers/accel/accel_executable.h"
#include "iree/hal/drivers/accel/accel_command_buffer.h"

#include <string.h>
#include "iree/hal/drivers/accel/accel_executable_loader.h"

#include "iree/base/api.h"
#include "iree/hal/local/local_executable.h"
#include "iree/vm/bytecode/module.h"

static void iree_hal_accel_executable_destroy(
    iree_hal_executable_t* base_executable) {
  iree_hal_accel_executable_t* executable =
      (iree_hal_accel_executable_t*)base_executable;
  iree_allocator_t host_allocator = executable->base.host_allocator;
  IREE_TRACE_ZONE_BEGIN(z0);

  for (iree_host_size_t i = 0; i < executable->worker_capacity; ++i) {
    iree_hal_vmvx_worker_state_deinitialize(
        &executable->worker_states[i]);
  }
  iree_vm_module_release(executable->bytecode_module);
  iree_hal_local_executable_deinitialize(&executable->base);
  iree_allocator_free(host_allocator, executable);

  IREE_TRACE_ZONE_END(z0);
}

static iree_status_t iree_hal_accel_executable_issue_call(
    iree_hal_local_executable_t* base_executable, iree_host_size_t ordinal,
    const iree_hal_executable_dispatch_state_v0_t* dispatch_state,
    const iree_hal_executable_workgroup_state_v0_t* workgroup_state,
    uint32_t worker_id) {
  return iree_make_status(
      IREE_STATUS_UNIMPLEMENTED,
      "CPU fallback dispatch not implemented for accel executables");
}

const iree_hal_local_executable_vtable_t iree_hal_accel_executable_vtable = {
    .base =
        {
            .destroy = iree_hal_accel_executable_destroy,
        },
    .issue_call = iree_hal_accel_executable_issue_call,
};

bool iree_hal_accel_executable_isa(iree_hal_executable_t* executable) {
  return iree_hal_resource_is(
      (iree_hal_resource_t*)executable,
      (const iree_hal_resource_vtable_t*)&iree_hal_accel_executable_vtable.base);
}