/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */

#ifndef OT_COMMON_KLAD_H
#define OT_COMMON_KLAD_H

#include "ot_type.h"

#ifdef __cplusplus
extern "C" {
#endif

/* klad error code value */
#define OT_ERR_KLAD_NOT_INIT            0x805D0000
#define OT_ERR_KLAD_FAILED_INIT         0x805D0001
#define OT_ERR_KLAD_NULL_PTR            0x805D0002
#define OT_ERR_KLAD_INVALID_PARAM       0x805D0003
#define OT_ERR_KLAD_FAILED_CREATE_DEV   0x805D0004
#define OT_ERR_KLAD_DEVICE_BUSY         0x805D0005
#define OT_ERR_KLAD_FAILED_SEC_FUNC     0x805D0006
#define OT_ERR_KLAD_TIMEOUT             0x805D0007
#define OT_ERR_KLAD_FAILED_MEM          0x805D0008
#define OT_ERR_KLAD_FAILED_OPERATE      0x805D0009
#define OT_ERR_KLAD_INVALID_OWNER       0x805D000A
#define OT_ERR_KLAD_INVALID_HANDLE      0x805D000B

/* klad max key length */
#define OT_KLAD_MAX_KEY_LEN     32

/* klad rootkey select */
typedef enum {
    OT_KLAD_ROOTKEY_SEL_OEM0 = 0x00,
    OT_KLAD_ROOTKEY_SEL_OEM1,
    OT_KLAD_ROOTKEY_SEL_OEM2,
    OT_KLAD_ROOTKEY_SEL_OEM3,
    OT_KLAD_ROOTKEY_SEL_VENDOR,
    OT_KLAD_ROOTKEY_SEL_BUTT,
}  ot_klad_rootkey_sel;

typedef enum {
    OT_KLAD_ROOTKEY_SEC_REE = 0x00,     /* REE key, TEE CPU can select ree key */
    OT_KLAD_ROOTKEY_SEC_TEE,            /* TEE key, REE CPU can't select tee key */
    OT_KLAD_ROOTKEY_SEC_BUTT,
} ot_klad_rootkey_secure;

/* only OT_KLAD_TYPE_COMMON is valid */
typedef struct {
    td_u32 owner_id;                    /* Derivative material, used for mcipher */
    ot_klad_rootkey_sel key_sel;        /* common klad route select rootkey */
    ot_klad_rootkey_secure key_secure;  /* Static value select: for ree key or for tee key */
} ot_klad_rootkey_attr;

/* klad route select */
typedef enum {
    OT_KLAD_TYPE_CLEARCW,           /* Used for clear key  */
    OT_KLAD_TYPE_COMMON,            /* Used for root key */
    OT_KLAD_TYPE_BUTT,
} ot_klad_type;

/* klad config */
typedef struct {
    ot_klad_type klad_type;             /* klad route select: common/clear */
    ot_klad_rootkey_attr rootkey_attr;  /* rootkey attr, OT_KLAD_TYPE_COMMON is valid */
} ot_klad_cfg;

typedef enum {
    OT_KLAD_CRYPTO_ALG_AES = 0,
    OT_KLAD_CRYPTO_ALG_SM4,
    OT_KLAD_CRYPTO_ALG_BUTT,
} ot_klad_crypto_alg;        /* The content key can be used for which algorithm of the crypto engine. */

/* klad attribute */
typedef struct {
    ot_klad_cfg klad_cfg;
} ot_klad_attr;

/* klad algorithm */
typedef enum {
    OT_KLAD_ALG_TYPE_AES = 0,
    OT_KLAD_ALG_TYPE_SM4,
    OT_KLAD_ALG_TYPE_BUTT,
} ot_klad_alg_type;

/* klad level */
typedef enum {
    OT_KLAD_LEVEL1 = 0,
    OT_KLAD_LEVEL2,
    OT_KLAD_LEVEL3,
    OT_KLAD_LEVEL_BUTT,
} ot_klad_level;

/* session key: set 1~n-1 stage common route klad */
typedef struct {
    ot_klad_level level;            /* klad level */
    ot_klad_alg_type alg;           /* klad algorithm */
    td_u32 key_size;                /* klad key size */
    td_u8 key[OT_KLAD_MAX_KEY_LEN]; /* klad key */
} ot_klad_session_key;

/* content key: set n stage common route klad */
typedef struct {
    ot_klad_alg_type alg;           /* klad algorithm */
    ot_klad_crypto_alg crypto_alg;  /* allowed target engine algorithm. */
    td_u32 key_size;                /* klad key size */
    td_u8 key[OT_KLAD_MAX_KEY_LEN]; /* klad key */
} ot_klad_content_key;

/* clear key: set clear route klad */
typedef struct {
    ot_klad_crypto_alg crypto_alg;  /* allowed target engine algorithm. */
    td_u32 key_size;                /* klad key size */
    td_u8 key[OT_KLAD_MAX_KEY_LEN]; /* klad key */
} ot_klad_clear_key;

#ifdef __cplusplus
}
#endif

#endif /* OT_COMMON_KLAD_H */
