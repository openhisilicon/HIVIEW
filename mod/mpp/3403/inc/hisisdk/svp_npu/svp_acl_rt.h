/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */

#ifndef SVP_ACL_RT_H
#define SVP_ACL_RT_H

#include <stdint.h>
#include <stddef.h>
#include "svp_acl_base.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum svp_acl_rt_run_mode {
    SVP_ACL_DEVICE,
    SVP_ACL_HOST,
} svp_acl_rt_run_mode;

typedef enum svp_acl_rt_callback_block_type {
    SVP_ACL_CALLBACK_NO_BLOCK,
    SVP_ACL_CALLBACK_BLOCK,
} svp_acl_rt_callback_block_type;

typedef enum svp_acl_rt_mem_malloc_policy {
    SVP_ACL_MEM_MALLOC_HUGE_FIRST,
    SVP_ACL_MEM_MALLOC_HUGE_ONLY,
    SVP_ACL_MEM_MALLOC_NORMAL_ONLY,
} svp_acl_rt_mem_malloc_policy;

typedef void (*svp_acl_rt_callback)(void *user_data);

/**
 * @ingroup svp_acl
 * @brief The thread that handles the callback function on the Stream
 *
 * @param thread_id [IN]   thread ID
 * @param stream [IN]   stream handle
 * @retval SVP_ACL_SUCCESS The function is successfully executed.
 * @retval OtherValues Failure
 */
SVP_ACL_FUNC_VISIBILITY svp_acl_error svp_acl_rt_subscribe_report(uint64_t thread_id, svp_acl_rt_stream stream);

/**
 * @ingroup svp_acl
 * @brief Add a callback function to be executed on the host
 *        to the task queue of the Stream
 *
 * @param fn [IN]   Specify the callback function to be added
 *                  The function prototype of the callback function is:
 *                  typedef void (*svp_acl_rt_callback)(void *user_data);
 * @param user_data [IN]   User data to be passed to the callback function
 * @param block_type [IN]   callback block type
 * @param stream [IN]   stream handle
 * @retval SVP_ACL_SUCCESS The function is successfully executed.
 * @retval OtherValues Failure
 */
SVP_ACL_FUNC_VISIBILITY svp_acl_error svp_acl_rt_launch_callback(svp_acl_rt_callback fn, void *user_data,
    svp_acl_rt_callback_block_type block_type, svp_acl_rt_stream stream);

/**
 * @ingroup svp_acl
 * @brief After waiting for a specified time, trigger callback processing
 *
 * @par Function
 *  The thread processing callback specified by
 *  the svp_acl_rt_subscribe_report interface
 * @param timeout [IN]   timeout value
 * @retval SVP_ACL_SUCCESS The function is successfully executed.
 * @retval OtherValues Failure
 *
 * @see svp_acl_rt_subscribe_report
 */
SVP_ACL_FUNC_VISIBILITY svp_acl_error svp_acl_rt_process_report(int32_t timeout);

/**
 * @ingroup svp_acl
 * @brief Cancel thread registration,
 *        the callback function on the specified Stream
 *        is no longer processed by the specified thread
 *
 * @param thread_id [IN]   thread ID
 * @param stream [IN]   stream handle
 * @retval SVP_ACL_SUCCESS The function is successfully executed.
 * @retval OtherValues Failure
 */
SVP_ACL_FUNC_VISIBILITY svp_acl_error svp_acl_rt_unsubscribe_report(uint64_t thread_id, svp_acl_rt_stream stream);

/**
 * @ingroup svp_acl
 * @brief create context and associates it with the calling thread
 *
 * @par Function
 * The following use cases are supported:
 * @li If you don't call the svp_acl_rt_create_context interface
 * to explicitly create the context,
 * the system will use the default context, which is implicitly created
 * when the svp_acl_rt_set_device interface is called.
 * @li If multiple contexts are created in a process
 * (there is no limit on the number of contexts),
 * the current thread can only use one of them at the same time.
 * It is recommended to explicitly specify the context of the current thread
 * through the svp_acl_rt_set_current_context interface to increase.
 * the maintainability of the program.
 * @param  context [OUT]   point to the created context
 * @param  device_id [IN]    device to create context on
 * @retval SVP_ACL_SUCCESS The function is successfully executed.
 * @retval OtherValues Failure
 *
 * @see svp_acl_rt_set_device | svp_acl_rt_set_current_context
 */
SVP_ACL_FUNC_VISIBILITY svp_acl_error svp_acl_rt_create_context(svp_acl_rt_context *context, int32_t device_id);

/**
 * @ingroup svp_acl
 * @brief destroy context instance
 *
 * @par Function
 * Can only destroy context created through svp_acl_rt_create_context interface
 * @param  context [IN]   the context to destroy
 * @retval SVP_ACL_SUCCESS The function is successfully executed.
 * @retval OtherValues Failure
 *
 * @see svp_acl_rt_create_context
 */
SVP_ACL_FUNC_VISIBILITY svp_acl_error svp_acl_rt_destroy_context(svp_acl_rt_context context);

/**
 * @ingroup svp_acl
 * @brief set the context of the thread
 *
 * @par Function
 * The following scenarios are supported:
 * @li If the svp_acl_rt_create_context interface is called in a thread to explicitly
 * create a Context (for example: ctx1), the thread's Context can be specified
 * without calling the svp_acl_rt_set_current_context interface.
 * The system uses ctx1 as the context of thread1 by default.
 * @li If the svp_acl_rt_create_context interface is not explicitly created,
 * the system uses the default context as the context of the thread.
 * At this time, the svp_acl_rt_destroy_context interface cannot be used to release
 * the default context.
 * @li If the svp_acl_rt_set_current_context interface is called multiple times to
 * set the thread's Context, the last one prevails.
 *
 * @par Restriction
 * @li If the device corresponding to the context set for the thread
 * has been reset, you cannot set the context as the context of the thread,
 * otherwise a business exception will result.
 * @li It is recommended to use the context created in a thread.
 * If the svp_acl_rt_create_context interface is called in thread A to create a context,
 * and the context is used in thread B,
 * the user must guarantee the execution order of tasks in the same stream
 * under the same context in two threads.
 * @param  context [IN]   the current context of the thread
 * @retval SVP_ACL_SUCCESS The function is successfully executed.
 * @retval OtherValues Failure
 *
 * @see svp_acl_rt_create_context | svp_acl_rt_destroy_context
 */
SVP_ACL_FUNC_VISIBILITY svp_acl_error svp_acl_rt_set_current_context(svp_acl_rt_context context);

/**
 * @ingroup svp_acl
 * @brief get the context of the thread
 *
 * @par Function
 * If the user calls the svp_acl_rt_set_current_context interface
 * multiple times to set the context of the current thread,
 * then the last set context is obtained
 * @param  context [OUT]   the current context of the thread
 * @retval SVP_ACL_SUCCESS The function is successfully executed.
 * @retval OtherValues Failure
 *
 * @see svp_acl_rt_set_current_context
 */
SVP_ACL_FUNC_VISIBILITY svp_acl_error svp_acl_rt_get_current_context(svp_acl_rt_context *context);

/**
 * @ingroup svp_acl
 * @brief Specify the device to use for the operation
 * implicitly create the default context and the default stream
 *
 * @par Function
 * The following use cases are supported:
 * @li Device can be specified in the process or thread.
 * If you call the svp_acl_rt_set_device interface multiple
 * times to specify the same device,
 * you only need to call the svp_acl_rt_reset_device interface to reset the device.
 * @li The same device can be specified for operation
 *  in different processes or threads.
 * @li Device is specified in a process,
 * and multiple threads in the process can share this device to explicitly
 * create a Context (svp_acl_rt_create_context interface).
 * @li In multi-device scenarios, you can switch to other devices
 * through the svp_acl_rt_set_device interface in the process.
 * @param  device_id [IN]  the device id
 * @retval SVP_ACL_SUCCESS The function is successfully executed.
 * @retval OtherValues Failure
 *
 * @see svp_acl_rt_reset_device |svp_acl_rt_create_context
 */
SVP_ACL_FUNC_VISIBILITY svp_acl_error svp_acl_rt_set_device(int32_t device_id);

/**
 * @ingroup svp_acl
 * @brief Reset the current operating Device and free resources on the device,
 * including the default context, the default stream,
 * and all streams created under the default context,
 * and synchronizes the interface.
 * If the task under the default context or stream has not been completed,
 * the system will stop processing the task and release it.
 *
 * @par Restriction
 * @li The Context, Stream that are explicitly created
 * on the device to be reset. Before resetting,
 * it is recommended to follow the following interface calling sequence,
 * otherwise business abnormalities may be caused.
 * @li Interface calling sequence:
 * call svp_acl_rt_destroy_stream interface to release explicitly created Stream->
 * call svp_acl_rt_destroy_context to release explicitly created Context->
 * call svp_acl_rt_reset_device interface
 * @param  device_id [IN]   the device id
 * @retval SVP_ACL_SUCCESS The function is successfully executed.
 * @retval OtherValues Failure
 */
SVP_ACL_FUNC_VISIBILITY svp_acl_error svp_acl_rt_reset_device(int32_t device_id);

/**
 * @ingroup svp_acl
 * @brief get target device of current thread
 *
 * @param device_id [OUT]  the device id
 * @retval SVP_ACL_SUCCESS The function is successfully executed.
 * @retval OtherValues Failure
 */
SVP_ACL_FUNC_VISIBILITY svp_acl_error svp_acl_rt_get_device(int32_t *device_id);

/**
 * @ingroup svp_acl
 * @brief get target side
 *
 * @param run_mode [OUT]    the run mode
 * @retval SVP_ACL_SUCCESS The function is successfully executed.
 * @retval OtherValues Failure
 */
SVP_ACL_FUNC_VISIBILITY svp_acl_error svp_acl_rt_get_run_mode(svp_acl_rt_run_mode *run_mode);

/**
 * @ingroup svp_acl
 * @brief Wait for compute device to finish
 *
 * @retval SVP_ACL_SUCCESS The function is successfully executed.
 * @retval OtherValues Failure
 */
SVP_ACL_FUNC_VISIBILITY svp_acl_error svp_acl_rt_synchronize_device(void);

/**
 * @ingroup svp_acl
 * @brief get total device number.
 *
 * @param count [IN|OUT]    the device number
 * @retval SVP_ACL_SUCCESS The function is successfully executed.
 * @retval OtherValues Failure
 */
SVP_ACL_FUNC_VISIBILITY svp_acl_error svp_acl_rt_get_device_count(uint32_t *count);

/**
 * @ingroup svp_acl
 * @brief alloc memory on device
 *
 * @par Function
 *  alloc for size linear memory on device
 *  and return a pointer to allocated memory by *dev_ptr
 *
 * @par Restriction
 * @li The memory requested by the svp_acl_rt_malloc interface needs to be released
 * through the svp_acl_rt_free interface.
 * @param dev_ptr [IN|OUT]  pointer to pointer to allocated memory on device
 * @param size [IN]   alloc memory size
 * @param policy [IN]   memory alloc policy
 * @retval SVP_ACL_SUCCESS The function is successfully executed.
 * @retval OtherValues Failure
 *
 * @see svp_acl_rt_free
 */
SVP_ACL_FUNC_VISIBILITY svp_acl_error svp_acl_rt_malloc(void **dev_ptr, size_t size,
    svp_acl_rt_mem_malloc_policy policy);

/**
 * @ingroup svp_acl
 * @brief allocate memory on device with cache
 *
 * @par Function
 *  alloc for size linear memory on device
 *  and return a pointer to allocated memory by *dev_ptr
 *
 * @par Restriction
 * @li The memory requested by the svp_acl_rt_malloc_cached interface needs to be released
 * through the svp_acl_rt_free interface.
 *
 * @param dev_ptr [IN|OUT]  pointer to pointer to allocated memory on device
 * @param size [IN]   alloc memory size
 * @param policy [IN]   memory alloc policy
 *
 * @retval SVP_ACL_SUCCESS The function is successfully executed.
 * @retval OtherValues Failure
 *
 * @see svp_acl_rt_free | svp_acl_rt_malloc
 */
SVP_ACL_FUNC_VISIBILITY svp_acl_error svp_acl_rt_malloc_cached(void **dev_ptr, size_t size,
    svp_acl_rt_mem_malloc_policy policy);

/**
 * @ingroup svp_acl
 * @brief flush cache data to ddr
 *
 * @param dev_ptr [IN]  the pointer that flush data to ddr
 * @param size [IN]   flush size
 *
 * @retval SVP_ACL_SUCCESS The function is successfully executed.
 * @retval OtherValues Failure
 */
SVP_ACL_FUNC_VISIBILITY svp_acl_error svp_acl_rt_mem_flush(const void *dev_ptr, size_t size);

/**
 * @ingroup svp_acl
 * @brief invalidate cache data
 *
 * @param dev_ptr [IN]  pointer to invalidate cache data
 * @param size [IN]    invalidate size
 *
 * @retval SVP_ACL_SUCCESS The function is successfully executed.
 * @retval OtherValues Failure
 */
SVP_ACL_FUNC_VISIBILITY svp_acl_error svp_acl_rt_mem_invalidate(const void *dev_ptr, size_t size);

/**
 * @ingroup svp_acl
 * @brief free device memory
 *
 * @par Function
 *  can only free memory allocated through the svp_acl_rt_malloc interface
 * @param  dev_ptr [IN]  Pointer to memory to be freed
 * @retval SVP_ACL_SUCCESS The function is successfully executed.
 * @retval OtherValues Failure
 *
 * @see svp_acl_rt_malloc
 */
SVP_ACL_FUNC_VISIBILITY svp_acl_error svp_acl_rt_free(const void *dev_ptr);

/**
 * @ingroup svp_acl
 * @brief alloc memory on host
 *
 * @par Restriction
 * @li The requested memory cannot be used in the Device
 * and needs to be explicitly copied to the Device.
 * @li The memory requested by the svp_acl_rt_malloc_host interface
 * needs to be released through the svp_acl_rt_free_host interface.
 * @param  host_ptr [IN|OUT] pointer to pointer to allocated memory on the host
 * @param  size [IN]  alloc memory size
 * @retval SVP_ACL_SUCCESS The function is successfully executed.
 * @retval OtherValues Failure
 *
 * @see svp_acl_rt_free_host
 */
SVP_ACL_FUNC_VISIBILITY svp_acl_error svp_acl_rt_malloc_host(void **host_ptr, size_t size);

/**
 * @ingroup svp_acl
 * @brief free host memory
 *
 * @par Function
 *  can only free memory allocated through the svp_acl_rt_malloc_host interface
 * @param  host_ptr [IN]   free memory pointer
 * @retval SVP_ACL_SUCCESS The function is successfully executed.
 * @retval OtherValues Failure
 *
 * @see svp_acl_rt_malloc_host
 */
SVP_ACL_FUNC_VISIBILITY svp_acl_error svp_acl_rt_free_host(const void *host_ptr);

/**
 * @ingroup svp_acl
 * @brief  create stream instance
 *
 * @param  stream [OUT]   the created stream
 * @retval SVP_ACL_SUCCESS The function is successfully executed.
 * @retval OtherValues Failure
 */
SVP_ACL_FUNC_VISIBILITY svp_acl_error svp_acl_rt_create_stream(svp_acl_rt_stream *stream);

/**
 * @ingroup svp_acl
 * @brief destroy stream instance
 *
 * @par Function
 * Can only destroy streams created through the svp_acl_rt_create_stream interface
 *
 * @par Restriction
 * Before calling the svp_acl_rt_destroy_stream interface to destroy
 * the specified Stream, you need to call the svp_acl_rt_synchronize_stream interface
 * to ensure that the tasks in the Stream have been completed.
 * @param stream [IN]  the stream to destroy
 * @retval SVP_ACL_SUCCESS The function is successfully executed.
 * @retval OtherValues Failure
 *
 * @see svp_acl_rt_create_stream | svp_acl_rt_synchronize_stream
 */
SVP_ACL_FUNC_VISIBILITY svp_acl_error svp_acl_rt_destroy_stream(svp_acl_rt_stream stream);

/**
 * @ingroup svp_acl
 * @brief block the host until all tasks
 * in the specified stream have completed
 *
 * @param  stream [IN]   the stream to wait
 * @retval SVP_ACL_SUCCESS The function is successfully executed.
 * @retval OtherValues Failure
 */
SVP_ACL_FUNC_VISIBILITY svp_acl_error svp_acl_rt_synchronize_stream(svp_acl_rt_stream stream);

/**
 * @ingroup
 * @brief Set the timeout interval for waitting of op
 *
 * @param timeout [IN]   op wait timeout
 *
 * @retval SVP_ACL_SUCCESS The function is successfully executed.
 * @retval OtherValues Failure
 */
SVP_ACL_FUNC_VISIBILITY svp_acl_error svp_acl_rt_set_op_wait_timeout(uint32_t timeout);

#ifdef __cplusplus
}
#endif

#endif /* SVP_ACL_RT_H */

