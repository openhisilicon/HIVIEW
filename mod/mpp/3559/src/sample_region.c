#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/prctl.h>

#include "sample_comm.h"
#include "mpi_snap.h"

#ifdef __HuaweiLite__
#define  RES_BMP  "/sharefs/bmp/"
#else
#define  RES_BMP  "./res/"
#endif

#define  vi_chn_0_bmp  RES_BMP"vi_chn_0.bmp"
#define  mm_bmp        RES_BMP"mm.bmp"
#define  huawei_bmp    RES_BMP"huawei.bmp"
#define  mm2_bmp       RES_BMP"mm2.bmp"

SAMPLE_VI_CONFIG_S stViConfig          = {0};
SAMPLE_VO_CONFIG_S stVoConfig          = {0};
VPSS_GRP_ATTR_S    stVpssGrpAttr       = {0};
HI_BOOL            abChnEnable[4] = {1,1,0,0};
extern HI_CHAR*    Path_BMP;

/******************************************************************************
* function : show usage
******************************************************************************/
void SAMPLE_REGION_Usage(char* sPrgNm)
{
    printf("Usage : %s <index> \n", sPrgNm);
    printf("index:\n");
    printf("\t 0)VI OSDEX.\n");
    printf("\t 1)VI COVEREX.\n");
    printf("\t 2)VPSS OSDEX.\n");
    printf("\t 3)VPSS COVEREX.\n");
    printf("\t 4)VPSS COVER.\n");
    printf("\t 5)VPSS MOSAIC.\n");
    printf("\t 6)VO OSDEX.\n");
    printf("\t 7)VO COEREX.\n");
    printf("\t 8)VENC OSDEX.\n");
    printf("\t 9)VENC OSD 2BPP.\n");
    return;
}

/******************************************************************************
* function : to process abnormal case
******************************************************************************/
void SAMPLE_REGION_HandleSig(HI_S32 signo)
{
    if (SIGINT == signo || SIGTERM == signo)
    {
        SAMPLE_COMM_All_ISP_Stop();
        SAMPLE_COMM_VO_HdmiStop();
        SAMPLE_COMM_SYS_Exit();
        printf("\033[0;31mprogram termination abnormally!\033[0;39m\n");
    }
    exit(-1);
}

