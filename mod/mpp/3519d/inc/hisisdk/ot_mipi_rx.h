/*
  Copyright (c), 2001-2024, Shenshu Tech. Co., Ltd.
 */

#ifndef OT_MIPI_RX_H
#define OT_MIPI_RX_H

#include "osal_ioctl.h"

#ifdef __cplusplus
extern "C"{
#endif /* __cplusplus */

typedef unsigned int combo_dev_t;
typedef unsigned int sns_rst_source_t;
typedef unsigned int sns_clk_source_t;

#define MIPI_LANE_NUM           8
#define LVDS_LANE_NUM           8

#define WDR_VC_NUM              4
#define SYNC_CODE_NUM           4

#define MIPI_RX_MAX_DEV_NUM     4

#define SNS_MAX_CLK_SOURCE_NUM  4
#define SNS_MAX_RST_SOURCE_NUM  4

#define MAX_EXT_DATA_TYPE_NUM   3

#ifdef OT_MIPI_DEBUG
#define ot_msg(x...)                                     \
    do {                                                 \
        osal_printk("%s->%d: ", __FUNCTION__, __LINE__); \
        osal_printk(x);                                  \
        osal_printk("\n");                               \
    } while (0)
#else
#define ot_msg(args...) \
    do {                \
    } while (0)
#endif

#define ot_err(x...)                                     \
    do {                                                 \
        osal_printk("%s(%d): ", __FUNCTION__, __LINE__); \
        osal_printk(x);                                  \
    } while (0)

#define mipi_rx_unused(x) ((void)(x))

typedef enum {
    LANE_DIVIDE_MODE_0 = 0,  /* 8lane */                         /* 16D 4lane */
    LANE_DIVIDE_MODE_1 = 1,  /* 4lane + 4lane */                 /* 16D 2lane + 2lane */
    LANE_DIVIDE_MODE_2 = 2,  /* 4lane + 2lane +2lane */          /* 16D NA */
    LANE_DIVIDE_MODE_3 = 3,  /* 2lane + 2lane + 2lane + 2lane */ /* 16D NA */
    LANE_DIVIDE_MODE_4 = 4,  /* 2lane + 2lane + 4lane */         /* 16D NA */
    LANE_DIVIDE_MODE_BUTT
} lane_divide_mode_t;

typedef enum {
    WORK_MODE_LVDS = 0x0,
    WORK_MODE_MIPI = 0x1,
    WORK_MODE_CMOS = 0x2,
    WORK_MODE_BT1120 = 0x3,
    WORK_MODE_SLVS = 0x4,
    WORK_MODE_BUTT
} work_mode_t;

typedef enum {
    INPUT_MODE_MIPI = 0x0,    /* mipi */
    INPUT_MODE_SUBLVDS = 0x1, /* SUB_LVDS */
    INPUT_MODE_LVDS = 0x2,    /* LVDS */
    INPUT_MODE_HISPI = 0x3,   /* HISPI */

    INPUT_MODE_BUTT
} input_mode_t;

typedef enum {
    MIPI_DATA_RATE_X1 = 0, /* output 1 pixel per clock */
    MIPI_DATA_RATE_X2 = 1, /* output 2 pixel per clock */

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
    DATA_TYPE_YUV422_PACKED, /* yuv422 8bit transform user define 16bit raw */
    DATA_TYPE_BUTT
} data_type_t;

typedef struct {
    combo_dev_t devno;
    unsigned int num;
    unsigned int ext_data_bit_width[MAX_EXT_DATA_TYPE_NUM];
    unsigned int ext_data_type[MAX_EXT_DATA_TYPE_NUM];
} ext_data_type_t;

/* MIPI D_PHY WDR MODE defines */
typedef enum {
    OT_MIPI_WDR_MODE_NONE = 0x0,
    OT_MIPI_WDR_MODE_VC = 0x1,  /* Virtual Channel */
    OT_MIPI_WDR_MODE_DT = 0x2,  /* Data Type */
    OT_MIPI_WDR_MODE_DOL = 0x3, /* DOL Mode */
    OT_MIPI_WDR_MODE_BUTT
} mipi_wdr_mode_t;

typedef struct {
    data_type_t input_data_type;  /* data type: 8/10/12/14/16 bit */
    mipi_wdr_mode_t wdr_mode;     /* MIPI WDR mode */
    short lane_id[MIPI_LANE_NUM]; /* lane_id: -1 - disable */

    union {
        short data_type[WDR_VC_NUM]; /* attribute of MIPI WDR mode. AUTO:mipi_wdr_mode_t:OT_MIPI_WDR_MODE_DT; */
    };
} mipi_dev_attr_t;

typedef enum {
    OT_LVDS_WDR_MODE_NONE = 0x0,
    OT_LVDS_WDR_MODE_2F = 0x1,
    OT_LVDS_WDR_MODE_3F = 0x2,
    OT_LVDS_WDR_MODE_4F = 0x3,
    OT_LVDS_WDR_MODE_DOL_2F = 0x4,
    OT_LVDS_WDR_MODE_DOL_3F = 0x5,
    OT_LVDS_WDR_MODE_DOL_4F = 0x6,
    OT_LVDS_WDR_MODE_BUTT
} lvds_wdr_mode_t;

typedef enum {
    LVDS_SYNC_MODE_SOF = 0, /* sensor SOL, EOL, SOF, EOF */
    LVDS_SYNC_MODE_SAV,     /* SAV, EAV */
    LVDS_SYNC_MODE_BUTT
} lvds_sync_mode_t;

typedef enum {
    LVDS_VSYNC_NORMAL = 0x00,
    LVDS_VSYNC_SHARE = 0x01,
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
    LVDS_FID_NONE = 0x00,
    LVDS_FID_IN_SAV = 0x01,  /* frame identification id in SAV 4th */
    LVDS_FID_IN_DATA = 0x02, /* frame identification id in first data */
    LVDS_FID_BUTT
} lvds_fid_type_t;

typedef struct {
    lvds_fid_type_t fid_type;

    /* Sony DOL has the Frame Information Line, in DOL H-Connection mode,
       should configure this flag as false to disable output the Frame Information Line */
    unsigned char output_fil;
} lvds_fid_attr_t;

typedef enum {
    LVDS_ENDIAN_LITTLE = 0x0,
    LVDS_ENDIAN_BIG = 0x1,
    LVDS_ENDIAN_BUTT
} lvds_bit_endian_t;

typedef struct {
    data_type_t input_data_type; /* data type: 8/10/12/14 bit */
    lvds_wdr_mode_t wdr_mode;         /* WDR mode */

    lvds_sync_mode_t sync_mode;   /* sync mode: SOF, SAV */
    lvds_vsync_attr_t vsync_attr; /* normal, share, hconnect */
    lvds_fid_attr_t fid_attr;     /* frame identification code */

    lvds_bit_endian_t data_endian;      /* data endian: little/big */
    lvds_bit_endian_t sync_code_endian; /* sync code endian: little/big */
    short lane_id[LVDS_LANE_NUM];       /* lane_id: -1 - disable */

    /* each vc has 4 params, sync_code[i]:
       sync_mode is SYNC_MODE_SOF: SOF, EOF, SOL, EOL
       sync_mode is SYNC_MODE_SAV: invalid sav, invalid eav, valid sav, valid eav  */
    unsigned short sync_code[LVDS_LANE_NUM][WDR_VC_NUM][SYNC_CODE_NUM];
} lvds_dev_attr_t;

typedef struct {
    combo_dev_t devno;       /* device number */
    input_mode_t input_mode; /* input mode: MIPI/LVDS/SUBLVDS/HISPI/DC */
    mipi_data_rate_t data_rate;
    img_rect_t img_rect; /* MIPI Rx device crop area (corresponding to the original sensor input image size) */

    union {
        mipi_dev_attr_t mipi_attr; /* attribute of MIPI interface. AUTO:input_mode_t:INPUT_MODE_MIPI; */
        lvds_dev_attr_t lvds_attr; /* attribute of MIPI interface. AUTO:input_mode_t:INPUT_MODE_LVDS; */
    };
} combo_dev_attr_t;

typedef enum {
    PHY_CMV_GE1200MV = 0x00,
    PHY_CMV_LT1200MV = 0x01,
    PHY_CMV_BUTT
} phy_cmv_mode_t;

typedef struct {
    combo_dev_t devno;
    phy_cmv_mode_t cmv_mode;
} phy_cmv_t;

typedef enum {
    PHY_SPEED_LE1500M_BITPERSEC = 0x00, // less or equal to 1.5Gbps
    PHY_SPEED_GT1500M_BITPERSEC = 0x01, // greater than 1.5Gbps
    PHY_SPEED_BUTT
} phy_speed_mode_t;

typedef struct {
    combo_dev_t devno;
    phy_speed_mode_t speed;
} phy_speed_t;

#define OT_MIPI_IOC_MAGIC 'm'

/* init data lane, input mode, data type */
#define OT_MIPI_SET_DEV_ATTR                _IOW(OT_MIPI_IOC_MAGIC, 0x01, combo_dev_attr_t)

/* set phy common mode voltage mode */
#define OT_MIPI_SET_PHY_CMVMODE             _IOW(OT_MIPI_IOC_MAGIC, 0x04, phy_cmv_t)

/* reset sensor */
#define OT_MIPI_RESET_SENSOR                _IOW(OT_MIPI_IOC_MAGIC, 0x05, sns_rst_source_t)

/* unreset sensor */
#define OT_MIPI_UNRESET_SENSOR              _IOW(OT_MIPI_IOC_MAGIC, 0x06, sns_rst_source_t)

/* reset mipi */
#define OT_MIPI_RESET_MIPI                  _IOW(OT_MIPI_IOC_MAGIC, 0x07, combo_dev_t)

/* unreset mipi */
#define OT_MIPI_UNRESET_MIPI                _IOW(OT_MIPI_IOC_MAGIC, 0x08, combo_dev_t)

/* set mipi hs_mode */
#define OT_MIPI_SET_HS_MODE                 _IOW(OT_MIPI_IOC_MAGIC, 0x0b, lane_divide_mode_t)

/* enable mipi clock */
#define OT_MIPI_ENABLE_MIPI_CLOCK           _IOW(OT_MIPI_IOC_MAGIC, 0x0c, combo_dev_t)

/* disable mipi clock */
#define OT_MIPI_DISABLE_MIPI_CLOCK          _IOW(OT_MIPI_IOC_MAGIC, 0x0d, combo_dev_t)

/* enable sensor clock */
#define OT_MIPI_ENABLE_SENSOR_CLOCK         _IOW(OT_MIPI_IOC_MAGIC, 0x10, sns_clk_source_t)

/* disable sensor clock */
#define OT_MIPI_DISABLE_SENSOR_CLOCK        _IOW(OT_MIPI_IOC_MAGIC, 0x11, sns_clk_source_t)

/* ext data type */
#define OT_MIPI_SET_EXT_DATA_TYPE           _IOW(OT_MIPI_IOC_MAGIC, 0x12, ext_data_type_t)

/* set speed mode */
#define OT_MIPI_SET_PHY_SPEED_MODE          _IOW(OT_MIPI_IOC_MAGIC, 0x13, phy_speed_t)

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* OT_MIPI_RX_H */
