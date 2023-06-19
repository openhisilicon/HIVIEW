/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */

#ifndef OT_COMMON_DPU_RECT_H
#define OT_COMMON_DPU_RECT_H

#include "ot_common.h"
#include "ot_errno.h"
#include "ot_debug.h"
#include "ot_common_video.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

#define OT_DPU_RECT_LEFT_PIPE            0
#define OT_DPU_RECT_RIGHT_PIPE           1
#define OT_DPU_RECT_LEFT_CHN             0
#define OT_DPU_RECT_RIGHT_CHN            1

typedef td_s32 ot_dpu_rect_lut_id;
typedef td_s32 ot_dpu_rect_grp;
typedef td_s32 ot_dpu_rect_pipe;
typedef td_s32 ot_dpu_rect_chn;

typedef enum {
    OT_ERR_DPU_RECT_SYS_TIMEOUT = 0x40, /* dpu rect process timeout */
    OT_ERR_DPU_RECT_OPEN_FILE = 0x41, /* dpu rect open file error */
    OT_ERR_DPU_RECT_READ_FILE = 0x42, /* dpu rect read file error */
    OT_ERR_DPU_RECT_WRITE_FILE = 0x43, /* dpu rect write file error */

    OT_ERR_DPU_RECT_BUTT
} ot_dpu_rect_err_code;

#define OT_ERR_DPU_RECT_NULL_PTR       OT_DEFINE_ERR(OT_ID_DPU_RECT, OT_ERR_LEVEL_ERROR, OT_ERR_NULL_PTR)
#define OT_ERR_DPU_RECT_NOT_READY      OT_DEFINE_ERR(OT_ID_DPU_RECT, OT_ERR_LEVEL_ERROR, OT_ERR_NOT_READY)
#define OT_ERR_DPU_RECT_INVALID_DEV_ID OT_DEFINE_ERR(OT_ID_DPU_RECT, OT_ERR_LEVEL_ERROR, OT_ERR_INVALID_DEV_ID)
#define OT_ERR_DPU_RECT_INVALID_CHN_ID OT_DEFINE_ERR(OT_ID_DPU_RECT, OT_ERR_LEVEL_ERROR, OT_ERR_INVALID_CHN_ID)
#define OT_ERR_DPU_RECT_INVALID_GRP_ID OT_DEFINE_ERR(OT_ID_DPU_RECT, OT_ERR_LEVEL_ERROR, OT_ERR_INVALID_GRP_ID)
#define OT_ERR_DPU_RECT_EXIST          OT_DEFINE_ERR(OT_ID_DPU_RECT, OT_ERR_LEVEL_ERROR, OT_ERR_EXIST)
#define OT_ERR_DPU_RECT_UNEXIST        OT_DEFINE_ERR(OT_ID_DPU_RECT, OT_ERR_LEVEL_ERROR, OT_ERR_UNEXIST)
#define OT_ERR_DPU_RECT_NOT_SUPPORT    OT_DEFINE_ERR(OT_ID_DPU_RECT, OT_ERR_LEVEL_ERROR, OT_ERR_NOT_SUPPORT)
#define OT_ERR_DPU_RECT_NOT_PERM       OT_DEFINE_ERR(OT_ID_DPU_RECT, OT_ERR_LEVEL_ERROR, OT_ERR_NOT_PERM)
#define OT_ERR_DPU_RECT_NO_MEM         OT_DEFINE_ERR(OT_ID_DPU_RECT, OT_ERR_LEVEL_ERROR, OT_ERR_NO_MEM)
#define OT_ERR_DPU_RECT_NO_BUF         OT_DEFINE_ERR(OT_ID_DPU_RECT, OT_ERR_LEVEL_ERROR, OT_ERR_NO_BUF)
#define OT_ERR_DPU_RECT_ILLEGAL_PARAM  OT_DEFINE_ERR(OT_ID_DPU_RECT, OT_ERR_LEVEL_ERROR, OT_ERR_ILLEGAL_PARAM)
#define OT_ERR_DPU_RECT_BUSY           OT_DEFINE_ERR(OT_ID_DPU_RECT, OT_ERR_LEVEL_ERROR, OT_ERR_BUSY)
#define OT_ERR_DPU_RECT_BUF_EMPTY      OT_DEFINE_ERR(OT_ID_DPU_RECT, OT_ERR_LEVEL_ERROR, OT_ERR_BUF_EMPTY)
#define OT_ERR_DPU_RECT_SYS_TIMEOUT    OT_DEFINE_ERR(OT_ID_DPU_RECT, OT_ERR_LEVEL_ERROR, OT_ERR_DPU_RECT_SYS_TIMEOUT)
#define OT_ERR_DPU_RECT_OPEN_FILE      OT_DEFINE_ERR(OT_ID_DPU_RECT, OT_ERR_LEVEL_ERROR, OT_ERR_DPU_RECT_OPEN_FILE)
#define OT_ERR_DPU_RECT_READ_FILE      OT_DEFINE_ERR(OT_ID_DPU_RECT, OT_ERR_LEVEL_ERROR, OT_ERR_DPU_RECT_READ_FILE)
#define OT_ERR_DPU_RECT_WRITE_FILE     OT_DEFINE_ERR(OT_ID_DPU_RECT, OT_ERR_LEVEL_ERROR, OT_ERR_DPU_RECT_WRITE_FILE)

typedef struct {
    td_u64 phys_addr;
    td_u64 virt_addr;
    td_u32 size;
} ot_dpu_rect_mem_info;

typedef struct {
    ot_video_frame_info src_frame[OT_DPU_RECT_MAX_PIPE_NUM]; /* src_frame[0]:left frame,src_frame[1]:right frame */
    ot_video_frame_info rect_frame[OT_DPU_RECT_MAX_CHN_NUM]; /* rect_frame[0]:left frame,rect_frame[1]:right frame */
} ot_dpu_rect_frame_info;

typedef enum {
    OT_DPU_RECT_MODE_SINGLE = 0x0, /* only channel 0 work */
    OT_DPU_RECT_MODE_DOUBLE = 0x1, /* two channel work */
    OT_DPU_RECT_MODE_BUTT
} ot_dpu_rect_mode;

typedef struct {
    ot_dpu_rect_mode rect_mode; /* rectification mode, it can not be changed dynamic */
    ot_size left_image_size; /* left image size. */
    ot_size right_image_size; /* right image size. */
    ot_dpu_rect_lut_id left_lut_id; /* left image rectification lut */
    ot_dpu_rect_lut_id right_lut_id; /* right image rectification lut */
    td_u32 depth; /* the depth of user image queue for getting rectification output image,
                     * it can not be changed dynamic. range:[0,8]
                     */
    td_bool is_need_src_frame; /* the flag of getting source videoframe. */
    ot_frame_rate_ctrl frame_rate; /* grp frame rate control. */
} ot_dpu_rect_grp_attr;

typedef struct {
    ot_size image_size; /* rectification output image size */
} ot_dpu_rect_chn_attr;

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* OT_COMMON_DPU_RECT_H */
