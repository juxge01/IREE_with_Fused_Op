// Copyright 2024 The IREE Authors
//
// Licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "iree/hal/drivers/accel/accel_executable_loader.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include "iree/hal/drivers/accel/accel_executable.h"
#include "iree/hal/local/local_executable.h"
#include "iree/modules/vmvx/module.h"
#include "iree/vm/bytecode/module.h"

//===----------------------------------------------------------------------===//
// iree_hal_accel_executable_loader_t
//===----------------------------------------------------------------------===//

typedef struct iree_hal_accel_executable_loader_t {
  iree_hal_executable_loader_t base;
  iree_allocator_t host_allocator;
  iree_vm_instance_t* instance;
  iree_host_size_t common_module_count;
  iree_vm_module_t* common_modules[];
} iree_hal_accel_executable_loader_t;

static const iree_hal_executable_loader_vtable_t
    iree_hal_accel_executable_loader_vtable;

static iree_status_t iree_hal_accel_executable_set_constants(
    iree_vm_context_t* context, iree_vm_module_t* bytecode_module,
    iree_host_size_t constant_count, const uint32_t* constants,
    iree_allocator_t host_allocator) {
  iree_vm_function_t set_function;
  iree_status_t status = iree_vm_module_lookup_function_by_name(
      bytecode_module, IREE_VM_FUNCTION_LINKAGE_EXPORT,
      iree_make_cstring_view("__set_constants"), &set_function);
  if (iree_status_is_not_found(status)) {
    iree_status_ignore(status);
    if (constant_count > 0) {
      return iree_make_status(IREE_STATUS_INVALID_ARGUMENT,
                              "executable has no executable-level constants "
                              "but %" PRIhsz " constants were provided",
                              constant_count);
    }
    return iree_ok_status();
  } else if (!iree_status_is_ok(status)) {
    return status;
  } else if (!constant_count || !constants) {
    return iree_make_status(IREE_STATUS_INVALID_ARGUMENT,
                            "executable requires executable-level constants "
                            "but none were provided");
  }

  IREE_TRACE_ZONE_BEGIN(z0);

  iree_vm_buffer_t buffer = {{0}};
  iree_vm_buffer_initialize(
      IREE_VM_BUFFER_ACCESS_ORIGIN_HOST,
      iree_make_byte_span((void*)constants,
                          constant_count * sizeof(*constants)),
      iree_allocator_null(), &buffer);

  uint8_t input_storage[64] = {0};
  iree_vm_list_t* inputs = NULL;
  iree_vm_type_def_t element_type =
      iree_vm_make_ref_type_def(iree_vm_buffer_type());
  status = iree_vm_list_initialize(
      iree_make_byte_span(input_storage, sizeof(input_storage)), &element_type,
      1, &inputs);
  if (iree_status_is_ok(status)) {
    iree_vm_ref_t buffer_ref = iree_vm_buffer_retain_ref(&buffer);
    status = iree_vm_list_push_ref_move(inputs, &buffer_ref);
  }

  if (iree_status_is_ok(status)) {
    status = iree_vm_invoke(context, set_function,
                            IREE_VM_INVOCATION_FLAG_TRACE_INLINE,
                            /*policy=*/NULL, inputs,
                            /*outputs=*/NULL, host_allocator);
  }

  if (inputs) {
    iree_vm_list_deinitialize(inputs);
  }

  iree_vm_buffer_deinitialize(&buffer);

  IREE_TRACE_ZONE_END(z0);
  return status;
}

iree_status_t iree_hal_vmvx_worker_state_initialize(
    iree_vm_instance_t* instance, iree_host_size_t module_count,
    iree_vm_module_t** modules, iree_vm_module_t* bytecode_module,
    const iree_hal_executable_params_t* executable_params, iree_allocator_t host_allocator,
    iree_hal_vmvx_worker_state_t* out_state) {
  IREE_ASSERT_ARGUMENT(out_state);
  IREE_TRACE_ZONE_BEGIN(z0);
  memset(out_state, 0, sizeof(*out_state));

  iree_vm_context_t* context = NULL;
  IREE_RETURN_AND_END_ZONE_IF_ERROR(
      z0, iree_vm_context_create_with_modules(
              instance, IREE_VM_CONTEXT_FLAG_NONE, module_count, modules,
              host_allocator, &context));

  iree_vm_module_t* vmvx_module = modules[0];
  iree_vm_module_state_t* vmvx_module_state = NULL;
  iree_status_t status = iree_vm_context_resolve_module_state(
      context, vmvx_module, &vmvx_module_state);

  if (iree_status_is_ok(status)) {
    status = iree_hal_accel_executable_set_constants(
        context, bytecode_module, executable_params->constant_count,
        executable_params->constants, host_allocator);
  }

  if (iree_status_is_ok(status)) {
    out_state->context = context;
    out_state->vmvx_module_state = vmvx_module_state;
  } else {
    iree_vm_context_release(context);
  }
  IREE_TRACE_ZONE_END(z0);
  return status;
}

void iree_hal_vmvx_worker_state_deinitialize(
    iree_hal_vmvx_worker_state_t* state) {
  IREE_ASSERT_ARGUMENT(state);
  IREE_TRACE_ZONE_BEGIN(z0);
  if (state->context) {
    iree_vm_context_release(state->context);
    state->context = NULL;
  }
  IREE_TRACE_ZONE_END(z0);
}

static iree_status_t iree_hal_accel_executable_verify_entry_point(
    iree_vm_function_t* entry_fn) {
  iree_vm_function_signature_t signature = iree_vm_function_signature(entry_fn);
  if (!iree_string_view_equal(
          signature.calling_convention,
          iree_make_cstring_view("0rrriiiiiiiii_v"))) {
    return iree_make_status(
        IREE_STATUS_INVALID_ARGUMENT,
        "executable entry point does not match the expected calling "
        "convention; expected '0rrriiiiiiiii_v' but got '%.*s', possible ABI "
        "version mismatch",
        (int)signature.calling_convention.size,
        signature.calling_convention.data);
  }
  return iree_ok_status();
}

static iree_hal_accel_executable_loader_t*
iree_hal_accel_executable_loader_cast(
    iree_hal_executable_loader_t* base_loader) {
  IREE_HAL_ASSERT_TYPE(base_loader, &iree_hal_accel_executable_loader_vtable);
  return (iree_hal_accel_executable_loader_t*)base_loader;
}

static iree_status_t iree_hal_accel_executable_loader_try_load(
    iree_hal_executable_loader_t* base_loader,
    const iree_hal_executable_params_t* executable_params,
    iree_host_size_t worker_capacity,
    iree_hal_executable_t** out_executable) {
  iree_hal_accel_executable_loader_t* executable_loader =
      iree_hal_accel_executable_loader_cast(base_loader);
  IREE_TRACE_ZONE_BEGIN(z0);

  iree_const_byte_span_t bytecode_module_data =
      executable_params->executable_data;

  iree_allocator_t bytecode_module_allocator;
  if (iree_all_bits_set(executable_params->caching_mode,
                        IREE_HAL_EXECUTABLE_CACHING_MODE_ALIAS_PROVIDED_DATA)) {
    bytecode_module_allocator = iree_allocator_null();
  } else {
    bytecode_module_allocator = executable_loader->host_allocator;
    IREE_RETURN_AND_END_ZONE_IF_ERROR(
        z0, iree_allocator_clone(executable_loader->host_allocator,
                                 executable_params->executable_data,
                                 (void**)&bytecode_module_data.data));
  }

  iree_vm_module_t* bytecode_module = NULL;
  iree_status_t status = iree_vm_bytecode_module_create(
      executable_loader->instance, executable_params->executable_data,
      bytecode_module_allocator, executable_loader->host_allocator,
      &bytecode_module);

  if (iree_status_is_ok(status)) {
    iree_host_size_t context_module_count =
        executable_loader->common_module_count + 1;
    iree_vm_module_t** context_modules = (iree_vm_module_t**)iree_alloca(
        context_module_count * sizeof(iree_vm_module_t*));
    memcpy(context_modules, executable_loader->common_modules,
           executable_loader->common_module_count *
               sizeof(executable_loader->common_modules[0]));
    context_modules[context_module_count - 1] = bytecode_module;

    iree_host_size_t entry_count =
        iree_vm_module_signature(bytecode_module).export_function_count;

    iree_hal_accel_executable_t* executable = NULL;
    const iree_host_size_t entry_fn_ordinals_size = iree_host_align(
        entry_count * sizeof(*executable->entry_fn_ordinals), 8);
    const iree_host_size_t dispatch_attrs_size = iree_host_align(
        entry_count * sizeof(*executable->base.dispatch_attrs), 8);
    const iree_host_size_t worker_states_size = iree_host_align(
        worker_capacity * sizeof(iree_hal_vmvx_worker_state_t), 8);
    const iree_host_size_t total_size =
        sizeof(*executable) + entry_fn_ordinals_size + dispatch_attrs_size +
        worker_states_size;
    status = iree_allocator_malloc(executable_loader->host_allocator,
                                   total_size, (void**)&executable);
    iree_hal_executable_dispatch_attrs_v0_t* dispatch_attrs = NULL;
    if (iree_status_is_ok(status)) {
      uint8_t* ptr =
          (uint8_t*)executable + sizeof(*executable) + entry_fn_ordinals_size;
      dispatch_attrs = (iree_hal_executable_dispatch_attrs_v0_t*)ptr;
      ptr += dispatch_attrs_size;
      iree_hal_local_executable_initialize(
          &iree_hal_accel_executable_vtable, executable_loader->host_allocator,
          &executable->base);
      executable->base.dispatch_attrs = dispatch_attrs;

      executable->worker_capacity = worker_capacity;
      executable->worker_states = (iree_hal_vmvx_worker_state_t*)ptr;
      ptr += worker_states_size;

      executable->bytecode_module = bytecode_module;
      executable->entry_fn_count = entry_count;
      for (iree_host_size_t i = 0; i < executable->entry_fn_count; ++i) {
        iree_vm_function_t entry_fn;
        status = iree_vm_module_lookup_function_by_ordinal(
            bytecode_module, IREE_VM_FUNCTION_LINKAGE_EXPORT, i, &entry_fn);
        if (!iree_status_is_ok(status)) break;
        status = iree_hal_accel_executable_verify_entry_point(&entry_fn);
        if (!iree_status_is_ok(status)) break;
        executable->entry_fn_ordinals[i] = entry_fn.ordinal;
      }
    }

    if (iree_status_is_ok(status)) {
      for (iree_host_size_t i = 0; i < worker_capacity; ++i) {
        status = iree_hal_vmvx_worker_state_initialize(
            executable_loader->instance, context_module_count, context_modules,
            bytecode_module, executable_params,
            executable_loader->host_allocator,
            &executable->worker_states[i]);
        if (!iree_status_is_ok(status)) break;
      }
    }

    if (iree_status_is_ok(status)) {
      *out_executable = (iree_hal_executable_t*)executable;
    } else {
      iree_hal_local_executable_deinitialize(&executable->base);
      iree_hal_executable_release((iree_hal_executable_t*)executable);
    }
  }

  if (!iree_status_is_ok(status)) {
    // Must release the module here as the executable takes ownership.
    iree_vm_module_release(bytecode_module);
  }

  IREE_TRACE_ZONE_END(z0);
  return status;
}

static void iree_hal_accel_executable_loader_destroy(
    iree_hal_executable_loader_t* base_loader) {
  iree_hal_accel_executable_loader_t* loader =
      iree_hal_accel_executable_loader_cast(base_loader);
  iree_allocator_t host_allocator = loader->host_allocator;
  IREE_TRACE_ZONE_BEGIN(z0);

  for (iree_host_size_t i = 0; i < loader->common_module_count; ++i) {
    iree_vm_module_release(loader->common_modules[i]);
  }
  iree_vm_instance_release(loader->instance);
  iree_allocator_free(host_allocator, loader);

  IREE_TRACE_ZONE_END(z0);
}

iree_status_t iree_hal_accel_executable_loader_create(
    iree_vm_instance_t* instance, iree_host_size_t user_module_count,
    iree_vm_module_t** user_modules, iree_allocator_t host_allocator,
    iree_hal_executable_loader_t** out_executable_loader) {
  IREE_ASSERT_ARGUMENT(instance);
  IREE_ASSERT_ARGUMENT(!user_module_count || user_modules);
  IREE_ASSERT_ARGUMENT(out_executable_loader);
  *out_executable_loader = NULL;
  IREE_TRACE_ZONE_BEGIN(z0);

  iree_vm_module_t* vmvx_module = NULL;
  IREE_RETURN_IF_ERROR(
      iree_vmvx_module_create(instance, host_allocator, &vmvx_module));

  iree_host_size_t common_module_count = 1 + user_module_count;
  iree_hal_accel_executable_loader_t* loader = NULL;
  iree_status_t status = iree_allocator_malloc(
      host_allocator,
      sizeof(*loader) +
          common_module_count * sizeof(loader->common_modules[0]),
      (void**)&loader);
  if (iree_status_is_ok(status)) {
    iree_hal_executable_loader_initialize(
        &iree_hal_accel_executable_loader_vtable,
        iree_hal_executable_import_provider_null(), &loader->base);
    loader->host_allocator = host_allocator;
    loader->instance = instance;
    iree_vm_instance_retain(instance);

    loader->common_module_count = common_module_count;
    loader->common_modules[0] = vmvx_module;
    iree_vm_module_retain(vmvx_module);
    for (iree_host_size_t i = 0; i < user_module_count; ++i) {
      loader->common_modules[1 + i] = user_modules[i];
      iree_vm_module_retain(user_modules[i]);
    }

    *out_executable_loader = (iree_hal_executable_loader_t*)loader;
  }

  iree_vm_module_release(vmvx_module);
  IREE_TRACE_ZONE_END(z0);
  return iree_ok_status();
}

static bool iree_hal_accel_executable_loader_query_support(
    iree_hal_executable_loader_t* base_executable_loader,
    iree_hal_executable_caching_mode_t caching_mode,
    iree_string_view_t executable_format) {
  return iree_string_view_equal(executable_format,
                                iree_make_cstring_view("accel-bytecode-fb"));
}

static const iree_hal_executable_loader_vtable_t
    iree_hal_accel_executable_loader_vtable = {
        .destroy = iree_hal_accel_executable_loader_destroy,
        .query_support = iree_hal_accel_executable_loader_query_support,
        .try_load = iree_hal_accel_executable_loader_try_load,
};