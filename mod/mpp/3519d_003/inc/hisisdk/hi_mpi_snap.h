/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */

#ifndef HI_MPI_SNAP_H
#define HI_MPI_SNAP_H

#include "hi_common_snap.h"

#ifdef __cplusplus
extern "C" {
#endif

hi_s32 hi_mpi_snap_set_pipe_attr(hi_vi_pipe vi_pipe, const hi_snap_attr *snap_attr);
hi_s32 hi_mpi_snap_get_pipe_attr(hi_vi_pipe vi_pipe, hi_snap_attr *snap_attr);

hi_s32 hi_mpi_snap_enable_pipe(hi_vi_pipe vi_pipe);
hi_s32 hi_mpi_snap_disable_pipe(hi_vi_pipe vi_pipe);

hi_s32 hi_mpi_snap_trigger_pipe(hi_vi_pipe vi_pipe);

#ifdef __cplusplus
}
#endif
#endif /* HI_MPI_SNAP_H */
