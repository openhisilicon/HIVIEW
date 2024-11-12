/*
  Copyright (c), 2001-2024, Shenshu Tech. Co., Ltd.
 */

#ifndef OT_OPUSDEC_H
#define OT_OPUSDEC_H

#include "ot_type.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cpluscplus */

#define OT_OPUSDEC_MAX_OUT_CHANNEL 2   /* opus max output channel */
#define OT_OPUSDEC_MAX_OUT_SAMPLES 2880 /* opus max output samples per-frame, per-channel */
#define OT_OPUSDEC_MIN_BUF_SIZE 3840   /* opus minium size of input buffer. UNIT:bytes */
#define OT_OPUSDEC_MAX_PACKET 1500     /* opus max packet bytes */

typedef enum {
    OT_ERR_OPUSDEC_NONE = 0,                    /* no decode error */
    OT_ERR_OPUSDEC_NOT_ENOUGH_DATA = -1,        /* not enough input data */
    OT_ERR_OPUSDEC_NULL_POINTER = -2,           /* input null pointer */
    OT_ERR_OPUSDEC_INVALID_PAYLOAD_LENGTH = -3, /* invalid payload length */
    OT_ERR_OPUSDEC_RANGE_MISMATCH = -4,         /* range coder state mismatch */
    OT_ERR_OPUSDEC_DECODE_FAILED = -5,          /* decode failed */

    OT_ERR_OPUSDEC_UNKNOWN = -9999 /* reserved */
} ot_opusdec_error;

typedef td_void *ot_opus_decoder;

typedef struct {
    td_s32 sample_rate; /* Input bitstream samplerate, Support 8000/12000/16000/24000/48000. */
    td_s32 channels;    /* Input bitstream channels, Support 1/2. */
} ot_opusdec_config;

ot_opus_decoder ot_opusdec_create(const ot_opusdec_config *config);

td_void ot_opusdec_destroy(ot_opus_decoder decoder);

td_s32 ot_opusdec_process_frame(ot_opus_decoder decoder, td_u8 **in_buf, td_s32 *in_left_byte, td_s16 *out_buf,
    td_u32 *out_bytes_per_chn);

#ifdef __cplusplus
}
#endif /* __cpluscplus */

#endif
