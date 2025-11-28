// Copyright 2021 The IREE Authors
//
// Licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <inttypes.h>

#include "/home/jueun/park/renode/renode-latest/renode-dpi-examples/samples/axi_sa16/axi_sa16/sim_lib.h"
#include "iree/base/internal/arena.h"
#include "iree/base/internal/atomics.h"
#include "iree/base/internal/time.h"
#include "iree/base/internal/cpu.h"
#include "iree/base/internal/synchronization.h"
#include "iree/hal/drivers/accel/accel_event.h"
#include "iree/hal/drivers/accel/accel_semaphore.h"
#include "iree/hal/drivers/accel/accel_command_buffer.h"
#include "iree/hal/local/executable_environment.h"
#include "iree/hal/local/inline_command_buffer.h"
#include "iree/hal/local/local_executable_cache.h"
#include "iree/task/executor.h"
#include "iree/task/scope.h"
#include "iree/task/task.h"
#include "iree/task/topology.h"
#include "iree/hal/utils/deferred_command_buffer.h"
#include "iree/hal/utils/file_registry.h"
#include "iree/hal/drivers/accel/accel_device.h"
#include "iree/hal/utils/file_transfer.h"

#ifndef ACCEL_LOGI
#define ACCEL_LOGI(fmt, ...)  fprintf(stderr, "[ACCEL][I] " fmt "\n", ##__VA_ARGS__)
#define ACCEL_LOGW(fmt, ...)  fprintf(stderr, "[ACCEL][W] " fmt "\n", ##__VA_ARGS__)
#define ACCEL_LOGE(fmt, ...)  fprintf(stderr, "[ACCEL][E] " fmt "\n", ##__VA_ARGS__)
#endif

static int accel_device_debug_enabled(void) {
  static int inited = 0, enabled = 0;
  if (!inited) {
    const char* e = getenv("ACCEL_DEBUG");
    enabled = (e && *e && strcmp(e, "0") != 0) ? 1 : 0;
    inited = 1;
  }
  return enabled;
}

typedef struct iree_hal_accel_device_t {
  iree_hal_resource_t resource;
  iree_string_view_t identifier;

  iree_allocator_t host_allocator;
  iree_hal_allocator_t* device_allocator;

  // Optional provider used for creating/configuring collective channels.
  iree_hal_channel_provider_t* channel_provider;

  // Block pool used for command buffers with a larger block size (as command
  // buffers can contain inlined data uploads).
  iree_arena_block_pool_t large_block_pool;

  // Shared semaphore state used to emulate OS-level primitives. This backend
  // is intended to run on bare-metal systems where we need to perform all
  // synchronization ourselves.
  iree_hal_accel_semaphore_state_t semaphore_state;

  // --- NEW: Task executor for async execution ---
  iree_task_executor_t* executor;
  iree_task_topology_t topology;
  iree_task_scope_t task_scope;

  iree_host_size_t loader_count;
  iree_hal_executable_loader_t* loaders[];
} iree_hal_accel_device_t;

static const iree_hal_device_vtable_t iree_hal_accel_device_vtable;

static iree_hal_accel_device_t* iree_hal_accel_device_cast(
    iree_hal_device_t* base_value) {
  IREE_HAL_ASSERT_TYPE(base_value, &iree_hal_accel_device_vtable);
  return (iree_hal_accel_device_t*)base_value;
}

void iree_hal_accel_device_params_initialize(
    iree_hal_accel_device_params_t* out_params) {
  memset(out_params, 0, sizeof(*out_params));
  out_params->arena_block_size = 32 * 1024;
  out_params->queue_count = 1;
}

static iree_status_t iree_hal_accel_device_check_params(
    const iree_hal_accel_device_params_t* params) {
  if (params->arena_block_size < 4096) {
    return iree_make_status(IREE_STATUS_INVALID_ARGUMENT,
                            "arena block size too small (< 4096 bytes)");
  }
  if (params->queue_count == 0) {
    return iree_make_status(IREE_STATUS_INVALID_ARGUMENT,
                            "at least one queue is required");
  }
  return iree_ok_status();
}

iree_status_t iree_hal_accel_device_create(
    iree_string_view_t identifier, const iree_hal_accel_device_params_t* params,
    iree_host_size_t loader_count, iree_hal_executable_loader_t** loaders,
    iree_hal_allocator_t* device_allocator, iree_allocator_t host_allocator,
    iree_hal_device_t** out_device) {
  IREE_ASSERT_ARGUMENT(params);
  IREE_ASSERT_ARGUMENT(!loader_count || loaders);
  IREE_ASSERT_ARGUMENT(device_allocator);
  IREE_ASSERT_ARGUMENT(out_device);
  *out_device = NULL;
  IREE_TRACE_ZONE_BEGIN(z0);

  IREE_RETURN_AND_END_ZONE_IF_ERROR(z0,
                                    iree_hal_accel_device_check_params(params));

  iree_hal_accel_device_t* device = NULL;
  iree_host_size_t struct_size =
      sizeof(*device) + loader_count * sizeof(*device->loaders);
  iree_host_size_t total_size = struct_size + identifier.size;
  iree_status_t status = iree_allocator_malloc_aligned(
      host_allocator, total_size, iree_alignof(iree_hal_accel_device_t), 0,
      (void**)&device);
  if (iree_status_is_ok(status)) {
    memset(device, 0, total_size);
    iree_hal_resource_initialize(&iree_hal_accel_device_vtable,
                                 &device->resource);
    iree_string_view_append_to_buffer(identifier, &device->identifier,
                                      (char*)device + struct_size);
    device->host_allocator = host_allocator;
    device->device_allocator = device_allocator;
    iree_hal_allocator_retain(device_allocator);
    iree_arena_block_pool_initialize(params->arena_block_size, host_allocator,
                                     &device->large_block_pool);

    device->loader_count = loader_count;
    for (iree_host_size_t i = 0; i < device->loader_count; ++i) {
      device->loaders[i] = loaders[i];
      iree_hal_executable_loader_retain(device->loaders[i]);
    }

    iree_hal_accel_semaphore_state_initialize(&device->semaphore_state);

    // --- NEW: Create a persistent task executor ---
    iree_task_scope_initialize(IREE_SV("accel_device"), IREE_TASK_SCOPE_FLAG_NONE,
                               &device->task_scope);

    iree_task_topology_initialize_from_group_count(params->queue_count,
                                                   &device->topology);
    iree_task_executor_options_t executor_options;
    iree_task_executor_options_initialize(&executor_options);
    status = iree_task_executor_create(executor_options, &device->topology,
                                       host_allocator, &device->executor);
    if (!iree_status_is_ok(status)) {
      // Fallback to synchronous execution? For now, fail.
    }
  }

  if (iree_status_is_ok(status)) {
    *out_device = (iree_hal_device_t*)device;
    if (accel_device_debug_enabled()) {
      ACCEL_LOGI("SA16 accel device ready (id='%.*s')",
                 (int)device->identifier.size, device->identifier.data);
    }
  } else {
    iree_hal_device_release((iree_hal_device_t*)device);
  }
  IREE_TRACE_ZONE_END(z0);
  return status;
}

static void iree_hal_accel_device_destroy(iree_hal_device_t* base_device) {
  iree_hal_accel_device_t* device = iree_hal_accel_device_cast(base_device);
  iree_allocator_t host_allocator = iree_hal_device_host_allocator(base_device);
  IREE_TRACE_ZONE_BEGIN(z0);

  iree_task_executor_release(device->executor);
  iree_task_topology_deinitialize(&device->topology);
  iree_task_scope_deinitialize(&device->task_scope);

  iree_hal_accel_semaphore_state_deinitialize(&device->semaphore_state);

  for (iree_host_size_t i = 0; i < device->loader_count; ++i) {
    iree_hal_executable_loader_release(device->loaders[i]);
  }

  iree_hal_allocator_release(device->device_allocator);
  iree_hal_channel_provider_release(device->channel_provider);

  iree_arena_block_pool_deinitialize(&device->large_block_pool);

  iree_allocator_free_aligned(host_allocator, device);

  IREE_TRACE_ZONE_END(z0);
}

static iree_string_view_t iree_hal_accel_device_id(
    iree_hal_device_t* base_device) {
  iree_hal_accel_device_t* device = iree_hal_accel_device_cast(base_device);
  return device->identifier;
}

static iree_allocator_t iree_hal_accel_device_host_allocator(
    iree_hal_device_t* base_device) {
  iree_hal_accel_device_t* device = iree_hal_accel_device_cast(base_device);
  return device->host_allocator;
}

static iree_hal_allocator_t* iree_hal_accel_device_allocator(
    iree_hal_device_t* base_device) {
  iree_hal_accel_device_t* device = iree_hal_accel_device_cast(base_device);
  return device->device_allocator;
}

static void iree_hal_accel_replace_device_allocator(
    iree_hal_device_t* base_device, iree_hal_allocator_t* new_allocator) {
  iree_hal_accel_device_t* device = iree_hal_accel_device_cast(base_device);
  iree_hal_allocator_retain(new_allocator);
  iree_hal_allocator_release(device->device_allocator);
  device->device_allocator = new_allocator;
}

static void iree_hal_accel_replace_channel_provider(
    iree_hal_device_t* base_device, iree_hal_channel_provider_t* new_provider) {
  iree_hal_accel_device_t* device = iree_hal_accel_device_cast(base_device);
  iree_hal_channel_provider_retain(new_provider);
  iree_hal_channel_provider_release(device->channel_provider);
  device->channel_provider = new_provider;
}

static iree_status_t iree_hal_accel_device_trim(iree_hal_device_t* base_device) {
  iree_hal_accel_device_t* device = iree_hal_accel_device_cast(base_device);
  return iree_hal_allocator_trim(device->device_allocator);
}

static iree_status_t iree_hal_accel_device_query_i64(
    iree_hal_device_t* base_device, iree_string_view_t category,
    iree_string_view_t key, int64_t* out_value) {
  iree_hal_accel_device_t* device = iree_hal_accel_device_cast(base_device);
  *out_value = 0;

  if (iree_string_view_equal(category, IREE_SV("hal.device.id"))) {
    *out_value =
        iree_string_view_match_pattern(device->identifier, key) ? 1 : 0;
    return iree_ok_status();
  }

  if (iree_string_view_equal(category, IREE_SV("hal.executable.format"))) {
    *out_value =
        iree_hal_query_any_executable_loader_support(
            device->loader_count, device->loaders, /*caching_mode=*/0, key)
            ? 1
            : 0;
    return iree_ok_status();
  }

  if (iree_string_view_equal(category, IREE_SV("hal.device"))) {
    if (iree_string_view_equal(key, IREE_SV("concurrency"))) {
      *out_value = 1;
      return iree_ok_status();
    }
  } else if (iree_string_view_equal(category, IREE_SV("hal.dispatch"))) {
    if (iree_string_view_equal(key, IREE_SV("concurrency"))) {
      *out_value = 1;
      return iree_ok_status();
    }
  } else if (iree_string_view_equal(category, IREE_SV("hal.cpu"))) {
    return iree_cpu_lookup_data_by_key(key, out_value);
  }

  return iree_make_status(
      IREE_STATUS_NOT_FOUND,
      "unknown device configuration key value '%.*s :: %.*s'",
      (int)category.size, category.data, (int)key.size, key.data);
}

static iree_status_t iree_hal_accel_device_create_channel(
    iree_hal_device_t* base_device, iree_hal_queue_affinity_t queue_affinity,
    iree_hal_channel_params_t params, iree_hal_channel_t** out_channel) {
  return iree_make_status(IREE_STATUS_UNIMPLEMENTED,
                          "collectives not implemented");
}

static iree_status_t iree_hal_accel_device_create_command_buffer(
    iree_hal_device_t* base_device, iree_hal_command_buffer_mode_t mode,
    iree_hal_command_category_t command_categories,
    iree_hal_queue_affinity_t queue_affinity, iree_host_size_t binding_capacity,
    iree_hal_command_buffer_t** out_command_buffer) {
  if (iree_all_bits_set(mode,
                        IREE_HAL_COMMAND_BUFFER_MODE_ALLOW_INLINE_EXECUTION)) {
    return iree_hal_inline_command_buffer_create(
        iree_hal_device_allocator(base_device), mode, command_categories,
        queue_affinity, binding_capacity,
        iree_hal_device_host_allocator(base_device), out_command_buffer);
  } else {
    iree_hal_accel_device_t* device = iree_hal_accel_device_cast(base_device);
    return iree_hal_accel_command_buffer_create(
        iree_hal_device_allocator(base_device), &device->task_scope, mode,
        command_categories, queue_affinity, binding_capacity,
        &device->large_block_pool, device->host_allocator, out_command_buffer);
  }
}

static iree_status_t iree_hal_accel_device_create_event(
    iree_hal_device_t* base_device, iree_hal_queue_affinity_t queue_affinity,
    iree_hal_event_flags_t flags, iree_hal_event_t** out_event) {
  return iree_hal_accel_event_create(queue_affinity, flags,
                                    iree_hal_device_host_allocator(base_device),
                                    out_event);
}

static iree_status_t iree_hal_accel_device_create_executable_cache(
    iree_hal_device_t* base_device, iree_string_view_t identifier,
    iree_loop_t loop, iree_hal_executable_cache_t** out_executable_cache) {
  iree_hal_accel_device_t* device = iree_hal_accel_device_cast(base_device);
  return iree_hal_local_executable_cache_create(
      identifier, /*worker_capacity=*/1, device->loader_count, device->loaders,
      iree_hal_device_host_allocator(base_device), out_executable_cache);
}

static iree_status_t iree_hal_accel_device_import_file(
    iree_hal_device_t* base_device, iree_hal_queue_affinity_t queue_affinity,
    iree_hal_memory_access_t access, iree_io_file_handle_t* handle,
    iree_hal_external_file_flags_t flags, iree_hal_file_t** out_file) {
  return iree_hal_file_from_handle(
      iree_hal_device_allocator(base_device), queue_affinity, access, handle,
      iree_hal_device_host_allocator(base_device), out_file);
}

static iree_status_t iree_hal_accel_device_create_semaphore(
    iree_hal_device_t* base_device, uint64_t initial_value,
    iree_hal_semaphore_flags_t flags, iree_hal_semaphore_t** out_semaphore) {
  iree_hal_accel_device_t* device = iree_hal_accel_device_cast(base_device);
  return iree_hal_accel_semaphore_create(&device->semaphore_state, initial_value,
                                        device->host_allocator, out_semaphore);
}

static iree_hal_semaphore_compatibility_t
iree_hal_accel_device_query_semaphore_compatibility(
    iree_hal_device_t* base_device, iree_hal_semaphore_t* semaphore) {
  // The synchronous submission queue handles all semaphores as if host-side.
  return IREE_HAL_SEMAPHORE_COMPATIBILITY_HOST_ONLY;
}

static iree_status_t iree_hal_accel_device_queue_alloca(
    iree_hal_device_t* base_device, iree_hal_queue_affinity_t queue_affinity,
    const iree_hal_semaphore_list_t wait_semaphore_list,
    const iree_hal_semaphore_list_t signal_semaphore_list,
    iree_hal_allocator_pool_t pool, iree_hal_buffer_params_t params,
    iree_device_size_t allocation_size, iree_hal_alloca_flags_t flags,
    iree_hal_buffer_t** IREE_RESTRICT out_buffer) {
  // TODO(benvanik): queue-ordered allocations.
  IREE_RETURN_IF_ERROR(iree_hal_semaphore_list_wait(wait_semaphore_list,
                                                    iree_infinite_timeout()));
  IREE_RETURN_IF_ERROR(
      iree_hal_allocator_allocate_buffer(iree_hal_device_allocator(base_device),
                                         params, allocation_size, out_buffer));
  IREE_RETURN_IF_ERROR(iree_hal_semaphore_list_signal(signal_semaphore_list));
  return iree_ok_status();
}

static iree_status_t iree_hal_accel_device_queue_dealloca(
    iree_hal_device_t* base_device, iree_hal_queue_affinity_t queue_affinity,
    const iree_hal_semaphore_list_t wait_semaphore_list,
    const iree_hal_semaphore_list_t signal_semaphore_list,
    iree_hal_buffer_t* buffer, iree_hal_dealloca_flags_t flags) {
  // TODO(benvanik): queue-ordered allocations.
  // This is a synchronous deallocation. For now, just treat it as a barrier.
  return iree_hal_device_queue_barrier(base_device, queue_affinity,
                                       wait_semaphore_list,
                                       signal_semaphore_list,
                                       IREE_HAL_EXECUTE_FLAG_NONE);
}

static iree_status_t iree_hal_accel_device_queue_read(
    iree_hal_device_t* base_device, iree_hal_queue_affinity_t queue_affinity,
    const iree_hal_semaphore_list_t wait_semaphore_list,
    const iree_hal_semaphore_list_t signal_semaphore_list,
    iree_hal_file_t* source_file, uint64_t source_offset,
    iree_hal_buffer_t* target_buffer, iree_device_size_t target_offset,
    iree_device_size_t length, iree_hal_read_flags_t flags) {
  // TODO: expose streaming chunk count/size options.
  iree_status_t loop_status = iree_ok_status();
  iree_hal_file_transfer_options_t options = {
      .loop = iree_loop_inline(&loop_status),
      .chunk_count = IREE_HAL_FILE_TRANSFER_CHUNK_COUNT_DEFAULT,
      .chunk_size = IREE_HAL_FILE_TRANSFER_CHUNK_SIZE_DEFAULT,
  };
  IREE_RETURN_IF_ERROR(iree_hal_device_queue_read_streaming(
      base_device, queue_affinity, wait_semaphore_list, signal_semaphore_list,
      source_file, source_offset, target_buffer, target_offset, length, flags,
      options));
  return loop_status;
}

static iree_status_t iree_hal_accel_device_queue_write(
    iree_hal_device_t* base_device, iree_hal_queue_affinity_t queue_affinity,
    const iree_hal_semaphore_list_t wait_semaphore_list,
    const iree_hal_semaphore_list_t signal_semaphore_list,
    iree_hal_buffer_t* source_buffer, iree_device_size_t source_offset,
    iree_hal_file_t* target_file, uint64_t target_offset,
    iree_device_size_t length, iree_hal_write_flags_t flags) {
  // TODO: expose streaming chunk count/size options.
  iree_status_t loop_status = iree_ok_status();
  iree_hal_file_transfer_options_t options = {
      .loop = iree_loop_inline(&loop_status),
      .chunk_count = IREE_HAL_FILE_TRANSFER_CHUNK_COUNT_DEFAULT,
      .chunk_size = IREE_HAL_FILE_TRANSFER_CHUNK_SIZE_DEFAULT,
  };
  IREE_RETURN_IF_ERROR(iree_hal_device_queue_write_streaming(
      base_device, queue_affinity, wait_semaphore_list, signal_semaphore_list,
      source_buffer, source_offset, target_file, target_offset, length, flags,
      options));
  return loop_status;
}

// OFFLOADING 구현
typedef struct {
  iree_notification_t* notification;
  iree_atomic_int32_t done;
} completion_state_t;

static iree_status_t signal_notification_task_fn(
    void* user_context, iree_task_t* task,
    iree_task_submission_t* pending_submission) {
  completion_state_t* st = (completion_state_t*)user_context;
  iree_atomic_store(&st->done, 1, iree_memory_order_release);
  iree_notification_post(st->notification, IREE_STATUS_OK);
  return iree_ok_status();
}

static bool completion_predicate(void* user_data) {
  completion_state_t* st = (completion_state_t*)user_data;
  return iree_atomic_load(&st->done, iree_memory_order_acquire) != 0;
}

static iree_status_t iree_hal_accel_device_apply_deferred_command_buffer(
    iree_hal_accel_device_t* device, iree_hal_command_buffer_t* command_buffer,
    iree_hal_buffer_binding_table_t binding_table) {
  // If there were no deferred command buffers no-op this call - they've already
  // been issued.
  // NOTE: this is just a size guess; we could be more precise by walking the
  // command buffer and summing the sizes of all commands, but this is good
  // enough for now. The arena will grow if needed.
  // been issued.
  if (!command_buffer ||
      !iree_hal_deferred_command_buffer_isa(command_buffer)) {
    return iree_ok_status();
  }

  // Stack allocate storage for an inline command buffer we'll use to replay
  // the deferred command buffers. We want to reset it between each apply so
  // that we don't get state carrying across.
  iree_host_size_t storage_size = iree_hal_inline_command_buffer_size(
      iree_hal_command_buffer_mode(command_buffer) |
          IREE_HAL_COMMAND_BUFFER_MODE_ONE_SHOT |
          IREE_HAL_COMMAND_BUFFER_MODE_ALLOW_INLINE_EXECUTION |
          // NOTE: we need to validate if a binding table is provided as
          // the bindings were not known when it was originally recorded.
          (iree_hal_buffer_binding_table_is_empty(binding_table)
               ? IREE_HAL_COMMAND_BUFFER_MODE_UNVALIDATED
               : 0),
      /*binding_capacity=*/0);
  iree_byte_span_t storage =
      iree_make_byte_span(iree_alloca(storage_size), storage_size);

  // NOTE: we run unvalidated as inline command buffers don't support
  // binding tables and can be validated entirely while recording.
  iree_hal_command_buffer_t* inline_command_buffer = NULL;
  IREE_RETURN_IF_ERROR(iree_hal_inline_command_buffer_initialize(
      device->device_allocator,
      iree_hal_command_buffer_mode(command_buffer) |
          IREE_HAL_COMMAND_BUFFER_MODE_ONE_SHOT |
          IREE_HAL_COMMAND_BUFFER_MODE_ALLOW_INLINE_EXECUTION |
          // NOTE: we need to validate if a binding table is provided as the
          // bindings were not known when it was originally recorded.
          (iree_hal_buffer_binding_table_is_empty(binding_table)
               ? IREE_HAL_COMMAND_BUFFER_MODE_UNVALIDATED
               : 0),
      iree_hal_command_buffer_allowed_categories(command_buffer),
      IREE_HAL_QUEUE_AFFINITY_ANY,
      /*binding_capacity=*/0, device->host_allocator, storage,
      &inline_command_buffer));

  iree_status_t status = iree_hal_deferred_command_buffer_apply(
      command_buffer, inline_command_buffer, binding_table);

  iree_hal_inline_command_buffer_deinitialize(inline_command_buffer);
  return status;
}

static iree_status_t iree_hal_accel_device_queue_execute(
    iree_hal_device_t* base_device, iree_hal_queue_affinity_t queue_affinity,
    const iree_hal_semaphore_list_t wait_semaphore_list,
    const iree_hal_semaphore_list_t signal_semaphore_list,
    iree_hal_command_buffer_t* command_buffer,
    iree_hal_buffer_binding_table_t binding_table,
    iree_hal_execute_flags_t flags) {
  iree_hal_accel_device_t* device = iree_hal_accel_device_cast(base_device);

  // Wait for semaphores to be signaled before performing any work.
  IREE_RETURN_IF_ERROR(iree_hal_accel_semaphore_multi_wait(
      &device->semaphore_state, IREE_HAL_WAIT_MODE_ALL, wait_semaphore_list,
      iree_infinite_timeout()));

  if (command_buffer == NULL) {
    return iree_hal_accel_semaphore_multi_signal(&device->semaphore_state,
                                                 signal_semaphore_list);
  }

  iree_status_t status = iree_ok_status();
  
  if (iree_hal_accel_command_buffer_isa(command_buffer)) {
    // 임시(동기) 실행기를 만들어 해당 배치를 실행한다.
    // 호스트 시간 측정을 시작합니다.
    // int64_t host_start_ns = iree_time_now();

    iree_task_topology_t topology;
    iree_task_topology_initialize_from_group_count(2, &topology);

    iree_task_executor_options_t executor_options;
    iree_task_executor_options_initialize(&executor_options);

    iree_task_executor_t* executor = NULL;
    status = iree_task_executor_create(executor_options, &topology,
                                       device->host_allocator, &executor);
    iree_task_topology_deinitialize(&topology);
    if (!iree_status_is_ok(status)) {
      return status;
    }

    // 완료 통지를 위한 notification과 상태.
    iree_notification_t completion_notification;
    iree_notification_initialize(&completion_notification);

    completion_state_t st = {0};
    st.notification = &completion_notification;
    iree_atomic_store(&st.done, 0, iree_memory_order_relaxed);

    // retire 배리어 + 완료 콜 작업 준비.
    // 1) 제출 준비
    iree_task_submission_t submission;
    iree_task_submission_initialize(&submission);

    // 2) 완료 signal_task 준비 (스코프는 device->task_scope 사용)
    iree_task_call_t signal_task;
    iree_task_call_initialize(
        &device->task_scope,
        iree_task_make_call_closure(signal_notification_task_fn, &st),
        &signal_task);

    // 3) 그래프 발행 (마지막 completion에 signal_task 연결)
    status = iree_hal_accel_command_buffer_issue(
        command_buffer, /*queue_state=*/NULL, &signal_task.header, &submission);
    if (iree_status_is_ok(status)) {
      iree_task_executor_submit(executor, &submission);

      // mobilenetv2 만 가능한 코드 (왜지 // 모델 문제인가?)
      bool notified = iree_notification_await(&completion_notification,
                                              completion_predicate, &st,
                                              iree_infinite_timeout());
      if (!notified) {
        status = iree_make_status(IREE_STATUS_DEADLINE_EXCEEDED,
                                  "wait for completion timed out");
      }
    }

    // 4) 필요 시: submission 정리(제출 뒤 로컬 큐는 비워도 됨)
    // iree_task_submission_deinitialize(&submission);

    iree_notification_deinitialize(&completion_notification);
    iree_task_executor_release(executor);

  }
  // Run all deferred command buffers - any we could have run inline we already
  // did during recording.
  IREE_RETURN_IF_ERROR(iree_hal_accel_device_apply_deferred_command_buffer(
      device, command_buffer, binding_table));

  // Signal all semaphores now that batch work has completed.
  IREE_RETURN_IF_ERROR(iree_hal_accel_semaphore_multi_signal(
      &device->semaphore_state, signal_semaphore_list));

  return status;
}

// static iree_status_t iree_hal_accel_device_queue_execute(
//     iree_hal_device_t* base_device, iree_hal_queue_affinity_t queue_affinity,
//     const iree_hal_semaphore_list_t wait_semaphore_list,
//     const iree_hal_semaphore_list_t signal_semaphore_list,
//     iree_hal_command_buffer_t* command_buffer,
//     iree_hal_buffer_binding_table_t binding_table,
//     iree_hal_execute_flags_t flags) {
//   iree_hal_accel_device_t* device = iree_hal_accel_device_cast(base_device);

//   // 1) 대기 세마포어 처리
//   IREE_RETURN_IF_ERROR(iree_hal_accel_semaphore_multi_wait(
//       &device->semaphore_state, IREE_HAL_WAIT_MODE_ALL, wait_semaphore_list,
//       iree_infinite_timeout()));

//   if (command_buffer == NULL) {
//     // 실행할 커맨드가 없으면 바로 signal
//     return iree_hal_accel_semaphore_multi_signal(&device->semaphore_state,
//                                                  signal_semaphore_list);
//   }

//   iree_status_t status = iree_ok_status();

//   if (iree_hal_accel_command_buffer_isa(command_buffer)) {
//     // 2) 완료 통지를 위한 notification/state
//     iree_notification_t completion_notification;
//     iree_notification_initialize(&completion_notification);

//     completion_state_t st = {0};
//     st.notification = &completion_notification;
//     iree_atomic_store(&st.done, 0, iree_memory_order_relaxed);

//     // 3) 제출 준비 (영구 task scope / executor 사용)
//     iree_task_submission_t submission;
//     iree_task_submission_initialize(&submission);

//     iree_task_call_t signal_task;
//     iree_task_call_initialize(
//         &device->task_scope,
//         iree_task_make_call_closure(signal_notification_task_fn, &st),
//         &signal_task);

//     // 4) 그래프 발행(마지막에 signal_task를 completion으로 연결)
//     status = iree_hal_accel_command_buffer_issue(
//         command_buffer, /*queue_state=*/NULL, &signal_task.header, &submission);
//     if (iree_status_is_ok(status)) {
//       // ★ 임시 executor 만들지 말고, 영구 executor로 제출
//       iree_task_executor_submit(device->executor, &submission);

//       // 완료 대기
//       bool notified = iree_notification_await(&completion_notification,
//                                               completion_predicate, &st,
//                                               iree_infinite_timeout());
//       if (!notified) {
//         status = iree_make_status(IREE_STATUS_DEADLINE_EXCEEDED,
//                                   "wait for completion timed out");
//       }
//     }

//     // 로컬 큐는 제출 이후 정리
//     // iree_task_submission_deinitialize(&submission);
//     iree_notification_deinitialize(&completion_notification);
//   }

//   // 5) 레코딩 당시 inline으로 못 돌린 deferred CB 적용
//   IREE_RETURN_IF_ERROR(iree_hal_accel_device_apply_deferred_command_buffer(
//       device, command_buffer, binding_table));

//   // 6) 완료 signal
//   IREE_RETURN_IF_ERROR(iree_hal_accel_semaphore_multi_signal(
//       &device->semaphore_state, signal_semaphore_list));

//   return status;
// }

static iree_status_t iree_hal_accel_device_queue_flush(
    iree_hal_device_t* base_device, iree_hal_queue_affinity_t queue_affinity) {
  // Currently unused; we flush as submissions are made.
  return iree_ok_status();
}

static iree_status_t iree_hal_accel_device_wait_semaphores(
    iree_hal_device_t* base_device, iree_hal_wait_mode_t wait_mode,
    const iree_hal_semaphore_list_t semaphore_list, iree_timeout_t timeout) {
  iree_hal_accel_device_t* device = iree_hal_accel_device_cast(base_device);
  return iree_hal_accel_semaphore_multi_wait(&device->semaphore_state, wait_mode,
                                            semaphore_list, timeout);
}

static iree_status_t iree_hal_accel_device_profiling_begin(
    iree_hal_device_t* base_device,
    const iree_hal_device_profiling_options_t* options) {
  // Unimplemented (and that's ok).
  // We could hook in to vendor APIs (Intel/ARM/etc) or generic perf infra:
  // https://man7.org/linux/man-pages/man2/perf_event_open.2.html
  // Capturing things like:
  //   PERF_COUNT_HW_CPU_CYCLES / PERF_COUNT_HW_INSTRUCTIONS
  //   PERF_COUNT_HW_CACHE_REFERENCES / PERF_COUNT_HW_CACHE_MISSES
  //   etc
  // TODO(benvanik): shared iree/hal/local/profiling implementation of this.
  return iree_ok_status();
}

static iree_status_t iree_hal_accel_device_profiling_flush(
    iree_hal_device_t* base_device) {
  // Unimplemented (and that's ok).
  return iree_ok_status();
}

static iree_status_t iree_hal_accel_device_profiling_end(
    iree_hal_device_t* base_device) {
  // Unimplemented (and that's ok).
  return iree_ok_status();
}

static const iree_hal_device_vtable_t iree_hal_accel_device_vtable = {
    .destroy = iree_hal_accel_device_destroy,
    .id = iree_hal_accel_device_id,
    .host_allocator = iree_hal_accel_device_host_allocator,
    .device_allocator = iree_hal_accel_device_allocator,
    .replace_device_allocator = iree_hal_accel_replace_device_allocator,
    .replace_channel_provider = iree_hal_accel_replace_channel_provider,
    .trim = iree_hal_accel_device_trim,
    .query_i64 = iree_hal_accel_device_query_i64,
    .create_channel = iree_hal_accel_device_create_channel,
    .create_command_buffer = iree_hal_accel_device_create_command_buffer,
    .create_event = iree_hal_accel_device_create_event,
    .create_executable_cache = iree_hal_accel_device_create_executable_cache,
    .import_file = iree_hal_accel_device_import_file,
    .create_semaphore = iree_hal_accel_device_create_semaphore,
    .query_semaphore_compatibility =
        iree_hal_accel_device_query_semaphore_compatibility,
    .queue_alloca = iree_hal_accel_device_queue_alloca,
    .queue_dealloca = iree_hal_accel_device_queue_dealloca,
    .queue_fill = iree_hal_device_queue_emulated_fill,
    .queue_update = iree_hal_device_queue_emulated_update,
    .queue_copy = iree_hal_device_queue_emulated_copy,
    .queue_read = iree_hal_accel_device_queue_read,
    .queue_write = iree_hal_accel_device_queue_write,
    .queue_dispatch = iree_hal_device_queue_emulated_dispatch,
    .queue_execute = iree_hal_accel_device_queue_execute,
    .queue_flush = iree_hal_accel_device_queue_flush,
    .wait_semaphores = iree_hal_accel_device_wait_semaphores,
    .profiling_begin = iree_hal_accel_device_profiling_begin,
    .profiling_flush = iree_hal_accel_device_profiling_flush,
    .profiling_end = iree_hal_accel_device_profiling_end,
};