HI_S32 SAMPLE_REGION_StartVi(HI_VOID)
{
    HI_S32             s32Ret;
    VI_DEV             ViDev               = 0;
    VI_PIPE            ViPipe              = 0;
    VI_CHN             ViChn               = 0;
    HI_S32             s32SnsId            = 0;
    SIZE_S             stSize;
    HI_U32             u32BlkSize;
    DYNAMIC_RANGE_E    enDynamicRange      = DYNAMIC_RANGE_SDR8;
    VB_CONFIG_S        stVbConf;
    PIC_SIZE_E         enPicSize           = PIC_3840x2160;

    /************************************************
    step1:  config vi
    *************************************************/
    SAMPLE_COMM_VI_GetSensorInfo(&stViConfig);

    stViConfig.s32WorkingViNum                                  = 1;

    stViConfig.as32WorkingViId[0]                               = s32SnsId;

    stViConfig.astViInfo[s32SnsId].stSnsInfo.MipiDev            = ViDev;
    stViConfig.astViInfo[s32SnsId].stSnsInfo.s32BusId           = 0;

    stViConfig.astViInfo[s32SnsId].stDevInfo.ViDev              = ViDev;
    stViConfig.astViInfo[s32SnsId].stDevInfo.enWDRMode          = WDR_MODE_NONE;

    stViConfig.astViInfo[s32SnsId].stPipeInfo.enMastPipeMode    = VI_ONLINE_VPSS_OFFLINE;
    stViConfig.astViInfo[s32SnsId].stPipeInfo.aPipe[0]          = ViPipe;
    stViConfig.astViInfo[s32SnsId].stPipeInfo.aPipe[1]          = -1;
    stViConfig.astViInfo[s32SnsId].stPipeInfo.aPipe[2]          = -1;
    stViConfig.astViInfo[s32SnsId].stPipeInfo.aPipe[3]          = -1;

    stViConfig.astViInfo[s32SnsId].stChnInfo.ViChn              = ViChn;
    stViConfig.astViInfo[s32SnsId].stChnInfo.enPixFormat        = PIXEL_FORMAT_YVU_SEMIPLANAR_420;
    stViConfig.astViInfo[s32SnsId].stChnInfo.enDynamicRange     = enDynamicRange;
    stViConfig.astViInfo[s32SnsId].stChnInfo.enVideoFormat      = VIDEO_FORMAT_LINEAR;
    stViConfig.astViInfo[s32SnsId].stChnInfo.enCompressMode     = COMPRESS_MODE_NONE;

    /************************************************
    step2:  Get  input size
    *************************************************/
    s32Ret = SAMPLE_COMM_VI_GetSizeBySensor(stViConfig.astViInfo[s32SnsId].stSnsInfo.enSnsType, &enPicSize);
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

    /************************************************
    step3:  Init SYS and common VB
    *************************************************/
    memset(&stVbConf, 0, sizeof(VB_CONFIG_S));
    stVbConf.u32MaxPoolCnt              = 2;

    u32BlkSize = COMMON_GetPicBufferSize(stSize.u32Width, stSize.u32Height, SAMPLE_PIXEL_FORMAT, DATA_BITWIDTH_8, COMPRESS_MODE_SEG, DEFAULT_ALIGN);
    stVbConf.astCommPool[0].u64BlkSize  = u32BlkSize;
    stVbConf.astCommPool[0].u32BlkCnt   = 10;

    u32BlkSize = VI_GetRawBufferSize(stSize.u32Width, stSize.u32Height, PIXEL_FORMAT_RGB_BAYER_16BPP, COMPRESS_MODE_NONE, DEFAULT_ALIGN);
    stVbConf.astCommPool[1].u64BlkSize  = u32BlkSize;
    stVbConf.astCommPool[1].u32BlkCnt   = 4;

    s32Ret = SAMPLE_COMM_SYS_Init(&stVbConf);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("system init failed with %d!\n", s32Ret);
        goto EXIT;
    }

    /************************************************
    step 4: start VI
    *************************************************/
    s32Ret = SAMPLE_COMM_VI_StartVi(&stViConfig);

    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("SAMPLE_COMM_VI_StartVi failed with %d!\n", s32Ret);
        goto EXIT1;
    }
    return s32Ret;
EXIT1:
    SAMPLE_COMM_VI_StopVi(&stViConfig);
EXIT:
    SAMPLE_COMM_SYS_Exit();
    return s32Ret;
}

HI_S32 SAMPLE_REGION_StopVi(HI_VOID)
{
    SAMPLE_COMM_VI_StopVi(&stViConfig);
    SAMPLE_COMM_SYS_Exit();
    return HI_SUCCESS;
}

HI_S32 SAMPLE_REGION_StartVpss(HI_VOID)
{
    HI_S32 i;
    HI_S32 s32Ret;
    VPSS_GRP_ATTR_S stVpssGrpAttr;
    VPSS_CHN_ATTR_S stVpssChnAttr[VPSS_MAX_PHY_CHN_NUM];
    PIC_SIZE_E      enPicSize;
    SIZE_S          stSize;
    VPSS_GRP        VpssGrp        = 0;
    HI_S32          s32SnsId       = 0;

    SAMPLE_COMM_VI_GetSizeBySensor(stViConfig.astViInfo[s32SnsId].stSnsInfo.enSnsType, &enPicSize);

    SAMPLE_COMM_SYS_GetPicSize(enPicSize, &stSize);

    stVpssGrpAttr.enDynamicRange = DYNAMIC_RANGE_SDR8;
    stVpssGrpAttr.enPixelFormat  = PIXEL_FORMAT_YVU_SEMIPLANAR_420;
    stVpssGrpAttr.u32MaxW        = stSize.u32Width;
    stVpssGrpAttr.u32MaxH        = stSize.u32Height;
    stVpssGrpAttr.stFrameRate.s32SrcFrameRate = -1;
    stVpssGrpAttr.stFrameRate.s32DstFrameRate = -1;
    stVpssGrpAttr.bNrEn          = HI_TRUE;
    for(i=0; i<VPSS_MAX_PHY_CHN_NUM; i++)
    {
        if(HI_TRUE == abChnEnable[i])
        {
            stVpssChnAttr[i].u32Width                     = stSize.u32Width;
            stVpssChnAttr[i].u32Height                    = stSize.u32Height;
            stVpssChnAttr[i].enChnMode                    = VPSS_CHN_MODE_USER;
            stVpssChnAttr[i].enCompressMode               = COMPRESS_MODE_NONE;
            stVpssChnAttr[i].enDynamicRange               = DYNAMIC_RANGE_SDR8;
            stVpssChnAttr[i].enPixelFormat                = PIXEL_FORMAT_YVU_SEMIPLANAR_420;
            stVpssChnAttr[i].stFrameRate.s32SrcFrameRate  = 30;
            stVpssChnAttr[i].stFrameRate.s32DstFrameRate  = 30;
            stVpssChnAttr[i].u32Depth                     = 0;
            stVpssChnAttr[i].bMirror                      = HI_FALSE;
            stVpssChnAttr[i].bFlip                        = HI_FALSE;
            stVpssChnAttr[i].enVideoFormat                = VIDEO_FORMAT_LINEAR;
            //stVpssChnAttr[i].stAspectRatio.enMode         = ASPECT_RATIO_NONE;
        }

    }

    s32Ret = SAMPLE_COMM_VPSS_Start(VpssGrp, abChnEnable,&stVpssGrpAttr,stVpssChnAttr);
    if(HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("SAMPLE_COMM_VPSS_StartVPSS failed with %d!\n", s32Ret);
        goto EXIT;
    }
    return HI_SUCCESS;
EXIT:
    SAMPLE_COMM_VPSS_Stop(VpssGrp, abChnEnable);
    return s32Ret;
}

