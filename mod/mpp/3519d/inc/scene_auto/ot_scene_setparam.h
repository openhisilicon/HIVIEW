/*
  Copyright (c), 2001-2024, Shenshu Tech. Co., Ltd.
 */

#ifndef HI_SCENE_SETPARAM_H
#define HI_SCENE_SETPARAM_H

#include "hi_common.h"
#include "hi_common_isp.h"
#include "hi_common_vi.h"
#include "hi_common_vpss.h"
#include "hi_common_venc.h"
#include "hi_common_mcf.h"
#include "hi_mpi_aidrc.h"
#include "hi_mpi_ai3dnr.h"

#ifdef __cplusplus
extern "C" {
#endif
#pragma pack(4)

#define HI_SCENE_PIPETYPE_NUM 4

#define HI_SCENE_FPS_ISO_MAX_COUNT 10
#define HI_SCENE_AE_EXPOSURE_MAX_COUNT 12
#define HI_SCENE_DEHAZE_LUT_NUM 256
#define HI_SCENE_ISO_STRENGTH_NUM 16
#define HI_SCENE_RATIO_STRENGTH_NUM 11
#define HI_SCENE_SHADING_EXPOSURE_MAX_COUNT 10
#define HI_SCENE_GAMMA_EXPOSURE_MAX_COUNT 10
#define HI_SCENE_DRC_ISO_MAX_COUNT 16
#define HI_SCENE_DRC_REF_RATIO_MAX_COUNT 12
#define HI_SCENE_DRC_RATIO_MAX_COUNT 12
#define HI_SCENE_REF_EXP_RATIO_FRM 16
#define HI_SCENE_NR_LUT_LENGTH 33
#define HI_SCENE_NR_LUT_LENGTH1 32
#define HI_SCENE_NR_RATIO_MAX_COUNT 16
#define HI_SCENE_FALSECOLOR_EXPOSURE_MAX_COUNT 10
#define HI_SCENE_DEMOSAIC_EXPOSURE_MAX_COUNT 8
#define HI_SCENE_DIS_ISO_MAX_COUNT 3
#define HI_SCENE_LDCI_EXPOSURE_MAX_COUNT 6
#define HI_SCENE_FSWDR_ISO_MAX_COUNT 3
#define HI_SCENE_DEHAZE_EXPOSURE_MAX_COUNT 16
#define HI_SCENE_3DNR_MAX_COUNT 16
#define HI_SCENE_3DNR_IDX_LEN 17
#define ISP_AUTO_ISO_CA_NUM 16
#define ISP_SCENE_CA_MAX_BLEND_WEIGHT 256
#define ISP_SCENE_CA_SHIFT 8
#define ISP_AUTO_ISO_BLC_NUM 16
#define HI_SCENE_MCF_CC_UV_GAIN_LUT_NUM 256
#define HI_SCENE_MCF_WEIGHT_LUT_NUM  33
#define ISP_AUTO_ISO_DPC_NUM 16
#define FPN_GAIN_ISO 100
#define FPN_GAIN_SHIFT 20
#define FPN_CORRECTION_STR 256
#define DIR_NAME_BUFF 200
#define ISP_AUTO_RATIO_FSWDR_NUM 9
#define AIBNR_ISO_NUM 2
#define VENC_MODE_ISO_NUM 2
#define HI_SCENE_BAYERNR_DENOISE_Y_LUT_NUM      16
#define HI_SCENE_BSHP_CURVE_NUM    8
#define HI_SCENE_ACAC_THR_NUM             2

#undef  _tmprt1_5
#define _tmprt1_5(ps,    X)  ps[0].X, ps[1].X, ps[2].X, ps[3].X, ps[4].X

#undef  _tmprt1_4
#define _tmprt1_4(ps,    X)  ps[0].X, ps[1].X, ps[2].X, ps[3].X

#undef  _tmprt1y
#define _tmprt1y(ps,    X)  ps[1].X, ps[2].X
#undef  _tmprt1z
#define _tmprt1z(ps, a, b, X)  ps[a].X, ps[b].X

#undef  _tmprt2_3
#define _tmprt2_3(ps, X, Y)  ps[0].X, ps[0].Y,  ps[1].X, ps[1].Y,  ps[2].X, ps[2].Y

#undef  _tmprt2_4
#define _tmprt2_4(ps, X, Y)  ps[0].X, ps[0].Y,  ps[1].X, ps[1].Y,  ps[2].X, ps[2].Y,  ps[3].X, ps[3].Y

#undef  _tmprt2_4x
#define _tmprt2_4x(ps, X, Y) ps[1].X, ps[1].Y,  ps[2].X, ps[2].Y,  ps[3].X, ps[3].Y,  ps[4].X, ps[4].Y


#undef  _tmprt2_5
#define _tmprt2_5(ps, X, Y)  ps[0].X, ps[0].Y, ps[1].X, ps[1].Y,  ps[2].X, ps[2].Y, ps[3].X, ps[3].Y, ps[4].X, ps[4].Y

#undef  _tmprt2y
#define _tmprt2y(ps, X, Y)  ps[0].X, ps[0].Y,  ps[1].X, ps[1].Y,  ps[2].X, ps[2].Y

#undef  _tmprt2z
#define _tmprt2z(ps, a, b, X, Y)  ps[a].X, ps[a].Y,  ps[b].X, ps[b].Y

#undef  _t3_
#define _t3_(ie, n, X, Y, Z)            ie[n].X, ie[n].Y, ie[n].Z
#undef  _t4_
#define _t4_(ie, n, K, X, Y, Z)   ie[n].K, ie[n].X, ie[n].Y, ie[n].Z
#undef  _t5_
#define _t5_(ie, n, J, K, X, Y, Z)  ie[n].J, ie[n].K, ie[n].X, ie[n].Y, ie[n].Z

#undef  _t4_0_
#define _t4_0_(pc, X)   (pc)->X[0x00], (pc)->X[0x01], (pc)->X[0x02], (pc)->X[0x03]
#undef  _t4_4_
#define _t4_4_(pc, X)   (pc)->X[0x04], (pc)->X[0x05], (pc)->X[0x06], (pc)->X[0x07]
#undef  _t4_8_
#define _t4_8_(pc, X)   (pc)->X[0x08], (pc)->X[0x09], (pc)->X[0x0a], (pc)->X[0x0b]
#undef  _t4_12_
#define _t4_12_(pc, X)  (pc)->X[0x0c], (pc)->X[0x0d], (pc)->X[0x0e], (pc)->X[0x0f]

#undef  _t4a_0_
#define _t4a_0_(ps, a, X)   ps[a].X[0x00],  ps[a].X[0x01], ps[a].X[0x02], ps[a].X[0x03]
#undef  _t4a_4_
#define _t4a_4_(ps, a, X)   ps[a].X[0x04],  ps[a].X[0x05], ps[a].X[0x06], ps[a].X[0x07]
#undef  _t4a_8_
#define _t4a_8_(ps, a, X)   ps[a].X[0x08],  ps[a].X[0x09], ps[a].X[0x0a], ps[a].X[0x0b]
#undef  _t4a_12_
#define _t4a_12_(ps, a, X)   ps[a].X[0x0c],  ps[a].X[0x0d], ps[a].X[0x0e], ps[a].X[0x0f]

#undef  _t4a_10_
#define _t4a_10_(ps, a, X)   ps[a].X[0x10],  ps[a].X[0x11], ps[a].X[0x12], ps[a].X[0x13]
#undef  _t4a_14_
#define _t4a_14_(ps, a, X)   ps[a].X[0x14],  ps[a].X[0x15], ps[a].X[0x16], ps[a].X[0x17]
#undef  _t4a_18_
#define _t4a_18_(ps, a, X)   ps[a].X[0x18],  ps[a].X[0x19], ps[a].X[0x1a], ps[a].X[0x1b]
#undef  _t4a_1c_
#define _t4a_1c_(ps, a, X)   ps[a].X[0x1c],  ps[a].X[0x1d], ps[a].X[0x1e], ps[a].X[0x1f]


#undef  _tmprt3z
#define _tmprt3z(ps, a, b, X, Y, Z)  _t3_(ps, a, X, Y, Z), _t3_(ps, b, X, Y, Z)
#undef  _tmprt3y
#define _tmprt3y(ps, X, Y, Z)  _t3_(ps, 0, X, Y, Z), _t3_(ps, 1, X, Y, Z), _t3_(ps, 2, X, Y, Z)
#undef  _tmprt3x
#define _tmprt3x(ps, X, Y, Z)  _t3_(ps, 1, X, Y, Z), _t3_(ps, 2, X, Y, Z), _t3_(ps, 3, X, Y, Z)
#undef  _tmprt3_
#define _tmprt3_(ps, X, Y, Z)  _t3_(ps, 0, X, Y, Z), _t3_(ps, 1, X, Y, Z), _t3_(ps, 2, X, Y, Z), _t3_(ps, 3, X, Y, Z)

#undef  _tmprt3_5
#define _tmprt3_5(ps, X, Y, Z) \
    _t3_(ps, 0, X, Y, Z), _t3_(ps, 1, X, Y, Z), _t3_(ps, 2, X, Y, Z), _t3_(ps, 3, X, Y, Z), _t3_(ps, 4, X, Y, Z)

#undef  _tmprt3_4
#define _tmprt3_4(ps, X, Y, Z)  _t3_(ps, 1, X, Y, Z), _t3_(ps, 2, X, Y, Z), _t3_(ps, 3, X, Y, Z), _t3_(ps, 4, X, Y, Z)


#undef  _tmprt3x_
#define _tmprt3x_(ps, X, Y, Z) \
    _t3_(ps, 0, X, Y, Z), _t3_(ps, 1, X, Y, Z), _t3_(ps, 2, X, Y, Z), _t3_(ps, 3, X, Y, Z)

#undef  _tmprt4_5
#define _tmprt4_5(ps, K, X, Y, Z) \
    _t4_(ps, 0, K, X, Y, Z), _t4_(ps, 1, K, X, Y, Z), _t4_(ps, 2, K, X, Y, Z), \
    _t4_(ps, 3, K, X, Y, Z), _t4_(ps, 4, K, X, Y, Z)

#undef  _tmprt4_4
#define _tmprt4_4(ps, K, X, Y, Z) \
    _t4_(ps, 1, K, X, Y, Z), _t4_(ps, 2, K, X, Y, Z), _t4_(ps, 3, K, X, Y, Z), _t4_(ps, 4, K, X, Y, Z)

#undef  _tmprt4_2
#define _tmprt4_2(ps, K, X, Y, Z)  _t4_(ps, 1, K, X, Y, Z), _t4_(ps, 2, K, X, Y, Z)

#undef  _tmprt5_5
#define _tmprt5_5(ps, J, K, X, Y, Z) \
    _t5_(ps, 0, J, K, X, Y, Z), _t5_(ps, 1, J, K, X, Y, Z), \
    _t5_(ps, 2, J, K, X, Y, Z), _t5_(ps, 3, J, K, X, Y, Z), _t5_(ps, 4, J, K, X, Y, Z)

#undef  _tmprt5_3
#define _tmprt5_3(ps, J, K, X, Y, Z)  _t5_(ps, 0, J, K, X, Y, Z), _t5_(ps, 1, J, K, X, Y, Z), _t5_(ps, 2, J, K, X, Y, Z)

#undef  _tmprt4_3
#define _tmprt4_3(ps, K, X, Y, Z)  _t4_(ps, 0, K, X, Y, Z), _t4_(ps, 1, K, X, Y, Z), _t4_(ps, 2, K, X, Y, Z)

#undef  _tmprt4x_
#define _tmprt4x_(ps, K, X, Y, Z) \
    _t4_(ps, 0, K, X, Y, Z), _t4_(ps, 1, K, X, Y, Z), _t4_(ps, 2, K, X, Y, Z), _t4_(ps, 3, K, X, Y, Z)

#define scene_set_dynamic_linear_drc(target, member) do {                                                        \
        if ((iso_level == 0) || (iso_level == (iso_count - 1))) {                                                \
            target = g_scene_pipe_param[index].dynamic_linear_drc.member[iso_level];                             \
        } else {                                                                                                 \
            target = scene_interpulate(iso, iso_level_thresh[iso_level - 1],                                     \
                g_scene_pipe_param[index].dynamic_linear_drc.member[iso_level - 1], iso_level_thresh[iso_level], \
                g_scene_pipe_param[index].dynamic_linear_drc.member[iso_level]);                                 \
        }                                                                                                        \
    } while (0)

#define scene_set_dynamic_drc_member_piso(member, temp_member) do {                                       \
        if ((iso_level == 0) || (iso_level == (iso_count - 1))) {                                         \
            temp_member = g_scene_pipe_param[index].dynamic_drc.member[iso_level];                        \
        } else {                                                                                          \
            temp_member = scene_interpulate(iso, iso_level_thresh[iso_level - 1],                         \
                g_scene_pipe_param[index].dynamic_drc.member[iso_level - 1], iso_level_thresh[iso_level], \
                g_scene_pipe_param[index].dynamic_drc.member[iso_level]);                                 \
        }                                                                                                 \
    } while (0)

#define scene_set_dynamic_aidrc_member_piso(member, temp_member) do {                                       \
        if ((iso_level == 0) || (iso_level == (iso_count - 1))) {                                         \
            temp_member = g_scene_pipe_param[index].dynamic_aidrc.member[iso_level];                        \
        } else {                                                                                          \
            temp_member = scene_interpulate(iso, iso_level_thresh[iso_level - 1],                         \
                g_scene_pipe_param[index].dynamic_aidrc.member[iso_level - 1], iso_level_thresh[iso_level], \
                g_scene_pipe_param[index].dynamic_aidrc.member[iso_level]);                                 \
        }                                                                                                 \
    } while (0)

#define scene_set_dynamic_drc_member_pratio(member, temp_member) do {                                           \
        if ((ratio_level == 0) || (ratio_level == (ratio_count - 1))) {                                         \
            temp_member = g_scene_pipe_param[index].dynamic_drc.member[ratio_level];                            \
        } else {                                                                                                \
            temp_member = scene_interpulate(wdr_ratio, ratio_level_thresh[ratio_level - 1],                     \
                g_scene_pipe_param[index].dynamic_drc.member[ratio_level - 1], ratio_level_thresh[ratio_level], \
                g_scene_pipe_param[index].dynamic_drc.member[ratio_level]);                                     \
        }                                                                                                       \
    } while (0)

#define scene_set_dynamic_aidrc_member_pratio(member, temp_member) do {                                           \
        if ((ratio_level == 0) || (ratio_level == (ratio_count - 1))) {                                         \
            temp_member = g_scene_pipe_param[index].dynamic_aidrc.member[ratio_level];                            \
        } else {                                                                                                \
            temp_member = scene_interpulate(wdr_ratio, ratio_level_thresh[ratio_level - 1],                     \
                g_scene_pipe_param[index].dynamic_aidrc.member[ratio_level - 1], ratio_level_thresh[ratio_level], \
                g_scene_pipe_param[index].dynamic_aidrc.member[ratio_level]);                                     \
        }                                                                                                       \
    } while (0)

#define scene_set_drc_member(target, member, temp_member, flag) do {                                        \
        if (flag == 0) {                                                                                    \
            scene_set_dynamic_drc_member_pratio(member, temp_member);                                       \
        } else {                                                                                            \
            scene_set_dynamic_drc_member_piso(member, temp_member);                                         \
        }                                                                                                   \
        target = scene_time_filter(target, temp_member, g_scene_pipe_param[index].dynamic_drc.interval, i); \
    } while (0)

#define scene_set_aidrc_member(target, member, temp_member, flag) do {                                        \
        if (flag == 0) {                                                                                    \
            scene_set_dynamic_aidrc_member_pratio(member, temp_member);                                       \
        } else {                                                                                            \
            scene_set_dynamic_aidrc_member_piso(member, temp_member);                                         \
        }                                                                                                   \
        (target) = scene_time_filter((target), temp_member, g_scene_pipe_param[index].dynamic_aidrc.interval, i); \
    } while (0)


#define scene_set_dynamic_drc_return(target, member, flag) do {                       \
        void *temp_member;                                                            \
        temp_member = (typeof(target) *)malloc(sizeof(target));                       \
        if (temp_member == NULL) {                                                    \
            return;                                                                   \
        }                                                                             \
        scene_set_drc_member(target, member, *((typeof(target) *)temp_member), flag); \
        free(temp_member);                                                            \
    } while (0)

#define scene_set_dynamic_aidrc_return(target, member, flag) do {                       \
        void *temp_member;                                                            \
        temp_member = (typeof(target) *)malloc(sizeof(target));                       \
        if (temp_member == NULL) {                                                    \
            return;                                                                   \
        }                                                                             \
        scene_set_aidrc_member(target, member, *((typeof(target) *)temp_member), flag); \
        free(temp_member);                                                            \
    } while (0)

#define scene_set_nrx_attr_interpulate(pack, member) do {                        \
        (pack)->nrx_attr->member = scene_interpulate((pack)->mid, (pack)->left,        \
            (pack)->nrx_param->threednr_value[(pack)->iso_level - 1].member, (pack)->right,  \
            (pack)->nrx_param->threednr_value[(pack)->iso_level].member);                  \
    } while (0)

typedef struct {
    hi_u8 index;
    hi_u32 iso_level;
    hi_u32 iso_count;
    hi_u32 ratio_level;
    hi_u32 ratio_count;
    hi_u32 interval;
    hi_u32 wdr_ratio;
    hi_u32 iso;
} set_isp_attr_param;

typedef enum {
    HI_SCENE_OP_TYPE_AUTO = 0,
    HI_SCENE_OP_TYPE_MANUAL = 1,
    HI_SCENE_OP_TYPE_BUTT
} ot_scene_op_type;

typedef struct {
    hi_u32 fps_max;
} ot_scene_fps_auto;

typedef struct {
    hi_u32 fps;
} ot_scene_fps_manual;

typedef struct {
    hi_bool enable;
    ot_scene_op_type op_type;
    ot_scene_fps_auto auto_s;
    ot_scene_fps_manual manual;
} ot_scene_fps;

typedef struct {
    hi_bool debug;
    hi_bool ae_weight_tab;
    hi_bool static_ae;
    hi_bool static_cac;
    hi_bool static_local_cac;
    hi_bool static_dpc;
    hi_bool static_wdr_exposure;
    hi_bool static_fswdr;
    hi_bool static_awb;
    hi_bool static_awbex;
    hi_bool static_pipe_diff;
    hi_bool static_drc;
    hi_bool static_aidrc;
    hi_bool static_dehaze;
    hi_bool static_ldci;
    hi_bool static_saturation;
    hi_bool static_ccm;
    hi_bool static_dm;
    hi_bool static_nr;
    hi_bool static_shading;
    hi_bool static_csc;
    hi_bool static_bayeshp;
    hi_bool static_sharpen;
    hi_bool static_clut;
    hi_bool static_ca;
    hi_bool static_cross_talk;
    hi_bool static_3dnr;
    hi_bool static_venc;
    hi_bool static_pre_gamma;
    hi_bool static_blc;

    hi_bool dynamic_fps;
    hi_bool dynamic_ae;
    hi_bool dynamic_wdr_exposure;
    hi_bool dynamic_iso_venc;
    hi_bool dynamic_fswdr;
    hi_bool dynamic_drc;
    hi_bool dynamic_linear_drc;
    hi_bool dynamic_aidrc;
    hi_bool dynamic_dehaze;
    hi_bool dynamic_ldci;
    hi_bool dynamic_nr;
    hi_bool dynamic_dpc;
    hi_bool dynamic_blc;
    hi_bool dynamic_linear_ca;
    hi_bool dynamic_ca;
    hi_bool dynamic_shading;
    hi_bool dynamic_clut;
    hi_bool dynamic_gamma;
    hi_bool dynamic_false_color;
    hi_bool dynamic_mcf;
    hi_bool dynamic_3dnr;
    hi_bool dynamic_ai3dnr;
    hi_bool dynamic_aibnr;
    hi_bool dynamic_fpn;
    hi_bool dynamic_venc_mode;
} ot_scene_module_state;

typedef struct {
    hi_bool ae_route_ex_valid;
    hi_u8 ae_run_interval;
    hi_u32 auto_sys_gain_max;
    hi_u8 auto_speed;
    hi_u8 auto_tolerance;
    hi_u16 auto_black_delay_frame;
    hi_u16 auto_white_delay_frame;
    hi_u32 auto_exp_time_max;
    hi_bool ae_auto_iris;
} ot_scene_static_ae;

typedef struct {
    hi_u32 total_num;
    hi_u32 int_time[HI_ISP_AE_ROUTE_MAX_NODES];
    hi_u32 sys_gain[HI_ISP_AE_ROUTE_MAX_NODES];
} ot_scene_static_ae_route;

typedef struct {
    hi_u32 total_num;
    hi_u32 int_time[HI_ISP_AE_ROUTE_MAX_NODES];
    hi_u32 again[HI_ISP_AE_ROUTE_MAX_NODES];
    hi_u32 dgain[HI_ISP_AE_ROUTE_MAX_NODES];
    hi_u32 isp_dgain[HI_ISP_AE_ROUTE_MAX_NODES];
    hi_u32 iris_fno[HI_ISP_AE_ROUTE_MAX_NODES];
} ot_scene_static_ae_route_ex;

typedef struct {
    hi_u8  op_type;
    hi_u16 manual_rgain;
    hi_u16 manual_gbgain;
    hi_u16 manual_grgain;
    hi_u16 manual_bgain;
    hi_u16 auto_static_wb[HI_ISP_BAYER_CHN_NUM];
    hi_s32 auto_curve_para[HI_ISP_AWB_CURVE_PARA_NUM];
    hi_u16 auto_speed;
    hi_u16 auto_low_color_temp;
    hi_u16 auto_cr_max[HI_ISP_AUTO_ISO_NUM];
    hi_u16 auto_cr_min[HI_ISP_AUTO_ISO_NUM];
    hi_u16 auto_cb_max[HI_ISP_AUTO_ISO_NUM];
    hi_u16 auto_cb_min[HI_ISP_AUTO_ISO_NUM];
    hi_u16 luma_hist_enable;
    hi_u16 awb_switch;
    hi_u16 black_level;
} ot_scene_static_awb;

typedef struct {
    hi_bool bypass;
    hi_u8 tolerance;
    hi_u8 out_shift_limit;
    hi_u32 out_thresh;
    hi_u16 low_stop;
    hi_u16 high_start;
    hi_u16 high_stop;
    hi_bool multi_light_source_en;
    hi_u16 multi_ctwt[HI_ISP_AWB_MULTI_CT_NUM];
} ot_scene_static_awb_ex;

typedef struct {
    hi_u32 gain[HI_ISP_BAYER_CHN_NUM];
} ot_scene_isp_pipe_diff;

typedef struct {
    hi_bool enable;                     /* RW; range: [0, 1]; format:1.0; cac enable */
    hi_u8   op_type;                 /* RW; range: [0, 1]; format:1.0; purple fringing correction working mode */
    hi_u8   detect_mode;                /* RW; range: [0, 2]; format:2.0; cac edge detect mode */
    hi_s16  purple_upper_limit;         /* RW; range: [-2048,2047]; format:s12.0; cac purple high limitation */

    hi_s16  purple_lower_limit;         /* RW; range: [-2048,2047]; format:s12.0; cac purple low limitation */
    hi_u16  edge_threshold[HI_SCENE_ACAC_THR_NUM][HI_ISP_AUTO_ISO_NUM];
    hi_u16  edge_gain[HI_ISP_AUTO_ISO_NUM];
    hi_u16  cac_rb_strength[HI_ISP_AUTO_ISO_NUM];        /* RW; range: [0, 31]; format:5.0; cac strength on rb */
    hi_u16  purple_alpha[HI_ISP_AUTO_ISO_NUM];           /* RW; range: [0, 63]; format:6.0; cac purple alpha */
    hi_u16  edge_alpha[HI_ISP_AUTO_ISO_NUM];             /* RW; range: [0, 63]; format:6.0; cac edge alpha */
    hi_u16  satu_low_threshold[HI_ISP_AUTO_ISO_NUM];     /* RW; range: [0, 4095]; format:12.0;
                                                         cac purple saturation low threshold */
    hi_u16  satu_high_threshold[HI_ISP_AUTO_ISO_NUM];    /* RW; range: [0, 16383]; format:14.0;
                                                         cac purple saturation high threshold */

    hi_u8   de_purple_cr_strength[OT_ISP_LCAC_EXP_RATIO_NUM]; /* RW; range: [0, 8]; format:4.0;
                                                         correction strength of the R channel */
    hi_u8   de_purple_cb_strength[OT_ISP_LCAC_EXP_RATIO_NUM]; /* RW; range: [0, 8]; format:4.0; */
    hi_u16  purple_detect_range;        /* RW; range: [0, 410]; format:9.0; purple fringing detection scope */
    hi_u16  var_threshold;              /* RW; range: [0, 4095]; format:12.0; edge detection threshold */

    hi_u16 r_detect_threshold[HI_ISP_LCAC_DET_NUM]; /* RW;Range:[0,4095];Format:12.0;Component R detection threshold */
    hi_u16 g_detect_threshold[HI_ISP_LCAC_DET_NUM]; /* RW;Range:[0,4095];Format:12.0;Component G detection threshold */
    hi_u16 b_detect_threshold[HI_ISP_LCAC_DET_NUM]; /* RW;Range:[0,4095];Format:12.0;Component B detection threshold */
} ot_scene_static_cac;

typedef struct {
    hi_bool enable;
    hi_op_mode op_type[HI_ISP_WDR_MAX_FRAME_NUM];
    hi_u16 strength[HI_ISP_WDR_MAX_FRAME_NUM][HI_SCENE_ISO_STRENGTH_NUM];
    hi_u16 blend_ratio[HI_ISP_WDR_MAX_FRAME_NUM][HI_SCENE_ISO_STRENGTH_NUM];
} ot_scene_static_dpc;

typedef struct {
    hi_u8 exp_ratio_type;
    hi_u32 exp_ratio_max;
    hi_u32 exp_ratio_min;
    hi_u32 exp_ratio[HI_ISP_EXP_RATIO_NUM];
    hi_u16 tolerance;
    hi_u32 ref_ratio_up;
    hi_u32 ref_ratio_dn;
    hi_u32 exp_thr;
    hi_u32 high_light_target;
    hi_u32 exp_coef_min;
} ot_scene_static_wdr_exposure;

typedef struct {
    hi_u32 iso_thresh_cnt;
    hi_u32 iso_thresh_ltoh[HI_SCENE_SHADING_EXPOSURE_MAX_COUNT];
    hi_u16 manual_percent[HI_SCENE_SHADING_EXPOSURE_MAX_COUNT];
} ot_scene_dynamic_venc_bitrate;

typedef struct {
    hi_u8 ae_exposure_cnt;
    hi_u64 exp_ltoh_thresh[HI_SCENE_AE_EXPOSURE_MAX_COUNT];
    hi_u64 exp_htol_thresh[HI_SCENE_AE_EXPOSURE_MAX_COUNT];
    hi_u32 exp_compensation[HI_SCENE_AE_EXPOSURE_MAX_COUNT];
    hi_u32 max_hist_offset[HI_SCENE_AE_EXPOSURE_MAX_COUNT];
} ot_scene_dynamic_wdr_exposure;

typedef struct {
    hi_isp_wdr_merge_mode wdr_merge_mode;
    hi_bool fusion_blend_en;
} ot_scene_static_fswdr;

typedef struct {
    hi_bool enable;
    hi_u8 ldci_op_type;
    hi_u8 gauss_lpf_sigma;
    hi_u16 tpr_incr_coef;
    hi_u16 tpr_decr_coef;
    hi_u8 auto_he_pos_wgt[HI_ISP_AUTO_ISO_NUM];
    hi_u8 auto_he_pos_sigma[HI_ISP_AUTO_ISO_NUM];
    hi_u8 auto_he_pos_mean[HI_ISP_AUTO_ISO_NUM];
    hi_u8 auto_he_neg_wgt[HI_ISP_AUTO_ISO_NUM];
    hi_u8 auto_he_neg_sigma[HI_ISP_AUTO_ISO_NUM];
    hi_u8 auto_he_neg_mean[HI_ISP_AUTO_ISO_NUM];
    hi_u8 auto_blc_ctrl[HI_ISP_AUTO_ISO_NUM];
} ot_scene_static_ldci;

typedef struct {
    hi_bool enable;
    hi_u8 dehaze_op_type;
    hi_bool user_lut_enable;
    hi_u16 tmprflt_incr_coef;
    hi_u16 tmprflt_decr_coef;
    hi_u8 dehaze_lut[HI_ISP_DEHAZE_LUT_SIZE];
} ot_scene_static_dehaze;

typedef struct {
    hi_bool enable;
    hi_op_mode op_type;
    hi_isp_drc_curve_select curve_select; /* RW; Range:[0x0, 0x1]; Tone mapping curve selection */
    hi_u8 purple_reduction_strength;      /* RW; Range:[0x0, 0x80]; Purple detection and reduction strength */
    hi_u8 contrast_ctrl;  /* RW; Range:[0x0, 0xF]; Format:4.0; Contrast control, smaller value yields better contrast */
    hi_u8 blend_luma_max; /* RW; Range:[0x0, 0xFF]; Luma-based filter blending weight control */
    hi_u8 blend_luma_bright_min;         /* RW; Range:[0x0, 0xFF]; Luma-based filter blending weight control */
    hi_u8 blend_luma_bright_threshold;   /* RW; Range:[0x0, 0xFF]; Luma-based filter blending weight control */

    hi_u8 blend_luma_dark_min;           /* RW; Range:[0x0, 0xFF]; Luma-based filter blending weight control */
    hi_u8 blend_luma_dark_threshold;     /* RW; Range:[0x0, 0xFF]; Luma-based filter blending weight control */

    hi_u8 blend_detail_max;              /* RW; Range:[0x0, 0xFF]; Detail-based filter blending weight control */
    hi_u8 blend_detail_bright_min;       /* RW; Range:[0x0, 0xFF]; Detail-based filter blending weight control */
    hi_u8 blend_detail_bright_threshold; /* RW; Range:[0x0, 0xFF]; Detail-based filter blending weight control */

    hi_u8 blend_detail_dark_min;         /* RW; Range:[0x0, 0xFF]; Detail-based filter blending weight control */
    hi_u8 blend_detail_dark_threshold;   /* RW; Range:[0x0, 0xFF]; Detail-based filter blending weight control */

    hi_u8  global_color_ctrl;
    hi_u16 color_correction_lut[HI_ISP_DRC_CC_NODE_NUM];
} ot_scene_static_drc;

typedef struct {
    td_u8 tone_mapping_wgt_x; /* RW; Range:[0x0, 0x80]; Local TM gain scaling coef */
    td_u8 detail_adjust_coef_x; /* RW; Range:[0x0, 0xF]; X filter detail adjust coefficient */
    td_u8 blend_luma_max; /* RW; Range:[0x0, 0xFF]; Luma-based filter blending weight control */
    td_u8 blend_luma_bright_min; /* RW; Range:[0x0, 0xFF]; Luma-based filter blending weight control */
    td_u8 blend_luma_bright_threshold; /* RW; Range:[0x0, 0xFF]; Luma-based filter blending weight control */
    td_u8 blend_luma_dark_min; /* RW; Range:[0x0, 0xFF]; Luma-based filter blending weight control */
    td_u8 blend_luma_dark_threshold; /* RW; Range:[0x0, 0xFF]; Luma-based filter blending weight control */
    td_u8 blend_detail_max; /* RW; Range:[0x0, 0xFF]; Detail-based filter blending weight control */
    td_u8 blend_detail_bright_min; /* RW; Range:[0x0, 0xFF]; Detail-based filter blending weight control */
    td_u8 blend_detail_bright_threshold; /* RW; Range:[0x0, 0xF]; Detail-based filter blending weight control */
    td_u8 blend_detail_dark_min; /* RW; Range:[0x0, 0xFF]; Detail-based filter blending weight control */
    td_u8 blend_detail_dark_threshold; /* RW; Range:[0x0, 0xF]; Detail-based filter blending weight control */
    td_u8 detail_adjust_coef_blend; /* RW; Range:[0x0, 0xF]; Extra detail gain of X filter details */
} ot_scene_static_aidrc;

typedef struct {
    hi_u8 fps_exposure_cnt;
    hi_u64 exp_ltoh_thresh[HI_SCENE_FPS_ISO_MAX_COUNT];
    hi_u64 exp_htol_thresh[HI_SCENE_FPS_ISO_MAX_COUNT];
    hi_u32 fps[HI_SCENE_FPS_ISO_MAX_COUNT];
    hi_u32 venc_gop[HI_SCENE_FPS_ISO_MAX_COUNT];
    hi_u32 ae_max_time[HI_SCENE_FPS_ISO_MAX_COUNT];
} ot_scene_dynamic_fps;

typedef struct {
    hi_u8 ae_exposure_cnt;
    hi_u32 auto_exp_ratio_max[HI_SCENE_AE_EXPOSURE_MAX_COUNT];
    hi_u32 auto_exp_ratio_min[HI_SCENE_AE_EXPOSURE_MAX_COUNT];
    hi_u64 exp_ltoh_thresh[HI_SCENE_AE_EXPOSURE_MAX_COUNT];
    hi_u64 exp_htol_thresh[HI_SCENE_AE_EXPOSURE_MAX_COUNT];
    hi_u8 auto_compensation[HI_SCENE_AE_EXPOSURE_MAX_COUNT];
    hi_u8 auto_max_hist_offset[HI_SCENE_AE_EXPOSURE_MAX_COUNT];
    hi_u32  wdr_ratio_threshold;
    hi_bool l_advance_ae;
    hi_bool h_advance_ae;
} ot_scene_dynamic_ae;

typedef struct {
    hi_u32 iso_count;
    hi_u32 iso_level[HI_SCENE_DRC_ISO_MAX_COUNT];
    hi_bool enable;

    hi_u8 local_mixing_bright[HI_ISP_DRC_LMIX_NODE_NUM][HI_SCENE_DRC_ISO_MAX_COUNT];
    hi_u8 local_mixing_dark[HI_ISP_DRC_LMIX_NODE_NUM][HI_SCENE_DRC_ISO_MAX_COUNT];
    hi_u8 spatial_filter_coef[HI_SCENE_DRC_ISO_MAX_COUNT];
    hi_u8 range_filter_coef[HI_SCENE_DRC_ISO_MAX_COUNT];
    hi_u8 detail_adjust_coef[HI_SCENE_DRC_ISO_MAX_COUNT];
    hi_u8 rim_reduction_strength[HI_SCENE_DRC_ISO_MAX_COUNT];
    hi_u8 rim_reduction_threshold[HI_SCENE_DRC_ISO_MAX_COUNT];
    hi_u8 bright_gain_limit[HI_SCENE_DRC_ISO_MAX_COUNT];
    hi_u8 bright_gain_limit_step[HI_SCENE_DRC_ISO_MAX_COUNT];
    hi_u8 dark_gain_limit_luma[HI_SCENE_DRC_ISO_MAX_COUNT];
    hi_u8 dark_gain_limit_chroma[HI_SCENE_DRC_ISO_MAX_COUNT];

    hi_u8 high_saturation_color_ctrl[HI_SCENE_DRC_ISO_MAX_COUNT];
    hi_u8 asymmetry[HI_SCENE_DRC_ISO_MAX_COUNT];
    hi_u8 second_pole[HI_SCENE_DRC_ISO_MAX_COUNT];
    hi_u8 compress[HI_SCENE_DRC_ISO_MAX_COUNT];
    hi_u8 stretch[HI_SCENE_DRC_ISO_MAX_COUNT];
    hi_u16 strength[HI_SCENE_DRC_ISO_MAX_COUNT];
} ot_scene_dynamic_linear_drc;

typedef struct {
    hi_bool enable;
    hi_u32 ratio_count;
    hi_u32 ratio_level[HI_SCENE_DRC_RATIO_MAX_COUNT];
    hi_u32 iso_count;
    hi_u32 iso_level[HI_SCENE_DRC_ISO_MAX_COUNT];
    hi_u32 interval;

    hi_u8 high_saturation_color_ctrl[HI_SCENE_DRC_RATIO_MAX_COUNT];
    hi_u8 bright_gain_limit[HI_SCENE_DRC_RATIO_MAX_COUNT];
    hi_u8 bright_gain_limit_step[HI_SCENE_DRC_RATIO_MAX_COUNT];
    hi_u16 drc_str_incre[HI_SCENE_DRC_RATIO_MAX_COUNT];

    hi_u8 local_mixing_bright[HI_ISP_DRC_LMIX_NODE_NUM][HI_SCENE_DRC_ISO_MAX_COUNT];
    hi_u8 local_mixing_dark[HI_ISP_DRC_LMIX_NODE_NUM][HI_SCENE_DRC_ISO_MAX_COUNT];
    hi_u16 color_correction_lut[HI_ISP_DRC_CC_NODE_NUM][HI_SCENE_DRC_RATIO_MAX_COUNT];

    hi_u8 tone_mapping_wgt_x[HI_SCENE_DRC_ISO_MAX_COUNT];

    hi_u8 spatial_filter_coef[HI_SCENE_DRC_ISO_MAX_COUNT];
    hi_u8 range_filter_coef[HI_SCENE_DRC_ISO_MAX_COUNT];
    hi_u8 detail_adjust_coef[HI_SCENE_DRC_ISO_MAX_COUNT];
    hi_u8 rim_reduction_strength[HI_SCENE_DRC_ISO_MAX_COUNT];
    hi_u8 rim_reduction_threshold[HI_SCENE_DRC_ISO_MAX_COUNT];
    hi_u8 dark_gain_limit_luma[HI_SCENE_DRC_ISO_MAX_COUNT];
    hi_u8 dark_gain_limit_chroma[HI_SCENE_DRC_ISO_MAX_COUNT];
    hi_u8 global_color_ctrl[HI_SCENE_DRC_RATIO_MAX_COUNT]; /* global color control  for CSP curve */
    hi_bool shoot_reduction_en[HI_SCENE_DRC_RATIO_MAX_COUNT];

    hi_u16 manual_str[HI_SCENE_DRC_ISO_MAX_COUNT];
    hi_u32 ref_ratio_count;
    hi_u32 ref_ratio_ltoh[HI_SCENE_DRC_REF_RATIO_MAX_COUNT];
    hi_u32 ref_ratio_alpha[HI_SCENE_DRC_REF_RATIO_MAX_COUNT];
    hi_u32 tm_ratio_threshold;
    hi_u16 tm_value_low[HI_ISP_DRC_TM_NODE_NUM];
    hi_u16 tm_value_high[HI_ISP_DRC_TM_NODE_NUM];
    hi_u16 tm_val_higher[HI_ISP_DRC_TM_NODE_NUM];
    hi_u16 tm_val_highest[HI_ISP_DRC_TM_NODE_NUM];
} ot_scene_dynamic_drc;

typedef struct {
    hi_bool enable;
    hi_u32 ratio_count;
    hi_u32 ratio_level[HI_SCENE_DRC_RATIO_MAX_COUNT];
    hi_u32 iso_count;
    hi_u32 iso_level[HI_SCENE_DRC_ISO_MAX_COUNT];
    hi_u32 interval;

    hi_u8 blend_luma_max[HI_SCENE_DRC_RATIO_MAX_COUNT];
    hi_u8 detail_adjust_coef_x[HI_SCENE_DRC_RATIO_MAX_COUNT];
    hi_u8 detail_adjust_coef_blend[HI_SCENE_DRC_RATIO_MAX_COUNT];

    hi_u32 aidrc_strength[HI_SCENE_DRC_RATIO_MAX_COUNT];
    hi_u32 aidrc_threshold[HI_SCENE_DRC_RATIO_MAX_COUNT];

    hi_u8 local_mixing_bright_x[HI_ISP_DRC_LMIX_NODE_NUM][HI_SCENE_DRC_ISO_MAX_COUNT];
    hi_u8 local_mixing_dark_x[HI_ISP_DRC_LMIX_NODE_NUM][HI_SCENE_DRC_ISO_MAX_COUNT];
} ot_scene_dynamic_aidrc;

typedef struct {
    hi_u32 ref_exp_ratio;
    hi_u32 ref_ratio_alpha;
    hi_u32 k;
} ot_scene_dynamic_drc_coef;

typedef struct {
    hi_u32 enable_cnt;
    hi_u8 enable[HI_SCENE_LDCI_EXPOSURE_MAX_COUNT];
    hi_u64 enable_exp_thresh_ltoh[HI_SCENE_LDCI_EXPOSURE_MAX_COUNT];
    hi_u32 exp_thresh_cnt;
    hi_u8 manual_ldci_he_pos_wgt[HI_SCENE_LDCI_EXPOSURE_MAX_COUNT];
    hi_u64 exp_thresh_ltoh[HI_SCENE_LDCI_EXPOSURE_MAX_COUNT];
} ot_scene_dynamic_ldci;

#define SDK_RELY
#define S_IDX_LEN   17

typedef struct {
    /* ies0~3 for different frequency response. */
    hi_u32 ies0;     /* RW; Range: [0, 255]; The gains of edge and texture enhancement. */
    hi_u32 ies1;     /* RW; Range: [0, 255]; The gains of edge and texture enhancement. */
    hi_u32 ies2;     /* RW; Range: [0, 255]; The gains of edge and texture enhancement. */
    hi_u32 ies3;     /* RW; Range: [0, 255]; The gains of edge and texture enhancement. */
    hi_u32 iedz;    /* RW; Range: [0, 999]; The threshold to control the generated artifacts. */
    hi_u32 o_sht;    /* RW; Range: [0, 255]; The gains to control the overshoot. */
    hi_u32 u_sht;    /* RW; Range: [0, 255]; The gains to control the undershoot. */
} t_v200_vpss_iey;

typedef struct {
    hi_u32 spn;  /* RW; Range: [0,   6]; The selection of filters to be mixed for NO.7 filter. */
    hi_u32 sbn;  /* RW; Range: [0,   6]; The selection of filters to be mixed for NO.7 filter. */
    hi_u32 pbr;  /* RW; Range: [0,  16]; The mix ratio between spn and sbr. */
    hi_u32 j_mode;  /* RW; Range: [0,   2]; The selection modes for the blending of spatial filters. */
    hi_u32 _rb_;  /* RW; Reserved */

    hi_u32 sfr6[4];     /* RW; Range: [0, 31]; The relative NR strength for NO.6 filter. (Effective when jmode = 2). */
    hi_u32 sbr6[2];     /* RW; Range: [0, 15]; The control of overshoot and undershoot. */

    hi_u32 sfs1;        /* RW; Range: [0, 255]; The NR strength parameters for NO.1 filter. */
    hi_u32 sbr1;        /* RW; Range: [0, 255]; The NR strength parameters for NO.1 filter. */
    hi_u32 sfs2;        /* RW; Range: [0, 255]; The NR strength parameters for NO.2 filter. */
    hi_u32 sft2;        /* RW; Range: [0, 255]; The NR strength parameters for NO.2 filter. */
    hi_u32 sbr2;        /* RW; Range: [0, 255]; The NR strength parameters for NO.2 filter. */
    hi_u32 sfs4;        /* RW; Range: [0, 255]; The NR strength parameters for NO.3 and NO.4 filters. */
    hi_u32 sft4;        /* RW; Range: [0, 255]; The NR strength parameters for NO.3 and NO.4 filters. */
    hi_u32 sbr4;        /* RW; Range: [0, 255]; The NR strength parameters for NO.3 and NO.4 filters. */
    /*
    * sth1_0, sth2_0, sth3_0; Range: [0, 511]; The thresholds for protection of edges on foreground from blurring.
    * sth1_1, sth2_1, sth3_1; Range: [0, 511]; The thresholds for protection of edges on background from blurring.
    * bri_idx0, bri_idx1; Range: [0, 7]; The filter selection based on brightness
    * for foreground and background, respectively.
    */
    hi_u32 sth1_0;
    hi_u32 sth2_0;
    hi_u32 sth3_0;
    hi_u32 sth1_1;
    hi_u32 sth2_1;
    hi_u32 sth3_1;

    /*
    * sfn0_0~sfn3_0; Range: [0, 6]; Filter selection for different foreground image areas based on sth1_0~sth3_0.
    * sfn0_1~sfn3_1; Range: [0, 6]; Filter selection for different background image areas based on sth1_1~sth3_1.
    */

    hi_u32 sfn0_0;
    hi_u32 sfn1_0;
    hi_u32 sfn2_0;
    hi_u32 sfn3_0;

    hi_u32 sfn0_1;
    hi_u32 sfn1_1;
    hi_u32 sfn2_1;
    hi_u32 sfn3_1;

    hi_u32 bri_str[OT_NR_S_IDX_LEN]; /* RW ; Range: [0, 255];  Spatial NR strength based on brightness. */

    hi_u32 bri_idx0;     /* RW; Range: [0, 6]; The filter selection based on brightness */
    hi_u32 bri_idx1;     /* RW; Range: [0, 6]; The filter selection based on brightness */
    hi_u32 reserved1;
    hi_u32 k_mode;
    hi_u32 sbs_k[33];
    hi_u32 sds_k[33];
    hi_u32 reserved2;
} t_v200_vpss_sfy;

typedef struct {
    /* ies0~3 for different frequency response. */
    hi_u32 ies0;     /* RW; Range: [0, 255]; The gains of edge and texture enhancement. */
    hi_u32 ies1;     /* RW; Range: [0, 255]; The gains of edge and texture enhancement. */
    hi_u32 ies2;     /* RW; Range: [0, 255]; The gains of edge and texture enhancement. */
    hi_u32 ies3;     /* RW; Range: [0, 255]; The gains of edge and texture enhancement. */
    hi_u32 aux_ies;  /* RW; Range: [0, 255]; The gains of edge enhancement for NO.2 filter. */

    hi_u32 iedz;    /* RW; Range: [0, 999]; The threshold to control the generated artifacts. */
    hi_u32 _rb_;

    hi_u32 o_sht;       /* RW; Range: [0, 255]; The gains to control the overshoot. */
    hi_u32 u_sht;       /* RW; Range: [0, 255]; The gains to control the undershoot. */
    hi_u32 aux_o_sht;   /* RW; Range: [0, 255]; The gains to control the overshoot. */
    hi_u32 aux_u_sht;   /* RW; Range: [0, 255]; The gains to control the undershoot. */
} t_v200_vpss_adv_iey;

typedef struct {
    hi_u32 spn6;  /* RW; Range: [0,   5]; The selection of filters to be mixed for NO.6 filter. */
    hi_u32 sbn6;  /* RW; Range: [0,   5]; The selection of filters to be mixed for NO.6 filter. */
    hi_u32 pbr6;  /* RW; Range: [0,  16]; The mix ratio between spn and sbr. */
    hi_u32 j_mode6;  /* RW; Range: [0,   2]; The selection modes for the blending of spatial filters. */
    hi_u32 _rb_;  /* RW; Reserved */

    hi_u32 sfr6[4];     /* RW; Range: [0, 31]; The relative NR strength for NO.6 filter. (Effective when jmode = 2). */
    hi_u32 sbr6[2];     /* RW; Range: [0, 15]; The control of overshoot and undershoot. */
    hi_u32 sfs1;        /* RW; Range: [0, 15]; The NR strength parameters for NO.1 filter. */
    hi_u32 sfs2;        /* RW; Range: [0, 255]; Currently no use, reserved. */
    hi_u32 bld1;       /* RW; Range: [0, 999];  The NR strength parameters for NO.1 filter. */

    hi_u32 spn3;  /* RW; Range: [0,   5]; The selection of filters to be mixed for NO.3 filter. */
    hi_u32 sbn3;  /* RW; Range: [0,   5]; The selection of filters to be mixed for NO.3 filter. */
    hi_u32 pbr3;  /* RW; Range: [0,  16]; The mix ratio between spn and sbr. */
    hi_u32 _rb0_;

    hi_u32 spn4;  /* RW; Range: [0,   5]; The selection of filters to be mixed for NO.4 filter. */
    hi_u32 sbn4;  /* RW; Range: [0,   5]; The selection of filters to be mixed for NO.4 filter. */
    hi_u32 pbr4;  /* RW; Range: [0,  16]; The mix ratio between spn and sbr. */
    hi_u32 _rb1_;

    hi_u32 sfs5;           /* RW; Range: [0, 255]; The NR strength parameters for NO.5 filters. */
    hi_u32 sft5;           /* RW; Range: [0, 255]; The NR strength parameters for NO.5 filters. */
    hi_u32 sbr5;           /* RW; Range: [0, 255]; The NR strength parameters for NO.5 filters. */
    hi_u32 reserved2;

    hi_u32  sth1;        /* RW; Range: [0, 511]; The thresholds for protection of edges from blurring. */
    hi_u32  sth2;        /* RW; Range: [0, 511]; The thresholds for protection of edges from blurring. */
    hi_u32  sth3;        /* RW; Range: [0, 511]; The thresholds for protection of edges from blurring. */

    hi_u32 sfn0; /* RW; Range: [0, 6]; Filter selection for different foreground image areas based on sth1. */
    hi_u32 sfn1; /* RW; Range: [0, 6]; Filter selection for different foreground image areas based on sth1. */
    hi_u32 sfn2; /* RW; Range: [0, 6]; Filter selection for different foreground image areas based on sth2. */
    hi_u32 sfn3; /* RW; Range: [0, 6]; Filter selection for different foreground image areas based on sth3. */

    hi_u32   bri_str[OT_NR_S_IDX_LEN];   /* RW; Range: [0, 255];  Spatial NR strength based on brightness. */
    hi_u32 bri_idx;                       /* RW; Range: [0, 6]; The filter selection based on brightness */

    hi_u32 k_mode;
    hi_u32 sbs_k[33];
    hi_u32 sds_k[33];
    hi_u32 reserved3;
} t_v200_vpss_adv_sfy;

typedef struct {
    hi_u32 tfs0;   /* RW; Range: [0,  15]; The NR strength for temporal filtering. */
    hi_u32 tdz0;  /* RW; Range: [0, 999]; Protection of the weak texture area from temporal filtering. */
    hi_u32 ref_en;   /* RW; Range: [0,  1];  The switch the reference frame. */
    hi_u32 _rb0_;

    hi_u32 tfs1;   /* RW; Range: [0,  15]; The NR strength for temporal filtering. */
    hi_u32 tdz1;  /* RW; Range: [0, 999]; Protection of the weak texture area from temporal filtering. */
    hi_u32 math_mode;   /* RW; Range: [0,   1]; The motion detection mode. */
    hi_u32 _rb1_;

    hi_u32   tss0;       /* RW; Range: [0,  15]; The ratio for blending spatial NR with the temporal NR results. */
    hi_u32   tss1;       /* RW; Range: [0,  15]; The ratio for blending spatial NR with the temporal NR results. */

    hi_u32 auto_math;   /* RW; Range: [0, 999]; The motion threshold for the Level 0 denoise. */
    hi_u32 tfr0[6];      /* RW; Range: [0,  31]; The temporal NR strength control for background (static) area. */
    hi_u32 tfr1[6];      /* RW; Range: [0,  31]; The temporal NR strength control for background (static) area. */
} t_v200_vpss_tfy;

typedef struct {
    hi_u32 math0;  /* RW; Range: [0, 999]; The threshold for motion detection. */
    hi_u32 mate0;   /* RW; Range: [0,   8]; The motion index for smooth image area. */
    hi_u32 adv_math; /* RW; Range: [0,   2]; The mode selection for motion detection. */
    hi_u32 math1;  /* RW; Range: [0, 999]; The threshold for motion detection. */
    hi_u32 mate1;   /* RW; Range: [0,   8]; The motion index for smooth image area. */
    hi_u32 _rb1_;
    hi_u32   mabw0;          /* RW; Range: [5,   9]; The window size for motion detection. */
    hi_u32   mabw1;          /* RW; Range: [5,   9]; The window size for motion detection. */

    hi_u32 artc;           /* RW; Range: [0, 255]; The artifact control for motion */
    hi_u32 mcth;           /* RW; Range: [0, 999]; The sensitivity for motion */
    hi_u32 dci_w;          /* RW; Range: [0, 999]; The temporal gain for dci gain */
} t_v200_vpss_mdy;

typedef struct {
    hi_u32 sfc_enhance;     /* RW; Range: [0, 255];  The chroma NR strength for sfc. */
    hi_u32 sfc_ext;         /* RW; Range: [0, 255];  The extra chroma NR strength for sfc. */
    hi_u32 trc;             /* RW; Range: [0, 255];  The temporal chroma NR strength. */
    hi_u32 reserved;
    hi_u32 sfc;   /* RW; Range: [0, 255];  The motion threshold for chroma. */
    hi_u32 tfc;   /* RW; Range: [0, 31];   The control of temporal chroma NR strength. */
    hi_u32 _rb0_;

    hi_u32 tpc;              /* RW; Range: [0, 31];  Chroma denoise mode. */
    hi_u32 pre_sfc;          /* RW; Range: [0, 31];  pre-chroma denoise. */
} t_v200_vpss_nrc0;

typedef struct {
    hi_u32 sfs2;      /* RW; Range: [0, 255];  The NR strength parameters for NO.2 filter. */
    hi_u32 sft2;      /* RW; Range: [0, 255];  The NR strength parameters for NO.2 filter. */
    hi_u32 sbr2;      /* RW; Range: [0, 255];  The NR strength parameters for NO.2 filter. */
    hi_u32 sfc3;      /* RW; Range: [0, 255];  The chroma NR strength for NO.3 filter. */
    hi_u32 sfc_ext3;  /* RW; Range: [0, 255];  The extra chroma NR strength for NO.3 filter. */
    /* sfr3_u, sfr3_v; Range: [0, 15]; The chroma NR strength control for U and V, respectively, for NO.3 filter. */

    hi_u32 sfr3_u;
    hi_u32 sfr3_v;

    hi_u32 spn0;    /* RW; Range: [0,  3];  The selection of filters to be mixed for NO.X filter. */
    hi_u32 sbn0;    /* RW; Range: [0,  3];  The selection of filters to be mixed for NO.X filter. */
    hi_u32 pbr0;    /* RW; Range: [0, 15];  The mix ratio for NO.7 filter. */
    hi_u32 spn1;    /* RW; Range: [0,  3];  The selection of filters to be mixed for NO.X filter. */
    hi_u32 sbn1;    /* RW; Range: [0,  3];  The selection of filters to be mixed for NO.X filter. */
    hi_u32 pbr1;    /* RW; Range: [0,  15]; The mix ratio for NO.X filter. */
} t_v200_vpss_nrc1;

typedef struct {
    t_v200_vpss_adv_iey adv_ie_y[2];
    t_v200_vpss_adv_sfy adv_sf_y[2];

    t_v200_vpss_iey  iey[5];
    t_v200_vpss_sfy  sfy[5];
    t_v200_vpss_mdy  mdy[2];
    t_v200_vpss_tfy  tfy[3];
    t_v200_vpss_nrc0 nrc0;
    t_v200_vpss_nrc1 nrc1;

    hi_u32 limit_range_en;  /* RW; Range: [0, 1]; The switch for limit range mode. */
    hi_u32 nry0_en;  /* RW; Range: [0, 1]; The switch for luma denoise. */
    hi_u32 nry1_en;  /* RW; Range: [0, 1]; The switch for luma denoise. */
    hi_u32 nry2_en;  /* RW; Range: [0, 1]; The switch for luma denoise. */
    hi_u32 nry3_en;  /* RW; Range: [0, 1]; The switch for luma denoise. */
    hi_u32 nrc0_en;  /* RW; Range: [0, 1]; The switch for chroma denoise. */
    hi_u32 nrc1_en;  /* RW; Range: [0, 1]; The switch for chroma denoise. */
    hi_u32 _rb_;

    hi_u32 adv_mode;                /* RW; Range: [0, 3]; */
} ot_scene_3dnr;


typedef struct {
    hi_u32 threed_nr_count;
    hi_u32 threed_nr_iso[HI_SCENE_3DNR_MAX_COUNT];
    ot_scene_3dnr threednr_value[HI_SCENE_3DNR_MAX_COUNT];
} ot_scene_static_3dnr;

typedef struct {
    hi_u8 ae_weight[HI_ISP_AE_ZONE_ROW][HI_ISP_AE_ZONE_COLUMN];
} ot_scene_static_statisticscfg;

typedef struct {
    hi_u8 auto_sat[HI_ISP_AUTO_ISO_NUM];
} ot_scene_static_saturation;

typedef struct {
    hi_u8 ccm_op_type;
    hi_u16 manual_ccm[HI_ISP_CCM_MATRIX_SIZE];
    hi_bool auto_iso_act_en;
    hi_bool auto_temp_act_en;
    hi_u32 total_num;
    hi_u16 auto_color_temp[HI_ISP_CCM_MATRIX_NUM];
    hi_u16 auto_ccm[HI_ISP_CCM_MATRIX_NUM][HI_ISP_CCM_MATRIX_SIZE];
} ot_scene_static_ccm;

typedef struct {
    hi_bool enable;
    hi_bool limited_range_en;
    hi_u8 hue;
    hi_u8 luma;
    hi_u8 contrast;
    hi_u8 saturation;
    hi_color_gamut color_gamut;
} ot_scene_static_csc;

typedef struct {
    hi_u8 nddm_strength[HI_ISP_AUTO_ISO_NUM];           /* RW; Range:[0x0, 0xFF]; Format:4.4; Non-direction strength */
    hi_u8 nddm_mf_detail_strength[HI_ISP_AUTO_ISO_NUM]; /* RW; Range:SD3403V100 = [0x0, 0x7f];
                                                           Format:3.4; Non-direction medium frequent detail
                                                           enhance strength */
    hi_u8 nddm_hf_detail_strength[HI_ISP_AUTO_ISO_NUM]; /* RW; Range:[0x0, 0x10]; Format:2.2; Non-direction
                                                           high frequent detail enhance strength */
    hi_u8 detail_smooth_range[HI_ISP_AUTO_ISO_NUM];     /* RW; Range:SD3403V100 = [0x1, 0x7];
                                                           Format:4.0; Detail smooth range */
    hi_u8 color_noise_f_threshold[HI_ISP_AUTO_ISO_NUM]; /* RW;Range:[0x0, 0xff]; Format:8.0;
                                                           Threshold of color noise cancel */
    hi_u8 color_noise_f_strength[HI_ISP_AUTO_ISO_NUM];  /* RW;Range:[0x0, 0x8]; Format:4.0; Strength of color
                                                           noise cancel */
    hi_u8 color_noise_y_threshold[HI_ISP_AUTO_ISO_NUM]; /* RW;Range:SD3403V100 = [0x0, 0xFF]; Format:8.0;Range of
	                                                       color denoise luma, related to luminance and saturation */
    hi_u8 color_noise_y_strength[HI_ISP_AUTO_ISO_NUM];  /* RW;Range:SD3403V100 = [0x0, 0xFF]; Format:8.0;Format:8.0;
                                                           Strength of color denoise luma */
} ot_scene_static_dm_auto;

typedef struct {
    hi_bool enable; /* RW; Range:[0, 1]; Format:1.0;Enable/Disable demosaic module */
    hi_op_mode op_type;
    ot_scene_static_dm_auto dm_auto_cfg;
} ot_scene_static_demosaic;

typedef struct {
    hi_u16  dering_strength[HI_ISP_AUTO_ISO_NUM];
    hi_u8   dering_thresh[HI_ISP_AUTO_ISO_NUM];
    hi_u8   dering_static_strength[HI_ISP_AUTO_ISO_NUM];
    hi_u8   dering_motion_strength[HI_ISP_AUTO_ISO_NUM];
} ot_scene_static_bnr_dering_snr;

typedef struct {
    hi_u16 sfm0_coarse_strength[HI_ISP_BAYER_CHN_NUM][HI_ISP_AUTO_ISO_NUM];
    hi_u8  sfm6_strength[HI_ISP_AUTO_ISO_NUM];
    hi_u8  sfm7_strength[HI_ISP_AUTO_ISO_NUM];
    hi_u8  sth[HI_ISP_AUTO_ISO_NUM];
    hi_u8  sfm1_adp_strength[OT_ISP_AUTO_ISO_NUM];
    hi_u16  sfm1_strength[OT_ISP_AUTO_ISO_NUM];
    hi_u8 sfm0_detail_prot[HI_ISP_AUTO_ISO_NUM];

    hi_u8 fine_strength[HI_ISP_AUTO_ISO_NUM];
    hi_u16 coring_wgt[HI_ISP_AUTO_ISO_NUM];
    hi_u8 coring_mot_ratio[HI_ISP_AUTO_ISO_NUM];
} ot_scene_static_bnr_snr;

typedef struct {
    hi_bool md_mode[HI_ISP_AUTO_ISO_NUM];
    hi_u8 md_anti_flicker_strength[HI_ISP_AUTO_ISO_NUM];
    hi_u8 md_static_ratio[HI_ISP_AUTO_ISO_NUM];
    hi_u8 md_motion_ratio[HI_ISP_AUTO_ISO_NUM];
    hi_u8 md_static_fine_strength[HI_ISP_AUTO_ISO_NUM];
    hi_u8 tfs[HI_ISP_AUTO_ISO_NUM];

    hi_bool user_define_md[HI_ISP_AUTO_ISO_NUM];
    hi_s16 user_define_slope[HI_ISP_AUTO_ISO_NUM];
    hi_u16 user_define_dark_thresh[HI_ISP_AUTO_ISO_NUM];
    hi_u8 user_define_color_thresh[HI_ISP_AUTO_ISO_NUM];

    hi_u8 sfr_r[HI_ISP_AUTO_ISO_NUM];
    hi_u8 sfr_g[HI_ISP_AUTO_ISO_NUM];
    hi_u8 sfr_b[HI_ISP_AUTO_ISO_NUM];
} ot_scene_static_bnr_tnr;

typedef struct {
    hi_bool enable;        /* RW; Range:[0,1];Format:1.0; bayernr enable  */
    hi_op_mode op_type;    /* RW; Range:[0,1];Format:1.0; bayernr operation mode  */
    hi_bool tnr_enable;    /* RW; Range:[0,1];Format:1.0; bayernr tnr enable */
    hi_bool lsc_nr_enable; /* RW; Range:[0,1];Format:1.0; lsc nr enable */
    hi_u8 lsc_ratio1;      /* RW; Range:[0,15];Format:4.0; lsc nr ratio1 */

    hi_u16 coring_ratio[HI_ISP_BAYERNR_LUT_LENGTH];
    hi_u16 mix_gain[HI_ISP_BAYERNR_LUT_LENGTH1];
    ot_scene_static_bnr_snr snr_cfg;
    ot_scene_static_bnr_tnr tnr_cfg;
    ot_scene_static_bnr_dering_snr  dering_snr_cfg;
} ot_scene_static_nr;

typedef struct {
    hi_u8 luma_wgt[HI_ISP_SHARPEN_LUMA_NUM][HI_ISP_AUTO_ISO_NUM]; /* RW; Range:SD3403V100 =[0, 31]; Format:0.5;
                                                                     Adjust the sharpen strength according to luma.
                                                                     Sharpen strength will be weaker when it decrease */
    hi_u16 texture_strength[HI_ISP_SHARPEN_GAIN_NUM][HI_ISP_AUTO_ISO_NUM]; /* RW; Range: [0, 4095]; Format:7.5;
                                                                              Undirectional sharpen strength for texture
                                                                              and detail enhancement */
    hi_u16 edge_strength[HI_ISP_SHARPEN_GAIN_NUM][HI_ISP_AUTO_ISO_NUM]; /* RW; Range: [0, 4095]; Format:7.5; Directional
                                                                           sharpen strength for edge enhancement */
    hi_u16 texture_freq[HI_ISP_AUTO_ISO_NUM]; /* RW; Range: [0, 4095]; Format:6.6;Texture frequency adjustment.
                                                 Texture and detail will be finer when it increase */
    hi_u16 edge_freq[HI_ISP_AUTO_ISO_NUM];    /* RW; Range: [0, 4095]; Format:6.6;Edge frequency adjustment.
                                                 Edge will be narrower and thiner when it increase */
    hi_u8 over_shoot[HI_ISP_AUTO_ISO_NUM];    /* RW; Range: [0, 127];  Format:7.0 */
    hi_u8 under_shoot[HI_ISP_AUTO_ISO_NUM];   /* RW; Range: [0, 127];  Format:7.0 */

    hi_u8 shoot_sup_strength[HI_ISP_AUTO_ISO_NUM]; /* overshoot and undershoot suppression strength */
    hi_u8 shoot_sup_adj[HI_ISP_AUTO_ISO_NUM]; /* RW; Range: [0, 15]; Format:4.0;overshoot and undershoot
                                                 suppression adjusting, adjust the edge shoot suppression strength */
    hi_u8 detail_ctrl[HI_ISP_AUTO_ISO_NUM];           /* RW; Range: [0, 255]; Format:8.0;Different sharpen strength for
                                                         detail and edge. When it is bigger than 128, detail sharpen
                                                         strength will be stronger than edge. */
    hi_u8 detail_ctrl_threshold[HI_ISP_AUTO_ISO_NUM]; /* RW; Range: [0, 255]; Format:8.0; The threshold of DetailCtrl,
                                                         it is used to distinguish detail and edge. */
    hi_u8 edge_filt_strength[HI_ISP_AUTO_ISO_NUM];    /* RW; Range: [0, 63]; Format:6.0;
                                                         The strength of edge filtering. */
    hi_u8 edge_filt_max_cap[HI_ISP_AUTO_ISO_NUM];     /* RW; Range: [0, 47]; Format:6.0;
                                                         The max capacity of edge filtering. */
    hi_u8 r_gain[HI_ISP_AUTO_ISO_NUM];                /* RW; Range: [0, 31]; Format:5.0; Sharpen Gain for Red Area */
    hi_u8 g_gain[HI_ISP_AUTO_ISO_NUM];                /* RW; Range: [0, 255]; Format:8.0; Sharpen Gain for Green Area */
    hi_u8 b_gain[HI_ISP_AUTO_ISO_NUM];                /* RW; Range: [0, 31]; Format:5.0; Sharpen Gain for Blue Area */
    hi_u8 skin_gain[HI_ISP_AUTO_ISO_NUM];             /* RW; Range: [0, 31]; Format:5.0;Sharpen Gain for Skin Area */
    hi_u16 max_sharp_gain[HI_ISP_AUTO_ISO_NUM];       /* RW; Range: [0, 0x7FF]; Format:8.3; Maximum sharpen gain */
    hi_u8 weak_detail_gain[HI_ISP_AUTO_ISO_NUM];      /* RW; Range: [0, 127]; Format:7.0; sharpen Gain for weakdetail */
} ot_scene_static_sharpen_auto;

typedef struct {
    hi_bool enable;          /* RW; Range:[0, 1]; Format:1.0;Enable/Disable sharpen module */
    hi_u8 skin_umin;         /* RW; Range: [0, 255];  Format:8.0; U min value of the range of skin area */
    hi_u8 skin_vmin;         /* RW; Range: [0, 255];  Format:8.0; V min value of the range of skin area */
    hi_u8 skin_umax;         /* RW; Range: [0, 255];  Format:8.0; U max value of the range of skin area */
    hi_u8 skin_vmax;         /* RW; Range: [0, 255];  Format:8.0; V max value of the range of skin area */

    ot_scene_static_sharpen_auto sharpen_auto_cfg;
} ot_scene_static_sharpen;

typedef struct {
    hi_u8  mf_strength[HI_SCENE_BSHP_CURVE_NUM][OT_ISP_AUTO_ISO_NUM];   /* RW; range:[0, 63];mf_strength. */
    hi_u8  hf_strength[HI_SCENE_BSHP_CURVE_NUM][OT_ISP_AUTO_ISO_NUM];   /* RW; range:[0, 63];hf_strength. */
    hi_u8  dark_strength[HI_SCENE_BSHP_CURVE_NUM][OT_ISP_AUTO_ISO_NUM]; /* RW; range:[0, 63];dark_strength. */
    hi_u8  mf_gain[OT_ISP_AUTO_ISO_NUM];    /* RW; range:[0, 15];format:4.0; mf gain */
    hi_u8  hf_gain[OT_ISP_AUTO_ISO_NUM];    /* RW; range:[0, 15];format:4.0; hf gain */
    hi_u8  dark_gain[OT_ISP_AUTO_ISO_NUM];  /* RW; range:[0, 15];format:4.0; dark gain */
    hi_u16 overshoot[OT_ISP_AUTO_ISO_NUM];  /* RW; range:[0, 255];format:8.0; over shoot */
    hi_u16 undershoot[OT_ISP_AUTO_ISO_NUM]; /* RW; range:[0, 255];format:8.0; under shoot */
} ot_scene_static_bayershp_auto;

typedef struct {
    hi_bool enable; /* RW; range:[0, 1]; format:1.0;en/disable bayershp module */
    hi_u16  dark_threshold[OT_ISP_BSHP_THD_NUM];    /* RW; range:[0, 4095];format:12.0; dark threshold */
    hi_u16  texture_threshold[OT_ISP_BSHP_THD_NUM]; /* RW; range:[0, 4095];format:12.0; texture threshold */
    hi_op_mode op_type;
    ot_scene_static_bayershp_auto bayershp_auto_attr;
} ot_scene_static_bayershp;

typedef struct {
    hi_bool enable;
} ot_scene_static_clut;

typedef struct {
    hi_bool enable;
} ot_scene_static_crosstalk;

typedef struct {
    hi_bool enable;
    hi_u32 y_ratio_lut[HI_ISP_CA_YRATIO_LUT_LENGTH];
    hi_u32 y_sat_lut[HI_ISP_CA_YRATIO_LUT_LENGTH];
    hi_u16 iso_ratio[ISP_AUTO_ISO_CA_NUM];
} ot_scene_static_ca;

/* The param of h265e vbr */
typedef struct {
    hi_s32 change_pos;         /* RW; Range:[50, 100];Indicates the ratio of the current bit rate to the maximum
                                     bit rate when the QP value starts to be adjusted */
    hi_u32 min_iprop;          /* RW; [1, 100]the min ratio of i frame and p frame */
    hi_u32 max_iprop;          /* RW; [1, 100]the max ratio of i frame and p frame,can not be smaller than min_iprop */
    hi_s32 max_reencode_times; /* RW; Range:[0, 3]; Range:max number of re-encode times. */

    hi_s32 min_still_percent; /* RW; Range:[5, 100]; the min percent of target bitrate for still scene */
    hi_u32 max_still_qp;      /* RW; Range:[0, 51]; the max QP value of I frame for still scene,
                                      can not be smaller than min_iqp and can not be larger than su32MaxIQp */
    hi_u32 min_still_psnr;    /* RW; reserved */

    hi_u32 max_qp;  /* RW; Range:[0, 51];the max P B qp */
    hi_u32 min_qp;  /* RW; Range:[0, 51];the min P B qp,can not be larger than max_qp */
    hi_u32 max_iqp; /* RW; Range:[0, 51];the max I qp */
    hi_u32 min_iqp; /* RW; Range:[0, 51];the min I qp,can not be larger than max_iqp */

    hi_u32 min_qp_delta;
    hi_u32 motion_sensitivity; /* RW; Range:[0, 100]; Motion Sensitivity */

    hi_bool qp_map_en;                 /* RW; Range:[0, 1]; enable qpmap. */
    hi_venc_rc_qpmap_mode qp_map_mode; /* RW; Qpmap Mode */
} ot_scene_static_venc_param_h265_avbr;

typedef struct {
    hi_u32 min_iprop; /* RW; Range:[1, 100] ; the min ratio of i frame and p frame */
    hi_u32 max_iprop; /* RW; Range:[1, 100] ; the max ratio of i frame and p frame,can not be smaller than min_iprop */
    hi_s32 max_reencode_times; /* RW; Range:[0, 3]; max number of re-encode times */
    hi_bool qp_map_en;
    hi_venc_rc_qpmap_mode qp_map_mode; /* RW; Qpmap Mode */

    hi_u32 max_qp;  /* RW; Range:[0, 51]; the max P B qp */
    hi_u32 min_qp;  /* RW; Range:[0, 51]; the min P B qp,can not be larger than max_qp */
    hi_u32 max_iqp; /* RW; Range:[0, 51]; the max I qp */
    hi_u32 min_iqp; /* RW; Range:[0, 51]; the min I qp,can not be larger than max_iqp */

    hi_u32 min_qp_delta;
    hi_u32 max_qp_delta;
    hi_u32 extra_bit_percent;
    hi_u32 long_term_static_time_unit;
} ot_scene_static_venc_param_h265_cvbr;

typedef struct {
    hi_bool deblur_en;
    hi_bool deblur_adaptive_en;
    ot_scene_static_venc_param_h265_avbr venc_param_h265_avbr;
    ot_scene_static_venc_param_h265_cvbr venc_param_h265_cvbr;
} ot_scene_static_venc_attr;

typedef struct {
    hi_u32 ratio_count;
    hi_u32 ratio_level[HI_SCENE_DRC_RATIO_MAX_COUNT];
    hi_u16 mix_gain[HI_SCENE_NR_LUT_LENGTH1][HI_SCENE_DRC_RATIO_MAX_COUNT];
    hi_u8 md_fusion_frame_str_incr[HI_ISP_WDR_MAX_FRAME_NUM][HI_SCENE_DRC_RATIO_MAX_COUNT];
    hi_u8 snr_sfm0_fusion_frame_str_incr[HI_ISP_WDR_MAX_FRAME_NUM][HI_SCENE_DRC_RATIO_MAX_COUNT];
    hi_u8 md_wdr_frame_str_incr[HI_ISP_WDR_MAX_FRAME_NUM][HI_SCENE_DRC_RATIO_MAX_COUNT];
    hi_u8 snr_sfm0_wdr_frame_str_incr[HI_ISP_WDR_MAX_FRAME_NUM][HI_SCENE_DRC_RATIO_MAX_COUNT];
    hi_u8 snr_fusion_sfm6_sfm7_frame_incr[HI_ISP_WDR_MAX_FRAME_NUM][HI_SCENE_DRC_RATIO_MAX_COUNT];
    hi_u8 snr_wdr_sfm6_sfm7_frame_incr[HI_ISP_WDR_MAX_FRAME_NUM][HI_SCENE_DRC_RATIO_MAX_COUNT];

    hi_u32 coring_ratio_count;
    hi_u32 coring_ratio_iso[HI_SCENE_NR_RATIO_MAX_COUNT];
    hi_u16 coring_ratio[HI_SCENE_NR_LUT_LENGTH][HI_SCENE_ISO_STRENGTH_NUM];
    hi_u8 snr_sfm0_wdr_frame_str[HI_ISP_WDR_MAX_FRAME_NUM][HI_SCENE_ISO_STRENGTH_NUM];
    hi_u8 snr_sfm0_fusion_frame_str[HI_ISP_WDR_MAX_FRAME_NUM][HI_SCENE_ISO_STRENGTH_NUM];
    hi_u8 md_wdr_frame_str[HI_ISP_WDR_MAX_FRAME_NUM][HI_SCENE_ISO_STRENGTH_NUM];
    hi_u8 md_fusion_frame_str[HI_ISP_WDR_MAX_FRAME_NUM][HI_SCENE_ISO_STRENGTH_NUM];
    hi_u8 snr_wdr_sfm6_strength[HI_ISP_WDR_MAX_FRAME_NUM][HI_SCENE_ISO_STRENGTH_NUM];
    hi_u8 snr_wdr_sfm7_strength[HI_ISP_WDR_MAX_FRAME_NUM][HI_SCENE_ISO_STRENGTH_NUM];
    hi_u8 snr_fusion_sfm6_strength[HI_ISP_WDR_MAX_FRAME_NUM][HI_SCENE_ISO_STRENGTH_NUM];
    hi_u8 snr_fusion_sfm7_strength[HI_ISP_WDR_MAX_FRAME_NUM][HI_SCENE_ISO_STRENGTH_NUM];

    /* used for different exp ratio */
    hi_u16 wdr_ratio_threshold;
    hi_u8  fine_strength_l[HI_ISP_AUTO_ISO_NUM];
    hi_u8  fine_strength_h[HI_ISP_AUTO_ISO_NUM];
    hi_u16 coring_wgt_l[HI_ISP_AUTO_ISO_NUM];
    hi_u16 coring_wgt_h[HI_ISP_AUTO_ISO_NUM];
} ot_scene_dynamic_nr;

typedef struct {
    hi_u8 md_fusion_incr[HI_ISP_WDR_MAX_FRAME_NUM];
    hi_u8 sfm0_fusion_incr[HI_ISP_WDR_MAX_FRAME_NUM];
    hi_u8 md_wdr_incr[HI_ISP_WDR_MAX_FRAME_NUM];
    hi_u8 sfm0_wdr_incr[HI_ISP_WDR_MAX_FRAME_NUM];
    hi_u8 snr_fusion_sfm6_sfm7_incr[HI_ISP_WDR_MAX_FRAME_NUM];
    hi_u8 snr_wdr_sfm6_sfm7_incr[HI_ISP_WDR_MAX_FRAME_NUM];
} ot_scene_nr_para;

typedef struct {
    hi_u32 iso_count;
    hi_u32 iso_level[HI_SCENE_ISO_STRENGTH_NUM];
    hi_u8 denoise_y_alpha[HI_SCENE_ISO_STRENGTH_NUM];        /* RW; Range:[0, 255];Format:8.0; */
    hi_u8 denoise_y_mot_ratio[HI_SCENE_ISO_STRENGTH_NUM];    /* RW; Range:[0, 63];  Format:6.0; */
    hi_u8 denoise_y_fg_str_lut[HI_SCENE_BAYERNR_DENOISE_Y_LUT_NUM][HI_SCENE_ISO_STRENGTH_NUM];
    hi_u8 denoise_y_bg_str_lut[HI_SCENE_BAYERNR_DENOISE_Y_LUT_NUM][HI_SCENE_ISO_STRENGTH_NUM];
    hi_u8 denoise_y_mot_map_lut[HI_SCENE_BAYERNR_DENOISE_Y_LUT_NUM][HI_SCENE_ISO_STRENGTH_NUM];
} ot_scene_dynamic_ynet_para;

typedef struct {
    hi_u32 iso_count;
    hi_u32 ratio_count;
    hi_u32 iso_level[HI_SCENE_ISO_STRENGTH_NUM];                                 /* iso threshold count */
    hi_u32 ratio_level[HI_SCENE_RATIO_STRENGTH_NUM];                                 /* iso threshold count */
    hi_u32 blend_weight[HI_SCENE_RATIO_STRENGTH_NUM];                                 /* iso threshold count */
    hi_u32 ca_y_ratio_iso_lut[HI_SCENE_ISO_STRENGTH_NUM][HI_ISP_CA_YRATIO_LUT_LENGTH]; /* cay_ratio_lut count */
    hi_u32 ca_y_ratio_ratio_lut[HI_SCENE_RATIO_STRENGTH_NUM][HI_ISP_CA_YRATIO_LUT_LENGTH]; /* cay_ratio_lut count */
} ot_scene_dynamic_ca;

typedef struct {
    hi_u8  black_level_mode;
    hi_u32 blc_count;
    hi_u32 iso_thresh[ISP_AUTO_ISO_BLC_NUM];
    hi_u32 blc_r[ISP_AUTO_ISO_BLC_NUM];
    hi_u32 blc_gr[ISP_AUTO_ISO_BLC_NUM];
    hi_u32 blc_gb[ISP_AUTO_ISO_BLC_NUM];
    hi_u32 blc_b[ISP_AUTO_ISO_BLC_NUM];
} ot_scene_dynamic_blc;

typedef struct {
    hi_u8  iso_count;
    hi_u32 iso_level[ISP_AUTO_ISO_DPC_NUM];
    hi_u8  sup_twinkle_en[HI_ISP_WDR_MAX_FRAME_NUM][ISP_AUTO_ISO_DPC_NUM];
    hi_u8  soft_thr[HI_ISP_WDR_MAX_FRAME_NUM][ISP_AUTO_ISO_DPC_NUM];
    hi_u8  soft_slope[HI_ISP_WDR_MAX_FRAME_NUM][ISP_AUTO_ISO_DPC_NUM];
    hi_u8  ratio_count;
    hi_u32 ratio_level[ISP_AUTO_ISO_DPC_NUM];
    hi_u8  bright_strength[HI_ISP_WDR_MAX_FRAME_NUM][ISP_AUTO_ISO_DPC_NUM];
    hi_u8  dark_strength[HI_ISP_WDR_MAX_FRAME_NUM][ISP_AUTO_ISO_DPC_NUM];
} ot_scene_dynamic_dpc;

typedef struct {
    hi_bool enable;
} ot_scene_static_shading;

typedef struct {
    hi_u32 exp_thresh_cnt;
    hi_u64 exp_thresh_ltoh[HI_SCENE_SHADING_EXPOSURE_MAX_COUNT];
    hi_u16 mesh_strength[HI_SCENE_SHADING_EXPOSURE_MAX_COUNT];
} ot_scene_dynamic_shading;

typedef struct {
    hi_u32 iso_count;
    hi_u32 iso_level[HI_SCENE_ISO_STRENGTH_NUM];

    hi_u32 gain_r[HI_SCENE_ISO_STRENGTH_NUM];
    hi_u32 gain_g[HI_SCENE_ISO_STRENGTH_NUM];
    hi_u32 gain_b[HI_SCENE_ISO_STRENGTH_NUM];
} ot_scene_dynamic_clut;

typedef struct {
    hi_u8 option;
    hi_u32 interval;
    hi_u32 total_num;
    hi_u8 iso_count;
    hi_u8 ratio_count;
    hi_u64 exp_thresh_ltoh[HI_SCENE_GAMMA_EXPOSURE_MAX_COUNT];
    hi_u64 exp_thresh_htol[HI_SCENE_GAMMA_EXPOSURE_MAX_COUNT];
    hi_u16 table[HI_SCENE_GAMMA_EXPOSURE_MAX_COUNT][HI_ISP_GAMMA_NODE_NUM];
    hi_u16 gamma_idx[HI_SCENE_ISO_STRENGTH_NUM][HI_SCENE_RATIO_STRENGTH_NUM];
} ot_scene_dynamic_gamma;

typedef struct {
    hi_bool enable;
    hi_u32 table[HI_ISP_PREGAMMA_NODE_NUM];
} ot_scene_static_pregamma;

typedef enum {
    OT_ISP_BLC_NORMAL = 0,
    OT_ISP_BLC_GLOBAL,
    OT_ISP_BLC_REAL_TIME,
    OT_ISP_BLC_MODE_BUTT
} ot_isp_blc_process_sel;


typedef struct {
    hi_bool enable;
    ot_isp_blc_process_sel blc_sel;
    hi_u16 user_offset[HI_ISP_BAYER_CHN_NUM];
} ot_scene_static_blc;

typedef struct {
    hi_u32 exp_thresh_cnt;
    hi_u64 exp_thresh_ltoh[HI_SCENE_DEHAZE_EXPOSURE_MAX_COUNT];
    hi_u8 manual_strength[HI_SCENE_DEHAZE_EXPOSURE_MAX_COUNT];
    hi_u32 wdr_ratio_threshold;
    hi_u8 manual_strengther[HI_SCENE_DEHAZE_EXPOSURE_MAX_COUNT];
} ot_scene_dynamic_dehaze;

typedef struct {
    hi_u8 md_thr_low_gain[HI_SCENE_RATIO_STRENGTH_NUM][HI_SCENE_ISO_STRENGTH_NUM];
    hi_u8 md_thr_high_gain[HI_SCENE_RATIO_STRENGTH_NUM][HI_SCENE_ISO_STRENGTH_NUM];
} ot_scene_wdr_combine_attr;

typedef struct {
    hi_u8  ratio_count;
    hi_u32 ratio_level[ISP_AUTO_RATIO_FSWDR_NUM];
    hi_u8  wdr_merge_mode[ISP_AUTO_RATIO_FSWDR_NUM];
    hi_u8  motion_comp[ISP_AUTO_RATIO_FSWDR_NUM];

    hi_u8  iso_count;
    hi_u32 ratio_thr;
    hi_u32 iso_ltoh_thresh[HI_SCENE_ISO_STRENGTH_NUM];
    hi_bool fusion_blend_en[HI_SCENE_ISO_STRENGTH_NUM];
    hi_u8 fusion_blend_wgt[HI_SCENE_ISO_STRENGTH_NUM];
    hi_bool fusion_blend_en_h[HI_SCENE_ISO_STRENGTH_NUM];
    hi_u8 fusion_blend_wgt_h[HI_SCENE_ISO_STRENGTH_NUM];
} ot_scene_dynamic_fswdr;

typedef struct {
    hi_u32 total_num;
    hi_u32 false_color_exp_thresh[HI_SCENE_FALSECOLOR_EXPOSURE_MAX_COUNT];
    hi_u8 manual_strength[HI_SCENE_FALSECOLOR_EXPOSURE_MAX_COUNT];
} ot_scene_dynamic_false_color;

typedef struct {
    hi_u32 iso_count;
    hi_u32 iso_thresh[HI_SCENE_ISO_STRENGTH_NUM];
    hi_bool fusion_mono_flat_en[HI_SCENE_ISO_STRENGTH_NUM];
    hi_bool fusion_color_ratio_en[HI_SCENE_ISO_STRENGTH_NUM];
    hi_bool fusion_mono_ratio_en[HI_SCENE_ISO_STRENGTH_NUM];
    hi_u8  fusion_ratio_scale[HI_SCENE_ISO_STRENGTH_NUM];
    hi_u8  lf_mono_flt_radius[HI_SCENE_ISO_STRENGTH_NUM];
    hi_u8  mf_mono_flt_radius[HI_SCENE_ISO_STRENGTH_NUM];
    hi_u8  hf_mono_flt_radius[HI_SCENE_ISO_STRENGTH_NUM];
    hi_u8  lf_fusion_det_gain[HI_SCENE_ISO_STRENGTH_NUM];
    hi_u8  mf_fusion_det_gain[HI_SCENE_ISO_STRENGTH_NUM];
    hi_u8  hf_fusion_det_gain[HI_SCENE_ISO_STRENGTH_NUM];
    hi_u16 cc_uv_gain_lut[HI_SCENE_ISO_STRENGTH_NUM][HI_SCENE_MCF_CC_UV_GAIN_LUT_NUM];
    hi_u8  lf_fusion_alpha_lut[HI_SCENE_ISO_STRENGTH_NUM][HI_SCENE_MCF_WEIGHT_LUT_NUM];
    hi_u8  mf_fusion_alpha_lut[HI_SCENE_ISO_STRENGTH_NUM][HI_SCENE_MCF_WEIGHT_LUT_NUM];
    hi_u8  hf_fusion_alpha_lut[HI_SCENE_ISO_STRENGTH_NUM][HI_SCENE_MCF_WEIGHT_LUT_NUM];
    hi_u8  fusion_color_ratio_lut[HI_SCENE_ISO_STRENGTH_NUM][HI_SCENE_MCF_WEIGHT_LUT_NUM];
    hi_u8  lf_fusion_mono_det_lut[HI_SCENE_ISO_STRENGTH_NUM][HI_SCENE_MCF_WEIGHT_LUT_NUM];
    hi_u8  mf_fusion_mono_det_lut[HI_SCENE_ISO_STRENGTH_NUM][HI_SCENE_MCF_WEIGHT_LUT_NUM];
    hi_u8  hf_fusion_mono_det_lut[HI_SCENE_ISO_STRENGTH_NUM][HI_SCENE_MCF_WEIGHT_LUT_NUM];
} ot_scene_dynamic_mcf;

typedef struct {
    hi_u32 aibnr_iso_thresh[AIBNR_ISO_NUM];
    hi_bool aibnr_chg_en;
    hi_bool blend_en;
    hi_op_mode op_type;
    hi_u8 manual_id; /* RW; Range:[0, 15]; Format:4.0 */
    hi_u8 auto_id[OT_AIISP_AUTO_ISO_NUM]; /* RW; Range:[0, 15]; Format:4.0 */
    hi_s32 pipe_id;
    hi_u32 iso_count;
    hi_u32 iso_level[HI_SCENE_ISO_STRENGTH_NUM];
    hi_u32 sfs[HI_SCENE_ISO_STRENGTH_NUM];
} ot_scene_dynamic_aibnr;

typedef struct {
    hi_u32 iso_count;
    hi_bool ai3dnr_en[HI_SCENE_ISO_STRENGTH_NUM];
    hi_u32 iso_level[HI_SCENE_ISO_STRENGTH_NUM];
    hi_u32 uv_denoise_fg[HI_SCENE_ISO_STRENGTH_NUM];
    hi_u32 uv_denoise_bg[HI_SCENE_ISO_STRENGTH_NUM];
    hi_u8 denoise_y_fg_str_lut[HI_SCENE_BAYERNR_DENOISE_Y_LUT_NUM][HI_SCENE_ISO_STRENGTH_NUM];
    hi_u8 denoise_y_bg_str_lut[HI_SCENE_BAYERNR_DENOISE_Y_LUT_NUM][HI_SCENE_ISO_STRENGTH_NUM];
} ot_scene_dynamic_ai3dnr;

typedef struct {
    hi_u32 deblur_mode_iso_thresh[VENC_MODE_ISO_NUM];
    hi_bool deblur_en[VENC_MODE_ISO_NUM];
    hi_bool deblur_adaptive_en[VENC_MODE_ISO_NUM];
} ot_scene_dynamic_venc_mode;

typedef struct {
    hi_u32 iso_count;
    hi_bool aibnr_mode;
    hi_u32 fpn_iso_thresh;
    hi_u32 iso_thresh[HI_SCENE_ISO_STRENGTH_NUM];
    hi_u32 fpn_offset[HI_SCENE_ISO_STRENGTH_NUM];
} ot_scene_dynamic_fpn;

typedef struct {
    ot_scene_module_state module_state;
    ot_scene_static_ae static_ae;
    ot_scene_static_ae_route static_ae_route;
    ot_scene_static_ae_route_ex static_ae_route_ex;
    ot_scene_static_statisticscfg static_statistics;
    ot_scene_static_wdr_exposure static_wdr_exposure;
    ot_scene_static_fswdr static_fswdr;
    ot_scene_static_awb static_awb;
    ot_scene_static_awb_ex static_awb_ex;
    ot_scene_isp_pipe_diff static_pipe_diff;
    ot_scene_static_ccm static_ccm;
    ot_scene_static_saturation static_saturation;
    ot_scene_static_clut static_clut;
    ot_scene_static_ldci static_ldci;
    ot_scene_static_drc static_drc;
    ot_scene_static_aidrc static_aidrc;
    ot_scene_static_nr static_nr;
    ot_scene_static_ca static_ca;
    ot_scene_static_pregamma static_pregamma;
    ot_scene_static_blc static_blc;
    ot_scene_static_venc_attr static_venc_attr;
    ot_scene_static_cac static_cac;
    ot_scene_static_dpc static_dpc;
    ot_scene_static_dehaze static_dehaze;
    ot_scene_static_shading static_shading;
    ot_scene_static_csc staic_csc;
    ot_scene_static_crosstalk static_crosstalk;
    ot_scene_static_demosaic static_dm;
    ot_scene_static_sharpen static_sharpen;
    ot_scene_static_bayershp static_bayershp;
    ot_scene_static_3dnr static_threednr;

    ot_scene_dynamic_fps dynamic_fps;
    ot_scene_dynamic_ae dynamic_ae;
    ot_scene_dynamic_wdr_exposure dynamic_wdr_exposure;
    ot_scene_dynamic_fswdr dynamic_fswdr;
    ot_scene_dynamic_dehaze dynamic_dehaze;
    ot_scene_dynamic_drc dynamic_drc;
    ot_scene_dynamic_aidrc dynamic_aidrc;
    ot_scene_dynamic_linear_drc dynamic_linear_drc;
    ot_scene_dynamic_gamma dynamic_gamma;
    ot_scene_dynamic_nr dynamic_nr;
    ot_scene_dynamic_ca dynamic_ca;
    ot_scene_dynamic_blc dynamic_blc;
    ot_scene_dynamic_dpc dynamic_dpc;
    ot_scene_dynamic_shading dynamic_shading;
    ot_scene_dynamic_clut dynamic_clut;
    ot_scene_dynamic_venc_bitrate dynamic_venc_bitrate;
    ot_scene_dynamic_ldci dynamic_ldci;
    ot_scene_dynamic_false_color dynamic_false_color;
    ot_scene_dynamic_mcf dynamic_mcf;
    ot_scene_dynamic_aibnr dynamic_aibnr;
    ot_scene_dynamic_ai3dnr dynamic_ai3dnr;
    ot_scene_dynamic_fpn dynamic_fpn;
    ot_scene_dynamic_venc_mode dynamic_venc_mode;
} ot_scene_pipe_param;

typedef struct {
    ot_scene_3dnr *nrx_attr;
    const ot_scene_static_3dnr *nrx_param;
    hi_u32 iso_level;
    hi_u32 mid;
    hi_u32 left;
    hi_u32 right;
} _3dnr_nrx_pack;

hi_s32 ot_scene_set_static_ae(hi_vi_pipe vi_pipe, hi_u8 index);

hi_s32 ot_scene_set_dynamic_fps(hi_vi_pipe vi_pipe, hi_u64 exposure, hi_u64 last_exposure, hi_u8 index);

hi_s32 ot_scene_set_dynamic_ae(hi_vi_pipe vi_pipe, hi_u64 exposure, hi_u64 last_exposure, hi_u8 index);

hi_s32 ot_scene_set_static_cac(hi_vi_pipe vi_pipe, hi_u8 index);

hi_s32 ot_scene_set_static_dpc(hi_vi_pipe vi_pipe, hi_u8 index);

hi_s32 ot_scene_set_static_wdr_exposure(hi_vi_pipe vi_pipe, hi_u8 index);

hi_s32 ot_scene_set_static_fswdr(hi_vi_pipe vi_pipe, hi_u8 index);

hi_s32 ot_scene_set_static_drc(hi_vi_pipe vi_pipe, hi_u8 index);

hi_s32 ot_scene_set_static_aidrc(hi_vi_pipe vi_pipe, hi_u8 index);

hi_s32 ot_scene_set_static_dehaze(hi_vi_pipe vi_pipe, hi_u8 index);

hi_s32 ot_scene_set_static_ldci(hi_vi_pipe vi_pipe, hi_u8 index);

hi_s32 ot_scene_set_static_awb(hi_vi_pipe vi_pipe, hi_u8 index);

hi_s32 ot_scene_set_static_awbex(hi_vi_pipe vi_pipe, hi_u8 index);

hi_s32 ot_scene_set_static_pipe_diff(hi_vi_pipe vi_pipe, hi_u8 index);

hi_s32 ot_scene_set_saturation(hi_vi_pipe vi_pipe, hi_u8 index);

hi_s32 ot_scene_set_static_ccm(hi_vi_pipe vi_pipe, hi_u8 index);

hi_s32 ot_scene_set_static_csc(hi_vi_pipe vi_pipe, hi_u8 index);

hi_s32 ot_scene_set_static_nr(hi_vi_pipe vi_pipe, hi_u8 index);

hi_s32 ot_scene_set_static_shading(hi_vi_pipe vi_pipe, hi_u8 index);

hi_s32 ot_scene_set_ae_weight_table(hi_vi_pipe vi_pipe, hi_u8 index);

hi_s32 ot_scene_set_static_3dnr(hi_vi_pipe vi_pipe, hi_u8 index);

hi_s32 ot_scene_set_static_pregamma(hi_vi_pipe vi_pipe, hi_u8 index);

hi_s32 ot_scene_set_static_blc(hi_vi_pipe vi_pipe, hi_u8 index);

hi_s32 ot_scene_set_dynamic_3dnr(hi_vi_pipe vi_pipe, hi_u32 iso, hi_u8 index, hi_3dnr_pos_type pos_3dnr);

hi_s32 ot_scene_set_static_demosaic(hi_vi_pipe vi_pipe, hi_u8 index);

hi_s32 ot_scene_set_static_sharpen(hi_vi_pipe vi_pipe, hi_u8 index);

hi_s32 ot_scene_set_static_bayershp(hi_vi_pipe vi_pipe, hi_u8 index);

hi_s32 ot_scene_set_static_clut(hi_vi_pipe vi_pipe, hi_u8 index);

hi_s32 ot_scene_set_static_ca(hi_vi_pipe vi_pipe, hi_u8 index);

hi_s32 ot_scene_set_static_venc(hi_vi_pipe vi_pipe, hi_u8 index);

hi_s32 ot_scene_set_static_crosstalk(hi_vi_pipe vi_pipe, hi_u8 index);

hi_s32 ot_scene_set_dynamic_photo_gamma(hi_vi_pipe vi_pipe, hi_u64 exposure, hi_u64 last_exposure, hi_u8 index);

hi_s32 ot_scene_set_dynamic_video_gamma(hi_vi_pipe vi_pipe, hi_u64 exposure, hi_u64 last_exposure, hi_u8 index);

hi_s32 ot_scene_set_dynamic_wdr_exposure(hi_vi_pipe vi_pipe, hi_u64 exposure, hi_u64 last_exposure, hi_u8 index);

hi_s32 ot_scene_set_dynamic_shading(hi_vi_pipe vi_pipe, hi_u64 exposure, hi_u64 last_exposure, hi_u8 index);

hi_s32 ot_scene_set_dynamic_clut(hi_vi_pipe vi_pipe, hi_u64 iso, hi_u64 last_iso, hi_u8 index);

hi_s32 ot_scene_set_dynamic_linear_ca(hi_vi_pipe vi_pipe, hi_u64 iso, hi_u64 last_iso, hi_u8 index);

hi_s32 ot_scene_set_dynamic_venc_bitrate(hi_vi_pipe vi_pipe, hi_u64 iso, hi_u64 last_iso, hi_u8 index);

hi_s32 ot_scene_set_dynamic_mcf(hi_vi_pipe vi_pipe, hi_u64 iso, hi_u64 last_iso, hi_u8 index);

hi_s32 ot_scene_set_dynamic_aibnr(hi_vi_pipe vi_pipe, hi_u64 iso, hi_u64 last_iso, hi_u8 index);

hi_s32 ot_scene_set_dynamic_ai3dnr(hi_vi_pipe vi_pipe, hi_u64 iso, hi_u64 last_iso, hi_u8 index);

hi_s32 ot_scene_set_dynamic_fpn(hi_vi_pipe vi_pipe, hi_u64 iso, hi_u64 last_iso, hi_u8 index);

hi_s32 ot_scene_set_dynamic_nr(hi_vi_pipe vi_pipe, hi_u8 index, hi_u32 wdr_ratio, hi_u32 iso);

hi_s32 ot_scene_set_dynamic_ca(hi_vi_pipe vi_pipe, hi_u8 index, hi_u32 wdr_ratio, hi_u32 iso);

hi_s32 ot_scene_set_dynamic_blc(hi_vi_pipe vi_pipe, hi_u64 iso, hi_u64 last_iso, hi_u8 index);

hi_s32 ot_scene_set_dynamic_dpc(hi_vi_pipe vi_pipe, hi_u64 iso, hi_u64 last_iso, hi_u8 index);

hi_s32 ot_scene_set_dynamic_dpc_ratio(hi_vi_pipe vi_pipe, hi_u8 index, hi_u32 wdr_ratio);

hi_s32 ot_scene_set_dynamic_ldci(hi_vi_pipe vi_pipe, hi_u64 exposure, hi_u64 last_exposure, hi_u8 index);

hi_s32 ot_scene_set_dynamic_black_level(hi_vi_pipe vi_pipe, hi_u64 exposure, hi_u64 last_exposure, hi_u8 index);

hi_s32 ot_scene_set_dynamic_false_color(hi_vi_pipe vi_pipe, hi_u64 exposure, hi_u64 last_exposure, hi_u8 index);

hi_s32 ot_scene_set_dynamic_dehaze(hi_vi_pipe vi_pipe, hi_u64 exposure, hi_u64 last_exposure, hi_u8 index);

hi_s32 ot_scene_set_dynamic_fswdr(hi_vi_pipe vi_pipe, hi_u32 iso, hi_u32 last_iso, hi_u8 index, hi_u32 wdr_ratio);

hi_s32 ot_scene_set_dynamic_drc(hi_vi_pipe vi_pipe, hi_u8 index, hi_u32 wdr_ratio, hi_u32 iso);

hi_s32 ot_scene_set_dynamic_aidrc(hi_vi_pipe vi_pipe, hi_u8 index, hi_u32 wdr_ratio, hi_u32 iso);

hi_s32 ot_scene_set_dynamic_linear_drc(hi_vi_pipe vi_pipe, hi_u64 iso, hi_u64 last_iso, hi_u8 index);

hi_s32 ot_scene_set_dynamic_venc_mode(hi_vi_pipe vi_pipe, hi_u64 iso, hi_u64 last_iso, hi_u8 index);

hi_s32 ot_scene_set_pipe_param(const ot_scene_pipe_param *scene_pipe_param, hi_u32 num);

hi_s32 ot_scene_set_pause(hi_bool pause);

hi_s32 ot_scene_set_rc_param(hi_vi_pipe vi_pipe, hi_u8 idx);

hi_void set_dir_name(const hi_char *dir_name);

hi_s32 ot_scene_release_fpn(hi_vi_pipe vi_pipe, hi_u8 index);

#pragma pack()
#ifdef __cplusplus
}
#endif

#endif
