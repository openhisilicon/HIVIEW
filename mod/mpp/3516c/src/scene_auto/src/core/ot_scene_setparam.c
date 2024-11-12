/*
  Copyright (c), 2001-2024, Shenshu Tech. Co., Ltd.
 */

#include <unistd.h>
#include <string.h>
#include "securec.h"
#include "hi_mpi_awb.h"
#include "hi_mpi_ae.h"
#include "hi_mpi_isp.h"
#include "hi_mpi_venc.h"
#include "hi_common_rc.h"
#include "sample_comm.h"
#include "ot_scenecomm.h"
#include "scene_setparam_inner.h"
#include "ot_scene_setparam.h"

#ifdef __cplusplus
extern "C" {
#endif

hi_char g_dir_name[DIR_NAME_BUFF];

ot_scene_fps g_sceneauto_fps = {0};

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
    for (hi_u32 i = 0; i < ae_route_ex.total_num && i < HI_ISP_AE_ROUTE_EX_MAX_NODES; i++) {
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
    wdr_exposure_attr.tolerance = get_pipe_params()[index].static_wdr_exposure.tolerance;
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
    wb_attr.auto_attr.shift_limit = get_pipe_params()[index].static_awb.shift_limit;

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
    awb_attr_ex.in_or_out.out_shift_limit = get_pipe_params()[index].static_awb_ex.out_shift_limit;
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

hi_s32 ot_scene_set_static_pipe_diff(hi_vi_pipe vi_pipe, hi_u8 index)
{
    ot_scenecomm_expr_true_return(index >= HI_SCENE_PIPETYPE_NUM, HI_FAILURE);
    if (get_pipe_params()[index].module_state.static_pipe_diff != HI_TRUE) {
        return HI_SUCCESS;
    }

    hi_s32 i, ret;
    hi_isp_pipe_diff_attr pipe_diff_attr;

    ret = hi_mpi_isp_get_pipe_differ_attr(vi_pipe, &pipe_diff_attr);
    check_scene_ret(ret);

    for (i = 0; i < HI_ISP_BAYER_CHN_NUM; i++) {
        pipe_diff_attr.gain[i] = get_pipe_params()[index].static_pipe_diff.gain[i];
    }
    ret = hi_mpi_isp_set_pipe_differ_attr(vi_pipe, &pipe_diff_attr);
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

    saturation_attr.op_type = get_pipe_params()[index].static_saturation.op_type;
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

    hi_u32 i, j;
    hi_s32 ret;
    hi_isp_color_matrix_attr color_matrix_attr;
    hi_isp_color_tone_attr  color_tone_attr;

    ret = hi_mpi_isp_get_ccm_attr(vi_pipe, &color_matrix_attr);
    check_scene_ret(ret);
    color_matrix_attr.op_type = (hi_op_mode)get_pipe_params()[index].static_ccm.ccm_op_type;

    for (i = 0; i < HI_ISP_CCM_MATRIX_SIZE; i++) {
        color_matrix_attr.manual_attr.ccm[i] = get_pipe_params()[index].static_ccm.manual_ccm[i];
    }
    color_matrix_attr.auto_attr.iso_act_en = get_pipe_params()[index].static_ccm.auto_iso_act_en;
    color_matrix_attr.auto_attr.temp_act_en = get_pipe_params()[index].static_ccm.auto_temp_act_en;
    color_matrix_attr.auto_attr.ccm_tab_num = (hi_u16)get_pipe_params()[index].static_ccm.total_num;

    for (i = 0; i < get_pipe_params()[index].static_ccm.total_num && i < HI_ISP_CCM_MATRIX_NUM; i++) {
        color_matrix_attr.auto_attr.ccm_tab[i].color_temp = get_pipe_params()[index].static_ccm.auto_color_temp[i];
        for (j = 0; j < HI_ISP_CCM_MATRIX_SIZE; j++) {
            color_matrix_attr.auto_attr.ccm_tab[i].ccm[j] = get_pipe_params()[index].static_ccm.auto_ccm[i][j];
        }
    }
    ret = hi_mpi_isp_set_ccm_attr(vi_pipe, &color_matrix_attr);
    check_scene_ret(ret);

    ret = hi_mpi_isp_get_color_tone_attr(vi_pipe, &color_tone_attr);
    check_scene_ret(ret);

    color_tone_attr.red_cast_gain = get_pipe_params()[index].static_ccm.red_cast_gain;
    color_tone_attr.green_cast_gain = get_pipe_params()[index].static_ccm.green_cast_gain;
    color_tone_attr.blue_cast_gain = get_pipe_params()[index].static_ccm.blue_cast_gain;

    ret = hi_mpi_isp_set_color_tone_attr(vi_pipe, &color_tone_attr);
    check_scene_ret(ret);
    return HI_SUCCESS;
}

hi_s32 ot_scene_set_static_color_sector(hi_vi_pipe vi_pipe, hi_u8 index)
{
    ot_scenecomm_expr_true_return(index >= HI_SCENE_PIPETYPE_NUM, HI_FAILURE);
    if (get_pipe_params()[index].module_state.static_color_sector != HI_TRUE) {
        return HI_SUCCESS;
    }

    hi_u32 i, j;
    hi_s32 ret;
    hi_isp_color_sector_attr color_sector_attr;

    ret = hi_mpi_isp_get_color_sector_attr(vi_pipe, &color_sector_attr);
    check_scene_ret(ret);
    color_sector_attr.enable = get_pipe_params()[index].static_color_sector.enable;

    for (i = 0; i < HI_ISP_CCM_MATRIX_NUM; i++) {
        for (j = 0; j < HI_ISP_COLOR_SECTOR_NUM; j++) {
            color_sector_attr.auto_attr.color_tab[i].hue_shift[j] =
                get_pipe_params()[index].static_color_sector.auto_attr.color_tab[i].hue_shift[j];
            color_sector_attr.auto_attr.color_tab[i].sat_shift[j] =
                get_pipe_params()[index].static_color_sector.auto_attr.color_tab[i].sat_shift[j];
        }
    }

    ret = hi_mpi_isp_set_color_sector_attr(vi_pipe, &color_sector_attr);
    check_scene_ret(ret);
    return HI_SUCCESS;
}

hi_s32 ot_scene_set_static_cac(hi_vi_pipe vi_pipe, hi_u8 index)
{
    ot_scenecomm_expr_true_return(index >= HI_SCENE_PIPETYPE_NUM, HI_FAILURE);
    if (get_pipe_params()[index].module_state.static_cac != HI_TRUE) {
        return HI_SUCCESS;
    }
    hi_u32 i, j;
    hi_s32 ret;
    hi_isp_cac_attr cac_attr;

    ret = hi_mpi_isp_get_cac_attr(vi_pipe, &cac_attr);
    check_scene_ret(ret);
    cac_attr.enable = get_pipe_params()[index].static_cac.enable;
    cac_attr.op_type = get_pipe_params()[index].static_cac.op_type;
    cac_attr.purple_lower_limit = get_pipe_params()[index].static_cac.purple_lower_limit;
    cac_attr.purple_upper_limit = get_pipe_params()[index].static_cac.purple_upper_limit;
    cac_attr.lcac_cfg.purple_detect_range = get_pipe_params()[index].static_cac.purple_detect_range;
    cac_attr.lcac_cfg.var_threshold = get_pipe_params()[index].static_cac.var_threshold;

    for (i = 0; i < HI_SCENE_ACAC_THR_NUM; i++) {
        for (j = 0; j < HI_ISP_AUTO_ISO_NUM; j++) {
            cac_attr.acac_cfg.acac_auto.edge_threshold[i][j] =
                get_pipe_params()[index].static_cac.edge_threshold[i][j];
        }
    }

    for (i = 0; i < HI_ISP_AUTO_ISO_NUM; i++) {
        cac_attr.acac_cfg.acac_auto.edge_gain[i] = get_pipe_params()[index].static_cac.edge_gain[i];
        cac_attr.acac_cfg.acac_auto.purple_alpha[i] = get_pipe_params()[index].static_cac.purple_alpha[i];
        cac_attr.acac_cfg.acac_auto.edge_alpha[i] = get_pipe_params()[index].static_cac.edge_alpha[i];
        cac_attr.acac_cfg.acac_auto.cac_rb_strength[i] = get_pipe_params()[index].static_cac.cac_rb_strength[i];
        cac_attr.acac_cfg.acac_auto.satu_low_threshold[i] =
            get_pipe_params()[index].static_cac.satu_low_threshold[i];
    }

    for (i = 0; i < HI_ISP_LCAC_DET_NUM; i++) {
        cac_attr.lcac_cfg.r_detect_threshold[i] = get_pipe_params()[index].static_cac.r_detect_threshold[i];
        cac_attr.lcac_cfg.g_detect_threshold[i] = get_pipe_params()[index].static_cac.g_detect_threshold[i];
        cac_attr.lcac_cfg.b_detect_threshold[i] = get_pipe_params()[index].static_cac.b_detect_threshold[i];
    }

    for (i = 0; i < OT_ISP_LCAC_EXP_RATIO_NUM; i++) {
        cac_attr.lcac_cfg.lcac_auto.de_purple_cb_strength[i] =
           get_pipe_params()[index].static_cac.de_purple_cb_strength[i];
        cac_attr.lcac_cfg.lcac_auto.de_purple_cr_strength[i] =
           get_pipe_params()[index].static_cac.de_purple_cr_strength[i];
    }

    ret = hi_mpi_isp_set_cac_attr(vi_pipe, &cac_attr);
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
    dp_dynamic_attr.frame_dynamic[0].op_type = get_pipe_params()[index].static_dpc.op_type;

    for (i = 0; i < HI_SCENE_ISO_STRENGTH_NUM; i++) {
        dp_dynamic_attr.frame_dynamic[0].auto_attr.strength[i] = get_pipe_params()[index].static_dpc.strength[i];
        dp_dynamic_attr.frame_dynamic[0].auto_attr.blend_ratio[i] =
            get_pipe_params()[index].static_dpc.blend_ratio[i];
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

    drc_attr.contrast_ctrl = get_pipe_params()[index].static_drc.contrast_ctrl;
    drc_attr.global_color_ctrl = get_pipe_params()[index].static_drc.global_color_ctrl;
    drc_attr.bcnr_attr.enable = get_pipe_params()[index].static_drc.drc_bcnr_en;

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

    ldci_attr.enable = get_pipe_params()[index].static_ldci.enable;
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
    dehaze_attr.enable = get_pipe_params()[index].static_dehaze.enable;
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

    csc_attr.enable = get_pipe_params()[index].staic_csc.enable;
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
        nr_attr->snr_cfg.snr_attr.snr_auto.sfm0_detail_prot[i] =
            get_pipe_params()[index].static_nr.snr_cfg.sfm0_detail_prot[i];
        nr_attr->snr_cfg.snr_attr.snr_auto.fine_strength[i] =
            get_pipe_params()[index].static_nr.snr_cfg.fine_strength[i];
        nr_attr->snr_cfg.snr_attr.snr_auto.tss[i] = get_pipe_params()[index].static_nr.snr_cfg.tss[i];
        for (j = 0; j < HI_ISP_BAYER_CHN_NUM; j++) {
            nr_attr->snr_cfg.snr_attr.snr_auto.sfm0_coarse_strength[j][i] =
                get_pipe_params()[index].static_nr.snr_cfg.sfm0_coarse_strength[j][i];
        }
    }

    for (i = 0; i < HI_ISP_BAYERNR_LUT_LENGTH1; i++) {
        for (j = 0; j < HI_ISP_AUTO_ISO_NUM; j++) {
            nr_attr->snr_cfg.snr_attr.snr_auto.noisesd_lut[i][j] =
                get_pipe_params()[index].static_nr.snr_cfg.noisesd_lut[i][j];
        }
    }

    return HI_SUCCESS;
}

static hi_s32 ot_scene_set_static_nr_md(hi_vi_pipe vi_pipe, hi_u8 index, hi_isp_nr_attr *nr_attr)
{
    hi_s32 i;

    ot_scenecomm_expr_true_return(index >= HI_SCENE_PIPETYPE_NUM, HI_FAILURE);
    ot_scenecomm_check_pointer_return(nr_attr, HI_FAILURE);
    for (i = 0; i < HI_ISP_AUTO_ISO_NUM; i++) {
        nr_attr->md_cfg.md_auto.md_mode[i]  = get_pipe_params()[index].static_nr.md_cfg.md_mode[i];
        nr_attr->md_cfg.md_auto.md_anti_flicker_strength[i] =
            get_pipe_params()[index].static_nr.md_cfg.md_anti_flicker_strength[i];
        nr_attr->md_cfg.md_auto.md_static_ratio[i] = get_pipe_params()[index].static_nr.md_cfg.md_static_ratio[i];
        nr_attr->md_cfg.md_auto.md_motion_ratio[i] = get_pipe_params()[index].static_nr.md_cfg.md_motion_ratio[i];
        nr_attr->md_cfg.md_auto.md_static_fine_strength[i] =
            get_pipe_params()[index].static_nr.md_cfg.md_static_fine_strength[i];
        nr_attr->md_cfg.md_auto.user_define_md[i] = get_pipe_params()[index].static_nr.md_cfg.user_define_md[i];
        nr_attr->md_cfg.md_auto.user_define_slope[i] = get_pipe_params()[index].static_nr.md_cfg.user_define_slope[i];
        nr_attr->md_cfg.md_auto.user_define_dark_thresh[i] =
            get_pipe_params()[index].static_nr.md_cfg.user_define_dark_thresh[i];

        nr_attr->md_cfg.md_auto.sfr_r[i] = get_pipe_params()[index].static_nr.md_cfg.sfr_r[i];
        nr_attr->md_cfg.md_auto.sfr_g[i] = get_pipe_params()[index].static_nr.md_cfg.sfr_g[i];
        nr_attr->md_cfg.md_auto.sfr_b[i] = get_pipe_params()[index].static_nr.md_cfg.sfr_b[i];
    }
    return HI_SUCCESS;
}

hi_s32 ot_scene_set_static_nr(hi_vi_pipe vi_pipe, hi_u8 index)
{
    ot_scenecomm_expr_true_return(index >= HI_SCENE_PIPETYPE_NUM, HI_FAILURE);
    if (get_pipe_params()[index].module_state.static_nr != HI_TRUE) {
        return HI_SUCCESS;
    }

    hi_s32 ret;
    hi_isp_nr_attr nr_attr;

    ret = hi_mpi_isp_get_nr_attr(vi_pipe, &nr_attr);
    check_scene_ret(ret);

    nr_attr.enable = get_pipe_params()[index].static_nr.enable;
    nr_attr.op_type = (hi_op_mode)get_pipe_params()[index].static_nr.op_type;
    nr_attr.md_en = get_pipe_params()[index].static_nr.md_enable;
    nr_attr.lsc_nr_en = get_pipe_params()[index].static_nr.lsc_nr_enable;
    nr_attr.lsc_ratio1 = get_pipe_params()[index].static_nr.lsc_ratio1;

    ret = ot_scene_set_static_nr_snr(vi_pipe, index, &nr_attr);
    check_scene_ret(ret);

    ret = ot_scene_set_static_nr_md(vi_pipe, index, &nr_attr);
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

    shading_attr.enable = get_pipe_params()[index].static_shading.enable;
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

        sharpen_attr->auto_attr.shoot_threshold_attr.shoot_inner_threshold[i] =
            get_pipe_params()[index].static_sharpen.sharpen_auto_cfg.shoot_inner_threshold[i];
        sharpen_attr->auto_attr.shoot_threshold_attr.shoot_outer_threshold[i] =
            get_pipe_params()[index].static_sharpen.sharpen_auto_cfg.shoot_outer_threshold[i];
        sharpen_attr->auto_attr.shoot_threshold_attr.shoot_protect_threshold[i] =
            get_pipe_params()[index].static_sharpen.sharpen_auto_cfg.shoot_protect_threshold[i];

        sharpen_attr->auto_attr.edge_rly_attr.edge_rly_fine_threshold[i] =
            get_pipe_params()[index].static_sharpen.sharpen_auto_cfg.edge_rly_fine_threshold[i];
        sharpen_attr->auto_attr.edge_rly_attr.edge_rly_coarse_threshold[i] =
            get_pipe_params()[index].static_sharpen.sharpen_auto_cfg.edge_rly_coarse_threshold[i];
        sharpen_attr->auto_attr.edge_rly_attr.edge_overshoot[i] =
            get_pipe_params()[index].static_sharpen.sharpen_auto_cfg.edge_overshoot[i];
        sharpen_attr->auto_attr.edge_rly_attr.edge_undershoot[i] =
            get_pipe_params()[index].static_sharpen.sharpen_auto_cfg.edge_undershoot[i];
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
    hi_isp_sharpen_auto_attr *auto_attr = &sharpen_attr.auto_attr;
    ot_scene_static_sharpen *static_sharpen = &get_pipe_params()[index].static_sharpen;

    ret = hi_mpi_isp_get_sharpen_attr(vi_pipe, &sharpen_attr);
    check_scene_ret(ret);

    sharpen_attr.enable = get_pipe_params()[index].static_sharpen.enable;
    sharpen_attr.op_type = get_pipe_params()[index].static_sharpen.op_type;
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
        }
    }

    (hi_void)memcpy_s(auto_attr->edge_rly_attr.edge_gain_by_rly, HI_ISP_SHARPEN_RLYWGT_NUM * HI_ISP_AUTO_ISO_NUM,
        static_sharpen->sharpen_auto_cfg.edge_gain_by_rly,  HI_ISP_SHARPEN_RLYWGT_NUM * HI_ISP_AUTO_ISO_NUM);

    (hi_void)memcpy_s(auto_attr->edge_rly_attr.edge_rly_by_mot, HI_ISP_SHARPEN_STDGAIN_NUM * HI_ISP_AUTO_ISO_NUM,
        static_sharpen->sharpen_auto_cfg.edge_rly_by_mot,  HI_ISP_SHARPEN_STDGAIN_NUM * HI_ISP_AUTO_ISO_NUM);

    (hi_void)memcpy_s(auto_attr->edge_rly_attr.edge_rly_by_luma, HI_ISP_SHARPEN_STDGAIN_NUM * HI_ISP_AUTO_ISO_NUM,
        static_sharpen->sharpen_auto_cfg.edge_rly_by_luma,  HI_ISP_SHARPEN_STDGAIN_NUM * HI_ISP_AUTO_ISO_NUM);

    (hi_void)memcpy_s(auto_attr->gain_by_mot_attr.mf_gain_by_mot, HI_ISP_SHARPEN_MOT_NUM * HI_ISP_AUTO_ISO_NUM,
        static_sharpen->sharpen_auto_cfg.mf_gain_by_mot,  HI_ISP_SHARPEN_MOT_NUM * HI_ISP_AUTO_ISO_NUM);

    (hi_void)memcpy_s(auto_attr->gain_by_mot_attr.hf_gain_by_mot, HI_ISP_SHARPEN_MOT_NUM * HI_ISP_AUTO_ISO_NUM,
        static_sharpen->sharpen_auto_cfg.hf_gain_by_mot,  HI_ISP_SHARPEN_MOT_NUM * HI_ISP_AUTO_ISO_NUM);

    (hi_void)memcpy_s(auto_attr->gain_by_mot_attr.lmf_gain_by_mot, HI_ISP_SHARPEN_MOT_NUM * HI_ISP_AUTO_ISO_NUM,
        static_sharpen->sharpen_auto_cfg.lmf_gain_by_mot,  HI_ISP_SHARPEN_MOT_NUM * HI_ISP_AUTO_ISO_NUM);

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

    dm_attr.enable = get_pipe_params()[index].static_dm.enable;
    dm_attr.op_type = get_pipe_params()[index].static_dm.op_type;

    for (i = 0; i < HI_ISP_AUTO_ISO_NUM; i++) {
        dm_attr.auto_attr.nddm_strength[i] = get_pipe_params()[index].static_dm.dm_auto_cfg.nddm_strength[i];
        dm_attr.auto_attr.nddm_mf_detail_strength[i] =
            get_pipe_params()[index].static_dm.dm_auto_cfg.nddm_mf_detail_strength[i];
        dm_attr.auto_attr.hf_detail_strength[i] =
            get_pipe_params()[index].static_dm.dm_auto_cfg.hf_detail_strength[i];
        dm_attr.auto_attr.detail_smooth_range[i] =
            get_pipe_params()[index].static_dm.dm_auto_cfg.detail_smooth_range[i];
    }

    ret = hi_mpi_isp_set_demosaic_attr(vi_pipe, &dm_attr);
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

    ca_attr.enable = get_pipe_params()[index].static_ca.enable;
    for (i = 0; i < ISP_AUTO_ISO_CA_NUM; i++) {
        ca_attr.ca.iso_ratio[i] = get_pipe_params()[index].static_ca.iso_ratio[i];
    }

    for (i = 0; i < HI_ISP_CA_YRATIO_LUT_LENGTH; i++) {
        ca_attr.ca.y_ratio_lut[i] = get_pipe_params()[index].static_ca.y_ratio_lut[i];
    }

    for (i = 0; i < HI_ISP_CA_YRATIO_LUT_LENGTH; i++) {
        ca_attr.ca.y_sat_lut[i] = get_pipe_params()[index].static_ca.y_sat_lut[i];
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

    deblur_param.deblur_en = get_pipe_params()[index].static_venc_attr.deblur_en;
    deblur_param.deblur_adaptive_en = get_pipe_params()[index].static_venc_attr.deblur_adaptive_en;

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

    crosstalk_attr.enable = get_pipe_params()[index].static_crosstalk.enable;
    ret = hi_mpi_isp_set_crosstalk_attr(vi_pipe, &crosstalk_attr);
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
            get_pipe_params()[index].dynamic_ldci.enable_exp_thresh_ltoh, HI_SCENE_LDCI_EXPOSURE_MAX_COUNT);
        ot_scenecomm_expr_true_return(exp_level >= HI_SCENE_LDCI_EXPOSURE_MAX_COUNT, HI_FAILURE);
        ldci_attr.enable = get_pipe_params()[index].dynamic_ldci.enable[exp_level];

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

hi_void ot_scene_set_fps_param_board(hi_u32 exposure, hi_u32 *frame_rate, hi_u32 *ae_time, hi_u32 *gop, hi_u8 index)
{
    hi_u32 exp_level;
    exp_level = scene_get_level_ltoh(exposure, get_pipe_params()[index].dynamic_fps.fps_exposure_cnt,
        get_pipe_params()[index].dynamic_fps.exp_ltoh_thresh, HI_SCENE_FPS_ISO_MAX_COUNT);
    if ((exp_level == 0) || (exp_level == (hi_u32)(get_pipe_params()[index].dynamic_fps.fps_exposure_cnt - 1))) {
        *frame_rate = get_pipe_params()[index].dynamic_fps.fps[exp_level];
        *gop = get_pipe_params()[index].dynamic_fps.venc_gop[exp_level];
        *ae_time = get_pipe_params()[index].dynamic_fps.ae_max_time[exp_level];
    } else {
        hi_u64 left_exposure;
        hi_u64 right_expoure;
        left_exposure =  get_pipe_params()[index].dynamic_fps.exp_ltoh_thresh[exp_level - 1];
        right_expoure = get_pipe_params()[index].dynamic_fps.exp_ltoh_thresh[exp_level];
        *frame_rate = scene_interpulate(exposure, left_exposure,
                                        get_pipe_params()[index].dynamic_fps.fps[exp_level - 1],
                                        right_expoure, get_pipe_params()[index].dynamic_fps.fps[exp_level]);
        *ae_time = scene_interpulate(exposure, left_exposure,
                                     get_pipe_params()[index].dynamic_fps.ae_max_time[exp_level - 1],
                                     right_expoure, get_pipe_params()[index].dynamic_fps.ae_max_time[exp_level]);
        *gop = scene_interpulate(exposure, left_exposure,
                                 get_pipe_params()[index].dynamic_fps.venc_gop[exp_level - 1],
                                 right_expoure, get_pipe_params()[index].dynamic_fps.venc_gop[exp_level]);
    }
}

hi_void ot_scene_set_pub_ae_venc(hi_vi_pipe vi_pipe, hi_u32 frame_rate, hi_u32 ae_time, hi_u32 gop)
{
    hi_s32 ret;
    hi_isp_pub_attr pub_attr;
    hi_isp_exposure_attr exposure_attr;
    hi_venc_chn_attr venc_attr;

    ret = hi_mpi_isp_get_pub_attr(vi_pipe, &pub_attr);
    check_scene_ret(ret);
    pub_attr.frame_rate = frame_rate;
    ret = hi_mpi_isp_set_pub_attr(vi_pipe, &pub_attr);
    check_scene_ret(ret);
    ret = hi_mpi_isp_get_exposure_attr(vi_pipe, &exposure_attr);
    check_scene_ret(ret);
    exposure_attr.auto_attr.exp_time_range.max = ae_time;
    ret = hi_mpi_isp_set_exposure_attr(vi_pipe, &exposure_attr);
    check_scene_ret(ret);
    ret = hi_mpi_venc_get_chn_attr(0, &venc_attr);
    check_scene_ret(ret);

    if (venc_attr.rc_attr.rc_mode == HI_VENC_RC_MODE_H265_CBR) {
        venc_attr.rc_attr.h265_cbr.dst_frame_rate = frame_rate;
        venc_attr.rc_attr.h265_cbr.src_frame_rate = frame_rate;
        venc_attr.rc_attr.h265_cbr.gop = gop;
    } else if (venc_attr.rc_attr.rc_mode == HI_VENC_RC_MODE_H265_AVBR) {
        venc_attr.rc_attr.h265_avbr.dst_frame_rate = frame_rate;
        venc_attr.rc_attr.h265_avbr.src_frame_rate = frame_rate;
        venc_attr.rc_attr.h265_avbr.gop = gop;
    } else if (venc_attr.rc_attr.rc_mode == HI_VENC_RC_MODE_H265_CVBR) {
        venc_attr.rc_attr.h265_cvbr.dst_frame_rate = frame_rate;
        venc_attr.rc_attr.h265_cvbr.src_frame_rate = frame_rate;
        venc_attr.rc_attr.h265_cvbr.gop = gop;
    } else if (venc_attr.rc_attr.rc_mode == HI_VENC_RC_MODE_H264_CBR) {
        venc_attr.rc_attr.h264_cbr.dst_frame_rate = frame_rate;
        venc_attr.rc_attr.h264_cbr.src_frame_rate = frame_rate;
        venc_attr.rc_attr.h264_cbr.gop = gop;
    }  else if (venc_attr.rc_attr.rc_mode == HI_VENC_RC_MODE_H264_AVBR) {
        venc_attr.rc_attr.h264_avbr.dst_frame_rate = frame_rate;
        venc_attr.rc_attr.h264_avbr.src_frame_rate = frame_rate;
        venc_attr.rc_attr.h264_avbr.gop = gop;
    } else if (venc_attr.rc_attr.rc_mode == HI_VENC_RC_MODE_H264_CVBR) {
        venc_attr.rc_attr.h264_cvbr.dst_frame_rate = frame_rate;
        venc_attr.rc_attr.h264_cvbr.src_frame_rate = frame_rate;
        venc_attr.rc_attr.h264_cvbr.gop = gop;
    } else {
        printf("Warning:SceneAuto now only support H265/H264 CBR/AVBR/CVBR FPS settings.\n");
    }
    ret = hi_mpi_venc_set_chn_attr(0, &venc_attr);
    check_scene_ret(ret);
}

hi_s32 ot_scene_set_dynamic_fps(hi_vi_pipe vi_pipe, hi_u64 exposure, hi_u64 last_exposure, hi_u8 index)
{
    ot_scenecomm_expr_true_return(index >= HI_SCENE_PIPETYPE_NUM, HI_FAILURE);
    if ((get_pipe_params()[index].module_state.dynamic_fps != HI_TRUE) || (last_exposure == exposure)) {
        return HI_SUCCESS;
    }
    hi_u32 exp_level;
    hi_u32 frame_rate;
    hi_u32 ae_time;
    hi_u32 gop;

    exp_level = scene_get_level_ltoh(exposure, get_pipe_params()[index].dynamic_fps.fps_exposure_cnt,
        get_pipe_params()[index].dynamic_fps.exp_ltoh_thresh, HI_SCENE_FPS_ISO_MAX_COUNT);
    ot_scene_set_fps_param_board(exposure, &frame_rate, &ae_time, &gop, index);
    if (g_sceneauto_fps.enable == HI_TRUE) {
        if (g_sceneauto_fps.op_type == HI_SCENE_OP_TYPE_AUTO) {
            frame_rate = (g_sceneauto_fps.auto_s.fps_max < get_pipe_params()[index].dynamic_fps.fps[exp_level]) ?
                         (g_sceneauto_fps.auto_s.fps_max) : frame_rate;
            gop = (g_sceneauto_fps.auto_s.fps_max < get_pipe_params()[index].dynamic_fps.fps[exp_level]) ?
                  (frame_rate << 1) : gop;
        } else {
            frame_rate = g_sceneauto_fps.manual.fps;
            gop = frame_rate << 1;
        }
    }

    if (exposure != last_exposure) {
        ot_scene_set_pub_ae_venc(vi_pipe, frame_rate, ae_time, gop);
    }
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
    static hi_u32 last_ratio[HI_VI_MAX_PIPE_NUM] = {[0 ... HI_VI_MAX_PIPE_NUM - 1] = 0x40};
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
            get_pipe_params()[index].dynamic_ae.exp_ltoh_thresh, HI_SCENE_AE_EXPOSURE_MAX_COUNT);

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
    hi_u32  ratio_level = (hi_u32)scene_get_level_ltoh_u32((hi_u32)wdr_ratio, ratio_count, ratio_level_thresh,
                                                           ISP_AUTO_RATIO_FSWDR_NUM);
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
        ratio_level = scene_get_level_ltoh_u32(inner_state_info.wdr_exp_ratio_actual[0], ratio_count,
            ratio_level_thresh, HI_SCENE_DRC_RATIO_MAX_COUNT);
    } else {
        ratio_level = scene_get_level_ltoh_u32(wdr_ratio, ratio_count, ratio_level_thresh,
            HI_SCENE_DRC_RATIO_MAX_COUNT);
    }
    iso_level = scene_get_level_ltoh_u32(iso, iso_count, iso_level_thresh, HI_SCENE_DRC_ISO_MAX_COUNT);
    ot_scenecomm_expr_true_return(ratio_level >= HI_SCENE_DRC_RATIO_MAX_COUNT, HI_FAILURE);
    ot_scenecomm_expr_true_return(iso_level >= HI_SCENE_DRC_ISO_MAX_COUNT, HI_FAILURE);

    set_isp_attr_param param = { index, iso_level, iso_count, ratio_level, ratio_count, interval, wdr_ratio, iso };
    scene_set_isp_attr(param, ratio_level_thresh, iso_level_thresh, &isp_drc_attr);

    /* right shift 3 */
    ret = scene_set_tone_mapping_value(vi_pipe, index, (interval >> 3), &inner_state_info, &isp_drc_attr);
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
    static hi_u32 last_ratio[HI_VI_MAX_PIPE_NUM] = {[0 ... HI_VI_MAX_PIPE_NUM - 1] = 0x40};
    hi_u32 actual_ratio;

    ret = hi_mpi_isp_query_inner_state_info(vi_pipe, &state_info);
    check_scene_ret(ret);

    actual_ratio = state_info.wdr_exp_ratio_actual[0];
    if ((exposure != last_exposure) || (last_ratio[vi_pipe] != actual_ratio)) {
        check_scene_return_if_pause();
        ret  = hi_mpi_isp_get_dehaze_attr(vi_pipe, &dehaze_attr);
        check_scene_ret(ret);

        exp_level = scene_get_level_ltoh(exposure, get_pipe_params()[index].dynamic_dehaze.exp_thresh_cnt,
            get_pipe_params()[index].dynamic_dehaze.exp_thresh_ltoh, HI_SCENE_DEHAZE_EXPOSURE_MAX_COUNT);
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

static hi_s32 ot_scene_set_dynamic_nr_part1(ot_scene_nr_para *nr_para, hi_u8 index, hi_u32 wdr_ratio)
{
    hi_u32 i, ratio_index;
    hi_u32 ratio_count = get_pipe_params()[index].dynamic_nr.ratio_count;
    hi_u32 *level = get_pipe_params()[index].dynamic_nr.ratio_level;

    ratio_index = scene_get_level_ltoh_u32(wdr_ratio, ratio_count, level, HI_SCENE_DRC_RATIO_MAX_COUNT);
    ot_scenecomm_expr_true_return(ratio_index >= HI_SCENE_DRC_RATIO_MAX_COUNT, HI_FAILURE);

    if ((ratio_index == 0) || (ratio_index == (ratio_count - 1))) {
        for (i = 0; i < HI_ISP_WDR_MAX_FRAME_NUM; i++) {
            nr_para->sfm0_fusion_incr[i] =
                get_pipe_params()[index].dynamic_nr.snr_sfm0_fusion_frame_str_incr[i][ratio_index];
            nr_para->sfm0_wdr_incr[i] = get_pipe_params()[index].dynamic_nr.snr_sfm0_wdr_frame_str_incr[i][ratio_index];
        }
    } else {
        for (i = 0; i < HI_ISP_WDR_MAX_FRAME_NUM; i++) {
            nr_para->sfm0_fusion_incr[i] = scene_interpulate(wdr_ratio, level[ratio_index - 1],
                get_pipe_params()[index].dynamic_nr.snr_sfm0_fusion_frame_str_incr[i][ratio_index - 1],
                level[ratio_index], get_pipe_params()[index].dynamic_nr.snr_sfm0_fusion_frame_str_incr[i][ratio_index]);

            nr_para->sfm0_wdr_incr[i] = scene_interpulate(wdr_ratio, level[ratio_index - 1],
                get_pipe_params()[index].dynamic_nr.snr_sfm0_wdr_frame_str_incr[i][ratio_index - 1],
                level[ratio_index], get_pipe_params()[index].dynamic_nr.snr_sfm0_wdr_frame_str_incr[i][ratio_index]);
        }
    }

    return HI_SUCCESS;
}

hi_s32 ot_scene_set_dynamic_nr(hi_vi_pipe vi_pipe, hi_u8 index, hi_u32 wdr_ratio, hi_u32 iso)
{
    ot_scenecomm_expr_true_return(index >= HI_SCENE_PIPETYPE_NUM, HI_FAILURE);
    if (get_pipe_params()[index].module_state.dynamic_nr != HI_TRUE) {
        return HI_SUCCESS;
    }

    hi_s32 ret;
    hi_u32 ratio_index;
    hi_u32 ratio_count = get_pipe_params()[index].dynamic_nr.ratio_count;
    hi_u32 *level = get_pipe_params()[index].dynamic_nr.ratio_level;
    ot_scene_nr_para nr_para = {{0}};
    hi_isp_nr_attr nr_attr;

    ratio_index = scene_get_level_ltoh_u32(wdr_ratio, ratio_count, level, HI_SCENE_DRC_RATIO_MAX_COUNT);
    ot_scenecomm_expr_true_return(ratio_index >= HI_SCENE_DRC_RATIO_MAX_COUNT, HI_FAILURE);

    check_scene_return_if_pause();
    ret = hi_mpi_isp_get_nr_attr(vi_pipe, &nr_attr);
    check_scene_ret(ret);

    ret = ot_scene_set_dynamic_nr_part1(&nr_para, index, wdr_ratio);
    check_scene_ret(ret);

    ret = scene_set_nr_attr_para(index, iso, &nr_attr, &nr_para);
    check_scene_ret(ret);

    ret = scene_set_nr_wdr_ratio_para(vi_pipe, index, wdr_ratio, ratio_index, &nr_attr);
    check_scene_ret(ret);

    check_scene_return_if_pause();
    ret = hi_mpi_isp_set_nr_attr(vi_pipe, &nr_attr);
    check_scene_ret(ret);
    return HI_SUCCESS;
}

static hi_s32 ot_scene_set_dynamic_dpc_part1(hi_isp_dp_dynamic_attr *dp_dynamic_attr, hi_u8 index, hi_u64 iso)
{
    hi_u32  iso_count = get_pipe_params()[index].dynamic_dpc.iso_count;
    hi_u32 *iso_level_thresh = get_pipe_params()[index].dynamic_dpc.iso_level;
    hi_u32 iso_level = (hi_u32)scene_get_level_ltoh_u32((hi_u32)iso, iso_count, iso_level_thresh, ISP_AUTO_ISO_DPC_NUM);
    ot_scenecomm_expr_true_return(iso_level >= ISP_AUTO_ISO_DPC_NUM, HI_FAILURE);

    if ((iso_level == (iso_count - 1)) || (iso_level == 0)) {
            dp_dynamic_attr->frame_dynamic[0].sup_twinkle_en =
                get_pipe_params()[index].dynamic_dpc.sup_twinkle_en[iso_level];
            dp_dynamic_attr->frame_dynamic[0].soft_thr =
                (hi_s8)get_pipe_params()[index].dynamic_dpc.soft_thr[iso_level];
            dp_dynamic_attr->frame_dynamic[0].soft_slope =
                get_pipe_params()[index].dynamic_dpc.soft_slope[iso_level];
            dp_dynamic_attr->frame_dynamic[0].bright_strength =
                get_pipe_params()[index].dynamic_dpc.bright_strength[iso_level];
            dp_dynamic_attr->frame_dynamic[0].dark_strength =
                get_pipe_params()[index].dynamic_dpc.dark_strength[iso_level];
    } else {
        hi_u32 mid_iso = (hi_u32)iso;
        hi_u32 left_iso = iso_level_thresh[iso_level - 1];
        hi_u32 right_iso = iso_level_thresh[iso_level];
        dp_dynamic_attr->frame_dynamic[0].sup_twinkle_en =
            get_pipe_params()[index].dynamic_dpc.sup_twinkle_en[iso_level - 1];
        dp_dynamic_attr->frame_dynamic[0].soft_thr = scene_interpulate(mid_iso, left_iso,
            get_pipe_params()[index].dynamic_dpc.soft_thr[iso_level - 1], right_iso,
            get_pipe_params()[index].dynamic_dpc.soft_thr[iso_level]);
        dp_dynamic_attr->frame_dynamic[0].soft_slope = scene_interpulate(mid_iso, left_iso,
            get_pipe_params()[index].dynamic_dpc.soft_slope[iso_level - 1], right_iso,
            get_pipe_params()[index].dynamic_dpc.soft_slope[iso_level]);
        dp_dynamic_attr->frame_dynamic[0].bright_strength = scene_interpulate(mid_iso, left_iso,
            get_pipe_params()[index].dynamic_dpc.bright_strength[iso_level - 1], right_iso,
            get_pipe_params()[index].dynamic_dpc.bright_strength[iso_level]);
        dp_dynamic_attr->frame_dynamic[0].dark_strength = scene_interpulate(mid_iso, left_iso,
            get_pipe_params()[index].dynamic_dpc.dark_strength[iso_level - 1], right_iso,
            get_pipe_params()[index].dynamic_dpc.dark_strength[iso_level]);
    }

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

    check_scene_return_if_pause();
    ret = hi_mpi_isp_get_dp_dynamic_attr(vi_pipe, &dp_dynamic_attr);
    check_scene_ret(ret);

    ret = ot_scene_set_dynamic_dpc_part1(&dp_dynamic_attr, index, iso);
    check_scene_ret(ret);

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
            get_pipe_params()[index].dynamic_shading.exp_thresh_ltoh, HI_SCENE_SHADING_EXPOSURE_MAX_COUNT);
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

hi_s32 ot_scene_set_dynamic_linear_ca(hi_vi_pipe vi_pipe, hi_u64 iso, hi_u64 last_iso, hi_u8 index)
{
    ot_scenecomm_expr_true_return(index >= HI_SCENE_PIPETYPE_NUM, HI_FAILURE);
    if ((get_pipe_params()[index].module_state.dynamic_linear_ca != HI_TRUE) || (iso == last_iso)) {
        return HI_SUCCESS;
    }

    hi_s32 ret;
    hi_s32 i;
    hi_isp_ca_attr ca_attr;
    hi_u32 ca_y_ratio_iso_interp_lut[HI_ISP_CA_YRATIO_LUT_LENGTH];
    hi_u32 ca_y_sat_lut_interp_lut[HI_ISP_CA_YRATIO_LUT_LENGTH];
    hi_u32  iso_count = get_pipe_params()[index].dynamic_ca.iso_count;
    hi_u32 *iso_level_thresh = get_pipe_params()[index].dynamic_ca.iso_level;
    hi_u32 iso_level = (hi_u32)scene_get_level_ltoh_u32((hi_u32)iso, iso_count, iso_level_thresh,
        HI_SCENE_ISO_STRENGTH_NUM);

    ot_scenecomm_expr_true_return(iso_level >= ISP_AUTO_ISO_DPC_NUM, HI_FAILURE);
    check_scene_return_if_pause();
    ret = hi_mpi_isp_get_ca_attr(vi_pipe, &ca_attr);
    check_scene_ret(ret);

    if ((iso_level == (iso_count - 1)) || (iso_level == 0)) {
        for (i = 0; i < HI_ISP_CA_YRATIO_LUT_LENGTH; i++) {
            ca_y_ratio_iso_interp_lut[i] = get_pipe_params()[index].dynamic_ca.ca_y_ratio_iso_lut[iso_level][i];
            ca_y_sat_lut_interp_lut[i] = get_pipe_params()[index].dynamic_ca.ca_y_ratio_iso_lut[iso_level][i];
        }
    } else {
        hi_u32 mid_iso = (hi_u32)iso;
        hi_u32 left_iso = iso_level_thresh[iso_level - 1];
        hi_u32 right_iso = iso_level_thresh[iso_level];
        for (i = 0; i < HI_ISP_CA_YRATIO_LUT_LENGTH; i++) {
            ca_y_ratio_iso_interp_lut[i] = scene_interpulate(mid_iso, left_iso,
                get_pipe_params()[index].dynamic_ca.ca_y_ratio_iso_lut[iso_level - 1][i], right_iso,
                get_pipe_params()[index].dynamic_ca.ca_y_ratio_iso_lut[iso_level][i]);
            ca_y_sat_lut_interp_lut[i] = scene_interpulate(mid_iso, left_iso,
                get_pipe_params()[index].dynamic_ca.ca_y_sat_iso_lut[iso_level - 1][i], right_iso,
                get_pipe_params()[index].dynamic_ca.ca_y_sat_iso_lut[iso_level][i]);
        }
    }

    for (i = 0; i < HI_ISP_CA_YRATIO_LUT_LENGTH; i++) {
        ca_attr.ca.y_ratio_lut[i] = ca_y_ratio_iso_interp_lut[i];
        ca_attr.ca.y_sat_lut[i] = ca_y_sat_lut_interp_lut[i];
    }

    check_scene_return_if_pause();
    ret = hi_mpi_isp_set_ca_attr(vi_pipe, &ca_attr);
    check_scene_ret(ret);
    return HI_SUCCESS;
}

static hi_s32  ot_scene_set_dynamic_ca_part1(hi_isp_ca_attr *ca_attr, hi_u8 index, hi_u32 wdr_ratio, hi_u32 iso)
{
    hi_u32 i;
    hi_u32 ca_y_ratio_iso_interp_lut[HI_ISP_CA_YRATIO_LUT_LENGTH];
    hi_u32 ca_y_ratio_ratio_interp_lut[HI_ISP_CA_YRATIO_LUT_LENGTH];
    hi_u32 blend_weight;
    hi_u32  iso_count = get_pipe_params()[index].dynamic_ca.iso_count;
    hi_u32 *iso_level_thresh = get_pipe_params()[index].dynamic_ca.iso_level;
    hi_u32 iso_level = scene_get_level_ltoh_u32((hi_u32)iso, iso_count, iso_level_thresh, HI_SCENE_ISO_STRENGTH_NUM);
    hi_u32  ratio_count = get_pipe_params()[index].dynamic_ca.ratio_count;
    hi_u32 *ratio_level_thresh = get_pipe_params()[index].dynamic_ca.ratio_level;
    hi_u32 ratio_level = scene_get_level_ltoh_u32((hi_u32)wdr_ratio, ratio_count, ratio_level_thresh,
        HI_SCENE_RATIO_STRENGTH_NUM);
    ot_scenecomm_expr_true_return(iso_level >= ISP_AUTO_ISO_CA_NUM, HI_FAILURE);
    if ((iso_level == (iso_count - 1)) || (iso_level == 0)) {
        for (i = 0; i < HI_ISP_CA_YRATIO_LUT_LENGTH; i++) {
            ca_y_ratio_iso_interp_lut[i] = get_pipe_params()[index].dynamic_ca.ca_y_ratio_iso_lut[iso_level][i];
        }
    } else {
        hi_u32 left_iso = iso_level_thresh[iso_level - 1];
        hi_u32 right_iso = iso_level_thresh[iso_level];
        for (i = 0; i < HI_ISP_CA_YRATIO_LUT_LENGTH; i++) {
            ca_y_ratio_iso_interp_lut[i] = scene_interpulate(iso, left_iso,
                get_pipe_params()[index].dynamic_ca.ca_y_ratio_iso_lut[iso_level - 1][i], right_iso,
                get_pipe_params()[index].dynamic_ca.ca_y_ratio_iso_lut[iso_level][i]);
        }
    }
    if ((ratio_level == (ratio_count - 1)) || (ratio_level == 0)) {
        blend_weight = get_pipe_params()[index].dynamic_ca.blend_weight[ratio_level];
        for (i = 0; i < HI_ISP_CA_YRATIO_LUT_LENGTH; i++) {
            ca_y_ratio_ratio_interp_lut[i] = get_pipe_params()[index].dynamic_ca.ca_y_ratio_ratio_lut[ratio_level][i];
        }
    } else {
        hi_u32 mid_ratio = (hi_u32)wdr_ratio;
        hi_u32 left_ratio = ratio_level_thresh[ratio_level - 1];
        hi_u32 right_ratio = ratio_level_thresh[ratio_level];
        blend_weight = scene_interpulate(mid_ratio, left_ratio,
            get_pipe_params()[index].dynamic_ca.blend_weight[ratio_level - 1], right_ratio,
            get_pipe_params()[index].dynamic_ca.blend_weight[ratio_level]);
        for (i = 0; i < HI_ISP_CA_YRATIO_LUT_LENGTH; i++) {
            ca_y_ratio_ratio_interp_lut[i] = scene_interpulate(mid_ratio, left_ratio,
                get_pipe_params()[index].dynamic_ca.ca_y_ratio_ratio_lut[ratio_level - 1][i], right_ratio,
                get_pipe_params()[index].dynamic_ca.ca_y_ratio_ratio_lut[ratio_level][i]);
        }
    }
    for (i = 0; i < HI_ISP_CA_YRATIO_LUT_LENGTH; i++) {
        ca_attr->ca.y_ratio_lut[i] = (blend_weight * ca_y_ratio_iso_interp_lut[i] +
            (ISP_SCENE_CA_MAX_BLEND_WEIGHT - blend_weight) *ca_y_ratio_ratio_interp_lut[i]) >> ISP_SCENE_CA_SHIFT;
    }
    return HI_SUCCESS;
}

hi_s32 ot_scene_set_dynamic_ca(hi_vi_pipe vi_pipe, hi_u8 index, hi_u32 wdr_ratio, hi_u32 iso)
{
    ot_scenecomm_expr_true_return(index >= HI_SCENE_PIPETYPE_NUM, HI_FAILURE);
    if ((get_pipe_params()[index].module_state.dynamic_ca != HI_TRUE)) {
        return HI_SUCCESS;
    }

    hi_s32 ret;
    hi_isp_ca_attr ca_attr;

    check_scene_return_if_pause();
    ret = hi_mpi_isp_get_ca_attr(vi_pipe, &ca_attr);
    check_scene_ret(ret);

    ret = ot_scene_set_dynamic_ca_part1(&ca_attr, index, wdr_ratio, iso);
    check_scene_ret(ret);

    check_scene_return_if_pause();
    ret = hi_mpi_isp_set_ca_attr(vi_pipe, &ca_attr);
    check_scene_ret(ret);
    return HI_SUCCESS;
}

static hi_s32  ot_scene_set_dynamic_color_sector_part1(hi_isp_color_sector_attr *cs_attr, hi_u8 index, hi_u32 iso)
{
    hi_u32 i;
    hi_u32 j;
    hi_u32  iso_count = get_pipe_params()[index].dynamic_cs.iso_count;
    hi_u32 *iso_level_thresh = get_pipe_params()[index].dynamic_cs.iso_level;
    hi_u32 iso_level = (hi_u32)scene_get_level_ltoh_u32((hi_u32)iso, iso_count, iso_level_thresh,
        HI_SCENE_ISO_STRENGTH_NUM);
    ot_scenecomm_expr_true_return(iso_level >= ISP_AUTO_ISO_CS_NUM, HI_FAILURE);

    if ((iso_level == (iso_count - 1)) || (iso_level == 0)) {
        for (j = 0; j < HI_SCENE_CS_NUM; j++) {
            for (i = 0; i < HI_ISP_COLOR_SECTOR_NUM; i++) {
                cs_attr->auto_attr.color_tab[j].hue_shift[i] =
                    get_pipe_params()[index].dynamic_cs.hue_shift[j][i][iso_level];
                cs_attr->auto_attr.color_tab[j].sat_shift[i] =
                    get_pipe_params()[index].dynamic_cs.sat_shift[j][i][iso_level];
            }
        }
    } else {
        hi_u32 mid_iso = (hi_u32)iso;
        hi_u32 left_iso = iso_level_thresh[iso_level - 1];
        hi_u32 right_iso = iso_level_thresh[iso_level];
        for (j = 0; j < HI_SCENE_CS_NUM; j++) {
            for (i = 0; i < HI_ISP_COLOR_SECTOR_NUM; i++) {
                cs_attr->auto_attr.color_tab[j].hue_shift[i] = scene_interpulate(mid_iso, left_iso,
                    get_pipe_params()[index].dynamic_cs.hue_shift[j][i][iso_level - 1], right_iso,
                    get_pipe_params()[index].dynamic_cs.hue_shift[j][i][iso_level]);
                cs_attr->auto_attr.color_tab[j].sat_shift[i] = scene_interpulate(mid_iso, left_iso,
                    get_pipe_params()[index].dynamic_cs.sat_shift[j][i][iso_level - 1], right_iso,
                    get_pipe_params()[index].dynamic_cs.sat_shift[j][i][iso_level]);
            }
        }
    }

    return HI_SUCCESS;
}

hi_s32 ot_scene_set_dynamic_color_sector(hi_vi_pipe vi_pipe, hi_u64 iso, hi_u64 last_iso, hi_u8 index)
{
    ot_scenecomm_expr_true_return(index >= HI_SCENE_PIPETYPE_NUM, HI_FAILURE);
    if ((get_pipe_params()[index].module_state.dynamic_color_sector != HI_TRUE) || (iso == last_iso)) {
        return HI_SUCCESS;
    }

    hi_s32 ret;
    hi_isp_color_sector_attr cs_attr;
    check_scene_return_if_pause();
    ret = hi_mpi_isp_get_color_sector_attr(vi_pipe, &cs_attr);
    check_scene_ret(ret);

    ret = ot_scene_set_dynamic_color_sector_part1(&cs_attr, index, iso);
    check_scene_ret(ret);

    check_scene_return_if_pause();
    ret = hi_mpi_isp_set_color_sector_attr(vi_pipe, &cs_attr);
    check_scene_ret(ret);
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
    hi_u32 iso_level = (hi_u32)scene_get_level_ltoh_u32((hi_u32)iso, iso_count, iso_level_thresh, ISP_AUTO_ISO_BLC_NUM);
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

hi_s32 ot_scene_set_dynamic_csc(hi_vi_pipe vi_pipe, hi_u64 iso, hi_u64 last_iso, hi_u8 index)
{
    ot_scenecomm_expr_true_return(index >= HI_SCENE_PIPETYPE_NUM, HI_FAILURE);
    if ((get_pipe_params()[index].module_state.dynamic_csc != HI_TRUE) || (iso == last_iso)) {
        return HI_SUCCESS;
    }

    hi_s32 ret;
    hi_isp_csc_attr csc_attr;

    hi_u32 iso_count = get_pipe_params()[index].dynamic_csc.iso_count;
    hi_u32 *iso_level_thresh = get_pipe_params()[index].dynamic_csc.iso_level;
    hi_u32 iso_level = (hi_u32)scene_get_level_ltoh_u32((hi_u32)iso, iso_count, iso_level_thresh,
        HI_SCENE_ISO_STRENGTH_NUM);
    ot_scenecomm_expr_true_return(iso_level >= HI_SCENE_ISO_STRENGTH_NUM, HI_FAILURE);

    check_scene_return_if_pause();
    ret = hi_mpi_isp_get_csc_attr(vi_pipe, &csc_attr);
    check_scene_ret(ret);

    if ((iso_level == (iso_count - 1)) || (iso_level == 0)) {
        csc_attr.contr = get_pipe_params()[index].dynamic_csc.contrast[iso_level];
        csc_attr.satu = get_pipe_params()[index].dynamic_csc.saturation[iso_level];
    } else {
        hi_u32 mid_iso = (hi_u32)iso;
        hi_u32 left_iso = iso_level_thresh[iso_level - 1];
        hi_u32 right_iso = iso_level_thresh[iso_level];

        csc_attr.contr = scene_interpulate(mid_iso, left_iso,
            get_pipe_params()[index].dynamic_csc.contrast[iso_level - 1], right_iso,
            get_pipe_params()[index].dynamic_csc.contrast[iso_level]);

        csc_attr.satu = scene_interpulate(mid_iso, left_iso,
            get_pipe_params()[index].dynamic_csc.saturation[iso_level - 1], right_iso,
            get_pipe_params()[index].dynamic_csc.saturation[iso_level]);
    }
    check_scene_return_if_pause();
    ret = hi_mpi_isp_set_csc_attr(vi_pipe, &csc_attr);
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
            get_pipe_params()[index].dynamic_venc_bitrate.iso_thresh_ltoh, HI_SCENE_VENC_EXPOSURE_MAX_COUNT);
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

hi_s32 ot_scene_set_dynamic_venc_mode(hi_vi_pipe vi_pipe, hi_u64 iso, hi_u64 last_iso, hi_u8 index)
{
    ot_scenecomm_expr_true_return(index >= HI_SCENE_PIPETYPE_NUM, HI_FAILURE);
    if (get_pipe_params()[index].module_state.dynamic_venc_mode != HI_TRUE) {
        return HI_SUCCESS;
    }
    hi_s32 ret;
    hi_venc_deblur_param deblur_param;

    hi_u32 *deblur_mode_iso_thresh = get_pipe_params()[index].dynamic_venc_mode.deblur_mode_iso_thresh;
    hi_bool *deblur_en = get_pipe_params()[index].dynamic_venc_mode.deblur_en;
    hi_bool *deblur_adaptive_en = get_pipe_params()[index].dynamic_venc_mode.deblur_adaptive_en;

    check_scene_return_if_pause();
    ret = hi_mpi_venc_get_deblur(vi_pipe, &deblur_param);
    check_scene_ret(ret);

    if (iso >= deblur_mode_iso_thresh[1]) {
        deblur_param.deblur_en = deblur_en[1];
        deblur_param.deblur_adaptive_en = deblur_adaptive_en[1];
    }

    if (iso < deblur_mode_iso_thresh[0]) {
        deblur_param.deblur_en = deblur_en[0];
        deblur_param.deblur_adaptive_en = deblur_adaptive_en[0];
    }

    check_scene_return_if_pause();
    ret = hi_mpi_venc_set_deblur(vi_pipe, &deblur_param);
    check_scene_ret(ret);

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
    static hi_u32 last_exp_level[HI_VI_MAX_PIPE_NUM] = {0};
    hi_s32 ret;
    hi_isp_gamma_attr isp_gamma_attr;

    if (exposure != last_exposure) {
        exp_level = scene_get_level_ltoh(exposure, get_pipe_params()[index].dynamic_gamma.total_num,
            get_pipe_params()[index].dynamic_gamma.exp_thresh_htol, HI_SCENE_GAMMA_EXPOSURE_MAX_COUNT);

        ot_scenecomm_expr_true_return(index >= HI_SCENE_PIPETYPE_NUM, HI_FAILURE);
        ot_scenecomm_expr_true_return(exp_level >= HI_SCENE_GAMMA_EXPOSURE_MAX_COUNT, HI_FAILURE);
        ot_scenecomm_expr_true_return(vi_pipe >= HI_VI_MAX_PIPE_NUM, HI_FAILURE);
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
    return ot_scene_set_dynamic_video_gamma(vi_pipe, exposure, last_exposure, index);
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
            get_pipe_params()[index].dynamic_false_color.false_color_exp_thresh,
            HI_SCENE_FALSECOLOR_EXPOSURE_MAX_COUNT);
        ot_scenecomm_expr_true_return(exp_level >= HI_SCENE_FALSECOLOR_EXPOSURE_MAX_COUNT, HI_FAILURE);
        anti_false_color_attr.manual_attr.strength =
            get_pipe_params()[index].dynamic_false_color.manual_strength[exp_level];

        check_scene_return_if_pause();
        ret = hi_mpi_isp_set_anti_false_color_attr(vi_pipe, &anti_false_color_attr);
        check_scene_ret(ret);
    }
    return HI_SUCCESS;
}

static hi_bool g_fpn_enable[HI_VI_MAX_PIPE_NUM] = {HI_FALSE};

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
        for (i = 0; (i < iso_count - 1) && i < HI_SCENE_ISO_STRENGTH_NUM; i++) {
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

    static hi_u32 last_iso_tmp[HI_VI_MAX_PIPE_NUM] = {0};
    hi_isp_exp_info exp_info;
    hi_u32 iso_count = get_pipe_params()[index].dynamic_fpn.iso_count;
    hi_u32 *iso_level_thresh = get_pipe_params()[index].dynamic_fpn.iso_thresh;
    sample_scene_fpn_offset_cfg  scene_fpn_offset_cfg;

    check_scene_return_if_pause();
    ret = hi_mpi_isp_query_exposure_info(vi_pipe, &exp_info);
    check_scene_ret(ret);

    sns_iso = ((hi_u64)exp_info.a_gain * exp_info.d_gain * FPN_GAIN_ISO) >> FPN_GAIN_SHIFT;
    iso_level = (hi_u32)scene_get_level_ltoh_u32((hi_u32)sns_iso, iso_count, iso_level_thresh,
        HI_SCENE_ISO_STRENGTH_NUM);
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
    check_scene_return_if_pause();
    hi_u32 iso_level = 0;
    ot_scene_3dnr nrx_attr;
    const ot_scene_static_3dnr *nrx_param = &(get_pipe_params()[index].static_threednr);
    hi_u32 count = nrx_param->threed_nr_count;
    hi_u32 *thresh = (hi_u32 *)nrx_param->threed_nr_iso;
    iso_level = scene_get_level_ltoh_u32(iso, count, thresh, HI_SCENE_3DNR_MAX_COUNT);
    if (iso_level == 0) {
        nrx_attr = nrx_param->threednr_value[0];
    } else {
        ot_scenecomm_expr_true_return(iso_level >= HI_SCENE_3DNR_MAX_COUNT, HI_FAILURE);
        hi_u32 mid = iso;
        hi_u32 left = thresh[iso_level - 1];
        hi_u32 right = thresh[iso_level];

        nrx_attr = nrx_param->threednr_value[iso_level];

        _3dnr_nrx_pack pack = { &nrx_attr, nrx_param, iso_level, mid, left, right };

        scene_set_3dnr_nrx_nry(&pack);

        scene_set_3dnr_nrx_iey(&pack);

        scene_set_3dnr_nrx_sfy(&pack);

        scene_set_3dnr_sfy_lut(&pack);

        scene_set_3dnr_nrx_tfy(&pack);

        scene_set_3dnr_nrx_mdy(&pack);

        scene_set_3dnr_nrx_nrc0(&pack);

        scene_set_3dnr_nrx_nrc1(&pack);
    }
    check_scene_return_if_pause();
    hi_s32 ret = scene_set_3dnr(vi_pipe, &nrx_attr, index);
    check_scene_ret(ret);
    return HI_SUCCESS;
}

hi_s32 ot_scene_set_pipe_param(const ot_scene_pipe_param *scene_pipe_param, hi_u32 num)
{
    if (scene_pipe_param == HI_NULL) {
        printf("null pointer");
        return HI_FAILURE;
    }

    errno_t ret = memcpy_s(get_pipe_params(), sizeof(ot_scene_pipe_param) * HI_SCENE_PIPETYPE_NUM, scene_pipe_param,
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
    for (i = 0; i < scene_array_size(g_iso_info) - 1; i++) {
        g_iso_info[i] = g_iso_info[i + 1];
        g_meanqp_info[i] = g_meanqp_info[i + 1];
    }
    g_iso_info[i] = iso;
    g_meanqp_info[i] = mean_qp;
}

static hi_void calculate_sum_iso_mean_qp(hi_u32 min_qp, hi_u32 *sum_mean_qp, hi_u32 *sum_iso)
{
    hi_u32 i;
    hi_u32 local_min_qp = min_qp;
    for (i = 0; i < scene_array_size(g_meanqp_info); i++) {
        if (local_min_qp < g_meanqp_info[i]) {
            local_min_qp = g_meanqp_info[i];
        }
        *sum_mean_qp += g_meanqp_info[i];
        *sum_iso += g_iso_info[i];
    }
    *sum_mean_qp = *sum_mean_qp - local_min_qp;
    *sum_mean_qp = *sum_mean_qp / (hi_u32)(scene_array_size(g_meanqp_info) - 1);
    *sum_iso = *sum_iso / (hi_u32)scene_array_size(g_iso_info);
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
    pic_width = venc_chn_attr.venc_attr.pic_width;
    pic_height = venc_chn_attr.venc_attr.pic_height;

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
    return HI_SUCCESS;
}

#ifdef __cplusplus
}
#endif
