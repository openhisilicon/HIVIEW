/*
  Copyright (c), 2001-2024, Shenshu Tech. Co., Ltd.
 */

#ifndef HI_MPI_PSFM_H
#define HI_MPI_PSFM_H

#include "hi_common_psfm.h"

#ifdef __cplusplus
extern "C" {
#endif

hi_s32 hi_mpi_psfm_create_grp(hi_psfm_grp psfm_grp, const hi_psfm_grp_attr *grp_attr);
hi_s32 hi_mpi_psfm_destroy_grp(hi_psfm_grp psfm_grp);

hi_s32 hi_mpi_psfm_start_grp(hi_psfm_grp psfm_grp);
hi_s32 hi_mpi_psfm_stop_grp(hi_psfm_grp psfm_grp);

hi_s32 hi_mpi_psfm_set_chn_attr(hi_psfm_grp psfm_grp, hi_psfm_chn psfm_chn, const hi_psfm_chn_attr *chn_attr);
hi_s32 hi_mpi_psfm_get_chn_attr(hi_psfm_grp psfm_grp, hi_psfm_chn psfm_chn, hi_psfm_chn_attr *chn_attr);

hi_s32 hi_mpi_psfm_enable_chn(hi_psfm_grp psfm_grp, hi_psfm_chn psfm_chn);
hi_s32 hi_mpi_psfm_disable_chn(hi_psfm_grp psfm_grp, hi_psfm_chn psfm_chn);

hi_s32 hi_mpi_psfm_get_chn_frame(hi_psfm_grp psfm_grp, hi_psfm_chn psfm_chn, hi_video_frame_info *frame_info,
    hi_s32 milli_sec);
hi_s32 hi_mpi_psfm_release_chn_frame(hi_psfm_grp psfm_grp, hi_psfm_chn psfm_chn,
    const hi_video_frame_info *frame_info);

hi_s32 hi_mpi_psfm_set_chn_user_pic(hi_psfm_grp psfm_grp, hi_psfm_chn psfm_chn,
    const hi_video_frame_info *user_pic_info);
hi_s32 hi_mpi_psfm_enable_chn_user_pic(hi_psfm_grp psfm_grp, hi_psfm_chn psfm_chn);
hi_s32 hi_mpi_psfm_disable_chn_user_pic(hi_psfm_grp psfm_grp, hi_psfm_chn psfm_chn);

hi_s32 hi_mpi_psfm_register_frame_source_callback(const hi_psfm_frame_source_callback *callback);
hi_s32 hi_mpi_psfm_unregister_frame_source_callback(hi_void);

#ifdef __cplusplus
}
#endif
#endif /* HI_MPI_PSFM_H */
