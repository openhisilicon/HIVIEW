/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
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
#define OT_VPSS_INVALID_CHN            (-1)

#define OT_VPSS_S_IDX_LEN                 17
#define OT_VPSS_3DNR_MAX_AUTO_PARAM_NUM   16

/* Define de-interlace mode */
typedef enum {
    OT_VPSS_DEI_MODE_OFF = 0,
    OT_VPSS_DEI_MODE_ON,
    OT_VPSS_DEI_MODE_AUTO,
    OT_VPSS_DEI_MODE_BUTT
} ot_vpss_dei_mode;

typedef enum {
    OT_VPSS_NR_TYPE_VIDEO_NORM = 0,
    OT_VPSS_NR_TYPE_SNAP_NORM,
    OT_VPSS_NR_TYPE_VIDEO_SPATIAL,
    OT_VPSS_NR_TYPE_VIDEO_ENHANCED,
    OT_VPSS_NR_TYPE_BUTT
} ot_vpss_nr_type;

typedef enum {
    OT_VPSS_NR_MOTION_MODE_NORM = 0,              /* Normal. */
    OT_VPSS_NR_MOTION_MODE_COMPENSATION,          /* Motion compensate. */
    OT_VPSS_NR_MOTION_MODE_BUTT
} ot_vpss_nr_motion_mode;

typedef struct {
    ot_vpss_nr_type     nr_type;                  /* RW; NR type. */
    ot_compress_mode    compress_mode;            /* RW; Reference frame compression mode. */
    ot_vpss_nr_motion_mode   nr_motion_mode;      /* RW; NR motion compensate mode. */
} ot_vpss_nr_attr;

typedef struct {
    td_bool             nr_en;                    /* RW; Range: [0, 1]; NR enable. */
    td_bool             ie_en;                    /* RW; Range: [0, 1]; Image enhance enable. */
    td_bool             dci_en;                   /* RW; Range: [0, 1]; Dynamic contrast improve enable. */
    td_bool             buf_share_en;             /* RW; Range: [0, 1]; Buffer share enable. */
    td_bool             mcf_en;                   /* RW; Range: [0, 1]; MCF enable. */
    /* RW; Range: SS528V100/SS524V100 = [64, 16384] | SS928V100 = [64, 8192]; Width of source image. */
    td_u32              max_width;
    /* RW; Range: SS528V100/SS524V100/SS928V100 = [64, 8192]; Height of source image. */
    td_u32              max_height;
    /* RW; Range: SS528V100/SS524V100 = [64, 960]; Width of max dei image. */
    td_u32              max_dei_width;
    /* RW; Range: SS528V100/SS524V100 = [64, 4096]; Height of max dei image. */
    td_u32              max_dei_height;
    ot_dynamic_range    dynamic_range;            /* RW; Dynamic range of source image. */
    ot_pixel_format     pixel_format;             /* RW; Pixel format of source image. */
    ot_vpss_dei_mode    dei_mode;                 /* RW; De-interlace enable. */
    ot_vpss_chn         buf_share_chn;            /* RW; Buffer share channel. */
    ot_vpss_nr_attr     nr_attr;                  /* RW; NR attr. */
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
    /* RW; range: SS528V100/SS524V100/SS928V100 = [64, 16384]; Width of target image. */
    td_u32              width;
    /* RW; range: SS528V100/SS524V100/SS928V100 = [64, 8192]; Height of target image. */
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

/* Define vpss filter info */
typedef struct {
    td_bool             enable;                   /* RW; Range: [0, 1]; Image filter enable. */
    ot_size             size;                     /* RW; Image filter size info. */
} ot_vpss_sizer_info;

typedef struct {
    td_bool                  enable;              /* RW; Range: [0, 1]; Crop enable. */
    ot_coord                 crop_mode;           /* RW; Coordinate mode of crop. */
    ot_rect                  crop_rect;           /* RW; Crop rectangular. */
} ot_vpss_crop_info;

/* Only used for SS528V100/SS524V100 */
typedef struct {
    td_u32                   sf_strength;         /* RW; Range: [0, 255]; Strength of y space filter. */
    td_u32                   tf_strength;         /* RW; Range: [0, 63]; Strength of y time filter. */
    td_u32                   csf_strength;        /* RW; Range: [0, 255]; Strength of c space filter. */
    td_u32                   ctf_strength;        /* RW; Range: [0, 32]; Strength of c time filter. */
    td_u32                   cvbs_strength;       /* RW; Range: [0, 32]; Strength of cvbs. */
} ot_vpss_nrx_norm_param_v1;

typedef struct {
    td_u8   ies0;   /* RW; Range: [0, 255]; The gains of edge and texture enhancement. */
    td_u8   ies1;   /* RW; Range: [0, 255]; The gains of edge and texture enhancement. */
    td_u8   ies2;   /* RW; Range: [0, 255]; The gains of edge and texture enhancement. */
    td_u8   ies3;   /* RW; Range: [0, 255]; The gains of edge and texture enhancement. */
    td_u16  iedz;   /* RW; Range: [0, 999]; The threshold to control the generated artifacts. */
} ot_vpss_nrx_v2_iey;

typedef struct {
    struct {
        td_u16 spn    : 4;  /* RW; Range: [0,   6]; The selection of filters to be mixed for NO.7 filter. */
        td_u16 sbn    : 4;  /* RW; Range: [0,   6]; The selection of filters to be mixed for NO.7 filter. */
        td_u16 pbr    : 5;  /* RW; Range: [0,  16]; The mix ratio between spn and sbr. */
        td_u16 j_mode : 3;  /* RW; Range: [0,   4]; The selection modes for the blending of spatial filters. */
    };
    td_u8   sfr7[4];    /* RW; Range: [0, 31]; The relative NR strength for NO.7 filter. (Effective when jmode = 4). */
    td_u8   sbr7[2];    /* RW; Range: [0, 15]; The control of overshoot and undershoot. */
    td_u8   sfs1;       /* RW; Range: [0, 255]; The NR strength parameters for NO.1 filter. */
    td_u8   sbr1;       /* RW; Range: [0, 255]; The NR strength parameters for NO.1 filter. */
    td_u8   sfs2;       /* RW; Range: [0, 255]; The NR strength parameters for NO.2 filter. */
    td_u8   sft2;       /* RW; Range: [0, 255]; The NR strength parameters for NO.2 filter. */
    td_u8   sbr2;       /* RW; Range: [0, 255]; The NR strength parameters for NO.2 filter. */
    td_u8   sfs4;       /* RW; Range: [0, 255]; The NR strength parameters for NO.3 and NO.4 filters. */
    td_u8   sft4;       /* RW; Range: [0, 255]; The NR strength parameters for NO.3 and NO.4 filters. */
    td_u8   sbr4;       /* RW; Range: [0, 255]; The NR strength parameters for NO.3 and NO.4 filters. */
    /*
     * sth1_0, sth2_0, sth3_0; Range: [0, 511]; The thresholds for protection of edges on foreground from blurring.
     * sth1_1, sth2_1, sth3_1; Range: [0, 511]; The thresholds for protection of edges on background from blurring.
     * sf5_md; Range: [0, 1]; The strength control of NO.5 filter.
     * sf8_idx0, sf8_idx1; Range: [0, 7]; The filter selection for NO.8.
     * bri_idx0, bri_idx1; Range: [0, 7]; The filter selection based on brightness
     * for foreground and background, respectively.
     */
    struct {
        td_u16 sth1_0 : 9;
        td_u16 sf5_md : 1;
        td_u16 _rb1_  : 6;
    };
    struct {
        td_u16 sth2_0   : 9;
        td_u16 sf8_idx0 : 3;
        td_u16 bri_idx0 : 4;
    };
    struct {
        td_u16 sth3_0   : 9;
        td_u16 sf8_idx1 : 3;
        td_u16 bri_idx1 : 4;
    };
    td_u16  sth1_1;
    td_u16  sth2_1;
    td_u16  sth3_1;
    /*
     * sfn0_0~sfn3_0; Range: [0, 8]; Filter selection for different foreground image areas based on sth1_0~sth3_0.
     * sfn0_1~sfn3_1; Range: [0, 8]; Filter selection for different background image areas based on sth1_1~sth3_1.
     */
    struct {
        td_u16 sfn0_0 : 4;
        td_u16 sfn1_0 : 4;
        td_u16 sfn2_0 : 4;
        td_u16 sfn3_0 : 4;
    };
    struct {
        td_u16 sfn0_1 : 4;
        td_u16 sfn1_1 : 4;
        td_u16 sfn2_1 : 4;
        td_u16 sfn3_1 : 4;
    };
    td_u8   sf8_tfr;    /* RW ; Range: [0, 255]; The blending strength for NO. 8 filter. */
    td_u8   sf8_thrd;   /* RW ; Range: [0, 255]; The blending strength for NO. 8 filter. */
    td_u8   sfr;        /* RW ; Range: [0, 31]; The relative NR strength to the sfi and sfk filter. */
    td_u8   bri_str[OT_VPSS_S_IDX_LEN]; /* RW ; Range: [0, 255];  Spatial NR strength based on brightness. */
} ot_vpss_nrx_v2_sfy;

typedef struct {
    struct {
        td_u16 tfs0  : 4;   /* RW; Range: [0,  15]; The NR strength for temporal filtering. */
        td_u16 tdz0  : 10;  /* RW; Range: [0, 999]; Protection of the weak texturearea from temporal filtering. */
        td_u16 _rb0_ : 2;
    };
    struct {
        td_u16 tfs1      : 4;   /* RW; Range: [0,  15]; The NR strength for temporal filtering. */
        td_u16 tdz1      : 10;  /* RW; Range: [0, 999]; Protection of the weak texture area from temporal filtering. */
        td_u16 math_mode : 2;   /* RW; Range: [0,   1]; The motion detection mode. */
    };
    struct {
        td_u16 sdz0   : 10; /* RW; Range: [0, 999]; The threshold of NR control for result mai00. */
        td_u16 str0   : 5;  /* RW; Range: [0,  31]; The strength  of NR control for result mai00. */
        td_u16 ref_en : 1;  /* RW; Range: [0,   1]; The enable switch for reference frame. */
    };
    struct {
        td_u16 sdz1  : 10;  /* RW; Range: [0, 999]; The threshold of NR control for result mai00. */
        td_u16 str1  : 5;   /* RW; Range: [0,  31]; The strength  of NR control for result mai00. */
        td_u16 _rb1_ : 1;
    };
    td_u8   tss0;       /* RW; Range: [0,  15]; The ratio for blending spatial NR with the temporal NR results. */
    td_u8   tss1;       /* RW; Range: [0,  15]; The ratio for blending spatial NR with the temporal NR results. */
    td_u16  auto_math;  /* RW; Range: [0, 999]; The motion threshold for the Level 0 denoise. */
    td_u8   tfr0[6];    /* RW; Range: [0,  31]; The temporal NR strength control for background (static) area. */
    td_u8   tfr1[6];    /* RW; Range: [0,  31]; The temporal NR strength control for background (static) area. */
} ot_vpss_nrx_v2_tfy;

typedef struct {
    struct {
        td_u16 math0 : 10;  /* RW; Range: [0, 999]; The threshold for motion detection. */
        td_u16 mate0 : 4;   /* RW; Range: [0,   8]; The motion index for smooth image area. */
        td_u16 mai00 : 2;   /* RW; Range: [0,   3]; The blending results between spatial and temporal filtering. */
    };
    struct {
        td_u16 math1 : 10;  /* RW; Range: [0, 999]; The threshold for motion detection. */
        td_u16 mate1 : 4;   /* RW; Range: [0,   8]; The motion index for smooth image area. */
        td_u16 mai02 : 2;   /* RW; Range: [0,   3]; The blending results between spatial and temporal filtering. */
    };
    td_u8   mabw0;          /* RW; Range: [0,   9]; The window size for motion detection. */
    td_u8   mabw1;          /* RW; Range: [5,   9]; The window size for motion detection. */
    struct {
        td_u16 adv_math : 1;    /* RW; Range: [0, 1]; The switch to active the advanced mode. */
        td_u16 adv_th   : 12;   /* RW; Range: [0, 511]; The threshold to control the effects of the adv_math. */
        td_u16 _rb_     : 3;
    };
} ot_vpss_nrx_v2_mdy;

typedef struct {
    td_u8   sfc;                /* RW; Range: [0, 31];  The chroma NR strength. */
    td_u8   sfc_enhance;        /* RW; Range: [0, 255];  The chroma NR strength for sfc. */
    td_u8   sfc_ext;            /* RW; Range: [0, 255];  The extra chroma NR strength for sfc. */
    td_u8   trc;                /* RW; Range: [0, 255];  The temporal chroma NR strength. */
    struct {
        td_u16 auto_math : 10;  /* RW; Range: [0, 999];  The motion threshold for chroma. */
        td_u16 tfc       : 6;   /* RW; Range: [0, 32];   The control of temporal chroma NR strength. */
    };
} ot_vpss_nrx_v2_nrc0;

typedef struct {
    td_u8   sfs1;       /* RW; Range: [0, 255];  The NR strength parameters for NO.1 filter. */
    td_u8   sbr1;       /* RW; Range: [0, 255];  The NR strength parameters for NO.1 filter. */
    td_u8   sfs2;       /* RW; Range: [0, 255];  The NR strength parameters for NO.2 filter. */
    td_u8   sft2;       /* RW; Range: [0, 255];  The NR strength parameters for NO.2 filter. */
    td_u8   sbr2;       /* RW; Range: [0, 255];  The NR strength parameters for NO.2 filter. */
    td_u8   sfs4;       /* RW; Range: [0, 255];  The NR strength parameters for NO.3 and NO.4 filter. */
    td_u8   sft4;       /* RW; Range: [0, 255];  The NR strength parameters for NO.3 and NO.4 filter. */
    td_u8   sbr4;       /* RW; Range: [0, 255];  The NR strength parameters for NO.3 and NO.4 filter. */
    td_u8   sfc6;       /* RW; Range: [0, 255];  The chroma NR strength for NO.6 filter. */
    td_u8   sfc_ext6;   /* RW; Range: [0, 255];  The extra chroma NR strength for NO.6 filter. */
    /* sfr6_u, sfr6_v; Range: [0, 15]; The chroma NR strength control for U and V, respectively, for NO.6 filter. */
    struct {
        td_u8 sfr6_u : 4;
        td_u8 sfr6_v : 4;
    };
    /*
     * sf5_str_u, sf5_str_v; Range: [0, 31]; The chroma NR strength for U and V, respectively, for NO.5 filter.
     * post_sfc; Range: [0, 31]; The chroma NR strength for sfc filter.
     */
    struct {
        td_u16 sf5_str_u : 5;
        td_u16 sf5_str_v : 5;
        td_u16 post_sfc  : 5;
        td_u16 _rb0_     : 1;
    };
    struct {
        td_u16 spn0 : 3;    /* RW; Range: [0,  6];  The selection of filters to be mixed for NO.7 filter. */
        td_u16 sbn0 : 3;    /* RW; Range: [0,  6];  The selection of filters to be mixed for NO.7 filter. */
        td_u16 pbr0 : 4;    /* RW; Range: [0, 15];  The mix ratio for NO.7 filter. */
        td_u16 spn1 : 3;    /* RW; Range: [0,  6];  The selection of filters to be mixed for NO.7 filter. */
        td_u16 sbn1 : 3;    /* RW; Range: [0,  6];  The selection of filters to be mixed for NO.7 filter. */
    };
    struct {
        td_u8 pbr1  : 4;    /* RW; Range: [0, 15];  The mix ratio for NO.7 filter. */
        td_u8 _rb1_ : 4;
    };
    struct {
        td_u8 sat0_l_sfn8 : 4;  /* RW; Range: [0, 7];  The selection of filters to be mixed for NO.8 filter. */
        td_u8 sat1_l_sfn8 : 4;  /* RW; Range: [0, 7];  The selection of filters to be mixed for NO.8 filter. */
    };
    struct {
        td_u8 sat0_h_sfn8 : 4;  /* RW; Range: [0, 7];  The selection of filters to be mixed for NO.8 filter. */
        td_u8 sat1_h_sfn8 : 4;  /* RW; Range: [0, 7];  The selection of filters to be mixed for NO.8 filter. */
    };
    struct {
        td_u8 hue0_l_sfn9 : 4;  /* RW; Range: [0, 8];  The selection of filters to be mixed for NO.9 filter. */
        td_u8 hue1_l_sfn9 : 4;  /* RW; Range: [0, 8];  The selection of filters to be mixed for NO.9 filter. */
    };
    struct {
        td_u8 hue0_h_sfn9 : 4;  /* RW; Range: [0, 8];  The selection of filters to be mixed for NO.9 filter. */
        td_u8 hue1_h_sfn9 : 4;  /* RW; Range: [0, 8];  The selection of filters to be mixed for NO.9 filter. */
    };
    struct {
        td_u8 bri0_l_sfn10 : 4; /* RW; Range: [0, 9];  The selection of filters to be mixed for NO.10 filter. */
        td_u8 bri1_l_sfn10 : 4; /* RW; Range: [0, 9];  The selection of filters to be mixed for NO.10 filter. */
    };
    struct {
        td_u8 bri0_h_sfn10 : 4; /* RW; Range: [0, 9];  The selection of filters to be mixed for NO.10 filter. */
        td_u8 bri1_h_sfn10 : 4; /* RW; Range: [0, 9];  The selection of filters to be mixed for NO.10 filter. */
    };
    struct {
        td_u8 sfn0 : 4; /* RW; Range: [0, 10]; The selection of filters for foreground and background, respectively. */
        td_u8 sfn1 : 4; /* RW; Range: [0, 10]; The selection of filters for foreground and background, respectively. */
    };
    td_u8   bak_grd_sat[OT_VPSS_S_IDX_LEN]; /* RW; Range: [0, 255]; Chroma NR strength based on saturation. */
    td_u8   for_grd_sat[OT_VPSS_S_IDX_LEN]; /* RW; Range: [0, 255]; Chroma NR strength based on saturation. */
    td_u8   bak_grd_hue[OT_VPSS_S_IDX_LEN]; /* RW; Range: [0, 255]; Chroma NR strength based on hue. */
    td_u8   for_grd_hue[OT_VPSS_S_IDX_LEN]; /* RW; Range: [0, 255]; Chroma NR strength based on hue. */
    td_u8   bak_grd_bri[OT_VPSS_S_IDX_LEN]; /* RW; Range: [0, 255]; Chroma NR strength based on brightness. */
    td_u8   for_grd_bri[OT_VPSS_S_IDX_LEN]; /* RW; Range: [0, 255]; Chroma NR strength based on brightness. */
} ot_vpss_nrx_v2_nrc1;

typedef struct {
    ot_vpss_nrx_v2_iey  iey[5];
    ot_vpss_nrx_v2_sfy  sfy[5];
    ot_vpss_nrx_v2_mdy  mdy[2];
    ot_vpss_nrx_v2_tfy  tfy[3];
    ot_vpss_nrx_v2_nrc0 nrc0;
    ot_vpss_nrx_v2_nrc1 nrc1;
    struct {
        td_u16 limit_range_en : 1;  /* RW; Range: [0, 1]; The switch for limit range mode. */
        td_u16 nry0_en        : 1;  /* RW; Range: [0, 1]; The switch for luma denoise. */
        td_u16 nry1_en        : 1;  /* RW; Range: [0, 1]; The switch for luma denoise. */
        td_u16 nry2_en        : 1;  /* RW; Range: [0, 1]; The switch for luma denoise. */
        td_u16 nry3_en        : 1;  /* RW; Range: [0, 1]; The switch for luma denoise. */
        td_u16 nrc0_en        : 1;  /* RW; Range: [0, 1]; The switch for chroma denoise. */
        td_u16 nrc1_en        : 1;  /* RW; Range: [0, 1]; The switch for chroma denoise. */
        td_u16 _rb_           : 9;
    };
} ot_vpss_nrx_v2;

typedef struct {
    ot_vpss_nrx_v2 nrx_param;
} ot_vpss_nrx_param_manual_v2;

typedef struct {
    td_u32 param_num;
    td_u32 ATTRIBUTE *iso;
    ot_vpss_nrx_v2 ATTRIBUTE *nrx_param;
} ot_vpss_nrx_param_auto_v2;

typedef struct {
    ot_op_mode op_mode;
    ot_vpss_nrx_param_manual_v2 nrx_manual;
    ot_vpss_nrx_param_auto_v2 nrx_auto;
} ot_vpss_nrx_norm_param_v2;

typedef struct {
    td_u32                   mdz;                 /* RW; Range: [0, 63]; */
    td_u32                   hard_tf_strength;    /* RW; Range: [0, 96]; Relation strength of hard NR channel. */
    /* Only for weak nr chn */
    td_u32                   edz;                 /* RW; Range: [0, 63]; */
    td_u32                   weak_tf_strength;    /* RW; Range: [0, 63]; Relation strength of weak NR channel. */
} ot_vpss_nrx_adv_param;

typedef enum {
    OT_VPSS_NR_V1 = 1,
    OT_VPSS_NR_V2 = 2,
    OT_VPSS_NR_BUTT
} ot_vpss_nr_version;

typedef struct {
    ot_vpss_nr_version       nr_version;                /* RW; Version of NR parameters. */
    union {
        ot_vpss_nrx_norm_param_v1 nrx_norm_param_v1;    /* AUTO: ot_vpss_nr_version:OT_VPSS_NR_V1. */
        ot_vpss_nrx_norm_param_v2 nrx_norm_param_v2;    /* AUTO: ot_vpss_nr_version:OT_VPSS_NR_V2. */
    };
} ot_vpss_grp_nrx_param;

typedef struct {
    td_u32                  contrast;             /* RW; Range: [0, 63]; Strength of dynamic contrast improve. */
    td_u32                  ie_strength;          /* RW; Range: [0, 63]; Strength of image enhance. */
    ot_vpss_grp_nrx_param   nrx_param;            /* RW; Parameters of NR. */
} ot_vpss_grp_param;

typedef struct {
    td_bool                 enable;               /* RW; Range: [0, 1]; Sharpen enable. */
    td_u32                  strength;             /* RW; Range: [0, 100]; Sharpen strength. */
} ot_vpss_chn_sharpen_param;

typedef struct {
    td_u32                  split_node_num;        /* RW; Range: [1, 16]; Max num of split node. */
    td_bool                 coverex_ratio_support; /* RW; Range: [0, 1]; Coverex support ratio coordinate. */
    td_bool                 nr_low_delay_en;       /* RW; Range: [0, 1]; Low delay enable of NR */
} ot_vpss_mod_param;

typedef struct {
    ot_size                 img_size;             /* R; Image size info. */
    td_u64 ATTRIBUTE        *luma_data;           /* R; Luma data. */
} ot_vpss_luma_result;

typedef enum {
    OT_VPSS_COLOR_RANGE_LIMITED = 0,
    OT_VPSS_COLOR_RANGE_FULL,
    OT_VPSS_COLOR_RANGE_BUTT
} ot_vpss_color_range;

typedef struct {
    td_u8 weight;   /* RW; Range: [0, 255]; Weight of Gaussian distribution. */
    td_u8 sigma;    /* RW; Range: [1, 255]; Sigma of Gaussian distribution. */
    td_u8 mean;     /* RW; Range: [0, 255]; Mean of Gaussian distribution. */
} ot_vpss_ldci_gauss_coef_attr;

typedef struct {
    /* RW; Weight curve, the degree of brightening can be controlled based on the luminance of original images. */
    ot_vpss_ldci_gauss_coef_attr positive_attr;
    /* RW; Weight curve, the degree of darkening can be controlled based on the luminance of original images. */
    ot_vpss_ldci_gauss_coef_attr negative_attr;
} ot_vpss_ldci_he_weight_attr;

typedef struct {
    td_bool enable;                     /* RW; Range: [0, 1]; LDCI enable. */
    ot_vpss_color_range color_range;    /* RW; Color range of input yuv image. */
    td_u16 black_ctrl_strength;         /* RW; Range: [0, 511]; Restrain dark region. */
    /*
     * RW; Used for controlling the weight curve of blending
     * between histogram equalization results and original images.
     * The curve is Gaussian-distributed with the luminance as the horizontal coordinate.
     */
    ot_vpss_ldci_he_weight_attr he_weight_attr;
} ot_vpss_ldci_attr;

#endif /* OT_COMMON_VPSS_H */
