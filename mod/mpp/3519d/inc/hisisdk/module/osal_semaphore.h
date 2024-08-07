/*
  Copyright (c), 2001-2024, Shenshu Tech. Co., Ltd.
 */

/**
 * @defgroup osal_semaphore osal_semaphore
 */
#ifndef __OSAL_SEMAPHORE_H__
#define __OSAL_SEMAPHORE_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define OSAL_SEM_WAIT_FOREVER (-1)

typedef struct {
    void *sem;
} osal_semaphore;

/**
 * @ingroup osal_semaphore
 * @brief Create a semaphore.
 *
 * @par Description:
 * This API is used to create a semaphore control structure
 * according to the initial number of available semaphores specified by count
 *
 * @param val [in]  Initial number of available semaphores.
 * @param sem [out] Pointer of semaphore control structure that is initialized.
 *
 * @return OSAL_SUCCESS/OSAL_FAILURE
 *
 * @par Support System:
 * linux liteos freertos.
 */
int osal_sem_init(osal_semaphore *sem, int val);

/**
 * @ingroup osal_semaphore
 * @brief Create a binary semaphore.
 *
 * @par Description:
 * This API is used to create a binary semaphore control structure
 * according to the initial number of available semaphores specified by count
 *
 * @param val [in]  Initial number of available semaphores. The value range is [0, 1].
 * @param sem [out] Pointer of semaphore control structure that is initialized.
 *
 * @return OSAL_SUCCESS/OSAL_FAILURE
 *
 * @par Support System:
 * liteos freertos.
 */
int osal_sem_binary_sem_init(osal_semaphore *sem, int val);

/**
 * @ingroup osal_semaphore
 * @brief Request a semaphore.
 *
 * @par Description:
 * This API is used to attempt to obtain the semaphore. Acquires the semaphore.
 * If no more tasks are allowed to acquire the semaphore,
 * calling this function will put the task to sleep until the semaphore is released.
 *
 * @attention
 * Do not pend sem during an interrupt.
 * Do not pend sem in a system task, such as idle or swtmr task.
 * The specified sem id must be created first.
 * Do not recommend to use this API in software timer callback.
 *
 * @param sem [in] The semaphore to be acquired.
 *
 * @return OSAL_SUCCESS/OSAL_FAILURE
 *
 * @par Support System:
 * linux liteos freertos.
 */
int osal_sem_down(osal_semaphore *sem);

/**
 * @ingroup osal_semaphore
 * @brief Acquire the semaphore within a specified time that specifies the timeout period.
 *
 * @par Description:
 * Acquire the semaphore within a specified time that specifies the timeout period.
 *
 * @attention
 * Do not pend sem during an interrupt.
 * Do not pend sem in a system task, such as idle or swtmr task.
 * The specified sem id must be created first.
 * Do not recommend to use this API in software timer callback.
 *
 * @param sem [in] The semaphore to be acquired.
 * @param timeout [in] How long to wait before failing. (unit: ms)
 *
 * @return OSAL_SUCCESS/OSAL_FAILURE
 *
 * @par Support System:
 * liteos freertos.
 */
int osal_sem_down_timeout(osal_semaphore *sem, unsigned int timeout);

/**
 * @ingroup osal_semaphore
 * @brief acquire the semaphore unless interrupted.
 *
 * @par Description:
 * Acquire the semaphore unless interrupted. Attempts to acquire the semaphore.
 * If no more tasks are allowed to acquire the semaphore, calling this function will put the task to sleep.
 *
 * @param sem [in] The semaphore to be acquired.
 *
 * @return
 * If the sleep is interrupted by a signal, this function will return OSAL_EINTR.
 * If the semaphore is successfully acquired, this function returns OSAL_SUCCESS, failed return OSAL_FAILURE
 *
 * @par Support System:
 * linux liteos freertos.
 */
int osal_sem_down_interruptible(osal_semaphore *sem);

/**
 * @ingroup osal_semaphore
 * @brief try to acquire the semaphore, without waiting.
 *
 * @par Description:
 * try to acquire the semaphore, without waiting.
 *
 * @attention
 * This return value is inverted from both spin_trylock and mutex_trylock! Be careful about this when converting code.
 * Unlike mutex_trylock, this function can be used from interrupt context,
 * and the semaphore can be released by any task or interrupt.
 *
 * @param sem [in] The semaphore to be acquired.
 *
 * @return
 * If the semaphore cannot be obtained, the system returns 1 instead of sleeping.
 * If the value 0 is returned, the semaphore is obtained.
 *
 * @par Support System:
 * linux liteos freertos.
 */
int osal_sem_trydown(osal_semaphore *sem);

/**
 * @ingroup osal_semaphore
 * @brief Release a semaphore.
 *
 * @par Description:
 * Release a semaphore.
 *
 * @attention
 * Unlike mutexes, osal_sem_up() may be called from any context and
 * even by tasks which have never called osal_sem_down().
 *
 * @param sem [in] The semaphore to release.
 *
 * @par Support System:
 * linux liteos freertos.
 */
void osal_sem_up(osal_semaphore *sem);

/**
 * @ingroup osal_semaphore
 * @brief Delete a semaphore.
 *
 * @par Description:
 * This API is used to destroy semaphore and release space.
 *
 * @attention
 * The specified sem must be created first before deleting it.
 *
 * @param sem [in] The semaphore to destroyed.
 *
 * @par Support System:
 * linux liteos freertos.
 */
void osal_sem_destroy(osal_semaphore *sem);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
#endif /* __OSAL_SEMAPHORE_H__ */