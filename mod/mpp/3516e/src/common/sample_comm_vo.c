

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

HI_S32 SAMPLE_COMM_VO_StartChn(VO_LAYER VoLayer, VO_CHN VoChn)
{
    HI_S32 s32Ret    = HI_SUCCESS;
    HI_U32 u32Width  = 0;
    HI_U32 u32Height = 0;
    VO_CHN_ATTR_S         stChnAttr;
    VO_VIDEO_LAYER_ATTR_S stLayerAttr;

    s32Ret = HI_MPI_VO_GetVideoLayerAttr(VoLayer, &stLayerAttr);
    if (s32Ret != HI_SUCCESS)
    {
        SAMPLE_PRT("failed with %#x!\n", s32Ret);
        return HI_FAILURE;
    }
    u32Width  = stLayerAttr.stImageSize.u32Width;
    u32Height = stLayerAttr.stImageSize.u32Height;
    SAMPLE_PRT("u32Width:%d, u32Height:%d\n", u32Width, u32Height);

    stChnAttr.stRect.s32X       = 0;
    stChnAttr.stRect.s32Y       = 0;
    stChnAttr.stRect.u32Width   = ALIGN_DOWN(u32Width, 2);
    stChnAttr.stRect.u32Height  = ALIGN_DOWN(u32Height, 2);
    stChnAttr.u32Priority       = 0;
    stChnAttr.bDeflicker        = HI_FALSE;

    s32Ret = HI_MPI_VO_SetChnAttr(VoLayer, VoChn, &stChnAttr);
    if (s32Ret != HI_SUCCESS)
    {
        printf("%s(%d):failed with %#x!\n", __FUNCTION__, __LINE__,  s32Ret);
        return HI_FAILURE;
    }

    s32Ret = HI_MPI_VO_EnableChn(VoLayer, VoChn);
    if (s32Ret != HI_SUCCESS)
    {
        SAMPLE_PRT("failed with %#x!\n", s32Ret);
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

HI_S32 SAMPLE_COMM_VO_StopChn(VO_LAYER VoLayer, VO_CHN VoChn)
{
    HI_S32 s32Ret    = HI_SUCCESS;

    s32Ret = HI_MPI_VO_DisableChn(VoLayer, VoChn);
    if (s32Ret != HI_SUCCESS)
    {
        SAMPLE_PRT("failed with %#x!\n", s32Ret);
        return HI_FAILURE;
    }

    return s32Ret;
}

/*
* Name : SAMPLE_COMM_VO_GetDefConfig
* Desc : An instance of SAMPLE_VO_CONFIG_S, which allows you to use vo immediately.
*/
HI_S32 SAMPLE_COMM_VO_GetDefConfig(SAMPLE_VO_CONFIG_S *pstVoConfig)
{
    RECT_S  stDefDispRect  = {0, 0, 1920, 1080};
    SIZE_S  stDefImageSize = {1920, 1080};
    HI_U32  u32ChipId;
    if(NULL == pstVoConfig)
    {
        SAMPLE_PRT("Error:argument can not be NULL\n");
        return HI_FAILURE;
    }

    pstVoConfig->VoDev             = SAMPLE_VO_DEV_DHD0;

    HI_MPI_SYS_GetChipId(&u32ChipId);

    pstVoConfig->enVoIntfType      = VO_INTF_BT1120;
    pstVoConfig->enIntfSync        = VO_OUTPUT_1080P30;
    pstVoConfig->u32BgColor        = COLOR_RGB_BLUE;
    pstVoConfig->enPixFormat       = PIXEL_FORMAT_YVU_SEMIPLANAR_420;
    pstVoConfig->stDispRect        = stDefDispRect;
    pstVoConfig->stImageSize       = stDefImageSize;
    pstVoConfig->enVoPartMode      = VO_PART_MODE_SINGLE;
    pstVoConfig->u32DisBufLen      = 3;
    pstVoConfig->enDstDynamicRange = DYNAMIC_RANGE_SDR8;

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
    VO_CHN                 VoChn          = 0;
    VO_INTF_TYPE_E         enVoIntfType   = VO_INTF_BT1120;
    VO_INTF_SYNC_E         enIntfSync     = VO_OUTPUT_1080P30;
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
    VoChn          = pstVoConfig->VoDev;
    enVoIntfType   = pstVoConfig->enVoIntfType;
    enIntfSync     = pstVoConfig->enIntfSync;
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
    s32Ret = SAMPLE_COMM_VO_StartChn(VoLayer, VoChn);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("SAMPLE_COMM_VO_StartChn failed!\n");
        SAMPLE_COMM_VO_StopLayer(VoLayer);
        SAMPLE_COMM_VO_StopDev(VoDev);
        return s32Ret;
    }

    return HI_SUCCESS;
}


HI_S32 SAMPLE_COMM_VO_StopVO(SAMPLE_VO_CONFIG_S *pstVoConfig)
{
    VO_DEV              VoDev    = 0;
    VO_LAYER            VoLayer = 0;
    VO_CHN              VoChn   = 0;

    if (NULL == pstVoConfig)
    {
        SAMPLE_PRT("Error:argument can not be NULL\n");
        return HI_FAILURE;
    }

    VoDev     = pstVoConfig->VoDev;
    VoLayer   = pstVoConfig->VoDev;
    VoChn     = pstVoConfig->VoDev;

    SAMPLE_COMM_VO_StopChn(VoLayer, VoChn);
    SAMPLE_COMM_VO_StopLayer(VoLayer);
    SAMPLE_COMM_VO_StopDev(VoDev);

    return HI_SUCCESS;
}

HI_S32 SAMPLE_COMM_VO_GetDefLayerConfig(SAMPLE_COMM_VO_LAYER_CONFIG_S * pstVoLayerConfig)
{
    RECT_S  stDefDispRect   = {0, 0, 1920, 1080};
    SIZE_S  stDefImageSize = {1920, 1080};
    HI_U32  u32Framerate = 30;

    if (NULL == pstVoLayerConfig)
    {
        SAMPLE_PRT("Error:argument can not be NULL\n");
        return HI_FAILURE;
    }
    pstVoLayerConfig->VoLayer        = SAMPLE_VO_LAYER_VHD0;
    pstVoLayerConfig->enIntfSync     = VO_OUTPUT_1080P30;
    pstVoLayerConfig->stDispRect     = stDefDispRect;
    pstVoLayerConfig->stImageSize    = stDefImageSize;
    pstVoLayerConfig->u32Framerate = u32Framerate;
    pstVoLayerConfig->enPixFormat    = PIXEL_FORMAT_YVU_SEMIPLANAR_420;
    pstVoLayerConfig->enDstDynamicRange = DYNAMIC_RANGE_SDR8;
    pstVoLayerConfig->u32DisBufLen   = 3;

    return HI_SUCCESS;
}

HI_S32 SAMPLE_COMM_VO_StartLayerChn(SAMPLE_COMM_VO_LAYER_CONFIG_S * pstVoLayerConfig)
{
    RECT_S                  stDefDispRect     = {0, 0, 1920, 1080};
    SIZE_S                  stDefImageSize    = {1920, 1080};
    VO_LAYER                VoLayer           = 0;
    VO_CHN                  VoChn            = 0;
    VO_INTF_SYNC_E          enIntfSync        = VO_OUTPUT_1080P30;
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
    VoChn             = pstVoLayerConfig->VoLayer;

    /******************************
    * start vo layer.
    ********************************/
    s32Ret = SAMPLE_COMM_VO_GetWH(enIntfSync, &u32Width, &u32Height, &u32Frmt);
    if (HI_SUCCESS != s32Ret)
    {
        printf("Can not get synchronization information!\n");
        return HI_FAILURE;
    }

    if (enIntfSync == VO_OUTPUT_USER) {
        u32Width = pstVoLayerConfig->stImageSize.u32Width;
        u32Height = pstVoLayerConfig->stImageSize.u32Height;
        u32Frmt = pstVoLayerConfig->u32Framerate;
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
    s32Ret = SAMPLE_COMM_VO_StartChn(VoLayer, VoChn);
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
    VO_CHN                VoChn    = 0;

    if (NULL == pstVoLayerConfig)
    {
        SAMPLE_PRT("Error:argument can not be NULL\n");
        return HI_FAILURE;
    }

    VoLayer   = pstVoLayerConfig->VoLayer;
    VoChn  = pstVoLayerConfig->VoLayer;

    SAMPLE_COMM_VO_StopChn(VoLayer, VoChn);
    SAMPLE_COMM_VO_StopLayer(VoLayer);

    return HI_SUCCESS;
}

HI_VOID SAMPLE_COMM_VO_Exit()
{
    HI_S32 i=0, j=0;

    for (i=0; i < VO_MAX_LAYER_NUM; i++)
    {
        for (j=0; j<VO_MAX_CHN_NUM; j++)
        {
            HI_MPI_VO_DisableChn(i, j);
        }
    }

    for (i = 0; i < VO_MAX_LAYER_NUM; i++)
    {
        HI_MPI_VO_DisableVideoLayer(i);
    }

    for (i = 0; i < VO_MAX_DEV_NUM; i++)
    {
        HI_MPI_VO_Disable(i);
    }
}



#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */
