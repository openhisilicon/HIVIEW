/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */

#ifndef __HI_MPI_MCF_CALIBRATION_H__
#define __HI_MPI_MCF_CALIBRATION_H__

#include "hi_common_mcf_calibration.h"


#ifdef __cplusplus
extern "C" {
#endif

hi_s32 hi_mpi_mcf_calibration(const hi_video_frame *pic_in_match, const hi_video_frame *pic_in_refer,
    hi_mcf_calibration_mode mode, hi_mcf_feature_info *feature_info, hi_mcf_calibration *calibration_info);

#ifdef __cplusplus
}
#endif
#endif /* __HI_MPI_MCF_CALIBRATION_H__ */
