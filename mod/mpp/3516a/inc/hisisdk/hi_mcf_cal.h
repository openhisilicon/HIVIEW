 /*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2018-2019. All rights reserved.
 * Description: calibration for mcf algorithm
 * Author: ISP SW
 * Create: 2019-6-18
 */

#ifndef __HI_MCF_CAL_H__
#define __HI_MCF_CAL_H__

#include "hi_type.h"
#include "hi_comm_video.h"
#include "hi_comm_mcf.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

hi_s32 hi_mcf_calibration(VIDEO_FRAME_S *pic_in_match, VIDEO_FRAME_S *pic_in_refer,
    hi_mcf_feature_info *feature_info, hi_mcf_cal *mcf_cal_info);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* end of __cplusplus */

#endif /* hi_mcf_cal.h */
