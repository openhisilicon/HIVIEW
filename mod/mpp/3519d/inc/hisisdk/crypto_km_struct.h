/*
  Copyright (c), 2001-2024, Shenshu Tech. Co., Ltd.
 */

#ifndef OT_KM_STRUCT_H
#define OT_KM_STRUCT_H

#include "ot_type.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif  /* __cplusplus */

typedef uintptr_t           crypto_handle;

typedef td_bool (*drv_kdf_wait_condition_func)(const td_void *param);

typedef td_bool (*drv_klad_wait_condition_func)(const td_void *param);

typedef td_s32 (*osal_kdf_wait_timeout_uninterruptible)(const td_void *wait, drv_kdf_wait_condition_func func,
    const td_void *param, const td_u32 timeout_ms);

typedef td_s32 (*osal_klad_wait_timeout_uninterruptible)(const td_void *wait, drv_klad_wait_condition_func func,
    const td_void *param, const td_u32 timeout_ms);

/* soc kald */
#define KM_KDF_OTP_KEY    (0x02000000)
#define KM_OTP_KEY(n)  (KM_KDF_OTP_KEY | (n))

#define KM_KDF_OTP_KEY_MRK1  KM_OTP_KEY(0x00)
#define KM_KDF_OTP_KEY_USK   KM_OTP_KEY(0x01)
#define KM_KDF_OTP_KEY_RUSK  KM_OTP_KEY(0x02)

#define KM_KLAD_ROOTKEY (0x03000000)
#define KM_ROOT_KEY(n)  (KM_KLAD_ROOTKEY | (n))

/* SBRK0, TEE Global Boot Root Key0, delivered from MRK1, soc_tee_enable and Hash of TEE Root Public Key */
#define KM_KLAD_KEY_TYPE_SBRK0  KM_ROOT_KEY(0x00)
/* SBRK1, TEE Global Boot Root Key1, delivered from MRK1, soc_tee_enable and Hash of TEE Root Public Key */
#define KM_KLAD_KEY_TYPE_SBRK1  KM_ROOT_KEY(0x01)
/* SBRK2, TEE Global Boot Root Key2, delivered from MRK1, soc_tee_enable and Hash of TEE Root Public Key */
#define KM_KLAD_KEY_TYPE_SBRK2  KM_ROOT_KEY(0x02)
#define KM_KLAD_KEY_TYPE_ABRK0  KM_ROOT_KEY(0x03) /* ABRK0, REE Global Boot Root Key0, delivered from MRK1 */
#define KM_KLAD_KEY_TYPE_ABRK1  KM_ROOT_KEY(0x04) /* ABRK1, REE Global Boot Root Key1, delivered from MRK1 */
#define KM_KLAD_KEY_TYPE_ABRK2  KM_ROOT_KEY(0x05) /* ABRK2, REE Global Boot Root Key2, delivered from MRK1 */
/* DRK0, TEE unique data decryption root key, delivered from USK */
#define KM_KLAD_KEY_TYPE_DRK0   KM_ROOT_KEY(0x06)
/* DRK1, REE unique data decryption root key, delivered from USK */
#define KM_KLAD_KEY_TYPE_DRK1   KM_ROOT_KEY(0x07)
/* RDRK0, TEE random data decryption root key, delivered from RUSK */
#define KM_KLAD_KEY_TYPE_RDRK0  KM_ROOT_KEY(0x08)
/* RDRK1, REE random data decryption root key, delivered from RUSK */
#define KM_KLAD_KEY_TYPE_RDRK1  KM_ROOT_KEY(0x09)
#define KM_KLAD_KEY_TYPE_PSK    KM_ROOT_KEY(0x0A) /* PSK, delivered from MRK1 */
/* ODRK0, TEE flash online decryptin root key, delivered from MRK0/1, soc_tee_enable and Hash of TEE Root Public Key */
#define KM_KLAD_KEY_TYPE_ODRK0  KM_ROOT_KEY(0x0B)
/* OARK0  TEE flash online authentication root key, delivered from MRK0/1,
    soc_tee_enable and Hash of TEE Root Public Key */
#define KM_KLAD_KEY_TYPE_OARK0  KM_ROOT_KEY(0x0C)
/* ODRK1, REE flash online decryption root key, delivered from MRK1 */
#define KM_KLAD_KEY_TYPE_ODRK1  KM_ROOT_KEY(0x0D)
/* MDRK0 */
#define KM_KLAD_KEY_TYPE_MDRK0    KM_ROOT_KEY(0x0E)
/* MDRK1 */
#define KM_KLAD_KEY_TYPE_MDRK1    KM_ROOT_KEY(0x0F)
/* MDRK2 */
#define KM_KLAD_KEY_TYPE_MDRK2    KM_ROOT_KEY(0x10)
/* MDRK3 */
#define KM_KLAD_KEY_TYPE_MDRK3    KM_ROOT_KEY(0x11)

typedef enum {
    KM_KLAD_HMAC_SHA1 = 0x20, /* Do Not Recommend to Use */
    KM_KLAD_HMAC_SHA224,      /* Software Kdf not support */
    KM_KLAD_HMAC_SHA256,
    KM_KLAD_HMAC_SHA384,
    KM_KLAD_HMAC_SHA512,
    KM_KLAD_HMAC_SM3 = 0x30,
    KM_KLAD_HMAC_MAX,
    KM_KLAD_HMAC_INVALID = 0xffffffff,
} km_klad_hmac_type;

/* Define the key security attribute. */
typedef enum {
    KM_KLAD_SEC_DISABLE = 0,
    KM_KLAD_SEC_ENABLE,
    KM_KLAD_SEC_MAX,
    KM_KLAD_SEC_INVALID = 0xffffffff,
} km_klad_sec;

/* Define the keyladder level. */
typedef enum {
    KM_KLAD_LEVEL1 = 0,
    KM_KLAD_LEVEL_MAX,
    KM_KLAD_LEVEL_INVALID = 0xffffffff,
} km_klad_level;

/* Define the keyladder algorithm. */
typedef enum {
    KM_KLAD_ALG_TYPE_TDES = 0, /* Do Not Recommend to Use */
    KM_KLAD_ALG_TYPE_AES,
    KM_KLAD_ALG_TYPE_SM4,
    KM_KLAD_ALG_TYPE_MAX,
    KM_KLAD_ALG_TYPE_INVALID = 0xffffffff,
} km_klad_alg_type;

/* Define the algorithm of crypto engine. */
typedef enum {
    KM_CRYPTO_ALG_AES = 0x20,
    KM_CRYPTO_ALG_LEA = 0x40,        /* Flash online decryption and Cipher offline encryption */
    KM_CRYPTO_ALG_SM4 = 0x50,
    KM_CRYPTO_ALG_TDES = 0x70,       /* Do Not Recommend to Use */
    KM_CRYPTO_ALG_HMAC_SHA1 = 0xA0,  /* Do Not Recommend to Use */
    KM_CRYPTO_ALG_HMAC_SHA2 = 0xA1,
    KM_CRYPTO_ALG_HMAC_SM3 = 0xA2,
    KM_CRYPTO_ALG_MAX,
    KM_CRYPTO_ALG_INVALID = 0xffffffff,
} km_crypto_alg;

/* Define the destination type of keyladder. */
typedef enum {
    KM_KLAD_DEST_TYPE_MCIPHER = 0x00,   /* keyslot for mcipher */
    KM_KLAD_DEST_TYPE_HMAC,             /* keyslot for hmac */
    KM_KLAD_DEST_TYPE_FLASH,            /* flash controller, for flash on line decryption and authentication */
    KM_KLAD_DEST_TYPE_NPU,              /* for NPU */
    KM_KLAD_DEST_TYPE_MAX,
    KM_KLAD_DEST_TYPE_INVALID = 0xffffffff,
} km_klad_dest_type;

/* Define the flash online decryption key type it's valid for non-TEE platform,  it's will be ignored on TEE platform */
typedef enum {
    KM_KLAD_FLASH_KEY_TYPE_REE_DEC = 0x00,  /* REE flash online decryption key */
    KM_KLAD_FLASH_KEY_TYPE_TEE_DEC,         /* TEE flash online decryption key */
    KM_KLAD_FLASH_KEY_TYPE_TEE_AUT,         /* TEE flash online authentication key */
    KM_KLAD_FLASH_KEY_TYPE_MAX,
    KM_KLAD_FLASH_KEY_TYPE_INVALID = 0xffffffff,
} km_klad_flash_key_type;

/* * Key parity, valid when key length is not more than 128bit */
/* * CNcomment: Key Parity Attribute */
typedef enum {
    KM_KLAD_KEY_EVEN =  0x0,   /* *< even key  */
    KM_KLAD_KEY_ODD  =  0x1,   /* *< odd key */
    KM_KLAD_KEY_PARITY_MAX,
    KM_KLAD_KEY_PARITY_INVALID = 0xffffffff,
} km_klad_key_parity;

/* Define the structure of keyladder configuration. */
typedef struct {
    td_u32 rootkey_type;  /* Keyladder rootkey type, KM_KLAD_KEY_TYPE_xxx. */
} km_klad_config;

/* Define the structure of content key security configurations. */
/* when cipher work mode is CBC_MAC, dest_buf_sec_support and dest_buf_non_sec_support cannot be both false */
typedef struct {
    km_klad_sec key_sec; /* Secure key can only be used by TEE CPU and AIDSP locked cipher and hash channel */
    /* Only the cipher or hash channel which is locked by same CPU as keyladder can use this key,
    valid only for TEE CPU and AIDSP */
    td_bool master_only_enable;
    td_bool dest_buf_sec_support;     /* The destination buffer of target engine can be secure. */
    td_bool dest_buf_non_sec_support; /* The destination buffer of target engine can be non-secure. */
    td_bool src_buf_sec_support;      /* The source buffer of target engine can be secure. */
    td_bool src_buf_non_sec_support;  /* The source buffer of target engine can be non-secure. */
} km_klad_key_secure_config;

/* Define the structure of content key configurations. */
typedef struct {
    km_crypto_alg engine; /* The content key can be used for which algorithm of the crypto engine. */
    td_bool decrypt_support;  /* The content key can be used for decrypting. */
    td_bool encrypt_support;  /* The content key can be used for encrypting. */
} km_klad_key_config;

/* Structure of keyladder extend attributes. */
typedef struct {
    km_klad_config klad_cfg;          /* The keyladder configuration, valid for harware key. */
    km_klad_key_config key_cfg;       /* The content key configuration. */
    km_klad_key_secure_config key_sec_cfg; /* The content key security configuration. */
    td_u32 rkp_sw_cfg; /* The software configuration for NPU */
} km_klad_attr;

/* Structure of setting session key. */
typedef struct {
    km_klad_level level;              /* the level of session key. */
    km_klad_alg_type alg;             /* the algorithm used to decrypt session key. */
    td_u32 key_size;                      /* the size of session key, should be 16. */
    td_u8 *key;   /* the session key. */
} km_klad_session_key;

/* Structure of setting content key. */
typedef struct {
    km_klad_alg_type alg;             /* The algorithm of the content key. */
    td_u32 key_size;                      /* The size of content key, should be 16 or 32. */
    td_u8 *key;                           /* The content key. */
    km_klad_key_parity key_parity;    /* Odd or even key flag. */
} km_klad_content_key;

/* Structure of sending clear key. */
typedef struct {
    km_klad_hmac_type hmac_type;      /* hmac type, only send hmac key need to config. */
    td_u32 key_size;                      /* The size of content key, 16 or 32 for cipher,
                                             not more than block size for HMAC. */
    td_u8 *key;                           /* The content key. */
    km_klad_key_parity key_parity;    /* Odd or even key flag. */
} km_klad_clear_key;

/* soc keyslot */
/* Define the keyslot type. */
typedef enum {
    KM_KEYSLOT_TYPE_MCIPHER = 0x00,
    KM_KEYSLOT_TYPE_HMAC,
    KM_KEYSLOT_TYPE_MAX,
    KM_KEYSLOT_TYPE_INVALID = 0xffffffff,
} km_keyslot_type;

/**
 * @brief  Root key selection during KDF key derivation.
 */
typedef enum {
    CRYPTO_KDF_OTP_KEY_MRK1 = 0,
    CRYPTO_KDF_OTP_KEY_USK,
    CRYPTO_KDF_OTP_KEY_RUSK
} crypto_kdf_otp_key;
 
/**
 * @brief  Symmetric algorithm selection during KDF key derivation.
 */
typedef enum {
    CRYPTO_KDF_UPDATE_ALG_AES = 0,
    CRYPTO_KDF_UPDATE_ALG_SM4
} crypto_kdf_update_alg;
 
/**
 * @brief  Hash algorithm selection when the software PBKDF2 algorithm is used.
 */
typedef enum {
    CRYPTO_KDF_SW_ALG_SHA1 = 0,
    CRYPTO_KDF_SW_ALG_SHA256,
    CRYPTO_KDF_SW_ALG_SHA384,
    CRYPTO_KDF_SW_ALG_SHA512,
    CRYPTO_KDF_SW_ALG_SM3
} crypto_kdf_sw_alg;
 
/**
 * @brief  Select the derived key type during KDF key derivation.
 */
typedef enum {
    CRYPTO_KDF_HARD_KEY_TYPE_SBRK0  = 0x03000000,
    CRYPTO_KDF_HARD_KEY_TYPE_SBRK1,
    CRYPTO_KDF_HARD_KEY_TYPE_SBRK2,
    CRYPTO_KDF_HARD_KEY_TYPE_ABRK0,
    CRYPTO_KDF_HARD_KEY_TYPE_ABRK1,
    CRYPTO_KDF_HARD_KEY_TYPE_ABRK2,
    CRYPTO_KDF_HARD_KEY_TYPE_DRK0,
    CRYPTO_KDF_HARD_KEY_TYPE_DRK1,
    CRYPTO_KDF_HARD_KEY_TYPE_RDRK0,
    CRYPTO_KDF_HARD_KEY_TYPE_RDRK1,
    CRYPTO_KDF_HARD_KEY_TYPE_PSK,
    CRYPTO_KDF_HARD_KEY_TYPE_ODRK0,
    CRYPTO_KDF_HARD_KEY_TYPE_OARK0,
    CRYPTO_KDF_HARD_KEY_TYPE_ODRK1,
    CRYPTO_KDF_HARD_KEY_TYPE_MDRK0,
    CRYPTO_KDF_HARD_KEY_TYPE_MDRK1,
    CRYPTO_KDF_HARD_KEY_TYPE_MDRK2,
    CRYPTO_KDF_HARD_KEY_TYPE_MDRK3,

    CRYPTO_KDF_HARD_KEY_TYPE_ABRK_REE,
    CRYPTO_KDF_HARD_KEY_TYPE_ABRK_TEE,
    CRYPTO_KDF_HARD_KEY_TYPE_RDRK_REE,
    CRYPTO_KDF_HARD_KEY_TYPE_RDRK_TEE,
} crypto_kdf_hard_key_type;
 
/**
 * @brief  KDF key derivation, hash algorithm selection when the hardware PBKDF2 algorithm is used.
 */
typedef enum {
    CRYPTO_KDF_HARD_ALG_SHA256 = 0,
    CRYPTO_KDF_HARD_ALG_SM3
} crypto_kdf_hard_alg;
 
typedef enum {
    CRYPTO_KDF_HARD_KEY_SIZE_128BIT = 0,
    CRYPTO_KDF_HARD_KEY_SIZE_192BIT,
    CRYPTO_KDF_HARD_KEY_SIZE_256BIT,
} crypto_kdf_hard_key_size;
 
typedef struct {
    crypto_kdf_hard_key_type hard_key_type;
    crypto_kdf_hard_alg hard_alg;
    crypto_kdf_hard_key_size hard_key_size;
    td_u32 rkp_sw_cfg;
    td_u8 *salt;
    td_u32 salt_length;
    td_bool is_oneway;
} crypto_kdf_hard_calc_param;
 
/**
 * @brief  The klad target module's algorithm engine, determining the algorithm supported by the sent key.
 */
typedef enum {
    CRYPTO_KLAD_ENGINE_AES = 0x20,
    CRYPTO_KLAD_ENGINE_LAE = 0x40,
    CRYPTO_KLAD_ENGINE_SM4 = 0x50,
    CRYPTO_KLAD_ENGINE_TDES = 0x70,
    CRYPTO_KLAD_ENGINE_SHA1_HMAC = 0xA0,
    CRYPTO_KLAD_ENGINE_SHA2_HMAC = 0xA1,
    CRYPTO_KLAD_ENGINE_SM3_HMAC = 0xA2,
} crypto_klad_engine;
 
/**
 * @brief  The klad target module, determining the module to which the key is sent.
 */
typedef enum {
    CRYPTO_KLAD_DEST_MCIPHER = 0,
    CRYPTO_KLAD_DEST_HMAC,
    CRYPTO_KLAD_DEST_FLASH,
    CRYPTO_KLAD_DEST_NPU,
    CRYPTO_KLAD_DEST_AIDSP,
} crypto_klad_dest;
 
/**
 * @brief  Flash online decryption mode, determining the mode used after the key is sent.
 */
typedef enum {
    CRYPTO_KLAD_FLASH_KEY_TYPE_REE_DEC = 0x00,  /* REE flash online decryption key */
    CRYPTO_KLAD_FLASH_KEY_TYPE_TEE_DEC,         /* TEE flash online decryption key */
    CRYPTO_KLAD_FLASH_KEY_TYPE_TEE_AUT,         /* TEE flash online authentication key */
    CRYPTO_KLAD_FLASH_KEY_TYPE_INVALID,
} crypto_klad_flash_key_type;
 
/**
 * @brief  Symmetric key length. Determines the length of the final working key.
 */
typedef enum {
    CRYPTO_KLAD_KEY_SIZE_128BIT,
    CRYPTO_KLAD_KEY_SIZE_192BIT,
    CRYPTO_KLAD_KEY_SIZE_256BIT,
} crypto_klad_key_size;
 
/**
 * @brief  When the target engine is HMAC, determine the HAMC algorithm to be used.
 */
typedef enum {
    CRYPTO_KLAD_HMAC_TYPE_SHA1 = 0x20, /* Insecure algorithm, not recommended. */
    CRYPTO_KLAD_HMAC_TYPE_SHA224,
    CRYPTO_KLAD_HMAC_TYPE_SHA256,
    CRYPTO_KLAD_HMAC_TYPE_SHA384,
    CRYPTO_KLAD_HMAC_TYPE_SHA512,
    CRYPTO_KLAD_HMAC_TYPE_SM3 = 0x30,
    CRYPTO_KLAD_HMAC_TYPE_MAX,
    CRYPTO_KLAD_HMAC_TYPE_INVALID = 0xffffffff,
} crypto_klad_hmac_type;
 
/**
 * @brief  Determines the current derived key level during klad key derivation.
 */
typedef enum {
    CRYPTO_KLAD_LEVEL_SEL_FIRST = 0,
    CRYPTO_KLAD_LEVEL_SEL_SECOND
} crypto_klad_level_sel;
 
/**
 * @brief  Determines the symmetric algorithm used for derivation during klad key derivation.
 */
typedef enum {
    CRYPTO_KLAD_ALG_SEL_TDES = 0,
    CRYPTO_KLAD_ALG_SEL_AES,
    CRYPTO_KLAD_ALG_SEL_SM4,
} crypto_klad_alg_sel;
 
/**
 * @brief  Clear key structure when klad sends a clear key.
 */
typedef struct {
    td_u8 *key;     /* Clear key content. */
    td_u32 key_length;  /* Length of the clear key, in bytes.
                                For the symmetric algorithm, the value can only be 16, 24, or 32.
                                For HMAC-SH1/SHA224/SHA256/SM3, the value cannot exceed 64.
                                For HMAC-SHA384/SHA512, the value cannot exceed 128. */
    td_bool key_parity; /* Indicates the parity attribute of a key.
                                Valid when the target is a symmetric algorithm engine and key_length is set to 16. */
    crypto_klad_hmac_type hmac_type; /* Indicates the HMAC algorithm.
                                            Valid only when the target is the HMAC algorithm engine. */
} crypto_klad_clear_key;
 
/**
 * @brief  Keyladder root key type selection.
 */
typedef struct {
    crypto_kdf_hard_key_type rootkey_type;
} crypto_klad_config;
 
/**
 * @brief  Keyladder working key attribute configuration.
 */
typedef struct {
    crypto_klad_engine engine;  /* The working key can be used for which algorithm of the crypto engine. */
    td_bool decrypt_support;    /* The working key can be used for decrypting. */
    td_bool encrypt_support;    /* The working key can be used for encrypting. */
} crypto_klad_key_config;
 
/**
 * @brief  Security attribute of the key.
    when cipher work mode is CBC_MAC, dest_buf_sec_support and dest_buf_non_sec_support cannot be both false
 */
typedef struct {
    td_bool key_sec;    /* Secure key can only be used by TEE CPU and AIDSP locked cipher and hash channel. */
    td_bool master_only_enable; /* Only the cipher or hash channel which is locked by same CPU as keyladder
                                        can use this key, valid only for TEE CPU and AIDSP. */
    td_bool dest_buf_sec_support;   /* The destination buffer of target engine can be secure. */
    td_bool dest_buf_non_sec_support; /* The destination buffer of target engine can be secure. */
    td_bool src_buf_sec_support;      /* The destination buffer of target engine can be secure. */
    td_bool src_buf_non_sec_support;  /* The destination buffer of target engine can be secure. */
} crypto_klad_key_secure_config;
 
/**
 * @brief  Keyladder configuration attributes.
 */
typedef struct {
    crypto_klad_config klad_cfg;    /* The keyladder configuration, valid for harware key. */
    crypto_klad_key_config key_cfg; /* The working key configuration. */
    crypto_klad_key_secure_config key_sec_cfg;  /* The working key security configuration. */
    td_u32 rkp_sw_cfg;
} crypto_klad_attr;

/**
 * @brief  Keyladder hardware key configuration attributes.
 */
typedef struct {
    crypto_kdf_hard_alg kdf_hard_alg;
    td_bool key_parity; /* Indicates the parity attribute of a key.
                                Valid when the target is a symmetric algorithm engine and key_length is set to 16. */
    crypto_klad_key_size key_size;
    td_u8 *salt;    /* Salt content. Used as user input materials for key derivation.
                            The final working key varies according to the salt value. */
    td_u32 salt_length; /* Salt length, in bytes. It can only be 28. */
    td_bool oneway; /* Salt length, in bytes. It can only be 28. */
} crypto_klad_hard_key;

typedef struct {
    td_u8 key[16];
    td_u32 key_length;
    crypto_klad_level_sel level;
    crypto_klad_alg_sel alg;
} crypto_klad_session_key;

typedef struct {
    td_u8 key[32];
    td_u32 key_length;
    crypto_klad_alg_sel alg;
    td_bool key_parity;
} crypto_klad_content_key;

/**
 * @brief  Keyslot type selection.
 */
typedef enum {
    CRYPTO_KEYSLOT_TYPE_MCIPHER = 0,
    CRYPTO_KEYSLOT_TYPE_HMAC,
    CRYPTO_KEYSLOT_TYPE_FLASH,
} crypto_keyslot_type;

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif  /* __cplusplus */

#endif /* OT_KM_STRUCT_H */
