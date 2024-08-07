/*
  Copyright (c), 2001-2024, Shenshu Tech. Co., Ltd.
 */

#ifndef OT_RESAMPLE_H
#define OT_RESAMPLE_H

#include "ot_type.h"

#ifdef __cplusplus
extern "C" {
#endif

#define MAXFRAMESIZE 2048

#ifndef OT_ERR_RESAMPLE_PREFIX
#define OT_ERR_RESAMPLE_PREFIX 0x80000000
#endif

/* input handle is invalid */
#define OT_ERR_RESAMPLE_HANDLE (OT_ERR_RESAMPLE_PREFIX | 0x0001)

/* pcm circ buffer state is invalid */
#define OT_ERR_RESAMPLE_PCMBUF (OT_ERR_RESAMPLE_PREFIX | 0x0002)

/*
 * input sample number is more than MAXFRAMESIZE or input buffer size
 * , or input sample number is not invalid (eg. even)
 */
#define OT_ERR_RESAMPLE_SAMPLE_NUMBER (OT_ERR_RESAMPLE_PREFIX | 0x0003)

/* output pcm buffer space is not enough */
#define OT_ERR_RESAMPLE_OUTPCM_SPACE (OT_ERR_RESAMPLE_PREFIX | 0x0004)

/* the channels of input pcm is invalid */
#define OT_ERR_PCM_CHANNEL (OT_ERR_RESAMPLE_PREFIX | 0x0005)

/* the bit width of input pcm is invalid */
#define OT_ERR_PCM_FORMAT (OT_ERR_RESAMPLE_PREFIX | 0x0006)

/* invalid bypass flag */
#define OT_ERR_INVALID_BYPASSFLAG (OT_ERR_RESAMPLE_PREFIX | 0x0007)

/* error unknown */
#define OT_ERR_UNKNOWN (OT_ERR_RESAMPLE_PREFIX | 0x0008)

/* input empty pointer */
#define OT_ERR_INPUT_EMPTY_POINTER (OT_ERR_RESAMPLE_PREFIX | 0x0009)

/*
 * function:    ot_resample_create
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
td_void *ot_resample_create(td_s32 in_rate, td_s32 out_rate, td_s32 chn_num);

/*
 * function:    ot_resample_process
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
td_s32 ot_resample_process(td_void *instance, const td_s16 *in_buf, td_s32 in_samples, td_s16 *out_buf);

/*
 * function:    ot_resample_destroy
 *
 * Description: free platform-specific data allocated by resampler_create
 *
 * inputs:      valid resampler instance pointer (resampler)
 * outputs:     none
 *
 * return:      none
 */
td_void ot_resample_destroy(td_void *instance);

/*
 * function:    ot_resample_get_max_output_num
 *
 * Description: calculate max output number at specific input number
 *
 * inputs:      instance:     valid resampler instance pointer (td_handle)
 *              in_samples:  input data number per-channel, in_samples must be even
 * outputs:     none
 * return:      >=0:    success, return the max output number per-channel
 *              other:  fail, return error code
 * notes
 * 1  if stereo(chans==2), sure in_samples%2 == 0
 */
td_s32 ot_resample_get_max_output_num(td_void *instance, td_s32 in_samples);

#ifdef __cplusplus
}
#endif

#endif
