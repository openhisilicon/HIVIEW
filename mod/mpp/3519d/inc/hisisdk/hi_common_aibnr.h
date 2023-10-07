/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
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

typedef ot_aibnr_ref_mode                    hi_aibnr_ref_mode;
typedef ot_aibnr_model                       hi_aibnr_model;
typedef ot_aibnr_manual_attr                 hi_aibnr_manual_attr;
typedef ot_aibnr_auto_attr                   hi_aibnr_auto_attr;
typedef ot_aibnr_attr                        hi_aibnr_attr;
typedef ot_aibnr_cfg                         hi_aibnr_cfg;

#ifdef __cplusplus
}
#endif

#endif /* __HI_COMMON_AIBNR_H__ */
