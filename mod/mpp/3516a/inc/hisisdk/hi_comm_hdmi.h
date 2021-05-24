/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2015-2018. All rights reserved.
 * Description: Common defination of hdmi
 * Author: Hisilicon HDMI software group
 * Create: 2015/03/01
 */
#ifndef __HI_COMM_HDMI_H__
#define __HI_COMM_HDMI_H__

#include "hi_type.h"
#ifndef HDMI_BUILD_IN_BOOT
#include "hi_errno.h"
#endif
#include "hi_common.h"
#include "hi_comm_video.h"

#define HI_HDMI_MAX_AUDIO_CAP_COUNT  16
#define HI_HDMI_MAX_SAMPE_RATE_NUM   8
#define HI_HDMI_MAX_BIT_DEPTH_NUM    6
#define HI_HDMI_EDID_RAW_DATA_LEN    512
/* user data len:31 - 4(pkt header) - 5(4K- the length must be 0x05) */
#define HI_HDMI_VENDOR_USER_DATA_MAX_LEN 22
#define HI_HDMI_DETAIL_TIMING_MAX        10
#define HI_HDMI_VENDOR_NAME_SIZE         8
#define HI_HDMI_VENDOR_DESC_SIZE         16

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

typedef enum hiHDMI_VIDEO_FMT_E {
    HI_HDMI_VIDEO_FMT_1080P_60 = 0,
    HI_HDMI_VIDEO_FMT_1080P_50,
    HI_HDMI_VIDEO_FMT_1080P_30,
    HI_HDMI_VIDEO_FMT_1080P_25,
    HI_HDMI_VIDEO_FMT_1080P_24,
    HI_HDMI_VIDEO_FMT_1080i_60,
    HI_HDMI_VIDEO_FMT_1080i_50,
    HI_HDMI_VIDEO_FMT_720P_60,
    HI_HDMI_VIDEO_FMT_720P_50,
    HI_HDMI_VIDEO_FMT_576P_50,
    HI_HDMI_VIDEO_FMT_480P_60,
    HI_HDMI_VIDEO_FMT_PAL,
    HI_HDMI_VIDEO_FMT_PAL_N,
    HI_HDMI_VIDEO_FMT_PAL_Nc,
    HI_HDMI_VIDEO_FMT_NTSC,
    HI_HDMI_VIDEO_FMT_NTSC_J,
    HI_HDMI_VIDEO_FMT_NTSC_PAL_M,
    HI_HDMI_VIDEO_FMT_SECAM_SIN,
    HI_HDMI_VIDEO_FMT_SECAM_COS,
    HI_HDMI_VIDEO_FMT_861D_640X480_60,
    HI_HDMI_VIDEO_FMT_VESA_800X600_60,
    HI_HDMI_VIDEO_FMT_VESA_1024X768_60,
    HI_HDMI_VIDEO_FMT_VESA_1280X720_60,
    HI_HDMI_VIDEO_FMT_VESA_1280X800_60,
    HI_HDMI_VIDEO_FMT_VESA_1280X1024_60,
    HI_HDMI_VIDEO_FMT_VESA_1366X768_60,
    HI_HDMI_VIDEO_FMT_VESA_1440X900_60,
    HI_HDMI_VIDEO_FMT_VESA_1440X900_60_RB,
    HI_HDMI_VIDEO_FMT_VESA_1600X900_60_RB,
    HI_HDMI_VIDEO_FMT_VESA_1600X1200_60,
    HI_HDMI_VIDEO_FMT_VESA_1680X1050_60,
    HI_HDMI_VIDEO_FMT_VESA_1920X1080_60,
    HI_HDMI_VIDEO_FMT_VESA_1920X1200_60,
    HI_HDMI_VIDEO_FMT_VESA_2048X1152_60,
    HI_HDMI_VIDEO_FMT_2560x1440_30,
    HI_HDMI_VIDEO_FMT_2560x1440_60,
    HI_HDMI_VIDEO_FMT_2560x1600_60,
    HI_HDMI_VIDEO_FMT_1920x2160_30,
    HI_HDMI_VIDEO_FMT_3840X2160P_24,
    HI_HDMI_VIDEO_FMT_3840X2160P_25,
    HI_HDMI_VIDEO_FMT_3840X2160P_30,
    HI_HDMI_VIDEO_FMT_3840X2160P_50,
    HI_HDMI_VIDEO_FMT_3840X2160P_60,
    HI_HDMI_VIDEO_FMT_4096X2160P_24,
    HI_HDMI_VIDEO_FMT_4096X2160P_25,
    HI_HDMI_VIDEO_FMT_4096X2160P_30,
    HI_HDMI_VIDEO_FMT_4096X2160P_50,
    HI_HDMI_VIDEO_FMT_4096X2160P_60,
    HI_HDMI_VIDEO_FMT_3840X2160P_120,
    HI_HDMI_VIDEO_FMT_4096X2160P_120,
    HI_HDMI_VIDEO_FMT_7680X4320P_30,
    HI_HDMI_VIDEO_FMT_VESA_CUSTOMER_DEFINE,
    HI_HDMI_VIDEO_FMT_BUTT
} HI_HDMI_VIDEO_FMT_E;

typedef enum hiHDMI_PIC_ASPECT_RATIO_E {
    HI_HDMI_PIC_ASP_RATIO_NO_DATA, /* Aspect Ratio unknown */
    HI_HDMI_PIC_ASP_RATIO_4TO3,    /* Aspect Ratio 4:3 */
    HI_HDMI_PIC_ASP_RATIO_16TO9,   /* Aspect Ratio 16:9 */
    HI_HDMI_PIC_ASP_RATIO_64TO27,
    HI_HDMI_PIC_ASP_RATIO_256TO135,
    HI_HDMI_PIC_ASP_RATIO_RESERVED
} HI_HDMI_PIC_ASPECT_RATIO_E;

typedef enum hiHDMI_ACT_ASPECT_RATIO_E {
    HI_HDMI_ACT_ASP_RATIO_RESERVED_0,
    HI_HDMI_ACT_ASP_RATIO_RESERVED_1,
    HI_HDMI_ACT_ASP_RATIO_16TO9_TOP,
    HI_HDMI_ACT_ASP_RATIO_14TO9_TOP,
    HI_HDMI_ACT_ASP_RATIO_16TO9_BOX_CENTER,
    HI_HDMI_ACT_ASP_RATIO_RESERVED_5,
    HI_HDMI_ACT_ASP_RATIO_RESERVED_6,
    HI_HDMI_ACT_ASP_RATIO_RESERVED_7,
    HI_HDMI_ACT_ASP_RATIO_SAME_PIC,
    HI_HDMI_ACT_ASP_RATIO_4TO3_CENTER,
    HI_HDMI_ACT_ASP_RATIO_16TO9_CENTER,
    HI_HDMI_ACT_ASP_RATIO_14TO9_CENTER,
    HI_HDMI_ACT_ASP_RATIO_RESERVED_12,
    HI_HDMI_ACT_ASP_RATIO_4TO3_14_9,
    HI_HDMI_ACT_ASP_RATIO_16TO9_14_9,
    HI_HDMI_ACT_ASP_RATIO_16TO9_4_3
} HI_HDMI_ACT_ASPECT_RATIO_E;

typedef enum hiHDMI_SAMPLE_RATE_E {
    HI_HDMI_SAMPLE_RATE_UNKNOWN,       /* unknown sample rate */
    HI_HDMI_SAMPLE_RATE_8K   = 8000,   /* 8K sample rate */
    HI_HDMI_SAMPLE_RATE_11K  = 11025,  /* 11.025K sample rate */
    HI_HDMI_SAMPLE_RATE_12K  = 12000,  /* 12K sample rate */
    HI_HDMI_SAMPLE_RATE_16K  = 16000,  /* 16K sample rate */
    HI_HDMI_SAMPLE_RATE_22K  = 22050,  /* 22.050K sample rate */
    HI_HDMI_SAMPLE_RATE_24K  = 24000,  /* 24K sample rate */
    HI_HDMI_SAMPLE_RATE_32K  = 32000,  /* 32K sample rate */
    HI_HDMI_SAMPLE_RATE_44K  = 44100,  /* 44.1K sample rate */
    HI_HDMI_SAMPLE_RATE_48K  = 48000,  /* 48K sample rate */
    HI_HDMI_SAMPLE_RATE_88K  = 88200,  /* 88.2K sample rate */
    HI_HDMI_SAMPLE_RATE_96K  = 96000,  /* 96K sample rate */
    HI_HDMI_SAMPLE_RATE_176K = 176400, /* 176K sample rate */
    HI_HDMI_SAMPLE_RATE_192K = 192000, /* 192K sample rate */
    HI_HDMI_SAMPLE_RATE_768K = 768000, /* 768K sample rate */
    HI_HDMI_SAMPLE_RATE_BUTT
} HI_HDMI_SAMPLE_RATE_E;

typedef enum hiHDMI_AUDIO_FORMAT_CODE_E {
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
} HI_HDMI_AUDIO_FORMAT_CODE_E;

typedef enum hiHDMI_BIT_DEPTH_E {
    HI_HDMI_BIT_DEPTH_UNKNOWN,  /* unknown bit width */
    HI_HDMI_BIT_DEPTH_8  = 8,   /* 8 bits width */
    HI_HDMI_BIT_DEPTH_16 = 16,  /* 16 bits width */
    HI_HDMI_BIT_DEPTH_18 = 18,  /* 18 bits width */
    HI_HDMI_BIT_DEPTH_20 = 20,  /* 20 bits width */
    HI_HDMI_BIT_DEPTH_24 = 24,  /* 24 bits width */
    HI_HDMI_BIT_DEPTH_32 = 32,  /* 32 bits width */
    HI_HDMI_BIT_DEPTH_BUTT
} HI_HDMI_BIT_DEPTH_E;

typedef enum hiHDMI_SND_INTERFACE_E {
    HI_HDMI_SND_INTERFACE_I2S,   /* I2S(Inter-IC Sound) */
    HI_HDMI_SND_INTERFACE_SPDIF, /* SPDIF(Sony/Philips Digital Interface) */
    HI_HDMI_SND_INTERFACE_HBR,   /* SPDIF(Sony/Philips Digital Interface) */
    HI_HDMI_SND_INTERFACE_BUTT
} HI_HDMI_SND_INTERFACE_E;

typedef enum hiHDMI_ID_E {
    HI_HDMI_ID_0,
    HI_HDMI_ID_BUTT
} HI_HDMI_ID_E;

typedef enum hiHDMI_EVENT_TYPE_E {
    HI_HDMI_EVENT_HOTPLUG = 0x10,          /* HDMI hot-plug event */
    HI_HDMI_EVENT_NO_PLUG,                 /* HDMI cable disconnection event */
    HI_HDMI_EVENT_EDID_FAIL,               /* HDMI EDID read failure event */
    HI_HDMI_EVENT_HDCP_FAIL,               /* HDCP verification failure event */
    HI_HDMI_EVENT_HDCP_SUCCESS,            /* HDCP verification success event */
    HI_HDMI_EVENT_HDCP_USERSETTING = 0x17, /* HDMI reset event,user should re-verification HDCP */
    HI_HDMI_EVENT_BUTT
} HI_HDMI_EVENT_TYPE_E;

typedef enum hiHDMI_VIDEO_MODE {
    HI_HDMI_VIDEO_MODE_RGB444,
    HI_HDMI_VIDEO_MODE_YCBCR422,
    HI_HDMI_VIDEO_MODE_YCBCR444,
    HI_HDMI_VIDEO_MODE_YCBCR420,
    HI_HDMI_VIDEO_MODE_BUTT
} HI_HDMI_VIDEO_MODE_E;

/* Color Component Sample format and chroma sampling format enum,see EIA-CEA-861-D/F */
typedef enum hiHDMI_COLOR_SPACE_E {
    HI_HDMI_COLOR_SPACE_RGB444,
    HI_HDMI_COLOR_SPACE_YCBCR422,
    HI_HDMI_COLOR_SPACE_YCBCR444,
    /* following is new featrue of CEA-861-F */
    HI_HDMI_COLOR_SPACE_YCBCR420
} HI_HDMI_COLOR_SPACE_E;

typedef enum hiHDMI_DEEP_COLOR_E {
    HI_HDMI_DEEP_COLOR_24BIT,       /* HDMI Deep Color 24bit mode */
    HI_HDMI_DEEP_COLOR_30BIT,       /* HDMI Deep Color 30bit mode */
    HI_HDMI_DEEP_COLOR_36BIT,       /* HDMI Deep Color 36bit mode */
    HI_HDMI_DEEP_COLOR_OFF = 0xff,  /* HDMI Deep Color OFF mode(24bit). */
    HI_HDMI_DEEP_COLOR_BUTT
} HI_HDMI_DEEP_COLOR_E;

typedef enum hiHDMI_BARINFO_E {
    HDMI_BAR_INFO_NOT_VALID, /* Bar Data not valid */
    HDMI_BAR_INFO_V,         /* Vertical bar data valid */
    HDMI_BAR_INFO_H,         /* Horizental bar data valid */
    HDMI_BAR_INFO_VH         /* Horizental and Vertical bar data valid */
} HI_HDMI_BARINFO_E;

typedef enum hiHDMI_SCANINFO_E {
    HDMI_SCAN_INFO_NO_DATA,      /* No Scan information */
    HDMI_SCAN_INFO_OVERSCANNED,  /* Scan information, Overscanned (for television) */
    HDMI_SCAN_INFO_UNDERSCANNED, /* Scan information, Underscanned (for computer) */
    HDMI_SCAN_INFO_FUTURE
} HI_HDMI_SCANINFO_E;

typedef enum hiHDMI_PICTURE_SCALING_E {
    HDMI_PICTURE_NON_UNIFORM_SCALING, /* No Known, non-uniform picture scaling */
    HDMI_PICTURE_SCALING_H,           /* Picture has been scaled horizentally */
    HDMI_PICTURE_SCALING_V,           /* Picture has been scaled Vertically */
    HDMI_PICTURE_SCALING_HV           /* Picture has been scaled Horizentally and Vertically */
} HI_HDMI_PICTURE_SCALING_E;

typedef enum hiHDMI_COLORIMETRY_E {
    HDMI_COLORIMETRY_NO_DATA,  /* Colorimetry no data option */
    HDMI_COLORIMETRY_ITU601,   /* Colorimetry ITU601 option */
    HDMI_COLORIMETRY_ITU709,   /* Colorimetry ITU709 option */
    HDMI_COLORIMETRY_EXTENDED  /* Colorimetry extended option */
} HI_HDMI_COLORIMETRY_E;

typedef enum hiHDMI_EXT_COLORIMETRY_E {
    HDMI_COLORIMETRY_XVYCC_601,               /* Colorimetry xvYCC601 extened option */
    HDMI_COLORIMETRY_XVYCC_709,               /* Colorimetry xvYCC709 extened option */
    HDMI_COLORIMETRY_S_YCC_601,               /* Colorimetry S YCC 601 extened option */
    HDMI_COLORIMETRY_ADOBE_YCC_601,           /* Colorimetry ADOBE YCC 601 extened option */
    HDMI_COLORIMETRY_ADOBE_RGB,               /* Colorimetry ADOBE RGB extened option */
    HDMI_COLORIMETRY_2020_CONST_LUMINOUS,     /* Colorimetry ITU2020 extened option */
    HDMI_COLORIMETRY_2020_NON_CONST_LUMINOUS, /* Colorimetry ITU2020 extened option */
    HDMI_COLORIMETRY_RESERVED
} HI_HDMI_EXT_COLORIMETRY_E;

typedef enum hiHDMI_RGB_QUAN_RAGE_E {
    HDMI_RGB_QUANTIZATION_DEFAULT_RANGE, /* Defaulr range, it depends on the video format */
    HDMI_RGB_QUANTIZATION_LIMITED_RANGE, /* Limited quantization range of 220 levels when receiving a CE video format */
    HDMI_RGB_QUANTIZATION_FULL_RANGE,    /* Full quantization range of 256 levels when receiving an IT video format */
    HDMI_RGB_QUANTIZATION_FULL_RESERVED
} HI_HDMI_RGB_QUAN_RAGE_E;

typedef enum hiHDMI_YCC_QUAN_RAGE_E {
    HDMI_YCC_QUANTIZATION_LIMITED_RANGE, /* Limited quantization range of 220 levels when receiving a CE video format */
    HDMI_YCC_QUANTIZATION_FULL_RANGE,    /* Full quantization range of 256 levels when receiving an IT video format */
    HDMI_YCC_QUANTIZATION_RESERVED_2,
    HDMI_YCC_QUANTIZATION_RESERVED_3
} HI_HDMI_YCC_QUAN_RAGE_E;

typedef enum hiHDMI_QUANTIZATION_E {
    HDMI_QUANTIZATION_LIMITED_RANGE, /* Limited quantization range of 220 levels when receiving a CE video format */
    HDMI_QUANTIZATION_FULL_RANGE,    /* Full quantization range of 256 levels when receiving an IT video format */
    HDMI_QUANTIZATION_BUTT
} HI_HDMI_QUANTIZATION_E;

typedef enum hiHDMI_PIXEL_REPETITION_E {
    HDMI_PIXEL_REPET_NO,
    HDMI_PIXEL_REPET_2_TIMES,
    HDMI_PIXEL_REPET_3_TIMES,
    HDMI_PIXEL_REPET_4_TIMES,
    HDMI_PIXEL_REPET_5_TIMES,
    HDMI_PIXEL_REPET_6_TIMES,
    HDMI_PIXEL_REPET_7_TIMES,
    HDMI_PIXEL_REPET_8_TIMES,
    HDMI_PIXEL_REPET_9_TIMES,
    HDMI_PIXEL_REPET_10_TIMES,
    HDMI_PIXEL_REPET_RESERVED_A,
    HDMI_PIXEL_REPET_RESERVED_B,
    HDMI_PIXEL_REPET_RESERVED_C,
    HDMI_PIXEL_REPET_RESERVED_D,
    HDMI_PIXEL_REPET_RESERVED_E,
    HDMI_PIXEL_REPET_RESERVED_F
} HI_HDMI_PIXEL_REPETITION_E;

