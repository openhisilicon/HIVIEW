/*
  Copyright (c), 2001-2024, Shenshu Tech. Co., Ltd.
 */

#ifndef OT_COMMON_DPU_MATCH_H
#define OT_COMMON_DPU_MATCH_H

#include "ot_common.h"
#include "ot_errno.h"
#include "ot_debug.h"
#include "ot_common_video.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

#define OT_DPU_MATCH_LEFT_PIPE            0
#define OT_DPU_MATCH_RIGHT_PIPE           1

typedef td_s32 ot_dpu_match_grp;
typedef td_s32 ot_dpu_match_pipe;
typedef td_s32 ot_dpu_match_chn;

typedef enum {
    OT_ERR_DPU_MATCH_SYS_TIMEOUT = 0x40, /* dpu match process timeout */
    OT_ERR_DPU_MATCH_OPEN_FILE = 0x41, /* dpu match open file error */
    OT_ERR_DPU_MATCH_READ_FILE = 0x42, /* dpu match read file error */
    OT_ERR_DPU_MATCH_WRITE_FILE = 0x43, /* dpu match write file error */

    OT_ERR_DPU_MATCH_BUTT
} ot_dpu_match_err_code;

#define OT_ERR_DPU_MATCH_NULL_PTR       OT_DEFINE_ERR(OT_ID_DPU_MATCH, OT_ERR_LEVEL_ERROR, OT_ERR_NULL_PTR)
#define OT_ERR_DPU_MATCH_NOT_READY      OT_DEFINE_ERR(OT_ID_DPU_MATCH, OT_ERR_LEVEL_ERROR, OT_ERR_NOT_READY)
#define OT_ERR_DPU_MATCH_INVALID_DEV_ID OT_DEFINE_ERR(OT_ID_DPU_MATCH, OT_ERR_LEVEL_ERROR, OT_ERR_INVALID_DEV_ID)
#define OT_ERR_DPU_MATCH_INVALID_CHN_ID OT_DEFINE_ERR(OT_ID_DPU_MATCH, OT_ERR_LEVEL_ERROR, OT_ERR_INVALID_CHN_ID)
#define OT_ERR_DPU_MATCH_INVALID_GRP_ID OT_DEFINE_ERR(OT_ID_DPU_MATCH, OT_ERR_LEVEL_ERROR, OT_ERR_INVALID_GRP_ID)
#define OT_ERR_DPU_MATCH_EXIST          OT_DEFINE_ERR(OT_ID_DPU_MATCH, OT_ERR_LEVEL_ERROR, OT_ERR_EXIST)
#define OT_ERR_DPU_MATCH_UNEXIST        OT_DEFINE_ERR(OT_ID_DPU_MATCH, OT_ERR_LEVEL_ERROR, OT_ERR_UNEXIST)
#define OT_ERR_DPU_MATCH_NOT_SUPPORT    OT_DEFINE_ERR(OT_ID_DPU_MATCH, OT_ERR_LEVEL_ERROR, OT_ERR_NOT_SUPPORT)
#define OT_ERR_DPU_MATCH_NOT_PERM       OT_DEFINE_ERR(OT_ID_DPU_MATCH, OT_ERR_LEVEL_ERROR, OT_ERR_NOT_PERM)
#define OT_ERR_DPU_MATCH_NO_MEM         OT_DEFINE_ERR(OT_ID_DPU_MATCH, OT_ERR_LEVEL_ERROR, OT_ERR_NO_MEM)
#define OT_ERR_DPU_MATCH_NO_BUF         OT_DEFINE_ERR(OT_ID_DPU_MATCH, OT_ERR_LEVEL_ERROR, OT_ERR_NO_BUF)
#define OT_ERR_DPU_MATCH_ILLEGAL_PARAM  OT_DEFINE_ERR(OT_ID_DPU_MATCH, OT_ERR_LEVEL_ERROR, OT_ERR_ILLEGAL_PARAM)
#define OT_ERR_DPU_MATCH_BUSY           OT_DEFINE_ERR(OT_ID_DPU_MATCH, OT_ERR_LEVEL_ERROR, OT_ERR_BUSY)
#define OT_ERR_DPU_MATCH_BUF_EMPTY      OT_DEFINE_ERR(OT_ID_DPU_MATCH, OT_ERR_LEVEL_ERROR, OT_ERR_BUF_EMPTY)
#define OT_ERR_DPU_MATCH_BUF_FULL       OT_DEFINE_ERR(OT_ID_DPU_MATCH, OT_ERR_LEVEL_ERROR, OT_ERR_BUF_FULL)

#define OT_ERR_DPU_MATCH_SYS_TIMEOUT    OT_DEFINE_ERR(OT_ID_DPU_MATCH, OT_ERR_LEVEL_ERROR, OT_ERR_DPU_MATCH_SYS_TIMEOUT)
#define OT_ERR_DPU_MATCH_OPEN_FILE      OT_DEFINE_ERR(OT_ID_DPU_MATCH, OT_ERR_LEVEL_ERROR, OT_ERR_DPU_MATCH_OPEN_FILE)
#define OT_ERR_DPU_MATCH_READ_FILE      OT_DEFINE_ERR(OT_ID_DPU_MATCH, OT_ERR_LEVEL_ERROR, OT_ERR_DPU_MATCH_READ_FILE)
#define OT_ERR_DPU_MATCH_WRITE_FILE     OT_DEFINE_ERR(OT_ID_DPU_MATCH, OT_ERR_LEVEL_ERROR, OT_ERR_DPU_MATCH_WRITE_FILE)

typedef struct {
    td_u64 phys_addr;
    td_u64 virt_addr;
    td_u32 size;
} ot_dpu_match_mem_info;

typedef struct {
    ot_video_frame_info src_frame[OT_DPU_MATCH_MAX_PIPE_NUM]; /* src_frame[0]:left frame,src_frame[1]:right frame */
    ot_video_frame_info rect_frame[OT_DPU_MATCH_MAX_PIPE_NUM]; /* rect_frame[0]:left frame,rect_frame[1]:right frame */
} ot_dpu_match_src_frame_info;

typedef struct {
    ot_video_frame_info disp_frame; /* disparity frame */
    ot_video_frame_info conf_frame; /* confidence frame */
    ot_video_frame_info depth_frame; /* depth frame */
} ot_dpu_match_dst_frame_info;

typedef enum {
    OT_DPU_MATCH_MASK_DEFAULT_MODE = 0x0,
    OT_DPU_MATCH_MASK_1X1_MODE = 0x1,
    OT_DPU_MATCH_MASK_3X3_MODE = 0x2,
    OT_DPU_MATCH_MASK_5X5_MODE = 0x3,
    OT_DPU_MATCH_MASK_7X7_MODE = 0x4,
    OT_DPU_MATCH_MASK_9X9_MODE = 0x5,
    OT_DPU_MATCH_MASK_11X11_MODE = 0x6,
    OT_DPU_MATCH_MASK_13X13_MODE = 0x7,
    OT_DPU_MATCH_MASK_15X15_MODE = 0x8,
    OT_DPU_MATCH_MASK_17X17_MODE = 0x9,
    OT_DPU_MATCH_MASK_MODE_BUTT
} ot_dpu_match_mask_mode;

typedef enum {
    OT_DPU_MATCH_SPEED_ACCURACY_MODE_SPEED = 0x0,
    OT_DPU_MATCH_SPEED_ACCURACY_MODE_ACCURACY = 0x1,
    OT_DPU_MATCH_SPEED_ACCURACY_MODE_BUTT
} ot_dpu_match_speed_accuracy_mode;

typedef enum {
    OT_DPU_MATCH_DISP_FROM_MATCH = 0x0,
    OT_DPU_MATCH_DISP_FROM_USER = 0x1,  /* only disparity to depth */
    OT_DPU_MATCH_DISP_FROM_BUTT
} ot_dpu_match_disp_from;

typedef enum {
    OT_DPU_MATCH_BG_MODE_NONE = 0x0,
    OT_DPU_MATCH_BG_MODE_LOW_LIGHT = 0x1,
    OT_DPU_MATCH_BG_MODE_NORMAL_LIGHT = 0x2,
    OT_DPU_MATCH_BG_MODE_HIGH_LIGHT = 0x3,
    OT_DPU_MATCH_BG_MODE_BUTT
} ot_dpu_match_bg_mode;

typedef enum {
    OT_DPU_MATCH_OPTI_MODE_DEFAULT = 0x0,
    OT_DPU_MATCH_OPTI_MODE_ADAPTIVE = 0x1,
    OT_DPU_MATCH_OPTI_MODE_BUTT
} ot_dpu_match_opti_mode;

typedef enum {
    OT_DPU_MATCH_DISP_VERSION_V1 = 0,
    OT_DPU_MATCH_DISP_VERSION_BUTT
} ot_dpu_match_disp_version;

typedef enum {
    OT_DPU_MATCH_DEPTH_VERSION_V1 = 0,
    OT_DPU_MATCH_DEPTH_VERSION_BUTT
} ot_dpu_match_depth_version;

typedef struct {
    td_bool is_proc_disp;
    td_bool is_output_disp;
    td_bool is_output_conf;
    td_bool is_post_proc;
    td_bool is_filter_disp_by_conf; /* confidence filter disparity switch */
    td_bool is_time_domain_filter;
    td_bool is_enable_disp_subpixel; /* calculate subpixel disparity or not. */
    ot_dpu_match_mask_mode mask_mode; /* aggregation mask mode. */
    ot_dpu_match_speed_accuracy_mode speed_accuracy_mode; /* adjust speed-accuracy trade-off. */
    ot_dpu_match_bg_mode  bg_mode; /* backgroud mode */
    ot_dpu_match_opti_mode opti_mode; /* update the correlation value of first_penalty_coef and second_penalty_coef */
    td_u16 disp_num; /* the number of disparity, it must be the multiple of 16. range:[16,224] */
    td_s16 disp_start_pos; /* minimum disparity, it must be the multiple of 2. range:[-224, 224] */
    td_u16 first_penalty_coef; /* First penalty coefficient. Range: [0, 127] */
    td_u16 second_penalty_coef; /* Secode penalty coefficient. Range: [0, 127] */
    td_u8 fg_init_cost_zero_thr;  /* the foreground overexposure area filter threshold,[1, 16] */
    td_u8 local_feature; /* census cost rate, [0, 8] */
    td_u8 conf_filter_disp_thr; /* confidece filter disparity threshold, [0,15] */
    td_u8 aggregate_coef; /* aggregate coefficient.[0, 8] */
    td_u8 unique_ratio; /* uniqueness ratio. [0, 31] */
    td_u8 rsv[3];
} ot_dpu_match_disp_param_v1;

typedef struct {
    ot_dpu_match_disp_version version;
    union {
        ot_dpu_match_disp_param_v1 v1; /* v1 param,AUTO:ot_dpu_match_disp_version:OT_DPU_MATCH_DISP_VERSION_V1 */
    };
} ot_dpu_match_disp_param;

typedef struct {
    td_bool is_output_depth; /* output depth switch */
    ot_dpu_match_disp_from disp_from; /* disparity source */
    td_u32 fb; /* fixed-point fb，the decimal bits wide is fb_norm_bits，fixed-point:{24.8,22.10,20.12} */
    td_u16 z0; /* z0 */
    td_u16 min_depth_thr; /* min valid depth value */
    td_u16 max_depth_thr; /* max valid depth value */
    td_u16 invalid_depth; /* invalid depth  */
    td_s8 disp_coef; /* the coefficient of disparity to depth, {-1, 1} */
    td_u8 fb_norm_bits; /* the decimal bits wide of fb, {8,10,12} */
    td_u8 depth_prec; /* the decimal bits wide of depth, {0,2,4} .one-to-one correspondence fb_norm_bits. */
    td_u8 rsv;
} ot_dpu_match_depth_param_v1;

typedef struct {
    ot_dpu_match_depth_version version;
    union {
        ot_dpu_match_depth_param_v1 v1; /* v1 param,AUTO:ot_dpu_match_depth_version:OT_DPU_MATCH_DEPTH_VERSION_V1 */
    };
} ot_dpu_match_depth_param;

typedef struct {
    ot_size left_image_size; /* left image size. */
    ot_size right_image_size; /* right image size. */
    td_s32 invalid_disp;  /* output this value,when cannot match valid disparity, [-225, 223] << 6 */
    td_u32 depth; /* the depth of user image queue for getting match output image,
                     it can not be changed dynamic. range:[0,8] */
    td_bool is_need_src_frame; /* the flag of getting source videoframe.it will effect when bind dpu rect. */
    ot_dpu_match_mem_info assist_buf; /* assistance buffer. */
    ot_frame_rate_ctrl frame_rate; /* grp frame rate control. */
    ot_dpu_match_disp_param disp_param; /*  disparity param */
    ot_dpu_match_depth_param depth_param; /* disparity to depth param */
} ot_dpu_match_grp_attr;

typedef struct {
    ot_size image_size; /* output image size. */
} ot_dpu_match_chn_attr;


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* OT_COMMON_MATCH_H */
