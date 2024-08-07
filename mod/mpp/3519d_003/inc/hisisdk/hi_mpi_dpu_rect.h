/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */

#ifndef HI_MPI_DPU_RECT_H
#define HI_MPI_DPU_RECT_H

#include "hi_common_dpu_rect.h"

#ifdef __cplusplus
extern "C" {
#endif

hi_s32 hi_mpi_dpu_rect_load_lut(const hi_dpu_rect_mem_info *lut_mem, hi_dpu_rect_lut_id *rect_lut_id);
hi_s32 hi_mpi_dpu_rect_unload_lut(hi_dpu_rect_lut_id rect_lut_id);

hi_s32 hi_mpi_dpu_rect_create_grp(hi_dpu_rect_grp rect_grp, const hi_dpu_rect_grp_attr *grp_attr);
hi_s32 hi_mpi_dpu_rect_destroy_grp(hi_dpu_rect_grp rect_grp);

hi_s32 hi_mpi_dpu_rect_set_grp_attr(hi_dpu_rect_grp rect_grp, const hi_dpu_rect_grp_attr *grp_attr);
hi_s32 hi_mpi_dpu_rect_get_grp_attr(hi_dpu_rect_grp rect_grp, hi_dpu_rect_grp_attr *grp_attr);

hi_s32 hi_mpi_dpu_rect_set_chn_attr(hi_dpu_rect_grp rect_grp, hi_dpu_rect_chn rect_chn,
    const hi_dpu_rect_chn_attr *chn_attr);
hi_s32 hi_mpi_dpu_rect_get_chn_attr(hi_dpu_rect_grp rect_grp, hi_dpu_rect_chn rect_chn,
    hi_dpu_rect_chn_attr *chn_attr);

hi_s32 hi_mpi_dpu_rect_enable_chn(hi_dpu_rect_grp rect_grp, hi_dpu_rect_chn rect_chn);
hi_s32 hi_mpi_dpu_rect_disable_chn(hi_dpu_rect_grp rect_grp, hi_dpu_rect_chn rect_chn);

hi_s32 hi_mpi_dpu_rect_start_grp(hi_dpu_rect_grp rect_grp);
hi_s32 hi_mpi_dpu_rect_stop_grp(hi_dpu_rect_grp rect_grp);

hi_s32 hi_mpi_dpu_rect_get_frame(hi_dpu_rect_grp rect_grp, hi_s32 milli_sec, hi_dpu_rect_frame_info *rect_frame_info);

hi_s32 hi_mpi_dpu_rect_release_frame(hi_dpu_rect_grp rect_grp, const hi_dpu_rect_frame_info *rect_frame_info);

hi_s32 hi_mpi_dpu_rect_send_frame(hi_dpu_rect_grp rect_grp,
    const hi_video_frame_info *left_frame, const hi_video_frame_info *right_frame, hi_s32 milli_sec);

#ifdef __cplusplus
}
#endif
#endif /* HI_MPI_DPU_RECT_H */
