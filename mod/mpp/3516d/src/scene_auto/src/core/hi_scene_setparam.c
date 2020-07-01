#include "hi_scene_setparam.h"
#include <unistd.h>
#include <string.h>
#include "mpi_awb.h"
#include "mpi_ae.h"
#include "mpi_isp.h"
#include "mpi_vi.h"
#include "mpi_vpss.h"
#include "mpi_venc.h"
#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define PIC_WIDTH_1080P   1920
#define PIC_HEIGHT_1080P  1080

#define LCU_ALIGN_H265 64
#define MB_ALIGN_H264 16
#define SCENE_MAX(a, b) (((a) < (b)) ? (b) : (a))

#define SCENE_DIV_0TO1(a)  ((0 == (a)) ? 1 : (a))

#define SCENE_SHADING_TRIGMODE_L2H            (1)

#define SCENE_SHADING_TRIGMODE_H2L            (2)

#define SCENE_MAX_ISP_PIPE_NUM                (6)

#define SCENE_3DNR_PRINT_ENABLE             (0)

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

static  HI_U32 SCENE_GetLevelLtoH(HI_U64 u64Value, HI_U32 u32Count, HI_U64* pu64Thresh)
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
static  HI_U32 SCENE_GetLevelHtoL(HI_U64 u64Value, HI_U32 u32Count, HI_U64* pu64Thresh)
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


static HI_U32 SCENE_GetLevelLtoH_U32(HI_U32 u32Value, HI_U32 u32Count, HI_U32* pu32Thresh)
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

static  HI_U32 SCENE_GetLevelHtoL_U32(HI_U32 u32Value, HI_U32 u32Count, HI_U32* pu32Thresh)
{
    HI_U32 u32Level = 0;

    for (u32Level = u32Count; u32Level > 0; u32Level--)
    {
        if (u32Value > pu32Thresh[u32Level - 1])
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
        u32Value = u32Para0 - (HI_U32)u64Temp - 1;
    }
    else if (u32Para0 < u32Para1)
    {
        u64Temp = (HI_U64)(u32Para1 - u32Para0) << 8;
        u64Temp = (u64Temp * (u32Index + 1)) / SCENE_DIV_0TO1(u32TimeCnt) >> 8;
        u32Value = u32Para0 + (HI_U32)u64Temp + 1;
    }
    else
    {
        u32Value = u32Para0;
    }

    return u32Value;
}

HI_S32 HI_SCENE_SetStaticAE_AutoGenerate(VI_PIPE ViPipe, HI_U8 u8Index)
{
	if(HI_TRUE != g_astScenePipeParam[u8Index].stModuleState.bStaticAE)
	{
		return HI_SUCCESS;
	}

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

HI_S32 HI_SCENE_SetStaticAWB_AutoGenerate(VI_PIPE ViPipe, HI_U8 u8Index)
{
	if(HI_TRUE != g_astScenePipeParam[u8Index].stModuleState.bStaticAWB)
	{
		return HI_SUCCESS;
	}

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
	if(HI_TRUE != g_astScenePipeParam[u8Index].stModuleState.bStaticAWBEx)
	{
		return HI_SUCCESS;
	}

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
	if(HI_TRUE != g_astScenePipeParam[u8Index].stModuleState.bStaticGlobalCac)
	{
		return HI_SUCCESS;
	}

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
	if(HI_TRUE != g_astScenePipeParam[u8Index].stModuleState.bStaticWdrExposure)
	{
		return HI_SUCCESS;
	}

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
	if(HI_TRUE != g_astScenePipeParam[u8Index].stModuleState.bStaticDrc)
	{
		return HI_SUCCESS;
	}

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

    for (i = 0; i < HI_ISP_DRC_CC_NODE_NUM; i++)
    {
        stDrcAttr.au16ColorCorrectionLut[i] = g_astScenePipeParam[u8Index].stStaticDrc.au16ColorCorrectionLut[i];
    }

    s32Ret = HI_MPI_ISP_SetDRCAttr(ViPipe, &stDrcAttr);
    CHECK_SCENE_RET(s32Ret);
    return HI_SUCCESS;
}

HI_S32 HI_SCENE_SetStaticLDCI_AutoGenerate(VI_PIPE ViPipe, HI_U8 u8Index)
{
	if(HI_TRUE != g_astScenePipeParam[u8Index].stModuleState.bStaticLdci)
	{
		return HI_SUCCESS;
	}

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
	if(HI_TRUE != g_astScenePipeParam[u8Index].stModuleState.bStaticDehaze)
	{
		return HI_SUCCESS;
	}

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

HI_S32 HI_SCENE_SetStaticStatisticsCfg_AutoGenerate(VI_PIPE ViPipe, HI_U8 u8Index, HI_BOOL bMetryFixed)
{
	if(HI_TRUE != g_astScenePipeParam[u8Index].stModuleState.bStaticStatistics)
	{
		return HI_SUCCESS;
	}

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
	if(HI_TRUE != g_astScenePipeParam[u8Index].stModuleState.bStaticSaturation)
	{
		return HI_SUCCESS;
	}

    HI_S32 i = 0;
    HI_S32 s32Ret = HI_SUCCESS;

    ISP_SATURATION_ATTR_S stSaturationAttr;

    s32Ret = HI_MPI_ISP_GetSaturationAttr(ViPipe, &stSaturationAttr);
    CHECK_SCENE_RET(s32Ret);

    for (i = 0; i < ISP_AUTO_ISO_STRENGTH_NUM; i++)
    {
        stSaturationAttr.stAuto.au8Sat[i] = g_astScenePipeParam[u8Index].stStaticSaturation.au8AutoSat[i];
    }

    s32Ret = HI_MPI_ISP_SetSaturationAttr(ViPipe, &stSaturationAttr);
    CHECK_SCENE_RET(s32Ret);
    return HI_SUCCESS;
}

HI_S32 HI_SCENE_SetStaticCCM_AutoGenerate(VI_PIPE ViPipe, HI_U8 u8Index)
{
	if(HI_TRUE != g_astScenePipeParam[u8Index].stModuleState.bStaticCCM)
	{
		return HI_SUCCESS;
	}

    HI_S32 s32Ret = HI_SUCCESS;

    ISP_COLORMATRIX_ATTR_S stColormatrixAttr;

    s32Ret = HI_MPI_ISP_GetCCMAttr(ViPipe, &stColormatrixAttr);
    CHECK_SCENE_RET(s32Ret);

    stColormatrixAttr.stAuto.bISOActEn = g_astScenePipeParam[u8Index].stStaticCcm.bISOActEn;
    stColormatrixAttr.stAuto.bTempActEn = g_astScenePipeParam[u8Index].stStaticCcm.bTempActEn;

    s32Ret = HI_MPI_ISP_SetCCMAttr(ViPipe, &stColormatrixAttr);
    CHECK_SCENE_RET(s32Ret);
    return HI_SUCCESS;
}

HI_S32 HI_SCENE_SetStaticShading_AutoGenerate(VI_PIPE ViPipe, HI_U8 u8Index)
{
	if(HI_TRUE != g_astScenePipeParam[u8Index].stModuleState.bStaticShading)
	{
		return HI_SUCCESS;
	}

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
	if(HI_TRUE != g_astScenePipeParam[u8Index].stModuleState.bDynamicGamma)
	{
		return HI_SUCCESS;
	}

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
	if(HI_TRUE != g_astScenePipeParam[u8Index].stModuleState.bDynamicGamma)
	{
		return HI_SUCCESS;
	}

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

HI_S32 HI_SCENE_SetDynamicVencBitrate_AutoGenerate(VI_PIPE ViPipe, HI_U64 u64Iso, HI_U64 u64LastIso, HI_U8 u8Index)
{
     if(HI_TRUE != g_astScenePipeParam[u8Index].stModuleState.bDynamicIsoVenc)
     {
     	return HI_SUCCESS;
     }

    HI_U32 u32ExpLevel = 0;
    HI_S32 s32Ret = HI_SUCCESS;
    HI_U32 u32Iso = u64Iso;
    HI_U32 u32LastIso = u64LastIso;

    VENC_CHN_ATTR_S stVencChnAttr;
    VENC_RC_PARAM_S stRcParam;

    PAYLOAD_TYPE_E  enType;

    if (u32Iso != u32LastIso)
    {
        CHECK_SCENE_PAUSE();
        s32Ret = HI_MPI_VENC_GetChnAttr(ViPipe, &stVencChnAttr);
	CHECK_SCENE_RET(s32Ret);
	enType  = stVencChnAttr.stVencAttr.enType;
	//printf("u32IsoThreshCnt = %d\n",g_astScenePipeParam[u8Index].stDynamicVencBitrate.u32IsoThreshCnt);
        s32Ret = HI_MPI_VENC_GetRcParam(ViPipe, &stRcParam);
        CHECK_SCENE_RET(s32Ret);

        u32ExpLevel = SCENE_GetLevelLtoH_U32(u32Iso, g_astScenePipeParam[u8Index].stDynamicVencBitrate.u32IsoThreshCnt, g_astScenePipeParam[u8Index].stDynamicVencBitrate.au32IsoThreshLtoH);

        if (u32ExpLevel == 0 || u32ExpLevel == g_astScenePipeParam[u8Index].stDynamicVencBitrate.u32IsoThreshCnt - 1)
        {
            if(enType == PT_H264 )
            {
                stRcParam.stParamH264AVbr.s32ChangePos = g_astScenePipeParam[u8Index].stDynamicVencBitrate.au16ManualPercent[u32ExpLevel];
            }
	    else
            {
                stRcParam.stParamH265AVbr.s32ChangePos = g_astScenePipeParam[u8Index].stDynamicVencBitrate.au16ManualPercent[u32ExpLevel];
            }

        }
        else
        {
            if(enType == PT_H264 )
            {
                 stRcParam.stParamH264AVbr.s32ChangePos = SCENE_Interpulate(u32Iso,
                            g_astScenePipeParam[u8Index].stDynamicVencBitrate.au32IsoThreshLtoH[u32ExpLevel - 1],
                            g_astScenePipeParam[u8Index].stDynamicVencBitrate.au16ManualPercent[u32ExpLevel - 1],
                            g_astScenePipeParam[u8Index].stDynamicVencBitrate.au32IsoThreshLtoH[u32ExpLevel],
                            g_astScenePipeParam[u8Index].stDynamicVencBitrate.au16ManualPercent[u32ExpLevel]);
            }
	    else
            {
                 stRcParam.stParamH265AVbr.s32ChangePos = SCENE_Interpulate(u32Iso,
                            g_astScenePipeParam[u8Index].stDynamicVencBitrate.au32IsoThreshLtoH[u32ExpLevel - 1],
                            g_astScenePipeParam[u8Index].stDynamicVencBitrate.au16ManualPercent[u32ExpLevel - 1],
                            g_astScenePipeParam[u8Index].stDynamicVencBitrate.au32IsoThreshLtoH[u32ExpLevel],
                            g_astScenePipeParam[u8Index].stDynamicVencBitrate.au16ManualPercent[u32ExpLevel]);
            }

        }

        CHECK_SCENE_PAUSE();

        s32Ret =  HI_MPI_VENC_SetRcParam(ViPipe, &stRcParam);
        CHECK_SCENE_RET(s32Ret);
    }

    return HI_SUCCESS;
}

extern NR_MOTION_MODE_E g_enGmc;

#if SCENE_3DNR_PRINT_ENABLE
static void PrintNRxParam(VI_PIPE_NRX_PARAM_S* pstVINRXParam, VPSS_GRP_NRX_PARAM_S* pstVPSSNRXParam)
{
    VI_PIPE_NRX_PARAM_V2_S* pVIX = &pstVINRXParam->stNRXParamV2.stNRXManualV2.stNRXParamV2;
    VPSS_NRX_V2_S* pVPX = &pstVPSSNRXParam->stNRXParam_V2.stNRXManual.stNRXParam;

    tV500_VI_IEy*  vi_pi   = &( pVIX->IEy );
    tV500_VI_SFy*  vi_ps   = &( pVIX->SFy );
    tV500_VPSS_IEy*  pi = pVPX->IEy;
    tV500_VPSS_SFy*  ps = pVPX->SFy;
    tV500_VPSS_MDy*  pm = pVPX->MDy;
    tV500_VPSS_TFy*  pt = pVPX->TFy;
    tV500_VPSS_RFs*  pr = &pVPX->RFs;

#undef  _tmprt1_
#define _tmprt1_( ps,    X )  vi_ps->X, ps[0].X, ps[1].X, ps[2].X
#undef  _tmprt1y
#define _tmprt1y( ps,    X )  ps[0].X, ps[1].X
#undef  _tmprt1z
#define _tmprt1z( ps,a,b,X )  ps[a].X, ps[b].X

#undef  _tmprt2_
#define _tmprt2_( ps,    X,Y )  vi_ps->X, vi_ps->Y,  ps[0].X,ps[0].Y,  ps[1].X,ps[1].Y,  ps[2].X, ps[2].Y
#undef  _tmprt2y
#define _tmprt2y( ps,    X,Y )  vi_ps->X, vi_ps->Y,  ps[0].X,ps[0].Y,  ps[1].X,ps[1].Y
#undef  _tmprt2z
#define _tmprt2z( ps,a,b,X,Y )  ps[a].X,ps[a].Y,  ps[b].X,ps[b].Y

#undef  _t3_
#define _t3_( ie, n,   X,Y,Z )            ie[n].X, ie[n].Y, ie[n].Z
#undef  _t4_
#define _t4_( ie, n, K,X,Y,Z )   ie[n].K, ie[n].X, ie[n].Y, ie[n].Z

#undef  _tmprt3z
#define _tmprt3z( ps,a,b,X,Y,Z )  _t3_( ps,a,  X,Y,Z ),_t3_( ps,b,  X,Y,Z )
#undef  _tmprt3y
#define _tmprt3y( ps,    X,Y,Z )  _t3_( ps,0,  X,Y,Z ),_t3_( ps,1,  X,Y,Z ),_t3_( ps,2,  X,Y,Z )
#undef  _tmprt3x
#define _tmprt3x( ps,    X,Y,Z )  _t3_( ps,1,  X,Y,Z ),_t3_( ps,2,  X,Y,Z ),_t3_( ps,3,  X,Y,Z )
#undef  _tmprt3_
#define _tmprt3_( ps,    X,Y,Z )  _t3_( ps,0,  X,Y,Z ),_t3_( ps,1,  X,Y,Z ),_t3_( ps,2,  X,Y,Z ),_t3_( ps,3,  X,Y,Z )
#undef  _tmprt3x_
#define _tmprt3x_( ps,    X,Y,Z )  vi_ps->X, vi_ps->Y, vi_ps->Z ,_t3_( ps,0,  X,Y,Z ),_t3_( ps,1,  X,Y,Z ),_t3_( ps,2,  X,Y,Z )
#undef  _tmprt4_
#define _tmprt4_( ps,  K,X,Y,Z )  _t4_( ps,0,K,X,Y,Z ),_t4_( ps,1,K,X,Y,Z ),_t4_( ps,2,K,X,Y,Z ),_t4_( ps,3,K,X,Y,Z )
#undef  _tmprt4x_
#define _tmprt4x_( ps,  K,X,Y,Z )  vi_ps->K,vi_ps->X,vi_ps->Y,vi_ps->Z,_t4_( ps,0,K,X,Y,Z ),_t4_( ps,1,K,X,Y,Z ),_t4_( ps,2,K,X,Y,Z )

    //printf("\n\n                             sizeof( X ) = %d ", sizeof(*pX) );
    printf("\n****************************NRy (n0sfx to n3sfx)******************************");
    printf("\n -en                    |             %3d |             %3d |             %3d ", ps[0].NRyEn, ps[1].NRyEn, ps[2].NRyEn);
    printf("\n -nXsf1     %3d:%3d:%3d |     %3d:%3d:%3d |     %3d:%3d:%3d |     %3d:%3d:%3d ", vi_ps->SFS1, vi_ps->SFT1, vi_ps->SBR1, _tmprt3y( ps, SFS1, SFT1, SBR1));
    printf("\n -nXsf2     %3d:%3d:%3d |     %3d:%3d:%3d |     %3d:%3d:%3d |     %3d:%3d:%3d ", vi_ps->SFS2, vi_ps->SFT2, vi_ps->SBR2, _tmprt3y( ps, SFS2, SFT2, SBR2 ));
    printf("\n -nXsf4     %3d:%3d:%3d |     %3d:%3d:%3d |     %3d:%3d:%3d |     %3d:%3d:%3d ", vi_ps->SFS4, vi_ps->SFT4, vi_ps->SBR4, _tmprt3y( ps, SFS4, SFT4, SBR4 ));
    printf("\n -bwsf4             %3d |             %3d |                 |                 ", vi_ps->BWSF4, ps[0].BWSF4);
    printf("\n -kmsf4                 |                 |             %3d |             %3d ", ps[1].kMode, ps[2].kMode);
    printf("\n -nXsf5 %3d:%3d:%3d:%3d | %3d:%3d:%3d:%3d | %3d:%3d:%3d:%3d | %3d:%3d:%3d:%3d ", vi_pi->IES0, vi_pi->IES1, vi_pi->IES2, vi_pi->IES3 ,
           _t4_( pi, 0, IES0, IES1, IES2, IES3),
           _t4_( pi, 1, IES0, IES1, IES2, IES3),
           _t4_( pi, 2, IES0, IES1, IES2, IES3));
    printf("\n -dzsf5             %3d |             %3d |             %3d |             %3d ", vi_pi->IEDZ, pi[0].IEDZ, pi[1].IEDZ, pi[2].IEDZ);
    printf("\n -nXsf6 %3d:%3d:%3d:%3d | %3d:%3d:%3d:%3d | %3d:%3d:%3d:%3d | %3d:%3d:%3d:%3d ", _tmprt4x_(ps, SPN6, SBN6, PBR6, JMODE));
    printf("\n -nXsfr6    %3d:%3d:%3d |     %3d:%3d:%3d |     %3d:%3d:%3d |     %3d:%3d:%3d ", _tmprt3x_(ps, SFR6[0], SFR6[1], SFR6[2]));
    printf("\n                        |                 |                 |                 ");
    printf("\n -SelRt         %3d:%3d |                 |                 |                 ", vi_ps->SRT0, vi_ps->SRT1);
    printf("\n -DeRt          %3d:%3d |                 |                 |                 ", vi_ps->DeRate, vi_ps->DeIdx);
    printf("\n                        |                 |                 |                 ");
    printf("\n -TriTh             %3d |             %3d |             %3d |             %3d ", vi_ps->TriTh, ps[0].TriTh, ps[1].TriTh, ps[2].TriTh);

    if ( ps->TriTh )
    {
        printf("\n -nXsfn %3d:%3d:%3d:%3d | %3d:%3d:%3d:%3d | %3d:%3d:%3d:%3d | %3d:%3d:%3d:%3d ", _tmprt4x_(ps, SFN0, SFN1, SFN2, SFN3));
        printf("\n -nXsth     %3d:%3d:%3d |     %3d:%3d:%3d |     %3d:%3d:%3d |     %3d:%3d:%3d ", _tmprt3x_(ps, STH1, STH2, STH3));
    }
    else
    {
        printf("\n -nXsfn     %3d:%3d:%3d |     %3d:%3d:%3d |     %3d:%3d:%3d |     %3d:%3d:%3d ", _tmprt3x_(ps, SFN0, SFN1, SFN3));
        printf("\n -nXsth         %3d:%3d |         %3d:%3d |         %3d:%3d |         %3d:%3d ", _tmprt2_(ps, STH1, STH3));
    }

    printf("\n -sfr    (0)        %3d |             %3d |             %3d |             %3d ", _tmprt1_(ps, SFR));
    printf("\n                        |                 |                 |                 ");
    printf("\n -tedge                 |             %3d |             %3d |                 ", pt[0].tEdge, pt[1].tEdge);
    printf("\n                        |                 |                 |                 ");
#ifdef NR_MC_SUPPORT
    printf("\n -ref                   |         %3d:%3d |                 |                 ", pt[0].bRef, g_enGmc);
#else
    printf("\n -ref                   |             %3d |                 |                 ", pt[0].bRef);
#endif
    printf("\n -refUpt                |             %3d |                 |                 ", pr->RFUI);
    printf("\n -rftIdx                |             %3d |                 |                 ", pt[0].RFI);
    printf("\n -refCtl                |         %3d:%3d |                 |                 ", pr->RFDZ, pr->RFSLP);
    printf("\n                        |                 |                 |                 ");
    printf("\n -biPath                |             %3d |             %3d |                 ", pm[0].biPath, pm[1].biPath);
    printf("\n -nXstr   (1)           |         %3d:%3d |         %3d:%3d |                 ", _tmprt2z(pt, 0, 1, STR0, STR1));
    printf("\n -nXsdz                 |         %3d:%3d |         %3d:%3d |                 ", _tmprt2z(pt, 0, 1, SDZ0, SDZ1));
    printf("\n                        |                 |                 |                 ");
    printf("\n -nXtss                 |         %3d:%3d |         %3d:%3d |                 ", _tmprt2z(pt, 0, 1, TSS0, TSS1));
    printf("\n -nXtsi                 |         %3d:%3d |         %3d:%3d |                 ", _tmprt2z(pt, 0, 1, TSI0, TSI1));
    printf("\n -nXtfs                 |         %3d:%3d |         %3d:%3d |                 ", _tmprt2z(pt, 0, 1, TFS0, TFS1));
	printf("\n -nXdzm                 |         %3d:%3d |         %3d:%3d |                 ", _tmprt2z( pt, 0, 1, DZMode0, DZMode1));
    printf("\n -nXtdz   (3)           |         %3d:%3d |         %3d:%3d |                 ", _tmprt2z(pt, 0, 1, TDZ0, TDZ1));
    printf("\n -nXtdx                 |         %3d:%3d |         %3d:%3d |                 ", _tmprt2z(pt, 0, 1, TDX0, TDX1));
    printf("\n                        |                 |                 |                 ");
    printf("\n -nXtfr0  (2)           |     %3d:%3d:%3d |     %3d:%3d:%3d |                 ", _tmprt3z(pt, 0, 1, TFR0[0], TFR0[1], TFR0[2]));
    printf("\n                        |     %3d:%3d:%3d |     %3d:%3d:%3d |                 ", _tmprt3z(pt, 0, 1, TFR0[3], TFR0[4], TFR0[5]));
    printf("\n -nXtfr1  (2)           |     %3d:%3d:%3d |     %3d:%3d:%3d |                 ", _tmprt3z(pt, 0, 1, TFR1[0], TFR1[1], TFR1[2]));
    printf("\n                        |     %3d:%3d:%3d |     %3d:%3d:%3d |                 ", _tmprt3z(pt, 0, 1, TFR1[3], TFR1[4], TFR1[5]));
    printf("\n                        |                 |                 |                 ");
    printf("\n -mXid0                 |     %3d:%3d:%3d |     %3d:%3d:%3d |                 ", _tmprt3z( pm, 0, 1, MAI00, MAI01, MAI02 ));
    printf("\n -mXid1                 |     %3d:%3d:%3d |     %3d:%3d:%3d |                 ", _tmprt3z( pm, 0, 1, MAI10, MAI11, MAI12 ));
    printf("\n -mXmadz                |         %3d:%3d |         %3d:%3d |                 ", pm[0].MADZ0, pm[0].MADZ1, pm[1].MADZ0, pm[1].MADZ1);
	printf("\n -mXmabr                |         %3d:%3d |         %3d:%3d |                 ", pm[0].MABR0, pm[0].MABR1, pm[1].MABR0, pm[1].MABR1);
    printf("\n                        |                 |                 +----------------+");
    printf("\n -AdvMath               |         %3d     |                 |  -sfc      %3d  ", pr->advMATH, pVPX->pNRc.SFC);
    printf("\n -mXmath                |         %3d:%3d |         %3d:%3d |                 ", _tmprt2z(pm, 0, 1, MATH0, MATH1));
    printf("\n                        |                 |                 |  -ctfs     %3d  ", pVPX->pNRc.CTFS);
    printf("\n                        |                 |                 |  -tfc      %3d  ", pVPX->pNRc.TFC);
    printf("\n -mXmate                |         %3d:%3d |         %3d:%3d |                 ", _tmprt2z(pm, 0, 1, MATE0, MATE1));
    printf("\n -mXmabw                |         %3d:%3d |         %3d:%3d |                 ", _tmprt2z(pm, 0, 1, MABW0, MABW1));
    printf("\n -mXmatw                |             %3d |             %3d |                 ",  pm[0].MATW, pm[1].MATW);
	printf("\n -mXmasw                |             %3d |             %3d |                 ",  pm[0].MASW, pm[1].MASW);
    printf("\n******************************************************************************\n");

    if (pm[0].MADZ0 > 0 || pm[0].MADZ1 > 0)
    {
        const HI_U8* p = pm[0].MABR0;
        //printf("\n----------------------+-----------------+-----------------+------------------- ");
        //printf("\n  m1mabr0 -m1r00    %6d %6d %6d %6d    %6d %6d %6d %6d ", p[0x00], p[0x01], p[0x02], p[0x03], p[0x04], p[0x05], p[0x06], p[0x07]);
        //printf("\n          -m1r08    %6d %6d %6d %6d    %6d %6d %6d %6d ", p[0x08], p[0x09], p[0x0A], p[0x0B], p[0x0C], p[0x0D], p[0x0E], p[0x0F]);
        p = pm[0].MABR1;
        //printf("\n");
        //printf("\n  m1mabr1 -m1r10    %6d %6d %6d %6d    %6d %6d %6d %6d ", p[0x00], p[0x01], p[0x02], p[0x03], p[0x04], p[0x05], p[0x06], p[0x07]);
        //printf("\n          -m1r18    %6d %6d %6d %6d    %6d %6d %6d %6d ", p[0x08], p[0x09], p[0x0A], p[0x0B], p[0x0C], p[0x0D], p[0x0E], p[0x0F]);
    }

    if (pm[1].MADZ0 > 0 || pm[1].MADZ1 > 0)
    {
        const HI_U8* p = pm[1].MABR0;
        //printf("\n");
        //printf("\n  m2mabr0 -m2r10    %6d %6d %6d %6d    %6d %6d %6d %6d ", p[0x00], p[0x01], p[0x02], p[0x03], p[0x04], p[0x05], p[0x06], p[0x07]);
        //printf("\n          -m2r18    %6d %6d %6d %6d    %6d %6d %6d %6d ", p[0x08], p[0x09], p[0x0A], p[0x0B], p[0x0C], p[0x0D], p[0x0E], p[0x0F]);
        p = pm[1].MABR1;
        //printf("\n");
        //printf("\n  m2mabr1 -m2r10    %6d %6d %6d %6d    %6d %6d %6d %6d ", p[0x00], p[0x01], p[0x02], p[0x03], p[0x04], p[0x05], p[0x06], p[0x07]);
        //printf("\n          -m2r18    %6d %6d %6d %6d    %6d %6d %6d %6d ", p[0x08], p[0x09], p[0x0A], p[0x0B], p[0x0C], p[0x0D], p[0x0E], p[0x0F]);
    }

    if (ps[1].kMode > 1 || ps[2].kMode > 1)
    {
        const HI_U16*  p;
        printf("\n----------------------+-----------------+-----------------+-------------------");

        if (ps[1].kMode > 1)
        {
            p = ps[1].SBSk;
            printf("\n");
            printf("\n  SBSk   -k2b0    %6d %6d %6d %6d    %6d %6d %6d %6d ", p[0x00], p[0x01], p[0x02], p[0x03], p[0x04], p[0x05], p[0x06], p[0x07]);
            printf("\n         -k2b8    %6d %6d %6d %6d    %6d %6d %6d %6d ", p[0x08], p[0x09], p[0x0A], p[0x0B], p[0x0C], p[0x0D], p[0x0E], p[0x0F]);
            printf("\n         -k2b16   %6d %6d %6d %6d    %6d %6d %6d %6d ", p[0x10], p[0x11], p[0x12], p[0x13], p[0x14], p[0x15], p[0x16], p[0x17]);
            printf("\n         -k2b24   %6d %6d %6d %6d    %6d %6d %6d %6d ", p[0x18], p[0x19], p[0x1A], p[0x1B], p[0x1C], p[0x1D], p[0x1E], p[0x1F]);

            p = ps[1].SDSk;
            printf("\n");
            printf("\n  SDSk   -k2d0    %6d %6d %6d %6d    %6d %6d %6d %6d ", p[0x00], p[0x01], p[0x02], p[0x03], p[0x04], p[0x05], p[0x06], p[0x07]);
            printf("\n         -k2d8    %6d %6d %6d %6d    %6d %6d %6d %6d ", p[0x08], p[0x09], p[0x0A], p[0x0B], p[0x0C], p[0x0D], p[0x0E], p[0x0F]);
            printf("\n         -k2d16   %6d %6d %6d %6d    %6d %6d %6d %6d ", p[0x10], p[0x11], p[0x12], p[0x13], p[0x14], p[0x15], p[0x16], p[0x17]);
            printf("\n         -k2d24   %6d %6d %6d %6d    %6d %6d %6d %6d ", p[0x18], p[0x19], p[0x1A], p[0x1B], p[0x1C], p[0x1D], p[0x1E], p[0x1F]);
        }

        if (ps[2].kMode > 1)
        {

            p = ps[2].SBSk;
            printf("\n");
            printf("\n  SBSk   -k3b0    %6d %6d %6d %6d    %6d %6d %6d %6d ", p[0x00], p[0x01], p[0x02], p[0x03], p[0x04], p[0x05], p[0x06], p[0x07]);
            printf("\n         -k3b8    %6d %6d %6d %6d    %6d %6d %6d %6d ", p[0x08], p[0x09], p[0x0A], p[0x0B], p[0x0C], p[0x0D], p[0x0E], p[0x0F]);
            printf("\n         -k3b16   %6d %6d %6d %6d    %6d %6d %6d %6d ", p[0x10], p[0x11], p[0x12], p[0x13], p[0x14], p[0x15], p[0x16], p[0x17]);
            printf("\n         -k3b24   %6d %6d %6d %6d    %6d %6d %6d %6d ", p[0x18], p[0x19], p[0x1A], p[0x1B], p[0x1C], p[0x1D], p[0x1E], p[0x1F]);

            p = ps[2].SDSk;
            printf("\n");
            printf("\n  SDSk   -k3d0    %6d %6d %6d %6d    %6d %6d %6d %6d ", p[0x00], p[0x01], p[0x02], p[0x03], p[0x04], p[0x05], p[0x06], p[0x07]);
            printf("\n         -k3d8    %6d %6d %6d %6d    %6d %6d %6d %6d ", p[0x08], p[0x09], p[0x0A], p[0x0B], p[0x0C], p[0x0D], p[0x0E], p[0x0F]);
            printf("\n         -k3d16   %6d %6d %6d %6d    %6d %6d %6d %6d ", p[0x10], p[0x11], p[0x12], p[0x13], p[0x14], p[0x15], p[0x16], p[0x17]);
            printf("\n         -k3d24   %6d %6d %6d %6d    %6d %6d %6d %6d ", p[0x18], p[0x19], p[0x1A], p[0x1B], p[0x1C], p[0x1D], p[0x1E], p[0x1F]);
        }
    }
}
#endif

HI_S32 HI_SCENE_SetDynamicThreeDNR_AutoGenerate(VI_PIPE ViPipe, VPSS_GRP VpssGrp, HI_U32 u32ISO, HI_U32 u32LastISO, HI_U8 u8Index)
{
	if(HI_TRUE != g_astScenePipeParam[u8Index].stModuleState.bDynamicThreeDNR)
	{
		return HI_SUCCESS;
	}

    HI_U32 i, j;
    HI_U32 u32IsoLevel = 0, u32NextIsoLevel = 0;
    HI_S32 s32Ret = HI_SUCCESS;
    VI_PIPE_ATTR_S stViPipeAttr = {0};
    VPSS_GRP_ATTR_S stGrpAttr = {0};
    ISP_DE_ATTR_S stDEAttr = {0};
    VI_PIPE_NRX_PARAM_S stVINRXParam = {0};
    VPSS_GRP_NRX_PARAM_S stVPSSNRXParam = {0};

    stVINRXParam .enNRVersion = VI_NR_V2;
    stVINRXParam .stNRXParamV2.enOptMode = OPERATION_MODE_MANUAL;
    stVPSSNRXParam .enNRVer = VPSS_NR_V2;
    stVPSSNRXParam .stNRXParam_V2.enOptMode = OPERATION_MODE_MANUAL;

    //printf("ViPipe:%d, u32ISO:%d, u32LastISO:%d, u8Index:%d\n", ViPipe, u32ISO, u32LastISO, u8Index);

    if (u32ISO != u32LastISO)
    {
        CHECK_SCENE_PAUSE();
        s32Ret = HI_MPI_VI_GetPipeNRXParam(ViPipe, &stVINRXParam);
        CHECK_SCENE_RET(s32Ret);
        s32Ret = HI_MPI_VPSS_GetGrpNRXParam(VpssGrp, &stVPSSNRXParam);
        CHECK_SCENE_RET(s32Ret);

        //printf(" %d %d\n", stVINRXParam .stNRXParamV2.enOptMode, stVPSSNRXParam .stNRXParam_V2.enOptMode);
        //printf("Real CTFS : %d\n", stVPSSNRXParam.stNRXParam_V2.stNRXManual.stNRXParam.NRc.CTFS);
        u32IsoLevel = SCENE_GetLevelHtoL_U32(u32ISO, g_astScenePipeParam[u8Index].stDynamicThreeDNR.u32ThreeDNRCount, g_astScenePipeParam[u8Index].stDynamicThreeDNR.au32ThreeDNRIso);

        if (g_astScenePipeParam[u8Index].stDynamicThreeDNR.u32ThreeDNRCount - 1 == u32IsoLevel)
        {
            u32NextIsoLevel = u32IsoLevel;
        }
        else
        {
            u32NextIsoLevel = u32IsoLevel + 1;
        }

        /*VI*/
        stVINRXParam.stNRXParamV2.stNRXManualV2.stNRXParamV2.SFy.SFS1 = SCENE_Interpulate(u32ISO,
                g_astScenePipeParam[u8Index].stDynamicThreeDNR.au32ThreeDNRIso[u32IsoLevel],
                g_astScenePipeParam[u8Index].stDynamicThreeDNR.astThreeDNRVIValue[u32IsoLevel].SFy.SFS1,
                g_astScenePipeParam[u8Index].stDynamicThreeDNR.au32ThreeDNRIso[u32NextIsoLevel],
                g_astScenePipeParam[u8Index].stDynamicThreeDNR.astThreeDNRVIValue[u32NextIsoLevel].SFy.SFS1);
        stVINRXParam.stNRXParamV2.stNRXManualV2.stNRXParamV2.SFy.SFS2 = SCENE_Interpulate(u32ISO,
                g_astScenePipeParam[u8Index].stDynamicThreeDNR.au32ThreeDNRIso[u32IsoLevel],
                g_astScenePipeParam[u8Index].stDynamicThreeDNR.astThreeDNRVIValue[u32IsoLevel].SFy.SFS2,
                g_astScenePipeParam[u8Index].stDynamicThreeDNR.au32ThreeDNRIso[u32NextIsoLevel],
                g_astScenePipeParam[u8Index].stDynamicThreeDNR.astThreeDNRVIValue[u32NextIsoLevel].SFy.SFS2);
        stVINRXParam.stNRXParamV2.stNRXManualV2.stNRXParamV2.SFy.SFS4 = SCENE_Interpulate(u32ISO,
                g_astScenePipeParam[u8Index].stDynamicThreeDNR.au32ThreeDNRIso[u32IsoLevel],
                g_astScenePipeParam[u8Index].stDynamicThreeDNR.astThreeDNRVIValue[u32IsoLevel].SFy.SFS4,
                g_astScenePipeParam[u8Index].stDynamicThreeDNR.au32ThreeDNRIso[u32NextIsoLevel],
                g_astScenePipeParam[u8Index].stDynamicThreeDNR.astThreeDNRVIValue[u32NextIsoLevel].SFy.SFS4);
        stVINRXParam.stNRXParamV2.stNRXManualV2.stNRXParamV2.SFy.SFT1 = SCENE_Interpulate(u32ISO,
                g_astScenePipeParam[u8Index].stDynamicThreeDNR.au32ThreeDNRIso[u32IsoLevel],
                g_astScenePipeParam[u8Index].stDynamicThreeDNR.astThreeDNRVIValue[u32IsoLevel].SFy.SFT1,
                g_astScenePipeParam[u8Index].stDynamicThreeDNR.au32ThreeDNRIso[u32NextIsoLevel],
                g_astScenePipeParam[u8Index].stDynamicThreeDNR.astThreeDNRVIValue[u32NextIsoLevel].SFy.SFT1);
        stVINRXParam.stNRXParamV2.stNRXManualV2.stNRXParamV2.SFy.SFT2 = SCENE_Interpulate(u32ISO,
                g_astScenePipeParam[u8Index].stDynamicThreeDNR.au32ThreeDNRIso[u32IsoLevel],
                g_astScenePipeParam[u8Index].stDynamicThreeDNR.astThreeDNRVIValue[u32IsoLevel].SFy.SFT2,
                g_astScenePipeParam[u8Index].stDynamicThreeDNR.au32ThreeDNRIso[u32NextIsoLevel],
                g_astScenePipeParam[u8Index].stDynamicThreeDNR.astThreeDNRVIValue[u32NextIsoLevel].SFy.SFT2);
        stVINRXParam.stNRXParamV2.stNRXManualV2.stNRXParamV2.SFy.SFT4 = SCENE_Interpulate(u32ISO,
                g_astScenePipeParam[u8Index].stDynamicThreeDNR.au32ThreeDNRIso[u32IsoLevel],
                g_astScenePipeParam[u8Index].stDynamicThreeDNR.astThreeDNRVIValue[u32IsoLevel].SFy.SFT4,
                g_astScenePipeParam[u8Index].stDynamicThreeDNR.au32ThreeDNRIso[u32NextIsoLevel],
                g_astScenePipeParam[u8Index].stDynamicThreeDNR.astThreeDNRVIValue[u32NextIsoLevel].SFy.SFT4);
        stVINRXParam.stNRXParamV2.stNRXManualV2.stNRXParamV2.SFy.SBR1 = SCENE_Interpulate(u32ISO,
                g_astScenePipeParam[u8Index].stDynamicThreeDNR.au32ThreeDNRIso[u32IsoLevel],
                g_astScenePipeParam[u8Index].stDynamicThreeDNR.astThreeDNRVIValue[u32IsoLevel].SFy.SBR1,
                g_astScenePipeParam[u8Index].stDynamicThreeDNR.au32ThreeDNRIso[u32NextIsoLevel],
                g_astScenePipeParam[u8Index].stDynamicThreeDNR.astThreeDNRVIValue[u32NextIsoLevel].SFy.SBR1);
        stVINRXParam.stNRXParamV2.stNRXManualV2.stNRXParamV2.SFy.SBR2 = SCENE_Interpulate(u32ISO,
                g_astScenePipeParam[u8Index].stDynamicThreeDNR.au32ThreeDNRIso[u32IsoLevel],
                g_astScenePipeParam[u8Index].stDynamicThreeDNR.astThreeDNRVIValue[u32IsoLevel].SFy.SBR2,
                g_astScenePipeParam[u8Index].stDynamicThreeDNR.au32ThreeDNRIso[u32NextIsoLevel],
                g_astScenePipeParam[u8Index].stDynamicThreeDNR.astThreeDNRVIValue[u32NextIsoLevel].SFy.SBR2);
        stVINRXParam.stNRXParamV2.stNRXManualV2.stNRXParamV2.SFy.SBR4 = SCENE_Interpulate(u32ISO,
                g_astScenePipeParam[u8Index].stDynamicThreeDNR.au32ThreeDNRIso[u32IsoLevel],
                g_astScenePipeParam[u8Index].stDynamicThreeDNR.astThreeDNRVIValue[u32IsoLevel].SFy.SBR4,
                g_astScenePipeParam[u8Index].stDynamicThreeDNR.au32ThreeDNRIso[u32NextIsoLevel],
                g_astScenePipeParam[u8Index].stDynamicThreeDNR.astThreeDNRVIValue[u32IsoLevel].SFy.SBR4);

        stVINRXParam.stNRXParamV2.stNRXManualV2.stNRXParamV2.SFy.SRT0 = SCENE_Interpulate(u32ISO,
                g_astScenePipeParam[u8Index].stDynamicThreeDNR.au32ThreeDNRIso[u32IsoLevel],
                g_astScenePipeParam[u8Index].stDynamicThreeDNR.astThreeDNRVIValue[u32IsoLevel].SFy.SRT0,
                g_astScenePipeParam[u8Index].stDynamicThreeDNR.au32ThreeDNRIso[u32NextIsoLevel],
                g_astScenePipeParam[u8Index].stDynamicThreeDNR.astThreeDNRVIValue[u32NextIsoLevel].SFy.SRT0);
        stVINRXParam.stNRXParamV2.stNRXManualV2.stNRXParamV2.SFy.SRT1 = SCENE_Interpulate(u32ISO,
                g_astScenePipeParam[u8Index].stDynamicThreeDNR.au32ThreeDNRIso[u32IsoLevel],
                g_astScenePipeParam[u8Index].stDynamicThreeDNR.astThreeDNRVIValue[u32IsoLevel].SFy.SRT1,
                g_astScenePipeParam[u8Index].stDynamicThreeDNR.au32ThreeDNRIso[u32NextIsoLevel],
                g_astScenePipeParam[u8Index].stDynamicThreeDNR.astThreeDNRVIValue[u32NextIsoLevel].SFy.SRT1);
        stVINRXParam.stNRXParamV2.stNRXManualV2.stNRXParamV2.SFy.DeRate = SCENE_Interpulate(u32ISO,
                g_astScenePipeParam[u8Index].stDynamicThreeDNR.au32ThreeDNRIso[u32IsoLevel],
                g_astScenePipeParam[u8Index].stDynamicThreeDNR.astThreeDNRVIValue[u32IsoLevel].SFy.DeRate,
                g_astScenePipeParam[u8Index].stDynamicThreeDNR.au32ThreeDNRIso[u32NextIsoLevel],
                g_astScenePipeParam[u8Index].stDynamicThreeDNR.astThreeDNRVIValue[u32NextIsoLevel].SFy.DeRate);
        stVINRXParam.stNRXParamV2.stNRXManualV2.stNRXParamV2.SFy.SFR = SCENE_Interpulate(u32ISO,
                g_astScenePipeParam[u8Index].stDynamicThreeDNR.au32ThreeDNRIso[u32IsoLevel],
                g_astScenePipeParam[u8Index].stDynamicThreeDNR.astThreeDNRVIValue[u32IsoLevel].SFy.SFR,
                g_astScenePipeParam[u8Index].stDynamicThreeDNR.au32ThreeDNRIso[u32NextIsoLevel],
                g_astScenePipeParam[u8Index].stDynamicThreeDNR.astThreeDNRVIValue[u32NextIsoLevel].SFy.SFR);

        stVINRXParam.stNRXParamV2.stNRXManualV2.stNRXParamV2.IEy.IES0 = SCENE_Interpulate(u32ISO,
                g_astScenePipeParam[u8Index].stDynamicThreeDNR.au32ThreeDNRIso[u32IsoLevel],
                g_astScenePipeParam[u8Index].stDynamicThreeDNR.astThreeDNRVIValue[u32IsoLevel].IEy.IES0,
                g_astScenePipeParam[u8Index].stDynamicThreeDNR.au32ThreeDNRIso[u32NextIsoLevel],
                g_astScenePipeParam[u8Index].stDynamicThreeDNR.astThreeDNRVIValue[u32NextIsoLevel].IEy.IES0);
        stVINRXParam.stNRXParamV2.stNRXManualV2.stNRXParamV2.IEy.IES1 = SCENE_Interpulate(u32ISO,
                g_astScenePipeParam[u8Index].stDynamicThreeDNR.au32ThreeDNRIso[u32IsoLevel],
                g_astScenePipeParam[u8Index].stDynamicThreeDNR.astThreeDNRVIValue[u32IsoLevel].IEy.IES1,
                g_astScenePipeParam[u8Index].stDynamicThreeDNR.au32ThreeDNRIso[u32NextIsoLevel],
                g_astScenePipeParam[u8Index].stDynamicThreeDNR.astThreeDNRVIValue[u32NextIsoLevel].IEy.IES1);
        stVINRXParam.stNRXParamV2.stNRXManualV2.stNRXParamV2.IEy.IES2 = SCENE_Interpulate(u32ISO,
                g_astScenePipeParam[u8Index].stDynamicThreeDNR.au32ThreeDNRIso[u32IsoLevel],
                g_astScenePipeParam[u8Index].stDynamicThreeDNR.astThreeDNRVIValue[u32IsoLevel].IEy.IES2,
                g_astScenePipeParam[u8Index].stDynamicThreeDNR.au32ThreeDNRIso[u32NextIsoLevel],
                g_astScenePipeParam[u8Index].stDynamicThreeDNR.astThreeDNRVIValue[u32NextIsoLevel].IEy.IES2);
        stVINRXParam.stNRXParamV2.stNRXManualV2.stNRXParamV2.IEy.IES3 = SCENE_Interpulate(u32ISO,
                g_astScenePipeParam[u8Index].stDynamicThreeDNR.au32ThreeDNRIso[u32IsoLevel],
                g_astScenePipeParam[u8Index].stDynamicThreeDNR.astThreeDNRVIValue[u32IsoLevel].IEy.IES3,
                g_astScenePipeParam[u8Index].stDynamicThreeDNR.au32ThreeDNRIso[u32NextIsoLevel],
                g_astScenePipeParam[u8Index].stDynamicThreeDNR.astThreeDNRVIValue[u32NextIsoLevel].IEy.IES3);
        stVINRXParam.stNRXParamV2.stNRXManualV2.stNRXParamV2.IEy.IEDZ = SCENE_Interpulate(u32ISO,
                g_astScenePipeParam[u8Index].stDynamicThreeDNR.au32ThreeDNRIso[u32IsoLevel],
                g_astScenePipeParam[u8Index].stDynamicThreeDNR.astThreeDNRVIValue[u32IsoLevel].IEy.IEDZ,
                g_astScenePipeParam[u8Index].stDynamicThreeDNR.au32ThreeDNRIso[u32NextIsoLevel],
                g_astScenePipeParam[u8Index].stDynamicThreeDNR.astThreeDNRVIValue[u32NextIsoLevel].IEy.IEDZ);

        for (j = 0; j < 3; j++)
        {
            stVINRXParam.stNRXParamV2.stNRXManualV2.stNRXParamV2.SFy.SFR6[j]  = g_astScenePipeParam[u8Index].stDynamicThreeDNR.astThreeDNRVIValue[u32IsoLevel].SFy.SFR6[j];
        }

        stVINRXParam.stNRXParamV2.stNRXManualV2.stNRXParamV2.SFy.BWSF4 = g_astScenePipeParam[u8Index].stDynamicThreeDNR.astThreeDNRVIValue[u32IsoLevel].SFy.BWSF4;
        stVINRXParam.stNRXParamV2.stNRXManualV2.stNRXParamV2.SFy.SPN6  = g_astScenePipeParam[u8Index].stDynamicThreeDNR.astThreeDNRVIValue[u32IsoLevel].SFy.SPN6;
        stVINRXParam.stNRXParamV2.stNRXManualV2.stNRXParamV2.SFy.SBN6  = g_astScenePipeParam[u8Index].stDynamicThreeDNR.astThreeDNRVIValue[u32IsoLevel].SFy.SBN6;
        stVINRXParam.stNRXParamV2.stNRXManualV2.stNRXParamV2.SFy.PBR6  = g_astScenePipeParam[u8Index].stDynamicThreeDNR.astThreeDNRVIValue[u32IsoLevel].SFy.PBR6;
        stVINRXParam.stNRXParamV2.stNRXManualV2.stNRXParamV2.SFy.JMODE = g_astScenePipeParam[u8Index].stDynamicThreeDNR.astThreeDNRVIValue[u32IsoLevel].SFy.JMODE;
        stVINRXParam.stNRXParamV2.stNRXManualV2.stNRXParamV2.SFy.DeIdx = g_astScenePipeParam[u8Index].stDynamicThreeDNR.astThreeDNRVIValue[u32IsoLevel].SFy.DeIdx;
        stVINRXParam.stNRXParamV2.stNRXManualV2.stNRXParamV2.SFy.TriTh = g_astScenePipeParam[u8Index].stDynamicThreeDNR.astThreeDNRVIValue[u32IsoLevel].SFy.TriTh;

        if (stVINRXParam.stNRXParamV2.stNRXManualV2.stNRXParamV2.SFy.TriTh)
        {
            stVINRXParam.stNRXParamV2.stNRXManualV2.stNRXParamV2.SFy.SFN0 = g_astScenePipeParam[u8Index].stDynamicThreeDNR.astThreeDNRVIValue[u32IsoLevel].SFy.SFN0;
            stVINRXParam.stNRXParamV2.stNRXManualV2.stNRXParamV2.SFy.SFN1 = g_astScenePipeParam[u8Index].stDynamicThreeDNR.astThreeDNRVIValue[u32IsoLevel].SFy.SFN1;
            stVINRXParam.stNRXParamV2.stNRXManualV2.stNRXParamV2.SFy.SFN2 = g_astScenePipeParam[u8Index].stDynamicThreeDNR.astThreeDNRVIValue[u32IsoLevel].SFy.SFN2;
            stVINRXParam.stNRXParamV2.stNRXManualV2.stNRXParamV2.SFy.SFN3 = g_astScenePipeParam[u8Index].stDynamicThreeDNR.astThreeDNRVIValue[u32IsoLevel].SFy.SFN3;
            stVINRXParam.stNRXParamV2.stNRXManualV2.stNRXParamV2.SFy.STH1 = SCENE_Interpulate(u32ISO,
                    g_astScenePipeParam[u8Index].stDynamicThreeDNR.au32ThreeDNRIso[u32IsoLevel],
                    g_astScenePipeParam[u8Index].stDynamicThreeDNR.astThreeDNRVIValue[u32IsoLevel].SFy.STH1,
                    g_astScenePipeParam[u8Index].stDynamicThreeDNR.au32ThreeDNRIso[u32NextIsoLevel],
                    g_astScenePipeParam[u8Index].stDynamicThreeDNR.astThreeDNRVIValue[u32NextIsoLevel].SFy.STH1);
            stVINRXParam.stNRXParamV2.stNRXManualV2.stNRXParamV2.SFy.STH2 = SCENE_Interpulate(u32ISO,
                    g_astScenePipeParam[u8Index].stDynamicThreeDNR.au32ThreeDNRIso[u32IsoLevel],
                    g_astScenePipeParam[u8Index].stDynamicThreeDNR.astThreeDNRVIValue[u32IsoLevel].SFy.STH2,
                    g_astScenePipeParam[u8Index].stDynamicThreeDNR.au32ThreeDNRIso[u32NextIsoLevel],
                    g_astScenePipeParam[u8Index].stDynamicThreeDNR.astThreeDNRVIValue[u32NextIsoLevel].SFy.STH2);
            stVINRXParam.stNRXParamV2.stNRXManualV2.stNRXParamV2.SFy.STH3 = SCENE_Interpulate(u32ISO,
                    g_astScenePipeParam[u8Index].stDynamicThreeDNR.au32ThreeDNRIso[u32IsoLevel],
                    g_astScenePipeParam[u8Index].stDynamicThreeDNR.astThreeDNRVIValue[u32IsoLevel].SFy.STH3,
                    g_astScenePipeParam[u8Index].stDynamicThreeDNR.au32ThreeDNRIso[u32NextIsoLevel],
                    g_astScenePipeParam[u8Index].stDynamicThreeDNR.astThreeDNRVIValue[u32NextIsoLevel].SFy.STH3);
        }
        else
        {
            stVINRXParam.stNRXParamV2.stNRXManualV2.stNRXParamV2.SFy.SFN0 = g_astScenePipeParam[u8Index].stDynamicThreeDNR.astThreeDNRVIValue[u32IsoLevel].SFy.SFN0;
            stVINRXParam.stNRXParamV2.stNRXManualV2.stNRXParamV2.SFy.SFN1 = g_astScenePipeParam[u8Index].stDynamicThreeDNR.astThreeDNRVIValue[u32IsoLevel].SFy.SFN1;
            stVINRXParam.stNRXParamV2.stNRXManualV2.stNRXParamV2.SFy.SFN3 = g_astScenePipeParam[u8Index].stDynamicThreeDNR.astThreeDNRVIValue[u32IsoLevel].SFy.SFN3;
            stVINRXParam.stNRXParamV2.stNRXManualV2.stNRXParamV2.SFy.STH1 = SCENE_Interpulate(u32ISO,
                    g_astScenePipeParam[u8Index].stDynamicThreeDNR.au32ThreeDNRIso[u32IsoLevel],
                    g_astScenePipeParam[u8Index].stDynamicThreeDNR.astThreeDNRVIValue[u32IsoLevel].SFy.STH1,
                    g_astScenePipeParam[u8Index].stDynamicThreeDNR.au32ThreeDNRIso[u32NextIsoLevel],
                    g_astScenePipeParam[u8Index].stDynamicThreeDNR.astThreeDNRVIValue[u32NextIsoLevel].SFy.STH1);
            stVINRXParam.stNRXParamV2.stNRXManualV2.stNRXParamV2.SFy.STH3 = SCENE_Interpulate(u32ISO,
                    g_astScenePipeParam[u8Index].stDynamicThreeDNR.au32ThreeDNRIso[u32IsoLevel],
                    g_astScenePipeParam[u8Index].stDynamicThreeDNR.astThreeDNRVIValue[u32IsoLevel].SFy.STH3,
                    g_astScenePipeParam[u8Index].stDynamicThreeDNR.au32ThreeDNRIso[u32NextIsoLevel],
                    g_astScenePipeParam[u8Index].stDynamicThreeDNR.astThreeDNRVIValue[u32NextIsoLevel].SFy.STH3);
        }

        /*VPSS*/
        for (i = 0; i < 3; i++)
        {
            stVPSSNRXParam.stNRXParam_V2.stNRXManual.stNRXParam.SFy[i].SFS1 = SCENE_Interpulate(u32ISO,
                    g_astScenePipeParam[u8Index].stDynamicThreeDNR.au32ThreeDNRIso[u32IsoLevel],
                    g_astScenePipeParam[u8Index].stDynamicThreeDNR.astThreeDNRVPSSValue[u32IsoLevel].SFy[i].SFS1,
                    g_astScenePipeParam[u8Index].stDynamicThreeDNR.au32ThreeDNRIso[u32NextIsoLevel],
                    g_astScenePipeParam[u8Index].stDynamicThreeDNR.astThreeDNRVPSSValue[u32NextIsoLevel].SFy[i].SFS1);
            stVPSSNRXParam.stNRXParam_V2.stNRXManual.stNRXParam.SFy[i].SFS2 = SCENE_Interpulate(u32ISO,
                    g_astScenePipeParam[u8Index].stDynamicThreeDNR.au32ThreeDNRIso[u32IsoLevel],
                    g_astScenePipeParam[u8Index].stDynamicThreeDNR.astThreeDNRVPSSValue[u32IsoLevel].SFy[i].SFS2,
                    g_astScenePipeParam[u8Index].stDynamicThreeDNR.au32ThreeDNRIso[u32NextIsoLevel],
                    g_astScenePipeParam[u8Index].stDynamicThreeDNR.astThreeDNRVPSSValue[u32NextIsoLevel].SFy[i].SFS2);
            stVPSSNRXParam.stNRXParam_V2.stNRXManual.stNRXParam.SFy[i].SFS4 = SCENE_Interpulate(u32ISO,
                    g_astScenePipeParam[u8Index].stDynamicThreeDNR.au32ThreeDNRIso[u32IsoLevel],
                    g_astScenePipeParam[u8Index].stDynamicThreeDNR.astThreeDNRVPSSValue[u32IsoLevel].SFy[i].SFS4,
                    g_astScenePipeParam[u8Index].stDynamicThreeDNR.au32ThreeDNRIso[u32NextIsoLevel],
                    g_astScenePipeParam[u8Index].stDynamicThreeDNR.astThreeDNRVPSSValue[u32NextIsoLevel].SFy[i].SFS4);
            stVPSSNRXParam.stNRXParam_V2.stNRXManual.stNRXParam.SFy[i].SFT1 = SCENE_Interpulate(u32ISO,
                    g_astScenePipeParam[u8Index].stDynamicThreeDNR.au32ThreeDNRIso[u32IsoLevel],
                    g_astScenePipeParam[u8Index].stDynamicThreeDNR.astThreeDNRVPSSValue[u32IsoLevel].SFy[i].SFT1,
                    g_astScenePipeParam[u8Index].stDynamicThreeDNR.au32ThreeDNRIso[u32NextIsoLevel],
                    g_astScenePipeParam[u8Index].stDynamicThreeDNR.astThreeDNRVPSSValue[u32NextIsoLevel].SFy[i].SFT1);
            stVPSSNRXParam.stNRXParam_V2.stNRXManual.stNRXParam.SFy[i].SFT2 = SCENE_Interpulate(u32ISO,
                    g_astScenePipeParam[u8Index].stDynamicThreeDNR.au32ThreeDNRIso[u32IsoLevel],
                    g_astScenePipeParam[u8Index].stDynamicThreeDNR.astThreeDNRVPSSValue[u32IsoLevel].SFy[i].SFT2,
                    g_astScenePipeParam[u8Index].stDynamicThreeDNR.au32ThreeDNRIso[u32NextIsoLevel],
                    g_astScenePipeParam[u8Index].stDynamicThreeDNR.astThreeDNRVPSSValue[u32NextIsoLevel].SFy[i].SFT2);
            stVPSSNRXParam.stNRXParam_V2.stNRXManual.stNRXParam.SFy[i].SFT4 = SCENE_Interpulate(u32ISO,
                    g_astScenePipeParam[u8Index].stDynamicThreeDNR.au32ThreeDNRIso[u32IsoLevel],
                    g_astScenePipeParam[u8Index].stDynamicThreeDNR.astThreeDNRVPSSValue[u32IsoLevel].SFy[i].SFT4,
                    g_astScenePipeParam[u8Index].stDynamicThreeDNR.au32ThreeDNRIso[u32NextIsoLevel],
                    g_astScenePipeParam[u8Index].stDynamicThreeDNR.astThreeDNRVPSSValue[u32NextIsoLevel].SFy[i].SFT4);
            stVPSSNRXParam.stNRXParam_V2.stNRXManual.stNRXParam.SFy[i].SBR1 = SCENE_Interpulate(u32ISO,
                    g_astScenePipeParam[u8Index].stDynamicThreeDNR.au32ThreeDNRIso[u32IsoLevel],
                    g_astScenePipeParam[u8Index].stDynamicThreeDNR.astThreeDNRVPSSValue[u32IsoLevel].SFy[i].SBR1,
                    g_astScenePipeParam[u8Index].stDynamicThreeDNR.au32ThreeDNRIso[u32NextIsoLevel],
                    g_astScenePipeParam[u8Index].stDynamicThreeDNR.astThreeDNRVPSSValue[u32NextIsoLevel].SFy[i].SBR1);
            stVPSSNRXParam.stNRXParam_V2.stNRXManual.stNRXParam.SFy[i].SBR2 = SCENE_Interpulate(u32ISO,
                    g_astScenePipeParam[u8Index].stDynamicThreeDNR.au32ThreeDNRIso[u32IsoLevel],
                    g_astScenePipeParam[u8Index].stDynamicThreeDNR.astThreeDNRVPSSValue[u32IsoLevel].SFy[i].SBR2,
                    g_astScenePipeParam[u8Index].stDynamicThreeDNR.au32ThreeDNRIso[u32NextIsoLevel],
                    g_astScenePipeParam[u8Index].stDynamicThreeDNR.astThreeDNRVPSSValue[u32NextIsoLevel].SFy[i].SBR2);
            stVPSSNRXParam.stNRXParam_V2.stNRXManual.stNRXParam.SFy[i].SBR4 = SCENE_Interpulate(u32ISO,
                    g_astScenePipeParam[u8Index].stDynamicThreeDNR.au32ThreeDNRIso[u32IsoLevel],
                    g_astScenePipeParam[u8Index].stDynamicThreeDNR.astThreeDNRVPSSValue[u32IsoLevel].SFy[i].SBR4,
                    g_astScenePipeParam[u8Index].stDynamicThreeDNR.au32ThreeDNRIso[u32NextIsoLevel],
                    g_astScenePipeParam[u8Index].stDynamicThreeDNR.astThreeDNRVPSSValue[u32NextIsoLevel].SFy[i].SBR4);
            stVPSSNRXParam.stNRXParam_V2.stNRXManual.stNRXParam.SFy[i].SFR = SCENE_Interpulate(u32ISO,
                    g_astScenePipeParam[u8Index].stDynamicThreeDNR.au32ThreeDNRIso[u32IsoLevel],
                    g_astScenePipeParam[u8Index].stDynamicThreeDNR.astThreeDNRVPSSValue[u32IsoLevel].SFy[i].SFR,
                    g_astScenePipeParam[u8Index].stDynamicThreeDNR.au32ThreeDNRIso[u32NextIsoLevel],
                    g_astScenePipeParam[u8Index].stDynamicThreeDNR.astThreeDNRVPSSValue[u32NextIsoLevel].SFy[i].SFR);

            stVPSSNRXParam.stNRXParam_V2.stNRXManual.stNRXParam.IEy[i].IES0 = SCENE_Interpulate(u32ISO,
                    g_astScenePipeParam[u8Index].stDynamicThreeDNR.au32ThreeDNRIso[u32IsoLevel],
                    g_astScenePipeParam[u8Index].stDynamicThreeDNR.astThreeDNRVPSSValue[u32IsoLevel].IEy[i].IES0,
                    g_astScenePipeParam[u8Index].stDynamicThreeDNR.au32ThreeDNRIso[u32NextIsoLevel],
                    g_astScenePipeParam[u8Index].stDynamicThreeDNR.astThreeDNRVPSSValue[u32NextIsoLevel].IEy[i].IES0);
            stVPSSNRXParam.stNRXParam_V2.stNRXManual.stNRXParam.IEy[i].IES1 = SCENE_Interpulate(u32ISO,
                    g_astScenePipeParam[u8Index].stDynamicThreeDNR.au32ThreeDNRIso[u32IsoLevel],
                    g_astScenePipeParam[u8Index].stDynamicThreeDNR.astThreeDNRVPSSValue[u32IsoLevel].IEy[i].IES1,
                    g_astScenePipeParam[u8Index].stDynamicThreeDNR.au32ThreeDNRIso[u32NextIsoLevel],
                    g_astScenePipeParam[u8Index].stDynamicThreeDNR.astThreeDNRVPSSValue[u32NextIsoLevel].IEy[i].IES1);
            stVPSSNRXParam.stNRXParam_V2.stNRXManual.stNRXParam.IEy[i].IES2 = SCENE_Interpulate(u32ISO,
                    g_astScenePipeParam[u8Index].stDynamicThreeDNR.au32ThreeDNRIso[u32IsoLevel],
                    g_astScenePipeParam[u8Index].stDynamicThreeDNR.astThreeDNRVPSSValue[u32IsoLevel].IEy[i].IES2,
                    g_astScenePipeParam[u8Index].stDynamicThreeDNR.au32ThreeDNRIso[u32NextIsoLevel],
                    g_astScenePipeParam[u8Index].stDynamicThreeDNR.astThreeDNRVPSSValue[u32NextIsoLevel].IEy[i].IES2);
            stVPSSNRXParam.stNRXParam_V2.stNRXManual.stNRXParam.IEy[i].IES3 = SCENE_Interpulate(u32ISO,
                    g_astScenePipeParam[u8Index].stDynamicThreeDNR.au32ThreeDNRIso[u32IsoLevel],
                    g_astScenePipeParam[u8Index].stDynamicThreeDNR.astThreeDNRVPSSValue[u32IsoLevel].IEy[i].IES3,
                    g_astScenePipeParam[u8Index].stDynamicThreeDNR.au32ThreeDNRIso[u32NextIsoLevel],
                    g_astScenePipeParam[u8Index].stDynamicThreeDNR.astThreeDNRVPSSValue[u32NextIsoLevel].IEy[i].IES3);
            stVPSSNRXParam.stNRXParam_V2.stNRXManual.stNRXParam.IEy[i].IEDZ = SCENE_Interpulate(u32ISO,
                    g_astScenePipeParam[u8Index].stDynamicThreeDNR.au32ThreeDNRIso[u32IsoLevel],
                    g_astScenePipeParam[u8Index].stDynamicThreeDNR.astThreeDNRVPSSValue[u32IsoLevel].IEy[i].IEDZ,
                    g_astScenePipeParam[u8Index].stDynamicThreeDNR.au32ThreeDNRIso[u32NextIsoLevel],
                    g_astScenePipeParam[u8Index].stDynamicThreeDNR.astThreeDNRVPSSValue[u32NextIsoLevel].IEy[i].IEDZ);

            for (j = 0; j < 3; j++)
            {
                stVPSSNRXParam.stNRXParam_V2.stNRXManual.stNRXParam.SFy[i].SFR6[j]  = g_astScenePipeParam[u8Index].stDynamicThreeDNR.astThreeDNRVPSSValue[u32IsoLevel].SFy[i].SFR6[j];
            }

            stVPSSNRXParam.stNRXParam_V2.stNRXManual.stNRXParam.SFy[i].NRyEn = g_astScenePipeParam[u8Index].stDynamicThreeDNR.astThreeDNRVPSSValue[u32IsoLevel].SFy[i].NRyEn;
            stVPSSNRXParam.stNRXParam_V2.stNRXManual.stNRXParam.SFy[i].BWSF4 = g_astScenePipeParam[u8Index].stDynamicThreeDNR.astThreeDNRVPSSValue[u32IsoLevel].SFy[i].BWSF4;
            stVPSSNRXParam.stNRXParam_V2.stNRXManual.stNRXParam.SFy[i].SPN6  = g_astScenePipeParam[u8Index].stDynamicThreeDNR.astThreeDNRVPSSValue[u32IsoLevel].SFy[i].SPN6;
            stVPSSNRXParam.stNRXParam_V2.stNRXManual.stNRXParam.SFy[i].SBN6  = g_astScenePipeParam[u8Index].stDynamicThreeDNR.astThreeDNRVPSSValue[u32IsoLevel].SFy[i].SBN6;
            stVPSSNRXParam.stNRXParam_V2.stNRXManual.stNRXParam.SFy[i].PBR6  = g_astScenePipeParam[u8Index].stDynamicThreeDNR.astThreeDNRVPSSValue[u32IsoLevel].SFy[i].PBR6;
            stVPSSNRXParam.stNRXParam_V2.stNRXManual.stNRXParam.SFy[i].JMODE = g_astScenePipeParam[u8Index].stDynamicThreeDNR.astThreeDNRVPSSValue[u32IsoLevel].SFy[i].JMODE;
            stVPSSNRXParam.stNRXParam_V2.stNRXManual.stNRXParam.SFy[i].TriTh = g_astScenePipeParam[u8Index].stDynamicThreeDNR.astThreeDNRVPSSValue[u32IsoLevel].SFy[i].TriTh;

            if (stVINRXParam.stNRXParamV2.stNRXManualV2.stNRXParamV2.SFy.TriTh)
            {
                stVPSSNRXParam.stNRXParam_V2.stNRXManual.stNRXParam.SFy[i].SFN0 = g_astScenePipeParam[u8Index].stDynamicThreeDNR.astThreeDNRVPSSValue[u32IsoLevel].SFy[i].SFN0;
                stVPSSNRXParam.stNRXParam_V2.stNRXManual.stNRXParam.SFy[i].SFN1 = g_astScenePipeParam[u8Index].stDynamicThreeDNR.astThreeDNRVPSSValue[u32IsoLevel].SFy[i].SFN1;
                stVPSSNRXParam.stNRXParam_V2.stNRXManual.stNRXParam.SFy[i].SFN2 = g_astScenePipeParam[u8Index].stDynamicThreeDNR.astThreeDNRVPSSValue[u32IsoLevel].SFy[i].SFN2;
                stVPSSNRXParam.stNRXParam_V2.stNRXManual.stNRXParam.SFy[i].SFN3 = g_astScenePipeParam[u8Index].stDynamicThreeDNR.astThreeDNRVPSSValue[u32IsoLevel].SFy[i].SFN3;
                stVPSSNRXParam.stNRXParam_V2.stNRXManual.stNRXParam.SFy[i].STH1 = SCENE_Interpulate(u32ISO,
                        g_astScenePipeParam[u8Index].stDynamicThreeDNR.au32ThreeDNRIso[u32IsoLevel],
                        g_astScenePipeParam[u8Index].stDynamicThreeDNR.astThreeDNRVPSSValue[u32IsoLevel].SFy[i].STH1,
                        g_astScenePipeParam[u8Index].stDynamicThreeDNR.au32ThreeDNRIso[u32NextIsoLevel],
                        g_astScenePipeParam[u8Index].stDynamicThreeDNR.astThreeDNRVPSSValue[u32NextIsoLevel].SFy[i].STH1);
                stVPSSNRXParam.stNRXParam_V2.stNRXManual.stNRXParam.SFy[i].STH2 = SCENE_Interpulate(u32ISO,
                        g_astScenePipeParam[u8Index].stDynamicThreeDNR.au32ThreeDNRIso[u32IsoLevel],
                        g_astScenePipeParam[u8Index].stDynamicThreeDNR.astThreeDNRVPSSValue[u32IsoLevel].SFy[i].STH2,
                        g_astScenePipeParam[u8Index].stDynamicThreeDNR.au32ThreeDNRIso[u32NextIsoLevel],
                        g_astScenePipeParam[u8Index].stDynamicThreeDNR.astThreeDNRVPSSValue[u32NextIsoLevel].SFy[i].STH2);
                stVPSSNRXParam.stNRXParam_V2.stNRXManual.stNRXParam.SFy[i].STH3 = SCENE_Interpulate(u32ISO,
                        g_astScenePipeParam[u8Index].stDynamicThreeDNR.au32ThreeDNRIso[u32IsoLevel],
                        g_astScenePipeParam[u8Index].stDynamicThreeDNR.astThreeDNRVPSSValue[u32IsoLevel].SFy[i].STH3,
                        g_astScenePipeParam[u8Index].stDynamicThreeDNR.au32ThreeDNRIso[u32NextIsoLevel],
                        g_astScenePipeParam[u8Index].stDynamicThreeDNR.astThreeDNRVPSSValue[u32NextIsoLevel].SFy[i].STH3);
            }
            else
            {
                stVPSSNRXParam.stNRXParam_V2.stNRXManual.stNRXParam.SFy[i].SFN0 = g_astScenePipeParam[u8Index].stDynamicThreeDNR.astThreeDNRVPSSValue[u32IsoLevel].SFy[i].SFN0;
                stVPSSNRXParam.stNRXParam_V2.stNRXManual.stNRXParam.SFy[i].SFN1 = g_astScenePipeParam[u8Index].stDynamicThreeDNR.astThreeDNRVPSSValue[u32IsoLevel].SFy[i].SFN1;
                stVPSSNRXParam.stNRXParam_V2.stNRXManual.stNRXParam.SFy[i].SFN3 = g_astScenePipeParam[u8Index].stDynamicThreeDNR.astThreeDNRVPSSValue[u32IsoLevel].SFy[i].SFN3;
                stVPSSNRXParam.stNRXParam_V2.stNRXManual.stNRXParam.SFy[i].STH1 = SCENE_Interpulate(u32ISO,
                        g_astScenePipeParam[u8Index].stDynamicThreeDNR.au32ThreeDNRIso[u32IsoLevel],
                        g_astScenePipeParam[u8Index].stDynamicThreeDNR.astThreeDNRVPSSValue[u32IsoLevel].SFy[i].STH1,
                        g_astScenePipeParam[u8Index].stDynamicThreeDNR.au32ThreeDNRIso[u32NextIsoLevel],
                        g_astScenePipeParam[u8Index].stDynamicThreeDNR.astThreeDNRVPSSValue[u32NextIsoLevel].SFy[i].STH1);
                stVPSSNRXParam.stNRXParam_V2.stNRXManual.stNRXParam.SFy[i].STH3 = SCENE_Interpulate(u32ISO,
                        g_astScenePipeParam[u8Index].stDynamicThreeDNR.au32ThreeDNRIso[u32IsoLevel],
                        g_astScenePipeParam[u8Index].stDynamicThreeDNR.astThreeDNRVPSSValue[u32IsoLevel].SFy[i].STH3,
                        g_astScenePipeParam[u8Index].stDynamicThreeDNR.au32ThreeDNRIso[u32NextIsoLevel],
                        g_astScenePipeParam[u8Index].stDynamicThreeDNR.astThreeDNRVPSSValue[u32NextIsoLevel].SFy[i].STH3);
            }
        }

        stVPSSNRXParam.stNRXParam_V2.stNRXManual.stNRXParam.SFy[1].kMode = g_astScenePipeParam[u8Index].stDynamicThreeDNR.astThreeDNRVPSSValue[u32IsoLevel].SFy[1].kMode;
        stVPSSNRXParam.stNRXParam_V2.stNRXManual.stNRXParam.SFy[2].kMode = g_astScenePipeParam[u8Index].stDynamicThreeDNR.astThreeDNRVPSSValue[u32IsoLevel].SFy[2].kMode;

        if (stVPSSNRXParam.stNRXParam_V2.stNRXManual.stNRXParam.SFy[1].kMode > 1)
        {
            for (j = 0; j < 32; j++)
            {
                stVPSSNRXParam.stNRXParam_V2.stNRXManual.stNRXParam.SFy[1].SBSk[j] = g_astScenePipeParam[u8Index].stDynamicThreeDNR.astThreeDNRVPSSValue[u32IsoLevel].SFy[1].SBSk[j];
                stVPSSNRXParam.stNRXParam_V2.stNRXManual.stNRXParam.SFy[1].SDSk[j] = g_astScenePipeParam[u8Index].stDynamicThreeDNR.astThreeDNRVPSSValue[u32IsoLevel].SFy[1].SDSk[j];
            }
        }

        if (stVPSSNRXParam.stNRXParam_V2.stNRXManual.stNRXParam.SFy[2].kMode > 1)
        {
            for (j = 0; j < 32; j++)
            {
                stVPSSNRXParam.stNRXParam_V2.stNRXManual.stNRXParam.SFy[2].SBSk[j] = g_astScenePipeParam[u8Index].stDynamicThreeDNR.astThreeDNRVPSSValue[u32IsoLevel].SFy[2].SBSk[j];
                stVPSSNRXParam.stNRXParam_V2.stNRXManual.stNRXParam.SFy[2].SDSk[j] = g_astScenePipeParam[u8Index].stDynamicThreeDNR.astThreeDNRVPSSValue[u32IsoLevel].SFy[2].SDSk[j];
            }
        }

        stVPSSNRXParam.stNRXParam_V2.stNRXManual.stNRXParam.TFy[0].tEdge = g_astScenePipeParam[u8Index].stDynamicThreeDNR.astThreeDNRVPSSValue[u32IsoLevel].TFy[0].tEdge;
        stVPSSNRXParam.stNRXParam_V2.stNRXManual.stNRXParam.TFy[1].tEdge = g_astScenePipeParam[u8Index].stDynamicThreeDNR.astThreeDNRVPSSValue[u32IsoLevel].TFy[1].tEdge;
        stVPSSNRXParam.stNRXParam_V2.stNRXManual.stNRXParam.TFy[0].bRef = g_astScenePipeParam[u8Index].stDynamicThreeDNR.astThreeDNRVPSSValue[u32IsoLevel].TFy[0].bRef;
        //stVPSSNRXParam.stNRXParam_V2.stNRXManual.stNRXParam.GMC.GMEMode = g_astScenePipeParam[u8Index].stDynamicThreeDNR.astThreeDNRVPSSValue[u32IsoLevel].GMC.GMEMode;
        stGrpAttr.stNrAttr.enNrMotionMode = g_astScenePipeParam[u8Index].stDynamicThreeDNR.astThreeDNRVPSSValue[u32IsoLevel].GMC.GMEMode;
        stVPSSNRXParam.stNRXParam_V2.stNRXManual.stNRXParam.TFy[0].RFI = g_astScenePipeParam[u8Index].stDynamicThreeDNR.astThreeDNRVPSSValue[u32IsoLevel].TFy[0].RFI;
        stVPSSNRXParam.stNRXParam_V2.stNRXManual.stNRXParam.TFy[0].DZMode0 = g_astScenePipeParam[u8Index].stDynamicThreeDNR.astThreeDNRVPSSValue[u32IsoLevel].TFy[0].DZMode0;
        stVPSSNRXParam.stNRXParam_V2.stNRXManual.stNRXParam.TFy[0].DZMode1 = g_astScenePipeParam[u8Index].stDynamicThreeDNR.astThreeDNRVPSSValue[u32IsoLevel].TFy[0].DZMode1;
        stVPSSNRXParam.stNRXParam_V2.stNRXManual.stNRXParam.TFy[1].DZMode0 = g_astScenePipeParam[u8Index].stDynamicThreeDNR.astThreeDNRVPSSValue[u32IsoLevel].TFy[1].DZMode0;
        stVPSSNRXParam.stNRXParam_V2.stNRXManual.stNRXParam.TFy[1].DZMode1 = g_astScenePipeParam[u8Index].stDynamicThreeDNR.astThreeDNRVPSSValue[u32IsoLevel].TFy[1].DZMode1;

        stVPSSNRXParam.stNRXParam_V2.stNRXManual.stNRXParam.RFs.RFUI = g_astScenePipeParam[u8Index].stDynamicThreeDNR.astThreeDNRVPSSValue[u32IsoLevel].RFs.RFUI;
        stVPSSNRXParam.stNRXParam_V2.stNRXManual.stNRXParam.RFs.RFDZ = g_astScenePipeParam[u8Index].stDynamicThreeDNR.astThreeDNRVPSSValue[u32IsoLevel].RFs.RFDZ;
        stVPSSNRXParam.stNRXParam_V2.stNRXManual.stNRXParam.RFs.RFSLP = g_astScenePipeParam[u8Index].stDynamicThreeDNR.astThreeDNRVPSSValue[u32IsoLevel].RFs.RFSLP;
        stVPSSNRXParam.stNRXParam_V2.stNRXManual.stNRXParam.RFs.advMATH = g_astScenePipeParam[u8Index].stDynamicThreeDNR.astThreeDNRVPSSValue[u32IsoLevel].RFs.advMATH;

        for (i = 0; i < 2; i++)
        {

            stVPSSNRXParam.stNRXParam_V2.stNRXManual.stNRXParam.MDy[i].MATH0 = SCENE_Interpulate(u32ISO,
                    g_astScenePipeParam[u8Index].stDynamicThreeDNR.au32ThreeDNRIso[u32IsoLevel],
                    g_astScenePipeParam[u8Index].stDynamicThreeDNR.astThreeDNRVPSSValue[u32IsoLevel].MDy[i].MATH0,
                    g_astScenePipeParam[u8Index].stDynamicThreeDNR.au32ThreeDNRIso[u32NextIsoLevel],
                    g_astScenePipeParam[u8Index].stDynamicThreeDNR.astThreeDNRVPSSValue[u32NextIsoLevel].MDy[i].MATH0);
            stVPSSNRXParam.stNRXParam_V2.stNRXManual.stNRXParam.MDy[i].MATH1 = SCENE_Interpulate(u32ISO,
                    g_astScenePipeParam[u8Index].stDynamicThreeDNR.au32ThreeDNRIso[u32IsoLevel],
                    g_astScenePipeParam[u8Index].stDynamicThreeDNR.astThreeDNRVPSSValue[u32IsoLevel].MDy[i].MATH1,
                    g_astScenePipeParam[u8Index].stDynamicThreeDNR.au32ThreeDNRIso[u32NextIsoLevel],
                    g_astScenePipeParam[u8Index].stDynamicThreeDNR.astThreeDNRVPSSValue[u32NextIsoLevel].MDy[i].MATH1);
            stVPSSNRXParam.stNRXParam_V2.stNRXManual.stNRXParam.MDy[i].MATE0 = SCENE_Interpulate(u32ISO,
                    g_astScenePipeParam[u8Index].stDynamicThreeDNR.au32ThreeDNRIso[u32IsoLevel],
                    g_astScenePipeParam[u8Index].stDynamicThreeDNR.astThreeDNRVPSSValue[u32IsoLevel].MDy[i].MATE0,
                    g_astScenePipeParam[u8Index].stDynamicThreeDNR.au32ThreeDNRIso[u32NextIsoLevel],
                    g_astScenePipeParam[u8Index].stDynamicThreeDNR.astThreeDNRVPSSValue[u32NextIsoLevel].MDy[i].MATE0);
            stVPSSNRXParam.stNRXParam_V2.stNRXManual.stNRXParam.MDy[i].MATE1 = SCENE_Interpulate(u32ISO,
                    g_astScenePipeParam[u8Index].stDynamicThreeDNR.au32ThreeDNRIso[u32IsoLevel],
                    g_astScenePipeParam[u8Index].stDynamicThreeDNR.astThreeDNRVPSSValue[u32IsoLevel].MDy[i].MATE1,
                    g_astScenePipeParam[u8Index].stDynamicThreeDNR.au32ThreeDNRIso[u32NextIsoLevel],
                    g_astScenePipeParam[u8Index].stDynamicThreeDNR.astThreeDNRVPSSValue[u32NextIsoLevel].MDy[i].MATE1);
            stVPSSNRXParam.stNRXParam_V2.stNRXManual.stNRXParam.MDy[i].MABW0 = SCENE_Interpulate(u32ISO,
                    g_astScenePipeParam[u8Index].stDynamicThreeDNR.au32ThreeDNRIso[u32IsoLevel],
                    g_astScenePipeParam[u8Index].stDynamicThreeDNR.astThreeDNRVPSSValue[u32IsoLevel].MDy[i].MABW0,
                    g_astScenePipeParam[u8Index].stDynamicThreeDNR.au32ThreeDNRIso[u32NextIsoLevel],
                    g_astScenePipeParam[u8Index].stDynamicThreeDNR.astThreeDNRVPSSValue[u32NextIsoLevel].MDy[i].MABW0);
            stVPSSNRXParam.stNRXParam_V2.stNRXManual.stNRXParam.MDy[i].MABW1 = SCENE_Interpulate(u32ISO,
                    g_astScenePipeParam[u8Index].stDynamicThreeDNR.au32ThreeDNRIso[u32IsoLevel],
                    g_astScenePipeParam[u8Index].stDynamicThreeDNR.astThreeDNRVPSSValue[u32IsoLevel].MDy[i].MABW1,
                    g_astScenePipeParam[u8Index].stDynamicThreeDNR.au32ThreeDNRIso[u32NextIsoLevel],
                    g_astScenePipeParam[u8Index].stDynamicThreeDNR.astThreeDNRVPSSValue[u32NextIsoLevel].MDy[i].MABW1);
            stVPSSNRXParam.stNRXParam_V2.stNRXManual.stNRXParam.MDy[i].MASW = SCENE_Interpulate(u32ISO,
                    g_astScenePipeParam[u8Index].stDynamicThreeDNR.au32ThreeDNRIso[u32IsoLevel],
                    g_astScenePipeParam[u8Index].stDynamicThreeDNR.astThreeDNRVPSSValue[u32IsoLevel].MDy[i].MASW,
                    g_astScenePipeParam[u8Index].stDynamicThreeDNR.au32ThreeDNRIso[u32NextIsoLevel],
                    g_astScenePipeParam[u8Index].stDynamicThreeDNR.astThreeDNRVPSSValue[u32NextIsoLevel].MDy[i].MASW);
            stVPSSNRXParam.stNRXParam_V2.stNRXManual.stNRXParam.MDy[i].MADZ0 = SCENE_Interpulate(u32ISO,
                    g_astScenePipeParam[u8Index].stDynamicThreeDNR.au32ThreeDNRIso[u32IsoLevel],
                    g_astScenePipeParam[u8Index].stDynamicThreeDNR.astThreeDNRVPSSValue[u32IsoLevel].MDy[i].MADZ0,
                    g_astScenePipeParam[u8Index].stDynamicThreeDNR.au32ThreeDNRIso[u32NextIsoLevel],
                    g_astScenePipeParam[u8Index].stDynamicThreeDNR.astThreeDNRVPSSValue[u32NextIsoLevel].MDy[i].MADZ0);
            stVPSSNRXParam.stNRXParam_V2.stNRXManual.stNRXParam.MDy[i].MADZ1 = SCENE_Interpulate(u32ISO,
                    g_astScenePipeParam[u8Index].stDynamicThreeDNR.au32ThreeDNRIso[u32IsoLevel],
                    g_astScenePipeParam[u8Index].stDynamicThreeDNR.astThreeDNRVPSSValue[u32IsoLevel].MDy[i].MADZ1,
                    g_astScenePipeParam[u8Index].stDynamicThreeDNR.au32ThreeDNRIso[u32NextIsoLevel],
                    g_astScenePipeParam[u8Index].stDynamicThreeDNR.astThreeDNRVPSSValue[u32NextIsoLevel].MDy[i].MADZ1);

            if ((stVPSSNRXParam.stNRXParam_V2.stNRXManual.stNRXParam.MDy[i].MADZ0 > 0) || (stVPSSNRXParam.stNRXParam_V2.stNRXManual.stNRXParam.MDy[i].MADZ1 > 0))
            {
                //for (j = 0; j > 16; j++)
                {
                    stVPSSNRXParam.stNRXParam_V2.stNRXManual.stNRXParam.MDy[i].MABR0 = g_astScenePipeParam[u8Index].stDynamicThreeDNR.astThreeDNRVPSSValue[u32IsoLevel].MDy[i].MABR0;
                    stVPSSNRXParam.stNRXParam_V2.stNRXManual.stNRXParam.MDy[i].MABR1 = g_astScenePipeParam[u8Index].stDynamicThreeDNR.astThreeDNRVPSSValue[u32IsoLevel].MDy[i].MABR1;
                }
            }

            stVPSSNRXParam.stNRXParam_V2.stNRXManual.stNRXParam.MDy[i].biPath = g_astScenePipeParam[u8Index].stDynamicThreeDNR.astThreeDNRVPSSValue[u32IsoLevel].MDy[i].biPath;
            stVPSSNRXParam.stNRXParam_V2.stNRXManual.stNRXParam.MDy[i].MAI00 = g_astScenePipeParam[u8Index].stDynamicThreeDNR.astThreeDNRVPSSValue[u32IsoLevel].MDy[i].MAI00;
            stVPSSNRXParam.stNRXParam_V2.stNRXManual.stNRXParam.MDy[i].MAI01 = g_astScenePipeParam[u8Index].stDynamicThreeDNR.astThreeDNRVPSSValue[u32IsoLevel].MDy[i].MAI01;
            stVPSSNRXParam.stNRXParam_V2.stNRXManual.stNRXParam.MDy[i].MAI02 = g_astScenePipeParam[u8Index].stDynamicThreeDNR.astThreeDNRVPSSValue[u32IsoLevel].MDy[i].MAI02;
            stVPSSNRXParam.stNRXParam_V2.stNRXManual.stNRXParam.MDy[i].MAI10 = g_astScenePipeParam[u8Index].stDynamicThreeDNR.astThreeDNRVPSSValue[u32IsoLevel].MDy[i].MAI10;
            stVPSSNRXParam.stNRXParam_V2.stNRXManual.stNRXParam.MDy[i].MAI11 = g_astScenePipeParam[u8Index].stDynamicThreeDNR.astThreeDNRVPSSValue[u32IsoLevel].MDy[i].MAI11;
            stVPSSNRXParam.stNRXParam_V2.stNRXManual.stNRXParam.MDy[i].MAI12 = g_astScenePipeParam[u8Index].stDynamicThreeDNR.astThreeDNRVPSSValue[u32IsoLevel].MDy[i].MAI12;
            stVPSSNRXParam.stNRXParam_V2.stNRXManual.stNRXParam.MDy[i].MATW  = g_astScenePipeParam[u8Index].stDynamicThreeDNR.astThreeDNRVPSSValue[u32IsoLevel].MDy[i].MATW;
            stVPSSNRXParam.stNRXParam_V2.stNRXManual.stNRXParam.MDy[i].MASW  = g_astScenePipeParam[u8Index].stDynamicThreeDNR.astThreeDNRVPSSValue[u32IsoLevel].MDy[i].MASW;
        }

        for (i = 0; i < 2; i++)
        {
            stVPSSNRXParam.stNRXParam_V2.stNRXManual.stNRXParam.TFy[i].TSS0 = SCENE_Interpulate(u32ISO,
                    g_astScenePipeParam[u8Index].stDynamicThreeDNR.au32ThreeDNRIso[u32IsoLevel],
                    g_astScenePipeParam[u8Index].stDynamicThreeDNR.astThreeDNRVPSSValue[u32IsoLevel].TFy[i].TSS0,
                    g_astScenePipeParam[u8Index].stDynamicThreeDNR.au32ThreeDNRIso[u32NextIsoLevel],
                    g_astScenePipeParam[u8Index].stDynamicThreeDNR.astThreeDNRVPSSValue[u32NextIsoLevel].TFy[i].TSS0);
            stVPSSNRXParam.stNRXParam_V2.stNRXManual.stNRXParam.TFy[i].TSS1 = SCENE_Interpulate(u32ISO,
                    g_astScenePipeParam[u8Index].stDynamicThreeDNR.au32ThreeDNRIso[u32IsoLevel],
                    g_astScenePipeParam[u8Index].stDynamicThreeDNR.astThreeDNRVPSSValue[u32IsoLevel].TFy[i].TSS1,
                    g_astScenePipeParam[u8Index].stDynamicThreeDNR.au32ThreeDNRIso[u32NextIsoLevel],
                    g_astScenePipeParam[u8Index].stDynamicThreeDNR.astThreeDNRVPSSValue[u32NextIsoLevel].TFy[i].TSS1);
            stVPSSNRXParam.stNRXParam_V2.stNRXManual.stNRXParam.TFy[i].TFS0 = SCENE_Interpulate(u32ISO,
                    g_astScenePipeParam[u8Index].stDynamicThreeDNR.au32ThreeDNRIso[u32IsoLevel],
                    g_astScenePipeParam[u8Index].stDynamicThreeDNR.astThreeDNRVPSSValue[u32IsoLevel].TFy[i].TFS0,
                    g_astScenePipeParam[u8Index].stDynamicThreeDNR.au32ThreeDNRIso[u32NextIsoLevel],
                    g_astScenePipeParam[u8Index].stDynamicThreeDNR.astThreeDNRVPSSValue[u32NextIsoLevel].TFy[i].TFS0);
            stVPSSNRXParam.stNRXParam_V2.stNRXManual.stNRXParam.TFy[i].TFS1 = SCENE_Interpulate(u32ISO,
                    g_astScenePipeParam[u8Index].stDynamicThreeDNR.au32ThreeDNRIso[u32IsoLevel],
                    g_astScenePipeParam[u8Index].stDynamicThreeDNR.astThreeDNRVPSSValue[u32IsoLevel].TFy[i].TFS1,
                    g_astScenePipeParam[u8Index].stDynamicThreeDNR.au32ThreeDNRIso[u32NextIsoLevel],
                    g_astScenePipeParam[u8Index].stDynamicThreeDNR.astThreeDNRVPSSValue[u32NextIsoLevel].TFy[i].TFS1);

            for (j = 0; j < 6; j++)
            {
                stVPSSNRXParam.stNRXParam_V2.stNRXManual.stNRXParam.TFy[i].TFR0[j] = SCENE_Interpulate(u32ISO,
                        g_astScenePipeParam[u8Index].stDynamicThreeDNR.au32ThreeDNRIso[u32IsoLevel],
                        g_astScenePipeParam[u8Index].stDynamicThreeDNR.astThreeDNRVPSSValue[u32IsoLevel].TFy[i].TFR0[j],
                        g_astScenePipeParam[u8Index].stDynamicThreeDNR.au32ThreeDNRIso[u32NextIsoLevel],
                        g_astScenePipeParam[u8Index].stDynamicThreeDNR.astThreeDNRVPSSValue[u32NextIsoLevel].TFy[i].TFR0[j]);
                stVPSSNRXParam.stNRXParam_V2.stNRXManual.stNRXParam.TFy[i].TFR0[j] = SCENE_Interpulate(u32ISO,
                        g_astScenePipeParam[u8Index].stDynamicThreeDNR.au32ThreeDNRIso[u32IsoLevel],
                        g_astScenePipeParam[u8Index].stDynamicThreeDNR.astThreeDNRVPSSValue[u32IsoLevel].TFy[i].TFR0[j],
                        g_astScenePipeParam[u8Index].stDynamicThreeDNR.au32ThreeDNRIso[u32NextIsoLevel],
                        g_astScenePipeParam[u8Index].stDynamicThreeDNR.astThreeDNRVPSSValue[u32NextIsoLevel].TFy[i].TFR0[j]);
                stVPSSNRXParam.stNRXParam_V2.stNRXManual.stNRXParam.TFy[i].TFR1[j] = SCENE_Interpulate(u32ISO,
                        g_astScenePipeParam[u8Index].stDynamicThreeDNR.au32ThreeDNRIso[u32IsoLevel],
                        g_astScenePipeParam[u8Index].stDynamicThreeDNR.astThreeDNRVPSSValue[u32IsoLevel].TFy[i].TFR1[j],
                        g_astScenePipeParam[u8Index].stDynamicThreeDNR.au32ThreeDNRIso[u32NextIsoLevel],
                        g_astScenePipeParam[u8Index].stDynamicThreeDNR.astThreeDNRVPSSValue[u32NextIsoLevel].TFy[i].TFR1[j]);
                stVPSSNRXParam.stNRXParam_V2.stNRXManual.stNRXParam.TFy[i].TFR1[j] = SCENE_Interpulate(u32ISO,
                        g_astScenePipeParam[u8Index].stDynamicThreeDNR.au32ThreeDNRIso[u32IsoLevel],
                        g_astScenePipeParam[u8Index].stDynamicThreeDNR.astThreeDNRVPSSValue[u32IsoLevel].TFy[i].TFR1[j],
                        g_astScenePipeParam[u8Index].stDynamicThreeDNR.au32ThreeDNRIso[u32NextIsoLevel],
                        g_astScenePipeParam[u8Index].stDynamicThreeDNR.astThreeDNRVPSSValue[u32NextIsoLevel].TFy[i].TFR1[j]);
            }

            stVPSSNRXParam.stNRXParam_V2.stNRXManual.stNRXParam.TFy[i].STR0 = g_astScenePipeParam[u8Index].stDynamicThreeDNR.astThreeDNRVPSSValue[u32IsoLevel].TFy[i].STR0;
            stVPSSNRXParam.stNRXParam_V2.stNRXManual.stNRXParam.TFy[i].STR1 = g_astScenePipeParam[u8Index].stDynamicThreeDNR.astThreeDNRVPSSValue[u32IsoLevel].TFy[i].STR1;
            stVPSSNRXParam.stNRXParam_V2.stNRXManual.stNRXParam.TFy[i].SDZ0 = g_astScenePipeParam[u8Index].stDynamicThreeDNR.astThreeDNRVPSSValue[u32IsoLevel].TFy[i].SDZ0;
            stVPSSNRXParam.stNRXParam_V2.stNRXManual.stNRXParam.TFy[i].SDZ1 = g_astScenePipeParam[u8Index].stDynamicThreeDNR.astThreeDNRVPSSValue[u32IsoLevel].TFy[i].SDZ1;
            stVPSSNRXParam.stNRXParam_V2.stNRXManual.stNRXParam.TFy[i].TSI0 = g_astScenePipeParam[u8Index].stDynamicThreeDNR.astThreeDNRVPSSValue[u32IsoLevel].TFy[i].TSI0;
            stVPSSNRXParam.stNRXParam_V2.stNRXManual.stNRXParam.TFy[i].TSI1 = g_astScenePipeParam[u8Index].stDynamicThreeDNR.astThreeDNRVPSSValue[u32IsoLevel].TFy[i].TSI1;
            stVPSSNRXParam.stNRXParam_V2.stNRXManual.stNRXParam.TFy[i].TDZ0 = g_astScenePipeParam[u8Index].stDynamicThreeDNR.astThreeDNRVPSSValue[u32IsoLevel].TFy[i].TDZ0;
            stVPSSNRXParam.stNRXParam_V2.stNRXManual.stNRXParam.TFy[i].TDZ1 = g_astScenePipeParam[u8Index].stDynamicThreeDNR.astThreeDNRVPSSValue[u32IsoLevel].TFy[i].TDZ1;
            stVPSSNRXParam.stNRXParam_V2.stNRXManual.stNRXParam.TFy[i].TDX0 = g_astScenePipeParam[u8Index].stDynamicThreeDNR.astThreeDNRVPSSValue[u32IsoLevel].TFy[i].TDX0;
            stVPSSNRXParam.stNRXParam_V2.stNRXManual.stNRXParam.TFy[i].TDX1 = g_astScenePipeParam[u8Index].stDynamicThreeDNR.astThreeDNRVPSSValue[u32IsoLevel].TFy[i].TDX1;
        }

        stVPSSNRXParam.stNRXParam_V2.stNRXManual.stNRXParam.pNRc.SFC = SCENE_Interpulate(u32ISO,
                g_astScenePipeParam[u8Index].stDynamicThreeDNR.au32ThreeDNRIso[u32IsoLevel],
                g_astScenePipeParam[u8Index].stDynamicThreeDNR.astThreeDNRVPSSValue[u32IsoLevel].pNRc.SFC,
                g_astScenePipeParam[u8Index].stDynamicThreeDNR.au32ThreeDNRIso[u32NextIsoLevel],
                g_astScenePipeParam[u8Index].stDynamicThreeDNR.astThreeDNRVPSSValue[u32NextIsoLevel].pNRc.SFC);
        stVPSSNRXParam.stNRXParam_V2.stNRXManual.stNRXParam.pNRc.CTFS = SCENE_Interpulate(u32ISO,
                g_astScenePipeParam[u8Index].stDynamicThreeDNR.au32ThreeDNRIso[u32IsoLevel],
                g_astScenePipeParam[u8Index].stDynamicThreeDNR.astThreeDNRVPSSValue[u32IsoLevel].pNRc.CTFS,
                g_astScenePipeParam[u8Index].stDynamicThreeDNR.au32ThreeDNRIso[u32NextIsoLevel],
                g_astScenePipeParam[u8Index].stDynamicThreeDNR.astThreeDNRVPSSValue[u32NextIsoLevel].pNRc.CTFS);
        stVPSSNRXParam.stNRXParam_V2.stNRXManual.stNRXParam.pNRc.TFC = SCENE_Interpulate(u32ISO,
                g_astScenePipeParam[u8Index].stDynamicThreeDNR.au32ThreeDNRIso[u32IsoLevel],
                g_astScenePipeParam[u8Index].stDynamicThreeDNR.astThreeDNRVPSSValue[u32IsoLevel].pNRc.TFC,
                g_astScenePipeParam[u8Index].stDynamicThreeDNR.au32ThreeDNRIso[u32NextIsoLevel],
                g_astScenePipeParam[u8Index].stDynamicThreeDNR.astThreeDNRVPSSValue[u32NextIsoLevel].pNRc.TFC);


        stVINRXParam .enNRVersion = VI_NR_V2;
        stVINRXParam .stNRXParamV2.enOptMode = OPERATION_MODE_MANUAL;
        stVPSSNRXParam .enNRVer = VPSS_NR_V2;
        stVPSSNRXParam .stNRXParam_V2.enOptMode = OPERATION_MODE_MANUAL;

        CHECK_SCENE_PAUSE();
        s32Ret = HI_MPI_VI_SetPipeNRXParam(ViPipe, &stVINRXParam);
        CHECK_SCENE_RET(s32Ret);
        s32Ret = HI_MPI_VPSS_SetGrpNRXParam(VpssGrp, &stVPSSNRXParam);
        CHECK_SCENE_RET(s32Ret);

#if SCENE_3DNR_PRINT_ENABLE
        static int h = 0;
        HI_MPI_VI_GetPipeNRXParam(ViPipe, &stVINRXParam);
        HI_MPI_VPSS_GetGrpNRXParam(ViPipe, &stVPSSNRXParam);

        if ((h++ & 0xF) == 0)
        {
            printf("\n====ISO:%d, u32IsoLevel:%d===\n", u32ISO, u32IsoLevel);
            PrintNRxParam(&stVINRXParam, &stVPSSNRXParam);
        }

#endif

        s32Ret = HI_MPI_VPSS_GetGrpAttr(VpssGrp, &stGrpAttr);
        CHECK_SCENE_RET(s32Ret);
        s32Ret = HI_MPI_VI_GetPipeAttr(ViPipe, &stViPipeAttr);
        CHECK_SCENE_RET(s32Ret);
        s32Ret =  HI_MPI_ISP_GetDEAttr(ViPipe, &stDEAttr);
        CHECK_SCENE_RET(s32Ret);

        if (u32IsoLevel < g_astScenePipeParam[u8Index].stDynamicThreeDNR.u16VI_3DNRStartPoint)
        {
            stViPipeAttr.bNrEn = HI_FALSE;
            stDEAttr.bEnable = HI_TRUE;
        }
        else
        {
            stViPipeAttr.bNrEn = HI_TRUE;
            stDEAttr.bEnable = HI_FALSE;
        }

        stGrpAttr.bNrEn = HI_TRUE;

        ISP_PUB_ATTR_S stPubAttr;
        s32Ret = HI_MPI_ISP_GetPubAttr(ViPipe, &stPubAttr);
        CHECK_SCENE_RET(s32Ret);

        if (stPubAttr.enWDRMode)
        {
            if (u32IsoLevel < g_astScenePipeParam[u8Index].stDynamicThreeDNR.u16VI_3DNRStartPoint)
            {
                stViPipeAttr.bNrEn = HI_FALSE;
                stDEAttr.bEnable = HI_TRUE;
            }
            else
            {
                stViPipeAttr.bNrEn = HI_TRUE;
                stDEAttr.bEnable = HI_FALSE;
            }

            stGrpAttr.bNrEn = HI_TRUE;
        }

        s32Ret = HI_MPI_VI_SetPipeAttr(ViPipe, &stViPipeAttr);
        CHECK_SCENE_RET(s32Ret);
        s32Ret = HI_MPI_VPSS_SetGrpAttr(VpssGrp, &stGrpAttr );
        CHECK_SCENE_RET(s32Ret);
        s32Ret =  HI_MPI_ISP_SetDEAttr(ViPipe, &stDEAttr);
        CHECK_SCENE_RET(s32Ret);
    }

    return HI_SUCCESS;
}

HI_S32 HI_SCENE_SetDynamicAE_AutoGenerate(VI_PIPE ViPipe, HI_U64 u64Exposure, HI_U64 u64LastExposure, HI_U8 u8Index)
{
	if(HI_TRUE != g_astScenePipeParam[u8Index].stModuleState.bDynamicAE)
	{
		return HI_SUCCESS;
	}

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
	if(HI_TRUE != g_astScenePipeParam[u8Index].stModuleState.bDynamicShading)
	{
		return HI_SUCCESS;
	}

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
	if(HI_TRUE != g_astScenePipeParam[u8Index].stModuleState.bDynamicLdci)
	{
		return HI_SUCCESS;
	}

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
	if(HI_TRUE != g_astScenePipeParam[u8Index].stModuleState.bDynamicDehaze)
	{
		return HI_SUCCESS;
	}

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
	if(HI_TRUE != g_astScenePipeParam[u8Index].stModuleState.bDynamicFSWDR)
	{
		return HI_SUCCESS;
	}

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
        stFSWDRAttr.stBnr.enBnrMode = (ISP_BNR_MODE_E)g_astScenePipeParam[u8Index].stDynamicFSWDR.au8BnrMode[u32IsoLevel];

        if (u32ISO < g_astScenePipeParam[u8Index].stDynamicFSWDR.au32ISOModeChange[0])
        {
            stFSWDRAttr.enWDRMergeMode = g_astScenePipeParam[u8Index].stDynamicFSWDR.au8MergeMode[0];
        }
        else
        {
            stFSWDRAttr.enWDRMergeMode = g_astScenePipeParam[u8Index].stDynamicFSWDR.au8MergeMode[1];
        }

        CHECK_SCENE_PAUSE();
        s32Ret = HI_MPI_ISP_SetFSWDRAttr(ViPipe, &stFSWDRAttr);
        CHECK_SCENE_RET(s32Ret);
    }

    return HI_SUCCESS;
}

HI_S32 HI_SCENE_SetStaticDemosaic_AutoGenerate(VI_PIPE ViPipe, HI_U8 u8Index)
{
	if(HI_TRUE != g_astScenePipeParam[u8Index].stModuleState.bStaticDemosaic)
	{
		return HI_SUCCESS;
	}

    HI_U32 i = 0;
    HI_S32 s32Ret = HI_SUCCESS;

    ISP_DEMOSAIC_ATTR_S stDemosaicAttr;

    CHECK_SCENE_PAUSE();
    s32Ret = HI_MPI_ISP_GetDemosaicAttr(ViPipe, &stDemosaicAttr);
    CHECK_SCENE_RET(s32Ret);

    for (i = 0; i < ISP_AUTO_ISO_STRENGTH_NUM; i++)
    {
        stDemosaicAttr.stAuto.au8NonDirMFDetailEhcStr[i] = g_astScenePipeParam[u8Index].stStaticDemosaic.au8NonDirMFDetailEhcStr[i];
    }

    CHECK_SCENE_PAUSE();
    s32Ret = HI_MPI_ISP_SetDemosaicAttr(ViPipe, &stDemosaicAttr);
    CHECK_SCENE_RET(s32Ret);

    return HI_SUCCESS;
}

HI_S32 HI_SCENE_SetStaticSharpen_AutoGenerate(VI_PIPE ViPipe, HI_U8 u8Index)
{
	if(HI_TRUE != g_astScenePipeParam[u8Index].stModuleState.bStaticSharpen)
	{
		return HI_SUCCESS;
	}

    HI_U32 i = 0, j = 0;
    HI_S32 s32Ret = HI_SUCCESS;

    ISP_SHARPEN_ATTR_S stSharpenAttr;

    CHECK_SCENE_PAUSE();
    s32Ret = HI_MPI_ISP_GetIspSharpenAttr(ViPipe, &stSharpenAttr);
    CHECK_SCENE_RET(s32Ret);

    for (i = 0; i < ISP_AUTO_ISO_STRENGTH_NUM; i++)
    {
        stSharpenAttr.stAuto.au8OverShoot[i] = g_astScenePipeParam[u8Index].stStaticSharpen.au8OverShoot[i];
        stSharpenAttr.stAuto.au8UnderShoot[i] = g_astScenePipeParam[u8Index].stStaticSharpen.au8UnderShoot[i];
    }

    for (i = 0; i < ISP_SHARPEN_GAIN_NUM; i++)
    {
        for (j = 0; j < ISP_AUTO_ISO_STRENGTH_NUM; j++)
        {
            stSharpenAttr.stAuto.au16TextureStr[i][j] = g_astScenePipeParam[u8Index].stStaticSharpen.au16TextureStr[j][i];
            stSharpenAttr.stAuto.au16EdgeStr[i][j] = g_astScenePipeParam[u8Index].stStaticSharpen.au16EdgeStr[j][i];
        }
    }

    CHECK_SCENE_PAUSE();
    s32Ret = HI_MPI_ISP_SetIspSharpenAttr(ViPipe, &stSharpenAttr);
    CHECK_SCENE_RET(s32Ret);

    return HI_SUCCESS;
}

HI_S32 HI_SCENE_SetDynamicBayernr_AutoGenerate(VI_PIPE ViPipe, HI_U32 u32Iso, HI_U32 u32LastIso, HI_U8 u8Index)
{
	if(HI_TRUE != g_astScenePipeParam[u8Index].stModuleState.bDynamicBayernr)
	{
		return HI_SUCCESS;
	}

    HI_U32 i = 0;
    HI_S32 s32Ret = HI_SUCCESS;
    HI_U32 u32IsoLevel = 0;

    ISP_NR_ATTR_S stBayernrAttr;

    if (u32Iso != u32LastIso)
    {
        CHECK_SCENE_PAUSE();
        s32Ret = HI_MPI_ISP_GetNRAttr(ViPipe, &stBayernrAttr);
        CHECK_SCENE_RET(s32Ret);

        u32IsoLevel = SCENE_GetLevelLtoH_U32(u32Iso, g_astScenePipeParam[u8Index].stDynamicBayernr.u32IsoCount, g_astScenePipeParam[u8Index].stDynamicBayernr.au32IsoLevel);

        for (i = 0; i < ISP_AUTO_ISO_STRENGTH_NUM; i++)
        {
            stBayernrAttr.stAuto.au16CoringWgt[i] = g_astScenePipeParam[u8Index].stDynamicBayernr.au16CoringWgt[i];
            stBayernrAttr.stAuto.au8FineStr[i] = g_astScenePipeParam[u8Index].stDynamicBayernr.au8FineStr[i];
        }

        if (0 == u32IsoLevel || g_astScenePipeParam[u8Index].stDynamicBayernr.u32IsoCount - 1 == u32IsoLevel)
        {
            for (i = 0; i < HI_ISP_BAYERNR_LUT_LENGTH; i++)
            {
                stBayernrAttr.au16CoringRatio[i] = g_astScenePipeParam[u8Index].stDynamicBayernr.au16CoringRatio[u32IsoLevel][i];
            }
        }
        else
        {
            for (i = 0; i < HI_ISP_BAYERNR_LUT_LENGTH; i++)
            {
                stBayernrAttr.au16CoringRatio[i] = SCENE_Interpulate(u32Iso, g_astScenePipeParam[u8Index].stDynamicBayernr.au32IsoLevel[u32IsoLevel - 1],
                                      g_astScenePipeParam[u8Index].stDynamicBayernr.au16CoringRatio[u32IsoLevel - 1][i],
                                      g_astScenePipeParam[u8Index].stDynamicBayernr.au32IsoLevel[u32IsoLevel],
                                      g_astScenePipeParam[u8Index].stDynamicBayernr.au16CoringRatio[u32IsoLevel][i]);
            }
        }

        CHECK_SCENE_PAUSE();
        s32Ret = HI_MPI_ISP_SetNRAttr(ViPipe, &stBayernrAttr);
        CHECK_SCENE_RET(s32Ret);
    }

    return HI_SUCCESS;
}

HI_S32 HI_SCENE_SetStaticDetail_AutoGenerate(VI_PIPE ViPipe, HI_U8 u8Index)
{
	if(HI_TRUE != g_astScenePipeParam[u8Index].stModuleState.bStaticDetail)
	{
		return HI_SUCCESS;
	}

    HI_U32 i = 0;
    HI_S32 s32Ret = HI_SUCCESS;

    ISP_DE_ATTR_S stDetailAttr;

    CHECK_SCENE_PAUSE();
    s32Ret = HI_MPI_ISP_GetDEAttr(ViPipe, &stDetailAttr);
    CHECK_SCENE_RET(s32Ret);

    for (i = 0; i < ISP_AUTO_ISO_STRENGTH_NUM; i++)
    {
        stDetailAttr.stAuto.au16GlobalGain[i] = g_astScenePipeParam[u8Index].stStaticDetail.au16GlobalGain[i];
    }

    CHECK_SCENE_PAUSE();
    s32Ret = HI_MPI_ISP_SetDEAttr(ViPipe, &stDetailAttr);
    CHECK_SCENE_RET(s32Ret);

    return HI_SUCCESS;
}


HI_S32 HI_SCENE_SetDynamicDIS_AutoGenerate(VI_PIPE ViPipe, VI_CHN ViChn, HI_U32 u32Iso, HI_U32 u32LastIso, HI_U8 u8Index, HI_BOOL bEnable)
{
	if(HI_TRUE != g_astScenePipeParam[u8Index].stModuleState.bDynamicDIS)
	{
		return HI_SUCCESS;
	}

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

HI_S32 HI_SCENE_SetDynamicLinearDRC_AutoGenerate(VI_PIPE ViPipe, HI_U32 u32Iso, HI_U32 u32LastIso, HI_U8 u8Index)
{
	if(HI_TRUE != g_astScenePipeParam[u8Index].stModuleState.bDynamicLinearDrc)
	{
		return HI_SUCCESS;
	}
    HI_U16 u16Strength = 0;
    HI_S32 s32Ret = HI_SUCCESS;
    HI_U32 u32IsoLevel = 0;
    HI_U32 u32ISOCount = g_astScenePipeParam[u8Index].stDynamicLinearDrc.u32ISOCount;
    HI_U32 *pu32IsoLevelThresh = g_astScenePipeParam[u8Index].stDynamicLinearDrc.au32ISOLevel;

    ISP_DRC_ATTR_S stDrcAttr;

    CHECK_SCENE_PAUSE();
    s32Ret = HI_MPI_ISP_GetDRCAttr(ViPipe, &stDrcAttr);
    CHECK_SCENE_RET(s32Ret);

    if(HI_TRUE == g_astScenePipeParam[u8Index].stDynamicLinearDrc.bEnable)
    {
        u32IsoLevel = SCENE_GetLevelLtoH_U32(u32Iso, u32ISOCount, pu32IsoLevelThresh);
        if (u32IsoLevel == 0 || u32IsoLevel == u32ISOCount - 1)
        {
            stDrcAttr.u8LocalMixingBrightMax = g_astScenePipeParam[u8Index].stDynamicLinearDrc.au8LocalMixingBrightMax[u32IsoLevel];
            stDrcAttr.u8LocalMixingBrightMin = g_astScenePipeParam[u8Index].stDynamicLinearDrc.au8LocalMixingBrightMin[u32IsoLevel];
            stDrcAttr.u8LocalMixingDarkMax = g_astScenePipeParam[u8Index].stDynamicLinearDrc.au8LocalMixingDarkMax[u32IsoLevel];
            stDrcAttr.u8LocalMixingDarkMin = g_astScenePipeParam[u8Index].stDynamicLinearDrc.au8LocalMixingDarkMin[u32IsoLevel];
            stDrcAttr.u8BrightGainLmt = g_astScenePipeParam[u8Index].stDynamicLinearDrc.au8BrightGainLmt[u32IsoLevel];
            stDrcAttr.u8BrightGainLmtStep = g_astScenePipeParam[u8Index].stDynamicLinearDrc.au8BrightGainLmtStep[u32IsoLevel];
            stDrcAttr.u8DarkGainLmtY = g_astScenePipeParam[u8Index].stDynamicLinearDrc.au8DarkGainLmtY[u32IsoLevel];
            stDrcAttr.u8DarkGainLmtC = g_astScenePipeParam[u8Index].stDynamicLinearDrc.au8DarkGainLmtC[u32IsoLevel];
            stDrcAttr.u8ContrastControl = g_astScenePipeParam[u8Index].stDynamicLinearDrc.au8ContrastControl[u32IsoLevel];
            stDrcAttr.s8DetailAdjustFactor = g_astScenePipeParam[u8Index].stDynamicLinearDrc.as8DetailAdjustFactor[u32IsoLevel];
            stDrcAttr.stAsymmetryCurve.u8Asymmetry = g_astScenePipeParam[u8Index].stDynamicLinearDrc.au8Asymmetry[u32IsoLevel];
            stDrcAttr.stAsymmetryCurve.u8SecondPole = g_astScenePipeParam[u8Index].stDynamicLinearDrc.au8SecondPole[u32IsoLevel];
            stDrcAttr.stAsymmetryCurve.u8Compress = g_astScenePipeParam[u8Index].stDynamicLinearDrc.au8Compress[u32IsoLevel];
            stDrcAttr.stAsymmetryCurve.u8Stretch = g_astScenePipeParam[u8Index].stDynamicLinearDrc.au8Stretch[u32IsoLevel];
            u16Strength = g_astScenePipeParam[u8Index].stDynamicLinearDrc.au16Strength[u32IsoLevel];
        }
        else
        {
            stDrcAttr.u8LocalMixingBrightMax = SCENE_Interpulate(u32Iso,
                                     pu32IsoLevelThresh[u32IsoLevel - 1], g_astScenePipeParam[u8Index].stDynamicLinearDrc.au8LocalMixingBrightMax[u32IsoLevel - 1],
                                     pu32IsoLevelThresh[u32IsoLevel], g_astScenePipeParam[u8Index].stDynamicLinearDrc.au8LocalMixingBrightMax[u32IsoLevel]);

            stDrcAttr.u8LocalMixingBrightMin = SCENE_Interpulate(u32Iso,
                                     pu32IsoLevelThresh[u32IsoLevel - 1], g_astScenePipeParam[u8Index].stDynamicLinearDrc.au8LocalMixingBrightMin[u32IsoLevel - 1],
                                     pu32IsoLevelThresh[u32IsoLevel], g_astScenePipeParam[u8Index].stDynamicLinearDrc.au8LocalMixingBrightMin[u32IsoLevel]);

            stDrcAttr.u8LocalMixingDarkMax = SCENE_Interpulate(u32Iso,
                                   pu32IsoLevelThresh[u32IsoLevel - 1], g_astScenePipeParam[u8Index].stDynamicLinearDrc.au8LocalMixingDarkMax[u32IsoLevel - 1],
                                   pu32IsoLevelThresh[u32IsoLevel], g_astScenePipeParam[u8Index].stDynamicLinearDrc.au8LocalMixingDarkMax[u32IsoLevel]);

            stDrcAttr.u8LocalMixingDarkMin = SCENE_Interpulate(u32Iso,
                                   pu32IsoLevelThresh[u32IsoLevel - 1], g_astScenePipeParam[u8Index].stDynamicLinearDrc.au8LocalMixingDarkMin[u32IsoLevel - 1],
                                   pu32IsoLevelThresh[u32IsoLevel], g_astScenePipeParam[u8Index].stDynamicLinearDrc.au8LocalMixingDarkMin[u32IsoLevel]);

            stDrcAttr.u8BrightGainLmt = SCENE_Interpulate(u32Iso,
                             pu32IsoLevelThresh[u32IsoLevel - 1],g_astScenePipeParam[u8Index].stDynamicLinearDrc.au8BrightGainLmt[u32IsoLevel - 1],
                             pu32IsoLevelThresh[u32IsoLevel], g_astScenePipeParam[u8Index].stDynamicLinearDrc.au8BrightGainLmt[u32IsoLevel]);

            stDrcAttr.u8BrightGainLmtStep = SCENE_Interpulate(u32Iso,
                             pu32IsoLevelThresh[u32IsoLevel - 1], g_astScenePipeParam[u8Index].stDynamicLinearDrc.au8BrightGainLmtStep[u32IsoLevel - 1],
                             pu32IsoLevelThresh[u32IsoLevel], g_astScenePipeParam[u8Index].stDynamicLinearDrc.au8BrightGainLmtStep[u32IsoLevel]);

            stDrcAttr.u8DarkGainLmtY = SCENE_Interpulate(u32Iso,
                             pu32IsoLevelThresh[u32IsoLevel - 1],g_astScenePipeParam[u8Index].stDynamicLinearDrc.au8DarkGainLmtY[u32IsoLevel - 1],
                             pu32IsoLevelThresh[u32IsoLevel], g_astScenePipeParam[u8Index].stDynamicLinearDrc.au8DarkGainLmtY[u32IsoLevel]);

            stDrcAttr.u8DarkGainLmtC = SCENE_Interpulate(u32Iso,
                             pu32IsoLevelThresh[u32IsoLevel - 1], g_astScenePipeParam[u8Index].stDynamicLinearDrc.au8DarkGainLmtC[u32IsoLevel - 1],
                             pu32IsoLevelThresh[u32IsoLevel], g_astScenePipeParam[u8Index].stDynamicLinearDrc.au8DarkGainLmtC[u32IsoLevel]);

            stDrcAttr.u8ContrastControl = SCENE_Interpulate(u32Iso,
                           pu32IsoLevelThresh[u32IsoLevel - 1], g_astScenePipeParam[u8Index].stDynamicLinearDrc.au8ContrastControl[u32IsoLevel - 1],
                           pu32IsoLevelThresh[u32IsoLevel], g_astScenePipeParam[u8Index].stDynamicLinearDrc.au8ContrastControl[u32IsoLevel]);

            stDrcAttr.s8DetailAdjustFactor = SCENE_Interpulate(u32Iso,
                                   pu32IsoLevelThresh[u32IsoLevel - 1], g_astScenePipeParam[u8Index].stDynamicLinearDrc.as8DetailAdjustFactor[u32IsoLevel - 1],
                                   pu32IsoLevelThresh[u32IsoLevel], g_astScenePipeParam[u8Index].stDynamicLinearDrc.as8DetailAdjustFactor[u32IsoLevel]);

            stDrcAttr.stAsymmetryCurve.u8Asymmetry = SCENE_Interpulate(u32Iso,
                           pu32IsoLevelThresh[u32IsoLevel - 1], g_astScenePipeParam[u8Index].stDynamicLinearDrc.au8Asymmetry[u32IsoLevel - 1],
                           pu32IsoLevelThresh[u32IsoLevel], g_astScenePipeParam[u8Index].stDynamicLinearDrc.au8Asymmetry[u32IsoLevel]);


            stDrcAttr.stAsymmetryCurve.u8SecondPole = SCENE_Interpulate(u32Iso,
                           pu32IsoLevelThresh[u32IsoLevel - 1], g_astScenePipeParam[u8Index].stDynamicLinearDrc.au8SecondPole[u32IsoLevel - 1],
                           pu32IsoLevelThresh[u32IsoLevel], g_astScenePipeParam[u8Index].stDynamicLinearDrc.au8SecondPole[u32IsoLevel]);

            stDrcAttr.stAsymmetryCurve.u8SecondPole = SCENE_Interpulate(u32Iso,
                           pu32IsoLevelThresh[u32IsoLevel - 1], g_astScenePipeParam[u8Index].stDynamicLinearDrc.au8SecondPole[u32IsoLevel - 1],
                           pu32IsoLevelThresh[u32IsoLevel], g_astScenePipeParam[u8Index].stDynamicLinearDrc.au8SecondPole[u32IsoLevel]);

            stDrcAttr.stAsymmetryCurve.u8Compress = SCENE_Interpulate(u32Iso,
                         pu32IsoLevelThresh[u32IsoLevel - 1], g_astScenePipeParam[u8Index].stDynamicLinearDrc.au8Compress[u32IsoLevel - 1],
                         pu32IsoLevelThresh[u32IsoLevel], g_astScenePipeParam[u8Index].stDynamicLinearDrc.au8Compress[u32IsoLevel]);

            stDrcAttr.stAsymmetryCurve.u8Stretch = SCENE_Interpulate(u32Iso,
                        pu32IsoLevelThresh[u32IsoLevel - 1], g_astScenePipeParam[u8Index].stDynamicLinearDrc.au8Stretch[u32IsoLevel - 1],
                        pu32IsoLevelThresh[u32IsoLevel], g_astScenePipeParam[u8Index].stDynamicLinearDrc.au8Stretch[u32IsoLevel]);

            u16Strength = SCENE_Interpulate(u32Iso,
                        pu32IsoLevelThresh[u32IsoLevel - 1], g_astScenePipeParam[u8Index].stDynamicLinearDrc.au16Strength[u32IsoLevel - 1],
                        pu32IsoLevelThresh[u32IsoLevel], g_astScenePipeParam[u8Index].stDynamicLinearDrc.au16Strength[u32IsoLevel]);
        }

        if(0 == stDrcAttr.enOpType)
        {
            stDrcAttr.stAuto.u16Strength = u16Strength;
        }
        else
        {
            stDrcAttr.stManual.u16Strength = u16Strength;
        }
    }

    CHECK_SCENE_PAUSE();
    s32Ret = HI_MPI_ISP_SetDRCAttr(ViPipe, &stDrcAttr);
    CHECK_SCENE_RET(s32Ret);

    return HI_SUCCESS;
}

#define HI_SCENE_REF_EXP_RATIO_FRM   (16)
#define HI_SCENE_REF_EXP_RATIO_LEVEL (9)
HI_S32 HI_SCENE_SetDynamicThreadDRC_AutoGenerate(VI_PIPE ViPipe, HI_U8 u8Index, HI_U32 u32ActRation, HI_U32 u32Iso, HI_U64 u64Exposure, HI_U64 u64LastExposure)
{
	if(HI_TRUE != g_astScenePipeParam[u8Index].stModuleState.bDynamicThreadDrc)
	{
		return HI_SUCCESS;
	}

    HI_U32 u32IsoLevel = 0;
    HI_U32 u32RatioLevel = 0;
    HI_U32 i = 0;
    HI_S32 s32Ret = HI_SUCCESS;

    ISP_EXP_INFO_S stExpInfo;
    ISP_PUB_ATTR_S stPubAttr;
    ISP_DRC_ATTR_S stIspDrcAttr;
    HI_U8 u8LocalMixingBrightMax = 0;
    HI_U8 u8LocalMixingBrightMin = 0;
    HI_U8 u8LocalMixingDarkMax = 0;
    HI_U8 u8LocalMixingDarkMin = 0;
    HI_U8 u8DarkGainLmtY = 0;
    HI_U8 u8DarkGainLmtC = 0;
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

    s32Ret = HI_MPI_ISP_QueryExposureInfo(ViPipe, &stExpInfo);
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
        u8DarkGainLmtY = g_astScenePipeParam[u8Index].stThreadDrc.au8DarkGainLmtY[u32IsoLevel];
    }
    else
    {
        u8DarkGainLmtY = SCENE_Interpulate(u32Iso, g_astScenePipeParam[u8Index].stThreadDrc.au32ISOLevel[u32IsoLevel - 1],
                                           g_astScenePipeParam[u8Index].stThreadDrc.au8DarkGainLmtY[u32IsoLevel - 1],
                                           g_astScenePipeParam[u8Index].stThreadDrc.au32ISOLevel[u32IsoLevel],
                                           g_astScenePipeParam[u8Index].stThreadDrc.au8DarkGainLmtY[u32IsoLevel]);
    }

    if (u32IsoLevel == 0 || u32IsoLevel == g_astScenePipeParam[u8Index].stThreadDrc.u32ISOCount - 1)
    {
        u8DarkGainLmtC = g_astScenePipeParam[u8Index].stThreadDrc.au8DarkGainLmtC[u32IsoLevel];
    }
    else
    {
        u8DarkGainLmtC = SCENE_Interpulate(u32Iso, g_astScenePipeParam[u8Index].stThreadDrc.au32ISOLevel[u32IsoLevel - 1],
                                           g_astScenePipeParam[u8Index].stThreadDrc.au8DarkGainLmtC[u32IsoLevel - 1],
                                           g_astScenePipeParam[u8Index].stThreadDrc.au32ISOLevel[u32IsoLevel],
                                           g_astScenePipeParam[u8Index].stThreadDrc.au8DarkGainLmtC[u32IsoLevel]);
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

    stIspDrcAttr.enCurveSelect = DRC_CURVE_USER;

    HI_U16 au16ToneMappingValue[200] = {0};
    HI_U32 u32CurRefExpRatio = 0;
    static HI_U32 au32RefExpRatio[HI_SCENE_PIPETYPE_NUM][HI_SCENE_REF_EXP_RATIO_FRM] = {0};
    HI_U32 u32RefRatioLevel = 0;
    HI_U32 u32RefExpRatioAlpha = 0;

    for (i = 0; i < HI_SCENE_REF_EXP_RATIO_FRM - 1; i++)
    {
        au32RefExpRatio[ViPipe][i] = au32RefExpRatio[ViPipe][i + 1];
    }

    au32RefExpRatio[ViPipe][i] = stExpInfo.u32RefExpRatio;

    for (i = 0; i < HI_SCENE_REF_EXP_RATIO_FRM; i++)
    {
        u32CurRefExpRatio += au32RefExpRatio[ViPipe][i];
    }

    u32CurRefExpRatio = u32CurRefExpRatio / HI_SCENE_REF_EXP_RATIO_FRM;
    //printf("u32RefExpRatio:%d, u32CurRefExpRatio:%d\n", stExpInfo.u32RefExpRatio, u32CurRefExpRatio);

    u32RefRatioLevel = SCENE_GetLevelLtoH_U32(u32CurRefExpRatio, g_astScenePipeParam[u8Index].stThreadDrc.u32RefRatioCount, g_astScenePipeParam[u8Index].stThreadDrc.au32RefRatioLtoH);

    //printf("u32RefRatioLevel:%d\n", u32RefRatioLevel);
    if (u32RefRatioLevel == 0 || u32RefRatioLevel == g_astScenePipeParam[u8Index].stThreadDrc.u32RefRatioCount - 1)
    {
        u32RefExpRatioAlpha = g_astScenePipeParam[u8Index].stThreadDrc.u32RefRatioAlpha[u32RefRatioLevel];
    }
    else
    {
        u32RefExpRatioAlpha = SCENE_Interpulate(u32CurRefExpRatio, g_astScenePipeParam[u8Index].stThreadDrc.au32RefRatioLtoH[u32RefRatioLevel - 1],
                                                g_astScenePipeParam[u8Index].stThreadDrc.u32RefRatioAlpha[u32RefRatioLevel - 1],
                                                g_astScenePipeParam[u8Index].stThreadDrc.au32RefRatioLtoH[u32RefRatioLevel],
                                                g_astScenePipeParam[u8Index].stThreadDrc.u32RefRatioAlpha[u32RefRatioLevel]);
    }

    //printf("u32RefExpRatioAlpha:%d\n", u32RefExpRatioAlpha);
    for (i = 0 ; i < 200 ; i++)
    {
        au16ToneMappingValue[i] = ((HI_U32)u32RefExpRatioAlpha * g_astScenePipeParam[u8Index].stThreadDrc.au16TMValueLow[i]
                                   + (HI_U32)(0x100 - u32RefExpRatioAlpha) * g_astScenePipeParam[u8Index].stThreadDrc.au16TMValueHigh[i]) >> 8;
    }

    //for (i = 0; i < g_astScenePipeParam[u8Index].stThreadDrc.u32Interval; i++)
    i = g_astScenePipeParam[u8Index].stThreadDrc.u32Interval >> 3;
    {
        stIspDrcAttr.u8LocalMixingBrightMax = SCENE_TimeFilter(stIspDrcAttr.u8LocalMixingBrightMax,
                                              u8LocalMixingBrightMax, g_astScenePipeParam[u8Index].stThreadDrc.u32Interval, i);
        stIspDrcAttr.u8LocalMixingBrightMin = SCENE_TimeFilter(stIspDrcAttr.u8LocalMixingBrightMin,
                                              u8LocalMixingBrightMin, g_astScenePipeParam[u8Index].stThreadDrc.u32Interval, i);
        stIspDrcAttr.u8LocalMixingDarkMax = SCENE_TimeFilter(stIspDrcAttr.u8LocalMixingDarkMax,
                                            u8LocalMixingDarkMax, g_astScenePipeParam[u8Index].stThreadDrc.u32Interval, i);
        stIspDrcAttr.u8LocalMixingDarkMin = SCENE_TimeFilter(stIspDrcAttr.u8LocalMixingDarkMin,
                                            u8LocalMixingDarkMin, g_astScenePipeParam[u8Index].stThreadDrc.u32Interval, i);
        stIspDrcAttr.u8DarkGainLmtY = SCENE_TimeFilter(stIspDrcAttr.u8DarkGainLmtY,
                                      u8DarkGainLmtY, g_astScenePipeParam[u8Index].stThreadDrc.u32Interval, i);
        stIspDrcAttr.u8DarkGainLmtC = SCENE_TimeFilter(stIspDrcAttr.u8DarkGainLmtC,
                                      u8DarkGainLmtC, g_astScenePipeParam[u8Index].stThreadDrc.u32Interval, i);
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

        for (j = 0 ; j < 200 ; j++)
        {
            stIspDrcAttr.au16ToneMappingValue[j]  = SCENE_TimeFilter(stIspDrcAttr.au16ToneMappingValue[j],
                                                    au16ToneMappingValue[j], g_astScenePipeParam[u8Index].stThreadDrc.u32Interval, i);
        }

        CHECK_SCENE_PAUSE();
        s32Ret = HI_MPI_ISP_SetDRCAttr(ViPipe, &stIspDrcAttr);
        CHECK_SCENE_RET(s32Ret);
        //usleep(80000);
    }
    return HI_SUCCESS;
}

HI_S32 HI_SCENE_SetPipeParam_AutoGenerate(const HI_SCENE_PIPE_PARAM_S* pstScenePipeParam, HI_U32 u32Num)
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

HI_S32 HI_SCENE_SetQP_AutoGenerate(HI_U32  u32PicWidth, HI_U32  u32PicHeight,HI_U32 u32MaxBitRate,PAYLOAD_TYPE_E  enType,VENC_RC_PARAM_S * pstRcParam)
{
     HI_U32 u32AlignPicHeight;
     HI_U32 u32AlignPicWidht;
     HI_U32 u32AlignPicHeight_1080P;
     HI_U32 u32AlignPicWidht_1080P;

     u32AlignPicHeight = ((u32PicHeight + MB_ALIGN_H264 - 1) / MB_ALIGN_H264);
     u32AlignPicWidht  = ((u32PicWidth + MB_ALIGN_H264- 1) / MB_ALIGN_H264);
     u32AlignPicHeight_1080P = ((PIC_HEIGHT_1080P + MB_ALIGN_H264- 1) / MB_ALIGN_H264);
     u32AlignPicWidht_1080P  = ((PIC_WIDTH_1080P + MB_ALIGN_H264- 1) / MB_ALIGN_H264);
     u32MaxBitRate = (u32AlignPicHeight_1080P *u32AlignPicWidht_1080P*u32MaxBitRate)/(u32AlignPicHeight*u32AlignPicWidht);

    if(enType == PT_H265)
    {

        if(u32MaxBitRate <= 768)
        {
             pstRcParam->stParamH265AVbr.u32MinIQp = 30;
	      pstRcParam->stParamH265AVbr.u32MinQp  = 32;
	      pstRcParam->stParamH265AVbr.u32MaxIQp = 48;
	      pstRcParam->stParamH265AVbr.u32MaxQp = 48;
        }
	 else if(u32MaxBitRate > 768 && u32MaxBitRate <= 1536)
	 {
	      pstRcParam->stParamH265AVbr.u32MinIQp = 29;
	      pstRcParam->stParamH265AVbr.u32MinQp  = 31;
	      pstRcParam->stParamH265AVbr.u32MaxIQp = 44;
	      pstRcParam->stParamH265AVbr.u32MaxQp = 44;
        }
	 else if(u32MaxBitRate > 1536 && u32MaxBitRate <= 2560)
	 {
	      pstRcParam->stParamH265AVbr.u32MinIQp = 26;
	      pstRcParam->stParamH265AVbr.u32MinQp  = 28;
	      pstRcParam->stParamH265AVbr.u32MaxIQp = 42;
	      pstRcParam->stParamH265AVbr.u32MaxQp = 42;
        }
	 else if(u32MaxBitRate > 2560 && u32MaxBitRate <= 3584)
	 {
	      pstRcParam->stParamH265AVbr.u32MinIQp = 25;
	      pstRcParam->stParamH265AVbr.u32MinQp  = 27;
	      pstRcParam->stParamH265AVbr.u32MaxIQp = 42;
	      pstRcParam->stParamH265AVbr.u32MaxQp = 42;
        }
	 else if(u32MaxBitRate > 3584 && u32MaxBitRate <= 4096)
	 {
	      pstRcParam->stParamH265AVbr.u32MinIQp = 24;
	      pstRcParam->stParamH265AVbr.u32MinQp  = 26;
	      pstRcParam->stParamH265AVbr.u32MaxIQp = 42;
	      pstRcParam->stParamH265AVbr.u32MaxQp = 42;
        }
	 else if(u32MaxBitRate > 4096)
	 {
	      pstRcParam->stParamH265AVbr.u32MinIQp = 23;
	      pstRcParam->stParamH265AVbr.u32MinQp  = 25;
	      pstRcParam->stParamH265AVbr.u32MaxIQp = 42;
	      pstRcParam->stParamH265AVbr.u32MaxQp = 42;
        }
       pstRcParam->stParamH265AVbr.u32MinIprop = 1;
       pstRcParam->stParamH265AVbr.u32MaxIprop  = 100;
       pstRcParam->stParamH265AVbr.s32MaxReEncodeTimes = 2;

       pstRcParam->stParamH265AVbr.s32MinStillPercent = 25;
	pstRcParam->stParamH265AVbr.u32MaxStillQP = 35;
       pstRcParam->stParamH265AVbr.u32MinStillPSNR = 0;
    }
    if(enType == PT_H264)
    {

        if(u32MaxBitRate <= 768)
        {
             pstRcParam->stParamH264AVbr.u32MinIQp = 30;
	      pstRcParam->stParamH264AVbr.u32MinQp  = 32;
	      pstRcParam->stParamH264AVbr.u32MaxIQp = 48;
	      pstRcParam->stParamH264AVbr.u32MaxQp = 48;
        }
	 else if(u32MaxBitRate > 768 && u32MaxBitRate <= 1536)
	 {
	      pstRcParam->stParamH264AVbr.u32MinIQp = 29;
	      pstRcParam->stParamH264AVbr.u32MinQp  = 31;
	      pstRcParam->stParamH264AVbr.u32MaxIQp = 44;
	      pstRcParam->stParamH264AVbr.u32MaxQp = 44;
        }
	 else if(u32MaxBitRate > 1536 && u32MaxBitRate <= 3072)
	 {
	      pstRcParam->stParamH264AVbr.u32MinIQp = 27;
	      pstRcParam->stParamH264AVbr.u32MinQp  = 29;
	      pstRcParam->stParamH264AVbr.u32MaxIQp = 42;
	      pstRcParam->stParamH264AVbr.u32MaxQp = 42;
        }
	 else if(u32MaxBitRate > 3072 && u32MaxBitRate <= 5120)
	 {
	      pstRcParam->stParamH264AVbr.u32MinIQp = 26;
	      pstRcParam->stParamH264AVbr.u32MinQp  = 28;
	      pstRcParam->stParamH264AVbr.u32MaxIQp = 42;
	      pstRcParam->stParamH264AVbr.u32MaxQp = 42;
        }
	 else if(u32MaxBitRate > 5120 && u32MaxBitRate <= 7168)
	 {
	      pstRcParam->stParamH264AVbr.u32MinIQp = 25;
	      pstRcParam->stParamH264AVbr.u32MinQp  = 27;
	      pstRcParam->stParamH264AVbr.u32MaxIQp = 42;
	      pstRcParam->stParamH264AVbr.u32MaxQp = 42;
        }
	 else if(u32MaxBitRate > 7168)
	 {
	      pstRcParam->stParamH264AVbr.u32MinIQp = 23;
	      pstRcParam->stParamH264AVbr.u32MinQp  = 25;
	      pstRcParam->stParamH264AVbr.u32MaxIQp = 42;
	      pstRcParam->stParamH264AVbr.u32MaxQp = 42;
        }
       pstRcParam->stParamH264AVbr.u32MinIprop = 1;
       pstRcParam->stParamH264AVbr.u32MaxIprop  = 100;
       pstRcParam->stParamH264AVbr.s32MaxReEncodeTimes = 2;

       pstRcParam->stParamH264AVbr.s32MinStillPercent = 25;
	pstRcParam->stParamH264AVbr.u32MaxStillQP = 35;
       pstRcParam->stParamH264AVbr.u32MinStillPSNR = 0;
    }
    return HI_SUCCESS;
}
HI_S32 HI_SCENE_SetRCParam_AutoGenerate(VI_PIPE ViPipe,HI_U8 u8Index)
{
    HI_S32 s32Ret = HI_SUCCESS;
    ISP_EXP_INFO_S stIspExpInfo;
    VENC_CHN_ATTR_S stVencChnAttr;
    VENC_CHN_STATUS_S stStatEx;
    VENC_RC_PARAM_S stRcParam;

    HI_U32 i,u32MinQP,u32Iso;
    HI_U32 u32MeanQp;								/*the start Qp of encoded frames*/
    HI_S32 s32Percent=90;
    HI_U32 u32SumMeanQP=0;
    HI_U32 u32SumIso = 0;
    HI_U32 u32MaxBitRate;
    PAYLOAD_TYPE_E  enType;
    static HI_U32  u32IsoInfo[8] = {100,100,100,100,100,100,100,100,};
    static HI_U32  u32MeanQpInfo[8] ={30,30,30,30,30,30,30,30};
    HI_U32  u32PicWidth;
    HI_U32  u32PicHeight;
    HI_U32  u32Index = 0;

    if(HI_TRUE != g_astScenePipeParam[u8Index].stModuleState.bDynamicIsoVenc)
    {
       // printf("HI_SCENE_SetRCParam_AutoGenerate will not use scencauto !\n");
    	return HI_SUCCESS;
    }

    s32Ret = HI_MPI_VENC_GetChnAttr(ViPipe, &stVencChnAttr);
    CHECK_SCENE_RET(s32Ret);


    if(stVencChnAttr.stRcAttr.enRcMode != VENC_RC_MODE_H264AVBR && stVencChnAttr.stRcAttr.enRcMode != VENC_RC_MODE_H265AVBR)
    {
        return HI_SUCCESS;
    }

    s32Ret = HI_MPI_ISP_QueryExposureInfo(ViPipe, &stIspExpInfo);
    CHECK_SCENE_RET(s32Ret);

    u32Iso = stIspExpInfo.u32ISO;

    s32Ret = HI_MPI_VENC_QueryStatus(ViPipe,&stStatEx);
    CHECK_SCENE_RET(s32Ret);

    enType  = stVencChnAttr.stVencAttr.enType;
    //u32StartQp = stStatEx.stVencStrmInfo.u32StartQp;
    u32MeanQp = stStatEx.stVencStrmInfo.u32MeanQp;
    u32MinQP = u32MeanQpInfo[0];
    u32MaxBitRate = (enType == PT_H264)?stVencChnAttr.stRcAttr.stH264AVbr.u32MaxBitRate : stVencChnAttr.stRcAttr.stH265AVbr.u32MaxBitRate;
    u32PicWidth = (enType == PT_H264)?stVencChnAttr.stVencAttr.u32PicWidth  : stVencChnAttr.stVencAttr.u32PicWidth;
    u32PicHeight = (enType == PT_H264)?stVencChnAttr.stVencAttr.u32PicHeight  : stVencChnAttr.stVencAttr.u32PicHeight;
    //printf("--1-- u32Iso = %d,u32MeanQp = %d,enType = %d\n",u32Iso,u32MeanQp,enType);

    for (i = 0; i < 7; i++)
    {
	 u32IsoInfo[i]= u32IsoInfo[i+1];
	 u32MeanQpInfo[i]=u32MeanQpInfo[i+1];
    }
    u32IsoInfo[i]= u32Iso;
    u32MeanQpInfo[i]=u32MeanQp;

    for (i=0; i< 8; i++)
    {
        if(u32MinQP < u32MeanQpInfo[i]) u32MinQP = u32MeanQpInfo[i];
        u32SumMeanQP += u32MeanQpInfo[i];
	    u32SumIso += u32IsoInfo[i];
    }
    u32SumMeanQP = u32SumMeanQP-u32MinQP;
    u32SumMeanQP = u32SumMeanQP/7;
    u32SumIso = u32SumIso/8;

    for(i =0;i < g_astScenePipeParam[u8Index].stDynamicVencBitrate.u32IsoThreshCnt;i++)
    {
         if( u32SumIso > g_astScenePipeParam[u8Index].stDynamicVencBitrate.au32IsoThreshLtoH[i]) u32Index++;
    }

    s32Ret= HI_MPI_VENC_GetRcParam(ViPipe, &stRcParam);
    CHECK_SCENE_RET(s32Ret);
    if(enType == PT_H264)
    {
        stRcParam.stParamH264AVbr.s32ChangePos =  g_astScenePipeParam[u8Index].stDynamicVencBitrate.au16ManualPercent[u32Index -1];
        s32Percent = stRcParam.stParamH264AVbr.s32ChangePos;
    }
    if(enType == PT_H265)
    {
        stRcParam.stParamH265AVbr.s32ChangePos =  g_astScenePipeParam[u8Index].stDynamicVencBitrate.au16ManualPercent[u32Index -1];
        s32Percent = stRcParam.stParamH265AVbr.s32ChangePos;
    }
//   printf("--2-- u32Iso = %d,u32SumMeanQP = %d,enType = %d,au32IsoThreshLtoH = %d\n",u32Iso,u32SumMeanQP,enType,g_astScenePipeParam[u8Index].stDynamicVencBitrate.au16ManualStrength[2]);
    if(u32SumIso >  g_astScenePipeParam[u8Index].stDynamicVencBitrate.au32IsoThreshLtoH[2])
    {
          //printf("--2-- u32Iso = %d,u32SumMeanQP = %d,enType = %d,s32ChangePos = %d\n",u32Iso,u32SumMeanQP,enType,stRcParam.stParamH265AVbr.s32ChangePos);

          if(u32SumMeanQP <= 33 && u32SumMeanQP >= 31)
          {
         	  s32Percent = 19 * s32Percent/20;
          }
          else if(u32SumMeanQP <= 30 && u32SumMeanQP >= 29)
   	      {
   	          s32Percent = 9 * s32Percent/10;
   	      }
	      else if(u32SumMeanQP < 29 )
   	      {
   	          s32Percent = 8 * s32Percent/10;
   	      }

          if(enType == PT_H264)
          {
               stRcParam.stParamH264AVbr.s32ChangePos = s32Percent;
	           stRcParam.stParamH264AVbr.u32MinQpDelta = 2;
          }
	      if(enType == PT_H265)
          {
               stRcParam.stParamH265AVbr.s32ChangePos = s32Percent;
	           stRcParam.stParamH265AVbr.u32MinQpDelta = 2;
          }
	   //printf("--4-- u32Iso = %d,s32ChangePos = %d,u32MinQpDelta = %d,s32MinStillPercent = %d\n",u32SumIso,stRcParam.stParamH265AVbr.s32ChangePos,stRcParam.stParamH265AVbr.u32MinQpDelta,stRcParam.stParamH265AVbr.s32MinStillPercent);


    }
    else
    {

	  //printf("--3-- u32Iso = %d,s32ChangePos = %d,u32MinQpDelta = %d,s32MinStillPercent = %d\n",u32SumIso,stRcParam.stParamH265AVbr.s32ChangePos,stRcParam.stParamH265AVbr.u32MinQpDelta,stRcParam.stParamH265AVbr.s32MinStillPercent);
	   if(stVencChnAttr.stGopAttr.enGopMode == VENC_GOPMODE_SMARTP)
      	   {
	      	   if(enType == PT_H264)
	          {
	               //stRcParam.stParamH264AVbr.s32ChangePos = s32Percent;
		        stRcParam.stParamH264AVbr.u32MinQpDelta = 3;
	          }
		      if(enType == PT_H265)
	          {
	               //stRcParam.stParamH265AVbr.s32ChangePos = s32Percent;
		        stRcParam.stParamH265AVbr.u32MinQpDelta = 3;
	          }
          }
	   else
	   {
		      if(enType == PT_H264)
	          {
	              // stRcParam.stParamH264AVbr.s32ChangePos = s32Percent;
		        stRcParam.stParamH264AVbr.u32MinQpDelta = 3;
	          }
		      if(enType == PT_H265)
	          {
	               //sstRcParam.stParamH265AVbr.s32ChangePos = s32Percent;
		        stRcParam.stParamH265AVbr.u32MinQpDelta = 3;
	          }
	   }
    }

    s32Ret = HI_SCENE_SetQP_AutoGenerate(u32PicWidth,u32PicHeight,u32MaxBitRate, enType, &stRcParam);
    CHECK_SCENE_RET(s32Ret);

    s32Ret= HI_MPI_VENC_SetRcParam(ViPipe, &stRcParam);
    CHECK_SCENE_RET(s32Ret);

    return HI_SUCCESS;
}



#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
