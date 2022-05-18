/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: hdmi intf.
 * Author: Hisilicon multimedia software group
 * Create: 2012/12/22
 */
#ifndef __HI_COMMON_HDMI_H__
#define __HI_COMMON_HDMI_H__

#include "hi_type.h"
#include "hi_errno.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define HI_HDMI_MAX_AUDIO_CAPBILITY_CNT  16
#define HI_HDMI_MAX_SAMPLE_RATE_NUM      8
#define HI_HDMI_MAX_BIT_DEPTH_NUM        6
#define HI_HDMI_DETAIL_TIMING_MAX        10
#define HI_HDMI_EDID_RAW_DATA_LEN        512
#define HI_HDMI_VENDOR_NAME_LEN          8
#define HI_HDMI_HW_PARAM_NUM             4
#define HI_HDMI_BKSV_LEN                 5
#define HI_HDMI_MANUFACTURE_NAME_LEN     4
/* user data len:31 - 4(pkt header) - 5(4K- the length must be 0x05)  */
#define HI_HDMI_VENDOR_USER_DATA_MAX_LEN 22

typedef enum {
    HI_ERRNO_HDMI_NOT_INIT = 1,
    HI_ERRNO_HDMI_INVALID_PARAM,
    HI_ERRNO_HDMI_NULL_PTR,
    HI_ERRNO_HDMI_DEV_NOT_OPEN,
    HI_ERRNO_HDMI_DEV_NOT_CONNECT,
    HI_ERRNO_HDMI_READ_SINK_FAILED,
    HI_ERRNO_HDMI_INIT_ALREADY,
    HI_ERRNO_HDMI_CALLBACK_ALREADY,
    HI_ERRNO_HDMI_INVALID_CALLBACK,
    HI_ERRNO_HDMI_FEATURE_NO_SUPPORT,
    HI_ERRNO_HDMI_BUS_BUSY,
    HI_ERRNO_HDMI_READ_EVENT_FAILED,
    HI_ERRNO_HDMI_NOT_START,
    HI_ERRNO_HDMI_READ_EDID_FAILED,
    HI_ERRNO_HDMI_INIT_FAILED,
    HI_ERRNO_HDMI_CREATE_TESK_FAILED,
    HI_ERRNO_HDMI_MALLOC_FAILED,
    HI_ERRNO_HDMI_FREE_FAILED,
    HI_ERRNO_HDMI_PTHREAD_CREATE_FAILED,
    HI_ERRNO_HDMI_PTHREAD_JOIN_FAILED,
    HI_ERRNO_HDMI_STRATEGY_FAILED,
    HI_ERRNO_HDMI_SET_ATTR_FAILED,
    HI_ERRNO_HDMI_CALLBACK_NOT_REGISTER,
    HI_ERRNO_HDMI_UNKNOWN_CMD,
    HI_ERRNO_HDMI_MUTEX_LOCK_FAILED,
    ERR_HDMI_BUTT
} hi_hdmi_errno;

typedef enum {
    HI_HDMI_AUDIO_CHN_CNT_STREAM,
    HI_HDMI_AUDIO_CHN_CNT_2,
    HI_HDMI_AUDIO_CHN_CNT_3,
    HI_HDMI_AUDIO_CHN_CNT_4,
    HI_HDMI_AUDIO_CHN_CNT_5,
    HI_HDMI_AUDIO_CHN_CNT_6,
    HI_HDMI_AUDIO_CHN_CNT_7,
    HI_HDMI_AUDIO_CHN_CNT_8,
    HI_HDMI_AUDIO_CHN_CNT_BUTT
} hi_hdmi_audio_chn_cnt;

typedef enum {
    HI_HDMI_AUDIO_SAMPLE_SIZE_STREAM,
    HI_HDMI_AUDIO_SAMPLE_SIZE_16,
    HI_HDMI_AUDIO_SAMPLE_SIZE_20,
    HI_HDMI_AUDIO_SAMPLE_SIZE_24,
    HI_HDMI_AUDIO_SAMPLE_SIZE_BUTT
} hi_hdmi_audio_sample_size;

typedef enum {
    HI_HDMI_AUDIO_SAMPLE_FREQ_STREAM,
    HI_HDMI_AUDIO_SAMPLE_FREQ_32000,
    HI_HDMI_AUDIO_SAMPLE_FREQ_44100,
    HI_HDMI_AUDIO_SAMPLE_FREQ_48000,
    HI_HDMI_AUDIO_SAMPLE_FREQ_88200,
    HI_HDMI_AUDIO_SAMPLE_FREQ_96000,
    HI_HDMI_AUDIO_SAMPLE_FREQ_176400,
    HI_HDMI_AUDIO_SAMPLE_FREQ_192000,
    HI_HDMI_AUDIO_SAMPLE_FREQ_BUTT
} hi_hdmi_audio_sample_freq;

typedef enum {
    HI_HDMI_LEVEL_SHIFT_VAL_0_DB,
    HI_HDMI_LEVEL_SHIFT_VAL_1_DB,
    HI_HDMI_LEVEL_SHIFT_VAL_2_DB,
    HI_HDMI_LEVEL_SHIFT_VAL_3_DB,
    HI_HDMI_LEVEL_SHIFT_VAL_4_DB,
    HI_HDMI_LEVEL_SHIFT_VAL_5_DB,
    HI_HDMI_LEVEL_SHIFT_VAL_6_DB,
    HI_HDMI_LEVEL_SHIFT_VAL_7_DB,
    HI_HDMI_LEVEL_SHIFT_VAL_8_DB,
    HI_HDMI_LEVEL_SHIFT_VAL_9_DB,
    HI_HDMI_LEVEL_SHIFT_VAL_10_DB,
    HI_HDMI_LEVEL_SHIFT_VAL_11_DB,
    HI_HDMI_LEVEL_SHIFT_VAL_12_DB,
    HI_HDMI_LEVEL_SHIFT_VAL_13_DB,
    HI_HDMI_LEVEL_SHIFT_VAL_14_DB,
    HI_HDMI_LEVEL_SHIFT_VAL_15_DB,
    HI_HDMI_LEVEL_SHIFT_VAL_BUTT
} hi_hdmi_level_shift_val;

typedef enum {
    HI_HDMI_LFE_PLAYBACK_NO,
    HI_HDMI_LFE_PLAYBACK_0_DB,
    HI_HDMI_LFE_PLAYBACK_10_DB,
    HI_HDMI_LFE_PLAYBACK_BUTT
} hi_hdmi_lfe_playback_level;

typedef enum {
    HI_INFOFRAME_TYPE_AVI,
    HI_INFOFRAME_TYPE_AUDIO,
    HI_INFOFRAME_TYPE_VENDORSPEC,
    HI_INFOFRAME_TYPE_BUTT
} hi_hdmi_infoframe_type;

typedef enum {
    HI_HDMI_VIDEO_FORMAT_1080P_60,
    HI_HDMI_VIDEO_FORMAT_1080P_50,
    HI_HDMI_VIDEO_FORMAT_1080P_30,
    HI_HDMI_VIDEO_FORMAT_1080P_25,
    HI_HDMI_VIDEO_FORMAT_1080P_24,
    HI_HDMI_VIDEO_FORMAT_1080i_60,
    HI_HDMI_VIDEO_FORMAT_1080i_50,
    HI_HDMI_VIDEO_FORMAT_720P_60,
    HI_HDMI_VIDEO_FORMAT_720P_50,
    HI_HDMI_VIDEO_FORMAT_576P_50,
    HI_HDMI_VIDEO_FORMAT_480P_60,
    HI_HDMI_VIDEO_FORMAT_PAL,
    HI_HDMI_VIDEO_FORMAT_NTSC,
    HI_HDMI_VIDEO_FORMAT_861D_640X480_60,
    HI_HDMI_VIDEO_FORMAT_VESA_800X600_60,
    HI_HDMI_VIDEO_FORMAT_VESA_1024X768_60,
    HI_HDMI_VIDEO_FORMAT_VESA_1280X800_60,
    HI_HDMI_VIDEO_FORMAT_VESA_1280X1024_60,
    HI_HDMI_VIDEO_FORMAT_VESA_1366X768_60,
    HI_HDMI_VIDEO_FORMAT_VESA_1440X900_60,
    HI_HDMI_VIDEO_FORMAT_VESA_1400X1050_60,
    HI_HDMI_VIDEO_FORMAT_VESA_1600X1200_60,
    HI_HDMI_VIDEO_FORMAT_VESA_1680X1050_60,
    HI_HDMI_VIDEO_FORMAT_VESA_1920X1200_60,
    HI_HDMI_VIDEO_FORMAT_2560x1440_30,
    HI_HDMI_VIDEO_FORMAT_2560x1440_60,
    HI_HDMI_VIDEO_FORMAT_2560x1600_60,
    HI_HDMI_VIDEO_FORMAT_1920x2160_30,
    HI_HDMI_VIDEO_FORMAT_3840X2160P_24,
    HI_HDMI_VIDEO_FORMAT_3840X2160P_25,
    HI_HDMI_VIDEO_FORMAT_3840X2160P_30,
    HI_HDMI_VIDEO_FORMAT_3840X2160P_50,
    HI_HDMI_VIDEO_FORMAT_3840X2160P_60,
    HI_HDMI_VIDEO_FORMAT_4096X2160P_24,
    HI_HDMI_VIDEO_FORMAT_4096X2160P_25,
    HI_HDMI_VIDEO_FORMAT_4096X2160P_30,
    HI_HDMI_VIDEO_FORMAT_4096X2160P_50,
    HI_HDMI_VIDEO_FORMAT_4096X2160P_60,
    HI_HDMI_VIDEO_FORMAT_3840X2160P_120,
    HI_HDMI_VIDEO_FORMAT_4096X2160P_120,
    HI_HDMI_VIDEO_FORMAT_7680X4320P_30,
    HI_HDMI_VIDEO_FORMAT_VESA_CUSTOMER_DEFINE,
    HI_HDMI_VIDEO_FORMAT_BUTT
} hi_hdmi_video_format;

typedef enum {
    HI_HDMI_PIC_ASPECT_RATIO_NO_DATA,
    HI_HDMI_PIC_ASPECT_RATIO_4TO3,
    HI_HDMI_PIC_ASPECT_RATIO_16TO9,
    HI_HDMI_PIC_ASPECT_RATIO_64TO27,
    HI_HDMI_PIC_ASPECT_RATIO_256TO135,
    HI_HDMI_PIC_ASPECT_RATIO_BUTT
} hi_pic_aspect_ratio;

typedef enum {
    HI_HDMI_ACTIVE_ASPECT_RATIO_16TO9_TOP = 2,
    HI_HDMI_ACTIVE_ASPECT_RATIO_14TO9_TOP,
    HI_HDMI_ACTIVE_ASPECT_RATIO_16TO9_BOX_CENTER,
    HI_HDMI_ACTIVE_ASPECT_RATIO_SAME_PIC = 8,
    HI_HDMI_ACTIVE_ASPECT_RATIO_4TO3_CENTER,
    HI_HDMI_ACTIVE_ASPECT_RATIO_16TO9_CENTER,
    HI_HDMI_ACTIVE_ASPECT_RATIO_14TO9_CENTER,
    HI_HDMI_ACTIVE_ASPECT_RATIO_4TO3_14_9 = 13,
    HI_HDMI_ACTIVE_ASPECT_RATIO_16TO9_14_9,
    HI_HDMI_ACTIVE_ASPECT_RATIO_16TO9_4_3,
    HI_HDMI_ACTIVE_ASPECT_RATIO_BUTT
} hi_hdmi_active_aspect_ratio;

