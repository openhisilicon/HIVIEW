/*
  Copyright (c), 2001-2024, Shenshu Tech. Co., Ltd.
 */

#ifndef MMZ_H
#define MMZ_H

#include "ot_osal.h"

#define CACHE_LINE_SIZE     0x40

#define OT_MMZ_NAME_LEN     32
#define OT_MMB_NAME_LEN     32

#define OT_MMB_MAX_PID_NUM  5

struct ot_media_memory_zone {
    char name[OT_MMZ_NAME_LEN];

    unsigned long gfp;

    unsigned long phys_start;
    unsigned long nbytes;

    struct osal_list_head list;
    union {
        struct device *cma_dev;
        unsigned char *bitmap;
    };
    struct osal_list_head mmb_list;

    unsigned int alloc_type;
    unsigned long block_align;

    void (*destructor)(const void *);
};
typedef struct ot_media_memory_zone ot_mmz_t;

#define OT_MMZ_FMT_S        "PHYS(0x%08lX, 0x%08lX), GFP=%lu, nBYTES=%luKB,    NAME=\"%s\""
#define ot_mmz_fmt_arg(p)   (p)->phys_start, (p)->phys_start + (p)->nbytes - 1, (p)->gfp, (p)->nbytes / SZ_1K, (p)->name

struct ot_media_memory_block {
    char name[OT_MMB_NAME_LEN];
    struct ot_media_memory_zone *zone;
    struct osal_list_head list;

    unsigned long phys_addr;
    void *kvirt;
    unsigned long length;

    unsigned long flags;

    unsigned int order;

    int phy_ref;
    int map_ref;

    int kernel_only;
    int check_pid;
    int pid_num;
    int pid[OT_MMB_MAX_PID_NUM];
};
typedef struct ot_media_memory_block ot_mmb_t;

#define ot_mmb_kvirt(p)     ({ot_mmb_t *__mmb=(p); OSAL_BUG_ON(__mmb == NULL); __mmb->kvirt;})
#define ot_mmb_phys(p)      ({ot_mmb_t *__mmb=(p); OSAL_BUG_ON(__mmb == NULL); __mmb->phys_addr;})
#define ot_mmb_length(p)    ({ot_mmb_t *__mmb=(p); OSAL_BUG_ON(__mmb == NULL); __mmb->length;})
#define ot_mmb_name(p)      ({ot_mmb_t *__mmb=(p); OSAL_BUG_ON(__mmb == NULL); __mmb->name;})
#define ot_mmb_zone(p)      ({ot_mmb_t *__mmb=(p); OSAL_BUG_ON(__mmb == NULL); __mmb->zone;})

#define OT_MMB_MAP2KERN         (1 << 0)
#define OT_MMB_MAP2KERN_CACHED  (1 << 1)
#define OT_MMB_RELEASED         (1 << 2)

#ifdef CONFIG_64BIT
#define OT_MMB_FMT_S \
    "phys(0x%08lX, 0x%08lX), kvirt=0x%016lX, flags=0x%lX, kernel_only=%d, share_all=%d pid[%d,%d,%d,%d,%d], " \
    "length=%luKB,    name=\"%s\""
#else
#define OT_MMB_FMT_S \
    "phys(0x%08lX, 0x%08lX), kvirt=0x%08lX, flags=0x%lX, kernel_only=%d, share_all=%d pid[%d,%d,%d,%d,%d], " \
    "length=%luKB,    name=\"%s\""
#endif
#define ot_mmb_fmt_arg(p)   (p)->phys_addr, mmz_grain_align((p)->phys_addr + (p)->length) - 1, \
    (unsigned long)(uintptr_t)((p)->kvirt), (p)->flags, (p)->kernel_only, !((p)->check_pid), \
    (p)->pid[0], (p)->pid[1], (p)->pid[2], (p)->pid[3], (p)->pid[4], (p)->length / SZ_1K, (p)->name

#if defined(KERNEL_BIT_64) && defined(USER_BIT_32)
#define __phys_addr_type__         unsigned long long
#define __phys_len_type__          unsigned long long
#define __phys_addr_align__        __attribute__((aligned(8)))
#else
#define __phys_addr_type__         unsigned long
#define __phys_len_type__          unsigned long
#define __phys_addr_align__        __attribute__((aligned(sizeof(long))))
#endif

struct mmb_info {
    __phys_addr_type__ phys_addr; /* phys-memory address */
    __phys_addr_type__ __phys_addr_align__ align; /* if you need your phys-memory have special align size */
    __phys_len_type__ __phys_addr_align__ size; /* length of memory you need, in bytes */
    unsigned int __phys_addr_align__ order;

    void *__phys_addr_align__ mapped; /* userspace mapped ptr */

    union {
        struct {
            unsigned long prot : 8; /* PROT_READ or PROT_WRITE */
            unsigned long flags : 12; /* MAP_SHARED or MAP_PRIVATE */

#ifdef __KERNEL__
            unsigned long reserved : 8; /* reserved, do not use */
            unsigned long delayed_free : 1;
            unsigned long map_cached : 1;
#endif
        };
        unsigned long w32_stuf;
    } __phys_addr_align__;

    char mmb_name[OT_MMB_NAME_LEN];
    char mmz_name[OT_MMZ_NAME_LEN];
    unsigned long __phys_addr_align__ gfp; /* reserved, do set to 0 */
} __attribute__((aligned(8)));

struct mmb_share_info {
    unsigned long phys_addr;
    void *virt_addr;
    void *mem_handle;
    unsigned long offset;
    int shared_pid;
};

struct dirty_area {
    __phys_addr_type__ dirty_phys_start; /* dirty physical address */
    void *__phys_addr_align__ dirty_virt_start; /* dirty virtual address, must be coherent with dirty_phys_addr */
    __phys_len_type__ __phys_addr_align__ dirty_size;
} __phys_addr_align__;

#define MMZ_MMB_IOC_MAGIC               'm'
#define MMZ_MMB_DCACHE_DIRTY_IOC_MAGIC  'd'
#define MMZ_MMB_SHARE_IOC_MAGIC         's'

typedef enum {
    OT_MMZ_IOC_NR_MMB_ALLOC = 10,
    OT_MMZ_IOC_NR_MMB_ATTR,
    OT_MMZ_IOC_NR_MMB_FREE,

    OT_MMZ_IOC_NR_MMB_USER_REMAP = 20,
    OT_MMZ_IOC_NR_MMB_USER_REMAP_CACHED,
    OT_MMZ_IOC_NR_MMB_USER_UNMAP,
    OT_MMZ_IOC_NR_MMB_VIRT_GET_PHYS,
    OT_MMZ_IOC_NR_MMB_SYS_FLUSH_CACHE,
    OT_MMZ_IOC_NR_MMB_BASE_CHECK_ADDR,

    OT_MMZ_IOC_NR_MMB_MEM_SHARE = 40,
    OT_MMZ_IOC_NR_MMB_MEM_UNSHARE,
    OT_MMZ_IOC_NR_MMB_MEM_SHARE_ALL,
    OT_MMZ_IOC_NR_MMB_MEM_UNSHARE_ALL,
    OT_MMZ_IOC_NR_MMB_VIRT_GET_SYS_MEM,
    OT_MMZ_IOC_NR_MMB_PHYS_GET_SYS_MEM,
    OT_MMZ_IOC_NR_MMB_HANDLE_GET_SYS_MEM,

    OT_MMZ_IOC_NR_MMB_FLUSH_DCACHE_DIRTY = 50,

    OT_MMZ_IOC_NR_BUTT
} ot_mmz_ioc_nr;

#define IOC_MMB_ALLOC              _IOWR(MMZ_MMB_IOC_MAGIC, OT_MMZ_IOC_NR_MMB_ALLOC, struct mmb_info)
#define IOC_MMB_ATTR               _IOR(MMZ_MMB_IOC_MAGIC, OT_MMZ_IOC_NR_MMB_ATTR, struct mmb_info)
#define IOC_MMB_FREE               _IOW(MMZ_MMB_IOC_MAGIC, OT_MMZ_IOC_NR_MMB_FREE, struct mmb_info)

#define IOC_MMB_USER_REMAP         _IOWR(MMZ_MMB_IOC_MAGIC, OT_MMZ_IOC_NR_MMB_USER_REMAP, struct mmb_info)
#define IOC_MMB_USER_REMAP_CACHED  _IOWR(MMZ_MMB_IOC_MAGIC, OT_MMZ_IOC_NR_MMB_USER_REMAP_CACHED, struct mmb_info)
#define IOC_MMB_USER_UNMAP         _IOWR(MMZ_MMB_IOC_MAGIC, OT_MMZ_IOC_NR_MMB_USER_UNMAP, struct mmb_info)
#define IOC_MMB_VIRT_GET_PHYS      _IOWR(MMZ_MMB_IOC_MAGIC, OT_MMZ_IOC_NR_MMB_VIRT_GET_PHYS, struct mmb_info)
#define IOC_MMB_SYS_FLUSH_CACHE    _IOW(MMZ_MMB_IOC_MAGIC, OT_MMZ_IOC_NR_MMB_SYS_FLUSH_CACHE, struct mmb_info)
#define IOC_MMB_BASE_CHECK_ADDR    _IOW(MMZ_MMB_IOC_MAGIC, OT_MMZ_IOC_NR_MMB_BASE_CHECK_ADDR, struct mmb_info)

#define IOC_MMB_MEM_SHARE          _IOW(MMZ_MMB_SHARE_IOC_MAGIC, OT_MMZ_IOC_NR_MMB_MEM_SHARE, \
                                        struct mmb_share_info)
#define IOC_MMB_MEM_UNSHARE        _IOW(MMZ_MMB_SHARE_IOC_MAGIC, OT_MMZ_IOC_NR_MMB_MEM_UNSHARE, \
                                        struct mmb_share_info)
#define IOC_MMB_MEM_SHARE_ALL      _IOW(MMZ_MMB_SHARE_IOC_MAGIC, OT_MMZ_IOC_NR_MMB_MEM_SHARE_ALL, \
                                        struct mmb_share_info)
#define IOC_MMB_MEM_UNSHARE_ALL    _IOW(MMZ_MMB_SHARE_IOC_MAGIC, OT_MMZ_IOC_NR_MMB_MEM_UNSHARE_ALL, \
                                        struct mmb_share_info)
#define IOC_MMB_VIRT_GET_SYS_MEM   _IOWR(MMZ_MMB_SHARE_IOC_MAGIC, OT_MMZ_IOC_NR_MMB_VIRT_GET_SYS_MEM, \
                                         struct mmb_share_info)
#define IOC_MMB_PHYS_GET_SYS_MEM   _IOWR(MMZ_MMB_SHARE_IOC_MAGIC, OT_MMZ_IOC_NR_MMB_PHYS_GET_SYS_MEM, \
                                         struct mmb_share_info)
#define IOC_MMB_HANDLE_GET_SYS_MEM _IOWR(MMZ_MMB_SHARE_IOC_MAGIC, OT_MMZ_IOC_NR_MMB_HANDLE_GET_SYS_MEM, \
                                         struct mmb_share_info)

#define IOC_MMB_FLUSH_DCACHE_DIRTY _IOW(MMZ_MMB_DCACHE_DIRTY_IOC_MAGIC, OT_MMZ_IOC_NR_MMB_FLUSH_DCACHE_DIRTY, \
                                        struct dirty_area)

#define MMZ_SETUP_CMDLINE_LEN      256
typedef struct {
    char mmz[MMZ_SETUP_CMDLINE_LEN];
    char map_mmz[MMZ_SETUP_CMDLINE_LEN];
    int anony;
} mmz_module_params_s;

typedef struct {
    const char *mmz_name;
    const char *buf_name;
    unsigned long size;
    unsigned long align;
    unsigned long gfp;
    int kernel_only;
} ot_mmz_alloc_para_in;

/* APIs */
ot_mmz_t *ot_mmz_create(const char *name, unsigned long gfp, unsigned long phys_start, unsigned long nbytes);
int ot_mmz_destroy(const ot_mmz_t *zone);
ot_mmz_t *ot_mmz_find(unsigned long gfp, const char *mmz_name);
unsigned long ot_mmz_get_phys(const char *zone_name);

ot_mmb_t *ot_mmb_alloc(const ot_mmz_alloc_para_in *para_in);
int ot_mmb_free(ot_mmb_t *mmb);
ot_mmb_t *ot_mmb_getby_phys(unsigned long addr);
ot_mmb_t *ot_mmb_getby_phys_2(unsigned long addr, unsigned long *out_offset);
ot_mmb_t *ot_mmb_getby_kvirt(const void *virt);
#define ot_mmb_freeby_phys(phys_addr)   ot_mmb_free(ot_mmb_getby_phys(phys_addr))
#define ot_mmb_freeby_kvirt(kvirt)      ot_mmb_free(ot_mmb_getby_kvirt(kvirt))

int ot_is_phys_in_mmz(unsigned long addr_start, unsigned long addr_len);
int ot_map_mmz_check_phys(unsigned long addr_start, unsigned long addr_len);

int ot_mmb_check_mem_share(const ot_mmb_t *mmb);
int ot_mmb_check_mem_share_with_pid(const ot_mmb_t *mmb, int pid);
int ot_mmz_check_phys_addr(unsigned long phys_addr, unsigned long size);
int ot_mmz_get_mem_process_isolation(void);

void *ot_mmb_map2kern(ot_mmb_t *mmb);
void *ot_mmb_map2kern_cached(ot_mmb_t *mmb);
int ot_mmb_unmap(ot_mmb_t *mmb);

void *ot_mmf_map2kern_nocache(unsigned long phys, int len);
void *ot_mmf_map2kern_cache(unsigned long phys, int len);
void ot_mmf_unmap(void *virt);

unsigned long usr_virt_to_phys(unsigned long virt);
int ot_vma_check(unsigned long vm_start, unsigned long vm_end);

int ot_mmb_flush_dcache_byaddr(void *kvirt, unsigned long phys_addr, unsigned long length);
int ot_mmb_flush_dcache_byaddr_safe(void *kvirt, unsigned long phys_addr, unsigned long length);
int ot_mmb_invalid_cache_byaddr(void *kvirt, unsigned long phys_addr, unsigned long length);

#endif  /* MMZ_H */
