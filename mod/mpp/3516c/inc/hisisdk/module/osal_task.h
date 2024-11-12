/*
  Copyright (c), 2001-2024, Shenshu Tech. Co., Ltd.
 */

/**
 * @defgroup osal_task osal_task
 */
#ifndef __OSAL_TASK_H__
#define __OSAL_TASK_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#if defined(__LITEOS__) || defined(__FREERTOS__)
#define OSAL_TASK_PRIORITY_ABOVE_HIGH   2
#define OSAL_TASK_PRIORITY_HIGH         3
#define OSAL_TASK_PRIORITY_BELOW_HIGH   4
#define OSAL_TASK_PRIORITY_ABOVE_MIDDLE 5
#define OSAL_TASK_PRIORITY_MIDDLE       6
#define OSAL_TASK_PRIORITY_BELOW_MIDDLE 7
#define OSAL_TASK_PRIORITY_ABOVE_LOW    8
#define OSAL_TASK_PRIORITY_LOW          10
#define OSAL_TASK_PRIORITY_BELOW_LOW    11
#elif defined(__KERNEL__)
#define OSAL_TASK_PRIORITY_ABOVE_HIGH   99
#define OSAL_TASK_PRIORITY_HIGH         90
#define OSAL_TASK_PRIORITY_BELOW_HIGH   80
#define OSAL_TASK_PRIORITY_ABOVE_MIDDLE 70
#define OSAL_TASK_PRIORITY_MIDDLE       60
#define OSAL_TASK_PRIORITY_BELOW_MIDDLE 50
#define OSAL_TASK_PRIORITY_ABOVE_LOW    40
#define OSAL_TASK_PRIORITY_LOW          30
#define OSAL_TASK_PRIORITY_BELOW_LOW    20
#else
#define OSAL_TASK_PRIORITY_ABOVE_HIGH   2
#define OSAL_TASK_PRIORITY_HIGH         3
#define OSAL_TASK_PRIORITY_BELOW_HIGH   4
#define OSAL_TASK_PRIORITY_ABOVE_MIDDLE 5
#define OSAL_TASK_PRIORITY_MIDDLE       6
#define OSAL_TASK_PRIORITY_BELOW_MIDDLE 7
#define OSAL_TASK_PRIORITY_ABOVE_LOW    8
#define OSAL_TASK_PRIORITY_LOW          10
#define OSAL_TASK_PRIORITY_BELOW_LOW    11
#endif
typedef struct {
    void *task;
} osal_task;
typedef int (*osal_kthread_handler)(void *data);

/**
 * @ingroup osal_task
 *
 * @brief Provides an interface for creating a thread and invokes kthread_run for creating a kernel thread.
 *
 * @par Description:
 * Provides an interface for creating a thread and invokes kthread_run for creating a kernel thread.
 * If the size of the task stack to be created is less than or equal to MINIMAL_STACK_SIZE,
 * set @stack_size to MINIMAL_STACK_SIZE to specify the default size of the task stack.
 * The stack size is determined by whether it is big enough to avoid task stack overflow.
 *
 * @param stack_size [in] Size of the stack space of the thread.
 * @param handler [in] Functions to be processed by the thread
 * @param data [in] Function handler data.
 * @param name [in] Thread name displayed.
 *
 * @retval   osal_task*  If the thread is successfully created, the pointer of the thread is returned.
 * @retval   NULL        If the thread fails to be created, NULL is returned.
 *
 * @par Support System:
 * linux liteos freertos.
 */
osal_task *osal_kthread_create(osal_kthread_handler handler, void *data, const char *name, unsigned int stack_size);

/**
 * @ingroup osal_task
 *
 * @brief Setting the priority of a thread.
 *
 * @par Description:
 * Sets the priority of the current thread.
 *
 * @param task [in] Threads to be prioritized.
 * @param priority [in] Priority to be set, It must be one of the following three priorities.
 * OSAL_TASK_PRIORITY_HIGH, OSAL_TASK_PRIORITY_MIDDLE, OSAL_TASK_PRIORITY_LOW.
 *
 * @return OSAL_FAILURE/OSAL_SUCCESS.
 *
 * @par Support System:
 * linux liteos freertos.
 */
int osal_kthread_set_priority(osal_task *task, unsigned int priority);

/**
 * @ingroup osal_task
 *
 * @brief Setting the CPU Affinity of a Task.
 *
 * @par Description:
 * Setting the CPU Affinity of a Task.
 *
 * @param task [in]    Thread to change
 * @param cpu_mask [in]  Specifies the mask of the CPU.You can only choose one of the following
 * OSAL_CPU_ALL, OSAL_CPU_0, OSAL_CPU_1, OSAL_CPU_2, OSAL_CPU_3.
 *
 * @par Support System:
 * linux liteos .
 */
void osal_kthread_set_affinity(osal_task *task, int cpu_mask);

/**
 * @ingroup osal_task
 * @brief Check whether the thread is running.
 *
 * @par Description:
 * A function used to determine whether a thread in the kernel is still running.
 *
 * @retval 0 If one of the current threads continues to run in the kernel.
 * @retval 1 If a current thread is stopped somewhere in the kernel by using kthread_stop(struct task_struct *).
 *
 * @par Support System:
 * linux liteos.
 */
int osal_kthread_should_stop(void);

/**
 * @ingroup osal_task
 *
 * @brief This function is used to wake up a thread.
 *
 * @par Description:
 * This function is used to wake up a thread.
 *
 * @param task [in] Thread you want to wake up.
 *
 * @retval 0  If the wakeup is successful, 0 is returned.
 * @retval -1 If the wakeup fails, -1 is returned.
 *
 * @par Support System:
 * linux.
 */
int osal_kthread_wakeup_process(osal_task *task);

/**
 * @ingroup osal_task
 *
 * @brief Bind the created thread to run on the execution CPU core.
 *
 * @par Description:
 * Bind the created thread to run on the execution CPU core.
 *
 * @param task [in] Thread you want to bind.
 * @param cpu [in] This kernel thread is bound to the CPU.
 *
 * @par Support System:
 * linux.
 */
void osal_kthread_bind(osal_task *task, unsigned int cpu);

/**
 * @ingroup osal_task
 * @brief Lock the task scheduling.
 *
 * @par Description:
 * This API is used to lock the task scheduling. Task switching will not occur if the task scheduling is locked.
 *
 * @attention
 * If the task scheduling is locked, but interrupts are not disabled, tasks are still able to be interrupted.
 * One is added to the number of task scheduling locks if this API is called. The number of locks is decreased by
 * one if the task scheduling is unlocked. Therefore, this API should be used together with osal_kthread_unlock.
 *
 * @par Support System:
 * liteos freertos .
 */
void osal_kthread_lock(void);

/**
 * @ingroup osal_task
 * @brief Unlock the task scheduling.
 *
 * @par Description:
 * This API is used to unlock the task scheduling. Calling this API will decrease the number of task locks by one.
 * If a task is locked more than once, the task scheduling will be unlocked only when the number of locks becomes zero.
 *
 * @attention
 * The number of locks is decreased by one if this API is called. One is added to the number of task scheduling
 * locks if the task scheduling is locked. Therefore, this API should be used together with osal_kthread_lock.
 *
 * @par Support System:
 * liteos freertos.
 */
void osal_kthread_unlock(void);

/**
 * @ingroup osal_task
 *
 * @brief Used to destroy the threads you created.
 *
 * @par Description:
 * Stops a specified kernel thread.
 *
 * @attention osal_kthread_destroy will free memory of task, caller should clear pointer to be NULL.
 * Note that if you want to destroy a thread, the thread cannot end before calling this function,
 * Or something terrible will happen.
 * When you call the Kthread_stop function, the thread function cannot be finished, otherwise it will oops.
 * @task may be free in api, @task should be from osal_kthread_create.
 *
 * @param task  [in] Threads you want to destroy.
 * @param stop_flag [in] Indicates whether the current thread exits. If the value of stop_flag is 0,
 * The current thread does not exit. The stop flag is not 0.
 *
 * @par Support System:
 * linux liteos freertos.
 */
void osal_kthread_destroy(osal_task *task, unsigned int stop_flag);

/**
 * @ingroup osal_task
 *
 * @brief In this state, it cannot be woken up by an external signal and can only be woken up by the kernel itself.
 *
 * @par Description:
 * The current thread enters the Task_UNINTERRUPTIBLE state and cannot be woken up by external signals.
 * The kernel wakes up the thread when the sleep time arrives.
 *
 * @param sleep_ns [in]  Sleep time, units is nanosecond(ns).
 *
 * @par Support System:
 * linux.
 */
void osal_kthread_schedule(unsigned int sleep_ns);

/**
 * @ingroup osal_task
 *
 * @brief  Set the current thread to the Task_UNINTERRUPTIBLE state.
 *
 * @par Description:
 * In this state, it cannot be woken up by an external signal and can only be woken up by the kernel itself.
 *
 * @par Support System:
 * linux.
 */
void osal_kthread_set_uninterrupt(void);

/**
 * @ingroup osal_task
 *
 * @brief  Set the current thread status to TASK_RUNNING.
 *
 * @par Description:
 * The fact that the process is runnable does not mean that the process has actually been allocd to the CPU,
 * it may wait until the scheduler selects it.
 * This state simply ensures that the process can run as soon as it is selected by the CPU,
 * without waiting for an external event.
 *
 * @par Support System:
 * linux.
 */
void osal_kthread_set_running(void);

/**
 * @ingroup osal_task
 *
 * @brief Programs running in kernel mode can call osal_cond_resched to actively give up CPU resources.
 *
 * @par Description:
 * This prevents soft lockup or long scheduling delay caused by the program running in kernel mode for a long time.
 *
 * @par Support System:
 * linux.
 */
void osal_cond_resched(void);

/**
 * @ingroup osal_task
 * @brief This function put the current task back to the ready queue and try to do the schedule.
 *
 * @par Description:
 * This function put the current task back to the ready queue and try to do the schedule.
 *
 *
 * @par Support System:
 * linux liteos freertos.
 */
void osal_schedule(void);

/**
 * @ingroup osal_task
 * @brief Enabling NEON Algorithm Acceleration.
 *
 * @par Description:
 * Enabling NEON Algorithm Acceleration.
 *
 * @attention
 * This interface work only when CONFIG_KERNEL_MODE_NEON is defined, Otherwise do nothing.
 *
 * @par Support System:
 * linux.
 */
void osal_kneon_begin(void);

/**
 * @ingroup osal_task
 * @brief Disable NEON Algorithm Acceleration.
 *
 * @par Description:
 * Disable NEON Algorithm Acceleration.
 *
 * @attention
 * This interface work only when CONFIG_KERNEL_MODE_NEON is defined, Otherwise do nothing.
 *
 * @par Support System:
 * linux.
 */
void osal_kneon_end(void);

/**
 * @ingroup osal_task
 *
 * @brief  Pause the current thread.
 *
 * @par Description:
 * The time slice of the CPU of the current thread is released
 * so that the running thread becomes ready again and contends for the scheduling right of the CPU again.
 * It may be obtained, or it may be obtained by other threads.
 *
 * @par Support System:
 * linux freertos.
 */
void osal_yield(void);

/**
 * @ingroup osal_task
 *
 * @brief Obtains the pid of the current thread.
 *
 * @par Description:
 * Obtains the pid of the current thread.
 *
 * @return pid of the current thread.
 *
 * @par Support System:
 * linux liteos.
 */
long osal_get_current_pid(void);

/**
 * @ingroup osal_task
 *
 * @brief Obtains the tid of the current thread.
 *
 * @par Description:
 * Obtains the tid of the current thread.
 *
 * @return  tid of the current thread.
 *
 * @par Support System:
 * linux liteos seliteos.
 */
long osal_get_current_tid(void);

/**
 * @ingroup osal_task
 *
 * @brief Obtains the tgid of the current thread.
 *
 * @par Description:
 * Obtains the tgid of the current thread.
 *
 * @return  tgid of the current thread.
 *
 * @par Support System:
 * linux.
 */
int osal_get_current_tgid(void);

/**
 * @ingroup osal_task
 *
 * @brief Obtains the name of a thread.
 *
 * @par Description:
 * Obtains the name of a thread.
 *
 * @return name of the current thread.
 *
 * @par Support System:
 * linux.
 */
char *osal_get_current_taskname(void);

/**
 * @ingroup osal_task
 * @brief sleep.
 *
 * @par Description:
 * sleep.
 *
 * @param msecs [in] Time in milliseconds to sleep for.
 *
 * @return Returns 0 when the timer has expired otherwise the remaining time in ms will be returned.
 *
 * @par Support System:
 * linux liteos freertos.
 */
unsigned long osal_msleep(unsigned int msecs);

/**
 * @ingroup osal_task
 * @brief sleep safely even with waitqueue interruptions.
 *
 * @par Description:
 * sleep safely even with waitqueue interruptions.
 *
 * @param msecs [in] Time in milliseconds to sleep for.
 *
 * @par Support System:
 * linux liteos.
 */
void osal_msleep_uninterruptible(unsigned int msecs);

/**
 * @ingroup osal_task
 * @brief spinning-delay in microsecond (us).
 *
 * @par Description:
 * This API is used to delay in microsecond.
 *
 * @param usecs [in] Time in microseconds to wait for.
 *
 * @par Support System:
 * linux liteos freertos.
 */
void osal_udelay(unsigned int usecs);

/**
 * @ingroup osal_task
 * @brief spinning-delay in millisecond (ms).
 *
 * @par Description:
 * This API is used to delay in millisecond.
 *
 * @param msecs [in] Time in milliseconds to wait for.
 *
 * @par Support System:
 * linux liteos freertos.
 */
void osal_mdelay(unsigned int msecs);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
#endif /* __OSAL_TASK_H__ */