typedef enum {
    HI_HDMI_SAMPLE_RATE_UNKNOWN, /* unknown sample rate */
    HI_HDMI_SAMPLE_RATE_8K,      /* 8K sample rate */
    HI_HDMI_SAMPLE_RATE_11K,     /* 11.025K sample rate */
    HI_HDMI_SAMPLE_RATE_12K,     /* 12K sample rate */
    HI_HDMI_SAMPLE_RATE_16K,     /* 16K sample rate */
    HI_HDMI_SAMPLE_RATE_22K,     /* 22.050K sample rate */
    HI_HDMI_SAMPLE_RATE_24K,     /* 24K sample rate */
    HI_HDMI_SAMPLE_RATE_32K,     /* 32K sample rate */
    HI_HDMI_SAMPLE_RATE_44K,     /* 44.1K sample rate */
    HI_HDMI_SAMPLE_RATE_48K,     /* 48K sample rate */
    HI_HDMI_SAMPLE_RATE_88K,     /* 88.2K sample rate */
    HI_HDMI_SAMPLE_RATE_96K,     /* 96K sample rate */
    HI_HDMI_SAMPLE_RATE_176K,    /* 176K sample rate */
    HI_HDMI_SAMPLE_RATE_192K,    /* 192K sample rate */
    HI_HDMI_SAMPLE_RATE_768K,    /* 768K sample rate */
    HI_HDMI_SAMPLE_RATE_BUTT
} hi_hdmi_sample_rate;

typedef enum {
    HI_HDMI_AUDIO_FORMAT_CODE_RESERVED, /* Audio coding type, refer stream,default type */
    HI_HDMI_AUDIO_FORMAT_CODE_PCM,      /* Audio coding PCM type */
    HI_HDMI_AUDIO_FORMAT_CODE_AC3,      /* Audio coding AC3 type */
    HI_HDMI_AUDIO_FORMAT_CODE_MPEG1,    /* Audio coding MPEG1 type */
    HI_HDMI_AUDIO_FORMAT_CODE_MP3,      /* Audio coding MP3 type */
    HI_HDMI_AUDIO_FORMAT_CODE_MPEG2,    /* Audio coding MPEG2 type */
    HI_HDMI_AUDIO_FORMAT_CODE_AAC,      /* Audio coding AAC type */
    HI_HDMI_AUDIO_FORMAT_CODE_DTS,      /* Audio coding DTS type */
    HI_HDMI_AUDIO_FORMAT_CODE_ATRAC,    /* Audio coding ATRAC type */
    HI_HDMI_AUDIO_FORMAT_CODE_ONE_BIT,  /* Audio coding ONE BIT AUDIO type */
    HI_HDMI_AUDIO_FORMAT_CODE_DDP,      /* Audio coding DDPLUS type */
    HI_HDMI_AUDIO_FORMAT_CODE_DTS_HD,   /* Audio coding DTS HD type */
    HI_HDMI_AUDIO_FORMAT_CODE_MAT,      /* Audio coding MAT type */
    HI_HDMI_AUDIO_FORMAT_CODE_DST,      /* Audio coding DST type */
    HI_HDMI_AUDIO_FORMAT_CODE_WMA_PRO,  /* Audio coding WMA PRO type */
    HI_HDMI_AUDIO_FORMAT_CODE_BUTT
} hi_hdmi_audio_format_code;

typedef enum {
    HI_HDMI_BIT_DEPTH_UNKNOWN, /* unknown bit width */
    HI_HDMI_BIT_DEPTH_8,       /* 8 bits width */
    HI_HDMI_BIT_DEPTH_16,      /* 16 bits width */
    HI_HDMI_BIT_DEPTH_18,      /* 18 bits width */
    HI_HDMI_BIT_DEPTH_20,      /* 20 bits width */
    HI_HDMI_BIT_DEPTH_24,      /* 24 bits width */
    HI_HDMI_BIT_DEPTH_32,      /* 32 bits width */
    HI_HDMI_BIT_DEPTH_BUTT
} hi_hdmi_bit_depth;

typedef enum {
    HI_HDMI_ID_0,
    HI_HDMI_ID_BUTT
} hi_hdmi_id;

typedef enum {
    HI_HDMI_EVENT_HOTPLUG = 0x10, /* HDMI hot-plug event */
    HI_HDMI_EVENT_NO_PLUG,        /* HDMI cable disconnection event */
    HI_HDMI_EVENT_EDID_FAIL,      /* HDMI EDID read failure event */
    HI_HDMI_EVENT_BUTT
} hi_hdmi_event_type;

typedef enum {
    HI_HDMI_VIDEO_MODE_RGB444,
    HI_HDMI_VIDEO_MODE_YCBCR422,
    HI_HDMI_VIDEO_MODE_YCBCR444,
    HI_HDMI_VIDEO_MODE_YCBCR420,
    HI_HDMI_VIDEO_MODE_BUTT
} hi_hdmi_video_mode;

/* Color Component Sample format and chroma sampling format enum,see EIA-CEA-861-D/F */
typedef enum {
    HI_HDMI_COLOR_SPACE_RGB444,
    HI_HDMI_COLOR_SPACE_YCBCR422,
    HI_HDMI_COLOR_SPACE_YCBCR444,
    /* following is new featrue of CEA-861-F */
    HI_HDMI_COLOR_SPACE_YCBCR420,
    HI_HDMI_COLOR_SPACE_BUTT
} hi_hdmi_color_space;

typedef enum {
    HI_HDMI_DEEP_COLOR_24BIT, /* HDMI Deep Color 24bit mode */
    HI_HDMI_DEEP_COLOR_30BIT, /* HDMI Deep Color 30bit mode */
    HI_HDMI_DEEP_COLOR_36BIT, /* HDMI Deep Color 36bit mode */
    HI_HDMI_DEEP_COLOR_BUTT
} hi_hdmi_deep_color;

typedef enum {
    HI_HDMI_BAR_INFO_NOT_VALID, /* Bar Data not valid */
    HI_HDMI_BAR_INFO_V,         /* Vertical bar data valid */
    HI_HDMI_BAR_INFO_H,         /* Horizental bar data valid */
    HI_HDMI_BAR_INFO_VH,        /* Horizental and Vertical bar data valid */
    HI_HDMI_BAR_INFO_BUTT
} hi_hdmi_bar_info;

typedef enum {
    HI_HDMI_SCAN_INFO_NO_DATA,      /* No Scan information */
    HI_HDMI_SCAN_INFO_OVERSCANNED,  /* Scan information, Overscanned (for television) */
    HI_HDMI_SCAN_INFO_UNDERSCANNED, /* Scan information, Underscanned (for computer) */
    HI_HDMI_SCAN_INFO_BUTT
} hi_hdmi_scan_info;

typedef enum {
    HI_HDMI_PIC_NON_UNIFORM_SCALING, /* No Known, non-uniform picture scaling */
    HI_HDMI_PIC_SCALING_H,           /* Picture has been scaled horizentally */
    HI_HDMI_PIC_SCALING_V,           /* Picture has been scaled Vertically */
    HI_HDMI_PIC_SCALING_HV,          /* Picture has been scaled Horizentally and Vertically */
    HI_HDMI_PIC_SCALING_BUTT
} hi_hdmi_pic_scaline;

typedef enum {
    HI_HDMI_COMMON_COLORIMETRY_NO_DATA, /* Colorimetry No Data option */
    HI_HDMI_COMMON_COLORIMETRY_ITU601,  /* Colorimetry ITU601 option */
    HI_HDMI_COMMON_COLORIMETRY_ITU709,  /* Colorimetry ITU709 option */
    HI_HDMI_COMMON_COLORIMETRY_BUTT     /* Colorimetry extended option */
} hi_hdmi_colorimetry;

typedef enum {
    HI_HDMI_COMMON_COLORIMETRY_XVYCC_601,               /* Colorimetry xvYCC601 extened option */
    HI_HDMI_COMMON_COLORIMETRY_XVYCC_709,               /* Colorimetry xvYCC709 extened option */
    HI_HDMI_COMMON_COLORIMETRY_S_YCC_601,               /* Colorimetry S YCC 601 extened option */
    HI_HDMI_COMMON_COLORIMETRY_ADOBE_YCC_601,           /* Colorimetry ADOBE YCC 601 extened option */
    HI_HDMI_COMMON_COLORIMETRY_ADOBE_RGB,               /* Colorimetry ADOBE RGB extened option */
    HI_HDMI_COMMON_COLORIMETRY_2020_CONST_LUMINOUS,     /* Colorimetry ITU2020 extened option */
    HI_HDMI_COMMON_COLORIMETRY_2020_NON_CONST_LUMINOUS, /* Colorimetry ITU2020 extened option */
    HI_HDMI_COMMON_COLORIMETRY_EXT_BUTT
} hi_hdmi_ex_colorimetry;

typedef enum {
    HI_HDMI_RGB_QUANT_DEFAULT_RANGE, /* Defaulr range, it depends on the video format */
    HI_HDMI_RGB_QUANT_LIMITED_RANGE, /* Limited quantization range of 220 levels when receiving a CE video format */
    HI_HDMI_RGB_QUANT_FULL_RANGE,    /* Full quantization range of 256 levels when receiving an IT video format */
    HI_HDMI_RGB_QUANT_FULL_BUTT
} hi_hdmi_rgb_quant_range;

typedef enum {
    HI_HDMI_YCC_QUANT_LIMITED_RANGE, /* Limited quantization range of 220 levels when receiving a CE video format */
    HI_HDMI_YCC_QUANT_FULL_RANGE,    /* Full quantization range of 256 levels when receiving an IT video format */
    HI_HDMI_YCC_QUANT_BUTT
} hi_hdmi_ycc_quant_range;

typedef enum {
    HI_HDMI_PIXEL_REPET_NO,
    HI_HDMI_PIXEL_REPET_2_TIMES,
    HI_HDMI_PIXEL_REPET_3_TIMES,
    HI_HDMI_PIXEL_REPET_4_TIMES,
    HI_HDMI_PIXEL_REPET_5_TIMES,
    HI_HDMI_PIXEL_REPET_6_TIMES,
    HI_HDMI_PIXEL_REPET_7_TIMES,
    HI_HDMI_PIXEL_REPET_8_TIMES,
    HI_HDMI_PIXEL_REPET_9_TIMES,
    HI_HDMI_PIXEL_REPET_10_TIMES,
    HI_HDMI_PIXEL_REPET_BUTT
} hi_hdmi_pixel_repetition;

typedef enum {
    HI_HDMI_CONTNET_GRAPHIC,
    HI_HDMI_CONTNET_PHOTO,
    HI_HDMI_CONTNET_CINEMA,
    HI_HDMI_CONTNET_GAME,
    HI_HDMI_CONTNET_BUTT
} hi_hdmi_content_type;

typedef enum {
    HI_HDMI_AUDIO_CODING_REFER_STREAM_HEAD,
    HI_HDMI_AUDIO_CODING_PCM,
    HI_HDMI_AUDIO_CODING_AC3,
    HI_HDMI_AUDIO_CODING_MPEG1,
    HI_HDMI_AUDIO_CODING_MP3,
    HI_HDMI_AUDIO_CODING_MPEG2,
    HI_HDMI_AUDIO_CODING_AACLC,
    HI_HDMI_AUDIO_CODING_DTS,
    HI_HDMI_AUDIO_CODING_ATRAC,
    HI_HDMI_AUDIO_CODIND_ONE_BIT_AUDIO,
    HI_HDMI_AUDIO_CODING_ENAHNCED_AC3,
    HI_HDMI_AUDIO_CODING_DTS_HD,
    HI_HDMI_AUDIO_CODING_MAT,
    HI_HDMI_AUDIO_CODING_DST,
    HI_HDMI_AUDIO_CODING_WMA_PRO,
    HI_HDMI_AUDIO_CODING_BUTT
} hi_hdmi_coding_type;

typedef enum {
    HI_HDMI_AUDIO_SPEAKER_FL_FR,
    HI_HDMI_AUDIO_SPEAKER_LFE,
    HI_HDMI_AUDIO_SPEAKER_FC,
    HI_HDMI_AUDIO_SPEAKER_RL_RR,
    HI_HDMI_AUDIO_SPEAKER_RC,
    HI_HDMI_AUDIO_SPEAKER_FLC_FRC,
    HI_HDMI_AUDIO_SPEAKER_RLC_RRC,
    HI_HDMI_AUDIO_SPEAKER_FLW_FRW,
    HI_HDMI_AUDIO_SPEAKER_FLH_FRH,
    HI_HDMI_AUDIO_SPEAKER_TC,
    HI_HDMI_AUDIO_SPEAKER_FCH,
    HI_HDMI_AUDIO_SPEAKER_BUTT
} hi_hdmi_audio_speaker;

typedef void (*hi_hdmi_callback)(hi_hdmi_event_type event, hi_void *private_data);

typedef struct {
    hi_hdmi_audio_format_code audio_format_code; /* Audio coding type */
    hi_hdmi_sample_rate support_sample_rate[HI_HDMI_MAX_SAMPLE_RATE_NUM]; /* Audio sample rate */
    hi_u8 audio_chn; /* Aud Channel of the coding type */
    /* sample bit depth,when audio format code is HI_UNF_EDID_AUDIO_FORMAT_CODE_PCM */
    hi_hdmi_bit_depth support_bit_depth[HI_HDMI_MAX_BIT_DEPTH_NUM];
    /* sample bit depth Num,when audio format code is HI_UNF_EDID_AUDIO_FORMAT_CODE_PCM */
    hi_u32 support_bit_depth_num;
    /*
     * enter max bit rate,when audio format code is
     * HI_UNF_EDID_AUDIO_FORMAT_CODE_AC3 - HI_UNF_EDID_AUDIO_FORMAT_CODE_ATRAC
     */
    hi_u32 max_bit_rate;
} hi_hdmi_audio_info;

typedef struct {
    hi_bool eotf_sdr;          /* Traditional gamma - SDR Luminance Range. */
    hi_bool eotf_hdr;          /* Traditional gamma - HDR Luminance Range. */
    hi_bool eotf_smpte_st2084; /* SMPTE ST 2084 */
    hi_bool eotf_hlg;          /* Hybrid Log-Gamma(HLG) based on Recommendation ITU-R BT.2100-0 */
    hi_bool eotf_future;       /* Future EOTF */
} hi_hdmi_eotf;

/* EDID Supported Static Metadata Descriptor */
typedef struct {
    hi_bool descriptor_type1; /* Support Static Metadata Type 1 or not. */
} hi_hdmi_hdr_metadata_type;

typedef struct {
    hi_hdmi_eotf eotf; /* EOTF support. */
    hi_hdmi_hdr_metadata_type metadata; /* static metadata descriptor type(ID) support. */
    hi_u8 max_luma_cv; /* Desired Content Max Luminance Data. Real value(in 1cd/m^2) = 50.2^(CV/32) */
    hi_u8 avg_luma_cv; /* Desired Content Max Frame-average Luminance. Real value(in 1cd/m^2) = 50.2^(CV/32) */
    /*
     * Desired Content Min Luminance Data.
     * Real value(in 1cd/m^2) = u8MaxLuminance_CV * (u8MinLuminance_CV/255)^2 / 100
     */
    hi_u8 min_luma_cv;
} hi_hdmi_hdr_capability;

typedef struct {
    hi_u32  vfb;            /* vertical front blank */
    hi_u32  vbb;            /* vertical back blank */
    hi_u32  vact;           /* vertical active area */
    hi_u32  hfb;            /* horizonal front blank */
    hi_u32  hbb;            /* horizonal back blank */
    hi_u32  hact;           /* horizonal active area */
    hi_u32  vpw;            /* vertical sync pluse width */
    hi_u32  hpw;            /* horizonal sync pluse width */
    hi_bool idv;            /* flag of data valid signal is needed flip */
    hi_bool ihs;            /* flag of horizonal sync pluse is needed flip */
    hi_bool ivs;            /* flag of vertical sync pluse is needed flip */
    hi_u32  img_width;      /* image width */
    hi_u32  img_height;     /* image height */
    hi_u32  aspect_ratio_w; /* aspect ratio width */
    hi_u32  aspect_ratio_h; /* aspect ratio height */
    hi_bool interlace;      /* flag of interlace */
    hi_s32  pixel_clk;      /* pixelc clk for this timing, unit:KHz */
} hi_hdmi_timing_info;

typedef struct {
    hi_u32 detail_timing_num;
    hi_hdmi_timing_info detail_timing[HI_HDMI_DETAIL_TIMING_MAX];
} hi_hdmi_detail_timing;

typedef struct {
    hi_bool is_connected; /* Whether the devices are connected. */
    /* Whether the HDMI is supported by the device. If the HDMI is not supported by the device, the device is DVI. */
    hi_bool support_hdmi;
    hi_bool is_sink_power_on; /* Whether the sink device is powered on. */
    hi_hdmi_video_format native_video_format; /* Physical resolution of the display device. */
    /*
     * Video capability set.
     * HI_TRUE: This display format is supported.
     * HI_FALSE: This display format is not supported.
     */
    hi_bool support_video_format[HI_HDMI_VIDEO_FORMAT_BUTT];
    /*
     * Whether the YCBCR display is supported.
     * HI_TRUE: The YCBCR display is supported.
     * HI_FALSE: Only red-green-blue (RGB) is supported.
     */
    hi_bool support_ycbcr;
    /* Whether the xvYCC601 color format is supported. */
    hi_bool support_xvycc601;
    /* Whether the xvYCC709 color format is supported. */
    hi_bool support_xvycc709;
    /* Transfer profile supported by xvYCC601. 1: P0; 2: P1; 4: P2. */
    hi_u8 md_bit;
    /* Num of audio Info */
    hi_u32 audio_info_num;
    /* Audio Info.For details, see Table 37 in EIA-CEA-861-D. */
    hi_hdmi_audio_info audio_info[HI_HDMI_MAX_AUDIO_CAPBILITY_CNT];
    /* Speaker position. For details, see the definition of SpeakerDATABlock in EIA-CEA-861-D. */
    hi_bool speaker[HI_HDMI_AUDIO_SPEAKER_BUTT];
    /* Device vendor flag */
    hi_u8 manufacture_name[HI_HDMI_MANUFACTURE_NAME_LEN];
    /* Device ID. */
    hi_u32 pdt_code;
    /* Device sequence number. */
    hi_u32 serial_num;
    /* Device production data (week). */
    hi_u32 week_of_manufacture;
    /* Set the production data (year). */
    hi_u32 year_of_manufacture;
    /* Device version number */
    hi_u8 version;
    /* Device sub version number */
    hi_u8 revision;
    /* EDID extended block number */
    hi_u8 edid_ex_blk_num;
    /* Valid flag of the consumer electronics control (CEC) physical address */
    hi_bool is_phys_addr_valid;
    /* CEC physical address A */
    hi_u8 phys_addr_a;
    /* CEC physical address B */
    hi_u8 phys_addr_b;
    /* CEC physical address C */
    hi_u8 phys_addr_c;
    /* CEC physical address D */
    hi_u8 phys_addr_d;
    /* Whether to support the DVI dual-link operation. */
    hi_bool support_dvi_dual;
    /* Whether to support the YCBCR 4:4:4 deep-color mode. */
    hi_bool support_deepcolor_ycbcr444;
    /* Whether to support the deep-color 30-bit mode. */
    hi_bool support_deepcolor_30bit;
    /* Whether to support the deep-color 36-bit mode. */
    hi_bool support_deepcolor_36bit;
    /* Whether to support the deep-color 48-bit mode. */
    hi_bool support_deepcolor_48bit;
    /* Whether to support the Supports_AI mode. */
    hi_bool support_ai;
    /* Maximum TMDS clock. */
    hi_u32 max_tmds_clk;
    /* Delay flag bit. */
    hi_bool i_latency_fields_present;
    /* whether Video_Latency and Audio_Latency fields are present */
    hi_bool latency_fields_present;
    /* Special video format */
    hi_bool hdmi_video_present;
    /* Video delay */
    hi_u8 video_latency;
    /* Audio delay */
    hi_u8 audio_latency;
    /* Video delay in interlaced video mode */
    hi_u8 interlaced_video_latency;
    /* Audio delay in interlaced video mode */
    hi_u8 interlaced_audio_latency;
    /* Whether to support the YUV420 deep-color 30-bit mode. */
    hi_bool support_y420_dc_30bit;
    /* Whether to support the YUV420 deep-color 36-bit mode. */
    hi_bool support_y420_dc_36bit;
    /* Whether to support the YUV420 deep-color 48-bit mode. */
    hi_bool support_y420_dc_48bit;
    /* Whether to support HDMI2.0. */
    hi_bool support_hdmi_2_0;
    /* the format support YUV420 */
    hi_bool support_y420_format[HI_HDMI_VIDEO_FORMAT_BUTT];
    /* the format only support YUV420 */
    hi_bool only_support_y420_format[HI_HDMI_VIDEO_FORMAT_BUTT];
    /*
     * flag of RGB Quantization Range selectable.
     * when HI_TRUE,sink expect full/limited range is selectable;or sink expect default range.
     */
    hi_bool ycc_quant_selectable;
    /*
     * flag of YCC Quantization Range selectable.
     * when HI_TRUE,sink expect full/limited range is selectable;or sink expect default range.
     */
    hi_bool rgb_quant_selectable;
    /* HDR support flag.When HDR Static Metadata Data Block is indicated,flag HI_TRUE;or,HI_FALSE. */
    hi_bool support_hdr;
    /* HDR capability */
    hi_hdmi_hdr_capability hdr;
    /* Detailed timing infomation */
    hi_hdmi_detail_timing detailed_timing;
} hi_hdmi_sink_capability;

