/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2019-2020. All rights reserved.
 * Description: hi_common_vi.h
 * Author: Hisilicon multimedia software group
 * Create: 2019-05-21
 */

#ifndef __HI_COMMON_VI_H__
#define __HI_COMMON_VI_H__

#include "hi_common.h"
#include "hi_errno.h"
#include "hi_common_video.h"

#ifdef __cplusplus
extern "C" {
#endif

#define HI_VI_INVALID_FRAME_RATE (-1U)
#define HI_VI_MAX_USER_FRAME_DEPTH 8
#define HI_VI_COMPONENT_MASK_NUM 2
#define HI_VI_VBI_NUM 2

#define HI_ERR_VI_INVALID_DEV_ID HI_DEFINE_ERR(HI_ID_VI, HI_ERR_LEVEL_ERROR, HI_ERR_INVALID_DEV_ID)
#define HI_ERR_VI_INVALID_CHN_ID HI_DEFINE_ERR(HI_ID_VI, HI_ERR_LEVEL_ERROR, HI_ERR_INVALID_CHN_ID)
#define HI_ERR_VI_INVALID_PARAM HI_DEFINE_ERR(HI_ID_VI, HI_ERR_LEVEL_ERROR, HI_ERR_ILLEGAL_PARAM)
#define HI_ERR_VI_INVALID_NULL_PTR HI_DEFINE_ERR(HI_ID_VI, HI_ERR_LEVEL_ERROR, HI_ERR_NULL_PTR)
#define HI_ERR_VI_FAILED_NOT_CFG HI_DEFINE_ERR(HI_ID_VI, HI_ERR_LEVEL_ERROR, HI_ERR_NOT_CFG)
#define HI_ERR_VI_NOT_SUPPORT HI_DEFINE_ERR(HI_ID_VI, HI_ERR_LEVEL_ERROR, HI_ERR_NOT_SUPPORT)
#define HI_ERR_VI_NOT_PERM HI_DEFINE_ERR(HI_ID_VI, HI_ERR_LEVEL_ERROR, HI_ERR_NOT_PERM)
#define HI_ERR_VI_NOT_ENABLE HI_DEFINE_ERR(HI_ID_VI, HI_ERR_LEVEL_ERROR, HI_ERR_NOT_ENABLE)
#define HI_ERR_VI_NOT_DISABLE HI_DEFINE_ERR(HI_ID_VI, HI_ERR_LEVEL_ERROR, HI_ERR_NOT_DISABLE)
#define HI_ERR_VI_NO_MEM HI_DEFINE_ERR(HI_ID_VI, HI_ERR_LEVEL_ERROR, HI_ERR_NO_MEM)
#define HI_ERR_VI_BUF_EMPTY HI_DEFINE_ERR(HI_ID_VI, HI_ERR_LEVEL_ERROR, HI_ERR_BUF_EMPTY)
#define HI_ERR_VI_BUF_FULL HI_DEFINE_ERR(HI_ID_VI, HI_ERR_LEVEL_ERROR, HI_ERR_BUF_FULL)
#define HI_ERR_VI_NOT_READY HI_DEFINE_ERR(HI_ID_VI, HI_ERR_LEVEL_ERROR, HI_ERR_NOT_READY)
#define HI_ERR_VI_BUSY HI_DEFINE_ERR(HI_ID_VI, HI_ERR_LEVEL_ERROR, HI_ERR_BUSY)
#define HI_ERR_VI_NOT_BINDED HI_DEFINE_ERR(HI_ID_VI, HI_ERR_LEVEL_ERROR, HI_ERR_NOT_BINDED)
#define HI_ERR_VI_BINDED HI_DEFINE_ERR(HI_ID_VI, HI_ERR_LEVEL_ERROR, HI_ERR_BINDED)

/* interface mode of video input */
typedef enum {
    HI_VI_MODE_BT656 = 0,          /* ITU-R BT.656 YUV4:2:2 */
    HI_VI_MODE_BT1120_STANDARD,    /* BT.1120 progressive mode */
    HI_VI_MODE_BT1120_INTERLEAVED, /* BT.1120 interleave mode */
    HI_VI_MODE_MIPI_YUV422,        /* MIPI YUV 422 mode */

    HI_VI_MODE_BUTT
} hi_vi_intf_mode;

/* input mode */
typedef enum {
    HI_VI_IN_MODE_BT656 = 0,   /* ITU-R BT.656 YUV4:2:2 */
    HI_VI_IN_MODE_INTERLEAVED, /* interleave mode */
    HI_VI_IN_MODE_MIPI,        /* MIPI mode */

    HI_VI_IN_MODE_BUTT
} hi_vi_in_mode;

typedef enum {
    HI_VI_WORK_MODE_MULTIPLEX_1 = 0, /* 1 Multiplex mode */
    HI_VI_WORK_MODE_MULTIPLEX_2,     /* 2 Multiplex mode */
    HI_VI_WORK_MODE_MULTIPLEX_4,     /* 4 Multiplex mode */

    HI_VI_WORK_MODE_BUTT
} hi_vi_work_mode;

/* whether an input picture is interlaced or progressive */
typedef enum {
    HI_VI_SCAN_INTERLACED = 0,
    HI_VI_SCAN_PROGRESSIVE,

    HI_VI_SCAN_BUTT
} hi_vi_scan_mode;

typedef enum {
    /* the input sequence of the second component(only contains u and v) in BT.1120 mode */
    HI_VI_IN_DATA_VUVU = 0,
    HI_VI_IN_DATA_UVUV,

    /* the input sequence for yuv */
    HI_VI_IN_DATA_UYVY,
    HI_VI_IN_DATA_VYUY,
    HI_VI_IN_DATA_YUYV,
    HI_VI_IN_DATA_YVYU,

    HI_VI_IN_DATA_YUV_BUTT
} hi_vi_data_yuv_seq;

typedef enum {
    HI_VI_CLK_EDGE_SINGLE_UP = 0, /* single-edge mode and in rising edge */
    HI_VI_CLK_EDGE_SINGLE_DOWN,   /* single-edge mode and in falling edge */
    HI_VI_CLK_EDGE_DOUBLE,        /* double-edge mode */

    HI_VI_CLK_EDGE_BUTT
} hi_vi_clk_edge;

typedef enum {
    HI_VI_COMPONENT_MODE_SINGLE = 0, /* in single component mode */
    HI_VI_COMPONENT_MODE_DOUBLE = 1, /* in double component mode */
    HI_VI_COMPONENT_MODE_BUTT
} hi_vi_component_mode;

/* Y/C composite or separation mode */
typedef enum {
    HI_VI_COMBINE_COMPOSITE = 0, /* composite mode */
    HI_VI_COMBINE_SEPARATE,      /* separate mode */
    HI_VI_COMBINE_BUTT
} hi_vi_combine_mode;

/* attribute of the vertical synchronization signal */
typedef enum {
    HI_VI_VSYNC_FIELD = 0, /* field/toggle mode:a signal reversal means a new frame or a field */
    HI_VI_VSYNC_PULSE,     /* pulse/effective mode:a pulse or an effective signal means a new frame or a field */
    HI_VI_VSYNC_BUTT
} hi_vi_vsync;

/* polarity of the vertical synchronization signal */
typedef enum {
    HI_VI_VSYNC_NEG_HIGH = 0, /*
                               * if hi_vi_vsync = VIU_VSYNC_FIELD, then the vertical synchronization signal of
                               * even field is high-level, if hi_vi_vsync = VIU_VSYNC_PULSE, then the vertical
                               * synchronization pulse is positive pulse
                               */
    HI_VI_VSYNC_NEG_LOW, /*
                          * if hi_vi_vsync = VIU_VSYNC_FIELD, then the vertical synchronization signal of
                          * even field is low-level, if hi_vi_vsync = VIU_VSYNC_PULSE, then the vertical
                          * synchronization pulse is negative pulse
                          */
    HI_VI_VSYNC_NEG_BUTT
} hi_vi_vsync_neg;

/* attribute of the horizontal synchronization signal */
typedef enum {
    HI_VI_HSYNC_VALID_SIG = 0, /* the horizontal synchronization is valid signal mode */
    HI_VI_HSYNC_PULSE,         /*
                                * the horizontal synchronization is pulse mode,
                                * a new pulse means the beginning of a new line
                                */
    HI_VI_HSYNC_BUTT
} hi_vi_hsync;

/* polarity of the horizontal synchronization signal */
typedef enum {
    HI_VI_HSYNC_NEG_HIGH = 0, /*
                               * if hi_vi_hsync = VI_HSYNC_VALID_SINGNAL, then the valid horizontal
                               * synchronization signal is high-level;
                               * if hi_vi_hsync = HI_VI_HSYNC_PULSE, then the horizontal synchronization
                               * pulse is positive pulse
                               */
    HI_VI_HSYNC_NEG_LOW, /*
                          * if hi_vi_hsync = VI_HSYNC_VALID_SINGNAL, then the valid horizontal
                          * synchronization signal is low-level;
                          * if hi_vi_hsync = HI_VI_HSYNC_PULSE,then the horizontal synchronization
                          * pulse is negative pulse
                          */
    HI_VI_HSYNC_NEG_BUTT
} hi_vi_hsync_neg;

/* attribute of the valid vertical synchronization signal */
typedef enum {
    HI_VI_VSYNC_NORM_PULSE = 0, /* the vertical synchronization is pulse mode, a pulse means a new frame or field */
    HI_VI_VSYNC_VALID_SIG,      /*
                                 * the vertical synchronization is effective mode, a effective signal means a new
                                 * frame or field
                                 */
    HI_VI_VSYNC_VALID_BUTT
} hi_vi_vsync_valid;

/* polarity of the valid vertical synchronization signal */
typedef enum {
    HI_VI_VSYNC_VALID_NEG_HIGH = 0, /*
                                     * if hi_vi_vsync_valid = HI_VI_VSYNC_NORM_PULSE, a positive pulse means vertical
                                     * synchronization pulse;
                                     * if hi_vi_vsync_valid = HI_VI_VSYNC_VALID_SIG, the valid vertical synchronization
                                     * signal is high-level
                                     */
    HI_VI_VSYNC_VALID_NEG_LOW,      /*
                                     * if hi_vi_vsync_valid = HI_VI_VSYNC_NORM_PULSE, a negative pulse means
                                     * vertical synchronization pulse;
                                     * if hi_vi_vsync_valid = HI_VI_VSYNC_VALID_SIG, the valid vertical synchronization
                                     * signal is low-level
                                     */
    HI_VI_VSYNC_AVLID_NEG_BUTT
} hi_vi_vsync_valid_neg;

/* blank information of the input timing */
typedef struct {
    hi_u32 hsync_hfb;   /* horizontal front blanking width */
    hi_u32 hsync_act;   /* horizontal effective width */
    hi_u32 hsync_hbb;   /* horizontal back blanking width */
    hi_u32 vsync_vfb;   /* vertical front blanking height of one frame or odd-field frame picture */
    hi_u32 vsync_vact;  /* vertical effective width of one frame or odd-field frame picture */
    hi_u32 vsync_vbb;   /* vertical back blanking height of one frame or odd-field frame picture */
    hi_u32 vsync_vbfb;  /*
                         * even-field vertical front blanking height when input mode is interlace
                         * (invalid when progressive input mode)
                         */
    hi_u32 vsync_vbact; /*
                         * even-field vertical effective width when input mode is interlace
                         * (invalid when progressive input mode)
                         */
    hi_u32 vsync_vbbb;  /*
                         * even-field vertical back blanking height when input mode is interlace
                         * (invalid when progressive input mode)
                         */
} hi_vi_timing_blank;

/* synchronization information about the BT.601 or DC timing */
typedef struct {
    hi_vi_vsync vsync;
    hi_vi_vsync_neg vsync_neg;
    hi_vi_hsync hsync;
    hi_vi_hsync_neg hsync_neg;
    hi_vi_vsync_valid vsync_valid;
    hi_vi_vsync_valid_neg vsync_valid_neg;
    hi_vi_timing_blank timing_blank;
} hi_vi_sync_cfg;

/* the highest bit of the BT.656 timing reference code */
typedef enum {
    HI_BT656_FIXCODE_1 = 0, /* the highest bit of the EAV/SAV data over the BT.656 protocol is always 1. */
    HI_BT656_FIXCODE_0,     /* the highest bit of the EAV/SAV data over the BT.656 protocol is always 0. */
    HI_BT656_FIXCODE_BUTT
} hi_bt656_fixcode;

/* polarity of the field indicator bit (F) of the BT.656 timing reference code */
typedef enum {
    HI_BT656_FIELD_POLAR_STD = 0, /* the standard BT.656 mode,the first filed F=0,the second filed F=1 */
    HI_BT656_FIELD_POLAR_NON_STD, /* the non-standard BT.656 mode,the first filed F=1,the second filed F=0 */
    HI_BT656_FIELD_POLAR_BUTT
} hi_bt656_field_polar;

typedef struct {
    hi_bt656_fixcode fix_code;
    hi_bt656_field_polar field_polar;
} hi_vi_bt656_sync_cfg;

typedef enum {
    HI_VI_VBI_LOCAL_ODD_FRONT = 0,
    HI_VI_VBI_LOCAL_ODD_END,
    HI_VI_VBI_LOCAL_EVEN_FRONT,
    HI_VI_VBI_LOCAL_EVEN_END,
    HI_VI_VBI_LOCAL_BUTT
} hi_vi_vbi_location;

typedef struct {
    hi_vi_vbi_location location; /* location of VBI */
    hi_s32 x;                    /* horizontal original position of the VBI data */
    hi_s32 y;                    /* vertical original position of the VBI data */
    hi_u32 len;                  /* length of VBI data, by word(4 bytes) */
} hi_vi_vbi_attr;

typedef struct {
    hi_vi_vbi_attr vbi_attr[HI_VI_MAX_VBI_NUM];
} hi_vi_vbi_arg;

typedef enum {
    HI_VI_VBI_MODE_8BIT = 0,
    HI_VI_VBI_MODE_6BIT,

    VI_VBI_MODE_BUTT
} hi_vi_vbi_mode;

typedef struct {
    hi_u32 data[HI_VI_MAX_VBI_LEN];
    hi_u32 len;
} hi_video_vbi_info;

/* the extended attributes of VI device */
typedef struct {
    hi_vi_in_mode in_mode;     /* input mode */
    hi_vi_work_mode work_mode; /* 1-, 2-, or 4-channel multiplexed work mode */

    hi_vi_combine_mode combine_mode;     /* Y/C composite or separation mode */
    hi_vi_component_mode component_mode; /* component mode (single-component or dual-component) */
    hi_vi_clk_edge clk_edge;             /* clock edge mode (sampling on the rising or falling edge) */

    hi_u32 component_mask[HI_VI_COMPONENT_MASK_NUM]; /* component mask */

    hi_vi_data_yuv_seq data_seq; /* input data sequence (only the YUV format is supported) */
    hi_vi_sync_cfg sync_cfg;     /* sync timing. this member must be configured in BT.601 mode or DC mode */

    hi_vi_bt656_sync_cfg bt656_sync_cfg; /* sync timing. this member must be configured in BT.656 mode */

    hi_bool data_reverse_en; /* data reverse */

    hi_bool mixed_capture_en; /* mixed capatrue switch */
} hi_vi_dev_attr_ex;

/* the attributes of a VI device */
typedef struct {
    hi_vi_intf_mode intf_mode; /* interface mode */
    hi_vi_work_mode work_mode; /* 1-, 2-, or 4-channel multiplexed work mode */

    hi_u32 component_mask[HI_VI_COMPONENT_MASK_NUM]; /* component mask */

    hi_vi_clk_edge clk_edge; /* clock edge mode (sampling on the rising or falling, double edge) */

    hi_vi_data_yuv_seq data_seq; /* input data sequence (only the YUV format is supported) */
    hi_vi_sync_cfg sync_cfg;     /* sync timing. this member must be configured in BT.601 mode or DC mode */

    hi_bool data_reverse_en; /* data reverse */

    hi_bool mixed_capture_en; /* mixed capatrue switch */
} hi_vi_dev_attr;


typedef struct {
    hi_vi_dev vi_dev;
    hi_vi_way vi_way;
} hi_vi_chn_bind_attr;

/* capture selection of video input */
typedef enum {
    HI_VI_CAPTURE_SELECT_TOP = 0, /* top field */
    HI_VI_CAPTURE_SELECT_BOTTOM,  /* bottom field */
    HI_VI_CAPTURE_SELECT_BOTH,    /* top and bottom field */
    HI_VI_CAPTURE_SELECT_BUTT
} hi_vi_capture_select;

/* the attributes of a VI channel */
typedef struct {
    hi_rect capture_rect; /*
                           * the capture rect (corresponding to the size of the picture captured
                           * by a VI device).
                           * for primary channels, the capture_rect's width and height are static attributes.
                           * that is, the value of them can be changed only after primary and secondary
                           * channels are disabled.
                           * for secondary channels, capture_rect is an invalid attribute
                           */
    hi_size dst_size;     /* target picture size. */

    hi_vi_capture_select capture_select; /*
                                          * frame/field select. it is used only in interlaced mode.
                                          * for primary channels, capture_select is a static attribute
                                          */
    hi_vi_scan_mode scan_mode;           /* input scanning mode (progressive or interlaced) */
    hi_pixel_format pixel_format;        /*
                                          * pixel storage format.
                                          * only the formats semi-planar420, semi-planar422 or yuv400 are supported
                                          */
    hi_bool mirror_en;                   /* whether to mirror */
    hi_bool flip_en;                     /* whether to flip */
    hi_compress_mode compress_mode;      /* segment compress or no compress */
    hi_s32 src_frame_rate; /* source frame rate. the value -1 indicates that the frame rate is not controlled */
    hi_s32 dst_frame_rate; /* target frame rate. the value -1 indicates that the frame rate is not controlled */
} hi_vi_chn_attr;

typedef struct {
    hi_bool enable;                    /* whether this channel is enabled */
    hi_u32 interrupt_cnt;              /* the video frame interrupt count */
    hi_u32 frame_rate;                 /* current frame rate */
    hi_u32 lost_interrupt_cnt;         /* the interrupt is received but nobody care */
    hi_u32 vb_fail_cnt;                /* video buffer malloc failure */
    hi_u32 pic_width;                  /* current pic width */
    hi_u32 pic_height;                 /* current pic height */
    hi_u32 auto_disable_interrupt_cnt; /* auto disable interrupt count, when VIU detected too many interrupts */
    hi_u32 cc_err_cnt;                 /* capture complete err count */
} hi_vi_chn_status;

typedef enum {
    HI_VI_USER_PIC_MODE_PIC = 0, /* YUV picture */
    HI_VI_USER_PIC_MODE_BG,      /* background picture only with a color */
    HI_VI_USER_PIC_MODE_BUTT,
} hi_vi_user_pic_mode;

typedef struct {
    hi_u32 bg_color;
} hi_vi_user_pic_bg_info;

typedef struct {
    hi_bool pub_en; /* whether the user picture information is shared by all VI devices and channels */
    hi_vi_user_pic_mode user_pic_mode; /* user picture mode */
    union {
        /*
         * information about a YUV picture
         * AUTO:hi_vi_user_pic_mode:HI_VI_USER_PIC_MODE_PIC;
         */
        hi_video_frame_info user_pic_frame_info;
        /*
         * information about a background picture only with a color
         * AUTO:hi_vi_user_pic_mode:HI_VI_USER_PIC_MODE_BG;
         */
        hi_vi_user_pic_bg_info user_pic_bg_info;
    } user_pic_info;
} hi_vi_user_pic_attr;


typedef struct {
    hi_u32 luma; /* luma sum of current frame */
    hi_u64 pts;  /* PTS of current frame */
} hi_vi_chn_luma;

typedef enum {
    HI_VI_SKIP_NONE = 0, /* no skip */
    HI_VI_SKIP_YXYX,     /* 1/2 skip,1010 */
    HI_VI_SKIP_YXXX,     /* 1/4 skip,1000 */
    HI_VI_SKIP_YXXY,     /* 1/2 skip,1001 */
    HI_VI_SKIP_BUTT,
} hi_vi_skip_mode;

typedef struct {
    hi_bool enable;
    hi_u32 y_mask; /* ymask,0000-1111 in binary */
    hi_u32 c_mask; /* cmask,0000-1111 in binary */
} hi_vi_skip_mode_ex;

typedef struct {
    hi_s32 detect_err_frame;
    hi_s32 drop_err_frame;
    hi_s32 int_gap_time;
    hi_s32 min_cas_gap;
    hi_s32 max_cas_gap;
    hi_s32 stop_int_level;
    hi_s32 int_time;
    hi_s32 discard_int;
    hi_s32 yc_reverse;
    hi_s32 compat_mode;
    hi_s32 time_ref_gap;
} hi_vi_mod_param;

#ifdef __cplusplus
}
#endif

#endif