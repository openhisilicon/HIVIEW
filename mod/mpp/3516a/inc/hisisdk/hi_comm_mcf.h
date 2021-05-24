/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2019-2023. All rights reserved.
 * Description: hi_comm_mcf.h
 * Author: Hisilicon multimedia software group
 * Create: 2019-10-27
 */

#ifndef __HI_COMM_MCF_H__
#define __HI_COMM_MCF_H__

#include "hi_type.h"
#include "hi_common.h"
#include "hi_comm_video.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* end of #ifdef __cplusplus */

#define EN_ERR_NOT_INITED         20
#define EN_ERR_NOT_START          21
#define EN_ERR_NOT_CREATE         22
#define EN_ERR_NOT_ENCHN          23

#define EN_ERR_CALIB_FEATURE_OVERFLOW   30
#define EN_ERR_CALIB_FEATURE_LACK       31
#define EN_ERR_CALIB_RANSAC_FAIL        32
#define EN_ERR_CALIB_ILLEGAL_MATRIX     33
#define EN_ERR_CALIB_ILLEGAL_MOTION     34
#define EN_ERR_CALIB_FAIL               35

#define HI_ERR_MCF_NULL_PTR         HI_DEF_ERR(HI_ID_MCF, EN_ERR_LEVEL_ERROR, EN_ERR_NULL_PTR)
#define HI_ERR_MCF_NOTREADY         HI_DEF_ERR(HI_ID_MCF, EN_ERR_LEVEL_ERROR, EN_ERR_SYS_NOTREADY)
#define HI_ERR_MCF_INVALID_DEVID    HI_DEF_ERR(HI_ID_MCF, EN_ERR_LEVEL_ERROR, EN_ERR_INVALID_DEVID)
#define HI_ERR_MCF_INVALID_PIPEID   HI_DEF_ERR(HI_ID_MCF, EN_ERR_LEVEL_ERROR, EN_ERR_INVALID_PIPEID)
#define HI_ERR_MCF_INVALID_CHNID    HI_DEF_ERR(HI_ID_MCF, EN_ERR_LEVEL_ERROR, EN_ERR_INVALID_CHNID)
#define HI_ERR_MCF_EXIST            HI_DEF_ERR(HI_ID_MCF, EN_ERR_LEVEL_ERROR, EN_ERR_EXIST)
#define HI_ERR_MCF_UNEXIST          HI_DEF_ERR(HI_ID_MCF, EN_ERR_LEVEL_ERROR, EN_ERR_UNEXIST)
#define HI_ERR_MCF_NOT_SUPPORT      HI_DEF_ERR(HI_ID_MCF, EN_ERR_LEVEL_ERROR, EN_ERR_NOT_SUPPORT)
#define HI_ERR_MCF_NOT_PERM         HI_DEF_ERR(HI_ID_MCF, EN_ERR_LEVEL_ERROR, EN_ERR_NOT_PERM)
#define HI_ERR_MCF_NOMEM            HI_DEF_ERR(HI_ID_MCF, EN_ERR_LEVEL_ERROR, EN_ERR_NOMEM)
#define HI_ERR_MCF_NOBUF            HI_DEF_ERR(HI_ID_MCF, EN_ERR_LEVEL_ERROR, EN_ERR_NOBUF)
#define HI_ERR_MCF_ILLEGAL_PARAM    HI_DEF_ERR(HI_ID_MCF, EN_ERR_LEVEL_ERROR, EN_ERR_ILLEGAL_PARAM)
#define HI_ERR_MCF_BUSY             HI_DEF_ERR(HI_ID_MCF, EN_ERR_LEVEL_ERROR, EN_ERR_BUSY)
#define HI_ERR_MCF_BUF_EMPTY        HI_DEF_ERR(HI_ID_MCF, EN_ERR_LEVEL_ERROR, EN_ERR_BUF_EMPTY)
#define HI_ERR_MCF_NOT_INITED       HI_DEF_ERR(HI_ID_MCF, EN_ERR_LEVEL_ERROR, EN_ERR_NOT_INITED)
#define HI_ERR_MCF_NOT_START        HI_DEF_ERR(HI_ID_MCF, EN_ERR_LEVEL_ERROR, EN_ERR_NOT_START)
#define HI_ERR_MCF_NOT_CREATE       HI_DEF_ERR(HI_ID_MCF, EN_ERR_LEVEL_ERROR, EN_ERR_NOT_CREATE)
#define HI_ERR_MCF_NOT_ENCHN        HI_DEF_ERR(HI_ID_MCF, EN_ERR_LEVEL_ERROR, EN_ERR_NOT_ENCHN)

#define HI_ERR_MCF_CALIB_FEATURE_OVERFLOW HI_DEF_ERR(HI_ID_MCF, EN_ERR_LEVEL_ERROR, EN_ERR_CALIB_FEATURE_OVERFLOW)
#define HI_ERR_MCF_CALIB_FEATURE_LACK     HI_DEF_ERR(HI_ID_MCF, EN_ERR_LEVEL_ERROR, EN_ERR_CALIB_FEATURE_LACK)
#define HI_ERR_MCF_CALIB_RANSAC_FAIL      HI_DEF_ERR(HI_ID_MCF, EN_ERR_LEVEL_ERROR, EN_ERR_CALIB_RANSAC_FAIL)
#define HI_ERR_MCF_CALIB_ILLEGAL_MATRIX   HI_DEF_ERR(HI_ID_MCF, EN_ERR_LEVEL_ERROR, EN_ERR_CALIB_ILLEGAL_MATRIX)
#define HI_ERR_MCF_CALIB_ILLEGAL_MOTION   HI_DEF_ERR(HI_ID_MCF, EN_ERR_LEVEL_ERROR, EN_ERR_CALIB_ILLEGAL_MOTION)
#define HI_ERR_MCF_CALIB_FAIL             HI_DEF_ERR(HI_ID_MCF, EN_ERR_LEVEL_ERROR, EN_ERR_CALIB_FAIL)

#define MCF_COEF_NUM              9
#define MCF_MAX_GRP_WIDTH         2592
#define MCF_MAX_GRP_HEIGHT        1536
#define MCF_MIN_GRP_WIDTH         256
#define MCF_MIN_GRP_HEIGHT        256
#define MCF_LUMA_NUM              256

typedef struct {
    hi_u16 left;   /* Circumscribed rectangle left border */
    hi_u16 right;  /* Circumscribed rectangle right border */
    hi_u16 top;    /* Circumscribed rectangle top border */
    hi_u16 bottom; /* Circumscribed rectangle bottom border */
} hi_region_s;

typedef struct {
    hi_s64 correct_coef[MCF_COEF_NUM];  /* matrix for match the two sensor */
    RECT_S region;                      /* ROI region */
} hi_mcf_cal;

typedef struct {
    hi_s32 refer_feature_num;    /* SURF feature number of reference image */
    hi_s32 register_feature_num; /* SURF feature number of image to be registered */
    hi_s32 match_feature_num;    /* match feature number between two image after RANSAC process */
} hi_mcf_feature_info;

typedef enum {
    MCF_LENS_MODE_ONE = 0,      /* single lens with two sensor. */
    MCF_LENS_MODE_TWO,          /* two lens with two sensor. */
    MCF_LENS_MODE_BUTT
} hi_mcf_lens_mode;

typedef enum {
    MCF_FUSION_MODE_ADV = 0,    /* MCF advance fusion. */
    MCF_FUSION_MODE_MAIN,       /* MCF local fusion. */
    MCF_FUSION_MODE_BASE,       /* MCF global fusion based on weight. */

    MCF_FUSION_MODE_BUTT
} hi_mcf_fusion_mode;

typedef enum {
    MCF_REFER_MONO = 0,      /* MCF output image refer mono. */
    MCF_REFER_COLOR,         /* MCF output image refer color. */
    MCF_REFER_BUTT
} hi_mcf_refer_mode;

typedef struct {
    hi_mcf_fusion_mode fusion_mode; /* MCF-Base \ MCF-Middle \ MCF-Advance */
    hi_mcf_lens_mode lens_mode;     /* MCF_LENS_MODE_ONE: one lens match two sensor, two lens match two sensor */
    hi_mcf_refer_mode refer_mode;   /* MCF output image refer which sensor */
    hi_u32     width;               /* range[MCF_MIN_GRP_WIDTH,MCF_MAX_GRP_WIDTH] */
    hi_u32     height;              /* range[MCF_MIN_GRP_HEIGHT,MCF_MAX_GRP_HEIGHT] */
    MCF_PIPE   mono_pipe;           /* mono  pip ID. range: 0-1 */
    MCF_PIPE   color_pipe;          /* color pip ID. range: 0-1, c_pip_id should not be the same with m_pip_id */
    hi_bool    sync_pipe;           /* whether sync pipe image. */
    hi_mcf_cal cal_info;            /* paramter matrix and reset region. valid in MCF_MODE_TWO_LENS mode */
} hi_mcf_grp_attr;

typedef struct {
    hi_u8 mono_alpha;       /* range: [0, 255]; output alpha from mono alpha ratio */
} hi_mcf_param_base;

typedef struct {
    hi_u8 alpha[MCF_LUMA_NUM]; /* default, range: [0, 255]. */
} hi_mcf_param_main;

typedef struct {
    hi_u8 sharpen;              /* range: [1, 32]; adv sharpen */
    hi_u8 enhance_strength;     /* range: [0, 255]; adv enhance strength */
    hi_u8 alpha[MCF_LUMA_NUM];
} hi_mcf_param_adv;

typedef struct {
    union {
        hi_mcf_param_adv   param_adv;    /* MCF advance fusion mode, debug alg param */
        hi_mcf_param_main  param_main;   /* MCF main fusion mode, debug alg param */
        hi_mcf_param_base  param_base;   /* MCF base fusion mode, debug alg param */
    };
} hi_mcf_alg_param;

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* end of #ifdef __cplusplus */

#endif /* end of #ifndef __HI_COMM_MCF_H__ */