typedef struct {
    /* Whether to forcibly output the video over the HDMI. */
    hi_bool hdmi_en;
    /* Video norm. This value of the video norm must be consistent with the norm of the video output. */
    hi_hdmi_video_format video_format;
    /* DeepColor output mode.It is HI_HDMI_DEEP_COLOR_24BIT by default. */
    hi_hdmi_deep_color deep_color_mode;
    /* Whether to enable the audio. */
    hi_bool audio_en;
    /* Audio sampling rate. This parameter needs to be consistent with that of the VO. */
    hi_hdmi_sample_rate sample_rate;
    /* Audio bit width. It is 16 by default. This parameter needs to be consistent with that of the VO. */
    hi_hdmi_bit_depth bit_depth;
    /* Whether to enable auth mode. 0: disabled 1: enabled */
    hi_bool auth_mode_en;
    /*
     * Enable flag of deep color mode adapting case of user setting incorrect,
     * default: HI_FALSE.When user have no any adapting strategy,suggestion HI_TRUE
     */
    hi_bool deep_color_adapt_en;
    /*
     * Pixclk of enVideoFmt(unit is kHz).
     * (This param is valid only when enVideoFmt is HI_HDMI_VIDEO_FMT_VESA_CUSTOMER_DEFINE)
     */
    hi_u32 pix_clk;
} hi_hdmi_attr;

typedef struct {
    hi_bool edid_valid;
    hi_u32  edid_len;
    hi_u8   edid[HI_HDMI_EDID_RAW_DATA_LEN];
} hi_hdmi_edid;

typedef struct {
    hi_hdmi_audio_chn_cnt      chn_cnt;
    hi_hdmi_coding_type        coding_type;
    hi_hdmi_audio_sample_size  sample_size;
    hi_hdmi_audio_sample_freq  sampling_freq;
    hi_u8                      chn_alloc; /* Channel/Speaker Allocation.Range [0,255] */
    hi_hdmi_level_shift_val    level_shift;
    hi_hdmi_lfe_playback_level lfe_playback_level;
    hi_bool                    down_mix_inhibit;
} hi_hdmi_audio_infoframe;

typedef struct {
    hi_hdmi_video_format        timing_mode;
    hi_hdmi_color_space         color_space;
    hi_bool                     active_info_present;
    hi_hdmi_bar_info            bar_info;
    hi_hdmi_scan_info           scan_info;
    hi_hdmi_colorimetry         colorimetry;
    hi_hdmi_ex_colorimetry      ex_colorimetry;
    hi_pic_aspect_ratio         aspect_ratio;
    hi_hdmi_active_aspect_ratio active_aspect_ratio;
    hi_hdmi_pic_scaline         pic_scaling;
    hi_hdmi_rgb_quant_range     rgb_quant;
    hi_bool                     is_it_content;
    hi_hdmi_pixel_repetition    pixel_repetition;
    hi_hdmi_content_type        content_type;
    hi_hdmi_ycc_quant_range     ycc_quant;
    hi_u16                      line_n_end_of_top_bar;
    hi_u16                      line_n_start_of_bot_bar;
    hi_u16                      pixel_n_end_of_left_bar;
    hi_u16                      pixel_n_start_of_right_bar;
} hi_hdmi_avi_infoframe;

typedef struct {
    hi_u8 data_len;
    hi_u8 user_data[HI_HDMI_VENDOR_USER_DATA_MAX_LEN];
} hi_hdmi_vendorspec_infoframe;

typedef union {
    hi_hdmi_avi_infoframe avi_infoframe;     /* AUTO:hi_hdmi_infoframe_type:HI_INFOFRAME_TYPE_AVI; */
    hi_hdmi_audio_infoframe audio_infoframe; /* AUTO:hi_hdmi_infoframe_type:HI_INFOFRAME_TYPE_AUDIO; */
    hi_hdmi_vendorspec_infoframe vendor_spec_infoframe; /* AUTO:hi_hdmi_infoframe_type:HI_INFOFRAME_TYPE_VENDORSPEC; */
} hi_hdmi_infoframe_unit;

typedef struct {
    hi_hdmi_infoframe_type infoframe_type; /* InfoFrame type */
    hi_hdmi_infoframe_unit infoframe_unit; /* InfoFrame date */
} hi_hdmi_infoframe;

typedef struct {
    hi_hdmi_callback hdmi_event_callback; /* Event handling callback function */
    hi_void *private_data;                /* Private data of the callback functions and parameters */
} hi_hdmi_callback_func;

typedef struct {
    hi_u32 i_de_main_clk;
    hi_u32 i_de_main_data;
    hi_u32 i_main_clk;
    hi_u32 i_main_data;
    hi_u32 ft_cap_clk;
    hi_u32 ft_cap_data;
} hi_hdmi_hw_param;

typedef struct {
    hi_hdmi_hw_param hw_param[HI_HDMI_HW_PARAM_NUM];
} hi_hdmi_hw_spec;

typedef struct {
    hi_bool is_connected;     /* The Device is connected or disconnected */
    hi_bool is_sink_power_on; /* The sink is PowerOn or not */
} hi_hdmi_status;

typedef enum {
    HI_HDMI_TRACE_LEN_0,
    HI_HDMI_TRACE_LEN_1,
    HI_HDMI_TRACE_LEN_2,
    HI_HDMI_TRACE_LEN_3,
    HI_HDMI_TRACE_LEN_4,
    HI_HDMI_TRACE_LEN_5,
    HI_HDMI_TRACE_LEN_6,
    HI_HDMI_TRACE_LEN_7,
    HI_HDMI_TRACE_LEN_8,
    HI_HDMI_TRACE_DEFAULT,
    HI_HDMI_TRACE_BUTT
} hi_hdmi_trace_len;

typedef struct {
    hi_bool emi_en; /* emi enable/disable */
    hi_hdmi_trace_len trace_len; /* Phy parameters selection. */
} hi_hdmi_mod_param;

#define HI_ERR_HDMI_NOT_INIT              HI_DEFINE_ERR(HI_ID_HDMI, HI_ERR_LEVEL_ERROR, HI_ERRNO_HDMI_NOT_INIT)
#define HI_ERR_HDMI_INVALID_PARA          HI_DEFINE_ERR(HI_ID_HDMI, HI_ERR_LEVEL_ERROR, HI_ERRNO_HDMI_INVALID_PARAM)
#define HI_ERR_HDMI_NULL_PTR              HI_DEFINE_ERR(HI_ID_HDMI, HI_ERR_LEVEL_ERROR, HI_ERRNO_HDMI_NULL_PTR)
#define HI_ERR_HDMI_DEV_NOT_OPEN          HI_DEFINE_ERR(HI_ID_HDMI, HI_ERR_LEVEL_ERROR, HI_ERRNO_HDMI_DEV_NOT_OPEN)
#define HI_ERR_HDMI_DEV_NOT_CONNECT       HI_DEFINE_ERR(HI_ID_HDMI, HI_ERR_LEVEL_ERROR, HI_ERRNO_HDMI_DEV_NOT_CONNECT)
#define HI_ERR_HDMI_READ_SINK_FAILED      HI_DEFINE_ERR(HI_ID_HDMI, HI_ERR_LEVEL_ERROR, HI_ERRNO_HDMI_READ_SINK_FAILED)
#define HI_ERR_HDMI_INIT_ALREADY          HI_DEFINE_ERR(HI_ID_HDMI, HI_ERR_LEVEL_ERROR, HI_ERRNO_HDMI_INIT_ALREADY)
#define HI_ERR_HDMI_CALLBACK_ALREADY      HI_DEFINE_ERR(HI_ID_HDMI, HI_ERR_LEVEL_ERROR, HI_ERRNO_HDMI_CALLBACK_ALREADY)
#define HI_ERR_HDMI_INVALID_CALLBACK      HI_DEFINE_ERR(HI_ID_HDMI, HI_ERR_LEVEL_ERROR, HI_ERRNO_HDMI_INVALID_CALLBACK)
#define HI_ERR_HDMI_FEATURE_NO_SUPPORT \
    HI_DEFINE_ERR(HI_ID_HDMI, HI_ERR_LEVEL_ERROR, HI_ERRNO_HDMI_FEATURE_NO_SUPPORT)
#define HI_ERR_HDMI_BUS_BUSY              HI_DEFINE_ERR(HI_ID_HDMI, HI_ERR_LEVEL_ERROR, HI_ERRNO_HDMI_BUS_BUSY)
#define HI_ERR_HDMI_READ_EVENT_FAILED     HI_DEFINE_ERR(HI_ID_HDMI, HI_ERR_LEVEL_ERROR, HI_ERRNO_HDMI_READ_EVENT_FAILED)
#define HI_ERR_HDMI_NOT_START             HI_DEFINE_ERR(HI_ID_HDMI, HI_ERR_LEVEL_ERROR, HI_ERRNO_HDMI_NOT_START)
#define HI_ERR_HDMI_READ_EDID_FAILED      HI_DEFINE_ERR(HI_ID_HDMI, HI_ERR_LEVEL_ERROR, HI_ERRNO_HDMI_READ_EDID_FAILED)
#define HI_ERR_HDMI_INIT_FAILED           HI_DEFINE_ERR(HI_ID_HDMI, HI_ERR_LEVEL_ERROR, HI_ERRNO_HDMI_INIT_FAILED)
#define HI_ERR_HDMI_CREATE_TESK_FAILED \
    HI_DEFINE_ERR(HI_ID_HDMI, HI_ERR_LEVEL_ERROR, HI_ERRNO_HDMI_CREATE_TESK_FAILED)
#define HI_ERR_HDMI_MALLOC_FAILED         HI_DEFINE_ERR(HI_ID_HDMI, HI_ERR_LEVEL_ERROR, HI_ERRNO_HDMI_MALLOC_FAILED)
#define HI_ERR_HDMI_FREE_FAILED           HI_DEFINE_ERR(HI_ID_HDMI, HI_ERR_LEVEL_ERROR, HI_ERRNO_HDMI_FREE_FAILED)
#define HI_ERR_HDMI_PTHREAD_CREATE_FAILED \
    HI_DEFINE_ERR(HI_ID_HDMI, HI_ERR_LEVEL_ERROR, HI_ERRNO_HDMI_PTHREAD_CREATE_FAILED)
#define HI_ERR_HDMI_PTHREAD_JOIN_FAILED \
    HI_DEFINE_ERR(HI_ID_HDMI, HI_ERR_LEVEL_ERROR, HI_ERRNO_HDMI_PTHREAD_JOIN_FAILED)
#define HI_ERR_HDMI_STRATEGY_FAILED       HI_DEFINE_ERR(HI_ID_HDMI, HI_ERR_LEVEL_ERROR, HI_ERRNO_HDMI_STRATEGY_FAILED)
#define HI_ERR_HDMI_SET_ATTR_FAILED       HI_DEFINE_ERR(HI_ID_HDMI, HI_ERR_LEVEL_ERROR, HI_ERRNO_HDMI_SET_ATTR_FAILED)
#define HI_ERR_HDMI_CALLBACK_NOT_REGISTER \
    HI_DEFINE_ERR(HI_ID_HDMI, HI_ERR_LEVEL_ERROR, HI_ERRNO_HDMI_CALLBACK_NOT_REGISTER)
#define HI_ERR_HDMI_UNKNOWN_COMMAND       HI_DEFINE_ERR(HI_ID_HDMI, HI_ERR_LEVEL_ERROR, HI_ERRNO_HDMI_UNKNOWN_CMD)
#define HI_ERR_HDMI_MUTEX_LOCK_FAILED     HI_DEFINE_ERR(HI_ID_HDMI, HI_ERR_LEVEL_ERROR, HI_ERRNO_HDMI_MUTEX_LOCK_FAILED)

#ifdef __cplusplu
#if __cplusplus
}
#endif
#endif
#endif

