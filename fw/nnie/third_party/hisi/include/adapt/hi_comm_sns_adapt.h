/*
*  Copyright (C),2013-2019, Hisilicon Tech. Co., Ltd.
*  Version       : Initial Draft
*  Author        : ISP_SW
*  Created       : 2013/11/07
*  Description   :
*  History       :
*  1.Date        : 2013/11/07
*  Modification: Created file
*/

#ifndef __HI_COMM_SNS_ADAPT_H__
#define __HI_COMM_SNS_ADAPT_H__

#include "hi_type.h"
#include "hi_common_adapt.h"
#include "hi_comm_isp_adapt.h"
#include "hi_comm_sns.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */

typedef struct {
    hi_bool update;
    hi_u16  black_level[ISP_BAYER_CHN_NUM];
} hi_isp_cmos_black_level;

typedef struct {
    SENSOR_ID            sensor_id;
} hi_isp_sns_attr_info;

typedef struct {
    hi_bool enable;
    hi_u8   non_dir_str[ISP_AUTO_ISO_STRENGTH_NUM];
    hi_u8   non_dir_mf_detail_ehc_str[ISP_AUTO_ISO_STRENGTH_NUM];
    hi_u8   non_dir_hf_detail_ehc_str[ISP_AUTO_ISO_STRENGTH_NUM];
    hi_u8   detail_smooth_range[ISP_AUTO_ISO_STRENGTH_NUM];
} hi_isp_cmos_demosaic;

typedef struct {
    hi_bool  enable;
    hi_bool  bnr_mono_sensor_en;
    hi_bool  nr_lsc_enable;
    hi_u8    bnr_lsc_max_gain;
    hi_u16   bnr_lsc_cmp_strength;

    hi_u8    lut_fine_str[ISP_AUTO_ISO_STRENGTH_NUM];
    hi_u8    chroma_str[ISP_BAYER_CHN_NUM][ISP_AUTO_ISO_STRENGTH_NUM];
    hi_u8    wdr_frame_str[WDR_MAX_FRAME_NUM];
    hi_u8    fusion_frame_str[WDR_MAX_FRAME_NUM];
    hi_u16   coarse_str[ISP_BAYER_CHN_NUM][ISP_AUTO_ISO_STRENGTH_NUM];
    hi_u16   lut_coring_wgt[ISP_AUTO_ISO_STRENGTH_NUM];
    hi_u16   lut_coring_ratio[HI_ISP_BAYERNR_LUT_LENGTH];
} hi_isp_cmos_bayernr;

typedef struct {
    hi_u16   calibration_lut_num;
    hi_float calibration_coef[BAYER_CALIBTAION_MAX_NUM][3];
} hi_isp_cmos_noise_calibration;

typedef struct {
    hi_isp_de_attr attr;
} hi_isp_cmos_detail;

typedef struct {
    hi_u8  luma_wgt[ISP_SHARPEN_LUMA_NUM];        /* RW; Range: [0, 127]; Adjust the sharpen strength according to luma. Sharpen strength will be weaker when it decrease. */
    hi_u16 texture_str[ISP_SHARPEN_GAIN_NUM];    /* RW; Range: [0, 4095]; Format:7.5;Undirectional sharpen strength for texture and detail enhancement */
    hi_u16 edge_str[ISP_SHARPEN_GAIN_NUM];       /* RW; Range: [0, 4095]; Format:7.5;Directional sharpen strength for edge enhancement */
    hi_u16 texture_freq;         /* RW; Range: [0, 4095];Format:6.6; Texture frequency adjustment. Texture and detail will be finer when it increase */
    hi_u16 edge_freq;            /* RW; Range: [0, 4095];Format:6.6; Edge frequency adjustment. Edge will be narrower and thiner when it increase */
    hi_u8  over_shoot;            /* RW; Range: [0, 127]; Format:7.0;u8OvershootAmt */
    hi_u8  under_shoot;           /* RW; Range: [0, 127]; Format:7.0;u8UndershootAmt */
    hi_u8  shoot_sup_str;          /* RW; Range: [0, 255]; Format:8.0;overshoot and undershoot suppression strength, the amplitude and width of shoot will be decrease when shootSupSt increase */
    hi_u8  shoot_sup_adj;          /* RW; Range: [0, 15]; Format:4.0;overshoot and undershoot suppression adjusting, adjust the edge shoot suppression strength */
    hi_u8  detail_ctrl;           /* RW; Range: [0, 255]; Format:8.0;Different sharpen strength for detail and edge. When it is bigger than 128, detail sharpen strength will be stronger than edge. */
    hi_u8  detail_ctrl_thr;        /* RW; Range: [0, 255]; Format:8.0; The threshold of DetailCtrl, it is used to distinguish detail and edge. */
    hi_u8  edge_filt_str;          /* RW; Range: [0, 63]; Format:6.0;The strength of edge filtering. */
    hi_u8  edge_filt_max_cap;       /* RW; Range: [0, 47]; Format:6.0;The max capacity of edge filtering.*/
    hi_u8  r_gain;                /* RW; Range: [0, 31];   Format:5.0;Sharpen Gain for Red Area */
    hi_u8  g_gain;                /* RW; Range: [0, 255]; Format:8.0;Sharpen Gain for Green Area */
    hi_u8  b_gain;                /* RW; Range: [0, 31];   Format:5.0;Sharpen Gain for Blue Area */
    hi_u8  skin_gain;             /* RW; Range: [0, 31]; Format:5.0;Sharpen Gain for Skin Area */
    hi_u16 max_sharp_gain;        /* RW; Range: [0, 0x7FF]; Format:8.3; Maximum sharpen gain */
} hi_isp_cmos_sharpen_manual;

typedef struct {
    hi_u8  luma_wgt[ISP_SHARPEN_LUMA_NUM][ISP_AUTO_ISO_STRENGTH_NUM];      /* RW; Range: [0, 127];  Adjust the sharpen strength according to luma. Sharpen strength will be weaker when it decrease. */
    hi_u16 texture_str[ISP_SHARPEN_GAIN_NUM][ISP_AUTO_ISO_STRENGTH_NUM];  /* RW; Range: [0, 4095]; Format:7.5;Undirectional sharpen strength for texture and detail enhancement */
    hi_u16 edge_str[ISP_SHARPEN_GAIN_NUM][ISP_AUTO_ISO_STRENGTH_NUM];     /* RW; Range: [0, 4095]; Format:7.5;Directional sharpen strength for edge enhancement */
    hi_u16 texture_freq[ISP_AUTO_ISO_STRENGTH_NUM];         /* RW; Range: [0, 4095]; Format:6.6;Texture frequency adjustment. Texture and detail will be finer when it increase */
    hi_u16 edge_freq[ISP_AUTO_ISO_STRENGTH_NUM];            /* RW; Range: [0, 4095]; Format:6.6;Edge frequency adjustment. Edge will be narrower and thiner when it increase */
    hi_u8  over_shoot[ISP_AUTO_ISO_STRENGTH_NUM];            /* RW; Range: [0, 127];  Format:7.0;u8OvershootAmt */
    hi_u8  under_shoot[ISP_AUTO_ISO_STRENGTH_NUM];           /* RW; Range: [0, 127];  Format:7.0;u8UndershootAmt */
    hi_u8  shoot_sup_str[ISP_AUTO_ISO_STRENGTH_NUM];          /* RW; Range: [0, 255]; Format:8.0;overshoot and undershoot suppression strength, the amplitude and width of shoot will be decrease when shootSupSt increase */
    hi_u8  shoot_sup_adj[ISP_AUTO_ISO_STRENGTH_NUM];          /* RW; Range: [0, 15]; Format:4.0;overshoot and undershoot suppression adjusting, adjust the edge shoot suppression strength */
    hi_u8  detail_ctrl[ISP_AUTO_ISO_STRENGTH_NUM];           /* RW; Range: [0, 255]; Format:8.0;Different sharpen strength for detail and edge. When it is bigger than 128, detail sharpen strength will be stronger than edge. */
    hi_u8  detail_ctrl_thr[ISP_AUTO_ISO_STRENGTH_NUM];        /* RW; Range: [0, 255]; Format:8.0; The threshold of DetailCtrl, it is used to distinguish detail and edge. */
    hi_u8  edge_filt_str[ISP_AUTO_ISO_STRENGTH_NUM];          /* RW; Range: [0, 63]; Format:6.0;The strength of edge filtering. */
    hi_u8  edge_filt_max_cap[ISP_AUTO_ISO_STRENGTH_NUM];        /* RW; Range: [0, 47]; Format:6.0;The max capacity of edge filtering.*/
    hi_u8  r_gain[ISP_AUTO_ISO_STRENGTH_NUM];                /* RW; Range: [0, 31];   Format:5.0;Sharpen Gain for Red Area */
    hi_u8  g_gain[ISP_AUTO_ISO_STRENGTH_NUM];                /* RW; Range: [0, 255]; Format:8.0;Sharpen Gain for Green Area */
    hi_u8  b_gain[ISP_AUTO_ISO_STRENGTH_NUM];                /* RW; Range: [0, 31];   Format:5.0;Sharpen Gain for Blue Area */
    hi_u8  skin_gain[ISP_AUTO_ISO_STRENGTH_NUM];             /* RW; Range: [0, 31]; Format:5.0; */
    hi_u16 max_sharp_gain[ISP_AUTO_ISO_STRENGTH_NUM];        /* RW; Range: [0, 0x7FF]; Format:8.3; */
} hi_isp_cmos_sharpen_auto;

typedef struct {
    hi_u8 skin_umin;
    hi_u8 skin_vmin;
    hi_u8 skin_umax;
    hi_u8 skin_vmax;
    hi_isp_cmos_sharpen_manual manual_attr;
    hi_isp_cmos_sharpen_auto   auto_attr;
} hi_isp_cmos_sharpen;

typedef struct {
    hi_bool enable;               /* RW; Range:[0, 1]; Format:1.0;Enable/Disable Edge Mark */
    hi_u8   threshold;           /* RW; range: [0, 255];  Format:8.0; */
    hi_u32  color;              /* RW; range: [0, 0xFFFFFF];  Format:32.0; */
} hi_isp_cmos_edgemark;

typedef struct {
    hi_bool enable;         /* RW; Range:[0, 1]; Format:1.0;Enable/Disable HLC module */
    hi_u8   luma_thr;       /* RW; range: [0, 255];  Format:8.0; */
    hi_u8   luma_target;    /* RW; range: [0, 255];  Format:8.0; */
} hi_isp_cmos_hlc;

typedef struct {
    hi_bool enable;
    hi_isp_op_type op_type;

    hi_u16  manual_strength;
    hi_u16  auto_strength;

    hi_u8   spatial_flt_coef;
    hi_u8   range_flt_coef;
    hi_u8   contrast_control;
    hi_s8   detail_adjust_factor;
    hi_u8   range_ada_max;

    hi_u8   flt_scale_fine;
    hi_u8   flt_scale_coarse;
    hi_u8   grad_rev_max;
    hi_u8   grad_rev_thr;

    hi_u8   bright_gain_lmt;
    hi_u8   bright_gain_lmt_step;
    hi_u8   dark_gain_lmt_y;
    hi_u8   dark_gain_lmt_c;

    hi_u8   pd_strength;
    hi_u8   local_mixing_bright_max;
    hi_u8   local_mixing_bright_min;
    hi_u8   local_mixing_bright_thr;
    hi_s8   local_mixing_bright_slo;
    hi_u8   local_mixing_dark_max;
    hi_u8   local_mixing_dark_min;
    hi_u8   local_mixing_dark_thr;
    hi_s8   local_mixing_dark_slo;
    hi_u16  color_correction_lut[33];
    hi_u16  tone_mapping_value[200];

    hi_u8   asymmetry;
    hi_u8   second_pole;
    hi_u8   stretch;
    hi_u8   compress;

    hi_u8   curve_sel;

    hi_u16 x_point[5];
    hi_u16 y_point[5];
    hi_u16 slope[5];
} hi_isp_cmos_drc;

typedef struct {
    hi_bool  fusion_mode;
    hi_bool  motion_comp;

    hi_u16   short_thr;
    hi_u16   long_thr;

    hi_bool  force_long;
    hi_u16   force_long_low_thr;
    hi_u16   force_long_hig_thr;

    hi_bool  short_expo_chk;
    hi_u16   short_check_thd;
    hi_bool  md_ref_flicker;

    hi_u8    md_thr_low_gain[ISP_AUTO_ISO_STRENGTH_NUM];
    hi_u8    md_thr_hig_gain[ISP_AUTO_ISO_STRENGTH_NUM];

    hi_isp_bnr_mode  bnr_mode;
    hi_u16   fusion_thr[WDR_MAX_FRAME];
    hi_u8    mdt_still_thd;
    hi_u8    mdt_long_blend;
} hi_isp_cmos_wdr;

typedef struct {
    hi_bool        enable;           /* RW;Range:[0,1];Format:1.0; */
    hi_bool        user_lut_enable;    /* RW;Range:[0,1];0:Auto Lut 1:User Lut */
    hi_isp_op_type op_type;
    hi_u8          auto_strength;
    hi_u8          manual_strength;
} hi_isp_cmos_dehaze;

typedef struct {
    hi_bool enable;
    hi_u32  pregamma_lut[PREGAMMA_NODE_NUM];
} hi_isp_cmos_pregamma;

typedef struct {
    hi_u16  gamma_lut[GAMMA_EXT_NODE_NUMBER];
} hi_isp_cmos_gamma;

typedef struct {
    hi_u32  max_width;
    hi_u32  max_height;
} hi_isp_cmos_sensor_max_resolution;

typedef struct {
    hi_u16  strength[ISP_AUTO_ISO_STRENGTH_NUM];
    hi_u16  blend_ratio[ISP_AUTO_ISO_STRENGTH_NUM];
} hi_isp_cmos_dpc;

typedef struct {
    hi_u16 r_gain[HI_ISP_LSC_GRID_POINTS];
    hi_u16 gr_gain[HI_ISP_LSC_GRID_POINTS];
    hi_u16 gb_gain[HI_ISP_LSC_GRID_POINTS];
    hi_u16 b_gain[HI_ISP_LSC_GRID_POINTS];
} hi_isp_lsc_cabli_table;

typedef struct {
    hi_u16 r_gain[HI_ISP_RLSC_POINTS];
    hi_u16 gr_gain[HI_ISP_RLSC_POINTS];
    hi_u16 gb_gain[HI_ISP_RLSC_POINTS];
    hi_u16 b_gain[HI_ISP_RLSC_POINTS];
} hi_isp_bnr_lsc_calib_table;

typedef struct {
    hi_u16  mesh_strength;
    hi_u8   mesh_scale;
    hi_isp_lsc_cabli_table lsc_calib_table[2];
    hi_isp_bnr_lsc_calib_table bnr_lsc_calib_table;
} hi_isp_cmos_lsc;

typedef struct {
    hi_bool   enable;
    hi_u16    y_ratio_lut[HI_ISP_CA_YRATIO_LUT_LENGTH];  // 1.10bit  Y Ratio For UV ; Max = 2047 FW Limit
    hi_s16    iso_ratio[ISP_AUTO_ISO_STRENGTH_NUM];     // 1.10bit  ISO Ratio  For UV,Max = 2047 FW Limi
} hi_isp_cmos_ca;

typedef struct {
    hi_bool enable;
    hi_u32  gain_r;
    hi_u32  gain_g;
    hi_u32  gain_b;
    hi_isp_clut_lut clut_lut;
} hi_isp_cmos_clut;

typedef struct {
    hi_u16 x;                    /* RW;Range:[0x0,0x101];Format:9.0;The X point of the knee */
    hi_u32 y;                    /* RW;Range:[0x0,0x100000];Format:21.0;The Y point of the  knee */
} hi_isp_cmos_expander_point;

typedef struct {
    hi_bool enable;
    hi_u8   bit_depth_in;            /* RW;Range:[0xC,0x14];Format:5.0;The Bit depth of input */
    hi_u8   bit_depth_out;           /* RW;Range:[0xC,0x14];Format:5.0;The Bit depth of output */
    hi_isp_cmos_expander_point expander_point[ISP_EXPANDER_POINT_NUM];
} hi_isp_cmos_expander;

typedef struct {
    hi_bool enable;                                 /* RW,Range: [   0, 1]      */
    hi_u8   slope;                                  /* RW,Range: [   0, 0xE]    */
    hi_u8   sensi_slope;                             /* RW,Range: [   0, 0xE]    */
    hi_u16  sensi_thr;                              /* RW,Range: [   0, 0x3FFF] */
    hi_u16  threshold[ISP_AUTO_ISO_STRENGTH_NUM]; /* RW,Range: [   0, 0x3FFF] */
    hi_u16  strength[ISP_AUTO_ISO_STRENGTH_NUM];  /* RW,Range: [   0, 0x100]  */
    hi_u16  np_offset[ISP_AUTO_ISO_STRENGTH_NUM];  /* RW,Range: [0x200, 0x3FFF] */
} hi_isp_cmos_ge;

typedef struct {
    hi_bool enable;                                                     /* RW;Range:[0x0,0x1];Format:1.0; AntiFalseColor Enable */
    hi_u8   threshold[ISP_AUTO_ISO_STRENGTH_NUM];       /* RW;Range:[0x0,0x20];Format:6.0;Threshold for antifalsecolor */
    hi_u8   strength[ISP_AUTO_ISO_STRENGTH_NUM];        /* RW;Range:[0x0,0x1F];Format:5.0;Strength of antifalsecolor */
} hi_isp_cmos_anti_false_color;

typedef struct {
    hi_bool  enable;
    hi_u8    gauss_lpf_sigma;
    hi_u8    he_pos_wgt[ISP_AUTO_ISO_STRENGTH_NUM];
    hi_u8    he_pos_sigma[ISP_AUTO_ISO_STRENGTH_NUM];
    hi_u8    he_pos_mean[ISP_AUTO_ISO_STRENGTH_NUM];
    hi_u8    he_neg_wgt[ISP_AUTO_ISO_STRENGTH_NUM];
    hi_u8    he_neg_sigma[ISP_AUTO_ISO_STRENGTH_NUM];
    hi_u8    he_neg_mean[ISP_AUTO_ISO_STRENGTH_NUM];
    hi_u16   blc_ctrl[ISP_AUTO_ISO_STRENGTH_NUM];
} hi_isp_cmos_ldci;

typedef struct {
    hi_u32  sensor_id;
    hi_u8   sensor_mode;
    hi_bool valid_dng_raw_format;
    hi_isp_dng_raw_format dng_raw_format;
} hi_isp_cmos_sensor_mode;

typedef struct {
    hi_isp_dng_wb_gain wb_gain1; /* the calibration White balance gain of colorcheker in A Light */
    hi_isp_dng_wb_gain wb_gain2; /* the calibration White balance gain of colorcheker in D50 Light */
} hi_isp_cmos_dng_color_param;

typedef struct {
    hi_u32   exp_ratio[EXP_RATIO_NUM];
} hi_isp_cmos_wdr_switch_attr;

typedef union {
    hi_u64  key;
    struct {
        hi_u64  bit1_drc              : 1 ;   /* [0] */
        hi_u64  bit1_demosaic         : 1 ;   /* [1] */
        hi_u64  bit1_pregamma         : 1 ;   /* [2] */
        hi_u64  bit1_gamma            : 1 ;   /* [3] */
        hi_u64  bit1_sharpen          : 1 ;   /* [4] */
        hi_u64  bit1_edge_mark        : 1 ;   /* [5] */
        hi_u64  bit1_hlc              : 1 ;   /* [6] */
        hi_u64  bit1_ldci             : 1 ;   /* [7] */
        hi_u64  bit1_dpc              : 1 ;   /* [8] */
        hi_u64  bit1_lsc              : 1 ;   /* [9] */
        hi_u64  bit1_ge               : 1 ;   /* [10] */
        hi_u64  bit1_anti_false_color : 1 ;   /* [11] */
        hi_u64  bit1_bayer_nr         : 1 ;   /* [12] */
        hi_u64  bit1_detail           : 1 ;   /* [13] */
        hi_u64  bit1_ca               : 1 ;   /* [14] */
        hi_u64  bit1_expander         : 1 ;   /* [15] */
        hi_u64  bit1_clut             : 1 ;   /* [16] */
        hi_u64  bit1_wdr              : 1 ;   /* [17] */
        hi_u64  bit1_dehaze           : 1 ;   /* [18] */
        hi_u64  bit45_rsv             : 45;   /* [19:63] */
    };
} hi_isp_cmos_alg_key;

typedef struct {
    hi_isp_cmos_alg_key              key;
    const hi_isp_cmos_drc             *drc;
    const hi_isp_cmos_demosaic        *demosaic;
    const hi_isp_cmos_pregamma        *pregamma;
    const hi_isp_cmos_gamma           *gamma;
    const hi_isp_cmos_sharpen         *sharpen;
    const hi_isp_cmos_edgemark        *edge_mark;
    const hi_isp_cmos_hlc             *hlc;
    const hi_isp_cmos_ldci            *ldci;
    const hi_isp_cmos_dpc             *dpc;
    const hi_isp_cmos_lsc             *lsc;
    const hi_isp_cmos_ge              *ge;
    const hi_isp_cmos_anti_false_color  *anti_false_color;
    const hi_isp_cmos_bayernr         *bayer_nr;
    const hi_isp_cmos_detail          *detail;
    const hi_isp_cmos_ca              *ca;
    const hi_isp_cmos_expander        *expander;
    const hi_isp_cmos_clut            *clut;
    const hi_isp_cmos_wdr             *wdr;
    const hi_isp_cmos_dehaze          *dehaze;
    hi_isp_cmos_noise_calibration     noise_calibration;
    hi_isp_cmos_sensor_max_resolution sensor_max_resolution;
    hi_isp_cmos_sensor_mode           sensor_mode;
    hi_isp_cmos_dng_color_param       dng_color_param;
    hi_isp_cmos_wdr_switch_attr       wdr_switch_attr;
} hi_isp_cmos_default;

typedef struct {
    hi_u16   width;
    hi_u16   height;
    hi_float fps;
    hi_u8    sns_mode;
} hi_isp_cmos_sensor_image_mode;

typedef struct {
    hi_void (*pfn_cmos_sensor_init)(VI_PIPE vi_pipe);
    hi_void (*pfn_cmos_sensor_exit)(VI_PIPE vi_pipe);
    hi_void (*pfn_cmos_sensor_global_init)(VI_PIPE vi_pipe);
    hi_s32 (*pfn_cmos_set_image_mode)(VI_PIPE vi_pipe, hi_isp_cmos_sensor_image_mode *sensor_image_mode);
    hi_s32 (*pfn_cmos_set_wdr_mode)(VI_PIPE vi_pipe, hi_u8 mode);

    /* the algs get data which is associated with sensor, except 3a */
    hi_s32 (*pfn_cmos_get_isp_default)(VI_PIPE vi_pipe, hi_isp_cmos_default *def);
    hi_s32 (*pfn_cmos_get_isp_black_level)(VI_PIPE vi_pipe, hi_isp_cmos_black_level *black_level);
    hi_s32 (*pfn_cmos_get_sns_reg_info)(VI_PIPE vi_pipe, hi_isp_sns_regs_info *sns_regs_info);

    /* the function of sensor set pixel detect */
    hi_void (*pfn_cmos_set_pixel_detect)(VI_PIPE vi_pipe, hi_bool enable);
    hi_s32 (*pfn_cmos_get_awb_gains)(VI_PIPE vi_pipe, hi_s32 *sensor_awb_gain);
} hi_isp_sensor_exp_func;

typedef struct {
    hi_isp_sensor_exp_func sns_exp;
} hi_isp_sensor_register;

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif /* __HI_COMM_SNS_ADAPT_H__ */
