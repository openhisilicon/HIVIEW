/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */

#ifndef __HI_MPI_KLAD_H__
#define __HI_MPI_KLAD_H__

#include "hi_common_klad.h"

#ifdef __cplusplus
extern "C" {
#endif

hi_s32 hi_mpi_klad_init(hi_void);

hi_s32 hi_mpi_klad_deinit(hi_void);

hi_s32 hi_mpi_klad_create(hi_handle *klad);

hi_s32 hi_mpi_klad_destroy(hi_handle klad);

hi_s32 hi_mpi_klad_attach(hi_handle klad, hi_handle target);

hi_s32 hi_mpi_klad_detach(hi_handle klad, hi_handle target);

hi_s32 hi_mpi_klad_set_attr(hi_handle klad, const hi_klad_attr *attr);

hi_s32 hi_mpi_klad_get_attr(hi_handle klad, hi_klad_attr *attr);

hi_s32 hi_mpi_klad_set_session_key(hi_handle klad, const hi_klad_session_key *key);

hi_s32 hi_mpi_klad_set_content_key(hi_handle klad, const hi_klad_content_key *key);

hi_s32 hi_mpi_klad_set_clear_key(hi_handle klad, const hi_klad_clear_key *key);

#ifdef __cplusplus
}
#endif

#endif /* __HI_MPI_KLAD_H__ */
