/******************************************************************************
  A simple program of Hisilicon Hi35xx video encode implementation.
  Copyright (C), 2018, Hisilicon Tech. Co., Ltd.
******************************************************************************/

#include <unistd.h>
#include <pthread.h>

#include "histream.h"
#include "uvc_venc_glue.h"
#include "frame_cache.h"

#if 0 //maohw

#include "mpi_vpss.h"
#include "sample_comm.h"
#include "sample_venc.h"
#include "sample_yuv.h"


#ifndef __HuaweiLite__
#include "log.h"
#include "config_svc.h"
#endif

#define PIC_SIZE PIC_720P

#define v4l2_fourcc(a, b, c, d) \
    ((uint32_t)(a) | ((uint32_t)(b) << 8) | ((uint32_t)(c) << 16) | ((uint32_t)(d) << 24))

#define V4L2_PIX_FMT_YUYV v4l2_fourcc('Y', 'U', 'Y', 'V')   /* 16  YUV 4:2:2     */
#define V4L2_PIX_FMT_YUV420 v4l2_fourcc('Y', 'U', '1', '2') /* 16  YUV 4:2:0     */
#define V4L2_PIX_FMT_MJPEG v4l2_fourcc('M', 'J', 'P', 'G')  /* Motion-JPEG   */
#define V4L2_PIX_FMT_H264 v4l2_fourcc('H', '2', '6', '4')   /* H264 with start codes */

static SAMPLE_VI_CONFIG_S g_stViConfig;
static HI_U32 g_u32SupplementConfig = HI_FALSE;
static PIC_SIZE_E g_enSize = PIC_SIZE;
static HI_S32 g_s32ChnNum;
static HI_U8 __started = 0;
extern encoder_property __encoder_property;
extern HI_S32 __SAMPLE_StartGetYUV(HI_S32 s32VpssGrp, HI_S32 s32VpssChn);
extern HI_S32 __SAMPLE_StopGetYUV(HI_VOID);

static HI_S32 __SAMPLE_VENC_VPSS_Init(VPSS_GRP VpssGrp, HI_BOOL *pabChnEnable, DYNAMIC_RANGE_E enDynamicRange,
                                      PIXEL_FORMAT_E enPixelFormat, SIZE_S stSize[], SAMPLE_SNS_TYPE_E enSnsType)
{
    HI_S32 i;
    HI_S32 s32Ret;
    PIC_SIZE_E enSnsSize;
    SIZE_S stSnsSize;
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
    stVpssGrpAttr.enPixelFormat = enPixelFormat;
    stVpssGrpAttr.u32MaxW = stSnsSize.u32Width;
    stVpssGrpAttr.u32MaxH = stSnsSize.u32Height;
    stVpssGrpAttr.stFrameRate.s32SrcFrameRate = -1;
    stVpssGrpAttr.stFrameRate.s32DstFrameRate = -1;
    stVpssGrpAttr.bNrEn = HI_TRUE;
    stVpssGrpAttr.stNrAttr.enNrType = VPSS_NR_TYPE_VIDEO;
    stVpssGrpAttr.stNrAttr.enNrMotionMode = NR_MOTION_MODE_NORMAL;
    stVpssGrpAttr.stNrAttr.enCompressMode = COMPRESS_MODE_FRAME;

    for (i = 0; i < VPSS_MAX_PHY_CHN_NUM; ++i)
    {
        if (HI_TRUE == pabChnEnable[i])
        {
            stVpssChnAttr[i].u32Width = stSize[i].u32Width;
            stVpssChnAttr[i].u32Height = stSize[i].u32Height;
            stVpssChnAttr[i].enChnMode = VPSS_CHN_MODE_USER;
            stVpssChnAttr[i].enCompressMode = COMPRESS_MODE_NONE;
            stVpssChnAttr[i].enDynamicRange = enDynamicRange;
            stVpssChnAttr[i].enPixelFormat = enPixelFormat;
            stVpssChnAttr[i].stFrameRate.s32SrcFrameRate = -1;
            stVpssChnAttr[i].stFrameRate.s32DstFrameRate = -1;
            stVpssChnAttr[i].u32Depth = 0;
            stVpssChnAttr[i].bMirror = HI_FALSE;
            stVpssChnAttr[i].bFlip = HI_FALSE;
            stVpssChnAttr[i].enVideoFormat = VIDEO_FORMAT_LINEAR;
            stVpssChnAttr[i].stAspectRatio.enMode = ASPECT_RATIO_NONE;
        }
    }

    s32Ret = SAMPLE_COMM_VPSS_Start(VpssGrp, pabChnEnable, &stVpssGrpAttr, stVpssChnAttr);
    if (s32Ret != HI_SUCCESS)
    {
        SAMPLE_PRT("start VPSS fail for %#x!\n", s32Ret);
    }

    return s32Ret;
}

static HI_S32 __SAMPLE_VENC_CheckSensor(SAMPLE_SNS_TYPE_E enSnsType, SIZE_S stSize)
{
    HI_S32 s32Ret;
    SIZE_S stSnsSize;
    PIC_SIZE_E enSnsSize;

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

    if ((stSnsSize.u32Width < stSize.u32Width) || (stSnsSize.u32Height < stSize.u32Height))
    {
        SAMPLE_PRT("Sensor size is (%d,%d), but encode chnl is (%d,%d)!\n",
                   stSnsSize.u32Width, stSnsSize.u32Height, stSize.u32Width, stSize.u32Height);
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

static HI_S32 __SAMPLE_VENC_SYS_Init(HI_U32 u32SupplementConfig, SAMPLE_SNS_TYPE_E enSnsType)
{
    HI_S32 s32Ret;
    HI_U64 u64BlkSize;
    VB_CONFIG_S stVbConf;
    PIC_SIZE_E enSnsSize;
    SIZE_S stSnsSize;

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

    u64BlkSize = COMMON_GetPicBufferSize(stSnsSize.u32Width, stSnsSize.u32Height, PIXEL_FORMAT_YVU_SEMIPLANAR_422, DATA_BITWIDTH_8, COMPRESS_MODE_NONE, DEFAULT_ALIGN);
    stVbConf.astCommPool[0].u64BlkSize = u64BlkSize;
    stVbConf.astCommPool[0].u32BlkCnt = 3;
    stVbConf.u32MaxPoolCnt = 1;

    if (0 == u32SupplementConfig)
    {
        s32Ret = SAMPLE_COMM_SYS_Init(&stVbConf);
    }
    else
    {
        s32Ret = SAMPLE_COMM_SYS_InitWithVbSupplement(&stVbConf, u32SupplementConfig);
    }

    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("SAMPLE_COMM_SYS_GetPicSize failed!\n");
        return s32Ret;
    }

    return HI_SUCCESS;
}

static HI_S32 __SAMPLE_VENC_VI_Init(SAMPLE_VI_CONFIG_S *pstViConfig, HI_U32 u32SupplementConfig)
{
    HI_S32 s32Ret;
    SAMPLE_SNS_TYPE_E enSnsType;
    ISP_CTRL_PARAM_S stIspCtrlParam;
    HI_U32 u32FrameRate;

    enSnsType = pstViConfig->astViInfo[0].stSnsInfo.enSnsType;

    pstViConfig->as32WorkingViId[0] = 0;
    pstViConfig->astViInfo[0].stSnsInfo.MipiDev = SAMPLE_COMM_VI_GetComboDevBySensor(pstViConfig->astViInfo[0].stSnsInfo.enSnsType, 0);
    pstViConfig->astViInfo[0].stSnsInfo.s32BusId = 0;
    pstViConfig->astViInfo[0].stDevInfo.enWDRMode = WDR_MODE_NONE;
    pstViConfig->astViInfo[0].stPipeInfo.enMastPipeMode = VI_ONLINE_VPSS_ONLINE;
    pstViConfig->astViInfo[0].stPipeInfo.aPipe[1] = -1;
    pstViConfig->astViInfo[0].stChnInfo.enVideoFormat = VIDEO_FORMAT_LINEAR;
    pstViConfig->astViInfo[0].stChnInfo.enCompressMode = COMPRESS_MODE_NONE;

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
    stIspCtrlParam.u32StatIntvl = u32FrameRate / 30;

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

static HI_S32 __SAMPLE_VENC_NORMALP_CLASSIC(HI_VOID)
{
    HI_S32 s32Ret = HI_FAILURE;

    SIZE_S stSize[3];
    HI_S32 s32ChnNum = 1;
    VENC_CHN VencChn[2] = {0, 1};
    HI_U32 u32Profile = 0;
    PAYLOAD_TYPE_E enPayLoad = PT_H264;
    VENC_GOP_MODE_E enGopMode = VENC_GOPMODE_NORMALP;
    VENC_GOP_ATTR_S stGopAttr;
    SAMPLE_RC_E enRcMode = SAMPLE_RC_CBR;
    HI_BOOL bRcnRefShareBuf = HI_TRUE;

    VPSS_GRP VpssGrp = 0;
    VPSS_CHN VpssChn = 1;
    HI_BOOL abChnEnable[3] = {0, 1, 0};
    PIXEL_FORMAT_E enPixFormat = PIXEL_FORMAT_YVU_SEMIPLANAR_420;

    set_user_config_format(&enPayLoad, &g_enSize, &s32ChnNum);

    if (V4L2_PIX_FMT_YUYV == __encoder_property.format)
    {
        enPixFormat = PIXEL_FORMAT_YVU_SEMIPLANAR_422;
    }

    s32Ret = SAMPLE_COMM_SYS_GetPicSize(g_enSize, &stSize[VpssChn]);
    if (s32Ret != HI_SUCCESS)
    {
        SAMPLE_PRT("SAMPLE_COMM_SYS_GetPicSize failed!\n");
        return s32Ret;
    }

    SAMPLE_COMM_VI_GetSensorInfo(&g_stViConfig);
    if (SAMPLE_SNS_TYPE_BUTT == g_stViConfig.astViInfo[0].stSnsInfo.enSnsType)
    {
        SAMPLE_PRT("Not set SENSOR%d_TYPE !\n", 0);
        return HI_FAILURE;
    }

    s32Ret = __SAMPLE_VENC_CheckSensor(g_stViConfig.astViInfo[0].stSnsInfo.enSnsType, stSize[VpssChn]);
    if (s32Ret != HI_SUCCESS)
    {
        SAMPLE_PRT("Check Sensor err!\n");
        return HI_FAILURE;
    }

    /******************************************
     start SYS VI VPSS
    ******************************************/
    g_stViConfig.s32WorkingViNum = 1;
    g_stViConfig.astViInfo[0].stDevInfo.ViDev = 0;
    g_stViConfig.astViInfo[0].stPipeInfo.aPipe[0] = 0;
    g_stViConfig.astViInfo[0].stChnInfo.ViChn = 0;
    g_stViConfig.astViInfo[0].stChnInfo.enDynamicRange = DYNAMIC_RANGE_SDR8;
    g_stViConfig.astViInfo[0].stChnInfo.enPixFormat = enPixFormat;
    s32Ret = __SAMPLE_VENC_VI_Init(&g_stViConfig, g_u32SupplementConfig);
    if (s32Ret != HI_SUCCESS)
    {
        SAMPLE_PRT("Init VI err for %#x!\n", s32Ret);
        return HI_FAILURE;
    }

    s32Ret = __SAMPLE_VENC_VPSS_Init(VpssGrp, abChnEnable, DYNAMIC_RANGE_SDR8, enPixFormat,
                                     stSize, g_stViConfig.astViInfo[0].stSnsInfo.enSnsType);
    if (s32Ret != HI_SUCCESS)
    {
        SAMPLE_PRT("Init VPSS err for %#x!\n", s32Ret);
        goto EXIT_VI_STOP;
    }

    s32Ret = SAMPLE_COMM_VI_Bind_VPSS(0, 0, VpssGrp);
    if (s32Ret != HI_SUCCESS)
    {
        SAMPLE_PRT("VI Bind VPSS err for %#x!\n", s32Ret);
        goto EXIT_VPSS_STOP;
    }

    if (__encoder_property.format != V4L2_PIX_FMT_YUYV && __encoder_property.format != V4L2_PIX_FMT_YUV420)
    {
        enRcMode = SAMPLE_RC_CBR;
        if (enPayLoad == PT_H264)
        {
            enRcMode = SAMPLE_RC_AVBR;
        }

        /******************************************
         start stream venc
        ******************************************/
        s32Ret = SAMPLE_COMM_VENC_GetGopAttr(enGopMode, &stGopAttr);
        if (HI_SUCCESS != s32Ret)
        {
            SAMPLE_PRT("Venc Get GopAttr for %#x!\n", s32Ret);
            goto EXIT_VI_VPSS_UNBIND;
        }
        if (VENC_GOPMODE_BIPREDB == enGopMode)
        {
            SAMPLE_PRT("BIPREDB and ADVSMARTP not support lowdelay!\n");
            goto EXIT_VI_VPSS_UNBIND;
        }

        s32Ret = SAMPLE_COMM_VENC_Start(VencChn[0], enPayLoad, g_enSize, enRcMode, u32Profile, bRcnRefShareBuf, &stGopAttr);
        if (HI_SUCCESS != s32Ret)
        {
            SAMPLE_PRT("Venc Start failed for %#x!\n", s32Ret);
            goto EXIT_VI_VPSS_UNBIND;
        }

        s32Ret = SAMPLE_COMM_VPSS_Bind_VENC(VpssGrp, VpssChn, VencChn[0]);
        if (HI_SUCCESS != s32Ret)
        {
            SAMPLE_PRT("Venc Get GopAttr failed for %#x!\n", s32Ret);
            goto EXIT_VENC_STOP;
        }

        /******************************************
         stream save process
        ******************************************/
        g_s32ChnNum = s32ChnNum;
        s32Ret = __SAMPLE_COMM_VENC_StartGetStream(VencChn, s32ChnNum);
        if (HI_SUCCESS != s32Ret)
        {
            SAMPLE_PRT("Start Venc failed!\n");
            goto EXIT_VPSS_VENC_UNBIND;
        }
    }
    else
    {
        s32Ret = __SAMPLE_StartGetYUV(VpssGrp, VpssChn);
        if (HI_SUCCESS != s32Ret)
        {
            SAMPLE_PRT("Start get YUV failed!\n");
            goto EXIT_VI_VPSS_UNBIND;
        }
    }

    return s32Ret;

EXIT_VPSS_VENC_UNBIND:
    SAMPLE_COMM_VPSS_UnBind_VENC(VpssGrp, VpssChn, VencChn[0]);
EXIT_VENC_STOP:
    SAMPLE_COMM_VENC_Stop(VencChn[0]);
EXIT_VI_VPSS_UNBIND:
    SAMPLE_COMM_VI_UnBind_VPSS(0, 0, VpssGrp);
EXIT_VPSS_STOP:
    SAMPLE_COMM_VPSS_Stop(VpssGrp, abChnEnable);
EXIT_VI_STOP:
    SAMPLE_COMM_VI_StopVi(&g_stViConfig);
    SAMPLE_COMM_SYS_Exit();
    return s32Ret;
}

/*******************************************************
 * Processing Unit Operation Functions
 *******************************************************/
static uint16_t _venc_brightness_get(HI_VOID)
{
    ISP_CSC_ATTR_S stCSCAttr;

    HI_MPI_ISP_GetCSCAttr(0, &stCSCAttr);

    return (uint16_t)stCSCAttr.u8Luma;
}

static uint16_t _venc_contrast_get(HI_VOID)
{
    ISP_CSC_ATTR_S stCSCAttr;

    HI_MPI_ISP_GetCSCAttr(0, &stCSCAttr);

    return (uint16_t)stCSCAttr.u8Contr;
}

static uint16_t _venc_hue_get(HI_VOID)
{
    ISP_CSC_ATTR_S stCSCAttr;

    HI_MPI_ISP_GetCSCAttr(0, &stCSCAttr);

    return (uint16_t)stCSCAttr.u8Hue;
}

static uint8_t _venc_power_line_frequency_get(HI_VOID)
{
    ISP_EXPOSURE_ATTR_S stExpAttr;

    HI_MPI_ISP_GetExposureAttr(0, &stExpAttr);

    if (stExpAttr.stAuto.stAntiflicker.u8Frequency != 50 &&
        stExpAttr.stAuto.stAntiflicker.u8Frequency != 60)
    {
        return 0;
    }

    return (stExpAttr.stAuto.stAntiflicker.u8Frequency == 50) ? 0x1 : 0x2;
}

static uint16_t _venc_saturation_get(HI_VOID)
{
    ISP_CSC_ATTR_S stCSCAttr;

    HI_MPI_ISP_GetCSCAttr(0, &stCSCAttr);

    return (uint16_t)stCSCAttr.u8Satu;
}

static uint8_t _venc_white_balance_temperature_auto_get(HI_VOID)
{
    ISP_WB_ATTR_S stWBAttr;

    HI_MPI_ISP_GetWBAttr(0, &stWBAttr);

    return (stWBAttr.enOpType == OP_TYPE_AUTO) ? 0x1 : 0x0;
}

static uint16_t _venc_white_balance_temperature_get(HI_VOID)
{
    ISP_WB_INFO_S stWBInfo;

    HI_MPI_ISP_QueryWBInfo(0, &stWBInfo);

    return (uint16_t)stWBInfo.u16ColorTemp;
}

static HI_VOID _venc_brightness_set(uint16_t v)
{
    ISP_CSC_ATTR_S stCSCAttr;

    HI_MPI_ISP_GetCSCAttr(0, &stCSCAttr);
    stCSCAttr.u8Luma = v;
    HI_MPI_ISP_SetCSCAttr(0, &stCSCAttr);
}

static HI_VOID _venc_contrast_set(uint16_t v)
{
    ISP_CSC_ATTR_S stCSCAttr;

    HI_MPI_ISP_GetCSCAttr(0, &stCSCAttr);
    stCSCAttr.u8Contr = v;
    HI_MPI_ISP_SetCSCAttr(0, &stCSCAttr);
}

static HI_VOID _venc_hue_set(uint16_t v)
{
    ISP_CSC_ATTR_S stCSCAttr;

    HI_MPI_ISP_GetCSCAttr(0, &stCSCAttr);
    stCSCAttr.u8Hue = v;
    HI_MPI_ISP_SetCSCAttr(0, &stCSCAttr);
}

static HI_VOID _venc_power_line_frequency_set(uint8_t v)
{
    HI_S32 s32Ret;
    ISP_EXPOSURE_ATTR_S stExpAttr;

    HI_MPI_ISP_GetExposureAttr(0, &stExpAttr);
    if (0 == v)
    {
        stExpAttr.stAuto.stAntiflicker.bEnable = HI_FALSE;
    }
    else if (1 == v)
    {
        stExpAttr.stAuto.stAntiflicker.bEnable = HI_TRUE;
        stExpAttr.stAuto.stAntiflicker.u8Frequency = 50;
    }
    else if (2 == v)
    {
        stExpAttr.stAuto.stAntiflicker.bEnable = HI_TRUE;
        stExpAttr.stAuto.stAntiflicker.u8Frequency = 60;
    }

    s32Ret = HI_MPI_ISP_SetExposureAttr(0, &stExpAttr);
    if (s32Ret != HI_SUCCESS)
    {
        printf("HI_MPI_ISP_SetExposureAttr err 0x%x\n", s32Ret);
    }
}

static HI_VOID _venc_saturation_set(uint16_t v)
{
    ISP_CSC_ATTR_S stCSCAttr;

    HI_MPI_ISP_GetCSCAttr(0, &stCSCAttr);
    stCSCAttr.u8Satu = v;
    HI_MPI_ISP_SetCSCAttr(0, &stCSCAttr);
}

static HI_VOID _venc_white_balance_temperature_auto_set(uint8_t v)
{
    ISP_WB_ATTR_S stWBAttr;

    HI_MPI_ISP_GetWBAttr(0, &stWBAttr);
    stWBAttr.enOpType = (v == 1) ? OP_TYPE_AUTO : OP_TYPE_MANUAL;
    HI_MPI_ISP_SetWBAttr(0, &stWBAttr);
}

static HI_VOID _venc_white_balance_temperature_set(uint16_t v)
{
    ISP_WB_INFO_S stWBInfo;
    ISP_WB_ATTR_S stWBAttr;
    HI_U16 u16ColorTemp;
    HI_U16 u16AWBGain[4];

    HI_MPI_ISP_QueryWBInfo(0, &stWBInfo);
    HI_MPI_ISP_GetWBAttr(0, &stWBAttr);

    u16ColorTemp = v;
    HI_MPI_ISP_CalGainByTemp(0, &stWBAttr, u16ColorTemp, 0, u16AWBGain);

    stWBAttr.enOpType = OP_TYPE_MANUAL;
    memcpy(&stWBAttr.stManual, u16AWBGain, sizeof(stWBAttr.stManual));

    HI_MPI_ISP_SetWBAttr(0, &stWBAttr);
}
/* Processing Unit Operation Functions End */

/*******************************************************
 * Input Terminal Operation Functions
 *******************************************************/
static uint8_t _venc_exposure_auto_mode_get(HI_VOID)
{
    ISP_EXPOSURE_ATTR_S stExpAttr;

    HI_MPI_ISP_GetExposureAttr(0, &stExpAttr);

    return (stExpAttr.enOpType == OP_TYPE_AUTO) ? 0x02 : 0x04;
}

static uint32_t _venc_exposure_ansolute_time_get(HI_VOID)
{
    ISP_EXPOSURE_ATTR_S stExpAttr;

    HI_MPI_ISP_GetExposureAttr(0, &stExpAttr);

    return stExpAttr.stManual.u32ExpTime;
}

static HI_VOID _venc_exposure_auto_mode_set(uint8_t v)
{
    ISP_EXPOSURE_ATTR_S stExpAttr;

    HI_MPI_ISP_GetExposureAttr(0, &stExpAttr);
    stExpAttr.enOpType = (v == 4) ? OP_TYPE_MANUAL : OP_TYPE_AUTO;
    HI_MPI_ISP_SetExposureAttr(0, &stExpAttr);
}

static HI_VOID _venc_exposure_ansolute_time_set(uint32_t v)
{
    ISP_EXPOSURE_ATTR_S stExpAttr;

    HI_MPI_ISP_GetExposureAttr(0, &stExpAttr);
    stExpAttr.stManual.u32ExpTime = v * 100;
    stExpAttr.stManual.enExpTimeOpType = OP_TYPE_MANUAL;
    HI_MPI_ISP_SetExposureAttr(0, &stExpAttr);
}
/* Input Terminal Operation Functions End */

/*******************************************************
 * Stream Control Operation Functions
 *******************************************************/
static HI_S32 sample_venc_set_idr(HI_VOID)
{
    return HI_MPI_VENC_RequestIDR(1, HI_TRUE);
}

static HI_S32 sample_venc_init(HI_VOID)
{
    HI_S32 s32Ret = HI_FAILURE;
    SAMPLE_SNS_TYPE_E enSnsType = SENSOR0_TYPE;

    s32Ret = __SAMPLE_VENC_SYS_Init(g_u32SupplementConfig, enSnsType);
    if (s32Ret != HI_SUCCESS)
    {
        SAMPLE_PRT("Init SYS err for %#x!\n", s32Ret);
        return s32Ret;
    }

    return 0;
}

static HI_S32 sample_venc_startup(HI_VOID)
{
    HI_S32 ret = HI_FAILURE;

    __started = 1;

    ret = __SAMPLE_VENC_NORMALP_CLASSIC();
    HI_MPI_VENC_RequestIDR(0, HI_TRUE);

    return ret;
}

static HI_S32 sample_venc_shutdown(HI_VOID)
{
    HI_S32 VpssGrp = 0;
    HI_S32 VpssChn = 1;
    HI_S32 VencChn = 0;
    HI_BOOL abChnEnable[3] = {0, 1, 0};

    if (!__started)
    {
        return 0;
    }

    if (__encoder_property.format != V4L2_PIX_FMT_YUYV && __encoder_property.format != V4L2_PIX_FMT_YUV420)
    {
        __SAMPLE_COMM_VENC_StopGetStream();
        SAMPLE_COMM_VPSS_UnBind_VENC(VpssGrp, VpssChn, VencChn);
        SAMPLE_COMM_VENC_Stop(VencChn);
    }
    else
    {
        __SAMPLE_StopGetYUV();
    }

    SAMPLE_COMM_VI_UnBind_VPSS(0, 0, VpssGrp);
    SAMPLE_COMM_VPSS_Stop(0, abChnEnable);
    SAMPLE_COMM_VI_StopVi(&g_stViConfig);

    __started = 0;

    return HI_SUCCESS;
}

static HI_S32 sample_venc_set_property(encoder_property *p)
{
    __encoder_property = *p;
    set_yuv_property();
    return 0;
}
/* Stream Control Operation Functions End */

static struct processing_unit_ops venc_pu_ops = {
    .brightness_get = _venc_brightness_get,
    .contrast_get = _venc_contrast_get,
    .hue_get = _venc_hue_get,
    .power_line_frequency_get = _venc_power_line_frequency_get,
    .saturation_get = _venc_saturation_get,
    .white_balance_temperature_auto_get = _venc_white_balance_temperature_auto_get,
    .white_balance_temperature_get = _venc_white_balance_temperature_get,

    .brightness_set = _venc_brightness_set,
    .contrast_set = _venc_contrast_set,
    .hue_set = _venc_hue_set,
    .power_line_frequency_set = _venc_power_line_frequency_set,
    .saturation_set = _venc_saturation_set,
    .white_balance_temperature_auto_set = _venc_white_balance_temperature_auto_set,
    .white_balance_temperature_set = _venc_white_balance_temperature_set,
};

static struct input_terminal_ops venc_it_ops = {
    .exposure_ansolute_time_get = _venc_exposure_ansolute_time_get,
    .exposure_auto_mode_get = _venc_exposure_auto_mode_get,
    .exposure_ansolute_time_set = _venc_exposure_ansolute_time_set,
    .exposure_auto_mode_set = _venc_exposure_auto_mode_set,
};

static struct stream_control_ops venc_sc_ops = {
    .init = sample_venc_init,
    .startup = sample_venc_startup,
    .shutdown = sample_venc_shutdown,
    .set_idr = sample_venc_set_idr,
    .set_property = sample_venc_set_property,
};
#else


static struct processing_unit_ops venc_pu_ops = { NULL };
static struct input_terminal_ops venc_it_ops = { NULL };


#include "mod/codec/inc/codec.h"

static pthread_t gpid;
static int running;

#include "inc/frm.h"

void* send_task(void *parm)
{
  printf("start.\n");
  int i = 0, ret = 0, waitI = 0;
  struct cfifo_ex* video_fifo = NULL;
  struct cfifo_ex* audio_fifo = NULL;
  int ep = cfifo_ep_alloc(1);
  unsigned char buf[sizeof(gsf_frm_t)+GSF_FRM_MAX_SIZE];
 
  GSF_MSG_DEF(gsf_sdp_t, sdp, sizeof(gsf_msg_t)+sizeof(gsf_sdp_t));
  do
  {
    //wait codec.exe;
    sdp->video_shmid = sdp->audio_shmid = -1;
    ret = GSF_MSG_SENDTO(GSF_ID_CODEC_SDP, 0, GET, 0
                          , sizeof(gsf_sdp_t)
                          , GSF_IPC_CODEC
                          , 2000);
  }while(running && ret < 0);
  
  if(ret == 0)
  {
    video_fifo = cfifo_shmat(cfifo_recsize, cfifo_rectag, sdp->video_shmid);
    //audio_fifo = cfifo_shmat(cfifo_recsize, cfifo_rectag, sdp->audio_shmid);
    
    cfifo_ep_ctl(ep, CFIFO_EP_ADD, video_fifo);
    unsigned int video_utc = cfifo_newest(video_fifo, 0);
     
    if(audio_fifo)
    {
      cfifo_ep_ctl(ep, CFIFO_EP_ADD, audio_fifo);
      unsigned int audio_utc = cfifo_oldest(audio_fifo, video_utc);
      printf("video_utc:%u, audio_utc:%u\n", video_utc, audio_utc);
    }
      
    GSF_MSG_DEF(char, msgdata, sizeof(gsf_msg_t));
    GSF_MSG_SENDTO(GSF_ID_CODEC_IDR, 0, SET, 0
                    , 0
                    , GSF_IPC_CODEC
                    , 2000);
  }
  while(running)
  {
    int cnt = 0;
    struct cfifo_ex* result[255];
    cnt = cfifo_ep_wait(ep, 2000, result, 255);
    if(cnt <= 0)
    {
       printf("cfifo_ep_wait err cnt:%d\n", cnt);
    }
    
    for(i = 0; i < cnt; i++)
    {
      struct cfifo_ex* fifo = result[i];
      
      while(ret >= 0)
      {
          ret = cfifo_get(fifo, cfifo_recgut, (unsigned char*)buf);
          if(ret < 0)
          {
              printf("cfifo err ret:%d\n", ret);
          }
          else if (ret == 0)
          {
              //printf("cfifo empty \n");
              break;
          }
          else
          {
              //cfifo_info(fifo);
          }
          
          // TODO;
          
          gsf_frm_t *rec = (gsf_frm_t *)buf;

          waitI = (waitI)?:(rec->flag&GSF_FRM_FLAG_IDR);
          if(!waitI)
            continue;
          
          uvc_cache_t *uvc_cache = uvc_cache_get();
          frame_node_t *fnode = HI_NULL;

          if (uvc_cache)
          {
            get_node_from_queue(uvc_cache->free_queue, &fnode);
          }

          if (fnode)
          {
            fnode->used = 0;
            if(rec->size < fnode->length)
            {
              memcpy(fnode->mem + fnode->used, rec->data, rec->size);
              fnode->used += rec->size;
            }
            //printf("put_node_to_queue fnode[length:%d, used:%d]\n", fnode->length, fnode->used);
            put_node_to_queue(uvc_cache->ok_queue, fnode);
          }
          else 
          {
            //printf("get_node_from_queue fnode: %p\n", fnode);
          }
      }
    }
  }
  
  if(ep)
    cfifo_ep_free(ep);
  
  if(video_fifo)
    cfifo_free(video_fifo);
    
  if(audio_fifo)
    cfifo_free(audio_fifo);
    
  printf("exit.\n");
  return NULL;
}


/*******************************************************
 * Stream Control Operation Functions
 *******************************************************/
static HI_S32 sample_venc_set_idr(HI_VOID)
{
   //IDR;
   return HI_SUCCESS;
}

static HI_S32 sample_venc_init(HI_VOID)
{
    //SYS_INIT;
    return HI_SUCCESS;
}

static HI_S32 sample_venc_startup(HI_VOID)
{
    //START;
    if(gpid)
      return -1;
    
    running = 1;
    pthread_create(&gpid, NULL, send_task, (void*)NULL);
    return HI_SUCCESS;
}

static HI_S32 sample_venc_shutdown(HI_VOID)
{
    //STOP;
    running = 0;
    if(gpid)
      pthread_join(gpid, NULL);
    gpid = 0;
    
    return HI_SUCCESS;
}

static HI_S32 sample_venc_set_property(encoder_property *p)
{
    return HI_SUCCESS;
}


static struct stream_control_ops venc_sc_ops = {
    .init = sample_venc_init,
    .startup = sample_venc_startup,
    .shutdown = sample_venc_shutdown,
    .set_idr = sample_venc_set_idr,
    .set_property = sample_venc_set_property,
};

#endif


HI_VOID sample_venc_config(HI_VOID)
{
    histream_register_mpi_ops(&venc_sc_ops, &venc_pu_ops, &venc_it_ops, HI_NULL);
}
