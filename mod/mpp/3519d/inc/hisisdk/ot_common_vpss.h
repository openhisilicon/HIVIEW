/*
  Copyright (c), 2001-2024, Shenshu Tech. Co., Ltd.
 */

#ifndef OT_COMMON_VPSS_H
#define OT_COMMON_VPSS_H

#include "ot_type.h"
#include "ot_common.h"
#include "ot_errno.h"
#include "ot_common_video.h"
#include "ot_common_sys.h"

#define OT_ERR_VPSS_NULL_PTR           OT_DEFINE_ERR(OT_ID_VPSS, OT_ERR_LEVEL_ERROR, OT_ERR_NULL_PTR)
#define OT_ERR_VPSS_NOT_READY          OT_DEFINE_ERR(OT_ID_VPSS, OT_ERR_LEVEL_ERROR, OT_ERR_NOT_READY)
#define OT_ERR_VPSS_INVALID_DEV_ID     OT_DEFINE_ERR(OT_ID_VPSS, OT_ERR_LEVEL_ERROR, OT_ERR_INVALID_DEV_ID)
#define OT_ERR_VPSS_INVALID_CHN_ID     OT_DEFINE_ERR(OT_ID_VPSS, OT_ERR_LEVEL_ERROR, OT_ERR_INVALID_CHN_ID)
#define OT_ERR_VPSS_EXIST              OT_DEFINE_ERR(OT_ID_VPSS, OT_ERR_LEVEL_ERROR, OT_ERR_EXIST)
#define OT_ERR_VPSS_UNEXIST            OT_DEFINE_ERR(OT_ID_VPSS, OT_ERR_LEVEL_ERROR, OT_ERR_UNEXIST)
#define OT_ERR_VPSS_NOT_SUPPORT        OT_DEFINE_ERR(OT_ID_VPSS, OT_ERR_LEVEL_ERROR, OT_ERR_NOT_SUPPORT)
#define OT_ERR_VPSS_NOT_PERM           OT_DEFINE_ERR(OT_ID_VPSS, OT_ERR_LEVEL_ERROR, OT_ERR_NOT_PERM)
#define OT_ERR_VPSS_NO_MEM             OT_DEFINE_ERR(OT_ID_VPSS, OT_ERR_LEVEL_ERROR, OT_ERR_NO_MEM)
#define OT_ERR_VPSS_NO_BUF             OT_DEFINE_ERR(OT_ID_VPSS, OT_ERR_LEVEL_ERROR, OT_ERR_NO_BUF)
#define OT_ERR_VPSS_SIZE_NOT_ENOUGH    OT_DEFINE_ERR(OT_ID_VPSS, OT_ERR_LEVEL_ERROR, OT_ERR_SIZE_NOT_ENOUGH)
#define OT_ERR_VPSS_ILLEGAL_PARAM      OT_DEFINE_ERR(OT_ID_VPSS, OT_ERR_LEVEL_ERROR, OT_ERR_ILLEGAL_PARAM)
#define OT_ERR_VPSS_BUSY               OT_DEFINE_ERR(OT_ID_VPSS, OT_ERR_LEVEL_ERROR, OT_ERR_BUSY)
#define OT_ERR_VPSS_BUF_EMPTY          OT_DEFINE_ERR(OT_ID_VPSS, OT_ERR_LEVEL_ERROR, OT_ERR_BUF_EMPTY)

#define OT_VPSS_CHN0                   0
#define OT_VPSS_CHN1                   1
#define OT_VPSS_CHN2                   2
#define OT_VPSS_CHN3                   3
#define OT_VPSS_DIRECT_CHN             31
#define OT_VPSS_INVALID_CHN            (-1)

/* Define de-interlace mode */
typedef enum {
    OT_VPSS_DEI_MODE_OFF = 0,
    OT_VPSS_DEI_MODE_ON,
    OT_VPSS_DEI_MODE_AUTO,
    OT_VPSS_DEI_MODE_BUTT
} ot_vpss_dei_mode;

typedef struct {
    td_bool             ie_en;                    /* RW; Range: [0, 1]; Image enhance enable. */
    td_bool             dci_en;                   /* RW; Range: [0, 1]; Dynamic contrast improve enable. */
    td_bool             buf_share_en;             /* RW; Range: [0, 1]; Buffer share enable. */
    td_bool             mcf_en;                   /* RW; Range: [0, 1]; MCF enable. */
    /* RW; Range: Hi3519DV500 = [64, 8192]; Width of source image. */
    td_u32              max_width;
    /* RW; Range: Hi3519DV500 = [64, 8192]; Height of source image. */
    td_u32              max_height;
    /* RW; Width of max dei image. */
    td_u32              max_dei_width;
    /* RW; Height of max dei image. */
    td_u32              max_dei_height;
    ot_dynamic_range    dynamic_range;            /* RW; Dynamic range of source image. */
    ot_pixel_format     pixel_format;             /* RW; Pixel format of source image. */
    ot_vpss_dei_mode    dei_mode;                 /* RW; De-interlace enable. */
    ot_vpss_chn         buf_share_chn;            /* RW; Buffer share channel. */
    ot_frame_rate_ctrl  frame_rate;               /* RW; Group frame rate control. */
} ot_vpss_grp_attr;

typedef enum {
    OT_VPSS_CHN_MODE_AUTO = 0,                    /* Auto mode. */
    OT_VPSS_CHN_MODE_USER,                        /* User mode. */
    OT_VPSS_CHN_MODE_BUTT
} ot_vpss_chn_mode;

typedef struct {
    td_bool             mirror_en;                /* RW; Range: [0, 1]; Mirror enable. */
    td_bool             flip_en;                  /* RW; Range: [0, 1]; Flip enable. */
    td_bool             border_en;                /* RW; Range: [0, 1]; Border enable. */
    /* RW; range: Hi3519DV500 = [64, 8192]; Width of target image. */
    td_u32              width;
    /* RW; range: Hi3519DV500 = [64, 8192]; Height of target image. */
    td_u32              height;
    td_u32              depth;                    /* RW; Range: [0, 8]; Depth of chn image list. */
    ot_vpss_chn_mode    chn_mode;                 /* RW; Work mode of vpss channel. */
    ot_video_format     video_format;             /* RW; Video format of target image. */
    ot_dynamic_range    dynamic_range;            /* RW; Dynamic range of target image. */
    ot_pixel_format     pixel_format;             /* RW; Pixel format of target image. */
    ot_compress_mode    compress_mode;            /* RW; Compression mode of the output. */
    ot_frame_rate_ctrl  frame_rate;               /* RW; Frame rate control info. */
    ot_border           border_attr;              /* RW; Border info. */
    ot_aspect_ratio     aspect_ratio;             /* RW; Aspect ratio info. */
} ot_vpss_chn_attr;

typedef struct {
    ot_vpss_chn            bind_chn;
    ot_ext_chn_src_type    src_type;
    td_u32                 width;
    td_u32                 height;
    td_u32                 depth;
    ot_video_format        video_format;
    ot_dynamic_range       dynamic_range;
    ot_pixel_format        pixel_format;
    ot_compress_mode       compress_mode;
    ot_frame_rate_ctrl     frame_rate;
} ot_vpss_ext_chn_attr;

typedef struct {
    td_bool                  enable;              /* RW; Range: [0, 1]; Crop enable. */
    ot_coord                 crop_mode;           /* RW; Coordinate mode of crop. */
    ot_rect                  crop_rect;           /* RW; Crop rectangular. */
} ot_vpss_crop_info;

typedef struct {
    td_u32          contrast;             /* RW; Range: [0, 63]; Strength of dynamic contrast improve. */
    td_u32          ie_strength;          /* RW; Range: [0, 63]; Strength of image enhance. */
} ot_vpss_grp_param;

typedef struct {
    td_bool          enable;    /* RW; Range: [0, 1]; save gdc out lut or not */
    td_u32           buf_size;  /* RW; buf size of out lut */
} ot_vpss_out_lut_cfg;

typedef struct {
    td_bool             is_rotation_input_support;  /* RW; Range: [0, 1]; is a rotation input or not. default false */
    td_bool             is_dis_gyro_support;        /* RW; Range: [0, 1]; is a dis gyro grp or not. default false */
    td_bool             is_motion_denoise_support;  /* RW; Range: [0, 1]; is motion_denoise or not. default false */
    td_u32              max_split_num;              /* RW; Max node number of split. */
    td_u32              max_out_rgn_num;            /* RW; Max region number for fisheye or stitch. */
    ot_vpss_out_lut_cfg lut_cfg;
} ot_vpss_grp_cfg;

typedef struct {
    td_u32             rgn_num;     /* W; count of the rgn */
    ot_rect ATTRIBUTE* rgn;         /* W; rgn attribute */
} ot_vpss_rgn_info;

typedef struct {
    td_u32 max_out_width;           /* RW; Range: [64, 8192]; max chn out width, default 8192. */
    td_u32 max_out_height;          /* RW; Range: [64, 8192]; max chn out height, default 8192. */
} ot_vpss_mod_param;

#endif /* OT_COMMON_VPSS_H */
