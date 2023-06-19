/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */
#ifndef OT_COMMON_HDMI_H
#define OT_COMMON_HDMI_H

#include "ot_type.h"
#include "ot_errno.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define OT_HDMI_MAX_AUDIO_CAPBILITY_CNT  16
#define OT_HDMI_MAX_SAMPLE_RATE_NUM      8
#define OT_HDMI_MAX_BIT_DEPTH_NUM        6
#define OT_HDMI_DETAIL_TIMING_MAX        10
#define OT_HDMI_EDID_RAW_DATA_LEN        512
#define OT_HDMI_VENDOR_NAME_LEN          8
#define OT_HDMI_HW_PARAM_NUM             4
#define OT_HDMI_BKSV_LEN                 5
#define OT_HDMI_MANUFACTURE_NAME_LEN     4
/* user data len:31 - 4(pkt header) - 5(4K- the length must be 0x05)  */
#define OT_HDMI_VENDOR_USER_DATA_MAX_LEN 22

typedef enum {
    OT_ERRNO_HDMI_NOT_INIT = 1,
    OT_ERRNO_HDMI_INVALID_PARAM,
    OT_ERRNO_HDMI_NULL_PTR,
    OT_ERRNO_HDMI_DEV_NOT_OPEN,
    OT_ERRNO_HDMI_DEV_NOT_CONNECT,
    OT_ERRNO_HDMI_READ_SINK_FAILED,
    OT_ERRNO_HDMI_INIT_ALREADY,
    OT_ERRNO_HDMI_CALLBACK_ALREADY,
    OT_ERRNO_HDMI_INVALID_CALLBACK,
    OT_ERRNO_HDMI_FEATURE_NO_SUPPORT,
    OT_ERRNO_HDMI_BUS_BUSY,
    OT_ERRNO_HDMI_READ_EVENT_FAILED,
    OT_ERRNO_HDMI_NOT_START,
    OT_ERRNO_HDMI_READ_EDID_FAILED,
    OT_ERRNO_HDMI_INIT_FAILED,
    OT_ERRNO_HDMI_CREATE_TESK_FAILED,
    OT_ERRNO_HDMI_MALLOC_FAILED,
    OT_ERRNO_HDMI_FREE_FAILED,
    OT_ERRNO_HDMI_PTHREAD_CREATE_FAILED,
    OT_ERRNO_HDMI_PTHREAD_JOIN_FAILED,
    OT_ERRNO_HDMI_STRATEGY_FAILED,
    OT_ERRNO_HDMI_SET_ATTR_FAILED,
    OT_ERRNO_HDMI_CALLBACK_NOT_REGISTER,
    OT_ERRNO_HDMI_UNKNOWN_CMD,
    OT_ERRNO_HDMI_MUTEX_LOCK_FAILED,
    ERR_HDMI_BUTT
} ot_hdmi_errno;

typedef enum {
    OT_HDMI_AUDIO_CHN_CNT_STREAM,
    OT_HDMI_AUDIO_CHN_CNT_2,
    OT_HDMI_AUDIO_CHN_CNT_3,
    OT_HDMI_AUDIO_CHN_CNT_4,
    OT_HDMI_AUDIO_CHN_CNT_5,
    OT_HDMI_AUDIO_CHN_CNT_6,
    OT_HDMI_AUDIO_CHN_CNT_7,
    OT_HDMI_AUDIO_CHN_CNT_8,
    OT_HDMI_AUDIO_CHN_CNT_BUTT
} ot_hdmi_audio_chn_cnt;

typedef enum {
    OT_HDMI_AUDIO_SAMPLE_SIZE_STREAM,
    OT_HDMI_AUDIO_SAMPLE_SIZE_16,
    OT_HDMI_AUDIO_SAMPLE_SIZE_20,
    OT_HDMI_AUDIO_SAMPLE_SIZE_24,
    OT_HDMI_AUDIO_SAMPLE_SIZE_BUTT
} ot_hdmi_audio_sample_size;

typedef enum {
    OT_HDMI_AUDIO_SAMPLE_FREQ_STREAM,
    OT_HDMI_AUDIO_SAMPLE_FREQ_32000,
    OT_HDMI_AUDIO_SAMPLE_FREQ_44100,
    OT_HDMI_AUDIO_SAMPLE_FREQ_48000,
    OT_HDMI_AUDIO_SAMPLE_FREQ_88200,
    OT_HDMI_AUDIO_SAMPLE_FREQ_96000,
    OT_HDMI_AUDIO_SAMPLE_FREQ_176400,
    OT_HDMI_AUDIO_SAMPLE_FREQ_192000,
    OT_HDMI_AUDIO_SAMPLE_FREQ_BUTT
} ot_hdmi_audio_sample_freq;

typedef enum {
    OT_HDMI_LEVEL_SHIFT_VAL_0_DB,
    OT_HDMI_LEVEL_SHIFT_VAL_1_DB,
    OT_HDMI_LEVEL_SHIFT_VAL_2_DB,
    OT_HDMI_LEVEL_SHIFT_VAL_3_DB,
    OT_HDMI_LEVEL_SHIFT_VAL_4_DB,
    OT_HDMI_LEVEL_SHIFT_VAL_5_DB,
    OT_HDMI_LEVEL_SHIFT_VAL_6_DB,
    OT_HDMI_LEVEL_SHIFT_VAL_7_DB,
    OT_HDMI_LEVEL_SHIFT_VAL_8_DB,
    OT_HDMI_LEVEL_SHIFT_VAL_9_DB,
    OT_HDMI_LEVEL_SHIFT_VAL_10_DB,
    OT_HDMI_LEVEL_SHIFT_VAL_11_DB,
    OT_HDMI_LEVEL_SHIFT_VAL_12_DB,
    OT_HDMI_LEVEL_SHIFT_VAL_13_DB,
    OT_HDMI_LEVEL_SHIFT_VAL_14_DB,
    OT_HDMI_LEVEL_SHIFT_VAL_15_DB,
    OT_HDMI_LEVEL_SHIFT_VAL_BUTT
} ot_hdmi_level_shift_val;

