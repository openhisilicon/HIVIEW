
/******************************************************************************
Copyright (C), 2016-2018, Hisilicon Tech. Co., Ltd.
******************************************************************************
File Name     : hi_comm_vpss.h
Version       : Initial Draft
Author        : Hisilicon multimedia software group
Created       : 2016/09/27
Last Modified :
Description   :
Function List :
******************************************************************************/

#ifndef __HI_COMM_VPSS_H__
#define __HI_COMM_VPSS_H__


#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

#include "hi_type.h"
#include "hi_common.h"
#include "hi_errno.h"
#include "hi_comm_video.h"

#define VPSS_SHARPEN_GAIN_NUM        (32)
#define VPSS_AUTO_ISO_STRENGTH_NUM   (16)
#define VPSS_YUV_SHPLUMA_NUM         (32)

#define HI_ERR_VPSS_NULL_PTR        HI_DEF_ERR(HI_ID_VPSS, EN_ERR_LEVEL_ERROR, EN_ERR_NULL_PTR)
#define HI_ERR_VPSS_NOTREADY        HI_DEF_ERR(HI_ID_VPSS, EN_ERR_LEVEL_ERROR, EN_ERR_SYS_NOTREADY)
#define HI_ERR_VPSS_INVALID_DEVID   HI_DEF_ERR(HI_ID_VPSS, EN_ERR_LEVEL_ERROR, EN_ERR_INVALID_DEVID)
#define HI_ERR_VPSS_INVALID_CHNID   HI_DEF_ERR(HI_ID_VPSS, EN_ERR_LEVEL_ERROR, EN_ERR_INVALID_CHNID)
#define HI_ERR_VPSS_EXIST           HI_DEF_ERR(HI_ID_VPSS, EN_ERR_LEVEL_ERROR, EN_ERR_EXIST)
#define HI_ERR_VPSS_UNEXIST         HI_DEF_ERR(HI_ID_VPSS, EN_ERR_LEVEL_ERROR, EN_ERR_UNEXIST)
#define HI_ERR_VPSS_NOT_SUPPORT     HI_DEF_ERR(HI_ID_VPSS, EN_ERR_LEVEL_ERROR, EN_ERR_NOT_SUPPORT)
#define HI_ERR_VPSS_NOT_PERM        HI_DEF_ERR(HI_ID_VPSS, EN_ERR_LEVEL_ERROR, EN_ERR_NOT_PERM)
#define HI_ERR_VPSS_NOMEM           HI_DEF_ERR(HI_ID_VPSS, EN_ERR_LEVEL_ERROR, EN_ERR_NOMEM)
#define HI_ERR_VPSS_NOBUF           HI_DEF_ERR(HI_ID_VPSS, EN_ERR_LEVEL_ERROR, EN_ERR_NOBUF)
#define HI_ERR_VPSS_ILLEGAL_PARAM   HI_DEF_ERR(HI_ID_VPSS, EN_ERR_LEVEL_ERROR, EN_ERR_ILLEGAL_PARAM)
#define HI_ERR_VPSS_BUSY            HI_DEF_ERR(HI_ID_VPSS, EN_ERR_LEVEL_ERROR, EN_ERR_BUSY)
#define HI_ERR_VPSS_BUF_EMPTY       HI_DEF_ERR(HI_ID_VPSS, EN_ERR_LEVEL_ERROR, EN_ERR_BUF_EMPTY)

#define VPSS_INVALID_FRMRATE   -1
#define VPSS_CHN0               0
#define VPSS_CHN1               1
#define VPSS_CHN2               2
#define VPSS_CHN3               3
#define VPSS_INVALID_CHN       -1

typedef enum hiNR_MOTION_MODE_E
{
    NR_MOTION_MODE_NORMAL          = 0,        /* normal */
    NR_MOTION_MODE_COMPENSATE      = 1,        /* motion compensate */
    NR_MOTION_MODE_COMPENSATE_PART = 2,        /* motion compensate locality */
    NR_MOTION_MODE_BUTT
}NR_MOTION_MODE_E;

typedef struct hiVPSS_NR_ATTR_S
{
    COMPRESS_MODE_E     enCompressMode;   /* RW; Reference frame compress mode */
    NR_MOTION_MODE_E    enNrMotionMode;   /* RW; NR motion compensate mode. */
}VPSS_NR_ATTR_S;

typedef struct hiVPSS_GRP_ATTR_S
{
    HI_U32                     u32MaxW;           /* RW; Range: Hi3559AV100 = [64, 16384] | Hi3519AV100 = [64, 8192]; Width of source image. */
    HI_U32                     u32MaxH;           /* RW; Range: Hi3559AV100 = [64, 16384] | Hi3519AV100 = [64, 8192]; Height of source image. */
    PIXEL_FORMAT_E             enPixelFormat;     /* RW; Pixel format of source image. */
    DYNAMIC_RANGE_E            enDynamicRange;    /* RW; DynamicRange of source image. */
    FRAME_RATE_CTRL_S          stFrameRate;       /* Grp frame rate contrl. */
    HI_BOOL                    bNrEn;             /* RW; NR enable. */
    VPSS_NR_ATTR_S             stNrAttr;          /* RW; NR attr. */
} VPSS_GRP_ATTR_S;

typedef enum hiVPSS_CHN_MODE_E
{
    VPSS_CHN_MODE_USER  = 0,       /* User mode. */
    VPSS_CHN_MODE_AUTO  = 1        /* Auto mode. */

} VPSS_CHN_MODE_E;

typedef struct hiVPSS_CHN_ATTR_S
{
    VPSS_CHN_MODE_E     enChnMode;          /* RW; Vpss channel's work mode. */
    HI_U32              u32Width;           /* RW; Range: Hi3559AV100 = [64, 16384] | Hi3519AV100 = [64, 8192]; Width of target image. */
    HI_U32              u32Height;          /* RW; Range: Hi3559AV100 = [64, 16384] | Hi3519AV100 = [64, 8192]; Height of target image. */
    VIDEO_FORMAT_E      enVideoFormat;      /* RW; Video format of target image. */
    PIXEL_FORMAT_E      enPixelFormat;      /* RW; Pixel format of target image. */
    DYNAMIC_RANGE_E     enDynamicRange;     /* RW; DynamicRange of target image. */
    COMPRESS_MODE_E     enCompressMode;     /* RW; Compression mode of the output. */
    FRAME_RATE_CTRL_S   stFrameRate;        /* Frame rate control info */
    HI_BOOL             bMirror;            /* RW; Mirror enable. */
    HI_BOOL             bFlip;              /* RW; Flip enable. */
    HI_U32              u32Depth;           /* RW; Range: [0, 8]; User get list depth. */
    ASPECT_RATIO_S      stAspectRatio;      /* Aspect Ratio info. */
} VPSS_CHN_ATTR_S;

typedef enum hiVPSS_CROP_COORDINATE_E
{
    VPSS_CROP_RATIO_COOR = 0,   /* Ratio coordinate. */
    VPSS_CROP_ABS_COOR          /* Absolute coordinate. */
} VPSS_CROP_COORDINATE_E;

typedef struct hiVPSS_CROP_INFO_S
{
    HI_BOOL                 bEnable;            /* RW; CROP enable. */
    VPSS_CROP_COORDINATE_E  enCropCoordinate;   /* RW; Coordinate mode of the crop start point. */
    RECT_S                  stCropRect;         /* CROP rectangular. */
} VPSS_CROP_INFO_S;

typedef struct hiVPSS_LDC_ATTR_S
{
    HI_BOOL     bEnable;                        /* RW;Whether LDC is enbale */
    LDC_ATTR_S  stAttr;
} VPSS_LDC_ATTR_S;

typedef struct hiVPSS_ROTATION_EX_ATTR_S
{
    HI_BOOL       bEnable;                      /* Whether ROTATE_EX_S is enbale */
    ROTATION_EX_S stRotationEx;                 /* Rotate Attribute */
}VPSS_ROTATION_EX_ATTR_S;

typedef struct hiVPSS_LOW_DELAY_INFO_S
{
    HI_BOOL bEnable;          /* RW; Low delay enable. */
    HI_U32 u32LineCnt;        /* RW; Range: [16, 16384]; Low delay shoreline. */
}VPSS_LOW_DELAY_INFO_S;

typedef struct hiVPSS_EXT_CHN_ATTR_S
{
    VPSS_CHN           s32BindChn;      /* RW; Range: [0, 3]; Channel bind to. */
    HI_U32             u32Width;        /* RW; Range: Hi3559AV100 = [64, 16384] | Hi3519AV100 = [64, 8192]; Width of target image. */
    HI_U32             u32Height;       /* RW; Range: Hi3559AV100 = [64, 16384] | Hi3519AV100 = [64, 8192]; Height of target image. */
    VIDEO_FORMAT_E     enVideoFormat;   /* RW; Video format of target image. */
    PIXEL_FORMAT_E     enPixelFormat;   /* RW; Pixel format of target image. */
    DYNAMIC_RANGE_E    enDynamicRange;  /* RW; Dynamic range. */
    COMPRESS_MODE_E    enCompressMode;  /* RW; Compression mode of the output. */
    HI_U32             u32Depth;        /* RW; Range: [0, 8]; User get list depth. */
    FRAME_RATE_CTRL_S  stFrameRate;     /* Frame rate control info */
} VPSS_EXT_CHN_ATTR_S;



typedef struct hiVPSS_GRP_SHARPEN_MANUAL_ATTR_S
{
    HI_U16 au16TextureStr[VPSS_SHARPEN_GAIN_NUM];    /* RW; Range: [0, 4095]; Undirectional sharpen strength for texture and detail enhancement*/
    HI_U16 au16EdgeStr[VPSS_SHARPEN_GAIN_NUM];       /* RW; Range: [0, 4095]; Directional sharpen strength for edge enhancement*/
    HI_U16 u16TextureFreq;                           /* RW; Range: [0, 4095]; Texture frequency adjustment. Texture and detail will be finer when it increase*/
    HI_U16 u16EdgeFreq;                              /* RW; Range: [0, 4095]; Edge frequency adjustment. Edge will be narrower and thiner when it increase*/
    HI_U8  u8OverShoot;                              /* RW; Range: [0, 127]; u8OvershootAmt*/
    HI_U8  u8UnderShoot;                             /* RW; Range: [0, 127]; u8UndershootAmt*/
    HI_U8  u8ShootSupStr;                            /* RW; Range: [0, 255]; overshoot and undershoot suppression strength, the amplitude and width of shoot will be decrease when shootSupSt increase*/
    HI_U8  u8DetailCtrl;                             /* RW; Range: [0, 255]; Different sharpen strength for detail and edge. When it is bigger than 128, detail sharpen strength will be stronger than edge. */
} VPSS_GRP_SHARPEN_MANUAL_ATTR_S;

typedef struct hiVPSS_GRP_SHARPEN_AUTO_ATTR_S
{
    HI_U16 au16TextureStr[VPSS_SHARPEN_GAIN_NUM][VPSS_AUTO_ISO_STRENGTH_NUM]; /* RW; Range: [0, 4095]; Undirectional sharpen strength for texture and detail enhancement*/
    HI_U16 au16EdgeStr[VPSS_SHARPEN_GAIN_NUM][VPSS_AUTO_ISO_STRENGTH_NUM];    /* RW; Range: [0, 4095]; Directional sharpen strength for edge enhancement*/
    HI_U16 au16TextureFreq[VPSS_AUTO_ISO_STRENGTH_NUM];                       /* RW; Range: [0, 4095]; Texture frequency adjustment. Texture and detail will be finer when it increase*/
    HI_U16 au16EdgeFreq[VPSS_AUTO_ISO_STRENGTH_NUM];                          /* RW; Range: [0, 4095]; Edge frequency adjustment. Edge will be narrower and thiner when it increase*/
    HI_U8  au8OverShoot[VPSS_AUTO_ISO_STRENGTH_NUM];                          /* RW; Range: [0, 127]; u8OvershootAmt*/
    HI_U8  au8UnderShoot[VPSS_AUTO_ISO_STRENGTH_NUM];                         /* RW; Range: [0, 127]; u8UndershootAmt*/
    HI_U8  au8ShootSupStr[VPSS_AUTO_ISO_STRENGTH_NUM];                        /* RW; Range: [0, 255]; overshoot and undershoot suppression strength, the amplitude and width of shoot will be decrease when shootSupSt increase*/
    HI_U8  au8DetailCtrl[VPSS_AUTO_ISO_STRENGTH_NUM];                         /* RW; Range: [0, 255]; Different sharpen strength for detail and edge. When it is bigger than 128, detail sharpen strength will be stronger than edge. */
} VPSS_GRP_SHARPEN_AUTO_ATTR_S;


typedef struct hiVPSS_GRP_SHARPEN_ATTR_S
{
    HI_BOOL                         bEnable;                          /* RW; Sharpen enable. */
    OPERATION_MODE_E                enOpType;                         /* RW; Sharpen Operation mode. */
    HI_U8                           au8LumaWgt[VPSS_YUV_SHPLUMA_NUM]; /* RW; Range: [0, 127]; sharpen weight based on loacal luma*/
    VPSS_GRP_SHARPEN_MANUAL_ATTR_S  stSharpenManualAttr;              /* RW; Sharpen manual attribute*/
    VPSS_GRP_SHARPEN_AUTO_ATTR_S    stSharpenAutoAttr;                /* RW; Sharpen auto attribute*/
} VPSS_GRP_SHARPEN_ATTR_S;

/****************************VPSS 3DNR********************/

/* 3DNR X interface for Hi3519AV100 */
typedef struct
{
    HI_U8  IES0, IES1, IES2, IES3;
    HI_U16 IEDZ : 10,  _rb_ : 6;
} tV56aIEy;

typedef struct
{
    HI_U8  SPN6 : 3, SFR  : 5;
    HI_U8  SBN6 : 3, PBR6 : 5;

    HI_U8  SFS2, SFT2, SBR2;
    HI_U8  SFS4, SFT4, SBR4;

    HI_U16 STH1 : 9,  SFN1 : 3, SFN0  : 3, NRyEn : 1;
    HI_U16 STH2 : 9,  SFN2 : 3, BWSF4 : 1, kMode : 3;
    HI_U16 STH3 : 9,  SFN3 : 3, tEdge : 2, TriTh : 1, _rb_  : 1;
} tV56aSFy;

typedef struct
{
    HI_U16 MADZ : 10, MAI0 : 2, MAI1 : 2,  MAI2 : 2;
    HI_U8  MADK,      MABR;

    HI_U16 MATH : 10, MATE : 4, MATW : 2;
    HI_U8  MASW :  4, MABW : 3, MAXN : 1, _rB_;
} tV56aMDy;

typedef struct
{
    HI_U16 TFS : 4,  TDZ : 10, TDX : 2;
    HI_U8  TFR[5],   TSS : 4,  TSI : 1, _rb_ : 2;
    HI_U16 SDZ : 10, STR : 5,  bRef : 1;
} tV56aTFy;

typedef struct
{
    HI_U8  SFC, _rb_ : 2, TFC : 6;

    HI_U16 CSFS : 10,    CSFR : 6;
    HI_U16 CTFS :  4,    CIIR : 1;
    HI_U16 CTFR : 11;
} tV56aNRc;

typedef struct
{
    tV56aIEy IEy[2];
    tV56aSFy SFy[4];
    tV56aMDy MDy[2];
    tV56aTFy TFy[2];
    tV56aNRc NRc;

    HI_U16 SBSk2[32], SDSk2[32];
    HI_U16 SBSk3[32], SDSk3[32];
} VPSS_NRX_PARAM_V1_S;

typedef struct
{
    HI_U8  IES0, IES1, IES2, IES3;
    HI_U16 IEDZ : 10, IEEn : 1, _rb_ : 5;
} tV500_VPSS_IEy;

typedef struct
{
    HI_U8  SPN3 : 3 ;
    HI_U8  SBN3 : 3, PBR3 : 5, JMODE;
    HI_U8  SFR;
    HI_U8  JSFR[3];

    HI_U8  SFS1,  SFT1,  SBR1;
    HI_U8  SFS2,  SFT2,  SBR2;
    HI_U8  SFS4,  SFT4,  SBR4;
    HI_U8  CPRat, CPMode;
    HI_U8  BRT0,  BRT1;

    HI_U16 STH1 : 9,  SFN1 : 3, SFN0 : 3, NRyEn : 1;
    HI_U16 STH2 : 9,  SFN2 : 3, kWnd : 1, kMode : 3;
    HI_U16 STH3 : 9,  SFN3 : 3, _rb_ : 3, TriTh : 1;

    HI_U16 STHd1 : 9;
    HI_U16 STHd2 : 9;
    HI_U16 STHd3 : 9;
} tV500_VPSS_SFy;

typedef struct
{
    /* PATH0 */
    HI_U16 MADZ0   : 9,  MAI00   : 2, MAI01 : 2,  MAI02 : 2;
    HI_U16 MADZd0  : 9;
    HI_U8  MABR0   : 8;
    HI_U8  MABRd0  : 8;

    HI_U16 MATH0   : 10,  MATE0   : 4,  MATW0  : 2;
    HI_U8  MASW0   :  4,  MAXN    : 1, _rB0_;
    HI_U16 MATHd0  : 10,  MABW0   : 4;

    HI_S32 FWTDZ0;
    HI_U8  FWTSLP0 : 4;
    HI_U8  bSDF00  : 1,  SDFSLP00 : 3,  bSDF10 : 1,  SDFSLP10 : 3;
    HI_U8  RFWUTH0 : 4,   RMODE0  : 2;

    /* PATH1 */
    HI_U16 MADZ1   : 9,  MAI10    : 2,  MAI11 : 2, MAI12 : 2;
    HI_U16 MADZd1  : 9;
    HI_U8  MABR1   :  8;
    HI_U8  MABRd1  :  8;

    HI_U16 MATH1   : 10,  MATE1    : 4,  MATW1 : 2;
    HI_U8  MASW1   :  4,  MABW1    : 4;
    HI_U16 MATHd1  : 10;

    HI_S32 FWTDZ1;
    HI_U8  FWTSLP1 : 4;
    HI_U8  bSDF01  : 1,   SDFSLP01 : 3,  bSDF11 : 1,  SDFSLP11 : 3;
    HI_U8  RMODE1  : 2,   RFWUTH1  : 4;

    /* CONTROL */
    HI_U8  FMODE  : 2,    WMODE  : 1;
    HI_U16 SADDZ  : 12,   SADSLP : 4;
} tV500_VPSS_MDy;

typedef struct
{
    HI_U16 TFS0  : 4,  TDZ0   : 10, TDX0    : 2;
    HI_U8  TFR0[7],    TSS0   : 4,  TSI0    : 4;
    HI_U16 SDZ0  : 10, STR0   : 5;
    HI_U8  RFI0  : 3,  HGI0   : 3,  DZMode0 : 1;

    HI_U16 TFS1  : 4,  TDZ1   : 10, TDX1    : 2;
    HI_U8  TFR1[7],    TSS1   : 4,  TSI1    : 4;
    HI_U16 SDZ1  : 10, STR1   : 5;
    HI_U8  RFI1  : 3,  HGI1   : 3,  DZMode1 : 1;

    HI_U8  TED   : 2,   TEDI  : 3,    bRfr  : 1,  bRef    : 1,  _rb_1;
} tV500_VPSS_TFy;

typedef struct
{
    HI_U8 SMODE  : 1,  bRFU   : 1,  RFUI   : 3;

    HI_U16 SDTH   : 12, SDSLP  : 5;
    HI_U16 RFDZ0  : 11, RFSLP0 : 5;
    HI_U16 RFDZ1  : 11, RFSLP1 : 5;
    HI_U16 RFDZ2  : 11, RFSLP2 : 5;

    HI_U8 RFUTH0 : 5,  RFHI0  : 2;
    HI_U8 RFUTH1 : 5,  RFHI1  : 2,  RFHI2  : 2;
} tV500_VPSS_RFs;

typedef struct
{
    HI_U8  SFC, _rb_ : 2, TFC : 6;

    HI_U8  CSFR : 6;
    HI_U16 CTFS :  4,    CIIR : 1,   CTPW : 4;
    HI_U16 CTFR : 11;
} tV500_VPSS_NRc;

typedef struct
{
    tV500_VPSS_IEy  IEy[4];
    tV500_VPSS_SFy  SFy[4];
    tV500_VPSS_MDy  MDy[2];
    tV500_VPSS_RFs  RFs;
    tV500_VPSS_TFy  TFy[2];
    tV500_VPSS_NRc  NRc;

    HI_U8  SBSk2[32], SDSk2[32];
    HI_U8  SBSk3[32], SDSk3[32];
} VPSS_NRX_V2_S;

typedef struct hiVPSS_NRX_PARAM_MANUAL_V2_S
{
    VPSS_NRX_V2_S stNRXParam;
} VPSS_NRX_PARAM_MANUAL_V2_S;

typedef struct hiVPSS_NRX_PARAM_AUTO_V2_S
{
    HI_U32 u32ParamNum;
    HI_U32* pau32ISO;
    VPSS_NRX_V2_S* pastNRXParam;
} VPSS_NRX_PARAM_AUTO_V2_S;

typedef struct hiVPSS_NRX_PARAM_V2_S
{
    OPERATION_MODE_E           enOptMode;           /* RW;Adaptive NR */
    VPSS_NRX_PARAM_MANUAL_V2_S stNRXManual;         /* RW;NRX V2 param for manual video */
    VPSS_NRX_PARAM_AUTO_V2_S   stNRXAuto;           /* RW;NRX V2 param for auto video */
} VPSS_NRX_PARAM_V2_S;

/* 3DNR interface */
typedef enum hiVPSS_NR_VER_E
{
    VPSS_NR_V1 = 1,
    VPSS_NR_V2 = 2,
    VPSS_NR_V3 = 3,
    VPSS_NR_V4 = 4,
    VPSS_NR_BUTT
}VPSS_NR_VER_E;

typedef struct hiVPSS_GRP_NRX_PARAM_S
{
    VPSS_NR_VER_E enNRVer;
    union
    {
        VPSS_NRX_PARAM_V1_S stNRXParam_V1;   /* interface X V1 for Hi3519AV100 */
        VPSS_NRX_PARAM_V2_S stNRXParam_V2;
    };

}VPSS_GRP_NRX_PARAM_S;


typedef struct hiVPSS_PARAM_MOD_S
{
    HI_BOOL bOneBufForLowDelay;
    HI_U32  u32VpssVbSource;
}VPSS_MOD_PARAM_S;


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */
#endif /* __HI_COMM_VPSS_H__ */

