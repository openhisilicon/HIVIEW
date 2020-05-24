#ifndef _HI_SCENE_SETPARAM_H_
#define _HI_SCENE_SETPARAM_H_

#include "hi_common.h"
#include "hi_comm_dis.h"
#include "hi_comm_isp.h"
#include "hi_comm_vi.h"
#include "hi_comm_vpss.h"
#include "hi_comm_venc.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif
#pragma pack(4)
#define MAX_ISP_PIPE_NUM                         (6)

#define HI_SCENE_PIPETYPE_NUM                    (4)

#define HI_SCENE_FPS_ISO_MAX_COUNT               (10)
#define HI_SCENE_AE_EXPOSURE_MAX_COUNT           (12)
#define HI_SCENE_DEHAZE_LUT_NUM                   (256)
#define HI_SCENE_ISO_STRENGTH_NUM                 (16)
#define HI_SCENE_RATIO_STRENGTH_NUM               (11)
#define HI_SCENE_SHADING_EXPOSURE_MAX_COUNT      (10)
#define HI_SCENE_GAMMA_EXPOSURE_MAX_COUNT        (10)
#define HI_SCENE_DRC_ISO_MAX_COUNT               (16)
#define HI_SCENE_DRC_REF_RATIO_MAX_COUNT         (9)
#define HI_SCENE_DRC_RATIO_MAX_COUNT             (7)
#define HI_SCENE_REF_EXP_RATIO_FRM               (16)
#define HI_SCENE_NR_LUT_LENGTH                    (33)
#define HI_SCENE_NR_RATIO_MAX_COUNT               (10)
#define HI_SCENE_FALSECOLOR_EXPOSURE_MAX_COUNT   (10)
#define HI_SCENE_DEMOSAIC_EXPOSURE_MAX_COUNT     (8)
#define HI_SCENE_DIS_ISO_MAX_COUNT               (3)
#define HI_SCENE_LDCI_EXPOSURE_MAX_COUNT         (6)
#define HI_SCENE_FSWDR_ISO_MAX_COUNT             (3)
#define HI_SCENE_DEHAZE_EXPOSURE_MAX_COUNT       (10)
#define HI_SCENE_3DNR_MAX_COUNT                  (16)
#define ISP_AUTO_ISO_CA_NUM                      (16)

typedef enum hiSCENE_OP_TYPE_E
{
    HI_SCENE_OP_TYPE_AUTO    = 0,
    HI_SCENE_OP_TYPE_MANUAL  = 1,
    HI_SCENE_OP_TYPE_BUTT
} HI_SCENE_OP_TYPE_E;

typedef struct hiSCENE_FPS_AUTO_S
{
    HI_U32 u32FpsMax;
} HI_SCENE_FPS_AUTO_S;

typedef struct hiSCENE_FPS_MANUAL_S
{
    HI_U32 u32Fps;
} HI_SCENE_FPS_MANUAL_S;

typedef struct hiSCENE_FPS_S
{
    HI_BOOL   bEnable;     /* RW; Range:[0, 1]; Format:1.0;Enable/Disable fps*/
    HI_SCENE_OP_TYPE_E   enOpType;
    HI_SCENE_FPS_AUTO_S   stAuto;
    HI_SCENE_FPS_MANUAL_S stManual;
} HI_SCENE_FPS_S;
typedef struct hiSCENE_MODULE_STATE_S
{
    HI_BOOL bDebug;
    HI_BOOL bAeWeightTab;
    HI_BOOL bStaticAE;
    HI_BOOL bStaticGlobalCac;
    HI_BOOL bStaticLocalCac;
    HI_BOOL bStaticDPC;
    HI_BOOL bStaticWdrExposure;
    HI_BOOL bStaticFsWdr;
    HI_BOOL bStaticAWB;
    HI_BOOL bStaticAWBEx;
    HI_BOOL bStaticDRC;
    HI_BOOL bStaticDehaze;
    HI_BOOL bStaticLdci;
    HI_BOOL bStaticSaturation;
    HI_BOOL bStaticCCM;
    HI_BOOL bStaticNr;
    HI_BOOL bStaticShading;
    HI_BOOL bStaticCSC;
    HI_BOOL bStaticSharpen;
    HI_BOOL bStaticClut;
    HI_BOOL bStaticCa;
    HI_BOOL bStaticCrosstalk;
    HI_BOOL bStatic3DNR;
    HI_BOOL bStaticVENC;

    HI_BOOL bDynamicFps;
    HI_BOOL bDynamicAE;
    HI_BOOL bDynamicWdrExposure;
    HI_BOOL bDynamicIsoVenc;
    HI_BOOL bDynamicFSWDR;
    HI_BOOL bDynamicDrc;
    HI_BOOL bDynamicLinearDrc;
    HI_BOOL bDynamicDehaze;
    HI_BOOL bDynamicLdci;
	HI_BOOL bDynamicNr;
	HI_BOOL bDynamicDpc;
	HI_BOOL bDynamicBLC;
	HI_BOOL bDynamicCA;
    HI_BOOL bDynamicShading;
    HI_BOOL bDynamicGamma;
    HI_BOOL bDynamicFalseColor;
    HI_BOOL bDynamic3DNR;
} HI_SCENE_MODULE_STATE_S;

typedef struct hiSCENE_STATIC_AE_S
{
    HI_BOOL bAERouteExValid;
    HI_U8 u8AERunInterval;
    HI_U32 u32AutoSysGainMax;
    HI_U8 u8AutoSpeed;
    HI_U8 u8AutoTolerance;
    HI_U16 u16AutoBlackDelayFrame;
    HI_U16 u16AutoWhiteDelayFrame;
} HI_SCENE_STATIC_AE_S;

