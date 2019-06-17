/******************************************************************************

  Copyright (C), 2001-2011, Hisilicon Tech. Co., Ltd.

 ******************************************************************************
  File Name     : histitch.h
  Version       : Initial Draft
  Author        : Hisilicon multimedia software group
  Created       : 
  Description   :
  History       :
  1.Date        : 2016/04/07
    Author      : h00293269
    Modification: Created file
******************************************************************************/

 
#ifndef __HISTITCH_H__
#define __HISTITCH_H__

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

#include "hi_type.h"

#define HI_ERR_STITCH_NULL_PTR          0xA0F48006
#define HI_ERR_STITCH_ILLEGAL_PARAM     0xA0F48003
#define HI_ERR_STITCH_NOT_SUPPORT       0xA0F48008
#define PMF_COEF_MAX_NUM   9

typedef struct hiOVERLAP_POINT_S
{
    HI_S32   s32X;
    HI_S32   s32Y;
}OVERLAP_POINT_S;

typedef struct hiIMAGE_SIZE_S
{
    HI_U32   u32Width;
    HI_U32   u32Height;    
}IMAGE_SIZE_S;

typedef struct hiPMF_ATTR_S
{
    HI_BOOL     bEnable;
    HI_S32      as32PMFCoef[PMF_COEF_MAX_NUM];    
}PMF_ATTR_S;

typedef enum hiSTITCH_MODE_E{
    STITCH_MODE_PERSPECTIVE = 0,
    STITCH_MODE_CYLINDRICAL = 1,
} STITCH_MODE_E;

typedef struct hiSTITCH_INPUT_PARAM_S
{
    STITCH_MODE_E   enStitchMode;
    IMAGE_SIZE_S    stLeftOriSize;
    IMAGE_SIZE_S    stRightOriSize;
    IMAGE_SIZE_S    stOutSize;
    PMF_ATTR_S      stLeftPMFAttr;
    PMF_ATTR_S      stRightPMFAttr;
    OVERLAP_POINT_S stOverlapPoint[4];
} STITCH_INPUT_PARAM_S;

typedef struct hiSTITCH_ADJUST_S
{
    HI_S32   s32XAdjust; 
    HI_S32   s32YAdjust; 
    HI_S32   s32Theta;  
}STITCH_ADJUST_S;


typedef struct hiSTITCH_ADJUST_INPUT_S
{
    STITCH_INPUT_PARAM_S    stInputParam;
    STITCH_ADJUST_S         stAdjust;
}STITCH_ADJUST_INPUT_S;

typedef struct hiSTITCH_OUTPUT_PARAM_S
{
    IMAGE_SIZE_S    stLeftSize;
    IMAGE_SIZE_S    stRightSize;
    IMAGE_SIZE_S    stOutSize;
    PMF_ATTR_S      stLeftPMFAttr;
    PMF_ATTR_S      stRightPMFAttr;    
    OVERLAP_POINT_S stOverlapPoint[4];
} STITCH_OUTPUT_PARAM_S;


HI_S32 HI_STITCH_AdjustCompute(STITCH_ADJUST_INPUT_S* pstAdjustInput, STITCH_OUTPUT_PARAM_S* pstOutput);


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


#endif /* __HISTITCH_H__ */

