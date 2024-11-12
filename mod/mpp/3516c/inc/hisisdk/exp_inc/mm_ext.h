/*
  Copyright (c), 2001-2024, Shenshu Tech. Co., Ltd.
 */

#ifndef MM_EXT_H
#define MM_EXT_H

#include "ot_osal.h"
#include "ot_math.h"
#include "ot_common.h"
#include "mmz.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    const td_char *mmz_name;
    const td_char *buf_name;
    td_ulong size;
    td_bool kernel_only;
} mm_malloc_param;

td_phys_addr_t cmpi_mmz_malloc(const mm_malloc_param *param);
td_void cmpi_mmz_free(td_phys_addr_t phys_addr, td_void *virt_addr);
td_s32 cmpi_mmz_malloc_nocache(const mm_malloc_param *param, td_phys_addr_t *phys_addr, td_void **virt_addr);
td_s32 cmpi_mmz_malloc_cached(const mm_malloc_param *param, td_phys_addr_t *phys_addr, td_void **virt_addr);
td_void *cmpi_remap_cached(td_phys_addr_t phys_addr, td_ulong size);
td_void *cmpi_remap_nocache(td_phys_addr_t phys_addr, td_ulong size);
td_void cmpi_unmap(td_void *virt_addr);
td_s32 cmpi_check_mmz_phy_addr(td_phys_addr_t phys_addr, td_ulong len);
td_void cmpi_dcache_region_wb(td_void *virt_addr, td_phys_addr_t phys_addr, td_ulong len);
td_s32 cmpi_invalid_cache_byaddr(td_void *virt_addr, td_phys_addr_t phys_addr, td_ulong len);

td_void *cmpi_mmz_phys_to_handle(td_phys_addr_t phys_addr);

static inline td_bool read_user_linear_space_valid(td_u8 *addr_start, td_u32 len)
{
    td_u8 check;
    td_u8 *addr_end = TD_NULL;

    if (len == 0) {
        return TD_FALSE;
    }

    if (!osal_access_ok(OSAL_VERIFY_READ, addr_start, len)) {
        return TD_FALSE;
    }

    addr_end = addr_start + len - 1;
    if (osal_copy_from_user(&check, addr_end, 1)) {
        return TD_FALSE;
    }

    return TD_TRUE;
}

static inline td_bool write_user_linear_space_valid(td_u8 *addr_start, td_u32 len)
{
    td_u8 check = 0;
    td_u8 *addr_end = TD_NULL;

    if (len == 0) {
        return TD_FALSE;
    }

    if (!osal_access_ok(OSAL_VERIFY_WRITE, addr_start, len)) {
        return TD_FALSE;
    }

    addr_end = addr_start + len - 1;
    if (osal_copy_to_user(addr_end, &check, 1)) {
        return TD_FALSE;
    }

    return TD_TRUE;
}

#ifdef __cplusplus
}
#endif

#endif
