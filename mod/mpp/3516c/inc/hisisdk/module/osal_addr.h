/*
  Copyright (c), 2001-2024, Shenshu Tech. Co., Ltd.
 */

/**
 * @defgroup osal_addr osal_addr
 */
#ifndef __OSAL_ADDR_H__
#define __OSAL_ADDR_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define OSAL_GFP_ZERO (0x1)
#define OSAL_GFP_ATOMIC (0x1 << 1)
#define OSAL_GFP_DMA (0x1 << 2)
#define OSAL_GFP_KERNEL (0x1 << 3)

#define OSAL_VERIFY_READ 0
#define OSAL_VERIFY_WRITE 1

/**
 * @ingroup osal_addr
 * @brief Alloc dynamic memory.
 *
 * @par Description:
 * This API is used to alloc a memory block of which the size is specified.
 *
 * @param size [in] How many bytes of memory are required.
 * @param osal_gfp_flag [in] The type of memory to alloc. This parameter is not used in liteos and freertos.
 * In linux, it must include one of the following access modes: OSAL_GFP_ATOMIC, OSAL_GFP_DMA, OSAL_GFP_KERNEL.
 * OSAL_GFP_ZERO can be bitwise-or'd in flags, Then, the memory is set to zero.
 *
 * @par Support System:
 * linux liteos freertos.
 */
void *osal_kmalloc(unsigned long size, unsigned int osal_gfp_flag);

/**
 * @ingroup osal_addr
 * @brief alloc aligned dynamic memory.
 *
 * @par Description:
 * This API is used to alloc memory blocks of specified size and of which the starting addresses are aligned on
 * a specified boundary.
 *
 * @attention
 * The alignment parameter value must be a power of 2 with the minimum value being 4.
 *
 * @param size [in] Size of the memory block to be allocd (unit: byte).
 * @param osal_gfp_flag [in] The type of memory to alloc. This parameter is not used in liteos and freertos.
 * In linux, it must include one of the following access modes: OSAL_GFP_ATOMIC, OSAL_GFP_DMA, OSAL_GFP_KERNEL.
 * OSAL_GFP_ZERO can be bitwise-or'd in flags, Then, the memory is set to zero.
 * @param boundary [in] Boundary on which the memory is aligned. (unit: byte).
 *
 * @par Support System:
 * liteos, freertos.
 */
void *osal_kmalloc_align(unsigned int size, unsigned int osal_gfp_flag, unsigned int boundary);

/**
 * @ingroup osal_addr
 * @brief Free dynamic memory.
 *
 * @par Description:
 * This API is used to free specified dynamic memory that has been allocd and update module mem used.
 *
 * @param addr [in] Starting address of the memory block to be freed.
 *
 * @par Support System:
 * linux liteos freertos.
 */
void osal_kfree(void *addr);

/**
 * @ingroup osal_addr
 * @brief alloc virtually contiguous memory.
 *
 * @par Description:
 * This API is used to alloc memory space with consecutive virtual addresses.
 *
 * @param size [in] How many bytes of memory are required.
 *
 * @return Returns the pointer to the virtual memory.
 *
 * @par Support System:
 * linux liteos freertos.
 */
void *osal_vmalloc(unsigned long size);

/**
 * @ingroup osal_addr
 * @brief release memory allocd by vmalloc().
 *
 * @par Description:
 * This API is used to release memory allocd by osal_vmalloc().
 *
 * @param addr [in] Starting address of the memory block to be freed.
 *
 * @par Support System:
 * linux liteos freertos.
 */
void osal_vfree(void *addr);

/**
 * @ingroup osal_addr
 * @brief Initialize dynamic memory.
 *
 * @par Description:
 * This API is used to initialize the dynamic memory of a doubly linked list.
 *
 * @attention
 * The size parameter value should match the following two conditions:\n
 * 1) Be less than or equal to the Memory pool size;\n
 * 2) Be greater than the size of min_pool_size of each system.\n
 * Call this API when dynamic memory needs to be initialized during the startup of LiteOS.
 * The parameter input must be 4 or 8 byte-aligned.
 * The init area [pool, pool + size] should not conflict with other pools.
 * This function is defined only when LOSCFG_MEM_MUL_MODULE is defined in liteos.
 * This function is defined only when XLTCFG_SUPPORT_MEMMNG is defined in freertos.
 *
 * @param pool [in] Starting address of memory.
 * @param size [in] Memory size.
 *
 * @return OSAL_SUCCESS/OSAL_FAILURE
 *
 * @par Support System:
 * liteos, seliteos, freertos.
 */
int osal_pool_mem_init(void *pool, unsigned int size);

/**
 * @ingroup osal_addr
 * @brief alloc dynamic memory.
 *
 * @par Description:
 * This API is used to alloc a memory block of which the size is specified and update module mem used.
 *
 * @attention
 * The input pool parameter must be initialized via func osal_pool_mem_init.
 * The size of the input parameter size can not be greater than the memory pool size that specified at the second
 * input parameter of osal_pool_mem_init.
 * The size of the input parameter size must be four byte-aligned.
 * This function is defined only when LOSCFG_MEM_MUL_MODULE is defined in liteos.
 * This function is defined only when XLTCFG_SUPPORT_MEMMNG is defined in freertos.
 *
 * @param pool [in] Pointer to the memory pool that contains the memory block to be allocd.
 * @param size [in] Size of the memory block to be allocd (unit: byte).
 *
 * @retval NULL The memory fails to be allocd.
 * @retval VOID* The memory is successfully allocd, and the API returns the pointer to the allocd memory block.
 *
 * @par Support System:
 * liteos, seliteos, freertos.
 */
void *osal_pool_mem_alloc(void *pool, unsigned int size);

/**
 * @ingroup osal_addr
 * @brief alloc aligned memory.
 *
 * @par Description:
 * This API is used to alloc memory blocks of specified size and of which the starting addresses are aligned on
 * a specified boundary and update module mem used.
 *
 * @attention
 * The input pool parameter must be initialized via func osal_pool_mem_init.
 * The size of the input parameter size can not be greater than the memory pool size that specified at the second
 * input parameter of osal_pool_mem_init.
 * The alignment parameter value must be a power of 2 with the minimum value being 4.
 * This function is defined only when LOSCFG_MEM_MUL_MODULE is defined in liteos.
 * This function is defined only when XLTCFG_SUPPORT_MEMMNG is defined in freertos.
 *
 * @param pool [in] Pointer to the memory pool that contains the memory block to be allocd.
 * @param size [in] Size of the memory block to be allocd (unit: byte).
 *
 * @par Support System:
 * liteos, seliteos, freertos.
 */
void *osal_pool_mem_alloc_align(void *pool, unsigned int size, unsigned int boundary);

/**
 * @ingroup osal_addr
 * @brief Free dynamic memory.
 *
 * @par Description:
 * This API is used to free specified dynamic memory that has been allocd.
 *
 * @attention
 * The input pool parameter must be initialized via func osal_pool_mem_init.
 * The input addr parameter must be allocd by osal_pool_mem_alloc or osal_pool_mem_alloc_align.
 * This function is defined only when LOSCFG_MEM_MUL_MODULE is defined in liteos.
 * This function is defined only when XLTCFG_SUPPORT_MEMMNG is defined in freertos.
 *
 * @param  pool  [in] Pointer to the memory pool that contains the dynamic memory block to be freed.
 * @param  addr  [in] Starting address of the memory block to be freed.
 *
 * @par Support System:
 * liteos, seliteos, freertos.
 */
void osal_pool_mem_free(void *pool, const void *addr);