typedef enum {
    OT_HDMI_LFE_PLAYBACK_NO,
    OT_HDMI_LFE_PLAYBACK_0_DB,
    OT_HDMI_LFE_PLAYBACK_10_DB,
    OT_HDMI_LFE_PLAYBACK_BUTT
} ot_hdmi_lfe_playback_level;

typedef enum {
    OT_INFOFRAME_TYPE_AVI,
    OT_INFOFRAME_TYPE_AUDIO,
    OT_INFOFRAME_TYPE_VENDORSPEC,
    OT_INFOFRAME_TYPE_BUTT
} ot_hdmi_infoframe_type;

typedef enum {
    OT_HDMI_VIDEO_FORMAT_1080P_60,
    OT_HDMI_VIDEO_FORMAT_1080P_50,
    OT_HDMI_VIDEO_FORMAT_1080P_30,
    OT_HDMI_VIDEO_FORMAT_1080P_25,
    OT_HDMI_VIDEO_FORMAT_1080P_24,
    OT_HDMI_VIDEO_FORMAT_1080i_60,
    OT_HDMI_VIDEO_FORMAT_1080i_50,
    OT_HDMI_VIDEO_FORMAT_720P_60,
    OT_HDMI_VIDEO_FORMAT_720P_50,
    OT_HDMI_VIDEO_FORMAT_576P_50,
    OT_HDMI_VIDEO_FORMAT_480P_60,
    OT_HDMI_VIDEO_FORMAT_PAL,
    OT_HDMI_VIDEO_FORMAT_NTSC,
    OT_HDMI_VIDEO_FORMAT_861D_640X480_60,
    OT_HDMI_VIDEO_FORMAT_VESA_800X600_60,
    OT_HDMI_VIDEO_FORMAT_VESA_1024X768_60,
    OT_HDMI_VIDEO_FORMAT_VESA_1280X800_60,
    OT_HDMI_VIDEO_FORMAT_VESA_1280X1024_60,
    OT_HDMI_VIDEO_FORMAT_VESA_1366X768_60,
    OT_HDMI_VIDEO_FORMAT_VESA_1440X900_60,
    OT_HDMI_VIDEO_FORMAT_VESA_1400X1050_60,
    OT_HDMI_VIDEO_FORMAT_VESA_1600X1200_60,
    OT_HDMI_VIDEO_FORMAT_VESA_1680X1050_60,
    OT_HDMI_VIDEO_FORMAT_VESA_1920X1200_60,
    OT_HDMI_VIDEO_FORMAT_2560x1440_30,
    OT_HDMI_VIDEO_FORMAT_2560x1440_60,
    OT_HDMI_VIDEO_FORMAT_2560x1600_60,
    OT_HDMI_VIDEO_FORMAT_1920x2160_30,
    OT_HDMI_VIDEO_FORMAT_3840X2160P_24,
    OT_HDMI_VIDEO_FORMAT_3840X2160P_25,
    OT_HDMI_VIDEO_FORMAT_3840X2160P_30,
    OT_HDMI_VIDEO_FORMAT_3840X2160P_50,
    OT_HDMI_VIDEO_FORMAT_3840X2160P_60,
    OT_HDMI_VIDEO_FORMAT_4096X2160P_24,
    OT_HDMI_VIDEO_FORMAT_4096X2160P_25,
    OT_HDMI_VIDEO_FORMAT_4096X2160P_30,
    OT_HDMI_VIDEO_FORMAT_4096X2160P_50,
    OT_HDMI_VIDEO_FORMAT_4096X2160P_60,
    OT_HDMI_VIDEO_FORMAT_3840X2160P_120,
    OT_HDMI_VIDEO_FORMAT_4096X2160P_120,
    OT_HDMI_VIDEO_FORMAT_7680X4320P_30,
    OT_HDMI_VIDEO_FORMAT_VESA_CUSTOMER_DEFINE,
    OT_HDMI_VIDEO_FORMAT_BUTT
} ot_hdmi_video_format;

typedef enum {
    OT_HDMI_PIC_ASPECT_RATIO_NO_DATA,
    OT_HDMI_PIC_ASPECT_RATIO_4TO3,
    OT_HDMI_PIC_ASPECT_RATIO_16TO9,
    OT_HDMI_PIC_ASPECT_RATIO_64TO27,
    OT_HDMI_PIC_ASPECT_RATIO_256TO135,
    OT_HDMI_PIC_ASPECT_RATIO_BUTT
} ot_pic_aspect_ratio;

typedef enum {
    OT_HDMI_ACTIVE_ASPECT_RATIO_16TO9_TOP = 2,
    OT_HDMI_ACTIVE_ASPECT_RATIO_14TO9_TOP,
    OT_HDMI_ACTIVE_ASPECT_RATIO_16TO9_BOX_CENTER,
    OT_HDMI_ACTIVE_ASPECT_RATIO_SAME_PIC = 8,
    OT_HDMI_ACTIVE_ASPECT_RATIO_4TO3_CENTER,
    OT_HDMI_ACTIVE_ASPECT_RATIO_16TO9_CENTER,
    OT_HDMI_ACTIVE_ASPECT_RATIO_14TO9_CENTER,
    OT_HDMI_ACTIVE_ASPECT_RATIO_4TO3_14_9 = 13,
    OT_HDMI_ACTIVE_ASPECT_RATIO_16TO9_14_9,
    OT_HDMI_ACTIVE_ASPECT_RATIO_16TO9_4_3,
    OT_HDMI_ACTIVE_ASPECT_RATIO_BUTT
} ot_hdmi_active_aspect_ratio;

