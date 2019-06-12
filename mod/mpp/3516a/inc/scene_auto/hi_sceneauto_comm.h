#ifndef __HI_SCENEAUTO_COMM_H__
#define __HI_SCENEAUTO_COMM_H__

#include "hi_type.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

#define AE_WEIGHT_ROW       15
#define AE_WEIGHT_COLUMN    17
typedef enum hiADPT_VENC_RCMODE_E
{
    ADPT_SCENEAUTO_RCMODE_H264 = 0,
    ADPT_SCENEAUTO_RCMODE_H265,

    ADPT_SCENEAUTO_RCMODE_BUTT,
} ADPT_SCENEAUTO_VENC_RCMODE_E;


typedef struct hiADPT_SCENEAUTO_DEMOSAIC_S
{
    HI_U8   u8VhSlope;
    HI_U8   u8AaSlope;
    HI_U8   u8VaSlope;
    HI_U8   u8UuSlope;
    HI_U8   au8LumThresh[16];
} ADPT_SCENEAUTO_DEMOSAIC_S;

typedef struct hiADPT_SCENEAUTO_SHARPEN_S
{
    HI_U8 au8SharpenD[16];
    HI_U8 au8SharpenUd[16];
    HI_U8 au8SharpenRGB[16];
} ADPT_SCENEAUTO_SHARPEN_S;

typedef struct hiADPT_SCENEAUTO_DP_S
{
    HI_U16 u16Slope;
} ADPT_SCENEAUTO_DP_S;

typedef struct hiADPT_SCENEAUTO_PUBATTR_S
{
    HI_U32 u32Frame;
} ADPT_SCENEAUTO_PUBATTR_S;

typedef struct hiADPT_SCENEAUTO_GAMMA_S
{
    HI_U8 u8CurveType;
    HI_U16 au16GammaTable[257];
} ADPT_SCENEAUTO_GAMMA_S;

typedef struct hiADPT_SCENEAUTO_H264TRANS_S
{
    HI_S32 chroma_qp_index_offset;
} ADPT_SCENEAUTO_H264TRANS_S;

typedef struct hiADPT_SCENEAUTO_H264_DEBLOCK_S
{
    HI_U32 disable_deblocking_filter_idc;
    HI_S32 slice_alpha_c0_offset_div2;
    HI_S32 slice_beta_offset_div2;
} ADPT_SCENEAUTO_H264_DEBLOCK_S;

typedef struct hiADPT_SCENEAUTO_H264_RCPARAM_S
{
    HI_U32 u32ThrdI[12];
    HI_U32 u32ThrdP[12];
    HI_U32 u32QpDelta;
    HI_S32 s32IPQPDelta;
} ADPT_SCENEAUTO_H264_RCPARAM_S;

typedef struct hiADPT_SCENEAUTO_AEROUTE_NODE_S
{
    HI_U32  u32IntTime;
    HI_U32  u32SysGain;
} ADPT_SCENEAUTO_AEROUTE_NODE_S;

typedef struct hiADPT_SCENEAUTO_AEROUTE_S
{
    HI_U32 u32TotalNum;
    ADPT_SCENEAUTO_AEROUTE_NODE_S astRouteNode[8];
} ADPT_SCENEAUTO_AEROUTE_S;

typedef struct hiADPT_SCENEAUTO_DCIPARAM_S
{
    HI_BOOL bEnable;
    HI_U32 u32BlackGain;
    HI_U32 u32ContrastGain;
    HI_U32 u32LightGain;
} ADPT_SCENEAUTO_DCIPARAM_S;

typedef struct hiADPT_SCENEAUTO_DRCATTR_S
{
    HI_BOOL bEnable;
    HI_BOOL bManulEnable;
    HI_U32 u32Strength;
    HI_U32 u32SlopeMax;
    HI_U32 u32SlopeMin;
    HI_U32 u32VarianceSpace;
    HI_U32 u32VarianceIntensity;
    HI_U32 u32WhiteLevel;
    HI_U32 u32BlackLevel;
} ADPT_SCENEAUTO_DRCATTR_S;

typedef struct hiADPT_SCENEAUTO_SATURATION_S
{
    HI_U8 u8OpType;
    HI_U8 u8ManualSat;
    HI_U8 au8AutoSat[16];
} ADPT_SCENEAUTO_SATURATION_S;

typedef struct hiADPT_SCENEAUTO_DIS_ATTR_S
{
    HI_BOOL bEnable;
} ADPT_SCENEAUTO_DIS_ATTR_S;

typedef struct hiADPT_SCENEAUTO_AEATTR_S
{
    HI_U8  u8Speed;
    HI_U8  u8Tolerance;
    HI_U8 u8ExpCompensation;
    HI_U16 u16BlackDelayFrame;
    HI_U16 u16WhiteDelayFrame;
    HI_U8 u8AEStrategyMode;
    HI_U8 u8MaxHistOffset;
    HI_U16 u16HistRatioSlope;
    HI_U32 u32SysGainMax;
    HI_U8 u8AERunInterval;
	HI_U8 au8AeWeight[AE_WEIGHT_ROW][AE_WEIGHT_COLUMN];
} ADPT_SCENEAUTO_AEATTR_S;

typedef struct hiADPT_SCENEAUTO_EXPOSUREINFO_S
{
    HI_U32 u32Exposure;
    HI_U32 u32AGain;
    HI_U32 u32DGain;
    HI_U32 u32ISPDGain;
    HI_U32 u32ExpTime;
    HI_U8  u8AveLum;
    HI_U32 u32Hist256Value[256];
} ADPT_SCENEAUTO_EXPOSUREINFO_S;

typedef struct hiADPT_SCENEAUTO_WDRATTR_S
{
    HI_U8 u8WdrMode;
} ADPT_SCENEAUTO_WDRATTR_S;

typedef struct hiADPT_SCENEAUTO_STATEINFO_S
{
    HI_U32 u32DRCStrengthActual;
    HI_U32 u32DefogStrengthActual;
} ADPT_SCENEAUTO_STATEINFO_S;

typedef struct hiADPT_SCENEAUTO_3DNR_S
{
    HI_S32 s32SHP0;
    HI_S32 s32SHP1;
    HI_S32 s32SHP2;

    HI_S32 s32MDDZ0;
    HI_S32 s32MDDZ1;
    HI_S32 s32MDDZ2;

    HI_S32 s32TFS0;
    HI_S32 s32TFS1;
    HI_S32 s32TFS2;

    HI_S32 s32SFS0;
    HI_S32 s32SFS1;
    HI_S32 s32SFS2;

    HI_S32 s32TFP0;
    HI_S32 s32TFP1;
    HI_S32 s32TFP2;

    HI_S32 s32SPS0;
    HI_S32 s32SPS1;
    HI_S32 s32SPS2;

    HI_S32 s32TFR0;
    HI_S32 s32TFR1;
    HI_S32 s32TFR2;

    HI_S32 s32SPT0;
    HI_S32 s32SPT1;
    HI_S32 s32SPT2;

    HI_S32 s32SBF0;
    HI_S32 s32SBF1;
    HI_S32 s32SBF2;

    HI_S32 s32SFC;
    HI_S32 s32TFC;
    HI_S32 s32Mdn;

    HI_S32 s32MATW;
    HI_S32 s32MATH;
} ADPT_SCENEAUTO_3DNR_S;

typedef struct hiADPT_SCENEAUTO_DEFOG_ATTR_S
{
    HI_BOOL bEnable;
} ADPT_SCENEAUTO_DEFOG_ATTR_S;
typedef struct hiADPT_SCENEAUTO_3DNR
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
}ADPT_SCENEAUTO_3DNR;

typedef struct hiADPT_SCENEAUTO_FPN_ATTR_S
{
    HI_BOOL bEnable;
} ADPT_SCENEAUTO_FPN_ATTR_S;

typedef struct hiADPT_SCENEAUTO_CCM_ATTR_S
{
    HI_U16 u16HighColorTemp;
    HI_U16 au16HighCCM[9];
    HI_U16 u16MidColorTemp;
    HI_U16 au16MidCCM[9];
    HI_U16 u16LowColorTemp;
    HI_U16 au16LowCCM[9];
} ADPT_SCENEAUTO_CCM_ATTR_S;

typedef struct hiADPT_SCENEAUTO_ACM_ATTR_S
{
    HI_BOOL bEnable;
    HI_U32 u32CbcrThr;
    HI_U32 u32GainLuma;
    HI_U32 u32GainHue;
    HI_U32 u32GainSat;
} ADPT_SCENEAUTO_ACM_ATTR_S;


typedef struct hiADPT_SCENEAUTO_VENC_ATTR_S
{
    ADPT_SCENEAUTO_VENC_RCMODE_E eRcMode;
    HI_U32 u32BitRate;
} ADPT_SCENEAUTO_VENC_ATTR_S;

typedef struct hiADPT_SCENEAUTO_H265_RCPARAM_S
{
    HI_U32 u32ThrdI[12];
    HI_U32 u32ThrdP[12];
    HI_U32 u32QpDelta;
    HI_S32 s32IPQPDelta;
} ADPT_SCENEAUTO_H265_RCPARAM_S;
typedef struct hiADPT_SCENEAUTO_H265_FACE_CFG_S
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
} ADPT_SCENEAUTO_H265_FACE_CFG_S;
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif
