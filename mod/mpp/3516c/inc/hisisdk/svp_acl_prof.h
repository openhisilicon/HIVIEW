/*
  Copyright (c), 2001-2024, Shenshu Tech. Co., Ltd.
 */

#ifndef SVP_ACL_PROF_H
#define SVP_ACL_PROF_H

#include "svp_acl_base.h"

#ifdef __cplusplus
extern "C" {
#endif

#define SVP_ACL_PROF_ACL_API                0x0001
#define SVP_ACL_PROF_TASK_TIME              0x0002
#define SVP_ACL_PROF_AICORE_METRICS         0x0004
#define SVP_ACL_PROF_AICPU_TRACE            0x0008

#define SVP_ACL_PROF_MAX_OP_NAME_LEN        257
#define SVP_ACL_PROF_MAX_OP_TYPE_LEN        65

typedef enum {
    SVP_ACL_AICORE_ARITHMETIC_UTILIZATION = 0,
    SVP_ACL_AICORE_NONE = 0xFF
} svp_acl_prof_aicore_metrics;

typedef struct svp_acl_prof_config svp_acl_prof_config;
typedef struct svp_acl_prof_aicore_events svp_acl_prof_aicore_events;
typedef struct svp_acl_prof_subscribe_config svp_acl_prof_subscribe_config;

/**
 * @ingroup svp_acl
 * @brief profiling initialize
 *
 * @param  result_path [IN]  path of profiling result
 * @param  length [IN]              length of profilerResultPath
 *
 * @retval SVP_ACL_SUCCESS The function is successfully executed.
 * @retval Other Values Failure
 *
 * @see svp_acl_prof_finalize
 */
SVP_ACL_FUNC_VISIBILITY svp_acl_error svp_acl_prof_init(const char *result_path, size_t length);

/**
 * @ingroup svp_acl
 * @brief profiling finalize
 *
 * @retval SVP_ACL_SUCCESS The function is successfully executed.
 * @retval Other Values Failure
 *
 * @see svp_acl_prof_init
 */
SVP_ACL_FUNC_VISIBILITY svp_acl_error svp_acl_prof_finalize(void);

/**
 * @ingroup svp_acl
 * @brief Start profiling modules by profiler_config
 *
 * @param  profiler_config [IN]  config of profiling
 *
 * @retval SVP_ACL_SUCCESS The function is successfully executed.
 * @retval OtherValues Failure
 *
 * @see svp_acl_prof_stop
 */
SVP_ACL_FUNC_VISIBILITY svp_acl_error svp_acl_prof_start(const svp_acl_prof_config *profiler_config);

/**
 * @ingroup svp_acl
 * @brief Create data of type svp_acl_prof_config
 *
 * @param  device_id_list [IN]      list of device id
 * @param  device_nums [IN]        number of devices
 * @param  aicore_metrics [IN]     type of aicore metrics
 * @param  aicore_events [IN]      pointer to aicore events, only support NULL now
 * @param  data_type_config [IN]    config modules need profiling
 *
 * @retval the svp_acl_prof_config pointer
 *
 * @see svp_acl_prof_destroy_config
 */
SVP_ACL_FUNC_VISIBILITY svp_acl_prof_config *svp_acl_prof_create_config(const uint32_t *device_id_list,
    uint32_t device_nums, svp_acl_prof_aicore_metrics aicore_metrics, const svp_acl_prof_aicore_events *aicore_events,
    uint64_t data_type_config);

/**
 * @ingroup svp_acl
 * @brief Destroy data of type svp_acl_prof_config
 *
 * @param  profiler_config [IN]  config of profiling
 *
 * @retval SVP_ACL_SUCCESS The function is successfully executed.
 * @retval OtherValues Failure
 *
 * @see svp_acl_prof_create_config
 */
SVP_ACL_FUNC_VISIBILITY svp_acl_error svp_acl_prof_destroy_config(const svp_acl_prof_config *profiler_config);

/**
 * @ingroup svp_acl
 * @brief stop profiling modules by stopProfilingConfig
 *
 * @param  profiler_config [IN]  pointer to stop config of profiling
 *
 * @retval SVP_ACL_SUCCESS The function is successfully executed.
 * @retval OtherValues Failure
 *
 * @see svp_acl_prof_start
 */
SVP_ACL_FUNC_VISIBILITY svp_acl_error svp_acl_prof_stop(const svp_acl_prof_config *profiler_config);

/**
 * @ingroup svp_acl
 * @brief subscribe profiling data of model
 *
 * @param  model_id [IN]          the model id subscribed
 * @param  subscribe_config [IN]  pointer to config of model subscribe
 *
 * @retval SVP_ACL_SUCCESS The function is successfully executed.
 * @retval OtherValues Failure
 *
 * @see svp_acl_prof_model_unsubscribe
 */
SVP_ACL_FUNC_VISIBILITY svp_acl_error svp_acl_prof_model_subscribe(uint32_t model_id,
    const svp_acl_prof_subscribe_config *subscribe_config);

/**
 * @ingroup svp_acl
 * @brief unsubscribe profiling data of model
 *
 * @param  model_id [IN]  the model id unsubscribed
 *
 * @retval SVP_ACL_SUCCESS The function is successfully executed.
 * @retval OtherValues Failure
 *
 * @see svp_acl_prof_model_subscribe
 */
SVP_ACL_FUNC_VISIBILITY svp_acl_error svp_acl_prof_model_unsubscribe(uint32_t model_id);

/**
 * @ingroup svp_acl
 * @brief create subscribe config
 *
 * @param  time_info_switch [IN] switch whether get time info from model
 * @param  aicore_metrics [IN]  aicore metrics
 * @param  fd [IN]             pointer to write pipe
 *
 * @retval the svp_acl_prof_subscribe_config pointer
 *
 * @see svp_acl_prof_destroy_subscribe_config
 */
SVP_ACL_FUNC_VISIBILITY svp_acl_prof_subscribe_config *svp_acl_prof_create_subscribe_config(
    int8_t time_info_switch, svp_acl_prof_aicore_metrics aicore_metrics, const void *fd);

/**
 * @ingroup svp_acl
 * @brief destroy subscribe config
 *
 * @param  subscribe_config [IN]  subscribe config
 *
 * @retval SVP_ACL_SUCCESS The function is successfully executed.
 * @retval OtherValues Failure
 *
 * @see svp_acl_prof_create_subscribe_config
 */
SVP_ACL_FUNC_VISIBILITY svp_acl_error svp_acl_prof_destroy_subscribe_config(
    const svp_acl_prof_subscribe_config *subscribe_config);

/**
 * @ingroup svp_acl
 * @brief create subscribe config
 *
 * @param  op_desc_size [OUT]  size of op desc
 *
 * @retval SVP_ACL_SUCCESS The function is successfully executed.
 * @retval OtherValues Failure
 */
SVP_ACL_FUNC_VISIBILITY svp_acl_error svp_acl_prof_get_op_desc_size(size_t *op_desc_size);

/**
 * @ingroup svp_acl
 * @brief get op number from subscription data
 *
 * @param  op_info [IN]     pointer to subscription data
 * @param  op_info_len [IN]  memory size of subscription data
 * @param  op_number [OUT]  op number of subscription data
 *
 * @retval SVP_ACL_SUCCESS The function is successfully executed.
 * @retval OtherValues Failure
 */
SVP_ACL_FUNC_VISIBILITY svp_acl_error svp_acl_prof_get_op_num(const void *op_info,
    size_t op_info_len, uint32_t *op_number);

/**
 * @ingroup svp_acl
 * @brief get length op type from subscription data
 *
 * @param  op_info [IN]      pointer to subscription data
 * @param  op_info_len [IN]   memory size of subscription data
 * @param  index [IN]       index of op array in op_info
 * @param  op_type_len [OUT]  actual length of op type string
 *
 * @retval SVP_ACL_SUCCESS The function is successfully executed.
 * @retval OtherValues Failure
 */
SVP_ACL_FUNC_VISIBILITY svp_acl_error svp_acl_prof_get_op_type_len(const void *op_info,
    size_t op_info_len, uint32_t index, size_t *op_type_len);

/**
 * @ingroup svp_acl
 * @brief get op type from subscription data
 *
 * @param  op_info [IN]      pointer to subscription data
 * @param  op_info_len [IN]   memory size of subscription data
 * @param  index [IN]       index of op array in op_info
 * @param  op_type [OUT]     obtained op type string
 * @param  op_type_len [IN]   obtained length of op type string
 *
 * @retval SVP_ACL_SUCCESS The function is successfully executed.
 * @retval OtherValues Failure
 */
SVP_ACL_FUNC_VISIBILITY svp_acl_error svp_acl_prof_get_op_type(const void *op_info,
    size_t op_info_len, uint32_t index, char *op_type, size_t op_type_len);

/**
 * @ingroup svp_acl
 * @brief get length op name from subscription data
 *
 * @param  op_info [IN]      pointer to subscription data
 * @param  op_info_len [IN]   memory size of subscription data
 * @param  index [IN]       index of op array in op_info
 * @param  op_name_len [OUT]  actual length of op name string
 *
 * @retval SVP_ACL_SUCCESS The function is successfully executed.
 * @retval OtherValues Failure
 */
SVP_ACL_FUNC_VISIBILITY svp_acl_error svp_acl_prof_get_op_name_len(const void *op_info,
    size_t op_info_len, uint32_t index, size_t *op_name_len);

/**
 * @ingroup svp_acl
 * @brief get op type from subscription data
 *
 * @param  op_info [IN]      pointer to subscription data
 * @param  op_info_len [IN]   memory size of subscription data
 * @param  index [IN]       index of op array in op_info
 * @param  opName [OUT]     obtained op name string
 * @param  opNameLen [IN]   obtained length of op name string
 *
 * @retval SVP_ACL_SUCCESS The function is successfully executed.
 * @retval OtherValues Failure
 */
SVP_ACL_FUNC_VISIBILITY svp_acl_error svp_acl_prof_get_op_name(const void *op_info,
    size_t op_info_len, uint32_t index, char *op_name, size_t op_name_len);

/**
 * @ingroup svp_acl
 * @brief get start time of specified op from subscription data
 *
 * @param  op_info [IN]     pointer to subscription data
 * @param  op_info_len [IN]  memory size of subscription data
 * @param  index [IN]      index of op array in op_info
 *
 * @retval start time(us) of specified op with timestamp
 * @retval 0 for failed
 */
SVP_ACL_FUNC_VISIBILITY uint64_t svp_acl_prof_get_op_start(const void *op_info,
    size_t op_info_len, uint32_t index);

/**
 * @ingroup svp_acl
 * @brief get end time of specified op from subscription data
 *
 * @param  op_info [IN]     pointer to subscription data
 * @param  op_info_len [IN]  memory size of subscription data
 * @param  index [IN]      index of op array in op_info
 *
 * @retval end time(us) of specified op with timestamp
 * @retval 0 for failed
 */
SVP_ACL_FUNC_VISIBILITY uint64_t svp_acl_prof_get_op_end(const void *op_info,
    size_t op_info_len, uint32_t index);

/**
 * @ingroup svp_acl
 * @brief get excution time of specified op from subscription data
 *
 * @param  op_info [IN]     pointer to subscription data
 * @param  op_info_len [IN]  memory size of subscription data
 * @param  index [IN]      index of op array in op_info
 *
 * @retval execution time(us) of specified op with timestamp
 * @retval 0 for failed
 */
SVP_ACL_FUNC_VISIBILITY uint64_t svp_acl_prof_get_op_duration(const void *op_info,
    size_t op_info_len, uint32_t index);

/**
 * @ingroup svp_acl
 * @brief get model id from subscription data
 *
 * @param  op_info [IN]     pointer to subscription data
 * @param  op_info_len [IN]  memory size of subscription data
 *
 * @retval model id of subscription data
 * @retval 0 for failed
 */
SVP_ACL_FUNC_VISIBILITY size_t svp_acl_prof_get_model_id(const void *op_info,
    size_t op_info_len, uint32_t index);

#ifdef __cplusplus
}
#endif

#endif /* SVP_ACL_PROF_H */

