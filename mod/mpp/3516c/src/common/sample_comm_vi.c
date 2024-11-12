/*
  Copyright (c), 2001-2024, Shenshu Tech. Co., Ltd.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <poll.h>
#include <sys/time.h>
#include <fcntl.h>
#include <errno.h>
#include <pthread.h>
#include <math.h>
#include <unistd.h>
#include <signal.h>
#include <sys/prctl.h>
#include <limits.h>

#include "sample_comm.h"
#include "hi_common.h"
#include "hi_mipi_rx.h"
#include "hi_mpi_vi.h"
#include "hi_mpi_isp.h"
#include "securec.h"

#define MIPI_DEV_NAME "/dev/ot_mipi_rx"

#define FPN_FILE_NAME_LENGTH 150
#define WIDTH_1920 1920
#define HEIGHT_1080 1080
#define WIDTH_2560 2560
#define HEIGHT_1440 1440
#define WIDTH_2688 2688
#define HEIGHT_1520 1520
#define WIDTH_2880 2880
#define HEIGHT_1620 1620
#define WIDTH_3840 3840
#define HEIGHT_2160 2160
#define WIDTH_2688 2688
#define HEIGHT_1520 1520
#define SLEEP_TIME 1000
#define MIPI_NUM 3
#define OB_HEIGHT_END 24
#define OB_HEIGHT_START 0
#define IMX347_OB_HEIGHT_END 20
#define IMX485_OB_HEIGHT_END 20
#define THERMO_WIDTH_400 400
#define THERMO_HEIGHT_308 308
#define THERMO_WIDTH_384 384
#define THERMO_HEIGHT_288 288

typedef struct {
    sample_vi_user_frame_info *user_frame_info;
    hi_vi_pipe vi_pipe;
    hi_u32 frame_cnt;
} sample_vi_send_frame_info;

static hi_bool g_send_pipe_pthread = HI_FALSE;
static hi_bool g_start_isp[HI_VI_MAX_PIPE_NUM] = {HI_FALSE};

static ext_data_type_t g_mipi_ext_data_type_default_attr = {
    .devno = 0,
    .num = MIPI_NUM,
    .ext_data_bit_width = {12, 12, 12},
    .ext_data_type = {0x2c, 0x2c, 0x2c}
};

static combo_dev_attr_t g_mipi_2lane_chn0_sensor_sc4336p_10bit_4m_nowdr_attr = {
    .devno = 0,
    .input_mode = INPUT_MODE_MIPI,
    .data_rate = MIPI_DATA_RATE_X1,
    .img_rect = {0, 0, WIDTH_2560, HEIGHT_1440},
    .mipi_attr = {
        DATA_TYPE_RAW_12BIT,
        HI_MIPI_WDR_MODE_NONE,
        {0, 2, -1, -1}
    }
};

static combo_dev_attr_t g_mipi_2lane_chn0_sensor_sc4336p_10bit_4m_nowdr_dev0_attr = {
    .devno = 0,
    .input_mode = INPUT_MODE_MIPI,
    .data_rate = MIPI_DATA_RATE_X1,
    .img_rect = {0, 0, WIDTH_2560, HEIGHT_1440},
    .mipi_attr = {
        DATA_TYPE_RAW_12BIT,
        HI_MIPI_WDR_MODE_NONE,
        {0, 2, -1, -1}
    }
};

static combo_dev_attr_t g_mipi_2lane_chn0_sensor_sc4336p_10bit_4m_nowdr_dev1_attr = {
    .devno = 1,
    .input_mode = INPUT_MODE_MIPI,
    .data_rate = MIPI_DATA_RATE_X1,
    .img_rect = {0, 0, WIDTH_2560, HEIGHT_1440},
    .mipi_attr = {
        DATA_TYPE_RAW_12BIT,
        HI_MIPI_WDR_MODE_NONE,
        {1, 3, -1, -1}
    }
};

static combo_dev_attr_t g_mipi_2lane_chn0_sensor_sc4336p_10bit_3m_nowdr_attr = {
    .devno = 0,
    .input_mode = INPUT_MODE_MIPI,
    .data_rate = MIPI_DATA_RATE_X1,
    .img_rect = {0, 0, WIDTH_2304, HEIGHT_1296},
    .mipi_attr = {
        DATA_TYPE_RAW_12BIT,
        HI_MIPI_WDR_MODE_NONE,
        {0, 2, -1, -1}
    }
};

static combo_dev_attr_t g_mipi_2lane_chn0_sensor_sc4336p_10bit_3m_nowdr_dev0_attr = {
    .devno = 0,
    .input_mode = INPUT_MODE_MIPI,
    .data_rate = MIPI_DATA_RATE_X1,
    .img_rect = {0, 0, WIDTH_2304, HEIGHT_1296},
    .mipi_attr = {
        DATA_TYPE_RAW_12BIT,
        HI_MIPI_WDR_MODE_NONE,
        {0, 2, -1, -1}
    }
};

static combo_dev_attr_t g_mipi_2lane_chn0_sensor_sc4336p_10bit_3m_nowdr_dev1_attr = {
    .devno = 1,
    .input_mode = INPUT_MODE_MIPI,
    .data_rate = MIPI_DATA_RATE_X1,
    .img_rect = {0, 0, WIDTH_2304, HEIGHT_1296},
    .mipi_attr = {
        DATA_TYPE_RAW_12BIT,
        HI_MIPI_WDR_MODE_NONE,
        {1, 3, -1, -1}
    }
};

static combo_dev_attr_t g_mipi_2lane_chn0_sensor_os04d10_10bit_4m_nowdr_attr = {
    .devno = 0,
    .input_mode = INPUT_MODE_MIPI,
    .data_rate = MIPI_DATA_RATE_X1,
    .img_rect = {0, 0, WIDTH_2560, HEIGHT_1440},
    .mipi_attr = {
        DATA_TYPE_RAW_12BIT,
        HI_MIPI_WDR_MODE_NONE,
        {0, 2, -1, -1}
    }
};

static combo_dev_attr_t g_mipi_2lane_chn0_sensor_os04d10_10bit_4m_nowdr_dev0_attr = {
    .devno = 0,
    .input_mode = INPUT_MODE_MIPI,
    .data_rate = MIPI_DATA_RATE_X1,
    .img_rect = {0, 0, WIDTH_2560, HEIGHT_1440},
    .mipi_attr = {
        DATA_TYPE_RAW_12BIT,
        HI_MIPI_WDR_MODE_NONE,
        {0, 2, -1, -1}
    }
};

static combo_dev_attr_t g_mipi_2lane_chn0_sensor_os04d10_10bit_4m_nowdr_dev1_attr = {
    .devno = 1,
    .input_mode = INPUT_MODE_MIPI,
    .data_rate = MIPI_DATA_RATE_X1,
    .img_rect = {0, 0, WIDTH_2560, HEIGHT_1440},
    .mipi_attr = {
        DATA_TYPE_RAW_12BIT,
        HI_MIPI_WDR_MODE_NONE,
        {1, 3, -1, -1}
    }
};

static combo_dev_attr_t g_mipi_4lane_chn0_sensor_os04a10_12bit_4m_nowdr_attr = {
    .devno = 0,
    .input_mode = INPUT_MODE_MIPI,
    .data_rate  = MIPI_DATA_RATE_X1,
    .img_rect   = {0, 0, WIDTH_2688, HEIGHT_1520},
    .mipi_attr  = {
        DATA_TYPE_RAW_12BIT,
        HI_MIPI_WDR_MODE_NONE,
        {0, 1, 2, 3}
    }
};

static combo_dev_attr_t g_mipi_4lane_chn0_sensor_os04a10_12bit_4m_wdr2to1_attr = {
    .devno = 0,
    .input_mode = INPUT_MODE_MIPI,
    .data_rate  = MIPI_DATA_RATE_X1,
    .img_rect   = {0, 0, WIDTH_2688, HEIGHT_1520},
    .mipi_attr  = {
        DATA_TYPE_RAW_12BIT,
        HI_MIPI_WDR_MODE_NONE,
        {0, 1, 2, 3}
    }
};

static combo_dev_attr_t g_mipi_4lane_chn0_sensor_os04a10_12bit_4m_wdr2to1_dev0_attr = {
    .devno = 0,
    .input_mode = INPUT_MODE_MIPI,
    .data_rate  = MIPI_DATA_RATE_X1,
    .img_rect   = {0, 0, WIDTH_2688, HEIGHT_1520},
    .mipi_attr  = {
        DATA_TYPE_RAW_12BIT,
        HI_MIPI_WDR_MODE_VC,
        {0, 2, -1, -1}
    }
};

static combo_dev_attr_t g_mipi_4lane_chn0_sensor_os04a10_12bit_4m_wdr2to1_dev1_attr = {
    .devno = 1,
    .input_mode = INPUT_MODE_MIPI,
    .data_rate  = MIPI_DATA_RATE_X1,
    .img_rect   = {0, 0, WIDTH_2688, HEIGHT_1520},
    .mipi_attr  = {
        DATA_TYPE_RAW_12BIT,
        HI_MIPI_WDR_MODE_VC,
        {1, 3, -1, -1}
    }
};


static combo_dev_attr_t g_mipi_4lane_chn0_sensor_os04a10_12bit_4m_nowdr_dev0_attr = {
    .devno = 0, /* dev1 */
    .input_mode = INPUT_MODE_MIPI,
    .data_rate  = MIPI_DATA_RATE_X1,
    .img_rect   = {0, 0, WIDTH_2688, HEIGHT_1520},
    .mipi_attr  = {
        DATA_TYPE_RAW_12BIT,
        HI_MIPI_WDR_MODE_NONE,
        {0, 2, -1, -1}
    }
};
static combo_dev_attr_t g_mipi_4lane_chn0_sensor_os04a10_12bit_4m_nowdr_dev1_attr = {
    .devno = 1, /* dev1 */
    .input_mode = INPUT_MODE_MIPI,
    .data_rate  = MIPI_DATA_RATE_X1,
    .img_rect   = {0, 0, WIDTH_2688, HEIGHT_1520},
    .mipi_attr  = {
        DATA_TYPE_RAW_12BIT,
        HI_MIPI_WDR_MODE_NONE,
        {1, 3, -1, -1}
    }
};




static combo_dev_attr_t g_mipi_2lane_chn0_sensor_gc4023_10bit_4m_nowdr_attr = {
    .devno = 0,
    .input_mode = INPUT_MODE_MIPI,
    .data_rate = MIPI_DATA_RATE_X1,
    .img_rect = {0, 0, WIDTH_2560, HEIGHT_1440},
    .mipi_attr = {
        DATA_TYPE_RAW_12BIT,
        HI_MIPI_WDR_MODE_NONE,
        {0, 1, -1, -1}
        //for zw-board {0, 2, -1, -1}
    }
};

static combo_dev_attr_t g_mipi_2lane_chn0_sensor_gc4023_10bit_4m_nowdr_dev0_attr = {
    .devno = 0,
    .input_mode = INPUT_MODE_MIPI,
    .data_rate = MIPI_DATA_RATE_X1,
    .img_rect = {0, 0, WIDTH_2560, HEIGHT_1440},
    .mipi_attr = {
        DATA_TYPE_RAW_12BIT,
        HI_MIPI_WDR_MODE_NONE,
        {0, 2, -1, -1}
    }
};

static combo_dev_attr_t g_mipi_2lane_chn0_sensor_gc4023_10bit_4m_nowdr_dev1_attr = {
    .devno = 1,
    .input_mode = INPUT_MODE_MIPI,
    .data_rate = MIPI_DATA_RATE_X1,
    .img_rect = {0, 0, WIDTH_2560, HEIGHT_1440},
    .mipi_attr = {
        DATA_TYPE_RAW_12BIT,
        HI_MIPI_WDR_MODE_NONE,
        {1, 3, -1, -1}
    }
};

static combo_dev_attr_t g_mipi_2lane_chn0_sensor_sc450ai_10bit_4m_nowdr_dev0_attr = {
    .devno = 0,
    .input_mode = INPUT_MODE_MIPI,
    .data_rate = MIPI_DATA_RATE_X1,
    .img_rect = {0, 0, WIDTH_2688, HEIGHT_1520},
    .mipi_attr = {
        DATA_TYPE_RAW_12BIT,
        HI_MIPI_WDR_MODE_NONE,
        {0, 2, -1, -1}
    }
};

static combo_dev_attr_t g_mipi_2lane_chn0_sensor_sc450ai_10bit_4m_nowdr_dev1_attr = {
    .devno = 1,
    .input_mode = INPUT_MODE_MIPI,
    .data_rate = MIPI_DATA_RATE_X1,
    .img_rect = {0, 0, WIDTH_2688, HEIGHT_1520},
    .mipi_attr = {
        DATA_TYPE_RAW_12BIT,
        HI_MIPI_WDR_MODE_NONE,
        {1, 3, -1, -1}
    }
};

static combo_dev_attr_t g_mipi_4lane_chn0_sensor_sc431hai_10bit_4m_nowdr_attr = {
    .devno = 0,
    .input_mode = INPUT_MODE_MIPI,
    .data_rate = MIPI_DATA_RATE_X1,
    .img_rect = {0, 0, WIDTH_2560, HEIGHT_1440},
    .mipi_attr = {
        DATA_TYPE_RAW_10BIT,
        HI_MIPI_WDR_MODE_NONE,
        {0, 1, 2, 3}
    }
};

static combo_dev_attr_t g_mipi_2lane_chn0_sensor_sc450ai_10bit_4m_nowdr_attr = {
    .devno = 0,
    .input_mode = INPUT_MODE_MIPI,
    .data_rate = MIPI_DATA_RATE_X1,
    .img_rect = {0, 0, WIDTH_2688, HEIGHT_1520},
    .mipi_attr = {
        DATA_TYPE_RAW_10BIT,
        HI_MIPI_WDR_MODE_NONE,
        {0, 1, -1, -1}
    }
};

static combo_dev_attr_t g_mipi_4lane_chn0_sensor_sc450ai_10bit_4m_nowdr_attr = {
    .devno = 0,
    .input_mode = INPUT_MODE_MIPI,
    .data_rate = MIPI_DATA_RATE_X1,
    .img_rect = {0, 0, WIDTH_2688, HEIGHT_1520},
    .mipi_attr = {
        DATA_TYPE_RAW_10BIT,
        HI_MIPI_WDR_MODE_NONE,
        {0, 1, 2, 3}
    }
};

static combo_dev_attr_t g_mipi_4lane_chn0_sensor_sc500ai_10bit_4m_nowdr_attr = {
    .devno = 0,
    .input_mode = INPUT_MODE_MIPI,
    .data_rate = MIPI_DATA_RATE_X1,
    .img_rect = {0, 0, WIDTH_2880, HEIGHT_1620},
    .mipi_attr = {
        DATA_TYPE_RAW_10BIT,
        HI_MIPI_WDR_MODE_NONE,
        {0, 1, 2, 3}
    }
};

static combo_dev_attr_t g_mipi_2lane_chn0_sensor_sc500ai_10bit_5m_nowdr_attr = {
    .devno = 0,
    .input_mode = INPUT_MODE_MIPI,
    .data_rate = MIPI_DATA_RATE_X1,
    .img_rect = {0, 0, WIDTH_2880, HEIGHT_1620},
    .mipi_attr = {
        DATA_TYPE_RAW_10BIT,
        HI_MIPI_WDR_MODE_NONE,
        {0, 1, -1, -1}
    }
};

static combo_dev_attr_t g_mipi_2lane_chn0_sensor_sc500ai_10bit_5m_nowdr_dev0_attr = {
    .devno = 0,
    .input_mode = INPUT_MODE_MIPI,
    .data_rate = MIPI_DATA_RATE_X1,
    .img_rect = {0, 0, WIDTH_2880, HEIGHT_1620},
    .mipi_attr = {
        DATA_TYPE_RAW_12BIT,
        HI_MIPI_WDR_MODE_NONE,
        {0, 2, -1, -1}
    }
};

static combo_dev_attr_t g_mipi_2lane_chn0_sensor_sc500ai_10bit_5m_nowdr_dev1_attr = {
    .devno = 1,
    .input_mode = INPUT_MODE_MIPI,
    .data_rate = MIPI_DATA_RATE_X1,
    .img_rect = {0, 0, WIDTH_2880, HEIGHT_1620},
    .mipi_attr = {
        DATA_TYPE_RAW_12BIT,
        HI_MIPI_WDR_MODE_NONE,
        {1, 3, -1, -1}
    }
};

/* calc online pipe sensor pixel_rate: lane_rate(Mbps) * 1000000 * lane_num / bit_num / wdr_num */
static hi_u32 sample_comm_vi_calc_pipe_pixel_rate(hi_u32 lane_rate, hi_u32 lane_num, hi_u32 bit_num, hi_u32 wdr_num)
{
    hi_u32 lane_base_bps = 1000 * 1000; /* base: 1Mbps */
    hi_u64 sensor_total_rate = lane_rate * lane_base_bps * lane_num / bit_num;
    return sensor_total_rate / wdr_num;
}

static hi_u32 sample_comm_vi_get_sensor_pixel_rate_by_type(sample_sns_type sns_type)
{
    hi_u32 pixel_rate = 0;
    switch (sns_type) {
        case SC4336P_MIPI_4M_30FPS_10BIT:
        case SC4336P_MIPI_3M_30FPS_10BIT:
            pixel_rate = sample_comm_vi_calc_pipe_pixel_rate(630, 2, 10, 1); /* 630Mbps * 2lane / 10bit */
            break;
        case OS04D10_MIPI_4M_30FPS_10BIT:
            pixel_rate = sample_comm_vi_calc_pipe_pixel_rate(720, 2, 10, 1); /* 720Mbps * 2lane / 10bit */
            break;
        case OV_OS04A10_MIPI_4M_30FPS_12BIT:
            pixel_rate = sample_comm_vi_calc_pipe_pixel_rate(720, 4, 12, 1); /* 720Mbps * 2lane / 10bit */
            break;
        case GC4023_MIPI_4M_30FPS_10BIT:
            pixel_rate = sample_comm_vi_calc_pipe_pixel_rate(702, 2, 10, 1); /* 702Mbps * 2lane / 10bit */
            break;
        case SC431HAI_MIPI_4M_30FPS_10BIT:
            pixel_rate = sample_comm_vi_calc_pipe_pixel_rate(315, 4, 10, 1); /* 315Mbps * 4lane / 10bit */
            break;
        case SC431HAI_MIPI_4M_30FPS_10BIT_WDR2TO1:
            pixel_rate = sample_comm_vi_calc_pipe_pixel_rate(630, 4, 10, 2); /* 630Mbps * 4lane / 10bit / 2vc */
            break;
        case SC450AI_MIPI_4M_30FPS_10BIT:
            pixel_rate = sample_comm_vi_calc_pipe_pixel_rate(720, 2, 10, 1); /* 720Mbps * 2lane / 10bit */
            break;
        case SC450AI_MIPI_4M_30FPS_10BIT_WDR2TO1:
            pixel_rate = sample_comm_vi_calc_pipe_pixel_rate(720, 4, 10, 2); /* 720Mbps * 4lane / 10bit / 2vc */
            break;
        case SC500AI_MIPI_5M_30FPS_10BIT:
            pixel_rate = sample_comm_vi_calc_pipe_pixel_rate(792, 2, 10, 1); /* 792Mbps * 2lane / 10bit */
            break;
        case SC500AI_MIPI_5M_30FPS_10BIT_WDR2TO1:
            pixel_rate = sample_comm_vi_calc_pipe_pixel_rate(792, 4, 10, 2); /* 792Mbps * 4lane / 10bit / 2vc */
            break;
        default:
            break;
    }

    return pixel_rate;
}



static hi_void sample_comm_vi_get_mipi_attr(sample_sns_type sns_type, combo_dev_attr_t *combo_attr)
{
    hi_u32 ob_height = OB_HEIGHT_START;
    switch (sns_type) {
        case SC4336P_MIPI_4M_30FPS_10BIT:
            (hi_void)memcpy_s(combo_attr, sizeof(combo_dev_attr_t),
                &g_mipi_2lane_chn0_sensor_sc4336p_10bit_4m_nowdr_attr, sizeof(combo_dev_attr_t));
            break;
        case SC4336P_MIPI_3M_30FPS_10BIT:
            (hi_void)memcpy_s(combo_attr, sizeof(combo_dev_attr_t),
                &g_mipi_2lane_chn0_sensor_sc4336p_10bit_3m_nowdr_attr, sizeof(combo_dev_attr_t));
            break;
        case OS04D10_MIPI_4M_30FPS_10BIT:
            (hi_void)memcpy_s(combo_attr, sizeof(combo_dev_attr_t),
                &g_mipi_2lane_chn0_sensor_os04d10_10bit_4m_nowdr_attr, sizeof(combo_dev_attr_t));
            break;
        case GC4023_MIPI_4M_30FPS_10BIT:
            (hi_void)memcpy_s(combo_attr, sizeof(combo_dev_attr_t),
                &g_mipi_2lane_chn0_sensor_gc4023_10bit_4m_nowdr_attr, sizeof(combo_dev_attr_t));
            break;
        case SC431HAI_MIPI_4M_30FPS_10BIT:
        case SC431HAI_MIPI_4M_30FPS_10BIT_WDR2TO1:
            (hi_void)memcpy_s(combo_attr, sizeof(combo_dev_attr_t),
                &g_mipi_4lane_chn0_sensor_sc431hai_10bit_4m_nowdr_attr, sizeof(combo_dev_attr_t));
            break;

        case SC450AI_MIPI_4M_30FPS_10BIT:
            (hi_void)memcpy_s(combo_attr, sizeof(combo_dev_attr_t),
                &g_mipi_2lane_chn0_sensor_sc450ai_10bit_4m_nowdr_attr, sizeof(combo_dev_attr_t));
            break;
        case SC450AI_MIPI_4M_30FPS_10BIT_WDR2TO1:
            (hi_void)memcpy_s(combo_attr, sizeof(combo_dev_attr_t),
                &g_mipi_4lane_chn0_sensor_sc450ai_10bit_4m_nowdr_attr, sizeof(combo_dev_attr_t));
            break;

        case SC500AI_MIPI_5M_30FPS_10BIT:
            (hi_void)memcpy_s(combo_attr, sizeof(combo_dev_attr_t),
                &g_mipi_2lane_chn0_sensor_sc500ai_10bit_5m_nowdr_attr, sizeof(combo_dev_attr_t));
            break;
        case SC500AI_MIPI_5M_30FPS_10BIT_WDR2TO1:
            (hi_void)memcpy_s(combo_attr, sizeof(combo_dev_attr_t),
                &g_mipi_4lane_chn0_sensor_sc500ai_10bit_4m_nowdr_attr, sizeof(combo_dev_attr_t));
            break;
        case OV_OS04A10_MIPI_4M_30FPS_12BIT:
        case OV_OS04A10_SLAVE_MIPI_4M_30FPS_12BIT:
            (hi_void)memcpy_s(combo_attr, sizeof(combo_dev_attr_t),
                &g_mipi_4lane_chn0_sensor_os04a10_12bit_4m_nowdr_attr, sizeof(combo_dev_attr_t));
            break;
        case OV_OS04A10_MIPI_4M_30FPS_12BIT_WDR2TO1:
            (hi_void)memcpy_s(combo_attr, sizeof(combo_dev_attr_t),
                &g_mipi_4lane_chn0_sensor_os04a10_12bit_4m_wdr2to1_attr, sizeof(combo_dev_attr_t));
            break;
        default:
            (hi_void)memcpy_s(combo_attr, sizeof(combo_dev_attr_t),
                &g_mipi_2lane_chn0_sensor_sc4336p_10bit_4m_nowdr_attr, sizeof(combo_dev_attr_t));
            break;
    }
    combo_attr->img_rect.height = combo_attr->img_rect.height + ob_height;
}

static hi_void sample_comm_vi_get_mipi_ext_data_attr(sample_sns_type sns_type, ext_data_type_t *ext_data_attr)
{
    switch (sns_type) {
        case SC4336P_MIPI_4M_30FPS_10BIT:
        case SC4336P_MIPI_3M_30FPS_10BIT:
        case GC4023_MIPI_4M_30FPS_10BIT:
        case OS04D10_MIPI_4M_30FPS_10BIT:
        case OV_OS04A10_MIPI_4M_30FPS_12BIT:
        case OV_OS04A10_MIPI_4M_30FPS_12BIT_WDR2TO1:
        case OV_OS04A10_SLAVE_MIPI_4M_30FPS_12BIT:
        case SC431HAI_MIPI_4M_30FPS_10BIT:
        case SC450AI_MIPI_4M_30FPS_10BIT:
        case SC500AI_MIPI_5M_30FPS_10BIT:
            (hi_void)memcpy_s(ext_data_attr, sizeof(ext_data_type_t),
                &g_mipi_ext_data_type_default_attr, sizeof(ext_data_type_t));
            break;

        default:
            (hi_void)memcpy_s(ext_data_attr, sizeof(ext_data_type_t),
                &g_mipi_ext_data_type_default_attr, sizeof(ext_data_type_t));
    }
}

static hi_void sample_comm_vi_get_mipi_attr_by_dev_id_part1(sample_sns_type sns_type, hi_vi_dev vi_dev,
                                                            combo_dev_attr_t *combo_attr)
{
    switch (sns_type) {
        case OS04D10_MIPI_4M_30FPS_10BIT:
            if (vi_dev == 0) {
                (hi_void)memcpy_s(combo_attr, sizeof(combo_dev_attr_t),
                    &g_mipi_2lane_chn0_sensor_os04d10_10bit_4m_nowdr_dev0_attr, sizeof(combo_dev_attr_t));
            } else if (vi_dev == 1) { /* dev1 */
                (hi_void)memcpy_s(combo_attr, sizeof(combo_dev_attr_t),
                    &g_mipi_2lane_chn0_sensor_os04d10_10bit_4m_nowdr_dev1_attr, sizeof(combo_dev_attr_t));
            }
            break;
            
        case OV_OS04A10_MIPI_4M_30FPS_12BIT:
        case OV_OS04A10_SLAVE_MIPI_4M_30FPS_12BIT:
            if (vi_dev == 0) {
                (hi_void)memcpy_s(combo_attr, sizeof(combo_dev_attr_t),
                    &g_mipi_4lane_chn0_sensor_os04a10_12bit_4m_nowdr_dev0_attr, sizeof(combo_dev_attr_t));
            } else if (vi_dev == 1) { /* dev1 */
                (hi_void)memcpy_s(combo_attr, sizeof(combo_dev_attr_t),
                    &g_mipi_4lane_chn0_sensor_os04a10_12bit_4m_nowdr_dev1_attr, sizeof(combo_dev_attr_t));
            }
            break;

        case OV_OS04A10_MIPI_4M_30FPS_12BIT_WDR2TO1:
            if (vi_dev == 0) {
                (hi_void)memcpy_s(combo_attr, sizeof(combo_dev_attr_t),
                    &g_mipi_4lane_chn0_sensor_os04a10_12bit_4m_wdr2to1_dev0_attr, sizeof(combo_dev_attr_t));
            } else if (vi_dev == 1) { /* dev1 */
                (hi_void)memcpy_s(combo_attr, sizeof(combo_dev_attr_t),
                    &g_mipi_4lane_chn0_sensor_os04a10_12bit_4m_wdr2to1_dev1_attr, sizeof(combo_dev_attr_t));
            }
            break;
            
        case SC500AI_MIPI_5M_30FPS_10BIT:
            if (vi_dev == 0) {
                (hi_void)memcpy_s(combo_attr, sizeof(combo_dev_attr_t),
                    &g_mipi_2lane_chn0_sensor_sc500ai_10bit_5m_nowdr_dev0_attr, sizeof(combo_dev_attr_t));
            } else if (vi_dev == 1) { /* dev1 */
                (hi_void)memcpy_s(combo_attr, sizeof(combo_dev_attr_t),
                    &g_mipi_2lane_chn0_sensor_sc500ai_10bit_5m_nowdr_dev1_attr, sizeof(combo_dev_attr_t));
            }
            break;

        default:
            break;
    }
}

static hi_void sample_comm_vi_get_mipi_attr_by_dev_id(sample_sns_type sns_type, hi_vi_dev vi_dev,
                                                      combo_dev_attr_t *combo_attr)
{
    hi_u32 ob_height = OB_HEIGHT_START;
    switch (sns_type) {
        case SC4336P_MIPI_4M_30FPS_10BIT:
            if (vi_dev == 0) {
                (hi_void)memcpy_s(combo_attr, sizeof(combo_dev_attr_t),
                    &g_mipi_2lane_chn0_sensor_sc4336p_10bit_4m_nowdr_dev0_attr, sizeof(combo_dev_attr_t));
            } else if (vi_dev == 1) { /* dev1 */
                (hi_void)memcpy_s(combo_attr, sizeof(combo_dev_attr_t),
                    &g_mipi_2lane_chn0_sensor_sc4336p_10bit_4m_nowdr_dev1_attr, sizeof(combo_dev_attr_t));
            }
            break;
        case SC4336P_MIPI_3M_30FPS_10BIT:
            if (vi_dev == 0) {
                (hi_void)memcpy_s(combo_attr, sizeof(combo_dev_attr_t),
                    &g_mipi_2lane_chn0_sensor_sc4336p_10bit_3m_nowdr_dev0_attr, sizeof(combo_dev_attr_t));
            } else if (vi_dev == 1) { /* dev1 */
                (hi_void)memcpy_s(combo_attr, sizeof(combo_dev_attr_t),
                    &g_mipi_2lane_chn0_sensor_sc4336p_10bit_3m_nowdr_dev1_attr, sizeof(combo_dev_attr_t));
            }
            break;
        case GC4023_MIPI_4M_30FPS_10BIT:
            if (vi_dev == 0) {
                (hi_void)memcpy_s(combo_attr, sizeof(combo_dev_attr_t),
                    &g_mipi_2lane_chn0_sensor_gc4023_10bit_4m_nowdr_dev0_attr, sizeof(combo_dev_attr_t));
            } else if (vi_dev == 1) { /* dev1 */
                (hi_void)memcpy_s(combo_attr, sizeof(combo_dev_attr_t),
                    &g_mipi_2lane_chn0_sensor_gc4023_10bit_4m_nowdr_dev1_attr, sizeof(combo_dev_attr_t));
            }
            break;
        case SC450AI_MIPI_4M_30FPS_10BIT:
            if (vi_dev == 0) {
                (hi_void)memcpy_s(combo_attr, sizeof(combo_dev_attr_t),
                    &g_mipi_2lane_chn0_sensor_sc450ai_10bit_4m_nowdr_dev0_attr, sizeof(combo_dev_attr_t));
            } else if (vi_dev == 1) { /* dev1 */
                (hi_void)memcpy_s(combo_attr, sizeof(combo_dev_attr_t),
                    &g_mipi_2lane_chn0_sensor_sc450ai_10bit_4m_nowdr_dev1_attr, sizeof(combo_dev_attr_t));
            }
            break;

        default:
            (hi_void)memcpy_s(combo_attr, sizeof(combo_dev_attr_t),
                &g_mipi_2lane_chn0_sensor_sc4336p_10bit_4m_nowdr_dev0_attr, sizeof(combo_dev_attr_t));
    }
    sample_comm_vi_get_mipi_attr_by_dev_id_part1(sns_type, vi_dev, combo_attr);
    combo_attr->img_rect.height = combo_attr->img_rect.height + ob_height;
}

static hi_vi_dev_attr g_mipi_raw_dev_attr = {
    .intf_mode = HI_VI_INTF_MODE_MIPI,

    /* Invalid argument */
    .work_mode = HI_VI_WORK_MODE_MULTIPLEX_1,

    /* mask component */
    .component_mask = {0xfff00000, 0x00000000},

    .scan_mode = HI_VI_SCAN_PROGRESSIVE,

    /* Invalid argument */
    .ad_chn_id = {-1, -1, -1, -1},

    /* data seq */
    .data_seq = HI_VI_DATA_SEQ_YVYU,

    /* sync param */
    .sync_cfg = {
        .vsync           = HI_VI_VSYNC_FIELD,
        .vsync_neg       = HI_VI_VSYNC_NEG_HIGH,
        .hsync           = HI_VI_HSYNC_VALID_SIG,
        .hsync_neg       = HI_VI_HSYNC_NEG_HIGH,
        .vsync_valid     = HI_VI_VSYNC_VALID_SIG,
        .vsync_valid_neg = HI_VI_VSYNC_VALID_NEG_HIGH,
        .timing_blank    = {
            /* hsync_hfb      hsync_act     hsync_hhb */
            0,                0,            0,
            /* vsync0_vhb     vsync0_act    vsync0_hhb */
            0,                0,            0,
            /* vsync1_vhb     vsync1_act    vsync1_hhb */
            0,                0,            0
        }
    },

    /* data type */
    .data_type = HI_VI_DATA_TYPE_RAW,

    /* data reverse */
    .data_reverse = HI_FALSE,

    /* input size */
    .in_size = {WIDTH_3840, HEIGHT_2160},

    /* data rate */
    .data_rate = HI_DATA_RATE_X1,
};

static hi_void sample_comm_vi_get_dev_attr_by_intf_mode(hi_vi_intf_mode intf_mode, hi_vi_dev_attr *dev_attr)
{
    switch (intf_mode) {
        case HI_VI_INTF_MODE_MIPI:
            (hi_void)memcpy_s(dev_attr, sizeof(hi_vi_dev_attr), &g_mipi_raw_dev_attr, sizeof(hi_vi_dev_attr));
            break;

        default:
            (hi_void)memcpy_s(dev_attr, sizeof(hi_vi_dev_attr), &g_mipi_raw_dev_attr, sizeof(hi_vi_dev_attr));
            break;
    }
}

hi_void sample_comm_vi_get_size_by_sns_type(sample_sns_type sns_type, hi_size *size)
{
    switch (sns_type) {
        case SC4336P_MIPI_4M_30FPS_10BIT:
        case OS04D10_MIPI_4M_30FPS_10BIT:
        case GC4023_MIPI_4M_30FPS_10BIT:
        case SC431HAI_MIPI_4M_30FPS_10BIT:
        case SC431HAI_MIPI_4M_30FPS_10BIT_WDR2TO1:
            size->width  = WIDTH_2560;
            size->height = HEIGHT_1440;
            break;

        case SC450AI_MIPI_4M_30FPS_10BIT:
        case SC450AI_MIPI_4M_30FPS_10BIT_WDR2TO1:
        case OV_OS04A10_MIPI_4M_30FPS_12BIT:
        case OV_OS04A10_SLAVE_MIPI_4M_30FPS_12BIT:
        case OV_OS04A10_MIPI_4M_30FPS_12BIT_WDR2TO1:
            size->width  = WIDTH_2688;
            size->height = HEIGHT_1520;
            break;

        case SC500AI_MIPI_5M_30FPS_10BIT:
        case SC500AI_MIPI_5M_30FPS_10BIT_WDR2TO1:
            size->width  = WIDTH_2880;
            size->height = HEIGHT_1620;
            break;

        case SC4336P_MIPI_3M_30FPS_10BIT:
            size->width  = WIDTH_2304;
            size->height = HEIGHT_1296;
            break;

        default:
            size->width  = WIDTH_1920;
            size->height = HEIGHT_1080;
            break;
    }
}

hi_u32 sample_comm_vi_get_obheight_by_sns_type(sample_sns_type sns_type)
{
    hi_u32 ob_height = OB_HEIGHT_START;
    switch (sns_type) {
        case SC4336P_MIPI_4M_30FPS_10BIT:
        case SC4336P_MIPI_3M_30FPS_10BIT:
        case GC4023_MIPI_4M_30FPS_10BIT:
        case OS04D10_MIPI_4M_30FPS_10BIT:
        case SC431HAI_MIPI_4M_30FPS_10BIT:
        case SC431HAI_MIPI_4M_30FPS_10BIT_WDR2TO1:
        case SC450AI_MIPI_4M_30FPS_10BIT:
        case SC450AI_MIPI_4M_30FPS_10BIT_WDR2TO1:
        case SC500AI_MIPI_5M_30FPS_10BIT:
        case SC500AI_MIPI_5M_30FPS_10BIT_WDR2TO1:
        case OV_OS04A10_MIPI_4M_30FPS_12BIT:
        case OV_OS04A10_SLAVE_MIPI_4M_30FPS_12BIT:
        case OV_OS04A10_MIPI_4M_30FPS_12BIT_WDR2TO1:
            ob_height = OB_HEIGHT_START;
            break;
        default:
            break;
    }

    return ob_height;
}

static hi_u32 sample_comm_vi_get_pipe_num_by_sns_type(sample_sns_type sns_type)
{
    switch (sns_type) {
        case SC4336P_MIPI_4M_30FPS_10BIT:
        case SC4336P_MIPI_3M_30FPS_10BIT:
        case SC431HAI_MIPI_4M_30FPS_10BIT:
        case SC450AI_MIPI_4M_30FPS_10BIT:
        case SC500AI_MIPI_5M_30FPS_10BIT:
        case OS04D10_MIPI_4M_30FPS_10BIT:
        case OV_OS04A10_MIPI_4M_30FPS_12BIT:
        case OV_OS04A10_SLAVE_MIPI_4M_30FPS_12BIT:
            return 1;

        case SC431HAI_MIPI_4M_30FPS_10BIT_WDR2TO1:
        case SC450AI_MIPI_4M_30FPS_10BIT_WDR2TO1:
        case SC500AI_MIPI_5M_30FPS_10BIT_WDR2TO1:
        case OV_OS04A10_MIPI_4M_30FPS_12BIT_WDR2TO1:
            return 2; /* 2: wdr2to1 */

        default:
            return 1;
    }
}

static hi_wdr_mode sample_comm_vi_get_wdr_mode_by_sns_type(sample_sns_type sns_type)
{
    switch (sns_type) {
        case SC4336P_MIPI_4M_30FPS_10BIT:
        case SC4336P_MIPI_3M_30FPS_10BIT:
        case GC4023_MIPI_4M_30FPS_10BIT:
        case OS04D10_MIPI_4M_30FPS_10BIT:
        case SC431HAI_MIPI_4M_30FPS_10BIT:
        case SC450AI_MIPI_4M_30FPS_10BIT:
        case SC500AI_MIPI_5M_30FPS_10BIT:
        case OV_OS04A10_MIPI_4M_30FPS_12BIT:
        case OV_OS04A10_SLAVE_MIPI_4M_30FPS_12BIT:
            return HI_WDR_MODE_NONE;

        case SC431HAI_MIPI_4M_30FPS_10BIT_WDR2TO1:
        case SC450AI_MIPI_4M_30FPS_10BIT_WDR2TO1:
        case SC500AI_MIPI_5M_30FPS_10BIT_WDR2TO1:
        case OV_OS04A10_MIPI_4M_30FPS_12BIT_WDR2TO1:
            return HI_WDR_MODE_2To1_LINE;

        default:
            return HI_WDR_MODE_NONE;
    }
}

hi_void sample_comm_vi_get_default_sns_info(sample_sns_type sns_type, sample_sns_info *sns_info)
{
    sns_info->sns_type    = sns_type;
    sns_info->sns_clk_src = 0;
    sns_info->sns_rst_src = 0;
    sns_info->bus_id      = 0; /* asic i2c4 , FPGA default 0 */
    sns_info->sns_clk_rst_en = HI_TRUE;
}

hi_void sample_comm_vi_get_default_mipi_info(sample_sns_type sns_type, sample_mipi_info *mipi_info)
{
    mipi_info->mipi_dev    = 0;
    mipi_info->divide_mode = LANE_DIVIDE_MODE_0;
    sample_comm_vi_get_mipi_attr(sns_type, &mipi_info->combo_dev_attr);
    sample_comm_vi_get_mipi_ext_data_attr(sns_type, &mipi_info->ext_data_type_attr);
}

/* used for two sensor: mipi lane 2 + 2 */
hi_void sample_comm_vi_get_mipi_info_by_dev_id(sample_sns_type sns_type, hi_vi_dev vi_dev, sample_mipi_info *mipi_info)
{
    mipi_info->mipi_dev    = vi_dev;
    mipi_info->divide_mode = LANE_DIVIDE_MODE_1;
    sample_comm_vi_get_mipi_attr_by_dev_id(sns_type, vi_dev, &mipi_info->combo_dev_attr);
    sample_comm_vi_get_mipi_ext_data_attr(sns_type, &mipi_info->ext_data_type_attr);
    mipi_info->ext_data_type_attr.devno = vi_dev;
}

hi_void sample_comm_vi_get_default_dev_info(sample_sns_type sns_type, sample_vi_dev_info *dev_info)
{
    hi_size size;
    hi_u32 ob_height;

    dev_info->vi_dev = 0;
    sample_comm_vi_get_dev_attr_by_intf_mode(HI_VI_INTF_MODE_MIPI, &dev_info->dev_attr);
    sample_comm_vi_get_size_by_sns_type(sns_type, &size);

    ob_height = sample_comm_vi_get_obheight_by_sns_type(sns_type);
    dev_info->dev_attr.in_size.width  = size.width;
    dev_info->dev_attr.in_size.height = size.height + ob_height;
    dev_info->bas_attr.enable = HI_FALSE;
}

static hi_void sample_comm_vi_get_default_bind_info(sample_sns_type sns_type, hi_vi_bind_pipe *bind_pipe)
{
    hi_u32 i;

    bind_pipe->pipe_num = sample_comm_vi_get_pipe_num_by_sns_type(sns_type);
    for (i = 0; i < bind_pipe->pipe_num; i++) {
        bind_pipe->pipe_id[i] = i;
    }
}

static hi_void sample_comm_vi_get_default_grp_info(sample_sns_type sns_type, sample_vi_grp_info *grp_info)
{
    hi_u32 i;
    hi_u32 pipe_num;
    hi_size size;

    sample_comm_vi_get_size_by_sns_type(sns_type, &size);
    grp_info->grp_num = 1;
    grp_info->fusion_grp[0] = 0;

    grp_info->fusion_grp_attr[0].wdr_mode = sample_comm_vi_get_wdr_mode_by_sns_type(sns_type);
    grp_info->fusion_grp_attr[0].cache_line = size.height;
    pipe_num = sample_comm_vi_get_pipe_num_by_sns_type(sns_type);
    for (i = 0; i < pipe_num; i++) {
        grp_info->fusion_grp_attr[0].pipe_id[i] = i;
    }
}

hi_void sample_comm_vi_get_default_pipe_info(sample_sns_type sns_type, hi_vi_bind_pipe *bind_pipe,
                                             sample_vi_pipe_info pipe_info[])
{
    hi_u32 i;
    hi_size size;

    sample_comm_vi_get_size_by_sns_type(sns_type, &size);
    for (i = 0; i < bind_pipe->pipe_num; i++) {
        /* pipe attr */
        pipe_info[i].pipe_attr.pipe_bypass_mode               = HI_VI_PIPE_BYPASS_NONE;
        pipe_info[i].pipe_attr.isp_bypass                     = HI_FALSE;
        pipe_info[i].pipe_attr.size.width                     = size.width;
        pipe_info[i].pipe_attr.size.height                    = size.height;
        pipe_info[i].pipe_attr.pixel_format                   = HI_PIXEL_FORMAT_RGB_BAYER_12BPP;
        pipe_info[i].pipe_attr.compress_mode                  = HI_COMPRESS_MODE_LINE;
        pipe_info[i].pipe_attr.frame_rate_ctrl.src_frame_rate = -1;
        pipe_info[i].pipe_attr.frame_rate_ctrl.dst_frame_rate = -1;

        pipe_info[i].pipe_need_start = HI_TRUE;
        pipe_info[i].isp_need_run = HI_TRUE;
        pipe_info[i].isp_quick_start = HI_FALSE;

        if (i == 0) {
            pipe_info[i].is_master_pipe = HI_TRUE;
        }

        /* pub attr */
        sample_comm_isp_get_pub_attr_by_sns(sns_type, &pipe_info[i].isp_info.isp_pub_attr);

        pipe_info[i].nr_attr.enable = HI_TRUE;
        pipe_info[i].nr_attr.compress_mode = HI_COMPRESS_MODE_FRAME;
        pipe_info[i].nr_attr.nr_type = HI_NR_TYPE_VIDEO_NORM;
        pipe_info[i].nr_attr.nr_motion_mode = HI_NR_MOTION_MODE_NORM;
        pipe_info[i].pixel_rate = sample_comm_vi_get_sensor_pixel_rate_by_type(sns_type);

        /* chn info */
        pipe_info[i].chn_num = 1;
        pipe_info[i].chn_info[0].vi_chn                                  = 0;
        pipe_info[i].chn_info[0].fmu_mode                                = HI_FMU_MODE_OFF;
        pipe_info[i].chn_info[0].chn_attr.size.width                     = size.width;
        pipe_info[i].chn_info[0].chn_attr.size.height                    = size.height;
        pipe_info[i].chn_info[0].chn_attr.pixel_format                   = HI_PIXEL_FORMAT_YVU_SEMIPLANAR_420;
        pipe_info[i].chn_info[0].chn_attr.dynamic_range                  = HI_DYNAMIC_RANGE_SDR8;
        pipe_info[i].chn_info[0].chn_attr.video_format                   = HI_VIDEO_FORMAT_LINEAR;
        pipe_info[i].chn_info[0].chn_attr.compress_mode                  = HI_COMPRESS_MODE_NONE;
        pipe_info[i].chn_info[0].chn_attr.mirror_en                      = HI_FALSE;
        pipe_info[i].chn_info[0].chn_attr.flip_en                        = HI_FALSE;
        pipe_info[i].chn_info[0].chn_attr.depth                          = 0;
        pipe_info[i].chn_info[0].chn_attr.frame_rate_ctrl.src_frame_rate = -1;
        pipe_info[i].chn_info[0].chn_attr.frame_rate_ctrl.dst_frame_rate = -1;
    }
}

hi_void sample_comm_vi_get_default_vi_cfg(sample_sns_type sns_type, sample_vi_cfg *vi_cfg)
{
    (hi_void)memset_s(vi_cfg, sizeof(sample_vi_cfg), 0, sizeof(sample_vi_cfg));

    /* sensor info */
    sample_comm_vi_get_default_sns_info(sns_type, &vi_cfg->sns_info);
    /* mipi info */
    sample_comm_vi_get_default_mipi_info(sns_type, &vi_cfg->mipi_info);
    /* dev info */
    sample_comm_vi_get_default_dev_info(sns_type, &vi_cfg->dev_info);
    /* bind info */
    sample_comm_vi_get_default_bind_info(sns_type, &vi_cfg->bind_pipe);
    /* grp info */
    sample_comm_vi_get_default_grp_info(sns_type, &vi_cfg->grp_info);
    /* pipe info */
    sample_comm_vi_get_default_pipe_info(sns_type, &vi_cfg->bind_pipe, vi_cfg->pipe_info);
}

hi_void sample_comm_vi_get_vi_cfg_by_fmu_mode(sample_sns_type sns_type, hi_fmu_mode fum_mode, sample_vi_cfg *vi_cfg)
{
    hi_u32 i;

    sample_comm_vi_get_default_vi_cfg(sns_type, vi_cfg);
    for (i = 0; i < vi_cfg->bind_pipe.pipe_num; i++) {
        vi_cfg->pipe_info[i].chn_info[0].chn_attr.compress_mode = HI_COMPRESS_MODE_NONE;
        vi_cfg->pipe_info[i].chn_info[0].chn_attr.video_format = HI_VIDEO_FORMAT_LINEAR;
    }
}

hi_void sample_comm_vi_init_vi_cfg(sample_sns_type sns_type, hi_size *size, sample_vi_cfg *vi_cfg)
{
    hi_u32 i;
    sample_vi_pipe_info *pipe_info = vi_cfg->pipe_info;
    hi_vi_bind_pipe *bind_pipe = &vi_cfg->bind_pipe;

    sample_comm_vi_get_default_vi_cfg(sns_type, vi_cfg);
    for (i = 0; i < bind_pipe->pipe_num; i++) {
        pipe_info[i].pipe_attr.size.width            = size->width;
        pipe_info[i].pipe_attr.size.height           = size->height;
        pipe_info[i].chn_info[0].chn_attr.size.width = size->width;
        pipe_info[i].chn_info[0].chn_attr.size.height= size->height;
    }
}

hi_s32 sample_comm_vi_set_vi_vpss_mode(hi_vi_vpss_mode_type mode_type, hi_vi_aiisp_mode aiisp_mode)
{
    hi_u32 i;
    hi_s32 ret;
    hi_vi_vpss_mode_type other_pipe_mode_type;
    hi_vi_vpss_mode vi_vpss_mode;

    if (mode_type == HI_VI_OFFLINE_VPSS_ONLINE) {
        other_pipe_mode_type = HI_VI_OFFLINE_VPSS_ONLINE;
    } else {
        other_pipe_mode_type = HI_VI_OFFLINE_VPSS_OFFLINE;
    }

    vi_vpss_mode.mode[0] = mode_type;
    for (i = 1; i < HI_VI_MAX_PIPE_NUM; i++) {
        vi_vpss_mode.mode[i] = other_pipe_mode_type;
    }

    ret = hi_mpi_sys_set_vi_vpss_mode(&vi_vpss_mode);
    if (ret != HI_SUCCESS) {
        sample_print("set vi vpss mode failed!\n");
        return HI_FAILURE;
    }

    ret = hi_mpi_sys_set_vi_aiisp_mode(0, aiisp_mode); /* only pipe0 can set aiisp other mode */
    if (ret != HI_SUCCESS) {
        sample_print("set vi video mode failed!\n");
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

hi_s32 sample_comm_vi_set_mipi_hs_mode(lane_divide_mode_t hs_mode)
{
    hi_s32 fd;
    hi_s32 ret;

    fd = open(MIPI_DEV_NAME, O_RDWR);
    if (fd < 0) {
        sample_print("open %s failed!\n", MIPI_DEV_NAME);
        return HI_FAILURE;
    }

    ret = ioctl(fd, HI_MIPI_SET_HS_MODE, &hs_mode);

    close(fd);

    return ret;
}

hi_s32 sample_comm_vi_mipi_ctrl_cmd(hi_u32 devno, hi_u32 cmd)
{
    hi_s32 ret;
    hi_s32 fd;

    fd = open(MIPI_DEV_NAME, O_RDWR);
    if (fd < 0) {
        sample_print("open %s failed!\n", MIPI_DEV_NAME);
        return HI_FAILURE;
    }

    ret = ioctl(fd, cmd, &devno);

    close(fd);

    return ret;
}

static hi_s32 sample_comm_vi_set_mipi_combo_attr(const combo_dev_attr_t *combo_dev_attr)
{
    hi_s32 fd;
    hi_s32 ret;

    fd = open(MIPI_DEV_NAME, O_RDWR);
    if (fd < 0) {
        sample_print("open %s failed!\n", MIPI_DEV_NAME);
        return HI_FAILURE;
    }

    ret = ioctl(fd, HI_MIPI_SET_DEV_ATTR, combo_dev_attr);

    close(fd);

    return ret;
}

static hi_s32 sample_comm_vi_set_mipi_ext_data_type_attr(const ext_data_type_t *ext_data_type_attr)
{
    hi_s32 fd;
    hi_s32 ret;

    fd = open(MIPI_DEV_NAME, O_RDWR);
    if (fd < 0) {
        sample_print("open %s failed!\n", MIPI_DEV_NAME);
        return HI_FAILURE;
    }

    ret = ioctl(fd, HI_MIPI_SET_EXT_DATA_TYPE, ext_data_type_attr);

    close(fd);

    return ret;
}

hi_s32 sample_comm_vi_start_sensor(const sample_sns_info *sns_info, const sample_mipi_info *mipi_info)
{
    hi_s32 ret;
    ret = sample_comm_vi_set_mipi_hs_mode(mipi_info->divide_mode);
    if (ret != HI_SUCCESS) {
        sample_print("mipi rx set hs_mode failed!\n");
        return HI_FAILURE;
    }

    ret = sample_comm_vi_mipi_ctrl_cmd(sns_info->sns_clk_src, HI_MIPI_ENABLE_SENSOR_CLOCK);
    if (ret != HI_SUCCESS) {
        sample_print("devno %u enable sensor clock failed!\n", sns_info->sns_clk_src);
        return HI_FAILURE;
    }

    ret = sample_comm_vi_mipi_ctrl_cmd(sns_info->sns_rst_src, HI_MIPI_RESET_SENSOR);
    if (ret != HI_SUCCESS) {
        sample_print("devno %u reset sensor failed!\n", sns_info->sns_rst_src);
        return HI_FAILURE;
    }

    ret = sample_comm_vi_mipi_ctrl_cmd(sns_info->sns_rst_src, HI_MIPI_UNRESET_SENSOR);
    if (ret != HI_SUCCESS) {
        sample_print("devno %u unreset sensor failed!\n", sns_info->sns_rst_src);
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

static hi_s32 sample_comm_vi_start_mipi_rx(const sample_sns_info *sns_info, const sample_mipi_info *mipi_info)
{
    hi_s32 ret;

    if (sns_info->sns_clk_rst_en) {
        sample_comm_vi_start_sensor(sns_info, mipi_info);
    }

    ret = sample_comm_vi_mipi_ctrl_cmd(mipi_info->mipi_dev, HI_MIPI_ENABLE_MIPI_CLOCK);
    if (ret != HI_SUCCESS) {
        sample_print("devno %d enable mipi rx clock failed!\n", mipi_info->mipi_dev);
        return HI_FAILURE;
    }

    ret = sample_comm_vi_mipi_ctrl_cmd(mipi_info->mipi_dev, HI_MIPI_RESET_MIPI);
    if (ret != HI_SUCCESS) {
        sample_print("devno %d reset mipi rx failed!\n", mipi_info->mipi_dev);
        return HI_FAILURE;
    }

    ret = sample_comm_vi_set_mipi_combo_attr(&mipi_info->combo_dev_attr);
    if (ret != HI_SUCCESS) {
        sample_print("mipi rx set combo attr failed!\n");
        return HI_FAILURE;
    }

    ret = sample_comm_vi_set_mipi_ext_data_type_attr(&mipi_info->ext_data_type_attr);
    if (ret != HI_SUCCESS) {
        sample_print("mipi rx set ext data attr failed!\n");
        return HI_FAILURE;
    }

    ret = sample_comm_vi_mipi_ctrl_cmd(mipi_info->mipi_dev, HI_MIPI_UNRESET_MIPI);
    if (ret != HI_SUCCESS) {
        sample_print("devno %d unreset mipi rx failed!\n", mipi_info->mipi_dev);
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

hi_void sample_comm_vi_stop_mipi_rx(const sample_sns_info *sns_info, const sample_mipi_info *mipi_info)
{
    hi_s32 ret;

    ret = sample_comm_vi_mipi_ctrl_cmd(mipi_info->mipi_dev, HI_MIPI_RESET_MIPI);
    if (ret != HI_SUCCESS) {
        sample_print("devno %d reset mipi rx failed!\n", mipi_info->mipi_dev);
    }

    ret = sample_comm_vi_mipi_ctrl_cmd(mipi_info->mipi_dev, HI_MIPI_DISABLE_MIPI_CLOCK);
    if (ret != HI_SUCCESS) {
        sample_print("devno %d disable mipi rx clock failed!\n", mipi_info->mipi_dev);
    }

    ret = sample_comm_vi_mipi_ctrl_cmd(sns_info->sns_rst_src, HI_MIPI_RESET_SENSOR);
    if (ret != HI_SUCCESS) {
        sample_print("devno %u reset sensor failed!\n", sns_info->sns_rst_src);
    }

    ret = sample_comm_vi_mipi_ctrl_cmd(sns_info->sns_clk_src, HI_MIPI_DISABLE_SENSOR_CLOCK);
    if (ret != HI_SUCCESS) {
        sample_print("devno %u disable sensor clock failed!\n", sns_info->sns_clk_src);
    }
}

static hi_s32 sample_comm_vi_start_dev(hi_vi_dev vi_dev, const hi_vi_dev_attr *dev_attr)
{
    hi_s32 ret;

    ret = hi_mpi_vi_set_dev_attr(vi_dev, dev_attr);
    if (ret != HI_SUCCESS) {
        sample_print("vi set dev attr failed with 0x%x!\n", ret);
        return HI_FAILURE;
    }

    ret = hi_mpi_vi_enable_dev(vi_dev);
    if (ret != HI_SUCCESS) {
        sample_print("vi enable dev failed with 0x%x!\n", ret);
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

static hi_void sample_comm_vi_stop_dev(hi_vi_dev vi_dev)
{
    hi_s32 ret;

    ret = hi_mpi_vi_disable_dev(vi_dev);
    if (ret != HI_SUCCESS) {
        sample_print("vi disable dev failed with 0x%x!\n", ret);
    }
}

static hi_s32 sample_comm_vi_dev_bind_pipe(hi_vi_dev vi_dev, const hi_vi_bind_pipe *bind_pipe)
{
    hi_u32 i;
    hi_s32 j;
    hi_s32 ret;

    for (i = 0; i < bind_pipe->pipe_num; i++) {
        ret = hi_mpi_vi_bind(vi_dev, bind_pipe->pipe_id[i]);
        if (ret != HI_SUCCESS) {
            sample_print("vi dev(%d) bind pipe(%d) failed!\n", vi_dev, bind_pipe->pipe_id[i]);
            goto exit;
        }
    }

    return HI_SUCCESS;

exit:
    for (j = (hi_s32)i - 1; j >= 0; j--) {
        ret = hi_mpi_vi_unbind(vi_dev, bind_pipe->pipe_id[j]);
        if (ret != HI_SUCCESS) {
            sample_print("vi dev(%d) unbind pipe(%d) failed!\n", vi_dev, bind_pipe->pipe_id[j]);
        }
    }
    return HI_FAILURE;
}

static hi_void sample_comm_vi_dev_unbind_pipe(hi_vi_dev vi_dev, const hi_vi_bind_pipe *bind_pipe)
{
    hi_u32 i;
    hi_s32 ret;

    for (i = 0; i < bind_pipe->pipe_num; i++) {
        ret = hi_mpi_vi_unbind(vi_dev, bind_pipe->pipe_id[i]);
        if (ret != HI_SUCCESS) {
            sample_print("vi dev(%d) unbind pipe(%d) failed!\n", vi_dev, bind_pipe->pipe_id[i]);
        }
    }
}

static hi_s32 sample_comm_vi_set_grp_info(const sample_vi_grp_info *grp_info)
{
    hi_s32 ret;
    hi_u32 i;
    for (i = 0; i < grp_info->grp_num; i++) {
        ret = hi_mpi_vi_set_wdr_fusion_grp_attr(grp_info->fusion_grp[i], &grp_info->fusion_grp_attr[i]);
        if (ret != HI_SUCCESS) {
            sample_print("vi set wdr fusion grp attr failed!\n");
            return HI_FAILURE;
        }
    }

    return HI_SUCCESS;
}

static hi_s32 sample_comm_vi_stop_chn(hi_vi_pipe vi_pipe, const sample_vi_chn_info chn_info[], hi_u32 chn_num)
{
    hi_u32 i;
    hi_s32 ret;

    for (i = 0; i < chn_num; i++) {
        hi_vi_chn vi_chn = chn_info[i].vi_chn;

        ret = hi_mpi_vi_disable_chn(vi_pipe, vi_chn);
        if (ret != HI_SUCCESS) {
            sample_print("vi disable chn(%d) failed with 0x%x!\n", vi_chn, ret);
            return HI_FAILURE;
        }
    }

    return HI_SUCCESS;
}

static hi_s32 sample_comm_vi_start_chn(hi_vi_pipe vi_pipe, const sample_vi_pipe_info *pipe_info)
{
    hi_u32 i;
    hi_s32 ret;
    hi_u32 chn_num = pipe_info->chn_num;
    const sample_vi_chn_info *chn_info = pipe_info->chn_info;

    for (i = 0; i < chn_num; i++) {
        hi_vi_chn vi_chn = chn_info[i].vi_chn;
        const hi_vi_chn_attr *chn_attr = &chn_info[i].chn_attr;

        ret = hi_mpi_vi_set_chn_attr(vi_pipe, vi_chn, chn_attr);
        if (ret != HI_SUCCESS) {
            sample_print("vi set chn(%d) attr failed with 0x%x!\n", vi_chn, ret);
            return HI_FAILURE;
        }

        ret = hi_mpi_vi_enable_chn(vi_pipe, vi_chn);
        if (ret != HI_SUCCESS) {
            sample_print("vi enable chn(%d) failed with 0x%x!\n", vi_chn, ret);
            return HI_FAILURE;
        }
    }

    if (pipe_info->nr_attr.enable == HI_TRUE) {
        ret = hi_mpi_vi_set_pipe_3dnr_attr(vi_pipe, &pipe_info->nr_attr);
        if (ret != HI_SUCCESS) {
            sample_print("vi pipe(%d) set 3dnr_attr failed!\n", vi_pipe);
            sample_comm_vi_stop_chn(vi_pipe, chn_info, chn_num);
            return HI_FAILURE;
        }
    }

    return HI_SUCCESS;
}

static hi_s32 sample_comm_vi_switch_mode_start_chn(hi_vi_pipe vi_pipe,
                                                   const sample_vi_chn_info chn_info[],
                                                   hi_u32 chn_num)
{
    hi_u32 i;
    hi_s32 ret;

    for (i = 0; i < chn_num; i++) {
        hi_vi_chn vi_chn = chn_info[i].vi_chn;
        const hi_vi_chn_attr *chn_attr = &chn_info[i].chn_attr;

        ret = hi_mpi_vi_set_chn_attr(vi_pipe, vi_chn, chn_attr);
        if (ret != HI_SUCCESS) {
            sample_print("vi set chn(%d) attr failed with 0x%x!\n", vi_chn, ret);
            return HI_FAILURE;
        }
    }

    return HI_SUCCESS;
}

static hi_s32 sample_comm_vi_set_pipe_pixel_rate(hi_vi_pipe vi_pipe, hi_u32 pixel_rate)
{
    hi_vi_vpss_mode vi_vpss_mode;
    hi_s32 ret = hi_mpi_sys_get_vi_vpss_mode(&vi_vpss_mode);
    if (ret != HI_SUCCESS) {
        sample_print("pipe%d get vi_vpss_mode failed!", vi_pipe);
        return ret;
    }

    if (vi_vpss_mode.mode[vi_pipe] != HI_VI_ONLINE_VPSS_ONLINE &&
        vi_vpss_mode.mode[vi_pipe] != HI_VI_ONLINE_VPSS_OFFLINE) {
        return HI_SUCCESS; /* only vi online support set pixel_rate */
    }

    if (pixel_rate == 0) {
        sample_print("pipe%d sns_type not adapt pixel_rate calc!", vi_pipe);
        return HI_SUCCESS; /* only vi online support set pixel_rate */
    }

    return hi_mpi_vi_set_pipe_online_clock(vi_pipe, pixel_rate);
}

static hi_s32 sample_comm_vi_init_one_pipe(hi_vi_pipe vi_pipe, const sample_vi_pipe_info *pipe_info)
{
    hi_s32 ret;

    if (pipe_info->bnr_bnf_num != 0) {
        ret = hi_mpi_vi_set_pipe_bnr_buf_num(vi_pipe, pipe_info->bnr_bnf_num);
        if (ret != HI_SUCCESS) {
            sample_print("vi set pipe(%d) bnr_buf_num failed with %#x!\n", vi_pipe, ret);
            return HI_FAILURE;
        }
    }

    if (pipe_info->wrap_attr.enable) {
        ret = hi_mpi_vi_set_pipe_buf_wrap_attr(vi_pipe, &pipe_info->wrap_attr);
        if (ret != HI_SUCCESS) {
            sample_print("vi set pipe(%d) buf_wrap_attr failed with %#x!\n", vi_pipe, ret);
            return HI_FAILURE;
        }
    }

    ret = hi_mpi_vi_create_pipe(vi_pipe, &pipe_info->pipe_attr);
    if (ret != HI_SUCCESS) {
        sample_print("vi create pipe(%d) failed with 0x%x!\n", vi_pipe, ret);
        return HI_FAILURE;
    }

    ret = sample_comm_vi_set_pipe_pixel_rate(vi_pipe, pipe_info->pixel_rate);
    if (ret != HI_SUCCESS) {
        sample_print("pipe%d set online clock fail!", vi_pipe);
        goto start_pipe_failed;
    }

    if (pipe_info->vc_change_en) {
        ret = hi_mpi_vi_set_pipe_vc_number(vi_pipe, pipe_info->vc_number);
        if (ret != HI_SUCCESS) {
            sample_print("vi set pipe(%d) vc_number failed with %#x!\n", vi_pipe, ret);
            goto start_pipe_failed;
        }
    }

    return HI_SUCCESS;

start_pipe_failed:
    hi_mpi_vi_destroy_pipe(vi_pipe);
    return HI_FAILURE;
}

static hi_s32 sample_comm_vi_start_one_pipe(hi_vi_pipe vi_pipe, const sample_vi_pipe_info *pipe_info,
    hi_bool is_master_pipe)
{
    hi_s32 ret = sample_comm_vi_init_one_pipe(vi_pipe, pipe_info);
    if (ret != HI_SUCCESS) {
        return HI_FAILURE;
    }

    if (pipe_info->pipe_need_start == HI_TRUE) {
        ret = hi_mpi_vi_start_pipe(vi_pipe);
        if (ret != HI_SUCCESS) {
            sample_print("vi start pipe(%d) failed with 0x%x!\n", vi_pipe, ret);
            goto start_pipe_failed;
        }
    }

    if (is_master_pipe != HI_TRUE) {
        return HI_SUCCESS;
    }

    ret = sample_comm_vi_start_chn(vi_pipe, pipe_info);
    if (ret != HI_SUCCESS) {
        sample_print("vi pipe(%d) start chn failed!\n", vi_pipe);
        goto start_chn_failed;
    }

    return HI_SUCCESS;

start_chn_failed:
    hi_mpi_vi_stop_pipe(vi_pipe);
start_pipe_failed:
    hi_mpi_vi_destroy_pipe(vi_pipe);
    return HI_FAILURE;
}

static hi_void sample_comm_vi_stop_one_pipe(hi_vi_pipe vi_pipe, const sample_vi_pipe_info *pipe_info,
    hi_bool is_master_pipe)
{
    hi_s32 ret;

    if (is_master_pipe == HI_TRUE) {
        ret = sample_comm_vi_stop_chn(vi_pipe, pipe_info->chn_info, pipe_info->chn_num);
        if (ret != HI_SUCCESS) {
            sample_print("vi pipe(%d) stop chn failed!\n", vi_pipe);
        }
    }

    ret = hi_mpi_vi_stop_pipe(vi_pipe);
    if (ret != HI_SUCCESS) {
        sample_print("vi stop pipe(%d) failed with 0x%x!\n", vi_pipe, ret);
    }

    ret = hi_mpi_vi_destroy_pipe(vi_pipe);
    if (ret != HI_SUCCESS) {
        sample_print("vi destroy pipe(%d) failed with 0x%x!\n", vi_pipe, ret);
    }
}

static hi_s32 sample_comm_vi_start_pipe(const hi_vi_bind_pipe *bind_pipe, const sample_vi_pipe_info pipe_info[])
{
    hi_s32 i;
    hi_s32 ret;

    for (i = 0; i < (hi_s32)bind_pipe->pipe_num; i++) {
        hi_vi_pipe vi_pipe = bind_pipe->pipe_id[i];
        hi_bool is_master_pipe = pipe_info[i].is_master_pipe;
        ret = sample_comm_vi_start_one_pipe(vi_pipe, &pipe_info[i], is_master_pipe);
        if (ret != HI_SUCCESS) {
            goto exit;
        }
    }

    return HI_SUCCESS;

exit:
    for (i = i - 1; i >= 0; i--) {
        hi_vi_pipe vi_pipe = bind_pipe->pipe_id[i];
        hi_bool is_master_pipe = pipe_info[i].is_master_pipe;
        sample_comm_vi_stop_one_pipe(vi_pipe, &pipe_info[i], is_master_pipe);
    }
    return HI_FAILURE;
}

static hi_void sample_comm_vi_stop_pipe(const hi_vi_bind_pipe *bind_pipe, const sample_vi_pipe_info pipe_info[])
{
    hi_u32 i;
    for (i = 0; i < bind_pipe->pipe_num; i++) {
        hi_vi_pipe vi_pipe = bind_pipe->pipe_id[i];
        hi_bool is_master_pipe = pipe_info[i].is_master_pipe;
        sample_comm_vi_stop_one_pipe(vi_pipe, &pipe_info[i], is_master_pipe);
    }
}

static hi_s32 sample_comm_vi_register_sensor_lib(hi_vi_pipe vi_pipe, hi_u8 pipe_index, const sample_vi_cfg *vi_cfg)
{
    hi_s32 ret;
    hi_u32 bus_id;
    sample_sns_type sns_type = vi_cfg->sns_info.sns_type;

    ret = sample_comm_isp_sensor_regiter_callback(vi_pipe, sns_type);
    if (ret != HI_SUCCESS) {
        printf("register sensor to ISP %d failed\n", vi_pipe);
        return HI_FAILURE;
    }

    if (pipe_index > 0) {
        bus_id = -1;
    } else {
        bus_id = vi_cfg->sns_info.bus_id;
    }

    ret = sample_comm_isp_bind_sns(vi_pipe, sns_type, bus_id);
    if (ret != HI_SUCCESS) {
        printf("register sensor bus id %u failed\n", bus_id);
        goto exit0;
    }

    ret = sample_comm_isp_ae_lib_callback(vi_pipe);
    if (ret != HI_SUCCESS) {
        printf("isp_mst_comm_ae_lib_callback failed\n");
        goto exit0;
    }

    ret = sample_comm_isp_awb_lib_callback(vi_pipe);
    if (ret != HI_SUCCESS) {
        printf("isp_mst_comm_awb_lib_callback failed\n");
        goto exit1;
    }

    return HI_SUCCESS;

exit1:
    sample_comm_isp_ae_lib_uncallback(vi_pipe);
exit0:
    sample_comm_isp_sensor_unregiter_callback(vi_pipe);
    return ret;
}

static hi_void sample_comm_vi_deregister_sensor_lib(hi_vi_pipe vi_pipe, const sample_vi_cfg *vi_cfg)
{
    // ISP use it
    sample_comm_isp_awb_lib_uncallback(vi_pipe);
    sample_comm_isp_ae_lib_uncallback(vi_pipe);
    sample_comm_isp_sensor_unregiter_callback(vi_pipe);
}

static hi_void sample_comm_vi_set_isp_ctrl_param(hi_vi_pipe vi_pipe, hi_bool is_isp_be_end_trigger,
    hi_bool is_isp_quick_start)
{
    hi_isp_ctrl_param isp_ctrl_param = {};

    if (hi_mpi_isp_get_ctrl_param(vi_pipe, &isp_ctrl_param) != HI_SUCCESS) {
        printf("vi_pipe = %d get isp ctrl para failed!\n", vi_pipe);
        return;
    }

    isp_ctrl_param.isp_run_wakeup_select = (is_isp_be_end_trigger) ?
        OT_ISP_RUN_WAKEUP_BE_END : OT_ISP_RUN_WAKEUP_FE_START;
    isp_ctrl_param.quick_start_en = is_isp_quick_start;

    if (hi_mpi_isp_set_ctrl_param(vi_pipe, &isp_ctrl_param) != HI_SUCCESS) {
        printf("vi_pipe = %d wakeup select be end failed!\n", vi_pipe);
    }
}

static hi_s32 sample_comm_vi_start_one_pipe_isp(hi_vi_pipe vi_pipe, hi_u8 pipe_index, const sample_vi_cfg *vi_cfg)
{
    hi_s32 ret;

    ret = sample_comm_vi_register_sensor_lib(vi_pipe, pipe_index, vi_cfg);
    if (ret != HI_SUCCESS) {
        printf("register sensor to ISP %d failed\n", vi_pipe);
        return HI_FAILURE;
    }

    sample_comm_vi_set_isp_ctrl_param(vi_pipe, vi_cfg->pipe_info[pipe_index].isp_be_end_trigger,
        vi_cfg->pipe_info[pipe_index].isp_quick_start);

    ret = hi_mpi_isp_mem_init(vi_pipe);
    if (ret != HI_SUCCESS) {
        printf("HI_MPI_ISP_MemInit failed with 0x%x!\n", ret);
        goto exit0;
    }

    ret = hi_mpi_isp_set_pub_attr(vi_pipe, &vi_cfg->pipe_info[pipe_index].isp_info.isp_pub_attr);
    if (ret != HI_SUCCESS) {
        printf("HI_MPI_ISP_SetPubAttr failed with 0x%x!\n", ret);
        goto exit1;
    }

    ret = hi_mpi_isp_init(vi_pipe);
    if (ret != HI_SUCCESS) {
        printf("HI_MPI_ISP_Init failed with 0x%x!\n", ret);
        return -1;
    }

    if ((vi_pipe < HI_VI_MAX_PHYS_PIPE_NUM ||
        (vi_cfg->pipe_info[pipe_index].isp_be_end_trigger == HI_TRUE && vi_pipe < HI_VI_MAX_PIPE_NUM)) &&
        (vi_cfg->pipe_info[pipe_index].isp_need_run == HI_TRUE)) {
        ret = sample_comm_isp_run(vi_pipe);
        if (ret != HI_SUCCESS) {
            printf("ISP Run failed with 0x%x!\n", ret);
            goto exit1;
        }
    }

    g_start_isp[vi_pipe] = HI_TRUE;

    return HI_SUCCESS;

exit1:
    hi_mpi_isp_exit(vi_pipe);
exit0:
    sample_comm_vi_deregister_sensor_lib(vi_pipe, vi_cfg);
    return ret;
}

static hi_void sample_comm_vi_stop_one_pipe_isp(hi_vi_pipe vi_pipe, const sample_vi_cfg *vi_cfg)
{
    hi_mpi_isp_exit(vi_pipe);
    sample_comm_isp_stop(vi_pipe);
    sample_comm_vi_deregister_sensor_lib(vi_pipe, vi_cfg);

    g_start_isp[vi_pipe] = HI_FALSE;
}

hi_s32 sample_comm_vi_get_isp_run_state(hi_bool *isp_states, hi_u32 size)
{
    if (isp_states == HI_NULL) {
        sample_print("isp_states is NULL\n");
        return HI_FAILURE;
    }
    if (size < HI_VI_MAX_PIPE_NUM) {
        sample_print("array size smaller than %d\n", HI_VI_MAX_PIPE_NUM);
        return HI_FAILURE;
    }
    hi_s32 ret = memcpy_s(isp_states, size * sizeof(hi_bool), g_start_isp, HI_VI_MAX_PIPE_NUM * sizeof(hi_bool));
    if (ret != EOK) {
        sample_print("memcpy_s isp states fail %x\n", ret);
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

static hi_s32 sample_comm_vi_get_wdr_num(hi_wdr_mode wdr_mode)
{
    hi_u32 pipe_num = 1;
    switch (wdr_mode) {
        case HI_WDR_MODE_NONE:
        case HI_WDR_MODE_BUILT_IN:
            pipe_num = 1;
            break;
        case HI_WDR_MODE_2To1_LINE:
        case HI_WDR_MODE_2To1_FRAME:
            pipe_num = 2; /* WDR 2 pipe */
            break;
        case HI_WDR_MODE_3To1_LINE:
            pipe_num = 3; /* WDR 3 pipe */
            break;
        default:
            break;
    }
    return pipe_num;
}

hi_s32 sample_comm_vi_start_isp(const sample_vi_cfg *vi_cfg)
{
    hi_s8 i, j;
    hi_s32 ret;
    hi_vi_pipe vi_pipe;
    hi_wdr_mode wdr_mode = vi_cfg->grp_info.fusion_grp_attr[0].wdr_mode;
    td_bool pipe_reverse = vi_cfg->grp_info.fusion_grp_attr[0].pipe_reverse;
    const hi_vi_pipe *pipe_id = vi_cfg->bind_pipe.pipe_id;

    for (i = 0; i < (hi_u8)vi_cfg->bind_pipe.pipe_num; i++) {
        vi_pipe = pipe_reverse ? pipe_id[vi_cfg->bind_pipe.pipe_num - 1 - i] : pipe_id[i];

        if (vi_cfg->pipe_info[i].pipe_attr.isp_bypass == HI_TRUE) {
            continue;
        }

        if ((wdr_mode != HI_WDR_MODE_NONE) && (wdr_mode != HI_WDR_MODE_BUILT_IN) &&
            (i > 0) && (i < sample_comm_vi_get_wdr_num(wdr_mode))) {
            continue;
        }

        ret = sample_comm_vi_start_one_pipe_isp(vi_pipe, i, vi_cfg);
        if (ret != HI_SUCCESS) {
            for (j = i - 1; (j >= 0) && (i != 0); j--) {
                vi_pipe = pipe_reverse ? pipe_id[vi_cfg->bind_pipe.pipe_num - 1 - j] : pipe_id[j];
                sample_comm_vi_stop_one_pipe_isp(vi_pipe, vi_cfg);
            }
            return ret;
        }
    }

    return HI_SUCCESS;
}

hi_void sample_comm_vi_stop_isp(const sample_vi_cfg *vi_cfg)
{
    hi_u32     i;
    hi_bool    start_pipe;
    hi_vi_pipe vi_pipe;
    td_bool pipe_reverse = vi_cfg->grp_info.fusion_grp_attr[0].pipe_reverse;
    const hi_vi_pipe *pipe_id = vi_cfg->bind_pipe.pipe_id;

    for (i = 0; i < vi_cfg->bind_pipe.pipe_num; i++) {
        if (vi_cfg->pipe_info[i].pipe_attr.isp_bypass == HI_TRUE) {
            continue;
        }

        if ((vi_cfg->pipe_info[i].isp_info.isp_pub_attr.wdr_mode == HI_WDR_MODE_NONE) ||
            (vi_cfg->pipe_info[i].isp_info.isp_pub_attr.wdr_mode == HI_WDR_MODE_BUILT_IN)) {
            start_pipe = HI_TRUE;
        } else {
            start_pipe = (i > 0) ? HI_FALSE : HI_TRUE;
        }

        if (start_pipe != HI_TRUE) {
            continue;
        }

        vi_pipe = pipe_reverse ? pipe_id[vi_cfg->bind_pipe.pipe_num - 1 - i] : pipe_id[i];
        sample_comm_vi_stop_one_pipe_isp(vi_pipe, vi_cfg);
    }
}

hi_s32 sample_comm_vi_start_vi(const sample_vi_cfg *vi_cfg)
{
    hi_s32 ret;
    hi_vi_dev vi_dev;

    ret = sample_comm_vi_start_mipi_rx(&vi_cfg->sns_info, &vi_cfg->mipi_info);
    if (ret != HI_SUCCESS) {
        sample_print("start mipi rx failed!\n");
        goto start_mipi_rx_failed;
    }

    vi_dev   = vi_cfg->dev_info.vi_dev;
    ret = sample_comm_vi_start_dev(vi_dev, &vi_cfg->dev_info.dev_attr);
    if (ret != HI_SUCCESS) {
        sample_print("start dev failed!\n");
        goto start_dev_failed;
    }

    ret = sample_comm_vi_dev_bind_pipe(vi_dev, &vi_cfg->bind_pipe);
    if (ret != HI_SUCCESS) {
        sample_print("dev bind pipe failed!\n");
        goto dev_bind_pipe_failed;
    }

    ret = sample_comm_vi_set_grp_info(&vi_cfg->grp_info);
    if (ret != HI_SUCCESS) {
        sample_print("set grp info failed!\n");
        goto set_grp_info_failed;
    }

    ret = sample_comm_vi_start_pipe(&vi_cfg->bind_pipe, vi_cfg->pipe_info);
    if (ret != HI_SUCCESS) {
        sample_print("start pipe failed!\n");
        goto start_pipe_failed;
    }

    ret = sample_comm_vi_start_isp(vi_cfg);
    if (ret != HI_SUCCESS) {
        sample_print("start isp failed!\n");
        goto start_isp_failed;
    }

    return HI_SUCCESS;

start_isp_failed:
    sample_comm_vi_stop_pipe(&vi_cfg->bind_pipe, vi_cfg->pipe_info);
start_pipe_failed: /* fall through */
set_grp_info_failed:
    sample_comm_vi_dev_unbind_pipe(vi_dev, &vi_cfg->bind_pipe);
dev_bind_pipe_failed:
    sample_comm_vi_stop_dev(vi_dev);
start_dev_failed:
    sample_comm_vi_stop_mipi_rx(&vi_cfg->sns_info, &vi_cfg->mipi_info);
start_mipi_rx_failed:
    return HI_FAILURE;
}

hi_void sample_comm_vi_stop_vi(const sample_vi_cfg *vi_cfg)
{
    hi_vi_dev vi_dev = vi_cfg->dev_info.vi_dev;

    sample_comm_vi_stop_isp(vi_cfg);
    sample_comm_vi_stop_pipe(&vi_cfg->bind_pipe, vi_cfg->pipe_info);
    sample_comm_vi_dev_unbind_pipe(vi_dev, &vi_cfg->bind_pipe);
    sample_comm_vi_stop_dev(vi_dev);
    sample_comm_vi_stop_mipi_rx(&vi_cfg->sns_info, &vi_cfg->mipi_info);
}

hi_void sample_comm_vi_mode_switch_stop_vi(const sample_vi_cfg *vi_cfg)
{
    hi_vi_dev vi_dev = vi_cfg->dev_info.vi_dev;

    sample_comm_vi_stop_pipe(&vi_cfg->bind_pipe, vi_cfg->pipe_info);
    sample_comm_vi_dev_unbind_pipe(vi_dev, &vi_cfg->bind_pipe);
    sample_comm_vi_stop_dev(vi_dev);
    sample_comm_vi_stop_mipi_rx(&vi_cfg->sns_info, &vi_cfg->mipi_info);
}

static hi_s32 sample_comm_vi_mode_switch_start_one_pipe(hi_vi_pipe vi_pipe, const sample_vi_pipe_info *pipe_info,
    hi_bool is_master_pipe)
{
    hi_s32 ret;

    if (pipe_info->wrap_attr.enable) {
        ret = hi_mpi_vi_set_pipe_buf_wrap_attr(vi_pipe, &pipe_info->wrap_attr);
        if (ret != HI_SUCCESS) {
            sample_print("vi set pipe(%d) buf_wrap_attr failed with %#x!\n", vi_pipe, ret);
            return HI_FAILURE;
        }
    }

    ret = hi_mpi_vi_create_pipe(vi_pipe, &pipe_info->pipe_attr);
    if (ret != HI_SUCCESS) {
        sample_print("vi create pipe(%d) failed with 0x%x!\n", vi_pipe, ret);
        return HI_FAILURE;
    }

    ret = sample_comm_vi_set_pipe_pixel_rate(vi_pipe, pipe_info->pixel_rate);
    if (ret != HI_SUCCESS) {
        sample_print("pipe%d set online clock fail!", vi_pipe);
        goto start_pipe_failed;
    }

    if (is_master_pipe != HI_TRUE) {
        return HI_SUCCESS;
    }

    ret = sample_comm_vi_switch_mode_start_chn(vi_pipe, pipe_info->chn_info, pipe_info->chn_num);
    if (ret != HI_SUCCESS) {
        sample_print("vi pipe(%d) start chn failed!\n", vi_pipe);
        goto start_pipe_failed;
    }

    return HI_SUCCESS;

start_pipe_failed:
    hi_mpi_vi_destroy_pipe(vi_pipe);
    return HI_FAILURE;
}

static hi_s32 sample_comm_vi_mode_switch_start_pipe(const hi_vi_bind_pipe *bind_pipe,
                                                    const sample_vi_pipe_info pipe_info[])
{
    hi_s32 i;
    hi_s32 ret;

    for (i = 0; i < (hi_s32)bind_pipe->pipe_num; i++) {
        hi_vi_pipe vi_pipe = bind_pipe->pipe_id[i];
        hi_bool is_master_pipe = pipe_info[i].is_master_pipe;
        ret = sample_comm_vi_mode_switch_start_one_pipe(vi_pipe, &pipe_info[i], is_master_pipe);
        if (ret != HI_SUCCESS) {
            goto exit;
        }
    }

    return HI_SUCCESS;

exit:
    for (i = i - 1; i >= 0; i--) {
        hi_vi_pipe vi_pipe = bind_pipe->pipe_id[i];
        hi_bool is_master_pipe = pipe_info[i].is_master_pipe;
        sample_comm_vi_stop_one_pipe(vi_pipe, &pipe_info[i], is_master_pipe);
    }
    return HI_FAILURE;
}

hi_s32 sample_comm_vi_mode_switch_start_vi(const sample_vi_cfg *vi_cfg, hi_bool chg_resolution, const hi_size *size)
{
    hi_s32 ret;
    hi_vi_dev vi_dev;

    ret = sample_comm_vi_start_mipi_rx(&vi_cfg->sns_info, &vi_cfg->mipi_info);
    if (ret != HI_SUCCESS) {
        sample_print("start mipi rx failed!\n");
        goto start_mipi_rx_failed;
    }

    vi_dev   = vi_cfg->dev_info.vi_dev;
    ret = sample_comm_vi_start_dev(vi_dev, &vi_cfg->dev_info.dev_attr);
    if (ret != HI_SUCCESS) {
        sample_print("start dev failed!\n");
        goto start_dev_failed;
    }

    ret = sample_comm_vi_dev_bind_pipe(vi_dev, &vi_cfg->bind_pipe);
    if (ret != HI_SUCCESS) {
        sample_print("dev bind pipe failed!\n");
        goto dev_bind_pipe_failed;
    }

    ret = sample_comm_vi_set_grp_info(&vi_cfg->grp_info);
    if (ret != HI_SUCCESS) {
        sample_print("set grp info failed!\n");
        goto set_grp_info_failed;
    }

    ret = sample_comm_vi_mode_switch_start_pipe(&vi_cfg->bind_pipe, vi_cfg->pipe_info);
    if (ret != HI_SUCCESS) {
        sample_print("set grp info failed!\n");
        goto set_grp_info_failed;
    }

    if (chg_resolution == HI_TRUE) {
        ret = sample_comm_vi_switch_isp_resolution(vi_cfg, size);
    } else {
        ret = sample_comm_vi_switch_isp_mode(vi_cfg);
    }
    if (ret != HI_SUCCESS) {
        sample_print("sample_comm_vi_start_isp failed!\n");
        goto start_isp_failed;
    }

    return HI_SUCCESS;

start_isp_failed:
    sample_comm_vi_stop_pipe(&vi_cfg->bind_pipe, vi_cfg->pipe_info);
set_grp_info_failed:
    sample_comm_vi_dev_unbind_pipe(vi_dev, &vi_cfg->bind_pipe);
dev_bind_pipe_failed:
    sample_comm_vi_stop_dev(vi_dev);
start_dev_failed:
    sample_comm_vi_stop_mipi_rx(&vi_cfg->sns_info, &vi_cfg->mipi_info);
start_mipi_rx_failed:
    return HI_FAILURE;
}

static hi_s32 sample_comm_vi_mode_switch_start_one_pipe_chn(hi_vi_pipe vi_pipe, const sample_vi_pipe_info *pipe_info,
    hi_bool is_master_pipe)
{
    hi_s32 ret;

    ret = hi_mpi_vi_start_pipe(vi_pipe);
    if (ret != HI_SUCCESS) {
        sample_print("vi start pipe(%d) failed with 0x%x!\n", vi_pipe, ret);
        goto start_pipe_failed;
    }

    if (is_master_pipe != HI_TRUE) {
        return HI_SUCCESS;
    }

    ret = sample_comm_vi_start_chn(vi_pipe, pipe_info);
    if (ret != HI_SUCCESS) {
        sample_print("vi pipe(%d) start chn failed!\n", vi_pipe);
        goto start_chn_failed;
    }

    return HI_SUCCESS;

start_chn_failed:
    hi_mpi_vi_stop_pipe(vi_pipe);
start_pipe_failed:
    hi_mpi_vi_destroy_pipe(vi_pipe);
    return HI_FAILURE;
}

static hi_s32 sample_comm_vi_mode_switch_start_pipe_chn(const hi_vi_bind_pipe *bind_pipe,
                                                        const sample_vi_pipe_info pipe_info[])
{
    hi_s32 i;
    hi_s32 ret;

    for (i = 0; i < (hi_s32)bind_pipe->pipe_num; i++) {
        hi_vi_pipe vi_pipe = bind_pipe->pipe_id[i];
        hi_bool is_master_pipe = pipe_info[i].is_master_pipe;
        ret = sample_comm_vi_mode_switch_start_one_pipe_chn(vi_pipe, &pipe_info[i], is_master_pipe);
        if (ret != HI_SUCCESS) {
            goto exit;
        }
    }

    return HI_SUCCESS;

exit:
    for (i = i - 1; i >= 0; i--) {
        hi_vi_pipe vi_pipe = bind_pipe->pipe_id[i];
        hi_bool is_master_pipe = pipe_info[i].is_master_pipe;
        sample_comm_vi_stop_one_pipe(vi_pipe, &pipe_info[i], is_master_pipe);
    }
    return HI_FAILURE;
}

static hi_void sample_comoon_vi_query_isp_inner_state_info(hi_vi_pipe vi_pipe, hi_bool switch_wdr)
{
    hi_isp_inner_state_info inner_state_info;
    hi_bool switch_finish;
    hi_u32 i;
    const hi_u32 dev_num = 1;

    while (1) {
        switch_finish = HI_TRUE;
        for (i = 0; i < dev_num; i++) {
            hi_mpi_isp_query_inner_state_info(vi_pipe, &inner_state_info);
            if (switch_wdr == HI_TRUE) {
                switch_finish &= inner_state_info.wdr_switch_finish;
            } else {
                switch_finish &= inner_state_info.res_switch_finish;
            }
        }
        if (switch_finish == HI_TRUE) {
            sample_print("switch finish !\n");
            break;
        }
        hi_usleep(SLEEP_TIME);
    }
}

static hi_bool sample_common_vi_check_need_pipe(hi_vi_pipe vi_pipe, hi_wdr_mode wdr_mode, hi_u32 index)
{
    hi_bool need_pipe = HI_FALSE;

    if (vi_pipe < 0 || vi_pipe >= HI_VI_MAX_PHYS_PIPE_NUM) {
        return need_pipe;
    }

    if (wdr_mode == HI_WDR_MODE_NONE) {
        need_pipe = HI_TRUE;
    } else {
        need_pipe = (index > 0) ? HI_FALSE : HI_TRUE;
    }

    return need_pipe;
}

hi_s32 sample_comm_vi_switch_isp_mode(const sample_vi_cfg *vi_cfg)
{
    hi_u32  i, j;
    hi_s32 ret;
    const hi_u32  dev_num = 1;
    hi_bool need_pipe;
    hi_bool switch_wdr[HI_VI_MAX_PHYS_PIPE_NUM] = {HI_FALSE};
    hi_vi_pipe vi_pipe;
    hi_isp_pub_attr pub_attr, pre_pub_attr;

    for (i = 0; i < dev_num; i++) {
        for (j = 0; j < vi_cfg->bind_pipe.pipe_num; j++) {
            vi_pipe = vi_cfg->bind_pipe.pipe_id[j];
            need_pipe = sample_common_vi_check_need_pipe(vi_pipe,
                vi_cfg->pipe_info[j].isp_info.isp_pub_attr.wdr_mode, j);
            if (need_pipe != HI_TRUE) {
                continue;
            }

            sample_comm_isp_get_pub_attr_by_sns(vi_cfg->sns_info.sns_type, &pub_attr);
            pub_attr.wdr_mode = vi_cfg->pipe_info[j].isp_info.isp_pub_attr.wdr_mode;

            ret = hi_mpi_isp_get_pub_attr(vi_pipe, &pre_pub_attr);
            if (ret != HI_SUCCESS) {
                sample_print("hi_mpi_isp_get_pub_attr failed!\n");
                sample_comm_vi_stop_isp(vi_cfg);
            }
            ret = hi_mpi_isp_set_pub_attr(vi_pipe, &pub_attr);
            if (ret != HI_SUCCESS) {
                sample_print("hi_mpi_isp_set_pub_attr failed!\n");
                sample_comm_vi_stop_isp(vi_cfg);
            }

            if (pre_pub_attr.wdr_mode != pub_attr.wdr_mode) {
                switch_wdr[vi_pipe] = HI_TRUE;
            }
        }
    }

    vi_pipe = vi_cfg->bind_pipe.pipe_id[0];
    sample_comoon_vi_query_isp_inner_state_info(vi_pipe, switch_wdr[vi_pipe]);

    for (i = 0; i < dev_num; i++) {
        ret = sample_comm_vi_mode_switch_start_pipe_chn(&vi_cfg->bind_pipe, vi_cfg->pipe_info);
        if (ret != HI_SUCCESS) {
            sample_print("set grp info failed!\n");
            goto start_pipe_failed;
        }

        return HI_SUCCESS;

        start_pipe_failed: /* fall through */
        sample_comm_vi_dev_unbind_pipe(vi_cfg->dev_info.vi_dev, &vi_cfg->bind_pipe);
    }

    return HI_SUCCESS;
}

hi_s32 sample_comm_vi_switch_isp_resolution(const sample_vi_cfg *vi_cfg, const hi_size *size)
{
    hi_u32  i, j;
    hi_s32 ret;
    const hi_u32  dev_num = 1;
    hi_bool need_pipe;
    hi_bool switch_wdr[HI_VI_MAX_PHYS_PIPE_NUM] = {HI_FALSE};
    hi_vi_pipe vi_pipe;
    hi_isp_pub_attr pub_attr, pre_pub_attr;

    for (i = 0; i < dev_num; i++) {
        for (j = 0; j < vi_cfg->bind_pipe.pipe_num; j++) {
            vi_pipe = vi_cfg->bind_pipe.pipe_id[j];
            need_pipe = sample_common_vi_check_need_pipe(vi_pipe,
                vi_cfg->pipe_info[j].isp_info.isp_pub_attr.wdr_mode, j);
            if (need_pipe != HI_TRUE) {
                continue;
            }

            sample_comm_isp_get_pub_attr_by_sns(vi_cfg->sns_info.sns_type, &pub_attr);
            pub_attr.wdr_mode = vi_cfg->pipe_info[j].isp_info.isp_pub_attr.wdr_mode;
            pub_attr.wnd_rect.width = size->width;
            pub_attr.wnd_rect.height = size->height;

            ret = hi_mpi_isp_get_pub_attr(vi_pipe, &pre_pub_attr);
            if (ret != HI_SUCCESS) {
                sample_print("hi_mpi_isp_get_pub_attr failed!\n");
                sample_comm_vi_stop_isp(vi_cfg);
            }
            ret = hi_mpi_isp_set_pub_attr(vi_pipe, &pub_attr);
            if (ret != HI_SUCCESS) {
                sample_print("hi_mpi_isp_set_pub_attr failed!\n");
                sample_comm_vi_stop_isp(vi_cfg);
            }

            if (pre_pub_attr.wdr_mode != pub_attr.wdr_mode) {
                switch_wdr[vi_pipe] = HI_TRUE;
            }
        }
    }

    vi_pipe = vi_cfg->bind_pipe.pipe_id[0];
    sample_comoon_vi_query_isp_inner_state_info(vi_pipe, switch_wdr[vi_pipe]);

    for (i = 0; i < dev_num; i++) {
        ret = sample_comm_vi_mode_switch_start_pipe_chn(&vi_cfg->bind_pipe, vi_cfg->pipe_info);
        if (ret != HI_SUCCESS) {
            sample_print("set grp info failed!\n");
            goto start_pipe_failed;
        }

        return HI_SUCCESS;

        start_pipe_failed: /* fall through */
        sample_comm_vi_dev_unbind_pipe(vi_cfg->dev_info.vi_dev, &vi_cfg->bind_pipe);
    }

    return HI_SUCCESS;
}


/* use this func to exit vi when start 4 route of vi */
hi_void sample_comm_vi_stop_four_vi(const sample_vi_cfg *vi_cfg, hi_s32 route_num)
{
    hi_s32 i;
    hi_vi_dev vi_dev;

    for (i = 0; i < route_num; i++) {
        vi_dev = vi_cfg[i].dev_info.vi_dev;
        sample_comm_vi_stop_isp(&vi_cfg[i]);
        sample_comm_vi_stop_pipe(&vi_cfg[i].bind_pipe, vi_cfg[i].pipe_info);
        sample_comm_vi_dev_unbind_pipe(vi_dev, &vi_cfg[i].bind_pipe);
        sample_comm_vi_stop_dev(vi_dev);
    }

    for (i = 0; i < route_num; i++) {
        sample_comm_vi_stop_mipi_rx(&vi_cfg[i].sns_info, &vi_cfg[i].mipi_info);
    }
}

static hi_void sample_comm_vi_get_vb_calc_cfg(sample_vi_get_frame_vb_cfg *get_frame_vb_cfg, hi_vb_calc_cfg *calc_cfg)
{
    hi_pic_buf_attr buf_attr;

    buf_attr.width         = get_frame_vb_cfg->size.width;
    buf_attr.height        = get_frame_vb_cfg->size.height;
    buf_attr.align         = HI_DEFAULT_ALIGN;
    buf_attr.bit_width     =
        (get_frame_vb_cfg->dynamic_range == HI_DYNAMIC_RANGE_SDR8) ? HI_DATA_BIT_WIDTH_8 : HI_DATA_BIT_WIDTH_10;
    buf_attr.pixel_format  = get_frame_vb_cfg->pixel_format;
    buf_attr.compress_mode = get_frame_vb_cfg->compress_mode;
    buf_attr.video_format  = get_frame_vb_cfg->video_format;

    hi_common_get_pic_buf_cfg(&buf_attr, calc_cfg);
}

static hi_s32 sample_comm_vi_malloc_frame_blk(hi_vb_pool pool_id,
                                              sample_vi_get_frame_vb_cfg *get_frame_vb_cfg, hi_vb_calc_cfg *calc_cfg,
                                              sample_vi_user_frame_info *user_frame_info)
{
    hi_vb_blk vb_blk;
    hi_phys_addr_t phys_addr;
    hi_void *virt_addr = HI_NULL;
    hi_video_frame_info *frame_info = HI_NULL;

    vb_blk = hi_mpi_vb_get_blk(pool_id, calc_cfg->vb_size, HI_NULL);
    if (vb_blk == HI_VB_INVALID_HANDLE) {
        sample_print("hi_mpi_vb_get_blk err, size:%u\n", calc_cfg->vb_size);
        return HI_FAILURE;
    }

    phys_addr = hi_mpi_vb_handle_to_phys_addr(vb_blk);
    virt_addr = (hi_u8 *)hi_mpi_sys_mmap(phys_addr, calc_cfg->vb_size);
    if (virt_addr == HI_NULL) {
        sample_print("hi_mpi_sys_mmap err!\n");
        hi_mpi_vb_release_blk(vb_blk);
        return HI_FAILURE;
    }

    user_frame_info->vb_blk   = vb_blk;
    user_frame_info->blk_size = calc_cfg->vb_size;

    frame_info = &user_frame_info->frame_info;

    frame_info->pool_id                   = pool_id;
    frame_info->mod_id                    = HI_ID_VI;
    frame_info->video_frame.phys_addr[0]  = phys_addr;
    frame_info->video_frame.phys_addr[1]  = frame_info->video_frame.phys_addr[0] + calc_cfg->main_y_size;
    frame_info->video_frame.virt_addr[0]  = virt_addr;
    frame_info->video_frame.virt_addr[1]  = frame_info->video_frame.virt_addr[0] + calc_cfg->main_y_size;
    frame_info->video_frame.stride[0]     = calc_cfg->main_stride;
    frame_info->video_frame.stride[1]     = calc_cfg->main_stride;
    frame_info->video_frame.width         = get_frame_vb_cfg->size.width;
    frame_info->video_frame.height        = get_frame_vb_cfg->size.height;
    frame_info->video_frame.pixel_format  = get_frame_vb_cfg->pixel_format;
    frame_info->video_frame.video_format  = get_frame_vb_cfg->video_format;
    frame_info->video_frame.compress_mode = get_frame_vb_cfg->compress_mode;
    frame_info->video_frame.dynamic_range = get_frame_vb_cfg->dynamic_range;
    frame_info->video_frame.field         = HI_VIDEO_FIELD_FRAME;
    frame_info->video_frame.color_gamut   = HI_COLOR_GAMUT_BT601;

    return HI_SUCCESS;
}

hi_void sample_comm_vi_free_frame_blk(sample_vi_user_frame_info *user_frame_info)
{
    hi_s32 ret;
    hi_vb_blk vb_blk = user_frame_info->vb_blk;
    hi_u32 blk_size = user_frame_info->blk_size;
    hi_void *virt_addr = user_frame_info->frame_info.video_frame.virt_addr[0];

    ret = hi_mpi_sys_munmap(virt_addr, blk_size);
    if (ret != HI_SUCCESS) {
        sample_print("hi_mpi_sys_munmap failure!\n");
    }

    ret = hi_mpi_vb_release_blk(vb_blk);
    if (ret != HI_SUCCESS) {
        sample_print("hi_mpi_vb_release_blk block 0x%x failure\n", vb_blk);
    }

    user_frame_info->vb_blk = HI_VB_INVALID_HANDLE;
}

hi_s32 sample_comm_vi_get_frame_blk(sample_vi_get_frame_vb_cfg *get_frame_vb_cfg,
                                    sample_vi_user_frame_info user_frame_info[], hi_s32 frame_cnt)
{
    hi_s32 ret;
    hi_s32 i;
    hi_vb_pool pool_id;
    hi_vb_calc_cfg calc_cfg = {0};
    hi_vb_pool_cfg vb_pool_cfg = {0};

    sample_comm_vi_get_vb_calc_cfg(get_frame_vb_cfg, &calc_cfg);

    vb_pool_cfg.blk_size   = calc_cfg.vb_size;
    vb_pool_cfg.blk_cnt    = frame_cnt;
    vb_pool_cfg.remap_mode = HI_VB_REMAP_MODE_NONE;
    pool_id = hi_mpi_vb_create_pool(&vb_pool_cfg);
    if (pool_id == HI_VB_INVALID_POOL_ID) {
        sample_print("hi_mpi_vb_create_pool failed!\n");
        return HI_FAILURE;
    }

    for (i = 0; i < frame_cnt; i++) {
        ret = sample_comm_vi_malloc_frame_blk(pool_id, get_frame_vb_cfg, &calc_cfg, &user_frame_info[i]);
        if (ret != HI_SUCCESS) {
            goto exit;
        }
    }

    return HI_SUCCESS;

exit:
    for (i = i - 1; i >= 0; i--) {
        sample_comm_vi_free_frame_blk(&user_frame_info[i]);
    }
    hi_mpi_vb_destroy_pool(pool_id);
    return HI_FAILURE;
}

hi_void sample_comm_vi_release_frame_blk(sample_vi_user_frame_info user_frame_info[], hi_s32 frame_cnt)
{
    hi_s32 i;
    hi_vb_pool pool_id;

    for (i = 0; i < frame_cnt; i++) {
        sample_comm_vi_free_frame_blk(&user_frame_info[i]);
    }

    pool_id = user_frame_info[0].frame_info.pool_id;
    hi_mpi_vb_destroy_pool(pool_id);
}

static hi_s32 sample_comm_vi_get_fpn_frame_info(hi_vi_pipe vi_pipe,
                                                hi_pixel_format pixel_format, hi_compress_mode compress_mode,
                                                sample_vi_user_frame_info *user_frame_info, hi_s32 blk_cnt)
{
    hi_s32 ret;
    hi_vi_pipe_attr pipe_attr;
    sample_vi_get_frame_vb_cfg vb_cfg;

    ret = hi_mpi_vi_get_pipe_attr(vi_pipe, &pipe_attr);
    if (ret != HI_SUCCESS) {
        sample_print("vi get pipe attr failed!\n");
        return ret;
    }

    vb_cfg.size.width    = pipe_attr.size.width;
    vb_cfg.size.height   = pipe_attr.size.height;
    vb_cfg.pixel_format  = pixel_format;
    vb_cfg.video_format  = HI_VIDEO_FORMAT_LINEAR;
    vb_cfg.compress_mode = compress_mode;
    vb_cfg.dynamic_range = HI_DYNAMIC_RANGE_SDR8;

    ret = sample_comm_vi_get_frame_blk(&vb_cfg, user_frame_info, blk_cnt);
    if (ret != HI_SUCCESS) {
        sample_print("get fpn frame vb failed!\n");
        return ret;
    }

    return HI_SUCCESS;
}

static hi_s32 sample_comm_vi_get_raw_bit_width(hi_pixel_format pixel_format)
{
    switch (pixel_format) {
        case HI_PIXEL_FORMAT_RGB_BAYER_8BPP:
            return 8; /* 8:single pixel width */
        case HI_PIXEL_FORMAT_RGB_BAYER_10BPP:
            return 10; /* 10:single pixel width */
        case HI_PIXEL_FORMAT_RGB_BAYER_12BPP:
            return 12; /* 12:single pixel width */
        case HI_PIXEL_FORMAT_RGB_BAYER_14BPP:
            return 14; /* 14:single pixel width */
        case HI_PIXEL_FORMAT_RGB_BAYER_16BPP:
            return 16; /* 16:single pixel width */
        default:
            return 0;
    }
}

static hi_void sample_comm_vi_get_fpn_file_name(hi_video_frame *video_frame, hi_char *file_name, hi_u32 length)
{
    (hi_void)snprintf_s(file_name, length, length - 1, "./FPN_frame_%ux%u_%dbit.raw",
                        video_frame->width, video_frame->height,
                        sample_comm_vi_get_raw_bit_width(video_frame->pixel_format));
}

static hi_s32 sample_comm_vi_get_fpn_file_name_iso(hi_video_frame *video_frame, const hi_char *dir_name,
                                                   hi_char *file_name, hi_u32 length, hi_u32 iso)
{
    hi_s32 err;
    err = snprintf_s(file_name, length, length - 1, "./%s/FPN_frame_%ux%u_%dbit_iso%u.raw",
                     dir_name, video_frame->width, video_frame->height,
                     sample_comm_vi_get_raw_bit_width(video_frame->pixel_format), iso);
    if (err < 0) {
        return HI_FAILURE;
    }
    return HI_SUCCESS;
}

hi_void sample_comm_vi_save_fpn_file(hi_isp_fpn_frame_info *fpn_frame_info, FILE *pfd)
{
    hi_u8 *virt_addr;
    hi_u32 fpn_height;
    hi_s32 i;

    fpn_height = fpn_frame_info->fpn_frame.video_frame.height;
    virt_addr = (hi_u8 *)fpn_frame_info->fpn_frame.video_frame.virt_addr[0];

    /* save Y
        * ---------------------------------------------------------------- */
    (hi_void)fprintf(stderr,
                     "FPN: saving......Raw data......stide: %u, width: %u, "
                     "height: %u, iso: %u.\n",
                     fpn_frame_info->fpn_frame.video_frame.stride[0],
                     fpn_frame_info->fpn_frame.video_frame.width, fpn_height,
                     fpn_frame_info->iso);
    (hi_void)fprintf(stderr, "phys addr: 0x%lx\n", (hi_ulong)fpn_frame_info->fpn_frame.video_frame.phys_addr[0]);
    (hi_void)fprintf(stderr, "please wait a moment to save FPN raw data.\n");
    (hi_void)fflush(stderr);

    (hi_void)fwrite(virt_addr, fpn_frame_info->frm_size, 1, pfd);
    sync();

    /* save offset */
    for (i = 0; i < HI_VI_MAX_SPLIT_NODE_NUM; i++) {
        (hi_void)fwrite(&fpn_frame_info->offset[i], 4, 1, pfd); /* 4: 4byte */
        sync();
    }

    /* save compress mode */
    (hi_void)fwrite(&fpn_frame_info->fpn_frame.video_frame.compress_mode, 4, 1, pfd); /* 4: 4byte */
    sync();

    /* save fpn frame size */
    (hi_void)fwrite(&fpn_frame_info->frm_size, 4, 1, pfd); /* 4: 4byte */
    sync();

    /* save iso */
    (hi_void)fwrite(&fpn_frame_info->iso, 4, 1, pfd); /* 4: 4byte */
    sync();
    (hi_void)fflush(pfd);
}

static hi_s32 sample_comm_vi_fpn_multi_calibrate(hi_vi_pipe vi_pipe, sample_vi_user_frame_info *user_frame_info,
    hi_isp_fpn_calibrate_attr *calibrate_attr, hi_s32 calib_cnt)
{
    hi_s32 i, ret;

    for (i = 0; i < calib_cnt; i++) {
        /* point each fpn dark frame vb to calibrate_attr */
        (hi_void)memcpy_s(&calibrate_attr->fpn_cali_frame.fpn_frame, sizeof(hi_video_frame_info),
                          &user_frame_info[i].frame_info, sizeof(hi_video_frame_info));

        ret = hi_mpi_isp_fpn_calibrate(vi_pipe, calibrate_attr);
        if (ret != HI_SUCCESS) {
            sample_print("vi fpn calibrate failed!\n");
            return HI_FAILURE;
        }
        (hi_void)memcpy_s(&user_frame_info[i].frame_info, sizeof(hi_video_frame_info),
                          &calibrate_attr->fpn_cali_frame.fpn_frame, sizeof(hi_video_frame_info));
    }

    return HI_SUCCESS;
}

static hi_s32 sample_comm_vi_fpn_calibrate_process(hi_vi_pipe vi_pipe, sample_vi_user_frame_info *user_frame_info,
    hi_isp_fpn_calibrate_attr *calibrate_attr, hi_s32 calib_cnt)
{
    calibrate_attr->frame_num = calib_cnt;
    calibrate_attr->fpn_mode = HI_ISP_FPN_OUT_MODE_NORM;
    return  sample_comm_vi_fpn_multi_calibrate(vi_pipe, user_frame_info, calibrate_attr, calib_cnt);
}

hi_s32 sample_comm_vi_fpn_calibrate(hi_vi_pipe vi_pipe, sample_vi_fpn_calibration_cfg *calibration_cfg)
{
    hi_s32 ret, i;
    const hi_vi_chn vi_chn = 0;
    FILE *pfd = HI_NULL;
    sample_vi_user_frame_info user_frame_info = {0};
    hi_isp_fpn_calibrate_attr calibrate_attr;

    hi_char fpn_file_name[FPN_FILE_NAME_LENGTH];

    printf("please turn off camera aperture to start calibrate!\nhit enter key ,start calibrate!\n");
    (hi_void)getchar();

    ret = hi_mpi_vi_disable_chn(vi_pipe, vi_chn);
    if (ret != HI_SUCCESS) {
        return HI_FAILURE;
    }

    calibrate_attr.threshold = calibration_cfg->threshold;
    calibrate_attr.frame_num = calibration_cfg->frame_num;
    calibrate_attr.fpn_type  = calibration_cfg->fpn_type;
    ret = sample_comm_vi_get_fpn_frame_info(vi_pipe, HI_PIXEL_FORMAT_RGB_BAYER_16BPP,
                                            calibration_cfg->compress_mode, &user_frame_info, 1);
    if (ret != HI_SUCCESS) {
        hi_mpi_vi_enable_chn(vi_pipe, vi_chn);
        return HI_FAILURE;
    }

    ret = sample_comm_vi_fpn_calibrate_process(vi_pipe, &user_frame_info, &calibrate_attr, 1);
    if (ret != HI_SUCCESS) {
        sample_print("vi fpn calibrate failed!\n");
        goto exit;
    } else {
        sample_print("vi fpn calibrate done!\n");
    }

    printf("\nafter calibrate ");
    for (i = 0; i < HI_VI_MAX_SPLIT_NODE_NUM; i++) {
        printf("offset[%d] = 0x%x, ", i, calibrate_attr.fpn_cali_frame.offset[i]);
    }
    printf("frame_size = %u, iso = %u\n", calibrate_attr.fpn_cali_frame.frm_size, calibrate_attr.fpn_cali_frame.iso);

    sample_comm_vi_get_fpn_file_name(&calibrate_attr.fpn_cali_frame.fpn_frame.video_frame,
                                     fpn_file_name, FPN_FILE_NAME_LENGTH);
    printf("save dark frame file: %s!\n", fpn_file_name);
    pfd = fopen(fpn_file_name, "wb");
    if (pfd == HI_NULL) {
        printf("open file %s err!\n", fpn_file_name);
        goto exit;
    }

    sample_comm_vi_save_fpn_file(&calibrate_attr.fpn_cali_frame, pfd);

    (hi_void)fclose(pfd);

exit:
    sample_comm_vi_release_frame_blk(&user_frame_info, 1);
    ret = hi_mpi_vi_enable_chn(vi_pipe, vi_chn);
    return ret;
}

static hi_void sample_comm_vi_read_fpn_file(hi_isp_fpn_frame_info *fpn_frame_info, FILE *pfd)
{
    hi_video_frame_info *frame_info;
    hi_s32 i;

    frame_info = &fpn_frame_info->fpn_frame;
    (hi_void)fread((hi_u8 *)frame_info->video_frame.virt_addr[0], fpn_frame_info->frm_size, 1, pfd);

    for (i = 0; i < HI_VI_MAX_SPLIT_NODE_NUM; i++) {
        (hi_void)fread((hi_u8 *)&fpn_frame_info->offset[i], 4, 1, pfd); /* 4: 4byte */
    }

    (hi_void)fread((hi_u8 *)&frame_info->video_frame.compress_mode, 4, 1, pfd); /* 4: 4byte */
    (hi_void)fread((hi_u8 *)&fpn_frame_info->frm_size, 4, 1, pfd); /* 4: 4byte */
    (hi_void)fread((hi_u8 *)&fpn_frame_info->iso, 4, 1, pfd); /* 4: 4byte */
}

hi_s32 sample_comm_vi_enable_fpn_correction(hi_vi_pipe vi_pipe, sample_vi_fpn_correction_cfg *correction_cfg)
{
    hi_s32 ret;
    hi_u32 i;
    FILE *pfd = HI_NULL;
    hi_isp_fpn_attr correction_attr;
    sample_vi_user_frame_info *user_frame_info = &correction_cfg->user_frame_info;
    hi_char fpn_file_name[FPN_FILE_NAME_LENGTH];

    ret = sample_comm_vi_get_fpn_frame_info(vi_pipe, correction_cfg->pixel_format,
                                            correction_cfg->compress_mode, user_frame_info, 1);
    if (ret != HI_SUCCESS) {
        return HI_FAILURE;
    }
    (hi_void)memcpy_s(&correction_attr.fpn_frm_info.fpn_frame, sizeof(hi_video_frame_info),
                      &user_frame_info->frame_info, sizeof(hi_video_frame_info));

    sample_comm_vi_get_fpn_file_name(&correction_attr.fpn_frm_info.fpn_frame.video_frame,
                                     fpn_file_name, FPN_FILE_NAME_LENGTH);
    pfd = fopen(fpn_file_name, "rb");
    if (pfd == HI_NULL) {
        printf("open file %s err!\n", fpn_file_name);
        goto exit;
    }

    correction_attr.fpn_frm_info.frm_size = user_frame_info->blk_size;
    sample_comm_vi_read_fpn_file(&correction_attr.fpn_frm_info, pfd);

    (hi_void)fclose(pfd);

    for (i = 0; i < HI_VI_MAX_SPLIT_NODE_NUM; i++) {
        printf("offset[%u] = 0x%x; ", i, correction_attr.fpn_frm_info.offset[i]);
    }
    printf("\n");
    printf("frame_size = %u.\n", correction_attr.fpn_frm_info.frm_size);
    printf("iso = %u.\n", correction_attr.fpn_frm_info.iso);

    correction_attr.enable = HI_TRUE;
    correction_attr.aibnr_mode = HI_FALSE;
    correction_attr.op_type = correction_cfg->op_mode;
    correction_attr.fpn_type = correction_cfg->fpn_type;
    correction_attr.manual_attr.strength = correction_cfg->strength;
    ret = hi_mpi_isp_set_fpn_attr(vi_pipe, &correction_attr);
    if (ret != HI_SUCCESS) {
        sample_print("set fpn attr failed!\n");
        goto exit;
    }

    return HI_SUCCESS;

exit:
    sample_comm_vi_release_frame_blk(user_frame_info, 1);
    return ret;
}

hi_s32 sample_comm_vi_enable_fpn_correction_for_thermo(hi_vi_pipe vi_pipe, sample_vi_fpn_correction_cfg *correction_cfg)
{
    hi_s32 ret;
    FILE *pfd = HI_NULL;
    hi_isp_fpn_attr correction_attr;
    sample_vi_user_frame_info *user_frame_info = &correction_cfg->user_frame_info;
    hi_char fpn_file_name[FPN_FILE_NAME_LENGTH];

    ret = sample_comm_vi_get_fpn_frame_info(vi_pipe, correction_cfg->pixel_format,
                                            correction_cfg->compress_mode, user_frame_info, 1);
    if (ret != HI_SUCCESS) {
        return HI_FAILURE;
    }
    (hi_void)memcpy_s(&correction_attr.fpn_frm_info.fpn_frame, sizeof(hi_video_frame_info),
                      &user_frame_info->frame_info, sizeof(hi_video_frame_info));

    sample_comm_vi_get_fpn_file_name(&correction_attr.fpn_frm_info.fpn_frame.video_frame,
                                     fpn_file_name, FPN_FILE_NAME_LENGTH);
    pfd = fopen(fpn_file_name, "rb");
    if (pfd == HI_NULL) {
        printf("open file %s err!\n", fpn_file_name);
        goto exit;
    }

    correction_attr.fpn_frm_info.frm_size = user_frame_info->blk_size;
    sample_comm_vi_read_fpn_file(&correction_attr.fpn_frm_info, pfd);

    (hi_void)fclose(pfd);

    correction_attr.enable = HI_TRUE;
    correction_attr.aibnr_mode = HI_FALSE;
    correction_attr.op_type = correction_cfg->op_mode;
    correction_attr.fpn_type = correction_cfg->fpn_type;
    correction_attr.manual_attr.strength = correction_cfg->strength;
    ret = hi_mpi_isp_set_fpn_attr(vi_pipe, &correction_attr);
    if (ret != HI_SUCCESS) {
        sample_print("set fpn attr failed!\n");
        goto exit;
    }

    return HI_SUCCESS;

exit:
    sample_comm_vi_release_frame_blk(user_frame_info, 1);
    return ret;
}

static FILE *sample_comm_vi_fopen(hi_char *file_name, const char *mode, hi_char *path)
{
    if (file_name[0] == '\0') {
        sample_print("file name is NULL!\n");
        return HI_NULL;
    }

    if (strlen(file_name) > (PATH_MAX - 1)) {
        sample_print("file name extra long!\n");
        return HI_NULL;
    }

    if (realpath(file_name, path) == HI_NULL) {
        sample_print("file name realpath fail!\n");
        return HI_NULL;
    }

    return fopen(path, "rb");
}

hi_s32 sample_comm_vi_enable_fpn_correction_for_scene(hi_vi_pipe vi_pipe, sample_vi_fpn_correction_cfg *correction_cfg,
    hi_u32 iso, sample_scene_fpn_offset_cfg *scene_fpn_offset_cfg, const hi_char *dir_name)
{
    hi_s32 ret = HI_SUCCESS;
    hi_u32 i;
    FILE *pfd = HI_NULL;
    hi_isp_fpn_attr correction_attr;
    sample_vi_user_frame_info *user_frame_info = &correction_cfg->user_frame_info;
    hi_char fpn_file_name[FPN_FILE_NAME_LENGTH];
    hi_char path[PATH_MAX] = {0};
    check_return(sample_comm_vi_get_fpn_frame_info(vi_pipe, correction_cfg->pixel_format, correction_cfg->compress_mode,
                                                   user_frame_info, 1),
                 "sample_comm_vi_get_fpn_frame_info");
    (hi_void)memcpy_s(&correction_attr.fpn_frm_info.fpn_frame, sizeof(hi_video_frame_info),
                      &user_frame_info->frame_info, sizeof(hi_video_frame_info));

    check_return(sample_comm_vi_get_fpn_file_name_iso(&correction_attr.fpn_frm_info.fpn_frame.video_frame, dir_name,
                                                      fpn_file_name, FPN_FILE_NAME_LENGTH, iso),
                 "sample_comm_vi_get_fpn_file_name_iso");
    pfd = sample_comm_vi_fopen(fpn_file_name, "rb", path);
    if (pfd == HI_NULL) {
        printf("open file %s err!\n", fpn_file_name);
        goto exit;
    }
    printf("open file %s success!\n", fpn_file_name);
    correction_attr.fpn_frm_info.frm_size = user_frame_info->blk_size;
    sample_comm_vi_read_fpn_file(&correction_attr.fpn_frm_info, pfd);
    ret = fclose(pfd);
    if (ret != HI_SUCCESS) {
        goto exit;
    }
    correction_attr.fpn_frm_info.iso = iso;
    for (i = 0; i < HI_VI_MAX_SPLIT_NODE_NUM; i++) {
        correction_attr.fpn_frm_info.offset[i] = scene_fpn_offset_cfg->offset;
        printf("offset[%u] = %#x; ", i, scene_fpn_offset_cfg->offset);
    }
    printf("\n frame_size = %u. iso = %u.\n", correction_attr.fpn_frm_info.frm_size, correction_attr.fpn_frm_info.iso);
    correction_attr.enable = HI_TRUE;
    correction_attr.aibnr_mode = correction_cfg->aibnr_mode;
    correction_attr.op_type = correction_cfg->op_mode;
    correction_attr.fpn_type = correction_cfg->fpn_type;
    correction_attr.manual_attr.strength = correction_cfg->strength;
    correction_attr.fpn_frm_info.fpn_frame.video_frame.compress_mode = correction_cfg->compress_mode;
    ret = hi_mpi_isp_set_fpn_attr(vi_pipe, &correction_attr);
    if (ret != HI_SUCCESS) {
        sample_print("set fpn attr failed!\n");
        goto exit;
    }
    return HI_SUCCESS;

exit:
    sample_comm_vi_release_frame_blk(user_frame_info, 1);
    return ret;
}


hi_s32 sample_comm_vi_disable_fpn_correction(hi_vi_pipe vi_pipe, sample_vi_fpn_correction_cfg *correction_cfg)
{
    hi_s32 ret;
    hi_isp_fpn_attr correction_attr;

    ret = hi_mpi_isp_get_fpn_attr(vi_pipe, &correction_attr);
    if (ret != HI_SUCCESS) {
        sample_print("get fpn attr failed!\n");
        return HI_FAILURE;
    }

    correction_attr.enable = HI_FALSE;
    ret = hi_mpi_isp_set_fpn_attr(vi_pipe, &correction_attr);
    if (ret != HI_SUCCESS) {
        sample_print("set fpn attr failed!\n");
        return HI_FAILURE;
    }

    sample_comm_vi_release_frame_blk(&correction_cfg->user_frame_info, 1);

    return HI_SUCCESS;
}

hi_s32 sample_comm_vi_disable_fpn_correction_for_thermo(hi_vi_pipe vi_pipe,
                                                        sample_vi_fpn_correction_cfg *correction_cfg)
{
    hi_s32 ret;
    hi_isp_fpn_attr correction_attr;

    ret = hi_mpi_isp_get_fpn_attr(vi_pipe, &correction_attr);
    if (ret != HI_SUCCESS) {
        sample_print("get fpn attr failed!\n");
        return HI_FAILURE;
    }

    if (correction_attr.enable == HI_FALSE) {
        return HI_SUCCESS;
    }

    correction_attr.enable = HI_FALSE;
    ret = hi_mpi_isp_set_fpn_attr(vi_pipe, &correction_attr);
    if (ret != HI_SUCCESS) {
        sample_print("set fpn attr failed!\n");
        return HI_FAILURE;
    }

    sample_comm_vi_release_frame_blk(&correction_cfg->user_frame_info, 1);

    return HI_SUCCESS;
}

hi_s32 sample_comm_vi_start_virt_pipe(const sample_vi_cfg *vi_cfg)
{
    hi_s32 ret;

    ret = sample_comm_vi_start_pipe(&vi_cfg->bind_pipe, vi_cfg->pipe_info);
    if (ret != HI_SUCCESS) {
        sample_print("start pipe failed!\n");
        goto start_pipe_failed;
    }

    ret = sample_comm_vi_start_isp(vi_cfg);
    if (ret != HI_SUCCESS) {
        sample_print("sample_comm_vi_start_isp failed!\n");
        goto start_isp_failed;
    }

    return HI_SUCCESS;

start_isp_failed:
    sample_comm_vi_stop_pipe(&vi_cfg->bind_pipe, vi_cfg->pipe_info);
start_pipe_failed:
    return HI_FAILURE;
}

hi_void sample_comm_vi_stop_virt_pipe(const sample_vi_cfg *vi_cfg)
{
    sample_comm_vi_stop_isp(vi_cfg);
    sample_comm_vi_stop_pipe(&vi_cfg->bind_pipe, vi_cfg->pipe_info);
}


static hi_s32 sample_comm_vi_convert_chroma_planar_to_sp42x(FILE *file, hi_u8 *chroma_data,
    hi_u32 luma_stride, hi_u32 chroma_width, hi_u32 chroma_height)
{
    hi_u32 chroma_stride = luma_stride >> 1;
    hi_u8 *dst = HI_NULL;
    hi_u32 row;
    hi_u32 list;
    hi_u8 *temp = HI_NULL;

    temp = (hi_u8*)malloc(chroma_stride);
    if (temp == HI_NULL) {
        sample_print("vi malloc failed!\n");
        return HI_FAILURE;
    }
    if (memset_s(temp, chroma_stride, 0, chroma_stride) != EOK) {
        sample_print("vi memset_s failed!\n");
        free(temp);
        temp = HI_NULL;
        return HI_FAILURE;
    }

    /* U */
    dst = chroma_data + 1;
    for (row = 0; row < chroma_height; ++row) {
        (hi_void)fread(temp, chroma_width, 1, file); /* sp420 U-component data starts 1/2 way from the beginning */
        for (list = 0; list < chroma_stride; ++list) {
            *dst = *(temp + list);
            dst += 2; /* traverse 2 steps away to the next U-component data */
        }
        dst = chroma_data + 1;
        dst += (row + 1) * luma_stride;
    }

    /* V */
    dst = chroma_data;
    for (row = 0; row < chroma_height; ++row) {
        (hi_void)fread(temp, chroma_width, 1, file); /* sp420 V-component data starts 1/2 way from the beginning */
        for (list = 0; list < chroma_stride; ++list) {
            *dst = *(temp + list);
            dst += 2; /* traverse 2 steps away to the next V-component data */
        }
        dst = chroma_data;
        dst += (row + 1) * luma_stride;
    }

    free(temp);

    return HI_SUCCESS;
}

static hi_s32 sample_comm_vi_read_file_to_sp42_x(FILE *file, hi_video_frame *frame)
{
    hi_u8 *luma = (hi_u8*)(hi_uintptr_t)frame->virt_addr[0];
    hi_u8 *chroma = (hi_u8*)(hi_uintptr_t)frame->virt_addr[1];
    hi_u32 luma_width = frame->width;
    hi_u32 chroma_width = luma_width >> 1;
    hi_u32 luma_height = frame->height;
    hi_u32 chroma_height = luma_height;
    hi_u32 luma_stride = frame->stride[0];

    hi_u8 *dst = HI_NULL;
    hi_u32 row;

    if (frame->video_format == HI_VIDEO_FORMAT_LINEAR) {
        /* Y */
        dst = luma;
        for (row = 0; row < luma_height; ++row) {
            (hi_void)fread(dst, luma_width, 1, file);
            dst += luma_stride;
        }

        if (HI_PIXEL_FORMAT_YUV_400 == frame->pixel_format) {
            return HI_SUCCESS;
        } else if (HI_PIXEL_FORMAT_YVU_SEMIPLANAR_420 == frame->pixel_format) {
            chroma_height = chroma_height >> 1;
        }
        if (sample_comm_vi_convert_chroma_planar_to_sp42x(
            file, chroma, luma_stride, chroma_width, chroma_height) != HI_SUCCESS) {
            return HI_FAILURE;
        }
    } else {
        (hi_void)fread(luma, luma_stride * luma_height * 3 / 2, 1,  file); /* Tile 64x16 size = stridexheight*3/2 */
    }

    return HI_SUCCESS;
}

static hi_s32 sample_comm_vi_get_user_pic_frame_info(hi_size *dst_size, sample_vi_user_frame_info *user_frame_info)
{
    hi_s32 ret;
    sample_vi_get_frame_vb_cfg vb_cfg;

    vb_cfg.size.width    = dst_size->width;
    vb_cfg.size.height   = dst_size->height;
    vb_cfg.pixel_format  = HI_PIXEL_FORMAT_YVU_SEMIPLANAR_420;
    vb_cfg.video_format  = HI_VIDEO_FORMAT_LINEAR;
    vb_cfg.compress_mode = HI_COMPRESS_MODE_NONE;
    vb_cfg.dynamic_range = HI_DYNAMIC_RANGE_SDR8;

    ret = sample_comm_vi_get_frame_blk(&vb_cfg, user_frame_info, 1);
    if (ret != HI_SUCCESS) {
        sample_print("get user pic frame vb failed!\n");
        return ret;
    }

    return HI_SUCCESS;
}

static hi_s32 sample_comm_vi_add_scale_task(hi_video_frame_info *src_frame, hi_video_frame_info *dst_frame)
{
    hi_s32 ret;
    hi_vgs_handle handle;
    hi_vgs_task_attr vgs_task_attr;

    ret = hi_mpi_vgs_begin_job(&handle);
    if (ret != HI_SUCCESS) {
        sample_print("hi_mpi_vgs_begin_job failed, ret:0x%x", ret);
        return HI_FAILURE;
    }

    if (memcpy_s(&vgs_task_attr.img_in, sizeof(hi_video_frame_info),
        src_frame, sizeof(hi_video_frame_info)) != EOK) {
        sample_print("memcpy_s img_in failed\n");
        return HI_FAILURE;
    }

    if (memcpy_s(&vgs_task_attr.img_out, sizeof(hi_video_frame_info),
        dst_frame, sizeof(hi_video_frame_info)) != EOK) {
        sample_print("memcpy_s img_out failed\n");
        return HI_FAILURE;
    }

    if (hi_mpi_vgs_add_scale_task(handle, &vgs_task_attr, HI_VGS_SCALE_COEF_NORM) != HI_SUCCESS) {
        sample_print("hi_mpi_vgs_add_scale_task failed\n");
        return HI_FAILURE;
    }

    ret = hi_mpi_vgs_end_job(handle);
    if (ret != HI_SUCCESS) {
        hi_mpi_vgs_cancel_job(handle);
        sample_print("hi_mpi_vgs_end_job failed, ret:0x%x", ret);
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

static hi_s32 sample_comm_vi_read_user_frame_file(hi_vi_pipe vi_pipe, sample_vi_user_frame_info *user_frame_info)
{
    hi_s32 ret;
    FILE *pfd = HI_NULL;
    const hi_char *frame_file = "./UsePic_2560x1440_sp420.yuv";
    hi_size frame_size = {WIDTH_2560, HEIGHT_1440};
    sample_vi_user_frame_info pic_frame_info;

    ret = sample_comm_vi_get_user_pic_frame_info(&frame_size, &pic_frame_info);
    if (ret != HI_SUCCESS) {
        return ret;
    }

    pfd = fopen(frame_file, "rb");
    if (pfd == HI_NULL) {
        sample_print("open file \"%s\" failed!\n", frame_file);
        ret = HI_FAILURE;
        goto exit0;
    }

    ret = sample_comm_vi_read_file_to_sp42_x(pfd, &pic_frame_info.frame_info.video_frame);
    if (ret != HI_SUCCESS) {
        goto exit1;
    }

    (hi_void)fflush(pfd);

    ret = sample_comm_vi_add_scale_task(&pic_frame_info.frame_info, &user_frame_info->frame_info);
    if (ret != HI_SUCCESS) {
        sample_print("add vgs scale task failed.\n");
    }

exit1:
    (hi_void)fclose(pfd);
exit0:
    sample_comm_vi_release_frame_blk(&pic_frame_info, 1);
    return ret;
}

static hi_s32 sample_comm_vi_add_coverex_task(hi_video_frame_info *dst_frame)
{
    hi_s32 ret;
    hi_vgs_handle handle;
    hi_vgs_task_attr vgs_task_attr;
    hi_cover cover;

    ret = hi_mpi_vgs_begin_job(&handle);
    if (ret != HI_SUCCESS) {
        sample_print("hi_mpi_vgs_begin_job failed, ret:0x%x", ret);
        return HI_FAILURE;
    }

    if (memcpy_s(&vgs_task_attr.img_in, sizeof(hi_video_frame_info),
        dst_frame, sizeof(hi_video_frame_info)) != EOK) {
        sample_print("memcpy_s img_in failed\n");
        return HI_FAILURE;
    }

    if (memcpy_s(&vgs_task_attr.img_out, sizeof(hi_video_frame_info),
        dst_frame, sizeof(hi_video_frame_info)) != EOK) {
        sample_print("memcpy_s img_out failed\n");
        return HI_FAILURE;
    }

    cover.type = HI_COVER_RECT;
    cover.rect_attr.rect.x = 0;
    cover.rect_attr.rect.y = 0;
    cover.rect_attr.rect.width = dst_frame->video_frame.width;
    cover.rect_attr.rect.height = dst_frame->video_frame.height;
    cover.rect_attr.is_solid = HI_TRUE;
    cover.color = 0xFF0000;
    if (hi_mpi_vgs_add_cover_task(handle, &vgs_task_attr, &cover, 1) != HI_SUCCESS) {
        sample_print("hi_mpi_vgs_add_scale_task failed\n");
        return HI_FAILURE;
    }

    ret = hi_mpi_vgs_end_job(handle);
    if (ret != HI_SUCCESS) {
        hi_mpi_vgs_cancel_job(handle);
        sample_print("hi_mpi_vgs_end_job failed, ret:0x%x", ret);
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

hi_s32 sample_common_vi_load_user_pic(hi_vi_pipe vi_pipe, sample_vi_user_pic_type user_pic_type,
    sample_vi_user_frame_info *user_frame_info)
{
    hi_s32 ret;
    hi_vi_pipe_attr pipe_attr;

    ret = hi_mpi_vi_get_pipe_attr(vi_pipe, &pipe_attr);
    if (ret != HI_SUCCESS) {
        sample_print("vi get pipe attr failed!\n");
        return ret;
    }

    ret = sample_comm_vi_get_user_pic_frame_info(&pipe_attr.size, user_frame_info);
    if (ret != HI_SUCCESS) {
        return ret;
    }

    if (user_pic_type == VI_USER_PIC_FRAME) {
        ret = sample_comm_vi_read_user_frame_file(vi_pipe, user_frame_info);
    } else {
        ret = sample_comm_vi_add_coverex_task(&user_frame_info->frame_info);
    }

    if (ret != HI_SUCCESS) {
        sample_comm_vi_release_frame_blk(user_frame_info, 1);
        return ret;
    }

    return HI_SUCCESS;
}

hi_void sample_common_vi_unload_user_pic(sample_vi_user_frame_info *user_frame_info)
{
    sample_comm_vi_release_frame_blk(user_frame_info, 1);
}

#define WDR_FUSION_MAX_FRAME    2
#define SENSOR_DEV_MAX_NUM      4

#define WDR_MAX_PTS_DIFF 10000
static hi_bool sample_common_vi_is_frame_pts_suitable(hi_u64 pts1, hi_u64 pts2)
{
    hi_s64 pts_diff;

    pts_diff = pts1 - pts2;
    pts_diff = ((pts_diff >= 0) ? pts_diff : (-pts_diff));

    if (pts_diff <= WDR_MAX_PTS_DIFF) { /* 10000: The maximum difference between two frames is 10ms */
        return HI_TRUE;
    } else {
        return HI_FALSE;
    }
}

static hi_s32 sample_common_vi_match_wdr_pts(hi_video_frame_info frame_info[], hi_vi_pipe vi_pipe[], hi_s32 pipe_num)
{
    hi_s32 cnt = 0;
    hi_s32 try_time = 5; /* try 5 times */
    const hi_s32 millsec = 1000; /* timeout parameter */
    hi_u64 pts_max, pts_min;
    hi_s32 ret;
    hi_s32 min_id, i;

    if (pipe_num != WDR_FUSION_MAX_FRAME) {
        printf("this demo only support two pipe\n");
        return HI_FAILURE;
    }

    while (cnt++ < try_time) {
        pts_max = frame_info[0].video_frame.pts;
        pts_min = frame_info[0].video_frame.pts;
        min_id = 0;
        for (i = 1; i < pipe_num; i++) {
            pts_max = pts_max > frame_info[i].video_frame.pts ? pts_max : frame_info[i].video_frame.pts;
            pts_min = pts_min < frame_info[i].video_frame.pts ? pts_min : frame_info[i].video_frame.pts;
            if (pts_min == frame_info[i].video_frame.pts) {
                min_id = i;
            }
        }

        if (sample_common_vi_is_frame_pts_suitable(pts_max, pts_min) == HI_TRUE) {
            return HI_SUCCESS;
        }

        (hi_void)hi_mpi_vi_release_pipe_frame(vi_pipe[min_id], &frame_info[min_id]);
        ret = hi_mpi_vi_get_pipe_frame(vi_pipe[min_id], &frame_info[min_id], millsec);
        if (ret != HI_SUCCESS) {
            printf("repeated get pipe[%d] frame failed\n", vi_pipe[min_id]);
            return HI_FAILURE;
        }
    }

    return HI_FAILURE;
}

static hi_s32 sample_common_vi_run_be_set_pipe_dump_attr(hi_vi_pipe vi_pipe[], hi_s32 pipe_num,
    hi_vi_frame_dump_attr *dump_attr)
{
    hi_s32 i = 0;
    hi_s32 ret;

    for (i = 0; i < pipe_num; i++) {
        ret = hi_mpi_vi_set_pipe_frame_dump_attr(vi_pipe[i], dump_attr);
        if (ret != HI_SUCCESS) {
            printf("set pipe[%d] dump attr failed\n", vi_pipe[i]);
            return HI_FAILURE;
        }

        ret = hi_mpi_vi_set_pipe_frame_source(vi_pipe[i], HI_VI_PIPE_FRAME_SOURCE_USER);
        if (ret != HI_SUCCESS) {
            printf("set pipe[%d] user frame source failed\n", vi_pipe[i]);
            return HI_FAILURE;
        }
    }

    return HI_SUCCESS;
}

static hi_void *sample_vi_run_be_dump_and_send_proc_single(hi_void *param)
{
    hi_vi_frame_dump_attr dump_attr = {
        .depth = 2, /* dump depth */
        .enable = HI_TRUE
    };
    const hi_s32 millsec = 1000; /* timeout parameter */
    hi_s32 wait_time = -1; /* blocking mode */
    const hi_video_frame_info *send_frame_info[HI_VI_MAX_WDR_FRAME_NUM] = {HI_NULL};
    sample_run_be_bind_pipe *bind_pipe = (sample_run_be_bind_pipe*)param;
    hi_vi_pipe *vi_pipe = bind_pipe->pipe_id;
    hi_s32 pipe_num = bind_pipe->pipe_num;
    hi_s32 i, ret;
    hi_video_frame_info frame_info[WDR_FUSION_MAX_FRAME];

    if (sample_common_vi_run_be_set_pipe_dump_attr(vi_pipe, pipe_num, &dump_attr) != HI_SUCCESS) {
        return HI_NULL;
    }

    while (g_send_pipe_pthread) {
        for (i = 0; i < (hi_s32)bind_pipe->pipe_num && i < WDR_FUSION_MAX_FRAME; i++) {
            ret = hi_mpi_vi_get_pipe_frame(vi_pipe[i], &frame_info[i], wait_time);
            if (ret != HI_SUCCESS) {
                printf("%s, %d vi pipe %d get pipe frame failed! wait time = %d\n",
                    __func__, __LINE__, vi_pipe[i], wait_time);
                goto err;
            }
        }

        if (bind_pipe->wdr_mode != HI_WDR_MODE_NONE) {
            if (sample_common_vi_match_wdr_pts(frame_info, vi_pipe, bind_pipe->pipe_num) != HI_SUCCESS) {
                printf("pipe frame not suitable, lost frame\n");
                goto err;
            }
        }

        send_frame_info[0] = &frame_info[0];
        send_frame_info[1] = &frame_info[1];

        ret = hi_mpi_vi_send_pipe_raw(vi_pipe[0], send_frame_info, pipe_num, millsec);
        if (ret != HI_SUCCESS) {
            printf("pipe[%d] send frame failed\n", vi_pipe[0]);
        }

err:
    for (i = i - 1; i >= 0; i--) {
        (hi_void)hi_mpi_vi_release_pipe_frame(vi_pipe[i], &frame_info[i]);
    }
    }

    return HI_NULL;
}

hi_s32 sample_comm_vi_send_run_be_frame(sample_run_be_bind_pipe *bind_pipe)
{
    hi_s32 ret;
    pthread_t thread_id = 0;
    g_send_pipe_pthread = HI_TRUE;
    ret = pthread_create(&thread_id, HI_NULL, sample_vi_run_be_dump_and_send_proc_single, (hi_void *)bind_pipe);
    if (ret != HI_SUCCESS) {
        printf("vi create run be send frame thread failed!\n");
        g_send_pipe_pthread = HI_FALSE;
        return HI_FAILURE;
    }

    printf("runbe send frame thread running, press enter key to exit!\n");
    (hi_void)getchar();

    g_send_pipe_pthread = HI_FALSE;
    pthread_join(thread_id, HI_NULL);
    return ret;
}

hi_u32 sample_comm_vi_get_raw_stride(hi_pixel_format pixel_format, hi_u32 width, hi_u32 byte_align, hi_u32 align)
{
    hi_u32 stride = 0;

    if (pixel_format == HI_PIXEL_FORMAT_RGB_BAYER_16BPP) {
        stride = HI_ALIGN_UP(HI_ALIGN_UP(width * 16, 8) / 8, align); // align 16, 8
    } else if (pixel_format == HI_PIXEL_FORMAT_RGB_BAYER_14BPP) {
        if (byte_align == 1) {
            stride = (width * 14 + 127) / 128 * 128 / 8; // raw14 (width * 14 + 127) / 128 * 128 / 8;
        } else {
            if (((width * 14) % 8) == 0) { // raw14 8bit
                stride = (width * 14) / 8; // raw14 -- v_buf->width * nbit / 8
            } else {
                stride = (width * 14) / 8 + 8; // raw14 -- v_buf->width * nbit / 8
            }
        }
    } else if (pixel_format == HI_PIXEL_FORMAT_RGB_BAYER_12BPP) {
        if (byte_align == 1) {
            stride = (width * 12 + 127) / 128 * 128 / 8; // raw12 (width * 12 + 127) / 128 * 128 / 8;
        } else {
            if (((width * 12) % 8) == 0) { // raw12 8bit
                stride = (width * 12) / 8; // raw12 -- v_buf->width * nbit / 8
            } else {
                stride = (width * 12) / 8 + 8; // raw12 -- v_buf->width * nbit / 8
            }
        }
    } else if (pixel_format == HI_PIXEL_FORMAT_RGB_BAYER_10BPP) {
        if (byte_align == 1) {
            stride = (width * 10 + 127) / 128 * 128 / 8; // raw10 (width * 10 + 127) / 128 * 128 / 8;
        } else {
            if (((width * 10) % 8) == 0) { // raw10 8bit
                stride = (width * 10) / 8; // raw10 -- v_buf->width * nbit / 8
            } else {
                stride = (width * 10) / 8 + 8; // raw10 -- v_buf->width * nbit / 8
            }
        }
    } else if (pixel_format == HI_PIXEL_FORMAT_RGB_BAYER_8BPP) {
        stride = (width * 8 + 127) / 128 * 128 / 8; // raw8 (width * 8+ 127) / 128 * 128 / 8;
    }

    return stride;
}

static hi_void sample_comm_vi_read_raw_from_file(FILE *pfd, hi_video_frame *v_frame)
{
    hi_u32 bit_width;
    hi_u8 *data;
    hi_u32 i;

    bit_width = sample_comm_vi_get_raw_bit_width(v_frame->pixel_format);

    data = (hi_u8 *)(hi_ulong)v_frame->virt_addr[0];
    for (i = 0; i < v_frame->height; i++) {
        if (bit_width == 8) { // 8 bit
            fread(data, v_frame->width, 1, pfd); // 1bytes
        } else if (bit_width == 16) { // 16 bit
            fread(data, v_frame->width, 2, pfd);  // 2bytes
        } else {
            if (((v_frame->width * bit_width) % 8) == 0) {  // 8 bit per byte
                fread(data, v_frame->width * bit_width / 8, 1, pfd);  // 8 bit per byte
            } else {
                fread(data, ((v_frame->width * bit_width) / 8 + 8), 1, pfd);  // 8 bit per byte
            }
        }
        data += v_frame->stride[0];
    }
}

hi_s32 sample_comm_vi_read_raw_frame(hi_char *frame_file,
    sample_vi_user_frame_info user_frame_info[], hi_u32 frame_cnt)
{
    FILE *pfd;
    hi_u32 i;
    hi_char path[PATH_MAX] = {0};

    printf("reading frame file: %s, please wait...\n", frame_file);
    pfd = sample_comm_vi_fopen(frame_file, "rb", path);
    if (!pfd) {
        printf("open file \"%s\" failed!\n", frame_file);
        return HI_FAILURE;
    }

    for (i = 0; i < frame_cnt; i++) {
        hi_video_frame *video_frame = &user_frame_info[i].frame_info.video_frame;

        if ((video_frame->pixel_format == HI_PIXEL_FORMAT_RGB_BAYER_8BPP) ||
            (video_frame->pixel_format == HI_PIXEL_FORMAT_RGB_BAYER_10BPP) ||
            (video_frame->pixel_format == HI_PIXEL_FORMAT_RGB_BAYER_12BPP) ||
            (video_frame->pixel_format == HI_PIXEL_FORMAT_RGB_BAYER_14BPP) ||
            (video_frame->pixel_format == HI_PIXEL_FORMAT_RGB_BAYER_16BPP)) {
            sample_comm_vi_read_raw_from_file(pfd, video_frame);
        } else {
            printf("unsupport pixel format (%d)!\n", video_frame->pixel_format);
        }
    }

    if (fclose(pfd)) {
        return HI_FAILURE;
    }

    printf("read raw frame from file \"%s\" done!\n", frame_file);

    return HI_SUCCESS;
}