typedef enum {
    OT_HDMI_SAMPLE_RATE_UNKNOWN, /* unknown sample rate */
    OT_HDMI_SAMPLE_RATE_8K,      /* 8K sample rate */
    OT_HDMI_SAMPLE_RATE_11K,     /* 11.025K sample rate */
    OT_HDMI_SAMPLE_RATE_12K,     /* 12K sample rate */
    OT_HDMI_SAMPLE_RATE_16K,     /* 16K sample rate */
    OT_HDMI_SAMPLE_RATE_22K,     /* 22.050K sample rate */
    OT_HDMI_SAMPLE_RATE_24K,     /* 24K sample rate */
    OT_HDMI_SAMPLE_RATE_32K,     /* 32K sample rate */
    OT_HDMI_SAMPLE_RATE_44K,     /* 44.1K sample rate */
    OT_HDMI_SAMPLE_RATE_48K,     /* 48K sample rate */
    OT_HDMI_SAMPLE_RATE_88K,     /* 88.2K sample rate */
    OT_HDMI_SAMPLE_RATE_96K,     /* 96K sample rate */
    OT_HDMI_SAMPLE_RATE_176K,    /* 176K sample rate */
    OT_HDMI_SAMPLE_RATE_192K,    /* 192K sample rate */
    OT_HDMI_SAMPLE_RATE_768K,    /* 768K sample rate */
    OT_HDMI_SAMPLE_RATE_BUTT
} ot_hdmi_sample_rate;

typedef enum {
    OT_HDMI_AUDIO_FORMAT_CODE_RESERVED, /* Audio coding type, refer stream,default type */
    OT_HDMI_AUDIO_FORMAT_CODE_PCM,      /* Audio coding PCM type */
    OT_HDMI_AUDIO_FORMAT_CODE_AC3,      /* Audio coding AC3 type */
    OT_HDMI_AUDIO_FORMAT_CODE_MPEG1,    /* Audio coding MPEG1 type */
    OT_HDMI_AUDIO_FORMAT_CODE_MP3,      /* Audio coding MP3 type */
    OT_HDMI_AUDIO_FORMAT_CODE_MPEG2,    /* Audio coding MPEG2 type */
    OT_HDMI_AUDIO_FORMAT_CODE_AAC,      /* Audio coding AAC type */
    OT_HDMI_AUDIO_FORMAT_CODE_DTS,      /* Audio coding DTS type */
    OT_HDMI_AUDIO_FORMAT_CODE_ATRAC,    /* Audio coding ATRAC type */
    OT_HDMI_AUDIO_FORMAT_CODE_ONE_BIT,  /* Audio coding ONE BIT AUDIO type */
    OT_HDMI_AUDIO_FORMAT_CODE_DDP,      /* Audio coding DDPLUS type */
    OT_HDMI_AUDIO_FORMAT_CODE_DTS_HD,   /* Audio coding DTS HD type */
    OT_HDMI_AUDIO_FORMAT_CODE_MAT,      /* Audio coding MAT type */
    OT_HDMI_AUDIO_FORMAT_CODE_DST,      /* Audio coding DST type */
    OT_HDMI_AUDIO_FORMAT_CODE_WMA_PRO,  /* Audio coding WMA PRO type */
    OT_HDMI_AUDIO_FORMAT_CODE_BUTT
} ot_hdmi_audio_format_code;

typedef enum {
    OT_HDMI_BIT_DEPTH_UNKNOWN, /* unknown bit width */
    OT_HDMI_BIT_DEPTH_8,       /* 8 bits width */
    OT_HDMI_BIT_DEPTH_16,      /* 16 bits width */
    OT_HDMI_BIT_DEPTH_18,      /* 18 bits width */
    OT_HDMI_BIT_DEPTH_20,      /* 20 bits width */
    OT_HDMI_BIT_DEPTH_24,      /* 24 bits width */
    OT_HDMI_BIT_DEPTH_32,      /* 32 bits width */
    OT_HDMI_BIT_DEPTH_BUTT
} ot_hdmi_bit_depth;

typedef enum {
    OT_HDMI_ID_0,
    OT_HDMI_ID_1,
    OT_HDMI_ID_BUTT
} ot_hdmi_id;

typedef enum {
    OT_HDMI_EVENT_HOTPLUG = 0x10, /* HDMI hot-plug event */
    OT_HDMI_EVENT_NO_PLUG,        /* HDMI cable disconnection event */
    OT_HDMI_EVENT_EDID_FAIL,      /* HDMI EDID read failure event */
    OT_HDMI_EVENT_BUTT
} ot_hdmi_event_type;

typedef enum {
    OT_HDMI_VIDEO_MODE_RGB444,
    OT_HDMI_VIDEO_MODE_YCBCR422,
    OT_HDMI_VIDEO_MODE_YCBCR444,
    OT_HDMI_VIDEO_MODE_YCBCR420,
    OT_HDMI_VIDEO_MODE_BUTT
} ot_hdmi_video_mode;

/* Color Component Sample format and chroma sampling format enum,see EIA-CEA-861-D/F */
typedef enum {
    OT_HDMI_COLOR_SPACE_RGB444,
    OT_HDMI_COLOR_SPACE_YCBCR422,
    OT_HDMI_COLOR_SPACE_YCBCR444,
    /* following is new featrue of CEA-861-F */
    OT_HDMI_COLOR_SPACE_YCBCR420,
    OT_HDMI_COLOR_SPACE_BUTT
} ot_hdmi_color_space;

typedef enum {
    OT_HDMI_DEEP_COLOR_24BIT, /* HDMI Deep Color 24bit mode */
    OT_HDMI_DEEP_COLOR_30BIT, /* HDMI Deep Color 30bit mode */
    OT_HDMI_DEEP_COLOR_36BIT, /* HDMI Deep Color 36bit mode */
    OT_HDMI_DEEP_COLOR_BUTT
} ot_hdmi_deep_color;

typedef enum {
    OT_HDMI_BAR_INFO_NOT_VALID, /* Bar Data not valid */
    OT_HDMI_BAR_INFO_V,         /* Vertical bar data valid */
    OT_HDMI_BAR_INFO_H,         /* Horizontal bar data valid */
    OT_HDMI_BAR_INFO_VH,        /* Horizontal and Vertical bar data valid */
    OT_HDMI_BAR_INFO_BUTT
} ot_hdmi_bar_info;

typedef enum {
    OT_HDMI_SCAN_INFO_NO_DATA,      /* No Scan information */
    OT_HDMI_SCAN_INFO_OVERSCANNED,  /* Scan information, Overscanned (for television) */
    OT_HDMI_SCAN_INFO_UNDERSCANNED, /* Scan information, Underscanned (for computer) */
    OT_HDMI_SCAN_INFO_BUTT
} ot_hdmi_scan_info;

typedef enum {
    OT_HDMI_PIC_NON_UNIFORM_SCALING, /* No Known, non-uniform picture scaling */
    OT_HDMI_PIC_SCALING_H,           /* Picture has been scaled horizontally */
    OT_HDMI_PIC_SCALING_V,           /* Picture has been scaled Vertically */
    OT_HDMI_PIC_SCALING_HV,          /* Picture has been scaled horizontally and Vertically */
    OT_HDMI_PIC_SCALING_BUTT
} ot_hdmi_pic_scaline;

typedef enum {
    OT_HDMI_COMMON_COLORIMETRY_NO_DATA, /* Colorimetry No Data option */
    OT_HDMI_COMMON_COLORIMETRY_ITU601,  /* Colorimetry ITU601 option */
    OT_HDMI_COMMON_COLORIMETRY_ITU709,  /* Colorimetry ITU709 option */
    OT_HDMI_COMMON_COLORIMETRY_BUTT     /* Colorimetry extended option */
} ot_hdmi_colorimetry;

typedef enum {
    OT_HDMI_COMMON_COLORIMETRY_XVYCC_601,               /* Colorimetry xvYCC601 extended option */
    OT_HDMI_COMMON_COLORIMETRY_XVYCC_709,               /* Colorimetry xvYCC709 extended option */
    OT_HDMI_COMMON_COLORIMETRY_S_YCC_601,               /* Colorimetry S YCC 601 extended option */
    OT_HDMI_COMMON_COLORIMETRY_ADOBE_YCC_601,           /* Colorimetry ADOBE YCC 601 extended option */
    OT_HDMI_COMMON_COLORIMETRY_ADOBE_RGB,               /* Colorimetry ADOBE RGB extended option */
    OT_HDMI_COMMON_COLORIMETRY_2020_CONST_LUMINOUS,     /* Colorimetry ITU2020 extended option */
    OT_HDMI_COMMON_COLORIMETRY_2020_NON_CONST_LUMINOUS, /* Colorimetry ITU2020 extended option */
    OT_HDMI_COMMON_COLORIMETRY_EXT_BUTT
} ot_hdmi_ex_colorimetry;

typedef enum {
    OT_HDMI_RGB_QUANT_DEFAULT_RANGE, /* Default range, it depends on the video format */
    OT_HDMI_RGB_QUANT_LIMITED_RANGE, /* Limited quantization range of 220 levels when receiving a CE video format */
    OT_HDMI_RGB_QUANT_FULL_RANGE,    /* Full quantization range of 256 levels when receiving an IT video format */
    OT_HDMI_RGB_QUANT_FULL_BUTT
} ot_hdmi_rgb_quant_range;

typedef enum {
    OT_HDMI_YCC_QUANT_LIMITED_RANGE, /* Limited quantization range of 220 levels when receiving a CE video format */
    OT_HDMI_YCC_QUANT_FULL_RANGE,    /* Full quantization range of 256 levels when receiving an IT video format */
    OT_HDMI_YCC_QUANT_BUTT
} ot_hdmi_ycc_quant_range;

typedef enum {
    OT_HDMI_PIXEL_REPET_NO,
    OT_HDMI_PIXEL_REPET_2_TIMES,
    OT_HDMI_PIXEL_REPET_3_TIMES,
    OT_HDMI_PIXEL_REPET_4_TIMES,
    OT_HDMI_PIXEL_REPET_5_TIMES,
    OT_HDMI_PIXEL_REPET_6_TIMES,
    OT_HDMI_PIXEL_REPET_7_TIMES,
    OT_HDMI_PIXEL_REPET_8_TIMES,
    OT_HDMI_PIXEL_REPET_9_TIMES,
    OT_HDMI_PIXEL_REPET_10_TIMES,
    OT_HDMI_PIXEL_REPET_BUTT
} ot_hdmi_pixel_repetition;

typedef enum {
    OT_HDMI_CONTNET_GRAPHIC,
    OT_HDMI_CONTNET_PHOTO,
    OT_HDMI_CONTNET_CINEMA,
    OT_HDMI_CONTNET_GAME,
    OT_HDMI_CONTNET_BUTT
} ot_hdmi_content_type;

