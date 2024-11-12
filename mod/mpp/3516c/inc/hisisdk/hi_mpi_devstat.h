/*
  Copyright (c), 2001-2024, Shenshu Tech. Co., Ltd.
 */

#ifndef HI_MPI_DEVSTAT_H
#define HI_MPI_DEVSTAT_H

#include "hi_common_devstat.h"

#ifdef __cplusplus
extern "C" {
#endif

hi_s32 hi_mpi_devstat_query_device_status(hi_devstat_device_status *device_status);

#ifdef __cplusplus
}
#endif

#endif /* HI_MPI_DEVSTAT_H */