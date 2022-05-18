/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2019-2020. All rights reserved.
 * Description: hi_aacdec.h
 * Author: Hisilicon multimedia software group
 * Create: 2019-07-10
 * History: 2019-07-10 created for CSEC
 */

#ifndef __HI_AACDEC_H__
#define __HI_AACDEC_H__

#include "hi_type.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cpluscplus */
#endif /* __cpluscplus */

#ifndef HI_AAC_MAX_CHN_NUM
#define HI_AAC_MAX_CHN_NUM 2
#endif
#define HI_AAC_MAX_SAMPLE_NUM 1024

/*
 * according to spec (13818-7 section 8.2.2, 14496-3 section 4.5.3),
 * 6144 bits = 768 bytes per SCE or CCE-I, 12288 bits = 1536 bytes per CPE
 */
#define HI_AAC_MAINBUF_SIZE (768 * HI_AAC_MAX_CHN_NUM)

#define HI_AAC_NUM_PROFILES 3
#define HI_AAC_PROFILE_MP   0
#define HI_AAC_PROFILE_LC   1
#define HI_AAC_PROFILE_SSR  2

#define HI_AACDEC_VERSION_MAX_BYTE 64 /* version max byte size */

/* Defines AACDEC error code */
typedef enum {
    HI_ERR_AAC_NONE                          =   0,    /* no decode error */
    HI_ERR_AAC_INDATA_UNDERFLOW              =  -1,    /* not enough input data */
    HI_ERR_AAC_NULL_POINTER                  =  -2,    /* null pointer */
    HI_ERR_AAC_INVALID_ADTS_HEADER           =  -3,    /* invalid adts header */
    HI_ERR_AAC_INVALID_ADIF_HEADER           =  -4,    /* invalid adif header */
    HI_ERR_AAC_INVALID_FRAME                 =  -5,    /* invalid frame */
    HI_ERR_AAC_MPEG4_UNSUPPORTED             =  -6,    /* upsupport mpeg4 format */
    HI_ERR_AAC_CHANNEL_MAP                   =  -7,    /* channel map error */
    HI_ERR_AAC_SYNTAX_ELEMENT                =  -8,    /* element error */
    HI_ERR_AAC_DEQUANT                       =  -9,    /* dequant error */
    HI_ERR_AAC_STEREO_PROCESS                = -10,    /* stereo process error */
    HI_ERR_AAC_PNS                           = -11,    /* pns process error */
    HI_ERR_AAC_SHORT_BLOCK_DEINT             = -12,    /* reserved */
    HI_ERR_AAC_TNS                           = -13,    /* TNS process error */
    HI_ERR_AAC_IMDCT                         = -14,    /* IMDCT process error */
    HI_ERR_AAC_NCHANS_TOO_HIGH               = -15,    /* unsupport mutil channel */
    HI_ERR_AAC_SBR_INIT                      = -16,    /* SBR init error */
    HI_ERR_AAC_SBR_BITSTREAM                 = -17,    /* SBR bitstream error */
    HI_ERR_AAC_SBR_DATA                      = -18,    /* SBR data error */
    HI_ERR_AAC_SBR_PCM_FORMAT                = -19,    /* SBR pcm data error */
    HI_ERR_AAC_SBR_NCHANS_TOO_HIGH           = -20,    /* unsupport SBR multi channel */
    HI_ERR_AAC_SBR_SINGLERATE_UNSUPPORTED    = -21,    /* SBR invalid samplerate */
    HI_ERR_AAC_RAWBLOCK_PARAMS               = -22,    /* invalid RawBlock params */
    HI_ERR_AAC_PS_INIT                       = -23,    /* PS init error */
    HI_ERR_AAC_CH_MAPPING                    = -24,
    HI_ERR_AAC_WRAP_UNKNOWN                  = -9999,  /* reserved */
    HI_ERR_AAC_OUT_OF_MEMORY                 = 2,      /* Heap returned NULL pointer. Output buffer is invalid. */
    HI_ERR_AAC_UNKNOWN                       = 5,      /* Error condition is of unknown reason,
                                                          or from a another module. Output buffer is invalid. */
    HI_ERR_AAC_TRANSPORT_SYNC_ERROR          = 4097,   /* The transport decoder had syncronisation problems.
                                                          Do not exit decoding. Just feed new bitstream data. */
    HI_ERR_AAC_NOT_ENOUGH_BITS               = 4098,   /* The input buffer ran out of bits. */
    HI_ERR_AAC_TRANSPORT_FATAL_ERROR         = 4099,   /* The transport decoder occut fatal error. Reset Tranport */
    HI_ERR_AAC_INVALID_HANDLE                = 8193,   /* The handle passed to the function call was invalid (NULL). */
    HI_ERR_AAC_UNSUPPORTED_AOT               = 8194,   /* The AOT found in the configuration is not supported. */
    HI_ERR_AAC_UNSUPPORTED_FORMAT            = 8195,   /* The bitstream format is not supported.  */
    HI_ERR_AAC_UNSUPPORTED_ER_FORMAT         = 8196,   /* The error resilience tool format is not supported. */
    HI_ERR_AAC_UNSUPPORTED_EPCONFIG          = 8197,   /* The error protection format is not supported. */
    HI_ERR_AAC_UNSUPPORTED_MULTILAYER        = 8198,   /* More than one layer for AAC scalable is not supported. */
    HI_ERR_AAC_UNSUPPORTED_CHANNELCONFIG     = 8199,   /* The channel configuration (either number or arrangement)
                                                          is not supported. */
    HI_ERR_AAC_UNSUPPORTED_SAMPLINGRATE      = 8200,   /* The sample rate specified in the configuration
                                                          is not supported. */
    HI_ERR_AAC_INVALID_SBR_CONFIG            = 8201,   /* The SBR configuration is not supported. */
    HI_ERR_AAC_SET_PARAM_FAIL                = 8202,   /* The parameter could not be set. Either the value was out
                                                          of range or the parameter does not exist. */
    HI_ERR_AAC_NEED_TO_RESTART               = 8203,   /* The decoder needs to be restarted, since the
                                                          requiered configuration change cannot be performed. */
    HI_ERR_AAC_TRANSPORT_ERROR               = 16385,  /* The transport decoder encountered an unexpected error. */
    HI_ERR_AAC_PARSE_ERROR                   = 16386,  /* Error while parsing the bitstream.
                                                          Most probably it is corrupted, or the system crashed. */
    HI_ERR_AAC_UNSUPPORTED_EXTENSION_PAYLOAD = 16387,  /* Error while parsing the extension payload of the bitstream.
                                                          The extension payload type found is not supported. */
    HI_ERR_AAC_DECODE_FRAME_ERROR            = 16388,  /* The parsed bitstream value is out of range. Most probably
                                                          the bitstream is corrupt, or the system crashed. */
    HI_ERR_AAC_CRC_ERROR                     = 16389,  /* The embedded CRC did not match. */
    HI_ERR_AAC_INVALID_CODE_BOOK             = 16390,  /* An invalid codebook was signalled. Most probably the
                                                          bitstream is corrupt, or the system crashed. */
    HI_ERR_AAC_UNSUPPORTED_PREDICTION        = 16391,  /* Predictor found, but not supported in the AAC Low
                                                          Complexity profile. Most probably the bitstream is corrupt,
                                                          or has a wrong format. */
    HI_ERR_AAC_UNSUPPORTED_CCE               = 16392,  /* A CCE element was found which is not supported. Most
                                                          probably the bitstream is corrupt, or has a wrong format. */
    HI_ERR_AAC_UNSUPPORTED_LFE               = 16393,  /* A LFE element was found which is not supported. Most
                                                          probably the bitstream is corrupt, or has a wrong format. */
    HI_ERR_AAC_UNSUPPORTED_GAIN_CONTROL_DATA = 16394,  /* Gain control data found but not supported. Most probably
                                                          the bitstream is corrupt, or has a wrong format. */
    HI_ERR_AAC_UNSUPPORTED_SBA               = 16395,  /* SBA found, but currently not supported
                                                          in the BSAC profile. */
    HI_ERR_AAC_TNS_READ_ERROR                = 16396,  /* Error while reading TNS data. Most probably the bitstream
                                                          is corrupt or the system crashed. */
    HI_ERR_AAC_RVLC_ERROR                    = 16397,  /* Error while decoding error resillient data. */
    HI_ERR_AAC_ANC_DATA_ERROR                = 32769,  /* Non severe error concerning the ancillary data handling. */
    HI_ERR_AAC_TOO_SMALL_ANC_BUFFER          = 32770,  /* The registered ancillary data buffer is too small to
                                                          receive the parsed data. */
    HI_ERR_AAC_TOO_MANY_ANC_ELEMENTS         = 32771,  /* More than the allowed number of ancillary data elements
                                                          should be written to buffer. */
} hi_aacdec_err;

typedef struct {
    int bit_rate;
    int chn_num;            /* channels,range:1,2 */
    int sample_rate_core;   /* inner sample rate */
    int sample_rate_out;    /* output sample rate */
    int bits_per_sample;    /* bitwidth ,range:16 */
    int output_samples;     /* output samples */
    int profile;            /* profile */
    int tns_used;           /* tns tools */
    int pns_used;           /* pns tools */
} hi_aacdec_frame_info;

typedef enum {
    HI_AACDEC_ADTS = 0,
    HI_AACDEC_LOAS = 1,
    HI_AACDEC_LATM_MCP1 = 2,
} hi_aacdec_transport_type;

typedef void *hi_aac_decoder;

typedef struct {
    hi_u8 version[HI_AACDEC_VERSION_MAX_BYTE];
} hi_aacdec_version;

hi_s32 hi_aacdec_get_version(hi_aacdec_version *version);

hi_aac_decoder hi_aacdec_init_decoder(hi_aacdec_transport_type transport_type);

hi_void hi_aacdec_free_decoder(hi_aac_decoder aac_decoder);

hi_s32 hi_aacdec_set_raw_mode(hi_aac_decoder aac_decoder, hi_s32 chn_num, hi_s32 sample_rate);

hi_s32 hi_aacdec_find_sync_header(hi_aac_decoder aac_decoder, hi_u8 **in_buf, hi_s32 *bytes_left);

hi_s32 hi_aacdec_frame(hi_aac_decoder aac_decoder, hi_u8 **in_buf, hi_s32 *bytes_left, hi_s16 *out_pcm);

hi_s32 hi_aacdec_set_eos_flag(hi_aac_decoder aac_decoder, hi_s32 eos_flag);

hi_s32 hi_aacdec_get_last_frame_info(hi_aac_decoder aac_decoder, hi_aacdec_frame_info *frame_info);

hi_s32 hi_aacdec_flush_codec(hi_aac_decoder aac_decoder);

hi_s32 hi_aacdec_register_mod(hi_void *mod_handle);

hi_void *hi_aac_sbrdec_get_handle(hi_void);

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cpluscplus */
#endif /* __cpluscplus */

#endif /* HI_AACDEC_H */
