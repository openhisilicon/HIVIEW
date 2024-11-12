/*
  Copyright (c), 2001-2024, Shenshu Tech. Co., Ltd.
 */

#ifndef OT_MP3ENC_H
#define OT_MP3ENC_H

#include "ot_type.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cpluscplus */

#define OT_MP3ENC_MAX_CHN_NUM 2 /* mp3 max number of channels */

#define OT_MP3ENC_MAX_IN_SAMPELS  1152 /* mp3 max input samples per-frame, per-channel */
#define OT_MP3ENC_MAX_OUTBUF_SIZE 2048 /* mp3 minium size of output buffer. UNIT:bytes */

typedef void *ot_mp3enc_handle;

typedef struct {
    td_u32 sample_rate;     /* encode pcm samplerate, support [8000,48000]. */
    td_u32 channels;        /* encode pcm channel, support [1,2]. */
    td_u32 bit_rate;        /* encode bitstream bitrate, support[32000, 320000]. */
    td_u32 quality;         /* encode bitstream quality, support[0, 9].
                                0:  Highest quality, very slow; 9:  Poor quality, but fast. default 7. */
} ot_mp3enc_attr;

/*
 * brief create and initial encoder device.
 * attention \n
 * Before before deocede,you must call this application programming interface (API) first.
 * param[in] mp3enc_attr mp3 encode attr.
 * retval :mp3enc_handle   : Success
 * retval :NULL      : FAILURE.
 * see \n
 */
ot_mp3enc_handle ot_mp3enc_create(const ot_mp3enc_attr *attr);

/*
 * brief Free MP3 decoder.
 * attention \n
 * param[in] mp3_encoder MP3encode handle
 * retval \n
 * see \n
 */
td_void ot_mp3enc_destroy(ot_mp3enc_handle mp3_encoder);

/*
 * brief encode input 1152(L2/L3) OR 384(L1) 16bit PCM samples per channel and output MPEG frame.
 * attention \n
 * param[in] mp3_encoder        MP3-Encoder handle
 * param[in] pcm_buf            address of the pointer of start-point of the input-pcm buf,
 * this data need be uninterleaved.
 * param[in] out_buf            pointer to BytesLeft that indicates bitstream numbers at input buffer,
 * indicates the left bytes.
 * param[in] num_out_bytes      the address of the out pcm buffer,pcm data in noninterlaced fotmat: L/L/L/... R/R/R/...
 * retval :: ERR_MP3_NONE :     Success
 * retval :: ERROR_CODE :       FAILURE
 */
td_s32 ot_mp3enc_process_frame(ot_mp3enc_handle mp3_encoder, td_s16 *pcm_buf, td_u8 *out_buf, td_s32 *num_out_bytes);

#ifdef __cplusplus
#if _cplusplus
}
#endif /* __cpluscplus */
#endif /* __cpluscplus */

#endif /* __MP3ENC_H__ */
