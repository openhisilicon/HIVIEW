/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */

#ifndef __HI_MPI_PHOTO_H__
#define __HI_MPI_PHOTO_H__

#include "hi_common.h"
#include "hi_common_photo.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

hi_s32 hi_mpi_photo_alg_init(hi_photo_alg_type alg_type, const hi_photo_alg_init *photo_init);
hi_s32 hi_mpi_photo_alg_deinit(hi_photo_alg_type alg_type);

hi_s32 hi_mpi_photo_alg_process(hi_photo_alg_type alg_type, const hi_photo_alg_attr *photo_attr);

hi_s32 hi_mpi_photo_set_alg_coef(hi_photo_alg_type alg_type, const hi_photo_alg_coef *alg_coef);
hi_s32 hi_mpi_photo_get_alg_coef(hi_photo_alg_type alg_type, hi_photo_alg_coef *alg_coef);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __HI_MPI_PHOTO_H__ */
