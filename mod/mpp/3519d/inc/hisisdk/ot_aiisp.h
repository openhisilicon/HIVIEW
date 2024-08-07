/*
  Copyright (c), 2001-2024, Shenshu Tech. Co., Ltd.
 */

#ifndef OT_AIISP_H
#define OT_AIISP_H

#include "ot_common_vb.h"
#include "ot_common_aibnr.h"
#include "ot_common_aidrc.h"
#include "ot_common_ai3dnr.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

typedef struct {
    ot_vb_pool vb_pool;
} ot_aibnr_pool;

typedef struct {
    ot_vb_pool in_vb_pool;
    ot_vb_pool out_vb_pool;
} ot_aidrc_pool;

typedef struct {
    ot_vb_pool in_vb_pool;
    ot_vb_pool out_vb_pool;
} ot_aidm_pool;

typedef struct {
    ot_vb_pool vb_pool;
    ot_ai3dnr_net_type net_type;
} ot_ai3dnr_pool;

typedef struct {
    ot_aiisp_type aiisp_type;
    union {
        ot_aibnr_pool aibnr_pool; /* AUTO:ot_aiisp_type:OT_AIISP_TYPE_AIBNR */
        ot_aidrc_pool aidrc_pool; /* AUTO:ot_aiisp_type:OT_AIISP_TYPE_AIDRC */
        ot_aidm_pool aidm_pool; /* AUTO:ot_aiisp_type:OT_AIISP_TYPE_AIDM */
        ot_ai3dnr_pool ai3dnr_pool; /* AUTO:ot_aiisp_type:OT_AIISP_TYPE_AI3DNR */
    };
} ot_aiisp_pool;

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif
