/**
 * Main public include file
 */
#ifndef __CONTENT_STATE_INCLUDE__
#define __CONTENT_STATE_INCLUDE__

#pragma once

#ifdef  __cplusplus
extern "C" {
#endif

#include <com/amazonaws/kinesis/video/common/CommonDefs.h>
#include <com/amazonaws/kinesis/video/common/PlatformUtils.h>

// IMPORTANT! Some of the headers are not tightly packed!
////////////////////////////////////////////////////
// Public headers
////////////////////////////////////////////////////
#include <com/amazonaws/kinesis/video/mkvgen/Include.h>

////////////////////////////////////////////////////
// Status return codes
////////////////////////////////////////////////////
#define STATUS_STATE_BASE                                                           0x52000000
#define STATUS_INVALID_STREAM_STATE                                                 STATUS_STATE_BASE + 0x0000000e
#define STATUS_STATE_MACHINE_STATE_NOT_FOUND                                        STATUS_STATE_BASE + 0x00000056

////////////////////////////////////////////////////
// Main structure declarations
////////////////////////////////////////////////////
/**
 * Service call retry timeout - 100ms
 */
#define SERVICE_CALL_RETRY_TIMEOUT                  (100 * HUNDREDS_OF_NANOS_IN_A_MILLISECOND)

/**
 * Indicates infinite retries
 */
#define INFINITE_RETRY_COUNT_SENTINEL               0

/**
 * State machine current version
 */
#define STATE_MACHINE_CURRENT_VERSION               0

/**
 * State transition function definitions
 *
 * @param 1 UINT64 - IN - Custom data passed in
 * @param 2 PUINT64 - OUT - Returned next state on success
 *
 * @return Status of the callback
 */
typedef STATUS (*GetNextStateFunc)(UINT64, PUINT64);

/**
 * State execution function definitions
 *
 * @param 1 - IN - Custom data passed in
 * @param 2 - IN - Delay time Time after which to execute the function
 *
 * @return Status of the callback
 */
typedef STATUS (*ExecuteStateFunc)(UINT64, UINT64);

/**
 * State Machine state
 */
typedef struct __StateMachineState StateMachineState;
struct __StateMachineState {
    UINT64 state;
    UINT64 acceptStates;
    GetNextStateFunc getNextStateFn;
    ExecuteStateFunc executeStateFn;
    UINT32 retry;
    STATUS status;
};
typedef struct __StateMachineState* PStateMachineState;

/**
 * State Machine definition
 */
typedef struct __StateMachine StateMachine;
struct __StateMachine {
    UINT32 version;
};
typedef struct __StateMachine* PStateMachine;

////////////////////////////////////////////////////
// Public functions
////////////////////////////////////////////////////

PUBLIC_API STATUS createStateMachine(PStateMachineState, UINT32, UINT64, GetCurrentTimeFunc, UINT64, PStateMachine*);
PUBLIC_API STATUS freeStateMachine(PStateMachine);
PUBLIC_API STATUS stepStateMachine(PStateMachine);
PUBLIC_API STATUS acceptStateMachineState(PStateMachine, UINT64);
PUBLIC_API STATUS getStateMachineState(PStateMachine, UINT64, PStateMachineState*);
PUBLIC_API STATUS getStateMachineCurrentState(PStateMachine, PStateMachineState*);
PUBLIC_API STATUS setStateMachineCurrentState(PStateMachine, UINT64);
PUBLIC_API STATUS resetStateMachineRetryCount(PStateMachine);

#ifdef  __cplusplus
}
#endif
#endif  /* __CONTENT_STATE_INCLUDE__ */
