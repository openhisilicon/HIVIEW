#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <sys/prctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <math.h>

#include "hi_scene_inner.h"
#include "hi_scenecomm.h"
#include "hi_scene_setparam.h"
#include "mpi_vpss.h"
#include "mpi_awb.h"
#include "mpi_ae.h"
#include "mpi_isp.h"
#include "mpi_venc.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

/**notes scene mode*/
static HI_SCENE_MODE_S s_stSceneMode;

/**notes scene state*/
static SCENE_STATE_S s_stSceneState;

/**scene lock*/
pthread_mutex_t g_mScene_lock = PTHREAD_MUTEX_INITIALIZER;

#define MAX2(a, b)    ((a) > (b) ? (a) : (b))
#define ABS(a,b) ((a >= b) ? (a-b) : (b-a))

/**use to check if the module init*/
#define SCENE_CHECK_INIT()\
    do{\
        pthread_mutex_lock(&g_mScene_lock);\
        if (HI_FALSE == s_stSceneState.bSceneInit)\
        {\
            MLOGE("func:%s,line:%d, please init sceneauto first!\n",__FUNCTION__,__LINE__);\
            pthread_mutex_unlock(&g_mScene_lock);\
            return HI_SCENE_ENOTINIT;\
        }\
        pthread_mutex_unlock(&g_mScene_lock);\
    }while(0);

/**use to check if scene was pause*/
#define SCENE_CHECK_PAUSE()\
    do{\
        if(HI_TRUE == s_stSceneState.bPause)\
        {\
            return HI_SUCCESS;\
        }\
    }while(0);

/**-------------------------internal function interface------------------------- */

static HI_S32 SCENE_SetDynamicParam_byParam(SCENE_SET_DYNAMIC_BYPARAM_CALLBACK_FN_PTR pFuncParam, SCENE_THREAD_TYPE_E enThreadType, SCENE_DYNAMIC_CHANGE_TYPE_E enDynamicChangeType)
{
    HI_S32 i, j = 0;
    HI_S32 s32Ret = HI_SUCCESS;

    for (i = 0; i < s_stSceneState.u32MainPipeNum; i++)
    {
        for (j = 0; j < s_stSceneState.astMainPipe[i].u32SubPipeNum; j++)
        {
            if (SCENE_DYNAMIC_CHANGE_TYPE_EXPOSURE ==  enDynamicChangeType)
            {
                HI_U64 u64Exposure = s_stSceneState.astMainPipe[i].u64Exposure;
                HI_U64 u64LastExposure = 0;

                switch (enThreadType)
                {
                    case SCENE_THREAD_TYPE_NORMAL:
                        u64LastExposure = s_stSceneState.astMainPipe[i].u64LastNormalExposure;
                        break;
                    case SCENE_THREAD_TYPE_LUMINANCE:
                        u64LastExposure = s_stSceneState.astMainPipe[i].u64LastLuminanceExposure;
                        break;
                    case SCENE_THREAD_TYPE_NOTLINEAR:
                        u64LastExposure = s_stSceneState.astMainPipe[i].u64LastNotLinearExposure;
                        break;
                    default:
                        MLOGE("Error ThreadType");
                        break;
                }

                HI_S32 s32Index = s_stSceneState.astMainPipe[i].aSubPipeHdl[j];
                HI_U8 u8PipeIndex = s_stSceneMode.astPipeAttr[s32Index].u8PipeParamIndex;

                s32Ret = pFuncParam(s32Index, u64Exposure, u64LastExposure, u8PipeIndex);
                HI_SCENECOMM_CHECK_RETURN(s32Ret, HI_SCENE_EINTER);
            }
            else
            {
                HI_U32 u32Iso = s_stSceneState.astMainPipe[i].u32Iso;
                HI_U32 u32LastIso = 0;

                switch (enThreadType)
                {
                    case SCENE_THREAD_TYPE_NORMAL:
                        u32LastIso = s_stSceneState.astMainPipe[i].u32LastNormalIso;
                        break;
                    case SCENE_THREAD_TYPE_LUMINANCE:
                        u32LastIso = s_stSceneState.astMainPipe[i].u32LastLuminanceISO;
                        break;
                    case SCENE_THREAD_TYPE_NOTLINEAR:
                        u32LastIso = s_stSceneState.astMainPipe[i].u32LastNotLinearISO;
                        break;
                    default:
                        MLOGE("Error ThreadType");
                        break;
                }

                HI_S32 s32Index = s_stSceneState.astMainPipe[i].aSubPipeHdl[j];
                HI_U8 u8PipeIndex = s_stSceneMode.astPipeAttr[s32Index].u8PipeParamIndex;

                s32Ret = pFuncParam(s32Index, u32Iso, u32LastIso, u8PipeIndex);
                HI_SCENECOMM_CHECK_RETURN(s32Ret, HI_SCENE_EINTER);
            }
        }
    }
    return HI_SUCCESS;
}



static HI_S32 SCENE_SetMainPipeState(const HI_SCENE_MODE_S *pstSceneMode)
{
    HI_S32 i, j = 0;
    HI_U32 u32MainPipeCnt = 0;
    /**if not use, set to 0*/
    memset(s_stSceneState.astMainPipe, 0, sizeof(SCENE_MAINPIPE_STATE_S) * HI_SCENE_PIPE_MAX_NUM);

    for (i = 0; i < HI_SCENE_PIPE_MAX_NUM; i++)
    {
        s_stSceneState.astMainPipe[i].bDISEnable = HI_FALSE;
        s_stSceneState.astMainPipe[i].bLongExp = HI_FALSE;
        s_stSceneState.astMainPipe[i].bMetryFixed = HI_FALSE;
    }

    /**get mainpipe array*/
    for (i = 0; i < HI_SCENE_PIPE_MAX_NUM; i++)
    {
        if (HI_TRUE != s_stSceneMode.astPipeAttr[i].bEnable)
        {
            continue;
        }

        if (0 == u32MainPipeCnt)
        {
            s_stSceneState.astMainPipe[u32MainPipeCnt].MainPipeHdl = s_stSceneMode.astPipeAttr[i].MainPipeHdl;
            u32MainPipeCnt++;
            continue;
        }

        for (j = 0; j < u32MainPipeCnt; j++)
        {
            if (s_stSceneState.astMainPipe[j].MainPipeHdl == s_stSceneMode.astPipeAttr[i].MainPipeHdl)
            {
                break;
            }
        }

        if (u32MainPipeCnt == j)
        {
            s_stSceneState.astMainPipe[u32MainPipeCnt].MainPipeHdl = s_stSceneMode.astPipeAttr[i].MainPipeHdl;
            u32MainPipeCnt++;
        }
    }

    /**set subpipe in certain mainpipe*/
    for (i = 0; i < u32MainPipeCnt; i++)
    {
        HI_U32 u32SubPipeCnt = 0;

        for (j = 0; j < HI_SCENE_PIPE_MAX_NUM; j++)
        {
            if (HI_TRUE != s_stSceneMode.astPipeAttr[j].bEnable)
            {
                continue;
            }

            if (s_stSceneState.astMainPipe[i].MainPipeHdl == s_stSceneMode.astPipeAttr[j].MainPipeHdl)
            {
                s_stSceneState.astMainPipe[i].aSubPipeHdl[u32SubPipeCnt] = s_stSceneMode.astPipeAttr[j].VcapPipeHdl;
                u32SubPipeCnt++;
            }
        }

        s_stSceneState.astMainPipe[i].u32SubPipeNum = u32SubPipeCnt;
    }

    s_stSceneState.u32MainPipeNum = u32MainPipeCnt;

    for (i = 0; i < s_stSceneState.u32MainPipeNum; i++)
    {
        MLOGD("The mainpipe is %d.", s_stSceneState.astMainPipe[i].MainPipeHdl);
        for (j = 0; j < s_stSceneState.astMainPipe[i].u32SubPipeNum; j++)
        {
            MLOGD("The subpipe in mainpipe %d is %d.", s_stSceneState.astMainPipe[i].MainPipeHdl,
                  s_stSceneState.astMainPipe[i].aSubPipeHdl[j])

        }
        MLOGD("\n");
    }
    return HI_SUCCESS;
}

static HI_S32 SCENE_CalculateExp(VI_PIPE ViPipe, HI_U32 *pu32Iso, HI_U64 *pu64Exposure)
{
    HI_S32 s32Ret = HI_SUCCESS;
    HI_U64 u64SysGainValue = 0;

    ISP_EXP_INFO_S stIspExpInfo;
    ISP_PUB_ATTR_S stPubAttr;

    SCENE_CHECK_PAUSE();

    s32Ret = HI_MPI_ISP_QueryExposureInfo(ViPipe, &stIspExpInfo);
    HI_SCENECOMM_CHECK_RETURN(s32Ret, HI_SCENE_EINTER);

    s32Ret = HI_MPI_ISP_GetPubAttr(ViPipe, &stPubAttr);
    HI_SCENECOMM_CHECK_RETURN(s32Ret, HI_SCENE_EINTER);

    u64SysGainValue = (HI_U64)stIspExpInfo.u32AGain * (HI_U64)stIspExpInfo.u32DGain * (HI_U64)stIspExpInfo.u32ISPDGain;

    *pu32Iso = (u64SysGainValue * 100) >> 30;

    if (WDR_MODE_4To1_LINE == stPubAttr.enWDRMode)
    {
        *pu64Exposure = (u64SysGainValue * (HI_U64)stIspExpInfo.u32LongExpTime) >> 30;
    }
    else if (WDR_MODE_3To1_LINE == stPubAttr.enWDRMode)
    {
        *pu64Exposure = (u64SysGainValue * (HI_U64)stIspExpInfo.u32MedianExpTime) >> 30;
    }
    else if (WDR_MODE_2To1_LINE == stPubAttr.enWDRMode)
    {
        *pu64Exposure = (u64SysGainValue * (HI_U64)stIspExpInfo.u32ShortExpTime) >> 30;
    }
    else
    {
        *pu64Exposure = (u64SysGainValue * (HI_U64)stIspExpInfo.u32ExpTime) >> 30;
    }

    return HI_SUCCESS;
}

static HI_S32 SCENE_CalculateWdrParam(VI_PIPE ViPipe, HI_U32 *pu32ActRation, HI_U32 *pu32HDRBrightRatio)
{
    HI_S32 s32Ret = HI_SUCCESS;

    ISP_INNER_STATE_INFO_S stInnerStateInfo;

    SCENE_CHECK_PAUSE();

    s32Ret = HI_MPI_ISP_QueryInnerStateInfo(ViPipe, &stInnerStateInfo);
    HI_SCENECOMM_CHECK_RETURN(s32Ret, HI_SCENE_EINTER);

    *pu32ActRation = stInnerStateInfo.u32WDRExpRatioActual[0];

    return HI_SUCCESS;
}

static HI_S32 SCENE_SetMainPipeSpecialParam(VI_PIPE ViPipe, HI_BOOL bMetryFixed)
{
    HI_S32 s32Ret = HI_SUCCESS;

    s32Ret = HI_SCENE_SetStaticAE_AutoGenerate(ViPipe, s_stSceneMode.astPipeAttr[ViPipe].u8PipeParamIndex);
    HI_SCENECOMM_CHECK_RETURN(s32Ret, HI_SCENE_EINTER);

    s32Ret = HI_SCENE_SetStaticAWB_AutoGenerate(ViPipe, s_stSceneMode.astPipeAttr[ViPipe].u8PipeParamIndex);
    HI_SCENECOMM_CHECK_RETURN(s32Ret, HI_SCENE_EINTER);

    s32Ret = HI_SCENE_SetStaticAWBEX_AutoGenerate(ViPipe, s_stSceneMode.astPipeAttr[ViPipe].u8PipeParamIndex);
    HI_SCENECOMM_CHECK_RETURN(s32Ret, HI_SCENE_EINTER);

    s32Ret = HI_SCENE_SetStaticAERouteEX_AutoGenerate(ViPipe, s_stSceneMode.astPipeAttr[ViPipe].u8PipeParamIndex);
    HI_SCENECOMM_CHECK_RETURN(s32Ret, HI_SCENE_EINTER);

    s32Ret = HI_SCENE_SetStaticSaturation_AutoGenerate(ViPipe, s_stSceneMode.astPipeAttr[ViPipe].u8PipeParamIndex);
    HI_SCENECOMM_CHECK_RETURN(s32Ret, HI_SCENE_EINTER);

    s32Ret = HI_SCENE_SetStaticWDRExposure_AutoGenerate(ViPipe, s_stSceneMode.astPipeAttr[ViPipe].u8PipeParamIndex);
    HI_SCENECOMM_CHECK_RETURN(s32Ret, HI_SCENE_EINTER);

    s32Ret = HI_SCENE_SetStaticDRC_Autogenerate(ViPipe, s_stSceneMode.astPipeAttr[ViPipe].u8PipeParamIndex);
    HI_SCENECOMM_CHECK_RETURN(s32Ret, HI_SCENE_EINTER);

    s32Ret = HI_SCENE_SetStaticStatisticsCfg_AutoGenerate(ViPipe, s_stSceneMode.astPipeAttr[ViPipe].u8PipeParamIndex, bMetryFixed);
    HI_SCENECOMM_CHECK_RETURN(s32Ret, HI_SCENE_EINTER);

    s32Ret = HI_SCENE_SetStaticShading_AutoGenerate(ViPipe, s_stSceneMode.astPipeAttr[ViPipe].u8PipeParamIndex);
    HI_SCENECOMM_CHECK_RETURN(s32Ret, HI_SCENE_EINTER);

    return HI_SUCCESS;
}

static HI_S32 SCENE_SetPipeStaticParam(HI_VOID)
{
    HI_S32 i = 0;
    HI_S32 s32Ret = HI_SUCCESS;

    for (i = 0; i < HI_SCENE_PIPE_MAX_NUM; i++)
    {
        if (HI_TRUE != s_stSceneMode.astPipeAttr[i].bEnable)
        {
            continue;
        }

        s32Ret = HI_SCENE_SetStaticDeHaze_AutoGenerate(s_stSceneMode.astPipeAttr[i].VcapPipeHdl, s_stSceneMode.astPipeAttr[i].u8PipeParamIndex);
        HI_SCENECOMM_CHECK_RETURN(s32Ret, HI_SCENE_EINTER);
    }

    for (i = 0; i < HI_SCENE_PIPE_MAX_NUM; i++)
    {
        if (HI_TRUE != s_stSceneMode.astPipeAttr[i].bEnable)
        {
            continue;
        }

        s32Ret = HI_SCENE_SetStaticCCM_AutoGenerate(s_stSceneMode.astPipeAttr[i].VcapPipeHdl, s_stSceneMode.astPipeAttr[i].u8PipeParamIndex);
        HI_SCENECOMM_CHECK_RETURN(s32Ret, HI_SCENE_EINTER);
    }

    for (i = 0; i < HI_SCENE_PIPE_MAX_NUM; i++)
    {
        if (HI_TRUE != s_stSceneMode.astPipeAttr[i].bEnable)
        {
            continue;
        }

        s32Ret = HI_SCENE_SetStaticLDCI_AutoGenerate(s_stSceneMode.astPipeAttr[i].VcapPipeHdl, s_stSceneMode.astPipeAttr[i].u8PipeParamIndex);
        HI_SCENECOMM_CHECK_RETURN(s32Ret, HI_SCENE_EINTER);
    }

    for (i = 0; i < HI_SCENE_PIPE_MAX_NUM; i++)
    {
        if (HI_TRUE != s_stSceneMode.astPipeAttr[i].bEnable)
        {
            continue;
        }

        s32Ret = HI_SCENE_SetStaticGlobalCac_AutoGenerate(s_stSceneMode.astPipeAttr[i].VcapPipeHdl, s_stSceneMode.astPipeAttr[i].u8PipeParamIndex);
        HI_SCENECOMM_CHECK_RETURN(s32Ret, HI_SCENE_EINTER);
    }

    for (i = 0; i < HI_SCENE_PIPE_MAX_NUM; i++)
    {
        if (HI_TRUE != s_stSceneMode.astPipeAttr[i].bEnable)
        {
            continue;
        }

        s32Ret = HI_SCENE_SetStaticDemosaic_AutoGenerate(s_stSceneMode.astPipeAttr[i].VcapPipeHdl, s_stSceneMode.astPipeAttr[i].u8PipeParamIndex);
        HI_SCENECOMM_CHECK_RETURN(s32Ret, HI_SCENE_EINTER);
    }

    for (i = 0; i < HI_SCENE_PIPE_MAX_NUM; i++)
    {
        if (HI_TRUE != s_stSceneMode.astPipeAttr[i].bEnable)
        {
            continue;
        }

        s32Ret = HI_SCENE_SetStaticSharpen_AutoGenerate(s_stSceneMode.astPipeAttr[i].VcapPipeHdl, s_stSceneMode.astPipeAttr[i].u8PipeParamIndex);
        HI_SCENECOMM_CHECK_RETURN(s32Ret, HI_SCENE_EINTER);
    }

    for (i = 0; i < HI_SCENE_PIPE_MAX_NUM; i++)
    {
        if (HI_TRUE != s_stSceneMode.astPipeAttr[i].bEnable)
        {
            continue;
        }

        s32Ret = HI_SCENE_SetStaticDetail_AutoGenerate(s_stSceneMode.astPipeAttr[i].VcapPipeHdl, s_stSceneMode.astPipeAttr[i].u8PipeParamIndex);
        HI_SCENECOMM_CHECK_RETURN(s32Ret, HI_SCENE_EINTER);
    }

    return HI_SUCCESS;
}

static HI_S32 SCENE_SetPipeDynamicParam(HI_VOID)
{
    HI_S32 i, j = 0;
    HI_S32 s32Ret = HI_SUCCESS;

    SCENE_SET_DYNAMIC_BYPARAM_CALLBACK_FN_PTR pFuncShading = HI_SCENE_SetDynamicShading_AutoGenerate;
    s32Ret = SCENE_SetDynamicParam_byParam(pFuncShading, SCENE_THREAD_TYPE_NORMAL, SCENE_DYNAMIC_CHANGE_TYPE_EXPOSURE);
    HI_SCENECOMM_CHECK_RETURN(s32Ret, HI_SCENE_EINTER);

    SCENE_SET_DYNAMIC_BYPARAM_CALLBACK_FN_PTR pFuncLdci = HI_SCENE_SetDynamicLDCI_AutoGenerate;
    s32Ret = SCENE_SetDynamicParam_byParam(pFuncLdci, SCENE_THREAD_TYPE_NORMAL, SCENE_DYNAMIC_CHANGE_TYPE_EXPOSURE);
    HI_SCENECOMM_CHECK_RETURN(s32Ret, HI_SCENE_EINTER);

    SCENE_SET_DYNAMIC_BYPARAM_CALLBACK_FN_PTR pFuncDehaze = HI_SCENE_SetDynamicDehaze_AutoGenerate;
    s32Ret = SCENE_SetDynamicParam_byParam(pFuncDehaze, SCENE_THREAD_TYPE_NORMAL, SCENE_DYNAMIC_CHANGE_TYPE_EXPOSURE);
    HI_SCENECOMM_CHECK_RETURN(s32Ret, HI_SCENE_EINTER);
    SCENE_SET_DYNAMIC_BYPARAM_CALLBACK_FN_PTR pFuncVencBitrate = HI_SCENE_SetDynamicVencBitrate_AutoGenerate;
    s32Ret = SCENE_SetDynamicParam_byParam(pFuncVencBitrate, SCENE_THREAD_TYPE_NORMAL, SCENE_DYNAMIC_CHANGE_TYPE_ISO);
    HI_SCENECOMM_CHECK_RETURN(s32Ret, HI_SCENE_EINTER); 

    for (i = 0; i < s_stSceneState.u32MainPipeNum; i++)
    {
        for (j = 0; j < s_stSceneState.astMainPipe[i].u32SubPipeNum; j++)
        {
            HI_U32 u32Iso = 0;
            HI_S32 s32Index = s_stSceneState.astMainPipe[i].aSubPipeHdl[j];
            HI_U8 u8PipeIndex = s_stSceneMode.astPipeAttr[s32Index].u8PipeParamIndex;
            VI_CHN ViChn = s_stSceneMode.astPipeAttr[s32Index].PipeChnHdl;

            if (HI_SCENE_PIPE_TYPE_VIDEO != s_stSceneMode.astPipeAttr[s32Index].enPipeType
                || HI_TRUE != s_stSceneState.astMainPipe[i].bDISEnable)
            {
                continue;
            }

            u32Iso = s_stSceneState.astMainPipe[i].u32Iso;

            s32Ret = HI_SCENE_SetDynamicDIS_AutoGenerate(s32Index, ViChn, u32Iso, s_stSceneState.astMainPipe[i].u32LastNormalIso, u8PipeIndex, s_stSceneState.astMainPipe[i].bDISEnable);
            HI_SCENECOMM_CHECK_RETURN(s32Ret, HI_SCENE_EINTER);
        }
    }

    for (i = 0; i < s_stSceneState.u32MainPipeNum; i++)
    {
        for (j = 0; j < s_stSceneState.astMainPipe[i].u32SubPipeNum; j++)
        {
            HI_U32 u32Iso = 0;
            HI_S32 s32Index = s_stSceneState.astMainPipe[i].aSubPipeHdl[j];
            HI_U8 u8PipeIndex = s_stSceneMode.astPipeAttr[s32Index].u8PipeParamIndex;
            HI_U8 u8VpssGrpID = s_stSceneMode.astPipeAttr[s32Index].VpssHdl;
            u32Iso = s_stSceneState.astMainPipe[i].u32Iso;

            s32Ret = HI_SCENE_SetDynamicThreeDNR_AutoGenerate(s32Index, u8VpssGrpID, u32Iso, s_stSceneState.astMainPipe[i].u32LastLuminanceISO, u8PipeIndex);
            HI_SCENECOMM_CHECK_RETURN(s32Ret, HI_SCENE_EINTER);
        }
    }

    for (i = 0; i < s_stSceneState.u32MainPipeNum; i++)
    {
        for (j = 0; j < s_stSceneState.astMainPipe[i].u32SubPipeNum; j++)
        {
            HI_U32 u32Iso = 0;
            HI_S32 s32Index = s_stSceneState.astMainPipe[i].aSubPipeHdl[j];
            HI_U8 u8PipeIndex = s_stSceneMode.astPipeAttr[s32Index].u8PipeParamIndex;
            u32Iso = s_stSceneState.astMainPipe[i].u32Iso;

            s32Ret = HI_SCENE_SetDynamicBayernr_AutoGenerate(s32Index, u32Iso, s_stSceneState.astMainPipe[i].u32LastLuminanceISO, u8PipeIndex);
            HI_SCENECOMM_CHECK_RETURN(s32Ret, HI_SCENE_EINTER);
        }
    }

    for (i = 0; i < s_stSceneState.u32MainPipeNum; i++)
    {
        for (j = 0; j < s_stSceneState.astMainPipe[i].u32SubPipeNum; j++)
        {
            HI_U32 u32Iso = 0;
            HI_S32 s32Index = s_stSceneState.astMainPipe[i].aSubPipeHdl[j];
            HI_U8 u8PipeIndex = s_stSceneMode.astPipeAttr[s32Index].u8PipeParamIndex;
            u32Iso = s_stSceneState.astMainPipe[i].u32Iso;

            s32Ret = HI_SCENE_SetDynamicLinearDRC_AutoGenerate(s32Index, u32Iso, s_stSceneState.astMainPipe[i].u32LastLuminanceISO, u8PipeIndex);
            HI_SCENECOMM_CHECK_RETURN(s32Ret, HI_SCENE_EINTER);
        }
    }

    for (i = 0; i < s_stSceneState.u32MainPipeNum; i++)
    {
        s_stSceneState.astMainPipe[i].u32LastLuminanceISO = s_stSceneState.astMainPipe[i].u32Iso;
    }

    return HI_SUCCESS;
}

static HI_S32 SCENE_SetVIPipeParam(HI_VOID)
{
    HI_S32 i = 0;
    HI_S32 s32Ret = HI_SUCCESS;

    /*set mainIsp param*/
    for (i = 0; i < s_stSceneState.u32MainPipeNum ; i++)
    {
        VI_PIPE ViPipe = s_stSceneState.astMainPipe[i].MainPipeHdl;
        HI_BOOL bMetryFixed;

        bMetryFixed = s_stSceneState.astMainPipe[i].bMetryFixed;

        s32Ret = SCENE_SetMainPipeSpecialParam(ViPipe, bMetryFixed);
        HI_SCENECOMM_CHECK_RETURN(s32Ret, HI_SCENE_EINTER);

        s32Ret = SCENE_CalculateExp(ViPipe, &(s_stSceneState.astMainPipe[i].u32Iso), &(s_stSceneState.astMainPipe[i].u64Exposure));
        HI_SCENECOMM_CHECK_RETURN(s32Ret, HI_SCENE_EINTER);
    }

    s32Ret = SCENE_SetPipeStaticParam();
    HI_SCENECOMM_CHECK_RETURN(s32Ret, HI_SCENE_EINTER);

#if 0
    s32Ret = SCENE_SetPipeDynamicParam();
    HI_SCENECOMM_CHECK_RETURN(s32Ret, HI_SCENE_EINTER);
#endif

    return HI_SUCCESS;
}

static HI_S32 SCENE_SetVPSSParam(HI_VOID)
{
    // to do
    return HI_SUCCESS;
}

static HI_S32 SCENE_SetVENCParam(HI_VOID)
{
    // to do
    return HI_SUCCESS;
}

HI_VOID *SCENE_NormalAutoThread(HI_VOID *pVoid)
{
    HI_S32 s32Ret = HI_SUCCESS;
    HI_S32 i = 0;

    prctl(PR_SET_NAME, (unsigned long)"HI_SCENE_NormalThread", 0, 0, 0);

    while (HI_TRUE == s_stSceneState.stThreadNormal.bThreadFlag)
    {

        for (i = 0; i < s_stSceneState.u32MainPipeNum; i++)
        {
            VI_PIPE ViPipe = s_stSceneState.astMainPipe[i].MainPipeHdl;

            s32Ret = SCENE_CalculateExp(ViPipe, &(s_stSceneState.astMainPipe[i].u32Iso), &(s_stSceneState.astMainPipe[i].u64Exposure));
            HI_SCENECOMM_CHECK(s32Ret, HI_SCENE_EINTER);
        }

        s32Ret = SCENE_SetPipeDynamicParam();
        HI_SCENECOMM_CHECK(s32Ret, HI_SCENE_EINTER);

        for (i = 0; i < s_stSceneState.u32MainPipeNum; i++)
        {
            s_stSceneState.astMainPipe[i].u64LastNormalExposure = s_stSceneState.astMainPipe[i].u64Exposure;
            s_stSceneState.astMainPipe[i].u32LastNormalIso = s_stSceneState.astMainPipe[i].u32Iso;
        }

        usleep(300000);
    }

    return HI_NULL;
}

