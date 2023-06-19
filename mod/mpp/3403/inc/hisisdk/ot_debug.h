/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */

#ifndef __OT_DEBUG_H__
#define __OT_DEBUG_H__

#ifndef __KERNEL__
#include <stdio.h>
#include <stdarg.h>
#else
#include "ot_osal.h"
#endif

#include "ot_type.h"
#include "ot_common.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

#define _EX__FILE_LINE(fxx, lxx) "[File]:"fxx"\n[Line]:"#lxx"\n[Info]:"
#define EX__FILE_LINE(fxx, lxx) _EX__FILE_LINE(fxx, lxx)
#define __FILE_LINE__ EX__FILE_LINE(__FILE__, __LINE__)
#define OT_MAX_MOD_NAME_LEN 16

#define OT_DBG_EMERG      0   /* system is unusable                   */
#define OT_DBG_ALERT      1   /* action must be taken immediately     */
#define OT_DBG_CRIT       2   /* critical conditions                  */
#define OT_DBG_ERR        3   /* error conditions                     */
#define OT_DBG_WARN       4   /* warning conditions                   */
#define OT_DBG_NOTICE     5   /* normal but significant condition     */
#define OT_DBG_INFO       6   /* informational                        */
#define OT_DBG_DEBUG      7   /* debug-level messages                 */

#ifndef __KERNEL__
/*
 * For User Mode : OT_PRINT, ot_assert, OT_TRACE
 */
#define OT_PRINT      printf

#define ot_assert(expr)
/* #ifdef OT_DEBUG */
#ifdef CONFIG_OT_LOG_TRACE_SUPPORT

    /*
     * Using samples:
     * OT_TRACE(OT_DBG_DEBUG, OT_ID_CMPI, "Test %d, %s\n", 12, "Test");
     */
    #define OT_TRACE(level, enModId, fmt...) fprintf(stderr, ##fmt)
#else
    #define OT_TRACE(level, enModId, fmt...)
#endif

#else
/*
 * For Linux Kernel : OT_PRINT, ot_assert, OT_TRACE
 */
#define OT_PRINT      osal_printk

int OT_LOG(td_s32 level, ot_mod_id enModId, const char *fmt, ...) __attribute__((format(printf, 3, 4)));

/* #ifdef OT_DEBUG */
#ifdef CONFIG_OT_LOG_TRACE_SUPPORT

#ifdef OT_DEBUG
#define ot_assert_log   osal_panic
#else
#define ot_assert_log   osal_printk
#endif
/* Using samples:   ot_assert(x>y); */
#define ot_assert(expr)                    \
    do {                                   \
        if (!(expr)) {                     \
            ot_assert_log("\nASSERT at:\n" \
                  "  >Function : %s\n"     \
                  "  >Line No. : %d\n"     \
                  "  >Condition: %s\n",    \
                  __FUNCTION__, __LINE__, #expr); \
        } \
    } while (0)

    /*
     * Using samples:
     * OT_TRACE(OT_DBG_DEBUG, OT_ID_CMPI, "Test %d, %s\n", 12, "Test");
     */
#define OT_TRACE OT_LOG
#else
#define ot_assert(expr)
#define OT_TRACE(level, enModId, fmt...)
#endif

#endif /* end of __KERNEL__ */

#if (CONFIG_OT_LOG_TRACE_LEVEL >= OT_DBG_EMERG)

#define OT_EMERG_TRACE(mod, fmt...) OT_TRACE(OT_DBG_EMERG, mod, fmt)

#else
#define OT_EMERG_TRACE(mod, fmt...)
#endif

#if (CONFIG_OT_LOG_TRACE_LEVEL >= OT_DBG_ALERT)

#define OT_ALERT_TRACE(mod, fmt...) OT_TRACE(OT_DBG_ALERT, mod, fmt)

#else
#define OT_ALERT_TRACE(mod, fmt...)
#endif

#if (CONFIG_OT_LOG_TRACE_LEVEL >= OT_DBG_CRIT)

#define OT_CRIT_TRACE(mod, fmt...) OT_TRACE(OT_DBG_CRIT, mod, fmt)

#else
#define OT_CRIT_TRACE(mod, fmt...)
#endif

#if (CONFIG_OT_LOG_TRACE_LEVEL >= OT_DBG_ERR)

#define OT_ERR_TRACE(mod, fmt...) OT_TRACE(OT_DBG_ERR, mod, fmt)

#else
#define OT_ERR_TRACE(mod, fmt...)
#endif

#if (CONFIG_OT_LOG_TRACE_LEVEL >= OT_DBG_WARN)

#define OT_WARN_TRACE(mod, fmt...) OT_TRACE(OT_DBG_WARN, mod, fmt)

#else
#define OT_WARN_TRACE(mod, fmt...)
#endif

#if (CONFIG_OT_LOG_TRACE_LEVEL >= OT_DBG_NOTICE)

#define OT_NOTICE_TRACE(mod, fmt...) OT_TRACE(OT_DBG_NOTICE, mod, fmt)

#else
#define OT_NOTICE_TRACE(mod, fmt...)
#endif

#if (CONFIG_OT_LOG_TRACE_LEVEL >= OT_DBG_INFO)

#define OT_INFO_TRACE(mod, fmt...) OT_TRACE(OT_DBG_INFO, mod, fmt)

#else
#define OT_INFO_TRACE(mod, fmt...)
#endif

#if (CONFIG_OT_LOG_TRACE_LEVEL >= OT_DBG_DEBUG)

#define OT_DEBUG_TRACE(mod, fmt...) OT_TRACE(OT_DBG_DEBUG, mod, fmt)

#else
#define OT_DEBUG_TRACE(mod, fmt...)
#endif

#define MPI_STATIC

typedef struct {
    ot_mod_id  mod_id;
    td_s32  level;
    td_char mod_name[OT_MAX_MOD_NAME_LEN];
} ot_log_level_cfg;

#ifndef __KERNEL__

#else

int ot_log(td_s32 level, ot_mod_id mod_id, const char *fmt, ...) __attribute__((format(printf, 3, 4)));

#endif /* end of __KERNEL__ */

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __OT_DEBUG_H__ */
