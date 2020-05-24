
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <limits.h>

#include "hi_type.h"
#include "hi_scene.h"
#include "hi_scene_loadparam.h"
#include "hi_scenecomm_log.h"
#include "sample_comm.h"
#include "hi_scene_prev.h"
HI_S32 SAMPLE_SCENE_START_PREV(SCENE_PREV_S *pstScenePrev)
{

    HI_S32             s32Ret = HI_SUCCESS;
    HI_S32             s32ViCnt       = 1;
    VI_DEV             ViDev          = 0;
    VI_PIPE            ViPipe         = 0;
    VI_CHN             ViChn          = 0;
    HI_S32             s32WorkSnsId   = 0;

    SIZE_S             stSize;
    VB_CONFIG_S        stVbConf;
    PIC_SIZE_E         enPicSize;
    HI_U32             u32BlkSize;

    WDR_MODE_E         enWDRMode      = WDR_MODE_NONE;
    //WDR_MODE_E         enWDRMode      = WDR_MODE_2To1_LINE;
    DYNAMIC_RANGE_E    enDynamicRange = DYNAMIC_RANGE_SDR8;
    PIXEL_FORMAT_E     enPixFormat    = PIXEL_FORMAT_YVU_SEMIPLANAR_420;
    VIDEO_FORMAT_E     enVideoFormat  = VIDEO_FORMAT_LINEAR;
    COMPRESS_MODE_E    enCompressMode = COMPRESS_MODE_NONE;
    VI_VPSS_MODE_E     enMastPipeMode = VI_ONLINE_VPSS_ONLINE;
    VPSS_GRP_ATTR_S    stVpssGrpAttr;

    VPSS_CHN_ATTR_S    astVpssChnAttr[VPSS_MAX_PHY_CHN_NUM];
    HI_U8 i = 0;

    PAYLOAD_TYPE_E     enType      = PT_H265;
    SAMPLE_RC_E        enRcMode    = SAMPLE_RC_CBR;
    HI_U32             u32Profile  = 0;
    HI_BOOL            bRcnRefShareBuf = HI_TRUE;
    VENC_GOP_ATTR_S    stGopAttr;

    pstScenePrev->VencChn[0] = 0;
    pstScenePrev->VpssGrp    = 0;
    pstScenePrev->VpssChn    = VPSS_CHN0;
    pstScenePrev->VoChn      = 0;

    for(i=0;i<VPSS_MAX_PHY_CHN_NUM;i++)
    {
        pstScenePrev->abChnEnable[i] = 0;
    }

    /*config vi*/
    SAMPLE_COMM_VI_GetSensorInfo(&pstScenePrev->stViConfig);

    pstScenePrev->stViConfig.s32WorkingViNum                                   = s32ViCnt;
    pstScenePrev->stViConfig.as32WorkingViId[0]                                = 0;
    pstScenePrev->stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.MipiDev         = ViDev;
    pstScenePrev->stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.s32BusId        = 0;
    pstScenePrev->stViConfig.astViInfo[s32WorkSnsId].stDevInfo.ViDev           = ViDev;
    pstScenePrev->stViConfig.astViInfo[s32WorkSnsId].stDevInfo.enWDRMode       = enWDRMode;
    pstScenePrev->stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.enMastPipeMode = enMastPipeMode;
    pstScenePrev->stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[0]       = ViPipe;
    pstScenePrev->stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[1]       = -1;
    //pstScenePrev->stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[1]       = ViPipe+1;
    pstScenePrev->stViConfig.astViInfo[s32WorkSnsId].stChnInfo.ViChn           = ViChn;
    pstScenePrev->stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enPixFormat     = enPixFormat;
    pstScenePrev->stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enDynamicRange  = enDynamicRange;
    pstScenePrev->stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enVideoFormat   = enVideoFormat;
    pstScenePrev->stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enCompressMode  = enCompressMode;

    /*get picture size*/
    s32Ret = SAMPLE_COMM_VI_GetSizeBySensor(pstScenePrev->stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.enSnsType, &enPicSize);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("get picture size by sensor failed!\n");
        return s32Ret;
    }

    s32Ret = SAMPLE_COMM_SYS_GetPicSize(enPicSize, &stSize);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("get picture size failed!\n");
        return s32Ret;
    }

    /*config vb*/
    hi_memset(&stVbConf, sizeof(VB_CONFIG_S), 0, sizeof(VB_CONFIG_S));
    stVbConf.u32MaxPoolCnt              = 1;

    u32BlkSize = COMMON_GetPicBufferSize(stSize.u32Width, stSize.u32Height, SAMPLE_PIXEL_FORMAT, DATA_BITWIDTH_8, COMPRESS_MODE_SEG, DEFAULT_ALIGN);
    stVbConf.astCommPool[0].u64BlkSize  = u32BlkSize;
    stVbConf.astCommPool[0].u32BlkCnt   = 3;

    u32BlkSize = COMMON_GetPicBufferSize(720, 576, SAMPLE_PIXEL_FORMAT, DATA_BITWIDTH_8, COMPRESS_MODE_NONE, DEFAULT_ALIGN);
    stVbConf.astCommPool[1].u64BlkSize  = u32BlkSize;
    stVbConf.astCommPool[1].u32BlkCnt   = 5;


    s32Ret = SAMPLE_COMM_SYS_Init(&stVbConf);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("system init failed with %d!\n", s32Ret);
        return s32Ret;
    }

    /*start vi*/
    s32Ret = SAMPLE_COMM_VI_StartVi(&pstScenePrev->stViConfig);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("start vi failed.s32Ret:0x%x !\n", s32Ret);
        goto EXIT;
    }

    /*config vpss*/
    hi_memset(&stVpssGrpAttr, sizeof(VPSS_GRP_ATTR_S), 0, sizeof(VPSS_GRP_ATTR_S));
    stVpssGrpAttr.stFrameRate.s32SrcFrameRate    = -1;
    stVpssGrpAttr.stFrameRate.s32DstFrameRate    = -1;
    stVpssGrpAttr.enDynamicRange                 = DYNAMIC_RANGE_SDR8;
    stVpssGrpAttr.enPixelFormat                  = enPixFormat;
    stVpssGrpAttr.u32MaxW                        = stSize.u32Width;
    stVpssGrpAttr.u32MaxH                        = stSize.u32Height;
    stVpssGrpAttr.bNrEn                          = HI_TRUE;
    stVpssGrpAttr.stNrAttr.enCompressMode        = COMPRESS_MODE_FRAME;
    stVpssGrpAttr.stNrAttr.enNrMotionMode        = NR_MOTION_MODE_NORMAL;

    astVpssChnAttr[pstScenePrev->VpssChn].u32Width                    = stSize.u32Width;
    astVpssChnAttr[pstScenePrev->VpssChn].u32Height                   = stSize.u32Height;
    astVpssChnAttr[pstScenePrev->VpssChn].enChnMode                   = VPSS_CHN_MODE_USER;
    astVpssChnAttr[pstScenePrev->VpssChn].enCompressMode              = COMPRESS_MODE_SEG;
    astVpssChnAttr[pstScenePrev->VpssChn].enDynamicRange              = enDynamicRange;
    astVpssChnAttr[pstScenePrev->VpssChn].enVideoFormat               = enVideoFormat;
    astVpssChnAttr[pstScenePrev->VpssChn].enPixelFormat               = enPixFormat;
    astVpssChnAttr[pstScenePrev->VpssChn].stFrameRate.s32SrcFrameRate = -1;
    astVpssChnAttr[pstScenePrev->VpssChn].stFrameRate.s32DstFrameRate = -1;
    astVpssChnAttr[pstScenePrev->VpssChn].u32Depth                    = 0;
    astVpssChnAttr[pstScenePrev->VpssChn].bMirror                     = HI_FALSE;
    astVpssChnAttr[pstScenePrev->VpssChn].bFlip                       = HI_FALSE;
    astVpssChnAttr[pstScenePrev->VpssChn].stAspectRatio.enMode        = ASPECT_RATIO_NONE;

    astVpssChnAttr[VPSS_CHN1].u32Width                    = 720;
    astVpssChnAttr[VPSS_CHN1].u32Height                   = 576;
    astVpssChnAttr[VPSS_CHN1].enChnMode                   = VPSS_CHN_MODE_USER;
    astVpssChnAttr[VPSS_CHN1].enCompressMode              = enCompressMode;
    astVpssChnAttr[VPSS_CHN1].enDynamicRange              = enDynamicRange;
    astVpssChnAttr[VPSS_CHN1].enVideoFormat               = enVideoFormat;
    astVpssChnAttr[VPSS_CHN1].enPixelFormat               = enPixFormat;
    astVpssChnAttr[VPSS_CHN1].stFrameRate.s32SrcFrameRate = -1;
    astVpssChnAttr[VPSS_CHN1].stFrameRate.s32DstFrameRate = -1;
    astVpssChnAttr[VPSS_CHN1].u32Depth                    = 0;
    astVpssChnAttr[VPSS_CHN1].bMirror                     = HI_FALSE;
    astVpssChnAttr[VPSS_CHN1].bFlip                       = HI_FALSE;
    astVpssChnAttr[VPSS_CHN1].stAspectRatio.enMode        = ASPECT_RATIO_NONE;
    /*start vpss*/
    pstScenePrev->abChnEnable[0] = HI_TRUE;
    pstScenePrev->abChnEnable[1] = HI_TRUE;
    s32Ret = SAMPLE_COMM_VPSS_Start(pstScenePrev->VpssGrp, pstScenePrev->abChnEnable, &stVpssGrpAttr, astVpssChnAttr);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("start vpss group failed. s32Ret: 0x%x !\n", s32Ret);
        goto EXIT1;
    }

    /*config venc */
    stGopAttr.enGopMode  = VENC_GOPMODE_SMARTP;
    stGopAttr.stSmartP.s32BgQpDelta  = 7;
    stGopAttr.stSmartP.s32ViQpDelta  = 2;
    stGopAttr.stSmartP.u32BgInterval = 1200;
    s32Ret = SAMPLE_COMM_VENC_Start(pstScenePrev->VencChn[0], enType, enPicSize, enRcMode, u32Profile,bRcnRefShareBuf, &stGopAttr);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("start venc failed. s32Ret: 0x%x !\n", s32Ret);
        goto EXIT2;
    }

    s32Ret = SAMPLE_COMM_VPSS_Bind_VENC(pstScenePrev->VpssGrp, pstScenePrev->VpssChn, pstScenePrev->VencChn[0]);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("Venc bind Vpss failed. s32Ret: 0x%x !n", s32Ret);
        goto EXIT3;
    }

    /*config vo*/
    SAMPLE_COMM_VO_GetDefConfig(&pstScenePrev->stVoConfig);
    pstScenePrev->stVoConfig.enDstDynamicRange = enDynamicRange;
    pstScenePrev->stVoConfig.enVoIntfType = VO_INTF_BT1120;
    pstScenePrev->stVoConfig.enIntfSync   = VO_OUTPUT_576P50;
    pstScenePrev->stVoConfig.u32DisBufLen = 3;
    pstScenePrev->stVoConfig.enPicSize = enPicSize;

    /*start vo*/
    s32Ret = SAMPLE_COMM_VO_StartVO(&pstScenePrev->stVoConfig);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("start vo failed. s32Ret: 0x%x !\n", s32Ret);
        goto EXIT4;
    }

    /*vpss bind vo*/
    s32Ret = SAMPLE_COMM_VPSS_Bind_VO(pstScenePrev->VpssGrp, VPSS_CHN1, pstScenePrev->stVoConfig.VoDev, pstScenePrev->VoChn);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("vo bind vpss failed. s32Ret: 0x%x !\n", s32Ret);
        goto EXIT5;
    }

    s32Ret = SAMPLE_COMM_VENC_StartGetStream(pstScenePrev->VencChn, sizeof(pstScenePrev->VencChn)/sizeof(VENC_CHN));
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("Get venc stream failed!\n");
        goto EXIT6;
    }

    return HI_SUCCESS;

EXIT6:
    SAMPLE_COMM_VPSS_UnBind_VO(pstScenePrev->VpssGrp, VPSS_CHN1, pstScenePrev->stVoConfig.VoDev, pstScenePrev->VoChn);
EXIT5:
    SAMPLE_COMM_VO_StopVO(&pstScenePrev->stVoConfig);
EXIT4:
    SAMPLE_COMM_VPSS_UnBind_VENC(pstScenePrev->VpssGrp, pstScenePrev->VpssChn, pstScenePrev->VencChn[0]);
EXIT3:
    SAMPLE_COMM_VENC_Stop(pstScenePrev->VencChn[0]);
EXIT2:
    SAMPLE_COMM_VPSS_Stop(pstScenePrev->VpssGrp, pstScenePrev->abChnEnable);
EXIT1:
    SAMPLE_COMM_VI_StopVi(&pstScenePrev->stViConfig);
EXIT:
    SAMPLE_COMM_SYS_Exit();
    return s32Ret;

}

HI_S32 SAMPLE_SCENE_STOP_PREV(SCENE_PREV_S *pstScenePrev)
{
    //printf("%s,%d\n",__FUNCTION__,__LINE__);
    if(pstScenePrev == HI_NULL)
    {
        SAMPLE_PRT("Err: pstScenePrev is NULL \n");
        return HI_FAILURE;
    }

    SAMPLE_COMM_VENC_StopGetStream();
    SAMPLE_COMM_VPSS_UnBind_VO(pstScenePrev->VpssGrp, VPSS_CHN1, pstScenePrev->stVoConfig.VoDev, pstScenePrev->VoChn);
    SAMPLE_COMM_VO_StopVO(&pstScenePrev->stVoConfig);
    SAMPLE_COMM_VPSS_UnBind_VENC(pstScenePrev->VpssGrp, pstScenePrev->VpssChn, pstScenePrev->VencChn[0]);
    SAMPLE_COMM_VENC_Stop(pstScenePrev->VencChn[0]);
    SAMPLE_COMM_VPSS_Stop(pstScenePrev->VpssGrp, pstScenePrev->abChnEnable);
    SAMPLE_COMM_VI_StopVi(&pstScenePrev->stViConfig);
    SAMPLE_COMM_SYS_Exit();
    return HI_SUCCESS;
}
