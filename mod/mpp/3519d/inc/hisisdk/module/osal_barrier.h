/*
  Copyright (c), 2001-2024, Shenshu Tech. Co., Ltd.
 */

/**
 * @defgroup osal_barrier osal_barrier
 */
#ifndef __OSAL_BARRIER_H__
#define __OSAL_BARRIER_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/**
 * @ingroup osal_barrier
 * @brief General basic CPU memory barriers
 *
 * @par Description:
 * The memory barrier mb() function ensures that any memory access that appears before the barrier is completed
 * before the execution of any memory access that appears after the barrier.
 *
 * @par Support System:
 * linux.
 */
void osal_mb(void);

/**
 * @ingroup osal_barrier
 * @brief Read basic CPU memory barriers
 *
 * @par Description:
 * The read memory barrier rmb() function ensures that any read that appears before the barrier is completed
 * before the execution of any read that appears after the barrier.
 *
 * @par Support System:
 * linux.
 */
void osal_rmb(void);

/**
 * @ingroup osal_barrier
 * @brief Write basic CPU memory barriers
 *
 * @par Description:
 * The write memory barrier wmb() function ensures that any write that appears before the barrier is completed
 * before the execution of any write that appears after the barrier.
 *
 * @par Support System:
 * linux.
 */
void osal_wmb(void);

/**
 * @ingroup osal_barrier
 * @brief General basic CPU memory barriers, SMP conditional.
 *
 * @par Description:
 * Corresponding SMP versions of osal_mb.
 *
 * @par Support System:
 * linux.
 */
void osal_smp_mb(void);

/**
 * @ingroup osal_barrier
 * @brief Read basic CPU memory barriers, SMP conditional.
 *
 * @par Description:
 * Corresponding SMP versions of osal_rmb.
 *
 * @par Support System:
 * linux.
 */
void osal_smp_rmb(void);

/**
 * @ingroup osal_barrier
 * @brief Write basic CPU memory barriers, SMP conditional.
 *
 * @par Description:
 * Corresponding SMP versions of osal_wmb.
 *
 * @par Support System:
 * linux.
 */
void osal_smp_wmb(void);

/**
 * @ingroup osal_barrier
 * @brief Instruction Synchronization Barrier.
 *
 * @par Description:
 * Instruction Synchronization Barrier flushes the pipeline in the processor, so that all instructions following
 * the ISB are fetched from cache or memory, after the instruction has been completed. It ensures that the effects
 * of context altering operations, such as changing the ASID, or completed TLB maintenance operations,
 * or branch predictor maintenance operations, as well as all changes to the CP15 registers,
 * executed before the ISB instruction are visible to the instructions fetched after the ISB.
 * In addition, the ISB instruction ensures that any branches that appear in program order after it
 * are always written into the branch prediction logic with the context that is visible after the ISB instruction.
 * This is required to ensure correct execution of the instruction stream.
 *
 * @par Support System:
 * linux liteos.
 */
void osal_isb(void);

/**
 * @ingroup osal_barrier
 * @brief Data Synchronization Barrier.
 *
 * @par Description:
 * Data Synchronization Barrier acts as a special kind of memory barrier. No instruction in program order after
 * this instruction executes until this instruction completes. This instruction completes when:
 * All explicit memory accesses before this instruction complete.
 * All Cache, Branch predictor and TLB maintenance operations before this instruction complete.
 *
 * @par Support System:
 * linux liteos.
 */
void osal_dsb(void);

/**
 * @ingroup osal_barrier
 * @brief Data Memory Barrier.
 *
 * @par Description:
 * Data Memory Barrier acts as a memory barrier. It ensures that all explicit memory accesses that appear in program
 * order before the DMB instruction are observed before any explicit memory accesses that appear in program order
 * after the DMB instruction. It does not affect the ordering of any other instructions executing on the processor.
 *
 * @par Support System:
 * linux liteos.
 */
void osal_dmb(void);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
#endif /* __OSAL_BARRIER_H__ */
