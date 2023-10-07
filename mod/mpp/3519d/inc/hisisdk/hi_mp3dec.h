/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */

#ifndef HI_MP3DEC_H
#define HI_MP3DEC_H

#include "hi_type.h"
#include "ot_mp3dec.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef ot_mp3dec_handle hi_mp3dec_handle;
typedef ot_mp3dec_frame_info hi_mp3dec_frame_info;

#define HI_MP3DEC_MAX_CHN_NUMS OT_MP3DEC_MAX_CHN_NUMS
#define HI_MP3DEC_MAX_OUT_SAMPLES OT_MP3DEC_MAX_OUT_SAMPLES
#define HI_MP3DEC_MIN_BUF_SIZE OT_MP3DEC_MIN_BUF_SIZE
#define HI_MPEG_1 OT_MPEG_1
#define HI_MPEG_2 OT_MPEG_2
#define HI_MPEG_25 OT_MPEG_25
#define HI_ERR_MP3_NONE OT_ERR_MP3_NONE
#define HI_ERR_MP3_INDATA_UNDERFLOW OT_ERR_MP3_INDATA_UNDERFLOW
#define HI_ERR_MP3_MAINDATA_UNDERFLOW OT_ERR_MP3_MAINDATA_UNDERFLOW
#define HI_ERR_MP3_FREE_BITRATE_SYNC OT_ERR_MP3_FREE_BITRATE_SYNC
#define HI_ERR_MP3_OUT_OF_MEMORY OT_ERR_MP3_OUT_OF_MEMORY
#define HI_ERR_MP3_NULL_POINTER OT_ERR_MP3_NULL_POINTER
#define HI_ERR_MP3_INVALID_FRAMEHEADER OT_ERR_MP3_INVALID_FRAMEHEADER
#define HI_ERR_MP3_INVALID_SIDEINFO OT_ERR_MP3_INVALID_SIDEINFO
#define HI_ERR_MP3_INVALID_SCALEFACT OT_ERR_MP3_INVALID_SCALEFACT
#define HI_ERR_MP3_INVALID_HUFFCODES OT_ERR_MP3_INVALID_HUFFCODES
#define HI_ERR_MP3_FAIL_SYNC OT_ERR_MP3_FAIL_SYNC
#define HI_ERR_MP3_UNKNOWN OT_ERR_MP3_UNKNOWN

typedef ot_mpeg_version hi_mpeg_version;
typedef ot_mp3dec_error hi_mp3dec_error;

hi_mp3dec_handle hi_mp3dec_init_decoder(hi_void);

hi_s32 hi_mp3dec_find_sync_header(hi_mp3dec_handle mp3_decoder, hi_u8 **in_buff, hi_s32 *bytes_left);

hi_s32 hi_mp3dec_frame(hi_mp3dec_handle mp3_decoder, hi_u8 **in_buff, hi_s32 *bytes_left, hi_s16 *out_pcm,
    hi_s32 reserved_num);

hi_void hi_mp3dec_free_decoder(hi_mp3dec_handle mp3_decoder);

hi_s32 hi_mp3dec_get_last_frame_info(hi_mp3dec_handle mp3_decoder, hi_mp3dec_frame_info *mp3dec_frame_info);

#ifdef __cplusplus
}
#endif
#endif /* HI_MP3DEC_H */