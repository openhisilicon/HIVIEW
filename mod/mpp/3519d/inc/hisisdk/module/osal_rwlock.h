/*
  Copyright (c), 2001-2024, Shenshu Tech. Co., Ltd.
 */

/**
 * @defgroup osal_rwlock osal_rwlock
 */
#ifndef __OSAL_RWLOCK_H__
#define __OSAL_RWLOCK_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

typedef struct {
    void *rwlock;
} osal_rwlock;

/**
 * @ingroup osal_rwlock
 *
 * @brief We need to initialize when we need to.
 *
 * @par Description:
 * Read/write lock initialization.
 *
 * @return OSAL_FAILURE/OSAL_SUCCESS
 *
 * @par Support System:
 * linux.
 */
int osal_rwlock_init(osal_rwlock *rw_lock);

/**
 * @ingroup osal_rwlock
 *
 * @brief Lock it when you go in and read it.
 *
 * @par Description:
 * Read lock.
 *
 * @par Support System:
 * linux.
 */
void osal_rwlock_read_lock(osal_rwlock *rw_lock);

/**
 * @ingroup osal_rwlock
 *
 * @brief Unlock when you read it out.
 *
 * @par Description:
 * Read Unlock.
 *
 * @par Support System:
 * linux.
 */
void osal_rwlock_read_unlock(osal_rwlock *rw_lock);

/**
 * @ingroup osal_rwlock
 *
 * @brief Lock when you want to write.
 *
 * @par Description:
 * Write lock.
 *
 * @par Support System:
 * linux.
 */
void osal_rwlock_write_lock(osal_rwlock *rw_lock);

/**
 * @ingroup osal_rwlock
 *
 * @brief Unlock after writing.
 *
 * @par Description:
 * Write Unlock.
 *
 * @par Support System:
 * linux.
 */
void osal_rwlock_write_unlock(osal_rwlock *rw_lock);

/**
 * @ingroup osal_rwlock
 *
 * @brief We release rw_locks when we don't need it.
 *
 * @par Description:
 * Release Lock.
 *
 * @par Support System:
 * linux.
 */
void osal_rwlock_destory(osal_rwlock *rw_lock);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
#endif /* __OSAL_RWLOCK_H__ */