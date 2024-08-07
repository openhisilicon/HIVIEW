/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */

#ifndef HI_MPI_MCF_H
#define HI_MPI_MCF_H

#include "hi_common_video.h"
#include "hi_common_vb.h"
#include "hi_common_mcf.h"

#ifdef __cplusplus
extern "C" {
#endif

hi_s32 hi_mpi_mcf_create_grp(hi_mcf_grp grp, const hi_mcf_grp_attr *grp_attr);
hi_s32 hi_mpi_mcf_destroy_grp(hi_mcf_grp grp);

hi_s32 hi_mpi_mcf_reset_grp(hi_mcf_grp grp);

hi_s32 hi_mpi_mcf_start_grp(hi_mcf_grp grp);
hi_s32 hi_mpi_mcf_stop_grp(hi_mcf_grp grp);

hi_s32 hi_mpi_mcf_set_grp_attr(hi_mcf_grp grp, const hi_mcf_grp_attr *grp_attr);
hi_s32 hi_mpi_mcf_get_grp_attr(hi_mcf_grp grp, hi_mcf_grp_attr *grp_attr);

hi_s32 hi_mpi_mcf_set_grp_crop(hi_mcf_grp grp, const hi_mcf_crop_info *crop_info);
hi_s32 hi_mpi_mcf_get_grp_crop(hi_mcf_grp grp, hi_mcf_crop_info *crop_info);

hi_s32 hi_mpi_mcf_set_alg_param(hi_mcf_grp grp, const hi_mcf_alg_param *alg_param);
hi_s32 hi_mpi_mcf_get_alg_param(hi_mcf_grp grp, hi_mcf_alg_param *alg_param);

hi_s32 hi_mpi_mcf_send_pipe_frame(hi_mcf_grp grp, hi_mcf_pipe mcf_pipe, const hi_video_frame_info *video_frame,
                                  hi_s32 milli_sec);
hi_s32 hi_mpi_mcf_set_chn_attr(hi_mcf_grp grp, hi_mcf_chn chn, const hi_mcf_chn_attr *chn_attr);
hi_s32 hi_mpi_mcf_get_chn_attr(hi_mcf_grp grp, hi_mcf_chn chn, hi_mcf_chn_attr *chn_attr);

hi_s32 hi_mpi_mcf_enable_chn(hi_mcf_grp grp, hi_mcf_chn chn);
hi_s32 hi_mpi_mcf_disable_chn(hi_mcf_grp grp, hi_mcf_chn chn);

hi_s32 hi_mpi_mcf_get_chn_frame(hi_mcf_grp grp, hi_mcf_chn chn, hi_video_frame_info *video_frame, hi_s32 milli_sec);
hi_s32 hi_mpi_mcf_release_chn_frame(hi_mcf_grp grp, hi_mcf_chn chn, const hi_video_frame_info *video_frame);

hi_s32 hi_mpi_mcf_set_chn_low_delay(hi_mcf_grp grp, hi_mcf_chn chn, const hi_low_delay_info *low_delay_info);
hi_s32 hi_mpi_mcf_get_chn_low_delay(hi_mcf_grp grp, hi_mcf_chn chn, hi_low_delay_info *low_delay_info);

hi_s32 hi_mpi_mcf_chn_attach_vb_pool(hi_mcf_grp grp, hi_mcf_chn chn, hi_vb_pool vb_pool);
hi_s32 hi_mpi_mcf_chn_detach_vb_pool(hi_mcf_grp grp, hi_mcf_chn chn);

hi_s32 hi_mpi_mcf_chn_set_vb_src(hi_mcf_grp grp, hi_mcf_chn chn, hi_vb_src vb_src);
hi_s32 hi_mpi_mcf_chn_get_vb_src(hi_mcf_grp grp, hi_mcf_chn chn, hi_vb_src *vb_src);

hi_s32 hi_mpi_mcf_set_chn_stride_align(hi_mcf_grp grp, hi_mcf_chn chn, hi_u32 align);
hi_s32 hi_mpi_mcf_get_chn_stride_align(hi_mcf_grp grp, hi_mcf_chn chn, hi_u32 *align);

hi_s32 hi_mpi_mcf_set_ext_chn_attr(hi_mcf_grp grp, hi_mcf_chn chn, const hi_mcf_ext_chn_attr *ext_chn_attr);
hi_s32 hi_mpi_mcf_get_ext_chn_attr(hi_mcf_grp grp, hi_mcf_chn chn, hi_mcf_ext_chn_attr *ext_chn_attr);

hi_s32 hi_mpi_mcf_set_chn_crop(hi_mcf_grp grp, hi_mcf_chn chn, const hi_mcf_crop_info *crop_info);
hi_s32 hi_mpi_mcf_get_chn_crop(hi_mcf_grp grp, hi_mcf_chn chn, hi_mcf_crop_info *crop_info);

hi_s32 hi_mpi_mcf_get_chn_rgn_luma(hi_mcf_grp grp, hi_mcf_chn chn, hi_u32 num, const hi_rect *rgn, hi_u64 *luma_data,
    hi_s32 milli_sec);

hi_s32 hi_mpi_mcf_set_chn_rotation(hi_mcf_grp grp, hi_mcf_chn chn, hi_rotation rotation);
hi_s32 hi_mpi_mcf_get_chn_rotation(hi_mcf_grp grp, hi_mcf_chn chn, hi_rotation *rotation);

hi_s32 hi_mpi_mcf_get_chn_fd(hi_mcf_grp grp, hi_mcf_chn chn);

hi_s32 hi_mpi_mcf_get_grp_frame(hi_mcf_grp grp, hi_video_frame_info *color_frame,
    hi_video_frame_info *mono_frame, hi_s32 milli_sec);
hi_s32 hi_mpi_mcf_release_grp_frame(hi_mcf_grp grp, hi_video_frame_info *color_frame,
    hi_video_frame_info *mono_frame);

hi_s32 hi_mpi_mcf_set_vi_attr(hi_mcf_grp grp, const hi_mcf_vi_attr *mcf_vi_attr);
hi_s32 hi_mpi_mcf_get_vi_attr(hi_mcf_grp grp, hi_mcf_vi_attr *mcf_vi_attr);

#ifdef __cplusplus
}
#endif
#endif /* HI_MPI_MCF_H */
