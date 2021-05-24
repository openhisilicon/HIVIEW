/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2018-2019. All rights reserved.
 * Description : fisheye_calibrate
 * Author : ISP SW
 * Create : 2018-12-22
 * Version : version1.0
 */
#ifndef __FISHEYE_CALIBRATE_H__
#define __FISHEYE_CALIBRATE_H__

#include "hi_comm_video.h"
#include "hi_errno.h"

#ifdef __cplusplus
#if __cplusplus

extern "C" {
#endif
#endif /* __cplusplus */

#define FISHEYE_EN_ERR_ILLEGAL_PARAM    (EN_ERR_ILLEGAL_PARAM + 30) /* 30 : errno inteval from gdc */
#define FISHEYE_EN_ERR_NULL_PTR         (EN_ERR_NULL_PTR + 30)      /* 30 : errno inteval from gdc */
#define FISHEYE_EN_ERR_NOT_SUPPORT      (EN_ERR_NOT_SUPPORT + 30)   /* 30 : errno inteval from gdc */
#define FISHEYE_EN_ERR_NOMEM            (EN_ERR_NOMEM + 30)         /* 30 : errno inteval from gdc */
#define FISHEYE_EN_ERR_BUSY             (EN_ERR_BUSY + 30)          /* 30 : errno inteval from gdc */

#define HI_ERR_FISHEYE_CALIBRATE_ILLEGAL_PARAM  HI_DEF_ERR(HI_ID_GDC, EN_ERR_LEVEL_ERROR, FISHEYE_EN_ERR_ILLEGAL_PARAM)
#define HI_ERR_FISHEYE_CALIBRATE_NULL_PTR       HI_DEF_ERR(HI_ID_GDC, EN_ERR_LEVEL_ERROR, FISHEYE_EN_ERR_NULL_PTR)
#define HI_ERR_FISHEYE_CALIBRATE_NOT_SUPPORT    HI_DEF_ERR(HI_ID_GDC, EN_ERR_LEVEL_ERROR, FISHEYE_EN_ERR_NOT_SUPPORT)
#define HI_ERR_FISHEYE_CALIBRATE_NOMEM          HI_DEF_ERR(HI_ID_GDC, EN_ERR_LEVEL_ERROR, FISHEYE_EN_ERR_NOMEM)
#define HI_ERR_FISHEYE_CALIBRATE_BUSY           HI_DEF_ERR(HI_ID_GDC, EN_ERR_LEVEL_ERROR, FISHEYE_EN_ERR_BUSY)

typedef enum {
    LEVEL_0 = 0,    /* calibrate level 0 */
    LEVEL_1 = 1,    /* calibrate level 1 */
    LEVEL_2 = 2,    /* calibrate level 2 */

    LEVEL_BUTT
} FISHEYE_CALIBRATE_LEVEL_E;

typedef struct {
    HI_S32 s32OffsetV;      /* the horizontal offset between image center and physical center of len */
    HI_S32 s32OffsetH;      /* the vertical offset between image center and physical center of len */
    HI_S32 s32Radius_X;     /* the X coordinate of physical center of len */
    HI_S32 s32Radius_Y;     /* the Y coordinate of physical center of len */
    HI_U32 u32Radius;       /* the radius of len */
} FISHEYE_CALIBTATE_RESULT_S;

typedef struct {
    FISHEYE_CALIBTATE_RESULT_S stCalibrateResult;   /* the output of result */
} CALIBTATE_OUTPUT_S;

hi_s32 HI_FISHEYE_ComputeCalibrateResult(VIDEO_FRAME_S* pstVFramIn,
    FISHEYE_CALIBRATE_LEVEL_E enLevel, CALIBTATE_OUTPUT_S* pOutCalibrate);

hi_s32 HI_FISHEYE_MarkCalibrateResult(VIDEO_FRAME_S* pstPicIn, VIDEO_FRAME_S* pstPicOut,
    FISHEYE_CALIBTATE_RESULT_S* pCalibrateResult);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


#endif /* __FISHEYE_CALIBRATE_H__ */

