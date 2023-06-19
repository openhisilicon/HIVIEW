/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */

#ifndef OT_COMMON_PHOTO_H
#define OT_COMMON_PHOTO_H

#include "ot_math.h"
#include "ot_common.h"
#include "ot_errno.h"
#include "ot_common_video.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

#define OT_PHOTO_HDR_FRAME_NUM  3
#define OT_PHOTO_MFNR_FRAME_NUM 4

#define OT_PHOTO_MIN_WIDTH  1280
#define OT_PHOTO_MIN_HEIGHT 720
#define OT_PHOTO_MAX_WIDTH  8192
#define OT_PHOTO_MAX_HEIGHT 6144

#define OT_PHOTO_HDR_MAX_HEIGHT 8192

#define OT_PHOTO_HDR_ISO_LEVEL_CNT  10
#define OT_PHOTO_SFNR_ISO_LEVEL_CNT 8
#define OT_PHOTO_MFNR_ISO_LEVEL_CNT 8
#define OT_PHOTO_DE_ISO_LEVEL_CNT   8

#define OT_PHOTO_MAX_FACE_NUM 10

typedef enum {
    OT_PHOTO_ALG_TYPE_HDR = 0x0,
    OT_PHOTO_ALG_TYPE_SFNR = 0x1,
    OT_PHOTO_ALG_TYPE_MFNR = 0x2,
    OT_PHOTO_ALG_TYPE_DE = 0x3,

    OT_PHOTO_ALG_TYPE_BUTT
} ot_photo_alg_type;

typedef struct {
    td_u32 id;                /* RW: face info id, reserved. */
    ot_rect face_rect;        /* RW: face rect. */
    ot_rect left_eye_rect;    /* RW: left_eye rect, reserved. */
    ot_rect right_eye_rect;   /* RW: right_eye rect, reserved. */
    td_u32 blink_score;       /* RW: blink score, reserved. */
    td_u32 smile_score;       /* RW: smile score, reserved. */
} ot_photo_face_info;

typedef struct {
    td_s32 iso_speed;          /* RW: iso value, Range: [160, 6400]. */
    td_s32 motion_low_gray;    /* RW: Range: [0, 255]. */
    td_s32 motion_high_gray;   /* RW: Range: [0, 255]. */
    td_float motion_ratio;     /* RW: Range: [0, 100]. */
    td_s32 night_average_luma; /* RW: Range: [0, 255]. */
} ot_photo_dark_motion_detection_param;

typedef enum {
    OT_PHOTO_HDR_FUSION_AUTO = 0,
    OT_PHOTO_HDR_FUSION_SHORT_MEDIAN = 1,
    OT_PHOTO_HDR_FUSION_MEDIAN_LONG = 2,
    OT_PHOTO_HDR_FUSION_ALL = 3,
    OT_PHOTO_HDR_FUSION_BUTT
} ot_photo_hdr_fusion_mode;

typedef struct {
    td_s32 iso_speed;               /* RW: iso value, Range: [160, 6400]. */
    td_s32 pyramid_top_size;        /* RW: Range: [16, 128]. */
    td_s32 weight_curve_method;     /* RW: Range: [0, 1]. */
    td_s32 weight_calc_method;      /* RW: Range: [0, 1]. */
    td_s32 blend_uv_gain;           /* RW: Range: [128, 255]. */
    td_s32 detail_enhance_ratio_l0; /* RW: Range: [0, 1024]. */
    td_s32 detail_enhance_ratio_l1; /* RW: Range: [0, 1024]. */
    td_float blend_sigma;           /* RW: Range: [0.00, 4.00]. */
    ot_photo_hdr_fusion_mode fusion_mode; /* RW;Range:[0, 3]; */
} ot_photo_image_fusion_param;

typedef struct {
    td_s32 ajust_ratio;           /* RW: Range: [0, 255]. */
    td_s32 image_scale_method;    /* RW: 0: Bilinear, 1: Lanczos. */
    ot_photo_dark_motion_detection_param motion_detection_param[OT_PHOTO_HDR_ISO_LEVEL_CNT];
    ot_photo_image_fusion_param hdr_image_fusion_param[OT_PHOTO_HDR_ISO_LEVEL_CNT];
} ot_photo_hdr_coef;

