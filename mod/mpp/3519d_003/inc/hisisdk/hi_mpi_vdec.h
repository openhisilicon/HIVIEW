/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */

#ifndef HI_MPI_VDEC_H
#define HI_MPI_VDEC_H

#include "hi_common.h"
#include "hi_common_video.h"
#include "hi_common_vdec.h"

#ifdef __cplusplus
extern "C" {
#endif

hi_s32 hi_mpi_vdec_set_mod_param(const hi_vdec_mod_param *mod_param);
hi_s32 hi_mpi_vdec_get_mod_param(hi_vdec_mod_param *mod_param);

hi_s32 hi_mpi_vdec_create_chn(hi_vdec_chn chn, const hi_vdec_chn_attr *attr);
hi_s32 hi_mpi_vdec_destroy_chn(hi_vdec_chn chn);
hi_s32 hi_mpi_vdec_reset_chn(hi_vdec_chn chn);

hi_s32 hi_mpi_vdec_get_chn_attr(hi_vdec_chn chn, hi_vdec_chn_attr *attr);
hi_s32 hi_mpi_vdec_set_chn_attr(hi_vdec_chn chn, const hi_vdec_chn_attr *attr);

hi_s32 hi_mpi_vdec_start_recv_stream(hi_vdec_chn chn);
hi_s32 hi_mpi_vdec_stop_recv_stream(hi_vdec_chn chn);

hi_s32 hi_mpi_vdec_query_status(hi_vdec_chn chn, hi_vdec_chn_status *status);

hi_s32 hi_mpi_vdec_send_stream(hi_vdec_chn chn, const hi_vdec_stream *stream, hi_s32 milli_sec);

hi_s32 hi_mpi_vdec_get_frame(hi_vdec_chn chn, hi_video_frame_info *frame_info,
    hi_vdec_supplement_info *supplement,  hi_s32 milli_sec);
hi_s32 hi_mpi_vdec_release_frame(hi_vdec_chn chn, const hi_video_frame_info *frame_info);

hi_s32 hi_mpi_vdec_set_chn_param(hi_vdec_chn chn, const hi_vdec_chn_param *chn_param);
hi_s32 hi_mpi_vdec_get_chn_param(hi_vdec_chn chn, hi_vdec_chn_param *chn_param);

hi_s32 hi_mpi_vdec_set_protocol_param(hi_vdec_chn chn, const hi_vdec_protocol_param *protocol_param);
hi_s32 hi_mpi_vdec_get_protocol_param(hi_vdec_chn chn, hi_vdec_protocol_param *protocol_param);

hi_s32 hi_mpi_vdec_set_chn_config(hi_vdec_chn chn, const hi_vdec_chn_config *chn_config);
hi_s32 hi_mpi_vdec_get_chn_config(hi_vdec_chn chn, hi_vdec_chn_config *chn_config);

hi_s32 hi_mpi_vdec_get_user_data(hi_vdec_chn chn, hi_vdec_user_data *user_data, hi_s32 milli_sec);
hi_s32 hi_mpi_vdec_release_user_data(hi_vdec_chn chn, const hi_vdec_user_data *user_data);

hi_s32 hi_mpi_vdec_set_user_pic(hi_vdec_chn chn, const hi_video_frame_info *user_pic);
hi_s32 hi_mpi_vdec_enable_user_pic(hi_vdec_chn chn, hi_bool instant);
hi_s32 hi_mpi_vdec_disable_user_pic(hi_vdec_chn chn);

hi_s32 hi_mpi_vdec_set_display_mode(hi_vdec_chn chn, hi_video_display_mode display_mode);
hi_s32 hi_mpi_vdec_get_display_mode(hi_vdec_chn chn, hi_video_display_mode *display_mode);

hi_s32 hi_mpi_vdec_set_rotation(hi_vdec_chn chn, hi_rotation rotation);
hi_s32 hi_mpi_vdec_get_rotation(hi_vdec_chn chn, hi_rotation *rotation);

hi_s32 hi_mpi_vdec_attach_vb_pool(hi_vdec_chn chn, const hi_vdec_chn_pool *pool);
hi_s32 hi_mpi_vdec_detach_vb_pool(hi_vdec_chn chn);

hi_s32 hi_mpi_vdec_get_fd(hi_vdec_chn chn);

hi_s32 hi_mpi_vdec_set_low_delay(hi_vdec_chn chn, const hi_low_delay_info *low_delay_attr);
hi_s32 hi_mpi_vdec_get_low_delay(hi_vdec_chn chn, hi_low_delay_info *low_delay_attr);

#ifdef __cplusplus
}
#endif
#endif /* HI_MPI_VDEC_H */
