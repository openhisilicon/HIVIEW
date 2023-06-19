/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */

#ifndef OT_AACDEC_H
#define OT_AACDEC_H

#include "ot_type.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cpluscplus */
#endif /* __cpluscplus */

#ifndef OT_AAC_MAX_CHN_NUM
#define OT_AAC_MAX_CHN_NUM 2
#endif
#define OT_AAC_MAX_SAMPLE_NUM 1024

/*
 * according to spec (13818-7 section 8.2.2, 14496-3 section 4.5.3),
 * 6144 bits = 768 bytes per SCE or CCE-I, 12288 bits = 1536 bytes per CPE
 */
#define OT_AAC_MAINBUF_SIZE (768 * OT_AAC_MAX_CHN_NUM)

#define OT_AAC_NUM_PROFILES 3
#define OT_AAC_PROFILE_MP   0
#define OT_AAC_PROFILE_LC   1
#define OT_AAC_PROFILE_SSR  2

#define OT_AACDEC_VERSION_MAX_BYTE 64 /* version max byte size */

/* Defines AACDEC error code */
typedef enum {
    OT_ERR_AAC_NONE                          =   0,    /* no decode error */
    OT_ERR_AAC_INDATA_UNDERFLOW              =  -1,    /* not enough input data */
    OT_ERR_AAC_NULL_POINTER                  =  -2,    /* null pointer */
    OT_ERR_AAC_INVALID_ADTS_HEADER           =  -3,    /* invalid adts header */
    OT_ERR_AAC_INVALID_ADIF_HEADER           =  -4,    /* invalid adif header */
    OT_ERR_AAC_INVALID_FRAME                 =  -5,    /* invalid frame */
    OT_ERR_AAC_MPEG4_UNSUPPORTED             =  -6,    /* upsupport mpeg4 format */
    OT_ERR_AAC_CHANNEL_MAP                   =  -7,    /* channel map error */
    OT_ERR_AAC_SYNTAX_ELEMENT                =  -8,    /* element error */
    OT_ERR_AAC_DEQUANT                       =  -9,    /* dequant error */
    OT_ERR_AAC_STEREO_PROCESS                = -10,    /* stereo process error */
    OT_ERR_AAC_PNS                           = -11,    /* pns process error */
    OT_ERR_AAC_SHORT_BLOCK_DEINT             = -12,    /* reserved */
    OT_ERR_AAC_TNS                           = -13,    /* TNS process error */
    OT_ERR_AAC_IMDCT                         = -14,    /* IMDCT process error */
    OT_ERR_AAC_NCHANS_TOO_HIGH               = -15,    /* unsupported mutil channel */
    OT_ERR_AAC_SBR_INIT                      = -16,    /* SBR init error */
    OT_ERR_AAC_SBR_BITSTREAM                 = -17,    /* SBR bitstream error */
    OT_ERR_AAC_SBR_DATA                      = -18,    /* SBR data error */
    OT_ERR_AAC_SBR_PCM_FORMAT                = -19,    /* SBR pcm data error */
    OT_ERR_AAC_SBR_NCHANS_TOO_HIGH           = -20,    /* unsupported SBR multi channel */
    OT_ERR_AAC_SBR_SINGLERATE_UNSUPPORTED    = -21,    /* SBR invalid samplerate */
    OT_ERR_AAC_RAWBLOCK_PARAMS               = -22,    /* invalid RawBlock params */
    OT_ERR_AAC_PS_INIT                       = -23,    /* PS init error */
    OT_ERR_AAC_CH_MAPPING                    = -24,
    OT_ERR_AAC_WRAP_UNKNOWN                  = -9999,  /* reserved */
    OT_ERR_AAC_OUT_OF_MEMORY                 = 2,      /* Heap returned NULL pointer. Output buffer is invalid. */
    OT_ERR_AAC_UNKNOWN                       = 5,      /* Error condition is of unknown reason,
                                                          or from a another module. Output buffer is invalid. */
    OT_ERR_AAC_TRANSPORT_SYNC_ERROR          = 4097,   /* The transport decoder had syncronisation problems.
                                                          Do not exit decoding. Just feed new bitstream data. */
    OT_ERR_AAC_NOT_ENOUGH_BITS               = 4098,   /* The input buffer ran out of bits. */
    OT_ERR_AAC_TRANSPORT_FATAL_ERROR         = 4099,   /* The transport decoder occut fatal error. Reset Transport */
    OT_ERR_AAC_INVALID_HANDLE                = 8193,   /* The handle passed to the function call was invalid (NULL). */
    OT_ERR_AAC_UNSUPPORTED_AOT               = 8194,   /* The AOT found in the configuration is not supported. */
    OT_ERR_AAC_UNSUPPORTED_FORMAT            = 8195,   /* The bitstream format is not supported.  */
    OT_ERR_AAC_UNSUPPORTED_ER_FORMAT         = 8196,   /* The error resilience tool format is not supported. */
    OT_ERR_AAC_UNSUPPORTED_EPCONFIG          = 8197,   /* The error protection format is not supported. */
    OT_ERR_AAC_UNSUPPORTED_MULTILAYER        = 8198,   /* More than one layer for AAC scalable is not supported. */
    OT_ERR_AAC_UNSUPPORTED_CHANNELCONFIG     = 8199,   /* The channel configuration (either number or arrangement)
                                                          is not supported. */
    OT_ERR_AAC_UNSUPPORTED_SAMPLINGRATE      = 8200,   /* The sample rate specified in the configuration
                                                          is not supported. */
    OT_ERR_AAC_INVALID_SBR_CONFIG            = 8201,   /* The SBR configuration is not supported. */
    OT_ERR_AAC_SET_PARAM_FAIL                = 8202,   /* The parameter could not be set. Either the value was out
                                                          of range or the parameter does not exist. */
    OT_ERR_AAC_NEED_TO_RESTART               = 8203,   /* The decoder needs to be restarted, since the
                                                          required configuration change cannot be performed. */
    OT_ERR_AAC_TRANSPORT_ERROR               = 16385,  /* The transport decoder encountered an unexpected error. */
    OT_ERR_AAC_PARSE_ERROR                   = 16386,  /* Error while parsing the bitstream.
                                                          Most probably it is corrupted, or the system crashed. */
    OT_ERR_AAC_UNSUPPORTED_EXTENSION_PAYLOAD = 16387,  /* Error while parsing the extension payload of the bitstream.
                                                          The extension payload type found is not supported. */
    OT_ERR_AAC_DECODE_FRAME_ERROR            = 16388,  /* The parsed bitstream value is out of range. Most probably
                                                          the bitstream is corrupt, or the system crashed. */
    OT_ERR_AAC_CRC_ERROR                     = 16389,  /* The embedded CRC did not match. */
    OT_ERR_AAC_INVALID_CODE_BOOK             = 16390,  /* An invalid codebook was signalled. Most probably the
                                                          bitstream is corrupt, or the system crashed. */
    OT_ERR_AAC_UNSUPPORTED_PREDICTION        = 16391,  /* Predictor found, but not supported in the AAC Low
                                                          Complexity profile. Most probably the bitstream is corrupt,
                                                          or has a wrong format. */
    OT_ERR_AAC_UNSUPPORTED_CCE               = 16392,  /* A CCE element was found which is not supported. Most
                                                          probably the bitstream is corrupt, or has a wrong format. */
    OT_ERR_AAC_UNSUPPORTED_LFE               = 16393,  /* A LFE element was found which is not supported. Most
                                                          probably the bitstream is corrupt, or has a wrong format. */
    OT_ERR_AAC_UNSUPPORTED_GAIN_CONTROL_DATA = 16394,  /* Gain control data found but not supported. Most probably
                                                          the bitstream is corrupt, or has a wrong format. */
    OT_ERR_AAC_UNSUPPORTED_SBA               = 16395,  /* SBA found, but currently not supported
                                                          in the BSAC profile. */
    OT_ERR_AAC_TNS_READ_ERROR                = 16396,  /* Error while reading TNS data. Most probably the bitstream
                                                          is corrupt or the system crashed. */
    OT_ERR_AAC_RVLC_ERROR                    = 16397,  /* Error while decoding error resillient data. */
    OT_ERR_AAC_ANC_DATA_ERROR                = 32769,  /* Non severe error concerning the ancillary data handling. */
    OT_ERR_AAC_TOO_SMALL_ANC_BUFFER          = 32770,  /* The registered ancillary data buffer is too small to
                                                          receive the parsed data. */
    OT_ERR_AAC_TOO_MANY_ANC_ELEMENTS         = 32771,  /* More than the allowed number of ancillary data elements
                                                          should be written to buffer. */
} ot_aacdec_err;

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
} ot_aacdec_frame_info;

typedef enum {
    OT_AACDEC_ADTS = 0,
    OT_AACDEC_LOAS = 1,
    OT_AACDEC_LATM_MCP1 = 2,
} ot_aacdec_transport_type;

typedef void *ot_aac_decoder;

typedef struct {
    td_u8 version[OT_AACDEC_VERSION_MAX_BYTE];
} ot_aacdec_version;

td_s32 ot_aacdec_get_version(ot_aacdec_version *version);

ot_aac_decoder ot_aacdec_init_decoder(ot_aacdec_transport_type transport_type);

td_void ot_aacdec_free_decoder(ot_aac_decoder aac_decoder);

td_s32 ot_aacdec_set_raw_mode(ot_aac_decoder aac_decoder, td_s32 chn_num, td_s32 sample_rate);

td_s32 ot_aacdec_find_sync_header(ot_aac_decoder aac_decoder, td_u8 **in_buf, td_s32 *bytes_left);

td_s32 ot_aacdec_frame(ot_aac_decoder aac_decoder, td_u8 **in_buf, td_s32 *bytes_left, td_s16 *out_pcm);

td_s32 ot_aacdec_set_eos_flag(ot_aac_decoder aac_decoder, td_s32 eos_flag);

td_s32 ot_aacdec_get_last_frame_info(ot_aac_decoder aac_decoder, ot_aacdec_frame_info *frame_info);

td_s32 ot_aacdec_flush_codec(ot_aac_decoder aac_decoder);

td_s32 ot_aacdec_register_mod(const td_void *mod_handle);

td_void *ot_aac_sbrdec_get_handle(td_void);

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cpluscplus */
#endif /* __cpluscplus */

#endif /* OT_AACDEC_H */
