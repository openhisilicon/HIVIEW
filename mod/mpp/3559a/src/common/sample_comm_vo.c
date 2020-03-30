

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

#define SAMPLE_VO_DEF_VALUE (-1)

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
        case VO_OUTPUT_640x480_60:
            *pu32W = 640;
            *pu32H = 480;
            *pu32Frm = 60;
            break;
        case VO_OUTPUT_960H_PAL:
            *pu32W = 960;
            *pu32H = 576;
            *pu32Frm = 50;
            break;
        case VO_OUTPUT_960H_NTSC:
            *pu32W = 960;
            *pu32H = 480;
            *pu32Frm = 60;
            break;
        case VO_OUTPUT_1920x2160_30:
            *pu32W = 1920;
            *pu32H = 2160;
            *pu32Frm = 30;
            break;
        case VO_OUTPUT_2560x1440_30:
            *pu32W = 2560;
            *pu32H = 1440;
            *pu32Frm = 30;
            break;
        case VO_OUTPUT_2560x1600_60:
            *pu32W = 2560;
            *pu32H = 1600;
            *pu32Frm = 60;
            break;
        case VO_OUTPUT_3840x2160_30    :
            *pu32W = 3840;
            *pu32H = 2160;
            *pu32Frm = 30;
            break;
        case VO_OUTPUT_3840x2160_60    :
            *pu32W = 3840;
            *pu32H = 2160;
            *pu32Frm = 60;
            break;
        case VO_OUTPUT_320x240_60    :
            *pu32W = 320;
            *pu32H = 240;
            *pu32Frm = 60;
            break;
        case VO_OUTPUT_320x240_50    :
            *pu32W = 320;
            *pu32H = 240;
            *pu32Frm = 50;
            break;
        case VO_OUTPUT_240x320_50    :
            *pu32W = 240;
            *pu32H = 320;
            *pu32Frm = 50;
            break;
        case VO_OUTPUT_240x320_60    :
            *pu32W = 240;
            *pu32H = 320;
            *pu32Frm = 60;
            break;
        case VO_OUTPUT_800x600_50    :
            *pu32W = 800;
            *pu32H = 600;
            *pu32Frm = 50;
            break;
        case VO_OUTPUT_720x1280_60    :
            *pu32W = 720;
            *pu32H = 1280;
            *pu32Frm = 60;
            break;
        case VO_OUTPUT_1080x1920_60    :
            *pu32W = 1080;
            *pu32H = 1920;
            *pu32Frm = 60;
            break;
        case VO_OUTPUT_7680x4320_30    :
            *pu32W = 7680;
            *pu32H = 4320;
            *pu32Frm = 30;
            break;
        case VO_OUTPUT_USER    :
            *pu32W = 720;
            *pu32H = 576;
            *pu32Frm = 25;
            break;
        default:
            SAMPLE_PRT("vo enIntfSync %d not support!\n", enIntfSync);
            return HI_FAILURE;
    }


    return HI_SUCCESS;
}

HI_S32 SAMPLE_COMM_VO_StartDev(VO_DEV VoDev, VO_PUB_ATTR_S* pstPubAttr)
{
    HI_S32 s32Ret = HI_SUCCESS;

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

HI_S32 SAMPLE_COMM_VO_StartLayer(VO_LAYER VoLayer, const VO_VIDEO_LAYER_ATTR_S* pstLayerAttr)
{
    HI_S32 s32Ret = HI_SUCCESS;

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
    HI_S32 s32Ret    = HI_SUCCESS;
    HI_U32 u32WndNum = 0;
    HI_U32 u32Square = 0;
    HI_U32 u32Row    = 0;
    HI_U32 u32Col    = 0;
    HI_U32 u32Width  = 0;
    HI_U32 u32Height = 0;
    VO_CHN_ATTR_S         stChnAttr;
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
        case VO_MODE_4MUX:
            u32WndNum = 4;
            u32Square = 2;
            break;
        case VO_MODE_8MUX:
            u32WndNum = 8;
            u32Square = 3;
            break;
        case VO_MODE_9MUX:
            u32WndNum = 9;
            u32Square = 3;
            break;
        case VO_MODE_16MUX:
            u32WndNum = 16;
            u32Square = 4;
            break;
        case VO_MODE_25MUX:
            u32WndNum = 25;
            u32Square = 5;
            break;
        case VO_MODE_36MUX:
            u32WndNum = 36;
            u32Square = 6;
            break;
        case VO_MODE_49MUX:
            u32WndNum = 49;
            u32Square = 7;
            break;
        case VO_MODE_64MUX:
            u32WndNum = 64;
            u32Square = 8;
            break;
        case VO_MODE_2X4:
            u32WndNum = 8;
            u32Square = 3;
            u32Row    = 4;
            u32Col    = 2;
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
    u32Width  = stLayerAttr.stImageSize.u32Width;
    u32Height = stLayerAttr.stImageSize.u32Height;
    SAMPLE_PRT("u32Width:%d, u32Height:%d, u32Square:%d\n", u32Width, u32Height, u32Square);
    for (i = 0; i < u32WndNum; i++)
    {
        if( enMode == VO_MODE_1MUX  ||
            enMode == VO_MODE_2MUX  ||
            enMode == VO_MODE_4MUX  ||
            enMode == VO_MODE_8MUX  ||
            enMode == VO_MODE_9MUX  ||
            enMode == VO_MODE_16MUX ||
            enMode == VO_MODE_25MUX ||
            enMode == VO_MODE_36MUX ||
            enMode == VO_MODE_49MUX ||
            enMode == VO_MODE_64MUX )
        {
            stChnAttr.stRect.s32X       = ALIGN_DOWN((u32Width / u32Square) * (i % u32Square), 2);
            stChnAttr.stRect.s32Y       = ALIGN_DOWN((u32Height / u32Square) * (i / u32Square), 2);
            stChnAttr.stRect.u32Width   = ALIGN_DOWN(u32Width / u32Square, 2);
            stChnAttr.stRect.u32Height  = ALIGN_DOWN(u32Height / u32Square, 2);
            stChnAttr.u32Priority       = 0;
            stChnAttr.bDeflicker        = HI_FALSE;
        }
        else if(enMode == VO_MODE_2X4)
        {
            stChnAttr.stRect.s32X       = ALIGN_DOWN((u32Width / u32Col) * (i % u32Col), 2);
            stChnAttr.stRect.s32Y       = ALIGN_DOWN((u32Height / u32Row) * (i / u32Col), 2);
            stChnAttr.stRect.u32Width   = ALIGN_DOWN(u32Width / u32Col, 2);
            stChnAttr.stRect.u32Height  = ALIGN_DOWN(u32Height / u32Row, 2);
            stChnAttr.u32Priority       = 0;
            stChnAttr.bDeflicker        = HI_FALSE;
        }

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
    HI_S32 s32Ret    = HI_SUCCESS;
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
        case VO_MODE_4MUX:
        {
            u32WndNum = 4;
            break;
        }
        case VO_MODE_8MUX:
        {
            u32WndNum = 8;
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

static HI_VOID SAMPLE_PRIVATE_VO_GetDefWbcAttr(VO_WBC_ATTR_S *pstWbcAttr, VO_WBC_SOURCE_S *pstSource)
{
    if (NULL == pstWbcAttr)
    {
        printf("pstWbcAttr is NULL !!, Line:%d,Func:%s\n", __LINE__,__FUNCTION__);
        return;
    }
    pstWbcAttr->stTargetSize.u32Width   = 1920;
    pstWbcAttr->stTargetSize.u32Height  = 1080;
    pstWbcAttr->enPixelFormat           = PIXEL_FORMAT_YVU_SEMIPLANAR_420;
    pstWbcAttr->u32FrameRate            = 30;
    pstWbcAttr->enCompressMode          = COMPRESS_MODE_NONE;
    pstWbcAttr->enDynamicRange          = DYNAMIC_RANGE_SDR8;
    pstSource->enSourceType             = VO_WBC_SOURCE_DEV;
    pstSource->u32SourceId              = SAMPLE_VO_DEV_DHD0;
    return;
}


static HI_S32 SAMPLE_PRIVATE_VO_StartWbc(VO_WBC VoWbc,VO_WBC_ATTR_S *pstWbcAttr, VO_WBC_SOURCE_S *pstSource)
{
    HI_S32 s32Ret = HI_SUCCESS;

    s32Ret = HI_MPI_VO_SetWBCSource(VoWbc,pstSource);
    if (s32Ret != HI_SUCCESS)
    {
        SAMPLE_PRT("failed with %#x!\n", s32Ret);
        return HI_FAILURE;
    }
    s32Ret = HI_MPI_VO_SetWBCAttr(VoWbc,pstWbcAttr);
    if (s32Ret != HI_SUCCESS)
    {
        SAMPLE_PRT("failed with %#x!\n", s32Ret);
        return HI_FAILURE;
    }
    s32Ret = HI_MPI_VO_EnableWBC(VoWbc);
        if (s32Ret != HI_SUCCESS)
    {
        SAMPLE_PRT("failed with %#x!\n", s32Ret);
        return HI_FAILURE;
    }
    return s32Ret;
}

static HI_S32 SAMPLE_PRIVATE_VO_StopWbc(VO_WBC VoWbc)
{
    HI_S32 s32Ret = HI_SUCCESS;

    s32Ret = HI_MPI_VO_DisableWBC(VoWbc);
    if (s32Ret != HI_SUCCESS)
    {
        SAMPLE_PRT("failed with %#x!\n", s32Ret);
        return HI_FAILURE;
    }
    return s32Ret;
}

HI_S32 SAMPLE_COMM_VO_StartWBC(SAMPLE_VO_WBC_CONFIG * pWbcConfig)
{
    HI_S32 s32Ret = HI_SUCCESS;

     /******************************
     *set dynaminc range if changed.
     *******************************/
    if(-1 != pWbcConfig->enDynamicRange)
    {
        pWbcConfig->stWbcAttr.enDynamicRange = pWbcConfig->enDynamicRange;
    }
    /******************************
    * set compress mode if changed.
    *******************************/
    if(-1 != pWbcConfig->enCompressMode)
    {
        pWbcConfig->stWbcAttr.enCompressMode = pWbcConfig->enCompressMode;
    }
    if (-1 < pWbcConfig->s32Depth)
    {
        s32Ret = HI_MPI_VO_SetWBCDepth(pWbcConfig->VoWbc, 5);
        if (HI_SUCCESS != s32Ret)
        {
            printf("Dev(%d) HI_MPI_VO_SetWbcDepth errno %#x\n", pWbcConfig->VoWbc, s32Ret);
            return s32Ret;
        }
    }
    pWbcConfig->stWbcSource.enSourceType = pWbcConfig->enSourceType;
    s32Ret = SAMPLE_PRIVATE_VO_StartWbc(pWbcConfig->VoWbc,
        &pWbcConfig->stWbcAttr,
        &pWbcConfig->stWbcSource);
    return s32Ret;
}

HI_S32 SAMPLE_COMM_VO_StopWBC(SAMPLE_VO_WBC_CONFIG * pWbcConfig)
{
    return SAMPLE_PRIVATE_VO_StopWbc(pWbcConfig->VoWbc);
}

HI_S32 SAMPLE_COMM_VO_GetDefWBCConfig(SAMPLE_VO_WBC_CONFIG * pWbcConfig)
{
    pWbcConfig->VoWbc                    = 0;
    pWbcConfig->enSourceType             = VO_WBC_SOURCE_DEV;
    pWbcConfig->stWbcSource.enSourceType = VO_WBC_SOURCE_DEV;
    pWbcConfig->stWbcSource.u32SourceId  = 0;
    pWbcConfig->enDynamicRange           = SAMPLE_VO_DEF_VALUE;
    pWbcConfig->enCompressMode           = SAMPLE_VO_DEF_VALUE;
    pWbcConfig->s32Depth                 = SAMPLE_VO_DEF_VALUE;

    SAMPLE_PRIVATE_VO_GetDefWbcAttr(&pWbcConfig->stWbcAttr,
        &pWbcConfig->stWbcSource);

    return HI_SUCCESS;
}


HI_VOID SAMPLE_COMM_VO_HdmiConvertSync(VO_INTF_SYNC_E enIntfSync,
    HI_HDMI_VIDEO_FMT_E *penVideoFmt)
{
    switch (enIntfSync)
    {
        case VO_OUTPUT_PAL:
            *penVideoFmt = HI_HDMI_VIDEO_FMT_PAL;
            break;
        case VO_OUTPUT_NTSC:
            *penVideoFmt = HI_HDMI_VIDEO_FMT_NTSC;
            break;
        case VO_OUTPUT_1080P24:
            *penVideoFmt = HI_HDMI_VIDEO_FMT_1080P_24;
            break;
        case VO_OUTPUT_1080P25:
            *penVideoFmt = HI_HDMI_VIDEO_FMT_1080P_25;
            break;
        case VO_OUTPUT_1080P30:
            *penVideoFmt = HI_HDMI_VIDEO_FMT_1080P_30;
            break;
        case VO_OUTPUT_720P50:
            *penVideoFmt = HI_HDMI_VIDEO_FMT_720P_50;
            break;
        case VO_OUTPUT_720P60:
            *penVideoFmt = HI_HDMI_VIDEO_FMT_720P_60;
            break;
        case VO_OUTPUT_1080I50:
            *penVideoFmt = HI_HDMI_VIDEO_FMT_1080i_50;
            break;
        case VO_OUTPUT_1080I60:
            *penVideoFmt = HI_HDMI_VIDEO_FMT_1080i_60;
            break;
        case VO_OUTPUT_1080P50:
            *penVideoFmt = HI_HDMI_VIDEO_FMT_1080P_50;
            break;
        case VO_OUTPUT_1080P60:
            *penVideoFmt = HI_HDMI_VIDEO_FMT_1080P_60;
            break;
        case VO_OUTPUT_576P50:
            *penVideoFmt = HI_HDMI_VIDEO_FMT_576P_50;
            break;
        case VO_OUTPUT_480P60:
            *penVideoFmt = HI_HDMI_VIDEO_FMT_480P_60;
            break;
        case VO_OUTPUT_800x600_60:
            *penVideoFmt = HI_HDMI_VIDEO_FMT_VESA_800X600_60;
            break;
        case VO_OUTPUT_1024x768_60:
            *penVideoFmt = HI_HDMI_VIDEO_FMT_VESA_1024X768_60;
            break;
        case VO_OUTPUT_1280x1024_60:
            *penVideoFmt = HI_HDMI_VIDEO_FMT_VESA_1280X1024_60;
            break;
        case VO_OUTPUT_1366x768_60:
            *penVideoFmt = HI_HDMI_VIDEO_FMT_VESA_1366X768_60;
            break;
        case VO_OUTPUT_1440x900_60:
            *penVideoFmt = HI_HDMI_VIDEO_FMT_VESA_1440X900_60;
            break;
        case VO_OUTPUT_1280x800_60:
            *penVideoFmt = HI_HDMI_VIDEO_FMT_VESA_1280X800_60;
            break;
        case VO_OUTPUT_1920x2160_30:
            *penVideoFmt = HI_HDMI_VIDEO_FMT_1920x2160_30;
            break;
        case VO_OUTPUT_1600x1200_60:
            *penVideoFmt = HI_HDMI_VIDEO_FMT_VESA_1600X1200_60;
            break;
        case VO_OUTPUT_1920x1200_60:
            *penVideoFmt = HI_HDMI_VIDEO_FMT_VESA_1920X1200_60;
            break;
        case VO_OUTPUT_2560x1440_30:
            *penVideoFmt = HI_HDMI_VIDEO_FMT_2560x1440_30;
            break;
        case VO_OUTPUT_2560x1600_60:
            *penVideoFmt = HI_HDMI_VIDEO_FMT_2560x1600_60;
            break;
        case VO_OUTPUT_3840x2160_30:
            *penVideoFmt = HI_HDMI_VIDEO_FMT_3840X2160P_30;
            break;
        case VO_OUTPUT_3840x2160_60:
            *penVideoFmt = HI_HDMI_VIDEO_FMT_3840X2160P_60;
        break;
        default:
            *penVideoFmt = HI_HDMI_VIDEO_FMT_1080P_60;
            break;
    }

    return;
}



HI_S32 SAMPLE_COMM_VO_HdmiStart(VO_INTF_SYNC_E enIntfSync)
{
    HI_HDMI_ATTR_S      stAttr;
    HI_HDMI_VIDEO_FMT_E enVideoFmt;
    HI_HDMI_ID_E        enHdmiId    = HI_HDMI_ID_0;

    SAMPLE_COMM_VO_HdmiConvertSync(enIntfSync, &enVideoFmt);

    CHECK_RET(HI_MPI_HDMI_Init(), "HI_MPI_HDMI_Init");
    CHECK_RET(HI_MPI_HDMI_Open(enHdmiId), "HI_MPI_HDMI_Open");
    CHECK_RET(HI_MPI_HDMI_GetAttr(enHdmiId, &stAttr), "HI_MPI_HDMI_GetAttr");
    stAttr.bEnableHdmi           = HI_TRUE;
    stAttr.bEnableVideo          = HI_TRUE;
    stAttr.enVideoFmt            = enVideoFmt;
    stAttr.enVidOutMode          = HI_HDMI_VIDEO_MODE_YCBCR444;
    stAttr.enDeepColorMode       = HI_HDMI_DEEP_COLOR_24BIT;
    stAttr.bxvYCCMode            = HI_FALSE;
    stAttr.enOutCscQuantization  = HDMI_QUANTIZATION_LIMITED_RANGE;

    stAttr.bEnableAudio          = HI_FALSE;
    stAttr.enSoundIntf           = HI_HDMI_SND_INTERFACE_I2S;
    stAttr.bIsMultiChannel       = HI_FALSE;

    stAttr.enBitDepth            = HI_HDMI_BIT_DEPTH_16;

    stAttr.bEnableAviInfoFrame   = HI_TRUE;
    stAttr.bEnableAudInfoFrame   = HI_TRUE;
    stAttr.bEnableSpdInfoFrame   = HI_FALSE;
    stAttr.bEnableMpegInfoFrame  = HI_FALSE;

    stAttr.bDebugFlag            = HI_FALSE;
    stAttr.bHDCPEnable           = HI_FALSE;

    stAttr.b3DEnable             = HI_FALSE;
    stAttr.enDefaultMode         = HI_HDMI_FORCE_HDMI;

    CHECK_RET(HI_MPI_HDMI_SetAttr(enHdmiId, &stAttr), "HI_MPI_HDMI_SetAttr");
    CHECK_RET(HI_MPI_HDMI_Start(enHdmiId), "HI_MPI_HDMI_Start");

    return HI_SUCCESS;
}

/*
* Name : SAMPLE_COMM_VO_HdmiStartByDyRg
* Desc : Another function to start hdmi, according to video's dynamic range.
*/
HI_S32 SAMPLE_COMM_VO_HdmiStartByDyRg(VO_INTF_SYNC_E enIntfSync, DYNAMIC_RANGE_E enDyRg)
{
    HI_HDMI_ATTR_S          stAttr;
    HI_HDMI_VIDEO_FMT_E     enVideoFmt;
    HI_HDMI_ID_E            enHdmiId    = HI_HDMI_ID_0;

    SAMPLE_COMM_VO_HdmiConvertSync(enIntfSync, &enVideoFmt);

    CHECK_RET(HI_MPI_HDMI_Init(), "HI_MPI_HDMI_Init");
    CHECK_RET(HI_MPI_HDMI_Open(enHdmiId), "HI_MPI_HDMI_Open");
    CHECK_RET(HI_MPI_HDMI_GetAttr(enHdmiId, &stAttr), "HI_MPI_HDMI_GetAttr");
    stAttr.bEnableHdmi           = HI_TRUE;
    stAttr.bEnableVideo          = HI_TRUE;
    stAttr.enVideoFmt            = enVideoFmt;
    stAttr.enVidOutMode          = HI_HDMI_VIDEO_MODE_YCBCR444;
    switch(enDyRg)
    {
        case DYNAMIC_RANGE_SDR8:
            stAttr.enDeepColorMode = HI_HDMI_DEEP_COLOR_24BIT;
            break;
        case DYNAMIC_RANGE_HDR10:
            stAttr.enVidOutMode    = HI_HDMI_VIDEO_MODE_YCBCR422;
            break;
        default:
            stAttr.enDeepColorMode = HI_HDMI_DEEP_COLOR_24BIT;
            break;
    }
    stAttr.bxvYCCMode            = HI_FALSE;
    stAttr.enOutCscQuantization  = HDMI_QUANTIZATION_LIMITED_RANGE;

    stAttr.bEnableAudio          = HI_FALSE;
    stAttr.enSoundIntf           = HI_HDMI_SND_INTERFACE_I2S;
    stAttr.bIsMultiChannel       = HI_FALSE;

    stAttr.enBitDepth            = HI_HDMI_BIT_DEPTH_16;

    stAttr.bEnableAviInfoFrame   = HI_TRUE;
    stAttr.bEnableAudInfoFrame   = HI_TRUE;
    stAttr.bEnableSpdInfoFrame   = HI_FALSE;
    stAttr.bEnableMpegInfoFrame  = HI_FALSE;

    stAttr.bDebugFlag            = HI_FALSE;
    stAttr.bHDCPEnable           = HI_FALSE;

    stAttr.b3DEnable             = HI_FALSE;
    stAttr.enDefaultMode         = HI_HDMI_FORCE_HDMI;

    CHECK_RET(HI_MPI_HDMI_SetAttr(enHdmiId, &stAttr), "HI_MPI_HDMI_SetAttr");
    CHECK_RET(HI_MPI_HDMI_Start(enHdmiId), "HI_MPI_HDMI_Start");

    return HI_SUCCESS;
}


HI_S32 SAMPLE_COMM_VO_HdmiStop(HI_VOID)
{
    HI_HDMI_ID_E enHdmiId = HI_HDMI_ID_0;

    CHECK_RET(HI_MPI_HDMI_Stop(enHdmiId),"HI_MPI_HDMI_Stop");
    CHECK_RET(HI_MPI_HDMI_Close(enHdmiId),"HI_MPI_HDMI_Close");
    CHECK_RET(HI_MPI_HDMI_DeInit(),"HI_MPI_HDMI_DeInit");

    return HI_SUCCESS;
}

/*
* Name : SAMPLE_COMM_VO_GetDefConfig
* Desc : An instance of SAMPLE_VO_CONFIG_S, which allows you to use vo immediately.
*/
HI_S32 SAMPLE_COMM_VO_GetDefConfig(SAMPLE_VO_CONFIG_S *pstVoConfig)
{
    RECT_S  stDefDispRect  = {0, 0, 1920, 1080};
    SIZE_S  stDefImageSize = {1920, 1080};
    if(NULL == pstVoConfig)
    {
        SAMPLE_PRT("Error:argument can not be NULL\n");
        return HI_FAILURE;
    }

    pstVoConfig->VoDev             = SAMPLE_VO_DEV_UHD;
    pstVoConfig->enVoIntfType      = VO_INTF_HDMI;
    pstVoConfig->enIntfSync        = VO_OUTPUT_1080P30;
    pstVoConfig->u32BgColor        = COLOR_RGB_BLUE;
    pstVoConfig->enPixFormat       = PIXEL_FORMAT_YVU_SEMIPLANAR_420;
    pstVoConfig->stDispRect        = stDefDispRect;
    pstVoConfig->stImageSize       = stDefImageSize;
    pstVoConfig->enVoPartMode      = VO_PART_MODE_SINGLE;
    pstVoConfig->u32DisBufLen      = 3;
    pstVoConfig->enDstDynamicRange = DYNAMIC_RANGE_SDR8;
    pstVoConfig->enVoMode          = VO_MODE_1MUX;

    return HI_SUCCESS;
}
HI_S32 SAMPLE_COMM_VO_StartVO(SAMPLE_VO_CONFIG_S *pstVoConfig)
{
    RECT_S                 stDefDispRect  = {0, 0, 1920, 1080};
    SIZE_S                 stDefImageSize = {1920, 1080};

    /*******************************************
    * VO device VoDev# information declaration.
    ********************************************/
    VO_DEV                 VoDev          = 0;
    VO_LAYER               VoLayer        = 0;
    SAMPLE_VO_MODE_E       enVoMode       = 0;
    VO_INTF_TYPE_E         enVoIntfType   = VO_INTF_HDMI;
    VO_INTF_SYNC_E         enIntfSync     = VO_OUTPUT_1080P30;
    DYNAMIC_RANGE_E        enDstDyRg      = DYNAMIC_RANGE_SDR8;
    VO_PART_MODE_E         enVoPartMode   = VO_PART_MODE_SINGLE;
    VO_PUB_ATTR_S          stVoPubAttr    = {0};
    VO_VIDEO_LAYER_ATTR_S  stLayerAttr    = {0};
    HI_S32                 s32Ret         = HI_SUCCESS;

    if (NULL == pstVoConfig)
    {
        SAMPLE_PRT("Error:argument can not be NULL\n");
        return HI_FAILURE;
    }
    VoDev          = pstVoConfig->VoDev;
    VoLayer        = pstVoConfig->VoDev;
    enVoMode       = pstVoConfig->enVoMode;
    enVoIntfType   = pstVoConfig->enVoIntfType;
    enIntfSync     = pstVoConfig->enIntfSync;
    enDstDyRg      = pstVoConfig->enDstDynamicRange;
    enVoPartMode   = pstVoConfig->enVoPartMode;

    /********************************
    * Set and start VO device VoDev#.
    *********************************/
    stVoPubAttr.enIntfType  = enVoIntfType;
    stVoPubAttr.enIntfSync  = enIntfSync;

    stVoPubAttr.u32BgColor  = pstVoConfig->u32BgColor;

    s32Ret = SAMPLE_COMM_VO_StartDev(VoDev, &stVoPubAttr);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("SAMPLE_COMM_VO_StartDev failed!\n");
        return s32Ret;
    }

    /******************************
    * Set and start layer VoDev#.
    ********************************/

    s32Ret = SAMPLE_COMM_VO_GetWH(stVoPubAttr.enIntfSync,
                                  &stLayerAttr.stDispRect.u32Width, &stLayerAttr.stDispRect.u32Height,
                                  &stLayerAttr.u32DispFrmRt);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("SAMPLE_COMM_VO_GetWH failed!\n");
        SAMPLE_COMM_VO_StopDev(VoDev);
        return s32Ret;
    }
    stLayerAttr.bClusterMode     = HI_FALSE;
    stLayerAttr.bDoubleFrame    = HI_FALSE;
    stLayerAttr.enPixFormat       = pstVoConfig->enPixFormat;

    stLayerAttr.stDispRect.s32X = 0;
    stLayerAttr.stDispRect.s32Y = 0;

    /******************************
    // Set display rectangle if changed.
    ********************************/
    if (0 != memcmp(&pstVoConfig->stDispRect, &stDefDispRect, sizeof(RECT_S)))
    {
        stLayerAttr.stDispRect = pstVoConfig->stDispRect;
    }
    stLayerAttr.stImageSize.u32Width  = stLayerAttr.stDispRect.u32Width;
    stLayerAttr.stImageSize.u32Height = stLayerAttr.stDispRect.u32Height;

    stLayerAttr.stImageSize.u32Width  = stLayerAttr.stDispRect.u32Width;
    stLayerAttr.stImageSize.u32Height = stLayerAttr.stDispRect.u32Height;

    /******************************
    //Set image size if changed.
    ********************************/
    if (0 != memcmp(&pstVoConfig->stImageSize, &stDefImageSize, sizeof(SIZE_S)))
    {
        stLayerAttr.stImageSize = pstVoConfig->stImageSize;
    }
    stLayerAttr.enDstDynamicRange     = pstVoConfig->enDstDynamicRange;

    if (pstVoConfig->u32DisBufLen)
    {
        s32Ret = HI_MPI_VO_SetDisplayBufLen(VoLayer, pstVoConfig->u32DisBufLen);
        if (HI_SUCCESS != s32Ret)
        {
            SAMPLE_PRT("HI_MPI_VO_SetDisplayBufLen failed with %#x!\n",s32Ret);
            SAMPLE_COMM_VO_StopDev(VoDev);
            return s32Ret;
        }
    }
    if (VO_PART_MODE_MULTI == enVoPartMode)
    {
        s32Ret = HI_MPI_VO_SetVideoLayerPartitionMode(VoLayer, enVoPartMode);
        if (HI_SUCCESS != s32Ret)
        {
            SAMPLE_PRT("HI_MPI_VO_SetVideoLayerPartitionMode failed!\n");
            SAMPLE_COMM_VO_StopDev(VoDev);
            return s32Ret;
        }
    }

    s32Ret = SAMPLE_COMM_VO_StartLayer(VoLayer, &stLayerAttr);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("SAMPLE_COMM_VO_Start video layer failed!\n");
        SAMPLE_COMM_VO_StopDev(VoDev);
        return s32Ret;
    }

    /******************************
    * start vo channels.
    ********************************/
    s32Ret = SAMPLE_COMM_VO_StartChn(VoLayer, enVoMode);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("SAMPLE_COMM_VO_StartChn failed!\n");
        SAMPLE_COMM_VO_StopLayer(VoLayer);
        SAMPLE_COMM_VO_StopDev(VoDev);
        return s32Ret;
    }

    /******************************
    * Start hdmi device.
    * Note : do this after vo device started.
    ********************************/
    if(VO_INTF_HDMI == enVoIntfType)
    {
        SAMPLE_COMM_VO_HdmiStartByDyRg(enIntfSync, enDstDyRg);
    }

    return HI_SUCCESS;
}

HI_S32 SAMPLE_COMM_VO_StopVO(SAMPLE_VO_CONFIG_S *pstVoConfig)
{
    VO_DEV                VoDev     = 0;
    VO_LAYER              VoLayer   = 0;
    SAMPLE_VO_MODE_E      enVoMode  = VO_MODE_BUTT;

    if (NULL == pstVoConfig)
    {
        SAMPLE_PRT("Error:argument can not be NULL\n");
        return HI_FAILURE;
    }

    VoDev     = pstVoConfig->VoDev;
    VoLayer   = pstVoConfig->VoDev;
    enVoMode  = pstVoConfig->enVoMode;

    if(VO_INTF_HDMI == pstVoConfig->enVoIntfType)
    {
        SAMPLE_COMM_VO_HdmiStop();
    }
    SAMPLE_COMM_VO_StopChn(VoLayer, enVoMode);
    SAMPLE_COMM_VO_StopLayer(VoLayer);
    SAMPLE_COMM_VO_StopDev(VoDev);

    return HI_SUCCESS;
}

HI_S32 SAMPLE_COMM_VO_GetDefLayerConfig(SAMPLE_COMM_VO_LAYER_CONFIG_S * pstVoLayerConfig)
{

    RECT_S  stDefDispRect  = {0, 0, 1920, 1080};
    SIZE_S  stDefImageSize = {1920, 1080};

    if (NULL == pstVoLayerConfig)
    {
        SAMPLE_PRT("Error:argument can not be NULL\n");
        return HI_FAILURE;
    }
    pstVoLayerConfig->VoLayer        = SAMPLE_VO_LAYER_VHD2;
    pstVoLayerConfig->enIntfSync     = VO_OUTPUT_1080P30;
    pstVoLayerConfig->stDispRect     = stDefDispRect;
    pstVoLayerConfig->stImageSize    = stDefImageSize;
    pstVoLayerConfig->enPixFormat    = PIXEL_FORMAT_YVU_SEMIPLANAR_420;
    pstVoLayerConfig->u32DisBufLen   = 3;
    pstVoLayerConfig->enVoMode       = VO_MODE_1MUX;

    return HI_SUCCESS;
}

HI_S32 SAMPLE_COMM_VO_StartLayerChn(SAMPLE_COMM_VO_LAYER_CONFIG_S * pstVoLayerConfig)
{
    RECT_S                  stDefDispRect     = {0, 0, 1920, 1080};
    SIZE_S                  stDefImageSize    = {1920, 1080};
    VO_LAYER                VoLayer           = 0;
    VO_INTF_SYNC_E          enIntfSync        = VO_OUTPUT_1080P30;
    SAMPLE_VO_MODE_E        enVoMode          = VO_MODE_BUTT;
    HI_S32                  s32Ret            = HI_SUCCESS;
    VO_VIDEO_LAYER_ATTR_S   stLayerAttr;
    HI_U32                  u32Frmt, u32Width, u32Height;

    if (NULL == pstVoLayerConfig)
    {
        SAMPLE_PRT("Error:argument can not be NULL\n");
        return HI_FAILURE;
    }

    VoLayer           = pstVoLayerConfig->VoLayer;
    enIntfSync        = pstVoLayerConfig->enIntfSync;
    enVoMode          = pstVoLayerConfig->enVoMode;

    /******************************
    * start vo layer.
    ********************************/
    s32Ret = SAMPLE_COMM_VO_GetWH(enIntfSync, &u32Width, &u32Height, &u32Frmt);
    if (HI_SUCCESS != s32Ret)
    {
        printf("Can not get synchronization information!\n");
        return HI_FAILURE;
    }

    stLayerAttr.stDispRect.s32X       = 0;
    stLayerAttr.stDispRect.s32Y       = 0;
    stLayerAttr.stDispRect.u32Width   = u32Width;
    stLayerAttr.stDispRect.u32Height  = u32Height;
    stLayerAttr.stImageSize.u32Width  = u32Width;
    stLayerAttr.stImageSize.u32Height = u32Height;
    stLayerAttr.u32DispFrmRt          = u32Frmt;
    stLayerAttr.bDoubleFrame          = HI_FALSE;
    stLayerAttr.bClusterMode          = HI_FALSE;
    stLayerAttr.enDstDynamicRange     = DYNAMIC_RANGE_SDR8;

    stLayerAttr.enPixFormat           = pstVoLayerConfig->enPixFormat;

    stLayerAttr.stDispRect.s32X = 0;
    stLayerAttr.stDispRect.s32Y = 0;
    /******************************
    // Set display rectangle if changed.
    ********************************/
    if (0 != memcmp(&pstVoLayerConfig->stDispRect, &stDefDispRect, sizeof(RECT_S)))
    {
        stLayerAttr.stDispRect = pstVoLayerConfig->stDispRect;
    }
    stLayerAttr.stImageSize.u32Width  = stLayerAttr.stDispRect.u32Width;
    stLayerAttr.stImageSize.u32Height = stLayerAttr.stDispRect.u32Height;

    stLayerAttr.stImageSize.u32Width  = stLayerAttr.stDispRect.u32Width;
    stLayerAttr.stImageSize.u32Height = stLayerAttr.stDispRect.u32Height;

    /******************************
    //Set image size if changed.
    ********************************/
    if (0 != memcmp(&pstVoLayerConfig->stImageSize, &stDefImageSize, sizeof(SIZE_S)))
    {
        stLayerAttr.stImageSize = pstVoLayerConfig->stImageSize;
    }
    stLayerAttr.enDstDynamicRange     = pstVoLayerConfig->enDstDynamicRange;

    if (pstVoLayerConfig->u32DisBufLen)
    {
        s32Ret = HI_MPI_VO_SetDisplayBufLen(VoLayer, pstVoLayerConfig->u32DisBufLen);
        if (HI_SUCCESS != s32Ret)
        {
            SAMPLE_PRT("HI_MPI_VO_SetDisplayBufLen failed with %#x!\n",s32Ret);
            return s32Ret;
        }
    }

    s32Ret = SAMPLE_COMM_VO_StartLayer(VoLayer, &stLayerAttr);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("SAMPLE_COMM_VO_Start video layer failed!\n");
        return s32Ret;
    }

    /******************************
    * start vo channels.
    ********************************/
    s32Ret = SAMPLE_COMM_VO_StartChn(VoLayer, enVoMode);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("SAMPLE_COMM_VO_StartChn failed!\n");
        SAMPLE_COMM_VO_StopLayer(VoLayer);
        return s32Ret;
    }
    return s32Ret;
}

HI_S32 SAMPLE_COMM_VO_StopLayerChn(SAMPLE_COMM_VO_LAYER_CONFIG_S * pstVoLayerConfig)
{
    VO_LAYER              VoLayer   = 0;
    SAMPLE_VO_MODE_E      enVoMode  = VO_MODE_BUTT;

    if (NULL == pstVoLayerConfig)
    {
        SAMPLE_PRT("Error:argument can not be NULL\n");
        return HI_FAILURE;
    }

    VoLayer   = pstVoLayerConfig->VoLayer;
    enVoMode  = pstVoLayerConfig->enVoMode;

    SAMPLE_COMM_VO_StopChn(VoLayer, enVoMode);
    SAMPLE_COMM_VO_StopLayer(VoLayer);

    return HI_SUCCESS;
}


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */
