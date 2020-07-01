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
#define HI_SCENE_DRC_ISO_MAX_COUNT               (10)
#define HI_SCENE_DRC_EXP_MAX_COUNT               (7)
#define HI_SCENE_DRC_REF_RATIO_MAX_COUNT         (9)
#define HI_SCENE_DRC_RATIO_MAX_COUNT             (6)
#define HI_SCENE_FALSECOLOR_EXPOSURE_MAX_COUNT   (10)
#define HI_SCENE_DIS_ISO_MAX_COUNT               (3)
#define HI_SCENE_LDCI_EXPOSURE_MAX_COUNT         (6)
#define HI_SCENE_FSWDR_ISO_MAX_COUNT             (3)
#define HI_SCENE_DEHAZE_EXPOSURE_MAX_COUNT       (5)
#define HI_SCENE_THREEDNR_MAX_COUNT              (15)

typedef struct hiSCENE_MODULE_STATE_S
{
    HI_BOOL bStaticAE;
    HI_BOOL bStaticAWB;
    HI_BOOL bStaticAWBEx;
    HI_BOOL bStaticCCM;
    HI_BOOL bStaticAERouteEx;
    HI_BOOL bStaticGlobalCac;
    HI_BOOL bStaticWdrExposure;
    HI_BOOL bStaticDehaze;
    HI_BOOL bStaticLdci;
    HI_BOOL bStaticStatistics;
    HI_BOOL bStaticSaturation;
    HI_BOOL bStaticShading;
    HI_BOOL bDynamicAE;
    HI_BOOL bDynamicThreeDNR;
    HI_BOOL bDynamicGamma;
    HI_BOOL bDynamicShading;
    HI_BOOL bDynamicIsoVenc;
    HI_BOOL bDynamicLdci;
    HI_BOOL bDynamicDehaze;
    HI_BOOL bDynamicFSWDR;
    HI_BOOL bStaticSharpen;
    HI_BOOL bStaticDemosaic;
    HI_BOOL bDynamicBayernr;
    HI_BOOL bStaticDetail;
    HI_BOOL bDynamicDIS;
    HI_BOOL bStaticDrc;
    HI_BOOL bDynamicLinearDrc;
    HI_BOOL bDynamicThreadDrc;
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
    HI_U8 u8DRCOpType;
    HI_U16 au16ToneMappingValue[HI_ISP_DRC_TM_NODE_NUM];
    HI_U16 au16ColorCorrectionLut[HI_ISP_DRC_CC_NODE_NUM];
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
    HI_BOOL bISOActEn;
    HI_BOOL bTempActEn;
} HI_SCENE_STATIC_CCM_S;

typedef struct hiSCENE_STATIC_CSC_S
{
} HI_SCENE_STATIC_CSC_S;

typedef struct hiSCENE_STATIC_SHADING_S
{
    HI_BOOL bEnable;
} HI_SCENE_STATIC_SHADING_S;

typedef struct hiSCENE_3DNR_VI_IEy
{
    HI_S32 IES0, IES1, IES2, IES3;
    HI_S32 IEDZ;
} HI_SCENE_3DNR_VI_IEy;

typedef struct hiSCENE_3DNR_VI_SFy
{
    HI_S32 SPN6, SFR;
    HI_S32 SBN6, PBR6;
    HI_S32 SRT0, SRT1, JMODE, DeIdx;
    HI_S32 DeRate, SFR6[3];

    HI_S32 SFS1, SFT1, SBR1;
    HI_S32 SFS2, SFT2, SBR2;
    HI_S32 SFS4, SFT4, SBR4;

    HI_S32 STH1,  SFN1, NRyEn, SFN0;
    HI_S32 STH2,  SFN2, BWSF4, kMode;
    HI_S32 STH3,  SFN3, TriTh;
} HI_SCENE_3DNR_VI_SFy;

typedef struct hiSCENE_DYNAMIC_VENCBITRATE_S
{
    HI_U32 u32IsoThreshCnt;
    HI_U32 au32IsoThreshLtoH[HI_SCENE_SHADING_EXPOSURE_MAX_COUNT];
    HI_U16 au16ManualPercent[HI_SCENE_SHADING_EXPOSURE_MAX_COUNT];
} HI_SCENE_DYNAMIC_VENCBITRATE_S;

typedef struct hiSCENE_3DNR_VI_S
{
    HI_SCENE_3DNR_VI_IEy  IEy;
    HI_SCENE_3DNR_VI_SFy  SFy;
} HI_SCENE_3DNR_VI_S;


typedef struct hiSCENE_3DNR_VPSS_IEy
{
    HI_S32 IES0, IES1, IES2, IES3;
    HI_S32 IEDZ;
} HI_SCENE_3DNR_VPSS_IEy;

typedef struct hiSCENE_3DNR_VPSS_SFy
{
    HI_S32 SPN6, SFR;
    HI_S32 SBN6, PBR6;
    HI_S32 SRT0, SRT1, JMODE, DeIdx;
    HI_S32 DeRate, SFR6[3];

    HI_S32 SFS1, SFT1, SBR1;
    HI_S32 SFS2, SFT2, SBR2;
    HI_S32 SFS4, SFT4, SBR4;

    HI_S32 STH1,  SFN1, SFN0, NRyEn;
    HI_S32 STH2,  SFN2, BWSF4, kMode;
    HI_S32 STH3,  SFN3, TriTh;

    HI_S32 SBSk[32], SDSk[32];
} HI_SCENE_3DNR_VPSS_SFy;

typedef struct hiSCENE_3DNR_VPSS_MDy
{
    HI_S32 MADZ0,  MAI00, MAI01, MAI02, biPath;
    HI_S32 MADZ1, MAI10, MAI11, MAI12;
    HI_S32 MABR0, MABR1;

    HI_S32 MATH0,  MATE0,  MATW;
    HI_S32 MATH1,  MATE1;
    HI_S32 MASW;
    HI_S32 MABW0,  MABW1;
} HI_SCENE_3DNR_VPSS_MDy;

typedef struct hiSCENE_3DNR_VPSS_TFy
{
    HI_S32 TFS0, TDZ0, TDX0;
    HI_S32 TFS1, TDZ1, TDX1;
    HI_S32 SDZ0, STR0, DZMode0;
    HI_S32 SDZ1, STR1, DZMode1;

    HI_S32 TFR0[7], TSS0,  TSI0;
    HI_S32 TFR1[7], TSS1,  TSI1;

    HI_S32 RFI, tEdge;
    HI_S32 bRef;
} HI_SCENE_3DNR_VPSS_TFy;

typedef struct hiSCENE_3DNR_VPSS_RFs
{
    HI_S32 advMATH, RFDZ;
    HI_S32 RFUI, RFSLP, bRFU;
} HI_SCENE_3DNR_VPSS_RFs;

typedef struct hiSCENE_3DNR_VPSS_NRc
{
    HI_SCENE_3DNR_VPSS_IEy  IEy;
    HI_SCENE_3DNR_VPSS_SFy  SFy;
    HI_SCENE_3DNR_VPSS_TFy  TFy;
    HI_SCENE_3DNR_VPSS_MDy  MDy;

    HI_U8 NRcEn : 1;
} HI_SCENE_3DNR_VPSS_NRc;

typedef struct hiSCENE_3DNR_VPSS_pNRc
{
    HI_S32 SFC, TFC;
    HI_S32 CTFS;
} HI_SCENE_3DNR_VPSS_pNRc;

typedef struct hiSCENE_3DNR_VPSS_GMC
{
    HI_S32 GMEMode;
} HI_SCENE_3DNR_VPSS_GMC;

typedef struct hiSCENE_3DNR_VPSS_S
{
    HI_SCENE_3DNR_VPSS_IEy  IEy[3];
    HI_SCENE_3DNR_VPSS_GMC  GMC;
    HI_SCENE_3DNR_VPSS_SFy  SFy[3];
    HI_SCENE_3DNR_VPSS_MDy  MDy[2];
    HI_SCENE_3DNR_VPSS_RFs  RFs;
    HI_SCENE_3DNR_VPSS_TFy  TFy[2];
    HI_SCENE_3DNR_VPSS_pNRc pNRc;
    HI_SCENE_3DNR_VPSS_NRc  NRc;

} HI_SCENE_3DNR_VPSS_S;

typedef struct hiSCENE_DYNAMIC_THREEDNR_S
{
    HI_U32 u32ThreeDNRCount;
    HI_U16 u16VI_3DNRStartPoint;
    HI_U32 au32ThreeDNRIso[HI_SCENE_THREEDNR_MAX_COUNT];
    HI_SCENE_3DNR_VI_S astThreeDNRVIValue[HI_SCENE_THREEDNR_MAX_COUNT];
    HI_SCENE_3DNR_VPSS_S astThreeDNRVPSSValue[HI_SCENE_THREEDNR_MAX_COUNT];
} HI_SCENE_DYNAMIC_THREEDNR_S;

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
    HI_U32 au32ISOModeChange[HI_SCENE_FSWDR_ISO_MAX_COUNT];
    HI_U8 au8MergeMode[HI_SCENE_FSWDR_ISO_MAX_COUNT];
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

typedef struct hiSCENE_STATIC_SHARPEN_S
{
    HI_U8  au8OverShoot[ISP_AUTO_ISO_STRENGTH_NUM];
    HI_U8  au8UnderShoot[ISP_AUTO_ISO_STRENGTH_NUM];
    HI_U16 au16TextureStr[ISP_AUTO_ISO_STRENGTH_NUM][ISP_SHARPEN_GAIN_NUM];
    HI_U16 au16EdgeStr[ISP_AUTO_ISO_STRENGTH_NUM][ISP_SHARPEN_GAIN_NUM];
} HI_SCENE_STATIC_SHARPEN_S;

typedef struct hiSCENE_STATIC_DEMOSAIC_S
{
    HI_U8  au8NonDirMFDetailEhcStr[ISP_AUTO_ISO_STRENGTH_NUM];
} HI_SCENE_STATIC_DEMOSAIC_S;

typedef struct hiSCENE_DYNAMIC_BAYERNR_S
{
    HI_U32 u32IsoCount;
    HI_U32 au32IsoLevel[ISP_AUTO_ISO_STRENGTH_NUM];
    HI_U16 au16CoringWgt[ISP_AUTO_ISO_STRENGTH_NUM];
    HI_U8  au8FineStr[ISP_AUTO_ISO_STRENGTH_NUM];
    HI_U16 au16CoringRatio[ISP_AUTO_ISO_STRENGTH_NUM][HI_ISP_BAYERNR_LUT_LENGTH];
} HI_SCENE_DYNAMIC_BAYERNR_S;

typedef struct hiSCENE_STATIC_DETAIL_S
{
    HI_U16 au16GlobalGain[ISP_AUTO_ISO_STRENGTH_NUM];
} HI_SCENE_STATIC_DETAIL_S;

typedef struct hiSCENE_DYNAMIC_FALSECOLOR_S
{
} HI_SCENE_DYNAMIC_FALSECOLOR_S;

typedef struct hiSCENE_DYNAMIC_DIS_S
{
} HI_SCENE_DYNAMIC_DIS_S;

typedef struct hiSCENE_DYNAMIC_LINEARDRC_S
{
    HI_BOOL bEnable;
    HI_U32 u32ISOCount;
    HI_U32 au32ISOLevel[HI_SCENE_DRC_ISO_MAX_COUNT];
    HI_U8  au8LocalMixingBrightMax[HI_SCENE_DRC_ISO_MAX_COUNT];
    HI_U8  au8LocalMixingBrightMin[HI_SCENE_DRC_ISO_MAX_COUNT];
    HI_U8  au8LocalMixingDarkMax[HI_SCENE_DRC_ISO_MAX_COUNT];
    HI_U8  au8LocalMixingDarkMin[HI_SCENE_DRC_ISO_MAX_COUNT];
    HI_U8  au8BrightGainLmt[HI_SCENE_DRC_ISO_MAX_COUNT];
    HI_U8  au8BrightGainLmtStep[HI_SCENE_DRC_ISO_MAX_COUNT];
    HI_U8  au8DarkGainLmtY[HI_SCENE_DRC_ISO_MAX_COUNT];
    HI_U8  au8DarkGainLmtC[HI_SCENE_DRC_ISO_MAX_COUNT];
    HI_U8  au8ContrastControl[HI_SCENE_DRC_ISO_MAX_COUNT];
    HI_S8  as8DetailAdjustFactor[HI_SCENE_DRC_ISO_MAX_COUNT];
    HI_U8  au8Asymmetry[HI_SCENE_DRC_ISO_MAX_COUNT];
    HI_U8  au8SecondPole[HI_SCENE_DRC_ISO_MAX_COUNT];
    HI_U8  au8Compress[HI_SCENE_DRC_ISO_MAX_COUNT];
    HI_U8  au8Stretch[HI_SCENE_DRC_ISO_MAX_COUNT];
    HI_U16 au16Strength[HI_SCENE_DRC_ISO_MAX_COUNT];
} HI_SCENE_DYNAMIC_LINEARDRC_S;

typedef struct hiSCENE_THREAD_DRC_S
{
    HI_U32 u32RationCount;
    HI_U32 au32RationLevel[HI_SCENE_DRC_RATIO_MAX_COUNT];
    HI_U32 u32ISOCount;
    HI_U32 au32ISOLevel[HI_SCENE_DRC_ISO_MAX_COUNT];
    HI_U32 u32Interval;
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
    HI_SCENE_STATIC_AWB_S stStaticAwb;
    HI_SCENE_STATIC_AWBEX_S stStaticAwbEx;
    HI_SCENE_STATIC_CCM_S stStaticCcm;
    HI_SCENE_STATIC_CSC_S stStaticCsc;
    HI_SCENE_STATIC_SHADING_S stStaticShading;
    HI_SCENE_STATIC_SATURATION_S stStaticSaturation;
    HI_SCENE_STATIC_SHARPEN_S stStaticSharpen;
    HI_SCENE_STATIC_DEMOSAIC_S stStaticDemosaic;
    HI_SCENE_STATIC_DETAIL_S stStaticDetail;
    HI_SCENE_DYNAMIC_THREEDNR_S stDynamicThreeDNR;
    HI_SCENE_DYNAMIC_AE_S stDynamicAe;
    HI_SCENE_DYNAMIC_SHADING_S stDynamicShading;
    HI_SCENE_DYNAMIC_VENCBITRATE_S stDynamicVencBitrate;
    HI_SCENE_DYNAMIC_LDCI_S stDynamicLDCI;
    HI_SCENE_DYNAMIC_DEHAZE_S stDynamicDehaze;
    HI_SCENE_DYNAMIC_FSWDR_S stDynamicFSWDR;
    HI_SCENE_DYNAMIC_GAMMA_S stDynamicGamma;
    HI_SCENE_DYNAMIC_BAYERNR_S stDynamicBayernr;
    HI_SCENE_DYNAMIC_FALSECOLOR_S stDynamicFalsecolor;
	HI_SCENE_DYNAMIC_DIS_S stDynamicDis;
    HI_SCENE_DYNAMIC_LINEARDRC_S stDynamicLinearDrc;
    HI_SCENE_THREAD_DRC_S stThreadDrc;
    HI_SCENE_MODULE_STATE_S stModuleState;
} HI_SCENE_PIPE_PARAM_S;

HI_S32 HI_SCENE_SetStaticAE_AutoGenerate(VI_PIPE ViPipe, HI_U8 u8Index);

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

HI_S32 HI_SCENE_SetStaticShading_AutoGenerate(VI_PIPE ViPipe, HI_U8 u8Index);

HI_S32 HI_SCENE_SetStaticStatisticsCfg_AutoGenerate(VI_PIPE ViPipe, HI_U8 u8Index, HI_BOOL bMetryFixed);

HI_S32 HI_SCENE_SetDynamicThreeDNR_AutoGenerate(VI_PIPE ViPipe, VPSS_GRP VpssGrp, HI_U32 u32ISO, HI_U32 u32LastISO, HI_U8 u8Index);

HI_S32 HI_SCENE_SetDynamicPhotoGamma_AutoGenerate(VI_PIPE ViPipe, HI_U64 u64Exposure, HI_U64 u64LastExposure, HI_U8 u8Index);

HI_S32 HI_SCENE_SetDynamicVideoGamma_AutoGenerate(VI_PIPE ViPipe, HI_U64 u64Exposure, HI_U64 u64LastExposure, HI_U8 u8Index);

HI_S32 HI_SCENE_SetDynamicAE_AutoGenerate(VI_PIPE ViPipe, HI_U64 u64Exposure, HI_U64 u64LastExposure, HI_U8 u8Index);

HI_S32 HI_SCENE_SetDynamicShading_AutoGenerate(VI_PIPE ViPipe, HI_U64 u64ISO, HI_U64 u64LastISO, HI_U8 u8Index);

HI_S32 HI_SCENE_SetDynamicVencBitrate_AutoGenerate(VI_PIPE ViPipe, HI_U64 u64ISO, HI_U64 u64LastISO, HI_U8 u8Index);

HI_S32 HI_SCENE_SetDynamicLDCI_AutoGenerate(VI_PIPE ViPipe, HI_U64 u64Exposure, HI_U64 u64LastExposure, HI_U8 u8Index);

HI_S32 HI_SCENE_SetDynamicDehaze_AutoGenerate(VI_PIPE ViPipe, HI_U64 u64Exposure, HI_U64 u64LastExposure, HI_U8 u8Index);

HI_S32 HI_SCENE_SetDynamicFsWdr_AutoGenerate(VI_PIPE ViPipe, HI_U32 u32ISO, HI_U32 u32LastISO, HI_U8 u8Index, HI_U32 u32ActRation);

HI_S32 HI_SCENE_SetStaticSharpen_AutoGenerate(VI_PIPE ViPipe, HI_U8 u8Index);

HI_S32 HI_SCENE_SetStaticDemosaic_AutoGenerate(VI_PIPE ViPipe, HI_U8 u8Index);

HI_S32 HI_SCENE_SetDynamicBayernr_AutoGenerate(VI_PIPE ViPipe, HI_U32 u32ISO, HI_U32 u32LastISO, HI_U8 u8Index);

HI_S32 HI_SCENE_SetStaticDetail_AutoGenerate(VI_PIPE ViPipe, HI_U8 u8Index);
HI_S32 HI_SCENE_SetDynamicDIS_AutoGenerate(VI_PIPE ViPipe, VI_CHN ViChn, HI_U32 u32Iso, HI_U32 u32LastIso, HI_U8 u8Index, HI_BOOL bEnable);

HI_S32 HI_SCENE_SetDynamicLinearDRC_AutoGenerate(VI_PIPE ViPipe, HI_U32 u32Iso, HI_U32 u32LastIso, HI_U8 u8Index);

HI_S32 HI_SCENE_SetDynamicThreadDRC_AutoGenerate(VI_PIPE ViPipe, HI_U8 u8Index, HI_U32 u32ActRation, HI_U32 u32Iso, HI_U64 u64Exposure, HI_U64 u64LastExposure);

HI_S32 HI_SCENE_SetPipeParam_AutoGenerate(const HI_SCENE_PIPE_PARAM_S *pstScenePipeParam, HI_U32 u32Num);

HI_S32 HI_SCENE_SetPause_AutoGenerate(HI_BOOL bPause);

HI_S32 HI_SCENE_GetPipeAERoute_AutoGenerate(HI_S32 s32PipeIndex, HI_U8 au8AEWeight[][AE_ZONE_COLUMN]);

HI_S32 HI_SCENE_SetRCParam_AutoGenerate(VI_PIPE ViPipe, HI_U8 u8Index);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif
