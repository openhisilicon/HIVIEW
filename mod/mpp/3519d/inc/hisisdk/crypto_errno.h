/*
  Copyright (c), 2001-2024, Shenshu Tech. Co., Ltd.
 */

#ifndef CRYPTO_ERRNO_H
#define CRYPTO_ERRNO_H

/**
 * The Struction of Error Number is as follows:
 * Env(4 bits) | Layer(4 bits) | Modules(4 bits) | Reserved(12 bits) | Error Code(8 bits)
 **/

/* Error for Environments Definition. */
#define ERROR_ENV_LINUX         0x1
#define ERROR_ENV_ITRUSTEE      0x2
#define ERROR_ENV_OPTEE         0x3
#define ERROR_ENV_LITEOS        0x4
#define ERROR_ENV_SELITEOS      0x5
#define ERROR_ENV_NOOS          0x6

/* Error for Layer Enumerations. */
enum {
    ERROR_LAYER_UAPI = 0x1,
    ERROR_LAYER_DISPATCH,
    ERROR_LAYER_KAPI,
    ERROR_LAYER_DRV,
    ERROR_LAYER_HAL,
};

/* Error for Module Enumerations. */
enum {
    ERROR_MODULE_SYMC = 0x1,
    ERROR_MODULE_HASH,
    ERROR_MODULE_PKE,
    ERROR_MODULE_TRNG,
    ERROR_MODULE_KM,
    ERROR_MODULE_OTP,
    ERROR_MODULE_OTHER
};

/* Error for Error Code Enumerations. */
enum {
    /* Common Error Code. 0x00 ~ 0x3F. */
    ERROR_INVALID_PARAM = 0x0,      /* return when the input param's value is not in the valid range. */
    ERROR_PARAM_IS_NULL,            /* return when the input param is NULL and required not NULL. */
    ERROR_NOT_INIT,                 /* return when call other functions before call init function. */
    ERROR_UNSUPPORT,                /* return when some configuration is unsupport. */
    ERROR_UNEXPECTED,               /* reture when unexpected error occurs. */
    ERROR_TRY_TIMES,                /* return when the number of try attempts failed. */
    ERROR_CHN_BUSY,                 /* return when try to create one channel but all channels are busy. */
    ERROR_CTX_CLOSED,               /* return when using one ctx to do something but has been closed. */
    ERROR_NOT_SET_CONFIG,           /* return when not set_config but need for symc. */
    ERROR_NOT_ATTACHED,             /* return when not attach but need for symc. */
    ERROR_NOT_SET_SESSION_KEY,      /* return when not set_session_key but need for klad. */
    ERROR_NOT_MAC_START,            /* return when not mac_start but need for symc. */
    ERROR_INVALID_HANDLE,           /* return when pass one invalid handle. */
    ERROR_GET_PHYS_ADDR,            /* return when transfer from virt_addr to phys_addr failed. */
    ERROR_SYMC_LEN_NOT_ALIGNED,     /* return when length isn't aligned to 16-Byte except CTR/CCM/GCM.  */
    ERROR_SYMC_ADDR_NOT_ALIGNED,    /* return when the phys_addr writing to register is not aligned to 4-Byte. */
    ERROR_PKE_RSA_SAME_DATA,        /* return when rsa exp_mod, the input is equal to output. */
    ERROR_PKE_RSA_CRYPTO_V15_CHECK, /* return when rsa crypto v15 padding check failed. */
    ERROR_PKE_RSA_CRYPTO_OAEP_CHECK,    /* return when rsa crypto oaep padding check failed. */
    ERROR_PKE_RSA_VERIFY_V15_CHECK,     /* return when rsa verify v15 padding check failed. */
    ERROR_PKE_RSA_VERIFY_PSS_CHECK,     /* return when rsa verify pss padding check failed. */
    ERROR_PKE_RSA_GEN_KEY,          /* return when rsa generate key failed. */
    ERROR_PKE_SM2_DECRYPT,          /* return when sm2 decrypt failed. */
    ERROR_PKE_ECDSA_VERIFY_CHECK,   /* return when ecdsa verify check failed. */
    ERROR_OTP_ADDR_NOT_ALIGNED,     /* return when otp address is not aligned to 4. */

    /* Outer's Error Code. 0x40 ~ 0x5F. */
    ERROR_MEMCPY_S      = 0x40,     /* return when call memcpy_s failed. */
    ERROR_MALLOC,                   /* return when call xxx_malloc failed. */
    ERROR_MUTEX_INIT,               /* return when call xxx_mutex_init failed. */
    ERROR_MUTEX_LOCK,               /* return when call xxx_lock failed. */
    /* Specific Error Code for UAPI. 0x60 ~ 0x6F. */
    ERROR_DEV_OPEN_FAILED,          /* return when open dev failed. */
    ERROR_COUNT_OVERFLOW,           /* return when call init too many times. */

    /* Specific Error Code for Dispatch. 0x70 ~ 0x7F. */
    ERROR_CMD_DISMATCH  = 0x70,     /* return when cmd is dismatched. */
    ERROR_COPY_FROM_USER,           /* return when call copy_from_user failed. */
    ERROR_COPY_TO_USER,             /* return when call copy_to_user failed. */
    ERROR_MEM_HANDLE_GET,           /* return when parse user's mem handle to kernel's mem handle failed. */
    ERROR_GET_OWNER,                /* return when call crypto_get_owner failed. */

    /* Specific Error Code for KAPI. 0x80 ~ 0x8F. */
    ERROR_PROCESS_NOT_INIT = 0x80,  /* return when one process not call kapi_xxx_init first. */
    ERROR_MAX_PROCESS,              /* return when process's num is over the limit. */
    ERROR_MEMORY_ACCESS,            /* return when access the memory that does not belong to itself.  */
    ERROR_INVALID_PROCESS,          /* return when the process accesses resources of other processes. */
    /* Specific Error Code for DRV. 0x90 ~ 0x9F. */

    /* Specific Error Code for HAL. 0xA0 ~ 0xAF. */
    ERROR_HASH_LOGIC    = 0xA0,     /* return when hash logic's error occurs. */
    ERROR_PKE_LOGIC,                /* return when pke logic's error occurs. */
    ERROR_INVALID_CPU_TYPE,         /* return when logic get the invalid cpu type. */
    ERROR_INVALID_REGISTER_VALUE,   /* return when value in register is invalid. */
    ERROR_INVALID_PHYS_ADDR,        /* return when phys_addr is invalid. */
    ERROR_OTP_PERMISSION,           /* return when otp access is forbidden. */
    ERROR_KM_LOGIC,                  /* return when km logic's error occurs. */

    /* Specific Error Code for Timeout. 0xB0 ~ 0xBF. */
    ERROR_GET_TRNG_TIMEOUT = 0xB0,  /* return when logic get rnd timeout. */
    ERROR_HASH_CLEAR_CHN_TIMEOUT,   /* return when clear hash channel timeout. */
    ERROR_HASH_CALC_TIMEOUT,        /* return when hash calculation timeout. */
    ERROR_SYMC_CLEAR_CHN_TIMEOUT,   /* return when clear symc channel timeout. */
    ERROR_SYMC_CALC_TIMEOUT,        /* return when symc crypto timeout. */
    ERROR_SYMC_GET_TAG_TIMEOUT,     /* return when symc get tag timeout. */
    ERROR_PKE_LOCK_TIMEOUT,         /* return when pke lock timeout. */
    ERROR_PKE_WAIT_DONE_TIMEOUT,    /* return when pke wait done timeout. */
    ERROR_PKE_ROBUST_WARNING,    /* return when pke wait done timeout. */
    ERROR_RKP_LOCK_TIMEOUT,      /* return when rkp lock timeout. */
    ERROR_RKP_CALC_TIMEOUT,      /* return when rkp calculate timeout. */
    ERROR_OTP_TIMEOUT,          /* return when otp wait timeout. */
    ERROR_KEYSLOT_TIMEOUT,       /* return when keyslot wait timeout. */
    ERROR_KEYSLOT_LOCK,          /* return when keyslot lock or unlock fail. */
    ERROR_KLAD_TIMEOUT,          /* return when klad wait timeout. */
    ERROR_KLAD_LOCK,             /* return when klad lock or unlock fail. */
};

#define CRYPTO_COMPAT_ERRNO(env, layer, module, err_code) \
    ((((env) & 0xF) << 28) | (((layer) & 0xF) << 24) | (((module) & 0xF) << 20) | ((err_code) & 0xFF))

/* Notice: you should define CRYPTO_ERROR_ENV as one of ERROR_ENV_XXX before using these macros. */
#define UAPI_COMPAT_ERRNO(module, err_code)     \
    CRYPTO_COMPAT_ERRNO(CRYPTO_ERROR_ENV, ERROR_LAYER_UAPI, module, err_code)

#define DISPATCH_COMPAT_ERRNO(module, err_code)     \
    CRYPTO_COMPAT_ERRNO(CRYPTO_ERROR_ENV, ERROR_LAYER_DISPATCH, module, err_code)

#define KAPI_COMPAT_ERRNO(module, err_code)     \
    CRYPTO_COMPAT_ERRNO(CRYPTO_ERROR_ENV, ERROR_LAYER_KAPI, module, err_code)

#define DRV_COMPAT_ERRNO(module, err_code)     \
    CRYPTO_COMPAT_ERRNO(CRYPTO_ERROR_ENV, ERROR_LAYER_DRV, module, err_code)

#define HAL_COMPAT_ERRNO(module, err_code)      \
    CRYPTO_COMPAT_ERRNO(CRYPTO_ERROR_ENV, ERROR_LAYER_HAL, module, err_code)

enum {
    CRYPTO_SUCCESS          = 0,
    CRYPTO_FAILURE          = 0xffffffff,
};

#endif