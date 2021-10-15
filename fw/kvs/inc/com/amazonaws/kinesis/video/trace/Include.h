/**
 * Trace Profiler main include file
 */
#ifndef __TRACE_PROFILER_INCLUDE_H__
#define __TRACE_PROFILER_INCLUDE_H__

#ifdef __cplusplus
extern "C" {
#endif

#pragma once
#include <com/amazonaws/kinesis/video/common/CommonDefs.h>
#include <com/amazonaws/kinesis/video/common/PlatformUtils.h>

/**
 * Definition of the handles
 */
typedef UINT64 TRACE_PROFILER_HANDLE;
typedef TRACE_PROFILER_HANDLE* PTRACE_PROFILER_HANDLE;

typedef UINT64 TRACE_HANDLE;
typedef TRACE_HANDLE* PTRACE_HANDLE;

/**
 * Invalid trace handle value
 */
#ifndef INVALID_TRACE_HANDLE_VALUE
#define INVALID_TRACE_HANDLE_VALUE ((TRACE_HANDLE) MAX_UINT64)
#endif

#ifndef IS_VALID_TRACE_HANDLE
#define IS_VALID_TRACE_HANDLE(h) ((h) != INVALID_TRACE_HANDLE_VALUE)
#endif

/**
 * This is a sentinel indicating an invalid allocation handle value
 */
#ifndef INVALID_TRACE_PROFILER_HANDLE
#define INVALID_TRACE_PROFILER_HANDLE ((TRACE_PROFILER_HANDLE) INVALID_PIC_HANDLE_VALUE)
#endif

#ifndef IS_VALID_TRACE_PROFILER_HANDLE
#define IS_VALID_TRACE_PROFILER_HANDLE(h) ((h) != INVALID_TRACE_PROFILER_HANDLE)
#endif

/**
 * Trace profiler flags
 */
typedef enum
{
    /**
     * No flags specified. Used as a sentinel
     */
    PROFILER_FLAGS_NONE = 0x0,

    /**
     * Whether to use the AIV specific formatting
     */
    FLAGS_USE_AIV_TRACE_PROFILER_FORMAT = 0x1 << 0,

} TRACE_PROFILER_BEHAVIOR_FLAGS;


/**
 * Format flags mask - currently only AIV format is supported - Binary OR any other flags in the future
 */
#define PROFILER_FORMAT_MASK (UINT32)FLAGS_USE_AIV_TRACE_PROFILER_FORMAT

/**
 * Trace levels enum - matches Android profiler implementation in com.amazon.avod.perf
 */
typedef enum
{
    TRACE_LEVEL_REPORT_ALWAYS = 0,
    TRACE_LEVEL_CRITICAL,
    TRACE_LEVEL_INFO,
    TRACE_LEVEL_DEBUG,
    TRACE_LEVEL_VERBOSE,
    TRACE_LEVEL_DISABLED = 0xFF
} TRACE_LEVEL;

/**
 * Error values
 */
#define STATUS_TP_BASE                                          0x010100000
#define STATUS_MIN_PROFILER_BUFFER                              STATUS_TP_BASE + 0x00000001

////////////////////////////////////////////////////////////////////
// Forward declaration of the functions
////////////////////////////////////////////////////////////////////
/**
 * Trace function prototypes
 */
typedef STATUS (*TraceStartFunc)(TRACE_PROFILER_HANDLE, PCHAR, TRACE_LEVEL, PTRACE_HANDLE);
typedef STATUS (*TraceStopFunc)(TRACE_PROFILER_HANDLE, TRACE_HANDLE);

/**
 * Definition macros making it easier for function declarations/definitions
 */
#define DEFINE_TRACE_START(name)            STATUS name(TRACE_PROFILER_HANDLE traceProfilerHandle, PCHAR traceName, TRACE_LEVEL traceLevel, PTRACE_HANDLE pTraceHandle)
#define DEFINE_TRACE_STOP(name)             STATUS name(TRACE_PROFILER_HANDLE traceProfilerHandle, TRACE_HANDLE traceHandle)


//////////////////////////////////////////////////////////////////////////
// Public functions
//////////////////////////////////////////////////////////////////////////
/**
 * Creates and initializes the trace profiler.
 *
 * Parameters:
 *
 *      @bufferSize Internal buffer size of the profiler used to store the traces. The actual allocation will be slightly larger.
 *      @traceLevel Initial trace level.
 *      @behaviorFlags Trace profiler behavior flags
 *
 *      @pTraceProfilerHandle OUT_PARAM The newly created trace profiler.
 */
PUBLIC_API STATUS profilerInitialize(UINT32 bufferSize, TRACE_LEVEL traceLevel, TRACE_PROFILER_BEHAVIOR_FLAGS behaviorFlags, PTRACE_PROFILER_HANDLE pTraceProfilerHandle);

/**
 * Releases the entire trace profiler object.
 *
 * Parameters:
 *
 *      @traceProfilerHandle Trace profiler object.
 */
PUBLIC_API STATUS profilerRelease(TRACE_PROFILER_HANDLE traceProfilerHandle);

/**
 * Sets the profiling level - can be used for enabling/disabling.
 *
 * IMPORTANT!!! If the level was enabled for a given trace it will be reported even if the level gets set to disabled before reporting
 *
 * Parameters:
 *
 *      @traceProfilerHandle Trace profiler object.
 *      @traceLevel New trace level to use. Can be used for enabling/disabling profiler
 */
PUBLIC_API STATUS setProfilerLevel(TRACE_PROFILER_HANDLE traceProfilerHandle, TRACE_LEVEL traceLevel);

/**
 * Gets the formatted output of the collected traces. The formatting is dependent on the profiler flags.
 *
 * IMPORTANT!!! The returned buffer pointer CAN be NULL if AIV formatting is specified and there are no traces reported.
 *
 * Parameters:
 *
 *      @traceProfilerHandle Trace profiler object.
 *      @ppBuffer OUT_PARAM Can not be NULL. The buffer will be allocated by the trace profiler and returned in this pointer.
 *      @pBufferSize OUT_OPT_PARAM The buffer size.
 */
PUBLIC_API STATUS getFormattedTraceBuffer(TRACE_PROFILER_HANDLE traceProfilerHandle, PCHAR* ppBuffer, PUINT32 pBufferSize);

/**
 * Frees the allocated trace buffer.
 *
 * Parameters:
 *
 *      @pBuffer IN_PARAM The previously returned trace buffer to be released.
 */
PUBLIC_API STATUS freeTraceBuffer(PCHAR pBuffer);

/**
 * Creates and initializes and starts trace object.
 *
 * IMPORTANT!!! This will be a No-op if the trace level is disabled
 *
 *
 * Parameters:
 *
 *      @traceProfilerHandle Trace profiler object.
 *      @traceName The name of the newly initialized trace.
 *      @traceLevel Trace level which will be used to match against the profiler trace level.
 *
 *      @pTraceHandle OUT_PARAM The newly initialized trace object.
 */
PUBLIC_API DEFINE_TRACE_START(traceStart);

/**
 * Stops the specified trace.
 *
 * IMPORTANT!!! This will be a No-op if the trace level is disabled
 *
 * Parameters:
 *
 *      @traceProfilerHandle Trace profiler object.
 *      @traceHandle traceHandle to stop the trace for.
 */
PUBLIC_API DEFINE_TRACE_STOP(traceStop);

/**
 * Including the headers
 */
#include <com/amazonaws/kinesis/video/utils/Include.h>

#ifdef __cplusplus
}
#endif

#endif // __TRACE_PROFILER_INCLUDE_H__
