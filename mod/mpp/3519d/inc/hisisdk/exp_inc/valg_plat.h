/*
  Copyright (c), 2001-2024, Shenshu Tech. Co., Ltd.
 */

#ifndef VALG_PLAT_H
#define VALG_PLAT_H

#include "ot_osal.h"

#include "ot_type.h"
#include "ot_debug.h"
#include "ot_common.h"
#include "proc_ext.h"

#ifdef __KERNEL__
#else
#include <sys/time.h>
#endif

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define valg_trace(level, mod_id, fmt, ...)                                                                \
    do {                                                                                                    \
        OT_TRACE(level, mod_id, "[Func]:%s [Line]:%d [Info]:" fmt, __FUNCTION__, __LINE__, ##__VA_ARGS__); \
    } while (0)
#define VALG_ERR_LEVEL_NOTE 0
typedef osal_spinlock valg_lock;

__inline static td_s32 valg_create_lock(valg_lock *lock, td_bool locked)
{
    ot_unused(locked);
    return osal_spin_lock_init(lock);
}

__inline static td_s32 valg_lock_irqsave(valg_lock *lock, td_ulong *flag)
{
    osal_spin_lock_irqsave(lock, flag);
    return TD_SUCCESS;
}

__inline static td_s32 valg_unlock(valg_lock *lock, td_ulong *flag)
{
    osal_spin_unlock_irqrestore(lock, flag);
    return TD_SUCCESS;
}

__inline static td_void valg_delete_lock(valg_lock *lock)
{
    osal_spin_lock_destroy(lock);
    return;
}
#define FORCEINLINE         __inline__ __attribute__((always_inline))

typedef osal_timer valg_timer;
typedef td_void (*ptr_timer_call_back)(unsigned long data);

__inline static td_s32 valg_timer_init(valg_timer *timer, ptr_timer_call_back handler, td_ulong data, td_u32 interval)
{
    td_s32 ret;

    timer->handler = handler;
    timer->data = data;
    timer->interval = interval;
    ret = osal_timer_init(timer);
    if (ret) {
        return ret;
    }

    return TD_SUCCESS;
}

__inline static td_s32 valg_timer_start_once(valg_timer *timer, td_u32 interval)
{
    return osal_timer_mod(timer, interval);
}

__inline static td_void valg_timer_delete(valg_timer *timer)
{
    osal_timer_stop(timer);
    osal_timer_destroy(timer);

    return;
}

__inline static td_s32 valg_thread_init(osal_task **thread, td_void *task_function,
    td_void *data, const char *task_name)
{
    *thread = osal_kthread_create(task_function, data, task_name, 0);
    if (*thread == NULL) {
        osal_printk("create thread fail!!!\n");
        return TD_FAILURE;
    }

    return TD_SUCCESS;
}

__inline static td_u32 valg_thread_delete(osal_task *task, td_bool stop_flag)
{
    osal_kthread_destroy(task, stop_flag);
    return TD_SUCCESS;
}

typedef osal_irq_handler ptr_int_callback;

__inline static td_s32 valg_irq_init(td_s32 irq, ptr_int_callback ptr_call_back, td_void *data)
{
    return osal_irq_request(irq, ptr_call_back, NULL, "vedu", data);
}

__inline static td_s32 valg_irq_delete(td_s32 irq, td_void *dev_id)
{
    osal_irq_free(irq, dev_id);
    return TD_SUCCESS;
}

__inline static td_u64 get_sys_time_by_sec(td_void)
{
#ifdef __KERNEL__
    osal_timeval time;
    osal_gettimeofday(&time);
#else
    struct timeval time;
    gettimeofday(&time, NULL);
#endif
    return (td_u64)time.tv_sec;
}

__inline static td_u64 get_sys_time_by_usec(td_void)
{
#ifdef __KERNEL__
    osal_timeval time;
    osal_gettimeofday(&time);
#else
    struct timeval time;
    gettimeofday(&time, NULL);
#endif
    return (td_u64)((time.tv_sec * 1000000LLU) + time.tv_usec);
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif
