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

#define ISP_AUTO_ISO_STENGTH_NUM  (16)
typedef struct hiISP_CMOS_BLACK_LEVEL_S
{
    HI_BOOL bUpdate;
    
    HI_U16  au16BlackLevel[ISP_BAYER_CHN_NUM];
} ISP_CMOS_BLACK_LEVEL_S;

typedef struct hiISP_CMOS_AGC_TABLE_S
{
    HI_BOOL bValid;
    
    HI_U8   au8SnrThresh[ISP_AUTO_STENGTH_NUM];            /* adjust denoise strength, different iso with different strength */    
    HI_U8   au8DemosaicNpOffset[ISP_AUTO_STENGTH_NUM];
    HI_U8   au8GeStrength[ISP_AUTO_STENGTH_NUM];

} ISP_CMOS_AGC_TABLE_S;
	
typedef struct hiISP_CMOS_BAYER_SHARPEN_S
{
    HI_BOOL bValid;    
    HI_BOOL bShpAlgSel;                                      /* RW,  Set enShpAlgSel = 1 is Demosaic SharpenEx, else Demosaic sharpen. */		
    /* Demosaic Sharpen */
    HI_U8 au8SharpenAltD[ISP_AUTO_STENGTH_NUM];          /* adjust image edge,different iso with different sharp strength */
    HI_U8 au8SharpenAltUd[ISP_AUTO_STENGTH_NUM];         /* adjust image texture, different iso with different strength */
    HI_U8 au8DemosaicLumThresh[ISP_AUTO_STENGTH_NUM];    /* RW,  Range:[0, 0xFF]. */	
    /* Demosaic SharpenEx */
    HI_U8 au8SharpenHF[ISP_AUTO_STENGTH_NUM];			 /* RW,  Range:[0, 0xFF]. Strength parameter for high frequency components.*/
    HI_U8 au8SharpenMF[ISP_AUTO_STENGTH_NUM];			 /* RW,  Range:[0, 0xFF]. Strength parameter for medium frequency components.*/
    HI_U8 au8SharpenLF[ISP_AUTO_STENGTH_NUM];			 /* RW,  Range:[0, 0xFF]. Strength parameter for low frequency components.*/
    HI_U8 au8SadAmplifier[ISP_AUTO_STENGTH_NUM];		 /* RW,  Range:[0, 0xFF]. Amplifier for sad.*/
}ISP_CMOS_BAYER_SHARPEN_S;

typedef struct hiISP_CMOS_YUV_SHARPEN_S
{
    HI_BOOL bValid;    
    
    HI_BOOL abEnLowLumaShoot[ISP_AUTO_STENGTH_NUM];
    HI_U8 au8TextureSt[ISP_AUTO_STENGTH_NUM];  		        /* RW,  Sharpen strength of Edge, range [0, 0xFF], equal to maxEdgeAmt in fw. */
    HI_U8 au8EdgeSt[ISP_AUTO_STENGTH_NUM];  		        /* RW,  Sharpen strength of detail, range [0, 0xFF], equal to maxSharpAmt1 in fw */ 
    HI_U8 au8OverShoot[ISP_AUTO_STENGTH_NUM];		        /* RW,  Range:[0, 0xFF]. */	
    HI_U8 au8UnderShoot[ISP_AUTO_STENGTH_NUM];		        /* RW,  Range:[0, 0xFF]. */	
    HI_U8 au8TextureThd[ISP_AUTO_STENGTH_NUM];		        /* RW,  Range:[0, 0xFF]. */	
    HI_U8 au8EdgeThd[ISP_AUTO_STENGTH_NUM];	 		        /* RW,  Range:[0, 0xFF]. */
	HI_U8 au8JagCtrl[ISP_AUTO_STENGTH_NUM];		 	        /* RW,  Range:[0, 0x3F]. set to '0' is off */	
	HI_U8 au8SaltCtrl[ISP_AUTO_STENGTH_NUM];		        /* RW,  Range:[0, 0xFF]. */
	HI_U8 au8PepperCtrl[ISP_AUTO_STENGTH_NUM];		        /* RW,  Range:[0, 0xFF]. */
	HI_U8 au8DetailCtrl[ISP_AUTO_STENGTH_NUM];		        /* RW,  Range:[0, 0xFF]. */
	
	HI_U8 au8LumaThd[ISP_YUV_SHPLUMA_NUM][ISP_AUTO_STENGTH_NUM];  	 /* RW,  Range:[0, 0xFF]. luma threshold */	
	HI_U8 au8LumaWgt[ISP_YUV_SHPLUMA_NUM][ISP_AUTO_STENGTH_NUM]; 	 /* RW,  Range:[0, 0xFF]. luma weight */		
} ISP_CMOS_YUV_SHARPEN_S;

typedef struct hiISP_CMOS_NOISE_TABLE_S
{
    HI_BOOL bValid;
    
    HI_U8   au8NoiseProfileWeightLut[NP_NODE_NUM_MAX];
    HI_U8   au8DemosaicWeightLut[NP_NODE_NUM_MAX];
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
    HI_BOOL bFilterMux;
    HI_U32  u32Svariance;
    HI_U32  u32BrightPr;
    HI_U32  u32Contrast;
    HI_U32  u32DarkEnhance;
} ISP_CMOS_DRC_S;

#define GAMMA_FE0_LUT_SIZE 33
#define GAMMA_FE1_LUT_SIZE 257
typedef struct hiISP_CMOS_GAMMAFE_S
{
    HI_BOOL bValid;         /* wdr sensor should set */

    HI_U32  au32Gammafe0[GAMMA_FE0_NODE_NUM];    
    HI_U32  au32Gammafe1[GAMMA_FE1_NODE_NUM];
} ISP_CMOS_GAMMAFE_S;


typedef struct hiISP_CMOS_INPUT_FORMATTER_S
{
    HI_BOOL bValid;         /* sensor built_in WDR should be set */

    HI_U16 au16KneePoint[IF_KNEE_POINT_NUM];
    HI_U8  au8SlopeSelect[IF_SLOPE_NUM];
} ISP_CMOS_INPUT_FORMATTER_S;

#define CMOS_RADIAL_SHADING_TABLE_NODE_NUMBER_MAX (129)
typedef struct hiISP_CMOS_RADIAL_SHADING_S
{
    HI_BOOL bValid;
    
    HI_U16 u16RCenterX;
    HI_U16 u16RCenterY;
    HI_U16 u16GCenterX;
    HI_U16 u16GCenterY;
    HI_U16 u16BCenterX;
    HI_U16 u16BCenterY;

    HI_U16 au16RShadingTbl[SHADING_NODE_NUM_MAX];
    HI_U16 au16GShadingTbl[SHADING_NODE_NUM_MAX];
    HI_U16 au16BShadingTbl[SHADING_NODE_NUM_MAX];

    HI_U16 u16ROffCenter;
    HI_U16 u16GOffCenter;
    HI_U16 u16BOffCenter;

    HI_U16 u16TblNodeNum;
} ISP_CMOS_RADIAL_SHADING_S;

#define CMOS_MESH_SHADING_TABLE_NODE_NUMBER_MAX (64*64)
#define CMOS_MESH_SHADING_PLANE_NUM             (3)
#define CMOS_MESH_LIGHT_SOURCE_NUM_MAX          (4)
typedef struct hiISP_CMOS_MESH_SHADING_S
{
    HI_BOOL bValid;

    HI_U32 au32MeshColorTemp[ISP_MESH_LS_NUM_MAX];
    HI_U8 u8MeshLSMode;
    HI_U8 u8MeshScale;
    HI_U8 au8MeshTable[ISP_MESH_SHADING_PLANE_NUM][ISP_MESH_SHADING_NODE_NUM];
    HI_U16 u16MeshStrength;
} ISP_CMOS_MESH_SHADING_S;

#define GAMMA_NODE_NUMBER   257
typedef struct hiISP_CMOS_GAMMA_S
{
    HI_BOOL bValid;
    
    HI_U16  au16Gamma[GAMMA_NODE_NUM];
} ISP_CMOS_GAMMA_S;

typedef struct hiISP_CMOS_SENSOR_MAX_RESOLUTION_S
{
    HI_U32  u32MaxWidth;
    HI_U32  u32MaxHeight;
}ISP_CMOS_SENSOR_MAX_RESOLUTION_S;

typedef struct hiISP_CMOS_WDR_ATTR_S
{
    HI_U32  au32ExpRatio[3];
}ISP_CMOS_WDR_ATTR_S;

typedef struct hiISP_CMOS_SENSOR_MODE_S
{
    HI_U32  u32SensorID;
    HI_U8   u8SensorMode;
}ISP_CMOS_SENSOR_MODE_S;

typedef struct hiISP_CMOS_DEFAULT_S
{    
    ISP_CMOS_DRC_S           stDrc;
    ISP_CMOS_AGC_TABLE_S     stAgcTbl;
    ISP_CMOS_NOISE_TABLE_S   stNoiseTbl;
    ISP_CMOS_DEMOSAIC_S      stDemosaic;
    ISP_CMOS_GAMMAFE_S       stGammafe;
    ISP_CMOS_GAMMA_S         stGamma;
    ISP_CMOS_RADIAL_SHADING_S   stRadialShading;
	ISP_CMOS_MESH_SHADING_S     stMeshShading; 
    ISP_CMOS_BAYER_SHARPEN_S stBayerSharpen;
    ISP_CMOS_YUV_SHARPEN_S   stYuvSharpen;
    ISP_CMOS_SENSOR_MAX_RESOLUTION_S stSensorMaxResolution;
    ISP_CMOS_INPUT_FORMATTER_S    stInputFormatter;
    ISP_CMOS_WDR_ATTR_S    stWDRAttr;
    ISP_CMOS_SENSOR_MODE_S stSensorMode;
} ISP_CMOS_DEFAULT_S;

typedef struct hiISP_CMOS_SENSOR_IMAGE_MODE_S
{
    HI_U16   u16Width;
    HI_U16   u16Height;
    HI_FLOAT f32Fps;
}ISP_CMOS_SENSOR_IMAGE_MODE_S;

typedef struct hiISP_SENSOR_EXP_FUNC_S
{
    HI_VOID(*pfn_cmos_sensor_init)(ISP_DEV IspDev);
    HI_VOID(*pfn_cmos_sensor_exit)(ISP_DEV IspDev);
    HI_VOID(*pfn_cmos_sensor_global_init)(ISP_DEV IspDev);
    HI_S32(*pfn_cmos_set_image_mode)(ISP_DEV IspDev, ISP_CMOS_SENSOR_IMAGE_MODE_S *pstSensorImageMode);
    HI_VOID(*pfn_cmos_set_wdr_mode)(ISP_DEV IspDev, HI_U8 u8Mode);
    
    /* the algs get data which is associated with sensor, except 3a */
    HI_U32(*pfn_cmos_get_isp_default)(ISP_DEV IspDev, ISP_CMOS_DEFAULT_S *pstDef);
    HI_U32(*pfn_cmos_get_isp_black_level)(ISP_DEV IspDev, ISP_CMOS_BLACK_LEVEL_S *pstBlackLevel);
    HI_U32(*pfn_cmos_get_sns_reg_info)(ISP_DEV IspDev, ISP_SNS_REGS_INFO_S *pstSnsRegsInfo);

    /* the function of sensor set pixel detect */
    HI_VOID(*pfn_cmos_set_pixel_detect)(ISP_DEV IspDev, HI_BOOL bEnable);
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

