/******************************************************************************

  Copyright (C), 2001-2011, Hisilicon Tech. Co., Ltd.

 ******************************************************************************
  File Name     : hi_comm_sns.h
  Version       : Initial Draft
  Author        : Hisilicon multimedia software group
  Created       : 2011/01/05
  Description   : 
  History       :
  1.Date        : 2011/01/05
    Author      : x00100808
    Modification: Created file

******************************************************************************/

#ifndef __HI_COMM_SNS_H__
#define __HI_COMM_SNS_H__

#include "hi_type.h"
#include "hi_common.h"
#include "hi_comm_isp.h"

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* End of #ifdef __cplusplus */

typedef struct hiISP_CMOS_BLACK_LEVEL_S
{
    HI_BOOL bUpdate;
    
    HI_U16  au16BlackLevel[4];
} ISP_CMOS_BLACK_LEVEL_S;

typedef struct hiISP_CMOS_AGC_TABLE_S
{
    HI_BOOL bValid;
    
    HI_U8   au8SharpenAltD[16];          /* adjust image edge,different iso with different sharp strength */
    HI_U8   au8SharpenAltUd[16];         /* adjust image texture, different iso with different strength */
    HI_U8   au8SnrThresh[16];            /* adjust denoise strength, different iso with different strength */
    HI_U8   au8DemosaicLumThresh[16];  
    HI_U8   au8DemosaicNpOffset[16];
    HI_U8   au8GeStrength[16];
    HI_U8   au8SharpenRGB[16];          /* adjust image edge,different iso with different RGBSharp strength */
} ISP_CMOS_AGC_TABLE_S;

typedef struct hiISP_CMOS_NOISE_TABLE_S
{
    HI_BOOL bValid;
    
    HI_U8   au8NoiseProfileWeightLut[128];
    HI_U8   au8DemosaicWeightLut[128];
} ISP_CMOS_NOISE_TABLE_S;

typedef struct hiISP_CMOS_DEMOSAIC_S
{
    HI_BOOL bValid;
    
    HI_U8   u8VhSlope;
    HI_U8   u8AaSlope;
    HI_U8   u8VaSlope;
    HI_U8   u8UuSlope;
    HI_U8   u8SatSlope;
    HI_U8   u8AcSlope;
    HI_U8   u8FcSlope;
    HI_U16  u16VhThresh;
    HI_U16  u16AaThresh;
    HI_U16  u16VaThresh;
    HI_U16  u16UuThresh;
    HI_U16  u16SatThresh;
    HI_U16  u16AcThresh;
} ISP_CMOS_DEMOSAIC_S;

typedef struct hiISP_CMOS_RGBSHARPEN_S
{    
    HI_BOOL bValid;
    
    HI_U8   u8LutCore;         /* lut_core 8bit [0-255]*/
    HI_U8   u8LutStrength;     /* lut_strength 7bit [0-127]*/
    HI_U8   u8LutMagnitude;    /* lut_magnitude 5bit [0-31]*/    
} ISP_CMOS_RGBSHARPEN_S;

typedef struct hiISP_CMOS_DRC_S
{
    HI_BOOL bEnable;    
    HI_U32  u32BlackLevel;
    HI_U32  u32WhiteLevel;        
    HI_U32  u32SlopeMax;        
    HI_U32  u32SlopeMin;        
    HI_U32  u32VarianceSpace;
    HI_U32  u32VarianceIntensity;
    HI_U32  u32Asymmetry;
    HI_U32  u32BrightEnhance;
} ISP_CMOS_DRC_S;


#define GAMMA_FE0_LUT_SIZE 33
#define GAMMA_FE1_LUT_SIZE 257
typedef struct hiISP_CMOS_GAMMAFE_S
{
    HI_BOOL bValid;         /* wdr sensor should set */

    HI_U16  au16Gammafe0[GAMMA_FE0_LUT_SIZE];    
    HI_U16  au16Gammafe1[GAMMA_FE1_LUT_SIZE];
} ISP_CMOS_GAMMAFE_S;

#define CMOS_SHADING_TABLE_NODE_NUMBER_MAX (129)
typedef struct hiISP_CMOS_SHADING_S
{
    HI_BOOL bValid;
    
    HI_U16 u16RCenterX;
    HI_U16 u16RCenterY;
    HI_U16 u16GCenterX;
    HI_U16 u16GCenterY;
    HI_U16 u16BCenterX;
    HI_U16 u16BCenterY;

    HI_U16 au16RShadingTbl[CMOS_SHADING_TABLE_NODE_NUMBER_MAX];
    HI_U16 au16GShadingTbl[CMOS_SHADING_TABLE_NODE_NUMBER_MAX];
    HI_U16 au16BShadingTbl[CMOS_SHADING_TABLE_NODE_NUMBER_MAX];

    HI_U16 u16ROffCenter;
    HI_U16 u16GOffCenter;
    HI_U16 u16BOffCenter;

    HI_U16 u16TblNodeNum;
} ISP_CMOS_SHADING_S;

#define GAMMA_NODE_NUMBER   257
typedef struct hiISP_CMOS_GAMMA_S
{
    HI_BOOL bValid;
    
    HI_U16  au16Gamma[GAMMA_NODE_NUMBER];
} ISP_CMOS_GAMMA_S;

typedef struct hiISP_CMOS_SENSOR_MAX_RESOLUTION_S
{
    HI_U32  u32MaxWidth;
    HI_U32  u32MaxHeight;
}ISP_CMOS_SENSOR_MAX_RESOLUTION_S;

typedef struct hiISP_CMOS_DEFAULT_S
{    
    ISP_CMOS_DRC_S          stDrc;
    ISP_CMOS_AGC_TABLE_S    stAgcTbl;
    ISP_CMOS_NOISE_TABLE_S  stNoiseTbl;
    ISP_CMOS_DEMOSAIC_S     stDemosaic;
    ISP_CMOS_GAMMAFE_S      stGammafe;
    ISP_CMOS_GAMMA_S        stGamma;
    ISP_CMOS_SHADING_S      stShading; 
    ISP_CMOS_RGBSHARPEN_S   stRgbSharpen;

    ISP_CMOS_SENSOR_MAX_RESOLUTION_S stSensorMaxResolution;
} ISP_CMOS_DEFAULT_S;

typedef struct hiISP_CMOS_SENSOR_IMAGE_MODE_S
{
    HI_U16   u16Width;
    HI_U16   u16Height;
    HI_FLOAT f32Fps;
}ISP_CMOS_SENSOR_IMAGE_MODE_S;

typedef struct hiISP_SENSOR_EXP_FUNC_S
{
    HI_VOID(*pfn_cmos_sensor_init)(HI_VOID);
    HI_VOID(*pfn_cmos_sensor_exit)(HI_VOID);
    HI_VOID(*pfn_cmos_sensor_global_init)(HI_VOID);
    HI_S32(*pfn_cmos_set_image_mode)(ISP_CMOS_SENSOR_IMAGE_MODE_S *pstSensorImageMode);
    HI_VOID(*pfn_cmos_set_wdr_mode)(HI_U8 u8Mode);
    
    /* the algs get data which is associated with sensor, except 3a */
    HI_U32(*pfn_cmos_get_isp_default)(ISP_CMOS_DEFAULT_S *pstDef);
    HI_U32(*pfn_cmos_get_isp_black_level)(ISP_CMOS_BLACK_LEVEL_S *pstBlackLevel);
    HI_U32(*pfn_cmos_get_sns_reg_info)(ISP_SNS_REGS_INFO_S *pstSnsRegsInfo);

    /* the function of sensor set pixel detect */
    HI_VOID(*pfn_cmos_set_pixel_detect)(HI_BOOL bEnable);
} ISP_SENSOR_EXP_FUNC_S;

typedef struct hiISP_SENSOR_REGISTER_S
{
    ISP_SENSOR_EXP_FUNC_S stSnsExp;
} ISP_SENSOR_REGISTER_S;


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif /*__HI_COMM_SNS_H__ */

