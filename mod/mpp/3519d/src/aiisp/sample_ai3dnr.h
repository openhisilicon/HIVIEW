/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */
#ifndef __SAMPLE_AI3DNR_H__
#define __SAMPLE_AI3DNR_H__

#include "sample_aiisp_common.h"
#include "hi_common_ai3dnr.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

typedef struct {
    hi_ai3dnr_net_type net_type;
} sample_ai3dnr_param;

hi_s32 sample_ai3dnr(sample_ai3dnr_param *ai3dnr_param);
hi_s32 sample_ai3dnr_and_mcf(hi_void);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif
