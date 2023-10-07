/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */

#ifndef OT_COMMON_AIBNR_H
#define OT_COMMON_AIBNR_H

#include "ot_common.h"
#include "ot_common_video.h"
#include "ot_common_aiisp.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

typedef enum {
    OT_AIBNR_REF_MODE_NORM = 0,
    OT_AIBNR_REF_MODE_NONE,
    OT_AIBNR_REF_MODE_BUTT
} ot_aibnr_ref_mode;

typedef struct {
    ot_aiisp_model model;
    td_bool is_wdr_mode;
    ot_aibnr_ref_mode ref_mode;
} ot_aibnr_model;

typedef struct {
    td_u32 sfs; /* RW; spatial filter strength. Range:[0, 31]; Format:5.0 */
    td_u32 tfs; /* RW; temporal filter strength. Range:[0, 31]; Format:5.0 */
} ot_aibnr_manual_attr;

typedef struct {
    td_u32 sfs[OT_AIISP_AUTO_ISO_NUM]; /* RW; spatial filter strength. Range:[0, 31]; Format:5.0 */
    td_u32 tfs[OT_AIISP_AUTO_ISO_NUM]; /* RW; temporal filter strength. Range:[0, 31]; Format:5.0 */
} ot_aibnr_auto_attr;

typedef struct {
    td_bool enable;     /* RW; Range:[0x0, 0x1]; Format:1.0 */
    td_bool bnr_bypass; /* RW; Range:[0x0, 0x1]; Format:1.0 */
    td_bool blend;      /* RW; Range:[0x0, 0x1]; Format:1.0 */
    td_u32 input_depth;
    td_u32 output_depth;
    ot_op_mode op_type;
    ot_aibnr_manual_attr manual_attr;
    ot_aibnr_auto_attr auto_attr;
} ot_aibnr_attr;

typedef struct {
    ot_aibnr_ref_mode ref_mode;
} ot_aibnr_cfg;

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* OT_COMMON_AIBNR_H */