HI_S32 SAMPLE_REGION_StopVpss(HI_VOID)
{
    SAMPLE_COMM_VPSS_Stop(0, abChnEnable);
    return HI_SUCCESS;
}

HI_S32 SAMPLE_REGION_StartVenc(HI_VOID)
{
    HI_S32          s32Ret;
    VENC_GOP_MODE_E enGopMode;
    VENC_GOP_ATTR_S stGopAttr;
    SAMPLE_RC_E     enRcMode;
    HI_S32          s32SnsId            = 0;
    PIC_SIZE_E      enPicSize;
    HI_BOOL         bRcnRefShareBuf = HI_FALSE;
    enRcMode = SAMPLE_RC_CBR;

    enGopMode = VENC_GOPMODE_NORMALP;

    SAMPLE_COMM_VI_GetSizeBySensor(stViConfig.astViInfo[s32SnsId].stSnsInfo.enSnsType, &enPicSize);

    s32Ret = SAMPLE_COMM_VENC_GetGopAttr(enGopMode,&stGopAttr);

   /***encode h.265 **/
    s32Ret = SAMPLE_COMM_VENC_Start(0, PT_H265,enPicSize, enRcMode,0,bRcnRefShareBuf,&stGopAttr);
    if(HI_SUCCESS !=s32Ret)
    {
        SAMPLE_PRT("SAMPLE_COMM_VENC_StartVenc failed with %d!\n", s32Ret);
        goto EXIT;
    }
    return s32Ret;
EXIT:
    SAMPLE_COMM_VENC_Stop(0);
    return s32Ret;
}
HI_S32 SAMPLE_REGION_StopVenc(HI_VOID)
{
    SAMPLE_COMM_VENC_Stop(0);
    return HI_SUCCESS;
}
HI_S32 SAMPLE_REGION_StartVo(HI_VOID)
{
    HI_S32 s32Ret;
    SAMPLE_COMM_VO_GetDefConfig(&stVoConfig);

    stVoConfig.enDstDynamicRange = DYNAMIC_RANGE_SDR8;
    stVoConfig.enVoIntfType = VO_INTF_HDMI;
    stVoConfig.enPicSize = PIC_3840x2160;

    s32Ret= SAMPLE_COMM_VO_StartVO(&stVoConfig);
    if(HI_SUCCESS!=s32Ret)
    {
       SAMPLE_PRT("SAMPLE_COMM_VO_Start failed! s32Ret:0x%x.\n", s32Ret);
       goto EXIT;
    }
    return HI_SUCCESS;
EXIT:
    SAMPLE_COMM_VO_StopVO(&stVoConfig);
    return s32Ret;
}

HI_S32 SAMPLE_REGION_StopVo(HI_VOID)
{
    SAMPLE_COMM_VO_StopVO(&stVoConfig);
    return HI_SUCCESS;
}

HI_VOID SAMPLE_REGION_StartGetVencStream(HI_VOID)
{
    HI_S32 VencChn[2]={0,1};

    SAMPLE_COMM_VENC_StartGetStream(VencChn,1);
    return ;
}

HI_VOID SAMPLE_REGION_StopGetVencStream(HI_VOID)
{
    SAMPLE_COMM_VENC_StopGetStream();
    return;
}
HI_S32 SAMPLE_REGION_MPP_VI_VPSS_VENC_START(HI_VOID)
{
    HI_S32 s32Ret;

    s32Ret = SAMPLE_REGION_StartVi();
    if(HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("Start vi failed!\n");
        return s32Ret;
    }
    s32Ret = SAMPLE_REGION_StartVpss();
    if(HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("Start vpss failed!\n");
        goto START_VPSS_FAILED;
    }
    s32Ret = SAMPLE_COMM_VI_Bind_VPSS(0, 0, 0);
    if(HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("Bind vi vpss failed!\n");
        goto BIND_VI_VPSS_FAILED;
    }
    s32Ret = SAMPLE_REGION_StartVenc();
    if(HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("Start venc failed!\n");
        goto START_VENC_FAILED;
    }
    s32Ret = SAMPLE_COMM_VPSS_Bind_VENC(0, 0, 0);
    if(HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("Bind vpss venc failed!\n");
        goto BIND_VPSS_VENC_FAILED;
    }
    SAMPLE_REGION_StartGetVencStream();
    return HI_SUCCESS;

BIND_VPSS_VENC_FAILED:
    SAMPLE_REGION_StopVenc();
START_VENC_FAILED:
    SAMPLE_COMM_VI_UnBind_VPSS(0,0,0);
BIND_VI_VPSS_FAILED:
    SAMPLE_REGION_StopVpss();
START_VPSS_FAILED:
    SAMPLE_REGION_StopVi();
    return s32Ret;
}

HI_VOID SAMPLE_REGION_MPP_VI_VPSS_VENC_END(HI_VOID)
{
    SAMPLE_REGION_StopGetVencStream();
    SAMPLE_COMM_VPSS_UnBind_VENC(0,0,0);
    SAMPLE_REGION_StopVenc();
    SAMPLE_COMM_VI_UnBind_VPSS(0,0,0);
    SAMPLE_REGION_StopVpss();
    SAMPLE_REGION_StopVi();
    return ;
}

HI_S32 SAMPLE_REGION_VI_VPSS_VENC(HI_S32 HandleNum,RGN_TYPE_E  enType,MPP_CHN_S *pstChn)
{
    HI_S32         i;
    HI_S32         s32Ret;
    HI_S32         MinHandle;
    s32Ret = SAMPLE_REGION_MPP_VI_VPSS_VENC_START();
    if(HI_SUCCESS != s32Ret)
    {
        return  s32Ret;
    }
    s32Ret = SAMPLE_COMM_REGION_Create(HandleNum,enType);
    if(HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("SAMPLE_COMM_REGION_Create failed!\n");
        goto EXIT1;
    }
    s32Ret = SAMPLE_COMM_REGION_AttachToChn(HandleNum,enType,pstChn);
    if(HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("SAMPLE_COMM_REGION_AttachToChn failed!\n");
        goto EXIT2;
    }
    MinHandle = SAMPLE_COMM_REGION_GetMinHandle(enType);
    if(OVERLAY_RGN == enType || OVERLAYEX_RGN == enType)
    {
        for(i= MinHandle; i<MinHandle + HandleNum;i++)
        {
            s32Ret = SAMPLE_COMM_REGION_SetBitMap(i,PIXEL_FORMAT_ARGB_1555);
            if(HI_SUCCESS != s32Ret)
            {
                SAMPLE_PRT("SAMPLE_COMM_REGION_SetBitMap failed!\n");
                goto EXIT2;
            }
        }
    }
    PAUSE();
EXIT2:
    s32Ret = SAMPLE_COMM_REGION_DetachFrmChn(HandleNum,enType,pstChn);
    if(HI_SUCCESS!= s32Ret)
    {
        SAMPLE_PRT("SAMPLE_COMM_REGION_AttachToChn failed!\n");
    }
EXIT1:
    s32Ret = SAMPLE_COMM_REGION_Destroy(HandleNum,enType);
    if(HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("SAMPLE_COMM_REGION_AttachToChn failed!\n");
    }
    SAMPLE_REGION_MPP_VI_VPSS_VENC_END();
    return s32Ret;
}


HI_S32 SAMPLE_REGION_MPP_VI_VPSS_VO_START(HI_VOID)
{
    HI_S32 s32Ret;

    s32Ret = SAMPLE_REGION_StartVi();
    if(HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("Start vi failed!\n");
        return s32Ret;
    }
    s32Ret = SAMPLE_REGION_StartVpss();
    if(HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("Start vpss failed!\n");
        goto START_VPSS_FAILED;
    }
    s32Ret = SAMPLE_COMM_VI_Bind_VPSS(0, 0, 0);
    if(HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("Bind vi vpss failed!\n");
        goto VI_BIND_VPSS_FAILED;

    }
    s32Ret = SAMPLE_REGION_StartVo();
    if(HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("Start  vi failed!\n");
        goto START_Vo_FAILED;
    }
    s32Ret = SAMPLE_COMM_VPSS_Bind_VO(0,0,0,0);
    if(HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("Bind vpss vo failed!\n");
        goto VPSS_BIND_VO_FAILED;
    }
    return HI_SUCCESS;

VPSS_BIND_VO_FAILED:
    SAMPLE_REGION_StopVo();
START_Vo_FAILED:
    SAMPLE_COMM_VI_UnBind_VPSS(0, 0, 0);
VI_BIND_VPSS_FAILED:
    SAMPLE_REGION_StopVpss();
START_VPSS_FAILED:
    SAMPLE_REGION_StopVi();
    return s32Ret;
}

HI_VOID SAMPLE_REGION_MPP_VI_VPSS_VO_END(HI_VOID)
{
    SAMPLE_COMM_VPSS_UnBind_VO(0, 0, 0, 0);
    SAMPLE_REGION_StopVo();
    SAMPLE_COMM_VI_UnBind_VPSS(0, 0, 0);
    SAMPLE_REGION_StopVpss();
    SAMPLE_REGION_StopVi();
    return ;
}

HI_S32 SAMPLE_REGION_VI_VPSS_VO(HI_S32 HandleNum,RGN_TYPE_E  enType,MPP_CHN_S *pstChn)
{
    HI_S32         i;
    HI_S32         s32Ret;
    HI_S32         MinHandle;

    s32Ret = SAMPLE_REGION_MPP_VI_VPSS_VO_START();
    if(HI_SUCCESS != s32Ret)
    {
        return s32Ret;
    }

    s32Ret = SAMPLE_COMM_REGION_Create(HandleNum,enType);
    if(HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("SAMPLE_COMM_REGION_Create failed!\n");
        goto EXIT1;
    }
    s32Ret = SAMPLE_COMM_REGION_AttachToChn(HandleNum,enType,pstChn);
    if(HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("SAMPLE_COMM_REGION_AttachToChn failed!\n");
        goto EXIT2;
    }
    MinHandle = SAMPLE_COMM_REGION_GetMinHandle(enType);

    if(OVERLAY_RGN == enType || OVERLAYEX_RGN == enType)
    {
        for(i= MinHandle; i<MinHandle + HandleNum;i++)
        {

            //s32Ret = SAMPLE_COMM_REGION_SetBitMap(i);
            s32Ret = SAMPLE_COMM_REGION_GetUpCanvas(i);
            if(HI_SUCCESS != s32Ret)
            {
                SAMPLE_PRT("SAMPLE_COMM_REGION_SetBitMap failed!\n");
                goto EXIT2;
            }
        }
    }

    PAUSE();
EXIT2:
    s32Ret = SAMPLE_COMM_REGION_DetachFrmChn(HandleNum,enType,pstChn);
    if(HI_SUCCESS!= s32Ret)
    {
        SAMPLE_PRT("SAMPLE_COMM_REGION_AttachToChn failed!\n");
    }
EXIT1:
    s32Ret = SAMPLE_COMM_REGION_Destroy(HandleNum,enType);
    if(HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("SAMPLE_COMM_REGION_AttachToChn failed!\n");
    }

    SAMPLE_REGION_MPP_VI_VPSS_VO_END();
    return s32Ret;
}

