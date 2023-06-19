/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */

#ifndef OT_COMMON_MCF_CALIBRATION_H
#define OT_COMMON_MCF_CALIBRATION_H

#include "ot_type.h"
#include "ot_common_video.h"
#include "ot_errno.h"

#define OT_ERR_CALIB_FEATURE_OVERFLOW 30
#define OT_ERR_CALIB_FEATURE_LACK 31
#define OT_ERR_CALIB_RANSAC_FAIL 32
#define OT_ERR_CALIB_ILLEGAL_MATRIX 33
#define OT_ERR_CALIB_ILLEGAL_MOTION 34
#define OT_ERR_CALIB_FAIL 35

#define OT_ERR_MCF_CALIBRATION_NULL_PTR   OT_DEFINE_ERR(OT_ID_MCF_CALIBRATION, OT_ERR_LEVEL_ERROR, OT_ERR_NULL_PTR)

#define OT_ERR_MCF_CALIB_FEATURE_OVERFLOW OT_DEFINE_ERR(OT_ID_MCF_CALIBRATION, OT_ERR_LEVEL_ERROR, \
                                                        OT_ERR_CALIB_FEATURE_OVERFLOW)
#define OT_ERR_MCF_CALIB_FEATURE_LACK     OT_DEFINE_ERR(OT_ID_MCF_CALIBRATION, OT_ERR_LEVEL_ERROR, \
                                                        OT_ERR_CALIB_FEATURE_LACK)
#define OT_ERR_MCF_CALIB_RANSAC_FAIL      OT_DEFINE_ERR(OT_ID_MCF_CALIBRATION, OT_ERR_LEVEL_ERROR, \
                                                        OT_ERR_CALIB_RANSAC_FAIL)
#define OT_ERR_MCF_CALIB_ILLEGAL_MATRIX   OT_DEFINE_ERR(OT_ID_MCF_CALIBRATION, OT_ERR_LEVEL_ERROR, \
                                                        OT_ERR_CALIB_ILLEGAL_MATRIX)
#define OT_ERR_MCF_CALIB_ILLEGAL_MOTION   OT_DEFINE_ERR(OT_ID_MCF_CALIBRATION, OT_ERR_LEVEL_ERROR, \
                                                        OT_ERR_CALIB_ILLEGAL_MOTION)
#define OT_ERR_MCF_CALIB_FAIL             OT_DEFINE_ERR(OT_ID_MCF_CALIBRATION, OT_ERR_LEVEL_ERROR, OT_ERR_CALIB_FAIL)
#define OT_ERR_MCF_CALIBRATION_ILLEGAL_PARAM OT_DEFINE_ERR(OT_ID_MCF_CALIBRATION, OT_ERR_LEVEL_ERROR, \
                                                           OT_ERR_ILLEGAL_PARAM)

#define OT_ERR_MCF_CALIBRATION_NOMEM      OT_DEFINE_ERR(OT_ID_MCF_CALIBRATION, OT_ERR_LEVEL_ERROR, OT_ERR_NO_MEM)

#define OT_MCF_COEF_NUM              9

typedef enum {
    OT_MCF_CALIBRATION_AFFINE = 0,      /* affine Transform mode */
    OT_MCF_CALIBRATION_PROJECTIVE,      /* projective mode */
    OT_MCF_CALIBRATION_MODE_BUTT
} ot_mcf_calibration_mode;

typedef struct {
    td_s64 correct_coef[OT_MCF_COEF_NUM]; /* matrix for match the two */
    ot_rect region;                       /* ROI region */
} ot_mcf_calibration;

typedef struct {
    td_s32 refer_feature_num;    /* SURF feature number of reference image */
    td_s32 register_feature_num; /* SURF feature number of image to be registered */
    td_s32 match_feature_num;    /* match feature number between two image after RANSAC process */
} ot_mcf_feature_info;

#endif
