/*
  Copyright (c), 2001-2024, Shenshu Tech. Co., Ltd.
 */

/**
 * @defgroup osal_msgqueue osal_msgqueue
 */
#ifndef __OSAL_MSGQUEUE_H__
#define __OSAL_MSGQUEUE_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#ifdef LOS_WAIT_FOREVER
#define OSAL_MSGQ_WAIT_FOREVER LOS_WAIT_FOREVER
#else
#define OSAL_MSGQ_WAIT_FOREVER 0xFFFFFFFF
#endif

#ifdef LOS_NO_WAIT
#define OSAL_MSGQ_NO_WAIT LOS_NO_WAIT
#else
#define OSAL_MSGQ_NO_WAIT 0
#endif

/**
 * @ingroup osal_msgqueue
 * @brief Create a message queue.
 *
 * @par Description:
 * This API is used to create a message queue.
 *
 * @attention
 * There are LOSCFG_BASE_IPC_QUEUE_LIMIT queues available, change it's value when necessary.
 * This function is defined only when LOSCFG_QUEUE_DYNAMIC_ALLOCATION is defined.
 * The input parameter @queue_id is used as an address in the FreeRtos system,
 * and the input parameter @queue_id is used as an integer in the LiteOS system.
 * parameter @queue_id is created by osal_msg_queue_create.
 *
 * @param name        [in]  Message queue name. Reserved parameter, not used for now.
 * @param queue_len   [in]  Queue length. The value range is [1,0xffff].
 * @param queue_id    [out] ID of the queue control structure that is successfully created.
 * @param flags       [in]  Queue mode. Reserved parameter, not used for now.
 * @param max_msgsize [in]  Node size. The value range is [1,0xffff].
 *
 * @return OSAL_SUCCESS/OSAL_FAILURE
 *
 * @par Support System:
 * liteos freertos.
 */
int osal_msg_queue_create(const char *name, unsigned short queue_len, unsigned long *queue_id, unsigned int flags,
    unsigned short max_msgsize);

/**
 * @ingroup osal_msgqueue
 * @brief Write data into a queue.
 *
 * @par Description:
 * This API is used to write the data of the size specified by buffer_size and stored at the address specified by
 * buffer_addr into a queue.
 *
 * @attention
 * The specific queue should be created firstly.
 * Do not read or write a queue in unblocking modes such as interrupt.
 * This API cannot be called before the LiteOS is initialized.
 * The data to be written is of the size specified by buffer_size and is stored at the address specified by buffer_addr.
 * The argument timeout is a relative time.
 * Do not call this API in software timer callback.
 * The buffer_size parameter is not used in the freertos system and
 * buffer length depends on the size transferred during creation.
 * The input parameter @queue_id is used as an address in the FreeRtos system,
 * and the input parameter @queue_id is used as an integer in the LiteOS system.
 * parameter @queue_id is created by osal_msg_queue_create.
 *
 * @param queue_id    [in] Queue ID created by osal_msg_queue_create.
 * @param buffer_addr [in] Starting address that stores the data to be written.
 * The starting address must not be null.
 * @param buffer_size [in] Passed-in buffer size.
 * @param timeout     [in] Expiry time. (unit: Tick).
 *
 * @return OSAL_SUCCESS/OSAL_FAILURE
 *
 * @par Support System:
 * liteos freertos
 * @par System Differ: The bufferSize of freertos does not support read/write of a specified size,
 * only supports full storage and rounding.
 */
int osal_msg_queue_write_copy(unsigned long queue_id, void *buffer_addr, unsigned int buffer_size,
    unsigned int timeout);

/**
 * @ingroup osal_msgqueue
 * @brief Read a queue.
 *
 * @par Description:
 * This API is used to read data in a specified queue, and store the obtained data to the address specified
 * by buffer_addr. The address and the size of the data to be read are defined by users.
 *
 * @attention
 * The specific queue should be created firstly.
 * Queue reading adopts the fist in first out (FIFO) mode. The data that is first stored in the queue is read first.
 * buffer_addr stores the obtained data.
 * Do not read or write a queue in unblocking modes such as an interrupt.
 * This API cannot be called before the LiteOS is initialized.
 * The argument timeout is a relative time.
 * Do not call this API in software timer callback.
 * The buffer_size parameter is not used in the freertos system and
 * buffer length depends on the size transferred during creation.
 * The input parameter @queue_id is used as an address in the FreeRtos system,
 * and the input parameter @queue_id is used as an integer in the LiteOS system.
 * parameter @queue_id is created by osal_msg_queue_create.
 *
 * @param queue_id    [in] Queue ID created by osal_msg_queue_create.
 * @param buffer_addr [out] Starting address that stores the data to be written.
 * The starting address must not be null.
 * @param buffer_size [in/out] Where to maintain the buffer wanted-size before read, and the real-size after read.
 * @param timeout     [in] Expiry time. (unit: Tick).
 *
 * @return OSAL_SUCCESS/OSAL_FAILURE
 *
 * @par Support System:
 * liteos freertos
 * @par System Differ: The bufferSize of freertos does not support read/write of a specified size,
 * only supports full storage and rounding.
 */
int osal_msg_queue_read_copy(unsigned long queue_id, void *buffer_addr, unsigned int *buffer_size,
    unsigned int timeout);

/**
 * @ingroup osal_msgqueue
 * @brief Write data into a queue header.
 *
 * @par Description:
 * This API is used to write the data of the size specified by bufferSize and stored at the address specified by
 * buffer_addr into a queue header.
 *
 * @attention
 * Do not read or write a queue in unblocking modes such as an interrupt.
 * This API cannot be called before the LiteOS is initialized.
 * The address of the data of the size specified by bufferSize and stored at the address specified
 * by bufferAddr is to be written.
 * The argument timeout is a relative time.
 * Do not call this API in software timer callback.
 * The buffer_size parameter is not used in the freertos system and
 * buffer length depends on the size transferred during creation.
 * The input parameter @queue_id is used as an address in the FreeRtos system,
 * and the input parameter @queue_id is used as an integer in the LiteOS system.
 * parameter @queue_id is created by osal_msg_queue_create.
 *
 * @param queue_id    [in] Queue ID created by osal_msg_queue_create.
 * @param buffer_addr [out] Starting address that stores the data to be written.
 * The starting address must not be null.
 * @param buffer_size [in] Passed-in buffer size, which must not be 0. The value range is [1,0xffffffff].
 * @param timeout     [in] Expiry time. (unit: Tick).
 *
 * @return OSAL_SUCCESS/OSAL_FAILURE
 *
 * @par Support System:
 * liteos freertos
 * @par System Differ: The bufferSize of freertos does not support read/write of a specified size,
 * only supports full storage and rounding.
 */
int osal_msg_queue_write_head_copy(unsigned long queue_id, void *buffer_addr, unsigned int buffer_size,
    unsigned int timeout);

/**
 * @ingroup osal_msgqueue
 * @brief Delete a queue.
 *
 * @par Description:
 * This API is used to delete a queue.
 *
 * @attention
 * This API cannot be used to delete a queue that is not created.
 * A synchronous queue fails to be deleted if any tasks are blocked on it, or some queues are being read or written.
 * The input parameter @queue_id is used as an address in the FreeRtos system,
 * and the input parameter @queue_id is used as an integer in the LiteOS system.
 * parameter @queue_id is created by osal_msg_queue_create.
 *
 * @param queue_id    [in] Queue ID created by osal_msg_queue_create..
 *
 * @par Support System:
 * liteos freertos
 */
void osal_msg_queue_delete(unsigned long queue_id);

/**
 * @ingroup osal_msgqueue
 * @brief Check whether the message queue is full.
 *
 * @par Description:
 * This API is used to check whether the message queue is full.
 *
 * @attention
 * The specific queue should be created firstly.
 * The input parameter @queue_id is used as an address in the FreeRtos system,
 * and the input parameter @queue_id is used as an integer in the LiteOS system.
 * parameter @queue_id is created by osal_msg_queue_create.
 *
 * @param queue_id [in] Queue ID created by osal_msg_queue_create.
 *
 * @return true/false
 *
 * @par Support System:
 * liteos freertos
 */
int osal_msg_queue_is_full(unsigned long queue_id);

/**
 * @ingroup osal_msgqueue
 * @brief Obtains the number of messages in the current message queue.
 *
 * @par Description:
 * This API is used to obtains the number of messages in the current message queue.
 *
 * @attention
 * The input parameter @queue_id is used as an address in the FreeRtos system,
 * and the input parameter @queue_id is used as an integer in the LiteOS system.
 * parameter @queue_id is created by osal_msg_queue_create.
 *
 * @param queue_id [in] Queue ID created by osal_msg_queue_create
 *
 * @return The number of messages in the current message queue. or OSAL_INVALID_MSG_NUM when get number failed.
 *
 * @par Support System:
 * liteos freertos
 */
unsigned int osal_msg_queue_get_msg_num(unsigned long queue_id);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
#endif /* __OSAL_MSGQUEUE_H__ */