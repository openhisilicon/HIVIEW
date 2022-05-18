/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2006-2019. All rights reserved.
 * Description: hi_resample.h
 * Author: Hisilicon multimedia software group
 * Create: 2006-11-01
 * History:
 *    1.Date        : 2006-11-01
 *      Modification: Created file
 *    2.Date        : 2009-11-11
 *      Modification: add 1X2/1x4 and 4x1/2x1 SRC
 */

#ifndef __HI_RESAMPLER_H__
#define __HI_RESAMPLER_H__

#include "hi_type.h"

#ifdef __cplusplus
extern "C" {
#endif

#define MAXFRAMESIZE 2048

#ifndef HI_ERR_RESAMPLE_PREFIX
#define HI_ERR_RESAMPLE_PREFIX 0x80000000
#endif

/* input handle is invalid */
#define HI_ERR_RESAMPLE_HANDLE (HI_ERR_RESAMPLE_PREFIX | 0x0001)

/* pcm circ buffer state is invalid */
#define HI_ERR_RESAMPLE_PCMBUF (HI_ERR_RESAMPLE_PREFIX | 0x0002)

/*
 * input sample number is more than MAXFRAMESIZE or input buffer size
 * , or input sample number is not invalid (eg. even)
 */
#define HI_ERR_RESAMPLE_SAMPLE_NUMBER (HI_ERR_RESAMPLE_PREFIX | 0x0003)

/* output pcm buffer space is not enough */
#define HI_ERR_RESAMPLE_OUTPCM_SPACE (HI_ERR_RESAMPLE_PREFIX | 0x0004)

/* the channels of input pcm is invalid */
#define HI_ERR_PCM_CHANNEL (HI_ERR_RESAMPLE_PREFIX | 0x0005)

/* the bit width of input pcm is invalid */
#define HI_ERR_PCM_FORMAT (HI_ERR_RESAMPLE_PREFIX | 0x0006)

/* invalid bypass flag */
#define HI_ERR_INVALID_BYPASSFLAG (HI_ERR_RESAMPLE_PREFIX | 0x0007)

/* error unknown */
#define HI_ERR_UNKNOWN (HI_ERR_RESAMPLE_PREFIX | 0x0008)

/* input empty pointer */
#define HI_ERR_INPUT_EMPTY_POINTER (HI_ERR_RESAMPLE_PREFIX | 0x0009)

/*
 * function:    hi_resample_create
 *
 * Description: allocate memory for platform-specific data
 *              clear all the user-accessible fields
 *
 * inputs:      in_rate:  8000, 11025, 12000, 16000, 22050, 24000, 32000, 44100, 48000
 *              out_rate: 8000, 11025, 12000, 16000, 22050, 24000, 32000, 44100, 48000
 *              chn_num:  1 or 2
 * outputs:     none
 *
 * return:      handle to resampler instance, 0 if malloc fails
 */
hi_void *hi_resample_create(hi_s32 in_rate, hi_s32 out_rate, hi_s32 chn_num);

/*
 * function:    hi_resample_process
 *
 * Description: resample pcm data to specific samplerate, only for interlaced format
 *
 * inputs:      instance: valid resampler instance pointer (resampler)
 *              in_buf:   pointer to inputbuf
 *              in_samples: input number of sample pointers
 * outputs:     out_buf:  pointer to outputbuf
 *
 * return:      output sample number per-channel
 * notes:       sure in_samples < MAXFRAMESIZE
 *
 */
hi_s32 hi_resample_process(hi_void *instance, hi_s16 *in_buf, hi_s32 in_samples, hi_s16 *out_buf);

/*
 * function:    hi_resample_destroy
 *
 * Description: free platform-specific data allocated by resampler_create
 *
 * inputs:      valid resampler instance pointer (resampler)
 * outputs:     none
 *
 * return:      none
 */
hi_void hi_resample_destroy(hi_void *instance);

/*
 * function:    hi_resample_get_max_output_num
 *
 * Description: caculate max output number at specific input number
 *
 * inputs:      instance:     valid resampler instance pointer (hi_handle)
 *              in_samples:  input data number per-channel, in_samples must be even
 * outputs:     none
 * return:      >=0:    success, return the max output number per-channel
 *              other:  fail, return error code
 * notes:
 * 1  if stereo(chans==2), sure in_samples%2 == 0
 */
hi_s32 hi_resample_get_max_output_num(hi_void *instance, hi_s32 in_samples);

#ifdef __cplusplus
}
#endif

#endif
