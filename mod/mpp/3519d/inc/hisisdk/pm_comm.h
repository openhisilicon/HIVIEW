/*
  Copyright (c), 2001-2024, Shenshu Tech. Co., Ltd.
 */
#ifndef OT_PM_COMM_H
#define OT_PM_COMM_H

#include "ot_type.h"
#include "ot_common.h"
#include "ot_osal.h"
#include "ot_debug.h"
#include "ot_pm_mod_init.h"

#define TSENSOR_WAIT_TIME 			10

#define PM_DDR_SIZE_1K    			0x400          /* 1KB */

#define PM_TEMP_25 				25
#define PM_TEMP_35 				35
#define PM_TEMP_95 				95

#define PM_ADJUST_VOLT_STEP_5MV 			5
#define PM_ADJUST_VOLT_STEP_6MV 			6
#define PM_CORE_ADJUST_WAIT_2S 		2

#define SVB_16D_SEC  			0xc
#define SVB_16D_SEC_BATTERY  		0xd
#define SVB_19D_SEC  			0x4
#define SVB_19D_SEC_BATTERY  		0x5
#define SVB_19D_PT  				0x8
#define SVB_19D_PT_BATTERY  			0x9

#define PM_19D_PT_BRANCH  			1
#define PM_19D_BRANCH  				2
#define PM_16D_BRANCH  				3

#ifdef __cplusplus
extern "C" {
#endif /* end of #ifdef __cplusplus */

#define pm_emerg_trace(fmt, ...) \
    OT_EMERG_TRACE(OT_ID_PM, "[Func]:%s [Line]:%d [Info]:" fmt, __FUNCTION__, __LINE__, ##__VA_ARGS__)

#define pm_alert_trace(fmt, ...) \
    OT_ALERT_TRACE(OT_ID_PM, "[Func]:%s [Line]:%d [Info]:" fmt, __FUNCTION__, __LINE__, ##__VA_ARGS__)

#define pm_crit_trace(fmt, ...) \
    OT_CRIT_TRACE(OT_ID_PM, "[Func]:%s [Line]:%d [Info]:" fmt, __FUNCTION__, __LINE__, ##__VA_ARGS__)

#define pm_err_trace(fmt, ...) \
    OT_ERR_TRACE(OT_ID_PM, "[Func]:%s [Line]:%d [Info]:" fmt, __FUNCTION__, __LINE__, ##__VA_ARGS__)

#define pm_warn_trace(fmt, ...) \
    OT_WARN_TRACE(OT_ID_PM, "[Func]:%s [Line]:%d [Info]:" fmt, __FUNCTION__, __LINE__, ##__VA_ARGS__)

#define pm_notice_trace(fmt, ...) \
    OT_NOTICE_TRACE(OT_ID_PM, "[Func]:%s [Line]:%d [Info]:" fmt, __FUNCTION__, __LINE__, ##__VA_ARGS__)

#define pm_info_trace(fmt, ...) \
    OT_INFO_TRACE(OT_ID_PM, "[Func]:%s [Line]:%d [Info]:" fmt, __FUNCTION__, __LINE__, ##__VA_ARGS__)

#define pm_debug_trace(fmt, ...) \
    OT_DEBUG_TRACE(OT_ID_PM, "[Func]:%s [Line]:%d [Info]:" fmt, __FUNCTION__, __LINE__, ##__VA_ARGS__)

typedef struct {
    td_bool core_tcomp_en;
    td_bool cpu_tcomp_en;
    td_bool npu_tcomp_en;
    td_bool core_pacomp_en;
    td_bool cpu_pacomp_en;
    td_bool npu_pacomp_en;
    td_s32 cur_temp;
    td_s32 core_temp_comp;
    td_s32 cpu_temp_comp;
    td_s32 npu_temp_comp;
    td_s32 core_pa_comp;
    td_s32 cpu_pa_comp;
    td_s32 npu_pa_comp;
    td_u32 core_cur_volt;
    td_u32 cpu_cur_volt;
    td_u32 npu_cur_volt;
    td_s32 cpu_first_nbti_data;
    td_s32 npu_first_nbti_data;
    osal_semaphore pm_sem;
} pm_context;

typedef struct {
    td_u32 temp_comp_timer_cnt;
    td_u32 age_comp_timer_cnt;
} pm_proc;

td_void pm_set_svb_version(td_void);
td_u32 pm_get_svb_version(td_void);

pm_context *pm_get_ctx(td_void);
pm_proc *pm_get_proc_ctx(td_void);

td_s32 pm_temp_comp_init(td_void);
td_void pm_temp_comp_deinit(td_void);

td_s32 pm_age_comp_init(td_void);
td_void pm_age_comp_deinit(td_void);

td_s32 pm_common_init(td_void);
td_void pm_common_exit(td_void);

td_void pm_debug_fill_reg_data(td_void);

#ifdef __cplusplus
}
#endif /* end of #ifdef __cplusplus */
#endif /* end of #ifndef OT_PM_COMM_H */