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

#define MAX_ISP_PIPE_NUM                         (6)
#define HI_SCENE_PIPETYPE_NUM                    (10)
#define HI_SCENE_AE_EXPOSURE_MAX_COUNT           (12)
#define HI_SCENE_SHADING_EXPOSURE_MAX_COUNT      (10)
#define HI_SCENE_GAMMA_EXPOSURE_MAX_COUNT        (10)
#define HI_SCENE_DRC_ISO_MAX_COUNT               (8)
#define HI_SCENE_DRC_REF_RATIO_MAX_COUNT         (9)
//#define HI_SCENE_DRC_EXP_MAX_COUNT               (7)
#define HI_SCENE_DRC_RATIO_MAX_COUNT             (7)
#define HI_SCENE_FALSECOLOR_EXPOSURE_MAX_COUNT   (10)
#define HI_SCENE_DEMOSAIC_EXPOSURE_MAX_COUNT     (8)
#define HI_SCENE_DIS_ISO_MAX_COUNT               (3)
#define HI_SCENE_LDCI_EXPOSURE_MAX_COUNT         (6)
#define HI_SCENE_FSWDR_ISO_MAX_COUNT             (3)
#define HI_SCENE_DEHAZE_EXPOSURE_MAX_COUNT       (5)
#define HI_SCENE_3DNR_MAX_COUNT                  (12)

typedef struct hiSCENE_MODULE_STATE_S
{
    HI_BOOL bStaticAE;
    HI_BOOL bDynamicAE;
    HI_BOOL bStaticGlobalCac;
	HI_BOOL bStaticLocalCac;
    HI_BOOL bStaticWdrExposure;
	HI_BOOL bDynamicWdrExposure;
    HI_BOOL bStaticAWB;
    HI_BOOL bStaticAWBEx;
    HI_BOOL bStaticDRC;
    HI_BOOL bDynamicDrc;
    HI_BOOL bStaticDehaze;
    HI_BOOL bDynamicDehaze;
    HI_BOOL bStaticLdci;
    HI_BOOL bDynamicLdci;
    HI_BOOL bAeWeightTab;
    HI_BOOL bStatic3DNR;
	HI_BOOL bDyanamic3DNR;
    HI_BOOL bStaticSaturation;
    HI_BOOL bStaticCCM;
    HI_BOOL bStaticNr;
	HI_BOOL bDynamicNr;
	HI_BOOL bDynamicDpc;
	HI_BOOL bDynamicBLC;
    HI_BOOL bStaticShading;
    HI_BOOL bDynamicShading;
    HI_BOOL bStaticCSC;
    HI_BOOL bStaticSharpen;
    HI_BOOL bStaticClut;
    HI_BOOL bStaticCa;
    HI_BOOL bStaticCrosstalk;
    HI_BOOL bDynamicGamma;
    HI_BOOL bDynamicFSWDR;
    HI_BOOL bDynamicFalseColor;
    HI_BOOL bDynamicDIS;
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
} HI_SCENE_STATIC_AWB_S;

typedef struct hiSCENE_STATIC_AWBEX_S
{
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
} HI_SCENE_STATIC_LOCALCAC_S;

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

typedef struct hiSCENE_DYNAMIC_WDREXPOSURE_S
{
    HI_U8 u8AEExposureCnt;
    HI_U64 au64ExpLtoHThresh[HI_SCENE_AE_EXPOSURE_MAX_COUNT];
    HI_U64 au64ExpHtoLThresh[HI_SCENE_AE_EXPOSURE_MAX_COUNT];
    HI_U32 ExpCompensation[HI_SCENE_AE_EXPOSURE_MAX_COUNT];
    HI_U32 MaxHistOffset[HI_SCENE_AE_EXPOSURE_MAX_COUNT];
} HI_SCENE_DYNAMIC_WDREXPOSURE_S;

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

