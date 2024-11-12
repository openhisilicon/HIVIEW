/*
  Copyright (c), 2001-2024, Shenshu Tech. Co., Ltd.
 */

#ifndef OT_CONFACCESS_H
#define OT_CONFACCESS_H

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include "ot_scenecomm.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* * Length Define */
#define HI_CONFACCESS_PATH_MAX_LEN 256
#define HI_CONFACCESS_NAME_MAX_LEN (32 + 1)
#define HI_CONFACCESS_KEY_MAX_LEN 128

/* * Error Define */
#define HI_CONFACCESS_EINVAL HI_SCENECOMM_ERR_ID(HI_SCENE_MOD_CONFACCESS, 0)         /* <Invalid argument */
#define HI_CONFACCESS_ENOTINIT HI_SCENECOMM_ERR_ID(HI_SCENE_MOD_CONFACCESS, 1)       /* <Not inited */
#define HI_CONFACCESS_EREINIT HI_SCENECOMM_ERR_ID(HI_SCENE_MOD_CONFACCESS, 2)        /* <re init */
#define HI_CONFACCESS_EMALLOC HI_SCENECOMM_ERR_ID(HI_SCENE_MOD_CONFACCESS, 3)        /* <malloc failed */
#define HI_CONFACCESS_ECFG_NOTEXIST HI_SCENECOMM_ERR_ID(HI_SCENE_MOD_CONFACCESS, 4)  /* <cfg not exist */
#define HI_CONFACCESS_EMOD_NOTEXIST HI_SCENECOMM_ERR_ID(HI_SCENE_MOD_CONFACCESS, 5)  /* <module not exist */
#define HI_CONFACCESS_EITEM_NOTEXIST HI_SCENECOMM_ERR_ID(HI_SCENE_MOD_CONFACCESS, 6) /* <confitem not exist */


/* *
 * @brief     Load a config, includes of the common config and all the submode config
 * @param[in] cfg_name : The config name to be load
 * @param[in] cfg_path : The path of config
 * @param[out] module_num : Module Number
 * @return    0 success,non-zero error code.
 * @exception None
 * @author    MobileCam Reference Develop Team
 * @date      2017/12/15
 */
hi_s32 ot_confaccess_init(const hi_char *cfg_name, const hi_char *cfg_path, hi_u32 *module_num);

/* *
 * @brief     Deinit all source of a config
 * @param[in] cfg_name : The config name
 * @return    0 success,non-zero error code.
 * @exception None
 * @author    MobileCam Reference Develop Team
 * @date      2017/12/15
 */
hi_s32 ot_confaccess_deinit(const hi_char *cfg_name);

/* *
 * @brief     get string conf item of certain module, if not found the default willbe given
 * @param[in] cfg_name : The config name
 * @param[in] module : The module name
 * @param[in] conf_item : The confitem name
 * @param[in] default_val : default value if item not found
 * @param[out]value_vector : value
 * @return    0 success,non-zero error code.
 * @exception None
 * @author    MobileCam Reference Develop Team
 * @date      2017/12/15
 */
hi_s32 ot_confaccess_get_string(const hi_char *cfg_name, const hi_char *module, const hi_char *conf_item,
    const hi_char *default_val, hi_char ** const value_vector);

/* *
 * @brief     get string conf item of certain module
 * @param[in] cfg_name : The config name
 * @param[in] module : The module name
 * @param[in] conf_item : The confitem name
 * @param[out]value_vector : value
 * @return    0 success,non-zero error code.
 * @exception None
 * @author    MobileCam Reference Develop Team
 * @date      2017/12/15
 */
hi_s32 ot_confaccess_get_str(const hi_char *cfg_name, const hi_char *module, const hi_char *conf_item,
    hi_char **value_vector);

/* *
 * @brief     get int conf item of certain module
 * @param[in] cfg_name : The config name
 * @param[in] module : The module name
 * @param[in] conf_item : The confitem name
 * @param[in] default_val : default value if item not found
 * @param[out]out_value : out value
 * @return    0 success,non-zero error code.
 * @exception None
 * @author    MobileCam Reference Develop Team
 * @date      2017/12/15
 */
hi_s32 ot_confaccess_get_int(const hi_char *cfg_name, const hi_char *module, const hi_char *conf_item,
    hi_s32 default_val, hi_s32 * const out_value);

/* *
 * @brief     get int conf item of certain module
 * @param[in] cfg_name : The config name
 * @param[in] module : The module name
 * @param[in] conf_item : The confitem name
 * @param[in] default_val : default value if item not found
 * @param[out]out_value : out value
 * @return    0 success,non-zero error code.
 * @exception None
 * @author    MobileCam Reference Develop Team
 * @date      2017/12/15
 */
hi_s32 ot_confaccess_get_long_long(const hi_char *cfg_name, const hi_char *module, const hi_char *conf_item,
    hi_s32 default_val, hi_s64 * const out_value);


/* *
 * @brief     get double conf item of certain module
 * @param[in] cfg_name : The config name
 * @param[in] module : The module name
 * @param[in] conf_item : The confitem name
 * @param[in] default_val : default value if item not found
 * @param[out]out_value : out double value
 * @return    0 success,non-zero error code.
 * @exception None
 * @author    MobileCam Reference Develop Team
 * @date      2017/12/15
 */
hi_s32 ot_confaccess_get_double(const hi_char *cfg_name, const hi_char *module, const hi_char *conf_item,
    hi_double default_val, hi_double * const out_value);

/* *
 * @brief     get bool conf item of certain module
 * @param[in] cfg_name : The config name
 * @param[in] module : The module name
 * @param[in] conf_item : The confitem name
 * @param[in] default_val : default value if item not found
 * @param[out]out_value : value
 * @return    0 success,non-zero error code.
 * @exception None
 * @author    MobileCam Reference Develop Team
 * @date      2017/12/15
 */
hi_s32 ot_confaccess_get_bool(const hi_char *cfg_name, const hi_char *module, const hi_char *conf_item,
    hi_bool default_val, hi_bool *out_value);

/* *
 * @brief     set string conf item of certain module
 * @param[in] cfg_name : The config name
 * @param[in] module : The module name
 * @param[in] conf_item : The confitem name
 * @param[in] value : value
 * @return    0 success,non-zero error code.
 * @exception None
 * @author    MobileCam Reference Develop Team
 * @date      2017/12/15
 */
hi_s32 ot_confaccess_set_string(const hi_char *cfg_name, const hi_char *module, const hi_char *conf_item,
    const hi_char *value);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* OT_CONFACCESS_H */
