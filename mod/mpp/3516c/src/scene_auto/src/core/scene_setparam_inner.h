/*
  Copyright (c), 2001-2024, Shenshu Tech. Co., Ltd.
 */

#ifndef SCENE_SETPARAM_INNER_H
#define SCENE_SETPARAM_INNER_H

#include "ot_scene_setparam.h"


#ifdef __cplusplus
extern "C" {
#endif

#define PIC_WIDTH_1080P 1920
#define PIC_HEIGHT_1080P 1080
#define LCU_ALIGN_H265 64
#define MB_ALIGN_H264 16

#define scene_div_0to1(a) (((a) == 0) ? 1 : (a))

#define SCENE_SHADING_TRIGMODE_L2H 1

#define SCENE_SHADING_TRIGMODE_H2L 2


ot_scene_pipe_param *get_pipe_params(hi_void);
hi_bool *get_isp_pause(hi_void);

#define check_scene_return_if_pause() do {   \
        if (*(get_isp_pause()) == HI_TRUE) { \
            return HI_SUCCESS;               \
        }                                    \
    } while (0)

#define check_scene_ret(ret) do {                                                    \
        if ((ret) != HI_SUCCESS) {                                                   \
            printf("Failed at %s: LINE: %d with %#x!", __FUNCTION__, __LINE__, ret); \
        }                                                                            \
    } while (0)

hi_u32 scene_get_level_ltoh(hi_u64 value, hi_u32 count, const hi_u64 *thresh, hi_u32 thresh_size);
hi_u32 scene_get_level_ltoh_u32(hi_u32 value, hi_u32 count, const hi_u32 *thresh, hi_u32 thresh_size);
hi_u32 scene_interpulate(hi_u64 middle, hi_u64 left, hi_u64 left_value, hi_u64 right, hi_u64 right_value);
hi_u32 scene_time_filter(hi_u32 param0, hi_u32 param1, hi_u32 time_cnt, hi_u32 index);
hi_void scene_set_static_h265_avbr(hi_venc_rc_param *rc_param, hi_u8 index);
hi_void scene_set_static_h265_cvbr(hi_venc_rc_param *rc_param, hi_u8 index);
hi_void scene_set_isp_attr(set_isp_attr_param param, const hi_u32 *ratio_level_thresh,
    const hi_u32 *iso_level_thresh, hi_isp_drc_attr *isp_drc_attr);
hi_s32 scene_blend_tone_mapping(hi_vi_pipe vi_pipe, hi_u8 index,
    ot_scene_dynamic_drc_coef *drc_coef, hi_isp_drc_attr *isp_drc_attr);
hi_s32 scene_set_tone_mapping_value(hi_vi_pipe vi_pipe, hi_u8 index, hi_u32 k,
    const hi_isp_inner_state_info *inner_state_info, hi_isp_drc_attr *isp_drc_attr);
hi_s32 scene_set_isp_drc_attr(hi_u8 index, hi_u64 iso, hi_isp_drc_attr *isp_drc_attr);
hi_s32 scene_set_nr_attr_para(hi_u8 index, hi_u32 iso, hi_isp_nr_attr *nr_attr, const ot_scene_nr_para *nr_para);
hi_s32 scene_set_nr_wdr_ratio_para(hi_vi_pipe vi_pipe, hi_u8 index, hi_u32 wdr_ratio, hi_u32 ratio_index,
    hi_isp_nr_attr *nr_attr);
hi_s32 scene_set_3dnr(hi_vi_pipe vi_pipe, const ot_scene_3dnr *_3dnr, hi_u8 index);
hi_void scene_set_3dnr_nrx_nry(const _3dnr_nrx_pack *pack);
hi_void scene_set_3dnr_nrx_iey(const _3dnr_nrx_pack *pack);
hi_void scene_set_3dnr_nrx_sfy(const _3dnr_nrx_pack *pack);
hi_void scene_set_3dnr_nrx_tfy(const _3dnr_nrx_pack *pack);
hi_void scene_set_3dnr_nrx_mdy(const _3dnr_nrx_pack *pack);
hi_void scene_set_3dnr_nrx_nrc0(const _3dnr_nrx_pack *pack);
hi_void scene_set_3dnr_nrx_nrc1(const _3dnr_nrx_pack *pack);
hi_void scene_set_3dnr_sfy_lut(const _3dnr_nrx_pack *pack);

hi_s32 scene_set_qp(hi_u32 pic_width, hi_u32 pic_height, hi_u32 max_bitrate, hi_payload_type type,
    hi_venc_rc_param *rc_param);
hi_s32 get_iso_mean_qp_chn_attr(hi_vi_pipe vi_pipe, hi_u32 *iso, hi_u32 *mean_qp, hi_venc_chn_attr *venc_chn_attr);
hi_void calculate_manual_percent_index(hi_u8 pipe_param_index, hi_u32 sum_iso, hi_u32 *out_index);
hi_void set_initial_percent(hi_payload_type type, hi_u32 index, hi_venc_rc_param *rc_param,
    const ot_scene_pipe_param *param, hi_s32 *percent);
hi_void set_min_qp_delta_when_iso_larger(hi_payload_type type, hi_u32 sum_mean_qp, hi_s32 percent,
    hi_venc_rc_param *rc_param);
hi_void set_min_qp_delta_when_iso_less(hi_payload_type type, hi_u32 sum_mean_qp, hi_venc_rc_param *rc_param,
    hi_venc_chn_attr *venc_chn_attr);


#ifdef __cplusplus
}
#endif

#endif
