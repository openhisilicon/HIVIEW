/*
  Copyright (c), 2001-2024, Shenshu Tech. Co., Ltd.
 */

/**
 * @defgroup osal_wait osal_wait
 */
#ifndef __OSAL_WAIT_H__
#define __OSAL_WAIT_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define OSAL_WAIT_FOREVER 0xFFFFFFFF
#define OSAL_WAIT_CONDITION_TRUE 1

typedef struct {
    void *wait;
} osal_wait;

/* return value is a bool type */
typedef int (*osal_wait_condition_func)(const void *param);

/**
 * @ingroup osal_wait
 * @brief Initialize a waiting queue.
 *
 * @par Description:
 * This API is used to Initialize a waiting queue.
 *
 * @param wait [out] The wait queue to be initialized.
 *
 * @par Support System:
 * linux liteos freertos.
 */
int osal_wait_init(osal_wait *wait);

/**
 * @ingroup osal_wait
 * @brief sleep until a condition gets true
 *
 * @par Description:
 * sleep until a condition gets true.
 * The process is put to sleep (TASK_INTERRUPTIBLE) until the @func returned true or a signal is received.
 * Return value of the @func is checked each time the waitqueue @wait is woken up.
 *
 * @attention
 * wake_up() has to be called after changing any variable that could change the result of the wait condition.
 * it is the same as osal_wait_uninterruptible on LiteOS.
 * LiteOS does not support this interface, so it is the same as osal_wait_uninterruptible on LiteOS.
 *
 * @param wait  [in] The waitqueue to wait on.
 * @param func  [in] Then function to be run.
 * @param param [in] Param of the function to be run.
 *
 * @return OSAL_SUCCESS/OSAL_FAILURE.
 * In linux, The function will return -ERESTARTSYS if it was interrupted by a signal. liteos
 *
 * @par Support System:
 * linux liteos.
 */
int osal_wait_interruptible(osal_wait *wait, osal_wait_condition_func func, const void *param);

/**
 * @ingroup osal_wait
 * @brief sleep until a condition gets true
 *
 * @par Description:
 * sleep until a condition gets true.
 * The process is put to sleep (TASK_UNINTERRUPTIBLE) until the @func returned true.
 * Return value of the @func is checked each time the waitqueue @wait is woken up.
 *
 * @attention
 * wake_up() has to be called after changing any variable that could change the result of the wait condition.
 *
 * @return OSAL_SUCCESS/OSAL_FAILURE
 *
 * @par Support System:
 * linux liteos freetos.
 */
int osal_wait_uninterruptible(osal_wait *wait, osal_wait_condition_func func, const void *param);

/**
 * @ingroup osal_wait
 * @brief sleep until a condition gets true or a timeout elapses.
 *
 * @par Description:
 * sleep until a condition gets true or a timeout elapses.
 * The process is put to sleep (TASK_INTERRUPTIBLE) until the @func returned true or a signal is received.
 * Return value of the @func is checked each time the waitqueue @wait is woken up.
 *
 * @param wait [in] the waitqueue to wait on.
 * @param wait [in] a C expression for the event to wait for.
 * @param wait [in] timeout, in ms.
 *
 * @par Support System:
 * linux liteos freertos.
 */
int osal_wait_timeout_interruptible(osal_wait *wait, osal_wait_condition_func func, const void *param,
    unsigned long ms);

/**
 * @ingroup osal_wait
 * @brief sleep until a condition gets true or a timeout elapses.
 *
 * @par Description:
 * sleep until a condition gets true or a timeout elapses.
 * The process is put to sleep (TASK_UNINTERRUPTIBLE) until the @func returned true.
 * Return value of the @func is checked each time the waitqueue @wait is woken up.
 *
 * @param wait [in] The waitqueue to wait on.
 * @param wait [in] A C expression for the event to wait for.
 * @param wait [in] Timeout, in ms.
 *
 * @par Support System:
 * linux liteos.
 */
int osal_wait_timeout_uninterruptible(osal_wait *wait, osal_wait_condition_func func, const void *param,
    unsigned long ms);

/**
 * @ingroup osal_wait
 * @brief wake up threads blocked on a waitqueue.
 *
 * @par Description:
 * Wake-up wait queue,pair to @wait_event.
 *
 * @param wait [in] The wait to be wake up.
 *
 * @par Support System:
 * linux liteos freertos.
 */
void osal_wait_wakeup(osal_wait *wait); // same as wake_up_all

/**
 * @ingroup osal_wait
 * @brief wake up threads blocked on a waitqueue.
 *
 * @par Description:
 * Wake-up wait queue,pair to @wait_event_interruptible.
 *
 * @param wait [in] The wait to be wake up.
 *
 * @par Support System:
 * linux liteos.
 */
void osal_wait_wakeup_interruptible(osal_wait *wait); // same as osal_wait_wakeup on liteos

/**
 * @ingroup osal_wait
 * @brief to destroy the wait.
 *
 * @par Description:
 * This API is used to destroy the wait.
 *
 * @param wait [in] The wait to be destroyed.
 *
 * @attention this api may free memory, wait should be from osal_complete_init.
 * @par Support System:
 * linux liteos freertos.
 */
void osal_wait_destroy(osal_wait *wait);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
#endif /* __OSAL_WAIT_H__ */
