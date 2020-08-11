/*
* Copyright (C) Hisilicon Technologies Co., Ltd. 2012-2019. All rights reserved.
* Description:
* Author: Hisilicon multimedia software group
* Create: 2011/06/28
*/

#ifndef __HI_COMM_3A_ADAPT_H__
#define __HI_COMM_3A_ADAPT_H__

#include "hi_common_adapt.h"
#include "hi_comm_isp_adapt.h"
#include "hi_comm_sns.h"
#include "hi_comm_3a.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */

typedef ISP_ALG_MOD_E  hi_isp_alg_mod;
typedef ISP_CTRL_CMD_E hi_isp_ctrl_cmd;

typedef struct {
    hi_char *proc_buff;
    hi_u32   buff_len;
    hi_u32   write_len;   /* the len count should contain '\0'. */
} hi_isp_ctrl_proc_write;

typedef struct {
    hi_bool stitch_enable;
    hi_bool main_pipe;
    hi_u8   stitch_pipe_num;
    hi_s8   stitch_bind_id[VI_MAX_PIPE_NUM];
} hi_isp_stitch_attr;

/********************************  AE  *************************************/
/* the init param of ae alg */
typedef struct {
    SENSOR_ID sensor_id;
    hi_u8  wdr_mode;
    hi_u8  hdr_mode;
    hi_u16 black_level;
    hi_float fps;
    hi_isp_bayer_format bayer;
    hi_isp_stitch_attr stitch_attr;

    hi_s32 rsv;
} hi_isp_ae_param;

/* the statistics of ae alg */
typedef struct {
    hi_u32  pixel_count[ISP_CHN_MAX_NUM];
    hi_u32  pixel_weight[ISP_CHN_MAX_NUM];
    hi_u32  histogram_mem_array[ISP_CHN_MAX_NUM][1024];
} hi_isp_fe_ae_stat_1;

typedef struct {
    hi_u16  global_avg_r[ISP_CHN_MAX_NUM];
    hi_u16  global_avg_gr[ISP_CHN_MAX_NUM];
    hi_u16  global_avg_gb[ISP_CHN_MAX_NUM];
    hi_u16  global_avg_b[ISP_CHN_MAX_NUM];
} hi_isp_fe_ae_stat_2;

typedef struct {
    hi_u16  zone_avg[ISP_CHN_MAX_NUM][AE_ZONE_ROW][AE_ZONE_COLUMN][4];
} hi_isp_fe_ae_stat_3;

typedef struct {
    hi_u16  zone_avg[VI_MAX_PIPE_NUM][ISP_CHN_MAX_NUM][AE_ZONE_ROW][AE_ZONE_COLUMN][4];
} hi_isp_fe_ae_stitch_stat_3;

typedef struct {
    hi_u32  pixel_count;
    hi_u32  pixel_weight;
    hi_u32  histogram_mem_array[1024];
} hi_isp_be_ae_stat_1;

typedef struct {
    hi_u16  global_avg_r;
    hi_u16  global_avg_gr;
    hi_u16  global_avg_gb;
    hi_u16  global_avg_b;
} hi_isp_be_ae_stat_2;

typedef struct {
    hi_u16  zone_avg[AE_ZONE_ROW][AE_ZONE_COLUMN][4];
} hi_isp_be_ae_stat_3;

typedef struct {
    hi_u16  zone_avg[VI_MAX_PIPE_NUM][AE_ZONE_ROW][AE_ZONE_COLUMN][4];
} hi_isp_be_ae_stitch_stat_3;

typedef struct {
    hi_u32  frame_cnt;    /* the counting of frame */
    hi_isp_smart_info smart_info;  /* Only used for Hi3516EV200/Hi3516EV300/Hi3518EV300 */

    hi_isp_fe_ae_stat_1 *fe_ae_stat1;
    hi_isp_fe_ae_stat_2 *fe_ae_stat2;
    hi_isp_fe_ae_stat_3 *fe_ae_stat3;
    hi_isp_fe_ae_stitch_stat_3 *fe_ae_sti_stat;
    hi_isp_be_ae_stat_1 *be_ae_stat1;
    hi_isp_be_ae_stat_2 *be_ae_stat2;
    hi_isp_be_ae_stat_3 *be_ae_stat3;
    hi_isp_be_ae_stitch_stat_3 *be_ae_sti_stat;
} hi_isp_ae_info;

typedef struct {
    hi_bool change;

    hi_bool hist_adjust;
    hi_u8 ae_be_sel;
    hi_u8 four_plane_mode;
    hi_u8 hist_offset_x;
    hi_u8 hist_offset_y;
    hi_u8 hist_skip_x;
    hi_u8 hist_skip_y;

    hi_bool mode_update;
    hi_u8 hist_mode;
    hi_u8 aver_mode;
    hi_u8 max_gain_mode;

    hi_bool wight_table_update;
    hi_u8 weight_table[VI_MAX_PIPE_NUM][AE_ZONE_ROW][AE_ZONE_COLUMN];
} hi_isp_ae_stat_attr;

/* the final calculate of ae alg */
typedef struct {
    hi_u32  int_time[AE_INT_TIME_NUM];
    hi_u32  isp_d_gain;
    hi_u32  a_gain;
    hi_u32  d_gain;
    hi_u32  iso;
    hi_u32  isp_d_gain_sf;
    hi_u32  a_gain_sf;
    hi_u32  d_gain_sf;
    hi_u32  iso_sf;
    hi_u8   ae_run_interval;

    hi_bool piris_valid;
    hi_s32  piris_pos;
    hi_u32  piris_gain;

    hi_isp_fswdr_mode fswdr_mode;
    hi_u32  wdr_gain[4];
    hi_u32  hmax_times; /* unit is ns */
    hi_u32  vmax; /* unit is line */

    hi_isp_ae_stat_attr stat_attr;
    hi_isp_dcf_update_info update_info;
} hi_isp_ae_result;

typedef struct {
    hi_s32 (*pfn_ae_init)(hi_s32 handle, const hi_isp_ae_param *ae_param);
    hi_s32 (*pfn_ae_run)(hi_s32 handle,
                         const hi_isp_ae_info *ae_info,
                         hi_isp_ae_result *ae_result,
                         hi_s32 rsv
                        );
    hi_s32 (*pfn_ae_ctrl)(hi_s32 handle, hi_u32 cmd, hi_void *value);
    hi_s32 (*pfn_ae_exit)(hi_s32 handle);
} hi_isp_ae_exp_func;

typedef struct {
    hi_isp_ae_exp_func ae_exp_func;
} hi_isp_ae_register;

/********************************  AWB  *************************************/
typedef AWB_CTRL_CMD_E hi_isp_awb_ctrl_cmd;

typedef struct {
    hi_bool awb_bypass_en;
    hi_bool manual_sat_en;
    hi_bool manual_temp_en;

    hi_u32  manual_sat_value;
    hi_u32  manual_temp_value;
    hi_u16  ccm_speed;

    hi_u16  high_ccm[9];
    hi_u16  low_ccm[9];
    hi_u16  high_color_temp;
    hi_u16  low_color_temp;
} awb_ccm_config;

/* the init param of awb alg */
typedef struct {
    SENSOR_ID sensor_id;
    hi_u8 wdr_mode;
    hi_u8 awb_zone_row;
    hi_u8 awb_zone_col;
    hi_u8 awb_zone_bin;
    hi_isp_stitch_attr stitch_attr;
    hi_u16 awb_width;
    hi_u16 awb_height;
    hi_s8 rsv;
} hi_isp_awb_param;

/* the statistics of awb alg */
typedef struct {
    hi_u16  metering_awb_avg_r;
    hi_u16  metering_awb_avg_g;
    hi_u16  metering_awb_avg_b;
    hi_u16  metering_awb_count_all;
} hi_isp_awb_stat_1;

typedef struct {
    hi_u16  metering_mem_array_avg_r[AWB_ZONE_NUM];
    hi_u16  metering_mem_array_avg_g[AWB_ZONE_NUM];
    hi_u16  metering_mem_array_avg_b[AWB_ZONE_NUM];
    hi_u16  metering_mem_array_count_all[AWB_ZONE_NUM];
} hi_isp_awb_stat_2;

typedef struct {
    hi_u16  zone_col;
    hi_u16  zone_row;
    hi_u16  zone_bin;
    hi_u16  metering_mem_array_avg_r[AWB_ZONE_STITCH_MAX];
    hi_u16  metering_mem_array_avg_g[AWB_ZONE_STITCH_MAX];
    hi_u16  metering_mem_array_avg_b[AWB_ZONE_STITCH_MAX];
    hi_u16  metering_mem_array_count_all[AWB_ZONE_STITCH_MAX];
} hi_isp_awb_stat_stitch;

typedef struct {
    hi_u16 *zone_avg_r;
    hi_u16 *zone_avg_g;
    hi_u16 *zone_avg_b;
    hi_u16 *zone_count;
} hi_isp_awb_stat_result;

typedef struct {
    hi_u32  frame_cnt;

    hi_isp_awb_stat_1 *awb_stat1;
    hi_isp_awb_stat_result awb_stat2;
    hi_u8  awb_gain_switch;
    hi_u8  awb_stat_switch;
    hi_bool wb_gain_in_sensor;
    hi_u32 wdr_wb_gain[ISP_BAYER_CHN_NUM];
} hi_isp_awb_info;

/* the statistics's attr of awb alg */
typedef struct {
    hi_bool stat_cfg_update;

    hi_u16  metering_white_level_awb;
    hi_u16  metering_black_level_awb;
    hi_u16  metering_cr_ref_max_awb;
    hi_u16  metering_cb_ref_max_awb;
    hi_u16  metering_cr_ref_min_awb;
    hi_u16  metering_cb_ref_min_awb;
} hi_isp_awb_raw_stat_attr;

/* the final calculate of awb alg */
typedef struct {
    hi_u32  white_balance_gain[ISP_BAYER_CHN_NUM];
    hi_u16  color_matrix[CCM_MATRIX_SIZE];
    hi_u32  color_temp;
    hi_u8   saturation;
    hi_isp_awb_raw_stat_attr raw_stat_attr;
} hi_isp_awb_result;

typedef struct {
    hi_s32 (*pfn_awb_init)(hi_s32 handle, const hi_isp_awb_param *awb_param, hi_isp_awb_result *awb_result);
    hi_s32 (*pfn_awb_run)(hi_s32 handle,
                          const hi_isp_awb_info *awb_info,
                          hi_isp_awb_result *awb_result,
                          hi_s32 rsv);
    hi_s32 (*pfn_awb_ctrl)(hi_s32 handle, hi_u32 cmd, hi_void *value);
    hi_s32 (*pfn_awb_exit)(hi_s32 handle);
} hi_isp_awb_exp_func;

typedef struct {
    hi_isp_awb_exp_func awb_exp_func;
} hi_isp_awb_register;

/********************************  AF  *************************************/
/* the statistics of af alg */
typedef struct {
    hi_u16  v1;
    hi_u16  h1;
    hi_u16  v2;
    hi_u16  h2;
    hi_u16  y;
    hi_u16  hl_cnt;
} hi_isp_af_zone;

typedef struct {
    hi_isp_af_zone zone_metrics[WDR_CHN_MAX][AF_ZONE_ROW][AF_ZONE_COLUMN]; /* R; the zoned measure of contrast */
} hi_isp_fe_af_stat;

typedef struct {
    hi_isp_af_zone zone_metrics[AF_ZONE_ROW][AF_ZONE_COLUMN]; /* R; the zoned measure of contrast */
} hi_isp_be_af_stat;

typedef struct {
    hi_s32  id;
    hi_char lib_name[ALG_LIB_NAME_SIZE_MAX];
} hi_isp_alg_lib;

typedef struct {
    SENSOR_ID       sensor_id;
    hi_isp_alg_lib  ae_lib;
    hi_isp_alg_lib  af_lib;
    hi_isp_alg_lib  awb_lib;
} hi_isp_bind_attr;

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif /* __HI_COMM_3A_ADAPT_H__ */
