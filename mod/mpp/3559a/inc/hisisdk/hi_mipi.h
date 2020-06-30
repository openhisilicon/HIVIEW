/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2016-2019. All rights reserved.
 * Description: hi_mipi.h
 * Author:
 * Create: 2016-10-07
 */

#ifndef __HI_MIPI__
#define __HI_MIPI__

#include "hi_type.h"

typedef unsigned int MIPI_PHY;
typedef unsigned int SLVS_PHY;
typedef unsigned int COMBO_LINK;

typedef unsigned int combo_dev_t;
typedef unsigned int sns_rst_source_t;
typedef unsigned int sns_clk_source_t;

#define COMBO_MIN_WIDTH          32
#define COMBO_MIN_HEIGHT         32

#define COMBO_MAX_WIDTH          8192
#define COMBO_MAX_HEIGHT         8192

#define MIPI_DEV_NAME           "hi_mipi"
#define MIPI_PROC_NAME          "hi_mipi"
#define HIMEDIA_DYNAMIC_MINOR   255


#define MIPI_IRQ_NUM            1   /* Hi3559AV100 has 1 MIPI interrupt */
#define SLVS_IRQ_NUM            1   /* Hi3559AV100 has 1 SLVS interrupt */

#define SLVS_REG_BASE_NUM       1   /* Hi3559AV100 has 1 SLVS reg base addr */

#define MIPI_PHY_NUM            4   /* Hi3559AV100 4 MIPI HPY */

#define MIPI_RX_MAX_DEV_NUM     8

#define MIPI_MAX_NAME_LEN       10
#define SLVS_MAX_NAME_LEN       10

#define MAX_LANE_NUM_PER_LINK   2  /* one link has 2 lanes at most */


#define MIPI_LANE_NUM           (MAX_LANE_NUM_PER_LINK * 4)   /* Hi3559AV100 mipi support max 4 links */
#define WDR_VC_NUM              4
#define SYNC_CODE_NUM           4
#define SYNC_CODE_WIDTH         4

#define LVDS_LANE_NUM           COMBO_MAX_LANE_NUM        /* Hi3559AV100 lvds suppor max 8 links, so has 16 lanes */


#define SLVS_MAX_LINK_NUM       2   /* Hi3559A has 2 links per phy */
#define SLVS_MAX_DEV_NUM        4
#define SLVS_DEV_NUM_START      0
#define SLVS_PHY_NUM            2   /* Hi3559AV100 suppor 2 SLVS HPY */
#define SLVS_LANE_NUM           8

#define COMS_MAX_DEV_NUM        3

#define SNS_MAX_RST_SOURCE_NUM  4   /* Hi3559AV100 has 4 sensor reset source */
#define SNS_MAX_CLK_SOURCE_NUM  4   /* Hi3559AV100 has 4 sensor clock source */

#define COMBO_MAX_LINK_NUM      8   /* Hi3559AV100 has 8 links */
#define COMBO_MAX_LANE_NUM      16
#define COMBO_MAX_DEV_NUM       MIPI_RX_MAX_DEV_NUM


#ifdef HI_MIPI_DEBUG

#define HI_MSG(x...) \
    do { \
        osal_printk("%s->%d: ", __FUNCTION__, __LINE__); \
        osal_printk(x); \
        osal_printk("\n"); \
    } while (0)
#else

#define HI_MSG(args...) do { } while (0)
#endif

#define HI_ERR(x...) \
    do { \
        osal_printk("%s(%d): ", __FUNCTION__, __LINE__); \
        osal_printk(x); \
        osal_printk("\n"); \
    } while (0)

#define MIPIRX_CHECK_NULL_PTR(ptr)\
    do{\
        if((ptr) == NULL)\
        {\
            HI_ERR("NULL point \r\n");\
            return HI_FAILURE;\
        }\
    } while (0)

#define MIPI_ALIGN_UP(x, a)           ((((x) + ((a) - 1)) / (a)) * (a))

typedef enum {
    MIPI_VC0_NO_MATCH  = 0x1 << 0,        /* VC0,frame's start and frame's end do not match  */
    MIPI_VC1_NO_MATCH  = 0x1 << 1,        /* VC1,frame's start and frame's end do not match  */
    MIPI_VC2_NO_MATCH  = 0x1 << 2,        /* VC2,frame's start and frame's end do not match  */
    MIPI_VC3_NO_MATCH  = 0x1 << 3,        /* VC3,frame's start and frame's end do not match  */

    MIPI_VC0_ORDER_ERR = 0x1 << 8,        /* VC0'S frame order error */
    MIPI_VC1_ORDER_ERR = 0x1 << 9,        /* VC1'S frame order error */
    MIPI_VC2_ORDER_ERR = 0x1 << 10,       /* VC2'S frame order error */
    MIPI_VC3_ORDER_ERR = 0x1 << 11,       /* VC3'S frame order error */

    MIPI_VC0_FRAME_CRC = 0x1 << 16,        /* in the last frame, VC0'S data has a CRC ERROR at least */
    MIPI_VC1_FRAME_CRC = 0x1 << 17,        /* in the last frame, VC1'S data has a CRC ERROR at least */
    MIPI_VC2_FRAME_CRC = 0x1 << 18,        /* in the last frame, VC2'S data has a CRC ERROR at least */
    MIPI_VC3_FRAME_CRC = 0x1 << 19,        /* in the last frame, VC3'S data has a CRC ERROR at least */
} MIPI_FRAME_INT_ERR;

typedef enum {
    MIPI_VC0_PKT_DATA_CRC = 0x1 << 0,      /* VC0'S data CRC error */
    MIPI_VC1_PKT_DATA_CRC = 0x1 << 1,
    MIPI_VC2_PKT_DATA_CRC = 0x1 << 2,
    MIPI_VC3_PKT_DATA_CRC = 0x1 << 3,

    MIPI_PKT_HEADER_ERR    = 0x1 << 16,    /* Header has two error at least ,and ECC error correction is invalid */
} MIPI_PKT_INT1_ERR;

typedef enum {
    MIPI_VC0_PKT_INVALID_DT = 0x1 << 0,        /* do not support VC0'S data type */
    MIPI_VC1_PKT_INVALID_DT = 0x1 << 1,        /* do not support VC1'S data type */
    MIPI_VC2_PKT_INVALID_DT = 0x1 << 2,        /* do not support VC2'S data type */
    MIPI_VC3_PKT_INVALID_DT = 0x1 << 3,        /* do not support VC3'S data type */

    MIPI_VC0_PKT_HEADER_ECC = 0x1 << 16,       /* VC0'S header has errors,and ECC error correction is ok */
    MIPI_VC1_PKT_HEADER_ECC = 0x1 << 17,
    MIPI_VC2_PKT_HEADER_ECC = 0x1 << 18,
    MIPI_VC3_PKT_HEADER_ECC = 0x1 << 19,
} MIPI_PKT_INT2_ERR;

typedef enum {
    MIPI_ESC_D0   = 0x1 << 0,               /* data lane 0 escape interrupt state */
    MIPI_ESC_D1   = 0x1 << 1,               /* data lane 1 escape interrupt state */
    MIPI_ESC_D2   = 0x1 << 2,               /* data lane 2 escape interrupt state */
    MIPI_ESC_D3   = 0x1 << 3,               /* data lane 3 escape interrupt state */
    MIPI_ESC_CLK  = 0x1 << 4,                /* clock lane escape interrupt state */

    MIPI_TIMEOUT_D0  = 0x1 << 8,            /* data lane 0 FSM timeout interrupt state */
    MIPI_TIMEOUT_D1  = 0x1 << 9,            /* data lane 1 FSM timeout interrupt state */
    MIPI_TIMEOUT_D2  = 0x1 << 10,           /* data lane 2 FSM timeout interrupt state */
    MIPI_TIMEOUT_D3  = 0x1 << 11,           /* data lane 3 FSM timeout interrupt state */
    MIPI_TIMEOUT_CLK = 0x1 << 8,            /* clock lane FSM timeout interrupt state */

} LINK_INT_STAT;

typedef enum {
    CMD_FIFO_WRITE_ERR  = 0x1 << 0,           /* MIPI_CTRL write command FIFO error */
    DATA_FIFO_WRITE_ERR = 0x1 << 1,
    CMD_FIFO_READ_ERR   = 0x1 << 16,
    DATA_FIFO_READ_ERR  = 0x1 << 17,
} MIPI_CTRL_INT_ERR;

typedef enum {
    LANE0_SYNC_ERR  = 0x1 << 0,
    LANE1_SYNC_ERR  = 0x1 << 1,
    LANE2_SYNC_ERR  = 0x1 << 2,
    LANE3_SYNC_ERR  = 0x1 << 3,
    LANE4_SYNC_ERR  = 0x1 << 4,
    LANE5_SYNC_ERR  = 0x1 << 5,
    LANE6_SYNC_ERR  = 0x1 << 6,
    LANE7_SYNC_ERR  = 0x1 << 7,
    LANE8_SYNC_ERR  = 0x1 << 8,
    LANE9_SYNC_ERR  = 0x1 << 9,
    LANE10_SYNC_ERR = 0x1 << 10,
    LANE11_SYNC_ERR = 0x1 << 11,
    LANE12_SYNC_ERR = 0x1 << 12,
    LANE13_SYNC_ERR = 0x1 << 13,
    LANE14_SYNC_ERR = 0x1 << 14,
    LANE15_SYNC_ERR = 0x1 << 15,
    LINK0_WRITE_ERR = 0x1 << 16,
    LINK1_WRITE_ERR = 0x1 << 17,
    LINK2_WRITE_ERR = 0x1 << 18,
    LINK3_WRITE_ERR = 0x1 << 19,
    LINK0_READ_ERR  = 0x1 << 20,
    LINK1_READ_ERR  = 0x1 << 21,
    LINK2_READ_ERR  = 0x1 << 22,
    LINK3_READ_ERR  = 0x1 << 23,
    LVDS_STAT_ERR   = 0x1 << 24,


} LVDS_CTRL_INTR_ERR;

typedef enum {
    ALIGN_FIFO_FULL_ERR = 0x1 << 0,            /* MIPI ALIGN FIFO full */
    ALIGN_LANE0_ERR     = 0x1 << 1,            /* MIPI ALIGN lane0 err */
    ALIGN_LANE1_ERR     = 0x1 << 2,
    ALIGN_LANE2_ERR     = 0x1 << 3,
    ALIGN_LANE3_ERR     = 0x1 << 4,
    ALIGN_LANE4_ERR     = 0x1 << 5,
    ALIGN_LANE5_ERR     = 0x1 << 6,
    ALIGN_LANE6_ERR     = 0x1 << 7,
    ALIGN_LANE7_ERR     = 0x1 << 8,
    ALIGN_LANE8_ERR     = 0x1 << 9,
    ALIGN_LANE9_ERR     = 0x1 << 10,
    ALIGN_LANE10_ERR    = 0x1 << 11,
    ALIGN_LANE11_ERR    = 0x1 << 12,
} ALIGN_CTRL_INT_ERR;

typedef enum {
    SLVS_HD_CRC_ERR         = 0x1 << 0,         /* header crc err */
    SLVS_PLD_CRC_ERR        = 0x1 << 1,         /* payload crc err */
    SLVS_ECC_ERR            = 0x1 << 2,         /* ECC err */
    SLVS_DATA_FIFO_W_ERR    = 0x1 << 3,         /* data fifo write err */
    SLVS_DATA_FIFO_R_ERR    = 0x1 << 4,         /* data fifo read err */
    SLVS_CMD_FIFO_FULL      = 0x1 << 5,         /* command fifo full */
    SLVS_SKEW_ERR           = 0x1 << 6,         /* skew err */
    SLVS_VSYNC_RAW          = 0x1 << 7,         /* VSYNC cnt */

} SLVS_LINK_INT_STAT;

/* hs mode  */
typedef enum {
    LANE_DIVIDE_MODE_0    = 0x0,
    LANE_DIVIDE_MODE_1    = 0x1,
    LANE_DIVIDE_MODE_2    = 0x2,
    LANE_DIVIDE_MODE_3    = 0x3,
    LANE_DIVIDE_MODE_4    = 0x4,
    LANE_DIVIDE_MODE_5    = 0x5,
    LANE_DIVIDE_MODE_6    = 0x6,
    LANE_DIVIDE_MODE_7    = 0x7,
    LANE_DIVIDE_MODE_8    = 0x8,
    LANE_DIVIDE_MODE_9    = 0x9,
    LANE_DIVIDE_MODE_A    = 0xA,
    LANE_DIVIDE_MODE_B    = 0xB,
    LANE_DIVIDE_MODE_BUTT
} lane_divide_mode_t;

typedef enum {
    WORK_MODE_LVDS          = 0x0,
    WORK_MODE_MIPI          = 0x1,
    WORK_MODE_CMOS          = 0x2,
    WORK_MODE_BT1120        = 0x3,
    WORK_MODE_SLVS          = 0x4,
    WORK_MODE_BUTT
} work_mode_t;

typedef enum {
    INPUT_MODE_MIPI         = 0x0,              /* mipi */
    INPUT_MODE_SUBLVDS      = 0x1,              /* SUB_LVDS */
    INPUT_MODE_LVDS         = 0x2,              /* LVDS */
    INPUT_MODE_HISPI        = 0x3,              /* HISPI */
    INPUT_MODE_SLVS         = 0x4,              /* SLVS */
    INPUT_MODE_CMOS         = 0x5,              /* CMOS */
    INPUT_MODE_BT601        = 0x6,              /* BT601 */
    INPUT_MODE_BT656        = 0x7,              /* BT656 */
    INPUT_MODE_BT1120       = 0x8,              /* BT1120 */
    INPUT_MODE_BYPASS       = 0x9,              /* MIPI Bypass */
    INPUT_MODE_LVDS_EX      = 0xA,              /* LVDS EX */

    INPUT_MODE_BUTT
} input_mode_t;

typedef enum {
    MIPI_DATA_RATE_X1 = 0,         /* output 1 pixel per clock */
    MIPI_DATA_RATE_X2 = 1,         /* output 2 pixel per clock */

    MIPI_DATA_RATE_BUTT
} mipi_data_rate_t;


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
    DATA_TYPE_RAW_8BIT = 0,
    DATA_TYPE_RAW_10BIT,
    DATA_TYPE_RAW_12BIT,
    DATA_TYPE_RAW_14BIT,
    DATA_TYPE_RAW_16BIT,
    DATA_TYPE_YUV420_8BIT_NORMAL,
    DATA_TYPE_YUV420_8BIT_LEGACY,
    DATA_TYPE_YUV422_8BIT,
    DATA_TYPE_BUTT
} data_type_t;


/* MIPI D_PHY WDR MODE defines */
typedef enum {
    HI_MIPI_WDR_MODE_NONE = 0x0,
    HI_MIPI_WDR_MODE_VC   = 0x1,    /* Virtual Channel */
    HI_MIPI_WDR_MODE_DT   = 0x2,    /* Data Type */
    HI_MIPI_WDR_MODE_DOL  = 0x3,    /* DOL Mode */
    HI_MIPI_WDR_MODE_BUTT
} mipi_wdr_mode_t;

typedef enum {
    SLVS_LANE_RATE_LOW = 0,         /* 1152Mbps */
    SLVS_LANE_RATE_HIGH = 1,        /* 2304Mbps */

    SLVS_LANE_RATE_BUTT
} slvs_lane_rate_t;


typedef struct {
    data_type_t           input_data_type;          /* data type: 8/10/12/14/16 bit */
    mipi_wdr_mode_t       wdr_mode;                 /* MIPI WDR mode */
    short                 lane_id[MIPI_LANE_NUM];   /* lane_id: -1 - disable */

    union {
        short data_type[WDR_VC_NUM];                /* used by the HI_MIPI_WDR_MODE_DT */
    };
} mipi_dev_attr_t;

/* LVDS WDR MODE defines */
typedef enum {
    HI_WDR_MODE_NONE     = 0x0,
    HI_WDR_MODE_2F       = 0x1,
    HI_WDR_MODE_3F       = 0x2,
    HI_WDR_MODE_4F       = 0x3,
    HI_WDR_MODE_DOL_2F  = 0x4,
    HI_WDR_MODE_DOL_3F  = 0x5,
    HI_WDR_MODE_DOL_4F  = 0x6,
    HI_WDR_MODE_BUTT
} wdr_mode_t;

typedef enum {
    LVDS_SYNC_MODE_SOF = 0,         /* sensor SOL, EOL, SOF, EOF */
    LVDS_SYNC_MODE_SAV,             /* SAV, EAV */
    LVDS_SYNC_MODE_BUTT
} lvds_sync_mode_t;

typedef enum {
    LVDS_VSYNC_NORMAL   = 0x00,
    LVDS_VSYNC_SHARE    = 0x01,
    LVDS_VSYNC_HCONNECT = 0x02,
    LVDS_VSYNC_BUTT
} lvds_vsync_type_t;

typedef struct {
    lvds_vsync_type_t sync_type;

    /* hconnect vsync blanking len, valid when the sync_type is LVDS_VSYNC_HCONNECT */
    unsigned short hblank1;
    unsigned short hblank2;
} lvds_vsync_attr_t;

typedef enum {
    LVDS_FID_NONE    = 0x00,
    LVDS_FID_IN_SAV  = 0x01,    /* frame identification id in SAV 4th */
    LVDS_FID_IN_DATA = 0x02,    /* frame identification id in first data */
    LVDS_FID_BUTT
} lvds_fid_type_t;

typedef struct {
    lvds_fid_type_t fid_type;

    /* Sony DOL has the Frame Information Line, in DOL H-Connection mode,
       should configure this flag as false to disable output the Frame Information Line */
    HI_BOOL output_fil;
} lvds_fid_attr_t;

typedef enum {
    LVDS_ENDIAN_LITTLE  = 0x0,
    LVDS_ENDIAN_BIG     = 0x1,
    LVDS_ENDIAN_BUTT
} lvds_bit_endian_t;

typedef enum {
    MIPI_CLK_MODE_SHARE  = 0x0,
    MIPI_CLK_MODE_SEPARATE = 0x1,
    MIPI_CLK_MODE_BUTT
} mipi_clk_mode_t;

typedef enum {
    LVDS_LANE_WORK_MODE_SYNC   = 0x0,
    LVDS_LANE_WORK_MODE_ASYNC  = 0x1,
    LVDS_LANE_WORK_MODE_BUTT
} lvds_lane_work_mode_t;

typedef struct {
    lvds_lane_work_mode_t lane_work_mode;
    unsigned int          sensor_valid_width;
} lvds_lane_work_attr_t;

typedef enum {
    SLVS_ERR_CHECK_MODE_NONE = 0x0,       /* disable ECC & CRC */
    SLVS_ERR_CHECK_MODE_CRC = 0x1,        /* enable   CRC */
    SLVS_ERR_CHECK_MODE_ECC_2BYTE = 0x2,  /* enable   2 Byte ECC */
    SLVS_ERR_CHECK_MODE_ECC_4BYTE = 0x3,  /* enable   4 Byte ECC */

    SLVS_ERR_CHECK_MODE_BUTT
} slvs_err_check_mode_t;


typedef struct {
    data_type_t         input_data_type;            /* data type: 8/10/12/14 bit */
    wdr_mode_t          wdr_mode;                   /* WDR mode */

    lvds_sync_mode_t    sync_mode;                  /* sync mode: SOF, SAV */
    lvds_vsync_attr_t   vsync_attr;                 /* normal, share, hconnect */
    lvds_fid_attr_t     fid_attr;                   /* frame identification code */

    lvds_bit_endian_t   data_endian;                /* data endian: little/big */
    lvds_bit_endian_t   sync_code_endian;           /* sync code endian: little/big */
    short               lane_id[LVDS_LANE_NUM];     /* lane_id: -1 - disable */

    /* each vc has 4 params, sync_code[i]:
       sync_mode is SYNC_MODE_SOF: SOF, EOF, SOL, EOL
       sync_mode is SYNC_MODE_SAV: invalid sav, invalid eav, valid sav, valid eav  */
    unsigned short      sync_code[LVDS_LANE_NUM][WDR_VC_NUM][SYNC_CODE_NUM];
} lvds_dev_attr_t;

typedef struct {
    mipi_clk_mode_t         clk_mode;
    lvds_dev_attr_t         lvds_attr;
    lvds_lane_work_attr_t   lane_work_attr;
} lvds_dev_attr_ex_t;


typedef struct {
    data_type_t           input_data_type;          /* data type: 8/10/12/14/16 bit */
    wdr_mode_t            wdr_mode;                 /* WDR mode */
    slvs_lane_rate_t      lane_rate;
    int                   sensor_valid_width;
    short                 lane_id[SLVS_LANE_NUM];   /* lane_id: -1 - disable */
    slvs_err_check_mode_t err_check_mode;           /* ECC CRC mode */
} slvs_dev_attr_t;


typedef struct {
    combo_dev_t         devno;              /* device number */
    input_mode_t        input_mode;         /* input mode: MIPI/LVDS/SUBLVDS/HISPI/DC */
    mipi_data_rate_t    data_rate;
	/* MIPI Rx device crop area (corresponding to the oringnal sensor input image size) */
    img_rect_t          img_rect;

    union {
        mipi_dev_attr_t     mipi_attr;
        lvds_dev_attr_t     lvds_attr;
        slvs_dev_attr_t     slvs_attr;
        lvds_dev_attr_ex_t  lvds_attr_ex;
    };
} combo_dev_attr_t;

typedef struct hi_MIPI_RX_DEV_CTX_S {
    char mipi_name[MIPI_MAX_NAME_LEN];
    char slvs_name[SLVS_MAX_NAME_LEN];
    combo_dev_attr_t dev_attr;
    HI_BOOL is_configed;
} MIPI_RX_DEV_CTX_S;

typedef struct hi_MIPI_COMS_DEV_CTX_S {
    combo_dev_attr_t dev_attr;
    HI_BOOL is_configed;
} MIPI_COMS_DEV_CTX_S;

/* phy common mode voltage mode, greater than 900mv or less than 900mv */
typedef enum {
    PHY_CMV_GE1200MV    = 0x00,
    PHY_CMV_LT1200MV    = 0x01,
    PHY_CMV_BUTT
} phy_cmv_mode_t;

typedef struct {
    combo_dev_t       devno;
    phy_cmv_mode_t    cmv_mode;
} phy_cmv_t;

#define HI_MIPI_IOC_MAGIC   'm'

/* init data lane, input mode, data type */
#define HI_MIPI_SET_DEV_ATTR                _IOW(HI_MIPI_IOC_MAGIC, 0x01, combo_dev_attr_t)

/* set phy common mode voltage mode */
#define HI_MIPI_SET_PHY_CMVMODE             _IOW(HI_MIPI_IOC_MAGIC, 0x04, phy_cmv_t)

/* reset sensor */
#define HI_MIPI_RESET_SENSOR                _IOW(HI_MIPI_IOC_MAGIC, 0x05, sns_rst_source_t)

/* unreset sensor */
#define HI_MIPI_UNRESET_SENSOR              _IOW(HI_MIPI_IOC_MAGIC, 0x06, sns_rst_source_t)

/* reset mipi */
#define HI_MIPI_RESET_MIPI                  _IOW(HI_MIPI_IOC_MAGIC, 0x07, combo_dev_t)

/* unreset mipi */
#define HI_MIPI_UNRESET_MIPI                _IOW(HI_MIPI_IOC_MAGIC, 0x08, combo_dev_t)

/* reset slvs */
#define HI_MIPI_RESET_SLVS                  _IOW(HI_MIPI_IOC_MAGIC, 0x09, combo_dev_t)

/* unreset slvs */
#define HI_MIPI_UNRESET_SLVS                _IOW(HI_MIPI_IOC_MAGIC, 0x0a, combo_dev_t)

/* set mipi hs_mode */
#define HI_MIPI_SET_HS_MODE                 _IOW(HI_MIPI_IOC_MAGIC, 0x0b, lane_divide_mode_t)

/* enable mipi clock */
#define HI_MIPI_ENABLE_MIPI_CLOCK           _IOW(HI_MIPI_IOC_MAGIC, 0x0c, combo_dev_t)

/* disable mipi clock */
#define HI_MIPI_DISABLE_MIPI_CLOCK          _IOW(HI_MIPI_IOC_MAGIC, 0x0d, combo_dev_t)

/* enable slvs clock */
#define HI_MIPI_ENABLE_SLVS_CLOCK           _IOW(HI_MIPI_IOC_MAGIC, 0x0e, combo_dev_t)

/* disable slvs clock */
#define HI_MIPI_DISABLE_SLVS_CLOCK          _IOW(HI_MIPI_IOC_MAGIC, 0x0f, combo_dev_t)

/* enable sensor clock */
#define HI_MIPI_ENABLE_SENSOR_CLOCK         _IOW(HI_MIPI_IOC_MAGIC, 0x10, sns_clk_source_t)

/* disable sensor clock */
#define HI_MIPI_DISABLE_SENSOR_CLOCK        _IOW(HI_MIPI_IOC_MAGIC, 0x11, sns_clk_source_t)

#define HI_MIPI_CLEAR                       _IOW(HI_MIPI_IOC_MAGIC, 0x12, combo_dev_t)

#endif

