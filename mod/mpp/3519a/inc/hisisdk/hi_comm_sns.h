/******************************************************************************

  Copyright (C), 2016, Hisilicon Tech. Co., Ltd.

 ******************************************************************************
  File Name     : hi_comm_sns.h
  Version       : Initial Draft
  Author        : Hisilicon multimedia software group
  Created       : 2011/01/05
  Description   :
  History       :
  1.Date        : 2011/01/05
    Author      :
    Modification: Created file

******************************************************************************/

#ifndef __HI_COMM_SNS_H__
#define __HI_COMM_SNS_H__

#include "hi_type.h"
#include "hi_common.h"
#include "hi_comm_isp.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */

typedef struct hiISP_CMOS_BLACK_LEVEL_S
{
    HI_BOOL bUpdate;
    HI_U16  au16BlackLevel[ISP_BAYER_CHN_NUM];
} ISP_CMOS_BLACK_LEVEL_S;

typedef struct hiISP_SNS_ATTR_INFO_S
{
    SENSOR_ID            eSensorId;
} ISP_SNS_ATTR_INFO_S;

#define ISP_SPLIT_POINT_NUM      (5)

typedef struct hiISP_CMOS_DEMOSAIC_S
{
    HI_BOOL bEnable;
    HI_U8   au8NonDirStr[ISP_AUTO_ISO_STRENGTH_NUM];
    HI_U8   au8NonDirMFDetailEhcStr[ISP_AUTO_ISO_STRENGTH_NUM];
    HI_U8   au8NonDirHFDetailEhcStr[ISP_AUTO_ISO_STRENGTH_NUM];
    HI_U8   au8DetailSmoothRange[ISP_AUTO_ISO_STRENGTH_NUM];
} ISP_CMOS_DEMOSAIC_S;

#define WDR_MAX_FRAME    (4)

typedef struct hiISP_CMOS_BAYERNR_S
{
    HI_BOOL  bEnable;
    HI_BOOL  bLowPowerEnable;
    HI_BOOL  bBnrMonoSensorEn;
    HI_BOOL  bNrLscEnable;
    HI_U8    u8NrLscRatio;
    HI_U8    au8LutFineStr[ISP_AUTO_ISO_STRENGTH_NUM];
    HI_U8    au8ChromaStr[ISP_BAYER_CHN_NUM][ISP_AUTO_ISO_STRENGTH_NUM];
    HI_U8    au8WDRFrameStr[WDR_MAX_FRAME_NUM];
    HI_U16   au16CoarseStr[ISP_BAYER_CHN_NUM][ISP_AUTO_ISO_STRENGTH_NUM];             //u10.0
    HI_U16   au16LutCoringWgt[ISP_AUTO_ISO_STRENGTH_NUM];
    HI_U16   au16LutCoringRatio[HI_ISP_BAYERNR_LUT_LENGTH];
} ISP_CMOS_BAYERNR_S;

typedef struct hiISP_CMOS_NOISE_CALIBRATION_S
{
    HI_U16   u16CalibrationLutNum;
    HI_FLOAT afCalibrationCoef[BAYER_CALIBTAION_MAX_NUM][3];
} ISP_CMOS_NOISE_CALIBRATION_S;

typedef struct hiISP_CMOS_SHARPEN_S
{
    HI_U8 u8SkinUmin;
    HI_U8 u8SkinVmin;
    HI_U8 u8SkinUmax;
    HI_U8 u8SkinVmax;
    ISP_SHARPEN_MANUAL_ATTR_S stManual;
    ISP_SHARPEN_AUTO_ATTR_S   stAuto;
} ISP_CMOS_SHARPEN_S;

typedef struct hiISP_CMOS_EDGEMARK_S
{
    HI_BOOL bEnable;               /*RW; Range:[0, 1]; Format:1.0;Enable/Disable Edge Mark*/
    HI_U8   u8Threshold;           /* RW; range: [0, 255];  Format:8.0;*/
    HI_U32  u32Color;              /* RW; range: [0, 0xFFFFFF];  Format:32.0;*/
} ISP_CMOS_EDGEMARK_S;

typedef struct hiISP_CMOS_HLC_S
{
    HI_BOOL bEnable;         /*RW; Range:[0, 1]; Format:1.0;Enable/Disable HLC module*/
    HI_U8   u8LumaThr;       /* RW; range: [0, 255];  Format:8.0;*/
    HI_U8   u8LumaTarget;    /* RW; range: [0, 255];  Format:8.0;*/
} ISP_CMOS_HLC_S;

typedef struct hiISP_CMOS_DRC_S
{
    HI_BOOL bEnable;

    HI_U16  u16ManualStrength;
    HI_U16  u16AutoStrength;

    HI_U8   u8SpatialFltCoef;
    HI_U8   u8RangeFltCoef;
    HI_U8   u8ContrastControl;
    HI_S8   s8DetailAdjustFactor;

    HI_U8   u8FltScaleFine;
    HI_U8   u8FltScaleCoarse;
    HI_U8   u8GradRevMax;
    HI_U8   u8GradRevThr;

    HI_U8   u8PDStrength;
    HI_U8   u8LocalMixingBrightMax;
    HI_U8   u8LocalMixingBrightMin;
    HI_U8   u8LocalMixingDarkMax;
    HI_U8   u8LocalMixingDarkMin;
    HI_U16  u16ColorCorrectionLut[33];

    HI_U8   u8Asymmetry;
    HI_U8   u8SecondPole;
    HI_U8   u8Stretch;
    HI_U8   u8Compress;

    HI_U8   u8CurveSel;

    HI_U16 au16Xpoint[5];
    HI_U16 au16Ypoint[5];
    HI_U16 au16Slope[5];
} ISP_CMOS_DRC_S;

typedef struct hiISP_CMOS_WDR_S
{
    HI_BOOL  bFusionMode;
    HI_BOOL  bMotionComp;

    HI_U16   u16ShortThr;
    HI_U16   u16LongThr;

    HI_BOOL  bForceLong;
    HI_U16   u16ForceLongLowThr;
    HI_U16   u16ForceLongHigThr;

    HI_BOOL  bShortExpoChk;
    HI_U16   u16ShortCheckThd;
    HI_BOOL  bMDRefFlicker;

    HI_U8    au8MdThrLowGain[ISP_AUTO_ISO_STRENGTH_NUM];
    HI_U8    au8MdThrHigGain[ISP_AUTO_ISO_STRENGTH_NUM];

    ISP_BNR_MODE_E  enBnrMode;
    HI_U16   au16FusionThr[WDR_MAX_FRAME];
    HI_U8    au8GsigmaGain[3];
    HI_U8    au8RBsigmaGain[3];
    HI_U8    u8MdtStillThd;
    HI_U8    u8MdtLongBlend;
    HI_U8    u8FullMdtSigGWgt;
    HI_U8    u8FullMdtSigRBWgt;
    HI_U8    au8NoiseFloor[NoiseSet_EleNum];
} ISP_CMOS_WDR_S;

typedef struct hiISP_CMOS_PREGAMMA_S
{
    HI_BOOL bEnable;
    HI_U32  au32PreGamma[PREGAMMA_NODE_NUM];
} ISP_CMOS_PREGAMMA_S;

#define GAMMA_NODE_NUMBER   1025 //Update NODE NUMBER
typedef struct hiISP_CMOS_GAMMA_S
{
    HI_U16  au16Gamma[GAMMA_NODE_NUMBER];
} ISP_CMOS_GAMMA_S;

typedef struct hiISP_CMOS_SENSOR_MAX_RESOLUTION_S
{
    HI_U32  u32MaxWidth;
    HI_U32  u32MaxHeight;
} ISP_CMOS_SENSOR_MAX_RESOLUTION_S;

typedef struct hiISP_CMOS_DPC_S
{
    HI_U16  au16Strength[ISP_AUTO_ISO_STRENGTH_NUM];
    HI_U16  au16BlendRatio[ISP_AUTO_ISO_STRENGTH_NUM];
} ISP_CMOS_DPC_S;

typedef struct hiISP_LSC_CABLI_TABLE_S
{
    HI_U16 au16R_Gain[HI_ISP_LSC_GRID_POINTS];
    HI_U16 au16Gr_Gain[HI_ISP_LSC_GRID_POINTS];
    HI_U16 au16Gb_Gain[HI_ISP_LSC_GRID_POINTS];
    HI_U16 au16B_Gain[HI_ISP_LSC_GRID_POINTS];
} ISP_LSC_CABLI_TABLE_S;

typedef struct hiISP_CMOS_LSC_S
{
    HI_U8   u8MeshScale;
    ISP_LSC_CABLI_TABLE_S astLscCalibTable[2];
} ISP_CMOS_LSC_S;

typedef struct hiISP_RLSC_CABLI_TABLE_S
{
    HI_U16 u16WBRGain;
    HI_U16 u16WBBGain;

    HI_U16 au16R_Gain[HI_ISP_RLSC_POINTS];
    HI_U16 au16Gr_Gain[HI_ISP_RLSC_POINTS];
    HI_U16 au16Gb_Gain[HI_ISP_RLSC_POINTS];
    HI_U16 au16B_Gain[HI_ISP_RLSC_POINTS];
} ISP_RLSC_CABLI_TABLE_S;

typedef struct hiISP_CMOS_RLSC_S
{
    HI_U8   u8Scale;

    HI_U16  u16CenterRX;
    HI_U16  u16CenterRY;
    HI_U16  u16CenterGrX;
    HI_U16  u16CenterGrY;
    HI_U16  u16CenterGbX;
    HI_U16  u16CenterGbY;
    HI_U16  u16CenterBX;
    HI_U16  u16CenterBY;

    HI_U16  u16OffCenterR;
    HI_U16  u16OffCenterGr;
    HI_U16  u16OffCenterGb;
    HI_U16  u16OffCenterB;

    ISP_RLSC_CABLI_TABLE_S stLscCalibTable[3];
} ISP_CMOS_RLSC_S;

typedef struct hiISP_CMOS_CA_S
{
    HI_BOOL   bEnable;
    HI_U32    au32YRatioLut[HI_ISP_CA_YRATIO_LUT_LENGTH];  //1.10bit  Y Ratio For UV ; Max = 2047 FW Limit
    HI_S16    as16ISORatio[ISP_AUTO_ISO_STRENGTH_NUM];     //1.10bit  ISO Ratio  For UV ,Max = 2047 FW Limi
    HI_BOOL   bCpEnable;
    HI_U32    au32YRatioLUTY[HI_ISP_CA_YRATIO_LUT_LENGTH];
    HI_U32    au32YRatioLUTU[HI_ISP_CA_YRATIO_LUT_LENGTH];
    HI_U32    au32YRatioLUTV[HI_ISP_CA_YRATIO_LUT_LENGTH];
} ISP_CMOS_CA_S;

typedef struct hiISP_CMOS_CLUT_S
{
    HI_BOOL bEnable;
    HI_U32  u32GainR;
    HI_U32  u32GainG;
    HI_U32  u32GainB;
    ISP_CLUT_LUT_S stClutLut;
} ISP_CMOS_CLUT_S;

typedef struct hiISP_CMOS_SPLIT_POINT_S
{
    HI_U8  u8X;                     /*RW;Range:[0x0,0x81];Format:8.0;The X point of the knee */
    HI_U16 u16Y;                    /*RW;Range:[0x0,0x8000];Format:16.0;The Y point of the  knee */
} ISP_CMOS_SPLIT_POINT_S;

typedef struct hiISP_CMOS_SPLIT_S
{
    HI_BOOL bEnable;             /*RW;Range:[0x0,0x1];Format:1.0;*/
    HI_U8   u8InputWidthSel;     /*RW;Range:[0x0,0x3];Format:2.0;Inputwidthselect: 0=12bit; 1=14bit; 2=16bit; */
    HI_U8   u8ModeIn;            /*RW;Range:[0x0,0x3];Format:2.0;ModeIn: 0=linear; 2=16LOG; 3=sensor-built-in*/
    HI_U8   u8ModeOut;           /*RW;Range:[0x0,0x3];Format:2.0;ModeOut: 0= 16bit when decompress; 1=2chn ; 2=3chn; */
    HI_U32  u32BitDepthOut;      /*RW;Range:[0xC,0x14];Format:5.0;The Bit depth of output */
    ISP_CMOS_SPLIT_POINT_S astSplitPoint[ISP_SPLIT_POINT_NUM];
} ISP_CMOS_SPLIT_S;

typedef struct hiISP_CMOS_GE_S
{
    HI_BOOL bEnable;                                 /*RW,Range: [   0, 1]      */
    HI_U8  u8Slope;                                  /*RW,Range: [   0, 0xE]    */
    HI_U8  u8SensiSlope;                             /*RW,Range: [   0, 0xE]    */
    HI_U16 u16SensiThr;                              /*RW,Range: [   0, 0x3FFF] */
    HI_U16 au16Threshold[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW,Range: [   0, 0x3FFF] */
    HI_U16 au16Strength[ISP_AUTO_ISO_STRENGTH_NUM];  /*RW,Range: [   0, 0x100]  */
    HI_U16 au16NpOffset[ISP_AUTO_ISO_STRENGTH_NUM];  /*RW,Range: [0x200, 0x3FFF] */
} ISP_CMOS_GE_S;

typedef struct hiISP_CMOS_ANTIFALSECOLOR_S
{
    HI_BOOL bEnable;                                                     /*RW;Range:[0x0,0x1];Format:1.0; AntiFalseColor Enable*/
    HI_U8   au8AntiFalseColorThreshold[ISP_AUTO_ISO_STRENGTH_NUM];       /*RW;Range:[0x0,0x20];Format:6.0;Threshold for antifalsecolor */
    HI_U8   au8AntiFalseColorStrength[ISP_AUTO_ISO_STRENGTH_NUM];        /*RW;Range:[0x0,0x1F];Format:5.0;Strength of antifalsecolor */
} ISP_CMOS_ANTIFALSECOLOR_S;

typedef struct hiISP_CMOS_LDCI_S
{
    HI_BOOL  bEnable;
    HI_U8    u8GaussLPFSigma;
    HI_U8    au8HePosWgt[ISP_AUTO_ISO_STRENGTH_NUM];
    HI_U8    au8HePosSigma[ISP_AUTO_ISO_STRENGTH_NUM];
    HI_U8    au8HePosMean[ISP_AUTO_ISO_STRENGTH_NUM];
    HI_U8    au8HeNegWgt[ISP_AUTO_ISO_STRENGTH_NUM];
    HI_U8    au8HeNegSigma[ISP_AUTO_ISO_STRENGTH_NUM];
    HI_U8    au8HeNegMean[ISP_AUTO_ISO_STRENGTH_NUM];
    HI_U16   au16BlcCtrl[ISP_AUTO_ISO_STRENGTH_NUM];
} ISP_CMOS_LDCI_S;

typedef struct hiISP_CMOS_PRELOGLUT_S
{
    HI_U32   au32PreLogLUT[PRE_LOG_LUT_SIZE];
} ISP_CMOS_PRELOGLUT_S;

typedef struct hiISP_CMOS_LOGLUT_S
{
    HI_U32   au32LogLUT[LOG_LUT_SIZE];
} ISP_CMOS_LOGLUT_S;

typedef struct hiISP_CMOS_SENSOR_MODE_S
{
    HI_U32  u32SensorID;
    HI_U8   u8SensorMode;
    HI_BOOL bValidDngRawFormat;
    DNG_RAW_FORMAT_S stDngRawFormat;
} ISP_CMOS_SENSOR_MODE_S;

typedef struct hiISP_CMOS_DNG_COLORPARAM_S
{
    ISP_DNG_WBGAIN_S stWbGain1;/*the calibration White balance gain of colorcheker in A Light*/
    ISP_DNG_WBGAIN_S stWbGain2;/*the calibration White balance gain of colorcheker in D50 Light*/
} ISP_CMOS_DNG_COLORPARAM_S;

typedef struct hiISP_CMOS_WDR_SWITCH_ATTR_S
{
    HI_U32   au32ExpRatio[EXP_RATIO_NUM];
}ISP_CMOS_WDR_SWITCH_ATTR_S;

typedef union hiISP_CMOS_ALG_KEY_U
{
    HI_U64  u64Key;
    struct
    {
        HI_U64  bit1Drc             : 1 ;   /* [0] */
        HI_U64  bit1Demosaic        : 1 ;   /* [1] */
        HI_U64  bit1PreGamma        : 1 ;   /* [2] */
        HI_U64  bit1Gamma           : 1 ;   /* [3] */
        HI_U64  bit1Sharpen         : 1 ;   /* [4] */
        HI_U64  bit1EdgeMark        : 1 ;   /* [5] */
        HI_U64  bit1Hlc             : 1 ;   /* [6] */
        HI_U64  bit1Ldci            : 1 ;   /* [7] */
        HI_U64  bit1Dpc             : 1 ;   /* [8] */
        HI_U64  bit1Lsc             : 1 ;   /* [9] */
        HI_U64  bit1RLsc            : 1 ;   /* [10] */
        HI_U64  bit1Ge              : 1 ;   /* [11] */
        HI_U64  bit1AntiFalseColor  : 1 ;   /* [12] */
        HI_U64  bit1BayerNr         : 1 ;   /* [13] */
        HI_U64  bit1Split           : 1 ;   /* [14] */
        HI_U64  bit1Ca              : 1 ;   /* [15] */
        HI_U64  bit1Clut            : 1 ;   /* [16] */
        HI_U64  bit1LogLUT          : 1 ;   /* [17] */
        HI_U64  bit1PreLogLUT       : 1 ;   /* [18] */
        HI_U64  bit1Wdr             : 1 ;   /* [19] */
        HI_U64  bit44Rsv            : 44;   /* [20:63] */
    };
} ISP_CMOS_ALG_KEY_U;

typedef struct hiISP_CMOS_DEFAULT_S
{
    ISP_CMOS_ALG_KEY_U               unKey;
    const ISP_CMOS_DRC_S             *pstDrc;
    const ISP_CMOS_DEMOSAIC_S        *pstDemosaic;
    const ISP_CMOS_PREGAMMA_S        *pstPreGamma;
    const ISP_CMOS_GAMMA_S           *pstGamma;
    const ISP_CMOS_SHARPEN_S         *pstSharpen;
    const ISP_CMOS_EDGEMARK_S        *pstEdgeMark;
    const ISP_CMOS_HLC_S             *pstHlc;
    const ISP_CMOS_LDCI_S            *pstLdci;
    const ISP_CMOS_DPC_S             *pstDpc;
    const ISP_CMOS_LSC_S             *pstLsc;
    const ISP_CMOS_RLSC_S            *pstRLsc;
    const ISP_CMOS_GE_S              *pstGe;
    const ISP_CMOS_ANTIFALSECOLOR_S  *pstAntiFalseColor;
    const ISP_CMOS_BAYERNR_S         *pstBayerNr;
    const ISP_CMOS_SPLIT_S           *pstSplit;
    const ISP_CMOS_CA_S              *pstCa;
    const ISP_CMOS_CLUT_S            *pstClut;
    const ISP_CMOS_LOGLUT_S          *pstLogLUT;
    const ISP_CMOS_PRELOGLUT_S       *pstPreLogLUT;
    const ISP_CMOS_WDR_S             *pstWdr;
    ISP_CMOS_NOISE_CALIBRATION_S     stNoiseCalibration;
    ISP_CMOS_SENSOR_MAX_RESOLUTION_S stSensorMaxResolution;
    ISP_CMOS_SENSOR_MODE_S           stSensorMode;
    ISP_CMOS_DNG_COLORPARAM_S        stDngColorParam;
    ISP_CMOS_WDR_SWITCH_ATTR_S       stWdrSwitchAttr;
} ISP_CMOS_DEFAULT_S;

typedef struct hiISP_CMOS_SENSOR_IMAGE_MODE_S
{
    HI_U16   u16Width;
    HI_U16   u16Height;
    HI_FLOAT f32Fps;
    HI_U8    u8SnsMode;
} ISP_CMOS_SENSOR_IMAGE_MODE_S;

typedef struct hiISP_SENSOR_EXP_FUNC_S
{
    HI_VOID(*pfn_cmos_sensor_init)(VI_PIPE ViPipe);
    HI_VOID(*pfn_cmos_sensor_exit)(VI_PIPE ViPipe);
    HI_VOID(*pfn_cmos_sensor_global_init)(VI_PIPE ViPipe);
    HI_S32 (*pfn_cmos_set_image_mode)(VI_PIPE ViPipe, ISP_CMOS_SENSOR_IMAGE_MODE_S *pstSensorImageMode);
    HI_S32 (*pfn_cmos_set_wdr_mode)(VI_PIPE ViPipe, HI_U8 u8Mode);

    /* the algs get data which is associated with sensor, except 3a */
    HI_S32(*pfn_cmos_get_isp_default)(VI_PIPE ViPipe, ISP_CMOS_DEFAULT_S *pstDef);
    HI_S32(*pfn_cmos_get_isp_black_level)(VI_PIPE ViPipe, ISP_CMOS_BLACK_LEVEL_S *pstBlackLevel);
    HI_S32(*pfn_cmos_get_sns_reg_info)(VI_PIPE ViPipe, ISP_SNS_REGS_INFO_S *pstSnsRegsInfo);

    /* the function of sensor set pixel detect */
    HI_VOID(*pfn_cmos_set_pixel_detect)(VI_PIPE ViPipe, HI_BOOL bEnable);
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