typedef enum {
    OT_HDMI_AUDIO_CODING_REFER_STREAM_HEAD,
    OT_HDMI_AUDIO_CODING_PCM,
    OT_HDMI_AUDIO_CODING_AC3,
    OT_HDMI_AUDIO_CODING_MPEG1,
    OT_HDMI_AUDIO_CODING_MP3,
    OT_HDMI_AUDIO_CODING_MPEG2,
    OT_HDMI_AUDIO_CODING_AACLC,
    OT_HDMI_AUDIO_CODING_DTS,
    OT_HDMI_AUDIO_CODING_ATRAC,
    OT_HDMI_AUDIO_CODIND_ONE_BIT_AUDIO,
    OT_HDMI_AUDIO_CODING_ENAHNCED_AC3,
    OT_HDMI_AUDIO_CODING_DTS_HD,
    OT_HDMI_AUDIO_CODING_MAT,
    OT_HDMI_AUDIO_CODING_DST,
    OT_HDMI_AUDIO_CODING_WMA_PRO,
    OT_HDMI_AUDIO_CODING_BUTT
} ot_hdmi_coding_type;

typedef enum {
    OT_HDMI_AUDIO_SPEAKER_FL_FR,
    OT_HDMI_AUDIO_SPEAKER_LFE,
    OT_HDMI_AUDIO_SPEAKER_FC,
    OT_HDMI_AUDIO_SPEAKER_RL_RR,
    OT_HDMI_AUDIO_SPEAKER_RC,
    OT_HDMI_AUDIO_SPEAKER_FLC_FRC,
    OT_HDMI_AUDIO_SPEAKER_RLC_RRC,
    OT_HDMI_AUDIO_SPEAKER_FLW_FRW,
    OT_HDMI_AUDIO_SPEAKER_FLH_FRH,
    OT_HDMI_AUDIO_SPEAKER_TC,
    OT_HDMI_AUDIO_SPEAKER_FCH,
    OT_HDMI_AUDIO_SPEAKER_BUTT
} ot_hdmi_audio_speaker;

typedef void (*ot_hdmi_callback)(ot_hdmi_event_type event, td_void *private_data);

typedef struct {
    ot_hdmi_audio_format_code audio_format_code; /* Audio coding type */
    ot_hdmi_sample_rate support_sample_rate[OT_HDMI_MAX_SAMPLE_RATE_NUM]; /* Audio sample rate */
    td_u8 audio_chn; /* Aud Channel of the coding type */
    /* sample bit depth,when audio format code is OT_UNF_EDID_AUDIO_FORMAT_CODE_PCM */
    ot_hdmi_bit_depth support_bit_depth[OT_HDMI_MAX_BIT_DEPTH_NUM];
    /* sample bit depth Num,when audio format code is OT_UNF_EDID_AUDIO_FORMAT_CODE_PCM */
    td_u32 support_bit_depth_num;
    /*
     * enter max bit rate,when audio format code is
     * OT_UNF_EDID_AUDIO_FORMAT_CODE_AC3 - OT_UNF_EDID_AUDIO_FORMAT_CODE_ATRAC
     */
    td_u32 max_bit_rate;
} ot_hdmi_audio_info;

typedef struct {
    td_bool eotf_sdr;          /* Traditional gamma - SDR Luminance Range. */
    td_bool eotf_hdr;          /* Traditional gamma - HDR Luminance Range. */
    td_bool eotf_smpte_st2084; /* SMPTE ST 2084 */
    td_bool eotf_hlg;          /* Hybrid Log-Gamma(HLG) based on Recommendation ITU-R BT.2100-0 */
    td_bool eotf_future;       /* Future EOTF */
} ot_hdmi_eotf;

/* EDID Supported Static Metadata Descriptor */
typedef struct {
    td_bool descriptor_type1; /* Support Static Metadata Type 1 or not. */
} ot_hdmi_hdr_metadata_type;

typedef struct {
    ot_hdmi_eotf eotf; /* EOTF support. */
    ot_hdmi_hdr_metadata_type metadata; /* static metadata descriptor type(ID) support. */
    td_u8 max_luma_cv; /* Desired Content Max Luminance Data. Real value(in 1cd/m^2) = 50.2^(CV/32) */
    td_u8 avg_luma_cv; /* Desired Content Max Frame-average Luminance. Real value(in 1cd/m^2) = 50.2^(CV/32) */
    /*
     * Desired Content Min Luminance Data.
     * Real value(in 1cd/m^2) = u8MaxLuminance_CV * (u8MinLuminance_CV/255)^2 / 100
     */
    td_u8 min_luma_cv;
} ot_hdmi_hdr_capability;

typedef struct {
    td_u32  vfb;            /* vertical front blank */
    td_u32  vbb;            /* vertical back blank */
    td_u32  vact;           /* vertical active area */
    td_u32  hfb;            /* horizontal front blank */
    td_u32  hbb;            /* horizontal back blank */
    td_u32  hact;           /* horizontal active area */
    td_u32  vpw;            /* vertical sync pulse width */
    td_u32  hpw;            /* horizontal sync pulse width */
    td_bool idv;            /* flag of data valid signal is needed flip */
    td_bool ihs;            /* flag of horizontal sync pulse is needed flip */
    td_bool ivs;            /* flag of vertical sync pulse is needed flip */
    td_u32  img_width;      /* image width */
    td_u32  img_height;     /* image height */
    td_u32  aspect_ratio_w; /* aspect ratio width */
    td_u32  aspect_ratio_h; /* aspect ratio height */
    td_bool interlace;      /* flag of interlace */
    td_s32  pixel_clk;      /* pixelc clk for this timing, unit:KHz */
} ot_hdmi_timing_info;

typedef struct {
    td_u32 detail_timing_num;
    ot_hdmi_timing_info detail_timing[OT_HDMI_DETAIL_TIMING_MAX];
} ot_hdmi_detail_timing;

