/*
  Copyright (c), 2001-2024, Shenshu Tech. Co., Ltd.
 */

#ifndef __HI_COMMON_AIBNR_H__
#define __HI_COMMON_AIBNR_H__

#include "hi_common.h"
#include "hi_common_video.h"
#include "ot_common_aibnr.h"

#ifdef __cplusplus
extern "C" {
#endif

#define HI_AIBNR_REF_MODE_NORM               OT_AIBNR_REF_MODE_NORM
#define HI_AIBNR_REF_MODE_NONE               OT_AIBNR_REF_MODE_NONE
#define HI_AIBNR_REF_MODE_BUTT               OT_AIBNR_REF_MODE_BUTT
#define HI_AIBNR_NLC_LUT_NUM                 OT_AIBNR_NLC_LUT_NUM

typedef ot_aibnr_ref_mode                    hi_aibnr_ref_mode;
typedef ot_aibnr_model                       hi_aibnr_model;
typedef ot_aibnr_manual_attr                 hi_aibnr_manual_attr;
typedef ot_aibnr_auto_attr                   hi_aibnr_auto_attr;
typedef ot_aibnr_attr                        hi_aibnr_attr;
typedef ot_aibnr_cfg                         hi_aibnr_cfg;
typedef ot_aibnr_status                      hi_aibnr_status;

typedef ot_aibnr_model_attr                  hi_aibnr_model_attr;
typedef ot_aibnr_model_info                  hi_aibnr_model_info;
typedef ot_aibnr_model_list                  hi_aibnr_model_list;
typedef ot_aibnr_nlc                         hi_aibnr_nlc;

#ifdef __cplusplus
}
#endif

#endif /* __HI_COMMON_AIBNR_H__ */
