/*
  Copyright (c), 2001-2024, Shenshu Tech. Co., Ltd.
 */

#ifndef HI_MPI_VI_H
#define HI_MPI_VI_H

#include "hi_common_vi.h"
#include "hi_common_gdc.h"
#include "hi_common_dis.h"
#include "hi_common_vb.h"
#include "hi_common_aiisp.h"
#include "hi_aiisp.h"

#ifdef __cplusplus
extern "C" {
#endif

/* device interface */
hi_s32 hi_mpi_vi_set_dev_attr(hi_vi_dev vi_dev, const hi_vi_dev_attr *dev_attr);
hi_s32 hi_mpi_vi_get_dev_attr(hi_vi_dev vi_dev, hi_vi_dev_attr *dev_attr);

hi_s32 hi_mpi_vi_set_dev_irq_affinity(hi_vi_dev vi_dev, hi_u32 irq_index);
hi_s32 hi_mpi_vi_get_dev_irq_affinity(hi_vi_dev vi_dev, hi_u32 *irq_index);

hi_s32 hi_mpi_vi_enable_dev(hi_vi_dev vi_dev);
hi_s32 hi_mpi_vi_disable_dev(hi_vi_dev vi_dev);

hi_s32 hi_mpi_vi_set_thermo_sns_attr(hi_vi_dev vi_dev, const hi_vi_thermo_sns_attr *sns_attr);
hi_s32 hi_mpi_vi_get_thermo_sns_attr(hi_vi_dev vi_dev, hi_vi_thermo_sns_attr *sns_attr);

hi_s32 hi_mpi_vi_enable_dev_send_frame(hi_vi_dev vi_dev);
hi_s32 hi_mpi_vi_disable_dev_send_frame(hi_vi_dev vi_dev);
hi_s32 hi_mpi_vi_send_dev_frame(hi_vi_dev vi_dev, const hi_video_frame_info *frame_info, hi_s32 milli_sec);

hi_s32 hi_mpi_vi_set_dev_timing_attr(hi_vi_dev vi_dev, const hi_vi_dev_timing_attr *timing_attr);
hi_s32 hi_mpi_vi_get_dev_timing_attr(hi_vi_dev vi_dev, hi_vi_dev_timing_attr *timing_attr);

hi_s32 hi_mpi_vi_set_dev_data_attr(hi_vi_dev vi_dev, const hi_vi_dev_data_attr *data_attr);
hi_s32 hi_mpi_vi_get_dev_data_attr(hi_vi_dev vi_dev, hi_vi_dev_data_attr *data_attr);

/* vi dev bind pipe interface */
hi_s32 hi_mpi_vi_bind(hi_vi_dev vi_dev, hi_vi_pipe vi_pipe);
hi_s32 hi_mpi_vi_unbind(hi_vi_dev vi_dev, hi_vi_pipe vi_pipe);
hi_s32 hi_mpi_vi_get_bind_by_dev(hi_vi_dev vi_dev, hi_vi_bind_pipe *bind_pipe);
hi_s32 hi_mpi_vi_get_bind_by_pipe(hi_vi_pipe vi_pipe, hi_vi_dev *vi_dev);

/* WDR fusion group */
hi_s32 hi_mpi_vi_set_wdr_fusion_grp_attr(hi_vi_grp fusion_grp, const hi_vi_wdr_fusion_grp_attr *grp_attr);
hi_s32 hi_mpi_vi_get_wdr_fusion_grp_attr(hi_vi_grp fusion_grp, hi_vi_wdr_fusion_grp_attr *grp_attr);

/* pipe interface */
hi_s32 hi_mpi_vi_create_pipe(hi_vi_pipe vi_pipe, const hi_vi_pipe_attr *pipe_attr);
hi_s32 hi_mpi_vi_destroy_pipe(hi_vi_pipe vi_pipe);

hi_s32 hi_mpi_vi_set_pipe_attr(hi_vi_pipe vi_pipe, const hi_vi_pipe_attr *pipe_attr);
hi_s32 hi_mpi_vi_get_pipe_attr(hi_vi_pipe vi_pipe, hi_vi_pipe_attr *pipe_attr);

hi_s32 hi_mpi_vi_start_pipe(hi_vi_pipe vi_pipe);
hi_s32 hi_mpi_vi_stop_pipe(hi_vi_pipe vi_pipe);

hi_s32 hi_mpi_vi_set_pipe_3dnr_attr(hi_vi_pipe vi_pipe, const hi_3dnr_attr *attr);
hi_s32 hi_mpi_vi_get_pipe_3dnr_attr(hi_vi_pipe vi_pipe, hi_3dnr_attr *attr);

hi_s32 hi_mpi_vi_set_pipe_3dnr_param(hi_vi_pipe vi_pipe, const hi_3dnr_param *param);
hi_s32 hi_mpi_vi_get_pipe_3dnr_param(hi_vi_pipe vi_pipe, hi_3dnr_param *param);

hi_s32 hi_mpi_vi_set_pipe_pre_crop(hi_vi_pipe vi_pipe, const hi_crop_info *crop_info);
hi_s32 hi_mpi_vi_get_pipe_pre_crop(hi_vi_pipe vi_pipe, hi_crop_info *crop_info);

hi_s32 hi_mpi_vi_set_pipe_post_crop(hi_vi_pipe vi_pipe, const hi_crop_info *crop_info);
hi_s32 hi_mpi_vi_get_pipe_post_crop(hi_vi_pipe vi_pipe, hi_crop_info *crop_info);

hi_s32 hi_mpi_vi_set_pipe_frame_dump_attr(hi_vi_pipe vi_pipe, const hi_vi_frame_dump_attr *dump_attr);
hi_s32 hi_mpi_vi_get_pipe_frame_dump_attr(hi_vi_pipe vi_pipe, hi_vi_frame_dump_attr *dump_attr);

hi_s32 hi_mpi_vi_get_pipe_frame(hi_vi_pipe vi_pipe, hi_video_frame_info *frame_info, hi_s32 milli_sec);
hi_s32 hi_mpi_vi_release_pipe_frame(hi_vi_pipe vi_pipe, const hi_video_frame_info *frame_info);

hi_s32 hi_mpi_vi_set_pipe_private_data_dump_attr(hi_vi_pipe vi_pipe, const hi_vi_private_data_dump_attr *dump_attr);
hi_s32 hi_mpi_vi_get_pipe_private_data_dump_attr(hi_vi_pipe vi_pipe, hi_vi_private_data_dump_attr *dump_attr);

hi_s32 hi_mpi_vi_get_pipe_private_data(hi_vi_pipe vi_pipe, hi_vi_private_data_info *data_info, hi_s32 milli_sec);
hi_s32 hi_mpi_vi_release_pipe_private_data(hi_vi_pipe vi_pipe, const hi_vi_private_data_info *data_info);

hi_s32 hi_mpi_vi_set_pipe_param(hi_vi_pipe vi_pipe, const hi_vi_pipe_param *pipe_param);
hi_s32 hi_mpi_vi_get_pipe_param(hi_vi_pipe vi_pipe, hi_vi_pipe_param *pipe_param);

hi_s32 hi_mpi_vi_set_pipe_bnr_buf_num(hi_vi_pipe vi_pipe, hi_u32 buf_num);
hi_s32 hi_mpi_vi_get_pipe_bnr_buf_num(hi_vi_pipe vi_pipe, hi_u32 *buf_num);

hi_s32 hi_mpi_vi_set_pipe_frame_source(hi_vi_pipe vi_pipe, const hi_vi_pipe_frame_source frame_source);
hi_s32 hi_mpi_vi_get_pipe_frame_source(hi_vi_pipe vi_pipe, hi_vi_pipe_frame_source *frame_source);

hi_s32 hi_mpi_vi_send_pipe_yuv(hi_vi_pipe vi_pipe, const hi_video_frame_info *frame_info, hi_s32 milli_sec);
hi_s32 hi_mpi_vi_send_pipe_raw(hi_vi_pipe vi_pipe,
                               const hi_video_frame_info *frame_info[], hi_u32 frame_num,
                               hi_s32 milli_sec);

hi_s32 hi_mpi_vi_query_pipe_status(hi_vi_pipe vi_pipe, hi_vi_pipe_status *pipe_status);

hi_s32 hi_mpi_vi_enable_pipe_interrupt(hi_vi_pipe vi_pipe);
hi_s32 hi_mpi_vi_disable_pipe_interrupt(hi_vi_pipe vi_pipe);

hi_s32 hi_mpi_vi_set_pipe_vc_number(hi_vi_pipe vi_pipe, hi_u32 vc_number);
hi_s32 hi_mpi_vi_get_pipe_vc_number(hi_vi_pipe vi_pipe, hi_u32 *vc_number);

hi_s32 hi_mpi_vi_set_pipe_low_delay(hi_vi_pipe vi_pipe, const hi_low_delay_info *low_delay_info);
hi_s32 hi_mpi_vi_get_pipe_low_delay(hi_vi_pipe vi_pipe, hi_low_delay_info *low_delay_info);

hi_s32 hi_mpi_vi_set_pipe_buf_wrap_attr(hi_vi_pipe vi_pipe, const hi_vi_pipe_buf_wrap_attr *wrap_attr);
hi_s32 hi_mpi_vi_get_pipe_buf_wrap_attr(hi_vi_pipe vi_pipe, hi_vi_pipe_buf_wrap_attr *wrap_attr);

hi_s32 hi_mpi_vi_set_pipe_frame_interrupt_attr(hi_vi_pipe vi_pipe, const hi_frame_interrupt_attr *interrupt_attr);
hi_s32 hi_mpi_vi_get_pipe_frame_interrupt_attr(hi_vi_pipe vi_pipe, hi_frame_interrupt_attr *interrupt_attr);

hi_s32 hi_mpi_vi_get_pipe_compress_param(hi_vi_pipe vi_pipe, hi_vi_compress_param *compress_param);

hi_s32 hi_mpi_vi_set_pipe_user_pic(hi_vi_pipe vi_pipe, const hi_video_frame_info *user_pic_info);
hi_s32 hi_mpi_vi_enable_pipe_user_pic(hi_vi_pipe vi_pipe);
hi_s32 hi_mpi_vi_disable_pipe_user_pic(hi_vi_pipe vi_pipe);

hi_s32 hi_mpi_vi_set_pipe_vb_src(hi_vi_pipe vi_pipe, hi_vb_src vb_src);
hi_s32 hi_mpi_vi_get_pipe_vb_src(hi_vi_pipe vi_pipe, hi_vb_src *vb_src);

hi_s32 hi_mpi_vi_attach_pipe_vb_pool(hi_vi_pipe vi_pipe, hi_vb_pool vb_pool);
hi_s32 hi_mpi_vi_detach_pipe_vb_pool(hi_vi_pipe vi_pipe);

hi_s32 hi_mpi_vi_attach_aiisp_vb_pool(hi_vi_pipe vi_pipe, hi_aiisp_pool *pool_attr);
hi_s32 hi_mpi_vi_detach_aiisp_vb_pool(hi_vi_pipe vi_pipe, hi_aiisp_type aiisp_type);
hi_s32 hi_mpi_vi_set_pipe_online_clock(hi_vi_pipe vi_pipe, hi_u32 pixel_rate);
hi_s32 hi_mpi_vi_get_pipe_fd(hi_vi_pipe vi_pipe);

/* channel interface */
hi_s32 hi_mpi_vi_set_chn_attr(hi_vi_pipe vi_pipe, hi_vi_chn vi_chn, const hi_vi_chn_attr *chn_attr);
hi_s32 hi_mpi_vi_get_chn_attr(hi_vi_pipe vi_pipe, hi_vi_chn vi_chn, hi_vi_chn_attr *chn_attr);

hi_s32 hi_mpi_vi_set_ext_chn_attr(hi_vi_pipe vi_pipe, hi_vi_chn vi_chn, const hi_vi_ext_chn_attr *ext_chn_attr);
hi_s32 hi_mpi_vi_get_ext_chn_attr(hi_vi_pipe vi_pipe, hi_vi_chn vi_chn, hi_vi_ext_chn_attr *ext_chn_attr);

hi_s32 hi_mpi_vi_enable_chn(hi_vi_pipe vi_pipe, hi_vi_chn vi_chn);
hi_s32 hi_mpi_vi_disable_chn(hi_vi_pipe vi_pipe, hi_vi_chn vi_chn);

hi_s32 hi_mpi_vi_set_chn_crop(hi_vi_pipe vi_pipe, hi_vi_chn vi_chn, const hi_vi_crop_info *crop_info);
hi_s32 hi_mpi_vi_get_chn_crop(hi_vi_pipe vi_pipe, hi_vi_chn vi_chn, hi_vi_crop_info *crop_info);

hi_s32 hi_mpi_vi_set_chn_dis_cfg(hi_vi_pipe vi_pipe, hi_vi_chn vi_chn, const hi_dis_cfg *dis_cfg);
hi_s32 hi_mpi_vi_get_chn_dis_cfg(hi_vi_pipe vi_pipe, hi_vi_chn vi_chn, hi_dis_cfg *dis_cfg);
hi_s32 hi_mpi_vi_set_chn_dis_attr(hi_vi_pipe vi_pipe, hi_vi_chn vi_chn, const hi_dis_attr *dis_attr);
hi_s32 hi_mpi_vi_get_chn_dis_attr(hi_vi_pipe vi_pipe, hi_vi_chn vi_chn, hi_dis_attr *dis_attr);
hi_s32 hi_mpi_vi_get_chn_dis_out_size(hi_vi_pipe vi_pipe, hi_vi_chn vi_chn, hi_size *out_size);
hi_s32 hi_mpi_vi_set_chn_dis_param(hi_vi_pipe vi_pipe, hi_vi_chn vi_chn, const hi_dis_param *dis_param);
hi_s32 hi_mpi_vi_get_chn_dis_param(hi_vi_pipe vi_pipe, hi_vi_chn vi_chn, hi_dis_param *dis_param);
hi_s32 hi_mpi_vi_set_chn_dis_alg_attr(hi_vi_pipe vi_pipe, hi_vi_chn vi_chn, const hi_dis_alg_attr *dis_alg_attr);
hi_s32 hi_mpi_vi_get_chn_dis_alg_attr(hi_vi_pipe vi_pipe, hi_vi_chn vi_chn, hi_dis_alg_attr *dis_alg_attr);

hi_s32 hi_mpi_vi_set_chn_rotation(hi_vi_pipe vi_pipe, hi_vi_chn vi_chn, const hi_rotation_attr *rotation_attr);
hi_s32 hi_mpi_vi_get_chn_rotation(hi_vi_pipe vi_pipe, hi_vi_chn vi_chn, hi_rotation_attr *rotation_attr);

hi_s32 hi_mpi_vi_set_chn_ldc_attr(hi_vi_pipe vi_pipe, hi_vi_chn vi_chn, const hi_ldc_attr *ldc_attr);
hi_s32 hi_mpi_vi_get_chn_ldc_attr(hi_vi_pipe vi_pipe, hi_vi_chn vi_chn, hi_ldc_attr *ldc_attr);

hi_s32 hi_mpi_vi_get_chn_frame(hi_vi_pipe vi_pipe, hi_vi_chn vi_chn, hi_video_frame_info *frame_info,
    hi_s32 milli_sec);
hi_s32 hi_mpi_vi_release_chn_frame(hi_vi_pipe vi_pipe, hi_vi_chn vi_chn, const hi_video_frame_info *frame_info);

hi_s32 hi_mpi_vi_set_chn_low_delay(hi_vi_pipe vi_pipe, hi_vi_chn vi_chn, const hi_low_delay_info *low_delay_info);
hi_s32 hi_mpi_vi_get_chn_low_delay(hi_vi_pipe vi_pipe, hi_vi_chn vi_chn, hi_low_delay_info *low_delay_info);

hi_s32 hi_mpi_vi_set_chn_align(hi_vi_pipe vi_pipe, hi_vi_chn vi_chn, hi_u32 align);
hi_s32 hi_mpi_vi_get_chn_align(hi_vi_pipe vi_pipe, hi_vi_chn vi_chn, hi_u32 *align);

hi_s32 hi_mpi_vi_set_chn_fmu_mode(hi_vi_pipe vi_pipe, hi_vi_chn vi_chn, hi_fmu_mode fmu_mode);
hi_s32 hi_mpi_vi_get_chn_fmu_mode(hi_vi_pipe vi_pipe, hi_vi_chn vi_chn, hi_fmu_mode *fmu_mode);

hi_s32 hi_mpi_vi_set_chn_vb_src(hi_vi_pipe vi_pipe, hi_vi_chn vi_chn, hi_vb_src vb_src);
hi_s32 hi_mpi_vi_get_chn_vb_src(hi_vi_pipe vi_pipe, hi_vi_chn vi_chn, hi_vb_src *vb_src);

hi_s32 hi_mpi_vi_attach_chn_vb_pool(hi_vi_pipe vi_pipe, hi_vi_chn vi_chn, hi_vb_pool vb_pool);
hi_s32 hi_mpi_vi_detach_chn_vb_pool(hi_vi_pipe vi_pipe, hi_vi_chn vi_chn);

hi_s32 hi_mpi_vi_query_chn_status(hi_vi_pipe vi_pipe, hi_vi_chn vi_chn, hi_vi_chn_status *chn_status);

hi_s32 hi_mpi_vi_get_chn_fd(hi_vi_pipe vi_pipe, hi_vi_chn vi_chn);

/* stitch group interface */
hi_s32 hi_mpi_vi_set_stitch_grp_attr(hi_vi_grp stitch_grp, const hi_vi_stitch_grp_attr *stitch_grp_attr);
hi_s32 hi_mpi_vi_get_stitch_grp_attr(hi_vi_grp stitch_grp, hi_vi_stitch_grp_attr *stitch_grp_attr);

/* distribute group interface */
hi_s32 hi_mpi_vi_set_distribute_grp_attr(hi_vi_grp distribute_grp,
                                         const hi_vi_distribute_grp_attr *distribute_grp_attr);
hi_s32 hi_mpi_vi_get_distribute_grp_attr(hi_vi_grp distribute_grp, hi_vi_distribute_grp_attr *distribute_grp_attr);

/* vi module interface */
hi_s32 hi_mpi_vi_set_mod_param(const hi_vi_mod_param *mod_param);
hi_s32 hi_mpi_vi_get_mod_param(hi_vi_mod_param *mod_param);
hi_s32 hi_mpi_vi_enable_chn_motion_denoise(hi_vi_pipe vi_pipe, hi_vi_chn vi_chn);
hi_s32 hi_mpi_vi_disable_chn_motion_denoise(hi_vi_pipe vi_pipe, hi_vi_chn vi_chn);

/* psfm interface */
hi_s32 hi_mpi_vi_set_pipe_psfm_attr(hi_vi_pipe vi_pipe, const hi_vi_psfm_attr *psfm_attr);
hi_s32 hi_mpi_vi_get_pipe_psfm_attr(hi_vi_pipe vi_pipe, hi_vi_psfm_attr *psfm_attr);

#ifdef __cplusplus
}
#endif
#endif /* HI_MPI_VI_H */
