/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */

#ifndef __HI_RESAMPLE_H__
#define __HI_RESAMPLE_H__

#include "hi_type.h"
#include "ot_resample.h"


#ifdef __cplusplus
extern "C" {
#endif

#define HI_ERR_RESAMPLE_PREFIX OT_ERR_RESAMPLE_PREFIX
#define HI_ERR_RESAMPLE_HANDLE OT_ERR_RESAMPLE_HANDLE
#define HI_ERR_RESAMPLE_PCMBUF OT_ERR_RESAMPLE_PCMBUF
#define HI_ERR_RESAMPLE_SAMPLE_NUMBER OT_ERR_RESAMPLE_SAMPLE_NUMBER
#define HI_ERR_RESAMPLE_OUTPCM_SPACE OT_ERR_RESAMPLE_OUTPCM_SPACE
#define HI_ERR_PCM_CHANNEL OT_ERR_PCM_CHANNEL
#define HI_ERR_PCM_FORMAT OT_ERR_PCM_FORMAT
#define HI_ERR_INVALID_BYPASSFLAG OT_ERR_INVALID_BYPASSFLAG
#define HI_ERR_UNKNOWN OT_ERR_UNKNOWN
#define HI_ERR_INPUT_EMPTY_POINTER OT_ERR_INPUT_EMPTY_POINTER

hi_void *hi_resample_create(hi_s32 in_rate, hi_s32 out_rate, hi_s32 chn_num);
hi_s32 hi_resample_process(hi_void *instance, const hi_s16 *in_buf, hi_s32 in_samples, hi_s16 *out_buf);
hi_void hi_resample_destroy(hi_void *instance);
hi_s32 hi_resample_get_max_output_num(hi_void *instance, hi_s32 in_samples);

#ifdef __cplusplus
}
#endif
#endif /* __HI_RESAMPLE_H__ */