typedef struct {
    td_bool is_connected; /* Whether the devices are connected. */
    /* Whether the HDMI is supported by the device. If the HDMI is not supported by the device, the device is DVI. */
    td_bool support_hdmi;
    td_bool is_sink_power_on; /* Whether the sink device is powered on. */
    ot_hdmi_video_format native_video_format; /* Physical resolution of the display device. */
    /*
     * Video capability set.
     * TD_TRUE: This display format is supported.
     * TD_FALSE: This display format is not supported.
     */
    td_bool support_video_format[OT_HDMI_VIDEO_FORMAT_BUTT];
    /*
     * Whether the YCBCR display is supported.
     * TD_TRUE: The YCBCR display is supported.
     * TD_FALSE: Only red-green-blue (RGB) is supported.
     */
    td_bool support_ycbcr;
    /* Whether the xvYCC601 color format is supported. */
    td_bool support_xvycc601;
    /* Whether the xvYCC709 color format is supported. */
    td_bool support_xvycc709;
    /* Transfer profile supported by xvYCC601. 1: P0; 2: P1; 4: P2. */
    td_u8 md_bit;
    /* Num of audio Info */
    td_u32 audio_info_num;
    /* Audio Info.For details, see Table 37 in EIA-CEA-861-D. */
    ot_hdmi_audio_info audio_info[OT_HDMI_MAX_AUDIO_CAPBILITY_CNT];
    /* Speaker position. For details, see the definition of SpeakerDATABlock in EIA-CEA-861-D. */
    td_bool speaker[OT_HDMI_AUDIO_SPEAKER_BUTT];
    /* Device vendor flag */
    td_u8 manufacture_name[OT_HDMI_MANUFACTURE_NAME_LEN];
    /* Device ID. */
    td_u32 pdt_code;
    /* Device sequence number. */
    td_u32 serial_num;
    /* Device production data (week). */
    td_u32 week_of_manufacture;
    /* Set the production data (year). */
    td_u32 year_of_manufacture;
    /* Device version number */
    td_u8 version;
    /* Device sub version number */
    td_u8 revision;
    /* EDID extended block number */
    td_u8 edid_ex_blk_num;
    /* Valid flag of the consumer electronics control (CEC) physical address */
    td_bool is_phys_addr_valid;
    /* CEC physical address A */
    td_u8 phys_addr_a;
    /* CEC physical address B */
    td_u8 phys_addr_b;
    /* CEC physical address C */
    td_u8 phys_addr_c;
    /* CEC physical address D */
    td_u8 phys_addr_d;
    /* Whether to support the DVI dual-link operation. */
    td_bool support_dvi_dual;
    /* Whether to support the YCBCR 4:4:4 deep-color mode. */
    td_bool support_deepcolor_ycbcr444;
    /* Whether to support the deep-color 30-bit mode. */
    td_bool support_deepcolor_30bit;
    /* Whether to support the deep-color 36-bit mode. */
    td_bool support_deepcolor_36bit;
    /* Whether to support the deep-color 48-bit mode. */
    td_bool support_deepcolor_48bit;
    /* Whether to support the Supports_AI mode. */
    td_bool support_ai;
    /* Maximum TMDS clock. */
    td_u32 max_tmds_clk;
    /* Delay flag bit. */
    td_bool i_latency_fields_present;
    /* whether Video_Latency and Audio_Latency fields are present */
    td_bool latency_fields_present;
    /* Special video format */
    td_bool hdmi_video_present;
    /* Video delay */
    td_u8 video_latency;
    /* Audio delay */
    td_u8 audio_latency;
    /* Video delay in interlaced video mode */
    td_u8 interlaced_video_latency;
    /* Audio delay in interlaced video mode */
    td_u8 interlaced_audio_latency;
    /* Whether to support the YUV420 deep-color 30-bit mode. */
    td_bool support_y420_dc_30bit;
    /* Whether to support the YUV420 deep-color 36-bit mode. */
    td_bool support_y420_dc_36bit;
    /* Whether to support the YUV420 deep-color 48-bit mode. */
    td_bool support_y420_dc_48bit;
    /* Whether to support HDMI2.0. */
    td_bool support_hdmi_2_0;
    /* the format support YUV420 */
    td_bool support_y420_format[OT_HDMI_VIDEO_FORMAT_BUTT];
    /* the format only support YUV420 */
    td_bool only_support_y420_format[OT_HDMI_VIDEO_FORMAT_BUTT];
    /*
     * flag of RGB Quantization Range selectable.
     * when TD_TRUE,sink expect full/limited range is selectable;or sink expect default range.
     */
    td_bool ycc_quant_selectable;
    /*
     * flag of YCC Quantization Range selectable.
     * when TD_TRUE,sink expect full/limited range is selectable;or sink expect default range.
     */
    td_bool rgb_quant_selectable;
    /* HDR support flag.When HDR Static Metadata Data Block is indicated,flag TD_TRUE;or,TD_FALSE. */
    td_bool support_hdr;
    /* HDR capability */
    ot_hdmi_hdr_capability hdr;
    /* Detailed timing information */
    ot_hdmi_detail_timing detailed_timing;
} ot_hdmi_sink_capability;

