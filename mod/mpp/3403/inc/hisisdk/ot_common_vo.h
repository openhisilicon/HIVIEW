/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */

#ifndef OT_COMMON_VO_H
#define OT_COMMON_VO_H

#include "ot_type.h"
#include "ot_common.h"
#include "ot_errno.h"
#include "ot_common_video.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* end of #ifdef __cplusplus */

/* VO define error code */
#define OT_ERR_VO_INVALID_DEV_ID      OT_DEFINE_ERR(OT_ID_VO, OT_ERR_LEVEL_ERROR, OT_ERR_INVALID_DEV_ID)
#define OT_ERR_VO_INVALID_CHN_ID      OT_DEFINE_ERR(OT_ID_VO, OT_ERR_LEVEL_ERROR, OT_ERR_INVALID_CHN_ID)
#define OT_ERR_VO_INVALID_LAYER_ID    OT_DEFINE_ERR(OT_ID_VO, OT_ERR_LEVEL_ERROR, OT_ERR_INVALID_LAYER_ID)
#define OT_ERR_VO_ILLEGAL_PARAM       OT_DEFINE_ERR(OT_ID_VO, OT_ERR_LEVEL_ERROR, OT_ERR_ILLEGAL_PARAM)
#define OT_ERR_VO_NULL_PTR            OT_DEFINE_ERR(OT_ID_VO, OT_ERR_LEVEL_ERROR, OT_ERR_NULL_PTR)
#define OT_ERR_VO_NOT_CFG             OT_DEFINE_ERR(OT_ID_VO, OT_ERR_LEVEL_ERROR, OT_ERR_NOT_CFG)
#define OT_ERR_VO_NOT_SUPPORT         OT_DEFINE_ERR(OT_ID_VO, OT_ERR_LEVEL_ERROR, OT_ERR_NOT_SUPPORT)
#define OT_ERR_VO_NOT_PERM            OT_DEFINE_ERR(OT_ID_VO, OT_ERR_LEVEL_ERROR, OT_ERR_NOT_PERM)
#define OT_ERR_VO_NOT_ENABLE          OT_DEFINE_ERR(OT_ID_VO, OT_ERR_LEVEL_ERROR, OT_ERR_NOT_ENABLE)
#define OT_ERR_VO_NOT_DISABLE         OT_DEFINE_ERR(OT_ID_VO, OT_ERR_LEVEL_ERROR, OT_ERR_NOT_DISABLE)
#define OT_ERR_VO_NO_MEM              OT_DEFINE_ERR(OT_ID_VO, OT_ERR_LEVEL_ERROR, OT_ERR_NO_MEM)
#define OT_ERR_VO_NOT_READY           OT_DEFINE_ERR(OT_ID_VO, OT_ERR_LEVEL_ERROR, OT_ERR_NOT_READY)
#define OT_ERR_VO_TIMEOUT             OT_DEFINE_ERR(OT_ID_VO, OT_ERR_LEVEL_ERROR, OT_ERR_TIMEOUT)
#define OT_ERR_VO_BUSY                OT_DEFINE_ERR(OT_ID_VO, OT_ERR_LEVEL_ERROR, OT_ERR_BUSY)
#define OT_ERR_VO_NOT_BINDED          OT_DEFINE_ERR(OT_ID_VO, OT_ERR_LEVEL_ERROR, OT_ERR_NOT_BINDED)
#define OT_ERR_VO_BINDED              OT_DEFINE_ERR(OT_ID_VO, OT_ERR_LEVEL_ERROR, OT_ERR_BINDED)

/* VO video output interface type */
#define OT_VO_INTF_CVBS       (0x01L << 0)
#define OT_VO_INTF_VGA        (0x01L << 1)
#define OT_VO_INTF_BT656      (0x01L << 2)
#define OT_VO_INTF_BT1120     (0x01L << 3)
#define OT_VO_INTF_HDMI       (0x01L << 4)
#define OT_VO_INTF_RGB_6BIT   (0x01L << 5)
#define OT_VO_INTF_RGB_8BIT   (0x01L << 6)
#define OT_VO_INTF_RGB_16BIT  (0x01L << 7)
#define OT_VO_INTF_RGB_18BIT  (0x01L << 8)
#define OT_VO_INTF_RGB_24BIT  (0x01L << 9)
#define OT_VO_INTF_MIPI       (0x01L << 10)
#define OT_VO_INTF_MIPI_SLAVE (0x01L << 11)
#define OT_VO_INTF_HDMI1      (0x01L << 12)

typedef td_u32 ot_vo_intf_type;

typedef enum {
    OT_VO_OUT_PAL = 0, /* PAL standard */
    OT_VO_OUT_NTSC = 1, /* NTSC standard */
    OT_VO_OUT_960H_PAL = 2, /* ITU-R BT.1302 960 x 576 at 50 Hz (interlaced) */
    OT_VO_OUT_960H_NTSC = 3, /* ITU-R BT.1302 960 x 480 at 60 Hz (interlaced) */

    OT_VO_OUT_640x480_60 = 4, /* VESA 640 x 480 at 60 Hz (non-interlaced) CVT */
    OT_VO_OUT_480P60 = 5, /* 720 x 480 at 60 Hz. */
    OT_VO_OUT_576P50 = 6, /* 720 x 576 at 50 Hz. */
    OT_VO_OUT_800x600_60 = 7, /* VESA 800 x 600 at 60 Hz (non-interlaced) */
    OT_VO_OUT_1024x768_60 = 8, /* VESA 1024 x 768 at 60 Hz (non-interlaced) */
    OT_VO_OUT_720P50 = 9, /* 1280 x 720 at 50 Hz. */
    OT_VO_OUT_720P60 = 10, /* 1280 x 720 at 60 Hz. */
    OT_VO_OUT_1280x800_60 = 11, /* 1280*800@60Hz VGA@60Hz */
    OT_VO_OUT_1280x1024_60 = 12, /* VESA 1280 x 1024 at 60 Hz (non-interlaced) */
    OT_VO_OUT_1366x768_60 = 13, /* VESA 1366 x 768 at 60 Hz (non-interlaced) */
    OT_VO_OUT_1400x1050_60 = 14, /* VESA 1400 x 1050 at 60 Hz (non-interlaced) CVT */
    OT_VO_OUT_1440x900_60 = 15, /* VESA 1440 x 900 at 60 Hz (non-interlaced) CVT Compliant */
    OT_VO_OUT_1680x1050_60 = 16, /* VESA 1680 x 1050 at 60 Hz (non-interlaced) */

    OT_VO_OUT_1080P24 = 17, /* 1920 x 1080 at 24 Hz. */
    OT_VO_OUT_1080P25 = 18, /* 1920 x 1080 at 25 Hz. */
    OT_VO_OUT_1080P30 = 19, /* 1920 x 1080 at 30 Hz. */
    OT_VO_OUT_1080I50 = 20, /* 1920 x 1080 at 50 Hz, interlaced. */
    OT_VO_OUT_1080I60 = 21, /* 1920 x 1080 at 60 Hz, interlaced. */
    OT_VO_OUT_1080P50 = 22, /* 1920 x 1080 at 50 Hz. */
    OT_VO_OUT_1080P60 = 23, /* 1920 x 1080 at 60 Hz. */

    OT_VO_OUT_1600x1200_60 = 24, /* VESA 1600 x 1200 at 60 Hz (non-interlaced) */
    OT_VO_OUT_1920x1200_60 = 25, /* VESA 1920 x 1200 at 60 Hz (non-interlaced) CVT (Reduced Blanking) */
    OT_VO_OUT_1920x2160_30 = 26, /* 1920x2160_30 */
    OT_VO_OUT_2560x1440_30 = 27, /* 2560x1440_30 */
    OT_VO_OUT_2560x1440_60 = 28, /* 2560x1440_60 */
    OT_VO_OUT_2560x1600_60 = 29, /* 2560x1600_60 */

    OT_VO_OUT_3840x2160_24 = 30, /* 3840x2160_24 */
    OT_VO_OUT_3840x2160_25 = 31, /* 3840x2160_25 */
    OT_VO_OUT_3840x2160_30 = 32, /* 3840x2160_30 */
    OT_VO_OUT_3840x2160_50 = 33, /* 3840x2160_50 */
    OT_VO_OUT_3840x2160_60 = 34, /* 3840x2160_60 */
    OT_VO_OUT_4096x2160_24 = 35, /* 4096x2160_24 */
    OT_VO_OUT_4096x2160_25 = 36, /* 4096x2160_25 */
    OT_VO_OUT_4096x2160_30 = 37, /* 4096x2160_30 */
    OT_VO_OUT_4096x2160_50 = 38, /* 4096x2160_50 */
    OT_VO_OUT_4096x2160_60 = 39, /* 4096x2160_60 */
    OT_VO_OUT_7680x4320_30 = 40, /* 7680x4320_30 */

    OT_VO_OUT_240x320_50 = 41, /* 240x320_50 */
    OT_VO_OUT_320x240_50 = 42, /* 320x240_50 */
    OT_VO_OUT_240x320_60 = 43, /* 240x320_60 */
    OT_VO_OUT_320x240_60 = 44, /* 320x240_60 */
    OT_VO_OUT_800x600_50 = 45, /* 800x600_50 */

    OT_VO_OUT_720x1280_60 = 46, /* For MIPI DSI Tx 720 x1280 at 60 Hz */
    OT_VO_OUT_1080x1920_60 = 47, /* For MIPI DSI Tx 1080x1920 at 60 Hz */

    OT_VO_OUT_USER = 48, /* User timing. */

    OT_VO_OUT_BUTT,
} ot_vo_intf_sync;

