/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */

#ifndef __HI_COMMON_AE_H__
#define __HI_COMMON_AE_H__

#include "hi_type.h"
#include "hi_debug.h"
#include "hi_common_isp.h"
#include "ot_common_ae.h"


#ifdef __cplusplus
extern "C" {
#endif

#define HI_AE_LIB_NAME OT_AE_LIB_NAME
#define HI_ISP_AE_DEBUG_ATTR_SET OT_ISP_AE_DEBUG_ATTR_SET
#define HI_ISP_AE_DEBUG_ATTR_GET OT_ISP_AE_DEBUG_ATTR_GET
#define HI_ISP_AE_CTRL_BUTT OT_ISP_AE_CTRL_BUTT
typedef ot_isp_ae_ctrl_cmd hi_isp_ae_ctrl_cmd;
typedef ot_isp_ae_dbg_attr hi_isp_ae_dbg_attr;
typedef ot_isp_ae_dbg_status hi_isp_ae_dbg_status;
#define HI_ISP_AE_ACCURACY_DB OT_ISP_AE_ACCURACY_DB
#define HI_ISP_AE_ACCURACY_LINEAR OT_ISP_AE_ACCURACY_LINEAR
#define HI_ISP_AE_ACCURACY_TABLE OT_ISP_AE_ACCURACY_TABLE
#define HI_ISP_AE_ACCURACY_BUTT OT_ISP_AE_ACCURACY_BUTT
typedef ot_isp_ae_accuracy_type hi_isp_ae_accuracy_type;
typedef ot_isp_ae_accuracy hi_isp_ae_accuracy;
typedef ot_isp_quick_start_param hi_isp_quick_start_param;
typedef ot_isp_ae_sensor_default hi_isp_ae_sensor_default;
typedef ot_isp_ae_fswdr_attr hi_isp_ae_fswdr_attr;
typedef ot_isp_ae_param_reg hi_isp_ae_param_reg;
typedef ot_isp_ae_convert_param hi_isp_ae_convert_param;
typedef ot_isp_ae_int_time_range hi_isp_ae_int_time_range;
typedef ot_isp_ae_sensor_exp_func hi_isp_ae_sensor_exp_func;
typedef ot_isp_ae_sensor_register hi_isp_ae_sensor_register;

#ifdef __cplusplus
}
#endif
#endif /* __HI_COMMON_AE_H__ */
