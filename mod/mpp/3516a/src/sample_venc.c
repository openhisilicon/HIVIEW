/******************************************************************************

  Copyright (C), 2017, Hisilicon Tech. Co., Ltd.

 ******************************************************************************
  File Name     : sample_venc.c
  Version       : Initial Draft
  Author        : Hisilicon multimedia software group
  Created       : 2017
  Description   :
******************************************************************************/

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

#include "sample_comm.h"

#ifdef HI_FPGA
    #define PIC_SIZE   PIC_1080P
#else
    #define PIC_SIZE   PIC_1080P
#endif


/******************************************************************************
* function : show usage
******************************************************************************/
void SAMPLE_VENC_Usage(char* sPrgNm)
{
    printf("Usage : %s [index] \n", sPrgNm);
    printf("index:\n");
    printf("\t  0) H.265e + H264e.\n");
    printf("\t  1) Lowdelay:H.265e.\n");
    printf("\t  2) Qpmap:H.265e + H264e.\n");
    printf("\t  3) IntraRefresh:H.265e + H264e.\n");
    printf("\t  4) RoiBgFrameRate:H.265e + H.264e.\n");
    printf("\t  5) DeBreathEffect:H.265e + H.264e.\n");
    printf("\t  6) svc-t :H.264.\n");
    printf("\t  7) Mjpeg +Jpeg.\n");

    return;
}

/******************************************************************************
* function : to process abnormal case
******************************************************************************/
void SAMPLE_VENC_HandleSig(HI_S32 signo)
{
    signal(SIGINT, SIG_IGN);
    signal(SIGTERM, SIG_IGN);

    if (SIGINT == signo || SIGTERM == signo)
    {
        SAMPLE_COMM_VENC_StopSendQpmapFrame();
        SAMPLE_COMM_VENC_StopGetStream();
        SAMPLE_COMM_All_ISP_Stop();
        SAMPLE_COMM_SYS_Exit();
        printf("\033[0;31mprogram termination abnormally!\033[0;39m\n");
    }
    exit(-1);
}

/******************************************************************************
* function : to process abnormal case - the case of stream venc
******************************************************************************/
void SAMPLE_VENC_StreamHandleSig(HI_S32 signo)
{

    if (SIGINT == signo || SIGTERM == signo)
    {
        SAMPLE_COMM_SYS_Exit();
        printf("\033[0;31mprogram exit abnormally!\033[0;39m\n");
    }

    exit(0);
}

VENC_GOP_MODE_E SAMPLE_VENC_GetGopMode(void)
{
    char c;
    VENC_GOP_MODE_E enGopMode = 0;

Begin_Get:

    printf("please input choose gop mode!\n");
    printf("\t 0) NORMALP.\n");
    printf("\t 1) DUALP.\n");
    printf("\t 2) SMARTP.\n");

    while((c = getchar()) != '\n' && c != EOF)
    switch(c)
    {
        case '0':
            enGopMode = VENC_GOPMODE_NORMALP;
            break;
        case '1':
            enGopMode = VENC_GOPMODE_DUALP;
            break;
        case '2':
            enGopMode = VENC_GOPMODE_SMARTP;
            break;
        default:
            SAMPLE_PRT("input rcmode: %c, is invaild!\n",c);
            goto Begin_Get;
    }

    return enGopMode;
}

SAMPLE_RC_E SAMPLE_VENC_GetRcMode(void)
{
    char c;
    SAMPLE_RC_E  enRcMode = 0;

Begin_Get:

    printf("please input choose rc mode!\n");
    printf("\t c) cbr.\n");
    printf("\t v) vbr.\n");
    printf("\t a) avbr.\n");
    printf("\t x) cvbr.\n");
    printf("\t q) qvbr.\n");
    printf("\t f) fixQp\n");

    while((c = getchar()) != '\n' && c != EOF)
    switch(c)
    {
        case 'c':
            enRcMode = SAMPLE_RC_CBR;
            break;
        case 'v':
            enRcMode = SAMPLE_RC_VBR;
            break;
        case 'a':
            enRcMode = SAMPLE_RC_AVBR;
            break;
        case 'q':
            enRcMode = SAMPLE_RC_QVBR;
            break;
        case 'x':
            enRcMode = SAMPLE_RC_CVBR;
            break;
        case 'f':
            enRcMode = SAMPLE_RC_FIXQP;
            break;
        default:
            SAMPLE_PRT("input rcmode: %c, is invaild!\n",c);
            goto Begin_Get;
    }
    return enRcMode;
}

VENC_INTRA_REFRESH_MODE_E SAMPLE_VENC_GetIntraRefreshMode(void)
{
    char c;
    VENC_INTRA_REFRESH_MODE_E   enIntraRefreshMode = INTRA_REFRESH_ROW;

Begin_Get:

    printf("please input choose IntraRefresh mode!\n");
    printf("\t r) ROW.\n");
    printf("\t c) COLUMN.\n");

    while((c = getchar()) != '\n' && c != EOF)
    switch(c)
    {
        case 'r':
            enIntraRefreshMode = INTRA_REFRESH_ROW;
            break;
        case 'c':
            enIntraRefreshMode = INTRA_REFRESH_COLUMN;
            break;

        default:
            SAMPLE_PRT("input IntraRefresh Mode: %c, is invaild!\n",c);
            goto Begin_Get;
    }
    return enIntraRefreshMode;
}


HI_S32 SAMPLE_VENC_SYS_Init(HI_U32 u32SupplementConfig,SAMPLE_SNS_TYPE_E  enSnsType)
{
    HI_S32 s32Ret;
    HI_U64 u64BlkSize;
    VB_CONFIG_S stVbConf;
    PIC_SIZE_E enSnsSize;
    SIZE_S     stSnsSize;

    memset(&stVbConf, 0, sizeof(VB_CONFIG_S));

    s32Ret = SAMPLE_COMM_VI_GetSizeBySensor(enSnsType, &enSnsSize);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("SAMPLE_COMM_VI_GetSizeBySensor failed!\n");
        return s32Ret;
    }

    s32Ret = SAMPLE_COMM_SYS_GetPicSize(enSnsSize, &stSnsSize);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("SAMPLE_COMM_SYS_GetPicSize failed!\n");
        return s32Ret;
    }

    u64BlkSize = COMMON_GetPicBufferSize(stSnsSize.u32Width, stSnsSize.u32Height, PIXEL_FORMAT_YVU_SEMIPLANAR_422, DATA_BITWIDTH_8, COMPRESS_MODE_SEG,DEFAULT_ALIGN);
    stVbConf.astCommPool[0].u64BlkSize   = u64BlkSize;
    stVbConf.astCommPool[0].u32BlkCnt    = (stSnsSize.u32Width>=3840)?20/2:20*1.5;

    stVbConf.u32MaxPoolCnt = 1;
    
    if(stSnsSize.u32Width != 1920)
    {
      u64BlkSize = COMMON_GetPicBufferSize(1920, 1080, PIXEL_FORMAT_YVU_SEMIPLANAR_422, DATA_BITWIDTH_8, COMPRESS_MODE_SEG,DEFAULT_ALIGN);
      stVbConf.astCommPool[1].u64BlkSize   = u64BlkSize;
      stVbConf.astCommPool[1].u32BlkCnt    = 10;
      stVbConf.u32MaxPoolCnt++;
    }

    if(0 == u32SupplementConfig)
    {
        s32Ret = SAMPLE_COMM_SYS_Init(&stVbConf);
    }
    else
    {
        s32Ret = SAMPLE_COMM_SYS_InitWithVbSupplement(&stVbConf,u32SupplementConfig);
    }
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("SAMPLE_COMM_SYS_GetPicSize failed!\n");
        return s32Ret;
    }

    return HI_SUCCESS;
}

HI_VOID SAMPLE_VENC_SetDCFInfo(VI_PIPE ViPipe)
{
    ISP_DCF_INFO_S stIspDCF;

    HI_MPI_ISP_GetDCFInfo(ViPipe, &stIspDCF);

    //description: Thumbnail test
    strncpy((char *)stIspDCF.stIspDCFConstInfo.au8ImageDescription,"Thumbnail test",DCF_DRSCRIPTION_LENGTH);
    //manufacturer: Hisilicon
    strncpy((char *)stIspDCF.stIspDCFConstInfo.au8Make,"Hisilicon",DCF_DRSCRIPTION_LENGTH);
    //model number: Hisilicon IP Camera
    strncpy((char *)stIspDCF.stIspDCFConstInfo.au8Model,"Hisilicon IP Camera",DCF_DRSCRIPTION_LENGTH);
    //firmware version: v.1.1.0
    strncpy((char *)stIspDCF.stIspDCFConstInfo.au8Software,"v.1.1.0",DCF_DRSCRIPTION_LENGTH);


    stIspDCF.stIspDCFConstInfo.u32FocalLength             = 0x00640001;
    stIspDCF.stIspDCFConstInfo.u8Contrast                 = 5;
    stIspDCF.stIspDCFConstInfo.u8CustomRendered           = 0;
    stIspDCF.stIspDCFConstInfo.u8FocalLengthIn35mmFilm    = 1;
    stIspDCF.stIspDCFConstInfo.u8GainControl              = 1;
    stIspDCF.stIspDCFConstInfo.u8LightSource              = 1;
    stIspDCF.stIspDCFConstInfo.u8MeteringMode             = 1;
    stIspDCF.stIspDCFConstInfo.u8Saturation               = 1;
    stIspDCF.stIspDCFConstInfo.u8SceneCaptureType         = 1;
    stIspDCF.stIspDCFConstInfo.u8SceneType                = 0;
    stIspDCF.stIspDCFConstInfo.u8Sharpness                = 5;
    stIspDCF.stIspDCFUpdateInfo.u32ISOSpeedRatings         = 500;
    stIspDCF.stIspDCFUpdateInfo.u32ExposureBiasValue       = 5;
    stIspDCF.stIspDCFUpdateInfo.u32ExposureTime            = 0x00010004;
    stIspDCF.stIspDCFUpdateInfo.u32FNumber                 = 0x0001000f;
    stIspDCF.stIspDCFUpdateInfo.u8WhiteBalance             = 1;
    stIspDCF.stIspDCFUpdateInfo.u8ExposureMode             = 0;
    stIspDCF.stIspDCFUpdateInfo.u8ExposureProgram          = 1;
    stIspDCF.stIspDCFUpdateInfo.u32MaxApertureValue        = 0x00010001;

    HI_MPI_ISP_SetDCFInfo(ViPipe, &stIspDCF);

    return;
}

