/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */

#ifndef PM_THREAD_H
#define PM_THREAD_H

#include "ot_osal.h"
#include "ot_type.h"

#ifdef __cplusplus
extern "C" {
#endif

extern td_u32 g_core_adjust_wait_time;

typedef enum {
    THREAD_CMD_START = 0, /* 启动指令：启动处于停止状态的线程 */
    THREAD_CMD_SLEEP, /* 休眠指令：让线程休眠等待 */
    THREAD_CMD_STOP, /* 销毁指令：使线程停止运行 */
    THREAD_CMD_DESTROYED, /* 线程已被销毁 */
    THREAD_CMD_BUTT
} pm_thread_cmd;

typedef struct {
    osal_task *h_thread; /* 线程句柄 */
    osal_wait wait_event;
    pm_thread_cmd thread_ctrl;
    td_bool wakeup_flag;
} pm_thread;

int pm_get_core_extra_comp(void);
int pm_get_cpu_extra_comp(void);
int pm_get_npu_extra_comp(void);
td_void pm_wake_up_thread(td_void);
td_void pm_thread_create(td_void);
td_void pm_thread_destory(td_void);

#ifdef __cplusplus
}
#endif

#endif /* end of #ifndef PM_THREAD_H */
