/******************************************************************************

  Copyright (C), 2013-2033, Hisilicon Tech. Co., Ltd.

 ******************************************************************************
  File Name     : hi_comm_vgs.h
  Version       : 
  Author        : Hisilicon Hi35xx MPP Team
  Created       : 2013/07/24
  Last Modified :
  Description   : common struct definition for VGS
  Function List :
  History       :
******************************************************************************/
#ifndef __HI_COMM_VGS_H__
#define __HI_COMM_VGS_H__

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

#include "hi_type.h"
#include "hi_common.h"
#include "hi_errno.h"
#include "hi_comm_video.h"

/* failure caused by malloc buffer */
#define HI_ERR_VGS_NOBUF           HI_DEF_ERR(HI_ID_VGS, EN_ERR_LEVEL_ERROR, EN_ERR_NOBUF)
#define HI_ERR_VGS_BUF_EMPTY       HI_DEF_ERR(HI_ID_VGS, EN_ERR_LEVEL_ERROR, EN_ERR_BUF_EMPTY)
#define HI_ERR_VGS_NULL_PTR        HI_DEF_ERR(HI_ID_VGS, EN_ERR_LEVEL_ERROR, EN_ERR_NULL_PTR)
#define HI_ERR_VGS_ILLEGAL_PARAM   HI_DEF_ERR(HI_ID_VGS, EN_ERR_LEVEL_ERROR, EN_ERR_ILLEGAL_PARAM)
#define HI_ERR_VGS_BUF_FULL        HI_DEF_ERR(HI_ID_VGS, EN_ERR_LEVEL_ERROR, EN_ERR_BUF_FULL)
#define HI_ERR_VGS_SYS_NOTREADY    HI_DEF_ERR(HI_ID_VGS, EN_ERR_LEVEL_ERROR, EN_ERR_SYS_NOTREADY)
#define HI_ERR_VGS_NOT_SUPPORT     HI_DEF_ERR(HI_ID_VGS, EN_ERR_LEVEL_ERROR, EN_ERR_NOT_SUPPORT)
#define HI_ERR_VGS_NOT_PERMITTED   HI_DEF_ERR(HI_ID_VGS, EN_ERR_LEVEL_ERROR, EN_ERR_NOT_PERM)

#define VGS_PRIVATE_DATA_LEN        6

typedef HI_S32      VGS_HANDLE;

typedef struct hiVGS_TASK_ATTR_S
{
    VIDEO_FRAME_INFO_S      stImgIn;        	/* input picture */
    VIDEO_FRAME_INFO_S      stImgOut;       	/* output picture */
    HI_U32                  au32privateData[4]; /* task's private data */
    HI_U32                  reserved;       	/* save current picture's state while debug */   
}VGS_TASK_ATTR_S;

typedef struct hiVGS_LINE_S
{
    POINT_S 				stStartPoint;	/* line start point [-VGS_MAX_LINE_POS,VGS_MAX_LINE_POS] */
	POINT_S 				stEndPoint;     /* line end point [-VGS_MAX_LINE_POS,VGS_MAX_LINE_POS] */

	HI_U32  				u32Thick;       /* width of line [0, 14] */
	HI_U32  				u32Color;       /* color of line */
}VGS_LINE_S;

typedef enum hiVGS_COVER_TYPE_E
{
    COVER_RECT = 0,
    COVER_QUAD_RANGLE,
    COVER_BUTT
} VGS_COVER_TYPE_E;

typedef struct hiVGS_QUADRANGLE_COVER_S
{
	HI_BOOL bSolid;            /* solid or hollow cover */
    HI_U32  u32Thick;          /* width of hollow cover */	
    POINT_S stPoint[4];        /* four coordinate of quadrangle */
} VGS_QUADRANGLE_COVER_S;

typedef struct hiVGS_COVER_S
{
	VGS_COVER_TYPE_E        	enCoverType;
	union
    {
        RECT_S              	stDstRect;  	/* config of rectrangle */
        VGS_QUADRANGLE_COVER_S  stQuadRangle;   /* config of quadrangle */
    };
	
	HI_U32  u32Color;          					/* color of cover */
}VGS_COVER_S;

typedef struct hiVGS_OSD_S
{
    RECT_S 					stRect;			/* start point, width and height of osd:
    										w:[VGS_MIN_OSD_W, VGS_MAX_OSD_W]
											h:[VGS_MIN_OSD_H, VGS_MAX_OSD_H]
	                                        */
	HI_U32 					u32BgColor;     /* background color of osd */
	PIXEL_FORMAT_E 			enPixelFmt;     /* pixel format of osd */
	HI_U32                  u32PhyAddr;
	HI_U32                  u32Stride;
	HI_U32                  u32BgAlpha;
	HI_U32                  u32FgAlpha;
}VGS_OSD_S;

typedef enum hiVGS_ROTATE_E
{
    VGS_ROTATE_NONE = 0,               /* no rotate */
    VGS_ROTATE_90   = 1,               /* 90 degrees clockwise */
    VGS_ROTATE_180  = 2,               /* 180 degrees clockwise */
    VGS_ROTATE_270  = 3,               /* 270 degrees clockwise */
    VGS_ROTATE_BUTT
} VGS_ROTATE_E;

typedef struct HI_VGS_BORDER_S
{
    HI_U32  u32Width[4]; 	/* width of 4 frames,0:L,1:R,2:B,3:T */
    HI_U32  u32Color; 							/* color of 4 frames,R/G/B */
}VGS_BORDER_S;

typedef struct hiVGS_ASPECTRATIO_S
{
	RECT_S  stVideoRect;
	HI_U32 	u32CoverData;
 }VGS_ASPECTRATIO_S;


typedef struct hiVGS_ONLINE_S
{
    HI_BOOL                 bCrop;              /* if enable crop */
    RECT_S          		stCropRect;
    HI_BOOL                 bHSharpen;          /* if enable sharpen */
    HI_U32                  u32LumaGain; 
    HI_BOOL                 bBorder;            /* if enable Border */
    VGS_BORDER_S        	stBorderOpt; 
	HI_BOOL    				bAspectRatio;		/* if enable aspect ratio */
	VGS_ASPECTRATIO_S 		stAspectRatioOpt;
	
    HI_BOOL                 bForceHFilt;        /* if enable horizontal filter */
    HI_BOOL                 bForceVFilt;        /* if enable vertical filter */	
    HI_BOOL                 bDeflicker;        	/* if enable deflicker */
}VGS_ONLINE_S;



#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __HI_COMM_VGS_H__ */
