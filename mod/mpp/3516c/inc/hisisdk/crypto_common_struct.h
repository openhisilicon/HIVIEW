/**
  Copyright (c), 2001-2024, Shenshu Tech. Co., Ltd.
 */

#ifndef CRYPTO_COMMON_STRUCT_H
#define CRYPTO_COMMON_STRUCT_H

#include "crypto_type.h"

/* ! \union of compat addr */
typedef union {
    void *p;                /* !<  virtual address */
    unsigned long long phy; /* !<  physical address */
    unsigned int word[2];   /* !<  2 word of address */
} crypto_compat_addr;

typedef enum {
    CRYPTO_BUF_SECURE,
    CRYPTO_BUF_NONSECURE,
} crypto_buffer_secure;

typedef struct {
    unsigned long long uapi_mem_handle;     /* Handle to buffer header address */
    unsigned long long addr_offset;         /* buffer offset, unused. */
    td_void *kapi_mem_handle;

    unsigned long long phys_addr;
    void *virt_addr;
    crypto_buffer_secure buf_sec;           /* NONSECURE or SECURE */
} crypto_buf_attr;

typedef enum {
    CPU_ID_AIDSP = 0x00000035,
    CPU_ID_PCPU = 0x0000006a,
    CPU_ID_SCPU = 0x000000a5,
    CPU_ID_ACPU = 0x000000aa,
    CPU_ID_INVALID = 0xffffffff
} ca_misc_cpu_id;

typedef enum {
    CRYPTO_MEM_TYPE_MMZ,
    CRYPTO_MEM_TYPE_SMMU,
} crypto_mem_type;

typedef enum {
    CRYPTO_CPU_TYPE_SCPU,
    CRYPTO_CPU_TYPE_ACPU,
    CRYPTO_CPU_TYPE_HPPCPU,
    CRYPTO_CPU_TYPE_PCPU,
    CRYPTO_CPU_TYPE_AIDSP,
    CRYPTO_CPU_TYPE_INVALID
} crypto_cpu_type;

typedef enum {
    IN_NODE_TYPE_FIRST = 1 << 0,
    IN_NODE_TYPE_NORMAL = 1 << 1,
    IN_NODE_TYPE_LAST = 1 << 2,
    /* CCM. */
    IN_NODE_TYPE_CCM_N = 1 << 3,
    IN_NODE_TYPE_CCM_AAD = 1 << 4,
    IN_NODE_TYPE_CCM_P = 1 << 5,
    IN_NODE_TYPE_CCM_LAST = 1 << 6,
    /* GCM. */
    IN_NODE_TYPE_GCM_FIRST = 1 << 7,
    IN_NODE_TYPE_GCM_A = 1 << 8,
    IN_NODE_TYPE_GCM_P = 1 << 9,
    IN_NODE_TYPE_GCM_LEN = 1 << 10,
    IN_NODE_TYPE_GCM_GHASH = 1 << 11,
    IN_NODE_TYPE_GCM_IV = 1 << 12,
} in_node_type_e;

typedef void *(*func_malloc)(unsigned int size);
typedef void (*func_free)(const void *ptr);
typedef void (*func_udelay)(unsigned int us);
typedef void *(*func_malloc_coherent)(unsigned int size, crypto_mem_type mem_type, const td_char *name);
typedef void (*func_free_coherent)(void *ptr);
typedef td_u64 (*func_get_phys_address)(const void *ptr);
typedef void *(*func_get_virt_addr)(void *ptr);
typedef td_bool (*func_get_smmu_table_addr)(td_u64 *table, td_u64 *rdaddr, td_u64 *wraddr);
typedef void *(*func_register_coherent_mem)(void *virt_addr, td_u64 phys_addr, unsigned int size);
typedef void (*func_unregister_coherent_mem)(void *virt_addr);
typedef crypto_cpu_type (*func_get_cpu_type)(void);
typedef td_u32 (*func_get_pke_rom_lib_start_addr)(td_void);

typedef struct {
    func_malloc malloc;
    func_free free;
    func_udelay udelay;
} crypto_romable_drv_func;

typedef struct {
    func_malloc_coherent malloc_coherent;
    func_free_coherent free_coherent;
    func_get_phys_address get_phys_addr;
    func_get_virt_addr get_virt_addr;
    func_get_smmu_table_addr get_smmu_table_addr;
    func_register_coherent_mem register_coherent_mem;
    func_unregister_coherent_mem unregister_coherent_mem;
    func_get_cpu_type get_cpu_type;
    func_get_pke_rom_lib_start_addr get_pke_rom_lib_start_addr;
} crypto_drv_func;

typedef td_bool (*drv_wait_condition_func)(const td_void *param);
typedef td_s32 (*crypto_wait_timeout_interruptible)(const td_void *wait, drv_wait_condition_func func,
    const td_void *param, const td_u32 timeout_ms);

#endif