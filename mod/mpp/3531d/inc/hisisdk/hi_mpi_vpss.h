/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: Vpss mpi interface
 * Author: Hisilicon multimedia software group
 * Create: 2019/07/30
 */

#ifndef __HI_MPI_VPSS_H__
#define __HI_MPI_VPSS_H__

#include "hi_common.h"
#include "hi_common_video.h"
#include "hi_common_vb.h"
#include "hi_common_vpss.h"


#ifdef __cplusplus
extern "C" {
#endif

/* group settings */
hi_s32 hi_mpi_vpss_create_grp(hi_vpss_grp grp, const hi_vpss_grp_attr *grp_attr);
hi_s32 hi_mpi_vpss_destroy_grp(hi_vpss_grp grp);

hi_s32 hi_mpi_vpss_start_grp(hi_vpss_grp grp);
hi_s32 hi_mpi_vpss_stop_grp(hi_vpss_grp grp);

hi_s32 hi_mpi_vpss_reset_grp(hi_vpss_grp grp);

hi_s32 hi_mpi_vpss_get_grp_attr(hi_vpss_grp grp, hi_vpss_grp_attr *grp_attr);
hi_s32 hi_mpi_vpss_set_grp_attr(hi_vpss_grp grp, const hi_vpss_grp_attr *grp_attr);

hi_s32 hi_mpi_vpss_get_grp_param(hi_vpss_grp grp, hi_vpss_grp_param *grp_param);
hi_s32 hi_mpi_vpss_set_grp_param(hi_vpss_grp grp, const hi_vpss_grp_param *grp_param);

hi_s32 hi_mpi_vpss_get_grp_nrx_adv_param(hi_vpss_grp grp, hi_vpss_nrx_adv_param *nrx_adv_param);
hi_s32 hi_mpi_vpss_set_grp_nrx_adv_param(hi_vpss_grp grp, const hi_vpss_nrx_adv_param *nrx_adv_param);

hi_s32 hi_mpi_vpss_set_grp_crop(hi_vpss_grp grp, const hi_vpss_crop_info *crop_info);
hi_s32 hi_mpi_vpss_get_grp_crop(hi_vpss_grp grp, hi_vpss_crop_info *crop_info);

hi_s32 hi_mpi_vpss_send_frame(hi_vpss_grp grp, const hi_video_frame_info *frame_info, hi_s32 milli_sec);

hi_s32 hi_mpi_vpss_get_grp_frame(hi_vpss_grp grp, hi_video_frame_info *frame_info, hi_s32 milli_sec);
hi_s32 hi_mpi_vpss_release_grp_frame(hi_vpss_grp grp, const hi_video_frame_info *frame_info);

hi_s32 hi_mpi_vpss_enable_backup_frame(hi_vpss_grp grp);
hi_s32 hi_mpi_vpss_disable_backup_frame(hi_vpss_grp grp);

hi_s32 hi_mpi_vpss_set_chn_sharpen(hi_vpss_grp grp, hi_vpss_chn chn, const hi_vpss_chn_sharpen_param *sharpen_param);
hi_s32 hi_mpi_vpss_get_chn_sharpen(hi_vpss_grp grp, hi_vpss_chn chn, hi_vpss_chn_sharpen_param *sharpen_param);

hi_s32 hi_mpi_vpss_set_grp_delay(hi_vpss_grp grp, hi_u32 delay);
hi_s32 hi_mpi_vpss_get_grp_delay(hi_vpss_grp grp, hi_u32 *delay);

hi_s32 hi_mpi_vpss_set_grp_sizer(hi_vpss_grp grp, const hi_vpss_sizer_info *sizer_info);
hi_s32 hi_mpi_vpss_get_grp_sizer(hi_vpss_grp grp, hi_vpss_sizer_info *sizer_info);

hi_s32 hi_mpi_vpss_enable_user_frame_rate_ctrl(hi_vpss_grp grp);
hi_s32 hi_mpi_vpss_disable_user_frame_rate_ctrl(hi_vpss_grp grp);

hi_s32 hi_mpi_vpss_get_ldci_attr(hi_vpss_grp grp, hi_vpss_ldci_attr *ldci_attr);
hi_s32 hi_mpi_vpss_set_ldci_attr(hi_vpss_grp grp, const hi_vpss_ldci_attr *ldci_attr);

/* chn settings */
hi_s32 hi_mpi_vpss_set_chn_attr(hi_vpss_grp grp, hi_vpss_chn chn, const hi_vpss_chn_attr *chn_attr);
hi_s32 hi_mpi_vpss_get_chn_attr(hi_vpss_grp grp, hi_vpss_chn chn, hi_vpss_chn_attr *chn_attr);

hi_s32 hi_mpi_vpss_enable_chn(hi_vpss_grp grp, hi_vpss_chn chn);
hi_s32 hi_mpi_vpss_disable_chn(hi_vpss_grp grp, hi_vpss_chn chn);

hi_s32 hi_mpi_vpss_set_chn_rotation(hi_vpss_grp grp, hi_vpss_chn chn, const hi_rotation_attr *rotation_attr);
hi_s32 hi_mpi_vpss_get_chn_rotation(hi_vpss_grp grp, hi_vpss_chn chn, hi_rotation_attr *rotation_attr);

hi_s32 hi_mpi_vpss_get_chn_frame(hi_vpss_grp grp, hi_vpss_chn chn, hi_video_frame_info *frame_info, hi_s32 milli_sec);
hi_s32 hi_mpi_vpss_release_chn_frame(hi_vpss_grp grp, hi_vpss_chn chn, const hi_video_frame_info *frame_info);

hi_s32 hi_mpi_vpss_get_grp_rgn_luma(hi_vpss_grp grp, hi_u32 num,
    const hi_rect *rgn, hi_vpss_luma_result *luma_result, hi_s32 milli_sec);
hi_s32 hi_mpi_vpss_get_chn_rgn_luma(hi_vpss_grp grp, hi_vpss_chn chn, hi_u32 num,
    const hi_rect *rgn, hi_u64 *luma_data, hi_s32 milli_sec);

hi_s32 hi_mpi_vpss_attach_vb_pool(hi_vpss_grp grp, hi_vpss_chn chn, hi_vb_pool vb_pool);
hi_s32 hi_mpi_vpss_detach_vb_pool(hi_vpss_grp grp, hi_vpss_chn chn);

hi_s32 hi_mpi_vpss_enable_quick_send(hi_vpss_grp grp, hi_vpss_chn chn);
hi_s32 hi_mpi_vpss_disable_quick_send(hi_vpss_grp grp, hi_vpss_chn chn);

/* module param settings */
hi_s32 hi_mpi_vpss_set_mod_param(const hi_vpss_mod_param *mod_param);
hi_s32 hi_mpi_vpss_get_mod_param(hi_vpss_mod_param *mod_param);

hi_s32 hi_mpi_vpss_get_chn_fd(hi_vpss_grp grp, hi_vpss_chn chn);
hi_s32 hi_mpi_vpss_close_fd(hi_void);

#ifdef __cplusplus
}
#endif

#endif /* __HI_MPI_VPSS_H__ */
