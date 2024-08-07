/*
  Copyright (c), 2001-2024, Shenshu Tech. Co., Ltd.
 */

#ifndef HI_OPUSENC_H
#define HI_OPUSENC_H

#include "hi_type.h"
#include "ot_opusenc.h"

#ifdef __cplusplus
extern "C" {
#endif /* End of #ifdef __cplusplus */

#define HI_OPUSENC_MAX_CHANNEL_NUM OT_OPUSENC_MAX_CHANNEL_NUM
#define HI_OPUSENC_MAX_IN_SAMPLES OT_OPUSENC_MAX_IN_SAMPLES
#define HI_OPUSENC_MAX_PACKET OT_OPUSENC_MAX_PACKET

#define HI_ERR_OPUSENC_NONE OT_ERR_OPUSENC_NONE
#define HI_ERR_OPUSENC_NULL_POINTER OT_ERR_OPUSENC_NULL_POINTER
#define HI_ERR_OPUSENC_INVALID_PAYLOAD_LENGTH OT_ERR_OPUSENC_INVALID_PAYLOAD_LENGTH
#define HI_ERR_OPUSENC_ENCODE_FAILED OT_ERR_OPUSENC_ENCODE_FAILED
#define HI_ERR_OPUSENC_UNKNOWN OT_ERR_OPUSENC_UNKNOWN
typedef ot_opusenc_error hi_opusenc_error;
#define HI_OPUSENC_APPLICATION_VOIP OT_OPUSENC_APPLICATION_VOIP
#define HI_OPUSENC_APPLICATION_AUDIO OT_OPUSENC_APPLICATION_AUDIO
#define HI_OPUSENC_APPLICATION_RESTRICTED_LOWDELAY OT_OPUSENC_APPLICATION_RESTRICTED_LOWDELAY
typedef ot_opusenc_application hi_opusenc_application;
typedef ot_opusenc_config hi_opusenc_config;
typedef ot_opus_encoder hi_opus_encoder;

hi_opus_encoder hi_opusenc_create(const hi_opusenc_config *config);

hi_void hi_opusenc_destroy(hi_opus_encoder encoder);

hi_s32 hi_opusenc_process_frame(hi_opus_encoder encoder, hi_s16 *in_buf, hi_s32 samples, hi_u8 *out_buf,
    hi_u32 *out_valid_len);

#ifdef __cplusplus
}
#endif /* End of #ifdef __cplusplus */

#endif
