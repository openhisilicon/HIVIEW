/******************************************************************************

  Copyright (C), 2001-2011, Hisilicon Tech. Co., Ltd.

 ******************************************************************************
  File Name     : histitch.h
  Version       : Initial Draft
  Author        : Hisilicon multimedia software group
  Created       :
  Description   :
  History       :
  1.Date        : 2016/09/25
    Author      : 
    Modification: Created file
******************************************************************************/


#ifndef __HIGDC_H__
#define __HIGDC_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

#include "hi_comm_video.h"

#define HI_ERR_FISHEYE_CALIBRATE_NULL_PTR          0xA0258011
#define HI_ERR_FISHEYE_CALIBRATE_ILLEGAL_PARAM     0xA0258012
#define HI_ERR_FISHEYE_CALIBRATE_NOT_SUPPORT       0xA0258013


typedef enum hiFISHEYE_CALIBRATE_LEVEL_E
{
    LEVEL_0 = 0,    /*calibrate level 0*/
    LEVEL_1 = 1,    /*calibrate level 1*/
    LEVEL_2 = 2,    /*calibrate level 2*/

    LEVEL_BUTT
} FISHEYE_CALIBRATE_LEVEL_E;


typedef struct hiFISHEYE_CALIBTATE_RESULT_S
{
    HI_S32 s32OffsetV;      /* the horizontal offset between image center and physical center of len*/
    HI_S32 s32OffsetH;      /* the vertical offset between image center and physical center of len*/
    HI_S32 s32Radius_X;     /* the X coordinate of physical center of len*/
    HI_S32 s32Radius_Y;     /* the Y coordinate of physical center of len*/
    HI_U32 u32Radius;       /* the radius of len*/
} FISHEYE_CALIBTATE_RESULT_S;

typedef struct hiCALIBTATE_OUTPUT_S
{
    FISHEYE_CALIBTATE_RESULT_S stCalibrateResult;   /*the output of result*/
} CALIBTATE_OUTPUT_S;


HI_S32 HI_FISHEYE_ComputeCalibrateResult(VIDEO_FRAME_S *pstVFramIn, FISHEYE_CALIBRATE_LEVEL_E enLevel, CALIBTATE_OUTPUT_S *pOutCalibrate);
HI_S32 HI_FISHEYE_MarkCalibrateResult(VIDEO_FRAME_S *pstPicIn, VIDEO_FRAME_S *pstPicOut, FISHEYE_CALIBTATE_RESULT_S *pCalibrateResult);


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


#endif /* __HISTITCH_H__ */

