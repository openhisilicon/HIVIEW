/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */

#ifndef __HI_MPI_AUDIO_H__
#define __HI_MPI_AUDIO_H__

#include "hi_type.h"
#include "hi_common.h"
#include "hi_common_aio.h"
#include "hi_common_aenc.h"
#include "hi_common_adec.h"


#ifdef __cplusplus
extern "C" {
#endif

#define HI_AENC_ADAPT_MAGIC OT_AENC_ADAPT_MAGIC

/* Audio function api. */
hi_s32 hi_mpi_audio_set_mod_param(const hi_audio_mod_param *mod_param);
hi_s32 hi_mpi_audio_get_mod_param(hi_audio_mod_param *mod_param);

hi_s32 hi_mpi_audio_register_vqe_mod(const hi_audio_vqe_register *vqe_register);

/* AI function api. */
hi_s32 hi_mpi_ai_set_pub_attr(hi_audio_dev ai_dev, const hi_aio_attr *attr);
hi_s32 hi_mpi_ai_get_pub_attr(hi_audio_dev ai_dev, hi_aio_attr *attr);
hi_s32 hi_mpi_ai_clr_pub_attr(hi_audio_dev ai_dev);

hi_s32 hi_mpi_ai_enable(hi_audio_dev ai_dev);
hi_s32 hi_mpi_ai_disable(hi_audio_dev ai_dev);

hi_s32 hi_mpi_ai_set_chn_attr(hi_audio_dev ai_dev, hi_ai_chn ai_chn, const hi_ai_chn_attr *chn_attr);
hi_s32 hi_mpi_ai_get_chn_attr(hi_audio_dev ai_dev, hi_ai_chn ai_chn, hi_ai_chn_attr *chn_attr);

hi_s32 hi_mpi_ai_set_chn_param(hi_audio_dev ai_dev, hi_ai_chn ai_chn, const hi_ai_chn_param *chn_param);
hi_s32 hi_mpi_ai_get_chn_param(hi_audio_dev ai_dev, hi_ai_chn ai_chn, hi_ai_chn_param *chn_param);

hi_s32 hi_mpi_ai_enable_chn(hi_audio_dev ai_dev, hi_ai_chn ai_chn);
hi_s32 hi_mpi_ai_disable_chn(hi_audio_dev ai_dev, hi_ai_chn ai_chn);

hi_s32 hi_mpi_ai_get_frame(hi_audio_dev ai_dev, hi_ai_chn ai_chn,
                           hi_audio_frame *frame, hi_aec_frame *aec_frame, hi_s32 milli_sec);
hi_s32 hi_mpi_ai_release_frame(hi_audio_dev ai_dev, hi_ai_chn ai_chn,
                               const hi_audio_frame *frame, const hi_aec_frame *aec_frame);

hi_s32 hi_mpi_ai_set_talk_vqe_v2_attr(hi_audio_dev ai_dev, hi_ai_chn ai_chn,
    hi_audio_dev ao_dev, hi_ao_chn ao_chn, const hi_ai_talk_vqe_v2_cfg *vqe_cfg);
hi_s32 hi_mpi_ai_get_talk_vqe_v2_attr(hi_audio_dev ai_dev, hi_ai_chn ai_chn, hi_ai_talk_vqe_v2_cfg *vqe_cfg);

hi_s32 hi_mpi_ai_set_record_vqe_attr(hi_audio_dev ai_dev, hi_ai_chn ai_chn, const hi_ai_record_vqe_cfg *vqe_cfg);
hi_s32 hi_mpi_ai_get_record_vqe_attr(hi_audio_dev ai_dev, hi_ai_chn ai_chn, hi_ai_record_vqe_cfg *vqe_cfg);

hi_s32 hi_mpi_ai_set_talk_vqe_attr(hi_audio_dev ai_dev, hi_ai_chn ai_chn,
                                   hi_audio_dev ao_dev, hi_ao_chn ao_chn, const hi_ai_talk_vqe_cfg *vqe_cfg);
hi_s32 hi_mpi_ai_get_talk_vqe_attr(hi_audio_dev ai_dev, hi_ai_chn ai_chn, hi_ai_talk_vqe_cfg *vqe_cfg);

hi_s32 hi_mpi_ai_enable_vqe(hi_audio_dev ai_dev, hi_ai_chn ai_chn);
hi_s32 hi_mpi_ai_disable_vqe(hi_audio_dev ai_dev, hi_ai_chn ai_chn);

hi_s32 hi_mpi_ai_enable_resample(hi_audio_dev ai_dev, hi_ai_chn ai_chn, hi_audio_sample_rate out_sample_rate);
hi_s32 hi_mpi_ai_disable_resample(hi_audio_dev ai_dev, hi_ai_chn ai_chn);

hi_s32 hi_mpi_ai_set_track_mode(hi_audio_dev ai_dev, hi_audio_track_mode track_mode);
hi_s32 hi_mpi_ai_get_track_mode(hi_audio_dev ai_dev, hi_audio_track_mode *track_mode);

hi_s32 hi_mpi_ai_save_file(hi_audio_dev ai_dev, hi_ai_chn ai_chn, const hi_audio_save_file_info *save_file_info);
hi_s32 hi_mpi_ai_query_file_status(hi_audio_dev ai_dev, hi_ai_chn ai_chn, hi_audio_file_status *file_status);

hi_s32 hi_mpi_ai_get_fd(hi_audio_dev ai_dev, hi_ai_chn ai_chn);

hi_s32 hi_mpi_ai_enable_aec_ref_frame(hi_audio_dev ai_dev, hi_ai_chn ai_chn, hi_audio_dev ao_dev, hi_ao_chn ao_chn);
hi_s32 hi_mpi_ai_disable_aec_ref_frame(hi_audio_dev ai_dev, hi_ai_chn ai_chn);

/* AO function api. */
hi_s32 hi_mpi_ao_set_pub_attr(hi_audio_dev ao_dev, const hi_aio_attr *attr);
hi_s32 hi_mpi_ao_get_pub_attr(hi_audio_dev ao_dev, hi_aio_attr *attr);
hi_s32 hi_mpi_ao_clr_pub_attr(hi_audio_dev ao_dev);

hi_s32 hi_mpi_ao_enable(hi_audio_dev ao_dev);
hi_s32 hi_mpi_ao_disable(hi_audio_dev ao_dev);

hi_s32 hi_mpi_ao_enable_chn(hi_audio_dev ao_dev, hi_ao_chn ao_chn);
hi_s32 hi_mpi_ao_disable_chn(hi_audio_dev ao_dev, hi_ao_chn ao_chn);

hi_s32 hi_mpi_ao_pause_chn(hi_audio_dev ao_dev, hi_ao_chn ao_chn);
hi_s32 hi_mpi_ao_resume_chn(hi_audio_dev ao_dev, hi_ao_chn ao_chn);

hi_s32 hi_mpi_ao_send_frame(hi_audio_dev ao_dev, hi_ao_chn ao_chn, const hi_audio_frame *data, hi_s32 milli_sec);

hi_s32 hi_mpi_ao_enable_resample(hi_audio_dev ao_dev, hi_ao_chn ao_chn, hi_audio_sample_rate in_sample_rate);
hi_s32 hi_mpi_ao_disable_resample(hi_audio_dev ao_dev, hi_ao_chn ao_chn);

hi_s32 hi_mpi_ao_clr_chn_buf(hi_audio_dev ao_dev, hi_ao_chn ao_chn);
hi_s32 hi_mpi_ao_query_chn_status(hi_audio_dev ao_dev, hi_ao_chn ao_chn, hi_ao_chn_state *status);

hi_s32 hi_mpi_ao_get_chn_delay(hi_audio_dev ao_dev, hi_ao_chn ao_chn, hi_u32 *milli_sec);

hi_s32 hi_mpi_ao_set_volume(hi_audio_dev ao_dev, hi_s32 volume_db);
hi_s32 hi_mpi_ao_get_volume(hi_audio_dev ao_dev, hi_s32 *volume_db);

hi_s32 hi_mpi_ao_set_mute(hi_audio_dev ao_dev, hi_bool enable, const hi_audio_fade *fade);
hi_s32 hi_mpi_ao_get_mute(hi_audio_dev ao_dev, hi_bool *enable, hi_audio_fade *fade);

hi_s32 hi_mpi_ao_set_track_mode(hi_audio_dev ao_dev, hi_audio_track_mode track_mode);
hi_s32 hi_mpi_ao_get_track_mode(hi_audio_dev ao_dev, hi_audio_track_mode *track_mode);

hi_s32 hi_mpi_ao_get_fd(hi_audio_dev ao_dev, hi_ao_chn ao_chn);

hi_s32 hi_mpi_ao_set_vqe_attr(hi_audio_dev ao_dev, hi_ao_chn ao_chn, const hi_ao_vqe_cfg *vqe_cfg);
hi_s32 hi_mpi_ao_get_vqe_attr(hi_audio_dev ao_dev, hi_ao_chn ao_chn, hi_ao_vqe_cfg *vqe_cfg);

hi_s32 hi_mpi_ao_enable_vqe(hi_audio_dev ao_dev, hi_ao_chn ao_chn);
hi_s32 hi_mpi_ao_disable_vqe(hi_audio_dev ao_dev, hi_ao_chn ao_chn);

hi_s32 hi_mpi_ao_save_file(hi_audio_dev ao_dev, hi_ao_chn ao_chn, const hi_audio_save_file_info *save_file_info);
hi_s32 hi_mpi_ao_query_file_status(hi_audio_dev ao_dev, hi_ao_chn ao_chn, hi_audio_file_status *file_status);

/* AENC function api. */
hi_s32 hi_mpi_aenc_register_encoder(hi_s32 *handle, const hi_aenc_encoder *encoder);
hi_s32 hi_mpi_aenc_unregister_encoder(hi_s32 handle);

hi_s32 hi_mpi_aenc_create_chn(hi_aenc_chn aenc_chn, const hi_aenc_chn_attr *attr);
hi_s32 hi_mpi_aenc_destroy_chn(hi_aenc_chn aenc_chn);

hi_s32 hi_mpi_aenc_send_frame(hi_aenc_chn aenc_chn, const hi_audio_frame *frame, const hi_aec_frame *aec_frame);

hi_s32 hi_mpi_aenc_get_stream(hi_aenc_chn aenc_chn, hi_audio_stream *stream, hi_s32 milli_sec);
hi_s32 hi_mpi_aenc_release_stream(hi_aenc_chn aenc_chn, const hi_audio_stream *stream);

hi_s32 hi_mpi_aenc_get_fd(hi_aenc_chn aenc_chn);

hi_s32 hi_mpi_aenc_get_stream_buf_info(hi_aenc_chn aenc_chn, hi_phys_addr_t *phys_addr, hi_u32 *size);

hi_s32 hi_mpi_aenc_set_mute(hi_aenc_chn aenc_chn, hi_bool enable);
hi_s32 hi_mpi_aenc_get_mute(hi_aenc_chn aenc_chn, hi_bool *enable);

hi_s32 hi_mpi_aenc_clr_chn_buf(hi_aenc_chn aenc_chn);

/* ADEC function api. */
hi_s32 hi_mpi_adec_register_decoder(hi_s32 *handle, const hi_adec_decoder *decoder);
hi_s32 hi_mpi_adec_unregister_decoder(hi_s32 handle);

hi_s32 hi_mpi_adec_create_chn(hi_adec_chn adec_chn, const hi_adec_chn_attr *attr);
hi_s32 hi_mpi_adec_destroy_chn(hi_adec_chn adec_chn);

hi_s32 hi_mpi_adec_send_stream(hi_adec_chn adec_chn, const hi_audio_stream *stream, hi_bool block);

hi_s32 hi_mpi_adec_clr_chn_buf(hi_adec_chn adec_chn);

hi_s32 hi_mpi_adec_get_frame(hi_adec_chn adec_chn, hi_audio_frame_info *frame_info, hi_bool block);
hi_s32 hi_mpi_adec_release_frame(hi_adec_chn adec_chn, const hi_audio_frame_info *frame_info);

hi_s32 hi_mpi_adec_send_end_of_stream(hi_adec_chn adec_chn, hi_bool instant);
hi_s32 hi_mpi_adec_query_chn_status(hi_adec_chn adec_chn, hi_adec_chn_state *buf_status);

hi_s32 hi_mpi_adec_get_frame_info(hi_adec_chn adec_chn, hi_void *decode_frame_info);


#ifdef __cplusplus
}
#endif
#endif /* __HI_MPI_AUDIO_H__ */
