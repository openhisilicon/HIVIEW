/*
  Copyright (c), 2001-2024, Shenshu Tech. Co., Ltd.
 */

#ifndef CRYPTO_SYMC_STRUCT_H
#define CRYPTO_SYMC_STRUCT_H

#include "crypto_common_struct.h"

/* Structure for SYMC Create. */
typedef enum {
    CRYPTO_SYMC_TYPE_NORMAL = 0x0,
    CRYPTO_SYMC_TYPE_REG,
    CRYPTO_SYMC_TYPE_MAX,
    CRYPTO_SYMC_TYPE_INVALID = 0xffffffff,
} crypto_symc_type;

typedef enum {
    CRYPTO_SYMC_ALG_TDES = 0x0,
    CRYPTO_SYMC_ALG_AES = 0x1,
    CRYPTO_SYMC_ALG_SM4 = 0x2,
    CRYPTO_SYMC_ALG_LEA = 0x3,
    CRYPTO_SYMC_ALG_DMA = 0x4,
    CRYPTO_SYMC_ALG_MAX,
    CRYPTO_SYMC_ALG_INVALID = 0xffffffff,
} crypto_symc_alg;

typedef enum {
    CRYPTO_SYMC_WORK_MODE_ECB = 0x0,
    CRYPTO_SYMC_WORK_MODE_CBC,
    CRYPTO_SYMC_WORK_MODE_CTR,
    CRYPTO_SYMC_WORK_MODE_OFB,
    CRYPTO_SYMC_WORK_MODE_CFB,
    CRYPTO_SYMC_WORK_MODE_CCM,
    CRYPTO_SYMC_WORK_MODE_GCM,
    CRYPTO_SYMC_WORK_MODE_CBC_MAC,
    CRYPTO_SYMC_WORK_MODE_CMAC,
    CRYPTO_SYMC_WORK_MODE_MAX,
    CRYPTO_SYMC_WORK_MODE_INVALID = 0xffffffff,
} crypto_symc_work_mode;

typedef enum {
    CRYPTO_SYMC_KEY_64BIT =  0x0,
    CRYPTO_SYMC_KEY_128BIT = 0x1,
    CRYPTO_SYMC_KEY_192BIT = 0x2,
    CRYPTO_SYMC_KEY_256BIT = 0x3,
    CRYPTO_SYMC_KEY_LENGTH_MAX,
    CRYPTO_SYMC_KEY_LENGTH_INVALID = 0xffffffff,
} crypto_symc_key_length;

typedef enum {
    CRYPTO_SYMC_KEY_EVEN =  0x0,
    CRYPTO_SYMC_KEY_ODD  =  0x1,
    CRYPTO_SYMC_KEY_PARITY_MAX,
    CRYPTO_SYMC_KEY_PARITY_INVALID = 0xffffffff,
} crypto_symc_key_parity;

typedef enum {
    CRYPTO_SYMC_BIT_WIDTH_1BIT = 0x0,
    CRYPTO_SYMC_BIT_WIDTH_8BIT = 0x1,
    CRYPTO_SYMC_BIT_WIDTH_64BIT = 0x2,
    CRYPTO_SYMC_BIT_WIDTH_128BIT = 0x3,
    CRYPTO_SYMC_BIT_WIDTH_MAX,
    CRYPTO_SYMC_BIT_WIDTH_INVALID = 0xffffffff,
} crypto_symc_bit_width;

typedef enum {
    CRYPTO_SYMC_IV_DO_NOT_CHANGE = 0,
    CRYPTO_SYMC_IV_CHANGE_ONE_PKG,
    CRYPTO_SYMC_IV_CHANGE_ALL_PKG,
    /* GCM. */
    CRYPTO_SYMC_GCM_IV_DO_NOT_CHANGE,
    CRYPTO_SYMC_GCM_IV_CHANGE_START,
    CRYPTO_SYMC_GCM_IV_CHANGE_UPDATE,
    CRYPTO_SYMC_GCM_IV_CHANGE_FINISH,
    /* CCM. */
    CRYPTO_SYMC_CCM_IV_DO_NOT_CHANGE,
    CRYPTO_SYMC_CCM_IV_CHANGE_START,
    CRYPTO_SYMC_CCM_IV_CHANGE_UPDATE,
    CRYPTO_SYMC_CCM_IV_CHANGE_FINISH,
    CRYPTO_SYMC_IV_CHANGE_MAX,
    CRYPTO_SYMC_IV_CHANGE_INVALID = 0xffffffff,
} crypto_symc_iv_change_type;

typedef struct {
    crypto_symc_alg symc_alg;
    crypto_symc_work_mode work_mode;
    crypto_symc_type symc_type;
    td_bool is_long_term;
} crypto_symc_attr;

typedef struct {
    crypto_buf_attr buf_attr;
    td_u32 length;
} crypto_symc_pack;

typedef struct {
    td_u32 clear_header_len;
    td_u32 payload_len;
    td_u32 payload_pattern_encrypt_len;
    td_u32 payload_pattern_clear_len;
    td_u32 payload_pattern_offset_len;
    td_bool iv_change;
    td_u32 iv[CRYPTO_AES_IV_SIZE_IN_WORD];
} crypto_symc_cenc_subsample;

typedef struct {
    td_bool use_odd_key;
    td_u32 first_encrypt_offset;
    crypto_symc_cenc_subsample *subsample;
    td_u32 subsample_num;
    crypto_symc_alg alg;
    crypto_symc_work_mode work_mode;
    td_u32 iv[CRYPTO_AES_IV_SIZE_IN_WORD];
} crypto_symc_cenc_param;

typedef struct {
    crypto_buf_attr aad_buf;
    td_u32 aad_len;         /* Addition Data Length In Bytes. */
    td_u32 data_len;        /* Crypto Data Length In Bytes. */
    td_u32 tag_len;         /* Tag Length In Bytes. */
} crypto_symc_config_aes_ccm_gcm;

typedef struct {
    crypto_symc_alg symc_alg;
    crypto_symc_work_mode work_mode;
    crypto_symc_key_length symc_key_length;
    crypto_symc_key_parity key_parity;
    crypto_symc_bit_width symc_bit_width;
    crypto_symc_iv_change_type iv_change_flag;
    td_u8 iv[CRYPTO_IV_LEN_IN_BYTES];
    td_u32 iv_length;
    td_void *param;
} crypto_symc_ctrl_t;

typedef struct {
    td_u32 chn_id;                                         /* Channel number */
    td_u32 open;                                           /* Open or closed */
    td_u32 is_decrypt;                                     /* Decrypt or encrypt */
    td_u32 alg;                                            /* Algorithm */
    td_u32 mode;                                           /* Work mode */
    td_u32 key_len;                                        /* Key length */
    td_u32 key_source;                                     /* Hard or soft key */
    td_u32 int_raw;                                        /* Raw intertupt */
    td_u32 int_en;                                         /* Enable interrupt */
    td_u32 int_status;                                     /* Status interrupt */
    td_u32 owner;                                          /* Process PID of owner */
    td_u32 is_secure;                                      /* Secure channel or not */
    td_u32 smmu_enable;                                    /* Smmu enable */
    td_u32 in_node_head;                                   /* In node list head */
    td_u32 in_node_rptr;                                   /* In node list read index */
    td_u32 in_node_wptr;                                   /* In node list write index */
    td_u32 in_node_depth;                                  /* In node depth */
    td_u32 out_node_head;                                  /* Out node list head */
    td_u32 out_node_rptr;                                  /* Out node list read index */
    td_u32 out_node_wptr;                                  /* Out node list write index */
    td_u32 out_node_depth;                                 /* Out node depth */
    td_u8 iv[CRYPTO_AES_IV_SIZE + CRYPTO_AES_IV_SIZE + 1]; /* Out iv */
} crypto_symc_proc_info;

typedef struct {
    td_u32 allow_reset; /* allow reset CPU or not */
    td_u32 sec_cpu;     /* secure CPU or not */
    const char *name;   /* interrupt name */
    td_u32 int_num;     /* interrupt number */
    td_u32 int_en;      /* interrupt enable */
    td_u32 smmu_base;   /* smmu base address */
    td_u32 err_code;    /* error code */
} crypto_symc_module_info;

typedef struct {
    td_bool is_long_term;
    crypto_symc_alg symc_alg;
    crypto_symc_work_mode work_mode;
    crypto_symc_key_length symc_key_length;
    td_u32 keyslot_chn;
} crypto_symc_mac_attr;

typedef struct {
    td_u8 key[32];
    td_bool is_odd;
    td_u32 klen;
} crypto_symc_clear_key;
#endif
