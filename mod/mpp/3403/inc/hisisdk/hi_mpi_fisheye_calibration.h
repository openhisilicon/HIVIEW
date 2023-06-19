/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */

#ifndef __HI_MPI_FISHEYE_CALIBRATION_H__
#define __HI_MPI_FISHEYE_CALIBRATION_H__

#include "hi_common_video.h"
#include "hi_common_fisheye_calibration.h"


#ifdef __cplusplus
extern "C" {
#endif

hi_s32 hi_mpi_fisheye_calibration_compute(const hi_video_frame *frame_in,
    hi_fisheye_calibration_level level, hi_fisheye_calibration_output *calibration_out);

hi_s32 hi_mpi_fisheye_calibration_mark(const hi_video_frame *frame_in, const hi_video_frame *frame_out,
    const hi_fisheye_calibration_result *calibration_result);

#ifdef __cplusplus
}
#endif
#endif /* __HI_MPI_FISHEYE_CALIBRATION_H__ */
