/*
  Copyright (c), 2001-2024, Shenshu Tech. Co., Ltd.
 */

#ifndef OT_AACENC_H
#define OT_AACENC_H

#include "ot_type.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cpluscplus */

#ifdef MONO_ONLY
#define OT_AACENC_MAX_CHANNELS 1 /* aacenc encoder channels */
#else
#define OT_AACENC_MAX_CHANNELS 2
#endif

#define OT_AACENC_BLOCK_SIZE 1024 /* aacenc block size */

#define OT_AACENC_VERSION_MAX_BYTE 64 /* version max byte size */

/* Defines AACENC error code */
typedef enum {
    OT_AACENC_OK                     = 0x0000,  /* No error happened. All fine. */

    OT_AACENC_INVALID_HANDLE         = 0x0020,  /* Handle passed to function call was invalid. */
    OT_AACENC_MEMORY_ERROR           = 0x0021,  /* Memory allocation failed. */
    OT_AACENC_UNSUPPORTED_PARAMETER  = 0x0022,  /* Parameter not available. */
    OT_AACENC_INVALID_CONFIG         = 0x0023,  /* Configuration not provided. */

    OT_AACENC_INIT_ERROR             = 0x0040,  /* General initialization error. */
    OT_AACENC_INIT_AAC_ERROR         = 0x0041,  /* AAC library initialization error. */
    OT_AACENC_INIT_SBR_ERROR         = 0x0042,  /* SBR library initialization error. */
    OT_AACENC_INIT_TP_ERROR          = 0x0043,  /* Transport library initialization error. */
    OT_AACENC_INIT_META_ERROR        = 0x0044,  /* Meta data library initialization error. */

    OT_AACENC_ENCODE_ERROR           = 0x0060,  /* The encoding process was interrupted by an unexpected error. */

    OT_AACENC_ENCODE_EOF             = 0x0080   /* End of file reached. */
} ot_aacenc_error;

/* Defines AACENC quality */
typedef enum {
    OT_AACENC_QUALITY_EXCELLENT = 0,
    OT_AACENC_QUALITY_HIGH = 1,
    OT_AACENC_QUALITY_MEDIUM = 2,
    OT_AACENC_QUALITY_LOW = 3,
} ot_aacenc_quality;

/* Defines AACENC format */
typedef enum {
    OT_AACLC = 0,    /* AAC-LC format */
    OT_EAAC = 1,     /* HEAAC or AAC+ or aacPlusV1 */
    OT_EAACPLUS = 2, /* AAC++ or aacPlusV2 */
    OT_AACLD = 3,    /* AAC LD(Low Delay) */
    OT_AACELD = 4,   /* AAC ELD(Low Delay) */
} ot_aacenc_format;

/* Defines AACENC container */
typedef enum {
    OT_AACENC_ADTS = 0,
    OT_AACENC_LOAS = 1,
    OT_AACENC_LATM_MCP1 = 2,
} ot_aacenc_transport_type;

/* Defines AACENC configuration */
typedef struct {
    ot_aacenc_quality quality;
    ot_aacenc_format coder_format;
    td_s16 bits_per_sample;
    td_s32 sample_rate;         /* audio file sample rate */
    td_s32 bit_rate;            /* encoder bit rate in bits/sec */
    td_s16 num_channels_in;     /* number of channels on input (1,2) */
    td_s16 num_channels_out;    /* number of channels on output (1,2) */
    td_s16 band_width;          /* targeted audio bandwidth in Hz */
    ot_aacenc_transport_type trans_type;
} ot_aacenc_config;

/* Defines AACENC version */
typedef struct {
    td_u8 version[OT_AACENC_VERSION_MAX_BYTE];
} ot_aacenc_version;

typedef td_u32 ot_aac_encoder;

td_s32 ot_aacenc_get_version(ot_aacenc_version *version);

td_s32 ot_aacenc_init_default_config(ot_aacenc_config *config);

td_s32 ot_aacenc_open(ot_aac_encoder **aac_encoder, const ot_aacenc_config *config);

td_s32 ot_aacenc_frame(ot_aac_encoder *aac_encoder, td_s16 *input_buf, td_u8 *output_buf, td_s32 *num_out_bytes);

td_void ot_aacenc_close(ot_aac_encoder *aac_encoder);

td_s32 ot_aacenc_register_mod(const td_void *mod_handle);

td_void *ot_aac_sbrenc_get_handle(td_void);

#ifdef __cplusplus
}
#endif /* __cpluscplus */

#endif /* OT_AACENC_H */