HI_VOID *SCENE_LuminanceAutoThread(HI_VOID *pVoid)
{
    HI_S32 s32Ret = HI_SUCCESS;
    HI_U32 i, j = 0;

    prctl(PR_SET_NAME, (unsigned long)"HI_SCENE_LuminanceThread", 0, 0, 0);

    while (HI_TRUE == s_stSceneState.stThreadLuminance.bThreadFlag)
    {
        /*set AE compesation with exposure calculated by effective ISP dev*/
        for (i = 0; i < s_stSceneState.u32MainPipeNum ; i++)
        {
            VI_PIPE ViPipe = s_stSceneState.astMainPipe[i].MainPipeHdl;

            s32Ret = SCENE_CalculateExp(ViPipe, &(s_stSceneState.astMainPipe[i].u32Iso), &(s_stSceneState.astMainPipe[i].u64Exposure));
            HI_SCENECOMM_CHECK(s32Ret, HI_SCENE_EINTER);
        }

        SCENE_SET_DYNAMIC_BYPARAM_CALLBACK_FN_PTR pFuncAE = HI_SCENE_SetDynamicAE_AutoGenerate;
        s32Ret = SCENE_SetDynamicParam_byParam(pFuncAE, SCENE_THREAD_TYPE_LUMINANCE, SCENE_DYNAMIC_CHANGE_TYPE_EXPOSURE);
        HI_SCENECOMM_CHECK(s32Ret, HI_SCENE_EINTER);

        for (i = 0; i < s_stSceneState.u32MainPipeNum; i++)
        {
            for (j = 0; j < s_stSceneState.astMainPipe[i].u32SubPipeNum; j++)
            {
                HI_U64 u64Exposure = s_stSceneState.astMainPipe[i].u64Exposure;
                HI_S32 s32Index = s_stSceneState.astMainPipe[i].aSubPipeHdl[j];
                HI_U8 u8PipeIndex = s_stSceneMode.astPipeAttr[s32Index].u8PipeParamIndex;

                if (HI_SCENE_PIPE_TYPE_SNAP == s_stSceneMode.astPipeAttr[s32Index].enPipeType)
                {
                    s32Ret = HI_SCENE_SetDynamicPhotoGamma_AutoGenerate(s32Index, u64Exposure, s_stSceneState.astMainPipe[i].u64LastLuminanceExposure, u8PipeIndex);
                    HI_SCENECOMM_CHECK(s32Ret, HI_SCENE_EINTER);
                }
                else if (HI_SCENE_PIPE_TYPE_VIDEO == s_stSceneMode.astPipeAttr[s32Index].enPipeType)
                {
                    s32Ret = HI_SCENE_SetDynamicVideoGamma_AutoGenerate(s32Index, u64Exposure, s_stSceneState.astMainPipe[i].u64LastLuminanceExposure, u8PipeIndex);
                    HI_SCENECOMM_CHECK(s32Ret, HI_SCENE_EINTER);
                }
            }
        }

        for (i = 0; i < s_stSceneState.u32MainPipeNum; i++)
        {
            s_stSceneState.astMainPipe[i].u64LastLuminanceExposure = s_stSceneState.astMainPipe[i].u64Exposure;
            s_stSceneState.astMainPipe[i].u32LastLuminanceISO = s_stSceneState.astMainPipe[i].u32Iso;
        }

        usleep(100000);
    }
    return HI_NULL;
}

HI_VOID *SCENE_NotLinearAutoThread(HI_VOID *pVoid)
{
    HI_S32 s32Ret = HI_SUCCESS;
    HI_U32 i = 0;

    prctl(PR_SET_NAME, (unsigned long)"HI_SCENE_NotLinearThread", 0, 0, 0);

    while (HI_TRUE == s_stSceneState.stThreadNotLinear.bThreadFlag)
    {

        if (HI_SCENE_PIPE_MODE_WDR != s_stSceneMode.enPipeMode && HI_SCENE_PIPE_MODE_HDR != s_stSceneMode.enPipeMode)
        {
            usleep(1000000);
            continue;
        }

        for (i = 0; i < s_stSceneState.u32MainPipeNum ; i++)
        {
            VI_PIPE ViPipe = s_stSceneState.astMainPipe[i].MainPipeHdl;

            s32Ret = SCENE_CalculateExp(ViPipe, &(s_stSceneState.astMainPipe[i].u32Iso), &(s_stSceneState.astMainPipe[i].u64Exposure));
            HI_SCENECOMM_CHECK(s32Ret, HI_SCENE_EINTER);

            s32Ret = SCENE_CalculateWdrParam(ViPipe, &(s_stSceneState.astMainPipe[i].u32ActRation), &(s_stSceneState.astMainPipe[i].u32HDRBrightRatio));
            HI_SCENECOMM_CHECK(s32Ret, HI_SCENE_EINTER);
        }

        for (i = 0; i < s_stSceneState.u32MainPipeNum ; i++)
        {
            VI_PIPE ViPipe = s_stSceneState.astMainPipe[i].MainPipeHdl;
            s32Ret = HI_SCENE_SetDynamicFsWdr_AutoGenerate(ViPipe, s_stSceneState.astMainPipe[i].u32Iso, s_stSceneState.astMainPipe[i].u32LastNotLinearISO,
                         s_stSceneMode.astPipeAttr[ViPipe].u8PipeParamIndex, s_stSceneState.astMainPipe[i].u32ActRation);
            HI_SCENECOMM_CHECK(s32Ret, HI_SCENE_EINTER);
        }

        for (i = 0; i < s_stSceneState.u32MainPipeNum ; i++)
        {
            VI_PIPE ViPipe = s_stSceneState.astMainPipe[i].MainPipeHdl;
            s32Ret = HI_SCENE_SetDynamicThreadDRC_AutoGenerate(ViPipe, s_stSceneMode.astPipeAttr[ViPipe].u8PipeParamIndex,
                         s_stSceneState.astMainPipe[i].u32ActRation, s_stSceneState.astMainPipe[i].u32Iso,
                         s_stSceneState.astMainPipe[i].u64Exposure, s_stSceneState.astMainPipe[i].u64LastNotLinearExposure);
            HI_SCENECOMM_CHECK(s32Ret, HI_SCENE_EINTER);
        }

        for (i = 0; i < s_stSceneState.u32MainPipeNum; i++)
        {
            s_stSceneState.astMainPipe[i].u64LastNotLinearExposure = s_stSceneState.astMainPipe[i].u64Exposure;
            s_stSceneState.astMainPipe[i].u32LastNotLinearISO = s_stSceneState.astMainPipe[i].u32Iso;
        }

        usleep(100000);
    }
    return HI_NULL;
}
void *SCENE_VencAutoThread(HI_VOID* pVoid)
{
    HI_S32 s32Ret = HI_SUCCESS;
    HI_S32 i;
	
    prctl(PR_SET_NAME, (unsigned long)"hi_AutoVenc", 0, 0, 0);

	
    while (HI_TRUE == s_stSceneState.stThreadVenc.bThreadFlag)
    {
        //Set VENC RC PARAM
        for (i = 0; i < s_stSceneState.u32MainPipeNum; i++)
        {
            VI_PIPE ViPipe = s_stSceneState.astMainPipe[i].MainPipeHdl;
	        HI_S32 s32Index = s_stSceneMode.astPipeAttr[i].u8PipeParamIndex;

            s32Ret = HI_SCENE_SetRCParam_AutoGenerate(ViPipe,s32Index);
            HI_SCENECOMM_CHECK(s32Ret, HI_SCENE_EINTER);
        }

        usleep(200000);
    }
   return NULL;
}

static HI_S32 SCENE_StartAutoThread(HI_VOID)
{
    HI_S32 s32Ret = HI_SUCCESS;

    //pthread_attr_t threadattr;
    pthread_attr_init(&(s_stSceneState.stThreadnormattr));
    pthread_attr_setdetachstate(&(s_stSceneState.stThreadnormattr), PTHREAD_CREATE_DETACHED);

    pthread_attr_init(&(s_stSceneState.stThreadluminanceattr));
    pthread_attr_setdetachstate(&(s_stSceneState.stThreadluminanceattr), PTHREAD_CREATE_DETACHED);

    pthread_attr_init(&(s_stSceneState.stThreadnotlinearattr));
    pthread_attr_setdetachstate(&(s_stSceneState.stThreadnotlinearattr), PTHREAD_CREATE_DETACHED);
    pthread_attr_init(&(s_stSceneState.stThreadvencattr));
    pthread_attr_setdetachstate(&(s_stSceneState.stThreadvencattr), PTHREAD_CREATE_DETACHED);


    if (HI_FALSE == s_stSceneState.stThreadNormal.bThreadFlag)
    {
        s_stSceneState.stThreadNormal.bThreadFlag = HI_TRUE;
        s32Ret = pthread_create(&s_stSceneState.stThreadNormal.pThread, &(s_stSceneState.stThreadnormattr), SCENE_NormalAutoThread, NULL);
        HI_SCENECOMM_CHECK_RETURN(s32Ret, HI_SCENE_EINTER);
    }

    if (HI_FALSE == s_stSceneState.stThreadLuminance.bThreadFlag)
    {
        s_stSceneState.stThreadLuminance.bThreadFlag = HI_TRUE;
        s32Ret = pthread_create(&s_stSceneState.stThreadLuminance.pThread, &(s_stSceneState.stThreadluminanceattr), SCENE_LuminanceAutoThread, NULL);
        HI_SCENECOMM_CHECK_RETURN(s32Ret, HI_SCENE_EINTER);
    }

    if (HI_FALSE == s_stSceneState.stThreadNotLinear.bThreadFlag && (HI_SCENE_PIPE_MODE_WDR == s_stSceneMode.enPipeMode || HI_SCENE_PIPE_MODE_HDR == s_stSceneMode.enPipeMode))
    {
        s_stSceneState.stThreadNotLinear.bThreadFlag = HI_TRUE;
        s32Ret = pthread_create(&s_stSceneState.stThreadLuminance.pThread, &(s_stSceneState.stThreadnotlinearattr), SCENE_NotLinearAutoThread, NULL);
        HI_SCENECOMM_CHECK_RETURN(s32Ret, HI_SCENE_EINTER);
    }

    if (HI_FALSE == s_stSceneState.stThreadVenc.bThreadFlag)
    {
        s_stSceneState.stThreadVenc.bThreadFlag = HI_TRUE;
        s32Ret = pthread_create(&s_stSceneState.stThreadVenc.pThread, &(s_stSceneState.stThreadvencattr), SCENE_VencAutoThread, NULL);
        HI_SCENECOMM_CHECK_RETURN(s32Ret, HI_SCENE_EINTER);
    }

    return HI_SUCCESS;
}

static HI_S32 SCENE_StopAutoThread(HI_VOID)
{
    if (HI_TRUE == s_stSceneState.stThreadNormal.bThreadFlag)
    {
        s_stSceneState.stThreadNormal.bThreadFlag = HI_FALSE;
        pthread_attr_destroy(&(s_stSceneState.stThreadnormattr));
    }

    if (HI_TRUE == s_stSceneState.stThreadLuminance.bThreadFlag)
    {
        s_stSceneState.stThreadLuminance.bThreadFlag = HI_FALSE;
        pthread_attr_destroy(&(s_stSceneState.stThreadluminanceattr));
    }

    if (HI_TRUE == s_stSceneState.stThreadNotLinear.bThreadFlag)
    {
        s_stSceneState.stThreadNotLinear.bThreadFlag = HI_FALSE;
        pthread_attr_destroy(&(s_stSceneState.stThreadnotlinearattr));
    } 
    if (HI_TRUE == s_stSceneState.stThreadVenc.bThreadFlag)
    {
        s_stSceneState.stThreadVenc.bThreadFlag = HI_FALSE;
        pthread_attr_destroy(&(s_stSceneState.stThreadvencattr));
    }
    return HI_SUCCESS;
}


/**-------------------------external function interface-------------------------*/
HI_S32 HI_SCENE_Init(const HI_SCENE_PARAM_S *pstSceneParam)
{
    HI_SCENECOMM_CHECK_POINTER(pstSceneParam, HI_SCENE_ENONPTR);

    HI_S32 s32Ret = HI_SUCCESS;

    if (HI_TRUE == s_stSceneState.bSceneInit)
    {
        MLOGE("SCENE module has already been inited.\n");
        return HI_SCENE_EINITIALIZED;
    }

    s32Ret = HI_SCENE_SetPipeParam_AutoGenerate(pstSceneParam->astPipeParam, HI_SCENE_PIPETYPE_NUM);
    HI_SCENECOMM_CHECK_RETURN(s32Ret, HI_SCENE_EINTER);

    memset(&s_stSceneMode, 0, sizeof(HI_SCENE_MODE_S));

    memset(&s_stSceneState, 0, sizeof(SCENE_STATE_S));

    s_stSceneState.bSceneInit = HI_TRUE;

    MLOGD("SCENE module has been inited successfully.\n");

    return HI_SUCCESS;
}


HI_S32 HI_SCENE_SetSceneMode(const HI_SCENE_MODE_S *pstSceneMode)
{
    SCENE_CHECK_INIT();
    HI_SCENECOMM_CHECK_POINTER(pstSceneMode, HI_SCENE_ENONPTR);

    HI_S32 i = 0;
    HI_S32 s32Ret = HI_SUCCESS;

    HI_MUTEX_LOCK(g_mScene_lock);

    if (HI_SCENE_PIPE_MODE_LINEAR != pstSceneMode->enPipeMode &&
        HI_SCENE_PIPE_MODE_WDR != pstSceneMode->enPipeMode)
    {
        MLOGE("The pipe mode must be LINEAR, WDR or HDR.\n");
        HI_MUTEX_UNLOCK(g_mScene_lock);
        return HI_SCENE_EINVAL;
    }

    for (i = 0; i < HI_SCENE_PIPE_MAX_NUM; i++)
    {
        if (HI_TRUE != pstSceneMode->astPipeAttr[i].bEnable)
        {
            continue;
        }

        if (i != pstSceneMode->astPipeAttr[i].VcapPipeHdl)
        {
            MLOGE("The value of pipe in scene pipe array must be equal to the index of array!\n");
            HI_MUTEX_UNLOCK(g_mScene_lock);
            return HI_SCENE_EINVAL;
        }

        if (HI_SCENE_PIPE_TYPE_SNAP != pstSceneMode->astPipeAttr[i].enPipeType && HI_SCENE_PIPE_TYPE_VIDEO != pstSceneMode->astPipeAttr[i].enPipeType)
        {
            MLOGE("Pipe Type is not video or snap!\n");
            HI_MUTEX_UNLOCK(g_mScene_lock);
            return HI_SCENE_EOUTOFRANGE;
        }

        if (pstSceneMode->astPipeAttr[i].u8PipeParamIndex < 0 || pstSceneMode->astPipeAttr[i].u8PipeParamIndex >= HI_SCENE_PIPETYPE_NUM)
        {
            MLOGE("Pipe param index is out of range!\n");
            HI_MUTEX_UNLOCK(g_mScene_lock);
            return HI_SCENE_EOUTOFRANGE;
        }
    }

    memcpy(&s_stSceneMode, pstSceneMode, sizeof(HI_SCENE_MODE_S));

    s32Ret = SCENE_SetMainPipeState(pstSceneMode);
    if (HI_SUCCESS != s32Ret)
    {
        MLOGE("SCENE_SetMainIspState failed.\n");
        HI_MUTEX_UNLOCK(g_mScene_lock);
        return HI_FAILURE;
    }

    s32Ret = SCENE_SetVIPipeParam();
    if (HI_SUCCESS != s32Ret)
    {
        MLOGE("SCENE_SetIspParam failed.\n");
        HI_MUTEX_UNLOCK(g_mScene_lock);
        return HI_FAILURE;
    }

    s32Ret = SCENE_SetVPSSParam();
    if (HI_SUCCESS != s32Ret)
    {
        MLOGE("SCENE_SetVPSSParam failed.\n");
        HI_MUTEX_UNLOCK(g_mScene_lock);
        return HI_FAILURE;
    }

    s32Ret = SCENE_SetVENCParam();
    if (HI_SUCCESS != s32Ret)
    {
        MLOGE("SCENE_SetVENCParam failed.\n");
        HI_MUTEX_UNLOCK(g_mScene_lock);
        return HI_FAILURE;
    }

    s32Ret = SCENE_StartAutoThread();
    if (HI_SUCCESS != s32Ret)
    {
        MLOGE("SCENE_StartThread failed.\n");
        HI_MUTEX_UNLOCK(g_mScene_lock);
        return HI_FAILURE;
    }

    HI_MUTEX_UNLOCK(g_mScene_lock);

    return HI_SUCCESS;
}

HI_S32 HI_SCENE_Deinit()
{
    SCENE_CHECK_INIT();

    HI_S32 s32Ret = HI_SUCCESS;

    s32Ret = SCENE_StopAutoThread();
    HI_SCENECOMM_CHECK_RETURN(s32Ret, HI_SCENE_EINTER);

    pthread_mutex_lock(&g_mScene_lock);
    s_stSceneState.bSceneInit = HI_FALSE;
    pthread_mutex_unlock(&g_mScene_lock);

    MLOGD("SCENE Module has been deinited successfully!\n");

    return HI_SUCCESS;
}

HI_S32 HI_SCENE_GetSceneMode(HI_SCENE_MODE_S *pstSceneMode)
{
    SCENE_CHECK_INIT();

    HI_SCENECOMM_CHECK_POINTER(pstSceneMode, HI_SCENE_ENONPTR);

    memcpy(pstSceneMode, &s_stSceneMode, sizeof(HI_SCENE_MODE_S));

    return HI_SUCCESS;
}

HI_S32 HI_SCENE_Pause(HI_BOOL bEnable)
{
    SCENE_CHECK_INIT();

    HI_S32 s32Ret = HI_SUCCESS;

    s_stSceneState.bPause = bEnable;

    s32Ret = HI_SCENE_SetPause_AutoGenerate(bEnable);
    HI_SCENECOMM_CHECK_RETURN(s32Ret, HI_SCENE_EINTER);

    return HI_SUCCESS;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


