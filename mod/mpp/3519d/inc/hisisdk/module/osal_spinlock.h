/*
  Copyright (c), 2001-2024, Shenshu Tech. Co., Ltd.
 */

/**
 * @defgroup osal_spinlock osal_spinlock
 */
#ifndef __OSAL_SPINLOCK_H__
#define __OSAL_SPINLOCK_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

typedef struct {
    void *lock;
} osal_spinlock;

/**
 * @ingroup osal_spinlock
 * @brief Initialize a spin lock.
 *
 * @par Description:
 * This API is used to initialization of spin_lock.
 *
 * @param lock [out] the lock to be initialized.
 *
 * @attention
 * must be free with osal_spin_lock_destroy, other wise will lead to memory leak;
 *
 * @return OSAL_SUCCESS/OSAL_FAILURE
 *
 * @par Support System:
 * linux liteos.
 */
int osal_spin_lock_init(osal_spinlock *lock);

/**
 * @ingroup osal_spinlock
 * @brief Lock the spinlock.
 *
 * @par Description:
 * This API is used to lock the spinlock. If the spinlock has been obtained by another thread,
 * the thread will wait cyclically until it can lock the spinlock successfully.
 *
 * @attention
 * The spinlock must be initialized before it is used. It should be initialized by osal_spin_lock_init.
 * A spinlock can not be locked for multiple times in a task. Otherwise, a deadlock occurs.
 * If the spinlock will be used in both task and interrupt, using osal_spin_lock_irqsave instead of this API.
 *
 * @param lock [in] The lock to be acquired, Initialized by osal_spin_lock_init.
 *
 * @par Support System:
 * linux liteos.
 */
void osal_spin_lock(osal_spinlock *lock);

/**
 * @ingroup osal_spinlock
 * @brief Disable soft interrupts and lock the spin lock.
 *
 * @par Description:
 * Disable soft interrupts and lock the spin lock on linux. Same as osal_spin_lock, but it disables soft interrupts.
 * Disables scheduling on LiteOS and Freertos.
 *
 * @param lock [in] The lock to be acquired, Initialized by osal_spin_lock_init.
 *
 * @par Support System:
 * linux liteos freertos.
 */
void osal_spin_lock_bh(osal_spinlock *lock);

/**
 * @ingroup osal_spinlock
 * @brief Try to acquire the spin_lock.
 *
 * @par Description:
 * try to acquire the spin_lock.
 *
 * @param lock [in] the lock to be acquired.
 *
 * @return Returns true if the lock can be obtained immediately, otherwise returns false immediately.
 *
 * @par Support System:
 * linux liteos.
 */
int osal_spin_trylock(osal_spinlock *lock);

/**
 * @ingroup osal_spinlock
 * @brief Try to acquire the spin_lock.
 *
 * @par Description:
 * Try to acquire the spin_lock and disables the CPU interrupt.
 *
 * @param lock [in] the lock to be acquired.
 *
 * @return Returns true if the lock can be obtained immediately, otherwise returns false immediately.
 *
 * @par Support System:
 * linux.
 */
int osal_spin_trylock_irq(osal_spinlock *lock);

/**
 * @ingroup osal_spinlock
 * @brief Try to acquire the spin_lock.
 *
 * @par Description:
 * Saves the current IRQ status of the CPU, try to acquire the spin_lock, and disables the interrupts of the CPU.
 *
 * @param lock [in] the lock to be acquired.
 *
 * @par Support System:
 * linux.
 */
void osal_spin_trylock_irqsave(osal_spinlock *lock, unsigned long *flags);

/**
 * @ingroup osal_spinlock
 * @brief release the spin_lock.
 *
 * @par Description:
 * release the spin_lock.
 *
 * @param lock [in] the lock to be released.
 *
 * @par Support System:
 * linux liteos.
 */
void osal_spin_unlock(osal_spinlock *lock);

/**
 * @ingroup osal_spinlock
 * @brief release the spin_lock.
 *
 * @par Description:
 * Release the spin_lock and enables the interrupts of the CPU on linux.
 * Resume scheduling on LiteOS and Freertos.
 *
 * @par Support System:
 * linux liteos freertos.
 */
void osal_spin_unlock_bh(osal_spinlock *lock);

/**
 * @ingroup osal_spinlock
 * @brief acquire the spin_lock.
 *
 * @par Description:
 * Saves the current IRQ status of the CPU, obtains the specified spin_lock, and disables the interrupts of the CPU.
 *
 * @param lock [in] the lock to be acquired.
 *
 * @par Support System:
 * linux liteos freertos.
 */
void osal_spin_lock_irqsave(osal_spinlock *lock, unsigned long *flags);

/**
 * @ingroup osal_spinlock
 * @brief release the spin_lock.
 *
 * @par Description:
 * Releases the specified spin_lock and restores the interrupt status of the CPU, and enables the interrupts of the CPU.
 *
 * @param lock [in] the lock to be released.
 *
 * @par Support System:
 * linux liteos freertos.
 */
void osal_spin_unlock_irqrestore(osal_spinlock *lock, unsigned long *flags);

/**
 * @ingroup osal_spinlock
 * @brief Destroy the spin_lock.
 *
 * @par Description:
 * Destroy the spin_lock.
 *
 * @param lock [in] The lock to be destroyed.
 *
 * @attention
 * must be called when kmod exit, other wise will lead to memory leak;
 * this API will free memory, lock must be from osal_spin_lock_init returns
 *
 * @par Support System:
 * linux liteos.
 */
void osal_spin_lock_destroy(osal_spinlock *lock);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
#endif /* __OSAL_SPINLOCK_H__ */