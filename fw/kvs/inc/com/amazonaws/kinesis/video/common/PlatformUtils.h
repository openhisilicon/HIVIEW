/**
 * Platform specific definitions for logging, assert, etc..
 */
#ifndef __PLATFORM_UTILS_DEFINES_H__
#define __PLATFORM_UTILS_DEFINES_H__

#ifdef __cplusplus
extern "C" {
#endif

#pragma once

// Tag for the logging
#ifndef LOG_CLASS
#define LOG_CLASS "platform-utils"
#endif

// Log print function definition
typedef VOID(*logPrintFunc) (UINT32, PCHAR, PCHAR, ...);

//
// Default logger function
//
PUBLIC_API INLINE VOID defaultLogPrint(UINT32 level, PCHAR tag, PCHAR fmt, ...);

extern logPrintFunc globalCustomLogPrintFn;

#ifdef ANDROID_BUILD
// Compiling with NDK
#include <android/log.h>
#define __LOG(p1, p2, p3, ...)     __android_log_print(p1, p2, p3, ##__VA_ARGS__)
#define __ASSERT(p1, p2, p3, ...)  __android_log_assert(p1, p2, p3, ##__VA_ARGS__)
#else
// Compiling under non-NDK
#include <stddef.h>
#include <stdlib.h>
#include <assert.h>
#define __ASSERT(p1, p2, p3, ...)  assert(p1)
#define __LOG globalCustomLogPrintFn
#endif // ANDROID_BUILD

#define LOG_LEVEL_VERBOSE           1
#define LOG_LEVEL_DEBUG             2
#define LOG_LEVEL_INFO              3
#define LOG_LEVEL_WARN              4
#define LOG_LEVEL_ERROR             5
#define LOG_LEVEL_FATAL             6
#define LOG_LEVEL_SILENT            7

#define LOG_LEVEL_VERBOSE_STR           (PCHAR) "VERBOSE"
#define LOG_LEVEL_DEBUG_STR             (PCHAR) "DEBUG"
#define LOG_LEVEL_INFO_STR              (PCHAR) "INFO"
#define LOG_LEVEL_WARN_STR              (PCHAR) "WARN"
#define LOG_LEVEL_ERROR_STR             (PCHAR) "ERROR"
#define LOG_LEVEL_FATAL_STR             (PCHAR) "FATAL"
#define LOG_LEVEL_SILENT_STR            (PCHAR) "SILENT"

// LOG_LEVEL_VERBOSE_STR string lenth
#define MAX_LOG_LEVEL_STRLEN            7

// Extra logging macros
#ifndef DLOGE
#define DLOGE(fmt, ...) __LOG(LOG_LEVEL_ERROR, (PCHAR) LOG_CLASS, (PCHAR) "%s(): " fmt, __FUNCTION__, ##__VA_ARGS__)
#endif
#ifndef DLOGW
#define DLOGW(fmt, ...) __LOG(LOG_LEVEL_WARN, (PCHAR) LOG_CLASS, (PCHAR) "%s(): " fmt, __FUNCTION__, ##__VA_ARGS__)
#endif
#ifndef DLOGI
#define DLOGI(fmt, ...) __LOG(LOG_LEVEL_INFO, (PCHAR) LOG_CLASS, (PCHAR) "%s(): " fmt, __FUNCTION__, ##__VA_ARGS__)
#endif
#ifndef DLOGD
#define DLOGD(fmt, ...) __LOG(LOG_LEVEL_DEBUG, (PCHAR) LOG_CLASS, (PCHAR) "%s(): " fmt, __FUNCTION__, ##__VA_ARGS__)
#endif
#ifndef DLOGV
#define DLOGV(fmt, ...) __LOG(LOG_LEVEL_VERBOSE, (PCHAR) LOG_CLASS, (PCHAR) "%s(): " fmt, __FUNCTION__, ##__VA_ARGS__)
#endif
#ifndef ENTER
#define ENTER() DLOGV("Enter")
#endif
#ifndef LEAVE
#define LEAVE() DLOGV("Leave")
#endif
#ifndef ENTERS
#define ENTERS() DLOGS("Enter")
#endif
#ifndef LEAVES
#define LEAVES() DLOGS("Leave")
#endif

// Optionally log very verbose data (>200 msgs/second) about the streaming process
#ifndef DLOGS
#ifdef LOG_STREAMING
    #define DLOGS DLOGV
#else
    #define DLOGS(...) do {} while (0)
#endif
#endif

// Assertions
#ifndef CONDITION
#ifdef __GNUC__
#define CONDITION(cond)     (__builtin_expect((cond) != 0, 0))
#else
#define CONDITION(cond)     ((cond) == TRUE)
#endif
#endif
#ifndef LOG_ALWAYS_FATAL_IF
#define LOG_ALWAYS_FATAL_IF(cond, ...) \
    ( (CONDITION(cond)) \
    ? ((void)__ASSERT(FALSE, (PCHAR) LOG_CLASS, ## __VA_ARGS__)) \
    : (void) 0 )
#endif

#ifndef LOG_ALWAYS_FATAL
#define LOG_ALWAYS_FATAL(...) \
    ( ((void)__ASSERT(FALSE, (PCHAR) LOG_CLASS, ## __VA_ARGS__)) )
#endif

#ifndef SANITIZED_FILE
#define SANITIZED_FILE (STRRCHR(__FILE__, '/') ? STRRCHR(__FILE__, '/') + 1 : __FILE__)
#endif
#ifndef CRASH
#define CRASH(fmt, ...) LOG_ALWAYS_FATAL("%s::%s: " fmt, (PCHAR) LOG_CLASS, __FUNCTION__, ##__VA_ARGS__)
#endif
#ifndef CHECK
#define CHECK(x) LOG_ALWAYS_FATAL_IF(!(x), "%s::%s: ASSERTION FAILED at %s:%d: " #x, (PCHAR) LOG_CLASS, __FUNCTION__, SANITIZED_FILE, __LINE__)
#endif
#ifndef CHECK_EXT
#define CHECK_EXT(x, fmt, ...) LOG_ALWAYS_FATAL_IF(!(x), "%s::%s: ASSERTION FAILED at %s:%d: " fmt, (PCHAR) LOG_CLASS, __FUNCTION__, SANITIZED_FILE, __LINE__, ##__VA_ARGS__)
#endif

#ifdef __cplusplus
}
#endif

#endif // __PLATFORM_UTILS_DEFINES_H__
