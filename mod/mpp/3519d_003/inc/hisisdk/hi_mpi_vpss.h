/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */

#ifndef HI_MPI_VPSS_H
#define HI_MPI_VPSS_H

#include "hi_common.h"
#include "hi_common_video.h"
#include "hi_common_vb.h"
#include "hi_common_gdc.h"
#include "hi_common_vpss.h"

#ifdef __cplusplus
extern "C" {
#endif
/* module param settings */
hi_s32 hi_mpi_vpss_set_mod_param(const hi_vpss_mod_param *mod_param);
hi_s32 hi_mpi_vpss_get_mod_param(hi_vpss_mod_param *mod_param);

/* group settings */
hi_s32 hi_mpi_vpss_create_grp(hi_vpss_grp grp, const hi_vpss_grp_attr *grp_attr);
hi_s32 hi_mpi_vpss_destroy_grp(hi_vpss_grp grp);

hi_s32 hi_mpi_vpss_start_grp(hi_vpss_grp grp);
hi_s32 hi_mpi_vpss_stop_grp(hi_vpss_grp grp);

hi_s32 hi_mpi_vpss_reset_grp(hi_vpss_grp grp);

hi_s32 hi_mpi_vpss_set_grp_attr(hi_vpss_grp grp, const hi_vpss_grp_attr *grp_attr);
hi_s32 hi_mpi_vpss_get_grp_attr(hi_vpss_grp grp, hi_vpss_grp_attr *grp_attr);

hi_s32 hi_mpi_vpss_set_grp_3dnr_attr(hi_vpss_grp grp, const hi_3dnr_attr *attr);
hi_s32 hi_mpi_vpss_get_grp_3dnr_attr(hi_vpss_grp grp, hi_3dnr_attr *attr);

hi_s32 hi_mpi_vpss_set_grp_3dnr_param(hi_vpss_grp grp, const hi_3dnr_param *param);
hi_s32 hi_mpi_vpss_get_grp_3dnr_param(hi_vpss_grp grp, hi_3dnr_param *param);

hi_s32 hi_mpi_vpss_set_grp_param(hi_vpss_grp grp, const hi_vpss_grp_param *grp_param);
hi_s32 hi_mpi_vpss_get_grp_param(hi_vpss_grp grp, hi_vpss_grp_param *grp_param);

hi_s32 hi_mpi_vpss_set_grp_cfg(hi_vpss_grp grp, const hi_vpss_grp_cfg *grp_cfg);
hi_s32 hi_mpi_vpss_get_grp_cfg(hi_vpss_grp grp, hi_vpss_grp_cfg *grp_cfg);

hi_s32 hi_mpi_vpss_set_grp_crop(hi_vpss_grp grp, const hi_vpss_crop_info *crop_info);
hi_s32 hi_mpi_vpss_get_grp_crop(hi_vpss_grp grp, hi_vpss_crop_info *crop_info);

hi_s32 hi_mpi_vpss_send_frame(hi_vpss_grp grp, const hi_video_frame_info *frame_info, hi_s32 milli_sec);
hi_s32 hi_mpi_vpss_send_frame_ex(hi_vpss_grp grp, hi_vpss_grp_pipe pipe,
    const hi_video_frame_info *frame_info, hi_s32 milli_sec);

hi_s32 hi_mpi_vpss_get_grp_frame(hi_vpss_grp grp, hi_video_frame_info *frame_info, hi_s32 milli_sec);
hi_s32 hi_mpi_vpss_release_grp_frame(hi_vpss_grp grp, const hi_video_frame_info *frame_info);

hi_s32 hi_mpi_vpss_get_grp_frame_ex(hi_vpss_grp grp, hi_vpss_grp_pipe pipe,
    hi_video_frame_info *frame_info, hi_s32 milli_sec);
hi_s32 hi_mpi_vpss_release_grp_frame_ex(hi_vpss_grp grp, hi_vpss_grp_pipe pipe,
    const hi_video_frame_info *frame_info);

hi_s32 hi_mpi_vpss_enable_backup_frame(hi_vpss_grp grp);
hi_s32 hi_mpi_vpss_disable_backup_frame(hi_vpss_grp grp);

hi_s32 hi_mpi_vpss_enable_user_frame_rate_ctrl(hi_vpss_grp grp);
hi_s32 hi_mpi_vpss_disable_user_frame_rate_ctrl(hi_vpss_grp grp);

hi_s32 hi_mpi_vpss_set_grp_frame_interrupt_attr(hi_vpss_grp grp, const hi_frame_interrupt_attr *frame_interrupt_attr);
hi_s32 hi_mpi_vpss_get_grp_frame_interrupt_attr(hi_vpss_grp grp, hi_frame_interrupt_attr *frame_interrupt_attr);

/* chn settings */
hi_s32 hi_mpi_vpss_set_chn_attr(hi_vpss_grp grp, hi_vpss_chn chn, const hi_vpss_chn_attr *chn_attr);
hi_s32 hi_mpi_vpss_get_chn_attr(hi_vpss_grp grp, hi_vpss_chn chn, hi_vpss_chn_attr *chn_attr);

hi_s32 hi_mpi_vpss_enable_chn(hi_vpss_grp grp, hi_vpss_chn chn);
hi_s32 hi_mpi_vpss_disable_chn(hi_vpss_grp grp, hi_vpss_chn chn);

hi_s32 hi_mpi_vpss_set_chn_fmu_mode(hi_vpss_grp grp, hi_vpss_chn chn, hi_fmu_mode mode);
hi_s32 hi_mpi_vpss_get_chn_fmu_mode(hi_vpss_grp grp, hi_vpss_chn chn, hi_fmu_mode *mode);

hi_s32 hi_mpi_vpss_set_chn_rotation(hi_vpss_grp grp, hi_vpss_chn chn, const hi_rotation_attr *rotation_attr);
hi_s32 hi_mpi_vpss_get_chn_rotation(hi_vpss_grp grp, hi_vpss_chn chn, hi_rotation_attr *rotation_attr);

hi_s32 hi_mpi_vpss_get_chn_frame(hi_vpss_grp grp, hi_vpss_chn chn, hi_video_frame_info *frame_info, hi_s32 milli_sec);
hi_s32 hi_mpi_vpss_release_chn_frame(hi_vpss_grp grp, hi_vpss_chn chn, const hi_video_frame_info *frame_info);

hi_s32 hi_mpi_vpss_get_chn_rgn_luma(hi_vpss_grp grp, hi_vpss_chn chn, const hi_vpss_rgn_info *luma_rgn,
    hi_u64 *luma_data, hi_s32 milli_sec);

hi_s32 hi_mpi_vpss_set_chn_crop(hi_vpss_grp grp, hi_vpss_chn chn, const hi_vpss_crop_info *crop_info);
hi_s32 hi_mpi_vpss_get_chn_crop(hi_vpss_grp grp, hi_vpss_chn chn, hi_vpss_crop_info *crop_info);

hi_s32 hi_mpi_vpss_set_chn_vb_src(hi_vpss_grp grp, hi_vpss_chn chn, hi_vb_src vb_src);
hi_s32 hi_mpi_vpss_get_chn_vb_src(hi_vpss_grp grp, hi_vpss_chn chn, hi_vb_src *vb_src);

hi_s32 hi_mpi_vpss_attach_chn_vb_pool(hi_vpss_grp grp, hi_vpss_chn chn, hi_vb_pool vb_pool);
hi_s32 hi_mpi_vpss_detach_chn_vb_pool(hi_vpss_grp grp, hi_vpss_chn chn);

hi_s32 hi_mpi_vpss_set_chn_low_delay(hi_vpss_grp grp, hi_vpss_chn chn, const hi_low_delay_info *low_delay_info);
hi_s32 hi_mpi_vpss_get_chn_low_delay(hi_vpss_grp grp, hi_vpss_chn chn, hi_low_delay_info *low_delay_info);

hi_s32 hi_mpi_vpss_set_chn_stride_align(hi_vpss_grp grp, hi_vpss_chn chn, hi_u32 align);
hi_s32 hi_mpi_vpss_get_chn_stride_align(hi_vpss_grp grp, hi_vpss_chn chn, hi_u32 *align);

hi_s32 hi_mpi_vpss_set_ext_chn_attr(hi_vpss_grp grp, hi_vpss_chn chn, const hi_vpss_ext_chn_attr *ext_chn_attr);
hi_s32 hi_mpi_vpss_get_ext_chn_attr(hi_vpss_grp grp, hi_vpss_chn chn, hi_vpss_ext_chn_attr *ext_chn_attr);

hi_s32 hi_mpi_vpss_set_chn_scale_coef_type(hi_vpss_grp grp, hi_vpss_chn chn, hi_scale_coef_type type);
hi_s32 hi_mpi_vpss_get_chn_scale_coef_type(hi_vpss_grp grp, hi_vpss_chn chn, hi_scale_coef_type *type);

/* gdc settings */
hi_s32 hi_mpi_vpss_set_grp_gdc_param(hi_vpss_grp grp, const hi_gdc_param *gdc_param);
hi_s32 hi_mpi_vpss_get_grp_gdc_param(hi_vpss_grp grp, hi_gdc_param *gdc_param);

hi_s32 hi_mpi_vpss_set_grp_fisheye_cfg(hi_vpss_grp grp, const hi_fisheye_cfg *fisheye_cfg);
hi_s32 hi_mpi_vpss_get_grp_fisheye_cfg(hi_vpss_grp grp, hi_fisheye_cfg *fisheye_cfg);

hi_s32 hi_mpi_vpss_query_grp_fisheye_pos_dst_to_src(hi_vpss_grp grp, hi_u32 rgn_index,
    const hi_point *dst, hi_point *src);
hi_s32 hi_mpi_vpss_query_grp_ldc_pos_dst_to_src(hi_vpss_grp grp, const hi_point *dst, hi_point *src);
hi_s32 hi_mpi_vpss_query_grp_ldc_pos_src_to_dst(hi_vpss_grp grp, const hi_point *src, hi_point *dst);

hi_s32 hi_mpi_vpss_query_grp_gdc_pos_dst_to_src(hi_vpss_grp grp, hi_u32 index,
    const hi_point *dst_point, hi_point *src_point);

hi_s32 hi_mpi_vpss_set_grp_ldc(hi_vpss_grp grp, const hi_ldc_attr *ldc_attr);
hi_s32 hi_mpi_vpss_get_grp_ldc(hi_vpss_grp grp, hi_ldc_attr *ldc_attr);

hi_s32 hi_mpi_vpss_set_grp_spread(hi_vpss_grp grp, const hi_spread_attr *spread_attr);
hi_s32 hi_mpi_vpss_get_grp_spread(hi_vpss_grp grp, hi_spread_attr *spread_attr);

hi_s32 hi_mpi_vpss_set_grp_fisheye(hi_vpss_grp grp, const hi_fisheye_correction_attr *fisheye_attr);
hi_s32 hi_mpi_vpss_get_grp_fisheye(hi_vpss_grp grp, hi_fisheye_correction_attr *fisheye_attr);

hi_s32 hi_mpi_vpss_set_grp_rotation(hi_vpss_grp grp, const hi_rotation_attr *rotation_attr);
hi_s32 hi_mpi_vpss_get_grp_rotation(hi_vpss_grp grp, hi_rotation_attr *rotation_attr);

hi_s32 hi_mpi_vpss_set_grp_pmf(hi_vpss_grp grp, const hi_pmf_attr *pmf_attr);
hi_s32 hi_mpi_vpss_get_grp_pmf(hi_vpss_grp grp, hi_pmf_attr *pmf_attr);

hi_s32 hi_mpi_vpss_set_grp_fov(hi_vpss_grp grp, const hi_fov_attr *fov_attr);
hi_s32 hi_mpi_vpss_get_grp_fov(hi_vpss_grp grp, hi_fov_attr *fov_attr);

hi_s32 hi_mpi_vpss_set_grp_lut(hi_vpss_grp grp, const hi_lut_attr *lut_attr);
hi_s32 hi_mpi_vpss_get_grp_lut(hi_vpss_grp grp, hi_lut_attr *lut_attr);

hi_s32 hi_mpi_vpss_set_grp_zme(hi_vpss_grp grp, const hi_size *out_size);
hi_s32 hi_mpi_vpss_get_grp_zme(hi_vpss_grp grp, hi_size *out_size);

hi_s32 hi_mpi_vpss_set_grp_stitch(hi_vpss_grp grp, const hi_stitch_attr *stitch_attr);
hi_s32 hi_mpi_vpss_get_grp_stitch(hi_vpss_grp grp, hi_stitch_attr *stitch_attr);

hi_s32 hi_mpi_vpss_set_grp_stitch_blend_param(hi_vpss_grp grp, const hi_stitch_blend_param *blend_param);
hi_s32 hi_mpi_vpss_get_grp_stitch_blend_param(hi_vpss_grp grp, hi_stitch_blend_param *blend_param);

/* fd */
hi_s32 hi_mpi_vpss_get_chn_fd(hi_vpss_grp grp, hi_vpss_chn chn);

#ifdef __cplusplus
}
#endif
#endif /* HI_MPI_VPSS_H */
