/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */
#ifndef __SAMPLE_AIBNR_H__
#define __SAMPLE_AIBNR_H__

#include "sample_aiisp_common.h"
#include "hi_common_aibnr.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

typedef struct {
    hi_bool is_wdr_mode;
    hi_aibnr_ref_mode ref_mode;
    hi_bool is_blend;
} sample_aibnr_param;

hi_s32 sample_aibnr(sample_aibnr_param *aibnr_param);
hi_s32 sample_aibnr_snap(sample_aibnr_param *aibnr_param);
hi_s32 sample_aibnr_line_switch_to_wdr(hi_void);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif
