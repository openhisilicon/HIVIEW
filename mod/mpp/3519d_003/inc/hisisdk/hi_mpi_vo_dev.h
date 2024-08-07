/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */

#ifndef __HI_MPI_VO_DEV_H__
#define __HI_MPI_VO_DEV_H__

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

hi_s32 hi_mpi_vo_set_layer_priority(hi_vo_layer layer, hi_u32 priority);
hi_s32 hi_mpi_vo_get_layer_priority(hi_vo_layer layer, hi_u32 *priority);
hi_s32 hi_mpi_vo_bind_layer(hi_vo_layer layer, hi_vo_dev dev);
hi_s32 hi_mpi_vo_unbind_layer(hi_vo_layer layer, hi_vo_dev dev);

#ifdef __cplusplus
}
#endif
#endif /* __HI_MPI_VO_DEV_H__ */
