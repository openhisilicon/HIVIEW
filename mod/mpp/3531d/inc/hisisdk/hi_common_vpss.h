/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2019-2020. All rights reserved.
 * Description: Vpss common interface
 * Author: Hisilicon multimedia software group
 * Create: 2019/07/30
 */

#ifndef __HI_COMMON_VPSS_H__
#define __HI_COMMON_VPSS_H__

#include "hi_type.h"
#include "hi_common.h"
#include "hi_errno.h"
#include "hi_common_video.h"


#define HI_ERR_VPSS_NULL_PTR           HI_DEFINE_ERR(HI_ID_VPSS, HI_ERR_LEVEL_ERROR, HI_ERR_NULL_PTR)
#define HI_ERR_VPSS_NOT_READY          HI_DEFINE_ERR(HI_ID_VPSS, HI_ERR_LEVEL_ERROR, HI_ERR_NOT_READY)
#define HI_ERR_VPSS_INVALID_DEV_ID     HI_DEFINE_ERR(HI_ID_VPSS, HI_ERR_LEVEL_ERROR, HI_ERR_INVALID_DEV_ID)
#define HI_ERR_VPSS_INVALID_CHN_ID     HI_DEFINE_ERR(HI_ID_VPSS, HI_ERR_LEVEL_ERROR, HI_ERR_INVALID_CHN_ID)
#define HI_ERR_VPSS_EXIST              HI_DEFINE_ERR(HI_ID_VPSS, HI_ERR_LEVEL_ERROR, HI_ERR_EXIST)
#define HI_ERR_VPSS_UNEXIST            HI_DEFINE_ERR(HI_ID_VPSS, HI_ERR_LEVEL_ERROR, HI_ERR_UNEXIST)
#define HI_ERR_VPSS_NOT_SUPPORT        HI_DEFINE_ERR(HI_ID_VPSS, HI_ERR_LEVEL_ERROR, HI_ERR_NOT_SUPPORT)
#define HI_ERR_VPSS_NOT_PERM           HI_DEFINE_ERR(HI_ID_VPSS, HI_ERR_LEVEL_ERROR, HI_ERR_NOT_PERM)
#define HI_ERR_VPSS_NO_MEM             HI_DEFINE_ERR(HI_ID_VPSS, HI_ERR_LEVEL_ERROR, HI_ERR_NO_MEM)
#define HI_ERR_VPSS_NO_BUF             HI_DEFINE_ERR(HI_ID_VPSS, HI_ERR_LEVEL_ERROR, HI_ERR_NO_BUF)
#define HI_ERR_VPSS_SIZE_NOT_ENOUGH    HI_DEFINE_ERR(HI_ID_VPSS, HI_ERR_LEVEL_ERROR, HI_ERR_SIZE_NOT_ENOUGH)
#define HI_ERR_VPSS_ILLEGAL_PARAM      HI_DEFINE_ERR(HI_ID_VPSS, HI_ERR_LEVEL_ERROR, HI_ERR_ILLEGAL_PARAM)
#define HI_ERR_VPSS_BUSY               HI_DEFINE_ERR(HI_ID_VPSS, HI_ERR_LEVEL_ERROR, HI_ERR_BUSY)
#define HI_ERR_VPSS_BUF_EMPTY          HI_DEFINE_ERR(HI_ID_VPSS, HI_ERR_LEVEL_ERROR, HI_ERR_BUF_EMPTY)

#define HI_VPSS_CHN0                   0
#define HI_VPSS_CHN1                   1
#define HI_VPSS_CHN2                   2
#define HI_VPSS_CHN3                   3
#define HI_VPSS_INVALID_CHN            (-1)

/* Define de-interlace mode */
typedef enum {
    HI_VPSS_DEI_MODE_OFF = 0,
    HI_VPSS_DEI_MODE_ON,
    HI_VPSS_DEI_MODE_AUTO,
    HI_VPSS_DEI_MODE_BUTT
} hi_vpss_dei_mode;

typedef enum {
    HI_VPSS_NR_TYPE_VIDEO_NORM = 0,
    HI_VPSS_NR_TYPE_SNAP_NORM,
    HI_VPSS_NR_TYPE_VIDEO_SPATIAL,
    HI_VPSS_NR_TYPE_VIDEO_ENHANCED,
    HI_VPSS_NR_TYPE_BUTT
} hi_vpss_nr_type;

typedef enum {
    HI_VPSS_NR_MOTION_MODE_NORM = 0,              /* Normal. */
    HI_VPSS_NR_MOTION_MODE_COMPENSATION,          /* Motion compensate. */
    HI_VPSS_NR_MOTION_MODE_BUTT
} hi_vpss_nr_motion_mode;

typedef struct {
    hi_vpss_nr_type     nr_type;                  /* RW; NR type. */
    hi_compress_mode    compress_mode;            /* RW; Reference frame compression mode. */
    hi_vpss_nr_motion_mode   nr_motion_mode;      /* RW; NR motion compensate mode. */
} hi_vpss_nr_attr;

typedef struct {
    hi_bool             nr_en;                    /* RW; Range: [0, 1]; NR enable. */
    hi_bool             ie_en;                    /* RW; Range: [0, 1]; Image enhance enable. */
    hi_bool             dci_en;                   /* RW; Range: [0, 1]; Dynamic contrast improve enable. */
    hi_bool             buf_share_en;             /* RW; Range: [0, 1]; Buffer share enable. */
    hi_bool             mcf_en;                   /* RW; Range: [0, 1]; MCF enable. */
    /* RW; Range: Hi3531DV200/Hi3521DV200 = [64, 16384]; Width of source image. */
    hi_u32              max_width;
    /* RW; Range: Hi3531DV200/Hi3521DV200 = [64, 8192]; Height of source image. */
    hi_u32              max_height;
    /* RW; Range: Hi3531DV200/Hi3521DV200 = [64, 960]; Width of max dei image. */
    hi_u32              max_dei_width;
    /* RW; Range: Hi3531DV200/Hi3521DV200 = [64, 4096]; Height of max dei image. */
    hi_u32              max_dei_height;
    hi_dynamic_range    dynamic_range;            /* RW; Dynamic range of source image. */
    hi_pixel_format     pixel_format;             /* RW; Pixel format of source image. */
    hi_vpss_dei_mode    dei_mode;                 /* RW; De-interlace enable. */
    hi_vpss_chn         buf_share_chn;            /* RW; Buffer share channel. */
    hi_vpss_nr_attr     nr_attr;                  /* RW; NR attr. */
    hi_frame_rate_ctrl  frame_rate;               /* RW; Group frame rate control. */
} hi_vpss_grp_attr;

typedef enum {
    HI_VPSS_CHN_MODE_AUTO = 0,                    /* Auto mode. */
    HI_VPSS_CHN_MODE_USER,                        /* User mode. */
    HI_VPSS_CHN_MODE_BUTT
} hi_vpss_chn_mode;

typedef struct {
    hi_bool             mirror_en;                /* RW; Range: [0, 1]; Mirror enable. */
    hi_bool             flip_en;                  /* RW; Range: [0, 1]; Flip enable. */
    hi_bool             border_en;                /* RW; Range: [0, 1]; Border enable. */
    /* RW; range: Hi3531DV200/Hi3521DV200 = [64, 16384]; Width of target image. */
    hi_u32              width;
    /* RW; range: Hi3531DV200/Hi3521DV200 = [64, 8192]; Height of target image. */
    hi_u32              height;
    hi_u32              depth;                    /* RW; Range: [0, 8]; Depth of chn image list. */
    hi_vpss_chn_mode    chn_mode;                 /* RW; Work mode of vpss channel. */
    hi_video_format     video_format;             /* RW; Video format of target image. */
    hi_dynamic_range    dynamic_range;            /* RW; Dynamic range of target image. */
    hi_pixel_format     pixel_format;             /* RW; Pixel format of target image. */
    hi_compress_mode    compress_mode;            /* RW; Compression mode of the output. */
    hi_frame_rate_ctrl  frame_rate;               /* RW; Frame rate control info. */
    hi_border           border_attr;              /* RW; Border info. */
    hi_aspect_ratio     aspect_ratio;             /* RW; Aspect ratio info. */
} hi_vpss_chn_attr;

/* Define vpss filter info */
typedef struct {
    hi_bool             enable;                   /* RW; Range: [0, 1]; Image filter enable. */
    hi_size             size;                     /* RW; Image filter size info. */
} hi_vpss_sizer_info;

typedef struct {
    hi_bool                  enable;              /* RW; Range: [0, 1]; Crop enable. */
    hi_coord                 crop_mode;           /* RW; Coordinate mode of crop. */
    hi_rect                  crop_rect;           /* RW; Crop rectangular. */
} hi_vpss_crop_info;

/* Only used for Hi3531DV200/Hi3521DV200 */
typedef struct {
    hi_u32                   sf_strength;         /* RW; Range: [0, 255]; Strength of y space filter. */
    hi_u32                   tf_strength;         /* RW; Range: [0, 63]; Strength of y time filter. */
    hi_u32                   csf_strength;        /* RW; Range: [0, 255]; Strength of c space filter. */
    hi_u32                   ctf_strength;        /* RW; Range: [0, 32]; Strength of c time filter. */
    hi_u32                   cvbs_strength;       /* RW; Range: [0, 32]; Strength of cvbs. */
} hi_vpss_nrx_norm_param_v1;

/*
 * Only used for Hi3531DV200/Hi3521DV200
 * RW; Advanced NR Param
 */
typedef struct {
    hi_u32                   mdz;                 /* RW; Range: [0, 63]; */
    hi_u32                   hard_tf_strength;    /* RW; Range: [0, 96]; Relation strength of hard NR channel. */
    /* Only for weak nr chn */
    hi_u32                   edz;                 /* RW; Range: [0, 63]; */
    hi_u32                   weak_tf_strength;    /* RW; Range: [0, 63]; Relation strength of weak NR channel. */
} hi_vpss_nrx_adv_param;

/* Only used for Hi3531DV200/Hi3521DV200 */
typedef enum {
    HI_VPSS_NR_V1 = 1,
    HI_VPSS_NR_BUTT
} hi_vpss_nr_version;

typedef struct {
    hi_vpss_nr_version       nr_version;          /* RW; Version of NR parameters. */
    union {
        /*
         * RW; NR normal parameters for Hi3531DV200/Hi3521DV200.
         * AUTO:hi_vpss_nr_version:HI_VPSS_NR_V1;
         */
        hi_vpss_nrx_norm_param_v1 nrx_norm_param_v1;
    };
} hi_vpss_grp_nrx_param;

typedef struct {
    hi_u32                  contrast;             /* RW; Range: [0, 63]; Strength of dynamic contrast improve. */
    hi_u32                  ie_strength;          /* RW; Range: [0, 63]; Strength of image enhance. */
    hi_vpss_grp_nrx_param   nrx_param;            /* RW; Parameters of NR. */
} hi_vpss_grp_param;

typedef struct {
    hi_bool                 enable;               /* RW; Range: [0, 1]; Sharpen enable. */
    hi_u32                  strength;             /* RW; Range: [0, 100]; Sharpen strength. */
} hi_vpss_chn_sharpen_param;

typedef struct {
    hi_u32                  split_node_num;        /* RW; Range: [1, 16]; Max num of split node. */
    hi_bool                 coverex_ratio_support; /* RW; Range: [0, 1]; Coverex support ratio coordinate. */
} hi_vpss_mod_param;

typedef struct {
    hi_size                 img_size;             /* R; Image size info. */
    hi_u64 ATTRIBUTE        *luma_data;           /* R; Luma data. */
} hi_vpss_luma_result;

typedef enum {
    HI_VPSS_COLOR_RANGE_LIMITED = 0,
    HI_VPSS_COLOR_RANGE_FULL,
    HI_VPSS_COLOR_RANGE_BUTT
} hi_vpss_color_range;

typedef struct {
    hi_u8 weight;   /* RW; Range: [0, 255]; Weight of Gaussian distribution. */
    hi_u8 sigma;    /* RW; Range: [1, 255]; Sigma of Gaussian distribution. */
    hi_u8 mean;     /* RW; Range: [0, 255]; Mean of Gaussian distribution. */
} hi_vpss_ldci_gauss_coef_attr;

typedef struct {
    /* RW; Weight curve, the degree of brightening can be controlled based on the luminance of original images. */
    hi_vpss_ldci_gauss_coef_attr positive_attr;
    /* RW; Weight curve, the degree of darkening can be controlled based on the luminance of original images. */
    hi_vpss_ldci_gauss_coef_attr negative_attr;
} hi_vpss_ldci_he_weight_attr;

typedef struct {
    hi_bool enable;                     /* RW; Range: [0, 1]; LDCI enable. */
    hi_vpss_color_range color_range;    /* RW; Color range of input yuv image. */
    hi_u16 black_ctrl_strength;         /* RW; Range: [0, 511]; Restrain dark region. */
    /*
     * RW; Used for controlling the weight curve of blending
     * between histogram equalization results and original images.
     * The curve is Gaussian-distributed with the luminance as the horizontal coordinate.
     */
    hi_vpss_ldci_he_weight_attr he_weight_attr;
} hi_vpss_ldci_attr;

#endif /* __HI_COMMON_VPSS_H__ */
