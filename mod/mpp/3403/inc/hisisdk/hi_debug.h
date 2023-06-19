/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */

#ifndef __HI_DEBUG_H__
#define __HI_DEBUG_H__

#include "ot_debug.h"

#ifdef __cplusplus
extern "C" {
#endif

#define HI_MAX_MOD_NAME_LEN OT_MAX_MOD_NAME_LEN
#define HI_DBG_EMERG OT_DBG_EMERG
#define HI_DBG_ALERT OT_DBG_ALERT
#define HI_DBG_CRIT OT_DBG_CRIT
#define HI_DBG_ERR OT_DBG_ERR
#define HI_DBG_WARN OT_DBG_WARN
#define HI_DBG_NOTICE OT_DBG_NOTICE
#define HI_DBG_INFO OT_DBG_INFO
#define HI_DBG_DEBUG OT_DBG_DEBUG
#define HI_PRINT OT_PRINT
#define HI_TRACE OT_TRACE
#define HI_EMERG_TRACE OT_EMERG_TRACE
#define HI_ALERT_TRACE OT_ALERT_TRACE
#define HI_CRIT_TRACE OT_CRIT_TRACE
#define HI_ERR_TRACE OT_ERR_TRACE
#define HI_WARN_TRACE OT_WARN_TRACE
#define HI_NOTICE_TRACE OT_NOTICE_TRACE
#define HI_INFO_TRACE OT_INFO_TRACE
#define HI_DEBUG_TRACE OT_DEBUG_TRACE
typedef ot_log_level_cfg hi_log_level_cfg;

#ifdef __KERNEL__
#define hi_assert(expr) ot_assert(expr)
#endif

#ifdef __cplusplus
}
#endif
#endif /* __HI_DEBUG_H__ */
