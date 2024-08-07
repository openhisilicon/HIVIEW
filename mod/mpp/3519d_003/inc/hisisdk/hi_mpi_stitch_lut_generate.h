/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */

#ifndef HI_MPI_STITCH_LUT_GENERATE_H
#define HI_MPI_STITCH_LUT_GENERATE_H

#include "hi_type.h"
#include "hi_common_stitch_lut_generate.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ot_mpi_stitch_lut_generate: output lut for gdc tranform and stitch param
*       stitch_cfg: input stitch tuning info
*         size_cfg: input param for lut generate, crop area(x coordinates) is needed
*        out_param: output lut and params
*   mesh_file_name: output lut file name address
*/
hi_s32 hi_mpi_stitch_lut_generate(const hi_stitch_config *stitch_cfg,
    const hi_stitch_img_size_cfg *size_cfg, hi_stitch_out_param *out_param, const hi_char *mesh_file_prefix);

#ifdef __cplusplus
}
#endif
#endif /* HI_MPI_STITCH_LUT_GENERATE_H */
