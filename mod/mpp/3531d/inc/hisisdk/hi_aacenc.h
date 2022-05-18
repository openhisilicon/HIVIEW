/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2019-2020. All rights reserved.
 * Description: hi_aacenc.h
 * Author: Hisilicon multimedia software group
 * Create: 2019-07-08
 * History:
 *   1.Date        : 2019-07-08
 *     Modification: Created file
 */

#ifndef __HI_AACENC_H__
#define __HI_AACENC_H__

#include "hi_type.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cpluscplus */
#endif /* __cpluscplus */

#ifdef MONO_ONLY
#define HI_AACENC_MAX_CHANNELS 1 /* aacenc encoder channels */
#else
#define HI_AACENC_MAX_CHANNELS 2
#endif

#define HI_AACENC_BLOCK_SIZE 1024 /* aacenc block size */

#define HI_AACENC_VERSION_MAX_BYTE 64 /* version max byte size */

/* Defines AACENC error code */
typedef enum {
    HI_AACENC_OK                     = 0x0000,  /* No error happened. All fine. */

    HI_AACENC_INVALID_HANDLE         = 0x0020,  /* Handle passed to function call was invalid. */
    HI_AACENC_MEMORY_ERROR           = 0x0021,  /* Memory allocation failed. */
    HI_AACENC_UNSUPPORTED_PARAMETER  = 0x0022,  /* Parameter not available. */
    HI_AACENC_INVALID_CONFIG         = 0x0023,  /* Configuration not provided. */

    HI_AACENC_INIT_ERROR             = 0x0040,  /* General initialization error. */
    HI_AACENC_INIT_AAC_ERROR         = 0x0041,  /* AAC library initialization error. */
    HI_AACENC_INIT_SBR_ERROR         = 0x0042,  /* SBR library initialization error. */
    HI_AACENC_INIT_TP_ERROR          = 0x0043,  /* Transport library initialization error. */
    HI_AACENC_INIT_META_ERROR        = 0x0044,  /* Meta data library initialization error. */

    HI_AACENC_ENCODE_ERROR           = 0x0060,  /* The encoding process was interrupted by an unexpected error. */

    HI_AACENC_ENCODE_EOF             = 0x0080   /* End of file reached. */
} hi_aacenc_error;

/* Defines AACENC quality */
typedef enum {
    HI_AACENC_QUALITY_EXCELLENT = 0,
    HI_AACENC_QUALITY_HIGH = 1,
    HI_AACENC_QUALITY_MEDIUM = 2,
    HI_AACENC_QUALITY_LOW = 3,
} hi_aacenc_quality;

/* Defines AACENC format */
typedef enum {
    HI_AACLC = 0,    /* AAC-LC format */
    HI_EAAC = 1,     /* HEAAC or AAC+ or aacPlusV1 */
    HI_EAACPLUS = 2, /* AAC++ or aacPlusV2 */
    HI_AACLD = 3,    /* AAC LD(Low Delay) */
    HI_AACELD = 4,   /* AAC ELD(Low Delay) */
} hi_aacenc_format;

/* Defines AACENC container */
typedef enum {
    HI_AACENC_ADTS = 0,
    HI_AACENC_LOAS = 1,
    HI_AACENC_LATM_MCP1 = 2,
} hi_aacenc_transport_type;

/* Defines AACENC configuration */
typedef struct {
    hi_aacenc_quality quality;
    hi_aacenc_format coder_format;
    hi_s16 bits_per_sample;
    hi_s32 sample_rate;         /* audio file sample rate */
    hi_s32 bit_rate;            /* encoder bit rate in bits/sec */
    hi_s16 num_channels_in;     /* number of channels on input (1,2) */
    hi_s16 num_channels_out;    /* number of channels on output (1,2) */
    hi_s16 band_width;          /* targeted audio bandwidth in Hz */
    hi_aacenc_transport_type trans_type;
} hi_aacenc_config;

/* Defines AACENC version */
typedef struct {
    hi_u8 version[HI_AACENC_VERSION_MAX_BYTE];
} hi_aacenc_version;

typedef hi_u32 hi_aac_encoder;

hi_s32 hi_aacenc_get_version(hi_aacenc_version *version);

hi_s32 hi_aacenc_init_default_config(hi_aacenc_config *config);

hi_s32 hi_aacenc_open(hi_aac_encoder **aac_encoder, hi_aacenc_config *config);

hi_s32 hi_aacenc_frame(hi_aac_encoder *aac_encoder, hi_s16 *input_buf, hi_u8 *output_buf, hi_s32 *num_out_bytes);

hi_void hi_aacenc_close(hi_aac_encoder *aac_encoder);

hi_s32 hi_aacenc_register_mod(hi_void *mod_handle);

hi_void *hi_aac_sbrenc_get_handle(hi_void);

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cpluscplus */
#endif /* __cpluscplus */

#endif /* HI_AACENC_H */