typedef struct hiSCENE_DYNAMIC_DRC_S
{
    HI_U32 u32RationCount;
    HI_U32 au32RationLevel[HI_SCENE_DRC_RATIO_MAX_COUNT];
    HI_U32 u32ISOCount;
    HI_U32 au32ISOLevel[HI_SCENE_DRC_ISO_MAX_COUNT];
	
	HI_U32 u32RefRatioCount;
	HI_U32 au32RefRatioLtoH[HI_SCENE_DRC_REF_RATIO_MAX_COUNT];
    HI_U32 u32RefRatioAlpha[HI_SCENE_DRC_REF_RATIO_MAX_COUNT];
	
    HI_U32 u32Interval;
    HI_BOOL bEnable;
    HI_U8 au8LocalMixingBrightMax[HI_SCENE_DRC_ISO_MAX_COUNT];
    HI_U8 au8LocalMixingBrightMin[HI_SCENE_DRC_ISO_MAX_COUNT];
    HI_U8 au8LocalMixingDarkMax[HI_SCENE_DRC_ISO_MAX_COUNT];
    HI_U8 au8LocalMixingDarkMin[HI_SCENE_DRC_ISO_MAX_COUNT];
    HI_S8 as8DetailAdjustFactor[HI_SCENE_DRC_ISO_MAX_COUNT];
    HI_U8 au8SpatialFltCoef[HI_SCENE_DRC_ISO_MAX_COUNT];
    HI_U8 au8RangeFltCoef[HI_SCENE_DRC_ISO_MAX_COUNT];
    HI_U8 au8GradRevMax[HI_SCENE_DRC_ISO_MAX_COUNT];
    HI_U8 au8GradRevThr[HI_SCENE_DRC_ISO_MAX_COUNT];
    HI_U8 au8Compress[HI_SCENE_DRC_ISO_MAX_COUNT];
    HI_U8 au8Stretch[HI_SCENE_DRC_ISO_MAX_COUNT];
    HI_U16 u16AutoStrength[HI_SCENE_DRC_ISO_MAX_COUNT][HI_SCENE_DRC_RATIO_MAX_COUNT];
    HI_U32 u32ExpCount;
    //HI_U64 au64ExpThreshLtoH[HI_SCENE_DRC_EXP_MAX_COUNT];
    HI_U16 au16ToneMappingValue1[HI_ISP_DRC_TM_NODE_NUM];
    HI_U16 au16ToneMappingValue2[HI_ISP_DRC_TM_NODE_NUM];
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
    HI_U32  IES0;
    HI_U32  IES1;
    HI_U32  IES2;
    HI_U32  IES3;
    HI_U32  IEDZ;
    HI_U32  _rb_;
} tV56xIEy;

typedef struct
{
    HI_U32  SPN6,SFR;
    HI_U32  SBN6,PBR6;
    HI_U32  SFS2,SFT2,SBR2;
    HI_U32  SFS4,SFT4,SBR4;
    HI_U32  STH1,SFN1,SFN0,NRyEn;
    HI_U32  STH2,SFN2,BWSF4,kMode;
    HI_U32  STH3,SFN3,tEdge,TriTh,_rb_;
} tV56xSFy;

typedef struct
{
    HI_U32  MADZ,MAI0,MAI1,MAI2;
    HI_U32  MADK,MABR;
    HI_U32  MATH,MATE,MATW;
    HI_U32  MASW,MABW,MAXN,_rB_;
} tV56xMDy;

typedef struct
{
    HI_U32  TFS,TDZ,TDX;
    HI_U32  TFR[5],TSS,TSI,_rb_;
    HI_U32  SDZ,STR,bRef;
} tV56xTFy;

typedef struct
{
    HI_U32  SFC,_rb_,TFC;
    HI_U32  CSFS,CSFR;
    HI_U32  CTFS,CIIR;
    HI_U32  CTFR;
} tV56xNRc;

typedef struct hiSCENE_3DNR_S
{
    tV56xIEy IEy[2];
    tV56xSFy SFy[4];
    tV56xMDy MDy[2];
    tV56xTFy TFy[2];
    tV56xNRc NRc;
    HI_U32 SBSk2[32], SDSk2[32];
    HI_U32 SBSk3[32], SDSk3[32];
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
} HI_SCENE_STATIC_NR_S;

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
    HI_U16 au16AutoTextureStr[ISP_SHARPEN_GAIN_NUM][ISP_AUTO_ISO_STRENGTH_NUM];
    HI_U16 au16AutoEdgeStr[ISP_SHARPEN_GAIN_NUM][ISP_AUTO_ISO_STRENGTH_NUM];
    HI_U8 au8AutoOverShoot[ISP_AUTO_ISO_STRENGTH_NUM];
    HI_U8 au8AutoUnderShoot[ISP_AUTO_ISO_STRENGTH_NUM];
    HI_U8 au8ShootSupStr[ISP_AUTO_ISO_STRENGTH_NUM];
    HI_U16 au16AutoTextureFreq[ISP_AUTO_ISO_STRENGTH_NUM];
    HI_U16 au16AutoEdgeFreq[ISP_AUTO_ISO_STRENGTH_NUM];
    HI_U8 au8AutoDetailCtrl[ISP_AUTO_ISO_STRENGTH_NUM];
    HI_U8 au8EdgeFiltStr[ISP_AUTO_ISO_STRENGTH_NUM];
    HI_U8 au8RGain[ISP_AUTO_ISO_STRENGTH_NUM];
    HI_U8 au8BGain[ISP_AUTO_ISO_STRENGTH_NUM];
    HI_U8 au8SkinGain[ISP_AUTO_ISO_STRENGTH_NUM];
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
} HI_SCENE_STATIC_CA_S;

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
    HI_U8 au8ManualStrength[HI_SCENE_DEHAZE_EXPOSURE_MAX_COUNT];
} HI_SCENE_DYNAMIC_DEHAZE_S;

typedef struct hiSCENE_DYNAMIC_FSWDR_S
{
    HI_U32 u32IsoCnt;
    HI_U32 au32ExpRation[HI_SCENE_FSWDR_ISO_MAX_COUNT];
    HI_U8 au8MotionComp[HI_SCENE_FSWDR_ISO_MAX_COUNT];
    HI_U32 au32ISOLtoHThresh[HI_SCENE_FSWDR_ISO_MAX_COUNT];
    HI_U8 au8BnrStr[HI_SCENE_FSWDR_ISO_MAX_COUNT];
    HI_U8 au8BnrMode[HI_SCENE_FSWDR_ISO_MAX_COUNT];
} HI_SCENE_DYNAMIC_FSWDR_S;

typedef struct hiSCENE_DYNAMIC_FALSECOLOR_S
{
    HI_U32 u32TotalNum;
    HI_U32 au32FalsecolorExpThresh[HI_SCENE_FALSECOLOR_EXPOSURE_MAX_COUNT];
    HI_U8 au8ManualStrength[HI_SCENE_FALSECOLOR_EXPOSURE_MAX_COUNT];
} HI_SCENE_DYNAMIC_FALSECOLOR_S;

typedef struct hiSCENE_DYNAMIC_DIS_S
{
    HI_U32 u32TotalNum;
    HI_U32 au32DISIsoThresh[HI_SCENE_DIS_ISO_MAX_COUNT];
    HI_U32 au32MovingSubjectLevel[HI_SCENE_DIS_ISO_MAX_COUNT];
} HI_SCENE_DYNAMIC_DIS_S;

typedef struct hiSCENE_PIPE_PARAM_S
{
    HI_SCENE_STATIC_AE_S stStaticAe;
    HI_SCENE_DYNAMIC_AE_S stDynamicAe;
    HI_SCENE_STATIC_AEROUTE_S stStaticAeRoute;
    HI_SCENE_STATIC_AEROUTEEX_S stStaticAeRouteEx;
    HI_SCENE_STATIC_GLOBALCAC_S stStaticGlobalCac;
	HI_SCENE_STATIC_LOCALCAC_S  stStaticLocalCac;
    HI_SCENE_STATIC_WDREXPOSURE_S stStaticWdrExposure;
    HI_SCENE_DYNAMIC_WDREXPOSURE_S stDynamicWdrExposure;
    HI_SCENE_STATIC_DRC_S stStaticDrc;
    HI_SCENE_DYNAMIC_DRC_S stDynamicDrc;
    HI_SCENE_STATIC_DEHAZE_S stStaticDehaze;
    HI_SCENE_DYNAMIC_DEHAZE_S stDynamicDehaze;
    HI_SCENE_STATIC_LDCI_S stStaticLdci;
    HI_SCENE_DYNAMIC_LDCI_S stDynamicLDCI;
    HI_SCENE_STATIC_STATISTICSCFG_S stStaticStatistics;
    HI_SCENE_STATIC_3DNR_S stStaticThreeDNR;
    HI_SCENE_STATIC_AWB_S stStaticAwb;
    HI_SCENE_STATIC_AWBEX_S stStaticAwbEx;
    HI_SCENE_STATIC_CCM_S stStaticCcm;
    HI_SCENE_STATIC_CSC_S stStaticCsc;
    HI_SCENE_STATIC_NR_S stStaticNr;
    HI_SCENE_STATIC_SHADING_S stStaticShading;
    HI_SCENE_DYNAMIC_SHADING_S stDynamicShading;
    HI_SCENE_STATIC_SATURATION_S stStaticSaturation;
    HI_SCENE_STATIC_SHARPEN_S stStaticSharpen;
    HI_SCENE_STATIC_CA_S stStaticCa;
    HI_SCENE_STATIC_CROSSTALK_S stStaticCrosstalk;
    HI_SCENE_STATIC_CLUT_S stStaticClut;
    HI_SCENE_DYNAMIC_FSWDR_S stDynamicFSWDR;
    HI_SCENE_DYNAMIC_GAMMA_S stDynamicGamma;
    HI_SCENE_DYNAMIC_FALSECOLOR_S stDynamicFalsecolor;
    HI_SCENE_DYNAMIC_DIS_S stDynamicDis;
    HI_SCENE_MODULE_STATE_S stModuleState;
} HI_SCENE_PIPE_PARAM_S;

HI_S32 HI_SCENE_SetStaticAE(VI_PIPE ViPipe, HI_U8 u8Index);

HI_S32 HI_SCENE_SetDynamicAE(VI_PIPE ViPipe, HI_U64 u64Exposure, HI_U64 u64LastExposure, HI_U8 u8Index);

HI_S32 HI_SCENE_SetStaticAE(VI_PIPE ViPipe, HI_U8 u8Index);

HI_S32 HI_SCENE_SetStaticGlobalCac(VI_PIPE ViPipe, HI_U8 u8Index);

HI_S32 HI_SCENE_SetStaticLocalCac(VI_PIPE ViPipe, HI_U8 u8Index);

HI_S32 HI_SCENE_SetStaticWDRExposure(VI_PIPE ViPipe, HI_U8 u8Index);

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

HI_S32 HI_SCENE_SetStaticCrosstalk(VI_PIPE ViPipe, HI_U8 u8Index);

HI_S32 HI_SCENE_SetDynamicPhotoGamma(VI_PIPE ViPipe, HI_U64 u64Exposure, HI_U64 u64LastExposure, HI_U8 u8Index);

HI_S32 HI_SCENE_SetDynamicVideoGamma(VI_PIPE ViPipe, HI_U64 u64Exposure, HI_U64 u64LastExposure, HI_U8 u8Index);

HI_S32 HI_SCENE_SetDynamicWdrExposure(VI_PIPE ViPipe, HI_U64 u64Exposure, HI_U64 u64LastExposure, HI_U8 u8Index);

HI_S32 HI_SCENE_SetDynamicShading(VI_PIPE ViPipe, HI_U64 u64ISO, HI_U64 u64LastISO, HI_U8 u8Index);

HI_S32 HI_SCENE_SetDynamicNr(VI_PIPE ViPipe, HI_U64 u64ISO, HI_U64 u64LastISO, HI_U8 u8Index);

HI_S32 HI_SCENE_SetDynamicDpc(VI_PIPE ViPipe, HI_U64 u64ISO, HI_U64 u64LastISO, HI_U8 u8Index);

HI_S32 HI_SCENE_SetDynamicLDCI(VI_PIPE ViPipe, HI_U64 u64Exposure, HI_U64 u64LastExposure, HI_U8 u8Index);

HI_S32 HI_SCENE_SetDynamicBlackLevel(VI_PIPE ViPipe, HI_U64 u64Exposure, HI_U64 u64LastExposure, HI_U8 u8Index);

HI_S32 HI_SCENE_SetDynamicFalseColor(VI_PIPE ViPipe, HI_U64 u64Exposure, HI_U64 u64LastExposure, HI_U8 u8Index);

HI_S32 HI_SCENE_SetDynamicDehaze(VI_PIPE ViPipe, HI_U64 u64Exposure, HI_U64 u64LastExposure, HI_U8 u8Index);

HI_S32 HI_SCENE_SetDynamicFsWdr(VI_PIPE ViPipe, HI_U32 u32ISO, HI_U32 u32LastISO, HI_U8 u8Index, HI_U32 u32WdrRatio);

HI_S32 HI_SCENE_SetDynamicDIS(VI_PIPE ViPipe, VI_CHN ViChn, HI_U32 u32Iso, HI_U32 u32LastIso, HI_U8 u8Index, HI_BOOL bEnable);

HI_S32 HI_SCENE_SetDynamicDRC(VI_PIPE ViPipe, HI_U8 u8Index, HI_U32 u32WdrRatio, HI_U32 u32Iso, HI_U64 u64Exposure);

HI_S32 HI_SCENE_SetPipeParam(const HI_SCENE_PIPE_PARAM_S *pstScenePipeParam, HI_U32 u32Num);

HI_S32 HI_SCENE_SetPause(HI_BOOL bPause);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif
