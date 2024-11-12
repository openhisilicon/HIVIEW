/*
  Copyright (c), 2001-2024, Shenshu Tech. Co., Ltd.
 */

#ifndef __OSAL_EVENT_H__
#define __OSAL_EVENT_H__

/**
 * @defgroup osal_event osal_event
 */
#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/* timeout forever macro */
#define OSAL_EVENT_FOREVER 0xFFFFFFFF

// Event reading mode: The task waits for all its expected events to occur.
#define OSAL_WAITMODE_AND 4U
// Event reading mode: The task waits for any of its expected events to occur.
#define OSAL_WAITMODE_OR 2U
// Event reading mode: The event flag is immediately cleared after the event is read.
#define OSAL_WAITMODE_CLR 1U

typedef struct {
    void *event;
} osal_event;

/**
 * @ingroup osal_event
 * @brief Initialize an event control block.
 *
 * @par Description:
 * This API is used to initialize the event control block pointed to by eventCB.
 *
 * @return OSAL_SUCCESS/OSAL_FAILURE
 *
 * @par Support System:
 * liteos freertos.
 */
int osal_event_init(osal_event *event);

/**
 * @ingroup osal_event
 * @brief Write an event.
 *
 * @par Description:
 * This API is used to write an event specified by the passed-in event mask into an event control block
 * pointed to by eventCB.
 *
 * @return OSAL_SUCCESS/OSAL_FAILURE
 *
 * @par Support System:
 * liteos freertos.
 */
int osal_event_write(osal_event *event, unsigned int event_bits);

/**
 * @ingroup osal_event
 * @brief Read an event.
 *
 * @par Description:
 * This API is used to block or schedule a task that reads an event of which the event control block, event mask,
 * reading mode, and timeout information are specified.
 *
 * @attention
 * Do not read event during an interrupt.
 * Do not recommend to use this API in software timer callback.
 * bit 25 of the event mask is forbidden to be used on liteos.
 *
 * @param event_obj [in/out] Pointer to the event control block to be checked.
 * This parameter must point to valid memory.
 * @param mask [in] Mask of the event expected to occur by the user, indicating the event obtained after
 * it is logically processed that matches the ID pointed to by eventId.
 * @param timeout [in] Timeout interval of event reading.(unit: ms)
 * @param mode [in] Event reading mode.
 *
 * @return OSAL_SUCCESS/OSAL_FAILURE
 *
 * @par Support System:
 * liteos freertos.
 */
int osal_event_read(osal_event *event_obj, unsigned int mask, unsigned int timeout_ms, unsigned int mode);

/**
 * @ingroup osal_event
 * @brief Clear the event occurring in a specified task.
 *
 * @par Description:
 * This API is used to set the ID of an event that has a specified mask and of which the information is stored in
 * an event control block pointed to by eventCB to 0. eventCB must point to valid memory.
 *
 * @param event_obj [in/out] Pointer to the event control block to be cleared.
 * @param event_bits [in] Mask of the event to be cleared.
 *
 * @return OSAL_SUCCESS/OSAL_FAILURE
 *
 * @par Support System:
 * liteos freertos.
 */
int osal_event_clear(osal_event *event_obj, unsigned int event_bits);

/**
 * @ingroup osal_event
 * @brief Destroy a event.
 *
 * @par Description:
 * This API is used to destroy a event.
 *
 * @param event_obj [in/out] Pointer to the event control block to be destroyed.
 *
 * @attention this api may free memory, event_obj should be from osal_event_init.
 *
 * @return OSAL_SUCCESS/OSAL_FAILURE
 *
 * @par Support System:
 * liteos freertos.
 */
int osal_event_destroy(osal_event *event_obj);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
#endif /* __OSAL_EVENT_H__ */