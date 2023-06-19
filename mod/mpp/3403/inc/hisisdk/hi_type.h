/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */

#ifndef __HI_TYPE_H__
#define __HI_TYPE_H__

#include "ot_type.h"

#ifdef __cplusplus
extern "C" {
#endif

#define HI_NULL TD_NULL
#define HI_SUCCESS TD_SUCCESS
#define HI_FAILURE TD_FAILURE
typedef td_uchar hi_uchar;
typedef td_u8 hi_u8;
typedef td_u16 hi_u16;
typedef td_u32 hi_u32;
typedef td_ulong hi_ulong;
typedef td_char hi_char;
typedef td_s8 hi_s8;
typedef td_s16 hi_s16;
typedef td_s32 hi_s32;
typedef td_slong hi_slong;
typedef td_float hi_float;
typedef td_double hi_double;
typedef td_void hi_void;
typedef td_u64 hi_u64;
typedef td_s64 hi_s64;
typedef td_phys_addr_t hi_phys_addr_t;
typedef td_handle hi_handle;
typedef td_uintptr_t hi_uintptr_t;
typedef td_fr32 hi_fr32;
#define HI_FALSE TD_FALSE
#define HI_TRUE TD_TRUE
typedef td_bool hi_bool;

#ifdef __cplusplus
}
#endif
#endif /* __HI_TYPE_H__ */
