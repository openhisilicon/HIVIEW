/******************************************************************************

  Copyright (C), 2013-2033, Hisilicon Tech. Co., Ltd.

 ******************************************************************************
  File Name     : hi_comm_fisheye.h
  Version       :
  Author        : Hisilicon Hi35xx MPP Team
  Created       : 2015/06/15
  Last Modified :
  Description   : common struct definition for FISHEYE
  Function List :
  History       :
******************************************************************************/
#ifndef __HI_COMM_FISHEYE_H__
#define __HI_COMM_FISHEYE_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

#include "hi_type.h"
#include "hi_common.h"
#include "hi_errno.h"
#include "hi_comm_video.h"

#define MAKE_DWORD(high,low) (((low)&0x0000ffff)|((high)<<16))
#define HIGH_WORD(x) (((x)&0xffff0000)>>16)
#define LOW_WORD(x) ((x)&0x0000ffff)

/* failure caused by malloc buffer */
#define HI_ERR_FISHEYE_NOBUF           HI_DEF_ERR(HI_ID_FISHEYE, EN_ERR_LEVEL_ERROR, EN_ERR_NOBUF)
#define HI_ERR_FISHEYE_BUF_EMPTY       HI_DEF_ERR(HI_ID_FISHEYE, EN_ERR_LEVEL_ERROR, EN_ERR_BUF_EMPTY)
#define HI_ERR_FISHEYE_NULL_PTR        HI_DEF_ERR(HI_ID_FISHEYE, EN_ERR_LEVEL_ERROR, EN_ERR_NULL_PTR)
#define HI_ERR_FISHEYE_ILLEGAL_PARAM   HI_DEF_ERR(HI_ID_FISHEYE, EN_ERR_LEVEL_ERROR, EN_ERR_ILLEGAL_PARAM)
#define HI_ERR_FISHEYE_BUF_FULL        HI_DEF_ERR(HI_ID_FISHEYE, EN_ERR_LEVEL_ERROR, EN_ERR_BUF_FULL)
#define HI_ERR_FISHEYE_SYS_NOTREADY    HI_DEF_ERR(HI_ID_FISHEYE, EN_ERR_LEVEL_ERROR, EN_ERR_SYS_NOTREADY)
#define HI_ERR_FISHEYE_NOT_SUPPORT     HI_DEF_ERR(HI_ID_FISHEYE, EN_ERR_LEVEL_ERROR, EN_ERR_NOT_SUPPORT)
#define HI_ERR_FISHEYE_NOT_PERMITTED   HI_DEF_ERR(HI_ID_FISHEYE, EN_ERR_LEVEL_ERROR, EN_ERR_NOT_PERM)

#define FISHEYE_MAX_REGION_NUM 		4
#define FISHEYE_LMFCOEF_NUM		  128
#define FISHEYE_PMFCOEF_NUM			9


typedef HI_S32      FISHEYE_HANDLE;

typedef struct hiFISHEYE_TASK_ATTR_S
{
    VIDEO_FRAME_INFO_S      stImgIn;             /* input picture */
    VIDEO_FRAME_INFO_S      stImgOut;            /* output picture */
    HI_U32                  au32privateData[4];  /* task's private data */
    HI_U32                  reserved;            /* save current picture's state while debug */
} FISHEYE_TASK_ATTR_S;

typedef enum hiFISHEYE_MOUNT_MODE_E
{		
	FISHEYE_DESKTOP_MOUNT 	= 0,		/* desktop mount mode */
	FISHEYE_CEILING_MOUNT	= 1,		/* ceiling mount mode */
	FISHEYE_WALL_MOUNT   	= 2,		/* wall mount mode */
	
    FISHEYE_MOUNT_MODE_BUTT
}FISHEYE_MOUNT_MODE_E;


