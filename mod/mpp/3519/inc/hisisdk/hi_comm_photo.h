/******************************************************************************

  Copyright (C), 2016-2036, Hisilicon Tech. Co., Ltd.

 ******************************************************************************
  File Name     : hi_comm_photo.h
  Version       : Initial Draft
  Author        : Hisilicon multimedia software group
  Created       : 
  Last Modified :
  Description   : common definitions for photo algorithms
  Function List :
  History       :
  1.Date        : 20160328
    Author      : 
    Modification: Create
******************************************************************************/

#ifndef __HI_COMM_PHOTO_H__
#define __HI_COMM_PHOTO_H__


#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

#include "hi_type.h"
#include "hi_common.h"
#include "hi_errno.h"
#include "hi_comm_video.h"

typedef   HI_S32 PHOTO_CHN;

typedef enum hiPHOTO_ALG_TYPE_E
{
    PHOTO_ALG_TYPE_LL   = 0x0,
    PHOTO_ALG_TYPE_HDR  = 0x1,
    PHOTO_ALG_TYPE_SR   = 0x2,
    PHOTO_ALG_TYPE_BUTT
} PHOTO_ALG_TYPE_E;

#define PHOTO_FRM_NUM_MAX   8
typedef struct hiPHOTO_SRC_IMG_S
{
    HI_S32 s32FrmNum;
    VIDEO_FRAME_INFO_S astFrm[PHOTO_FRM_NUM_MAX];
}PHOTO_SRC_IMG_S;

typedef struct hiPHOTO_DST_IMG_S
{
    VIDEO_FRAME_INFO_S stFrm;
}PHOTO_DST_IMG_S;


typedef struct hiPHOTO_HDR_ATTR_S
{
    HI_S32 s32NrLuma;          /* noise reduction for luma,   [0, 10], 0=lowest level,10=highest filtering */
    HI_S32 s32NrChroma;        /* noise reduction for chroma  [0, 10], 0=off, 10=highest filtering */
    HI_S32 s32Sharpen;         /* Intelligent edge sharpening [0, 10], 0=off, 10 = largest sharpening */
    HI_S32 s32Saturation;      /* increase color saturation   [0, 10], 0=off, 10 = maximum */
    HI_S32 s32GlobalContrast;  /* global contrast enhancement strength [0, 10] */
    HI_S32 s32LocalContrast;   /* local contrast enhancement strength [0, 10] */
}PHOTO_HDR_ATTR_S;

typedef struct hiPHOTO_LL_ATTR_S
{
    HI_S32 s32NrLuma;          /* noise reduction for luma,   [0, 21], */
    HI_S32 s32NrChroma;        /* noise reduction for chroma  [0, 10], */
    HI_S32 s32Sharpen;         /* Intelligent edge sharpening [0, 10], 0=off, 10 = largest sharpening */
    HI_S32 s32Saturation;      /* increase color saturation   [0, 10], */
    HI_S32 s32Iso;
}PHOTO_LL_ATTR_S;

typedef struct hiPHOTO_SR_ATTR_S
{
    HI_S32 s32NrLuma;          /* noise reduction for luma,   [0, 10] 0=lowest level, 10 highest filtering */
    HI_S32 s32NrChroma;        /* noise reduction for chroma  [0, 10] */
    HI_S32 s32Sharpen;         /* Intelligent edge sharpening [0, 10], 0=off, 10 = largest sharpening */
    HI_S32 s32Saturation;      /* increase color saturation   [0, 10] */
    HI_S32 s32Iso;             /* ISO value used for capturing input photos, set -1 to ignore */
}PHOTO_SR_ATTR_S;

typedef struct hiPHOTO_ALG_ATTR_S
{
    PHOTO_ALG_TYPE_E enAlgType;

    union
    {
        PHOTO_HDR_ATTR_S  stHDRAttr;
        PHOTO_LL_ATTR_S   stLLAttr;
        PHOTO_SR_ATTR_S   stSRAttr;
    }unAttr;
}PHOTO_ALG_ATTR_S;


#define HI_ERR_PHOTO_NULL_PTR        HI_DEF_ERR(HI_ID_PHOTO, EN_ERR_LEVEL_ERROR, EN_ERR_NULL_PTR)
#define HI_ERR_PHOTO_NOTREADY        HI_DEF_ERR(HI_ID_PHOTO, EN_ERR_LEVEL_ERROR, EN_ERR_SYS_NOTREADY)
#define HI_ERR_PHOTO_INVALID_DEVID   HI_DEF_ERR(HI_ID_PHOTO, EN_ERR_LEVEL_ERROR, EN_ERR_INVALID_DEVID)
#define HI_ERR_PHOTO_INVALID_CHNID   HI_DEF_ERR(HI_ID_PHOTO, EN_ERR_LEVEL_ERROR, EN_ERR_INVALID_CHNID)
#define HI_ERR_PHOTO_EXIST           HI_DEF_ERR(HI_ID_PHOTO, EN_ERR_LEVEL_ERROR, EN_ERR_EXIST)
#define HI_ERR_PHOTO_UNEXIST         HI_DEF_ERR(HI_ID_PHOTO, EN_ERR_LEVEL_ERROR, EN_ERR_UNEXIST)
#define HI_ERR_PHOTO_NOT_SUPPORT     HI_DEF_ERR(HI_ID_PHOTO, EN_ERR_LEVEL_ERROR, EN_ERR_NOT_SUPPORT)
#define HI_ERR_PHOTO_NOT_PERM        HI_DEF_ERR(HI_ID_PHOTO, EN_ERR_LEVEL_ERROR, EN_ERR_NOT_PERM)
#define HI_ERR_PHOTO_NOMEM           HI_DEF_ERR(HI_ID_PHOTO, EN_ERR_LEVEL_ERROR, EN_ERR_NOMEM)
#define HI_ERR_PHOTO_NOBUF           HI_DEF_ERR(HI_ID_PHOTO, EN_ERR_LEVEL_ERROR, EN_ERR_NOBUF)
#define HI_ERR_PHOTO_ILLEGAL_PARAM   HI_DEF_ERR(HI_ID_PHOTO, EN_ERR_LEVEL_ERROR, EN_ERR_ILLEGAL_PARAM)
#define HI_ERR_PHOTO_BUSY            HI_DEF_ERR(HI_ID_PHOTO, EN_ERR_LEVEL_ERROR, EN_ERR_BUSY)
#define HI_ERR_PHOTO_BUF_EMPTY       HI_DEF_ERR(HI_ID_PHOTO, EN_ERR_LEVEL_ERROR, EN_ERR_BUF_EMPTY)
#define HI_ERR_PHOTO_SYS_NOTREADY    HI_DEF_ERR(HI_ID_PHOTO, EN_ERR_LEVEL_ERROR, EN_ERR_SYS_NOTREADY)



#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */
#endif /* __HI_COMM_PHOTO_H__ */


