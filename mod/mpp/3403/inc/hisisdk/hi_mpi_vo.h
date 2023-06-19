/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */

#ifndef __HI_MPI_VO_H__
#define __HI_MPI_VO_H__

#include "hi_common_vo.h"


#ifdef __cplusplus
extern "C" {
#endif

hi_s32 hi_mpi_vo_enable(hi_vo_dev dev);
hi_s32 hi_mpi_vo_disable(hi_vo_dev dev);
hi_s32 hi_mpi_vo_set_pub_attr(hi_vo_dev dev, const hi_vo_pub_attr *pub_attr);
hi_s32 hi_mpi_vo_get_pub_attr(hi_vo_dev dev, hi_vo_pub_attr *pub_attr);
hi_s32 hi_mpi_vo_set_dev_param(hi_vo_dev dev, const hi_vo_dev_param *dev_param);
hi_s32 hi_mpi_vo_get_dev_param(hi_vo_dev dev, hi_vo_dev_param *dev_param);
hi_s32 hi_mpi_vo_close_fd(hi_void);
hi_s32 hi_mpi_vo_set_dev_frame_rate(hi_vo_dev dev, hi_u32 frame_rate);
hi_s32 hi_mpi_vo_get_dev_frame_rate(hi_vo_dev dev, hi_u32 *frame_rate);
hi_s32 hi_mpi_vo_set_user_sync_info(hi_vo_dev dev, const hi_vo_user_sync_info *sync_info);
hi_s32 hi_mpi_vo_get_user_sync_info(hi_vo_dev dev, hi_vo_user_sync_info *sync_info);
hi_s32 hi_mpi_vo_set_mod_param(const hi_vo_mod_param *mod_param);
hi_s32 hi_mpi_vo_get_mod_param(hi_vo_mod_param *mod_param);
hi_s32 hi_mpi_vo_set_vtth(hi_vo_dev dev, hi_u32 vtth);
hi_s32 hi_mpi_vo_get_vtth(hi_vo_dev dev, hi_u32 *vtth);
hi_s32 hi_mpi_vo_set_less_buf_attr(hi_vo_dev dev, const hi_vo_less_buf_attr *less_buf_attr);
hi_s32 hi_mpi_vo_get_less_buf_attr(hi_vo_dev dev, hi_vo_less_buf_attr *less_buf_attr);
hi_s32 hi_mpi_vo_set_user_notify_attr(hi_vo_dev dev, const hi_vo_user_notify_attr *user_notify_attr);
hi_s32 hi_mpi_vo_get_user_notify_attr(hi_vo_dev dev, hi_vo_user_notify_attr *user_notify_attr);

hi_s32 hi_mpi_vo_query_intf_status(hi_vo_intf_type intf_type, hi_vo_intf_status *status);
hi_s32 hi_mpi_vo_set_vga_param(hi_vo_dev dev, const hi_vo_vga_param *vga_param);
hi_s32 hi_mpi_vo_get_vga_param(hi_vo_dev dev, hi_vo_vga_param *vga_param);
hi_s32 hi_mpi_vo_set_hdmi_param(hi_vo_dev dev, const hi_vo_hdmi_param *hdmi_param);
hi_s32 hi_mpi_vo_get_hdmi_param(hi_vo_dev dev, hi_vo_hdmi_param *hdmi_param);
hi_s32 hi_mpi_vo_set_hdmi1_param(hi_vo_dev dev, const hi_vo_hdmi_param *hdmi1_param);
hi_s32 hi_mpi_vo_get_hdmi1_param(hi_vo_dev dev, hi_vo_hdmi_param *hdmi1_param);
hi_s32 hi_mpi_vo_set_rgb_param(hi_vo_dev dev, const hi_vo_rgb_param *rgb_param);
hi_s32 hi_mpi_vo_get_rgb_param(hi_vo_dev dev, hi_vo_rgb_param *rgb_param);
hi_s32 hi_mpi_vo_set_bt_param(hi_vo_dev dev, const hi_vo_bt_param *bt_param);
hi_s32 hi_mpi_vo_get_bt_param(hi_vo_dev dev, hi_vo_bt_param *bt_param);
hi_s32 hi_mpi_vo_set_mipi_param(hi_vo_dev dev, const hi_vo_mipi_param *mipi_param);
hi_s32 hi_mpi_vo_get_mipi_param(hi_vo_dev dev, hi_vo_mipi_param *mipi_param);

hi_s32 hi_mpi_vo_enable_video_layer(hi_vo_layer layer);
hi_s32 hi_mpi_vo_disable_video_layer(hi_vo_layer layer);
hi_s32 hi_mpi_vo_set_video_layer_attr(hi_vo_layer layer, const hi_vo_video_layer_attr *layer_attr);
hi_s32 hi_mpi_vo_get_video_layer_attr(hi_vo_layer layer, hi_vo_video_layer_attr *layer_attr);
hi_s32 hi_mpi_vo_set_video_layer_param(hi_vo_layer layer, const hi_vo_layer_param *layer_param);
hi_s32 hi_mpi_vo_get_video_layer_param(hi_vo_layer layer, hi_vo_layer_param *layer_param);
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

hi_s32 hi_mpi_vo_set_layer_priority(hi_vo_layer layer, hi_u32 priority);
hi_s32 hi_mpi_vo_get_layer_priority(hi_vo_layer layer, hi_u32 *priority);
hi_s32 hi_mpi_vo_set_layer_csc(hi_vo_layer layer, const hi_vo_csc *csc);
hi_s32 hi_mpi_vo_get_layer_csc(hi_vo_layer layer, hi_vo_csc *csc);
hi_s32 hi_mpi_vo_bind_layer(hi_vo_layer layer, hi_vo_dev dev);
hi_s32 hi_mpi_vo_unbind_layer(hi_vo_layer layer, hi_vo_dev dev);

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

hi_s32 hi_mpi_vo_set_wbc_src(hi_vo_wbc wbc, const hi_vo_wbc_src *wbc_src);
hi_s32 hi_mpi_vo_get_wbc_src(hi_vo_wbc wbc, hi_vo_wbc_src *wbc_src);
hi_s32 hi_mpi_vo_enable_wbc(hi_vo_wbc wbc);
hi_s32 hi_mpi_vo_disable_wbc(hi_vo_wbc wbc);
hi_s32 hi_mpi_vo_set_wbc_attr(hi_vo_wbc wbc, const hi_vo_wbc_attr *wbc_attr);
hi_s32 hi_mpi_vo_get_wbc_attr(hi_vo_wbc wbc, hi_vo_wbc_attr *wbc_attr);
hi_s32 hi_mpi_vo_set_wbc_mode(hi_vo_wbc wbc, hi_vo_wbc_mode wbc_mode);
hi_s32 hi_mpi_vo_get_wbc_mode(hi_vo_wbc wbc, hi_vo_wbc_mode *wbc_mode);
hi_s32 hi_mpi_vo_set_wbc_depth(hi_vo_wbc wbc, hi_u32 depth);
hi_s32 hi_mpi_vo_get_wbc_depth(hi_vo_wbc wbc, hi_u32 *depth);
hi_s32 hi_mpi_vo_get_wbc_frame(hi_vo_wbc wbc, hi_video_frame_info *frame_info, hi_s32 milli_sec);
hi_s32 hi_mpi_vo_release_wbc_frame(hi_vo_wbc wbc, const hi_video_frame_info *frame_info);

hi_s32 hi_mpi_vo_set_cas_attr(const hi_vo_cas_attr *cas_attr);
hi_s32 hi_mpi_vo_get_cas_attr(hi_vo_cas_attr *cas_attr);
hi_s32 hi_mpi_vo_enable_cas_dev(hi_vo_dev cas_dev);
hi_s32 hi_mpi_vo_disable_cas_dev(hi_vo_dev cas_dev);
hi_s32 hi_mpi_vo_set_cas_pattern(hi_vo_dev cas_dev, hi_u32 pattern);
hi_s32 hi_mpi_vo_get_cas_pattern(hi_vo_dev cas_dev, hi_u32 *pattern);
hi_s32 hi_mpi_vo_bind_cas_chn(hi_u32 pos, hi_vo_dev cas_dev, hi_vo_chn chn);
hi_s32 hi_mpi_vo_unbind_cas_chn(hi_u32 pos, hi_vo_dev cas_dev, hi_vo_chn chn);
hi_s32 hi_mpi_vo_enable_cas(hi_void);
hi_s32 hi_mpi_vo_disable_cas(hi_void);

#ifdef __cplusplus
}
#endif
#endif /* __HI_MPI_VO_H__ */
