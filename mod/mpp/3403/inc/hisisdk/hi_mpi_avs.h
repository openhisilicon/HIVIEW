/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */

#ifndef __HI_MPI_AVS_H__
#define __HI_MPI_AVS_H__

#include "hi_common_avs.h"


#ifdef __cplusplus
extern "C" {
#endif

hi_s32 hi_mpi_avs_create_grp(hi_avs_grp grp, const hi_avs_grp_attr *grp_attr);
hi_s32 hi_mpi_avs_destroy_grp(hi_avs_grp grp);

hi_s32 hi_mpi_avs_start_grp(hi_avs_grp grp);
hi_s32 hi_mpi_avs_stop_grp(hi_avs_grp grp);

hi_s32 hi_mpi_avs_reset_grp(hi_avs_grp grp);

hi_s32 hi_mpi_avs_close_fd(hi_void);

hi_s32 hi_mpi_avs_get_grp_attr(hi_avs_grp grp, hi_avs_grp_attr *grp_attr);
hi_s32 hi_mpi_avs_set_grp_attr(hi_avs_grp grp, const hi_avs_grp_attr *grp_attr);

hi_s32 hi_mpi_avs_send_pipe_frame(hi_avs_grp grp, hi_avs_pipe pipe, const hi_video_frame_info *frame_info,
                                  hi_s32 milli_sec);

hi_s32 hi_mpi_avs_get_pipe_frame(hi_avs_grp grp, hi_avs_pipe pipe, hi_video_frame_info *frame_info, hi_s32 milli_sec);
hi_s32 hi_mpi_avs_release_pipe_frame(hi_avs_grp grp, hi_avs_pipe pipe,
                                     const hi_video_frame_info *frame_info);

hi_s32 hi_mpi_avs_set_chn_attr(hi_avs_grp grp, hi_avs_chn chn, const hi_avs_chn_attr *chn_attr);
hi_s32 hi_mpi_avs_get_chn_attr(hi_avs_grp grp, hi_avs_chn chn, hi_avs_chn_attr *chn_attr);

hi_s32 hi_mpi_avs_enable_chn(hi_avs_grp grp, hi_avs_chn chn);
hi_s32 hi_mpi_avs_disable_chn(hi_avs_grp grp, hi_avs_chn chn);

hi_s32 hi_mpi_avs_get_chn_frame(hi_avs_grp grp, hi_avs_chn chn, hi_video_frame_info *frame_info,
                                hi_s32 milli_sec);
hi_s32 hi_mpi_avs_release_chn_frame(hi_avs_grp grp, hi_avs_chn chn, const hi_video_frame_info *frame_info);

hi_s32 hi_mpi_avs_get_mod_param(hi_avs_mod_param *mod_param);
hi_s32 hi_mpi_avs_set_mod_param(const hi_avs_mod_param *mod_param);

#ifdef __cplusplus
}
#endif
#endif /* __HI_MPI_AVS_H__ */
