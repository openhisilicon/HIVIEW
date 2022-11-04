
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
#include <sys/prctl.h>

#include "hi_mipi.h"

#include "hi_common.h"
#include "sample_comm.h"
#include "mppex.h"

#define MIPI_DEV_NODE       "/dev/hi_mipi"

#define MAX_FRAME_WIDTH     8192

SAMPLE_VI_DUMP_THREAD_INFO_S g_stViDumpRawThreadInfo;


combo_dev_attr_t MIPI_4lane_CHN0_SENSOR_IMX327_12BIT_2M_NOWDR_ATTR =
{
    .devno = 0,
    .input_mode = INPUT_MODE_MIPI,
    .data_rate = MIPI_DATA_RATE_X1,
    .img_rect = {0, 0, 1920, 1080},

    {
        .mipi_attr =
        {
            DATA_TYPE_RAW_12BIT,
            HI_MIPI_WDR_MODE_NONE,
            {0, 1, 2, 3}
        }
    }
};

combo_dev_attr_t MIPI_4lane_CHN0_SENSOR_IMX327_12BIT_2M_WDR2to1_ATTR =
{
    .devno = 0,
    .input_mode = INPUT_MODE_MIPI,
    .data_rate = MIPI_DATA_RATE_X1,
    .img_rect = {0, 0, 1920, 1080},

    {
        .mipi_attr =
        {
            DATA_TYPE_RAW_12BIT,
            HI_MIPI_WDR_MODE_DOL,
            {0, 1, 2, 3}
        }
    }
};

combo_dev_attr_t MIPI_2lane_CHN0_SENSOR_IMX327_12BIT_2M_NOWDR_ATTR =
{
    .devno = 0,
    .input_mode = INPUT_MODE_MIPI,
    .data_rate = MIPI_DATA_RATE_X1,
    .img_rect = {0, 0, 1920, 1080},

    {
        .mipi_attr =
        {
            DATA_TYPE_RAW_12BIT,
            HI_MIPI_WDR_MODE_NONE,
            {0, 2, -1, -1}
        }
    }
};

combo_dev_attr_t MIPI_2lane_CHN1_SENSOR_IMX327_12BIT_2M_NOWDR_ATTR =
{
    .devno = 1,
    .input_mode = INPUT_MODE_MIPI,
    .data_rate = MIPI_DATA_RATE_X1,
    .img_rect = {0, 0, 1920, 1080},

    {
        .mipi_attr =
        {
            DATA_TYPE_RAW_12BIT,
            HI_MIPI_WDR_MODE_NONE,
            {1, 3, -1, -1}
        }
    }
};

combo_dev_attr_t MIPI_2lane_CHN0_SENSOR_IMX327_12BIT_2M_WDR2to1_ATTR =
{
    .devno = 0,
    .input_mode = INPUT_MODE_MIPI,
    .data_rate = MIPI_DATA_RATE_X1,
    .img_rect = {0, 0, 1920, 1080},

    {
        .mipi_attr =
        {
            DATA_TYPE_RAW_12BIT,
            HI_MIPI_WDR_MODE_DOL,
            {0, 2, -1, -1}
        }
    }
};

combo_dev_attr_t MIPI_2lane_CHN1_SENSOR_IMX327_12BIT_2M_WDR2to1_ATTR =
{
    .devno = 1,
    .input_mode = INPUT_MODE_MIPI,
    .data_rate = MIPI_DATA_RATE_X1,
    .img_rect = {0, 0, 1920, 1080},

    {
        .mipi_attr =
        {
            DATA_TYPE_RAW_12BIT,
            HI_MIPI_WDR_MODE_DOL,
            {1, 3, -1, -1}
        }
    }
};

combo_dev_attr_t MIPI_4lane_CHN0_SENSOR_IMX390_12BIT_2M_NOWDR_ATTR =
{
    .devno = 0,
    .input_mode = INPUT_MODE_MIPI,
    .data_rate = MIPI_DATA_RATE_X1,
    .img_rect = {0, 12, 1920, 1080},

    {
        .mipi_attr =
        {
            DATA_TYPE_RAW_12BIT,
            HI_MIPI_WDR_MODE_NONE,
            {0, 1, 2, 3}
        }
    }
};

combo_dev_attr_t MIPI_4lane_CHN0_SENSOR_IMX290_12BIT_2M_NOWDR_ATTR =
{
    .devno = 0,
    .input_mode = INPUT_MODE_MIPI,
    .data_rate = MIPI_DATA_RATE_X1,
    .img_rect = {0, 0, 1920, 1080},

    {
        .mipi_attr =
        {
            DATA_TYPE_RAW_12BIT,
            HI_MIPI_WDR_MODE_NONE,
            {0, 1, 2, 3}
        }
    }
};

combo_dev_attr_t MIPI_4lane_CHN0_SENSOR_IMX290_12BIT_2M_WDR2to1_ATTR =
{
    .devno = 0,
    .input_mode = INPUT_MODE_MIPI,
    .data_rate = MIPI_DATA_RATE_X1,
    .img_rect = {0, 0, 1920, 1080},

    {
        .mipi_attr =
        {
            DATA_TYPE_RAW_12BIT,
            HI_MIPI_WDR_MODE_DOL,
            {0, 1, 2, 3}
        }
    }
};

combo_dev_attr_t MIPI_2lane_CHN0_SENSOR_IMX290_12BIT_2M_NOWDR_ATTR =
{
    .devno = 0,
    .input_mode = INPUT_MODE_MIPI,
    .data_rate = MIPI_DATA_RATE_X1,
    .img_rect = {0, 0, 1920, 1080},

    {
        .mipi_attr =
        {
            DATA_TYPE_RAW_12BIT,
            HI_MIPI_WDR_MODE_NONE,
            {0, 2, -1, -1}
        }
    }
};

combo_dev_attr_t MIPI_2lane_CHN1_SENSOR_IMX290_12BIT_2M_NOWDR_ATTR =
{
    .devno = 1,
    .input_mode = INPUT_MODE_MIPI,
    .data_rate = MIPI_DATA_RATE_X1,
    .img_rect = {0, 0, 1920, 1080},

    {
        .mipi_attr =
        {
            DATA_TYPE_RAW_12BIT,
            HI_MIPI_WDR_MODE_NONE,
            {1, 3, -1, -1}
        }
    }
};

combo_dev_attr_t MIPI_2lane_CHN0_SENSOR_IMX290_12BIT_2M_WDR2to1_ATTR =
{
    .devno = 0,
    .input_mode = INPUT_MODE_MIPI,
    .data_rate = MIPI_DATA_RATE_X1,
    .img_rect = {0, 0, 1920, 1080},

    {
        .mipi_attr =
        {
            DATA_TYPE_RAW_12BIT,
            HI_MIPI_WDR_MODE_DOL,
            {0, 2, -1, -1}
        }
    }
};

combo_dev_attr_t MIPI_2lane_CHN1_SENSOR_IMX290_12BIT_2M_WDR2to1_ATTR =
{
    .devno = 1,
    .input_mode = INPUT_MODE_MIPI,
    .data_rate = MIPI_DATA_RATE_X1,
    .img_rect = {0, 0, 1920, 1080},

    {
        .mipi_attr =
        {
            DATA_TYPE_RAW_12BIT,
            HI_MIPI_WDR_MODE_DOL,
            {1, 3, -1, -1}
        }
    }
};



combo_dev_attr_t MIPI_4lane_CHN0_SENSOR_IMX307_12BIT_2M_NOWDR_ATTR =
{
    .devno = 0,
    .input_mode = INPUT_MODE_MIPI,
    .data_rate = MIPI_DATA_RATE_X1,
    .img_rect = {0, 0, 1920, 1080},

    {
        .mipi_attr =
        {
            DATA_TYPE_RAW_12BIT,
            HI_MIPI_WDR_MODE_NONE,
            {0, 1, 2, 3}
        }
    }
};

combo_dev_attr_t MIPI_4lane_CHN0_SENSOR_IMX307_12BIT_2M_WDR2to1_ATTR =
{
    .devno = 0,
    .input_mode = INPUT_MODE_MIPI,
    .data_rate = MIPI_DATA_RATE_X1,
    .img_rect = {0, 0, 1920, 1080},

    {
        .mipi_attr =
        {
            DATA_TYPE_RAW_12BIT,
            HI_MIPI_WDR_MODE_DOL,
            {0, 1, 2, 3}
        }
    }
};

combo_dev_attr_t MIPI_2lane_CHN0_SENSOR_IMX307_12BIT_2M_NOWDR_ATTR =
{
    .devno = 0,
    .input_mode = INPUT_MODE_MIPI,
    .data_rate = MIPI_DATA_RATE_X1,
    .img_rect = {0, 0, 1920, 1080},

    {
        .mipi_attr =
        {
            DATA_TYPE_RAW_12BIT,
            HI_MIPI_WDR_MODE_NONE,
            {0, 2, -1, -1}
        }
    }
};

combo_dev_attr_t MIPI_2lane_CHN1_SENSOR_IMX307_12BIT_2M_NOWDR_ATTR =
{
    .devno = 1,
    .input_mode = INPUT_MODE_MIPI,
    .data_rate = MIPI_DATA_RATE_X1,
    .img_rect = {0, 0, 1920, 1080},

    {
        .mipi_attr =
        {
            DATA_TYPE_RAW_12BIT,
            HI_MIPI_WDR_MODE_NONE,
            {1, 3, -1, -1}
        }
    }
};

combo_dev_attr_t MIPI_2lane_CHN0_SENSOR_IMX307_12BIT_2M_WDR2to1_ATTR =
{
    .devno = 0,
    .input_mode = INPUT_MODE_MIPI,
    .data_rate = MIPI_DATA_RATE_X1,
    .img_rect = {0, 0, 1920, 1080},

    {
        .mipi_attr =
        {
            DATA_TYPE_RAW_12BIT,
            HI_MIPI_WDR_MODE_DOL,
            {0, 2, -1, -1}
        }
    }
};

combo_dev_attr_t MIPI_2lane_CHN1_SENSOR_IMX307_12BIT_2M_WDR2to1_ATTR =
{
    .devno = 1,
    .input_mode = INPUT_MODE_MIPI,
    .data_rate = MIPI_DATA_RATE_X1,
    .img_rect = {0, 0, 1920, 1080},

    {
        .mipi_attr =
        {
            DATA_TYPE_RAW_12BIT,
            HI_MIPI_WDR_MODE_DOL,
            {1, 3, -1, -1}
        }
    }
};

combo_dev_attr_t MIPI_2lane_CHN0_SENSOR_PS5260_12BIT_2M_NOWDR_ATTR =
{
    .devno = 0,
    .input_mode = INPUT_MODE_MIPI,
    .data_rate = MIPI_DATA_RATE_X1,
    .img_rect = {0, 0, 1920, 1080},

    {
        .mipi_attr =
        {
            DATA_TYPE_RAW_12BIT,
            HI_MIPI_WDR_MODE_NONE,
            {0, 1, -1, -1}
        }
    }
};

combo_dev_attr_t MIPI_2lane_CHN0_SENSOR_PS5260_12BIT_2M_WDR2to1_ATTR =
{
    .devno = 0,
    .input_mode = INPUT_MODE_MIPI,
    .data_rate = MIPI_DATA_RATE_X1,
    .img_rect = {0, 0, 1920, 1080},

    {
        .mipi_attr =
        {
            DATA_TYPE_RAW_12BIT,
            HI_MIPI_WDR_MODE_DOL,
            {0, 1, -1, -1}
        }
    }
};

combo_dev_attr_t LVDS_4lane_CHN0_SENSOR_MN34220_12BIT_2M_NOWDR_ATTR =
{
    .devno = 0,
    .input_mode = INPUT_MODE_LVDS,
    .data_rate  = MIPI_DATA_RATE_X1,
    .img_rect   = {0, 0, 1920, 1080},

    .lvds_attr =
    {
        .input_data_type   = DATA_TYPE_RAW_12BIT,
        .wdr_mode          = HI_WDR_MODE_NONE,
        .sync_mode         = LVDS_SYNC_MODE_SOF,
        .vsync_attr        = {LVDS_VSYNC_NORMAL, 0, 0},
        .fid_attr          = {LVDS_FID_NONE, HI_TRUE},
        .data_endian       = LVDS_ENDIAN_BIG,
        .sync_code_endian  = LVDS_ENDIAN_BIG,
        .lane_id = {0, 2, 1, 3},
        .sync_code = {
            {
                {0x002, 0x003, 0x000, 0x001},      // lane 0
                {0x002, 0x003, 0x000, 0x001},
                {0x002, 0x003, 0x000, 0x001},
                {0x002, 0x003, 0x000, 0x001}
            },

            {
                {0x012, 0x013, 0x010, 0x011},      // lane 1
                {0x012, 0x013, 0x010, 0x011},
                {0x012, 0x013, 0x010, 0x011},
                {0x012, 0x013, 0x010, 0x011}
            },

            {
                {0x006, 0x007, 0x004, 0x005},      // lane2
                {0x006, 0x007, 0x004, 0x005},
                {0x006, 0x007, 0x004, 0x005},
                {0x006, 0x007, 0x004, 0x005}
            },

            {
                {0x016, 0x017, 0x014, 0x015},      // lane3
                {0x016, 0x017, 0x014, 0x015},
                {0x016, 0x017, 0x014, 0x015},
                {0x016, 0x017, 0x014, 0x015}
            },
        }
    }
};



combo_dev_attr_t MIPI_2lane_CHN0_SENSOR_IMX335_10BIT_5M_NOWDR_ATTR =
{
    .devno = 0,
    .input_mode = INPUT_MODE_MIPI,
    .data_rate = MIPI_DATA_RATE_X1,
    .img_rect = {0, 0, 2592, 1944},

    {
        .mipi_attr =
        {
            DATA_TYPE_RAW_10BIT,
            HI_MIPI_WDR_MODE_NONE,
            {0, 2, -1, -1}
        }
    }
};

combo_dev_attr_t MIPI_2lane_CHN1_SENSOR_IMX335_10BIT_5M_NOWDR_ATTR =
{
    .devno = 1,
    .input_mode = INPUT_MODE_MIPI,
    .data_rate = MIPI_DATA_RATE_X1,
    .img_rect = {0, 0, 2592, 1944},

    {
        .mipi_attr =
        {
            DATA_TYPE_RAW_10BIT,
            HI_MIPI_WDR_MODE_NONE,
            {1, 3, -1, -1}
        }
    }
};


combo_dev_attr_t MIPI_4lane_CHN0_SENSOR_IMX335_12BIT_5M_NOWDR_ATTR =
{
    .devno = 0,
    .input_mode = INPUT_MODE_MIPI,
    .data_rate = MIPI_DATA_RATE_X1,
    .img_rect = {0, 0, 2592, 1944},

    {
        .mipi_attr =
        {
            DATA_TYPE_RAW_12BIT,
            HI_MIPI_WDR_MODE_NONE,
            {0, 1, 2, 3}
        }
    }
};

combo_dev_attr_t MIPI_4lane_CHN0_SENSOR_IMX335_10BIT_5M_WDR2TO1_ATTR =
{
    .devno = 0,
    .input_mode = INPUT_MODE_MIPI,
    .data_rate = MIPI_DATA_RATE_X1,
    .img_rect = {0, 0, 2592, 1944},

    {
        .mipi_attr =
        {
            DATA_TYPE_RAW_10BIT,
            HI_MIPI_WDR_MODE_VC,
            {0, 1, 2, 3}
        }
    }
};

combo_dev_attr_t MIPI_4lane_CHN0_SENSOR_IMX335_12BIT_4M_NOWDR_ATTR =
{
    .devno = 0,
    .input_mode = INPUT_MODE_MIPI,
    .data_rate = MIPI_DATA_RATE_X1,
    .img_rect = {0, 204, 2592, 1536},

    {
        .mipi_attr =
        {
            DATA_TYPE_RAW_12BIT,
            HI_MIPI_WDR_MODE_NONE,
            {0, 1, 2, 3}
        }
    }
};

combo_dev_attr_t MIPI_4lane_CHN0_SENSOR_IMX335_10BIT_4M_WDR2TO1_ATTR =
{
    .devno = 0,
    .input_mode = INPUT_MODE_MIPI,
    .data_rate = MIPI_DATA_RATE_X1,
    .img_rect = {0, 204, 2592, 1536},

    {
        .mipi_attr =
        {
            DATA_TYPE_RAW_10BIT,
            HI_MIPI_WDR_MODE_VC,
            {0, 1, 2, 3}
        }
    }
};

combo_dev_attr_t MIPI_4lane_CHN0_SENSOR_SC4210_12BIT_3M_NOWDR_ATTR =
{
    .devno = 0,
    .input_mode = INPUT_MODE_MIPI,
    .data_rate = MIPI_DATA_RATE_X1,
    .img_rect = {0, 0, 2560, 1440},

    {
        .mipi_attr =
        {
            DATA_TYPE_RAW_12BIT,
            HI_MIPI_WDR_MODE_NONE,
            {0, 1, 2, 3}
        }
    }
};

combo_dev_attr_t MIPI_4lane_CHN0_SENSOR_SC4210_10BIT_3M_WDR2TO1_ATTR =
{
    .devno = 0,
    .input_mode = INPUT_MODE_MIPI,
    .data_rate = MIPI_DATA_RATE_X1,
    .img_rect = {0, 0, 2560, 1440},

    {
        .mipi_attr =
        {
            DATA_TYPE_RAW_10BIT,
            HI_MIPI_WDR_MODE_VC,
            {0, 1, 2, 3}
        }
    }
};

combo_dev_attr_t MIPI_4lane_CHN0_SENSOR_IMX458_10BIT_8M_NOWDR_ATTR =
{
    .devno = 0,
    .input_mode = INPUT_MODE_MIPI,
    .data_rate = MIPI_DATA_RATE_X1,
    .img_rect = {0, 0, 3840, 2160},

    {
        .mipi_attr =
        {
            DATA_TYPE_RAW_10BIT,
            HI_MIPI_WDR_MODE_NONE,
            {0, 1, 2, 3}
        }
    }
};

combo_dev_attr_t MIPI_4lane_CHN0_SENSOR_IMX458_10BIT_12M_NOWDR_ATTR =
{
    .devno = 0,
    .input_mode = INPUT_MODE_MIPI,
    .data_rate = MIPI_DATA_RATE_X1,
    .img_rect = {0, 0, 4000, 3000},

    {
        .mipi_attr =
        {
            DATA_TYPE_RAW_10BIT,
            HI_MIPI_WDR_MODE_NONE,
            {0, 1, 2, 3}
        }
    }
};

combo_dev_attr_t MIPI_4lane_CHN0_SENSOR_IMX458_10BIT_4M_NOWDR_ATTR =
{
    .devno = 0,
    .input_mode = INPUT_MODE_MIPI,
    .data_rate = MIPI_DATA_RATE_X1,
    .img_rect = {0, 0, 2716, 1524},

    {
        .mipi_attr =
        {
            DATA_TYPE_RAW_10BIT,
            HI_MIPI_WDR_MODE_NONE,
            {0, 1, 2, 3}
        }
    }
};

combo_dev_attr_t MIPI_4lane_CHN0_SENSOR_IMX458_10BIT_2M_NOWDR_ATTR =
{
    .devno = 0,
    .input_mode = INPUT_MODE_MIPI,
    .data_rate = MIPI_DATA_RATE_X1,
    .img_rect = {0, 0, 1920, 1080},

    {
        .mipi_attr =
        {
            DATA_TYPE_RAW_10BIT,
            HI_MIPI_WDR_MODE_NONE,
            {0, 1, 2, 3}
        }
    }
};

combo_dev_attr_t MIPI_4lane_CHN0_SENSOR_IMX458_10BIT_1M_NOWDR_ATTR =
{
    .devno = 0,
    .input_mode = INPUT_MODE_MIPI,
    .data_rate = MIPI_DATA_RATE_X1,
    .img_rect = {0, 0, 1280, 720},

    {
        .mipi_attr =
        {
            DATA_TYPE_RAW_10BIT,
            HI_MIPI_WDR_MODE_NONE,
            {0, 1, 2, 3}
        }
    }
};

combo_dev_attr_t MIPI_2lane_CHN0_SENSOR_OS04B10_10BIT_4M_NOWDR_ATTR =
{
    .devno = 0,
    .input_mode = INPUT_MODE_MIPI,
    .data_rate = MIPI_DATA_RATE_X1,
    .img_rect = {0, 0, 2560, 1440},

    {
        .mipi_attr =
        {
            DATA_TYPE_RAW_10BIT,
            HI_MIPI_WDR_MODE_NONE,
            {0, 1,-1, -1}
        }
    }
};

combo_dev_attr_t MIPI_4lane_CHN0_SENSOR_OS05A_12BIT_4M_NOWDR_ATTR =
{
    .devno = 0,
    .input_mode = INPUT_MODE_MIPI,
    .data_rate = MIPI_DATA_RATE_X1,
    .img_rect = {0, 204, 2688, 1536},

    {
        .mipi_attr =
        {
            DATA_TYPE_RAW_12BIT,
            HI_MIPI_WDR_MODE_NONE,
            {0, 1, 2, 3}
        }
    }
};

combo_dev_attr_t MIPI_4lane_CHN0_SENSOR_OS05A_10BIT_4M_WDR2TO1_ATTR =
{
    .devno = 0,
    .input_mode = INPUT_MODE_MIPI,
    .data_rate = MIPI_DATA_RATE_X1,
    .img_rect = {0, 204, 2688, 1536},

    {
        .mipi_attr =
        {
            DATA_TYPE_RAW_10BIT,
            HI_MIPI_WDR_MODE_VC,
            {0, 1, 2, 3}
        }
    }
};

/* os05a mipi_2l only 10bit linear and wdr */
combo_dev_attr_t MIPI_2lane_CHN0_SENSOR_OS05A_10BIT_1M_NOWDR_ATTR =
{
    .devno = 0,
    .input_mode = INPUT_MODE_MIPI,
    .data_rate = MIPI_DATA_RATE_X1,
    .img_rect = {32, 126, 1280, 720},

    {
        .mipi_attr =
        {
            DATA_TYPE_RAW_10BIT,
            HI_MIPI_WDR_MODE_NONE,
            {0, 2, -1, -1}
        }
    }
};

combo_dev_attr_t MIPI_2lane_CHN1_SENSOR_OS05A_10BIT_1M_NOWDR_ATTR =
{
    .devno = 1,
    .input_mode = INPUT_MODE_MIPI,
    .data_rate = MIPI_DATA_RATE_X1,
    .img_rect = {32, 126, 1280, 720},

    {
        .mipi_attr =
        {
            DATA_TYPE_RAW_10BIT,
            HI_MIPI_WDR_MODE_NONE,
            {1, 3, -1, -1}
        }
    }
};

combo_dev_attr_t MIPI_2lane_CHN0_SENSOR_OS05A_10BIT_4M_NOWDR_ATTR =
{
    .devno = 0,
    .input_mode = INPUT_MODE_MIPI,
    .data_rate = MIPI_DATA_RATE_X1,
    .img_rect = {16, 252, 2560, 1440},

    {
        .mipi_attr =
        {
            DATA_TYPE_RAW_10BIT,
            HI_MIPI_WDR_MODE_NONE,
            {0, 2, -1, -1}
        }
    }
};

combo_dev_attr_t MIPI_2lane_CHN1_SENSOR_OS05A_10BIT_4M_NOWDR_ATTR =
{
    .devno = 1,
    .input_mode = INPUT_MODE_MIPI,
    .data_rate = MIPI_DATA_RATE_X1,
    .img_rect = {16, 252, 2560, 1440},

    {
        .mipi_attr =
        {
            DATA_TYPE_RAW_10BIT,
            HI_MIPI_WDR_MODE_NONE,
            {1, 3, -1, -1}
        }
    }
};

combo_dev_attr_t MIPI_2lane_CHN0_SENSOR_OS05A_10BIT_4M_WDR2TO1_ATTR =
{
    .devno = 0,
    .input_mode = INPUT_MODE_MIPI,
    .data_rate = MIPI_DATA_RATE_X1,
    .img_rect = {16, 252, 2560, 1440},

    {
        .mipi_attr =
        {
            DATA_TYPE_RAW_10BIT,
            HI_MIPI_WDR_MODE_VC,
            {0, 2, -1, -1}
        }
    }
};

combo_dev_attr_t MIPI_2lane_CHN1_SENSOR_OS05A_10BIT_4M_WDR2TO1_ATTR =
{
    .devno = 1,
    .input_mode = INPUT_MODE_MIPI,
    .data_rate = MIPI_DATA_RATE_X1,
    .img_rect = {16, 252, 2560, 1440},

    {
        .mipi_attr =
        {
            DATA_TYPE_RAW_10BIT,
            HI_MIPI_WDR_MODE_VC,
            {1, 3, -1, -1}
        }
    }
};

combo_dev_attr_t MIPI_4lane_CHN0_SENSOR_OS08A10_10BIT_8M_NOWDR_ATTR =
{
    .devno = 0,
    .input_mode = INPUT_MODE_MIPI,
    .data_rate = MIPI_DATA_RATE_X1,
    .img_rect = {0, 0, 3840, 2160},

    {
        .mipi_attr =
        {
            DATA_TYPE_RAW_10BIT,
            HI_MIPI_WDR_MODE_NONE,
            {0, 1, 2, 3}
        }
    }
};

combo_dev_attr_t MIPI_2lane_CHN0_SENSOR_GC2053_10BIT_2M_NOWDR_ATTR =
{
    .devno = 0,
    .input_mode = INPUT_MODE_MIPI,
    .data_rate = MIPI_DATA_RATE_X1,
    .img_rect = {0, 0, 1920, 1080},

    {
        .mipi_attr =
        {
            DATA_TYPE_RAW_10BIT,
            HI_MIPI_WDR_MODE_NONE,
            {0, 1, -1, -1}
        }
    }
};

combo_dev_attr_t MIPI_4lane_CHN0_SENSOR_OV12870_10BIT_8M_NOWDR_ATTR =
{
    .devno = 0,
    .input_mode = INPUT_MODE_MIPI,
    .data_rate = MIPI_DATA_RATE_X1,
    .img_rect = {0, 0, 3840, 2160},

    {
        .mipi_attr =
        {
            DATA_TYPE_RAW_10BIT,
            HI_MIPI_WDR_MODE_NONE,
            {0, 1, 2, 3}
        }
    }
};

combo_dev_attr_t MIPI_4lane_CHN0_SENSOR_OV12870_10BIT_12M_NOWDR_ATTR =
{
    .devno = 0,
    .input_mode = INPUT_MODE_MIPI,
    .data_rate = MIPI_DATA_RATE_X1,
    .img_rect = {0, 0, 4000, 3000},

    {
        .mipi_attr =
        {
            DATA_TYPE_RAW_10BIT,
            HI_MIPI_WDR_MODE_NONE,
            {0, 1, 2, 3}
        }
    }
};

combo_dev_attr_t MIPI_4lane_CHN0_SENSOR_OV12870_10BIT_2M_NOWDR_ATTR =
{
    .devno = 0,
    .input_mode = INPUT_MODE_MIPI,
    .data_rate = MIPI_DATA_RATE_X1,
    .img_rect = {0, 0, 1920, 1080},

    {
        .mipi_attr =
        {
            DATA_TYPE_RAW_10BIT,
            HI_MIPI_WDR_MODE_NONE,
            {0, 1, 2, 3}
        }
    }
};

combo_dev_attr_t MIPI_4lane_CHN0_SENSOR_OV12870_10BIT_1M_NOWDR_ATTR =
{
    .devno = 0,
    .input_mode = INPUT_MODE_MIPI,
    .data_rate = MIPI_DATA_RATE_X1,
    .img_rect = {0, 0, 1280, 720},

    {
        .mipi_attr =
        {
            DATA_TYPE_RAW_10BIT,
            HI_MIPI_WDR_MODE_NONE,
            {0, 1, 2, 3}
        }
    }
};

combo_dev_attr_t MIPI_2lane_CHN0_SENSOR_OV9284_10BIT_8M_NOWDR_ATTR =
{
    .devno = 0,
    .input_mode = INPUT_MODE_MIPI,
    .data_rate = MIPI_DATA_RATE_X1,
    .img_rect = {0, 0, 1280, 800},

    {
        .mipi_attr =
        {
            DATA_TYPE_RAW_10BIT,
            HI_MIPI_WDR_MODE_NONE,
            {0, 1, -1, -1}
        }
    }
};

combo_dev_attr_t MIPI_4lane_CHN0_SENSOR_IMX415_12BIT_8M_NOWDR_ATTR =
{
    .devno = 0,
    .input_mode = INPUT_MODE_MIPI,
    .data_rate = MIPI_DATA_RATE_X1,
    .img_rect = {0, 0, 3840, 2160},

    {
        .mipi_attr =
        {
            DATA_TYPE_RAW_12BIT,
            HI_MIPI_WDR_MODE_NONE,
            {0, 1, 2, 3}
        }
    }
};

combo_dev_attr_t MIPI_4lane_CHN0_SENSOR_IMX334_12BIT_8M_NOWDR_ATTR =
{
    .devno = 0,
    .input_mode = INPUT_MODE_MIPI,
    .data_rate = MIPI_DATA_RATE_X1,
    .img_rect = {0, 0, 3840, 2160},

    {
        .mipi_attr =
        {
            DATA_TYPE_RAW_12BIT,
            HI_MIPI_WDR_MODE_NONE,
            {0, 1, 2, 3}
        }
    }
};

combo_dev_attr_t MIPI_4lane_CHN0_SENSOR_IMX415_12BIT_8M_WDR2TO1_ATTR =
{
    .devno = 0,
    .input_mode = INPUT_MODE_MIPI,
    .data_rate = MIPI_DATA_RATE_X1,
    .img_rect = {0, 0, 3840, 2160},

    {
        .mipi_attr =
        {
            DATA_TYPE_RAW_12BIT,
            HI_MIPI_WDR_MODE_VC,
            {0, 1, 2, 3}
        }
    }
};


combo_dev_attr_t MIPI_4lane_CHN0_SENSOR_IMX415_12BIT_2M_NOWDR_ATTR =
{
    .devno = 0,
    .input_mode = INPUT_MODE_MIPI,
    .data_rate = MIPI_DATA_RATE_X1,
    .img_rect = {0, 0, 1920, 1080},

    {
        .mipi_attr =
        {
            DATA_TYPE_RAW_12BIT,
            HI_MIPI_WDR_MODE_NONE,
            {0, 1, 2, 3}
        }
    }
};

combo_dev_attr_t MIPI_4lane_CHN0_SENSOR_OV2775_12BIT_2M_NOWDR_ATTR =
{
    .devno = 0,
    .input_mode = INPUT_MODE_MIPI,
    .data_rate = MIPI_DATA_RATE_X1,
    .img_rect = {0, 0, 1920, 1080},

    {
        .mipi_attr =
        {
            DATA_TYPE_RAW_12BIT,
            HI_MIPI_WDR_MODE_NONE,
            {0, 1, 2, 3}
        }
    }
};

combo_dev_attr_t MIPI_4lane_CHN0_SENSOR_OV2775_12BIT_2M_WDR2TO1_ATTR =
{
    .devno = 0,
    .input_mode = INPUT_MODE_MIPI,
    .data_rate = MIPI_DATA_RATE_X1,
    .img_rect = {0, 0, 1920, 1080},

    {
        .mipi_attr =
        {
            DATA_TYPE_RAW_12BIT,
            HI_MIPI_WDR_MODE_VC,
            {0, 1, 2, 3}
        }
    }
};

combo_dev_attr_t MIPI_4lane_CHN0_SENSOR_OV2775_12BIT_2M_BUILTIN_ATTR =
{
    .devno = 0,
    .input_mode = INPUT_MODE_MIPI,
    .data_rate = MIPI_DATA_RATE_X1,
    .img_rect = {0, 0, 1920, 1080},

    {
        .mipi_attr =
        {
            DATA_TYPE_RAW_12BIT,
            HI_MIPI_WDR_MODE_NONE,
            {0, 1, 2, 3}
        }
    }
};

combo_dev_attr_t MIPI_2lane_CHN0_SENSOR_OV2775_12BIT_2M_NOWDR_ATTR =
{
    .devno = 0,
    .input_mode = INPUT_MODE_MIPI,
    .data_rate = MIPI_DATA_RATE_X1,
    .img_rect = {0, 0, 1920, 1080},

    {
        .mipi_attr =
        {
            DATA_TYPE_RAW_12BIT,
            HI_MIPI_WDR_MODE_NONE,
            {0, 2, -1, -1}
        }
    }
};

combo_dev_attr_t MIPI_2lane_CHN1_SENSOR_OV2775_12BIT_2M_NOWDR_ATTR =
{
    .devno = 1,
    .input_mode = INPUT_MODE_MIPI,
    .data_rate = MIPI_DATA_RATE_X1,
    .img_rect = {0, 0, 1920, 1080},

    {
        .mipi_attr =
        {
            DATA_TYPE_RAW_12BIT,
            HI_MIPI_WDR_MODE_NONE,
            {1, 3, -1, -1}
        }
    }
};

combo_dev_attr_t MIPI_2lane_CHN0_SENSOR_OV2775_12BIT_2M_WDR2TO1_ATTR =
{
    .devno = 0,
    .input_mode = INPUT_MODE_MIPI,
    .data_rate = MIPI_DATA_RATE_X1,
    .img_rect = {0, 0, 1920, 1080},

    {
        .mipi_attr =
        {
            DATA_TYPE_RAW_12BIT,
            HI_MIPI_WDR_MODE_VC,
            {0, 2, -1, -1}
        }
    }
};

combo_dev_attr_t MIPI_2lane_CHN1_SENSOR_OV2775_12BIT_2M_WDR2TO1_ATTR =
{
    .devno = 1,
    .input_mode = INPUT_MODE_MIPI,
    .data_rate = MIPI_DATA_RATE_X1,
    .img_rect = {0, 0, 1920, 1080},

    {
        .mipi_attr =
        {
            DATA_TYPE_RAW_12BIT,
            HI_MIPI_WDR_MODE_VC,
            {1, 3, -1, -1}
        }
    }
};

VI_DEV_ATTR_S DEV_ATTR_IMX327_2M_BASE =
{
    VI_MODE_MIPI,
    VI_WORK_MODE_1Multiplex,
    {0xFFF00000,    0x0},
    VI_SCAN_PROGRESSIVE,
    {-1, -1, -1, -1},
    VI_DATA_SEQ_YUYV,

    {
    /*port_vsync   port_vsync_neg     port_hsync        port_hsync_neg        */
    VI_VSYNC_PULSE, VI_VSYNC_NEG_LOW, VI_HSYNC_VALID_SINGNAL,VI_HSYNC_NEG_HIGH,VI_VSYNC_VALID_SINGAL,VI_VSYNC_VALID_NEG_HIGH,

    /*hsync_hfb    hsync_act    hsync_hhb*/
    {0,            1280,        0,
    /*vsync0_vhb vsync0_act vsync0_hhb*/
     0,            720,        0,
    /*vsync1_vhb vsync1_act vsync1_hhb*/
     0,            0,            0}
    },
    VI_DATA_TYPE_RGB,
    HI_FALSE,
    {1920, 1080},
    {
        {
            {1920 , 1080},

        },
        {
            VI_REPHASE_MODE_NONE,
            VI_REPHASE_MODE_NONE
        }
    },
    {
        WDR_MODE_NONE,
        1080
    },
    DATA_RATE_X1
};

VI_DEV_ATTR_S DEV_ATTR_IMX390_2M_BASE =
{
    VI_MODE_MIPI,
    VI_WORK_MODE_1Multiplex,
    {0xFFF00000,    0x0},
    VI_SCAN_PROGRESSIVE,
    {-1, -1, -1, -1},
    VI_DATA_SEQ_YUYV,

    {
    /*port_vsync   port_vsync_neg     port_hsync        port_hsync_neg        */
    VI_VSYNC_PULSE, VI_VSYNC_NEG_LOW, VI_HSYNC_VALID_SINGNAL,VI_HSYNC_NEG_HIGH,VI_VSYNC_VALID_SINGAL,VI_VSYNC_VALID_NEG_HIGH,

    /*hsync_hfb    hsync_act    hsync_hhb*/
    {0,            1280,        0,
    /*vsync0_vhb vsync0_act vsync0_hhb*/
     0,            720,        0,
    /*vsync1_vhb vsync1_act vsync1_hhb*/
     0,            0,            0}
    },
    VI_DATA_TYPE_RGB,
    HI_FALSE,
    {1920, 1080},
    {
        {
            {1920 , 1080},

        },
        {
            VI_REPHASE_MODE_NONE,
            VI_REPHASE_MODE_NONE
        }
    },
    {
        WDR_MODE_NONE,
        1080
    },
    DATA_RATE_X1
};

VI_DEV_ATTR_S DEV_ATTR_IMX290_2M_BASE =
{
    VI_MODE_MIPI,
    VI_WORK_MODE_1Multiplex,
    {0xFFF00000,    0x0},
    VI_SCAN_PROGRESSIVE,
    {-1, -1, -1, -1},
    VI_DATA_SEQ_YUYV,

    {
    /*port_vsync   port_vsync_neg     port_hsync        port_hsync_neg        */
    VI_VSYNC_PULSE, VI_VSYNC_NEG_LOW, VI_HSYNC_VALID_SINGNAL,VI_HSYNC_NEG_HIGH,VI_VSYNC_VALID_SINGAL,VI_VSYNC_VALID_NEG_HIGH,

    /*hsync_hfb    hsync_act    hsync_hhb*/
    {0,            1280,        0,
    /*vsync0_vhb vsync0_act vsync0_hhb*/
     0,            720,        0,
    /*vsync1_vhb vsync1_act vsync1_hhb*/
     0,            0,            0}
    },
    VI_DATA_TYPE_RGB,
    HI_FALSE,
    {1920, 1080},
    {
        {
            {1920 , 1080},

        },
        {
            VI_REPHASE_MODE_NONE,
            VI_REPHASE_MODE_NONE
        }
    },
    {
        WDR_MODE_NONE,
        1080
    },
    DATA_RATE_X1
};



VI_DEV_ATTR_S DEV_ATTR_IMX307_2M_BASE =
{
    VI_MODE_MIPI,
    VI_WORK_MODE_1Multiplex,
    {0xFFF00000,    0x0},
    VI_SCAN_PROGRESSIVE,
    {-1, -1, -1, -1},
    VI_DATA_SEQ_YUYV,

    {
    /*port_vsync   port_vsync_neg     port_hsync        port_hsync_neg        */
    VI_VSYNC_PULSE, VI_VSYNC_NEG_LOW, VI_HSYNC_VALID_SINGNAL,VI_HSYNC_NEG_HIGH,VI_VSYNC_VALID_SINGAL,VI_VSYNC_VALID_NEG_HIGH,

    /*hsync_hfb    hsync_act    hsync_hhb*/
    {0,            1280,        0,
    /*vsync0_vhb vsync0_act vsync0_hhb*/
     0,            720,        0,
    /*vsync1_vhb vsync1_act vsync1_hhb*/
     0,            0,            0}
    },
    VI_DATA_TYPE_RGB,
    HI_FALSE,
    {1920, 1080},
    {
        {
            {1920 , 1080},

        },
        {
            VI_REPHASE_MODE_NONE,
            VI_REPHASE_MODE_NONE
        }
    },
    {
        WDR_MODE_NONE,
        1080
    },
    DATA_RATE_X1
};

VI_DEV_ATTR_S DEV_ATTR_PS5260_2M_BASE =
{
    VI_MODE_MIPI,
    VI_WORK_MODE_1Multiplex,
    {0xFFF00000,    0x0},
    VI_SCAN_PROGRESSIVE,
    {-1, -1, -1, -1},
    VI_DATA_SEQ_YUYV,

    {
    /*port_vsync   port_vsync_neg     port_hsync        port_hsync_neg        */
    VI_VSYNC_PULSE, VI_VSYNC_NEG_LOW, VI_HSYNC_VALID_SINGNAL,VI_HSYNC_NEG_HIGH,VI_VSYNC_VALID_SINGAL,VI_VSYNC_VALID_NEG_HIGH,

    /*hsync_hfb    hsync_act    hsync_hhb*/
    {0,            1280,        0,
    /*vsync0_vhb vsync0_act vsync0_hhb*/
     0,            720,        0,
    /*vsync1_vhb vsync1_act vsync1_hhb*/
     0,            0,            0}
    },
    VI_DATA_TYPE_RGB,
    HI_FALSE,
    {1920, 1080},
    {
        {
            {1920 , 1080},

        },
        {
            VI_REPHASE_MODE_NONE,
            VI_REPHASE_MODE_NONE
        }
    },
    {
        WDR_MODE_NONE,
        1080
    },
    DATA_RATE_X1
};

VI_DEV_ATTR_S DEV_ATTR_MN34220_2M_BASE =
{
    VI_MODE_LVDS,
    VI_WORK_MODE_1Multiplex,
    {0xFFF00000,    0x0},
    VI_SCAN_PROGRESSIVE,
    {-1, -1, -1, -1},
    VI_DATA_SEQ_YUYV,

    {
    /*port_vsync   port_vsync_neg     port_hsync        port_hsync_neg        */
    VI_VSYNC_PULSE, VI_VSYNC_NEG_LOW, VI_HSYNC_VALID_SINGNAL,VI_HSYNC_NEG_HIGH,VI_VSYNC_VALID_SINGAL,VI_VSYNC_VALID_NEG_HIGH,

    /*hsync_hfb    hsync_act    hsync_hhb*/
    {0,            1280,        0,
    /*vsync0_vhb vsync0_act vsync0_hhb*/
     0,            720,        0,
    /*vsync1_vhb vsync1_act vsync1_hhb*/
     0,            0,            0}
    },
    VI_DATA_TYPE_RGB,
    HI_FALSE,
    {1920 , 1080},
    {
        {
            {1920 , 1080},
        },
        {
            VI_REPHASE_MODE_NONE,
            VI_REPHASE_MODE_NONE
        }
    },
    {
        WDR_MODE_NONE,
        1080
    },
    DATA_RATE_X1
};

VI_DEV_ATTR_S DEV_ATTR_IMX335_5M_BASE =
{
    VI_MODE_MIPI,
    VI_WORK_MODE_1Multiplex,
    {0xFFF00000,    0x0},
    VI_SCAN_PROGRESSIVE,
    {-1, -1, -1, -1},
    VI_DATA_SEQ_YUYV,

    {
    /*port_vsync   port_vsync_neg     port_hsync        port_hsync_neg        */
    VI_VSYNC_PULSE, VI_VSYNC_NEG_LOW, VI_HSYNC_VALID_SINGNAL,VI_HSYNC_NEG_HIGH,VI_VSYNC_VALID_SINGAL,VI_VSYNC_VALID_NEG_HIGH,

    /*hsync_hfb    hsync_act    hsync_hhb*/
    {0,            1280,        0,
    /*vsync0_vhb vsync0_act vsync0_hhb*/
     0,            720,        0,
    /*vsync1_vhb vsync1_act vsync1_hhb*/
     0,            0,            0}
    },
    VI_DATA_TYPE_RGB,
    HI_FALSE,
    {2592 , 1944},
    {
        {
            {2592 , 1944},
        },
        {
            VI_REPHASE_MODE_NONE,
            VI_REPHASE_MODE_NONE
        }
    },
    {
        WDR_MODE_NONE,
        1944
    },
    DATA_RATE_X1
};

VI_DEV_ATTR_S DEV_ATTR_IMX335_4M_BASE =
{
    VI_MODE_MIPI,
    VI_WORK_MODE_1Multiplex,
    {0xFFF00000,    0x0},
    VI_SCAN_PROGRESSIVE,
    {-1, -1, -1, -1},
    VI_DATA_SEQ_YUYV,

    {
    /*port_vsync   port_vsync_neg     port_hsync        port_hsync_neg        */
    VI_VSYNC_PULSE, VI_VSYNC_NEG_LOW, VI_HSYNC_VALID_SINGNAL,VI_HSYNC_NEG_HIGH,VI_VSYNC_VALID_SINGAL,VI_VSYNC_VALID_NEG_HIGH,

    /*hsync_hfb    hsync_act    hsync_hhb*/
    {0,            1280,        0,
    /*vsync0_vhb vsync0_act vsync0_hhb*/
     0,            720,        0,
    /*vsync1_vhb vsync1_act vsync1_hhb*/
     0,            0,            0}
    },
    VI_DATA_TYPE_RGB,
    HI_FALSE,
    {2592 , 1536},
    {
        {
            {2592 , 1536},
        },
        {
            VI_REPHASE_MODE_NONE,
            VI_REPHASE_MODE_NONE
        }
    },
    {
        WDR_MODE_NONE,
        1536
    },
    DATA_RATE_X1
};

VI_DEV_ATTR_S DEV_ATTR_SC4210_3M_BASE =
{
    VI_MODE_MIPI,
    VI_WORK_MODE_1Multiplex,
    {0xFFF00000,    0x0},
    VI_SCAN_PROGRESSIVE,
    {-1, -1, -1, -1},
    VI_DATA_SEQ_YUYV,

    {
    /*port_vsync   port_vsync_neg     port_hsync        port_hsync_neg        */
    VI_VSYNC_PULSE, VI_VSYNC_NEG_LOW, VI_HSYNC_VALID_SINGNAL,VI_HSYNC_NEG_HIGH,VI_VSYNC_VALID_SINGAL,VI_VSYNC_VALID_NEG_HIGH,

    /*hsync_hfb    hsync_act    hsync_hhb*/
    {0,            1280,        0,
    /*vsync0_vhb vsync0_act vsync0_hhb*/
     0,            720,        0,
    /*vsync1_vhb vsync1_act vsync1_hhb*/
     0,            0,            0}
    },
    VI_DATA_TYPE_RGB,
    HI_FALSE,
    {2560 , 1440},
    {
        {
            {2560 , 1440},
        },
        {
            VI_REPHASE_MODE_NONE,
            VI_REPHASE_MODE_NONE
        }
    },
    {
        WDR_MODE_NONE,
        1440
    },
    DATA_RATE_X1
};


VI_DEV_ATTR_S DEV_ATTR_IMX458_8M_BASE =
{
    VI_MODE_MIPI,
    VI_WORK_MODE_1Multiplex,
    {0xFFC00000,    0x0},
    VI_SCAN_PROGRESSIVE,
    { -1, -1, -1, -1},
    VI_DATA_SEQ_YUYV,

    {
        /*port_vsync   port_vsync_neg     port_hsync        port_hsync_neg        */
        VI_VSYNC_PULSE, VI_VSYNC_NEG_LOW, VI_HSYNC_VALID_SINGNAL, VI_HSYNC_NEG_HIGH, VI_VSYNC_VALID_SINGAL, VI_VSYNC_VALID_NEG_HIGH,

        /*hsync_hfb    hsync_act    hsync_hhb*/
        {
            0,            1280,        0,
            /*vsync0_vhb vsync0_act vsync0_hhb*/
            0,            720,        0,
            /*vsync1_vhb vsync1_act vsync1_hhb*/
            0,            0,            0
        }
    },
    VI_DATA_TYPE_RGB,
    HI_FALSE,
    {3840 , 2160},
    {
        {
            {3840 , 2160},
        },
        {
            VI_REPHASE_MODE_NONE,
            VI_REPHASE_MODE_NONE
        }
    },
    {
        WDR_MODE_NONE,
        2160
    },
    DATA_RATE_X1
};

VI_DEV_ATTR_S DEV_ATTR_IMX458_12M_BASE =
{
    VI_MODE_MIPI,
    VI_WORK_MODE_1Multiplex,
    {0xFFC00000,    0x0},
    VI_SCAN_PROGRESSIVE,
    { -1, -1, -1, -1},
    VI_DATA_SEQ_YUYV,

    {
        /*port_vsync   port_vsync_neg     port_hsync        port_hsync_neg        */
        VI_VSYNC_PULSE, VI_VSYNC_NEG_LOW, VI_HSYNC_VALID_SINGNAL, VI_HSYNC_NEG_HIGH, VI_VSYNC_VALID_SINGAL, VI_VSYNC_VALID_NEG_HIGH,

        /*hsync_hfb    hsync_act    hsync_hhb*/
        {
            0,            1280,        0,
            /*vsync0_vhb vsync0_act vsync0_hhb*/
            0,            720,        0,
            /*vsync1_vhb vsync1_act vsync1_hhb*/
            0,            0,            0
        }
    },
    VI_DATA_TYPE_RGB,
    HI_FALSE,
    {4000 , 3000},
    {
        {
            {4000 , 3000},
        },
        {
            VI_REPHASE_MODE_NONE,
            VI_REPHASE_MODE_NONE
        }
    },
    {
        WDR_MODE_NONE,
        3000
    },
    DATA_RATE_X1
};

VI_DEV_ATTR_S DEV_ATTR_IMX458_4M_BASE =
{
    VI_MODE_MIPI,
    VI_WORK_MODE_1Multiplex,
    {0xFFC00000,    0x0},
    VI_SCAN_PROGRESSIVE,
    { -1, -1, -1, -1},
    VI_DATA_SEQ_YUYV,

    {
        /*port_vsync   port_vsync_neg     port_hsync        port_hsync_neg        */
        VI_VSYNC_PULSE, VI_VSYNC_NEG_LOW, VI_HSYNC_VALID_SINGNAL, VI_HSYNC_NEG_HIGH, VI_VSYNC_VALID_SINGAL, VI_VSYNC_VALID_NEG_HIGH,

        /*hsync_hfb    hsync_act    hsync_hhb*/
        {
            0,            1280,        0,
            /*vsync0_vhb vsync0_act vsync0_hhb*/
            0,            720,        0,
            /*vsync1_vhb vsync1_act vsync1_hhb*/
            0,            0,            0
        }
    },
    VI_DATA_TYPE_RGB,
    HI_FALSE,
    {2716 , 1524},
    {
        {
            {2716 , 1524},
        },
        {
            VI_REPHASE_MODE_NONE,
            VI_REPHASE_MODE_NONE
        }
    },
    {
        WDR_MODE_NONE,
        1524
    },
    DATA_RATE_X1
};

VI_DEV_ATTR_S DEV_ATTR_IMX458_2M_BASE =
{
    VI_MODE_MIPI,
    VI_WORK_MODE_1Multiplex,
    {0xFFC00000,    0x0},
    VI_SCAN_PROGRESSIVE,
    { -1, -1, -1, -1},
    VI_DATA_SEQ_YUYV,

    {
        /*port_vsync   port_vsync_neg     port_hsync        port_hsync_neg        */
        VI_VSYNC_PULSE, VI_VSYNC_NEG_LOW, VI_HSYNC_VALID_SINGNAL, VI_HSYNC_NEG_HIGH, VI_VSYNC_VALID_SINGAL, VI_VSYNC_VALID_NEG_HIGH,

        /*hsync_hfb    hsync_act    hsync_hhb*/
        {
            0,            1280,        0,
            /*vsync0_vhb vsync0_act vsync0_hhb*/
            0,            720,        0,
            /*vsync1_vhb vsync1_act vsync1_hhb*/
            0,            0,            0
        }
    },
    VI_DATA_TYPE_RGB,
    HI_FALSE,
    {1920 , 1080},
    {
        {
            {1920 , 1080},
        },
        {
            VI_REPHASE_MODE_NONE,
            VI_REPHASE_MODE_NONE
        }
    },
    {
        WDR_MODE_NONE,
        1080
    },
    DATA_RATE_X1
};

VI_DEV_ATTR_S DEV_ATTR_IMX458_1M_BASE =
{
    VI_MODE_MIPI,
    VI_WORK_MODE_1Multiplex,
    {0xFFC00000,    0x0},
    VI_SCAN_PROGRESSIVE,
    { -1, -1, -1, -1},
    VI_DATA_SEQ_YUYV,

    {
        /*port_vsync   port_vsync_neg     port_hsync        port_hsync_neg        */
        VI_VSYNC_PULSE, VI_VSYNC_NEG_LOW, VI_HSYNC_VALID_SINGNAL, VI_HSYNC_NEG_HIGH, VI_VSYNC_VALID_SINGAL, VI_VSYNC_VALID_NEG_HIGH,

        /*hsync_hfb    hsync_act    hsync_hhb*/
        {
            0,            1280,        0,
            /*vsync0_vhb vsync0_act vsync0_hhb*/
            0,            720,        0,
            /*vsync1_vhb vsync1_act vsync1_hhb*/
            0,            0,            0
        }
    },
    VI_DATA_TYPE_RGB,
    HI_FALSE,
    {1280 , 720},
    {
        {
            {1280 , 720},
        },
        {
            VI_REPHASE_MODE_NONE,
            VI_REPHASE_MODE_NONE
        }
    },
    {
        WDR_MODE_NONE,
        720
    },
    DATA_RATE_X1
};

VI_DEV_ATTR_S DEV_ATTR_OS04B10_4M_BASE =
{
    VI_MODE_MIPI,
    VI_WORK_MODE_1Multiplex,
    {0xFFF00000,    0x0},
    VI_SCAN_PROGRESSIVE,
    {-1, -1, -1, -1},
    VI_DATA_SEQ_YUYV,

    {
    /*port_vsync   port_vsync_neg     port_hsync        port_hsync_neg        */
    VI_VSYNC_PULSE, VI_VSYNC_NEG_LOW, VI_HSYNC_VALID_SINGNAL,VI_HSYNC_NEG_HIGH,VI_VSYNC_VALID_SINGAL,VI_VSYNC_VALID_NEG_HIGH,

    /*hsync_hfb    hsync_act    hsync_hhb*/
    {0,            1280,        0,
    /*vsync0_vhb vsync0_act vsync0_hhb*/
     0,            720,        0,
    /*vsync1_vhb vsync1_act vsync1_hhb*/
     0,            0,            0}
    },
    VI_DATA_TYPE_RGB,
    HI_FALSE,
    {2560 , 1440},
    {
        {
            {2560 , 1440},
        },
        {
            VI_REPHASE_MODE_NONE,
            VI_REPHASE_MODE_NONE
        }
    },
    {
        WDR_MODE_NONE,
        1440
    },
    DATA_RATE_X1
};

VI_DEV_ATTR_S DEV_ATTR_OS05A_4M_BASE =
{
    VI_MODE_MIPI,
    VI_WORK_MODE_1Multiplex,
    {0xFFF00000,    0x0},
    VI_SCAN_PROGRESSIVE,
    {-1, -1, -1, -1},
    VI_DATA_SEQ_YUYV,

    {
    /*port_vsync   port_vsync_neg     port_hsync        port_hsync_neg        */
    VI_VSYNC_PULSE, VI_VSYNC_NEG_LOW, VI_HSYNC_VALID_SINGNAL,VI_HSYNC_NEG_HIGH,VI_VSYNC_VALID_SINGAL,VI_VSYNC_VALID_NEG_HIGH,

    /*hsync_hfb    hsync_act    hsync_hhb*/
    {0,            1280,        0,
    /*vsync0_vhb vsync0_act vsync0_hhb*/
     0,            720,        0,
    /*vsync1_vhb vsync1_act vsync1_hhb*/
     0,            0,            0}
    },
    VI_DATA_TYPE_RGB,
    HI_FALSE,
    {2688 , 1536},
    {
        {
            {2688 , 1536},
        },
        {
            VI_REPHASE_MODE_NONE,
            VI_REPHASE_MODE_NONE
        }
    },
    {
        WDR_MODE_NONE,
        1536
    },
    DATA_RATE_X1
};

VI_DEV_ATTR_S DEV_ATTR_OS05A_2L_1M_BASE =
{
    VI_MODE_MIPI,
    VI_WORK_MODE_1Multiplex,
    {0xFFC00000,    0x0},
    VI_SCAN_PROGRESSIVE,
    {-1, -1, -1, -1},
    VI_DATA_SEQ_YUYV,

    {
    /*port_vsync   port_vsync_neg     port_hsync        port_hsync_neg        */
    VI_VSYNC_PULSE, VI_VSYNC_NEG_LOW, VI_HSYNC_VALID_SINGNAL,VI_HSYNC_NEG_HIGH,VI_VSYNC_VALID_SINGAL,VI_VSYNC_VALID_NEG_HIGH,

    /*hsync_hfb    hsync_act    hsync_hhb*/
    {0,            1280,        0,
    /*vsync0_vhb vsync0_act vsync0_hhb*/
     0,            720,        0,
    /*vsync1_vhb vsync1_act vsync1_hhb*/
     0,            0,            0}
    },
    VI_DATA_TYPE_RGB,
    HI_FALSE,
    {1280 , 720},
    {
        {
            {1280 , 720},
        },
        {
            VI_REPHASE_MODE_NONE,
            VI_REPHASE_MODE_NONE
        }
    },
    {
        WDR_MODE_NONE,
        720
    },
    DATA_RATE_X1
};

VI_DEV_ATTR_S DEV_ATTR_OS05A_2L_4M_BASE =
{
    VI_MODE_MIPI,
    VI_WORK_MODE_1Multiplex,
    {0xFFC00000,    0x0},
    VI_SCAN_PROGRESSIVE,
    {-1, -1, -1, -1},
    VI_DATA_SEQ_YUYV,

    {
    /*port_vsync   port_vsync_neg     port_hsync        port_hsync_neg        */
    VI_VSYNC_PULSE, VI_VSYNC_NEG_LOW, VI_HSYNC_VALID_SINGNAL,VI_HSYNC_NEG_HIGH,VI_VSYNC_VALID_SINGAL,VI_VSYNC_VALID_NEG_HIGH,

    /*hsync_hfb    hsync_act    hsync_hhb*/
    {0,            1280,        0,
    /*vsync0_vhb vsync0_act vsync0_hhb*/
     0,            720,        0,
    /*vsync1_vhb vsync1_act vsync1_hhb*/
     0,            0,            0}
    },
    VI_DATA_TYPE_RGB,
    HI_FALSE,
    {2560 , 1440},
    {
        {
            {2560 , 1440},
        },
        {
            VI_REPHASE_MODE_NONE,
            VI_REPHASE_MODE_NONE
        }
    },
    {
        WDR_MODE_NONE,
        1440
    },
    DATA_RATE_X1
};

VI_DEV_ATTR_S DEV_ATTR_OS08A10_8M_BASE =
{
    VI_MODE_MIPI,
    VI_WORK_MODE_1Multiplex,
    {0xFFC00000,    0x0},
    VI_SCAN_PROGRESSIVE,
    { -1, -1, -1, -1},
    VI_DATA_SEQ_YUYV,

    {
        /*port_vsync   port_vsync_neg     port_hsync        port_hsync_neg        */
        VI_VSYNC_PULSE, VI_VSYNC_NEG_LOW, VI_HSYNC_VALID_SINGNAL, VI_HSYNC_NEG_HIGH, VI_VSYNC_VALID_SINGAL, VI_VSYNC_VALID_NEG_HIGH,

        /*hsync_hfb    hsync_act    hsync_hhb*/
        {
            0,            1280,        0,
            /*vsync0_vhb vsync0_act vsync0_hhb*/
            0,            720,        0,
            /*vsync1_vhb vsync1_act vsync1_hhb*/
            0,            0,            0
        }
    },
    VI_DATA_TYPE_RGB,
    HI_FALSE,
    {3840 , 2160},
    {
        {
            {3840 , 2160},
        },
        {
            VI_REPHASE_MODE_NONE,
            VI_REPHASE_MODE_NONE
        }
    },
    {
        WDR_MODE_NONE,
        2160
    },
    DATA_RATE_X1
};

VI_DEV_ATTR_S DEV_ATTR_GC2053_2M_BASE =
{
    VI_MODE_MIPI,
    VI_WORK_MODE_1Multiplex,
    {0xFFC00000,    0x0},
    VI_SCAN_PROGRESSIVE,
    { -1, -1, -1, -1},
    VI_DATA_SEQ_YUYV,

    {
        /*port_vsync   port_vsync_neg     port_hsync        port_hsync_neg        */
        VI_VSYNC_PULSE, VI_VSYNC_NEG_LOW, VI_HSYNC_VALID_SINGNAL, VI_HSYNC_NEG_HIGH, VI_VSYNC_VALID_SINGAL, VI_VSYNC_VALID_NEG_HIGH,

        /*hsync_hfb    hsync_act    hsync_hhb*/
        {
            0,            1280,        0,
            /*vsync0_vhb vsync0_act vsync0_hhb*/
            0,            720,        0,
            /*vsync1_vhb vsync1_act vsync1_hhb*/
            0,            0,            0
        }
    },
    VI_DATA_TYPE_RGB,
    HI_FALSE,
    {1920 , 1080},
    {
        {
            {1920 , 1080},
        },
        {
            VI_REPHASE_MODE_NONE,
            VI_REPHASE_MODE_NONE
        }
    },
    {
        WDR_MODE_NONE,
        1080
    },
    DATA_RATE_X1
};

VI_DEV_ATTR_S DEV_ATTR_OV12870_8M_BASE =
{
    VI_MODE_MIPI,
    VI_WORK_MODE_1Multiplex,
    {0xFFC00000,    0x0},
    VI_SCAN_PROGRESSIVE,
    { -1, -1, -1, -1},
    VI_DATA_SEQ_YUYV,

    {
        /*port_vsync   port_vsync_neg     port_hsync        port_hsync_neg        */
        VI_VSYNC_PULSE, VI_VSYNC_NEG_LOW, VI_HSYNC_VALID_SINGNAL, VI_HSYNC_NEG_HIGH, VI_VSYNC_VALID_SINGAL, VI_VSYNC_VALID_NEG_HIGH,

        /*hsync_hfb    hsync_act    hsync_hhb*/
        {
            0,            1280,        0,
            /*vsync0_vhb vsync0_act vsync0_hhb*/
            0,            720,        0,
            /*vsync1_vhb vsync1_act vsync1_hhb*/
            0,            0,            0
        }
    },
    VI_DATA_TYPE_RGB,
    HI_FALSE,
    {3840 , 2160},
    {
        {
            {3840 , 2160},
        },
        {
            VI_REPHASE_MODE_NONE,
            VI_REPHASE_MODE_NONE
        }
    },
    {
        WDR_MODE_NONE,
        2160
    },
    DATA_RATE_X1
};

VI_DEV_ATTR_S DEV_ATTR_OV12870_12M_BASE =
{
    VI_MODE_MIPI,
    VI_WORK_MODE_1Multiplex,
    {0xFFC00000,    0x0},
    VI_SCAN_PROGRESSIVE,
    { -1, -1, -1, -1},
    VI_DATA_SEQ_YUYV,

    {
        /*port_vsync   port_vsync_neg     port_hsync        port_hsync_neg        */
        VI_VSYNC_PULSE, VI_VSYNC_NEG_LOW, VI_HSYNC_VALID_SINGNAL, VI_HSYNC_NEG_HIGH, VI_VSYNC_VALID_SINGAL, VI_VSYNC_VALID_NEG_HIGH,

        /*hsync_hfb    hsync_act    hsync_hhb*/
        {
            0,            1280,        0,
            /*vsync0_vhb vsync0_act vsync0_hhb*/
            0,            720,        0,
            /*vsync1_vhb vsync1_act vsync1_hhb*/
            0,            0,            0
        }
    },
    VI_DATA_TYPE_RGB,
    HI_FALSE,
    {4000 , 3000},
    {
        {
            {4000 , 3000},
        },
        {
            VI_REPHASE_MODE_NONE,
            VI_REPHASE_MODE_NONE
        }
    },
    {
        WDR_MODE_NONE,
        3000
    },
    DATA_RATE_X1
};

VI_DEV_ATTR_S DEV_ATTR_OV12870_2M_BASE =
{
    VI_MODE_MIPI,
    VI_WORK_MODE_1Multiplex,
    {0xFFC00000,    0x0},
    VI_SCAN_PROGRESSIVE,
    { -1, -1, -1, -1},
    VI_DATA_SEQ_YUYV,

    {
        /*port_vsync   port_vsync_neg     port_hsync        port_hsync_neg        */
        VI_VSYNC_PULSE, VI_VSYNC_NEG_LOW, VI_HSYNC_VALID_SINGNAL, VI_HSYNC_NEG_HIGH, VI_VSYNC_VALID_SINGAL, VI_VSYNC_VALID_NEG_HIGH,

        /*hsync_hfb    hsync_act    hsync_hhb*/
        {
            0,            1280,        0,
            /*vsync0_vhb vsync0_act vsync0_hhb*/
            0,            720,        0,
            /*vsync1_vhb vsync1_act vsync1_hhb*/
            0,            0,            0
        }
    },
    VI_DATA_TYPE_RGB,
    HI_FALSE,
    {1920 , 1080},
    {
        {
            {1920 , 1080},
        },
        {
            VI_REPHASE_MODE_NONE,
            VI_REPHASE_MODE_NONE
        }
    },
    {
        WDR_MODE_NONE,
        1080
    },
    DATA_RATE_X1
};

VI_DEV_ATTR_S DEV_ATTR_OV12870_1M_BASE =
{
    VI_MODE_MIPI,
    VI_WORK_MODE_1Multiplex,
    {0xFFC00000,    0x0},
    VI_SCAN_PROGRESSIVE,
    { -1, -1, -1, -1},
    VI_DATA_SEQ_YUYV,

    {
        /*port_vsync   port_vsync_neg     port_hsync        port_hsync_neg        */
        VI_VSYNC_PULSE, VI_VSYNC_NEG_LOW, VI_HSYNC_VALID_SINGNAL, VI_HSYNC_NEG_HIGH, VI_VSYNC_VALID_SINGAL, VI_VSYNC_VALID_NEG_HIGH,

        /*hsync_hfb    hsync_act    hsync_hhb*/
        {
            0,            1280,        0,
            /*vsync0_vhb vsync0_act vsync0_hhb*/
            0,            720,        0,
            /*vsync1_vhb vsync1_act vsync1_hhb*/
            0,            0,            0
        }
    },
    VI_DATA_TYPE_RGB,
    HI_FALSE,
    {1280 , 720},
    {
        {
            {1280 , 720},
        },
        {
            VI_REPHASE_MODE_NONE,
            VI_REPHASE_MODE_NONE
        }
    },
    {
        WDR_MODE_NONE,
        720
    },
    DATA_RATE_X1
};

VI_DEV_ATTR_S DEV_ATTR_OV9284_1M_BASE =
{
    VI_MODE_MIPI,
    VI_WORK_MODE_1Multiplex,
    {0xFFC00000,    0x0},
    VI_SCAN_PROGRESSIVE,
    { -1, -1, -1, -1},
    VI_DATA_SEQ_YUYV,

    {
        /*port_vsync   port_vsync_neg     port_hsync        port_hsync_neg        */
        VI_VSYNC_PULSE, VI_VSYNC_NEG_LOW, VI_HSYNC_VALID_SINGNAL, VI_HSYNC_NEG_HIGH, VI_VSYNC_VALID_SINGAL, VI_VSYNC_VALID_NEG_HIGH,

        /*hsync_hfb    hsync_act    hsync_hhb*/
        {
            0,            1280,        0,
            /*vsync0_vhb vsync0_act vsync0_hhb*/
            0,            720,        0,
            /*vsync1_vhb vsync1_act vsync1_hhb*/
            0,            0,            0
        }
    },
    VI_DATA_TYPE_RGB,
    HI_FALSE,
    {1280 , 800},
    {
        {
            {1280 , 800},
        },
        {
            VI_REPHASE_MODE_NONE,
            VI_REPHASE_MODE_NONE
        }
    },
    {
        WDR_MODE_NONE,
        800
    },
    DATA_RATE_X1
};


VI_DEV_ATTR_S DEV_ATTR_IMX334_8M_BASE =
{
    VI_MODE_MIPI,
    VI_WORK_MODE_1Multiplex,
    {0xFFF00000,    0x0},
    VI_SCAN_PROGRESSIVE,
    { -1, -1, -1, -1},
    VI_DATA_SEQ_YUYV,

    {
        /*port_vsync   port_vsync_neg     port_hsync        port_hsync_neg        */
        VI_VSYNC_PULSE, VI_VSYNC_NEG_LOW, VI_HSYNC_VALID_SINGNAL, VI_HSYNC_NEG_HIGH, VI_VSYNC_VALID_SINGAL, VI_VSYNC_VALID_NEG_HIGH,

        /*hsync_hfb    hsync_act    hsync_hhb*/
        {
            0,            1280,        0,
            /*vsync0_vhb vsync0_act vsync0_hhb*/
            0,            720,        0,
            /*vsync1_vhb vsync1_act vsync1_hhb*/
            0,            0,            0
        }
    },
    VI_DATA_TYPE_RGB,
    HI_FALSE,
    {3840 , 2160},
    {
        {
            {3840 , 2160},
        },
        {
            VI_REPHASE_MODE_NONE,
            VI_REPHASE_MODE_NONE
        }
    },
    {
        WDR_MODE_NONE,
        2160
    },
    DATA_RATE_X1
};


VI_DEV_ATTR_S DEV_ATTR_IMX415_8M_BASE =
{
    VI_MODE_MIPI,
    VI_WORK_MODE_1Multiplex,
    {0xFFF00000,    0x0},
    VI_SCAN_PROGRESSIVE,
    { -1, -1, -1, -1},
    VI_DATA_SEQ_YUYV,

    {
        /*port_vsync   port_vsync_neg     port_hsync        port_hsync_neg        */
        VI_VSYNC_PULSE, VI_VSYNC_NEG_LOW, VI_HSYNC_VALID_SINGNAL, VI_HSYNC_NEG_HIGH, VI_VSYNC_VALID_SINGAL, VI_VSYNC_VALID_NEG_HIGH,

        /*hsync_hfb    hsync_act    hsync_hhb*/
        {
            0,            1280,        0,
            /*vsync0_vhb vsync0_act vsync0_hhb*/
            0,            720,        0,
            /*vsync1_vhb vsync1_act vsync1_hhb*/
            0,            0,            0
        }
    },
    VI_DATA_TYPE_RGB,
    HI_FALSE,
    {3840 , 2160},
    {
        {
            {3840 , 2160},
        },
        {
            VI_REPHASE_MODE_NONE,
            VI_REPHASE_MODE_NONE
        }
    },
    {
        WDR_MODE_NONE,
        2160
    },
    DATA_RATE_X1
};

VI_DEV_ATTR_S DEV_ATTR_IMX415_2M_BASE =
{
    VI_MODE_MIPI,
    VI_WORK_MODE_1Multiplex,
    {0xFFF00000,    0x0},
    VI_SCAN_PROGRESSIVE,
    { -1, -1, -1, -1},
    VI_DATA_SEQ_YUYV,

    {
        /*port_vsync   port_vsync_neg     port_hsync        port_hsync_neg        */
        VI_VSYNC_PULSE, VI_VSYNC_NEG_LOW, VI_HSYNC_VALID_SINGNAL, VI_HSYNC_NEG_HIGH, VI_VSYNC_VALID_SINGAL, VI_VSYNC_VALID_NEG_HIGH,

        /*hsync_hfb    hsync_act    hsync_hhb*/
        {
            0,            1280,        0,
            /*vsync0_vhb vsync0_act vsync0_hhb*/
            0,            720,        0,
            /*vsync1_vhb vsync1_act vsync1_hhb*/
            0,            0,            0
        }
    },
    VI_DATA_TYPE_RGB,
    HI_FALSE,
    {1920 , 1080},
    {
        {
            {1920 , 1080},
        },
        {
            VI_REPHASE_MODE_NONE,
            VI_REPHASE_MODE_NONE
        }
    },
    {
        WDR_MODE_NONE,
        1080
    },
    DATA_RATE_X1
};

VI_DEV_ATTR_S DEV_ATTR_OV2775_2M_BASE =
{
    VI_MODE_MIPI,
    VI_WORK_MODE_1Multiplex,
    {0xFFF00000,    0x0},
    VI_SCAN_PROGRESSIVE,
    { -1, -1, -1, -1},
    VI_DATA_SEQ_YUYV,

    {
        /*port_vsync   port_vsync_neg     port_hsync        port_hsync_neg        */
        VI_VSYNC_PULSE, VI_VSYNC_NEG_LOW, VI_HSYNC_VALID_SINGNAL, VI_HSYNC_NEG_HIGH, VI_VSYNC_VALID_SINGAL, VI_VSYNC_VALID_NEG_HIGH,

        /*hsync_hfb    hsync_act    hsync_hhb*/
        {
            0,            1280,        0,
            /*vsync0_vhb vsync0_act vsync0_hhb*/
            0,            720,        0,
            /*vsync1_vhb vsync1_act vsync1_hhb*/
            0,            0,            0
        }
    },
    VI_DATA_TYPE_RGB,
    HI_FALSE,
    {1920 , 1080},
    {
        {
            {1920 , 1080},
        },
        {
            VI_REPHASE_MODE_NONE,
            VI_REPHASE_MODE_NONE
        }
    },
    {
        WDR_MODE_NONE,
        1080
    },
    DATA_RATE_X1
};

VI_PIPE_ATTR_S PIPE_ATTR_1920x1080_RAW12_420 =
{
    /* bBindDev bYuvSkip */
    VI_PIPE_BYPASS_NONE, HI_FALSE,HI_FALSE,
    1920, 1080,
    PIXEL_FORMAT_RGB_BAYER_12BPP,
    COMPRESS_MODE_LINE,
    DATA_BITWIDTH_12,
    HI_FALSE,
    {
        PIXEL_FORMAT_YVU_SEMIPLANAR_420,
        DATA_BITWIDTH_8,
        VI_NR_REF_FROM_RFR,
        COMPRESS_MODE_NONE
    },
    HI_FALSE,
    { -1, -1}
};

VI_PIPE_ATTR_S PIPE_ATTR_1920x1080_RAW12_420_3DNR_RFR =
{
    VI_PIPE_BYPASS_NONE, HI_FALSE, HI_FALSE,
    1920, 1080,
    PIXEL_FORMAT_RGB_BAYER_12BPP,
    COMPRESS_MODE_NONE,
    DATA_BITWIDTH_12,
    HI_FALSE,
    {
        PIXEL_FORMAT_YVU_SEMIPLANAR_420,
        DATA_BITWIDTH_8,
        VI_NR_REF_FROM_RFR,
        COMPRESS_MODE_NONE
    },
    HI_FALSE,
    { -1, -1}
};

VI_PIPE_ATTR_S PIPE_ATTR_1920x1080_RAW12_420_3DNR_CHN0 =
{
    VI_PIPE_BYPASS_NONE, HI_FALSE,HI_FALSE,
    1920, 1080,
    PIXEL_FORMAT_RGB_BAYER_12BPP,
    COMPRESS_MODE_LINE,
    DATA_BITWIDTH_12,
    HI_TRUE,
    {
        PIXEL_FORMAT_YVU_SEMIPLANAR_420,
        DATA_BITWIDTH_8,
        VI_NR_REF_FROM_CHN0,
        COMPRESS_MODE_NONE
    },
    HI_FALSE,
    { -1, -1}
};

static VI_PIPE_ATTR_S PIPE_ATTR_2592x1944_RAW12_420_3DNR_RFR =
{
    VI_PIPE_BYPASS_NONE, HI_FALSE, HI_FALSE,
    2592, 1944,
    PIXEL_FORMAT_RGB_BAYER_12BPP,
    COMPRESS_MODE_LINE,
    DATA_BITWIDTH_12,
    HI_FALSE,
    {
        PIXEL_FORMAT_YVU_SEMIPLANAR_420,
        DATA_BITWIDTH_8,
        VI_NR_REF_FROM_RFR,
        COMPRESS_MODE_NONE
    },
    HI_FALSE,
    {-1, -1}
};

static VI_PIPE_ATTR_S PIPE_ATTR_2592x1944_RAW10_420_3DNR_RFR =
{
    VI_PIPE_BYPASS_NONE, HI_FALSE, HI_FALSE,
    2592, 1944,
    PIXEL_FORMAT_RGB_BAYER_10BPP,
    COMPRESS_MODE_LINE,
    DATA_BITWIDTH_10,
    HI_FALSE,
    {
        PIXEL_FORMAT_YVU_SEMIPLANAR_420,
        DATA_BITWIDTH_8,
        VI_NR_REF_FROM_RFR,
        COMPRESS_MODE_NONE
    },
    HI_FALSE,
    {-1, -1}
};

static VI_PIPE_ATTR_S PIPE_ATTR_2592x1536_RAW12_420_3DNR_RFR =
{
    VI_PIPE_BYPASS_NONE, HI_FALSE, HI_FALSE,
    2592, 1536,
    PIXEL_FORMAT_RGB_BAYER_12BPP,
    COMPRESS_MODE_LINE,
    DATA_BITWIDTH_12,
    HI_FALSE,
    {
        PIXEL_FORMAT_YVU_SEMIPLANAR_420,
        DATA_BITWIDTH_8,
        VI_NR_REF_FROM_RFR,
        COMPRESS_MODE_NONE
    },
    HI_FALSE,
    {-1, -1}
};

static VI_PIPE_ATTR_S PIPE_ATTR_2592x1536_RAW10_420_3DNR_RFR =
{
    VI_PIPE_BYPASS_NONE, HI_FALSE, HI_FALSE,
    2592, 1536,
    PIXEL_FORMAT_RGB_BAYER_10BPP,
    COMPRESS_MODE_LINE,
    DATA_BITWIDTH_10,
    HI_FALSE,
    {
        PIXEL_FORMAT_YVU_SEMIPLANAR_420,
        DATA_BITWIDTH_8,
        VI_NR_REF_FROM_RFR,
        COMPRESS_MODE_NONE
    },
    HI_FALSE,
    {-1, -1}
};

static VI_PIPE_ATTR_S PIPE_ATTR_2560x1440_RAW12_420_3DNR_RFR =
{
    VI_PIPE_BYPASS_NONE, HI_FALSE, HI_FALSE,
    2560, 1440,
    PIXEL_FORMAT_RGB_BAYER_12BPP,
    COMPRESS_MODE_NONE,
    DATA_BITWIDTH_12,
    HI_FALSE,
    {
        PIXEL_FORMAT_YVU_SEMIPLANAR_420,
        DATA_BITWIDTH_8,
        VI_NR_REF_FROM_RFR,
        COMPRESS_MODE_NONE
    },
    HI_FALSE,
    {-1, -1}
};

static VI_PIPE_ATTR_S PIPE_ATTR_2560x1440_RAW10_420_3DNR_RFR =
{
    VI_PIPE_BYPASS_NONE, HI_FALSE, HI_FALSE,
    2560, 1440,
    PIXEL_FORMAT_RGB_BAYER_10BPP,
    COMPRESS_MODE_NONE,
    DATA_BITWIDTH_10,
    HI_FALSE,
    {
        PIXEL_FORMAT_YVU_SEMIPLANAR_420,
        DATA_BITWIDTH_8,
        VI_NR_REF_FROM_RFR,
        COMPRESS_MODE_NONE
    },
    HI_FALSE,
    {-1, -1}
};

static VI_PIPE_ATTR_S PIPE_ATTR_3840x2160_RAW10_420_3DNR_RFR =
{
    VI_PIPE_BYPASS_NONE, HI_FALSE, HI_FALSE,
    3840, 2160,
    PIXEL_FORMAT_RGB_BAYER_10BPP,
    COMPRESS_MODE_LINE,
    DATA_BITWIDTH_10,
    HI_TRUE,
    {
        PIXEL_FORMAT_YVU_SEMIPLANAR_420,
        DATA_BITWIDTH_8,
        VI_NR_REF_FROM_RFR,
        COMPRESS_MODE_NONE
    },
    HI_FALSE,
    { -1, -1}
};

static VI_PIPE_ATTR_S PIPE_ATTR_3840x2160_RAW12_420_3DNR_RFR =
{
    VI_PIPE_BYPASS_NONE, HI_FALSE, HI_FALSE,
    3840, 2160,
    PIXEL_FORMAT_RGB_BAYER_12BPP,
    COMPRESS_MODE_LINE,
    DATA_BITWIDTH_12,
    HI_TRUE,
    {
        PIXEL_FORMAT_YVU_SEMIPLANAR_420,
        DATA_BITWIDTH_8,
        VI_NR_REF_FROM_RFR,
        COMPRESS_MODE_NONE
    },
    HI_FALSE,
    { -1, -1}
};

static VI_PIPE_ATTR_S PIPE_ATTR_4000x3000_RAW10_420_3DNR_RFR =
{
    VI_PIPE_BYPASS_NONE, HI_FALSE, HI_FALSE,
    4000, 3000,
    PIXEL_FORMAT_RGB_BAYER_10BPP,
    COMPRESS_MODE_LINE,
    DATA_BITWIDTH_10,
    HI_TRUE,
    {
        PIXEL_FORMAT_YVU_SEMIPLANAR_420,
        DATA_BITWIDTH_8,
        VI_NR_REF_FROM_RFR,
        COMPRESS_MODE_NONE
    },
    HI_FALSE,
    { -1, -1}
};

static VI_PIPE_ATTR_S PIPE_ATTR_2716x1524_RAW10_420_3DNR_RFR =
{
    VI_PIPE_BYPASS_NONE, HI_FALSE, HI_FALSE,
    2716, 1524,
    PIXEL_FORMAT_RGB_BAYER_10BPP,
    COMPRESS_MODE_LINE,
    DATA_BITWIDTH_10,
    HI_TRUE,
    {
        PIXEL_FORMAT_YVU_SEMIPLANAR_420,
        DATA_BITWIDTH_8,
        VI_NR_REF_FROM_RFR,
        COMPRESS_MODE_NONE
    },
    HI_FALSE,
    { -1, -1}
};

VI_PIPE_ATTR_S PIPE_ATTR_1920x1080_RAW10_420_3DNR_RFR =
{
    VI_PIPE_BYPASS_NONE, HI_FALSE, HI_FALSE,
    1920, 1080,
    PIXEL_FORMAT_RGB_BAYER_10BPP,
    COMPRESS_MODE_LINE,
    DATA_BITWIDTH_10,
    HI_TRUE,
    {
        PIXEL_FORMAT_YVU_SEMIPLANAR_420,
        DATA_BITWIDTH_8,
        VI_NR_REF_FROM_RFR,
        COMPRESS_MODE_NONE
    },
    HI_FALSE,
    { -1, -1}
};

VI_PIPE_ATTR_S PIPE_ATTR_1280x720_RAW10_420_3DNR_RFR =
{
    VI_PIPE_BYPASS_NONE, HI_FALSE, HI_FALSE,
    1280, 720,
    PIXEL_FORMAT_RGB_BAYER_10BPP,
    COMPRESS_MODE_LINE,
    DATA_BITWIDTH_10,
    HI_TRUE,
    {
        PIXEL_FORMAT_YVU_SEMIPLANAR_420,
        DATA_BITWIDTH_8,
        VI_NR_REF_FROM_RFR,
        COMPRESS_MODE_NONE
    },
    HI_FALSE,
    { -1, -1}
};

static VI_PIPE_ATTR_S PIPE_ATTR_1280x800_RAW10_420_3DNR_RFR =
{
    VI_PIPE_BYPASS_NONE, HI_FALSE, HI_FALSE,
    1280, 800,
    PIXEL_FORMAT_RGB_BAYER_10BPP,
    COMPRESS_MODE_LINE,
    DATA_BITWIDTH_10,
    HI_TRUE,
    {
        PIXEL_FORMAT_YVU_SEMIPLANAR_420,
        DATA_BITWIDTH_8,
        VI_NR_REF_FROM_RFR,
        COMPRESS_MODE_NONE
    },
    HI_FALSE,
    { -1, -1}
};

static VI_PIPE_ATTR_S PIPE_ATTR_2688x1536_RAW12_420_3DNR_RFR =
{
    VI_PIPE_BYPASS_NONE, HI_FALSE, HI_FALSE,
    2688, 1536,
    PIXEL_FORMAT_RGB_BAYER_12BPP,
    COMPRESS_MODE_LINE,
    DATA_BITWIDTH_12,
    HI_FALSE,
    {
        PIXEL_FORMAT_YVU_SEMIPLANAR_420,
        DATA_BITWIDTH_8,
        VI_NR_REF_FROM_RFR,
        COMPRESS_MODE_NONE
    },
    HI_FALSE,
    {-1, -1}
};

static VI_PIPE_ATTR_S PIPE_ATTR_2688x1536_RAW10_420_3DNR_RFR =
{
    VI_PIPE_BYPASS_NONE, HI_FALSE, HI_FALSE,
    2688, 1536,
    PIXEL_FORMAT_RGB_BAYER_10BPP,
    COMPRESS_MODE_LINE,
    DATA_BITWIDTH_10,
    HI_FALSE,
    {
        PIXEL_FORMAT_YVU_SEMIPLANAR_420,
        DATA_BITWIDTH_8,
        VI_NR_REF_FROM_RFR,
        COMPRESS_MODE_NONE
    },
    HI_FALSE,
    {-1, -1}
};


VI_CHN_ATTR_S CHN_ATTR_1920x1080_422_SDR8_LINEAR =
{
    {1920, 1080},
    PIXEL_FORMAT_YVU_SEMIPLANAR_422,
    DYNAMIC_RANGE_SDR8,
    VIDEO_FORMAT_LINEAR,
    COMPRESS_MODE_NONE,
    0,      0,
    0,
    { -1, -1}
};

VI_CHN_ATTR_S CHN_ATTR_1920x1080_420_SDR8_LINEAR =
{
    {1920, 1080},
    PIXEL_FORMAT_YVU_SEMIPLANAR_420,
    DYNAMIC_RANGE_SDR8,
    VIDEO_FORMAT_LINEAR,
    COMPRESS_MODE_NONE,
    0,      0,
    0,
    { -1, -1}
};

VI_CHN_ATTR_S CHN_ATTR_1920x1080_400_SDR8_LINEAR =
{
    {1920, 1080},
    PIXEL_FORMAT_YUV_400,
    DYNAMIC_RANGE_SDR8,
    VIDEO_FORMAT_LINEAR,
    COMPRESS_MODE_NONE,
    0,      0,
    0,
    { -1, -1}
};

static VI_CHN_ATTR_S CHN_ATTR_2592x1944_420_SDR8_LINEAR =
{
    {2592, 1944},
    PIXEL_FORMAT_YVU_SEMIPLANAR_420,
    DYNAMIC_RANGE_SDR8,
    VIDEO_FORMAT_LINEAR,
    COMPRESS_MODE_NONE,
    0,      0,
    0,
    {-1, -1}
};

static VI_CHN_ATTR_S CHN_ATTR_2592x1536_420_SDR8_LINEAR =
{
    {2592, 1536},
    PIXEL_FORMAT_YVU_SEMIPLANAR_420,
    DYNAMIC_RANGE_SDR8,
    VIDEO_FORMAT_LINEAR,
    COMPRESS_MODE_NONE,
    0,      0,
    0,
    {-1, -1}
};

static VI_CHN_ATTR_S CHN_ATTR_2560x1440_420_SDR8_LINEAR =
{
    {2560, 1440},
    PIXEL_FORMAT_YVU_SEMIPLANAR_420,
    DYNAMIC_RANGE_SDR8,
    VIDEO_FORMAT_LINEAR,
    COMPRESS_MODE_NONE,
    0,      0,
    0,
    {-1, -1}
};

static VI_CHN_ATTR_S CHN_ATTR_4000x3000_420_SDR8_LINEAR =
{
    {4000, 3000},
    PIXEL_FORMAT_YVU_SEMIPLANAR_420,
    DYNAMIC_RANGE_SDR8,
    VIDEO_FORMAT_LINEAR,
    COMPRESS_MODE_NONE,
    0,      0,
    0,
    { -1, -1}
};

VI_CHN_ATTR_S CHN_ATTR_3840x2160_420_SDR8_LINEAR =
{
    {3840, 2160},
    PIXEL_FORMAT_YVU_SEMIPLANAR_420,
    DYNAMIC_RANGE_SDR8,
    VIDEO_FORMAT_LINEAR,
    COMPRESS_MODE_NONE,
    0,      0,
    0,
    { -1, -1}
};

static VI_CHN_ATTR_S CHN_ATTR_2716x1524_420_SDR8_LINEAR =
{
    {2716, 1524},
    PIXEL_FORMAT_YVU_SEMIPLANAR_420,
    DYNAMIC_RANGE_SDR8,
    VIDEO_FORMAT_LINEAR,
    COMPRESS_MODE_NONE,
    0,      0,
    0,
    { -1, -1}
};

VI_CHN_ATTR_S CHN_ATTR_1280x720_420_SDR8_LINEAR =
{
    {1280, 720},
    PIXEL_FORMAT_YVU_SEMIPLANAR_420,
    DYNAMIC_RANGE_SDR8,
    VIDEO_FORMAT_LINEAR,
    COMPRESS_MODE_NONE,
    0,      0,
    0,
    { -1, -1}
};

static VI_CHN_ATTR_S CHN_ATTR_1280x800_420_SDR8_LINEAR =
{
    {1280, 800},
    PIXEL_FORMAT_YVU_SEMIPLANAR_420,
    DYNAMIC_RANGE_SDR8,
    VIDEO_FORMAT_LINEAR,
    COMPRESS_MODE_NONE,
    0,      0,
    0,
    { -1, -1}
};

static VI_CHN_ATTR_S CHN_ATTR_2688x1536_420_SDR8_LINEAR =
{
    {2688, 1536},
    PIXEL_FORMAT_YVU_SEMIPLANAR_420,
    DYNAMIC_RANGE_SDR8,
    VIDEO_FORMAT_LINEAR,
    COMPRESS_MODE_NONE,
    0,      0,
    0,
    {-1, -1}
};


HI_BOOL IsSensorInput(SAMPLE_SNS_TYPE_E enSnsType)
{
    HI_BOOL bRet = HI_TRUE;

    switch (enSnsType)
    {
        case SAMPLE_SNS_TYPE_BUTT:
            bRet = HI_FALSE;
            break;

        default:
            break;
    }

    return bRet;
}


HI_S32 SAMPLE_COMM_VI_ResetSns(sns_rst_source_t SnsDev)
{
    HI_S32 fd;
    HI_S32 s32Ret;

    fd = open(MIPI_DEV_NODE, O_RDWR);

    if (fd < 0)
    {
        SAMPLE_PRT("open hi_mipi dev failed\n");
        return -1;
    }

    s32Ret = ioctl(fd, HI_MIPI_RESET_SENSOR, &SnsDev);

    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("HI_MIPI_SET_HS_MODE failed\n");
    }

    close(fd);
    return s32Ret;
}

HI_S32 SAMPLE_COMM_VI_SetMipiHsMode(lane_divide_mode_t enHsMode)
{
    HI_S32 fd;
    HI_S32 s32Ret;

    fd = open(MIPI_DEV_NODE, O_RDWR);

    if (fd < 0)
    {
        SAMPLE_PRT("open hi_mipi dev failed\n");
        return -1;
    }

    s32Ret = ioctl(fd, HI_MIPI_SET_HS_MODE, &enHsMode);

    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("HI_MIPI_SET_HS_MODE failed\n");
    }

    close(fd);
    return s32Ret;
}

HI_S32 SAMPLE_COMM_VI_EnableMipiClock(SAMPLE_VI_CONFIG_S* pstViConfig)
{
    HI_S32              i = 0;
    HI_S32              s32ViNum = 0;
    HI_S32              s32Ret = HI_SUCCESS;
    HI_S32              fd;
    combo_dev_t           devno = 0;
    SAMPLE_VI_INFO_S*   pstViInfo = HI_NULL;

    if (!pstViConfig)
    {
        SAMPLE_PRT("%s: null ptr\n", __FUNCTION__);
        return HI_FAILURE;
    }

    fd = open(MIPI_DEV_NODE, O_RDWR);

    if (fd < 0)
    {
        SAMPLE_PRT("open hi_mipi dev failed\n");
        return HI_FAILURE;
    }

    for (i = 0; i < pstViConfig->s32WorkingViNum; i++)
    {
        s32ViNum  = pstViConfig->as32WorkingViId[i];
        pstViInfo = &pstViConfig->astViInfo[s32ViNum];

        devno = pstViInfo->stSnsInfo.MipiDev;

        s32Ret = ioctl(fd, HI_MIPI_ENABLE_MIPI_CLOCK, &devno);


        if (HI_SUCCESS != s32Ret)
        {
            SAMPLE_PRT("MIPI_ENABLE_CLOCK %d failed\n", devno);
            goto EXIT;
        }
    }

EXIT:
    close(fd);

    return s32Ret;
}

HI_S32 SAMPLE_COMM_VI_DisableMipiClock(SAMPLE_VI_CONFIG_S* pstViConfig)
{
    HI_S32              i = 0;
    HI_S32              s32ViNum = 0;
    HI_S32              s32Ret = HI_SUCCESS;
    HI_S32              fd;
    combo_dev_t           devno = 0;
    SAMPLE_VI_INFO_S*   pstViInfo = HI_NULL;

    if (!pstViConfig)
    {
        SAMPLE_PRT("%s: null ptr\n", __FUNCTION__);
        return HI_FAILURE;
    }

    fd = open(MIPI_DEV_NODE, O_RDWR);

    if (fd < 0)
    {
        SAMPLE_PRT("open hi_mipi dev failed\n");
        return HI_FAILURE;
    }

    for (i = 0; i < pstViConfig->s32WorkingViNum; i++)
    {
        s32ViNum  = pstViConfig->as32WorkingViId[i];
        pstViInfo = &pstViConfig->astViInfo[s32ViNum];

        devno = pstViInfo->stSnsInfo.MipiDev;


        s32Ret = ioctl(fd, HI_MIPI_DISABLE_MIPI_CLOCK, &devno);


        if (HI_SUCCESS != s32Ret)
        {
            SAMPLE_PRT("MIPI_DISABLE_CLOCK %d failed\n", devno);
            goto EXIT;
        }
    }

EXIT:
    close(fd);

    return s32Ret;
}

HI_S32 SAMPLE_COMM_VI_ResetMipi(SAMPLE_VI_CONFIG_S* pstViConfig)
{
    HI_S32              i = 0;
    HI_S32              s32ViNum = 0;
    HI_S32              s32Ret = HI_SUCCESS;
    HI_S32              fd;
    combo_dev_t           devno = 0;
    SAMPLE_VI_INFO_S*   pstViInfo = HI_NULL;

    if (!pstViConfig)
    {
        SAMPLE_PRT("%s: null ptr\n", __FUNCTION__);
        return HI_FAILURE;
    }

    fd = open(MIPI_DEV_NODE, O_RDWR);

    if (fd < 0)
    {
        SAMPLE_PRT("open hi_mipi dev failed\n");
        return HI_FAILURE;
    }

    for (i = 0; i < pstViConfig->s32WorkingViNum; i++)
    {
        s32ViNum  = pstViConfig->as32WorkingViId[i];
        pstViInfo = &pstViConfig->astViInfo[s32ViNum];

        devno = pstViInfo->stSnsInfo.MipiDev;


        s32Ret = ioctl(fd, HI_MIPI_RESET_MIPI, &devno);


        if (HI_SUCCESS != s32Ret)
        {
            SAMPLE_PRT("RESET_MIPI %d failed\n", devno);
            goto EXIT;
        }
    }

EXIT:
    close(fd);

    return s32Ret;
}

HI_S32 SAMPLE_COMM_VI_UnresetMipi(SAMPLE_VI_CONFIG_S* pstViConfig)
{
    HI_S32              i = 0;
    HI_S32              s32ViNum = 0;
    HI_S32              s32Ret = HI_SUCCESS;
    HI_S32              fd;
    combo_dev_t           devno = 0;
    SAMPLE_VI_INFO_S*   pstViInfo = HI_NULL;

    if (!pstViConfig)
    {
        SAMPLE_PRT("%s: null ptr\n", __FUNCTION__);
        return HI_FAILURE;
    }

    fd = open(MIPI_DEV_NODE, O_RDWR);

    if (fd < 0)
    {
        SAMPLE_PRT("open hi_mipi dev failed\n");
        return HI_FAILURE;
    }

    for (i = 0; i < pstViConfig->s32WorkingViNum; i++)
    {
        s32ViNum  = pstViConfig->as32WorkingViId[i];
        pstViInfo = &pstViConfig->astViInfo[s32ViNum];

        devno = pstViInfo->stSnsInfo.MipiDev;


        s32Ret = ioctl(fd, HI_MIPI_UNRESET_MIPI, &devno);


        if (HI_SUCCESS != s32Ret)
        {
            SAMPLE_PRT("UNRESET_MIPI %d failed\n", devno);
            goto EXIT;
        }
    }

EXIT:
    close(fd);

    return s32Ret;

}

HI_S32 SAMPLE_COMM_VI_EnableSensorClock(SAMPLE_VI_CONFIG_S* pstViConfig)
{
    HI_S32              s32Ret = HI_SUCCESS;
    HI_S32              fd;
    sns_clk_source_t       SnsDev = 0;

    fd = open(MIPI_DEV_NODE, O_RDWR);

    if (fd < 0)
    {
        SAMPLE_PRT("open hi_mipi dev failed\n");
        return HI_FAILURE;
    }

    for (SnsDev = 0; SnsDev < SNS_MAX_CLK_SOURCE_NUM; SnsDev++)
    {
        s32Ret = ioctl(fd, HI_MIPI_ENABLE_SENSOR_CLOCK, &SnsDev);

        if (HI_SUCCESS != s32Ret)
        {
            SAMPLE_PRT("HI_MIPI_ENABLE_SENSOR_CLOCK failed\n");
            goto EXIT;
        }
    }

EXIT:
    close(fd);

    return s32Ret;
}

HI_S32 SAMPLE_COMM_VI_DisableSensorClock(SAMPLE_VI_CONFIG_S* pstViConfig)
{
    HI_S32              s32Ret = HI_SUCCESS;
    HI_S32              fd;
    sns_clk_source_t       SnsDev = 0;

    fd = open(MIPI_DEV_NODE, O_RDWR);

    if (fd < 0)
    {
        SAMPLE_PRT("open hi_mipi dev failed\n");
        return HI_FAILURE;
    }

    for (SnsDev = 0; SnsDev < SNS_MAX_CLK_SOURCE_NUM; SnsDev++)
    {
        s32Ret = ioctl(fd, HI_MIPI_DISABLE_SENSOR_CLOCK, &SnsDev);

        if (HI_SUCCESS != s32Ret)
        {
            SAMPLE_PRT("HI_MIPI_DISABLE_SENSOR_CLOCK failed\n");
            goto EXIT;
        }
    }

EXIT:
    close(fd);

    return s32Ret;
}

HI_S32 SAMPLE_COMM_VI_ResetSensor(SAMPLE_VI_CONFIG_S* pstViConfig)
{
    HI_S32              s32Ret = HI_SUCCESS;
    HI_S32              fd;
    sns_rst_source_t       SnsDev = 0;

    fd = open(MIPI_DEV_NODE, O_RDWR);

    if (fd < 0)
    {
        SAMPLE_PRT("open hi_mipi dev failed\n");
        return HI_FAILURE;
    }

    for (SnsDev = 0; SnsDev < SNS_MAX_RST_SOURCE_NUM; SnsDev++)
    {
        s32Ret = ioctl(fd, HI_MIPI_RESET_SENSOR, &SnsDev);

        if (HI_SUCCESS != s32Ret)
        {
            SAMPLE_PRT("HI_MIPI_RESET_SENSOR failed\n");
            goto EXIT;
        }
    }

EXIT:
    close(fd);

    return s32Ret;
}

HI_S32 SAMPLE_COMM_VI_UnresetSensor(SAMPLE_VI_CONFIG_S* pstViConfig)
{
    HI_S32              s32Ret = HI_SUCCESS;
    HI_S32              fd;
    sns_rst_source_t       SnsDev = 0;

    fd = open(MIPI_DEV_NODE, O_RDWR);

    if (fd < 0)
    {
        SAMPLE_PRT("open hi_mipi dev failed\n");
        return HI_FAILURE;
    }

    for (SnsDev = 0; SnsDev < SNS_MAX_RST_SOURCE_NUM; SnsDev++)
    {
        s32Ret = ioctl(fd, HI_MIPI_UNRESET_SENSOR, &SnsDev);

        if (HI_SUCCESS != s32Ret)
        {
            SAMPLE_PRT("HI_MIPI_UNRESET_SENSOR failed\n");
            goto EXIT;
        }
    }

EXIT:
    close(fd);

    return s32Ret;
}

HI_S32 SAMPLE_COMM_VI_GetComboAttrBySns(SAMPLE_SNS_TYPE_E enSnsType, combo_dev_t MipiDev, combo_dev_attr_t* pstComboAttr)
{
    switch (enSnsType)
    {
        case SONY_IMX327_MIPI_2M_30FPS_12BIT:
        case SONY_IMX385_MIPI_2M_30FPS_12BIT:
        case SONY_IMX482_MIPI_2M_30FPS_12BIT:
        case SONY_IMX585_MIPI_2M_30FPS_12BIT:  
            memcpy_s(pstComboAttr, sizeof(combo_dev_attr_t), &MIPI_4lane_CHN0_SENSOR_IMX327_12BIT_2M_NOWDR_ATTR, sizeof(combo_dev_attr_t));
            break;

        case SONY_IMX327_MIPI_2M_30FPS_12BIT_WDR2TO1:
            memcpy_s(pstComboAttr, sizeof(combo_dev_attr_t), &MIPI_4lane_CHN0_SENSOR_IMX327_12BIT_2M_WDR2to1_ATTR, sizeof(combo_dev_attr_t));
            break;

        case SONY_IMX327_2L_MIPI_2M_30FPS_12BIT:
            if (0 == MipiDev)
            {
                memcpy(pstComboAttr, &MIPI_2lane_CHN0_SENSOR_IMX327_12BIT_2M_NOWDR_ATTR, sizeof(combo_dev_attr_t));
            }
            else if (1 == MipiDev)
            {
                memcpy(pstComboAttr, &MIPI_2lane_CHN1_SENSOR_IMX327_12BIT_2M_NOWDR_ATTR, sizeof(combo_dev_attr_t));
            }

            break;

        case SONY_IMX327_2L_MIPI_2M_30FPS_12BIT_WDR2TO1:
            if (0 == MipiDev)
            {
                memcpy(pstComboAttr, &MIPI_2lane_CHN0_SENSOR_IMX327_12BIT_2M_WDR2to1_ATTR, sizeof(combo_dev_attr_t));
            }
            else if (1 == MipiDev)
            {
                memcpy(pstComboAttr, &MIPI_2lane_CHN1_SENSOR_IMX327_12BIT_2M_WDR2to1_ATTR, sizeof(combo_dev_attr_t));
            }

            break;

        case SONY_IMX390_MIPI_2M_30FPS_12BIT:
            memcpy_s(pstComboAttr, sizeof(combo_dev_attr_t), &MIPI_4lane_CHN0_SENSOR_IMX390_12BIT_2M_NOWDR_ATTR, sizeof(combo_dev_attr_t));
            break;

        case SONY_IMX290_MIPI_2M_30FPS_12BIT:
        case SONY_IMX290_MIPI_2M_120FPS_12BIT:
            memcpy_s(pstComboAttr, sizeof(combo_dev_attr_t), &MIPI_4lane_CHN0_SENSOR_IMX290_12BIT_2M_NOWDR_ATTR, sizeof(combo_dev_attr_t));
            break;

        case SONY_IMX290_MIPI_2M_30FPS_12BIT_WDR2TO1:
            memcpy_s(pstComboAttr, sizeof(combo_dev_attr_t), &MIPI_4lane_CHN0_SENSOR_IMX290_12BIT_2M_WDR2to1_ATTR, sizeof(combo_dev_attr_t));
            break;

        case SONY_IMX290_2L_MIPI_2M_30FPS_12BIT:
            if (0 == MipiDev)
            {
                memcpy(pstComboAttr, &MIPI_2lane_CHN0_SENSOR_IMX290_12BIT_2M_NOWDR_ATTR, sizeof(combo_dev_attr_t));
            }
            else if (1 == MipiDev)
            {
                memcpy(pstComboAttr, &MIPI_2lane_CHN1_SENSOR_IMX290_12BIT_2M_NOWDR_ATTR, sizeof(combo_dev_attr_t));
            }

            break;

        case SONY_IMX290_2L_MIPI_2M_30FPS_12BIT_WDR2TO1:
            if (0 == MipiDev)
            {
                memcpy(pstComboAttr, &MIPI_2lane_CHN0_SENSOR_IMX290_12BIT_2M_WDR2to1_ATTR, sizeof(combo_dev_attr_t));
            }
            else if (1 == MipiDev)
            {
                memcpy(pstComboAttr, &MIPI_2lane_CHN1_SENSOR_IMX290_12BIT_2M_WDR2to1_ATTR, sizeof(combo_dev_attr_t));
            }

            break;
        case SONY_IMX307_MIPI_2M_30FPS_12BIT:
            memcpy_s(pstComboAttr, sizeof(combo_dev_attr_t), &MIPI_4lane_CHN0_SENSOR_IMX307_12BIT_2M_NOWDR_ATTR, sizeof(combo_dev_attr_t));
            break;

        case SONY_IMX307_MIPI_2M_30FPS_12BIT_WDR2TO1:
            memcpy_s(pstComboAttr, sizeof(combo_dev_attr_t), &MIPI_4lane_CHN0_SENSOR_IMX307_12BIT_2M_WDR2to1_ATTR, sizeof(combo_dev_attr_t));
            break;

        case SONY_IMX307_2L_MIPI_2M_30FPS_12BIT:
        case SONY_IMX307_2L_SLAVE_MIPI_2M_30FPS_12BIT:
            if (0 == MipiDev)
            {
                memcpy(pstComboAttr, &MIPI_2lane_CHN0_SENSOR_IMX307_12BIT_2M_NOWDR_ATTR, sizeof(combo_dev_attr_t));
            }
            else if (1 == MipiDev)
            {
                memcpy(pstComboAttr, &MIPI_2lane_CHN1_SENSOR_IMX307_12BIT_2M_NOWDR_ATTR, sizeof(combo_dev_attr_t));
            }

            break;

        case SONY_IMX307_2L_SLAVE_MIPI_2M_30FPS_12BIT_WDR2TO1:
        case SONY_IMX307_2L_MIPI_2M_30FPS_12BIT_WDR2TO1:
            if (0 == MipiDev)
            {
                memcpy(pstComboAttr, &MIPI_2lane_CHN0_SENSOR_IMX307_12BIT_2M_WDR2to1_ATTR, sizeof(combo_dev_attr_t));
            }
            else if (1 == MipiDev)
            {
                memcpy(pstComboAttr, &MIPI_2lane_CHN1_SENSOR_IMX307_12BIT_2M_WDR2to1_ATTR, sizeof(combo_dev_attr_t));
            }

            break;

        case PRIMESENSOR_PS5260_2L_MIPI_2M_30FPS_12BIT:
            if (0 == MipiDev)
            {
                memcpy(pstComboAttr, &MIPI_2lane_CHN0_SENSOR_PS5260_12BIT_2M_NOWDR_ATTR, sizeof(combo_dev_attr_t));
            }
            break;

        case PRIMESENSOR_PS5260_2L_MIPI_2M_30FPS_12BIT_BUILTIN:
            if (0 == MipiDev)
            {
                memcpy(pstComboAttr, &MIPI_2lane_CHN0_SENSOR_PS5260_12BIT_2M_NOWDR_ATTR, sizeof(combo_dev_attr_t));
            }
            else if (1 == MipiDev)
            {
                memcpy(pstComboAttr, &MIPI_2lane_CHN0_SENSOR_PS5260_12BIT_2M_NOWDR_ATTR, sizeof(combo_dev_attr_t));
            }

            break;

        case PANASONIC_MN34220_LVDS_2M_30FPS_12BIT:
            memcpy_s(pstComboAttr, sizeof(combo_dev_attr_t), &LVDS_4lane_CHN0_SENSOR_MN34220_12BIT_2M_NOWDR_ATTR, sizeof(combo_dev_attr_t));
            break;

        case SONY_IMX335_2L_MIPI_5M_30FPS_10BIT: 
            if (0 == MipiDev)
            {
                memcpy(pstComboAttr, &MIPI_2lane_CHN0_SENSOR_IMX335_10BIT_5M_NOWDR_ATTR, sizeof(combo_dev_attr_t));
            }
            else if (1 == MipiDev)
            {
                memcpy(pstComboAttr, &MIPI_2lane_CHN1_SENSOR_IMX335_10BIT_5M_NOWDR_ATTR, sizeof(combo_dev_attr_t));
            }
            break;
          
        case SONY_IMX335_MIPI_5M_30FPS_12BIT:
            memcpy_s(pstComboAttr, sizeof(combo_dev_attr_t), &MIPI_4lane_CHN0_SENSOR_IMX335_12BIT_5M_NOWDR_ATTR, sizeof(combo_dev_attr_t));
            break;

        case SONY_IMX335_MIPI_5M_30FPS_10BIT_WDR2TO1:
            memcpy_s(pstComboAttr, sizeof(combo_dev_attr_t), &MIPI_4lane_CHN0_SENSOR_IMX335_10BIT_5M_WDR2TO1_ATTR, sizeof(combo_dev_attr_t));
            break;

        case SONY_IMX335_MIPI_4M_30FPS_12BIT:
            memcpy_s(pstComboAttr, sizeof(combo_dev_attr_t), &MIPI_4lane_CHN0_SENSOR_IMX335_12BIT_4M_NOWDR_ATTR, sizeof(combo_dev_attr_t));
            break;

        case SONY_IMX335_MIPI_4M_30FPS_10BIT_WDR2TO1:
            memcpy_s(pstComboAttr, sizeof(combo_dev_attr_t), &MIPI_4lane_CHN0_SENSOR_IMX335_10BIT_4M_WDR2TO1_ATTR, sizeof(combo_dev_attr_t));
            break;

        case SMART_SC4210_MIPI_3M_30FPS_12BIT:
            memcpy_s(pstComboAttr, sizeof(combo_dev_attr_t), &MIPI_4lane_CHN0_SENSOR_SC4210_12BIT_3M_NOWDR_ATTR, sizeof(combo_dev_attr_t));
            break;

        case SMART_SC4210_MIPI_3M_30FPS_10BIT_WDR2TO1:
            memcpy_s(pstComboAttr, sizeof(combo_dev_attr_t), &MIPI_4lane_CHN0_SENSOR_SC4210_10BIT_3M_WDR2TO1_ATTR, sizeof(combo_dev_attr_t));
            break;

        case SONY_IMX458_MIPI_8M_30FPS_10BIT:
            memcpy_s(pstComboAttr, sizeof(combo_dev_attr_t), &MIPI_4lane_CHN0_SENSOR_IMX458_10BIT_8M_NOWDR_ATTR, sizeof(combo_dev_attr_t));
            break;
        case SONY_IMX458_MIPI_12M_20FPS_10BIT:
            memcpy_s(pstComboAttr, sizeof(combo_dev_attr_t), &MIPI_4lane_CHN0_SENSOR_IMX458_10BIT_12M_NOWDR_ATTR, sizeof(combo_dev_attr_t));
            break;
        case SONY_IMX458_MIPI_4M_60FPS_10BIT:
        case SONY_IMX458_MIPI_4M_40FPS_10BIT:
            memcpy_s(pstComboAttr, sizeof(combo_dev_attr_t), &MIPI_4lane_CHN0_SENSOR_IMX458_10BIT_4M_NOWDR_ATTR, sizeof(combo_dev_attr_t));
            break;
        case SONY_IMX458_MIPI_2M_90FPS_10BIT:
            memcpy_s(pstComboAttr, sizeof(combo_dev_attr_t), &MIPI_4lane_CHN0_SENSOR_IMX458_10BIT_2M_NOWDR_ATTR, sizeof(combo_dev_attr_t));
            break;
        case SONY_IMX458_MIPI_1M_129FPS_10BIT:
            memcpy_s(pstComboAttr, sizeof(combo_dev_attr_t), &MIPI_4lane_CHN0_SENSOR_IMX458_10BIT_1M_NOWDR_ATTR, sizeof(combo_dev_attr_t));
            break;

        case OMNIVISION_OS04B10_MIPI_4M_30FPS_10BIT:
            memcpy_s(pstComboAttr, sizeof(combo_dev_attr_t), &MIPI_2lane_CHN0_SENSOR_OS04B10_10BIT_4M_NOWDR_ATTR, sizeof(combo_dev_attr_t));
            break;

        case OMNIVISION_OS05A_MIPI_4M_30FPS_12BIT:
            memcpy_s(pstComboAttr, sizeof(combo_dev_attr_t), &MIPI_4lane_CHN0_SENSOR_OS05A_12BIT_4M_NOWDR_ATTR, sizeof(combo_dev_attr_t));
            break;
        case OMNIVISION_OS05A_MIPI_4M_30FPS_10BIT_WDR2TO1:
            memcpy_s(pstComboAttr, sizeof(combo_dev_attr_t), &MIPI_4lane_CHN0_SENSOR_OS05A_10BIT_4M_WDR2TO1_ATTR, sizeof(combo_dev_attr_t));
            break;
        case OMNIVISION_OS05A_2L_MIPI_1M_30FPS_10BIT:
            if (0 == MipiDev)
            {
                memcpy(pstComboAttr, &MIPI_2lane_CHN0_SENSOR_OS05A_10BIT_1M_NOWDR_ATTR, sizeof(combo_dev_attr_t));
            }
            else if (1 == MipiDev)
            {
                memcpy(pstComboAttr, &MIPI_2lane_CHN1_SENSOR_OS05A_10BIT_1M_NOWDR_ATTR, sizeof(combo_dev_attr_t));
            }
            break;
        case OMNIVISION_OS05A_2L_MIPI_4M_30FPS_12BIT:
            if (0 == MipiDev)
            {
                memcpy(pstComboAttr, &MIPI_2lane_CHN0_SENSOR_OS05A_10BIT_4M_NOWDR_ATTR, sizeof(combo_dev_attr_t));
            }
            else if (1 == MipiDev)
            {
                memcpy(pstComboAttr, &MIPI_2lane_CHN1_SENSOR_OS05A_10BIT_4M_NOWDR_ATTR, sizeof(combo_dev_attr_t));
            }
            break;
        case OMNIVISION_OS05A_2L_MIPI_4M_30FPS_10BIT_WDR2TO1:
            if (0 == MipiDev)
            {
                memcpy(pstComboAttr, &MIPI_2lane_CHN0_SENSOR_OS05A_10BIT_4M_WDR2TO1_ATTR, sizeof(combo_dev_attr_t));
            }
            else if (1 == MipiDev)
            {
                memcpy(pstComboAttr, &MIPI_2lane_CHN1_SENSOR_OS05A_10BIT_4M_WDR2TO1_ATTR, sizeof(combo_dev_attr_t));
            }
            break;

        case OMNIVISION_OS08A10_MIPI_8M_30FPS_10BIT:
            memcpy_s(pstComboAttr, sizeof(combo_dev_attr_t), &MIPI_4lane_CHN0_SENSOR_OS08A10_10BIT_8M_NOWDR_ATTR, sizeof(combo_dev_attr_t));
            break;

        case GALAXYCORE_GC2053_MIPI_2M_30FPS_10BIT:
            memcpy_s(pstComboAttr, sizeof(combo_dev_attr_t), &MIPI_2lane_CHN0_SENSOR_GC2053_10BIT_2M_NOWDR_ATTR, sizeof(combo_dev_attr_t));
            break;
        case OMNIVISION_OV12870_MIPI_8M_30FPS_10BIT:
            memcpy_s(pstComboAttr, sizeof(combo_dev_attr_t), &MIPI_4lane_CHN0_SENSOR_OV12870_10BIT_8M_NOWDR_ATTR, sizeof(combo_dev_attr_t));
            break;
        case OMNIVISION_OV12870_MIPI_12M_30FPS_10BIT:
            memcpy_s(pstComboAttr, sizeof(combo_dev_attr_t), &MIPI_4lane_CHN0_SENSOR_OV12870_10BIT_12M_NOWDR_ATTR, sizeof(combo_dev_attr_t));
            break;
        case OMNIVISION_OV12870_MIPI_2M_120FPS_10BIT:
            memcpy_s(pstComboAttr, sizeof(combo_dev_attr_t), &MIPI_4lane_CHN0_SENSOR_OV12870_10BIT_2M_NOWDR_ATTR, sizeof(combo_dev_attr_t));
            break;
        case OMNIVISION_OV12870_MIPI_1M_240FPS_10BIT:
            memcpy_s(pstComboAttr, sizeof(combo_dev_attr_t), &MIPI_4lane_CHN0_SENSOR_OV12870_10BIT_1M_NOWDR_ATTR, sizeof(combo_dev_attr_t));
            break;
        case OMNIVISION_OV9284_MIPI_1M_60FPS_10BIT:
            memcpy_s(pstComboAttr, sizeof(combo_dev_attr_t), &MIPI_2lane_CHN0_SENSOR_OV9284_10BIT_8M_NOWDR_ATTR, sizeof(combo_dev_attr_t));
            break;

        case SONY_IMX415_MIPI_8M_30FPS_12BIT:
        case SONY_IMX415_MIPI_8M_20FPS_12BIT:
            memcpy_s(pstComboAttr, sizeof(combo_dev_attr_t), &MIPI_4lane_CHN0_SENSOR_IMX415_12BIT_8M_NOWDR_ATTR, sizeof(combo_dev_attr_t));
            break;
        case SONY_IMX415_MIPI_8M_20FPS_12BIT_WDR2TO1:
            memcpy_s(pstComboAttr, sizeof(combo_dev_attr_t), &MIPI_4lane_CHN0_SENSOR_IMX415_12BIT_8M_WDR2TO1_ATTR, sizeof(combo_dev_attr_t));
            break;
			
        case SONY_IMX334_MIPI_8M_30FPS_12BIT:
        case SONY_IMX585_MIPI_8M_30FPS_12BIT:  
            memcpy_s(pstComboAttr, sizeof(combo_dev_attr_t), &MIPI_4lane_CHN0_SENSOR_IMX334_12BIT_8M_NOWDR_ATTR, sizeof(combo_dev_attr_t));
            break;
        
        case SONY_IMX415_MIPI_2M_60FPS_12BIT:
            memcpy_s(pstComboAttr, sizeof(combo_dev_attr_t), &MIPI_4lane_CHN0_SENSOR_IMX415_12BIT_2M_NOWDR_ATTR, sizeof(combo_dev_attr_t));
            break;

        case OMNIVISION_OV2775_MIPI_2M_30FPS_12BIT:
            memcpy_s(pstComboAttr, sizeof(combo_dev_attr_t), &MIPI_4lane_CHN0_SENSOR_OV2775_12BIT_2M_NOWDR_ATTR, sizeof(combo_dev_attr_t));
            break;
        case OMNIVISION_OV2775_MIPI_2M_30FPS_12BIT_WDR2TO1:
        case OMNIVISION_OV2775_MIPI_2M_30FPS_12BIT_WDR2TO1_HLCG:
            memcpy_s(pstComboAttr, sizeof(combo_dev_attr_t), &MIPI_4lane_CHN0_SENSOR_OV2775_12BIT_2M_WDR2TO1_ATTR, sizeof(combo_dev_attr_t));
            break;
        case OMNIVISION_OV2775_MIPI_2M_30FPS_12BIT_BUILTIN:
            memcpy_s(pstComboAttr, sizeof(combo_dev_attr_t), &MIPI_4lane_CHN0_SENSOR_OV2775_12BIT_2M_BUILTIN_ATTR, sizeof(combo_dev_attr_t));
            break;
        case OMNIVISION_OV2775_2L_MIPI_2M_30FPS_12BIT:
            if (0 == MipiDev)
            {
                memcpy_s(pstComboAttr, sizeof(combo_dev_attr_t), &MIPI_2lane_CHN0_SENSOR_OV2775_12BIT_2M_NOWDR_ATTR, sizeof(combo_dev_attr_t));
            }
            else if (1 == MipiDev)
            {
                memcpy_s(pstComboAttr, sizeof(combo_dev_attr_t), &MIPI_2lane_CHN1_SENSOR_OV2775_12BIT_2M_NOWDR_ATTR, sizeof(combo_dev_attr_t));
            }
            break;
        case OMNIVISION_OV2775_2L_MIPI_2M_30FPS_12BIT_WDR2TO1:
            if (0 == MipiDev)
            {
                memcpy_s(pstComboAttr, sizeof(combo_dev_attr_t), &MIPI_2lane_CHN0_SENSOR_OV2775_12BIT_2M_WDR2TO1_ATTR, sizeof(combo_dev_attr_t));
            }
            else if (1 == MipiDev)
            {
                memcpy_s(pstComboAttr, sizeof(combo_dev_attr_t), &MIPI_2lane_CHN1_SENSOR_OV2775_12BIT_2M_WDR2TO1_ATTR, sizeof(combo_dev_attr_t));
            }
            break;

        default:
            SAMPLE_PRT("not support enSnsType: %d\n", enSnsType);
            memcpy_s(pstComboAttr, sizeof(combo_dev_attr_t), &MIPI_4lane_CHN0_SENSOR_IMX327_12BIT_2M_NOWDR_ATTR, sizeof(combo_dev_attr_t));
    }

	mppex_GetComboAttrBySns(enSnsType, MipiDev, pstComboAttr);

    return HI_SUCCESS;
}

HI_S32 SAMPLE_COMM_VI_SetMipiAttr(SAMPLE_VI_CONFIG_S* pstViConfig)
{
    HI_S32              i = 0;
    HI_S32              s32ViNum = 0;
    HI_S32              s32Ret = HI_SUCCESS;
    HI_S32              fd;
    SAMPLE_VI_INFO_S*   pstViInfo = HI_NULL;
    combo_dev_attr_t    stcomboDevAttr;

    if (!pstViConfig)
    {
        SAMPLE_PRT("%s: null ptr\n", __FUNCTION__);
        return HI_FAILURE;
    }

    fd = open(MIPI_DEV_NODE, O_RDWR);

    if (fd < 0)
    {
        SAMPLE_PRT("open hi_mipi dev failed\n");
        return HI_FAILURE;
    }

    for (i = 0; i < pstViConfig->s32WorkingViNum; i++)
    {
        s32ViNum  = pstViConfig->as32WorkingViId[i];
        pstViInfo = &pstViConfig->astViInfo[s32ViNum];

        SAMPLE_COMM_VI_GetComboAttrBySns(pstViInfo->stSnsInfo.enSnsType, pstViInfo->stSnsInfo.MipiDev, &stcomboDevAttr);
        stcomboDevAttr.devno = pstViInfo->stSnsInfo.MipiDev;

        SAMPLE_PRT("============= MipiDev %d, SetMipiAttr enWDRMode: %d\n", pstViInfo->stSnsInfo.MipiDev, pstViInfo->stDevInfo.enWDRMode);

        s32Ret = ioctl(fd, HI_MIPI_SET_DEV_ATTR, &stcomboDevAttr);

        if (HI_SUCCESS != s32Ret)
        {
            SAMPLE_PRT("MIPI_SET_DEV_ATTR failed\n");
            goto EXIT;
        }
    }

EXIT:
    close(fd);

    return s32Ret;
}


HI_U32 SAMPLE_COMM_VI_GetMipiLaneDivideMode(SAMPLE_VI_CONFIG_S* pstViConfig)
{
    lane_divide_mode_t lane_divide_mode;

    if ((SONY_IMX327_2L_MIPI_2M_30FPS_12BIT == pstViConfig->astViInfo[0].stSnsInfo.enSnsType) ||
        (SONY_IMX327_2L_MIPI_2M_30FPS_12BIT_WDR2TO1 == pstViConfig->astViInfo[0].stSnsInfo.enSnsType) ||
		    (SONY_IMX290_2L_MIPI_2M_30FPS_12BIT == pstViConfig->astViInfo[0].stSnsInfo.enSnsType) ||
        (SONY_IMX290_2L_MIPI_2M_30FPS_12BIT_WDR2TO1 == pstViConfig->astViInfo[0].stSnsInfo.enSnsType) ||
        (SONY_IMX307_2L_MIPI_2M_30FPS_12BIT == pstViConfig->astViInfo[0].stSnsInfo.enSnsType) ||
        (SONY_IMX307_2L_MIPI_2M_30FPS_12BIT_WDR2TO1 == pstViConfig->astViInfo[0].stSnsInfo.enSnsType) ||
        (OMNIVISION_OS05A_2L_MIPI_1M_30FPS_10BIT == pstViConfig->astViInfo[0].stSnsInfo.enSnsType) ||
        (OMNIVISION_OS05A_2L_MIPI_4M_30FPS_12BIT == pstViConfig->astViInfo[0].stSnsInfo.enSnsType) ||
        (OMNIVISION_OS05A_2L_MIPI_4M_30FPS_10BIT_WDR2TO1 == pstViConfig->astViInfo[0].stSnsInfo.enSnsType) ||
        (SONY_IMX307_2L_SLAVE_MIPI_2M_30FPS_12BIT == pstViConfig->astViInfo[0].stSnsInfo.enSnsType) ||
        (SONY_IMX307_2L_SLAVE_MIPI_2M_30FPS_12BIT_WDR2TO1 == pstViConfig->astViInfo[0].stSnsInfo.enSnsType) ||
        (OMNIVISION_OV2775_2L_MIPI_2M_30FPS_12BIT == pstViConfig->astViInfo[0].stSnsInfo.enSnsType) ||
        (OMNIVISION_OV2775_2L_MIPI_2M_30FPS_12BIT_WDR2TO1 == pstViConfig->astViInfo[0].stSnsInfo.enSnsType) ||
        (SONY_IMX335_2L_MIPI_5M_30FPS_10BIT == pstViConfig->astViInfo[0].stSnsInfo.enSnsType)
        )
    {
        lane_divide_mode = LANE_DIVIDE_MODE_1;
    }
    else
    {
        lane_divide_mode = LANE_DIVIDE_MODE_0;
    }

    return (HI_U32)lane_divide_mode;
}

/*****************************************************************************
* function : init mipi
*****************************************************************************/
HI_S32 SAMPLE_COMM_VI_StartMIPI(SAMPLE_VI_CONFIG_S* pstViConfig)
{
    HI_S32 s32Ret = HI_SUCCESS;
    lane_divide_mode_t lane_divide_mode = LANE_DIVIDE_MODE_0;

    if (!pstViConfig)
    {
        SAMPLE_PRT("%s: null ptr\n", __FUNCTION__);
        return HI_FAILURE;
    }

    lane_divide_mode = SAMPLE_COMM_VI_GetMipiLaneDivideMode(pstViConfig);

    s32Ret = SAMPLE_COMM_VI_SetMipiHsMode(lane_divide_mode);

    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("SAMPLE_COMM_VI_SetMipiHsMode failed!\n");

        return HI_FAILURE;
    }

    s32Ret = SAMPLE_COMM_VI_EnableMipiClock(pstViConfig);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("SAMPLE_COMM_VI_EnableMipiClock failed!\n");

        return HI_FAILURE;
    }

    s32Ret = SAMPLE_COMM_VI_ResetMipi(pstViConfig);

    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("SAMPLE_COMM_VI_ResetMipi failed!\n");

        return HI_FAILURE;
    }

    s32Ret = SAMPLE_COMM_VI_EnableSensorClock(pstViConfig);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("SAMPLE_COMM_VI_EnableSensorClock failed!\n");

        return HI_FAILURE;
    }

    s32Ret = SAMPLE_COMM_VI_ResetSensor(pstViConfig);

    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("SAMPLE_COMM_VI_ResetSensor failed!\n");

        return HI_FAILURE;
    }

    s32Ret = SAMPLE_COMM_VI_SetMipiAttr(pstViConfig);

    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("SAMPLE_COMM_VI_SetMipiAttr failed!\n");

        return HI_FAILURE;
    }

    s32Ret = SAMPLE_COMM_VI_UnresetMipi(pstViConfig);

    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("SAMPLE_COMM_VI_UnresetMipi failed!\n");

        return HI_FAILURE;
    }

    s32Ret = SAMPLE_COMM_VI_UnresetSensor(pstViConfig);

    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("SAMPLE_COMM_VI_UnresetSensor failed!\n");

        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

HI_S32 SAMPLE_COMM_VI_StopMIPI(SAMPLE_VI_CONFIG_S* pstViConfig)
{
    HI_S32 s32Ret = HI_SUCCESS;

    if (!pstViConfig)
    {
        SAMPLE_PRT("%s: null ptr\n", __FUNCTION__);
        return HI_FAILURE;
    }

    s32Ret = SAMPLE_COMM_VI_ResetSensor(pstViConfig);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("SAMPLE_COMM_VI_ResetSensor failed!\n");

        return HI_FAILURE;
    }

    s32Ret = SAMPLE_COMM_VI_DisableSensorClock(pstViConfig);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("SAMPLE_COMM_VI_DisableSensorClock failed!\n");

        return HI_FAILURE;
    }

    s32Ret = SAMPLE_COMM_VI_ResetMipi(pstViConfig);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("SAMPLE_COMM_VI_ResetMipi failed!\n");

        return HI_FAILURE;
    }

    s32Ret = SAMPLE_COMM_VI_DisableMipiClock(pstViConfig);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("SAMPLE_COMM_VI_DisableMipiClock failed!\n");

        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

HI_S32 SAMPLE_COMM_VI_UpdateVIVPSSMode(VI_VPSS_MODE_S* pstVIVPSSMode)
{
    HI_S32   i = 0;

    if (VI_OFFLINE_VPSS_OFFLINE == pstVIVPSSMode->aenMode[0])
    {
        for (i = 0; i < VI_MAX_PIPE_NUM; i++)
        {
            if (VI_OFFLINE_VPSS_ONLINE == pstVIVPSSMode->aenMode[i])
            {
                for (i = 0; i < VI_MAX_PIPE_NUM; i++)
                {
                    pstVIVPSSMode->aenMode[i] = VI_OFFLINE_VPSS_ONLINE;
                }

                break;
            }
        }
    }
    else if (VI_OFFLINE_VPSS_ONLINE == pstVIVPSSMode->aenMode[0])
    {
        for (i = 0; i < VI_MAX_PIPE_NUM; i++)
        {
            pstVIVPSSMode->aenMode[i] = pstVIVPSSMode->aenMode[0];
        }
    }

    return HI_SUCCESS;
}

HI_S32 SAMPLE_COMM_VI_SetParam(SAMPLE_VI_CONFIG_S* pstViConfig)
{
    HI_S32              i;
    HI_S32              s32ViNum;
    HI_S32              s32Ret;
    VI_PIPE             ViPipe;
    VI_VPSS_MODE_S      stVIVPSSMode;
    SAMPLE_VI_INFO_S*   pstViInfo = HI_NULL;

    if (!pstViConfig)
    {
        SAMPLE_PRT("%s: null ptr\n", __FUNCTION__);
        return HI_FAILURE;
    }

    s32Ret = HI_MPI_SYS_GetVIVPSSMode(&stVIVPSSMode);

    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("Get VI-VPSS mode Param failed with %#x!\n", s32Ret);

        return HI_FAILURE;
    }

    for (i = 0; i < pstViConfig->s32WorkingViNum; i++)
    {
        s32ViNum  = pstViConfig->as32WorkingViId[i];
        pstViInfo = &pstViConfig->astViInfo[s32ViNum];
        ViPipe    = pstViInfo->stPipeInfo.aPipe[0];
        stVIVPSSMode.aenMode[ViPipe] = pstViInfo->stPipeInfo.enMastPipeMode;

        if ((pstViInfo->stPipeInfo.bMultiPipe == HI_TRUE)
            || (VI_OFFLINE_VPSS_ONLINE == pstViInfo->stPipeInfo.enMastPipeMode))
        {
            SAMPLE_COMM_VI_UpdateVIVPSSMode(&stVIVPSSMode);

            ViPipe = pstViInfo->stPipeInfo.aPipe[1];
            if(ViPipe != -1)
            {
                stVIVPSSMode.aenMode[ViPipe] = pstViInfo->stPipeInfo.enMastPipeMode;
            }

            ViPipe = pstViInfo->stPipeInfo.aPipe[2];
            if(ViPipe != -1)
            {
                stVIVPSSMode.aenMode[ViPipe] = pstViInfo->stPipeInfo.enMastPipeMode;
            }

            ViPipe = pstViInfo->stPipeInfo.aPipe[3];
            if(ViPipe != -1)
            {
                stVIVPSSMode.aenMode[ViPipe] = pstViInfo->stPipeInfo.enMastPipeMode;
            }
        }

        if ((pstViInfo->stSnapInfo.bSnap) && (pstViInfo->stSnapInfo.bDoublePipe))
        {
            ViPipe    = pstViInfo->stPipeInfo.aPipe[1];
            if(ViPipe != -1)
            {
                stVIVPSSMode.aenMode[ViPipe] = pstViInfo->stSnapInfo.enSnapPipeMode;
            }
        }
    }
    s32Ret = HI_MPI_SYS_SetVIVPSSMode(&stVIVPSSMode);

    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("Set VI-VPSS mode Param failed with %#x!\n", s32Ret);

        return HI_FAILURE;
    }

    return HI_SUCCESS;
}


HI_S32 SAMPLE_COMM_VI_GetDevAttrBySns(SAMPLE_SNS_TYPE_E enSnsType, VI_DEV_ATTR_S* pstViDevAttr)
{
    switch (enSnsType)
    {
        case SONY_IMX327_MIPI_2M_30FPS_12BIT:
        case SONY_IMX327_2L_MIPI_2M_30FPS_12BIT:
        case SONY_IMX327_2L_MIPI_2M_30FPS_12BIT_WDR2TO1:
        case SONY_IMX385_MIPI_2M_30FPS_12BIT:
        case SONY_IMX482_MIPI_2M_30FPS_12BIT:
        case SONY_IMX585_MIPI_2M_30FPS_12BIT:  
            memcpy_s(pstViDevAttr, sizeof(VI_DEV_ATTR_S), &DEV_ATTR_IMX327_2M_BASE, sizeof(VI_DEV_ATTR_S));
            break;

        case SONY_IMX327_MIPI_2M_30FPS_12BIT_WDR2TO1:
            memcpy_s(pstViDevAttr, sizeof(VI_DEV_ATTR_S), &DEV_ATTR_IMX327_2M_BASE, sizeof(VI_DEV_ATTR_S));
            pstViDevAttr->au32ComponentMask[0] = 0xFFC00000;
            break;

        case SONY_IMX390_MIPI_2M_30FPS_12BIT:
            memcpy_s(pstViDevAttr, sizeof(VI_DEV_ATTR_S), &DEV_ATTR_IMX390_2M_BASE, sizeof(VI_DEV_ATTR_S));
            break;
        case SONY_IMX290_MIPI_2M_30FPS_12BIT:
        case SONY_IMX290_2L_MIPI_2M_30FPS_12BIT:
            memcpy_s(pstViDevAttr, sizeof(VI_DEV_ATTR_S), &DEV_ATTR_IMX290_2M_BASE, sizeof(VI_DEV_ATTR_S));
            break;
            
        case SONY_IMX290_2L_MIPI_2M_30FPS_12BIT_WDR2TO1:
        case SONY_IMX290_MIPI_2M_30FPS_12BIT_WDR2TO1:
        case SONY_IMX290_MIPI_2M_120FPS_12BIT:
            memcpy_s(pstViDevAttr, sizeof(VI_DEV_ATTR_S), &DEV_ATTR_IMX290_2M_BASE, sizeof(VI_DEV_ATTR_S));
            pstViDevAttr->au32ComponentMask[0] = 0xFFC00000;
            break;

        case SONY_IMX307_MIPI_2M_30FPS_12BIT:
        case SONY_IMX307_2L_MIPI_2M_30FPS_12BIT:
        case SONY_IMX307_2L_SLAVE_MIPI_2M_30FPS_12BIT:
        case SONY_IMX307_2L_SLAVE_MIPI_2M_30FPS_12BIT_WDR2TO1:
            memcpy_s(pstViDevAttr, sizeof(VI_DEV_ATTR_S), &DEV_ATTR_IMX307_2M_BASE, sizeof(VI_DEV_ATTR_S));
            break;

        case SONY_IMX307_MIPI_2M_30FPS_12BIT_WDR2TO1:
        case SONY_IMX307_2L_MIPI_2M_30FPS_12BIT_WDR2TO1:
            memcpy_s(pstViDevAttr, sizeof(VI_DEV_ATTR_S), &DEV_ATTR_IMX307_2M_BASE, sizeof(VI_DEV_ATTR_S));
            pstViDevAttr->au32ComponentMask[0] = 0xFFC00000;
            break;

        case PRIMESENSOR_PS5260_2L_MIPI_2M_30FPS_12BIT:
        case PRIMESENSOR_PS5260_2L_MIPI_2M_30FPS_12BIT_BUILTIN:
            memcpy_s(pstViDevAttr, sizeof(VI_DEV_ATTR_S), &DEV_ATTR_PS5260_2M_BASE, sizeof(VI_DEV_ATTR_S));
            break;

        case PANASONIC_MN34220_LVDS_2M_30FPS_12BIT:
            memcpy_s(pstViDevAttr, sizeof(VI_DEV_ATTR_S), &DEV_ATTR_MN34220_2M_BASE, sizeof(VI_DEV_ATTR_S));
            break;

        case SONY_IMX335_2L_MIPI_5M_30FPS_10BIT: 
        case SONY_IMX335_MIPI_5M_30FPS_12BIT:
            memcpy_s(pstViDevAttr, sizeof(VI_DEV_ATTR_S), &DEV_ATTR_IMX335_5M_BASE, sizeof(VI_DEV_ATTR_S));
            break;

        case SONY_IMX335_MIPI_5M_30FPS_10BIT_WDR2TO1:
            memcpy_s(pstViDevAttr, sizeof(VI_DEV_ATTR_S), &DEV_ATTR_IMX335_5M_BASE, sizeof(VI_DEV_ATTR_S));
            pstViDevAttr->au32ComponentMask[0] = 0xFFC00000;
            break;

        case SONY_IMX335_MIPI_4M_30FPS_12BIT:
            memcpy_s(pstViDevAttr, sizeof(VI_DEV_ATTR_S), &DEV_ATTR_IMX335_4M_BASE, sizeof(VI_DEV_ATTR_S));
            break;

        case SONY_IMX335_MIPI_4M_30FPS_10BIT_WDR2TO1:
            memcpy_s(pstViDevAttr, sizeof(VI_DEV_ATTR_S), &DEV_ATTR_IMX335_4M_BASE, sizeof(VI_DEV_ATTR_S));
            pstViDevAttr->au32ComponentMask[0] = 0xFFC00000;
            break;

        case SMART_SC4210_MIPI_3M_30FPS_12BIT:
        case SMART_SC4210_MIPI_3M_30FPS_10BIT_WDR2TO1:
            memcpy_s(pstViDevAttr, sizeof(VI_DEV_ATTR_S), &DEV_ATTR_SC4210_3M_BASE, sizeof(VI_DEV_ATTR_S));
            break;

        case SONY_IMX458_MIPI_8M_30FPS_10BIT:
            memcpy(pstViDevAttr, &DEV_ATTR_IMX458_8M_BASE, sizeof(VI_DEV_ATTR_S));
            break;
        case SONY_IMX458_MIPI_12M_20FPS_10BIT:
            memcpy(pstViDevAttr, &DEV_ATTR_IMX458_12M_BASE, sizeof(VI_DEV_ATTR_S));
            break;
        case SONY_IMX458_MIPI_4M_60FPS_10BIT:
        case SONY_IMX458_MIPI_4M_40FPS_10BIT:
            memcpy(pstViDevAttr, &DEV_ATTR_IMX458_4M_BASE, sizeof(VI_DEV_ATTR_S));
            break;
        case SONY_IMX458_MIPI_2M_90FPS_10BIT:
            memcpy(pstViDevAttr, &DEV_ATTR_IMX458_2M_BASE, sizeof(VI_DEV_ATTR_S));
            break;
        case SONY_IMX458_MIPI_1M_129FPS_10BIT:
            memcpy(pstViDevAttr, &DEV_ATTR_IMX458_1M_BASE, sizeof(VI_DEV_ATTR_S));
            break;
        case GALAXYCORE_GC2053_MIPI_2M_30FPS_10BIT:
            memcpy_s(pstViDevAttr, sizeof(VI_DEV_ATTR_S), &DEV_ATTR_GC2053_2M_BASE, sizeof(VI_DEV_ATTR_S));
            pstViDevAttr->au32ComponentMask[0] = 0xFFC00000;
            break;
        case OMNIVISION_OV12870_MIPI_8M_30FPS_10BIT:
            memcpy(pstViDevAttr, &DEV_ATTR_OV12870_8M_BASE, sizeof(VI_DEV_ATTR_S));
            break;
        case OMNIVISION_OV12870_MIPI_12M_30FPS_10BIT:
            memcpy(pstViDevAttr, &DEV_ATTR_OV12870_12M_BASE, sizeof(VI_DEV_ATTR_S));
            break;
        case OMNIVISION_OV12870_MIPI_2M_120FPS_10BIT:
            memcpy(pstViDevAttr, &DEV_ATTR_OV12870_2M_BASE, sizeof(VI_DEV_ATTR_S));
            break;
        case OMNIVISION_OV12870_MIPI_1M_240FPS_10BIT:
            memcpy(pstViDevAttr, &DEV_ATTR_OV12870_1M_BASE, sizeof(VI_DEV_ATTR_S));
            break;

        case OMNIVISION_OV9284_MIPI_1M_60FPS_10BIT:
            memcpy(pstViDevAttr, &DEV_ATTR_OV9284_1M_BASE, sizeof(VI_DEV_ATTR_S));
            break;

        case OMNIVISION_OS04B10_MIPI_4M_30FPS_10BIT:
            memcpy_s(pstViDevAttr, sizeof(VI_DEV_ATTR_S), &DEV_ATTR_OS04B10_4M_BASE, sizeof(VI_DEV_ATTR_S));
            break;

        case OMNIVISION_OS05A_MIPI_4M_30FPS_12BIT:
            memcpy_s(pstViDevAttr, sizeof(VI_DEV_ATTR_S), &DEV_ATTR_OS05A_4M_BASE, sizeof(VI_DEV_ATTR_S));
            break;
        case OMNIVISION_OS05A_MIPI_4M_30FPS_10BIT_WDR2TO1:
            memcpy_s(pstViDevAttr, sizeof(VI_DEV_ATTR_S), &DEV_ATTR_OS05A_4M_BASE, sizeof(VI_DEV_ATTR_S));
            pstViDevAttr->au32ComponentMask[0] = 0xFFC00000;
            break;
        case OMNIVISION_OS05A_2L_MIPI_1M_30FPS_10BIT:
            memcpy_s(pstViDevAttr, sizeof(VI_DEV_ATTR_S), &DEV_ATTR_OS05A_2L_1M_BASE, sizeof(VI_DEV_ATTR_S));
            break;
        case OMNIVISION_OS05A_2L_MIPI_4M_30FPS_12BIT:
        case OMNIVISION_OS05A_2L_MIPI_4M_30FPS_10BIT_WDR2TO1:
            memcpy_s(pstViDevAttr, sizeof(VI_DEV_ATTR_S), &DEV_ATTR_OS05A_2L_4M_BASE, sizeof(VI_DEV_ATTR_S));
            break;

        case OMNIVISION_OS08A10_MIPI_8M_30FPS_10BIT:
            memcpy_s(pstViDevAttr, sizeof(VI_DEV_ATTR_S), &DEV_ATTR_OS08A10_8M_BASE, sizeof(VI_DEV_ATTR_S));
            pstViDevAttr->au32ComponentMask[0] = 0xFFC00000;
            break;

        case SONY_IMX415_MIPI_8M_30FPS_12BIT:
        case SONY_IMX415_MIPI_8M_20FPS_12BIT:
        case SONY_IMX415_MIPI_8M_20FPS_12BIT_WDR2TO1:
            memcpy(pstViDevAttr, &DEV_ATTR_IMX415_8M_BASE, sizeof(VI_DEV_ATTR_S));
            break;
        case SONY_IMX334_MIPI_8M_30FPS_12BIT:
        case SONY_IMX585_MIPI_8M_30FPS_12BIT:  
            memcpy(pstViDevAttr, &DEV_ATTR_IMX334_8M_BASE, sizeof(VI_DEV_ATTR_S));
            break;

        case SONY_IMX415_MIPI_2M_60FPS_12BIT:
            memcpy(pstViDevAttr, &DEV_ATTR_IMX415_2M_BASE, sizeof(VI_DEV_ATTR_S));
            break;
        case OMNIVISION_OV2775_MIPI_2M_30FPS_12BIT:
        case OMNIVISION_OV2775_MIPI_2M_30FPS_12BIT_WDR2TO1:
        case OMNIVISION_OV2775_MIPI_2M_30FPS_12BIT_WDR2TO1_HLCG:
        case OMNIVISION_OV2775_MIPI_2M_30FPS_12BIT_BUILTIN:
        case OMNIVISION_OV2775_2L_MIPI_2M_30FPS_12BIT:
        case OMNIVISION_OV2775_2L_MIPI_2M_30FPS_12BIT_WDR2TO1:
            memcpy(pstViDevAttr, &DEV_ATTR_OV2775_2M_BASE, sizeof(VI_DEV_ATTR_S));
            break;

        default:
            memcpy_s(pstViDevAttr, sizeof(VI_DEV_ATTR_S), &DEV_ATTR_IMX327_2M_BASE, sizeof(VI_DEV_ATTR_S));
    }
	
	mppex_GetDevAttrBySns(enSnsType, pstViDevAttr);
    return HI_SUCCESS;
}

HI_S32 SAMPLE_COMM_VI_GetPipeAttrBySns(SAMPLE_SNS_TYPE_E enSnsType, VI_PIPE_ATTR_S* pstPipeAttr)
{
    switch (enSnsType)
    {
        case SONY_IMX327_MIPI_2M_30FPS_12BIT:
        case SONY_IMX327_2L_MIPI_2M_30FPS_12BIT:
        case SONY_IMX327_2L_MIPI_2M_30FPS_12BIT_WDR2TO1:
        case SONY_IMX385_MIPI_2M_30FPS_12BIT:
        case SONY_IMX482_MIPI_2M_30FPS_12BIT:
        case SONY_IMX585_MIPI_2M_30FPS_12BIT:  
            memcpy_s(pstPipeAttr, sizeof(VI_PIPE_ATTR_S), &PIPE_ATTR_1920x1080_RAW12_420_3DNR_RFR, sizeof(VI_PIPE_ATTR_S));
            break;

        case SONY_IMX327_MIPI_2M_30FPS_12BIT_WDR2TO1:
            memcpy_s(pstPipeAttr, sizeof(VI_PIPE_ATTR_S), &PIPE_ATTR_1920x1080_RAW12_420_3DNR_RFR, sizeof(VI_PIPE_ATTR_S));
            pstPipeAttr->enPixFmt = PIXEL_FORMAT_RGB_BAYER_10BPP;
            pstPipeAttr->enBitWidth = DATA_BITWIDTH_10;
            break;

        case SONY_IMX390_MIPI_2M_30FPS_12BIT:
            memcpy_s(pstPipeAttr, sizeof(VI_PIPE_ATTR_S), &PIPE_ATTR_1920x1080_RAW12_420_3DNR_RFR, sizeof(VI_PIPE_ATTR_S));
            break;

        case SONY_IMX290_MIPI_2M_30FPS_12BIT:
        case SONY_IMX290_2L_MIPI_2M_30FPS_12BIT:
            memcpy_s(pstPipeAttr, sizeof(VI_PIPE_ATTR_S), &PIPE_ATTR_1920x1080_RAW12_420_3DNR_RFR, sizeof(VI_PIPE_ATTR_S));
            break;

        case SONY_IMX290_2L_MIPI_2M_30FPS_12BIT_WDR2TO1:
        case SONY_IMX290_MIPI_2M_30FPS_12BIT_WDR2TO1:
        case SONY_IMX290_MIPI_2M_120FPS_12BIT:
            memcpy_s(pstPipeAttr, sizeof(VI_PIPE_ATTR_S), &PIPE_ATTR_1920x1080_RAW12_420_3DNR_RFR, sizeof(VI_PIPE_ATTR_S));
            pstPipeAttr->enPixFmt = PIXEL_FORMAT_RGB_BAYER_10BPP;
            pstPipeAttr->enBitWidth = DATA_BITWIDTH_10;
            break;
        case SONY_IMX307_MIPI_2M_30FPS_12BIT:
        case SONY_IMX307_2L_MIPI_2M_30FPS_12BIT:
        case SONY_IMX307_2L_SLAVE_MIPI_2M_30FPS_12BIT:
        case SONY_IMX307_2L_SLAVE_MIPI_2M_30FPS_12BIT_WDR2TO1:
            memcpy_s(pstPipeAttr, sizeof(VI_PIPE_ATTR_S), &PIPE_ATTR_1920x1080_RAW12_420_3DNR_RFR, sizeof(VI_PIPE_ATTR_S));
            break;

        case SONY_IMX307_MIPI_2M_30FPS_12BIT_WDR2TO1:
        case SONY_IMX307_2L_MIPI_2M_30FPS_12BIT_WDR2TO1:
            memcpy_s(pstPipeAttr, sizeof(VI_PIPE_ATTR_S), &PIPE_ATTR_1920x1080_RAW12_420_3DNR_RFR, sizeof(VI_PIPE_ATTR_S));
            pstPipeAttr->enPixFmt = PIXEL_FORMAT_RGB_BAYER_10BPP;
            pstPipeAttr->enBitWidth = DATA_BITWIDTH_10;
            break;

        case PRIMESENSOR_PS5260_2L_MIPI_2M_30FPS_12BIT:
        case PRIMESENSOR_PS5260_2L_MIPI_2M_30FPS_12BIT_BUILTIN:
            memcpy_s(pstPipeAttr, sizeof(VI_PIPE_ATTR_S), &PIPE_ATTR_1920x1080_RAW12_420_3DNR_RFR, sizeof(VI_PIPE_ATTR_S));
            break;

        case PANASONIC_MN34220_LVDS_2M_30FPS_12BIT:
            memcpy_s(pstPipeAttr, sizeof(VI_PIPE_ATTR_S), &PIPE_ATTR_1920x1080_RAW12_420_3DNR_RFR, sizeof(VI_PIPE_ATTR_S));
            break;

        case SONY_IMX335_2L_MIPI_5M_30FPS_10BIT:
            memcpy_s(pstPipeAttr, sizeof(VI_PIPE_ATTR_S), &PIPE_ATTR_2592x1944_RAW10_420_3DNR_RFR, sizeof(VI_PIPE_ATTR_S));
            break;
        case SONY_IMX335_MIPI_5M_30FPS_12BIT:
            memcpy_s(pstPipeAttr, sizeof(VI_PIPE_ATTR_S), &PIPE_ATTR_2592x1944_RAW12_420_3DNR_RFR, sizeof(VI_PIPE_ATTR_S));
            break;

        case SONY_IMX335_MIPI_5M_30FPS_10BIT_WDR2TO1:
            memcpy_s(pstPipeAttr, sizeof(VI_PIPE_ATTR_S), &PIPE_ATTR_2592x1944_RAW10_420_3DNR_RFR, sizeof(VI_PIPE_ATTR_S));
            break;

        case SONY_IMX335_MIPI_4M_30FPS_12BIT:
            memcpy_s(pstPipeAttr, sizeof(VI_PIPE_ATTR_S), &PIPE_ATTR_2592x1536_RAW12_420_3DNR_RFR, sizeof(VI_PIPE_ATTR_S));
            break;

        case SONY_IMX335_MIPI_4M_30FPS_10BIT_WDR2TO1:
            memcpy_s(pstPipeAttr, sizeof(VI_PIPE_ATTR_S), &PIPE_ATTR_2592x1536_RAW10_420_3DNR_RFR, sizeof(VI_PIPE_ATTR_S));
            break;

        case SMART_SC4210_MIPI_3M_30FPS_12BIT:
            memcpy_s(pstPipeAttr, sizeof(VI_PIPE_ATTR_S), &PIPE_ATTR_2560x1440_RAW12_420_3DNR_RFR, sizeof(VI_PIPE_ATTR_S));
            break;

        case SMART_SC4210_MIPI_3M_30FPS_10BIT_WDR2TO1:
            memcpy_s(pstPipeAttr, sizeof(VI_PIPE_ATTR_S), &PIPE_ATTR_2560x1440_RAW10_420_3DNR_RFR, sizeof(VI_PIPE_ATTR_S));
            break;

        case OMNIVISION_OS08A10_MIPI_8M_30FPS_10BIT:
        case SONY_IMX458_MIPI_8M_30FPS_10BIT:
            memcpy_s(pstPipeAttr, sizeof(VI_PIPE_ATTR_S), &PIPE_ATTR_3840x2160_RAW10_420_3DNR_RFR, sizeof(VI_PIPE_ATTR_S));
            break;
        case SONY_IMX458_MIPI_12M_20FPS_10BIT:
            memcpy_s(pstPipeAttr, sizeof(VI_PIPE_ATTR_S), &PIPE_ATTR_4000x3000_RAW10_420_3DNR_RFR, sizeof(VI_PIPE_ATTR_S));
            break;
        case SONY_IMX458_MIPI_4M_60FPS_10BIT:
        case SONY_IMX458_MIPI_4M_40FPS_10BIT:
            memcpy_s(pstPipeAttr, sizeof(VI_PIPE_ATTR_S), &PIPE_ATTR_2716x1524_RAW10_420_3DNR_RFR, sizeof(VI_PIPE_ATTR_S));
            break;
        case SONY_IMX458_MIPI_2M_90FPS_10BIT:
            memcpy_s(pstPipeAttr, sizeof(VI_PIPE_ATTR_S), &PIPE_ATTR_1920x1080_RAW10_420_3DNR_RFR, sizeof(VI_PIPE_ATTR_S));
            break;
        case SONY_IMX458_MIPI_1M_129FPS_10BIT:
            memcpy_s(pstPipeAttr, sizeof(VI_PIPE_ATTR_S), &PIPE_ATTR_1280x720_RAW10_420_3DNR_RFR, sizeof(VI_PIPE_ATTR_S));
            break;
        case GALAXYCORE_GC2053_MIPI_2M_30FPS_10BIT:
            memcpy_s(pstPipeAttr, sizeof(VI_PIPE_ATTR_S), &PIPE_ATTR_1920x1080_RAW10_420_3DNR_RFR, sizeof(VI_PIPE_ATTR_S));
            break;
        case OMNIVISION_OV12870_MIPI_8M_30FPS_10BIT:
            memcpy_s(pstPipeAttr, sizeof(VI_PIPE_ATTR_S), &PIPE_ATTR_3840x2160_RAW10_420_3DNR_RFR, sizeof(VI_PIPE_ATTR_S));
            break;
        case OMNIVISION_OV12870_MIPI_12M_30FPS_10BIT:
            memcpy_s(pstPipeAttr, sizeof(VI_PIPE_ATTR_S), &PIPE_ATTR_4000x3000_RAW10_420_3DNR_RFR, sizeof(VI_PIPE_ATTR_S));
            break;
        case OMNIVISION_OV12870_MIPI_2M_120FPS_10BIT:
            memcpy_s(pstPipeAttr, sizeof(VI_PIPE_ATTR_S), &PIPE_ATTR_1920x1080_RAW10_420_3DNR_RFR, sizeof(VI_PIPE_ATTR_S));
            break;
        case OMNIVISION_OV12870_MIPI_1M_240FPS_10BIT:
            memcpy_s(pstPipeAttr, sizeof(VI_PIPE_ATTR_S), &PIPE_ATTR_1280x720_RAW10_420_3DNR_RFR, sizeof(VI_PIPE_ATTR_S));
            break;

        case OMNIVISION_OV9284_MIPI_1M_60FPS_10BIT:
            memcpy_s(pstPipeAttr, sizeof(VI_PIPE_ATTR_S), &PIPE_ATTR_1280x800_RAW10_420_3DNR_RFR, sizeof(VI_PIPE_ATTR_S));
            break;

        case OMNIVISION_OS04B10_MIPI_4M_30FPS_10BIT:
            memcpy_s(pstPipeAttr, sizeof(VI_PIPE_ATTR_S), &PIPE_ATTR_2560x1440_RAW10_420_3DNR_RFR, sizeof(VI_PIPE_ATTR_S));
            break;

        case OMNIVISION_OS05A_MIPI_4M_30FPS_12BIT:
            memcpy_s(pstPipeAttr, sizeof(VI_PIPE_ATTR_S), &PIPE_ATTR_2688x1536_RAW12_420_3DNR_RFR, sizeof(VI_PIPE_ATTR_S));
            break;
        case OMNIVISION_OS05A_MIPI_4M_30FPS_10BIT_WDR2TO1:
            memcpy_s(pstPipeAttr, sizeof(VI_PIPE_ATTR_S), &PIPE_ATTR_2688x1536_RAW10_420_3DNR_RFR, sizeof(VI_PIPE_ATTR_S));
            break;
        case OMNIVISION_OS05A_2L_MIPI_1M_30FPS_10BIT:
            memcpy_s(pstPipeAttr, sizeof(VI_PIPE_ATTR_S), &PIPE_ATTR_1280x720_RAW10_420_3DNR_RFR, sizeof(VI_PIPE_ATTR_S));
            break;
        case OMNIVISION_OS05A_2L_MIPI_4M_30FPS_12BIT:
        case OMNIVISION_OS05A_2L_MIPI_4M_30FPS_10BIT_WDR2TO1:
            memcpy_s(pstPipeAttr, sizeof(VI_PIPE_ATTR_S), &PIPE_ATTR_2560x1440_RAW10_420_3DNR_RFR, sizeof(VI_PIPE_ATTR_S));
            break;

        case SONY_IMX415_MIPI_8M_30FPS_12BIT:
        case SONY_IMX415_MIPI_8M_20FPS_12BIT:
		    case SONY_IMX334_MIPI_8M_30FPS_12BIT:
		    case SONY_IMX585_MIPI_8M_30FPS_12BIT:  
        case SONY_IMX415_MIPI_8M_20FPS_12BIT_WDR2TO1:
            memcpy_s(pstPipeAttr, sizeof(VI_PIPE_ATTR_S), &PIPE_ATTR_3840x2160_RAW12_420_3DNR_RFR, sizeof(VI_PIPE_ATTR_S));
            break;


        case SONY_IMX415_MIPI_2M_60FPS_12BIT:
            memcpy_s(pstPipeAttr, sizeof(VI_PIPE_ATTR_S), &PIPE_ATTR_1920x1080_RAW12_420_3DNR_RFR, sizeof(VI_PIPE_ATTR_S));
            break;

        case OMNIVISION_OV2775_MIPI_2M_30FPS_12BIT:
        case OMNIVISION_OV2775_MIPI_2M_30FPS_12BIT_WDR2TO1:
        case OMNIVISION_OV2775_MIPI_2M_30FPS_12BIT_WDR2TO1_HLCG:
        case OMNIVISION_OV2775_MIPI_2M_30FPS_12BIT_BUILTIN:
        case OMNIVISION_OV2775_2L_MIPI_2M_30FPS_12BIT:
        case OMNIVISION_OV2775_2L_MIPI_2M_30FPS_12BIT_WDR2TO1:
            memcpy_s(pstPipeAttr, sizeof(VI_PIPE_ATTR_S), &PIPE_ATTR_1920x1080_RAW12_420_3DNR_RFR, sizeof(VI_PIPE_ATTR_S));
            break;

        default:
            memcpy_s(pstPipeAttr, sizeof(VI_PIPE_ATTR_S), &PIPE_ATTR_1920x1080_RAW12_420_3DNR_RFR, sizeof(VI_PIPE_ATTR_S));
    }
	
	mppex_GetPipeAttrBySns(enSnsType, pstPipeAttr);
    return HI_SUCCESS;
}

HI_S32 SAMPLE_COMM_VI_GetChnAttrBySns(SAMPLE_SNS_TYPE_E enSnsType, VI_CHN_ATTR_S* pstChnAttr)
{
    switch (enSnsType)
    {
        case SONY_IMX327_MIPI_2M_30FPS_12BIT:
        case SONY_IMX327_MIPI_2M_30FPS_12BIT_WDR2TO1:
        case SONY_IMX385_MIPI_2M_30FPS_12BIT:
        case SONY_IMX482_MIPI_2M_30FPS_12BIT:
        case SONY_IMX585_MIPI_2M_30FPS_12BIT:   
        case SONY_IMX327_2L_MIPI_2M_30FPS_12BIT:
        case SONY_IMX327_2L_MIPI_2M_30FPS_12BIT_WDR2TO1:
            memcpy_s(pstChnAttr, sizeof(VI_CHN_ATTR_S), &CHN_ATTR_1920x1080_420_SDR8_LINEAR, sizeof(VI_CHN_ATTR_S));
            break;

        case SONY_IMX390_MIPI_2M_30FPS_12BIT:
        case SONY_IMX290_MIPI_2M_30FPS_12BIT:
        case SONY_IMX290_MIPI_2M_120FPS_12BIT:
        case SONY_IMX290_MIPI_2M_30FPS_12BIT_WDR2TO1:
        case SONY_IMX290_2L_MIPI_2M_30FPS_12BIT:
        case SONY_IMX290_2L_MIPI_2M_30FPS_12BIT_WDR2TO1:
            memcpy_s(pstChnAttr, sizeof(VI_CHN_ATTR_S), &CHN_ATTR_1920x1080_420_SDR8_LINEAR, sizeof(VI_CHN_ATTR_S));
            break;

        case SONY_IMX307_MIPI_2M_30FPS_12BIT:
        case SONY_IMX307_MIPI_2M_30FPS_12BIT_WDR2TO1:
        case SONY_IMX307_2L_MIPI_2M_30FPS_12BIT:
        case SONY_IMX307_2L_MIPI_2M_30FPS_12BIT_WDR2TO1:
        case SONY_IMX307_2L_SLAVE_MIPI_2M_30FPS_12BIT:
        case SONY_IMX307_2L_SLAVE_MIPI_2M_30FPS_12BIT_WDR2TO1:
            memcpy_s(pstChnAttr, sizeof(VI_CHN_ATTR_S), &CHN_ATTR_1920x1080_420_SDR8_LINEAR, sizeof(VI_CHN_ATTR_S));
            break;

        case PRIMESENSOR_PS5260_2L_MIPI_2M_30FPS_12BIT:
        case PRIMESENSOR_PS5260_2L_MIPI_2M_30FPS_12BIT_BUILTIN:
            memcpy_s(pstChnAttr, sizeof(VI_CHN_ATTR_S), &CHN_ATTR_1920x1080_420_SDR8_LINEAR, sizeof(VI_CHN_ATTR_S));
            break;

        case PANASONIC_MN34220_LVDS_2M_30FPS_12BIT:
            memcpy_s(pstChnAttr, sizeof(VI_CHN_ATTR_S), &CHN_ATTR_1920x1080_420_SDR8_LINEAR, sizeof(VI_CHN_ATTR_S));
            break;

        case SONY_IMX335_2L_MIPI_5M_30FPS_10BIT:
        case SONY_IMX335_MIPI_5M_30FPS_12BIT:
        case SONY_IMX335_MIPI_5M_30FPS_10BIT_WDR2TO1:
            memcpy_s(pstChnAttr, sizeof(VI_CHN_ATTR_S), &CHN_ATTR_2592x1944_420_SDR8_LINEAR, sizeof(VI_CHN_ATTR_S));
            break;

        case SONY_IMX335_MIPI_4M_30FPS_12BIT:
        case SONY_IMX335_MIPI_4M_30FPS_10BIT_WDR2TO1:
            memcpy_s(pstChnAttr, sizeof(VI_CHN_ATTR_S), &CHN_ATTR_2592x1536_420_SDR8_LINEAR, sizeof(VI_CHN_ATTR_S));
            break;

        case SMART_SC4210_MIPI_3M_30FPS_12BIT:
        case SMART_SC4210_MIPI_3M_30FPS_10BIT_WDR2TO1:
            memcpy_s(pstChnAttr, sizeof(VI_CHN_ATTR_S), &CHN_ATTR_2560x1440_420_SDR8_LINEAR, sizeof(VI_CHN_ATTR_S));
            break;

        case OMNIVISION_OS08A10_MIPI_8M_30FPS_10BIT:
        case SONY_IMX458_MIPI_8M_30FPS_10BIT:
            memcpy_s(pstChnAttr, sizeof(VI_CHN_ATTR_S), &CHN_ATTR_3840x2160_420_SDR8_LINEAR, sizeof(VI_CHN_ATTR_S));
            break;
        case SONY_IMX458_MIPI_12M_20FPS_10BIT:
            memcpy_s(pstChnAttr, sizeof(VI_CHN_ATTR_S), &CHN_ATTR_4000x3000_420_SDR8_LINEAR, sizeof(VI_CHN_ATTR_S));
            break;
        case SONY_IMX458_MIPI_4M_60FPS_10BIT:
        case SONY_IMX458_MIPI_4M_40FPS_10BIT:
            memcpy_s(pstChnAttr, sizeof(VI_CHN_ATTR_S), &CHN_ATTR_2716x1524_420_SDR8_LINEAR, sizeof(VI_CHN_ATTR_S));
            break;
        case SONY_IMX458_MIPI_2M_90FPS_10BIT:
            memcpy_s(pstChnAttr, sizeof(VI_CHN_ATTR_S), &CHN_ATTR_1920x1080_420_SDR8_LINEAR, sizeof(VI_CHN_ATTR_S));
            break;
        case SONY_IMX458_MIPI_1M_129FPS_10BIT:
            memcpy_s(pstChnAttr, sizeof(VI_CHN_ATTR_S), &CHN_ATTR_1280x720_420_SDR8_LINEAR, sizeof(VI_CHN_ATTR_S));
            break;
        case GALAXYCORE_GC2053_MIPI_2M_30FPS_10BIT:
            memcpy_s(pstChnAttr, sizeof(VI_CHN_ATTR_S), &CHN_ATTR_1920x1080_420_SDR8_LINEAR, sizeof(VI_CHN_ATTR_S));
            break;
        case OMNIVISION_OV12870_MIPI_8M_30FPS_10BIT:
            memcpy_s(pstChnAttr, sizeof(VI_CHN_ATTR_S), &CHN_ATTR_3840x2160_420_SDR8_LINEAR, sizeof(VI_CHN_ATTR_S));
            break;
        case OMNIVISION_OV12870_MIPI_12M_30FPS_10BIT:
            memcpy_s(pstChnAttr, sizeof(VI_CHN_ATTR_S), &CHN_ATTR_4000x3000_420_SDR8_LINEAR, sizeof(VI_CHN_ATTR_S));
            break;
        case OMNIVISION_OV12870_MIPI_2M_120FPS_10BIT:
            memcpy_s(pstChnAttr, sizeof(VI_CHN_ATTR_S), &CHN_ATTR_1920x1080_420_SDR8_LINEAR, sizeof(VI_CHN_ATTR_S));
            break;
        case OMNIVISION_OV12870_MIPI_1M_240FPS_10BIT:
            memcpy_s(pstChnAttr, sizeof(VI_CHN_ATTR_S), &CHN_ATTR_1280x720_420_SDR8_LINEAR, sizeof(VI_CHN_ATTR_S));
            break;

        case OMNIVISION_OV9284_MIPI_1M_60FPS_10BIT:
            memcpy_s(pstChnAttr, sizeof(VI_CHN_ATTR_S), &CHN_ATTR_1280x800_420_SDR8_LINEAR, sizeof(VI_CHN_ATTR_S));
            break;

        case OMNIVISION_OS04B10_MIPI_4M_30FPS_10BIT:
            memcpy_s(pstChnAttr, sizeof(VI_CHN_ATTR_S), &CHN_ATTR_2560x1440_420_SDR8_LINEAR, sizeof(VI_CHN_ATTR_S));
            break;

        case OMNIVISION_OS05A_MIPI_4M_30FPS_12BIT:
        case OMNIVISION_OS05A_MIPI_4M_30FPS_10BIT_WDR2TO1:
            memcpy_s(pstChnAttr, sizeof(VI_CHN_ATTR_S), &CHN_ATTR_2688x1536_420_SDR8_LINEAR, sizeof(VI_CHN_ATTR_S));
            break;
        case OMNIVISION_OS05A_2L_MIPI_1M_30FPS_10BIT:
            memcpy(pstChnAttr, &CHN_ATTR_1280x720_420_SDR8_LINEAR, sizeof(VI_CHN_ATTR_S));
            break;
        case OMNIVISION_OS05A_2L_MIPI_4M_30FPS_12BIT:
        case OMNIVISION_OS05A_2L_MIPI_4M_30FPS_10BIT_WDR2TO1:
            memcpy(pstChnAttr, &CHN_ATTR_2560x1440_420_SDR8_LINEAR, sizeof(VI_CHN_ATTR_S));
            break;

        case SONY_IMX415_MIPI_8M_30FPS_12BIT:
        case SONY_IMX415_MIPI_8M_20FPS_12BIT:
        case SONY_IMX415_MIPI_8M_20FPS_12BIT_WDR2TO1:
        case SONY_IMX585_MIPI_8M_30FPS_12BIT:  
            memcpy_s(pstChnAttr, sizeof(VI_CHN_ATTR_S), &CHN_ATTR_3840x2160_420_SDR8_LINEAR, sizeof(VI_CHN_ATTR_S));
            break;
        case SONY_IMX334_MIPI_8M_30FPS_12BIT:
            memcpy_s(pstChnAttr, sizeof(VI_CHN_ATTR_S), &CHN_ATTR_3840x2160_420_SDR8_LINEAR, sizeof(VI_CHN_ATTR_S));
            //maohw for sml sns board;
            //pstChnAttr->bFlip = HI_TRUE;
            break;
        case SONY_IMX415_MIPI_2M_60FPS_12BIT:
            memcpy_s(pstChnAttr, sizeof(VI_CHN_ATTR_S), &CHN_ATTR_1920x1080_420_SDR8_LINEAR, sizeof(VI_CHN_ATTR_S));
            break;

        case OMNIVISION_OV2775_MIPI_2M_30FPS_12BIT:
        case OMNIVISION_OV2775_MIPI_2M_30FPS_12BIT_WDR2TO1:
        case OMNIVISION_OV2775_MIPI_2M_30FPS_12BIT_WDR2TO1_HLCG:
        case OMNIVISION_OV2775_MIPI_2M_30FPS_12BIT_BUILTIN:
        case OMNIVISION_OV2775_2L_MIPI_2M_30FPS_12BIT:
        case OMNIVISION_OV2775_2L_MIPI_2M_30FPS_12BIT_WDR2TO1:
            memcpy_s(pstChnAttr, sizeof(VI_CHN_ATTR_S), &CHN_ATTR_1920x1080_420_SDR8_LINEAR, sizeof(VI_CHN_ATTR_S));
            break;

        default:
            memcpy_s(pstChnAttr, sizeof(VI_CHN_ATTR_S), &CHN_ATTR_1920x1080_420_SDR8_LINEAR, sizeof(VI_CHN_ATTR_S));
    }
	mppex_GetChnAttrBySns(enSnsType, pstChnAttr);
    return HI_SUCCESS;
}

HI_S32 SAMPLE_COMM_VI_StartDev(SAMPLE_VI_INFO_S* pstViInfo)
{
    HI_S32              s32Ret;
    VI_DEV              ViDev;
    SAMPLE_SNS_TYPE_E    enSnsType;
    VI_DEV_ATTR_S       stViDevAttr;

    ViDev       = pstViInfo->stDevInfo.ViDev;
    enSnsType    = pstViInfo->stSnsInfo.enSnsType;

    SAMPLE_COMM_VI_GetDevAttrBySns(enSnsType, &stViDevAttr);
    stViDevAttr.stWDRAttr.enWDRMode = pstViInfo->stDevInfo.enWDRMode;
    if(VI_PARALLEL_VPSS_OFFLINE == pstViInfo->stPipeInfo.enMastPipeMode || VI_PARALLEL_VPSS_PARALLEL == pstViInfo->stPipeInfo.enMastPipeMode)
    {
        stViDevAttr.enDataRate = DATA_RATE_X2;
    }

    //maohw add;
    SAMPLE_PRT("HI_MPI_VI_DisableDev ViDev:%d\n", ViDev);
    s32Ret  = HI_MPI_VI_DisableDev(ViDev);
    if (s32Ret != HI_SUCCESS)
    {
        SAMPLE_PRT("HI_MPI_VI_DisableDev failed with %#x!\n", s32Ret);
        return HI_FAILURE;
    }
    
    s32Ret = HI_MPI_VI_SetDevAttr(ViDev, &stViDevAttr);

    if (s32Ret != HI_SUCCESS)
    {
        SAMPLE_PRT("HI_MPI_VI_SetDevAttr failed with %#x!\n", s32Ret);
        return HI_FAILURE;
    }

    s32Ret = HI_MPI_VI_EnableDev(ViDev);

    if (s32Ret != HI_SUCCESS)
    {
        SAMPLE_PRT("HI_MPI_VI_EnableDev failed with %#x!\n", s32Ret);
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

HI_S32 SAMPLE_COMM_VI_StopDev(SAMPLE_VI_INFO_S* pstViInfo)
{
    HI_S32 s32Ret;
    VI_DEV ViDev;

    ViDev   = pstViInfo->stDevInfo.ViDev;
    s32Ret  = HI_MPI_VI_DisableDev(ViDev);

    if (s32Ret != HI_SUCCESS)
    {
        SAMPLE_PRT("HI_MPI_VI_DisableDev failed with %#x!\n", s32Ret);
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

HI_S32 SAMPLE_COMM_VI_BindPipeDev(SAMPLE_VI_INFO_S* pstViInfo)
{
    HI_S32              i;
    HI_S32              s32PipeCnt = 0;
    HI_S32              s32Ret;
    VI_DEV_BIND_PIPE_S  stDevBindPipe = {0};

    for (i = 0; i < 4; i++)
    {
        if (pstViInfo->stPipeInfo.aPipe[i] >= 0  && pstViInfo->stPipeInfo.aPipe[i] < VI_MAX_PIPE_NUM)
        {
            stDevBindPipe.PipeId[s32PipeCnt] = pstViInfo->stPipeInfo.aPipe[i];
            s32PipeCnt++;
            stDevBindPipe.u32Num = s32PipeCnt;
        }
    }

    s32Ret = HI_MPI_VI_SetDevBindPipe(pstViInfo->stDevInfo.ViDev, &stDevBindPipe);

    if (s32Ret != HI_SUCCESS)
    {
        SAMPLE_PRT("HI_MPI_VI_SetDevBindPipe failed with %#x! ViDev:%d, s32PipeCnt:%d\n"
                  , s32Ret, pstViInfo->stDevInfo.ViDev, s32PipeCnt);
        return HI_FAILURE;
    }

    return s32Ret;
}

static HI_S32 SAMPLE_COMM_VI_ModeSwitchCreateSingleViPipe(VI_PIPE ViPipe, VI_PIPE_ATTR_S* pstPipeAttr)
{
    HI_S32 s32Ret;

    s32Ret = HI_MPI_VI_CreatePipe(ViPipe, pstPipeAttr);
    if (s32Ret != HI_SUCCESS)
    {
        SAMPLE_PRT("HI_MPI_VI_CreatePipe failed with %#x!\n", s32Ret);
        return HI_FAILURE;
    }

    return s32Ret;
}


static HI_S32 SAMPLE_COMM_VI_ModeSwitch_EnableSingleViPipe(VI_PIPE ViPipe, VI_PIPE_ATTR_S* pstPipeAttr)
{
    HI_S32 s32Ret;

    s32Ret = HI_MPI_VI_StartPipe(ViPipe);

    if (s32Ret != HI_SUCCESS)
    {
        HI_MPI_VI_DestroyPipe(ViPipe);
        SAMPLE_PRT("HI_MPI_VI_StartPipe failed with %#x!\n", s32Ret);
        return HI_FAILURE;
    }

    return s32Ret;
}


static HI_S32 SAMPLE_COMM_VI_StopSingleViPipe(VI_PIPE ViPipe)
{
    HI_S32  s32Ret;

    s32Ret = HI_MPI_VI_StopPipe(ViPipe);

    if (s32Ret != HI_SUCCESS)
    {
        SAMPLE_PRT("HI_MPI_VI_StopPipe failed with %#x!\n", s32Ret);
        return HI_FAILURE;
    }

    s32Ret = HI_MPI_VI_DestroyPipe(ViPipe);

    if (s32Ret != HI_SUCCESS)
    {
        SAMPLE_PRT("HI_MPI_VI_DestroyPipe failed with %#x!\n", s32Ret);
        return HI_FAILURE;
    }

    return s32Ret;
}


HI_S32 SAMPLE_COMM_VI_ModeSwitch_StartViPipe(SAMPLE_VI_INFO_S* pstViInfo)
{
    HI_S32          i, j;
    HI_S32          s32Ret = HI_SUCCESS;
    VI_PIPE         ViPipe;
    VI_PIPE_ATTR_S  stPipeAttr;

    for (i = 0; i < WDR_MAX_PIPE_NUM; i++)
    {
        if (pstViInfo->stPipeInfo.aPipe[i] >= 0  && pstViInfo->stPipeInfo.aPipe[i] < VI_MAX_PIPE_NUM)
        {
            ViPipe = pstViInfo->stPipeInfo.aPipe[i];
            SAMPLE_COMM_VI_GetPipeAttrBySns(pstViInfo->stSnsInfo.enSnsType, &stPipeAttr);
            {
                s32Ret = SAMPLE_COMM_VI_ModeSwitchCreateSingleViPipe(ViPipe, &stPipeAttr);
                if (s32Ret != HI_SUCCESS)
                {
                    SAMPLE_PRT("SAMPLE_COMM_VI_StartSingleViPipe  %d failed!\n", ViPipe);
                    goto EXIT;
                }
            }

        }
    }

    return s32Ret;

EXIT:

    for (j = 0; j < i; j++)
    {
        ViPipe = j;
        SAMPLE_COMM_VI_StopSingleViPipe(ViPipe);
    }

    return s32Ret;
}


HI_S32 SAMPLE_COMM_VI_ModeSwitch_EnableViPipe(SAMPLE_VI_INFO_S* pstViInfo)
{
    HI_S32          i, j;
    HI_S32          s32Ret = HI_SUCCESS;
    VI_PIPE         ViPipe;
    VI_PIPE_ATTR_S  stPipeAttr;

    for (i = 0; i < WDR_MAX_PIPE_NUM; i++)
    {
        if (pstViInfo->stPipeInfo.aPipe[i] >= 0  && pstViInfo->stPipeInfo.aPipe[i] < VI_MAX_PIPE_NUM)
        {
            ViPipe = pstViInfo->stPipeInfo.aPipe[i];
            SAMPLE_COMM_VI_GetPipeAttrBySns(pstViInfo->stSnsInfo.enSnsType, &stPipeAttr);
            {
                s32Ret = SAMPLE_COMM_VI_ModeSwitch_EnableSingleViPipe(ViPipe, &stPipeAttr);
                if (s32Ret != HI_SUCCESS)
                {
                    SAMPLE_PRT("SAMPLE_COMM_VI_StartSingleViPipe  %d failed!\n", ViPipe);
                    goto EXIT;
                }
            }

        }
    }

    return s32Ret;

EXIT:

    for (j = 0; j < i; j++)
    {
        ViPipe = j;
        SAMPLE_COMM_VI_StopSingleViPipe(ViPipe);
    }

    return s32Ret;
}

HI_S32 SAMPLE_COMM_VI_StartViPipe(SAMPLE_VI_INFO_S* pstViInfo)
{
    HI_S32          i, j;
    HI_S32          s32Ret = HI_SUCCESS;
    VI_PIPE         ViPipe;
    VI_PIPE_ATTR_S  stPipeAttr;

    for (i = 0; i < WDR_MAX_PIPE_NUM; i++)
    {
        if (pstViInfo->stPipeInfo.aPipe[i] >= 0  && pstViInfo->stPipeInfo.aPipe[i] < VI_MAX_PIPE_NUM)
        {
            ViPipe = pstViInfo->stPipeInfo.aPipe[i];
            SAMPLE_COMM_VI_GetPipeAttrBySns(pstViInfo->stSnsInfo.enSnsType, &stPipeAttr);
            if (HI_TRUE == pstViInfo->stPipeInfo.bIspBypass)
            {
                stPipeAttr.bIspBypass = HI_TRUE;
                stPipeAttr.enPixFmt   = pstViInfo->stPipeInfo.enPixFmt;
                stPipeAttr.enBitWidth = DATA_BITWIDTH_8;
            }

            if((2 == ViPipe) || (3 == ViPipe))
            {
                stPipeAttr.enCompressMode = COMPRESS_MODE_NONE;
            }
            if (pstViInfo->stSnsInfo.enSnsType == SONY_IMX415_MIPI_8M_20FPS_12BIT_WDR2TO1) {
                stPipeAttr.enCompressMode = COMPRESS_MODE_NONE;
            }
            if ((pstViInfo->stSnapInfo.bSnap) && (pstViInfo->stSnapInfo.bDoublePipe) && (ViPipe == pstViInfo->stSnapInfo.SnapPipe))
            {
                s32Ret = HI_MPI_VI_CreatePipe(ViPipe, &stPipeAttr);
                if (s32Ret != HI_SUCCESS)
                {
                    SAMPLE_PRT("HI_MPI_VI_CreatePipe ViPipe:%d, failed with %#x!\n", ViPipe, s32Ret);
                    goto EXIT;
                }
            }
            else
            {
                s32Ret = HI_MPI_VI_CreatePipe(ViPipe, &stPipeAttr);
                if (s32Ret != HI_SUCCESS)
                {
                    SAMPLE_PRT("HI_MPI_VI_CreatePipe ViPipe:%d, failed with %#x!\n", ViPipe, s32Ret);
                    return HI_FAILURE;
                }

                if (HI_TRUE == pstViInfo->stPipeInfo.bVcNumCfged)
                {
                    s32Ret = HI_MPI_VI_SetPipeVCNumber(ViPipe, pstViInfo->stPipeInfo.u32VCNum[i]);
                    if (s32Ret != HI_SUCCESS)
                    {
                        HI_MPI_VI_DestroyPipe(ViPipe);
                        SAMPLE_PRT("HI_MPI_VI_SetPipeVCNumber failed with %#x!\n", s32Ret);
                        return HI_FAILURE;
                    }
                }
                
                //SetPipeAttr;                
                mppex_SetPipeAttr(ViPipe, &stPipeAttr);

                s32Ret = HI_MPI_VI_StartPipe(ViPipe);
                if (s32Ret != HI_SUCCESS)
                {
                    HI_MPI_VI_DestroyPipe(ViPipe);
                    SAMPLE_PRT("HI_MPI_VI_StartPipe failed with %#x!\n", s32Ret);
                    return HI_FAILURE;
                }
            }

        }
    }

    return s32Ret;

EXIT:

    for (j = 0; j < i; j++)
    {
        ViPipe = j;
        SAMPLE_COMM_VI_StopSingleViPipe(ViPipe);
    }

    return s32Ret;
}

HI_S32 SAMPLE_COMM_VI_StopViPipe(SAMPLE_VI_INFO_S* pstViInfo)
{
    HI_S32  i;
    VI_PIPE ViPipe;

    for (i = 0; i < WDR_MAX_PIPE_NUM; i++)
    {
        if (pstViInfo->stPipeInfo.aPipe[i] >= 0  && pstViInfo->stPipeInfo.aPipe[i] < VI_MAX_PIPE_NUM)
        {
            ViPipe = pstViInfo->stPipeInfo.aPipe[i];
            SAMPLE_COMM_VI_StopSingleViPipe(ViPipe);
        }
    }

    return HI_SUCCESS;
}


HI_S32 SAMPLE_COMM_VI_ModeSwitch_StartViChn(SAMPLE_VI_INFO_S* pstViInfo)
{
    HI_S32              i;
    HI_BOOL             bNeedChn;
    HI_S32              s32Ret = HI_SUCCESS;
    VI_PIPE             ViPipe;
    VI_CHN              ViChn;
    VI_CHN_ATTR_S       stChnAttr;

    for (i = 0; i < WDR_MAX_PIPE_NUM; i++)
    {
        if (pstViInfo->stPipeInfo.aPipe[i] >= 0  && pstViInfo->stPipeInfo.aPipe[i] < VI_MAX_PIPE_NUM)
        {
            ViPipe = pstViInfo->stPipeInfo.aPipe[i];
            ViChn  = pstViInfo->stChnInfo.ViChn;

            SAMPLE_COMM_VI_GetChnAttrBySns(pstViInfo->stSnsInfo.enSnsType, &stChnAttr);
            stChnAttr.enDynamicRange = pstViInfo->stChnInfo.enDynamicRange;
            stChnAttr.enVideoFormat  = pstViInfo->stChnInfo.enVideoFormat;
            stChnAttr.enPixelFormat  = pstViInfo->stChnInfo.enPixFormat;
            stChnAttr.enCompressMode = pstViInfo->stChnInfo.enCompressMode;

            if (WDR_MODE_NONE == pstViInfo->stDevInfo.enWDRMode)
            {
                bNeedChn = HI_TRUE;
            }
            else
            {
                bNeedChn = (i > 0) ? HI_FALSE : HI_TRUE;
            }

            if (bNeedChn)
            {
                s32Ret = HI_MPI_VI_SetChnAttr(ViPipe, ViChn, &stChnAttr);
                if (s32Ret != HI_SUCCESS)
                {
                    SAMPLE_PRT("HI_MPI_VI_SetChnAttr failed with %#x!\n", s32Ret);
                    return HI_FAILURE;
                }
            }
        }
    }

    return s32Ret;
}
HI_S32 SAMPLE_COMM_VI_StartViChn(SAMPLE_VI_INFO_S* pstViInfo)
{
    HI_S32              i;
    HI_BOOL             bNeedChn;
    HI_S32              s32Ret = HI_SUCCESS;
    VI_PIPE             ViPipe;
    VI_CHN              ViChn;
    VI_CHN_ATTR_S       stChnAttr;
    VI_VPSS_MODE_E      enMastPipeMode;

    for (i = 0; i < WDR_MAX_PIPE_NUM; i++)
    {
        if (pstViInfo->stPipeInfo.aPipe[i] >= 0  && pstViInfo->stPipeInfo.aPipe[i] < VI_MAX_PIPE_NUM)
        {
            ViPipe = pstViInfo->stPipeInfo.aPipe[i];
            ViChn  = pstViInfo->stChnInfo.ViChn;

            SAMPLE_COMM_VI_GetChnAttrBySns(pstViInfo->stSnsInfo.enSnsType, &stChnAttr);
            stChnAttr.enDynamicRange = pstViInfo->stChnInfo.enDynamicRange;
            stChnAttr.enVideoFormat  = pstViInfo->stChnInfo.enVideoFormat;
            stChnAttr.enPixelFormat  = pstViInfo->stChnInfo.enPixFormat;
            stChnAttr.enCompressMode = pstViInfo->stChnInfo.enCompressMode;

            if (WDR_MODE_NONE == pstViInfo->stDevInfo.enWDRMode)
            {
                bNeedChn = HI_TRUE;
            }
            else
            {
                bNeedChn = (i > 0) ? HI_FALSE : HI_TRUE;
            }

            if (bNeedChn)
            {
                s32Ret = HI_MPI_VI_SetChnAttr(ViPipe, ViChn, &stChnAttr);

                if (s32Ret != HI_SUCCESS)
                {
                    SAMPLE_PRT("HI_MPI_VI_SetChnAttr failed with %#x!\n", s32Ret);
                    return HI_FAILURE;
                }

                enMastPipeMode = pstViInfo->stPipeInfo.enMastPipeMode;

                if (VI_OFFLINE_VPSS_OFFLINE == enMastPipeMode
                    || VI_ONLINE_VPSS_OFFLINE == enMastPipeMode
                    || VI_PARALLEL_VPSS_OFFLINE == enMastPipeMode)
                {
                    s32Ret = HI_MPI_VI_EnableChn(ViPipe, ViChn);

                    if (s32Ret != HI_SUCCESS)
                    {
                        SAMPLE_PRT("HI_MPI_VI_EnableChn failed with %#x!\n", s32Ret);
                        return HI_FAILURE;
                    }
                }
            }
        }
    }

    return s32Ret;
}

HI_S32 SAMPLE_COMM_VI_StopViChn(SAMPLE_VI_INFO_S* pstViInfo)
{
    HI_S32              i;
    HI_BOOL             bNeedChn;
    HI_S32              s32Ret = HI_SUCCESS;
    VI_PIPE             ViPipe;
    VI_CHN              ViChn;
    VI_VPSS_MODE_E      enMastPipeMode;


    for (i = 0; i < 4; i++)
    {
        if (pstViInfo->stPipeInfo.aPipe[i] >= 0  && pstViInfo->stPipeInfo.aPipe[i] < VI_MAX_PIPE_NUM)
        {
            ViPipe = pstViInfo->stPipeInfo.aPipe[i];
            ViChn  = pstViInfo->stChnInfo.ViChn;

            if (WDR_MODE_NONE == pstViInfo->stDevInfo.enWDRMode)
            {
                bNeedChn = HI_TRUE;
            }
            else
            {
                bNeedChn = (i > 0) ? HI_FALSE : HI_TRUE;
            }

            if (bNeedChn)
            {
                enMastPipeMode = pstViInfo->stPipeInfo.enMastPipeMode;

                if (VI_OFFLINE_VPSS_OFFLINE == enMastPipeMode
                    || VI_ONLINE_VPSS_OFFLINE == enMastPipeMode
                    || VI_PARALLEL_VPSS_OFFLINE == enMastPipeMode)
                {
                    s32Ret = HI_MPI_VI_DisableChn(ViPipe, ViChn);

                    if (s32Ret != HI_SUCCESS)
                    {
                        SAMPLE_PRT("HI_MPI_VI_DisableChn failed with %#x!\n", s32Ret);
                        return HI_FAILURE;
                    }
                }
            }
        }
    }

    return s32Ret;
}

HI_S32 SAMPLE_COMM_VI_CreateSingleVi(SAMPLE_VI_INFO_S* pstViInfo)
{
    HI_S32 s32Ret = HI_SUCCESS;

    s32Ret = SAMPLE_COMM_VI_StartDev(pstViInfo);

    if (s32Ret != HI_SUCCESS)
    {
        SAMPLE_PRT("SAMPLE_COMM_VI_StartDev failed !\n");
        return HI_FAILURE;
    }

    //we should bind pipe,then creat pipe
    s32Ret = SAMPLE_COMM_VI_BindPipeDev(pstViInfo);

    if (s32Ret != HI_SUCCESS)
    {
        SAMPLE_PRT("SAMPLE_COMM_VI_BindPipeDev failed !\n");
        goto EXIT1;
    }

    s32Ret = SAMPLE_COMM_VI_StartViPipe(pstViInfo);

    if (s32Ret != HI_SUCCESS)
    {
        SAMPLE_PRT("SAMPLE_COMM_VI_StartViPipe failed !\n");
        goto EXIT1;
    }

    s32Ret = SAMPLE_COMM_VI_StartViChn(pstViInfo);

    if (s32Ret != HI_SUCCESS)
    {
        SAMPLE_PRT("SAMPLE_COMM_VI_StartViChn failed !\n");
        goto EXIT2;
    }

    return HI_SUCCESS;

EXIT2:
    SAMPLE_COMM_VI_StopViPipe(pstViInfo);

EXIT1:
    SAMPLE_COMM_VI_StopDev(pstViInfo);


    return s32Ret;
}

HI_S32 SAMPLE_COMM_ModeSwitch_VI_CreateSingleVi(SAMPLE_VI_INFO_S* pstViInfo)
{
    HI_S32 s32Ret = HI_SUCCESS;

    s32Ret = SAMPLE_COMM_VI_StartDev(pstViInfo);
    if (s32Ret != HI_SUCCESS)
    {
        SAMPLE_PRT("SAMPLE_COMM_VI_StartDev failed !\n");
        return HI_FAILURE;
    }

    //we should bind pipe,then creat pipe
    s32Ret = SAMPLE_COMM_VI_BindPipeDev(pstViInfo);
    if (s32Ret != HI_SUCCESS)
    {
        SAMPLE_PRT("SAMPLE_COMM_VI_BindPipeDev failed !\n");
        goto EXIT1;
    }


    s32Ret = SAMPLE_COMM_VI_ModeSwitch_StartViPipe(pstViInfo);
    if (s32Ret != HI_SUCCESS)
    {
        SAMPLE_PRT("SAMPLE_COMM_VI_StartViPipe failed !\n");
        goto EXIT1;
    }

    s32Ret = SAMPLE_COMM_VI_ModeSwitch_StartViChn(pstViInfo);
    if (s32Ret != HI_SUCCESS)
    {
        SAMPLE_PRT("SAMPLE_COMM_VI_StartViChn failed !\n");
        goto EXIT2;
    }


    return HI_SUCCESS;

EXIT2:
    SAMPLE_COMM_VI_StopViPipe(pstViInfo);

EXIT1:
    SAMPLE_COMM_VI_StopDev(pstViInfo);


    return s32Ret;
}


static HI_S32 SAMPLE_COMM_VI_StartPipe_Chn(SAMPLE_VI_INFO_S* pstViInfo)
{
    HI_S32 s32Ret = HI_SUCCESS;


    s32Ret = SAMPLE_COMM_VI_ModeSwitch_EnableViPipe(pstViInfo);

    if (s32Ret != HI_SUCCESS)
    {
        SAMPLE_PRT("SAMPLE_COMM_VI_StartViPipe failed !\n");
        goto EXIT1;
    }

    s32Ret = SAMPLE_COMM_VI_StartViChn(pstViInfo);

    if (s32Ret != HI_SUCCESS)
    {
        SAMPLE_PRT("SAMPLE_COMM_VI_StartViChn failed !\n");
        goto EXIT2;
    }

    return HI_SUCCESS;

EXIT2:
    SAMPLE_COMM_VI_StopViPipe(pstViInfo);

EXIT1:
    SAMPLE_COMM_VI_StopDev(pstViInfo);


    return s32Ret;
}

HI_S32 SAMPLE_COMM_VI_DestroySingleVi(SAMPLE_VI_INFO_S* pstViInfo)
{
    SAMPLE_COMM_VI_StopViChn(pstViInfo);

    SAMPLE_COMM_VI_StopViPipe(pstViInfo);

    SAMPLE_COMM_VI_StopDev(pstViInfo);

    return HI_SUCCESS;
}

static HI_S32 SAMPLE_COMM_VI_DestroySinglePipe_Chn(SAMPLE_VI_INFO_S* pstViInfo)
{
   SAMPLE_COMM_VI_StopViChn(pstViInfo);

   SAMPLE_COMM_VI_StopViPipe(pstViInfo);

    return HI_SUCCESS;
}

HI_S32 SAMPLE_COMM_VI_CreateVi(SAMPLE_VI_CONFIG_S* pstViConfig)
{
    HI_S32              i, j;
    HI_S32              s32ViNum;
    HI_S32              s32Ret = HI_SUCCESS;
    SAMPLE_VI_INFO_S*   pstViInfo = HI_NULL;

    if (!pstViConfig)
    {
        SAMPLE_PRT("%s: null ptr\n", __FUNCTION__);
        return HI_FAILURE;
    }

    for (i = 0; i < pstViConfig->s32WorkingViNum; i++)
    {
        s32ViNum  = pstViConfig->as32WorkingViId[i];
        pstViInfo = &pstViConfig->astViInfo[s32ViNum];
        
        SAMPLE_PRT("s32ViNum:%d, ViDev:%d, enSnsType:%d\n"
              , s32ViNum, pstViInfo->stDevInfo.ViDev, pstViInfo->stSnsInfo.enSnsType);
              
        s32Ret = SAMPLE_COMM_VI_CreateSingleVi(pstViInfo);

        if (s32Ret != HI_SUCCESS)
        {
            SAMPLE_PRT("SAMPLE_COMM_VI_CreateSingleVi failed !\n");
            goto EXIT;
        }
    }

    return HI_SUCCESS;
EXIT:

    for (j = 0; j < i; j++)
    {
        s32ViNum  = pstViConfig->as32WorkingViId[j];
        pstViInfo = &pstViConfig->astViInfo[s32ViNum];

        SAMPLE_COMM_VI_DestroySingleVi(pstViInfo);
    }

    return s32Ret;
}


HI_S32 SAMPLE_COMM_ModeSwitch_VI_CreateVi(SAMPLE_VI_CONFIG_S* pstViConfig)
{
    HI_S32              i, j;
    HI_S32              s32ViNum;
    HI_S32              s32Ret = HI_SUCCESS;
    SAMPLE_VI_INFO_S*   pstViInfo = HI_NULL;

    if (!pstViConfig)
    {
        SAMPLE_PRT("%s: null ptr\n", __FUNCTION__);
        return HI_FAILURE;
    }

    for (i = 0; i < pstViConfig->s32WorkingViNum; i++)
    {
        s32ViNum  = pstViConfig->as32WorkingViId[i];
        pstViInfo = &pstViConfig->astViInfo[s32ViNum];

        s32Ret = SAMPLE_COMM_ModeSwitch_VI_CreateSingleVi(pstViInfo);

        if (s32Ret != HI_SUCCESS)
        {
            SAMPLE_PRT("SAMPLE_COMM_VI_CreateSingleVi failed !\n");
            goto EXIT;
        }
    }

    return HI_SUCCESS;
EXIT:

    for (j = 0; j < i; j++)
    {
        s32ViNum  = pstViConfig->as32WorkingViId[j];
        pstViInfo = &pstViConfig->astViInfo[s32ViNum];

        SAMPLE_COMM_VI_DestroySingleVi(pstViInfo);
    }

    return s32Ret;
}

HI_S32 SAMPLE_COMM_ModeSwitch_VI_StartPipe_Chn(SAMPLE_VI_CONFIG_S* pstViConfig)
{
    HI_S32              i, j;
    HI_S32              s32ViNum;
    HI_S32              s32Ret = HI_SUCCESS;
    SAMPLE_VI_INFO_S*   pstViInfo = HI_NULL;

    if (!pstViConfig)
    {
        SAMPLE_PRT("%s: null ptr\n", __FUNCTION__);
        return HI_FAILURE;
    }

    for (i = 0; i < pstViConfig->s32WorkingViNum; i++)
    {
        s32ViNum  = pstViConfig->as32WorkingViId[i];
        pstViInfo = &pstViConfig->astViInfo[s32ViNum];

        s32Ret = SAMPLE_COMM_VI_StartPipe_Chn(pstViInfo);

        if (s32Ret != HI_SUCCESS)
        {
            SAMPLE_PRT("SAMPLE_COMM_VI_CreateSingleVi failed !\n");
            goto EXIT;
        }
    }

    return HI_SUCCESS;
EXIT:

    for (j = 0; j < i; j++)
    {
        s32ViNum  = pstViConfig->as32WorkingViId[j];
        pstViInfo = &pstViConfig->astViInfo[s32ViNum];

        SAMPLE_COMM_VI_DestroySinglePipe_Chn(pstViInfo);
    }

    return s32Ret;
}

HI_S32 SAMPLE_COMM_VI_DestroyVi(SAMPLE_VI_CONFIG_S* pstViConfig)
{
    HI_S32            i;
    HI_S32            s32ViNum;
    SAMPLE_VI_INFO_S* pstViInfo = HI_NULL;

    if (!pstViConfig)
    {
        SAMPLE_PRT("%s: null ptr\n", __FUNCTION__);
        return HI_FAILURE;
    }

    for (i = 0; i < pstViConfig->s32WorkingViNum; i++)
    {
        s32ViNum  = pstViConfig->as32WorkingViId[i];
        pstViInfo = &pstViConfig->astViInfo[s32ViNum];

        SAMPLE_COMM_VI_DestroySingleVi(pstViInfo);
    }

    return HI_SUCCESS;
}

HI_S32 SAMPLE_COMM_VI_StartIsp(SAMPLE_VI_INFO_S* pstViInfo)
{
    HI_S32              i;
    HI_BOOL             bNeedPipe;
    HI_S32              s32Ret = HI_SUCCESS;
    VI_PIPE             ViPipe;
    HI_U32              u32SnsId;
    ISP_PUB_ATTR_S      stPubAttr;
    VI_PIPE_ATTR_S      stPipeAttr;

    SAMPLE_COMM_VI_GetPipeAttrBySns(pstViInfo->stSnsInfo.enSnsType, &stPipeAttr);
    if (VI_PIPE_BYPASS_BE == stPipeAttr.enPipeBypassMode)
    {
        return HI_SUCCESS;
    }

    for (i = 0; i < WDR_MAX_PIPE_NUM; i++)
    {
        if (pstViInfo->stPipeInfo.aPipe[i] >= 0  && pstViInfo->stPipeInfo.aPipe[i] < VI_MAX_PIPE_NUM)
        {
            ViPipe      = pstViInfo->stPipeInfo.aPipe[i];
            u32SnsId    = pstViInfo->stSnsInfo.s32SnsId;

            SAMPLE_COMM_ISP_GetIspAttrBySns(pstViInfo->stSnsInfo.enSnsType, &stPubAttr);
            if (pstViInfo->stSnsInfo.enSnsType != OMNIVISION_OV2775_MIPI_2M_30FPS_12BIT_BUILTIN &&
                pstViInfo->stSnsInfo.enSnsType != PRIMESENSOR_PS5260_2L_MIPI_2M_30FPS_12BIT_BUILTIN)
            {
                stPubAttr.enWDRMode = pstViInfo->stDevInfo.enWDRMode;
            }

            if (WDR_MODE_NONE == pstViInfo->stDevInfo.enWDRMode)
            {
                bNeedPipe = HI_TRUE;
            }
            else
            {
                bNeedPipe = (i > 0) ? HI_FALSE : HI_TRUE;
            }

            if (HI_TRUE != bNeedPipe)
            {
                continue;
            }

            s32Ret = SAMPLE_COMM_ISP_Sensor_Regiter_callback(ViPipe, u32SnsId);

            if (HI_SUCCESS != s32Ret)
            {
                SAMPLE_PRT("register sensor %d to ISP %d failed\n", u32SnsId, ViPipe);
                SAMPLE_COMM_ISP_Stop(ViPipe);
                return HI_FAILURE;
            }

            if(((pstViInfo->stSnapInfo.bDoublePipe) && (pstViInfo->stSnapInfo.SnapPipe == ViPipe))
                || (pstViInfo->stPipeInfo.bMultiPipe && i > 0))
            {
                s32Ret = SAMPLE_COMM_ISP_BindSns(ViPipe, u32SnsId, pstViInfo->stSnsInfo.enSnsType, -1);

                if (HI_SUCCESS != s32Ret)
                {
                    SAMPLE_PRT("register sensor %d bus id %d failed\n", u32SnsId, pstViInfo->stSnsInfo.s32BusId);
                    SAMPLE_COMM_ISP_Stop(ViPipe);
                    return HI_FAILURE;
                }
            }
            else
            {
                s32Ret = SAMPLE_COMM_ISP_BindSns(ViPipe, u32SnsId, pstViInfo->stSnsInfo.enSnsType, pstViInfo->stSnsInfo.s32BusId);

                if (HI_SUCCESS != s32Ret)
                {
                    SAMPLE_PRT("register sensor %d bus id %d failed\n", u32SnsId, pstViInfo->stSnsInfo.s32BusId);
                    SAMPLE_COMM_ISP_Stop(ViPipe);
                    return HI_FAILURE;
                }
            }
            s32Ret = SAMPLE_COMM_ISP_Aelib_Callback(ViPipe);

            if (HI_SUCCESS != s32Ret)
            {
                SAMPLE_PRT("SAMPLE_COMM_ISP_Aelib_Callback failed\n");
                SAMPLE_COMM_ISP_Stop(ViPipe);
                return HI_FAILURE;
            }

            s32Ret = SAMPLE_COMM_ISP_Awblib_Callback(ViPipe);

            if (HI_SUCCESS != s32Ret)
            {
                SAMPLE_PRT("SAMPLE_COMM_ISP_Awblib_Callback failed\n");
                SAMPLE_COMM_ISP_Stop(ViPipe);
                return HI_FAILURE;
            }

            s32Ret = HI_MPI_ISP_MemInit(ViPipe);

            if (s32Ret != HI_SUCCESS)
            {
                SAMPLE_PRT("Init Ext memory failed with %#x!\n", s32Ret);
                SAMPLE_COMM_ISP_Stop(ViPipe);
                return HI_FAILURE;
            }
            //SAMPLE_PRT("Init Ext memory ViPipe:%d, s32Ret:%#x!\n", ViPipe, s32Ret);

            s32Ret = HI_MPI_ISP_SetPubAttr(ViPipe, &stPubAttr);

            if (s32Ret != HI_SUCCESS)
            {
                SAMPLE_PRT("SetPubAttr failed with %#x!\n", s32Ret);
                SAMPLE_COMM_ISP_Stop(ViPipe);
                return HI_FAILURE;
            }

            s32Ret = HI_MPI_ISP_Init(ViPipe);

            if (s32Ret != HI_SUCCESS)
            {
                SAMPLE_PRT("ISP Init failed with %#x!\n", s32Ret);
                SAMPLE_COMM_ISP_Stop(ViPipe);
                return HI_FAILURE;
            }

            s32Ret = SAMPLE_COMM_ISP_Run(ViPipe);

            if (s32Ret != HI_SUCCESS)
            {
                SAMPLE_PRT("ISP Run failed with %#x!\n", s32Ret);
                SAMPLE_COMM_ISP_Stop(ViPipe);
                return HI_FAILURE;
            }
        }
    }

    return s32Ret;
}

HI_S32 SAMPLE_COMM_VI_StopIsp(SAMPLE_VI_INFO_S* pstViInfo)
{
    HI_S32  i;
    HI_BOOL bNeedPipe;
    VI_PIPE ViPipe;

    for (i = 0; i < WDR_MAX_PIPE_NUM; i++)
    {
        if (pstViInfo->stPipeInfo.aPipe[i] >= 0  && pstViInfo->stPipeInfo.aPipe[i] < VI_MAX_PIPE_NUM)
        {
            ViPipe    = pstViInfo->stPipeInfo.aPipe[i];

            if (WDR_MODE_NONE == pstViInfo->stDevInfo.enWDRMode)
            {
                bNeedPipe = HI_TRUE;
            }
            else
            {
                bNeedPipe = (i > 0) ? HI_FALSE : HI_TRUE;
            }

            if (HI_TRUE != bNeedPipe)
            {
                continue;
            }

            SAMPLE_COMM_ISP_Stop(ViPipe);
        }
    }

    return HI_SUCCESS;
}

HI_S32 SAMPLE_COMM_VI_CreateIsp(SAMPLE_VI_CONFIG_S* pstViConfig)
{
    HI_S32              i;
    HI_S32              s32ViNum;
    HI_S32              s32Ret = HI_SUCCESS;

    SAMPLE_VI_INFO_S*   pstViInfo = HI_NULL;

    if (!pstViConfig)
    {
        SAMPLE_PRT("%s: null ptr\n", __FUNCTION__);
        return HI_FAILURE;
    }

    for (i = 0; i < pstViConfig->s32WorkingViNum; i++)
    {
        s32ViNum  = pstViConfig->as32WorkingViId[i];
        pstViInfo = &pstViConfig->astViInfo[s32ViNum];



        s32Ret = SAMPLE_COMM_VI_StartIsp(pstViInfo);

        if (s32Ret != HI_SUCCESS)
        {
            SAMPLE_PRT("SAMPLE_COMM_VI_StartIsp failed !\n");
            return HI_FAILURE;
        }



    }

    return HI_SUCCESS;
}

HI_S32 SAMPLE_COMM_VI_DestroyIsp(SAMPLE_VI_CONFIG_S* pstViConfig)
{
    HI_S32              i;
    HI_S32              s32ViNum;
    HI_S32              s32Ret = HI_SUCCESS;
    SAMPLE_VI_INFO_S*   pstViInfo = HI_NULL;

    if (!pstViConfig)
    {
        SAMPLE_PRT("%s: null ptr\n", __FUNCTION__);
        return HI_FAILURE;
    }

    for (i = 0; i < pstViConfig->s32WorkingViNum; i++)
    {
        s32ViNum  = pstViConfig->as32WorkingViId[i];
        pstViInfo = &pstViConfig->astViInfo[s32ViNum];



        s32Ret = SAMPLE_COMM_VI_StopIsp(pstViInfo);

        if (s32Ret != HI_SUCCESS)
        {
            SAMPLE_PRT("SAMPLE_COMM_VI_StopIsp failed !\n");
            return HI_FAILURE;
        }
    }

    return HI_SUCCESS;
}

HI_S32 SAMPLE_COMM_VI_StartVi(SAMPLE_VI_CONFIG_S* pstViConfig)
{
    HI_S32 s32Ret = HI_SUCCESS;

    if (!pstViConfig)
    {
        SAMPLE_PRT("%s: null ptr\n", __FUNCTION__);
        return HI_FAILURE;
    }

    s32Ret = SAMPLE_COMM_VI_StartMIPI(pstViConfig);
    if (s32Ret != HI_SUCCESS)
    {
        SAMPLE_PRT("SAMPLE_COMM_VI_StartMIPI failed!\n");
        return HI_FAILURE;
    }

    s32Ret = SAMPLE_COMM_VI_SetParam(pstViConfig);
    if (s32Ret != HI_SUCCESS)
    {
        SAMPLE_PRT("SAMPLE_COMM_VI_SetParam failed!\n");
        return HI_FAILURE;
    }

    s32Ret = SAMPLE_COMM_VI_CreateVi(pstViConfig);
    if (s32Ret != HI_SUCCESS)
    {
        SAMPLE_PRT("SAMPLE_COMM_VI_CreateVi failed!\n");
        return HI_FAILURE;
    }

    s32Ret = SAMPLE_COMM_VI_CreateIsp(pstViConfig);
    if (s32Ret != HI_SUCCESS)
    {
        SAMPLE_COMM_VI_DestroyVi(pstViConfig);
        SAMPLE_PRT("SAMPLE_COMM_VI_CreateIsp failed!\n");
        return HI_FAILURE;
    }

    return s32Ret;
}

HI_S32 SAMPLE_COMM_VI_StopVi(SAMPLE_VI_CONFIG_S* pstViConfig)
{
    HI_S32 s32Ret = HI_SUCCESS;

        s32Ret = SAMPLE_COMM_VI_DestroyIsp(pstViConfig);

        if (s32Ret != HI_SUCCESS)
        {
            SAMPLE_PRT("SAMPLE_COMM_VI_DestroyIsp failed !\n");
            return HI_FAILURE;
        }

#if 0
    else
    {
        if (HI_FALSE == pstViConfig->bSwitch)
        {
            s32Ret = SAMPLE_COMM_VI_DestroyIsp(pstViConfig);

            if (s32Ret != HI_SUCCESS)
            {
                SAMPLE_PRT("SAMPLE_COMM_VI_DestroyIsp failed !\n");
                return HI_FAILURE;
            }
        }
    }
#endif

    s32Ret = SAMPLE_COMM_VI_DestroyVi(pstViConfig);

    if (s32Ret != HI_SUCCESS)
    {
        SAMPLE_PRT("SAMPLE_COMM_VI_DestroyVi failed !\n");
        return HI_FAILURE;
    }

    s32Ret = SAMPLE_COMM_VI_StopMIPI(pstViConfig);

    if (s32Ret != HI_SUCCESS)
    {
        SAMPLE_PRT("SAMPLE_COMM_VI_StopMIPI failed !\n");
        return HI_FAILURE;
    }

    return s32Ret;
}

HI_S32 SAMPLE_COMM_VI_SwitchISPMode(SAMPLE_VI_CONFIG_S* pstViConfig)
{
    HI_S32              i,j;
    HI_S32              s32ViNum;
    HI_S32              s32Ret = HI_SUCCESS;
    HI_BOOL             bNeedPipe;
    HI_BOOL             bSwitchWDR[VI_MAX_PIPE_NUM] = { HI_FALSE };
    VI_PIPE             ViPipe = 0;
    ISP_PUB_ATTR_S      stPubAttr;
    ISP_PUB_ATTR_S      stPrePubAttr;
    SAMPLE_VI_INFO_S*   pstViInfo = HI_NULL;
    ISP_INNER_STATE_INFO_S stInnerStateInfo;
    HI_BOOL bSwitchFinish;

    if (!pstViConfig)
    {
        SAMPLE_PRT("%s: null ptr\n", __FUNCTION__);
        return HI_FAILURE;
    }

    for (i = 0; i < pstViConfig->s32WorkingViNum; i++)
    {
        s32ViNum  = pstViConfig->as32WorkingViId[i];
        pstViInfo = &pstViConfig->astViInfo[s32ViNum];

        for (j = 0; j < WDR_MAX_PIPE_NUM; j++)
        {
            if ( pstViInfo->stPipeInfo.aPipe[j] >= 0 && pstViInfo->stPipeInfo.aPipe[j] < VI_MAX_PIPE_NUM)
            {

                SAMPLE_COMM_ISP_GetIspAttrBySns(pstViInfo->stSnsInfo.enSnsType, &stPubAttr);

                stPubAttr.enWDRMode =  pstViInfo->stDevInfo.enWDRMode;

                SAMPLE_PRT("SAMPLE_COMM_VI_CreateIsp enWDRMode is %d!\n", stPubAttr.enWDRMode);

                if (WDR_MODE_NONE == pstViInfo->stDevInfo.enWDRMode )
                {
                    bNeedPipe = HI_TRUE;
                }
                else
                {
                    bNeedPipe = (j > 0) ? HI_FALSE : HI_TRUE;
                }

                if (HI_TRUE != bNeedPipe)
                {
                    continue;
                }

                ViPipe = pstViInfo->stPipeInfo.aPipe[j];
                s32Ret = HI_MPI_ISP_GetPubAttr(ViPipe, &stPrePubAttr);
                if (s32Ret != HI_SUCCESS)
                {
                    SAMPLE_PRT("GetPubAttr failed with %#x!\n", s32Ret);
                    SAMPLE_COMM_ISP_Stop(ViPipe);
                    return HI_FAILURE;
                }

                s32Ret = HI_MPI_ISP_SetPubAttr(ViPipe, &stPubAttr);
                if (s32Ret != HI_SUCCESS)
                {
                    SAMPLE_PRT("SetPubAttr failed with %#x!\n", s32Ret);
                    SAMPLE_COMM_ISP_Stop(ViPipe);
                    return HI_FAILURE;
                }

                if (stPrePubAttr.enWDRMode != stPubAttr.enWDRMode)
                {
                    bSwitchWDR[ViPipe] = HI_TRUE;
                }
            }
        }
    }

    while (1)
    {
        bSwitchFinish = HI_TRUE;
        for (i = 0; i < pstViConfig->s32WorkingViNum; i++)
        {
            s32ViNum  = pstViConfig->as32WorkingViId[i];
            pstViInfo = &pstViConfig->astViInfo[s32ViNum];
            ViPipe    = pstViInfo->stPipeInfo.aPipe[0];

            HI_MPI_ISP_QueryInnerStateInfo(ViPipe, &stInnerStateInfo);
            if (bSwitchWDR[ViPipe] == HI_TRUE)
            {
                bSwitchFinish &= stInnerStateInfo.bWDRSwitchFinish;
            }
            else
            {
                bSwitchFinish &= stInnerStateInfo.bResSwitchFinish;
            }
        }

        if (bSwitchFinish == HI_TRUE) {
            SAMPLE_PRT("Switch finish!\n");
            break;
        }
        usleep(1000);
    }

    for (i = 0; i < pstViConfig->s32WorkingViNum; i++)
    {
        s32ViNum  = pstViConfig->as32WorkingViId[i];
        pstViInfo = &pstViConfig->astViInfo[s32ViNum];

        SAMPLE_COMM_VI_StartPipe_Chn(pstViInfo);
    }

    return HI_SUCCESS;
}


HI_S32  SAMPLE_COMM_VI_SwitchMode_StopVI(SAMPLE_VI_CONFIG_S* pstViConfigSrc)
{

    HI_S32              s32Ret = HI_SUCCESS;

    s32Ret = SAMPLE_COMM_VI_DestroyVi(pstViConfigSrc);

    if (s32Ret != HI_SUCCESS)
    {
        SAMPLE_PRT("SAMPLE_COMM_VI_DestroyVi failed !\n");
        return HI_FAILURE;
    }

    s32Ret = SAMPLE_COMM_VI_StopMIPI(pstViConfigSrc);

    if (s32Ret != HI_SUCCESS)
    {
        SAMPLE_PRT("SAMPLE_COMM_VI_StopMIPI failed !\n");
        return HI_FAILURE;
    }

    return HI_SUCCESS;

}


HI_S32  SAMPLE_COMM_VI_SwitchMode(SAMPLE_VI_CONFIG_S* pstViConfigDes)
{

    HI_S32 s32Ret = HI_SUCCESS;

    s32Ret = SAMPLE_COMM_VI_StartMIPI(pstViConfigDes);
    if (s32Ret != HI_SUCCESS)
    {
        SAMPLE_PRT("SAMPLE_COMM_VI_StartMIPI failed!\n");
        return HI_FAILURE;
    }

    /*   create vi without enable chn and enable pipe. */
    s32Ret =  SAMPLE_COMM_ModeSwitch_VI_CreateVi(pstViConfigDes);
    if (s32Ret != HI_SUCCESS)
    {
        SAMPLE_PRT("SAMPLE_COMM_VI_CreateVi failed!\n");
        return HI_FAILURE;
    }

    s32Ret = SAMPLE_COMM_VI_SwitchISPMode(pstViConfigDes);
    if (s32Ret != HI_SUCCESS)
    {
        SAMPLE_PRT("SAMPLE_COMM_ModeSwitch_VI_CreateIsp!\n");
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}


int SAMPLE_COMM_VI_ExitMpp( int s32poolId)
{
    if (s32poolId < 0)
    {
        if (HI_MPI_SYS_Exit())
        {
            SAMPLE_PRT("sys exit fail\n");
            return -1;
        }

        if (HI_MPI_VB_Exit())
        {
            SAMPLE_PRT("vb exit fail\n");
            return -1;
        }

        return -1;
    }

    return 0;
}

/******************************************************************************
* funciton : Get enWDRMode by diffrent sensor
******************************************************************************/
HI_S32 SAMPLE_COMM_VI_GetWDRModeBySensor(SAMPLE_SNS_TYPE_E enMode, WDR_MODE_E* penWDRMode)
{
    HI_S32 s32Ret = HI_SUCCESS;

    if (!penWDRMode)
    {
        return HI_FAILURE;
    }

    switch (enMode)
    {
        case SONY_IMX327_MIPI_2M_30FPS_12BIT_WDR2TO1:
        case SONY_IMX307_MIPI_2M_30FPS_12BIT_WDR2TO1:
        case SONY_IMX335_MIPI_5M_30FPS_10BIT_WDR2TO1:
        case SONY_IMX335_MIPI_4M_30FPS_10BIT_WDR2TO1:
        case OMNIVISION_OS05A_MIPI_4M_30FPS_10BIT_WDR2TO1:
        case SMART_SC4210_MIPI_3M_30FPS_10BIT_WDR2TO1:
            *penWDRMode = WDR_MODE_2To1_LINE;
            break;

        default:
            *penWDRMode = WDR_MODE_2To1_LINE;
            break;
    }

    return s32Ret;
}

/******************************************************************************
* funciton : Get Pipe by diffrent sensor
******************************************************************************/
HI_S32 SAMPLE_COMM_VI_GetPipeBySensor(SAMPLE_SNS_TYPE_E enMode, SAMPLE_PIPE_INFO_S* pstPipeInfo)
{
    HI_S32 s32Ret = HI_SUCCESS;

    if (!pstPipeInfo)
    {
        return HI_FAILURE;
    }

    switch (enMode)
    {
        default:
            pstPipeInfo->enMastPipeMode = VI_ONLINE_VPSS_OFFLINE;
            pstPipeInfo->aPipe[0] = 0;
            pstPipeInfo->aPipe[1] = 1;
            pstPipeInfo->aPipe[2] = -1;
            pstPipeInfo->aPipe[3] = -1;
            break;
    }

    return s32Ret;
}

/******************************************************************************
* funciton : Get enSize by diffrent sensor
******************************************************************************/
HI_S32 SAMPLE_COMM_VI_GetSizeBySensor(SAMPLE_SNS_TYPE_E enMode, PIC_SIZE_E* penSize)
{
    HI_S32 s32Ret = HI_SUCCESS;

    if (!penSize)
    {
        return HI_FAILURE;
    }

    switch (enMode)
    {
        case SONY_IMX327_MIPI_2M_30FPS_12BIT:
        case SONY_IMX327_MIPI_2M_30FPS_12BIT_WDR2TO1:
        case SONY_IMX385_MIPI_2M_30FPS_12BIT:
        case SONY_IMX482_MIPI_2M_30FPS_12BIT:
        case SONY_IMX585_MIPI_2M_30FPS_12BIT:  
            *penSize = PIC_1080P;
            break;

        case SONY_IMX390_MIPI_2M_30FPS_12BIT:
            *penSize = PIC_1080P;
            break;

        case SONY_IMX307_MIPI_2M_30FPS_12BIT:
        case SONY_IMX307_MIPI_2M_30FPS_12BIT_WDR2TO1:
            *penSize = PIC_1080P;
            break;

        case PANASONIC_MN34220_LVDS_2M_30FPS_12BIT:
            *penSize = PIC_1080P;
            break;
            
        case SONY_IMX335_2L_MIPI_5M_30FPS_10BIT:
        case SONY_IMX335_MIPI_5M_30FPS_12BIT:
        case SONY_IMX335_MIPI_5M_30FPS_10BIT_WDR2TO1:
            *penSize = PIC_2592x1944;
            break;

        case SONY_IMX335_MIPI_4M_30FPS_12BIT:
        case SONY_IMX335_MIPI_4M_30FPS_10BIT_WDR2TO1:
            *penSize = PIC_2592x1536;
            break;

        case SMART_SC4210_MIPI_3M_30FPS_12BIT:
        case SMART_SC4210_MIPI_3M_30FPS_10BIT_WDR2TO1:
            *penSize = PIC_2560x1440;
            break;

        case OMNIVISION_OS08A10_MIPI_8M_30FPS_10BIT:
        case SONY_IMX458_MIPI_8M_30FPS_10BIT:
        case OMNIVISION_OV12870_MIPI_8M_30FPS_10BIT:
        case SONY_IMX415_MIPI_8M_30FPS_12BIT:
        case SONY_IMX415_MIPI_8M_20FPS_12BIT:
        case SONY_IMX334_MIPI_8M_30FPS_12BIT:
        case SONY_IMX585_MIPI_8M_30FPS_12BIT:  
        case SONY_IMX415_MIPI_8M_20FPS_12BIT_WDR2TO1:
            *penSize = PIC_3840x2160;
            break;
        case SONY_IMX415_MIPI_2M_60FPS_12BIT:
        case OMNIVISION_OV2775_MIPI_2M_30FPS_12BIT:
        case OMNIVISION_OV2775_MIPI_2M_30FPS_12BIT_WDR2TO1:
        case OMNIVISION_OV2775_MIPI_2M_30FPS_12BIT_WDR2TO1_HLCG:
        case OMNIVISION_OV2775_MIPI_2M_30FPS_12BIT_BUILTIN:
        case OMNIVISION_OV2775_2L_MIPI_2M_30FPS_12BIT:
        case OMNIVISION_OV2775_2L_MIPI_2M_30FPS_12BIT_WDR2TO1:
            *penSize = PIC_1080P;
            break;
        case SONY_IMX458_MIPI_12M_20FPS_10BIT:
        case OMNIVISION_OV12870_MIPI_12M_30FPS_10BIT:
            *penSize = PIC_4000x3000;
            break;
        case SONY_IMX458_MIPI_4M_60FPS_10BIT:
        case SONY_IMX458_MIPI_4M_40FPS_10BIT:
            *penSize = PIC_2716x1524;
            break;
        case SONY_IMX458_MIPI_2M_90FPS_10BIT:
        case OMNIVISION_OV12870_MIPI_2M_120FPS_10BIT:
            *penSize = PIC_1080P;
            break;
        case OMNIVISION_OS05A_2L_MIPI_1M_30FPS_10BIT:
        case SONY_IMX458_MIPI_1M_129FPS_10BIT:
        case OMNIVISION_OV12870_MIPI_1M_240FPS_10BIT:
            *penSize = PIC_720P;
            break;

        case OMNIVISION_OV9284_MIPI_1M_60FPS_10BIT:
            *penSize = PIC_1280x800;
            break;

        case OMNIVISION_OS05A_2L_MIPI_4M_30FPS_12BIT:
        case OMNIVISION_OS05A_2L_MIPI_4M_30FPS_10BIT_WDR2TO1:
        case OMNIVISION_OS04B10_MIPI_4M_30FPS_10BIT:
            *penSize = PIC_2560x1440;
            break;

        case OMNIVISION_OS05A_MIPI_4M_30FPS_12BIT:
        case OMNIVISION_OS05A_MIPI_4M_30FPS_10BIT_WDR2TO1:
            *penSize = PIC_2688x1536;
            break;

        default:
            *penSize = PIC_1080P;
            break;
    }
	mppex_GetSizeBySensor(enMode, penSize);
    return s32Ret;
}



/******************************************************************************
* funciton : Get enSize by diffrent sensor
******************************************************************************/
HI_S32 SAMPLE_COMM_VI_GetFrameRateBySensor(SAMPLE_SNS_TYPE_E enMode, HI_U32* pu32FrameRate)
{
    HI_S32 s32Ret = HI_SUCCESS;

    if (!pu32FrameRate)
    {
        return HI_FAILURE;
    }

    switch (enMode)
    {
        case SONY_IMX327_MIPI_2M_30FPS_12BIT:
        case SONY_IMX327_MIPI_2M_30FPS_12BIT_WDR2TO1:
        case SONY_IMX385_MIPI_2M_30FPS_12BIT:
        case SONY_IMX482_MIPI_2M_30FPS_12BIT:  
        case SONY_IMX390_MIPI_2M_30FPS_12BIT:
        case SONY_IMX307_MIPI_2M_30FPS_12BIT:
        case SONY_IMX307_MIPI_2M_30FPS_12BIT_WDR2TO1:
        case SONY_IMX335_2L_MIPI_5M_30FPS_10BIT:  
        case SONY_IMX335_MIPI_5M_30FPS_12BIT:
        case SONY_IMX335_MIPI_5M_30FPS_10BIT_WDR2TO1:
        case SONY_IMX335_MIPI_4M_30FPS_12BIT:
        case SONY_IMX335_MIPI_4M_30FPS_10BIT_WDR2TO1:
        case SMART_SC4210_MIPI_3M_30FPS_12BIT:
        case SMART_SC4210_MIPI_3M_30FPS_10BIT_WDR2TO1:
        case PANASONIC_MN34220_LVDS_2M_30FPS_12BIT:
        case OMNIVISION_OS04B10_MIPI_4M_30FPS_10BIT:
        case OMNIVISION_OS05A_MIPI_4M_30FPS_12BIT:
        case OMNIVISION_OS05A_MIPI_4M_30FPS_10BIT_WDR2TO1:
        case OMNIVISION_OS05A_2L_MIPI_1M_30FPS_10BIT:
        case OMNIVISION_OS05A_2L_MIPI_4M_30FPS_12BIT:
        case OMNIVISION_OS05A_2L_MIPI_4M_30FPS_10BIT_WDR2TO1:
        case OMNIVISION_OS08A10_MIPI_8M_30FPS_10BIT:
        case OMNIVISION_OV12870_MIPI_12M_30FPS_10BIT:
        case OMNIVISION_OV12870_MIPI_8M_30FPS_10BIT:
            *pu32FrameRate = 30;
            break;

        case SONY_IMX458_MIPI_8M_30FPS_10BIT:
        case SONY_IMX415_MIPI_8M_30FPS_12BIT:
        case SONY_IMX334_MIPI_8M_30FPS_12BIT:
        case SONY_IMX585_MIPI_8M_30FPS_12BIT:
        case SONY_IMX585_MIPI_2M_30FPS_12BIT:
        case OMNIVISION_OV2775_MIPI_2M_30FPS_12BIT:
        case OMNIVISION_OV2775_MIPI_2M_30FPS_12BIT_WDR2TO1:
        case OMNIVISION_OV2775_MIPI_2M_30FPS_12BIT_WDR2TO1_HLCG:
        case OMNIVISION_OV2775_MIPI_2M_30FPS_12BIT_BUILTIN:
        case OMNIVISION_OV2775_2L_MIPI_2M_30FPS_12BIT:
        case OMNIVISION_OV2775_2L_MIPI_2M_30FPS_12BIT_WDR2TO1:
            *pu32FrameRate = 30;
            break;
        case SONY_IMX458_MIPI_12M_20FPS_10BIT:
        case SONY_IMX415_MIPI_8M_20FPS_12BIT:
            *pu32FrameRate = 20;
            break;
        case SONY_IMX415_MIPI_2M_60FPS_12BIT:
            *pu32FrameRate = 60;
            break;
        case SONY_IMX458_MIPI_4M_60FPS_10BIT:
            *pu32FrameRate = 60;
            break;
        case SONY_IMX458_MIPI_4M_40FPS_10BIT:
            *pu32FrameRate = 40;
            break;
        case SONY_IMX458_MIPI_2M_90FPS_10BIT:
            *pu32FrameRate = 90;
            break;
        case SONY_IMX458_MIPI_1M_129FPS_10BIT:
            *pu32FrameRate = 129;
            break;
        case OMNIVISION_OV12870_MIPI_2M_120FPS_10BIT:
            *pu32FrameRate = 120;
            break;
        case OMNIVISION_OV12870_MIPI_1M_240FPS_10BIT:
            *pu32FrameRate = 240;
            break;
        case OMNIVISION_OV9284_MIPI_1M_60FPS_10BIT:
            *pu32FrameRate = 60;
            break;
        case SONY_IMX290_MIPI_2M_120FPS_12BIT:
            *pu32FrameRate = 120;
            break;
        default:
            *pu32FrameRate = 30;
            break;

    }

	mppex_GetFrameRateBySensor(enMode, pu32FrameRate);
    return s32Ret;
}




HI_VOID SAMPLE_COMM_VI_GetSensorInfo(SAMPLE_VI_CONFIG_S* pstViConfig)
{
    HI_S32 i;

    for (i = 0; i < VI_MAX_DEV_NUM; i++)
    {
        pstViConfig->astViInfo[i].stSnsInfo.s32SnsId = i;
        pstViConfig->astViInfo[i].stSnsInfo.s32BusId = i;
        pstViConfig->astViInfo[i].stSnsInfo.MipiDev  = i;
        memset_s(&pstViConfig->astViInfo[i].stSnapInfo, sizeof(SAMPLE_SNAP_INFO_S), 0, sizeof(SAMPLE_SNAP_INFO_S));
        pstViConfig->astViInfo[i].stPipeInfo.bMultiPipe = HI_FALSE;
        pstViConfig->astViInfo[i].stPipeInfo.bVcNumCfged = HI_FALSE;
    }

    pstViConfig->astViInfo[0].stSnsInfo.enSnsType = SENSOR0_TYPE;
    pstViConfig->astViInfo[1].stSnsInfo.enSnsType = SENSOR1_TYPE;
}

combo_dev_t SAMPLE_COMM_VI_GetComboDevBySensor(SAMPLE_SNS_TYPE_E enMode, HI_S32 s32SnsIdx)
{
    combo_dev_t dev = 0;

    switch (enMode)
    {
        case SONY_IMX327_MIPI_2M_30FPS_12BIT:
        case SONY_IMX327_MIPI_2M_30FPS_12BIT_WDR2TO1:
        case SONY_IMX385_MIPI_2M_30FPS_12BIT:
        case SONY_IMX482_MIPI_2M_30FPS_12BIT:  
        case SONY_IMX307_MIPI_2M_30FPS_12BIT:
        case SONY_IMX390_MIPI_2M_30FPS_12BIT:
        case SONY_IMX307_MIPI_2M_30FPS_12BIT_WDR2TO1:
        case SONY_IMX335_2L_MIPI_5M_30FPS_10BIT:  
        case SONY_IMX335_MIPI_5M_30FPS_12BIT:
        case SONY_IMX335_MIPI_5M_30FPS_10BIT_WDR2TO1:
        case SONY_IMX335_MIPI_4M_30FPS_12BIT:
        case SONY_IMX335_MIPI_4M_30FPS_10BIT_WDR2TO1:
        case SONY_IMX458_MIPI_8M_30FPS_10BIT:
        case SONY_IMX458_MIPI_12M_20FPS_10BIT:
        case SONY_IMX458_MIPI_4M_60FPS_10BIT:
        case SONY_IMX458_MIPI_4M_40FPS_10BIT:
        case SONY_IMX458_MIPI_2M_90FPS_10BIT:
        case SONY_IMX458_MIPI_1M_129FPS_10BIT:
        case OMNIVISION_OV12870_MIPI_1M_240FPS_10BIT:
        case OMNIVISION_OV12870_MIPI_2M_120FPS_10BIT:
        case OMNIVISION_OV12870_MIPI_8M_30FPS_10BIT:
        case OMNIVISION_OV12870_MIPI_12M_30FPS_10BIT:
        case OMNIVISION_OV9284_MIPI_1M_60FPS_10BIT:
        case PANASONIC_MN34220_LVDS_2M_30FPS_12BIT:
        case OMNIVISION_OS04B10_MIPI_4M_30FPS_10BIT:
        case OMNIVISION_OS05A_MIPI_4M_30FPS_12BIT:
        case OMNIVISION_OS05A_MIPI_4M_30FPS_10BIT_WDR2TO1:
        case OMNIVISION_OS05A_2L_MIPI_1M_30FPS_10BIT:
        case OMNIVISION_OS05A_2L_MIPI_4M_30FPS_12BIT:
        case OMNIVISION_OS05A_2L_MIPI_4M_30FPS_10BIT_WDR2TO1:
        case OMNIVISION_OS08A10_MIPI_8M_30FPS_10BIT:
        case SMART_SC4210_MIPI_3M_30FPS_12BIT:
        case SMART_SC4210_MIPI_3M_30FPS_10BIT_WDR2TO1:
        case SONY_IMX415_MIPI_8M_30FPS_12BIT:
        case SONY_IMX415_MIPI_8M_20FPS_12BIT:
        case SONY_IMX415_MIPI_2M_60FPS_12BIT:
        case SONY_IMX334_MIPI_8M_30FPS_12BIT:
        case SONY_IMX585_MIPI_8M_30FPS_12BIT:
        case SONY_IMX585_MIPI_2M_30FPS_12BIT:
        case OMNIVISION_OV2775_MIPI_2M_30FPS_12BIT:
        case OMNIVISION_OV2775_MIPI_2M_30FPS_12BIT_WDR2TO1:
        case OMNIVISION_OV2775_MIPI_2M_30FPS_12BIT_WDR2TO1_HLCG:
        case OMNIVISION_OV2775_MIPI_2M_30FPS_12BIT_BUILTIN:
        case OMNIVISION_OV2775_2L_MIPI_2M_30FPS_12BIT:
        case OMNIVISION_OV2775_2L_MIPI_2M_30FPS_12BIT_WDR2TO1:
		//maohw add;  
        case SONY_IMX327_2L_MIPI_2M_30FPS_12BIT:
            if (0 == s32SnsIdx)
            {
                dev = 0;
            }
            else if (1 == s32SnsIdx)
            {
                dev =  1; //maohw dev = 2;
            }
            break;

        default:
            dev = 0;
            break;
    }
    mppex_GetComboDevBySensor(enMode, s32SnsIdx, &dev);
    return dev;
}

HI_S32 SAMPLE_COMM_VI_ConvertBitPixel(HI_U8 *pu8Data, HI_U32 u32DataNum, HI_U32 u32BitWidth, HI_U16 *pu16OutData)
{
    HI_S32 i, u32Tmp, s32OutCnt;
    HI_U32 u32Val;
    HI_U64 u64Val;
    HI_U8 *pu8Tmp = pu8Data;

    s32OutCnt = 0;
    switch(u32BitWidth)
    {
    case 10:
        {
            /* 4 pixels consist of 5 bytes  */
            u32Tmp = u32DataNum / 4;

            for (i = 0; i < u32Tmp; i++)
            {
                /* byte4 byte3 byte2 byte1 byte0 */
                pu8Tmp = pu8Data + 5 * i;
                u64Val = pu8Tmp[0] + ((HI_U32)pu8Tmp[1] << 8) + ((HI_U32)pu8Tmp[2] << 16) +
                         ((HI_U32)pu8Tmp[3] << 24) + ((HI_U64)pu8Tmp[4] << 32);

                pu16OutData[s32OutCnt++] = u64Val & 0x3ff;
                pu16OutData[s32OutCnt++] = (u64Val >> 10) & 0x3ff;
                pu16OutData[s32OutCnt++] = (u64Val >> 20) & 0x3ff;
                pu16OutData[s32OutCnt++] = (u64Val >> 30) & 0x3ff;
            }
        }
        break;
    case 12:
        {
            /* 2 pixels consist of 3 bytes  */
            u32Tmp = u32DataNum / 2;

            for (i = 0; i < u32Tmp; i++)
            {
                /* byte2 byte1 byte0 */
                pu8Tmp = pu8Data + 3 * i;
                u32Val = pu8Tmp[0] + (pu8Tmp[1] << 8) + (pu8Tmp[2] << 16);
                pu16OutData[s32OutCnt++] = u32Val & 0xfff;
                pu16OutData[s32OutCnt++] = (u32Val >> 12) & 0xfff;
            }
        }
        break;
    case 14:
        {
            /* 4 pixels consist of 7 bytes  */
            u32Tmp = u32DataNum / 4;

            for (i = 0; i < u32Tmp; i++)
            {
                pu8Tmp = pu8Data + 7 * i;
                u64Val = pu8Tmp[0] + ((HI_U32)pu8Tmp[1] << 8) + ((HI_U32)pu8Tmp[2] << 16) +
                         ((HI_U32)pu8Tmp[3] << 24) + ((HI_U64)pu8Tmp[4] << 32) +
                         ((HI_U64)pu8Tmp[5] << 40) + ((HI_U64)pu8Tmp[6] << 48);

                pu16OutData[s32OutCnt++] = u64Val & 0x3fff;
                pu16OutData[s32OutCnt++] = (u64Val >> 14) & 0x3fff;
                pu16OutData[s32OutCnt++] = (u64Val >> 28) & 0x3fff;
                pu16OutData[s32OutCnt++] = (u64Val >> 42) & 0x3fff;
            }
        }
        break;
    default:
        SAMPLE_PRT("unsuport bitWidth: %d\n", u32BitWidth);
        return HI_FAILURE;
        break;
    }

    return s32OutCnt;
}

static HI_S32 SAMPLE_COMM_VI_BitWidth2PixelFormat(HI_U32 u32Nbit, PIXEL_FORMAT_E *penPixelFormat)
{
    PIXEL_FORMAT_E enPixelFormat;

    if (8 == u32Nbit)
    {
        enPixelFormat = PIXEL_FORMAT_RGB_BAYER_8BPP;
    }
    else if (10 == u32Nbit)
    {
        enPixelFormat = PIXEL_FORMAT_RGB_BAYER_10BPP;
    }
    else if (12 == u32Nbit)
    {
        enPixelFormat = PIXEL_FORMAT_RGB_BAYER_12BPP;
    }
    else if (14 == u32Nbit)
    {
        enPixelFormat = PIXEL_FORMAT_RGB_BAYER_14BPP;
    }
    else if (16 == u32Nbit)
    {
        enPixelFormat = PIXEL_FORMAT_RGB_BAYER_16BPP;
    }
    else
    {
        return HI_FAILURE;
    }

    *penPixelFormat = enPixelFormat;
    return HI_SUCCESS;
}


HI_S32 SAMPLE_COMM_VI_SaveUncompressRaw(VIDEO_FRAME_S* pVBuf, HI_U32 u32Nbit, FILE* pfd)
{
    HI_U32  u32Height;
    HI_U64  u64PhyAddr;
    HI_U64  u64Size;
    HI_U8*  pu8VirAddr;
    HI_U16 *pu16Data = NULL;
    HI_U8  *pu8Data;
    PIXEL_FORMAT_E enPixelFormat = PIXEL_FORMAT_BUTT;

    SAMPLE_COMM_VI_BitWidth2PixelFormat(u32Nbit, &enPixelFormat);
    if (enPixelFormat != pVBuf->enPixelFormat)
    {
        SAMPLE_PRT("invalid pixel format:%d, u32Nbit: %d\n", pVBuf->enPixelFormat, u32Nbit);

        return HI_FAILURE;
    }

    u64Size = (pVBuf->u32Stride[0]) * ((HI_U64)pVBuf->u32Height);
    u64PhyAddr = pVBuf->u64PhyAddr[0];


    pu8VirAddr = (HI_U8*) HI_MPI_SYS_Mmap(u64PhyAddr, u64Size);
    if (NULL == pu8VirAddr)
    {
        SAMPLE_PRT("HI_MPI_SYS_Mmap fail !\n");

        return HI_FAILURE;
    }

    pu8Data = pu8VirAddr;
    if ((8 != u32Nbit) && (16 != u32Nbit))
    {
        pu16Data = (HI_U16*)malloc(pVBuf->u32Width * 2U);
        if (NULL == pu16Data)
        {
            SAMPLE_PRT("alloc memory failed\n");

            HI_MPI_SYS_Munmap(pu8VirAddr, u64Size);
            pu8VirAddr = NULL;
            return HI_FAILURE;
        }
    }

    /* save Y ----------------------------------------------------------------*/
    SAMPLE_PRT("saving......dump data......u32Stride[0]: %d, width: %d\n", pVBuf->u32Stride[0], pVBuf->u32Width);

    for (u32Height = 0; u32Height < pVBuf->u32Height; u32Height++)
    {
        if (8 == u32Nbit)
        {
            fwrite(pu8Data, pVBuf->u32Width, 1, pfd);
        }
        else if (16 == u32Nbit)
        {
            fwrite(pu8Data, pVBuf->u32Width, 2, pfd);
            fflush(pfd);
        }
        else
        {
            SAMPLE_COMM_VI_ConvertBitPixel(pu8Data, pVBuf->u32Width, u32Nbit, pu16Data);
            fwrite(pu16Data, pVBuf->u32Width, 2, pfd);
        }
        pu8Data += pVBuf->u32Stride[0];
    }
    fflush(pfd);

    SAMPLE_PRT("done u32TimeRef: %d!\n", pVBuf->u32TimeRef);

    if (NULL != pu16Data)
    {
        free(pu16Data);
    }
    HI_MPI_SYS_Munmap(pu8VirAddr, u64Size);
    pu8VirAddr = NULL;

    return HI_SUCCESS;
}

HI_S32 SAMPLE_COMM_VI_SaveCompressedRaw(VIDEO_FRAME_S* pVBuf, HI_U32 u32Nbit, FILE* pfd)
{
    HI_U32  u32Height;
    HI_U64  u64PhyAddr;
    HI_U64  u64Size;
    HI_U32  u32DataSize;
    HI_U16  u16HeadData = 0x0;
    HI_U8*  pu8VirAddr;
    HI_U8  *pu8Data;
    PIXEL_FORMAT_E enPixelFormat = PIXEL_FORMAT_BUTT;

    SAMPLE_COMM_VI_BitWidth2PixelFormat(u32Nbit, &enPixelFormat);
    if (enPixelFormat != pVBuf->enPixelFormat)
    {
        SAMPLE_PRT("invalid pixel format:%d, u32Nbit: %d\n", pVBuf->enPixelFormat, u32Nbit);

        return HI_FAILURE;
    }

    u64Size = (pVBuf->u32Stride[0]) * ((HI_U64)pVBuf->u32Height);
    u64PhyAddr = pVBuf->u64PhyAddr[0];


    pu8VirAddr = (HI_U8*) HI_MPI_SYS_Mmap(u64PhyAddr, u64Size);
    if (NULL == pu8VirAddr)
    {
        SAMPLE_PRT("HI_MPI_SYS_Mmap fail !\n");

        return HI_FAILURE;
    }

    pu8Data = pu8VirAddr;

    /* save Y ----------------------------------------------------------------*/
    SAMPLE_PRT("saving......dump data......u32Stride[0]: %d, width: %d\n", pVBuf->u32Stride[0], pVBuf->u32Width);

    for (u32Height = 0; u32Height < pVBuf->u32Height; u32Height++)
    {
        u16HeadData = *(HI_U16*)pu8Data;

        u32DataSize =  (u16HeadData + 1) * 16;

        fwrite(pu8Data, u32DataSize, 1, pfd);

        pu8Data += pVBuf->u32Stride[0];
    }

    fflush(pfd);

    SAMPLE_PRT("done u32TimeRef: %d!\n", pVBuf->u32TimeRef);

    HI_MPI_SYS_Munmap(pu8VirAddr, u64Size);
    pu8VirAddr = NULL;

    return HI_SUCCESS;
}

HI_S32 SAMPLE_COMM_VI_SaveRaw(VIDEO_FRAME_S* pVBuf, HI_U32 u32Nbit, FILE* pfd)
{
    if(COMPRESS_MODE_NONE == pVBuf->enCompressMode)
    {
        return SAMPLE_COMM_VI_SaveUncompressRaw(pVBuf, u32Nbit, pfd);
    }
    else
    {
        return SAMPLE_COMM_VI_SaveCompressedRaw(pVBuf, u32Nbit, pfd);
    }

}
HI_U32 SAMPLE_COMM_VI_PixelFormat2BitWidth(PIXEL_FORMAT_E  enPixelFormat)
{
    switch (enPixelFormat)
    {
        case PIXEL_FORMAT_RGB_BAYER_8BPP:
            return 8;

        case PIXEL_FORMAT_RGB_BAYER_10BPP:
            return 10;

        case PIXEL_FORMAT_RGB_BAYER_12BPP:
            return 12;

        case PIXEL_FORMAT_RGB_BAYER_14BPP:
            return 14;

        case PIXEL_FORMAT_RGB_BAYER_16BPP:
            return 16;

        default:
            return 0;
    }

}

char* SAMPLE_COMM_VI_CompressMode2String(COMPRESS_MODE_E enCompressMode)
{
    if(COMPRESS_MODE_NONE == enCompressMode)
    {
        return "CMP_NONE";
    }
    else if(COMPRESS_MODE_LINE == enCompressMode)
    {
        return "CMP_LINE";
    }
    else if(COMPRESS_MODE_SEG == enCompressMode)
    {
        return "CMP_SEG";
    }
    else
    {
        return "CMP_XXX";
    }
}

int SAMPLE_COMM_VI_SaveCompressParam(VI_CMP_PARAM_S* pCmpParam, FILE* pfd)
{
    fwrite(pCmpParam, sizeof(VI_CMP_PARAM_S), 1, pfd);
    fflush(pfd);
    return HI_SUCCESS;
}

HI_VOID* SAMPLE_COMM_VI_DumpRaw(HI_VOID* arg)
{
    HI_S32      s32Ret;
    VI_PIPE     ViPipe;
    HI_S32      s32Cnt;
    HI_S32      s32DumpCnt = 0;
    HI_U32      u32Width;
    HI_U32      u32Height;
    HI_U32      u32BitWidth;
    FILE*       pfile;
    HI_S32      s32MilliSec = 2000;
    HI_CHAR     name[128] = {0};
    VIDEO_FRAME_INFO_S stVideoFrame;
    VI_CMP_PARAM_S stCmpPara;
    HI_CHAR szThreadName[20];
    SAMPLE_VI_DUMP_THREAD_INFO_S* pstViDumpRawThreadInfo = HI_NULL;

    if (NULL == arg)
    {
        SAMPLE_PRT("arg is NULL\n");
        return NULL;
    }

    pstViDumpRawThreadInfo = (SAMPLE_VI_DUMP_THREAD_INFO_S*)arg;

    ViPipe = pstViDumpRawThreadInfo->ViPipe;
    s32Cnt = pstViDumpRawThreadInfo->s32Cnt;

    if (s32Cnt < 1)
    {
        SAMPLE_PRT("You really want to dump %d frame? That is impossible !\n", s32Cnt);
        return NULL;
    }

    snprintf(szThreadName, 20, "VI_PIPE%d_DUMP_RAW", ViPipe);
    prctl(PR_SET_NAME, szThreadName, 0,0,0);

    s32Ret = HI_MPI_VI_GetPipeFrame(ViPipe, &stVideoFrame, s32MilliSec);

    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("HI_MPI_VI_GetPipeFrame failed with %#x!\n", s32Ret);
        return HI_NULL;
    }

    s32Ret = HI_MPI_VI_ReleasePipeFrame(ViPipe, &stVideoFrame);

    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("HI_MPI_VI_ReleasePipeFrame failed with %#x!\n", s32Ret);
        return HI_NULL;
    }

    u32Width    = stVideoFrame.stVFrame.u32Width;
    u32Height   = stVideoFrame.stVFrame.u32Height;
    u32BitWidth = SAMPLE_COMM_VI_PixelFormat2BitWidth(stVideoFrame.stVFrame.enPixelFormat);

    snprintf(name, sizeof(name), VI_DATA_PATH"/%s_pipe%d_w%d_h%d_%dbits_%s_%d.raw",
             pstViDumpRawThreadInfo->aszName, ViPipe, u32Width, u32Height, u32BitWidth,
             SAMPLE_COMM_VI_CompressMode2String(stVideoFrame.stVFrame.enCompressMode), s32Cnt);
    pfile = fopen(name, "ab");

    if (NULL == pfile)
    {
        SAMPLE_PRT("open file %s fail !\n", name);
        return HI_NULL;
    }

    if(COMPRESS_MODE_NONE != stVideoFrame.stVFrame.enCompressMode)
    {
        if(HI_SUCCESS != HI_MPI_VI_GetPipeCmpParam(ViPipe, &stCmpPara))
        {
            SAMPLE_PRT("HI_MPI_VI_GetPipeCmpParam failed with %#x!\n", s32Ret);
            goto end;
        }

        SAMPLE_COMM_VI_SaveCompressParam(&stCmpPara, pfile);
    }

    while ((HI_TRUE == g_stViDumpRawThreadInfo.bDump) && (s32DumpCnt < s32Cnt))
    {
        s32Ret = HI_MPI_VI_GetPipeFrame(ViPipe, &stVideoFrame, s32MilliSec);

        if (HI_SUCCESS != s32Ret)
        {
            SAMPLE_PRT("HI_MPI_VI_GetPipeFrame failed with %#x!\n", s32Ret);
            goto end;
        }

        s32Ret = SAMPLE_COMM_VI_SaveRaw(&stVideoFrame.stVFrame, u32BitWidth, pfile);

        if (HI_SUCCESS != s32Ret)
        {
            SAMPLE_PRT("SAMPLE_COMM_VI_SaveRaw failed with %#x!\n", s32Ret);
            goto end;
        }

        s32DumpCnt++;

        s32Ret = HI_MPI_VI_ReleasePipeFrame(ViPipe, &stVideoFrame);

        if (HI_SUCCESS != s32Ret)
        {
            SAMPLE_PRT("HI_MPI_VI_ReleasePipeFrame failed with %#x!\n", s32Ret);
            goto end;
        }
    }

end:
    fclose(pfile);
    pfile = HI_NULL;
    return HI_NULL;
}

HI_S32 SAMPLE_COMM_VI_StartDumpRawThread(VI_PIPE ViPipe, HI_S32 s32Cnt, const HI_CHAR* pzsName)
{
    HI_S32 s32Ret;
    pthread_attr_t* pattr = NULL;

    g_stViDumpRawThreadInfo.ViPipe = ViPipe;
    g_stViDumpRawThreadInfo.s32Cnt = s32Cnt;
    g_stViDumpRawThreadInfo.bDump  = HI_TRUE;
    memcpy_s(g_stViDumpRawThreadInfo.aszName, sizeof(g_stViDumpRawThreadInfo.aszName), pzsName, sizeof(g_stViDumpRawThreadInfo.aszName));

    s32Ret = pthread_create(&g_stViDumpRawThreadInfo.ThreadId, pattr, SAMPLE_COMM_VI_DumpRaw, (HI_VOID*)&g_stViDumpRawThreadInfo);

    if (0 != s32Ret)
    {
        SAMPLE_PRT("create GetViFrame thread failed! %s\n", strerror(s32Ret));
        goto out;
    }

out:

    if (NULL != pattr)
    {
        pthread_attr_destroy(pattr);
    }

    return s32Ret;

}

HI_S32 SAMPLE_COMM_VI_StopDumpRawThread(HI_VOID)
{
    if (HI_FALSE != g_stViDumpRawThreadInfo.bDump)
    {
        g_stViDumpRawThreadInfo.bDump  = HI_FALSE;
        pthread_join(g_stViDumpRawThreadInfo.ThreadId, NULL);
    }

    return HI_SUCCESS;
}

static HI_CHAR *PRINT_FPNTYPE(ISP_FPN_TYPE_E enFpnType)
{
    if (ISP_FPN_TYPE_FRAME == enFpnType)
        return "Frame";
    else if (ISP_FPN_TYPE_LINE == enFpnType)
        return "Line";
    else
        return "NA";
}

static HI_U32 SAMPLE_VI_PixelFormat2BitWidth(VIDEO_FRAME_S *pstVFrame)
{
    switch (pstVFrame->enPixelFormat)
    {
        case PIXEL_FORMAT_RGB_BAYER_8BPP:
            return 8;

        case PIXEL_FORMAT_RGB_BAYER_10BPP:
            return 10;

        case PIXEL_FORMAT_RGB_BAYER_12BPP:
            return 12;

        case PIXEL_FORMAT_RGB_BAYER_14BPP:
            return 14;

        case PIXEL_FORMAT_RGB_BAYER_16BPP:
            return 16;

        case PIXEL_FORMAT_YVU_SEMIPLANAR_422:
        case PIXEL_FORMAT_YVU_SEMIPLANAR_420:
        case PIXEL_FORMAT_YUV_400:
            if (pstVFrame->enDynamicRange == DYNAMIC_RANGE_SDR8)
            {
                return 8;
            }
            else
            {
                return 10;
            }

        default:
            return 0;
    }

}

static HI_U32 SAMPLE_VI_GetRawStride(PIXEL_FORMAT_E enPixelFormat, HI_U32 u32Width, HI_U32 u32ByteAlign)
{
    HI_U32 u32Stride = 0;

    if (PIXEL_FORMAT_RGB_BAYER_16BPP == enPixelFormat)
    {
       u32Stride = ALIGN_UP(u32Width*2, 16);
    }
    else if (PIXEL_FORMAT_RGB_BAYER_12BPP == enPixelFormat)
    {
        if (1 == u32ByteAlign)
        {
            u32Stride = (u32Width * 12 + 127) / 128 * 128 / 8;
        }
        else
        {
            u32Stride = (u32Width * 12 ) / 8;

            if (0 == ((u32Width * 12) % 8))
            {
                u32Stride = (u32Width * 12 ) / 8; //-- pVBuf->u32Width * u32Nbit / 8
            }
            else
            {
                u32Stride = (u32Width * 12 ) / 8 + 8; //-- pVBuf->u32Width * u32Nbit / 8
            }
        }
    }
    else if (PIXEL_FORMAT_RGB_BAYER_10BPP == enPixelFormat)
    {
        u32Stride = (u32Width * 10 + 127) / 128 * 128 / 8;
        if (1 == u32ByteAlign)
        {
            u32Stride = (u32Width * 10 + 127) / 128 * 128 / 8;
        }
        else
        {
            u32Stride = (u32Width * 10 ) / 8;

            if (0 == ((u32Width * 10 ) % 8))
            {
                u32Stride = (u32Width * 10 ) / 8; //-- pVBuf->u32Width * u32Nbit / 8
            }
            else
            {
                u32Stride = (u32Width * 10 ) / 8 + 8; //-- pVBuf->u32Width * u32Nbit / 8
            }
        }
    }
    else if (PIXEL_FORMAT_RGB_BAYER_8BPP == enPixelFormat)
    {
        u32Stride = (u32Width * 8 + 127) / 128 * 128 / 8;
    }

    return u32Stride;
}

HI_S32 SAMPLE_VI_GetFrameBlkInfo(SAMPLE_VI_FRAME_CONFIG_S *pstFrmCfg, HI_S32 s32FrmCnt, SAMPLE_VI_FRAME_INFO_S *pastViFrameInfo)
{
    HI_U32 i = 0;
    HI_U32 u32Stride;
    HI_U32 u32LStride;
    HI_U32 u32CStride;
    HI_U32 u32LumaSize = 0;
    HI_U32 u32ChrmSize = 0;
    HI_U32 u32Size;
    HI_U64 u64PhyAddr;
    HI_U8 *pVirAddr;
    VB_POOL u32PoolId;
    VB_BLK VbBlk;
    PIXEL_FORMAT_E enPixelFormat;
    VB_POOL_CONFIG_S stVbPoolCfg;

    enPixelFormat = pstFrmCfg->enPixelFormat;

    if (enPixelFormat == PIXEL_FORMAT_YVU_SEMIPLANAR_422 
      ||enPixelFormat == PIXEL_FORMAT_YUV_SEMIPLANAR_422) //maohw
    {
        u32Stride = ALIGN_UP((pstFrmCfg->u32Width * 8 + 7) >> 3, DEFAULT_ALIGN);
        u32LStride  = u32Stride;
        u32CStride  = u32Stride;
        u32Size = u32Stride * pstFrmCfg->u32Height * 2;
        u32LumaSize = u32Stride * pstFrmCfg->u32Height;
        u32ChrmSize = u32Stride * pstFrmCfg->u32Height / 2;
    }
    else if (enPixelFormat == PIXEL_FORMAT_YVU_SEMIPLANAR_420)
    {
        u32Stride = ALIGN_UP((pstFrmCfg->u32Width * 8 + 7) >> 3, DEFAULT_ALIGN);
        u32LStride  = u32Stride;
        u32CStride  = u32Stride;
        u32Size = u32Stride * pstFrmCfg->u32Height * 3 / 2;
        u32LumaSize = u32Stride * pstFrmCfg->u32Height;
        u32ChrmSize = u32Stride * pstFrmCfg->u32Height / 4;
    }
    else if (enPixelFormat == PIXEL_FORMAT_YUV_400)
    {
        u32Stride = ALIGN_UP((pstFrmCfg->u32Width * 8 + 7) >> 3, DEFAULT_ALIGN);
        u32LStride  = u32Stride;
        u32CStride  = u32Stride;
        u32Size = u32Stride * pstFrmCfg->u32Height;
        u32LumaSize = u32Size;
        u32ChrmSize = 0;
    }
    else
    {
        u32Stride = SAMPLE_VI_GetRawStride(enPixelFormat, pstFrmCfg->u32Width, pstFrmCfg->u32ByteAlign);
        u32LStride  = u32Stride;
        u32CStride  = u32Stride;
        u32Size = u32Stride * pstFrmCfg->u32Height;
    }

    memset(&stVbPoolCfg, 0, sizeof(VB_POOL_CONFIG_S));
    stVbPoolCfg.u64BlkSize  = u32Size;
    stVbPoolCfg.u32BlkCnt   = s32FrmCnt;
    stVbPoolCfg.enRemapMode = VB_REMAP_MODE_NONE;
    u32PoolId = HI_MPI_VB_CreatePool(&stVbPoolCfg);
    if (VB_INVALID_POOLID == u32PoolId)
    {
        SAMPLE_PRT("HI_MPI_VB_CreatePool failed!\n");
        return HI_FAILURE;
    }

    for (i = 0; i < s32FrmCnt; i++)
    {
        VbBlk = HI_MPI_VB_GetBlock(u32PoolId, u32Size, HI_NULL);
        if (VB_INVALID_HANDLE == VbBlk)
        {
            SAMPLE_PRT("HI_MPI_VB_GetBlock err! size:%d\n", u32Size);
            return HI_FAILURE;
        }

        u64PhyAddr = HI_MPI_VB_Handle2PhysAddr(VbBlk);
        if (0 == u64PhyAddr)
        {
            SAMPLE_PRT("HI_MPI_VB_Handle2PhysAddr err!\n");
            return HI_FAILURE;
        }

        pVirAddr = (HI_U8 *)HI_MPI_SYS_Mmap(u64PhyAddr, u32Size);
        if (NULL == pVirAddr)
        {
            SAMPLE_PRT("HI_MPI_SYS_Mmap err!\n");
            return HI_FAILURE;
        }

        pastViFrameInfo[i].stVideoFrameInfo.u32PoolId = u32PoolId;
        pastViFrameInfo[i].stVideoFrameInfo.enModId = HI_ID_VI;
        pastViFrameInfo[i].stVideoFrameInfo.stVFrame.u64PhyAddr[0]   = u64PhyAddr;
        pastViFrameInfo[i].stVideoFrameInfo.stVFrame.u64PhyAddr[1]   = pastViFrameInfo[i].stVideoFrameInfo.stVFrame.u64PhyAddr[0] + u32LumaSize;
        pastViFrameInfo[i].stVideoFrameInfo.stVFrame.u64PhyAddr[2]   = pastViFrameInfo[i].stVideoFrameInfo.stVFrame.u64PhyAddr[1] + u32ChrmSize;

        pastViFrameInfo[i].stVideoFrameInfo.stVFrame.u64VirAddr[0]   = (HI_U64)(HI_UL)pVirAddr;
        pastViFrameInfo[i].stVideoFrameInfo.stVFrame.u64VirAddr[1]   = pastViFrameInfo[i].stVideoFrameInfo.stVFrame.u64VirAddr[0] + u32LumaSize;
        pastViFrameInfo[i].stVideoFrameInfo.stVFrame.u64VirAddr[2]   = pastViFrameInfo[i].stVideoFrameInfo.stVFrame.u64VirAddr[1] + u32ChrmSize;

        pastViFrameInfo[i].stVideoFrameInfo.stVFrame.u32Stride[0]    = u32LStride;
        pastViFrameInfo[i].stVideoFrameInfo.stVFrame.u32Stride[1]    = u32CStride;
        pastViFrameInfo[i].stVideoFrameInfo.stVFrame.u32Stride[2]    = u32CStride;

        pastViFrameInfo[i].stVideoFrameInfo.stVFrame.u32Width        = pstFrmCfg->u32Width;
        pastViFrameInfo[i].stVideoFrameInfo.stVFrame.u32Height       = pstFrmCfg->u32Height;
        pastViFrameInfo[i].stVideoFrameInfo.stVFrame.enPixelFormat   = pstFrmCfg->enPixelFormat;
        pastViFrameInfo[i].stVideoFrameInfo.stVFrame.enVideoFormat   = pstFrmCfg->enVideoFormat;
        pastViFrameInfo[i].stVideoFrameInfo.stVFrame.enCompressMode  = pstFrmCfg->enCompressMode;
        pastViFrameInfo[i].stVideoFrameInfo.stVFrame.enDynamicRange  = pstFrmCfg->enDynamicRange;
        pastViFrameInfo[i].stVideoFrameInfo.stVFrame.enField         = VIDEO_FIELD_FRAME;
        pastViFrameInfo[i].stVideoFrameInfo.stVFrame.enColorGamut    = COLOR_GAMUT_BT709;

        pastViFrameInfo[i].VbBlk   = VbBlk;
        pastViFrameInfo[i].u32Size = u32Size;
    }

    return HI_SUCCESS;
}

HI_S32 SAMPLE_VI_COMM_ReleaseFrameBlkInfo(HI_S32 s32FrmCnt, SAMPLE_VI_FRAME_INFO_S *pastViFrameInfo)
{
    HI_S32 s32Ret = HI_SUCCESS;
    HI_U32 i;
    VB_POOL u32PoolId;
    VB_BLK VbBlk;
    HI_U32 u32Size;

    for (i = 0; i < s32FrmCnt; i++)
    {
        VbBlk = pastViFrameInfo[i].VbBlk;
        s32Ret = HI_MPI_VB_ReleaseBlock(VbBlk);
        if (s32Ret != HI_SUCCESS)
        {
            SAMPLE_PRT("HI_MPI_VB_ReleaseBlock block 0x%x failure\n", VbBlk);
        }

        u32Size = pastViFrameInfo[i].u32Size;
        s32Ret = HI_MPI_SYS_Munmap((HI_VOID*)(HI_UL)pastViFrameInfo[i].stVideoFrameInfo.stVFrame.u64VirAddr[0], u32Size);
        if (s32Ret != HI_SUCCESS)
        {
            SAMPLE_PRT("HI_MPI_SYS_Munmap failure!\n");
        }
    }

    u32PoolId = pastViFrameInfo[0].stVideoFrameInfo.u32PoolId;
    HI_MPI_VB_DestroyPool(u32PoolId);

    return HI_SUCCESS;
}

HI_VOID SAMPLE_COMM_VI_SaveFpnFile(ISP_FPN_TYPE_E enFpnType, ISP_FPN_FRAME_INFO_S *pVBuf, FILE *pfd)
{
    HI_U8   *pU8VBufVirt_Y;
    HI_U32  u32FpnHeight;
    HI_S32  i;

    u32FpnHeight = pVBuf->stFpnFrame.stVFrame.u32Height;
    pU8VBufVirt_Y = (HI_U8 *)(HI_UL)pVBuf->stFpnFrame.stVFrame.u64VirAddr[0];

    /* save Y ---------------------------------------------------------------- */
    fprintf(stderr, "FPN: saving......Raw data......u32Stride[0]: %d, width: %d, height: %d\n"
            , pVBuf->stFpnFrame.stVFrame.u32Stride[0]
            , pVBuf->stFpnFrame.stVFrame.u32Width
            , u32FpnHeight);
    fprintf(stderr, "phy Addr: 0x%llx\n", pVBuf->stFpnFrame.stVFrame.u64PhyAddr[0]);
    fprintf(stderr, "Please wait a moment to save data.\n");
    fflush(stderr);

    fwrite(pU8VBufVirt_Y, pVBuf->u32FrmSize, 1, pfd);

    /* save offset */
    for (i = 0; i < VI_MAX_SPLIT_NODE_NUM; i++)
    {
        fwrite(&pVBuf->u32Offset[i], 4, 1, pfd);
    }

    /* save compress mode */
    fwrite(&pVBuf->stFpnFrame.stVFrame.enCompressMode, 4, 1, pfd);

    /* save fpn frame size */
    fwrite(&pVBuf->u32FrmSize, 4, 1, pfd);

    /* save ISO */
    fwrite(&pVBuf->u32Iso, 4, 1, pfd);
    fflush(pfd);

}


HI_VOID SAMPLE_COMM_VI_ReadFpnFile(ISP_FPN_FRAME_INFO_S *pstFpnFrmInfo, FILE *pfd)
{
    VIDEO_FRAME_INFO_S *pstFpnFrame;
    HI_S32 i;

    printf("u32FrmSize: %d!!\n", pstFpnFrmInfo->u32FrmSize);

    pstFpnFrame = &pstFpnFrmInfo->stFpnFrame;
    fread((HI_U8 *)(HI_UL)pstFpnFrame->stVFrame.u64VirAddr[0], pstFpnFrmInfo->u32FrmSize, 1, pfd);
    for (i = 0; i < VI_MAX_SPLIT_NODE_NUM; i++)
    {
        fread((HI_U8 *)&pstFpnFrmInfo->u32Offset[i], 4, 1, pfd);
    }
    fread((HI_U8 *)&pstFpnFrame->stVFrame.enCompressMode, 4, 1, pfd);
    fread((HI_U8 *)&pstFpnFrmInfo->u32FrmSize, 4, 1, pfd);
    fread((HI_U8 *)&pstFpnFrmInfo->u32Iso, 4, 1, pfd);
}

HI_S32 SAMPLE_COMM_VI_FpnCalibrateConfig(VI_PIPE ViPipe, SAMPLE_VI_FPN_CALIBRATE_INFO_S *pstViFpnCalibrateInfo)
{
    SAMPLE_VI_FRAME_CONFIG_S stFrmCfg;
    SAMPLE_VI_FRAME_INFO_S stViFrameInfo;
    ISP_FPN_CALIBRATE_ATTR_S stCalibrateAttr;
    ISP_PUB_ATTR_S stPubAttr;
    HI_S32 s32Ret;
    HI_S32 i;
    FILE *pfd;
    VI_CHN ViChn = 0;
    HI_CHAR FpnFileName[FILE_NAME_LEN];

    s32Ret = HI_MPI_ISP_GetPubAttr(ViPipe, &stPubAttr);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("get isp pub attr failed!\n");
        return s32Ret;
    }

    stFrmCfg.u32Width = stPubAttr.stWndRect.u32Width;
    if (ISP_FPN_TYPE_LINE == pstViFpnCalibrateInfo->enFpnType)
    {
        stFrmCfg.u32Height = 1;
    }
    else
    {
        stFrmCfg.u32Height = stPubAttr.stWndRect.u32Height;
    }

    stFrmCfg.u32ByteAlign = 0;
    stFrmCfg.enPixelFormat = pstViFpnCalibrateInfo->enPixelFormat;
    stFrmCfg.enCompressMode = pstViFpnCalibrateInfo->enCompressMode;
    stFrmCfg.enVideoFormat = VIDEO_FORMAT_LINEAR;
    stFrmCfg.enDynamicRange = DYNAMIC_RANGE_SDR8;
    s32Ret = SAMPLE_VI_GetFrameBlkInfo(&stFrmCfg, 1, &stViFrameInfo);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("SAMPLE_VI_GetFrameBlkInfo failed!\n");
        return s32Ret;
    }

    printf("Pipe %d Width=%d, Height=%d.\n", ViPipe, stFrmCfg.u32Width, stFrmCfg.u32Height);

    printf("please turn off camera aperture to start calibrate!\n");
    printf("hit Enter key ,start calibrate!\n");
    getchar();

    s32Ret = HI_MPI_VI_DisableChn(ViPipe, ViChn);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("disable vi chn %d failed!", ViChn);
        goto EXIT0;
    }

    stCalibrateAttr.enFpnType             = pstViFpnCalibrateInfo->enFpnType;
    stCalibrateAttr.u32FrameNum           = pstViFpnCalibrateInfo->u32FrameNum;
    stCalibrateAttr.u32Threshold          = pstViFpnCalibrateInfo->u32Threshold;
    stCalibrateAttr.stFpnCaliFrame.u32Iso = 0;
    for (i = 0; i < VI_MAX_SPLIT_NODE_NUM; i++)
    {
        stCalibrateAttr.stFpnCaliFrame.u32Offset[i] = 0;
    }
    stCalibrateAttr.stFpnCaliFrame.u32FrmSize = stViFrameInfo.u32Size;
    memcpy(&stCalibrateAttr.stFpnCaliFrame.stFpnFrame, &stViFrameInfo.stVideoFrameInfo, sizeof(VIDEO_FRAME_INFO_S));

    s32Ret = HI_MPI_ISP_FPNCalibrate(ViPipe, &stCalibrateAttr);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("HI_MPI_ISP_FPNCalibrate err: 0x%x\n", s32Ret);
        goto EXIT1;
    }

    /* save FPN raw data */
    snprintf(FpnFileName, FILE_NAME_LEN, VI_DATA_PATH"/FPN_%s_w%d_h%d_%dbit.raw",
             PRINT_FPNTYPE(stCalibrateAttr.enFpnType),
             stPubAttr.stWndRect.u32Width,
             stPubAttr.stWndRect.u32Height,
             SAMPLE_VI_PixelFormat2BitWidth(&stViFrameInfo.stVideoFrameInfo.stVFrame));

    printf("\nafter calibrate ");
    for (i = 0; i < VI_MAX_SPLIT_NODE_NUM; i++)
    {
        printf("offset[%d] = 0x%x, ", i, stCalibrateAttr.stFpnCaliFrame.u32Offset[i]);
    }
    printf("u32FrmSize = %d\n", stCalibrateAttr.stFpnCaliFrame.u32FrmSize);
    printf("ISO = %d\n", stCalibrateAttr.stFpnCaliFrame.u32Iso);

    printf("save dark frame file: %s!\n", FpnFileName);

    pfd = fopen(FpnFileName, "wb");
    if (NULL == pfd)
    {
        printf("open file %s err!\n", FpnFileName);
        goto EXIT1;
    }

    SAMPLE_COMM_VI_SaveFpnFile(stCalibrateAttr.enFpnType, &stCalibrateAttr.stFpnCaliFrame, pfd);

    fclose(pfd);

EXIT1:
    s32Ret = HI_MPI_VI_EnableChn(ViPipe, ViChn);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("enable vi chn %d failed!", ViChn);
    }
EXIT0:
    SAMPLE_VI_COMM_ReleaseFrameBlkInfo(1, &stViFrameInfo);

    return s32Ret;
}

HI_S32 SAMPLE_COMM_VI_FpnCorrectionConfig(VI_PIPE ViPipe, SAMPLE_VI_FPN_CORRECTION_INFO_S *pstViFpnCorrectionInfo)
{
    SAMPLE_VI_FRAME_CONFIG_S stFrmCfg;
    SAMPLE_VI_FRAME_INFO_S *pstViFrameInfo;
    ISP_FPN_ATTR_S stFPNAttr;
    ISP_PUB_ATTR_S stPubAttr;
    HI_S32 s32Ret;
    FILE *pfd;
    HI_S32 i;
    HI_CHAR FpnFileName[FILE_NAME_LEN];

    s32Ret = HI_MPI_ISP_GetPubAttr(ViPipe, &stPubAttr);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("get isp pub attr failed!\n");
        return s32Ret;
    }

    if (ISP_FPN_TYPE_LINE == pstViFpnCorrectionInfo->enFpnType)
    {
        stFrmCfg.u32Height = 1;
    }
    else
    {
        stFrmCfg.u32Height = stPubAttr.stWndRect.u32Height;
    }

    pstViFrameInfo = &pstViFpnCorrectionInfo->stViFrameInfo;

    stFrmCfg.u32Width = stPubAttr.stWndRect.u32Width;
    stFrmCfg.u32ByteAlign = 0;
    stFrmCfg.enPixelFormat = pstViFpnCorrectionInfo->enPixelFormat;
    stFrmCfg.enCompressMode = pstViFpnCorrectionInfo->enCompressMode;
    stFrmCfg.enVideoFormat = VIDEO_FORMAT_LINEAR;
    stFrmCfg.enDynamicRange = DYNAMIC_RANGE_SDR8;
    s32Ret = SAMPLE_VI_GetFrameBlkInfo(&stFrmCfg, 1, pstViFrameInfo);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("VI_MST_COMM_GetFrameBlkInfo failed!\n");
        return s32Ret;
    }

    stFPNAttr.stFpnFrmInfo.u32FrmSize = pstViFrameInfo->u32Size;
    memcpy(&stFPNAttr.stFpnFrmInfo.stFpnFrame, &pstViFrameInfo->stVideoFrameInfo, sizeof(VIDEO_FRAME_INFO_S));

    /* save FPN raw data */
    snprintf(FpnFileName, FILE_NAME_LEN, VI_DATA_PATH"/FPN_%s_w%d_h%d_%dbit.raw",
                                          PRINT_FPNTYPE(pstViFpnCorrectionInfo->enFpnType),
                                          stPubAttr.stWndRect.u32Width,
                                          stPubAttr.stWndRect.u32Height,
                                          SAMPLE_VI_PixelFormat2BitWidth(&pstViFrameInfo->stVideoFrameInfo.stVFrame));

    printf("====to open Fpn raw file: %s. \n", FpnFileName);
    pfd = fopen(FpnFileName, "rb");
    if (NULL == pfd)
    {
        SAMPLE_PRT("open file %s err!\n", FpnFileName);
        goto EXIT0;
    }

    SAMPLE_COMM_VI_ReadFpnFile(&stFPNAttr.stFpnFrmInfo, pfd);

    fclose(pfd);

    stFPNAttr.bEnable = HI_TRUE;
    stFPNAttr.enOpType = pstViFpnCorrectionInfo->enOpType;
    stFPNAttr.enFpnType = pstViFpnCorrectionInfo->enFpnType;
    stFPNAttr.stManual.u32Strength = pstViFpnCorrectionInfo->u32Strength;

    for (i = 0; i < VI_MAX_SPLIT_NODE_NUM; i++)
    {
        printf("offset[%d] = 0x%x; ", i, stFPNAttr.stFpnFrmInfo.u32Offset[i]);
    }
    printf("\n");
    printf("u32FrmSize = %d.\n", stFPNAttr.stFpnFrmInfo.u32FrmSize);
    printf("ISO = %d.\n", stFPNAttr.stFpnFrmInfo.u32Iso);

    printf("\nstFPNAttr: u32Width = %d, u32Height = %d\n",
            stFPNAttr.stFpnFrmInfo.stFpnFrame.stVFrame.u32Width, stFPNAttr.stFpnFrmInfo.stFpnFrame.stVFrame.u32Height);
    printf("FrmSize: %d, stride: %d, compress mode: %d\n", stFPNAttr.stFpnFrmInfo.u32FrmSize,
                                        stFPNAttr.stFpnFrmInfo.stFpnFrame.stVFrame.u32Stride[0],
                                        stFPNAttr.stFpnFrmInfo.stFpnFrame.stVFrame.enCompressMode);

    s32Ret = HI_MPI_ISP_SetFPNAttr(ViPipe, &stFPNAttr);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("fpn correction fail 0x%x\n", s32Ret);
        goto EXIT0;
    }

    return HI_SUCCESS;

EXIT0:
    SAMPLE_VI_COMM_ReleaseFrameBlkInfo(1, pstViFrameInfo);

    return s32Ret;
}

HI_S32 SAMPLE_COMM_VI_DisableFpnCorrection(VI_PIPE ViPipe, SAMPLE_VI_FPN_CORRECTION_INFO_S *pstViFpnCorrectionInfo)
{
    ISP_FPN_ATTR_S stFPNAttr;
    HI_S32 s32Ret;

    s32Ret = HI_MPI_ISP_GetFPNAttr(ViPipe, &stFPNAttr);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("get fpn correction fail, ret 0x%x\n", s32Ret);
        return HI_FAILURE;
    }

    stFPNAttr.bEnable = HI_FALSE;
    s32Ret = HI_MPI_ISP_SetFPNAttr(ViPipe, &stFPNAttr);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("set fpn correction fail, ret 0x%x\n", s32Ret);
        return HI_FAILURE;
    }

    SAMPLE_VI_COMM_ReleaseFrameBlkInfo(1, &pstViFpnCorrectionInfo->stViFrameInfo);

    return HI_SUCCESS;

}

HI_S32 SAMPLE_VI_PlanToSemi(HI_U8* pY, HI_S32 yStride,
                           HI_U8* pU, HI_S32 uStride,
                           HI_U8* pV, HI_S32 vStride,
                           HI_S32 picWidth, HI_S32 picHeight)
{
    HI_S32 i;
    HI_U8 *pTmpU, *ptu;
    HI_U8 *pTmpV, *ptv;

    HI_S32 s32HafW = uStride >> 1 ;
    HI_S32 s32HafH = picHeight >> 1 ;
    HI_S32 s32Size = s32HafW * s32HafH;

    pTmpU = malloc(s32Size);
    ptu = pTmpU;
    pTmpV = malloc(s32Size);
    ptv = pTmpV;

    memcpy(pTmpU, pU, s32Size);
    memcpy(pTmpV, pV, s32Size);

    for (i = 0; i<s32Size >> 1; i++)
    {
        *pU++ = *pTmpV++;
        *pU++ = *pTmpU++;

    }
    for (i = 0; i<s32Size >> 1; i++)
    {
        *pV++ = *pTmpV++;
        *pV++ = *pTmpU++;
    }

    free(ptu);
    free(ptv);

    return HI_SUCCESS;
}

HI_S32 SAMPLE_VI_PlanToSemi_422(HI_U8* pY, HI_S32 yStride,
                               HI_U8* pU, HI_S32 uStride,
                               HI_U8* pV, HI_S32 vStride,
                               HI_S32 picWidth, HI_S32 picHeight)
{
    HI_S32 i;
    HI_U8 *pTmpU, *ptu;
    HI_U8 *pTmpV, *ptv;
    HI_S32 s32HafW = uStride >> 1;
    HI_S32 s32HafH = picHeight;
    HI_S32 s32Size = s32HafW * s32HafH;

    pTmpU = malloc(s32Size);
    ptu = pTmpU;
    pTmpV = malloc(s32Size);
    ptv = pTmpV;

    memcpy(pTmpU, pU, s32Size);
    memcpy(pTmpV, pV, s32Size);

    for (i = 0; i<s32Size >> 1; i++)
    {
        *pU++ = *pTmpV++;
        *pU++ = *pTmpU++;

    }

    for (i = 0; i<s32Size >> 1; i++)
    {
        *pV++ = *pTmpV++;
        *pV++ = *pTmpU++;
    }

    free(ptu);
    free(ptv);

    return HI_SUCCESS;
}

HI_VOID SAMPLE_VI_ReadOneFrame_400( FILE* fp, HI_U8* pY, HI_U8* pU, HI_U8* pV,
                                   HI_U32 width, HI_U32 height, HI_U32 stride, HI_U32 stride2)
{
    HI_U8 *pDst;
    HI_U32 u32Row;

    pDst = pY;
    for (u32Row = 0; u32Row < height; u32Row++)
    {
        fread(pDst, width, 1, fp);
        pDst += stride;
    }

    char temp[1];
    fread(temp, 1, 1, fp);

    if (feof(fp) != 0)
    {
        printf("file is EOF!\n");
    }
    else
    {
        fseek(fp, ftell(fp) - 1, SEEK_SET);
    }

}

HI_VOID SAMPLE_VI_ReadOneFrame( FILE* fp, HI_U8* pY, HI_U8* pU, HI_U8* pV,
                               HI_U32 width, HI_U32 height, HI_U32 stride, HI_U32 stride2)
{
    HI_U8 *pDst;
    HI_U32 u32Row;
    pDst = pY;

    for (u32Row = 0; u32Row < height; u32Row++)
    {
        fread(pDst, width, 1, fp);
        pDst += stride;
    }
    pDst = pU;
    for (u32Row = 0; u32Row < height / 2; u32Row++)
    {
        fread(pDst, width / 2, 1, fp);
        pDst += stride2;
    }

    pDst = pV;
    for (u32Row = 0; u32Row < height / 2; u32Row++)
    {
        fread(pDst, width / 2, 1, fp);
        pDst += stride2;
    }

    char temp[1];
    fread(temp, 1, 1, fp);

    if (feof(fp) != 0)
    {
        printf("file is EOF!\n");
    }
    else
    {
        fseek(fp, ftell(fp) - 1, SEEK_SET);
    }

}

HI_VOID SAMPLE_VI_ReadOneFrame_422( FILE* fp, HI_U8* pY, HI_U8* pU, HI_U8* pV,
                                   HI_U32 width, HI_U32 height, HI_U32 stride, HI_U32 stride2)
{
    HI_U8 *pDst;
    HI_U32 u32Row;
    pDst = pY;

    for (u32Row = 0; u32Row < height; u32Row++)
    {
        fread(pDst, width, 1, fp);
        pDst += stride;
    }

    pDst = pU;
    for (u32Row = 0; u32Row < height; u32Row++)
    {
        fread(pDst, width >> 1, 1, fp);
        pDst += stride2;
    }

    pDst = pV;
    for (u32Row = 0; u32Row < height; u32Row++)
    {
        fread(pDst, width >> 1, 1, fp);
        pDst += stride2;
    }

    char temp[1];
    fread(temp, 1, 1, fp);

    if (feof(fp) != 0)
    {
        printf("the End-of-file is reached\n");
    }
    else
    {
        fseek(fp, ftell(fp) - 1, SEEK_SET);
    }

}

static HI_VOID SAMPLE_VI_COMM_ReadYuvFile(FILE *pfd, VIDEO_FRAME_INFO_S *pstVideoFrameInfo)
{
    if (pstVideoFrameInfo->stVFrame.enPixelFormat == PIXEL_FORMAT_YVU_SEMIPLANAR_422)
    {
        SAMPLE_VI_ReadOneFrame_422(pfd, (HI_U8 *)(HI_UL)pstVideoFrameInfo->stVFrame.u64VirAddr[0],
                                   (HI_U8 *)(HI_UL)pstVideoFrameInfo->stVFrame.u64VirAddr[1], (HI_U8 *)(HI_UL)pstVideoFrameInfo->stVFrame.u64VirAddr[2],
                                   pstVideoFrameInfo->stVFrame.u32Width, pstVideoFrameInfo->stVFrame.u32Height,
                                   pstVideoFrameInfo->stVFrame.u32Stride[0], pstVideoFrameInfo->stVFrame.u32Stride[1] >> 1);

        sleep(1);

        SAMPLE_VI_PlanToSemi_422((HI_U8 *)(HI_UL)pstVideoFrameInfo->stVFrame.u64VirAddr[0], pstVideoFrameInfo->stVFrame.u32Stride[0],
                                 (HI_U8 *)(HI_UL)pstVideoFrameInfo->stVFrame.u64VirAddr[1], pstVideoFrameInfo->stVFrame.u32Stride[1],
                                 (HI_U8 *)(HI_UL)pstVideoFrameInfo->stVFrame.u64VirAddr[2], pstVideoFrameInfo->stVFrame.u32Stride[1],
                                 pstVideoFrameInfo->stVFrame.u32Width, pstVideoFrameInfo->stVFrame.u32Height);
    }
    else if (pstVideoFrameInfo->stVFrame.enPixelFormat == PIXEL_FORMAT_YVU_SEMIPLANAR_420)
    {
        SAMPLE_VI_ReadOneFrame(pfd, (HI_U8 *)(HI_UL)pstVideoFrameInfo->stVFrame.u64VirAddr[0],
                               (HI_U8 *)(HI_UL)pstVideoFrameInfo->stVFrame.u64VirAddr[1], (HI_U8 *)(HI_UL)pstVideoFrameInfo->stVFrame.u64VirAddr[2],
                               pstVideoFrameInfo->stVFrame.u32Width, pstVideoFrameInfo->stVFrame.u32Height,
                               pstVideoFrameInfo->stVFrame.u32Stride[0], pstVideoFrameInfo->stVFrame.u32Stride[1] >> 1);

        sleep(1);

        SAMPLE_VI_PlanToSemi((HI_U8 *)(HI_UL)pstVideoFrameInfo->stVFrame.u64VirAddr[0], pstVideoFrameInfo->stVFrame.u32Stride[0],
                             (HI_U8 *)(HI_UL)pstVideoFrameInfo->stVFrame.u64VirAddr[1], pstVideoFrameInfo->stVFrame.u32Stride[1],
                             (HI_U8 *)(HI_UL)pstVideoFrameInfo->stVFrame.u64VirAddr[2], pstVideoFrameInfo->stVFrame.u32Stride[1],
                             pstVideoFrameInfo->stVFrame.u32Width, pstVideoFrameInfo->stVFrame.u32Height);
    }
    else
    {
        SAMPLE_VI_ReadOneFrame_400(pfd, (HI_U8 *)(HI_UL)pstVideoFrameInfo->stVFrame.u64VirAddr[0],
                                   (HI_U8 *)(HI_UL)pstVideoFrameInfo->stVFrame.u64VirAddr[1], (HI_U8 *)(HI_UL)pstVideoFrameInfo->stVFrame.u64VirAddr[2],
                                   pstVideoFrameInfo->stVFrame.u32Width, pstVideoFrameInfo->stVFrame.u32Height,
                                   pstVideoFrameInfo->stVFrame.u32Stride[0], pstVideoFrameInfo->stVFrame.u32Stride[1] >> 1);
    }
}

HI_S32 SAMPLE_COMM_VI_Load_UserPic(const char *pszYuvFile, VI_USERPIC_ATTR_S *pstUsrPic, SAMPLE_VI_FRAME_INFO_S *pstViFrameInfo)
{
    FILE *pfd;
    SAMPLE_VI_FRAME_CONFIG_S stFrmCfg;
    HI_S32 s32Ret;

    stFrmCfg.u32Width = pstUsrPic->unUsrPic.stUsrPicFrm.stVFrame.u32Width;
    stFrmCfg.u32Height = pstUsrPic->unUsrPic.stUsrPicFrm.stVFrame.u32Height;
    stFrmCfg.u32ByteAlign = 0;
    stFrmCfg.enPixelFormat = pstUsrPic->unUsrPic.stUsrPicFrm.stVFrame.enPixelFormat;
    stFrmCfg.enCompressMode = COMPRESS_MODE_NONE;
    stFrmCfg.enVideoFormat = VIDEO_FORMAT_LINEAR;
    stFrmCfg.enDynamicRange = DYNAMIC_RANGE_SDR8;

    s32Ret = SAMPLE_VI_GetFrameBlkInfo(&stFrmCfg, 1, pstViFrameInfo);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("SAMPLE_VI_GetFrameBlkInfo failed!\n");
        return s32Ret;
    }

    memcpy(&pstUsrPic->unUsrPic.stUsrPicFrm, &pstViFrameInfo->stVideoFrameInfo, sizeof(VIDEO_FRAME_INFO_S));

    printf("====to open YUV file: %s. \n", pszYuvFile);
    pfd = fopen(pszYuvFile, "rb");
    if (!pfd)
    {
        SAMPLE_PRT("open file -> %s fail \n", pszYuvFile);
        goto EXIT;
    }

    SAMPLE_VI_COMM_ReadYuvFile(pfd, &pstUsrPic->unUsrPic.stUsrPicFrm);

    fclose(pfd);

    return HI_SUCCESS;

EXIT:
    SAMPLE_VI_COMM_ReleaseFrameBlkInfo(1, pstViFrameInfo);
    return s32Ret;
}

HI_VOID SAMPLE_COMM_VI_Release_UserPic(SAMPLE_VI_FRAME_INFO_S *pstViFrameInfo)
{
    SAMPLE_VI_COMM_ReleaseFrameBlkInfo(1, pstViFrameInfo);
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */
