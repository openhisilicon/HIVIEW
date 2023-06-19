/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */

#ifndef __HI_MPI_HNR_H__
#define __HI_MPI_HNR_H__

#include "hi_common_vi.h"
#include "hi_common_hnr.h"
#include "hi_common_vb.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

hi_s32 hi_mpi_hnr_init(hi_void);
hi_void hi_mpi_hnr_exit(hi_void);

hi_s32 hi_mpi_hnr_load_cfg(const hi_hnr_cfg *cfg, hi_s32 *cfg_id);
hi_s32 hi_mpi_hnr_unload_cfg(hi_s32 cfg_id);

hi_s32 hi_mpi_hnr_set_alg_cfg(hi_vi_pipe vi_pipe, const hi_hnr_alg_cfg *cfg);
hi_s32 hi_mpi_hnr_get_alg_cfg(hi_vi_pipe vi_pipe, hi_hnr_alg_cfg *cfg);

hi_s32 hi_mpi_hnr_enable(hi_vi_pipe vi_pipe);
hi_s32 hi_mpi_hnr_disable(hi_vi_pipe vi_pipe);

hi_s32 hi_mpi_hnr_set_attr(hi_vi_pipe vi_pipe, const hi_hnr_attr *attr);
hi_s32 hi_mpi_hnr_get_attr(hi_vi_pipe vi_pipe, hi_hnr_attr *attr);

hi_s32 hi_mpi_hnr_set_input_depth(hi_vi_pipe vi_pipe, hi_u32 depth);

hi_s32 hi_mpi_hnr_set_thread_attr(const hi_hnr_thread_attr *thread_attr);
hi_s32 hi_mpi_hnr_get_thread_attr(hi_hnr_thread_attr *thread_attr);

hi_s32 hi_mpi_hnr_attach_out_vb_pool(hi_vi_pipe vi_pipe, hi_vb_pool vb_pool);
hi_s32 hi_mpi_hnr_detach_out_vb_pool(hi_vi_pipe vi_pipe);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __HI_MPI_HNR_H__ */
