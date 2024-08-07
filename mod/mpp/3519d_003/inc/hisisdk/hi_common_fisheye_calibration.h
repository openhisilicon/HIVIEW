/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */

#ifndef HI_COMMON_FISHEYE_CALIBRATION_H
#define HI_COMMON_FISHEYE_CALIBRATION_H

#include "hi_common_video.h"
#include "hi_errno.h"
#include "ot_common_fisheye_calibration.h"

#ifdef __cplusplus
extern "C" {
#endif

#define HI_ERR_FISHEYE_CALIBRATION_ILLEGAL_PARAM OT_ERR_FISHEYE_CALIBRATION_ILLEGAL_PARAM
#define HI_ERR_FISHEYE_CALIBRATION_NULL_PTR OT_ERR_FISHEYE_CALIBRATION_NULL_PTR
#define HI_ERR_FISHEYE_CALIBRATION_NOT_SUPPORT OT_ERR_FISHEYE_CALIBRATION_NOT_SUPPORT
#define HI_ERR_FISHEYE_CALIBRATION_NOMEM OT_ERR_FISHEYE_CALIBRATION_NOMEM
#define HI_ERR_FISHEYE_CALIBRATION_BUSY OT_ERR_FISHEYE_CALIBRATION_BUSY
#define HI_FISHEYE_CALIBRATION_LEVEL_0 OT_FISHEYE_CALIBRATION_LEVEL_0
#define HI_FISHEYE_CALIBRATION_LEVEL_1 OT_FISHEYE_CALIBRATION_LEVEL_1
#define HI_FISHEYE_CALIBRATION_LEVEL_2 OT_FISHEYE_CALIBRATION_LEVEL_2
#define HI_FISHEYE_CALIBRATION_LEVEL_BUTT OT_FISHEYE_CALIBRATION_LEVEL_BUTT
typedef ot_fisheye_calibration_level hi_fisheye_calibration_level;
typedef ot_fisheye_calibration_result hi_fisheye_calibration_result;
typedef ot_fisheye_calibration_output hi_fisheye_calibration_output;

#ifdef __cplusplus
}
#endif
#endif /* HI_COMMON_FISHEYE_CALIBRATION_H */
