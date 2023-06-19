/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */

#ifndef OT_COMMON_MATCH_H
#define OT_COMMON_MATCH_H

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

#define OT_DPU_MATCH_PENALTY_COEF_NUM    2

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
} ot_dpu_match_frame_info;

typedef enum {
    OT_DPU_MATCH_MASK_DEFAULT_MODE = 0x0,
    OT_DPU_MATCH_MASK_1X1_MODE = 0x1,
    OT_DPU_MATCH_MASK_3X3_MODE = 0x2,
    OT_DPU_MATCH_MASK_5X5_MODE = 0x3,
    OT_DPU_MATCH_MASK_7X7_MODE = 0x4,
    OT_DPU_MATCH_MASK_9X9_MODE = 0x5,
    OT_DPU_MATCH_MASK_MODE_BUTT
} ot_dpu_match_mask_mode;

typedef enum {
    OT_DPU_MATCH_DENSITY_ACCURACY_MODE_D0_A9 = 0x0,
    OT_DPU_MATCH_DENSITY_ACCURACY_MODE_D1_A8 = 0x1,
    OT_DPU_MATCH_DENSITY_ACCURACY_MODE_D2_A7 = 0x2,
    OT_DPU_MATCH_DENSITY_ACCURACY_MODE_D3_A6 = 0x3,
    OT_DPU_MATCH_DENSITY_ACCURACY_MODE_D4_A5 = 0x4,
    OT_DPU_MATCH_DENSITY_ACCURACY_MODE_D5_A4 = 0x5,
    OT_DPU_MATCH_DENSITY_ACCURACY_MODE_D6_A3 = 0x6,
    OT_DPU_MATCH_DENSITY_ACCURACY_MODE_D7_A2 = 0x7,
    OT_DPU_MATCH_DENSITY_ACCURACY_MODE_D8_A1 = 0x8,
    OT_DPU_MATCH_DENSITY_ACCURACY_MODE_D9_A0 = 0x9,
    OT_DPU_MATCH_DENSITY_ACCURACY_MODE_BUTT
} ot_dpu_match_density_accuracy_mode;

typedef enum {
    OT_DPU_MATCH_SPEED_ACCURACY_MODE_SPEED = 0x0,
    OT_DPU_MATCH_SPEED_ACCURACY_MODE_ACCURACY = 0x1,
    OT_DPU_MATCH_SPEED_ACCURACY_MODE_BUTT
} ot_dpu_match_speed_accuracy_mode;

typedef enum {
    OT_DPU_MATCH_DISPARITY_SUBPIXEL_DISABLE = 0x0,
    OT_DPU_MATCH_DISPARITY_SUBPIXEL_ENABLE = 0x1,
    OT_DPU_MATCH_DISPARITY_SUBPIXEL_BUTT
} ot_dpu_match_disparity_subpixel;

typedef struct {
    ot_size left_image_size; /* left image size. */
    ot_size right_image_size; /* right image size. */
    ot_dpu_match_mask_mode match_mask_mode; /* aggregation mask mode. */
    ot_dpu_match_density_accuracy_mode density_accuracy_mode; /* adjust density-accuracy trade-off. */
    ot_dpu_match_speed_accuracy_mode speed_accuracy_mode; /* adjust speed-accuracy trade-off. */
    ot_dpu_match_disparity_subpixel disparity_subpixel_en; /* calculate subpixel disparity or not. */
    td_u16 disparity_num; /* the number of disparity, when the value is less than or equal to 128,
                         it must be the multiple of 16, range:[16,224] */
    td_u16 disparity_start_pos; /* minimum disparity, it must be the multiple of 2. range:[0,126] */
    td_u32 depth; /* the depth of user image queue for getting match output image,
                           it can not be changed dynamic. range:[0,8] */
    td_bool is_need_src_frame; /* the flag of getting source videoframe.it will effect when bind dpu rect. */
    ot_dpu_match_mem_info assist_buf; /* assistance buffer. */
    ot_frame_rate_ctrl frame_rate; /* grp frame rate control. */
} ot_dpu_match_grp_attr;

typedef struct {
    ot_size image_size; /* output image size. */
} ot_dpu_match_chn_attr;

typedef struct {
    td_u16 first_penalty_coef; /* first penalty coefficient. Range: [0, 138] */
    td_u16 second_penalty_coef; /* second penalty coefficient. Range: [0, 138] */
} ot_dpu_match_cost_param;

typedef enum {
    OT_DPU_MATCH_VERSION_V1 = 0,
    OT_DPU_MATCH_VERSION_V2 = 1,
    OT_DPU_MATCH_VERSION_BUTT
} ot_dpu_match_version;

typedef ot_dpu_match_cost_param ot_dpu_match_param_v1;

typedef struct {
    td_u16 first_penalty_coef; /* first penalty coefficient. Range: [0, 138] */
    td_u16 second_penalty_coef; /* second penalty coefficient. Range: [0, 138] */
    td_u8 aggregate_coef; /* aggregate coefficient. Range: [0, 8] */
    td_u8 unique_ratio; /* uniqueness ratio. Range: [0, 31] */
    td_u8 fg_init_cost_zero_thr; /* the foreground overexposure area filter threshold,[1, 16] */
    td_u8 rsv;
} ot_dpu_match_param_v2;

typedef struct {
    ot_dpu_match_version version;
    union {
        ot_dpu_match_param_v1 v1; /* v1 param,AUTO:ot_dpu_match_version:OT_DPU_MATCH_VERSION_V1 */
        ot_dpu_match_param_v2 v2; /* v2 param,AUTO:ot_dpu_match_version:OT_DPU_MATCH_VERSION_V2 */
    };
} ot_dpu_match_param;

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* OT_COMMON_MATCH_H */
