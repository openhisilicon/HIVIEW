#ifndef __HI_MIPI__
#define __HI_MIPI__

#include "hi_type.h"

typedef unsigned int MIPI_PHY;
typedef unsigned int COMBO_LINK;
typedef unsigned int COMBO_DEV;

#define LVDS_MIN_WIDTH      32
#define LVDS_MIN_HEIGHT     32

#define COMBO_MAX_DEV_NUM   2   /* hi3519 support 2 sensors */
#define COMBO_MAX_LINK_NUM  3   /* hi3519 has 3 links */
#define COMBO_MAX_LANE_NUM  12
#define LANE_NUM_PER_LINK   4   /* one link has 4 lanes */


#define MIPI_LANE_NUM       (LANE_NUM_PER_LINK * 2)   /* hi3519 mipi support max 2 links */
#define LVDS_LANE_NUM       COMBO_MAX_LANE_NUM        /* hi3519 lvds suppor max 3 links, so has 12 lanes */


#define WDR_VC_NUM          4
#define SYNC_CODE_NUM       4


//#define HI_MIPI_REG_DEBUG
//#define HI_MIPI_DEBUG
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


typedef enum
{
    MIPI_VC0_NO_MATCH  = 0x1 << 0,        /* VC0,frame's start and frame's end do not match  */
    MIPI_VC1_NO_MATCH  = 0x1 << 1,        /* VC1,frame's start and frame's end do not match  */
    MIPI_VC2_NO_MATCH  = 0x1 << 2,        /* VC2,frame's start and frame's end do not match  */
    MIPI_VC3_NO_MATCH  = 0x1 << 3,        /* VC3,frame's start and frame's end do not match  */

    MIPI_VC0_ORDER_ERR = 0x1 << 8,        /* VC0'S frame order error*/
    MIPI_VC1_ORDER_ERR = 0x1 << 9,        /* VC1'S frame order error*/
    MIPI_VC2_ORDER_ERR = 0x1 << 10,       /* VC2'S frame order error*/
    MIPI_VC3_ORDER_ERR = 0x1 << 11,       /* VC3'S frame order error*/

    MIPI_VC0_FRAME_CRC = 0x1 << 16,        /* in the last frame,VC0'S data has a CRC ERROR at least  */
    MIPI_VC1_FRAME_CRC = 0x1 << 17,        /* in the last frame,VC1'S data has a CRC ERROR at least  */
    MIPI_VC2_FRAME_CRC = 0x1 << 18,        /* in the last frame,VC2'S data has a CRC ERROR at least  */
    MIPI_VC3_FRAME_CRC = 0x1 << 19,        /* in the last frame,VC3'S data has a CRC ERROR at least  */
} MIPI_FRAME_INT_ERR;

typedef enum
{
    MIPI_VC0_PKT_DATA_CRC = 0x1 << 0,      /* VC0'S data CRC error */
    MIPI_VC1_PKT_DATA_CRC = 0x1 << 1,       
    MIPI_VC2_PKT_DATA_CRC = 0x1 << 2,
    MIPI_VC3_PKT_DATA_CRC = 0x1 << 3,

    MIPI_PKT_HEADER_ERR    = 0x1 << 16,    /* Header has two error at least ,and ECC error correction is invalid */
} MIPI_PKT_INT1_ERR;

typedef enum
{
    MIPI_VC0_PKT_INVALID_DT = 0x1 << 0,        /* do not support VC0'S data type */
    MIPI_VC1_PKT_INVALID_DT = 0x1 << 1,        /* do not support VC1'S data type */
    MIPI_VC2_PKT_INVALID_DT = 0x1 << 2,        /* do not support VC2'S data type */
    MIPI_VC3_PKT_INVALID_DT = 0x1 << 3,        /* do not support VC3'S data type */

    MIPI_VC0_PKT_HEADER_ECC = 0x1 << 16,       /* VC0'S header has errors,and ECC error correction is ok */
    MIPI_VC1_PKT_HEADER_ECC = 0x1 << 17,       
    MIPI_VC2_PKT_HEADER_ECC = 0x1 << 18,
    MIPI_VC3_PKT_HEADER_ECC = 0x1 << 19,
} MIPI_PKT_INT2_ERR;

typedef enum
{
    MIPI_ESC_D0   = 0x1 << 0,               /* data lane 0 escape interrupt state */
    MIPI_ESC_D1   = 0x1 << 1,               /* data lane 1 escape interrupt state */
    MIPI_ESC_D2   = 0x1 << 2,               /* data lane 2 escape interrupt state */
    MIPI_ESC_D3   = 0x1 << 3,               /* data lane 3 escape interrupt state */
    MIPI_ESC_CLK  = 0x1 << 4,				/* clock lane escape interrupt state */

    MIPI_TIMEOUT_D0  = 0x1 << 8,            /* data lane 0 FSM timeout interrupt state */
    MIPI_TIMEOUT_D1  = 0x1 << 9,            /* data lane 1 FSM timeout interrupt state */
    MIPI_TIMEOUT_D2  = 0x1 << 10,           /* data lane 2 FSM timeout interrupt state */
    MIPI_TIMEOUT_D3  = 0x1 << 11,           /* data lane 3 FSM timeout interrupt state */
    MIPI_TIMEOUT_CLK = 0x1 << 8,			/* clock lane FSM timeout interrupt state */

} LINK_INT_STAT;

typedef enum
{
    CMD_FIFO_READ_ERR   = 0x1 << 0,        /* MIPI_CTRL read command FIFO error */
    DATA_FIFO_READ_ERR  = 0x1 << 1,
    CMD_FIFO_WRITE_ERR  = 0x1 << 16,
    DATA_FIFO_WRITE_ERR = 0x1 << 17,
} MIPI_CTRL_INT_ERR;

typedef enum
{
    LANE0_SYNC_ERR  = 0x1 << 9,
    LANE1_SYNC_ERR  = 0x1 << 10,
    LANE2_SYNC_ERR  = 0x1 << 11,
    LANE3_SYNC_ERR  = 0x1 << 12,
    LANE4_SYNC_ERR  = 0x1 << 13,
    LANE5_SYNC_ERR  = 0x1 << 14,
    LANE6_SYNC_ERR  = 0x1 << 15,
    LANE7_SYNC_ERR  = 0x1 << 16,
    LANE8_SYNC_ERR  = 0x1 << 17,
    LANE9_SYNC_ERR  = 0x1 << 18,
    LANE10_SYNC_ERR = 0x1 << 19,
    LANE11_SYNC_ERR = 0x1 << 20,

    LINK0_WRITE_ERR = 0x1 << 0,
    LINK1_WRITE_ERR = 0x1 << 3,
    LINK2_WRITE_ERR = 0x1 << 6,

    LINK0_VSYNC_ERR = 0x1 << 1,
    LINK1_VSYNC_ERR = 0x1 << 4,
    LINK2_VSYNC_ERR = 0x1 << 7,

    LINK0_HSYNC_ERR = 0x1 << 2,
    LINK1_HSYNC_ERR = 0x1 << 5,
    LINK2_HSYNC_ERR = 0x1 << 8,

    LVDS_STAT_ERR   = 0x1 << 21,

    LINK0_READ_ERR  = 0x1 << 24,
    LINK1_READ_ERR  = 0x1 << 25,
    LINK2_READ_ERR  = 0x1 << 26,

} LVDS_CTRL_INTR_ERR;