/**
 * @ingroup osal_addr
 * @brief Deinitialize dynamic memory.
 *
 * @par Description:
 * This API is used to deinitialize the dynamic memory of a doubly linked list.
 *
 * @attention
 * This function is defined only when LOSCFG_MEM_MUL_POOL is defined in liteos.
 * This function is defined only when XLTCFG_SUPPORT_MEMMNG and  XLTCFG_MEM_MUL_POOL are defined in freertos.
 *
 * @param pool [in] Starting address of memory.
 *
 * @retval OSAL_FAILURE The dynamic memory fails to be deinitialized.
 * @retval OSAL_SUCCESS The dynamic memory is successfully deinitialized.
 *
 * @par Support System:
 * liteos, seliteos, freertos.
 */
int osal_pool_mem_deinit(void *pool);

typedef enum {
    OSAL_BLOCKMEM_VALID = 0,
    OSAL_BLOCKMEM_INVALID_PHYADDR = 1,
    OSAL_BLOCKMEM_INVALID_SIZE = 2,
    OSAL_BLOCKMEM_MAX,
} osal_blockmem_status;

/**
 * @ingroup osal_addr
 * @brief Get the block mem status.
 *
 * @par Description:
 * Get the block mem status.
 *
 * @param phys_addr physical address
 * @param size Size of the address to be manipulated
 *
 * @retval OSAL_BLOCKMEM_VALID The block mem is valid.
 * @retval OSAL_BLOCKMEM_INVALID_PHYADDR The block mem is invalid.
 * @retval OSAL_BLOCKMEM_INVALID_SIZE The block mem is invalid.
 *
 * @par Support System:
 * linux.
 */
osal_blockmem_status osal_blockmem_get_status(unsigned long phyaddr, unsigned int size);

/**
 * @ingroup osal_addr
 * @brief Map bus memory into CPU space.
 *
 * @par Description:
 * This API is used to map the device whose memory type is device memory and does not use the cache.
 *
 * @param phys_addr [in] bus address of the memory
 * @param size [in] size of the resource to map
 *
 * @return Returns the virtual address mapped to
 *
 * @par Support System:
 * linux liteos.
 */
void *osal_ioremap(unsigned long phys_addr, unsigned long size);

/**
 * @ingroup osal_addr
 * @brief Map bus memory into CPU space.
 *
 * @par Description:
 * Map bus memory into CPU space.The functions of osal_ioremap_nocache are the same as those of osal_ioremap.
 * and reserved symbols for backward compatibility with drivers that use the osal_ioremap_nocache interface.
 *
 * @param phys_addr [in] bus address of the memory
 * @param size [in] size of the resource to map
 *
 * @attention Must be freed with iounmap.
 *
 * @return Returns the virtual address mapped to
 *
 * @par Support System:
 * linux liteos freertos.
 */
void *osal_ioremap_nocache(unsigned long phys_addr, unsigned long size);

/**
 * @ingroup osal_addr
 * @brief Map bus memory into CPU space.
 *
 * @par Description:
 * This API is used to map the device whose memory type is normal memory and use the cache.
 * This speeds up memory access and improves system performance.
 *
 * @param phys_addr [in] bus address of the memory
 * @param size [in] size of the resource to map
 *
 * @return Returns the virtual address mapped to
 *
 * @par Support System:
 * linux liteos.
 */
void *osal_ioremap_cached(unsigned long phys_addr, unsigned long size);

/**
 * @ingroup osal_addr
 * @brief Free a IO remapping.
 *
 * @param addr [in] virtual address from ioremap_*.
 *
 * @attention Caller must ensure there is only one unmapping for the same pointer.
 *
 * @par Support System:
 * linux liteos freertos.
 */
void osal_iounmap(void *addr, unsigned long size);

/**
 * @ingroup osal_addr
 * @brief map memory into CPU space write combined.
 *
 * @par Description:
 * map memory into CPU space write combined.
 *
 * @param phys_addr [in] bus address of the memory
 * @param size [in] size of the resource to map
 *
 * @attention Must be freed with iounmap.
 *
 * @return Returns the virtual address mapped to
 *
 * @par Support System:
 * linux liteos.
 */
void *osal_ioremap_wc(unsigned long phys_addr, unsigned long size);

/**
 * @ingroup osal_addr
 * @brief Change physical addresses to virtual addresses.
 *
 * @param addr physical address
 *
 * @return Returns the virtual address mapped to
 *
 * @par Support System:
 * linux liteos freertos.
 */
void *osal_phys_to_virt(unsigned long addr);

/**
 * @ingroup osal_addr
 * @brief Change virtual addresses to physical addresses.
 *
 * @par Support System:
 * linux liteos.
 */
unsigned long osal_virt_to_phys(const void *virt_addr);

/**
 * @ingroup osal_addr
 * @brief Maps @size from @phys_addr into contiguous kernel virtual space.
 *
 * @param phys_addr physical address
 * @param size Size of the address
 *
 * @return vaddr start address after vmap
 *
 * @attention
 * this function only support VM_MAP with PAGE_KERNEL flag.
 *
 * @par Support System:
 * linux.
 */
void *osal_blockmem_vmap(unsigned long phys_addr, unsigned long size);

/**
 * @ingroup osal_addr
 * @brief Convert to nocache format
 *
 * @param phys_addr physical address
 * @param size Size of the address
 *
 * @return vaddr (format:nocache)start address after vmap
 *
 * @par Support System:
 * linux.
 */
void *osal_blockmem_vmap_nocache(unsigned long phys_addr, unsigned long size);

/**
 * @ingroup osal_addr
 * @brief Free the virtually contiguous memory area.
 *
 * @par Description:
 * Free the virtually contiguous memory area starting at @virt_addr
 * which was created from the phys_addr passed to osal_vunmap().
 *
 * @param virt_addr virtual address
 *
 * @attention
 * Must not be called in interrupt context.
 *
 * @par Support System:
 * linux.
 */
void osal_blockmem_vunmap(const void *virt_addr);

/**
 * @ingroup osal_addr
 * @brief Free the reserved memory which has been defined in product.
 *
 * @par Description:
 * Free the reserved memory which has been defined in product.
 *
 * @param phys_addr physical address
 * @param size Size of the address to be manipulated
 *
 * @par Support System:
 * linux.
 */
void osal_blockmem_free(unsigned long phys_addr, unsigned long size);

/**
 * @ingroup osal_addr
 * @brief copy_from_user.
 *
 * @param to   [out] The kernel address to which the data is copied.
 * @param from [in] The user space address of the data to be copied.
 * @param n    [in] Length of data to be copied (unit: bytes).
 *
 * @return 0 is returned on success; otherwise, the number of bytes that are not copied is returned.
 *
 * @par Support System:
 * linux liteos freertos.
 */
unsigned long osal_copy_from_user(void *to, const void *from, unsigned long n);

/**
 * @ingroup osal_addr
 * @brief copy_to_user.
 *
 * @param to   [out] The user space address to which the data is copied.
 * @param from [in] The kernel address of the data to be copied.
 * @param n    [in] Length of data to be copied (unit: bytes).
 *
 * @return 0 is returned on success; otherwise, the number of bytes that are not copied is returned.
 *
 * @par Support System:
 * linux liteos freertos.
 */
unsigned long osal_copy_to_user(void *to, const void *from, unsigned long n);

/**
 * @ingroup osal_addr
 * @brief Check whether the user space memory block is available.
 *
 * @param tpye [in] Access type. 0:read, 1:write.
 * @param addr [in] A pointer variable to user space that points to the beginning of a memory block to be checked.
 * @param size [in] Size of the memory block to be checked.
 *
 * @retval true   user space memory block is available.
 * @retval false  user space memory block is unavailable.
 *
 * @par Support System:
 * linux liteos.
 */
int osal_access_ok(int type, const void *addr, unsigned long size);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
#endif /* __OSAL_ADDR_H__ */
