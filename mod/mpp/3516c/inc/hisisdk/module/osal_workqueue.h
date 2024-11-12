/*
  Copyright (c), 2001-2024, Shenshu Tech. Co., Ltd.
 */

/**
 * @defgroup osal_workqueue osal_workqueue
 */
#ifndef __OSAL_WORKQUEUE_H__
#define __OSAL_WORKQUEUE_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

typedef struct osal_workqueue_ {
    int queue_flag;
    void *work;
    void (*handler)(struct osal_workqueue_ *workqueue);
} osal_workqueue;
typedef void (*osal_workqueue_handler)(osal_workqueue *workqueue);

/**
 * @ingroup osal_workqueue
 * @brief This API is used to initialization of workqueue.
 *
 * @par Description:
 * This API is used to initialization of workqueue.
 *
 * @param work [in/out] the workqueue to be initialized.
 *
 * @par Support System:
 * linux liteos freertos.
 */
int osal_workqueue_init(osal_workqueue *work, osal_workqueue_handler handler);

/**
 * @ingroup osal_workqueue
 * @brief put work task in global workqueue.
 *
 * @par Description:
 * put work task in global workqueue.
 * This puts a job in the kernel-global workqueue if it was not already queued and leaves it in
 * the same position on the kernel-global workqueue otherwise.
 *
 * @param work [in] Job to be done.
 * @return True/False
 * @par Support System:
 * linux liteos freertos.
 */
int osal_workqueue_schedule(osal_workqueue *work);

/**
 * @ingroup osal_workqueue
 * @brief This API is used to destroy workqueue.
 *
 * @par Description:
 * This API is used to destroy workqueue.
 *
 * @param work [in] The work to be destroyed.
 *
 * @attention this api may free memory, @work should be from osal_workqueue_init.
 *
 * @par Support System:
 * linux liteos freertos.
 */
void osal_workqueue_destroy(osal_workqueue *work);

/**
 * @ingroup osal_workqueue
 * @brief wait for a work to finish executing the last queueing instance.
 *
 * @par Description:
 * wait for a work to finish executing the last queueing instance.
 * Wait until @work has finished execution.
 * @work is guaranteed to be idle on return if it hasn't been requeued since flush started.
 *
 * @param work [in] The work to flush.
 *
 * @par Support System:
 * linux liteos.
 */
int osal_workqueue_flush(osal_workqueue *work);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
#endif /* __OSAL_WORKQUEUE_H__ */