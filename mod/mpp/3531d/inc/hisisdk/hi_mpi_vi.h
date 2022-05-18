/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2019-2020. All rights reserved.
 * Description: hi_mpi_vi.h
 * Author: Hisilicon multimedia software group
 * Create: 2019-05-21
 */

#ifndef __HI_MPI_VI_H__
#define __HI_MPI_VI_H__

#include "hi_common_vi.h"
#include "hi_common_vb.h"

#ifdef __cplusplus
extern "C" {
#endif

hi_s32 hi_mpi_vi_set_dev_attr(hi_vi_dev vi_dev, const hi_vi_dev_attr *dev_attr);
hi_s32 hi_mpi_vi_get_dev_attr(hi_vi_dev vi_dev, hi_vi_dev_attr *dev_attr);

hi_s32 hi_mpi_vi_enable_dev(hi_vi_dev vi_dev);
hi_s32 hi_mpi_vi_disable_dev(hi_vi_dev vi_dev);

hi_s32 hi_mpi_vi_set_chn_attr(hi_vi_chn vi_chn, const hi_vi_chn_attr *attr);
hi_s32 hi_mpi_vi_get_chn_attr(hi_vi_chn vi_chn, hi_vi_chn_attr *attr);

hi_s32 hi_mpi_vi_set_minor_chn_attr(hi_vi_chn vi_chn, const hi_vi_chn_attr *attr);
hi_s32 hi_mpi_vi_get_minor_chn_attr(hi_vi_chn vi_chn, hi_vi_chn_attr *attr);

hi_s32 hi_mpi_vi_enable_chn(hi_vi_chn vi_chn);
hi_s32 hi_mpi_vi_disable_chn(hi_vi_chn vi_chn);

hi_s32 hi_mpi_vi_get_frame(hi_vi_chn vi_chn, hi_video_frame_info *frame_info, hi_s32 milli_sec);
hi_s32 hi_mpi_vi_release_frame(hi_vi_chn vi_chn, hi_video_frame_info *frame_info);
hi_s32 hi_mpi_vi_set_frame_depth(hi_vi_chn vi_chn, hi_u32 depth);
hi_s32 hi_mpi_vi_get_frame_depth(hi_vi_chn vi_chn, hi_u32 *depth);

hi_s32 hi_mpi_vi_set_user_pic(hi_vi_chn vi_chn, hi_vi_user_pic_attr *user_pic);
hi_s32 hi_mpi_vi_enable_user_pic(hi_vi_chn vi_chn);
hi_s32 hi_mpi_vi_disable_user_pic(hi_vi_chn vi_chn);

/*
 * these functions are used to start the cascade mode.
 * VI cascade mode can work normally only when they have been called.
 */
hi_s32 hi_mpi_vi_set_vbi_attr(hi_vi_chn vi_chn, hi_vi_vbi_arg *vbi_attr);
hi_s32 hi_mpi_vi_get_vbi_attr(hi_vi_chn vi_chn, hi_vi_vbi_arg *vbi_attr);
hi_s32 hi_mpi_vi_set_vbi_mode(hi_vi_chn vi_chn, hi_vi_vbi_mode mode);
hi_s32 hi_mpi_vi_get_vbi_mode(hi_vi_chn vi_chn, hi_vi_vbi_mode *mode);
hi_s32 hi_mpi_vi_enable_vbi(hi_vi_chn vi_chn);
hi_s32 hi_mpi_vi_disable_vbi(hi_vi_chn vi_chn);
hi_s32 hi_mpi_vi_enable_cas_chn(hi_vi_chn vi_chn);
hi_s32 hi_mpi_vi_disable_cas_chn(hi_vi_chn vi_chn);

/* normally, these functions are not necessary in typical business */
hi_s32 hi_mpi_vi_bind_chn(hi_vi_chn vi_chn, const hi_vi_chn_bind_attr *chn_bind_attr);
hi_s32 hi_mpi_vi_unbind_chn(hi_vi_chn vi_chn);
hi_s32 hi_mpi_vi_get_chn_bind(hi_vi_chn vi_chn, hi_vi_chn_bind_attr *chn_bind_attr);

hi_s32 hi_mpi_vi_set_dev_attr_ex(hi_vi_dev vi_dev, const hi_vi_dev_attr_ex *dev_attr_ex);
hi_s32 hi_mpi_vi_get_dev_attr_ex(hi_vi_dev vi_dev, hi_vi_dev_attr_ex *dev_attr_ex);

hi_s32 hi_mpi_vi_get_fd(hi_vi_chn vi_chn);
hi_s32 hi_mpi_vi_close_fd(hi_void);

hi_s32 hi_mpi_vi_query(hi_vi_chn vi_chn, hi_vi_chn_status *status);

hi_s32 hi_mpi_vi_enable_chn_interrupt(hi_vi_chn vi_chn);
hi_s32 hi_mpi_vi_disable_chn_interrupt(hi_vi_chn vi_chn);

hi_s32 hi_mpi_vi_get_chn_luma(hi_vi_chn vi_chn, hi_vi_chn_luma *luma);

hi_s32 hi_mpi_vi_set_skip_mode(hi_vi_chn vi_chn, hi_vi_skip_mode skip_mode);
hi_s32 hi_mpi_vi_get_skip_mode(hi_vi_chn vi_chn, hi_vi_skip_mode *skip_mode);

hi_s32 hi_mpi_vi_enable_dll_slave(hi_vi_dev vi_dev);
hi_s32 hi_mpi_vi_disable_dll_slave(hi_vi_dev vi_dev);

hi_s32 hi_mpi_vi_set_skip_mode_ex(hi_vi_chn vi_chn, hi_vi_skip_mode_ex *mode_ex);
hi_s32 hi_mpi_vi_get_skip_mode_ex(hi_vi_chn vi_chn, hi_vi_skip_mode_ex *mode_ex);

hi_s32 hi_mpi_vi_attach_vb_pool(hi_vi_chn vi_chn, hi_vb_pool pool);
hi_s32 hi_mpi_vi_detach_vb_pool(hi_vi_chn vi_chn);
hi_s32 hi_mpi_vi_detach_vb_pool_by_pool_id(hi_vi_chn vi_chn, hi_vb_pool pool);

hi_s32 hi_mpi_vi_set_chn_vb_src(hi_vi_chn vi_chn, hi_vb_src vb_src);
hi_s32 hi_mpi_vi_get_chn_vb_src(hi_vi_chn vi_chn, hi_vb_src *vb_src);

hi_s32 hi_mpi_vi_set_mod_param(const hi_vi_mod_param *mod_param);
hi_s32 hi_mpi_vi_get_mod_param(hi_vi_mod_param *mod_param);

hi_s32 hi_mpi_vi_set_rotation(hi_vi_chn vi_chn, const hi_rotation rotation);
hi_s32 hi_mpi_vi_get_rotation(hi_vi_chn vi_chn, hi_rotation *rotation);

hi_s32 hi_mpi_vi_set_frame_interrupt_attr(hi_vi_chn vi_chn, const hi_frame_interrupt_attr *frame_interrupt_attr);
hi_s32 hi_mpi_vi_get_frame_interrupt_attr(hi_vi_chn vi_chn, hi_frame_interrupt_attr *frame_interrupt_attr);

#ifdef __cplusplus
}
#endif

#endif