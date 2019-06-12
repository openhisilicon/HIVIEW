#ifndef _HI_SCENEAUTO_DEFINE_H_
#define _HI_SCENEAUTO_DEFINE_H_

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

#define EXPOSURE_LEVEL	16
#define AE_WEIGHT_ROW       15
#define AE_WEIGHT_COLUMN    17

typedef struct hiSCENEAUTO_INIPARAM_MPINFO_S
{
    HI_S32 s32IspDev;
    HI_S32 s32ViDev;
    HI_S32 s32ViChn;
    HI_S32 s32VpssGrp;
    HI_S32 s32VpssChn;
    HI_S32 s32VencGrp;
    HI_S32 s32VencChn;
} SCENEAUTO_INIPARAM_MPINFO_S;

typedef struct hiSCENEAUTO_DEMOSAIC_S
{
    HI_U8   u8VhSlope;
    HI_U8   u8AaSlope;
    HI_U8   u8VaSlope;
    HI_U8   u8UuSlope;
    HI_U8   au8LumThresh[16];
} SCENEAUTO_DEMOSAIC_S;

typedef struct hiSCENEAUTO_AERELATEDBIT_S
{
    HI_U8  u8Speed;
    HI_U8  u8Tolerance;
    HI_U16 u16BlackDelayFrame;
    HI_U16 u16WhiteDelayFrame;
    HI_U32 u32SysGainMax;
} SCENEAUTO_AERELATEDBIT_S;

typedef struct hiSCENEAUTO_AERELATEDEXP_S
{
    HI_U8 u8AECompesation;
    HI_U8 u8AEHistOffset;
} SCENEAUTO_AERELATEDEXP_S;

typedef struct hiSCENEAUTO_SHARPEN_S
{
    HI_U8 au8SharpenD[EXPOSURE_LEVEL];
    HI_U8 au8SharpenUd[EXPOSURE_LEVEL];
    HI_U8 au8SharpenRGB[EXPOSURE_LEVEL];
} SCENEAUTO_SHARPEN_S;

typedef struct hiSCENEAUTO_DEPATTR_S
{
    HI_U16 u16Slope;
} SCENEAUTO_DEPATTR_S;

typedef struct hiSCENEAUTO_GAMMA_S
{
    HI_U8 u8CurveType;
    HI_U16 u16Table[257];
} SCENEAUTO_GAMMA_S;

typedef struct hiSCENEAUTO_ACM_S
{
    HI_BOOL bEnable;
    HI_U32 u32CbcrThr;
    HI_U32 u32GainLuma;
    HI_U32 u32GainHue;
    HI_U32 u32GainSat;
} SCENEAUTO_ACM_S;

typedef struct hiSCENEAUTO_CCM_S
{
    HI_U16 u16HighColorTemp;
    HI_U16 au16HighCCM[9];
    HI_U16 u16MidColorTemp;
    HI_U16 au16MidCCM[9];
    HI_U16 u16LowColorTemp;
    HI_U16 au16LowCCM[9];
} SCENEAUTO_CCM_S;

typedef struct hiSCENEAUTO_H264DBLK_S
{
    HI_U32 disable_deblocking_filter_idc;
    HI_S32 slice_alpha_c0_offset_div2;
    HI_S32 slice_beta_offset_div2;
} SCENEAUTO_H264DBLK_S;


typedef struct hiSCENEAUTO_H264VENC_S
{
    HI_U32 u32ThrdI[12];
    HI_U32 u32ThrdP[12];
    HI_U32 u32DeltaQP;
    HI_S32 s32IPQPDelta;
    HI_S32 s32chroma_qp_index_offset;
    SCENEAUTO_H264DBLK_S stH264Dblk;
} SCENEAUTO_H264VENC_S;



typedef struct hiSCENEAUTO_ROUTE_NODE_S
{
    HI_U32  u32IntTime;
    HI_U32  u32SysGain;
} SCENEAUTO_ROUTE_NODE_S;