typedef struct hiSCENE_DYNAMIC_FPS_S
{
    HI_U8 u8FPSExposureCnt;
    HI_U32 au32ExposureLtoHThresh[HI_SCENE_FPS_ISO_MAX_COUNT];
    HI_U32 au32ExposureHtoLThresh[HI_SCENE_FPS_ISO_MAX_COUNT];
    HI_U32 Fps[HI_SCENE_FPS_ISO_MAX_COUNT];
    HI_U32 u32VencGop[HI_SCENE_FPS_ISO_MAX_COUNT];
    HI_U32 au32AEMaxExpTime[HI_SCENE_FPS_ISO_MAX_COUNT];
} HI_SCENE_DYNAMIC_FPS_S;

typedef struct hiSCENE_DYNAMIC_AE_S
{
    HI_U8 u8AEExposureCnt;
	HI_U32 au32AutoExpRatioMax[HI_SCENE_AE_EXPOSURE_MAX_COUNT];
	HI_U32 au32AutoExpRatioMin[HI_SCENE_AE_EXPOSURE_MAX_COUNT];
	HI_U64 au64ExpLtoHThresh[HI_SCENE_AE_EXPOSURE_MAX_COUNT];
    HI_U64 au64ExpHtoLThresh[HI_SCENE_AE_EXPOSURE_MAX_COUNT];
    HI_U8 au8AutoCompensation[HI_SCENE_AE_EXPOSURE_MAX_COUNT];
    HI_U8 au8AutoMaxHistOffset[HI_SCENE_AE_EXPOSURE_MAX_COUNT];
} HI_SCENE_DYNAMIC_AE_S;

typedef struct hiSCENE_STATIC_AEROUTE_S
{
    HI_U32 u32TotalNum;
    HI_U32 au32IntTime[ISP_AE_ROUTE_MAX_NODES];
    HI_U32 au32SysGain[ISP_AE_ROUTE_MAX_NODES];
} HI_SCENE_STATIC_AEROUTE_S;

typedef struct hiSCENE_STATIC_AEROUTEEX_S
{
    HI_U32 u32TotalNum;
    HI_U32 au32IntTime[ISP_AE_ROUTE_EX_MAX_NODES];
    HI_U32 au32Again[ISP_AE_ROUTE_EX_MAX_NODES];
    HI_U32 au32Dgain[ISP_AE_ROUTE_EX_MAX_NODES];
    HI_U32 au32IspDgain[ISP_AE_ROUTE_EX_MAX_NODES];
} HI_SCENE_STATIC_AEROUTEEX_S;

typedef struct hiSCENE_STATIC_AWB_S
{
    HI_U16 au16AutoStaticWB[4];
    HI_S32 as32AutoCurvePara[6];
    HI_U16 u16AutoSpeed;
    HI_U16 u16AutoLowColorTemp;
    HI_U16 au16AutoCrMax[ISP_AUTO_ISO_STRENGTH_NUM];
    HI_U16 au16AutoCrMin[ISP_AUTO_ISO_STRENGTH_NUM];
    HI_U16 au16AutoCbMax[ISP_AUTO_ISO_STRENGTH_NUM];
    HI_U16 au16AutoCbMin[ISP_AUTO_ISO_STRENGTH_NUM];
    HI_U16 u16LumaHistEnable;
    HI_U16 u16AWBSwitch;
} HI_SCENE_STATIC_AWB_S;

typedef struct hiSCENE_STATIC_AWBEX_S
{
    HI_BOOL bBypass;
    HI_U8 u8Tolerance;
    HI_U32 u32OutThresh;
    HI_U16 u16LowStop;
    HI_U16 u16HighStart;
    HI_U16 u16HighStop;
    HI_BOOL bMultiLightSourceEn;
    HI_U16 au16MultiCTWt[8];
} HI_SCENE_STATIC_AWBEX_S;

typedef struct hiSCENE_STATIC_GLOBALCAC_S
{
    HI_BOOL bEnable;
    HI_U16 u16VerCoordinate;
    HI_U16 u16HorCoordinate;
    HI_S16 s16ParamRedA;
    HI_S16 s16ParamRedB;
    HI_S16 s16ParamRedC;
    HI_S16 s16ParamBlueA;
    HI_S16 s16ParamBlueB;
    HI_S16 s16ParamBlueC;
    HI_U8 u8VerNormShift;
    HI_U8 u8VerNormFactor;
    HI_U8 u8HorNormShift;
    HI_U8 u8HorNormFactor;
    HI_U16 u16CorVarThr;
} HI_SCENE_STATIC_GLOBALCAC_S;
typedef struct hiSCENE_STATIC_LOCALCAC_S
{
    HI_BOOL bEnable;
    HI_U16  u16PurpleDetRange;
    HI_U16  u16VarThr;
    HI_U8   au8DePurpleCrStr[HI_SCENE_ISO_STRENGTH_NUM];
    HI_U8   au8DePurpleCbStr[HI_SCENE_ISO_STRENGTH_NUM];
} HI_SCENE_STATIC_LOCALCAC_S;
typedef struct hiSCENE_STATIC_DPC_S
{
    HI_BOOL bEnable;
    HI_U16  au16Strength[HI_SCENE_ISO_STRENGTH_NUM];
    HI_U16  au16BlendRatio[HI_SCENE_ISO_STRENGTH_NUM];
} HI_SCENE_STATIC_DPC_S;

typedef struct hiSCENE_STATIC_WDREXPOSURE_S
{
    HI_U8 u8ExpRatioType;
    HI_U32 u32ExpRatioMax;
    HI_U32 u32ExpRatioMin;
    HI_U32 au32ExpRatio[EXP_RATIO_NUM];
	HI_U32 u32RefRatioUp;
	HI_U32 u32RefRatioDn;
	HI_U32 u32ExpTh;
} HI_SCENE_STATIC_WDREXPOSURE_S;

typedef struct hiSCENE_DYNAMIC_VENCBITRATE_S
{
    HI_U32 u32IsoThreshCnt;
    HI_U32 au32IsoThreshLtoH[HI_SCENE_SHADING_EXPOSURE_MAX_COUNT];
    HI_U16 au16ManualPercent[HI_SCENE_SHADING_EXPOSURE_MAX_COUNT];
} HI_SCENE_DYNAMIC_VENCBITRATE_S;
typedef struct hiSCENE_DYNAMIC_WDREXPOSURE_S
{
    HI_U8 u8AEExposureCnt;
    HI_U64 au64ExpLtoHThresh[HI_SCENE_AE_EXPOSURE_MAX_COUNT];
    HI_U64 au64ExpHtoLThresh[HI_SCENE_AE_EXPOSURE_MAX_COUNT];
    HI_U32 ExpCompensation[HI_SCENE_AE_EXPOSURE_MAX_COUNT];
    HI_U32 MaxHistOffset[HI_SCENE_AE_EXPOSURE_MAX_COUNT];
} HI_SCENE_DYNAMIC_WDREXPOSURE_S;

typedef struct hiSCENE_STATIC_FSWDR_S
{
    ISP_WDR_MERGE_MODE_E enWDRMergeMode;
} HI_SCENE_STATIC_FSWDR_S;

typedef struct hiSCENE_STATIC_DRC_S
{
    HI_BOOL bEnable;
    HI_U8 u8CurveSelect;
    HI_U8 u8DRCOpType;;
    HI_U16 u16AutoStrength;
    HI_U16 u16AutoStrengthMin;
    HI_U16 u16AutoStrengthMax;
    HI_U16 au16ToneMappingValue[HI_ISP_DRC_TM_NODE_NUM];
} HI_SCENE_STATIC_DRC_S;
typedef struct hiSCENE_DYNAMIC_LINEARDRC_S
{
    HI_U32 u32ISOCount;
    HI_U32 au32ISOLevel[HI_SCENE_DRC_ISO_MAX_COUNT];
    HI_BOOL bEnable;
    HI_U8  au8LocalMixingBrightMax[HI_SCENE_DRC_ISO_MAX_COUNT];
    HI_U8  au8LocalMixingBrightMin[HI_SCENE_DRC_ISO_MAX_COUNT];
    HI_U8  au8LocalMixingDarkMax[HI_SCENE_DRC_ISO_MAX_COUNT];
    HI_U8  au8LocalMixingDarkMin[HI_SCENE_DRC_ISO_MAX_COUNT];
    HI_U8  au8BrightGainLmt[HI_SCENE_DRC_ISO_MAX_COUNT];
    HI_U8  au8BrightGainLmtStep[HI_SCENE_DRC_ISO_MAX_COUNT];
    HI_U8  au8DarkGainLmtY[HI_SCENE_DRC_ISO_MAX_COUNT];
    HI_U8  au8DarkGainLmtC[HI_SCENE_DRC_ISO_MAX_COUNT];
    HI_U8  au8FltScaleCoarse[HI_SCENE_DRC_ISO_MAX_COUNT];
    HI_U8  au8FltScaleFine[HI_SCENE_DRC_ISO_MAX_COUNT];
    HI_U8  au8ContrastControl[HI_SCENE_DRC_ISO_MAX_COUNT];
    HI_S8  as8DetailAdjustFactor[HI_SCENE_DRC_ISO_MAX_COUNT];
    HI_U8  au8Asymmetry[HI_SCENE_DRC_ISO_MAX_COUNT];
    HI_U8  au8SecondPole[HI_SCENE_DRC_ISO_MAX_COUNT];
    HI_U8  au8Compress[HI_SCENE_DRC_ISO_MAX_COUNT];
    HI_U8  au8Stretch[HI_SCENE_DRC_ISO_MAX_COUNT];
    HI_U16 au16Strength[HI_SCENE_DRC_ISO_MAX_COUNT];
} HI_SCENE_DYNAMIC_LINEARDRC_S;

typedef struct hiSCENE_DYNAMIC_DRC_S
{
    HI_U32 u32RationCount;
    HI_U32 au32RationLevel[HI_SCENE_DRC_RATIO_MAX_COUNT];
    HI_U32 u32ISOCount;
    HI_U32 au32ISOLevel[HI_SCENE_DRC_ISO_MAX_COUNT];
    HI_U32 u32Interval;
    HI_BOOL bEnable;
    HI_U8 au8LocalMixingBrightMax[HI_SCENE_DRC_ISO_MAX_COUNT];
    HI_U8 au8LocalMixingBrightMin[HI_SCENE_DRC_ISO_MAX_COUNT];
    HI_U8 au8LocalMixingDarkMax[HI_SCENE_DRC_ISO_MAX_COUNT];
    HI_U8 au8LocalMixingDarkMin[HI_SCENE_DRC_ISO_MAX_COUNT];
    HI_U8 au8DarkGainLmtY[HI_SCENE_DRC_ISO_MAX_COUNT];
    HI_U8 au8DarkGainLmtC[HI_SCENE_DRC_ISO_MAX_COUNT];
    HI_S8 as8DetailAdjustFactor[HI_SCENE_DRC_ISO_MAX_COUNT];
    HI_U8 au8SpatialFltCoef[HI_SCENE_DRC_ISO_MAX_COUNT];
    HI_U8 au8RangeFltCoef[HI_SCENE_DRC_ISO_MAX_COUNT];
    HI_U8 au8GradRevMax[HI_SCENE_DRC_ISO_MAX_COUNT];
    HI_U8 au8GradRevThr[HI_SCENE_DRC_ISO_MAX_COUNT];
    HI_U8 au8Compress[HI_SCENE_DRC_ISO_MAX_COUNT];
    HI_U8 au8Stretch[HI_SCENE_DRC_ISO_MAX_COUNT];
    HI_U16 u16AutoStrength[HI_SCENE_DRC_ISO_MAX_COUNT][HI_SCENE_DRC_RATIO_MAX_COUNT];
    HI_U32 u32RefRatioCount;
    HI_U32 au32RefRatioLtoH[HI_SCENE_DRC_REF_RATIO_MAX_COUNT];
    HI_U32 u32RefRatioAlpha[HI_SCENE_DRC_REF_RATIO_MAX_COUNT];
    HI_U16 au16TMValueLow[HI_ISP_DRC_TM_NODE_NUM];
    HI_U16 au16TMValueHigh[HI_ISP_DRC_TM_NODE_NUM];
} HI_SCENE_DYNAMIC_DRC_S;

typedef struct hiSCENE_STATIC_LDCI_S
{
    HI_BOOL bEnable;
    HI_U8 u8LDCIOpType;
    HI_U8 u8GaussLPFSigma;
    HI_U8 au8AutoHePosWgt[ISP_AUTO_ISO_STRENGTH_NUM];
    HI_U8 au8AutoHePosSigma[ISP_AUTO_ISO_STRENGTH_NUM];
    HI_U8 au8AutoHePosMean[ISP_AUTO_ISO_STRENGTH_NUM];
    HI_U8 au8AutoHeNegWgt[ISP_AUTO_ISO_STRENGTH_NUM];
    HI_U8 au8AutoHeNegSigma[ISP_AUTO_ISO_STRENGTH_NUM];
    HI_U8 au8AutoHeNegMean[ISP_AUTO_ISO_STRENGTH_NUM];
    HI_U8 au16AutoBlcCtrl[ISP_AUTO_ISO_STRENGTH_NUM];
} HI_SCENE_STATIC_LDCI_S;

typedef struct hiSCENE_DYNAMIC_LDCI_S
{
    HI_U32 u32EnableCnt;
    HI_U8 au8Enable[HI_SCENE_LDCI_EXPOSURE_MAX_COUNT];
    HI_U64 au64EnableExpThreshLtoH[HI_SCENE_LDCI_EXPOSURE_MAX_COUNT];
    HI_U32 u32ExpThreshCnt;
    HI_U8 au8ManualLDCIHePosWgt[HI_SCENE_LDCI_EXPOSURE_MAX_COUNT];
    HI_U64 au64ExpThreshLtoH[HI_SCENE_LDCI_EXPOSURE_MAX_COUNT];
} HI_SCENE_DYNAMIC_LDCI_S;

typedef struct hiSCENE_STATIC_DEHAZE_S
{
    HI_BOOL bEnable;
    HI_U8 u8DehazeOpType;
    HI_BOOL bUserLutEnable;
    HI_U8 au8DehazeLut[256];
} HI_SCENE_STATIC_DEHAZE_S;

typedef struct
{

    HI_U32  IES0, IES1, IES2, IES3;
    HI_U32  IEDZ, IEEn;

} tV200_VPSS_X_IEy;



typedef struct
{

    HI_U32   SPN6 , SFR;
    HI_U32   SBN6, PBR6;
    HI_U32  SRT0, SRT1, JMODE, DeIdx;
    HI_U32   SFR6[4], SBR6[2], DeRate;
    HI_U32   SFS1,  SFT1,  SBR1;
    HI_U32   SFS2,  SFT2,  SBR2;
    HI_U32   SFS4,  SFT4,  SBR4;
    HI_U32  STH1,  SFN1, SFN0, NRyEn;
    HI_U32  STHd1;
    HI_U32  STH2,  SFN2, kMode;
    HI_U32  STHd2;
    HI_U32  SBSk[32], SDSk[32];



} tV200_VPSS_X_SFy;

typedef struct
{
    HI_U32  MADZ0,   MAI00,  MAI01, MAI02;
    HI_U32  MADZ1,   MAI10,  MAI11, MAI12;
    HI_U32   MABR0, MABR1;
    HI_U32  MATH0,  MATE0,  MATW;
    HI_U32  MATHd0;
    HI_U32  MATH1;
    HI_U32  MATHd1;
    HI_U32   MASW ,  MATE1;
    HI_U32   MABW0,  MABW1;
    HI_U32  AdvMATH,   AdvTH;

} tV200_VPSS_X_MDy;

typedef struct
{

    HI_U32  TFS0,   TDZ0,  TDX0;
    HI_U32  TFS1,   TDZ1,  TDX1;
    HI_U32  SDZ0,  STR0,   DZMode0;
    HI_U32  SDZ1,  STR1,   DZMode1;
    HI_U32   TSS0,   TSI0,   TFR0[6];
    HI_U32   TSS1,   TSI1,   TFR1[6];
    HI_U32   TFRS,   TED,   bRef;

} tV200_VPSS_X_TFy;

//typedef struct
//{
//    HI_U32 SMODE, bRFU, RFUI;   // searchMode,  bRefUpt, refUptIdx (For reference update)
//
//    HI_U32 SDTH, SDSLP;                // sadThr, sadSlp
//    HI_U32 RFDZ0, RFSLP0;                // refDz0, refSlp0
//    HI_U32 RFDZ1, RFSLP1;
//    HI_U32 RFDZ2, RFSLP2;
//
//    HI_U32 RFUTH0, RFHI0;                // refUptThr0, refHdgIdx0, bRefUpt
//    HI_U32 RFUTH1, RFHI1, RFHI2;
//} tV200_VPSS_X_RFs;

typedef struct
{
    HI_U32   SFC, TFC;
    HI_U32   TRC, TPC;

} tV200_VPSS_X_NRc;

typedef struct hiSCENE_3DNR_S
{
    tV200_VPSS_X_IEy  IEy[5];
    tV200_VPSS_X_SFy  SFy[5];
    tV200_VPSS_X_MDy  MDy[2];
    tV200_VPSS_X_TFy  TFy[3];
    tV200_VPSS_X_NRc  NRc;

} HI_SCENE_3DNR_S;

HI_U32 MCREn;
HI_U32 MCRTmp;
HI_U32 u32RefExpRatio;

typedef struct hiSCENE_STATIC_3DNR_S
{
    HI_U32 u32ThreeDNRCount;
    HI_U32 au32ThreeDNRIso[HI_SCENE_3DNR_MAX_COUNT];
    HI_SCENE_3DNR_S astThreeDNRValue[HI_SCENE_3DNR_MAX_COUNT];
} HI_SCENE_STATIC_3DNR_S;

typedef struct hiSCENE_STATIC_STATISTICSCFG_S
{
    HI_U8 au8AEWeight[AE_ZONE_ROW][AE_ZONE_COLUMN];
} HI_SCENE_STATIC_STATISTICSCFG_S;

typedef struct hiSCENE_STATIC_SATURATION_S
{
    HI_U8 au8AutoSat[ISP_AUTO_ISO_STRENGTH_NUM];
} HI_SCENE_STATIC_SATURATION_S;

typedef struct hiSCENE_STATIC_CCM_S
{
    HI_U8  u8CCMOpType;
    HI_U16 au16ManCCM[CCM_MATRIX_SIZE];
    HI_BOOL bAutoISOActEn;
    HI_BOOL bAutoTempActEn;
    HI_U32 u32TotalNum;
    HI_U16 au16AutoColorTemp[CCM_MATRIX_NUM];
    HI_U16 au16AutoCCM[CCM_MATRIX_NUM][CCM_MATRIX_SIZE];
} HI_SCENE_STATIC_CCM_S;

typedef struct hiSCENE_STATIC_CSC_S
{
    HI_BOOL bEnable;
	HI_U8 u8Hue;
	HI_U8 u8Luma;
	HI_U8 u8Contr;
	HI_U8 u8Satu;
    COLOR_GAMUT_E enColorGamut;
} HI_SCENE_STATIC_CSC_S;

typedef struct hiSCENE_STATIC_NR_S
{
    HI_BOOL bEnable;
    HI_U8   au8FineStr[ISP_AUTO_ISO_STRENGTH_NUM];
    HI_U16  au16CoringWgt[ISP_AUTO_ISO_STRENGTH_NUM];
} HI_SCENE_STATIC_NR_S;

typedef struct hiSCENE_DYNAMIC_NR_S
{
    HI_U32 u32CoringRatioCount;
    HI_U32 au32CoringRatioIso[HI_SCENE_NR_RATIO_MAX_COUNT];
    HI_U16 au16CoringRatio[HI_SCENE_NR_RATIO_MAX_COUNT][HI_SCENE_NR_LUT_LENGTH];
    HI_U8  au8FrameShortStr[HI_SCENE_NR_RATIO_MAX_COUNT];
    HI_U8  au8FrameLongStr[HI_SCENE_NR_RATIO_MAX_COUNT];
} HI_SCENE_DYNAMIC_NR_S;

typedef struct hiSCENE_DYNAMIC_CA_S
{
    HI_U32 u32CACount;
    HI_U32 au32IsoThresh[16];
    HI_U32 au32CAYRatioLut[16][HI_ISP_CA_YRATIO_LUT_LENGTH];

} HI_SCENE_DYNAMIC_CA_S;

typedef struct hiSCENE_DYNAMIC_BLC_S
{
    HI_U32 u32BLCCount;
    HI_U32 au32IsoThresh[16];
    HI_U32 au32BLCValue[16][4];//4

} HI_SCENE_DYNAMIC_BLC_S;

typedef struct hiSCENE_STATIC_SHADING_S
{
    HI_BOOL bEnable;
} HI_SCENE_STATIC_SHADING_S;

typedef struct hiSCENE_DYNAMIC_SHADING_S
{
    HI_U32 u32ExpThreshCnt;
    HI_U64 au64ExpThreshLtoH[HI_SCENE_SHADING_EXPOSURE_MAX_COUNT];
    HI_U16 au16ManualStrength[HI_SCENE_SHADING_EXPOSURE_MAX_COUNT];
} HI_SCENE_DYNAMIC_SHADING_S;

typedef struct hiSCENE_STATIC_SHARPEN_S
{
    HI_BOOL bEnable;
    HI_U8  au8LumaWgt[ISP_SHARPEN_GAIN_NUM][ISP_AUTO_ISO_STRENGTH_NUM];
    HI_U16 au16AutoTextureStr[ISP_SHARPEN_GAIN_NUM][ISP_AUTO_ISO_STRENGTH_NUM];
    HI_U16 au16AutoEdgeStr[ISP_SHARPEN_GAIN_NUM][ISP_AUTO_ISO_STRENGTH_NUM];
    HI_U8 au8AutoOverShoot[ISP_AUTO_ISO_STRENGTH_NUM];
    HI_U8 au8AutoUnderShoot[ISP_AUTO_ISO_STRENGTH_NUM];
    HI_U8 au8ShootSupStr[ISP_AUTO_ISO_STRENGTH_NUM];
    HI_U8 au8ShootSupAdj[ISP_AUTO_ISO_STRENGTH_NUM];
    HI_U16 au16AutoTextureFreq[ISP_AUTO_ISO_STRENGTH_NUM];
    HI_U16 au16AutoEdgeFreq[ISP_AUTO_ISO_STRENGTH_NUM];
    HI_U8 au8AutoDetailCtrl[ISP_AUTO_ISO_STRENGTH_NUM];
    HI_U8 au8EdgeFiltStr[ISP_AUTO_ISO_STRENGTH_NUM];
    HI_U8 au8EdgeFiltMaxCap[ISP_AUTO_ISO_STRENGTH_NUM];
    HI_U8 au8RGain[ISP_AUTO_ISO_STRENGTH_NUM];
    HI_U8 au8BGain[ISP_AUTO_ISO_STRENGTH_NUM];
    HI_U8 au8GGain[ISP_AUTO_ISO_STRENGTH_NUM];
    HI_U8 au8SkinGain[ISP_AUTO_ISO_STRENGTH_NUM];
    HI_U16 au16MaxSharpGain[ISP_AUTO_ISO_STRENGTH_NUM];
} HI_SCENE_STATIC_SHARPEN_S;

typedef struct hiSCENE_STATIC_CLUT_S
{
    HI_BOOL bEnable;
} HI_SCENE_STATIC_CLUT_S;

typedef struct hiSCENE_STATIC_CROSSTALK_S
{
    HI_BOOL bEnable;
} HI_SCENE_STATIC_CROSSTALK_S;

typedef struct hiSCENE_STATIC_CA_S
{
    HI_BOOL bEnable;
    HI_U16 au16IsoRatio[ISP_AUTO_ISO_CA_NUM];
} HI_SCENE_STATIC_CA_S;

/* The param of h265e vbr */
typedef struct hiSCENE_STATIC_VENC_PARAM_H265_AVBR_S {
    HI_S32  s32ChangePos;               /* RW; Range:[50, 100];Indicates the ratio of the current bit rate to the maximum
                                            bit rate when the QP value starts to be adjusted */
    HI_U32  u32MinIprop;                /* RW; [1, 100]the min ratio of i frame and p frame */
    HI_U32  u32MaxIprop;                /* RW; [1, 100]the max ratio of i frame and p frame,can not be smaller than u32MinIprop */
    HI_S32  s32MaxReEncodeTimes;        /* RW; Range:[0, 3]; Range:max number of re-encode times.*/

    HI_S32  s32MinStillPercent;         /* RW; Range:[5, 100]; the min percent of target bitrate for still scene */
    HI_U32  u32MaxStillQP;              /* RW; Range:[0, 51]; the max QP value of I frame for still scene,
                                               can not be smaller than u32MinIQp and can not be larger than su32MaxIQp*/
    HI_U32  u32MinStillPSNR;            /* RW; reserved */

    HI_U32  u32MaxQp;                   /* RW; Range:[0, 51];the max P B qp */
    HI_U32  u32MinQp;                   /* RW; Range:[0, 51];the min P B qp,can not be larger than u32MaxQp */
    HI_U32  u32MaxIQp;                  /* RW; Range:[0, 51];the max I qp */
    HI_U32  u32MinIQp;                  /* RW; Range:[0, 51];the min I qp,can not be larger than u32MaxIQp */

    HI_U32  u32MinQpDelta;              /* RW; Range:[0, 4];Difference between FrameLevelMinQp and MinQp, FrameLevelMinQp = MinQp(or MinIQp) + MinQpDelta  */
    HI_U32  u32MotionSensitivity;       /* RW; Range:[0, 100]; Motion Sensitivity */

    HI_BOOL bQpMapEn;                   /* RW; Range:[0, 1]; enable qpmap.*/
    VENC_RC_QPMAP_MODE_E enQpMapMode;   /* RW; Qpmap Mode*/
} HI_SCENE_STATIC_VENC_PARAM_H265_AVBR_S;

typedef struct hi_SCENE_STATIC_VENC_PARAM_H265_CVBR_S {
    HI_U32  u32MinIprop;                /* RW; Range:[1, 100] ; the min ratio of i frame and p frame */
    HI_U32  u32MaxIprop;                /* RW; Range:[1, 100] ; the max ratio of i frame and p frame,can not be smaller than u32MinIprop */
    HI_S32  s32MaxReEncodeTimes;        /* RW; Range:[0, 3]; max number of re-encode times */
    HI_BOOL bQpMapEn;
    VENC_RC_QPMAP_MODE_E enQpMapMode;   /* RW; Qpmap Mode*/

    HI_U32  u32MaxQp;                   /* RW; Range:[0, 51]; the max P B qp */
    HI_U32  u32MinQp;                   /* RW; Range:[0, 51]; the min P B qp,can not be larger than u32MaxQp */
    HI_U32  u32MaxIQp;                  /* RW; Range:[0, 51]; the max I qp */
    HI_U32  u32MinIQp;                  /* RW; Range:[0, 51]; the min I qp,can not be larger than u32MaxIQp */

    HI_U32  u32MinQpDelta;              /* RW; Range:[0, 4];Difference between FrameLevelMinQp and MinQp, FrameLevelMinQp = MinQp(or MinIQp) + MinQpDelta  */
    HI_U32  u32MaxQpDelta;              /* RW; Range:[0, 4];Difference between FrameLevelMaxQp and MaxQp, FrameLevelMaxQp = MaxQp(or MaxIQp) - MaxQpDelta  */

    HI_U32  u32ExtraBitPercent;         /* RW; Range:[0, 1000];the extra ratio of bitrate that can be allocated when the actual bitrate goes above the long-term target bitrate*/
    HI_U32  u32LongTermStatTimeUnit;    /* RW; Range:[1, 1800]; the time unit of LongTermStatTime, the unit is senconds(s)*/
} HI_SCENE_STATIC_VENC_PARAM_H265_CVBR_S;


typedef struct hiSCENE_STATIC_VENCATTR_S
{
    HI_SCENE_STATIC_VENC_PARAM_H265_AVBR_S VENC_PARAM_H265_AVBR_S;
    HI_SCENE_STATIC_VENC_PARAM_H265_CVBR_S VENC_PARAM_H265_CVBR_S;

} HI_SCENE_STATIC_VENCATTR_S;

typedef struct hiSCENE_DYNAMIC_GAMMA_S
{
    HI_U32 u32InterVal;
    HI_U32 u32TotalNum;
    HI_U64 au64ExpThreshLtoH[HI_SCENE_GAMMA_EXPOSURE_MAX_COUNT];
    HI_U64 au64ExpThreshHtoL[HI_SCENE_GAMMA_EXPOSURE_MAX_COUNT];
    HI_U16 au16Table[HI_SCENE_GAMMA_EXPOSURE_MAX_COUNT][GAMMA_NODE_NUM];
} HI_SCENE_DYNAMIC_GAMMA_S;

typedef struct hiSCENE_DYNAMIC_DEHAZE_S
{
    HI_U32 u32ExpThreshCnt;
    HI_U64 au64ExpThreshLtoH[HI_SCENE_DEHAZE_EXPOSURE_MAX_COUNT];
    HI_U8  au8ManualStrength[HI_SCENE_DEHAZE_EXPOSURE_MAX_COUNT];
} HI_SCENE_DYNAMIC_DEHAZE_S;

/*
typedef struct hiSCENE_DYNAMIC_FSWDR_S
{
    HI_U32 u32IsoCnt;
    HI_U32 au32ExpRation[HI_SCENE_FSWDR_ISO_MAX_COUNT];
    HI_U8 au8MotionComp[HI_SCENE_FSWDR_ISO_MAX_COUNT];
    HI_U32 au32ISOLtoHThresh[HI_SCENE_FSWDR_ISO_MAX_COUNT];
    HI_U8 au8BnrStr[HI_SCENE_FSWDR_ISO_MAX_COUNT];
    HI_U8 au8BnrMode[HI_SCENE_FSWDR_ISO_MAX_COUNT];
} HI_SCENE_DYNAMIC_FSWDR_S;
*/

typedef struct hiSCENE_WDR_COMBINE_ATTR_S
{
    HI_U8 au8MdThrLowGain[HI_SCENE_RATIO_STRENGTH_NUM][HI_SCENE_ISO_STRENGTH_NUM];
    HI_U8 au8MdThrHigGain[HI_SCENE_RATIO_STRENGTH_NUM][HI_SCENE_ISO_STRENGTH_NUM];
} HI_SCENE_WDR_COMBINE_ATTR_S;

typedef struct hiSCENE_DYNAMIC_WDR_FS_ATTR_S
{
    HI_U32 au32ISOLtoHThresh[HI_SCENE_ISO_STRENGTH_NUM];
    HI_U32 au32RatioLtoHThresh[HI_SCENE_RATIO_STRENGTH_NUM];
    HI_SCENE_WDR_COMBINE_ATTR_S stWDRCombine;
} HI_SCENE_DYNAMIC_WDR_FS_ATTR_S;

typedef struct hiSCENE_DYNAMIC_FALSECOLOR_S
{
    HI_U32 u32TotalNum;
    HI_U32 au32FalsecolorExpThresh[HI_SCENE_FALSECOLOR_EXPOSURE_MAX_COUNT];
    HI_U8 au8ManualStrength[HI_SCENE_FALSECOLOR_EXPOSURE_MAX_COUNT];
} HI_SCENE_DYNAMIC_FALSECOLOR_S;

typedef struct hiSCENE_PIPE_PARAM_S
{
    HI_SCENE_MODULE_STATE_S stModuleState;
    HI_SCENE_STATIC_AE_S stStaticAe;
    HI_SCENE_STATIC_AEROUTE_S stStaticAeRoute;
    HI_SCENE_STATIC_AEROUTEEX_S stStaticAeRouteEx;
    HI_SCENE_STATIC_STATISTICSCFG_S stStaticStatistics;
    HI_SCENE_STATIC_WDREXPOSURE_S stStaticWdrExposure;
    HI_SCENE_STATIC_FSWDR_S stStaticFsWdr;
    HI_SCENE_STATIC_AWB_S stStaticAwb;
    HI_SCENE_STATIC_AWBEX_S stStaticAwbEx;
    HI_SCENE_STATIC_CCM_S stStaticCcm;
    HI_SCENE_STATIC_SATURATION_S stStaticSaturation;
    HI_SCENE_STATIC_CLUT_S stStaticClut;
    HI_SCENE_STATIC_LDCI_S stStaticLdci;
    HI_SCENE_STATIC_DRC_S stStaticDrc;
    HI_SCENE_STATIC_NR_S stStaticNr;
    HI_SCENE_STATIC_CA_S stStaticCa;
    HI_SCENE_STATIC_VENCATTR_S stStaticVencAttr;
    HI_SCENE_STATIC_GLOBALCAC_S stStaticGlobalCac;
    HI_SCENE_STATIC_LOCALCAC_S  stStaticLocalCac;
    HI_SCENE_STATIC_DPC_S stStaticDPC;
    HI_SCENE_STATIC_DEHAZE_S stStaticDehaze;
    HI_SCENE_STATIC_SHADING_S stStaticShading;
    HI_SCENE_STATIC_CSC_S stStaticCsc;
    HI_SCENE_STATIC_CROSSTALK_S stStaticCrosstalk;
    HI_SCENE_STATIC_SHARPEN_S stStaticSharpen;
    HI_SCENE_STATIC_3DNR_S stStaticThreeDNR;

    HI_SCENE_DYNAMIC_FPS_S stDynamicFps;
    HI_SCENE_DYNAMIC_AE_S stDynamicAe;
    HI_SCENE_DYNAMIC_WDREXPOSURE_S stDynamicWdrExposure;
    HI_SCENE_DYNAMIC_WDR_FS_ATTR_S stDynamicFSWDR;
    HI_SCENE_DYNAMIC_DEHAZE_S stDynamicDehaze;
    HI_SCENE_DYNAMIC_DRC_S stDynamicDrc;
    HI_SCENE_DYNAMIC_LINEARDRC_S stDynamicLinearDrc;
    HI_SCENE_DYNAMIC_GAMMA_S stDynamicGamma;
    HI_SCENE_DYNAMIC_NR_S stDynamicNr;
    HI_SCENE_DYNAMIC_CA_S stDynamicCA;
    HI_SCENE_DYNAMIC_BLC_S stDynamicBLC;
    HI_SCENE_DYNAMIC_SHADING_S stDynamicShading;
    HI_SCENE_DYNAMIC_VENCBITRATE_S stDynamicVencBitrate;
    HI_SCENE_DYNAMIC_LDCI_S stDynamicLDCI;
    HI_SCENE_DYNAMIC_FALSECOLOR_S stDynamicFalsecolor;
} HI_SCENE_PIPE_PARAM_S;

HI_S32 HI_SCENE_SetStaticAE(VI_PIPE ViPipe, HI_U8 u8Index);

HI_S32 HI_SCENE_SetDynamicFps(VI_PIPE ViPipe, HI_U64 u64Exposure, HI_U64 u64LastExposure, HI_U8 u8Index);

HI_S32 HI_SCENE_SetDynamicAE(VI_PIPE ViPipe, HI_U64 u64Exposure, HI_U64 u64LastExposure, HI_U8 u8Index);

HI_S32 HI_SCENE_SetStaticGlobalCac(VI_PIPE ViPipe, HI_U8 u8Index);

HI_S32 HI_SCENE_SetStaticLocalCac(VI_PIPE ViPipe, HI_U8 u8Index);
HI_S32 HI_SCENE_SetStaticDPC(VI_PIPE ViPipe, HI_U8 u8Index);
HI_S32 HI_SCENE_SetStaticWDRExposure(VI_PIPE ViPipe, HI_U8 u8Index);

HI_S32 HI_SCENE_SetStaticFSWDR(VI_PIPE ViPipe, HI_U8 u8Index);

HI_S32 HI_SCENE_SetStaticDRC(VI_PIPE ViPipe, HI_U8 u8Index);

HI_S32 HI_SCENE_SetStaticDeHaze(VI_PIPE ViPipe, HI_U8 u8Index);

HI_S32 HI_SCENE_SetStaticLDCI(VI_PIPE ViPipe, HI_U8 u8Index);

HI_S32 HI_SCENE_SetStaticAWB(VI_PIPE ViPipe, HI_U8 u8Index);

HI_S32 HI_SCENE_SetStaticAWBEX(VI_PIPE ViPipe, HI_U8 u8Index);

HI_S32 HI_SCENE_SetStaticSaturation(VI_PIPE ViPipe, HI_U8 u8Index);

HI_S32 HI_SCENE_SetStaticCCM(VI_PIPE ViPipe, HI_U8 u8Index);

HI_S32 HI_SCENE_SetStaticCSC(VI_PIPE ViPipe, HI_U8 u8Index);

HI_S32 HI_SCENE_SetStaticNr(VI_PIPE ViPipe, HI_U8 u8Index);

HI_S32 HI_SCENE_SetStaticShading(VI_PIPE ViPipe, HI_U8 u8Index);

HI_S32 HI_SCENE_SetAeWeightTab(VI_PIPE ViPipe, HI_U8 u8Index);

HI_S32 HI_SCENE_SetStatic3DNR(VI_PIPE ViPipe, HI_U8 u8Index);

HI_S32 HI_SCENE_SetDynamic3DNR(VI_PIPE ViPipe, HI_U32 u32Iso, HI_U8 u8Index);

HI_S32 HI_SCENE_SetStaticSharpen(VI_PIPE ViPipe, HI_U8 u8Index);

HI_S32 HI_SCENE_SetStaticClut(VI_PIPE ViPipe, HI_U8 u8Index);

HI_S32 HI_SCENE_SetStaticCA(VI_PIPE ViPipe, HI_U8 u8Index);

HI_S32 HI_SCENE_SetStaticVENC(VI_PIPE ViPipe, HI_U8 u8Index);
HI_S32 HI_SCENE_SetStaticCrosstalk(VI_PIPE ViPipe, HI_U8 u8Index);

HI_S32 HI_SCENE_SetDynamicPhotoGamma(VI_PIPE ViPipe, HI_U64 u64Exposure, HI_U64 u64LastExposure, HI_U8 u8Index);

HI_S32 HI_SCENE_SetDynamicVideoGamma(VI_PIPE ViPipe, HI_U64 u64Exposure, HI_U64 u64LastExposure, HI_U8 u8Index);

HI_S32 HI_SCENE_SetDynamicWdrExposure(VI_PIPE ViPipe, HI_U64 u64Exposure, HI_U64 u64LastExposure, HI_U8 u8Index);

HI_S32 HI_SCENE_SetDynamicShading(VI_PIPE ViPipe, HI_U64 u64ISO, HI_U64 u64LastISO, HI_U8 u8Index);

HI_S32 HI_SCENE_SetDynamicVencBitrate(VI_PIPE ViPipe, HI_U64 u64Iso, HI_U64 u64LastIso, HI_U8 u8Index);
HI_S32 HI_SCENE_SetDynamicNr(VI_PIPE ViPipe, HI_U64 u64Iso, HI_U64 u64LastIso, HI_U8 u8Index);

HI_S32 HI_SCENE_SetDynamicCA(VI_PIPE ViPipe, HI_U64 u64Iso, HI_U64 u64LastIso, HI_U8 u8Index);

HI_S32 HI_SCENE_SetDynamicBLC(VI_PIPE ViPipe, HI_U64 u64Iso, HI_U64 u64LastIso, HI_U8 u8Index);

HI_S32 HI_SCENE_SetDynamicDpc(VI_PIPE ViPipe, HI_U64 u64ISO, HI_U64 u64LastISO, HI_U8 u8Index);

HI_S32 HI_SCENE_SetDynamicLDCI(VI_PIPE ViPipe, HI_U64 u64Exposure, HI_U64 u64LastExposure, HI_U8 u8Index);

HI_S32 HI_SCENE_SetDynamicBlackLevel(VI_PIPE ViPipe, HI_U64 u64Exposure, HI_U64 u64LastExposure, HI_U8 u8Index);

HI_S32 HI_SCENE_SetDynamicFalseColor(VI_PIPE ViPipe, HI_U64 u64Exposure, HI_U64 u64LastExposure, HI_U8 u8Index);

HI_S32 HI_SCENE_SetDynamicDehaze(VI_PIPE ViPipe, HI_U64 u64Exposure, HI_U64 u64LastExposure, HI_U8 u8Index);

HI_S32 HI_SCENE_SetDynamicFsWdr(VI_PIPE ViPipe, HI_U32 u32ISO, HI_U32 u32LastISO, HI_U8 u8Index, HI_U32 u32WdrRatio);

HI_S32 HI_SCENE_SetDynamicDRC(VI_PIPE ViPipe, HI_U8 u8Index, HI_U32 u32WdrRatio, HI_U32 u32Iso);

HI_S32 HI_SCENE_SetDynamicLinearDRC(VI_PIPE ViPipe, HI_U64 u64Iso, HI_U64 u64LastIso, HI_U8 u8Index);
HI_S32 HI_SCENE_SetPipeParam(const HI_SCENE_PIPE_PARAM_S *pstScenePipeParam, HI_U32 u32Num);

HI_S32 HI_SCENE_SetPause(HI_BOOL bPause);

HI_S32 HI_SCENE_SetRCParam(VI_PIPE ViPipe, HI_U8 u8Index);
#pragma pack ()
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif
