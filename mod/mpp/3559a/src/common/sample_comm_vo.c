

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
#include "hi_mipi_tx.h"

#define SAMPLE_VO_DEF_VALUE (-1)
#define SAMPLE_VO_USE_DEFAULT_MIPI_TX 1

/*******************************
* GLOBAL vars for mipi_tx
*******************************/

combo_dev_cfg_t MIPI_TX_720X576_50_CONFIG =
{
    .devno = 0,
    .lane_id = {0, 1, 2, 3},
    .output_mode = OUTPUT_MODE_DSI_VIDEO,
    .output_format = OUT_FORMAT_RGB_24_BIT,
    .video_mode =  BURST_MODE,
    .sync_info = {
        .vid_pkt_size     = 720,
        .vid_hsa_pixels   = 64,
        .vid_hbp_pixels   = 68,
        .vid_hline_pixels = 864,
        .vid_vsa_lines    = 5,
        .vid_vbp_lines    = 39,
        .vid_vfp_lines    = 5,
        .vid_active_lines = 576,
        .edpi_cmd_size    = 0,
    },
    .phy_data_rate = 459,
    .pixel_clk = 27000,
};


combo_dev_cfg_t MIPI_TX_1280X720_60_CONFIG =
{
    .devno = 0,
    .lane_id = {0, 1, 2, 3},
    .output_mode = OUTPUT_MODE_DSI_VIDEO,
    .output_format = OUT_FORMAT_RGB_24_BIT,
    .video_mode =  BURST_MODE,
    .sync_info = {
        .vid_pkt_size     = 1280,
        .vid_hsa_pixels   = 40,
        .vid_hbp_pixels   = 220,
        .vid_hline_pixels = 1650,
        .vid_vsa_lines    = 5,
        .vid_vbp_lines    = 20,
        .vid_vfp_lines    = 5,
        .vid_active_lines = 720,
        .edpi_cmd_size    = 0,
    },
    .phy_data_rate = 459,
    .pixel_clk = 74250,
};

combo_dev_cfg_t MIPI_TX_1024X768_60_CONFIG =
{
    .devno = 0,
    .lane_id = {0, 1, 2, 3},
    .output_mode = OUTPUT_MODE_DSI_VIDEO,
    .output_format = OUT_FORMAT_RGB_24_BIT,
    .video_mode =  BURST_MODE,
    .sync_info = {
        .vid_pkt_size     = 1024,
        .vid_hsa_pixels   = 136,
        .vid_hbp_pixels   = 160,
        .vid_hline_pixels = 1344,
        .vid_vsa_lines    = 6,
        .vid_vbp_lines    = 29,
        .vid_vfp_lines    = 3,
        .vid_active_lines = 768,
        .edpi_cmd_size    = 0,
    },
    .phy_data_rate = 495,//486
    .pixel_clk = 65000,
};

combo_dev_cfg_t MIPI_TX_1280x1024_60_CONFIG =
{
    .devno = 0,
    .lane_id = {0, 1, 2, 3},
    .output_mode = OUTPUT_MODE_DSI_VIDEO,
    .output_format = OUT_FORMAT_RGB_24_BIT,
    .video_mode =  BURST_MODE,
    .sync_info = {
        .vid_pkt_size     = 1280,
        .vid_hsa_pixels   = 112,
        .vid_hbp_pixels   = 248,
        .vid_hline_pixels = 1688,
        .vid_vsa_lines    = 3,
        .vid_vbp_lines    = 38,
        .vid_vfp_lines    = 1,
        .vid_active_lines = 1024,
        .edpi_cmd_size    = 0,
    },
    .phy_data_rate = 495,//486
    .pixel_clk = 108000,
};


combo_dev_cfg_t MIPI_TX_1920X1080_60_CONFIG =
{
    .devno = 0,
    .lane_id = {0, 1, 2, 3},
    .output_mode = OUTPUT_MODE_DSI_VIDEO,
    .output_format = OUT_FORMAT_RGB_24_BIT,
    .video_mode =  BURST_MODE,
    .sync_info = {
        .vid_pkt_size = 1920,
        .vid_hsa_pixels = 44,
        .vid_hbp_pixels = 148,
        .vid_hline_pixels = 2200,
        .vid_vsa_lines = 5,
        .vid_vbp_lines = 36,
        .vid_vfp_lines = 4,
        .vid_active_lines = 1080,
        .edpi_cmd_size = 0,
    },
    .phy_data_rate = 945,
    .pixel_clk = 148500,
};

combo_dev_cfg_t MIPI_TX_720X1280_60_CONFIG =
{
    .devno = 0,
    .lane_id = {0, 1, 2, 3},
    .output_mode = OUTPUT_MODE_DSI_VIDEO,
    .output_format = OUT_FORMAT_RGB_24_BIT,
    .video_mode =  BURST_MODE,
    .sync_info = {
        .vid_pkt_size     = 720, // hact
        .vid_hsa_pixels   = 24,  // hsa
        .vid_hbp_pixels   = 99,  // hbp
        .vid_hline_pixels = 943, // hact + hsa + hbp + hfp
        .vid_vsa_lines    = 4,   // vsa
        .vid_vbp_lines    = 20,  // vbp
        .vid_vfp_lines    = 8,   // vfp
        .vid_active_lines = 1280,// vact
        .edpi_cmd_size    = 0,
    },
    .phy_data_rate = 459,
    .pixel_clk = 74250,
};

combo_dev_cfg_t MIPI_TX_1080X1920_60_CONFIG =
{
    .devno = 0,
    .lane_id = {0, 1, 2, 3},
    .output_mode = OUTPUT_MODE_DSI_VIDEO,
    .output_format = OUT_FORMAT_RGB_24_BIT,
    .video_mode =  BURST_MODE,
    .sync_info = {
        .vid_pkt_size     = 1080,
        .vid_hsa_pixels   = 8,
        .vid_hbp_pixels   = 20,
        .vid_hline_pixels = 1238,
        .vid_vsa_lines    = 10,
        .vid_vbp_lines    = 26,
        .vid_vfp_lines    = 16,
        .vid_active_lines = 1920,
        .edpi_cmd_size = 0,
    },
    .phy_data_rate = 945,
    .pixel_clk = 148500,
};



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
    stAttr.enVidInMode           = HI_HDMI_VIDEO_MODE_YCBCR444;
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
    stAttr.enVidInMode           = HI_HDMI_VIDEO_MODE_YCBCR444;
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

void SAMPLE_COMM_VO_StartMipiTx(VO_INTF_SYNC_E enVoIntfSync);

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
    VO_CSC_S               stVideoCSC     = {0};
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

    s32Ret = SAMPLE_COMM_VO_StartLayer(VoLayer, &stLayerAttr);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("SAMPLE_COMM_VO_Start video layer failed!\n");
        SAMPLE_COMM_VO_StopDev(VoDev);
        return s32Ret;
    }

    if(VO_INTF_MIPI == enVoIntfType)
    {
        s32Ret = HI_MPI_VO_GetVideoLayerCSC(VoLayer, &stVideoCSC);
        if (HI_SUCCESS != s32Ret)
        {
            SAMPLE_PRT("HI_MPI_VO_GetVideoLayerCSC failed!\n");
            SAMPLE_COMM_VO_StopDev(VoDev);
            return s32Ret;
        }
        stVideoCSC.enCscMatrix =VO_CSC_MATRIX_BT709_TO_RGB_PC;
        s32Ret = HI_MPI_VO_SetVideoLayerCSC(VoLayer, &stVideoCSC);
        if (HI_SUCCESS != s32Ret)
        {
            SAMPLE_PRT("HI_MPI_VO_SetVideoLayerCSC failed!\n");
            SAMPLE_COMM_VO_StopDev(VoDev);
            return s32Ret;
        }
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
    if(VO_INTF_HDMI & enVoIntfType)
    {
        SAMPLE_COMM_VO_HdmiStartByDyRg(enIntfSync, enDstDyRg);
    }

    /******************************
    * Start mipi_tx device.
    * Note : do this after vo device started.
    ********************************/
    if(VO_INTF_MIPI & enVoIntfType)
    {
        SAMPLE_COMM_VO_StartMipiTx(enIntfSync);
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

    if(VO_INTF_HDMI & pstVoConfig->enVoIntfType)
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

/******************************************************************************
* function : vo exit
******************************************************************************/
HI_VOID SAMPLE_COMM_VO_Exit(void)
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

/*
* Name: SAMPLE_PRIVATE_VO_InitScreen1080x1920
* Desc: Initialize the screen(1080x1920) through mipi_tx.
*/
static HI_VOID SAMPLE_PRIVATE_VO_InitScreen1080x1920(HI_S32 fd)
{
    HI_S32 s32Ret;
    cmd_info_t cmd_info;

    cmd_info.devno = 0;
    cmd_info.cmd_size = 0xeeff;
    cmd_info.data_type = 0x23;
    cmd_info.cmd = NULL;
    s32Ret = ioctl(fd, HI_MIPI_TX_SET_CMD, &cmd_info);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("MIPI_TX SET CMD failed\n");
        close(fd);
        return;
    }

    usleep(1000);

    cmd_info.devno = 0;
    cmd_info.cmd_size = 0x4018;
    cmd_info.data_type = 0x23;
    cmd_info.cmd = NULL;
    s32Ret = ioctl(fd, HI_MIPI_TX_SET_CMD, &cmd_info);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("MIPI_TX SET CMD failed\n");
        close(fd);
        return;
    }

    usleep(10000);

    cmd_info.devno = 0;
    cmd_info.cmd_size = 0x18;
    cmd_info.data_type = 0x23;
    cmd_info.cmd = NULL;
    s32Ret = ioctl(fd, HI_MIPI_TX_SET_CMD, &cmd_info);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("MIPI_TX SET CMD failed\n");
        close(fd);
        return;
    }

    usleep(20000);

    cmd_info.devno = 0;
    cmd_info.cmd_size = 0xff;
    cmd_info.data_type = 0x23;
    cmd_info.cmd = NULL;
    s32Ret = ioctl(fd, HI_MIPI_TX_SET_CMD, &cmd_info);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("MIPI_TX SET CMD failed\n");
        close(fd);
        return;
    }

    usleep(10000);

    cmd_info.devno = 0;
    cmd_info.cmd_size = 0x1fb;
    cmd_info.data_type = 0x23;
    cmd_info.cmd = NULL;
    s32Ret = ioctl(fd, HI_MIPI_TX_SET_CMD, &cmd_info);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("MIPI_TX SET CMD failed\n");
        close(fd);
        return;
    }

    usleep(10000);

    cmd_info.devno = 0;
    cmd_info.cmd_size = 0x135;
    cmd_info.data_type = 0x23;
    cmd_info.cmd = NULL;
    s32Ret = ioctl(fd, HI_MIPI_TX_SET_CMD, &cmd_info);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("MIPI_TX SET CMD failed\n");
        close(fd);
        return;
    }

    usleep(10000);

    cmd_info.devno = 0;
    cmd_info.cmd_size = 0xff51;
    cmd_info.data_type = 0x23;
    cmd_info.cmd = NULL;
    s32Ret = ioctl(fd, HI_MIPI_TX_SET_CMD, &cmd_info);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("MIPI_TX SET CMD failed\n");
        close(fd);
        return;
    }

    usleep(1000);

    cmd_info.devno = 0;
    cmd_info.cmd_size = 0x2c53;
    cmd_info.data_type = 0x23;
    cmd_info.cmd = NULL;
    s32Ret = ioctl(fd, HI_MIPI_TX_SET_CMD, &cmd_info);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("MIPI_TX SET CMD failed\n");
        close(fd);
        return;
    }

    usleep(1000);

    cmd_info.devno = 0;
    cmd_info.cmd_size = 0x155;
    cmd_info.data_type = 0x23;
    cmd_info.cmd = NULL;
    s32Ret = ioctl(fd, HI_MIPI_TX_SET_CMD, &cmd_info);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("MIPI_TX SET CMD failed\n");
        close(fd);
        return;
    }

    usleep(1000);

    cmd_info.devno = 0;
    cmd_info.cmd_size = 0x24d3;
    cmd_info.data_type = 0x23;
    cmd_info.cmd = NULL;
    s32Ret = ioctl(fd, HI_MIPI_TX_SET_CMD, &cmd_info);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("MIPI_TX SET CMD failed\n");
        close(fd);
        return;
    }

    usleep(10000);

    cmd_info.devno = 0;
    cmd_info.cmd_size = 0x10d4;
    cmd_info.data_type = 0x23;
    cmd_info.cmd = NULL;
    s32Ret = ioctl(fd, HI_MIPI_TX_SET_CMD, &cmd_info);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("MIPI_TX SET CMD failed\n");
        close(fd);
        return;
    }

    usleep(10000);

    cmd_info.devno = 0;
    cmd_info.cmd_size = 0x11;
    cmd_info.data_type = 0x05;
    cmd_info.cmd = NULL;
    s32Ret = ioctl(fd, HI_MIPI_TX_SET_CMD, &cmd_info);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("MIPI_TX SET CMD failed\n");
        close(fd);
        return;
    }

    usleep(200000);

    cmd_info.devno = 0;
    cmd_info.cmd_size = 0x29;
    cmd_info.data_type = 0x05;
    cmd_info.cmd = NULL;
    s32Ret = ioctl(fd, HI_MIPI_TX_SET_CMD, &cmd_info);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("MIPI_TX SET CMD failed\n");
        close(fd);
        return;
    }
}

static HI_S32 SAMPLE_PRIVATE_VO_SendCmd(HI_S32 fd, HI_U32 data[], HI_U32 cmd)
{
    HI_U8          mipi_tx_cmd[30];
    unsigned short cmd_size  = 0;
    unsigned short data_type = 0x29;
    int            data_num  = 0;
    int            i         = 0;
    int            j         = 0;
    cmd_info_t     cmd_info  = {0};
    HI_S32         s32Ret    = HI_SUCCESS;

    data_type = cmd & 0x3f;
    cmd_size  = (cmd & 0xffff00) >> 8;

    cmd_info.devno     = 0;
    cmd_info.cmd_size  = cmd_size;
    cmd_info.data_type = data_type;

    switch(data_type)
    {
        // data type: 0x29
        case 0x29:
        {
            data_num = (cmd_size + 4 - 1) / 4;
            for (i = 0; i < data_num; i++)
            {
                mipi_tx_cmd[i * 4]     = data[i] & 0xff;
                mipi_tx_cmd[i * 4 + 1] = (data[i] & 0xff00) >> 8;
                mipi_tx_cmd[i * 4 + 2] = (data[i] & 0xff0000) >> 16;
                mipi_tx_cmd[i * 4 + 3] = (data[i] & 0xff000000) >> 24;
            }

            j = cmd_size % 4;
            if (j != 0)
            {
                if (j > 0)
                {
                    mipi_tx_cmd[i * 4]     = data[i] & 0xff;
                }
                if (j > 1)
                {
                    mipi_tx_cmd[i * 4 + 1] = (data[i] & 0xff00) >> 8;
                }
                if (j > 2)
                {
                    mipi_tx_cmd[i * 4 + 2] = (data[i] & 0xff0000) >> 16;
                }
            }
            cmd_info.cmd = mipi_tx_cmd;
            break;
        }

        // data type: 0x23
        // data type: 0x5
        case 0x23:
        case 0x05:
            cmd_info.cmd = NULL;
            break;
        default:
            printf("unknow data_type : 0x%x\n", data_type);
            return HI_FAILURE;
    }


    s32Ret = ioctl(fd, HI_MIPI_TX_SET_CMD, &cmd_info);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("MIPI_TX SET CMD failed\n");
    }

    return s32Ret;
}


/*
* Name: SAMPLE_PRIVATE_VO_InitScreen720x1280
* Desc: Initialize the screen(720x1280) through mipi_tx.
*/
static HI_VOID SAMPLE_PRIVATE_VO_InitScreen720x1280(HI_S32 s32fd)
{
    HI_S32     fd     = s32fd;
    HI_S32     s32Ret;
    HI_U8      cmd[30];
    cmd_info_t cmd_info = {0};
    HI_U32     command = 0;
    HI_U32     data[8] = {0};

    // 0x1117006c 0x23;
    command = 0x23;
    s32Ret = SAMPLE_PRIVATE_VO_SendCmd(fd, NULL, command);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("MIPI_TX SET CMD failed\n");
        close(fd);
        return;
    }
    usleep(1000);

    // 0x11170070 0x018712ff;
    // 0x1117006c 0x00000429;
    data[0] = 0x018712ff;
    command = 0x00000429;
    s32Ret = SAMPLE_PRIVATE_VO_SendCmd(fd, data, command);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("MIPI_TX SET CMD failed\n");
        close(fd);
        return;
    }
    usleep(1000);

    // 0x1117006c 0x00800023;
    cmd_info.devno     = 0;
    cmd_info.cmd_size  = 0x8000;
    cmd_info.data_type = 0x23;
    cmd_info.cmd       = NULL;
    s32Ret = ioctl(fd, HI_MIPI_TX_SET_CMD, &cmd_info);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("MIPI_TX SET CMD failed\n");
        close(fd);
        return;
    }
    usleep(1000);

    // 0x11170070 0x008712ff;
    // 0x1117006c 0x00000329;
    cmd[0] = 0xff;
    cmd[1] = 0x12;
    cmd[2] = 0x87;

    cmd_info.devno     = 0;
    cmd_info.cmd_size  = 0x03;
    cmd_info.data_type = 0x29;
    cmd_info.cmd       = cmd;
    s32Ret = ioctl(fd, HI_MIPI_TX_SET_CMD, &cmd_info);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("MIPI_TX SET CMD failed\n");
        close(fd);
        return;
    }
    usleep(1000);

    // 0x1117006c 0x23;
    cmd_info.devno     = 0;
    cmd_info.cmd_size  = 0x0;
    cmd_info.data_type = 0x23;
    cmd_info.cmd       = NULL;
    s32Ret = ioctl(fd, HI_MIPI_TX_SET_CMD, &cmd_info);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("MIPI_TX SET CMD failed\n");
        close(fd);
        return;
    }
    usleep(1000);


    // 0x11170070 0x312200e1;
    // 0x11170070 0x68635443;
    // 0x11170070 0x61a28896;
    // 0x11170070 0x42415f4c;
    // 0x11170070 0x21262e37;
    // 0x11170070 0x0000001f;
    // 0x1117006c 0x00001529;
    cmd[0] = 0xe1;
    cmd[1] = 0x00;
    cmd[2] = 0x22;
    cmd[3] = 0x31;

    cmd[4] = 0x43;
    cmd[5] = 0x54;
    cmd[6] = 0x63;
    cmd[7] = 0x68;

    cmd[8] = 0x96;
    cmd[9] = 0x88;
    cmd[10] = 0xa2;
    cmd[11] = 0x61;

    cmd[12] = 0x4c;
    cmd[13] = 0x5f;
    cmd[14] = 0x41;
    cmd[15] = 0x42;

    cmd[16] = 0x37;
    cmd[17] = 0x2e;
    cmd[18] = 0x26;
    cmd[19] = 0x21;

    cmd[20] = 0x1f;

    cmd_info.devno     = 0;
    cmd_info.cmd_size  = 0x15;
    cmd_info.data_type = 0x29;
    cmd_info.cmd       = cmd;
    s32Ret = ioctl(fd, HI_MIPI_TX_SET_CMD, &cmd_info);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("MIPI_TX SET CMD failed\n");
        close(fd);
        return;
    }
    usleep(1000);

    // 0x1117006c 0x23;
    cmd_info.devno     = 0;
    cmd_info.cmd_size  = 0x0;
    cmd_info.data_type = 0x23;
    cmd_info.cmd       = NULL;
    s32Ret = ioctl(fd, HI_MIPI_TX_SET_CMD, &cmd_info);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("MIPI_TX SET CMD failed\n");
        close(fd);
        return;
    }
    usleep(1000);

    // 0x11170070 0x312200e2;
    // 0x11170070 0x68635443;
    // 0x11170070 0x61a28896;
    // 0x11170070 0x42415f4c;
    // 0x11170070 0x21262e37;
    // 0x11170070 0x0000001f;
    // 0x1117006c 0x00001529;
    cmd[0] = 0xe2;
    cmd[1] = 0x00;
    cmd[2] = 0x22;
    cmd[3] = 0x31;

    cmd[4] = 0x43;
    cmd[5] = 0x54;
    cmd[6] = 0x63;
    cmd[7] = 0x68;

    cmd[8] = 0x96;
    cmd[9] = 0x88;
    cmd[10] = 0xa2;
    cmd[11] = 0x61;

    cmd[12] = 0x4c;
    cmd[13] = 0x5f;
    cmd[14] = 0x41;
    cmd[15] = 0x42;

    cmd[16] = 0x37;
    cmd[17] = 0x2e;
    cmd[18] = 0x26;
    cmd[19] = 0x21;

    cmd[20] = 0x1f;

    cmd_info.devno     = 0;
    cmd_info.cmd_size  = 0x15;
    cmd_info.data_type = 0x29;
    cmd_info.cmd       = cmd;
    s32Ret = ioctl(fd, HI_MIPI_TX_SET_CMD, &cmd_info);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("MIPI_TX SET CMD failed\n");
        close(fd);
        return;
    }
    usleep(1000);

    // 0x1117006c 0x00910023;
    cmd_info.devno     = 0;
    cmd_info.cmd_size  = 0x9100;
    cmd_info.data_type = 0x23;
    cmd_info.cmd       = NULL;
    s32Ret = ioctl(fd, HI_MIPI_TX_SET_CMD, &cmd_info);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("MIPI_TX SET CMD failed\n");
        close(fd);
        return;
    }
    usleep(1000);

    // 0x11170070 0xa3ffe8ca;
    // 0x11170070 0x05ffa3ff;
    // 0x11170070 0x05030503;
    // 0x11170070 0x00000003;
    // 0x1117006c 0x00000d29;
    cmd[0] = 0xca;
    cmd[1] = 0xe8;
    cmd[2] = 0xff;
    cmd[3] = 0xa3;

    cmd[4] = 0xff;
    cmd[5] = 0xa3;
    cmd[6] = 0xff;
    cmd[7] = 0x05;

    cmd[8] = 0x03;
    cmd[9] = 0x05;
    cmd[10] = 0x03;
    cmd[11] = 0x05;

    cmd[12] = 0x03;

    cmd_info.devno     = 0;
    cmd_info.cmd_size  = 0x0d;
    cmd_info.data_type = 0x29;
    cmd_info.cmd       = cmd;
    s32Ret = ioctl(fd, HI_MIPI_TX_SET_CMD, &cmd_info);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("MIPI_TX SET CMD failed\n");
        close(fd);
        return;
    }
    usleep(1000);

    // 0x1117006c 0x23;
    cmd_info.devno     = 0;
    cmd_info.cmd_size  = 0x0;
    cmd_info.data_type = 0x23;
    cmd_info.cmd       = NULL;
    s32Ret = ioctl(fd, HI_MIPI_TX_SET_CMD, &cmd_info);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("MIPI_TX SET CMD failed\n");
        close(fd);
        return;
    }
    usleep(1000);

    // 0x1117006c 0x0010c623;
    cmd_info.devno     = 0;
    cmd_info.cmd_size  = 0x10c6;
    cmd_info.data_type = 0x23;
    cmd_info.cmd       = NULL;
    s32Ret = ioctl(fd, HI_MIPI_TX_SET_CMD, &cmd_info);
    if (HI_SUCCESS != s32Ret)
    {
        printf("MIPI_TX SET CMD failed\n");
        close(fd);
        return;
    }
    usleep(1000);

    // 0x1117006c 0x00000023;
    cmd_info.devno     = 0;
    cmd_info.cmd_size  = 0x00;
    cmd_info.data_type = 0x23;
    cmd_info.cmd       = NULL;
    s32Ret = ioctl(fd, HI_MIPI_TX_SET_CMD, &cmd_info);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("MIPI_TX SET CMD failed\n");
        close(fd);
        return;
    }
    usleep(1000);

    // 0x11170070 0xacaba0c7;
    // 0x11170070 0xbacca7bc;
    // 0x11170070 0x8888bbbc;
    // 0x11170070 0x44444678;
    // 0x11170070 0x00444444;
    // 0x1117006c 0x00001329;
    cmd[0] = 0xc7;
    cmd[1] = 0xa0;
    cmd[2] = 0xab;
    cmd[3] = 0xac;

    cmd[4] = 0xbc;
    cmd[5] = 0xa7;
    cmd[6] = 0xcc;
    cmd[7] = 0xba;

    cmd[8] = 0xbc;
    cmd[9] = 0xbb;
    cmd[10] = 0x88;
    cmd[11] = 0x88;

    cmd[12] = 0x78;
    cmd[13] = 0x46;
    cmd[14] = 0x44;
    cmd[15] = 0x44;

    cmd[16] = 0x44;
    cmd[17] = 0x44;
    cmd[18] = 0x44;

    cmd_info.devno     = 0;
    cmd_info.cmd_size  = 0x13;
    cmd_info.data_type = 0x29;
    cmd_info.cmd       = cmd;
    s32Ret = ioctl(fd, HI_MIPI_TX_SET_CMD, &cmd_info);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("MIPI_TX SET CMD failed\n");
        close(fd);
        return;
    }
    usleep(1000);

    // 0x1117006c 0x23;
    cmd_info.devno     = 0;
    cmd_info.cmd_size  = 0x0;
    cmd_info.data_type = 0x23;
    cmd_info.cmd       = NULL;
    s32Ret = ioctl(fd, HI_MIPI_TX_SET_CMD, &cmd_info);
    if (HI_SUCCESS != s32Ret)
    {
        printf("MIPI_TX SET CMD failed\n");
        close(fd);
        return;
    }
    usleep(1000);

    // 0x1117006c 0x0011c623;
    cmd_info.devno     = 0;
    cmd_info.cmd_size  = 0x11c6;
    cmd_info.data_type = 0x23;
    cmd_info.cmd       = NULL;
    s32Ret = ioctl(fd, HI_MIPI_TX_SET_CMD, &cmd_info);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("MIPI_TX SET CMD failed\n");
        close(fd);
        return;
    }
    usleep(1000);

    // 0x1117006c 0x00000023;
    cmd_info.devno     = 0;
    cmd_info.cmd_size  = 0x00;
    cmd_info.data_type = 0x23;
    cmd_info.cmd       = NULL;
    s32Ret = ioctl(fd, HI_MIPI_TX_SET_CMD, &cmd_info);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("MIPI_TX SET CMD failed\n");
        close(fd);
        return;
    }
    usleep(1000);

    // 0x11170070 0xabaaa0c7;
    // 0x11170070 0xaacc88bc;
    // 0x11170070 0x8888babc;
    // 0x11170070 0x55555678;
    // 0x11170070 0x00444445;
    // 0x1117006c 0x00001329;
    cmd[0] = 0xc7;
    cmd[1] = 0xa0;
    cmd[2] = 0xaa;
    cmd[3] = 0xab;

    cmd[4] = 0xbc;
    cmd[5] = 0x88;
    cmd[6] = 0xcc;
    cmd[7] = 0xaa;

    cmd[8] = 0xbc;
    cmd[9] = 0xba;
    cmd[10] = 0x88;
    cmd[11] = 0x88;

    cmd[12] = 0x78;
    cmd[13] = 0x56;
    cmd[14] = 0x55;
    cmd[15] = 0x55;

    cmd[16] = 0x45;
    cmd[17] = 0x44;
    cmd[18] = 0x44;

    cmd_info.devno     = 0;
    cmd_info.cmd_size  = 0x13;
    cmd_info.data_type = 0x29;
    cmd_info.cmd       = cmd;
    s32Ret = ioctl(fd, HI_MIPI_TX_SET_CMD, &cmd_info);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("MIPI_TX SET CMD failed\n");
        close(fd);
        return;
    }
    usleep(1000);

    // 0x1117006c 0x23;
    cmd_info.devno     = 0;
    cmd_info.cmd_size  = 0x0;
    cmd_info.data_type = 0x23;
    cmd_info.cmd       = NULL;
    s32Ret = ioctl(fd, HI_MIPI_TX_SET_CMD, &cmd_info);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("MIPI_TX SET CMD failed\n");
        close(fd);
        return;
    }
    usleep(1000);

    // 0x1117006c 0x0012c623;
    cmd_info.devno     = 0;
    cmd_info.cmd_size  = 0x12c6;
    cmd_info.data_type = 0x23;
    cmd_info.cmd       = NULL;
    s32Ret = ioctl(fd, HI_MIPI_TX_SET_CMD, &cmd_info);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("MIPI_TX SET CMD failed\n");
        close(fd);
        return;
    }
    usleep(1000);

    // 0x1117006c 0x00000023;
    cmd_info.devno     = 0;
    cmd_info.cmd_size  = 0x00;
    cmd_info.data_type = 0x23;
    cmd_info.cmd       = NULL;
    s32Ret = ioctl(fd, HI_MIPI_TX_SET_CMD, &cmd_info);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("MIPI_TX SET CMD failed\n");
        close(fd);
        return;
    }
    usleep(1000);

    // 0x11170070 0xab9aa0c7;
    // 0x11170070 0xaabc88ac;
    // 0x11170070 0x8888aabb;
    // 0x11170070 0x55555678;
    // 0x11170070 0x00555555;
    // 0x1117006c 0x00001329;
    cmd[0] = 0xc7;
    cmd[1] = 0xa0;
    cmd[2] = 0x9a;
    cmd[3] = 0xab;

    cmd[4] = 0xac;
    cmd[5] = 0x88;
    cmd[6] = 0xbc;
    cmd[7] = 0xaa;

    cmd[8] = 0xbb;
    cmd[9] = 0xaa;
    cmd[10] = 0x88;
    cmd[11] = 0x88;

    cmd[12] = 0x78;
    cmd[13] = 0x56;
    cmd[14] = 0x55;
    cmd[15] = 0x55;

    cmd[16] = 0x55;
    cmd[17] = 0x55;
    cmd[18] = 0x55;

    cmd_info.devno     = 0;
    cmd_info.cmd_size  = 0x13;
    cmd_info.data_type = 0x29;
    cmd_info.cmd       = cmd;
    s32Ret = ioctl(fd, HI_MIPI_TX_SET_CMD, &cmd_info);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("MIPI_TX SET CMD failed\n");
        close(fd);
        return;
    }
    usleep(1000);

    // 0x1117006c 0x23;
    cmd_info.devno     = 0;
    cmd_info.cmd_size  = 0x0;
    cmd_info.data_type = 0x23;
    cmd_info.cmd       = NULL;
    s32Ret = ioctl(fd, HI_MIPI_TX_SET_CMD, &cmd_info);
    if (HI_SUCCESS != s32Ret)
    {
        printf("MIPI_TX SET CMD failed\n");
        close(fd);
        return;
    }
    usleep(1000);

    // 0x1117006c 0x0013c623;
    cmd_info.devno     = 0;
    cmd_info.cmd_size  = 0x13c6;
    cmd_info.data_type = 0x23;
    cmd_info.cmd       = NULL;
    s32Ret = ioctl(fd, HI_MIPI_TX_SET_CMD, &cmd_info);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("MIPI_TX SET CMD failed\n");
        close(fd);
        return;
    }
    usleep(1000);

    // 0x1117006c 0x00000023;
    cmd_info.devno     = 0;
    cmd_info.cmd_size  = 0x00;
    cmd_info.data_type = 0x23;
    cmd_info.cmd       = NULL;
    s32Ret = ioctl(fd, HI_MIPI_TX_SET_CMD, &cmd_info);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("MIPI_TX SET CMD failed\n");
        close(fd);
        return;
    }
    usleep(1000);

    // 0x11170070 0xab99a0c7;
    // 0x11170070 0x9abb88ba;
    // 0x11170070 0x88889bbb;
    // 0x11170070 0x55556788;
    // 0x11170070 0x00555555;
    // 0x1117006c 0x00001329;
    cmd[0] = 0xc7;
    cmd[1] = 0xa0;
    cmd[2] = 0x99;
    cmd[3] = 0xab;

    cmd[4] = 0xba;
    cmd[5] = 0x88;
    cmd[6] = 0xbb;
    cmd[7] = 0x9a;

    cmd[8] = 0xbb;
    cmd[9] = 0x9b;
    cmd[10] = 0x88;
    cmd[11] = 0x88;

    cmd[12] = 0x88;
    cmd[13] = 0x67;
    cmd[14] = 0x55;
    cmd[15] = 0x55;

    cmd[16] = 0x55;
    cmd[17] = 0x55;
    cmd[18] = 0x55;

    cmd_info.devno     = 0;
    cmd_info.cmd_size  = 0x13;
    cmd_info.data_type = 0x29;
    cmd_info.cmd       = cmd;
    s32Ret = ioctl(fd, HI_MIPI_TX_SET_CMD, &cmd_info);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("MIPI_TX SET CMD failed\n");
        close(fd);
        return;
    }
    usleep(1000);

    // 0x1117006c 0x23;
    cmd_info.devno     = 0;
    cmd_info.cmd_size  = 0x0;
    cmd_info.data_type = 0x23;
    cmd_info.cmd       = NULL;
    s32Ret = ioctl(fd, HI_MIPI_TX_SET_CMD, &cmd_info);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("MIPI_TX SET CMD failed\n");
        close(fd);
        return;
    }
    usleep(1000);

    // 0x1117006c 0x0014c623;
    cmd_info.devno     = 0;
    cmd_info.cmd_size  = 0x14c6;
    cmd_info.data_type = 0x23;
    cmd_info.cmd       = NULL;
    s32Ret = ioctl(fd, HI_MIPI_TX_SET_CMD, &cmd_info);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("MIPI_TX SET CMD failed\n");
        close(fd);
        return;
    }
    usleep(1000);

    // 0x1117006c 0x00000023;
    cmd_info.devno     = 0;
    cmd_info.cmd_size  = 0x00;
    cmd_info.data_type = 0x23;
    cmd_info.cmd       = NULL;
    s32Ret = ioctl(fd, HI_MIPI_TX_SET_CMD, &cmd_info);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("MIPI_TX SET CMD failed\n");
        close(fd);
        return;
    }
    usleep(1000);

    // 0x11170070 0xaa9a90c7;
    // 0x11170070 0xa9ba88ba;
    // 0x11170070 0x88889baa;
    // 0x11170070 0x66666788;
    // 0x11170070 0x00555555;
    // 0x1117006c 0x00001329;
    cmd[0] = 0xc7;
    cmd[1] = 0x90;
    cmd[2] = 0x9a;
    cmd[3] = 0xaa;

    cmd[4] = 0xba;
    cmd[5] = 0x88;
    cmd[6] = 0xba;
    cmd[7] = 0xa9;

    cmd[8] = 0xaa;
    cmd[9] = 0x9b;
    cmd[10] = 0x88;
    cmd[11] = 0x88;

    cmd[12] = 0x88;
    cmd[13] = 0x67;
    cmd[14] = 0x66;
    cmd[15] = 0x66;

    cmd[16] = 0x55;
    cmd[17] = 0x55;
    cmd[18] = 0x55;

    cmd_info.devno     = 0;
    cmd_info.cmd_size  = 0x13;
    cmd_info.data_type = 0x29;
    cmd_info.cmd       = cmd;
    s32Ret = ioctl(fd, HI_MIPI_TX_SET_CMD, &cmd_info);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("MIPI_TX SET CMD failed\n");
        close(fd);
        return;
    }
    usleep(1000);

    // 0x1117006c 0x23;
    cmd_info.devno     = 0;
    cmd_info.cmd_size  = 0x0;
    cmd_info.data_type = 0x23;
    cmd_info.cmd       = NULL;
    s32Ret = ioctl(fd, HI_MIPI_TX_SET_CMD, &cmd_info);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("MIPI_TX SET CMD failed\n");
        close(fd);
        return;
    }
    usleep(1000);

    // 0x1117006c 0x0015c623;
    cmd_info.devno     = 0;
    cmd_info.cmd_size  = 0x15c6;
    cmd_info.data_type = 0x23;
    cmd_info.cmd       = NULL;
    s32Ret = ioctl(fd, HI_MIPI_TX_SET_CMD, &cmd_info);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("MIPI_TX SET CMD failed\n");
        close(fd);
        return;
    }
    usleep(1000);

    // 0x1117006c 0x00000023;
    cmd_info.devno     = 0;
    cmd_info.cmd_size  = 0x00;
    cmd_info.data_type = 0x23;
    cmd_info.cmd       = NULL;
    s32Ret = ioctl(fd, HI_MIPI_TX_SET_CMD, &cmd_info);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("MIPI_TX SET CMD failed\n");
        close(fd);
        return;
    }
    usleep(1000);

    // 0x11170070 0xaa9990c7;
    // 0x11170070 0x99ba88b9;
    // 0x11170070 0x88889baa;
    // 0x11170070 0x66666788;
    // 0x11170070 0x00555666;
    // 0x1117006c 0x00001329;
    cmd[0] = 0xc7;
    cmd[1] = 0x90;
    cmd[2] = 0x99;
    cmd[3] = 0xaa;

    cmd[4] = 0xb9;
    cmd[5] = 0x88;
    cmd[6] = 0xba;
    cmd[7] = 0x99;

    cmd[8] = 0xaa;
    cmd[9] = 0x9b;
    cmd[10] = 0x88;
    cmd[11] = 0x88;

    cmd[12] = 0x88;
    cmd[13] = 0x67;
    cmd[14] = 0x66;
    cmd[15] = 0x66;

    cmd[16] = 0x66;
    cmd[17] = 0x56;
    cmd[18] = 0x55;

    cmd_info.devno     = 0;
    cmd_info.cmd_size  = 0x13;
    cmd_info.data_type = 0x29;
    cmd_info.cmd       = cmd;
    s32Ret = ioctl(fd, HI_MIPI_TX_SET_CMD, &cmd_info);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("MIPI_TX SET CMD failed\n");
        close(fd);
        return;
    }
    usleep(1000);

    // 0x1117006c 0x23;
    cmd_info.devno     = 0;
    cmd_info.cmd_size  = 0x0;
    cmd_info.data_type = 0x23;
    cmd_info.cmd       = NULL;
    s32Ret = ioctl(fd, HI_MIPI_TX_SET_CMD, &cmd_info);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("MIPI_TX SET CMD failed\n");
        close(fd);
        return;
    }
    usleep(1000);

    // 0x1117006c 0x0016c623;
    cmd_info.devno     = 0;
    cmd_info.cmd_size  = 0x16c6;
    cmd_info.data_type = 0x23;
    cmd_info.cmd       = NULL;
    s32Ret = ioctl(fd, HI_MIPI_TX_SET_CMD, &cmd_info);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("MIPI_TX SET CMD failed\n");
        close(fd);
        return;
    }
    usleep(1000);

    // 0x1117006c 0x00000023;
    cmd_info.devno     = 0;
    cmd_info.cmd_size  = 0x00;
    cmd_info.data_type = 0x23;
    cmd_info.cmd       = NULL;
    s32Ret = ioctl(fd, HI_MIPI_TX_SET_CMD, &cmd_info);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("MIPI_TX SET CMD failed\n");
        close(fd);
        return;
    }
    usleep(1000);

    // 0x11170070 0x9a9990c7;
    // 0x11170070 0xa8ba87b9;
    // 0x11170070 0x88889b99;
    // 0x11170070 0x66666888;
    // 0x11170070 0x00666666;
    // 0x1117006c 0x00001329;
    cmd[0] = 0xc7;
    cmd[1] = 0x90;
    cmd[2] = 0x99;
    cmd[3] = 0x9a;

    cmd[4] = 0xb9;
    cmd[5] = 0x87;
    cmd[6] = 0xba;
    cmd[7] = 0xa8;

    cmd[8] = 0x99;
    cmd[9] = 0x9b;
    cmd[10] = 0x88;
    cmd[11] = 0x88;

    cmd[12] = 0x88;
    cmd[13] = 0x68;
    cmd[14] = 0x66;
    cmd[15] = 0x66;

    cmd[16] = 0x66;
    cmd[17] = 0x66;
    cmd[18] = 0x66;

    cmd_info.devno     = 0;
    cmd_info.cmd_size  = 0x13;
    cmd_info.data_type = 0x29;
    cmd_info.cmd       = cmd;
    s32Ret = ioctl(fd, HI_MIPI_TX_SET_CMD, &cmd_info);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("MIPI_TX SET CMD failed\n");
        close(fd);
        return;
    }
    usleep(1000);

    // 0x1117006c 0x23;
    cmd_info.devno     = 0;
    cmd_info.cmd_size  = 0x0;
    cmd_info.data_type = 0x23;
    cmd_info.cmd       = NULL;
    s32Ret = ioctl(fd, HI_MIPI_TX_SET_CMD, &cmd_info);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("MIPI_TX SET CMD failed\n");
        close(fd);
        return;
    }
    usleep(1000);

    // 0x1117006c 0x0017c623;
    cmd_info.devno     = 0;
    cmd_info.cmd_size  = 0x17c6;
    cmd_info.data_type = 0x23;
    cmd_info.cmd       = NULL;
    s32Ret = ioctl(fd, HI_MIPI_TX_SET_CMD, &cmd_info);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("MIPI_TX SET CMD failed\n");
        close(fd);
        return;
    }
    usleep(1000);

    // 0x1117006c 0x00000023;
    cmd_info.devno     = 0;
    cmd_info.cmd_size  = 0x00;
    cmd_info.data_type = 0x23;
    cmd_info.cmd       = NULL;
    s32Ret = ioctl(fd, HI_MIPI_TX_SET_CMD, &cmd_info);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("MIPI_TX SET CMD failed\n");
        close(fd);
        return;
    }
    usleep(1000);

    // 0x11170070 0xaa9890c7;
    // 0x11170070 0x99a987c7;
    // 0x11170070 0x8888aa9a;
    // 0x11170070 0x66677888;
    // 0x11170070 0x00666666;
    // 0x1117006c 0x00001329;
    cmd[0] = 0xc7;
    cmd[1] = 0x90;
    cmd[2] = 0x98;
    cmd[3] = 0xaa;

    cmd[4] = 0xc7;
    cmd[5] = 0x87;
    cmd[6] = 0xa9;
    cmd[7] = 0x99;

    cmd[8] = 0x9a;
    cmd[9] = 0xaa;
    cmd[10] = 0x88;
    cmd[11] = 0x88;

    cmd[12] = 0x88;
    cmd[13] = 0x78;
    cmd[14] = 0x67;
    cmd[15] = 0x66;

    cmd[16] = 0x66;
    cmd[17] = 0x66;
    cmd[18] = 0x66;

    cmd_info.devno     = 0;
    cmd_info.cmd_size  = 0x13;
    cmd_info.data_type = 0x29;
    cmd_info.cmd       = cmd;
    s32Ret = ioctl(fd, HI_MIPI_TX_SET_CMD, &cmd_info);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("MIPI_TX SET CMD failed\n");
        close(fd);
        return;
    }
    usleep(1000);

    // 0x1117006c 0x23;
    cmd_info.devno     = 0;
    cmd_info.cmd_size  = 0x0;
    cmd_info.data_type = 0x23;
    cmd_info.cmd       = NULL;
    s32Ret = ioctl(fd, HI_MIPI_TX_SET_CMD, &cmd_info);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("MIPI_TX SET CMD failed\n");
        close(fd);
        return;
    }
    usleep(1000);

    // 0x1117006c 0x0018c623;
    cmd_info.devno     = 0;
    cmd_info.cmd_size  = 0x18c6;
    cmd_info.data_type = 0x23;
    cmd_info.cmd       = NULL;
    s32Ret = ioctl(fd, HI_MIPI_TX_SET_CMD, &cmd_info);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("MIPI_TX SET CMD failed\n");
        close(fd);
        return;
    }
    usleep(1000);

    // 0x1117006c 0x00000023;
    cmd_info.devno     = 0;
    cmd_info.cmd_size  = 0x00;
    cmd_info.data_type = 0x23;
    cmd_info.cmd       = NULL;
    s32Ret = ioctl(fd, HI_MIPI_TX_SET_CMD, &cmd_info);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("MIPI_TX SET CMD failed\n");
        close(fd);
        return;
    }
    usleep(1000);

    // 0x11170070 0xa99890c7;
    // 0x11170070 0x89b888b7;
    // 0x11170070 0x8888aa99;
    // 0x11170070 0x77777888;
    // 0x11170070 0x00666667;
    // 0x1117006c 0x00001329;
    cmd[0] = 0xc7;
    cmd[1] = 0x90;
    cmd[2] = 0x98;
    cmd[3] = 0xa9;

    cmd[4] = 0xb7;
    cmd[5] = 0x88;
    cmd[6] = 0xb8;
    cmd[7] = 0x89;

    cmd[8] = 0x99;
    cmd[9] = 0xaa;
    cmd[10] = 0x88;
    cmd[11] = 0x88;

    cmd[12] = 0x88;
    cmd[13] = 0x78;
    cmd[14] = 0x77;
    cmd[15] = 0x77;

    cmd[16] = 0x67;
    cmd[17] = 0x66;
    cmd[18] = 0x66;

    cmd_info.devno     = 0;
    cmd_info.cmd_size  = 0x13;
    cmd_info.data_type = 0x29;
    cmd_info.cmd       = cmd;
    s32Ret = ioctl(fd, HI_MIPI_TX_SET_CMD, &cmd_info);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("MIPI_TX SET CMD failed\n");
        close(fd);
        return;
    }
    usleep(1000);

    // 0x1117006c 0x23;
    cmd_info.devno     = 0;
    cmd_info.cmd_size  = 0x0;
    cmd_info.data_type = 0x23;
    cmd_info.cmd       = NULL;
    s32Ret = ioctl(fd, HI_MIPI_TX_SET_CMD, &cmd_info);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("MIPI_TX SET CMD failed\n");
        close(fd);
        return;
    }
    usleep(1000);

    // 0x1117006c 0x0019c623;
    cmd_info.devno     = 0;
    cmd_info.cmd_size  = 0x19c6;
    cmd_info.data_type = 0x23;
    cmd_info.cmd       = NULL;
    s32Ret = ioctl(fd, HI_MIPI_TX_SET_CMD, &cmd_info);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("MIPI_TX SET CMD failed\n");
        close(fd);
        return;
    }
    usleep(1000);

    // 0x1117006c 0x00000023;
    cmd_info.devno     = 0;
    cmd_info.cmd_size  = 0x00;
    cmd_info.data_type = 0x23;
    cmd_info.cmd       = NULL;
    s32Ret = ioctl(fd, HI_MIPI_TX_SET_CMD, &cmd_info);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("MIPI_TX SET CMD failed\n");
        close(fd);
        return;
    }
    usleep(1000);

    // 0x11170070 0xa99790c7;
    // 0x11170070 0x89a888b7;
    // 0x11170070 0x8888a98a;
    // 0x11170070 0x77777888;
    // 0x11170070 0x00667777;
    // 0x1117006c 0x00001329;
    cmd[0] = 0xc7;
    cmd[1] = 0x90;
    cmd[2] = 0x97;
    cmd[3] = 0xa9;

    cmd[4] = 0xb7;
    cmd[5] = 0x88;
    cmd[6] = 0xa8;
    cmd[7] = 0x89;

    cmd[8] = 0x8a;
    cmd[9] = 0xa9;
    cmd[10] = 0x88;
    cmd[11] = 0x88;

    cmd[12] = 0x88;
    cmd[13] = 0x78;
    cmd[14] = 0x77;
    cmd[15] = 0x77;

    cmd[16] = 0x77;
    cmd[17] = 0x77;
    cmd[18] = 0x66;

    cmd_info.devno     = 0;
    cmd_info.cmd_size  = 0x13;
    cmd_info.data_type = 0x29;
    cmd_info.cmd       = cmd;
    s32Ret = ioctl(fd, HI_MIPI_TX_SET_CMD, &cmd_info);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("MIPI_TX SET CMD failed\n");
        close(fd);
        return;
    }
    usleep(1000);

    // 0x1117006c 0x23;
    cmd_info.devno     = 0;
    cmd_info.cmd_size  = 0x0;
    cmd_info.data_type = 0x23;
    cmd_info.cmd       = NULL;
    s32Ret = ioctl(fd, HI_MIPI_TX_SET_CMD, &cmd_info);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("MIPI_TX SET CMD failed\n");
        close(fd);
        return;
    }
    usleep(1000);

    // 0x1117006c 0x001ac623;
    cmd_info.devno     = 0;
    cmd_info.cmd_size  = 0x1ac6;
    cmd_info.data_type = 0x23;
    cmd_info.cmd       = NULL;
    s32Ret = ioctl(fd, HI_MIPI_TX_SET_CMD, &cmd_info);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("MIPI_TX SET CMD failed\n");
        close(fd);
        return;
    }
    usleep(1000);

    // 0x1117006c 0x00000023;
    cmd_info.devno     = 0;
    cmd_info.cmd_size  = 0x00;
    cmd_info.data_type = 0x23;
    cmd_info.cmd       = NULL;
    s32Ret = ioctl(fd, HI_MIPI_TX_SET_CMD, &cmd_info);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("MIPI_TX SET CMD failed\n");
        close(fd);
        return;
    }
    usleep(1000);

    // 0x11170070 0x999970c7;
    // 0x11170070 0x88b887b7;
    // 0x11170070 0x8888a989;
    // 0x11170070 0x77778888;
    // 0x11170070 0x00777777;
    // 0x1117006c 0x00001329;
    cmd[0] = 0xc7;
    cmd[1] = 0x70;
    cmd[2] = 0x99;
    cmd[3] = 0x99;

    cmd[4] = 0xb7;
    cmd[5] = 0x87;
    cmd[6] = 0xb8;
    cmd[7] = 0x88;

    cmd[8] = 0x89;
    cmd[9] = 0xa9;
    cmd[10] = 0x88;
    cmd[11] = 0x88;

    cmd[12] = 0x88;
    cmd[13] = 0x88;
    cmd[14] = 0x77;
    cmd[15] = 0x77;

    cmd[16] = 0x77;
    cmd[17] = 0x77;
    cmd[18] = 0x77;

    cmd_info.devno     = 0;
    cmd_info.cmd_size  = 0x13;
    cmd_info.data_type = 0x29;
    cmd_info.cmd       = cmd;
    s32Ret = ioctl(fd, HI_MIPI_TX_SET_CMD, &cmd_info);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("MIPI_TX SET CMD failed\n");
        close(fd);
        return;
    }
    usleep(1000);

    // 0x1117006c 0x23;
    cmd_info.devno     = 0;
    cmd_info.cmd_size  = 0x0;
    cmd_info.data_type = 0x23;
    cmd_info.cmd       = NULL;
    s32Ret = ioctl(fd, HI_MIPI_TX_SET_CMD, &cmd_info);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("MIPI_TX SET CMD failed\n");
        close(fd);
        return;
    }
    usleep(1000);

    // 0x1117006c 0x001bc623;
    cmd_info.devno     = 0;
    cmd_info.cmd_size  = 0x1bc6;
    cmd_info.data_type = 0x23;
    cmd_info.cmd       = NULL;
    s32Ret = ioctl(fd, HI_MIPI_TX_SET_CMD, &cmd_info);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("MIPI_TX SET CMD failed\n");
        close(fd);
        return;
    }
    usleep(1000);

    // 0x1117006c 0x00000023;
    cmd_info.devno     = 0;
    cmd_info.cmd_size  = 0x00;
    cmd_info.data_type = 0x23;
    cmd_info.cmd       = NULL;
    s32Ret = ioctl(fd, HI_MIPI_TX_SET_CMD, &cmd_info);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("MIPI_TX SET CMD failed\n");
        close(fd);
        return;
    }
    usleep(1000);

    // 0x11170070 0x998970c7;
    // 0x11170070 0x88a888a7;
    // 0x11170070 0x88889989;
    // 0x11170070 0x78888888;
    // 0x11170070 0x00777777;
    // 0x1117006c 0x00001329;
    cmd[0] = 0xc7;
    cmd[1] = 0x70;
    cmd[2] = 0x89;
    cmd[3] = 0x99;

    cmd[4] = 0xa7;
    cmd[5] = 0x88;
    cmd[6] = 0xa8;
    cmd[7] = 0x88;

    cmd[8] = 0x89;
    cmd[9] = 0x99;
    cmd[10] = 0x88;
    cmd[11] = 0x88;

    cmd[12] = 0x88;
    cmd[13] = 0x88;
    cmd[14] = 0x88;
    cmd[15] = 0x78;

    cmd[16] = 0x77;
    cmd[17] = 0x77;
    cmd[18] = 0x77;

    cmd_info.devno     = 0;
    cmd_info.cmd_size  = 0x13;
    cmd_info.data_type = 0x29;
    cmd_info.cmd       = cmd;
    s32Ret = ioctl(fd, HI_MIPI_TX_SET_CMD, &cmd_info);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("MIPI_TX SET CMD failed\n");
        close(fd);
        return;
    }
    usleep(1000);

    // 0x1117006c 0x23;
    cmd_info.devno     = 0;
    cmd_info.cmd_size  = 0x0;
    cmd_info.data_type = 0x23;
    cmd_info.cmd       = NULL;
    s32Ret = ioctl(fd, HI_MIPI_TX_SET_CMD, &cmd_info);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("MIPI_TX SET CMD failed\n");
        close(fd);
        return;
    }
    usleep(1000);

    // 0x1117006c 0x001cc623;
    cmd_info.devno     = 0;
    cmd_info.cmd_size  = 0x1cc6;
    cmd_info.data_type = 0x23;
    cmd_info.cmd       = NULL;
    s32Ret = ioctl(fd, HI_MIPI_TX_SET_CMD, &cmd_info);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("MIPI_TX SET CMD failed\n");
        close(fd);
        return;
    }
    usleep(1000);

    // 0x1117006c 0x00000023;
    cmd_info.devno     = 0;
    cmd_info.cmd_size  = 0x00;
    cmd_info.data_type = 0x23;
    cmd_info.cmd       = NULL;
    s32Ret = ioctl(fd, HI_MIPI_TX_SET_CMD, &cmd_info);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("MIPI_TX SET CMD failed\n");
        close(fd);
        return;
    }
    usleep(1000);

    // 0x11170070 0xa89790c7;
    // 0x11170070 0x98a878a7;
    // 0x11170070 0x88889988;
    // 0x11170070 0x78888888;
    // 0x11170070 0x00777777;
    // 0x1117006c 0x00001329;
    cmd[0] = 0xc7;
    cmd[1] = 0x90;
    cmd[2] = 0x97;
    cmd[3] = 0xa8;

    cmd[4] = 0xa7;
    cmd[5] = 0x78;
    cmd[6] = 0xa8;
    cmd[7] = 0x98;

    cmd[8] = 0x88;
    cmd[9] = 0x99;
    cmd[10] = 0x88;
    cmd[11] = 0x88;

    cmd[12] = 0x88;
    cmd[13] = 0x88;
    cmd[14] = 0x88;
    cmd[15] = 0x78;

    cmd[16] = 0x77;
    cmd[17] = 0x77;
    cmd[18] = 0x77;

    cmd_info.devno     = 0;
    cmd_info.cmd_size  = 0x13;
    cmd_info.data_type = 0x29;
    cmd_info.cmd       = cmd;
    s32Ret = ioctl(fd, HI_MIPI_TX_SET_CMD, &cmd_info);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("MIPI_TX SET CMD failed\n");
        close(fd);
        return;
    }
    usleep(1000);

    // 0x1117006c 0x23;
    cmd_info.devno     = 0;
    cmd_info.cmd_size  = 0x0;
    cmd_info.data_type = 0x23;
    cmd_info.cmd       = NULL;
    s32Ret = ioctl(fd, HI_MIPI_TX_SET_CMD, &cmd_info);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("MIPI_TX SET CMD failed\n");
        close(fd);
        return;
    }
    usleep(1000);

    // 0x1117006c 0x001dc623;
    cmd_info.devno     = 0;
    cmd_info.cmd_size  = 0x1dc6;
    cmd_info.data_type = 0x23;
    cmd_info.cmd       = NULL;
    s32Ret = ioctl(fd, HI_MIPI_TX_SET_CMD, &cmd_info);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("MIPI_TX SET CMD failed\n");
        close(fd);
        return;
    }
    usleep(1000);

    // 0x1117006c 0x00000023;
    cmd_info.devno     = 0;
    cmd_info.cmd_size  = 0x00;
    cmd_info.data_type = 0x23;
    cmd_info.cmd       = NULL;
    s32Ret = ioctl(fd, HI_MIPI_TX_SET_CMD, &cmd_info);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("MIPI_TX SET CMD failed\n");
        close(fd);
        return;
    }
    usleep(1000);

    // 0x11170070 0x898790c7;
    // 0x11170070 0x89988898;
    // 0x11170070 0x88889889;
    // 0x11170070 0x88888888;
    // 0x11170070 0x00777778;
    cmd[0] = 0xc7;
    cmd[1] = 0x90;
    cmd[2] = 0x87;
    cmd[3] = 0x89;

    cmd[4] = 0x98;
    cmd[5] = 0x88;
    cmd[6] = 0x98;
    cmd[7] = 0x89;

    cmd[8] = 0x89;
    cmd[9] = 0x98;
    cmd[10] = 0x88;
    cmd[11] = 0x88;

    cmd[12] = 0x88;
    cmd[13] = 0x88;
    cmd[14] = 0x88;
    cmd[15] = 0x88;

    cmd[16] = 0x78;
    cmd[17] = 0x77;
    cmd[18] = 0x77;

    cmd_info.devno     = 0;
    cmd_info.cmd_size  = 0x13;
    cmd_info.data_type = 0x29;
    cmd_info.cmd       = cmd;
    s32Ret = ioctl(fd, HI_MIPI_TX_SET_CMD, &cmd_info);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("MIPI_TX SET CMD failed\n");
        close(fd);
        return;
    }
    usleep(1000);

    // 0x1117006c 0x23;
    cmd_info.devno     = 0;
    cmd_info.cmd_size  = 0x0;
    cmd_info.data_type = 0x23;
    cmd_info.cmd       = NULL;
    s32Ret = ioctl(fd, HI_MIPI_TX_SET_CMD, &cmd_info);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("MIPI_TX SET CMD failed\n");
        close(fd);
        return;
    }
    usleep(1000);

    // 0x1117006c 0x001ec623;
    cmd_info.devno     = 0;
    cmd_info.cmd_size  = 0x1ec6;
    cmd_info.data_type = 0x23;
    cmd_info.cmd       = NULL;
    s32Ret = ioctl(fd, HI_MIPI_TX_SET_CMD, &cmd_info);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("MIPI_TX SET CMD failed\n");
        close(fd);
        return;
    }
    usleep(1000);

    // 0x1117006c 0x00000023;
    cmd_info.devno     = 0;
    cmd_info.cmd_size  = 0x00;
    cmd_info.data_type = 0x23;
    cmd_info.cmd       = NULL;
    s32Ret = ioctl(fd, HI_MIPI_TX_SET_CMD, &cmd_info);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("MIPI_TX SET CMD failed\n");
        close(fd);
        return;
    }
    usleep(1000);

    // 0x11170070 0x988790c7;
    // 0x11170070 0x88988897;
    // 0x11170070 0x88889889;
    // 0x11170070 0x88888888;
    // 0x11170070 0x00777888;
    // 0x1117006c 0x00001329;
    cmd[0] = 0xc7;
    cmd[1] = 0x90;
    cmd[2] = 0x87;
    cmd[3] = 0x98;

    cmd[4] = 0x97;
    cmd[5] = 0x88;
    cmd[6] = 0x98;
    cmd[7] = 0x88;

    cmd[8] = 0x89;
    cmd[9] = 0x98;
    cmd[10] = 0x88;
    cmd[11] = 0x88;

    cmd[12] = 0x88;
    cmd[13] = 0x88;
    cmd[14] = 0x88;
    cmd[15] = 0x88;

    cmd[16] = 0x88;
    cmd[17] = 0x78;
    cmd[18] = 0x77;

    cmd_info.devno     = 0;
    cmd_info.cmd_size  = 0x13;
    cmd_info.data_type = 0x29;
    cmd_info.cmd       = cmd;
    s32Ret = ioctl(fd, HI_MIPI_TX_SET_CMD, &cmd_info);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("MIPI_TX SET CMD failed\n");
        close(fd);
        return;
    }
    usleep(1000);

    // 0x1117006c 0x23;
    cmd_info.devno     = 0;
    cmd_info.cmd_size  = 0x0;
    cmd_info.data_type = 0x23;
    cmd_info.cmd       = NULL;
    s32Ret = ioctl(fd, HI_MIPI_TX_SET_CMD, &cmd_info);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("MIPI_TX SET CMD failed\n");
        close(fd);
        return;
    }
    usleep(1000);

    // 0x1117006c 0x001fc623;
    cmd_info.devno     = 0;
    cmd_info.cmd_size  = 0x1fc6;
    cmd_info.data_type = 0x23;
    cmd_info.cmd       = NULL;
    s32Ret = ioctl(fd, HI_MIPI_TX_SET_CMD, &cmd_info);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("MIPI_TX SET CMD failed\n");
        close(fd);
        return;
    }
    usleep(1000);

    // 0x1117006c 0x00000023;
    cmd_info.devno     = 0;
    cmd_info.cmd_size  = 0x00;
    cmd_info.data_type = 0x23;
    cmd_info.cmd       = NULL;
    s32Ret = ioctl(fd, HI_MIPI_TX_SET_CMD, &cmd_info);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("MIPI_TX SET CMD failed\n");
        close(fd);
        return;
    }
    usleep(1000);


    // 0x11170070 0x888790c7;
    // 0x11170070 0x88988798;
    // 0x11170070 0x88888888;
    // 0x11170070 0x88888888;
    // 0x11170070 0x00888888;
    // 0x1117006c 0x00001329;
    cmd[0] = 0xc7;
    cmd[1] = 0x90;
    cmd[2] = 0x87;
    cmd[3] = 0x88;

    cmd[4] = 0x98;
    cmd[5] = 0x87;
    cmd[6] = 0x98;
    cmd[7] = 0x88;

    cmd[8] = 0x88;
    cmd[9] = 0x88;
    cmd[10] = 0x88;
    cmd[11] = 0x88;

    cmd[12] = 0x88;
    cmd[13] = 0x88;
    cmd[14] = 0x88;
    cmd[15] = 0x88;

    cmd[16] = 0x88;
    cmd[17] = 0x88;
    cmd[18] = 0x88;

    cmd_info.devno     = 0;
    cmd_info.cmd_size  = 0x13;
    cmd_info.data_type = 0x29;
    cmd_info.cmd       = cmd;
    s32Ret = ioctl(fd, HI_MIPI_TX_SET_CMD, &cmd_info);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("MIPI_TX SET CMD failed\n");
        close(fd);
        return;
    }
    usleep(1000);

    // 0x1117006c 0x23;
    cmd_info.devno     = 0;
    cmd_info.cmd_size  = 0x0;
    cmd_info.data_type = 0x23;
    cmd_info.cmd       = NULL;
    s32Ret = ioctl(fd, HI_MIPI_TX_SET_CMD, &cmd_info);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("MIPI_TX SET CMD failed\n");
        close(fd);
        return;
    }
    usleep(1000);

    // 0x1117006c 0x0000c623;
    cmd_info.devno     = 0;
    cmd_info.cmd_size  = 0x00c6;
    cmd_info.data_type = 0x23;
    cmd_info.cmd       = NULL;
    s32Ret = ioctl(fd, HI_MIPI_TX_SET_CMD, &cmd_info);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("MIPI_TX SET CMD failed\n");
        close(fd);
        return;
    }
    usleep(1000);

    // 0x1117006c 0x00b10023;
    cmd_info.devno     = 0;
    cmd_info.cmd_size  = 0xb100;
    cmd_info.data_type = 0x23;
    cmd_info.cmd       = NULL;
    s32Ret = ioctl(fd, HI_MIPI_TX_SET_CMD, &cmd_info);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("MIPI_TX SET CMD failed\n");
        close(fd);
        return;
    }
    usleep(1000);

    // 0x1117006c 0x0005c623;
    cmd_info.devno     = 0;
    cmd_info.cmd_size  = 0x05c6;
    cmd_info.data_type = 0x23;
    cmd_info.cmd       = NULL;
    s32Ret = ioctl(fd, HI_MIPI_TX_SET_CMD, &cmd_info);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("MIPI_TX SET CMD failed\n");
        close(fd);
        return;
    }
    usleep(1000);

    // 0x1117006c 0x00640023;
    cmd_info.devno     = 0;
    cmd_info.cmd_size  = 0xb400;
    cmd_info.data_type = 0x23;
    cmd_info.cmd       = NULL;
    s32Ret = ioctl(fd, HI_MIPI_TX_SET_CMD, &cmd_info);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("MIPI_TX SET CMD failed\n");
        close(fd);
        return;
    }
    usleep(1000);

    // 0x1117006c 0x0013c623;
    cmd_info.devno     = 0;
    cmd_info.cmd_size  = 0x13c6;
    cmd_info.data_type = 0x23;
    cmd_info.cmd       = NULL;
    s32Ret = ioctl(fd, HI_MIPI_TX_SET_CMD, &cmd_info);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("MIPI_TX SET CMD failed\n");
        close(fd);
        return;
    }
    usleep(1000);

    // 0x1117006c 0x00a00023;
    cmd_info.devno     = 0;
    cmd_info.cmd_size  = 0xa000;
    cmd_info.data_type = 0x23;
    cmd_info.cmd       = NULL;
    s32Ret = ioctl(fd, HI_MIPI_TX_SET_CMD, &cmd_info);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("MIPI_TX SET CMD failed\n");
        close(fd);
        return;
    }
    usleep(1000);


    // 0x1117006c 0x00000023;
    cmd_info.devno     = 0;
    cmd_info.cmd_size  = 0x00;
    cmd_info.data_type = 0x23;
    cmd_info.cmd       = NULL;
    s32Ret = ioctl(fd, HI_MIPI_TX_SET_CMD, &cmd_info);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("MIPI_TX SET CMD failed\n");
        close(fd);
        return;
    }
    usleep(1000);

    // 0x11170070 0x010001d6;
    // 0x11170070 0x014d0100;
    // 0x11170070 0x019a01b3;
    // 0x11170070 0x0000009a;
    // 0x1117006c 0x00000d29;
    cmd[0] = 0xd6;
    cmd[1] = 0x01;
    cmd[2] = 0x00;
    cmd[3] = 0x01;

    cmd[4] = 0x00;
    cmd[5] = 0x01;
    cmd[6] = 0x4d;
    cmd[7] = 0x01;

    cmd[8] = 0xb3;
    cmd[9] = 0x01;
    cmd[10] = 0x9a;
    cmd[11] = 0x01;

    cmd[12] = 0x9a;

    cmd_info.devno     = 0;
    cmd_info.cmd_size  = 0x0d;
    cmd_info.data_type = 0x29;
    cmd_info.cmd       = cmd;
    s32Ret = ioctl(fd, HI_MIPI_TX_SET_CMD, &cmd_info);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("MIPI_TX SET CMD failed\n");
        close(fd);
        return;
    }
    usleep(1000);

    // 0x1117006c 0x00b00023;
    cmd_info.devno     = 0;
    cmd_info.cmd_size  = 0xb000;
    cmd_info.data_type = 0x23;
    cmd_info.cmd       = NULL;
    s32Ret = ioctl(fd, HI_MIPI_TX_SET_CMD, &cmd_info);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("MIPI_TX SET CMD failed\n");
        close(fd);
        return;
    }
    usleep(1000);

    // 0x11170070 0x159a01d6;
    // 0x11170070 0x159a159a;
    // 0x11170070 0x019a019a;
    // 0x11170070 0x0000009a;
    // 0x1117006c 0x00000d29;
    cmd[0] = 0xd6;
    cmd[1] = 0x01;
    cmd[2] = 0x9a;
    cmd[3] = 0x15;

    cmd[4] = 0x9a;
    cmd[5] = 0x15;
    cmd[6] = 0x9a;
    cmd[7] = 0x15;

    cmd[8] = 0x9a;
    cmd[9] = 0x01;
    cmd[10] = 0x9a;
    cmd[11] = 0x01;

    cmd[12] = 0x9a;

    cmd_info.devno     = 0;
    cmd_info.cmd_size  = 0x0d;
    cmd_info.data_type = 0x29;
    cmd_info.cmd       = cmd;
    s32Ret = ioctl(fd, HI_MIPI_TX_SET_CMD, &cmd_info);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("MIPI_TX SET CMD failed\n");
        close(fd);
        return;
    }
    usleep(1000);

    // 0x1117006c 0x00c00023;
    cmd_info.devno     = 0;
    cmd_info.cmd_size  = 0xc000;
    cmd_info.data_type = 0x23;
    cmd_info.cmd       = NULL;
    s32Ret = ioctl(fd, HI_MIPI_TX_SET_CMD, &cmd_info);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("MIPI_TX SET CMD failed\n");
        close(fd);
        return;
    }
    usleep(1000);

    // 0x11170070 0x001133d6;
    // 0x11170070 0x66771166;
    // 0x11170070 0x11666611;
    // 0x11170070 0x00000066;
    // 0x1117006c 0x00000d29;
    cmd[0] = 0xd6;
    cmd[1] = 0x33;
    cmd[2] = 0x11;
    cmd[3] = 0x00;

    cmd[4] = 0x66;
    cmd[5] = 0x11;
    cmd[6] = 0x77;
    cmd[7] = 0x66;

    cmd[8] = 0x11;
    cmd[9] = 0x66;
    cmd[10] = 0x66;
    cmd[11] = 0x11;

    cmd[12] = 0x66;

    cmd_info.devno     = 0;
    cmd_info.cmd_size  = 0x0d;
    cmd_info.data_type = 0x29;
    cmd_info.cmd       = cmd;
    s32Ret = ioctl(fd, HI_MIPI_TX_SET_CMD, &cmd_info);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("MIPI_TX SET CMD failed\n");
        close(fd);
        return;
    }
    usleep(1000);

    // 0x1117006c 0x00d00023;
    cmd_info.devno     = 0;
    cmd_info.cmd_size  = 0xd000;
    cmd_info.data_type = 0x23;
    cmd_info.cmd       = NULL;
    s32Ret = ioctl(fd, HI_MIPI_TX_SET_CMD, &cmd_info);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("MIPI_TX SET CMD failed\n");
        close(fd);
        return;
    }
    usleep(1000);

    // 0x11170070 0x661166d6;
    // 0x11170070 0x00661166;
    // 0x1117006c 0x00000729;
    cmd[0] = 0xd6;
    cmd[1] = 0x66;
    cmd[2] = 0x11;
    cmd[3] = 0x66;

    cmd[4] = 0x66;
    cmd[5] = 0x11;
    cmd[6] = 0x66;

    cmd_info.devno     = 0;
    cmd_info.cmd_size  = 0x07;
    cmd_info.data_type = 0x29;
    cmd_info.cmd       = cmd;
    s32Ret = ioctl(fd, HI_MIPI_TX_SET_CMD, &cmd_info);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("MIPI_TX SET CMD failed\n");
        close(fd);
        return;
    }
    usleep(1000);

    // 0x1117006c 0x00e00023;
    cmd_info.devno     = 0;
    cmd_info.cmd_size  = 0xe000;
    cmd_info.data_type = 0x23;
    cmd_info.cmd       = NULL;
    s32Ret = ioctl(fd, HI_MIPI_TX_SET_CMD, &cmd_info);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("MIPI_TX SET CMD failed\n");
        close(fd);
        return;
    }
    usleep(1000);

    // 0x11170070 0x331133d6;
    // 0x11170070 0x333c1133;
    // 0x11170070 0x11333311;
    // 0x11170070 0x00000033;
    // 0x1117006c 0x00000d29;
    cmd[0] = 0xd6;
    cmd[1] = 0x33;
    cmd[2] = 0x11;
    cmd[3] = 0x33;

    cmd[4] = 0x33;
    cmd[5] = 0x11;
    cmd[6] = 0x3c;
    cmd[7] = 0x33;

    cmd[8] = 0x11;
    cmd[9] = 0x33;
    cmd[10] = 0x33;
    cmd[11] = 0x11;

    cmd[12] = 0x33;

    cmd_info.devno     = 0;
    cmd_info.cmd_size  = 0x0d;
    cmd_info.data_type = 0x29;
    cmd_info.cmd       = cmd;
    s32Ret = ioctl(fd, HI_MIPI_TX_SET_CMD, &cmd_info);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("MIPI_TX SET CMD failed\n");
        close(fd);
        return;
    }
    usleep(1000);

    // 0x1117006c 0x00f00023;
    cmd_info.devno     = 0;
    cmd_info.cmd_size  = 0xf000;
    cmd_info.data_type = 0x23;
    cmd_info.cmd       = NULL;
    s32Ret = ioctl(fd, HI_MIPI_TX_SET_CMD, &cmd_info);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("MIPI_TX SET CMD failed\n");
        close(fd);
        return;
    }
    usleep(1000);

    // 0x11170070 0x331133d6;
    // 0x11170070 0x00331133;
    // 0x1117006c 0x00000729;
    cmd[0] = 0xd6;
    cmd[1] = 0x33;
    cmd[2] = 0x11;
    cmd[3] = 0x33;

    cmd[4] = 0x33;
    cmd[5] = 0x11;
    cmd[6] = 0x33;

    cmd_info.devno     = 0;
    cmd_info.cmd_size  = 0x07;
    cmd_info.data_type = 0x29;
    cmd_info.cmd       = cmd;
    s32Ret = ioctl(fd, HI_MIPI_TX_SET_CMD, &cmd_info);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("MIPI_TX SET CMD failed\n");
        close(fd);
        return;
    }
    usleep(1000);

    // 0x1117006c 0x00800023;
    cmd_info.devno     = 0;
    cmd_info.cmd_size  = 0x8000;
    cmd_info.data_type = 0x23;
    cmd_info.cmd       = NULL;
    s32Ret = ioctl(fd, HI_MIPI_TX_SET_CMD, &cmd_info);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("MIPI_TX SET CMD failed\n");
        close(fd);
        return;
    }
    usleep(1000);

    // 0x1117006c 0x0000d623;
    cmd_info.devno     = 0;
    cmd_info.cmd_size  = 0x00d6;
    cmd_info.data_type = 0x23;
    cmd_info.cmd       = NULL;
    s32Ret = ioctl(fd, HI_MIPI_TX_SET_CMD, &cmd_info);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("MIPI_TX SET CMD failed\n");
        close(fd);
        return;
    }
    usleep(1000);

    // 0x1117006c 0x00d00023;
    cmd_info.devno     = 0;
    cmd_info.cmd_size  = 0xd000;
    cmd_info.data_type = 0x23;
    cmd_info.cmd       = NULL;
    s32Ret = ioctl(fd, HI_MIPI_TX_SET_CMD, &cmd_info);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("MIPI_TX SET CMD failed\n");
        close(fd);
        return;
    }
    usleep(1000);

    // 0x11170070 0x00aaaad9;
    // 0x1117006c 0x00000329;
    cmd[0] = 0xd9;
    cmd[1] = 0xaa;
    cmd[2] = 0xaa;

    cmd_info.devno     = 0;
    cmd_info.cmd_size  = 0x03;
    cmd_info.data_type = 0x29;
    cmd_info.cmd       = cmd;
    s32Ret = ioctl(fd, HI_MIPI_TX_SET_CMD, &cmd_info);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("MIPI_TX SET CMD failed\n");
        close(fd);
        return;
    }
    usleep(1000);

    // 0x1117006c 0x00c20023;
    cmd_info.devno     = 0;
    cmd_info.cmd_size  = 0xc200;
    cmd_info.data_type = 0x23;
    cmd_info.cmd       = NULL;
    s32Ret = ioctl(fd, HI_MIPI_TX_SET_CMD, &cmd_info);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("MIPI_TX SET CMD failed\n");
        close(fd);
        return;
    }
    usleep(1000);

    // 0x1117006c 0x00c0f523;
    cmd_info.devno     = 0;
    cmd_info.cmd_size  = 0xc0f5;
    cmd_info.data_type = 0x23;
    cmd_info.cmd       = NULL;
    s32Ret = ioctl(fd, HI_MIPI_TX_SET_CMD, &cmd_info);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("MIPI_TX SET CMD failed\n");
        close(fd);
        return;
    }
    usleep(1000);

    // 0x1117006c 0x00800023;
    cmd_info.devno     = 0;
    cmd_info.cmd_size  = 0x8000;
    cmd_info.data_type = 0x23;
    cmd_info.cmd       = NULL;
    s32Ret = ioctl(fd, HI_MIPI_TX_SET_CMD, &cmd_info);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("MIPI_TX SET CMD failed\n");
        close(fd);
        return;
    }
    usleep(1000);

    // 0x1117006c 0x0001c423;
    cmd_info.devno     = 0;
    cmd_info.cmd_size  = 0x01c4;
    cmd_info.data_type = 0x23;
    cmd_info.cmd       = NULL;
    s32Ret = ioctl(fd, HI_MIPI_TX_SET_CMD, &cmd_info);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("MIPI_TX SET CMD failed\n");
        close(fd);
        return;
    }
    usleep(1000);

    // 0x1117006c 0x00880023;
    cmd_info.devno     = 0;
    cmd_info.cmd_size  = 0x8800;
    cmd_info.data_type = 0x23;
    cmd_info.cmd       = NULL;
    s32Ret = ioctl(fd, HI_MIPI_TX_SET_CMD, &cmd_info);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("MIPI_TX SET CMD failed\n");
        close(fd);
        return;
    }
    usleep(1000);

    // 0x1117006c 0x0080c423;
    cmd_info.devno     = 0;
    cmd_info.cmd_size  = 0x80c4;
    cmd_info.data_type = 0x23;
    cmd_info.cmd       = NULL;
    s32Ret = ioctl(fd, HI_MIPI_TX_SET_CMD, &cmd_info);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("MIPI_TX SET CMD failed\n");
        close(fd);
        return;
    }
    usleep(1000);

    // 0x1117006c 0x00b20023;
    cmd_info.devno     = 0;
    cmd_info.cmd_size  = 0xb200;
    cmd_info.data_type = 0x23;
    cmd_info.cmd       = NULL;
    s32Ret = ioctl(fd, HI_MIPI_TX_SET_CMD, &cmd_info);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("MIPI_TX SET CMD failed\n");
        close(fd);
        return;
    }
    usleep(1000);

    // 0x1117006c 0x00e3c523;
    cmd_info.devno     = 0;
    cmd_info.cmd_size  = 0xe3c5;
    cmd_info.data_type = 0x23;
    cmd_info.cmd       = NULL;
    s32Ret = ioctl(fd, HI_MIPI_TX_SET_CMD, &cmd_info);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("MIPI_TX SET CMD failed\n");
        close(fd);
        return;
    }
    usleep(1000);
    // 0x1117006c 0x00810023;
    cmd_info.devno     = 0;
    cmd_info.cmd_size  = 0x8100;
    cmd_info.data_type = 0x23;
    cmd_info.cmd       = NULL;
    s32Ret = ioctl(fd, HI_MIPI_TX_SET_CMD, &cmd_info);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("MIPI_TX SET CMD failed\n");
        close(fd);
        return;
    }
    usleep(1000);

    // 0x11170070 0x8f877fca;
    // 0x11170070 0xafa79f97;
    // 0x11170070 0xcfc7bfb7;
    // 0x11170070 0xefe7dfd7;
    // 0x11170070 0x000000f7;
    // 0x1117006c 0x00001129;
    cmd[0] = 0xca;
    cmd[1] = 0x7f;
    cmd[2] = 0x87;
    cmd[3] = 0x8f;

    cmd[4] = 0x97;
    cmd[5] = 0x9f;
    cmd[6] = 0xa7;
    cmd[7] = 0xaf;

    cmd[8] = 0xb7;
    cmd[9] = 0xbf;
    cmd[10] = 0xc7;
    cmd[11] = 0xcf;

    cmd[12] = 0xd7;
    cmd[13] = 0xdf;
    cmd[14] = 0xe7;
    cmd[15] = 0xef;

    cmd[16] = 0xf7;

    cmd_info.devno     = 0;
    cmd_info.cmd_size  = 0x11;
    cmd_info.data_type = 0x29;
    cmd_info.cmd       = cmd;
    s32Ret = ioctl(fd, HI_MIPI_TX_SET_CMD, &cmd_info);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("MIPI_TX SET CMD failed\n");
        close(fd);
        return;
    }
    usleep(1000);

    // 0x1117006c 0x00000023;
    cmd_info.devno     = 0;
    cmd_info.cmd_size  = 0x0000;
    cmd_info.data_type = 0x23;
    cmd_info.cmd       = NULL;
    s32Ret = ioctl(fd, HI_MIPI_TX_SET_CMD, &cmd_info);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("MIPI_TX SET CMD failed\n");
        close(fd);
        return;
    }
    usleep(1000);

    // 0x11170070 0xffffffff;
    // 0x1117006c 0x00000429;
    cmd[0] = 0xff;
    cmd[1] = 0xff;
    cmd[2] = 0xff;
    cmd[3] = 0xff;

    cmd_info.devno     = 0;
    cmd_info.cmd_size  = 0x04;
    cmd_info.data_type = 0x29;
    cmd_info.cmd       = cmd;
    s32Ret = ioctl(fd, HI_MIPI_TX_SET_CMD, &cmd_info);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("MIPI_TX SET CMD failed\n");
        close(fd);
        return;
    }
    usleep(1000);

    // 0x1117006c 0x00000023;
    cmd_info.devno     = 0;
    cmd_info.cmd_size  = 0x0000;
    cmd_info.data_type = 0x23;
    cmd_info.cmd       = NULL;
    s32Ret = ioctl(fd, HI_MIPI_TX_SET_CMD, &cmd_info);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("MIPI_TX SET CMD failed\n");
        close(fd);
        return;
    }
    usleep(1000);

    // 0x1117006c 0x00008123;
    cmd_info.devno     = 0;
    cmd_info.cmd_size  = 0x0081;
    cmd_info.data_type = 0x23;
    cmd_info.cmd       = NULL;
    s32Ret = ioctl(fd, HI_MIPI_TX_SET_CMD, &cmd_info);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("MIPI_TX SET CMD failed\n");
        close(fd);
        return;
    }
    usleep(1000);

    // 0x1117006c 0x00ff5123;
    cmd_info.devno     = 0;
    cmd_info.cmd_size  = 0xff51;
    cmd_info.data_type = 0x23;
    cmd_info.cmd       = NULL;
    s32Ret = ioctl(fd, HI_MIPI_TX_SET_CMD, &cmd_info);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("MIPI_TX SET CMD failed\n");
        close(fd);
        return;
    }
    usleep(1000);

    // 0x1117006c 0x00245323;
    cmd_info.devno     = 0;
    cmd_info.cmd_size  = 0x2453;
    cmd_info.data_type = 0x23;
    cmd_info.cmd       = NULL;
    s32Ret = ioctl(fd, HI_MIPI_TX_SET_CMD, &cmd_info);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("MIPI_TX SET CMD failed\n");
        close(fd);
        return;
    }
    usleep(1000);

    // 0x1117006c 0x00015523;
    cmd_info.devno     = 0;
    cmd_info.cmd_size  = 0x0155;
    cmd_info.data_type = 0x23;
    cmd_info.cmd       = NULL;
    s32Ret = ioctl(fd, HI_MIPI_TX_SET_CMD, &cmd_info);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("MIPI_TX SET CMD failed\n");
        close(fd);
        return;
    }
    usleep(1000);

    // 0x1117006c 0x00285e23;
    cmd_info.devno     = 0;
    cmd_info.cmd_size  = 0x285e;
    cmd_info.data_type = 0x23;
    cmd_info.cmd       = NULL;
    s32Ret = ioctl(fd, HI_MIPI_TX_SET_CMD, &cmd_info);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("MIPI_TX SET CMD failed\n");
        close(fd);
        return;
    }
    usleep(1000);

    // 0x1117006c 0x00001105;
    cmd_info.devno     = 0;
    cmd_info.cmd_size  = 0x0011;
    cmd_info.data_type = 0x05;
    cmd_info.cmd       = NULL;
    s32Ret = ioctl(fd, HI_MIPI_TX_SET_CMD, &cmd_info);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("MIPI_TX SET CMD failed\n");
        close(fd);
        return;
    }
    usleep(200000);

    // 0x1117006c 0x00002905;
    cmd_info.devno     = 0;
    cmd_info.cmd_size  = 0x0029;
    cmd_info.data_type = 0x05;
    cmd_info.cmd       = NULL;
    s32Ret = ioctl(fd, HI_MIPI_TX_SET_CMD, &cmd_info);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("MIPI_TX SET CMD failed\n");
        close(fd);
        return;
    }
    usleep(10000);

    // 0x1117006c 0x00003515;
    cmd_info.devno     = 0;
    cmd_info.cmd_size  = 0x0035;
    cmd_info.data_type = 0x15;
    cmd_info.cmd       = NULL;
    s32Ret = ioctl(fd, HI_MIPI_TX_SET_CMD, &cmd_info);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("MIPI_TX SET CMD failed\n");
        close(fd);
        return;
    }
    usleep(10000);
}

#if SAMPLE_VO_USE_DEFAULT_MIPI_TX

/*
* Name: SAMPLE_PRIVATE_VO_InitMipiTxScreen
* Desc: start mipi_tx screen.
*/
static void SAMPLE_PRIVATE_VO_InitMipiTxScreen(VO_INTF_SYNC_E enVoIntfSync, HI_S32 fd)
{
    if( VO_OUTPUT_1080x1920_60 == enVoIntfSync ||
        VO_OUTPUT_1080P60      == enVoIntfSync)
    {
        SAMPLE_PRT("%s,%d,Init 1080p screen.\n",__FUNCTION__,__LINE__);
        // init screen for 1080x1920_60.
        SAMPLE_PRIVATE_VO_InitScreen1080x1920(fd);
    }
    else if (VO_OUTPUT_720x1280_60 == enVoIntfSync ||
             VO_OUTPUT_720P60      == enVoIntfSync)
    {
        SAMPLE_PRT("%s,%d,Init 720p screen.\n",__FUNCTION__,__LINE__);
        // init screen for 720x1280_60.
        SAMPLE_PRIVATE_VO_InitScreen720x1280(fd);
    }
    else
    {
        SAMPLE_PRT("%s,%d,Init 1080p screen.\n",__FUNCTION__,__LINE__);
        // init screen for 1080x1920_60.
        SAMPLE_PRIVATE_VO_InitScreen1080x1920(fd);
        SAMPLE_PRT("%s,%d,There is no screen to init\n",__FUNCTION__,__LINE__);
    }
}
#else
/*
* Name: SAMPLE_PRIVATE_VO_InitMipiTxDev
* Desc: start mipi_tx device.
*/
static void SAMPLE_PRIVATE_VO_InitMipiTxDev(VO_INTF_SYNC_E enVoIntfSync)
{
    SAMPLE_PRT("error: mipi_tx dev has not been initialized yet\n");
}
#endif

/*
* Name: SAMPLE_COMM_VO_StartMipiTx
* Desc: start mipi_tx units.
*/
void SAMPLE_COMM_VO_StartMipiTx(VO_INTF_SYNC_E enVoIntfSync)
{
    HI_S32            fd;
    HI_S32            s32Ret;
    combo_dev_cfg_t * pstMipiTxConfig;

    fd = open("/dev/hi_mipi_tx", O_RDWR);
    if (fd < 0)
    {
        SAMPLE_PRT("open hi_mipi_tx dev failed\n");
        return;
    }

    switch(enVoIntfSync)
    {
        case VO_OUTPUT_576P50:
            pstMipiTxConfig = &MIPI_TX_720X576_50_CONFIG;
            break;
        case VO_OUTPUT_720P60:
            pstMipiTxConfig = &MIPI_TX_1280X720_60_CONFIG;
            break;
        case VO_OUTPUT_1080P60:
            pstMipiTxConfig = &MIPI_TX_1920X1080_60_CONFIG;
            break;
        case VO_OUTPUT_1024x768_60:
            pstMipiTxConfig = &MIPI_TX_1024X768_60_CONFIG;
            break;
        case VO_OUTPUT_1280x1024_60:
            pstMipiTxConfig = &MIPI_TX_1280x1024_60_CONFIG;
            break;
        case VO_OUTPUT_720x1280_60:
            pstMipiTxConfig = &MIPI_TX_720X1280_60_CONFIG;
            break;
        case VO_OUTPUT_1080x1920_60:
            pstMipiTxConfig = &MIPI_TX_1080X1920_60_CONFIG;
            break;
        default :
            pstMipiTxConfig = &MIPI_TX_1080X1920_60_CONFIG;
            break;
    }
    /*
    * step 1 : Config mipi_tx controller.
    */
    s32Ret = ioctl(fd, HI_MIPI_TX_SET_DEV_CFG, pstMipiTxConfig);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("MIPI_TX SET_DEV_CONFIG failed\n");
        close(fd);
        return;
    }

    /*
    * NOTICE!!! Do it yourself: change SAMPLE_VO_USE_DEFAULT_MIPI_TX to 0.
    * step 2 : Init screen or other peripheral unit.
    */
    #if SAMPLE_VO_USE_DEFAULT_MIPI_TX
    SAMPLE_PRIVATE_VO_InitMipiTxScreen(enVoIntfSync,fd);
    #else
    SAMPLE_PRIVATE_VO_InitMipiTxDev(enVoIntfSync);
    #endif

    usleep(10000);

    /*
    * step 3 : enable mipi_tx controller.
    */
    s32Ret = ioctl(fd, HI_MIPI_TX_ENABLE);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("MIPI_TX enable failed\n");
        close(fd);
        return;
    }

    close(fd);
    return ;
}



#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */
