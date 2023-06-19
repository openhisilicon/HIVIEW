/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */

#ifndef __HI_MPI_MCF_VI_H__
#define __HI_MPI_MCF_VI_H__

#include "hi_common_mcf_vi.h"
#include "hi_common.h"


#ifdef __cplusplus
extern "C" {
#endif


hi_s32 hi_mpi_mcf_set_vi_attr(hi_mcf_id mcf_id, const hi_mcf_vi_attr *mcf_vi_attr);
hi_s32 hi_mpi_mcf_get_vi_attr(hi_mcf_id mcf_id, hi_mcf_vi_attr *mcf_vi_attr);

#ifdef __cplusplus
}
#endif
#endif /* __HI_MPI_MCF_VI_H__ */
