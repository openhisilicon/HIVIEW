/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */

#ifndef HI_MPI_VO_H
#define HI_MPI_VO_H

#include "hi_common_vo.h"

#ifdef __cplusplus
extern "C" {
#endif
hi_s32 hi_mpi_vo_enable_video_layer(hi_vo_layer layer);
hi_s32 hi_mpi_vo_disable_video_layer(hi_vo_layer layer);
hi_s32 hi_mpi_vo_set_video_layer_attr(hi_vo_layer layer, const hi_vo_video_layer_attr *layer_attr);
hi_s32 hi_mpi_vo_get_video_layer_attr(hi_vo_layer layer, hi_vo_video_layer_attr *layer_attr);
hi_s32 hi_mpi_vo_set_video_layer_param(hi_vo_layer layer, const hi_vo_layer_param *layer_param);
hi_s32 hi_mpi_vo_get_video_layer_param(hi_vo_layer layer, hi_vo_layer_param *layer_param);
hi_s32 hi_mpi_vo_set_video_layer_mirror(hi_vo_layer layer, hi_vo_mirror_mode mirror_mode);
hi_s32 hi_mpi_vo_get_video_layer_mirror(hi_vo_layer layer, hi_vo_mirror_mode *mirror_mode);
hi_s32 hi_mpi_vo_set_video_layer_crop(hi_vo_layer layer, const hi_crop_info *crop_info);
hi_s32 hi_mpi_vo_get_video_layer_crop(hi_vo_layer layer, hi_crop_info *crop_info);
hi_s32 hi_mpi_vo_set_video_layer_early_display(hi_vo_layer layer, hi_bool early_display_en);
hi_s32 hi_mpi_vo_get_video_layer_early_display(hi_vo_layer layer, hi_bool *early_display_en);
hi_s32 hi_mpi_vo_set_play_toleration(hi_vo_layer layer, hi_u32 toleration);
hi_s32 hi_mpi_vo_get_play_toleration(hi_vo_layer layer, hi_u32 *toleration);
hi_s32 hi_mpi_vo_batch_begin(hi_vo_layer layer);
hi_s32 hi_mpi_vo_batch_end(hi_vo_layer layer);
hi_s32 hi_mpi_vo_get_screen_frame(hi_vo_layer layer, hi_video_frame_info *frame_info,
    hi_s32 milli_sec);
hi_s32 hi_mpi_vo_release_screen_frame(hi_vo_layer layer, const hi_video_frame_info *frame_info);

hi_s32 hi_mpi_vo_set_layer_hsharpen(hi_vo_layer layer, const hi_vo_layer_hsharpen_param *hsharpen_param);
hi_s32 hi_mpi_vo_get_layer_hsharpen(hi_vo_layer layer, hi_vo_layer_hsharpen_param *hsharpen_param);

hi_s32 hi_mpi_vo_set_layer_csc(hi_vo_layer layer, const hi_vo_csc *csc);
hi_s32 hi_mpi_vo_get_layer_csc(hi_vo_layer layer, hi_vo_csc *csc);

hi_s32 hi_mpi_vo_enable_chn(hi_vo_layer layer, hi_vo_chn chn);
hi_s32 hi_mpi_vo_disable_chn(hi_vo_layer layer, hi_vo_chn chn);
hi_s32 hi_mpi_vo_set_chn_attr(hi_vo_layer layer, hi_vo_chn chn, const hi_vo_chn_attr *chn_attr);
hi_s32 hi_mpi_vo_get_chn_attr(hi_vo_layer layer, hi_vo_chn chn, hi_vo_chn_attr *chn_attr);
hi_s32 hi_mpi_vo_set_chn_param(hi_vo_layer layer, hi_vo_chn chn, const hi_vo_chn_param *chn_param);
hi_s32 hi_mpi_vo_get_chn_param(hi_vo_layer layer, hi_vo_chn chn, hi_vo_chn_param *chn_param);
hi_s32 hi_mpi_vo_set_chn_display_pos(hi_vo_layer layer, hi_vo_chn chn, const hi_point *display_pos);
hi_s32 hi_mpi_vo_get_chn_display_pos(hi_vo_layer layer, hi_vo_chn chn, hi_point *display_pos);
hi_s32 hi_mpi_vo_set_chn_frame_rate(hi_vo_layer layer, hi_vo_chn chn, hi_s32 frame_rate);
hi_s32 hi_mpi_vo_get_chn_frame_rate(hi_vo_layer layer, hi_vo_chn chn, hi_s32 *frame_rate);
hi_s32 hi_mpi_vo_get_chn_frame(hi_vo_layer layer, hi_vo_chn chn, hi_video_frame_info *frame_info,
    hi_s32 milli_sec);
hi_s32 hi_mpi_vo_release_chn_frame(hi_vo_layer layer, hi_vo_chn chn,
    const hi_video_frame_info *frame_info);
hi_s32 hi_mpi_vo_pause_chn(hi_vo_layer layer, hi_vo_chn chn);
hi_s32 hi_mpi_vo_resume_chn(hi_vo_layer layer, hi_vo_chn chn);
hi_s32 hi_mpi_vo_step_chn(hi_vo_layer layer, hi_vo_chn chn);
hi_s32 hi_mpi_vo_refresh_chn(hi_vo_layer layer, hi_vo_chn chn);
hi_s32 hi_mpi_vo_show_chn(hi_vo_layer layer, hi_vo_chn chn);
hi_s32 hi_mpi_vo_hide_chn(hi_vo_layer layer, hi_vo_chn chn);
hi_s32 hi_mpi_vo_set_zoom_in_window(hi_vo_layer layer, hi_vo_chn chn,
    const hi_vo_zoom_attr *zoom_attr);
hi_s32 hi_mpi_vo_get_zoom_in_window(hi_vo_layer layer, hi_vo_chn chn, hi_vo_zoom_attr *zoom_attr);
hi_s32 hi_mpi_vo_get_chn_pts(hi_vo_layer layer, hi_vo_chn chn, hi_u64 *chn_pts);
hi_s32 hi_mpi_vo_query_chn_status(hi_vo_layer layer, hi_vo_chn chn, hi_vo_chn_status *status);
hi_s32 hi_mpi_vo_send_frame(hi_vo_layer layer, hi_vo_chn chn, const hi_video_frame_info *frame_info,
    hi_s32 milli_sec);
hi_s32 hi_mpi_vo_clear_chn_buf(hi_vo_layer layer, hi_vo_chn chn, hi_bool is_clear_all);
hi_s32 hi_mpi_vo_set_chn_border(hi_vo_layer layer, hi_vo_chn chn, const hi_vo_border *border);
hi_s32 hi_mpi_vo_get_chn_border(hi_vo_layer layer, hi_vo_chn chn, hi_vo_border *border);
hi_s32 hi_mpi_vo_set_chn_recv_threshold(hi_vo_layer layer, hi_vo_chn chn, hi_u32 threshold);
hi_s32 hi_mpi_vo_get_chn_recv_threshold(hi_vo_layer layer, hi_vo_chn chn, hi_u32 *threshold);
hi_s32 hi_mpi_vo_set_chn_rotation(hi_vo_layer layer, hi_vo_chn chn, hi_rotation rotation);
hi_s32 hi_mpi_vo_get_chn_rotation(hi_vo_layer layer, hi_vo_chn chn, hi_rotation *rotation);
hi_s32 hi_mpi_vo_set_chn_mirror(hi_vo_layer layer, hi_vo_chn chn, hi_vo_mirror_mode mirror_mode);
hi_s32 hi_mpi_vo_get_chn_mirror(hi_vo_layer layer, hi_vo_chn chn, hi_vo_mirror_mode *mirror_mode);
hi_s32 hi_mpi_vo_get_chn_rgn_luma(hi_vo_layer layer, hi_vo_chn chn, hi_u32 num, const hi_rect *rgn,
    hi_u64 *luma_data, hi_s32 milli_sec);

#ifdef __cplusplus
}
#endif
#endif /* HI_MPI_VO_H */
