/******************************************************************************
  Some simple Hisilicon Hi3531 video output functions.

  Copyright (C), 2010-2011, Hisilicon Tech. Co., Ltd.
 ******************************************************************************
    Modification:  2011-2 Created
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


HI_S32 SAMPLE_COMM_VO_GetWH(VO_INTF_SYNC_E enIntfSync, HI_U32* pu32W, HI_U32* pu32H, HI_U32* pu32Frm)
{
    switch (enIntfSync)
    {
        case VO_OUTPUT_PAL       :
            *pu32W = 720;
            *pu32H = 576;
            *pu32Frm = 25;
            break;
        case VO_OUTPUT_NTSC      :
            *pu32W = 720;
            *pu32H = 480;
            *pu32Frm = 30;
            break;
        case VO_OUTPUT_576P50    :
            *pu32W = 720;
            *pu32H = 576;
            *pu32Frm = 50;
            break;
        case VO_OUTPUT_480P60    :
            *pu32W = 720;
            *pu32H = 480;
            *pu32Frm = 60;
            break;
        case VO_OUTPUT_800x600_60:
            *pu32W = 800;
            *pu32H = 600;
            *pu32Frm = 60;
            break;
        case VO_OUTPUT_720P50    :
            *pu32W = 1280;
            *pu32H = 720;
            *pu32Frm = 50;
            break;
        case VO_OUTPUT_720P60    :
            *pu32W = 1280;
            *pu32H = 720;
            *pu32Frm = 60;
            break;
        case VO_OUTPUT_1080I50   :
            *pu32W = 1920;
            *pu32H = 1080;
            *pu32Frm = 50;
            break;
        case VO_OUTPUT_1080I60   :
            *pu32W = 1920;
            *pu32H = 1080;
            *pu32Frm = 60;
            break;
        case VO_OUTPUT_1080P24   :
            *pu32W = 1920;
            *pu32H = 1080;
            *pu32Frm = 24;
            break;
        case VO_OUTPUT_1080P25   :
            *pu32W = 1920;
            *pu32H = 1080;
            *pu32Frm = 25;
            break;
        case VO_OUTPUT_1080P30   :
            *pu32W = 1920;
            *pu32H = 1080;
            *pu32Frm = 30;
            break;
        case VO_OUTPUT_1080P50   :
            *pu32W = 1920;
            *pu32H = 1080;
            *pu32Frm = 50;
            break;
        case VO_OUTPUT_1080P60   :
            *pu32W = 1920;
            *pu32H = 1080;
            *pu32Frm = 60;
            break;
        case VO_OUTPUT_1024x768_60:
            *pu32W = 1024;
            *pu32H = 768;
            *pu32Frm = 60;
            break;
        case VO_OUTPUT_1280x1024_60:
            *pu32W = 1280;
            *pu32H = 1024;
            *pu32Frm = 60;
            break;
        case VO_OUTPUT_1366x768_60:
            *pu32W = 1366;
            *pu32H = 768;
            *pu32Frm = 60;
            break;
        case VO_OUTPUT_1440x900_60:
            *pu32W = 1440;
            *pu32H = 900;
            *pu32Frm = 60;
            break;
        case VO_OUTPUT_1280x800_60:
            *pu32W = 1280;
            *pu32H = 800;
            *pu32Frm = 60;
            break;
        case VO_OUTPUT_1600x1200_60:
            *pu32W = 1600;
            *pu32H = 1200;
            *pu32Frm = 60;
            break;
        case VO_OUTPUT_1680x1050_60:
            *pu32W = 1680;
            *pu32H = 1050;
            *pu32Frm = 60;
            break;
        case VO_OUTPUT_1920x1200_60:
            *pu32W = 1920;
            *pu32H = 1200;
            *pu32Frm = 60;
            break;
        case VO_OUTPUT_USER    :
            *pu32W = 720;
            *pu32H = 576;
            *pu32Frm = 25;
            break;
        default:
            SAMPLE_PRT("vo enIntfSync not support!\n");
            return HI_FAILURE;
    }
    return HI_SUCCESS;
}


/******************************************************************************
* function : Set system memory location
******************************************************************************/
HI_S32 SAMPLE_COMM_VO_MemConfig(VO_DEV VoDev, HI_CHAR* pcMmzName)
{
    HI_S32 s32Ret = HI_SUCCESS;
    MPP_CHN_S stMppChnVO;

    /* config vo dev */
    stMppChnVO.enModId  = HI_ID_VOU;
    stMppChnVO.s32DevId = VoDev;
    stMppChnVO.s32ChnId = 0;
    s32Ret = HI_MPI_SYS_SetMemConf(&stMppChnVO, pcMmzName);
    if (s32Ret)
    {
        SAMPLE_PRT("HI_MPI_SYS_SetMemConf ERR !\n");
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

HI_S32 SAMPLE_COMM_VO_StartDev(VO_DEV VoDev, VO_PUB_ATTR_S* pstPubAttr)
{
    HI_S32 s32Ret = HI_SUCCESS;
    //printf("-----------------dev:%d\n", VoDev);
    s32Ret = HI_MPI_VO_SetPubAttr(VoDev, pstPubAttr);
    if (s32Ret != HI_SUCCESS)
    {
        SAMPLE_PRT("failed with %#x!\n", s32Ret);
        return HI_FAILURE;
    }

    s32Ret = HI_MPI_VO_Enable(VoDev);
    if (s32Ret != HI_SUCCESS)
    {
        SAMPLE_PRT("failed with %#x!\n", s32Ret);
        return HI_FAILURE;
    }

    return s32Ret;
}

HI_S32 SAMPLE_COMM_VO_StopDev(VO_DEV VoDev)
{
    HI_S32 s32Ret = HI_SUCCESS;

    s32Ret = HI_MPI_VO_Disable(VoDev);
    if (s32Ret != HI_SUCCESS)
    {
        SAMPLE_PRT("failed with %#x!\n", s32Ret);
        return HI_FAILURE;
    }
    return s32Ret;
}

HI_S32 SAMPLE_COMM_VO_StartLayer(VO_LAYER VoLayer, const VO_VIDEO_LAYER_ATTR_S* pstLayerAttr, HI_BOOL bVgsBypass)
{
    HI_S32 s32Ret = HI_SUCCESS;

    if (!bVgsBypass)
    {
        s32Ret = HI_MPI_VO_SetDispBufLen(VoLayer, 3);
        if (s32Ret != HI_SUCCESS)
        {
            SAMPLE_PRT("failed with %#x!\n", s32Ret);
            return HI_FAILURE;
        }
    }
    s32Ret = HI_MPI_VO_SetVideoLayerAttr(VoLayer, pstLayerAttr);
    if (s32Ret != HI_SUCCESS)
    {
        SAMPLE_PRT("failed with %#x!\n", s32Ret);
        return HI_FAILURE;
    }

    s32Ret = HI_MPI_VO_EnableVideoLayer(VoLayer);
    if (s32Ret != HI_SUCCESS)
    {
        SAMPLE_PRT("failed with %#x!\n", s32Ret);
        return HI_FAILURE;
    }

    return s32Ret;
}

HI_S32 SAMPLE_COMM_VO_StopLayer(VO_LAYER VoLayer)
{
    HI_S32 s32Ret = HI_SUCCESS;

    s32Ret = HI_MPI_VO_DisableVideoLayer(VoLayer);
    if (s32Ret != HI_SUCCESS)
    {
        SAMPLE_PRT("failed with %#x!\n", s32Ret);
        return HI_FAILURE;
    }
    return s32Ret;
}

HI_S32 SAMPLE_COMM_VO_StartChn(VO_LAYER VoLayer, SAMPLE_VO_MODE_E enMode)
{
    HI_S32 i;
    HI_S32 s32Ret = HI_SUCCESS;
    HI_U32 u32WndNum = 0;
    HI_U32 u32Square = 0;
    HI_U32 u32Width = 0;
    HI_U32 u32Height = 0;
    VO_CHN_ATTR_S stChnAttr;
    VO_VIDEO_LAYER_ATTR_S stLayerAttr;

    switch (enMode)
    {
        case VO_MODE_1MUX:
            u32WndNum = 1;
            u32Square = 1;
            break;
        case VO_MODE_2MUX:
            u32WndNum = 2;
            u32Square = 2;
            break;
        default:
            SAMPLE_PRT("failed with %#x!\n", s32Ret);
            return HI_FAILURE;
    }

    s32Ret = HI_MPI_VO_GetVideoLayerAttr(VoLayer, &stLayerAttr);
    if (s32Ret != HI_SUCCESS)
    {
        SAMPLE_PRT("failed with %#x!\n", s32Ret);
        return HI_FAILURE;
    }
    u32Width = stLayerAttr.stImageSize.u32Width;
    u32Height = stLayerAttr.stImageSize.u32Height;
    printf("u32Width:%d, u32Square:%d\n", u32Width, u32Square);
    for (i = 0; i < u32WndNum; i++)
    {
        stChnAttr.stRect.s32X       = ALIGN_BACK((u32Width / u32Square) * (i % u32Square), 2);
        stChnAttr.stRect.s32Y       = ALIGN_BACK((u32Height / u32Square) * (i / u32Square), 2);
        stChnAttr.stRect.u32Width   = ALIGN_BACK(u32Width / u32Square, 2);
        stChnAttr.stRect.u32Height  = ALIGN_BACK(u32Height / u32Square, 2);
        stChnAttr.u32Priority       = 0;
        stChnAttr.bDeflicker        = HI_FALSE;

        s32Ret = HI_MPI_VO_SetChnAttr(VoLayer, i, &stChnAttr);
        if (s32Ret != HI_SUCCESS)
        {
            printf("%s(%d):failed with %#x!\n", \
                   __FUNCTION__, __LINE__,  s32Ret);
            return HI_FAILURE;
        }

        s32Ret = HI_MPI_VO_EnableChn(VoLayer, i);
        if (s32Ret != HI_SUCCESS)
        {
            SAMPLE_PRT("failed with %#x!\n", s32Ret);
            return HI_FAILURE;
        }
    }
    return HI_SUCCESS;
}

HI_S32 SAMPLE_COMM_VO_StopChn(VO_LAYER VoLayer, SAMPLE_VO_MODE_E enMode)
{
    HI_S32 i;
    HI_S32 s32Ret = HI_SUCCESS;
    HI_U32 u32WndNum = 0;

    switch (enMode)
    {
        case VO_MODE_1MUX:
        {
            u32WndNum = 1;
            break;
        }
        case VO_MODE_2MUX:
        {
            u32WndNum = 2;
            break;
        }
        default:
            SAMPLE_PRT("failed with %#x!\n", s32Ret);
            return HI_FAILURE;
    }


    for (i = 0; i < u32WndNum; i++)
    {
        s32Ret = HI_MPI_VO_DisableChn(VoLayer, i);
        if (s32Ret != HI_SUCCESS)
        {
            SAMPLE_PRT("failed with %#x!\n", s32Ret);
            return HI_FAILURE;
        }
    }
    return s32Ret;
}

HI_S32 SAMPLE_COMM_VO_BindVpss(VO_LAYER VoLayer, VO_CHN VoChn, VPSS_GRP VpssGrp, VPSS_CHN VpssChn)
{
    HI_S32 s32Ret = HI_SUCCESS;
    MPP_CHN_S stSrcChn;
    MPP_CHN_S stDestChn;

    stSrcChn.enModId = HI_ID_VPSS;
    stSrcChn.s32DevId = VpssGrp;
    stSrcChn.s32ChnId = VpssChn;

    stDestChn.enModId = HI_ID_VOU;
    stDestChn.s32DevId = VoLayer;
    stDestChn.s32ChnId = VoChn;

    s32Ret = HI_MPI_SYS_Bind(&stSrcChn, &stDestChn);
    if (s32Ret != HI_SUCCESS)
    {
        SAMPLE_PRT("failed with %#x!\n", s32Ret);
        return HI_FAILURE;
    }

    return s32Ret;
}
HI_S32 SAMPLE_COMM_VO_UnBindVpss(VO_LAYER VoLayer, VO_CHN VoChn, VPSS_GRP VpssGrp, VPSS_CHN VpssChn)
{
    HI_S32 s32Ret = HI_SUCCESS;
    MPP_CHN_S stSrcChn;
    MPP_CHN_S stDestChn;

    stSrcChn.enModId = HI_ID_VPSS;
    stSrcChn.s32DevId = VpssGrp;
    stSrcChn.s32ChnId = VpssChn;

    stDestChn.enModId = HI_ID_VOU;
    stDestChn.s32DevId = VoLayer;
    stDestChn.s32ChnId = VoChn;

    s32Ret = HI_MPI_SYS_UnBind(&stSrcChn, &stDestChn);
    if (s32Ret != HI_SUCCESS)
    {
        SAMPLE_PRT("failed with %#x!\n", s32Ret);
        return HI_FAILURE;
    }
    return s32Ret;
}

HI_S32 SAMPLE_COMM_VO_BindVi(VO_LAYER VoLayer, VO_CHN VoChn, VI_CHN ViChn)
{
    MPP_CHN_S stSrcChn, stDestChn;

    stSrcChn.enModId    = HI_ID_VIU;
    stSrcChn.s32DevId   = 0;
    stSrcChn.s32ChnId   = ViChn;

    stDestChn.enModId   = HI_ID_VOU;
    stDestChn.s32ChnId  = VoChn;
    stDestChn.s32DevId  = VoLayer;

    return HI_MPI_SYS_Bind(&stSrcChn, &stDestChn);
}

HI_S32 SAMPLE_COMM_VO_UnBindVi(VO_LAYER VoLayer, VO_CHN VoChn)
{
    MPP_CHN_S stDestChn;

    stDestChn.enModId   = HI_ID_VOU;
    stDestChn.s32DevId  = VoLayer;
    stDestChn.s32ChnId  = VoChn;

    return HI_MPI_SYS_UnBind(NULL, &stDestChn);
}




#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */
