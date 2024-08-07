/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */

#ifndef __HI_COMMON_AIDRC_H__
#define __HI_COMMON_AIDRC_H__

#include "hi_common.h"
#include "hi_common_video.h"
#include "ot_common_aidrc.h"

#ifdef __cplusplus
extern "C" {
#endif

#define HI_AIDRC_MODE_NORMAL   OT_AIDRC_MODE_NORMAL
#define HI_AIDRC_MODE_ADVANCED OT_AIDRC_MODE_ADVANCED
#define HI_AIDRC_MODE_BUTT     OT_AIDRC_MODE_BUTT
#define HI_AIDRC_LMIX_NODE_NUM OT_AIDRC_LMIX_NODE_NUM

typedef ot_aidrc_model                       hi_aidrc_model;
typedef ot_aidrc_blend                       hi_aidrc_blend;
typedef ot_aidrc_param                       hi_aidrc_param;
typedef ot_aidrc_mode                        hi_aidrc_mode;
typedef ot_aidrc_attr                        hi_aidrc_attr;
typedef ot_aidrc_cfg                         hi_aidrc_cfg;
typedef ot_aidrc_smooth                      hi_aidrc_smooth;

#ifdef __cplusplus
}
#endif

#endif /* __HI_COMMON_AIDRC_H__ */
