/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */

#ifndef HI_COMMON_MCF_H
#define HI_COMMON_MCF_H

#include "hi_type.h"
#include "hi_common.h"
#include "hi_common_video.h"
#include "ot_common_mcf.h"

#ifdef __cplusplus
extern "C" {
#endif

#define HI_ERR_NOT_ENCHN OT_ERR_NOT_ENCHN
#define HI_ERR_MCF_NULL_PTR OT_ERR_MCF_NULL_PTR
#define HI_ERR_MCF_NOT_READY OT_ERR_MCF_NOT_READY
#define HI_ERR_MCF_INVALID_DEVID OT_ERR_MCF_INVALID_DEVID
#define HI_ERR_MCF_INVALID_PIPEID OT_ERR_MCF_INVALID_PIPEID
#define HI_ERR_MCF_INVALID_CHNID OT_ERR_MCF_INVALID_CHNID
#define HI_ERR_MCF_EXIST OT_ERR_MCF_EXIST
#define HI_ERR_MCF_UNEXIST OT_ERR_MCF_UNEXIST
#define HI_ERR_MCF_NOT_SUPPORT OT_ERR_MCF_NOT_SUPPORT
#define HI_ERR_MCF_NOT_PERM OT_ERR_MCF_NOT_PERM
#define HI_ERR_MCF_NO_MEM OT_ERR_MCF_NO_MEM
#define HI_ERR_MCF_NO_BUF OT_ERR_MCF_NO_BUF
#define HI_ERR_MCF_ILLEGAL_PARAM OT_ERR_MCF_ILLEGAL_PARAM
#define HI_ERR_MCF_BUSY OT_ERR_MCF_BUSY
#define HI_ERR_MCF_BUF_EMPTY OT_ERR_MCF_BUF_EMPTY
#define HI_ERR_MCF_NOT_START OT_ERR_MCF_NOT_START
#define HI_ERR_MCF_NOT_STOP OT_ERR_MCF_NOT_STOP
#define HI_ERR_MCF_NOT_ENCHN OT_ERR_MCF_NOT_ENCHN
#define HI_MCF_BIAS_LUT_NUM OT_MCF_BIAS_LUT_NUM
#define HI_MCF_WEIGHT_LUT_NUM OT_MCF_WEIGHT_LUT_NUM
#define HI_MCF_CC_UV_GAIN_LUT_NUM OT_MCF_CC_UV_GAIN_LUT_NUM
#define HI_MCF_COEF_NUM OT_MCF_COEF_NUM
typedef ot_mcf_crop_info hi_mcf_crop_info;
typedef ot_mcf_pipe_attr hi_mcf_pipe_attr;
#define HI_MCF_PATH_FUSION OT_MCF_PATH_FUSION
#define HI_MCF_PATH_COLOR OT_MCF_PATH_COLOR
#define HI_MCF_PATH_MONO OT_MCF_PATH_MONO
#define HI_MCF_PATH_BUTT OT_MCF_PATH_BUTT
typedef ot_mcf_path hi_mcf_path;
typedef ot_mcf_grp_attr hi_mcf_grp_attr;
typedef ot_mcf_chn_attr hi_mcf_chn_attr;
typedef ot_mcf_ext_chn_attr hi_mcf_ext_chn_attr;
#define HI_MCF_FUSION_GLOBAL_MODE OT_MCF_FUSION_GLOBAL_MODE
#define HI_MCF_FUSION_ADAPTIVE_MODE OT_MCF_FUSION_ADAPTIVE_MODE
#define HI_MCF_FUSION_ALPHA_MODE_BUTT OT_MCF_FUSION_ALPHA_MODE_BUTT
typedef ot_mcf_fusion_alpha_mode hi_mcf_fusion_alpha_mode;
#define HI_MCF_NO_HIST_ADJ OT_MCF_NO_HIST_ADJ
#define HI_MCF_COLOR_HIST_ADJ OT_MCF_COLOR_HIST_ADJ
#define HI_MCF_MONO_HIST_ADJ OT_MCF_MONO_HIST_ADJ
#define HI_MCF_HIST_ADJ_MODE_BUTT OT_MCF_HIST_ADJ_MODE_BUTT
typedef ot_mcf_hist_adj_mode hi_mcf_hist_adj_mode;
typedef ot_mcf_color_correct_cfg hi_mcf_color_correct_cfg;
typedef ot_mcf_fusion_global_alpha_mode_cfg hi_mcf_fusion_global_alpha_mode_cfg;
typedef ot_mcf_fusion_adaptive_alpha_mode_cfg hi_mcf_fusion_adaptive_alpha_mode_cfg;
typedef ot_mcf_filter_proc_cfg hi_mcf_filter_proc_cfg;
typedef ot_mcf_detail_proc_cfg hi_mcf_detail_proc_cfg;
typedef ot_mcf_base_proc_cfg hi_mcf_base_proc_cfg;
typedef ot_mcf_each_freq_proc_cfg hi_mcf_each_freq_proc_cfg;
typedef ot_mcf_alg_param hi_mcf_alg_param;
typedef ot_mcf_vi_attr hi_mcf_vi_attr;

#ifdef __cplusplus
}
#endif
#endif /* HI_COMMON_MCF_H */
