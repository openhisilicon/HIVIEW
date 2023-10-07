/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */

#ifndef __HI_MPI_AIBNR_H__
#define __HI_MPI_AIBNR_H__

#include "hi_common_aiisp.h"
#include "hi_common_aibnr.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

hi_s32 hi_mpi_aibnr_init(hi_void);
hi_s32 hi_mpi_aibnr_exit(hi_void);
hi_s32 hi_mpi_aibnr_set_thread_attr(const hi_aiisp_thread_attr *thread_attr);
hi_s32 hi_mpi_aibnr_get_thread_attr(hi_aiisp_thread_attr *thread_attr);

hi_s32 hi_mpi_aibnr_enable(hi_s32 pipe);
hi_s32 hi_mpi_aibnr_disable(hi_s32 pipe);
hi_s32 hi_mpi_aibnr_set_attr(hi_s32 pipe, const hi_aibnr_attr *attr);
hi_s32 hi_mpi_aibnr_get_attr(hi_s32 pipe, hi_aibnr_attr *attr);
hi_s32 hi_mpi_aibnr_set_cfg(hi_s32 pipe, const hi_aibnr_cfg *cfg);
hi_s32 hi_mpi_aibnr_get_cfg(hi_s32 pipe, hi_aibnr_cfg *cfg);

hi_s32 hi_mpi_aibnr_load_model(const hi_aibnr_model *model, hi_s32 *model_id);
hi_s32 hi_mpi_aibnr_unload_model(hi_s32 model_id);

hi_s32 hi_mpi_aibnr_get_input_frame(hi_s32 pipe, hi_video_frame_info *frame, hi_s32 milli_sec);
hi_s32 hi_mpi_aibnr_release_input_frame(hi_s32 pipe, hi_video_frame_info *frame);
hi_s32 hi_mpi_aibnr_get_output_frame(hi_s32 pipe, hi_video_frame_info *frame, hi_s32 milli_sec);
hi_s32 hi_mpi_aibnr_release_output_frame(hi_s32 pipe, hi_video_frame_info *frame);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __HI_MPI_AIBNR_H__ */
