/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */
#ifndef OT_PM_TIMER_H
#define OT_PM_TIMER_H

#include "ot_type.h"

extern td_u32 g_pm_svb_version;

#ifdef __cplusplus
extern "C" {
#endif /* end of #ifdef __cplusplus */

td_s32 pm_temp_comp_timer_start_once(td_void);
td_s32 pm_age_comp_timer_start_once(td_void);

td_s32 pm_temp_comp_timer_init(td_void);
td_void pm_temp_comp_timer_deinit(td_void);

td_s32 pm_age_comp_timer_init(td_void);
td_void pm_age_comp_timer_deinit(td_void);

#ifdef __cplusplus
}
#endif /* end of #ifdef __cplusplus */
#endif /* end of #ifndef OT_PM_TIMER_H */
