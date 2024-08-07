/*
  Copyright (c), 2001-2024, Shenshu Tech. Co., Ltd.
 */

#ifndef SVP_ACL_EXT_H
#define SVP_ACL_EXT_H

#include "svp_acl_base.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief After waiting for a specified time, trigger aicpu async processing
 *
 * @par Function
 *  The thread processing aicpu async specified by the aicpu task
 *
 * @param timeout [IN]   timeout value
 *
 * @retval SVP_ACL_SUCCESS The function is successfully executed.
 * @retval OtherValues Failure
 */
SVP_ACL_FUNC_VISIBILITY svp_acl_error svp_acl_ext_process_aicpu_task(int32_t timeout);

/**
 * @brief Get the num of AICPU tasks in a model
 *
 * @param model_id [IN]   model id
 * @param num [OUT]   aicpu task num
 *
 * @retval SVP_ACL_SUCCESS The function is successfully executed.
 * @retval OtherValues Failure
 */
SVP_ACL_FUNC_VISIBILITY svp_acl_error svp_acl_ext_get_mdl_aicpu_task_num(uint32_t model_id, uint32_t *num);

/**
 * @brief Get the type of net in a model
 *
 * @param model_id [IN]   model id
 * @param net_type [OUT]  net type in the om
 *
 * @retval SVP_ACL_SUCCESS The function is successfully executed.
 * @retval OtherValues Failure
 */
SVP_ACL_FUNC_VISIBILITY svp_acl_error svp_acl_ext_get_mdl_net_type(uint32_t model_id, uint32_t *net_type);

#ifdef __cplusplus
}
#endif

#endif /* SVP_ACL_EXT_H */
