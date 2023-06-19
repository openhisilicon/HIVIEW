/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */

#ifndef __HI_AACENC_H__
#define __HI_AACENC_H__

#include "hi_type.h"
#include "ot_aacenc.h"


#ifdef __cplusplus
extern "C" {
#endif

#define HI_AACENC_MAX_CHANNELS OT_AACENC_MAX_CHANNELS
#define HI_AACENC_MAX_CHANNELS OT_AACENC_MAX_CHANNELS
#define HI_AACENC_BLOCK_SIZE OT_AACENC_BLOCK_SIZE
#define HI_AACENC_VERSION_MAX_BYTE OT_AACENC_VERSION_MAX_BYTE
#define HI_AACENC_OK OT_AACENC_OK
#define HI_AACENC_INVALID_HANDLE OT_AACENC_INVALID_HANDLE
#define HI_AACENC_MEMORY_ERROR OT_AACENC_MEMORY_ERROR
#define HI_AACENC_UNSUPPORTED_PARAMETER OT_AACENC_UNSUPPORTED_PARAMETER
#define HI_AACENC_INVALID_CONFIG OT_AACENC_INVALID_CONFIG
#define HI_AACENC_INIT_ERROR OT_AACENC_INIT_ERROR
#define HI_AACENC_INIT_AAC_ERROR OT_AACENC_INIT_AAC_ERROR
#define HI_AACENC_INIT_SBR_ERROR OT_AACENC_INIT_SBR_ERROR
#define HI_AACENC_INIT_TP_ERROR OT_AACENC_INIT_TP_ERROR
#define HI_AACENC_INIT_META_ERROR OT_AACENC_INIT_META_ERROR
#define HI_AACENC_ENCODE_ERROR OT_AACENC_ENCODE_ERROR
#define HI_AACENC_ENCODE_EOF OT_AACENC_ENCODE_EOF
typedef ot_aacenc_error hi_aacenc_error;
#define HI_AACENC_QUALITY_EXCELLENT OT_AACENC_QUALITY_EXCELLENT
#define HI_AACENC_QUALITY_HIGH OT_AACENC_QUALITY_HIGH
#define HI_AACENC_QUALITY_MEDIUM OT_AACENC_QUALITY_MEDIUM
#define HI_AACENC_QUALITY_LOW OT_AACENC_QUALITY_LOW
typedef ot_aacenc_quality hi_aacenc_quality;
#define HI_AACLC OT_AACLC
#define HI_EAAC OT_EAAC
#define HI_EAACPLUS OT_EAACPLUS
#define HI_AACLD OT_AACLD
#define HI_AACELD OT_AACELD
typedef ot_aacenc_format hi_aacenc_format;
#define HI_AACENC_ADTS OT_AACENC_ADTS
#define HI_AACENC_LOAS OT_AACENC_LOAS
#define HI_AACENC_LATM_MCP1 OT_AACENC_LATM_MCP1
typedef ot_aacenc_transport_type hi_aacenc_transport_type;
typedef ot_aacenc_config hi_aacenc_config;
typedef ot_aacenc_version hi_aacenc_version;
typedef ot_aac_encoder hi_aac_encoder;

hi_s32 hi_aacenc_get_version(hi_aacenc_version *version);

hi_s32 hi_aacenc_init_default_config(hi_aacenc_config *config);

hi_s32 hi_aacenc_open(hi_aac_encoder **aac_encoder, const hi_aacenc_config *config);

hi_s32 hi_aacenc_frame(hi_aac_encoder *aac_encoder, hi_s16 *input_buf, hi_u8 *output_buf, hi_s32 *num_out_bytes);

hi_void hi_aacenc_close(hi_aac_encoder *aac_encoder);

hi_s32 hi_aacenc_register_mod(const hi_void *mod_handle);

hi_void *hi_aac_sbrenc_get_handle(hi_void);

#ifdef __cplusplus
}
#endif
#endif /* __HI_AACENC_H__ */