typedef struct hiSCENEAUTO_INIPARAM_DRC_S
{
    HI_BOOL bDRCEnable;
    HI_BOOL bDRCManulEnable;
    HI_U32 u32Strength;
    HI_U32 u32SlopeMax;
    HI_U32 u32SlopeMin;
    HI_U32 u32VarianceSpace;
    HI_U32 u32VarianceIntensity;
    HI_U32 u32WhiteLevel;
    HI_U32 u32BlackLevel;
} SCENEAUTO_INIPARAM_DRC_S;
typedef struct hiSCENEAUTO_INIPARAM_DYNAMIC_S
{
    HI_S32 s32TotalNum;
    SCENEAUTO_ROUTE_NODE_S* pstRouteNode;
} SCENEAUTO_INIPARAM_DYNAMIC_S;

typedef struct hiSCENEAUTO_INIPARAM_BLC_S
{
    HI_U8 u8AEStrategyMode;
    HI_U8 u8MaxHistOffset;
    HI_U16 u16HistRatioSlope;
} SCENEAUTO_INIPARAM_BLC_S;

typedef struct hiSCENEAUTO_INIPARAM_3DNRCFG_S
{
    HI_U8 u8SBS[3];
    HI_U8 u8SBT[3];
    HI_U8 u8SDS[3];
    HI_U8 u8SDT[3];
    HI_U8 u8SFC;
    HI_U8 u8SHP[3];
    HI_U8 u8TFC;
    HI_U16 u16PSFS;
    HI_U16 u16POST;
    HI_U16 u16TFS[3];
    HI_U16 u16TFR[3];
    HI_U16 u16SBF[3];
    HI_U16 u16MDZ[3];
    HI_U8 u8MABW;
    HI_U8 u8PostROW;
    HI_U8 u8MATW;
    HI_U8 u8MDAF;
    HI_U8 u8TextThr;
    HI_U8 u8MTFS;
    HI_U8 u8ExTfThr;
}SCENEAUTO_INIPARAM_3DNRCFG_S;
typedef struct hiSCENEAUTO_INIPARAM_IR_S
{
    HI_S32 s32ExpCount;
    HI_U32* pu32ExpThreshLtoH;
    HI_U32* pu32ExpThreshHtoL;
    HI_U8* pu8ExpCompensation;
    HI_U8* pu8MaxHistOffset;

    HI_U16 u16HistRatioSlope;
    HI_U16 u16BlackDelayFrame;
    HI_U16 u16WhiteDelayFrame;
    HI_U8 u8Speed;
    HI_U8 u8Tolerance;
    HI_BOOL bDCIEnable;
    HI_U32 u32DCIBlackGain;
    HI_U32 u32DCIContrastGain;
    HI_U32 u32DCILightGain;
    HI_U16 u16Slope;
    HI_U8 au8LumThresh[16];
    HI_U8 au8SharpenD[16];
    HI_U8 au8SharpenUd[16];
    HI_U8 au8SharpenRGB[16];
    HI_U16 u16GammaTable[257];
    HI_U8 au8Weight[AE_WEIGHT_ROW][AE_WEIGHT_COLUMN];    
    HI_S32 s323DnrIsoCount;
    HI_U32 *pu323DnrIsoThresh;
    SCENEAUTO_INIPARAM_3DNRCFG_S *pst3dnrParam; 
} SCENEAUTO_INIPARAM_IR_S;

typedef struct hiSCENEAUTO_INIPARAM_HLC_S
{
    HI_U8 u8ExpCompensation;
    HI_U8 u8Saturation[16];
    HI_U16 u16BlackDelayFrame;
    HI_U16 u16WhiteDelayFrame;
    HI_U8 u8Speed;
    HI_U16 u16HistRatioSlope;
    HI_U8 u8MaxHistOffset;
    HI_U8 u8Tolerance;
    HI_BOOL bDCIEnable;
    HI_U32 u32DCIBlackGain;
    HI_U32 u32DCIContrastGain;
    HI_U32 u32DCILightGain;
    HI_BOOL bDRCEnable;
    HI_BOOL bDRCManulEnable;
    HI_U32 u32DRCStrengthTarget;
    HI_U16 u16GammaTable[257];
    HI_U8 au8SharpenUd[16];
    HI_U8 au8SharpenRGB[16];
    HI_S32 s323DnrIsoCount;
    HI_U32 *pu323DnrIsoThresh;
    SCENEAUTO_INIPARAM_3DNRCFG_S *pst3dnrParam; 
} SCENEAUTO_INIPARAM_HLC_S;

typedef struct hiSCENEAUTO_INIPARAM_3DNR_S
{
    HI_S32 s323DnrIsoCount;
    HI_U32 *pu323DnrIsoThresh;
    SCENEAUTO_INIPARAM_3DNRCFG_S *pst3dnrParam;
} SCENEAUTO_INIPARAM_3DNR_S;

typedef struct hiSCENEAUTO_H265VENC_FACECFG_S
{
    HI_U8 u8NormIntra4RdCost_I;
    HI_U8 u8NormIntra8RdCost_I;
    HI_U8 u8NormIntra16RdCost_I;
    HI_U8 u8NormIntra32RdCost_I;
    HI_U8 u8SkinIntra4RdCost_I;
    HI_U8 u8SkinIntra8RdCost_I;
    HI_U8 u8SkinIntra16RdCost_I;
    HI_U8 u8SkinIntra32RdCost_I;
    HI_U8 u8HedgeIntra4RdCost_I;
    HI_U8 u8HedgeIntra8RdCost_I;
    HI_U8 u8HedgeIntra16RdCost_I;
    HI_U8 u8HedgeIntra32RdCost_I;
    HI_U8 u8NormIntra4RdCost_P;
    HI_U8 u8NormIntra8RdCost_P;
    HI_U8 u8NormIntra16RdCost_P;
    HI_U8 u8NormIntra32RdCost_P;
    HI_U8 u8SkinIntra4RdCost_P;
    HI_U8 u8SkinIntra8RdCost_P;
    HI_U8 u8SkinIntra16RdCost_P;
    HI_U8 u8SkinIntra32RdCost_P;
    HI_U8 u8HedgeIntra4RdCost_P;
    HI_U8 u8HedgeIntra8RdCost_P;
    HI_U8 u8HedgeIntra16RdCost_P;
    HI_U8 u8HedgeIntra32RdCost_P;
    HI_U8 u8NormFme8RdCost_P;
    HI_U8 u8NormFme16RdCost_P;
    HI_U8 u8NormFme32RdCost_P;
    HI_U8 u8NormFme64RdCost_P;
    HI_U8 u8SkinFme8RdCost_P;
    HI_U8 u8SkinFme16RdCost_P;
    HI_U8 u8SkinFme32RdCost_P;
    HI_U8 u8SkinFme64RdCost_P;
    HI_U8 u8HedgeFme8RdCost_P;
    HI_U8 u8HedgeFme16RdCost_P;
    HI_U8 u8HedgeFme32RdCost_P;
    HI_U8 u8HedgeFme64RdCost_P;
    HI_U8 u8NormMerg8RdCost_P;
    HI_U8 u8NormMerg16RdCost_P;
    HI_U8 u8NormMerg32RdCost_P;
    HI_U8 u8NormMerg64RdCost_P;
    HI_U8 u8SkinMerg8RdCost_P;
    HI_U8 u8SkinMerg16RdCost_P;
    HI_U8 u8SkinMerg32RdCost_P;
    HI_U8 u8SkinMerg64RdCost_P;
    HI_U8 u8HedgeMerg8RdCost_P;
    HI_U8 u8HedgeMerg16RdCost_P;
    HI_U8 u8HedgeMerg32RdCost_P;
    HI_U8 u8HedgeMerg64RdCost_P;
    HI_BOOL bSkinEn_I;
    HI_U32 u32SkinQpDelta_I;
    HI_U8 u8SkinUMax_I;
    HI_U8 u8SkinUMin_I;
    HI_U8 u8SkinVMax_I;
    HI_U8 u8SkinVMin_I;
    HI_U32 u32SkinNum_I;
    HI_BOOL bSkinEn_P;
    HI_U32 u32SkinQpDelta_P;
    HI_U8 u8SkinUMax_P;
    HI_U8 u8SkinUMin_P;
    HI_U8 u8SkinVMax_P;
    HI_U8 u8SkinVMin_P;
    HI_U32 u32SkinNum_P;
    HI_U8 u8HedgeThr_I;
    HI_U8 u8HedgeCnt_I;
    HI_BOOL bStroEdgeEn_I;
    HI_U32 u32StroEdgeQpDelta_I;
    HI_U8 u8HedgeThr_P;
    HI_U8 u8HedgeCnt_P;
    HI_BOOL bStroEdgeEn_P;
    HI_U32 u32StroEdgeQpDelta_P;
    HI_BOOL bImproveEn_I;
    HI_BOOL bImproveEn_P;
    HI_U32 u32Norm32MaxNum_P;
    HI_U32 u32Norm16MaxNum_P;
    HI_U32 u32Norm32ProtectNum_P;
    HI_U32 u32Norm16ProtectNum_P;
    HI_U32 u32Skin32MaxNum_P;
    HI_U32 u32Skin16MaxNum_P;
    HI_U32 u32Skin32ProtectNum_P;
    HI_U32 u32Skin16ProtectNum_P;
    HI_U32 u32Still32MaxNum_P;
    HI_U32 u32Still16MaxNum_P;
    HI_U32 u32Still32ProtectNum_P;
    HI_U32 u32Still16ProtectNum_P;
    HI_U32 u32Hedge32MaxNum_P;
    HI_U32 u32Hedge16MaxNum_P;
    HI_U32 u32Hedge32ProtectNum_P;
    HI_U32 u32Hedge16ProtectNum_P;
} SCENEAUTO_H265VENC_FACECFG_S;
typedef struct hiSCENEAUTO_H265VENC_RCPARAM_S
{
    HI_U32 u32ThrdI[12];
    HI_U32 u32ThrdP[12];
    HI_U32 u32DeltaQP;
    HI_S32 s32IPQPDelta;
} SCENEAUTO_H265VENC_RCPARAM_S;
typedef struct hiSCENEAUTO_INIPARAM_H265VENC_FACECFG_S
{
    HI_S32 s32BitrateCount;
    HI_U32* pu32BitrateThresh;
    HI_S32 s32ExpCount;
    HI_U32* pu32ExpThresh;
    SCENEAUTO_H265VENC_FACECFG_S* pstH265VencFaceCfg;
} SCENEAUTO_INIPARAM_H265VENC_FACECFG_S;
typedef struct hiSCENEAUTO_INIPARAM_H265VENC_RCPARAM_S
{
    HI_S32 s32BitrateCount;
    HI_U32* pu32BitrateThresh;
    SCENEAUTO_H265VENC_RCPARAM_S* pstH265VencRcParam;
} SCENEAUTO_INIPARAM_H265VENC_RCPARAM_S;
typedef struct hiSCENEAUTO_INIPARAM_H265VENC_S
{
    SCENEAUTO_INIPARAM_H265VENC_RCPARAM_S stIniH265VencRcParam;
    SCENEAUTO_INIPARAM_H265VENC_FACECFG_S stIniH265VencFaceCfg;
} SCENEAUTO_INIPARAM_H265VENC_S;
typedef struct hiSCENEAUTO_H265VENC_S
{
    SCENEAUTO_H265VENC_RCPARAM_S stH265VencRcParam;
    SCENEAUTO_H265VENC_FACECFG_S stH265VencFaceCfg;
} SCENEAUTO_H265VENC_S;

typedef struct hiSCENEAUTO_INIPARAM_H264VENC_S
{
    HI_S32 s32BitrateCount;
    HI_U32* pu32BitrateThresh;
    SCENEAUTO_H264VENC_S* pstH264Venc;
} SCENEAUTO_INIPARAM_H264VENC_S;

typedef struct hiSCENEAUTO_INIPARAM_GAMMA_S
{
    HI_S32 s32ExpCount;
    HI_U32 u32DelayCount;
    HI_S32 s32Interval;
    HI_U32* pu32ExpThreshLtoD;
    HI_U32* pu32ExpThreshDtoL;
    SCENEAUTO_GAMMA_S* pstGamma;
} SCENEAUTO_INIPARAM_GAMMA_S;

typedef struct hiSCENEAUTO_INIPARAM_DP_S
{
    HI_S32 s32ExpCount;
    HI_U32* pu32ExpThresh;
    SCENEAUTO_DEPATTR_S* pstDPAttr;
} SCENEAUTO_INIPARAM_DP_S;

typedef struct hiSCENEAUTO_INIPARAM_SHARPEN_S
{
    HI_S32 s32BitrateCount;
    HI_U32* pu32BitrateThresh;
    SCENEAUTO_SHARPEN_S* pstSharpen;
} SCENEAUTO_INIPARAM_SHARPEN_S;

typedef struct hiSCENEAUTO_INIPARAM_DEMOSAIC_S
{
    HI_S32 s32BitrateCount;
    HI_U32* pu32BitrateThresh;
    HI_S32 s32ExpCount;
    HI_U32* pu32ExpThresh;
    SCENEAUTO_DEMOSAIC_S* pstDemosaic;
} SCENEAUTO_INIPARAM_DEMOSAIC_S;

typedef struct hiSCENEAUTO_INIPARAM_AE_S
{
    HI_U8 u8AERunInterval;
    HI_S32 s32BitrateCount;
    HI_U32* pu32BitrateThresh;
    SCENEAUTO_AERELATEDBIT_S* pstAERelatedBit;
    HI_S32 s32ExpCount;
    HI_U32* pu32AEExpDtoLThresh;
    HI_U32* pu32AEExpLtoDThresh;
    SCENEAUTO_AERELATEDEXP_S* pstAERelatedExp;
} SCENEAUTO_INIPARAM_AE_S;

typedef struct hiSCENEAUTO_INIPARAM_NRPROFILE_S
{
    HI_U8 au8NpDefault1[128];
    HI_U8 au8NpDefault2[128];
    HI_U8 au8Np1[128];
    HI_U8 au8Np2[128];
    HI_U32 u32ExpLow;
    HI_U32 u32ExpHigh;
} SCENEAUTO_INIPARAM_NRPROFILE_S;

typedef struct hiSCENEAUTO_INIPARAM_THRESHVALUE_S
{
    HI_BOOL bIVEEnable;
    HI_BOOL bHLCAutoEnable;
    HI_U32 u32HLCOnThresh;
    HI_U32 u32HLCOffThresh;
    HI_U32 u32HLCTolerance;
    HI_U32 u32HLCExpThresh;
    HI_U32 u32HLCCount;
    HI_U32 u32AveLumThresh;
    HI_U32 u32DeltaDisExpThreash;
    HI_U32 u32FpnExpThresh;
    HI_U32 u32DRCStrengthThresh;
} SCENEAUTO_INIPARAM_THRESHVALUE_S;

typedef struct hiSCENEAUTO_INIPARA_S
{
    HI_U8 au8DciStrengthLut[111];
    SCENEAUTO_INIPARAM_MPINFO_S stMpInfo;
    SCENEAUTO_INIPARAM_THRESHVALUE_S stThreshValue;
    SCENEAUTO_INIPARAM_NRPROFILE_S stIniNRProfile;
    SCENEAUTO_INIPARAM_AE_S stIniAE;
    SCENEAUTO_INIPARAM_DEMOSAIC_S stIniDemosaic;
    SCENEAUTO_INIPARAM_SHARPEN_S stIniSharpen;
    SCENEAUTO_INIPARAM_DP_S stIniDP;
    SCENEAUTO_INIPARAM_GAMMA_S stIniGamma;
    SCENEAUTO_INIPARAM_H264VENC_S stIniH264Venc;
    SCENEAUTO_INIPARAM_H265VENC_S stIniH265Venc;
    SCENEAUTO_INIPARAM_3DNR_S stIni3dnr;
    SCENEAUTO_INIPARAM_HLC_S stHLC;
    SCENEAUTO_INIPARAM_IR_S stIR;
    SCENEAUTO_INIPARAM_BLC_S stBLC;
    SCENEAUTO_INIPARAM_DYNAMIC_S stFastDynamic;
    SCENEAUTO_INIPARAM_DYNAMIC_S stNormalDynamic;
    SCENEAUTO_INIPARAM_DRC_S stDRC;
} SCENEAUTO_INIPARA_S;

typedef struct hiSCENEAUTO_AEATTR_S
{
    HI_U8  u8Speed;
    HI_U8  u8Tolerance;
    HI_U16 u16BlackDelayFrame;
    HI_U16 u16WhiteDelayFrame;
    HI_U8 u8ExpCompensation;
    HI_U8 u8AEStrategyMode;
    HI_U8 u8MaxHistOffset;
    HI_U16 u16HistRatioSlope;
    HI_U32 u32SysGainMax;
    HI_U8 u8AERunInterval;
	HI_U8 au8AeWeight[AE_WEIGHT_ROW][AE_WEIGHT_COLUMN];
} SCENEAUTO_AEATTR_S;

typedef struct hiSCENEAUTO_DIS_S
{
    HI_BOOL bEnable;
} SCENEAUTO_DIS_S;

typedef struct hiSCENEAUTO_SATURATION_S
{
    HI_U8 u8OpType;
    HI_U8 u8ManualSat;
    HI_U8 au8AutoSat[16];
} SCENEAUTO_SATURATION_S;

typedef struct hiSCENEAUTO_DRCATTR_S
{
    HI_BOOL bEnable;
    HI_BOOL bManulEnable;
    HI_S32 s32Strength;
    HI_U32 u32SlopeMax;
    HI_U32 u32SlopeMin;
    HI_U32 u32VarianceSpace;
    HI_U32 u32VarianceIntensity;
    HI_U32 u32WhiteLevel;
    HI_U32 u32BlackLevel;
} SCENEAUTO_DRCATTR_S;

typedef struct hiSCENEAUTO_DCIPARAM_S
{
    HI_BOOL bEanble;
    HI_U32 u32BlackGain;
    HI_U32 u32ContrastGain;
    HI_U32 u32LightGain;
} SCENEAUTO_DCIPARAM_S;

typedef struct hiSCENEAUTO_AEROUTE_NODE_S
{
    HI_U32  u32IntTime;
    HI_U32  u32SysGain;
} SCENEAUTO_AEROUTE_NODE_S;

typedef struct hiSCENEAUTO_AEROUTE_S
{
    HI_U32 u32TotalNum;
    SCENEAUTO_AEROUTE_NODE_S astRouteNode[8];
} SCENEAUTO_AEROUTE_S;

typedef struct hiSCENEAUTO_PREVIOUSPARA_S
{
    SCENEAUTO_DEMOSAIC_S stDemosaic;
    SCENEAUTO_SHARPEN_S stSharpen;
    SCENEAUTO_DEPATTR_S stDP;
    SCENEAUTO_GAMMA_S stGamma;
    SCENEAUTO_CCM_S stCcm;
    SCENEAUTO_ACM_S stAcm;
    SCENEAUTO_H264VENC_S stH264Venc;
    SCENEAUTO_H265VENC_S stH265Venc;
    SCENEAUTO_AEROUTE_S stAERoute;
    SCENEAUTO_DCIPARAM_S stDCIParam;
    SCENEAUTO_DRCATTR_S stDRCAttr;
    SCENEAUTO_SATURATION_S stSaturation;
    SCENEAUTO_DIS_S stDis;
    SCENEAUTO_AEATTR_S stAEAttr;
} SCENEAUTO_PREVIOUSPARA_S;


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif

