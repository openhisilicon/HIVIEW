/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */

#ifndef __HI_MPI_GDC_H__
#define __HI_MPI_GDC_H__

#include "hi_common.h"
#include "hi_common_video.h"
#include "hi_common_gdc.h"


#ifdef __cplusplus
extern "C" {
#endif

/* Description: Begin a gdc job,then add task into the job,gdc will finish all the task in the job. */
hi_s32 hi_mpi_gdc_begin_job(hi_gdc_handle *handle);

/* Description: End a job,all tasks in the job will be submmitted to gdc */
hi_s32 hi_mpi_gdc_end_job(hi_gdc_handle handle);

/* Description: Cancel a job ,then all tasks in the job will not be submmitted to gdc */
hi_s32 hi_mpi_gdc_cancel_job(hi_gdc_handle handle);

/* Description: Add a task to a gdc job */
hi_s32 hi_mpi_gdc_add_correction_task(hi_gdc_handle handle, const hi_gdc_task_attr *task,
    const hi_fisheye_attr *fisheye_attr);

/* Description: Add a task to a gdc job */
hi_s32 hi_mpi_gdc_add_correction_ex_task(hi_gdc_handle h_handle, const hi_gdc_task_attr *task,
    const hi_fisheye_attr_ex *fisheye_attrr_ex, hi_bool check_mode);

/* Description: Set Config */
hi_s32 hi_mpi_gdc_set_cfg(hi_gdc_handle handle, const hi_gdc_fisheye_job_cfg *job_cfg);

/* Description: Set Config */
hi_s32 hi_mpi_gdc_add_pmf_task(hi_gdc_handle handle, const hi_gdc_task_attr *task,
    const hi_gdc_pmf_attr *gdc_pmf_attr);

hi_s32 hi_mpi_gdc_fisheye_pos_query_dst_to_src(const hi_gdc_fisheye_point_query_attr *fisheye_point_query_attr,
    const hi_video_frame_info *video_info, const hi_point *dst_point, hi_point *src_point);

#ifdef __cplusplus
}
#endif
#endif /* __HI_MPI_GDC_H__ */
