
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

#define MIPI_DEV_NODE       "/dev/hi_mipi"

#define MAX_FRAME_WIDTH     8192

SAMPLE_VI_DUMP_THREAD_INFO_S g_stViDumpRawThreadInfo;

combo_dev_attr_t MIPI_4lane_CHN0_SENSOR_IMX477_12BIT_12M_NOWDR_ATTR =
{
    .devno = 0,
    .input_mode = INPUT_MODE_MIPI,
    .data_rate = MIPI_DATA_RATE_X1,
    .img_rect = {0, 0, 4000, 3000},

    {
        .mipi_attr =
        {
            DATA_TYPE_RAW_12BIT,
            HI_MIPI_WDR_MODE_NONE,
            {0, 1, 2, 3, -1, -1, -1, -1}
        }
    }
};

combo_dev_attr_t MIPI_4lane_CHN2_SENSOR_IMX477_12BIT_12M_NOWDR_ATTR =
{
    .devno = 2,
    .input_mode = INPUT_MODE_MIPI,
    .data_rate = MIPI_DATA_RATE_X1,
    .img_rect = {0, 0, 4000, 3000},

    {
        .mipi_attr =
        {
            DATA_TYPE_RAW_12BIT,
            HI_MIPI_WDR_MODE_NONE,
            {4, 5, 6, 7, -1, -1, -1, -1}
        }
    }
};

combo_dev_attr_t MIPI_4lane_CHN4_SENSOR_IMX477_12BIT_12M_NOWDR_ATTR =
{
    .devno = 4,
    .input_mode = INPUT_MODE_MIPI,
    .data_rate = MIPI_DATA_RATE_X1,
    .img_rect = {0, 0, 4000, 3000},

    {
        .mipi_attr =
        {
            DATA_TYPE_RAW_12BIT,
            HI_MIPI_WDR_MODE_NONE,
            {8, 9, 10, 11, -1, -1, -1, -1}
        }
    }
};

combo_dev_attr_t MIPI_4lane_CHN6_SENSOR_IMX477_12BIT_12M_NOWDR_ATTR =
{
    .devno = 6,
    .input_mode = INPUT_MODE_MIPI,
    .data_rate = MIPI_DATA_RATE_X1,
    .img_rect = {0, 0, 4000, 3000},

    {
        .mipi_attr =
        {
            DATA_TYPE_RAW_12BIT,
            HI_MIPI_WDR_MODE_NONE,
            {12, 13, 14, 15, -1, -1, -1, -1}
        }
    }
};

combo_dev_attr_t MIPI_4lane_CHN0_SENSOR_IMX477_10BIT_9M_NOWDR_ATTR =
{
    .devno = 0,
    .input_mode = INPUT_MODE_MIPI,
    .data_rate = MIPI_DATA_RATE_X1,
    .img_rect = {0, 0, 3000, 3000},

    {
        .mipi_attr =
        {
            DATA_TYPE_RAW_10BIT,
            HI_MIPI_WDR_MODE_NONE,
            {0, 1, 2, 3, -1, -1, -1, -1}
        }
    }
};

combo_dev_attr_t MIPI_4lane_CHN2_SENSOR_IMX477_10BIT_9M_NOWDR_ATTR =
{
    .devno = 2,
    .input_mode = INPUT_MODE_MIPI,
    .data_rate = MIPI_DATA_RATE_X1,
    .img_rect = {0, 0, 3000, 3000},

    {
        .mipi_attr =
        {
            DATA_TYPE_RAW_10BIT,
            HI_MIPI_WDR_MODE_NONE,
            {4, 5, 6, 7, -1, -1, -1, -1}/*{8, 9, 10, 11, -1, -1, -1, -1}*/
        }
    }
};

combo_dev_attr_t MIPI_4lane_CHN4_SENSOR_IMX477_10BIT_9M_NOWDR_ATTR =
{
    .devno = 4,
    .input_mode = INPUT_MODE_MIPI,
    .data_rate = MIPI_DATA_RATE_X1,
    .img_rect = {0, 0, 3000, 3000},

    {
        .mipi_attr =
        {
            DATA_TYPE_RAW_10BIT,
            HI_MIPI_WDR_MODE_NONE,
            {8, 9, 10, 11, -1, -1, -1, -1}
        }
    }
};

combo_dev_attr_t MIPI_4lane_CHN6_SENSOR_IMX477_10BIT_9M_NOWDR_ATTR =
{
    .devno = 6,
    .input_mode = INPUT_MODE_MIPI,
    .data_rate = MIPI_DATA_RATE_X1,
    .img_rect = {0, 0, 3000, 3000},

    {
        .mipi_attr =
        {
            DATA_TYPE_RAW_10BIT,
            HI_MIPI_WDR_MODE_NONE,
            {12, 13, 14, 15, -1, -1, -1, -1}
        }
    }
};

combo_dev_attr_t MIPI_4lane_CHN0_SENSOR_IMX477_10BIT_8M_NOWDR_ATTR =
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
            {0, 1, 2, 3, -1, -1, -1, -1}
        }
    }
};

combo_dev_attr_t MIPI_4lane_CHN2_SENSOR_IMX477_10BIT_8M_NOWDR_ATTR =
{
    .devno = 2,
    .input_mode = INPUT_MODE_MIPI,
    .data_rate = MIPI_DATA_RATE_X1,
    .img_rect = {0, 0, 3840, 2160},

    {
        .mipi_attr =
        {
            DATA_TYPE_RAW_10BIT,
            HI_MIPI_WDR_MODE_NONE,
            {4, 5, 6, 7, -1, -1, -1, -1}
        }
    }
};

combo_dev_attr_t MIPI_4lane_CHN4_SENSOR_IMX477_10BIT_8M_NOWDR_ATTR =
{
    .devno = 4,
    .input_mode = INPUT_MODE_MIPI,
    .data_rate = MIPI_DATA_RATE_X1,
    .img_rect = {0, 0, 3840, 2160},

    {
        .mipi_attr =
        {
            DATA_TYPE_RAW_10BIT,
            HI_MIPI_WDR_MODE_NONE,
            {8, 9, 10, 11, -1, -1, -1, -1}
        }
    }
};

combo_dev_attr_t MIPI_4lane_CHN6_SENSOR_IMX477_10BIT_8M_NOWDR_ATTR =
{
    .devno = 6,
    .input_mode = INPUT_MODE_MIPI,
    .data_rate = MIPI_DATA_RATE_X1,
    .img_rect = {0, 0, 3840, 2160},

    {
        .mipi_attr =
        {
            DATA_TYPE_RAW_10BIT,
            HI_MIPI_WDR_MODE_NONE,
            {12, 13, 14, 15, -1, -1, -1, -1}
        }
    }
};

combo_dev_attr_t MIPI_4lane_CHN0_SENSOR_IMX477_12BIT_8M_NOWDR_ATTR =
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
            {0, 1, 2, 3, -1, -1, -1, -1}
        }
    }
};

combo_dev_attr_t MIPI_4lane_CHN2_SENSOR_IMX477_12BIT_8M_NOWDR_ATTR =
{
    .devno = 2,
    .input_mode = INPUT_MODE_MIPI,
    .data_rate = MIPI_DATA_RATE_X1,
    .img_rect = {0, 0, 3840, 2160},

    {
        .mipi_attr =
        {
            DATA_TYPE_RAW_12BIT,
            HI_MIPI_WDR_MODE_NONE,
            {4, 5, 6, 7, -1, -1, -1, -1}
        }
    }
};

combo_dev_attr_t MIPI_4lane_CHN4_SENSOR_IMX477_12BIT_8M_NOWDR_ATTR =
{
    .devno = 4,
    .input_mode = INPUT_MODE_MIPI,
    .data_rate = MIPI_DATA_RATE_X1,
    .img_rect = {0, 0, 3840, 2160},

    {
        .mipi_attr =
        {
            DATA_TYPE_RAW_12BIT,
            HI_MIPI_WDR_MODE_NONE,
            {8, 9, 10, 11, -1, -1, -1, -1}
        }
    }
};

combo_dev_attr_t MIPI_4lane_CHN6_SENSOR_IMX477_12BIT_8M_NOWDR_ATTR =
{
    .devno = 6,
    .input_mode = INPUT_MODE_MIPI,
    .data_rate = MIPI_DATA_RATE_X1,
    .img_rect = {0, 0, 3840, 2160},

    {
        .mipi_attr =
        {
            DATA_TYPE_RAW_12BIT,
            HI_MIPI_WDR_MODE_NONE,
            {12, 13, 14, 15, -1, -1, -1, -1}
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
            {0, 1, 2, 3, -1, -1, -1, -1}
        }
    }
};
combo_dev_attr_t MIPI_4lane_CHN4_SENSOR_IMX290_12BIT_2M_NOWDR_ATTR =
{
    .devno = 4,
    .input_mode = INPUT_MODE_MIPI,
    .data_rate = MIPI_DATA_RATE_X1,
    .img_rect = {0, 0, 1920, 1080},

    {
        .mipi_attr =
        {
            DATA_TYPE_RAW_12BIT,
            HI_MIPI_WDR_MODE_NONE,
            {8, 9, 10, 11, -1, -1, -1, -1}
        }
    }
};

combo_dev_attr_t MIPI_4lane_CHN1_SENSOR_IMX290_12BIT_2M_NOWDR_ATTR =
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
            {4, 5, 6, 7, -1, -1, -1, -1}
        }
    }
};

combo_dev_attr_t MIPI_4lane_CHN0_SENSOR_IMX290_10BIT_2M_WDR3to1_ATTR =
{
    .devno = 0,
    .input_mode = INPUT_MODE_MIPI,
    .data_rate = MIPI_DATA_RATE_X1,
    .img_rect = {0, 0, 1920, 1080},

    {
        .mipi_attr =
        {
            DATA_TYPE_RAW_10BIT,
            HI_MIPI_WDR_MODE_DOL,
            {0, 1, 2, 3, -1, -1, -1, -1}
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
            {0, 1, 2, 3, -1, -1, -1, -1}
        }
    }
};

combo_dev_attr_t MIPI_4lane_CHN2_SENSOR_IMX334_12BIT_8M_NOWDR_ATTR =
{
    .devno = 2,
    .input_mode = INPUT_MODE_MIPI,
    .data_rate = MIPI_DATA_RATE_X1,
    .img_rect = {0, 0, 3840, 2160},

    {
        .mipi_attr =
        {
            DATA_TYPE_RAW_12BIT,
            HI_MIPI_WDR_MODE_NONE,
            {4, 5, 6, 7, -1, -1, -1, -1}
        }
    }
};

combo_dev_attr_t MIPI_4lane_CHN4_SENSOR_IMX334_12BIT_8M_NOWDR_ATTR =
{
    .devno = 4,
    .input_mode = INPUT_MODE_MIPI,
    .data_rate = MIPI_DATA_RATE_X1,
    .img_rect = {0, 0, 3840, 2160},

    {
        .mipi_attr =
        {
            DATA_TYPE_RAW_12BIT,
            HI_MIPI_WDR_MODE_NONE,
            {8, 9, 10, 11, -1, -1, -1, -1}
        }
    }
};

combo_dev_attr_t MIPI_4lane_CHN6_SENSOR_IMX334_12BIT_8M_NOWDR_ATTR =
{
    .devno = 6,
    .input_mode = INPUT_MODE_MIPI,
    .data_rate = MIPI_DATA_RATE_X1,
    .img_rect = {0, 0, 3840, 2160},

    {
        .mipi_attr =
        {
            DATA_TYPE_RAW_12BIT,
            HI_MIPI_WDR_MODE_NONE,
            {12, 13, 14, 15, -1, -1, -1, -1}
        }
    }
};

combo_dev_attr_t LVDS_16lane_CHN0_SENSOR_SHARP8K_12BIT_8K_NOWDR_ATTR =
{
    .devno                  = 0,
    .input_mode             = INPUT_MODE_LVDS_EX,
    .data_rate              = MIPI_DATA_RATE_X2,
    .img_rect               = {64, 12, 7680, 4320},
    .lvds_attr_ex           =
    {
        .clk_mode           = MIPI_CLK_MODE_SEPARATE,
        .lane_work_attr     =
        {
            .lane_work_mode     = LVDS_LANE_WORK_MODE_ASYNC,
            .sensor_valid_width = 7752,
        },
        .lvds_attr          =
        {
            .input_data_type    = DATA_TYPE_RAW_12BIT,
            .wdr_mode           = HI_WDR_MODE_NONE,
            .sync_mode          = LVDS_SYNC_MODE_SAV,
            .vsync_attr         = {LVDS_VSYNC_NORMAL, 0, 0},
            .fid_attr           = {LVDS_FID_NONE, HI_TRUE},
            .data_endian        = LVDS_ENDIAN_BIG,
            .sync_code_endian   = LVDS_ENDIAN_BIG,
            .lane_id            = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15},
            .sync_code          =
            {
                /* each vc has 4 params, sync_code[i]:
                   sync_mode is SYNC_MODE_SOF: SOF, EOF, SOL, EOL
                   sync_mode is SYNC_MODE_SAV: invalid sav, invalid eav, valid sav, valid eav  */
                {   {0xab0, 0xb60, 0x800, 0x9d0},
                    {0xab0, 0xb60, 0x800, 0x9d0},
                    {0xab0, 0xb60, 0x800, 0x9d0},
                    {0xab0, 0xb60, 0x800, 0x9d0}
                },

                {   {0xab0, 0xb60, 0x800, 0x9d0},
                    {0xab0, 0xb60, 0x800, 0x9d0},
                    {0xab0, 0xb60, 0x800, 0x9d0},
                    {0xab0, 0xb60, 0x800, 0x9d0}
                },

                {   {0xab0, 0xb60, 0x800, 0x9d0},
                    {0xab0, 0xb60, 0x800, 0x9d0},
                    {0xab0, 0xb60, 0x800, 0x9d0},
                    {0xab0, 0xb60, 0x800, 0x9d0}
                },

                {   {0xab0, 0xb60, 0x800, 0x9d0},
                    {0xab0, 0xb60, 0x800, 0x9d0},
                    {0xab0, 0xb60, 0x800, 0x9d0},
                    {0xab0, 0xb60, 0x800, 0x9d0}
                },

                {   {0xab0, 0xb60, 0x800, 0x9d0},
                    {0xab0, 0xb60, 0x800, 0x9d0},
                    {0xab0, 0xb60, 0x800, 0x9d0},
                    {0xab0, 0xb60, 0x800, 0x9d0}
                },

                {   {0xab0, 0xb60, 0x800, 0x9d0},
                    {0xab0, 0xb60, 0x800, 0x9d0},
                    {0xab0, 0xb60, 0x800, 0x9d0},
                    {0xab0, 0xb60, 0x800, 0x9d0}
                },

                {   {0xab0, 0xb60, 0x800, 0x9d0},
                    {0xab0, 0xb60, 0x800, 0x9d0},
                    {0xab0, 0xb60, 0x800, 0x9d0},
                    {0xab0, 0xb60, 0x800, 0x9d0}
                },

                {   {0xab0, 0xb60, 0x800, 0x9d0},
                    {0xab0, 0xb60, 0x800, 0x9d0},
                    {0xab0, 0xb60, 0x800, 0x9d0},
                    {0xab0, 0xb60, 0x800, 0x9d0}
                },

                {   {0xab0, 0xb60, 0x800, 0x9d0},
                    {0xab0, 0xb60, 0x800, 0x9d0},
                    {0xab0, 0xb60, 0x800, 0x9d0},
                    {0xab0, 0xb60, 0x800, 0x9d0}
                },

                {   {0xab0, 0xb60, 0x800, 0x9d0},
                    {0xab0, 0xb60, 0x800, 0x9d0},
                    {0xab0, 0xb60, 0x800, 0x9d0},
                    {0xab0, 0xb60, 0x800, 0x9d0}
                },

                {   {0xab0, 0xb60, 0x800, 0x9d0},
                    {0xab0, 0xb60, 0x800, 0x9d0},
                    {0xab0, 0xb60, 0x800, 0x9d0},
                    {0xab0, 0xb60, 0x800, 0x9d0}
                },

                {   {0xab0, 0xb60, 0x800, 0x9d0},
                    {0xab0, 0xb60, 0x800, 0x9d0},
                    {0xab0, 0xb60, 0x800, 0x9d0},
                    {0xab0, 0xb60, 0x800, 0x9d0}
                },

                {   {0xab0, 0xb60, 0x800, 0x9d0},
                    {0xab0, 0xb60, 0x800, 0x9d0},
                    {0xab0, 0xb60, 0x800, 0x9d0},
                    {0xab0, 0xb60, 0x800, 0x9d0}
                },

                {   {0xab0, 0xb60, 0x800, 0x9d0},
                    {0xab0, 0xb60, 0x800, 0x9d0},
                    {0xab0, 0xb60, 0x800, 0x9d0},
                    {0xab0, 0xb60, 0x800, 0x9d0}
                },

                {   {0xab0, 0xb60, 0x800, 0x9d0},
                    {0xab0, 0xb60, 0x800, 0x9d0},
                    {0xab0, 0xb60, 0x800, 0x9d0},
                    {0xab0, 0xb60, 0x800, 0x9d0}
                },

                {   {0xab0, 0xb60, 0x800, 0x9d0},
                    {0xab0, 0xb60, 0x800, 0x9d0},
                    {0xab0, 0xb60, 0x800, 0x9d0},
                    {0xab0, 0xb60, 0x800, 0x9d0}
                },
            }
        }
    }
};


combo_dev_attr_t MIPI_4lane_CHN0_SENSOR_IMX334_12BIT_8M_WDR2to1_ATTR =
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
            {0, 1, 2, 3, -1, -1, -1, -1}
        }
    }
};

combo_dev_attr_t MIPI_4lane_CHN2_SENSOR_IMX334_12BIT_8M_WDR2to1_ATTR =
{
    .devno = 2,
    .input_mode = INPUT_MODE_MIPI,
    .data_rate = MIPI_DATA_RATE_X1,
    .img_rect = {0, 0, 3840, 2160},

    {
        .mipi_attr =
        {
            DATA_TYPE_RAW_12BIT,
            HI_MIPI_WDR_MODE_NONE,
            {4, 5, 6, 7, -1, -1, -1, -1}
        }
    }
};



combo_dev_attr_t MIPI_4lane_CHN4_SENSOR_IMX334_12BIT_8M_WDR2to1_ATTR =
{
    .devno = 4,
    .input_mode = INPUT_MODE_MIPI,
    .data_rate = MIPI_DATA_RATE_X1,
    .img_rect = {0, 0, 3840, 2160},

    {
        .mipi_attr =
        {
            DATA_TYPE_RAW_12BIT,
            HI_MIPI_WDR_MODE_NONE,
            {8, 9, 10, 11, -1, -1, -1, -1}
        }
    }
};

combo_dev_attr_t MIPI_4lane_CHN6_SENSOR_IMX334_12BIT_8M_WDR2to1_ATTR =
{
    .devno = 6,
    .input_mode = INPUT_MODE_MIPI,
    .data_rate = MIPI_DATA_RATE_X1,
    .img_rect = {0, 0, 3840, 2160},

    {
        .mipi_attr =
        {
            DATA_TYPE_RAW_12BIT,
            HI_MIPI_WDR_MODE_NONE,
            {12, 13, 14, 15, -1, -1, -1, -1}
        }
    }
};




combo_dev_attr_t SLVS_8lane_CHN0_SENSOR_IMX277_10BIT_8M_NOWDR_ATTR =
{
    .devno = 6,
    .input_mode = INPUT_MODE_SLVS,
    .data_rate = MIPI_DATA_RATE_X2,
    .img_rect = {164, 24, 3840, 2160},

    {
        .slvs_attr =
        {
            DATA_TYPE_RAW_10BIT,
            HI_WDR_MODE_NONE,
            SLVS_LANE_RATE_HIGH,
            4200,
            {6,4, 5, 0, 7, 2, 1,3},
            SLVS_ERR_CHECK_MODE_NONE
        }
    }
};

combo_dev_attr_t SLVS_6lane_CHN0_SENSOR_IMX277_12BIT_8M_NOWDR_ATTR =
{
    .devno = 6,
    .input_mode = INPUT_MODE_SLVS,
    .data_rate = MIPI_DATA_RATE_X2,
    .img_rect = {48, 24, 3840, 2160},

    {
        .slvs_attr =
        {
            DATA_TYPE_RAW_12BIT,
            HI_WDR_MODE_NONE,
            SLVS_LANE_RATE_HIGH,
            3936,
            {6,4, 5, 0, 7, 2, -1,-1},
            SLVS_ERR_CHECK_MODE_NONE
        }
    }
};

combo_dev_attr_t SLVS_6lane_CHN0_SENSOR_IMX277_12BIT_12M_NOWDR_ATTR =
{
    .devno = 6,
    .input_mode = INPUT_MODE_SLVS,
    .data_rate = MIPI_DATA_RATE_X2,
    .img_rect = {54, 38, 4000, 3000},

    {
        .slvs_attr =
        {
            DATA_TYPE_RAW_12BIT,
            HI_WDR_MODE_NONE,
            SLVS_LANE_RATE_HIGH,
            4104,
            {6,4, 5, 0, 7, 2, -1,-1},
            SLVS_ERR_CHECK_MODE_NONE
        }
    }
};

combo_dev_attr_t SLVS_6lane_CHN0_SENSOR_IMX277_12BIT_2M_NOWDR_ATTR =
{
    .devno = 6,
    .input_mode = INPUT_MODE_SLVS,
    .data_rate = MIPI_DATA_RATE_X2,
    .img_rect = {24, 18, 1920, 1080},

    {
        .slvs_attr =
        {
            DATA_TYPE_RAW_12BIT,
            HI_WDR_MODE_NONE,
            SLVS_LANE_RATE_HIGH,
            1968,
            {6,4, 5, 0, 7, 2, -1,-1},
            SLVS_ERR_CHECK_MODE_NONE
        }
    }
};

VI_DEV_ATTR_S DEV_ATTR_IMX477_12M_BASE =
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

VI_DEV_ATTR_S DEV_ATTR_IMX477_9M_BASE =
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
    {3000, 3000},
    {
        {
            {3000 , 3000},
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

VI_DEV_ATTR_S DEV_ATTR_IMX477_8M_BASE =
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
    {3840, 2160},
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


VI_DEV_ATTR_S DEV_ATTR_IMX334_8M_BASE =
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

VI_DEV_ATTR_S DEV_ATTR_SHARP8K_SLVDS_8K_BASE =
{
    VI_MODE_LVDS,
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
    {7680 , 4320},
    {
        {
            {7680 , 4320},
        },
        {
            VI_REPHASE_MODE_NONE,
            VI_REPHASE_MODE_NONE
        }
    },
    {
        WDR_MODE_NONE,
        4320
    },
    DATA_RATE_X2
};


VI_DEV_ATTR_S DEV_ATTR_IMX334_8M_WDR2TO1_BASE =
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

VI_DEV_ATTR_S DEV_ATTR_IMX277_SLVS_8M_BASE =
{
    VI_MODE_SLVS,
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
    DATA_RATE_X2
};

VI_DEV_ATTR_S DEV_ATTR_IMX277_SLVS_12M_BASE =
{
    VI_MODE_SLVS,
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
    DATA_RATE_X2
};

VI_DEV_ATTR_S DEV_ATTR_IMX277_SLVS_2M_BASE =
{
    VI_MODE_SLVS,
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
    DATA_RATE_X2
};

VI_PIPE_ATTR_S PIPE_ATTR_1920x1080_RAW12_420_3DNR_RFR =
{
    VI_PIPE_BYPASS_NONE, HI_FALSE,HI_FALSE,
    1920, 1080,
    PIXEL_FORMAT_RGB_BAYER_12BPP,
    COMPRESS_MODE_LINE,
    DATA_BITWIDTH_12,
    HI_TRUE,
    {
        PIXEL_FORMAT_YVU_SEMIPLANAR_420,
        DATA_BITWIDTH_10,
        VI_NR_REF_FROM_RFR,
        COMPRESS_MODE_NONE
    },
    HI_FALSE,
    { -1, -1}
};

VI_PIPE_ATTR_S PIPE_ATTR_3840x2160_RAW12_420_3DNR_RFR =
{
    VI_PIPE_BYPASS_NONE, HI_FALSE,HI_FALSE,
    3840, 2160,
    PIXEL_FORMAT_RGB_BAYER_12BPP,
    COMPRESS_MODE_LINE,
    DATA_BITWIDTH_12,
    HI_TRUE,
    {
        PIXEL_FORMAT_YVU_SEMIPLANAR_420,
        DATA_BITWIDTH_10,
        VI_NR_REF_FROM_RFR,
        COMPRESS_MODE_NONE
    },
    HI_FALSE,
    { -1, -1}
};

static VI_PIPE_ATTR_S PIPE_ATTR_7680x4320_RAW12_420_3DNR_RFR =
{
    VI_PIPE_BYPASS_NONE, HI_FALSE, HI_FALSE,
    7680, 4320,
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


VI_PIPE_ATTR_S PIPE_ATTR_3840x2160_RAW10_420_3DNR_RFR =
{
    VI_PIPE_BYPASS_NONE, HI_FALSE,HI_FALSE,
    3840, 2160,
    PIXEL_FORMAT_RGB_BAYER_12BPP,
    COMPRESS_MODE_LINE,
    DATA_BITWIDTH_10,
    HI_TRUE,
    {
        PIXEL_FORMAT_YVU_SEMIPLANAR_420,
        DATA_BITWIDTH_10,
        VI_NR_REF_FROM_RFR,
        COMPRESS_MODE_NONE
    },
    HI_FALSE,
    {-1, -1}
};

VI_PIPE_ATTR_S PIPE_ATTR_3000x3000_RAW10_420_3DNR_RFR =
{
    VI_PIPE_BYPASS_NONE, HI_FALSE,HI_FALSE,
    3000, 3000,
    PIXEL_FORMAT_RGB_BAYER_10BPP,
    COMPRESS_MODE_LINE,
    DATA_BITWIDTH_10,
    HI_TRUE,
    {
        PIXEL_FORMAT_YVU_SEMIPLANAR_420,
        DATA_BITWIDTH_10,
        VI_NR_REF_FROM_RFR,
        COMPRESS_MODE_NONE
    },
    HI_FALSE,
    {-1, -1}
};


VI_PIPE_ATTR_S PIPE_ATTR_4000x3000_RAW12_420_3DNR_RFR =
{
    VI_PIPE_BYPASS_NONE, HI_FALSE,HI_FALSE,
    4000, 3000,
    PIXEL_FORMAT_RGB_BAYER_12BPP,
    COMPRESS_MODE_LINE,
    DATA_BITWIDTH_12,
    HI_TRUE,
    {
        PIXEL_FORMAT_YVU_SEMIPLANAR_420,
        DATA_BITWIDTH_10,
        VI_NR_REF_FROM_RFR,
        COMPRESS_MODE_NONE
    },
    HI_FALSE,
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

VI_CHN_ATTR_S CHN_ATTR_3000x3000_420_SDR8_LINEAR =
{
    {3000, 3000},
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

static VI_CHN_ATTR_S CHN_ATTR_7680x4320_420_SDR8_LINEAR =
{
    {7680, 4320},
    PIXEL_FORMAT_YVU_SEMIPLANAR_420,
    DYNAMIC_RANGE_SDR8,
    VIDEO_FORMAT_LINEAR,
    COMPRESS_MODE_NONE,
    0,      0,
    0,
    { -1, -1}
};


VI_CHN_ATTR_S CHN_ATTR_4000x3000_420_SDR8_LINEAR =
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

static input_mode_t SAMPLE_COMM_VI_GetSnsInputMode(SAMPLE_SNS_TYPE_E enSnsType)
{
    input_mode_t enInputMode;

    switch (enSnsType)
    {
        case SONY_IMX277_SLVS_8M_120FPS_10BIT:  /*imx277 test*/
        case SONY_IMX277_SLVS_8M_30FPS_12BIT:
        case SONY_IMX277_SLVS_8M_60FPS_12BIT:
        case SONY_IMX277_SLVS_12M_30FPS_12BIT:
        case SONY_IMX277_SLVS_2M_240FPS_12BIT:
            enInputMode = INPUT_MODE_SLVS;
            break;

        default:
            enInputMode = INPUT_MODE_MIPI;
            break;
    }

    return enInputMode;
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
    input_mode_t        enInputMode;
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

        enInputMode = SAMPLE_COMM_VI_GetSnsInputMode(pstViInfo->stSnsInfo.enSnsType);

        if (INPUT_MODE_SLVS == enInputMode)
        {
            s32Ret = ioctl(fd, HI_MIPI_ENABLE_SLVS_CLOCK, &devno);
        }
        else
        {
            s32Ret = ioctl(fd, HI_MIPI_ENABLE_MIPI_CLOCK, &devno);
        }

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
    input_mode_t        enInputMode;
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

        enInputMode = SAMPLE_COMM_VI_GetSnsInputMode(pstViInfo->stSnsInfo.enSnsType);

        if (INPUT_MODE_SLVS == enInputMode)
        {
            s32Ret = ioctl(fd, HI_MIPI_DISABLE_SLVS_CLOCK, &devno);
        }
        else
        {
            s32Ret = ioctl(fd, HI_MIPI_DISABLE_MIPI_CLOCK, &devno);
        }

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

HI_S32 SAMPLE_COMM_VI_ClearMipi(SAMPLE_VI_CONFIG_S* pstViConfig)
{
    HI_S32              i = 0;
    HI_S32              s32ViNum = 0;
    HI_S32              s32Ret = HI_SUCCESS;
    HI_S32              fd;
    combo_dev_t         devno = 0;
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

        s32Ret = ioctl(fd, HI_MIPI_CLEAR, &devno);

        if (HI_SUCCESS != s32Ret)
        {
            SAMPLE_PRT("MIPI_CLEAR %d failed\n", devno);
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
    input_mode_t        enInputMode;
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

        enInputMode = SAMPLE_COMM_VI_GetSnsInputMode(pstViInfo->stSnsInfo.enSnsType);

        if (INPUT_MODE_SLVS == enInputMode)
        {
            s32Ret = ioctl(fd, HI_MIPI_RESET_SLVS, &devno);
        }
        else
        {
            s32Ret = ioctl(fd, HI_MIPI_RESET_MIPI, &devno);
        }

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
    input_mode_t        enInputMode;
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

        enInputMode = SAMPLE_COMM_VI_GetSnsInputMode(pstViInfo->stSnsInfo.enSnsType);

        if (INPUT_MODE_SLVS == enInputMode)
        {
            s32Ret = ioctl(fd, HI_MIPI_UNRESET_SLVS, &devno);
        }
        else
        {
            s32Ret = ioctl(fd, HI_MIPI_UNRESET_MIPI, &devno);
        }

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
        case SONY_IMX477_MIPI_12M_30FPS_12BIT:
            if (0 == MipiDev)
            {
                memcpy_s(pstComboAttr, sizeof(combo_dev_attr_t), &MIPI_4lane_CHN0_SENSOR_IMX477_12BIT_12M_NOWDR_ATTR, sizeof(combo_dev_attr_t));
            }
            else if (2 == MipiDev)
            {
                memcpy_s(pstComboAttr, sizeof(combo_dev_attr_t), &MIPI_4lane_CHN2_SENSOR_IMX477_12BIT_12M_NOWDR_ATTR, sizeof(combo_dev_attr_t));
            }
            else if (4 == MipiDev)
            {
                memcpy_s(pstComboAttr, sizeof(combo_dev_attr_t), &MIPI_4lane_CHN4_SENSOR_IMX477_12BIT_12M_NOWDR_ATTR, sizeof(combo_dev_attr_t));
            }
            else if (6 == MipiDev)
            {
                memcpy_s(pstComboAttr, sizeof(combo_dev_attr_t), &MIPI_4lane_CHN6_SENSOR_IMX477_12BIT_12M_NOWDR_ATTR, sizeof(combo_dev_attr_t));
            }
            else
            {
                SAMPLE_PRT("unsupported mipi dev :%d for SnsType :%d\n", MipiDev, enSnsType);
            }

            break;

            case SONY_IMX477_MIPI_9M_50FPS_10BIT:
            case SONY_IMX477_MIPI_9M_60FPS_10BIT:
            if (0 == MipiDev)
            {
                memcpy_s(pstComboAttr, sizeof(combo_dev_attr_t), &MIPI_4lane_CHN0_SENSOR_IMX477_10BIT_9M_NOWDR_ATTR, sizeof(combo_dev_attr_t));
            }
            else if (2 == MipiDev)
            {
                memcpy_s(pstComboAttr, sizeof(combo_dev_attr_t), &MIPI_4lane_CHN2_SENSOR_IMX477_10BIT_9M_NOWDR_ATTR, sizeof(combo_dev_attr_t));
            }
            else if (4 == MipiDev)
            {
                memcpy_s(pstComboAttr, sizeof(combo_dev_attr_t), &MIPI_4lane_CHN4_SENSOR_IMX477_10BIT_9M_NOWDR_ATTR, sizeof(combo_dev_attr_t));
            }
            else if (6 == MipiDev)
            {
                memcpy_s(pstComboAttr, sizeof(combo_dev_attr_t), &MIPI_4lane_CHN6_SENSOR_IMX477_10BIT_9M_NOWDR_ATTR, sizeof(combo_dev_attr_t));
            }
            else
            {
                SAMPLE_PRT("unsupported mipi dev :%d for SnsType :%d\n", MipiDev, enSnsType);
            }

            break;

        case SONY_IMX477_MIPI_8M_60FPS_12BIT:
            if (0 == MipiDev)
            {
                memcpy_s(pstComboAttr, sizeof(combo_dev_attr_t), &MIPI_4lane_CHN0_SENSOR_IMX477_10BIT_8M_NOWDR_ATTR, sizeof(combo_dev_attr_t));
            }
            else if (2 == MipiDev)
            {
                memcpy_s(pstComboAttr, sizeof(combo_dev_attr_t), &MIPI_4lane_CHN2_SENSOR_IMX477_10BIT_8M_NOWDR_ATTR, sizeof(combo_dev_attr_t));
            }
            else if (4 == MipiDev)
            {
                memcpy_s(pstComboAttr, sizeof(combo_dev_attr_t), &MIPI_4lane_CHN4_SENSOR_IMX477_10BIT_8M_NOWDR_ATTR, sizeof(combo_dev_attr_t));
            }
            else if (6 == MipiDev)
            {
                memcpy_s(pstComboAttr, sizeof(combo_dev_attr_t), &MIPI_4lane_CHN6_SENSOR_IMX477_10BIT_8M_NOWDR_ATTR, sizeof(combo_dev_attr_t));
            }
            else
            {
                SAMPLE_PRT("unsupported mipi dev :%d for SnsType :%d\n",MipiDev, enSnsType);
            }

            break;

        case SONY_IMX477_MIPI_8M_30FPS_12BIT:
            if (0 == MipiDev)
            {
                memcpy_s(pstComboAttr, sizeof(combo_dev_attr_t), &MIPI_4lane_CHN0_SENSOR_IMX477_12BIT_8M_NOWDR_ATTR, sizeof(combo_dev_attr_t));
            }
            else if (2 == MipiDev)
            {
                memcpy_s(pstComboAttr, sizeof(combo_dev_attr_t), &MIPI_4lane_CHN2_SENSOR_IMX477_12BIT_8M_NOWDR_ATTR, sizeof(combo_dev_attr_t));
            }
            else if (4 == MipiDev)
            {
                memcpy_s(pstComboAttr, sizeof(combo_dev_attr_t), &MIPI_4lane_CHN4_SENSOR_IMX477_12BIT_8M_NOWDR_ATTR, sizeof(combo_dev_attr_t));
            }
            else if (6 == MipiDev)
            {
                memcpy_s(pstComboAttr, sizeof(combo_dev_attr_t), &MIPI_4lane_CHN6_SENSOR_IMX477_12BIT_8M_NOWDR_ATTR, sizeof(combo_dev_attr_t));
            }
            else
            {
                SAMPLE_PRT("unsupported mipi dev :%d for SnsType :%d\n",MipiDev, enSnsType);
            }

            break;

        case SONY_IMX290_MIPI_2M_30FPS_12BIT:
            if (0 == MipiDev)
            {
                memcpy_s(pstComboAttr, sizeof(combo_dev_attr_t), &MIPI_4lane_CHN0_SENSOR_IMX290_12BIT_2M_NOWDR_ATTR, sizeof(combo_dev_attr_t));
            }
            else if (1 == MipiDev)
            {
                memcpy_s(pstComboAttr, sizeof(combo_dev_attr_t), &MIPI_4lane_CHN1_SENSOR_IMX290_12BIT_2M_NOWDR_ATTR, sizeof(combo_dev_attr_t));
            }
            else if (4 == MipiDev)
            {
                memcpy_s(pstComboAttr, sizeof(combo_dev_attr_t), &MIPI_4lane_CHN4_SENSOR_IMX290_12BIT_2M_NOWDR_ATTR, sizeof(combo_dev_attr_t));
            }
            else
            {
                SAMPLE_PRT("unsupported mipi dev :%d for SnsType :%d\n",MipiDev, enSnsType);
            }

            break;

        case SONY_IMX290_MIPI_2M_30FPS_12BIT_WDR3TO1:
            memcpy_s(pstComboAttr, sizeof(combo_dev_attr_t), &MIPI_4lane_CHN0_SENSOR_IMX290_10BIT_2M_WDR3to1_ATTR, sizeof(combo_dev_attr_t));
            break;

	case SONY_IMX334_SLAVE_MIPI_8M_30FPS_12BIT:
	case SONY_IMX334_MIPI_8M_30FPS_12BIT:
	        if (0 == MipiDev)
            {
                memcpy_s(pstComboAttr, sizeof(combo_dev_attr_t), &MIPI_4lane_CHN0_SENSOR_IMX334_12BIT_8M_NOWDR_ATTR, sizeof(combo_dev_attr_t));
            }
            else if (2 == MipiDev)
            {
                memcpy_s(pstComboAttr, sizeof(combo_dev_attr_t), &MIPI_4lane_CHN2_SENSOR_IMX334_12BIT_8M_NOWDR_ATTR, sizeof(combo_dev_attr_t));
            }
            else if (4 == MipiDev)
            {
                memcpy_s(pstComboAttr, sizeof(combo_dev_attr_t), &MIPI_4lane_CHN4_SENSOR_IMX334_12BIT_8M_NOWDR_ATTR, sizeof(combo_dev_attr_t));
            }
            else if (6 == MipiDev)
            {
                memcpy_s(pstComboAttr, sizeof(combo_dev_attr_t), &MIPI_4lane_CHN6_SENSOR_IMX334_12BIT_8M_NOWDR_ATTR, sizeof(combo_dev_attr_t));
            }
            else
            {
                SAMPLE_PRT("unsupported mipi dev :%d for SnsType :%d\n", MipiDev, enSnsType);
            }

            break;

        case COMSIS_SHARP8K_SLVDS_8K_30FPS_12BIT:
            memcpy_s(pstComboAttr, sizeof(combo_dev_attr_t),&LVDS_16lane_CHN0_SENSOR_SHARP8K_12BIT_8K_NOWDR_ATTR, sizeof(combo_dev_attr_t));
            break;

	case SONY_IMX334_MIPI_8M_30FPS_12BIT_WDR2TO1:
            if (0 == MipiDev)
            {
                memcpy_s(pstComboAttr, sizeof(combo_dev_attr_t), &MIPI_4lane_CHN0_SENSOR_IMX334_12BIT_8M_WDR2to1_ATTR, sizeof(combo_dev_attr_t));
            }
            else if (2 == MipiDev)
            {
                memcpy_s(pstComboAttr, sizeof(combo_dev_attr_t), &MIPI_4lane_CHN2_SENSOR_IMX334_12BIT_8M_WDR2to1_ATTR, sizeof(combo_dev_attr_t));
            }
            else if (4 == MipiDev)
            {
                memcpy_s(pstComboAttr, sizeof(combo_dev_attr_t), &MIPI_4lane_CHN4_SENSOR_IMX334_12BIT_8M_WDR2to1_ATTR, sizeof(combo_dev_attr_t));
            }
            else if (6 == MipiDev)
            {
                memcpy_s(pstComboAttr, sizeof(combo_dev_attr_t), &MIPI_4lane_CHN6_SENSOR_IMX334_12BIT_8M_WDR2to1_ATTR, sizeof(combo_dev_attr_t));
            }
            else
            {
                SAMPLE_PRT("unsupported mipi dev :%d for SnsType :%d\n", MipiDev, enSnsType);
            }
            break;

        case SONY_IMX277_SLVS_8M_120FPS_10BIT:
            memcpy_s(pstComboAttr, sizeof(combo_dev_attr_t), &SLVS_8lane_CHN0_SENSOR_IMX277_10BIT_8M_NOWDR_ATTR, sizeof(combo_dev_attr_t));
            break;

        case SONY_IMX277_SLVS_8M_30FPS_12BIT:
        case SONY_IMX277_SLVS_8M_60FPS_12BIT:
            memcpy_s(pstComboAttr, sizeof(combo_dev_attr_t), &SLVS_6lane_CHN0_SENSOR_IMX277_12BIT_8M_NOWDR_ATTR, sizeof(combo_dev_attr_t));
            break;

        case SONY_IMX277_SLVS_12M_30FPS_12BIT:
            memcpy_s(pstComboAttr, sizeof(combo_dev_attr_t), &SLVS_6lane_CHN0_SENSOR_IMX277_12BIT_12M_NOWDR_ATTR, sizeof(combo_dev_attr_t));
            break;

        case SONY_IMX277_SLVS_2M_240FPS_12BIT:
            memcpy_s(pstComboAttr, sizeof(combo_dev_attr_t), &SLVS_6lane_CHN0_SENSOR_IMX277_12BIT_2M_NOWDR_ATTR, sizeof(combo_dev_attr_t));
            break;
        default:
            SAMPLE_PRT("not support enSnsType: %d\n", enSnsType);
            memcpy_s(pstComboAttr, sizeof(combo_dev_attr_t), &MIPI_4lane_CHN0_SENSOR_IMX477_12BIT_12M_NOWDR_ATTR, sizeof(combo_dev_attr_t));
    }


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
        if(VI_PARALLEL_VPSS_OFFLINE == pstViInfo->stPipeInfo.enMastPipeMode || VI_PARALLEL_VPSS_PARALLEL == pstViInfo->stPipeInfo.enMastPipeMode)
        {
            stcomboDevAttr.data_rate = MIPI_DATA_RATE_X2;
        } else {
            stcomboDevAttr.data_rate = MIPI_DATA_RATE_X1;
        }

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

    if (COMSIS_SHARP8K_SLVDS_8K_30FPS_12BIT == pstViConfig->astViInfo[0].stSnsInfo.enSnsType)
    {
        lane_divide_mode = LANE_DIVIDE_MODE_0;
    }
    else
    {
        lane_divide_mode = LANE_DIVIDE_MODE_7;
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

    s32Ret = SAMPLE_COMM_VI_ClearMipi(pstViConfig);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("SAMPLE_COMM_VI_ClearMipi failed!\n");

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
        case SONY_IMX477_MIPI_12M_30FPS_12BIT:
            memcpy_s(pstViDevAttr, sizeof(VI_DEV_ATTR_S), &DEV_ATTR_IMX477_12M_BASE, sizeof(VI_DEV_ATTR_S));
            break;

        case SONY_IMX477_MIPI_9M_50FPS_10BIT:
            memcpy_s(pstViDevAttr, sizeof(VI_DEV_ATTR_S), &DEV_ATTR_IMX477_9M_BASE, sizeof(VI_DEV_ATTR_S));
            break;

        case SONY_IMX477_MIPI_9M_60FPS_10BIT:
            memcpy_s(pstViDevAttr, sizeof(VI_DEV_ATTR_S), &DEV_ATTR_IMX477_9M_BASE, sizeof(VI_DEV_ATTR_S));
            break;

        case SONY_IMX477_MIPI_8M_60FPS_12BIT:
            memcpy_s(pstViDevAttr, sizeof(VI_DEV_ATTR_S), &DEV_ATTR_IMX477_8M_BASE, sizeof(VI_DEV_ATTR_S));
            break;

        case SONY_IMX477_MIPI_8M_30FPS_12BIT:
            memcpy_s(pstViDevAttr, sizeof(VI_DEV_ATTR_S), &DEV_ATTR_IMX477_8M_BASE, sizeof(VI_DEV_ATTR_S));
            break;

        case SONY_IMX290_MIPI_2M_30FPS_12BIT:
            memcpy_s(pstViDevAttr, sizeof(VI_DEV_ATTR_S), &DEV_ATTR_IMX290_2M_BASE, sizeof(VI_DEV_ATTR_S));
            break;

        case SONY_IMX290_MIPI_2M_30FPS_12BIT_WDR3TO1:
            memcpy_s(pstViDevAttr, sizeof(VI_DEV_ATTR_S), &DEV_ATTR_IMX290_2M_BASE, sizeof(VI_DEV_ATTR_S));
            pstViDevAttr->au32ComponentMask[0] = 0xFFC00000;
            break;

        case SONY_IMX334_SLAVE_MIPI_8M_30FPS_12BIT:
        case SONY_IMX334_MIPI_8M_30FPS_12BIT:
            memcpy_s(pstViDevAttr, sizeof(VI_DEV_ATTR_S), &DEV_ATTR_IMX334_8M_BASE, sizeof(VI_DEV_ATTR_S));
            break;

        case COMSIS_SHARP8K_SLVDS_8K_30FPS_12BIT:
            memcpy_s(pstViDevAttr, sizeof(VI_DEV_ATTR_S), &DEV_ATTR_SHARP8K_SLVDS_8K_BASE, sizeof(VI_DEV_ATTR_S));
            break;

        case SONY_IMX334_MIPI_8M_30FPS_12BIT_WDR2TO1:
            memcpy_s(pstViDevAttr, sizeof(VI_DEV_ATTR_S), &DEV_ATTR_IMX334_8M_WDR2TO1_BASE, sizeof(VI_DEV_ATTR_S));
            break;

        case SONY_IMX277_SLVS_8M_120FPS_10BIT:
        case SONY_IMX277_SLVS_8M_30FPS_12BIT:
        case SONY_IMX277_SLVS_8M_60FPS_12BIT:
            memcpy_s(pstViDevAttr, sizeof(VI_DEV_ATTR_S), &DEV_ATTR_IMX277_SLVS_8M_BASE, sizeof(VI_DEV_ATTR_S));
            break;

        case SONY_IMX277_SLVS_12M_30FPS_12BIT:
            memcpy_s(pstViDevAttr, sizeof(VI_DEV_ATTR_S), &DEV_ATTR_IMX277_SLVS_12M_BASE, sizeof(VI_DEV_ATTR_S));
            break;

        case SONY_IMX277_SLVS_2M_240FPS_12BIT:
            memcpy_s(pstViDevAttr, sizeof(VI_DEV_ATTR_S), &DEV_ATTR_IMX277_SLVS_2M_BASE, sizeof(VI_DEV_ATTR_S));
            break;

        default:
            memcpy_s(pstViDevAttr, sizeof(VI_DEV_ATTR_S), &DEV_ATTR_IMX477_8M_BASE, sizeof(VI_DEV_ATTR_S));
    }

    return HI_SUCCESS;
}

HI_S32 SAMPLE_COMM_VI_GetPipeAttrBySns(SAMPLE_SNS_TYPE_E enSnsType, VI_PIPE_ATTR_S* pstPipeAttr)
{
    switch (enSnsType)
    {
        case SONY_IMX477_MIPI_12M_30FPS_12BIT:
            memcpy_s(pstPipeAttr, sizeof(VI_PIPE_ATTR_S), &PIPE_ATTR_4000x3000_RAW12_420_3DNR_RFR, sizeof(VI_PIPE_ATTR_S));
            break;

        case SONY_IMX477_MIPI_9M_50FPS_10BIT:
        case SONY_IMX477_MIPI_9M_60FPS_10BIT:
            memcpy_s(pstPipeAttr, sizeof(VI_PIPE_ATTR_S), &PIPE_ATTR_3000x3000_RAW10_420_3DNR_RFR, sizeof(VI_PIPE_ATTR_S));
            break;

        case SONY_IMX477_MIPI_8M_60FPS_12BIT:
            memcpy_s(pstPipeAttr, sizeof(VI_PIPE_ATTR_S), &PIPE_ATTR_3840x2160_RAW10_420_3DNR_RFR, sizeof(VI_PIPE_ATTR_S));
            break;

        case SONY_IMX477_MIPI_8M_30FPS_12BIT:
            memcpy_s(pstPipeAttr, sizeof(VI_PIPE_ATTR_S), &PIPE_ATTR_3840x2160_RAW12_420_3DNR_RFR, sizeof(VI_PIPE_ATTR_S));
            break;
        case SONY_IMX290_MIPI_2M_30FPS_12BIT:
            memcpy_s(pstPipeAttr, sizeof(VI_PIPE_ATTR_S), &PIPE_ATTR_1920x1080_RAW12_420_3DNR_RFR, sizeof(VI_PIPE_ATTR_S));
            break;

        case SONY_IMX290_MIPI_2M_30FPS_12BIT_WDR3TO1:
            memcpy_s(pstPipeAttr, sizeof(VI_PIPE_ATTR_S), &PIPE_ATTR_1920x1080_RAW12_420_3DNR_RFR, sizeof(VI_PIPE_ATTR_S));
            pstPipeAttr->enPixFmt = PIXEL_FORMAT_RGB_BAYER_10BPP;
            pstPipeAttr->enBitWidth = DATA_BITWIDTH_10;
            break;

        case SONY_IMX334_SLAVE_MIPI_8M_30FPS_12BIT:
        case SONY_IMX334_MIPI_8M_30FPS_12BIT:
            memcpy_s(pstPipeAttr, sizeof(VI_PIPE_ATTR_S), &PIPE_ATTR_3840x2160_RAW12_420_3DNR_RFR, sizeof(VI_PIPE_ATTR_S));
            break;

        case COMSIS_SHARP8K_SLVDS_8K_30FPS_12BIT:
            memcpy_s(pstPipeAttr, sizeof(VI_PIPE_ATTR_S),&PIPE_ATTR_7680x4320_RAW12_420_3DNR_RFR, sizeof(VI_PIPE_ATTR_S));
            break;

        case SONY_IMX334_MIPI_8M_30FPS_12BIT_WDR2TO1:
            memcpy_s(pstPipeAttr, sizeof(VI_PIPE_ATTR_S), &PIPE_ATTR_3840x2160_RAW12_420_3DNR_RFR, sizeof(VI_PIPE_ATTR_S));
            break;

        case SONY_IMX277_SLVS_8M_120FPS_10BIT:
            memcpy_s(pstPipeAttr, sizeof(VI_PIPE_ATTR_S), &PIPE_ATTR_3840x2160_RAW10_420_3DNR_RFR, sizeof(VI_PIPE_ATTR_S));
            break;

        case SONY_IMX277_SLVS_8M_30FPS_12BIT:
        case SONY_IMX277_SLVS_8M_60FPS_12BIT:
            memcpy_s(pstPipeAttr, sizeof(VI_PIPE_ATTR_S), &PIPE_ATTR_3840x2160_RAW12_420_3DNR_RFR, sizeof(VI_PIPE_ATTR_S));
            break;

        case SONY_IMX277_SLVS_12M_30FPS_12BIT:
            memcpy_s(pstPipeAttr, sizeof(VI_PIPE_ATTR_S), &PIPE_ATTR_4000x3000_RAW12_420_3DNR_RFR, sizeof(VI_PIPE_ATTR_S));
            break;

        case SONY_IMX277_SLVS_2M_240FPS_12BIT:
            memcpy_s(pstPipeAttr, sizeof(VI_PIPE_ATTR_S), &PIPE_ATTR_1920x1080_RAW12_420_3DNR_RFR, sizeof(VI_PIPE_ATTR_S));
            break;

        default:
            memcpy_s(pstPipeAttr, sizeof(VI_PIPE_ATTR_S), &PIPE_ATTR_3840x2160_RAW12_420_3DNR_RFR, sizeof(VI_PIPE_ATTR_S));
    }

    return HI_SUCCESS;
}

HI_S32 SAMPLE_COMM_VI_GetChnAttrBySns(SAMPLE_SNS_TYPE_E enSnsType, VI_CHN_ATTR_S* pstChnAttr)
{
    switch (enSnsType)
    {
        case SONY_IMX477_MIPI_12M_30FPS_12BIT:
            memcpy_s(pstChnAttr, sizeof(VI_CHN_ATTR_S), &CHN_ATTR_4000x3000_420_SDR8_LINEAR, sizeof(VI_CHN_ATTR_S));
            break;

        case SONY_IMX477_MIPI_9M_50FPS_10BIT:
        case SONY_IMX477_MIPI_9M_60FPS_10BIT:
            memcpy_s(pstChnAttr, sizeof(VI_CHN_ATTR_S), &CHN_ATTR_3000x3000_420_SDR8_LINEAR, sizeof(VI_CHN_ATTR_S));
            break;

        case SONY_IMX477_MIPI_8M_60FPS_12BIT:
            memcpy_s(pstChnAttr, sizeof(VI_CHN_ATTR_S), &CHN_ATTR_3840x2160_420_SDR8_LINEAR, sizeof(VI_CHN_ATTR_S));
            break;

        case SONY_IMX477_MIPI_8M_30FPS_12BIT:
            memcpy_s(pstChnAttr, sizeof(VI_CHN_ATTR_S), &CHN_ATTR_3840x2160_420_SDR8_LINEAR, sizeof(VI_CHN_ATTR_S));
            break;

        case SONY_IMX290_MIPI_2M_30FPS_12BIT:
            memcpy_s(pstChnAttr, sizeof(VI_CHN_ATTR_S), &CHN_ATTR_1920x1080_420_SDR8_LINEAR, sizeof(VI_CHN_ATTR_S));
            break;

        case SONY_IMX290_MIPI_2M_30FPS_12BIT_WDR3TO1:
            memcpy_s(pstChnAttr, sizeof(VI_CHN_ATTR_S), &CHN_ATTR_1920x1080_420_SDR8_LINEAR, sizeof(VI_CHN_ATTR_S));
            break;

        case SONY_IMX334_SLAVE_MIPI_8M_30FPS_12BIT:
        case SONY_IMX334_MIPI_8M_30FPS_12BIT:
            memcpy_s(pstChnAttr, sizeof(VI_CHN_ATTR_S), &CHN_ATTR_3840x2160_420_SDR8_LINEAR, sizeof(VI_CHN_ATTR_S));
            break;

        case COMSIS_SHARP8K_SLVDS_8K_30FPS_12BIT:
            memcpy_s(pstChnAttr,sizeof(VI_CHN_ATTR_S), &CHN_ATTR_7680x4320_420_SDR8_LINEAR, sizeof(VI_CHN_ATTR_S));
            break;

        case SONY_IMX334_MIPI_8M_30FPS_12BIT_WDR2TO1:
            memcpy_s(pstChnAttr, sizeof(VI_CHN_ATTR_S), &CHN_ATTR_3840x2160_420_SDR8_LINEAR, sizeof(VI_CHN_ATTR_S));
            break;

        case SONY_IMX277_SLVS_8M_120FPS_10BIT:
        case SONY_IMX277_SLVS_8M_30FPS_12BIT:
        case SONY_IMX277_SLVS_8M_60FPS_12BIT:
            memcpy_s(pstChnAttr, sizeof(VI_CHN_ATTR_S), &CHN_ATTR_3840x2160_420_SDR8_LINEAR, sizeof(VI_CHN_ATTR_S));
            break;

        case SONY_IMX277_SLVS_12M_30FPS_12BIT:
            memcpy_s(pstChnAttr, sizeof(VI_CHN_ATTR_S), &CHN_ATTR_4000x3000_420_SDR8_LINEAR, sizeof(VI_CHN_ATTR_S));
            break;

        case SONY_IMX277_SLVS_2M_240FPS_12BIT:
            memcpy_s(pstChnAttr, sizeof(VI_CHN_ATTR_S), &CHN_ATTR_1920x1080_420_SDR8_LINEAR, sizeof(VI_CHN_ATTR_S));
            break;

        default:
            memcpy_s(pstChnAttr, sizeof(VI_CHN_ATTR_S), &CHN_ATTR_3840x2160_420_SDR8_LINEAR, sizeof(VI_CHN_ATTR_S));
    }

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
    } else {
        stViDevAttr.enDataRate = DATA_RATE_X1;
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
        SAMPLE_PRT("HI_MPI_VI_SetDevBindPipe failed with %#x!\n", s32Ret);
        return HI_FAILURE;
    }

    return s32Ret;
}

static HI_S32 SAMPLE_COMM_VI_StartSingleViPipe(VI_PIPE ViPipe, VI_PIPE_ATTR_S* pstPipeAttr)
{
    HI_S32 s32Ret;

    s32Ret = HI_MPI_VI_CreatePipe(ViPipe, pstPipeAttr);

    if (s32Ret != HI_SUCCESS)
    {
        SAMPLE_PRT("HI_MPI_VI_CreatePipe failed with %#x!\n", s32Ret);
        return HI_FAILURE;
    }

    s32Ret = HI_MPI_VI_StartPipe(ViPipe);

    if (s32Ret != HI_SUCCESS)
    {
        HI_MPI_VI_DestroyPipe(ViPipe);
        SAMPLE_PRT("HI_MPI_VI_StartPipe failed with %#x!\n", s32Ret);
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

            if ((pstViInfo->stSnapInfo.bSnap) && (pstViInfo->stSnapInfo.bDoublePipe) && (ViPipe == pstViInfo->stSnapInfo.SnapPipe))
            {
                s32Ret = HI_MPI_VI_CreatePipe(ViPipe, &stPipeAttr);
                if (s32Ret != HI_SUCCESS)
                {
                    SAMPLE_PRT("HI_MPI_VI_CreatePipe failed with %#x!\n", s32Ret);
                    goto EXIT;
                }
                //do not start pipe
            }
            else
            {
                s32Ret = SAMPLE_COMM_VI_StartSingleViPipe(ViPipe, &stPipeAttr);
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

static HI_S32 SAMPLE_COMM_VI_CreateSingleVi(SAMPLE_VI_INFO_S* pstViInfo)
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



static HI_S32 SAMPLE_COMM_ModeSwitch_VI_CreateSingleVi(SAMPLE_VI_INFO_S* pstViInfo)
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




static HI_S32 SAMPLE_COMM_VI_DestroySingleVi(SAMPLE_VI_INFO_S* pstViInfo)
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

HI_S32 SAMPLE_COMM_VI_ReStartSns(SAMPLE_VI_INFO_S* pstViInfo)
{
    HI_S32 i;
    HI_S32 s32Ret = HI_SUCCESS;
    VI_PIPE ViPipe;
    HI_U32 u32SnsId;
    ISP_SNS_OBJ_S* pstSnsObj;

    for (i = 0; i < WDR_MAX_PIPE_NUM; i++)
    {
        if (pstViInfo->stPipeInfo.aPipe[i] >= 0  && pstViInfo->stPipeInfo.aPipe[i] < VI_MAX_PIPE_NUM)
        {
            ViPipe      = pstViInfo->stPipeInfo.aPipe[i];
            u32SnsId    = pstViInfo->stSnsInfo.s32SnsId;

            pstSnsObj = SAMPLE_COMM_ISP_GetSnsObj(u32SnsId);

            if (HI_NULL == pstSnsObj)
            {
                SAMPLE_PRT("sensor %d not exist!\n", u32SnsId);
                return HI_FAILURE;
            }

            if (pstSnsObj->pfnRegisterCallback != HI_NULL)
            {
                pstSnsObj->pfnRestart(ViPipe);
            }
            else
            {
                SAMPLE_PRT("sensor_register_callback failed with HI_NULL!!!\n");
            }

        }
    }

    return s32Ret;
}
/*Check if use standby mode or not, because IMX477 is not the real slave mode.
For IMX477, Vsync is generate by outside and Hsync is generate by sensor inside, both of them need to compliance with timing,
if use Sync mode for sync the all Pipe Vsync single, the IMX477 will output error raw data. So it need to entry standby mode first
before init next vipipe sensor, and after current Vipipe sensor init finish. And then restart all pipie sensor after all pipe sensor
are init finish.*/
HI_U32 SAMPLE_COMM_VI_UseStandByMode(SAMPLE_VI_INFO_S* pstViInfo)
{
    if ((SONY_IMX477_MIPI_12M_30FPS_12BIT == pstViInfo->stSnsInfo.enSnsType) ||
       (SONY_IMX477_MIPI_9M_50FPS_10BIT == pstViInfo->stSnsInfo.enSnsType) ||
       (SONY_IMX477_MIPI_9M_60FPS_10BIT == pstViInfo->stSnsInfo.enSnsType) ||
       (SONY_IMX477_MIPI_8M_60FPS_12BIT == pstViInfo->stSnsInfo.enSnsType) ||
       (SONY_IMX477_MIPI_8M_30FPS_12BIT == pstViInfo->stSnsInfo.enSnsType))
    {
        return HI_TRUE;
    }
    else
    {
        return HI_FALSE;
    }
}
HI_S32 SAMPLE_COMM_VI_StartIsp(SAMPLE_VI_INFO_S* pstViInfo)
{
    HI_S32              i;
    HI_BOOL             bNeedPipe;
    HI_S32              s32Ret = HI_SUCCESS;
    VI_PIPE             ViPipe;
    HI_U32              u32SnsId;
    ISP_PUB_ATTR_S      stPubAttr;
    ISP_SNS_OBJ_S* pstSnsObj;

    for (i = 0; i < WDR_MAX_PIPE_NUM; i++)
    {
        if (pstViInfo->stPipeInfo.aPipe[i] >= 0  && pstViInfo->stPipeInfo.aPipe[i] < VI_MAX_PIPE_NUM)
        {
            ViPipe      = pstViInfo->stPipeInfo.aPipe[i];
            u32SnsId    = pstViInfo->stSnsInfo.s32SnsId;

            SAMPLE_COMM_ISP_GetIspAttrBySns(pstViInfo->stSnsInfo.enSnsType, &stPubAttr);
            stPubAttr.enWDRMode = pstViInfo->stDevInfo.enWDRMode;
            
            SAMPLE_PRT(" => u32SnsId: %d ViPipe: %d, enWDRMode:%d\n", u32SnsId, ViPipe, stPubAttr.enWDRMode);
            
            /* //Because the pipe0 of 9M@50FPS sensor do flip in cmos.c, the bayer array changed to BGGR*/
            if ((SONY_IMX477_MIPI_9M_50FPS_10BIT == pstViInfo->stSnsInfo.enSnsType) && (0 == ViPipe))
            {
                stPubAttr.enBayer = BAYER_RGGB;
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

            if((pstViInfo->stSnapInfo.bDoublePipe) && (pstViInfo->stSnapInfo.SnapPipe == ViPipe))
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

            /*Check use standby mode or not*/
            if (SAMPLE_COMM_VI_UseStandByMode(pstViInfo) == HI_TRUE)
            {
                pstSnsObj = SAMPLE_COMM_ISP_GetSnsObj(u32SnsId);

                if (HI_NULL == pstSnsObj)
                {
                    SAMPLE_PRT("sensor %d not exist!\n", u32SnsId);
                    SAMPLE_COMM_ISP_Stop(ViPipe);
                    return HI_FAILURE;
                }

                if (pstSnsObj->pfnRegisterCallback != HI_NULL)
                {
                    pstSnsObj->pfnStandby(ViPipe);
                }
                else
                {
                    SAMPLE_PRT("sensor_register_callback failed with HI_NULL!!!\n");
                }
            }

            s32Ret = SAMPLE_COMM_ISP_Run(&pstViInfo->stPipeInfo.aPipe[i]);

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

    for (i = 0; i < pstViConfig->s32WorkingViNum; i++)
    {
        s32ViNum  = pstViConfig->as32WorkingViId[i];
        pstViInfo = &pstViConfig->astViInfo[s32ViNum];

        if (SAMPLE_COMM_VI_UseStandByMode(pstViInfo) == HI_TRUE)
        {
            s32Ret = SAMPLE_COMM_VI_ReStartSns(pstViInfo);

            if (s32Ret != HI_SUCCESS)
            {
                SAMPLE_PRT("SAMPLE_COMM_VI_ReStartSns failed !\n");
                return HI_FAILURE;
            }
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
    HI_U32         u32SnsId;
    ISP_SNS_OBJ_S *pstSnsObj;

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

                stPubAttr.enWDRMode =  pstViInfo->stDevInfo.enWDRMode ;

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
            u32SnsId  = pstViInfo->stSnsInfo.s32SnsId;

            HI_MPI_ISP_QueryInnerStateInfo(ViPipe, &stInnerStateInfo);
            if (bSwitchWDR[ViPipe] == HI_TRUE)
            {
                bSwitchFinish &= stInnerStateInfo.bWDRSwitchFinish;
            }
            else
            {
                bSwitchFinish &= stInnerStateInfo.bResSwitchFinish;
            }

            /*Check use standby mode or not*/
            if (SAMPLE_COMM_VI_UseStandByMode(pstViInfo) == HI_TRUE)
            {
                if ((stInnerStateInfo.bResSwitchFinish == HI_TRUE) ||
                    ((stInnerStateInfo.bWDRSwitchFinish == HI_TRUE) && (bSwitchWDR[ViPipe] == HI_TRUE)))
                {
                    pstSnsObj = SAMPLE_COMM_ISP_GetSnsObj(u32SnsId);

                    if (HI_NULL == pstSnsObj) {
                        SAMPLE_PRT("sensor %d not exist!\n", u32SnsId);
                        SAMPLE_COMM_ISP_Stop(ViPipe);
                        return HI_FAILURE;
                    }

                    if (pstSnsObj->pfnRegisterCallback != HI_NULL)
                    {
                        pstSnsObj->pfnStandby(ViPipe);
                    }
                    else
                    {
                        SAMPLE_PRT("sensor_register_callback failed with HI_NULL!!!\n");
                    }
                }
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

        if (SAMPLE_COMM_VI_UseStandByMode(pstViInfo) == HI_TRUE)
        {
            s32Ret = SAMPLE_COMM_VI_ReStartSns(pstViInfo);

            if (s32Ret != HI_SUCCESS)
            {
                SAMPLE_PRT("SAMPLE_COMM_VI_ReStartSns failed !\n");
                return HI_FAILURE;
            }
        }
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

   HI_S32              s32Ret = HI_SUCCESS;

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
        case SONY_IMX290_MIPI_2M_30FPS_12BIT_WDR3TO1:
            *penWDRMode = WDR_MODE_3To1_LINE;
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
        case SONY_IMX290_MIPI_2M_30FPS_12BIT_WDR3TO1:
            pstPipeInfo->enMastPipeMode = VI_ONLINE_VPSS_OFFLINE;
            pstPipeInfo->aPipe[0] = 0;
            pstPipeInfo->aPipe[1] = 1;
            pstPipeInfo->aPipe[2] = 2;
            pstPipeInfo->aPipe[3] = -1;
            break;

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
        case SONY_IMX477_MIPI_12M_30FPS_12BIT:
            *penSize = PIC_4000x3000;
            break;

        case SONY_IMX477_MIPI_9M_50FPS_10BIT:
        case SONY_IMX477_MIPI_9M_60FPS_10BIT:
            *penSize = PIC_3000x3000;
            break;

        case SONY_IMX477_MIPI_8M_60FPS_12BIT:
            *penSize = PIC_3840x2160;
            break;

        case SONY_IMX477_MIPI_8M_30FPS_12BIT:
            *penSize = PIC_3840x2160;
            break;

        case SONY_IMX290_MIPI_2M_30FPS_12BIT:
        case SONY_IMX290_MIPI_2M_30FPS_12BIT_WDR3TO1:
            *penSize = PIC_1080P;
            break;

        case SONY_IMX334_SLAVE_MIPI_8M_30FPS_12BIT:
        case SONY_IMX334_MIPI_8M_30FPS_12BIT:
        case SONY_IMX334_MIPI_8M_30FPS_12BIT_WDR2TO1:
            *penSize = PIC_3840x2160;
             break;

        case SONY_IMX277_SLVS_8M_120FPS_10BIT:
        case SONY_IMX277_SLVS_8M_30FPS_12BIT:
        case SONY_IMX277_SLVS_8M_60FPS_12BIT:
            *penSize = PIC_3840x2160;
            break;

        case SONY_IMX277_SLVS_12M_30FPS_12BIT:
            *penSize = PIC_4000x3000;
            break;

        case SONY_IMX277_SLVS_2M_240FPS_12BIT:
            *penSize = PIC_1080P;
            break;

        case COMSIS_SHARP8K_SLVDS_8K_30FPS_12BIT:
            *penSize = PIC_7680x4320;
            break;


        default:
            *penSize = PIC_3840x2160;
            break;
    }

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
        case SONY_IMX477_MIPI_12M_30FPS_12BIT:
        case SONY_IMX477_MIPI_8M_30FPS_12BIT:
        case SONY_IMX290_MIPI_2M_30FPS_12BIT:
        case SONY_IMX290_MIPI_2M_30FPS_12BIT_WDR3TO1:
        case SONY_IMX334_SLAVE_MIPI_8M_30FPS_12BIT:
        case SONY_IMX334_MIPI_8M_30FPS_12BIT:
        case SONY_IMX334_MIPI_8M_30FPS_12BIT_WDR2TO1:
        case SONY_IMX277_SLVS_8M_30FPS_12BIT:
        case SONY_IMX277_SLVS_12M_30FPS_12BIT:
            *pu32FrameRate = 30;
            break;

        case SONY_IMX477_MIPI_9M_50FPS_10BIT:

            *pu32FrameRate = 50;
            break;

        case SONY_IMX477_MIPI_9M_60FPS_10BIT:
        case SONY_IMX477_MIPI_8M_60FPS_12BIT:
        case SONY_IMX277_SLVS_8M_60FPS_12BIT:
            *pu32FrameRate = 60;
            break;

        case SONY_IMX277_SLVS_8M_120FPS_10BIT:
            *pu32FrameRate = 120;
            break;

        case SONY_IMX277_SLVS_2M_240FPS_12BIT:
            *pu32FrameRate = 240;
            break;

        case COMSIS_SHARP8K_SLVDS_8K_30FPS_12BIT:
            *pu32FrameRate = 30;
            break;

        default:
            *pu32FrameRate = 30;
            break;

    }

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
    }

    pstViConfig->astViInfo[0].stSnsInfo.enSnsType = SENSOR0_TYPE;

    pstViConfig->astViInfo[1].stSnsInfo.enSnsType = SENSOR1_TYPE;

    pstViConfig->astViInfo[2].stSnsInfo.enSnsType = SENSOR2_TYPE;

    pstViConfig->astViInfo[3].stSnsInfo.enSnsType = SENSOR3_TYPE;

    pstViConfig->astViInfo[4].stSnsInfo.enSnsType = SENSOR4_TYPE;

    pstViConfig->astViInfo[5].stSnsInfo.enSnsType = SENSOR5_TYPE;

    pstViConfig->astViInfo[6].stSnsInfo.enSnsType = SENSOR6_TYPE;

    pstViConfig->astViInfo[7].stSnsInfo.enSnsType = SENSOR7_TYPE;
}

combo_dev_t SAMPLE_COMM_VI_GetComboDevBySensor(SAMPLE_SNS_TYPE_E enMode, HI_S32 s32SnsIdx)
{
    combo_dev_t dev = 0;

    switch (enMode)
    {
        case SONY_IMX477_MIPI_12M_30FPS_12BIT:
        case SONY_IMX477_MIPI_9M_50FPS_10BIT:
        case SONY_IMX477_MIPI_9M_60FPS_10BIT:
        case SONY_IMX477_MIPI_8M_60FPS_12BIT:
        case SONY_IMX477_MIPI_8M_30FPS_12BIT:
        case SONY_IMX290_MIPI_2M_30FPS_12BIT:
        case SONY_IMX290_MIPI_2M_30FPS_12BIT_WDR3TO1:
        case SONY_IMX334_SLAVE_MIPI_8M_30FPS_12BIT:
        case SONY_IMX334_MIPI_8M_30FPS_12BIT:
        case SONY_IMX334_MIPI_8M_30FPS_12BIT_WDR2TO1:
            if(0 == s32SnsIdx)
            {
                dev= 0;
            }
            else if(1 == s32SnsIdx)
            {
                dev= 2;
            }
            else if(2 == s32SnsIdx)
            {
                dev= 4;
            }
            else if(3 == s32SnsIdx)
            {
                dev= 6;
            }
            break;

        case SONY_IMX277_SLVS_8M_120FPS_10BIT:
        case SONY_IMX277_SLVS_8M_30FPS_12BIT:
        case SONY_IMX277_SLVS_8M_60FPS_12BIT:
        case SONY_IMX277_SLVS_12M_30FPS_12BIT:
        case SONY_IMX277_SLVS_2M_240FPS_12BIT:
        case COMSIS_SHARP8K_SLVDS_8K_30FPS_12BIT:
            if(0 == s32SnsIdx)
            {
                dev= 0;
            }
            else if(1 == s32SnsIdx)
            {
                dev= 2; //8;
            }
            break;

        default:
            dev= 0;
            break;
    }

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

    snprintf(name, sizeof(name),"./data/%s_pipe%d_w%d_h%d_%dbits_%s_%d.raw",
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
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */
