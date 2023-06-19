/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */

#include "ot_scene_setparam.h"
#include "scene_setparam_inner.h"
#include <unistd.h>
#include <string.h>
#include "securec.h"
#include "hi_mpi_awb.h"
#include "hi_mpi_ae.h"
#include "hi_mpi_isp.h"
#include "hi_mpi_venc.h"
#include "hi_common_rc.h"
#include "sample_comm.h"

#ifdef MCF_SCENE_AUTO_USED
#include "hi_mpi_mcf.h"
#endif

#ifdef HNR_SCENE_AUTO_USED
#include "hi_mpi_hnr.h"
#endif

#include "ot_scenecomm.h"

#ifdef __cplusplus
extern "C" {
#endif

hi_char g_dir_name[DIR_NAME_BUFF];

hi_void set_dir_name(const hi_char *dir_name)
{
    snprintf_truncated_s(g_dir_name, DIR_NAME_BUFF, "%s", dir_name);
}

static sample_vi_fpn_correction_cfg g_correction_cfg = {
    .op_mode       = HI_OP_MODE_MANUAL,
    .fpn_type      = HI_ISP_FPN_TYPE_FRAME,
    .strength      = FPN_CORRECTION_STR,
    .pixel_format  = HI_PIXEL_FORMAT_RGB_BAYER_16BPP,
    .compress_mode = HI_COMPRESS_MODE_NONE,
};

hi_s32 ot_scene_set_ae_weight_table(hi_vi_pipe vi_pipe, hi_u8 index)
{
    ot_scenecomm_expr_true_return(index >= HI_SCENE_PIPETYPE_NUM, HI_FAILURE);
    hi_s32 i, j;
    hi_s32 ret;
    hi_isp_stats_cfg isp_stats_cfg;

    ret = hi_mpi_isp_get_stats_cfg(vi_pipe, &isp_stats_cfg);
    check_scene_ret(ret);
    for (i = 0; i < HI_ISP_AE_ZONE_ROW; i++) {
        for (j = 0; j < HI_ISP_AE_ZONE_COLUMN; j++) {
            isp_stats_cfg.ae_cfg.weight[i][j] = get_pipe_params()[index].static_statistics.ae_weight[i][j];
        }
    }
    ret = hi_mpi_isp_set_stats_cfg(vi_pipe, &isp_stats_cfg);
    check_scene_ret(ret);

    return HI_SUCCESS;
}

hi_s32 ot_scene_set_static_ae(hi_vi_pipe vi_pipe, hi_u8 index)
{
    ot_scenecomm_expr_true_return(index >= HI_SCENE_PIPETYPE_NUM, HI_FAILURE);
    if (get_pipe_params()[index].module_state.static_ae != HI_TRUE) {
        return HI_SUCCESS;
    }
    hi_s32 ret;
    hi_isp_exposure_attr exposure_attr;
    hi_isp_ae_route_ex ae_route_ex;

    ret = hi_mpi_isp_get_ae_route_attr_ex(vi_pipe, &ae_route_ex);
    check_scene_ret(ret);
    ae_route_ex.total_num = get_pipe_params()[index].static_ae_route_ex.total_num;
    for (hi_u32 i = 0; i < ae_route_ex.total_num; i++) {
        ae_route_ex.route_ex_node[i].int_time = get_pipe_params()[index].static_ae_route_ex.int_time[i];
        ae_route_ex.route_ex_node[i].a_gain = get_pipe_params()[index].static_ae_route_ex.again[i];
        ae_route_ex.route_ex_node[i].d_gain = get_pipe_params()[index].static_ae_route_ex.dgain[i];
        ae_route_ex.route_ex_node[i].isp_d_gain = get_pipe_params()[index].static_ae_route_ex.isp_dgain[i];
    }
    ret = hi_mpi_isp_set_ae_route_attr_ex(vi_pipe, &ae_route_ex);
    check_scene_ret(ret);

    ret = hi_mpi_isp_get_exposure_attr(vi_pipe, &exposure_attr);
    check_scene_ret(ret);

    exposure_attr.ae_route_ex_valid = get_pipe_params()[index].static_ae.ae_route_ex_valid;
    exposure_attr.ae_run_interval = get_pipe_params()[index].static_ae.ae_run_interval;
    exposure_attr.auto_attr.sys_gain_range.max = get_pipe_params()[index].static_ae.auto_sys_gain_max;
    exposure_attr.auto_attr.exp_time_range.max = get_pipe_params()[index].static_ae.auto_exp_time_max;
    exposure_attr.auto_attr.speed = get_pipe_params()[index].static_ae.auto_speed;
    exposure_attr.auto_attr.tolerance = get_pipe_params()[index].static_ae.auto_tolerance;
    exposure_attr.auto_attr.ae_delay_attr.black_delay_frame = get_pipe_params()[index].static_ae.auto_black_delay_frame;
    exposure_attr.auto_attr.ae_delay_attr.white_delay_frame = get_pipe_params()[index].static_ae.auto_white_delay_frame;

    ret = hi_mpi_isp_set_exposure_attr(vi_pipe, &exposure_attr);
    check_scene_ret(ret);

    if (get_pipe_params()[index].module_state.ae_weight_tab) {
        ret = ot_scene_set_ae_weight_table(vi_pipe, index);
        check_scene_ret(ret);
    }
    return HI_SUCCESS;
}

hi_s32 ot_scene_set_static_wdr_exposure(hi_vi_pipe vi_pipe, hi_u8 index)
{
    ot_scenecomm_expr_true_return(index >= HI_SCENE_PIPETYPE_NUM, HI_FAILURE);
    if (get_pipe_params()[index].module_state.static_wdr_exposure != HI_TRUE) {
        return HI_SUCCESS;
    }

    hi_s32 i, ret;
    hi_isp_wdr_exposure_attr wdr_exposure_attr;

    ret = hi_mpi_isp_get_wdr_exposure_attr(vi_pipe, &wdr_exposure_attr);
    check_scene_ret(ret);

    wdr_exposure_attr.exp_ratio_type = (hi_op_mode)get_pipe_params()[index].static_wdr_exposure.exp_ratio_type;
    wdr_exposure_attr.exp_ratio_max = get_pipe_params()[index].static_wdr_exposure.exp_ratio_max;
    wdr_exposure_attr.exp_ratio_min = get_pipe_params()[index].static_wdr_exposure.exp_ratio_min;
    wdr_exposure_attr.high_light_target = get_pipe_params()[index].static_wdr_exposure.high_light_target;
    wdr_exposure_attr.exp_coef_min = get_pipe_params()[index].static_wdr_exposure.exp_coef_min;

    for (i = 0; i < HI_ISP_EXP_RATIO_NUM; i++) {
        wdr_exposure_attr.exp_ratio[i] = get_pipe_params()[index].static_wdr_exposure.exp_ratio[i];
    }

    ret = hi_mpi_isp_set_wdr_exposure_attr(vi_pipe, &wdr_exposure_attr);
    check_scene_ret(ret);

    return HI_SUCCESS;
}

hi_s32 ot_scene_set_static_fswdr(hi_vi_pipe vi_pipe, hi_u8 index)
{
    if (get_pipe_params()[index].module_state.static_fswdr != HI_TRUE) {
        return HI_SUCCESS;
    }

    hi_s32 ret;
    hi_isp_wdr_fs_attr fswdr_attr;

    check_scene_return_if_pause();
    ret = hi_mpi_isp_get_fswdr_attr(vi_pipe, &fswdr_attr);
    check_scene_ret(ret);

    fswdr_attr.wdr_merge_mode = get_pipe_params()[index].static_fswdr.wdr_merge_mode;

    check_scene_return_if_pause();
    ret = hi_mpi_isp_set_fswdr_attr(vi_pipe, &fswdr_attr);
    check_scene_ret(ret);

    return HI_SUCCESS;
}

hi_s32 ot_scene_set_static_awb(hi_vi_pipe vi_pipe, hi_u8 index)
{
    ot_scenecomm_expr_true_return(index >= HI_SCENE_PIPETYPE_NUM, HI_FAILURE);
    if (get_pipe_params()[index].module_state.static_awb != HI_TRUE) {
        return HI_SUCCESS;
    }

    hi_s32 i, ret;
    hi_isp_wb_attr wb_attr;
    hi_isp_stats_cfg isp_stats_cfg;

    ret = hi_mpi_isp_get_wb_attr(vi_pipe, &wb_attr);
    check_scene_ret(ret);
    ret = hi_mpi_isp_get_stats_cfg(vi_pipe, &isp_stats_cfg);
    check_scene_ret(ret);

    wb_attr.op_type = get_pipe_params()[index].static_awb.op_type;
    wb_attr.manual_attr.r_gain = get_pipe_params()[index].static_awb.manual_rgain;
    wb_attr.manual_attr.gr_gain = get_pipe_params()[index].static_awb.manual_grgain;
    wb_attr.manual_attr.gb_gain = get_pipe_params()[index].static_awb.manual_gbgain;
    wb_attr.manual_attr.b_gain = get_pipe_params()[index].static_awb.manual_bgain;

    for (i = 0; i < HI_ISP_BAYER_CHN_NUM; i++) {
        wb_attr.auto_attr.static_wb[i] = get_pipe_params()[index].static_awb.auto_static_wb[i];
    }

    for (i = 0; i < HI_ISP_AWB_CURVE_PARA_NUM; i++) {
        wb_attr.auto_attr.curve_para[i] = get_pipe_params()[index].static_awb.auto_curve_para[i];
    }

    wb_attr.auto_attr.speed = get_pipe_params()[index].static_awb.auto_speed;
    wb_attr.auto_attr.low_color_temp = get_pipe_params()[index].static_awb.auto_low_color_temp;

    for (i = 0; i < HI_ISP_AUTO_ISO_NUM; i++) {
        wb_attr.auto_attr.cb_cr_track.cr_max[i] = get_pipe_params()[index].static_awb.auto_cr_max[i];
        wb_attr.auto_attr.cb_cr_track.cr_min[i] = get_pipe_params()[index].static_awb.auto_cr_min[i];
        wb_attr.auto_attr.cb_cr_track.cb_max[i] = get_pipe_params()[index].static_awb.auto_cb_max[i];
        wb_attr.auto_attr.cb_cr_track.cb_min[i] = get_pipe_params()[index].static_awb.auto_cb_min[i];
    }
    wb_attr.auto_attr.luma_hist.enable = get_pipe_params()[index].static_awb.luma_hist_enable;

    isp_stats_cfg.wb_cfg.awb_switch = get_pipe_params()[index].static_awb.awb_switch;
    isp_stats_cfg.wb_cfg.black_level = get_pipe_params()[index].static_awb.black_level;
    ret = hi_mpi_isp_set_wb_attr(vi_pipe, &wb_attr);
    check_scene_ret(ret);
    ret = hi_mpi_isp_set_stats_cfg(vi_pipe, &isp_stats_cfg);
    check_scene_ret(ret);

    return HI_SUCCESS;
}

hi_s32 ot_scene_set_static_awbex(hi_vi_pipe vi_pipe, hi_u8 index)
{
    ot_scenecomm_expr_true_return(index >= HI_SCENE_PIPETYPE_NUM, HI_FAILURE);
    if (get_pipe_params()[index].module_state.static_awbex != HI_TRUE) {
        return HI_SUCCESS;
    }

    hi_s32 i, ret;
    hi_isp_wb_attr wb_attr;
    hi_isp_awb_attr_ex awb_attr_ex;

    ret = hi_mpi_isp_get_wb_attr(vi_pipe, &wb_attr);
    check_scene_ret(ret);
    wb_attr.bypass = get_pipe_params()[index].static_awb_ex.bypass;
    ret = hi_mpi_isp_set_wb_attr(vi_pipe, &wb_attr);
    check_scene_ret(ret);

    ret = hi_mpi_isp_get_awb_attr_ex(vi_pipe, &awb_attr_ex);
    check_scene_ret(ret);

    awb_attr_ex.tolerance = get_pipe_params()[index].static_awb_ex.tolerance;
    awb_attr_ex.in_or_out.out_thresh = get_pipe_params()[index].static_awb_ex.out_thresh;
    awb_attr_ex.in_or_out.low_stop = get_pipe_params()[index].static_awb_ex.low_stop;
    awb_attr_ex.in_or_out.high_start = get_pipe_params()[index].static_awb_ex.high_start;
    awb_attr_ex.in_or_out.high_stop = get_pipe_params()[index].static_awb_ex.high_stop;
    awb_attr_ex.multi_light_source_en = get_pipe_params()[index].static_awb_ex.multi_light_source_en;

    for (i = 0; i < HI_ISP_AWB_MULTI_CT_NUM; i++) {
        awb_attr_ex.multi_ct_wt[i] = get_pipe_params()[index].static_awb_ex.multi_ctwt[i];
    }
    ret = hi_mpi_isp_set_awb_attr_ex(vi_pipe, &awb_attr_ex);
    check_scene_ret(ret);
    return HI_SUCCESS;
}

hi_s32 ot_scene_set_saturation(hi_vi_pipe vi_pipe, hi_u8 index)
{
    if (get_pipe_params()[index].module_state.static_saturation != HI_TRUE) {
        return HI_SUCCESS;
    }

    hi_s32 i, ret;
    hi_isp_saturation_attr saturation_attr;

    ret = hi_mpi_isp_get_saturation_attr(vi_pipe, &saturation_attr);
    check_scene_ret(ret);

    for (i = 0; i < HI_ISP_AUTO_ISO_NUM; i++) {
        saturation_attr.auto_attr.sat[i] = get_pipe_params()[index].static_saturation.auto_sat[i];
    }
    ret = hi_mpi_isp_set_saturation_attr(vi_pipe, &saturation_attr);
    check_scene_ret(ret);
    return HI_SUCCESS;
}

hi_s32 ot_scene_set_static_ccm(hi_vi_pipe vi_pipe, hi_u8 index)
{
    ot_scenecomm_expr_true_return(index >= HI_SCENE_PIPETYPE_NUM, HI_FAILURE);
    if (get_pipe_params()[index].module_state.static_ccm != HI_TRUE) {
        return HI_SUCCESS;
    }

    hi_s32 i, j, ret;
    hi_isp_color_matrix_attr color_matrix_attr;

    ret = hi_mpi_isp_get_ccm_attr(vi_pipe, &color_matrix_attr);
    check_scene_ret(ret);
    color_matrix_attr.op_type = (hi_op_mode)get_pipe_params()[index].static_ccm.ccm_op_type;

    for (i = 0; i < HI_ISP_CCM_MATRIX_SIZE; i++) {
        color_matrix_attr.manual_attr.ccm[i] = get_pipe_params()[index].static_ccm.manual_ccm[i];
    }
    color_matrix_attr.auto_attr.iso_act_en = get_pipe_params()[index].static_ccm.auto_iso_act_en;
    color_matrix_attr.auto_attr.temp_act_en = get_pipe_params()[index].static_ccm.auto_temp_act_en;
    color_matrix_attr.auto_attr.ccm_tab_num = (hi_u16)get_pipe_params()[index].static_ccm.total_num;

    for (hi_u32 i = 0; i < get_pipe_params()[index].static_ccm.total_num; i++) {
        color_matrix_attr.auto_attr.ccm_tab[i].color_temp = get_pipe_params()[index].static_ccm.auto_color_temp[i];
        for (j = 0; j < HI_ISP_CCM_MATRIX_SIZE; j++) {
            color_matrix_attr.auto_attr.ccm_tab[i].ccm[j] = get_pipe_params()[index].static_ccm.auto_ccm[i][j];
        }
    }
    ret = hi_mpi_isp_set_ccm_attr(vi_pipe, &color_matrix_attr);
    check_scene_ret(ret);
    return HI_SUCCESS;
}

hi_s32 ot_scene_set_static_acac(hi_vi_pipe vi_pipe, hi_u8 index)
{
    ot_scenecomm_expr_true_return(index >= HI_SCENE_PIPETYPE_NUM, HI_FAILURE);
    if (get_pipe_params()[index].module_state.static_acac != HI_TRUE) {
        return HI_SUCCESS;
    }
    hi_u32 i, j;
    hi_s32 ret;
    hi_isp_acac_attr acac_attr;

    ret = hi_mpi_isp_get_acac_attr(vi_pipe, &acac_attr);
    check_scene_ret(ret);
    acac_attr.en = get_pipe_params()[index].static_acac.enable;
    acac_attr.op_type = get_pipe_params()[index].static_acac.op_type;
    acac_attr.detect_mode = get_pipe_params()[index].static_acac.detect_mode;

    for (i = 0; i < HI_ISP_ACAC_THR_NUM; i++) {
        for (j = 0; j < HI_ISP_AUTO_ISO_NUM; j++) {
            acac_attr.auto_attr.edge_threshold[i][j] =
                get_pipe_params()[index].static_acac.acac_auto_cfg.edge_threshold[i][j];
        }
    }

    for (i = 0; i < HI_ISP_AUTO_ISO_NUM; i++) {
        acac_attr.auto_attr.edge_gain[i] = get_pipe_params()[index].static_acac.acac_auto_cfg.edge_gain[i];
        acac_attr.auto_attr.purple_upper_limit[i] =
            get_pipe_params()[index].static_acac.acac_auto_cfg.purple_upper_limit[i];
        acac_attr.auto_attr.purple_lower_limit[i] =
            get_pipe_params()[index].static_acac.acac_auto_cfg.purple_lower_limit[i];
        acac_attr.auto_attr.purple_sat_threshold[i] =
            get_pipe_params()[index].static_acac.acac_auto_cfg.purple_sat_threshold[i];
        acac_attr.auto_attr.purple_alpha[i] = get_pipe_params()[index].static_acac.acac_auto_cfg.purple_alpha[i];
        acac_attr.auto_attr.edge_alpha[i] = get_pipe_params()[index].static_acac.acac_auto_cfg.edge_alpha[i];
        acac_attr.auto_attr.fcc_y_strength[i] = get_pipe_params()[index].static_acac.acac_auto_cfg.fcc_y_strength[i];
        acac_attr.auto_attr.fcc_rb_strength[i] = get_pipe_params()[index].static_acac.acac_auto_cfg.fcc_rb_strength[i];
    }

    ret = hi_mpi_isp_set_acac_attr(vi_pipe, &acac_attr);
    check_scene_ret(ret);
    return HI_SUCCESS;
}

hi_s32 ot_scene_set_static_local_cac(hi_vi_pipe vi_pipe, hi_u8 index)
{
    ot_scenecomm_expr_true_return(index >= HI_SCENE_PIPETYPE_NUM, HI_FAILURE);
    if (get_pipe_params()[index].module_state.static_local_cac != HI_TRUE) {
        return HI_SUCCESS;
    }

    hi_s32 i, ret;
    hi_isp_local_cac_attr local_cac;

    ret = hi_mpi_isp_get_local_cac_attr(vi_pipe, &local_cac);
    check_scene_ret(ret);

    local_cac.en = get_pipe_params()[index].static_local_cac.enable;
    local_cac.op_type = get_pipe_params()[index].static_local_cac.op_type;
    local_cac.purple_detect_range = get_pipe_params()[index].static_local_cac.purple_detect_range;
    local_cac.var_threshold = get_pipe_params()[index].static_local_cac.var_threshold;

    for (i = 0; i < HI_ISP_LCAC_DET_NUM; i++) {
        local_cac.r_detect_threshold[i] = get_pipe_params()[index].static_local_cac.r_detect_threshold[i];
        local_cac.g_detect_threshold[i] = get_pipe_params()[index].static_local_cac.g_detect_threshold[i];
        local_cac.b_detect_threshold[i] = get_pipe_params()[index].static_local_cac.b_detect_threshold[i];
        local_cac.l_detect_threshold[i] = get_pipe_params()[index].static_local_cac.l_detect_threshold[i];
        local_cac.cb_cr_ratio[i] = get_pipe_params()[index].static_local_cac.cb_cr_ratio[i];
    }

    for (i = 0; i < HI_SCENE_ISO_STRENGTH_NUM; i++) {
        local_cac.auto_attr.de_purple_cr_strength[i] =
            get_pipe_params()[index].static_local_cac.depurplestr_auto_cfg.depur_cr_strength[i];
        local_cac.auto_attr.de_purple_cb_strength[i] =
            get_pipe_params()[index].static_local_cac.depurplestr_auto_cfg.depur_cb_strength[i];
    }

    ret = hi_mpi_isp_set_local_cac_attr(vi_pipe, &local_cac);
    check_scene_ret(ret);
    return HI_SUCCESS;
}

hi_s32 ot_scene_set_static_dpc(hi_vi_pipe vi_pipe, hi_u8 index)
{
    ot_scenecomm_expr_true_return(index >= HI_SCENE_PIPETYPE_NUM, HI_FAILURE);
    if (get_pipe_params()[index].module_state.static_dpc != HI_TRUE) {
        return HI_SUCCESS;
    }

    hi_s32 i, ret;
    hi_isp_dp_dynamic_attr dp_dynamic_attr;

    ret = hi_mpi_isp_get_dp_dynamic_attr(vi_pipe, &dp_dynamic_attr);
    check_scene_ret(ret);

    dp_dynamic_attr.enable = get_pipe_params()[index].static_dpc.enable;

    for (i = 0; i < HI_SCENE_ISO_STRENGTH_NUM; i++) {
        dp_dynamic_attr.auto_attr.strength[i] = get_pipe_params()[index].static_dpc.strength[i];
        dp_dynamic_attr.auto_attr.blend_ratio[i] = get_pipe_params()[index].static_dpc.blend_ratio[i];
    }

    ret = hi_mpi_isp_set_dp_dynamic_attr(vi_pipe, &dp_dynamic_attr);
    check_scene_ret(ret);
    return HI_SUCCESS;
}

hi_s32 ot_scene_set_static_drc(hi_vi_pipe vi_pipe, hi_u8 index)
{
    ot_scenecomm_expr_true_return(index >= HI_SCENE_PIPETYPE_NUM, HI_FAILURE);
    if (get_pipe_params()[index].module_state.static_drc != HI_TRUE) {
        return HI_SUCCESS;
    }

    hi_s32 i, ret;
    hi_isp_drc_attr drc_attr;

    ret = hi_mpi_isp_get_drc_attr(vi_pipe, &drc_attr);
    check_scene_ret(ret);

    drc_attr.enable = get_pipe_params()[index].static_drc.enable;
    drc_attr.curve_select = (hi_isp_drc_curve_select)get_pipe_params()[index].static_drc.curve_select;
    drc_attr.op_type = (hi_op_mode)get_pipe_params()[index].static_drc.op_type;
    drc_attr.purple_reduction_strength = get_pipe_params()[index].static_drc.purple_reduction_strength;

    drc_attr.contrast_ctrl = get_pipe_params()[index].static_drc.contrast_ctrl;
    drc_attr.blend_luma_max = get_pipe_params()[index].static_drc.blend_luma_max;
    drc_attr.blend_luma_bright_min = get_pipe_params()[index].static_drc.blend_luma_bright_min;
    drc_attr.blend_luma_bright_threshold = get_pipe_params()[index].static_drc.blend_luma_bright_threshold;
    drc_attr.blend_luma_bright_slope = get_pipe_params()[index].static_drc.blend_luma_bright_slope;
    drc_attr.blend_luma_dark_min = get_pipe_params()[index].static_drc.blend_luma_dark_min;
    drc_attr.blend_luma_dark_threshold = get_pipe_params()[index].static_drc.blend_luma_dark_threshold;
    drc_attr.blend_luma_dark_slope = get_pipe_params()[index].static_drc.blend_luma_dark_slope;
    drc_attr.blend_detail_max = get_pipe_params()[index].static_drc.blend_detail_max;
    drc_attr.blend_detail_bright_min = get_pipe_params()[index].static_drc.blend_detail_bright_min;
    drc_attr.blend_detail_bright_threshold = get_pipe_params()[index].static_drc.blend_detail_bright_threshold;
    drc_attr.blend_detail_bright_slope = get_pipe_params()[index].static_drc.blend_detail_bright_slope;
    drc_attr.blend_detail_dark_min = get_pipe_params()[index].static_drc.blend_detail_dark_min;
    drc_attr.blend_detail_dark_threshold = get_pipe_params()[index].static_drc.blend_detail_dark_threshold;
    drc_attr.blend_detail_dark_slope = get_pipe_params()[index].static_drc.blend_detail_dark_slope;
    drc_attr.color_correction_ctrl = get_pipe_params()[index].static_drc.color_correction_ctrl;

    for (i = 0; i < HI_ISP_DRC_CC_NODE_NUM; i++) {
        drc_attr.color_correction_lut[i] = get_pipe_params()[index].static_drc.color_correction_lut[i];
    }

    ret = hi_mpi_isp_set_drc_attr(vi_pipe, &drc_attr);
    check_scene_ret(ret);
    return HI_SUCCESS;
}

hi_s32 ot_scene_set_static_ldci(hi_vi_pipe vi_pipe, hi_u8 index)
{
    ot_scenecomm_expr_true_return(index >= HI_SCENE_PIPETYPE_NUM, HI_FAILURE);
    if (get_pipe_params()[index].module_state.static_ldci != HI_TRUE) {
        return HI_SUCCESS;
    }

    hi_s32 i, ret;
    hi_isp_ldci_attr ldci_attr;

    ret = hi_mpi_isp_get_ldci_attr(vi_pipe, &ldci_attr);
    check_scene_ret(ret);

    ldci_attr.en = get_pipe_params()[index].static_ldci.enable;
    ldci_attr.op_type = (hi_op_mode)get_pipe_params()[index].static_ldci.ldci_op_type;
    ldci_attr.gauss_lpf_sigma = get_pipe_params()[index].static_ldci.gauss_lpf_sigma;
    ldci_attr.tpr_incr_coef = get_pipe_params()[index].static_ldci.tpr_incr_coef;
    ldci_attr.tpr_decr_coef = get_pipe_params()[index].static_ldci.tpr_decr_coef;

    for (i = 0; i < HI_ISP_AUTO_ISO_NUM; i++) {
        ldci_attr.auto_attr.he_wgt[i].he_pos_wgt.sigma = get_pipe_params()[index].static_ldci.auto_he_pos_sigma[i];
        ldci_attr.auto_attr.he_wgt[i].he_pos_wgt.wgt = get_pipe_params()[index].static_ldci.auto_he_pos_wgt[i];
        ldci_attr.auto_attr.he_wgt[i].he_pos_wgt.mean = get_pipe_params()[index].static_ldci.auto_he_pos_mean[i];
        ldci_attr.auto_attr.he_wgt[i].he_neg_wgt.sigma = get_pipe_params()[index].static_ldci.auto_he_neg_sigma[i];
        ldci_attr.auto_attr.he_wgt[i].he_neg_wgt.wgt = get_pipe_params()[index].static_ldci.auto_he_neg_wgt[i];
        ldci_attr.auto_attr.he_wgt[i].he_neg_wgt.mean = get_pipe_params()[index].static_ldci.auto_he_neg_mean[i];
        ldci_attr.auto_attr.blc_ctrl[i] = get_pipe_params()[index].static_ldci.auto_blc_ctrl[i];
    }

    ret = hi_mpi_isp_set_ldci_attr(vi_pipe, &ldci_attr);
    check_scene_ret(ret);
    return HI_SUCCESS;
}

hi_s32 ot_scene_set_static_dehaze(hi_vi_pipe vi_pipe, hi_u8 index)
{
    ot_scenecomm_expr_true_return(index >= HI_SCENE_PIPETYPE_NUM, HI_FAILURE);
    if (get_pipe_params()[index].module_state.static_dehaze != HI_TRUE) {
        return HI_SUCCESS;
    }

    hi_s32 i, ret;
    hi_isp_dehaze_attr dehaze_attr;

    ret = hi_mpi_isp_get_dehaze_attr(vi_pipe, &dehaze_attr);
    check_scene_ret(ret);
    dehaze_attr.en = get_pipe_params()[index].static_dehaze.enable;
    dehaze_attr.op_type = (hi_op_mode)get_pipe_params()[index].static_dehaze.dehaze_op_type;
    dehaze_attr.user_lut_en = get_pipe_params()[index].static_dehaze.user_lut_enable;
    dehaze_attr.tmprflt_incr_coef = get_pipe_params()[index].static_dehaze.tmprflt_incr_coef;
    dehaze_attr.tmprflt_decr_coef = get_pipe_params()[index].static_dehaze.tmprflt_decr_coef;

    for (i = 0; i < HI_ISP_DEHAZE_LUT_SIZE; i++) {
        dehaze_attr.dehaze_lut[i] = get_pipe_params()[index].static_dehaze.dehaze_lut[i];
    }
    ret = hi_mpi_isp_set_dehaze_attr(vi_pipe, &dehaze_attr);
    check_scene_ret(ret);
    return HI_SUCCESS;
}

hi_s32 ot_scene_set_static_csc(hi_vi_pipe vi_pipe, hi_u8 index)
{
    ot_scenecomm_expr_true_return(index >= HI_SCENE_PIPETYPE_NUM, HI_FAILURE);
    if (get_pipe_params()[index].module_state.static_csc != HI_TRUE) {
        return HI_SUCCESS;
    }

    hi_s32 ret;
    hi_isp_csc_attr csc_attr;

    ret = hi_mpi_isp_get_csc_attr(vi_pipe, &csc_attr);
    check_scene_ret(ret);

    csc_attr.en = get_pipe_params()[index].staic_csc.enable;
    csc_attr.hue = get_pipe_params()[index].staic_csc.hue;
    csc_attr.luma = get_pipe_params()[index].staic_csc.luma;
    csc_attr.contr = get_pipe_params()[index].staic_csc.contrast;
    csc_attr.satu = get_pipe_params()[index].staic_csc.saturation;
    csc_attr.color_gamut = (hi_color_gamut)get_pipe_params()[index].staic_csc.color_gamut;

    ret = hi_mpi_isp_set_csc_attr(vi_pipe, &csc_attr);
    check_scene_ret(ret);
    return HI_SUCCESS;
}

static hi_s32 ot_scene_set_static_nr_snr(hi_vi_pipe vi_pipe, hi_u8 index, hi_isp_nr_attr *nr_attr)
{
    hi_s32 i, j;

    ot_scenecomm_expr_true_return(index >= HI_SCENE_PIPETYPE_NUM, HI_FAILURE);
    ot_scenecomm_check_pointer_return(nr_attr, HI_FAILURE);
    for (i = 0; i < HI_ISP_AUTO_ISO_NUM; i++) {
        nr_attr->snr_cfg.snr_auto.sfm_threshold[i] = get_pipe_params()[index].static_nr.snr_cfg.sfm_threshold[i];
        nr_attr->snr_cfg.snr_auto.sfm0_mode[i] = get_pipe_params()[index].static_nr.snr_cfg.sfm0_mode[i];
        nr_attr->snr_cfg.snr_auto.sfm0_ex_strength[i] = get_pipe_params()[index].static_nr.snr_cfg.sfm0_ex_strength[i];
        nr_attr->snr_cfg.snr_auto.sfm0_ex_detail_prot[i] =
            get_pipe_params()[index].static_nr.snr_cfg.sfm0_ex_detail_prot[i];
        nr_attr->snr_cfg.snr_auto.sfm0_norm_edge_strength[i] =
            get_pipe_params()[index].static_nr.snr_cfg.sfm0_norm_edge_strength[i];
        nr_attr->snr_cfg.snr_auto.sfm1_detail_prot[i] = get_pipe_params()[index].static_nr.snr_cfg.sfm1_detail_prot[i];
        nr_attr->snr_cfg.snr_auto.sfm1_coarse_strength[i] =
            get_pipe_params()[index].static_nr.snr_cfg.sfm1_coarse_strength[i];
        nr_attr->snr_cfg.snr_auto.fine_strength[i] = get_pipe_params()[index].static_nr.snr_cfg.fine_strength[i];
        nr_attr->snr_cfg.snr_auto.coring_wgt[i] = get_pipe_params()[index].static_nr.snr_cfg.coring_wgt[i];
        nr_attr->snr_cfg.snr_auto.coring_mot_thresh[i] =
            get_pipe_params()[index].static_nr.snr_cfg.coring_mot_thresh[i];
        for (j = 0; j < HI_ISP_BAYER_CHN_NUM; j++) {
            nr_attr->snr_cfg.snr_auto.sfm0_coarse_strength[j][i] =
                get_pipe_params()[index].static_nr.snr_cfg.sfm0_coarse_strength[j][i];
        }
    }
    return HI_SUCCESS;
}

static hi_s32 ot_scene_set_static_nr_tnr(hi_vi_pipe vi_pipe, hi_u8 index, hi_isp_nr_attr *nr_attr)
{
    hi_s32 i;

    ot_scenecomm_expr_true_return(index >= HI_SCENE_PIPETYPE_NUM, HI_FAILURE);
    ot_scenecomm_check_pointer_return(nr_attr, HI_FAILURE);
    for (i = 0; i < HI_ISP_AUTO_ISO_NUM; i++) {
        nr_attr->tnr_cfg.tnr_auto.md_mode[i] = get_pipe_params()[index].static_nr.tnr_cfg.md_mode[i];
        nr_attr->tnr_cfg.tnr_auto.md_anti_flicker_strength[i] =
            get_pipe_params()[index].static_nr.tnr_cfg.md_anti_flicker_strength[i];
        nr_attr->tnr_cfg.tnr_auto.md_static_ratio[i] = get_pipe_params()[index].static_nr.tnr_cfg.md_static_ratio[i];
        nr_attr->tnr_cfg.tnr_auto.md_static_fine_strength[i] =
            get_pipe_params()[index].static_nr.tnr_cfg.md_static_fine_strength[i];
        nr_attr->tnr_cfg.tnr_auto.tss[i] = get_pipe_params()[index].static_nr.tnr_cfg.tss[i];
        nr_attr->tnr_cfg.tnr_auto.tfr[i] = get_pipe_params()[index].static_nr.tnr_cfg.tfr[i];
        nr_attr->tnr_cfg.tnr_auto.tfs[i] = get_pipe_params()[index].static_nr.tnr_cfg.tfs[i];
        nr_attr->tnr_cfg.tnr_auto.user_define_md[i] = get_pipe_params()[index].static_nr.tnr_cfg.user_define_md[i];
        nr_attr->tnr_cfg.tnr_auto.user_define_slope[i] =
            get_pipe_params()[index].static_nr.tnr_cfg.user_define_slope[i];
        nr_attr->tnr_cfg.tnr_auto.user_define_dark_thresh[i] =
            get_pipe_params()[index].static_nr.tnr_cfg.user_define_dark_thresh[i];
        nr_attr->tnr_cfg.tnr_auto.user_define_color_thresh[i] =
            get_pipe_params()[index].static_nr.tnr_cfg.user_define_color_thresh[i];

        nr_attr->tnr_cfg.tnr_auto.sfr_r[i] = get_pipe_params()[index].static_nr.tnr_cfg.sfr_r[i];
        nr_attr->tnr_cfg.tnr_auto.sfr_g[i] = get_pipe_params()[index].static_nr.tnr_cfg.sfr_g[i];
        nr_attr->tnr_cfg.tnr_auto.sfr_b[i] = get_pipe_params()[index].static_nr.tnr_cfg.sfr_b[i];
    }
    return HI_SUCCESS;
}

hi_s32 ot_scene_set_static_nr(hi_vi_pipe vi_pipe, hi_u8 index)
{
    ot_scenecomm_expr_true_return(index >= HI_SCENE_PIPETYPE_NUM, HI_FAILURE);
    if (get_pipe_params()[index].module_state.static_nr != HI_TRUE) {
        return HI_SUCCESS;
    }

    hi_s32 i, ret;
    hi_isp_nr_attr nr_attr;

    ret = hi_mpi_isp_get_nr_attr(vi_pipe, &nr_attr);
    check_scene_ret(ret);

    nr_attr.en = get_pipe_params()[index].static_nr.enable;
    nr_attr.op_type = (hi_op_mode)get_pipe_params()[index].static_nr.op_type;
    nr_attr.tnr_en = get_pipe_params()[index].static_nr.tnr_enable;
    nr_attr.lsc_nr_en = get_pipe_params()[index].static_nr.lsc_nr_enable;
    nr_attr.lsc_ratio1 = get_pipe_params()[index].static_nr.lsc_ratio1;
    nr_attr.lsc_ratio2 = get_pipe_params()[index].static_nr.lsc_ratio2;

    for (i = 0; i < HI_ISP_BAYERNR_LUT_LENGTH; i++) {
        nr_attr.coring_ratio[i] = get_pipe_params()[index].static_nr.coring_ratio[i];
    }

    ret = ot_scene_set_static_nr_snr(vi_pipe, index, &nr_attr);
    check_scene_ret(ret);

    ret = ot_scene_set_static_nr_tnr(vi_pipe, index, &nr_attr);
    check_scene_ret(ret);

    ret = hi_mpi_isp_set_nr_attr(vi_pipe, &nr_attr);
    check_scene_ret(ret);
    return HI_SUCCESS;
}

hi_s32 ot_scene_set_static_shading(hi_vi_pipe vi_pipe, hi_u8 index)
{
    ot_scenecomm_expr_true_return(index >= HI_SCENE_PIPETYPE_NUM, HI_FAILURE);
    if (get_pipe_params()[index].module_state.static_shading != HI_TRUE) {
        return HI_SUCCESS;
    }

    hi_s32 ret;
    hi_isp_shading_attr shading_attr;

    ret = hi_mpi_isp_get_mesh_shading_attr(vi_pipe, &shading_attr);
    check_scene_ret(ret);

    shading_attr.en = get_pipe_params()[index].static_shading.enable;
    ret = hi_mpi_isp_set_mesh_shading_attr(vi_pipe, &shading_attr);
    check_scene_ret(ret);
    return HI_SUCCESS;
}

static hi_s32 ot_scene_set_static_sharpen_param_auto(hi_vi_pipe vi_pipe, hi_u8 index, hi_isp_sharpen_attr *sharpen_attr)
{
    ot_scenecomm_expr_true_return(index >= HI_SCENE_PIPETYPE_NUM, HI_FAILURE);
    ot_scenecomm_check_pointer_return(sharpen_attr, HI_FAILURE);
    hi_s32 i;

    for (i = 0; i < HI_ISP_AUTO_ISO_NUM; i++) {
        sharpen_attr->auto_attr.texture_freq[i] =
            get_pipe_params()[index].static_sharpen.sharpen_auto_cfg.texture_freq[i];
        sharpen_attr->auto_attr.edge_freq[i] = get_pipe_params()[index].static_sharpen.sharpen_auto_cfg.edge_freq[i];
        sharpen_attr->auto_attr.over_shoot[i] = get_pipe_params()[index].static_sharpen.sharpen_auto_cfg.over_shoot[i];
        sharpen_attr->auto_attr.under_shoot[i] =
            get_pipe_params()[index].static_sharpen.sharpen_auto_cfg.under_shoot[i];
        sharpen_attr->auto_attr.motion_texture_freq[i] =
            get_pipe_params()[index].static_sharpen.sharpen_auto_cfg.motion_texture_freq[i];
        sharpen_attr->auto_attr.motion_edge_freq[i] =
            get_pipe_params()[index].static_sharpen.sharpen_auto_cfg.motion_edge_freq[i];
        sharpen_attr->auto_attr.motion_over_shoot[i] =
            get_pipe_params()[index].static_sharpen.sharpen_auto_cfg.motion_over_shoot[i];
        sharpen_attr->auto_attr.motion_under_shoot[i] =
            get_pipe_params()[index].static_sharpen.sharpen_auto_cfg.motion_under_shoot[i];
        sharpen_attr->auto_attr.shoot_sup_strength[i] =
            get_pipe_params()[index].static_sharpen.sharpen_auto_cfg.shoot_sup_strength[i];
        sharpen_attr->auto_attr.shoot_sup_adj[i] =
            get_pipe_params()[index].static_sharpen.sharpen_auto_cfg.shoot_sup_adj[i];
        sharpen_attr->auto_attr.detail_ctrl[i] =
            get_pipe_params()[index].static_sharpen.sharpen_auto_cfg.detail_ctrl[i];
        sharpen_attr->auto_attr.detail_ctrl_threshold[i] =
            get_pipe_params()[index].static_sharpen.sharpen_auto_cfg.detail_ctrl_threshold[i];
        sharpen_attr->auto_attr.edge_filt_strength[i] =
            get_pipe_params()[index].static_sharpen.sharpen_auto_cfg.edge_filt_strength[i];
        sharpen_attr->auto_attr.edge_filt_max_cap[i] =
            get_pipe_params()[index].static_sharpen.sharpen_auto_cfg.edge_filt_max_cap[i];
        sharpen_attr->auto_attr.r_gain[i] = get_pipe_params()[index].static_sharpen.sharpen_auto_cfg.r_gain[i];
        sharpen_attr->auto_attr.g_gain[i] = get_pipe_params()[index].static_sharpen.sharpen_auto_cfg.g_gain[i];
        sharpen_attr->auto_attr.b_gain[i] = get_pipe_params()[index].static_sharpen.sharpen_auto_cfg.b_gain[i];
        sharpen_attr->auto_attr.skin_gain[i] = get_pipe_params()[index].static_sharpen.sharpen_auto_cfg.skin_gain[i];
        sharpen_attr->auto_attr.max_sharp_gain[i] =
            get_pipe_params()[index].static_sharpen.sharpen_auto_cfg.max_sharp_gain[i];
    }
    return HI_SUCCESS;
}

hi_s32 ot_scene_set_static_sharpen(hi_vi_pipe vi_pipe, hi_u8 index)
{
    ot_scenecomm_expr_true_return(index >= HI_SCENE_PIPETYPE_NUM, HI_FAILURE);
    if (get_pipe_params()[index].module_state.static_sharpen != HI_TRUE) {
        return HI_SUCCESS;
    }
    hi_s32 i, j, ret;
    hi_isp_sharpen_attr sharpen_attr;

    ret = hi_mpi_isp_get_sharpen_attr(vi_pipe, &sharpen_attr);
    check_scene_ret(ret);

    sharpen_attr.en = get_pipe_params()[index].static_sharpen.enable;
    sharpen_attr.motion_en = get_pipe_params()[index].static_sharpen.motion_en;
    sharpen_attr.motion_threshold0 = get_pipe_params()[index].static_sharpen.motion_threshold0;
    sharpen_attr.motion_threshold1 = get_pipe_params()[index].static_sharpen.motion_threshold1;
    sharpen_attr.motion_gain0 = get_pipe_params()[index].static_sharpen.motion_gain0;
    sharpen_attr.motion_gain1 = get_pipe_params()[index].static_sharpen.motion_gain1;
    sharpen_attr.skin_umin = get_pipe_params()[index].static_sharpen.skin_umin;
    sharpen_attr.skin_vmin = get_pipe_params()[index].static_sharpen.skin_vmin;
    sharpen_attr.skin_umax = get_pipe_params()[index].static_sharpen.skin_umax;
    sharpen_attr.skin_vmax = get_pipe_params()[index].static_sharpen.skin_vmax;

    for (i = 0; i < HI_ISP_SHARPEN_GAIN_NUM; i++) {
        for (j = 0; j < HI_ISP_AUTO_ISO_NUM; j++) {
            sharpen_attr.auto_attr.luma_wgt[i][j] =
                get_pipe_params()[index].static_sharpen.sharpen_auto_cfg.luma_wgt[i][j];
            sharpen_attr.auto_attr.texture_strength[i][j] =
                get_pipe_params()[index].static_sharpen.sharpen_auto_cfg.texture_strength[i][j];
            sharpen_attr.auto_attr.edge_strength[i][j] =
                get_pipe_params()[index].static_sharpen.sharpen_auto_cfg.edge_strength[i][j];
            sharpen_attr.auto_attr.motion_texture_strength[i][j] =
                get_pipe_params()[index].static_sharpen.sharpen_auto_cfg.motion_texture_strength[i][j];
            sharpen_attr.auto_attr.motion_edge_strength[i][j] =
                get_pipe_params()[index].static_sharpen.sharpen_auto_cfg.motion_edge_strength[i][j];
        }
    }

    ret = ot_scene_set_static_sharpen_param_auto(vi_pipe, index, &sharpen_attr);
    check_scene_ret(ret);

    ret = hi_mpi_isp_set_sharpen_attr(vi_pipe, &sharpen_attr);
    check_scene_ret(ret);
    return HI_SUCCESS;
}

hi_s32 ot_scene_set_static_demosaic(hi_vi_pipe vi_pipe, hi_u8 index)
{
    ot_scenecomm_expr_true_return(index >= HI_SCENE_PIPETYPE_NUM, HI_FAILURE);
    if (get_pipe_params()[index].module_state.static_dm != HI_TRUE) {
        return HI_SUCCESS;
    }
    hi_s32 i, ret;
    hi_isp_demosaic_attr dm_attr;

    ret = hi_mpi_isp_get_demosaic_attr(vi_pipe, &dm_attr);
    check_scene_ret(ret);

    dm_attr.en = get_pipe_params()[index].static_dm.enable;
    dm_attr.op_type = get_pipe_params()[index].static_dm.op_type;

    for (i = 0; i < HI_ISP_AUTO_ISO_NUM; i++) {
        dm_attr.auto_attr.nddm_strength[i] = get_pipe_params()[index].static_dm.dm_auto_cfg.nddm_strength[i];
        dm_attr.auto_attr.nddm_mf_detail_strength[i] =
            get_pipe_params()[index].static_dm.dm_auto_cfg.nddm_mf_detail_strength[i];
        dm_attr.auto_attr.nddm_hf_detail_strength[i] =
            get_pipe_params()[index].static_dm.dm_auto_cfg.nddm_hf_detail_strength[i];
        dm_attr.auto_attr.detail_smooth_range[i] =
            get_pipe_params()[index].static_dm.dm_auto_cfg.detail_smooth_range[i];
        dm_attr.auto_attr.color_noise_f_threshold[i] =
            get_pipe_params()[index].static_dm.dm_auto_cfg.color_noise_f_threshold[i];
        dm_attr.auto_attr.color_noise_f_strength[i] =
            get_pipe_params()[index].static_dm.dm_auto_cfg.color_noise_f_strength[i];
        dm_attr.auto_attr.color_noise_y_threshold[i] =
            get_pipe_params()[index].static_dm.dm_auto_cfg.color_noise_y_threshold[i];
        dm_attr.auto_attr.color_noise_y_strength[i] =
            get_pipe_params()[index].static_dm.dm_auto_cfg.color_noise_y_strength[i];
    }

    ret = hi_mpi_isp_set_demosaic_attr(vi_pipe, &dm_attr);
    check_scene_ret(ret);

    return HI_SUCCESS;
}

hi_s32 ot_scene_set_static_bayershp(hi_vi_pipe vi_pipe, hi_u8 index)
{
    ot_scenecomm_expr_true_return(index >= HI_SCENE_PIPETYPE_NUM, HI_FAILURE);
    if (get_pipe_params()[index].module_state.static_bayeshp != HI_TRUE) {
        return HI_SUCCESS;
    }
    hi_s32 i, j, ret;
    hi_isp_bayershp_attr bshp_attr;

    ret = hi_mpi_isp_get_bayershp_attr(vi_pipe, &bshp_attr);
    check_scene_ret(ret);

    bshp_attr.en = get_pipe_params()[index].static_bayershp.enable;
    bshp_attr.op_type = get_pipe_params()[index].static_bayershp.op_type;

    for (i = 0; i < HI_ISP_BSHP_HALF_CURVE_NUM; i++) {
        for (j = 0; j < HI_ISP_AUTO_ISO_NUM; j++) {
            bshp_attr.auto_attr.luma_wgt[i][j] =
                get_pipe_params()[index].static_bayershp.bayershp_auto_attr.luma_wgt[i][j];
            bshp_attr.auto_attr.edge_mf_strength[i][j] =
                get_pipe_params()[index].static_bayershp.bayershp_auto_attr.edge_mf_strength[i][j];
            bshp_attr.auto_attr.texture_mf_strength[i][j] =
                get_pipe_params()[index].static_bayershp.bayershp_auto_attr.texture_mf_strength[i][j];
            bshp_attr.auto_attr.edge_hf_strength[i][j] =
                get_pipe_params()[index].static_bayershp.bayershp_auto_attr.edge_hf_strength[i][j];
            bshp_attr.auto_attr.texture_hf_strength[i][j] =
                get_pipe_params()[index].static_bayershp.bayershp_auto_attr.texture_hf_strength[i][j];
        }
    }

    for (i = 0; i < HI_ISP_AUTO_ISO_NUM; i++) {
        bshp_attr.auto_attr.edge_filt_strength[i] =
            get_pipe_params()[index].static_bayershp.bayershp_auto_attr.edge_filt_strength[i];
        bshp_attr.auto_attr.texture_max_gain[i] =
            get_pipe_params()[index].static_bayershp.bayershp_auto_attr.texture_max_gain[i];
        bshp_attr.auto_attr.edge_max_gain[i] =
            get_pipe_params()[index].static_bayershp.bayershp_auto_attr.edge_max_gain[i];
        bshp_attr.auto_attr.overshoot[i] = get_pipe_params()[index].static_bayershp.bayershp_auto_attr.overshoot[i];
        bshp_attr.auto_attr.undershoot[i] = get_pipe_params()[index].static_bayershp.bayershp_auto_attr.undershoot[i];
        bshp_attr.auto_attr.g_chn_gain[i] = get_pipe_params()[index].static_bayershp.bayershp_auto_attr.g_chn_gain[i];
    }

    ret = hi_mpi_isp_set_bayershp_attr(vi_pipe, &bshp_attr);
    check_scene_ret(ret);

    return HI_SUCCESS;
}

hi_s32 ot_scene_set_static_ca(hi_vi_pipe vi_pipe, hi_u8 index)
{
    ot_scenecomm_expr_true_return(index >= HI_SCENE_PIPETYPE_NUM, HI_FAILURE);
    if (get_pipe_params()[index].module_state.static_ca != HI_TRUE) {
        return HI_SUCCESS;
    }

    hi_s32 i, ret;
    hi_isp_ca_attr ca_attr;

    ret = hi_mpi_isp_get_ca_attr(vi_pipe, &ca_attr);
    check_scene_ret(ret);

    ca_attr.en = get_pipe_params()[index].static_ca.enable;
    for (i = 0; i < ISP_AUTO_ISO_CA_NUM; i++) {
        ca_attr.ca.iso_ratio[i] = get_pipe_params()[index].static_ca.iso_ratio[i];
    }

    for (i = 0; i < HI_ISP_CA_YRATIO_LUT_LENGTH; i++) {
        ca_attr.ca.y_ratio_lut[i] = get_pipe_params()[index].static_ca.y_ratio_lut[i];
    }

    ret = hi_mpi_isp_set_ca_attr(vi_pipe, &ca_attr);
    check_scene_ret(ret);
    return HI_SUCCESS;
}

hi_s32 ot_scene_set_static_venc(hi_vi_pipe vi_pipe, hi_u8 index)
{
    ot_scenecomm_expr_true_return(index >= HI_SCENE_PIPETYPE_NUM, HI_FAILURE);
    if (get_pipe_params()[index].module_state.static_venc != HI_TRUE) {
        return HI_SUCCESS;
    }

    hi_s32 ret;
    hi_venc_rc_param rc_param;
    hi_venc_chn_attr venc_attr;
    hi_venc_deblur_param deblur_param;
    /* only support channel 0 settings; only supprot h265  cvbr and avbr param settings */
    ret = hi_mpi_venc_get_rc_param(0, &rc_param);
    check_scene_ret(ret);

    ret = hi_mpi_venc_get_chn_attr(0, &venc_attr);
    check_scene_ret(ret);

    ret = hi_mpi_venc_get_deblur(0, &deblur_param);
    check_scene_ret(ret);

    if (venc_attr.rc_attr.rc_mode == HI_VENC_RC_MODE_H265_AVBR) {
        scene_set_static_h265_avbr(&rc_param, index);
    } else if (venc_attr.rc_attr.rc_mode == HI_VENC_RC_MODE_H265_CVBR) {
        scene_set_static_h265_cvbr(&rc_param, index);
    } else {
    }

    deblur_param.deblur_en = 1;
    deblur_param.deblur_adaptive_en = 1;

    ret = hi_mpi_venc_set_deblur(0, &deblur_param);
    check_scene_ret(ret);

    ret = hi_mpi_venc_set_rc_param(0, &rc_param);
    check_scene_ret(ret);

    ret = hi_mpi_venc_set_chn_attr(0, &venc_attr);
    check_scene_ret(ret);

    ret = hi_mpi_venc_get_deblur(0, &deblur_param);
    check_scene_ret(ret);
    return HI_SUCCESS;
}

hi_s32 ot_scene_set_static_crosstalk(hi_vi_pipe vi_pipe, hi_u8 index)
{
    ot_scenecomm_expr_true_return(index >= HI_SCENE_PIPETYPE_NUM, HI_FAILURE);
    if (get_pipe_params()[index].module_state.static_cross_talk != HI_TRUE) {
        return HI_SUCCESS;
    }

    hi_s32 ret;
    hi_isp_cr_attr crosstalk_attr;

    ret = hi_mpi_isp_get_crosstalk_attr(vi_pipe, &crosstalk_attr);
    check_scene_ret(ret);

    crosstalk_attr.en = get_pipe_params()[index].static_crosstalk.enable;
    ret = hi_mpi_isp_set_crosstalk_attr(vi_pipe, &crosstalk_attr);
    check_scene_ret(ret);
    return HI_SUCCESS;
}

hi_s32 ot_scene_set_static_clut(hi_vi_pipe vi_pipe, hi_u8 index)
{
    if (get_pipe_params()[index].module_state.static_clut != HI_TRUE) {
        return HI_SUCCESS;
    }

    hi_s32 ret;
    hi_isp_clut_attr clut_attr;

    ret = hi_mpi_isp_get_clut_attr(vi_pipe, &clut_attr);
    check_scene_ret(ret);

    clut_attr.en = get_pipe_params()[index].static_clut.enable;
    ret = hi_mpi_isp_set_clut_attr(vi_pipe, &clut_attr);
    check_scene_ret(ret);
    return HI_SUCCESS;
}

hi_s32 ot_scene_set_static_pregamma(hi_vi_pipe vi_pipe, hi_u8 index)
{
    ot_scenecomm_expr_true_return(index >= HI_SCENE_PIPETYPE_NUM, HI_FAILURE);
    if (get_pipe_params()[index].module_state.static_pre_gamma != HI_TRUE) {
        return HI_SUCCESS;
    }

    hi_s32 i, ret;
    hi_isp_pregamma_attr pregamma_attr;

    ret = hi_mpi_isp_get_pregamma_attr(vi_pipe, &pregamma_attr);
    check_scene_ret(ret);

    pregamma_attr.enable = get_pipe_params()[index].static_pregamma.enable;
    for (i = 0; i < HI_ISP_PREGAMMA_NODE_NUM; i++) {
        pregamma_attr.table[i] = get_pipe_params()[index].static_pregamma.table[i];
    }

    ret = hi_mpi_isp_set_pregamma_attr(vi_pipe, &pregamma_attr);
    check_scene_ret(ret);
    return HI_SUCCESS;
}

hi_s32 ot_scene_set_static_blc(hi_vi_pipe vi_pipe, hi_u8 index)
{
    ot_scenecomm_expr_true_return(index >= HI_SCENE_PIPETYPE_NUM, HI_FAILURE);
    if (get_pipe_params()[index].module_state.static_blc != HI_TRUE) {
        return HI_SUCCESS;
    }

    hi_s32 i, j, ret;
    hi_isp_black_level_attr black_level_attr;

    ret = hi_mpi_isp_get_black_level_attr(vi_pipe, &black_level_attr);
    check_scene_ret(ret);

    black_level_attr.user_black_level_en = get_pipe_params()[index].static_blc.enable;

    for (i = 0; i < HI_ISP_WDR_MAX_FRAME_NUM; i++) {
        for (j = 0; j < HI_ISP_BAYER_CHN_NUM; j++) {
            black_level_attr.user_black_level[i][j] = get_pipe_params()[index].static_blc.user_offset;
        }
    }
    ret = hi_mpi_isp_set_black_level_attr(vi_pipe, &black_level_attr);
    check_scene_ret(ret);
    return HI_SUCCESS;
}

hi_s32 ot_scene_set_static_3dnr(hi_vi_pipe vi_pipe, hi_u8 index)
{
    return HI_SUCCESS;
}

hi_s32 ot_scene_set_dynamic_ldci(hi_vi_pipe vi_pipe, hi_u64 exposure, hi_u64 last_exposure, hi_u8 index)
{
    ot_scenecomm_expr_true_return(index >= HI_SCENE_PIPETYPE_NUM, HI_FAILURE);
    if ((get_pipe_params()[index].module_state.dynamic_ldci != HI_TRUE) || (last_exposure == exposure)) {
        return HI_SUCCESS;
    }

    hi_u32 exp_level;
    hi_s32 ret;
    hi_isp_ldci_attr ldci_attr;

    if (exposure != last_exposure) {
        check_scene_return_if_pause();
        ret = hi_mpi_isp_get_ldci_attr(vi_pipe, &ldci_attr);
        check_scene_ret(ret);

        exp_level = scene_get_level_ltoh(exposure, get_pipe_params()[index].dynamic_ldci.enable_cnt,
            get_pipe_params()[index].dynamic_ldci.enable_exp_thresh_ltoh);
        ot_scenecomm_expr_true_return(exp_level >= HI_SCENE_LDCI_EXPOSURE_MAX_COUNT, HI_FAILURE);
        ldci_attr.en = get_pipe_params()[index].dynamic_ldci.enable[exp_level];

        check_scene_return_if_pause();
        ret = hi_mpi_isp_set_ldci_attr(vi_pipe, &ldci_attr);
        check_scene_ret(ret);
    }
    return HI_SUCCESS;
}


hi_s32 ot_scene_set_dynamic_wdr_exposure(hi_vi_pipe vi_pipe, hi_u64 exposure, hi_u64 last_exposure, hi_u8 index)
{
    return HI_SUCCESS;
}

hi_s32 ot_scene_set_dynamic_fps(hi_vi_pipe vi_pipe, hi_u64 exposure, hi_u64 last_exposure, hi_u8 index)
{
    return HI_SUCCESS;
}

hi_s32 ot_scene_set_dynamic_ae(hi_vi_pipe vi_pipe, hi_u64 exposure, hi_u64 last_exposure, hi_u8 index)
{
    ot_scenecomm_expr_true_return(index >= HI_SCENE_PIPETYPE_NUM, HI_FAILURE);
    if (get_pipe_params()[index].module_state.dynamic_ae != HI_TRUE) {
        return HI_SUCCESS;
    }

    hi_u32 exp_level;
    hi_s32 ret;
    hi_isp_inner_state_info inner_state_info;
    hi_u32 actual_ratio;
    static hi_u32 last_ratio[MAX_ISP_PIPE_NUM] = {[0 ... MAX_ISP_PIPE_NUM - 1] = 0x40};
    hi_isp_exposure_attr exposure_attr;

    check_scene_return_if_pause();
    ret = hi_mpi_isp_query_inner_state_info(vi_pipe, &inner_state_info);
    check_scene_ret(ret);
    actual_ratio = inner_state_info.wdr_exp_ratio_actual[0];

    if ((exposure != last_exposure) || (last_ratio[vi_pipe] != actual_ratio)) {
        check_scene_return_if_pause();
        ret = hi_mpi_isp_get_exposure_attr(vi_pipe, &exposure_attr);
        check_scene_ret(ret);

        if (actual_ratio >= get_pipe_params()[index].dynamic_ae.wdr_ratio_threshold) {
            exposure_attr.advance_ae = get_pipe_params()[index].dynamic_ae.h_advance_ae;
        } else {
            exposure_attr.advance_ae = get_pipe_params()[index].dynamic_ae.l_advance_ae;
        }

        exp_level = scene_get_level_ltoh(exposure, get_pipe_params()[index].dynamic_ae.ae_exposure_cnt,
            get_pipe_params()[index].dynamic_ae.exp_ltoh_thresh);
        ot_scenecomm_expr_true_return(exp_level >= HI_SCENE_AE_EXPOSURE_MAX_COUNT, HI_FAILURE);
        if (exp_level == 0) {
            exposure_attr.auto_attr.compensation = get_pipe_params()[index].dynamic_ae.auto_compensation[exp_level];
            exposure_attr.auto_attr.max_hist_offset =
                get_pipe_params()[index].dynamic_ae.auto_max_hist_offset[exp_level];
        } else {
            exposure_attr.auto_attr.compensation =
                scene_interpulate(exposure, get_pipe_params()[index].dynamic_ae.exp_ltoh_thresh[exp_level - 1],
                    get_pipe_params()[index].dynamic_ae.auto_compensation[exp_level - 1],
                    get_pipe_params()[index].dynamic_ae.exp_ltoh_thresh[exp_level],
                    get_pipe_params()[index].dynamic_ae.auto_compensation[exp_level]);

            exposure_attr.auto_attr.max_hist_offset =
                scene_interpulate(exposure, get_pipe_params()[index].dynamic_ae.exp_ltoh_thresh[exp_level - 1],
                    get_pipe_params()[index].dynamic_ae.auto_max_hist_offset[exp_level - 1],
                    get_pipe_params()[index].dynamic_ae.exp_ltoh_thresh[exp_level],
                    get_pipe_params()[index].dynamic_ae.auto_max_hist_offset[exp_level]);
        }

        check_scene_return_if_pause();
        ret = hi_mpi_isp_set_exposure_attr(vi_pipe, &exposure_attr);
        check_scene_ret(ret);

        last_ratio[vi_pipe] = actual_ratio;
    }
    return HI_SUCCESS;
}

hi_s32 ot_scene_set_dynamic_fswdr(hi_vi_pipe vi_pipe, hi_u32 iso, hi_u32 last_iso, hi_u8 index, hi_u32 wdr_ratio)
{
    ot_scenecomm_expr_true_return(index >= HI_SCENE_PIPETYPE_NUM, HI_FAILURE);
    if (get_pipe_params()[index].module_state.dynamic_fswdr != HI_TRUE) {
        return HI_SUCCESS;
    }

    hi_s32 ret;
    hi_isp_wdr_fs_attr fswdr_attr;

    check_scene_return_if_pause();
    ret = hi_mpi_isp_get_fswdr_attr(vi_pipe, &fswdr_attr);
    check_scene_ret(ret);

    hi_u32  ratio_count = get_pipe_params()[index].dynamic_fswdr.ratio_count;
    hi_u32 *ratio_level_thresh = get_pipe_params()[index].dynamic_fswdr.ratio_level;
    hi_u32  ratio_level = (hi_u32)scene_get_level_ltoh_u32((hi_u32)wdr_ratio, ratio_count, ratio_level_thresh);
    ot_scenecomm_expr_true_return(ratio_level >= ISP_AUTO_RATIO_FSWDR_NUM, HI_FAILURE);

    if ((ratio_level == (ratio_count - 1)) || (ratio_level == 0)) {
        fswdr_attr.wdr_merge_mode = get_pipe_params()[index].dynamic_fswdr.wdr_merge_mode[ratio_level];
        fswdr_attr.wdr_combine.motion_comp = get_pipe_params()[index].dynamic_fswdr.motion_comp[ratio_level];
    } else {
        fswdr_attr.wdr_merge_mode = get_pipe_params()[index].dynamic_fswdr.wdr_merge_mode[ratio_level - 1];
        fswdr_attr.wdr_combine.motion_comp = get_pipe_params()[index].dynamic_fswdr.motion_comp[ratio_level - 1];
    }

    check_scene_return_if_pause();
    ret = hi_mpi_isp_set_fswdr_attr(vi_pipe, &fswdr_attr);
    check_scene_ret(ret);

    return HI_SUCCESS;
}

hi_s32 ot_scene_set_dynamic_drc(hi_vi_pipe vi_pipe, hi_u8 index, hi_u32 wdr_ratio, hi_u32 iso)
{
    ot_scenecomm_expr_true_return(index >= HI_SCENE_PIPETYPE_NUM, HI_FAILURE);
    if (get_pipe_params()[index].module_state.dynamic_drc != HI_TRUE) {
        return HI_SUCCESS;
    }

    hi_u32 iso_level;
    hi_u32 ratio_level = 0;
    hi_u32 interval = get_pipe_params()[index].dynamic_drc.interval;
    hi_u32 ratio_count = get_pipe_params()[index].dynamic_drc.ratio_count;
    hi_u32 *ratio_level_thresh = get_pipe_params()[index].dynamic_drc.ratio_level;
    hi_u32 iso_count = get_pipe_params()[index].dynamic_drc.iso_count;
    hi_u32 *iso_level_thresh = get_pipe_params()[index].dynamic_drc.iso_level;

    hi_isp_drc_attr isp_drc_attr;
    hi_isp_pub_attr pub_attr;
    hi_isp_inner_state_info inner_state_info;

    check_scene_return_if_pause();
    hi_s32 ret = hi_mpi_isp_get_pub_attr(vi_pipe, &pub_attr);
    check_scene_ret(ret);

    if (pub_attr.wdr_mode == HI_WDR_MODE_NONE) {
        return HI_SUCCESS;
    }

    check_scene_return_if_pause();
    ret = hi_mpi_isp_get_drc_attr(vi_pipe, &isp_drc_attr);
    check_scene_ret(ret);

    check_scene_return_if_pause();
    ret = hi_mpi_isp_query_inner_state_info(vi_pipe, &inner_state_info);
    check_scene_ret(ret);

    if (pub_attr.wdr_mode == HI_WDR_MODE_2To1_FRAME) {
        ratio_level =
            scene_get_level_ltoh_u32(inner_state_info.wdr_exp_ratio_actual[0], ratio_count, ratio_level_thresh);
    } else {
        ratio_level = scene_get_level_ltoh_u32(wdr_ratio, ratio_count, ratio_level_thresh);
    }
    iso_level = scene_get_level_ltoh_u32(iso, iso_count, iso_level_thresh);
    ot_scenecomm_expr_true_return(ratio_level >= HI_SCENE_DRC_RATIO_MAX_COUNT, HI_FAILURE);
    ot_scenecomm_expr_true_return(iso_level >= HI_SCENE_DRC_ISO_MAX_COUNT, HI_FAILURE);

    set_isp_attr_param param = { index, iso_level, iso_count, ratio_level, ratio_count, interval, wdr_ratio, iso };
    scene_set_isp_attr(param, ratio_level_thresh, iso_level_thresh, &isp_drc_attr);

    /* right shift 3 */
    ret = scene_set_tone_mapping_value(vi_pipe, index, (interval >> 3), inner_state_info, &isp_drc_attr);
    check_scene_ret(ret);

    check_scene_return_if_pause();
    ret = hi_mpi_isp_set_drc_attr(vi_pipe, &isp_drc_attr);
    check_scene_ret(ret);

    return HI_SUCCESS;
}

hi_s32 ot_scene_set_dynamic_linear_drc(hi_vi_pipe vi_pipe, hi_u64 iso, hi_u64 last_iso, hi_u8 index)
{
    ot_scenecomm_expr_true_return(index >= HI_SCENE_PIPETYPE_NUM, HI_FAILURE);
    if ((get_pipe_params()[index].module_state.dynamic_linear_drc != HI_TRUE) || (iso == last_iso)) {
        return HI_SUCCESS;
    }

    hi_s32 ret;
    hi_isp_drc_attr isp_drc_attr;

    check_scene_return_if_pause();
    ret = hi_mpi_isp_get_drc_attr(vi_pipe, &isp_drc_attr);
    check_scene_ret(ret);

    ret = scene_set_isp_drc_attr(index, iso, &isp_drc_attr);
    check_scene_ret(ret);

    check_scene_return_if_pause();
    ret = hi_mpi_isp_set_drc_attr(vi_pipe, &isp_drc_attr);
    check_scene_ret(ret);
    return HI_SUCCESS;
}

static hi_s32 scene_get_dynamic_dehaze_str(hi_vi_pipe vi_pipe, hi_u8 index, hi_u8 exp_level,
    hi_u64 *manual_str_l, hi_u64 *manual_str_r)
{
    hi_s32 ret = HI_SUCCESS;
    hi_isp_inner_state_info state_info;
    hi_u8 is_ratio_higher = 0;

    check_scene_return_if_pause();
    ret = hi_mpi_isp_query_inner_state_info(vi_pipe, &state_info);
    check_scene_ret(ret);

    is_ratio_higher =
        (state_info.wdr_exp_ratio_actual[0] > get_pipe_params()[index].dynamic_dehaze.wdr_ratio_threshold) ? 1 : 0;
    if (is_ratio_higher == 1) {
        *manual_str_l = (hi_u64)get_pipe_params()[index].dynamic_dehaze.manual_strengther[exp_level - 1];
        *manual_str_r = (hi_u64)get_pipe_params()[index].dynamic_dehaze.manual_strengther[exp_level];
    } else {
        *manual_str_l = (hi_u64)get_pipe_params()[index].dynamic_dehaze.manual_strength[exp_level - 1];
        *manual_str_r = (hi_u64)get_pipe_params()[index].dynamic_dehaze.manual_strength[exp_level];
    }

    return ret;
}

hi_s32 ot_scene_set_dynamic_dehaze(hi_vi_pipe vi_pipe, hi_u64 exposure, hi_u64 last_exposure, hi_u8 index)
{
    ot_scenecomm_expr_true_return(index >= HI_SCENE_PIPETYPE_NUM, HI_FAILURE);
    if ((get_pipe_params()[index].module_state.dynamic_dehaze != HI_TRUE)) {
        return HI_SUCCESS;
    }

    hi_s32 ret;
    hi_isp_dehaze_attr dehaze_attr;
    hi_u32 exp_level;
    hi_u64 man_str_l = 0;
    hi_u64 man_str_r = 0;
    hi_isp_inner_state_info state_info;
    static hi_u32 last_ratio[MAX_ISP_PIPE_NUM] = {[0 ... MAX_ISP_PIPE_NUM - 1] = 0x40};
    hi_u32 actual_ratio;

    ret = hi_mpi_isp_query_inner_state_info(vi_pipe, &state_info);
    check_scene_ret(ret);

    actual_ratio = state_info.wdr_exp_ratio_actual[0];
    if ((exposure != last_exposure) || (last_ratio[vi_pipe] != actual_ratio)) {
        check_scene_return_if_pause();
        ret  = hi_mpi_isp_get_dehaze_attr(vi_pipe, &dehaze_attr);
        check_scene_ret(ret);

        exp_level = scene_get_level_ltoh(exposure, get_pipe_params()[index].dynamic_dehaze.exp_thresh_cnt,
            get_pipe_params()[index].dynamic_dehaze.exp_thresh_ltoh);
        ot_scenecomm_expr_true_return(exp_level >= HI_SCENE_DEHAZE_EXPOSURE_MAX_COUNT, HI_FAILURE);

        ret = scene_get_dynamic_dehaze_str(vi_pipe, index, exp_level, &man_str_l, &man_str_r);
        check_scene_ret(ret);

        if (dehaze_attr.op_type == 0) {
            if ((exp_level == 0) || (exp_level == (get_pipe_params()[index].dynamic_dehaze.exp_thresh_cnt - 1))) {
                dehaze_attr.auto_attr.strength = man_str_r;
            } else {
                dehaze_attr.auto_attr.strength =
                    scene_interpulate(exposure, get_pipe_params()[index].dynamic_dehaze.exp_thresh_ltoh[exp_level - 1],
                    man_str_l, get_pipe_params()[index].dynamic_dehaze.exp_thresh_ltoh[exp_level], man_str_r);
            }
        } else if (dehaze_attr.op_type == 1) {
            if ((exp_level == 0) || (exp_level == (get_pipe_params()[index].dynamic_dehaze.exp_thresh_cnt - 1))) {
                dehaze_attr.manual_attr.strength = man_str_r;
            } else {
                dehaze_attr.manual_attr.strength =
                    scene_interpulate(exposure, get_pipe_params()[index].dynamic_dehaze.exp_thresh_ltoh[exp_level - 1],
                    man_str_l, get_pipe_params()[index].dynamic_dehaze.exp_thresh_ltoh[exp_level], man_str_r);
            }
        }

        check_scene_return_if_pause();
        ret = hi_mpi_isp_set_dehaze_attr(vi_pipe, &dehaze_attr);
        check_scene_ret(ret);

        last_ratio[vi_pipe] = actual_ratio;
    }

    return HI_SUCCESS;
}

hi_void ot_scene_dynamic_nr_param_clip(hi_isp_nr_attr *nr_attr)
{
    hi_u32 i;

    for (i = 0; i < HI_ISP_WDR_MAX_FRAME_NUM; i++) {
        if (nr_attr->wdr_cfg.md_fusion_strength[i] > NR_WDR_STRENGTH_MAX) {
            nr_attr->wdr_cfg.md_fusion_strength[i] = NR_WDR_STRENGTH_MAX;
        }
        if (nr_attr->wdr_cfg.md_wdr_strength[i] > NR_WDR_STRENGTH_MAX) {
            nr_attr->wdr_cfg.md_wdr_strength[i] = NR_WDR_STRENGTH_MAX;
        }
        if (nr_attr->wdr_cfg.snr_sfm0_fusion_strength[i] > NR_WDR_STRENGTH_MAX) {
            nr_attr->wdr_cfg.snr_sfm0_fusion_strength[i] = NR_WDR_STRENGTH_MAX;
        }
        if (nr_attr->wdr_cfg.snr_sfm0_wdr_strength[i] > NR_WDR_STRENGTH_MAX) {
            nr_attr->wdr_cfg.snr_sfm0_wdr_strength[i] = NR_WDR_STRENGTH_MAX;
        }
    }
}

hi_s32 ot_scene_set_dynamic_nr(hi_vi_pipe vi_pipe, hi_u8 index, hi_u32 wdr_ratio, hi_u32 iso)
{
    ot_scenecomm_expr_true_return(index >= HI_SCENE_PIPETYPE_NUM, HI_FAILURE);
    if (get_pipe_params()[index].module_state.dynamic_nr != HI_TRUE) {
        return HI_SUCCESS;
    }

    hi_s32 ret;
    hi_u32 i, ratio_index;
    hi_u32 ratio_count = get_pipe_params()[index].dynamic_nr.ratio_count;
    hi_u32 *level = get_pipe_params()[index].dynamic_nr.ratio_level;
    ot_scene_nr_para nr_para = {{0}};
    hi_isp_nr_attr nr_attr;

    check_scene_return_if_pause();
    ret = hi_mpi_isp_get_nr_attr(vi_pipe, &nr_attr);
    check_scene_ret(ret);

    ratio_index = scene_get_level_ltoh_u32(wdr_ratio, ratio_count, level);
    ot_scenecomm_expr_true_return(ratio_index >= HI_SCENE_DRC_RATIO_MAX_COUNT, HI_FAILURE);

    if ((ratio_index == 0) || (ratio_index == (ratio_count - 1))) {
        for (i = 0; i < HI_ISP_WDR_MAX_FRAME_NUM; i++) {
            nr_para.md_fusion_incr[i] = get_pipe_params()[index].dynamic_nr.md_fusion_frame_str_incr[i][ratio_index];
            nr_para.sfm0_fusion_incr[i] =
                get_pipe_params()[index].dynamic_nr.snr_sfm0_fusion_frame_str_incr[i][ratio_index];

            nr_para.md_wdr_incr[i] = get_pipe_params()[index].dynamic_nr.md_wdr_frame_str_incr[i][ratio_index];
            nr_para.sfm0_wdr_incr[i] = get_pipe_params()[index].dynamic_nr.snr_sfm0_wdr_frame_str_incr[i][ratio_index];
        }
    } else {
        for (i = 0; i < HI_ISP_WDR_MAX_FRAME_NUM; i++) {
            nr_para.md_fusion_incr[i] = scene_interpulate(wdr_ratio, level[ratio_index - 1],
                get_pipe_params()[index].dynamic_nr.md_fusion_frame_str_incr[i][ratio_index - 1],
                level[ratio_index], get_pipe_params()[index].dynamic_nr.md_fusion_frame_str_incr[i][ratio_index]);

            nr_para.sfm0_fusion_incr[i] = scene_interpulate(wdr_ratio, level[ratio_index - 1],
                get_pipe_params()[index].dynamic_nr.snr_sfm0_fusion_frame_str_incr[i][ratio_index - 1],
                level[ratio_index], get_pipe_params()[index].dynamic_nr.snr_sfm0_fusion_frame_str_incr[i][ratio_index]);

            nr_para.md_wdr_incr[i] = scene_interpulate(wdr_ratio, level[ratio_index - 1],
                get_pipe_params()[index].dynamic_nr.md_wdr_frame_str_incr[i][ratio_index - 1],
                level[ratio_index], get_pipe_params()[index].dynamic_nr.md_wdr_frame_str_incr[i][ratio_index]);

            nr_para.sfm0_wdr_incr[i] = scene_interpulate(wdr_ratio, level[ratio_index - 1],
                get_pipe_params()[index].dynamic_nr.snr_sfm0_wdr_frame_str_incr[i][ratio_index - 1],
                level[ratio_index], get_pipe_params()[index].dynamic_nr.snr_sfm0_wdr_frame_str_incr[i][ratio_index]);
        }
    }

    ret = scene_set_nr_attr_para(index, iso, &nr_attr, &nr_para);
    check_scene_ret(ret);

    ret = scene_set_nr_wdr_ratio_para(vi_pipe, index, wdr_ratio, &nr_attr);
    check_scene_ret(ret);

    ot_scene_dynamic_nr_param_clip(&nr_attr);

    check_scene_return_if_pause();
    ret = hi_mpi_isp_set_nr_attr(vi_pipe, &nr_attr);
    check_scene_ret(ret);
    return HI_SUCCESS;
}

hi_s32 ot_scene_set_dynamic_dpc(hi_vi_pipe vi_pipe, hi_u64 iso, hi_u64 last_iso, hi_u8 index)
{
    ot_scenecomm_expr_true_return(index >= HI_SCENE_PIPETYPE_NUM, HI_FAILURE);
    if ((get_pipe_params()[index].module_state.dynamic_dpc != HI_TRUE) || (last_iso == iso)) {
        return HI_SUCCESS;
    }

    hi_s32 ret;
    hi_isp_dp_dynamic_attr dp_dynamic_attr;

    hi_u32  iso_count = get_pipe_params()[index].dynamic_dpc.iso_count;
    hi_u32 *iso_level_thresh = get_pipe_params()[index].dynamic_dpc.iso_level;
    hi_u32 iso_level = (hi_u32)scene_get_level_ltoh_u32((hi_u32)iso, iso_count, iso_level_thresh);
    ot_scenecomm_expr_true_return(iso_level >= ISP_AUTO_ISO_DPC_NUM, HI_FAILURE);

    check_scene_return_if_pause();
    ret = hi_mpi_isp_get_dp_dynamic_attr(vi_pipe, &dp_dynamic_attr);
    check_scene_ret(ret);

    if ((iso_level == (iso_count - 1)) || (iso_level == 0)) {
        dp_dynamic_attr.sup_twinkle_en = get_pipe_params()[index].dynamic_dpc.sup_twinkle_en[iso_level];
        dp_dynamic_attr.soft_thr = (hi_s8)get_pipe_params()[index].dynamic_dpc.soft_thr[iso_level];
        dp_dynamic_attr.soft_slope = get_pipe_params()[index].dynamic_dpc.soft_slope[iso_level];
    } else {
        hi_u32 mid_iso = (hi_u32)iso;
        hi_u32 left_iso = iso_level_thresh[iso_level - 1];
        hi_u32 right_iso = iso_level_thresh[iso_level];

        dp_dynamic_attr.sup_twinkle_en = get_pipe_params()[index].dynamic_dpc.sup_twinkle_en[iso_level - 1];
        dp_dynamic_attr.soft_thr = scene_interpulate(mid_iso, left_iso,
            get_pipe_params()[index].dynamic_dpc.soft_thr[iso_level - 1], right_iso,
            get_pipe_params()[index].dynamic_dpc.soft_thr[iso_level]);
        dp_dynamic_attr.soft_slope = scene_interpulate(mid_iso, left_iso,
            get_pipe_params()[index].dynamic_dpc.soft_slope[iso_level - 1], right_iso,
            get_pipe_params()[index].dynamic_dpc.soft_slope[iso_level]);
    }

    check_scene_return_if_pause();
    ret = hi_mpi_isp_set_dp_dynamic_attr(vi_pipe, &dp_dynamic_attr);
    check_scene_ret(ret);

    return HI_SUCCESS;
}

hi_s32 ot_scene_set_dynamic_dpc_ratio(hi_vi_pipe vi_pipe, hi_u8 index, hi_u32 wdr_ratio)
{
    ot_scenecomm_expr_true_return(index >= HI_SCENE_PIPETYPE_NUM, HI_FAILURE);
    if (get_pipe_params()[index].module_state.dynamic_dpc != HI_TRUE) {
        return HI_SUCCESS;
    }

    hi_s32 ret;
    hi_isp_dp_dynamic_attr dp_dynamic_attr;
    hi_u32 ratio_index;
    hi_u32 ratio_count = get_pipe_params()[index].dynamic_dpc.ratio_count;
    hi_u32 *level = get_pipe_params()[index].dynamic_dpc.ratio_level;

    check_scene_return_if_pause();
    ret = hi_mpi_isp_get_dp_dynamic_attr(vi_pipe, &dp_dynamic_attr);
    check_scene_ret(ret);

    ratio_index = scene_get_level_ltoh_u32(wdr_ratio, ratio_count, level);
    ot_scenecomm_expr_true_return(ratio_index >= ISP_AUTO_ISO_DPC_NUM, HI_FAILURE);

    if ((ratio_index == 0) || (ratio_index == (ratio_count - 1))) {
        dp_dynamic_attr.bright_strength = get_pipe_params()[index].dynamic_dpc.bright_strength[ratio_index];
        dp_dynamic_attr.dark_strength   = get_pipe_params()[index].dynamic_dpc.dark_strength[ratio_index];
    } else {
        dp_dynamic_attr.bright_strength = scene_interpulate(wdr_ratio, level[ratio_index - 1],
            get_pipe_params()[index].dynamic_dpc.bright_strength[ratio_index - 1],
            level[ratio_index], get_pipe_params()[index].dynamic_dpc.bright_strength[ratio_index]);
        dp_dynamic_attr.dark_strength= scene_interpulate(wdr_ratio, level[ratio_index - 1],
            get_pipe_params()[index].dynamic_dpc.dark_strength[ratio_index - 1],
            level[ratio_index], get_pipe_params()[index].dynamic_dpc.dark_strength[ratio_index]);
    }

    check_scene_return_if_pause();
    ret = hi_mpi_isp_set_dp_dynamic_attr(vi_pipe, &dp_dynamic_attr);
    check_scene_ret(ret);
    return HI_SUCCESS;
}

hi_s32 ot_scene_set_dynamic_shading(hi_vi_pipe vi_pipe, hi_u64 exposure, hi_u64 last_exposure, hi_u8 index)
{
    ot_scenecomm_expr_true_return(index >= HI_SCENE_PIPETYPE_NUM, HI_FAILURE);
    if ((get_pipe_params()[index].module_state.dynamic_shading != HI_TRUE) || (last_exposure == exposure)) {
        return HI_SUCCESS;
    }

    hi_u32 exp_level;
    hi_s32 ret;
    hi_isp_shading_attr shading_attr;

    if (exposure != last_exposure) {
        check_scene_return_if_pause();
        ret = hi_mpi_isp_get_mesh_shading_attr(vi_pipe, &shading_attr);
        check_scene_ret(ret);

        exp_level = scene_get_level_ltoh(exposure, get_pipe_params()[index].dynamic_shading.exp_thresh_cnt,
            get_pipe_params()[index].dynamic_shading.exp_thresh_ltoh);
        ot_scenecomm_expr_true_return(exp_level >= HI_SCENE_SHADING_EXPOSURE_MAX_COUNT, HI_FAILURE);
        if ((exp_level == 0) || (exp_level == (get_pipe_params()[index].dynamic_shading.exp_thresh_cnt - 1))) {
            shading_attr.mesh_strength = get_pipe_params()[index].dynamic_shading.mesh_strength[exp_level];
        } else {
            shading_attr.mesh_strength =
                scene_interpulate(exposure, get_pipe_params()[index].dynamic_shading.exp_thresh_ltoh[exp_level - 1],
                    get_pipe_params()[index].dynamic_shading.mesh_strength[exp_level - 1],
                    get_pipe_params()[index].dynamic_shading.exp_thresh_ltoh[exp_level],
                    get_pipe_params()[index].dynamic_shading.mesh_strength[exp_level]);
        }

        check_scene_return_if_pause();
        ret = hi_mpi_isp_set_mesh_shading_attr(vi_pipe, &shading_attr);
        check_scene_ret(ret);
    }
    return HI_SUCCESS;
}

hi_s32 ot_scene_set_dynamic_clut(hi_vi_pipe vi_pipe, hi_u64 iso, hi_u64 last_iso, hi_u8 index)
{
    ot_scenecomm_expr_true_return(index >= HI_SCENE_PIPETYPE_NUM, HI_FAILURE);
    if ((get_pipe_params()[index].module_state.dynamic_clut != HI_TRUE) || (iso == last_iso)) {
        return HI_SUCCESS;
    }

    hi_s32 ret;
    hi_u32 iso_level;
    hi_u32 iso_count = get_pipe_params()[index].dynamic_clut.iso_count;
    hi_u32 *piso_level_thresh = get_pipe_params()[index].dynamic_clut.iso_level;
    hi_u32 gain_r, gain_g, gain_b;
    hi_isp_clut_attr isp_clut_attr;

    check_scene_return_if_pause();
    ret = hi_mpi_isp_get_clut_attr(vi_pipe, &isp_clut_attr);
    check_scene_ret(ret);

    iso_level = scene_get_level_ltoh_u32((hi_u32)iso, iso_count, piso_level_thresh);
    ot_scenecomm_expr_true_return(iso_level >= HI_SCENE_ISO_STRENGTH_NUM, HI_FAILURE);
    if ((iso_level == 0) || (iso_level == (iso_count - 1))) {
        gain_r = get_pipe_params()[index].dynamic_clut.gain_r[iso_level];
        gain_g = get_pipe_params()[index].dynamic_clut.gain_g[iso_level];
        gain_b = get_pipe_params()[index].dynamic_clut.gain_b[iso_level];
    } else {
        gain_r = scene_interpulate(iso, piso_level_thresh[iso_level - 1],
            get_pipe_params()[index].dynamic_clut.gain_r[iso_level - 1], piso_level_thresh[iso_level],
            get_pipe_params()[index].dynamic_clut.gain_r[iso_level]);

        gain_g = scene_interpulate(iso, piso_level_thresh[iso_level - 1],
            get_pipe_params()[index].dynamic_clut.gain_g[iso_level - 1], piso_level_thresh[iso_level],
            get_pipe_params()[index].dynamic_clut.gain_g[iso_level]);

        gain_b = scene_interpulate(iso, piso_level_thresh[iso_level - 1],
            get_pipe_params()[index].dynamic_clut.gain_b[iso_level - 1], piso_level_thresh[iso_level],
            get_pipe_params()[index].dynamic_clut.gain_b[iso_level]);
    }
    isp_clut_attr.gain_r = gain_r;
    isp_clut_attr.gain_g = gain_g;
    isp_clut_attr.gain_b = gain_b;

    check_scene_return_if_pause();
    ret = hi_mpi_isp_set_clut_attr(vi_pipe, &isp_clut_attr);
    check_scene_ret(ret);
    return HI_SUCCESS;
}


hi_s32 ot_scene_set_dynamic_ca(hi_vi_pipe vi_pipe, hi_u64 iso, hi_u64 last_iso, hi_u8 index)
{
    return HI_SUCCESS;
}


hi_s32 ot_scene_set_dynamic_blc(hi_vi_pipe vi_pipe, hi_u64 iso, hi_u64 last_iso, hi_u8 index)
{
    ot_scenecomm_expr_true_return(index >= HI_SCENE_PIPETYPE_NUM, HI_FAILURE);
    if ((get_pipe_params()[index].module_state.dynamic_blc != HI_TRUE) || (iso == last_iso)) {
        return HI_SUCCESS;
    }

    hi_s32 ret;
    hi_isp_black_level_attr isp_blc_attr;

    hi_u32 iso_count = get_pipe_params()[index].dynamic_blc.blc_count;
    hi_u32 *iso_level_thresh = get_pipe_params()[index].dynamic_blc.iso_thresh;
    hi_u32 iso_level = (hi_u32)scene_get_level_ltoh_u32((hi_u32)iso, iso_count, iso_level_thresh);
    ot_scenecomm_expr_true_return(iso_level >= ISP_AUTO_ISO_BLC_NUM, HI_FAILURE);

    check_scene_return_if_pause();
    ret = hi_mpi_isp_get_black_level_attr(vi_pipe, &isp_blc_attr);
    check_scene_ret(ret);
    isp_blc_attr.black_level_mode = get_pipe_params()[index].dynamic_blc.black_level_mode;

    if ((iso_level == (iso_count - 1)) || (iso_level == 0)) {
        isp_blc_attr.manual_attr.black_level[0][0] = get_pipe_params()[index].dynamic_blc.blc_r[iso_level];
        isp_blc_attr.manual_attr.black_level[0][1] = get_pipe_params()[index].dynamic_blc.blc_gr[iso_level];
        isp_blc_attr.manual_attr.black_level[0][2] = /* index2 */
            get_pipe_params()[index].dynamic_blc.blc_gb[iso_level];
        isp_blc_attr.manual_attr.black_level[0][3] = /* index3 */
            get_pipe_params()[index].dynamic_blc.blc_b[iso_level];
    } else {
        hi_u32 mid_iso = (hi_u32)iso;
        hi_u32 left_iso = iso_level_thresh[iso_level - 1];
        hi_u32 right_iso = iso_level_thresh[iso_level];

        isp_blc_attr.manual_attr.black_level[0][0] = scene_interpulate(mid_iso, left_iso,
            get_pipe_params()[index].dynamic_blc.blc_r[iso_level - 1], right_iso,
            get_pipe_params()[index].dynamic_blc.blc_r[iso_level]);
        isp_blc_attr.manual_attr.black_level[0][1] = scene_interpulate(mid_iso, left_iso,
            get_pipe_params()[index].dynamic_blc.blc_gr[iso_level - 1], right_iso,
            get_pipe_params()[index].dynamic_blc.blc_gr[iso_level]);
        isp_blc_attr.manual_attr.black_level[0][2] = scene_interpulate(mid_iso, left_iso, /* index2 */
            get_pipe_params()[index].dynamic_blc.blc_gb[iso_level - 1], right_iso,
            get_pipe_params()[index].dynamic_blc.blc_gb[iso_level]);
        isp_blc_attr.manual_attr.black_level[0][3] = scene_interpulate(mid_iso, left_iso, /* index3 */
            get_pipe_params()[index].dynamic_blc.blc_b[iso_level - 1], right_iso,
            get_pipe_params()[index].dynamic_blc.blc_b[iso_level]);
    }
    check_scene_return_if_pause();
    ret = hi_mpi_isp_set_black_level_attr(vi_pipe, &isp_blc_attr);
    check_scene_ret(ret);

    return HI_SUCCESS;
}

hi_s32 ot_scene_set_dynamic_venc_bitrate(hi_vi_pipe vi_pipe, hi_u64 iso, hi_u64 last_iso, hi_u8 index)
{
    ot_scenecomm_expr_true_return(index >= HI_SCENE_PIPETYPE_NUM, HI_FAILURE);
    if (get_pipe_params()[index].module_state.dynamic_iso_venc != HI_TRUE) {
        return HI_SUCCESS;
    }

    hi_venc_chn_attr venc_chn_attr;
    hi_venc_rc_param rc_param;

    if (iso != last_iso) {
        check_scene_return_if_pause();
        hi_s32 ret = hi_mpi_venc_get_chn_attr(vi_pipe, &venc_chn_attr);
        check_scene_ret(ret);

        ret = hi_mpi_venc_get_rc_param(vi_pipe, &rc_param);
        check_scene_ret(ret);

        hi_u32 exp_level = scene_get_level_ltoh_u32((hi_u32)iso,
            get_pipe_params()[index].dynamic_venc_bitrate.iso_thresh_cnt,
            get_pipe_params()[index].dynamic_venc_bitrate.iso_thresh_ltoh);
        ot_scenecomm_expr_true_return(exp_level >= HI_SCENE_SHADING_EXPOSURE_MAX_COUNT, HI_FAILURE);
        if ((exp_level == 0) || (exp_level == (get_pipe_params()[index].dynamic_venc_bitrate.iso_thresh_cnt - 1))) {
            if (venc_chn_attr.venc_attr.type == HI_PT_H264) {
                rc_param.h264_avbr_param.chg_pos =
                    get_pipe_params()[index].dynamic_venc_bitrate.manual_percent[exp_level];
            } else {
                rc_param.h265_avbr_param.chg_pos =
                    get_pipe_params()[index].dynamic_venc_bitrate.manual_percent[exp_level];
            }
        } else if (exp_level > HI_SCENE_SHADING_EXPOSURE_MAX_COUNT) {
            return HI_FAILURE;
        } else {
            if (venc_chn_attr.venc_attr.type == HI_PT_H264) {
                rc_param.h264_avbr_param.chg_pos = (hi_s32)scene_interpulate((hi_u32)iso,
                    get_pipe_params()[index].dynamic_venc_bitrate.iso_thresh_ltoh[exp_level - 1],
                    get_pipe_params()[index].dynamic_venc_bitrate.manual_percent[exp_level - 1],
                    get_pipe_params()[index].dynamic_venc_bitrate.iso_thresh_ltoh[exp_level],
                    get_pipe_params()[index].dynamic_venc_bitrate.manual_percent[exp_level]);
            } else {
                rc_param.h265_avbr_param.chg_pos = (hi_s32)scene_interpulate((hi_u32)iso,
                    get_pipe_params()[index].dynamic_venc_bitrate.iso_thresh_ltoh[exp_level - 1],
                    get_pipe_params()[index].dynamic_venc_bitrate.manual_percent[exp_level - 1],
                    get_pipe_params()[index].dynamic_venc_bitrate.iso_thresh_ltoh[exp_level],
                    get_pipe_params()[index].dynamic_venc_bitrate.manual_percent[exp_level]);
            }
        }
        check_scene_return_if_pause();
        ret = hi_mpi_venc_set_rc_param(vi_pipe, &rc_param);
        check_scene_ret(ret);
    }
    return HI_SUCCESS;
}

hi_s32 ot_scene_set_dynamic_video_gamma(hi_vi_pipe vi_pipe, hi_u64 exposure, hi_u64 last_exposure, hi_u8 index)
{
    ot_scenecomm_expr_true_return(index >= HI_SCENE_PIPETYPE_NUM, HI_FAILURE);
    if (get_pipe_params()[index].module_state.dynamic_gamma != HI_TRUE) {
        return HI_SUCCESS;
    }

    hi_u32 i, j;
    hi_u32 exp_level;
    static hi_u32 last_exp_level[HI_SCENE_PIPETYPE_NUM] = {0};
    hi_s32 ret;
    hi_isp_gamma_attr isp_gamma_attr;

    if (exposure != last_exposure) {
        exp_level = scene_get_level_ltoh(exposure, get_pipe_params()[index].dynamic_gamma.total_num,
            get_pipe_params()[index].dynamic_gamma.exp_thresh_htol);

        ot_scenecomm_expr_true_return(index >= HI_SCENE_PIPETYPE_NUM, HI_FAILURE);
        ot_scenecomm_expr_true_return(exp_level >= HI_SCENE_GAMMA_EXPOSURE_MAX_COUNT, HI_FAILURE);
        ot_scenecomm_expr_true_return(vi_pipe >= HI_SCENE_PIPETYPE_NUM, HI_FAILURE);
        for (i = 0; i < get_pipe_params()[index].dynamic_gamma.interval; i++) {
            check_scene_return_if_pause();
            ret = hi_mpi_isp_get_gamma_attr(vi_pipe, &isp_gamma_attr);
            check_scene_ret(ret);

            for (j = 0; j < HI_ISP_GAMMA_NODE_NUM; j++) {
                isp_gamma_attr.table[j] =
                    scene_time_filter(get_pipe_params()[index].dynamic_gamma.table[last_exp_level[vi_pipe]][j],
                        get_pipe_params()[index].dynamic_gamma.table[exp_level][j],
                        get_pipe_params()[index].dynamic_gamma.interval, i);
            }

            isp_gamma_attr.curve_type = HI_ISP_GAMMA_CURVE_USER_DEFINE;
            check_scene_return_if_pause();
            ret = hi_mpi_isp_set_gamma_attr(vi_pipe, &isp_gamma_attr);
            check_scene_ret(ret);
            usleep(30000); /* sleep 30000 us */
        }
        last_exp_level[vi_pipe] = exp_level;
    }
    return HI_SUCCESS;
}

hi_s32 ot_scene_set_dynamic_photo_gamma(hi_vi_pipe vi_pipe, hi_u64 exposure, hi_u64 last_exposure, hi_u8 index)
{
    return HI_SUCCESS;
}


hi_s32 ot_scene_set_dynamic_false_color(hi_vi_pipe vi_pipe, hi_u64 exposure, hi_u64 last_exposure, hi_u8 index)
{
    ot_scenecomm_expr_true_return(index >= HI_SCENE_PIPETYPE_NUM, HI_FAILURE);
    if ((get_pipe_params()[index].module_state.dynamic_false_color != HI_TRUE) || (last_exposure == exposure)) {
        return HI_SUCCESS;
    }

    hi_s32 ret;
    hi_u32 exp_level;
    hi_isp_anti_false_color_attr anti_false_color_attr;

    if (exposure != last_exposure) {
        check_scene_return_if_pause();
        ret = hi_mpi_isp_get_anti_false_color_attr(vi_pipe, &anti_false_color_attr);
        check_scene_ret(ret);

        exp_level = scene_get_level_ltoh_u32((hi_u32)exposure, get_pipe_params()[index].dynamic_false_color.total_num,
            get_pipe_params()[index].dynamic_false_color.false_color_exp_thresh);
        ot_scenecomm_expr_true_return(exp_level >= HI_SCENE_FALSECOLOR_EXPOSURE_MAX_COUNT, HI_FAILURE);
        anti_false_color_attr.manual_attr.strength =
            get_pipe_params()[index].dynamic_false_color.manual_strength[exp_level];

        check_scene_return_if_pause();
        ret = hi_mpi_isp_set_anti_false_color_attr(vi_pipe, &anti_false_color_attr);
        check_scene_ret(ret);
    }
    return HI_SUCCESS;
}

#ifdef MCF_SCENE_AUTO_USED
hi_void ot_scene_set_dynamic_mcf_border_part1(hi_mcf_alg_param *mcf_alg_attr, hi_u32 iso_level, hi_u8 index)
{
    mcf_alg_attr->mcf_color_hf_proc_cfg.color_hf_en = get_pipe_params()[index].dynamic_mcf.color_hf_en[iso_level];
    mcf_alg_attr->mcf_color_hf_proc_cfg.color_hf_gain = get_pipe_params()[index].dynamic_mcf.color_hf_gain[iso_level];

    mcf_alg_attr->mcf_hf_cfg.filter_proc_cfg.mono_flt_radius =
        get_pipe_params()[index].dynamic_mcf.hf_mono_flt_radius[iso_level];
    mcf_alg_attr->mcf_mf_cfg.filter_proc_cfg.mono_flt_radius =
        get_pipe_params()[index].dynamic_mcf.mf_mono_flt_radius[iso_level];
    mcf_alg_attr->mcf_lf_cfg.filter_proc_cfg.mono_flt_radius =
        get_pipe_params()[index].dynamic_mcf.lf_mono_flt_radius[iso_level];

    mcf_alg_attr->mcf_hf_cfg.detail_proc_cfg.fusion_det_gain =
        get_pipe_params()[index].dynamic_mcf.hf_fusion_det_gain[iso_level];
    mcf_alg_attr->mcf_mf_cfg.detail_proc_cfg.fusion_det_gain =
        get_pipe_params()[index].dynamic_mcf.mf_fusion_det_gain[iso_level];
    mcf_alg_attr->mcf_lf_cfg.detail_proc_cfg.fusion_det_gain =
        get_pipe_params()[index].dynamic_mcf.lf_fusion_det_gain[iso_level];

    mcf_alg_attr->mcf_hf_cfg.base_prco_cfg.fusion_adaptive_alpha_mode_cfg.fusion_mono_flat_en =
        get_pipe_params()[index].dynamic_mcf.fusion_mono_flat_en[iso_level];
    mcf_alg_attr->mcf_mf_cfg.base_prco_cfg.fusion_adaptive_alpha_mode_cfg.fusion_mono_flat_en =
        get_pipe_params()[index].dynamic_mcf.fusion_mono_flat_en[iso_level];
    mcf_alg_attr->mcf_lf_cfg.base_prco_cfg.fusion_adaptive_alpha_mode_cfg.fusion_mono_flat_en =
        get_pipe_params()[index].dynamic_mcf.fusion_mono_flat_en[iso_level];

    mcf_alg_attr->mcf_hf_cfg.base_prco_cfg.fusion_adaptive_alpha_mode_cfg.fusion_color_ratio_en =
        get_pipe_params()[index].dynamic_mcf.fusion_color_ratio_en[iso_level];
    mcf_alg_attr->mcf_mf_cfg.base_prco_cfg.fusion_adaptive_alpha_mode_cfg.fusion_color_ratio_en =
        get_pipe_params()[index].dynamic_mcf.fusion_color_ratio_en[iso_level];
    mcf_alg_attr->mcf_lf_cfg.base_prco_cfg.fusion_adaptive_alpha_mode_cfg.fusion_color_ratio_en =
        get_pipe_params()[index].dynamic_mcf.fusion_color_ratio_en[iso_level];

    mcf_alg_attr->mcf_hf_cfg.base_prco_cfg.fusion_adaptive_alpha_mode_cfg.fusion_mono_ratio_en =
        get_pipe_params()[index].dynamic_mcf.fusion_mono_ratio_en[iso_level];
    mcf_alg_attr->mcf_mf_cfg.base_prco_cfg.fusion_adaptive_alpha_mode_cfg.fusion_mono_ratio_en =
        get_pipe_params()[index].dynamic_mcf.fusion_mono_ratio_en[iso_level];
    mcf_alg_attr->mcf_lf_cfg.base_prco_cfg.fusion_adaptive_alpha_mode_cfg.fusion_mono_ratio_en =
        get_pipe_params()[index].dynamic_mcf.fusion_mono_ratio_en[iso_level];
}

hi_void ot_scene_set_dynamic_mcf_border_part2(hi_mcf_alg_param *mcf_alg_attr, hi_u32 iso_level, hi_u8 index)
{
    hi_u32 i;
    for (i = 0; i < HI_MCF_CC_UV_GAIN_LUT_NUM; i++) {
        mcf_alg_attr->mcf_color_correct_cfg.cc_uv_gain_lut[i] =
            get_pipe_params()[index].dynamic_mcf.cc_uv_gain_lut[iso_level][i];
    }
    for (i = 0; i < HI_MCF_WEIGHT_LUT_NUM; i++) {
        mcf_alg_attr->mcf_hf_cfg.base_prco_cfg.fusion_adaptive_alpha_mode_cfg.fusion_alpha_lut[i]
            = get_pipe_params()[index].dynamic_mcf.hf_fusion_alpha_lut[iso_level][i];
        mcf_alg_attr->mcf_mf_cfg.base_prco_cfg.fusion_adaptive_alpha_mode_cfg.fusion_alpha_lut[i]
            = get_pipe_params()[index].dynamic_mcf.mf_fusion_alpha_lut[iso_level][i];
        mcf_alg_attr->mcf_lf_cfg.base_prco_cfg.fusion_adaptive_alpha_mode_cfg.fusion_alpha_lut[i]
            = get_pipe_params()[index].dynamic_mcf.lf_fusion_alpha_lut[iso_level][i];

        mcf_alg_attr->mcf_hf_cfg.base_prco_cfg.fusion_adaptive_alpha_mode_cfg.fusion_color_ratio_lut[i]
            = get_pipe_params()[index].dynamic_mcf.fusion_color_ratio_lut[iso_level][i];
        mcf_alg_attr->mcf_mf_cfg.base_prco_cfg.fusion_adaptive_alpha_mode_cfg.fusion_color_ratio_lut[i]
            = get_pipe_params()[index].dynamic_mcf.fusion_color_ratio_lut[iso_level][i];
        mcf_alg_attr->mcf_lf_cfg.base_prco_cfg.fusion_adaptive_alpha_mode_cfg.fusion_color_ratio_lut[i]
            = get_pipe_params()[index].dynamic_mcf.fusion_color_ratio_lut[iso_level][i];

        mcf_alg_attr->mcf_hf_cfg.detail_proc_cfg.fusion_mono_det_lut[i]
            = get_pipe_params()[index].dynamic_mcf.hf_fusion_mono_det_lut[iso_level][i];
        mcf_alg_attr->mcf_mf_cfg.detail_proc_cfg.fusion_mono_det_lut[i]
            = get_pipe_params()[index].dynamic_mcf.mf_fusion_mono_det_lut[iso_level][i];
        mcf_alg_attr->mcf_lf_cfg.detail_proc_cfg.fusion_mono_det_lut[i]
            = get_pipe_params()[index].dynamic_mcf.lf_fusion_mono_det_lut[iso_level][i];
    }
}


hi_void ot_scene_set_dynamic_mcf_interpulate_part1(hi_mcf_alg_param *mcf_alg_attr, hi_u32 mid_iso,
                                                   hi_u32 *iso_level_thresh, hi_u32 iso_level, hi_u8 index)
{
    hi_u32 left_iso = iso_level_thresh[iso_level - 1];
    hi_u32 right_iso = iso_level_thresh[iso_level];
    mcf_alg_attr->mcf_color_hf_proc_cfg.color_hf_en = scene_interpulate(mid_iso, left_iso,
        get_pipe_params()[index].dynamic_mcf.color_hf_en[iso_level - 1], right_iso,
        get_pipe_params()[index].dynamic_mcf.color_hf_en[iso_level]);
    mcf_alg_attr->mcf_color_hf_proc_cfg.color_hf_gain = scene_interpulate(mid_iso, left_iso,
        get_pipe_params()[index].dynamic_mcf.color_hf_gain[iso_level - 1], right_iso,
        get_pipe_params()[index].dynamic_mcf.color_hf_gain[iso_level]);

    mcf_alg_attr->mcf_hf_cfg.filter_proc_cfg.mono_flt_radius = scene_interpulate(mid_iso, left_iso,
        get_pipe_params()[index].dynamic_mcf.hf_mono_flt_radius[iso_level - 1], right_iso,
        get_pipe_params()[index].dynamic_mcf.hf_mono_flt_radius[iso_level]);
    mcf_alg_attr->mcf_mf_cfg.filter_proc_cfg.mono_flt_radius = scene_interpulate(mid_iso, left_iso,
        get_pipe_params()[index].dynamic_mcf.mf_mono_flt_radius[iso_level - 1], right_iso,
        get_pipe_params()[index].dynamic_mcf.mf_mono_flt_radius[iso_level]);
    mcf_alg_attr->mcf_lf_cfg.filter_proc_cfg.mono_flt_radius = scene_interpulate(mid_iso, left_iso,
        get_pipe_params()[index].dynamic_mcf.lf_mono_flt_radius[iso_level - 1], right_iso,
        get_pipe_params()[index].dynamic_mcf.lf_mono_flt_radius[iso_level]);

    mcf_alg_attr->mcf_hf_cfg.base_prco_cfg.fusion_adaptive_alpha_mode_cfg.fusion_mono_flat_en =
        scene_interpulate(mid_iso, left_iso, get_pipe_params()[index].dynamic_mcf.fusion_mono_flat_en[iso_level - 1],
        right_iso, get_pipe_params()[index].dynamic_mcf.fusion_mono_flat_en[iso_level]);
    mcf_alg_attr->mcf_mf_cfg.base_prco_cfg.fusion_adaptive_alpha_mode_cfg.fusion_mono_flat_en =
        scene_interpulate(mid_iso, left_iso, get_pipe_params()[index].dynamic_mcf.fusion_mono_flat_en[iso_level - 1],
        right_iso, get_pipe_params()[index].dynamic_mcf.fusion_mono_flat_en[iso_level]);
    mcf_alg_attr->mcf_lf_cfg.base_prco_cfg.fusion_adaptive_alpha_mode_cfg.fusion_mono_flat_en =
        scene_interpulate(mid_iso, left_iso, get_pipe_params()[index].dynamic_mcf.fusion_mono_flat_en[iso_level - 1],
        right_iso, get_pipe_params()[index].dynamic_mcf.fusion_mono_flat_en[iso_level]);

    mcf_alg_attr->mcf_hf_cfg.base_prco_cfg.fusion_adaptive_alpha_mode_cfg.fusion_color_ratio_en =
        scene_interpulate(mid_iso, left_iso, get_pipe_params()[index].dynamic_mcf.fusion_color_ratio_en[iso_level - 1],
        right_iso, get_pipe_params()[index].dynamic_mcf.fusion_color_ratio_en[iso_level]);
    mcf_alg_attr->mcf_mf_cfg.base_prco_cfg.fusion_adaptive_alpha_mode_cfg.fusion_color_ratio_en =
        scene_interpulate(mid_iso, left_iso, get_pipe_params()[index].dynamic_mcf.fusion_color_ratio_en[iso_level - 1],
        right_iso, get_pipe_params()[index].dynamic_mcf.fusion_color_ratio_en[iso_level]);
    mcf_alg_attr->mcf_lf_cfg.base_prco_cfg.fusion_adaptive_alpha_mode_cfg.fusion_color_ratio_en =
        scene_interpulate(mid_iso, left_iso, get_pipe_params()[index].dynamic_mcf.fusion_color_ratio_en[iso_level - 1],
        right_iso, get_pipe_params()[index].dynamic_mcf.fusion_color_ratio_en[iso_level]);

    mcf_alg_attr->mcf_hf_cfg.detail_proc_cfg.fusion_det_gain = scene_interpulate(mid_iso, left_iso,
        get_pipe_params()[index].dynamic_mcf.hf_fusion_det_gain[iso_level - 1], right_iso,
        get_pipe_params()[index].dynamic_mcf.hf_fusion_det_gain[iso_level]);
    mcf_alg_attr->mcf_mf_cfg.detail_proc_cfg.fusion_det_gain = scene_interpulate(mid_iso, left_iso,
        get_pipe_params()[index].dynamic_mcf.mf_fusion_det_gain[iso_level - 1], right_iso,
        get_pipe_params()[index].dynamic_mcf.mf_fusion_det_gain[iso_level]);
    mcf_alg_attr->mcf_lf_cfg.detail_proc_cfg.fusion_det_gain = scene_interpulate(mid_iso, left_iso,
        get_pipe_params()[index].dynamic_mcf.lf_fusion_det_gain[iso_level - 1], right_iso,
        get_pipe_params()[index].dynamic_mcf.lf_fusion_det_gain[iso_level]);
}


hi_void ot_scene_set_dynamic_mcf_interpulate_part2(hi_mcf_alg_param *mcf_alg_attr, hi_u32 mid_iso,
                                                   hi_u32 *iso_level_thresh, hi_u32 iso_level, hi_u8 index)
{
    hi_u32 i;
    hi_u32 left_iso = iso_level_thresh[iso_level - 1];
    hi_u32 right_iso = iso_level_thresh[iso_level];

    for (i = 0; i < HI_MCF_WEIGHT_LUT_NUM; i++) {
        mcf_alg_attr->mcf_hf_cfg.base_prco_cfg.fusion_adaptive_alpha_mode_cfg.fusion_alpha_lut[i]
            = scene_interpulate(mid_iso, left_iso,
            get_pipe_params()[index].dynamic_mcf.hf_fusion_alpha_lut[iso_level - 1][i], right_iso,
            get_pipe_params()[index].dynamic_mcf.hf_fusion_alpha_lut[iso_level][i]);
        mcf_alg_attr->mcf_mf_cfg.base_prco_cfg.fusion_adaptive_alpha_mode_cfg.fusion_alpha_lut[i]
            = scene_interpulate(mid_iso, left_iso,
            get_pipe_params()[index].dynamic_mcf.mf_fusion_alpha_lut[iso_level - 1][i], right_iso,
            get_pipe_params()[index].dynamic_mcf.mf_fusion_alpha_lut[iso_level][i]);
        mcf_alg_attr->mcf_lf_cfg.base_prco_cfg.fusion_adaptive_alpha_mode_cfg.fusion_alpha_lut[i]
            = scene_interpulate(mid_iso, left_iso,
            get_pipe_params()[index].dynamic_mcf.lf_fusion_alpha_lut[iso_level - 1][i], right_iso,
            get_pipe_params()[index].dynamic_mcf.lf_fusion_alpha_lut[iso_level][i]);

        mcf_alg_attr->mcf_hf_cfg.base_prco_cfg.fusion_adaptive_alpha_mode_cfg.fusion_color_ratio_lut[i]
            = scene_interpulate(mid_iso, left_iso,
            get_pipe_params()[index].dynamic_mcf.fusion_color_ratio_lut[iso_level - 1][i], right_iso,
            get_pipe_params()[index].dynamic_mcf.fusion_color_ratio_lut[iso_level][i]);
        mcf_alg_attr->mcf_mf_cfg.base_prco_cfg.fusion_adaptive_alpha_mode_cfg.fusion_color_ratio_lut[i]
            = scene_interpulate(mid_iso, left_iso,
            get_pipe_params()[index].dynamic_mcf.fusion_color_ratio_lut[iso_level - 1][i], right_iso,
            get_pipe_params()[index].dynamic_mcf.fusion_color_ratio_lut[iso_level][i]);
        mcf_alg_attr->mcf_lf_cfg.base_prco_cfg.fusion_adaptive_alpha_mode_cfg.fusion_color_ratio_lut[i]
            = scene_interpulate(mid_iso, left_iso,
            get_pipe_params()[index].dynamic_mcf.fusion_color_ratio_lut[iso_level - 1][i], right_iso,
            get_pipe_params()[index].dynamic_mcf.fusion_color_ratio_lut[iso_level][i]);

        mcf_alg_attr->mcf_hf_cfg.detail_proc_cfg.fusion_mono_det_lut[i]
            = scene_interpulate(mid_iso, left_iso,
            get_pipe_params()[index].dynamic_mcf.hf_fusion_mono_det_lut[iso_level - 1][i], right_iso,
            get_pipe_params()[index].dynamic_mcf.hf_fusion_mono_det_lut[iso_level][i]);
        mcf_alg_attr->mcf_mf_cfg.detail_proc_cfg.fusion_mono_det_lut[i]
            = scene_interpulate(mid_iso, left_iso,
            get_pipe_params()[index].dynamic_mcf.mf_fusion_mono_det_lut[iso_level - 1][i], right_iso,
            get_pipe_params()[index].dynamic_mcf.mf_fusion_mono_det_lut[iso_level][i]);
        mcf_alg_attr->mcf_lf_cfg.detail_proc_cfg.fusion_mono_det_lut[i]
            = scene_interpulate(mid_iso, left_iso,
            get_pipe_params()[index].dynamic_mcf.lf_fusion_mono_det_lut[iso_level - 1][i], right_iso,
            get_pipe_params()[index].dynamic_mcf.lf_fusion_mono_det_lut[iso_level][i]);
    }
}

hi_void ot_scene_set_dynamic_mcf_interpulate_part3(hi_mcf_alg_param *mcf_alg_attr, hi_u32 mid_iso,
                                                   hi_u32 *iso_level_thresh, hi_u32 iso_level, hi_u8 index)
{
    hi_u32 i;
    hi_u32 left_iso = iso_level_thresh[iso_level - 1];
    hi_u32 right_iso = iso_level_thresh[iso_level];

    for (i = 0; i < HI_MCF_CC_UV_GAIN_LUT_NUM; i++) {
        mcf_alg_attr->mcf_color_correct_cfg.cc_uv_gain_lut[i] = scene_interpulate(mid_iso, left_iso,
            get_pipe_params()[index].dynamic_mcf.cc_uv_gain_lut[iso_level - 1][i], right_iso,
            get_pipe_params()[index].dynamic_mcf.cc_uv_gain_lut[iso_level][i]);
    }

    mcf_alg_attr->mcf_hf_cfg.base_prco_cfg.fusion_adaptive_alpha_mode_cfg.fusion_mono_ratio_en =
        scene_interpulate(mid_iso, left_iso, get_pipe_params()[index].dynamic_mcf.fusion_mono_ratio_en[iso_level - 1],
        right_iso, get_pipe_params()[index].dynamic_mcf.fusion_color_ratio_en[iso_level]);
    mcf_alg_attr->mcf_mf_cfg.base_prco_cfg.fusion_adaptive_alpha_mode_cfg.fusion_mono_ratio_en =
        scene_interpulate(mid_iso, left_iso, get_pipe_params()[index].dynamic_mcf.fusion_mono_ratio_en[iso_level - 1],
        right_iso, get_pipe_params()[index].dynamic_mcf.fusion_color_ratio_en[iso_level]);
    mcf_alg_attr->mcf_lf_cfg.base_prco_cfg.fusion_adaptive_alpha_mode_cfg.fusion_mono_ratio_en =
        scene_interpulate(mid_iso, left_iso, get_pipe_params()[index].dynamic_mcf.fusion_mono_ratio_en[iso_level - 1],
        right_iso, get_pipe_params()[index].dynamic_mcf.fusion_color_ratio_en[iso_level]);
}


hi_s32 ot_scene_set_dynamic_mcf(hi_vi_pipe vi_pipe, hi_u64 iso, hi_u64 last_iso, hi_u8 index)
{
    ot_scenecomm_expr_true_return(index >= HI_SCENE_PIPETYPE_NUM, HI_FAILURE);
    if ((get_pipe_params()[index].module_state.dynamic_mcf != HI_TRUE) || (last_iso == iso)) {
        return HI_SUCCESS;
    }

    hi_s32 ret;
    hi_u32 iso_level;
    hi_mcf_alg_param mcf_alg_attr;
    const hi_mcf_grp mcf_grp = 0;
    hi_u32 iso_count = get_pipe_params()[index].dynamic_mcf.iso_count;
    hi_u32 *iso_level_thresh = get_pipe_params()[index].dynamic_mcf.iso_thresh;

    iso_level = (hi_u32)scene_get_level_ltoh_u32((hi_u32)iso, iso_count, iso_level_thresh);
    ot_scenecomm_expr_true_return(iso_level >= HI_SCENE_ISO_STRENGTH_NUM, HI_FAILURE);

    check_scene_return_if_pause();
    ret = hi_mpi_mcf_get_alg_param(mcf_grp, &mcf_alg_attr);
    check_scene_ret(ret);

    if ((iso_level == (iso_count - 1)) || (iso_level == 0)) {
        ot_scene_set_dynamic_mcf_border_part1(&mcf_alg_attr, iso_level, index);
        ot_scene_set_dynamic_mcf_border_part2(&mcf_alg_attr, iso_level, index);
    } else {
        hi_u32 mid_iso = (hi_u32)iso;

        ot_scene_set_dynamic_mcf_interpulate_part1(&mcf_alg_attr, mid_iso, iso_level_thresh, iso_level, index);
        ot_scene_set_dynamic_mcf_interpulate_part2(&mcf_alg_attr, mid_iso, iso_level_thresh, iso_level, index);
        ot_scene_set_dynamic_mcf_interpulate_part3(&mcf_alg_attr, mid_iso, iso_level_thresh, iso_level, index);
    }

    check_scene_return_if_pause();
    ret = hi_mpi_mcf_set_alg_param(mcf_grp, &mcf_alg_attr);
    check_scene_ret(ret);
    return HI_SUCCESS;
}
#endif

#ifdef HNR_SCENE_AUTO_USED
hi_s32 ot_scene_set_dynamic_hnr(hi_vi_pipe vi_pipe, hi_u64 iso, hi_u64 last_iso, hi_u8 index)
{
    ot_scenecomm_expr_true_return(index >= HI_SCENE_PIPETYPE_NUM, HI_FAILURE);
    if ((get_pipe_params()[index].module_state.dynamic_hnr != HI_TRUE) || (last_iso == iso)) {
        return HI_SUCCESS;
    }


    hi_s32 ret;
    hi_isp_dp_dynamic_attr dp_dynamic_attr;
    hi_hnr_attr isp_hnr_attr;

    hi_u32 *dpc_iso_thresh = get_pipe_params()[index].dynamic_hnr.dpc_iso_thresh;
    hi_u32 *hnr_iso_thresh = get_pipe_params()[index].dynamic_hnr.hnr_iso_thresh;
    hi_bool dpc_chg_en = get_pipe_params()[index].dynamic_hnr.dpc_chg_en;
    hi_bool hnr_chg_en = get_pipe_params()[index].dynamic_hnr.hnr_chg_en;
    check_scene_return_if_pause();
    ret = hi_mpi_hnr_get_attr(vi_pipe, &isp_hnr_attr);
    check_scene_ret(ret);
    ret = hi_mpi_isp_get_dp_dynamic_attr(vi_pipe, &dp_dynamic_attr);
    check_scene_ret(ret);

    if ((iso >= hnr_iso_thresh[1]) && (hnr_chg_en == HI_TRUE)) {
        isp_hnr_attr.enable = HI_TRUE;
    }

    if ((iso <= hnr_iso_thresh[0]) && (hnr_chg_en == HI_TRUE)) {
        isp_hnr_attr.enable = HI_FALSE;
    }

    if ((iso >= dpc_iso_thresh[1]) && (dpc_chg_en == HI_TRUE)) {
        dp_dynamic_attr.enable = HI_FALSE;
    }

    if ((iso <= dpc_iso_thresh[0]) && (dpc_chg_en == HI_TRUE)) {
        dp_dynamic_attr.enable = HI_TRUE;
    }
    printf("iso:%d, hnr_iso_thresh[%d, %d], isp_hnr_attr.enable: %d, dp_dynamic_attr.enable: %d\n", iso, hnr_iso_thresh[0], hnr_iso_thresh[1], isp_hnr_attr.enable, dp_dynamic_attr.enable);
    ret = hi_mpi_hnr_set_attr(vi_pipe, &isp_hnr_attr);
    check_scene_ret(ret);
    ret = hi_mpi_isp_set_dp_dynamic_attr(vi_pipe, &dp_dynamic_attr);
    check_scene_ret(ret);
    check_scene_return_if_pause();
    return HI_SUCCESS;
}
#endif

static hi_bool g_fpn_enable[HI_SCENE_PIPETYPE_NUM] = {HI_FALSE};

hi_s32 ot_scene_release_fpn(hi_vi_pipe vi_pipe, hi_u8 index)
{
    hi_s32 ret;
    if (get_pipe_params()[index].module_state.dynamic_fpn != HI_TRUE) {
        return HI_SUCCESS;
    }
    if (g_fpn_enable[vi_pipe] == HI_TRUE) {
        g_fpn_enable[vi_pipe] = HI_FALSE;
        ret = sample_comm_vi_disable_fpn_correction(vi_pipe, &g_correction_cfg);
        check_scene_ret(ret);
        printf("release fpn buffer ok!\n");
    }
    return HI_SUCCESS;
}

hi_s32 ot_scene_dynamic_fpn_decide(hi_vi_pipe vi_pipe, hi_u32 *iso_tmp, hi_u32 *last_iso_tmp,
                                   hi_bool *fpn_enable, hi_u8 index)
{
    hi_s32 ret;
    hi_u32 i, delta, sns_iso;
    hi_isp_exp_info exp_info;
    hi_u32 iso_count = get_pipe_params()[index].dynamic_fpn.iso_count;
    hi_u32 *iso_level_thresh = get_pipe_params()[index].dynamic_fpn.iso_thresh;
    hi_u32 fpn_iso_thresh = get_pipe_params()[index].dynamic_fpn.fpn_iso_thresh;

    check_scene_return_if_pause();
    ret = hi_mpi_isp_query_exposure_info(vi_pipe, &exp_info);
    check_scene_ret(ret);

    sns_iso = ((hi_u64)exp_info.a_gain * exp_info.d_gain * FPN_GAIN_ISO) >> FPN_GAIN_SHIFT;

    if (sns_iso >= iso_level_thresh[iso_count - 1]) {
        *iso_tmp = iso_level_thresh[iso_count - 1];
        *fpn_enable = HI_TRUE;
    } else if (sns_iso < fpn_iso_thresh) {
        *iso_tmp = sns_iso;
        if (*fpn_enable == HI_TRUE) {
            *fpn_enable = HI_FALSE;
            ret = sample_comm_vi_disable_fpn_correction(vi_pipe, &g_correction_cfg);
            check_scene_ret(ret);
            *last_iso_tmp = *iso_tmp;
            printf("disable fpn correction\n");
        }
    } else {
        if ((iso_count - 1) == 0) {
            *iso_tmp = iso_level_thresh[0];
        }
        for (i = 0; i < iso_count - 1; i++) {
            delta = (iso_level_thresh[i + 1] - iso_level_thresh[i]) >> 1;
            if (sns_iso <= (iso_level_thresh[i] + delta)) {
                *iso_tmp = iso_level_thresh[i];
                break;
            } else if ((sns_iso > (iso_level_thresh[i] + delta)) && (sns_iso < iso_level_thresh[i + 1])) {
                *iso_tmp = iso_level_thresh[i + 1];
                break;
            }
        }
        *fpn_enable = HI_TRUE;
    }
    return HI_SUCCESS;
}

hi_s32 ot_scene_set_dynamic_fpn(hi_vi_pipe vi_pipe, hi_u64 iso, hi_u64 last_iso, hi_u8 index)
{
    ot_scenecomm_expr_true_return(index >= HI_SCENE_PIPETYPE_NUM, HI_FAILURE);
    if ((get_pipe_params()[index].module_state.dynamic_fpn != HI_TRUE) || (last_iso == iso)) {
        return HI_SUCCESS;
    }

    hi_s32 ret;
    hi_u32 iso_level, sns_iso;
    hi_u32 iso_tmp = 0;

    static hi_bool last_iso_tmp[HI_SCENE_PIPETYPE_NUM] = {0};
    hi_isp_exp_info exp_info;
    hi_u32 iso_count = get_pipe_params()[index].dynamic_fpn.iso_count;
    hi_u32 *iso_level_thresh = get_pipe_params()[index].dynamic_fpn.iso_thresh;
    sample_scene_fpn_offset_cfg  scene_fpn_offset_cfg;

    check_scene_return_if_pause();
    ret = hi_mpi_isp_query_exposure_info(vi_pipe, &exp_info);
    check_scene_ret(ret);

    sns_iso = ((hi_u64)exp_info.a_gain * exp_info.d_gain * FPN_GAIN_ISO) >> FPN_GAIN_SHIFT;
    iso_level = (hi_u32)scene_get_level_ltoh_u32((hi_u32)sns_iso, iso_count, iso_level_thresh);
    ot_scenecomm_expr_true_return(iso_level >= HI_SCENE_ISO_STRENGTH_NUM, HI_FAILURE);

    scene_fpn_offset_cfg.offset = get_pipe_params()[index].dynamic_fpn.fpn_offset[iso_level];

    ret = ot_scene_dynamic_fpn_decide(vi_pipe, &iso_tmp, &last_iso_tmp[vi_pipe], &g_fpn_enable[vi_pipe], index);
    check_scene_ret(ret);

    if (last_iso_tmp[vi_pipe] != iso_tmp && (g_fpn_enable[vi_pipe] == HI_TRUE)) {
        ret = sample_comm_vi_enable_fpn_correction_for_scene(vi_pipe, &g_correction_cfg, iso_tmp,
                                                             &scene_fpn_offset_cfg, g_dir_name);
        check_scene_ret(ret);
        last_iso_tmp[vi_pipe] = iso_tmp;
    }
    return HI_SUCCESS;
}

hi_s32 ot_scene_set_dynamic_3dnr(hi_vi_pipe vi_pipe, hi_u32 iso, hi_u8 index)
{
    if (get_pipe_params()[index].module_state.dynamic_3dnr != HI_TRUE) {
        return HI_SUCCESS;
    }

    hi_u32 iso_level = 0;
    ot_scene_3dnr nrx_attr;
    const ot_scene_static_3dnr *nrx_param = &(get_pipe_params()[index].static_threednr);
    hi_u32 count = nrx_param->threed_nr_count;
    hi_u32 *p_thresh = (hi_u32 *)nrx_param->threed_nr_iso;
    iso_level = get_level_ltoh(iso, iso_level, count, p_thresh);
    if (iso_level == 0) {
        nrx_attr = nrx_param->threednr_value[0];
    } else {
        ot_scenecomm_expr_true_return(iso_level >= HI_SCENE_3DNR_MAX_COUNT, HI_FAILURE);
        hi_u32 mid = iso;
        hi_u32 left = p_thresh[iso_level - 1];
        hi_u32 right = p_thresh[iso_level];

        nrx_attr = nrx_param->threednr_value[iso_level];

        _3dnr_nrx_param param = { &nrx_attr, nrx_param, iso_level, mid, left, right };

        scene_set_3dnr_nrx_nry(param);

        scene_set_3dnr_nrx_iey(param);

        scene_set_3dnr_nrx_sfy(param);

        scene_set_3dnr_nrx_tfy(param);

        scene_set_3dnr_nrx_mdy(param);

        scene_set_3dnr_nrx_nrc0(param);

        scene_set_3dnr_nrx_nrc1(param);
    }

    hi_s32 ret = scene_set_3dnr(vi_pipe, &nrx_attr, index);
    check_scene_ret(ret);
    return HI_SUCCESS;
}

hi_s32 ot_scene_set_pipe_param(const ot_scene_pipe_param *pscene_pipe_param, hi_u32 num)
{
    if (pscene_pipe_param == HI_NULL) {
        printf("null pointer");
        return HI_FAILURE;
    }

    errno_t ret = memcpy_s(get_pipe_params(), sizeof(ot_scene_pipe_param) * HI_SCENE_PIPETYPE_NUM, pscene_pipe_param,
        sizeof(ot_scene_pipe_param) * num);
    if (ret != EOK) {
        printf("copy scene pipe params fail. num = %u\n", num);
        return HI_FAILURE;
    }
    return HI_SUCCESS;
}

hi_s32 ot_scene_set_pause(hi_bool pause)
{
    *(get_isp_pause()) = pause;
    return HI_SUCCESS;
}

static hi_u32 g_iso_info[8] = {100, 100, 100, 100, 100, 100, 100, 100}; /* 8 len, 100 init value */
static hi_u32 g_meanqp_info[8] = {30, 30, 30, 30, 30, 30, 30, 30};      /* 8 len, 30 init value */

static hi_void shift_array_push_tail(hi_u32 iso, hi_u32 mean_qp)
{
    hi_u32 i;
    for (i = 0; i < ARRAY_SIZE(g_iso_info) - 1; i++) {
        g_iso_info[i] = g_iso_info[i + 1];
        g_meanqp_info[i] = g_meanqp_info[i + 1];
    }
    g_iso_info[i] = iso;
    g_meanqp_info[i] = mean_qp;
}

static hi_void calculate_sum_iso_mean_qp(hi_u32 min_qp, hi_u32 *sum_mean_qp, hi_u32 *sum_iso)
{
    hi_u32 i;
    for (i = 0; i < ARRAY_SIZE(g_meanqp_info); i++) {
        if (min_qp < g_meanqp_info[i]) {
            min_qp = g_meanqp_info[i];
        }
        *sum_mean_qp += g_meanqp_info[i];
        *sum_iso += g_iso_info[i];
    }
    *sum_mean_qp = *sum_mean_qp - min_qp;
    *sum_mean_qp = *sum_mean_qp / (ARRAY_SIZE(g_meanqp_info) - 1);
    *sum_iso = *sum_iso / ARRAY_SIZE(g_iso_info);
}

hi_s32 ot_scene_set_rc_param(hi_vi_pipe vi_pipe, hi_u8 idx)
{
    hi_s32 ret;
    hi_venc_chn_attr venc_chn_attr;
    hi_venc_rc_param rc_param;

    hi_u32 min_qp, iso, mean_qp, max_bitrate, pic_width, pic_height;
    hi_s32 percent = 90; /* percent initvalue is 90 */
    hi_u32 sum_mean_qp = 0;
    hi_u32 sum_iso = 0;
    hi_payload_type type;
    hi_u32 index = 0;

    ot_scenecomm_expr_true_return(idx >= HI_SCENE_PIPETYPE_NUM, HI_FAILURE);
    if (get_pipe_params()[idx].module_state.dynamic_iso_venc != HI_TRUE) {
        return HI_SUCCESS;
    }

    ret = get_iso_mean_qp_chn_attr(vi_pipe, &iso, &mean_qp, &venc_chn_attr);
    check_scene_ret(ret);
    if ((venc_chn_attr.rc_attr.rc_mode != HI_VENC_RC_MODE_H264_AVBR) &&
        (venc_chn_attr.rc_attr.rc_mode != HI_VENC_RC_MODE_H265_AVBR)) {
        return HI_SUCCESS;
    }

    type = venc_chn_attr.venc_attr.type;
    min_qp = g_meanqp_info[0];
    max_bitrate = (type == HI_PT_H264) ? venc_chn_attr.rc_attr.h264_avbr.max_bit_rate :
                                         venc_chn_attr.rc_attr.h265_avbr.max_bit_rate;
    pic_width = (type == HI_PT_H264) ? venc_chn_attr.venc_attr.pic_width : venc_chn_attr.venc_attr.pic_width;
    pic_height = (type == HI_PT_H264) ? venc_chn_attr.venc_attr.pic_height : venc_chn_attr.venc_attr.pic_height;

    shift_array_push_tail(iso, mean_qp);

    calculate_sum_iso_mean_qp(min_qp, &sum_mean_qp, &sum_iso);

    calculate_manual_percent_index(idx, sum_iso, &index);

    ret = hi_mpi_venc_get_rc_param(vi_pipe, &rc_param);
    check_scene_ret(ret);
    ot_scenecomm_expr_true_return(index >= HI_SCENE_SHADING_EXPOSURE_MAX_COUNT, HI_FAILURE);

    set_initial_percent(type, index, &rc_param, &get_pipe_params()[idx], &percent);

    if (sum_iso > get_pipe_params()[idx].dynamic_venc_bitrate.iso_thresh_ltoh[2]) { /* 2: iso_thresh_ltoh idx */
        set_min_qp_delta_when_iso_larger(type, sum_mean_qp, percent, &rc_param);
    } else {
        set_min_qp_delta_when_iso_less(type, sum_mean_qp, &rc_param, &venc_chn_attr);
    }

    ret = scene_set_qp(pic_width, pic_height, max_bitrate, type, &rc_param);
    check_scene_ret(ret);

    ret = hi_mpi_venc_set_rc_param(vi_pipe, &rc_param);
    check_scene_ret(ret);

    return HI_SUCCESS;
}

#ifdef __cplusplus
}
#endif
