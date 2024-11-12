/*
  Copyright (c), 2001-2024, Shenshu Tech. Co., Ltd.
 */

/**
 * @defgroup osal_bitmap osal_bitmap
 */
#ifndef __OSAL_BITMAP_H__
#define __OSAL_BITMAP_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define OSAL_BITS_PER_BYTE 8
#define OSAL_BITS_PER_LONG (OSAL_BITS_PER_BYTE * sizeof(unsigned long))
#define OSAL_BIT_MASK(nr) (1UL << ((nr) % OSAL_BITS_PER_LONG))
#define OSAL_BIT_WORD(nr) ((nr) / OSAL_BITS_PER_LONG)
#define OSAL_BITS_TO_LONGS(nr) (((nr) + (OSAL_BITS_PER_LONG)-1) / (OSAL_BITS_PER_LONG))
#define OSAL_DECLARE_BITMAP(name, bits) unsigned long name[OSAL_BITS_TO_LONGS(bits)]
#define osal_for_each_set_bit(bit, addr, size)                               \
    for ((bit) = osal_bitmap_find_first_bit((addr), (size)); (bit) < (size); \
        (bit) = osal_bitmap_find_next_bit((addr), (size), (bit) + 1))

#define osal_for_each_clear_bit(bit, addr, size)                                  \
    for ((bit) = osal_bitmap_find_first_zero_bit((addr), (size)); (bit) < (size); \
        (bit) = osal_bitmap_find_next_zero_bit((addr), (size), (bit) + 1))

/**
 * @ingroup osal_bitmap
 * @brief Used to set bit for bitmap.
 *
 * @par Description:
 * Used to set bit for bitmap.
 *
 * @param nr [in] bit number in bit array.
 * @param addr [in] address of the bit array to be set.
 *
 * @par Support System:
 * linux.
 */
void osal_bitmap_set_bit(int nr, unsigned long *addr);

/**
 * @ingroup osal_bitmap
 * @brief Used to clear bit for bitmap.
 *
 * @par Description:
 * Used to clear bit for bitmap.
 *
 * @param nr [in] bit number in bit array.
 * @param addr [in] address of the bit array to be clear.
 *
 * @par Support System:
 * linux.
 */
void osal_bitmap_clear_bit(int nr, unsigned long *addr);

/**
 * @ingroup osal_bitmap
 * @brief Used to change bit for bitmap.
 *
 * @par Description:
 * Used to change bit for bitmap.
 *
 * @param nr [in] bit number in bit array.
 * @param addr [in] address of the bit array to be change.
 *
 * @par Support System:
 * linux.
 */
void osal_bitmap_change_bit(int nr, unsigned long *addr);

/**
 * @ingroup osal_bitmap
 * @brief Tests whether a given bit in an array of bits is set.
 *
 * @par Description:
 * Tests whether a given bit in an array of bits is set.
 *
 * @param nr [in] bit number in bit array.
 * @param addr [in] address of the bit array to be change.
 *
 * @return true/false
 *
 * @par Support System:
 * linux.
 */
int osal_bitmap_test_bit(int nr, unsigned long *addr);

/**
 * @ingroup osal_bitmap
 * @brief Set bit and return old value.
 *
 * @par Description:
 * Set bit and return old value.
 *
 * @param nr [in] bit number in bit array.
 * @param addr [in] address of the bit array to be change.
 *
 * @return return old value.
 *
 * @par Support System:
 * linux.
 */
int osal_bitmap_test_and_set_bit(int nr, unsigned long *addr);

/**
 * @ingroup osal_bitmap
 * @brief Clear bit and return old value.
 *
 * @par Description:
 * Clear bit and return old value.
 *
 * @param nr [in] bit number in bit array.
 * @param addr [in] address of the bit array to be change.
 *
 * @return return old value.
 *
 * @par Support System:
 * linux.
 */
int osal_bitmap_test_and_clear_bit(int nr, unsigned long *addr);

/**
 * @ingroup osal_bitmap
 * @brief Change bit and return old value.
 *
 * @par Description:
 * Change bit and return old value.
 *
 * @param nr [in] bit number in bit array.
 * @param addr [in] address of the bit array to be change.
 *
 * @return return old value.
 *
 * @par Support System:
 * linux.
 */
int osal_bitmap_test_and_change_bit(int nr, unsigned long *addr);

/**
 * @ingroup osal_bitmap
 * @brief find the first cleared bit in a memory region.
 *
 * @par Description:
 * find the first cleared bit in a memory region.
 *
 * @param name [in] The name to base the search on.
 * @param size [in] The bitmap size in bits.
 *
 * @return The position of first zero bit in *addr
 *
 * @par Support System:
 * linux.
 */
int osal_bitmap_find_first_zero_bit(const unsigned long *name, unsigned size);

/**
 * @ingroup osal_bitmap
 * @brief find the first set bit in a memory region.
 *
 * @par Description:
 * find the first set bit in a memory region.
 *
 * @param name [in] The name to base the search on.
 * @param size [in] The bitmap size in bits.
 *
 * @return The position of first set bit in *addr
 *
 * @par Support System:
 * linux.
 */
int osal_bitmap_find_first_bit(const unsigned long *name, unsigned size);

/**
 * @ingroup osal_bitmap
 * @brief find the next cleared bit in a memory region.
 *
 * @par Description:
 * find the next cleared bit in a memory region.
 *
 * @param name [in] The name to base the search on.
 * @param size [in] The bitmap size in bits.
 * @param offset [in] The bitnumber to start searching at.
 *
 * @return The position of next zero bit in *addr >= bit
 *
 * @par Support System:
 * linux.
 */
int osal_bitmap_find_next_zero_bit(const unsigned long *name, int size, int offset);

/**
 * @ingroup osal_bitmap
 * @brief find the next set bit in a memory region.
 *
 * @par Description:
 * find the next set bit in a memory region.
 *
 * @param name [in] The name to base the search on.
 * @param size [in] The bitmap size in bits.
 * @param offset [in] The bitnumber to start searching at.
 *
 * @return The position of next set bit in *addr >= bit
 *
 * @par Support System:
 * linux.
 */
int osal_bitmap_find_next_bit(const unsigned long *name, unsigned siz, int offset);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
#endif /* __OSAL_BITMAP_H__ */