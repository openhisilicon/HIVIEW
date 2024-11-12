/*
  Copyright (c), 2001-2024, Shenshu Tech. Co., Ltd.
 */

/**
 * @defgroup osal_atomic osal_atomic
 */
#ifndef __OSAL_ATOMIC_H__
#define __OSAL_ATOMIC_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

typedef struct {
    volatile int counter;
} osal_atomic;

/**
 * @ingroup osal_atomic
 * @brief read atomic variable
 *
 * @par Description:
 * This API is used to implement the atomic read and return the value read from the input parameter @atomic.
 *
 * @par Support System:
 * linux liteos freertos.
 */
int osal_atomic_read(osal_atomic *atomic);

/**
 * @ingroup osal_atomic
 * @brief set atomic variable
 *
 * @par Description:
 * This API is used to implement the atomic setting operation.
 *
 * @par Support System:
 * linux liteos freertos.
 */
void osal_atomic_set(osal_atomic *atomic, int i);

/**
 * @ingroup osal_atomic
 * @brief increment atomic variable and return
 *
 * @par Description:
 * This API is used to implement the atomic self-addition and return the self-addition result.
 *
 * @par Support System:
 * linux liteos freertos.
 */
int osal_atomic_inc_return(osal_atomic *atomic);

/**
 * @ingroup osal_atomic
 * @brief Adds the given value to the atomic variable and returns the increased result.
 *
 * @par Description:
 * Adds the given value to the atomic variable and returns the increased result.
 *
 * @param atomic  [in]  The atomic to be change.
 * @param count   [in]  Number of atomic variables to be add.
 *
 * @par Support System:
 * freertos.
 */
int osal_atomic_add_return(osal_atomic *atomic, unsigned int count);

/**
 * @ingroup osal_atomic
 * @brief decrement atomic variable and return
 *
 * @par Description:
 * This API is used to implement the atomic self-decrement and return the self-decrement result.
 *
 * @par Support System:
 * linux liteos freertos.
 */
int osal_atomic_dec_return(osal_atomic *atomic);

/**
 * @ingroup osal_atomic
 * @brief increment atomic variable
 *
 * @par Description:
 * This API is used to implement the atomic self-addition.
 *
 * @par Support System:
 * linux liteos freertos.
 */
void osal_atomic_inc(osal_atomic *atomic);

/**
 * @ingroup osal_atomic
 * @brief Reduce a fixed number of atomic variables.
 *
 * @par Description:
 * Reduce a fixed number of atomic variables
 *
 * @param atomic  [in]  The atomic to be change.
 * @param count   [in]  Number of atomic variables to be reduced.
 *
 * @par Support System:
 * freertos.
 */
void osal_atomic_sub(osal_atomic *atomic, unsigned int count);

/**
 * @ingroup osal_atomic
 * @brief decrement atomic variable
 *
 * @par Description:
 * This API is used to implement the atomic self-decrement.
 *
 * @par Support System:
 * linux liteos freertos.
 */
void osal_atomic_dec(osal_atomic *atomic);

/**
 * @ingroup osal_atomic
 * @brief Add a fixed number of atomic variables.
 *
 * @par Description:
 * Add a fixed number of atomic variables
 *
 * @param atomic  [in]  The atomic to be change.
 * @param count   [in]  Number of atomic variables to be add.
 *
 * @par Support System:
 * freertos.
 */
void osal_atomic_add(osal_atomic *atomic, unsigned int count);

/**
 * @ingroup osal_atomic
 * @brief decrement atomic variable and test
 *
 * @par Description:
 * This API is used to subtract 1 atomically from the variable @atomic of the atomic type and
 * checks whether the result is 0. If yes, true is returned. Otherwise, false is returned.
 *
 * @par Support System:
 * linux.
 */
int osal_atomic_dec_and_test(osal_atomic *atomic);

/**
 * @ingroup osal_atomic
 * @brief increment atomic variable and test
 *
 * @par Description:
 * This API is used to add 1 atomically from the variable @atomic of the atomic type and checks whether the result is 0.
 * If yes, true is returned. Otherwise, false is returned.
 *
 * @par Support System:
 * linux.
 */
int osal_atomic_inc_and_test(osal_atomic *atomic);

/**
 * @ingroup osal_atomic
 * @brief increment unless the number is zero
 *
 * @par Description:
 * Atomically increments @atomic by 1, if @atomic is non-zero.
 *
 * @return Returns true if the increment was done.
 *
 * @par Support System:
 * linux.
 */
int osal_atomic_inc_not_zero(osal_atomic *atomic);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
#endif /* __OSAL_ATOMIC_H__ */
