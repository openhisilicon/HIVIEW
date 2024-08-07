/*
  Copyright (c), 2001-2024, Shenshu Tech. Co., Ltd.
 */

#ifndef OT_COMMON_AE_H
#define OT_COMMON_AE_H

#include "ot_type.h"
#include "ot_debug.h"
#include "ot_common_isp.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */

#define OT_AE_LIB_NAME "ot_ae_lib"
#define OT_THERMO_LIB_NAME "ot_thermo_lib"

/* sensor's interface to ae
 * eg: 0.35db, enAccuType=OT_ISP_AE_ACCURACY_DB, f32Accuracy=0.35
 *  and the multiply of 0.35db is power(10, (0.35/20))
 *  eg: 1/16, 2/16, 3/16 multiplies, enAccuType=OT_ISP_AE_ACCURACY_LINEAR, f32Accuracy=0.0625
 *  eg: 1,2,4,8,16 multiplies, enAccuType=OT_ISP_AE_ACCURACY_DB, f32Accuracy=6
 */
typedef enum {
    OT_ISP_AE_ACCURACY_DB = 0,
    OT_ISP_AE_ACCURACY_LINEAR,
    OT_ISP_AE_ACCURACY_TABLE,

    OT_ISP_AE_ACCURACY_BUTT,
} ot_isp_ae_accuracy_type;

typedef struct {
    ot_isp_ae_accuracy_type accu_type;
    float   accuracy;
    float   offset;
} ot_isp_ae_accuracy;

typedef struct {
    td_bool quick_start_enable;
    td_u8 black_frame_num;
    td_bool ir_mode_en;
    td_u32 init_exposure_ir;
    td_u32 iso_thr_ir;
    td_u16 ir_cut_delay_time;
} ot_isp_quick_start_param;

typedef struct {
    td_u8   ae_compensation;

    td_u32  lines_per500ms;
    td_u32  flicker_freq;
    td_float fps;
    td_u32  hmax_times; /* unit is ns */
    td_u32  init_exposure;
    td_u32  init_int_time;
    td_u32  init_again;
    td_u32  init_dgain;
    td_u32  init_isp_dgain;
    td_u32  init_ae_speed;
    td_u32  init_ae_tolerance;

    td_u32  full_lines_std;
    td_u32  full_lines_max;
    td_u32  full_lines;
    td_u32  binning_full_lines;
    td_u32  max_int_time;     /* RW;unit is line */
    td_u32  min_int_time;
    td_u32  max_int_time_target;
    td_u32  min_int_time_target;
    ot_isp_ae_accuracy int_time_accu;

    td_u32  max_again;
    td_u32  min_again;
    td_u32  max_again_target;
    td_u32  min_again_target;
    ot_isp_ae_accuracy again_accu;

    td_u32  max_dgain;
    td_u32  min_dgain;
    td_u32  max_dgain_target;
    td_u32  min_dgain_target;
    ot_isp_ae_accuracy dgain_accu;

    td_u32  max_isp_dgain_target;
    td_u32  min_isp_dgain_target;
    td_u32  isp_dgain_shift;

    td_u32  max_int_time_step;
    td_bool max_time_step_enable;
    td_u32  max_inc_time_step[OT_ISP_WDR_MAX_FRAME_NUM];
    td_u32  max_dec_time_step[OT_ISP_WDR_MAX_FRAME_NUM];
    td_u32  lf_max_short_time;
    td_u32  lf_min_exposure;

    ot_isp_ae_route ae_route_attr;
    td_bool ae_route_ex_valid;
    ot_isp_ae_route_ex ae_route_attr_ex;

    ot_isp_ae_route ae_route_sf_attr;
    ot_isp_ae_route_ex ae_route_sf_attr_ex;

    td_u16 man_ratio_enable;
    td_u32 arr_ratio[OT_ISP_EXP_RATIO_NUM];

    ot_isp_iris_type  iris_type;
    ot_isp_piris_attr piris_attr;
    ot_isp_iris_f_no  max_iris_fno;  /* RW; Range:[0, 10]; Format:4.0;
                                        Max F number of Piris's aperture, it's related to the specific iris */
    ot_isp_iris_f_no  min_iris_fno;  /* RW; Range:[0, 10]; Format:4.0;
                                        Min F number of Piris's aperture, it's related to the specific iris */

    ot_isp_ae_strategy ae_exp_mode;

    td_u16 iso_cal_coef;
    td_u8  ae_run_interval;
    td_u32 exp_ratio_max;
    td_u32 exp_ratio_min;
    td_bool diff_gain_support;
    ot_isp_quick_start_param quick_start;
    ot_isp_prior_frame prior_frame;
    td_bool ae_gain_sep_cfg;
    td_bool lhcg_support;
    td_u32 sns_lhcg_exp_ratio;

    td_u8 ae_stat_pos;          /* RW; Range:[0, 1]; Format:1.0 */
} ot_isp_ae_sensor_default;

typedef struct {
    ot_isp_fswdr_mode fswdr_mode;
} ot_isp_ae_fswdr_attr;

typedef struct {
    td_u32 reg_addr;
    td_u32 reg_value;
} ot_isp_ae_param_reg;

typedef struct {
    td_u32 tar_fps;
    td_u32 exp_time;
    td_u32 exp_again;
    td_u32 exp_dgain;
    td_u32 exp_isp_dgain;
    ot_isp_ae_param_reg time_reg[10]; /* 10 */
    ot_isp_ae_param_reg again_reg[10]; /* 10 */
    ot_isp_ae_param_reg dgain_reg[10]; /* 10 */
} ot_isp_ae_convert_param;

typedef struct {
    td_u32 int_time_min[OT_ISP_WDR_MAX_FRAME_NUM];
    td_u32 int_time_max[OT_ISP_WDR_MAX_FRAME_NUM];
    td_u32 pre_int_time[OT_ISP_WDR_MAX_FRAME_NUM];
} ot_isp_ae_int_time_range;

typedef struct {
    td_u8   dark_throw_ratio;    /* RW; Range:[1, 100]; Format:8.0; */
    td_u8   bright_throw_ratio;  /* RW; Range:[1, 100]; Format:8.0; */
    td_u32  bright_compensation; /* RW; Range:[0, 16383]; Format:32.0; */
} ot_isp_thermo_sensor_default;

typedef struct {
    td_s32 (*pfn_cmos_get_ae_default)(ot_vi_pipe vi_pipe, ot_isp_ae_sensor_default *ae_sns_dft);

    /* the function of sensor set fps */
    td_void (*pfn_cmos_fps_set)(ot_vi_pipe vi_pipe, td_float f32_fps, ot_isp_ae_sensor_default *ae_sns_dft);
    td_void (*pfn_cmos_slow_framerate_set)(ot_vi_pipe vi_pipe, td_u32 full_lines, ot_isp_ae_sensor_default *ae_sns_dft);

    /* while isp notify ae to update sensor regs, ae call these funcs. */
    td_void (*pfn_cmos_inttime_update)(ot_vi_pipe vi_pipe, td_u32 int_time);
    td_void (*pfn_cmos_gains_update)(ot_vi_pipe vi_pipe, td_u32 again, td_u32 dgain);

    td_void (*pfn_cmos_again_calc_table)(ot_vi_pipe vi_pipe, td_u32 *again_lin, td_u32 *again_db);
    td_void (*pfn_cmos_dgain_calc_table)(ot_vi_pipe vi_pipe, td_u32 *dgain_lin, td_u32 *dgain_db);

    td_void (*pfn_cmos_get_inttime_max)(ot_vi_pipe vi_pipe, td_u16 man_ratio_enable, td_u32 *ratio,
                                        ot_isp_ae_int_time_range *int_time, td_u32 *lf_max_int_time);

    /* long frame mode set */
    td_void (*pfn_cmos_ae_fswdr_attr_set)(ot_vi_pipe vi_pipe, ot_isp_ae_fswdr_attr *ae_fswdr_attr);
    td_void (*pfn_cmos_ae_quick_start_status_set)(ot_vi_pipe vi_pipe, td_bool quick_start_status);
    td_void (*pfn_cmos_exp_param_convert)(ot_vi_pipe vi_pipe, ot_isp_ae_convert_param *exp_param);
    td_s32 (*pfn_cmos_get_thermo_default)(ot_vi_pipe vi_pipe, ot_isp_thermo_sensor_default *thermo_sns_dft);
} ot_isp_ae_sensor_exp_func;

typedef struct {
    ot_isp_ae_sensor_exp_func sns_exp;
} ot_isp_ae_sensor_register;

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif
