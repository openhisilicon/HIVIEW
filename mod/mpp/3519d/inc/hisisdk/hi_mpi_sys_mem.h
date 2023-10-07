/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */

#ifndef HI_MPI_SYS_MEM_H
#define HI_MPI_SYS_MEM_H

#include "hi_type.h"
#include "hi_common_sys_mem.h"

#ifdef __cplusplus
extern "C" {
#endif

hi_void *hi_mpi_sys_mmap(hi_phys_addr_t phys_addr, hi_u32 size);

hi_void *hi_mpi_sys_mmap_cached(hi_phys_addr_t phys_addr, hi_u32 size);

hi_s32 hi_mpi_sys_munmap(const hi_void *virt_addr, hi_u32 size);

/* alloc mmz memory in user context */
hi_s32 hi_mpi_sys_mmz_alloc(hi_phys_addr_t *phys_addr, hi_void **virt_addr,
    const hi_char *mmb, const hi_char *zone, hi_u32 len);

/* alloc mmz memory with cache in user context */
hi_s32 hi_mpi_sys_mmz_alloc_cached(hi_phys_addr_t *phys_addr, hi_void **virt_addr,
    const hi_char *mmb, const hi_char *zone, hi_u32 len);

/* free mmz memory in user context */
hi_s32 hi_mpi_sys_mmz_free(hi_phys_addr_t phys_addr, const hi_void *virt_addr);

/* flush cache */
hi_s32 hi_mpi_sys_flush_cache(hi_phys_addr_t phys_addr, hi_void *virt_addr, hi_u32 size);

/* get virtual meminfo according to virtual addr, should be in one process */
hi_s32 hi_mpi_sys_get_virt_mem_info(const hi_void *virt_addr, hi_sys_virt_mem_info *mem_info);

hi_s32 hi_mpi_sys_mem_share(const hi_void *mem_handle, hi_s32 pid);
hi_s32 hi_mpi_sys_mem_unshare(const hi_void *mem_handle, hi_s32 pid);
hi_s32 hi_mpi_sys_mem_share_all(const hi_void *mem_handle);
hi_s32 hi_mpi_sys_mem_unshare_all(const hi_void *mem_handle);

hi_s32 hi_mpi_sys_get_mem_info_by_virt(const hi_void *virt_addr, hi_sys_mem_info *mem_info);
hi_s32 hi_mpi_sys_get_mem_info_by_phys(hi_phys_addr_t phys_addr, hi_sys_mem_info *mem_info);
hi_s32 hi_mpi_sys_get_mem_info_by_handle(const hi_void *mem_handle, hi_sys_mem_info *mem_info);

#ifdef __cplusplus
}
#endif

#endif /* HI_MPI_SYS_MEM_H */
