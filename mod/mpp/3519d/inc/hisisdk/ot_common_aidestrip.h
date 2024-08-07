/*
  Copyright (c), 2001-2024, Shenshu Tech. Co., Ltd.
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
    td_u32 input_depth; /* RW; Range:[1, 64]; Format:7.0 */
    td_u32 output_depth; /* RW; Range:[1, 64]; Format:7.0 */
    ot_aidestrip_param param;
} ot_aidestrip_attr;

typedef struct {
    ot_size size;
} ot_aidestrip_cfg;

typedef struct {
    td_bool enable;     /* RW; Range:[0x0, 0x1]; Format:1.0 */
} ot_aidestrip_status;

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif
