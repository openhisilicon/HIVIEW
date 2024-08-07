/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */

#ifndef HI_MP3ENC_H
#define HI_MP3ENC_H

#include "hi_type.h"
#include "ot_mp3enc.h"

#ifdef __cplusplus
extern "C" {
#endif
typedef ot_mp3enc_attr hi_mp3enc_attr;
typedef ot_mp3enc_handle hi_mp3enc_handle;

#define HI_MP3ENC_MAX_CHN_NUM OT_MP3ENC_MAX_CHN_NUM
#define HI_MP3ENC_MAX_IN_SAMPELS OT_MP3ENC_MAX_IN_SAMPELS
#define HI_MP3ENC_MAX_OUTBUF_SIZE OT_MP3ENC_MAX_OUTBUF_SIZE

hi_mp3enc_handle hi_mp3enc_create(const hi_mp3enc_attr *attr);

hi_s32 hi_mp3enc_process_frame(hi_mp3enc_handle mp3_encoder, hi_s16 *pcm_buf, hi_u8 *out_buf, hi_s32 *num_out_bytes);

hi_void hi_mp3enc_destroy(hi_mp3enc_handle mp3_encoder);

#ifdef __cplusplus
}
#endif
#endif /* HI_MP3ENC_H */
