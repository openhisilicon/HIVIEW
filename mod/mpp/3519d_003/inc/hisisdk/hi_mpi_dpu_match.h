/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */

#ifndef HI_MPI_DPU_MATCH_H
#define HI_MPI_DPU_MATCH_H

#include "hi_common_dpu_match.h"

#ifdef __cplusplus
extern "C" {
#endif

hi_s32 hi_mpi_dpu_match_get_assist_buf_size(hi_u16 disp_num, hi_u32 dst_height, hi_u32 *size);

hi_s32 hi_mpi_dpu_match_create_grp(hi_dpu_match_grp match_grp, const hi_dpu_match_grp_attr *grp_attr);

hi_s32 hi_mpi_dpu_match_destroy_grp(hi_dpu_match_grp match_grp);

hi_s32 hi_mpi_dpu_match_set_grp_attr(hi_dpu_match_grp match_grp, const hi_dpu_match_grp_attr *grp_attr);

hi_s32 hi_mpi_dpu_match_get_grp_attr(hi_dpu_match_grp match_grp, hi_dpu_match_grp_attr *grp_attr);

hi_s32 hi_mpi_dpu_match_start_grp(hi_dpu_match_grp match_grp);

hi_s32 hi_mpi_dpu_match_stop_grp(hi_dpu_match_grp match_grp);

hi_s32 hi_mpi_dpu_match_set_chn_attr(hi_dpu_match_grp match_grp, hi_dpu_match_chn match_chn,
    const hi_dpu_match_chn_attr *chn_attr);

hi_s32 hi_mpi_dpu_match_get_chn_attr(hi_dpu_match_grp match_grp, hi_dpu_match_chn match_chn,
    hi_dpu_match_chn_attr *chn_attr);

hi_s32 hi_mpi_dpu_match_enable_chn(hi_dpu_match_grp match_grp, hi_dpu_match_chn match_chn);

hi_s32 hi_mpi_dpu_match_disable_chn(hi_dpu_match_grp match_grp, hi_dpu_match_chn match_chn);

hi_s32 hi_mpi_dpu_match_get_frame(hi_dpu_match_grp match_grp, hi_s32 milli_sec,
    hi_dpu_match_src_frame_info *src_frame_info, hi_dpu_match_dst_frame_info *dst_frame_info);

hi_s32 hi_mpi_dpu_match_release_frame(hi_dpu_match_grp match_grp,
    const hi_dpu_match_src_frame_info *src_frame_info, const hi_dpu_match_dst_frame_info *dst_frame_info);

hi_s32 hi_mpi_dpu_match_send_frame(hi_dpu_match_grp match_grp,
    const hi_dpu_match_src_frame_info *src_frame_info, hi_s32 milli_sec);

#ifdef __cplusplus
}
#endif
#endif /* HI_MPI_DPU_MATCH_H */