typedef struct {
    td_s32 iso_value;  /* RW: iso value, Range: [100, 3200]. */
    td_s32 luma;       /* RW: luma value, Range: [-100, 0]. */
    td_s32 chroma;     /* RW: chroma value, Range: [-100, 0]. */
    td_s32 luma_hf;    /* RW: high frequency luma value, Range: [-100, 0]. */
    td_s32 chroma_hf;  /* RW: high frequency chroma value, Range: [-100, 0]. */
} ot_photo_sfnr_iso_strategy;

typedef struct {
    ot_photo_sfnr_iso_strategy ast_iso_strat[OT_PHOTO_SFNR_ISO_LEVEL_CNT];
} ot_photo_sfnr_coef;

typedef struct {
    td_s32 iso_value;       /* RW: iso value, Range: [100, 12800]. */
    td_s32 stnr;            /* RW: intensity ratio in time domain to space domain, Range: [0, 255]. */
    td_s32 tnr_frm_num;     /* RW: tnr frame num, Range: [1, 4]. */
    td_s32 stnr_dark_less;  /* RW: use the intensity of the airspace effect in the dark, Range: [0, 255]. */
    td_s32 stnr_ghost_less; /* RW: haunted area blur intensity, Range: [0, 255]. */
    td_s32 luma_alpha;      /* RW: luma catio threshold, Range: [0, 255]. */
    td_s32 luma_delta;      /* RW: luma difference threshold, Range: [0, 255]. */
    td_s32 chroma_alpha;    /* RW: chrominance catio threshold, Range: [0, 255]. */
    td_s32 chroma_delta;    /* RW: chrominance difference threshold, Range: [0, 255]. */
} ot_photo_mfnr_3dnr_iso_strategy;

typedef struct {
    ot_photo_mfnr_3dnr_iso_strategy iso_3dnr_param[OT_PHOTO_MFNR_ISO_LEVEL_CNT];
} ot_photo_mfnr_3dnr_param;

typedef struct {
    td_s32 iso_value;  /* RW: iso value, Range: [100, 12800]. */
    td_s32 luma;       /* RW: luma value, Range: [-100, 0]. */
    td_s32 chroma;     /* RW: chroma value, Range: [-100, 0]. */
    td_s32 luma_hf2;   /* RW: high frequency luma value, Range: [-100, 0]. */
    td_s32 detail_min; /* RW: high frequency luma detail, Range: [0, 8]. */
} ot_photo_mfnr_2dnr_iso_strategy;

typedef struct {
    ot_photo_mfnr_2dnr_iso_strategy iso_2dnr_param[OT_PHOTO_MFNR_ISO_LEVEL_CNT];
} ot_photo_mfnr_2dnr_param;

typedef struct {
    td_s32 iso_value;     /* RW: iso value, Range: [100, 6400]. */
    td_s32 global_gain;   /* RW: global detail intensity coefficient, Range: [0, 255]. */
    td_s32 gain_lf;       /* RW: low frequency proportion, Range: [0, 255]. */
    td_s32 gain_hf_pos;   /* RW: high frequency white point ratio, Range: [0, 255]. */
    td_s32 gain_hf_neg;   /* RW: high frequency black dot ratio, Range: [0, 255]. */
    td_s32 luma_scale_x0; /* RW: luminance stretch low threshold, Range: [0, 255]. */
    td_s32 luma_scale_x1; /* RW: luminance stretch high threshold, Range: [0, 255]. */
    td_s32 luma_scale_y1; /* RW: attenuation coefficient, Range: [0, 255]. */
    td_s32 satu_gain_x0;  /* RW: saturation low threshold, Range: [0, 255]. */
    td_s32 satu_gain_x1;  /* RW: saturation high threshold, Range: [0, 255]. */
    td_s32 satu_gain_y1;  /* RW: attenuation coefficient, Range: [0, 255]. */
} ot_photo_de_iso_strategy;

typedef struct {
    ot_photo_de_iso_strategy de_iso_strat[OT_PHOTO_DE_ISO_LEVEL_CNT];
} ot_photo_de_coef;

typedef struct {
    td_bool image_scale;  /* RW: TD_TRUE: scale, TD_FALSE: not scale. */
    ot_photo_mfnr_3dnr_param mfnr_3dnr;
    ot_photo_mfnr_2dnr_param mfnr_2dnr;

    td_bool de_enable;    /* RW: TD_TRUE: mfnr + de, TD_FALSE: mfnr. */
    ot_photo_de_coef mfnr_de_coef;
} ot_photo_mfnr_coef;

typedef struct {
    union {
        ot_photo_hdr_coef hdr_coef;   /* AUTO: ot_photo_alg_type:OT_PHOTO_ALG_TYPE_HDR. */
        ot_photo_sfnr_coef sfnr_coef; /* AUTO: ot_photo_alg_type:OT_PHOTO_ALG_TYPE_SFNR. */
        ot_photo_mfnr_coef mfnr_coef; /* AUTO: ot_photo_alg_type:OT_PHOTO_ALG_TYPE_MFNR. */
        ot_photo_de_coef de_coef;     /* AUTO: ot_photo_alg_type:OT_PHOTO_ALG_TYPE_DE. */
    };
} ot_photo_alg_coef;

typedef struct {
    td_u64 public_mem_phy_addr; /* RW; mem phys addr. */
    td_u64 public_mem_vir_addr; /* RW; mem virt addr, reserved. */
    td_u32 public_mem_size;     /* RW; mem size. */
    td_bool print_debug_info;   /* RW; debug info. */
} ot_photo_alg_init;

typedef struct {
    ot_video_frame_info src_frm;                         /* RW; hdr input yuv data. */
    ot_video_frame_info des_frm;                         /* RW; hdr output yuv data. */
    td_u32 frm_index;                                    /* RW; hdr yuv index, Range: [0, 2]. */
    td_u32 iso;                                          /* RW; hdr input yuv iso. */
    td_u32 face_num;                                     /* RW; Range: [0, 10]. */
    ot_photo_face_info face_info[OT_PHOTO_MAX_FACE_NUM]; /* RW; face info. */
} ot_photo_hdr_attr;

typedef struct {
    ot_video_frame_info frm;  /* RW; sfnr input yuv data. */
    td_u32 iso;               /* RW; sfnr input yuv iso. */
} ot_photo_sfnr_attr;

typedef struct {
    ot_video_frame_info src_frm; /* RW; mfnr input yuv data. */
    ot_video_frame_info des_frm; /* RW; mfnr output yuv data. */
    ot_video_frame_info raw_frm; /* RW; de input raw data. */
    td_u32 frm_index;            /* RW; mfnr yuv index, Range: [0, 3]. */
    td_u32 iso;                  /* RW; mfnr input yuv iso. */
} ot_photo_mfnr_attr;

typedef struct {
    ot_video_frame_info frm;     /* RW; de input yuv data. */
    ot_video_frame_info raw_frm; /* RW; de input raw data. */
    td_u32 iso;                  /* RW; de input yuv iso. */
} ot_photo_de_attr;

typedef struct {
    union {
        ot_photo_hdr_attr hdr_attr;   /* AUTO: ot_photo_alg_type:OT_PHOTO_ALG_TYPE_HDR. */
        ot_photo_sfnr_attr sfnr_attr; /* AUTO: ot_photo_alg_type:OT_PHOTO_ALG_TYPE_SFNR. */
        ot_photo_mfnr_attr mfnr_attr; /* AUTO: ot_photo_alg_type:OT_PHOTO_ALG_TYPE_MFNR. */
        ot_photo_de_attr de_attr;     /* AUTO: ot_photo_alg_type:OT_PHOTO_ALG_TYPE_DE. */
    };
} ot_photo_alg_attr;

#define OT_PHOTO_ALIGN       128
#define OT_PHOTO_INTERFACE_MEM   1024
#define OT_PHOTO_HDR_MEM_RATIO   13.4
#define OT_PHOTO_HDR_MEM_OFFSET  7000000
#define OT_PHOTO_MFNR_MEM_RATIO  12.6
#define OT_PHOTO_MFNR_MEM_OFFSET 6000000
#define OT_PHOTO_SFNR_MEM_RATIO  4.2
#define OT_PHOTO_SFNR_MEM_OFFSET 650000
#define OT_PHOTO_DE_MEM_RATIO    1.0
#define OT_PHOTO_DE_MEM_OFFSET   1024

__inline static td_u32 hdr_get_public_mem_size(td_u32 width, td_u32 height)
{
    td_u32 stride = OT_ALIGN_UP(width, OT_PHOTO_ALIGN);
    td_u32 pub_mem_size = stride * height * OT_PHOTO_HDR_MEM_RATIO + OT_PHOTO_HDR_MEM_OFFSET + OT_PHOTO_INTERFACE_MEM;

    return pub_mem_size;
}

