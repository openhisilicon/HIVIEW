/**
 * @file      hi_confaccess.h
 * @brief     The external header of libconfaccess, support manipulation to ini config file
 *
 * Copyright (c) 2017 Huawei Tech.Co.,Ltd
 *
 * @author    HiMobileCam Reference Develop Team
 * @date      2017/12/15
 * @version   1.0

 */

#ifndef __HI_CONFACCESS_H__
#define __HI_CONFACCESS_H__

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include "hi_scenecomm.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

/** \addtogroup     CONFACCESS */
/** @{ */  /** <!-- [CONFACCESS] */

/** Length Define */
#define HI_CONFACCESS_PATH_MAX_LEN     (256)
#define HI_CONFACCESS_NAME_MAX_LEN     (32 + 1)
#define HI_CONFACCESS_KEY_MAX_LEN      (128)

/** Error Define */
#define HI_CONFACCESS_EINVAL          HI_SCENECOMM_ERR_ID(HI_SCENE_MOD_CONFACCESS, 0)/**<Invalid argument */
#define HI_CONFACCESS_ENOTINIT        HI_SCENECOMM_ERR_ID(HI_SCENE_MOD_CONFACCESS, 1)/**<Not inited */
#define HI_CONFACCESS_EREINIT         HI_SCENECOMM_ERR_ID(HI_SCENE_MOD_CONFACCESS, 2)/**<re init */
#define HI_CONFACCESS_EMALLOC         HI_SCENECOMM_ERR_ID(HI_SCENE_MOD_CONFACCESS, 3)/**<malloc failed */
#define HI_CONFACCESS_ECFG_NOTEXIST   HI_SCENECOMM_ERR_ID(HI_SCENE_MOD_CONFACCESS, 4)/**<cfg not exist */
#define HI_CONFACCESS_EMOD_NOTEXIST   HI_SCENECOMM_ERR_ID(HI_SCENE_MOD_CONFACCESS, 5)/**<module not exist */
#define HI_CONFACCESS_EITEM_NOTEXIST  HI_SCENECOMM_ERR_ID(HI_SCENE_MOD_CONFACCESS, 6)/**<confitem not exist */


/**
 * @brief     Load a config, includes of the common config and all the submode config
 * @param[in] pszCfgName : The config name to be load
 * @param[in] pszCfgPath : The path of config
 * @param[out] pu32ModuleNum : Module Number
 * @return    0 success,non-zero error code.
 * @exception None
 * @author    HiMobileCam Reference Develop Team
 * @date      2017/12/15
 */
HI_S32 HI_CONFACCESS_Init(const HI_CHAR *pszCfgName, const HI_CHAR  *pszCfgPath, HI_U32 *pu32ModuleNum);

/**
 * @brief     Deinit all source of a config
 * @param[in] pszCfgName : The config name
 * @return    0 success,non-zero error code.
 * @exception None
 * @author    HiMobileCam Reference Develop Team
 * @date      2017/12/15
 */
HI_S32 HI_CONFACCESS_Deinit(const HI_CHAR *pszCfgName);

/**
 * @brief     get string conf item of certain module, if not found the default willbe given
 * @param[in] pszCfgName : The config name
 * @param[in] pszModule : The module name
 * @param[in] pszConfItem : The confitem name
 * @param[in] pszDefault : default value if item not found
 * @param[out]ppszValue : value
 * @return    0 success,non-zero error code.
 * @exception None
 * @author    HiMobileCam Reference Develop Team
 * @date      2017/12/15
 */
HI_S32 HI_CONFACCESS_GetString(const HI_CHAR *pszCfgName, const HI_CHAR *pszModule,
                               const HI_CHAR *pszConfItem, HI_CHAR *pszDefault, HI_CHAR **const ppszValue);

/**
 * @brief     get string conf item of certain module
 * @param[in] pszCfgName : The config name
 * @param[in] pszModule : The module name
 * @param[in] pszConfItem : The confitem name
 * @param[out]ppszValue : value
 * @return    0 success,non-zero error code.
 * @exception None
 * @author    HiMobileCam Reference Develop Team
 * @date      2017/12/15
 */
HI_S32 HI_CONFACCESS_GetStr(const HI_CHAR *pszCfgName, const HI_CHAR *pszModule,
                            const HI_CHAR  *pszConfItem, HI_CHAR **ppszValue);

/**
 * @brief     get int conf item of certain module
 * @param[in] pszCfgName : The config name
 * @param[in] pszModule : The module name
 * @param[in] pszConfItem : The confitem name
 * @param[in] s32Notfound : default value if item not found
 * @param[out]ps32Value : value
 * @return    0 success,non-zero error code.
 * @exception None
 * @author    HiMobileCam Reference Develop Team
 * @date      2017/12/15
 */
HI_S32 HI_CONFACCESS_GetInt(const HI_CHAR *pszCfgName, const HI_CHAR *pszModule,
                            const HI_CHAR *pszConfItem, HI_S32 s32Notfound, HI_S32 *const ps32Value);

/**
 * @brief     get int conf item of certain module
 * @param[in] pszCfgName : The config name
 * @param[in] pszModule : The module name
 * @param[in] pszConfItem : The confitem name
 * @param[in] s32Notfound : default value if item not found
 * @param[out]ps64Value : value
 * @return    0 success,non-zero error code.
 * @exception None
 * @author    HiMobileCam Reference Develop Team
 * @date      2017/12/15
 */
HI_S32 HI_CONFACCESS_GetLonglong(const HI_CHAR *pszCfgName, const HI_CHAR *pszModule,
                                 const HI_CHAR  *pszConfItem, HI_S32 s32Notfound, HI_S64 *const ps64Value);


/**
 * @brief     get double conf item of certain module
 * @param[in] pszCfgName : The config name
 * @param[in] pszModule : The module name
 * @param[in] pszConfItem : The confitem name
 * @param[in] dNotfound : default value if item not found
 * @param[out]pdValue : value
 * @return    0 success,non-zero error code.
 * @exception None
 * @author    HiMobileCam Reference Develop Team
 * @date      2017/12/15
 */
HI_S32 HI_CONFACCESS_GetDouble(const HI_CHAR *pszCfgName, const HI_CHAR *pszModule,
                               const HI_CHAR *pszConfItem, HI_DOUBLE dNotfound, HI_DOUBLE *const pdValue);

/**
 * @brief     get bool conf item of certain module
 * @param[in] pszCfgName : The config name
 * @param[in] pszModule : The module name
 * @param[in] pszConfItem : The confitem name
 * @param[in] bNotfound : default value if item not found
 * @param[out]pbValue : value
 * @return    0 success,non-zero error code.
 * @exception None
 * @author    HiMobileCam Reference Develop Team
 * @date      2017/12/15
 */
HI_S32 HI_CONFACCESS_GetBool(const HI_CHAR *pszCfgName, const HI_CHAR *pszModule,
                             const HI_CHAR *pszConfItem, HI_BOOL bNotfound, HI_BOOL *pbValue);

/**
 * @brief     set string conf item of certain module
 * @param[in] pszCfgName : The config name
 * @param[in] pszModule : The module name
 * @param[in] pszConfItem : The confitem name
 * @param[in] pszValue : value
 * @return    0 success,non-zero error code.
 * @exception None
 * @author    HiMobileCam Reference Develop Team
 * @date      2017/12/15
 */
HI_S32 HI_CONFACCESS_SetString(const HI_CHAR *pszCfgName, const HI_CHAR *pszModule,
                               const HI_CHAR *pszConfItem, const HI_CHAR *pszValue);

/** @}*/  /** <!-- ==== CONFACCESS End ====*/

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __HI_CONFACCESS_H__ */