typedef enum
{
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


typedef enum
{
    OUTPUT_DATA_WIDTH_2BIT = 0,
    OUTPUT_DATA_WIDTH_4BIT,
    OUTPUT_DATA_WIDTH_8BIT,
    OUTPUT_DATA_WIDTH_BUTT
} output_data_width;

typedef enum
{
    CLK_UP_EDGE = 0,
    CLK_DOWN_EDGE,
    CLK_EDGE_BUTT
} clk_edge;

typedef enum
{
    OUTPUT_NORM_MSB = 0,
    OUTPUT_REVERSE_MSB,
    OUTPUT_MSB_BUTT
} output_msb;

typedef enum
{
    INPUT_MODE_MIPI         = 0x0,              /* mipi */
    INPUT_MODE_SUBLVDS      = 0x1,              /* SUB_LVDS */
    INPUT_MODE_LVDS         = 0x2,              /* LVDS */
    INPUT_MODE_HISPI        = 0x3,              /* HISPI */
    INPUT_MODE_CMOS         = 0x4,              /* CMOS */
    INPUT_MODE_BT1120       = 0x5,              /* CMOS */

    INPUT_MODE_BUTT
} input_mode_t;

typedef enum
{
    WORK_MODE_LVDS          = 0x0,
    WORK_MODE_MIPI          = 0x1,
    WORK_MODE_CMOS          = 0x2,
    WORK_MODE_BT1120        = 0x3,
    WORK_MODE_BUTT
} work_mode_t;

typedef struct
{
    unsigned int width;
    unsigned int height;
} img_size_t;

/* LVDS WDR MODE defines */
typedef enum
{
    HI_WDR_MODE_NONE 	= 0x0,
    HI_WDR_MODE_2F   	= 0x1,
    HI_WDR_MODE_3F   	= 0x2,
    HI_WDR_MODE_4F   	= 0x3,
    HI_WDR_MODE_DOL_2F  = 0x4,
    HI_WDR_MODE_DOL_3F  = 0x5,
    HI_WDR_MODE_DOL_4F  = 0x6,
    HI_WDR_MODE_BUTT
} wdr_mode_e;

/* MIPI D_PHY WDR MODE defines */
typedef enum
{
    HI_MIPI_WDR_MODE_NONE = 0x0,
    HI_MIPI_WDR_MODE_VC   = 0x1,    /* Virtual Channel */
    HI_MIPI_WDR_MODE_DT   = 0x2,    /* Data Type */
    HI_MIPI_WDR_MODE_DOL  = 0x3,    /* DOL Mode */
    HI_MIPI_WDR_MODE_BUTT
} mipi_wdr_mode_e;

typedef enum
{
    PHY_CLK_SHARE_NONE = 0x0,
    PHY_CLK_SHARE_PHY0 = 0x1,      /* PHY share clock with PHY0 */
    PHY_CLK_SHARE_BUTT = 0x2,
} phy_clk_share_e;

typedef enum
{
    LVDS_ENDIAN_LITTLE  = 0x0,
    LVDS_ENDIAN_BIG     = 0x1,
    LVDS_ENDIAN_BUTT
} lvds_bit_endian;

typedef enum
{
    LVDS_SYNC_MODE_SOF = 0,         /* sensor SOL, EOL, SOF, EOF */
    LVDS_SYNC_MODE_SAV,             /* SAV, EAV */
    LVDS_SYNC_MODE_BUTT
} lvds_sync_mode_e;

typedef enum
{
    RAW_DATA_8BIT = 0,
    RAW_DATA_10BIT,
    RAW_DATA_12BIT,
    RAW_DATA_14BIT,
    RAW_DATA_16BIT,
    RAW_DATA_BUTT
} raw_data_type_e;

typedef enum
{
    LVDS_VSYNC_NORMAL   = 0x00,
    LVDS_VSYNC_SHARE    = 0x01,
    LVDS_VSYNC_HCONNECT = 0x02,
    LVDS_VSYNC_BUTT
} lvds_vsync_type_e;

typedef struct
{
    lvds_vsync_type_e sync_type;

    //hconnect vsync blanking len, valid when the sync_type is LVDS_VSYNC_HCONNECT
    unsigned short hblank1;
    unsigned short hblank2;
} lvds_vsync_type_t;

typedef enum
{
    LVDS_FID_NONE    = 0x00,
    LVDS_FID_IN_SAV  = 0x01,    /* frame identification id in SAV 4th */
    LVDS_FID_IN_DATA = 0x02,    /* frame identification id in first data */
    LVDS_FID_BUTT
} lvds_fid_type_e;

typedef struct
{
    lvds_fid_type_e fid;

    /* Sony DOL has the Frame Information Line, in DOL H-Connection mode,
       should configure this flag as false to disable output the Frame Information Line */
    HI_BOOL output_fil;
} lvds_fid_type_t;

typedef struct
{
    int x;
    int y;
    unsigned int width;
    unsigned int height;
} img_rect_t;

typedef struct
{
    raw_data_type_e     raw_data_type;              /* raw data type: 8/10/12/14 bit */
    wdr_mode_e          wdr_mode;                   /* WDR mode */

    lvds_sync_mode_e    sync_mode;                  /* sync mode: SOF, SAV */
    lvds_vsync_type_t   vsync_type;                 /* normal, share, hconnect */
    lvds_fid_type_t     fid_type;                   /* frame identification code */

    lvds_bit_endian     data_endian;                /* data endian: little/big */
    lvds_bit_endian     sync_code_endian;           /* sync code endian: little/big */
    short               lane_id[LVDS_LANE_NUM];     /* lane_id: -1 - disable */

    /* each vc has 4 params, sync_code[i]:
       sync_mode is SYNC_MODE_SOF: SOF, EOF, SOL, EOL
       sync_mode is SYNC_MODE_SAV: invalid sav, invalid eav, valid sav, valid eav  */
    unsigned short      sync_code[LVDS_LANE_NUM][WDR_VC_NUM][SYNC_CODE_NUM];
} lvds_dev_attr_t;

typedef struct
{
    raw_data_type_e       raw_data_type;            /* raw data type: 8/10/12/14 bit */
    mipi_wdr_mode_e       wdr_mode;                 /* MIPI WDR mode */
    short                 lane_id[MIPI_LANE_NUM];   /* lane_id: -1 - disable */

    union
    {
        short data_type[WDR_VC_NUM];                /* used by the HI_MIPI_WDR_MODE_DT */
    };
} mipi_dev_attr_t;

typedef struct
{
    COMBO_DEV       devno;              /* device number, select sensor0 and sensor 1 */
    input_mode_t    input_mode;         /* input mode: MIPI/LVDS/SUBLVDS/HISPI/DC */
    phy_clk_share_e phy_clk_share;
    img_rect_t      img_rect;           /* MIPI Rx device crop area (corresponding to the oringnal sensor input image size) */
    
    union
    {
        mipi_dev_attr_t     mipi_attr;
        lvds_dev_attr_t     lvds_attr;
    };
} combo_dev_attr_t;

/* phy common mode voltage mode, greater than 900mv or less than 900mv */
typedef enum
{
    PHY_CMV_GE900MV    = 0x00,
    PHY_CMV_LT900MV    = 0x01,
    PHY_CMV_BUTT
} phy_cmv_e;

typedef struct
{
    COMBO_DEV   devno;
    phy_cmv_e   cmv_mode;
} phy_cmv_t;

#define HI_MIPI_IOC_MAGIC   'm'

/* init data lane, input mode, data type */
#define HI_MIPI_SET_DEV_ATTR		        _IOW(HI_MIPI_IOC_MAGIC, 0x01, combo_dev_attr_t)

/* output clk edge */
#define HI_MIPI_SET_OUTPUT_CLK_EDGE         _IOW(HI_MIPI_IOC_MAGIC, 0x02, clk_edge)

/* output data msb */
#define HI_MIPI_SET_OUTPUT_MSB              _IOW(HI_MIPI_IOC_MAGIC, 0x03, output_msb)

/* set phy common mode voltage mode */
#define HI_MIPI_SET_PHY_CMVMODE             _IOW(HI_MIPI_IOC_MAGIC, 0x04, phy_cmv_t)

/* reset sensor */
#define HI_MIPI_RESET_SENSOR		        _IOW(HI_MIPI_IOC_MAGIC, 0x05, COMBO_DEV)

/* unreset sensor */
#define HI_MIPI_UNRESET_SENSOR		        _IOW(HI_MIPI_IOC_MAGIC, 0x06, COMBO_DEV)

/* reset mipi */
#define HI_MIPI_RESET_MIPI		            _IOW(HI_MIPI_IOC_MAGIC, 0x07, COMBO_DEV)

/* unreset mipi */
#define HI_MIPI_UNRESET_MIPI		        _IOW(HI_MIPI_IOC_MAGIC, 0x08, COMBO_DEV)


#endif