HI_S32 SAMPLE_VENC_VI_Init( SAMPLE_VI_CONFIG_S *pstViConfig, HI_BOOL bLowDelay, HI_U32 u32SupplementConfig)
{
    HI_S32              s32Ret;
    SAMPLE_SNS_TYPE_E   enSnsType;
    ISP_CTRL_PARAM_S    stIspCtrlParam;
    HI_U32              u32FrameRate;

    enSnsType = pstViConfig->astViInfo[0].stSnsInfo.enSnsType;

    int i = 0;
    for(i = 0; i < pstViConfig->s32WorkingViNum; i++)
    {
      pstViConfig->as32WorkingViId[i] = i;
      //pstViConfig->s32WorkingViNum  = 1;

      pstViConfig->astViInfo[i].stSnsInfo.MipiDev   = SAMPLE_COMM_VI_GetComboDevBySensor(pstViConfig->astViInfo[i].stSnsInfo.enSnsType, i);
      
      // Sensor绑定的I2C/SPI设备号
      pstViConfig->astViInfo[i].stSnsInfo.s32BusId  = i;

      // maohw ViDev = MipiDev;
      pstViConfig->astViInfo[i].stDevInfo.ViDev   =   pstViConfig->astViInfo[i].stSnsInfo.MipiDev;
      
      //maohw pstViConfig->astViInfo[i].stDevInfo.enWDRMode = WDR_MODE_NONE;

      pstViConfig->astViInfo[i].stPipeInfo.enMastPipeMode = VI_OFFLINE_VPSS_OFFLINE;
      pstViConfig->astViInfo[i].stChnInfo.enCompressMode  = COMPRESS_MODE_SEG;
      
      if(HI_TRUE == bLowDelay)
      {
        VI_PIPE_ATTR_S stPipeAttr = {0};      
        SAMPLE_COMM_VI_GetPipeAttrBySns(pstViConfig->astViInfo[i].stSnsInfo.enSnsType, &stPipeAttr);
        //Hi3516AV300 只有 VI PIPE0支持 VI_ONLINE_VPSS_ONLINE 和 VI_ONLINE_VPSS_OFFLINE
        if(pstViConfig->astViInfo[i].stPipeInfo.aPipe[0] == 0)
        {
          pstViConfig->astViInfo[i].stPipeInfo.enMastPipeMode = (stPipeAttr.u32MaxW > 2048)?VI_ONLINE_VPSS_OFFLINE:VI_ONLINE_VPSS_ONLINE;
        }
        else if(VI_PIPE_BYPASS_BE != stPipeAttr.enPipeBypassMode)
        {
          pstViConfig->astViInfo[i].stPipeInfo.enMastPipeMode = VI_OFFLINE_VPSS_ONLINE;
        }
        //printf("i:%d, aPipe[0]:%d, enMastPipeMode:%d\n", i, pstViConfig->astViInfo[i].stPipeInfo.aPipe[0], pstViConfig->astViInfo[i].stPipeInfo.enMastPipeMode);
      }
      
      //pstViConfig->astViInfo[0].stPipeInfo.aPipe[0]        = ViPipe0;
      pstViConfig->astViInfo[i].stPipeInfo.aPipe[1]          = pstViConfig->astViInfo[i].stDevInfo.enWDRMode?pstViConfig->astViInfo[i].stPipeInfo.aPipe[0]+1:-1;
      pstViConfig->astViInfo[i].stPipeInfo.aPipe[2]          = -1;
      pstViConfig->astViInfo[i].stPipeInfo.aPipe[3]          = -1;

      //pstViConfig->astViInfo[0].stChnInfo.ViChn              = ViChn;
      //pstViConfig->astViInfo[0].stChnInfo.enPixFormat        = PIXEL_FORMAT_YVU_SEMIPLANAR_420;
      //pstViConfig->astViInfo[0].stChnInfo.enDynamicRange     = enDynamicRange;
      pstViConfig->astViInfo[i].stChnInfo.enVideoFormat       = VIDEO_FORMAT_LINEAR;
	  }
	  
    s32Ret = SAMPLE_VENC_SYS_Init(u32SupplementConfig,enSnsType);
    if(s32Ret != HI_SUCCESS)
    {
        SAMPLE_PRT("Init SYS err for %#x!\n", s32Ret);
        return s32Ret;
    }

    s32Ret = SAMPLE_COMM_VI_SetParam(pstViConfig);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("SAMPLE_COMM_VI_SetParam failed with %d!\n", s32Ret);
        return s32Ret;
    }

    SAMPLE_COMM_VI_GetFrameRateBySensor(enSnsType, &u32FrameRate);
    
    for(i = 0; i < pstViConfig->s32WorkingViNum; i++)
    {

      s32Ret = HI_MPI_ISP_GetCtrlParam(pstViConfig->astViInfo[i].stPipeInfo.aPipe[0], &stIspCtrlParam);
      if (HI_SUCCESS != s32Ret)
      {
          SAMPLE_PRT("HI_MPI_ISP_GetCtrlParam failed with %d!\n", s32Ret);
          return s32Ret;
      }
      stIspCtrlParam.u32StatIntvl  = u32FrameRate/30;

      s32Ret = HI_MPI_ISP_SetCtrlParam(pstViConfig->astViInfo[i].stPipeInfo.aPipe[0], &stIspCtrlParam);
      if (HI_SUCCESS != s32Ret)
      {
          SAMPLE_PRT("HI_MPI_ISP_SetCtrlParam failed with %d!\n", s32Ret);
          return s32Ret;
      }

	  }

    s32Ret = SAMPLE_COMM_VI_StartVi(pstViConfig);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_COMM_SYS_Exit();
        SAMPLE_PRT("SAMPLE_COMM_VI_StartVi failed with %d!\n", s32Ret);
        return s32Ret;
    }

    return HI_SUCCESS;
}


typedef struct {
  PIXEL_FORMAT_E enPixelFormat;
}vpss_grp_attr_t;

vpss_grp_attr_t vpss_grp_attr[VPSS_MAX_GRP_NUM];

HI_S32 SAMPLE_VENC_VPSS_GrpAttr(VPSS_GRP VpssGrp, PIXEL_FORMAT_E enPixelFormat)
{
  vpss_grp_attr[VpssGrp].enPixelFormat = enPixelFormat;
  return 0;
}

HI_S32 SAMPLE_VENC_VPSS_Init(VPSS_GRP VpssGrp, HI_BOOL* pabChnEnable, DYNAMIC_RANGE_E enDynamicRange,PIXEL_FORMAT_E enPixelFormat,SIZE_S stSize[],SAMPLE_SNS_TYPE_E enSnsType)
{
    HI_S32 i;
    HI_S32 s32Ret;
    PIC_SIZE_E      enSnsSize;
    SIZE_S          stSnsSize;
    VPSS_GRP_ATTR_S stVpssGrpAttr = {0};
    VPSS_CHN_ATTR_S stVpssChnAttr[VPSS_MAX_PHY_CHN_NUM];

    s32Ret = SAMPLE_COMM_VI_GetSizeBySensor(enSnsType, &enSnsSize);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("SAMPLE_COMM_VI_GetSizeBySensor failed!\n");
        return s32Ret;
    }

    s32Ret = SAMPLE_COMM_SYS_GetPicSize(enSnsSize, &stSnsSize);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("SAMPLE_COMM_SYS_GetPicSize failed!\n");
        return s32Ret;
    }


    stVpssGrpAttr.enDynamicRange = enDynamicRange;
    stVpssGrpAttr.enPixelFormat  = vpss_grp_attr[VpssGrp].enPixelFormat?:enPixelFormat;//maohw;
    stVpssGrpAttr.u32MaxW        = stSnsSize.u32Width;
    stVpssGrpAttr.u32MaxH        = stSnsSize.u32Height;
    stVpssGrpAttr.stFrameRate.s32SrcFrameRate = -1;
    stVpssGrpAttr.stFrameRate.s32DstFrameRate = -1;
    stVpssGrpAttr.bNrEn = HI_TRUE;
    stVpssGrpAttr.stNrAttr.enNrType = VPSS_NR_TYPE_VIDEO;
    stVpssGrpAttr.stNrAttr.enNrMotionMode = NR_MOTION_MODE_NORMAL;
    stVpssGrpAttr.stNrAttr.enCompressMode = COMPRESS_MODE_FRAME;


    for(i=0; i<VPSS_MAX_PHY_CHN_NUM; i++)
    {
        if(HI_TRUE == pabChnEnable[i])
        {
            stVpssChnAttr[i].u32Width                     = stSize[i].u32Width;
            stVpssChnAttr[i].u32Height                    = stSize[i].u32Height;
            stVpssChnAttr[i].enChnMode                    = VPSS_CHN_MODE_USER;
            stVpssChnAttr[i].enCompressMode               = COMPRESS_MODE_NONE;//COMPRESS_MODE_SEG;//COMPRESS_MODE_NONE;
            stVpssChnAttr[i].enDynamicRange               = enDynamicRange;
            stVpssChnAttr[i].enPixelFormat                = enPixelFormat;//422
            stVpssChnAttr[i].stFrameRate.s32SrcFrameRate  = -1;
            stVpssChnAttr[i].stFrameRate.s32DstFrameRate  = -1;
            stVpssChnAttr[i].u32Depth                     = 1; // maohw HI_MPI_VPSS_GetChnFrame;
            stVpssChnAttr[i].bMirror                      = HI_FALSE;
            stVpssChnAttr[i].bFlip                        = HI_FALSE;
            stVpssChnAttr[i].enVideoFormat                = VIDEO_FORMAT_LINEAR;
            stVpssChnAttr[i].stAspectRatio.enMode         = ASPECT_RATIO_NONE;
        }
    }

    s32Ret = SAMPLE_COMM_VPSS_Start(VpssGrp, pabChnEnable,&stVpssGrpAttr,stVpssChnAttr);
    if(s32Ret != HI_SUCCESS)
    {
        SAMPLE_PRT("start VPSS fail for %#x!\n", s32Ret);
    }

    return s32Ret;
}


