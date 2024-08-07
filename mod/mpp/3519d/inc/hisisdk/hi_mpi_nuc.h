/*
  Copyright (c), 2001-2024, Shenshu Tech. Co., Ltd.
 */

#ifndef __HI_MPI_NUC_H__
#define __HI_MPI_NUC_H__

#include "hi_common_aiisp.h"
#include "hi_common_nuc.h"
#include "hi_common_vb.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

hi_s32 hi_mpi_nuc_init(hi_void);
hi_s32 hi_mpi_nuc_exit(hi_void);

hi_s32 hi_mpi_nuc_enable(hi_s32 pipe);
hi_s32 hi_mpi_nuc_disable(hi_s32 pipe);
hi_s32 hi_mpi_nuc_set_cfg(hi_s32 pipe, const hi_nuc_cfg *cfg);
hi_s32 hi_mpi_nuc_get_cfg(hi_s32 pipe, hi_nuc_cfg *cfg);

hi_s32 hi_mpi_nuc_load_model(const hi_nuc_model *model, hi_s32 *model_id);
hi_s32 hi_mpi_nuc_unload_model(hi_s32 model_id);

hi_s32 hi_mpi_nuc_send_frame(hi_s32 pipe, const hi_video_frame_info *frame,
    const hi_video_frame_info *fpn_frame, hi_s32 milli_sec);
hi_s32 hi_mpi_nuc_get_frame(hi_s32 pipe, hi_video_frame_info *frame, hi_s32 milli_sec);
hi_s32 hi_mpi_nuc_release_frame(hi_s32 pipe, const hi_video_frame_info *frame);

hi_s32 hi_mpi_nuc_query_status(hi_s32 pipe, hi_bool *enable);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __HI_MPI_NUC_H__ */
