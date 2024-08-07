/*
  Copyright (c), 2001-2024, Shenshu Tech. Co., Ltd.
 */

#ifndef OT_COMMON_SYS_BIND_H
#define OT_COMMON_SYS_BIND_H

#include "ot_common.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    td_u32 num;
    ot_mpp_chn mpp_chn[OT_MAX_BIND_DST_NUM];
} ot_mpp_bind_dst;

#ifdef __cplusplus
}
#endif
#endif /* OT_COMMON_SYS_BIND_H */