HI_S32 SAMPLE_REGION_VI_OSDEX(HI_VOID)
{
    HI_S32             s32Ret;
    HI_S32             HandleNum;
    RGN_TYPE_E         enType;
    MPP_CHN_S          stChn;

    HandleNum =8;
    enType = OVERLAYEX_RGN;
    stChn.enModId = HI_ID_VI;
    stChn.s32DevId = 0;
    stChn.s32ChnId = 0;
    Path_BMP = vi_chn_0_bmp;
    s32Ret = SAMPLE_REGION_VI_VPSS_VO(HandleNum,enType,&stChn);
    return s32Ret;
}

HI_S32 SAMPLE_REGION_VI_COVEREX(HI_VOID)
{
    HI_S32             s32Ret;
    HI_S32             HandleNum;
    RGN_TYPE_E         enType;
    MPP_CHN_S          stChn;

    HandleNum =8;
    enType = COVEREX_RGN;
    stChn.enModId = HI_ID_VI;
    stChn.s32DevId = 0;
    stChn.s32ChnId = 0;
    s32Ret = SAMPLE_REGION_VI_VPSS_VO(HandleNum,enType,&stChn);
    return s32Ret;
}

HI_S32 SAMPLE_REGION_VPSS_OSDEX(HI_VOID)
{
    HI_S32             s32Ret;
    HI_S32             HandleNum;
    RGN_TYPE_E         enType;
    MPP_CHN_S          stChn;

    HandleNum =8;
    enType = OVERLAYEX_RGN;
    stChn.enModId = HI_ID_VPSS;
    stChn.s32DevId = 0;
    stChn.s32ChnId = 0;
    Path_BMP = vi_chn_0_bmp;
    s32Ret = SAMPLE_REGION_VI_VPSS_VO(HandleNum,enType,&stChn);
    return s32Ret;
}

HI_S32 SAMPLE_REGION_VPSS_COVEREX(HI_VOID)
{
    HI_S32             s32Ret;
    HI_S32             HandleNum;
    RGN_TYPE_E         enType;
    MPP_CHN_S          stChn;

    HandleNum =8;
    enType = COVEREX_RGN;
    stChn.enModId = HI_ID_VPSS;
    stChn.s32DevId = 0;
    stChn.s32ChnId = 0;
    s32Ret = SAMPLE_REGION_VI_VPSS_VO(HandleNum,enType,&stChn);
    return s32Ret;
}

HI_S32 SAMPLE_REGION_VPSS_COVER(HI_VOID)
{
    HI_S32             s32Ret;
    HI_S32             HandleNum;
    RGN_TYPE_E         enType;
    MPP_CHN_S          stChn;

    HandleNum =8;
    enType = COVER_RGN;
    stChn.enModId = HI_ID_VPSS;
    stChn.s32DevId = 0;
    stChn.s32ChnId = 0;
    s32Ret = SAMPLE_REGION_VI_VPSS_VO(HandleNum,enType,&stChn);
    return s32Ret;
}

HI_S32 SAMPLE_REGION_VPSS_MOSAIC(HI_VOID)
{
    HI_S32             s32Ret;
    HI_S32             HandleNum;
    RGN_TYPE_E         enType;
    MPP_CHN_S          stChn;

    HandleNum =4;
    enType = MOSAIC_RGN;
    stChn.enModId = HI_ID_VPSS;
    stChn.s32DevId = 0;
    stChn.s32ChnId = 0;
    s32Ret = SAMPLE_REGION_VI_VPSS_VO(HandleNum,enType,&stChn);
    return s32Ret;
}

HI_S32 SAMPLE_REGION_VO_OSDEX(HI_VOID)
{
    HI_S32             s32Ret;
    HI_S32             HandleNum;
    RGN_TYPE_E         enType;
    MPP_CHN_S          stChn;

    HandleNum =1;
    enType = OVERLAYEX_RGN;
    stChn.enModId = HI_ID_VO;
    stChn.s32DevId = 0;
    stChn.s32ChnId = 0;
    Path_BMP = vi_chn_0_bmp;
    s32Ret = SAMPLE_REGION_VI_VPSS_VO(HandleNum,enType,&stChn);
    return s32Ret;
}

HI_S32 SAMPLE_REGION_VO_COVEREX(HI_VOID)
{
    HI_S32             s32Ret;
    HI_S32             HandleNum;
    RGN_TYPE_E         enType;
    MPP_CHN_S          stChn;

    HandleNum =1;
    enType = COVEREX_RGN;
    stChn.enModId = HI_ID_VO;
    stChn.s32DevId = 0;
    stChn.s32ChnId = 0;
    s32Ret = SAMPLE_REGION_VI_VPSS_VO(HandleNum,enType,&stChn);
    return s32Ret;
}

HI_S32 SAMPLE_REGION_VENC_OSD(HI_VOID)
{
    HI_S32             s32Ret;
    HI_S32             HandleNum;
    RGN_TYPE_E         enType;
    MPP_CHN_S          stChn;

    HandleNum =8;
    enType = OVERLAY_RGN;
    stChn.enModId = HI_ID_VENC;
    stChn.s32DevId = 0;
    stChn.s32ChnId = 0;
    Path_BMP = mm_bmp;
    s32Ret = SAMPLE_REGION_VI_VPSS_VENC(HandleNum,enType,&stChn);
    return s32Ret;
}

HI_S32 SAMPLE_REGION_CreateOverLay_2BPP(HI_S32 HandleNum)
{
    HI_S32 s32Ret;
    HI_S32 i;
    RGN_ATTR_S stRegion;
    PIXEL_FORMAT_E enPixelFmt = PIXEL_FORMAT_ARGB_2BPP;
    HI_U32         u32BgColor = 0x1;
    HI_S32 MinHandle = SAMPLE_COMM_REGION_GetMinHandle(OVERLAY_RGN);

    stRegion.enType = OVERLAY_RGN;
    stRegion.unAttr.stOverlay.stSize.u32Height = 160;
    stRegion.unAttr.stOverlay.stSize.u32Width  = 160;
    stRegion.unAttr.stOverlay.u32CanvasNum = 5;
    for(i = MinHandle;i < MinHandle + HandleNum;i++)
    {
        stRegion.unAttr.stOverlay.enPixelFmt = enPixelFmt;
        stRegion.unAttr.stOverlay.u32BgColor = u32BgColor;
        s32Ret = HI_MPI_RGN_Create(i, &stRegion);
        if(HI_SUCCESS != s32Ret)
        {
            SAMPLE_PRT("HI_MPI_RGN_Create failed with %#x!\n", s32Ret);
            return HI_FAILURE;
        }
    }

    return s32Ret;
}



HI_S32 SAMPLE_REGION_VI_VPSS_VENC_2BPP(HI_S32 HandleNum,RGN_TYPE_E  enType,MPP_CHN_S *pstChn)
{
    HI_S32         i;
    HI_S32         s32Ret;
    HI_S32         MinHandle;
    PIXEL_FORMAT_E enPixelFmt = PIXEL_FORMAT_ARGB_2BPP;

    s32Ret = SAMPLE_REGION_MPP_VI_VPSS_VENC_START();
    if(HI_SUCCESS != s32Ret)
    {
        return  s32Ret;
    }
    s32Ret = SAMPLE_REGION_CreateOverLay_2BPP(HandleNum);
    if(HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("SAMPLE_COMM_REGION_Create failed!\n");
        goto EXIT1;
    }
    s32Ret = SAMPLE_COMM_REGION_AttachToChn(HandleNum,enType,pstChn);
    if(HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("SAMPLE_COMM_REGION_AttachToChn failed!\n");
        goto EXIT2;
    }
    MinHandle = SAMPLE_COMM_REGION_GetMinHandle(enType);
    if(MinHandle == -1)
    {
        SAMPLE_PRT("SAMPLE_COMM_REGION_GetMinHandle failed!\n");
        return HI_FAILURE;
    }
    if(OVERLAY_RGN == enType || OVERLAYEX_RGN == enType)
    {
        for(i= MinHandle; i<MinHandle + HandleNum;i++)
        {

            s32Ret = SAMPLE_COMM_REGION_SetBitMap(i,enPixelFmt);
            if(HI_SUCCESS != s32Ret)
            {
                SAMPLE_PRT("SAMPLE_COMM_REGION_SetBitMap failed!\n");
                goto EXIT2;
            }
        }

    }

    PAUSE();
EXIT2:
    s32Ret = SAMPLE_COMM_REGION_DetachFrmChn(HandleNum,enType,pstChn);
    if(HI_SUCCESS!= s32Ret)
    {
        SAMPLE_PRT("SAMPLE_COMM_REGION_AttachToChn failed!\n");
    }
EXIT1:
    s32Ret = SAMPLE_COMM_REGION_Destroy(HandleNum,enType);
    if(HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("SAMPLE_COMM_REGION_AttachToChn failed!\n");
    }
    SAMPLE_REGION_MPP_VI_VPSS_VENC_END();
    return s32Ret;
}


HI_S32 SAMPLE_REGION_VENC_OSD_2BPP(HI_VOID)
{

    HI_S32             s32Ret;
    HI_S32             HandleNum;
    RGN_TYPE_E         enType;
    MPP_CHN_S          stChn;

    HandleNum =8;
    enType = OVERLAY_RGN;
    stChn.enModId = HI_ID_VENC;
    stChn.s32DevId = 0;
    stChn.s32ChnId = 0;
    Path_BMP = mm_bmp;
    s32Ret = SAMPLE_REGION_VI_VPSS_VENC_2BPP(HandleNum,enType,&stChn);
    return s32Ret;
}
/******************************************************************************
* function    : main()
* Description : main
******************************************************************************/
#ifdef __HuaweiLite__
int app_main(int argc, char *argv[])
#else
int sample_region_main(int argc, char* argv[])
#endif
{
    HI_S32 s32Ret = HI_FAILURE;
    HI_S32 s32Index;

    if (argc < 2 || argc > 2)
    {
        SAMPLE_REGION_Usage(argv[0]);
        return HI_FAILURE;
    }

    if (!strncmp(argv[1], "-h", 2))
    {
        SAMPLE_REGION_Usage(argv[0]);
        return HI_SUCCESS;
    }

#ifndef __HuaweiLite__
    signal(SIGINT, SAMPLE_REGION_HandleSig);
    signal(SIGTERM, SAMPLE_REGION_HandleSig);
#endif

    s32Index = atoi(argv[1]);
    switch (s32Index)
    {
        case 0:
            s32Ret = SAMPLE_REGION_VI_OSDEX();
            break;
        case 1:
            s32Ret = SAMPLE_REGION_VI_COVEREX();
            break;
        case 2:
            s32Ret = SAMPLE_REGION_VPSS_OSDEX();
            break;
        case 3:
            s32Ret = SAMPLE_REGION_VPSS_COVEREX();
            break;
        case 4:
#ifdef CONFIG_HI_VPSS_COVER_SUPPORT
            s32Ret = SAMPLE_REGION_VPSS_COVER();
#else
            SAMPLE_PRT("vpss not support cover!\n");
#endif
            break;
        case 5:
#ifdef CONFIG_HI_VPSS_MOSAIC_SUPPORT
            s32Ret = SAMPLE_REGION_VPSS_MOSAIC();
#else
            SAMPLE_PRT("vpss not support mosaic!\n");
#endif
            break;
        case 6:
#ifdef CONFIG_HI_VO_COVER_OSD_SUPPORT
            s32Ret = SAMPLE_REGION_VO_OSDEX();
#else
            SAMPLE_PRT("vo not support overlayex!\n");
#endif
            break;
        case 7:
#ifdef CONFIG_HI_VO_COVER_OSD_SUPPORT
            s32Ret = SAMPLE_REGION_VO_COVEREX();
#else
            SAMPLE_PRT("vo not support coverex!\n");
#endif
            break;
        case 8:
            s32Ret = SAMPLE_REGION_VENC_OSD();
            break;
        case 9:
            s32Ret = SAMPLE_REGION_VENC_OSD_2BPP();
            break;
        default:
            SAMPLE_PRT("the index %d is invaild!\n",s32Index);
            SAMPLE_REGION_Usage(argv[0]);
            s32Ret = HI_FAILURE;
            break;
    }

    if (HI_SUCCESS == s32Ret)
    {
        SAMPLE_PRT("program exit normally!\n");
    }
    else
    {
        SAMPLE_PRT("program exit abnormally!\n");
    }

    return (s32Ret);
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */
