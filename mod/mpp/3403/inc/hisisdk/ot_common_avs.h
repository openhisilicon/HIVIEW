/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */

#ifndef __OT_COMMON_AVS_H__
#define __OT_COMMON_AVS_H__

#include "ot_type.h"
#include "ot_common.h"
#include "ot_common_video.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* end of #ifdef __cplusplus */

#define OT_ERR_AVS_NULL_PTR       OT_DEFINE_ERR(OT_ID_AVS, OT_ERR_LEVEL_ERROR, OT_ERR_NULL_PTR)
#define OT_ERR_AVS_NOT_READY      OT_DEFINE_ERR(OT_ID_AVS, OT_ERR_LEVEL_ERROR, OT_ERR_NOT_READY)
#define OT_ERR_AVS_INVALID_DEVID  OT_DEFINE_ERR(OT_ID_AVS, OT_ERR_LEVEL_ERROR, OT_ERR_INVALID_DEV_ID)
#define OT_ERR_AVS_INVALID_PIPEID OT_DEFINE_ERR(OT_ID_AVS, OT_ERR_LEVEL_ERROR, OT_ERR_INVALID_PIPE_ID)
#define OT_ERR_AVS_INVALID_CHNID  OT_DEFINE_ERR(OT_ID_AVS, OT_ERR_LEVEL_ERROR, OT_ERR_INVALID_CHN_ID)
#define OT_ERR_AVS_EXIST          OT_DEFINE_ERR(OT_ID_AVS, OT_ERR_LEVEL_ERROR, OT_ERR_EXIST)
#define OT_ERR_AVS_UNEXIST        OT_DEFINE_ERR(OT_ID_AVS, OT_ERR_LEVEL_ERROR, OT_ERR_UNEXIST)
#define OT_ERR_AVS_NOT_SUPPORT    OT_DEFINE_ERR(OT_ID_AVS, OT_ERR_LEVEL_ERROR, OT_ERR_NOT_SUPPORT)
#define OT_ERR_AVS_NOT_PERM       OT_DEFINE_ERR(OT_ID_AVS, OT_ERR_LEVEL_ERROR, OT_ERR_NOT_PERM)
#define OT_ERR_AVS_NO_MEM         OT_DEFINE_ERR(OT_ID_AVS, OT_ERR_LEVEL_ERROR, OT_ERR_NO_MEM)
#define OT_ERR_AVS_NO_BUF         OT_DEFINE_ERR(OT_ID_AVS, OT_ERR_LEVEL_ERROR, OT_ERR_NO_BUF)
#define OT_ERR_AVS_ILLEGAL_PARAM  OT_DEFINE_ERR(OT_ID_AVS, OT_ERR_LEVEL_ERROR, OT_ERR_ILLEGAL_PARAM)
#define OT_ERR_AVS_BUSY           OT_DEFINE_ERR(OT_ID_AVS, OT_ERR_LEVEL_ERROR, OT_ERR_BUSY)
#define OT_ERR_AVS_BUF_EMPTY      OT_DEFINE_ERR(OT_ID_AVS, OT_ERR_LEVEL_ERROR, OT_ERR_BUF_EMPTY)


typedef enum {
    OT_AVS_LUT_ACCURACY_HIGH = 0, /* LUT high accuracy. */
    OT_AVS_LUT_ACCURACY_LOW = 1, /* LUT low accuracy. */
    OT_AVS_LUT_ACCURACY_BUTT
} ot_avs_lut_accuracy;

typedef struct {
    ot_avs_lut_accuracy accuracy; /* RW; Range: [0, 1]; LUT accuracy. */
    td_phys_addr_t phys_addr[OT_AVS_PIPE_NUM]; /* RW; physical address of the LUT. */
} ot_avs_lut;

typedef enum {
    OT_AVS_PROJECTION_EQUIRECTANGULAR = 0, /* Equirectangular mode. */
    OT_AVS_PROJECTION_RECTILINEAR = 1, /* Rectilinear mode. */
    OT_AVS_PROJECTION_CYLINDRICAL = 2, /* Cylindrical mode. */
    OT_AVS_PROJECTION_CUBE_MAP = 3, /* Cube map mode. */
    OT_AVS_PROJECTION_BUTT
} ot_avs_projection_mode;

typedef enum {
    OT_AVS_GAIN_MODE_MANUAL = 0, /* Manual gain mode. */
    OT_AVS_GAIN_MODE_AUTO = 1, /* Auto gain mode. */
    OT_AVS_GAIN_MODE_BUTT
} ot_avs_gain_mode;

typedef enum {
    OT_AVS_MODE_BLEND = 0, /* Stitching in blend mode. */
    OT_AVS_MODE_NOBLEND_VER = 1, /* Vertical stitching, no blending. */
    OT_AVS_MODE_NOBLEND_HOR = 2, /* Horizontal stitching, no blending. */
    OT_AVS_MODE_NOBLEND_QR = 3, /* Quarter stitching, no blending. */
    OT_AVS_MODE_BUTT
} ot_avs_mode;

typedef struct {
    ot_avs_gain_mode mode; /* RW; Range: [0, 1]; gain mode. */
    td_s32 coef[OT_AVS_PIPE_NUM]; /* RW; Range: [0, 65535]; gain coef. */
} ot_avs_gain_attr;

typedef struct {
    td_s32 yaw; /* RW; Range: [-18000, 18000]; yaw angle. */
    td_s32 pitch; /* RW; Range: [-18000, 18000]; pitch angle. */
    td_s32 roll; /* RW; Range: [-18000, 18000]; roll angle. */
} ot_avs_rotation;

typedef struct {
    td_u32 fov_x; /* RW; Range: [1000, 36000]; horizontal FOV. */
    td_u32 fov_y; /* RW; Range: [1000, 18000]; vertical FOV. */
} ot_avs_fov;

typedef struct {
    td_u32 pipe_num; /* RW; Range: [2, 4]; pipe number in the split. */
    ot_avs_pipe pipe[OT_AVS_SPLIT_PIPE_NUM]; /* RW; Range: [0, 6]; pipe id in the split. */
} ot_avs_split_attr;

typedef struct {
    td_bool bg_color_en; /* RW; whether use background color or not. */
    td_u32 bg_color; /* RW; Range: [0, 0xFFFFFF]; background color. */
    td_u32 surface_len; /* RW; Range: [256, 4096]; surface length. */
    ot_point start_point[OT_AVS_CUBE_MAP_SURFACE_NUM]; /* RW; start point of each surface. */
} ot_avs_cube_map_attr;

typedef struct {
    ot_avs_projection_mode projection_mode; /* RW; Range: [0, 3]; projection mode. */
    ot_point center; /* center point. */
    ot_avs_fov fov; /* output FOV. */
    ot_avs_rotation orig_rotation; /* output original rotation. */
    ot_avs_rotation rotation; /* output rotation. */
    ot_avs_split_attr split_attr[OT_AVS_SPLIT_NUM]; /* split attribute for 7 or 8 inputs stitching. */
    ot_avs_cube_map_attr cube_map_attr; /* cube map attribute. */
} ot_avs_out_attr;

typedef struct {
    ot_avs_mode mode; /* RW; Range: [0, 3]; group work mode */
    td_u32 pipe_num; /* RW; Range: [1, 4]; pipe number. */
    td_bool sync_pipe_en; /* RW; whether sync pipe image. */
    ot_avs_lut lut; /* look up table. */
    ot_avs_gain_attr gain_attr; /* gain attribute. */
    ot_avs_out_attr out_attr; /* output attribute. */
    ot_frame_rate_ctrl frame_rate; /* frame rate control info. */
} ot_avs_grp_attr;

typedef struct {
    td_u32 width; /* RW; Range: [256, 16384]; width of target image. */
    td_u32 height; /* RW; Range: [256, 8192]; height of target image. */
    ot_compress_mode compress_mode; /* RW; compression mode of the output. */
    ot_dynamic_range dynamic_range; /* RW; dynamic range. */
    td_u32 depth; /* RW; Range: [0, 8]; chn user list depth. */
} ot_avs_chn_attr;

typedef struct {
    td_u32 working_set_size; /* RW; Range: [1, 4000000]; working set size;  */
} ot_avs_mod_param;

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* end of #ifdef __cplusplus */

#endif /* end of #ifndef __OT_COMMON_AVS_H__ */

