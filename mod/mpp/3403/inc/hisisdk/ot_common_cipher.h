/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */

#ifndef OT_COMMON_CIPHER_H
#define OT_COMMON_CIPHER_H

#include "ot_type.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

/* CIPHER error code value */
#define OT_ERR_CIPHER_NOT_INIT                      (td_s32)(0x804D0001)
#define OT_ERR_CIPHER_INVALID_HANDLE                (td_s32)(0x804D0002)
#define OT_ERR_CIPHER_INVALID_POINT                 (td_s32)(0x804D0003)
#define OT_ERR_CIPHER_INVALID_PARAM                 (td_s32)(0x804D0004)
#define OT_ERR_CIPHER_FAILED_INIT                   (td_s32)(0x804D0005)
#define OT_ERR_CIPHER_FAILED_GETHANDLE              (td_s32)(0x804D0006)
#define OT_ERR_CIPHER_FAILED_RELEASEHANDLE          (td_s32)(0x804D0007)
#define OT_ERR_CIPHER_FAILED_CFG_AES                (td_s32)(0x804D0008)
#define OT_ERR_CIPHER_FAILED_CFG_DES                (td_s32)(0x804D0009)
#define OT_ERR_CIPHER_FAILED_ENCRYPT                (td_s32)(0x804D000A)
#define OT_ERR_CIPHER_FAILED_DECRYPT                (td_s32)(0x804D000B)
#define OT_ERR_CIPHER_BUSY                          (td_s32)(0x804D000C)
#define OT_ERR_CIPHER_NO_AVAILABLE_RNG              (td_s32)(0x804D000D)
#define OT_ERR_CIPHER_FAILED_MEM                    (td_s32)(0x804D000E)
#define OT_ERR_CIPHER_UNAVAILABLE                   (td_s32)(0x804D000F)
#define OT_ERR_CIPHER_OVERFLOW                      (td_s32)(0x804D0010)
#define OT_ERR_CIPHER_HARD_STATUS                   (td_s32)(0x804D0011)
#define OT_ERR_CIPHER_TIMEOUT                       (td_s32)(0x804D0012)
#define OT_ERR_CIPHER_UNSUPPORTED                   (td_s32)(0x804D0013)
#define OT_ERR_CIPHER_REGISTER_IRQ                  (td_s32)(0x804D0014)
#define OT_ERR_CIPHER_ILLEGAL_UUID                  (td_s32)(0x804D0015)
#define OT_ERR_CIPHER_ILLEGAL_KEY                   (td_s32)(0x804D0016)
#define OT_ERR_CIPHER_INVALID_ADDR                  (td_s32)(0x804D0017)
#define OT_ERR_CIPHER_INVALID_LEN                   (td_s32)(0x804D0018)
#define OT_ERR_CIPHER_ILLEGAL_DATA                  (td_s32)(0x804D0019)
#define OT_ERR_CIPHER_RSA_SIGN                      (td_s32)(0x804D001A)
#define OT_ERR_CIPHER_RSA_VERIFY                    (td_s32)(0x804D001B)
#define OT_ERR_CIPHER_FAILED_SEC_FUNC               (td_s32)(0x804D001C)

#define OT_CIPHER_MAX_IV_SIZE_IN_WORD       4
#define OT_CIPHER_SM2_LEN_IN_WORD           8

/* enum typedef */
/* Cipher work mode. */
typedef enum {
    OT_CIPHER_WORK_MODE_ECB     = 0x0,  /* Electronic codebook (ECB) mode, ECB has been considered insecure and it is
                                           recommended not to use it. */
    OT_CIPHER_WORK_MODE_CBC,            /* Cipher block chaining (CBC) mode. */
    OT_CIPHER_WORK_MODE_CFB,            /* Cipher feedback (CFB) mode. */
    OT_CIPHER_WORK_MODE_OFB,            /* Output feedback (OFB) mode. */
    OT_CIPHER_WORK_MODE_CTR,            /* Counter (CTR) mode. */
    OT_CIPHER_WORK_MODE_CCM,            /* Counter (CCM) mode. */
    OT_CIPHER_WORK_MODE_GCM,            /* Counter (GCM) mode. */
    OT_CIPHER_WORK_MODE_BUTT,
} ot_cipher_work_mode;

/* Cipher algorithm. */
typedef enum {
    OT_CIPHER_ALG_AES           = 0x0,  /* Advanced encryption standard (AES) algorithm */
    OT_CIPHER_ALG_SM1,                  /* SM1 algorithm. */
    OT_CIPHER_ALG_SM4,                  /* SM4 algorithm. */
    OT_CIPHER_ALG_DMA,                  /* DMA copy. */
    OT_CIPHER_ALG_BUTT,
} ot_cipher_alg;

/* Key length. */
typedef enum {
    OT_CIPHER_KEY_DEFAULT       = 0x0,  /* Default key length, AES-16, SM1-48, SM4-16 */
    OT_CIPHER_KEY_AES_128BIT    = 0x0,  /* 128-bit key for the AES algorithm */
    OT_CIPHER_KEY_AES_192BIT    = 0x1,  /* 192-bit key for the AES algorithm */
    OT_CIPHER_KEY_AES_256BIT    = 0x2,  /* 256-bit key for the AES algorithm */
    OT_CIPHER_KEY_LEN_BUTT      = 0x3,
} ot_cipher_key_len;

/* Cipher bit width. */
typedef enum {
    OT_CIPHER_BIT_WIDTH_1BIT    = 0x0,  /* 1-bit width */
    OT_CIPHER_BIT_WIDTH_8BIT,           /* 8-bit width */
    OT_CIPHER_BIT_WIDTH_64BIT,          /* 64-bit width */
    OT_CIPHER_BIT_WIDTH_128BIT,         /* 128-bit width */
    OT_CIPHER_BIT_WIDTH_BUTT,
} ot_cipher_bit_width;

/* Encryption/Decryption type selecting. */
typedef enum {
    OT_CIPHER_TYPE_NORMAL       = 0x0,
    OT_CIPHER_TYPE_BUTT,
} ot_cipher_type;

/* Cipher control parameters. */
typedef enum {
    OT_CIPHER_IV_CHG_NONE    = 0x0,     /* CIPHER don't set key and IV */
    OT_CIPHER_IV_CHG_ONE_PACK,          /* CIPHER set key and IV for first package */
    OT_CIPHER_IV_CHG_ALL_PACK,          /* CIPHER set key and IV for all package */
    OT_CIPHER_IV_CHG_BUTT,
} ot_cipher_ctrl_chg_flag;

/* Structure of the cipher type */
typedef struct {
    ot_cipher_type cipher_type;
} ot_cipher_attr;

/* Structure of the cipher AES control information */
typedef struct {
    td_u32 iv[OT_CIPHER_MAX_IV_SIZE_IN_WORD];          /* Initialization vector (IV) */
    ot_cipher_bit_width bit_width;           /* Bit width for encryption or decryption */
    ot_cipher_key_len key_len;               /* Key length */
    /* control information exchange choices, we default all woulde be change except they have been in the choices */
    ot_cipher_ctrl_chg_flag chg_flags;
} ot_cipher_ctrl_aes;

/* Structure of the cipher AES CCM/GCM control information */
typedef struct {
    td_u32 iv[OT_CIPHER_MAX_IV_SIZE_IN_WORD];      /* Initialization vector (IV) */
    ot_cipher_key_len key_len;           /* Key length */
    td_u32 iv_len;                       /* IV length for CCM/GCM, which is an element of {7, 8, 9, 10, 11, 12, 13}
                                            for CCM, and is an element of [1-16] for GCM. */
    td_u32 tag_len;                      /* Tag length for CCM which is an element of {4, 6, 8, 10, 12, 14, 16}. */
    td_u32 aad_len;                      /* Associated data for CCM and GCM. */
    td_phys_addr_t aad_phys_addr;        /* Physical address of Associated data  for CCM and GCM. */
    td_u8 *aad_addr;
} ot_cipher_ctrl_aes_ccm_gcm;

/* Structure of the cipher SM4 control information */
typedef struct {
    td_u32 iv[OT_CIPHER_MAX_IV_SIZE_IN_WORD];          /* Initialization vector (IV) */
    /* control information exchange choices, we default all woulde be change except they have been in the choices */
    ot_cipher_ctrl_chg_flag chg_flags;
} ot_cipher_ctrl_sm4;

/* Structure of the cipher control information */
typedef struct {
    ot_cipher_alg alg;                                  /* cipher algorithm */
    ot_cipher_work_mode work_mode;                      /* algorithm work mode */
    union {
        /* AES ECB/CBC/CFB/OFB/CTR control
           AUTO: ot_cipher_alg: OT_CIPHER_ALG_AES; ot_cipher_work_mode: OT_CIPHER_WORK_MODE_ECB
           AUTO: ot_cipher_alg: OT_CIPHER_ALG_AES; ot_cipher_work_mode: OT_CIPHER_WORK_MODE_CBC
           AUTO: ot_cipher_alg: OT_CIPHER_ALG_AES; ot_cipher_work_mode: OT_CIPHER_WORK_MODE_CFB
           AUTO: ot_cipher_alg: OT_CIPHER_ALG_AES; ot_cipher_work_mode: OT_CIPHER_WORK_MODE_OFB
           AUTO: ot_cipher_alg: OT_CIPHER_ALG_AES; ot_cipher_work_mode: OT_CIPHER_WORK_MODE_CTR */
        ot_cipher_ctrl_aes aes_ctrl;

        /* AES CCM/GCM control
           AUTO: ot_cipher_alg: OT_CIPHER_ALG_AES; ot_cipher_work_mode: OT_CIPHER_WORK_MODE_CCM
           AUTO: ot_cipher_alg: OT_CIPHER_ALG_AES; ot_cipher_work_mode: OT_CIPHER_WORK_MODE_GCM */
        ot_cipher_ctrl_aes_ccm_gcm aes_ccm_gcm_ctrl;

        /* SM4 ECB/CBC/CFB/OFB/CTR control
           AUTO: ot_cipher_alg: OT_CIPHER_ALG_SM4; ot_cipher_work_mode: OT_CIPHER_WORK_MODE_ECB
           AUTO: ot_cipher_alg: OT_CIPHER_ALG_SM4; ot_cipher_work_mode: OT_CIPHER_WORK_MODE_CBC
           AUTO: ot_cipher_alg: OT_CIPHER_ALG_SM4; ot_cipher_work_mode: OT_CIPHER_WORK_MODE_CFB
           AUTO: ot_cipher_alg: OT_CIPHER_ALG_SM4; ot_cipher_work_mode: OT_CIPHER_WORK_MODE_OFB
           AUTO: ot_cipher_alg: OT_CIPHER_ALG_SM4; ot_cipher_work_mode: OT_CIPHER_WORK_MODE_CTR */
        ot_cipher_ctrl_sm4 sm4_ctrl;
    };
} ot_cipher_ctrl;

/* Cipher data */
typedef struct {
    td_phys_addr_t src_phys_addr;   /* phy address of the original data */
    td_phys_addr_t dst_phys_addr;   /* phy address of the purpose data */
    td_u32 byte_len;                /* cipher data length. */
} ot_cipher_data;

/* Hash algrithm type. */
typedef enum {
    OT_CIPHER_HASH_TYPE_SHA1         = 0x00,
    OT_CIPHER_HASH_TYPE_SHA224,
    OT_CIPHER_HASH_TYPE_SHA256,
    OT_CIPHER_HASH_TYPE_SHA384,
    OT_CIPHER_HASH_TYPE_SHA512,
    OT_CIPHER_HASH_TYPE_SM3          =  0x10,
    OT_CIPHER_HASH_TYPE_HMAC_SHA1    =  0x20,
    OT_CIPHER_HASH_TYPE_HMAC_SHA224,
    OT_CIPHER_HASH_TYPE_HMAC_SHA256,
    OT_CIPHER_HASH_TYPE_HMAC_SHA384,
    OT_CIPHER_HASH_TYPE_HMAC_SHA512,
    OT_CIPHER_HASH_TYPE_HMAC_SM3     =  0x30,
    OT_CIPHER_HASH_TYPE_BUTT,
} ot_cipher_hash_type;

/* Hash init struct input */
typedef struct {
    td_u8 *hmac_key;
    td_u32 hmac_key_len;
    ot_cipher_hash_type sha_type;
} ot_cipher_hash_attr;

typedef struct {
    td_u8 *data;
    td_u32 data_len;
} ot_cipher_common_data;

typedef enum {
    OT_CIPHER_RSA_SCHEME_PKCS1_V15 = 0x00,  /* PKCS#1 V15 */
    OT_CIPHER_RSA_SCHEME_PKCS1_V21,         /* PKCS#1 V21, PSS for signning, OAEP for encryption */
    OT_CIPHER_RSA_SCHEME_BUTT,
} ot_cipher_rsa_scheme;

/* RSA public key struct */
typedef struct {
    td_u8 *n;         /* Point to public modulus N */
    td_u8 *e;         /* Point to public exponent E */
    td_u16 n_len;     /* Length of public modulus N, max value is 512Byte */
    td_u16 e_len;     /* Length of public exponent E, max value is 512Byte */
} ot_cipher_rsa_public_key;

/* RSA private key struct */
typedef struct {
    td_u8 *n;         /* Public modulus N. */
    td_u8 *e;         /* Public exponent E. */
    td_u8 *d;         /* Private exponent D. */
    td_u8 *p;         /* 1st prime factor P. */
    td_u8 *q;         /* 2nd prime factor Q. */
    td_u8 *dp;        /* descript:d % (p - 1) is DP. */
    td_u8 *dq;        /* descript:d % (q - 1) is DQ. */
    td_u8 *qp;        /* descript:1 / (q % p) is QP. */
    td_u16 n_len;     /* Length of public modulus */
    td_u16 e_len;     /* Length of public exponent */
    td_u16 d_len;     /* Length of private exponent */
    td_u16 p_len;     /* Length of 1st prime factor,should be half of n_len */
    td_u16 q_len;     /* Length of 2nd prime factor,should be half of n_len */
    td_u16 dp_len;    /* Length of D % (P - 1),should be half of n_len */
    td_u16 dq_len;    /* Length of D % (Q - 1),should be half of n_len */
    td_u16 qp_len;    /* Length of 1 / (Q % P),should be half of n_len */
} ot_cipher_rsa_private_key;

typedef enum {
    OT_CIPHER_SIGN_TYPE_MSG = 0x00,
    OT_CIPHER_SIGN_TYPE_HASH,
    OT_CIPHER_SIGN_TYPE_BUTT,
} ot_cipher_sign_type;

typedef struct {
    ot_cipher_sign_type sign_type;
    td_u8 *input;
    td_u32 input_len;
} ot_cipher_sign_in_data;

typedef struct {
    td_u32 px[OT_CIPHER_SM2_LEN_IN_WORD];
    td_u32 py[OT_CIPHER_SM2_LEN_IN_WORD];
} ot_cipher_sm2_public_key;

typedef struct {
    td_u32 d[OT_CIPHER_SM2_LEN_IN_WORD];
} ot_cipher_sm2_private_key;

typedef struct {
    td_u32 d[OT_CIPHER_SM2_LEN_IN_WORD];
    td_u32 px[OT_CIPHER_SM2_LEN_IN_WORD];
    td_u32 py[OT_CIPHER_SM2_LEN_IN_WORD];
    td_u8 *id;
    td_u16 id_len;
} ot_cipher_sm2_sign;

typedef struct {
    td_u32 px[OT_CIPHER_SM2_LEN_IN_WORD];
    td_u32 py[OT_CIPHER_SM2_LEN_IN_WORD];
    td_u8 *id;
    td_u16 id_len;
} ot_cipher_sm2_verify;

typedef struct {
    td_u32 r[OT_CIPHER_SM2_LEN_IN_WORD];
    td_u32 s[OT_CIPHER_SM2_LEN_IN_WORD];
} ot_cipher_sm2_sign_data;

typedef enum {
    OT_KEYSLOT_TYPE_MCIPHER,            /* keyslot is used to mcipher. */
    OT_KEYSLOT_TYPE_BUTT,
} ot_keyslot_type;

typedef enum {
    OT_KEYSLOT_SECURE_MODE_NONE = 0x00, /* no secure. */
    OT_KEYSLOT_SECURE_MODE_TEE,         /* tee. */
    OT_KEYSLOT_SECURE_MODE_BUTT,
} ot_keyslot_secure_mode;

typedef struct {
    ot_keyslot_type type;
    ot_keyslot_secure_mode secure_mode;
} ot_keyslot_attr;

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* OT_COMMON_CIPHER_H */
