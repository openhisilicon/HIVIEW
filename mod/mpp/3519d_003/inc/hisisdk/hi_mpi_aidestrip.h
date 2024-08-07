/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */

#ifndef __HI_MPI_AIDESTRIP_H__
#define __HI_MPI_AIDESTRIP_H__

#include "hi_common_aiisp.h"
#include "hi_common_aidestrip.h"
#include "hi_common_vb.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

hi_s32 hi_mpi_aidestrip_init(hi_void);
hi_s32 hi_mpi_aidestrip_exit(hi_void);

hi_s32 hi_mpi_aidestrip_enable(hi_s32 pipe);
hi_s32 hi_mpi_aidestrip_disable(hi_s32 pipe);
hi_s32 hi_mpi_aidestrip_set_attr(hi_s32 pipe, const hi_aidestrip_attr *attr);
hi_s32 hi_mpi_aidestrip_get_attr(hi_s32 pipe, hi_aidestrip_attr *attr);
hi_s32 hi_mpi_aidestrip_set_cfg(hi_s32 pipe, const hi_aidestrip_cfg *cfg);
hi_s32 hi_mpi_aidestrip_get_cfg(hi_s32 pipe, hi_aidestrip_cfg *cfg);

hi_s32 hi_mpi_aidestrip_load_model(const hi_aidestrip_model *model, hi_s32 *model_id);
hi_s32 hi_mpi_aidestrip_unload_model(hi_s32 model_id);

hi_s32 hi_mpi_aidestrip_send_frame(hi_s32 pipe, const hi_video_frame_info *frame, hi_s32 milli_sec);
hi_s32 hi_mpi_aidestrip_get_frame(hi_s32 pipe, hi_video_frame_info *frame, hi_s32 milli_sec);
hi_s32 hi_mpi_aidestrip_release_frame(hi_s32 pipe, const hi_video_frame_info *frame);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __HI_MPI_AIDESTRIP_H__ */