typedef struct {
    td_bool syncm; /* RW; sync mode(0:timing,as BT.656; 1:signal,as LCD) */
    td_bool iop; /* RW; interlaced or progressive display(0:i; 1:p) */
    td_u8 intfb; /* RW; interlaced bit width while output */

    td_u16 vact; /* RW; vertical active area */
    td_u16 vbb; /* RW; vertical back blank porch */
    td_u16 vfb; /* RW; vertical front blank porch */

    td_u16 hact; /* RW; horizontal active area */
    td_u16 hbb; /* RW; horizontal back blank porch */
    td_u16 hfb; /* RW; horizontal front blank porch */
    td_u16 hmid; /* RW; bottom horizontal active area */

    td_u16 bvact; /* RW; bottom vertical active area */
    td_u16 bvbb; /* RW; bottom vertical back blank porch */
    td_u16 bvfb; /* RW; bottom vertical front blank porch */

    td_u16 hpw; /* RW; horizontal pulse width */
    td_u16 vpw; /* RW; vertical pulse width */

    td_bool idv; /* RW; inverse data valid of output */
    td_bool ihs; /* RW; inverse horizontal synchronization signal */
    td_bool ivs; /* RW; inverse vertical synchronization signal */
} ot_vo_sync_info;

typedef struct {
    td_u32 bg_color; /* RW; background color of a device, in RGB format. */
    ot_vo_intf_type intf_type; /* RW; type of a VO interface */
    ot_vo_intf_sync intf_sync; /* RW; type of a VO interface timing */
    ot_vo_sync_info sync_info; /* RW; information about VO interface timing */
} ot_vo_pub_attr;

typedef struct {
    td_bool vo_bypass_en; /* RW, range: [0, 1]; vo bypass enable */
} ot_vo_dev_param;

typedef struct {
    td_bool exit_dev_en;  /* RW, range: [0, 1];  whether to disable the vo device when sys exit */
    td_bool dev_clk_ext_en;  /* RW, range: [0, 1];  whether to open the vo device clock by the external user */
    td_bool vga_detect_en;   /* RW, range: [0, 1];  whether to enable the vga detect */
    td_u32 vdac_detect_cycle;  /* RW; VDAC(video digital-to-analog converter) detect cycle; Unit: frame interruption */
} ot_vo_mod_param;

typedef enum {
    OT_VO_CLK_SRC_PLL = 0,       /* Clock source type PLL */
    OT_VO_CLK_SRC_LCDMCLK = 1,   /* Clock source type LCDMCLK */
    OT_VO_CLK_SRC_PLL_FOUT4 = 2, /* Clock source type PLL FOUT4 */
    OT_VO_CLK_SRC_FIXED = 3,     /* Clock source type FIXED */

    OT_VO_CLK_SRC_BUTT,
} ot_vo_clk_src;

typedef enum {
    OT_VO_FIXED_CLK_148_5M  = 0, /* Fixed clock source 148.5MHz */
    OT_VO_FIXED_CLK_134_4M  = 1, /* Fixed clock source 134.4MHz */
    OT_VO_FIXED_CLK_108M    = 2, /* Fixed clock source 108MHz */
    OT_VO_FIXED_CLK_74_25M  = 3, /* Fixed clock source 74.25MHz */
    OT_VO_FIXED_CLK_64M     = 4, /* Fixed clock source 64MHz */
    OT_VO_FIXED_CLK_54M     = 5, /* Fixed clock source 54MHz */
    OT_VO_FIXED_CLK_37_125M = 6, /* Fixed clock source 37.125MHz */
    OT_VO_FIXED_CLK_13_5M   = 7, /* Fixed clock source 13.5MHz */

    OT_VO_FIXED_CLK_BUTT,
} ot_vo_fixed_clk;

typedef struct {
    td_u32 fb_div;    /* RW, range: [0, 0xfff];  frequency double division */
    td_u32 frac;      /* RW, range: [0, 0xffffff]; fractional division */
    td_u32 ref_div;   /* RW, range: (0, 0x3f]; reference clock division */
    td_u32 post_div1; /* RW, range: (0, 0x7]; level 1 post division */
    td_u32 post_div2; /* RW, range: (0, 0x7]; level 2 post division */
} ot_vo_pll;

typedef struct {
    ot_vo_clk_src clk_src; /* RW; clock source type */

    union {
        /*
         * RW; user synchronization timing clock PLL information.
         * AUTO: ot_vo_clk_src:OT_VO_CLK_SRC_PLL, OT_VO_CLK_SRC_PLL_FOUT4;
         */
        ot_vo_pll vo_pll;
        /*
         * RW, range: [1, 8473341]; LCD clock division.
         * AUTO: ot_vo_clk_src:OT_VO_CLK_SRC_LCDMCLK;
         */
        td_u32 lcd_m_clk_div;

        ot_vo_fixed_clk fixed_clk; /* RW; fixed clock. AUTO: ot_vo_clk_src:OT_VO_CLK_SRC_FIXED; */
    };
} ot_vo_user_sync_attr;

typedef struct {
    ot_vo_user_sync_attr user_sync_attr; /* RW; user synchronization timing attribute */
    td_u32 pre_div;                      /* RW, range: [1, 32]; device previous division */
    td_u32 dev_div;                      /* RW, range: [1, 4]; device clock division */
    td_bool clk_reverse_en;              /* RW, range: [0, 1]; whether to reverse clock  */
} ot_vo_user_sync_info;

typedef struct {
    td_bool enable;  /* RW, less buf enable */
    td_u32 vtth;     /* RW, vtth value */
}ot_vo_less_buf_attr;

typedef struct {
    td_bool enable;  /* RW, user notify enable */
    td_u32 vtth;     /* RW, vtth value */
}ot_vo_user_notify_attr;

typedef enum {
    OT_VO_INTF_STATUS_NO_PLUG = 0,  /* Interface status is not plugged */
    OT_VO_INTF_STATUS_PLUG = 1,     /* Interface status is plugged */
    OT_VO_INTF_STATUS_BUTT,
}ot_vo_intf_plug_status;

typedef struct {
    ot_vo_intf_plug_status plug_status;  /* R; Interface plug status */
}ot_vo_intf_status;

typedef enum {
    OT_VO_CSC_MATRIX_BT601LIMIT_TO_BT601LIMIT = 0, /* Identity matrix.   from BT.601 limit to BT.601 limit */
    OT_VO_CSC_MATRIX_BT601FULL_TO_BT601LIMIT = 1,  /* Change color space from BT.601 full to BT.601 limit */
    OT_VO_CSC_MATRIX_BT709LIMIT_TO_BT601LIMIT = 2, /* Change color space from BT.709 limit to BT.601 limit */
    OT_VO_CSC_MATRIX_BT709FULL_TO_BT601LIMIT = 3,  /* Change color space from BT.709 full to BT.601 limit */

    OT_VO_CSC_MATRIX_BT601LIMIT_TO_BT709LIMIT = 4, /* Change color space from BT.601 limit to BT.709 limit */
    OT_VO_CSC_MATRIX_BT601FULL_TO_BT709LIMIT = 5,  /* Change color space from BT.601 full to BT.709 limit */
    OT_VO_CSC_MATRIX_BT709LIMIT_TO_BT709LIMIT = 6, /* Identity matrix.   from BT.709 limit to BT.709 limit */
    OT_VO_CSC_MATRIX_BT709FULL_TO_BT709LIMIT = 7,  /* Change color space from BT.709 full to BT.709 limit */

    OT_VO_CSC_MATRIX_BT601LIMIT_TO_BT601FULL = 8,  /* Change color space from BT.601 limit to BT.601 full */
    OT_VO_CSC_MATRIX_BT601FULL_TO_BT601FULL = 9,   /* Identity matrix.   from BT.601 full to BT.601 full */
    OT_VO_CSC_MATRIX_BT709LIMIT_TO_BT601FULL = 10,  /* Change color space from BT.709 limit to BT.601 full */
    OT_VO_CSC_MATRIX_BT709FULL_TO_BT601FULL = 11,   /* Change color space from BT.709 full to BT.601 full */

    OT_VO_CSC_MATRIX_BT601LIMIT_TO_BT709FULL = 12,  /* Change color space from BT.601 limit to BT.709 full */
    OT_VO_CSC_MATRIX_BT601FULL_TO_BT709FULL = 13,   /* Change color space from BT.601 full to BT.709 full */
    OT_VO_CSC_MATRIX_BT709LIMIT_TO_BT709FULL = 14,  /* Change color space from BT.709 limit to BT.709 full */
    OT_VO_CSC_MATRIX_BT709FULL_TO_BT709FULL = 15,   /* Identity matrix.   from BT.709 full to BT.709 full */

    OT_VO_CSC_MATRIX_BT601LIMIT_TO_RGBFULL = 16,    /* Change color space from BT.601 limit to RGB full */
    OT_VO_CSC_MATRIX_BT601FULL_TO_RGBFULL = 17,     /* Change color space from BT.601 full to RGB full */
    OT_VO_CSC_MATRIX_BT709LIMIT_TO_RGBFULL = 18,    /* Change color space from BT.709 limit to RGB full */
    OT_VO_CSC_MATRIX_BT709FULL_TO_RGBFULL = 19,     /* Change color space from BT.709 full to RGB full */

    OT_VO_CSC_MATRIX_BT601LIMIT_TO_RGBLIMIT = 20,   /* Change color space from BT.601 limit to RGB limit */
    OT_VO_CSC_MATRIX_BT601FULL_TO_RGBLIMIT = 21,    /* Change color space from BT.709 full to RGB limit */
    OT_VO_CSC_MATRIX_BT709LIMIT_TO_RGBLIMIT = 22,   /* Change color space from BT.601 limit to RGB limit */
    OT_VO_CSC_MATRIX_BT709FULL_TO_RGBLIMIT = 23,    /* Change color space from BT.709 full to RGB limit */

    OT_VO_CSC_MATRIX_RGBFULL_TO_BT601LIMIT = 24,    /* Change color space from RGB full to BT.601 limit */
    OT_VO_CSC_MATRIX_RGBFULL_TO_BT601FULL = 25,     /* Change color space from RGB full to BT.601 full */
    OT_VO_CSC_MATRIX_RGBFULL_TO_BT709LIMIT = 26,    /* Change color space from RGB full to BT.709 limit */
    OT_VO_CSC_MATRIX_RGBFULL_TO_BT709FULL = 27,     /* Change color space from RGB full to BT.709 full */

    OT_VO_CSC_MATRIX_BUTT,
} ot_vo_csc_matrix;

typedef struct {
    ot_vo_csc_matrix csc_matrix; /* RW; CSC matrix */
    td_u32 luma; /* RW; range: [0, 100]; luminance, default: 50 */
    td_u32 contrast; /* RW; range: [0, 100]; contrast, default: 50 */
    td_u32 hue; /* RW; range: [0, 100]; hue, default: 50 */
    td_u32 saturation; /* RW; range: [0, 100]; saturation, default: 50 */
    td_bool ex_csc_en; /* RW; range: [0, 1]; extended csc switch for luminance, default: 0 */
} ot_vo_csc;

typedef struct {
    ot_vo_csc csc;  /* RW; color space */
    td_u32 gain;  /* RW; range: [0, 64); current gain of VGA signals. */
    td_s32 sharpen_strength;  /* RW; range: [0, 255]; current sharpen strength of VGA signals. */
} ot_vo_vga_param;

typedef struct {
    ot_vo_csc csc; /* RW, color space */
} ot_vo_hdmi_param;

typedef struct {
    ot_vo_csc csc; /* RW, color space */
    td_bool rgb_inverted_en; /* RW; component r g b inverted, rgb to bgr, default: 0, rgb */
    td_bool bit_inverted_en; /* RW; data's bit inverted,
                                rgb6bit: bit[5:0] to bit[0:5],
                                rgb8bit: bit[7:0] to bit[0:7],
                                rgb16bit: bit[15:0] to bit[0:15],
                                rgb18bit: bit[17:0] to bit[0:17],
                                rgb24bit: bit[23:0] to bit[0:23],
                                default: 0, bit[5/7/15/17/23:0] */
} ot_vo_rgb_param;

typedef enum {
    OT_VO_CLK_EDGE_SINGLE = 0,  /* single-edge mode */
    OT_VO_CLK_EDGE_DUAL,        /* dual-edge mode */

    OT_VO_CLK_EDGE_BUTT
} ot_vo_clk_edge;

typedef struct {
    td_bool yc_inverted_en; /* RW; component y c inverted, yc to cy, default: 0, yc */
    td_bool bit_inverted_en; /* RW; data's bit inverted, bt.656: bit[7:0] to bit[0:7],
                                bt.1120: bit[15:0] to bit[0:15],
                                default: 0, bit[7:0] or bit[15:0] */
    ot_vo_clk_edge clk_edge; /* RW; clk edge */
} ot_vo_bt_param;

typedef struct {
    ot_vo_csc csc; /* RW, color space */
} ot_vo_mipi_param;

typedef enum {
    OT_VO_PARTITION_MODE_SINGLE = 0, /* Single partition, use software to make multi-picture in one hardware cell */
    OT_VO_PARTITION_MODE_MULTI = 1,  /* Multi partition, each partition is a hardware cell */
    OT_VO_PARTITION_MODE_BUTT,
} ot_vo_partition_mode;

typedef struct {
    ot_rect display_rect; /* RW; display resolution */
    ot_size img_size; /* RW; canvas size of the video layer */
    td_u32 display_frame_rate; /* RW; display frame rate */
    ot_pixel_format pixel_format; /* RW; pixel format of the video layer */
    td_bool double_frame_en; /* RW; whether to double frames */
    td_bool cluster_mode_en; /* RW; whether to take cluster way to use memory */
    ot_dynamic_range dst_dynamic_range; /* RW; video layer output dynamic range type */
    td_u32 display_buf_len;            /* RW; Video Layer display buffer length */
    ot_vo_partition_mode partition_mode;  /* RW; Video Layer partition mode */
    ot_compress_mode compress_mode; /* RW; Compressing mode */
} ot_vo_video_layer_attr;

typedef struct {
    ot_aspect_ratio aspect_ratio; /* RW; aspect ratio */
} ot_vo_layer_param;

typedef struct {
    td_u32 priority; /* RW; video out overlay priority sd */
    ot_rect rect; /* RW; rectangle of video output channel */
    td_bool deflicker_en; /* RW; deflicker or not sd */
} ot_vo_chn_attr;

typedef struct {
    ot_aspect_ratio aspect_ratio; /* RW; aspect ratio */
} ot_vo_chn_param;

typedef enum {
    OT_VO_ZOOM_IN_RECT = 0, /* Zoom in by rect */
    OT_VO_ZOOM_IN_RATIO = 1, /* Zoom in by ratio */
    OT_VO_ZOOM_IN_BUTT,
} ot_vo_zoom_in_type;

typedef struct {
    /* RW; range: [0, 1000]; x_ratio = x * 1000 / W, x means start point to be zoomed, W means channel's width. */
    td_u32 x_ratio;
    /* RW; range: [0, 1000]; y_ratio = y * 1000 / H, y means start point to be zoomed, H means channel's height. */
    td_u32 y_ratio;
    /* RW; range: [0, 1000]; width_ratio = w * 1000 / W, w means width to be zoomed, W means channel's width. */
    td_u32 width_ratio;
    /* RW; range: [0, 1000]; height_ratio = h * 1000 / H, h means height to be zoomed, H means channel's height. */
    td_u32 height_ratio;
} ot_vo_zoom_ratio;

typedef struct {
    ot_vo_zoom_in_type zoom_type; /* RW; choose the type of zoom in */
    union {
        ot_rect zoom_rect; /* RW; zoom in by rect. AUTO:ot_vo_zoom_in_type:OT_VO_ZOOM_IN_RECT; */
        ot_vo_zoom_ratio zoom_ratio; /* RW; zoom in by ratio. AUTO:ot_vo_zoom_in_type:OT_VO_ZOOM_IN_RATIO; */
    };
} ot_vo_zoom_attr;

typedef struct {
    td_bool enable; /* RW; do frame or not */
    ot_border border; /* RW; frame's top, bottom, left, right width and color */
} ot_vo_border;

typedef enum  {
    OT_VO_MIRROR_NONE = 0, /* Mirror mode is none */
    OT_VO_MIRROR_HOR  = 1, /* Mirror mode is horizontal mirror */
    OT_VO_MIRROR_VER  = 2, /* Mirror mode is vertical mirror */
    OT_VO_MIRROR_BOTH = 3, /* Mirror mode is horizontal and vertical mirror */
    OT_VO_MIRROR_BUTT
} ot_vo_mirror_mode;

typedef struct {
    td_u32 chn_buf_used; /* R; channel buffer that been used */
    td_u32 chn_busy_num; /* R; busy channel buffer number */
} ot_vo_chn_status;

typedef struct {
    ot_size target_size; /* RW; WBC zoom target size */
    ot_pixel_format pixel_format; /* RW; the pixel format of WBC output */
    td_u32 frame_rate; /* RW; frame rate control */
    ot_dynamic_range dynamic_range; /* RW; write back dynamic range type */
    ot_compress_mode compress_mode; /* RW; write back compressing mode */
} ot_vo_wbc_attr;

typedef enum {
    OT_VO_WBC_MODE_NORM = 0, /* In this mode, wbc will capture frames according to dev frame rate
                                and wbc frame rate */
    OT_VO_WBC_MODE_DROP_REPEAT = 1, /* In this mode, wbc will drop dev repeat frame, and capture the real frame
                                according to video layer's display rate and wbc frame rate */
    OT_VO_WBC_MODE_PROGRESSIVE_TO_INTERLACED = 2, /* In this mode, wbc will drop dev repeat frame which repeats more
                                than 3 times, and change two progressive frames to one interlaced frame */
    OT_VO_WBC_MODE_BUTT,
} ot_vo_wbc_mode;

typedef enum {
    OT_VO_WBC_SRC_DEV = 0, /* WBC source is device */
    OT_VO_WBC_SRC_VIDEO = 1, /* WBC source is video layer */
    OT_VO_WBC_SRC_BUTT,
} ot_vo_wbc_src_type;

typedef struct {
    ot_vo_wbc_src_type src_type; /* RW; WBC source's type */
    td_u32 src_id; /* RW; WBC source's ID */
} ot_vo_wbc_src;

typedef enum {
    OT_VO_CAS_MODE_SINGLE = 0, /* cascade mode is single */
    OT_VO_CAS_MODE_DUAL = 1, /* cascade mode is dual */
    OT_VO_CAS_MODE_BUTT,
} ot_vo_cas_mode;

typedef enum {
    OT_VO_CAS_MODE_SINGLE_EDGE = 0, /* single transmission,clock rising edge or falling edge trigger transmission */
    OT_VO_CAS_MODE_DUAL_EDGE = 1, /* dual transmission,clock rising edge and falling edge trigger transmission */
    OT_VO_CAS_MODE_EDGE_BUTT,
} ot_vo_cas_data_transmission_mode;

typedef enum {
    OT_VO_CAS_RGN_64 = 0, /* cascade region number 64 */
    OT_VO_CAS_RGN_32 = 1, /* cascade region number 32 */
    OT_VO_CAS_RGN_BUTT,
} ot_vo_cas_rgn;

typedef struct {
    td_bool         is_slave;                        /* RW; TD_TRUE: slave mode, TD_FALSE: master mode */
    ot_vo_cas_rgn   cas_rgn;                         /* RW; cascade region number */
    ot_vo_cas_mode  cas_mode;                        /* RW; cascade mode */
    ot_vo_cas_data_transmission_mode cas_edge_mode;  /* RW; cascade data transmission mode  */
} ot_vo_cas_attr;

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* end of #ifdef __cplusplus */

#endif /* end of #ifndef OT_COMMON_VO_H */
