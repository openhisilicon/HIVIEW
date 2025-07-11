/*
  Copyright (c), 2001-2024, Shenshu Tech. Co., Ltd.
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
#define SCENETOOL_MAX_FILESIZE 512

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
    size_t whole_count = 0;
    size_t length = strlen(input_line);
    hi_u64 hex_value;

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

        errno_t err = memcpy_s(part, sizeof(part) - 1, vr_begin, part_count);
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
    scene_load_int(module, "module_state:bDynamicAIDrc", module_state->dynamic_aidrc, hi_bool);
    scene_load_int(module, "module_state:bDynamicLinearDrc", module_state->dynamic_linear_drc, hi_bool);
    scene_load_int(module, "module_state:bDynamicGamma", module_state->dynamic_gamma, hi_bool);
    scene_load_int(module, "module_state:bDynamicNr", module_state->dynamic_nr, hi_bool);
    scene_load_int(module, "module_state:bDynamicDpc", module_state->dynamic_dpc, hi_bool);
    scene_load_int(module, "module_state:bDynamicLinearCA", module_state->dynamic_linear_ca, hi_bool);
    scene_load_int(module, "module_state:bDynamicCA", module_state->dynamic_ca, hi_bool);
    scene_load_int(module, "module_state:bDynamicBLC", module_state->dynamic_blc, hi_bool);
    scene_load_int(module, "module_state:bDynamicShading", module_state->dynamic_shading, hi_bool);
    scene_load_int(module, "module_state:bDynamicClut", module_state->dynamic_clut, hi_bool);
    scene_load_int(module, "module_state:bDynamicLdci", module_state->dynamic_ldci, hi_bool);
    scene_load_int(module, "module_state:bDynamicFalseColor", module_state->dynamic_false_color, hi_bool);
    scene_load_int(module, "module_state:bDyanamic3DNR", module_state->dynamic_3dnr, hi_bool);
    scene_load_int(module, "module_state:bDyanamicAI3DNR", module_state->dynamic_ai3dnr, hi_bool);
    scene_load_int(module, "module_state:bDyanamicVencMode", module_state->dynamic_venc_mode, hi_bool);

    return HI_SUCCESS;
}

static hi_s32 scene_load_module_state(const hi_char *module, ot_scene_module_state *module_state)
{
    hi_s32 ret, get_value;
    ot_scenecomm_check_pointer_return(module_state, HI_FAILURE);

    scene_load_int(module, "module_state:bDebug", module_state->debug, hi_bool);
    scene_load_int(module, "module_state:bDynamicFps", module_state->dynamic_fps, hi_bool);
    scene_load_int(module, "module_state:bStaticAE", module_state->static_ae, hi_bool);
    scene_load_int(module, "module_state:bAeWeightTab", module_state->ae_weight_tab, hi_bool);
    scene_load_int(module, "module_state:bStaticWdrExposure", module_state->static_wdr_exposure, hi_bool);
    scene_load_int(module, "module_state:bStaticFsWdr", module_state->static_fswdr, hi_bool);
    scene_load_int(module, "module_state:bStaticAWB", module_state->static_awb, hi_bool);
    scene_load_int(module, "module_state:bStaticAWBEx", module_state->static_awbex, hi_bool);
    scene_load_int(module, "module_state:bStaticPipediff", module_state->static_pipe_diff, hi_bool);
    scene_load_int(module, "module_state:bStaticCCM", module_state->static_ccm, hi_bool);
    scene_load_int(module, "module_state:bStaticSaturation", module_state->static_saturation, hi_bool);
    scene_load_int(module, "module_state:bStaticClut", module_state->static_clut, hi_bool);
    scene_load_int(module, "module_state:bStaticLdci", module_state->static_ldci, hi_bool);
    scene_load_int(module, "module_state:bStaticDRC", module_state->static_drc, hi_bool);
    scene_load_int(module, "module_state:bStaticAIDRC", module_state->static_aidrc, hi_bool);
    scene_load_int(module, "module_state:bStaticNr", module_state->static_nr, hi_bool);
    scene_load_int(module, "module_state:bStaticCa", module_state->static_ca, hi_bool);
    scene_load_int(module, "module_state:bStaticCac", module_state->static_cac, hi_bool);
    scene_load_int(module, "module_state:bStaticLocalCac", module_state->static_local_cac, hi_bool);
    scene_load_int(module, "module_state:bStaticDPC", module_state->static_dpc, hi_bool);
    scene_load_int(module, "module_state:bStaticDehaze", module_state->static_dehaze, hi_bool);
    scene_load_int(module, "module_state:bStaticShading", module_state->static_shading, hi_bool);
    scene_load_int(module, "module_state:bDynamicIsoVenc", module_state->dynamic_iso_venc, hi_bool);
    scene_load_int(module, "module_state:bDynamicMcf", module_state->dynamic_mcf, hi_bool);
    scene_load_int(module, "module_state:bDynamicAIBnr", module_state->dynamic_aibnr, hi_bool);
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
        snprintf_truncated_s(node, SCENE_INIPARAM_NODE_NAME_LEN, "static_aeweight:ae_weight_%u", row);
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
    scene_load_int(module, "static_ae:ae_auto_iris", static_ae->ae_auto_iris, hi_bool);

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
        
    scene_load_array(module, "static_aerouteex:iris_fno", static_ae_route_ex->iris_fno, static_ae_route_ex->total_num,
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

    scene_load_int(module, "static_wdrexposure:tolerance", static_wdr_exposure->tolerance, hi_u16);
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
    scene_load_int(module, "static_FsWdr:fusion_blend_en", static_fs_wdr->fusion_blend_en, hi_bool);

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
    scene_load_int(module, "static_awbex:out_shift_limit", static_awb_ex->out_shift_limit, hi_u8);
    scene_load_int(module, "static_awbex:out_thresh", static_awb_ex->out_thresh, hi_u32);
    scene_load_int(module, "static_awbex:low_stop", static_awb_ex->low_stop, hi_u16);
    scene_load_int(module, "static_awbex:high_start", static_awb_ex->high_start, hi_u16);
    scene_load_int(module, "static_awbex:high_stop", static_awb_ex->high_stop, hi_u16);
    scene_load_int(module, "static_awbex:multi_light_source_en", static_awb_ex->multi_light_source_en, hi_u16);

    scene_load_array(module, "static_awbex:multi_ctwt", static_awb_ex->multi_ctwt, HI_ISP_AWB_MULTI_CT_NUM, hi_u16);

    return HI_SUCCESS;
}

static hi_s32 scene_load_static_pipe_diff(const hi_char *module, ot_scene_isp_pipe_diff *static_pipe_diff)
{
    ot_scenecomm_check_pointer_return(static_pipe_diff, HI_FAILURE);

    scene_load_array(module, "static_isp_diff:diff_gain", static_pipe_diff->gain, HI_ISP_BAYER_CHN_NUM, hi_u32);

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
        snprintf_truncated_s(node, SCENE_INIPARAM_NODE_NAME_LEN, "static_ccm:auto_ccm_%u", i);
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
    scene_load_int(module, "static_drc:blend_luma_dark_min", static_drc->blend_luma_dark_min, hi_u8);
    scene_load_int(module, "static_drc:blend_luma_dark_threshold", static_drc->blend_luma_dark_threshold, hi_u8);
    scene_load_int(module, "static_drc:blend_detail_max", static_drc->blend_detail_max, hi_u8);
    scene_load_int(module, "static_drc:blend_detail_bright_min", static_drc->blend_detail_bright_min, hi_u8);
    scene_load_int(module, "static_drc:blend_detail_bright_threshold", static_drc->blend_detail_bright_threshold,
        hi_u8);
    scene_load_int(module, "static_drc:blend_detail_dark_min", static_drc->blend_detail_dark_min, hi_u8);
    scene_load_int(module, "static_drc:blend_detail_dark_threshold", static_drc->blend_detail_dark_threshold, hi_u8);
    scene_load_int(module, "static_drc:global_color_ctrl", static_drc->global_color_ctrl, hi_u8);
    scene_load_array(module, "static_drc:color_correction_lut", static_drc->color_correction_lut,
        HI_ISP_DRC_CC_NODE_NUM, hi_u16);
    return HI_SUCCESS;
}

static hi_s32 scene_load_static_aidrc(const hi_char *module, ot_scene_static_aidrc *static_aidrc)
{
    ot_scenecomm_check_pointer_return(static_aidrc, HI_FAILURE);
    hi_s32 ret;
    hi_s32 get_value;

    scene_load_int(module, "static_aidrc:tone_mapping_wgt_x", static_aidrc->tone_mapping_wgt_x, hi_u8);
    scene_load_int(module, "static_aidrc:detail_adjust_coef_x", static_aidrc->detail_adjust_coef_x, hi_u8);
    scene_load_int(module, "static_aidrc:blend_luma_max", static_aidrc->blend_luma_max, hi_u8);
    scene_load_int(module, "static_aidrc:blend_luma_bright_min", static_aidrc->blend_luma_bright_min, hi_u8);
    scene_load_int(module, "static_aidrc:blend_luma_bright_threshold", static_aidrc->blend_luma_bright_threshold,
        hi_u8);
    scene_load_int(module, "static_aidrc:blend_luma_dark_min", static_aidrc->blend_luma_dark_min, hi_u8);
    scene_load_int(module, "static_aidrc:blend_luma_dark_threshold", static_aidrc->blend_luma_dark_threshold, hi_u8);
    scene_load_int(module, "static_aidrc:blend_detail_max", static_aidrc->blend_detail_max, hi_u8);
    scene_load_int(module, "static_aidrc:blend_detail_bright_min", static_aidrc->blend_detail_bright_min, hi_u8);
    scene_load_int(module, "static_aidrc:blend_detail_bright_threshold", static_aidrc->blend_detail_bright_threshold,
        hi_u8);
    scene_load_int(module, "static_aidrc:blend_detail_dark_min", static_aidrc->blend_detail_dark_min, hi_u8);
    scene_load_int(module, "static_aidrc:blend_detail_dark_threshold", static_aidrc->blend_detail_dark_threshold,
        hi_u8);
    scene_load_int(module, "static_aidrc:detail_adjust_coef_blend", static_aidrc->detail_adjust_coef_blend, hi_u8);
    return HI_SUCCESS;
}


static hi_s32 scene_load_static_post_snr_arrays(const hi_char *module, ot_scene_static_nr *static_nr)
{
    scene_load_array(module, "static_nr:dering_strength", static_nr->dering_snr_cfg.dering_strength,
        HI_ISP_AUTO_ISO_NUM, hi_u16);
    scene_load_array(module, "static_nr:dering_thresh", static_nr->dering_snr_cfg.dering_thresh,
        HI_ISP_AUTO_ISO_NUM, hi_u8);
    scene_load_array(module, "static_nr:dering_static_strength", static_nr->dering_snr_cfg.dering_static_strength,
        HI_ISP_AUTO_ISO_NUM, hi_u8);
    scene_load_array(module, "static_nr:dering_motion_strength", static_nr->dering_snr_cfg.dering_motion_strength,
        HI_ISP_AUTO_ISO_NUM, hi_u8);

    return HI_SUCCESS;
}

static hi_s32 scene_load_static_nr_arrays(const hi_char *module, ot_scene_static_nr *static_nr)
{
    hi_s32 ret;
    scene_load_array(module, "static_nr:sfm6_strength", static_nr->snr_cfg.sfm6_strength,
        HI_ISP_AUTO_ISO_NUM, hi_u8);
    scene_load_array(module, "static_nr:sfm7_strength", static_nr->snr_cfg.sfm7_strength,
        HI_ISP_AUTO_ISO_NUM, hi_u8);
    scene_load_array(module, "static_nr:sth", static_nr->snr_cfg.sth,
        HI_ISP_AUTO_ISO_NUM, hi_u8);
    scene_load_array(module, "static_nr:sfm1_adp_strength", static_nr->snr_cfg.sfm1_adp_strength,
        HI_ISP_AUTO_ISO_NUM, hi_u8);
    scene_load_array(module, "static_nr:sfm1_strength", static_nr->snr_cfg.sfm1_strength,
        HI_ISP_AUTO_ISO_NUM, hi_u16);
    scene_load_array(module, "static_nr:sfm0_detail_prot", static_nr->snr_cfg.sfm0_detail_prot,
        HI_ISP_AUTO_ISO_NUM, hi_u8);
    scene_load_array(module, "static_nr:fine_strength", static_nr->snr_cfg.fine_strength, HI_ISP_AUTO_ISO_NUM, hi_u8);
    scene_load_array(module, "static_nr:coring_wgt", static_nr->snr_cfg.coring_wgt, HI_ISP_AUTO_ISO_NUM, hi_u16);
    scene_load_array(module, "static_nr:coring_mot_ratio", static_nr->snr_cfg.coring_mot_ratio,
        HI_ISP_AUTO_ISO_NUM, hi_u8);
    scene_load_array(module, "static_nr:md_mode", static_nr->tnr_cfg.md_mode, HI_ISP_AUTO_ISO_NUM, hi_bool);
    scene_load_array(module, "static_nr:md_anti_flicker_strength", static_nr->tnr_cfg.md_anti_flicker_strength,
        HI_ISP_AUTO_ISO_NUM, hi_u8);
    scene_load_array(module, "static_nr:md_static_ratio", static_nr->tnr_cfg.md_static_ratio,
        HI_ISP_AUTO_ISO_NUM, hi_u8);
    scene_load_array(module, "static_nr:md_motion_ratio", static_nr->tnr_cfg.md_motion_ratio,
        HI_ISP_AUTO_ISO_NUM, hi_u8);
    scene_load_array(module, "static_nr:md_static_fine_strength", static_nr->tnr_cfg.md_static_fine_strength,
        HI_ISP_AUTO_ISO_NUM, hi_u8);
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

    ret = scene_load_static_post_snr_arrays(module, static_nr);
    ot_scenecomm_check_return(ret, ret);

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

    scene_load_array(module, "static_nr:coring_ratio", static_nr->coring_ratio, HI_ISP_BAYERNR_LUT_LENGTH, hi_u16);
    scene_load_array(module, "static_nr:mix_gain", static_nr->mix_gain, HI_ISP_BAYERNR_LUT_LENGTH1, hi_u16);

    for (i = 0; i < HI_ISP_BAYER_CHN_NUM; i++) {
        snprintf_truncated_s(node, SCENE_INIPARAM_NODE_NAME_LEN, "static_nr:sfm0_coarse_strength_%u", i);
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
    scene_load_array(module, "static_ca:y_sat_lut", static_ca->y_sat_lut, HI_ISP_CA_YRATIO_LUT_LENGTH, hi_u32);

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

static hi_s32 scene_load_static_cac(const hi_char *module, ot_scene_static_cac *static_cac)
{
    ot_scenecomm_check_pointer_return(static_cac, HI_FAILURE);
    hi_s32 ret;
    hi_s32 get_value;

    scene_load_int(module, "static_cac:enable", static_cac->enable, hi_bool);
    scene_load_int(module, "static_cac:op_type", static_cac->op_type, hi_op_mode);
    scene_load_int(module, "static_cac:detect_mode", static_cac->detect_mode, hi_bool);
    scene_load_int(module, "static_cac:purple_upper_limit", static_cac->purple_upper_limit, hi_s16);
    scene_load_int(module, "static_cac:purple_lower_limit", static_cac->purple_lower_limit, hi_s16);
    scene_load_int(module, "static_cac:purple_detect_range", static_cac->purple_detect_range, hi_u16);
    scene_load_int(module, "static_cac:var_threshold", static_cac->var_threshold, hi_u16);

    scene_load_array(module, "static_cac:edge_threshold_0", static_cac->edge_threshold[0],
        HI_ISP_AUTO_ISO_NUM, hi_u16);
    scene_load_array(module, "static_cac:edge_threshold_1", static_cac->edge_threshold[1],
        HI_ISP_AUTO_ISO_NUM, hi_u16);
    scene_load_array(module, "static_cac:edge_gain", static_cac->edge_gain,
        HI_ISP_AUTO_ISO_NUM, hi_u16);
    scene_load_array(module, "static_cac:cac_rb_strength", static_cac->cac_rb_strength,
        HI_ISP_AUTO_ISO_NUM, hi_u16);
    scene_load_array(module, "static_cac:purple_alpha", static_cac->purple_alpha,
        HI_ISP_AUTO_ISO_NUM, hi_u16);
    scene_load_array(module, "static_cac:edge_alpha", static_cac->edge_alpha,
        HI_ISP_AUTO_ISO_NUM, hi_u16);
    scene_load_array(module, "static_cac:satu_low_threshold", static_cac->satu_low_threshold,
        HI_ISP_AUTO_ISO_NUM, hi_u16);
    scene_load_array(module, "static_cac:satu_high_threshold", static_cac->satu_high_threshold,
        HI_ISP_AUTO_ISO_NUM, hi_u16);

    scene_load_array(module, "static_cac:de_purple_cr_strength",
        static_cac->de_purple_cr_strength, HI_ISP_LCAC_EXP_RATIO_NUM, hi_u8);
    scene_load_array(module, "static_cac:de_purple_cb_strength",
        static_cac->de_purple_cb_strength, HI_ISP_LCAC_EXP_RATIO_NUM, hi_u8);
    scene_load_array(module, "static_cac:r_detect_threshold", static_cac->r_detect_threshold,
        HI_ISP_LCAC_DET_NUM, hi_u16);
    scene_load_array(module, "static_cac:g_detect_threshold", static_cac->g_detect_threshold,
        HI_ISP_LCAC_DET_NUM, hi_u16);
    scene_load_array(module, "static_cac:b_detect_threshold", static_cac->b_detect_threshold,
        HI_ISP_LCAC_DET_NUM, hi_u16);

    return HI_SUCCESS;
}


static hi_s32 scene_load_static_dpc(const hi_char *module, ot_scene_static_dpc *static_dpc)
{
    ot_scenecomm_check_pointer_return(static_dpc, HI_FAILURE);
    hi_s32 ret;
    hi_s32 get_value;

    scene_load_int(module, "static_dpc:enable", static_dpc->enable, hi_bool);

    scene_load_array(module, "static_dpc:op_type", static_dpc->op_type, HI_ISP_WDR_MAX_FRAME_NUM, hi_op_mode);
    scene_load_array(module, "static_dpc:strength_0", static_dpc->strength[0], HI_ISP_AUTO_ISO_NUM, hi_u16);
    scene_load_array(module, "static_dpc:blend_ratio_0", static_dpc->blend_ratio[0], HI_ISP_AUTO_ISO_NUM, hi_u16);
    scene_load_array(module, "static_dpc:strength_1", static_dpc->strength[1], HI_ISP_AUTO_ISO_NUM, hi_u16);
    scene_load_array(module, "static_dpc:blend_ratio_1", static_dpc->blend_ratio[1], HI_ISP_AUTO_ISO_NUM, hi_u16);
    scene_load_array(module, "static_dpc:strength_2", static_dpc->strength[2], HI_ISP_AUTO_ISO_NUM, hi_u16);
    scene_load_array(module, "static_dpc:blend_ratio_2", static_dpc->blend_ratio[2], HI_ISP_AUTO_ISO_NUM, hi_u16);
    scene_load_array(module, "static_dpc:strength_3", static_dpc->strength[3], HI_ISP_AUTO_ISO_NUM, hi_u16);
    scene_load_array(module, "static_dpc:blend_ratio_3", static_dpc->blend_ratio[3], HI_ISP_AUTO_ISO_NUM, hi_u16);

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
    scene_load_int(module, "static_csc:limited_range_en", static_csc->limited_range_en, hi_bool);
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

    scene_load_array(module, "static_bayershp:dark_threshold",
        static_bayershp->dark_threshold, OT_ISP_BSHP_THD_NUM, hi_u16);
    scene_load_array(module, "static_bayershp:texture_threshold",
        static_bayershp->texture_threshold, OT_ISP_BSHP_THD_NUM, hi_u16);

    for (i = 0; i < HI_SCENE_BSHP_CURVE_NUM; i++) {
        snprintf_truncated_s(node, SCENE_INIPARAM_NODE_NAME_LEN, "static_bayershp:mf_strength_%u", i);
        scene_load_array(module, node, static_bayershp->bayershp_auto_attr.mf_strength[i],
            HI_ISP_AUTO_ISO_NUM, hi_u8);
    }

    for (i = 0; i < HI_SCENE_BSHP_CURVE_NUM; i++) {
        snprintf_truncated_s(node, SCENE_INIPARAM_NODE_NAME_LEN, "static_bayershp:hf_strength_%u", i);
        scene_load_array(module, node, static_bayershp->bayershp_auto_attr.hf_strength[i],
            HI_ISP_AUTO_ISO_NUM, hi_u8);
    }

    for (i = 0; i < HI_SCENE_BSHP_CURVE_NUM; i++) {
        snprintf_truncated_s(node, SCENE_INIPARAM_NODE_NAME_LEN, "static_bayershp:dark_strength_%u", i);
        scene_load_array(module, node, static_bayershp->bayershp_auto_attr.dark_strength[i],
            HI_ISP_AUTO_ISO_NUM, hi_u8);
    }

    scene_load_array(module, "static_bayershp:mf_gain",
        static_bayershp->bayershp_auto_attr.mf_gain, HI_ISP_AUTO_ISO_NUM, hi_u8);
    scene_load_array(module, "static_bayershp:hf_gain",
        static_bayershp->bayershp_auto_attr.hf_gain, HI_ISP_AUTO_ISO_NUM, hi_u8);
    scene_load_array(module, "static_bayershp:dark_gain", static_bayershp->bayershp_auto_attr.dark_gain,
        HI_ISP_AUTO_ISO_NUM, hi_u8);
    scene_load_array(module, "static_bayershp:overshoot", static_bayershp->bayershp_auto_attr.overshoot,
        HI_ISP_AUTO_ISO_NUM, hi_u16);
    scene_load_array(module, "static_bayershp:undershoot", static_bayershp->bayershp_auto_attr.undershoot,
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
    scene_load_int(module, "static_sharpen:skin_umin", static_sharpen->skin_umin, hi_u8);
    scene_load_int(module, "static_sharpen:skin_vmin", static_sharpen->skin_vmin, hi_u8);
    scene_load_int(module, "static_sharpen:skin_umax", static_sharpen->skin_umax, hi_u8);
    scene_load_int(module, "static_sharpen:skin_vmax", static_sharpen->skin_vmax, hi_u8);

    for (i = 0; i < HI_ISP_SHARPEN_GAIN_NUM; i++) {
        snprintf_truncated_s(node, SCENE_INIPARAM_NODE_NAME_LEN, "static_sharpen:luma_wgt_%u", i);
        scene_load_array(module, node, static_sharpen->sharpen_auto_cfg.luma_wgt[i], HI_ISP_AUTO_ISO_NUM, hi_u8);
    }

    for (i = 0; i < HI_ISP_SHARPEN_GAIN_NUM; i++) {
        snprintf_truncated_s(node, SCENE_INIPARAM_NODE_NAME_LEN, "static_sharpen:texture_strength_%u", i);
        scene_load_array(module, node, static_sharpen->sharpen_auto_cfg.texture_strength[i],
            HI_ISP_AUTO_ISO_NUM, hi_u16);
    }

    for (i = 0; i < HI_ISP_SHARPEN_GAIN_NUM; i++) {
        snprintf_truncated_s(node, SCENE_INIPARAM_NODE_NAME_LEN, "static_sharpen:edge_strength_%u", i);
        scene_load_array(module, node, static_sharpen->sharpen_auto_cfg.edge_strength[i], HI_ISP_AUTO_ISO_NUM, hi_u16);
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
    scene_load_array(module, "static_blc:user_offset", static_blc->user_offset, HI_ISP_BAYER_CHN_NUM, hi_u16);

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

static hi_s32 scene_load_dynamic_fps(const hi_char *module, ot_scene_dynamic_fps *dynamic_fps)
{
    ot_scenecomm_check_pointer_return(dynamic_fps, HI_FAILURE);
    hi_s32 ret;
    hi_s32 get_value;

    scene_load_int(module, "dynamic_fps:fps_exposure_cnt", dynamic_fps->fps_exposure_cnt, hi_u8);
    scene_load_array(module, "dynamic_fps:exp_ltoh_thresh", dynamic_fps->exp_ltoh_thresh,
        dynamic_fps->fps_exposure_cnt, hi_u64);
    scene_load_array(module, "dynamic_fps:exp_htol_thresh", dynamic_fps->exp_htol_thresh,
        dynamic_fps->fps_exposure_cnt, hi_u64);

    scene_load_array(module, "dynamic_fps:fps", dynamic_fps->fps, dynamic_fps->fps_exposure_cnt, hi_u32);
    scene_load_array(module, "dynamic_fps:venc_gop", dynamic_fps->venc_gop, dynamic_fps->fps_exposure_cnt, hi_u32);
    scene_load_array(module, "dynamic_fps:ae_max_time", dynamic_fps->ae_max_time,
        dynamic_fps->fps_exposure_cnt, hi_u32);

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
        snprintf_truncated_s(node, SCENE_INIPARAM_NODE_NAME_LEN, "dynamic_drc:local_mixing_bright_%u", i);
        scene_load_array(module, node, dynamic_drc->local_mixing_bright[i], dynamic_drc->iso_count, hi_u8);

        snprintf_truncated_s(node, SCENE_INIPARAM_NODE_NAME_LEN, "dynamic_drc:local_mixing_dark_%u", i);
        scene_load_array(module, node, dynamic_drc->local_mixing_dark[i], dynamic_drc->iso_count, hi_u8);

        snprintf_truncated_s(node, SCENE_INIPARAM_NODE_NAME_LEN, "dynamic_drc:color_correction_lut_%u", i);
        scene_load_array(module, node, dynamic_drc->color_correction_lut[i], dynamic_drc->ratio_count, hi_u16);
    }

    scene_load_array(module, "dynamic_drc:tone_mapping_wgt_x", dynamic_drc->tone_mapping_wgt_x,
        dynamic_drc->iso_count, hi_u8);
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
    scene_load_array(module, "dynamic_drc:dark_gain_limit_luma", dynamic_drc->dark_gain_limit_luma,
        dynamic_drc->iso_count, hi_u8);
    scene_load_array(module, "dynamic_drc:dark_gain_limit_chroma", dynamic_drc->dark_gain_limit_chroma,
        dynamic_drc->iso_count, hi_u8);
    scene_load_array(module, "dynamic_drc:global_color_ctrl", dynamic_drc->global_color_ctrl,
        dynamic_drc->iso_count, hi_u8);
    scene_load_array(module, "dynamic_drc:shoot_reduction_en", dynamic_drc->shoot_reduction_en,
        dynamic_drc->iso_count, hi_bool);

    scene_load_array(module, "dynamic_drc:manual_str", dynamic_drc->manual_str, dynamic_drc->iso_count, hi_u16);
    scene_load_array(module, "dynamic_drc:tm_value_low",   dynamic_drc->tm_value_low,  HI_ISP_DRC_TM_NODE_NUM, hi_u16);
    scene_load_array(module, "dynamic_drc:tm_value_high",  dynamic_drc->tm_value_high, HI_ISP_DRC_TM_NODE_NUM, hi_u16);
    scene_load_array(module, "dynamic_drc:tm_val_higher",  dynamic_drc->tm_val_higher, HI_ISP_DRC_TM_NODE_NUM, hi_u16);
    scene_load_array(module, "dynamic_drc:tm_val_highest", dynamic_drc->tm_val_highest, HI_ISP_DRC_TM_NODE_NUM, hi_u16);

    return HI_SUCCESS;
}

static hi_s32 scene_load_dynamic_drc(const hi_char *module, const ot_scene_module_state *module_state,
    ot_scene_dynamic_drc *dynamic_drc)
{
    ot_scenecomm_check_pointer_return(dynamic_drc, HI_FAILURE);
    if (module_state->dynamic_drc == 0) {
        return HI_SUCCESS;
    }
    hi_s32 ret;
    hi_s32 get_value;

    scene_load_int(module, "dynamic_drc:ratio_count", dynamic_drc->ratio_count, hi_u32);
    scene_load_array(module, "dynamic_drc:ratio_level", dynamic_drc->ratio_level, dynamic_drc->ratio_count, hi_u32);
    scene_load_array(module, "dynamic_drc:high_saturation_color_ctrl", dynamic_drc->high_saturation_color_ctrl,
        dynamic_drc->ratio_count, hi_u8);

    scene_load_array(module, "dynamic_drc:bright_gain_limit", dynamic_drc->bright_gain_limit,
        dynamic_drc->ratio_count, hi_u8);
    scene_load_array(module, "dynamic_drc:bright_gain_limit_step", dynamic_drc->bright_gain_limit_step,
        dynamic_drc->ratio_count, hi_u8);
    scene_load_array(module, "dynamic_drc:drc_str_incre", dynamic_drc->drc_str_incre,
                     dynamic_drc->ratio_count, hi_u16);

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

static hi_s32 scene_load_dynamic_aidrc(const hi_char *module, const ot_scene_module_state *module_state,
    ot_scene_dynamic_aidrc *dynamic_aidrc)
{
    ot_scenecomm_check_pointer_return(dynamic_aidrc, HI_FAILURE);
    if (module_state->dynamic_aidrc == 0) {
        return HI_SUCCESS;
    }

    hi_s32 ret;
    hi_s32 get_value;
    hi_u32 i;
    hi_char node[SCENE_INIPARAM_NODE_NAME_LEN] = { 0 };

    scene_load_int(module, "dynamic_aidrc:ratio_count", dynamic_aidrc->ratio_count, hi_u32);
    scene_load_array(module, "dynamic_aidrc:ratio_level", dynamic_aidrc->ratio_level,
        dynamic_aidrc->ratio_count, hi_u32);
    scene_load_array(module, "dynamic_aidrc:blend_luma_max", dynamic_aidrc->blend_luma_max,
        dynamic_aidrc->ratio_count, hi_u8);
    scene_load_array(module, "dynamic_aidrc:detail_adjust_coef_x", dynamic_aidrc->detail_adjust_coef_x,
        dynamic_aidrc->ratio_count, hi_u8);
    scene_load_array(module, "dynamic_aidrc:detail_adjust_coef_blend", dynamic_aidrc->detail_adjust_coef_blend,
        dynamic_aidrc->ratio_count, hi_u8);
    scene_load_array(module, "dynamic_aidrc:aidrc_strength", dynamic_aidrc->aidrc_strength,
        dynamic_aidrc->ratio_count, hi_u32);
    scene_load_array(module, "dynamic_aidrc:aidrc_threshold", dynamic_aidrc->aidrc_threshold,
        dynamic_aidrc->ratio_count, hi_u32);

    scene_load_int(module, "dynamic_aidrc:iso_count", dynamic_aidrc->iso_count, hi_u32);
    scene_load_int(module, "dynamic_aidrc:interval", dynamic_aidrc->interval, hi_u32);
    scene_load_array(module, "dynamic_aidrc:iso_level", dynamic_aidrc->iso_level, dynamic_aidrc->iso_count, hi_u32);

    for (i = 0; i < HI_ISP_DRC_LMIX_NODE_NUM; i++) {
        snprintf_truncated_s(node, SCENE_INIPARAM_NODE_NAME_LEN, "dynamic_aidrc:local_mixing_bright_x_%u", i);
        scene_load_array(module, node, dynamic_aidrc->local_mixing_bright_x[i], dynamic_aidrc->iso_count, hi_u8);
        snprintf_truncated_s(node, SCENE_INIPARAM_NODE_NAME_LEN, "dynamic_aidrc:local_mixing_dark_x_%u", i);
        scene_load_array(module, node, dynamic_aidrc->local_mixing_dark_x[i], dynamic_aidrc->iso_count, hi_u8);
    }
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

static hi_s32 scene_load_dynamic_linear_drc(const hi_char *module, const ot_scene_module_state *module_state,
    ot_scene_dynamic_linear_drc *dynamic_linear_drc)
{
    ot_scenecomm_check_pointer_return(dynamic_linear_drc, HI_FAILURE);
    if (module_state->dynamic_linear_drc == 0) {
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
        snprintf_truncated_s(node, SCENE_INIPARAM_NODE_NAME_LEN, "dynamic_linear_drc:local_mixing_bright_%u", i);
        scene_load_array(module, node, dynamic_linear_drc->local_mixing_bright[i],
            dynamic_linear_drc->iso_count, hi_u8);

        snprintf_truncated_s(node, SCENE_INIPARAM_NODE_NAME_LEN, "dynamic_linear_drc:local_mixing_dark_%u", i);
        scene_load_array(module, node, dynamic_linear_drc->local_mixing_dark[i], dynamic_linear_drc->iso_count, hi_u8);
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

    scene_load_int(module, "dynamic_gamma:total_num", dynamic_gamma->total_num, hi_u32);
    scene_load_int(module, "dynamic_gamma:interval", dynamic_gamma->interval, hi_u32);

    scene_load_array(module, "dynamic_gamma:exp_thresh_ltoh", dynamic_gamma->exp_thresh_ltoh,
        dynamic_gamma->total_num, hi_u64);

    scene_load_array(module, "dynamic_gamma:exp_thresh_htol", dynamic_gamma->exp_thresh_htol,
        dynamic_gamma->total_num, hi_u64);

    /* table */
    for (i = 0; i < dynamic_gamma->total_num; i++) {
        snprintf_truncated_s(node, SCENE_INIPARAM_NODE_NAME_LEN, "dynamic_gamma:table_%u", i);
        scene_load_array(module, node, dynamic_gamma->table[i], HI_ISP_GAMMA_NODE_NUM, hi_u16);
    }
    return HI_SUCCESS;
}

static hi_s32 scene_load_dynamic_nr_arrays_part1(const hi_char *module, ot_scene_dynamic_nr *dynamic_nr)
{
    hi_u32 i;
    hi_char node[SCENE_INIPARAM_NODE_NAME_LEN] = { 0 };

    for (i = 0; i < HI_SCENE_NR_LUT_LENGTH1; i++) {
        snprintf_truncated_s(node, SCENE_INIPARAM_NODE_NAME_LEN, "dynamic_nr:mix_gain_%u", i);
        scene_load_array(module, node, dynamic_nr->mix_gain[i], dynamic_nr->ratio_count, hi_u16);
    }

    for (i = 0; i < HI_ISP_WDR_MAX_FRAME_NUM; i++) {
        snprintf_truncated_s(node, SCENE_INIPARAM_NODE_NAME_LEN, "dynamic_nr:snr_sfm0_wdr_frame_str_%u", i);
        scene_load_array(module, node, dynamic_nr->snr_sfm0_wdr_frame_str[i], dynamic_nr->coring_ratio_count, hi_u8);
    }

    for (i = 0; i < HI_ISP_WDR_MAX_FRAME_NUM; i++) {
        snprintf_truncated_s(node, SCENE_INIPARAM_NODE_NAME_LEN, "dynamic_nr:snr_sfm0_fusion_frame_str_%u", i);
        scene_load_array(module, node, dynamic_nr->snr_sfm0_fusion_frame_str[i], dynamic_nr->coring_ratio_count, hi_u8);
    }

    for (i = 0; i < HI_ISP_WDR_MAX_FRAME_NUM; i++) {
        snprintf_truncated_s(node, SCENE_INIPARAM_NODE_NAME_LEN, "dynamic_nr:md_wdr_frame_str_%u", i);
        scene_load_array(module, node, dynamic_nr->md_wdr_frame_str[i], dynamic_nr->coring_ratio_count, hi_u8);
    }

    for (i = 0; i < HI_ISP_WDR_MAX_FRAME_NUM; i++) {
        snprintf_truncated_s(node, SCENE_INIPARAM_NODE_NAME_LEN, "dynamic_nr:md_fusion_frame_str_%u", i);
        scene_load_array(module, node, dynamic_nr->md_fusion_frame_str[i], dynamic_nr->coring_ratio_count, hi_u8);
    }

    for (i = 0; i < HI_ISP_WDR_MAX_FRAME_NUM; i++) {
        snprintf_truncated_s(node, SCENE_INIPARAM_NODE_NAME_LEN, "dynamic_nr:snr_wdr_sfm6_strength_%u", i);
        scene_load_array(module, node, dynamic_nr->snr_wdr_sfm6_strength[i], dynamic_nr->coring_ratio_count, hi_u8);
    }

    for (i = 0; i < HI_ISP_WDR_MAX_FRAME_NUM; i++) {
        snprintf_truncated_s(node, SCENE_INIPARAM_NODE_NAME_LEN, "dynamic_nr:snr_wdr_sfm7_strength_%u", i);
        scene_load_array(module, node, dynamic_nr->snr_wdr_sfm7_strength[i], dynamic_nr->coring_ratio_count, hi_u8);
    }

    for (i = 0; i < HI_ISP_WDR_MAX_FRAME_NUM; i++) {
        snprintf_truncated_s(node, SCENE_INIPARAM_NODE_NAME_LEN, "dynamic_nr:snr_fusion_sfm6_strength_%u", i);
        scene_load_array(module, node, dynamic_nr->snr_fusion_sfm6_strength[i], dynamic_nr->coring_ratio_count, hi_u8);
    }

    for (i = 0; i < HI_ISP_WDR_MAX_FRAME_NUM; i++) {
        snprintf_truncated_s(node, SCENE_INIPARAM_NODE_NAME_LEN, "dynamic_nr:snr_fusion_sfm7_strength_%u", i);
        scene_load_array(module, node, dynamic_nr->snr_fusion_sfm7_strength[i], dynamic_nr->coring_ratio_count, hi_u8);
    }

    return HI_SUCCESS;
}

static hi_s32 scene_load_dynamic_nr_arrays(const hi_char *module, ot_scene_dynamic_nr *dynamic_nr)
{
    hi_u32 i;
    hi_char node[SCENE_INIPARAM_NODE_NAME_LEN] = { 0 };

    for (i = 0; i < HI_ISP_WDR_MAX_FRAME_NUM; i++) {
        snprintf_truncated_s(node, SCENE_INIPARAM_NODE_NAME_LEN, "dynamic_nr:md_fusion_frame_str_incr_%u", i);
        scene_load_array(module, node, dynamic_nr->md_fusion_frame_str_incr[i], dynamic_nr->ratio_count, hi_u8);
    }

    for (i = 0; i < HI_ISP_WDR_MAX_FRAME_NUM; i++) {
        snprintf_truncated_s(node, SCENE_INIPARAM_NODE_NAME_LEN, "dynamic_nr:snr_sfm0_fusion_frame_str_incr_%u", i);
        scene_load_array(module, node, dynamic_nr->snr_sfm0_fusion_frame_str_incr[i], dynamic_nr->ratio_count, hi_u8);
    }

    for (i = 0; i < HI_ISP_WDR_MAX_FRAME_NUM; i++) {
        snprintf_truncated_s(node, SCENE_INIPARAM_NODE_NAME_LEN, "dynamic_nr:md_wdr_frame_str_incr_%u", i);
        scene_load_array(module, node, dynamic_nr->md_wdr_frame_str_incr[i], dynamic_nr->ratio_count, hi_u8);
    }

    for (i = 0; i < HI_ISP_WDR_MAX_FRAME_NUM; i++) {
        snprintf_truncated_s(node, SCENE_INIPARAM_NODE_NAME_LEN, "dynamic_nr:snr_sfm0_wdr_frame_str_incr_%u", i);
        scene_load_array(module, node, dynamic_nr->snr_sfm0_wdr_frame_str_incr[i], dynamic_nr->ratio_count, hi_u8);
    }

    for (i = 0; i < HI_ISP_WDR_MAX_FRAME_NUM; i++) {
        snprintf_truncated_s(node, SCENE_INIPARAM_NODE_NAME_LEN,
            "dynamic_nr:snr_fusion_sfm6_sfm7_frame_incr_%u", i);
        scene_load_array(module, node,
        dynamic_nr->snr_fusion_sfm6_sfm7_frame_incr[i], dynamic_nr->ratio_count, hi_u8);
    }

    for (i = 0; i < HI_ISP_WDR_MAX_FRAME_NUM; i++) {
        snprintf_truncated_s(node, SCENE_INIPARAM_NODE_NAME_LEN, "dynamic_nr:snr_wdr_sfm6_sfm7_frame_incr_%u", i);
        scene_load_array(module, node, dynamic_nr->snr_wdr_sfm6_sfm7_frame_incr[i], dynamic_nr->ratio_count, hi_u8);
    }

    for (i = 0; i < HI_SCENE_NR_LUT_LENGTH; i++) {
        snprintf_truncated_s(node, SCENE_INIPARAM_NODE_NAME_LEN, "dynamic_nr:coring_ratio_%u", i);
        scene_load_array(module, node, dynamic_nr->coring_ratio[i], dynamic_nr->coring_ratio_count, hi_u16);
    }

    scene_load_dynamic_nr_arrays_part1(module, dynamic_nr);

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

    scene_load_int(module, "dynamic_nr:wdr_ratio_threshold", dynamic_nr->wdr_ratio_threshold, hi_u16);

    scene_load_array(module, "dynamic_nr:coring_wgt_l", dynamic_nr->coring_wgt_l, HI_ISP_AUTO_ISO_NUM, hi_u16);
    scene_load_array(module, "dynamic_nr:coring_wgt_h", dynamic_nr->coring_wgt_h, HI_ISP_AUTO_ISO_NUM, hi_u16);

    scene_load_array(module, "dynamic_nr:fine_strength_l", dynamic_nr->fine_strength_l, HI_ISP_AUTO_ISO_NUM, hi_u16);
    scene_load_array(module, "dynamic_nr:fine_strength_h", dynamic_nr->fine_strength_h, HI_ISP_AUTO_ISO_NUM, hi_u16);

    return HI_SUCCESS;
}

static hi_s32 scene_load_dynamic_ca(const hi_char *module, ot_scene_dynamic_ca *dynamic_ca)
{
    ot_scenecomm_check_pointer_return(dynamic_ca, HI_FAILURE);
    hi_s32 ret;
    hi_s32 get_value;
    hi_u32 i;
    hi_char node[SCENE_INIPARAM_NODE_NAME_LEN] = { 0 };

    scene_load_int(module, "dynamic_ca:iso_count", dynamic_ca->iso_count, hi_u32);
    scene_load_array(module, "dynamic_ca:iso_level", dynamic_ca->iso_level, dynamic_ca->iso_count, hi_u32);

    for (i = 0; i < dynamic_ca->iso_count; i++) {
        snprintf_truncated_s(node, SCENE_INIPARAM_NODE_NAME_LEN, "dynamic_ca:ca_y_ratio_lut_iso_%u", i);
        scene_load_array(module, node, dynamic_ca->ca_y_ratio_iso_lut[i], HI_ISP_CA_YRATIO_LUT_LENGTH, hi_u32);
    }

    scene_load_int(module, "dynamic_ca:ratio_count", dynamic_ca->ratio_count, hi_u32);
    scene_load_array(module, "dynamic_ca:ratio_level", dynamic_ca->ratio_level, dynamic_ca->ratio_count, hi_u32);
    scene_load_array(module, "dynamic_ca:blend_weight", dynamic_ca->blend_weight, dynamic_ca->ratio_count, hi_u32);

    for (i = 0; i < dynamic_ca->ratio_count; i++) {
        snprintf_truncated_s(node, SCENE_INIPARAM_NODE_NAME_LEN, "dynamic_ca:ca_y_ratio_lut_ratio_%u", i);
        scene_load_array(module, node, dynamic_ca->ca_y_ratio_ratio_lut[i], HI_ISP_CA_YRATIO_LUT_LENGTH, hi_u32);
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
    hi_u32 i;
    hi_char node[SCENE_INIPARAM_NODE_NAME_LEN] = { 0 };

    scene_load_int(module, "dynamic_dpc:iso_count", dynamic_dpc->iso_count, hi_u8);
    scene_load_array(module, "dynamic_dpc:iso_level", dynamic_dpc->iso_level, dynamic_dpc->iso_count, hi_u32);

    for (i = 0; i < HI_ISP_WDR_MAX_FRAME_NUM; i++) {
        snprintf_truncated_s(node, SCENE_INIPARAM_NODE_NAME_LEN, "dynamic_dpc:sup_twinkle_en_%u", i);
        scene_load_array(module, node, dynamic_dpc->sup_twinkle_en[i], dynamic_dpc->iso_count, hi_u8);
    }

    for (i = 0; i < HI_ISP_WDR_MAX_FRAME_NUM; i++) {
        snprintf_truncated_s(node, SCENE_INIPARAM_NODE_NAME_LEN, "dynamic_dpc:soft_thr_%u", i);
        scene_load_array(module, node, dynamic_dpc->soft_thr[i], dynamic_dpc->iso_count, hi_u8);
    }

    for (i = 0; i < HI_ISP_WDR_MAX_FRAME_NUM; i++) {
        snprintf_truncated_s(node, SCENE_INIPARAM_NODE_NAME_LEN, "dynamic_dpc:soft_slope_%u", i);
        scene_load_array(module, node, dynamic_dpc->soft_slope[i], dynamic_dpc->iso_count, hi_u8);
    }

    for (i = 0; i < HI_ISP_WDR_MAX_FRAME_NUM; i++) {
        snprintf_truncated_s(node, SCENE_INIPARAM_NODE_NAME_LEN, "dynamic_dpc:bright_strength_%u", i);
        scene_load_array(module, node, dynamic_dpc->bright_strength[i], dynamic_dpc->iso_count, hi_u8);
    }

    for (i = 0; i < HI_ISP_WDR_MAX_FRAME_NUM; i++) {
        snprintf_truncated_s(node, SCENE_INIPARAM_NODE_NAME_LEN, "dynamic_dpc:dark_strength_%u", i);
        scene_load_array(module, node, dynamic_dpc->dark_strength[i], dynamic_dpc->iso_count, hi_u8);
    }

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

    scene_load_int(module, "dynamic_fswdr:iso_count", dynamic_fswdr->iso_count, hi_u8);
    scene_load_int(module, "dynamic_fswdr:ratio_thr", dynamic_fswdr->ratio_thr, hi_u32);
    scene_load_array(module, "dynamic_fswdr:iso_level",
        dynamic_fswdr->iso_ltoh_thresh, dynamic_fswdr->iso_count, hi_u32);
    scene_load_array(module, "dynamic_fswdr:fusion_blend_en",
        dynamic_fswdr->fusion_blend_en, dynamic_fswdr->iso_count, hi_bool);
    scene_load_array(module, "dynamic_fswdr:fusion_blend_wgt",
        dynamic_fswdr->fusion_blend_wgt, dynamic_fswdr->iso_count, hi_u8);
    scene_load_array(module, "dynamic_fswdr:fusion_blend_en_h",
        dynamic_fswdr->fusion_blend_en_h, dynamic_fswdr->iso_count, hi_bool);
    scene_load_array(module, "dynamic_fswdr:fusion_blend_wgt_h",
        dynamic_fswdr->fusion_blend_wgt_h, dynamic_fswdr->iso_count, hi_u8);

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
    scene_load_array(module, "dynamic_mcf:fusion_ratio_scale", dynamic_mcf->fusion_ratio_scale,
        dynamic_mcf->iso_count, hi_u8);

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
        snprintf_truncated_s(node, SCENE_INIPARAM_NODE_NAME_LEN, "dynamic_mcf:cc_uv_gain_lut_%u", i);
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
        snprintf_truncated_s(node, SCENE_INIPARAM_NODE_NAME_LEN, "dynamic_mcf:lf_fusion_alpha_lut_%u", i);
        scene_load_array(module, node, dynamic_mcf->lf_fusion_alpha_lut[i], HI_SCENE_MCF_WEIGHT_LUT_NUM, hi_u8);
    }
    for (i = 0; i < dynamic_mcf->iso_count; i++) {
        snprintf_truncated_s(node, SCENE_INIPARAM_NODE_NAME_LEN, "dynamic_mcf:mf_fusion_alpha_lut_%u", i);
        scene_load_array(module, node, dynamic_mcf->mf_fusion_alpha_lut[i], HI_SCENE_MCF_WEIGHT_LUT_NUM, hi_u8);
    }
    for (i = 0; i < dynamic_mcf->iso_count; i++) {
        snprintf_truncated_s(node, SCENE_INIPARAM_NODE_NAME_LEN, "dynamic_mcf:hf_fusion_alpha_lut_%u", i);
        scene_load_array(module, node, dynamic_mcf->hf_fusion_alpha_lut[i], HI_SCENE_MCF_WEIGHT_LUT_NUM, hi_u8);
    }
    for (i = 0; i < dynamic_mcf->iso_count; i++) {
        snprintf_truncated_s(node, SCENE_INIPARAM_NODE_NAME_LEN, "dynamic_mcf:fusion_color_ratio_lut_%u", i);
        scene_load_array(module, node, dynamic_mcf->fusion_color_ratio_lut[i], HI_SCENE_MCF_WEIGHT_LUT_NUM, hi_u8);
    }

    for (i = 0; i < dynamic_mcf->iso_count; i++) {
        snprintf_truncated_s(node, SCENE_INIPARAM_NODE_NAME_LEN, "dynamic_mcf:lf_fusion_mono_det_lut_%u", i);
        scene_load_array(module, node, dynamic_mcf->lf_fusion_mono_det_lut[i], HI_SCENE_MCF_WEIGHT_LUT_NUM, hi_u8);
    }
    for (i = 0; i < dynamic_mcf->iso_count; i++) {
        snprintf_truncated_s(node, SCENE_INIPARAM_NODE_NAME_LEN, "dynamic_mcf:mf_fusion_mono_det_lut_%u", i);
        scene_load_array(module, node, dynamic_mcf->mf_fusion_mono_det_lut[i], HI_SCENE_MCF_WEIGHT_LUT_NUM, hi_u8);
    }
    for (i = 0; i < dynamic_mcf->iso_count; i++) {
        snprintf_truncated_s(node, SCENE_INIPARAM_NODE_NAME_LEN, "dynamic_mcf:hf_fusion_mono_det_lut_%u", i);
        scene_load_array(module, node, dynamic_mcf->hf_fusion_mono_det_lut[i], HI_SCENE_MCF_WEIGHT_LUT_NUM, hi_u8);
    }

    return HI_SUCCESS;
}

static hi_s32 scene_load_dynamic_aibnr(const hi_char *module, ot_scene_dynamic_aibnr *dynamic_aibnr)
{
    ot_scenecomm_check_pointer_return(dynamic_aibnr, HI_FAILURE);
    hi_s32 ret;
    hi_s32 get_value;

    scene_load_array(module, "dynamic_aibnr:aibnr_iso_thresh", dynamic_aibnr->aibnr_iso_thresh, AIBNR_ISO_NUM, hi_u32);
    scene_load_int(module, "dynamic_aibnr:aibnr_chg_en", dynamic_aibnr->aibnr_chg_en, hi_bool);
    scene_load_int(module, "dynamic_aibnr:blend_en", dynamic_aibnr->blend_en, hi_bool);
    scene_load_int(module, "dynamic_aibnr:op_type", dynamic_aibnr->op_type, hi_op_mode);
    scene_load_int(module, "dynamic_aibnr:manual_id", dynamic_aibnr->manual_id, hi_u8);
    scene_load_array(module, "dynamic_aibnr:auto_id", dynamic_aibnr->auto_id, HI_AIISP_AUTO_ISO_NUM, hi_u8);
    scene_load_int(module, "dynamic_aibnr:pipe_id", dynamic_aibnr->pipe_id, hi_s32);
    scene_load_int(module, "dynamic_aibnr:iso_count", dynamic_aibnr->iso_count, hi_u32);
    scene_load_array(module, "dynamic_aibnr:iso_level", dynamic_aibnr->iso_level, dynamic_aibnr->iso_count, hi_u32);
    scene_load_array(module, "dynamic_aibnr:sfs", dynamic_aibnr->sfs, dynamic_aibnr->iso_count, hi_u32);

    return HI_SUCCESS;
}

static hi_s32 scene_load_dynamic_ai3dnr(const hi_char *module, ot_scene_dynamic_ai3dnr *dynamic_ai3dnr)
{
    ot_scenecomm_check_pointer_return(dynamic_ai3dnr, HI_FAILURE);
    hi_s32 ret;
    hi_u32 i;
    hi_s32 get_value;
    hi_char node[SCENE_INIPARAM_NODE_NAME_LEN] = { 0 };

    scene_load_int(module, "dynamic_ai3dnr:iso_count", dynamic_ai3dnr->iso_count, hi_u32);
    scene_load_array(module, "dynamic_ai3dnr:ai3dnr_en", dynamic_ai3dnr->ai3dnr_en, dynamic_ai3dnr->iso_count, hi_bool);
    scene_load_array(module, "dynamic_ai3dnr:iso_level", dynamic_ai3dnr->iso_level, dynamic_ai3dnr->iso_count, hi_u32);
    scene_load_array(module, "dynamic_ai3dnr:uv_denoise_fg", dynamic_ai3dnr->uv_denoise_fg,
                     dynamic_ai3dnr->iso_count, hi_u32);
    scene_load_array(module, "dynamic_ai3dnr:uv_denoise_bg", dynamic_ai3dnr->uv_denoise_bg,
                     dynamic_ai3dnr->iso_count, hi_u32);

    for (i = 0; i < HI_SCENE_BAYERNR_DENOISE_Y_LUT_NUM; i++) {
        snprintf_truncated_s(node, SCENE_INIPARAM_NODE_NAME_LEN, "dynamic_ai3dnr:denoise_y_fg_str_lut_%u", i);
        scene_load_array(module, node, dynamic_ai3dnr->denoise_y_fg_str_lut[i], dynamic_ai3dnr->iso_count, hi_u8);
        snprintf_truncated_s(node, SCENE_INIPARAM_NODE_NAME_LEN, "dynamic_ai3dnr:denoise_y_bg_str_lut_%u", i);
        scene_load_array(module, node, dynamic_ai3dnr->denoise_y_bg_str_lut[i], dynamic_ai3dnr->iso_count, hi_u8);
    }
    return HI_SUCCESS;
}


static hi_s32 scene_load_dynamic_venc_mode(const hi_char *module, ot_scene_dynamic_venc_mode *dynamic_venc_mode)
{
    ot_scenecomm_check_pointer_return(dynamic_venc_mode, HI_FAILURE);

    scene_load_array(module, "dynamic_venc_mode:deblur_mode_iso_thresh", dynamic_venc_mode->deblur_mode_iso_thresh,
                     VENC_MODE_ISO_NUM, hi_u32);
    scene_load_array(module, "dynamic_venc_mode:deblur_en", dynamic_venc_mode->deblur_en,
                     VENC_MODE_ISO_NUM, hi_bool);
    scene_load_array(module, "dynamic_venc_mode:deblur_adaptive_en", dynamic_venc_mode->deblur_adaptive_en,
                     VENC_MODE_ISO_NUM, hi_bool);

    return HI_SUCCESS;
}


static hi_s32 scene_load_dynamic_fpn(const hi_char *module, ot_scene_dynamic_fpn *dynamic_fpn)
{
    ot_scenecomm_check_pointer_return(dynamic_fpn, HI_FAILURE);
    hi_s32 ret;
    hi_s32 get_value;

    scene_load_int(module, "dynamic_fpn:iso_count", dynamic_fpn->iso_count, hi_u32);
    scene_load_int(module, "dynamic_fpn:aibnr_mode", dynamic_fpn->aibnr_mode, hi_bool);
    scene_load_int(module, "dynamic_fpn:fpn_iso_thresh", dynamic_fpn->fpn_iso_thresh, hi_u32);
    scene_load_array(module, "dynamic_fpn:iso_thresh", dynamic_fpn->iso_thresh, dynamic_fpn->iso_count, hi_u32);
    scene_load_array(module, "dynamic_fpn:fpn_offset", dynamic_fpn->fpn_offset, dynamic_fpn->iso_count, hi_u32);
    return HI_SUCCESS;
}

#define SCENE_3DNR_ARG_LIST \
    &(p_x->nry0_en), &(p_x->nry1_en), &(p_x->nry2_en), &(p_x->nry3_en), \
    &pas[0].sfs1, &ps[1].sfs1, &ps[1].sbr1, &ps[2].sfs1, &ps[2].sbr1, &ps[3].sfs1, &ps[3].sbr1, \
    &pai[0].aux_ies, _tmprt3x(&ps, sfs2, sft2, sbr2), \
    _tmprt3x(&ps, sfs4, sft4, sbr4), \
    &pas[0].sfs5, &pas[0].sft5, &pas[0].sbr5, &ps[4].sfs2, &ps[4].sft2, &ps[4].sbr2, \
    &pai[0].ies0, &pai[0].ies1, &pai[0].ies2, &pai[0].ies3, _tmprt4_4(&pi, ies0, ies1, ies2, ies3), \
    &pai[0].o_sht, &pai[0].u_sht, &pi[3].o_sht, &pi[3].u_sht, &pi[4].o_sht, &pi[4].u_sht, \
    &ps[2].k_mode, &ps[3].k_mode, \
    &pas[0].spn3, &pas[0].sbn3, &pas[0].pbr3, \
    &pas[0].spn4, &pas[0].sbn4, &pas[0].pbr4, \
    &pas[0].spn6, &pas[0].sbn6, &pas[0].pbr6, &pas[0].j_mode6, _tmprt4_4(&ps, spn, sbn, pbr, j_mode), \
    &pas[0].sfr6[0], &pas[0].sfr6[1], &pas[0].sfr6[2], &pas[0].sfr6[3], _tmprt4_4(&ps, sfr6[0], sfr6[1], sfr6[2], sfr6[3]), \
    &pas[0].sbr6[0], &pas[0].sbr6[1], _tmprt2_4x(&ps, sbr6[0], sbr6[1]), \
    &pas[0].sfn0, &pas[0].sfn1, &pas[0].sfn2, &pas[0].sfn3, _tmprt4_4(&ps, sfn0_0, sfn1_0, sfn2_0, sfn3_0), \
    &pas[0].sth1, &pas[0].sth2, &pas[0].sth3, _tmprt3_4(&ps, sth1_0, sth2_0, sth3_0), \
    _tmprt4_2(&ps, sfn0_1, sfn1_1, sfn2_1, sfn3_1), \
    &pas[1].sfs1, _tmprt3z(&ps, 1, 2, sth1_1, sth2_1, sth3_1), \
    &pai[1].aux_ies, &ps[3].bri_idx0, &ps[3].bri_idx1, \
    &pt[1].ref_en, \
    &pai[1].ies0, &pai[1].ies1, &pai[1].ies2, &pai[1].ies3, &pt[0].auto_math, &(p_x->nrc0_en), \
    &pai[1].o_sht, &pai[1].u_sht, &pt[0].tfs0, &p_x->nrc0.pre_sfc, \
    &pm[0].dci_w, &p_x->nrc0.sfc, \
    &pas[1].spn3, &pas[1].sbn3, &pas[1].pbr3, &pt[1].tss0, &pt[1].tss1, &pt[2].tss0, &pt[2].tss1, &p_x->nrc0.sfc_enhance, &p_x->nrc0.sfc_ext, \
    &pas[1].spn4, &pas[1].sbn4, &pas[1].pbr4, &pt[1].tfs0, &pt[1].tfs1, &pt[2].tfs0, &pt[2].tfs1, &p_x->nrc0.tpc, \
    &pas[1].spn6, &pas[1].sbn6, &pas[1].pbr6, &pas[1].j_mode6, _tmprt3z(&pt, 1, 2, tfr0[0], tfr0[1], tfr0[2]), &p_x->nrc0.tfc, \
    &pas[1].sfr6[0], &pas[1].sfr6[1], &pas[1].sfr6[2], &pas[1].sfr6[3], _t3_(&pt, 1, tfr0[3], tfr0[4], tfr0[5]), _t3_(&pt, 2, tfr0[3], tfr0[4], tfr0[5]), &p_x->nrc0.trc, \
    &pas[1].sbr6[0], &pas[1].sbr6[1], _tmprt3z(&pt, 1, 2, tfr1[0], tfr1[1], tfr1[2]), \
    _tmprt3z(&pt, 1, 2, tfr1[3], tfr1[4], tfr1[5]), \
    &pas[1].sfn0, &pas[1].sfn1, &pas[1].sfn2, &pas[1].sfn3, &pt[0].math_mode, &pt[1].math_mode, &(p_x->nrc1_en), \
    &pas[1].sth1, &pas[1].sth2, &pas[1].sth3, &pc->sfs2, &pc->sft2, &pc->sbr2, \
    &pm[0].adv_math, &pm[1].adv_math, &pm[0].mcth, \
    &pas[0].bld1,  &pm[0].math0, &pm[0].math1, &pm[1].math0, &pm[1].math1, &pc->pbr0, &pc->pbr1, \
    &p_x->adv_mode, &pm[0].mate0, &pm[0].mate1, &pm[1].mate0, &pm[1].mate1, \
    &pas[0].bri_idx, &pas[1].bri_idx, &pm[0].mabw0, &pm[0].mabw1, &pm[1].mabw0, &pm[1].mabw1, \
    _t4a_0_(&ps, 3, bri_str), _t4a_4_(&ps, 3, bri_str), \
    _t4a_8_(&ps, 3, bri_str), _t4a_12_(&ps, 3, bri_str), &ps[3].bri_str[0x10], \
    _t4a_0_(&ps, 2, sbs_k), _t4a_4_(&ps, 2, sbs_k), _t4a_8_(&ps, 2, sbs_k), _t4a_12_(&ps, 2, sbs_k), \
    _t4a_10_(&ps, 2, sbs_k), _t4a_14_(&ps, 2, sbs_k), _t4a_18_(&ps, 2, sbs_k), _t4a_1c_(&ps, 2, sbs_k), &ps[2].sbs_k[0x20], \
    _t4a_0_(&ps, 2, sds_k), _t4a_4_(&ps, 2, sds_k), _t4a_8_(&ps, 2, sds_k), _t4a_12_(&ps, 2, sds_k), \
    _t4a_10_(&ps, 2, sds_k), _t4a_14_(&ps, 2, sds_k), _t4a_18_(&ps, 2, sds_k), _t4a_1c_(&ps, 2, sds_k), &ps[2].sds_k[0x20], \
    _t4a_0_(&ps, 3, sbs_k), _t4a_4_(&ps, 3, sbs_k), _t4a_8_(&ps, 3, sbs_k), _t4a_12_(&ps, 3, sbs_k), \
    _t4a_10_(&ps, 3, sbs_k), _t4a_14_(&ps, 3, sbs_k), _t4a_18_(&ps, 3, sbs_k), _t4a_1c_(&ps, 3, sbs_k), &ps[3].sbs_k[0x20], \
    _t4a_0_(&ps, 3, sds_k), _t4a_4_(&ps, 3, sds_k), _t4a_8_(&ps, 3, sds_k), _t4a_12_(&ps, 3, sds_k), \
    _t4a_10_(&ps, 3, sds_k), _t4a_14_(&ps, 3, sds_k), _t4a_18_(&ps, 3, sds_k), _t4a_1c_(&ps, 3, sds_k), &ps[3].sds_k[0x20]


static const char *g_3dnr_fmt =
    " -en          %3d:        |              %3d:      |       %3d:       |               %3d:                    |"
    " -nXsf1       %3d:        |          %3d:    %3d:  |    %3d:    %3d:  |            %3d:    %3d:               |"
    " -nXsf2       %3d:        |          %3d:%3d:%3d:  |    %3d:%3d:%3d:  |            %3d:%3d:%3d:               |"
    " -nXsf4                   |          %3d:%3d:%3d:  |    %3d:%3d:%3d:  |            %3d:%3d:%3d:               |"
    " -nXsfs5     %3d:%3d:%3d: |                        |                  |            %3d:%3d:%3d:               |"
    " -nXsf5  %3d:%3d:%3d:%3d: |        %3d:%3d:%3d:%3d:|  %3d:%3d:%3d:%3d:|  %3d:%3d:%3d:%3d: |  %3d:%3d:%3d:%3d: |"
    " -nXsht5         %3d:%3d: |                        |                  | -nXsht5  %3d:%3d: |          %3d:%3d: |"
    "                          |                        |-kmode      %3d:  |              %3d: |                   |"
    " -nXsf3      %3d:%3d:%3d: |                        |                  |                   |                   |"
    " -nXsf4      %3d:%3d:%3d: |                        |                  |                   |                   |"
    " -nXsf6  %3d:%3d:%3d:%3d: |        %3d:%3d:%3d:%3d:|  %3d:%3d:%3d:%3d:|  %3d:%3d:%3d:%3d: |  %3d:%3d:%3d:%3d: |"
    " -nXsfr6 %3d:%3d:%3d:%3d: |        %3d:%3d:%3d:%3d:|  %3d:%3d:%3d:%3d:|  %3d:%3d:%3d:%3d: |  %3d:%3d:%3d:%3d: |"
    " -nXsbr6         %3d:%3d: |             %3d:%3d:   |          %3d:%3d:|          %3d:%3d: |          %3d:%3d: |"
    "                          |                        |                  |                   |                   |"
    " -nXsfn  %3d:%3d:%3d:%3d: |        %3d:%3d:%3d:%3d:|  %3d:%3d:%3d:%3d:|  %3d:%3d:%3d:%3d: |  %3d:%3d:%3d:%3d: |"
    " -nXsth      %3d:%3d:%3d: |            %3d:%3d:%3d:|      %3d:%3d:%3d:|      %3d:%3d:%3d: |      %3d:%3d:%3d: |"
    " *************************|-nX2sfn %3d:%3d:%3d:%3d:|  %3d:%3d:%3d:%3d:|                                       |"
    " -nX2sf1      %3d:        |-nX2sth     %3d:%3d:%3d:|      %3d:%3d:%3d:|                                       |"
    " -nX2sf2      %3d:        |                        |                  |-nXbIdx           %3d:%3d:             |"
    "                          |-ref          %3d:      |                  |********************nrc0***************|"
    " -nX2sf5 %3d:%3d:%3d:%3d: |-preMath      %3d:      |                  |  -nC0en             %3d:              |"
    " -nX2sht5        %3d:%3d: |-preTFS       %3d:      |                  |  -presfc            %3d:              |"
    "                          |-dciW         %3d:      |                  |  -sfc               %3d:              |"
    " -nX2sf3     %3d:%3d:%3d: |-nXtss      %3d:%3d:    |     %3d:%3d:     |  -sfcExt          %3d:%3d:            |"
    " -nX2sf4     %3d:%3d:%3d: |-nXtfs      %3d:%3d:    |     %3d:%3d:     |  -tpc               %3d:              |"
    " -nX2sf6 %3d:%3d:%3d:%3d: |-nXtfr0   %3d:%3d:%3d:  |   %3d:%3d:%3d:   |  -tfc               %3d:              |"
    " -nX2sfr6%3d:%3d:%3d:%3d: |          %3d:%3d:%3d:  |   %3d:%3d:%3d:   |  -trc               %3d:              |"
    " -nX2sbr6        %3d:%3d: |-nXtfr1   %3d:%3d:%3d:  |   %3d:%3d:%3d:   |********************nrc1***************|"
    "                          |          %3d:%3d:%3d:  |   %3d:%3d:%3d:   |                                       |"
    " -nX2sfn %3d:%3d:%3d:%3d: |-maMode     %3d:%3d:    |                  | -nC1en              %3d:              |"
    " -nX2sth     %3d:%3d:%3d: |                        |                  | -nCsf2          %3d:%3d:%3d:          |"
    "                          |-advMath    %3d:%3d:    |-mcth    %3d:     |                                       |"
    " -mXmath             %3d: |            %3d:%3d:    |     %3d:%3d:     | -nCstr           %3d:| %3d:           |"
    " -advMode            %3d: |-mXmate     %3d:%3d:    |     %3d:%3d:     |                                       |"
    " -nXbIdx         %3d:%3d: |-mXmabw     %3d:%3d:    |     %3d:%3d:     |                                       |"
    "***************************************************************************************************************"
    " -n3bri0    %3d %3d %3d %3d   -n3bri4  %3d %3d %3d %3d                                                         "
    " -n3bri8    %3d %3d %3d %3d   -n3bri12 %3d %3d %3d %3d %3d                                                     "
    " -n2sbs0    %3d %3d %3d %3d %3d %3d %3d %3d     -n2sbs8   %3d %3d %3d %3d %3d %3d %3d %3d                      "
    " -n2sbs16   %3d %3d %3d %3d %3d %3d %3d %3d     -n2sbs24  %3d %3d %3d %3d %3d %3d %3d %3d %3d                  "
    " -n2sds0    %3d %3d %3d %3d %3d %3d %3d %3d     -n2sds8   %3d %3d %3d %3d %3d %3d %3d %3d                      "
    " -n2sds16   %3d %3d %3d %3d %3d %3d %3d %3d     -n2sds24  %3d %3d %3d %3d %3d %3d %3d %3d %3d                  "
    " -n3sbs0    %3d %3d %3d %3d %3d %3d %3d %3d     -n3sbs8   %3d %3d %3d %3d %3d %3d %3d %3d                      "
    " -n3sbs16   %3d %3d %3d %3d %3d %3d %3d %3d     -n3sbs24  %3d %3d %3d %3d %3d %3d %3d %3d %3d                  "
    " -n3sds0    %3d %3d %3d %3d %3d %3d %3d %3d     -n3sds8   %3d %3d %3d %3d %3d %3d %3d %3d                      "
    " -n3sds16   %3d %3d %3d %3d %3d %3d %3d %3d     -n3sds24  %3d %3d %3d %3d %3d %3d %3d %3d %3d                  ";


static hi_s32 scene_load_static_3dnr(const hi_char *module, const ot_scene_module_state *module_state,
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
        snprintf_truncated_s(node, SCENE_INIPARAM_NODE_NAME_LEN, "static_3dnr:3DnrParam_%u", i);
        ret = ot_confaccess_get_string(SCENE_INIPARAM, module, (const char *)node, NULL, &get_string);
        scene_iniparam_check_load_return(ret, module);

        if (get_string != NULL) {
            ot_scene_3dnr *p_x = &(static_3dnr->threednr_value[i]);
            t_v200_vpss_iey *pi = p_x->iey;
            t_v200_vpss_sfy *ps = p_x->sfy;
            t_v200_vpss_mdy *pm = p_x->mdy;
            t_v200_vpss_tfy *pt = p_x->tfy;
            t_v200_vpss_nrc1 *pc = &p_x->nrc1;

            t_v200_vpss_adv_iey *pai = p_x->adv_ie_y;
            t_v200_vpss_adv_sfy *pas = p_x->adv_sf_y;

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

    ret = scene_load_dynamic_drc(module, &scene_param->module_state, &scene_param->dynamic_drc);
    scene_iniparam_check_load_return(ret, module);
    ret = scene_load_dynamic_aidrc(module, &scene_param->module_state, &scene_param->dynamic_aidrc);
    scene_iniparam_check_load_return(ret, module);

    ret = scene_load_dynamic_linear_drc(module, &scene_param->module_state, &scene_param->dynamic_linear_drc);
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
    scene_load_module(scene_load_dynamic_aibnr, module, &scene_param->dynamic_aibnr);
    scene_load_module(scene_load_dynamic_ai3dnr, module, &scene_param->dynamic_ai3dnr);
    scene_load_module(scene_load_dynamic_fpn, module, &scene_param->dynamic_fpn);
    scene_load_module(scene_load_dynamic_venc_mode, module, &scene_param->dynamic_venc_mode);
    scene_load_module(scene_load_dynamic_fps, module, &scene_param->dynamic_fps);

    ret = scene_load_static_3dnr(module, &scene_param->module_state, &scene_param->static_threednr);
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
    scene_load_module(scene_load_static_pipe_diff, module, &scene_param->static_pipe_diff);
    scene_load_module(scene_load_static_clut, module, &scene_param->static_clut);
    scene_load_module(scene_load_static_cmm, module, &scene_param->static_ccm);
    scene_load_module(scene_load_static_saturation, module, &scene_param->static_saturation);
    scene_load_module(scene_load_static_ldci, module, &scene_param->static_ldci);
    scene_load_module(scene_load_static_pregamma, module, &scene_param->static_pregamma);
    scene_load_module(scene_load_static_blc, module, &scene_param->static_blc);
    scene_load_module(scene_load_static_drc, module, &scene_param->static_drc);
    scene_load_module(scene_load_static_aidrc, module, &scene_param->static_aidrc);
    scene_load_module(scene_load_static_nr, module, &scene_param->static_nr);
    scene_load_module(scene_load_static_ca, module, &scene_param->static_ca);
    scene_load_module(scene_load_static_venc, module, &scene_param->static_venc_attr);
    scene_load_module(scene_load_static_cac, module, &scene_param->static_cac);
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
    scene_loge("load scene[%d] \n", __LINE__);
    scene_iniparam_check_load_return(ret, module);
    return HI_SUCCESS;
}

static hi_s32 scene_load_scene_conf(ot_scene_pipe_param *scene_pipe_param, hi_s32 count)
{
    ot_scenecomm_check_pointer_return(scene_pipe_param, HI_FAILURE);
    hi_s32 ret;
    hi_s32 mode_index;
    hi_char module[SCENE_INIPARAM_MODULE_NAME_LEN] = { 0 };

    for (mode_index = 0; mode_index < count && mode_index < HI_SCENE_PIPETYPE_NUM; mode_index++) {
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
    hi_u32 i;
    hi_char node[SCENE_INIPARAM_NODE_NAME_LEN] = { 0 };

    for (i = 0; i < SCENE_MAX_VIDEOMODE; i++) {
        snprintf_truncated_s(node, SCENE_INIPARAM_NODE_NAME_LEN, "pipe_comm_%u:SCENE_MODE", i);
        scene_load_int(module, node, (video_mode->video_mode + i)->pipe_mode, hi_u8);

        for (mode_index = 0; mode_index < HI_SCENE_PIPE_MAX_NUM; mode_index++) {
            snprintf_truncated_s(node, SCENE_INIPARAM_NODE_NAME_LEN, "pipe_%u_%u:Enable", i, mode_index);
            scene_load_int(module, node, (video_mode->video_mode + i)->pipe_attr[mode_index].enable, hi_bool);

            snprintf_truncated_s(node, SCENE_INIPARAM_NODE_NAME_LEN, "pipe_%u_%u:VcapPipeHdl", i, mode_index);
            scene_load_int(module, node, (video_mode->video_mode + i)->pipe_attr[mode_index].vcap_pipe_hdl, hi_handle);

            /* SCENE_MODE */
            snprintf_truncated_s(node, SCENE_INIPARAM_NODE_NAME_LEN, "pipe_%u_%u:MainPipeHdl", i, mode_index);
            scene_load_int(module, node, (video_mode->video_mode + i)->pipe_attr[mode_index].main_pipe_hdl, hi_handle);

            snprintf_truncated_s(node, SCENE_INIPARAM_NODE_NAME_LEN, "pipe_%u_%u:PipeChnHdl", i, mode_index);
            scene_load_int(module, node, (video_mode->video_mode + i)->pipe_attr[mode_index].pipe_chn_hdl, hi_handle);

            snprintf_truncated_s(node, SCENE_INIPARAM_NODE_NAME_LEN, "pipe_%u_%u:VpssHdl", i, mode_index);
            scene_load_int(module, node, (video_mode->video_mode + i)->pipe_attr[mode_index].vpss_hdl, hi_handle);

            snprintf_truncated_s(node, SCENE_INIPARAM_NODE_NAME_LEN, "pipe_%u_%u:VportHdl", i, mode_index);
            scene_load_int(module, node, (video_mode->video_mode + i)->pipe_attr[mode_index].vport_hdl, hi_handle);

            snprintf_truncated_s(node, SCENE_INIPARAM_NODE_NAME_LEN, "pipe_%u_%u:VencHdl", i, mode_index);
            scene_load_int(module, node, (video_mode->video_mode + i)->pipe_attr[mode_index].venc_hdl, hi_handle);

            snprintf_truncated_s(node, SCENE_INIPARAM_NODE_NAME_LEN, "pipe_%u_%u:PipeParamIndex", i, mode_index);
            scene_load_int(module, node, (video_mode->video_mode + i)->pipe_attr[mode_index].pipe_param_index, hi_u8);

            snprintf_truncated_s(node, SCENE_INIPARAM_NODE_NAME_LEN, "pipe_%u_%u:PipeType", i, mode_index);
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

    printf("### ot_confaccess_init(%s) ret:%d\n", ini_path, ret);

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
