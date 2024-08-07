/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */

#ifndef HI_MPI_GDC_H
#define HI_MPI_GDC_H

#include "hi_common.h"
#include "hi_common_video.h"
#include "hi_common_gdc.h"

#ifdef __cplusplus
extern "C" {
#endif

hi_s32 hi_mpi_gdc_begin_job(hi_gdc_handle *handle);

hi_s32 hi_mpi_gdc_end_job(hi_gdc_handle handle);

hi_s32 hi_mpi_gdc_cancel_job(hi_gdc_handle handle);

hi_s32 hi_mpi_gdc_add_correction_task(hi_gdc_handle handle, const hi_gdc_task_attr *task,
    const hi_fisheye_attr *fisheye_attr);

hi_s32 hi_mpi_gdc_add_correction_ex_task(hi_gdc_handle handle, const hi_gdc_task_attr *task,
    const hi_fisheye_attr_ex *fisheye_attr_ex, hi_bool check_mode);

hi_s32 hi_mpi_gdc_set_cfg(hi_gdc_handle handle, const hi_gdc_fisheye_job_cfg *job_cfg);

hi_s32 hi_mpi_gdc_add_pmf_task(hi_gdc_handle handle, const hi_gdc_task_attr *task,
    const hi_gdc_pmf_attr *gdc_pmf_attr);

hi_s32 hi_mpi_gdc_fisheye_pos_query_dst_to_src(const hi_gdc_fisheye_point_query_attr *fisheye_point_query_attr,
    const hi_gdc_query_pic_info *video_info, const hi_point *dst_point, hi_point *src_point);

hi_s32 hi_mpi_gdc_add_lut_task(hi_gdc_handle handle, const hi_gdc_task_attr *task,
    const hi_lut_attr *lut_attr);

#ifdef __cplusplus
}
#endif
#endif /* HI_MPI_GDC_H */
