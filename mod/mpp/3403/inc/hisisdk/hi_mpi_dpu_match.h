/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */

#ifndef __HI_MPI_DPU_MATCH_H__
#define __HI_MPI_DPU_MATCH_H__

#include "hi_common_dpu_match.h"

#ifdef __cplusplus
extern "C" {
#endif

hi_s32 hi_mpi_dpu_match_get_assist_buf_size(hi_u16 disparity_num, hi_u32 dst_height, hi_u32 *size);
hi_s32 hi_mpi_dpu_match_create_grp(hi_dpu_match_grp match_grp,
    const hi_dpu_match_grp_attr *grp_attr);
hi_s32 hi_mpi_dpu_match_destroy_grp(hi_dpu_match_grp match_grp);
hi_s32 hi_mpi_dpu_match_set_grp_attr(hi_dpu_match_grp match_grp,
    const hi_dpu_match_grp_attr *grp_attr);
hi_s32 hi_mpi_dpu_match_get_grp_attr(hi_dpu_match_grp match_grp, hi_dpu_match_grp_attr *grp_attr);
hi_s32 hi_mpi_dpu_match_set_grp_cost_param(hi_dpu_match_grp match_grp,
    const hi_dpu_match_cost_param *cost_param);
hi_s32 hi_mpi_dpu_match_get_grp_cost_param(hi_dpu_match_grp match_grp,
    hi_dpu_match_cost_param *cost_param);
hi_s32 hi_mpi_dpu_match_start_grp(hi_dpu_match_grp match_grp);
hi_s32 hi_mpi_dpu_match_stop_grp(hi_dpu_match_grp match_grp);

hi_s32 hi_mpi_dpu_match_set_chn_attr(hi_dpu_match_grp match_grp, hi_dpu_match_chn match_chn,
    const hi_dpu_match_chn_attr *chn_attr);
hi_s32 hi_mpi_dpu_match_get_chn_attr(hi_dpu_match_grp match_grp, hi_dpu_match_chn match_chn,
    hi_dpu_match_chn_attr *chn_attr);
hi_s32 hi_mpi_dpu_match_enable_chn(hi_dpu_match_grp match_grp, hi_dpu_match_chn match_chn);
hi_s32 hi_mpi_dpu_match_disable_chn(hi_dpu_match_grp match_grp, hi_dpu_match_chn match_chn);

hi_s32 hi_mpi_dpu_match_get_frame(hi_dpu_match_grp match_grp, hi_s32 milli_sec, hi_dpu_match_frame_info *src_frame_info,
    hi_video_frame_info *dst_frame);
hi_s32 hi_mpi_dpu_match_release_frame(hi_dpu_match_grp match_grp, const hi_dpu_match_frame_info *src_frame_info,
    const hi_video_frame_info *dst_frame);
hi_s32 hi_mpi_dpu_match_send_frame(hi_dpu_match_grp match_grp, const hi_dpu_match_frame_info *src_frame_info,
    hi_s32 milli_sec);

hi_s32 hi_mpi_dpu_match_set_grp_param(hi_dpu_match_grp match_grp, const hi_dpu_match_param *param);
hi_s32 hi_mpi_dpu_match_get_grp_param(hi_dpu_match_grp match_grp, hi_dpu_match_param *param);

#ifdef __cplusplus
}
#endif
#endif /* __HI_MPI_DPU_MATCH_H__ */
