/******************************************************************************
  Some simple Hisilicon Hi35xx isp functions.

  Copyright (C), 2010-2018, Hisilicon Tech. Co., Ltd.
 ******************************************************************************
    Modification:  2018-2 Created
******************************************************************************/

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/poll.h>
#include <sys/time.h>
#include <fcntl.h>
#include <errno.h>
#include <pthread.h>
#include <math.h>
#include <unistd.h>
#include <signal.h>
#include "sample_comm.h"

#if 0 //maohw
#define MAX_SENSOR_NUM    2
#endif

/*******************For ISP**************************************/
static pthread_t g_IspPid[ISP_MAX_DEV_NUM] = {0};
static HI_BOOL g_bIspInit[ISP_MAX_DEV_NUM] = {HI_FALSE};
static HI_U32 g_au32IspSnsId[ISP_MAX_DEV_NUM] = {0, 1};

//static pthread_t g_DisPid;


#if 0 //maohw
const ISP_SNS_OBJ_S *g_pstSnsObj[MAX_SENSOR_NUM] =
{
#if defined(SNS_MN34220_MIPI_DOUBLE)
    &stSnsMn34220MipiObj, &stSnsMn34220MipiObj,
#elif defined(SNS_IMX290_MIPI_DOUBLE)
    &stSnsImx290Obj, &stSnsImx290Obj
#elif defined(SNS_IMX290_MIPI_SINGLE)
    &stSnsImx290Obj, HI_NULL
#elif defined(SNS_MN34220_SINGLE)
    &stSnsMn34220Obj, HI_NULL
#elif defined(SNS_MN34220_MIPI)
    &stSnsMn34220MipiObj, HI_NULL
#elif defined(SNS_OV4689_1080P_SINGLE)||defined(SNS_OV4689_4M_SINGLE)
    &stSnsOv4689SlaveObj, HI_NULL
#elif defined(SNS_OV4689_1080P_DOUBLE)||defined(SNS_OV4689_4M_DOUBLE)
    &stSnsOv4689SlaveObj, &stSnsOv4689SlaveObj
#elif defined(SNS_IMX226_8M_SINGLE)|| defined(SNS_IMX226_9M_SINGLE) || defined(SNS_IMX226_12M_SINGLE)
    &stSnsImx226Obj, HI_NULL
#elif defined(SNS_IMX117_8M_SINGLE) || defined(SNS_IMX117_1080P_SINGLE) || defined(SNS_IMX117_720P_SINGLE) || defined(SNS_IMX117_12M_SINGLE)
    &stSnsImx117Obj, HI_NULL
#elif defined(SNS_IMX274_8M_SINGLE)
    &stSnsImx274Obj, HI_NULL
#elif defined(SNS_MN34120_16M_SINGLE) ||defined(SNS_MN34120_8M_SINGLE) || defined(SNS_MN34120_1080P_SINGLE)
    &stSnsMn34120Obj, HI_NULL
#elif defined(SNS_IMX377_8M30_10BIT_SINGLE) || defined(SNS_IMX377_8M60_10BIT_SINGLE)  || defined(SNS_IMX377_1080P120_12BIT_SINGLE)  || defined(SNS_IMX377_720P240_10BIT_SINGLE)|| defined(SNS_IMX377_12M30_12BIT_SINGLE) || defined(SNS_IMX377_8M30_12BIT_SINGLE)
    &stSnsImx377Obj, HI_NULL
#elif defined(SNS_IMX326_5M_SINGLE)
    &stSnsImx326Obj,HI_NULL
#elif defined(SNS_OS05A_5M_SINGLE)
    &stSnsOs05aObj,HI_NULL
#elif defined(SNS_OS08A_8M_SINGLE)
    &stSnsOs08a10Obj, HI_NULL
#elif defined(SNS_IMX334_4K_SINGLE)
    &stSnsImx334Obj, HI_NULL
#endif
};
#endif

static void* Test_ISP_Run(void* param)
{
    ISP_DEV IspDev;
    IspDev = (ISP_DEV)param;

    /* Run loop, exit until HI_MPI_ISP_Exit() been called! */
    printf("ISP Dev %d HI_MPI_ISP_Run\n", IspDev);
    HI_MPI_ISP_Run(IspDev);

	return NULL;
}

/******************************************************************************
* funciton : ISP init
******************************************************************************/

HI_S32 SAMPLE_COMM_ISP_Aelib_Callback(ISP_DEV IspDev)
{
    ALG_LIB_S stAeLib;

    stAeLib.s32Id = IspDev;
    strncpy(stAeLib.acLibName, HI_AE_LIB_NAME, sizeof(HI_AE_LIB_NAME));
    CHECK_RET(HI_MPI_AE_Register(IspDev, &stAeLib),"aelib register call back");
    return HI_SUCCESS;
}

HI_S32 SAMPLE_COMM_ISP_Aelib_UnCallback(ISP_DEV IspDev)
{
    ALG_LIB_S stAeLib;

    stAeLib.s32Id = IspDev;
    strncpy(stAeLib.acLibName, HI_AE_LIB_NAME, sizeof(HI_AE_LIB_NAME));
    CHECK_RET(HI_MPI_AE_UnRegister(IspDev, &stAeLib),"aelib unregister call back");
    return HI_SUCCESS;
}

HI_S32 SAMPLE_COMM_ISP_Awblib_Callback(ISP_DEV IspDev)
{
    ALG_LIB_S stAwbLib;

    stAwbLib.s32Id = IspDev;
    strncpy(stAwbLib.acLibName, HI_AWB_LIB_NAME, sizeof(HI_AWB_LIB_NAME));
    CHECK_RET(HI_MPI_AWB_Register(IspDev, &stAwbLib),"awblib register call back");
    return HI_SUCCESS;
}

HI_S32 SAMPLE_COMM_ISP_Awblib_UnCallback(ISP_DEV IspDev)
{
    ALG_LIB_S stAwbLib;

    stAwbLib.s32Id = IspDev;
    strncpy(stAwbLib.acLibName, HI_AWB_LIB_NAME, sizeof(HI_AWB_LIB_NAME));
    CHECK_RET(HI_MPI_AWB_UnRegister(IspDev, &stAwbLib),"awblib unregister call back");
    return HI_SUCCESS;
}

HI_S32 SAMPLE_COMM_ISP_Aflib_Callback(ISP_DEV IspDev)
{
    ALG_LIB_S stAfLib;

    stAfLib.s32Id = IspDev;
    strncpy(stAfLib.acLibName, HI_AF_LIB_NAME, sizeof(HI_AF_LIB_NAME));
    CHECK_RET(HI_MPI_AF_Register(IspDev, &stAfLib),"aflib register call back");
    return HI_SUCCESS;
}

HI_S32 SAMPLE_COMM_ISP_Aflib_UnCallback(ISP_DEV IspDev)
{
    ALG_LIB_S stAfLib;

    stAfLib.s32Id = IspDev;
    strncpy(stAfLib.acLibName, HI_AF_LIB_NAME, sizeof(HI_AF_LIB_NAME));
    CHECK_RET(HI_MPI_AF_UnRegister(IspDev, &stAfLib),"aflib unregister call back");
    return HI_SUCCESS;
}


/******************************************************************************
* funciton : ISP Run
******************************************************************************/
HI_S32 SAMPLE_COMM_ISP_Run(ISP_DEV IspDev)
{
    HI_S32 s32Ret = 0;
    pthread_attr_t *pstAttr = NULL;

#if (HICHIP == HI3518E_V200)
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setstacksize(&attr, 4096 * 1024);
    pstAttr = &attr;
#endif

    s32Ret = pthread_create(&g_IspPid[IspDev], pstAttr, Test_ISP_Run, (HI_VOID *)IspDev);
    if (0 != s32Ret)
    {
        printf("%s: create isp running thread failed!, error: %d, %s\r\n", __FUNCTION__, s32Ret, strerror(s32Ret));
        goto out;
    }

out:
    if (NULL != pstAttr)
    {
        pthread_attr_destroy(pstAttr);
    }
    return s32Ret;
}



/*
 * Description:
 *     register sensor's callback to ISP
 */
HI_S32 SAMPLE_COMM_ISP_Sensor_Regiter_callback(ISP_DEV IspDev, HI_U32 u32SnsId)
{
    ALG_LIB_S stAeLib;
    ALG_LIB_S stAwbLib;
    ALG_LIB_S stAfLib;
    const ISP_SNS_OBJ_S *pstSnsObj;
    HI_S32    s32Ret = -1;

    if (MAX_SENSOR_NUM <= u32SnsId)
    {
        printf("%s: invalid sensor id: %d\n", __FUNCTION__, u32SnsId);
        return HI_FAILURE;
    }
    pstSnsObj = g_pstSnsObj[u32SnsId];
    if (HI_NULL == pstSnsObj)
    {
        printf("%s: sensor %d not exist!\n", __FUNCTION__, u32SnsId);
        return HI_FAILURE;
    }

    stAeLib.s32Id = IspDev;
    stAwbLib.s32Id = IspDev;
    stAfLib.s32Id = IspDev;
    strncpy(stAeLib.acLibName, HI_AE_LIB_NAME, sizeof(HI_AE_LIB_NAME));
    strncpy(stAwbLib.acLibName, HI_AWB_LIB_NAME, sizeof(HI_AWB_LIB_NAME));
    strncpy(stAfLib.acLibName, HI_AF_LIB_NAME, sizeof(HI_AF_LIB_NAME));
    if (pstSnsObj->pfnRegisterCallback != HI_NULL)
    {
        s32Ret = pstSnsObj->pfnRegisterCallback(IspDev, &stAeLib, &stAwbLib);
        if (s32Ret != HI_SUCCESS)
        {
            printf("%s: sensor_register_callback failed with %#x!\n", __FUNCTION__, s32Ret);
            return s32Ret;
        }
    }
    else
    {
        printf("%s: sensor_register_callback failed with HI_NULL!\n",  __FUNCTION__);
    }
    g_au32IspSnsId[IspDev] = u32SnsId;
	printf("%s isp_dev = %d\n", __FUNCTION__, IspDev);

    return HI_SUCCESS;
}

static HI_S32 GetIspPubAttrBySns(ISP_PUB_ATTR_S *pstPubAttr)
{
    ISP_PUB_ATTR_S stPubAttr;

    switch (SENSOR_TYPE)
    {
    case APTINA_9M034_DC_720P_30FPS:
    case APTINA_AR0130_DC_720P_30FPS:
        stPubAttr.enBayer               = BAYER_GRBG;
        stPubAttr.f32FrameRate          = 30;
        stPubAttr.stWndRect.s32X        = 0;
        stPubAttr.stWndRect.s32Y        = 0;
        stPubAttr.stWndRect.u32Width    = 1280;
        stPubAttr.stWndRect.u32Height   = 720;
        stPubAttr.stSnsSize.u32Width    = 1280;
        stPubAttr.stSnsSize.u32Height   = 720;
        break;

    case SONY_IMX122_DC_1080P_30FPS:
        stPubAttr.enBayer               = BAYER_RGGB;
        stPubAttr.f32FrameRate          = 30;
        stPubAttr.stWndRect.s32X        = 200;
        stPubAttr.stWndRect.s32Y        = 12;
        stPubAttr.stWndRect.u32Width    = 1920;
        stPubAttr.stWndRect.u32Height   = 1080;
        stPubAttr.stSnsSize.u32Width    = 1920;
        stPubAttr.stSnsSize.u32Height   = 1080;
        break;

    case PANASONIC_MN34220_SUBLVDS_1080P_30FPS:
    case PANASONIC_MN34220_MIPI_1080P_30FPS:
    case APTINA_AR0330_MIPI_1080P_30FPS:
        stPubAttr.enBayer               = BAYER_GRBG;
        stPubAttr.f32FrameRate          = 30;
        stPubAttr.stWndRect.s32X        = 0;
        stPubAttr.stWndRect.s32Y        = 0;
        stPubAttr.stWndRect.u32Width    = 1920;
        stPubAttr.stWndRect.u32Height   = 1080;
        stPubAttr.stSnsSize.u32Width    = 1920;
        stPubAttr.stSnsSize.u32Height   = 1080;
        break;

    case PANASONIC_MN34220_SUBLVDS_720P_120FPS:
    case PANASONIC_MN34220_MIPI_720P_120FPS:
        stPubAttr.enBayer               = BAYER_GRBG;
        stPubAttr.f32FrameRate          = 120;
        stPubAttr.stWndRect.s32X        = 0;
        stPubAttr.stWndRect.s32Y        = 0;
        stPubAttr.stWndRect.u32Width    = 1280;
        stPubAttr.stWndRect.u32Height   = 720;
        stPubAttr.stSnsSize.u32Width    = 1280;
        stPubAttr.stSnsSize.u32Height   = 720;
        break;

    case SONY_IMX178_LVDS_1080P_30FPS:
        stPubAttr.enBayer               = BAYER_GBRG;
        stPubAttr.f32FrameRate          = 30;
        stPubAttr.stWndRect.s32X        = 0;
        stPubAttr.stWndRect.s32Y        = 0;
        stPubAttr.stWndRect.u32Width    = 1920;
        stPubAttr.stWndRect.u32Height   = 1080;
        stPubAttr.stSnsSize.u32Width    = 1920;
        stPubAttr.stSnsSize.u32Height   = 1080;
        break;

    case SONY_IMX178_LVDS_5M_30FPS:
        stPubAttr.enBayer               = BAYER_GBRG;
        stPubAttr.f32FrameRate          = 30;
        stPubAttr.stWndRect.s32X        = 0;
        stPubAttr.stWndRect.s32Y        = 0;
        stPubAttr.stWndRect.u32Width    = 2592;
        stPubAttr.stWndRect.u32Height   = 1944;
        stPubAttr.stSnsSize.u32Width    = 2592;
        stPubAttr.stSnsSize.u32Height   = 1944;
        break;

    case SONY_IMX185_MIPI_1080P_30FPS:
    case SONY_IMX290_MIPI_1080P_30FPS:
        stPubAttr.enBayer               = BAYER_RGGB;
        stPubAttr.f32FrameRate          = 30;
        stPubAttr.stWndRect.s32X        = 0;
        stPubAttr.stWndRect.s32Y        = 0;
        stPubAttr.stWndRect.u32Width    = 1920;
        stPubAttr.stWndRect.u32Height   = 1080;
        stPubAttr.stSnsSize.u32Width    = 1920;
        stPubAttr.stSnsSize.u32Height   = 1080;
        break;

    case APTINA_MT9P006_DC_1080P_30FPS:
        stPubAttr.enBayer               = BAYER_GRBG;
        stPubAttr.f32FrameRate          = 30;
        stPubAttr.stWndRect.s32X        = 0;
        stPubAttr.stWndRect.s32Y        = 2;
        stPubAttr.stWndRect.u32Width    = 1920;
        stPubAttr.stWndRect.u32Height   = 1080;
        stPubAttr.stSnsSize.u32Width    = 1920;
        stPubAttr.stSnsSize.u32Height   = 1080;
        break;

    case APTINA_AR0330_MIPI_1296P_25FPS:
        stPubAttr.enBayer               = BAYER_GRBG;
        stPubAttr.f32FrameRate          = 25;
        stPubAttr.stWndRect.s32X        = 0;
        stPubAttr.stWndRect.s32Y        = 0;
        stPubAttr.stWndRect.u32Width    = 2304;
        stPubAttr.stWndRect.u32Height   = 1296;
        stPubAttr.stSnsSize.u32Width    = 2304;
        stPubAttr.stSnsSize.u32Height   = 1296;
        break;

    case APTINA_AR0330_MIPI_1536P_25FPS:
        stPubAttr.enBayer               = BAYER_GRBG;
        stPubAttr.f32FrameRate          = 25;
        stPubAttr.stWndRect.s32X        = 0;
        stPubAttr.stWndRect.s32Y        = 0;
        stPubAttr.stWndRect.u32Width    = 2048;
        stPubAttr.stWndRect.u32Height   = 1536;
        stPubAttr.stSnsSize.u32Width    = 2048;
        stPubAttr.stSnsSize.u32Height   = 1536;
        break;

    case OMNIVISION_OV4689_MIPI_4M_30FPS:
        stPubAttr.enBayer               = BAYER_BGGR;
        stPubAttr.f32FrameRate          = 30;
        stPubAttr.stWndRect.s32X        = 0;
        stPubAttr.stWndRect.s32Y        = 0;
        stPubAttr.stWndRect.u32Width    = 2592;
        stPubAttr.stWndRect.u32Height   = 1520;
        stPubAttr.stSnsSize.u32Width    = 2592;
        stPubAttr.stSnsSize.u32Height   = 1520;
        break;

    case OMNIVISION_OV4689_MIPI_1080P_30FPS:
        stPubAttr.enBayer               = BAYER_BGGR;
        stPubAttr.f32FrameRate          = 30;
        stPubAttr.stWndRect.s32X        = 0;
        stPubAttr.stWndRect.s32Y        = 0;
        stPubAttr.stWndRect.u32Width    = 1920;
        stPubAttr.stWndRect.u32Height   = 1080;
        stPubAttr.stSnsSize.u32Width    = 1920;
        stPubAttr.stSnsSize.u32Height   = 1080;
        break;

    case OMNIVISION_OV5658_MIPI_5M_30FPS:
        stPubAttr.enBayer               = BAYER_BGGR;
        stPubAttr.f32FrameRate          = 30;
        stPubAttr.stWndRect.s32X        = 0;
        stPubAttr.stWndRect.s32Y        = 0;
        stPubAttr.stWndRect.u32Width    = 2592;
        stPubAttr.stWndRect.u32Height   = 1944;
        stPubAttr.stSnsSize.u32Width    = 2592;
        stPubAttr.stSnsSize.u32Height   = 1944;
        break;

    case SONY_IMX117_LVDS_1080P_120FPS:
        stPubAttr.enBayer				= BAYER_RGGB;
        stPubAttr.f32FrameRate          = 120;
        stPubAttr.stWndRect.s32X        = 0;
        stPubAttr.stWndRect.s32Y        = 0;
        stPubAttr.stWndRect.u32Width    = 1920;
        stPubAttr.stWndRect.u32Height   = 1080;
        stPubAttr.stSnsSize.u32Width    = 1920;
        stPubAttr.stSnsSize.u32Height   = 1080;
        break;

    case SONY_IMX117_LVDS_720P_240FPS:
        stPubAttr.enBayer				= BAYER_RGGB;
        stPubAttr.f32FrameRate          = 240;
        stPubAttr.stWndRect.s32X        = 0;
        stPubAttr.stWndRect.s32Y        = 0;
        stPubAttr.stWndRect.u32Width    = 1280;
        stPubAttr.stWndRect.u32Height   = 720;
        stPubAttr.stSnsSize.u32Width    = 1280;
        stPubAttr.stSnsSize.u32Height   = 720;
        break;

    case SONY_IMX117_LVDS_4K_30FPS:
        stPubAttr.enBayer               = BAYER_RGGB;
        stPubAttr.f32FrameRate          = 30;
        stPubAttr.stWndRect.s32X        = 0;
        stPubAttr.stWndRect.s32Y        = 0;
        stPubAttr.stWndRect.u32Width    = 3840;
        stPubAttr.stWndRect.u32Height   = 2160;
        stPubAttr.stSnsSize.u32Width    = 4096;
        stPubAttr.stSnsSize.u32Height   = 2160;
        break;
    case SONY_IMX117_LVDS_12M_28FPS:
        stPubAttr.enBayer               = BAYER_RGGB;
        stPubAttr.f32FrameRate          = 28;
        stPubAttr.stWndRect.s32X        = 0;
        stPubAttr.stWndRect.s32Y        = 0;
        stPubAttr.stWndRect.u32Width    = 4000;
        stPubAttr.stWndRect.u32Height   = 3000;
        stPubAttr.stSnsSize.u32Width    = 4000;
        stPubAttr.stSnsSize.u32Height   = 3000;
        break;

#if 0
    case SONY_IMX377_MIPI_8M_30FPS:
        stPubAttr.enBayer				= BAYER_RGGB;
        stPubAttr.f32FrameRate          = 30;
        stPubAttr.stWndRect.s32X        = 0;
        stPubAttr.stWndRect.s32Y        = 0;
        stPubAttr.stWndRect.u32Width    = 3840;
        stPubAttr.stWndRect.u32Height   = 2160;
        stPubAttr.stSnsSize.u32Width    = 4096;
        stPubAttr.stSnsSize.u32Height   = 2160;
        break;
#endif

    case SONY_IMX377_MIPI_8M_30FPS_10BIT:
        stPubAttr.enBayer				= BAYER_RGGB;
        stPubAttr.f32FrameRate          = 30;
        stPubAttr.stWndRect.s32X        = 0;
        stPubAttr.stWndRect.s32Y        = 0;
        stPubAttr.stWndRect.u32Width    = 3840;
        stPubAttr.stWndRect.u32Height   = 2160;
        stPubAttr.stSnsSize.u32Width    = 3850;
        stPubAttr.stSnsSize.u32Height   = 2160;
        break;

    case SONY_IMX377_MIPI_8M_60FPS_10BIT:
        stPubAttr.enBayer				= BAYER_RGGB;
        stPubAttr.f32FrameRate          = 60;
        stPubAttr.stWndRect.s32X        = 0;
        stPubAttr.stWndRect.s32Y        = 0;
        stPubAttr.stWndRect.u32Width    = 3840;
        stPubAttr.stWndRect.u32Height   = 2160;
        stPubAttr.stSnsSize.u32Width    = 3840;
        stPubAttr.stSnsSize.u32Height   = 2160;
        break;

    case SONY_IMX377_MIPI_1080P_120FPS_12BIT:
        stPubAttr.enBayer               = BAYER_RGGB;
        stPubAttr.f32FrameRate          = 120;
        stPubAttr.stWndRect.s32X        = 0;
        stPubAttr.stWndRect.s32Y        = 0;
        stPubAttr.stWndRect.u32Width    = 1920;
        stPubAttr.stWndRect.u32Height   = 1080;
        stPubAttr.stSnsSize.u32Width    = 2048;
        stPubAttr.stSnsSize.u32Height   = 1080;
        break;

     case SONY_IMX377_MIPI_720P_240FPS_10BIT:
        stPubAttr.enBayer				= BAYER_RGGB;
        stPubAttr.f32FrameRate          = 240;
        stPubAttr.stWndRect.s32X        = 0;
        stPubAttr.stWndRect.s32Y        = 0;
        stPubAttr.stWndRect.u32Width    = 1280;
        stPubAttr.stWndRect.u32Height   = 720;
        stPubAttr.stSnsSize.u32Width    = 1288;
        stPubAttr.stSnsSize.u32Height   = 720;
        break;

    case SONY_IMX377_MIPI_12M_30FPS_12BIT:
        stPubAttr.enBayer               = BAYER_RGGB;
        stPubAttr.f32FrameRate          = 30;
        stPubAttr.stWndRect.s32X        = 0;
        stPubAttr.stWndRect.s32Y        = 0;
        stPubAttr.stWndRect.u32Width    = 4000;
        stPubAttr.stWndRect.u32Height   = 3000;
        stPubAttr.stSnsSize.u32Width    = 4000;
        stPubAttr.stSnsSize.u32Height   = 3000;
        break;

    case SONY_IMX377_MIPI_8M_30FPS_12BIT:
        stPubAttr.enBayer				= BAYER_RGGB;
        stPubAttr.f32FrameRate          = 30;
        stPubAttr.stWndRect.s32X        = 0;
        stPubAttr.stWndRect.s32Y        = 0;
        stPubAttr.stWndRect.u32Width    = 3840;
        stPubAttr.stWndRect.u32Height   = 2160;
        stPubAttr.stSnsSize.u32Width    = 4096;
        stPubAttr.stSnsSize.u32Height   = 2160;
        break;

    case SONY_IMX226_LVDS_4K_30FPS:
        stPubAttr.enBayer				= BAYER_RGGB;
        stPubAttr.f32FrameRate          = 30;
        stPubAttr.stWndRect.s32X        = 0;
        stPubAttr.stWndRect.s32Y        = 0;
        stPubAttr.stWndRect.u32Width    = 3840;
        stPubAttr.stWndRect.u32Height   = 2160;
        stPubAttr.stSnsSize.u32Width    = 3840;
        stPubAttr.stSnsSize.u32Height   = 2160;
        break;
    case SONY_IMX226_LVDS_4K_60FPS:
        stPubAttr.enBayer				= BAYER_RGGB;
        stPubAttr.f32FrameRate          = 60;
        stPubAttr.stWndRect.s32X        = 0;
        stPubAttr.stWndRect.s32Y        = 0;
        stPubAttr.stWndRect.u32Width    = 3840;
        stPubAttr.stWndRect.u32Height   = 2160;
        stPubAttr.stSnsSize.u32Width    = 4096;
        stPubAttr.stSnsSize.u32Height   = 2160;
        break;
    case SONY_IMX226_LVDS_9M_30FPS:
        stPubAttr.enBayer				= BAYER_RGGB;
        stPubAttr.f32FrameRate          = 30;
        stPubAttr.stWndRect.s32X        = 0;
        stPubAttr.stWndRect.s32Y        = 0;
        stPubAttr.stWndRect.u32Width    = 3000;
        stPubAttr.stWndRect.u32Height   = 3000;
        stPubAttr.stSnsSize.u32Width    = 3000;
        stPubAttr.stSnsSize.u32Height   = 3000;
        break;
	 case SONY_IMX226_LVDS_12M_30FPS:
        stPubAttr.enBayer				= BAYER_RGGB;
        stPubAttr.f32FrameRate          = 30;
        stPubAttr.stWndRect.s32X        = 0;
        stPubAttr.stWndRect.s32Y        = 0;
        stPubAttr.stWndRect.u32Width    = 4000;
        stPubAttr.stWndRect.u32Height   = 3000;
        stPubAttr.stSnsSize.u32Width    = 4000;
        stPubAttr.stSnsSize.u32Height   = 3000;
        break;

    case SONY_IMX274_LVDS_4K_30FPS:
        stPubAttr.enBayer				= BAYER_RGGB;
        stPubAttr.f32FrameRate          = 30;
        stPubAttr.stWndRect.s32X        = 0;
        stPubAttr.stWndRect.s32Y        = 0;
        stPubAttr.stWndRect.u32Width    = 3840;
        stPubAttr.stWndRect.u32Height   = 2160;
        stPubAttr.stSnsSize.u32Width    = 3840;
        stPubAttr.stSnsSize.u32Height   = 2160;
        break;

	case SONY_IMX334_MIPI_4K_30FPS:
        stPubAttr.enBayer				= BAYER_RGGB;
        stPubAttr.f32FrameRate          = 30;
        stPubAttr.stWndRect.s32X        = 0;
        stPubAttr.stWndRect.s32Y        = 0;
        stPubAttr.stWndRect.u32Width    = 3840;
        stPubAttr.stWndRect.u32Height   = 2160;
        stPubAttr.stSnsSize.u32Width    = 3840;
        stPubAttr.stSnsSize.u32Height   = 2160;
        break;  
        
    case PANASONIC_MN34120_LVDS_16M_16P25FPS:
        stPubAttr.enBayer               = BAYER_GRBG;
        stPubAttr.f32FrameRate          = 16.25;
        stPubAttr.stWndRect.s32X        = 0;
        stPubAttr.stWndRect.s32Y        = 0;
        stPubAttr.stWndRect.u32Width    = 4608;
        stPubAttr.stWndRect.u32Height   = 3456;
        stPubAttr.stSnsSize.u32Width    = 4608;
        stPubAttr.stSnsSize.u32Height   = 3456;
        break;

    case PANASONIC_MN34120_LVDS_4K_30FPS:
        stPubAttr.enBayer               = BAYER_GRBG;
        stPubAttr.f32FrameRate          = 30;
        stPubAttr.stWndRect.s32X        = 0;
        stPubAttr.stWndRect.s32Y        = 0;
        stPubAttr.stWndRect.u32Width    = 3840;
        stPubAttr.stWndRect.u32Height   = 2160;
        stPubAttr.stSnsSize.u32Width    = 3840;
        stPubAttr.stSnsSize.u32Height   = 2160;
        break;

    case PANASONIC_MN34120_LVDS_1080P_60FPS:
        stPubAttr.enBayer               = BAYER_GRBG;
        stPubAttr.f32FrameRate          = 60;
        stPubAttr.stWndRect.s32X        = 0;
        stPubAttr.stWndRect.s32Y        = 0;
        stPubAttr.stWndRect.u32Width    = 1920;
        stPubAttr.stWndRect.u32Height   = 1080;
        stPubAttr.stSnsSize.u32Width    = 1920;
        stPubAttr.stSnsSize.u32Height   = 1080;
        break;

    case APTINA_AR0230_HISPI_1080P_30FPS:
        stPubAttr.enBayer               = BAYER_GRBG;
        stPubAttr.f32FrameRate          = 30;
        stPubAttr.stWndRect.s32X        = 0;
        stPubAttr.stWndRect.s32Y        = 0;
        stPubAttr.stWndRect.u32Width    = 1920;
        stPubAttr.stWndRect.u32Height   = 1080;
        stPubAttr.stSnsSize.u32Width    = 1920;
        stPubAttr.stSnsSize.u32Height   = 1080;
        break;
	case SONY_IMX326_MIPI_5M_30FPS:
		stPubAttr.enBayer				= BAYER_RGGB;
		stPubAttr.f32FrameRate			= 30;
		stPubAttr.stWndRect.s32X		= 0;
		stPubAttr.stWndRect.s32Y		= 0;
		stPubAttr.stWndRect.u32Width	= 2592;
		stPubAttr.stWndRect.u32Height	= 1944;
		stPubAttr.stSnsSize.u32Width	= 2592;
		stPubAttr.stSnsSize.u32Height	= 1944;
		break;

    case OMNIVISION_OS05A_MIPI_5M_30FPS:
        stPubAttr.enBayer				= BAYER_BGGR;
		stPubAttr.f32FrameRate			= 30;
		stPubAttr.stWndRect.s32X		= 0;
		stPubAttr.stWndRect.s32Y		= 0;
		stPubAttr.stWndRect.u32Width	= 2688;
		stPubAttr.stWndRect.u32Height	= 1944;
		stPubAttr.stSnsSize.u32Width	= 2688;
		stPubAttr.stSnsSize.u32Height	= 1944;
		break;

    case OMNIVISION_OS08A_MIPI_4K_30FPS:
        stPubAttr.enBayer				= BAYER_BGGR;
		stPubAttr.f32FrameRate			= 30;
		stPubAttr.stWndRect.s32X		= 0;
		stPubAttr.stWndRect.s32Y		= 0;
		stPubAttr.stWndRect.u32Width	= 3840;
		stPubAttr.stWndRect.u32Height	= 2160;
		stPubAttr.stSnsSize.u32Width	= 3840;
		stPubAttr.stSnsSize.u32Height	= 2160;
		break;

    default:
        stPubAttr.enBayer               = BAYER_GRBG;
        stPubAttr.f32FrameRate          = 30;
        stPubAttr.stWndRect.s32X        = 0;
        stPubAttr.stWndRect.s32Y        = 0;
        stPubAttr.stWndRect.u32Width    = 1920;
        stPubAttr.stWndRect.u32Height   = 1080;
        stPubAttr.stSnsSize.u32Width    = 1920;
        stPubAttr.stSnsSize.u32Height   = 1080;
        break;
    }

    *pstPubAttr = stPubAttr;

    return HI_SUCCESS;
}

HI_S32 SAMPLE_COMM_ISP_Init(ISP_DEV IspDev, WDR_MODE_E  enWDRMode, SAMPLE_FRAMERATE_E enFrmRate, SAMPLE_SENSOR_NUM_E enSnsNum)
{
    //HI_S32 s32Ret;
    ISP_PUB_ATTR_S stPubAttr, *pstPubAttr;
    //ALG_LIB_S stLib;
	ISP_WDR_MODE_S stWdrMode;
    ISP_MODE_E enIspMode;

	stWdrMode.enWDRMode = enWDRMode;
	pstPubAttr = &stPubAttr;

    CHECK_RET(SAMPLE_COMM_ISP_Aelib_Callback(IspDev),"aelib call back");
    CHECK_RET(SAMPLE_COMM_ISP_Awblib_Callback(IspDev),"awblib call back");
    CHECK_RET(SAMPLE_COMM_ISP_Aflib_Callback(IspDev),"aflib call back");
    if (HI_SUCCESS != HI_MPI_ISP_MemInit(IspDev))
    {
        printf("Func: %s, Line: %d, Init Ext memory failed\n", __FUNCTION__, __LINE__);
        goto fail1;
    }

    if (SAMPLE_SENSOR_DOUBLE == enSnsNum)
    {
        enIspMode = ISP_MODE_STITCH;
        if (HI_SUCCESS != HI_MPI_ISP_SetMode(IspDev, enIspMode))
        {
            printf("Func: %s, Line: %d, Set ISP Mode failed\n", __FUNCTION__, __LINE__);
            goto fail1;
        }
    }

    if (HI_SUCCESS != HI_MPI_ISP_SetWDRMode(IspDev, &stWdrMode))
    {
        printf("Func: %s, Line: %d, Set WDR Mode failed\n", __FUNCTION__, __LINE__);
        goto fail1;
    }
    printf("Func: %s, Line: %d, WDR Mode: %d\n", __FUNCTION__, __LINE__, stWdrMode.enWDRMode);

    GetIspPubAttrBySns(&stPubAttr);

    if (SAMPLE_FRAMERATE_DEFAULT != enFrmRate)
    {
        stPubAttr.f32FrameRate = enFrmRate;
    }

    if (HI_SUCCESS != HI_MPI_ISP_SetPubAttr(IspDev, &stPubAttr))
    {
        printf("Func: %s, Line: %d, SetPubAttr failed\n", __FUNCTION__, __LINE__);
        goto fail1;
    }
    printf("stPubAttr (%d, %d, %d, %d, %f)\n",
            pstPubAttr->stWndRect.s32X, pstPubAttr->stWndRect.s32Y,
            pstPubAttr->stWndRect.u32Width, pstPubAttr->stWndRect.u32Height,
            pstPubAttr->f32FrameRate);

    if (HI_SUCCESS != HI_MPI_ISP_Init(IspDev))
    {
        printf("Func: %s, Line: %d, ISP Init failed\n", __FUNCTION__, __LINE__);
        goto fail1;
    }

    printf("Func: %s, Line: %d end IspDev:%d.\n", __FUNCTION__, __LINE__, IspDev);
    g_bIspInit[IspDev] = HI_TRUE;

    return HI_SUCCESS;

fail1:
    HI_MPI_ISP_Exit(IspDev);
    return HI_FAILURE;
}

/******************************************************************************
* funciton : change between linear and wdr mode
******************************************************************************/
HI_S32 SAMPLE_COMM_ISP_ChangeSensorMode(HI_U8 u8Mode)
{
    ISP_DEV IspDev = 0;
    HI_S32 s32Ret;

    ISP_WDR_MODE_S stWDRMode;
    stWDRMode.enWDRMode = u8Mode;
    s32Ret = HI_MPI_ISP_SetWDRMode(IspDev, &stWDRMode);
    if (s32Ret != HI_SUCCESS)
    {
        printf("%s: HI_MPI_ISP_SetWDRMode failed with %#x!\n",
               __FUNCTION__, s32Ret);
        return s32Ret;
    }

    return HI_SUCCESS;
}

HI_S32 SAMPLE_COMM_Sensor_UnRegiter_callback(ISP_DEV IspDev)
{
    ALG_LIB_S stAeLib;
    ALG_LIB_S stAwbLib;
    ALG_LIB_S stAfLib;
    HI_U32 u32SnsId;
    const ISP_SNS_OBJ_S *pstSnsObj;
    HI_S32    s32Ret = -1;

    u32SnsId = g_au32IspSnsId[IspDev];
    if (MAX_SENSOR_NUM <= u32SnsId)
    {
        printf("%s: invalid sensor id: %d\n", __FUNCTION__, u32SnsId);
        return HI_FAILURE;
    }
    pstSnsObj = g_pstSnsObj[u32SnsId];
    if (HI_NULL == pstSnsObj)
    {
        //printf("%s: sensor %d not exist!\n", __FUNCTION__, u32SnsId);
        return HI_FAILURE;
    }

    stAeLib.s32Id = IspDev;
    stAwbLib.s32Id = IspDev;
    stAfLib.s32Id = IspDev;
    strncpy(stAeLib.acLibName, HI_AE_LIB_NAME, sizeof(HI_AE_LIB_NAME));
    strncpy(stAwbLib.acLibName, HI_AWB_LIB_NAME, sizeof(HI_AWB_LIB_NAME));
    strncpy(stAfLib.acLibName, HI_AF_LIB_NAME, sizeof(HI_AF_LIB_NAME));
    if (pstSnsObj->pfnUnRegisterCallback != HI_NULL)
    {
        s32Ret = pstSnsObj->pfnUnRegisterCallback(IspDev, &stAeLib, &stAwbLib);
        if (s32Ret != HI_SUCCESS)
        {
            printf("%s: sensor_unregister_callback failed with %#x!\n", __FUNCTION__, s32Ret);
            return s32Ret;
        }
    }
    else
    {
        printf("%s: sensor_unregister_callback failed with HI_NULL!\n", __FUNCTION__);
    }
    return HI_SUCCESS;
}

/******************************************************************************
* funciton : stop ISP, and stop isp thread
******************************************************************************/
HI_VOID SAMPLE_COMM_ISP_Stop()
{
    ISP_DEV IspDev = 0;

	for(IspDev = 0; IspDev < ISP_MAX_DEV_NUM; IspDev++)
	{
	    if (g_IspPid[IspDev])
	    {
			HI_MPI_ISP_Exit(IspDev);
	        pthread_join(g_IspPid[IspDev], NULL);
			SAMPLE_COMM_ISP_Aflib_UnCallback(IspDev);
			SAMPLE_COMM_ISP_Awblib_UnCallback(IspDev);
			SAMPLE_COMM_ISP_Aelib_UnCallback(IspDev);
			SAMPLE_COMM_Sensor_UnRegiter_callback(IspDev);
	        g_IspPid[IspDev] = 0;
	    }
	}
    return;
}

static ISP_SNS_TYPE_E GetSnsType(SAMPLE_VI_MODE_E enViMode)
{
    ISP_SNS_TYPE_E enSnsType;

    switch (enViMode)
    {
    case SONY_IMX117_LVDS_4K_30FPS:
    case SONY_IMX117_LVDS_12M_28FPS:
    case SONY_IMX117_LVDS_1080P_120FPS:
    case SONY_IMX117_LVDS_720P_240FPS:
    case SONY_IMX226_LVDS_4K_30FPS:
	case SONY_IMX226_LVDS_4K_60FPS:
    case SONY_IMX226_LVDS_9M_30FPS:
    case SONY_IMX274_LVDS_4K_30FPS:
    case PANASONIC_MN34120_LVDS_16M_16P25FPS:
    case PANASONIC_MN34120_LVDS_4K_30FPS:
    case PANASONIC_MN34120_LVDS_1080P_60FPS:
        enSnsType = ISP_SNS_SSP_TYPE;
        break;
    default:
        enSnsType = ISP_SNS_I2C_TYPE;
        break;
    }

    return enSnsType;
}

/*
 * Bind ISP and sensor
 * Parameter:
 *     @IspDev,  ISP device number
 *     @
 */
HI_S32 SAMPLE_COMM_ISP_BindSns(ISP_DEV IspDev, HI_U32 u32SnsId, SAMPLE_VI_MODE_E enViMode, HI_S8 s8SnsDev)
{
    ISP_SNS_COMMBUS_U uSnsBusInfo;
    ISP_SNS_TYPE_E enSnsType;
    const ISP_SNS_OBJ_S *pstSnsObj;
    HI_S32 s32Ret;

    if (MAX_SENSOR_NUM <= u32SnsId)
    {
        printf("%s: invalid sensor id: %d\n", __FUNCTION__, u32SnsId);
        return HI_FAILURE;
    }
    pstSnsObj = g_pstSnsObj[u32SnsId];
    if (HI_NULL == pstSnsObj)
    {
        printf("%s: sensor %d not exist!\n", __FUNCTION__, u32SnsId);
        return HI_FAILURE;
    }

    enSnsType = GetSnsType(enViMode);

    if (ISP_SNS_I2C_TYPE == enSnsType)
    {
        uSnsBusInfo.s8I2cDev = s8SnsDev;
    }
    else
    {
        uSnsBusInfo.s8SspDev.bit4SspDev = s8SnsDev;
        uSnsBusInfo.s8SspDev.bit4SspCs  = 0;
    }

    if (HI_NULL != pstSnsObj->pfnSetBusInfo)
    {
        s32Ret = pstSnsObj->pfnSetBusInfo(IspDev, uSnsBusInfo);
        if (s32Ret != HI_SUCCESS)
        {
            SAMPLE_PRT("set sensor bus info failed with %#x!\n", s32Ret);
            return s32Ret;
        }
    }
    else
    {
        SAMPLE_PRT("not support set sensor bus info!\n");
        return HI_FAILURE;
    }

    return s32Ret;
}


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

