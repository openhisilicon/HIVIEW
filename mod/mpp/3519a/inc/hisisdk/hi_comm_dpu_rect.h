/******************************************************************************

  Copyright (C), 2017-2018, Hisilicon Tech. Co., Ltd.

 ******************************************************************************
  File Name     : hi_comm_rect.h
  Version       : Initial Draft
  Author        : Hisilicon multimedia software group
  Created       : 2017/09/20
  Description   :
  History       :
  1.Date        : 2017/09/20
    Modification: Created file

******************************************************************************/

#ifndef __HI_COMM_RECT_H__
#define __HI_COMM_RECT_H__


#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */
#include "hi_common.h"
#include "hi_errno.h"
#include "hi_defines.h"
#include "hi_comm_video.h"

#define DPU_RECT_LEFT_PIPE 0
#define DPU_RECT_RIGHT_PIPE 1
#define DPU_RECT_LEFT_CHN 0
#define DPU_RECT_RIGHT_CHN 1

typedef enum hiEN_DPU_RECT_ERR_CODE_E
{
    ERR_DPU_RECT_SYS_TIMEOUT    = 0x40,   /* DPU RECT process timeout */
    ERR_DPU_RECT_OPEN_FILE      = 0x41,   /* DPU RECT open file error */
    ERR_DPU_RECT_READ_FILE      = 0x42,   /* DPU RECT read file error */
    ERR_DPU_RECT_WRITE_FILE     = 0x43,   /* DPU RECT write file error */

    ERR_DPU_RECT_BUTT
}EN_DPU_RECT_ERR_CODE_E;

#define HI_ERR_DPU_RECT_NULL_PTR        HI_DEF_ERR(HI_ID_DPU_RECT, EN_ERR_LEVEL_ERROR, EN_ERR_NULL_PTR)
#define HI_ERR_DPU_RECT_NOTREADY        HI_DEF_ERR(HI_ID_DPU_RECT, EN_ERR_LEVEL_ERROR, EN_ERR_SYS_NOTREADY)
#define HI_ERR_DPU_RECT_INVALID_DEVID   HI_DEF_ERR(HI_ID_DPU_RECT, EN_ERR_LEVEL_ERROR, EN_ERR_INVALID_DEVID)
#define HI_ERR_DPU_RECT_INVALID_CHNID   HI_DEF_ERR(HI_ID_DPU_RECT, EN_ERR_LEVEL_ERROR, EN_ERR_INVALID_CHNID)
#define HI_ERR_DPU_RECT_EXIST           HI_DEF_ERR(HI_ID_DPU_RECT, EN_ERR_LEVEL_ERROR, EN_ERR_EXIST)
#define HI_ERR_DPU_RECT_UNEXIST         HI_DEF_ERR(HI_ID_DPU_RECT, EN_ERR_LEVEL_ERROR, EN_ERR_UNEXIST)
#define HI_ERR_DPU_RECT_NOT_SUPPORT     HI_DEF_ERR(HI_ID_DPU_RECT, EN_ERR_LEVEL_ERROR, EN_ERR_NOT_SUPPORT)
#define HI_ERR_DPU_RECT_NOT_PERM        HI_DEF_ERR(HI_ID_DPU_RECT, EN_ERR_LEVEL_ERROR, EN_ERR_NOT_PERM)
#define HI_ERR_DPU_RECT_NOMEM           HI_DEF_ERR(HI_ID_DPU_RECT, EN_ERR_LEVEL_ERROR, EN_ERR_NOMEM)
#define HI_ERR_DPU_RECT_NOBUF           HI_DEF_ERR(HI_ID_DPU_RECT, EN_ERR_LEVEL_ERROR, EN_ERR_NOBUF)
#define HI_ERR_DPU_RECT_ILLEGAL_PARAM   HI_DEF_ERR(HI_ID_DPU_RECT, EN_ERR_LEVEL_ERROR, EN_ERR_ILLEGAL_PARAM)
#define HI_ERR_DPU_RECT_BUSY            HI_DEF_ERR(HI_ID_DPU_RECT, EN_ERR_LEVEL_ERROR, EN_ERR_BUSY)
#define HI_ERR_DPU_RECT_BUF_EMPTY       HI_DEF_ERR(HI_ID_DPU_RECT, EN_ERR_LEVEL_ERROR, EN_ERR_BUF_EMPTY)

#define HI_ERR_DPU_RECT_SYS_TIMEOUT       HI_DEF_ERR(HI_ID_DPU_RECT, EN_ERR_LEVEL_ERROR, ERR_DPU_RECT_SYS_TIMEOUT)
#define HI_ERR_DPU_RECT_OPEN_FILE         HI_DEF_ERR(HI_ID_DPU_RECT, EN_ERR_LEVEL_ERROR, ERR_DPU_RECT_OPEN_FILE)
#define HI_ERR_DPU_RECT_READ_FILE         HI_DEF_ERR(HI_ID_DPU_RECT, EN_ERR_LEVEL_ERROR, ERR_DPU_RECT_READ_FILE)
#define HI_ERR_DPU_RECT_WRITE_FILE        HI_DEF_ERR(HI_ID_DPU_RECT, EN_ERR_LEVEL_ERROR, ERR_DPU_RECT_WRITE_FILE)

typedef   HI_S32 DPU_RECT_LUT_ID;
typedef   HI_S32 DPU_RECT_GRP;
typedef   HI_S32 DPU_RECT_PIPE;
typedef   HI_S32 DPU_RECT_CHN;

typedef struct hiDPU_RECT_MEM_INFO_S
{
	HI_U64  u64PhyAddr;
	HI_U64  u64VirAddr;
	HI_U32  u32Size;
}DPU_RECT_MEM_INFO_S;

typedef enum hiDPU_RECT_MODE_E
{
    DPU_RECT_MODE_SINGLE = 0x0, /* only channel 0 work */
    DPU_RECT_MODE_DOUBLE = 0x1, /* two channel work */
    DPU_RECT_MODE_BUTT
}DPU_RECT_MODE_E;

typedef struct hiDPU_RECT_GRP_ATTR_S
{
    DPU_RECT_MODE_E enRectMode; /* Rectification mode, it can not be changed dynamic */
    SIZE_S stLeftImageSize; /* Left image size. */
    SIZE_S stRightImageSize; /* Right image size. */
    DPU_RECT_LUT_ID LeftLutId;	/* Left image rectification lut */
    DPU_RECT_LUT_ID RightLutId;	/* Right image rectification lut */
    HI_U32 u32Depth;  /* The depth of user image queue for getting Rectification output image,
                        it can not be changed dynamic. Range:[0,8] */
    HI_BOOL bNeedSrcFrame; /* The flag of getting source videoframe. */
    FRAME_RATE_CTRL_S stFrameRate;  /* Grp frame rate contrl. */
}DPU_RECT_GRP_ATTR_S;

typedef struct hiDPU_RECT_CHN_ATTR_S
{
    SIZE_S stImageSize;  /* Rectify output image size */
}DPU_RECT_CHN_ATTR_S;

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


#endif /* __HI_COMM_DPU_RECT_H__ */

