/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */

#ifndef HI_MPI_SYS_BIND_H
#define HI_MPI_SYS_BIND_H

#include "hi_common.h"
#include "hi_common_sys_bind.h"

#ifdef __cplusplus
extern "C" {
#endif

hi_s32 hi_mpi_sys_bind(const hi_mpp_chn *src_chn, const hi_mpp_chn *dst_chn);
hi_s32 hi_mpi_sys_unbind(const hi_mpp_chn *src_chn, const hi_mpp_chn *dst_chn);

hi_s32 hi_mpi_sys_get_bind_by_dst(const hi_mpp_chn *dst_chn, hi_mpp_chn *src_chn);
hi_s32 hi_mpi_sys_get_bind_by_src(const hi_mpp_chn *src_chn, hi_mpp_bind_dst *bind_dst);

#ifdef __cplusplus
}
#endif
#endif /* HI_MPI_SYS_BIND_H */
