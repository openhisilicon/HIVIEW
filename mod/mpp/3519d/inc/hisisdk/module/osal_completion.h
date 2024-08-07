/*
  Copyright (c), 2001-2024, Shenshu Tech. Co., Ltd.
 */

/**
 * @defgroup osal_completion osal_completion
 */
#ifndef __OSAL_COMPLETION_H__
#define __OSAL_COMPLETION_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

typedef struct {
    void *completion;
} osal_completion;

/**
 * @ingroup osal_completion
 * @brief Initialize a dynamically allocd completion.
 *
 * @param com [out] pointer to completion structure that is to be initialized
 *
 * @attention Must be freed with osal_complete_destory.
 *
 * @return OSAL_SUCCESS/OSAL_FAILURE
 *
 * @par Support System:
 * linux liteos freertos.
 */
int osal_completion_init(osal_completion *com);

/**
 * @ingroup osal_completion
 * @brief Reset the count of osal_complete to 0.
 *
 * @param com [in] holds the state of this particular completion
 *
 * @attention Must be freed with osal_complete_destory.
 *
 * @par Support System:
 * linux.
 */
void osal_completion_reinit(osal_completion *com);

/**
 * @ingroup osal_completion
 * @brief signals a single thread waiting on this completion.
 *
 * @par Description:
 * This will wake up a single thread waiting on this completion.
 * Threads will be awakened in the same order in which they were queued.
 * If this function wakes up a task, it executes a full memory barrier before accessing the task state.
 *
 * @param com [in] holds the state of this particular completion
 *
 * @par Support System:
 * linux liteos freertos.
 */
void osal_complete(osal_completion *com);

/**
 * @ingroup osal_completion
 * @brief waits for completion of a task.
 *
 * @par Description:
 * This waits to be signaled for completion of a specific task.
 * It is NOT interruptible and there is no timeout.
 *
 * @param com [in] holds the state of this particular completion
 *
 * @par Support System:
 * linux liteos freertos.
 */
void osal_wait_for_completion(osal_completion *com);

/**
 * @ingroup osal_completion
 * @brief waits for completion of a task (w/timeout)
 *
 * @par Description:
 * This waits for either a completion of a specific task to be signaled or for a
 * specified timeout to expire. The timeout is in jiffies. It is not interruptible.
 *
 * @param com [in] holds the state of this particular completion
 * @param timeout [in] jeffies in linux, tick in liteos
 *
 * @return return 0 if timed out, and positive (left time till timeout) if completed, or -1 if failed.
 *
 * @par Support System:
 * linux liteos
 */
unsigned long osal_wait_for_completion_timeout(osal_completion *com, unsigned long timeout);

/**
 * @ingroup osal_completion
 * @brief signals all threads waiting on this completion.
 *
 * @par Description:
 * This will wake up all threads waiting on this particular completion event.
 * If this function wakes up a task, it executes a full memory barrier before accessing the task state.
 *
 * @param com [in] holds the state of this particular completion
 *
 * @par Support System:
 * linux liteos.
 */
void osal_complete_all(osal_completion *com);

/**
 * @ingroup osal_completion
 * @brief free a dynamically allocd completion.
 *
 * @par Description:
 * free a dynamically allocd completion.
 *
 * @param com [in] holds the state of this particular completion
 *
 * @attention this API may free memory, com Must be from osal_complete_init
 * @par Support System:
 * linux liteos freertos.
 */
void osal_complete_destory(osal_completion *com);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
#endif /* __OSAL_COMPLETION_H__ */
