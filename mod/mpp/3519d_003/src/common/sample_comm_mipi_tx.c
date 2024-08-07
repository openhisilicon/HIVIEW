/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */

#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>
#include "sample_comm.h"
#include "hi_mipi_tx.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* end of #ifdef __cplusplus */

#if VO_MIPI_SUPPORT

#ifdef OT_FPGA
#define SAMPLE_COMM_MIPI_TX_MAX_PHY_DATA_RATE 594
#else
#define SAMPLE_COMM_MIPI_TX_MAX_PHY_DATA_RATE 945
#endif

static hi_s32 g_sample_comm_mipi_fd = HI_INVALID_VALUE;

static const combo_dev_cfg_t g_sample_comm_mipi_tx_720x576_50_config = {
    .devno = 0,
    .lane_id = {0, 1, 2, 3},
    .out_mode = OUT_MODE_DSI_VIDEO,
    .out_format = OUT_FORMAT_RGB_24BIT,
    .video_mode =  BURST_MODE,
    .sync_info = {
        .hpw  = 64,
        .hbp  = 68,
        .hact = 720,
        .hfp  = 12,
        .vpw   = 5,
        .vbp   = 39,
        .vact  = 576,
        .vfp   = 5,
    },
    .phy_data_rate = 162,
    .pixel_clk = 27000,
};

static const combo_dev_cfg_t g_sample_comm_mipi_tx_1024x768_60_config = {
    .devno = 0,
    .lane_id = {0, 1, 2, 3},
    .out_mode = OUT_MODE_DSI_VIDEO,
    .out_format = OUT_FORMAT_RGB_24BIT,
    .video_mode =  BURST_MODE,
    .sync_info = {
        .hpw  = 136,
        .hbp  = 160,
        .hact = 1024,
        .hfp  = 24,
        .vpw   = 6,
        .vbp   = 29,
        .vact  = 768,
        .vfp   = 3,
    },
    .phy_data_rate = 390,
    .pixel_clk = 65000,
};

static const combo_dev_cfg_t g_sample_comm_mipi_tx_1280x720_50_config = {
    .devno = 0,
    .lane_id = {0, 1, 2, 3},
    .out_mode = OUT_MODE_DSI_VIDEO,
    .out_format = OUT_FORMAT_RGB_24BIT,
    .video_mode =  BURST_MODE,
    .sync_info = {
        .hpw  = 40,
        .hbp  = 220,
        .hact = 1280,
        .hfp  = 440,
        .vpw   = 5,
        .vbp   = 20,
        .vact  = 720,
        .vfp   = 5,
    },
    .phy_data_rate = 446,
    .pixel_clk = 74250,
};

static const combo_dev_cfg_t g_sample_comm_mipi_tx_1280x720_60_config = {
    .devno = 0,
    .lane_id = {0, 1, 2, 3},
    .out_mode = OUT_MODE_DSI_VIDEO,
    .out_format = OUT_FORMAT_RGB_24BIT,
    .video_mode =  BURST_MODE,
    .sync_info = {
        .hpw  = 40,
        .hbp  = 220,
        .hact = 1280,
        .hfp  = 110,
        .vpw   = 5,
        .vbp   = 20,
        .vact  = 720,
        .vfp   = 5,
    },
    .phy_data_rate = 446,
    .pixel_clk = 74250,
};

static const combo_dev_cfg_t g_sample_comm_mipi_tx_1280x1024_60_config = {
    .devno = 0,
    .lane_id = {0, 1, 2, 3},
    .out_mode = OUT_MODE_DSI_VIDEO,
    .out_format = OUT_FORMAT_RGB_24BIT,
    .video_mode =  BURST_MODE,
    .sync_info = {
        .hpw  = 112,
        .hbp  = 248,
        .hact = 1280,
        .hfp  = 48,
        .vpw   = 3,
        .vbp   = 38,
        .vact  = 1024,
        .vfp   = 1,
    },
    .phy_data_rate = 648, /* 486 */
    .pixel_clk = 108000,
};

static const combo_dev_cfg_t g_sample_comm_mipi_tx_1920x1080_24_config = {
    .devno = 0,
    .lane_id = {0, 1, 2, 3},
    .out_mode = OUT_MODE_DSI_VIDEO,
    .out_format = OUT_FORMAT_RGB_24BIT,
    .video_mode =  BURST_MODE,
    .sync_info = {
        .hpw  = 44,
        .hbp  = 148,
        .hact = 1920,
        .hfp  = 638,
        .vpw   = 5,
        .vbp   = 36,
        .vact  = 1080,
        .vfp   = 4,
    },
    .phy_data_rate = 446,
    .pixel_clk = 74250,
};

static const combo_dev_cfg_t g_sample_comm_mipi_tx_1920x1080_25_config = {
    .devno = 0,
    .lane_id = {0, 1, 2, 3},
    .out_mode = OUT_MODE_DSI_VIDEO,
    .out_format = OUT_FORMAT_RGB_24BIT,
    .video_mode =  BURST_MODE,
    .sync_info = {
        .hpw  = 44,
        .hbp  = 148,
        .hact = 1920,
        .hfp  = 528,
        .vpw   = 5,
        .vbp   = 36,
        .vact  = 1080,
        .vfp   = 4,
    },
    .phy_data_rate = 446,
    .pixel_clk = 74250,
};

static const combo_dev_cfg_t g_sample_comm_mipi_tx_1920x1080_30_config = {
    .devno = 0,
    .lane_id = {0, 1, 2, 3},
    .out_mode = OUT_MODE_DSI_VIDEO,
    .out_format = OUT_FORMAT_RGB_24BIT,
    .video_mode =  BURST_MODE,
    .sync_info = {
        .hpw  = 44,
        .hbp  = 148,
        .hact = 1920,
        .hfp  = 88,
        .vpw   = 5,
        .vbp   = 36,
        .vact  = 1080,
        .vfp   = 4,
    },
    .phy_data_rate = 446,
    .pixel_clk = 74250,
};

static const combo_dev_cfg_t g_sample_comm_mipi_tx_1920x1080_50_config = {
    .devno = 0,
    .lane_id = {0, 1, 2, 3},
    .out_mode = OUT_MODE_DSI_VIDEO,
    .out_format = OUT_FORMAT_RGB_24BIT,
    .video_mode =  BURST_MODE,
    .sync_info = {
        .hpw  = 44,
        .hbp  = 148,
        .hact = 1920,
        .hfp  = 528,
        .vpw   = 5,
        .vbp   = 36,
        .vact  = 1080,
        .vfp   = 4,
    },
    .phy_data_rate = 891,
    .pixel_clk = 148500,
};

static const combo_dev_cfg_t g_sample_comm_mipi_tx_1920x1080_60_config = {
    .devno = 0,
    .lane_id = {0, 1, 2, 3},
    .out_mode = OUT_MODE_DSI_VIDEO,
    .out_format = OUT_FORMAT_RGB_24BIT,
    .video_mode =  BURST_MODE,
    .sync_info = {
        .hpw  = 44,
        .hbp  = 148,
        .hact = 1920,
        .hfp  = 88,
        .vpw   = 5,
        .vbp   = 36,
        .vact  = 1080,
        .vfp   = 4,
    },
    .phy_data_rate = 891,
    .pixel_clk = 148500,
};

static const combo_dev_cfg_t g_sample_comm_mipi_tx_3840x2160_24_config = {
    .devno = 0,
    .lane_id = {0, 1, 2, 3},
    .out_mode = OUT_MODE_DSI_VIDEO,
    .out_format = OUT_FORMAT_RGB_24BIT,
    .video_mode =  BURST_MODE,
    .sync_info = {
        .hpw  = 88,
        .hbp  = 296,
        .hact = 3840,
        .hfp  = 1276,
        .vpw   = 10,
        .vbp   = 72,
        .vact  = 2160,
        .vfp   = 8,
    },
    .phy_data_rate = 1782,
    .pixel_clk = 297000,
};

static const combo_dev_cfg_t g_sample_comm_mipi_tx_3840x2160_25_config = {
    .devno = 0,
    .lane_id = {0, 1, 2, 3},
    .out_mode = OUT_MODE_DSI_VIDEO,
    .out_format = OUT_FORMAT_RGB_24BIT,
    .video_mode =  BURST_MODE,
    .sync_info = {
        .hpw  = 88,
        .hbp  = 296,
        .hact = 3840,
        .hfp  = 1056,
        .vpw   = 10,
        .vbp   = 72,
        .vact  = 2160,
        .vfp   = 8,
    },
    .phy_data_rate = 1782,
    .pixel_clk = 297000,
};

static const combo_dev_cfg_t g_sample_comm_mipi_tx_3840x2160_30_config = {
    .devno = 0,
    .lane_id = {0, 1, 2, 3},
    .out_mode = OUT_MODE_DSI_VIDEO,
    .out_format = OUT_FORMAT_RGB_24BIT,
    .video_mode =  BURST_MODE,
    .sync_info = {
        .hpw  = 88,
        .hbp  = 296,
        .hact = 3840,
        .hfp  = 176,
        .vpw   = 10,
        .vbp   = 72,
        .vact  = 2160,
        .vfp   = 8,
    },
    .phy_data_rate = 1782, /* lt9611's max data rate < 2000Mbps */
    .pixel_clk = 297000,
};

static const combo_dev_cfg_t g_sample_comm_mipi_tx_3840x2160_50_config = {
    .devno = 0,
    .lane_id = {0, 1, 2, 3},
    .out_mode = OUT_MODE_CSI,
    .out_format = OUT_FORMAT_RAW_16BIT,
    .video_mode =  BURST_MODE,
    .sync_info = {
        .hpw  = 88,
        .hbp  = 296,
        .hact = 3840,
        .hfp  = 1056,
        .vpw   = 10,
        .vbp   = 72,
        .vact  = 2160,
        .vfp   = 8,
    },
    .phy_data_rate = 2259, /* 2376 is larger than the max 2259, set to 2259 */
    .pixel_clk = 594000,
};

static const combo_dev_cfg_t g_sample_comm_mipi_tx_3840x2160_60_config = {
    .devno = 0,
    .lane_id = {0, 1, 2, 3},
    .out_mode = OUT_MODE_CSI,
    .out_format = OUT_FORMAT_RAW_16BIT,
    .video_mode =  BURST_MODE,
    .sync_info = {
        .hpw  = 88,
        .hbp  = 296,
        .hact = 3840,
        .hfp  = 176,
        .vpw   = 10,
        .vbp   = 72,
        .vact  = 2160,
        .vfp   = 8,
    },
    .phy_data_rate = 2259, /* 2376 is larger than the max 2259, set to 2259 */
    .pixel_clk = 594000,
};

static const combo_dev_cfg_t g_sample_comm_mipi_tx_720x1280_60_config = {
    .devno = 0,
    .lane_id = {0, 1, 2, 3},
    .out_mode = OUT_MODE_DSI_VIDEO,
    .out_format = OUT_FORMAT_RGB_24BIT,
    .video_mode =  BURST_MODE,
    .sync_info = {
        .hpw  = 24,
        .hbp  = 99,
        .hact = 720,
        .hfp  = 99,
        .vpw   = 4,
        .vbp   = 20,
        .vact  = 1280,
        .vfp   = 8,
    },
    .phy_data_rate = 459,
    .pixel_clk = 74250,
};

static const combo_dev_cfg_t g_sample_comm_mipi_tx_1080x1920_60_config = {
    .devno = 0,
    .lane_id = {0, 1, 2, 3},
    .out_mode = OUT_MODE_DSI_VIDEO,
    .out_format = OUT_FORMAT_RGB_24BIT,
    .video_mode =  BURST_MODE,
    .sync_info = {
        .hpw = 8,
        .hbp = 20,
        .hact = 1080,
        .hfp = 130,
        .vpw = 10,
        .vbp = 26,
        .vact = 1920,
        .vfp = 16,
    },
    .phy_data_rate = 891,
    .pixel_clk = 148500,
};

typedef struct {
    mipi_tx_intf_sync index;
    const combo_dev_cfg_t *mipi_tx_combo_dev_cfg;
} mipi_tx_intf_sync_cfg;

typedef struct {
    hi_vo_intf_sync intf_sync;
    mipi_tx_intf_sync mipi_tx_sync;
    hi_char *mipi_fmt_name;
} vo_mst_sync_mipi_tx;

static const mipi_tx_intf_sync_cfg g_sample_mipi_tx_timing[HI_MIPI_TX_OUT_USER] = {
    {HI_MIPI_TX_OUT_576P50,       &g_sample_comm_mipi_tx_720x576_50_config},
    {HI_MIPI_TX_OUT_1024X768_60,  &g_sample_comm_mipi_tx_1024x768_60_config},
    {HI_MIPI_TX_OUT_720P50,       &g_sample_comm_mipi_tx_1280x720_50_config},
    {HI_MIPI_TX_OUT_720P60,       &g_sample_comm_mipi_tx_1280x720_60_config},
    {HI_MIPI_TX_OUT_1280X1024_60, &g_sample_comm_mipi_tx_1280x1024_60_config},
    {HI_MIPI_TX_OUT_1080P24,      &g_sample_comm_mipi_tx_1920x1080_24_config},
    {HI_MIPI_TX_OUT_1080P25,      &g_sample_comm_mipi_tx_1920x1080_25_config},
    {HI_MIPI_TX_OUT_1080P30,      &g_sample_comm_mipi_tx_1920x1080_30_config},
    {HI_MIPI_TX_OUT_1080P50,      &g_sample_comm_mipi_tx_1920x1080_50_config},
    {HI_MIPI_TX_OUT_1080P60,      &g_sample_comm_mipi_tx_1920x1080_60_config},
    {HI_MIPI_TX_OUT_3840X2160_24, &g_sample_comm_mipi_tx_3840x2160_24_config},
    {HI_MIPI_TX_OUT_3840X2160_25, &g_sample_comm_mipi_tx_3840x2160_25_config},
    {HI_MIPI_TX_OUT_3840X2160_30, &g_sample_comm_mipi_tx_3840x2160_30_config},
    {HI_MIPI_TX_OUT_3840X2160_50, &g_sample_comm_mipi_tx_3840x2160_50_config},
    {HI_MIPI_TX_OUT_3840X2160_60, &g_sample_comm_mipi_tx_3840x2160_60_config},

    {HI_MIPI_TX_OUT_720X1280_60,  &g_sample_comm_mipi_tx_720x1280_60_config},
    {HI_MIPI_TX_OUT_1080X1920_60, &g_sample_comm_mipi_tx_1080x1920_60_config},
    {0},
};

static const combo_dev_cfg_t *sample_mipi_tx_get_combo_dev_config(mipi_tx_intf_sync mipi_intf_sync)
{
    hi_u32 loop;
    size_t loop_num = sizeof(g_sample_mipi_tx_timing) / sizeof(mipi_tx_intf_sync_cfg);

    for (loop = 0; loop < loop_num; loop++) {
        if (g_sample_mipi_tx_timing[loop].index == mipi_intf_sync) {
            return g_sample_mipi_tx_timing[loop].mipi_tx_combo_dev_cfg;
        }
    }
    return HI_NULL;
}

static hi_s32 vo_mst_mipi_tx_send_one_cmd(cmd_info_t *cmd_info)
{
    hi_s32 ret;
    ret = ioctl(g_sample_comm_mipi_fd, HI_MIPI_TX_SET_CMD, cmd_info);
    if (ret != HI_SUCCESS) {
        printf("MIPI_TX SET CMD failed\n");
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

static hi_s32 vo_mst_mipi_tx_init_screen(const sample_mipi_tx_config *tx_config)
{
    hi_s32 ret;
    cmd_info_t ci = { 0 };
    hi_u32 loop;
    hi_u32 loop_num = tx_config->cmd_count;

    for (loop = 0; loop < loop_num; loop++) {
        (hi_void)memcpy_s(&ci, sizeof(cmd_info_t), &(tx_config->cmd_info[loop].cmd_info), sizeof(cmd_info_t));
        ret = vo_mst_mipi_tx_send_one_cmd(&ci);
        if (ret != HI_SUCCESS) {
            printf("loop(%u): MIPI_TX SET CMD failed\n", loop);

            return HI_FAILURE;
        }
        usleep(tx_config->cmd_info[loop].usleep_value);
    }

    return HI_SUCCESS;
}

static hi_s32 sample_comm_mipi_tx_check_config(const sample_mipi_tx_config *tx_config)
{
    mipi_tx_intf_sync mipi_intf_sync;

    if (tx_config == NULL) {
        sample_print("tx_config is null\n");
        return HI_FAILURE;
    }

    mipi_intf_sync = tx_config->intf_sync;
    if ((mipi_intf_sync >= HI_MIPI_TX_OUT_BUTT)) {
        sample_print("mipi tx sync illegal\n");
        return HI_FAILURE;
    }
    return HI_SUCCESS;
}

static hi_s32 sample_comm_mipi_tx_get_config(const sample_mipi_tx_config *tx_config,
    const combo_dev_cfg_t **mipi_tx_config)
{
    mipi_tx_intf_sync mipi_intf_sync;
    mipi_intf_sync = tx_config->intf_sync;

    if (mipi_intf_sync == HI_MIPI_TX_OUT_USER) {
        *mipi_tx_config = &tx_config->combo_dev_cfg;
        if ((*mipi_tx_config)->phy_data_rate == 0) {
            printf("error: not set mipi tx user config\n");
            return HI_FAILURE;
        }
    } else {
        *mipi_tx_config = sample_mipi_tx_get_combo_dev_config(mipi_intf_sync);
        if (*mipi_tx_config == HI_NULL) {
            sample_print("error: mipi tx combo config is null\n");
            return HI_FAILURE;
        }
    }
    return HI_SUCCESS;
}

static hi_void sample_comm_mipi_tx_do_close_fd(hi_void)
{
    if (g_sample_comm_mipi_fd != HI_INVALID_VALUE) {
        close(g_sample_comm_mipi_fd);
        g_sample_comm_mipi_fd = HI_INVALID_VALUE;
    }
}

hi_s32 sample_comm_start_mipi_tx(const sample_mipi_tx_config *tx_config)
{
    mipi_tx_intf_sync mipi_intf_sync;
    const combo_dev_cfg_t *combo_config = HI_NULL;
    hi_s32 ret;

    ret = sample_comm_mipi_tx_check_config(tx_config);
    if (ret != HI_SUCCESS) {
        return ret;
    }

    g_sample_comm_mipi_fd = open(MIPI_TX_DEV_NAME, O_RDONLY);
    if (g_sample_comm_mipi_fd < 0) {
        printf("open mipi dev file (%s) fail\n", MIPI_TX_DEV_NAME);
        return HI_FAILURE;
    }

    mipi_intf_sync = tx_config->intf_sync;
    printf("mipi intf sync = %d\n", mipi_intf_sync);

    ret = sample_comm_mipi_tx_get_config(tx_config, &combo_config);
    if (ret != HI_SUCCESS) {
        printf("%s,%d, get mipi tx config fail\n", __FUNCTION__, __LINE__);
        sample_comm_mipi_tx_do_close_fd();
        return ret;
    }

    /* step1 */
    ret = ioctl(g_sample_comm_mipi_fd, HI_MIPI_TX_DISABLE, NULL);
    if (ret != HI_SUCCESS) {
        printf("%s,%d, ioctl mipi tx (%s) fail at ret(%d)\n", __FUNCTION__, __LINE__, MIPI_TX_DEV_NAME, ret);
        sample_comm_mipi_tx_do_close_fd();
        return ret;
    }

    /* step2 */
    ret = ioctl(g_sample_comm_mipi_fd, HI_MIPI_TX_SET_DEV_CFG, combo_config);
    if (ret != HI_SUCCESS) {
        printf("%s,%d, ioctl mipi tx (%s) fail at ret(%d)\n", __FUNCTION__, __LINE__, MIPI_TX_DEV_NAME, ret);
        sample_comm_mipi_tx_do_close_fd();
        return ret;
    }

    /* step3 */
    ret = vo_mst_mipi_tx_init_screen(tx_config);
    if (ret != HI_SUCCESS) {
        printf("%s,%d, init screen failed\n", __FUNCTION__, __LINE__);
        sample_comm_mipi_tx_do_close_fd();
        return ret;
    }

    /* step4 */
    ret = ioctl(g_sample_comm_mipi_fd, HI_MIPI_TX_ENABLE, NULL);
    if (ret != HI_SUCCESS) {
        printf("%s,%d, ioctl mipi tx (%s) fail at ret(%d)\n", __FUNCTION__, __LINE__, MIPI_TX_DEV_NAME, ret);
    }

    sample_comm_mipi_tx_do_close_fd();
    return ret;
}

hi_void sample_comm_stop_mipi_tx(hi_vo_intf_type intf_type)
{
    if (!((intf_type & HI_VO_INTF_MIPI) ||
        (intf_type & HI_VO_INTF_MIPI_SLAVE))) {
        sample_print("intf is not mipi\n");
        return;
    }

    g_sample_comm_mipi_fd = open(MIPI_TX_DEV_NAME, O_RDONLY);
    if (g_sample_comm_mipi_fd < 0) {
        printf("open mipi dev file (%s) fail\n", MIPI_TX_DEV_NAME);
        return;
    }

    if (ioctl(g_sample_comm_mipi_fd, HI_MIPI_TX_DISABLE, NULL) < 0) {
        printf("ioctl mipi tx (%s) fail\n", MIPI_TX_DEV_NAME);
        return;
    }

    close(g_sample_comm_mipi_fd);
    g_sample_comm_mipi_fd = HI_INVALID_VALUE;
}

#else

hi_void sample_comm_mipi_tx_set_intf_sync(mipi_tx_intf_sync intf_sync)
{
}

hi_void sample_comm_start_mipi_tx(hi_vo_pub_attr *pub_attr)
{
}

hi_void sample_comm_stop_mipi_tx(hi_vo_intf_type intf_type)
{
}
#endif /* end of #if VO_MIPI_SUPPORT */

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* end of #ifdef __cplusplus */
