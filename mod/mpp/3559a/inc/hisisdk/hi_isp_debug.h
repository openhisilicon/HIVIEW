/*
* Copyright (C) Hisilicon Technologies Co., Ltd. 2012-2019. All rights reserved.
* Description:
* Author: Hisilicon multimedia software group
* Create: 2011/06/28
*/


#ifndef __HI_ISP_DEBUG_H__
#define __HI_ISP_DEBUG_H__

#include "hi_debug.h"

#define ISP_EMERG_TRACE(fmt, ...)                                                                            \
    do {                                                                                                    \
        HI_EMERG_TRACE(HI_ID_ISP, "[Func]:%s [Line]:%d [Info]:" fmt, __FUNCTION__, __LINE__, ##__VA_ARGS__); \
    } while (0)

#define ISP_ALERT_TRACE(fmt, ...)                                                                            \
    do {                                                                                                    \
        HI_ALERT_TRACE(HI_ID_ISP, "[Func]:%s [Line]:%d [Info]:" fmt, __FUNCTION__, __LINE__, ##__VA_ARGS__); \
    } while (0)

#define ISP_CRIT_TRACE(fmt, ...)                                                                            \
    do {                                                                                                    \
        HI_CRIT_TRACE(HI_ID_ISP, "[Func]:%s [Line]:%d [Info]:" fmt, __FUNCTION__, __LINE__, ##__VA_ARGS__); \
    } while (0)

#define ISP_ERR_TRACE(fmt, ...)                                                                            \
    do {                                                                                                    \
        HI_ERR_TRACE(HI_ID_ISP, "[Func]:%s [Line]:%d [Info]:" fmt, __FUNCTION__, __LINE__, ##__VA_ARGS__); \
    } while (0)

#define ISP_WARN_TRACE(fmt, ...)                                                                            \
    do {                                                                                                    \
        HI_WARN_TRACE(HI_ID_ISP, "[Func]:%s [Line]:%d [Info]:" fmt, __FUNCTION__, __LINE__, ##__VA_ARGS__); \
    } while (0)

#define ISP_NOTICE_TRACE(fmt, ...)                                                                            \
    do {                                                                                                    \
        HI_NOTICE_TRACE(HI_ID_ISP, "[Func]:%s [Line]:%d [Info]:" fmt, __FUNCTION__, __LINE__, ##__VA_ARGS__); \
    } while (0)

#define ISP_INFO_TRACE(fmt, ...)                                                                            \
    do {                                                                                                    \
        HI_INFO_TRACE(HI_ID_ISP, "[Func]:%s [Line]:%d [Info]:" fmt, __FUNCTION__, __LINE__, ##__VA_ARGS__); \
    } while (0)

#define ISP_DEBUG_TRACE(fmt, ...)                                                                            \
    do {                                                                                                    \
        HI_DEBUG_TRACE(HI_ID_ISP, "[Func]:%s [Line]:%d [Info]:" fmt, __FUNCTION__, __LINE__, ##__VA_ARGS__); \
    } while (0)

/* To avoid divide-0 exception in code. */
#define DIV_0_TO_1(a)   ( ((a) == 0) ? 1 : (a) )
#define DIV_0_TO_1_FLOAT(a) ((((a) < 1E-10) && ((a) > -1E-10)) ? 1 : (a))

#endif     /* __HI_ISP_DEBUG_H__ */
