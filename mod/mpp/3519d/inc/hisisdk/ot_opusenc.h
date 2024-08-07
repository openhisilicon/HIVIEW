/*
  Copyright (c), 2001-2024, Shenshu Tech. Co., Ltd.
 */

#ifndef OT_OPUSENC_H
#define OT_OPUSENC_H

#include "ot_type.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cpluscplus */

#define OT_OPUSENC_MAX_CHANNEL_NUM 2   /* opus max number of channels */
#define OT_OPUSENC_MAX_IN_SAMPLES 2880 /* opus max input samples per-frame, per-channel */
#define OT_OPUSENC_MAX_PACKET 1500     /* opus max out packet bytes */

typedef enum {
    OT_ERR_OPUSENC_NONE = 0,                    /* no error */
    OT_ERR_OPUSENC_NULL_POINTER = -1,           /* input null pointer */
    OT_ERR_OPUSENC_INVALID_PAYLOAD_LENGTH = -2, /* invalid payload length */
    OT_ERR_OPUSENC_ENCODE_FAILED = -3,          /* encode failed */

    OT_ERR_OPUSENC_UNKNOWN = -9999 /* reserved */
} ot_opusenc_error;

typedef enum {
    /* Best for most VoIP/videoconference applications where listening quality and intelligibility matter most */
    OT_OPUSENC_APPLICATION_VOIP = 2048,

    /*
     * Best for broadcast/high-fidelity application where the decoded audio
     * should be as close as possible to the input
     */
    OT_OPUSENC_APPLICATION_AUDIO = 2049,

    /* Only use when lowest-achievable latency is what matters most. Voice-optimized modes cannot be used. */
    OT_OPUSENC_APPLICATION_RESTRICTED_LOWDELAY = 2051,
} ot_opusenc_application;

typedef struct {
    td_s32 sample_rate; /* encode samplerate, only support 8000/12000/16000/24000/48000. */
    td_s32 channels;    /* encode channels, only support 1/2. */
    td_s32 bit_rate;    /* bit_rate from 500 to 512000 bits per second are meaningful */

    ot_opusenc_application app;
} ot_opusenc_config;

typedef td_void *ot_opus_encoder;

ot_opus_encoder ot_opusenc_create(const ot_opusenc_config *config);

td_void ot_opusenc_destroy(ot_opus_encoder encoder);

td_s32 ot_opusenc_process_frame(ot_opus_encoder encoder, td_s16 *in_buf, td_s32 samples, td_u8 *out_buf,
    td_u32 *out_valid_len);

#ifdef __cplusplus
}
#endif /* __cpluscplus */

#endif