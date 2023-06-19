/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */

#ifndef __HI_MPI_AVS_POS_QUERY_H__
#define __HI_MPI_AVS_POS_QUERY_H__

#include "hi_common_avs_pos_query.h"


#ifdef __cplusplus
extern "C" {
#endif

/*
 * generates the lookup table about the position between output image and source image.
 * avs_config:     output image config
 * mesh_vir_addr:  the address of mesh data to save.
 */
hi_s32 hi_mpi_avs_pos_mesh_generate(const hi_avs_pos_cfg *cfg, const hi_u64 mesh_addr[HI_AVS_MAX_INPUT_NUM]);

/*
 * query the position in source image space from the output image space
 * dst_size:    the resolution of destination image;
 * window_size: the windows size of position mesh data, should be same as generating the position mesh.
 * mesh_addr:   the virtual address of position mesh data, the memory size should be same as the mesh file.
 * dst_point:   the input position in destination image space.
 * src_point:   the output position in source image space.
 */
hi_s32 hi_mpi_avs_pos_query_dst_to_src(const hi_size *dst_size, hi_u32 window_size, hi_u64 mesh_addr,
    const hi_point *dst_point, hi_point *src_point);

/*
 * query the position in output stitch image space from the source image space
 * src_size:    the resolution of source image;
 * window_size: the windows size of position mesh data, should be same as generating the position mesh.
 * mesh_addr:   the virtual address of position mesh data, the memory size should be same as the mesh file.
 * src_point:   the input position in source image space.
 * dst_point:   the output position in destination image space.
 */
hi_s32 hi_mpi_avs_pos_query_src_to_dst(const hi_size *src_size, hi_u32 window_size, hi_u64 mesh_addr,
    const hi_point *src_point, hi_point *dst_point);

#ifdef __cplusplus
}
#endif
#endif /* __HI_MPI_AVS_POS_QUERY_H__ */
