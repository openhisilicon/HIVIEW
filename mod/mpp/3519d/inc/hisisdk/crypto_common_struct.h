/*
  Copyright (c), 2001-2024, Shenshu Tech. Co., Ltd.
 */

#ifndef CRYPTO_COMMON_STRUCT_H
#define CRYPTO_COMMON_STRUCT_H

#include "crypto_type.h"

/* ! \union of compat addr */
typedef union {
    void *p;                /* !<  virtual address */
    unsigned long long phy; /* !<  physical address */
    unsigned int word[2];   /* !<  2 word of address */
} crypto_compat_addr;

#ifndef ADDR_H32
#define ADDR_H32(addr) (addr).word[1] /* !<  High 32 bit of td_u64 */
#endif
#ifndef ADDR_L32
#define ADDR_L32(addr) (addr).word[0] /* !<  Low 32 bit of td_u64 */
#endif
#ifndef ADDR_U64
#define ADDR_U64(addr) (addr).phy     /* !<  64 bit of td_u64 */
#endif
#ifndef ADDR_VIA
#define ADDR_VIA(addr) (addr).p       /* !<  buffer point */
#endif

typedef enum {
    CRYPTO_BUF_SECURE,
    CRYPTO_BUF_NONSECURE,
} crypto_buffer_secure;

typedef struct {
    unsigned long long uapi_mem_handle;     /* Handle to buffer header address */
    unsigned long long addr_offset;         /* buffer offset, unused. */
    td_void *kapi_mem_handle;

    unsigned long phys_addr;
    void *virt_addr;
    crypto_buffer_secure buf_sec;           /* NONSECURE or SECURE */
} crypto_buf_attr;

typedef enum {
    KEYSLOT_ENGINE_AES,
    KEYSLOT_ENGINE_SM4
} keyslot_engine;

#endif