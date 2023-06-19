/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */

#ifndef OT_COMMON_AWB_H
#define OT_COMMON_AWB_H

#include "ot_type.h"
#include "ot_debug.h"
#include "ot_common_isp.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* end of #ifdef __cplusplus */

#define OT_AWB_LIB_NAME "ot_awb_lib"

typedef struct {
    td_u16 white_level;
    td_u16 black_level;
    td_u16 cr_max;
    td_u16 cr_min;
    td_u16 cb_max;
    td_u16 cb_min;
    td_u16 cr_high;
    td_u16 cr_low;
    td_u16 cb_high;
    td_u16 cb_low;
    td_u16 raw_white_level;
    td_u16 raw_black_level;
    td_u16 raw_cr_max;
    td_u16 raw_cr_min;
    td_u16 raw_cb_max;
    td_u16 raw_cb_min;
    td_u16 raw_cr_high;
    td_u16 raw_cr_low;
    td_u16 raw_cb_high;
    td_u16 raw_cb_low;

    td_u16 wdr_mode;
    td_u16 enable;
    td_u16 manual_enable;
    td_u16 zone;
    td_u16 high_temp;
    td_u16 low_temp;
    td_u16 ref_temp;
    td_u16 rgain_base;
    td_u16 ggain_base;
    td_u16 bgain_base;
    td_s32 p1;
    td_s32 p2;
    td_s32 q;
    td_s32 a;
    td_s32 c;

    td_u16 man_sat_enable;
    td_u16 sat_target;
} ot_isp_awb_dbg_attr;

typedef struct {
    td_u16 sum;
    td_u16 rg;
    td_u16 bg;
    td_u16 count_all;
    td_u16 count_min;
    td_u16 count_max;
    td_u16 raw_r_avg;
    td_u16 raw_g_avg;
    td_u16 raw_b_avg;
    td_u16 tk;
    td_u16 weight;
    td_s16 shift;
} ot_isp_awb_zone_dbg;

typedef struct {
    td_u32 frm_num_bgn;
    td_u32 global_sum;
    td_u16 global_rg_sta;
    td_u16 global_bg_sta;
    td_u16 global_count_all;
    td_u16 global_count_min;
    td_u16 global_count_max;
    td_u16 global_r_avg;
    td_u16 global_g_avg;
    td_u16 global_b_avg;
    td_u16 tk;
    td_u16 r_gain;
    td_u16 g_gain;
    td_u16 b_gain;
    td_u16 ccm[OT_ISP_CCM_MATRIX_SIZE];

    td_u32 hist_info[256];    /* 256 is size of hist info */
    ot_isp_awb_zone_dbg zone_debug[OT_ISP_AWB_ZONE_NUM];

    td_u32 frm_num_end;
} ot_isp_awb_dbg_status;

/* sensor's interface to awb */
typedef struct {
    td_u16 color_temp;              /* RW;  range:[2000,10000]; format:16.0; the current color temperature */
    td_u16 ccm[OT_ISP_CCM_MATRIX_SIZE];    /* RW;  range: [0x0, 0xFFFF]; format:8.8;
                                       CCM matrixes for different color temperature */
} ot_isp_awb_ccm_tab;

typedef struct {
    td_u16  ccm_tab_num;                   /* RW;  range: [0x3, 0x7]; format:16.0; the number of CCM matrixes */
    ot_isp_awb_ccm_tab ccm_tab[OT_ISP_CCM_MATRIX_NUM];
} ot_isp_awb_ccm;

typedef struct {
    td_bool valid;

    td_u8   saturation[OT_ISP_AUTO_ISO_NUM];   /* RW;adjust saturation, different iso with different saturation */
} ot_isp_awb_agc_table;

typedef struct {
    td_u16  wb_ref_temp;       /* RW;reference color temperature for WB  */
    td_u16  gain_offset[OT_ISP_BAYER_CHN_NUM];  /* RW; gain offset for white balance */
    td_s32  wb_para[OT_ISP_AWB_CURVE_PARA_NUM];      /* RW; parameter for wb curve,p1,p2,q1,a1,b1,c1 */

    td_u16  golden_rgain;      /* rgain for the golden sample */
    td_u16  golden_bgain;      /* bgain for the golden sample */
    td_u16  sample_rgain;      /* rgain for the current sample */
    td_u16  sample_bgain;      /* bgain for the current sample */
    ot_isp_awb_agc_table agc_tbl;
    ot_isp_awb_ccm ccm;
    td_u16    init_rgain;           /* init WB gain */
    td_u16    init_ggain;
    td_u16    init_bgain;
    td_u8     awb_run_interval;       /* RW;AWB run interval */
    td_u16    init_ccm[OT_ISP_CCM_MATRIX_SIZE];
} ot_isp_awb_sensor_default;

typedef struct {
    td_s32 (*pfn_cmos_get_awb_default)(ot_vi_pipe vi_pipe, ot_isp_awb_sensor_default *awb_sns_dft);
} ot_isp_awb_sensor_exp_func;

typedef struct {
    ot_isp_awb_sensor_exp_func sns_exp;
} ot_isp_awb_sensor_register;

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* end of #ifdef __cplusplus */

#endif
