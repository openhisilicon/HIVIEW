/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */

#ifndef HI_OPUSDEC_H
#define HI_OPUSDEC_H

#include "hi_type.h"
#include "ot_opusdec.h"

#ifdef __cplusplus
extern "C" {
#endif /* End of #ifdef __cplusplus */

#define HI_OPUSDEC_MAX_OUT_CHANNEL OT_OPUSDEC_MAX_OUT_CHANNEL
#define HI_OPUSDEC_MAX_OUT_SAMPLES OT_OPUSDEC_MAX_OUT_SAMPLES
#define HI_OPUSDEC_MIN_BUF_SIZE OT_OPUSDEC_MIN_BUF_SIZE
#define HI_OPUSDEC_MAX_PACKET OT_OPUSDEC_MAX_PACKET

#define HI_ERR_OPUSDEC_NONE OT_ERR_OPUSDEC_NONE
#define HI_ERR_OPUSDEC_NOT_ENOUGH_DATA OT_ERR_OPUSDEC_NOT_ENOUGH_DATA
#define HI_ERR_OPUSDEC_NULL_POINTER OT_ERR_OPUSDEC_NULL_POINTER
#define HI_ERR_OPUSDEC_INVALID_PAYLOAD_LENGTH OT_ERR_OPUSDEC_INVALID_PAYLOAD_LENGTH
#define HI_ERR_OPUSDEC_RANGE_MISMATCH OT_ERR_OPUSDEC_RANGE_MISMATCH
#define HI_ERR_OPUSDEC_DECODE_FAILED OT_ERR_OPUSDEC_DECODE_FAILED
#define HI_ERR_OPUSDEC_UNKNOWN OT_ERR_OPUSDEC_UNKNOWN
typedef ot_opusdec_error hi_opusdec_error;
typedef ot_opus_decoder hi_opus_decoder;
typedef ot_opusdec_config hi_opusdec_config;

hi_opus_decoder hi_opusdec_create(const hi_opusdec_config *config);

hi_void hi_opusdec_destroy(hi_opus_decoder decoder);

hi_s32 hi_opusdec_process_frame(hi_opus_decoder decoder, hi_u8 **in_buf, hi_s32 *in_left_byte, hi_s16 *out_buf,
    hi_u32 *out_bytes_per_chn);

#ifdef __cplusplus
}
#endif /* End of #ifdef __cplusplus */

#endif
