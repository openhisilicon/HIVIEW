/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */

#ifndef HI_COMMON_IVE_H
#define HI_COMMON_IVE_H

#include "hi_common_svp.h"
#include "hi_common.h"
#include "ot_common_ive.h"

#ifdef __cplusplus
extern "C" {
#endif

#define HI_IVE_HIST_NUM OT_IVE_HIST_NUM
#define HI_IVE_MAP_NUM OT_IVE_MAP_NUM
#define HI_IVE_MAX_RGN_NUM OT_IVE_MAX_RGN_NUM
#define HI_IVE_ST_MAX_CORNER_NUM OT_IVE_ST_MAX_CORNER_NUM
#define HI_IVE_MASK_NUM OT_IVE_MASK_NUM
#define HI_IVE_MASK_NUM_121 OT_IVE_MASK_NUM_121
#define HI_IVE_PADDING_ARR_NUM OT_IVE_PADDING_ARR_NUM
#define HI_IVE_ARR_RESERVED_NUM_TWO OT_IVE_ARR_RESERVED_NUM_TWO
#define HI_IVE_ARR_RESERVED_NUM_THREE OT_IVE_ARR_RESERVED_NUM_THREE
#define HI_IVE_ARR_RESERVED_NUM_EIGHT OT_IVE_ARR_RESERVED_NUM_EIGHT
#define HI_IVE_ARR_RESERVED_NUM_TWELVE OT_IVE_ARR_RESERVED_NUM_TWELVE
#define HI_IVE_ARR_RESERVED_NUM_FOURTEEN OT_IVE_ARR_RESERVED_NUM_FOURTEEN
#define HI_IVE_ARR_NUM_THREE OT_IVE_ARR_NUM_THREE
#define HI_IVE_ARR_NUM_EIGHT OT_IVE_ARR_NUM_EIGHT
#define HI_IVE_ERR_SYS_TIMEOUT OT_IVE_ERR_SYS_TIMEOUT
#define HI_IVE_ERR_QUERY_TIMEOUT OT_IVE_ERR_QUERY_TIMEOUT
#define HI_IVE_ERR_BUS_ERR OT_IVE_ERR_BUS_ERR
#define HI_IVE_ERR_OPEN_FILE OT_IVE_ERR_OPEN_FILE
#define HI_IVE_ERR_READ_FILE OT_IVE_ERR_READ_FILE
#define HI_IVE_ERR_BUTT OT_IVE_ERR_BUTT
typedef ot_ive_err_code hi_ive_err_code;
#define HI_ERR_IVE_INVALID_DEV_ID OT_ERR_IVE_INVALID_DEV_ID
#define HI_ERR_IVE_INVALID_CHN_ID OT_ERR_IVE_INVALID_CHN_ID
#define HI_ERR_IVE_ILLEGAL_PARAM OT_ERR_IVE_ILLEGAL_PARAM
#define HI_ERR_IVE_EXIST OT_ERR_IVE_EXIST
#define HI_ERR_IVE_UNEXIST OT_ERR_IVE_UNEXIST
#define HI_ERR_IVE_NULL_PTR OT_ERR_IVE_NULL_PTR
#define HI_ERR_IVE_NOT_CFG OT_ERR_IVE_NOT_CFG
#define HI_ERR_IVE_NOT_SURPPORT OT_ERR_IVE_NOT_SURPPORT
#define HI_ERR_IVE_NOT_PERM OT_ERR_IVE_NOT_PERM
#define HI_ERR_IVE_NO_MEM OT_ERR_IVE_NO_MEM
#define HI_ERR_IVE_NO_BUF OT_ERR_IVE_NO_BUF
#define HI_ERR_IVE_BUF_EMPTY OT_ERR_IVE_BUF_EMPTY
#define HI_ERR_IVE_BUF_FULL OT_ERR_IVE_BUF_FULL
#define HI_ERR_IVE_NOT_READY OT_ERR_IVE_NOT_READY
#define HI_ERR_IVE_BAD_ADDR OT_ERR_IVE_BAD_ADDR
#define HI_ERR_IVE_BUSY OT_ERR_IVE_BUSY
#define HI_ERR_IVE_SYS_TIMEOUT OT_ERR_IVE_SYS_TIMEOUT
#define HI_ERR_IVE_QUERY_TIMEOUT OT_ERR_IVE_QUERY_TIMEOUT
#define HI_ERR_IVE_BUS_ERR OT_ERR_IVE_BUS_ERR
#define HI_ERR_IVE_OPEN_FILE OT_ERR_IVE_OPEN_FILE
#define HI_ERR_IVE_READ_FILE OT_ERR_IVE_READ_FILE
#define HI_IVE_DMA_MODE_DIRECT_COPY OT_IVE_DMA_MODE_DIRECT_COPY
#define HI_IVE_DMA_MODE_INTERVAL_COPY OT_IVE_DMA_MODE_INTERVAL_COPY
#define HI_IVE_DMA_MODE_SET_3BYTE OT_IVE_DMA_MODE_SET_3BYTE
#define HI_IVE_DMA_MODE_SET_8BYTE OT_IVE_DMA_MODE_SET_8BYTE
#define HI_IVE_DMA_MODE_MASK OT_IVE_DMA_MODE_MASK
#define HI_IVE_DMA_MODE_BUTT OT_IVE_DMA_MODE_BUTT
#define HI_IVE_DMA_MASK_MODE_32BIT OT_IVE_DMA_MASK_MODE_32BIT
#define HI_IVE_DMA_MASK_MODE_24BIT OT_IVE_DMA_MASK_MODE_24BIT
#define HI_IVE_DMA_MASK_MODE_BUTT OT_IVE_DMA_MASK_MODE_BUTT
typedef ot_ive_dma_mode hi_ive_dma_mode;
typedef ot_ive_dma_mask_mode hi_ive_dma_mask_mode;
typedef ot_ive_dma_ctrl hi_ive_dma_ctrl;
#define HI_IVE_PADDING_MODE_EDGE OT_IVE_PADDING_MODE_EDGE
#define HI_IVE_PADDING_MODE_CONSTANT OT_IVE_PADDING_MODE_CONSTANT
#define HI_IVE_PADDING_MODE_BUTT OT_IVE_PADDING_MODE_BUTT
typedef ot_ive_padding_mode hi_ive_padding_mode;
typedef ot_ive_filter_ctrl hi_ive_filter_ctrl;
#define HI_IVE_CSC_MODE_VIDEO_BT601_YUV_TO_RGB OT_IVE_CSC_MODE_VIDEO_BT601_YUV_TO_RGB
#define HI_IVE_CSC_MODE_VIDEO_BT709_YUV_TO_RGB OT_IVE_CSC_MODE_VIDEO_BT709_YUV_TO_RGB
#define HI_IVE_CSC_MODE_PIC_BT601_YUV_TO_RGB OT_IVE_CSC_MODE_PIC_BT601_YUV_TO_RGB
#define HI_IVE_CSC_MODE_PIC_BT709_YUV_TO_RGB OT_IVE_CSC_MODE_PIC_BT709_YUV_TO_RGB
#define HI_IVE_CSC_MODE_PIC_BT601_YUV_TO_HSV OT_IVE_CSC_MODE_PIC_BT601_YUV_TO_HSV
#define HI_IVE_CSC_MODE_PIC_BT709_YUV_TO_HSV OT_IVE_CSC_MODE_PIC_BT709_YUV_TO_HSV
#define HI_IVE_CSC_MODE_PIC_BT601_YUV_TO_LAB OT_IVE_CSC_MODE_PIC_BT601_YUV_TO_LAB
#define HI_IVE_CSC_MODE_PIC_BT709_YUV_TO_LAB OT_IVE_CSC_MODE_PIC_BT709_YUV_TO_LAB
#define HI_IVE_CSC_MODE_VIDEO_BT601_RGB_TO_YUV OT_IVE_CSC_MODE_VIDEO_BT601_RGB_TO_YUV
#define HI_IVE_CSC_MODE_VIDEO_BT709_RGB_TO_YUV OT_IVE_CSC_MODE_VIDEO_BT709_RGB_TO_YUV
#define HI_IVE_CSC_MODE_PIC_BT601_RGB_TO_YUV OT_IVE_CSC_MODE_PIC_BT601_RGB_TO_YUV
#define HI_IVE_CSC_MODE_PIC_BT709_RGB_TO_YUV OT_IVE_CSC_MODE_PIC_BT709_RGB_TO_YUV
#define HI_IVE_CSC_MODE_BUTT OT_IVE_CSC_MODE_BUTT
typedef ot_ive_csc_mode hi_ive_csc_mode;
typedef ot_ive_csc_ctrl hi_ive_csc_ctrl;
typedef ot_ive_filter_and_csc_ctrl hi_ive_filter_and_csc_ctrl;
#define HI_IVE_SOBEL_OUT_CTRL_BOTH OT_IVE_SOBEL_OUT_CTRL_BOTH
#define HI_IVE_SOBEL_OUT_CTRL_HOR OT_IVE_SOBEL_OUT_CTRL_HOR
#define HI_IVE_SOBEL_OUT_CTRL_VER OT_IVE_SOBEL_OUT_CTRL_VER
#define HI_IVE_SOBEL_OUT_CTRL_BUTT OT_IVE_SOBEL_OUT_CTRL_BUTT
typedef ot_ive_sobel_out_ctrl hi_ive_sobel_out_ctrl;
typedef ot_ive_sobel_ctrl hi_ive_sobel_ctrl;
#define HI_IVE_MAG_AND_ANG_OUT_CTRL_MAG OT_IVE_MAG_AND_ANG_OUT_CTRL_MAG
#define HI_IVE_MAG_AND_ANG_OUT_CTRL_MAG_AND_ANG OT_IVE_MAG_AND_ANG_OUT_CTRL_MAG_AND_ANG
#define HI_IVE_MAG_AND_ANG_OUT_CTRL_BUTT OT_IVE_MAG_AND_ANG_OUT_CTRL_BUTT
typedef ot_ive_mag_and_ang_out_ctrl hi_ive_mag_and_ang_out_ctrl;
typedef ot_ive_mag_and_ang_ctrl hi_ive_mag_and_ang_ctrl;
typedef ot_ive_dilate_ctrl hi_ive_dilate_ctrl;
typedef ot_ive_erode_ctrl hi_ive_erode_ctrl;
#define HI_IVE_THRESHOLD_MODE_BINARY OT_IVE_THRESHOLD_MODE_BINARY
#define HI_IVE_THRESHOLD_MODE_TRUNC OT_IVE_THRESHOLD_MODE_TRUNC
#define HI_IVE_THRESHOLD_MODE_TO_MIN_VAL OT_IVE_THRESHOLD_MODE_TO_MIN_VAL
#define HI_IVE_THRESHOLD_MODE_MIN_MID_MAX OT_IVE_THRESHOLD_MODE_MIN_MID_MAX
#define HI_IVE_THRESHOLD_MODE_ORIG_MID_MAX OT_IVE_THRESHOLD_MODE_ORIG_MID_MAX
#define HI_IVE_THRESHOLD_MODE_MIN_MID_ORI OT_IVE_THRESHOLD_MODE_MIN_MID_ORI
#define HI_IVE_THRESHOLD_MODE_MIN_ORIG_MAX OT_IVE_THRESHOLD_MODE_MIN_ORIG_MAX
#define HI_IVE_THRESHOLD_MODE_ORI_MID_ORIG OT_IVE_THRESHOLD_MODE_ORI_MID_ORIG
#define HI_IVE_THRESHOLD_MODE_BUTT OT_IVE_THRESHOLD_MODE_BUTT
typedef ot_ive_threshold_mode hi_ive_threshold_mode;
typedef ot_ive_threshold_ctrl hi_ive_threshold_ctrl;
#define HI_IVE_SUB_MODE_ABS OT_IVE_SUB_MODE_ABS
#define HI_IVE_SUB_MODE_SHIFT OT_IVE_SUB_MODE_SHIFT
#define HI_IVE_SUB_MODE_BUTT OT_IVE_SUB_MODE_BUTT
typedef ot_ive_sub_mode hi_ive_sub_mode;
typedef ot_ive_sub_ctrl hi_ive_sub_ctrl;
#define HI_IVE_INTEG_OUT_CTRL_COMBINE OT_IVE_INTEG_OUT_CTRL_COMBINE
#define HI_IVE_INTEG_OUT_CTRL_SUM OT_IVE_INTEG_OUT_CTRL_SUM
#define HI_IVE_INTEG_OUT_CTRL_SQRT_SUM OT_IVE_INTEG_OUT_CTRL_SQRT_SUM
#define HI_IVE_INTEG_OUT_CTRL_BUTT OT_IVE_INTEG_OUT_CTRL_BUTT
typedef ot_ive_integ_out_ctrl hi_ive_integ_out_ctrl;
typedef ot_ive_integ_ctrl hi_ive_integ_ctrl;
#define HI_IVE_THRESHOLD_S16_MODE_S16_TO_S8_MIN_MID_MAX OT_IVE_THRESHOLD_S16_MODE_S16_TO_S8_MIN_MID_MAX
#define HI_IVE_THRESHOLD_S16_MODE_S16_TO_S8_MIN_ORIG_MAX OT_IVE_THRESHOLD_S16_MODE_S16_TO_S8_MIN_ORIG_MAX
#define HI_IVE_THRESHOLD_S16_MODE_S16_TO_U8_MIN_MID_MAX OT_IVE_THRESHOLD_S16_MODE_S16_TO_U8_MIN_MID_MAX
#define HI_IVE_THRESHOLD_S16_MODE_S16_TO_U8_MIN_ORIG_MAX OT_IVE_THRESHOLD_S16_MODE_S16_TO_U8_MIN_ORIG_MAX
#define HI_IVE_THRESHOLD_S16_MODE_BUTT OT_IVE_THRESHOLD_S16_MODE_BUTT
typedef ot_ive_threshold_s16_mode hi_ive_threshold_s16_mode;
typedef ot_ive_threshold_s16_ctrl hi_ive_threshold_s16_ctrl;
#define HI_IVE_THRESHOLD_U16_MODE_U16_TO_U8_MIN_MID_MAX OT_IVE_THRESHOLD_U16_MODE_U16_TO_U8_MIN_MID_MAX
#define HI_IVE_THRESHOLD_U16_MODE_U16_TO_U8_MIN_ORIG_MAX OT_IVE_THRESHOLD_U16_MODE_U16_TO_U8_MIN_ORIG_MAX
#define HI_IVE_THRESHOLD_U16_MODE_BUTT OT_IVE_THRESHOLD_U16_MODE_BUTT
typedef ot_ive_threshold_u16_mode hi_ive_threshold_u16_mode;
typedef ot_ive_threshold_u16_ctrl hi_ive_threshold_u16_ctrl;
#define HI_IVE_16BIT_TO_8BIT_MODE_S16_TO_S8 OT_IVE_16BIT_TO_8BIT_MODE_S16_TO_S8
#define HI_IVE_16BIT_TO_8BIT_MODE_S16_TO_U8_ABS OT_IVE_16BIT_TO_8BIT_MODE_S16_TO_U8_ABS
#define HI_IVE_16BIT_TO_8BIT_MODE_S16_TO_U8_BIAS OT_IVE_16BIT_TO_8BIT_MODE_S16_TO_U8_BIAS
#define HI_IVE_16BIT_TO_8BIT_MODE_U16_TO_U8 OT_IVE_16BIT_TO_8BIT_MODE_U16_TO_U8
#define HI_IVE_16BIT_TO_8BIT_MODE_BUTT OT_IVE_16BIT_TO_8BIT_MODE_BUTT
typedef ot_ive_16bit_to_8bit_mode hi_ive_16bit_to_8bit_mode;
typedef ot_ive_16bit_to_8bit_ctrl hi_ive_16bit_to_8bit_ctrl;
#define HI_IVE_ORDER_STATS_FILTER_MODE_MEDIAN OT_IVE_ORDER_STATS_FILTER_MODE_MEDIAN
#define HI_IVE_ORDER_STATS_FILTER_MODE_MAX OT_IVE_ORDER_STATS_FILTER_MODE_MAX
#define HI_IVE_ORDER_STATS_FILTER_MODE_MIN OT_IVE_ORDER_STATS_FILTER_MODE_MIN
#define HI_IVE_ORD_STAT_FILTER_MODE_BUTT OT_IVE_ORD_STAT_FILTER_MODE_BUTT
typedef ot_ive_order_stats_filter_mode hi_ive_order_stats_filter_mode;
typedef ot_ive_order_stats_filter_ctrl hi_ive_order_stats_filter_ctrl;
#define HI_IVE_MAP_MODE_U8 OT_IVE_MAP_MODE_U8
#define HI_IVE_MAP_MODE_S16 OT_IVE_MAP_MODE_S16
#define HI_IVE_MAP_MODE_U16 OT_IVE_MAP_MODE_U16
#define HI_IVE_MAP_MODE_BUTT OT_IVE_MAP_MODE_BUTT
typedef ot_ive_map_mode hi_ive_map_mode;
typedef ot_ive_map_ctrl hi_ive_map_ctrl;
typedef ot_ive_map_u8bit_lut_mem hi_ive_map_u8bit_lut_mem;
typedef ot_ive_map_u16bit_lut_mem hi_ive_map_u16bit_lut_mem;
typedef ot_ive_map_s16bit_lut_mem hi_ive_map_s16bit_lut_mem;
typedef ot_ive_equalize_hist_ctrl_mem hi_ive_equalize_hist_ctrl_mem;
typedef ot_ive_equalize_hist_ctrl hi_ive_equalize_hist_ctrl;
typedef ot_ive_add_ctrl hi_ive_add_ctrl;
typedef ot_ive_ncc_dst_mem hi_ive_ncc_dst_mem;
typedef ot_ive_rgn hi_ive_rgn;
typedef ot_ive_ccblob_info hi_ive_ccblob_info;
typedef ot_ive_ccblob hi_ive_ccblob;
#define HI_IVE_CCL_MODE_4C OT_IVE_CCL_MODE_4C
#define HI_IVE_CCL_MODE_8C OT_IVE_CCL_MODE_8C
#define HI_IVE_CCL_MODE_BUTT OT_IVE_CCL_MODE_BUTT
typedef ot_ive_ccl_mode hi_ive_ccl_mode;
typedef ot_ive_ccl_ctrl hi_ive_ccl_ctrl;
typedef ot_ive_gmm_ctrl hi_ive_gmm_ctrl;
#define HI_IVE_GMM2_SNS_FACTOR_MODE_GLOBAL OT_IVE_GMM2_SNS_FACTOR_MODE_GLOBAL
#define HI_IVE_GMM2_SNS_FACTOR_MODE_PIXEL OT_IVE_GMM2_SNS_FACTOR_MODE_PIXEL
#define HI_IVE_GMM2_SNS_FACTOR_MODE_BUTT OT_IVE_GMM2_SNS_FACTOR_MODE_BUTT
typedef ot_ive_gmm2_sns_factor_mode hi_ive_gmm2_sns_factor_mode;
#define HI_IVE_GMM2_LIFE_UPDATE_FACTOR_MODE_GLOBAL OT_IVE_GMM2_LIFE_UPDATE_FACTOR_MODE_GLOBAL
#define HI_IVE_GMM2_LIFE_UPDATE_FACTOR_MODE_PIXEL OT_IVE_GMM2_LIFE_UPDATE_FACTOR_MODE_PIXEL
#define HI_IVE_GMM2_LIFE_UPDATE_FACTOR_MODE_BUTT OT_IVE_GMM2_LIFE_UPDATE_FACTOR_MODE_BUTT
typedef ot_ive_gmm2_life_update_factor_mode hi_ive_gmm2_life_update_factor_mode;
typedef ot_ive_gmm2_ctrl hi_ive_gmm2_ctrl;
typedef ot_ive_canny_hys_edge_ctrl hi_ive_canny_hys_edge_ctrl;
typedef ot_ive_canny_stack_size hi_ive_canny_stack_size;
#define HI_IVE_LBP_COMPARE_MODE_NORMAL OT_IVE_LBP_COMPARE_MODE_NORMAL
#define HI_IVE_LBP_COMPARE_MODE_ABS OT_IVE_LBP_COMPARE_MODE_ABS
#define HI_IVE_LBP_COMPARE_MODE_BUTT OT_IVE_LBP_COMPARE_MODE_BUTT
typedef ot_ive_lbp_compare_mode hi_ive_lbp_compare_mode;
typedef ot_ive_lbp_ctrl hi_ive_lbp_ctrl;
#define HI_IVE_NORM_GRAD_OUT_CTRL_HOR_AND_VER OT_IVE_NORM_GRAD_OUT_CTRL_HOR_AND_VER
#define HI_IVE_NORM_GRAD_OUT_CTRL_HOR OT_IVE_NORM_GRAD_OUT_CTRL_HOR
#define HI_IVE_NORM_GRAD_OUT_CTRL_VER OT_IVE_NORM_GRAD_OUT_CTRL_VER
#define HI_IVE_NORM_GRAD_OUT_CTRL_COMBINE OT_IVE_NORM_GRAD_OUT_CTRL_COMBINE
#define HI_IVE_NORM_GRAD_OUT_CTRL_BUTT OT_IVE_NORM_GRAD_OUT_CTRL_BUTT
typedef ot_ive_norm_grad_out_ctrl hi_ive_norm_grad_out_ctrl;
typedef ot_ive_norm_grad_ctrl hi_ive_norm_grad_ctrl;
#define HI_IVE_LK_OPTICAL_FLOW_PYR_OUT_MODE_NONE OT_IVE_LK_OPTICAL_FLOW_PYR_OUT_MODE_NONE
#define HI_IVE_LK_OPTICAL_FLOW_PYR_OUT_MODE_STATUS OT_IVE_LK_OPTICAL_FLOW_PYR_OUT_MODE_STATUS
#define HI_IVE_LK_OPTICAL_FLOW_PYR_OUT_MODE_BOTH OT_IVE_LK_OPTICAL_FLOW_PYR_OUT_MODE_BOTH
#define HI_IVE_LK_OPTICAL_FLOW_PYR_OUT_MODE_BUTT OT_IVE_LK_OPTICAL_FLOW_PYR_OUT_MODE_BUTT
typedef ot_ive_lk_optical_flow_pyr_out_mode hi_ive_lk_optical_flow_pyr_out_mode;
typedef ot_ive_lk_optical_flow_pyr_ctrl hi_ive_lk_optical_flow_pyr_ctrl;
typedef ot_ive_st_max_eig_val hi_ive_st_max_eig_val;
typedef ot_ive_st_cand_corner_ctrl hi_ive_st_cand_corner_ctrl;
typedef ot_ive_st_corner_info hi_ive_st_corner_info;
typedef ot_ive_st_corner_ctrl hi_ive_st_corner_ctrl;
#define HI_IVE_GRAD_FG_MODE_USE_CUR_GRAD OT_IVE_GRAD_FG_MODE_USE_CUR_GRAD
#define HI_IVE_GRAD_FG_MODE_FIND_MIN_GRAD OT_IVE_GRAD_FG_MODE_FIND_MIN_GRAD
#define HI_IVE_GRAD_FG_MODE_BUTT OT_IVE_GRAD_FG_MODE_BUTT
typedef ot_ive_grad_fg_mode hi_ive_grad_fg_mode;
typedef ot_ive_grad_fg_ctrl hi_ive_grad_fg_ctrl;
typedef ot_ive_cand_bg_pixel hi_ive_cand_bg_pixel;
typedef ot_ive_wrok_bg_pixel hi_ive_wrok_bg_pixel;
typedef ot_ive_bg_life hi_ive_bg_life;
typedef ot_ive_bg_model_pixel hi_ive_bg_model_pixel;
typedef ot_ive_fg_status_data hi_ive_fg_status_data;
typedef ot_ive_bg_status_data hi_ive_bg_status_data;
typedef ot_ive_match_bg_model_ctrl hi_ive_match_bg_model_ctrl;
typedef ot_ive_update_bg_model_ctrl hi_ive_update_bg_model_ctrl;
#define HI_IVE_ANN_MLP_ACTV_FUNC_IDENTITY OT_IVE_ANN_MLP_ACTV_FUNC_IDENTITY
#define HI_IVE_ANN_MLP_ACTV_FUNC_SIGMOID_SYM OT_IVE_ANN_MLP_ACTV_FUNC_SIGMOID_SYM
#define HI_IVE_ANN_MLP_ACTV_FUNC_GAUSSIAN OT_IVE_ANN_MLP_ACTV_FUNC_GAUSSIAN
#define HI_IVE_ANN_MLP_ACTV_FUNC_BUTT OT_IVE_ANN_MLP_ACTV_FUNC_BUTT
typedef ot_ive_ann_mlp_actv_func hi_ive_ann_mlp_actv_func;
#define HI_IVE_ANN_MLP_ACCURATE_SRC16_WGT16 OT_IVE_ANN_MLP_ACCURATE_SRC16_WGT16
#define HI_IVE_ANN_MLP_ACCURATE_SRC14_WGT20 OT_IVE_ANN_MLP_ACCURATE_SRC14_WGT20
#define HI_IVE_ANN_MLP_ACCURATE_BUTT OT_IVE_ANN_MLP_ACCURATE_BUTT
typedef ot_ive_ann_mlp_accurate hi_ive_ann_mlp_accurate;
typedef ot_ive_ann_mlp_model hi_ive_ann_mlp_model;
#define HI_IVE_SVM_TYPE_C_SVC OT_IVE_SVM_TYPE_C_SVC
#define HI_IVE_SVM_TYPE_NU_SVC OT_IVE_SVM_TYPE_NU_SVC
#define HI_IVE_SVM_TYPE_BUTT OT_IVE_SVM_TYPE_BUTT
typedef ot_ive_svm_type hi_ive_svm_type;
#define HI_IVE_SVM_KERNEL_TYPE_LINEAR OT_IVE_SVM_KERNEL_TYPE_LINEAR
#define HI_IVE_SVM_KERNEL_TYPE_POLY OT_IVE_SVM_KERNEL_TYPE_POLY
#define HI_IVE_SVM_KERNEL_TYPE_RBF OT_IVE_SVM_KERNEL_TYPE_RBF
#define HI_IVE_SVM_KERNEL_TYPE_SIGMOID OT_IVE_SVM_KERNEL_TYPE_SIGMOID
#define HI_IVE_SVM_KERNEL_TYPE_BUTT OT_IVE_SVM_KERNEL_TYPE_BUTT
typedef ot_ive_svm_kernel_type hi_ive_svm_kernel_type;
typedef ot_ive_svm_model hi_ive_svm_model;
#define HI_IVE_SAD_MODE_MB_4X4 OT_IVE_SAD_MODE_MB_4X4
#define HI_IVE_SAD_MODE_MB_8X8 OT_IVE_SAD_MODE_MB_8X8
#define HI_IVE_SAD_MODE_MB_16X16 OT_IVE_SAD_MODE_MB_16X16
#define HI_IVE_SAD_MODE_BUTT OT_IVE_SAD_MODE_BUTT
typedef ot_ive_sad_mode hi_ive_sad_mode;
#define HI_IVE_SAD_OUT_CTRL_16BIT_BOTH OT_IVE_SAD_OUT_CTRL_16BIT_BOTH
#define HI_IVE_SAD_OUT_CTRL_8BIT_BOTH OT_IVE_SAD_OUT_CTRL_8BIT_BOTH
#define HI_IVE_SAD_OUT_CTRL_16BIT_SAD OT_IVE_SAD_OUT_CTRL_16BIT_SAD
#define HI_IVE_SAD_OUT_CTRL_8BIT_SAD OT_IVE_SAD_OUT_CTRL_8BIT_SAD
#define HI_IVE_SAD_OUT_CTRL_THRESHOLD OT_IVE_SAD_OUT_CTRL_THRESHOLD
#define HI_IVE_SAD_OUT_CTRL_BUTT OT_IVE_SAD_OUT_CTRL_BUTT
typedef ot_ive_sad_out_ctrl hi_ive_sad_out_ctrl;
typedef ot_ive_sad_ctrl hi_ive_sad_ctrl;
#define HI_IVE_RESIZE_MODE_LINEAR OT_IVE_RESIZE_MODE_LINEAR
#define HI_IVE_RESIZE_MODE_AREA OT_IVE_RESIZE_MODE_AREA
#define HI_IVE_RESIZE_MODE_BUTT OT_IVE_RESIZE_MODE_BUTT
typedef ot_ive_resize_mode hi_ive_resize_mode;
typedef ot_ive_resize_ctrl hi_ive_resize_ctrl;
#define HI_IVE_CNN_ACTV_FUNC_NONE OT_IVE_CNN_ACTV_FUNC_NONE
#define HI_IVE_CNN_ACTV_FUNC_RELU OT_IVE_CNN_ACTV_FUNC_RELU
#define HI_IVE_CNN_ACTV_FUNC_SIGMOID OT_IVE_CNN_ACTV_FUNC_SIGMOID
#define HI_IVE_CNN_ACTV_FUNC_BUTT OT_IVE_CNN_ACTV_FUNC_BUTT
typedef ot_ive_cnn_actv_func hi_ive_cnn_actv_func;
#define HI_IVE_CNN_POOLING_NONE OT_IVE_CNN_POOLING_NONE
#define HI_IVE_CNN_POOLING_MAX OT_IVE_CNN_POOLING_MAX
#define HI_IVE_CNN_POOLING_AVG OT_IVE_CNN_POOLING_AVG
#define HI_IVE_CNN_POOLING_BUTT OT_IVE_CNN_POOLING_BUTT
typedef ot_ive_cnn_pooling hi_ive_cnn_pooling;
typedef ot_ive_cnn_conv_pooling hi_ive_cnn_conv_pooling;
typedef ot_ive_cnn_fc_info hi_ive_cnn_fc_info;
typedef ot_ive_cnn_model hi_ive_cnn_model;
typedef ot_ive_cnn_ctrl hi_ive_cnn_ctrl;
typedef ot_ive_cnn_result hi_ive_cnn_result;
#define HI_IVE_PERSP_TRANS_ALG_MODE_NR_SIM OT_IVE_PERSP_TRANS_ALG_MODE_NR_SIM
#define HI_IVE_PERSP_TRANS_ALG_MODE_SIM OT_IVE_PERSP_TRANS_ALG_MODE_SIM
#define HI_IVE_PERSP_TRANS_ALG_MODE_AFFINE OT_IVE_PERSP_TRANS_ALG_MODE_AFFINE
#define HI_IVE_PERSP_TRANS_ALG_MODE_BUTT OT_IVE_PERSP_TRANS_ALG_MODE_BUTT
typedef ot_ive_persp_trans_alg_mode hi_ive_persp_trans_alg_mode;
typedef ot_ive_persp_trans_point_pair hi_ive_persp_trans_point_pair;
#define HI_IVE_PERSP_TRANS_CSC_MODE_NONE OT_IVE_PERSP_TRANS_CSC_MODE_NONE
#define HI_IVE_PERSP_TRANS_CSC_MODE_VIDEO_BT601_YUV_TO_RGB OT_IVE_PERSP_TRANS_CSC_MODE_VIDEO_BT601_YUV_TO_RGB
#define HI_IVE_PERSP_TRANS_CSC_MODE_VIDEO_BT709_YUV_TO_RGB OT_IVE_PERSP_TRANS_CSC_MODE_VIDEO_BT709_YUV_TO_RGB
#define HI_IVE_PERSP_TRANS_CSC_MODE_PIC_BT601_YUV_TO_RGB OT_IVE_PERSP_TRANS_CSC_MODE_PIC_BT601_YUV_TO_RGB
#define HI_IVE_PERSP_TRANS_CSC_MODE_PIC_BT709_YUV_TO_RGB OT_IVE_PERSP_TRANS_CSC_MODE_PIC_BT709_YUV_TO_RGB
#define HI_IVE_PERSP_TRANS_CSC_MODE_BUTT OT_IVE_PERSP_TRANS_CSC_MODE_BUTT
typedef ot_ive_persp_trans_csc_mode hi_ive_persp_trans_csc_mode;
typedef ot_ive_persp_trans_ctrl hi_ive_persp_trans_ctrl;
#define HI_IVE_KCF_CORE0 OT_IVE_KCF_CORE0
#define HI_IVE_KCF_CORE_BUTT OT_IVE_KCF_CORE_BUTT
typedef ot_ive_kcf_core_id hi_ive_kcf_core_id;
typedef ot_ive_roi_info hi_ive_roi_info;
#define HI_IVE_CSC_MODE_VIDEO_BT601_YUV_TO_RGB OT_IVE_CSC_MODE_VIDEO_BT601_YUV_TO_RGB
#define HI_IVE_CSC_MODE_VIDEO_BT709_YUV_TO_RGB OT_IVE_CSC_MODE_VIDEO_BT709_YUV_TO_RGB
#define HI_IVE_CSC_MODE_PIC_BT601_YUV_TO_RGB OT_IVE_CSC_MODE_PIC_BT601_YUV_TO_RGB
#define HI_IVE_CSC_MODE_PIC_BT709_YUV_TO_RGB OT_IVE_CSC_MODE_PIC_BT709_YUV_TO_RGB
typedef ot_ive_kcf_proc_ctrl hi_ive_kcf_proc_ctrl;
typedef ot_ive_kcf_obj hi_ive_kcf_obj;
typedef ot_ive_list_head hi_ive_list_head;
typedef ot_ive_kcf_obj_node hi_ive_kcf_obj_node;
#define HI_IVE_KCF_LIST_STATE_CREATE OT_IVE_KCF_LIST_STATE_CREATE
#define HI_IVE_KCF_LIST_STATE_DESTORY OT_IVE_KCF_LIST_STATE_DESTORY
#define HI_IVE_KCF_LIST_STATE_BUTT OT_IVE_KCF_LIST_STATE_BUTT
typedef ot_ive_kcf_list_state hi_ive_kcf_list_state;
typedef ot_ive_kcf_obj_list hi_ive_kcf_obj_list;
typedef ot_ive_kcf_bbox hi_ive_kcf_bbox;
typedef ot_ive_kcf_bbox_ctrl hi_ive_kcf_bbox_ctrl;
#define HI_IVE_HOG_MODE_VER_TANGENT_PLANE OT_IVE_HOG_MODE_VER_TANGENT_PLANE
#define HI_IVE_HOG_MODE_HOR_TANGENT_PLANE OT_IVE_HOG_MODE_HOR_TANGENT_PLANE
#define HI_IVE_HOG_MODE_BUTT OT_IVE_HOG_MODE_BUTT
typedef ot_ive_hog_mode hi_ive_hog_mode;
typedef ot_ive_hog_ctrl hi_ive_hog_ctrl;

#ifdef __cplusplus
}
#endif
#endif /* HI_COMMON_IVE_H */