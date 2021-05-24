/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: Function of hi_comm_pq_ai.h
 * Author: ISP SW
 * Create: 2012/06/28
 */

#ifndef __HI_COMM_PQ_AI_ADAPT_H__
#define __HI_COMM_PQ_AI_ADAPT_H__

#include "hi_type.h"
#include "hi_errno.h"
#include "hi_common_adapt.h"
#include "hi_comm_isp_adapt.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */

#define HI_ERR_PQ_AI_NULL_PTR         HI_DEF_ERR(HI_ID_ISP, EN_ERR_LEVEL_ERROR, EN_ERR_NULL_PTR)
#define HI_ERR_PQ_AI_ILLEGAL_PARAM    HI_DEF_ERR(HI_ID_ISP, EN_ERR_LEVEL_ERROR, EN_ERR_ILLEGAL_PARAM)
#define HI_ERR_PQ_AI_NOT_SUPPORT      HI_DEF_ERR(HI_ID_ISP, EN_ERR_LEVEL_ERROR, EN_ERR_NOT_SUPPORT)

typedef struct {
    hi_bool      pq_ai_en;    /* RW; Range: [0, 1] */
    hi_vi_pipe   base_pipe_id;
} hi_pq_ai_group_attr;

/* AI color */
typedef struct {
    hi_u8 saturation_coef; /* RW; Range: [0x10, 0xFF]; saturation coef used for AI Color dimension, 64 means 1x,
                              16 means 0.25x, 0xFF means 3.984x */
    hi_u8 hue_coef;        /* RW; Range: [0x10, 0xFF]; hue coef used for AI Color dimension, 64 means 1x,
                              16 means 0.25x, 0xFF means 3.984x */
} hi_pq_ai_color_manual_attr;

typedef struct {
    hi_u8 saturation_coef[ISP_AUTO_ISO_STRENGTH_NUM]; /* RW; Range: [0x10, 0xFF];
                                                         saturation coef used for AI Color dimension, 64 means 1x,
                                                         16 means 0.25x, 0xFF means 3.984x */
    hi_u8 hue_coef[ISP_AUTO_ISO_STRENGTH_NUM]; /* RW; Range: [0x10, 0xFF]; hue coef used for AI Color dimension,
                                                  64 means 1x, 16 means 0.25x, 0xFF means 3.984x */
} hi_pq_ai_color_auto_attr;

typedef struct {
    hi_isp_op_type op_type;
    hi_pq_ai_color_manual_attr manual_attr;
    hi_pq_ai_color_auto_attr   auto_attr;
} hi_pq_ai_color_attr;

/* AI sharpeness */
typedef struct {
    hi_u8  texture_pre_mf_level;   /* RW; Range:[0x0, 0xF];Format:4.0;
                                      texture_pre_mf_level used for AI Sharpness dimension */
    hi_u8  texture_mf_coef;        /* RW; Range:[0x0, 0xFF]; texture_mf coef used for AI Sharpness dimension,
                                      64 means 1x, 0 means 0x, 0xFF means 3.984x */
    hi_u8  edge_mf_coef;           /* RW; Range:[0x0, 0xFF]; edge_mf coef used for AI Sharpness dimension, 64 means 1x,
                                      0 means 0x, 0xFF means 3.984x */
    hi_u8  shoot_level;            /* RW; Range:[0x0, 0xF];Format:4.0; Shoot_level used for AI Sharpness dimension */
}  hi_pq_ai_sharpness_manual_attr;

typedef struct {
    hi_u8 texture_pre_mf_level[ISP_AUTO_ISO_STRENGTH_NUM]; /* RW; Range:[0x0, 0xF];Format:4.0;
                                                              texture_pre_mf_level used for AI Sharpness dimension */
    hi_u8 texture_mf_coef[ISP_AUTO_ISO_STRENGTH_NUM]; /* RW; Range:[0x0, 0xFF];texture_mf coef used for AI Sharpness
                                                         dimension, 64 means 1x, 0 means 0x, 0xFF means 3.984x */
    hi_u8 edge_mf_coef[ISP_AUTO_ISO_STRENGTH_NUM]; /* RW; Range:[0x0, 0xFF]; edge_mf coef used for AI Sharpness
                                                      dimension, 64 means 1x, 0 means 0x, 0xFF means 3.984x */
    hi_u8 shoot_level[ISP_AUTO_ISO_STRENGTH_NUM]; /* RW; Range:[0x0, 0xF];Format:4.0;
                                                     Shoot_level used for AI Sharpness dimension */
} hi_pq_ai_sharpness_auto_attr;

typedef struct {
    hi_isp_op_type op_type;
    hi_pq_ai_sharpness_manual_attr manual_attr;
    hi_pq_ai_sharpness_auto_attr   auto_attr;
} hi_pq_ai_sharpness_attr;

/* AI Pre noiseness */
typedef struct {
    hi_u8 presfy_level;         /* RW;Range:[0x0, 0xF];Format:4.0; presfy_level used for AI Noiseness dimension */
} hi_pq_ai_noiseness_pre_manual_attr;

typedef struct {
    hi_u8 presfy_level[ISP_AUTO_ISO_STRENGTH_NUM]; /* RW;Range:[0x0, 0xF];Format:4.0;
                                                      presfy_level used for AI Noiseness dimension */
} hi_pq_ai_noiseness_pre_auto_attr;

typedef struct {
    hi_isp_op_type   op_type;
    hi_pq_ai_noiseness_pre_manual_attr manual_attr;
    hi_pq_ai_noiseness_pre_auto_attr   auto_attr;
} hi_pq_ai_noiseness_pre_attr;

/* AI Post noiseness */
typedef struct {
    hi_u8 sfy_coef;         /* RW, Range: [0x0, 0xFF]. sfy coef used for AI Noiseness dimension, 64 means 1x,
                               0 means 0x, 0xFF means 3.984x */
    hi_u8 tfy_coef;         /* RW, Range: [0x0, 0xFF]. tfy coef used for AI Noiseness dimension, 64 means 1x,
                               0 means 0x, 0xFF means 3.984x */
    hi_u8 mdthr_coef;       /* RW, Range: [0x0, 0xFF]. mdthr coef used for AI Noiseness dimension, 64 means 1x,
                               0 means 0x, 0xFF means 3.984x */
    hi_u8 colornoise_coef;  /* RW, Range: [0x0, 0xFF]. colornoise coef used for AI Noiseness dimension, 64 means 1x,
                               0 means 0x, 0xFF means 3.984x */
} hi_pq_ai_noiseness_post_manual_attr;

typedef struct {
    hi_u8 sfy_coef[ISP_AUTO_ISO_STRENGTH_NUM];
    hi_u8 tfy_coef [ISP_AUTO_ISO_STRENGTH_NUM];
    hi_u8 mdthr_coef[ISP_AUTO_ISO_STRENGTH_NUM];
    hi_u8 colornoise_coef[ISP_AUTO_ISO_STRENGTH_NUM];
} hi_pq_ai_noiseness_post_auto_attr;

typedef struct {
    hi_isp_op_type   op_type;
    hi_pq_ai_noiseness_post_manual_attr  manual_attr;
    hi_pq_ai_noiseness_post_auto_attr    auto_attr;
} hi_pq_ai_noiseness_post_attr;

/* AI brightness */
typedef struct {
    hi_u8 pre_enhance_level;  /* RW; Range:[0x0, 0xF];Format:4.0; pre_enhance_level used for AI Brightness dimension */
    hi_u8 post_enhance_level; /* RW; Range:[0x0, 0xF];Format:4.0; post_enhance_level used for AI Brightness dimension */
} hi_pq_ai_brightness_manual_attr;

typedef struct {
    hi_u8 pre_enhance_level[ISP_AUTO_ISO_STRENGTH_NUM]; /* RW; Range:[0x0, 0xF];Format:4.0;
                                                           pre_enhance_level used for AI Brightness dimension */
    hi_u8 post_enhance_level[ISP_AUTO_ISO_STRENGTH_NUM]; /* RW; Range:[0x0, 0xF];Format:4.0;
                                                            post_enhance_level used for AI Brightness dimension */
} hi_pq_ai_brightness_auto_attr;

typedef struct {
    hi_isp_op_type op_type;
    hi_pq_ai_brightness_manual_attr manual_attr;
    hi_pq_ai_brightness_auto_attr   auto_attr;
} hi_pq_ai_brightness_attr;

/* AI contrast */
typedef struct {
    hi_u8 contrast_coef;     /* RW; Range:[0x0, 0xFF]; contrast coef used for AI Contrast dimension, 64 means 1x,
                                0 means 0x, 0xFF means 3.984x */
} hi_pq_ai_contrast_manual_attr ;

typedef struct {
    hi_u8 contrast_coef[ISP_AUTO_ISO_STRENGTH_NUM]; /* RW; Range:[0x0, 0xFF]; contrast coef used for AI Contrast
                                                       dimension, 64 means 1x, 0 means 0x, 0xFF means 3.984x */
} hi_pq_ai_contrast_auto_attr;

typedef struct {
    hi_isp_op_type op_type;
    hi_pq_ai_contrast_manual_attr manual_attr;
    hi_pq_ai_contrast_auto_attr   auto_attr;
} hi_pq_ai_contrast_attr;

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif /* __HI_COMM_PQ_AI_ADAPT_H__ */