__inline static td_u32 mfnr_get_public_mem_size(td_u32 width, td_u32 height)
{
    td_u32 stride = OT_ALIGN_UP(width, OT_PHOTO_ALIGN);
    td_u32 pub_mem_size = stride * height * OT_PHOTO_MFNR_MEM_RATIO + OT_PHOTO_MFNR_MEM_OFFSET + OT_PHOTO_INTERFACE_MEM;

    return pub_mem_size;
}

__inline static td_u32 sfnr_get_public_mem_size(td_u32 width, td_u32 height)
{
    td_u32 stride = OT_ALIGN_UP(width, OT_PHOTO_ALIGN);
    td_u32 pub_mem_size = stride * height * OT_PHOTO_SFNR_MEM_RATIO + OT_PHOTO_SFNR_MEM_OFFSET + OT_PHOTO_INTERFACE_MEM;

    return pub_mem_size;
}

__inline static td_u32 de_get_public_mem_size(td_u32 width, td_u32 height)
{
    td_u32 stride = OT_ALIGN_UP(width >> 1, OT_PHOTO_ALIGN);
    td_u32 pub_mem_size = stride * height * OT_PHOTO_DE_MEM_RATIO + OT_PHOTO_DE_MEM_OFFSET + OT_PHOTO_INTERFACE_MEM;

    return pub_mem_size;
}

#define OT_ERR_PHOTO_NULL_PTR      OT_DEFINE_ERR(OT_ID_PHOTO, OT_ERR_LEVEL_ERROR, OT_ERR_NULL_PTR)
#define OT_ERR_PHOTO_NOT_READY     OT_DEFINE_ERR(OT_ID_PHOTO, OT_ERR_LEVEL_ERROR, OT_ERR_NOT_READY)
#define OT_ERR_PHOTO_INVALID_DEVID OT_DEFINE_ERR(OT_ID_PHOTO, OT_ERR_LEVEL_ERROR, OT_ERR_INVALID_DEV_ID)
#define OT_ERR_PHOTO_INVALID_CHNID OT_DEFINE_ERR(OT_ID_PHOTO, OT_ERR_LEVEL_ERROR, OT_ERR_INVALID_CHN_ID)
#define OT_ERR_PHOTO_EXIST         OT_DEFINE_ERR(OT_ID_PHOTO, OT_ERR_LEVEL_ERROR, OT_ERR_EXIST)
#define OT_ERR_PHOTO_UNEXIST       OT_DEFINE_ERR(OT_ID_PHOTO, OT_ERR_LEVEL_ERROR, OT_ERR_UNEXIST)
#define OT_ERR_PHOTO_NOT_CFG       OT_DEFINE_ERR(OT_ID_PHOTO, OT_ERR_LEVEL_ERROR, OT_ERR_NOT_CFG)
#define OT_ERR_PHOTO_NOT_SUPPORT   OT_DEFINE_ERR(OT_ID_PHOTO, OT_ERR_LEVEL_ERROR, OT_ERR_NOT_SUPPORT)
#define OT_ERR_PHOTO_NOT_PERM      OT_DEFINE_ERR(OT_ID_PHOTO, OT_ERR_LEVEL_ERROR, OT_ERR_NOT_PERM)
#define OT_ERR_PHOTO_NO_MEM        OT_DEFINE_ERR(OT_ID_PHOTO, OT_ERR_LEVEL_ERROR, OT_ERR_NO_MEM)
#define OT_ERR_PHOTO_NO_BUF        OT_DEFINE_ERR(OT_ID_PHOTO, OT_ERR_LEVEL_ERROR, OT_ERR_NO_BUF)
#define OT_ERR_PHOTO_ILLEGAL_PARAM OT_DEFINE_ERR(OT_ID_PHOTO, OT_ERR_LEVEL_ERROR, OT_ERR_ILLEGAL_PARAM)
#define OT_ERR_PHOTO_BUSY          OT_DEFINE_ERR(OT_ID_PHOTO, OT_ERR_LEVEL_ERROR, OT_ERR_BUSY)
#define OT_ERR_PHOTO_BUF_EMPTY     OT_DEFINE_ERR(OT_ID_PHOTO, OT_ERR_LEVEL_ERROR, OT_ERR_BUF_EMPTY)

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */
#endif /* OT_COMMON_PHOTO_H */
