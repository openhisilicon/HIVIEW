/*
  Copyright (c), 2001-2024, Shenshu Tech. Co., Ltd.
 */

#ifndef OT_COMMON_AIDRC_H
#define OT_COMMON_AIDRC_H

#include "ot_common.h"
#include "ot_common_video.h"
#include "ot_common_aiisp.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define OT_AIDRC_LMIX_NODE_NUM    33

typedef enum {
    OT_AIDRC_MODE_NORMAL = 0x0,
    OT_AIDRC_MODE_ADVANCED,
    OT_AIDRC_MODE_BUTT
} ot_aidrc_mode;

typedef struct {
    ot_aiisp_model model;
    ot_aidrc_mode mode;
} ot_aidrc_model;

typedef struct {
    td_u8 tone_mapping_wgt_x; /* RW; Range:[0x0, 0x80]; Local TM gain scaling coef */
    td_u8 detail_adjust_coef_x; /* RW; Range:[0x0, 0xF]; X filter detail adjust coefficient */
    td_u8 local_mixing_bright_x[OT_AIDRC_LMIX_NODE_NUM]; /* RW; Range:[0x0, 0x80];
                                                               LUT of enhancement coefficients for positive details */
    td_u8 local_mixing_dark_x[OT_AIDRC_LMIX_NODE_NUM]; /* RW; Range:[0x0, 0x80];
                                                             LUT of enhancement coefficients for positive details */
    td_u8 blend_luma_max; /* RW; Range:[0x0, 0xFF]; Luma-based filter blending weight control */
    td_u8 blend_luma_bright_min; /* RW; Range:[0x0, 0xFF]; Luma-based filter blending weight control */
    td_u8 blend_luma_bright_threshold; /* RW; Range:[0x0, 0xFF]; Luma-based filter blending weight control */
    td_u8 blend_luma_dark_min; /* RW; Range:[0x0, 0xFF]; Luma-based filter blending weight control */
    td_u8 blend_luma_dark_threshold; /* RW; Range:[0x0, 0xFF]; Luma-based filter blending weight control */
    td_u8 blend_detail_max; /* RW; Range:[0x0, 0xFF]; Detail-based filter blending weight control */
    td_u8 blend_detail_bright_min; /* RW; Range:[0x0, 0xFF]; Detail-based filter blending weight control */
    td_u8 blend_detail_bright_threshold; /* RW; Range:[0x0, 0xFF]; Detail-based filter blending weight control */
    td_u8 blend_detail_dark_min; /* RW; Range:[0x0, 0xFF]; Detail-based filter blending weight control */
    td_u8 blend_detail_dark_threshold; /* RW; Range:[0x0, 0xFF]; Detail-based filter blending weight control */
    td_u8 detail_adjust_coef_blend; /* RW; Range:[0x0, 0xF]; Extra detail gain of X filter details */
} ot_aidrc_blend;

typedef struct {
    td_u32 strength; /* RW; filter strength, larger value gives stronger details. Range:[0, 7]; Format:3.0 */
    td_u32 threshold;  /* RW; noise threshold, larger value means more noise suppression, Range: [0, 7]; Format: 3.0 */
    ot_aidrc_blend blend;
} ot_aidrc_param;

typedef struct {
    td_bool enable;      /* RW; Range:[0x0, 0x1]; Format:1.0 */
    td_u32 input_depth;  /* RW; Range:[0, 0x8]; Format:4.0 */
    td_u32 output_depth; /* RW; Range:[0, 0x8]; Format:4.0 */
    ot_aidrc_param param;
} ot_aidrc_attr;

typedef struct {
    ot_aidrc_mode mode;
} ot_aidrc_cfg;

typedef struct {
    td_bool enable;
} ot_aidrc_smooth;

typedef struct {
    td_bool enable;     /* RW; Range:[0x0, 0x1]; Format:1.0 */
} ot_aidrc_status;

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* OT_COMMON_AIDRC_H */
