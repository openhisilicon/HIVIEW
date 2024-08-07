/*
  Copyright (c), 2001-2024, Shenshu Tech. Co., Ltd.
 */

#ifndef PM_THREAD_H
#define PM_THREAD_H

#include "ot_osal.h"
#include "ot_type.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    THREAD_CMD_START = 0, /* start instruction: start the stopped thread */
    THREAD_CMD_SLEEP, /* sleep instruction: enables the thread to sleep and wait */
    THREAD_CMD_STOP, /* destroy instruction: stop the thread */
    THREAD_CMD_DESTROYED, /* thread has been destroyed */
    THREAD_CMD_BUTT
} pm_thread_cmd;

typedef struct {
    osal_task *h_thread; /* thread handle */
    osal_wait wait_event;
    pm_thread_cmd thread_ctrl;
    td_bool wakeup_flag;
} pm_thread;

td_u32 pm_get_quick_start_flag(void);
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
