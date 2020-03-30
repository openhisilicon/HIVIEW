
/******************************************************************************
Copyright (C), 2016-2018, Hisilicon Tech. Co., Ltd.
******************************************************************************
File Name     : hi_comm_hdr.h
Version       : Initial Draft
Author        : Hisilicon multimedia software group
Created       : 2016/09/27
Last Modified :
Description   :
Function List :
******************************************************************************/

#ifndef __HI_COMM_HDR_H__
#define __HI_COMM_HDR_H__


#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

#include "hi_type.h"
#include "hi_common.h"
#include "hi_comm_video.h"

#define SGAMMA_NODE_NUM 200
#define HDR_GAMMA_NODE_NUM 1025
#define HDR_CSC_IDC_NUM 3
#define HDR_CSC_ODC_NUM 3
#define HDR_CSC_COEF_NUM 9

#define HI_ERR_HDR_NULL_PTR        HI_DEF_ERR(HI_ID_HDR, EN_ERR_LEVEL_ERROR, EN_ERR_NULL_PTR)
#define HI_ERR_HDR_NOTREADY        HI_DEF_ERR(HI_ID_HDR, EN_ERR_LEVEL_ERROR, EN_ERR_SYS_NOTREADY)
#define HI_ERR_HDR_INVALID_DEVID   HI_DEF_ERR(HI_ID_HDR, EN_ERR_LEVEL_ERROR, EN_ERR_INVALID_DEVID)
#define HI_ERR_HDR_INVALID_CHNID   HI_DEF_ERR(HI_ID_HDR, EN_ERR_LEVEL_ERROR, EN_ERR_INVALID_CHNID)
#define HI_ERR_HDR_INVALID_PIPEID  HI_DEF_ERR(HI_ID_HDR, EN_ERR_LEVEL_ERROR, EN_ERR_INVALID_PIPEID)
#define HI_ERR_HDR_EXIST           HI_DEF_ERR(HI_ID_HDR, EN_ERR_LEVEL_ERROR, EN_ERR_EXIST)
#define HI_ERR_HDR_UNEXIST         HI_DEF_ERR(HI_ID_HDR, EN_ERR_LEVEL_ERROR, EN_ERR_UNEXIST)
#define HI_ERR_HDR_NOT_SUPPORT     HI_DEF_ERR(HI_ID_HDR, EN_ERR_LEVEL_ERROR, EN_ERR_NOT_SUPPORT)
#define HI_ERR_HDR_NOT_PERM        HI_DEF_ERR(HI_ID_HDR, EN_ERR_LEVEL_ERROR, EN_ERR_NOT_PERM)
#define HI_ERR_HDR_NOMEM           HI_DEF_ERR(HI_ID_HDR, EN_ERR_LEVEL_ERROR, EN_ERR_NOMEM)
#define HI_ERR_HDR_NOBUF           HI_DEF_ERR(HI_ID_HDR, EN_ERR_LEVEL_ERROR, EN_ERR_NOBUF)
#define HI_ERR_HDR_ILLEGAL_PARAM   HI_DEF_ERR(HI_ID_HDR, EN_ERR_LEVEL_ERROR, EN_ERR_ILLEGAL_PARAM)
#define HI_ERR_HDR_BUSY            HI_DEF_ERR(HI_ID_HDR, EN_ERR_LEVEL_ERROR, EN_ERR_BUSY)
#define HI_ERR_HDR_BUF_EMPTY       HI_DEF_ERR(HI_ID_HDR, EN_ERR_LEVEL_ERROR, EN_ERR_BUF_EMPTY)


/********************** CLUT **************************************/
typedef struct hiHDR_CLUT_PARAM_S
{
     HI_BOOL         bEnable         ; /* RW; Range:[0, 1];Format:1.0; Enable/Disable CLUT Function*/
     HI_U32          u32GainR        ; /* RW; Range:[0,4095];Format:12.0;*/
     HI_U32          u32GainG        ; /* RW; Range:[0,4095];Format:12.0;*/
     HI_U32          u32GainB        ; /* RW; Range:[0,4095];Format:12.0;*/
}HDR_CLUT_PARAM_S;

/*CLUT LUT*/
typedef struct hiHDR_CLUT_LUT_S
{
     HI_S16 as16R[CLUT_R_NUM][CLUT_B_NUM][CLUT_G_NUM];   /* RW; Range:[-255,255];Format:9.0;*/
     HI_S16 as16G[CLUT_R_NUM][CLUT_B_NUM][CLUT_G_NUM];   /* RW; Range:[-255,255];Format:9.0;*/
     HI_S16 as16B[CLUT_R_NUM][CLUT_B_NUM][CLUT_G_NUM];   /* RW; Range:[-255,255];Format:9.0;*/
}HDR_CLUT_LUT_S;

/*****************************************************************/
typedef enum hiHDR_OETF_CURVE_TYPE_E
{
    HDR_OETF_CURVE_DEFAULT = 0,     /*Default curve for OETF*/
    HDR_OETF_CURVE_SRGB,            /*Default sRGB curve for OETF*/
    HDR_OETF_CURVE_HDR10,           /*Default PQ(HDR10) curve for OETF*/
    HDR_OETF_CURVE_HLG,             /*Default HLG curve for OETF*/
    HDR_OETF_CURVE_SLF,             /*Default SLF curve for OETF*/
    HDR_OETF_CURVE_USER,            /*Curve defined by user */
    HDR_OETF_CURVE_BUTT             /*End of enumerate*/
}HDR_OETF_CURVE_TYPE_E;

typedef struct hiHDR_OETF_PARAM_S
{
    HI_BOOL bEnable;
    HI_U16  au16GammaLut[HDR_GAMMA_NODE_NUM]; /*RW; Range: [0, 4095];Format:12.0; OETF LUT nodes value*/

    HI_U32  u32MaxLum;                   /*RW; Range: [0, 10000];Format:14.0; Max Luminance level of the PQ curve*/
    HI_U32  u32CurLum;                   /*RW; Range: [0, 10000]; Format:14.0;Current Luminance level of the PQ curve*/

    HDR_OETF_CURVE_TYPE_E  enCurveType;  /*RW; Range: [0, 5]; Format:ENUM;OETF curve type*/
}HDR_OETF_PARAM_S;

typedef struct hiHDR_TM_CUBIC_ATTR_S
{
   HI_U16 u16X;      /*RW; Range:[0, 1000]; Format:10.0;Position on the X-axis*/
   HI_U16 u16Y;      /*RW; Range:[0, 1000]; Format:10.0;Position on the Y-axis*/
   HI_U16 u16Slope;  /*RW; Range:[0, 10000];Format:14.0;Gradiant of the point*/

} HDR_TM_CUBIC_ATTR_S;

typedef struct hiHDR_TM_PARAM_S
{
   HI_BOOL bEnable;                      /* RW; Range:[0, 1];Format:1.0; Enable/Disable TM Function*/
   HDR_TM_CUBIC_ATTR_S astCubicPoint[5]; /* RW; Points parameters used to draw TM cubic curve*/

} HDR_TM_PARAM_S;

/********************** CSC ******************************/
typedef struct hiHDR_CSC_MATRIX_S
{
    HI_S32 as32CSCIdc[HDR_CSC_IDC_NUM]; /*RW; Range:[-1024, 1023]; Format: S11.0; Input dc component for csc matrix*/
    HI_S32 as32CSCOdc[HDR_CSC_ODC_NUM]; /*RW; Range:[-1024, 1023]; Format: S11.0; Output dc component for csc matrix*/
    HI_S32 as32CSCCoef[HDR_CSC_COEF_NUM]; /*RW; Range:[-32768, 32767]; Format: S5.10; 3x3 coefficients for csc matrix*/
} HDR_CSC_MATRIX_S;

typedef struct hiHDR_CSC_PARAM_S
{
    COLOR_GAMUT_E enHDRCscType; /*RW; Range: [0, 4]; Format:ENUM; Color gamut type*/
    HI_U32  u32LumaVal;  /* RW; Range:[0, 100]; Format: U32; Luminance value*/
    HI_U32  u32ContrVal; /* RW; Range:[0, 100]; Format: U32; Contrast value*/
    HI_U32  u32HueVal;   /* RW; Range:[0, 100]; Format: U32; Hue value*/
    HI_U32  u32SatuVal;  /* RW; Range:[0, 100]; Format: U32; Saturation value*/
    HI_BOOL bLimitedRangeEn; /*RW; Range: [0x0, 0x1]; Format: Boolean; Enable/Disable: Enable Limited range output mode(default full range output) */
    HI_BOOL bExtCscEn; /*RW; Range: [0x0, 0x1]; Format: Boolean; Enable/Disable: Enable extended luma range*/
    HI_BOOL bCtModeEn; /*RW; Range: [0x0, 0x1]; Format: Boolean; Enable/Disable: Enable ct mode*/
    HDR_CSC_MATRIX_S stCSCMatrix; /*RW; Range: NA; Format: struct; Color Space Conversion matrix*/
}HDR_CSC_PARAM_S;

/********************** CCM ******************************/
typedef struct hiHDR_CCM_MANUAL_S
{
    HI_U16 au16CCM[CCM_MATRIX_SIZE];            /*RW;  Range: [0x0,  0xFFFF]; Format:8.8(15th bit is sign bit,0 represents negative,1 represents positive); Manul CCM matrix,*/
} HDR_CCM_MANUAL_S;

typedef struct hiHDR_CCM_TAB_S
{
    HI_U16 u16ColorTemp;                        /*RW;  Range:[500 ,30000]; Format:16.0; the current color temperature */
    HI_U16 au16CCM[CCM_MATRIX_SIZE];            /*RW;  Range: [0x0,  0xFFFF]; Format:8.8(15th bit is sign bit,0 represents negative,1 represents positive); CCM matrixes for different color temperature*/
} HDR_CCM_TAB_S;

typedef struct hiHDR_CCM_AUTO_S
{
    HI_U16  u16CCMTabNum;                       /*RW;  Range: [0x3,  0x7]; Format:16.0; The number of CCM matrixes*/
    HDR_CCM_TAB_S astCCMTab[CCM_MATRIX_NUM];    /*RW;  Range: NA; Format: struct; Color temp and matrix*/
} HDR_CCM_AUTO_S;

typedef struct hiHDR_CCM_PARAM_S
{
    OPERATION_MODE_E enOperationMode;           /*RW;  Range:[0, 1];Format:1.0; 0:Auto CCM, 1:Manual CCM*/
    HDR_CCM_MANUAL_S stManual;                  /*RW;  Range: NA; Format: struct; Manual Color matrix*/
    HDR_CCM_AUTO_S stAuto;                      /*RW;  Range: NA; Format: struct; Auto Color temp and matrix*/
} HDR_CCM_PARAM_S;






#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */
#endif /* __HI_COMM_HDR_H__ */

