#ifndef _HI_SCENE_AUTOGENERATE_H_
#define _HI_SCENE_AUTOGENERATE_H_

#include "hi_common.h"
#include "hi_comm_dis.h"
#include "hi_comm_isp.h"
#include "hi_comm_vi.h"
#include "hi_comm_vpss.h"
#include "hi_comm_venc.h"
#include "hi_comm_hdr.h"
#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define HI_SCENE_PIPETYPE_NUM                    (10)
#define HI_SCENE_AE_EXPOSURE_MAX_COUNT           (12)
#define HI_SCENE_SHADING_EXPOSURE_MAX_COUNT      (10)
#define HI_SCENE_GAMMA_EXPOSURE_MAX_COUNT        (10)
#define HI_SCENE_DRC_ISO_MAX_COUNT               (7)
#define HI_SCENE_DRC_EXP_MAX_COUNT               (7)
#define HI_SCENE_DRC_RATIO_MAX_COUNT             (6)
#define HI_SCENE_FALSECOLOR_EXPOSURE_MAX_COUNT   (10)
#define HI_SCENE_DEMOSAIC_EXPOSURE_MAX_COUNT     (8)
#define HI_SCENE_DIS_ISO_MAX_COUNT               (3)
#define HI_SCENE_LDCI_EXPOSURE_MAX_COUNT         (6)
#define HI_SCENE_FSWDR_ISO_MAX_COUNT             (3)
#define HI_SCENE_DEHAZE_EXPOSURE_MAX_COUNT       (5)
#define HI_SCENE_THREEDNR_MAX_COUNT              (12)
#define HI_SCENE_HDR_MAX_COUNT                   (10)
typedef struct hiSCENE_MODULE_STATE_S
{
    HI_BOOL bStaticAE;
    HI_BOOL bStaticAERoute;
    HI_BOOL bStaticAERouteEx;
    HI_BOOL bStaticGlobalCac;
    HI_BOOL bStaticWdrExposure;
    HI_BOOL bStaticAWB;
    HI_BOOL bStaticAWBEx;
    HI_BOOL bStaticDrc;
    HI_BOOL bStaticDehaze;
    HI_BOOL bStaticLdci;
    HI_BOOL bStaticStatistics;
    HI_BOOL bStaticThreeDNR;
    HI_BOOL bStaticSaturation;
    HI_BOOL bStaticCCM;
    HI_BOOL bStaticNr;
    HI_BOOL bStaticShading;
    HI_BOOL bStaticCSC;
    HI_BOOL bDyanamicGamma;
    HI_BOOL bDynamicShading;
    HI_BOOL bDynamicLdci;
    HI_BOOL bDynamicAE;
    HI_BOOL bDynamicDehaze;
    HI_BOOL bDynamicFSWDR;
    HI_BOOL bDynamicHDRTM;
    HI_BOOL bDynamicHDROETF;
    HI_BOOL bDynamicHDRDRC;
    HI_BOOL bDyanamicFalseColor;
    HI_BOOL bDyanamicDemosaic;
    HI_BOOL bDyanamicDIS;
    HI_BOOL bThreadDrc;
} HI_SCENE_MODULE_STATE_S;


typedef struct hiSCENE_STATIC_AE_S
{
    HI_BOOL bAERouteExValid;
    HI_U8 u8AERunInterval;
    HI_U32 u32AutoExpTimeMax;
    HI_U32 u32AutoSysGainMax;
    HI_U8 u8AutoSpeed;
    HI_U8 u8AutoTolerance;
    HI_U16 u16AutoBlackDelayFrame;
    HI_U16 u16AutoWhiteDelayFrame;
} HI_SCENE_STATIC_AE_S;

typedef struct hiSCENE_STATIC_AEROUTE_S
{
} HI_SCENE_STATIC_AEROUTE_S;

typedef struct hiSCENE_STATIC_AWB_S
{
} HI_SCENE_STATIC_AWB_S;

typedef struct hiSCENE_STATIC_AWBEX_S
{
} HI_SCENE_STATIC_AWBEX_S;

typedef struct hiSCENE_STATIC_AEROUTEEX_S
{
    HI_U32 u32TotalNum;
    HI_U32 au32IntTime[ISP_AE_ROUTE_EX_MAX_NODES];
    HI_U32 au32Again[ISP_AE_ROUTE_EX_MAX_NODES];
    HI_U32 au32Dgain[ISP_AE_ROUTE_EX_MAX_NODES];
    HI_U32 au32IspDgain[ISP_AE_ROUTE_EX_MAX_NODES];
} HI_SCENE_STATIC_AEROUTEEX_S;

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

typedef struct hiSCENE_STATIC_WDREXPOSURE_S
{
    HI_U8 u8ExpRatioType;
    HI_U32 u32ExpRatioMax;
    HI_U32 u32ExpRatioMin;
    HI_U32 au32ExpRatio[EXP_RATIO_NUM];
} HI_SCENE_STATIC_WDREXPOSURE_S;

typedef struct hiSCENE_STATIC_DRC_S
{
    HI_BOOL bEnable;
    HI_U8 u8CurveSelect;
    HI_U8 u8DRCOpType;;
    HI_U16 au16ToneMappingValue[HI_ISP_DRC_TM_NODE_NUM];
} HI_SCENE_STATIC_DRC_S;

typedef struct hiSCENE_STATIC_LDCI_S
{
} HI_SCENE_STATIC_LDCI_S;

typedef struct hiSCENE_STATIC_DEHAZE_S
{
    HI_BOOL bEnable;
    HI_U8 u8DehazeOpType;
    HI_BOOL bUserLutEnable;
    HI_U8 au8DehazeLut[256];
} HI_SCENE_STATIC_DEHAZE_S;

typedef struct
{
    HI_S32 IES, IESS;
    HI_S32 IEDZ;
} tV59aSceIEy;

typedef struct
{
    HI_S32  SBF, STR, STHp, SFT, kPro;
    HI_S32  STH[3], SBS[3], SDS[3];
} tV59aSceSFy;

typedef struct
{
    HI_S32  MATH,  MATE,  MATW;
    HI_S32  MASW,  MABW,  MAXN, _rB_;
} tV59aSceMDy;

typedef struct
{
    HI_S32  TFR[4];
    HI_S32  TDZ, TDX;
    HI_S32  TFS, _rb_;
} tV59aSceTFy;

typedef struct
{
    HI_S32  SFC, TFC;
    HI_S32  CSFS, CSFk;
    HI_S32  CTFS, CIIR;
    HI_S32  CTFR;
} tV59aSceNRc;

typedef struct hiSCENE_3DNR_S
{
    tV59aSceIEy  IEy;
    tV59aSceSFy  SFy[5];
    tV59aSceMDy  MDy[2];
    tV59aSceTFy  TFy[2];
    HI_S32  HdgType,   BriType;
    HI_S32  HdgMode,   kTab2;
    HI_S32  HdgWnd,    kTab3;
    HI_S32  HdgSFR,    nOut;
    HI_S32  HdgIES,    _rb_,   nRef;
    HI_S32  SFRi  [ 4], SFRk [ 4];
    HI_S32  SBSk2 [32], SBSk3[32];
    HI_S32  SDSk2 [32], SDSk3[32];
    HI_S32  BriThr[16];
    tV59aSceNRc  NRc;
} HI_SCENE_3DNR_S;

typedef struct hiSCENE_STATIC_THREEDNR_S
{
    HI_U32 u32ThreeDNRCount;
    HI_U32 au32ThreeDNRIso[HI_SCENE_THREEDNR_MAX_COUNT];
    HI_SCENE_3DNR_S astThreeDNRValue[HI_SCENE_THREEDNR_MAX_COUNT];
} HI_SCENE_STATIC_THREEDNR_S;

typedef struct hiSCENE_STATIC_STATISTICSCFG_S
{
    HI_U8 au8AEWeight[AE_ZONE_ROW][AE_ZONE_COLUMN];
} HI_SCENE_STATIC_STATISTICSCFG_S;

typedef struct hiSCENE_STATIC_SATURATION_S
{
} HI_SCENE_STATIC_SATURATION_S;

typedef struct hiSCENE_STATIC_CCM_S
{
} HI_SCENE_STATIC_CCM_S;

typedef struct hiSCENE_STATIC_CSC_S
{
} HI_SCENE_STATIC_CSC_S;

typedef struct hiSCENE_STATIC_NR_S
{
} HI_SCENE_STATIC_NR_S;

typedef struct hiSCENE_STATIC_SHADING_S
{
    HI_BOOL bEnable;
} HI_SCENE_STATIC_SHADING_S;

typedef struct hiSCENE_DYNAMIC_GAMMA_S
{
    HI_U32 u32InterVal;
    HI_U32 u32TotalNum;
    HI_U64 au64ExpThreshLtoH[HI_SCENE_GAMMA_EXPOSURE_MAX_COUNT];
    HI_U64 au64ExpThreshHtoL[HI_SCENE_GAMMA_EXPOSURE_MAX_COUNT];
    HI_U16 au16Table[HI_SCENE_GAMMA_EXPOSURE_MAX_COUNT][GAMMA_NODE_NUM];
} HI_SCENE_DYNAMIC_GAMMA_S;

typedef struct hiSCENE_DYNAMIC_AE_S
{
    HI_U8 u8AEExposureCnt;
    HI_U64 au64ExpLtoHThresh[HI_SCENE_AE_EXPOSURE_MAX_COUNT];
    HI_U64 au64ExpHtoLThresh[HI_SCENE_AE_EXPOSURE_MAX_COUNT];
    HI_U8 au8AutoCompensation[HI_SCENE_AE_EXPOSURE_MAX_COUNT];
    HI_U8 au8AutoMaxHistOffset[HI_SCENE_AE_EXPOSURE_MAX_COUNT];
    HI_U32 au32AutoExpRatioMax[HI_SCENE_AE_EXPOSURE_MAX_COUNT];
    HI_U32 au32AutoExpRatioMin[HI_SCENE_AE_EXPOSURE_MAX_COUNT];
} HI_SCENE_DYNAMIC_AE_S;

typedef struct hiSCENE_DYNAMIC_SHADING_S
{
    HI_U32 u32ExpThreshCnt;
    HI_U64 au64ExpThreshLtoH[HI_SCENE_SHADING_EXPOSURE_MAX_COUNT];
    HI_U16 au16ManualStrength[HI_SCENE_SHADING_EXPOSURE_MAX_COUNT];
} HI_SCENE_DYNAMIC_SHADING_S;

typedef struct hiSCENE_DYNAMIC_LDCI_S
{
    HI_U32 u32EnableCnt;
    HI_U8 au8Enable[HI_SCENE_LDCI_EXPOSURE_MAX_COUNT];
    HI_U64 au64EnableExpThreshLtoH[HI_SCENE_LDCI_EXPOSURE_MAX_COUNT];
} HI_SCENE_DYNAMIC_LDCI_S;

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

typedef struct hiSCENE_DYNAMIC_HDROEFT_S
{
} HI_SCENE_DYNAMIC_HDROETF_S;

typedef struct hiSCENE_DYNAMIC_HDRTM_S
{
} HI_SCENE_DYNAMIC_HDRTM_S;

typedef struct hiSCENE_DYNAMIC_HDRDRC_S
{
} HI_SCENE_DYNAMIC_HDRDRC_S;

typedef struct hiSCENE_DYNAMIC_FALSECOLOR_S
{
} HI_SCENE_DYNAMIC_FALSECOLOR_S;

typedef struct hiSCENE_DYNAMIC_DEMOSAIC_S
{
} HI_SCENE_DYNAMIC_DEMOSAIC_S;

typedef struct hiSCENE_DYNAMIC_DIS_S
{
} HI_SCENE_DYNAMIC_DIS_S;

typedef struct hiSCENE_THREAD_DRC_S
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
    HI_S8 as8DetailAdjustFactor[HI_SCENE_DRC_ISO_MAX_COUNT];
    HI_U8 au8SpatialFltCoef[HI_SCENE_DRC_ISO_MAX_COUNT];
    HI_U8 au8RangeFltCoef[HI_SCENE_DRC_ISO_MAX_COUNT];
    HI_U8 au8GradRevMax[HI_SCENE_DRC_ISO_MAX_COUNT];
    HI_U8 au8GradRevThr[HI_SCENE_DRC_ISO_MAX_COUNT];
    HI_U8 au8Compress[HI_SCENE_DRC_ISO_MAX_COUNT];
    HI_U8 au8Stretch[HI_SCENE_DRC_ISO_MAX_COUNT];
    HI_U16 u16AutoStrength[HI_SCENE_DRC_ISO_MAX_COUNT][HI_SCENE_DRC_RATIO_MAX_COUNT];
    HI_U32 u32ExpCount;
    HI_U64 au64ExpThreshLtoH[HI_SCENE_DRC_EXP_MAX_COUNT];
    HI_U32 u32Aplha[HI_SCENE_DRC_EXP_MAX_COUNT];
    HI_U16 au16ToneMappingValue1[HI_ISP_DRC_TM_NODE_NUM];
    HI_U16 au16ToneMappingValue2[HI_ISP_DRC_TM_NODE_NUM];
} HI_SCENE_THREAD_DRC_S;

typedef struct hiSCENE_PIPE_PARAM_S
{
    HI_SCENE_STATIC_AE_S stStaticAe;
    HI_SCENE_STATIC_AEROUTE_S stStaticAeRoute;
    HI_SCENE_STATIC_AEROUTEEX_S stStaticAeRouteEx;
    HI_SCENE_STATIC_GLOBALCAC_S stStaticGlobalCac;
    HI_SCENE_STATIC_WDREXPOSURE_S stStaticWdrExposure;
    HI_SCENE_STATIC_DRC_S stStaticDrc;
    HI_SCENE_STATIC_DEHAZE_S stStaticDehaze;
    HI_SCENE_STATIC_LDCI_S stStaticLdci;
    HI_SCENE_STATIC_STATISTICSCFG_S stStaticStatistics;
    HI_SCENE_STATIC_THREEDNR_S stStaticThreeDNR;
    HI_SCENE_STATIC_AWB_S stStaticAwb;
    HI_SCENE_STATIC_AWBEX_S stStaticAwbEx;
    HI_SCENE_STATIC_CCM_S stStaticCcm;
    HI_SCENE_STATIC_CSC_S stStaticCsc;
    HI_SCENE_STATIC_NR_S stStaticNr;
    HI_SCENE_STATIC_SHADING_S stStaticShading;
    HI_SCENE_STATIC_SATURATION_S stStaticSaturation;
    HI_SCENE_DYNAMIC_AE_S stDynamicAe;
    HI_SCENE_DYNAMIC_SHADING_S stDynamicShading;
    HI_SCENE_DYNAMIC_LDCI_S stDynamicLDCI;
    HI_SCENE_DYNAMIC_DEHAZE_S stDynamicDehaze;
    HI_SCENE_DYNAMIC_FSWDR_S stDynamicFSWDR;
    HI_SCENE_DYNAMIC_HDROETF_S stDynamicHDROETF;
    HI_SCENE_DYNAMIC_HDRTM_S stDynamicHDRTm;
    HI_SCENE_DYNAMIC_GAMMA_S stDynamicGamma;
    HI_SCENE_DYNAMIC_HDRDRC_S stDynamicHDRDRC;
    HI_SCENE_DYNAMIC_FALSECOLOR_S stDynamicFalsecolor;
    HI_SCENE_DYNAMIC_DEMOSAIC_S stDynamicDemosaic;
    HI_SCENE_DYNAMIC_DIS_S stDynamicDis;
    HI_SCENE_THREAD_DRC_S stThreadDrc;
} HI_SCENE_PIPE_PARAM_S;

HI_S32 HI_SCENE_SetStaticAE_AutoGenerate(VI_PIPE ViPipe, HI_U8 u8Index);

HI_S32 HI_SCENE_SetStaticAERoute_AutoGenerate(VI_PIPE ViPipe, HI_U8 u8Index);

HI_S32 HI_SCENE_SetStaticAERouteEX_AutoGenerate(VI_PIPE ViPipe, HI_U8 u8Index);

HI_S32 HI_SCENE_SetStaticGlobalCac_AutoGenerate(VI_PIPE ViPipe, HI_U8 u8Index);

HI_S32 HI_SCENE_SetStaticWDRExposure_AutoGenerate(VI_PIPE ViPipe, HI_U8 u8Index);

HI_S32 HI_SCENE_SetStaticDRC_Autogenerate(VI_PIPE ViPipe, HI_U8 u8Index);

HI_S32 HI_SCENE_SetStaticDeHaze_AutoGenerate(VI_PIPE ViPipe, HI_U8 u8Index);

HI_S32 HI_SCENE_SetStaticLDCI_AutoGenerate(VI_PIPE ViPipe, HI_U8 u8Index);

HI_S32 HI_SCENE_SetStaticAWB_AutoGenerate(VI_PIPE ViPipe, HI_U8 u8Index);

HI_S32 HI_SCENE_SetStaticAWBEX_AutoGenerate(VI_PIPE ViPipe, HI_U8 u8Index);

HI_S32 HI_SCENE_SetStaticSaturation_AutoGenerate(VI_PIPE ViPipe, HI_U8 u8Index);

HI_S32 HI_SCENE_SetStaticCCM_AutoGenerate(VI_PIPE ViPipe, HI_U8 u8Index);

HI_S32 HI_SCENE_SetStaticCSC_AutoGenerate(VI_PIPE ViPipe, HI_U8 u8Index);

HI_S32 HI_SCENE_SetStaticNr_AutoGenerate(VI_PIPE ViPipe, HI_U8 u8Index);

HI_S32 HI_SCENE_SetStaticShading_AutoGenerate(VI_PIPE ViPipe, HI_U8 u8Index);

HI_S32 HI_SCENE_SetStaticStatisticsCfg_AutoGenerare(VI_PIPE ViPipe, HI_U8 u8Index, HI_BOOL bMetryFixed);

HI_S32 HI_SCENE_SetStaticThreeDNR_AutoGenerate(VI_PIPE ViPipe, HI_U8 u8Index);

HI_S32 HI_SCENE_SetDynamicPhotoGamma_AutoGenerate(VI_PIPE ViPipe, HI_U64 u64Exposure, HI_U64 u64LastExposure, HI_U8 u8Index);

HI_S32 HI_SCENE_SetDynamicVideoGamma_AutoGenerate(VI_PIPE ViPipe, HI_U64 u64Exposure, HI_U64 u64LastExposure, HI_U8 u8Index);

HI_S32 HI_SCENE_SetDynamicAE_AutoGenerate(VI_PIPE ViPipe, HI_U64 u64Exposure, HI_U64 u64LastExposure, HI_U8 u8Index);

HI_S32 HI_SCENE_SetDynamicShading_AutoGenerate(VI_PIPE ViPipe, HI_U64 u64ISO, HI_U64 u64LastISO, HI_U8 u8Index);

HI_S32 HI_SCENE_SetDynamicLDCI_AutoGenerate(VI_PIPE ViPipe, HI_U64 u64Exposure, HI_U64 u64LastExposure, HI_U8 u8Index);

HI_S32 HI_SCENE_SetDynamicFalseColor_AutoGenerate(VI_PIPE ViPipe, HI_U64 u64Exposure, HI_U64 u64LastExposure, HI_U8 u8Index);

HI_S32 HI_SCENE_SetDynamicDehaze_AutoGenerate(VI_PIPE ViPipe, HI_U64 u64Exposure, HI_U64 u64LastExposure, HI_U8 u8Index);

HI_S32 HI_SCENE_SetDynamicFsWdr_AutoGenerate(VI_PIPE ViPipe, HI_U32 u32ISO, HI_U32 u32LastISO, HI_U8 u8Index, HI_U32 u32ActRation);

HI_S32 HI_SCENE_SetDynamicDemosaic_AutoGenerate(VI_PIPE ViPipe, HI_U64 u64Exposure, HI_U64 u64LastExposure, HI_U8 u8Index);

HI_S32 HI_SCENE_SetDynamicDIS_AutoGenerate(VI_PIPE ViPipe, VI_CHN ViChn, HI_U32 u32Iso, HI_U32 u32LastIso, HI_U8 u8Index, HI_BOOL bEnable);

HI_S32 HI_SCENE_SetDynamicHDROTEF_AutoGenerate(VI_PIPE ViPipe, VPSS_GRP VpssGrp, VPSS_CHN VpssChn, HI_U32 u32HDRBrightRatio, HI_U8 u8Index);

HI_S32 HI_SCENE_SetDynamicHDRTM_AutoGenerate(VI_PIPE ViPipe, VPSS_GRP VpssGrp, VPSS_CHN VpssChn, HI_U32 u32HDRBrightRatio, HI_U8 u8Index);

HI_S32 HI_SCENE_SetDynamicHDRDRC_AutoGenerate(VI_PIPE ViPipe, VPSS_GRP VpssGrp, VPSS_CHN VpssChn, HI_U32 u32HDRBrightRatio, HI_U8 u8Index);

HI_S32 HI_SCENE_SetThreadDRC_AutoGenerate(VI_PIPE ViPipe, HI_U8 u8Index, HI_U32 u32ActRation, HI_U32 u32Iso, HI_U64 u64Exposure, HI_U64 u64LastExposure);

HI_S32 HI_SCENE_GetModuleState_AutoGenerate(HI_SCENE_MODULE_STATE_S *pstModuleState);

HI_S32 HI_SCENE_SetPipeParam_AutoGenerate(const HI_SCENE_PIPE_PARAM_S *pstScenePipeParam, HI_U32 u32Num);

HI_S32 HI_SCENE_SetPause_AutoGenerate(HI_BOOL bPause);

HI_S32 HI_SCENE_GetPipeAERoute_AutoGenerate(HI_S32 s32PipeIndex, HI_U8 au8AEWeight[][AE_ZONE_COLUMN]);



#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif