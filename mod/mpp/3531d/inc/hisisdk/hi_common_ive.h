/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2019-2020. All rights reserved.
 * Description: definition of hi_common_ive.h
 * Author: Hisilicon multimedia software (SVP) group
 * Create: 2019-06-30
 */

#ifndef _HI_COMMON_IVE_H_
#define _HI_COMMON_IVE_H_

#include "hi_common_svp.h"
#include "hi_common.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define HI_IVE_HIST_NUM                     256
#define HI_IVE_MAP_NUM                      256
#define HI_IVE_MAX_RGN_NUM                  254
#define HI_IVE_ST_MAX_CORNER_NUM            500
#define HI_IVE_MASK_NUM                     25
#define HI_IVE_ARR_RESERVED_NUM_TWO         2
#define HI_IVE_ARR_RESERVED_NUM_THREE       3
#define HI_IVE_ARR_RESERVED_NUM_EIGHT       8
#define HI_IVE_ARR_RESERVED_NUM_TWELVE      12
#define HI_IVE_ARR_RESERVED_NUM_FOURTEEN    14
#define HI_IVE_ARR_NUM_THREE                3
#define HI_IVE_ARR_NUM_EIGHT                8

typedef enum {
    HI_IVE_ERR_SYS_TIMEOUT      = 0x40,             /* IVE process timeout */
    HI_IVE_ERR_QUERY_TIMEOUT    = 0x41,             /* IVE query timeout */
    HI_IVE_ERR_BUS_ERR          = 0x42,             /* IVE BUS error */
    HI_IVE_ERR_OPEN_FILE        = 0x43,             /* IVE open file error */
    HI_IVE_ERR_READ_FILE        = 0x44,             /* IVE read file error */

    HI_IVE_ERR_BUTT
} hi_ive_err_code;

/************************************************IVE error code ***********************************/
/* Invalid device ID */
#define HI_ERR_IVE_INVALID_DEV_ID HI_DEFINE_ERR(HI_ID_IVE, HI_ERR_LEVEL_ERROR, HI_ERR_INVALID_DEV_ID)
/* Invalid channel ID */
#define HI_ERR_IVE_INVALID_CHN_ID HI_DEFINE_ERR(HI_ID_IVE, HI_ERR_LEVEL_ERROR, HI_ERR_INVALID_CHN_ID)
/* At least one parameter is illegal. For example, an illegal enumeration value exists. */
#define HI_ERR_IVE_ILLEGAL_PARAM HI_DEFINE_ERR(HI_ID_IVE, HI_ERR_LEVEL_ERROR, HI_ERR_ILLEGAL_PARAM)
/* The channel exists. */
#define HI_ERR_IVE_EXIST HI_DEFINE_ERR(HI_ID_IVE, HI_ERR_LEVEL_ERROR, HI_ERR_EXIST)
/* The UN exists. */
#define HI_ERR_IVE_UNEXIST HI_DEFINE_ERR(HI_ID_IVE, HI_ERR_LEVEL_ERROR, HI_ERR_UNEXIST)
/* A null point is used. */
#define HI_ERR_IVE_NULL_PTR HI_DEFINE_ERR(HI_ID_IVE, HI_ERR_LEVEL_ERROR, HI_ERR_NULL_PTR)
/* Try to enable or initialize the system, device, or channel before configuring attributes. */
#define HI_ERR_IVE_NOT_CFG HI_DEFINE_ERR(HI_ID_IVE, HI_ERR_LEVEL_ERROR, HI_ERR_NOT_CFG)
/* The operation is not supported currently. */
#define HI_ERR_IVE_NOT_SURPPORT HI_DEFINE_ERR(HI_ID_IVE, HI_ERR_LEVEL_ERROR, HI_ERR_NOT_SUPPORT)
/* The operation, changing static attributes for example, is not permitted. */
#define HI_ERR_IVE_NOT_PERM HI_DEFINE_ERR(HI_ID_IVE, HI_ERR_LEVEL_ERROR, HI_ERR_NOT_PERM)
/* A failure caused by the malloc memory occurs. */
#define HI_ERR_IVE_NO_MEM HI_DEFINE_ERR(HI_ID_IVE, HI_ERR_LEVEL_ERROR, HI_ERR_NO_MEM)
/* A failure caused by the malloc buffer occurs. */
#define HI_ERR_IVE_NO_BUF HI_DEFINE_ERR(HI_ID_IVE, HI_ERR_LEVEL_ERROR, HI_ERR_NO_BUF)
/* The buffer is empty. */
#define HI_ERR_IVE_BUF_EMPTY HI_DEFINE_ERR(HI_ID_IVE, HI_ERR_LEVEL_ERROR, HI_ERR_BUF_EMPTY)
/* No buffer is provided for storing new data. */
#define HI_ERR_IVE_BUF_FULL HI_DEFINE_ERR(HI_ID_IVE, HI_ERR_LEVEL_ERROR, HI_ERR_BUF_FULL)
/* The system is not ready because it may be not initialized or loaded.
 * The error code is returned when a device file fails to be opened. */
#define HI_ERR_IVE_NOT_READY HI_DEFINE_ERR(HI_ID_IVE, HI_ERR_LEVEL_ERROR, HI_ERR_NOT_READY)
/* The source address or target address is incorrect during the operations such as calling
copy_from_user or copy_to_user. */
#define HI_ERR_IVE_BAD_ADDR HI_DEFINE_ERR(HI_ID_IVE, HI_ERR_LEVEL_ERROR, HI_ERR_BAD_ADDR)
/* The resource is busy during the operations such as destroying a VENC channel
without deregistering it. */
#define HI_ERR_IVE_BUSY HI_DEFINE_ERR(HI_ID_IVE, HI_ERR_LEVEL_ERROR, HI_ERR_BUSY)
/* IVE process timeout: 0xA01D8040 */
#define HI_ERR_IVE_SYS_TIMEOUT HI_DEFINE_ERR(HI_ID_IVE, HI_ERR_LEVEL_ERROR, HI_IVE_ERR_SYS_TIMEOUT)
/* IVE query timeout: 0xA01D8041 */
#define HI_ERR_IVE_QUERY_TIMEOUT HI_DEFINE_ERR(HI_ID_IVE, HI_ERR_LEVEL_ERROR, HI_IVE_ERR_QUERY_TIMEOUT)
/* IVE Bus error: 0xA01D8042 */
#define HI_ERR_IVE_BUS_ERR HI_DEFINE_ERR(HI_ID_IVE, HI_ERR_LEVEL_ERROR, HI_IVE_ERR_BUS_ERR)
/* IVE open file error: 0xA01D8043 */
#define HI_ERR_IVE_OPEN_FILE HI_DEFINE_ERR(HI_ID_IVE, HI_ERR_LEVEL_ERROR, HI_IVE_ERR_OPEN_FILE)
/* IVE read file error: 0xA01D8044 */
#define HI_ERR_IVE_READ_FILE HI_DEFINE_ERR(HI_ID_IVE, HI_ERR_LEVEL_ERROR, HI_IVE_ERR_READ_FILE)

/*
 * DMA mode
 */
typedef enum {
    HI_IVE_DMA_MODE_DIRECT_COPY = 0x0,
    HI_IVE_DMA_MODE_INTERVAL_COPY = 0x1,
    HI_IVE_DMA_MODE_SET_3BYTE = 0x2,
    HI_IVE_DMA_MODE_SET_8BYTE = 0x3,
    HI_IVE_DMA_MODE_BUTT
} hi_ive_dma_mode;

typedef struct {
    hi_ive_dma_mode mode;
    hi_u64 val;             /* Used in memset mode */
    hi_u8  hor_seg_size;    /* Used in interval-copy mode, every row was segmented by hor_seg_size bytes,
                            restricted in values of 2,3,4,8,16 */
    hi_u8 elem_size;        /* Used in interval-copy mode, the valid bytes copied in front of every segment
                            in a valid row, which 0<elem_size<hor_seg_size */
    hi_u8 ver_seg_rows;     /* Used in interval-copy mode, copy one row in every ver_seg_rows */
} hi_ive_dma_ctrl;

typedef struct {
    hi_s8 mask[HI_IVE_MASK_NUM];     /* Template parameter filter coefficient */
    hi_u8 norm;         /* Normalization parameter, by right shift */
} hi_ive_filter_ctrl;

/*
 * CSC working mode.
 */
typedef enum {
    HI_IVE_CSC_MODE_VIDEO_BT601_YUV_TO_RGB = 0x0,  /* CSC: YUV2RGB, video transfer mode, RGB value range [16, 235] */
    HI_IVE_CSC_MODE_VIDEO_BT709_YUV_TO_RGB = 0x1,  /* CSC: YUV2RGB, video transfer mode, RGB value range [16, 235] */
    HI_IVE_CSC_MODE_PIC_BT601_YUV_TO_RGB = 0x2,    /* CSC: YUV2RGB, picture transfer mode, RGB value range [0, 255] */
    HI_IVE_CSC_MODE_PIC_BT709_YUV_TO_RGB = 0x3,    /* CSC: YUV2RGB, picture transfer mode, RGB value range [0, 255] */
    HI_IVE_CSC_MODE_PIC_BT601_YUV_TO_HSV = 0x4,    /* CSC: YUV2HSV, picture transfer mode, HSV value range [0, 255] */
    HI_IVE_CSC_MODE_PIC_BT709_YUV_TO_HSV = 0x5,    /* CSC: YUV2HSV, picture transfer mode, HSV value range [0, 255] */
    HI_IVE_CSC_MODE_PIC_BT601_YUV_TO_LAB = 0x6,    /* CSC: YUV2LAB, picture transfer mode, Lab value range [0, 255] */
    HI_IVE_CSC_MODE_PIC_BT709_YUV_TO_LAB = 0x7,    /* CSC: YUV2LAB, picture transfer mode, Lab value range [0, 255] */
    HI_IVE_CSC_MODE_VIDEO_BT601_RGB_TO_YUV = 0x8,  /* CSC: RGB2YUV, video transfer mode, YUV value range [0, 255] */
    HI_IVE_CSC_MODE_VIDEO_BT709_RGB_TO_YUV = 0x9,  /* CSC: RGB2YUV, video transfer mode, YUV value range [0, 255] */
    HI_IVE_CSC_MODE_PIC_BT601_RGB_TO_YUV = 0xa,    /* CSC: RGB2YUV, picture transfer mode, Y:[16, 235],U\V:[16, 240] */
    HI_IVE_CSC_MODE_PIC_BT709_RGB_TO_YUV = 0xb,    /* CSC: RGB2YUV, picture transfer mode, Y:[16, 235],U\V:[16, 240] */

    HI_IVE_CSC_MODE_BUTT
} hi_ive_csc_mode;

typedef struct {
    hi_ive_csc_mode mode; /* Working mode */
} hi_ive_csc_ctrl;

typedef struct {
    hi_ive_csc_mode mode;   /* CSC working mode */
    hi_s8 mask[HI_IVE_MASK_NUM];         /* Template parameter filter coefficient */
    hi_u8 norm;             /* Normalization parameter, by right shift */
} hi_ive_filter_and_csc_ctrl;

/*
 * sobel output ctrl
 */
typedef enum {
    HI_IVE_SOBEL_OUT_CTRL_BOTH = 0x0, /* Output horizontal and vertical */
    HI_IVE_SOBEL_OUT_CTRL_HOR = 0x1,  /* Output horizontal */
    HI_IVE_SOBEL_OUT_CTRL_VER = 0x2,  /* Output vertical */
    HI_IVE_SOBEL_OUT_CTRL_BUTT
} hi_ive_sobel_out_ctrl;


typedef struct {
    hi_ive_sobel_out_ctrl out_ctrl;                 /* Output format */
    hi_s8 mask[HI_IVE_MASK_NUM];                    /* Template parameter */
} hi_ive_sobel_ctrl;

/*
 * Type of the magnitude and angle output results
 */
typedef enum {
    HI_IVE_MAG_AND_ANG_OUT_CTRL_MAG = 0x0,         /* Only the magnitude is output. */
    HI_IVE_MAG_AND_ANG_OUT_CTRL_MAG_AND_ANG = 0x1, /* The magnitude and angle are output. */
    HI_IVE_MAG_AND_ANG_OUT_CTRL_BUTT
} hi_ive_mag_and_ang_out_ctrl;

typedef struct {
    hi_ive_mag_and_ang_out_ctrl out_ctrl;
    hi_u16 threshold;
    hi_s8 mask[HI_IVE_MASK_NUM]; /* Template parameter. */
} hi_ive_mag_and_ang_ctrl;

typedef struct {
    hi_u8 mask[HI_IVE_MASK_NUM]; /* The template parameter value must be 0 or 255. */
} hi_ive_dilate_ctrl;

