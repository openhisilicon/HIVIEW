/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */

#ifndef OT_COMMON_AVS_LUT_GENERATE_H
#define OT_COMMON_AVS_LUT_GENERATE_H

#include "ot_type.h"
#include "ot_common_avs.h"

#ifdef __cplusplus
extern "C" {
#endif

#define OT_AVS_MAX_CAMERA_NUM 8
#define OT_AVS_LUT_SIZE  0x400000 /* 4MB */
#define OT_AVS_CALIBRATION_FILE_LEN 0x5800 /* 22KB */
#define OT_AVS_MATRIX_SIZE 9

/* the status of avs interface returned */
typedef enum {
    OT_AVS_STATUS_EOF = -1, /* internal error codes */
    OT_AVS_STATUS_OK = 0,   /* success */

    /* error statuses */
    OT_AVS_STATUS_FILE_READ_ERROR,
    OT_AVS_STATUS_FILE_WRITE_ERROR,
    OT_AVS_STATUS_FILE_INVALID,
    OT_AVS_STATUS_ALLOC_FAILED,
    OT_AVS_STATUS_INVALID_PARAM,
    OT_AVS_STATUS_INVALID_INPUT_POINTER,
    OT_AVS_STATUS_INVALID_MASK_POINTER,

    OT_AVS_STATUS_BUTT
} ot_avs_status;

/*
 * Specification of calibration file type
 * OT_AVS_TYPE_AVSP:  calibration file come from AVSP calibration
 * OT_AVS_TYPE_PTGUI: calibration file come from PTGUI calibration
 * OT_AVS_TYPE_HUGIN: calibration file come from HUGIN calibration
 * OT_AVS_TYPE_BUTT:  reserved.
 */
typedef enum {
    OT_AVS_TYPE_AVSP = 0,
    OT_AVS_TYPE_PTGUI,
    OT_AVS_TYPE_HUGIN,
    OT_AVS_TYPE_BUTT
} ot_avs_type;

/*
 * Specification of the mask type
 * OT_AVS_MASK_SHAPE_RECT:    build a rect shape mask;
 * OT_AVS_MASK_SHAPE_ELLIPSE: build a ecllipse shape mask, including circl shape;
 * OT_AVS_MASK_SHAPE_BUTT:    reserved.
 */
typedef enum {
    OT_AVS_MASK_SHAPE_RECT = 0,
    OT_AVS_MASK_SHAPE_ELLIPSE,
    OT_AVS_MASK_SHAPE_BUTT
} ot_avs_mask_shape;

/*
 * Specification of the mask define.
 * mask_shape : the mask shape type
 * offset_h   : the offset of center in x direction
 * offset_v   : the offset of center in y direction
 * half_major_axis: the half of long axis in X direction
 * half_minor_axis: the half of short axis in Y direction
 */
typedef struct {
    ot_avs_mask_shape mask_shape;
    td_s32 offset_h;
    td_s32 offset_v;
    td_u32 half_major_axis;
    td_u32 half_minor_axis;
} ot_avs_mask_define;

/*
 * Specification of each lut mask input.
 * same_mask_flag: if true, all the mask using mask[0] parameters, otherwise using eack mask parameters;
 * input_yuv_mask_flag: if true, using the yuv400 mask directly;
 *                      else create a new yuv400 by using mask_define parameters.
 * mask_define: mask define for each camera
 * mask_addr: memory for saving each mask, the memory size for each one should be sizeof(td_u16)*width*height,
 *            which width and height are the input resolution.
 */
typedef struct {
    td_bool same_mask_flag;
    td_bool input_yuv_mask_flag;
    ot_avs_mask_define mask_define[OT_AVS_MAX_CAMERA_NUM];
    td_u64 mask_addr[OT_AVS_MAX_CAMERA_NUM];
} ot_avs_lut_mask;

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
    td_s32  offset_x;
    td_s32  offset_y;
} ot_avs_adjust;

/*
 * Specification of fine tuning.
 * fine_tuning_en: the enable of fine tuning function.
 * adjust:         the adjustment for each camera.
 */
typedef struct {
    td_bool fine_tuning_en;
    ot_avs_adjust adjust[OT_AVS_MAX_CAMERA_NUM];
} ot_avs_fine_tuning;

/*
 * the input  of lut generate function.
 * type:            input calibration file type
 * file_input_addr: memory for saving input calibration file, the max memory size is 10KB;
 * mask:            the input parameters which define the mask
 * stitch_distance: the radius of the sphere used to create points to sample from.
 *                  stitching will be performed correctly at this distance.  (ignored if calibration from pt_gui.)
 * lut_accuracy:    the accuracy of the lookup table. valid values are 0 or 1.
 * fine_tuning_cfg: pitch/yaw/roll/xoffset/yoffset for each camera
 */
typedef struct {
    ot_avs_type type;
    td_u64 file_input_addr;
    ot_avs_lut_mask *mask;
    td_float stitch_distance;
    ot_avs_lut_accuracy lut_accuracy;
    ot_avs_fine_tuning fine_tuning_cfg;
} ot_avs_lut_generate_input;

#ifdef __cplusplus
}
#endif

#endif /* ot_mpi_avs_lut_generate_H */

