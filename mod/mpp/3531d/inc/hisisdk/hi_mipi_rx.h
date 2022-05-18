/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2019-2020. All rights reserved.
 * Description: hisilicon mipi_rx declaration
 * Author: Hisilicon multimedia software group
 * Create: 2019/05/15
 */

#ifndef __HI_MIPI_RX_H__
#define __HI_MIPI_RX_H__

typedef unsigned int combo_dev_t;

#define MIPI_LANE_NUM_PRE_DEV        4
#define MIPI_VC_NUM                  4
#define MIPI_RX_MAX_DEV_NUM          8

#define MIPI_LANE_ID_0 0
#define MIPI_LANE_ID_1 1
#define MIPI_LANE_ID_2 2
#define MIPI_LANE_ID_3 3

#ifdef HI_MIPI_DEBUG
#define HI_MSG(x...)                                     \
    do {                                                 \
        osal_printk("%s->%d: ", __FUNCTION__, __LINE__); \
        osal_printk(x);                                  \
        osal_printk("\n");                               \
    } while (0)
#else
#define HI_MSG(args...) \
    do {                \
    } while (0)
#endif

#define HI_ERR(x...)                                     \
    do {                                                 \
        osal_printk("%s(%d): ", __FUNCTION__, __LINE__); \
        osal_printk(x);                                  \
    } while (0)

#define mipirx_check_null_ptr_return(ptr)     \
    do {                               \
        if ((ptr) == NULL) {             \
            HI_ERR("NULL point \r\n"); \
            return HI_FAILURE;         \
        }                              \
    } while (0)

typedef enum {
    LANE_DIVIDE_MODE_0 = 0,

    LANE_DIVIDE_MODE_BUTT
} lane_divide_mode_t;

typedef enum {
    WORK_MODE_MIPI = 0x0,

    WORK_MODE_BUTT
} work_mode_t;

typedef enum {
    INPUT_MODE_MIPI = 0x0,

    INPUT_MODE_BUTT
} input_mode_t;

typedef enum {
    MIPI_SCAN_INTERLACED  = 0,
    MIPI_SCAN_PROGRESSIVE,

    MIPI_SCAN_BUTT
} mipi_scan_mode_t;

typedef struct {
    int x;
    int y;
    unsigned int width;
    unsigned int height;
} img_rect_t;

typedef struct {
    unsigned int width;
    unsigned int height;
} img_size_t;

typedef enum {
    DATA_TYPE_YUV422_8BIT = 0,

    DATA_TYPE_BUTT
} data_type_t;

typedef struct {
    data_type_t input_data_type;
    short lane_id[MIPI_LANE_NUM_PRE_DEV]; /* lane_id: -1 - disable */
} mipi_dev_attr_t;

typedef struct {
    combo_dev_t devno;        /* device number */
    input_mode_t input_mode;  /* input mode: MIPI/LVDS/SUBLVDS/HISPI/DC */
    img_rect_t img_rect;      /* reserved */
    mipi_scan_mode_t scan_mode; /* reserved */
    mipi_dev_attr_t mipi_attr;
} combo_dev_attr_t;

typedef struct {
    combo_dev_t devno;
    unsigned char pn_swap[MIPI_LANE_NUM_PRE_DEV];
} combo_dev_pn_swap_t;

typedef enum {
    PHY_CMV_GE1200MV = 0x00,
    PHY_CMV_LT1200MV = 0x01,
    PHY_CMV_BUTT
} phy_cmv_mode_t;

typedef struct {
    combo_dev_t devno;
    phy_cmv_mode_t cmv_mode;
} phy_cmv_t;

#define HI_MIPI_IOC_MAGIC            'm'

/* init data lane, input mode, data type */
#define HI_MIPI_SET_DEV_ATTR         _IOW(HI_MIPI_IOC_MAGIC, 0x01, combo_dev_attr_t)

#define HI_MIPI_SET_PN_SWAP          _IOW(HI_MIPI_IOC_MAGIC, 0x02, combo_dev_pn_swap_t)

/* set phy common mode voltage mode */
#define HI_MIPI_SET_PHY_CMVMODE      _IOW(HI_MIPI_IOC_MAGIC, 0x04, phy_cmv_t)

/* reset mipi */
#define HI_MIPI_RESET_MIPI           _IOW(HI_MIPI_IOC_MAGIC, 0x07, combo_dev_t)

/* unreset mipi */
#define HI_MIPI_UNRESET_MIPI         _IOW(HI_MIPI_IOC_MAGIC, 0x08, combo_dev_t)

/* set mipi hs_mode */
#define HI_MIPI_SET_HS_MODE          _IOW(HI_MIPI_IOC_MAGIC, 0x0b, lane_divide_mode_t)

/* enable mipi clock */
#define HI_MIPI_ENABLE_MIPI_CLOCK    _IOW(HI_MIPI_IOC_MAGIC, 0x0c, combo_dev_t)

/* disable mipi clock */
#define HI_MIPI_DISABLE_MIPI_CLOCK   _IOW(HI_MIPI_IOC_MAGIC, 0x0d, combo_dev_t)

#endif /* __HI_MIPI_RX_H__ */
