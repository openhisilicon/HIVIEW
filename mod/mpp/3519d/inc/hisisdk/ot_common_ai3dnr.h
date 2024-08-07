/*
  Copyright (c), 2001-2024, Shenshu Tech. Co., Ltd.
 */

#ifndef OT_COMMON_AI3DNR_H
#define OT_COMMON_AI3DNR_H

#include "ot_common.h"
#include "ot_common_video.h"
#include "ot_common_aiisp.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define OT_AI3DNR_GAMMA_LUT_NUM 16
#define OT_AI3DNR_BAYERNR_DENOISE_Y_LUT_NUM 16

typedef enum {
    OT_AI3DNR_NET_TYPE_Y = 0,
    OT_AI3DNR_NET_TYPE_UV,
    OT_AI3DNR_NET_TYPE_YUV,
    OT_AI3DNR_NET_TYPE_BUTT
} ot_ai3dnr_net_type;

typedef struct {
    ot_aiisp_model model;
    ot_ai3dnr_net_type net_type;
} ot_ai3dnr_model;

typedef struct {
    // only y net need cfg
    td_u32 denoise_fg; /* RW; Range:[0, 31]; Format:5.0 */
    td_u32 denoise_bg; /* RW; Range:[0, 31]; Format:5.0 */
    td_u32 depattern_fg; /* RW; Range:[0, 31]; Format:5.0 */
    td_u32 depattern_bg; /* RW; Range:[0, 31]; Format:5.0 */
    td_u32 sharp_str; /* RW; Range:[0, 31]; Format:5.0 */
    td_u32 coring_str; /* RW; Range:[0, 31]; Format:5.0 */
    td_u8 denoise_y_alpha; /* RW; Range:[0, 255];Format:8.0; */
    td_u8 denoise_y_fg_str_lut[OT_AI3DNR_BAYERNR_DENOISE_Y_LUT_NUM]; /* RW; Range:[0, 255];Format:8.0; */
    td_u8 denoise_y_bg_str_lut[OT_AI3DNR_BAYERNR_DENOISE_Y_LUT_NUM]; /* RW; Range:[0, 255];Format:8.0; */
    td_u8 gamma_lut[OT_AI3DNR_GAMMA_LUT_NUM]; /* RW; Range:[0, 255];Format:8.0; */

    // only uv net need cfg
    td_u32 uv_denoise_fg; /* RW; Range:[0, 31]; Format:5.0 */
    td_u32 uv_denoise_bg; /* RW; Range:[0, 31]; Format:5.0 */
    td_u32 uv_coring_str; /* RW; Range:[0, 31]; Format:5.0 */
} ot_ai3dnr_strength;

typedef struct {
    td_bool enable;
    td_u32 input_depth; /* RW; Range:[0x0, 0x8]; Format:4.0 */
    td_u32 output_depth; /* RW; Range:[0x0, 0x8]; Format:4.0 */
    ot_ai3dnr_strength strength;
    ot_ai3dnr_adv_param adv_param; /* only y net need cfg */
} ot_ai3dnr_attr;

typedef struct {
    ot_ai3dnr_net_type net_type;
} ot_ai3dnr_cfg;

typedef struct {
    td_bool enable; /* RW; Range:[0x0, 0x1]; Format:1.0 */
} ot_ai3dnr_smooth;

typedef struct {
    td_bool enable;     /* RW; Range:[0x0, 0x1]; Format:1.0 */
} ot_ai3dnr_status;

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif
