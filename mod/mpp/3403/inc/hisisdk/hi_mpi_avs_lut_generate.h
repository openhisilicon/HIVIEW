/*
  Copyright (c), 2001-2021, Shenshu Tech. Co., Ltd.
 */

#ifndef __HI_MPI_AVS_LUT_GENERATE_H__
#define __HI_MPI_AVS_LUT_GENERATE_H__

#include "hi_type.h"
#include "hi_common_avs_lut_generate.h"


#ifdef __cplusplus
extern "C" {
#endif

/*
 * generates mesh(LUT) files.
 * lut_input: the input of lut generates.
 * lut_output_addr: memory for saving each output lookup tables, the memory size for each one should be 4MB.
 */
hi_avs_status hi_mpi_avs_lut_generate(hi_avs_lut_generate_input *lut_input,
    hi_u64 lut_output_addr[HI_AVS_MAX_CAMERA_NUM]);


#ifdef __cplusplus
}
#endif
#endif /* __HI_MPI_AVS_LUT_GENERATE_H__ */
