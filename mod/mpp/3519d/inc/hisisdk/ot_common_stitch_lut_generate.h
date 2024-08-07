/*
  Copyright (c), 2001-2024, Shenshu Tech. Co., Ltd.
 */

#ifndef OT_COMMON_STITCH_LUT_GENERATE_H
#define OT_COMMON_STITCH_LUT_GENERATE_H

#include "ot_type.h"
#include "ot_common_video.h"

#ifdef __cplusplus
extern "C" {
#endif

#define FILE_NAME_SIZE 255
#define GDC_MAX_STITCH_NUM  4
#define FILE_HEADER_SIZE      100

typedef enum {
    PROJECTION_EQUIRECTANGULAR = 0,  /* Equirectangular mode. */
    PROJECTION_RECTILINEAR = 1,      /* Rectilinear mode. */
    PROJECTION_CYLINDRICAL = 2,      /* Cylindrical mode. */
    PROJECTION_BUTT
} ot_projection_mode;

typedef struct {
    ot_size          in_size;        /* lut image in size */
    ot_size          out_size;       /* lut image out size */
    ot_size          mesh_size;      /* lut size, added for pqtools */
    ot_lut_cell_size cell_size;      /* lut cell size */
    td_u32           lut_len;        /* length of lut, length = stride * mesh_height */
    ot_point        *mesh_points;    /* lut point */
} ot_stitch_lut;

typedef struct {
    td_s32 fov_x;   /* Range: [1000, 36000]; Horizontal FOV. */
    td_s32 fov_y;   /* Range: [1000, 18000]; Vertical FOV. */
} ot_stitch_fov;

typedef struct {
    td_s32 yaw;      /* Range: [-18000, 18000]; Yaw angle. */
    td_s32 pitch;    /* Range: [-18000, 18000]; Pitch angle. */
    td_s32 roll;     /* Range: [-18000, 18000]; Roll angle. */
} ot_stitch_rotation;

typedef struct {
    td_s32              camera_num;       /* camera number */
    ot_size             src_size;         /* Range: [256, 8192]; Size of source image. */
    ot_size             dst_size;         /* Range: [256, 8192]; Size of target image. */
    ot_projection_mode  prj_mode;         /* Projection mode. */
    ot_point            center;           /* Range: [-16383,16383]: Center point. */
    ot_stitch_fov       fov;              /* Output FOV. */
    ot_stitch_rotation  ori_rotation;     /* Output original rotation. */
    ot_stitch_rotation  rotation;         /* Output rotation. */
} ot_stitch_avs_config;

/*
 * Specification of calibration file type
 * TYPE_AVSP:  calibration file come from AVSP calibration
 * TYPE_PTGUI: calibration file come from PTGUI calibration
 * TYPE_HUGIN: calibration file come from HUGIN calibration
 * TYPE_BUTT:  reserved.
 */
typedef enum {
    TYPE_AVSP = 0,
    TYPE_PTGUI,
    TYPE_HUGIN,
    TYPE_BUTT
} ot_stitch_cal_type;

/*
 * Specification of fine tuning for each camera.
 * adjust_en: the enable of fine tuning for each camera.
 * yaw:       the yaw-direction adjustment for each camera.
 * pitch:     the pitch-direction adjustment for each camera.
 * roll:      the roll-direction adjustment for each camera.
 * offset_x:  the X-direction adjustment for each camera.
 * offset_y:  the Y-direction adjustment for each camera.
 */
typedef struct {
    td_bool adjust_en;
    td_s32  yaw;   /* range: [-18000, 18000]; yaw angle. */
    td_s32  pitch; /* range: [-18000, 18000]; pitch angle. */
    td_s32  roll;  /* range: [-18000, 18000]; roll angle. */
    td_s32  offset_x; /* range: [-width/2*100, width/2*100]; x offset. */
    td_s32  offset_y; /* range: [-height/2*100, height/2*100]; y offset. */
} ot_stitch_adjust;

/*
 * Specification of fine tuning.
 * fine_tuning_en: the enable of fine tuning function.
 * adjust:         the adjustment for each camera.
 */
typedef struct {
    td_bool fine_tuning_en;
    ot_stitch_adjust adjust[GDC_MAX_STITCH_NUM];
} ot_stitch_fine_tuning;

/*
 * the input  of lut generate function.
 * type:            input calibration file type
 * cal_file_name:   file address of input calibration file.
 * stitch_distance: the radius of the sphere used to create points to sample from.
 *                  stitching will be performed correctly at this distance.  (ignored if calibration from pt_gui.)
 * fine_tuning_cfg: pitch/yaw/roll/xoffset/yoffset for each camera
 */
typedef struct {
    ot_stitch_cal_type type;
    td_char *cal_file_name;
    td_float stitch_distance;
    ot_stitch_fine_tuning fine_tuning_cfg;
} ot_stitch_lut_generate_input;

typedef struct {
    ot_stitch_avs_config avs_stitch_config;
    ot_stitch_lut_generate_input lut_input;
} ot_stitch_config;

typedef struct {
    ot_point left_point[GDC_MAX_STITCH_NUM];
    ot_point right_point[GDC_MAX_STITCH_NUM];
} ot_stitch_img_size_cfg;

typedef struct {
    ot_stitch_lut lut[GDC_MAX_STITCH_NUM];
    ot_point overlap_point[GDC_MAX_STITCH_NUM][2]; /* 2 images stitching£¬only overlap_point[1][0].x is valid */
} ot_stitch_out_param;

#ifdef __cplusplus
}
#endif

#endif /* OT_COMMON_STITCH_LUT_GENERATE_H */

