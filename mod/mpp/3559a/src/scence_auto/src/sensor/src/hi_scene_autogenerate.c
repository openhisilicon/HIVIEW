#include "hi_scene_autogenerate.h"
#include <unistd.h>
#include <string.h>
#include "mpi_awb.h"
#include "mpi_ae.h"
#include "mpi_isp.h"
#include "mpi_vi.h"
#include "mpi_hdr.h"
#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define SCENE_MAX(a, b) (((a) < (b)) ? (b) : (a))

#define SCENE_DIV_0TO1(a)  ((0 == (a)) ? 1 : (a))

#define SCENE_SHADING_TRIGMODE_L2H            (1)

#define SCENE_SHADING_TRIGMODE_H2L            (2)

#define SCENE_MAX_ISP_PIPE_NUM                (6)

HI_BOOL g_bISPPause;
HI_SCENE_PIPE_PARAM_S g_astScenePipeParam[HI_SCENE_PIPETYPE_NUM];

#define CHECK_SCENE_PAUSE()\
    do{\
        if (HI_TRUE == g_bISPPause)\
        {\
            return HI_SUCCESS;\
        }\
    }while(0);\
     
#define CHECK_SCENE_RET(s32Ret)\
    do{\
        if (HI_SUCCESS != s32Ret)\
        {\
            printf("Failed at %s: LINE: %d with %#x!", __FUNCTION__, __LINE__, s32Ret);\
        }\
    }while(0);\
     
#define CHECK_SCENE_NULLPTR(ptr)\
    do{\
        if (NULL == ptr)\
        {\
            printf("NullPtr at %s: LINE: %d!", __FUNCTION__, __LINE__);\
            return HI_FAILURE;\
        }\
    }while(0);\
     
#define FREE_SCENE_PTR(ptr)\
    do{\
        if (NULL != ptr)\
        {\
            free(ptr);\
            ptr = NULL;\
        }\
    }while(0);\
     
static  HI_U32 SCENE_GetLevelLtoH(HI_U64 u64Value, HI_U32 u32Count, HI_U64 *pu64Thresh)
{
    HI_U32 u32Level = 0;

    for (u32Level = 0; u32Level < u32Count; u32Level++)
    {
        if (u64Value <= pu64Thresh[u32Level])
        {
            break;
        }
    }

    if (u32Level == u32Count)
    {
        u32Level = u32Count - 1;
    }

    return u32Level;
}
static  HI_U32 SCENE_GetLevelHtoL(HI_U64 u64Value, HI_U32 u32Count, HI_U64 *pu64Thresh)
{
    HI_U32 u32Level = 0;

    for (u32Level = u32Count; u32Level > 0; u32Level--)
    {
        if (u64Value > pu64Thresh[u32Level - 1])
        {
            break;
        }
    }

    if (u32Level > 0)
    {
        u32Level = u32Level - 1;
    }

    return u32Level;
}


static HI_U32 SCENE_GetLevelLtoH_U32(HI_U32 u32Value, HI_U32 u32Count, HI_U32 *pu32Thresh)
{
    HI_U32 u32Level = 0;

    for (u32Level = 0; u32Level < u32Count; u32Level++)
    {
        if (u32Value <= pu32Thresh[u32Level])
        {
            break;
        }
    }
    if (u32Level == u32Count)
    {
        u32Level = u32Count - 1;
    }

    return u32Level;
}
static HI_U32 SCENE_Interpulate(HI_U32 u32Mid, HI_U32 u32Left, HI_U32 u32LValue, HI_U32 u32Right, HI_U32 u32RValue)
{
    HI_U32 u32Value = 0;
    HI_U32 k = 0;

    if (u32Mid <= u32Left)
    {
        u32Value = u32LValue;
        return u32Value;
    }

    if (u32Mid >= u32Right)
    {
        u32Value = u32RValue;
        return u32Value;
    }

    k = (u32Right - u32Left);
    u32Value = (((u32Right - u32Mid) * u32LValue + (u32Mid - u32Left) * u32RValue + (k >> 1)) / k);
    return u32Value;
}

static HI_U32 SCENE_TimeFilter(HI_U32 u32Para0, HI_U32 u32Para1, HI_U32 u32TimeCnt, HI_U32 u32Index)
{
    HI_U64 u64Temp = 0;
    HI_U32 u32Value = 0;

    if (u32Para0 > u32Para1)
    {
        u64Temp = (HI_U64)(u32Para0 - u32Para1) << 8;
        u64Temp = (u64Temp * (u32Index + 1)) / SCENE_DIV_0TO1(u32TimeCnt) >> 8;
        u32Value = u32Para0 - (HI_U32)u64Temp;
    }

    else
    {
        u64Temp = (HI_U64)(u32Para1 - u32Para0) << 8;
        u64Temp = (u64Temp * (u32Index + 1)) / SCENE_DIV_0TO1(u32TimeCnt) >> 8;
        u32Value = u32Para0 + (HI_U32)u64Temp;
    }

    return u32Value;
}


HI_S32 HI_SCENE_SetStaticAE_AutoGenerate(VI_PIPE ViPipe, HI_U8 u8Index)
{
    HI_S32 s32Ret = HI_SUCCESS;

    ISP_EXPOSURE_ATTR_S stExposureAttr;

    s32Ret = HI_MPI_ISP_GetExposureAttr(ViPipe, &stExposureAttr);
    CHECK_SCENE_RET(s32Ret);

    stExposureAttr.bAERouteExValid = g_astScenePipeParam[u8Index].stStaticAe.bAERouteExValid;
    stExposureAttr.u8AERunInterval = g_astScenePipeParam[u8Index].stStaticAe.u8AERunInterval;
    stExposureAttr.stAuto.stExpTimeRange.u32Max = g_astScenePipeParam[u8Index].stStaticAe.u32AutoExpTimeMax;
    stExposureAttr.stAuto.stSysGainRange.u32Max = g_astScenePipeParam[u8Index].stStaticAe.u32AutoSysGainMax;
    stExposureAttr.stAuto.u8Speed = g_astScenePipeParam[u8Index].stStaticAe.u8AutoSpeed;
    stExposureAttr.stAuto.u8Tolerance = g_astScenePipeParam[u8Index].stStaticAe.u8AutoTolerance;
    stExposureAttr.stAuto.stAEDelayAttr.u16BlackDelayFrame = g_astScenePipeParam[u8Index].stStaticAe.u16AutoBlackDelayFrame;
    stExposureAttr.stAuto.stAEDelayAttr.u16WhiteDelayFrame = g_astScenePipeParam[u8Index].stStaticAe.u16AutoWhiteDelayFrame;

    s32Ret = HI_MPI_ISP_SetExposureAttr(ViPipe, &stExposureAttr);
    CHECK_SCENE_RET(s32Ret);
    return HI_SUCCESS;
}

HI_S32 HI_SCENE_SetStaticAERoute_AutoGenerate(VI_PIPE ViPipe, HI_U8 u8Index)
{
    HI_S32 i = 0;
    HI_S32 s32Ret = HI_SUCCESS;

    ISP_AE_ROUTE_S stAeRoute;

    s32Ret = HI_MPI_ISP_GetAERouteAttr(ViPipe, &stAeRoute);
    CHECK_SCENE_RET(s32Ret);


    s32Ret = HI_MPI_ISP_SetAERouteAttr(ViPipe, &stAeRoute);
    CHECK_SCENE_RET(s32Ret);
    printf("i is %d.", i);
    return HI_SUCCESS;
}

HI_S32 HI_SCENE_SetStaticAWB_AutoGenerate(VI_PIPE ViPipe, HI_U8 u8Index)
{
    HI_S32 i = 0;
    HI_S32 s32Ret = HI_SUCCESS;

    ISP_WB_ATTR_S stWbAttr;

    s32Ret = HI_MPI_ISP_GetWBAttr(ViPipe, &stWbAttr);
    CHECK_SCENE_RET(s32Ret);


    s32Ret = HI_MPI_ISP_SetWBAttr(ViPipe, &stWbAttr);
    CHECK_SCENE_RET(s32Ret);
    printf("i is %d.", i);
    return HI_SUCCESS;
}

HI_S32 HI_SCENE_SetStaticAWBEX_AutoGenerate(VI_PIPE ViPipe, HI_U8 u8Index)
{
    HI_S32 i = 0;
    HI_S32 s32Ret = HI_SUCCESS;

    ISP_AWB_ATTR_EX_S stAwbAttrEx;

    s32Ret = HI_MPI_ISP_GetAWBAttrEx(ViPipe, &stAwbAttrEx);
    CHECK_SCENE_RET(s32Ret);


    s32Ret = HI_MPI_ISP_SetAWBAttrEx(ViPipe, &stAwbAttrEx);
    CHECK_SCENE_RET(s32Ret);
    printf("i is %d.", i);
    return HI_SUCCESS;
}

HI_S32 HI_SCENE_SetStaticAERouteEX_AutoGenerate(VI_PIPE ViPipe, HI_U8 u8Index)
{
    HI_S32 i = 0;
    HI_S32 s32Ret = HI_SUCCESS;

    ISP_AE_ROUTE_EX_S stAeRouteEx;

    s32Ret = HI_MPI_ISP_GetAERouteAttrEx(ViPipe, &stAeRouteEx);
    CHECK_SCENE_RET(s32Ret);

    stAeRouteEx.u32TotalNum = g_astScenePipeParam[u8Index].stStaticAeRouteEx.u32TotalNum;
    for (i = 0; i < stAeRouteEx.u32TotalNum; i++)
    {
        stAeRouteEx.astRouteExNode[i].u32IntTime = g_astScenePipeParam[u8Index].stStaticAeRouteEx.au32IntTime[i];
    }
    for (i = 0; i < stAeRouteEx.u32TotalNum; i++)
    {
        stAeRouteEx.astRouteExNode[i].u32Again = g_astScenePipeParam[u8Index].stStaticAeRouteEx.au32Again[i];
    }
    for (i = 0; i < stAeRouteEx.u32TotalNum; i++)
    {
        stAeRouteEx.astRouteExNode[i].u32Dgain = g_astScenePipeParam[u8Index].stStaticAeRouteEx.au32Dgain[i];
    }
    for (i = 0; i < stAeRouteEx.u32TotalNum; i++)
    {
        stAeRouteEx.astRouteExNode[i].u32IspDgain = g_astScenePipeParam[u8Index].stStaticAeRouteEx.au32IspDgain[i];
    }

    s32Ret = HI_MPI_ISP_SetAERouteAttrEx(ViPipe, &stAeRouteEx);
    CHECK_SCENE_RET(s32Ret);
    return HI_SUCCESS;
}

HI_S32 HI_SCENE_SetStaticGlobalCac_AutoGenerate(VI_PIPE ViPipe, HI_U8 u8Index)
{
    HI_S32 s32Ret = HI_SUCCESS;

    ISP_GLOBAL_CAC_ATTR_S stGlobalCac;

    s32Ret = HI_MPI_ISP_GetGlobalCacAttr(ViPipe, &stGlobalCac);
    CHECK_SCENE_RET(s32Ret);

    stGlobalCac.bEnable = g_astScenePipeParam[u8Index].stStaticGlobalCac.bEnable;
    stGlobalCac.u16VerCoordinate = g_astScenePipeParam[u8Index].stStaticGlobalCac.u16VerCoordinate;
    stGlobalCac.u16HorCoordinate = g_astScenePipeParam[u8Index].stStaticGlobalCac.u16HorCoordinate;
    stGlobalCac.s16ParamRedA = g_astScenePipeParam[u8Index].stStaticGlobalCac.s16ParamRedA;
    stGlobalCac.s16ParamRedB = g_astScenePipeParam[u8Index].stStaticGlobalCac.s16ParamRedB;
    stGlobalCac.s16ParamRedC = g_astScenePipeParam[u8Index].stStaticGlobalCac.s16ParamRedC;
    stGlobalCac.s16ParamBlueA = g_astScenePipeParam[u8Index].stStaticGlobalCac.s16ParamBlueA;
    stGlobalCac.s16ParamBlueB = g_astScenePipeParam[u8Index].stStaticGlobalCac.s16ParamBlueB;
    stGlobalCac.s16ParamBlueC = g_astScenePipeParam[u8Index].stStaticGlobalCac.s16ParamBlueC;
    stGlobalCac.u8VerNormShift = g_astScenePipeParam[u8Index].stStaticGlobalCac.u8VerNormShift;
    stGlobalCac.u8VerNormFactor = g_astScenePipeParam[u8Index].stStaticGlobalCac.u8VerNormFactor;
    stGlobalCac.u8HorNormShift = g_astScenePipeParam[u8Index].stStaticGlobalCac.u8HorNormShift;
    stGlobalCac.u8HorNormFactor = g_astScenePipeParam[u8Index].stStaticGlobalCac.u8HorNormFactor;
    stGlobalCac.u16CorVarThr = g_astScenePipeParam[u8Index].stStaticGlobalCac.u16CorVarThr;

    s32Ret = HI_MPI_ISP_SetGlobalCacAttr(ViPipe, &stGlobalCac);
    CHECK_SCENE_RET(s32Ret);
    return HI_SUCCESS;
}

HI_S32 HI_SCENE_SetStaticWDRExposure_AutoGenerate(VI_PIPE ViPipe, HI_U8 u8Index)
{
    HI_S32 i = 0;
    HI_S32 s32Ret = HI_SUCCESS;

    ISP_WDR_EXPOSURE_ATTR_S stWdrExposureAttr;

    s32Ret = HI_MPI_ISP_GetWDRExposureAttr(ViPipe, &stWdrExposureAttr);
    CHECK_SCENE_RET(s32Ret);

    stWdrExposureAttr.enExpRatioType = (ISP_OP_TYPE_E)g_astScenePipeParam[u8Index].stStaticWdrExposure.u8ExpRatioType;
    stWdrExposureAttr.u32ExpRatioMax = g_astScenePipeParam[u8Index].stStaticWdrExposure.u32ExpRatioMax;
    stWdrExposureAttr.u32ExpRatioMin = g_astScenePipeParam[u8Index].stStaticWdrExposure.u32ExpRatioMin;
    for (i = 0; i < EXP_RATIO_NUM; i++)
    {
        stWdrExposureAttr.au32ExpRatio[i] = g_astScenePipeParam[u8Index].stStaticWdrExposure.au32ExpRatio[i];
    }

    s32Ret = HI_MPI_ISP_SetWDRExposureAttr(ViPipe, &stWdrExposureAttr);
    CHECK_SCENE_RET(s32Ret);
    return HI_SUCCESS;
}

HI_S32 HI_SCENE_SetStaticDRC_Autogenerate(VI_PIPE ViPipe, HI_U8 u8Index)
{
    HI_S32 i = 0;
    HI_S32 s32Ret = HI_SUCCESS;

    ISP_DRC_ATTR_S stDrcAttr;

    s32Ret = HI_MPI_ISP_GetDRCAttr(ViPipe, &stDrcAttr);
    CHECK_SCENE_RET(s32Ret);

    stDrcAttr.bEnable = g_astScenePipeParam[u8Index].stStaticDrc.bEnable;
    stDrcAttr.enCurveSelect = (ISP_DRC_CURVE_SELECT_E)g_astScenePipeParam[u8Index].stStaticDrc.u8CurveSelect;
    stDrcAttr.enOpType = (ISP_OP_TYPE_E)g_astScenePipeParam[u8Index].stStaticDrc.u8DRCOpType;
    for (i = 0; i < HI_ISP_DRC_TM_NODE_NUM; i++)
    {
        stDrcAttr.au16ToneMappingValue[i] = g_astScenePipeParam[u8Index].stStaticDrc.au16ToneMappingValue[i];
    }

    s32Ret = HI_MPI_ISP_SetDRCAttr(ViPipe, &stDrcAttr);
    CHECK_SCENE_RET(s32Ret);
    return HI_SUCCESS;
}

HI_S32 HI_SCENE_SetStaticLDCI_AutoGenerate(VI_PIPE ViPipe, HI_U8 u8Index)
{
    HI_S32 s32Ret = HI_SUCCESS;

    ISP_LDCI_ATTR_S stLDCIAttr;

    s32Ret = HI_MPI_ISP_GetLDCIAttr(ViPipe, &stLDCIAttr);
    CHECK_SCENE_RET(s32Ret);


    s32Ret = HI_MPI_ISP_SetLDCIAttr(ViPipe, &stLDCIAttr);
    CHECK_SCENE_RET(s32Ret);
    return HI_SUCCESS;
}

HI_S32 HI_SCENE_SetStaticDeHaze_AutoGenerate(VI_PIPE ViPipe, HI_U8 u8Index)
{
    HI_S32 i = 0;
    HI_S32 s32Ret = HI_SUCCESS;

    ISP_DEHAZE_ATTR_S stDehazeAttr;

    s32Ret = HI_MPI_ISP_GetDehazeAttr(ViPipe, &stDehazeAttr);
    CHECK_SCENE_RET(s32Ret);

    stDehazeAttr.bEnable = g_astScenePipeParam[u8Index].stStaticDehaze.bEnable;
    stDehazeAttr.enOpType = (ISP_OP_TYPE_E)g_astScenePipeParam[u8Index].stStaticDehaze.u8DehazeOpType;
    stDehazeAttr.bUserLutEnable = g_astScenePipeParam[u8Index].stStaticDehaze.bUserLutEnable;
    for (i = 0; i < 256; i++)
    {
        stDehazeAttr.au8DehazeLut[i] = g_astScenePipeParam[u8Index].stStaticDehaze.au8DehazeLut[i];
    }

    s32Ret = HI_MPI_ISP_SetDehazeAttr(ViPipe, &stDehazeAttr);
    CHECK_SCENE_RET(s32Ret);
    return HI_SUCCESS;
}

HI_S32 HI_SCENE_SetStaticThreeDNR_AutoGenerate(VI_PIPE ViPipe, HI_U8 u8Index)
{
    HI_S32 i, j, k = 0;
    HI_S32 s32Ret = HI_SUCCESS;

    VI_PIPE_NRX_PARAM_S stNrx;

    stNrx.stNRXParamV1.stNRXAutoV1.u32ParamNum = 12;
    stNrx.stNRXParamV1.stNRXAutoV1.pau32ISO = (HI_U32 *)malloc(sizeof(HI_U32) * 12);
    CHECK_SCENE_NULLPTR(stNrx.stNRXParamV1.stNRXAutoV1.pau32ISO);
    stNrx.stNRXParamV1.stNRXAutoV1.pastNRXParamV1 = (VI_PIPE_NRX_PARAM_V1_S *)malloc(sizeof(VI_PIPE_NRX_PARAM_V1_S) * 12);
    CHECK_SCENE_NULLPTR(stNrx.stNRXParamV1.stNRXAutoV1.pastNRXParamV1);

    stNrx.enNRVersion = VI_NR_V1;
    stNrx.stNRXParamV1.enOptMode = OPERATION_MODE_AUTO;

    s32Ret = HI_MPI_VI_GetPipeNRXParam(ViPipe, &stNrx);
    if (HI_SUCCESS != s32Ret)
    {
        printf("HI_MPI_VI_GetPipeNRXParam failed.");
        FREE_SCENE_PTR(stNrx.stNRXParamV1.stNRXAutoV1.pau32ISO);
        FREE_SCENE_PTR(stNrx.stNRXParamV1.stNRXAutoV1.pastNRXParamV1);
        return HI_FAILURE;
    }

    memcpy(stNrx.stNRXParamV1.stNRXAutoV1.pau32ISO, g_astScenePipeParam[u8Index].stStaticThreeDNR.au32ThreeDNRIso,
           sizeof(HI_U32) * g_astScenePipeParam[u8Index].stStaticThreeDNR.u32ThreeDNRCount);

    stNrx.stNRXParamV1.stNRXAutoV1.u32ParamNum = g_astScenePipeParam[u8Index].stStaticThreeDNR.u32ThreeDNRCount;
    for (i = 0; i < stNrx.stNRXParamV1.stNRXAutoV1.u32ParamNum; i++)
    {
        stNrx.stNRXParamV1.stNRXAutoV1.pastNRXParamV1[i].IEy.IES = g_astScenePipeParam[u8Index].stStaticThreeDNR.astThreeDNRValue[i].IEy.IES;
        stNrx.stNRXParamV1.stNRXAutoV1.pastNRXParamV1[i].IEy.IESS = g_astScenePipeParam[u8Index].stStaticThreeDNR.astThreeDNRValue[i].IEy.IESS;
        stNrx.stNRXParamV1.stNRXAutoV1.pastNRXParamV1[i].IEy.IEDZ = g_astScenePipeParam[u8Index].stStaticThreeDNR.astThreeDNRValue[i].IEy.IEDZ;
        for (j = 0; j < 5; j++)
        {
            stNrx.stNRXParamV1.stNRXAutoV1.pastNRXParamV1[i].SFy[j].SBF = g_astScenePipeParam[u8Index].stStaticThreeDNR.astThreeDNRValue[i].SFy[j].SBF;
            stNrx.stNRXParamV1.stNRXAutoV1.pastNRXParamV1[i].SFy[j].STR = g_astScenePipeParam[u8Index].stStaticThreeDNR.astThreeDNRValue[i].SFy[j].STR;
            stNrx.stNRXParamV1.stNRXAutoV1.pastNRXParamV1[i].SFy[j].STHp = g_astScenePipeParam[u8Index].stStaticThreeDNR.astThreeDNRValue[i].SFy[j].STHp;
            stNrx.stNRXParamV1.stNRXAutoV1.pastNRXParamV1[i].SFy[j].SFT = g_astScenePipeParam[u8Index].stStaticThreeDNR.astThreeDNRValue[i].SFy[j].SFT;
            stNrx.stNRXParamV1.stNRXAutoV1.pastNRXParamV1[i].SFy[j].kPro = g_astScenePipeParam[u8Index].stStaticThreeDNR.astThreeDNRValue[i].SFy[j].kPro;
            for (k = 0; k < 3; k++)
            {
                stNrx.stNRXParamV1.stNRXAutoV1.pastNRXParamV1[i].SFy[j].STH[k] = g_astScenePipeParam[u8Index].stStaticThreeDNR.astThreeDNRValue[i].SFy[j].STH[k];
                stNrx.stNRXParamV1.stNRXAutoV1.pastNRXParamV1[i].SFy[j].SBS[k] = g_astScenePipeParam[u8Index].stStaticThreeDNR.astThreeDNRValue[i].SFy[j].SBS[k];
                stNrx.stNRXParamV1.stNRXAutoV1.pastNRXParamV1[i].SFy[j].SDS[k] = g_astScenePipeParam[u8Index].stStaticThreeDNR.astThreeDNRValue[i].SFy[j].SDS[k];
            }
        }
        for (j = 0; j < 2; j++)
        {
            stNrx.stNRXParamV1.stNRXAutoV1.pastNRXParamV1[i].MDy[j].MATH = g_astScenePipeParam[u8Index].stStaticThreeDNR.astThreeDNRValue[i].MDy[j].MATH;
            stNrx.stNRXParamV1.stNRXAutoV1.pastNRXParamV1[i].MDy[j].MATE = g_astScenePipeParam[u8Index].stStaticThreeDNR.astThreeDNRValue[i].MDy[j].MATE;
            stNrx.stNRXParamV1.stNRXAutoV1.pastNRXParamV1[i].MDy[j].MATW = g_astScenePipeParam[u8Index].stStaticThreeDNR.astThreeDNRValue[i].MDy[j].MATW;
            stNrx.stNRXParamV1.stNRXAutoV1.pastNRXParamV1[i].MDy[j].MASW = g_astScenePipeParam[u8Index].stStaticThreeDNR.astThreeDNRValue[i].MDy[j].MASW;
            stNrx.stNRXParamV1.stNRXAutoV1.pastNRXParamV1[i].MDy[j].MABW = g_astScenePipeParam[u8Index].stStaticThreeDNR.astThreeDNRValue[i].MDy[j].MABW;
            stNrx.stNRXParamV1.stNRXAutoV1.pastNRXParamV1[i].MDy[j].MAXN = g_astScenePipeParam[u8Index].stStaticThreeDNR.astThreeDNRValue[i].MDy[j].MAXN;
        }
        for (j = 0; j < 2; j++)
        {
            for (k = 0; k < 4; k++)
            {
                stNrx.stNRXParamV1.stNRXAutoV1.pastNRXParamV1[i].TFy[j].TFR[k] = g_astScenePipeParam[u8Index].stStaticThreeDNR.astThreeDNRValue[i].TFy[j].TFR[k];
            }
            stNrx.stNRXParamV1.stNRXAutoV1.pastNRXParamV1[i].TFy[j].TDZ = g_astScenePipeParam[u8Index].stStaticThreeDNR.astThreeDNRValue[i].TFy[j].TDZ;
            stNrx.stNRXParamV1.stNRXAutoV1.pastNRXParamV1[i].TFy[j].TDX = g_astScenePipeParam[u8Index].stStaticThreeDNR.astThreeDNRValue[i].TFy[j].TDX;
            stNrx.stNRXParamV1.stNRXAutoV1.pastNRXParamV1[i].TFy[j].TFS = g_astScenePipeParam[u8Index].stStaticThreeDNR.astThreeDNRValue[i].TFy[j].TFS;
        }
        stNrx.stNRXParamV1.stNRXAutoV1.pastNRXParamV1[i].HdgType = g_astScenePipeParam[u8Index].stStaticThreeDNR.astThreeDNRValue[i].HdgType;
        stNrx.stNRXParamV1.stNRXAutoV1.pastNRXParamV1[i].BriType = g_astScenePipeParam[u8Index].stStaticThreeDNR.astThreeDNRValue[i].BriType;
        stNrx.stNRXParamV1.stNRXAutoV1.pastNRXParamV1[i].HdgMode = g_astScenePipeParam[u8Index].stStaticThreeDNR.astThreeDNRValue[i].HdgMode;
        stNrx.stNRXParamV1.stNRXAutoV1.pastNRXParamV1[i].kTab2 = g_astScenePipeParam[u8Index].stStaticThreeDNR.astThreeDNRValue[i].kTab2;
        stNrx.stNRXParamV1.stNRXAutoV1.pastNRXParamV1[i].HdgWnd = g_astScenePipeParam[u8Index].stStaticThreeDNR.astThreeDNRValue[i].HdgWnd;
        stNrx.stNRXParamV1.stNRXAutoV1.pastNRXParamV1[i].kTab3 = g_astScenePipeParam[u8Index].stStaticThreeDNR.astThreeDNRValue[i].kTab3;
        stNrx.stNRXParamV1.stNRXAutoV1.pastNRXParamV1[i].HdgSFR = g_astScenePipeParam[u8Index].stStaticThreeDNR.astThreeDNRValue[i].HdgSFR;
        stNrx.stNRXParamV1.stNRXAutoV1.pastNRXParamV1[i].nOut = g_astScenePipeParam[u8Index].stStaticThreeDNR.astThreeDNRValue[i].nOut;
        stNrx.stNRXParamV1.stNRXAutoV1.pastNRXParamV1[i].HdgIES = g_astScenePipeParam[u8Index].stStaticThreeDNR.astThreeDNRValue[i].HdgIES;
        stNrx.stNRXParamV1.stNRXAutoV1.pastNRXParamV1[i].nRef = g_astScenePipeParam[u8Index].stStaticThreeDNR.astThreeDNRValue[i].nRef;
        for (j = 0; j < 4; j++)
        {
            stNrx.stNRXParamV1.stNRXAutoV1.pastNRXParamV1[i].SFRi[j] = g_astScenePipeParam[u8Index].stStaticThreeDNR.astThreeDNRValue[i].SFRi[j];
            stNrx.stNRXParamV1.stNRXAutoV1.pastNRXParamV1[i].SFRk[j] = g_astScenePipeParam[u8Index].stStaticThreeDNR.astThreeDNRValue[i].SFRk[j];
        }
        for (j = 0; j < 32; j++)
        {
            stNrx.stNRXParamV1.stNRXAutoV1.pastNRXParamV1[i].SBSk2[j] = g_astScenePipeParam[u8Index].stStaticThreeDNR.astThreeDNRValue[i].SBSk2[j];
            stNrx.stNRXParamV1.stNRXAutoV1.pastNRXParamV1[i].SBSk3[j] = g_astScenePipeParam[u8Index].stStaticThreeDNR.astThreeDNRValue[i].SBSk3[j];
            stNrx.stNRXParamV1.stNRXAutoV1.pastNRXParamV1[i].SDSk2[j] = g_astScenePipeParam[u8Index].stStaticThreeDNR.astThreeDNRValue[i].SDSk2[j];
            stNrx.stNRXParamV1.stNRXAutoV1.pastNRXParamV1[i].SDSk3[j] = g_astScenePipeParam[u8Index].stStaticThreeDNR.astThreeDNRValue[i].SDSk3[j];
        }
        for (j = 0; j < 16; j++)
        {
            stNrx.stNRXParamV1.stNRXAutoV1.pastNRXParamV1[i].BriThr[j] = g_astScenePipeParam[u8Index].stStaticThreeDNR.astThreeDNRValue[i].BriThr[j];
        }
        stNrx.stNRXParamV1.stNRXAutoV1.pastNRXParamV1[i].NRc.SFC = g_astScenePipeParam[u8Index].stStaticThreeDNR.astThreeDNRValue[i].NRc.SFC;
        stNrx.stNRXParamV1.stNRXAutoV1.pastNRXParamV1[i].NRc.TFC = g_astScenePipeParam[u8Index].stStaticThreeDNR.astThreeDNRValue[i].NRc.TFC;
        stNrx.stNRXParamV1.stNRXAutoV1.pastNRXParamV1[i].NRc.CSFS = g_astScenePipeParam[u8Index].stStaticThreeDNR.astThreeDNRValue[i].NRc.CSFS;
        stNrx.stNRXParamV1.stNRXAutoV1.pastNRXParamV1[i].NRc.CSFk = g_astScenePipeParam[u8Index].stStaticThreeDNR.astThreeDNRValue[i].NRc.CSFk;
        stNrx.stNRXParamV1.stNRXAutoV1.pastNRXParamV1[i].NRc.CTFS = g_astScenePipeParam[u8Index].stStaticThreeDNR.astThreeDNRValue[i].NRc.CTFS;
        stNrx.stNRXParamV1.stNRXAutoV1.pastNRXParamV1[i].NRc.CIIR = g_astScenePipeParam[u8Index].stStaticThreeDNR.astThreeDNRValue[i].NRc.CIIR;
        stNrx.stNRXParamV1.stNRXAutoV1.pastNRXParamV1[i].NRc.CTFR = g_astScenePipeParam[u8Index].stStaticThreeDNR.astThreeDNRValue[i].NRc.CTFR;
    }
    stNrx.enNRVersion = VI_NR_V1;
    stNrx.stNRXParamV1.enOptMode = OPERATION_MODE_AUTO;

    s32Ret = HI_MPI_VI_SetPipeNRXParam(ViPipe, &stNrx);
    if (HI_SUCCESS != s32Ret)
    {
        printf("HI_MPI_VI_SetPipeNRXParam failed.");
        FREE_SCENE_PTR(stNrx.stNRXParamV1.stNRXAutoV1.pau32ISO);
        FREE_SCENE_PTR(stNrx.stNRXParamV1.stNRXAutoV1.pastNRXParamV1);
        return HI_FAILURE;
    }
    FREE_SCENE_PTR(stNrx.stNRXParamV1.stNRXAutoV1.pau32ISO);
    FREE_SCENE_PTR(stNrx.stNRXParamV1.stNRXAutoV1.pastNRXParamV1);
    return HI_SUCCESS;
}

HI_S32 HI_SCENE_SetStaticStatisticsCfg_AutoGenerare(VI_PIPE ViPipe, HI_U8 u8Index, HI_BOOL bMetryFixed)
{
    HI_S32 i, j = 0;
    HI_S32 s32Ret = HI_SUCCESS;

    ISP_STATISTICS_CFG_S stStatisticsCfg;

    s32Ret = HI_MPI_ISP_GetStatisticsConfig(ViPipe, &stStatisticsCfg);
    CHECK_SCENE_RET(s32Ret);

    if (HI_FALSE == bMetryFixed)
    {
        for (i = 0; i < AE_ZONE_ROW; i++)
        {
            for (j = 0; j < AE_ZONE_COLUMN; j++)
            {
                stStatisticsCfg.stAECfg.au8Weight[i][j] = g_astScenePipeParam[u8Index].stStaticStatistics.au8AEWeight[i][j];
            }
        }
    }

    s32Ret = HI_MPI_ISP_SetStatisticsConfig(ViPipe, &stStatisticsCfg);
    CHECK_SCENE_RET(s32Ret);
    return HI_SUCCESS;
}

HI_S32 HI_SCENE_SetStaticSaturation_AutoGenerate(VI_PIPE ViPipe, HI_U8 u8Index)
{
    HI_S32 i = 0;
    HI_S32 s32Ret = HI_SUCCESS;

    ISP_SATURATION_ATTR_S stSaturationAttr;

    s32Ret = HI_MPI_ISP_GetSaturationAttr(ViPipe, &stSaturationAttr);
    CHECK_SCENE_RET(s32Ret);


    s32Ret = HI_MPI_ISP_SetSaturationAttr(ViPipe, &stSaturationAttr);
    CHECK_SCENE_RET(s32Ret);
    printf("i is %d.", i);
    return HI_SUCCESS;
}

HI_S32 HI_SCENE_SetStaticCCM_AutoGenerate(VI_PIPE ViPipe, HI_U8 u8Index)
{
    HI_S32 i = 0;
    HI_S32 j = 0;
    HI_S32 s32Ret = HI_SUCCESS;

    ISP_COLORMATRIX_ATTR_S stColormatrixAttr;

    s32Ret = HI_MPI_ISP_GetCCMAttr(ViPipe, &stColormatrixAttr);
    CHECK_SCENE_RET(s32Ret);


    s32Ret = HI_MPI_ISP_SetCCMAttr(ViPipe, &stColormatrixAttr);
    CHECK_SCENE_RET(s32Ret);
    printf("i is %d and j is %d.", i, j);
    return HI_SUCCESS;
}

HI_S32 HI_SCENE_SetStaticCSC_AutoGenerate(VI_PIPE ViPipe, HI_U8 u8Index)
{
    HI_S32 s32Ret = HI_SUCCESS;

    ISP_CSC_ATTR_S stCscAttr;

    s32Ret = HI_MPI_ISP_GetCSCAttr(ViPipe, &stCscAttr);
    CHECK_SCENE_RET(s32Ret);


    s32Ret = HI_MPI_ISP_SetCSCAttr(ViPipe, &stCscAttr);
    CHECK_SCENE_RET(s32Ret);
    return HI_SUCCESS;
}

HI_S32 HI_SCENE_SetStaticNr_AutoGenerate(VI_PIPE ViPipe, HI_U8 u8Index)
{
    HI_S32 s32Ret = HI_SUCCESS;

    ISP_NR_ATTR_S stNrAttr;

    s32Ret = HI_MPI_ISP_GetNRAttr(ViPipe, &stNrAttr);
    CHECK_SCENE_RET(s32Ret);


    s32Ret = HI_MPI_ISP_SetNRAttr(ViPipe, &stNrAttr);
    CHECK_SCENE_RET(s32Ret);
    return HI_SUCCESS;
}

HI_S32 HI_SCENE_SetStaticShading_AutoGenerate(VI_PIPE ViPipe, HI_U8 u8Index)
{
    HI_S32 s32Ret = HI_SUCCESS;

    ISP_SHADING_ATTR_S stShadingAttr;

    s32Ret = HI_MPI_ISP_GetMeshShadingAttr(ViPipe, &stShadingAttr);
    CHECK_SCENE_RET(s32Ret);

    stShadingAttr.bEnable = g_astScenePipeParam[u8Index].stStaticShading.bEnable;

    s32Ret = HI_MPI_ISP_SetMeshShadingAttr(ViPipe, &stShadingAttr);
    CHECK_SCENE_RET(s32Ret);
    return HI_SUCCESS;
}

HI_S32 HI_SCENE_SetDynamicPhotoGamma_AutoGenerate(VI_PIPE ViPipe, HI_U64 u64Exposure, HI_U64 u64LastExposure, HI_U8 u8Index)
{
    HI_U32 i = 0;
    HI_U32 u32ExpLevel = 0;
    HI_S32 s32Ret = HI_SUCCESS;

    ISP_GAMMA_ATTR_S stIspGammaAttr;

    if (u64Exposure != u64LastExposure)
    {
        CHECK_SCENE_PAUSE();
        s32Ret = HI_MPI_ISP_GetGammaAttr(ViPipe, &stIspGammaAttr);
        CHECK_SCENE_RET(s32Ret);

        stIspGammaAttr.enCurveType = ISP_GAMMA_CURVE_USER_DEFINE;

        for (i = 0; i < GAMMA_NODE_NUM; i++)
        {
            stIspGammaAttr.u16Table[i] = g_astScenePipeParam[u8Index].stDynamicGamma.au16Table[u32ExpLevel][i];
        }

        CHECK_SCENE_PAUSE();
        s32Ret = HI_MPI_ISP_SetGammaAttr(ViPipe, &stIspGammaAttr);
        CHECK_SCENE_RET(s32Ret);
    }
    return HI_SUCCESS;
}

HI_S32 HI_SCENE_SetDynamicVideoGamma_AutoGenerate(VI_PIPE ViPipe, HI_U64 u64Exposure, HI_U64 u64LastExposure, HI_U8 u8Index)
{
    HI_U32 i, j = 0;
    HI_U32 u32ExpLevel = 0;
    static HI_U32 u32LastExpLevel;
    HI_S32 s32Ret = HI_SUCCESS;

    ISP_GAMMA_ATTR_S stIspGammaAttr;

    if (u64Exposure != u64LastExposure)
    {
        if (u64Exposure > u64LastExposure)
        {
            u32ExpLevel = SCENE_GetLevelLtoH(u64Exposure, g_astScenePipeParam[u8Index].stDynamicGamma.u32TotalNum, g_astScenePipeParam[u8Index].stDynamicGamma.au64ExpThreshLtoH);
        }
        else
        {
            u32ExpLevel = SCENE_GetLevelHtoL(u64Exposure, g_astScenePipeParam[u8Index].stDynamicGamma.u32TotalNum, g_astScenePipeParam[u8Index].stDynamicGamma.au64ExpThreshHtoL);
        }
    }
    if (u64Exposure != u64LastExposure)
    {
        for (i = 0; i < g_astScenePipeParam[u8Index].stDynamicGamma.u32InterVal; i++)
        {
            CHECK_SCENE_PAUSE();
            s32Ret = HI_MPI_ISP_GetGammaAttr(ViPipe, &stIspGammaAttr);
            CHECK_SCENE_RET(s32Ret);

            for (j = 0; j < GAMMA_NODE_NUM; j++)
            {
                stIspGammaAttr.u16Table[j] = SCENE_TimeFilter(g_astScenePipeParam[u8Index].stDynamicGamma.au16Table[u32LastExpLevel][j],
                                             g_astScenePipeParam[u8Index].stDynamicGamma.au16Table[u32ExpLevel][j],
                                             g_astScenePipeParam[u8Index].stDynamicGamma.u32InterVal, i);
            }
            stIspGammaAttr.enCurveType = ISP_GAMMA_CURVE_USER_DEFINE;
            CHECK_SCENE_PAUSE();
            s32Ret = HI_MPI_ISP_SetGammaAttr(ViPipe, &stIspGammaAttr);
            CHECK_SCENE_RET(s32Ret);
            usleep(30000);
        }
        u32LastExpLevel = u32ExpLevel;
    }
    return HI_SUCCESS;
}

HI_S32 HI_SCENE_SetDynamicAE_AutoGenerate(VI_PIPE ViPipe, HI_U64 u64Exposure, HI_U64 u64LastExposure, HI_U8 u8Index)
{
    HI_U32 u32ExpLevel = 0;
    HI_S32 s32Ret = HI_SUCCESS;

    if (u64Exposure != u64LastExposure)
    {
        ISP_EXPOSURE_ATTR_S stExposureAttr;
        ISP_WDR_EXPOSURE_ATTR_S stWdrExposureAttr;
        CHECK_SCENE_PAUSE();
        s32Ret = HI_MPI_ISP_GetExposureAttr(ViPipe, &stExposureAttr);
        CHECK_SCENE_RET(s32Ret);

        CHECK_SCENE_PAUSE();
        s32Ret = HI_MPI_ISP_GetWDRExposureAttr(ViPipe, &stWdrExposureAttr);
        CHECK_SCENE_RET(s32Ret);

        if (u64Exposure > u64LastExposure)
        {
            u32ExpLevel = SCENE_GetLevelLtoH(u64Exposure, g_astScenePipeParam[u8Index].stDynamicAe.u8AEExposureCnt, g_astScenePipeParam[u8Index].stDynamicAe.au64ExpLtoHThresh);
        }
        else
        {
            u32ExpLevel = SCENE_GetLevelHtoL(u64Exposure, g_astScenePipeParam[u8Index].stDynamicAe.u8AEExposureCnt, g_astScenePipeParam[u8Index].stDynamicAe.au64ExpHtoLThresh);
        }
        if (u32ExpLevel == g_astScenePipeParam[u8Index].stDynamicAe.u8AEExposureCnt - 1)
        {
            stExposureAttr.stAuto.u8Compensation = g_astScenePipeParam[u8Index].stDynamicAe.au8AutoCompensation[u32ExpLevel];
        }
        else
        {
            stExposureAttr.stAuto.u8Compensation = SCENE_Interpulate(u64Exposure,
                                                   g_astScenePipeParam[u8Index].stDynamicAe.au64ExpLtoHThresh[u32ExpLevel],
                                                   g_astScenePipeParam[u8Index].stDynamicAe.au8AutoCompensation[u32ExpLevel],
                                                   g_astScenePipeParam[u8Index].stDynamicAe.au64ExpLtoHThresh[u32ExpLevel + 1],
                                                   g_astScenePipeParam[u8Index].stDynamicAe.au8AutoCompensation[u32ExpLevel + 1]);

        }
        if (u32ExpLevel == g_astScenePipeParam[u8Index].stDynamicAe.u8AEExposureCnt - 1)
        {
            stExposureAttr.stAuto.u8MaxHistOffset = g_astScenePipeParam[u8Index].stDynamicAe.au8AutoMaxHistOffset[u32ExpLevel];
        }
        else
        {
            stExposureAttr.stAuto.u8MaxHistOffset = SCENE_Interpulate(u64Exposure,
                                                    g_astScenePipeParam[u8Index].stDynamicAe.au64ExpLtoHThresh[u32ExpLevel],
                                                    g_astScenePipeParam[u8Index].stDynamicAe.au8AutoMaxHistOffset[u32ExpLevel],
                                                    g_astScenePipeParam[u8Index].stDynamicAe.au64ExpLtoHThresh[u32ExpLevel + 1],
                                                    g_astScenePipeParam[u8Index].stDynamicAe.au8AutoMaxHistOffset[u32ExpLevel + 1]);

        }

        CHECK_SCENE_PAUSE();
        s32Ret = HI_MPI_ISP_SetExposureAttr(ViPipe, &stExposureAttr);
        CHECK_SCENE_RET(s32Ret);
        if (u32ExpLevel == g_astScenePipeParam[u8Index].stDynamicAe.u8AEExposureCnt - 1)
        {
            stWdrExposureAttr.u32ExpRatioMax = g_astScenePipeParam[u8Index].stDynamicAe.au32AutoExpRatioMax[u32ExpLevel];
        }
        else
        {
            stWdrExposureAttr.u32ExpRatioMax = SCENE_Interpulate(u64Exposure,
                                               g_astScenePipeParam[u8Index].stDynamicAe.au64ExpLtoHThresh[u32ExpLevel],
                                               g_astScenePipeParam[u8Index].stDynamicAe.au32AutoExpRatioMax[u32ExpLevel],
                                               g_astScenePipeParam[u8Index].stDynamicAe.au64ExpLtoHThresh[u32ExpLevel + 1],
                                               g_astScenePipeParam[u8Index].stDynamicAe.au32AutoExpRatioMax[u32ExpLevel + 1]);
        }
        if (u32ExpLevel == g_astScenePipeParam[u8Index].stDynamicAe.u8AEExposureCnt - 1)
        {
            stWdrExposureAttr.u32ExpRatioMin = g_astScenePipeParam[u8Index].stDynamicAe.au32AutoExpRatioMin[u32ExpLevel];
        }
        else
        {
            stWdrExposureAttr.u32ExpRatioMin = SCENE_Interpulate(u64Exposure,
                                               g_astScenePipeParam[u8Index].stDynamicAe.au64ExpLtoHThresh[u32ExpLevel],
                                               g_astScenePipeParam[u8Index].stDynamicAe.au32AutoExpRatioMin[u32ExpLevel],
                                               g_astScenePipeParam[u8Index].stDynamicAe.au64ExpLtoHThresh[u32ExpLevel + 1],
                                               g_astScenePipeParam[u8Index].stDynamicAe.au32AutoExpRatioMin[u32ExpLevel + 1]);
        }

        CHECK_SCENE_PAUSE();
        s32Ret = HI_MPI_ISP_SetWDRExposureAttr(ViPipe, &stWdrExposureAttr);
        CHECK_SCENE_RET(s32Ret);
    }
    return HI_SUCCESS;
}

HI_S32 HI_SCENE_SetDynamicShading_AutoGenerate(VI_PIPE ViPipe, HI_U64 u64Exposure, HI_U64 u64LastExposure, HI_U8 u8Index)
{
    HI_U32 u32ExpLevel = 0;
    HI_S32 s32Ret = HI_SUCCESS;

    ISP_SHADING_ATTR_S stShadingAttr;

    if (u64Exposure != u64LastExposure)
    {
        CHECK_SCENE_PAUSE();
        s32Ret = HI_MPI_ISP_GetMeshShadingAttr(ViPipe, &stShadingAttr);
        CHECK_SCENE_RET(s32Ret);

        u32ExpLevel = SCENE_GetLevelLtoH(u64Exposure, g_astScenePipeParam[u8Index].stDynamicShading.u32ExpThreshCnt, g_astScenePipeParam[u8Index].stDynamicShading.au64ExpThreshLtoH);
        if (u32ExpLevel == 0 || u32ExpLevel == g_astScenePipeParam[u8Index].stDynamicShading.u32ExpThreshCnt - 1)
        {
            stShadingAttr.u16MeshStr = g_astScenePipeParam[u8Index].stDynamicShading.au16ManualStrength[u32ExpLevel];
        }
        else
        {
            stShadingAttr.u16MeshStr = SCENE_Interpulate(u64Exposure,
                                       g_astScenePipeParam[u8Index].stDynamicShading.au64ExpThreshLtoH[u32ExpLevel - 1],
                                       g_astScenePipeParam[u8Index].stDynamicShading.au16ManualStrength[u32ExpLevel - 1],
                                       g_astScenePipeParam[u8Index].stDynamicShading.au64ExpThreshLtoH[u32ExpLevel],
                                       g_astScenePipeParam[u8Index].stDynamicShading.au16ManualStrength[u32ExpLevel]);
        }
        CHECK_SCENE_PAUSE();
        s32Ret = HI_MPI_ISP_SetMeshShadingAttr(ViPipe, &stShadingAttr);
        CHECK_SCENE_RET(s32Ret);
    }
    return HI_SUCCESS;
}

HI_S32 HI_SCENE_SetDynamicLDCI_AutoGenerate(VI_PIPE ViPipe, HI_U64 u64Exposure, HI_U64 u64LastExposure, HI_U8 u8Index)
{
    HI_U32 u32ExpLevel = 0;
    HI_S32 s32Ret = HI_SUCCESS;

    ISP_LDCI_ATTR_S stLDCIAttr;

    if (u64Exposure != u64LastExposure)
    {
        CHECK_SCENE_PAUSE();
        s32Ret = HI_MPI_ISP_GetLDCIAttr(ViPipe, &stLDCIAttr);
        CHECK_SCENE_RET(s32Ret);

        u32ExpLevel = SCENE_GetLevelLtoH(u64Exposure, g_astScenePipeParam[u8Index].stDynamicLDCI.u32EnableCnt, g_astScenePipeParam[u8Index].stDynamicLDCI.au64EnableExpThreshLtoH);
        stLDCIAttr.bEnable = g_astScenePipeParam[u8Index].stDynamicLDCI.au8Enable[u32ExpLevel];

        CHECK_SCENE_PAUSE();
        s32Ret = HI_MPI_ISP_SetLDCIAttr(ViPipe, &stLDCIAttr);
        CHECK_SCENE_RET(s32Ret);
    }
    return HI_SUCCESS;
}

HI_S32 HI_SCENE_SetDynamicDehaze_AutoGenerate(VI_PIPE ViPipe, HI_U64 u64Exposure, HI_U64 u64LastExposure, HI_U8 u8Index)
{
    HI_U32 u32ExpLevel = 0;
    HI_S32 s32Ret = HI_SUCCESS;

    ISP_DEHAZE_ATTR_S stDehazeAttr;

    if (u64Exposure != u64LastExposure)
    {
        CHECK_SCENE_PAUSE();
        s32Ret = HI_MPI_ISP_GetDehazeAttr(ViPipe, &stDehazeAttr);
        CHECK_SCENE_RET(s32Ret);

        u32ExpLevel = SCENE_GetLevelLtoH(u64Exposure, g_astScenePipeParam[u8Index].stDynamicDehaze.u32ExpThreshCnt, g_astScenePipeParam[u8Index].stDynamicDehaze.au64ExpThreshLtoH);
        if (u32ExpLevel == 0 || u32ExpLevel == g_astScenePipeParam[u8Index].stDynamicDehaze.u32ExpThreshCnt - 1)
        {
            stDehazeAttr.stManual.u8strength = g_astScenePipeParam[u8Index].stDynamicDehaze.au8ManualStrength[u32ExpLevel];
        }
        else
        {
            stDehazeAttr.stManual.u8strength = SCENE_Interpulate(u64Exposure,
                                               g_astScenePipeParam[u8Index].stDynamicDehaze.au64ExpThreshLtoH[u32ExpLevel - 1],
                                               g_astScenePipeParam[u8Index].stDynamicDehaze.au8ManualStrength[u32ExpLevel - 1],
                                               g_astScenePipeParam[u8Index].stDynamicDehaze.au64ExpThreshLtoH[u32ExpLevel],
                                               g_astScenePipeParam[u8Index].stDynamicDehaze.au8ManualStrength[u32ExpLevel]);
        }

        CHECK_SCENE_PAUSE();
        s32Ret = HI_MPI_ISP_SetDehazeAttr(ViPipe, &stDehazeAttr);
        CHECK_SCENE_RET(s32Ret);
    }
    return HI_SUCCESS;
}

HI_S32 HI_SCENE_SetDynamicFsWdr_AutoGenerate(VI_PIPE ViPipe, HI_U32 u32ISO, HI_U32 u32LastISO, HI_U8 u8Index, HI_U32 u32ActRation)
{
    HI_U32 u32IsoLevel = 0;
    HI_S32 s32Ret = HI_SUCCESS;

    ISP_WDR_FS_ATTR_S stFSWDRAttr;

    if (u32ISO != u32LastISO)
    {
        CHECK_SCENE_PAUSE();
        s32Ret = HI_MPI_ISP_GetFSWDRAttr(ViPipe, &stFSWDRAttr);
        CHECK_SCENE_RET(s32Ret);

        u32IsoLevel = SCENE_GetLevelLtoH_U32(u32ActRation, g_astScenePipeParam[u8Index].stDynamicFSWDR.u32IsoCnt, g_astScenePipeParam[u8Index].stDynamicFSWDR.au32ExpRation);
        stFSWDRAttr.stWDRCombine.bMotionComp = g_astScenePipeParam[u8Index].stDynamicFSWDR.au8MotionComp[u32IsoLevel];
        u32IsoLevel = SCENE_GetLevelLtoH_U32(u32ISO, g_astScenePipeParam[u8Index].stDynamicFSWDR.u32IsoCnt, g_astScenePipeParam[u8Index].stDynamicFSWDR.au32ISOLtoHThresh);
        stFSWDRAttr.stBnr.u8BnrStr = g_astScenePipeParam[u8Index].stDynamicFSWDR.au8BnrStr[u32IsoLevel];
        stFSWDRAttr.stBnr.enBnrMode = (ISP_BNR_MODE_E)g_astScenePipeParam[u8Index].stDynamicFSWDR.au8BnrMode[u32IsoLevel];

        CHECK_SCENE_PAUSE();
        s32Ret = HI_MPI_ISP_SetFSWDRAttr(ViPipe, &stFSWDRAttr);
        CHECK_SCENE_RET(s32Ret);
    }
    return HI_SUCCESS;
}

HI_S32 HI_SCENE_SetDynamicHDROTEF_AutoGenerate(VI_PIPE ViPipe, VPSS_GRP VpssGrp, VPSS_CHN VpssChn, HI_U32 u32HDRBrightRatio, HI_U8 u8Index)
{
    HI_S32 s32Ret = HI_SUCCESS;

    HDR_OETF_PARAM_S stOETFAttr;

    CHECK_SCENE_PAUSE();
    s32Ret = HI_MPI_HDR_GetOETFParam(ViPipe, VpssGrp, VpssChn, &stOETFAttr);
    CHECK_SCENE_RET(s32Ret);


    CHECK_SCENE_PAUSE();
    s32Ret = HI_MPI_HDR_SetOETFParam(ViPipe, VpssGrp, VpssChn, &stOETFAttr);
    CHECK_SCENE_RET(s32Ret);
    return HI_SUCCESS;
}

HI_S32 HI_SCENE_SetDynamicHDRTM_AutoGenerate(VI_PIPE ViPipe, VPSS_GRP VpssGrp, VPSS_CHN VpssChn, HI_U32 u32HDRBrightRatio, HI_U8 u8Index)
{
    HI_S32 s32Ret = HI_SUCCESS;

    HDR_TM_PARAM_S stTMAttr;

    CHECK_SCENE_PAUSE();
    s32Ret = HI_MPI_HDR_GetTMParam(ViPipe, VpssGrp, VpssChn, &stTMAttr);
    CHECK_SCENE_RET(s32Ret);


    CHECK_SCENE_PAUSE();
    s32Ret = HI_MPI_HDR_SetTMParam(ViPipe, VpssGrp, VpssChn, &stTMAttr);
    CHECK_SCENE_RET(s32Ret);
    return HI_SUCCESS;
}

HI_S32 HI_SCENE_SetDynamicHDRDRC_AutoGenerate(VI_PIPE ViPipe, VPSS_GRP VpssGrp, VPSS_CHN VpssChn, HI_U32 u32HDRBrightRatio, HI_U8 u8Index)
{
    HI_S32 s32Ret = HI_SUCCESS;

    ISP_DRC_ATTR_S stIspDrcAttr;

    CHECK_SCENE_PAUSE();
    s32Ret = HI_MPI_ISP_GetDRCAttr(ViPipe, &stIspDrcAttr);
    CHECK_SCENE_RET(s32Ret);


    CHECK_SCENE_PAUSE();
    s32Ret = HI_MPI_ISP_SetDRCAttr(ViPipe, &stIspDrcAttr);
    CHECK_SCENE_RET(s32Ret);
    return HI_SUCCESS;
}

HI_S32 HI_SCENE_SetDynamicFalseColor_AutoGenerate(VI_PIPE ViPipe, HI_U64 u64Exposure, HI_U64 u64LastExposure, HI_U8 u8Index)
{
    HI_S32 s32Ret = HI_SUCCESS;

    ISP_ANTIFALSECOLOR_ATTR_S stAntiFalsecolorAttr;

    if (u64Exposure != u64LastExposure)
    {
        CHECK_SCENE_PAUSE();
        s32Ret = HI_MPI_ISP_GetAntiFalseColorAttr(ViPipe, &stAntiFalsecolorAttr);
        CHECK_SCENE_RET(s32Ret);


        CHECK_SCENE_PAUSE();
        s32Ret = HI_MPI_ISP_SetAntiFalseColorAttr(ViPipe, &stAntiFalsecolorAttr);
        CHECK_SCENE_RET(s32Ret);
    }
    return HI_SUCCESS;
}

HI_S32 HI_SCENE_SetDynamicDemosaic_AutoGenerate(VI_PIPE ViPipe, HI_U64 u64Exposure, HI_U64 u64LastExposure, HI_U8 u8Index)
{
    HI_S32 s32Ret = HI_SUCCESS;

    ISP_DEMOSAIC_ATTR_S stDemosaicAttr;

    if (u64Exposure != u64LastExposure)
    {
        CHECK_SCENE_PAUSE();
        s32Ret = HI_MPI_ISP_GetDemosaicAttr(ViPipe, &stDemosaicAttr);
        CHECK_SCENE_RET(s32Ret);


        CHECK_SCENE_PAUSE();
        s32Ret = HI_MPI_ISP_SetDemosaicAttr(ViPipe, &stDemosaicAttr);
        CHECK_SCENE_RET(s32Ret);
    }
    return HI_SUCCESS;
}

HI_S32 HI_SCENE_SetDynamicDIS_AutoGenerate(VI_PIPE ViPipe, VI_CHN ViChn, HI_U32 u32Iso, HI_U32 u32LastIso, HI_U8 u8Index, HI_BOOL bEnable)
{
    HI_S32 s32Ret = HI_SUCCESS;

    DIS_ATTR_S stDisAttr;

    if (HI_TRUE != bEnable)
    {
        return HI_SUCCESS;
    }
    if (u32Iso != u32LastIso)
    {
        CHECK_SCENE_PAUSE();
        s32Ret = HI_MPI_VI_GetChnDISAttr(ViPipe, ViChn, &stDisAttr);
        CHECK_SCENE_RET(s32Ret);


        CHECK_SCENE_PAUSE();
        s32Ret = HI_MPI_VI_SetChnDISAttr(ViPipe, ViChn, &stDisAttr);
        CHECK_SCENE_RET(s32Ret);
    }
    return HI_SUCCESS;
}

HI_S32 HI_SCENE_SetThreadDRC_AutoGenerate(VI_PIPE ViPipe, HI_U8 u8Index, HI_U32 u32ActRation, HI_U32 u32Iso, HI_U64 u64Exposure, HI_U64 u64LastExposure)
{
    HI_U32 u32IsoLevel = 0;
    HI_U32 u32RatioLevel = 0;
    HI_U32 i = 0;
    HI_S32 s32Ret = HI_SUCCESS;

    ISP_PUB_ATTR_S stPubAttr;
    ISP_DRC_ATTR_S stIspDrcAttr;
    HI_U8 u8LocalMixingBrightMax = 0;
    HI_U8 u8LocalMixingBrightMin = 0;
    HI_U8 u8LocalMixingDarkMax = 0;
    HI_U8 u8LocalMixingDarkMin = 0;
    HI_S8 s8DetailAdjustFactor = 0;
    HI_U8 u8SpatialFltCoef = 0;
    HI_U8 u8RangeFltCoef = 0;
    HI_U8 u8GradRevMax = 0;
    HI_U8 u8GradRevThr = 0;
    HI_U8 u8Compress = 0;
    HI_U8 u8Stretch = 0;
    HI_U16 u16Strength = 0;
    CHECK_SCENE_PAUSE();
    s32Ret = HI_MPI_ISP_GetPubAttr(ViPipe, &stPubAttr);
    CHECK_SCENE_RET(s32Ret);

    if (WDR_MODE_NONE == stPubAttr.enWDRMode)
    {
        return HI_SUCCESS;
    }
    CHECK_SCENE_PAUSE();
    s32Ret = HI_MPI_ISP_GetDRCAttr(ViPipe, &stIspDrcAttr);
    CHECK_SCENE_RET(s32Ret);

    u32RatioLevel = SCENE_GetLevelLtoH_U32(u32ActRation, g_astScenePipeParam[u8Index].stThreadDrc.u32RationCount, g_astScenePipeParam[u8Index].stThreadDrc.au32RationLevel);
    u32IsoLevel = SCENE_GetLevelLtoH_U32(u32Iso, g_astScenePipeParam[u8Index].stThreadDrc.u32ISOCount, g_astScenePipeParam[u8Index].stThreadDrc.au32ISOLevel);
    if (u32IsoLevel == 0 || u32IsoLevel == g_astScenePipeParam[u8Index].stThreadDrc.u32ISOCount - 1)
    {
        u8LocalMixingBrightMax = g_astScenePipeParam[u8Index].stThreadDrc.au8LocalMixingBrightMax[u32IsoLevel];
    }
    else
    {
        u8LocalMixingBrightMax = SCENE_Interpulate(u32Iso, g_astScenePipeParam[u8Index].stThreadDrc.au32ISOLevel[u32IsoLevel - 1],
                                 g_astScenePipeParam[u8Index].stThreadDrc.au8LocalMixingBrightMax[u32IsoLevel - 1],
                                 g_astScenePipeParam[u8Index].stThreadDrc.au32ISOLevel[u32IsoLevel],
                                 g_astScenePipeParam[u8Index].stThreadDrc.au8LocalMixingBrightMax[u32IsoLevel]);
    }

    if (u32IsoLevel == 0 || u32IsoLevel == g_astScenePipeParam[u8Index].stThreadDrc.u32ISOCount - 1)
    {
        u8LocalMixingBrightMin = g_astScenePipeParam[u8Index].stThreadDrc.au8LocalMixingBrightMin[u32IsoLevel];
    }
    else
    {
        u8LocalMixingBrightMin = SCENE_Interpulate(u32Iso, g_astScenePipeParam[u8Index].stThreadDrc.au32ISOLevel[u32IsoLevel - 1],
                                 g_astScenePipeParam[u8Index].stThreadDrc.au8LocalMixingBrightMin[u32IsoLevel - 1],
                                 g_astScenePipeParam[u8Index].stThreadDrc.au32ISOLevel[u32IsoLevel],
                                 g_astScenePipeParam[u8Index].stThreadDrc.au8LocalMixingBrightMin[u32IsoLevel]);
    }

    if (u32IsoLevel == 0 || u32IsoLevel == g_astScenePipeParam[u8Index].stThreadDrc.u32ISOCount - 1)
    {
        u8LocalMixingDarkMax = g_astScenePipeParam[u8Index].stThreadDrc.au8LocalMixingDarkMax[u32IsoLevel];
    }
    else
    {
        u8LocalMixingDarkMax = SCENE_Interpulate(u32Iso, g_astScenePipeParam[u8Index].stThreadDrc.au32ISOLevel[u32IsoLevel - 1],
                               g_astScenePipeParam[u8Index].stThreadDrc.au8LocalMixingDarkMax[u32IsoLevel - 1],
                               g_astScenePipeParam[u8Index].stThreadDrc.au32ISOLevel[u32IsoLevel],
                               g_astScenePipeParam[u8Index].stThreadDrc.au8LocalMixingDarkMax[u32IsoLevel]);
    }

    if (u32IsoLevel == 0 || u32IsoLevel == g_astScenePipeParam[u8Index].stThreadDrc.u32ISOCount - 1)
    {
        u8LocalMixingDarkMin = g_astScenePipeParam[u8Index].stThreadDrc.au8LocalMixingDarkMin[u32IsoLevel];
    }
    else
    {
        u8LocalMixingDarkMin = SCENE_Interpulate(u32Iso, g_astScenePipeParam[u8Index].stThreadDrc.au32ISOLevel[u32IsoLevel - 1],
                               g_astScenePipeParam[u8Index].stThreadDrc.au8LocalMixingDarkMin[u32IsoLevel - 1],
                               g_astScenePipeParam[u8Index].stThreadDrc.au32ISOLevel[u32IsoLevel],
                               g_astScenePipeParam[u8Index].stThreadDrc.au8LocalMixingDarkMin[u32IsoLevel]);
    }

    if (u32IsoLevel == 0 || u32IsoLevel == g_astScenePipeParam[u8Index].stThreadDrc.u32ISOCount - 1)
    {
        s8DetailAdjustFactor = g_astScenePipeParam[u8Index].stThreadDrc.as8DetailAdjustFactor[u32IsoLevel];
    }
    else
    {
        s8DetailAdjustFactor = SCENE_Interpulate(u32Iso, g_astScenePipeParam[u8Index].stThreadDrc.au32ISOLevel[u32IsoLevel - 1],
                               g_astScenePipeParam[u8Index].stThreadDrc.as8DetailAdjustFactor[u32IsoLevel - 1],
                               g_astScenePipeParam[u8Index].stThreadDrc.au32ISOLevel[u32IsoLevel],
                               g_astScenePipeParam[u8Index].stThreadDrc.as8DetailAdjustFactor[u32IsoLevel]);
    }

    if (u32IsoLevel == 0 || u32IsoLevel == g_astScenePipeParam[u8Index].stThreadDrc.u32ISOCount - 1)
    {
        u8SpatialFltCoef = g_astScenePipeParam[u8Index].stThreadDrc.au8SpatialFltCoef[u32IsoLevel];
    }
    else
    {
        u8SpatialFltCoef = SCENE_Interpulate(u32Iso, g_astScenePipeParam[u8Index].stThreadDrc.au32ISOLevel[u32IsoLevel - 1],
                                             g_astScenePipeParam[u8Index].stThreadDrc.au8SpatialFltCoef[u32IsoLevel - 1],
                                             g_astScenePipeParam[u8Index].stThreadDrc.au32ISOLevel[u32IsoLevel],
                                             g_astScenePipeParam[u8Index].stThreadDrc.au8SpatialFltCoef[u32IsoLevel]);
    }

    if (u32IsoLevel == 0 || u32IsoLevel == g_astScenePipeParam[u8Index].stThreadDrc.u32ISOCount - 1)
    {
        u8RangeFltCoef = g_astScenePipeParam[u8Index].stThreadDrc.au8RangeFltCoef[u32IsoLevel];
    }
    else
    {
        u8RangeFltCoef = SCENE_Interpulate(u32Iso, g_astScenePipeParam[u8Index].stThreadDrc.au32ISOLevel[u32IsoLevel - 1],
                                           g_astScenePipeParam[u8Index].stThreadDrc.au8RangeFltCoef[u32IsoLevel - 1],
                                           g_astScenePipeParam[u8Index].stThreadDrc.au32ISOLevel[u32IsoLevel],
                                           g_astScenePipeParam[u8Index].stThreadDrc.au8RangeFltCoef[u32IsoLevel]);
    }

    if (u32IsoLevel == 0 || u32IsoLevel == g_astScenePipeParam[u8Index].stThreadDrc.u32ISOCount - 1)
    {
        u8GradRevMax = g_astScenePipeParam[u8Index].stThreadDrc.au8GradRevMax[u32IsoLevel];
    }
    else
    {
        u8GradRevMax = SCENE_Interpulate(u32Iso, g_astScenePipeParam[u8Index].stThreadDrc.au32ISOLevel[u32IsoLevel - 1],
                                         g_astScenePipeParam[u8Index].stThreadDrc.au8GradRevMax[u32IsoLevel - 1],
                                         g_astScenePipeParam[u8Index].stThreadDrc.au32ISOLevel[u32IsoLevel],
                                         g_astScenePipeParam[u8Index].stThreadDrc.au8GradRevMax[u32IsoLevel]);
    }

    if (u32IsoLevel == 0 || u32IsoLevel == g_astScenePipeParam[u8Index].stThreadDrc.u32ISOCount - 1)
    {
        u8GradRevThr = g_astScenePipeParam[u8Index].stThreadDrc.au8GradRevThr[u32IsoLevel];
    }
    else
    {
        u8GradRevThr = SCENE_Interpulate(u32Iso, g_astScenePipeParam[u8Index].stThreadDrc.au32ISOLevel[u32IsoLevel - 1],
                                         g_astScenePipeParam[u8Index].stThreadDrc.au8GradRevThr[u32IsoLevel - 1],
                                         g_astScenePipeParam[u8Index].stThreadDrc.au32ISOLevel[u32IsoLevel],
                                         g_astScenePipeParam[u8Index].stThreadDrc.au8GradRevThr[u32IsoLevel]);
    }

    if (u32IsoLevel == 0 || u32IsoLevel == g_astScenePipeParam[u8Index].stThreadDrc.u32ISOCount - 1)
    {
        u8Compress = g_astScenePipeParam[u8Index].stThreadDrc.au8Compress[u32IsoLevel];
    }
    else
    {
        u8Compress = SCENE_Interpulate(u32Iso, g_astScenePipeParam[u8Index].stThreadDrc.au32ISOLevel[u32IsoLevel - 1],
                                       g_astScenePipeParam[u8Index].stThreadDrc.au8Compress[u32IsoLevel - 1],
                                       g_astScenePipeParam[u8Index].stThreadDrc.au32ISOLevel[u32IsoLevel],
                                       g_astScenePipeParam[u8Index].stThreadDrc.au8Compress[u32IsoLevel]);
    }
    ISP_WDR_EXPOSURE_ATTR_S stWDRExpAttr;
    CHECK_SCENE_PAUSE();
    s32Ret = HI_MPI_ISP_GetWDRExposureAttr(ViPipe, &stWDRExpAttr);
    CHECK_SCENE_RET(s32Ret);

#if 0
    if (u32Iso < 400 && u32ActRation > stWDRExpAttr.u32ExpRatioMin)
    {
        u8Compress = u8Compress + (HI_U8)(stWDRExpAttr.u32ExpRatioMax / SCENE_DIV_0TO1(u32ActRation) * 4);
    }
#endif

    if (u32IsoLevel == 0 || u32IsoLevel == g_astScenePipeParam[u8Index].stThreadDrc.u32ISOCount - 1)
    {
        u8Stretch = g_astScenePipeParam[u8Index].stThreadDrc.au8Stretch[u32IsoLevel];
    }
    else
    {
        u8Stretch = SCENE_Interpulate(u32Iso, g_astScenePipeParam[u8Index].stThreadDrc.au32ISOLevel[u32IsoLevel - 1],
                                      g_astScenePipeParam[u8Index].stThreadDrc.au8Stretch[u32IsoLevel - 1],
                                      g_astScenePipeParam[u8Index].stThreadDrc.au32ISOLevel[u32IsoLevel],
                                      g_astScenePipeParam[u8Index].stThreadDrc.au8Stretch[u32IsoLevel]);
    }

    u16Strength = g_astScenePipeParam[u8Index].stThreadDrc.u16AutoStrength[u32IsoLevel][u32RatioLevel];
    stIspDrcAttr.u8DetailDarkStep = 0;

    HI_U32 u32ExpNum = 0;
    HI_U32 u32ExpLevel = 0;
    HI_U32 u32CurAlpha = 0;

    HI_U16 au16ToneMappingValue[200] = {0};
    u32ExpNum = g_astScenePipeParam[u8Index].stThreadDrc.u32ExpCount;

    u32ExpLevel = SCENE_GetLevelLtoH(u64Exposure, g_astScenePipeParam[u8Index].stThreadDrc.u32ExpCount , g_astScenePipeParam[u8Index].stThreadDrc.au64ExpThreshLtoH);
    if (u32ExpLevel == 0 || u32ExpLevel == u32ExpNum - 1)
    {
        u32CurAlpha = g_astScenePipeParam[u8Index].stThreadDrc.u32Aplha[u32ExpLevel];
    }
    else
    {
        u32CurAlpha = SCENE_Interpulate(u64Exposure,
                                        g_astScenePipeParam[u8Index].stThreadDrc.au64ExpThreshLtoH[u32ExpLevel - 1],
                                        g_astScenePipeParam[u8Index].stThreadDrc.u32Aplha[u32ExpLevel - 1],
                                        g_astScenePipeParam[u8Index].stThreadDrc.au64ExpThreshLtoH[u32ExpLevel],
                                        g_astScenePipeParam[u8Index].stThreadDrc.u32Aplha[u32ExpLevel]);
    }

    stIspDrcAttr.enCurveSelect = DRC_CURVE_USER;
    for (i = 0 ; i < 200 ; i++)
    {
        au16ToneMappingValue[i] = ((HI_U32)u32CurAlpha * g_astScenePipeParam[u8Index].stThreadDrc.au16ToneMappingValue1[i]
                                                + (HI_U32)(0x100 - u32CurAlpha) * g_astScenePipeParam[u8Index].stThreadDrc.au16ToneMappingValue2[i]) >> 8;
    }

    stIspDrcAttr.bEnable = g_astScenePipeParam[u8Index].stThreadDrc.bEnable;
    for (i = 0; i < g_astScenePipeParam[u8Index].stThreadDrc.u32Interval; i++)
    {
        stIspDrcAttr.u8LocalMixingBrightMax = SCENE_TimeFilter(stIspDrcAttr.u8LocalMixingBrightMax,
                                              u8LocalMixingBrightMax, g_astScenePipeParam[u8Index].stThreadDrc.u32Interval, i);
        stIspDrcAttr.u8LocalMixingBrightMin = SCENE_TimeFilter(stIspDrcAttr.u8LocalMixingBrightMin,
                                              u8LocalMixingBrightMin, g_astScenePipeParam[u8Index].stThreadDrc.u32Interval, i);
        stIspDrcAttr.u8LocalMixingDarkMax = SCENE_TimeFilter(stIspDrcAttr.u8LocalMixingDarkMax,
                                            u8LocalMixingDarkMax, g_astScenePipeParam[u8Index].stThreadDrc.u32Interval, i);
        stIspDrcAttr.u8LocalMixingDarkMin = SCENE_TimeFilter(stIspDrcAttr.u8LocalMixingDarkMin,
                                            u8LocalMixingDarkMin, g_astScenePipeParam[u8Index].stThreadDrc.u32Interval, i);
        stIspDrcAttr.s8DetailAdjustFactor = SCENE_TimeFilter(stIspDrcAttr.s8DetailAdjustFactor,
                                            s8DetailAdjustFactor, g_astScenePipeParam[u8Index].stThreadDrc.u32Interval, i);
        stIspDrcAttr.u8SpatialFltCoef = SCENE_TimeFilter(stIspDrcAttr.u8SpatialFltCoef,
                                        u8SpatialFltCoef, g_astScenePipeParam[u8Index].stThreadDrc.u32Interval, i);
        stIspDrcAttr.u8RangeFltCoef = SCENE_TimeFilter(stIspDrcAttr.u8RangeFltCoef,
                                      u8RangeFltCoef, g_astScenePipeParam[u8Index].stThreadDrc.u32Interval, i);
        stIspDrcAttr.u8GradRevMax = SCENE_TimeFilter(stIspDrcAttr.u8GradRevMax,
                                    u8GradRevMax, g_astScenePipeParam[u8Index].stThreadDrc.u32Interval, i);
        stIspDrcAttr.u8GradRevThr = SCENE_TimeFilter(stIspDrcAttr.u8GradRevThr,
                                    u8GradRevThr, g_astScenePipeParam[u8Index].stThreadDrc.u32Interval, i);
        stIspDrcAttr.stAsymmetryCurve.u8Compress = SCENE_TimeFilter(stIspDrcAttr.stAsymmetryCurve.u8Compress,
                u8Compress, g_astScenePipeParam[u8Index].stThreadDrc.u32Interval, i);
        stIspDrcAttr.stAsymmetryCurve.u8Stretch = SCENE_TimeFilter(stIspDrcAttr.stAsymmetryCurve.u8Stretch,
                u8Stretch, g_astScenePipeParam[u8Index].stThreadDrc.u32Interval, i);
        stIspDrcAttr.stAuto.u16Strength = SCENE_TimeFilter(stIspDrcAttr.stAuto.u16Strength,
                                               u16Strength, g_astScenePipeParam[u8Index].stThreadDrc.u32Interval, i);
        HI_U32 j = 0;
        for(j = 0 ; j < 200 ; j++)
        {
            stIspDrcAttr.au16ToneMappingValue[j]  = SCENE_TimeFilter(stIspDrcAttr.au16ToneMappingValue[j],
        		                          au16ToneMappingValue[j], g_astScenePipeParam[u8Index].stThreadDrc.u32Interval,i);
        }
        CHECK_SCENE_PAUSE();
        s32Ret = HI_MPI_ISP_SetDRCAttr(ViPipe, &stIspDrcAttr);
        CHECK_SCENE_RET(s32Ret);
        usleep(80000);
    }
    return HI_SUCCESS;
}

HI_S32 HI_SCENE_GetModuleState_AutoGenerate(HI_SCENE_MODULE_STATE_S *pstModuleState)
{
    if (HI_NULL == pstModuleState)
    {
        printf("null pointer");
        return HI_FAILURE;
    }

    pstModuleState->bStaticAE = 1;
    pstModuleState->bStaticAERoute = 0;
    pstModuleState->bStaticAERouteEx = 1;
    pstModuleState->bStaticGlobalCac = 1;
    pstModuleState->bStaticWdrExposure = 1;
    pstModuleState->bStaticDrc = 1;
    pstModuleState->bStaticDehaze = 1;
    pstModuleState->bStaticLdci = 0;
    pstModuleState->bStaticAWB = 0;
    pstModuleState->bStaticAWBEx = 0;
    pstModuleState->bStaticSaturation = 0;
    pstModuleState->bStaticCCM = 0;
    pstModuleState->bStaticThreeDNR = 1;
    pstModuleState->bStaticNr = 0;
    pstModuleState->bStaticShading = 1;
    pstModuleState->bStaticCSC = 0;
    pstModuleState->bStaticStatistics = 1;
    pstModuleState->bDyanamicGamma = 1;
    pstModuleState->bDynamicAE = 1;
    pstModuleState->bDyanamicFalseColor = 0;
    pstModuleState->bDyanamicDemosaic = 0;
    pstModuleState->bDynamicShading = 1;
    pstModuleState->bDynamicDehaze = 1;
    pstModuleState->bDynamicFSWDR = 1;
    pstModuleState->bDynamicHDRTM = 0;
    pstModuleState->bDynamicHDROETF = 0;
    pstModuleState->bDynamicHDRDRC = 0;
    pstModuleState->bDynamicLdci = 1;
    pstModuleState->bThreadDrc = 1;
    return HI_SUCCESS;
}

HI_S32 HI_SCENE_SetPipeParam_AutoGenerate(const HI_SCENE_PIPE_PARAM_S *pstScenePipeParam, HI_U32 u32Num)
{
    if (HI_NULL == pstScenePipeParam)
    {
        printf("null pointer");
        return HI_FAILURE;
    }

    memcpy(g_astScenePipeParam, pstScenePipeParam, sizeof(HI_SCENE_PIPE_PARAM_S) * u32Num);

    return HI_SUCCESS;
}

HI_S32 HI_SCENE_SetPause_AutoGenerate(HI_BOOL bPause)
{
    g_bISPPause = bPause;
    return HI_SUCCESS;
}

HI_S32 HI_SCENE_GetPipeAERoute_AutoGenerate(HI_S32 s32PipeIndex, HI_U8 au8AEWeight[][AE_ZONE_COLUMN])
{
    HI_S32 i, j = 0;
    for (i = 0; i < AE_ZONE_ROW; i++)
    {
        for (j = 0; j < AE_ZONE_COLUMN; j++)
        {
            au8AEWeight[i][j] = g_astScenePipeParam[s32PipeIndex].stStaticStatistics.au8AEWeight[i][j];
        }
    }

    return HI_SUCCESS;
}


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
