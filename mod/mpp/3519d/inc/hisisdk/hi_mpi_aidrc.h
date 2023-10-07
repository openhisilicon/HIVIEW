/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */

#ifndef __HI_MPI_AIDRC_H__
#define __HI_MPI_AIDRC_H__

#include "hi_common_aiisp.h"
#include "hi_common_aidrc.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

hi_s32 hi_mpi_aidrc_init(hi_void);
hi_s32 hi_mpi_aidrc_exit(hi_void);
hi_s32 hi_mpi_aidrc_set_thread_attr(const hi_aiisp_thread_attr *thread_attr);
hi_s32 hi_mpi_aidrc_get_thread_attr(hi_aiisp_thread_attr *thread_attr);

hi_s32 hi_mpi_aidrc_enable(hi_s32 pipe);
hi_s32 hi_mpi_aidrc_disable(hi_s32 pipe);
hi_s32 hi_mpi_aidrc_set_attr(hi_s32 pipe, const hi_aidrc_attr *attr);
hi_s32 hi_mpi_aidrc_get_attr(hi_s32 pipe, hi_aidrc_attr *attr);
hi_s32 hi_mpi_aidrc_set_cfg(hi_s32 pipe, const hi_aidrc_cfg *cfg);
hi_s32 hi_mpi_aidrc_get_cfg(hi_s32 pipe, hi_aidrc_cfg *cfg);

hi_s32 hi_mpi_aidrc_load_model(const hi_aidrc_model *model, hi_s32 *model_id);
hi_s32 hi_mpi_aidrc_unload_model(hi_s32 model_id);

hi_s32 hi_mpi_aidrc_set_smooth(hi_s32 pipe, const hi_aidrc_smooth *smooth);
hi_s32 hi_mpi_aidrc_get_smooth(hi_s32 pipe, hi_aidrc_smooth *smooth);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __HI_MPI_AIDRC_H__ */
