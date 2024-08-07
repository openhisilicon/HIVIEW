/*
  Copyright (c), 2001-2024, Shenshu Tech. Co., Ltd.
 */

/**
 * @defgroup osal_mutex osal_mutex
 */
#ifndef __OSAL_MUTEX_H__
#define __OSAL_MUTEX_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define OSAL_MUTEX_WAIT_FOREVER (-1)

typedef struct {
    void *mutex;
} osal_mutex;

/**
 * @ingroup osal_mutex
 * @brief Initialize the mutex.
 *
 * @par Description:
 * This API is used to initialize the mutex.
 *
 * @param mutex [in] the mutex to be initialized.
 *
 * @return OSAL_FAILURE/OSAL_SUCCESS
 *
 * @par Support System:
 * linux liteos freertos.
 */
int osal_mutex_init(osal_mutex *mutex);

/**
 * @ingroup osal_mutex
 * @brief Acquire the mutex.
 *
 * @par Description:
 * Acquire the mutex. Lock the mutex exclusively for this task.
 * If the mutex is not available right now, it will sleep until it can get it.
 *
 * @param mutex [in] the mutex to be initialized.
 *
 * @return OSAL_FAILURE/OSAL_SUCCESS
 *
 * @note
 * The mutex must later on be released by the same task that acquired it. Recursive locking is not allowed.
 * The task may not exit without first unlocking the mutex.
 * Also, kernel memory where the mutex resides must not be freed with the mutex still locked.
 * The mutex must first be initialized (or statically defined) before it can be locked.
 * memset()-ing the mutex to 0 is not allowed.
 *
 * @par Support System:
 * linux liteos freertos.
 * @par System Differ:LiteOS and Freertos supports the nested lock feature.
 */
int osal_mutex_lock(osal_mutex *mutex);

/**
 * @ingroup osal_mutex
 * @brief Acquire the mutex until timeout.
 *
 * @par Description:
 * This API is used to wait for a specified period of time to acquire the mutex.
 *
 * @param mutex [in] The mutex to be acquired.
 * @param timeout [in] how long to wait before failing.
 *
 * @return OSAL_FAILURE/OSAL_SUCCESS
 *
 * @par Support System:
 * liteos freertos.
 * @par System Differ: LiteOS and Freertos supports the nested lock feature.
 */
int osal_mutex_lock_timeout(osal_mutex *mutex, unsigned int timeout);

/**
 * @ingroup osal_mutex
 * @brief Acquire the mutex until timeout, interruptible by signals.
 *
 * @par Description:
 * Acquire the mutex, interruptible by signals. Lock the mutex like mutex_lock().
 * If a signal is delivered while the process is sleeping, this function will return without acquiring the mutex.
 *
 * @param mutex [in] The mutex to be acquired.
 *
 * @return OSAL_FAILURE/OSAL_SUCCESS/OSAL_EINTR
 *
 * @par Support System:
 * linux liteos.
 * @par System Differ: Only LiteOS supports the nested lock feature.
 */
int osal_mutex_lock_interruptible(osal_mutex *mutex);

/**
 * @ingroup osal_mutex
 * @brief Try to acquire the mutex.
 *
 * @par Description:
 * try to acquire the mutex, without waiting.
 *
 * @param mutex [in] The mutex to be acquired.
 *
 * @return TRUE/FALSE
 *
 * @par Support System:
 * linux liteos freertos.
 * @par System Differ: LiteOS and Freertos supports the nested lock feature.
 */
int osal_mutex_trylock(osal_mutex *mutex);

/**
 * @ingroup osal_mutex
 * @brief Release the mutex.
 *
 * @par Description:
 * Release the mutex. Unlock a mutex that has been locked by this task previously.
 * This function must not be used in interrupt context. Unlocking of a not locked mutex is not allowed.
 *
 * @param mutex [in] the mutex to be released.
 *
 * @par Support System:
 * linux liteos freertos.
 */
void osal_mutex_unlock(osal_mutex *mutex);

/**
 * @ingroup osal_mutex
 * @brief is the mutex locked.
 *
 * @par Description:
 * is the mutex locked.
 *
 * @param mutex [in] The mutex to be acquired.
 *
 * @return true if the mutex is locked, false if unlocked.
 *
 * @par Support System:
 * linux freertos.
 */
int osal_mutex_is_locked(osal_mutex *mutex);

/**
 * @ingroup osal_mutex
 * @brief Destroy the mutex.
 *
 * @par Description:
 * This API is used to destroy the mutex.
 *
 * @param mutex [in] The mutex to be destroyed.
 *
 * @attention
 * must be called when kmod exit, otherwise will lead to memory leak; osal_mutex_destroy will free memory,
 * caller should clear pointer to be NULL after called.
 *
 * @par Support System:
 * linux liteos freertos.
 */
void osal_mutex_destroy(osal_mutex *mutex);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
#endif /* __OSAL_MUTEX_H__ */