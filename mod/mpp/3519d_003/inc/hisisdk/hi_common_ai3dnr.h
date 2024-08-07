/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */

#ifndef __HI_COMMON_AI3DNR_H__
#define __HI_COMMON_AI3DNR_H__

#include "hi_common.h"
#include "hi_common_video.h"
#include "ot_common_ai3dnr.h"

#ifdef __cplusplus
extern "C" {
#endif

#define HI_AI3DNR_GAMMA_LUT_NUM               OT_AI3DNR_GAMMA_LUT_NUM
#define HI_AI3DNR_BAYERNR_DENOISE_Y_LUT_NUM   OT_AI3DNR_BAYERNR_DENOISE_Y_LUT_NUM
#define HI_AI3DNR_NET_TYPE_Y                  OT_AI3DNR_NET_TYPE_Y
#define HI_AI3DNR_NET_TYPE_UV                 OT_AI3DNR_NET_TYPE_UV
#define HI_AI3DNR_NET_TYPE_YUV                OT_AI3DNR_NET_TYPE_YUV
#define HI_AI3DNR_NET_TYPE_BUTT               OT_AI3DNR_NET_TYPE_BUTT

typedef ot_ai3dnr_net_type                    hi_ai3dnr_net_type;
typedef ot_ai3dnr_model                       hi_ai3dnr_model;
typedef ot_ai3dnr_strength                    hi_ai3dnr_strength;
typedef ot_ai3dnr_attr                        hi_ai3dnr_attr;
typedef ot_ai3dnr_cfg                         hi_ai3dnr_cfg;
typedef ot_ai3dnr_smooth                      hi_ai3dnr_smooth;

#ifdef __cplusplus
}
#endif

#endif
