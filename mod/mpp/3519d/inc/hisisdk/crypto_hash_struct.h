/*
  Copyright (c), 2001-2024, Shenshu Tech. Co., Ltd.
 */

#ifndef CRYPTO_HASH_STRUCT_H
#define CRYPTO_HASH_STRUCT_H

#include "crypto_common_struct.h"

/*
 * Component of crypto_hash_type.
 * is_hmac(4 bits) | alg(4 bits) | mode(4 bits) | max_message_length(4 bits) | block_size(4 bits) | result_size(12 bits)
 * is_hmac: h'0 - hash
 *          h'1 - hmac
 * alg:     h'0 - sha1
 *          h'1 - sha2
 *          h'2 - sm3
 * mode:    h'0 - 224
 *          h'1 - 256
 *          h'2 - 384
 *          h'3 - 512
 * max_message_length:
 *          h'6 - 2**6 -> 64, longest 2**64 Bits. For SHA256.
 *          h'7 - 2**7 -> 128, longest 2**128 Bits. For SHA384/SHA512.
 * block_size:
 *          h'9 - 2**9 -> 512, Block Size is 512 Bits. For SHA256.
 *          h'a - 2**10 -> 1024, Block Size is 1024 Bits. For SHA384/SHA512.
 * result_size:
 *          h'100 - 256, Result Size is 256 Bits. For SHA256.
 *          h'180 - 384, Result Size is 384 Bits. For SHA384.
 *          h'200 - 512, Result Size is 512 Bits. For SHA512.
 */
#define compat_hash_type(is_hmac, alg, mode, max_message_length, block_size, result_size)          \
    ((((is_hmac) & 0xF) << 28) | (((alg) & 0xF) << 24) | (((mode) & 0xF) << 20) |                  \
    (((max_message_length) & 0xF) << 16) | (((block_size) & 0xF) << 12) | ((result_size) & 0xFFF))

#define CRYPTO_HASH_TYPE        0
#define CRYPTO_HMAC_TYPE        1
#define CRYPTO_IS_HMAC_MASK     0xF0000000
#define CRYPTO_IS_HMAC_SHIFT    28

#define CRYPTO_HASH_ALG_SHA1    0
#define CRYPTO_HASH_ALG_SHA2    1
#define CRYPTO_HASH_ALG_SM3     2
#define CRYPTO_HASH_ALG_MASK    0x0F000000
#define CRYPTO_HASH_ALG_SHIFT   24

#define CRYPTO_HASH_MODE_224    0
#define CRYPTO_HASH_MODE_256    1
#define CRYPTO_HASH_MODE_384    2
#define CRYPTO_HASH_MODE_512    3
#define CRYPTO_HASH_MODE_UNDEF  0xf
#define CRYPTO_HASH_MODE_MASK   0x00F00000
#define CRYPTO_HASH_MODE_SHIFT  20

#define CRYPTO_HASH_MAX_MESSAGE_LEN_64BIT   0x6
#define CRYPTO_HASH_MAX_MESSAGE_LEN_128BIT  0x7
#define CRYPTO_HASH_MAX_MESSAGE_LEN_MASK    0x000F0000
#define CRYPTO_HASH_MAX_MESSAGE_LEN_SHIFT   16

#define CRYPTO_HASH_BLOCK_SIZE_512BIT       0x9
#define CRYPTO_HASH_BLOCK_SIZE_1024BIT      0xa
#define CRYPTO_HASH_BLOCK_SIZE_MASK         0x0000F000
#define CRYPTO_HASH_BLOCK_SIZE_SHIFT        12

#define CRYPTO_HASH_RESULT_SIZE_160BIT      0xa0
#define CRYPTO_HASH_RESULT_SIZE_224BIT      0xe0
#define CRYPTO_HASH_RESULT_SIZE_256BIT      0x100
#define CRYPTO_HASH_RESULT_SIZE_384BIT      0x180
#define CRYPTO_HASH_RESULT_SIZE_512BIT      0x200
#define CRYPTO_HASH_RESULT_SIZE_MASK        0x00000FFF
#define CRYPTO_HASH_RESULT_SIZE_SHIFT       0

#define CRYPTO_GET_ATTR(value, mask, shift)         (((value) & (mask)) >> (shift))
#define CRYPTO_MATCH(value, mask, target, shift)    (CRYPTO_GET_ATTR(value, mask, shift) == (target))

#define CRYPTO_HASH_GET_ALG(hash_type)              \
    CRYPTO_GET_ATTR(hash_type, CRYPTO_HASH_ALG_MASK, CRYPTO_HASH_ALG_SHIFT)
#define CRYPTO_HASH_GET_MODE(hash_type)             \
    CRYPTO_GET_ATTR(hash_type, CRYPTO_HASH_MODE_MASK, CRYPTO_HASH_MODE_SHIFT)
#define CRYPTO_HASH_IS_HMAC(hash_type)              \
    CRYPTO_MATCH(hash_type, CRYPTO_IS_HMAC_MASK, CRYPTO_HMAC_TYPE, CRYPTO_IS_HMAC_SHIFT)
#define CRYPTO_HASH_GET_MESSAGE_LEN(hash_type)      \
    (1 << CRYPTO_GET_ATTR(hash_type, CRYPTO_HASH_MAX_MESSAGE_LEN_MASK, CRYPTO_HASH_MAX_MESSAGE_LEN_SHIFT))
#define CRYPTO_HASH_GET_BLOCK_SIZE(hash_type)       \
    (1 << CRYPTO_GET_ATTR(hash_type, CRYPTO_HASH_BLOCK_SIZE_MASK, CRYPTO_HASH_BLOCK_SIZE_SHIFT))
#define CRYPTO_HASH_GET_RESULT_SIZE(hash_type)      \
    CRYPTO_GET_ATTR(hash_type, CRYPTO_HASH_RESULT_SIZE_MASK, CRYPTO_HASH_RESULT_SIZE_SHIFT)

typedef enum {
    /* Hash. */
    CRYPTO_HASH_TYPE_SHA1 = compat_hash_type(
        CRYPTO_HASH_TYPE, CRYPTO_HASH_ALG_SHA1, CRYPTO_HASH_MODE_UNDEF, CRYPTO_HASH_MAX_MESSAGE_LEN_64BIT,
        CRYPTO_HASH_BLOCK_SIZE_512BIT, CRYPTO_HASH_RESULT_SIZE_160BIT
    ),
    CRYPTO_HASH_TYPE_SHA224 = compat_hash_type(
        CRYPTO_HASH_TYPE, CRYPTO_HASH_ALG_SHA2, CRYPTO_HASH_MODE_224, CRYPTO_HASH_MAX_MESSAGE_LEN_64BIT,
        CRYPTO_HASH_BLOCK_SIZE_512BIT, CRYPTO_HASH_RESULT_SIZE_224BIT
    ),
    CRYPTO_HASH_TYPE_SHA256 = compat_hash_type(
        CRYPTO_HASH_TYPE, CRYPTO_HASH_ALG_SHA2, CRYPTO_HASH_MODE_256, CRYPTO_HASH_MAX_MESSAGE_LEN_64BIT,
        CRYPTO_HASH_BLOCK_SIZE_512BIT, CRYPTO_HASH_RESULT_SIZE_256BIT
    ),
    CRYPTO_HASH_TYPE_SHA384 = compat_hash_type(
        CRYPTO_HASH_TYPE, CRYPTO_HASH_ALG_SHA2, CRYPTO_HASH_MODE_384, CRYPTO_HASH_MAX_MESSAGE_LEN_128BIT,
        CRYPTO_HASH_BLOCK_SIZE_1024BIT, CRYPTO_HASH_RESULT_SIZE_384BIT
    ),
    CRYPTO_HASH_TYPE_SHA512 = compat_hash_type(
        CRYPTO_HASH_TYPE, CRYPTO_HASH_ALG_SHA2, CRYPTO_HASH_MODE_512, CRYPTO_HASH_MAX_MESSAGE_LEN_128BIT,
        CRYPTO_HASH_BLOCK_SIZE_1024BIT, CRYPTO_HASH_RESULT_SIZE_512BIT
    ),
    CRYPTO_HASH_TYPE_SM3 = compat_hash_type(
        CRYPTO_HASH_TYPE, CRYPTO_HASH_ALG_SM3, CRYPTO_HASH_MODE_256, CRYPTO_HASH_MAX_MESSAGE_LEN_64BIT,
        CRYPTO_HASH_BLOCK_SIZE_512BIT, CRYPTO_HASH_RESULT_SIZE_256BIT
    ),

    /* HMAC. */
    CRYPTO_HASH_TYPE_HMAC_SHA1 = compat_hash_type(
        CRYPTO_HMAC_TYPE, CRYPTO_HASH_ALG_SHA1, CRYPTO_HASH_MODE_UNDEF, CRYPTO_HASH_MAX_MESSAGE_LEN_64BIT,
        CRYPTO_HASH_BLOCK_SIZE_512BIT, CRYPTO_HASH_RESULT_SIZE_160BIT
    ),
    CRYPTO_HASH_TYPE_HMAC_SHA224 = compat_hash_type(
        CRYPTO_HMAC_TYPE, CRYPTO_HASH_ALG_SHA2, CRYPTO_HASH_MODE_224, CRYPTO_HASH_MAX_MESSAGE_LEN_64BIT,
        CRYPTO_HASH_BLOCK_SIZE_512BIT, CRYPTO_HASH_RESULT_SIZE_224BIT
    ),
    CRYPTO_HASH_TYPE_HMAC_SHA256 = compat_hash_type(
        CRYPTO_HMAC_TYPE, CRYPTO_HASH_ALG_SHA2, CRYPTO_HASH_MODE_256, CRYPTO_HASH_MAX_MESSAGE_LEN_64BIT,
        CRYPTO_HASH_BLOCK_SIZE_512BIT, CRYPTO_HASH_RESULT_SIZE_256BIT
    ),
    CRYPTO_HASH_TYPE_HMAC_SHA384 = compat_hash_type(
        CRYPTO_HMAC_TYPE, CRYPTO_HASH_ALG_SHA2, CRYPTO_HASH_MODE_384, CRYPTO_HASH_MAX_MESSAGE_LEN_128BIT,
        CRYPTO_HASH_BLOCK_SIZE_1024BIT, CRYPTO_HASH_RESULT_SIZE_384BIT
    ),
    CRYPTO_HASH_TYPE_HMAC_SHA512 = compat_hash_type(
        CRYPTO_HMAC_TYPE, CRYPTO_HASH_ALG_SHA2, CRYPTO_HASH_MODE_512, CRYPTO_HASH_MAX_MESSAGE_LEN_128BIT,
        CRYPTO_HASH_BLOCK_SIZE_1024BIT, CRYPTO_HASH_RESULT_SIZE_512BIT
    ),
    CRYPTO_HASH_TYPE_HMAC_SM3 = compat_hash_type(
        CRYPTO_HMAC_TYPE, CRYPTO_HASH_ALG_SM3, CRYPTO_HASH_MODE_256, CRYPTO_HASH_MAX_MESSAGE_LEN_64BIT,
        CRYPTO_HASH_BLOCK_SIZE_512BIT, CRYPTO_HASH_RESULT_SIZE_256BIT
    ),

    CRYPTO_HASH_TYPE_INVALID = 0xffffffff,
} crypto_hash_type;

typedef struct {
    td_u8 *key;
    td_u32 key_len;
    td_handle drv_keyslot_handle;
    crypto_hash_type hash_type;
    td_bool is_keyslot;
    td_bool is_long_term;
} crypto_hash_attr;

/* Structure for HASH. */
#define CRYPTO_HASH_RESULT_SIZE_MAX 64              // for SHA-512
#define CRYPTO_HASH_RESULT_SIZE_MAX_IN_WORD 16      // for SHA-512
#define CRYPTO_HASH_BLOCK_SIZE_MAX 128              // for SHA-512

typedef struct {
    td_u32 length[2];
    td_u32 state[CRYPTO_HASH_RESULT_SIZE_MAX_IN_WORD];
    td_u32 tail_len;
    crypto_hash_type hash_type;
    td_u8 o_key_pad[CRYPTO_HASH_BLOCK_SIZE_MAX];
    td_u8 i_key_pad[CRYPTO_HASH_BLOCK_SIZE_MAX];
    td_u8 tail[CRYPTO_HASH_BLOCK_SIZE_MAX];
} crypto_hash_clone_ctx;

#endif
