/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
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

/*
 * get rotation matrix for each camera.
 * file_input_addr: the memory address of input avs calibration file;
 * rotation_matrix: the output rotation matrix for each camera.
 */
hi_avs_status hi_mpi_avs_get_rotation_matrix(const hi_u64 file_input_addr,
    hi_double rotation_matrix[HI_AVS_MAX_CAMERA_NUM][HI_AVS_MATRIX_SIZE]);

#ifdef __cplusplus
}
#endif
#endif /* __HI_MPI_AVS_LUT_GENERATE_H__ */
