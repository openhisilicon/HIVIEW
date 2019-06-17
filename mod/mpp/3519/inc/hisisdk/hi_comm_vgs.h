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
extern "C" {
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

typedef struct hiVGS_ADD_QUICK_COPY_S
{
    RECT_S                  stSrcRect;			/* start point, width and height of Src */
    RECT_S                  stDestRect;			/* start point, width and height of Dest */
} VGS_ADD_QUICK_COPY_S;

typedef enum hiVGS_COLOR_REVERT_MODE_E
{
	VGS_COLOR_REVERT_RGB = 0,
	VGS_COLOR_REVERT_ALPHA,
	VGS_COLOR_REVERT_BOTH,
	VGS_COLOR_REVERT_BUTT
}VGS_COLOR_REVERT_MODE_E;

typedef struct hiVGS_ADD_REVERT_S
{
    RECT_S                    stSrcRect;			/* start point, width and height of Src */
    RECT_S                    stDestRect;			/* start point, width and height of Dest */
	VGS_COLOR_REVERT_MODE_E enRevertMode;         /* revert mode */
} VGS_ADD_REVERT_S;

typedef struct hiVGS_TASK_ATTR_S
{
    VIDEO_FRAME_INFO_S      stImgIn;             /* input picture */
    VIDEO_FRAME_INFO_S      stImgOut;            /* output picture */
    HI_U32                  au32privateData[4];  /* task's private data */
    HI_U32                  reserved;            /* save current picture's state while debug */
} VGS_TASK_ATTR_S;

typedef struct hiVGS_DRAW_LINE_S
{
    POINT_S                 stStartPoint;	/* line start point */
    POINT_S                 stEndPoint;     /* line end point */

    HI_U32                  u32Thick;       /* width of line */
    HI_U32                  u32Color;       /* color of line */
} VGS_DRAW_LINE_S;

typedef enum hiVGS_COVER_TYPE_E
{
    COVER_RECT = 0,        /* retangle cover */
    COVER_QUAD_RANGLE,     /* quadrangle cover */
    COVER_BUTT
} VGS_COVER_TYPE_E;

typedef struct hiVGS_QUADRANGLE_COVER_S
{
    HI_BOOL bSolid;            /* solid or hollow */
    HI_U32  u32Thick;          /* The thick of the hollow quadrangle */
    POINT_S stPoint[4];        /* four points of the quadrangle */
} VGS_QUADRANGLE_COVER_S;

typedef struct hiVGS_ADD_COVER_S
{
    VGS_COVER_TYPE_E            enCoverType;    /* the cover type */
    union
    {
        RECT_S                  stDstRect;      /* the rectangle attribute */
        VGS_QUADRANGLE_COVER_S  stQuadRangle;   /* the quadrangle attribute */
    };

    HI_U32                      u32Color;       /* color of cover */
} VGS_ADD_COVER_S;

typedef struct hiVGS_ADD_OSD_S
{
    RECT_S                  stRect;			/* start point, width and height of osd */
    HI_U32                  u32BgColor;     /* background color of osd */
    PIXEL_FORMAT_E          enPixelFmt;     /* pixel format of osd */
    HI_U32                  u32PhyAddr;     /* physical address of osd */
    HI_U32                  u32Stride;      /* stride of osd */
    HI_U32                  u32BgAlpha;     /* background alpha of osd */
    HI_U32                  u32FgAlpha;     /* foreground alpha of osd */
} VGS_ADD_OSD_S;


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __HI_COMM_VGS_H__ */
