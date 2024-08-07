/*
  Copyright (c), 2001-2024, Shenshu Tech. Co., Ltd.
 */

#ifndef CRYPTO_COMMON_DEF_H
#define CRYPTO_COMMON_DEF_H

#define MS_TO_US        1000

#define CRYPTO_BITS_IN_BYTE         8

#define CRYPTO_AES_IV_SIZE          16
#define CRYPTO_AES_IV_SIZE_IN_WORD  4
#define CRYPTO_WORD_WIDTH           4
#define CRYPTO_IV_LEN_IN_BYTES      16

#define CRYPTO_128_KEY_LEN          16
#define CRYPTO_192_KEY_LEN          24
#define CRYPTO_256_KEY_LEN          32

#define CRYPTO_AES_MAX_TAG_SIZE     16
#define CRYPTO_AES_CCM_N_LEN_IN_BYTES   16

#define CRYPTO_AES_BLOCK_SIZE_IN_BYTES  16

#define CRYPTO_BIT_3        3
#define CRYPTO_BIT_6        6
#define CRYPTO_BIT_8        8
#define CRYPTO_BIT_16       16
#define CRYPTO_BIT_24       24
#define CRYPTO_BYTE_1        1
#define CRYPTO_BYTE_2        2
#define CRYPTO_BYTE_3        3

#define CRYPTO_AES_CCM_NQ_LEN           14
#define CRYPTO_SYMC_CCM_Q_LEN_2B        2
#define CRYPTO_SYMC_CCM_Q_LEN_3B        3
#define CRYPTO_SYMC_CCM_Q_LEN_4B        4


#define CRYPTO_GCM_SPECIAL_IV_BYTES     12
#define CRYPTO_AES_MAX_TAG_LEN          16

#define CRYPTO_CENC_IV_LEN              8

#define CRYPTO_TYPE_ENCRYPT             0x0
#define CRYPTO_TYPE_DECRYPT             0x1
#define CRYPTO_TYPE_DMA                 0x2

#define CRYPTO_RSA_2048_LEN             256
#define CRYPTO_RSA_3072_LEN             384
#define CRYPTO_RSA_4096_LEN             512

/* Memory Limit. */
#define CRYPTO_MAX_AAD_SIZE         (4 * 1024)
#define CRYPTO_MAX_CRYPTO_SIZE      (4 * 1024)

/* PBKDF2 Limits. */
#define CRYPTO_PBKDF2_PASS_MAX_LENGTH   1024
#define CRYPTO_PBKDF2_SALT_MAX_LENGTH   1024
#define CRYPTO_PBKDF2_OUT_MAX_LENGTH    1024
#define CRYPTO_PBKDF2_MAX_COUNT         100000

/* Hash Limits. */
#define CRYPTO_HASH_KEY_MAX_LENGTH      128
#define CRYPTO_HASH_MIN_LEN             20
#define CRYPTO_HASH_MAX_LEN             64
#define CRYPTO_HASH_UPDATE_MAX_LEN      0xffff0000
#define CRYPTO_SHA256_BLOCK_SIZE        64

/* CENC Limits. */
#define CRYPTO_SYMC_CENC_SUBSAMPLE_MAX_SIZE     200

/* SYMC Limits. */
#define CRYPTO_SYMC_MULTI_PACK_MAX_SIZE         200
#define CRYPTO_SYMC_AAD_MAX_SIZE                (4 * 1024)
#define CRYPTO_SYMC_MAC_UPDATE_MAX_LEN          0xffff0000

/* PKE Limits. */
#define CRYPTO_PKE_ECC_KEY_MIN_SIZE             32
#define CRYPTO_PKE_ECC_KEY_MAX_SIZE             72
#define CRYPTO_PKE_RSA_KEY_MIN_SIZE             256
#define CRYPTO_PKE_RSA_KEY_MAX_SIZE             512

/* TRNG Limits. */
#define CRYPTO_MULTI_RANDOM_MAX_LENGTH          1024

#define CRYPTO_PKE_MSG_MIN_SIZE                 1
#define CRYPTO_PKE_MSG_MAX_SIZE                 (2 * 1024 * 1024)
#define CRYPTO_PKE_SM2_ID_MIN_SIZE              1
#define CRYPTO_PKE_SM2_ID_MAX_SIZE              1024
#define CRYPTO_PKE_SM2_PLAIN_TEXT_MIN_SIZE      1
#define CRYPTO_PKE_SM2_PLAIN_TEXT_MAX_SIZE      1024
#define CRYPTO_PKE_RSA_LABLE_MAX_SIZE           1024

#define KAPI_SYMC_MODULE_ID                 0x01
#define KAPI_HASH_MODULE_ID                 0x02
#define KAPI_KEYSLOT_MODULE_ID              0x03
#define KAPI_KLAD_MODULE_ID                 0x04
/*
 * Handle Process
 * Kapi Handle consists of the following parts:
 *|<---- SOC_ID_CIPHER(8-bit) --->|<---- Reserved(7-bit) --->|
 * <- Soft alg flag(1-bit) ->|<---- Module ID(8-bit) ---->|<---- Context Idx(8-bit)---->|
 */
#define crypto_set_soft_alg_flag(handle)                      ((handle) | 0x10000)
#define crypto_check_soft_alg_flag(handle)                    (((handle) >> 16) & 0x1)
#define crypto_get_soft_channel_id(handle)                    ((handle) & 0xff)

#define kapi_get_module_id(kapi_handle)                  (((kapi_handle) >> 8) & 0xff)
#define kapi_get_ctx_idx(kapi_handle)                    ((kapi_handle) & 0xff)
#define synthesize_kapi_handle(module_id, ctx_id)             (((0x33) << 24) | ((module_id) << 8) | (ctx_id))

/*
 *|<---- Reserved(8-bit) --->|<- keyslot_type(8-bit) ->|<---- Module ID(8-bit) ---->|<---- Context Idx(8-bit)---->|
 * keyslot_type: 0 - MCIPHER, 1 - HMAC
 */
#define crypto_keyslot_compat_handle(keyslot_type, idx) \
    (((keyslot_type) << 16) | ((KAPI_KEYSLOT_MODULE_ID) << 8) | (idx))
#define crypto_keyslot_compat_hmac_handle(idx)  crypto_keyslot_compat_handle(1, idx)
#define crypto_keyslot_compat_mcipher_handle(idx)  crypto_keyslot_compat_handle(0, idx)
#define crypto_keyslot_is_hmac(handle)          ((((handle) >> 16) & 0xff) == 0x1)
#define crypto_keyslot_is_mcipher(handle)       ((((handle) >> 16) & 0xff) == 0)
#define crypto_keyslot_get_idx(handle)          ((handle) & 0xff)
#define crypto_keyslot_get_module_id(handle)    (((handle) >> 8) & 0xff)

#endif
