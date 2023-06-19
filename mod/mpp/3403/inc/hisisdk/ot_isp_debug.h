/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */

#ifndef OT_ISP_DEBUG_H
#define OT_ISP_DEBUG_H

#include "ot_debug.h"

#define isp_emerg_trace(fmt, ...) \
    OT_EMERG_TRACE(OT_ID_ISP, "[Func]:%s [Line]:%d [Info]:" fmt, __FUNCTION__, __LINE__, ##__VA_ARGS__)

#define isp_alert_trace(fmt, ...)  \
    OT_ALERT_TRACE(OT_ID_ISP, "[Func]:%s [Line]:%d [Info]:" fmt, __FUNCTION__, __LINE__, ##__VA_ARGS__)

#define isp_crit_trace(fmt, ...) \
    OT_CRIT_TRACE(OT_ID_ISP, "[Func]:%s [Line]:%d [Info]:" fmt, __FUNCTION__, __LINE__, ##__VA_ARGS__)

#define isp_err_trace(fmt, ...) \
    OT_ERR_TRACE(OT_ID_ISP, "[Func]:%s [Line]:%d [Info]:" fmt, __FUNCTION__, __LINE__, ##__VA_ARGS__)

#define isp_warn_trace(fmt, ...) \
    OT_WARN_TRACE(OT_ID_ISP, "[Func]:%s [Line]:%d [Info]:" fmt, __FUNCTION__, __LINE__, ##__VA_ARGS__)

#define isp_notice_trace(fmt, ...) \
    OT_NOTICE_TRACE(OT_ID_ISP, "[Func]:%s [Line]:%d [Info]:" fmt, __FUNCTION__, __LINE__, ##__VA_ARGS__)

#define isp_info_trace(fmt, ...) \
    OT_INFO_TRACE(OT_ID_ISP, "[Func]:%s [Line]:%d [Info]:" fmt, __FUNCTION__, __LINE__, ##__VA_ARGS__)

#define isp_debug_trace(fmt, ...) \
    OT_DEBUG_TRACE(OT_ID_ISP, "[Func]:%s [Line]:%d [Info]:" fmt, __FUNCTION__, __LINE__, ##__VA_ARGS__)

/* To avoid divide-0 exception in code. */
#define div_0_to_1(a)   (((a) == 0) ? 1 : (a))
#define div_0_to_1_float(a) ((((a) < 1E-10) && ((a) > (-1E-10))) ? 1 : (a))

#endif     /* OT_ISP_DEBUG_H */