typedef struct {
    hi_u8 mask[HI_IVE_MASK_NUM]; /* The template parameter value must be 0 or 255. */
} hi_ive_erode_ctrl;

/*
 * Type of the threshold mode.
 */
typedef enum {
    HI_IVE_THRESHOLD_MODE_BINARY = 0x0,
    HI_IVE_THRESHOLD_MODE_TRUNC = 0x1,
    HI_IVE_THRESHOLD_MODE_TO_MIN_VAL = 0x2,
    HI_IVE_THRESHOLD_MODE_MIN_MID_MAX = 0x3,
    HI_IVE_THRESHOLD_MODE_ORIG_MID_MAX = 0x4,
    HI_IVE_THRESHOLD_MODE_MIN_MID_ORI = 0x5,
    HI_IVE_THRESHOLD_MODE_MIN_ORIG_MAX = 0x6,
    HI_IVE_THRESHOLD_MODE_ORI_MID_ORIG = 0x7,

    HI_IVE_THRESHOLD_MODE_BUTT
} hi_ive_threshold_mode;

typedef struct {
    hi_ive_threshold_mode mode;
    hi_u8 low_threshold;  /* user-defined threshold,  0<=low_threshold<=255 */
    hi_u8 high_threshold; /* user-defined threshold, if mode<HI_IVE_THRESHOLD_MODE_MIN_MID_MAX, high_thresh is not used,
                      else 0<=low_threshold<=high_threshold<=255; */
    hi_u8 min_val;  /* Minimum value when tri-level thresholding */
    hi_u8 mid_val;  /* Middle value when tri-level thresholding, if mode<2, mid_val is not used; */
    hi_u8 max_val;  /* Maxmum value when tri-level thresholding */
} hi_ive_threshold_ctrl;

/*
 * Type of the sub output results
 */
typedef enum {
    HI_IVE_SUB_MODE_ABS = 0x0,      /* Absolute value of the difference */
    HI_IVE_SUB_MODE_SHIFT = 0x1,    /* The output result is obtained by shifting the result one digit right
                                    to reserve the signed bit. */
    HI_IVE_SUB_MODE_BUTT
} hi_ive_sub_mode;

typedef struct {
    hi_ive_sub_mode mode;
} hi_ive_sub_ctrl;

/*
 * Type of the integ output results
 */
typedef enum {
    HI_IVE_INTEG_OUT_CTRL_COMBINE = 0x0,
    HI_IVE_INTEG_OUT_CTRL_SUM = 0x1,
    HI_IVE_INTEG_OUT_CTRL_SQRT_SUM = 0x2,
    HI_IVE_INTEG_OUT_CTRL_BUTT
} hi_ive_integ_out_ctrl;

typedef struct {
    hi_ive_integ_out_ctrl out_ctrl;
} hi_ive_integ_ctrl;

/*
 * Type of the thresh_s16 mode
 */
typedef enum {
    HI_IVE_THRESHOLD_S16_MODE_S16_TO_S8_MIN_MID_MAX = 0x0,
    HI_IVE_THRESHOLD_S16_MODE_S16_TO_S8_MIN_ORIG_MAX = 0x1,
    HI_IVE_THRESHOLD_S16_MODE_S16_TO_U8_MIN_MID_MAX = 0x2,
    HI_IVE_THRESHOLD_S16_MODE_S16_TO_U8_MIN_ORIG_MAX = 0x3,

    HI_IVE_THRESHOLD_S16_MODE_BUTT
} hi_ive_threshold_s16_mode;

typedef struct {
    hi_ive_threshold_s16_mode mode;
    hi_s16 low_threshold;           /* User-defined threshold */
    hi_s16 high_threshold;          /* User-defined threshold */
    hi_svp_8bit min_val;            /* Minimum value when tri-level thresholding */
    hi_svp_8bit mid_val;            /* Middle value when tri-level thresholding */
    hi_svp_8bit max_val;            /* Maxmum value when tri-level thresholding */
} hi_ive_threshold_s16_ctrl;

/*
 * Type of the thresh_u16 mode
 */
typedef enum {
    HI_IVE_THRESHOLD_U16_MODE_U16_TO_U8_MIN_MID_MAX = 0x0,
    HI_IVE_THRESHOLD_U16_MODE_U16_TO_U8_MIN_ORIG_MAX = 0x1,

    HI_IVE_THRESHOLD_U16_MODE_BUTT
} hi_ive_threshold_u16_mode;

typedef struct {
    hi_ive_threshold_u16_mode mode;
    hi_u16 low_threshold;
    hi_u16 high_threshold;
    hi_u8 min_val;
    hi_u8 mid_val;
    hi_u8 max_val;
} hi_ive_threshold_u16_ctrl;

/*
 * Mode of 16bit_to_8bit
 */
typedef enum {
    HI_IVE_16BIT_TO_8BIT_MODE_S16_TO_S8 = 0x0,
    HI_IVE_16BIT_TO_8BIT_MODE_S16_TO_U8_ABS = 0x1,
    HI_IVE_16BIT_TO_8BIT_MODE_S16_TO_U8_BIAS = 0x2,
    HI_IVE_16BIT_TO_8BIT_MODE_U16_TO_U8 = 0x3,

    HI_IVE_16BIT_TO_8BIT_MODE_BUTT
} hi_ive_16bit_to_8bit_mode;

typedef struct {
    hi_ive_16bit_to_8bit_mode mode;
    hi_u16 denominator;
    hi_u8 num;
    hi_s8 bias;
} hi_ive_16bit_to_8bit_ctrl;

/*
 * Type of the ord_stat_filter
 */
typedef enum {
    HI_IVE_ORDER_STATS_FILTER_MODE_MEDIAN = 0x0,
    HI_IVE_ORDER_STATS_FILTER_MODE_MAX = 0x1,
    HI_IVE_ORDER_STATS_FILTER_MODE_MIN = 0x2,

    HI_IVE_ORD_STAT_FILTER_MODE_BUTT
} hi_ive_order_stats_filter_mode;

typedef struct {
    hi_ive_order_stats_filter_mode mode;
} hi_ive_order_stats_filter_ctrl;

/*
 * Type of the map
 */
typedef enum {
    HI_IVE_MAP_MODE_U8 = 0x0,
    HI_IVE_MAP_MODE_S16 = 0x1,
    HI_IVE_MAP_MODE_U16 = 0x2,

    HI_IVE_MAP_MODE_BUTT
} hi_ive_map_mode;

typedef struct {
    hi_ive_map_mode mode;
} hi_ive_map_ctrl;

typedef struct {
    hi_u8 map[HI_IVE_MAP_NUM];
} hi_ive_map_u8bit_lut_mem;

typedef struct {
    hi_u16 map[HI_IVE_MAP_NUM];
} hi_ive_map_u16bit_lut_mem;

typedef struct {
    hi_s16 map[HI_IVE_MAP_NUM];
} hi_ive_map_s16bit_lut_mem;

typedef struct {
    hi_u32 hist[HI_IVE_HIST_NUM];
    hi_u8 map[HI_IVE_MAP_NUM];
} hi_ive_equalize_hist_ctrl_mem;

typedef struct {
    hi_svp_mem_info mem;
} hi_ive_equalize_hist_ctrl;

typedef struct {
    hi_u0q16 x; /* x of "xA+yB" */
    hi_u0q16 y; /* y of "xA+yB" */
} hi_ive_add_ctrl;

typedef struct {
    hi_u64 num;
    hi_u64 quad_sum1;
    hi_u64 quad_sum2;
    hi_u8 reserved[HI_IVE_ARR_RESERVED_NUM_EIGHT];
} hi_ive_ncc_dst_mem;

typedef struct {
    hi_u32 area;   /* Represented by the pixel number */
    hi_u16 left;   /* Circumscribed rectangle left border */
    hi_u16 right;  /* Circumscribed rectangle right border */
    hi_u16 top;    /* Circumscribed rectangle top border */
    hi_u16 bottom; /* Circumscribed rectangle bottom border */
} hi_ive_rgn;

typedef union {
    struct {
        hi_u32 cur_area_threshold: 23;  /* Threshold of the result regions' area */
        hi_u32 label_status: 1;         /* 1: Labeled failed ; 0: Labeled successfully */
        hi_u32 rgn_num: 8;              /* Number of valid region, non-continuous stored */
    } bits;
    hi_u32 u32;
} hi_ive_ccblob_info;

typedef struct {
    hi_ive_ccblob_info info;
    hi_ive_rgn rgn[HI_IVE_MAX_RGN_NUM];  /* Valid regions with 'cur_area_threshold>0' and 'label = array_index+1' */
} hi_ive_ccblob;

/*
 * Type of the ccl
 */
typedef enum {
    HI_IVE_CCL_MODE_4C = 0x0, /* 4-connected */
    HI_IVE_CCL_MODE_8C = 0x1, /* 8-connected */

    HI_IVE_CCL_MODE_BUTT
} hi_ive_ccl_mode;

typedef struct {
    hi_ive_ccl_mode mode;           /* Mode */
    hi_u16 init_area_threshold;     /* Init threshold of region area */
    hi_u16 step;                    /* Increase area step for once */
} hi_ive_ccl_ctrl;

/*
 * GMM control struct
 */
typedef struct {
    hi_u22q10 noise_var;        /* Initial noise Variance */
    hi_u22q10 max_var;          /* Max  Variance */
    hi_u22q10 min_var;          /* Min  Variance */
    hi_u0q16 learn_rate;        /* Learning rate */
    hi_u0q16 bg_ratio;          /* Background ratio */
    hi_u8q8 var_threshold;      /* Variance Threshold */
    hi_u0q16 init_wgt;          /* Initial Weight */
    hi_u8 model_num;            /* Model number: 3 or 5 */
} hi_ive_gmm_ctrl;

/*
 * Type of the GMM2 sensitivity factor mode
 */
typedef enum {
    HI_IVE_GMM2_SNS_FACTOR_MODE_GLOBAL = 0x0,   /* Global sensitivity factor mode */
    HI_IVE_GMM2_SNS_FACTOR_MODE_PIXEL = 0x1,    /* Pixel sensitivity factor mode */

    HI_IVE_GMM2_SNS_FACTOR_MODE_BUTT
} hi_ive_gmm2_sns_factor_mode;

/*
 * Type of the GMM2 life update factor mode
 */
typedef enum {
    HI_IVE_GMM2_LIFE_UPDATE_FACTOR_MODE_GLOBAL  = 0x0,      /* Global life update factor mode */
    HI_IVE_GMM2_LIFE_UPDATE_FACTOR_MODE_PIXEL     = 0x1,    /* Pixel life update factor mode */

    HI_IVE_GMM2_LIFE_UPDATE_FACTOR_MODE_BUTT
} hi_ive_gmm2_life_update_factor_mode;

typedef struct {
    hi_ive_gmm2_sns_factor_mode sns_factor_mode;                    /* Sensitivity factor mode */
    hi_ive_gmm2_life_update_factor_mode life_update_factor_mode;    /* Life update factor mode */
    hi_u16 global_life_update_factor;                               /* Global life update factor (default: 4) */
    hi_u16 life_threshold;                                          /* Life threshold (default: 5000) */
    hi_u16 freq_init_val;                                           /* Initial frequency (default: 20000) */
    hi_u16 freq_reduce_factor;                                      /* Frequency reduction factor (default: 0xFF00) */
    hi_u16 freq_add_factor;                                         /* Frequency adding factor (default: 0xEF) */
    hi_u16 freq_threshold;                                          /* Frequency threshold (default: 12000) */
    hi_u16 var_rate;                                                /* Variation update rate (default: 1) */
    hi_u9q7 max_var;                                                /* Max variation (default: (16 * 16)<<7) */
    hi_u9q7 min_var;                                                /* Min variation (default: ( 8 *  8)<<7) */
    hi_u8 global_sns_factor;                                        /* Global sensitivity factor (default: 8) */
    hi_u8 model_num;                                                /* Model number (range: 1~5, default: 3) */
} hi_ive_gmm2_ctrl;

typedef struct {
    hi_svp_mem_info mem;
    hi_u16 low_threshold;
    hi_u16 high_threshold;
    hi_s8 mask[HI_IVE_MASK_NUM];
} hi_ive_canny_hys_edge_ctrl;

typedef struct {
    hi_u32 stack_size;  /* Stack size for output */
    hi_u8 reserved[HI_IVE_ARR_RESERVED_NUM_TWELVE]; /* For 16 byte align */
} hi_ive_canny_stack_size;

/*
 * LBP compare mode
 */
typedef enum {
    HI_IVE_LBP_COMPARE_MODE_NORMAL = 0x0,
    HI_IVE_LBP_COMPARE_MODE_ABS = 0x1,

    HI_IVE_LBP_COMPARE_MODE_BUTT
} hi_ive_lbp_compare_mode;

typedef struct {
    hi_ive_lbp_compare_mode mode;
    hi_svp_8bit bit_threshold;
} hi_ive_lbp_ctrl;

/*
 * Type of the gradient_filter output format
 */
typedef enum {
    HI_IVE_NORM_GRAD_OUT_CTRL_HOR_AND_VER = 0x0,
    HI_IVE_NORM_GRAD_OUT_CTRL_HOR = 0x1,
    HI_IVE_NORM_GRAD_OUT_CTRL_VER = 0x2,
    HI_IVE_NORM_GRAD_OUT_CTRL_COMBINE = 0x3,

    HI_IVE_NORM_GRAD_OUT_CTRL_BUTT
} hi_ive_norm_grad_out_ctrl;

typedef struct {
    hi_ive_norm_grad_out_ctrl out_ctrl;
    hi_s8 mask[HI_IVE_MASK_NUM];
    hi_u8 norm;
} hi_ive_norm_grad_ctrl;

/*
 * lk_optical_flow_pyr output mode
 */
typedef enum {
    HI_IVE_LK_OPTICAL_FLOW_PYR_OUT_MODE_NONE = 0,   /* Output none */
    HI_IVE_LK_OPTICAL_FLOW_PYR_OUT_MODE_STATUS = 1, /* Output status */
    HI_IVE_LK_OPTICAL_FLOW_PYR_OUT_MODE_BOTH = 2,   /* Output status and err */

    HI_IVE_LK_OPTICAL_FLOW_PYR_OUT_MODE_BUTT
} hi_ive_lk_optical_flow_pyr_out_mode;

typedef struct {
    hi_ive_lk_optical_flow_pyr_out_mode out_mode;
    hi_bool use_init_flow;          /* where to use initial flow */
    hi_u16 points_num;              /* Number of the feature points,<=500 */
    hi_u8 max_level;                /* max_level should be in [0, 3] */
    hi_u0q8 min_eig_val_threshold;  /* Minimum eigenvalue threshold */
    hi_u8 iter_cnt;                 /* Maximum iteration times, <=20 */
    hi_u0q8 eps;                    /* Used for exit criteria: dx^2 + dy^2 < eps */
} hi_ive_lk_optical_flow_pyr_ctrl;

typedef struct {
    hi_u16 max_eig_val;     /* Shi-Tomasi second step output MaxEig */
    hi_u8 reserved[HI_IVE_ARR_RESERVED_NUM_FOURTEEN]; /* For 16 byte align */
} hi_ive_st_max_eig_val;

typedef struct {
    hi_svp_mem_info mem;
    hi_u0q8 quality_level;
} hi_ive_st_cand_corner_ctrl;

typedef struct {
    hi_u16 corner_num;
    hi_svp_point_u16 corner[HI_IVE_ST_MAX_CORNER_NUM];
} hi_ive_st_corner_info;

typedef struct {
    hi_u16 max_corner_num;
    hi_u16 min_dist;
} hi_ive_st_corner_ctrl;

/*
 * grad_fg mode enum
 */
typedef enum {
    HI_IVE_GRAD_FG_MODE_USE_CUR_GRAD = 0x0,
    HI_IVE_GRAD_FG_MODE_FIND_MIN_GRAD = 0x1,

    HI_IVE_GRAD_FG_MODE_BUTT
} hi_ive_grad_fg_mode;

typedef struct {
    hi_ive_grad_fg_mode mode;  /* Calculation mode */
    hi_u16 edw_factor;         /* Edge width adjustment factor (range: 500 to 2000; default: 1000) */
    hi_u8 crl_coef_threshold;  /* Gradient vector correlation coefficient threshold
                                (ranges: 50 to 100; default: 80) */
    hi_u8 mag_crl_threshold;   /* Gradient amplitude threshold (range: 0 to 20; default: 4) */
    hi_u8 min_mag_diff;        /* Gradient magnitude difference threshold (range: 2 to 8; default: 2) */
    hi_u8 noise_val;           /* Gradient amplitude noise threshold (range: 1 to 8; default: 1) */
    hi_u8 edw_dark;            /* Black pixels enable flag (range: 0 (no), 1 (yes); default: 1) */
} hi_ive_grad_fg_ctrl;

typedef struct {
    hi_u8q4f4 mean;         /* Candidate background grays value */
    hi_u16 start_time;      /* Candidate Background start time */
    hi_u16 sum_access_time; /* Candidate Background cumulative access time */
    hi_u16 short_keep_time; /* Candidate background short hold time */
    hi_u8 chg_condition;    /* Time condition for candidate background into the changing state */
    hi_u8 poten_bg_life;    /* Potential background cumulative access time */
} hi_ive_cand_bg_pixel;

typedef struct {
    hi_u8q4f4 mean;                         /* 0# background grays value */
    hi_u16 access_time;                     /* Background cumulative access time */
    hi_u8 prev_gray;                        /* Gray value of last pixel */
    hi_u5q3 diff_threshold;                 /* Differential threshold */
    hi_u8 access_flag;                      /* Background access flag */
    hi_u8 bg_gray[HI_IVE_ARR_NUM_THREE];    /* 1# ~ 3# background grays value */
} hi_ive_wrok_bg_pixel;

typedef struct {
    hi_u8 work_bg_life[HI_IVE_ARR_NUM_THREE];   /* 1# ~ 3# background vitality */
    hi_u8 candi_bg_life;                        /* Candidate background vitality */
} hi_ive_bg_life;

typedef struct {
    hi_ive_wrok_bg_pixel work_bg_pixel;     /* Working background */
    hi_ive_cand_bg_pixel cand_pixel;        /* Candidate background */
    hi_ive_bg_life bg_life;                 /* Background vitality */
} hi_ive_bg_model_pixel;

typedef struct {
    hi_u32 pixel_num;
    hi_u32 sum_luma;
    hi_u8 reserved[HI_IVE_ARR_RESERVED_NUM_EIGHT];
} hi_ive_fg_status_data;

typedef struct {
    hi_u32 pixel_num;
    hi_u32 sum_luma;
    hi_u8 reserved[HI_IVE_ARR_RESERVED_NUM_EIGHT];
} hi_ive_bg_status_data;

typedef struct {
    hi_u32 cur_frame_num;         /* Current frame timestamp, in frame units */
    hi_u32 prev_frame_num;         /* Previous frame timestamp, in frame units */
    hi_u16 time_threshold;            /* Potential background replacement time threshold
                                (range: 2 to 100 frames; default: 20) */

    hi_u8 diff_threshold_crl_coef;    /* Correlation coefficients between differential threshold and gray value
                                (range: 0 to 5; default: 0) */
    hi_u8 diff_max_threshold;         /* Maximum of background differential threshold (range: 3 to 15; default: 6) */
    hi_u8 diff_min_threshold;         /* Minimum of background differential threshold (range: 3 to 15; default: 4) */
    hi_u8 diff_threshold_inc;         /* Dynamic Background differential threshold increment
                                (range: 0 to 6; default: 0) */
    hi_u8 fast_learn_rate;      /* Quick background learning rate (range: 0 to 4; default: 2) */
    hi_u8 detected_chg_rgn;       /* Whether to detect change region (range: 0 (no), 1 (yes); default: 0) */
} hi_ive_match_bg_model_ctrl;

typedef struct {
    hi_u32 cur_frame_num;         /* Current frame timestamp, in frame units */
    hi_u32 prev_check_time;        /* The last time when background status is checked */
    hi_u32 frame_check_period;      /* Background status checking period (range: 0 to 2000 frames; default: 50) */

    hi_u32 init_min_time;       /* Background initialization shortest time
                                (range: 20 to 6000 frames; default: 100) */
    hi_u32 steady_bg_min_blend_time; /* Steady background integration shortest time
                                (range: 20 to 6000 frames; default: 200) */
    hi_u32 steady_bg_max_blend_time; /* Steady background integration longest time
                                (range: 20 to 40000 frames; default: 1500) */
    hi_u32 dynamic_bg_min_blend_time; /* Dynamic background integration shortest time
                                (range: 0 to 6000 frames; default: 0) */
    hi_u32 static_detection_min_time;  /* Still detection shortest time (range: 20 to 6000 frames; default: 80) */
    hi_u16 fg_max_fade_time;     /* Foreground disappearing longest time
                                (range: 1 to 255 seconds; default: 15) */
    hi_u16 bg_max_fade_time;     /* Background disappearing longest time
                                (range: 1 to 255  seconds ; default: 60) */

    hi_u8 steay_bg_access_time_rate_threshold; /* Steady background access time ratio threshold
                                (range: 10 to 100; default: 80) */
    hi_u8 chg_bg_access_time_rate_threshold; /* Change background access time ratio threshold
                                (range: 10 to 100; default: 60) */
    hi_u8 dynamic_bg_access_time_threshold;     /* Dynamic background access time ratio threshold
                                (range: 0 to 50; default: 0) */
    hi_u8 dynamic_bg_depth;          /* Dynamic background depth (range: 0 to 3; default: 3) */
    hi_u8 bg_eff_state_rate_threshold;     /* Background state time ratio threshold when initializing
                                (range: 90 to 100; default: 90) */

    hi_u8 acc_bg_learn;  /* Whether to accelerate background learning (range: 0 (no), 1 (yes); default: 0) */
    hi_u8 detected_chg_rgn; /* Whether to detect change region (range: 0 (no), 1 (yes); default: 0) */
} hi_ive_update_bg_model_ctrl;

typedef enum {
    HI_IVE_ANN_MLP_ACTV_FUNC_IDENTITY = 0x0,
    HI_IVE_ANN_MLP_ACTV_FUNC_SIGMOID_SYM = 0x1,
    HI_IVE_ANN_MLP_ACTV_FUNC_GAUSSIAN = 0x2,

    HI_IVE_ANN_MLP_ACTV_FUNC_BUTT
} hi_ive_ann_mlp_actv_func;

typedef enum {
    HI_IVE_ANN_MLP_ACCURATE_SRC16_WGT16 = 0x0, /* input decimals' accurate 16 bit, wgt 16bit */
    HI_IVE_ANN_MLP_ACCURATE_SRC14_WGT20 = 0x1, /* input decimals' accurate 14 bit, wgt 20bit */

    HI_IVE_ANN_MLP_ACCURATE_BUTT
} hi_ive_ann_mlp_accurate;

typedef struct {
    hi_ive_ann_mlp_actv_func actv_func;
    hi_ive_ann_mlp_accurate accurate;
    hi_svp_mem_info wgt;
    hi_u32 total_wgt_size;

    hi_u16 layer_cnt[HI_IVE_ARR_NUM_EIGHT];      /* 8 layers, including input and output layer */
    hi_u16 max_count;           /* MaxCount should be less than and equal 1024 */
    hi_u8 layer_num;            /* layerNum should in (2,8] */
    hi_u8 reserved;
} hi_ive_ann_mlp_model;

typedef enum {
    HI_IVE_SVM_TYPE_C_SVC = 0x0,
    HI_IVE_SVM_TYPE_NU_SVC = 0x1,

    HI_IVE_SVM_TYPE_BUTT
} hi_ive_svm_type;

typedef enum {
    HI_IVE_SVM_KERNEL_TYPE_LINEAR = 0x0,
    HI_IVE_SVM_KERNEL_TYPE_POLY = 0x1,
    HI_IVE_SVM_KERNEL_TYPE_RBF = 0x2,
    HI_IVE_SVM_KERNEL_TYPE_SIGMOID = 0x3,

    HI_IVE_SVM_KERNEL_TYPE_BUTT
} hi_ive_svm_kernel_type;

typedef struct {
    hi_ive_svm_type type;
    hi_ive_svm_kernel_type kernel_type;

    hi_svp_mem_info sv;                 /* SV memory */
    hi_svp_mem_info decision_func;      /* Decision functions memory */
    hi_u32 total_decision_func_size;    /* All decision functions coef size in byte */

    hi_u16 feature_dim;
    hi_u16 sv_total;
    hi_u8 class_cnt;
} hi_ive_svm_model;

/*
 * Sad mode
 */
typedef enum {
    HI_IVE_SAD_MODE_MB_4X4 = 0x0,   /* 4x4 */
    HI_IVE_SAD_MODE_MB_8X8 = 0x1,   /* 8x8 */
    HI_IVE_SAD_MODE_MB_16X16 = 0x2, /* 16x16 */

    HI_IVE_SAD_MODE_BUTT
} hi_ive_sad_mode;
/*
 * Sad output ctrl
 */
typedef enum {
    HI_IVE_SAD_OUT_CTRL_16BIT_BOTH = 0x0, /* Output 16 bit sad and threshold */
    HI_IVE_SAD_OUT_CTRL_8BIT_BOTH = 0x1,  /* Output 8 bit sad and threshold */
    HI_IVE_SAD_OUT_CTRL_16BIT_SAD = 0x2,  /* Output 16 bit sad */
    HI_IVE_SAD_OUT_CTRL_8BIT_SAD = 0x3,   /* Output 8 bit sad */
    HI_IVE_SAD_OUT_CTRL_THRESHOLD = 0x4,  /* Output threshold,16 bits sad */

    HI_IVE_SAD_OUT_CTRL_BUTT
} hi_ive_sad_out_ctrl;

typedef struct {
    hi_ive_sad_mode mode;
    hi_ive_sad_out_ctrl out_ctrl;
    hi_u16 threshold;
    hi_u8 min_val;              /* Min value */
    hi_u8 max_val;              /* Max value */
} hi_ive_sad_ctrl;

/*
 * Resize zoom mode
 */
typedef enum {
    HI_IVE_RESIZE_MODE_LINEAR = 0x0, /* Bilinear interpolation */
    HI_IVE_RESIZE_MODE_AREA = 0x1,

    HI_IVE_RESIZE_MODE_BUTT
} hi_ive_resize_mode;

typedef struct {
    hi_ive_resize_mode mode;
    hi_svp_mem_info mem;
    hi_u16 num;
} hi_ive_resize_ctrl;

/*
 * CNN active function mode
 */
typedef enum {
    HI_IVE_CNN_ACTV_FUNC_NONE = 0x0,    /* 1. Do not taking a activation, equivalent f(x)=x */
    HI_IVE_CNN_ACTV_FUNC_RELU = 0x1,    /* 2. f(x)=max(0, x) */
    HI_IVE_CNN_ACTV_FUNC_SIGMOID = 0x2, /* 3. f(x)=1/(1+exp(-x)), not support */

    HI_IVE_CNN_ACTV_FUNC_BUTT
} hi_ive_cnn_actv_func;

/*
 * CNN pooling mode
 */
typedef enum {
    HI_IVE_CNN_POOLING_NONE = 0x0, /* Do not taking a pooling action */
    HI_IVE_CNN_POOLING_MAX = 0x1,  /* Using max value of every pooling area */
    HI_IVE_CNN_POOLING_AVG = 0x2,  /* Using average value of every pooling area */

    HI_IVE_CNN_POOLING_BUTT
} hi_ive_cnn_pooling;

typedef struct {
    hi_ive_cnn_actv_func actv_func;   /* Type of activation function */
    hi_ive_cnn_pooling pooling;       /* Mode of pooling method */

    hi_u8 feature_map_num;            /* Number of feature maps */
    hi_u8 kernel_size;                /* Kernel size, only support 3 currently */
    hi_u8 conv_step;                  /* Convolution step, only support 1 currently */

    hi_u8 pool_size;                  /* Pooling size, only support 2 currently */
    hi_u8 pool_step;                  /* Pooling step, only support 2 currently */
    hi_u8 reserved[HI_IVE_ARR_RESERVED_NUM_THREE];
} hi_ive_cnn_conv_pooling;

typedef struct {
    hi_u16 layer_cnt[HI_IVE_ARR_NUM_EIGHT];    /* Neuron number of every fully connected layers */
    hi_u16 max_cnt;         /* Max neuron number in all fully connected layers */
    hi_u8 layer_num;        /* Number of fully connected layer */
    hi_u8 reserved;
} hi_ive_cnn_fc_info;

typedef struct {
    hi_ive_cnn_conv_pooling conv_pool[HI_IVE_ARR_NUM_EIGHT];  /* Conv-ReLU-Pooling layers info */
    hi_ive_cnn_fc_info fc_info;  /* Fully connected layers info */

    hi_svp_mem_info conv_kernel_bias;      /* Conv-ReLU-Pooling layers' kernels and bias */
    hi_u32 conv_kernel_bias_size;          /* Size of Conv-ReLU-Pooling layer' kernels and bias */

    hi_svp_mem_info fc_wgt_bias;          /* Fully Connection Layers' weights and bias */
    hi_u32 fc_wgt_bias_size;              /* Size of fully connection layers weights and bias */

    hi_u32 total_mem_size;                 /* Total memory size of all kernels, weights, bias */

    hi_svp_img_type type;                /* Image type used for the CNN model */
    hi_u32 width;                          /* Image width used for the model */
    hi_u32 height;                         /* Image height used for the model */

    hi_u16 class_cnt;                    /* Number of classes */
    hi_u8 conv_pool_layer_num;             /* Number of Conv-ReLU-Pooling layers */
    hi_u8 reserved;
} hi_ive_cnn_model;

typedef struct {
    hi_svp_mem_info mem;    /* Assist memory */
    hi_u32 num;             /* Input image number */
} hi_ive_cnn_ctrl;

typedef struct {
    hi_s32 class_idx;       /* The most possible index of the classification */
    hi_s32 conf;            /* The conf of the classification */
} hi_ive_cnn_result;

/*
 * Perspective transform algorithm mode
 */
typedef enum {
    HI_IVE_PERSP_TRANS_ALG_MODE_NR_SIM = 0x0, /* Non-reflective similarity transform mode */
    HI_IVE_PERSP_TRANS_ALG_MODE_SIM = 0x1,    /* Reflective similarity transform mode */
    HI_IVE_PERSP_TRANS_ALG_MODE_AFFINE = 0x2, /* Affine transform mode */

    HI_IVE_PERSP_TRANS_ALG_MODE_BUTT
} hi_ive_persp_trans_alg_mode;

typedef struct {
    hi_svp_point_u14q2 src_point; /* Source point */
    hi_svp_point_u14q2 dst_point; /* Destination point */
} hi_ive_persp_trans_point_pair;

/*
 * Perspective transform csc mode
 */
typedef enum {
    HI_IVE_PERSP_TRANS_CSC_MODE_NONE = 0x0,                /* No do csc */
    HI_IVE_PERSP_TRANS_CSC_MODE_VIDEO_BT601_YUV_TO_RGB = 0x1, /* CSC: YUV2RGB, video transfer mode,
                                                      RGB value range [16, 235] */
    HI_IVE_PERSP_TRANS_CSC_MODE_VIDEO_BT709_YUV_TO_RGB = 0x2, /* CSC: YUV2RGB, video transfer mode,
                                                      RGB value range [16, 235] */
    HI_IVE_PERSP_TRANS_CSC_MODE_PIC_BT601_YUV_TO_RGB = 0x3,   /* CSC: YUV2RGB, picture transfer mode,
                                                      RGB value range [0, 255] */
    HI_IVE_PERSP_TRANS_CSC_MODE_PIC_BT709_YUV_TO_RGB = 0x4,   /* CSC: YUV2RGB, picture transfer mode,
                                                      RGB value range [0, 255] */

    HI_IVE_PERSP_TRANS_CSC_MODE_BUTT
} hi_ive_persp_trans_csc_mode;

typedef struct {
    hi_ive_persp_trans_alg_mode alg_mode;   /* Alg mode */
    hi_ive_persp_trans_csc_mode csc_mode;   /* CSC mode */
    hi_u16 roi_num;                         /* Roi number */
    hi_u16 point_pair_num;                  /* Point pair number  */
} hi_ive_persp_trans_ctrl;

typedef enum {
    HI_IVE_KCF_CORE0 = 0x0,

    HI_IVE_KCF_CORE_BUTT
} hi_ive_kcf_core_id;

typedef struct {
    hi_svp_rect_s24q8 roi;
    hi_u32 roi_id;
} hi_ive_roi_info;

/* Kcf ctrl param */
typedef struct {
    hi_ive_csc_mode csc_mode;   /* Only support:
                                HI_IVE_CSC_MODE_VIDEO_BT601_YUV_TO_RGB  CSC: YUV2RGB, video transfer mode,
                                RGB value range [16, 235]
                                HI_IVE_CSC_MODE_VIDEO_BT709_YUV_TO_RGB  CSC: YUV2RGB, video transfer mode,
                                RGB value range [16, 235]
                                HI_IVE_CSC_MODE_PIC_BT601_YUV_TO_RGB  CSC: YUV2RGB, picture transfer mode,
                                RGB value range [0, 255]
                                HI_IVE_CSC_MODE_PIC_BT709_YUV_TO_RGB  CSC: YUV2RGB, picture transfer mode,
                                RGB value range [0, 255]
                                */
    hi_svp_mem_info tmp_buf;
    hi_u1q15 interp_factor; /* Blend coefficient. [0, 32768] */
    hi_u0q16 lamda;        /* The regularization coefficient. [0, 65535] */
    hi_u4q12 norm_trunc_alfa;   /* The normalization truncation threshold. [0, 4095] */
    hi_u0q8 sigma;         /* The gaussian kernel bandwidth. [0, 255]  */
    hi_u8 response_threshold;
    hi_ive_kcf_core_id core_id;
} hi_ive_kcf_proc_ctrl;

typedef struct {
    hi_ive_roi_info roi_info;
    hi_svp_mem_info cos_win_x;
    hi_svp_mem_info cos_win_y;
    hi_svp_mem_info gauss_peak;
    hi_svp_mem_info hog_feature;
    hi_svp_mem_info alpha;
    hi_svp_mem_info dst;
    hi_u3q5 padding; /* [48, 160]  */
    hi_u8 reserved[HI_IVE_ARR_RESERVED_NUM_THREE];
} hi_ive_kcf_obj;

typedef struct tag_hi_ive_list_head {
    struct tag_hi_ive_list_head *next, *prev;
} hi_ive_list_head;

typedef struct {
    hi_ive_list_head list;
    hi_ive_kcf_obj kcf_obj;
} hi_ive_kcf_obj_node;

typedef enum {
    HI_IVE_KCF_LIST_STATE_CREATE = 0x1,
    HI_IVE_KCF_LIST_STATE_DESTORY = 0x2,
    HI_IVE_KCF_LIST_STATE_BUTT
} hi_ive_kcf_list_state;

typedef struct {
    hi_ive_kcf_obj_node *obj_node_buf;  /* The object list node address */
    hi_ive_list_head free_obj_list;     /* The free list of object list */
    hi_ive_list_head train_obj_list;    /* The training list of object list */
    hi_ive_list_head track_obj_list;    /* The tracking list of object list */

    hi_u32 free_obj_num;                /* The numbers of free list */
    hi_u32 train_obj_num;               /* The numbers of training list */
    hi_u32 track_obj_num;               /* The numbers of tracking list */
    hi_u32 max_obj_num;                 /* The maximum numbers of object list */
    hi_ive_kcf_list_state list_state;   /* The object list state */
    hi_u8 *tmp_buf;                     /* Assist buffer */
    hi_u32 width;                       /* image width */
    hi_u32 height;                      /* image height */
} hi_ive_kcf_obj_list;

typedef struct {
    hi_ive_kcf_obj_node *node;
    hi_s32 response; /* Bbox Response value. */

    hi_ive_roi_info roi_info;
    hi_bool is_track_ok;
    hi_bool is_roi_refresh;
} hi_ive_kcf_bbox;

typedef struct {
    hi_u32 max_bbox_num;        /* The member numbers of Bbox Array. */
    hi_s32 response_threshold;  /* Select Bbox when Bbox'Response value is greater than or equal to RespThr. */
} hi_ive_kcf_bbox_ctrl;

typedef enum {
    HI_IVE_HOG_MODE_VER_TANGENT_PLANE = 0x1,
    HI_IVE_HOG_MODE_HOR_TANGENT_PLANE = 0x2,
    HI_IVE_HOG_MODE_BUTT
} hi_ive_hog_mode;

typedef struct {
    hi_ive_csc_mode csc_mode;   /* Only support:
                                 * HI_IVE_CSC_MODE_VIDEO_BT601_YUV_TO_RGB  CSC: YUV2RGB, video transfer mode,
                                 * RGB value range [16, 235]
                                 * HI_IVE_CSC_MODE_VIDEO_BT709_YUV_TO_RGB  CSC: YUV2RGB, video transfer mode,
                                 * RGB value range [16, 235]
                                 * HI_IVE_CSC_MODE_PIC_BT601_YUV_TO_RGB  CSC: YUV2RGB, picture transfer mode,
                                 * RGB value range [0, 255]
                                 * HI_IVE_CSC_MODE_PIC_BT709_YUV_TO_RGB  CSC: YUV2RGB, picture transfer mode,
                                 * RGB value range [0, 255] */
    hi_ive_hog_mode hog_mode;   /* Hog mode */
    hi_u32 roi_num;             /* Roi number. [1, 64] */
    hi_u4q12 norm_trunc_alfa;   /* The normalization truncation threshold. [0, 4095] */

    hi_ive_kcf_core_id core_id;
} hi_ive_hog_ctrl;
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
#endif /* _HI_COMMON_IVE_H_ */
