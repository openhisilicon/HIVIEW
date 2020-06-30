/******************************************************************************

  Copyright (C), 2017-2018, Hisilicon Tech. Co., Ltd.

 ******************************************************************************
  File Name     : hi_comm_photo.h
  Version       : Initial Draft
  Author        : Hisilicon multimedia software group
  Created       :
  Last Modified :
  Description   : common definitions for photo algorithms
  Function List :
  History       :
  1.Date        : 20171216
    Author      :
    Modification: Create
******************************************************************************/

#ifndef __HI_COMM_PHOTO_H__
#define __HI_COMM_PHOTO_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

#include "hi_type.h"
#include "hi_common.h"
#include "hi_errno.h"
#include "hi_math.h"
#include "hi_comm_video.h"

#define PHOTO_HDR_FRAME_NUM  3
#define PHOTO_MFNR_FRAME_NUM 4

#define PHOTO_MIN_WIDTH  1280
#define PHOTO_MIN_HEIGHT 720
#define PHOTO_MAX_WIDTH  7680
#define PHOTO_MAX_HEIGHT 4320

#define PHOTO_HDR_ISO_LEVEL_CNT  10
#define PHOTO_SFNR_ISO_LEVEL_CNT 8
#define PHOTO_MFNR_ISO_LEVEL_CNT 8
#define PHOTO_DE_ISO_LEVEL_CNT   8

#define PHOTO_MAX_FACE_NUM 10

typedef enum hiPHOTO_ALG_TYPE_E {
    PHOTO_ALG_TYPE_HDR = 0x0,
    PHOTO_ALG_TYPE_SFNR = 0x1,
    PHOTO_ALG_TYPE_MFNR = 0x2,
    PHOTO_ALG_TYPE_DE = 0x3,

    PHOTO_ALG_TYPE_BUTT
} PHOTO_ALG_TYPE_E;

typedef struct hiPHOTO_FACE_INFO_S {
    HI_U32 u32Id;  // reserved
    RECT_S stFaceRect;
    RECT_S stLeftEyeRect;   // reserved
    RECT_S stRightEyeRect;  // reserved
    HI_U32 u32BlinkScore;   // reserved
    HI_U32 u32SmileScore;   // reserved
} PHOTO_FACE_INFO_S;

typedef struct hiPHOTO_DARK_MOTION_DETECTION_PARAM_S {
    HI_S32 s32IsoSpeed;
    HI_S32 s32MotionLowGray;
    HI_S32 s32MotionHighGray;
    HI_FLOAT f32MotionRatio;
    HI_S32 s32NightAverageLuma;
} PHOTO_DARK_MOTION_DETECTION_PARAM_S;

typedef struct hiPHOTO_IMAGE_FUSION_PARAM_S {
    HI_S32 s32IsoSpeed;
    HI_S32 s32PyramidTopSize;
    HI_S32 s32WeightCurveMethod;
    HI_S32 s32WeightCalcMethod;
    HI_S32 s32BlendUVGain;
    HI_S32 s32DetailEnhanceRatioL0;
    HI_S32 s32DetailEnhanceRatioL1;
    HI_FLOAT f32BlendSigma;
} PHOTO_IMAGE_FUSION_PARAM_S;

typedef struct hiPHOTO_HDR_COEF_S {
    HI_S32 s32AjustRatio;
    HI_S32 s32ImageScaleMethod;
    PHOTO_DARK_MOTION_DETECTION_PARAM_S astMotionDetectionParam[PHOTO_HDR_ISO_LEVEL_CNT];
    PHOTO_IMAGE_FUSION_PARAM_S astImageFusionParam[PHOTO_HDR_ISO_LEVEL_CNT];
} PHOTO_HDR_COEF_S;

typedef struct hiPHOTO_SFNR_ISO_STRATEGY_S {
    HI_S32 s32IsoValue;
    HI_S32 s32Luma;
    HI_S32 s32Chroma;
    HI_S32 s32LumaHF;
    HI_S32 s32ChromaHF;
} PHOTO_SFNR_ISO_STRATEGY_S;

typedef struct hiPHOTO_SFNR_COEF_S {
    PHOTO_SFNR_ISO_STRATEGY_S astIsoStrat[PHOTO_SFNR_ISO_LEVEL_CNT];
} PHOTO_SFNR_COEF_S;

typedef struct hiPHOTO_MFNR_3DNR_ISO_STRATEGY_S {
    HI_S32 s32IsoValue;
    HI_S32 s32Stnr;
    HI_S32 s32TnrFrmNum;
    HI_S32 s32StnrDarkLess;
    HI_S32 s32StnrGhostLess;
    HI_S32 s32LumaAlpha;
    HI_S32 s32LumaDelta;
    HI_S32 s32ChromaAlpha;
    HI_S32 s32ChromaDelta;
} PHOTO_MFNR_3DNR_ISO_STRATEGY_S;

typedef struct hiPHOTO_MFNR_3DNR_PARAM_S {
    PHOTO_MFNR_3DNR_ISO_STRATEGY_S ast3DNRIsoStrat[PHOTO_MFNR_ISO_LEVEL_CNT];
} PHOTO_MFNR_3DNR_PARAM_S;

typedef struct hiPHOTO_MFNR_2DNR_ISO_STRATEGY_S {
    HI_S32 s32IsoValue;
    HI_S32 s32Luma;
    HI_S32 s32Chroma;
    HI_S32 s32LumaHF2;
    HI_S32 s32DetailMin;
} PHOTO_MFNR_2DNR_ISO_STRATEGY_S;

typedef struct hiPHOTO_MFNR_2DNR_PARAM_S {
    PHOTO_MFNR_2DNR_ISO_STRATEGY_S ast2DNRIsoStrat[PHOTO_MFNR_ISO_LEVEL_CNT];
} PHOTO_MFNR_2DNR_PARAM_S;

typedef struct hiPHOTO_DE_ISO_STRATEGY_S {
    HI_S32 s32IsoValue;
    HI_S32 s32GlobalGain;
    HI_S32 s32GainLF;
    HI_S32 s32GainHFPos;
    HI_S32 s32GainHFNeg;
    HI_S32 s32LumaScaleX0;
    HI_S32 s32LumaScaleX1;
    HI_S32 s32LumaScaleY1;
    HI_S32 s32SatuGainX0;
    HI_S32 s32SatuGainX1;
    HI_S32 s32SatuGainY1;
} PHOTO_DE_ISO_STRATEGY_S;

typedef struct hiPHOTO_DE_COEF_S {
    PHOTO_DE_ISO_STRATEGY_S astDEIsoStrat[PHOTO_DE_ISO_LEVEL_CNT];
} PHOTO_DE_COEF_S;

typedef struct hiPHOTO_MFNR_COEF_S {
    HI_BOOL bImageScale;
    PHOTO_MFNR_3DNR_PARAM_S st3DNRParam;
    PHOTO_MFNR_2DNR_PARAM_S st2DNRParam;

    HI_BOOL bDEEnable;
    PHOTO_DE_COEF_S stPhotoDECoef;
} PHOTO_MFNR_COEF_S;

typedef struct hiPHOTO_ALG_COEF_S {
    union {
        PHOTO_HDR_COEF_S stPhotoHdrCoef;
        PHOTO_SFNR_COEF_S stPhotoSfnrCoef;
        PHOTO_MFNR_COEF_S stPhotoMfnrCoef;
        PHOTO_DE_COEF_S stPhotoDECoef;
    };
} PHOTO_ALG_COEF_S;

#if 0
typedef HI_U32 (*PFN_PHOTO_AlgMemMalloc)(HI_U32 u32MemSize, HI_U64 *pu64PhyAddr, HI_VOID **ppVirAddr);
typedef HI_U32 (*PFN_PHOTO_AlgMemFree)(HI_U64 u64PhyAddr, HI_VOID *pVirAddr);

typedef struct hiPHOTO_ALG_INIT_S {
    PFN_PHOTO_AlgMalloc pfnMallocCB;
    PFN_PHOTO_AlgMemFree pfnFreeCB;
    HI_BOOL     bPrintDebugInfo;
} PHOTO_ALG_INIT_S;

#else

typedef struct hiPHOTO_ALG_INIT_S {
    HI_U64 u64PublicMemPhyAddr;
    HI_U64 u64PublicMemVirAddr;
    HI_U32 u32PublicMemSize;
    HI_BOOL bPrintDebugInfo;
} PHOTO_ALG_INIT_S;

#endif

typedef struct hiPHOTO_HDR_ATTR_S {
    VIDEO_FRAME_INFO_S stSrcFrm;
    VIDEO_FRAME_INFO_S stDesFrm;
    HI_U32 u32FrmIndex;
    HI_U32 u32ISO;
    HI_U32 u32FaceNum;
    PHOTO_FACE_INFO_S astFaceInfo[PHOTO_MAX_FACE_NUM];
} PHOTO_HDR_ATTR_S;

typedef struct hiPHOTO_SFNR_ATTR_S {
    VIDEO_FRAME_INFO_S stFrm;  // src & des
    HI_U32 u32ISO;
} PHOTO_SFNR_ATTR_S;

typedef struct hiPHOTO_MFNR_ATTR_S {
    VIDEO_FRAME_INFO_S stSrcFrm;
    VIDEO_FRAME_INFO_S stDesFrm;
    VIDEO_FRAME_INFO_S stRawFrm;
    HI_U32 u32FrmIndex;
    HI_U32 u32ISO;

} PHOTO_MFNR_ATTR_S;

typedef struct hiPHOTO_DE_ATTR_S {
    VIDEO_FRAME_INFO_S stFrm;  // src & des
    VIDEO_FRAME_INFO_S stRawFrm;
    HI_U32 u32ISO;
} PHOTO_DE_ATTR_S;

typedef struct hiPHOTO_ALG_ATTR_S {
    union {
        PHOTO_HDR_ATTR_S stHDRAttr;
        PHOTO_SFNR_ATTR_S stSFNRAttr;
        PHOTO_MFNR_ATTR_S stMFNRAttr;
        PHOTO_DE_ATTR_S stDEAttr;
    };
} PHOTO_ALG_ATTR_S;

__inline static HI_U32 HDR_GetPublicMemSize(HI_U32 u32Width, HI_U32 u32Height)
{
    HI_U32 u32Stride = ALIGN_UP(u32Width, 128);
    HI_U32 u32PubMemSize = u32Stride * u32Height * 13.4 + 7000000 + 1024;

    return u32PubMemSize;
}

__inline static HI_U32 MFNR_GetPublicMemSize(HI_U32 u32Width, HI_U32 u32Height)
{
    HI_U32 u32Stride = ALIGN_UP(u32Width, 128);
    HI_U32 u32PubMemSize = u32Stride * u32Height * 12.6 + 6000000 + 1024;

    return u32PubMemSize;
}

__inline static HI_U32 SFNR_GetPublicMemSize(HI_U32 u32Width, HI_U32 u32Height)
{
    HI_U32 u32Stride = ALIGN_UP(u32Width, 128);
    HI_U32 u32PubMemSize = u32Stride * u32Height * 4.2 + 650000 + 1024;

    return u32PubMemSize;
}

__inline static HI_U32 DE_GetPublicMemSize(HI_U32 u32Width, HI_U32 u32Height)
{
    HI_U32 u32Stride = ALIGN_UP(u32Width >> 1, 128);
    HI_U32 u32PubMemSize = u32Stride * u32Height + 1024 + 1024;

    return u32PubMemSize;
}

#define HI_ERR_PHOTO_NULL_PTR      HI_DEF_ERR(HI_ID_PHOTO, EN_ERR_LEVEL_ERROR, EN_ERR_NULL_PTR)
#define HI_ERR_PHOTO_NOTREADY      HI_DEF_ERR(HI_ID_PHOTO, EN_ERR_LEVEL_ERROR, EN_ERR_SYS_NOTREADY)
#define HI_ERR_PHOTO_INVALID_DEVID HI_DEF_ERR(HI_ID_PHOTO, EN_ERR_LEVEL_ERROR, EN_ERR_INVALID_DEVID)
#define HI_ERR_PHOTO_INVALID_CHNID HI_DEF_ERR(HI_ID_PHOTO, EN_ERR_LEVEL_ERROR, EN_ERR_INVALID_CHNID)
#define HI_ERR_PHOTO_EXIST         HI_DEF_ERR(HI_ID_PHOTO, EN_ERR_LEVEL_ERROR, EN_ERR_EXIST)
#define HI_ERR_PHOTO_UNEXIST       HI_DEF_ERR(HI_ID_PHOTO, EN_ERR_LEVEL_ERROR, EN_ERR_UNEXIST)
#define HI_ERR_PHOTO_NOT_CONFIG    HI_DEF_ERR(HI_ID_PHOTO, EN_ERR_LEVEL_ERROR, EN_ERR_NOT_CONFIG)
#define HI_ERR_PHOTO_NOT_SUPPORT   HI_DEF_ERR(HI_ID_PHOTO, EN_ERR_LEVEL_ERROR, EN_ERR_NOT_SUPPORT)
#define HI_ERR_PHOTO_NOT_PERM      HI_DEF_ERR(HI_ID_PHOTO, EN_ERR_LEVEL_ERROR, EN_ERR_NOT_PERM)
#define HI_ERR_PHOTO_NOMEM         HI_DEF_ERR(HI_ID_PHOTO, EN_ERR_LEVEL_ERROR, EN_ERR_NOMEM)
#define HI_ERR_PHOTO_NOBUF         HI_DEF_ERR(HI_ID_PHOTO, EN_ERR_LEVEL_ERROR, EN_ERR_NOBUF)
#define HI_ERR_PHOTO_ILLEGAL_PARAM HI_DEF_ERR(HI_ID_PHOTO, EN_ERR_LEVEL_ERROR, EN_ERR_ILLEGAL_PARAM)
#define HI_ERR_PHOTO_BUSY          HI_DEF_ERR(HI_ID_PHOTO, EN_ERR_LEVEL_ERROR, EN_ERR_BUSY)
#define HI_ERR_PHOTO_BUF_EMPTY     HI_DEF_ERR(HI_ID_PHOTO, EN_ERR_LEVEL_ERROR, EN_ERR_BUF_EMPTY)

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */
#endif /* __HI_COMM_PHOTO_H__ */