typedef enum hiHDMI_CONTENT_TYPE_E {
    HDMI_CONTNET_GRAPHIC, /* Graphics type */
    HDMI_CONTNET_PHOTO,   /* Photo type */
    HDMI_CONTNET_CINEMA,  /* Cinema type */
    HDMI_CONTNET_GAME     /* Game type */
} HI_HDMI_CONTENT_TYPE_E;

typedef enum hiHDMI_CODING_TYPE_E {
    HDMI_AUDIO_CODING_REFER_STREAM_HEAD,
    HDMI_AUDIO_CODING_PCM,
    HDMI_AUDIO_CODING_AC3,
    HDMI_AUDIO_CODING_MPEG1,
    HDMI_AUDIO_CODING_MP3,
    HDMI_AUDIO_CODING_MPEG2,
    HDMI_AUDIO_CODING_AACLC,
    HDMI_AUDIO_CODING_DTS,
    HDMI_AUDIO_CODING_ATRAC,
    HDMI_AUDIO_CODIND_ONE_BIT_AUDIO,
    HDMI_AUDIO_CODING_ENAHNCED_AC3,
    HDMI_AUDIO_CODING_DTS_HD,
    HDMI_AUDIO_CODING_MAT,
    HDMI_AUDIO_CODING_DST,
    HDMI_AUDIO_CODING_WMA_PRO,
    HDMI_AUDIO_CODING_MAX
} HI_HDMI_CODING_TYPE_E;

typedef enum hiHDMI_AUDIO_SPEAKER_E {
    HDMI_AUDIO_SPEAKER_FL_FR,
    HDMI_AUDIO_SPEAKER_LFE,
    HDMI_AUDIO_SPEAKER_FC,
    HDMI_AUDIO_SPEAKER_RL_RR,
    HDMI_AUDIO_SPEAKER_RC,
    HDMI_AUDIO_SPEAKER_FLC_FRC,
    HDMI_AUDIO_SPEAKER_RLC_RRC,
    HDMI_AUDIO_SPEAKER_FLW_FRW,
    HDMI_AUDIO_SPEAKER_FLH_FRH,
    HDMI_AUDIO_SPEAKER_TC,
    HDMI_AUDIO_SPEAKER_FCH,
    HDMI_AUDIO_SPEAKER_BUTT
} HI_HDMI_AUDIO_SPEAKER_E;

typedef enum hiHDMI_FORCE_ACTION_E {
    HI_HDMI_FORCE_NULL,      /* Standard mode */
    HI_HDMI_FORCE_HDMI,      /* Forcibly output video in HDMI mode */
    HI_HDMI_FORCE_DVI,       /* Forcibly output video in DVI mode */
    HI_HDMI_INIT_BOOT_CONFIG /* Test only */
} HI_HDMI_FORCE_ACTION_E;

typedef enum hiHI_HDMI_CSC_MATRIX_E {
    HI_HDMI_CSC_MATRIX_IDENTITY,
    HI_HDMI_CSC_MATRIX_BT601_TO_BT709,
    HI_HDMI_CSC_MATRIX_BT709_TO_BT601,
    HI_HDMI_CSC_MATRIX_BT601_TO_RGB_PC,
    HI_HDMI_CSC_MATRIX_BT709_TO_RGB_PC,
    HI_HDMI_CSC_MATRIX_RGB_TO_BT601_PC,
    HI_HDMI_CSC_MATRIX_RGB_TO_BT709_PC,
    HI_HDMI_CSC_MATRIX_BUTT
} HI_HDMI_CSC_MATRIX_E;

typedef void (*HI_HDMI_CallBack)(HI_HDMI_EVENT_TYPE_E event, hi_void *pPrivateData);

typedef struct hiHDMI_CALLBACK_FUNC_S {
    HI_HDMI_CallBack pfnHdmiEventCallback; /* Event handling callback function */
    hi_void *pPrivateData;                 /* Private data of the callback functions and parameters */
} HI_HDMI_CALLBACK_FUNC_S;

typedef struct hiHDMI_AUDIO_INFO_S {
    /* Audio coding type */
    HI_HDMI_AUDIO_FORMAT_CODE_E enAudFmtCode;
    /* Audio sample rate */
    HI_HDMI_SAMPLE_RATE_E enSupportSampleRate[HI_HDMI_MAX_SAMPE_RATE_NUM];
    /* Aud Channel of the coding type */
    hi_u8 u8AudChannel;
    /* sample bit depth, when audio format code is HI_UNF_EDID_AUDIO_FORMAT_CODE_PCM */
    HI_HDMI_BIT_DEPTH_E enSupportBitDepth[HI_HDMI_MAX_BIT_DEPTH_NUM];
    /* sample bit depth Num, when audio format code is HI_UNF_EDID_AUDIO_FORMAT_CODE_PCM */
    hi_u32 u32SupportBitDepthNum;
    /*
     * enter max bit rate, when audio format code is
     * HI_UNF_EDID_AUDIO_FORMAT_CODE_AC3 - HI_UNF_EDID_AUDIO_FORMAT_CODE_ATRAC
     */
    hi_u32 u32MaxBitRate;
} HI_HDMI_AUDIO_INFO_S;

typedef struct hiHDMI_EOTF_S {
    hi_bool bEotfSdr;         /* Traditional gamma - SDR Luminance Range. */
    hi_bool bEotfHdr;         /* Traditional gamma - HDR Luminance Range. */
    hi_bool bEotfSmpteSt2084; /* SMPTE ST 2084 */
    hi_bool bEotfHLG;         /* Hybrid Log-Gamma(HLG) based on Recommendation ITU-R BT.2100-0 */
    hi_bool bEotfFuture;      /* Future EOTF */
} HI_HDMI_EOTF_S;

typedef struct hiHDMI_HDR_METADATA_TYPE_S {
    hi_bool bDescriptorType1; /* Support Static Metadata Type 1 or not. */
} HI_HDMI_HDR_METADATA_TYPE_S;

typedef struct hiHDMI_HDR_CAP_S {
    /* EOTF support. */
    HI_HDMI_EOTF_S stEotf;
    /* static metadata descriptor type(ID) support. */
    HI_HDMI_HDR_METADATA_TYPE_S stMetadata;
    /* Desired Content Max Luminance Data. Real value(in 1cd/m^2) = 50.2^(CV/32) */
    hi_u8 u8MaxLuminance_CV;
    /* Desired Content Max Frame-average Luminance. Real value(in 1cd/m^2) = 50.2^(CV/32) */
    hi_u8 u8AverageLumin_CV;
    /*
     * Desired Content Min Luminance Data.
     * Real value(in 1cd/m^2) = u8MaxLuminance_CV * (u8MinLuminance_CV/255)^2 / 100
     */
    hi_u8 u8MinLuminance_CV;
} HI_HDMI_HDR_CAP_S;

typedef struct hiHDMI_TIMING_INFO_S {
    hi_u32 u32VFB;          /* vertical front blank */
    hi_u32 u32VBB;          /* vertical back blank */
    hi_u32 u32VACT;         /* vertical active area */
    hi_u32 u32HFB;          /* horizonal front blank */
    hi_u32 u32HBB;          /* horizonal back blank */
    hi_u32 u32HACT;         /* horizonal active area */
    hi_u32 u32VPW;          /* vertical sync pluse width */
    hi_u32 u32HPW;          /* horizonal sync pluse width */
    hi_bool bIDV;           /* flag of data valid signal is needed flip */
    hi_bool bIHS;           /* flag of horizonal sync pluse is needed flip */
    hi_bool bIVS;           /* flag of vertical sync pluse is needed flip */
    hi_u32 u32ImageWidth;   /* image width */
    hi_u32 u32ImageHeight;  /* image height */
    hi_u32 u32AspectRatioW; /* aspect ratio width */
    hi_u32 u32AspectRatioH; /* aspect ratio height */
    hi_bool bInterlace;     /* flag of interlace */
    hi_s32 u32PixelClk;     /* pixelc clk for this timing, unit:KHz */
} HI_HDMI_TIMING_INFO_S;

typedef struct hiHDMI_DET_TIMING_S {
    hi_u32 u32DetTimingNum;
    HI_HDMI_TIMING_INFO_S astDetTiming[HI_HDMI_DETAIL_TIMING_MAX];
} HI_HDMI_DET_TIMING_S;

