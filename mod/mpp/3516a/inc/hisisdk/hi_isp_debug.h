/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: Function of hi_isp_debug.h
 * Author: ISP SW
 * Create: 2012/06/28
 */

#ifndef __HI_ISP_DEBUG_H__
#define __HI_ISP_DEBUG_H__

#include "hi_debug.h"

#define isp_emerg_trace(fmt, ...) \
    HI_EMERG_TRACE(HI_ID_ISP, "[Func]:%s [Line]:%d [Info]:" fmt, __FUNCTION__, __LINE__, ##__VA_ARGS__);

#define isp_alert_trace(fmt, ...)  \
    HI_ALERT_TRACE(HI_ID_ISP, "[Func]:%s [Line]:%d [Info]:" fmt, __FUNCTION__, __LINE__, ##__VA_ARGS__);

#define isp_crit_trace(fmt, ...) \
    HI_CRIT_TRACE(HI_ID_ISP, "[Func]:%s [Line]:%d [Info]:" fmt, __FUNCTION__, __LINE__, ##__VA_ARGS__);

#define isp_err_trace(fmt, ...) \
    HI_ERR_TRACE(HI_ID_ISP, "[Func]:%s [Line]:%d [Info]:" fmt, __FUNCTION__, __LINE__, ##__VA_ARGS__);

#define isp_warn_trace(fmt, ...) \
    HI_WARN_TRACE(HI_ID_ISP, "[Func]:%s [Line]:%d [Info]:" fmt, __FUNCTION__, __LINE__, ##__VA_ARGS__);

#define isp_notice_trace(fmt, ...) \
    HI_NOTICE_TRACE(HI_ID_ISP, "[Func]:%s [Line]:%d [Info]:" fmt, __FUNCTION__, __LINE__, ##__VA_ARGS__);

#define isp_info_trace(fmt, ...) \
    HI_INFO_TRACE(HI_ID_ISP, "[Func]:%s [Line]:%d [Info]:" fmt, __FUNCTION__, __LINE__, ##__VA_ARGS__);

#define isp_debug_trace(fmt, ...) \
    HI_DEBUG_TRACE(HI_ID_ISP, "[Func]:%s [Line]:%d [Info]:" fmt, __FUNCTION__, __LINE__, ##__VA_ARGS__);

/* To avoid divide-0 exception in code. */
#define div_0_to_1(a)   (((a) == 0) ? 1 : (a))
#define div_0_to_1_float(a) ((((a) < 1E-10) && ((a) > (-1E-10))) ? 1 : (a))

#endif     /* __HI_ISP_DEBUG_H__ */
