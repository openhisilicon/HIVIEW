/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */

#ifndef HI_MPI_VGS_H
#define HI_MPI_VGS_H

#include "hi_common.h"
#include "hi_common_video.h"
#include "hi_common_vgs.h"

#ifdef __cplusplus
extern "C" {
#endif

hi_s32 hi_mpi_vgs_begin_job(hi_vgs_handle *handle);

hi_s32 hi_mpi_vgs_end_job(hi_vgs_handle handle);

hi_s32 hi_mpi_vgs_cancel_job(hi_vgs_handle handle);

hi_s32 hi_mpi_vgs_add_scale_task(hi_vgs_handle handle, const hi_vgs_task_attr *task, hi_vgs_scale_coef_mode mode);

hi_s32 hi_mpi_vgs_add_draw_line_task(hi_vgs_handle handle, const hi_vgs_task_attr *task,
    const hi_vgs_line line[], hi_u32 array_size);

hi_s32 hi_mpi_vgs_add_cover_task(hi_vgs_handle handle, const hi_vgs_task_attr *task,
    const hi_cover cover[], hi_u32 array_size);

hi_s32 hi_mpi_vgs_add_mosaic_task(hi_vgs_handle handle, const hi_vgs_task_attr *task,
    const hi_mosaic mosaic[], hi_u32 array_size);

hi_s32 hi_mpi_vgs_add_osd_task(hi_vgs_handle handle, const hi_vgs_task_attr *task,
    const hi_vgs_osd osd[], hi_u32 array_size);

hi_s32 hi_mpi_vgs_add_rotation_task(hi_vgs_handle handle, const hi_vgs_task_attr *task, hi_rotation angle);

hi_s32 hi_mpi_vgs_add_luma_task(hi_vgs_handle handle, const hi_vgs_task_attr *task,
    const hi_rect luma_rect[], hi_u32 array_size, hi_u64 luma_data[]);

hi_s32 hi_mpi_vgs_add_corner_rect_task(hi_vgs_handle handle, const hi_vgs_task_attr *task,
    const hi_corner_rect corner_rect[], const hi_corner_rect_attr *corner_rect_attr, hi_u32 array_size);

hi_s32 hi_mpi_vgs_add_stitch_task(hi_vgs_handle handle, const hi_vgs_stitch_task_attr *task, hi_u32 pic_num);

hi_s32 hi_mpi_vgs_add_online_task(hi_vgs_handle handle, const hi_vgs_task_attr *task, const hi_vgs_online *online);

#ifdef __cplusplus
}
#endif
#endif /* HI_MPI_VGS_H */
