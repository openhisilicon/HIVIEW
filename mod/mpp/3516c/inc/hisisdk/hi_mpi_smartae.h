/*
  Copyright (c), 2001-2024, Shenshu Tech. Co., Ltd.
 */

#ifndef HI_MPI_SMARTAE_H
#define HI_MPI_SMARTAE_H

#include "hi_common_video.h"
#include "hi_common_smartae.h"

hi_s32 hi_mpi_smartae_update_roi_info(hi_vi_pipe vi_pipe, const hi_video_frame_info *frame_info,
    hi_smartae_roi_info *smartinfo);

#endif /* HI_MPI_SMARTAE_H */
