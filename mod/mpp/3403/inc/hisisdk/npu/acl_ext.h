/**
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */

#ifndef __ACL_EXT_H__
#define __ACL_EXT_H__

#include "acl_base.h"

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
 * @retval ACL_SUCCESS The function is successfully executed.
 * @retval OtherValues Failure
 */
ACL_FUNC_VISIBILITY aclError aclextProcessAicpuTask(int32_t timeout);

#ifdef __cplusplus
}
#endif

#endif // __ACL_EXT_H__