typedef struct hiHDMI_SINK_CAPABILITY_S {
    /* Whether the devices are connected. */
    hi_bool bConnected;
    /* Whether the HDMI is supported by the device. If the HDMI is not supported by the device, the device is DVI. */
    hi_bool bSupportHdmi;
    /* Whether the sink device is powered on. */
    hi_bool bIsSinkPowerOn;
    /*
     * Whether the EDID obtains the flag from the sink device.
     * HI_TRUE: The EDID information is correctly read.
     * HI_FASE: default settings
     */
    hi_bool bIsRealEDID;
    /* Physical resolution of the display device. */
    HI_HDMI_VIDEO_FMT_E enNativeVideoFormat;
    /*
     * Video capability set.
     * HI_TRUE: This display format is supported.
     * HI_FALSE: This display format is not supported.
     */
    hi_bool bVideoFmtSupported[HI_HDMI_VIDEO_FMT_BUTT];
    /*
     * Whether the YCBCR display is supported.
     * HI_TRUE: The YCBCR display is supported.
     * HI_FALSE: Only red-green-blue (RGB) is supported.
     */
    hi_bool bSupportYCbCr;
    /* Whether the xvYCC601 color format is supported. */
    hi_bool bSupportxvYCC601;
    /* Whether the xvYCC709 color format is supported. */
    hi_bool bSupportxvYCC709;
    /* Transfer profile supported by xvYCC601. 1: P0; 2: P1; 4: P2. */
    hi_u8 u8MDBit;
    /* Num of audio Info */
    hi_u32 u32AudioInfoNum;
    /* Audio Info.For details, see Table 37 in EIA-CEA-861-D. */
    HI_HDMI_AUDIO_INFO_S stAudioInfo[HI_HDMI_MAX_AUDIO_CAP_COUNT];
    /* Speaker position. For details, see the definition of SpeakerDATABlock in EIA-CEA-861-D. */
    hi_bool bSpeaker[HDMI_AUDIO_SPEAKER_BUTT];
    /* Device vendor flag */
    hi_u8 u8IDManufactureName[4];
    /* Device ID. */
    hi_u32 u32IDProductCode;
    /* Device sequence number. */
    hi_u32 u32IDSerialNumber;
    /* Device production data (week). */
    hi_u32 u32WeekOfManufacture;
    /* Set the production data (year). */
    hi_u32 u32YearOfManufacture;
    /* Device version number */
    hi_u8 u8Version;
    /* Device sub version number */
    hi_u8 u8Revision;
    /* EDID extended block number */
    hi_u8 u8EDIDExternBlockNum;
    /* Valid flag of the consumer electronics control (CEC) physical address */
    hi_bool bIsPhyAddrValid;
    /* CEC physical address A */
    hi_u8 u8PhyAddr_A;
    /* CEC physical address B */
    hi_u8 u8PhyAddr_B;
    /* CEC physical address C */
    hi_u8 u8PhyAddr_C;
    /* CEC physical address D */
    hi_u8 u8PhyAddr_D;
    /* Whether to support the DVI dual-link operation. */
    hi_bool bSupportDVIDual;
    /* Whether to support the YCBCR 4:4:4 deep-color mode. */
    hi_bool bSupportDeepColorYCBCR444;
    /* Whether to support the deep-color 30-bit mode. */
    hi_bool bSupportDeepColor30Bit;
    /* Whether to support the deep-color 36-bit mode. */
    hi_bool bSupportDeepColor36Bit;
    /* Whether to support the deep-color 48-bit mode. */
    hi_bool bSupportDeepColor48Bit;
    /* Whether to support the Supports_AI mode. */
    hi_bool bSupportAI;
    /* Maximum TMDS clock. */
    hi_u32 u32MaxTMDSClock;
    /* Delay flag bit. */
    hi_bool bI_Latency_Fields_Present;
    /* whether Video_Latency and Audio_Latency fields are present */
    hi_bool bLatency_Fields_Present;
    /* Special video format */
    hi_bool bHDMI_Video_Present;
    /* Video delay */
    hi_u8 u8Video_Latency;
    /* Audio delay */
    hi_u8 u8Audio_Latency;
    /* Video delay in interlaced video mode */
    hi_u8 u8Interlaced_Video_Latency;
    /* Audio delay in interlaced video mode */
    hi_u8 u8Interlaced_Audio_Latency;
    /* Whether to support the YUV420 deep-color 30-bit mode. */
    hi_bool bSupportY420DC30Bit;
    /* Whether to support the YUV420 deep-color 36-bit mode. */
    hi_bool bSupportY420DC36Bit;
    /* Whether to support the YUV420 deep-color 48-bit mode. */
    hi_bool bSupportY420DC48Bit;
    /* Whether to support HDMI2.0. */
    hi_bool bSupportHdmi_2_0;
    /* the format support YUV420 */
    hi_bool bSupportY420Format[HI_HDMI_VIDEO_FMT_BUTT];
    /* the format only support YUV420 */
    hi_bool bOnlySupportY420Format[HI_HDMI_VIDEO_FMT_BUTT];
    /*
     * flag of RGB Quantization Range selectable.
     * when HI_TRUE,sink expect full/limited range is selectable
     * or sink expect default range.
     */
    hi_bool bYccQrangeSelectable;
    /*
     * flag of YCC Quantization Range selectable.
     * when HI_TRUE,sink expect full/limited range is selectable;or sink expect default range.
     */
    hi_bool bRgbQrangeSelectable;
    /* HDR support flag.When HDR Static Metadata Data Block is indicated,flag HI_TRUE;or,HI_FALSE. */
    hi_bool bHdrSupport;
    /* HDR capability */
    HI_HDMI_HDR_CAP_S stHdr;
    /* Detailed timing infomation */
    HI_HDMI_DET_TIMING_S stDetailedTiming;
} HI_HDMI_SINK_CAPABILITY_S;

typedef struct hiHDMI_ATTR_S {
    /* Whether to forcibly output the video over the HDMI. */
    hi_bool bEnableHdmi;
    /*
     * Whether to output video.The value must be HI_TRUE.
     * If the value is HI_FALSE, the HDMI is forcibly set to HI_TRUE.
     */
    hi_bool bEnableVideo;
    /* Video norm. This value of the video norm must be consistent with the norm of the video output. */
    HI_HDMI_VIDEO_FMT_E enVideoFmt;
    /* HDMI input video input mode.VIDEO_MODE_YCBCR444,VIDEO_MODE_YCBCR422,VIDEO_MODE_YCBCR420,VIDEO_MODE_RGB444 */
    HI_HDMI_VIDEO_MODE_E enVidInMode;
    /* HDMI output video output mode.VIDEO_MODE_YCBCR444,VIDEO_MODE_YCBCR422,VIDEO_MODE_YCBCR420,VIDEO_MODE_RGB444 */
    HI_HDMI_VIDEO_MODE_E enVidOutMode;
    /* CSC output quantization range HDMI_QUANTIZATION_RANGE_LIMITED, HDMI_QUANTIZATION_RANGE_FULL>**/
    HI_HDMI_QUANTIZATION_E enOutCscQuantization;
    /* DeepColor output mode. It is HI_HDMI_DEEP_COLOR_24BIT by default. */
    HI_HDMI_DEEP_COLOR_E enDeepColorMode;
    /* Whether to enable the xvYCC output mode. It is HI_FALSE by default. */
    hi_bool bxvYCCMode;
    /* Whether to enable the audio. */
    hi_bool bEnableAudio;
    /* HDMI audio source. It is set to HI_HDMI_SND_INTERFACE_I2S, which needs to be consistent with the VO interface. */
    HI_HDMI_SND_INTERFACE_E enSoundIntf;
    /* Multi-channel or stereo 0: stereo 1: eight-channel fixed for multi-channel */
    hi_bool bIsMultiChannel;
    /* Audio sampling rate. This parameter needs to be consistent with that of the VO. */
    HI_HDMI_SAMPLE_RATE_E enSampleRate;
    /* Audio down sampling rate parameter. It is 0 by default. */
    hi_u8 u8DownSampleParm;
    /* Audio bit width. It is 16 by default. This parameter needs to be consistent with that of the VO. */
    HI_HDMI_BIT_DEPTH_E enBitDepth;
    /* Reserved. It is set to 0. I2S control (0x7A:0x1D) */
    hi_u8 u8I2SCtlVbit;
    /* Whether to enable AVI InfoFrame. It is recommended to enable this function. */
    hi_bool bEnableAviInfoFrame;
    /* Whether to enable AUDIO InfoFrame. It is recommended to enable this function. */
    hi_bool bEnableAudInfoFrame;
    /* Whether to enable SPD InfoFrame. It is recommended to disable this function. */
    hi_bool bEnableSpdInfoFrame;
    /* Whether to enable MPEG InfoFrame. It is recommended to disable this function. */
    hi_bool bEnableMpegInfoFrame;
    /* Whether to enable the debug information in the HDMI. It is recommended to disable this function. */
    hi_bool bDebugFlag;
    /* Whether to enable the HDCP.0: disabled 1: enabled */
    hi_bool bHDCPEnable;
    /* Whether to enable 3D mode. 0: disabled 1: enabled */
    hi_bool b3DEnable;
    /* 3D Parameter. The default value is 9. */
    hi_u8 u83DParam;
    /* When get capability fail, HDMI forcily(priority) judgments output video mode. Default is HI_HDMI_FORCE_HDMI. */
    HI_HDMI_FORCE_ACTION_E enDefaultMode;
    /* Whether to enable auth mode. 0: disabled 1: enabled */
    hi_bool bAuthMode;
    /*
     * Enable flag of vedio mode & DVI adapting case of user setting incorrect, default:HI_TRUE.
     * When user have no any adapting strategy, suggestion HI_TRUE
     */
    hi_bool bEnableVidModeAdapt;
    /*
     * Enable flag of deep color mode adapting case of user setting incorrect, default: HI_FALSE.
     * When user have no any adapting strategy, suggestion HI_TRUE
     */
    hi_bool bEnableDeepClrAdapt;
    /*
     * Pixclk of enVideoFmt(unit is kHz).
     * (This param is valid only when enVideoFmt is HI_HDMI_VIDEO_FMT_VESA_CUSTOMER_DEFINE)
     */
    hi_u32 u32PixClk;
} HI_HDMI_ATTR_S;

typedef struct hiHI_HDMI_EDID_S {
    hi_bool bEdidValid;
    hi_u32 u32Edidlength;
    hi_u8 u8Edid[HI_HDMI_EDID_RAW_DATA_LEN];
} HI_HDMI_EDID_S;

typedef enum hiHDMI_INFOFRAME_TYPE_E {
    HI_INFOFRAME_TYPE_AVI,        /* HDMI AVI InfoFrame */
    HI_INFOFRAME_TYPE_SPD,        /* HDMI SPD InfoFrame */
    HI_INFOFRAME_TYPE_AUDIO,      /* HDMI AUDIO InfoFrame */
    HI_INFOFRAME_TYPE_MPEG,       /* HDMI MPEG InfoFrame */
    HI_INFOFRAME_TYPE_VENDORSPEC, /* HDMI Vendor Specific InfoFrame */
    HI_INFOFRAME_TYPE_BUTT
} HI_HDMI_INFOFRAME_TYPE_E;

typedef struct hi_HDMI_AVI_INFOFRAME_VER2_S {
    HI_HDMI_VIDEO_FMT_E        enTimingMode;
    HI_HDMI_COLOR_SPACE_E      enColorSpace;
    hi_bool                    bActive_Infor_Present;
    HI_HDMI_BARINFO_E          enBarInfo;
    HI_HDMI_SCANINFO_E         enScanInfo;
    HI_HDMI_COLORIMETRY_E      enColorimetry;
    HI_HDMI_EXT_COLORIMETRY_E  enExtColorimetry;
    HI_HDMI_PIC_ASPECT_RATIO_E enAspectRatio;
    HI_HDMI_ACT_ASPECT_RATIO_E enActiveAspectRatio;
    HI_HDMI_PICTURE_SCALING_E  enPictureScaling;
    HI_HDMI_RGB_QUAN_RAGE_E    enRGBQuantization;
    hi_bool                    bIsITContent;
    HI_HDMI_PIXEL_REPETITION_E enPixelRepetition;
    HI_HDMI_CONTENT_TYPE_E     enContentType;
    HI_HDMI_YCC_QUAN_RAGE_E    enYCCQuantization;
    hi_u16                     u16LineNEndofTopBar;
    hi_u16                     u16LineNStartofBotBar;
    hi_u16                     u16PixelNEndofLeftBar;
    hi_u16                     u16PixelNStartofRightBar;
} HI_HDMI_AVI_INFOFRAME_VER2_S;

typedef enum hiHDMI_AUDIO_CHANEL_CNT_E {
    HI_HDMI_AUDIO_CHANEL_CNT_STREAM,
    HI_HDMI_AUDIO_CHANEL_CNT_2,
    HI_HDMI_AUDIO_CHANEL_CNT_3,
    HI_HDMI_AUDIO_CHANEL_CNT_4,
    HI_HDMI_AUDIO_CHANEL_CNT_5,
    HI_HDMI_AUDIO_CHANEL_CNT_6,
    HI_HDMI_AUDIO_CHANEL_CNT_7,
    HI_HDMI_AUDIO_CHANEL_CNT_8
} HI_HDMI_AUDIO_CHANEL_CNT_E;

typedef enum hiHDMI_AUDIO_SAMPLE_SIZE_E {
    HI_HDMI_AUDIO_SAMPLE_SIZE_STREAM,
    HI_HDMI_AUDIO_SAMPLE_SIZE_16,
    HI_HDMI_AUDIO_SAMPLE_SIZE_20,
    HI_HDMI_AUDIO_SAMPLE_SIZE_24
} HI_HDMI_AUDIO_SAMPLE_SIZE_E;

typedef enum hiHDMI_AUDIO_SAMPLE_FREQ_E {
    HI_HDMI_AUDIO_SAMPLE_FREQ_STREAM,
    HI_HDMI_AUDIO_SAMPLE_FREQ_32000,
    HI_HDMI_AUDIO_SAMPLE_FREQ_44100,
    HI_HDMI_AUDIO_SAMPLE_FREQ_48000,
    HI_HDMI_AUDIO_SAMPLE_FREQ_88200,
    HI_HDMI_AUDIO_SAMPLE_FREQ_96000,
    HI_HDMI_AUDIO_SAMPLE_FREQ_176400,
    HI_HDMI_AUDIO_SAMPLE_FREQ_192000
} HI_HDMI_AUDIO_SAMPLE_FREQ_E;

typedef enum hiHDMI_LEVEL_SHIFT_VALUE_E {
    HI_HDMI_LEVEL_SHIFT_VALUE_0_DB,
    HI_HDMI_LEVEL_SHIFT_VALUE_1_DB,
    HI_HDMI_LEVEL_SHIFT_VALUE_2_DB,
    HI_HDMI_LEVEL_SHIFT_VALUE_3_DB,
    HI_HDMI_LEVEL_SHIFT_VALUE_4_DB,
    HI_HDMI_LEVEL_SHIFT_VALUE_5_DB,
    HI_HDMI_LEVEL_SHIFT_VALUE_6_DB,
    HI_HDMI_LEVEL_SHIFT_VALUE_7_DB,
    HI_HDMI_LEVEL_SHIFT_VALUE_8_DB,
    HI_HDMI_LEVEL_SHIFT_VALUE_9_DB,
    HI_HDMI_LEVEL_SHIFT_VALUE_10_DB,
    HI_HDMI_LEVEL_SHIFT_VALUE_11_DB,
    HI_HDMI_LEVEL_SHIFT_VALUE_12_DB,
    HI_HDMI_LEVEL_SHIFT_VALUE_13_DB,
    HI_HDMI_LEVEL_SHIFT_VALUE_14_DB,
    HI_HDMI_LEVEL_SHIFT_VALUE_15_DB
} HI_HDMI_LEVEL_SHIFT_VALUE_E;

typedef enum hiHDMI_LFE_PLAYBACK_LEVEL_E {
    HI_HDMI_LFE_PLAYBACK_NO,
    HI_HDMI_LFE_PLAYBACK_0_DB,
    HI_HDMI_LFE_PLAYBACK_10_DB,
    HI_HDMI_LFE_PLAYBACK_RESEVED
} HI_HDMI_LFE_PLAYBACK_LEVEL_E;

typedef struct hiHDMI_AUD_INFOFRAME_VER1_S {
    HI_HDMI_AUDIO_CHANEL_CNT_E   enChannelCount;
    HI_HDMI_CODING_TYPE_E        enCodingType;
    HI_HDMI_AUDIO_SAMPLE_SIZE_E  enSampleSize;
    HI_HDMI_AUDIO_SAMPLE_FREQ_E  enSamplingFrequency;
    hi_u8                        u8ChannelAlloc; /* Channel/Speaker Allocation.Range [0,255] */
    HI_HDMI_LEVEL_SHIFT_VALUE_E  enLevelShift;
    HI_HDMI_LFE_PLAYBACK_LEVEL_E enLfePlaybackLevel;
    hi_bool                      bDownmixInhibit;
} HI_HDMI_AUD_INFOFRAME_VER1_S;

typedef struct hiHDMI_SPD_INFOFRAME_S {
    hi_u8 u8VendorName[8];
    hi_u8 u8ProductDescription[16];
} HI_HDMI_SPD_INFOFRAME_S;

typedef struct hiHDMI_MPEGSOURCE_INFOFRAME_S {
    hi_u32  u32MPEGBitRate;
    hi_bool bIsFieldRepeated;
} HI_HDMI_MPEGSOURCE_INFOFRAME_S;

typedef struct hiHDMI_VENDORSPEC_INFOFRAME_S {
    hi_u8 u8DataLen;
    hi_u8 u8UserData[HI_HDMI_VENDOR_USER_DATA_MAX_LEN];
} HI_HDMI_VENDORSPEC_INFOFRAME_S;

typedef union hiHDMI_INFOFRAME_UNIT_U {
    HI_HDMI_AVI_INFOFRAME_VER2_S   stAVIInfoFrame;
    HI_HDMI_AUD_INFOFRAME_VER1_S   stAUDInfoFrame;
    HI_HDMI_SPD_INFOFRAME_S        stSPDInfoFrame;
    HI_HDMI_MPEGSOURCE_INFOFRAME_S stMPEGSourceInfoFrame;
    HI_HDMI_VENDORSPEC_INFOFRAME_S stVendorSpecInfoFrame;
} HI_HMDI_INFORFRAME_UNIT_U;

typedef struct hiUNF_HDMI_INFOFRAME_S {
    HI_HDMI_INFOFRAME_TYPE_E  enInfoFrameType; /* InfoFrame type */
    HI_HMDI_INFORFRAME_UNIT_U unInforUnit;     /* InfoFrame date */
} HI_HDMI_INFOFRAME_S;

/*
 * CEC's Opcode: Please refer to CEC 15 Message Descriptions
 * General Protocol messages
 * Feature Abort> Used as a response to indicate that the device does not support the requested message type,
 * or that it cannot execute it at the present time.
 */
#define CEC_OPCODE_FEATURE_ABORT 0x00

/* <Abort> Message This message is reserved for testing purposes. */
#define CEC_OPCODE_ABORT_MESSAGE 0xFF

/*
 * One Touch Play Feature
 * <Active Source> Used by a new source to indicate that
 * it has started to transmit a stream OR used in response to a <Request Active Source>
 */
#define CEC_OPCODE_ACTIVE_SOURCE 0x82

/*
 * <Image View On> Sent by a source device to the TV
 * whenever it enters the active state (alternatively it may send <Text View On>).
 */
#define CEC_OPCODE_IMAGE_VIEW_ON 0x04

/*
 * <Text View On> As <Image View On>,
 * but should also remove any text, menus and PIP windows from the TV's display.
 */
#define CEC_OPCODE_TEXT_VIEW_ON 0x0D

/*
 * Routing Control Feature
 * <Inactive Source> Used by the currently active source to inform the TV
 * that it has no video to be presented to the user, or is going into standby
 * as the result of a local user command on the device.
 */
#define CEC_OPCODE_INACTIVE_SOURCE 0x9D

/* <Request Active Source> Used by a new device to discover the status of the system. */
#define CEC_OPCODE_REQUEST_ACTIVE_SOURCE 0x85

/*
 * <Routing Change> Sent by a CEC Switch when it is manually switched to
 * inform all other devices on the network that the active route below the switch has changed.
 */
#define CEC_OPCODE_ROUTING_CHANGE 0x80

/* <Routing Information> Sent by a CEC Switch to indicate the active route below the switch. */
#define CEC_OPCODE_ROUTING_INFORMATION 0x81

/* <Set Stream Path> Used by the TV to request a streaming path from the specified physical address. */
#define CEC_OPCODE_SET_STREAM_PATH 0x86

/*
 * <Standby> Switches one or all devices into standby mode.
 * Can be used as a broadcast message or be addressed to a specific device.
 * See section CEC 13.3 for important notes on the use of this message
 */
#define CEC_OPCODE_STANDBY 0x36

/* <Record Off> Requests a device to stop a recording. */
#define CEC_OPCODE_RECORD_OFF 0x0B

/* <Record On> Attempt to record the specified source. */
#define CEC_OPCODE_RECORD_ON 0x09

/* <Record Status> Used by a Recording Device to inform the initiator of the message <Record On> about its status. */
#define CEC_OPCODE_RECORD_STATUS 0x0A

/* <Record TV Screen> Request by the Recording Device to record the presently displayed source. */
#define CEC_OPCODE_RECORD_TV_SCREEN 0x0F

/* <Clear Analogue Timer> Used to clear an Analogue timer block of a device. */
#define CEC_OPCODE_CLEAR_ANALOGUE_TIMER 0x33

/* <Clear Digital Timer> Used to clear a Digital timer block of a device. */
#define CEC_OPCODE_CLEAR_DIGITAL_TIMER 0x99

/* <Clear External Timer> Used to clear an External timer block of a device. */
#define CEC_OPCODE_CLEAR_EXTERNAL_TIMER 0xA1

/* <Set Analogue Timer> Used to set a single timer block on an Analogue Recording Device. */
#define CEC_OPCODE_SET_ANALOGUE_TIMER 0x34

/* <Set Digital Timer> Used to set a single timer block on a Digital Recording Device. */
#define CEC_OPCODE_SET_DIGITAL_TIMER 0x97

/* <Set External Timer> Used to set a single timer block to record from an external device. */
#define CEC_OPCODE_SET_EXTERNAL_TIMER 0xA2

/*
 * <Set Timer Program Title> Used to set the name of a program associated with
 * a timer block. Sent directly after sending a <Set Analogue Timer> or <Set Digital Timer> message.
 * The name is then associated with that timer block.
 */
#define CEC_OPCODE_SET_TIMER_PROGRAM_TITLE 0x67

/*
 * <Timer Cleared Status> Used to give the status of a <Clear Analogue Timer>,
 * <Clear Digital Timer> or <Clear External Timer> message.
 */
#define CEC_OPCODE_TIMER_CLEARED_STATUS 0x43

/* <Timer Status> Used to send timer status to the initiator of a <Set Timer> message. */
#define CEC_OPCODE_TIMER_STATUS 0x35

/* <CEC Version> Used to indicate the supported CEC version, in response to a <Get CEC Version> */
#define CEC_OPCODE_CEC_VERSION 0x9E

/* <Get CEC Version> Used by a device to enquire which version of CEC the target supports */
#define CEC_OPCODE_GET_CEC_VERSION 0x9F

/* <Give Physical Address> A request to a device to return its physical address. */
#define CEC_OPCODE_GIVE_PHYSICAL_ADDRESS 0x83

/*
 * <Report Physical Address> Used to inform all other devices of the mapping between
 * physical and logical address of the initiator.
 */
#define CEC_OPCODE_REPORT_PHYSICAL_ADDRESS 0x84

/*
 * Get Menu Language> Sent by a device capable of character generation (for OSD and Menus) to
 * a TV in order to discover the currently selected Menu language.
 * Also used by a TV during installation to discover the currently set menu language of other devices.
 */
#define CEC_OPCODE_GET_MENU_LANGUAGE 0x91

/* <Set Menu Language> Used by a TV or another device to indicate the menu language. */
#define CEC_OPCODE_SET_MENU_LANGUAGE 0x32

/* <Deck Control> Used to control a device's media functions. */
#define CEC_OPCODE_DECK_CONTROL 0x42
/* <Deck Status> Used to provide a deck's status to the initiator of the <Give Deck Status> message. */
#define CEC_OPCODE_DECK_STATUS 0x1B
/*
 * <Give Deck Status> Used to request the status of a device,
 * regardless of whether or not it is the current active source.
 */
#define CEC_OPCODE_GIVE_DECK_STATUS 0x1A

/* <Play> Used to control the playback behaviour of a source device. */
#define CEC_OPCODE_PLAY 0x41

/* <Give Tuner Device Status> Used to request the status of a tuner device. */
#define CEC_OPCODE_GIVE_TUNER_DEVICE_STATUS 0x08

/* <Select Analogue Service> Directly selects an Analogue TV service */
#define CEC_OPCODE_SELECT_ANALOGUE_SERVICE 0x92

/* <Select Digital Service> Directly selects a Digital TV, Radio or Data Broadcast Service */
#define CEC_OPCODE_SELECT_DIGITAL_SERVICE 0x93

/*
 * <Tuner Device Status> Use by a tuner device to provide its status to
 * the initiator of the <Give Tuner Device Status> message.
 */
#define CEC_OPCODE_TUNER_DEVICE_STATUS 0x07

/* <Tuner Step Decrement> Used to tune to next lowest service in a tuner's service list. Can be used for PIP. */
#define CEC_OPCODE_TUNER_STEP_DECREMENT 0x06

/*
 * <Tuner Step Increment> Used to tune to next highest
 * service in a tuner's service list. Can be used for PIP.
 */
#define CEC_OPCODE_TUNER_STEP_INCREMENT 0x05

/* <Device Vendor ID> Reports the vendor ID of this device. */
#define CEC_OPCODE_DEVICE_VENDOR_ID 0x87

/* <Give Device Vendor ID> Requests the Vendor ID from a device. */
#define CEC_OPCODE_GIVE_DEVICE_VENDOR_ID 0x8C

/* <Vendor Command> Allows vendor specific commands to be sent between two devices. */
#define CEC_OPCODE_VENDOR_COMMAND 0x89

/* <Vendor Command With ID> Allows vendor specific commands to be sent between two devices or broadcast. */
#define CEC_OPCODE_VENDOR_COMMAND_WITH_ID 0xA0

/* <Vendor Remote Button Down> Indicates that a remote control button has been depressed. */
#define CEC_OPCODE_VENDOR_REMOTE_BUTTON_DOWN 0x8A

/*
 * <Vendor Remote Button Up> Indicates that a remote control button
 * (the last button pressed indicated by the Vendor Remote Button Down message) has been released.
 */
#define CEC_OPCODE_VENDOR_REMOTE_BUTTON_UP 0x8B

/* <Set OSD String> Used to send a text message to output on a TV. */
#define CEC_OPCODE_SET_OSD_STRING 0x64

/* <Give OSD Name> Used to request the preferred OSD name of a device for use in menus associated with that device. */
#define CEC_OPCODE_GIVE_OSD_NAME 0x46

/* <Set OSD Name> Used to set the preferred OSD name of a device for use in menus associated with that device. */
#define CEC_OPCODE_SET_OSD_NAME 0x47

/*
 * <Menu Request> A request from the TV for a device
 * to show/remove a menu or to query if a device is currently showing a menu.
 */
#define CEC_OPCODE_MENU_REQUEST 0x8D

/*
 * <Menu Status> Used to indicate to the TV that the device
 * is showing/has removed a menu and requests the remote control keys to be passed though.
 */
#define CEC_OPCODE_MENU_STATUS 0x8E

/*
 * <User Control Pressed> Used to indicate that the user
 * pressed a remote control button or switched from one remote control button to another.
 */
#define CEC_OPCODE_USER_CONTROL_PRESSED 0x44

/*
 * <User Control Released> Indicates that user released a remote control button
 * (the last one indicated by the <User Control Pressed> message)
 */
#define CEC_OPCODE_USER_CONTROL_RELEASED 0x45

/* <Give Device Power Status> Used to determine the current power status of a target device */
#define CEC_OPCODE_GIVE_DEVICE_POWER_STATUS 0x8F

/* <Report Power Status> Used to inform a requesting device of the current power status */
#define CEC_OPCODE_REPORT_POWER_STATUS 0x90

/* <Give Audio Status> Requests an amplifier to send its volume and mute status */
#define CEC_OPCODE_GIVE_AUDIO_STATUS 0x71

/* <Give System Audio Mode Status> Requests the status of the System Audio Mode */
#define CEC_OPCODE_GIVE_SYSTEM_AUDIO_MODE_STATUS 0x7D

/* <Report Audio Status> Reports an amplifier's volume and mute status */
#define CEC_OPCODE_REPORT_AUDIO_STATUS 0x7A

/* <Set System Audio Mode> Turns the System Audio Mode On or Off. */
#define CEC_OPCODE_SET_SYSTEM_AUDIO_MODE 0x72

/*
 * <System Audio Mode Request> A device implementing System Audio Control and
 * which has volume control RC buttons (eg TV or STB) requests to use System Audio Mode to the amplifier.
 */
#define CEC_OPCODE_SYSTEM_AUDIO_MODE_REQUEST 0x70

/* <System Audio Mode Status> Reports the current status of the System Audio Mode */
#define CEC_OPCODE_SYSTEM_AUDIO_MODE_STATUS 0x7E

/* <Set Audio Rate> Used to control audio rate from Source Device. */
#define CEC_OPCODE_SET_AUDIO_RATE 0x9A

/* POLL message have no opcode, So, we just use this value. */
#define CEC_OPCODE_POLLING_MESSAGE 0xFE

typedef enum hiUNF_CEC_LOGICALADD_E {
    HI_CEC_LOGICALADD_TV,
    HI_CEC_LOGICALADD_RECORDDEV_1,
    HI_CEC_LOGICALADD_RECORDDEV_2,
    HI_CEC_LOGICALADD_TUNER_1,
    HI_CEC_LOGICALADD_PLAYDEV_1,
    HI_CEC_LOGICALADD_AUDIOSYSTEM,
    HI_CEC_LOGICALADD_TUNER_2,
    HI_CEC_LOGICALADD_TUNER_3,
    HI_CEC_LOGICALADD_PLAYDEV_2,
    HI_CEC_LOGICALADD_RECORDDEV_3,
    HI_CEC_LOGICALADD_TUNER_4,
    HI_CEC_LOGICALADD_PLAYDEV_3,
    HI_CEC_LOGICALADD_RESERVED_1,
    HI_CEC_LOGICALADD_RESERVED_2,
    HI_CEC_LOGICALADD_SPECIALUSE,
    HI_CEC_LOGICALADD_BROADCAST,
    HI_CEC_LOGICALADD_BUTT
} HI_CEC_LOGICALADD_E;

typedef enum hiUNF_CEC_CMDTYPE_E {
    HI_CEC_STRUCTCOMMAND,
    HI_CEC_RAWCOMMAND,
    HI_CEC_BUTT
} HI_CEC_CMDTYPE_E;

typedef struct hiUNF_CEC_RAWDATA_S {
    hi_u8 u8Length;
    hi_u8 u8Data[15];
} HI_CEC_RAWDATA_S;

typedef enum hiUNF_CEC_UICMD_E {
    HI_CEC_UICMD_SELECT,
    HI_CEC_UICMD_UP,
    HI_CEC_UICMD_DOWN,
    HI_CEC_UICMD_LEFT,
    HI_CEC_UICMD_RIGHT,
    HI_CEC_UICMD_RIGHT_UP,
    HI_CEC_UICMD_RIGHT_DOWN,
    HI_CEC_UICMD_LEFT_UP,
    HI_CEC_UICMD_LEFT_DOWN,
    HI_CEC_UICMD_ROOT_MENU,
    HI_CEC_UICMD_SETUP_MENU,
    HI_CEC_UICMD_CONTENTS_MENU,
    HI_CEC_UICMD_FAVORITE_MENU,
    HI_CEC_UICMD_EXIT,
    HI_CEC_UICMD_NUM_0 = 0x20,
    HI_CEC_UICMD_NUM_1,
    HI_CEC_UICMD_NUM_2,
    HI_CEC_UICMD_NUM_3,
    HI_CEC_UICMD_NUM_4,
    HI_CEC_UICMD_NUM_5,
    HI_CEC_UICMD_NUM_6,
    HI_CEC_UICMD_NUM_7,
    HI_CEC_UICMD_NUM_8,
    HI_CEC_UICMD_NUM_9,
    HI_CEC_UICMD_DOT,
    HI_CEC_UICMD_ENTER,
    HI_CEC_UICMD_CLEAR,
    HI_CEC_UICMD_NEXT_FAVORITE = 0x2F,
    HI_CEC_UICMD_CHANNEL_UP,
    HI_CEC_UICMD_CHANNEL_DOWN,
    HI_CEC_UICMD_PREVIOUS_CHANNEL,
    HI_CEC_UICMD_SOUND_SELECT,
    HI_CEC_UICMD_INPUT_SELECT,
    HI_CEC_UICMD_DISPLAY_INFORMATION,
    HI_CEC_UICMD_HELP,
    HI_CEC_UICMD_PAGE_UP,
    HI_CEC_UICMD_PAGE_DOWN,
    HI_CEC_UICMD_POWER = 0x40,
    HI_CEC_UICMD_VOLUME_UP,
    HI_CEC_UICMD_VOLUME_DOWN,
    HI_CEC_UICMD_MUTE,
    HI_CEC_UICMD_PLAY,
    HI_CEC_UICMD_STOP,
    HI_CEC_UICMD_PAUSE,
    HI_CEC_UICMD_RECORD,
    HI_CEC_UICMD_REWIND,
    HI_CEC_UICMD_FAST_FORWARD,
    HI_CEC_UICMD_EJECT,
    HI_CEC_UICMD_FORWARD,
    HI_CEC_UICMD_BACKWARD,
    HI_CEC_UICMD_STOP_RECORD,
    HI_CEC_UICMD_PAUSE_RECORD,
    HI_CEC_UICMD_ANGLE = 0x50,
    HI_CEC_UICMD_SUBPICTURE,
    HI_CEC_UICMD_VIDEO_ON_DEMAND,
    HI_CEC_UICMD_ELECTRONIC_PROGRAM_GUIDE,
    HI_CEC_UICMD_TIMER_PROGRAMMING,
    HI_CEC_UICMD_INITIAL_CONFIGURATION,
    HI_CEC_UICMD_PLAY_FUNCTION = 0x60,
    HI_CEC_UICMD_PAUSE_PLAY_FUNCTION,
    HI_CEC_UICMD_RECORD_FUNCTION,
    HI_CEC_UICMD_PAUSE_RECORD_FUNCTION,
    HI_CEC_UICMD_STOP_FUNCTION,
    HI_CEC_UICMD_MUTE_FUNCTION,
    HI_CEC_UICMD_RESTORE_VOLUME_FUNCTION,
    HI_CEC_UICMD_TUNE_FUNCTION,
    HI_CEC_UICMD_SELECT_MEDIA_FUNCTION,
    HI_CEC_UICMD_SELECT_AV_INPUT_FUNCTION,
    HI_CEC_UICMD_SELECT_AUDIO_INPUT_FUNCTION,
    HI_CEC_UICMD_POWER_TOGGLE_FUNCTION,
    HI_CEC_UICMD_POWER_OFF_FUNCTION,
    HI_CEC_UICMD_POWER_ON_FUNCTION,
    HI_CEC_UICMD_F1_BLUE = 0x71,
    HI_CEC_UICMD_F2_RED,
    HI_CEC_UICMD_F3_GREEN,
    HI_CEC_UICMD_F4_YELLOW,
    HI_CEC_UICMD_F5,
    HI_CEC_UICMD_DATA
} HI_CEC_UICMD_E; /* User Control Code */

typedef struct hiUNF_HDMI_CEC_CMD_S {
    HI_CEC_LOGICALADD_E enSrcAdd;
    HI_CEC_LOGICALADD_E enDstAdd;
    hi_u8               u8Opcode;
    HI_CEC_RAWDATA_S    stRawData;
} HI_HDMI_CEC_CMD_S;

typedef struct hiUNF_HDMI_CEC_STATUS_S {
    hi_bool bEnable;
    hi_u8 u8PhysicalAddr[4];                 /* CEC physic addr */
    hi_u8 u8LogicalAddr;                     /* CEC logic addr, default:0x03. */
    hi_u8 u8Network[HI_CEC_LOGICALADD_BUTT]; /* CEC's net work, 1:this device can answer cec command */
} HI_HDMI_CEC_STATUS_S;

typedef hi_void (*HI_HDMI_CECCALLBACK)(HI_HDMI_ID_E enHdmi, HI_HDMI_CEC_CMD_S *pstCECCmd, hi_void *pData);

typedef struct hiHDMI_CECCALLBACK_FUNC_S {
    HI_HDMI_CECCALLBACK pfnCecCallback; /* Event handling callback function */
    hi_void *pPrivateData;              /* Private data of the CEC callback functions and parameters */
} HI_HDMI_CECCALLBACK_FUNC_S;

typedef enum hiHDMI_TRACE_LEN_E {
    HI_HDMI_TRACE_LEN_0,
    HI_HDMI_TRACE_LEN_1,
    HI_HDMI_TRACE_LEN_2,
    HI_HDMI_TRACE_LEN_BUTT
} HI_HDMI_TRACE_LEN_E;

typedef struct hiHDMI_MOD_PARAM_S {
    HI_HDMI_TRACE_LEN_E enTraceLen; /* Phy parameters selection. */
} HI_HDMI_MOD_PARAM_S;

typedef enum hiERR_HDMI_CODE_E {
    ERR_HDMI_NOT_INIT = 1,
    ERR_HDMI_INVALID_PARA,
    ERR_HDMI_NUL_PTR,
    ERR_HDMI_DEV_NOT_OPEN,
    ERR_HDMI_DEV_NOT_CONNECT,
    ERR_HDMI_READ_SINK_FAILED,
    ERR_HDMI_INIT_ALREADY,
    ERR_HDMI_CALLBACK_ALREADY,
    ERR_HDMI_INVALID_CALLBACK,
    ERR_HDMI_FEATURE_NO_SUPPORT,
    ERR_HDMI_BUS_BUSY,
    ERR_HDMI_READ_EVENT_FAILED,
    ERR_HDMI_NOT_START,
    ERR_HDMI_READ_EDID_FAILED,
    ERR_HDMI_INIT_FAILED,
    ERR_HDMI_CREATE_TESK_FAILED,
    ERR_HDMI_MALLOC_FAILED,
    ERR_HDMI_FREE_FAILED,
    ERR_HDMI_PTHREAD_CREATE_FAILED,
    ERR_HDMI_PTHREAD_JOIN_FAILED,
    ERR_HDMI_STRATEGY_FAILED,
    ERR_HDMI_SET_ATTR_FAILED,
    ERR_HDMI_CALLBACK_NOT_REGISTER,
    ERR_HDMI_CEC_CALLBACK_REREGISTER,
    ERR_HDMI_UNKNOWN_COMMAND,
    ERR_HDMI_MUTEX_LOCK_FAILED,
    ERR_HDMI_CEC_NOT_ENABLE,
    ERR_HDMI_CECCALLBACK_NOT_REG,
    ERR_HDMI_BUTT
} ERR_HDMI_CODE_E;

#define HI_ERR_HDMI_NOT_INIT                HI_DEF_ERR(HI_ID_HDMI, EN_ERR_LEVEL_ERROR, ERR_HDMI_NOT_INIT)
#define HI_ERR_HDMI_INVALID_PARA            HI_DEF_ERR(HI_ID_HDMI, EN_ERR_LEVEL_ERROR, ERR_HDMI_INVALID_PARA)
#define HI_ERR_HDMI_NUL_PTR                 HI_DEF_ERR(HI_ID_HDMI, EN_ERR_LEVEL_ERROR, ERR_HDMI_NUL_PTR)
#define HI_ERR_HDMI_DEV_NOT_OPEN            HI_DEF_ERR(HI_ID_HDMI, EN_ERR_LEVEL_ERROR, ERR_HDMI_DEV_NOT_OPEN)
#define HI_ERR_HDMI_DEV_NOT_CONNECT         HI_DEF_ERR(HI_ID_HDMI, EN_ERR_LEVEL_ERROR, ERR_HDMI_DEV_NOT_CONNECT)
#define HI_ERR_HDMI_READ_SINK_FAILED        HI_DEF_ERR(HI_ID_HDMI, EN_ERR_LEVEL_ERROR, ERR_HDMI_READ_SINK_FAILED)
#define HI_ERR_HDMI_INIT_ALREADY            HI_DEF_ERR(HI_ID_HDMI, EN_ERR_LEVEL_ERROR, ERR_HDMI_INIT_ALREADY)
#define HI_ERR_HDMI_CALLBACK_ALREADY        HI_DEF_ERR(HI_ID_HDMI, EN_ERR_LEVEL_ERROR, ERR_HDMI_CALLBACK_ALREADY)
#define HI_ERR_HDMI_INVALID_CALLBACK        HI_DEF_ERR(HI_ID_HDMI, EN_ERR_LEVEL_ERROR, ERR_HDMI_INVALID_CALLBACK)
#define HI_ERR_HDMI_FEATURE_NO_SUPPORT      HI_DEF_ERR(HI_ID_HDMI, EN_ERR_LEVEL_ERROR, ERR_HDMI_FEATURE_NO_SUPPORT)
#define HI_ERR_HDMI_BUS_BUSY                HI_DEF_ERR(HI_ID_HDMI, EN_ERR_LEVEL_ERROR, ERR_HDMI_BUS_BUSY)
#define HI_ERR_HDMI_READ_EVENT_FAILED       HI_DEF_ERR(HI_ID_HDMI, EN_ERR_LEVEL_ERROR, ERR_HDMI_READ_EVENT_FAILED)
#define HI_ERR_HDMI_NOT_START               HI_DEF_ERR(HI_ID_HDMI, EN_ERR_LEVEL_ERROR, ERR_HDMI_NOT_START)
#define HI_ERR_HDMI_READ_EDID_FAILED        HI_DEF_ERR(HI_ID_HDMI, EN_ERR_LEVEL_ERROR, ERR_HDMI_READ_EDID_FAILED)
#define HI_ERR_HDMI_INIT_FAILED             HI_DEF_ERR(HI_ID_HDMI, EN_ERR_LEVEL_ERROR, ERR_HDMI_INIT_FAILED)
#define HI_ERR_HDMI_CREATE_TESK_FAILED      HI_DEF_ERR(HI_ID_HDMI, EN_ERR_LEVEL_ERROR, ERR_HDMI_CREATE_TESK_FAILED)
#define HI_ERR_HDMI_MALLOC_FAILED           HI_DEF_ERR(HI_ID_HDMI, EN_ERR_LEVEL_ERROR, ERR_HDMI_MALLOC_FAILED)
#define HI_ERR_HDMI_FREE_FAILED             HI_DEF_ERR(HI_ID_HDMI, EN_ERR_LEVEL_ERROR, ERR_HDMI_FREE_FAILED)
#define HI_ERR_HDMI_PTHREAD_CREATE_FAILED   HI_DEF_ERR(HI_ID_HDMI, EN_ERR_LEVEL_ERROR, ERR_HDMI_PTHREAD_CREATE_FAILED)
#define HI_ERR_HDMI_PTHREAD_JOIN_FAILED     HI_DEF_ERR(HI_ID_HDMI, EN_ERR_LEVEL_ERROR, ERR_HDMI_PTHREAD_JOIN_FAILED)
#define HI_ERR_HDMI_STRATEGY_FAILED         HI_DEF_ERR(HI_ID_HDMI, EN_ERR_LEVEL_ERROR, ERR_HDMI_STRATEGY_FAILED)
#define HI_ERR_HDMI_SET_ATTR_FAILED         HI_DEF_ERR(HI_ID_HDMI, EN_ERR_LEVEL_ERROR, ERR_HDMI_SET_ATTR_FAILED)
#define HI_ERR_HDMI_CEC_CALLBACK_REREGISTER HI_DEF_ERR(HI_ID_HDMI, EN_ERR_LEVEL_ERROR, ERR_HDMI_CEC_CALLBACK_REREGISTER)
#define HI_ERR_HDMI_CALLBACK_NOT_REGISTER   HI_DEF_ERR(HI_ID_HDMI, EN_ERR_LEVEL_ERROR, ERR_HDMI_CALLBACK_NOT_REGISTER)
#define HI_ERR_HDMI_UNKNOWN_COMMAND         HI_DEF_ERR(HI_ID_HDMI, EN_ERR_LEVEL_ERROR, ERR_HDMI_UNKNOWN_COMMAND)
#define HI_ERR_HDMI_MUTEX_LOCK_FAILED       HI_DEF_ERR(HI_ID_HDMI, EN_ERR_LEVEL_ERROR, ERR_HDMI_MUTEX_LOCK_FAILED)
#define HI_ERR_HDMI_CEC_NOT_ENABLE          HI_DEF_ERR(HI_ID_HDMI, EN_ERR_LEVEL_ERROR, ERR_HDMI_CEC_NOT_ENABLE)
#define HI_ERR_HDMI_CECCALLBACK_NOT_REG     HI_DEF_ERR(HI_ID_HDMI, EN_ERR_LEVEL_ERROR, ERR_HDMI_CECCALLBACK_NOT_REG)

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
#endif

