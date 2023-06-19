/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */

#ifndef __OT_MIPI_TX_H__
#define __OT_MIPI_TX_H__

#include "osal_ioctl.h"

#define CMD_MAX_NUM    4
#define LANE_MAX_NUM   4
#define MIPI_TX_DISABLE_LANE_ID (-1)
#define MIPI_TX_SET_DATA_SIZE 800
#define MIPI_TX_GET_DATA_SIZE 160

#define OT_ALIGN_NUM          8
#define ATTRIBUTE             __attribute__((aligned(OT_ALIGN_NUM)))

typedef enum {
    OUT_MODE_CSI            = 0x0,              /* csi mode */
    OUT_MODE_DSI_VIDEO      = 0x1,              /* dsi video mode */
    OUT_MODE_DSI_CMD        = 0x2,              /* dsi command mode */

    OUT_MODE_BUTT
} out_mode_t;

typedef enum {
    MIPI_TX_WORK_MODE_LP     = 0x0,              /* LP(Low Power) work mode */
    MIPI_TX_WORK_MODE_HS     = 0x1,              /* HS(High Speed) work mode */

    MIPI_TX_WORK_MODE_BUTT
} mipi_tx_work_mode_t;

typedef enum {
    BURST_MODE                      = 0x0,
    NON_BURST_MODE_SYNC_PULSES      = 0x1,
    NON_BURST_MODE_SYNC_EVENTS      = 0x2,

    VIDEO_DATA_MODE_BUTT
} video_mode_t;

typedef enum {
    OUT_FORMAT_RGB_16BIT          = 0x0, /* DSI */
    OUT_FORMAT_RGB_18BIT          = 0x1,
    OUT_FORMAT_RGB_18BIT_LOOSELY  = 0x2,
    OUT_FORMAT_RGB_24BIT          = 0x3,
    OUT_FORMAT_YUV420_12BIT       = 0x4,
    OUT_FORMAT_YUV422_16BIT       = 0x5,

    OUT_FORMAT_YUV420_8BIT_NORMAL = 0x6, /* CSI */
    OUT_FORMAT_YUV420_8BIT_LEGACY = 0x7,
    OUT_FORMAT_YUV422_8BIT        = 0x8,
    OUT_FORMAT_RGB_888            = 0x9,
    OUT_FORMAT_RAW_8BIT           = 0xa,
    OUT_FORMAT_RAW_10BIT          = 0xb,
    OUT_FORMAT_RAW_12BIT          = 0xc,
    OUT_FORMAT_RAW_14BIT          = 0xd,
    OUT_FORMAT_RAW_16BIT          = 0xe,
    OUT_FORMAT_BUTT
} out_format_t;

typedef struct {
    unsigned short  hsa_pixels;
    unsigned short  hbp_pixels;
    unsigned short  hact_pixels;
    unsigned short  hfp_pixels;

    unsigned short  vsa_lines;
    unsigned short  vbp_lines;
    unsigned short  vact_lines;
    unsigned short  vfp_lines;
} sync_info_t;

typedef struct {
    unsigned int        devno;                   /* device number */
    short               lane_id[LANE_MAX_NUM];   /* lane_id: -1 - disable */
    out_mode_t          out_mode;                /* output mode: CSI/DSI_VIDEO/DSI_CMD */
    video_mode_t        video_mode;
    out_format_t        out_format;
    sync_info_t         sync_info;
    unsigned int        phy_data_rate;           /* Mbps */
    unsigned int        pixel_clk;               /* KHz */
} combo_dev_cfg_t;

typedef struct {
    unsigned int        devno;                   /* device number */
    mipi_tx_work_mode_t work_mode;               /* work mode: low power mode, high speed mode. */
    unsigned char       lp_clk_en;               /* low power clock enable. */
    unsigned short      data_type;               /* data type */
    unsigned short      cmd_size;
    const unsigned char ATTRIBUTE *cmd;
} cmd_info_t;

typedef struct {
    unsigned int        devno;          /* device number */
    mipi_tx_work_mode_t work_mode;      /* work mode: low power mode, high speed mode. */
    unsigned char       lp_clk_en;      /* low power clock enable. */
    unsigned short      data_type;      /* DSI data type */
    unsigned short      data_param;     /* data param,low 8 bit:first param.high 8 bit:second param, set 0 if not use */
    unsigned short      get_data_size;  /* read data size */
    unsigned char       ATTRIBUTE *get_data;      /* read data memory address, should  malloc by user */
} get_cmd_info_t;

#define OT_MIPI_TX_IOC_MAGIC   't'

#define OT_MIPI_TX_SET_DEV_CFG  _IOW(OT_MIPI_TX_IOC_MAGIC, 0x01, combo_dev_cfg_t)
#define OT_MIPI_TX_SET_CMD      _IOW(OT_MIPI_TX_IOC_MAGIC, 0x02, cmd_info_t)
#define OT_MIPI_TX_ENABLE       _IO(OT_MIPI_TX_IOC_MAGIC, 0x03)
#define OT_MIPI_TX_GET_CMD      _IOWR(OT_MIPI_TX_IOC_MAGIC, 0x04, get_cmd_info_t)
#define OT_MIPI_TX_DISABLE      _IO(OT_MIPI_TX_IOC_MAGIC, 0x05)

#endif /* end of #ifndef __OT_MIPI_TX_H__ */
