/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */

#ifndef SVP_ACL_H
#define SVP_ACL_H

#include "svp_acl_rt.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @ingroup svp_acl
 * @brief acl initialize
 *
 * @par Restriction
 * The svp_acl_init interface can be called only once in a process
 * @param config_path [IN] the config path, it can be NULL
 * @retval SVP_ACL_SUCCESS The function is successfully executed.
 * @retval OtherValues Failure
 */
SVP_ACL_FUNC_VISIBILITY svp_acl_error svp_acl_init(const char *config_path);

/**
 * @ingroup svp_acl
 * @brief acl finalize
 *
 * @par Restriction
 * Need to call svp_acl_finalize before the process exits.
 * After calling svp_acl_finalize, the services cannot continue to be used normally.
 * @retval SVP_ACL_SUCCESS The function is successfully executed.
 * @retval OtherValues Failure
 */
SVP_ACL_FUNC_VISIBILITY svp_acl_error svp_acl_finalize();

#ifdef __cplusplus
}
#endif

#endif /* SVP_ACL_H */