typedef struct {
    /* Whether to forcibly output the video over the HDMI. */
    td_bool hdmi_en;
    /* Video norm. This value of the video norm must be consistent with the norm of the video output. */
    ot_hdmi_video_format video_format;
    /* DeepColor output mode.It is OT_HDMI_DEEP_COLOR_24BIT by default. */
    ot_hdmi_deep_color deep_color_mode;
    /* Whether to enable the audio. */
    td_bool audio_en;
    /* Audio sampling rate. This parameter needs to be consistent with that of the VO. */
    ot_hdmi_sample_rate sample_rate;
    /* Audio bit width. It is 16 by default. This parameter needs to be consistent with that of the VO. */
    ot_hdmi_bit_depth bit_depth;
    /* Whether to enable auth mode. 0: disabled 1: enabled */
    td_bool auth_mode_en;
    /*
     * Enable flag of deep color mode adapting case of user setting incorrect,
     * default: TD_FALSE.When user have no any adapting strategy,suggestion TD_TRUE
     */
    td_bool deep_color_adapt_en;
    /*
     * Pixclk of enVideoFmt(unit is kHz).
     * (This param is valid only when enVideoFmt is OT_HDMI_VIDEO_FMT_VESA_CUSTOMER_DEFINE)
     */
    td_u32 pix_clk;
} ot_hdmi_attr;

typedef struct {
    td_bool edid_valid;
    td_u32  edid_len;
    td_u8   edid[OT_HDMI_EDID_RAW_DATA_LEN];
} ot_hdmi_edid;

typedef struct {
    ot_hdmi_audio_chn_cnt      chn_cnt;
    ot_hdmi_coding_type        coding_type;
    ot_hdmi_audio_sample_size  sample_size;
    ot_hdmi_audio_sample_freq  sampling_freq;
    td_u8                      chn_alloc; /* Channel/Speaker Allocation.Range [0,255] */
    ot_hdmi_level_shift_val    level_shift;
    ot_hdmi_lfe_playback_level lfe_playback_level;
    td_bool                    down_mix_inhibit;
} ot_hdmi_audio_infoframe;

typedef struct {
    ot_hdmi_video_format        timing_mode;
    ot_hdmi_color_space         color_space;
    td_bool                     active_info_present;
    ot_hdmi_bar_info            bar_info;
    ot_hdmi_scan_info           scan_info;
    ot_hdmi_colorimetry         colorimetry;
    ot_hdmi_ex_colorimetry      ex_colorimetry;
    ot_pic_aspect_ratio         aspect_ratio;
    ot_hdmi_active_aspect_ratio active_aspect_ratio;
    ot_hdmi_pic_scaline         pic_scaling;
    ot_hdmi_rgb_quant_range     rgb_quant;
    td_bool                     is_it_content;
    ot_hdmi_pixel_repetition    pixel_repetition;
    ot_hdmi_content_type        content_type;
    ot_hdmi_ycc_quant_range     ycc_quant;
    td_u16                      line_n_end_of_top_bar;
    td_u16                      line_n_start_of_bot_bar;
    td_u16                      pixel_n_end_of_left_bar;
    td_u16                      pixel_n_start_of_right_bar;
} ot_hdmi_avi_infoframe;

typedef struct {
    td_u8 data_len;
    td_u8 user_data[OT_HDMI_VENDOR_USER_DATA_MAX_LEN];
} ot_hdmi_vendorspec_infoframe;

typedef union {
    ot_hdmi_avi_infoframe avi_infoframe;     /* AUTO:ot_hdmi_infoframe_type:OT_INFOFRAME_TYPE_AVI; */
    ot_hdmi_audio_infoframe audio_infoframe; /* AUTO:ot_hdmi_infoframe_type:OT_INFOFRAME_TYPE_AUDIO; */
    ot_hdmi_vendorspec_infoframe vendor_spec_infoframe; /* AUTO:ot_hdmi_infoframe_type:OT_INFOFRAME_TYPE_VENDORSPEC; */
} ot_hdmi_infoframe_unit;

typedef struct {
    ot_hdmi_infoframe_type infoframe_type; /* InfoFrame type */
    ot_hdmi_infoframe_unit infoframe_unit; /* InfoFrame date */
} ot_hdmi_infoframe;

typedef struct {
    ot_hdmi_callback hdmi_event_callback; /* Event handling callback function */
    td_void *private_data;                /* Private data of the callback functions and parameters */
} ot_hdmi_callback_func;

typedef struct {
    td_u32 i_de_main_clk;
    td_u32 i_de_main_data;
    td_u32 i_main_clk;
    td_u32 i_main_data;
    td_u32 ft_cap_clk;
    td_u32 ft_cap_data;
} ot_hdmi_hw_param;

typedef struct {
    ot_hdmi_hw_param hw_param[OT_HDMI_HW_PARAM_NUM];
} ot_hdmi_hw_spec;

typedef struct {
    td_bool is_connected;     /* The Device is connected or disconnected */
    td_bool is_sink_power_on; /* The sink is PowerOn or not */
} ot_hdmi_status;

typedef enum {
    OT_HDMI_TRACE_LEN_0,
    OT_HDMI_TRACE_LEN_1,
    OT_HDMI_TRACE_LEN_2,
    OT_HDMI_TRACE_LEN_3,
    OT_HDMI_TRACE_LEN_4,
    OT_HDMI_TRACE_LEN_5,
    OT_HDMI_TRACE_LEN_6,
    OT_HDMI_TRACE_LEN_7,
    OT_HDMI_TRACE_LEN_8,
    OT_HDMI_TRACE_DEFAULT,
    OT_HDMI_TRACE_BUTT
} ot_hdmi_trace_len;

typedef struct {
    td_bool emi_en; /* emi enable/disable */
    ot_hdmi_trace_len trace_len; /* Phy parameters selection. */
} ot_hdmi_mod_param;

