/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */

#ifndef __HI_MPI_VI_H__
#define __HI_MPI_VI_H__

#include "hi_common_vi.h"
#include "hi_common_gdc.h"
#include "hi_common_dis.h"
#include "hi_common_vb.h"


#ifdef __cplusplus
extern "C" {
#endif

/* device interface */
hi_s32 hi_mpi_vi_set_dev_attr(hi_vi_dev vi_dev, const hi_vi_dev_attr *dev_attr);
hi_s32 hi_mpi_vi_get_dev_attr(hi_vi_dev vi_dev, hi_vi_dev_attr *dev_attr);

hi_s32 hi_mpi_vi_set_bas_attr(hi_vi_dev vi_dev, const hi_vi_bas_attr *bas_attr);
hi_s32 hi_mpi_vi_get_bas_attr(hi_vi_dev vi_dev, hi_vi_bas_attr *bas_attr);

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

hi_s32 hi_mpi_vi_set_pipe_pre_crop(hi_vi_pipe vi_pipe, const hi_crop_info *crop_info);
hi_s32 hi_mpi_vi_get_pipe_pre_crop(hi_vi_pipe vi_pipe, hi_crop_info *crop_info);

hi_s32 hi_mpi_vi_set_pipe_post_crop(hi_vi_pipe vi_pipe, const hi_crop_info *crop_info);
hi_s32 hi_mpi_vi_get_pipe_post_crop(hi_vi_pipe vi_pipe, hi_crop_info *crop_info);

hi_s32 hi_mpi_vi_set_pipe_frame_dump_attr(hi_vi_pipe vi_pipe, const hi_vi_frame_dump_attr *dump_attr);
hi_s32 hi_mpi_vi_get_pipe_frame_dump_attr(hi_vi_pipe vi_pipe, hi_vi_frame_dump_attr *dump_attr);

hi_s32 hi_mpi_vi_get_pipe_frame(hi_vi_pipe vi_pipe, hi_video_frame_info *frame_info, hi_s32 milli_sec);
hi_s32 hi_mpi_vi_release_pipe_frame(hi_vi_pipe vi_pipe, const hi_video_frame_info *frame_info);

hi_s32 hi_mpi_vi_set_pipe_fe_out_frame_dump_attr(hi_vi_pipe vi_pipe, const hi_vi_frame_dump_attr *dump_attr);
hi_s32 hi_mpi_vi_get_pipe_fe_out_frame_dump_attr(hi_vi_pipe vi_pipe, hi_vi_frame_dump_attr *dump_attr);

hi_s32 hi_mpi_vi_get_pipe_fe_out_frame(hi_vi_pipe vi_pipe, hi_video_frame_info *frame_info, hi_s32 milli_sec);
hi_s32 hi_mpi_vi_release_pipe_fe_out_frame(hi_vi_pipe vi_pipe, const hi_video_frame_info *frame_info);

hi_s32 hi_mpi_vi_set_pipe_private_data_dump_attr(hi_vi_pipe vi_pipe, const hi_vi_private_data_dump_attr *dump_attr);
hi_s32 hi_mpi_vi_get_pipe_private_data_dump_attr(hi_vi_pipe vi_pipe, hi_vi_private_data_dump_attr *dump_attr);

hi_s32 hi_mpi_vi_get_pipe_private_data(hi_vi_pipe vi_pipe, hi_vi_private_data_info *data_info, hi_s32 milli_sec);
hi_s32 hi_mpi_vi_release_pipe_private_data(hi_vi_pipe vi_pipe, const hi_vi_private_data_info *data_info);

hi_s32 hi_mpi_vi_set_pipe_bas_frame_dump_attr(hi_vi_pipe vi_pipe, const hi_vi_frame_dump_attr *dump_attr);
hi_s32 hi_mpi_vi_get_pipe_bas_frame_dump_attr(hi_vi_pipe vi_pipe, hi_vi_frame_dump_attr *dump_attr);

hi_s32 hi_mpi_vi_get_pipe_bas_frame(hi_vi_pipe vi_pipe, hi_video_frame_info *frame_info, hi_s32 milli_sec);
hi_s32 hi_mpi_vi_release_pipe_bas_frame(hi_vi_pipe vi_pipe, const hi_video_frame_info *frame_info);

hi_s32 hi_mpi_vi_set_pipe_param(hi_vi_pipe vi_pipe, const hi_vi_pipe_param *pipe_param);
hi_s32 hi_mpi_vi_get_pipe_param(hi_vi_pipe vi_pipe, hi_vi_pipe_param *pipe_param);

hi_s32 hi_mpi_vi_enable_pipe_stagger_out_split(hi_vi_pipe vi_pipe);
hi_s32 hi_mpi_vi_disable_pipe_stagger_out_split(hi_vi_pipe vi_pipe);

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

hi_s32 hi_mpi_vi_set_pipe_low_delay_attr(hi_vi_pipe vi_pipe, const hi_low_delay_info *low_delay_info);
hi_s32 hi_mpi_vi_get_pipe_low_delay_attr(hi_vi_pipe vi_pipe, hi_low_delay_info *low_delay_info);

hi_s32 hi_mpi_vi_set_pipe_frame_interrupt_attr(hi_vi_pipe vi_pipe, const hi_frame_interrupt_attr *interrupt_attr);
hi_s32 hi_mpi_vi_get_pipe_frame_interrupt_attr(hi_vi_pipe vi_pipe, hi_frame_interrupt_attr *interrupt_attr);

hi_s32 hi_mpi_vi_set_pipe_fisheye_cfg(hi_vi_pipe vi_pipe, const hi_fisheye_cfg *fisheye_cfg);
hi_s32 hi_mpi_vi_get_pipe_fisheye_cfg(hi_vi_pipe vi_pipe, hi_fisheye_cfg *fisheye_cfg);

hi_s32 hi_mpi_vi_get_pipe_compress_param(hi_vi_pipe vi_pipe, hi_vi_compress_param *compress_param);

hi_s32 hi_mpi_vi_set_user_pic(hi_vi_pipe vi_pipe, const hi_video_frame_info *user_pic_info);
hi_s32 hi_mpi_vi_enable_user_pic(hi_vi_pipe vi_pipe);
hi_s32 hi_mpi_vi_disable_user_pic(hi_vi_pipe vi_pipe);

hi_s32 hi_mpi_vi_pipe_set_vb_src(hi_vi_pipe vi_pipe, hi_vb_src vb_src);
hi_s32 hi_mpi_vi_pipe_get_vb_src(hi_vi_pipe vi_pipe, hi_vb_src *vb_src);

hi_s32 hi_mpi_vi_pipe_attach_vb_pool(hi_vi_pipe vi_pipe, hi_vb_pool vb_pool);
hi_s32 hi_mpi_vi_pipe_detach_vb_pool(hi_vi_pipe vi_pipe);

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

hi_s32 hi_mpi_vi_set_chn_rotation(hi_vi_pipe vi_pipe, hi_vi_chn vi_chn, const hi_rotation_attr *rotation_attr);
hi_s32 hi_mpi_vi_get_chn_rotation(hi_vi_pipe vi_pipe, hi_vi_chn vi_chn, hi_rotation_attr *rotation_attr);

hi_s32 hi_mpi_vi_set_chn_ldc_attr(hi_vi_pipe vi_pipe, hi_vi_chn vi_chn, const hi_ldc_attr *ldc_attr);
hi_s32 hi_mpi_vi_get_chn_ldc_attr(hi_vi_pipe vi_pipe, hi_vi_chn vi_chn, hi_ldc_attr *ldc_attr);
hi_s32 hi_mpi_vi_ldc_pos_query_dst_to_src(hi_vi_pipe vi_pipe, hi_vi_chn vi_chn, const hi_point *dst_point,
    hi_point *src_point);
hi_s32 hi_mpi_vi_ldc_pos_query_src_to_dst(hi_vi_pipe vi_pipe, hi_vi_chn vi_chn, const hi_point *src_point,
    hi_point *dst_point);

hi_s32 hi_mpi_vi_set_chn_spread_attr(hi_vi_pipe vi_pipe, hi_vi_chn vi_chn, const hi_spread_attr *spread_attr);
hi_s32 hi_mpi_vi_get_chn_spread_attr(hi_vi_pipe vi_pipe, hi_vi_chn vi_chn, hi_spread_attr *spread_attr);

hi_s32 hi_mpi_vi_set_chn_fisheye(hi_vi_pipe vi_pipe, hi_vi_chn vi_chn,
                                 const hi_fisheye_correction_attr *correction_attr);
hi_s32 hi_mpi_vi_get_chn_fisheye(hi_vi_pipe vi_pipe, hi_vi_chn vi_chn, hi_fisheye_correction_attr *correction_attr);
hi_s32 hi_mpi_vi_fisheye_pos_query_dst_to_src(hi_vi_pipe vi_pipe, hi_vi_chn vi_chn, hi_u32 region_index,
                                              const hi_point *dst_point, hi_point *src_point);

hi_s32 hi_mpi_vi_get_chn_rgn_luma(hi_vi_pipe vi_pipe, hi_vi_chn vi_chn, hi_u32 num, const hi_rect *rgn,
                                  hi_u64 *luma_data, hi_s32 milli_sec);

hi_s32 hi_mpi_vi_set_chn_dis_cfg(hi_vi_pipe vi_pipe, hi_vi_chn vi_chn, const hi_dis_cfg *dis_cfg);
hi_s32 hi_mpi_vi_get_chn_dis_cfg(hi_vi_pipe vi_pipe, hi_vi_chn vi_chn, hi_dis_cfg *dis_cfg);
hi_s32 hi_mpi_vi_set_chn_dis_attr(hi_vi_pipe vi_pipe, hi_vi_chn vi_chn, const hi_dis_attr *dis_attr);
hi_s32 hi_mpi_vi_get_chn_dis_attr(hi_vi_pipe vi_pipe, hi_vi_chn vi_chn, hi_dis_attr *dis_attr);
hi_s32 hi_mpi_vi_set_chn_dis_param(hi_vi_pipe vi_pipe, hi_vi_chn vi_chn, const hi_dis_param *dis_param);
hi_s32 hi_mpi_vi_get_chn_dis_param(hi_vi_pipe vi_pipe, hi_vi_chn vi_chn, hi_dis_param *dis_param);

hi_s32 hi_mpi_vi_set_chn_fov_correction_attr(hi_vi_pipe vi_pipe, hi_vi_chn vi_chn,
    const hi_vi_fov_correction_attr *fov_correction_attr);
hi_s32 hi_mpi_vi_get_chn_fov_correction_attr(hi_vi_pipe vi_pipe, hi_vi_chn vi_chn,
    hi_vi_fov_correction_attr *fov_correction_attr);

hi_s32 hi_mpi_vi_get_chn_frame(hi_vi_pipe vi_pipe, hi_vi_chn vi_chn, hi_video_frame_info *frame_info, hi_s32 milli_sec);
hi_s32 hi_mpi_vi_release_chn_frame(hi_vi_pipe vi_pipe, hi_vi_chn vi_chn, const hi_video_frame_info *frame_info);

hi_s32 hi_mpi_vi_set_chn_low_delay_attr(hi_vi_pipe vi_pipe, hi_vi_chn vi_chn, const hi_low_delay_info *low_delay_info);
hi_s32 hi_mpi_vi_get_chn_low_delay_attr(hi_vi_pipe vi_pipe, hi_vi_chn vi_chn, hi_low_delay_info *low_delay_info);

hi_s32 hi_mpi_vi_set_chn_align(hi_vi_pipe vi_pipe, hi_vi_chn vi_chn, hi_u32 align);
hi_s32 hi_mpi_vi_get_chn_align(hi_vi_pipe vi_pipe, hi_vi_chn vi_chn, hi_u32 *align);

hi_s32 hi_mpi_vi_chn_set_vb_src(hi_vi_pipe vi_pipe, hi_vi_chn vi_chn, hi_vb_src vb_src);
hi_s32 hi_mpi_vi_chn_get_vb_src(hi_vi_pipe vi_pipe, hi_vi_chn vi_chn, hi_vb_src *vb_src);

hi_s32 hi_mpi_vi_chn_attach_vb_pool(hi_vi_pipe vi_pipe, hi_vi_chn vi_chn, hi_vb_pool vb_pool);
hi_s32 hi_mpi_vi_chn_detach_vb_pool(hi_vi_pipe vi_pipe, hi_vi_chn vi_chn);

hi_s32 hi_mpi_vi_query_chn_status(hi_vi_pipe vi_pipe, hi_vi_chn vi_chn, hi_vi_chn_status *chn_status);

hi_s32 hi_mpi_vi_get_chn_fd(hi_vi_pipe vi_pipe, hi_vi_chn vi_chn);

/* stitch group interface */
hi_s32 hi_mpi_vi_set_stitch_grp_attr(hi_vi_grp stitch_grp, const hi_vi_stitch_grp_attr *stitch_grp_attr);
hi_s32 hi_mpi_vi_get_stitch_grp_attr(hi_vi_grp stitch_grp, hi_vi_stitch_grp_attr *stitch_grp_attr);

/* vi module interface */
hi_s32 hi_mpi_vi_set_mod_param(const hi_vi_mod_param *mod_param);
hi_s32 hi_mpi_vi_get_mod_param(hi_vi_mod_param *mod_param);

hi_s32 hi_mpi_vi_close_fd(hi_void);

#ifdef __cplusplus
}
#endif
#endif /* __HI_MPI_VI_H__ */
