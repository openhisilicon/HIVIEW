/*
  Copyright (c), 2001-2024, Shenshu Tech. Co., Ltd.
 */

#ifndef OT_COMMON_IVE_H
#define OT_COMMON_IVE_H

#include "ot_common_svp.h"
#include "ot_common.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define OT_IVE_HIST_NUM                     256
#define OT_IVE_MAP_NUM                      256
#define OT_IVE_MAX_RGN_NUM                  254
#define OT_IVE_ST_MAX_CORNER_NUM            500
#define OT_IVE_MASK_NUM                     25
#define OT_IVE_MASK_NUM_121                 121
#define OT_IVE_PADDING_ARR_NUM              3
#define OT_IVE_ARR_RESERVED_NUM_TWO         2
#define OT_IVE_ARR_RESERVED_NUM_THREE       3
#define OT_IVE_ARR_RESERVED_NUM_EIGHT       8
#define OT_IVE_ARR_RESERVED_NUM_TWELVE      12
#define OT_IVE_ARR_RESERVED_NUM_FOURTEEN    14
#define OT_IVE_ARR_NUM_THREE                3
#define OT_IVE_ARR_NUM_EIGHT                8

typedef enum {
    OT_IVE_ERR_SYS_TIMEOUT      = 0x40,             /* IVE process timeout */
    OT_IVE_ERR_QUERY_TIMEOUT    = 0x41,             /* IVE query timeout */
    OT_IVE_ERR_BUS_ERR          = 0x42,             /* IVE BUS error */
    OT_IVE_ERR_OPEN_FILE        = 0x43,             /* IVE open file error */
    OT_IVE_ERR_READ_FILE        = 0x44,             /* IVE read file error */

    OT_IVE_ERR_BUTT
} ot_ive_err_code;

/************************************************IVE error code ***********************************/
/* Invalid device ID */
#define OT_ERR_IVE_INVALID_DEV_ID OT_DEFINE_ERR(OT_ID_IVE, OT_ERR_LEVEL_ERROR, OT_ERR_INVALID_DEV_ID)
/* Invalid channel ID */
#define OT_ERR_IVE_INVALID_CHN_ID OT_DEFINE_ERR(OT_ID_IVE, OT_ERR_LEVEL_ERROR, OT_ERR_INVALID_CHN_ID)
/* At least one parameter is illegal. For example, an illegal enumeration value exists. */
#define OT_ERR_IVE_ILLEGAL_PARAM OT_DEFINE_ERR(OT_ID_IVE, OT_ERR_LEVEL_ERROR, OT_ERR_ILLEGAL_PARAM)
/* The channel exists. */
#define OT_ERR_IVE_EXIST OT_DEFINE_ERR(OT_ID_IVE, OT_ERR_LEVEL_ERROR, OT_ERR_EXIST)
/* The UN exists. */
#define OT_ERR_IVE_UNEXIST OT_DEFINE_ERR(OT_ID_IVE, OT_ERR_LEVEL_ERROR, OT_ERR_UNEXIST)
/* A null point is used. */
#define OT_ERR_IVE_NULL_PTR OT_DEFINE_ERR(OT_ID_IVE, OT_ERR_LEVEL_ERROR, OT_ERR_NULL_PTR)
/* Try to enable or initialize the system, device, or channel before configuring attributes. */
#define OT_ERR_IVE_NOT_CFG OT_DEFINE_ERR(OT_ID_IVE, OT_ERR_LEVEL_ERROR, OT_ERR_NOT_CFG)
/* The operation is not supported currently. */
#define OT_ERR_IVE_NOT_SURPPORT OT_DEFINE_ERR(OT_ID_IVE, OT_ERR_LEVEL_ERROR, OT_ERR_NOT_SUPPORT)
/* The operation, changing static attributes for example, is not permitted. */
#define OT_ERR_IVE_NOT_PERM OT_DEFINE_ERR(OT_ID_IVE, OT_ERR_LEVEL_ERROR, OT_ERR_NOT_PERM)
/* A failure caused by the malloc memory occurs. */
#define OT_ERR_IVE_NO_MEM OT_DEFINE_ERR(OT_ID_IVE, OT_ERR_LEVEL_ERROR, OT_ERR_NO_MEM)
/* A failure caused by the malloc buffer occurs. */
#define OT_ERR_IVE_NO_BUF OT_DEFINE_ERR(OT_ID_IVE, OT_ERR_LEVEL_ERROR, OT_ERR_NO_BUF)
/* The buffer is empty. */
#define OT_ERR_IVE_BUF_EMPTY OT_DEFINE_ERR(OT_ID_IVE, OT_ERR_LEVEL_ERROR, OT_ERR_BUF_EMPTY)
/* No buffer is provided for storing new data. */
#define OT_ERR_IVE_BUF_FULL OT_DEFINE_ERR(OT_ID_IVE, OT_ERR_LEVEL_ERROR, OT_ERR_BUF_FULL)
/* The system is not ready because it may be not initialized or loaded.
 * The error code is returned when a device file fails to be opened. */
#define OT_ERR_IVE_NOT_READY OT_DEFINE_ERR(OT_ID_IVE, OT_ERR_LEVEL_ERROR, OT_ERR_NOT_READY)
/* The source address or target address is incorrect during the operations such as calling
copy_from_user or copy_to_user. */
#define OT_ERR_IVE_BAD_ADDR OT_DEFINE_ERR(OT_ID_IVE, OT_ERR_LEVEL_ERROR, OT_ERR_BAD_ADDR)
/* The resource is busy during the operations such as destroying a VENC channel
without deregistering it. */
#define OT_ERR_IVE_BUSY OT_DEFINE_ERR(OT_ID_IVE, OT_ERR_LEVEL_ERROR, OT_ERR_BUSY)
/* IVE process timeout: 0xA01D8040 */
#define OT_ERR_IVE_SYS_TIMEOUT OT_DEFINE_ERR(OT_ID_IVE, OT_ERR_LEVEL_ERROR, OT_IVE_ERR_SYS_TIMEOUT)
/* IVE query timeout: 0xA01D8041 */
#define OT_ERR_IVE_QUERY_TIMEOUT OT_DEFINE_ERR(OT_ID_IVE, OT_ERR_LEVEL_ERROR, OT_IVE_ERR_QUERY_TIMEOUT)
/* IVE Bus error: 0xA01D8042 */
#define OT_ERR_IVE_BUS_ERR OT_DEFINE_ERR(OT_ID_IVE, OT_ERR_LEVEL_ERROR, OT_IVE_ERR_BUS_ERR)
/* IVE open file error: 0xA01D8043 */
#define OT_ERR_IVE_OPEN_FILE OT_DEFINE_ERR(OT_ID_IVE, OT_ERR_LEVEL_ERROR, OT_IVE_ERR_OPEN_FILE)
/* IVE read file error: 0xA01D8044 */
#define OT_ERR_IVE_READ_FILE OT_DEFINE_ERR(OT_ID_IVE, OT_ERR_LEVEL_ERROR, OT_IVE_ERR_READ_FILE)

/*
 * DMA mode
 */
typedef enum {
    OT_IVE_DMA_MODE_DIRECT_COPY = 0x0,
    OT_IVE_DMA_MODE_INTERVAL_COPY = 0x1,
    OT_IVE_DMA_MODE_SET_3BYTE = 0x2,
    OT_IVE_DMA_MODE_SET_8BYTE = 0x3,
    OT_IVE_DMA_MODE_MASK = 0x4,

    OT_IVE_DMA_MODE_BUTT
} ot_ive_dma_mode;

/*
 * DMA mask mode
 */
typedef enum {
    OT_IVE_DMA_MASK_MODE_32BIT = 0x0,
    OT_IVE_DMA_MASK_MODE_24BIT = 0x1,

    OT_IVE_DMA_MASK_MODE_BUTT
} ot_ive_dma_mask_mode;

typedef struct {
    ot_ive_dma_mode mode;
    ot_ive_dma_mask_mode  hor_mask_mode;  /* Horizontal mask mode */
    ot_ive_dma_mask_mode  ver_mask_mode;  /* Vertical mask mode */
    td_u64 val;             /* Used in memory-set mode */
    td_u32 hor_mask;        /* Horizontal mask value */
    td_u32 ver_mask;        /* Vertical mask value */
    td_u8  hor_seg_size;    /* Used in interval-copy mode, every row was segmented by hor_seg_size bytes,
                            restricted in values of 2,3,4,8,16 */
    td_u8 elem_size;        /* Used in interval-copy mode, the valid bytes copied in front of every segment
                            in a valid row, which 0<elem_size<hor_seg_size */
    td_u8 ver_seg_rows;     /* Used in interval-copy mode, copy one row in every ver_seg_rows */
} ot_ive_dma_ctrl;

typedef enum {
    OT_IVE_PADDING_MODE_EDGE = 0x0,
    OT_IVE_PADDING_MODE_CONSTANT = 0x1,

    OT_IVE_PADDING_MODE_BUTT
} ot_ive_padding_mode;

typedef struct {
    td_s8 mask[OT_IVE_MASK_NUM_121];     /* Template parameter filter coefficient */
    td_u8 norm;         /* Normalization parameter, by right shift */
    td_u8 kernel_w;
    td_u8 kernel_h;
    ot_ive_padding_mode padding_mode;
    td_s32 padding_val[OT_IVE_PADDING_ARR_NUM];
} ot_ive_filter_ctrl;

/*
 * CSC working mode.
 */
typedef enum {
    OT_IVE_CSC_MODE_VIDEO_BT601_YUV_TO_RGB = 0x0,  /* CSC: YUV2RGB, video transfer mode, RGB value range [16, 235] */
    OT_IVE_CSC_MODE_VIDEO_BT709_YUV_TO_RGB = 0x1,  /* CSC: YUV2RGB, video transfer mode, RGB value range [16, 235] */
    OT_IVE_CSC_MODE_PIC_BT601_YUV_TO_RGB = 0x2,    /* CSC: YUV2RGB, picture transfer mode, RGB value range [0, 255] */
    OT_IVE_CSC_MODE_PIC_BT709_YUV_TO_RGB = 0x3,    /* CSC: YUV2RGB, picture transfer mode, RGB value range [0, 255] */
    OT_IVE_CSC_MODE_PIC_BT601_YUV_TO_HSV = 0x4,    /* CSC: YUV2HSV, picture transfer mode, HSV value range [0, 255] */
    OT_IVE_CSC_MODE_PIC_BT709_YUV_TO_HSV = 0x5,    /* CSC: YUV2HSV, picture transfer mode, HSV value range [0, 255] */
    OT_IVE_CSC_MODE_PIC_BT601_YUV_TO_LAB = 0x6,    /* CSC: YUV2LAB, picture transfer mode, Lab value range [0, 255] */
    OT_IVE_CSC_MODE_PIC_BT709_YUV_TO_LAB = 0x7,    /* CSC: YUV2LAB, picture transfer mode, Lab value range [0, 255] */
    OT_IVE_CSC_MODE_VIDEO_BT601_RGB_TO_YUV = 0x8,  /* CSC: RGB2YUV, video transfer mode, YUV value range [0, 255] */
    OT_IVE_CSC_MODE_VIDEO_BT709_RGB_TO_YUV = 0x9,  /* CSC: RGB2YUV, video transfer mode, YUV value range [0, 255] */
    OT_IVE_CSC_MODE_PIC_BT601_RGB_TO_YUV = 0xa,    /* CSC: RGB2YUV, picture transfer mode, Y:[16, 235],U\V:[16, 240] */
    OT_IVE_CSC_MODE_PIC_BT709_RGB_TO_YUV = 0xb,    /* CSC: RGB2YUV, picture transfer mode, Y:[16, 235],U\V:[16, 240] */

    OT_IVE_CSC_MODE_BUTT
} ot_ive_csc_mode;

typedef struct {
    ot_ive_csc_mode mode; /* Working mode */
} ot_ive_csc_ctrl;

typedef struct {
    ot_ive_csc_mode mode;   /* CSC working mode */
    td_s8 mask[OT_IVE_MASK_NUM];         /* Template parameter filter coefficient */
    td_u8 norm;             /* Normalization parameter, by right shift */
} ot_ive_filter_and_csc_ctrl;

/*
 * sobel output ctrl
 */
typedef enum {
    OT_IVE_SOBEL_OUT_CTRL_BOTH = 0x0, /* Output horizontal and vertical */
    OT_IVE_SOBEL_OUT_CTRL_HOR = 0x1,  /* Output horizontal */
    OT_IVE_SOBEL_OUT_CTRL_VER = 0x2,  /* Output vertical */
    OT_IVE_SOBEL_OUT_CTRL_BUTT
} ot_ive_sobel_out_ctrl;


typedef struct {
    ot_ive_sobel_out_ctrl out_ctrl;                 /* Output format */
    td_s8 mask[OT_IVE_MASK_NUM];                    /* Template parameter */
} ot_ive_sobel_ctrl;

/*
 * Type of the magnitude and angle output results
 */
typedef enum {
    OT_IVE_MAG_AND_ANG_OUT_CTRL_MAG = 0x0,         /* Only the magnitude is output. */
    OT_IVE_MAG_AND_ANG_OUT_CTRL_MAG_AND_ANG = 0x1, /* The magnitude and angle are output. */
    OT_IVE_MAG_AND_ANG_OUT_CTRL_BUTT
} ot_ive_mag_and_ang_out_ctrl;

typedef struct {
    ot_ive_mag_and_ang_out_ctrl out_ctrl;
    td_u16 threshold;
    td_s8 mask[OT_IVE_MASK_NUM]; /* Template parameter. */
} ot_ive_mag_and_ang_ctrl;

typedef struct {
    td_u8 mask[OT_IVE_MASK_NUM]; /* The template parameter value must be 0 or 255. */
} ot_ive_dilate_ctrl;

typedef struct {
    td_u8 mask[OT_IVE_MASK_NUM]; /* The template parameter value must be 0 or 255. */
} ot_ive_erode_ctrl;

/*
 * Type of the threshold mode.
 */
typedef enum {
    OT_IVE_THRESHOLD_MODE_BINARY = 0x0,
    OT_IVE_THRESHOLD_MODE_TRUNC = 0x1,
    OT_IVE_THRESHOLD_MODE_TO_MIN_VAL = 0x2,
    OT_IVE_THRESHOLD_MODE_MIN_MID_MAX = 0x3,
    OT_IVE_THRESHOLD_MODE_ORIG_MID_MAX = 0x4,
    OT_IVE_THRESHOLD_MODE_MIN_MID_ORI = 0x5,
    OT_IVE_THRESHOLD_MODE_MIN_ORIG_MAX = 0x6,
    OT_IVE_THRESHOLD_MODE_ORI_MID_ORIG = 0x7,

    OT_IVE_THRESHOLD_MODE_BUTT
} ot_ive_threshold_mode;

typedef struct {
    ot_ive_threshold_mode mode;
    td_u8 low_threshold;  /* user-defined threshold,  0<=low_threshold<=255 */
    td_u8 high_threshold; /* user-defined threshold, if mode<OT_IVE_THRESHOLD_MODE_MIN_MID_MAX, high_thresh is not used,
                      else 0<=low_threshold<=high_threshold<=255; */
    td_u8 min_val;  /* Minimum value when tri-level thresholding */
    td_u8 mid_val;  /* Middle value when tri-level thresholding, if mode<2, mid_val is not used; */
    td_u8 max_val;  /* Maxmum value when tri-level thresholding */
} ot_ive_threshold_ctrl;

/*
 * Type of the sub output results
 */
typedef enum {
    OT_IVE_SUB_MODE_ABS = 0x0,      /* Absolute value of the difference */
    OT_IVE_SUB_MODE_SHIFT = 0x1,    /* The output result is obtained by shifting the result one digit right
                                    to reserve the signed bit. */
    OT_IVE_SUB_MODE_BUTT
} ot_ive_sub_mode;

typedef struct {
    ot_ive_sub_mode mode;
} ot_ive_sub_ctrl;

/*
 * Type of the integ output results
 */
typedef enum {
    OT_IVE_INTEG_OUT_CTRL_COMBINE = 0x0,
    OT_IVE_INTEG_OUT_CTRL_SUM = 0x1,
    OT_IVE_INTEG_OUT_CTRL_SQRT_SUM = 0x2,
    OT_IVE_INTEG_OUT_CTRL_BUTT
} ot_ive_integ_out_ctrl;

typedef struct {
    ot_ive_integ_out_ctrl out_ctrl;
} ot_ive_integ_ctrl;

/*
 * Type of the thresh_s16 mode
 */
typedef enum {
    OT_IVE_THRESHOLD_S16_MODE_S16_TO_S8_MIN_MID_MAX = 0x0,
    OT_IVE_THRESHOLD_S16_MODE_S16_TO_S8_MIN_ORIG_MAX = 0x1,
    OT_IVE_THRESHOLD_S16_MODE_S16_TO_U8_MIN_MID_MAX = 0x2,
    OT_IVE_THRESHOLD_S16_MODE_S16_TO_U8_MIN_ORIG_MAX = 0x3,

    OT_IVE_THRESHOLD_S16_MODE_BUTT
} ot_ive_threshold_s16_mode;

typedef struct {
    ot_ive_threshold_s16_mode mode;
    td_s16 low_threshold;           /* User-defined threshold */
    td_s16 high_threshold;          /* User-defined threshold */
    ot_svp_8bit min_val;            /* Minimum value when tri-level thresholding */
    ot_svp_8bit mid_val;            /* Middle value when tri-level thresholding */
    ot_svp_8bit max_val;            /* Maxmum value when tri-level thresholding */
} ot_ive_threshold_s16_ctrl;

/*
 * Type of the thresh_u16 mode
 */
typedef enum {
    OT_IVE_THRESHOLD_U16_MODE_U16_TO_U8_MIN_MID_MAX = 0x0,
    OT_IVE_THRESHOLD_U16_MODE_U16_TO_U8_MIN_ORIG_MAX = 0x1,

    OT_IVE_THRESHOLD_U16_MODE_BUTT
} ot_ive_threshold_u16_mode;

typedef struct {
    ot_ive_threshold_u16_mode mode;
    td_u16 low_threshold;
    td_u16 high_threshold;
    td_u8 min_val;
    td_u8 mid_val;
    td_u8 max_val;
} ot_ive_threshold_u16_ctrl;

/*
 * Mode of 16bit_to_8bit
 */
typedef enum {
    OT_IVE_16BIT_TO_8BIT_MODE_S16_TO_S8 = 0x0,
    OT_IVE_16BIT_TO_8BIT_MODE_S16_TO_U8_ABS = 0x1,
    OT_IVE_16BIT_TO_8BIT_MODE_S16_TO_U8_BIAS = 0x2,
    OT_IVE_16BIT_TO_8BIT_MODE_U16_TO_U8 = 0x3,

    OT_IVE_16BIT_TO_8BIT_MODE_BUTT
} ot_ive_16bit_to_8bit_mode;

typedef struct {
    ot_ive_16bit_to_8bit_mode mode;
    td_u16 denominator;
    td_u8 num;
    td_s8 bias;
} ot_ive_16bit_to_8bit_ctrl;

/*
 * Type of the ord_stat_filter
 */
typedef enum {
    OT_IVE_ORDER_STATS_FILTER_MODE_MEDIAN = 0x0,
    OT_IVE_ORDER_STATS_FILTER_MODE_MAX = 0x1,
    OT_IVE_ORDER_STATS_FILTER_MODE_MIN = 0x2,

    OT_IVE_ORD_STAT_FILTER_MODE_BUTT
} ot_ive_order_stats_filter_mode;

typedef struct {
    ot_ive_order_stats_filter_mode mode;
} ot_ive_order_stats_filter_ctrl;

/*
 * Type of the map
 */
typedef enum {
    OT_IVE_MAP_MODE_U8 = 0x0,
    OT_IVE_MAP_MODE_S16 = 0x1,
    OT_IVE_MAP_MODE_U16 = 0x2,

    OT_IVE_MAP_MODE_BUTT
} ot_ive_map_mode;

typedef struct {
    ot_ive_map_mode mode;
} ot_ive_map_ctrl;

typedef struct {
    td_u8 map[OT_IVE_MAP_NUM];
} ot_ive_map_u8bit_lut_mem;

typedef struct {
    td_u16 map[OT_IVE_MAP_NUM];
} ot_ive_map_u16bit_lut_mem;

typedef struct {
    td_s16 map[OT_IVE_MAP_NUM];
} ot_ive_map_s16bit_lut_mem;

typedef struct {
    td_u32 hist[OT_IVE_HIST_NUM];
    td_u8 map[OT_IVE_MAP_NUM];
} ot_ive_equalize_hist_ctrl_mem;

typedef struct {
    ot_svp_mem_info mem;
} ot_ive_equalize_hist_ctrl;

typedef struct {
    td_u0q16 x; /* x of "xA+yB" */
    td_u0q16 y; /* y of "xA+yB" */
} ot_ive_add_ctrl;

typedef struct {
    td_u64 num;
    td_u64 quad_sum1;
    td_u64 quad_sum2;
    td_u8 reserved[OT_IVE_ARR_RESERVED_NUM_EIGHT];
} ot_ive_ncc_dst_mem;

typedef struct {
    td_u32 area;   /* Represented by the pixel number */
    td_u16 left;   /* Circumscribed rectangle left border */
    td_u16 right;  /* Circumscribed rectangle right border */
    td_u16 top;    /* Circumscribed rectangle top border */
    td_u16 bottom; /* Circumscribed rectangle bottom border */
} ot_ive_rgn;

typedef union {
    struct {
        td_u32 cur_area_threshold : 23;  /* Threshold of the result regions' area */
        td_u32 label_status : 1;         /* 1: Labeled failed ; 0: Labeled successfully */
        td_u32 rgn_num : 8;              /* Number of valid region, non-continuous stored */
    } bits;
    td_u32 u32;
} ot_ive_ccblob_info;

typedef struct {
    ot_ive_ccblob_info info;
    ot_ive_rgn rgn[OT_IVE_MAX_RGN_NUM];  /* Valid regions with 'cur_area_threshold>0' and 'label = array_index+1' */
} ot_ive_ccblob;

/*
 * Type of the ccl
 */
typedef enum {
    OT_IVE_CCL_MODE_4C = 0x0, /* 4-connected */
    OT_IVE_CCL_MODE_8C = 0x1, /* 8-connected */

    OT_IVE_CCL_MODE_BUTT
} ot_ive_ccl_mode;

typedef struct {
    ot_ive_ccl_mode mode;           /* Mode */
    td_u16 init_area_threshold;     /* Init threshold of region area */
    td_u16 step;                    /* Increase area step for once */
} ot_ive_ccl_ctrl;

/*
 * GMM control struct
 */
typedef struct {
    td_u22q10 noise_var;        /* Initial noise Variance */
    td_u22q10 max_var;          /* Max  Variance */
    td_u22q10 min_var;          /* Min  Variance */
    td_u0q16 learn_rate;        /* Learning rate */
    td_u0q16 bg_ratio;          /* Background ratio */
    td_u8q8 var_threshold;      /* Variance Threshold */
    td_u0q16 init_wgt;          /* Initial Weight */
    td_u8 model_num;            /* Model number: 3 or 5 */
} ot_ive_gmm_ctrl;

/*
 * Type of the GMM2 sensitivity factor mode
 */
typedef enum {
    OT_IVE_GMM2_SNS_FACTOR_MODE_GLOBAL = 0x0,   /* Global sensitivity factor mode */
    OT_IVE_GMM2_SNS_FACTOR_MODE_PIXEL = 0x1,    /* Pixel sensitivity factor mode */

    OT_IVE_GMM2_SNS_FACTOR_MODE_BUTT
} ot_ive_gmm2_sns_factor_mode;

/*
 * Type of the GMM2 life update factor mode
 */
typedef enum {
    OT_IVE_GMM2_LIFE_UPDATE_FACTOR_MODE_GLOBAL  = 0x0,      /* Global life update factor mode */
    OT_IVE_GMM2_LIFE_UPDATE_FACTOR_MODE_PIXEL     = 0x1,    /* Pixel life update factor mode */

    OT_IVE_GMM2_LIFE_UPDATE_FACTOR_MODE_BUTT
} ot_ive_gmm2_life_update_factor_mode;

typedef struct {
    ot_ive_gmm2_sns_factor_mode sns_factor_mode;                    /* Sensitivity factor mode */
    ot_ive_gmm2_life_update_factor_mode life_update_factor_mode;    /* Life update factor mode */
    td_u16 global_life_update_factor;                               /* Global life update factor (default: 4) */
    td_u16 life_threshold;                                          /* Life threshold (default: 5000) */
    td_u16 freq_init_val;                                           /* Initial frequency (default: 20000) */
    td_u16 freq_reduce_factor;                                      /* Frequency reduction factor (default: 0xFF00) */
    td_u16 freq_add_factor;                                         /* Frequency adding factor (default: 0xEF) */
    td_u16 freq_threshold;                                          /* Frequency threshold (default: 12000) */
    td_u16 var_rate;                                                /* Variation update rate (default: 1) */
    td_u9q7 max_var;                                                /* Max variation (default: (16 * 16)<<7) */
    td_u9q7 min_var;                                                /* Min variation (default: ( 8 *  8)<<7) */
    td_u8 global_sns_factor;                                        /* Global sensitivity factor (default: 8) */
    td_u8 model_num;                                                /* Model number (range: 1~5, default: 3) */
} ot_ive_gmm2_ctrl;

typedef struct {
    ot_svp_mem_info mem;
    td_u16 low_threshold;
    td_u16 high_threshold;
    td_s8 mask[OT_IVE_MASK_NUM];
} ot_ive_canny_hys_edge_ctrl;

typedef struct {
    td_u32 stack_size;  /* Stack size for output */
    td_u8 reserved[OT_IVE_ARR_RESERVED_NUM_TWELVE]; /* For 16 byte align */
} ot_ive_canny_stack_size;

/*
 * LBP compare mode
 */
typedef enum {
    OT_IVE_LBP_COMPARE_MODE_NORMAL = 0x0,
    OT_IVE_LBP_COMPARE_MODE_ABS = 0x1,

    OT_IVE_LBP_COMPARE_MODE_BUTT
} ot_ive_lbp_compare_mode;

typedef struct {
    ot_ive_lbp_compare_mode mode;
    ot_svp_8bit bit_threshold;
} ot_ive_lbp_ctrl;

/*
 * Type of the gradient_filter output format
 */
typedef enum {
    OT_IVE_NORM_GRAD_OUT_CTRL_HOR_AND_VER = 0x0,
    OT_IVE_NORM_GRAD_OUT_CTRL_HOR = 0x1,
    OT_IVE_NORM_GRAD_OUT_CTRL_VER = 0x2,
    OT_IVE_NORM_GRAD_OUT_CTRL_COMBINE = 0x3,

    OT_IVE_NORM_GRAD_OUT_CTRL_BUTT
} ot_ive_norm_grad_out_ctrl;

typedef struct {
    ot_ive_norm_grad_out_ctrl out_ctrl;
    td_s8 mask[OT_IVE_MASK_NUM];
    td_u8 norm;
} ot_ive_norm_grad_ctrl;

/*
 * lk_optical_flow_pyr output mode
 */
typedef enum {
    OT_IVE_LK_OPTICAL_FLOW_PYR_OUT_MODE_NONE = 0,   /* Output none */
    OT_IVE_LK_OPTICAL_FLOW_PYR_OUT_MODE_STATUS = 1, /* Output status */
    OT_IVE_LK_OPTICAL_FLOW_PYR_OUT_MODE_BOTH = 2,   /* Output status and err */

    OT_IVE_LK_OPTICAL_FLOW_PYR_OUT_MODE_BUTT
} ot_ive_lk_optical_flow_pyr_out_mode;

typedef struct {
    ot_ive_lk_optical_flow_pyr_out_mode out_mode;
    td_bool use_init_flow;          /* where to use initial flow */
    td_u16 points_num;              /* Number of the feature points,<=500 */
    td_u8 max_level;                /* max_level should be in [0, 3] */
    td_u0q8 min_eig_val_threshold;  /* Minimum eigenvalue threshold */
    td_u8 iter_cnt;                 /* Maximum iteration times, <=20 */
    td_u0q8 eps;                    /* Used for exit criteria: dx^2 + dy^2 < eps */
} ot_ive_lk_optical_flow_pyr_ctrl;

typedef struct {
    td_u16 max_eig_val;     /* Shi-Tomasi-like second step output MaxEig */
    td_u8 reserved[OT_IVE_ARR_RESERVED_NUM_FOURTEEN]; /* For 16 byte align */
} ot_ive_st_max_eig_val;

typedef struct {
    ot_svp_mem_info mem;
    td_u0q8 quality_level;
} ot_ive_st_cand_corner_ctrl;

typedef struct {
    td_u16 corner_num;
    ot_svp_point_u16 corner[OT_IVE_ST_MAX_CORNER_NUM];
} ot_ive_st_corner_info;

typedef struct {
    td_u16 max_corner_num;
    td_u16 min_dist;
} ot_ive_st_corner_ctrl;

/*
 * grad_fg mode enum
 */
typedef enum {
    OT_IVE_GRAD_FG_MODE_USE_CUR_GRAD = 0x0,
    OT_IVE_GRAD_FG_MODE_FIND_MIN_GRAD = 0x1,

    OT_IVE_GRAD_FG_MODE_BUTT
} ot_ive_grad_fg_mode;

typedef struct {
    ot_ive_grad_fg_mode mode;  /* Calculation mode */
    td_u16 edw_factor;         /* Edge width adjustment factor (range: 500 to 2000; default: 1000) */
    td_u8 crl_coef_threshold;  /* Gradient vector correlation coefficient threshold
                                (ranges: 50 to 100; default: 80) */
    td_u8 mag_crl_threshold;   /* Gradient amplitude threshold (range: 0 to 20; default: 4) */
    td_u8 min_mag_diff;        /* Gradient magnitude difference threshold (range: 2 to 8; default: 2) */
    td_u8 noise_val;           /* Gradient amplitude noise threshold (range: 1 to 8; default: 1) */
    td_u8 edw_dark;            /* Black pixels enable flag (range: 0 (no), 1 (yes); default: 1) */
} ot_ive_grad_fg_ctrl;

typedef struct {
    td_u8q4f4 mean;         /* Candidate background grays value */
    td_u16 start_time;      /* Candidate Background start time */
    td_u16 sum_access_time; /* Candidate Background cumulative access time */
    td_u16 short_keep_time; /* Candidate background short hold time */
    td_u8 chg_condition;    /* Time condition for candidate background into the changing state */
    td_u8 poten_bg_life;    /* Potential background cumulative access time */
} ot_ive_cand_bg_pixel;

typedef struct {
    td_u8q4f4 mean;                         /* 0# background grays value */
    td_u16 access_time;                     /* Background cumulative access time */
    td_u8 prev_gray;                        /* Gray value of last pixel */
    td_u5q3 diff_threshold;                 /* Differential threshold */
    td_u8 access_flag;                      /* Background access flag */
    td_u8 bg_gray[OT_IVE_ARR_NUM_THREE];    /* 1# ~ 3# background grays value */
} ot_ive_wrok_bg_pixel;

typedef struct {
    td_u8 work_bg_life[OT_IVE_ARR_NUM_THREE];   /* 1# ~ 3# background vitality */
    td_u8 candi_bg_life;                        /* Candidate background vitality */
} ot_ive_bg_life;

typedef struct {
    ot_ive_wrok_bg_pixel work_bg_pixel;     /* Working background */
    ot_ive_cand_bg_pixel cand_pixel;        /* Candidate background */
    ot_ive_bg_life bg_life;                 /* Background vitality */
} ot_ive_bg_model_pixel;

typedef struct {
    td_u32 pixel_num;
    td_u32 sum_luma;
    td_u8 reserved[OT_IVE_ARR_RESERVED_NUM_EIGHT];
} ot_ive_fg_status_data;

typedef struct {
    td_u32 pixel_num;
    td_u32 sum_luma;
    td_u8 reserved[OT_IVE_ARR_RESERVED_NUM_EIGHT];
} ot_ive_bg_status_data;

typedef struct {
    td_u32 cur_frame_num;         /* Current frame timestamp, in frame units */
    td_u32 prev_frame_num;         /* Previous frame timestamp, in frame units */
    td_u16 time_threshold;            /* Potential background replacement time threshold
                                (range: 2 to 100 frames; default: 20) */

    td_u8 diff_threshold_crl_coef;    /* Correlation coefficients between differential threshold and gray value
                                (range: 0 to 5; default: 0) */
    td_u8 diff_max_threshold;         /* Maximum of background differential threshold (range: 3 to 15; default: 6) */
    td_u8 diff_min_threshold;         /* Minimum of background differential threshold (range: 3 to 15; default: 4) */
    td_u8 diff_threshold_inc;         /* Dynamic Background differential threshold increment
                                (range: 0 to 6; default: 0) */
    td_u8 fast_learn_rate;      /* Quick background learning rate (range: 0 to 4; default: 2) */
    td_u8 detected_chg_rgn;       /* Whether to detect change region (range: 0 (no), 1 (yes); default: 0) */
} ot_ive_match_bg_model_ctrl;

typedef struct {
    td_u32 cur_frame_num;         /* Current frame timestamp, in frame units */
    td_u32 prev_check_time;        /* The last time when background status is checked */
    td_u32 frame_check_period;      /* Background status checking period (range: 0 to 2000 frames; default: 50) */

    td_u32 init_min_time;       /* Background initialization shortest time
                                (range: 20 to 6000 frames; default: 100) */
    td_u32 steady_bg_min_blend_time; /* Steady background integration shortest time
                                (range: 20 to 6000 frames; default: 200) */
    td_u32 steady_bg_max_blend_time; /* Steady background integration longest time
                                (range: 20 to 40000 frames; default: 1500) */
    td_u32 dynamic_bg_min_blend_time; /* Dynamic background integration shortest time
                                (range: 0 to 6000 frames; default: 0) */
    td_u32 static_detection_min_time;  /* Still detection shortest time (range: 20 to 6000 frames; default: 80) */
    td_u16 fg_max_fade_time;     /* Foreground disappearing longest time
                                (range: 1 to 255 seconds; default: 15) */
    td_u16 bg_max_fade_time;     /* Background disappearing longest time
                                (range: 1 to 255  seconds ; default: 60) */

    td_u8 steay_bg_access_time_rate_threshold; /* Steady background access time ratio threshold
                                (range: 10 to 100; default: 80) */
    td_u8 chg_bg_access_time_rate_threshold; /* Change background access time ratio threshold
                                (range: 10 to 100; default: 60) */
    td_u8 dynamic_bg_access_time_threshold;     /* Dynamic background access time ratio threshold
                                (range: 0 to 50; default: 0) */
    td_u8 dynamic_bg_depth;          /* Dynamic background depth (range: 0 to 3; default: 3) */
    td_u8 bg_eff_state_rate_threshold;     /* Background state time ratio threshold when initializing
                                (range: 90 to 100; default: 90) */

    td_u8 acc_bg_learn;  /* Whether to accelerate background learning (range: 0 (no), 1 (yes); default: 0) */
    td_u8 detected_chg_rgn; /* Whether to detect change region (range: 0 (no), 1 (yes); default: 0) */
} ot_ive_update_bg_model_ctrl;

typedef enum {
    OT_IVE_ANN_MLP_ACTV_FUNC_IDENTITY = 0x0,
    OT_IVE_ANN_MLP_ACTV_FUNC_SIGMOID_SYM = 0x1,
    OT_IVE_ANN_MLP_ACTV_FUNC_GAUSSIAN = 0x2,

    OT_IVE_ANN_MLP_ACTV_FUNC_BUTT
} ot_ive_ann_mlp_actv_func;

typedef enum {
    OT_IVE_ANN_MLP_ACCURATE_SRC16_WGT16 = 0x0, /* input decimals' accurate 16 bit, wgt 16bit */
    OT_IVE_ANN_MLP_ACCURATE_SRC14_WGT20 = 0x1, /* input decimals' accurate 14 bit, wgt 20bit */

    OT_IVE_ANN_MLP_ACCURATE_BUTT
} ot_ive_ann_mlp_accurate;

typedef struct {
    ot_ive_ann_mlp_actv_func actv_func;
    ot_ive_ann_mlp_accurate accurate;
    ot_svp_mem_info wgt;
    td_u32 total_wgt_size;

    td_u16 layer_cnt[OT_IVE_ARR_NUM_EIGHT];      /* 8 layers, including input and output layer */
    td_u16 max_count;           /* MaxCount should be less than and equal 1024 */
    td_u8 layer_num;            /* layerNum should in (2,8] */
    td_u8 reserved;
} ot_ive_ann_mlp_model;

typedef enum {
    OT_IVE_SVM_TYPE_C_SVC = 0x0,
    OT_IVE_SVM_TYPE_NU_SVC = 0x1,

    OT_IVE_SVM_TYPE_BUTT
} ot_ive_svm_type;

typedef enum {
    OT_IVE_SVM_KERNEL_TYPE_LINEAR = 0x0,
    OT_IVE_SVM_KERNEL_TYPE_POLY = 0x1,
    OT_IVE_SVM_KERNEL_TYPE_RBF = 0x2,
    OT_IVE_SVM_KERNEL_TYPE_SIGMOID = 0x3,

    OT_IVE_SVM_KERNEL_TYPE_BUTT
} ot_ive_svm_kernel_type;

typedef struct {
    ot_ive_svm_type type;
    ot_ive_svm_kernel_type kernel_type;

    ot_svp_mem_info sv;                 /* SV memory */
    ot_svp_mem_info decision_func;      /* Decision functions memory */
    td_u32 total_decision_func_size;    /* All decision functions coef size in byte */

    td_u16 feature_dim;
    td_u16 sv_total;
    td_u8 class_cnt;
} ot_ive_svm_model;

/*
 * Sad mode
 */
typedef enum {
    OT_IVE_SAD_MODE_MB_4X4 = 0x0,   /* 4x4 */
    OT_IVE_SAD_MODE_MB_8X8 = 0x1,   /* 8x8 */
    OT_IVE_SAD_MODE_MB_16X16 = 0x2, /* 16x16 */

    OT_IVE_SAD_MODE_BUTT
} ot_ive_sad_mode;
/*
 * Sad output ctrl
 */
typedef enum {
    OT_IVE_SAD_OUT_CTRL_16BIT_BOTH = 0x0, /* Output 16 bit sad and threshold */
    OT_IVE_SAD_OUT_CTRL_8BIT_BOTH = 0x1,  /* Output 8 bit sad and threshold */
    OT_IVE_SAD_OUT_CTRL_16BIT_SAD = 0x2,  /* Output 16 bit sad */
    OT_IVE_SAD_OUT_CTRL_8BIT_SAD = 0x3,   /* Output 8 bit sad */
    OT_IVE_SAD_OUT_CTRL_THRESHOLD = 0x4,  /* Output threshold,16 bits sad */

    OT_IVE_SAD_OUT_CTRL_BUTT
} ot_ive_sad_out_ctrl;

typedef struct {
    ot_ive_sad_mode mode;
    ot_ive_sad_out_ctrl out_ctrl;
    td_u16 threshold;
    td_u8 min_val;              /* Min value */
    td_u8 max_val;              /* Max value */
} ot_ive_sad_ctrl;

/*
 * Resize zoom mode
 */
typedef enum {
    OT_IVE_RESIZE_MODE_LINEAR = 0x0, /* Bilinear interpolation */

    OT_IVE_RESIZE_MODE_BUTT
} ot_ive_resize_mode;

typedef struct {
    ot_ive_resize_mode mode;
    ot_svp_mem_info mem;
    td_u16 num;
} ot_ive_resize_ctrl;

/*
 * CNN active function mode
 */
typedef enum {
    OT_IVE_CNN_ACTV_FUNC_NONE = 0x0,    /* 1. Do not taking a activation, equivalent f(x)=x */
    OT_IVE_CNN_ACTV_FUNC_RELU = 0x1,    /* 2. f(x)=max(0, x) */
    OT_IVE_CNN_ACTV_FUNC_SIGMOID = 0x2, /* 3. f(x)=1/(1+exp(-x)), not support */

    OT_IVE_CNN_ACTV_FUNC_BUTT
} ot_ive_cnn_actv_func;

/*
 * CNN pooling mode
 */
typedef enum {
    OT_IVE_CNN_POOLING_NONE = 0x0, /* Do not taking a pooling action */
    OT_IVE_CNN_POOLING_MAX = 0x1,  /* Using max value of every pooling area */
    OT_IVE_CNN_POOLING_AVG = 0x2,  /* Using average value of every pooling area */

    OT_IVE_CNN_POOLING_BUTT
} ot_ive_cnn_pooling;

typedef struct {
    ot_ive_cnn_actv_func actv_func;   /* Type of activation function */
    ot_ive_cnn_pooling pooling;       /* Mode of pooling method */

    td_u8 feature_map_num;            /* Number of feature maps */
    td_u8 kernel_size;                /* Kernel size, only support 3 currently */
    td_u8 conv_step;                  /* Convolution step, only support 1 currently */

    td_u8 pool_size;                  /* Pooling size, only support 2 currently */
    td_u8 pool_step;                  /* Pooling step, only support 2 currently */
    td_u8 reserved[OT_IVE_ARR_RESERVED_NUM_THREE];
} ot_ive_cnn_conv_pooling;

typedef struct {
    td_u16 layer_cnt[OT_IVE_ARR_NUM_EIGHT];    /* Neuron number of every fully connected layers */
    td_u16 max_cnt;         /* Max neuron number in all fully connected layers */
    td_u8 layer_num;        /* Number of fully connected layer */
    td_u8 reserved;
} ot_ive_cnn_fc_info;

typedef struct {
    ot_ive_cnn_conv_pooling conv_pool[OT_IVE_ARR_NUM_EIGHT];  /* Conv-ReLU-Pooling layers info */
    ot_ive_cnn_fc_info fc_info;  /* Fully connected layers info */

    ot_svp_mem_info conv_kernel_bias;      /* Conv-ReLU-Pooling layers' kernels and bias */
    td_u32 conv_kernel_bias_size;          /* Size of Conv-ReLU-Pooling layer' kernels and bias */

    ot_svp_mem_info fc_wgt_bias;          /* Fully Connection Layers' weights and bias */
    td_u32 fc_wgt_bias_size;              /* Size of fully connection layers weights and bias */

    td_u32 total_mem_size;                 /* Total memory size of all kernels, weights, bias */

    ot_svp_img_type type;                /* Image type used for the CNN model */
    td_u32 width;                          /* Image width used for the model */
    td_u32 height;                         /* Image height used for the model */

    td_u16 class_cnt;                    /* Number of classes */
    td_u8 conv_pool_layer_num;             /* Number of Conv-ReLU-Pooling layers */
    td_u8 reserved;
} ot_ive_cnn_model;

typedef struct {
    ot_svp_mem_info mem;    /* Assist memory */
    td_u32 num;             /* Input image number */
} ot_ive_cnn_ctrl;

typedef struct {
    td_s32 class_idx;       /* The most possible index of the classification */
    td_s32 conf;            /* The conf of the classification */
} ot_ive_cnn_result;

/*
 * Perspective transform algorithm mode
 */
typedef enum {
    OT_IVE_PERSP_TRANS_ALG_MODE_NR_SIM = 0x0, /* Non-reflective similarity transform mode */
    OT_IVE_PERSP_TRANS_ALG_MODE_SIM = 0x1,    /* Reflective similarity transform mode */
    OT_IVE_PERSP_TRANS_ALG_MODE_AFFINE = 0x2, /* Affine transform mode */

    OT_IVE_PERSP_TRANS_ALG_MODE_BUTT
} ot_ive_persp_trans_alg_mode;

typedef struct {
    ot_svp_point_u14q2 src_point; /* Source point */
    ot_svp_point_u14q2 dst_point; /* Destination point */
} ot_ive_persp_trans_point_pair;

/*
 * Perspective transform csc mode
 */
typedef enum {
    OT_IVE_PERSP_TRANS_CSC_MODE_NONE = 0x0,                /* No do csc */
    OT_IVE_PERSP_TRANS_CSC_MODE_VIDEO_BT601_YUV_TO_RGB = 0x1, /* CSC: YUV2RGB, video transfer mode,
                                                      RGB value range [16, 235] */
    OT_IVE_PERSP_TRANS_CSC_MODE_VIDEO_BT709_YUV_TO_RGB = 0x2, /* CSC: YUV2RGB, video transfer mode,
                                                      RGB value range [16, 235] */
    OT_IVE_PERSP_TRANS_CSC_MODE_PIC_BT601_YUV_TO_RGB = 0x3,   /* CSC: YUV2RGB, picture transfer mode,
                                                      RGB value range [0, 255] */
    OT_IVE_PERSP_TRANS_CSC_MODE_PIC_BT709_YUV_TO_RGB = 0x4,   /* CSC: YUV2RGB, picture transfer mode,
                                                      RGB value range [0, 255] */

    OT_IVE_PERSP_TRANS_CSC_MODE_BUTT
} ot_ive_persp_trans_csc_mode;

typedef struct {
    ot_ive_persp_trans_alg_mode alg_mode;   /* Alg mode */
    ot_ive_persp_trans_csc_mode csc_mode;   /* CSC mode */
    td_u16 roi_num;                         /* Roi number */
    td_u16 point_pair_num;                  /* Point pair number  */
} ot_ive_persp_trans_ctrl;

typedef enum {
    OT_IVE_KCF_CORE0 = 0x0,

    OT_IVE_KCF_CORE_BUTT
} ot_ive_kcf_core_id;

typedef struct {
    ot_svp_rect_s24q8 roi;
    td_u32 roi_id;
} ot_ive_roi_info;

/* Kcf ctrl param */
typedef struct {
    ot_ive_csc_mode csc_mode;   /* Only support:
                                OT_IVE_CSC_MODE_VIDEO_BT601_YUV_TO_RGB  CSC: YUV2RGB, video transfer mode,
                                RGB value range [16, 235]
                                OT_IVE_CSC_MODE_VIDEO_BT709_YUV_TO_RGB  CSC: YUV2RGB, video transfer mode,
                                RGB value range [16, 235]
                                OT_IVE_CSC_MODE_PIC_BT601_YUV_TO_RGB  CSC: YUV2RGB, picture transfer mode,
                                RGB value range [0, 255]
                                OT_IVE_CSC_MODE_PIC_BT709_YUV_TO_RGB  CSC: YUV2RGB, picture transfer mode,
                                RGB value range [0, 255]
                                */
    ot_svp_mem_info tmp_buf;
    td_u1q15 interp_factor; /* Blend coefficient. [0, 32768] */
    td_u0q16 lamda;        /* The regularization coefficient. [0, 65535] */
    td_u4q12 norm_trunc_alfa;   /* The normalization truncation threshold. [0, 4095] */
    td_u0q8 sigma;         /* The gaussian kernel bandwidth. [0, 255]  */
    td_u8 response_threshold;
    ot_ive_kcf_core_id core_id;
} ot_ive_kcf_proc_ctrl;

typedef struct {
    ot_ive_roi_info roi_info;
    ot_svp_mem_info cos_win_x;
    ot_svp_mem_info cos_win_y;
    ot_svp_mem_info gauss_peak;
    ot_svp_mem_info hog_feature;
    ot_svp_mem_info alpha;
    ot_svp_mem_info dst;
    td_u3q5 padding; /* [48, 160]  */
    td_u8 reserved[OT_IVE_ARR_RESERVED_NUM_THREE];
} ot_ive_kcf_obj;

typedef struct tag_ot_ive_list_head {
    struct tag_ot_ive_list_head *next, *prev;
} ot_ive_list_head;

typedef struct {
    ot_ive_list_head list;
    ot_ive_kcf_obj kcf_obj;
} ot_ive_kcf_obj_node;

typedef enum {
    OT_IVE_KCF_LIST_STATE_CREATE = 0x1,
    OT_IVE_KCF_LIST_STATE_DESTORY = 0x2,
    OT_IVE_KCF_LIST_STATE_BUTT
} ot_ive_kcf_list_state;

typedef struct {
    ot_ive_kcf_obj_node *obj_node_buf;  /* The object list node address */
    ot_ive_list_head free_obj_list;     /* The free list of object list */
    ot_ive_list_head train_obj_list;    /* The training list of object list */
    ot_ive_list_head track_obj_list;    /* The tracking list of object list */

    td_u32 free_obj_num;                /* The numbers of free list */
    td_u32 train_obj_num;               /* The numbers of training list */
    td_u32 track_obj_num;               /* The numbers of tracking list */
    td_u32 max_obj_num;                 /* The maximum numbers of object list */
    ot_ive_kcf_list_state list_state;   /* The object list state */
    td_u8 *tmp_buf;                     /* Assist buffer */
    td_u32 width;                       /* image width */
    td_u32 height;                      /* image height */
} ot_ive_kcf_obj_list;

typedef struct {
    ot_ive_kcf_obj_node *node;
    td_s32 response; /* Bbox Response value. */

    ot_ive_roi_info roi_info;
    td_bool is_track_ok;
    td_bool is_roi_refresh;
} ot_ive_kcf_bbox;

typedef struct {
    td_u32 max_bbox_num;        /* The member numbers of Bbox Array. */
    td_s32 response_threshold;  /* Select Bbox when Bbox'Response value is greater than or equal to RespThr. */
} ot_ive_kcf_bbox_ctrl;

typedef enum {
    OT_IVE_HOG_MODE_VER_TANGENT_PLANE = 0x1,
    OT_IVE_HOG_MODE_HOR_TANGENT_PLANE = 0x2,
    OT_IVE_HOG_MODE_BUTT
} ot_ive_hog_mode;

typedef struct {
    ot_ive_csc_mode csc_mode;   /* Only support:
                                 * OT_IVE_CSC_MODE_VIDEO_BT601_YUV_TO_RGB  CSC: YUV2RGB, video transfer mode,
                                 * RGB value range [16, 235]
                                 * OT_IVE_CSC_MODE_VIDEO_BT709_YUV_TO_RGB  CSC: YUV2RGB, video transfer mode,
                                 * RGB value range [16, 235]
                                 * OT_IVE_CSC_MODE_PIC_BT601_YUV_TO_RGB  CSC: YUV2RGB, picture transfer mode,
                                 * RGB value range [0, 255]
                                 * OT_IVE_CSC_MODE_PIC_BT709_YUV_TO_RGB  CSC: YUV2RGB, picture transfer mode,
                                 * RGB value range [0, 255] */
    ot_ive_hog_mode hog_mode;   /* Hog mode */
    td_u32 roi_num;             /* Roi number. [1, 64] */
    td_u4q12 norm_trunc_alfa;   /* The normalization truncation threshold. [0, 4095] */

    ot_ive_kcf_core_id core_id;
} ot_ive_hog_ctrl;
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
#endif /* OT_COMMON_IVE_H */