typedef enum hiFISHEYE_VIEW_MODE_E
{
	FISHEYE_VIEW_360_PANORAMA   = 0, 	/* 360 panorama mode of fisheye correction */
	FISHEYE_VIEW_180_PANORAMA	= 1,	/* 180 panorama mode of fisheye correction */	
	FISHEYE_VIEW_NORMAL   		= 2, 	/* normal mode of fisheye correction */
	FISHEYE_NO_TRANSFORMATION 	= 3, 	/* no fisheye correction */
	
    FISHEYE_VIEW_MODE_BUTT
}FISHEYE_VIEW_MODE_E;

typedef struct hiFISHEYE_REGION_ATTR_S
{
	FISHEYE_VIEW_MODE_E 	enViewMode;		/* fisheye view mode */
	HI_U32 					u32InRadius;    /* inner radius of fisheye correction region*/
	HI_U32 					u32OutRadius;   /* out radius of fisheye correction region*/
	HI_U32 					u32Pan;			/* [0, 360] */
	HI_U32 					u32Tilt;		/* [0, 360] */
	HI_U32 					u32HorZoom;		/* [1, 4095] */
	HI_U32 					u32VerZoom;		/* [1, 4095] */
	RECT_S                  stOutRect; 		
}FISHEYE_REGION_ATTR_S;


typedef struct hiFISHEYE_ATTR_S
{
	HI_BOOL 				bEnable;			/* whether enable fisheye correction or not */
	HI_BOOL 				bLMF; 				/* whether fisheye len's LMF coefficient is from user config or from default linear config */
	HI_BOOL                 bBgColor;       	/* whether use background color or not */
	HI_U32                  u32BgColor;			/* the background color RGB888 [0,0xffffff]*/

	HI_S32 					s32HorOffset;   	/* the horizontal offset between image center and physical center of len*/
	HI_S32 					s32VerOffset;		/* the vertical offset between image center and physical center of len*/
	
	HI_U32                  u32TrapezoidCoef;	/* strength coefficient of trapezoid correction */
    HI_S32                  s32FanStrength;     /* strength coefficient of fan correction */ 
    
	FISHEYE_MOUNT_MODE_E    enMountMode;		/* fisheye mount mode */	
	
	HI_U32               	u32RegionNum;       /* fisheye correction region number [1, FISHEYE_MAX_REGION_NUM] */
	FISHEYE_REGION_ATTR_S 	astFisheyeRegionAttr[FISHEYE_MAX_REGION_NUM];/* attribution of fisheye correction region */	 
}FISHEYE_ATTR_S;

typedef struct hiSPREAD_ATTR_S
{
	HI_BOOL 				bEnable;			/* whether enable fisheye correction or not */
	HI_U32					u32SpreadCoef;		/* strength coefficient of spread correction */
	SIZE_S      			stDestSize;			/*dest size of spread*/
}SPREAD_ATTR_S;


typedef struct hiFISHEYE_CYLIND_ATTR_S
{
    HI_S32 s32CenterXOffset;        /* Horizontal offset of the image distortion center relative to image center.*/
    HI_S32 s32CenterYOffset;        /* Vertical offset of the image distortion center relative to image center.*/
    HI_S32 s32Ratio;                /* Distortion ratio.*/
} FISHEYE_CYLIND_ATTR_S;

typedef struct hiFISHEYE_CYLIND_ATTR_EX_S
{
    FISHEYE_CYLIND_ATTR_S   stCylindAttr;   /*Cylind Nomal Attr*/
    HI_S32                  s32FanStrength; /*Strength coefficient of fan correction*/
} FISHEYE_CYLIND_ATTR_EX_S;

typedef struct hiFISHEYE_PMF_ATTR_S
{
	HI_S32      				as32PMFCoef[FISHEYE_PMFCOEF_NUM];
}FISHEYE_PMF_ATTR_S;

typedef struct hiFISHEYE_JOB_CONFIG_S
{    
	HI_U32					u32LenMapPhyAddr;	/*LMF coefficient Physic Addr*/
}FISHEYE_JOB_CONFIG_S;

typedef struct hiFISHEYE_CONFIG_S
{
    HI_U16               	au16LMFCoef[FISHEYE_LMFCOEF_NUM]; 	/* LMF coefficient of fisheye len */
}FISHEYE_CONFIG_S;


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __HI_COMM_FISHEYE_H__ */
