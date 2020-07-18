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

#define BIG_STREAM_SIZE     PIC_2688x1944
#define SMALL_STREAM_SIZE   PIC_VGA


#define VB_MAX_NUM            10
#define ONLINE_LIMIT_WIDTH    2304

#define WRAP_BUF_LINE_EXT     416


typedef struct hiSAMPLE_VPSS_ATTR_S
{
    SIZE_S            stMaxSize;
    DYNAMIC_RANGE_E   enDynamicRange;
    PIXEL_FORMAT_E    enPixelFormat;
    COMPRESS_MODE_E   enCompressMode[VPSS_MAX_PHY_CHN_NUM];
    SIZE_S            stOutPutSize[VPSS_MAX_PHY_CHN_NUM];
    FRAME_RATE_CTRL_S stFrameRate[VPSS_MAX_PHY_CHN_NUM];
    HI_BOOL           bMirror[VPSS_MAX_PHY_CHN_NUM];
    HI_BOOL           bFlip[VPSS_MAX_PHY_CHN_NUM];
    HI_BOOL           bChnEnable[VPSS_MAX_PHY_CHN_NUM];

    SAMPLE_SNS_TYPE_E enSnsType;
    HI_U32            BigStreamId;
    HI_U32            SmallStreamId;
    VI_VPSS_MODE_E    ViVpssMode;
    HI_BOOL           bWrapEn;
    HI_U32            WrapBufLine;
} SAMPLE_VPSS_CHN_ATTR_S;

typedef struct hiSAMPLE_VB_ATTR_S
{
    HI_U32            validNum;
    HI_U64            blkSize[VB_MAX_NUM];
    HI_U32            blkCnt[VB_MAX_NUM];
    HI_U32            supplementConfig;
} SAMPLE_VB_ATTR_S;


/******************************************************************************
* function : show usage
******************************************************************************/
void SAMPLE_VENC_Usage(char* sPrgNm)
{
    printf("Usage : %s [index] \n", sPrgNm);
    printf("index:\n");
    printf("\t  0) H265(Large Stream)+H264(Small Stream)+JPEG lowdelay encode with RingBuf.\n");
    printf("\t  1) H.265e + H264e.\n");
    printf("\t  2) Qpmap:H.265e + H264e.\n");
    printf("\t  3) IntraRefresh:H.265e + H264e.\n");
    printf("\t  4) RoiBgFrameRate:H.265e + H.264e.\n");
    printf("\t  5) Mjpeg +Jpeg snap.\n");

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
        case 'f':
            enRcMode = SAMPLE_RC_FIXQP;
            break;
        case 'x':
            enRcMode = SAMPLE_RC_CVBR;
            break;
        case 'q':
            enRcMode = SAMPLE_RC_QVBR;
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


static HI_U32 GetFrameRateFromSensorType(SAMPLE_SNS_TYPE_E enSnsType)
{
    HI_U32 FrameRate;

    SAMPLE_COMM_VI_GetFrameRateBySensor(enSnsType, &FrameRate);

    return FrameRate;
}

static HI_U32 GetFullLinesStdFromSensorType(SAMPLE_SNS_TYPE_E enSnsType)
{
    HI_U32 FullLinesStd = 0;

    switch (enSnsType)
    {
        case SONY_IMX327_MIPI_2M_30FPS_12BIT:
        case SONY_IMX327_MIPI_2M_30FPS_12BIT_WDR2TO1:
            FullLinesStd = 1125;
            break;
        case SONY_IMX307_MIPI_2M_30FPS_12BIT:
        case SONY_IMX307_MIPI_2M_30FPS_12BIT_WDR2TO1:
        case SONY_IMX307_2L_MIPI_2M_30FPS_12BIT:
        case SONY_IMX307_2L_MIPI_2M_30FPS_12BIT_WDR2TO1:
        case SMART_SC2235_DC_2M_30FPS_10BIT:
        case SMART_SC2231_MIPI_2M_30FPS_10BIT:
        case SMART_SC2335_MIPI_2M_30FPS_10BIT:
            FullLinesStd = 1125;
            break;
            
        case SMART_AR0130_DC_1M_30FPS_12BIT:    
            FullLinesStd = 990;//746;
            break;
            
        case SONY_IMX335_MIPI_5M_30FPS_12BIT:
        case SONY_IMX335_MIPI_5M_30FPS_10BIT_WDR2TO1:
            FullLinesStd = 1875;
            break;
        case SONY_IMX335_MIPI_4M_30FPS_12BIT:
        case SONY_IMX335_MIPI_4M_30FPS_10BIT_WDR2TO1:
            FullLinesStd = 1375;
            break;
        case SMART_SC4236_MIPI_3M_30FPS_10BIT:
        case SMART_SC4236_MIPI_3M_20FPS_10BIT:
            FullLinesStd = 1600;
            break;
        case GALAXYCORE_GC2053_MIPI_2M_30FPS_10BIT:
        case GALAXYCORE_GC2053_MIPI_2M_30FPS_10BIT_FORCAR:
            FullLinesStd = 1108;
            break;
        default:
            SAMPLE_PRT("Error: Not support this sensor now! ==> %d\n",enSnsType);
            break;
    }

    return FullLinesStd;
}

static HI_VOID AdjustWrapBufLineBySnsType(SAMPLE_SNS_TYPE_E enSnsType, HI_U32 *pWrapBufLine)
{
    /*some sensor as follow need to expand the wrapBufLine*/
    if ((enSnsType == SMART_SC4236_MIPI_3M_30FPS_10BIT) ||
        (enSnsType == SMART_SC4236_MIPI_3M_20FPS_10BIT) ||
        //(enSnsType == SMART_AR0130_DC_1M_30FPS_12BIT)   || 
        (enSnsType == SMART_SC2235_DC_2M_30FPS_10BIT)   )
    {
        *pWrapBufLine += WRAP_BUF_LINE_EXT;
    }

    return;
}

static HI_VOID GetSensorResolution(SAMPLE_SNS_TYPE_E enSnsType, SIZE_S *pSnsSize)
{
    HI_S32          ret;
    SIZE_S          SnsSize;
    PIC_SIZE_E      enSnsSize;

    ret = SAMPLE_COMM_VI_GetSizeBySensor(enSnsType, &enSnsSize);
    if (HI_SUCCESS != ret)
    {
        SAMPLE_PRT("SAMPLE_COMM_VI_GetSizeBySensor failed!\n");
        return;
    }
    ret = SAMPLE_COMM_SYS_GetPicSize(enSnsSize, &SnsSize);
    if (HI_SUCCESS != ret)
    {
        SAMPLE_PRT("SAMPLE_COMM_SYS_GetPicSize failed!\n");
        return;
    }

    *pSnsSize = SnsSize;

    return;
}

static HI_VOID GetVpssWrapBufLine(SAMPLE_VPSS_CHN_ATTR_S *pParam)
{
    HI_U32 vpssWrapBufLine = 0;

    VPSS_VENC_WRAP_PARAM_S wrapParam;

    memset(&wrapParam, 0, sizeof(VPSS_VENC_WRAP_PARAM_S));
    wrapParam.bAllOnline      = (pParam->ViVpssMode == VI_ONLINE_VPSS_ONLINE) ? 1 : 0;
    wrapParam.u32FrameRate    = GetFrameRateFromSensorType(pParam->enSnsType);
    wrapParam.u32FullLinesStd = GetFullLinesStdFromSensorType(pParam->enSnsType);
    wrapParam.stLargeStreamSize.u32Width = pParam->stOutPutSize[pParam->BigStreamId].u32Width;
    wrapParam.stLargeStreamSize.u32Height= pParam->stOutPutSize[pParam->BigStreamId].u32Height;
    wrapParam.stSmallStreamSize.u32Width = pParam->stOutPutSize[pParam->SmallStreamId].u32Width;
    wrapParam.stSmallStreamSize.u32Height= pParam->stOutPutSize[pParam->SmallStreamId].u32Height;

    if (HI_MPI_SYS_GetVPSSVENCWrapBufferLine(&wrapParam, &vpssWrapBufLine) != HI_SUCCESS)
    {
        SAMPLE_PRT("Error:Current BigStream(%dx%d@%d fps) and SmallStream(%dx%d@%d fps) not support Ring!== return 0x%x(0x%x)\n",
            wrapParam.stLargeStreamSize.u32Width,wrapParam.stLargeStreamSize.u32Height,wrapParam.u32FrameRate,
            wrapParam.stSmallStreamSize.u32Width,wrapParam.stSmallStreamSize.u32Height,wrapParam.u32FrameRate,
            HI_MPI_SYS_GetVPSSVENCWrapBufferLine(&wrapParam, &vpssWrapBufLine), HI_ERR_SYS_NOT_SUPPORT);
        vpssWrapBufLine = 0;
    }
    else
    {
        AdjustWrapBufLineBySnsType(pParam->enSnsType, &vpssWrapBufLine);
    }

    pParam->WrapBufLine = vpssWrapBufLine;

    return;
}

static VI_VPSS_MODE_E GetViVpssModeFromResolution(SAMPLE_SNS_TYPE_E SnsType)
{
    SIZE_S SnsSize = {0};
    VI_VPSS_MODE_E ViVpssMode;

    GetSensorResolution(SnsType, &SnsSize);

    if (SnsSize.u32Width > ONLINE_LIMIT_WIDTH)
    {
        ViVpssMode = VI_OFFLINE_VPSS_ONLINE;
    }
    else
    {
        ViVpssMode = VI_ONLINE_VPSS_ONLINE;
    }

    return ViVpssMode;
}

HI_VOID SAMPLE_VENC_GetDefaultVpssAttr(SAMPLE_SNS_TYPE_E enSnsType, HI_BOOL *pChanEnable, SIZE_S stEncSize[], SAMPLE_VPSS_CHN_ATTR_S *pVpssAttr)
{
    HI_S32 i;

    memset(pVpssAttr, 0, sizeof(SAMPLE_VPSS_CHN_ATTR_S));

    pVpssAttr->enDynamicRange = DYNAMIC_RANGE_SDR8;
    pVpssAttr->enPixelFormat  = PIXEL_FORMAT_YVU_SEMIPLANAR_420;
    pVpssAttr->bWrapEn        = 0;
    pVpssAttr->enSnsType      = enSnsType;
    pVpssAttr->ViVpssMode     = GetViVpssModeFromResolution(enSnsType);

    for (i = 0; i < VPSS_MAX_PHY_CHN_NUM; i++)
    {
        if (HI_TRUE == pChanEnable[i])
        {
            pVpssAttr->enCompressMode[i]          = (i == 0)? COMPRESS_MODE_SEG : COMPRESS_MODE_NONE;
            pVpssAttr->stOutPutSize[i].u32Width   = stEncSize[i].u32Width;
            pVpssAttr->stOutPutSize[i].u32Height  = stEncSize[i].u32Height;
            pVpssAttr->stFrameRate[i].s32SrcFrameRate  = -1;
            pVpssAttr->stFrameRate[i].s32DstFrameRate  = -1;
            pVpssAttr->bMirror[i]                      = HI_FALSE;
            pVpssAttr->bFlip[i]                        = HI_FALSE;

            pVpssAttr->bChnEnable[i]                   = HI_TRUE;
        }
    }

    return;
}

HI_S32 SAMPLE_VENC_SYS_Init(SAMPLE_VB_ATTR_S *pCommVbAttr)
{
    HI_S32 i;
    HI_S32 s32Ret;
    VB_CONFIG_S stVbConf;

    if (pCommVbAttr->validNum > VB_MAX_COMM_POOLS)
    {
        SAMPLE_PRT("SAMPLE_VENC_SYS_Init validNum(%d) too large than VB_MAX_COMM_POOLS(%d)!\n", pCommVbAttr->validNum, VB_MAX_COMM_POOLS);
        return HI_FAILURE;
    }

    memset(&stVbConf, 0, sizeof(VB_CONFIG_S));

    for (i = 0; i < pCommVbAttr->validNum; i++)
    {
        stVbConf.astCommPool[i].u64BlkSize   = pCommVbAttr->blkSize[i];
        stVbConf.astCommPool[i].u32BlkCnt    = pCommVbAttr->blkCnt[i];
        //printf("%s,%d,stVbConf.astCommPool[%d].u64BlkSize = %lld, blkSize = %d\n",__func__,__LINE__,i,stVbConf.astCommPool[i].u64BlkSize,stVbConf.astCommPool[i].u32BlkCnt);
    }

    stVbConf.u32MaxPoolCnt = pCommVbAttr->validNum;

    if(pCommVbAttr->supplementConfig == 0)
    {
        s32Ret = SAMPLE_COMM_SYS_Init(&stVbConf);
    }
    else
    {
        s32Ret = SAMPLE_COMM_SYS_InitWithVbSupplement(&stVbConf,pCommVbAttr->supplementConfig);
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

HI_S32 SAMPLE_VENC_VI_Init( SAMPLE_VI_CONFIG_S *pstViConfig, VI_VPSS_MODE_E ViVpssMode)
{
    HI_S32              s32Ret;
    SAMPLE_SNS_TYPE_E   enSnsType;
    ISP_CTRL_PARAM_S    stIspCtrlParam;
    HI_U32              u32FrameRate;


    enSnsType = pstViConfig->astViInfo[0].stSnsInfo.enSnsType;

    pstViConfig->as32WorkingViId[0]                           = 0;

    pstViConfig->astViInfo[0].stSnsInfo.MipiDev            = SAMPLE_COMM_VI_GetComboDevBySensor(pstViConfig->astViInfo[0].stSnsInfo.enSnsType, 0);
    pstViConfig->astViInfo[0].stSnsInfo.s32BusId           = 0;
    pstViConfig->astViInfo[0].stDevInfo.enWDRMode          = WDR_MODE_NONE;
    pstViConfig->astViInfo[0].stPipeInfo.enMastPipeMode    = ViVpssMode;

    //pstViConfig->astViInfo[0].stPipeInfo.aPipe[0]          = ViPipe0;
    pstViConfig->astViInfo[0].stPipeInfo.aPipe[1]          = -1;

    //pstViConfig->astViInfo[0].stChnInfo.ViChn              = ViChn;
    //pstViConfig->astViInfo[0].stChnInfo.enPixFormat        = PIXEL_FORMAT_YVU_SEMIPLANAR_420;
    //pstViConfig->astViInfo[0].stChnInfo.enDynamicRange     = enDynamicRange;
    pstViConfig->astViInfo[0].stChnInfo.enVideoFormat      = VIDEO_FORMAT_LINEAR;
    pstViConfig->astViInfo[0].stChnInfo.enCompressMode     = COMPRESS_MODE_NONE;
    s32Ret = SAMPLE_COMM_VI_SetParam(pstViConfig);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("SAMPLE_COMM_VI_SetParam failed with %d!\n", s32Ret);
        return s32Ret;
    }

    SAMPLE_COMM_VI_GetFrameRateBySensor(enSnsType, &u32FrameRate);

    s32Ret = HI_MPI_ISP_GetCtrlParam(pstViConfig->astViInfo[0].stPipeInfo.aPipe[0], &stIspCtrlParam);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("HI_MPI_ISP_GetCtrlParam failed with %d!\n", s32Ret);
        return s32Ret;
    }
    stIspCtrlParam.u32StatIntvl  = u32FrameRate/30;
    if (stIspCtrlParam.u32StatIntvl == 0)
    {
        stIspCtrlParam.u32StatIntvl = 1;
    }

    s32Ret = HI_MPI_ISP_SetCtrlParam(pstViConfig->astViInfo[0].stPipeInfo.aPipe[0], &stIspCtrlParam);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("HI_MPI_ISP_SetCtrlParam failed with %d!\n", s32Ret);
        return s32Ret;
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

static HI_S32 SAMPLE_VENC_VPSS_CreateGrp(VPSS_GRP VpssGrp, SAMPLE_VPSS_CHN_ATTR_S *pParam)
{
    HI_S32          s32Ret;
    PIC_SIZE_E      enSnsSize;
    SIZE_S          stSnsSize;
    VPSS_GRP_ATTR_S stVpssGrpAttr = {0};

    s32Ret = SAMPLE_COMM_VI_GetSizeBySensor(pParam->enSnsType, &enSnsSize);
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

    stVpssGrpAttr.enDynamicRange          = pParam->enDynamicRange;
    stVpssGrpAttr.enPixelFormat           = pParam->enPixelFormat;
    stVpssGrpAttr.u32MaxW                 = stSnsSize.u32Width;
    stVpssGrpAttr.u32MaxH                 = stSnsSize.u32Height;
    stVpssGrpAttr.bNrEn                   = HI_TRUE;
    stVpssGrpAttr.stNrAttr.enNrType       = VPSS_NR_TYPE_VIDEO;
    stVpssGrpAttr.stNrAttr.enNrMotionMode = NR_MOTION_MODE_NORMAL;
    stVpssGrpAttr.stNrAttr.enCompressMode = COMPRESS_MODE_FRAME;
    stVpssGrpAttr.stFrameRate.s32SrcFrameRate = -1;
    stVpssGrpAttr.stFrameRate.s32DstFrameRate = -1;

    s32Ret = HI_MPI_VPSS_CreateGrp(VpssGrp, &stVpssGrpAttr);

    if (s32Ret != HI_SUCCESS)
    {
        SAMPLE_PRT("HI_MPI_VPSS_CreateGrp(grp:%d) failed with %#x!\n", VpssGrp, s32Ret);
        return HI_FAILURE;
    }

    return s32Ret;
}

static HI_S32 SAMPLE_VENC_VPSS_DestoryGrp(VPSS_GRP VpssGrp)
{
    HI_S32          s32Ret;

    s32Ret = HI_MPI_VPSS_DestroyGrp(VpssGrp);
    if (s32Ret != HI_SUCCESS)
    {
        SAMPLE_PRT("failed with %#x!\n", s32Ret);
        return HI_FAILURE;
    }

    return s32Ret;
}

static HI_S32 SAMPLE_VENC_VPSS_StartGrp(VPSS_GRP VpssGrp)
{
    HI_S32          s32Ret;

    s32Ret = HI_MPI_VPSS_StartGrp(VpssGrp);
    if (s32Ret != HI_SUCCESS)
    {
        SAMPLE_PRT("HI_MPI_VPSS_CreateGrp(grp:%d) failed with %#x!\n", VpssGrp, s32Ret);
        return HI_FAILURE;
    }

    return s32Ret;
}

static HI_S32 SAMPLE_VENC_VPSS_ChnEnable(VPSS_GRP VpssGrp, VPSS_CHN VpssChn, SAMPLE_VPSS_CHN_ATTR_S *pParam, HI_BOOL bWrapEn)
{
    HI_S32 s32Ret;
    VPSS_CHN_ATTR_S     stVpssChnAttr;
    VPSS_CHN_BUF_WRAP_S stVpssChnBufWrap;

    memset(&stVpssChnAttr, 0, sizeof(VPSS_CHN_ATTR_S));
    stVpssChnAttr.u32Width                     = pParam->stOutPutSize[VpssChn].u32Width;
    stVpssChnAttr.u32Height                    = pParam->stOutPutSize[VpssChn].u32Height;
    stVpssChnAttr.enChnMode                    = VPSS_CHN_MODE_USER;
    stVpssChnAttr.enCompressMode               = pParam->enCompressMode[VpssChn];
    stVpssChnAttr.enDynamicRange               = pParam->enDynamicRange;
    stVpssChnAttr.enPixelFormat                = pParam->enPixelFormat;
    if (stVpssChnAttr.u32Width * stVpssChnAttr.u32Height > 2688 * 1520 ) {
        stVpssChnAttr.stFrameRate.s32SrcFrameRate  = 30;
        stVpssChnAttr.stFrameRate.s32DstFrameRate  = 20;
    } else {
        stVpssChnAttr.stFrameRate.s32SrcFrameRate  = pParam->stFrameRate[VpssChn].s32SrcFrameRate;
        stVpssChnAttr.stFrameRate.s32DstFrameRate  = pParam->stFrameRate[VpssChn].s32DstFrameRate;
    }
    stVpssChnAttr.u32Depth                     = 0;
    stVpssChnAttr.bMirror                      = pParam->bMirror[VpssChn];
    stVpssChnAttr.bFlip                        = pParam->bFlip[VpssChn];
    stVpssChnAttr.enVideoFormat                = VIDEO_FORMAT_LINEAR;
    stVpssChnAttr.stAspectRatio.enMode         = ASPECT_RATIO_NONE;

    s32Ret = HI_MPI_VPSS_SetChnAttr(VpssGrp, VpssChn, &stVpssChnAttr);
    if (s32Ret != HI_SUCCESS)
    {
        SAMPLE_PRT("HI_MPI_VPSS_SetChnAttr chan %d failed with %#x\n", VpssChn, s32Ret);
        goto exit0;
    }

    if (bWrapEn)
    {
        if (VpssChn != 0)   //vpss limit! just vpss chan0 support wrap
        {
            SAMPLE_PRT("Error:Just vpss chan 0 support wrap! Current chan %d\n", VpssChn);
            goto exit0;
        }


        HI_U32 WrapBufLen = 0;
        VPSS_VENC_WRAP_PARAM_S WrapParam;

        memset(&WrapParam, 0, sizeof(VPSS_VENC_WRAP_PARAM_S));
        WrapParam.bAllOnline      = (pParam->ViVpssMode == VI_ONLINE_VPSS_ONLINE) ? 1 : 0;
        WrapParam.u32FrameRate    = GetFrameRateFromSensorType(pParam->enSnsType);
        WrapParam.u32FullLinesStd = GetFullLinesStdFromSensorType(pParam->enSnsType);
        WrapParam.stLargeStreamSize.u32Width = pParam->stOutPutSize[pParam->BigStreamId].u32Width;
        WrapParam.stLargeStreamSize.u32Height= pParam->stOutPutSize[pParam->BigStreamId].u32Height;
        WrapParam.stSmallStreamSize.u32Width = pParam->stOutPutSize[pParam->SmallStreamId].u32Width;
        WrapParam.stSmallStreamSize.u32Height= pParam->stOutPutSize[pParam->SmallStreamId].u32Height;

        if (HI_MPI_SYS_GetVPSSVENCWrapBufferLine(&WrapParam, &WrapBufLen) == HI_SUCCESS)
        {
            AdjustWrapBufLineBySnsType(pParam->enSnsType, &WrapBufLen);

            stVpssChnBufWrap.u32WrapBufferSize = VPSS_GetWrapBufferSize(WrapParam.stLargeStreamSize.u32Width,
                WrapParam.stLargeStreamSize.u32Height, WrapBufLen, pParam->enPixelFormat, DATA_BITWIDTH_8,
                COMPRESS_MODE_NONE, DEFAULT_ALIGN);
            stVpssChnBufWrap.bEnable = 1;
            stVpssChnBufWrap.u32BufLine = WrapBufLen;
            s32Ret = HI_MPI_VPSS_SetChnBufWrapAttr(VpssGrp, VpssChn, &stVpssChnBufWrap);
            if (s32Ret != HI_SUCCESS)
            {
                SAMPLE_PRT("HI_MPI_VPSS_SetChnBufWrapAttr Chn %d failed with %#x\n", VpssChn, s32Ret);
                goto exit0;
            }
        }
        else
        {
            SAMPLE_PRT("Current sensor type: %d, not support BigStream(%dx%d) and SmallStream(%dx%d) Ring!!\n",
                pParam->enSnsType,
                pParam->stOutPutSize[pParam->BigStreamId].u32Width, pParam->stOutPutSize[pParam->BigStreamId].u32Height,
                pParam->stOutPutSize[pParam->SmallStreamId].u32Width, pParam->stOutPutSize[pParam->SmallStreamId].u32Height);
        }

    }

    s32Ret = HI_MPI_VPSS_EnableChn(VpssGrp, VpssChn);
    if (s32Ret != HI_SUCCESS)
    {
        SAMPLE_PRT("HI_MPI_VPSS_EnableChn (%d) failed with %#x\n", VpssChn, s32Ret);
        goto exit0;
    }

exit0:
    return s32Ret;
}

static HI_S32 SAMPLE_VENC_VPSS_ChnDisable(VPSS_GRP VpssGrp, VPSS_CHN VpssChn)
{
    HI_S32 s32Ret;

    s32Ret = HI_MPI_VPSS_DisableChn(VpssGrp, VpssChn);

    if (s32Ret != HI_SUCCESS)
    {
        SAMPLE_PRT("failed with %#x!\n", s32Ret);
        return HI_FAILURE;
    }

    return s32Ret;
}

HI_S32 SAMPLE_VENC_VPSS_Init(VPSS_GRP VpssGrp, SAMPLE_VPSS_CHN_ATTR_S *pstParam)
{
    HI_S32 i,j;
    HI_S32 s32Ret;
    HI_BOOL bWrapEn;

    s32Ret = SAMPLE_VENC_VPSS_CreateGrp(VpssGrp, pstParam);
    if (s32Ret != HI_SUCCESS)
    {
        goto exit0;
    }

    for (i = 0; i < VPSS_MAX_PHY_CHN_NUM; i++)
    {
        if (pstParam->bChnEnable[i] == HI_TRUE)
        {
            bWrapEn = (i==0)? pstParam->bWrapEn : 0;

            s32Ret = SAMPLE_VENC_VPSS_ChnEnable(VpssGrp, i, pstParam, bWrapEn);
            if (s32Ret != HI_SUCCESS)
            {
                goto exit1;
            }
        }
    }

    i--; // for abnormal case 'exit1' prossess;

    s32Ret = SAMPLE_VENC_VPSS_StartGrp(VpssGrp);
    if (s32Ret != HI_SUCCESS)
    {
        goto exit1;
    }

    return s32Ret;

exit1:
    for (j = 0; j <= i; j++)
    {
        if (pstParam->bChnEnable[j] == HI_TRUE)
        {
            SAMPLE_VENC_VPSS_ChnDisable(VpssGrp, i);
        }
    }

    SAMPLE_VENC_VPSS_DestoryGrp(VpssGrp);
exit0:
    return s32Ret;
}

HI_VOID SAMPLE_VENC_GetCommVbAttr(const SAMPLE_SNS_TYPE_E enSnsType, const SAMPLE_VPSS_CHN_ATTR_S *pstParam,
    HI_BOOL bSupportDcf, SAMPLE_VB_ATTR_S * pstcommVbAttr)
{
    if (pstParam->ViVpssMode != VI_ONLINE_VPSS_ONLINE)
    {
        SIZE_S snsSize = {0};
        GetSensorResolution(enSnsType, &snsSize);

        if (pstParam->ViVpssMode == VI_OFFLINE_VPSS_ONLINE || pstParam->ViVpssMode == VI_OFFLINE_VPSS_OFFLINE)
        {
            pstcommVbAttr->blkSize[pstcommVbAttr->validNum] = VI_GetRawBufferSize(snsSize.u32Width, snsSize.u32Height,
                                                                                  PIXEL_FORMAT_RGB_BAYER_12BPP,
                                                                                  COMPRESS_MODE_NONE,
                                                                                  DEFAULT_ALIGN);
            pstcommVbAttr->blkCnt[pstcommVbAttr->validNum]  = 3;
            pstcommVbAttr->validNum++;
        }

        if (pstParam->ViVpssMode == VI_OFFLINE_VPSS_OFFLINE)
        {
            pstcommVbAttr->blkSize[pstcommVbAttr->validNum] = COMMON_GetPicBufferSize(snsSize.u32Width, snsSize.u32Height,
                                                                                      PIXEL_FORMAT_YVU_SEMIPLANAR_420,
                                                                                      DATA_BITWIDTH_8,
                                                                                      COMPRESS_MODE_NONE,
                                                                                      DEFAULT_ALIGN);
            pstcommVbAttr->blkCnt[pstcommVbAttr->validNum]  = 2;
            pstcommVbAttr->validNum++;
        }

        if (pstParam->ViVpssMode == VI_ONLINE_VPSS_OFFLINE)
        {
            pstcommVbAttr->blkSize[pstcommVbAttr->validNum] = COMMON_GetPicBufferSize(snsSize.u32Width, snsSize.u32Height,
                                                                                      PIXEL_FORMAT_YVU_SEMIPLANAR_420,
                                                                                      DATA_BITWIDTH_8,
                                                                                      COMPRESS_MODE_NONE,
                                                                                      DEFAULT_ALIGN);
            pstcommVbAttr->blkCnt[pstcommVbAttr->validNum]  = 3;
            pstcommVbAttr->validNum++;

        }
    }
    if(HI_TRUE == pstParam->bWrapEn)
    {
        pstcommVbAttr->blkSize[pstcommVbAttr->validNum] = VPSS_GetWrapBufferSize(pstParam->stOutPutSize[pstParam->BigStreamId].u32Width,
                                                                                 pstParam->stOutPutSize[pstParam->BigStreamId].u32Height,
                                                                                 pstParam->WrapBufLine,
                                                                                 pstParam->enPixelFormat,DATA_BITWIDTH_8,COMPRESS_MODE_NONE,DEFAULT_ALIGN);
        pstcommVbAttr->blkCnt[pstcommVbAttr->validNum]  = 1;
        pstcommVbAttr->validNum++;
    }
    else
    {
        pstcommVbAttr->blkSize[pstcommVbAttr->validNum] = COMMON_GetPicBufferSize(pstParam->stOutPutSize[0].u32Width, pstParam->stOutPutSize[0].u32Height,
                                                                                  pstParam->enPixelFormat,
                                                                                  DATA_BITWIDTH_8,
                                                                                  pstParam->enCompressMode[0],
                                                                                  DEFAULT_ALIGN);

        if (pstParam->ViVpssMode == VI_ONLINE_VPSS_ONLINE)
        {
            pstcommVbAttr->blkCnt[pstcommVbAttr->validNum]  = 3;
        }
        else
        {
            pstcommVbAttr->blkCnt[pstcommVbAttr->validNum]  = 2;
        }

        pstcommVbAttr->validNum++;
    }



    pstcommVbAttr->blkSize[pstcommVbAttr->validNum] = COMMON_GetPicBufferSize(pstParam->stOutPutSize[1].u32Width, pstParam->stOutPutSize[1].u32Height,
                                                                              pstParam->enPixelFormat,
                                                                              DATA_BITWIDTH_8,
                                                                              pstParam->enCompressMode[1],
                                                                              DEFAULT_ALIGN);

    if (pstParam->ViVpssMode == VI_ONLINE_VPSS_ONLINE)
    {
        pstcommVbAttr->blkCnt[pstcommVbAttr->validNum]  = 3;
    }
    else
    {
        pstcommVbAttr->blkCnt[pstcommVbAttr->validNum]  = 2;
    }
    pstcommVbAttr->validNum++;


    //vgs dcf use
    if(HI_TRUE == bSupportDcf)
    {
        pstcommVbAttr->blkSize[pstcommVbAttr->validNum] = COMMON_GetPicBufferSize(160, 120,
                                                                                  pstParam->enPixelFormat,
                                                                                  DATA_BITWIDTH_8,
                                                                                  COMPRESS_MODE_NONE,
                                                                                  DEFAULT_ALIGN);
        pstcommVbAttr->blkCnt[pstcommVbAttr->validNum]  = 1;
        pstcommVbAttr->validNum++;
    }

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
            //stSnsSize.u32Width,stSnsSize.u32Height,stSize.u32Width,stSize.u32Height);
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
    PIC_SIZE_E      enSize[2]     = {BIG_STREAM_SIZE, SMALL_STREAM_SIZE};
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
    SAMPLE_VPSS_CHN_ATTR_S stParam;
    SAMPLE_VB_ATTR_S commVbAttr;

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

    SAMPLE_VENC_GetDefaultVpssAttr(stViConfig.astViInfo[0].stSnsInfo.enSnsType, abChnEnable, stSize, &stParam);

    /******************************************
      step 1: init sys alloc common vb
    ******************************************/
    memset(&commVbAttr, 0, sizeof(commVbAttr));
    commVbAttr.supplementConfig = HI_FALSE;
    SAMPLE_VENC_GetCommVbAttr(stViConfig.astViInfo[0].stSnsInfo.enSnsType, &stParam, HI_FALSE, &commVbAttr);

    s32Ret = SAMPLE_VENC_SYS_Init(&commVbAttr);
    if(s32Ret != HI_SUCCESS)
    {
        SAMPLE_PRT("Init SYS err for %#x!\n", s32Ret);
        return s32Ret;
    }

    stViConfig.s32WorkingViNum       = 1;
    stViConfig.astViInfo[0].stDevInfo.ViDev     = ViDev;
    stViConfig.astViInfo[0].stPipeInfo.aPipe[0] = ViPipe;
    stViConfig.astViInfo[0].stChnInfo.ViChn     = ViChn;
    stViConfig.astViInfo[0].stChnInfo.enDynamicRange = DYNAMIC_RANGE_SDR8;
    stViConfig.astViInfo[0].stChnInfo.enPixFormat    = PIXEL_FORMAT_YVU_SEMIPLANAR_420;

    s32Ret = SAMPLE_VENC_VI_Init(&stViConfig, stParam.ViVpssMode);
    if(s32Ret != HI_SUCCESS)
    {
        SAMPLE_PRT("Init VI err for %#x!\n", s32Ret);
        return HI_FAILURE;
    }

    s32Ret = SAMPLE_VENC_VPSS_Init(VpssGrp,&stParam);
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
* function: IntraRefresh, H.265e + H264e@720P,H.265 Channel resolution adaptable with sensor
******************************************************************************/
HI_S32 SAMPLE_VENC_IntraRefresh(void)
{
    HI_S32 i;
    HI_S32 s32Ret;
    SIZE_S          stSize[2];
    PIC_SIZE_E      enSize[2]     = {BIG_STREAM_SIZE, SMALL_STREAM_SIZE};

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
    HI_BOOL         abChnEnable[VPSS_MAX_PHY_CHN_NUM] = {1,1,0};
    SAMPLE_VPSS_CHN_ATTR_S stParam;
    SAMPLE_VB_ATTR_S commVbAttr;

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

    SAMPLE_VENC_GetDefaultVpssAttr(stViConfig.astViInfo[0].stSnsInfo.enSnsType, abChnEnable, stSize, &stParam);

    /******************************************
      step 1: init sys alloc common vb
    ******************************************/
    memset(&commVbAttr, 0, sizeof(commVbAttr));
    commVbAttr.supplementConfig = HI_FALSE;
    SAMPLE_VENC_GetCommVbAttr(stViConfig.astViInfo[0].stSnsInfo.enSnsType, &stParam, HI_FALSE, &commVbAttr);


    s32Ret = SAMPLE_VENC_SYS_Init(&commVbAttr);
    if(s32Ret != HI_SUCCESS)
    {
        SAMPLE_PRT("Init SYS err for %#x!\n", s32Ret);
        return s32Ret;
    }

    stViConfig.s32WorkingViNum       = 1;
    stViConfig.astViInfo[0].stDevInfo.ViDev     = ViDev;
    stViConfig.astViInfo[0].stPipeInfo.aPipe[0] = ViPipe;
    stViConfig.astViInfo[0].stChnInfo.ViChn     = ViChn;
    stViConfig.astViInfo[0].stChnInfo.enDynamicRange = DYNAMIC_RANGE_SDR8;
    stViConfig.astViInfo[0].stChnInfo.enPixFormat    = PIXEL_FORMAT_YVU_SEMIPLANAR_420;
    s32Ret = SAMPLE_VENC_VI_Init(&stViConfig, stParam.ViVpssMode);
    if(s32Ret != HI_SUCCESS)
    {
        SAMPLE_PRT("Init VI err for %#x!\n", s32Ret);
        return HI_FAILURE;
    }

    s32Ret = SAMPLE_VENC_VPSS_Init(VpssGrp, &stParam);
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
    PIC_SIZE_E      enSize[2]     = {BIG_STREAM_SIZE, SMALL_STREAM_SIZE};

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
    HI_BOOL         abChnEnable[VPSS_MAX_PHY_CHN_NUM] = {1,1,0};
    SAMPLE_VPSS_CHN_ATTR_S stParam;
    SAMPLE_VB_ATTR_S commVbAttr;

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

    SAMPLE_VENC_GetDefaultVpssAttr(stViConfig.astViInfo[0].stSnsInfo.enSnsType, abChnEnable, stSize, &stParam);

    /******************************************
      step 1: init sys alloc common vb
    ******************************************/
    memset(&commVbAttr, 0, sizeof(commVbAttr));
    commVbAttr.supplementConfig = HI_FALSE;
    SAMPLE_VENC_GetCommVbAttr(stViConfig.astViInfo[0].stSnsInfo.enSnsType, &stParam, HI_FALSE, &commVbAttr);


    s32Ret = SAMPLE_VENC_SYS_Init(&commVbAttr);
    if(s32Ret != HI_SUCCESS)
    {
        SAMPLE_PRT("Init SYS err for %#x!\n", s32Ret);
        return s32Ret;
    }


    stViConfig.s32WorkingViNum       = 1;
    stViConfig.astViInfo[0].stDevInfo.ViDev     = ViDev;
    stViConfig.astViInfo[0].stPipeInfo.aPipe[0] = ViPipe;
    stViConfig.astViInfo[0].stChnInfo.ViChn     = ViChn;
    stViConfig.astViInfo[0].stChnInfo.enDynamicRange = DYNAMIC_RANGE_SDR8;
    stViConfig.astViInfo[0].stChnInfo.enPixFormat    = PIXEL_FORMAT_YVU_SEMIPLANAR_420;
    s32Ret = SAMPLE_VENC_VI_Init(&stViConfig, stParam.ViVpssMode);
    if(s32Ret != HI_SUCCESS)
    {
        SAMPLE_PRT("Init VI err for %#x!\n", s32Ret);
        return HI_FAILURE;
    }

    s32Ret = SAMPLE_VENC_VPSS_Init(VpssGrp, &stParam);
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
* function: Qpmap, H.265e@1080P + H264e@1080P,H.265 Channel resolution adaptable with sensor
******************************************************************************/
HI_S32 SAMPLE_VENC_Qpmap(void)
{
    HI_S32 i;
    HI_S32 s32Ret;
    SIZE_S          stSize[2];
    PIC_SIZE_E      enSize[2]     = {BIG_STREAM_SIZE, SMALL_STREAM_SIZE};

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
    HI_BOOL         abChnEnable[VPSS_MAX_PHY_CHN_NUM] = {1,1,0};
    SAMPLE_VPSS_CHN_ATTR_S stParam;
    SAMPLE_VB_ATTR_S commVbAttr;

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

    SAMPLE_VENC_GetDefaultVpssAttr(stViConfig.astViInfo[0].stSnsInfo.enSnsType, abChnEnable, stSize, &stParam);

    /******************************************
      step 1: init sys alloc common vb
    ******************************************/
    memset(&commVbAttr, 0, sizeof(commVbAttr));
    commVbAttr.supplementConfig = HI_FALSE;
    SAMPLE_VENC_GetCommVbAttr(stViConfig.astViInfo[0].stSnsInfo.enSnsType, &stParam, HI_FALSE, &commVbAttr);


    s32Ret = SAMPLE_VENC_SYS_Init(&commVbAttr);
    if(s32Ret != HI_SUCCESS)
    {
        SAMPLE_PRT("Init SYS err for %#x!\n", s32Ret);
        return s32Ret;
    }

    /******************************************
      step 2: init and start vi
    ******************************************/
    stViConfig.s32WorkingViNum       = 1;
    stViConfig.astViInfo[0].stDevInfo.ViDev     = ViDev;
    stViConfig.astViInfo[0].stPipeInfo.aPipe[0] = ViPipe;
    stViConfig.astViInfo[0].stChnInfo.ViChn     = ViChn;
    stViConfig.astViInfo[0].stChnInfo.enDynamicRange = DYNAMIC_RANGE_SDR8;
    stViConfig.astViInfo[0].stChnInfo.enPixFormat    = PIXEL_FORMAT_YVU_SEMIPLANAR_420;
    s32Ret = SAMPLE_VENC_VI_Init(&stViConfig, stParam.ViVpssMode);
    if(s32Ret != HI_SUCCESS)
    {
        SAMPLE_PRT("Init VI err for %#x!\n", s32Ret);
        return HI_FAILURE;
    }

    /******************************************
      step 3: init and start vpss
    ******************************************/
    s32Ret = SAMPLE_VENC_VPSS_Init(VpssGrp, &stParam);
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

    s32Ret = SAMPLE_COMM_VENC_Start(VencChn[1], enPayLoad[1],enSize[1], enRcMode,u32Profile[1],bRcnRefShareBuf,&stGopAttr);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("Venc Start failed for %#x!\n", s32Ret);
        goto EXIT_VENC_H265_STOP;
    }

    s32Ret = SAMPLE_COMM_VENC_QpmapSendFrame(VpssGrp,VpssChn,VencChn,s32ChnNum,stSize);
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
    PIC_SIZE_E      enSize[2]     = {SMALL_STREAM_SIZE, BIG_STREAM_SIZE};
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
    HI_BOOL         abChnEnable[VPSS_MAX_PHY_CHN_NUM] = {1,1,0};
    SAMPLE_VPSS_CHN_ATTR_S stParam;
    SAMPLE_VB_ATTR_S commVbAttr;

    /******************************************
      step 0: Initialize related parameters
    ******************************************/
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

    s32Ret = SAMPLE_VENC_CheckSensor(stViConfig.astViInfo[0].stSnsInfo.enSnsType,stSize[1]);
    if(s32Ret != HI_SUCCESS)
    {
        for (i = 1; i < s32ChnNum; i++)
        {
            s32Ret = SAMPLE_VENC_ModifyResolution(stViConfig.astViInfo[0].stSnsInfo.enSnsType,&enSize[i],&stSize[i]);
            if(s32Ret != HI_SUCCESS)
            {
                return HI_FAILURE;
            }
        }
    }

    SAMPLE_VENC_GetDefaultVpssAttr(stViConfig.astViInfo[0].stSnsInfo.enSnsType, abChnEnable, stSize, &stParam);
    stParam.stOutPutSize[0]  = stSize[1];
    stParam.stOutPutSize[1]  = stSize[0];
    stParam.enCompressMode[0] = COMPRESS_MODE_NONE;
    stParam.enCompressMode[1] = COMPRESS_MODE_NONE;

    /******************************************
      step 1: init sys alloc common vb
    ******************************************/
    memset(&commVbAttr, 0, sizeof(commVbAttr));
    commVbAttr.supplementConfig = VB_SUPPLEMENT_JPEG_MASK;
    SAMPLE_VENC_GetCommVbAttr(stViConfig.astViInfo[0].stSnsInfo.enSnsType, &stParam, bSupportDcf, &commVbAttr);

    s32Ret = SAMPLE_VENC_SYS_Init(&commVbAttr);
    if(s32Ret != HI_SUCCESS)
    {
        SAMPLE_PRT("Init SYS err for %#x!\n", s32Ret);
        return s32Ret;
    }

    /******************************************
      step 2: init and start vi
    ******************************************/
    stViConfig.s32WorkingViNum       = 1;
    stViConfig.astViInfo[0].stDevInfo.ViDev     = ViDev;
    stViConfig.astViInfo[0].stPipeInfo.aPipe[0] = ViPipe;
    stViConfig.astViInfo[0].stChnInfo.ViChn     = ViChn;
    stViConfig.astViInfo[0].stChnInfo.enDynamicRange = DYNAMIC_RANGE_SDR8;
    stViConfig.astViInfo[0].stChnInfo.enPixFormat    = PIXEL_FORMAT_YVU_SEMIPLANAR_420;
    s32Ret = SAMPLE_VENC_VI_Init(&stViConfig, stParam.ViVpssMode);
    if(s32Ret != HI_SUCCESS)
    {
        SAMPLE_PRT("Init VI err for %#x!\n", s32Ret);
        return HI_FAILURE;
    }

    /******************************************
      step 3: init and start vpss
    ******************************************/
    s32Ret = SAMPLE_VENC_VPSS_Init(VpssGrp, &stParam);
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

    s32Ret = SAMPLE_COMM_VPSS_Bind_VENC(VpssGrp, VpssChn[1],VencChn[0]);
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

    s32Ret = SAMPLE_COMM_VPSS_Bind_VENC(VpssGrp, VpssChn[0],VencChn[1]);
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
    printf("press 'q' to exit snap!\nperess any other key to capture one picture to file\n");
    i = 0;
    while ((ch = (char)getchar()) != 'q')
    {
        printf("prepare to snap one picture! Please wait...\n");
        s32Ret = SAMPLE_COMM_VENC_SnapProcess(VencChn[1], 1, HI_TRUE, HI_TRUE);
        if (HI_SUCCESS != s32Ret)
        {
            SAMPLE_PRT("%s: sanp process failed!\n", __FUNCTION__);
            break;
        }
        printf("snap %d success!\n", i);
        i++;

        printf("\npress 'q' to exit snap!\nperess any other key to capture one picture to file\n");
    }
    printf("exit this sample!\n");

    /******************************************
     exit process
    ******************************************/
    SAMPLE_COMM_VENC_StopGetStream();

EXIT_VENC_JPEGE_UnBind:
    SAMPLE_COMM_VPSS_UnBind_VENC(VpssGrp,VpssChn[0],VencChn[1]);
EXIT_VENC_JPEGE_STOP:
    SAMPLE_COMM_VENC_Stop(VencChn[1]);
EXIT_VENC_MJPEGE_UnBind:
    SAMPLE_COMM_VPSS_UnBind_VENC(VpssGrp,VpssChn[1],VencChn[0]);
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
* function: H265(Large Stream)+H264(Small Stream) + Jpeg lowdelay Encode With RingBuf
******************************************************************************/
HI_S32 SAMPLE_VENC_Ring(void)
{
    HI_S32 i;
    HI_CHAR  ch;
    HI_S32 s32Ret;
    SIZE_S          stSize[3];
    PIC_SIZE_E      enSize[3]     = {BIG_STREAM_SIZE, BIG_STREAM_SIZE,SMALL_STREAM_SIZE};
    HI_S32          s32ChnNum     = 3;
    VENC_CHN        VencChn[3]    = {0,1,2};
    VENC_CHN        VideoChn[2]    = {0,2};
    HI_U32          u32Profile[3] = {0,0,0};
    PAYLOAD_TYPE_E  enPayLoad[3]  = {PT_H265, PT_JPEG, PT_H264};
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
    HI_BOOL         abChnEnable[VPSS_MAX_PHY_CHN_NUM] = {1, 1, 0};
    HI_BOOL         bSupportDcf    = HI_FALSE;    //Ring not support DCF and MPF
    SAMPLE_VPSS_CHN_ATTR_S stParam;
    VI_VPSS_MODE_E  ViVpssMode = VI_ONLINE_VPSS_ONLINE;  //
    SAMPLE_VB_ATTR_S commVbAttr;

    /******************************************
      step 0: Initialize related parameters
    ******************************************/
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

    /*modify chan0(h265) and chan1(jpeg) encode Size by sensor capability*/
    for (i = 0; i < 2; i++)
    {
        s32Ret = SAMPLE_VENC_CheckSensor(stViConfig.astViInfo[0].stSnsInfo.enSnsType,stSize[i]);
        if(s32Ret != HI_SUCCESS)
        {
            s32Ret = SAMPLE_VENC_ModifyResolution(stViConfig.astViInfo[0].stSnsInfo.enSnsType,&enSize[i],&stSize[i]);
            if(s32Ret != HI_SUCCESS)
            {
                return HI_FAILURE;
            }
        }
    }

    SAMPLE_VENC_GetDefaultVpssAttr(stViConfig.astViInfo[0].stSnsInfo.enSnsType, abChnEnable, stSize, &stParam);
    stParam.ViVpssMode = ViVpssMode;
    stParam.BigStreamId   = 0;
    stParam.SmallStreamId = 1;
    stParam.bWrapEn       = HI_TRUE;
    //modify vpss chan[1] attr ==> which bind with SmallStream Venc chan[2]
    stParam.stOutPutSize[stParam.SmallStreamId].u32Width   = stSize[2].u32Width;
    stParam.stOutPutSize[stParam.SmallStreamId].u32Height  = stSize[2].u32Height;
    stParam.enCompressMode[stParam.SmallStreamId]          = COMPRESS_MODE_NONE;

    GetVpssWrapBufLine(&stParam);
    if (stParam.WrapBufLine == 0)
    {
        return HI_FAILURE;
    }

    /******************************************
      step 1: init sys alloc common vb
    ******************************************/
    memset(&commVbAttr, 0, sizeof(commVbAttr));
    commVbAttr.supplementConfig = HI_FALSE;
    SAMPLE_VENC_GetCommVbAttr(stViConfig.astViInfo[0].stSnsInfo.enSnsType, &stParam, HI_FALSE, &commVbAttr);

    s32Ret = SAMPLE_VENC_SYS_Init(&commVbAttr);
    if(s32Ret != HI_SUCCESS)
    {
        SAMPLE_PRT("Init SYS err for %#x!\n", s32Ret);
        return s32Ret;
    }

    /******************************************
      step 2: init and start vi
    ******************************************/
    stViConfig.s32WorkingViNum                  = 1;
    stViConfig.astViInfo[0].stDevInfo.ViDev     = ViDev;
    stViConfig.astViInfo[0].stPipeInfo.aPipe[0] = ViPipe;
    stViConfig.astViInfo[0].stChnInfo.ViChn     = ViChn;
    stViConfig.astViInfo[0].stChnInfo.enDynamicRange = DYNAMIC_RANGE_SDR8;
    stViConfig.astViInfo[0].stChnInfo.enPixFormat    = PIXEL_FORMAT_YVU_SEMIPLANAR_420;
    s32Ret = SAMPLE_VENC_VI_Init(&stViConfig, ViVpssMode);
    if(s32Ret != HI_SUCCESS)
    {
        SAMPLE_PRT("Init VI err for %#x!\n", s32Ret);
        return HI_FAILURE;
    }

    /******************************************
      step 3: init and start vpss: One group with two channel(0:BigStream  1:SmallStream)
    ******************************************/
    s32Ret = SAMPLE_VENC_VPSS_Init(VpssGrp, &stParam);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("Init VPSS err for %#x!\n", s32Ret);
        goto EXIT_VI_STOP;
    }

    /******************************************
      step 4: Bind VI and VPSS
    ******************************************/
    s32Ret = SAMPLE_COMM_VI_Bind_VPSS(ViPipe, ViChn, VpssGrp);
    if(s32Ret != HI_SUCCESS)
    {
        SAMPLE_PRT("VI Bind VPSS err for %#x!\n", s32Ret);
        goto EXIT_VPSS_STOP;
    }

    /******************************************
     step 5: start VENC (0: H265 BigStream;  1: JPEG snap with BigStream;  2: H264 SmallStream)
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

    /***start h.265 bigStream encoder **/
    s32Ret = SAMPLE_COMM_VENC_Start(VencChn[0], enPayLoad[0],enSize[0], enRcMode,u32Profile[0],bRcnRefShareBuf,&stGopAttr);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("Venc Start failed for %#x!\n", s32Ret);
        goto EXIT_VI_VPSS_UNBIND;
    }

    /***start jpeg snap channel smallStream encoder **/
    s32Ret = SAMPLE_COMM_VENC_SnapStart(VencChn[1], &stSize[1], bSupportDcf);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("Venc Start failed for %#x!\n", s32Ret);
        goto EXIT_VENC_H265_STOP;
    }

    /***start h.264 smallStream encoder **/
    s32Ret = SAMPLE_COMM_VENC_Start(VencChn[2], enPayLoad[2],enSize[2], enRcMode,u32Profile[2],bRcnRefShareBuf,&stGopAttr);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("Venc Start failed for %#x!\n", s32Ret);
        goto EXIT_VENC_JPEG_STOP;
    }

    /******************************************
     step 6: Bind VPSS and VENC( BigStream attach Vpss chan[0], SmallStream attch Vpss chan[1])
    ******************************************/
    s32Ret = SAMPLE_COMM_VPSS_Bind_VENC(VpssGrp, VpssChn[0],VencChn[0]);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("Venc Get GopAttr failed for %#x!\n", s32Ret);
        goto EXIT_VENC_H264_STOP;
    }

    s32Ret = SAMPLE_COMM_VPSS_Bind_VENC(VpssGrp, VpssChn[0],VencChn[1]);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("Venc bind Vpss failed for %#x!\n", s32Ret);
        goto EXIT_VENC_H265_UnBind;
    }

    s32Ret = SAMPLE_COMM_VPSS_Bind_VENC(VpssGrp, VpssChn[1],VencChn[2]);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("Venc Get GopAttr failed for %#x!\n", s32Ret);
        goto EXIT_VENC_JPEG_UnBind;
    }

    /******************************************
     step 7: start get video stream (this thread just Get video channel stream(venc chan[0] and chan[2]))
    ******************************************/
    s32Ret = SAMPLE_COMM_VENC_StartGetStream(VideoChn, 2);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("Start Venc failed!\n");
        goto EXIT_VENC_H264_UnBind;
    }

    /******************************************
     step 8: snap picture process
    ******************************************/
    printf("please press 'q' to exit this sample, press any other key to snap one picture to file!\n");
    i = 0;
    while ((ch = (char)getchar()) != 'q')
    {
        printf("prepare to snap one picture! Please wait...\n");
        s32Ret = SAMPLE_COMM_VENC_SnapProcess(VencChn[1], 1, HI_TRUE, HI_FALSE);
        if (HI_SUCCESS != s32Ret)
        {
            SAMPLE_PRT("%s: sanp process failed!\n", __FUNCTION__);
            break;
        }
        printf("snap %d success!\n", i);
        i++;

        printf("\npress 'q' to exit snap!\nperess any other key to capture one picture to file\n");
    }

    printf("exit this sample!\n");
    /******************************************
     exit process
    ******************************************/
    SAMPLE_COMM_VENC_StopGetStream();

EXIT_VENC_H264_UnBind:
    SAMPLE_COMM_VPSS_UnBind_VENC(VpssGrp,VpssChn[1],VencChn[2]);
EXIT_VENC_JPEG_UnBind:
    SAMPLE_COMM_VPSS_UnBind_VENC(VpssGrp,VpssChn[0],VencChn[1]);
EXIT_VENC_H265_UnBind:
    SAMPLE_COMM_VPSS_UnBind_VENC(VpssGrp,VpssChn[0],VencChn[0]);
EXIT_VENC_H264_STOP:
    SAMPLE_COMM_VENC_Stop(VencChn[2]);
EXIT_VENC_JPEG_STOP:
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

    if (argc < 2)
    {
        SAMPLE_VENC_Usage(argv[0]);
        return HI_FAILURE;
    }
    u32Index = atoi(argv[1]);

#ifndef __HuaweiLite__
    signal(SIGINT, SAMPLE_VENC_HandleSig);
    signal(SIGTERM, SAMPLE_VENC_HandleSig);
#endif

    switch (u32Index)
    {
        case 0:
            s32Ret = SAMPLE_VENC_Ring();
            break;
        case 1:
            s32Ret = SAMPLE_VENC_H265_H264();
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
