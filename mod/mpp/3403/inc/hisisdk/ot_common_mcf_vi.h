/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */

#ifndef __OT_COMMON_MCF_VI_H__
#define __OT_COMMON_MCF_VI_H__

#include "ot_common.h"
#include "ot_common_mcf.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define OT_MAX_MCF_ID 2

typedef td_u32 ot_mcf_id;

typedef struct {
    td_bool    enable;
    ot_vi_pipe mono_pipe;
    ot_vi_pipe color_pipe;
} ot_mcf_vi_attr;

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __OT_COMMON_MCF_VI_H__ */