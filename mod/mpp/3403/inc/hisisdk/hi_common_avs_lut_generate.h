/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */

#ifndef __HI_COMMON_AVS_LUT_GENERATE_H__
#define __HI_COMMON_AVS_LUT_GENERATE_H__

#include "hi_type.h"
#include "hi_common_avs.h"
#include "ot_common_avs_lut_generate.h"


#ifdef __cplusplus
extern "C" {
#endif

#define HI_AVS_MAX_CAMERA_NUM OT_AVS_MAX_CAMERA_NUM
#define HI_AVS_LUT_SIZE OT_AVS_LUT_SIZE
#define HI_AVS_CALIBRATION_FILE_LEN OT_AVS_CALIBRATION_FILE_LEN
#define HI_AVS_MATRIX_SIZE OT_AVS_MATRIX_SIZE
#define HI_AVS_STATUS_EOF OT_AVS_STATUS_EOF
#define HI_AVS_STATUS_OK OT_AVS_STATUS_OK
#define HI_AVS_STATUS_FILE_READ_ERROR OT_AVS_STATUS_FILE_READ_ERROR
#define HI_AVS_STATUS_FILE_WRITE_ERROR OT_AVS_STATUS_FILE_WRITE_ERROR
#define HI_AVS_STATUS_FILE_INVALID OT_AVS_STATUS_FILE_INVALID
#define HI_AVS_STATUS_ALLOC_FAILED OT_AVS_STATUS_ALLOC_FAILED
#define HI_AVS_STATUS_INVALID_PARAM OT_AVS_STATUS_INVALID_PARAM
#define HI_AVS_STATUS_INVALID_INPUT_POINTER OT_AVS_STATUS_INVALID_INPUT_POINTER
#define HI_AVS_STATUS_INVALID_MASK_POINTER OT_AVS_STATUS_INVALID_MASK_POINTER
#define HI_AVS_STATUS_BUTT OT_AVS_STATUS_BUTT
typedef ot_avs_status hi_avs_status;
#define HI_AVS_TYPE_AVSP OT_AVS_TYPE_AVSP
#define HI_AVS_TYPE_PTGUI OT_AVS_TYPE_PTGUI
#define HI_AVS_TYPE_HUGIN OT_AVS_TYPE_HUGIN
#define HI_AVS_TYPE_BUTT OT_AVS_TYPE_BUTT
typedef ot_avs_type hi_avs_type;
#define HI_AVS_MASK_SHAPE_RECT OT_AVS_MASK_SHAPE_RECT
#define HI_AVS_MASK_SHAPE_ELLIPSE OT_AVS_MASK_SHAPE_ELLIPSE
#define HI_AVS_MASK_SHAPE_BUTT OT_AVS_MASK_SHAPE_BUTT
typedef ot_avs_mask_shape hi_avs_mask_shape;
typedef ot_avs_mask_define hi_avs_mask_define;
typedef ot_avs_lut_mask hi_avs_lut_mask;
typedef ot_avs_adjust hi_avs_adjust;
typedef ot_avs_fine_tuning hi_avs_fine_tuning;
typedef ot_avs_lut_generate_input hi_avs_lut_generate_input;

#ifdef __cplusplus
}
#endif
#endif /* __HI_COMMON_AVS_LUT_GENERATE_H__ */