#define OT_ERR_HDMI_NOT_INIT              OT_DEFINE_ERR(OT_ID_HDMI, OT_ERR_LEVEL_ERROR, OT_ERRNO_HDMI_NOT_INIT)
#define OT_ERR_HDMI_INVALID_PARA          OT_DEFINE_ERR(OT_ID_HDMI, OT_ERR_LEVEL_ERROR, OT_ERRNO_HDMI_INVALID_PARAM)
#define OT_ERR_HDMI_NULL_PTR              OT_DEFINE_ERR(OT_ID_HDMI, OT_ERR_LEVEL_ERROR, OT_ERRNO_HDMI_NULL_PTR)
#define OT_ERR_HDMI_DEV_NOT_OPEN          OT_DEFINE_ERR(OT_ID_HDMI, OT_ERR_LEVEL_ERROR, OT_ERRNO_HDMI_DEV_NOT_OPEN)
#define OT_ERR_HDMI_DEV_NOT_CONNECT       OT_DEFINE_ERR(OT_ID_HDMI, OT_ERR_LEVEL_ERROR, OT_ERRNO_HDMI_DEV_NOT_CONNECT)
#define OT_ERR_HDMI_READ_SINK_FAILED      OT_DEFINE_ERR(OT_ID_HDMI, OT_ERR_LEVEL_ERROR, OT_ERRNO_HDMI_READ_SINK_FAILED)
#define OT_ERR_HDMI_INIT_ALREADY          OT_DEFINE_ERR(OT_ID_HDMI, OT_ERR_LEVEL_ERROR, OT_ERRNO_HDMI_INIT_ALREADY)
#define OT_ERR_HDMI_CALLBACK_ALREADY      OT_DEFINE_ERR(OT_ID_HDMI, OT_ERR_LEVEL_ERROR, OT_ERRNO_HDMI_CALLBACK_ALREADY)
#define OT_ERR_HDMI_INVALID_CALLBACK      OT_DEFINE_ERR(OT_ID_HDMI, OT_ERR_LEVEL_ERROR, OT_ERRNO_HDMI_INVALID_CALLBACK)
#define OT_ERR_HDMI_FEATURE_NO_SUPPORT \
    OT_DEFINE_ERR(OT_ID_HDMI, OT_ERR_LEVEL_ERROR, OT_ERRNO_HDMI_FEATURE_NO_SUPPORT)
#define OT_ERR_HDMI_BUS_BUSY              OT_DEFINE_ERR(OT_ID_HDMI, OT_ERR_LEVEL_ERROR, OT_ERRNO_HDMI_BUS_BUSY)
#define OT_ERR_HDMI_READ_EVENT_FAILED     OT_DEFINE_ERR(OT_ID_HDMI, OT_ERR_LEVEL_ERROR, OT_ERRNO_HDMI_READ_EVENT_FAILED)
#define OT_ERR_HDMI_NOT_START             OT_DEFINE_ERR(OT_ID_HDMI, OT_ERR_LEVEL_ERROR, OT_ERRNO_HDMI_NOT_START)
#define OT_ERR_HDMI_READ_EDID_FAILED      OT_DEFINE_ERR(OT_ID_HDMI, OT_ERR_LEVEL_ERROR, OT_ERRNO_HDMI_READ_EDID_FAILED)
#define OT_ERR_HDMI_INIT_FAILED           OT_DEFINE_ERR(OT_ID_HDMI, OT_ERR_LEVEL_ERROR, OT_ERRNO_HDMI_INIT_FAILED)
#define OT_ERR_HDMI_CREATE_TESK_FAILED \
    OT_DEFINE_ERR(OT_ID_HDMI, OT_ERR_LEVEL_ERROR, OT_ERRNO_HDMI_CREATE_TESK_FAILED)
#define OT_ERR_HDMI_MALLOC_FAILED         OT_DEFINE_ERR(OT_ID_HDMI, OT_ERR_LEVEL_ERROR, OT_ERRNO_HDMI_MALLOC_FAILED)
#define OT_ERR_HDMI_FREE_FAILED           OT_DEFINE_ERR(OT_ID_HDMI, OT_ERR_LEVEL_ERROR, OT_ERRNO_HDMI_FREE_FAILED)
#define OT_ERR_HDMI_PTHREAD_CREATE_FAILED \
    OT_DEFINE_ERR(OT_ID_HDMI, OT_ERR_LEVEL_ERROR, OT_ERRNO_HDMI_PTHREAD_CREATE_FAILED)
#define OT_ERR_HDMI_PTHREAD_JOIN_FAILED \
    OT_DEFINE_ERR(OT_ID_HDMI, OT_ERR_LEVEL_ERROR, OT_ERRNO_HDMI_PTHREAD_JOIN_FAILED)
#define OT_ERR_HDMI_STRATEGY_FAILED       OT_DEFINE_ERR(OT_ID_HDMI, OT_ERR_LEVEL_ERROR, OT_ERRNO_HDMI_STRATEGY_FAILED)
#define OT_ERR_HDMI_SET_ATTR_FAILED       OT_DEFINE_ERR(OT_ID_HDMI, OT_ERR_LEVEL_ERROR, OT_ERRNO_HDMI_SET_ATTR_FAILED)
#define OT_ERR_HDMI_CALLBACK_NOT_REGISTER \
    OT_DEFINE_ERR(OT_ID_HDMI, OT_ERR_LEVEL_ERROR, OT_ERRNO_HDMI_CALLBACK_NOT_REGISTER)
#define OT_ERR_HDMI_UNKNOWN_COMMAND       OT_DEFINE_ERR(OT_ID_HDMI, OT_ERR_LEVEL_ERROR, OT_ERRNO_HDMI_UNKNOWN_CMD)
#define OT_ERR_HDMI_MUTEX_LOCK_FAILED     OT_DEFINE_ERR(OT_ID_HDMI, OT_ERR_LEVEL_ERROR, OT_ERRNO_HDMI_MUTEX_LOCK_FAILED)

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
#endif

