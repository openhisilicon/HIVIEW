/*
  Copyright (c), 2001-2024, Shenshu Tech. Co., Ltd.
 */

#ifndef OT_COMMON_SYS_MEM_H
#define OT_COMMON_SYS_MEM_H

#include "ot_type.h"
#include "ot_errno.h"

#define OT_ERR_MEM_ILLEGAL_PARAM OT_DEFINE_ERR(OT_ID_MEM, OT_ERR_LEVEL_ERROR, OT_ERR_ILLEGAL_PARAM)
#define OT_ERR_MEM_NULL_PTR      OT_DEFINE_ERR(OT_ID_MEM, OT_ERR_LEVEL_ERROR, OT_ERR_NULL_PTR)
#define OT_ERR_MEM_NOT_PERM      OT_DEFINE_ERR(OT_ID_MEM, OT_ERR_LEVEL_ERROR, OT_ERR_NOT_PERM)
#define OT_ERR_MEM_NOT_READY     OT_DEFINE_ERR(OT_ID_MEM, OT_ERR_LEVEL_ERROR, OT_ERR_NOT_READY)
#define OT_ERR_MEM_BUSY          OT_DEFINE_ERR(OT_ID_MEM, OT_ERR_LEVEL_ERROR, OT_ERR_BUSY)

typedef struct {
    td_phys_addr_t phys_addr;
    td_bool is_cached;
} ot_sys_virt_mem_info;

typedef struct {
    td_phys_addr_t phys_addr;
    td_u64 offset;
    td_void *mem_handle;
} ot_sys_mem_info;

#endif /* OT_COMMON_SYS_MEM_H */
