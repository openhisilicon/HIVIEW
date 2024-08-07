/*
  Copyright (c), 2001-2024, Shenshu Tech. Co., Ltd.
 */

#ifndef OT_COMMON_SNS_H
#define OT_COMMON_SNS_H

#include "ot_type.h"
#include "ot_common.h"
#include "ot_common_isp.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */

typedef struct {
    td_bool update;
    td_u16  black_level[OT_ISP_WDR_MAX_FRAME_NUM][OT_ISP_BAYER_CHN_NUM]; /* 14bits */
} ot_isp_black_level_auto_attr;

typedef struct {
    ot_size    ob_sns_size;
} ot_isp_sns_total_size_attr;

typedef struct {
    td_bool  user_black_level_en;
    td_u16   user_black_level[OT_ISP_WDR_MAX_FRAME_NUM][OT_ISP_BAYER_CHN_NUM];
    ot_isp_black_level_mode         black_level_mode;
    ot_isp_black_level_manual_attr  manual_attr;
    ot_isp_black_level_dynamic_attr dynamic_attr;
    ot_isp_black_level_auto_attr    auto_attr;
    ot_isp_sns_total_size_attr   sns_with_ob_attr;
} ot_isp_cmos_black_level;

typedef struct {
    ot_sensor_id            sns_id;
} ot_isp_sns_attr_info;

#define BAYER_CALIBRATION_PARA_NUM 3

typedef struct {
    td_u32  max_width;
    td_u32  max_height;
} ot_isp_cmos_sns_max_resolution;

typedef struct {
    ot_isp_shading_attr     lsc_attr;
    ot_isp_shading_lut_attr lsc_lut;
} ot_isp_cmos_lsc;

typedef struct {
    ot_isp_lblc_attr     lblc_attr;
    ot_isp_lblc_lut_attr lblc_lut;
} ot_isp_cmos_lblc;

typedef struct {
    ot_isp_clut_attr clut_attr;
    ot_isp_clut_lut clut_lut;
} ot_isp_cmos_clut;

typedef struct {
    td_u32  sns_id;
    td_u8   sns_mode;
    td_bool valid_dng_raw_format;
    ot_isp_dng_raw_format dng_raw_format;
} ot_isp_cmos_sns_mode;

typedef ot_isp_dng_color_param ot_isp_cmos_dng_color_param;

typedef struct {
    td_u32   exp_ratio[OT_ISP_EXP_RATIO_NUM];
} ot_isp_cmos_wdr_switch_attr;

typedef struct {
    td_u16 g_param_high_ct[OT_ISP_LSC_GRID_POINTS];
    td_u16 g_param_low_ct[OT_ISP_LSC_GRID_POINTS];
} ot_isp_acs_y_shading_lut;

typedef struct {
    td_float avg_rg_map[OT_ISP_LSC_GRID_POINTS];
    td_float avg_bg_map[OT_ISP_LSC_GRID_POINTS];
    td_float prof_rg_map[OT_ISP_LSC_GRID_POINTS];
    td_float prof_bg_map[OT_ISP_LSC_GRID_POINTS];
} ot_isp_acs_color_shading_lut;

typedef struct {
    td_s16   light_index[OT_ISP_ACS_LIGHT_NUM * OT_ISP_ACS_CHN_NUM];
    td_float model_ar_min;
    td_float model_ar_step;
    td_float model_ab_min;
    td_float model_ab_step;
    td_s16   light_type_g_high;
    td_s16   light_type_g_low;
} ot_isp_acs_calib_param;

typedef struct {
    ot_isp_acs_attr               acs_attr;
    ot_isp_acs_calib_param        acs_calib_param;
    ot_isp_acs_y_shading_lut      acs_y_shading_lut;
    ot_isp_acs_color_shading_lut  acs_color_shading_lut;
} ot_isp_cmos_acs;

typedef union {
    td_u64  key;
    struct {
        td_u64  bit1_drc              : 1;   /* [0] */
        td_u64  bit1_demosaic         : 1;   /* [1] */
        td_u64  bit1_pregamma         : 1;   /* [2] Not support for Hi3519DV500 */
        td_u64  bit1_gamma            : 1;   /* [3] */
        td_u64  bit1_sharpen          : 1;   /* [4] */
        td_u64  bit1_ldci             : 1;   /* [5] */
        td_u64  bit1_dpc              : 1;   /* [6] */
        td_u64  bit1_lsc              : 1;   /* [7] */
        td_u64  bit1_ge               : 1;   /* [8] */
        td_u64  bit1_anti_false_color : 1;   /* [9] */
        td_u64  bit1_bayer_nr         : 1;   /* [10] */
        td_u64  bit1_ca               : 1;   /* [11] */
        td_u64  bit1_expander         : 1;   /* [12] */
        td_u64  bit1_clut             : 1;   /* [13] */
        td_u64  bit1_wdr              : 1;   /* [14] */
        td_u64  bit1_dehaze           : 1;   /* [15] */
        td_u64  bit1_cac              : 1;   /* [16] */
        td_u64  bit1_acs              : 1;   /* [17] */
        td_u64  bit1_bshp             : 1;   /* [18] */
        td_u64  bit1_lblc             : 1;   /* [19] */
        td_u64  bit44_reserved        : 44;   /* [20:63] */
    };
} ot_isp_cmos_alg_key;

typedef struct {
    ot_isp_cmos_alg_key                key;
    const ot_isp_drc_attr              *drc;
    const ot_isp_demosaic_attr         *demosaic;
    const ot_isp_gamma_attr            *gamma;
    const ot_isp_sharpen_attr          *sharpen;
    const ot_isp_ldci_attr             *ldci;
    const ot_isp_dp_dynamic_attr       *dpc;
    const ot_isp_cmos_lsc              *lsc;
    const ot_isp_cr_attr               *ge;
    const ot_isp_anti_false_color_attr *anti_false_color;
    const ot_isp_nr_attr               *bayer_nr;
    const ot_isp_ca_attr               *ca;
    const ot_isp_expander_attr         *expander;
    const ot_isp_cmos_clut             *clut;
    const ot_isp_wdr_fs_attr           *wdr;
    const ot_isp_dehaze_attr           *dehaze;
    const ot_isp_cac_attr              *cac;
    const ot_isp_bayershp_attr         *bshp;
    const ot_isp_cmos_acs              *acs;
    const ot_isp_cmos_lblc             *lblc;
    ot_isp_noise_calibration     noise_calibration;
    ot_isp_cmos_sns_max_resolution sns_max_resolution;
    ot_isp_cmos_sns_mode           sns_mode;
    ot_isp_cmos_dng_color_param       dng_color_param;
    ot_isp_cmos_wdr_switch_attr       wdr_switch_attr;
} ot_isp_cmos_default;

typedef struct {
    td_u16   width;
    td_u16   height;
    td_float fps;
    td_u8    sns_mode;
} ot_isp_cmos_sns_image_mode;

typedef struct {
    td_void (*pfn_cmos_sns_init)(ot_vi_pipe vi_pipe);
    td_void (*pfn_cmos_sns_exit)(ot_vi_pipe vi_pipe);
    td_void (*pfn_cmos_sns_global_init)(ot_vi_pipe vi_pipe);
    td_s32 (*pfn_cmos_set_image_mode)(ot_vi_pipe vi_pipe, const ot_isp_cmos_sns_image_mode *sns_image_mode);
    td_s32 (*pfn_cmos_set_wdr_mode)(ot_vi_pipe vi_pipe, td_u8 mode);

    /* the algs get data which is associated with sensor, except 3a */
    td_s32 (*pfn_cmos_get_isp_default)(ot_vi_pipe vi_pipe, ot_isp_cmos_default *def);
    td_s32 (*pfn_cmos_get_isp_black_level)(ot_vi_pipe vi_pipe, ot_isp_cmos_black_level *black_level);
    td_s32 (*pfn_cmos_get_blc_clamp_info)(ot_vi_pipe vi_pipe, td_bool *clamp_en);
    td_s32 (*pfn_cmos_get_sns_reg_info)(ot_vi_pipe vi_pipe, ot_isp_sns_regs_info *sns_regs_info);

    /* the function of sensor set pixel detect */
    td_void (*pfn_cmos_set_pixel_detect)(ot_vi_pipe vi_pipe, td_bool enable);
    td_s32 (*pfn_cmos_get_awb_gains)(ot_vi_pipe vi_pipe, td_u32 *sns_awb_gain);
} ot_isp_sns_exp_func;

typedef struct {
    ot_isp_sns_exp_func sns_exp;
} ot_isp_sns_register;

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif /* OT_COMM_SNS_H */
