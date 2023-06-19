/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */

#include "scene_loadparam.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include "securec.h"
#include "ot_scene_setparam.h"
#include "ot_confaccess.h"
#include "ot_scenecomm.h"


#ifdef __cplusplus
extern "C" {
#endif

/* param config file path */
#define SCENE_INIPARAM "sceneparamini"
#define SCENE_INI_SCENEMODE "scene_param_"
#define SCENE_INIPARAM_MODULE_NAME_LEN 64
#define SCENE_INIPARAM_NODE_NAME_LEN 128
#define SCENETOOL_MAX_FILESIZE 128

#define SCENE_INI_VIDEOMODE "scene_mode"

#define scene_iniparam_check_load_return(ret, name) do { \
        if ((ret) != HI_SUCCESS) {                       \
            scene_loge(" Load [%s] failed\n", name);     \
            return HI_FAILURE;                           \
        }                                                \
    } while (0)

#define scene_copy_array(dest, src, size, type) do {         \
        for (hi_u32 index_ = 0; index_ < (size); index_++) { \
            (dest)[index_] = (type)(src)[index_];            \
        }                                                    \
    } while (0)

#define scene_load_array(module, node, array, size, type) do {                                    \
        char *got_string_ = NULL;                                                                 \
        hi_s32 ret_ = ot_confaccess_get_string(SCENE_INIPARAM, module, node, NULL, &got_string_); \
        scene_iniparam_check_load_return(ret_, node);                                             \
        if (got_string_ != HI_NULL) {                                                             \
            scene_get_numbers_in_one_line(got_string_);                                           \
            scene_copy_array(array, g_line_data, size, type);                                     \
            ot_scenecomm_safe_free(got_string_);                                                  \
        }                                                                                         \
    } while (0)

#define scene_load_int(module, node, dest, type) do {                             \
        ret = ot_confaccess_get_int(SCENE_INIPARAM, module, node, 0, &get_value); \
        scene_iniparam_check_load_return(ret, node);                              \
        dest = (type)get_value;                                                   \
    } while (0)

/* param */
static ot_scene_param g_scene_param_cfg;

static hi_s64 g_line_data[5000]; /* 5000 line num */

static ot_scene_video_mode g_video_mode;


static hi_s32 scene_get_numbers_in_one_line(const hi_char *input_line)
{
    const hi_char *vr_begin = input_line;
    const hi_char *vr_end = vr_begin;
    hi_u32 part_count = 0;
    hi_char    part[20] = {0}; /* 20 buffer len */
    hi_u32 whole_count = 0;
    hi_u32 length = strlen(input_line);
    hi_u64 hex_value;
    errno_t err;

    hi_s32 i = 0;
    hi_bool is_hex_num = HI_FALSE;
    (hi_void)memset_s(g_line_data, sizeof(g_line_data), 0, sizeof(g_line_data));
    while ((vr_end != NULL)) {
        if ((whole_count > length) || (whole_count == length)) {
            break;
        }

        while ((*vr_end != '|') && (*vr_end != '\0') && (*vr_end != ',')) {
            if (*vr_end == 'x') {
                is_hex_num = HI_TRUE;
            }
            vr_end++;
            part_count++;
            whole_count++;
        }

        err = memcpy_s(part, sizeof(part) - 1, vr_begin, part_count);
        if (err != EOK) {
            break;
        }

        if (is_hex_num == HI_TRUE) {
            hi_char *end_ptr = NULL;
            hex_value = (hi_u64)strtoll(part + 2, &end_ptr, 16); /* 16 Hexadecimal, 2 offset */
            g_line_data[i] = hex_value;
        } else {
            g_line_data[i] = (hi_s64)strtoll(part, NULL, 10); /* base 10 */
        }

        (hi_void)memset_s(part, sizeof(part), 0, sizeof(part));
        part_count = 0;
        vr_end++;
        vr_begin = vr_end;
        whole_count++;
        i++;
    }
    return i;
}

static hi_s32 scene_load_module_state_configs(const hi_char *module, ot_scene_module_state *module_state)
{
    hi_s32 ret, get_value;
    scene_load_int(module, "module_state:bStaticCSC", module_state->static_csc, hi_bool);
    scene_load_int(module, "module_state:bStaticCrosstalk", module_state->static_cross_talk, hi_bool);
    scene_load_int(module, "module_state:bStaticDemosaic", module_state->static_dm, hi_bool);
    scene_load_int(module, "module_state:bStaticBayeshp", module_state->static_bayeshp, hi_bool);
    scene_load_int(module, "module_state:bStaticSharpen", module_state->static_sharpen, hi_bool);
    scene_load_int(module, "module_state:bStatic3DNR", module_state->static_3dnr, hi_bool);
    scene_load_int(module, "module_state:bStaticVenc", module_state->static_venc, hi_bool);
    scene_load_int(module, "module_state:bStaticPreGamma", module_state->static_pre_gamma, hi_bool);
    scene_load_int(module, "module_state:bStaticBlc", module_state->static_blc, hi_bool);
    scene_load_int(module, "module_state:bDynamicAE", module_state->dynamic_ae, hi_bool);
    scene_load_int(module, "module_state:bDynamicWdrExposure", module_state->dynamic_wdr_exposure, hi_bool);
    scene_load_int(module, "module_state:bDynamicFSWDR", module_state->dynamic_fswdr, hi_bool);
    scene_load_int(module, "module_state:bDynamicBLC", module_state->dynamic_blc, hi_bool);
    scene_load_int(module, "module_state:bDynamicDehaze", module_state->dynamic_dehaze, hi_bool);
    scene_load_int(module, "module_state:bDynamicDrc", module_state->dynamic_drc, hi_bool);
    scene_load_int(module, "module_state:bDynamicLinearDrc", module_state->dynamic_linear_drc, hi_bool);
    scene_load_int(module, "module_state:bDynamicGamma", module_state->dynamic_gamma, hi_bool);
    scene_load_int(module, "module_state:bDynamicNr", module_state->dynamic_nr, hi_bool);
    scene_load_int(module, "module_state:bDynamicDpc", module_state->dynamic_dpc, hi_bool);
    scene_load_int(module, "module_state:bDynamicCA", module_state->dynamic_ca, hi_bool);
    scene_load_int(module, "module_state:bDynamicBLC", module_state->dynamic_blc, hi_bool);
    scene_load_int(module, "module_state:bDynamicShading", module_state->dynamic_shading, hi_bool);
    scene_load_int(module, "module_state:bDynamicClut", module_state->dynamic_clut, hi_bool);
    scene_load_int(module, "module_state:bDynamicLdci", module_state->dynamic_ldci, hi_bool);
    scene_load_int(module, "module_state:bDynamicFalseColor", module_state->dynamic_false_color, hi_bool);
    scene_load_int(module, "module_state:bDyanamic3DNR", module_state->dynamic_3dnr, hi_bool);

    return HI_SUCCESS;
}

static hi_s32 scene_load_module_state(const hi_char *module, ot_scene_module_state *module_state)
{
    hi_s32 ret, get_value;
    ot_scenecomm_check_pointer_return(module_state, HI_FAILURE);

    scene_load_int(module, "module_state:bDebug", module_state->debug, hi_bool);
    scene_load_int(module, "module_state:bStaticAE", module_state->static_ae, hi_bool);
    scene_load_int(module, "module_state:bAeWeightTab", module_state->ae_weight_tab, hi_bool);
    scene_load_int(module, "module_state:bStaticWdrExposure", module_state->static_wdr_exposure, hi_bool);
    scene_load_int(module, "module_state:bStaticFsWdr", module_state->static_fswdr, hi_bool);
    scene_load_int(module, "module_state:bStaticAWB", module_state->static_awb, hi_bool);
    scene_load_int(module, "module_state:bStaticAWBEx", module_state->static_awbex, hi_bool);
    scene_load_int(module, "module_state:bStaticCCM", module_state->static_ccm, hi_bool);
    scene_load_int(module, "module_state:bStaticSaturation", module_state->static_saturation, hi_bool);
    scene_load_int(module, "module_state:bStaticClut", module_state->static_clut, hi_bool);
    scene_load_int(module, "module_state:bStaticLdci", module_state->static_ldci, hi_bool);
    scene_load_int(module, "module_state:bStaticDRC", module_state->static_drc, hi_bool);
    scene_load_int(module, "module_state:bStaticNr", module_state->static_nr, hi_bool);
    scene_load_int(module, "module_state:bStaticCa", module_state->static_ca, hi_bool);
    scene_load_int(module, "module_state:bStaticAcac", module_state->static_acac, hi_bool);
    scene_load_int(module, "module_state:bStaticLocalCac", module_state->static_local_cac, hi_bool);
    scene_load_int(module, "module_state:bStaticDPC", module_state->static_dpc, hi_bool);
    scene_load_int(module, "module_state:bStaticDehaze", module_state->static_dehaze, hi_bool);
    scene_load_int(module, "module_state:bStaticShading", module_state->static_shading, hi_bool);
    scene_load_int(module, "module_state:bDynamicIsoVenc", module_state->dynamic_iso_venc, hi_bool);
    scene_load_int(module, "module_state:bDynamicMcf", module_state->dynamic_mcf, hi_bool);
    scene_load_int(module, "module_state:bDynamicHnr", module_state->dynamic_hnr, hi_bool);
    scene_load_int(module, "module_state:bDynamicFpn", module_state->dynamic_fpn, hi_bool);
    ret = scene_load_module_state_configs(module, module_state);
    ot_scenecomm_check_return(ret, ret);

    return HI_SUCCESS;
}

static hi_s32 scene_load_ae_weight_tab(const hi_char *module, ot_scene_static_statisticscfg *static_statistics)
{
    ot_scenecomm_check_pointer_return(static_statistics, HI_FAILURE);
    hi_u32 row;
    hi_char node[SCENE_INIPARAM_NODE_NAME_LEN] = {0};

    for (row = 0; row < HI_ISP_AE_ZONE_ROW; row++) {
        snprintf_truncated_s(node, SCENE_INIPARAM_NODE_NAME_LEN, "static_aeweight:ae_weight_%d", row);
        scene_load_array(module, node, static_statistics->ae_weight[row], HI_ISP_AE_ZONE_COLUMN, hi_u8);
    }

    return HI_SUCCESS;
}

static hi_s32 scene_load_static_ae(const hi_char *module, ot_scene_static_ae *static_ae)
{
    ot_scenecomm_check_pointer_return(static_ae, HI_FAILURE);
    hi_s32 ret;
    hi_s32 get_value;

    scene_load_int(module, "static_ae:ae_run_interval", static_ae->ae_run_interval, hi_u8);
    scene_load_int(module, "static_ae:ae_route_ex_valid", static_ae->ae_route_ex_valid, hi_bool);
    scene_load_int(module, "static_ae:auto_sys_gain_max", static_ae->auto_sys_gain_max, hi_u32);
    scene_load_int(module, "static_ae:auto_exp_time_max", static_ae->auto_exp_time_max, hi_u32);
    scene_load_int(module, "static_ae:auto_speed", static_ae->auto_speed, hi_u8);
    scene_load_int(module, "static_ae:auto_tolerance", static_ae->auto_tolerance, hi_u8);
    scene_load_int(module, "static_ae:auto_black_delay_frame", static_ae->auto_black_delay_frame, hi_u16);
    scene_load_int(module, "static_ae:auto_white_delay_frame", static_ae->auto_white_delay_frame, hi_u16);

    return HI_SUCCESS;
}

static hi_s32 scene_load_static_ae_route_ex(const hi_char *module, ot_scene_static_ae_route_ex *static_ae_route_ex)
{
    ot_scenecomm_check_pointer_return(static_ae_route_ex, HI_FAILURE);
    hi_s32 ret;
    hi_s32 get_value;

    scene_load_int(module, "static_aerouteex:total_num", static_ae_route_ex->total_num, hi_u32);

    scene_load_array(module, "static_aerouteex:int_time", static_ae_route_ex->int_time, static_ae_route_ex->total_num,
        hi_u32);

    scene_load_array(module, "static_aerouteex:again", static_ae_route_ex->again, static_ae_route_ex->total_num,
        hi_u32);

    scene_load_array(module, "static_aerouteex:dgain", static_ae_route_ex->dgain, static_ae_route_ex->total_num,
        hi_u32);

    scene_load_array(module, "static_aerouteex:isp_dgain", static_ae_route_ex->isp_dgain, static_ae_route_ex->total_num,
        hi_u32);

    return HI_SUCCESS;
}

static hi_s32 scene_load_static_wdr_exposure(const hi_char *module, ot_scene_static_wdr_exposure *static_wdr_exposure)
{
    ot_scenecomm_check_pointer_return(static_wdr_exposure, HI_FAILURE);
    hi_s32 ret;
    hi_s32 get_value;

    scene_load_int(module, "static_wdrexposure:exp_ratio_type", static_wdr_exposure->exp_ratio_type, hi_u8);
    scene_load_int(module, "static_wdrexposure:exp_ratio_max", static_wdr_exposure->exp_ratio_max, hi_u32);
    scene_load_int(module, "static_wdrexposure:exp_ratio_min", static_wdr_exposure->exp_ratio_min, hi_u32);

    scene_load_array(module, "static_wdrexposure:exp_ratio", static_wdr_exposure->exp_ratio, HI_ISP_EXP_RATIO_NUM,
        hi_u32);

    scene_load_int(module, "static_wdrexposure:ref_ratio_up", static_wdr_exposure->ref_ratio_up, hi_u32);
    scene_load_int(module, "static_wdrexposure:ref_ratio_dn", static_wdr_exposure->ref_ratio_dn, hi_u32);
    scene_load_int(module, "static_wdrexposure:exp_thr", static_wdr_exposure->exp_thr, hi_u32);

    scene_load_int(module, "static_wdrexposure:high_light_target", static_wdr_exposure->high_light_target, hi_u32);
    scene_load_int(module, "static_wdrexposure:exp_coef_min", static_wdr_exposure->exp_coef_min, hi_u32);

    return HI_SUCCESS;
}

static hi_s32 scene_load_static_fswdr(const hi_char *module, ot_scene_static_fswdr *static_fs_wdr)
{
    ot_scenecomm_check_pointer_return(static_fs_wdr, HI_FAILURE);
    hi_s32 ret;
    hi_s32 get_value;

    scene_load_int(module, "static_FsWdr:wdr_merge_mode", static_fs_wdr->wdr_merge_mode, hi_u16);

    return HI_SUCCESS;
}

static hi_s32 scene_load_static_awb(const hi_char *module, ot_scene_static_awb *static_awb)
{
    ot_scenecomm_check_pointer_return(static_awb, HI_FAILURE);
    hi_s32 ret;
    hi_s32 get_value;

    scene_load_array(module, "static_awb:auto_static_wb", static_awb->auto_static_wb, HI_ISP_BAYER_CHN_NUM, hi_u16);

    scene_load_array(module, "static_awb:auto_curve_para", static_awb->auto_curve_para, HI_ISP_AWB_CURVE_PARA_NUM,
        hi_s32);

    scene_load_int(module, "static_awb:op_type", static_awb->op_type, hi_u8);
    scene_load_int(module, "static_awb:manual_rgain", static_awb->manual_rgain, hi_u16);
    scene_load_int(module, "static_awb:manual_gbgain", static_awb->manual_gbgain, hi_u16);
    scene_load_int(module, "static_awb:manual_grgain", static_awb->manual_grgain, hi_u16);
    scene_load_int(module, "static_awb:manual_bgain", static_awb->manual_bgain, hi_u16);
    scene_load_int(module, "static_awb:auto_speed", static_awb->auto_speed, hi_u16);
    scene_load_int(module, "static_awb:auto_low_color_temp", static_awb->auto_low_color_temp, hi_u16);

    scene_load_array(module, "static_awb:auto_cr_max", static_awb->auto_cr_max, HI_ISP_AUTO_ISO_NUM, hi_u16);
    scene_load_array(module, "static_awb:auto_cr_min", static_awb->auto_cr_min, HI_ISP_AUTO_ISO_NUM, hi_u16);
    scene_load_array(module, "static_awb:auto_cb_max", static_awb->auto_cb_max, HI_ISP_AUTO_ISO_NUM, hi_u16);
    scene_load_array(module, "static_awb:auto_cb_min", static_awb->auto_cb_min, HI_ISP_AUTO_ISO_NUM, hi_u16);

    scene_load_int(module, "static_awb:luma_hist_enable", static_awb->luma_hist_enable, hi_u16);
    scene_load_int(module, "static_awb:awb_switch", static_awb->awb_switch, hi_u16);
    scene_load_int(module, "static_awb:black_level", static_awb->black_level, hi_u16);

    return HI_SUCCESS;
}

static hi_s32 scene_load_static_awb_ex(const hi_char *module, ot_scene_static_awb_ex *static_awb_ex)
{
    ot_scenecomm_check_pointer_return(static_awb_ex, HI_FAILURE);
    hi_s32 ret;
    hi_s32 get_value;

    scene_load_int(module, "static_awbex:bypass", static_awb_ex->bypass, hi_bool);
    scene_load_int(module, "static_awbex:tolerance", static_awb_ex->tolerance, hi_u8);
    scene_load_int(module, "static_awbex:out_thresh", static_awb_ex->out_thresh, hi_u32);
    scene_load_int(module, "static_awbex:low_stop", static_awb_ex->low_stop, hi_u16);
    scene_load_int(module, "static_awbex:high_start", static_awb_ex->high_start, hi_u16);
    scene_load_int(module, "static_awbex:high_stop", static_awb_ex->high_stop, hi_u16);
    scene_load_int(module, "static_awbex:multi_light_source_en", static_awb_ex->multi_light_source_en, hi_u16);

    scene_load_array(module, "static_awbex:multi_ctwt", static_awb_ex->multi_ctwt, HI_ISP_AWB_MULTI_CT_NUM, hi_u16);

    return HI_SUCCESS;
}

static hi_s32 scene_load_static_cmm(const hi_char *module, ot_scene_static_ccm *static_ccm)
{
    ot_scenecomm_check_pointer_return(static_ccm, HI_FAILURE);
    hi_s32 ret;
    hi_s32 get_value;
    hi_u32 i;
    hi_char node[SCENE_INIPARAM_NODE_NAME_LEN] = {0};

    scene_load_int(module, "static_ccm:ccm_op_type", static_ccm->ccm_op_type, hi_u8);

    scene_load_array(module, "static_ccm:manual_ccm", static_ccm->manual_ccm, HI_ISP_CCM_MATRIX_SIZE, hi_u16);

    scene_load_int(module, "static_ccm:auto_iso_act_en", static_ccm->auto_iso_act_en, hi_u32);
    scene_load_int(module, "static_ccm:auto_temp_act_en", static_ccm->auto_temp_act_en, hi_u32);
    scene_load_int(module, "static_ccm:total_num", static_ccm->total_num, hi_u32);

    scene_load_array(module, "static_ccm:auto_color_temp", static_ccm->auto_color_temp, static_ccm->total_num, hi_u16);

    for (i = 0; i < static_ccm->total_num; i++) {
        snprintf_truncated_s(node, SCENE_INIPARAM_NODE_NAME_LEN, "static_ccm:auto_ccm_%d", i);
        scene_load_array(module, node, static_ccm->auto_ccm[i], HI_ISP_CCM_MATRIX_SIZE, hi_u16);
    }

    return HI_SUCCESS;
}

static hi_s32 scene_load_static_saturation(const hi_char *module, ot_scene_static_saturation *static_saturation)
{
    ot_scenecomm_check_pointer_return(static_saturation, HI_FAILURE);
    scene_load_array(module, "static_saturation:auto_sat", static_saturation->auto_sat, HI_ISP_AUTO_ISO_NUM, hi_u8);
    return HI_SUCCESS;
}

static hi_s32 scene_load_static_clut(const hi_char *module, ot_scene_static_clut *static_clut)
{
    ot_scenecomm_check_pointer_return(static_clut, HI_FAILURE);
    hi_s32 ret;
    hi_s32 get_value;

    scene_load_int(module, "static_clut:enable", static_clut->enable, hi_bool);

    return HI_SUCCESS;
}

static hi_s32 scene_load_static_ldci(const hi_char *module, ot_scene_static_ldci *static_ldci)
{
    ot_scenecomm_check_pointer_return(static_ldci, HI_FAILURE);
    hi_s32 ret;
    hi_s32 get_value;

    scene_load_int(module, "static_ldci:enable", static_ldci->enable, hi_bool);
    scene_load_int(module, "static_ldci:ldci_op_type", static_ldci->ldci_op_type, hi_u8);
    scene_load_int(module, "static_ldci:gauss_lpf_sigma", static_ldci->gauss_lpf_sigma, hi_u8);
    scene_load_int(module, "static_ldci:tpr_incr_coef", static_ldci->tpr_incr_coef, hi_u8);
    scene_load_int(module, "static_ldci:tpr_decr_coef", static_ldci->tpr_decr_coef, hi_u8);

    scene_load_array(module, "static_ldci:auto_he_pos_wgt", static_ldci->auto_he_pos_wgt, HI_ISP_AUTO_ISO_NUM, hi_u8);

    scene_load_array(module, "static_ldci:auto_he_pos_sigma", static_ldci->auto_he_pos_sigma, HI_ISP_AUTO_ISO_NUM,
        hi_u8);

    scene_load_array(module, "static_ldci:auto_he_pos_mean", static_ldci->auto_he_pos_mean, HI_ISP_AUTO_ISO_NUM, hi_u8);

    scene_load_array(module, "static_ldci:auto_he_neg_wgt", static_ldci->auto_he_neg_wgt, HI_ISP_AUTO_ISO_NUM, hi_u8);

    scene_load_array(module, "static_ldci:auto_he_neg_sigma", static_ldci->auto_he_neg_sigma, HI_ISP_AUTO_ISO_NUM,
        hi_u8);

    scene_load_array(module, "static_ldci:auto_he_neg_mean", static_ldci->auto_he_neg_mean, HI_ISP_AUTO_ISO_NUM, hi_u8);

    scene_load_array(module, "static_ldci:auto_blc_ctrl", static_ldci->auto_blc_ctrl, HI_ISP_AUTO_ISO_NUM, hi_u8);

    return HI_SUCCESS;
}

static hi_s32 scene_load_static_drc(const hi_char *module, ot_scene_static_drc *static_drc)
{
    ot_scenecomm_check_pointer_return(static_drc, HI_FAILURE);
    hi_s32 ret;
    hi_s32 get_value;

    scene_load_int(module, "static_drc:enable", static_drc->enable, hi_bool);
    scene_load_int(module, "static_drc:curve_select", static_drc->curve_select, hi_isp_drc_curve_select);
    scene_load_int(module, "static_drc:op_type", static_drc->op_type, hi_op_mode);
    scene_load_int(module, "static_drc:purple_reduction_strength", static_drc->purple_reduction_strength, hi_u8);
    scene_load_int(module, "static_drc:contrast_ctrl", static_drc->contrast_ctrl, hi_u8);
    scene_load_int(module, "static_drc:blend_luma_max", static_drc->blend_luma_max, hi_u8);
    scene_load_int(module, "static_drc:blend_luma_bright_min", static_drc->blend_luma_bright_min, hi_u8);
    scene_load_int(module, "static_drc:blend_luma_bright_threshold", static_drc->blend_luma_bright_threshold, hi_u8);
    scene_load_int(module, "static_drc:blend_luma_bright_slope", static_drc->blend_luma_bright_slope, hi_u8);
    scene_load_int(module, "static_drc:blend_luma_dark_min", static_drc->blend_luma_dark_min, hi_u8);
    scene_load_int(module, "static_drc:blend_luma_dark_threshold", static_drc->blend_luma_dark_threshold, hi_u8);
    scene_load_int(module, "static_drc:blend_luma_dark_slope", static_drc->blend_luma_dark_slope, hi_u8);
    scene_load_int(module, "static_drc:blend_detail_max", static_drc->blend_detail_max, hi_u8);
    scene_load_int(module, "static_drc:blend_detail_bright_min", static_drc->blend_detail_bright_min, hi_u8);
    scene_load_int(module, "static_drc:blend_detail_bright_threshold", static_drc->blend_detail_bright_threshold,
        hi_u8);
    scene_load_int(module, "static_drc:blend_detail_bright_slope", static_drc->blend_detail_bright_slope, hi_u8);
    scene_load_int(module, "static_drc:blend_detail_dark_min", static_drc->blend_detail_dark_min, hi_u8);
    scene_load_int(module, "static_drc:blend_detail_dark_threshold", static_drc->blend_detail_dark_threshold, hi_u8);
    scene_load_int(module, "static_drc:blend_detail_dark_slope", static_drc->blend_detail_dark_slope, hi_u8);
    scene_load_int(module, "static_drc:color_correction_ctrl", static_drc->color_correction_ctrl, hi_bool);

    scene_load_array(module, "static_drc:color_correction_lut", static_drc->color_correction_lut,
        HI_ISP_DRC_CC_NODE_NUM, hi_u16);
    return HI_SUCCESS;
}

static hi_s32 scene_load_static_nr_arrays(const hi_char *module, ot_scene_static_nr *static_nr)
{
    scene_load_array(module, "static_nr:sfm0_ex_strength", static_nr->snr_cfg.sfm0_ex_strength,
        HI_ISP_AUTO_ISO_NUM, hi_u8);
    scene_load_array(module, "static_nr:sfm0_ex_detail_prot", static_nr->snr_cfg.sfm0_ex_detail_prot,
        HI_ISP_AUTO_ISO_NUM, hi_u8);
    scene_load_array(module, "static_nr:sfm0_norm_edge_strength", static_nr->snr_cfg.sfm0_norm_edge_strength,
        HI_ISP_AUTO_ISO_NUM, hi_u8);
    scene_load_array(module, "static_nr:sfm1_detail_prot", static_nr->snr_cfg.sfm1_detail_prot,
        HI_ISP_AUTO_ISO_NUM, hi_u8);
    scene_load_array(module, "static_nr:sfm1_coarse_strength", static_nr->snr_cfg.sfm1_coarse_strength,
        HI_ISP_AUTO_ISO_NUM, hi_u8);
    scene_load_array(module, "static_nr:fine_strength", static_nr->snr_cfg.fine_strength, HI_ISP_AUTO_ISO_NUM, hi_u8);
    scene_load_array(module, "static_nr:coring_wgt", static_nr->snr_cfg.coring_wgt, HI_ISP_AUTO_ISO_NUM, hi_u16);
    scene_load_array(module, "static_nr:coring_mot_thresh", static_nr->snr_cfg.coring_mot_thresh,
        HI_ISP_AUTO_ISO_NUM, hi_u8);
    scene_load_array(module, "static_nr:md_mode", static_nr->tnr_cfg.md_mode, HI_ISP_AUTO_ISO_NUM, hi_bool);
    scene_load_array(module, "static_nr:md_anti_flicker_strength", static_nr->tnr_cfg.md_anti_flicker_strength,
        HI_ISP_AUTO_ISO_NUM, hi_u8);
    scene_load_array(module, "static_nr:md_static_ratio", static_nr->tnr_cfg.md_static_ratio,
        HI_ISP_AUTO_ISO_NUM, hi_u8);
    scene_load_array(module, "static_nr:md_static_fine_strength", static_nr->tnr_cfg.md_static_fine_strength,
        HI_ISP_AUTO_ISO_NUM, hi_u8);
    scene_load_array(module, "static_nr:tnr_tss", static_nr->tnr_cfg.tss, HI_ISP_AUTO_ISO_NUM, hi_u8);
    scene_load_array(module, "static_nr:tnr_tfr", static_nr->tnr_cfg.tfr, HI_ISP_AUTO_ISO_NUM, hi_u8);
    scene_load_array(module, "static_nr:tnr_tfs", static_nr->tnr_cfg.tfs, HI_ISP_AUTO_ISO_NUM, hi_u8);
    scene_load_array(module, "static_nr:user_define_md", static_nr->tnr_cfg.user_define_md, HI_ISP_AUTO_ISO_NUM,
        hi_bool);
    scene_load_array(module, "static_nr:user_define_slope", static_nr->tnr_cfg.user_define_slope,
        HI_ISP_AUTO_ISO_NUM, hi_u16);
    scene_load_array(module, "static_nr:user_define_dark_thresh", static_nr->tnr_cfg.user_define_dark_thresh,
        HI_ISP_AUTO_ISO_NUM, hi_u16);
    scene_load_array(module, "static_nr:user_define_color_thresh", static_nr->tnr_cfg.user_define_color_thresh,
        HI_ISP_AUTO_ISO_NUM, hi_u8);
    scene_load_array(module, "static_nr:sfr_r", static_nr->tnr_cfg.sfr_r, HI_ISP_AUTO_ISO_NUM, hi_u8);
    scene_load_array(module, "static_nr:sfr_g", static_nr->tnr_cfg.sfr_g, HI_ISP_AUTO_ISO_NUM, hi_u8);
    scene_load_array(module, "static_nr:sfr_b", static_nr->tnr_cfg.sfr_b, HI_ISP_AUTO_ISO_NUM, hi_u8);

    return HI_SUCCESS;
}
static hi_s32 scene_load_static_nr(const hi_char *module, ot_scene_static_nr *static_nr)
{
    ot_scenecomm_check_pointer_return(static_nr, HI_FAILURE);
    hi_s32 ret;
    hi_s32 get_value;
    hi_u32 i;
    hi_char node[SCENE_INIPARAM_NODE_NAME_LEN] = {0};

    scene_load_int(module, "static_nr:enable", static_nr->enable, hi_bool);
    scene_load_int(module, "static_nr:op_type", static_nr->op_type, hi_op_mode);
    scene_load_int(module, "static_nr:tnr_enable", static_nr->tnr_enable, hi_bool);
    scene_load_int(module, "static_nr:lsc_nr_enable", static_nr->lsc_nr_enable, hi_bool);
    scene_load_int(module, "static_nr:lsc_ratio1", static_nr->lsc_ratio1, hi_bool);
    scene_load_int(module, "static_nr:lsc_ratio2", static_nr->lsc_ratio2, hi_bool);

    scene_load_array(module, "static_nr:coring_ratio", static_nr->coring_ratio, HI_ISP_BAYERNR_LUT_LENGTH, hi_u16);
    scene_load_array(module, "static_nr:sfm_threshold", static_nr->snr_cfg.sfm_threshold, HI_ISP_AUTO_ISO_NUM, hi_u8);
    scene_load_array(module, "static_nr:sfm0_mode", static_nr->snr_cfg.sfm0_mode, HI_ISP_AUTO_ISO_NUM,
        hi_isp_bnr_sfm0_mode);

    for (i = 0; i < HI_ISP_BAYER_CHN_NUM; i++) {
        snprintf_truncated_s(node, SCENE_INIPARAM_NODE_NAME_LEN, "static_nr:sfm0_coarse_strength_%d", i);
        scene_load_array(module, node, static_nr->snr_cfg.sfm0_coarse_strength[i], HI_ISP_AUTO_ISO_NUM, hi_u16);
    }

    ret = scene_load_static_nr_arrays(module, static_nr);
    ot_scenecomm_check_return(ret, ret);

    return HI_SUCCESS;
}

static hi_s32 scene_load_static_ca(const hi_char *module, ot_scene_static_ca *static_ca)
{
    ot_scenecomm_check_pointer_return(static_ca, HI_FAILURE);
    hi_s32 ret;
    hi_s32 get_value;

    scene_load_int(module, "static_ca:enable", static_ca->enable, hi_bool);

    scene_load_array(module, "static_ca:iso_ratio", static_ca->iso_ratio, ISP_AUTO_ISO_CA_NUM, hi_u16);
    scene_load_array(module, "static_ca:y_ratio_lut", static_ca->y_ratio_lut, HI_ISP_CA_YRATIO_LUT_LENGTH, hi_u32);

    return HI_SUCCESS;
}

static hi_s32 scene_load_static_venc(const hi_char *module, ot_scene_static_venc_attr *static_venc)
{
    ot_scenecomm_check_pointer_return(static_venc, HI_FAILURE);
    hi_s32 ret;
    hi_s32 get_value;
    scene_load_int(module, "static_venc:deblur_en", static_venc->deblur_en, hi_bool);
    scene_load_int(module, "static_venc:deblur_adaptive_en", static_venc->deblur_adaptive_en, hi_bool);
    scene_load_int(module, "static_venc:avbr_change_pos", static_venc->venc_param_h265_avbr.change_pos, hi_s32);
    scene_load_int(module, "static_venc:avbr_min_iprop", static_venc->venc_param_h265_avbr.min_iprop, hi_u32);
    scene_load_int(module, "static_venc:avbr_max_iprop", static_venc->venc_param_h265_avbr.max_iprop, hi_u32);
    scene_load_int(module, "static_venc:avbr_max_reencode_times", static_venc->venc_param_h265_avbr.max_reencode_times,
        hi_s32);
    scene_load_int(module, "static_venc:avbr_min_still_percent", static_venc->venc_param_h265_avbr.min_still_percent,
        hi_s32);
    scene_load_int(module, "static_venc:avbr_max_still_qp", static_venc->venc_param_h265_avbr.max_still_qp, hi_u32);
    scene_load_int(module, "static_venc:avbr_min_still_psnr", static_venc->venc_param_h265_avbr.min_still_psnr, hi_u32);
    scene_load_int(module, "static_venc:avbr_max_qp", static_venc->venc_param_h265_avbr.max_qp, hi_u32);
    scene_load_int(module, "static_venc:avbr_min_qp", static_venc->venc_param_h265_avbr.min_qp, hi_u32);
    scene_load_int(module, "static_venc:avbr_max_iqp", static_venc->venc_param_h265_avbr.max_iqp, hi_u32);
    scene_load_int(module, "static_venc:avbr_min_iqp", static_venc->venc_param_h265_avbr.min_iqp, hi_u32);
    scene_load_int(module, "static_venc:avbr_min_qp_delta", static_venc->venc_param_h265_avbr.min_qp_delta, hi_u32);
    scene_load_int(module, "static_venc:avbr_motion_sensitivity", static_venc->venc_param_h265_avbr.motion_sensitivity,
        hi_u32);
    scene_load_int(module, "static_venc:avbr_qp_map_en", static_venc->venc_param_h265_avbr.qp_map_en, hi_bool);
    scene_load_int(module, "static_venc:avbr_qp_map_mode", static_venc->venc_param_h265_avbr.qp_map_mode,
        hi_venc_rc_qpmap_mode);
    scene_load_int(module, "static_venc:cvbr_min_iprop", static_venc->venc_param_h265_cvbr.min_iprop, hi_u32);
    scene_load_int(module, "static_venc:cvbr_max_iprop", static_venc->venc_param_h265_cvbr.max_iprop, hi_u32);
    scene_load_int(module, "static_venc:cvbr_max_reencode_times", static_venc->venc_param_h265_cvbr.max_reencode_times,
        hi_s32);
    scene_load_int(module, "static_venc:cvbr_qp_map_en", static_venc->venc_param_h265_cvbr.qp_map_en, hi_bool);
    scene_load_int(module, "static_venc:cvbr_qp_map_mode", static_venc->venc_param_h265_cvbr.qp_map_mode,
        hi_venc_rc_qpmap_mode);
    scene_load_int(module, "static_venc:cvbr_max_qp", static_venc->venc_param_h265_cvbr.max_qp, hi_u32);
    scene_load_int(module, "static_venc:cvbr_min_qp", static_venc->venc_param_h265_cvbr.min_qp, hi_u32);
    scene_load_int(module, "static_venc:cvbr_max_iqp", static_venc->venc_param_h265_cvbr.max_iqp, hi_u32);
    scene_load_int(module, "static_venc:cvbr_min_iqp", static_venc->venc_param_h265_cvbr.min_iqp, hi_u32);
    scene_load_int(module, "static_venc:cvbr_min_qp_delta", static_venc->venc_param_h265_cvbr.min_qp_delta, hi_u32);
    scene_load_int(module, "static_venc:cvbr_max_qp_delta", static_venc->venc_param_h265_cvbr.max_qp_delta, hi_u32);
    scene_load_int(module, "static_venc:cvbr_extra_bit_percent", static_venc->venc_param_h265_cvbr.extra_bit_percent,
        hi_u32);
    scene_load_int(module, "static_venc:cvbr_long_term_static_time_unit",
        static_venc->venc_param_h265_cvbr.long_term_static_time_unit, hi_u32);

    return HI_SUCCESS;
}

static hi_s32 scene_load_static_acac(const hi_char *module, ot_scene_static_acac *static_acac)
{
    ot_scenecomm_check_pointer_return(static_acac, HI_FAILURE);
    hi_s32 ret;
    hi_s32 get_value;

    scene_load_int(module, "static_acac:enable", static_acac->enable, hi_bool);
    scene_load_int(module, "static_acac:op_type", static_acac->op_type, hi_op_mode);
    scene_load_int(module, "static_acac:detect_mode", static_acac->detect_mode, hi_bool);

    scene_load_array(module, "static_acac:edge_threshold_0", static_acac->acac_auto_cfg.edge_threshold[0],
        HI_ISP_AUTO_ISO_NUM, hi_u16);
    scene_load_array(module, "static_acac:edge_threshold_1", static_acac->acac_auto_cfg.edge_threshold[1],
        HI_ISP_AUTO_ISO_NUM, hi_u16);
    scene_load_array(module, "static_acac:edge_gain", static_acac->acac_auto_cfg.edge_gain,
        HI_ISP_AUTO_ISO_NUM, hi_u16);
    scene_load_array(module, "static_acac:purple_upper_limit", static_acac->acac_auto_cfg.purple_upper_limit,
        HI_ISP_AUTO_ISO_NUM, hi_s16);
    scene_load_array(module, "static_acac:purple_lower_limit", static_acac->acac_auto_cfg.purple_lower_limit,
        HI_ISP_AUTO_ISO_NUM, hi_s16);
    scene_load_array(module, "static_acac:purple_sat_threshold", static_acac->acac_auto_cfg.purple_sat_threshold,
        HI_ISP_AUTO_ISO_NUM, hi_u16);
    scene_load_array(module, "static_acac:purple_alpha", static_acac->acac_auto_cfg.purple_alpha,
        HI_ISP_AUTO_ISO_NUM, hi_u16);
    scene_load_array(module, "static_acac:edge_alpha", static_acac->acac_auto_cfg.edge_alpha,
        HI_ISP_AUTO_ISO_NUM, hi_u16);
    scene_load_array(module, "static_acac:fcc_y_strength", static_acac->acac_auto_cfg.fcc_y_strength,
        HI_ISP_AUTO_ISO_NUM, hi_u16);
    scene_load_array(module, "static_acac:fcc_rb_strength", static_acac->acac_auto_cfg.fcc_rb_strength,
        HI_ISP_AUTO_ISO_NUM, hi_u16);

    return HI_SUCCESS;
}

static hi_s32 scene_load_static_local_cac(const hi_char *module, ot_scene_static_local_cac *static_local_cac)
{
    ot_scenecomm_check_pointer_return(static_local_cac, HI_FAILURE);
    hi_s32 ret;
    hi_s32 get_value;

    scene_load_int(module, "static_localcac:enable", static_local_cac->enable, hi_bool);
    scene_load_int(module, "static_localcac:op_type", static_local_cac->op_type, hi_op_mode);
    scene_load_int(module, "static_localcac:purple_detect_range", static_local_cac->purple_detect_range, hi_u16);
    scene_load_int(module, "static_localcac:var_threshold", static_local_cac->var_threshold, hi_u16);

    scene_load_array(module, "static_localcac:r_detect_threshold", static_local_cac->r_detect_threshold,
        HI_ISP_LCAC_DET_NUM, hi_u16);
    scene_load_array(module, "static_localcac:g_detect_threshold", static_local_cac->g_detect_threshold,
        HI_ISP_LCAC_DET_NUM, hi_u16);
    scene_load_array(module, "static_localcac:b_detect_threshold", static_local_cac->b_detect_threshold,
        HI_ISP_LCAC_DET_NUM, hi_u16);
    scene_load_array(module, "static_localcac:l_detect_threshold", static_local_cac->l_detect_threshold,
        HI_ISP_LCAC_DET_NUM, hi_u16);
    scene_load_array(module, "static_localcac:cb_cr_ratio", static_local_cac->cb_cr_ratio, HI_ISP_LCAC_DET_NUM, hi_s16);
    scene_load_array(module, "static_localcac:depur_cr_strength",
        static_local_cac->depurplestr_auto_cfg.depur_cr_strength, HI_ISP_LCAC_EXP_RATIO_NUM, hi_u8);
    scene_load_array(module, "static_localcac:depur_cb_strength",
        static_local_cac->depurplestr_auto_cfg.depur_cb_strength, HI_ISP_LCAC_EXP_RATIO_NUM, hi_u8);

    return HI_SUCCESS;
}

static hi_s32 scene_load_static_dpc(const hi_char *module, ot_scene_static_dpc *static_dpc)
{
    ot_scenecomm_check_pointer_return(static_dpc, HI_FAILURE);
    hi_s32 ret;
    hi_s32 get_value;

    scene_load_int(module, "static_dpc:enable", static_dpc->enable, hi_bool);

    scene_load_array(module, "static_dpc:strength", static_dpc->strength, HI_ISP_AUTO_ISO_NUM, hi_u16);
    scene_load_array(module, "static_dpc:blend_ratio", static_dpc->blend_ratio, HI_ISP_AUTO_ISO_NUM, hi_u16);

    return HI_SUCCESS;
}

static hi_s32 scene_load_static_dehaze(const hi_char *module, ot_scene_static_dehaze *static_de_haze)
{
    ot_scenecomm_check_pointer_return(static_de_haze, HI_FAILURE);
    hi_s32 ret;
    hi_s32 get_value;

    scene_load_int(module, "static_dehaze:enable", static_de_haze->enable, hi_bool);
    scene_load_int(module, "static_dehaze:dehaze_op_type", static_de_haze->dehaze_op_type, hi_u8);
    scene_load_int(module, "static_dehaze:user_lut_enable", static_de_haze->user_lut_enable, hi_bool);
    scene_load_int(module, "static_dehaze:tmprflt_incr_coef", static_de_haze->tmprflt_incr_coef, hi_bool);
    scene_load_int(module, "static_dehaze:tmprflt_decr_coef", static_de_haze->tmprflt_decr_coef, hi_bool);

    scene_load_array(module, "static_dehaze:dehaze_lut", static_de_haze->dehaze_lut, HI_ISP_DEHAZE_LUT_SIZE, hi_u8);

    return HI_SUCCESS;
}

static hi_s32 scene_load_static_shading(const hi_char *module, ot_scene_static_shading *static_shading)
{
    ot_scenecomm_check_pointer_return(static_shading, HI_FAILURE);
    hi_s32 ret;
    hi_s32 get_value;

    scene_load_int(module, "static_shading:enable", static_shading->enable, hi_bool);

    return HI_SUCCESS;
}

static hi_s32 scene_load_static_csc(const hi_char *module, ot_scene_static_csc *static_csc)
{
    ot_scenecomm_check_pointer_return(static_csc, HI_FAILURE);
    hi_s32 ret;
    hi_s32 get_value;

    scene_load_int(module, "static_csc:enable", static_csc->enable, hi_bool);
    scene_load_int(module, "static_csc:hue", static_csc->hue, hi_u8);
    scene_load_int(module, "static_csc:luma", static_csc->luma, hi_u8);
    scene_load_int(module, "static_csc:contrast", static_csc->contrast, hi_u8);
    scene_load_int(module, "static_csc:saturation", static_csc->saturation, hi_u8);
    scene_load_int(module, "static_csc:color_gamut", static_csc->color_gamut, hi_color_gamut);

    return HI_SUCCESS;
}

static hi_s32 scene_load_static_cross_talk(const hi_char *module, ot_scene_static_crosstalk *static_cross_talk)
{
    ot_scenecomm_check_pointer_return(static_cross_talk, HI_FAILURE);
    hi_s32 ret;
    hi_s32 get_value;

    scene_load_int(module, "static_crosstalk:enable", static_cross_talk->enable, hi_bool);

    return HI_SUCCESS;
}

static hi_s32 scene_load_static_demosaic(const hi_char *module, ot_scene_static_demosaic *static_demosaic)
{
    ot_scenecomm_check_pointer_return(static_demosaic, HI_FAILURE);
    hi_s32 ret;
    hi_s32 get_value;

    scene_load_int(module, "static_dm:enable", static_demosaic->enable, hi_bool);
    scene_load_int(module, "static_dm:op_type", static_demosaic->op_type, hi_op_mode);

    scene_load_array(module, "static_dm:nddm_strength", static_demosaic->dm_auto_cfg.nddm_strength,
        HI_ISP_AUTO_ISO_NUM, hi_u8);
    scene_load_array(module, "static_dm:nddm_mf_detail_strength", static_demosaic->dm_auto_cfg.nddm_mf_detail_strength,
        HI_ISP_AUTO_ISO_NUM, hi_u8);
    scene_load_array(module, "static_dm:nddm_hf_detail_strength", static_demosaic->dm_auto_cfg.nddm_hf_detail_strength,
        HI_ISP_AUTO_ISO_NUM, hi_u8);
    scene_load_array(module, "static_dm:detail_smooth_range", static_demosaic->dm_auto_cfg.detail_smooth_range,
        HI_ISP_AUTO_ISO_NUM, hi_u8);
    scene_load_array(module, "static_dm:color_noise_f_threshold", static_demosaic->dm_auto_cfg.color_noise_f_threshold,
        HI_ISP_AUTO_ISO_NUM, hi_u8);
    scene_load_array(module, "static_dm:color_noise_f_strength", static_demosaic->dm_auto_cfg.color_noise_f_strength,
        HI_ISP_AUTO_ISO_NUM, hi_u8);
    scene_load_array(module, "static_dm:color_noise_y_threshold", static_demosaic->dm_auto_cfg.color_noise_y_threshold,
        HI_ISP_AUTO_ISO_NUM, hi_u8);
    scene_load_array(module, "static_dm:color_noise_y_strength", static_demosaic->dm_auto_cfg.color_noise_y_strength,
        HI_ISP_AUTO_ISO_NUM, hi_u8);

    return HI_SUCCESS;
}

static hi_s32 scene_load_static_bayershp(const hi_char *module, ot_scene_static_bayershp *static_bayershp)
{
    ot_scenecomm_check_pointer_return(static_bayershp, HI_FAILURE);
    hi_s32 ret;
    hi_u32 i;
    hi_s32 get_value;
    hi_char node[SCENE_INIPARAM_NODE_NAME_LEN] = { 0 };

    scene_load_int(module, "static_bayershp:enable", static_bayershp->enable, hi_bool);
    scene_load_int(module, "static_bayershp:op_type", static_bayershp->op_type, hi_op_mode);

    for (i = 0; i < HI_ISP_SHARPEN_GAIN_NUM; i++) {
        snprintf_truncated_s(node, SCENE_INIPARAM_NODE_NAME_LEN, "static_bayershp:luma_wgt_%d", i);
        scene_load_array(module, node, static_bayershp->bayershp_auto_attr.luma_wgt[i], HI_ISP_AUTO_ISO_NUM, hi_u8);
    }

    for (i = 0; i < HI_ISP_SHARPEN_GAIN_NUM; i++) {
        snprintf_truncated_s(node, SCENE_INIPARAM_NODE_NAME_LEN, "static_bayershp:edge_mf_strength_%d", i);
        scene_load_array(module, node, static_bayershp->bayershp_auto_attr.edge_mf_strength[i],
            HI_ISP_AUTO_ISO_NUM, hi_u16);
    }

    for (i = 0; i < HI_ISP_SHARPEN_GAIN_NUM; i++) {
        snprintf_truncated_s(node, SCENE_INIPARAM_NODE_NAME_LEN, "static_bayershp:texture_mf_strength_%d", i);
        scene_load_array(module, node, static_bayershp->bayershp_auto_attr.texture_mf_strength[i],
            HI_ISP_AUTO_ISO_NUM, hi_u16);
    }

    for (i = 0; i < HI_ISP_SHARPEN_GAIN_NUM; i++) {
        snprintf_truncated_s(node, SCENE_INIPARAM_NODE_NAME_LEN, "static_bayershp:edge_hf_strength_%d", i);
        scene_load_array(module, node, static_bayershp->bayershp_auto_attr.edge_hf_strength[i],
            HI_ISP_AUTO_ISO_NUM, hi_u16);
    }

    for (i = 0; i < HI_ISP_SHARPEN_GAIN_NUM; i++) {
        snprintf_truncated_s(node, SCENE_INIPARAM_NODE_NAME_LEN, "static_bayershp:texture_hf_strength_%d", i);
        scene_load_array(module, node, static_bayershp->bayershp_auto_attr.texture_hf_strength[i],
            HI_ISP_AUTO_ISO_NUM, hi_u16);
    }

    scene_load_array(module, "static_bayershp:edge_filt_strength",
        static_bayershp->bayershp_auto_attr.edge_filt_strength, HI_ISP_AUTO_ISO_NUM, hi_u8);
    scene_load_array(module, "static_bayershp:texture_max_gain",
        static_bayershp->bayershp_auto_attr.texture_max_gain, HI_ISP_AUTO_ISO_NUM, hi_u16);
    scene_load_array(module, "static_bayershp:edge_max_gain", static_bayershp->bayershp_auto_attr.edge_max_gain,
        HI_ISP_AUTO_ISO_NUM, hi_u16);
    scene_load_array(module, "static_bayershp:overshoot", static_bayershp->bayershp_auto_attr.overshoot,
        HI_ISP_AUTO_ISO_NUM, hi_u16);
    scene_load_array(module, "static_bayershp:undershoot", static_bayershp->bayershp_auto_attr.undershoot,
        HI_ISP_AUTO_ISO_NUM, hi_u16);
    scene_load_array(module, "static_bayershp:g_chn_gain", static_bayershp->bayershp_auto_attr.g_chn_gain,
        HI_ISP_AUTO_ISO_NUM, hi_u16);

    return HI_SUCCESS;
}


static hi_s32 scene_load_static_sharpen_arrays(const hi_char *module, ot_scene_static_sharpen *static_sharpen)
{
    scene_load_array(module, "static_sharpen:texture_freq", static_sharpen->sharpen_auto_cfg.texture_freq,
        HI_ISP_AUTO_ISO_NUM, hi_u16);
    scene_load_array(module, "static_sharpen:edge_freq", static_sharpen->sharpen_auto_cfg.edge_freq,
        HI_ISP_AUTO_ISO_NUM, hi_u16);
    scene_load_array(module, "static_sharpen:over_shoot", static_sharpen->sharpen_auto_cfg.over_shoot,
        HI_ISP_AUTO_ISO_NUM, hi_u8);
    scene_load_array(module, "static_sharpen:under_shoot", static_sharpen->sharpen_auto_cfg.under_shoot,
        HI_ISP_AUTO_ISO_NUM, hi_u8);
    scene_load_array(module, "static_sharpen:motion_texture_freq",
        static_sharpen->sharpen_auto_cfg.motion_texture_freq, HI_ISP_AUTO_ISO_NUM, hi_u16);
    scene_load_array(module, "static_sharpen:motion_edge_freq", static_sharpen->sharpen_auto_cfg.motion_edge_freq,
        HI_ISP_AUTO_ISO_NUM, hi_u16);
    scene_load_array(module, "static_sharpen:motion_over_shoot", static_sharpen->sharpen_auto_cfg.motion_over_shoot,
        HI_ISP_AUTO_ISO_NUM, hi_u8);
    scene_load_array(module, "static_sharpen:motion_under_shoot", static_sharpen->sharpen_auto_cfg.motion_under_shoot,
        HI_ISP_AUTO_ISO_NUM, hi_u8);
    scene_load_array(module, "static_sharpen:shoot_sup_strength", static_sharpen->sharpen_auto_cfg.shoot_sup_strength,
        HI_ISP_AUTO_ISO_NUM, hi_u8);
    scene_load_array(module, "static_sharpen:shoot_sup_adj", static_sharpen->sharpen_auto_cfg.shoot_sup_adj,
        HI_ISP_AUTO_ISO_NUM, hi_u8);
    scene_load_array(module, "static_sharpen:detail_ctrl", static_sharpen->sharpen_auto_cfg.detail_ctrl,
        HI_ISP_AUTO_ISO_NUM, hi_u8);
    scene_load_array(module, "static_sharpen:detail_ctrl_threshold",
        static_sharpen->sharpen_auto_cfg.detail_ctrl_threshold, HI_ISP_AUTO_ISO_NUM, hi_u8);
    scene_load_array(module, "static_sharpen:edge_filt_strength", static_sharpen->sharpen_auto_cfg.edge_filt_strength,
        HI_ISP_AUTO_ISO_NUM, hi_u8);
    scene_load_array(module, "static_sharpen:edge_filt_max_cap", static_sharpen->sharpen_auto_cfg.edge_filt_max_cap,
        HI_ISP_AUTO_ISO_NUM, hi_u8);
    scene_load_array(module, "static_sharpen:r_gain", static_sharpen->sharpen_auto_cfg.r_gain,
        HI_ISP_AUTO_ISO_NUM, hi_u8);
    scene_load_array(module, "static_sharpen:g_gain", static_sharpen->sharpen_auto_cfg.g_gain,
        HI_ISP_AUTO_ISO_NUM, hi_u8);
    scene_load_array(module, "static_sharpen:b_gain", static_sharpen->sharpen_auto_cfg.b_gain,
        HI_ISP_AUTO_ISO_NUM, hi_u8);
    scene_load_array(module, "static_sharpen:skin_gain", static_sharpen->sharpen_auto_cfg.skin_gain,
        HI_ISP_AUTO_ISO_NUM, hi_u8);
    scene_load_array(module, "static_sharpen:max_sharp_gain", static_sharpen->sharpen_auto_cfg.max_sharp_gain,
        HI_ISP_AUTO_ISO_NUM, hi_u16);
    scene_load_array(module, "static_sharpen:weak_detail_gain", static_sharpen->sharpen_auto_cfg.weak_detail_gain,
        HI_ISP_AUTO_ISO_NUM, hi_u8);

    return HI_SUCCESS;
}

static hi_s32 scene_load_static_sharpen(const hi_char *module, ot_scene_static_sharpen *static_sharpen)
{
    ot_scenecomm_check_pointer_return(static_sharpen, HI_FAILURE);
    hi_s32 ret;
    hi_u32 i;
    hi_s32 get_value;
    hi_char node[SCENE_INIPARAM_NODE_NAME_LEN] = { 0 };

    scene_load_int(module, "static_sharpen:enable", static_sharpen->enable, hi_bool);
    scene_load_int(module, "static_sharpen:motion_en", static_sharpen->motion_en, hi_bool);
    scene_load_int(module, "static_sharpen:motion_threshold0", static_sharpen->motion_threshold0, hi_u8);
    scene_load_int(module, "static_sharpen:motion_threshold1", static_sharpen->motion_threshold1, hi_u8);
    scene_load_int(module, "static_sharpen:motion_gain0", static_sharpen->motion_gain0, hi_u16);
    scene_load_int(module, "static_sharpen:motion_gain1", static_sharpen->motion_gain1, hi_u16);
    scene_load_int(module, "static_sharpen:skin_umin", static_sharpen->skin_umin, hi_u8);
    scene_load_int(module, "static_sharpen:skin_vmin", static_sharpen->skin_vmin, hi_u8);
    scene_load_int(module, "static_sharpen:skin_umax", static_sharpen->skin_umax, hi_u8);
    scene_load_int(module, "static_sharpen:skin_vmax", static_sharpen->skin_vmax, hi_u8);

    for (i = 0; i < HI_ISP_SHARPEN_GAIN_NUM; i++) {
        snprintf_truncated_s(node, SCENE_INIPARAM_NODE_NAME_LEN, "static_sharpen:luma_wgt_%d", i);
        scene_load_array(module, node, static_sharpen->sharpen_auto_cfg.luma_wgt[i], HI_ISP_AUTO_ISO_NUM, hi_u8);
    }

    for (i = 0; i < HI_ISP_SHARPEN_GAIN_NUM; i++) {
        snprintf_truncated_s(node, SCENE_INIPARAM_NODE_NAME_LEN, "static_sharpen:texture_strength_%d", i);
        scene_load_array(module, node, static_sharpen->sharpen_auto_cfg.texture_strength[i],
            HI_ISP_AUTO_ISO_NUM, hi_u16);
    }

    for (i = 0; i < HI_ISP_SHARPEN_GAIN_NUM; i++) {
        snprintf_truncated_s(node, SCENE_INIPARAM_NODE_NAME_LEN, "static_sharpen:edge_strength_%d", i);
        scene_load_array(module, node, static_sharpen->sharpen_auto_cfg.edge_strength[i], HI_ISP_AUTO_ISO_NUM, hi_u16);
    }

    for (i = 0; i < HI_ISP_SHARPEN_GAIN_NUM; i++) {
        snprintf_truncated_s(node, SCENE_INIPARAM_NODE_NAME_LEN, "static_sharpen:motion_texture_strength_%d", i);
        scene_load_array(module, node, static_sharpen->sharpen_auto_cfg.motion_texture_strength[i],
            HI_ISP_AUTO_ISO_NUM, hi_u16);
    }

    for (i = 0; i < HI_ISP_SHARPEN_GAIN_NUM; i++) {
        snprintf_truncated_s(node, SCENE_INIPARAM_NODE_NAME_LEN, "static_sharpen:motion_edge_strength_%d", i);
        scene_load_array(module, node, static_sharpen->sharpen_auto_cfg.motion_edge_strength[i],
            HI_ISP_AUTO_ISO_NUM, hi_u16);
    }

    ret = scene_load_static_sharpen_arrays(module, static_sharpen);
    ot_scenecomm_check_return(ret, ret);

    return HI_SUCCESS;
}

static hi_s32 scene_load_static_pregamma(const hi_char *module, ot_scene_static_pregamma *static_pregamma)
{
    ot_scenecomm_check_pointer_return(static_pregamma, HI_FAILURE);
    hi_s32 ret;
    hi_s32 get_value;

    scene_load_int(module, "static_pregamma:enable", static_pregamma->enable, hi_bool);

    scene_load_array(module, "static_pregamma:table", static_pregamma->table, HI_ISP_PREGAMMA_NODE_NUM, hi_u32);

    return HI_SUCCESS;
}

static hi_s32 scene_load_static_blc(const hi_char *module, ot_scene_static_blc *static_blc)
{
    ot_scenecomm_check_pointer_return(static_blc, HI_FAILURE);
    hi_s32 ret;
    hi_s32 get_value;

    scene_load_int(module, "static_blc:enable", static_blc->enable, hi_bool);
    scene_load_int(module, "static_blc:user_offset", static_blc->user_offset, hi_u16);

    return HI_SUCCESS;
}


static hi_s32 scene_load_dynamic_ae(const hi_char *module, ot_scene_dynamic_ae *dynamic_ae)
{
    ot_scenecomm_check_pointer_return(dynamic_ae, HI_FAILURE);
    hi_s32 ret;
    hi_s32 get_value;

    scene_load_int(module, "dynamic_ae:ae_exposure_cnt", dynamic_ae->ae_exposure_cnt, hi_u8);

    scene_load_array(module, "dynamic_ae:exp_ltoh_thresh", dynamic_ae->exp_ltoh_thresh,
        dynamic_ae->ae_exposure_cnt, hi_u64);
    scene_load_array(module, "dynamic_ae:exp_htol_thresh", dynamic_ae->exp_htol_thresh,
        dynamic_ae->ae_exposure_cnt, hi_u64);
    scene_load_array(module, "dynamic_ae:auto_compensation", dynamic_ae->auto_compensation,
        dynamic_ae->ae_exposure_cnt, hi_u8);
    scene_load_array(module, "dynamic_ae:auto_max_hist_offset", dynamic_ae->auto_max_hist_offset,
        dynamic_ae->ae_exposure_cnt, hi_u8);
    scene_load_array(module, "dynamic_ae:auto_exp_ratio_max", dynamic_ae->auto_exp_ratio_max,
        dynamic_ae->ae_exposure_cnt, hi_u32);
    scene_load_array(module, "dynamic_ae:auto_exp_ratio_min", dynamic_ae->auto_exp_ratio_min,
        dynamic_ae->ae_exposure_cnt, hi_u32);

    scene_load_int(module, "dynamic_ae:wdr_ratio_threshold", dynamic_ae->wdr_ratio_threshold, hi_u32);
    scene_load_int(module, "dynamic_ae:l_advance_ae", dynamic_ae->l_advance_ae, hi_bool);
    scene_load_int(module, "dynamic_ae:h_advance_ae", dynamic_ae->h_advance_ae, hi_bool);

    return HI_SUCCESS;
}

static hi_s32 scene_load_dynamic_dehaze(const hi_char *module, ot_scene_dynamic_dehaze *dynamic_dehaze)
{
    ot_scenecomm_check_pointer_return(dynamic_dehaze, HI_FAILURE);
    hi_s32 ret;
    hi_s32 get_value;

    scene_load_int(module, "dynamic_dehaze:exp_thresh_cnt", dynamic_dehaze->exp_thresh_cnt, hi_u32);

    scene_load_array(module, "dynamic_dehaze:exp_thresh_ltoh", dynamic_dehaze->exp_thresh_ltoh,
        dynamic_dehaze->exp_thresh_cnt, hi_u64);
    scene_load_array(module, "dynamic_dehaze:manual_strength", dynamic_dehaze->manual_strength,
        dynamic_dehaze->exp_thresh_cnt, hi_u8);

    scene_load_int(module, "dynamic_dehaze:wdr_ratio_threshold", dynamic_dehaze->wdr_ratio_threshold, hi_u32);
    scene_load_array(module, "dynamic_dehaze:manual_strengther", dynamic_dehaze->manual_strengther,
        dynamic_dehaze->exp_thresh_cnt, hi_u8);

    return HI_SUCCESS;
}

static hi_s32 scene_load_dynamic_drc_arrays(const hi_char *module, ot_scene_dynamic_drc *dynamic_drc)
{
    hi_u32 i;
    hi_char node[SCENE_INIPARAM_NODE_NAME_LEN] = { 0 };

    for (i = 0; i < HI_ISP_DRC_LMIX_NODE_NUM; i++) {
        snprintf_truncated_s(node, SCENE_INIPARAM_NODE_NAME_LEN, "dynamic_drc:local_mixing_bright_%d", i);
        scene_load_array(module, node, dynamic_drc->local_mixing_bright[i], dynamic_drc->iso_count, hi_u8);

        snprintf_truncated_s(node, SCENE_INIPARAM_NODE_NAME_LEN, "dynamic_drc:local_mixing_dark_%d", i);
        scene_load_array(module, node, dynamic_drc->local_mixing_dark[i], dynamic_drc->iso_count, hi_u8);

        snprintf_truncated_s(node, SCENE_INIPARAM_NODE_NAME_LEN, "dynamic_drc:local_mixing_bright_x_%d", i);
        scene_load_array(module, node, dynamic_drc->local_mixing_bright_x[i], dynamic_drc->iso_count, hi_u8);

        snprintf_truncated_s(node, SCENE_INIPARAM_NODE_NAME_LEN, "dynamic_drc:local_mixing_dark_x_%d", i);
        scene_load_array(module, node, dynamic_drc->local_mixing_dark_x[i], dynamic_drc->iso_count, hi_u8);
    }

    for (i = 0; i < HI_ISP_DRC_FLTX_NODE_NUM; i++) {
        snprintf_truncated_s(node, SCENE_INIPARAM_NODE_NAME_LEN, "dynamic_drc:filter_coef_x_%d", i);
        scene_load_array(module, node, dynamic_drc->filter_coef_x[i], dynamic_drc->ratio_count, hi_u8);

        snprintf_truncated_s(node, SCENE_INIPARAM_NODE_NAME_LEN, "dynamic_drc:filter_low_threshold_x_%d", i);
        scene_load_array(module, node, dynamic_drc->filter_low_threshold_x[i], dynamic_drc->ratio_count, hi_u8);

        snprintf_truncated_s(node, SCENE_INIPARAM_NODE_NAME_LEN, "dynamic_drc:filter_high_threshold_x_%d", i);
        scene_load_array(module, node, dynamic_drc->filter_high_threshold_x[i], dynamic_drc->ratio_count, hi_u8);
    }

    scene_load_array(module, "dynamic_drc:spatial_filter_coef", dynamic_drc->spatial_filter_coef,
        dynamic_drc->iso_count, hi_u8);
    scene_load_array(module, "dynamic_drc:range_filter_coef", dynamic_drc->range_filter_coef,
        dynamic_drc->iso_count, hi_u8);
    scene_load_array(module, "dynamic_drc:detail_adjust_coef", dynamic_drc->detail_adjust_coef,
        dynamic_drc->iso_count, hi_u8);
    scene_load_array(module, "dynamic_drc:rim_reduction_strength", dynamic_drc->rim_reduction_strength,
        dynamic_drc->iso_count, hi_u8);
    scene_load_array(module, "dynamic_drc:rim_reduction_threshold", dynamic_drc->rim_reduction_threshold,
        dynamic_drc->iso_count, hi_u8);
    scene_load_array(module, "dynamic_drc:bright_gain_limit", dynamic_drc->bright_gain_limit,
        dynamic_drc->iso_count, hi_u8);
    scene_load_array(module, "dynamic_drc:bright_gain_limit_step", dynamic_drc->bright_gain_limit_step,
        dynamic_drc->iso_count, hi_u8);
    scene_load_array(module, "dynamic_drc:dark_gain_limit_luma", dynamic_drc->dark_gain_limit_luma,
        dynamic_drc->iso_count, hi_u8);
    scene_load_array(module, "dynamic_drc:dark_gain_limit_chroma", dynamic_drc->dark_gain_limit_chroma,
        dynamic_drc->iso_count, hi_u8);

    scene_load_array(module, "dynamic_drc:manual_str", dynamic_drc->manual_str, dynamic_drc->iso_count, hi_u16);
    scene_load_array(module, "dynamic_drc:tm_value_low",   dynamic_drc->tm_value_low,  HI_ISP_DRC_TM_NODE_NUM, hi_u16);
    scene_load_array(module, "dynamic_drc:tm_value_high",  dynamic_drc->tm_value_high, HI_ISP_DRC_TM_NODE_NUM, hi_u16);
    scene_load_array(module, "dynamic_drc:tm_val_higher",  dynamic_drc->tm_val_higher, HI_ISP_DRC_TM_NODE_NUM, hi_u16);
    scene_load_array(module, "dynamic_drc:tm_val_highest", dynamic_drc->tm_val_highest, HI_ISP_DRC_TM_NODE_NUM, hi_u16);

    return HI_SUCCESS;
}
static hi_s32 scene_load_dynamic_drc(const hi_char *module, ot_scene_module_state module_state,
    ot_scene_dynamic_drc *dynamic_drc)
{
    ot_scenecomm_check_pointer_return(dynamic_drc, HI_FAILURE);
    if (module_state.dynamic_drc == 0) {
        return HI_SUCCESS;
    }
    hi_s32 ret;
    hi_s32 get_value;

    scene_load_int(module, "dynamic_drc:ratio_count", dynamic_drc->ratio_count, hi_u32);

    scene_load_array(module, "dynamic_drc:ratio_level", dynamic_drc->ratio_level, dynamic_drc->ratio_count, hi_u32);
    scene_load_array(module, "dynamic_drc:blend_luma_max", dynamic_drc->blend_luma_max,
        dynamic_drc->ratio_count, hi_u8);
    scene_load_array(module, "dynamic_drc:detail_adjust_coef_x", dynamic_drc->detail_adjust_coef_x,
        dynamic_drc->ratio_count, hi_u8);
    scene_load_array(module, "dynamic_drc:detail_adjust_coef_blend", dynamic_drc->detail_adjust_coef_blend,
        dynamic_drc->ratio_count, hi_u8);

    scene_load_array(module, "dynamic_drc:low_saturation_color_ctrl", dynamic_drc->low_saturation_color_ctrl,
        dynamic_drc->ratio_count, hi_u8);
    scene_load_array(module, "dynamic_drc:high_saturation_color_ctrl", dynamic_drc->high_saturation_color_ctrl,
        dynamic_drc->ratio_count, hi_u8);

    scene_load_int(module, "dynamic_drc:ref_ratio_count", dynamic_drc->ref_ratio_count, hi_u32);

    scene_load_array(module, "dynamic_drc:ref_ratio_ltoh", dynamic_drc->ref_ratio_ltoh,
        dynamic_drc->ref_ratio_count, hi_u32);
    scene_load_array(module, "dynamic_drc:ref_ratio_alpha", dynamic_drc->ref_ratio_alpha,
        dynamic_drc->ref_ratio_count, hi_u32);

    scene_load_int(module, "dynamic_drc:tm_ratio_threshold", dynamic_drc->tm_ratio_threshold, hi_u32);

    scene_load_int(module, "dynamic_drc:iso_count", dynamic_drc->iso_count, hi_u32);

    scene_load_array(module, "dynamic_drc:iso_level", dynamic_drc->iso_level, dynamic_drc->iso_count, hi_u32);

    scene_load_int(module, "dynamic_drc:interval", dynamic_drc->interval, hi_u32);
    scene_load_int(module, "dynamic_drc:enable", dynamic_drc->enable, hi_bool);

    ret = scene_load_dynamic_drc_arrays(module, dynamic_drc);
    ot_scenecomm_check_return(ret, ret);

    return HI_SUCCESS;
}

static hi_s32 scene_load_dynamic_linear_drc_arrays(const hi_char *module,
    ot_scene_dynamic_linear_drc *dynamic_linear_drc)
{
    scene_load_array(module, "dynamic_linear_drc:spatial_filter_coef", dynamic_linear_drc->spatial_filter_coef,
        dynamic_linear_drc->iso_count, hi_u8);

    scene_load_array(module, "dynamic_linear_drc:range_filter_coef", dynamic_linear_drc->range_filter_coef,
        dynamic_linear_drc->iso_count, hi_u8);

    scene_load_array(module, "dynamic_linear_drc:detail_adjust_coef", dynamic_linear_drc->detail_adjust_coef,
        dynamic_linear_drc->iso_count, hi_u8);

    scene_load_array(module, "dynamic_linear_drc:rim_reduction_strength", dynamic_linear_drc->rim_reduction_strength,
        dynamic_linear_drc->iso_count, hi_u8);

    scene_load_array(module, "dynamic_linear_drc:rim_reduction_threshold", dynamic_linear_drc->rim_reduction_threshold,
        dynamic_linear_drc->iso_count, hi_u8);

    scene_load_array(module, "dynamic_linear_drc:bright_gain_limit", dynamic_linear_drc->bright_gain_limit,
        dynamic_linear_drc->iso_count, hi_u8);

    scene_load_array(module, "dynamic_linear_drc:bright_gain_limit_step", dynamic_linear_drc->bright_gain_limit_step,
        dynamic_linear_drc->iso_count, hi_u8);

    scene_load_array(module, "dynamic_linear_drc:dark_gain_limit_luma", dynamic_linear_drc->dark_gain_limit_luma,
        dynamic_linear_drc->iso_count, hi_u8);

    scene_load_array(module, "dynamic_linear_drc:dark_gain_limit_chroma", dynamic_linear_drc->dark_gain_limit_chroma,
        dynamic_linear_drc->iso_count, hi_u8);

    scene_load_array(module, "dynamic_linear_drc:detail_adjust_coef_x", dynamic_linear_drc->detail_adjust_coef_x,
        dynamic_linear_drc->iso_count, hi_u8);

    scene_load_array(module, "dynamic_linear_drc:detail_adjust_coef_blend",
        dynamic_linear_drc->detail_adjust_coef_blend, dynamic_linear_drc->iso_count, hi_u8);

    scene_load_array(module, "dynamic_linear_drc:low_saturation_color_ctrl",
        dynamic_linear_drc->low_saturation_color_ctrl, dynamic_linear_drc->iso_count, hi_u8);

    scene_load_array(module, "dynamic_linear_drc:high_saturation_color_ctrl",
        dynamic_linear_drc->high_saturation_color_ctrl, dynamic_linear_drc->iso_count, hi_u8);

    scene_load_array(module, "dynamic_linear_drc:asymmetry", dynamic_linear_drc->asymmetry,
        dynamic_linear_drc->iso_count, hi_u8);

    scene_load_array(module, "dynamic_linear_drc:second_pole", dynamic_linear_drc->second_pole,
        dynamic_linear_drc->iso_count, hi_u8);

    scene_load_array(module, "dynamic_linear_drc:compress", dynamic_linear_drc->compress,
        dynamic_linear_drc->iso_count, hi_u8);

    scene_load_array(module, "dynamic_linear_drc:stretch", dynamic_linear_drc->stretch,
        dynamic_linear_drc->iso_count, hi_u8);

    scene_load_array(module, "dynamic_linear_drc:strength", dynamic_linear_drc->strength,
        dynamic_linear_drc->iso_count, hi_u16);

    return HI_SUCCESS;
}

static hi_s32 scene_load_dynamic_linear_drc(const hi_char *module, ot_scene_module_state module_state,
    ot_scene_dynamic_linear_drc *dynamic_linear_drc)
{
    ot_scenecomm_check_pointer_return(dynamic_linear_drc, HI_FAILURE);
    if (module_state.dynamic_linear_drc == 0) {
        return HI_SUCCESS;
    }
    hi_s32 ret;
    hi_s32 get_value;
    hi_u32 i;
    hi_char node[SCENE_INIPARAM_NODE_NAME_LEN] = { 0 };

    scene_load_int(module, "dynamic_linear_drc:enable", dynamic_linear_drc->enable, hi_bool);
    scene_load_int(module, "dynamic_linear_drc:iso_count", dynamic_linear_drc->iso_count, hi_u32);

    scene_load_array(module, "dynamic_linear_drc:iso_level", dynamic_linear_drc->iso_level,
        dynamic_linear_drc->iso_count, hi_u32);

    for (i = 0; i < HI_ISP_DRC_LMIX_NODE_NUM; i++) {
        snprintf_truncated_s(node, SCENE_INIPARAM_NODE_NAME_LEN, "dynamic_linear_drc:local_mixing_bright_%d", i);
        scene_load_array(module, node, dynamic_linear_drc->local_mixing_bright[i],
            dynamic_linear_drc->iso_count, hi_u8);

        snprintf_truncated_s(node, SCENE_INIPARAM_NODE_NAME_LEN, "dynamic_linear_drc:local_mixing_dark_%d", i);
        scene_load_array(module, node, dynamic_linear_drc->local_mixing_dark[i], dynamic_linear_drc->iso_count, hi_u8);

        snprintf_truncated_s(node, SCENE_INIPARAM_NODE_NAME_LEN, "dynamic_linear_drc:local_mixing_bright_x_%d", i);
        scene_load_array(module, node, dynamic_linear_drc->local_mixing_bright_x[i],
            dynamic_linear_drc->iso_count, hi_u8);

        snprintf_truncated_s(node, SCENE_INIPARAM_NODE_NAME_LEN, "dynamic_linear_drc:local_mixing_dark_x_%d", i);
        scene_load_array(module, node, dynamic_linear_drc->local_mixing_dark_x[i],
            dynamic_linear_drc->iso_count, hi_u8);
    }

    for (i = 0; i < HI_ISP_DRC_FLTX_NODE_NUM; i++) {
        snprintf_truncated_s(node, SCENE_INIPARAM_NODE_NAME_LEN, "dynamic_linear_drc:filter_coef_x_%d", i);
        scene_load_array(module, node, dynamic_linear_drc->filter_coef_x[i], dynamic_linear_drc->iso_count, hi_u8);

        snprintf_truncated_s(node, SCENE_INIPARAM_NODE_NAME_LEN, "dynamic_linear_drc:filter_low_threshold_x_%d", i);
        scene_load_array(module, node, dynamic_linear_drc->filter_low_threshold_x[i],
            dynamic_linear_drc->iso_count, hi_u8);

        snprintf_truncated_s(node, SCENE_INIPARAM_NODE_NAME_LEN, "dynamic_linear_drc:filter_high_threshold_x_%d", i);
        scene_load_array(module, node, dynamic_linear_drc->filter_high_threshold_x[i],
            dynamic_linear_drc->iso_count, hi_u8);
    }

    ret = scene_load_dynamic_linear_drc_arrays(module, dynamic_linear_drc);
    ot_scenecomm_check_return(ret, ret);

    return HI_SUCCESS;
}

static hi_s32 scene_load_dynamic_gamma(const hi_char *module, ot_scene_dynamic_gamma *dynamic_gamma)
{
    ot_scenecomm_check_pointer_return(dynamic_gamma, HI_FAILURE);
    hi_s32 ret;
    hi_s32 get_value;
    hi_u32 i;
    hi_char node[SCENE_INIPARAM_NODE_NAME_LEN] = { 0 };

    scene_load_int(module, "dynamic_gamma:option", dynamic_gamma->option, hi_u8);
    scene_load_int(module, "dynamic_gamma:total_num", dynamic_gamma->total_num, hi_u32);
    scene_load_int(module, "dynamic_gamma:interval", dynamic_gamma->interval, hi_u32);

    scene_load_array(module, "dynamic_gamma:exp_thresh_ltoh", dynamic_gamma->exp_thresh_ltoh,
        dynamic_gamma->total_num, hi_u64);

    scene_load_array(module, "dynamic_gamma:exp_thresh_htol", dynamic_gamma->exp_thresh_htol,
        dynamic_gamma->total_num, hi_u64);

    /* table */
    if (dynamic_gamma->option == 0) {
        for (i = 0; i < dynamic_gamma->total_num; i++) {
            snprintf_truncated_s(node, SCENE_INIPARAM_NODE_NAME_LEN, "dynamic_gamma:table_%d", i);
            scene_load_array(module, node, dynamic_gamma->table[i], HI_ISP_GAMMA_NODE_NUM, hi_u16);
        }
    } else {
        scene_load_int(module, "dynamic_gamma:u8IsoCount", dynamic_gamma->iso_count, hi_u8);
        scene_load_int(module, "dynamic_gamma:u8RatioCount", dynamic_gamma->ratio_count, hi_u8);

        for (i = 0; i < dynamic_gamma->iso_count; i++) {
            snprintf_truncated_s(node, SCENE_INIPARAM_NODE_NAME_LEN, "dynamic_gamma:gammaIdx_%d", i);
            scene_load_array(module, node, dynamic_gamma->gamma_idx[i], dynamic_gamma->ratio_count, hi_u16);
        }

        for (i = 0; i < dynamic_gamma->total_num; i++) {
            snprintf_truncated_s(node, SCENE_INIPARAM_NODE_NAME_LEN, "dynamic_gamma:WDR_Table_%d", i);
            scene_load_array(module, node, dynamic_gamma->table[i], HI_ISP_GAMMA_NODE_NUM, hi_u16);
        }
    }
    return HI_SUCCESS;
}

static hi_s32 scene_load_dynamic_nr_arrays(const hi_char *module, ot_scene_dynamic_nr *dynamic_nr)
{
    hi_u32 i;
    hi_char node[SCENE_INIPARAM_NODE_NAME_LEN] = { 0 };

    for (i = 0; i < HI_ISP_WDR_MAX_FRAME_NUM; i++) {
        snprintf_truncated_s(node, SCENE_INIPARAM_NODE_NAME_LEN, "dynamic_nr:md_fusion_frame_str_incr_%d", i);
        scene_load_array(module, node, dynamic_nr->md_fusion_frame_str_incr[i], dynamic_nr->ratio_count, hi_u8);
    }

    for (i = 0; i < HI_ISP_WDR_MAX_FRAME_NUM; i++) {
        snprintf_truncated_s(node, SCENE_INIPARAM_NODE_NAME_LEN, "dynamic_nr:snr_sfm0_fusion_frame_str_incr_%d", i);
        scene_load_array(module, node, dynamic_nr->snr_sfm0_fusion_frame_str_incr[i], dynamic_nr->ratio_count, hi_u8);
    }

    for (i = 0; i < HI_ISP_WDR_MAX_FRAME_NUM; i++) {
        snprintf_truncated_s(node, SCENE_INIPARAM_NODE_NAME_LEN, "dynamic_nr:md_wdr_frame_str_incr_%d", i);
        scene_load_array(module, node, dynamic_nr->md_wdr_frame_str_incr[i], dynamic_nr->ratio_count, hi_u8);
    }

    for (i = 0; i < HI_ISP_WDR_MAX_FRAME_NUM; i++) {
        snprintf_truncated_s(node, SCENE_INIPARAM_NODE_NAME_LEN, "dynamic_nr:snr_sfm0_wdr_frame_str_incr_%d", i);
        scene_load_array(module, node, dynamic_nr->snr_sfm0_wdr_frame_str_incr[i], dynamic_nr->ratio_count, hi_u8);
    }

    for (i = 0; i < HI_SCENE_NR_LUT_LENGTH; i++) {
        snprintf_truncated_s(node, SCENE_INIPARAM_NODE_NAME_LEN, "dynamic_nr:coring_ratio_%d", i);
        scene_load_array(module, node, dynamic_nr->coring_ratio[i], dynamic_nr->coring_ratio_count, hi_u16);
    }

    for (i = 0; i < HI_ISP_WDR_MAX_FRAME_NUM; i++) {
        snprintf_truncated_s(node, SCENE_INIPARAM_NODE_NAME_LEN, "dynamic_nr:snr_sfm0_wdr_frame_str_%d", i);
        scene_load_array(module, node, dynamic_nr->snr_sfm0_wdr_frame_str[i], dynamic_nr->coring_ratio_count, hi_u8);
    }

    for (i = 0; i < HI_ISP_WDR_MAX_FRAME_NUM; i++) {
        snprintf_truncated_s(node, SCENE_INIPARAM_NODE_NAME_LEN, "dynamic_nr:snr_sfm0_fusion_frame_str_%d", i);
        scene_load_array(module, node, dynamic_nr->snr_sfm0_fusion_frame_str[i], dynamic_nr->coring_ratio_count, hi_u8);
    }

    for (i = 0; i < HI_ISP_WDR_MAX_FRAME_NUM; i++) {
        snprintf_truncated_s(node, SCENE_INIPARAM_NODE_NAME_LEN, "dynamic_nr:md_wdr_frame_str_%d", i);
        scene_load_array(module, node, dynamic_nr->md_wdr_frame_str[i], dynamic_nr->coring_ratio_count, hi_u8);
    }

    for (i = 0; i < HI_ISP_WDR_MAX_FRAME_NUM; i++) {
        snprintf_truncated_s(node, SCENE_INIPARAM_NODE_NAME_LEN, "dynamic_nr:md_fusion_frame_str_%d", i);
        scene_load_array(module, node, dynamic_nr->md_fusion_frame_str[i], dynamic_nr->coring_ratio_count, hi_u8);
    }

    return HI_SUCCESS;
}
static hi_s32 scene_load_dynamic_nr(const hi_char *module, ot_scene_dynamic_nr *dynamic_nr)
{
    ot_scenecomm_check_pointer_return(dynamic_nr, HI_FAILURE);
    hi_s32 ret;
    hi_s32 get_value;

    scene_load_int(module, "dynamic_nr:coring_ratio_count", dynamic_nr->coring_ratio_count, hi_u32);
    scene_load_array(module, "dynamic_nr:coring_ratio_iso", dynamic_nr->coring_ratio_iso,
                     dynamic_nr->coring_ratio_count, hi_u32);
    scene_load_int(module, "dynamic_nr:ratio_count", dynamic_nr->ratio_count, hi_u32);
    scene_load_array(module, "dynamic_nr:ratio_level", dynamic_nr->ratio_level, dynamic_nr->ratio_count, hi_u32);
    ret = scene_load_dynamic_nr_arrays(module, dynamic_nr);
    ot_scenecomm_check_return(ret, ret);

    scene_load_array(module, "dynamic_nr:snr_sfm1_wdr_frame_str_s", dynamic_nr->snr_sfm1_wdr_frame_str_s,
                     dynamic_nr->coring_ratio_count, hi_u8);
    scene_load_array(module, "dynamic_nr:snr_sfm1_fusion_frame_str_s", dynamic_nr->snr_sfm1_fusion_frame_str_s,
                     dynamic_nr->coring_ratio_count, hi_u8);
    scene_load_int(module, "dynamic_nr:wdr_ratio_threshold", dynamic_nr->wdr_ratio_threshold, hi_u16);

    scene_load_array(module, "dynamic_nr:tnr_tss_l", dynamic_nr->tnr_tss_l, HI_ISP_AUTO_ISO_NUM, hi_u8);
    scene_load_array(module, "dynamic_nr:tnr_tss_h", dynamic_nr->tnr_tss_h, HI_ISP_AUTO_ISO_NUM, hi_u8);
    scene_load_array(module, "dynamic_nr:coring_wgt_l", dynamic_nr->coring_wgt_l, HI_ISP_AUTO_ISO_NUM, hi_u16);
    scene_load_array(module, "dynamic_nr:coring_wgt_h", dynamic_nr->coring_wgt_h, HI_ISP_AUTO_ISO_NUM, hi_u16);

    return HI_SUCCESS;
}

static hi_s32 scene_load_dynamic_ca(const hi_char *module, ot_scene_dynamic_ca *dynamic_ca)
{
    ot_scenecomm_check_pointer_return(dynamic_ca, HI_FAILURE);
    hi_s32 ret;
    hi_s32 get_value;
    hi_u32 i;
    hi_char node[SCENE_INIPARAM_NODE_NAME_LEN] = { 0 };

    scene_load_int(module, "dynamic_ca:CACount", dynamic_ca->ca_count, hi_u32);

    scene_load_array(module, "dynamic_ca:IsoThresh", dynamic_ca->iso_thresh, dynamic_ca->ca_count, hi_u32);

    for (i = 0; i < dynamic_ca->ca_count; i++) {
        snprintf_truncated_s(node, SCENE_INIPARAM_NODE_NAME_LEN, "dynamic_ca:CAYRatioLut_%d", i);
        scene_load_array(module, module, dynamic_ca->cay_ratio_lut[i], HI_ISP_CA_YRATIO_LUT_LENGTH, hi_u32);
    }

    return HI_SUCCESS;
}

static hi_s32 scene_load_dynamic_blc(const hi_char *module, ot_scene_dynamic_blc *dynamic_blc)
{
    ot_scenecomm_check_pointer_return(dynamic_blc, HI_FAILURE);
    hi_s32 ret;
    hi_s32 get_value;

    scene_load_int(module, "dynamic_blc:black_level_mode", dynamic_blc->black_level_mode, hi_u8);
    scene_load_int(module, "dynamic_blc:blc_count", dynamic_blc->blc_count, hi_u32);
    scene_load_array(module, "dynamic_blc:iso_thresh", dynamic_blc->iso_thresh, dynamic_blc->blc_count, hi_u32);

    /* 4 channels value R Gr Gb B */
    scene_load_array(module, "dynamic_blc:blc_r", dynamic_blc->blc_r, dynamic_blc->blc_count, hi_u32);
    scene_load_array(module, "dynamic_blc:blc_gr", dynamic_blc->blc_gr, dynamic_blc->blc_count, hi_u32);
    scene_load_array(module, "dynamic_blc:blc_gb", dynamic_blc->blc_gb, dynamic_blc->blc_count, hi_u32);
    scene_load_array(module, "dynamic_blc:blc_b", dynamic_blc->blc_b, dynamic_blc->blc_count, hi_u32);

    return HI_SUCCESS;
}

static hi_s32 scene_load_dynamic_dpc(const hi_char *module, ot_scene_dynamic_dpc *dynamic_dpc)
{
    ot_scenecomm_check_pointer_return(dynamic_dpc, HI_FAILURE);
    hi_s32 ret;
    hi_s32 get_value;

    scene_load_int(module, "dynamic_dpc:iso_count", dynamic_dpc->iso_count, hi_u8);
    scene_load_array(module, "dynamic_dpc:iso_level", dynamic_dpc->iso_level, dynamic_dpc->iso_count, hi_u32);

    scene_load_array(module, "dynamic_dpc:sup_twinkle_en", dynamic_dpc->sup_twinkle_en, dynamic_dpc->iso_count, hi_u8);
    scene_load_array(module, "dynamic_dpc:soft_thr", dynamic_dpc->soft_thr, dynamic_dpc->iso_count, hi_u8);
    scene_load_array(module, "dynamic_dpc:soft_slope", dynamic_dpc->soft_slope, dynamic_dpc->iso_count, hi_u8);

    scene_load_int(module, "dynamic_dpc:ratio_count", dynamic_dpc->ratio_count, hi_u8);
    scene_load_array(module, "dynamic_dpc:ratio_level", dynamic_dpc->ratio_level, dynamic_dpc->ratio_count, hi_u32);
    scene_load_array(module, "dynamic_dpc:bright_strength",
        dynamic_dpc->bright_strength, dynamic_dpc->ratio_count, hi_u8);
    scene_load_array(module, "dynamic_dpc:dark_strength", dynamic_dpc->dark_strength, dynamic_dpc->ratio_count, hi_u8);

    return HI_SUCCESS;
}

static hi_s32 scene_load_dynamic_fswdr(const hi_char *module, ot_scene_dynamic_fswdr *dynamic_fswdr)
{
    ot_scenecomm_check_pointer_return(dynamic_fswdr, HI_FAILURE);
    hi_s32 ret;
    hi_s32 get_value;

    scene_load_int(module, "dynamic_fswdr:ratio_count", dynamic_fswdr->ratio_count, hi_u8);
    scene_load_array(module, "dynamic_fswdr:ratio_level",
        dynamic_fswdr->ratio_level, dynamic_fswdr->ratio_count, hi_u32);

    scene_load_array(module, "dynamic_fswdr:wdr_merge_mode",
        dynamic_fswdr->wdr_merge_mode, dynamic_fswdr->ratio_count, hi_u8);

    scene_load_array(module, "dynamic_fswdr:motion_comp",
        dynamic_fswdr->motion_comp, dynamic_fswdr->ratio_count, hi_u8);

    return HI_SUCCESS;
}

static hi_s32 scene_load_dynamic_shading(const hi_char *module, ot_scene_dynamic_shading *dynamic_shading)
{
    ot_scenecomm_check_pointer_return(dynamic_shading, HI_FAILURE);
    hi_s32 ret;
    hi_s32 get_value;

    scene_load_int(module, "dynamic_shading:exp_thresh_cnt", dynamic_shading->exp_thresh_cnt, hi_u32);

    scene_load_array(module, "dynamic_shading:exp_thresh_ltoh", dynamic_shading->exp_thresh_ltoh,
        dynamic_shading->exp_thresh_cnt, hi_u64);

    scene_load_array(module, "dynamic_shading:mesh_strength", dynamic_shading->mesh_strength,
        dynamic_shading->exp_thresh_cnt, hi_u16);

    return HI_SUCCESS;
}

static hi_s32 scene_load_dynamic_clut(const hi_char *module, ot_scene_dynamic_clut *dynamic_clut)
{
    ot_scenecomm_check_pointer_return(dynamic_clut, HI_FAILURE);
    hi_s32 ret;
    hi_s32 get_value;

    scene_load_int(module, "dynamic_clut:iso_count", dynamic_clut->iso_count, hi_u32);

    scene_load_array(module, "dynamic_clut:iso_level", dynamic_clut->iso_level, dynamic_clut->iso_count, hi_u32);
    scene_load_array(module, "dynamic_clut:gain_r", dynamic_clut->gain_r, dynamic_clut->iso_count, hi_u32);
    scene_load_array(module, "dynamic_clut:gain_g", dynamic_clut->gain_g, dynamic_clut->iso_count, hi_u32);
    scene_load_array(module, "dynamic_clut:gain_b", dynamic_clut->gain_b, dynamic_clut->iso_count, hi_u32);

    return HI_SUCCESS;
}


static hi_s32 scene_load_dynamic_ldci(const hi_char *module, ot_scene_dynamic_ldci *dynamic_ldci)
{
    ot_scenecomm_check_pointer_return(dynamic_ldci, HI_FAILURE);
    hi_s32 ret;
    hi_s32 get_value;

    scene_load_int(module, "dynamic_ldci:enable_cnt", dynamic_ldci->enable_cnt, hi_u32);

    scene_load_array(module, "dynamic_ldci:enable_exp_thresh_ltoh", dynamic_ldci->enable_exp_thresh_ltoh,
        dynamic_ldci->enable_cnt, hi_u64);
    scene_load_array(module, "dynamic_ldci:enable", dynamic_ldci->enable, dynamic_ldci->enable_cnt, hi_u8);

    scene_load_int(module, "dynamic_ldci:exp_thresh_cnt", dynamic_ldci->exp_thresh_cnt, hi_u32);

    scene_load_array(module, "dynamic_ldci:exp_thresh_ltoh", dynamic_ldci->exp_thresh_ltoh,
        dynamic_ldci->exp_thresh_cnt, hi_u64);
    scene_load_array(module, "dynamic_ldci:manual_ldci_he_pos_wgt", dynamic_ldci->manual_ldci_he_pos_wgt,
        dynamic_ldci->exp_thresh_cnt, hi_u8);

    return HI_SUCCESS;
}

static hi_s32 scene_load_dynamic_venc_bitrate(const hi_char *module, ot_scene_dynamic_venc_bitrate *dynamic_venc)
{
    ot_scenecomm_check_pointer_return(dynamic_venc, HI_FAILURE);
    hi_s32 ret;
    hi_s32 get_value;

    scene_load_int(module, "dynamic_vencbitrate:iso_thresh_cnt", dynamic_venc->iso_thresh_cnt, hi_u32);

    scene_load_array(module, "dynamic_vencbitrate:iso_thresh_ltoh", dynamic_venc->iso_thresh_ltoh,
        dynamic_venc->iso_thresh_cnt, hi_u32);

    scene_load_array(module, "dynamic_vencbitrate:manual_percent", dynamic_venc->manual_percent,
        dynamic_venc->iso_thresh_cnt, hi_u16);

    return HI_SUCCESS;
}

static hi_s32 scene_load_dynamic_false_color(const hi_char *module, ot_scene_dynamic_false_color *dynamic_false_color)
{
    ot_scenecomm_check_pointer_return(dynamic_false_color, HI_FAILURE);
    hi_s32 ret;
    hi_s32 get_value;

    scene_load_int(module, "dynamic_falsecolor:total_num", dynamic_false_color->total_num, hi_u32);

    scene_load_array(module, "dynamic_falsecolor:false_color_exp_thresh",
        dynamic_false_color->false_color_exp_thresh, dynamic_false_color->total_num, hi_u32);
    scene_load_array(module, "dynamic_falsecolor:manual_strength", dynamic_false_color->manual_strength,
        dynamic_false_color->total_num, hi_u8);

    return HI_SUCCESS;
}

static hi_s32 scene_load_dynamic_mcf_part1(const hi_char *module, ot_scene_dynamic_mcf *dynamic_mcf)
{
    ot_scenecomm_check_pointer_return(dynamic_mcf, HI_FAILURE);
    hi_s32 ret;
    hi_s32 get_value;
    hi_u32 i;
    hi_char node[SCENE_INIPARAM_NODE_NAME_LEN] = { 0 };

    scene_load_int(module, "dynamic_mcf:iso_count", dynamic_mcf->iso_count, hi_u32);
    scene_load_array(module, "dynamic_mcf:iso_thresh", dynamic_mcf->iso_thresh, dynamic_mcf->iso_count, hi_u32);
    scene_load_array(module, "dynamic_mcf:color_hf_en", dynamic_mcf->color_hf_en, dynamic_mcf->iso_count, hi_bool);
    scene_load_array(module, "dynamic_mcf:color_hf_gain", dynamic_mcf->color_hf_gain, dynamic_mcf->iso_count, hi_u8);

    scene_load_array(module, "dynamic_mcf:fusion_mono_flat_en", dynamic_mcf->fusion_mono_flat_en,
                     dynamic_mcf->iso_count, hi_bool);
    scene_load_array(module, "dynamic_mcf:fusion_color_ratio_en", dynamic_mcf->fusion_color_ratio_en,
                     dynamic_mcf->iso_count, hi_bool);
    scene_load_array(module, "dynamic_mcf:fusion_mono_ratio_en", dynamic_mcf->fusion_mono_ratio_en,
                     dynamic_mcf->iso_count, hi_bool);

    scene_load_array(module, "dynamic_mcf:lf_det_gain", dynamic_mcf->lf_fusion_det_gain,
        dynamic_mcf->iso_count, hi_u32);
    scene_load_array(module, "dynamic_mcf:mf_det_gain", dynamic_mcf->mf_fusion_det_gain,
        dynamic_mcf->iso_count, hi_u32);
    scene_load_array(module, "dynamic_mcf:hf_det_gain", dynamic_mcf->hf_fusion_det_gain,
        dynamic_mcf->iso_count, hi_u32);
    scene_load_array(module, "dynamic_mcf:lf_mono_flt_radius", dynamic_mcf->lf_mono_flt_radius,
        dynamic_mcf->iso_count, hi_u8);
    scene_load_array(module, "dynamic_mcf:mf_mono_flt_radius", dynamic_mcf->mf_mono_flt_radius,
        dynamic_mcf->iso_count, hi_u8);
    scene_load_array(module, "dynamic_mcf:hf_mono_flt_radius", dynamic_mcf->hf_mono_flt_radius,
        dynamic_mcf->iso_count, hi_u8);

    for (i = 0; i < dynamic_mcf->iso_count; i++) {
        snprintf_truncated_s(node, SCENE_INIPARAM_NODE_NAME_LEN, "dynamic_mcf:cc_uv_gain_lut_%d", i);
        scene_load_array(module, node, dynamic_mcf->cc_uv_gain_lut[i], HI_SCENE_MCF_CC_UV_GAIN_LUT_NUM, hi_u16);
    }
    return HI_SUCCESS;
}

static hi_s32 scene_load_dynamic_mcf(const hi_char *module, ot_scene_dynamic_mcf *dynamic_mcf)
{
    ot_scenecomm_check_pointer_return(dynamic_mcf, HI_FAILURE);
    hi_s32 ret;
    hi_u32 i;
    hi_char node[SCENE_INIPARAM_NODE_NAME_LEN] = { 0 };

    ret = scene_load_dynamic_mcf_part1(module, dynamic_mcf);
    if (ret != HI_SUCCESS) {
        printf("scene_load_dynamic_mcf_part1 failed !\n");
    }

    for (i = 0; i < dynamic_mcf->iso_count; i++) {
        snprintf_truncated_s(node, SCENE_INIPARAM_NODE_NAME_LEN, "dynamic_mcf:lf_fusion_alpha_lut_%d", i);
        scene_load_array(module, node, dynamic_mcf->lf_fusion_alpha_lut[i], HI_SCENE_MCF_WEIGHT_LUT_NUM, hi_u8);
    }
    for (i = 0; i < dynamic_mcf->iso_count; i++) {
        snprintf_truncated_s(node, SCENE_INIPARAM_NODE_NAME_LEN, "dynamic_mcf:mf_fusion_alpha_lut_%d", i);
        scene_load_array(module, node, dynamic_mcf->mf_fusion_alpha_lut[i], HI_SCENE_MCF_WEIGHT_LUT_NUM, hi_u8);
    }
    for (i = 0; i < dynamic_mcf->iso_count; i++) {
        snprintf_truncated_s(node, SCENE_INIPARAM_NODE_NAME_LEN, "dynamic_mcf:hf_fusion_alpha_lut_%d", i);
        scene_load_array(module, node, dynamic_mcf->hf_fusion_alpha_lut[i], HI_SCENE_MCF_WEIGHT_LUT_NUM, hi_u8);
    }
    for (i = 0; i < dynamic_mcf->iso_count; i++) {
        snprintf_truncated_s(node, SCENE_INIPARAM_NODE_NAME_LEN, "dynamic_mcf:fusion_color_ratio_lut_%d", i);
        scene_load_array(module, node, dynamic_mcf->fusion_color_ratio_lut[i], HI_SCENE_MCF_WEIGHT_LUT_NUM, hi_u8);
    }

    for (i = 0; i < dynamic_mcf->iso_count; i++) {
        snprintf_truncated_s(node, SCENE_INIPARAM_NODE_NAME_LEN, "dynamic_mcf:lf_fusion_mono_det_lut_%d", i);
        scene_load_array(module, node, dynamic_mcf->lf_fusion_mono_det_lut[i], HI_SCENE_MCF_WEIGHT_LUT_NUM, hi_u8);
    }
    for (i = 0; i < dynamic_mcf->iso_count; i++) {
        snprintf_truncated_s(node, SCENE_INIPARAM_NODE_NAME_LEN, "dynamic_mcf:mf_fusion_mono_det_lut_%d", i);
        scene_load_array(module, node, dynamic_mcf->mf_fusion_mono_det_lut[i], HI_SCENE_MCF_WEIGHT_LUT_NUM, hi_u8);
    }
    for (i = 0; i < dynamic_mcf->iso_count; i++) {
        snprintf_truncated_s(node, SCENE_INIPARAM_NODE_NAME_LEN, "dynamic_mcf:hf_fusion_mono_det_lut_%d", i);
        scene_load_array(module, node, dynamic_mcf->hf_fusion_mono_det_lut[i], HI_SCENE_MCF_WEIGHT_LUT_NUM, hi_u8);
    }

    return HI_SUCCESS;
}

static hi_s32 scene_load_dynamic_hnr(const hi_char *module, ot_scene_dynamic_hnr *dynamic_hnr)
{
    ot_scenecomm_check_pointer_return(dynamic_hnr, HI_FAILURE);
    hi_s32 ret;
    hi_s32 get_value;

    scene_load_array(module, "dynamic_hnr:dpc_iso_thresh", dynamic_hnr->dpc_iso_thresh, HNR_ISO_NUM, hi_u32);
    scene_load_array(module, "dynamic_hnr:hnr_iso_thresh", dynamic_hnr->hnr_iso_thresh, HNR_ISO_NUM, hi_u32);
    scene_load_int(module, "dynamic_hnr:dpc_chg_en", dynamic_hnr->dpc_chg_en, hi_bool);
    scene_load_int(module, "dynamic_hnr:hnr_chg_en", dynamic_hnr->hnr_chg_en, hi_bool);

    return HI_SUCCESS;
}

static hi_s32 scene_load_dynamic_fpn(const hi_char *module, ot_scene_dynamic_fpn *dynamic_fpn)
{
    ot_scenecomm_check_pointer_return(dynamic_fpn, HI_FAILURE);
    hi_s32 ret;
    hi_s32 get_value;

    scene_load_int(module, "dynamic_fpn:iso_count", dynamic_fpn->iso_count, hi_u32);
    scene_load_int(module, "dynamic_fpn:fpn_iso_thresh", dynamic_fpn->fpn_iso_thresh, hi_u32);
    scene_load_array(module, "dynamic_fpn:iso_thresh", dynamic_fpn->iso_thresh, dynamic_fpn->iso_count, hi_u32);
    scene_load_array(module, "dynamic_fpn:fpn_offset", dynamic_fpn->fpn_offset, dynamic_fpn->iso_count, hi_u32);
    return HI_SUCCESS;
}

#define SCENE_3DNR_ARG_LIST \
    &(p_x->nry0_en), &(p_x->nry1_en), &(p_x->nry2_en), &(p_x->nry3_en), _tmprt2_4(&ps, sfs1, sbr1), \
    _tmprt3_(&ps, sfs2, sft2, sbr2), _tmprt3_(&ps, sfs4, sft4, sbr4), _tmprt1_4(&ps, sf5_md), \
    &ps[4].sfs4, &ps[4].sft4, &ps[4].sbr4, _tmprt4_5(&pi, ies0, ies1, ies2, ies3), \
    _tmprt4_5(&ps, spn, sbn, pbr, j_mode), _tmprt4_5(&ps, sfr7[0], sfr7[1], sfr7[2], sfr7[3]), \
    _tmprt2_5(&ps, sbr7[0], sbr7[1]), &ps[3].sf8_idx0, &ps[3].sf8_idx1, &ps[3].sf8_thrd, &ps[3].sf8_thr, \
    &ps[4].sf8_idx0, &ps[4].sf8_idx1, &ps[4].sf8_thrd, &ps[4].sf8_thr, \
    _tmprt4_5(&ps, sfn0_0, sfn1_0, sfn2_0, sfn3_0), _tmprt3_5(&ps, sth1_0, sth2_0, sth3_0), \
    _tmprt1_5(&ps, sfr), _tmprt4_3(&ps, sfn0_1, sfn1_1, sfn2_1, sfn3_1), \
    _tmprt3y(&ps, sth1_1, sth1_2, sth1_3), _tmprt2z(&ps, 2, 3, bri_idx0, bri_idx1), &pt[0].ref_en, \
    &pt[1].ref_en, &p_x->nrc0_en, &pt[0].str0, &pt[1].str0, &pt[1].str1, &pt[2].str0, \
    &p_x->nrc0.auto_math, &pt[0].sdz0, &pt[1].sdz0, &pt[1].sdz1, &pt[2].sdz0, &p_x->nrc0.sfc, \
    &pt[0].tss0, &pt[1].tss0, &pt[1].tss1, &pt[2].tss0, &p_x->nrc0.sfc_enhance, &p_x->nrc0.sfc_ext, \
    &p_x->nrc0.tfc, &pt[0].tfs0, &pt[1].tfs0, &pt[1].tfs1, &pt[2].tfs0, &p_x->nrc0.trc, &pt[0].tdz0, \
    &pt[1].tdz0, &pt[1].tdz1, &pt[2].tdz0, &p_x->nrc1_en, &pc->sfs1, &pc->sbr1, \
    _t3_(&pt, 0, tfr0[0], tfr0[1], tfr0[2]), _tmprt3z(&pt, 1, 2, tfr0[0], tfr0[1], tfr0[2]), &pc->sfs2, \
    &pc->sft2, &pc->sbr2, _t3_(&pt, 0, tfr0[3], tfr0[4], tfr0[5]), \
    _t3_(&pt, 1, tfr0[3], tfr0[4], tfr0[5]), _t3_(&pt, 2, tfr0[3], tfr0[4], tfr0[5]), &pc->sfs4, \
    &pc->sft4, &pc->sbr4, _t3_(&pt, 1, tfr1[0], tfr1[1], tfr1[2]), &pc->sf5_str_u, \
    _t3_(&pt, 1, tfr1[3], tfr1[4], tfr1[5]), &pc->sf5_str_v, &pc->sfc6, &pc->sfc_ext6, &pc->sfr6_u, \
    &pc->sfr6_v, _tmprt2z(&pm, 0, 1, mai00, mai02), &pc->sbn0, &pc->spn0, &pc->pbr0, &pc->sbn1, \
    &pc->spn1, &pc->pbr1, &pm[0].adv_math, &pc->sat0_l_sfn8, &pc->sat0_h_sfn8, &pc->sat1_l_sfn8, \
    &pc->sat1_h_sfn8, &pt[0].math_mode, &pm[0].adv_th, &pc->hue0_l_sfn9, &pc->hue0_h_sfn9, \
    &pc->hue1_l_sfn9, &pc->hue1_h_sfn9, &pt[0].auto_math, &pm[0].math0, &pm[0].math1, &pm[1].math0, \
    &pc->bri0_l_sfn10, &pc->bri0_h_sfn10, &pc->bri1_l_sfn10, &pc->bri1_h_sfn10, &pm[0].mate0, \
    &pm[0].mate1, &pm[1].mate0, &pc->sfn0, &pc->sfn1, &pm[0].mabw0, &pm[0].mabw1, &pm[1].mabw0, \
    &pc->post_sfc, _t4_0_(&pc, for_grd_sat), _t4_4_(&pc, for_grd_sat), _t4_8_(&pc, for_grd_sat), \
    _t4_12_(&pc, for_grd_sat), &pc->for_grd_sat[0x10], _t4_0_(&pc, for_grd_hue), \
    _t4_4_(&pc, for_grd_hue), _t4_8_(&pc, for_grd_hue), _t4_12_(&pc, for_grd_hue), \
    &pc->for_grd_hue[0x10], _t4_0_(&pc, for_grd_bri), _t4_4_(&pc, for_grd_bri), \
    _t4_8_(&pc, for_grd_bri), _t4_12_(&pc, for_grd_bri), &pc->for_grd_bri[0x10], \
    _t4_0_(&pc, bak_grd_sat), _t4_4_(&pc, bak_grd_sat), _t4_8_(&pc, bak_grd_sat), \
    _t4_12_(&pc, bak_grd_sat), &pc->bak_grd_sat[0x10], _t4_0_(&pc, bak_grd_hue), \
    _t4_4_(&pc, bak_grd_hue), _t4_8_(&pc, bak_grd_hue), _t4_12_(&pc, bak_grd_hue), \
    &pc->bak_grd_hue[0x10], _t4_0_(&pc, bak_grd_bri), _t4_4_(&pc, bak_grd_bri), \
    _t4_8_(&pc, bak_grd_bri), _t4_12_(&pc, bak_grd_bri), &pc->bak_grd_bri[0x10], \
    _t4a_0_(&ps, 2, bri_str), _t4a_4_(&ps, 2, bri_str), _t4a_8_(&ps, 2, bri_str), \
    _t4a_12_(&ps, 2, bri_str), &ps[2].bri_str[0x10], _t4a_0_(&ps, 3, bri_str), _t4a_4_(&ps, 3, bri_str), \
    _t4a_8_(&ps, 3, bri_str), _t4a_12_(&ps, 3, bri_str), &ps[3].bri_str[0x10]

static const char *g_3dnr_fmt =
    " -en                 %3d    |               %3d  |              %3d  |               %3d          "
    "             "
    " -nXsf1      %3d:    %3d    |       %3d:    %3d  |      %3d:    %3d  |            %3d:    %3d     "
    "             "
    " -nXsf2      %3d:%3d:%3d    |       %3d:%3d:%3d  |      %3d:%3d:%3d  |            %3d:%3d:%3d     "
    "             "
    " -nXsf4      %3d:%3d:%3d    |       %3d:%3d:%3d  |      %3d:%3d:%3d  |            %3d:%3d:%3d     "
    "             "
    " -sfs5               %3d    |               %3d  |              %3d  |                    %3d     "
    "             "
    "                            |                    |                   |  -sfs6     %3d:%3d:%3d     "
    "             "
    " -nXsf6  %3d:%3d:%3d:%3d    |   %3d:%3d:%3d:%3d  |  %3d:%3d:%3d:%3d  |  %3d:%3d:%3d:%3d  |  "
    "%3d:%3d:%3d:%3d    "
    " -nXsf7  %3d:%3d:%3d:%3d    |   %3d:%3d:%3d:%3d  |  %3d:%3d:%3d:%3d  |  %3d:%3d:%3d:%3d  |  "
    "%3d:%3d:%3d:%3d    "
    " -nXsfr7 %3d:%3d:%3d:%3d    |   %3d:%3d:%3d:%3d  |  %3d:%3d:%3d:%3d  |  %3d:%3d:%3d:%3d  |  "
    "%3d:%3d:%3d:%3d    "
    " -nXsbr7         %3d:%3d    |           %3d:%3d  |          %3d:%3d  |          %3d:%3d  |        "
    "  %3d:%3d    "
    " -nXsf8                     |                    |                   |  %3d:%3d:%3d:%3d  |  "
    "%3d:%3d:%3d:%3d    "
    " -nXsfn  %3d:%3d:%3d:%3d    |   %3d:%3d:%3d:%3d  |  %3d:%3d:%3d:%3d  |  %3d:%3d:%3d:%3d  |  "
    "%3d:%3d:%3d:%3d    "
    " -nXsth      %3d:%3d:%3d    |       %3d:%3d:%3d  |      %3d:%3d:%3d  |      %3d:%3d:%3d  |      "
    "%3d:%3d:%3d    "
    " -sfr     (0)    %3d        |        %3d         |        %3d        |        %3d        |        "
    "%3d          "
    " -nX2sfn %3d:%3d:%3d:%3d    |   %3d:%3d:%3d:%3d  |  %3d:%3d:%3d:%3d  |                            "
    "             "
    " -nX2sth     %3d:%3d:%3d    |       %3d:%3d:%3d  |      %3d:%3d:%3d  |                            "
    "             "
    " -nXbIdx                    |                    |      %3d:%3d      |                %3d:%3d     "
    "             "
    " -ref            %3d        |        %3d         |                   "
    "|********************nr_c0****************"
    "                            |                    |                   |  -nC0en             %3d    "
    "             "
    " -nXstr   (1)    %3d        |        %3d:%3d     |        %3d        |  -nCmath            %3d    "
    "             "
    " -nXsdz   (1)    %3d        |        %3d:%3d     |        %3d        |  -sfc               %3d    "
    "             "
    " -nXtss          %3d        |        %3d:%3d     |        %3d        |  -sfcExt          %3d:%3d  "
    "             "
    "                            |                    |                   |  -tfc               %3d    "
    "             "
    " -nXtfs   (3)    %3d        |        %3d:%3d     |        %3d        |  -trc               %3d    "
    "             "
    " -nXtdz          %3d        |        %3d:%3d     |        %3d        "
    "|********************nr_c1****************"
    "                            |                    |                   | -nC1en              %3d    "
    "             "
    "                            |                    |                   | -nCsf1          %3d    "
    ":%3d             "
    " -nXtfr0  (2) %3d:%3d:%3d   |     %3d:%3d:%3d    |    %3d:%3d:%3d    | -nCsf2          "
    "%3d:%3d:%3d             "
    "              %3d:%3d:%3d   |     %3d:%3d:%3d    |    %3d:%3d:%3d    | -nCsf4          "
    "%3d:%3d:%3d             "
    " -nXtfr1                    |     %3d:%3d:%3d    |                   | -nCsf5u             %3d    "
    "             "
    "                            |     %3d:%3d:%3d    |                   | -nCsf5v             %3d    "
    "             "
    "                            |                    |                   | -nCsfc6           %3d:%3d  "
    "             "
    "                            |                    |                   | -nCsf6uv          %3d:%3d  "
    "             "
    "                            |-mXid   %3d:%3d     |      %3d:%3d      | -nCXsf7   %3d:%3d:%3d |  "
    "%3d:%3d:%3d    "
    "                            |-advMath %3d        |                   | -nCXsf8       %3d:%3d |    "
    "  %3d:%3d    "
    " -mathMode   %3d            |-advTh   %3d        |                   | -nCXsf9       %3d:%3d |    "
    "  %3d:%3d    "
    " -autoMath   %3d            |-mXmath %3d:%3d     |        %3d        | -nCXsf10      %3d:%3d |    "
    "  %3d:%3d    "
    "                            |-mXmate %3d:%3d     |        %3d        | -nCsfn            %3d |    "
    "      %3d    "
    "                            |-mXmabw %3d:%3d     |        %3d        | -postSFC           %3d     "
    "             "
    "**************************************************************************************************"
    "*************"
    " -c0sat0   %3d:%3d:%3d:%3d  |-c0sat4  %3d:%3d:%3d:%3d |-c0sat8  %3d:%3d:%3d:%3d |-c0sat12  "
    "%3d:%3d:%3d:%3d:%3d "
    " -c0hue0   %3d:%3d:%3d:%3d  |-c0hue4  %3d:%3d:%3d:%3d |-c0hue8  %3d:%3d:%3d:%3d |-c0hue12  "
    "%3d:%3d:%3d:%3d:%3d "
    " -c0bri0   %3d:%3d:%3d:%3d  |-c0bri4  %3d:%3d:%3d:%3d |-c0bri8  %3d:%3d:%3d:%3d |-c0bri12  "
    "%3d:%3d:%3d:%3d:%3d "
    " -c1sat0   %3d:%3d:%3d:%3d  |-c1sat4  %3d:%3d:%3d:%3d |-c1sat8  %3d:%3d:%3d:%3d |-c1sat12  "
    "%3d:%3d:%3d:%3d:%3d "
    " -c1hue0   %3d:%3d:%3d:%3d  |-c1hue4  %3d:%3d:%3d:%3d |-c1hue8  %3d:%3d:%3d:%3d |-c1hue12  "
    "%3d:%3d:%3d:%3d:%3d "
    " -c1bri0   %3d:%3d:%3d:%3d  |-c1bri4  %3d:%3d:%3d:%3d |-c1bri8  %3d:%3d:%3d:%3d |-c1bri12  "
    "%3d:%3d:%3d:%3d:%3d "
    " -n2bri0   %3d:%3d:%3d:%3d  |-n2bri4  %3d:%3d:%3d:%3d |-n2bri8  %3d:%3d:%3d:%3d |-n2bri12  "
    "%3d:%3d:%3d:%3d:%3d "
    " -n3bri0   %3d:%3d:%3d:%3d  |-n3bri4  %3d:%3d:%3d:%3d |-n3bri8  %3d:%3d:%3d:%3d |-n3bri12  "
    "%3d:%3d:%3d:%3d:%3d ";

static hi_s32 scene_load_static_3dnr(const hi_char *module, ot_scene_module_state module_state,
    ot_scene_static_3dnr *static_3dnr)
{
    ot_scenecomm_check_pointer_return(static_3dnr, HI_FAILURE);
    hi_s32 ret;
    hi_char *get_string = NULL;
    hi_u32 i;
    hi_s32 get_value;
    hi_char node[SCENE_INIPARAM_NODE_NAME_LEN] = { 0 };

    scene_load_int(module, "static_3dnr:threed_nr_count", static_3dnr->threed_nr_count, hi_u32);

    scene_load_array(module, "static_3dnr:threed_nr_iso", static_3dnr->threed_nr_iso,
        static_3dnr->threed_nr_count, hi_u32);

    for (i = 0; i < static_3dnr->threed_nr_count; i++) {
        snprintf_truncated_s(node, SCENE_INIPARAM_NODE_NAME_LEN, "static_3dnr:3DnrParam_%d", i);
        ret = ot_confaccess_get_string(SCENE_INIPARAM, module, (const char *)node, NULL, &get_string);
        scene_iniparam_check_load_return(ret, module);

        if (get_string != NULL) {
            ot_scene_3dnr *p_x = &(static_3dnr->threednr_value[i]);
            t_v200_vpss_iey *pi = p_x->iey;
            t_v200_vpss_sfy *ps = p_x->sfy;
            t_v200_vpss_mdy *pm = p_x->mdy;
            t_v200_vpss_tfy *pt = p_x->tfy;
            t_v200_vpss_nrc1 *pc = &p_x->nrc1;

            ret = sscanf_s(get_string, g_3dnr_fmt, SCENE_3DNR_ARG_LIST);
            free(get_string);
            get_string = HI_NULL;
            if (ret == -1) {
                scene_loge("sscanf_s error\n");
                return HI_FAILURE;
            }
        }
    }
    return HI_SUCCESS;
}

#define scene_load_module(func, module, addr) do {     \
        ret = func(module, addr);                      \
        scene_iniparam_check_load_return(ret, module); \
    } while (0)

static hi_s32 scene_load_scene_param_part1(const hi_char *module, ot_scene_pipe_param *scene_param)
{
    ot_scenecomm_check_pointer_return(scene_param, HI_FAILURE);
    hi_s32 ret;

    ret = scene_load_dynamic_drc(module, scene_param->module_state, &scene_param->dynamic_drc);
    scene_iniparam_check_load_return(ret, module);

    ret = scene_load_dynamic_linear_drc(module, scene_param->module_state, &scene_param->dynamic_linear_drc);
    scene_iniparam_check_load_return(ret, module);
    scene_load_module(scene_load_dynamic_gamma, module, &scene_param->dynamic_gamma);
    scene_load_module(scene_load_dynamic_nr, module, &scene_param->dynamic_nr);
    scene_load_module(scene_load_dynamic_shading, module, &scene_param->dynamic_shading);
    scene_load_module(scene_load_dynamic_clut, module, &scene_param->dynamic_clut);
    scene_load_module(scene_load_dynamic_ca, module, &scene_param->dynamic_ca);
    scene_load_module(scene_load_dynamic_blc, module, &scene_param->dynamic_blc);
    scene_load_module(scene_load_dynamic_dpc, module, &scene_param->dynamic_dpc);
    scene_load_module(scene_load_dynamic_fswdr, module, &scene_param->dynamic_fswdr);
    scene_load_module(scene_load_dynamic_ldci, module, &scene_param->dynamic_ldci);
    scene_load_module(scene_load_dynamic_false_color, module, &scene_param->dynamic_false_color);
    scene_load_module(scene_load_dynamic_mcf, module, &scene_param->dynamic_mcf);
    scene_load_module(scene_load_dynamic_hnr, module, &scene_param->dynamic_hnr);
    scene_load_module(scene_load_dynamic_fpn, module, &scene_param->dynamic_fpn);

    ret = scene_load_static_3dnr(module, scene_param->module_state, &scene_param->static_threednr);
    scene_iniparam_check_load_return(ret, module);
    return HI_SUCCESS;
}


static hi_s32 scene_load_scene_param(const hi_char *module, ot_scene_pipe_param *scene_param)
{
    ot_scenecomm_check_pointer_return(scene_param, HI_FAILURE);
    hi_s32 ret;
    scene_load_module(scene_load_module_state, module, &scene_param->module_state);
    scene_load_module(scene_load_static_ae, module, &scene_param->static_ae);
    scene_load_module(scene_load_ae_weight_tab, module, &scene_param->static_statistics);
    scene_load_module(scene_load_static_ae_route_ex, module, &scene_param->static_ae_route_ex);
    scene_load_module(scene_load_static_wdr_exposure, module, &scene_param->static_wdr_exposure);
    scene_load_module(scene_load_static_fswdr, module, &scene_param->static_fswdr);
    scene_load_module(scene_load_static_awb, module, &scene_param->static_awb);
    scene_load_module(scene_load_static_awb_ex, module, &scene_param->static_awb_ex);
    scene_load_module(scene_load_static_clut, module, &scene_param->static_clut);
    scene_load_module(scene_load_static_cmm, module, &scene_param->static_ccm);
    scene_load_module(scene_load_static_saturation, module, &scene_param->static_saturation);
    scene_load_module(scene_load_static_ldci, module, &scene_param->static_ldci);
    scene_load_module(scene_load_static_pregamma, module, &scene_param->static_pregamma);
    scene_load_module(scene_load_static_blc, module, &scene_param->static_blc);
    scene_load_module(scene_load_static_drc, module, &scene_param->static_drc);
    scene_load_module(scene_load_static_nr, module, &scene_param->static_nr);
    scene_load_module(scene_load_static_ca, module, &scene_param->static_ca);
    scene_load_module(scene_load_static_venc, module, &scene_param->static_venc_attr);
    scene_load_module(scene_load_static_local_cac, module, &scene_param->static_local_cac);
    scene_load_module(scene_load_static_acac, module, &scene_param->static_acac);
    scene_load_module(scene_load_static_dpc, module, &scene_param->static_dpc);
    scene_load_module(scene_load_static_dehaze, module, &scene_param->static_dehaze);
    scene_load_module(scene_load_static_shading, module, &scene_param->static_shading);
    scene_load_module(scene_load_static_csc, module, &scene_param->staic_csc);
    scene_load_module(scene_load_static_demosaic, module, &scene_param->static_dm);
    scene_load_module(scene_load_static_cross_talk, module, &scene_param->static_crosstalk);
    scene_load_module(scene_load_static_bayershp, module, &scene_param->static_bayershp);
    scene_load_module(scene_load_static_sharpen, module, &scene_param->static_sharpen);
    scene_load_module(scene_load_dynamic_ae, module, &scene_param->dynamic_ae);
    scene_load_module(scene_load_dynamic_venc_bitrate, module, &scene_param->dynamic_venc_bitrate);
    scene_load_module(scene_load_dynamic_dehaze, module, &scene_param->dynamic_dehaze);

    ret = scene_load_scene_param_part1(module, scene_param);
    scene_iniparam_check_load_return(ret, module);
    return HI_SUCCESS;
}

static hi_s32 scene_load_scene_conf(ot_scene_pipe_param *scene_pipe_param, hi_s32 count)
{
    ot_scenecomm_check_pointer_return(scene_pipe_param, HI_FAILURE);
    hi_s32 ret;
    hi_s32 mode_index;
    hi_char module[SCENE_INIPARAM_MODULE_NAME_LEN] = { 0 };

    for (mode_index = 0; mode_index < count; mode_index++) {
        snprintf_truncated_s(module, SCENE_INIPARAM_MODULE_NAME_LEN, "%s%d", SCENE_INI_SCENEMODE, mode_index);

        ret = scene_load_scene_param((const char *)module, scene_pipe_param + mode_index);
        if (ret != HI_SUCCESS) {
            scene_loge("load scene[%d] config failed\n", mode_index);
            return HI_FAILURE;
        }
    }

    return HI_SUCCESS;
}

static hi_s32 scene_load_video_param(const hi_char *module, ot_scene_video_mode *video_mode)
{
    ot_scenecomm_check_pointer_return(video_mode, HI_FAILURE);
    hi_s32 ret;
    hi_s32 get_value;
    hi_u32 mode_index;
    hi_s32 i;
    hi_char node[SCENE_INIPARAM_NODE_NAME_LEN] = { 0 };

    for (i = 0; i < SCENE_MAX_VIDEOMODE; i++) {
        snprintf_truncated_s(node, SCENE_INIPARAM_NODE_NAME_LEN, "pipe_comm_%d:SCENE_MODE", i);
        scene_load_int(module, node, (video_mode->video_mode + i)->pipe_mode, hi_u8);

        for (mode_index = 0; mode_index < HI_SCENE_PIPE_MAX_NUM; mode_index++) {
            snprintf_truncated_s(node, SCENE_INIPARAM_NODE_NAME_LEN, "pipe_%d_%d:Enable", i, mode_index);
            scene_load_int(module, node, (video_mode->video_mode + i)->pipe_attr[mode_index].enable, hi_bool);

            snprintf_truncated_s(node, SCENE_INIPARAM_NODE_NAME_LEN, "pipe_%d_%d:VcapPipeHdl", i, mode_index);
            scene_load_int(module, node, (video_mode->video_mode + i)->pipe_attr[mode_index].vcap_pipe_hdl, hi_handle);

            /* SCENE_MODE */
            snprintf_truncated_s(node, SCENE_INIPARAM_NODE_NAME_LEN, "pipe_%d_%d:MainPipeHdl", i, mode_index);
            scene_load_int(module, node, (video_mode->video_mode + i)->pipe_attr[mode_index].main_pipe_hdl, hi_handle);

            snprintf_truncated_s(node, SCENE_INIPARAM_NODE_NAME_LEN, "pipe_%d_%d:PipeChnHdl", i, mode_index);
            scene_load_int(module, node, (video_mode->video_mode + i)->pipe_attr[mode_index].pipe_chn_hdl, hi_handle);

            snprintf_truncated_s(node, SCENE_INIPARAM_NODE_NAME_LEN, "pipe_%d_%d:VpssHdl", i, mode_index);
            scene_load_int(module, node, (video_mode->video_mode + i)->pipe_attr[mode_index].vpss_hdl, hi_handle);

            snprintf_truncated_s(node, SCENE_INIPARAM_NODE_NAME_LEN, "pipe_%d_%d:VportHdl", i, mode_index);
            scene_load_int(module, node, (video_mode->video_mode + i)->pipe_attr[mode_index].vport_hdl, hi_handle);

            snprintf_truncated_s(node, SCENE_INIPARAM_NODE_NAME_LEN, "pipe_%d_%d:VencHdl", i, mode_index);
            scene_load_int(module, node, (video_mode->video_mode + i)->pipe_attr[mode_index].venc_hdl, hi_handle);

            snprintf_truncated_s(node, SCENE_INIPARAM_NODE_NAME_LEN, "pipe_%d_%d:PipeParamIndex", i, mode_index);
            scene_load_int(module, node, (video_mode->video_mode + i)->pipe_attr[mode_index].pipe_param_index, hi_u8);

            snprintf_truncated_s(node, SCENE_INIPARAM_NODE_NAME_LEN, "pipe_%d_%d:PipeType", i, mode_index);
            scene_load_int(module, node, (video_mode->video_mode + i)->pipe_attr[mode_index].pipe_type,
                ot_scene_pipe_type);
        }
    }
    return HI_SUCCESS;
}

static hi_s32 scene_load_video_conf(ot_scene_video_mode *video_mode)
{
    ot_scenecomm_check_pointer_return(video_mode, HI_FAILURE);
    hi_s32 ret;
    hi_char module[SCENE_INIPARAM_MODULE_NAME_LEN] = { 0 };

    snprintf_truncated_s(module, SCENE_INIPARAM_MODULE_NAME_LEN, "%s", SCENE_INI_VIDEOMODE);

    ret = scene_load_video_param((const char *)module, video_mode);
    if (ret != HI_SUCCESS) {
        scene_loge("load videomode config failed\n");
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}


hi_s32 ot_scene_create_param(const hi_char *dir_name, ot_scene_param *scene_param, ot_scene_video_mode *video_mode)
{
    hi_s32 ret;
    hi_u32 module_num = 0;
    hi_char ini_path[SCENETOOL_MAX_FILESIZE] = {0};
    /* Load Product Ini Configure */
    if (scene_param == HI_NULL || video_mode == HI_NULL) {
        scene_loge("Null Pointer.");
        return HI_SUCCESS;
    }

    //maohw snprintf_truncated_s(ini_path, SCENETOOL_MAX_FILESIZE, "%s%s", dir_name, "/config_cfgaccess_hd.ini");
    snprintf(ini_path, SCENETOOL_MAX_FILESIZE, "%s", dir_name);
    ret = ot_confaccess_init(SCENE_INIPARAM, (const char *)ini_path, &module_num);
    if (ret != HI_SUCCESS) {
        scene_loge("load ini [%s] failed [%08x]\n", ini_path, ret);
        return HI_FAILURE;
    }

    (hi_void)memset_s(&g_scene_param_cfg, sizeof(ot_scene_param), 0, sizeof(ot_scene_param));
    ret = scene_load_scene_conf(g_scene_param_cfg.pipe_param, module_num - 1);
    if (ret != HI_SUCCESS) {
        scene_loge("SCENE_LoadConf failed!\n");
        return HI_FAILURE;
    }
    (hi_void)memcpy_s(scene_param, sizeof(ot_scene_param), &g_scene_param_cfg, sizeof(ot_scene_param));

    (hi_void)memset_s(&g_video_mode, sizeof(ot_scene_video_mode), 0, sizeof(ot_scene_video_mode));
    ret = scene_load_video_conf(&g_video_mode);
    if (ret != HI_SUCCESS) {
        scene_loge("SCENE_LoadConf failed!\n");
        return HI_FAILURE;
    }
    (hi_void)memcpy_s(video_mode, sizeof(ot_scene_video_mode), &g_video_mode, sizeof(ot_scene_video_mode));

    ret = ot_confaccess_deinit(SCENE_INIPARAM);
    if (ret != HI_SUCCESS) {
        scene_loge("load ini [%s] failed [%08x]\n", ini_path, ret);
        return HI_FAILURE;
    }
    return HI_SUCCESS;
}

#ifdef __cplusplus
}
#endif /* __cplusplus */
