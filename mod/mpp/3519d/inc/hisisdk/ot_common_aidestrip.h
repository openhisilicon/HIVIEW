/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */

#ifndef OT_COMMON_AIDESTRIP_H
#define OT_COMMON_AIDESTRIP_H

#include "ot_common_aiisp.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

typedef struct {
    ot_aiisp_model model;
} ot_aidestrip_model;

typedef struct {
    td_u32 strength; /* RW; Range:[0, 31]; Format:5.0 */
} ot_aidestrip_param;

typedef struct {
    td_bool enable;
    td_u32 input_depth;
    td_u32 output_depth;
    ot_aidestrip_param param;
} ot_aidestrip_attr;

typedef struct {
    ot_size size;
} ot_aidestrip_cfg;

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif
