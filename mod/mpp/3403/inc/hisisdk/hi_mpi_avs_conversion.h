/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */

#ifndef __HI_MPI_AVS_CONVERSION_H__
#define __HI_MPI_AVS_CONVERSION_H__

#include "hi_type.h"

#ifdef __cplusplus
extern "C" {
#endif

#define AVS_LUT_SIZE          0x400000   /* 4MB */
#define AVS_FILE_HEADER_SIZE  0x100  /* bin file header size 256B */

/*
 * avs lut convert function.
 * lut_output: virtual address of output memory for saving each output lookup tables,
 *             the memory size for each one should be 4MB.
 * lut_input:  virtual address of input lut memory.
 */
hi_s32 hi_mpi_avs_conversion(hi_u64 lut_input, hi_u64 lut_output);

#ifdef __cplusplus
}
#endif

#endif /* __HI_MPI_AVS_CONVERSION_H__ */