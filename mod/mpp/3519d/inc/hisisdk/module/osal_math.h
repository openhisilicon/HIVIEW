/*
  Copyright (c), 2001-2024, Shenshu Tech. Co., Ltd.
 */

/**
 * @defgroup osal_math osal_math
 */
#ifndef __OSAL_MATH_H__
#define __OSAL_MATH_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/**
 * @ingroup osal_math
 * @brief Unsigned 64bit divide with 32bit divisor.
 *
 * @par Description:
 * Unsigned 64bit divide with 32bit divisor.
 *
 * @param dividend [in] unsigned 64bit dividend.
 * @param divisor [in] unsigned 32bit divisor.
 *
 * @return Returns dividend / divisor.
 *
 * @attention Caller should ensure divisor not be zero
 *
 * @par Support System:
 * linux liteos.
 */
unsigned long long osal_div_u64(unsigned long long dividend, unsigned int divisor);

/**
 * @ingroup osal_math
 * @brief Signed 64bit divide with 32bit divisor.
 *
 * @par Description:
 * Signed 64bit divide with 32bit divisor.
 *
 * @param dividend [in] signed 64bit dividend.
 * @param divisor [in] signed 32bit divisor.
 *
 * @return Returns dividend / divisor.
 *
 * @attention Caller should ensure divisor not be zero
 *
 * @par Support System:
 * linux liteos.
 */
long long osal_div_s64(long long dividend, int divisor);

/**
 * @ingroup osal_math
 * @brief Unsigned 64bit divide with Unsigned 64bit divisor.
 *
 * @par Description:
 * Unsigned 64bit divide with Unsigned 64bit divisor.
 *
 * @param dividend [in] unsigned 64bit dividend.
 * @param divisor [in] unsigned 64bit divisor.
 *
 * @return Returns dividend / divisor.
 *
 * @attention Caller should ensure divisor not be zero
 *
 * @par Support System:
 * linux liteos.
 */
unsigned long long osal_div64_u64(unsigned long long dividend, unsigned long long divisor);

/**
 * @ingroup osal_math
 * @brief Signed 64bit divide with 64bit divisor.
 *
 * @par Description:
 * Signed 64bit divide with 64bit divisor.
 *
 * @param dividend [in] signed 64bit dividend.
 * @param divisor [in] signed 64bit divisor.
 *
 * @return Returns dividend / divisor.
 *
 * @attention Caller should ensure divisor not be zero
 *
 * @par Support System:
 * linux liteos.
 */
long long osal_div64_s64(long long dividend, long long divisor);

/**
 * @ingroup osal_math
 * @brief Unsigned 64bit divide with 32bit divisor with remainder.
 *
 * @par Description:
 * Unsigned 64bit divide with 32bit divisor with remainder.
 *
 * @param dividend [in] unsigned 64bit dividend.
 * @param divisor [in] unsigned 32bit divisor.
 *
 * @return Returns the remainder of dividend / divisor.
 *
 * @attention Caller should ensure divisor not be zero
 *
 * @par Support System:
 * linux liteos.
 */
unsigned long long osal_div_u64_rem(unsigned long long dividend, unsigned int divisor);

/**
 * @ingroup osal_math
 * @brief Signed 64bit divide with 32bit divisor with remainder.
 *
 * @par Description:
 * Signed 64bit divide with 32bit divisor with remainder.
 *
 * @param dividend [in] signed 64bit dividend.
 * @param divisor [in] signed 32bit divisor.
 *
 * @return Returns the remainder of dividend / divisor.
 *
 * @attention Caller should ensure divisor not be zero
 *
 * @par Support System:
 * linux liteos.
 */
long long osal_div_s64_rem(long long dividend, int divisor);

/**
 * @ingroup osal_math
 * @brief Unsigned 64bit divide with unsigned 64bit divisor with remainder.
 *
 * @par Description:
 * Unsigned 64bit divide with unsigned 64bit divisor with remainder.
 *
 * @param dividend [in] unsigned 64bit dividend.
 * @param divisor [in] unsigned 64bit divisor.
 *
 * @return Returns the remainder of dividend / divisor.
 *
 * @attention Caller should ensure divisor not be zero
 *
 * @par Support System:
 * linux liteos.
 */
unsigned long long osal_div64_u64_rem(unsigned long long dividend, unsigned long long divisor);

/**
 * @ingroup osal_math
 * @brief Random number generator.
 *
 * @par Description:
 * Random number generator.
 *
 * @return Returns random number.
 *
 * @par Support System:
 * linux liteos.
 */
unsigned int osal_get_random_int(void);

#define osal_max(x, y)                 \
    ({                                 \
        __typeof__(x)_max1 = (x);      \
        __typeof__(y)_max2 = (y);      \
        (void)(&_max1 == &_max2);      \
        _max1 > _max2 ? _max1 : _max2; \
    })

#define osal_min(x, y)                 \
    ({                                 \
        __typeof__(x)_min1 = (x);      \
        __typeof__(y)_min2 = (y);      \
        (void)(&_min1 == &_min2);      \
        _min1 < _min2 ? _min1 : _min2; \
    })

#define osal_abs(x)                           \
    ({                                        \
        long ret;                             \
        if (sizeof(x) == sizeof(long)) {      \
            long __x = (x);                   \
            ret = (__x < 0) ? (-__x) : (__x); \
        } else {                              \
            int __x = (x);                    \
            ret = (__x < 0) ? (-__x) : (__x); \
        }                                     \
        ret;                                  \
    })

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
#endif /* __OSAL_MATH_H__ */