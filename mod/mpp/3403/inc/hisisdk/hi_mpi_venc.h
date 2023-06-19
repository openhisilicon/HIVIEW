/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */

#ifndef HI_MPI_VENC_H
#define HI_MPI_VENC_H

#include "hi_common.h"
#include "hi_common_video.h"
#include "hi_common_venc.h"
#include "hi_common_vb.h"


#ifdef __cplusplus
extern "C" {
#endif

hi_s32 hi_mpi_venc_create_chn(hi_venc_chn chn, const hi_venc_chn_attr *attr);
hi_s32 hi_mpi_venc_destroy_chn(hi_venc_chn chn);
hi_s32 hi_mpi_venc_reset_chn(hi_venc_chn chn);
hi_s32 hi_mpi_venc_start_chn(hi_venc_chn chn, const hi_venc_start_param *recv_param);
hi_s32 hi_mpi_venc_stop_chn(hi_venc_chn chn);

hi_s32 hi_mpi_venc_set_chn_attr(hi_venc_chn chn, const hi_venc_chn_attr *chn_attr);
hi_s32 hi_mpi_venc_get_chn_attr(hi_venc_chn chn, hi_venc_chn_attr *chn_attr);
hi_s32 hi_mpi_venc_set_chn_param(hi_venc_chn chn, const hi_venc_chn_param *chn_param);
hi_s32 hi_mpi_venc_get_chn_param(hi_venc_chn chn, hi_venc_chn_param *chn_param);
hi_s32 hi_mpi_venc_query_status(hi_venc_chn chn, hi_venc_chn_status *status);
hi_s32 hi_mpi_venc_get_stream(hi_venc_chn chn, hi_venc_stream *stream, hi_s32 milli_sec);
hi_s32 hi_mpi_venc_release_stream(hi_venc_chn chn, const hi_venc_stream *stream);
hi_s32 hi_mpi_venc_get_stream_buf_info(hi_venc_chn chn, hi_venc_stream_buf_info *stream_buf_info);

hi_s32 hi_mpi_venc_insert_user_data(hi_venc_chn chn, hi_u8 *data, hi_u32 len);
hi_s32 hi_mpi_venc_send_frame(hi_venc_chn chn, const hi_video_frame_info *frame, hi_s32 milli_sec);
hi_s32 hi_mpi_venc_send_frame_ex(hi_venc_chn chn, const hi_venc_user_frame_info *frame, hi_s32 milli_sec);
hi_s32 hi_mpi_venc_send_multi_frame(hi_venc_chn chn, const hi_venc_multi_frame_info *frame, hi_s32 milli_sec);

hi_s32 hi_mpi_venc_set_chn_config(hi_venc_chn chn, const hi_venc_chn_config *chn_config);
hi_s32 hi_mpi_venc_get_chn_config(hi_venc_chn chn, hi_venc_chn_config *chn_config);

hi_s32 hi_mpi_venc_request_idr(hi_venc_chn chn, hi_bool instant);
hi_s32 hi_mpi_venc_enable_idr(hi_venc_chn chn, hi_bool enable_idr);

hi_s32 hi_mpi_venc_get_fd(hi_venc_chn chn);
hi_s32 hi_mpi_venc_close_fd(hi_venc_chn chn);

hi_s32 hi_mpi_venc_set_mod_param(const hi_venc_mod_param *mod_param);
hi_s32 hi_mpi_venc_get_mod_param(hi_venc_mod_param *mod_param);

hi_s32 hi_mpi_venc_set_roi_attr(hi_venc_chn chn, const hi_venc_roi_attr *roi_attr);
hi_s32 hi_mpi_venc_get_roi_attr(hi_venc_chn chn, hi_u32 idx, hi_venc_roi_attr *roi_attr);
hi_s32 hi_mpi_venc_set_roi_attr_ex(hi_venc_chn chn, const hi_venc_roi_attr_ex *roi_attr_ex);
hi_s32 hi_mpi_venc_get_roi_attr_ex(hi_venc_chn chn, hi_u32 idx, hi_venc_roi_attr_ex *roi_attr_ex);
hi_s32 hi_mpi_venc_set_jpeg_roi_attr(hi_venc_chn chn, const hi_venc_jpeg_roi_attr *roi_attr);
hi_s32 hi_mpi_venc_get_jpeg_roi_attr(hi_venc_chn chn, hi_u32 idx, hi_venc_jpeg_roi_attr *roi_attr);
hi_s32 hi_mpi_venc_set_roi_bg_frame_rate(hi_venc_chn chn, const hi_venc_roi_bg_frame_rate *roi_bg_frame_rate);
hi_s32 hi_mpi_venc_get_roi_bg_frame_rate(hi_venc_chn chn, hi_venc_roi_bg_frame_rate *roi_bg_frame_rate);

hi_s32 hi_mpi_venc_set_h264_intra_pred(hi_venc_chn chn, const hi_venc_h264_intra_pred *h264_intra_pred);
hi_s32 hi_mpi_venc_get_h264_intra_pred(hi_venc_chn chn, hi_venc_h264_intra_pred *h264_intra_pred);
hi_s32 hi_mpi_venc_set_h264_trans(hi_venc_chn chn, const hi_venc_h264_trans *h264_trans);
hi_s32 hi_mpi_venc_get_h264_trans(hi_venc_chn chn, hi_venc_h264_trans *h264_trans);
hi_s32 hi_mpi_venc_set_h264_entropy(hi_venc_chn chn, const hi_venc_h264_entropy *h264_entropy);
hi_s32 hi_mpi_venc_get_h264_entropy(hi_venc_chn chn, hi_venc_h264_entropy *h264_entropy);
hi_s32 hi_mpi_venc_set_h264_dblk(hi_venc_chn chn, const hi_venc_h264_dblk *h264_dblk);
hi_s32 hi_mpi_venc_get_h264_dblk(hi_venc_chn chn, hi_venc_h264_dblk *h264_dblk);
hi_s32 hi_mpi_venc_set_h264_vui(hi_venc_chn chn, const hi_venc_h264_vui *h264_vui);
hi_s32 hi_mpi_venc_get_h264_vui(hi_venc_chn chn, hi_venc_h264_vui *h264_vui);
hi_s32 hi_mpi_venc_set_h264_poc(hi_venc_chn chn, const hi_venc_h264_poc *h264_poc);
hi_s32 hi_mpi_venc_get_h264_poc(hi_venc_chn chn, hi_venc_h264_poc *h264_poc);

hi_s32 hi_mpi_venc_set_h265_vui(hi_venc_chn chn, const hi_venc_h265_vui *h265_vui);
hi_s32 hi_mpi_venc_get_h265_vui(hi_venc_chn chn, hi_venc_h265_vui *h265_vui);
hi_s32 hi_mpi_venc_set_h265_pu(hi_venc_chn chn, const hi_venc_h265_pu *pu);
hi_s32 hi_mpi_venc_get_h265_pu(hi_venc_chn chn, hi_venc_h265_pu *pu);
hi_s32 hi_mpi_venc_set_h265_trans(hi_venc_chn chn, const hi_venc_h265_trans *h265_trans);
hi_s32 hi_mpi_venc_get_h265_trans(hi_venc_chn chn, hi_venc_h265_trans *h265_trans);
hi_s32 hi_mpi_venc_set_h265_entropy(hi_venc_chn chn, const hi_venc_h265_entropy *h265_entropy);
hi_s32 hi_mpi_venc_get_h265_entropy(hi_venc_chn chn, hi_venc_h265_entropy *h265_entropy);
hi_s32 hi_mpi_venc_set_h265_dblk(hi_venc_chn chn, const hi_venc_h265_dblk *h265_dblk);
hi_s32 hi_mpi_venc_get_h265_dblk(hi_venc_chn chn, hi_venc_h265_dblk *h265_dblk);
hi_s32 hi_mpi_venc_set_h265_sao(hi_venc_chn chn, const hi_venc_h265_sao *h265_sao);
hi_s32 hi_mpi_venc_get_h265_sao(hi_venc_chn chn, hi_venc_h265_sao *h265_sao);

hi_s32 hi_mpi_venc_set_jpeg_param(hi_venc_chn chn, const hi_venc_jpeg_param *jpeg_param);
hi_s32 hi_mpi_venc_get_jpeg_param(hi_venc_chn chn, hi_venc_jpeg_param *jpeg_param);
hi_s32 hi_mpi_venc_set_mjpeg_param(hi_venc_chn chn, const hi_venc_mjpeg_param *mjpeg_param);
hi_s32 hi_mpi_venc_get_mjpeg_param(hi_venc_chn chn, hi_venc_mjpeg_param *mjpeg_param);
hi_s32 hi_mpi_venc_set_jpeg_dering_level(hi_venc_chn chn, const hi_venc_jpeg_dering_level dering_level);
hi_s32 hi_mpi_venc_get_jpeg_dering_level(hi_venc_chn chn, hi_venc_jpeg_dering_level *dering_level);
hi_s32 hi_mpi_venc_set_jpeg_enc_mode(hi_venc_chn chn, const hi_venc_jpeg_enc_mode enc_mode);
hi_s32 hi_mpi_venc_get_jpeg_enc_mode(hi_venc_chn chn, hi_venc_jpeg_enc_mode *enc_mode);

hi_s32 hi_mpi_venc_set_rc_param(hi_venc_chn chn, const hi_venc_rc_param *rc_param);
hi_s32 hi_mpi_venc_get_rc_param(hi_venc_chn chn, hi_venc_rc_param *rc_param);
hi_s32 hi_mpi_venc_set_rc_adv_param(hi_venc_chn chn, const hi_venc_rc_adv_param *rc_adv_param);
hi_s32 hi_mpi_venc_get_rc_adv_param(hi_venc_chn chn, hi_venc_rc_adv_param *rc_adv_param);
hi_s32 hi_mpi_venc_set_frame_lost_strategy(hi_venc_chn chn, const hi_venc_frame_lost_strategy *lost_param);
hi_s32 hi_mpi_venc_get_frame_lost_strategy(hi_venc_chn chn, hi_venc_frame_lost_strategy *lost_param);
hi_s32 hi_mpi_venc_set_super_frame_strategy(hi_venc_chn chn, const hi_venc_super_frame_strategy *super_frame_param);
hi_s32 hi_mpi_venc_get_super_frame_strategy(hi_venc_chn chn, hi_venc_super_frame_strategy *super_frame_param);
hi_s32 hi_mpi_venc_set_scene_mode(hi_venc_chn chn, const hi_venc_scene_mode scene_mode);
hi_s32 hi_mpi_venc_get_scene_mode(hi_venc_chn chn, hi_venc_scene_mode *scene_mode);

hi_s32 hi_mpi_venc_attach_vb_pool(hi_venc_chn chn, const hi_venc_chn_pool *pool);
hi_s32 hi_mpi_venc_detach_vb_pool(hi_venc_chn chn);

hi_s32 hi_mpi_venc_set_ref_param(hi_venc_chn chn, const hi_venc_ref_param *ref_param);
hi_s32 hi_mpi_venc_get_ref_param(hi_venc_chn chn, hi_venc_ref_param *ref_param);
hi_s32 hi_mpi_venc_set_intra_refresh(hi_venc_chn chn, const hi_venc_intra_refresh *intra_refresh);
hi_s32 hi_mpi_venc_get_intra_refresh(hi_venc_chn chn, hi_venc_intra_refresh *intra_refresh);
hi_s32 hi_mpi_venc_set_sse_rgn(hi_venc_chn chn, const hi_venc_sse_rgn *sse_cfg);
hi_s32 hi_mpi_venc_get_sse_rgn(hi_venc_chn chn, hi_u32 idx, hi_venc_sse_rgn *sse_cfg);
hi_s32 hi_mpi_venc_set_fg_protect(hi_venc_chn chn, const hi_venc_fg_protect *fg_protect);
hi_s32 hi_mpi_venc_get_fg_protect(hi_venc_chn chn, hi_venc_fg_protect *fg_protect);
hi_s32 hi_mpi_venc_set_cu_pred(hi_venc_chn chn, const hi_venc_cu_pred *cu_pred);
hi_s32 hi_mpi_venc_get_cu_pred(hi_venc_chn chn, hi_venc_cu_pred *cu_pred);
hi_s32 hi_mpi_venc_set_skip_bias(hi_venc_chn chn, const hi_venc_skip_bias *skip_bias);
hi_s32 hi_mpi_venc_get_skip_bias(hi_venc_chn chn, hi_venc_skip_bias *skip_bias);
hi_s32 hi_mpi_venc_set_debreath_effect(hi_venc_chn chn, const hi_venc_debreath_effect *debreath_effect);
hi_s32 hi_mpi_venc_get_debreath_effect(hi_venc_chn chn, hi_venc_debreath_effect *debreath_effect);
hi_s32 hi_mpi_venc_set_hierarchical_qp(hi_venc_chn chn, const hi_venc_hierarchical_qp *hierarchical_qp);
hi_s32 hi_mpi_venc_get_hierarchical_qp(hi_venc_chn chn, hi_venc_hierarchical_qp *hierarchical_qp);
hi_s32 hi_mpi_venc_set_slice_split(hi_venc_chn chn, const hi_venc_slice_split *slice_split);
hi_s32 hi_mpi_venc_get_slice_split(hi_venc_chn chn, hi_venc_slice_split *slice_split);
hi_s32 hi_mpi_venc_set_param_set_id(hi_venc_chn chn, const hi_venc_param_set_id *param_set_id);
hi_s32 hi_mpi_venc_get_param_set_id(hi_venc_chn chn, hi_venc_param_set_id *param_set_id);
hi_s32 hi_mpi_venc_set_search_window(hi_venc_chn chn, const hi_venc_search_window *search_window);
hi_s32 hi_mpi_venc_get_search_window(hi_venc_chn chn, hi_venc_search_window *search_window);

hi_s32 hi_mpi_venc_enable_svc(hi_venc_chn chn, hi_bool enable);
hi_s32 hi_mpi_venc_set_svc_param(hi_venc_chn chn, const hi_venc_svc_param *svc_param);
hi_s32 hi_mpi_venc_get_svc_param(hi_venc_chn chn, hi_venc_svc_param *svc_param);
hi_s32 hi_mpi_venc_send_svc_region(hi_venc_chn chn, const hi_venc_svc_rect_info *svc_region);

hi_s32 hi_mpi_venc_set_deblur(hi_venc_chn chn, const hi_venc_deblur_param *deblur_param);
hi_s32 hi_mpi_venc_get_deblur(hi_venc_chn chn, hi_venc_deblur_param *deblur_param);
hi_s32 hi_mpi_venc_set_md(hi_venc_chn chn, const hi_venc_md_param *md_param);
hi_s32 hi_mpi_venc_get_md(hi_venc_chn chn, hi_venc_md_param *md_param);
td_s32 hi_mpi_venc_omx_send_frame(hi_venc_chn chn, const hi_omx_user_frame_info *frame, hi_s32 milli_sec);

#ifdef __cplusplus
}
#endif
#endif /* HI_MPI_VENC_H */