HI_S32 SAMPLE_VENC_CheckSensor(SAMPLE_SNS_TYPE_E   enSnsType,SIZE_S  stSize)
{
    HI_S32 s32Ret;
    SIZE_S          stSnsSize;
    PIC_SIZE_E      enSnsSize;

    s32Ret = SAMPLE_COMM_VI_GetSizeBySensor(enSnsType, &enSnsSize);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("SAMPLE_COMM_VI_GetSizeBySensor failed!\n");
        return s32Ret;
    }
    s32Ret = SAMPLE_COMM_SYS_GetPicSize(enSnsSize, &stSnsSize);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("SAMPLE_COMM_SYS_GetPicSize failed!\n");
        return s32Ret;
    }

    if((stSnsSize.u32Width < stSize.u32Width) || (stSnsSize.u32Height < stSize.u32Height))
    {
        //SAMPLE_PRT("Sensor size is (%d,%d), but encode chnl is (%d,%d) !\n",
        //    stSnsSize.u32Width,stSnsSize.u32Height,stSize.u32Width,stSize.u32Height);
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

HI_S32 SAMPLE_VENC_ModifyResolution(SAMPLE_SNS_TYPE_E   enSnsType,PIC_SIZE_E *penSize,SIZE_S *pstSize)
{
    HI_S32 s32Ret;
    SIZE_S          stSnsSize;
    PIC_SIZE_E      enSnsSize;

    s32Ret = SAMPLE_COMM_VI_GetSizeBySensor(enSnsType, &enSnsSize);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("SAMPLE_COMM_VI_GetSizeBySensor failed!\n");
        return s32Ret;
    }
    s32Ret = SAMPLE_COMM_SYS_GetPicSize(enSnsSize, &stSnsSize);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("SAMPLE_COMM_SYS_GetPicSize failed!\n");
        return s32Ret;
    }

    *penSize = enSnsSize;
    pstSize->u32Width  = stSnsSize.u32Width;
    pstSize->u32Height = stSnsSize.u32Height;

    return HI_SUCCESS;
}
/******************************************************************************
* function: H.265e + H264e@720P, H.265 Channel resolution adaptable with sensor
******************************************************************************/
HI_S32 SAMPLE_VENC_H265_H264(void)
{
    HI_S32 i;
    HI_S32 s32Ret;
    SIZE_S          stSize[2];
    PIC_SIZE_E      enSize[2]     = {PIC_3840x2160, PIC_720P};
    HI_S32          s32ChnNum     = 2;
    VENC_CHN        VencChn[2]    = {0,1};
    HI_U32          u32Profile[2] = {0,0};
    PAYLOAD_TYPE_E  enPayLoad[2]  = {PT_H265, PT_H264};
    VENC_GOP_MODE_E enGopMode;
    VENC_GOP_ATTR_S stGopAttr;
    SAMPLE_RC_E     enRcMode;
    HI_BOOL         bRcnRefShareBuf = HI_TRUE;

    VI_DEV          ViDev        = 0;
    VI_PIPE         ViPipe       = 0;
    VI_CHN          ViChn        = 0;
    SAMPLE_VI_CONFIG_S stViConfig;

    VPSS_GRP        VpssGrp        = 0;
    VPSS_CHN        VpssChn[2]     = {0,1};
    HI_BOOL         abChnEnable[VPSS_MAX_PHY_CHN_NUM] = {1,1,0};

    HI_U32 u32SupplementConfig = HI_FALSE;

    for(i=0; i<s32ChnNum; i++)
    {
        s32Ret = SAMPLE_COMM_SYS_GetPicSize(enSize[i], &stSize[i]);
        if (HI_SUCCESS != s32Ret)
        {
            SAMPLE_PRT("SAMPLE_COMM_SYS_GetPicSize failed!\n");
            return s32Ret;
        }
    }

    SAMPLE_COMM_VI_GetSensorInfo(&stViConfig);
    if(SAMPLE_SNS_TYPE_BUTT == stViConfig.astViInfo[0].stSnsInfo.enSnsType)
    {
        SAMPLE_PRT("Not set SENSOR%d_TYPE !\n",0);
        return HI_FAILURE;
    }

    s32Ret = SAMPLE_VENC_CheckSensor(stViConfig.astViInfo[0].stSnsInfo.enSnsType,stSize[0]);
    if(s32Ret != HI_SUCCESS)
    {
        s32Ret = SAMPLE_VENC_ModifyResolution(stViConfig.astViInfo[0].stSnsInfo.enSnsType,&enSize[0],&stSize[0]);
        if(s32Ret != HI_SUCCESS)
        {
            return HI_FAILURE;
        }
    }

    stViConfig.s32WorkingViNum       = 1;
    stViConfig.astViInfo[0].stDevInfo.ViDev     = ViDev;
    stViConfig.astViInfo[0].stPipeInfo.aPipe[0] = ViPipe;
    stViConfig.astViInfo[0].stChnInfo.ViChn     = ViChn;
    stViConfig.astViInfo[0].stChnInfo.enDynamicRange = DYNAMIC_RANGE_SDR8;
    stViConfig.astViInfo[0].stChnInfo.enPixFormat    = PIXEL_FORMAT_YVU_SEMIPLANAR_420;

    s32Ret = SAMPLE_VENC_VI_Init(&stViConfig, HI_FALSE,u32SupplementConfig);
    if(s32Ret != HI_SUCCESS)
    {
        SAMPLE_PRT("Init VI err for %#x!\n", s32Ret);
        return HI_FAILURE;
    }

    s32Ret = SAMPLE_VENC_VPSS_Init(VpssGrp,abChnEnable,DYNAMIC_RANGE_SDR8,PIXEL_FORMAT_YVU_SEMIPLANAR_420,stSize,stViConfig.astViInfo[0].stSnsInfo.enSnsType);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("Init VPSS err for %#x!\n", s32Ret);
        goto EXIT_VI_STOP;
    }

    s32Ret = SAMPLE_COMM_VI_Bind_VPSS(ViPipe, ViChn, VpssGrp);
    if(s32Ret != HI_SUCCESS)
    {
        SAMPLE_PRT("VI Bind VPSS err for %#x!\n", s32Ret);
        goto EXIT_VPSS_STOP;
    }

   /******************************************
    start stream venc
    ******************************************/

    enRcMode = SAMPLE_VENC_GetRcMode();

    enGopMode = SAMPLE_VENC_GetGopMode();
    s32Ret = SAMPLE_COMM_VENC_GetGopAttr(enGopMode,&stGopAttr);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("Venc Get GopAttr for %#x!\n", s32Ret);
        goto EXIT_VI_VPSS_UNBIND;
    }

   /***encode h.265 **/
    s32Ret = SAMPLE_COMM_VENC_Start(VencChn[0], enPayLoad[0],enSize[0], enRcMode,u32Profile[0],bRcnRefShareBuf,&stGopAttr);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("Venc Start failed for %#x!\n", s32Ret);
        goto EXIT_VI_VPSS_UNBIND;
    }

    s32Ret = SAMPLE_COMM_VPSS_Bind_VENC(VpssGrp, VpssChn[0],VencChn[0]);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("Venc Get GopAttr failed for %#x!\n", s32Ret);
        goto EXIT_VENC_H265_STOP;
    }

    /***encode h.264 **/
    s32Ret = SAMPLE_COMM_VENC_Start(VencChn[1], enPayLoad[1], enSize[1], enRcMode,u32Profile[1],bRcnRefShareBuf,&stGopAttr);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("Venc Start failed for %#x!\n", s32Ret);
        goto EXIT_VENC_H265_UnBind;
    }

    s32Ret = SAMPLE_COMM_VPSS_Bind_VENC(VpssGrp, VpssChn[1],VencChn[1]);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("Venc bind Vpss failed for %#x!\n", s32Ret);
        goto EXIT_VENC_H264_STOP;
    }

    /******************************************
     stream save process
    ******************************************/
    s32Ret = SAMPLE_COMM_VENC_StartGetStream(VencChn,s32ChnNum);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("Start Venc failed!\n");
        goto EXIT_VENC_H264_UnBind;
    }

    printf("please press twice ENTER to exit this sample\n");
    getchar();
    getchar();

    /******************************************
     exit process
    ******************************************/
    SAMPLE_COMM_VENC_StopGetStream();

EXIT_VENC_H264_UnBind:
    SAMPLE_COMM_VPSS_UnBind_VENC(VpssGrp,VpssChn[1],VencChn[1]);
EXIT_VENC_H264_STOP:
    SAMPLE_COMM_VENC_Stop(VencChn[1]);
EXIT_VENC_H265_UnBind:
    SAMPLE_COMM_VPSS_UnBind_VENC(VpssGrp,VpssChn[0],VencChn[0]);
EXIT_VENC_H265_STOP:
    SAMPLE_COMM_VENC_Stop(VencChn[0]);
EXIT_VI_VPSS_UNBIND:
    SAMPLE_COMM_VI_UnBind_VPSS(ViPipe,ViChn,VpssGrp);
EXIT_VPSS_STOP:
    SAMPLE_COMM_VPSS_Stop(VpssGrp,abChnEnable);
EXIT_VI_STOP:
    SAMPLE_COMM_VI_StopVi(&stViConfig);
    SAMPLE_COMM_SYS_Exit();

    return s32Ret;
}



/******************************************************************************
* function: lowdelay H265e, Channel resolution adaptable with sensor
******************************************************************************/
HI_S32 SAMPLE_VENC_LOW_DELAY(void)
{
    HI_S32 i;
    HI_S32 s32Ret;
    SIZE_S          stSize[2];
    PIC_SIZE_E      enSize[2]     = {PIC_3840x2160, PIC_720P};

    HI_S32          s32ChnNum     = 1;
    VENC_CHN        VencChn[2]    = {0,1};
    HI_U32          u32Profile[2] = {0,0};
    PAYLOAD_TYPE_E  enPayLoad[2]  = {PT_H265, PT_H264};
    VENC_GOP_MODE_E enGopMode;
    VENC_GOP_ATTR_S stGopAttr;
    SAMPLE_RC_E     enRcMode;
    HI_BOOL         bRcnRefShareBuf = HI_TRUE;

    VI_DEV          ViDev        = 0;
    VI_PIPE         ViPipe       = 0;
    VI_CHN          ViChn        = 0;
    SAMPLE_VI_CONFIG_S stViConfig;

    VPSS_GRP        VpssGrp        = 0;
    VPSS_CHN        VpssChn[2]     = {0,1};
    HI_BOOL         abChnEnable[4] = {1,0,0,0};
    VPSS_LOW_DELAY_INFO_S  stLowDelayInfo;

    HI_U32 u32SupplementConfig = HI_FALSE;

    for(i=0; i<s32ChnNum; i++)
    {
        s32Ret = SAMPLE_COMM_SYS_GetPicSize(enSize[i], &stSize[i]);
        if (HI_SUCCESS != s32Ret)
        {
            SAMPLE_PRT("SAMPLE_COMM_SYS_GetPicSize failed!\n");
            return s32Ret;
        }
    }

    SAMPLE_COMM_VI_GetSensorInfo(&stViConfig);
    if(SAMPLE_SNS_TYPE_BUTT == stViConfig.astViInfo[0].stSnsInfo.enSnsType)
    {
        SAMPLE_PRT("Not set SENSOR%d_TYPE !\n",0);
        return HI_FAILURE;
    }


    s32Ret = SAMPLE_VENC_CheckSensor(stViConfig.astViInfo[0].stSnsInfo.enSnsType,stSize[0]);
    if(s32Ret != HI_SUCCESS)
    {
        s32Ret = SAMPLE_VENC_ModifyResolution(stViConfig.astViInfo[0].stSnsInfo.enSnsType,&enSize[0],&stSize[0]);
        if(s32Ret != HI_SUCCESS)
        {
            return HI_FAILURE;
        }
    }

    stViConfig.s32WorkingViNum                  = 1;
    stViConfig.astViInfo[0].stDevInfo.ViDev     = ViDev;
    stViConfig.astViInfo[0].stPipeInfo.aPipe[0] = ViPipe;
    stViConfig.astViInfo[0].stChnInfo.ViChn     = ViChn;
    stViConfig.astViInfo[0].stChnInfo.enDynamicRange = DYNAMIC_RANGE_SDR8;
    stViConfig.astViInfo[0].stChnInfo.enPixFormat    = PIXEL_FORMAT_YVU_SEMIPLANAR_420;
    s32Ret = SAMPLE_VENC_VI_Init(&stViConfig, HI_TRUE,u32SupplementConfig);
    if(s32Ret != HI_SUCCESS)
    {
        SAMPLE_PRT("Init VI err for %#x!\n", s32Ret);
        return HI_FAILURE;
    }


    s32Ret = SAMPLE_VENC_VPSS_Init(VpssGrp,abChnEnable,DYNAMIC_RANGE_SDR8,PIXEL_FORMAT_YVU_SEMIPLANAR_420,stSize,stViConfig.astViInfo[0].stSnsInfo.enSnsType);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("Init VPSS err for %#x!\n", s32Ret);
        goto EXIT_VI_STOP;
    }

    stLowDelayInfo.bEnable = HI_TRUE;
    stLowDelayInfo.u32LineCnt = stSize[0].u32Height/2;
    s32Ret = HI_MPI_VPSS_SetLowDelayAttr(VpssGrp,VpssChn[0],&stLowDelayInfo);
    if(s32Ret != HI_SUCCESS)
    {
        SAMPLE_PRT("Set LowDelay Attr for %#x!\n", s32Ret);
        goto EXIT_VPSS_STOP;
    }

    s32Ret = SAMPLE_COMM_VI_Bind_VPSS(ViPipe, ViChn, VpssGrp);
    if(s32Ret != HI_SUCCESS)
    {
        SAMPLE_PRT("VI Bind VPSS err for %#x!\n", s32Ret);
        goto EXIT_VPSS_STOP;
    }

   /******************************************
     step 4: start stream venc
    ******************************************/

    enRcMode = SAMPLE_VENC_GetRcMode();

    enGopMode = SAMPLE_VENC_GetGopMode();
    s32Ret = SAMPLE_COMM_VENC_GetGopAttr(enGopMode,&stGopAttr);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("Venc Get GopAttr for %#x!\n", s32Ret);
        goto EXIT_VI_VPSS_UNBIND;
    }
    if(VENC_GOPMODE_BIPREDB == enGopMode)
    {
        SAMPLE_PRT("BIPREDB and ADVSMARTP not support lowdelay!\n");
        goto EXIT_VI_VPSS_UNBIND;
    }
   /***encode h.265 **/
    s32Ret = SAMPLE_COMM_VENC_Start(VencChn[0], enPayLoad[0],enSize[0], enRcMode,u32Profile[0],bRcnRefShareBuf,&stGopAttr);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("Venc Start failed for %#x!\n", s32Ret);
        goto EXIT_VI_VPSS_UNBIND;
    }

    s32Ret = SAMPLE_COMM_VPSS_Bind_VENC(VpssGrp, VpssChn[0],VencChn[0]);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("Venc Get GopAttr failed for %#x!\n", s32Ret);
        goto EXIT_VENC_H265_STOP;
    }


    /******************************************
     stream save process
    ******************************************/
    s32Ret = SAMPLE_COMM_VENC_StartGetStream(VencChn,s32ChnNum);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("Start Venc failed!\n");
        goto EXIT_VENC_H265_UnBind;
    }

    printf("please press twice ENTER to exit this sample\n");
    getchar();
    getchar();

    /******************************************
     exit process
    ******************************************/
    SAMPLE_COMM_VENC_StopGetStream();


EXIT_VENC_H265_UnBind:
    SAMPLE_COMM_VPSS_UnBind_VENC(VpssGrp,VpssChn[0],VencChn[0]);
EXIT_VENC_H265_STOP:
    SAMPLE_COMM_VENC_Stop(VencChn[0]);
EXIT_VI_VPSS_UNBIND:
    SAMPLE_COMM_VI_UnBind_VPSS(ViPipe,ViChn,VpssGrp);
EXIT_VPSS_STOP:
    SAMPLE_COMM_VPSS_Stop(VpssGrp,abChnEnable);
EXIT_VI_STOP:
    SAMPLE_COMM_VI_StopVi(&stViConfig);
    SAMPLE_COMM_SYS_Exit();

    return s32Ret;
}


/******************************************************************************
* function: IntraRefresh, H.265e + H264e@720P,H.265 Channel resolution adaptable with sensor
******************************************************************************/
HI_S32 SAMPLE_VENC_IntraRefresh(void)
{
    HI_S32 i;
    HI_S32 s32Ret;
    SIZE_S          stSize[2];
    PIC_SIZE_E      enSize[2]     = {PIC_3840x2160, PIC_720P};

    HI_S32          s32ChnNum     = 2;
    VENC_CHN        VencChn[2]    = {0,1};
    HI_U32          u32Profile[2] = {0,0};
    PAYLOAD_TYPE_E  enPayLoad[2]  = {PT_H265, PT_H264};
    VENC_GOP_MODE_E enGopMode     = VENC_GOPMODE_NORMALP;
    VENC_GOP_ATTR_S stGopAttr;
    SAMPLE_RC_E     enRcMode;
    HI_BOOL         bRcnRefShareBuf = HI_TRUE;

    VENC_INTRA_REFRESH_S  stIntraRefresh;
    VENC_INTRA_REFRESH_MODE_E   enIntraRefreshMode;

    VI_DEV          ViDev        = 0;
    VI_PIPE         ViPipe       = 0;
    VI_CHN          ViChn        = 0;
    SAMPLE_VI_CONFIG_S stViConfig;

    VPSS_GRP        VpssGrp        = 0;
    VPSS_CHN        VpssChn[2]     = {0,1};
    HI_BOOL         abChnEnable[4] = {1,1,0,0};

    HI_U32 u32SupplementConfig = HI_FALSE;

    for(i=0; i<s32ChnNum; i++)
    {
        s32Ret = SAMPLE_COMM_SYS_GetPicSize(enSize[i], &stSize[i]);
        if (HI_SUCCESS != s32Ret)
        {
            SAMPLE_PRT("SAMPLE_COMM_SYS_GetPicSize failed!\n");
            return s32Ret;
        }
    }

    SAMPLE_COMM_VI_GetSensorInfo(&stViConfig);
    if(SAMPLE_SNS_TYPE_BUTT == stViConfig.astViInfo[0].stSnsInfo.enSnsType)
    {
        SAMPLE_PRT("Not set SENSOR%d_TYPE !\n",0);
        return HI_FAILURE;
    }

    s32Ret = SAMPLE_VENC_CheckSensor(stViConfig.astViInfo[0].stSnsInfo.enSnsType,stSize[0]);
    if(s32Ret != HI_SUCCESS)
    {
        s32Ret = SAMPLE_VENC_ModifyResolution(stViConfig.astViInfo[0].stSnsInfo.enSnsType,&enSize[0],&stSize[0]);
        if(s32Ret != HI_SUCCESS)
        {
            return HI_FAILURE;
        }
    }


    stViConfig.s32WorkingViNum       = 1;
    stViConfig.astViInfo[0].stDevInfo.ViDev     = ViDev;
    stViConfig.astViInfo[0].stPipeInfo.aPipe[0] = ViPipe;
    stViConfig.astViInfo[0].stChnInfo.ViChn     = ViChn;
    stViConfig.astViInfo[0].stChnInfo.enDynamicRange = DYNAMIC_RANGE_SDR8;
    stViConfig.astViInfo[0].stChnInfo.enPixFormat    = PIXEL_FORMAT_YVU_SEMIPLANAR_420;
    s32Ret = SAMPLE_VENC_VI_Init(&stViConfig, HI_FALSE,u32SupplementConfig);
    if(s32Ret != HI_SUCCESS)
    {
        SAMPLE_PRT("Init VI err for %#x!\n", s32Ret);
        return HI_FAILURE;
    }

    s32Ret = SAMPLE_VENC_VPSS_Init(VpssGrp,abChnEnable,DYNAMIC_RANGE_SDR8,PIXEL_FORMAT_YVU_SEMIPLANAR_420,stSize,stViConfig.astViInfo[0].stSnsInfo.enSnsType);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("Init VPSS err for %#x!\n", s32Ret);
        goto EXIT_VI_STOP;
    }

    s32Ret = SAMPLE_COMM_VI_Bind_VPSS(ViPipe, ViChn, VpssGrp);
    if(s32Ret != HI_SUCCESS)
    {
        SAMPLE_PRT("VI Bind VPSS err for %#x!\n", s32Ret);
        goto EXIT_VPSS_STOP;
    }

   /******************************************
     start stream venc
    ******************************************/

    enRcMode = SAMPLE_VENC_GetRcMode();
    enIntraRefreshMode = SAMPLE_VENC_GetIntraRefreshMode();

    s32Ret = SAMPLE_COMM_VENC_GetGopAttr(enGopMode,&stGopAttr);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("Venc Get GopAttr for %#x!\n", s32Ret);
        goto EXIT_VI_VPSS_UNBIND;
    }

   /***encode h.265 **/
    s32Ret = SAMPLE_COMM_VENC_Start(VencChn[0], enPayLoad[0],enSize[0], enRcMode,u32Profile[0],bRcnRefShareBuf,&stGopAttr);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("Venc Start failed for %#x!\n", s32Ret);
        goto EXIT_VI_VPSS_UNBIND;
    }

    s32Ret = SAMPLE_COMM_VPSS_Bind_VENC(VpssGrp, VpssChn[0],VencChn[0]);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("Venc Get GopAttr failed for %#x!\n", s32Ret);
        goto EXIT_VENC_H265_STOP;
    }

    s32Ret = HI_MPI_VENC_GetIntraRefresh(VencChn[0],&stIntraRefresh);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("Get Intra Refresh failed for %#x!\n", s32Ret);
        goto EXIT_VENC_H265_UnBind;
    }

    stIntraRefresh.bRefreshEnable = HI_TRUE;
    stIntraRefresh.enIntraRefreshMode = enIntraRefreshMode;
    if(INTRA_REFRESH_ROW == enIntraRefreshMode)
    {
        stIntraRefresh.u32RefreshNum = (stSize[0].u32Height + 63)>>8;
    }
    else
    {
        stIntraRefresh.u32RefreshNum = (stSize[0].u32Width+ 63)>>8;
    }
    stIntraRefresh.u32ReqIQp = 30;
    s32Ret = HI_MPI_VENC_SetIntraRefresh(VencChn[0],&stIntraRefresh);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("Set Intra Refresh failed for %#x!\n", s32Ret);
        goto EXIT_VENC_H265_UnBind;
    }

    /***encode h.264 **/
    s32Ret = SAMPLE_COMM_VENC_Start(VencChn[1], enPayLoad[1], enSize[1], enRcMode,u32Profile[1],bRcnRefShareBuf,&stGopAttr);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("Venc Start failed for %#x!\n", s32Ret);
        goto EXIT_VENC_H265_UnBind;
    }

    s32Ret = SAMPLE_COMM_VPSS_Bind_VENC(VpssGrp, VpssChn[1],VencChn[1]);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("Venc bind Vpss failed for %#x!\n", s32Ret);
        goto EXIT_VENC_H264_STOP;
    }

    s32Ret = HI_MPI_VENC_GetIntraRefresh(VencChn[1],&stIntraRefresh);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("Get Intra Refresh failed for %#x!\n", s32Ret);
        goto EXIT_VENC_H264_UnBind;
    }

    stIntraRefresh.bRefreshEnable = HI_TRUE;
    stIntraRefresh.enIntraRefreshMode = enIntraRefreshMode;
    if(INTRA_REFRESH_ROW == enIntraRefreshMode)
    {
        stIntraRefresh.u32RefreshNum = (stSize[0].u32Height + 15)>>8;
    }
    else
    {
        stIntraRefresh.u32RefreshNum = (stSize[0].u32Width+ 15)>>8;
    }

    stIntraRefresh.u32ReqIQp = 30;
    s32Ret = HI_MPI_VENC_SetIntraRefresh(VencChn[1],&stIntraRefresh);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("Set Intra Refresh failed for %#x!\n", s32Ret);
        goto EXIT_VENC_H264_UnBind;
    }

    /******************************************
     stream save process
    ******************************************/
    s32Ret = SAMPLE_COMM_VENC_StartGetStream(VencChn,s32ChnNum);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("Start Venc failed!\n");
        goto EXIT_VENC_H264_UnBind;
    }

    printf("please press twice ENTER to exit this sample\n");
    getchar();
    getchar();

    /******************************************
     exit process
    ******************************************/
    SAMPLE_COMM_VENC_StopGetStream();

EXIT_VENC_H264_UnBind:
    SAMPLE_COMM_VPSS_UnBind_VENC(VpssGrp,VpssChn[1],VencChn[1]);
EXIT_VENC_H264_STOP:
    SAMPLE_COMM_VENC_Stop(VencChn[1]);
EXIT_VENC_H265_UnBind:
    SAMPLE_COMM_VPSS_UnBind_VENC(VpssGrp,VpssChn[0],VencChn[0]);
EXIT_VENC_H265_STOP:
    SAMPLE_COMM_VENC_Stop(VencChn[0]);
EXIT_VI_VPSS_UNBIND:
    SAMPLE_COMM_VI_UnBind_VPSS(ViPipe,ViChn,VpssGrp);
EXIT_VPSS_STOP:
    SAMPLE_COMM_VPSS_Stop(VpssGrp,abChnEnable);
EXIT_VI_STOP:
    SAMPLE_COMM_VI_StopVi(&stViConfig);
    SAMPLE_COMM_SYS_Exit();

    return s32Ret;
}


/******************************************************************************
* function: RoiBgFrameRate, H.265 + H.264@720P,H.265 Channel resolution adaptable with sensor
******************************************************************************/
HI_S32 SAMPLE_VENC_ROIBG(void)
{
    HI_S32 i;
    HI_S32 s32Ret;
    SIZE_S          stSize[2];
    PIC_SIZE_E      enSize[2]     = {PIC_3840x2160, PIC_720P};

    HI_S32          s32ChnNum     = 2;
    VENC_CHN        VencChn[2]    = {0,1};
    HI_U32          u32Profile[2] = {0,0};
    PAYLOAD_TYPE_E  enPayLoad[2]  = {PT_H265, PT_H264};
    VENC_GOP_MODE_E enGopMode     = VENC_GOPMODE_NORMALP;
    VENC_GOP_ATTR_S stGopAttr;
    SAMPLE_RC_E     enRcMode;
    HI_BOOL         bRcnRefShareBuf = HI_TRUE;

    VENC_ROI_ATTR_S  stRoiAttr;
    VENC_ROIBG_FRAME_RATE_S  stRoiBgFrameRate;

    VI_DEV          ViDev        = 0;
    VI_PIPE         ViPipe       = 0;
    VI_CHN          ViChn        = 0;
    SAMPLE_VI_CONFIG_S stViConfig;

    VPSS_GRP        VpssGrp        = 0;
    VPSS_CHN        VpssChn[2]     = {0,1};
    HI_BOOL         abChnEnable[4] = {1,1,0,0};

    HI_U32 u32SupplementConfig = HI_FALSE;

    for(i=0; i<s32ChnNum; i++)
    {
        s32Ret = SAMPLE_COMM_SYS_GetPicSize(enSize[i], &stSize[i]);
        if (HI_SUCCESS != s32Ret)
        {
            SAMPLE_PRT("SAMPLE_COMM_SYS_GetPicSize failed!\n");
            return s32Ret;
        }
    }

    SAMPLE_COMM_VI_GetSensorInfo(&stViConfig);
    if(SAMPLE_SNS_TYPE_BUTT == stViConfig.astViInfo[0].stSnsInfo.enSnsType)
    {
        SAMPLE_PRT("Not set SENSOR%d_TYPE !\n",0);
        return HI_FAILURE;
    }

    s32Ret = SAMPLE_VENC_CheckSensor(stViConfig.astViInfo[0].stSnsInfo.enSnsType,stSize[0]);
    if(s32Ret != HI_SUCCESS)
    {
        s32Ret = SAMPLE_VENC_ModifyResolution(stViConfig.astViInfo[0].stSnsInfo.enSnsType,&enSize[0],&stSize[0]);
        if(s32Ret != HI_SUCCESS)
        {
            return HI_FAILURE;
        }
    }

    stViConfig.s32WorkingViNum       = 1;
    stViConfig.astViInfo[0].stDevInfo.ViDev     = ViDev;
    stViConfig.astViInfo[0].stPipeInfo.aPipe[0] = ViPipe;
    stViConfig.astViInfo[0].stChnInfo.ViChn     = ViChn;
    stViConfig.astViInfo[0].stChnInfo.enDynamicRange = DYNAMIC_RANGE_SDR8;
    stViConfig.astViInfo[0].stChnInfo.enPixFormat    = PIXEL_FORMAT_YVU_SEMIPLANAR_420;
    s32Ret = SAMPLE_VENC_VI_Init(&stViConfig, HI_FALSE,u32SupplementConfig);
    if(s32Ret != HI_SUCCESS)
    {
        SAMPLE_PRT("Init VI err for %#x!\n", s32Ret);
        return HI_FAILURE;
    }

    s32Ret = SAMPLE_VENC_VPSS_Init(VpssGrp,abChnEnable,DYNAMIC_RANGE_SDR8,PIXEL_FORMAT_YVU_SEMIPLANAR_420,stSize,stViConfig.astViInfo[0].stSnsInfo.enSnsType);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("Init VPSS err for %#x!\n", s32Ret);
        goto EXIT_VI_STOP;
    }

    s32Ret = SAMPLE_COMM_VI_Bind_VPSS(ViPipe, ViChn, VpssGrp);
    if(s32Ret != HI_SUCCESS)
    {
        SAMPLE_PRT("VI Bind VPSS err for %#x!\n", s32Ret);
        goto EXIT_VPSS_STOP;
    }

   /******************************************
     start stream venc
    ******************************************/

    enRcMode = SAMPLE_VENC_GetRcMode();

    s32Ret = SAMPLE_COMM_VENC_GetGopAttr(enGopMode,&stGopAttr);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("Venc Get GopAttr for %#x!\n", s32Ret);
        goto EXIT_VI_VPSS_UNBIND;
    }

   /***encode h.265 **/
    s32Ret = SAMPLE_COMM_VENC_Start(VencChn[0], enPayLoad[0],enSize[0], enRcMode,u32Profile[0],bRcnRefShareBuf,&stGopAttr);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("Venc Start failed for %#x!\n", s32Ret);
        goto EXIT_VI_VPSS_UNBIND;
    }

    s32Ret = SAMPLE_COMM_VPSS_Bind_VENC(VpssGrp, VpssChn[0],VencChn[0]);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("Venc Get GopAttr failed for %#x!\n", s32Ret);
        goto EXIT_VENC_H265_STOP;
    }

    s32Ret = HI_MPI_VENC_GetRoiAttr(VencChn[0], 0, &stRoiAttr);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("Get Roi Attr failed for %#x!\n", s32Ret);
        goto EXIT_VENC_H265_UnBind;
    }

    stRoiAttr.bAbsQp   = HI_TRUE;
    stRoiAttr.bEnable  = HI_TRUE;
    stRoiAttr.s32Qp    = 30;
    stRoiAttr.u32Index = 0;
    stRoiAttr.stRect.s32X = 64;
    stRoiAttr.stRect.s32Y = 64;
    stRoiAttr.stRect.u32Height = 256;
    stRoiAttr.stRect.u32Width = 256;

    s32Ret = HI_MPI_VENC_SetRoiAttr(VencChn[0],&stRoiAttr);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("Set Roi Attr failed for %#x!\n", s32Ret);
        goto EXIT_VENC_H265_UnBind;
    }

    s32Ret = HI_MPI_VENC_GetRoiBgFrameRate(VencChn[0],&stRoiBgFrameRate);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("Get Roi BgFrameRate failed for %#x!\n", s32Ret);
        goto EXIT_VENC_H265_UnBind;
    }

    stRoiBgFrameRate.s32SrcFrmRate = 30;
    stRoiBgFrameRate.s32DstFrmRate = 15;

    s32Ret = HI_MPI_VENC_SetRoiBgFrameRate(VencChn[0],&stRoiBgFrameRate);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("Set Roi BgFrameRate failed for %#x!\n", s32Ret);
        goto EXIT_VENC_H265_UnBind;
    }


    /***encode h.264 **/
    s32Ret = SAMPLE_COMM_VENC_Start(VencChn[1], enPayLoad[1], enSize[1], enRcMode,u32Profile[1],bRcnRefShareBuf,&stGopAttr);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("Venc Start failed for %#x!\n", s32Ret);
        goto EXIT_VENC_H265_UnBind;
    }

    s32Ret = SAMPLE_COMM_VPSS_Bind_VENC(VpssGrp, VpssChn[1],VencChn[1]);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("Venc bind Vpss failed for %#x!\n", s32Ret);
        goto EXIT_VENC_H264_STOP;
    }

    s32Ret = HI_MPI_VENC_GetRoiAttr(VencChn[1], 0, &stRoiAttr);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("Get Roi Attr failed for %#x!\n", s32Ret);
        goto EXIT_VENC_H264_UnBind;
    }

    stRoiAttr.bAbsQp   = HI_TRUE;
    stRoiAttr.bEnable  = HI_TRUE;
    stRoiAttr.s32Qp    = 30;
    stRoiAttr.u32Index = 0;
    stRoiAttr.stRect.s32X = 64;
    stRoiAttr.stRect.s32Y = 64;
    stRoiAttr.stRect.u32Height = 256;
    stRoiAttr.stRect.u32Width = 256;

    s32Ret = HI_MPI_VENC_SetRoiAttr(VencChn[1],&stRoiAttr);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("Set Roi Attr failed for %#x!\n", s32Ret);
        goto EXIT_VENC_H264_UnBind;
    }

    s32Ret = HI_MPI_VENC_GetRoiBgFrameRate(VencChn[1],&stRoiBgFrameRate);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("Get Roi BgFrameRate failed for %#x!\n", s32Ret);
        goto EXIT_VENC_H264_UnBind;
    }

    stRoiBgFrameRate.s32SrcFrmRate = 30;
    stRoiBgFrameRate.s32DstFrmRate = 15;

    s32Ret = HI_MPI_VENC_SetRoiBgFrameRate(VencChn[1],&stRoiBgFrameRate);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("Set Roi BgFrameRate failed for %#x!\n", s32Ret);
        goto EXIT_VENC_H264_UnBind;
    }

    /******************************************
     stream save process
    ******************************************/
    s32Ret = SAMPLE_COMM_VENC_StartGetStream(VencChn,s32ChnNum);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("Start Venc failed!\n");
        goto EXIT_VENC_H264_UnBind;
    }

    printf("please press twice ENTER to exit this sample\n");
    getchar();
    getchar();

    /******************************************
     exit process
    ******************************************/
    SAMPLE_COMM_VENC_StopGetStream();

EXIT_VENC_H264_UnBind:
    SAMPLE_COMM_VPSS_UnBind_VENC(VpssGrp,VpssChn[1],VencChn[1]);
EXIT_VENC_H264_STOP:
    SAMPLE_COMM_VENC_Stop(VencChn[1]);
EXIT_VENC_H265_UnBind:
    SAMPLE_COMM_VPSS_UnBind_VENC(VpssGrp,VpssChn[0],VencChn[0]);
EXIT_VENC_H265_STOP:
    SAMPLE_COMM_VENC_Stop(VencChn[0]);
EXIT_VI_VPSS_UNBIND:
    SAMPLE_COMM_VI_UnBind_VPSS(ViPipe,ViChn,VpssGrp);
EXIT_VPSS_STOP:
    SAMPLE_COMM_VPSS_Stop(VpssGrp,abChnEnable);
EXIT_VI_STOP:
    SAMPLE_COMM_VI_StopVi(&stViConfig);
    SAMPLE_COMM_SYS_Exit();

    return s32Ret;
}

/******************************************************************************
* function: DeBreathEffect, H.265e + H264e@720P, H.265 Channel resolution adaptable with sensor
******************************************************************************/
HI_S32 SAMPLE_VENC_DeBreathEffect(void)
{
    HI_S32 i;
    HI_S32 s32Ret;
    SIZE_S          stSize[2];
    PIC_SIZE_E      enSize[2]     = {PIC_3840x2160, PIC_720P};

    HI_S32          s32ChnNum     = 2;
    VENC_CHN        VencChn[2]    = {0,1};
    HI_U32          u32Profile[2] = {0,0};
    PAYLOAD_TYPE_E  enPayLoad[2]  = {PT_H265, PT_H264};
    VENC_GOP_MODE_E enGopMode     = VENC_GOPMODE_NORMALP;
    VENC_GOP_ATTR_S stGopAttr;
    SAMPLE_RC_E     enRcMode;
    HI_BOOL         bRcnRefShareBuf = HI_FALSE;

    VENC_DEBREATHEFFECT_S  stDeBreathEffect;

    VI_DEV          ViDev        = 0;
    VI_PIPE         ViPipe       = 0;
    VI_CHN          ViChn        = 0;
    SAMPLE_VI_CONFIG_S stViConfig;

    VPSS_GRP        VpssGrp        = 0;
    VPSS_CHN        VpssChn[2]     = {0,1};
    HI_BOOL         abChnEnable[4] = {1,1,0,0};

    HI_U32 u32SupplementConfig = HI_FALSE;

    for(i=0; i<s32ChnNum; i++)
    {
        s32Ret = SAMPLE_COMM_SYS_GetPicSize(enSize[i], &stSize[i]);
        if (HI_SUCCESS != s32Ret)
        {
            SAMPLE_PRT("SAMPLE_COMM_SYS_GetPicSize failed!\n");
            return s32Ret;
        }
    }

    SAMPLE_COMM_VI_GetSensorInfo(&stViConfig);
    if(SAMPLE_SNS_TYPE_BUTT == stViConfig.astViInfo[0].stSnsInfo.enSnsType)
    {
        SAMPLE_PRT("Not set SENSOR%d_TYPE !\n",0);
        return HI_FAILURE;
    }

    s32Ret = SAMPLE_VENC_CheckSensor(stViConfig.astViInfo[0].stSnsInfo.enSnsType,stSize[0]);
    if(s32Ret != HI_SUCCESS)
    {
        s32Ret = SAMPLE_VENC_ModifyResolution(stViConfig.astViInfo[0].stSnsInfo.enSnsType,&enSize[0],&stSize[0]);
        if(s32Ret != HI_SUCCESS)
        {
            return HI_FAILURE;
        }
    }

    stViConfig.s32WorkingViNum       = 1;
    stViConfig.astViInfo[0].stDevInfo.ViDev     = ViDev;
    stViConfig.astViInfo[0].stPipeInfo.aPipe[0] = ViPipe;
    stViConfig.astViInfo[0].stChnInfo.ViChn     = ViChn;
    stViConfig.astViInfo[0].stChnInfo.enDynamicRange = DYNAMIC_RANGE_SDR8;
    stViConfig.astViInfo[0].stChnInfo.enPixFormat    = PIXEL_FORMAT_YVU_SEMIPLANAR_420;
    s32Ret = SAMPLE_VENC_VI_Init(&stViConfig, HI_FALSE,u32SupplementConfig);
    if(s32Ret != HI_SUCCESS)
    {
        SAMPLE_PRT("Init VI err for %#x!\n", s32Ret);
        return HI_FAILURE;
    }

    s32Ret = SAMPLE_VENC_VPSS_Init(VpssGrp,abChnEnable,DYNAMIC_RANGE_SDR8,PIXEL_FORMAT_YVU_SEMIPLANAR_420,stSize,stViConfig.astViInfo[0].stSnsInfo.enSnsType);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("Init VPSS err for %#x!\n", s32Ret);
        goto EXIT_VI_STOP;
    }

    s32Ret = SAMPLE_COMM_VI_Bind_VPSS(ViPipe, ViChn, VpssGrp);
    if(s32Ret != HI_SUCCESS)
    {
        SAMPLE_PRT("VI Bind VPSS err for %#x!\n", s32Ret);
        goto EXIT_VPSS_STOP;
    }

   /******************************************
     start stream venc
    ******************************************/

    enRcMode = SAMPLE_VENC_GetRcMode();

    enGopMode = SAMPLE_VENC_GetGopMode();

    s32Ret = SAMPLE_COMM_VENC_GetGopAttr(enGopMode,&stGopAttr);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("Venc Get GopAttr for %#x!\n", s32Ret);
        goto EXIT_VI_VPSS_UNBIND;
    }

   /***encode h.265 **/
    s32Ret = SAMPLE_COMM_VENC_Start(VencChn[0], enPayLoad[0],enSize[0], enRcMode,u32Profile[0],bRcnRefShareBuf,&stGopAttr);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("Venc Start failed for %#x!\n", s32Ret);
        goto EXIT_VI_VPSS_UNBIND;
    }

    s32Ret = SAMPLE_COMM_VPSS_Bind_VENC(VpssGrp, VpssChn[0],VencChn[0]);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("Venc Get GopAttr failed for %#x!\n", s32Ret);
        goto EXIT_VENC_H265_STOP;
    }

    s32Ret = HI_MPI_VENC_GetDeBreathEffect(VencChn[0],&stDeBreathEffect);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("Get  DeBreathEffect failed for %#x!\n", s32Ret);
        goto EXIT_VENC_H265_UnBind;
    }

    stDeBreathEffect.bEnable = HI_TRUE;
    s32Ret = HI_MPI_VENC_SetDeBreathEffect(VencChn[0],&stDeBreathEffect);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("Set DeBreathEffect failed for %#x!\n", s32Ret);
        goto EXIT_VENC_H265_UnBind;
    }

    /***encode h.264 **/
    s32Ret = SAMPLE_COMM_VENC_Start(VencChn[1], enPayLoad[1], enSize[1], enRcMode,u32Profile[1],bRcnRefShareBuf,&stGopAttr);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("Venc Start failed for %#x!\n", s32Ret);
        goto EXIT_VENC_H265_UnBind;
    }

    s32Ret = SAMPLE_COMM_VPSS_Bind_VENC(VpssGrp, VpssChn[1],VencChn[1]);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("Venc bind Vpss failed for %#x!\n", s32Ret);
        goto EXIT_VENC_H264_STOP;
    }

    s32Ret = HI_MPI_VENC_GetDeBreathEffect(VencChn[1],&stDeBreathEffect);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("Get DeBreathEffect failed for %#x!\n", s32Ret);
        goto EXIT_VENC_H264_UnBind;
    }

    stDeBreathEffect.bEnable = HI_TRUE;
    s32Ret = HI_MPI_VENC_SetDeBreathEffect(VencChn[1],&stDeBreathEffect);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("Set DeBreathEffect failed for %#x!\n", s32Ret);
        goto EXIT_VENC_H264_UnBind;
    }

    /******************************************
     stream save process
    ******************************************/
    s32Ret = SAMPLE_COMM_VENC_StartGetStream(VencChn,s32ChnNum);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("Start Venc failed!\n");
        goto EXIT_VENC_H264_UnBind;
    }

    printf("please press twice ENTER to exit this sample\n");
    getchar();
    getchar();

    /******************************************
     exit process
    ******************************************/
    SAMPLE_COMM_VENC_StopGetStream();

EXIT_VENC_H264_UnBind:
    SAMPLE_COMM_VPSS_UnBind_VENC(VpssGrp,VpssChn[1],VencChn[1]);
EXIT_VENC_H264_STOP:
    SAMPLE_COMM_VENC_Stop(VencChn[1]);
EXIT_VENC_H265_UnBind:
    SAMPLE_COMM_VPSS_UnBind_VENC(VpssGrp,VpssChn[0],VencChn[0]);
EXIT_VENC_H265_STOP:
    SAMPLE_COMM_VENC_Stop(VencChn[0]);
EXIT_VI_VPSS_UNBIND:
    SAMPLE_COMM_VI_UnBind_VPSS(ViPipe,ViChn,VpssGrp);
EXIT_VPSS_STOP:
    SAMPLE_COMM_VPSS_Stop(VpssGrp,abChnEnable);
EXIT_VI_STOP:
    SAMPLE_COMM_VI_StopVi(&stViConfig);
    SAMPLE_COMM_SYS_Exit();

    return s32Ret;
}

/******************************************************************************
* function: SVC-T,H.264, Channel resolution adaptable with sensor
******************************************************************************/
HI_S32 SAMPLE_VENC_SVC_H264(void)
{
    HI_S32 i;
    HI_S32 s32Ret;
    SIZE_S          stSize[2];
    PIC_SIZE_E      enSize[2]     = {PIC_3840x2160};

    HI_S32          s32ChnNum     = 1;
    VENC_CHN        VencChn[2]    = {0,1};
    HI_U32          u32Profile[2] = {3,0};
    PAYLOAD_TYPE_E  enPayLoad[2]  = {PT_H264};
    VENC_GOP_MODE_E enGopMode     = VENC_GOPMODE_NORMALP;
    VENC_GOP_ATTR_S stGopAttr;
    SAMPLE_RC_E     enRcMode;
    HI_BOOL         bRcnRefShareBuf = HI_TRUE;

    VI_DEV          ViDev        = 0;
    VI_PIPE         ViPipe       = 0;
    VI_CHN          ViChn        = 0;
    SAMPLE_VI_CONFIG_S stViConfig;

    VPSS_GRP        VpssGrp        = 0;
    VPSS_CHN        VpssChn[2]     = {0,1};
    HI_BOOL         abChnEnable[4] = {1,0,0,0};

    HI_U32 u32SupplementConfig = HI_FALSE;

    for(i=0; i<s32ChnNum; i++)
    {
        s32Ret = SAMPLE_COMM_SYS_GetPicSize(enSize[i], &stSize[i]);
        if (HI_SUCCESS != s32Ret)
        {
            SAMPLE_PRT("SAMPLE_COMM_SYS_GetPicSize failed!\n");
            return s32Ret;
        }
    }

    SAMPLE_COMM_VI_GetSensorInfo(&stViConfig);
    if(SAMPLE_SNS_TYPE_BUTT == stViConfig.astViInfo[0].stSnsInfo.enSnsType)
    {
        SAMPLE_PRT("Not set SENSOR%d_TYPE !\n",0);
        return HI_FAILURE;
    }

    s32Ret = SAMPLE_VENC_CheckSensor(stViConfig.astViInfo[0].stSnsInfo.enSnsType,stSize[0]);
    if(s32Ret != HI_SUCCESS)
    {
        s32Ret = SAMPLE_VENC_ModifyResolution(stViConfig.astViInfo[0].stSnsInfo.enSnsType,&enSize[0],&stSize[0]);
        if(s32Ret != HI_SUCCESS)
        {
            return HI_FAILURE;
        }
    }

    stViConfig.s32WorkingViNum       = 1;
    stViConfig.astViInfo[0].stDevInfo.ViDev     = ViDev;
    stViConfig.astViInfo[0].stPipeInfo.aPipe[0] = ViPipe;
    stViConfig.astViInfo[0].stChnInfo.ViChn     = ViChn;
    stViConfig.astViInfo[0].stChnInfo.enDynamicRange = DYNAMIC_RANGE_SDR8;
    stViConfig.astViInfo[0].stChnInfo.enPixFormat    = PIXEL_FORMAT_YVU_SEMIPLANAR_420;
    s32Ret = SAMPLE_VENC_VI_Init(&stViConfig, HI_FALSE,u32SupplementConfig);
    if(s32Ret != HI_SUCCESS)
    {
        SAMPLE_PRT("Init VI err for %#x!\n", s32Ret);
        return HI_FAILURE;
    }

    s32Ret = SAMPLE_VENC_VPSS_Init(VpssGrp,abChnEnable,DYNAMIC_RANGE_SDR8,PIXEL_FORMAT_YVU_SEMIPLANAR_420,stSize,stViConfig.astViInfo[0].stSnsInfo.enSnsType);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("Init VPSS err for %#x!\n", s32Ret);
        goto EXIT_VI_STOP;
    }

    s32Ret = SAMPLE_COMM_VI_Bind_VPSS(ViPipe, ViChn, VpssGrp);
    if(s32Ret != HI_SUCCESS)
    {
        SAMPLE_PRT("VI Bind VPSS err for %#x!\n", s32Ret);
        goto EXIT_VPSS_STOP;
    }

   /******************************************
     start stream venc
    ******************************************/

    enRcMode = SAMPLE_VENC_GetRcMode();

    s32Ret = SAMPLE_COMM_VENC_GetGopAttr(enGopMode,&stGopAttr);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("Venc Get GopAttr for %#x!\n", s32Ret);
        goto EXIT_VI_VPSS_UNBIND;
    }

   /***encode h.264 **/
    s32Ret = SAMPLE_COMM_VENC_Start(VencChn[0], enPayLoad[0],enSize[0], enRcMode,u32Profile[0],bRcnRefShareBuf,&stGopAttr);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("Venc Start failed for %#x!\n", s32Ret);
        goto EXIT_VI_VPSS_UNBIND;
    }

    s32Ret = SAMPLE_COMM_VPSS_Bind_VENC(VpssGrp, VpssChn[0],VencChn[0]);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("Venc Get GopAttr failed for %#x!\n", s32Ret);
        goto EXIT_VENC_H264_STOP;
    }


    /******************************************
     stream save process
    ******************************************/
    s32Ret = SAMPLE_COMM_VENC_StartGetStream_Svc_t(s32ChnNum);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("Start Venc failed!\n");
        goto EXIT_VENC_H264_UnBind;
    }

    printf("please press twice ENTER to exit this sample\n");
    getchar();
    getchar();

    /******************************************
     exit process
    ******************************************/
    SAMPLE_COMM_VENC_StopGetStream();

EXIT_VENC_H264_UnBind:
    SAMPLE_COMM_VPSS_UnBind_VENC(VpssGrp,VpssChn[0],VencChn[0]);
EXIT_VENC_H264_STOP:
    SAMPLE_COMM_VENC_Stop(VencChn[0]);
EXIT_VI_VPSS_UNBIND:
    SAMPLE_COMM_VI_UnBind_VPSS(ViPipe,ViChn,VpssGrp);
EXIT_VPSS_STOP:
    SAMPLE_COMM_VPSS_Stop(VpssGrp,abChnEnable);
EXIT_VI_STOP:
    SAMPLE_COMM_VI_StopVi(&stViConfig);
    SAMPLE_COMM_SYS_Exit();

    return s32Ret;
}


/******************************************************************************
* function: Qpmap, H.265e + H264e@720P,H.265 Channel resolution adaptable with sensor
******************************************************************************/
HI_S32 SAMPLE_VENC_Qpmap(void)
{
    HI_S32 i;
    HI_S32 s32Ret;
    SIZE_S          stSize[2];
    PIC_SIZE_E      enSize[2]     = {PIC_3840x2160, PIC_720P};

    HI_S32          s32ChnNum     = 2;
    VENC_CHN        VencChn[2]    = {0,1};
    HI_U32          u32Profile[2] = {0,0};
    PAYLOAD_TYPE_E  enPayLoad[2]  = {PT_H265, PT_H264};
    VENC_GOP_MODE_E enGopMode;
    VENC_GOP_ATTR_S stGopAttr;
    SAMPLE_RC_E     enRcMode = SAMPLE_RC_QPMAP;
    HI_BOOL         bRcnRefShareBuf = HI_TRUE;

    VI_DEV          ViDev        = 0;
    VI_PIPE         ViPipe       = 0;
    VI_CHN          ViChn        = 0;
    SAMPLE_VI_CONFIG_S stViConfig;

    VPSS_GRP        VpssGrp        = 0;
    VPSS_CHN        VpssChn[2]     = {0,1};
    HI_BOOL         abChnEnable[4] = {1,0,0,0};

    HI_U32 u32SupplementConfig = HI_FALSE;

    for(i=0; i<s32ChnNum; i++)
    {
        s32Ret = SAMPLE_COMM_SYS_GetPicSize(enSize[i], &stSize[i]);
        if (HI_SUCCESS != s32Ret)
        {
            SAMPLE_PRT("SAMPLE_COMM_SYS_GetPicSize failed!\n");
            return s32Ret;
        }
    }

    SAMPLE_COMM_VI_GetSensorInfo(&stViConfig);

    if(SAMPLE_SNS_TYPE_BUTT == stViConfig.astViInfo[0].stSnsInfo.enSnsType)
    {
        SAMPLE_PRT("Not set SENSOR%d_TYPE !\n",0);
        return HI_FAILURE;
    }

    s32Ret = SAMPLE_VENC_CheckSensor(stViConfig.astViInfo[0].stSnsInfo.enSnsType,stSize[0]);
    if(s32Ret != HI_SUCCESS)
    {
        s32Ret = SAMPLE_VENC_ModifyResolution(stViConfig.astViInfo[0].stSnsInfo.enSnsType,&enSize[0],&stSize[0]);
        if(s32Ret != HI_SUCCESS)
        {
            return HI_FAILURE;
        }
    }

    stViConfig.s32WorkingViNum       = 1;
    stViConfig.astViInfo[0].stDevInfo.ViDev     = ViDev;
    stViConfig.astViInfo[0].stPipeInfo.aPipe[0] = ViPipe;
    stViConfig.astViInfo[0].stChnInfo.ViChn     = ViChn;
    stViConfig.astViInfo[0].stChnInfo.enDynamicRange = DYNAMIC_RANGE_SDR8;
    stViConfig.astViInfo[0].stChnInfo.enPixFormat    = PIXEL_FORMAT_YVU_SEMIPLANAR_420;
    s32Ret = SAMPLE_VENC_VI_Init(&stViConfig, HI_FALSE,u32SupplementConfig);
    if(s32Ret != HI_SUCCESS)
    {
        SAMPLE_PRT("Init VI err for %#x!\n", s32Ret);
        return HI_FAILURE;
    }

    s32Ret = SAMPLE_VENC_VPSS_Init(VpssGrp,abChnEnable,DYNAMIC_RANGE_SDR8,PIXEL_FORMAT_YVU_SEMIPLANAR_420,stSize,stViConfig.astViInfo[0].stSnsInfo.enSnsType);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("Init VPSS err for %#x!\n", s32Ret);
        goto EXIT_VI_STOP;
    }

    s32Ret = SAMPLE_COMM_VI_Bind_VPSS(ViPipe, ViChn, VpssGrp);
    if(s32Ret != HI_SUCCESS)
    {
        SAMPLE_PRT("VI Bind VPSS err for %#x!\n", s32Ret);
        goto EXIT_VPSS_STOP;
    }

   /******************************************
     start stream venc
    ******************************************/
    enGopMode = SAMPLE_VENC_GetGopMode();
    s32Ret = SAMPLE_COMM_VENC_GetGopAttr(enGopMode,&stGopAttr);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("Venc Get GopAttr for %#x!\n", s32Ret);
        goto EXIT_VI_VPSS_UNBIND;
    }

   /***encode h.265 **/
    s32Ret = SAMPLE_COMM_VENC_Start(VencChn[0], enPayLoad[0],enSize[0], enRcMode,u32Profile[0],bRcnRefShareBuf,&stGopAttr);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("Venc Start failed for %#x!\n", s32Ret);
        goto EXIT_VI_VPSS_UNBIND;
    }

    s32Ret = SAMPLE_COMM_VENC_Start(VencChn[1], enPayLoad[1],enSize[1], enRcMode,u32Profile[0],bRcnRefShareBuf,&stGopAttr);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("Venc Start failed for %#x!\n", s32Ret);
        goto EXIT_VENC_H265_STOP;
    }

    s32Ret = SAMPLE_COMM_VENC_QpmapSendFrame(VpssGrp,VpssChn[0],VencChn,s32ChnNum,stSize[0]);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("SAMPLE_COMM_VENC_QpmapSendFrame failed for %#x!\n", s32Ret);
        goto EXIT_VENC_H264_STOP;
    }


    /******************************************
     stream save process
    ******************************************/
    s32Ret = SAMPLE_COMM_VENC_StartGetStream(VencChn,s32ChnNum);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("Start Venc failed!\n");
        goto EXIT_VENC_H264_STOP;
    }

    printf("please press twice ENTER to exit this sample\n");
    getchar();
    getchar();

    /******************************************
     exit process
    ******************************************/
    SAMPLE_COMM_VENC_StopSendQpmapFrame();
    SAMPLE_COMM_VENC_StopGetStream();

EXIT_VENC_H264_STOP:
    SAMPLE_COMM_VENC_Stop(VencChn[1]);
EXIT_VENC_H265_STOP:
    SAMPLE_COMM_VENC_Stop(VencChn[0]);
EXIT_VI_VPSS_UNBIND:
    SAMPLE_COMM_VI_UnBind_VPSS(ViPipe,ViChn,VpssGrp);
EXIT_VPSS_STOP:
    SAMPLE_COMM_VPSS_Stop(VpssGrp,abChnEnable);
EXIT_VI_STOP:
    SAMPLE_COMM_VI_StopVi(&stViConfig);
    SAMPLE_COMM_SYS_Exit();

    return s32Ret;
}

/******************************************************************************
* function: Mjpeg + Jpeg,Channel resolution adaptable with sensor
******************************************************************************/
HI_S32 SAMPLE_VENC_MJPEG_JPEG(void)
{
    HI_S32 i;
    char  ch;
    HI_S32 s32Ret;
    SIZE_S          stSize[2];
    PIC_SIZE_E      enSize[2]     = {PIC_3840x2160,PIC_3840x2160};
    HI_S32          s32ChnNum     = 2;
    VENC_CHN        VencChn[2]    = {0,1};
    HI_U32          u32Profile[2] = {0,0};
    PAYLOAD_TYPE_E  enPayLoad[2]  = {PT_MJPEG, PT_JPEG};
    VENC_GOP_MODE_E enGopMode     = VENC_GOPMODE_NORMALP;
    VENC_GOP_ATTR_S stGopAttr;
    SAMPLE_RC_E     enRcMode;
    HI_BOOL         bSupportDcf   = HI_TRUE;

    VI_DEV          ViDev         = 0;
    VI_PIPE         ViPipe        = 0;
    VI_CHN          ViChn         = 0;
    SAMPLE_VI_CONFIG_S stViConfig;

    VPSS_GRP        VpssGrp        = 0;
    VPSS_CHN        VpssChn[2]     = {0,1};
    HI_BOOL         abChnEnable[4] = {1,1,0,0};

    HI_U32 u32SupplementConfig = HI_TRUE;

    for(i=0; i<s32ChnNum; i++)
    {
        s32Ret = SAMPLE_COMM_SYS_GetPicSize(enSize[i], &stSize[i]);
        if (HI_SUCCESS != s32Ret)
        {
            SAMPLE_PRT("SAMPLE_COMM_SYS_GetPicSize failed!\n");
            return s32Ret;
        }
    }

    SAMPLE_COMM_VI_GetSensorInfo(&stViConfig);

    if(SAMPLE_SNS_TYPE_BUTT == stViConfig.astViInfo[0].stSnsInfo.enSnsType)
    {
        SAMPLE_PRT("Not set SENSOR%d_TYPE !\n",0);
        return HI_FAILURE;
    }

    s32Ret = SAMPLE_VENC_CheckSensor(stViConfig.astViInfo[0].stSnsInfo.enSnsType,stSize[0]);
    if(s32Ret != HI_SUCCESS)
    {
        s32Ret = SAMPLE_VENC_ModifyResolution(stViConfig.astViInfo[0].stSnsInfo.enSnsType,&enSize[0],&stSize[0]);
        if(s32Ret != HI_SUCCESS)
        {
            return HI_FAILURE;
        }
        s32Ret = SAMPLE_VENC_ModifyResolution(stViConfig.astViInfo[0].stSnsInfo.enSnsType,&enSize[1],&stSize[1]);
        if(s32Ret != HI_SUCCESS)
        {
            return HI_FAILURE;
        }
    }


    stViConfig.s32WorkingViNum       = 1;
    stViConfig.astViInfo[0].stDevInfo.ViDev     = ViDev;
    stViConfig.astViInfo[0].stPipeInfo.aPipe[0] = ViPipe;
    stViConfig.astViInfo[0].stChnInfo.ViChn     = ViChn;
    stViConfig.astViInfo[0].stChnInfo.enDynamicRange = DYNAMIC_RANGE_SDR8;
    stViConfig.astViInfo[0].stChnInfo.enPixFormat    = PIXEL_FORMAT_YVU_SEMIPLANAR_422;
    s32Ret = SAMPLE_VENC_VI_Init(&stViConfig, HI_FALSE,u32SupplementConfig);
    if(s32Ret != HI_SUCCESS)
    {
        SAMPLE_PRT("Init VI err for %#x!\n", s32Ret);
        return HI_FAILURE;
    }
    s32Ret = SAMPLE_VENC_VPSS_Init(VpssGrp,abChnEnable,DYNAMIC_RANGE_SDR8,PIXEL_FORMAT_YVU_SEMIPLANAR_422,stSize,stViConfig.astViInfo[0].stSnsInfo.enSnsType);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("Init VPSS err for %#x!\n", s32Ret);
        goto EXIT_VI_STOP;
    }

    s32Ret = SAMPLE_COMM_VI_Bind_VPSS(ViPipe, ViChn, VpssGrp);
    if(s32Ret != HI_SUCCESS)
    {
        SAMPLE_PRT("VI Bind VPSS err for %#x!\n", s32Ret);
        goto EXIT_VPSS_STOP;
    }
    SAMPLE_VENC_SetDCFInfo(ViPipe);

   /******************************************
     start stream venc
    ******************************************/

    enRcMode = SAMPLE_VENC_GetRcMode();

    s32Ret = SAMPLE_COMM_VENC_GetGopAttr(enGopMode,&stGopAttr);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("Venc Get GopAttr for %#x!\n", s32Ret);
        goto EXIT_VI_VPSS_UNBIND;
    }


   /***encode Mjpege **/
    s32Ret = SAMPLE_COMM_VENC_Start(VencChn[0], enPayLoad[0],enSize[0], enRcMode,u32Profile[0],HI_FALSE,&stGopAttr);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("Venc Start failed for %#x!\n", s32Ret);
        goto EXIT_VI_VPSS_UNBIND;
    }

    s32Ret = SAMPLE_COMM_VPSS_Bind_VENC(VpssGrp, VpssChn[0],VencChn[0]);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("Venc Get GopAttr failed for %#x!\n", s32Ret);
        goto EXIT_VENC_MJPEGE_STOP;
    }

    /***encode Jpege **/
    s32Ret = SAMPLE_COMM_VENC_SnapStart(VencChn[1], &stSize[1], bSupportDcf);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("Venc Start failed for %#x!\n", s32Ret);
        goto EXIT_VENC_MJPEGE_UnBind;
    }

    s32Ret = SAMPLE_COMM_VPSS_Bind_VENC(VpssGrp, VpssChn[1],VencChn[1]);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("Venc bind Vpss failed for %#x!\n", s32Ret);
        goto EXIT_VENC_JPEGE_STOP;
    }

    /******************************************
     stream save process
    ******************************************/
    s32Ret = SAMPLE_COMM_VENC_StartGetStream(VencChn,1);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("Start Venc failed!\n");
        goto EXIT_VENC_JPEGE_UnBind;
    }

    /******************************************
     stream venc process -- get jpeg stream, then save it to file.
    ******************************************/
    printf("press 'q' to exit snap!\nperess ENTER to capture one picture to file\n");
    i = 0;
    while ((ch = (char)getchar()) != 'q')
    {
        printf("press any key to snap one pic\n");
        getchar();

        s32Ret = SAMPLE_COMM_VENC_SnapProcess(VencChn[1], 1, HI_TRUE, HI_TRUE);
        if (HI_SUCCESS != s32Ret)
        {
            printf("%s: sanp process failed!\n", __FUNCTION__);
            break;
        }
        printf("snap %d success!\n", i);
        i++;
    }

    printf("please press twice ENTER to exit this sample\n");
    getchar();
    getchar();
    /******************************************
     exit process
    ******************************************/
    SAMPLE_COMM_VENC_StopGetStream();

EXIT_VENC_JPEGE_UnBind:
    SAMPLE_COMM_VPSS_UnBind_VENC(VpssGrp,VpssChn[1],VencChn[1]);
EXIT_VENC_JPEGE_STOP:
    SAMPLE_COMM_VENC_Stop(VencChn[1]);
EXIT_VENC_MJPEGE_UnBind:
    SAMPLE_COMM_VPSS_UnBind_VENC(VpssGrp,VpssChn[0],VencChn[0]);
EXIT_VENC_MJPEGE_STOP:
    SAMPLE_COMM_VENC_Stop(VencChn[0]);
EXIT_VI_VPSS_UNBIND:
    SAMPLE_COMM_VI_UnBind_VPSS(ViPipe,ViChn,VpssGrp);
EXIT_VPSS_STOP:
    SAMPLE_COMM_VPSS_Stop(VpssGrp,abChnEnable);
EXIT_VI_STOP:
    SAMPLE_COMM_VI_StopVi(&stViConfig);
    SAMPLE_COMM_SYS_Exit();

    return s32Ret;
}


/******************************************************************************
* function    : main()
* Description : video venc sample
******************************************************************************/
#ifdef __HuaweiLite__
    int app_main(int argc, char *argv[])
#else
    int sample_venc_main(int argc, char *argv[])
#endif
{
    HI_S32 s32Ret;
    HI_U32 u32Index;

    if (argc < 2 || argc > 2)
    {
        SAMPLE_VENC_Usage(argv[0]);
        return HI_FAILURE;
    }

    if (!strncmp(argv[1], "-h", 2))
    {
        SAMPLE_VENC_Usage(argv[0]);
        return HI_SUCCESS;
    }

    u32Index = atoi(argv[1]);

#ifndef __HuaweiLite__
    signal(SIGINT, SAMPLE_VENC_HandleSig);
    signal(SIGTERM, SAMPLE_VENC_HandleSig);
#endif

    switch (u32Index)
    {
        case 0:
            s32Ret = SAMPLE_VENC_H265_H264();
            break;
        case 1:
            s32Ret = SAMPLE_VENC_LOW_DELAY();
            break;
        case 2:
            s32Ret = SAMPLE_VENC_Qpmap();
            break;
        case 3:
            s32Ret = SAMPLE_VENC_IntraRefresh();
            break;
        case 4:
            s32Ret = SAMPLE_VENC_ROIBG();
            break;
        case 5:
            s32Ret = SAMPLE_VENC_DeBreathEffect();
            break;
        case 6:
            s32Ret = SAMPLE_VENC_SVC_H264();
            break;
        case 7:
            s32Ret = SAMPLE_VENC_MJPEG_JPEG();
            break;
        default:
            printf("the index is invaild!\n");
            SAMPLE_VENC_Usage(argv[0]);
            return HI_FAILURE;
    }

    if (HI_SUCCESS == s32Ret)
    { printf("program exit normally!\n"); }
    else
    { printf("program exit abnormally!\n"); }

#ifdef __HuaweiLite__
    return s32Ret;
#else
    exit(s32Ret);
#endif
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */
