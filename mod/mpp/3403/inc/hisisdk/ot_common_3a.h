/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */

#ifndef OT_COMMON_3A_H
#define OT_COMMON_3A_H

#include "ot_common.h"
#include "ot_common_isp.h"
#include "ot_common_sns.h"
#include "ot_common_video.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */

#define is_linear_mode(mode)      ((mode) == OT_WDR_MODE_NONE)
#define is_wdr_mode(mode)         ((!is_linear_mode(mode)) && ((mode) < OT_WDR_MODE_BUTT))
#define is_hdr_mode(mode)         (((mode) > OT_DYNAMIC_RANGE_SDR10) && ((mode) < OT_DYNAMIC_RANGE_BUTT))
#define is_built_in_wdr_mode(mode) ((mode) == OT_WDR_MODE_BUILT_IN)
#define is_fs_wdr_mode(mode)      ((!is_linear_mode(mode))&& (!is_built_in_wdr_mode(mode)) && \
                                   ((mode) < OT_WDR_MODE_BUTT))
#define is_2to1_wdr_mode(mode)    (((mode) == OT_WDR_MODE_2To1_FRAME) || \
                                   ((mode) == OT_WDR_MODE_2To1_LINE) || ((mode) == OT_WDR_MODE_QUDRA))
#define is_3to1_wdr_mode(mode)    (((mode) == OT_WDR_MODE_3To1_FRAME) || ((mode) == OT_WDR_MODE_3To1_LINE))
#define is_4to1_wdr_mode(mode)    (((mode) == OT_WDR_MODE_4To1_FRAME) || ((mode) == OT_WDR_MODE_4To1_LINE))
#define is_full_wdr_mode(mode)     0

#define is_half_wdr_mode(mode)    (((mode) == OT_WDR_MODE_2To1_FRAME) || ((mode) == OT_WDR_MODE_3To1_FRAME) || \
                                   ((mode) == OT_WDR_MODE_4To1_FRAME))
#define is_line_wdr_mode(mode)    (((mode) == OT_WDR_MODE_2To1_LINE) || ((mode) == OT_WDR_MODE_3To1_LINE) || \
                                   ((mode) == OT_WDR_MODE_4To1_LINE) || ((mode) == OT_WDR_MODE_QUDRA))

#define is_stitch_main_pipe(vi_pipe, main_pipe) ((vi_pipe) == (main_pipe))
#define is_offline_mode(mode)    ((mode) == ISP_MODE_RUNNING_OFFLINE)
#define is_online_mode(mode)     ((mode) == ISP_MODE_RUNNING_ONLINE)
#define is_sidebyside_mode(mode) ((mode) == ISP_MODE_RUNNING_SIDEBYSIDE)
#define is_striping_mode(mode)   ((mode) == ISP_MODE_RUNNING_STRIPING)
#define is_pre_online_post_offline(mode)  ((mode) == ISP_MODE_RUNNING_PRE_ONLINE_POST_OFFLINE)
#define is_virt_pipe(vi_pipe)    (((vi_pipe) >= OT_ISP_MAX_PHY_PIPE_NUM) && ((vi_pipe) < OT_ISP_MAX_PIPE_NUM))

#define OT_ISP_MAX_REGISTER_ALG_LIB_NUM 2

typedef enum {
    OT_ISP_ALG_AE = 0,
    OT_ISP_ALG_AF,
    OT_ISP_ALG_AWB,
    OT_ISP_ALG_BLC,
    OT_ISP_ALG_DP,
    OT_ISP_ALG_PREGAMMA,
    OT_ISP_ALG_AGAMMA,
    OT_ISP_ALG_ADGAMMA,
    OT_ISP_ALG_DRC,
    OT_ISP_ALG_DEMOSAIC,
    OT_ISP_ALG_ANTIFALSECOLOR,
    OT_ISP_ALG_GAMMA,
    OT_ISP_ALG_GE,
    OT_ISP_ALG_SHARPEN,
    OT_ISP_ALG_FRAMEWDR,
    OT_ISP_ALG_FPN,
    OT_ISP_ALG_DEHAZE,
    OT_ISP_ALG_LCAC,
    OT_ISP_ALG_ACAC,
    OT_ISP_ALG_BSHP,
    OT_ISP_ALG_CSC,
    OT_ISP_ALG_EXPANDER,
    OT_ISP_ALG_MCDS,
    OT_ISP_ALG_ACS,
    OT_ISP_ALG_LSC,
    OT_ISP_ALG_RC,
    OT_ISP_ALG_RGBIR,
    OT_ISP_ALG_HRS,
    OT_ISP_ALG_DG,
    OT_ISP_ALG_BAYERNR,
    OT_ISP_ALG_FLICKER,
    OT_ISP_ALG_LDCI,
    OT_ISP_ALG_CA,
    OT_ISP_ALG_CLUT,
    OT_ISP_ALG_CCM,
    OT_ISP_ALG_AI,
    OT_ISP_ALG_CRB,
    OT_ISP_ALG_HNR,
    OT_ISP_ALG_MOD_BUTT,
} ot_isp_alg_mod;

typedef enum {
    OT_ISP_WDR_MODE_SET = 8000,
    OT_ISP_PROC_WRITE,

    OT_ISP_AE_FPS_BASE_SET,
    OT_ISP_AE_BLC_SET,
    OT_ISP_AE_RC_SET,
    OT_ISP_AE_BAYER_FORMAT_SET,
    OT_ISP_AE_INIT_INFO_GET,

    OT_ISP_AWB_ISO_SET,  /* set iso, change saturation when iso change */
    OT_ISP_CHANGE_IMAGE_MODE_SET,
    OT_ISP_UPDATE_INFO_GET,
    OT_ISP_FRAMEINFO_GET,
    OT_ISP_ATTACHINFO_GET,
    OT_ISP_COLORGAMUTINFO_GET,
    OT_ISP_AWB_INTTIME_SET,
    OT_ISP_BAS_MODE_SET,
    OT_ISP_PROTRIGGER_SET,
    OT_ISP_AWB_PIRIS_SET,
    OT_ISP_AWB_SNAP_MODE_SET,
    OT_ISP_AWB_ZONE_ROW_SET,
    OT_ISP_AWB_ZONE_COL_SET,
    OT_ISP_AWB_ZONE_BIN_SET,
    OT_ISP_AWB_ERR_GET,
    OT_ISP_CTRL_CMD_BUTT,
} ot_isp_ctrl_cmd;

typedef struct {
    td_char *proc_buff;
    td_u32   buff_len;
    td_u32   write_len;   /* the len count should contain '\0'. */
} ot_isp_ctrl_proc_write;

typedef struct {
    td_bool stitch_enable;
    td_bool main_pipe;
    td_u8   stitch_pipe_num;
    td_s8   stitch_bind_id[OT_ISP_MAX_STITCH_NUM];
} ot_isp_stitch_attr;

/* AE */
/* the init param of ae alg */
typedef struct {
    ot_sensor_id sensor_id;
    td_u8  wdr_mode;
    td_u8  hdr_mode;
    td_u16 black_level;
    td_float fps;
    ot_isp_bayer_format bayer;
    ot_isp_stitch_attr stitch_attr;

    td_s32 reserved;
} ot_isp_ae_param;

/* the statistics of ae alg */
typedef struct {
    td_u32  pixel_count[OT_ISP_WDR_MAX_FRAME_NUM];
    td_u32  pixel_weight[OT_ISP_WDR_MAX_FRAME_NUM];
    td_u32  histogram_mem_array[OT_ISP_WDR_MAX_FRAME_NUM][OT_ISP_HIST_NUM];
    td_u32  histogram_mem_array_ir[OT_ISP_HIST_NUM]; /* only support fe0 */
} ot_isp_fe_ae_stat_1;

typedef struct {
    td_u16  global_avg_r[OT_ISP_WDR_MAX_FRAME_NUM];
    td_u16  global_avg_gr[OT_ISP_WDR_MAX_FRAME_NUM];
    td_u16  global_avg_gb[OT_ISP_WDR_MAX_FRAME_NUM];
    td_u16  global_avg_b[OT_ISP_WDR_MAX_FRAME_NUM];
    td_u16  global_avg_ir; /* only support fe0 */
} ot_isp_fe_ae_stat_2;

typedef struct {
    td_u16  zone_avg[OT_ISP_WDR_MAX_FRAME_NUM][OT_ISP_AE_ZONE_ROW][OT_ISP_AE_ZONE_COLUMN][OT_ISP_BAYER_CHN_NUM];
    td_u16  zone_avg_ir[OT_ISP_AE_ZONE_ROW][OT_ISP_AE_ZONE_COLUMN]; /* only support fe0 */
} ot_isp_fe_ae_stat_3;

typedef struct {
    td_u16  zone_avg[OT_VI_MAX_PIPE_NUM][OT_ISP_WDR_MAX_FRAME_NUM][OT_ISP_AE_ZONE_ROW][OT_ISP_AE_ZONE_COLUMN]\
        [OT_ISP_BAYER_CHN_NUM];
} ot_isp_fe_ae_stitch_stat_3;

typedef struct {
    td_u32  pixel_count;
    td_u32  pixel_weight;
    td_u32  histogram_mem_array[OT_ISP_HIST_NUM];
    td_u32  estimate_histogram_mem_array[OT_ISP_HIST_NUM];
} ot_isp_be_ae_stat_1;

typedef struct {
    td_u16  global_avg_r;
    td_u16  global_avg_gr;
    td_u16  global_avg_gb;
    td_u16  global_avg_b;
} ot_isp_be_ae_stat_2;

typedef struct {
    td_u16  zone_avg[OT_ISP_AE_ZONE_ROW][OT_ISP_AE_ZONE_COLUMN][OT_ISP_BAYER_CHN_NUM];
} ot_isp_be_ae_stat_3;

typedef struct {
    td_u16  zone_avg[OT_VI_MAX_PIPE_NUM][OT_ISP_AE_ZONE_ROW][OT_ISP_AE_ZONE_COLUMN][OT_ISP_BAYER_CHN_NUM];
} ot_isp_be_ae_stitch_stat_3;

typedef struct {
    td_u32  frame_cnt;    /* the counting of frame */
    td_u64  frame_pts;
    td_u16  frame_width;
    td_u16  frame_height;
    ot_isp_smart_info smart_info;

    ot_isp_fe_ae_stat_1 *fe_ae_stat1;
    ot_isp_fe_ae_stat_2 *fe_ae_stat2;
    ot_isp_fe_ae_stat_3 *fe_ae_stat3;
    ot_isp_fe_ae_stitch_stat_3 *fe_ae_sti_stat;
    ot_isp_be_ae_stat_1 *be_ae_stat1;
    ot_isp_be_ae_stat_2 *be_ae_stat2;
    ot_isp_be_ae_stat_3 *be_ae_stat3;
    ot_isp_be_ae_stitch_stat_3 *be_ae_sti_stat;
} ot_isp_ae_info;

typedef struct {
    td_bool change;

    td_bool hist_adjust;
    td_u8 ae_be_sel;
    td_u8 four_plane_mode;
    td_u8 hist_offset_x;
    td_u8 hist_offset_y;
    td_u8 hist_skip_x;
    td_u8 hist_skip_y;

    td_bool mode_update;
    td_u8 hist_mode;
    td_u8 aver_mode;
    td_u8 max_gain_mode;

    td_bool wight_table_update;
    td_u8 weight_table[OT_VI_MAX_PIPE_NUM][OT_ISP_AE_ZONE_ROW][OT_ISP_AE_ZONE_COLUMN];
} ot_isp_ae_stat_attr;

/* the final calculate of ae alg */
#define AE_INT_TIME_NUM          4
typedef struct {
    td_u32  int_time[AE_INT_TIME_NUM];
    td_u32  isp_dgain;
    td_u32  again;
    td_u32  dgain;
    td_u32  iso;
    td_u32  isp_dgain_sf;
    td_u32  again_sf;
    td_u32  dgain_sf;
    td_u32  iso_sf;
    td_u8   ae_run_interval;

    td_bool piris_valid;
    td_s32  piris_pos;
    td_u32  piris_gain;
    td_u32  sns_lhcg_exp_ratio;

    ot_isp_fswdr_mode fswdr_mode;
    td_u32  wdr_gain[OT_ISP_WDR_MAX_FRAME_NUM];
    td_u32  hmax_times; /* unit is ns */
    td_u32  vmax; /* unit is line */

    ot_isp_ae_stat_attr stat_attr;
    ot_isp_dcf_update_info update_info;
} ot_isp_ae_result;

typedef struct {
    td_u32 isp_dgain;
    td_u32 iso;
} ot_isp_ae_init_info;

typedef struct {
    td_s32 (*pfn_ae_init)(td_s32 handle, const ot_isp_ae_param *ae_param);
    td_s32 (*pfn_ae_run)(td_s32 handle,
                         const ot_isp_ae_info *ae_info,
                         ot_isp_ae_result *ae_result,
                         td_s32 reserved);
    td_s32 (*pfn_ae_ctrl)(td_s32 handle, td_u32 cmd, td_void *value);
    td_s32 (*pfn_ae_exit)(td_s32 handle);
} ot_isp_ae_exp_func;

typedef struct {
    ot_isp_ae_exp_func ae_exp_func;
} ot_isp_ae_register;

/* AWB */
typedef enum {
    OT_ISP_AWB_CCM_CONFIG_SET,
    OT_ISP_AWB_CCM_CONFIG_GET,

    OT_ISP_AWB_DEBUG_ATTR_SET,
    OT_ISP_AWB_DEBUG_ATTR_GET,

    OT_ISP_AWB_CTRL_BUTT,
} ot_isp_awb_ctrl_cmd;

typedef struct {
    td_bool awb_bypass_en;
    td_bool manual_sat_en;
    td_bool manual_temp_en;

    td_u32  manual_sat_value;
    td_u32  manual_temp_value;
    td_u16  ccm_speed;

    td_u16  high_ccm[OT_ISP_CCM_MATRIX_SIZE];
    td_u16  low_ccm[OT_ISP_CCM_MATRIX_SIZE];
    td_u16  high_color_temp;
    td_u16  low_color_temp;
} awb_ccm_config;

/* the init param of awb alg */
typedef struct {
    ot_sensor_id sensor_id;
    td_u8 wdr_mode;
    td_u8 awb_zone_row;
    td_u8 awb_zone_col;
    td_u8 awb_zone_bin;
    ot_isp_stitch_attr stitch_attr;
    td_u16 awb_width;
    td_u16 awb_height;
    td_u32 init_iso;
    td_s8 reserved;
} ot_isp_awb_param;

/* the statistics of awb alg */
typedef struct {
    td_u16  metering_awb_avg_r;
    td_u16  metering_awb_avg_g;
    td_u16  metering_awb_avg_b;
    td_u16  metering_awb_count_all;
} ot_isp_awb_stat_1;

typedef struct {
    td_u16  metering_mem_array_avg_r[OT_ISP_AWB_ZONE_NUM];
    td_u16  metering_mem_array_avg_g[OT_ISP_AWB_ZONE_NUM];
    td_u16  metering_mem_array_avg_b[OT_ISP_AWB_ZONE_NUM];
    td_u16  metering_mem_array_count_all[OT_ISP_AWB_ZONE_NUM];
} ot_isp_awb_stat_2;

typedef struct {
    td_u16  zone_col;
    td_u16  zone_row;
    td_u16  zone_bin;
    td_u16  metering_mem_array_avg_r[OT_ISP_AWB_ZONE_STITCH_MAX];
    td_u16  metering_mem_array_avg_g[OT_ISP_AWB_ZONE_STITCH_MAX];
    td_u16  metering_mem_array_avg_b[OT_ISP_AWB_ZONE_STITCH_MAX];
    td_u16  metering_mem_array_count_all[OT_ISP_AWB_ZONE_STITCH_MAX];
} ot_isp_awb_stat_stitch;

typedef struct {
    td_u16 *zone_avg_r;
    td_u16 *zone_avg_g;
    td_u16 *zone_avg_b;
    td_u16 *zone_count;
} ot_isp_awb_stat_result;

typedef struct {
    td_u32  frame_cnt;

    ot_isp_awb_stat_1 *awb_stat1;
    ot_isp_awb_stat_result awb_stat2;
    td_u8  awb_gain_switch;
    td_u8  awb_stat_switch;
    td_bool wb_gain_in_sensor;
    td_u32 wdr_wb_gain[OT_ISP_BAYER_CHN_NUM];
} ot_isp_awb_info;

/* the statistics's attr of awb alg */
typedef struct {
    td_bool stat_cfg_update;

    td_u16  metering_white_level_awb;
    td_u16  metering_black_level_awb;
    td_u16  metering_cr_ref_max_awb;
    td_u16  metering_cb_ref_max_awb;
    td_u16  metering_cr_ref_min_awb;
    td_u16  metering_cb_ref_min_awb;
} ot_isp_awb_raw_stat_attr;

/* the final calculate of awb alg */
typedef struct {
    td_u32  white_balance_gain[OT_ISP_BAYER_CHN_NUM];
    td_u16  color_matrix[OT_ISP_CCM_MATRIX_SIZE];
    td_u32  color_temp;
    td_u8   saturation;
    ot_isp_awb_raw_stat_attr raw_stat_attr;
} ot_isp_awb_result;

typedef struct {
    td_s32 (*pfn_awb_init)(td_s32 handle, const ot_isp_awb_param *awb_param, ot_isp_awb_result *awb_result);
    td_s32 (*pfn_awb_run)(td_s32 handle,
                          const ot_isp_awb_info *awb_info,
                          ot_isp_awb_result *awb_result,
                          td_s32 reserved);
    td_s32 (*pfn_awb_ctrl)(td_s32 handle, td_u32 cmd, td_void *value);
    td_s32 (*pfn_awb_exit)(td_s32 handle);
} ot_isp_awb_exp_func;

typedef struct {
    ot_isp_awb_exp_func awb_exp_func;
} ot_isp_awb_register;

/* AF */
/* the statistics of af alg */
typedef struct {
    td_u16  v1;
    td_u16  h1;
    td_u16  v2;
    td_u16  h2;
    td_u16  y;
    td_u16  hl_cnt;
} ot_isp_af_zone;

typedef struct {
    ot_isp_af_zone zone_metrics[OT_ISP_WDR_MAX_FRAME_NUM][OT_ISP_AF_ZONE_ROW][OT_ISP_AF_ZONE_COLUMN]; /* R;
                                                                                       the zoned measure of contrast */
} ot_isp_fe_af_stat;

typedef struct {
    ot_isp_af_zone zone_metrics[OT_ISP_AF_ZONE_ROW][OT_ISP_AF_ZONE_COLUMN]; /* R; the zoned measure of contrast */
} ot_isp_be_af_stat;

#define ALG_LIB_NAME_SIZE_MAX    20
typedef struct {
    td_s32  id;
    td_char lib_name[ALG_LIB_NAME_SIZE_MAX];
} ot_isp_3a_alg_lib;

typedef struct {
    ot_sensor_id       sensor_id;
    ot_isp_3a_alg_lib  ae_lib;
    ot_isp_3a_alg_lib  af_lib;
    ot_isp_3a_alg_lib  awb_lib;
} ot_isp_bind_attr;

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif /* OT_COMM_3A_H */
