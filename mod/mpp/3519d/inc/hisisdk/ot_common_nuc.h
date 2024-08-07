/*
  Copyright (c), 2001-2024, Shenshu Tech. Co., Ltd.
 */

#ifndef OT_COMMON_NUC_H
#define OT_COMMON_NUC_H

#include "ot_common_aiisp.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

typedef struct {
    ot_aiisp_model model;
} ot_nuc_model;

typedef struct {
    td_u16 bias;
    td_u16 nuc_black_level;
    td_u16 *k_matrix;
    td_u32 k_matrix_size;
} ot_nuc_param;

typedef struct {
    td_u32 input_depth; /* RW; Range:[1, 64]; Format:7.0 */
    td_u32 output_depth; /* RW; Range:[1, 64]; Format:7.0 */
    ot_size size;
    ot_nuc_param param;
} ot_nuc_cfg;

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif
