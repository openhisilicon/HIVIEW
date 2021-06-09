

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

//maohw
combo_dev_cfg_t MIPI_TX_800X1280_60_CONFIG =
{
    .devno = 0,
    .lane_id = {0, 1, 2, 3},
    .output_mode = OUTPUT_MODE_DSI_VIDEO,
    .output_format = OUT_FORMAT_RGB_24_BIT,
    .video_mode =  BURST_MODE,
    .sync_info = {
        .vid_pkt_size     = 800,
        .vid_hsa_pixels   = 20,
        .vid_hbp_pixels   = 4,
        .vid_hline_pixels = 1336,
        .vid_vsa_lines    = 4,
        .vid_vbp_lines    = 4,
        .vid_vfp_lines    = 8,
        .vid_active_lines = 1280,
        .edpi_cmd_size    = 0,
    },
    .phy_data_rate = 410,//486
    .pixel_clk = 67807,
};





HI_S32 SAMPLE_COMM_VO_GetWH(VO_INTF_SYNC_E enIntfSync, HI_U32* pu32W, HI_U32* pu32H, HI_U32* pu32Frm)
{
    switch (enIntfSync)
    {
        case VO_OUTPUT_PAL:
            *pu32W = 720;
            *pu32H = 576;
            *pu32Frm = 25;
            break;
        case VO_OUTPUT_NTSC:
            *pu32W = 720;
            *pu32H = 480;
            *pu32Frm = 30;
            break;
        case VO_OUTPUT_1080P24:
            *pu32W = 1920;
            *pu32H = 1080;
            *pu32Frm = 24;
            break;
        case VO_OUTPUT_1080P25:
            *pu32W = 1920;
            *pu32H = 1080;
            *pu32Frm = 25;
            break;
        case VO_OUTPUT_1080P30:
            *pu32W = 1920;
            *pu32H = 1080;
            *pu32Frm = 30;
            break;
        case VO_OUTPUT_720P50:
            *pu32W = 1280;
            *pu32H = 720;
            *pu32Frm = 50;
            break;
        case VO_OUTPUT_720P60:
            *pu32W = 1280;
            *pu32H = 720;
            *pu32Frm = 60;
            break;
        case VO_OUTPUT_1080I50:
            *pu32W = 1920;
            *pu32H = 1080;
            *pu32Frm = 50;
            break;
        case VO_OUTPUT_1080I60:
            *pu32W = 1920;
            *pu32H = 1080;
            *pu32Frm = 60;
            break;
        case VO_OUTPUT_1080P50:
            *pu32W = 1920;
            *pu32H = 1080;
            *pu32Frm = 50;
            break;
        case VO_OUTPUT_1080P60:
            *pu32W = 1920;
            *pu32H = 1080;
            *pu32Frm = 60;
            break;
        case VO_OUTPUT_576P50:
            *pu32W = 720;
            *pu32H = 576;
            *pu32Frm = 50;
            break;
        case VO_OUTPUT_480P60:
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
        case VO_OUTPUT_3840x2160_30:
            *pu32W = 3840;
            *pu32H = 2160;
            *pu32Frm = 30;
            break;
        case VO_OUTPUT_3840x2160_60:
            *pu32W = 3840;
            *pu32H = 2160;
            *pu32Frm = 60;
            break;
        case VO_OUTPUT_320x240_60:
            *pu32W = 320;
            *pu32H = 240;
            *pu32Frm = 60;
            break;
        case VO_OUTPUT_320x240_50:
            *pu32W = 320;
            *pu32H = 240;
            *pu32Frm = 50;
            break;
        case VO_OUTPUT_240x320_50:
            *pu32W = 240;
            *pu32H = 320;
            *pu32Frm = 50;
            break;
        case VO_OUTPUT_240x320_60:
            *pu32W = 240;
            *pu32H = 320;
            *pu32Frm = 60;
            break;
        case VO_OUTPUT_800x600_50:
            *pu32W = 800;
            *pu32H = 600;
            *pu32Frm = 50;
            break;
        case VO_OUTPUT_720x1280_60:
            *pu32W = 720;
            *pu32H = 1280;
            *pu32Frm = 60;
            break;
        case VO_OUTPUT_1080x1920_60:
            *pu32W = 1080;
            *pu32H = 1920;
            *pu32Frm = 60;
            break;
        case VO_OUTPUT_7680x4320_30:
            *pu32W = 7680;
            *pu32H = 4320;
            *pu32Frm = 30;
            break;
        case VO_OUTPUT_USER    : //maohw
            *pu32W = 800;//720;
            *pu32H = 1280;//576;
            *pu32Frm = 60;//25;
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
    
    //maohw
    if(pstPubAttr->enIntfSync == VO_OUTPUT_USER)
    {
      /* Fill user sync info */
      VO_USER_INTFSYNC_INFO_S stUserInfo = {0};

      /* USER SET Div INFOMATION */
      //stUserInfo.bClkReverse = HI_TRUE;
      /* USER SET INTFSYNC ATTR */
      stUserInfo.stUserIntfSyncAttr.enClkSource = VO_CLK_SOURCE_PLL;

      stUserInfo.u32PreDiv = 1;
      stUserInfo.u32DevDiv = 1;
      stUserInfo.stUserIntfSyncAttr.stUserSyncPll.u32Fbdiv = 67;
      stUserInfo.stUserIntfSyncAttr.stUserSyncPll.u32Frac =   13534515;
      stUserInfo.stUserIntfSyncAttr.stUserSyncPll.u32Refdiv = 2;
      stUserInfo.stUserIntfSyncAttr.stUserSyncPll.u32Postdiv1 = 4;
      stUserInfo.stUserIntfSyncAttr.stUserSyncPll.u32Postdiv2 = 3;
   
      /* Set user interface sync info */
      s32Ret = HI_MPI_VO_SetUserIntfSyncInfo(VoDev, &stUserInfo);
      if (s32Ret != HI_SUCCESS)
      {
        printf("Set user interface sync info failed with %x.\n",s32Ret);
        return HI_FAILURE;
      }
     
      s32Ret = HI_MPI_VO_SetDevFrameRate(VoDev, 60);
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
    printf("%s => HI_MPI_VO_EnableVideoLayer\n", __func__);
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
    printf("%s => HI_MPI_VO_DisableVideoLayer\n", __func__);
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
            /* maohw enMode == VO_MODE_2MUX  ||*/
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
        else if(enMode == VO_MODE_2MUX) //maohw
        {
          VO_DEV VoDev = VoLayer;
          VO_PUB_ATTR_S stPubAttr;
          HI_MPI_VO_GetPubAttr(VoDev, &stPubAttr);
          if(stPubAttr.enIntfType & VO_INTF_MIPI)
          {
            stChnAttr.stRect.s32X       = ALIGN_DOWN(0, 2);
            stChnAttr.stRect.s32Y       = ALIGN_DOWN((u32Height / u32Square) * (i % u32Square), 2);
            stChnAttr.stRect.u32Width   = ALIGN_DOWN(u32Width / 1, 2);
            stChnAttr.stRect.u32Height  = ALIGN_DOWN(u32Height / u32Square, 2);
            stChnAttr.u32Priority       = 0;
            stChnAttr.bDeflicker        = HI_FALSE;
          }
          else
          {
            stChnAttr.stRect.s32X       = ALIGN_DOWN((u32Width / u32Square) * (i % u32Square), 2);
            stChnAttr.stRect.s32Y       = ALIGN_DOWN(u32Height / 4, 2);
            stChnAttr.stRect.u32Width   = ALIGN_DOWN(u32Width / u32Square, 2);
            stChnAttr.stRect.u32Height  = ALIGN_DOWN(u32Height / u32Square, 2);
            stChnAttr.u32Priority       = 0;
            stChnAttr.bDeflicker        = HI_FALSE; 
            
            if(0)//if(i == 0)
            {
              stChnAttr.stRect.s32X       = 0;
              stChnAttr.stRect.s32Y       = 0;
              stChnAttr.stRect.u32Width   = ALIGN_DOWN(u32Width, 2);
              stChnAttr.stRect.u32Height  = ALIGN_DOWN(u32Height, 2);
            }
          }
          
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

#ifdef CONFIG_HI_HDMI_SUPPORT
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
    
    // for audio;
    stAttr.bEnableAudio = HI_TRUE;        /**< if enable audio */
    stAttr.enSoundIntf = HI_HDMI_SND_INTERFACE_I2S; /**< source of HDMI audio, HI_HDMI_SND_INTERFACE_I2S suggested.the parameter must be consistent with the input of AO*/
    stAttr.enSampleRate = AUDIO_SAMPLE_RATE_48000;        /**< sampling rate of PCM audio,the parameter must be consistent with the input of AO */
    stAttr.u8DownSampleParm = HI_FALSE;    /**< parameter of downsampling  rate of PCM audio, default :0 */

    stAttr.enBitDepth = 8 * (AUDIO_BIT_WIDTH_16+1);   /**< bitwidth of audio,default :16,the parameter must be consistent with the config of AO */
    stAttr.u8I2SCtlVbit = 0;        /**< reserved, should be 0, I2S control (0x7A:0x1D) */

    stAttr.bEnableAviInfoFrame = HI_TRUE; /**< if enable  AVI InfoFrame*/
    stAttr.bEnableAudInfoFrame = HI_TRUE;; /**< if enable AUDIO InfoFrame*/
    

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
    
    
    // for audio;
    stAttr.bEnableAudio = HI_TRUE;        /**< if enable audio */
    stAttr.enSoundIntf = HI_HDMI_SND_INTERFACE_I2S; /**< source of HDMI audio, HI_HDMI_SND_INTERFACE_I2S suggested.the parameter must be consistent with the input of AO*/
    stAttr.enSampleRate = AUDIO_SAMPLE_RATE_48000;        /**< sampling rate of PCM audio,the parameter must be consistent with the input of AO */
    stAttr.u8DownSampleParm = HI_FALSE;    /**< parameter of downsampling  rate of PCM audio, default :0 */

    stAttr.enBitDepth = 8 * (AUDIO_BIT_WIDTH_16+1);   /**< bitwidth of audio,default :16,the parameter must be consistent with the config of AO */
    stAttr.u8I2SCtlVbit = 0;        /**< reserved, should be 0, I2S control (0x7A:0x1D) */

    stAttr.bEnableAviInfoFrame = HI_TRUE; /**< if enable  AVI InfoFrame*/
    stAttr.bEnableAudInfoFrame = HI_TRUE;; /**< if enable AUDIO InfoFrame*/
    

    CHECK_RET(HI_MPI_HDMI_SetAttr(enHdmiId, &stAttr), "HI_MPI_HDMI_SetAttr");
    CHECK_RET(HI_MPI_HDMI_Start(enHdmiId), "HI_MPI_HDMI_Start");

    return HI_SUCCESS;
}

HI_S32 SAMPLE_COMM_VO_HdmiStop(HI_VOID)
{
    HI_HDMI_ID_E enHdmiId = HI_HDMI_ID_0;

    HI_MPI_HDMI_Stop(enHdmiId);
    HI_MPI_HDMI_Close(enHdmiId);
    HI_MPI_HDMI_DeInit();

    return HI_SUCCESS;
}
#endif

void SAMPLE_COMM_VO_StartMipiTx(VO_INTF_SYNC_E enVoIntfSync);

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

    pstVoConfig->VoDev             = SAMPLE_VO_DEV_UHD;

    HI_MPI_SYS_GetChipId(&u32ChipId);

    if(HI3516C_V500 == u32ChipId)
    {
        pstVoConfig->enVoIntfType = VO_INTF_BT1120;
    }
    else
    {
        #ifdef CONFIG_HI_HDMI_SUPPORT
        pstVoConfig->enVoIntfType = VO_INTF_HDMI;
        pstVoConfig->enIntfSync   = VO_OUTPUT_1080P30;
        #else
        pstVoConfig->enVoIntfType = VO_INTF_LCD_6BIT;
        pstVoConfig->enIntfSync   = VO_OUTPUT_320x240_50;
        #endif
    }
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

//maohw
HI_VOID SAMPLE_VO_GetUserPubBaseAttr(VO_PUB_ATTR_S *pstPubAttr)
{
    pstPubAttr->u32BgColor = COLOR_RGB_BLUE;
    pstPubAttr->enIntfSync = VO_OUTPUT_USER;
    pstPubAttr->stSyncInfo.bSynm = 0;
    pstPubAttr->stSyncInfo.u8Intfb = 0;
    pstPubAttr->stSyncInfo.bIop = 1;

    pstPubAttr->stSyncInfo.u16Hmid = 1;
    pstPubAttr->stSyncInfo.u16Bvact = 1;
    pstPubAttr->stSyncInfo.u16Bvbb = 1;
    pstPubAttr->stSyncInfo.u16Bvfb = 1;

    pstPubAttr->stSyncInfo.u16Hact = 800;
    pstPubAttr->stSyncInfo.u16Hbb = 40;
    pstPubAttr->stSyncInfo.u16Hfb = 32;
    pstPubAttr->stSyncInfo.u16Hpw = 20;
    pstPubAttr->stSyncInfo.u16Vact = 1280;
    pstPubAttr->stSyncInfo.u16Vbb = 8;
    pstPubAttr->stSyncInfo.u16Vfb = 8;
    pstPubAttr->stSyncInfo.u16Vpw = 4;
    
    pstPubAttr->stSyncInfo.bIdv = 0;
    pstPubAttr->stSyncInfo.bIhs = 0;
    pstPubAttr->stSyncInfo.bIvs = 0;

    return;
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
    //maohw
    SAMPLE_VO_GetUserPubBaseAttr(&stVoPubAttr);
	
#ifdef CONFIG_HI_HDMI_SUPPORT
    stVoPubAttr.enIntfType  = enVoIntfType;
    stVoPubAttr.enIntfSync  = enIntfSync;
#else
    stVoPubAttr.enIntfType  = VO_INTF_LCD_6BIT;
    stVoPubAttr.enIntfSync  = VO_OUTPUT_240x320_50;
#endif
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
        memcpy(&stLayerAttr.stDispRect, &pstVoConfig->stDispRect, sizeof(RECT_S));
    }

    /******************************
    //Set image size if changed.
    ********************************/
    if (0 != memcmp(&pstVoConfig->stImageSize, &stDefImageSize, sizeof(SIZE_S)))
    {
        memcpy(&stLayerAttr.stImageSize, &pstVoConfig->stImageSize, sizeof(SIZE_S));
    }
#ifdef CONFIG_HI_HDMI_SUPPORT
    stLayerAttr.stImageSize.u32Width  = stLayerAttr.stDispRect.u32Width;
    stLayerAttr.stImageSize.u32Height = stLayerAttr.stDispRect.u32Height;
#else
    stLayerAttr.stImageSize.u32Width  = stLayerAttr.stDispRect.u32Width = 240;
    stLayerAttr.stImageSize.u32Height = stLayerAttr.stDispRect.u32Height = 320;
#endif
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
        printf("%s => SAMPLE_COMM_VO_StartLayer VO_INTF_MIPI\n", __func__);
    }
    else if (VO_INTF_MIPI == enVoIntfType)
    {
      VO_CSC_S stGhCSC = {0};
      int s32Ret = HI_MPI_VO_GetGraphicLayerCSC(0, &stGhCSC);
      if (HI_SUCCESS != s32Ret)
      {
          SAMPLE_PRT("HI_MPI_VO_GetGraphicLayerCSC failed s32Ret:0x%x!\n", s32Ret);
      }
      stGhCSC.enCscMatrix = VO_CSC_MATRIX_IDENTITY;
      s32Ret = HI_MPI_VO_SetGraphicLayerCSC(0, &stGhCSC);
      if (HI_SUCCESS != s32Ret)
      {
          SAMPLE_PRT("HI_MPI_VO_SetGraphicLayerCSC failed s32Ret:0x%x!\n", s32Ret);
      }
      printf("%s => SAMPLE_COMM_VO_StartLayer VO_INTF_MIPI\n", __func__);
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
#ifdef CONFIG_HI_HDMI_SUPPORT
    if(VO_INTF_HDMI & enVoIntfType)
    {
        SAMPLE_COMM_VO_HdmiStartByDyRg(enIntfSync, enDstDyRg);
    }
#endif

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

#ifdef CONFIG_HI_HDMI_SUPPORT
    if(VO_INTF_HDMI & pstVoConfig->enVoIntfType)
    {
        SAMPLE_COMM_VO_HdmiStop();
    }
#endif

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

    // 0x1117006c 0x23;
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

    // 0x11170070 0x018712ff;
    // 0x1117006c 0x00000429;
    cmd[0] = 0xff;
    cmd[1] = 0x12;
    cmd[2] = 0x87;
    cmd[3] = 0x01;

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

// maohw .beg
static void DSI_Single(HI_S32 fd, int addr ,int val)
{
    HI_S32     s32Ret;
    cmd_info_t cmd_info;

    cmd_info.devno     = 0;
    cmd_info.cmd_size  = val<<8 | addr;
    cmd_info.data_type = 0x15;
    //cmd_info.data_type = 0x23;
    cmd_info.cmd       = NULL;
	//printf("===> fd = %d , cmd = 0x%4x === \n",fd ,cmd_info.cmd_size);

    s32Ret = ioctl(fd, HI_MIPI_TX_SET_CMD, &cmd_info);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("MIPI_TX SET CMD failed\n");
        close(fd);
        return;
    }
    usleep(1000);

}

static void DSI_Cmd(HI_S32 fd, int val)
{
    HI_S32     s32Ret;
    cmd_info_t cmd_info;

    cmd_info.devno     = 0;
    cmd_info.cmd_size  = val;
    cmd_info.data_type = 0x05;
    cmd_info.cmd       = NULL;

    s32Ret = ioctl(fd, HI_MIPI_TX_SET_CMD, &cmd_info);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("MIPI_TX SET CMD failed\n");
        close(fd);
        return;
    }

    usleep(1000);

}

static void DSI_Multi3(HI_S32 fd, int addr , char val0, char val1, char val2)
{
  HI_S32     s32Ret;
  cmd_info_t cmd_info;
    
  unsigned char cmd[16];
  cmd[0] = addr;
  cmd[1] = val0;
  cmd[2] = val1;
  cmd[3] = val2;

  cmd_info.devno = 0;
  cmd_info.cmd_size = 1+3;
  cmd_info.data_type = 0x39;
  cmd_info.cmd = cmd;
  s32Ret = ioctl(fd, HI_MIPI_TX_SET_CMD, &cmd_info);
  if (HI_SUCCESS != s32Ret)
  {
      SAMPLE_PRT("MIPI_TX SET CMD failed\n");
      close(fd);
      return;
  }
  return;
}

static int __SAMPLE_PRIVATE_VO_InitScreen800x1280(HI_S32 fd)
{
  printf("=======> init LCD screen fd:%d <========= \n\n", fd);

  //Page0
  DSI_Single(fd,0xE0,0x00);

  //--- PASSWORD  ----//
  DSI_Single(fd,0xE1,0x93);
  DSI_Single(fd,0xE2,0x65);
  DSI_Single(fd,0xE3,0xF8);


  //Page0
  DSI_Single(fd,0xE0,0x00);
  //--- Sequence Ctrl  ----//
  DSI_Single(fd,0x70,0x10);	//DC0,DC1
  DSI_Single(fd,0x71,0x13);	//DC2,DC3
  DSI_Single(fd,0x72,0x06);	//DC7
  DSI_Single(fd,0x80,0x03);	//0x03:4-Lane£»0x02:3-Lane
  //--- Page4  ----//
  DSI_Single(fd,0xE0,0x04);
  DSI_Single(fd,0x2D,0x03);
  //--- Page1  ----//
  DSI_Single(fd,0xE0,0x01);

  //Set VCOM
  DSI_Single(fd,0x00,0x00);
  DSI_Single(fd,0x01,0xA0);
  //Set VCOM_Reverse
  DSI_Single(fd,0x03,0x00);
  DSI_Single(fd,0x04,0xA0);

  //Set Gamma Power, VGMP,VGMN,VGSP,VGSN
  DSI_Single(fd,0x17,0x00);
  DSI_Single(fd,0x18,0xB1);
  DSI_Single(fd,0x19,0x01);
  DSI_Single(fd,0x1A,0x00);
  DSI_Single(fd,0x1B,0xB1);  //VGMN=0
  DSI_Single(fd,0x1C,0x01);
                  
  //Set Gate Power
  DSI_Single(fd,0x1F,0x3E);     //VGH_R  = 15V                       
  DSI_Single(fd,0x20,0x2D);     //VGL_R  = -12V                      
  DSI_Single(fd,0x21,0x2D);     //VGL_R2 = -12V                      
  DSI_Single(fd,0x22,0x0E);     //PA[6]=0, PA[5]=0, PA[4]=0, PA[0]=0 

  //SETPANEL
  DSI_Single(fd,0x37,0x19);	//SS=1,BGR=1

  //SET RGBCYC
  DSI_Single(fd,0x38,0x05);	//JDT=101 zigzag inversion
  DSI_Single(fd,0x39,0x08);	//RGB_N_EQ1, modify 20140806
  DSI_Single(fd,0x3A,0x12);	//RGB_N_EQ2, modify 20140806
  DSI_Single(fd,0x3C,0x78);	//SET EQ3 for TE_H
  DSI_Single(fd,0x3E,0x80);	//SET CHGEN_OFF, modify 20140806 
  DSI_Single(fd,0x3F,0x80);	//SET CHGEN_OFF2, modify 20140806


  //Set TCON
  DSI_Single(fd,0x40,0x06);	//RSO=800 RGB
  DSI_Single(fd,0x41,0xA0);	//LN=640->1280 line

  //--- power voltage  ----//
  DSI_Single(fd,0x55,0x01);	//DCDCM=0001, JD PWR_IC
  DSI_Single(fd,0x56,0x01);
  DSI_Single(fd,0x57,0x69);
  DSI_Single(fd,0x58,0x0A);
  DSI_Single(fd,0x59,0x0A);	//VCL = -2.9V
  DSI_Single(fd,0x5A,0x28);	//VGH = 19V
  DSI_Single(fd,0x5B,0x19);	//VGL = -11V



  //--- Gamma  ----//
  DSI_Single(fd,0x5D,0x7C);              
  DSI_Single(fd,0x5E,0x65);      
  DSI_Single(fd,0x5F,0x53);    
  DSI_Single(fd,0x60,0x48);    
  DSI_Single(fd,0x61,0x43);    
  DSI_Single(fd,0x62,0x35);    
  DSI_Single(fd,0x63,0x39);    
  DSI_Single(fd,0x64,0x23);    
  DSI_Single(fd,0x65,0x3D);    
  DSI_Single(fd,0x66,0x3C);    
  DSI_Single(fd,0x67,0x3D);    
  DSI_Single(fd,0x68,0x5A);    
  DSI_Single(fd,0x69,0x46);    
  DSI_Single(fd,0x6A,0x57);    
  DSI_Single(fd,0x6B,0x4B);    
  DSI_Single(fd,0x6C,0x49);    
  DSI_Single(fd,0x6D,0x2F);    
  DSI_Single(fd,0x6E,0x03);    
  DSI_Single(fd,0x6F,0x00);    
  DSI_Single(fd,0x70,0x7C);    
  DSI_Single(fd,0x71,0x65);    
  DSI_Single(fd,0x72,0x53);    
  DSI_Single(fd,0x73,0x48);    
  DSI_Single(fd,0x74,0x43);    
  DSI_Single(fd,0x75,0x35);    
  DSI_Single(fd,0x76,0x39);    
  DSI_Single(fd,0x77,0x23);    
  DSI_Single(fd,0x78,0x3D);    
  DSI_Single(fd,0x79,0x3C);    
  DSI_Single(fd,0x7A,0x3D);    
  DSI_Single(fd,0x7B,0x5A);    
  DSI_Single(fd,0x7C,0x46);    
  DSI_Single(fd,0x7D,0x57);    
  DSI_Single(fd,0x7E,0x4B);    
  DSI_Single(fd,0x7F,0x49);    
  DSI_Single(fd,0x80,0x2F);    
  DSI_Single(fd,0x81,0x03);    
  DSI_Single(fd,0x82,0x00);    


  //Page2, for GIP
  DSI_Single(fd,0xE0,0x02);

  //GIP_L Pin mapping
  DSI_Single(fd,0x00,0x47);
  DSI_Single(fd,0x01,0x47);  
  DSI_Single(fd,0x02,0x45);
  DSI_Single(fd,0x03,0x45);
  DSI_Single(fd,0x04,0x4B);
  DSI_Single(fd,0x05,0x4B);
  DSI_Single(fd,0x06,0x49);
  DSI_Single(fd,0x07,0x49);
  DSI_Single(fd,0x08,0x41);
  DSI_Single(fd,0x09,0x1F);
  DSI_Single(fd,0x0A,0x1F);
  DSI_Single(fd,0x0B,0x1F);
  DSI_Single(fd,0x0C,0x1F);
  DSI_Single(fd,0x0D,0x1F);
  DSI_Single(fd,0x0E,0x1F);
  DSI_Single(fd,0x0F,0x43);
  DSI_Single(fd,0x10,0x1F);
  DSI_Single(fd,0x11,0x1F);
  DSI_Single(fd,0x12,0x1F);
  DSI_Single(fd,0x13,0x1F);
  DSI_Single(fd,0x14,0x1F);
  DSI_Single(fd,0x15,0x1F);

  //GIP_R Pin mapping
  DSI_Single(fd,0x16,0x46);
  DSI_Single(fd,0x17,0x46);
  DSI_Single(fd,0x18,0x44);
  DSI_Single(fd,0x19,0x44);
  DSI_Single(fd,0x1A,0x4A);
  DSI_Single(fd,0x1B,0x4A);
  DSI_Single(fd,0x1C,0x48);
  DSI_Single(fd,0x1D,0x48);
  DSI_Single(fd,0x1E,0x40);
  DSI_Single(fd,0x1F,0x1F);
  DSI_Single(fd,0x20,0x1F);
  DSI_Single(fd,0x21,0x1F);
  DSI_Single(fd,0x22,0x1F);
  DSI_Single(fd,0x23,0x1F);
  DSI_Single(fd,0x24,0x1F);
  DSI_Single(fd,0x25,0x42);
  DSI_Single(fd,0x26,0x1F);
  DSI_Single(fd,0x27,0x1F);
  DSI_Single(fd,0x28,0x1F);
  DSI_Single(fd,0x29,0x1F);
  DSI_Single(fd,0x2A,0x1F);
  DSI_Single(fd,0x2B,0x1F);
                        
  //GIP_L_GS Pin mapping
  DSI_Single(fd,0x2C,0x11);
  DSI_Single(fd,0x2D,0x0F);   
  DSI_Single(fd,0x2E,0x0D); 
  DSI_Single(fd,0x2F,0x0B); 
  DSI_Single(fd,0x30,0x09); 
  DSI_Single(fd,0x31,0x07); 
  DSI_Single(fd,0x32,0x05); 
  DSI_Single(fd,0x33,0x18); 
  DSI_Single(fd,0x34,0x17); 
  DSI_Single(fd,0x35,0x1F); 
  DSI_Single(fd,0x36,0x01); 
  DSI_Single(fd,0x37,0x1F); 
  DSI_Single(fd,0x38,0x1F); 
  DSI_Single(fd,0x39,0x1F); 
  DSI_Single(fd,0x3A,0x1F); 
  DSI_Single(fd,0x3B,0x1F); 
  DSI_Single(fd,0x3C,0x1F); 
  DSI_Single(fd,0x3D,0x1F); 
  DSI_Single(fd,0x3E,0x1F); 
  DSI_Single(fd,0x3F,0x13); 
  DSI_Single(fd,0x40,0x1F); 
  DSI_Single(fd,0x41,0x1F);
   
  //GIP_R_GS Pin mapping
  DSI_Single(fd,0x42,0x10);
  DSI_Single(fd,0x43,0x0E);   
  DSI_Single(fd,0x44,0x0C); 
  DSI_Single(fd,0x45,0x0A); 
  DSI_Single(fd,0x46,0x08); 
  DSI_Single(fd,0x47,0x06); 
  DSI_Single(fd,0x48,0x04); 
  DSI_Single(fd,0x49,0x18); 
  DSI_Single(fd,0x4A,0x17); 
  DSI_Single(fd,0x4B,0x1F); 
  DSI_Single(fd,0x4C,0x00); 
  DSI_Single(fd,0x4D,0x1F); 
  DSI_Single(fd,0x4E,0x1F); 
  DSI_Single(fd,0x4F,0x1F); 
  DSI_Single(fd,0x50,0x1F); 
  DSI_Single(fd,0x51,0x1F); 
  DSI_Single(fd,0x52,0x1F); 
  DSI_Single(fd,0x53,0x1F); 
  DSI_Single(fd,0x54,0x1F); 
  DSI_Single(fd,0x55,0x12); 
  DSI_Single(fd,0x56,0x1F); 
  DSI_Single(fd,0x57,0x1F); 

  //GIP Timing  
  DSI_Single(fd,0x58,0x40);
  DSI_Single(fd,0x59,0x00);
  DSI_Single(fd,0x5A,0x00);
  DSI_Single(fd,0x5B,0x30);
  DSI_Single(fd,0x5C,0x03);
  DSI_Single(fd,0x5D,0x30);
  DSI_Single(fd,0x5E,0x01);
  DSI_Single(fd,0x5F,0x02);
  DSI_Single(fd,0x60,0x00);
  DSI_Single(fd,0x61,0x01);
  DSI_Single(fd,0x62,0x02);
  DSI_Single(fd,0x63,0x03);
  DSI_Single(fd,0x64,0x6B);
  DSI_Single(fd,0x65,0x00);
  DSI_Single(fd,0x66,0x00);
  DSI_Single(fd,0x67,0x73);
  DSI_Single(fd,0x68,0x05);
  DSI_Single(fd,0x69,0x06);
  DSI_Single(fd,0x6A,0x6B);
  DSI_Single(fd,0x6B,0x08);
  DSI_Single(fd,0x6C,0x00);
  DSI_Single(fd,0x6D,0x04);
  DSI_Single(fd,0x6E,0x04);
  DSI_Single(fd,0x6F,0x88);
  DSI_Single(fd,0x70,0x00);
  DSI_Single(fd,0x71,0x00);
  DSI_Single(fd,0x72,0x06);
  DSI_Single(fd,0x73,0x7B);
  DSI_Single(fd,0x74,0x00);
  DSI_Single(fd,0x75,0x07);
  DSI_Single(fd,0x76,0x00);
  DSI_Single(fd,0x77,0x5D);
  DSI_Single(fd,0x78,0x17);
  DSI_Single(fd,0x79,0x1F);
  DSI_Single(fd,0x7A,0x00);
  DSI_Single(fd,0x7B,0x00);
  DSI_Single(fd,0x7C,0x00);
  DSI_Single(fd,0x7D,0x03);
  DSI_Single(fd,0x7E,0x7B);


  //Page1
  DSI_Single(fd,0xE0,0x01);
  DSI_Single(fd,0x0E,0x01);	//LEDON output VCSW2


  //Page3
  DSI_Single(fd,0xE0,0x03);
  DSI_Single(fd,0x98,0x2F);	//From 2E to 2F, LED_VOL

  //Page4
  DSI_Single(fd,0xE0,0x04);
  DSI_Single(fd,0x09,0x10);
  DSI_Single(fd,0x2B,0x2B);
  DSI_Single(fd,0x2E,0x44);

  //Page0
  DSI_Single(fd,0xE0,0x00);
  DSI_Single(fd,0xE6,0x02);
  DSI_Single(fd,0xE7,0x02);

  //SLP OUT
  DSI_Cmd(fd,0x11);  	// SLPOUT
  usleep(120000);


  //DISP ON

  DSI_Cmd(fd,0x29);  	// DSPON
  usleep(5000);

  //--- TE----//
  DSI_Single(fd,0x35,0x00);
}

static int SAMPLE_PRIVATE_VO_InitScreen800x1280(HI_S32 fd)
{
  printf("=======> LCDD (Peripheral) Setting fd:%d <========= \n\n", fd);

  DSI_Multi3(fd,0xFF,0x98,0x81,0x03);
  //GIP_1
  DSI_Single(fd,0x01,0x00);
  DSI_Single(fd,0x02,0x00);
  DSI_Single(fd,0x03,0x53);
  DSI_Single(fd,0x04,0x53);
  DSI_Single(fd,0x05,0x13);
  DSI_Single(fd,0x06,0x04);
  DSI_Single(fd,0x07,0x02);
  DSI_Single(fd,0x08,0x02);
  DSI_Single(fd,0x09,0x00);
  DSI_Single(fd,0x0a,0x00);
  DSI_Single(fd,0x0b,0x00);
  DSI_Single(fd,0x0c,0x00);
  DSI_Single(fd,0x0d,0x00);
  DSI_Single(fd,0x0e,0x00);
  DSI_Single(fd,0x0f,0x00);
  DSI_Single(fd,0x10,0x00);
  DSI_Single(fd,0x11,0x00);
  DSI_Single(fd,0x12,0x00);
  DSI_Single(fd,0x13,0x00);
  DSI_Single(fd,0x14,0x00);
  DSI_Single(fd,0x15,0x00);
  DSI_Single(fd,0x16,0x00);
  DSI_Single(fd,0x17,0x00);
  DSI_Single(fd,0x18,0x00);
  DSI_Single(fd,0x19,0x00);
  DSI_Single(fd,0x1a,0x00);
  DSI_Single(fd,0x1b,0x00);
  DSI_Single(fd,0x1c,0x00);
  DSI_Single(fd,0x1d,0x00);
  DSI_Single(fd,0x1e,0xc0);
  DSI_Single(fd,0x1f,0x80);
  DSI_Single(fd,0x20,0x02);
  DSI_Single(fd,0x21,0x09);
  DSI_Single(fd,0x22,0x00);
  DSI_Single(fd,0x23,0x00);
  DSI_Single(fd,0x24,0x00);
  DSI_Single(fd,0x25,0x00);
  DSI_Single(fd,0x26,0x00);
  DSI_Single(fd,0x27,0x00);
  DSI_Single(fd,0x28,0x55);
  DSI_Single(fd,0x29,0x03);
  DSI_Single(fd,0x2a,0x00);
  DSI_Single(fd,0x2b,0x00);
  DSI_Single(fd,0x2c,0x00);
  DSI_Single(fd,0x2d,0x00);
  DSI_Single(fd,0x2e,0x00);
  DSI_Single(fd,0x2f,0x00);
  DSI_Single(fd,0x30,0x00);
  DSI_Single(fd,0x31,0x00);
  DSI_Single(fd,0x32,0x00);
  DSI_Single(fd,0x33,0x00);
  DSI_Single(fd,0x34,0x00);
  DSI_Single(fd,0x35,0x00);
  DSI_Single(fd,0x36,0x00);
  DSI_Single(fd,0x37,0x00);
  DSI_Single(fd,0x38,0x3C);	//VDD1&2 toggle 1sec
  DSI_Single(fd,0x39,0x00);
  DSI_Single(fd,0x3a,0x00);
  DSI_Single(fd,0x3b,0x00);
  DSI_Single(fd,0x3c,0x00);
  DSI_Single(fd,0x3d,0x00);
  DSI_Single(fd,0x3e,0x00);
  DSI_Single(fd,0x3f,0x00);
  DSI_Single(fd,0x40,0x00);
  DSI_Single(fd,0x41,0x00);
  DSI_Single(fd,0x42,0x00);
  DSI_Single(fd,0x43,0x00);
  DSI_Single(fd,0x44,0x00);

  //GIP_2
  DSI_Single(fd,0x50,0x01);
  DSI_Single(fd,0x51,0x23);
  DSI_Single(fd,0x52,0x45);
  DSI_Single(fd,0x53,0x67);
  DSI_Single(fd,0x54,0x89);
  DSI_Single(fd,0x55,0xab);
  DSI_Single(fd,0x56,0x01);
  DSI_Single(fd,0x57,0x23);
  DSI_Single(fd,0x58,0x45);
  DSI_Single(fd,0x59,0x67);
  DSI_Single(fd,0x5a,0x89);
  DSI_Single(fd,0x5b,0xab);
  DSI_Single(fd,0x5c,0xcd);
  DSI_Single(fd,0x5d,0xef);

  //GIP_3
  DSI_Single(fd,0x5e,0x01);
  DSI_Single(fd,0x5f,0x08);       //FW_GOUT_L1   STV2_ODD
  DSI_Single(fd,0x60,0x02);       //FW_GOUT_L2   
  DSI_Single(fd,0x61,0x02);       //FW_GOUT_L3
  DSI_Single(fd,0x62,0x0A);       //FW_GOUT_L4   RESET_ODD
  DSI_Single(fd,0x63,0x15);       //FW_GOUT_L5   
  DSI_Single(fd,0x64,0x14);       //FW_GOUT_L6   
  DSI_Single(fd,0x65,0x02);       //FW_GOUT_L7   
  DSI_Single(fd,0x66,0x11);       //FW_GOUT_L8    CK11
  DSI_Single(fd,0x67,0x10);       //FW_GOUT_L9    CK9
  DSI_Single(fd,0x68,0x02);       //FW_GOUT_L10  
  DSI_Single(fd,0x69,0x0F);       //FW_GOUT_L11   CK7
  DSI_Single(fd,0x6a,0x0E);       //FW_GOUT_L12   CK5
  DSI_Single(fd,0x6b,0x02);       //FW_GOUT_L13
  DSI_Single(fd,0x6c,0x0D);       //FW_GOUT_L14   CK3
  DSI_Single(fd,0x6d,0x0C);       //FW_GOUT_L15   CK1
  DSI_Single(fd,0x6e,0x06);       //FW_GOUT_L16   STV1_ODD
  DSI_Single(fd,0x6f,0x02);       //FW_GOUT_L17
  DSI_Single(fd,0x70,0x02);       //FW_GOUT_L18
  DSI_Single(fd,0x71,0x02);       //FW_GOUT_L19
  DSI_Single(fd,0x72,0x02);       //FW_GOUT_L20
  DSI_Single(fd,0x73,0x02);       //FW_GOUT_L21  
  DSI_Single(fd,0x74,0x02);       //FW_GOUT_L22  

  DSI_Single(fd,0x75,0x06);       //BW_GOUT_L1   STV2_ODD
  DSI_Single(fd,0x76,0x02);       //BW_GOUT_L2   
  DSI_Single(fd,0x77,0x02);       //BW_GOUT_L3   
  DSI_Single(fd,0x78,0x0A);       //BW_GOUT_L4   RESET_ODD
  DSI_Single(fd,0x79,0x15);       //BW_GOUT_L5   
  DSI_Single(fd,0x7a,0x14);       //BW_GOUT_L6   
  DSI_Single(fd,0x7b,0x02);       //BW_GOUT_L7   
  DSI_Single(fd,0x7c,0x10);       //BW_GOUT_L8    CK11
  DSI_Single(fd,0x7d,0x11);       //BW_GOUT_L9    CK9
  DSI_Single(fd,0x7e,0x02);       //BW_GOUT_L10  
  DSI_Single(fd,0x7f,0x0C);       //BW_GOUT_L11   CK7
  DSI_Single(fd,0x80,0x0D);       //BW_GOUT_L12   CK5
  DSI_Single(fd,0x81,0x02);       //BW_GOUT_L13
  DSI_Single(fd,0x82,0x0E);       //BW_GOUT_L14   CK3
  DSI_Single(fd,0x83,0x0F);       //BW_GOUT_L15   CK1
  DSI_Single(fd,0x84,0x08);       //BW_GOUT_L16   STV1_ODD
  DSI_Single(fd,0x85,0x02);       //BW_GOUT_L17
  DSI_Single(fd,0x86,0x02);       //BW_GOUT_L18
  DSI_Single(fd,0x87,0x02);       //BW_GOUT_L19
  DSI_Single(fd,0x88,0x02);       //BW_GOUT_L20
  DSI_Single(fd,0x89,0x02);       //BW_GOUT_L21  
  DSI_Single(fd,0x8A,0x02);       //BW_GOUT_L22  

  //CMD_Page 4
  DSI_Multi3(fd,0xFF,0x98,0x81,0x04);
  DSI_Single(fd,0x6C,0x15);
  DSI_Single(fd,0x6E,0x30);  //VGH clamp 16.08V
  DSI_Single(fd,0x6F,0x37);  // reg vcl + pumping ratio VGH=3x VGL=-3x
  DSI_Single(fd,0x8D,0x1F);  //VGL clamp -12.03V
  DSI_Single(fd,0x87,0xBA);
  DSI_Single(fd,0x26,0x76);
  DSI_Single(fd,0xB2,0xD1);
  DSI_Single(fd,0xB5,0x07);
  DSI_Single(fd,0x35,0x17);
  DSI_Single(fd,0x33,0x14);
  DSI_Single(fd,0x31,0x75);
  DSI_Single(fd,0x3A,0x85);
  DSI_Single(fd,0x3B,0x98);
  DSI_Single(fd,0x38,0x01);
  DSI_Single(fd,0x39,0x00);
  DSI_Single(fd,0x7A,0x10);  //VREG1/2 out

  //CMD_Page 1
  DSI_Multi3(fd,0xFF,0x98,0x81,0x01);
  DSI_Single(fd,0x40,0x53);
  DSI_Single(fd,0x22,0x0A); //***
  DSI_Single(fd,0x31,0x00);              //column inversion
  DSI_Single(fd,0x50,0xE9);		//VREG1OUT=5.508V
  DSI_Single(fd,0x51,0xE4);		//VREG2OUT=-5.508V
  DSI_Single(fd,0x53,0x48);               //VCOM1
  DSI_Single(fd,0x55,0x48);               //VCOM2
  DSI_Single(fd,0x60,0x28);		//SDT
  DSI_Single(fd,0x2E,0xC8);               //1280 GATE NL SEL 
  DSI_Single(fd,0x34,0x01);

  DSI_Single(fd,0xA0,0x00);		//VP255	Gamma P
  DSI_Single(fd,0xA1,0x13);               //VP251        
  DSI_Single(fd,0xA2,0x24);               //VP247        
  DSI_Single(fd,0xA3,0x15);               //VP243        
  DSI_Single(fd,0xA4,0x18);               //VP239        
  DSI_Single(fd,0xA5,0x2F);               //VP231        
  DSI_Single(fd,0xA6,0x22);               //VP219        
  DSI_Single(fd,0xA7,0x21);               //VP203        
  DSI_Single(fd,0xA8,0x89);               //VP175        
  DSI_Single(fd,0xA9,0x1B);               //VP144        
  DSI_Single(fd,0xAA,0x27);               //VP111        
  DSI_Single(fd,0xAB,0x7D);               //VP80         
  DSI_Single(fd,0xAC,0x1B);               //VP52         
  DSI_Single(fd,0xAD,0x1B);               //VP36         
  DSI_Single(fd,0xAE,0x50);               //VP24         
  DSI_Single(fd,0xAF,0x24);               //VP16         
  DSI_Single(fd,0xB0,0x2A);               //VP12       
  DSI_Single(fd,0xB1,0x51);              //VP8          
  DSI_Single(fd,0xB2,0x5A);               //VP4          
  DSI_Single(fd,0xB3,0x1B);               //VP0          
                                                 
  DSI_Single(fd,0xC0,0x00);		//VN255 GAMMA N
  DSI_Single(fd,0xC1,0x16);               //VN251        
  DSI_Single(fd,0xC2,0x24);               //VN247        
  DSI_Single(fd,0xC3,0x13);               //VN243        
  DSI_Single(fd,0xC4,0x18);               //VN239        
  DSI_Single(fd,0xC5,0x2A);               //VN231        
  DSI_Single(fd,0xC6,0x20);               //VN219        
  DSI_Single(fd,0xC7,0x22);               //VN203        
  DSI_Single(fd,0xC8,0x80);               //VN175        
  DSI_Single(fd,0xC9,0x1C);               //VN144        
  DSI_Single(fd,0xCA,0x28);               //VN111        
  DSI_Single(fd,0xCB,0x71);               //VN80         
  DSI_Single(fd,0xCC,0x1D);               //VN52         
  DSI_Single(fd,0xCD,0x1B);               //VN36         
  DSI_Single(fd,0xCE,0x51);               //VN24        
  DSI_Single(fd,0xCF,0x24);               //VN16         
  DSI_Single(fd,0xD0,0x2A);               //VN12        
  DSI_Single(fd,0xD1,0x4C);               //VN8          
  DSI_Single(fd,0xD2,0x5A);               //VN4          
  DSI_Single(fd,0xD3,0x32);               //VN0


  DSI_Multi3(fd,0xFF,0x98,0x81,0x02); 
  DSI_Single(fd,0x04,0x17); 
  DSI_Single(fd,0x05,0x12); 
  DSI_Single(fd,0x06,0x40); 
  DSI_Single(fd,0x07,0x0B); 

  DSI_Multi3(fd,0xFF,0x98,0x81,0x00);
  //////DSI_Single(fd,0x36,0x08); //*** RGB->BRG
  DSI_Single(fd,0x35,0x00); 
  DSI_Cmd(fd,0x11);	  //sleep out
  usleep(120*1000);
  DSI_Cmd(fd,0x29);	  //display on
  usleep(20*1000);
}
// maohw .end


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
        //maohw
        SAMPLE_PRT("%s,%d,Init 800x1280 screen.\n",__FUNCTION__,__LINE__);
        // init screen for 1080x1920_60.
        //SAMPLE_PRIVATE_VO_InitScreen1080x1920(fd);
        //SAMPLE_PRT("%s,%d,There is no screen to init\n",__FUNCTION__,__LINE__);
        SAMPLE_PRIVATE_VO_InitScreen800x1280(fd);
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
            pstMipiTxConfig = &MIPI_TX_800X1280_60_CONFIG; //maohw
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
