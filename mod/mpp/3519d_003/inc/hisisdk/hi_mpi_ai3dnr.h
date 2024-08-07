/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */

#ifndef __HI_MPI_AI3DNR_H__
#define __HI_MPI_AI3DNR_H__

#include "hi_common_aiisp.h"
#include "hi_common_ai3dnr.h"

#ifdef __cplusplus
extern "C" {
#endif

hi_s32 hi_mpi_ai3dnr_init(hi_void);
hi_s32 hi_mpi_ai3dnr_exit(hi_void);
hi_s32 hi_mpi_ai3dnr_set_thread_attr(const hi_aiisp_thread_attr *thread_attr);
hi_s32 hi_mpi_ai3dnr_get_thread_attr(hi_aiisp_thread_attr *thread_attr);

hi_s32 hi_mpi_ai3dnr_enable(hi_s32 pipe);
hi_s32 hi_mpi_ai3dnr_disable(hi_s32 pipe);
hi_s32 hi_mpi_ai3dnr_set_attr(hi_s32 pipe, const hi_ai3dnr_attr *attr);
hi_s32 hi_mpi_ai3dnr_get_attr(hi_s32 pipe, hi_ai3dnr_attr *attr);
hi_s32 hi_mpi_ai3dnr_set_cfg(hi_s32 pipe, const hi_ai3dnr_cfg *cfg);
hi_s32 hi_mpi_ai3dnr_get_cfg(hi_s32 pipe, hi_ai3dnr_cfg *cfg);
hi_s32 hi_mpi_ai3dnr_set_smooth(hi_s32 pipe, const hi_ai3dnr_smooth *smooth);
hi_s32 hi_mpi_ai3dnr_get_smooth(hi_s32 pipe, hi_ai3dnr_smooth *smooth);

hi_s32 hi_mpi_ai3dnr_load_model(const hi_ai3dnr_model *model, hi_s32 *model_id);
hi_s32 hi_mpi_ai3dnr_unload_model(hi_s32 model_id);

hi_s32 hi_mpi_ai3dnr_get_input_frame(hi_s32 pipe, hi_video_frame_info *frame, hi_s32 milli_sec);
hi_s32 hi_mpi_ai3dnr_release_input_frame(hi_s32 pipe, hi_video_frame_info *frame);
hi_s32 hi_mpi_ai3dnr_get_output_frame(hi_s32 pipe, hi_video_frame_info *frame, hi_s32 milli_sec);
hi_s32 hi_mpi_ai3dnr_release_output_frame(hi_s32 pipe, hi_video_frame_info *frame);

#ifdef __cplusplus
}
#endif

#endif
