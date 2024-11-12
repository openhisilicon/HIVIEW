/**
  Copyright (c), 2001-2024, Shenshu Tech. Co., Ltd.
 */
#ifndef CRYPTO_PKE_STRUCT_H
#define CRYPTO_PKE_STRUCT_H

#include "crypto_common_struct.h"

typedef enum {
    DRV_PKE_LEN_192 = 24,
    DRV_PKE_LEN_224 = 32,
    DRV_PKE_LEN_256 = 32,
    DRV_PKE_LEN_384 = 48,
    DRV_PKE_LEN_448 = 56,
    DRV_PKE_LEN_512 = 64,
    DRV_PKE_LEN_521 = 68,
    DRV_PKE_LEN_576 = 72,
    DRV_PKE_LEN_1024 = 128,
    DRV_PKE_LEN_1536 = 192,
    DRV_PKE_LEN_2048 = 256,
    DRV_PKE_LEN_3072 = 384,
    DRV_PKE_LEN_4096 = 512,
    DRV_PKE_LEN_MAX,
    DRV_PKE_LEN_INVALID = 0xffffffff,
} drv_pke_len;

/*
 * Component of drv_pke_ecc_curve_type.
 * reserved(22 bits) | curve_id(10 bits) | klen(10 bits)
 */
typedef enum {
    RFC5639_P256_CURVE_ID = 0,
    RFC5639_P384_CURVE_ID,
    RFC5639_P512_CURVE_ID,
    FIPS_P256K_CURVE_ID,
    FIPS_P192R_CURVE_ID,
    FIPS_P224R_CURVE_ID,
    FIPS_P256R_CURVE_ID,
    FIPS_P384R_CURVE_ID,
    FIPS_P521R_CURVE_ID,
    RFC7748_CURVE_ID,
    RFC7748_448_CURVE_ID,
    RFC8032_CURVE_ID,
    SM2_CURVE_ID,
    CURVE_ID_MAX,
} drv_pke_curve_id;

#define CRYPTO_PKE_CURVE_ID_MASK     0x000FFC00
#define CRYPTO_PKE_CURVE_ID_SHIFT    10
#define CRYPTO_PKE_KLEN_MASK        0x000003FF
#define CRYPTO_PKE_KLEN_SHIFT       0

#define crypto_pke_get_attr(value, mask, shift)         (((td_u32)(value) & (td_u32)(mask)) >> (shift))

#define crypto_pke_get_klen(curve_type)              \
    crypto_pke_get_attr(curve_type, CRYPTO_PKE_KLEN_MASK, CRYPTO_PKE_KLEN_SHIFT)
#define compat_pke_curve_type(curve_id, klen)       \
    ((((curve_id) & 0x3FF) << CRYPTO_PKE_CURVE_ID_SHIFT) | ((klen) & 0x3FF))

typedef enum {
    /* 0x20, RFC 5639 - Brainpool P256 */
    DRV_PKE_ECC_TYPE_RFC5639_P256 = compat_pke_curve_type(RFC5639_P256_CURVE_ID, DRV_PKE_LEN_256),
    /* 0x430, RFC 5639 - Brainpool P384 */
    DRV_PKE_ECC_TYPE_RFC5639_P384 = compat_pke_curve_type(RFC5639_P384_CURVE_ID, DRV_PKE_LEN_384),
    /* 0x840, RFC 5639 - Brainpool P512 */
    DRV_PKE_ECC_TYPE_RFC5639_P512 = compat_pke_curve_type(RFC5639_P512_CURVE_ID, DRV_PKE_LEN_512),
    /* 0xc20, RFC 5639 - Brainpool P512 */
    DRV_PKE_ECC_TYPE_FIPS_P256K = compat_pke_curve_type(FIPS_P256K_CURVE_ID, DRV_PKE_LEN_256),
    /* 0x1018, NIST FIPS 186-4 P192/224/256/384/521, suggest not to use */
    DRV_PKE_ECC_TYPE_FIPS_P192R = compat_pke_curve_type(FIPS_P192R_CURVE_ID, DRV_PKE_LEN_192),
    /* 0x1420, NIST FIPS 186-4 P192/224/256/384/521, suggest not to use */
    DRV_PKE_ECC_TYPE_FIPS_P224R = compat_pke_curve_type(FIPS_P224R_CURVE_ID, DRV_PKE_LEN_224),
    /* 0x1820, NIST FIPS 186-4 P192/224/256/384/521, suggest not to use */
    DRV_PKE_ECC_TYPE_FIPS_P256R = compat_pke_curve_type(FIPS_P256R_CURVE_ID, DRV_PKE_LEN_256),
    /* 0x1c30, NIST FIPS 186-4 P192/224/256/384/521, suggest not to use */
    DRV_PKE_ECC_TYPE_FIPS_P384R = compat_pke_curve_type(FIPS_P384R_CURVE_ID, DRV_PKE_LEN_384),
    /* 0x2048, NIST FIPS 186-4 P192/224/256/384/521, suggest not to use */
    DRV_PKE_ECC_TYPE_FIPS_P521R = compat_pke_curve_type(FIPS_P521R_CURVE_ID, DRV_PKE_LEN_576),
    /* 0x2420, RFC 7748 - Curve25519 */
    DRV_PKE_ECC_TYPE_RFC7748 = compat_pke_curve_type(RFC7748_CURVE_ID, DRV_PKE_LEN_256),
    /* 0x2838, RFC 7748 - Curve448 */
    DRV_PKE_ECC_TYPE_RFC7748_448 = compat_pke_curve_type(RFC7748_448_CURVE_ID, DRV_PKE_LEN_448),
    /* 0x2c20, RFC 8032 - ED25519 */
    DRV_PKE_ECC_TYPE_RFC8032 = compat_pke_curve_type(RFC8032_CURVE_ID, DRV_PKE_LEN_256),
    /* 0x3020, GMT 0003.2-2012 */
    DRV_PKE_ECC_TYPE_SM2 = compat_pke_curve_type(SM2_CURVE_ID, DRV_PKE_LEN_256),
    DRV_PKE_ECC_TYPE_MAX,
    DRV_PKE_ECC_TYPE_INVALID = 0xffffffff,
} drv_pke_ecc_curve_type;

typedef enum {
    DRV_PKE_RSA_SCHEME_PKCS1_V15 = 0x00,  /* PKCS#1 V15 */
    DRV_PKE_RSA_SCHEME_PKCS1_V21,         /* PKCS#1 V21, PSS for signning, OAEP for encryption */
    DRV_PKE_RSA_SCHEME_MAX,
    DRV_PKE_RSA_SCHEME_INVALID = 0xffffffff,
} drv_pke_rsa_scheme;

typedef enum {
    DRV_PKE_HASH_TYPE_SHA1 = 0x00,   /* Suggest Not to use */
    DRV_PKE_HASH_TYPE_SHA224,
    DRV_PKE_HASH_TYPE_SHA256,
    DRV_PKE_HASH_TYPE_SHA384,
    DRV_PKE_HASH_TYPE_SHA512,
    DRV_PKE_HASH_TYPE_SM3,
    DRV_PKE_HASH_TYPE_MAX,
    DRV_PKE_HASH_TYPE_INVALID = 0xffffffff,
} drv_pke_hash_type;

typedef enum {
    DRV_PKE_BUF_NONSECURE = 0x00,
    DRV_PKE_BUF_SECURE,
    DRV_PKE_BUF_INVALID = 0xffffffff,
} drv_pke_buffer_secure;

typedef struct {
    td_u32  length;
    td_u8  *data;
} drv_pke_data;

/* * struct of ecc point */
typedef struct {
    td_u8 *x;   /* X coordinates of the generated public key, the caller ensures it is padded with leading
                   zeros if the effective size of this key is smaller than ecc key size. */
    td_u8 *y;   /* Y coordinates of the generated public key, the caller ensures it is padded with leading
                   zeros if the effective size of this key is smaller than ecc key size. */
    td_u32 length;
} drv_pke_ecc_point;

/* * struct of ecc signature */
typedef struct {
    td_u8 *r;   /* r component of the signature. */
    td_u8 *s;   /* s component of the signature. */
    td_u32 length;
} drv_pke_ecc_sig;

/* * struct of ecc curves parameters. */
typedef struct {
    const td_u8 *p;   /* prime specifying the base field. It is p (RFC5639), p (FIPS), p (RFC7748). */
    const td_u8 *a;   /* Curve parameter a. It is A (RFC5639), c (FIPS), A24 (RFC7748), d(RFC8032). */
    const td_u8 *b;   /* Curve parameter b. It is B (RFC5639), b (FIPS), N/A (RFC7748, RFC8032). */
    const td_u8 *gx;  /* X coordinates of G which is a base point on the curve.
                         It is x (RFC5639), Gx (FIPS), U(P) (RFC7748). */
    const td_u8 *gy;  /* Y coordinates of G which is a base point on the curve.
                         It is y (RFC5639), Gy (FIPS), N/A (RFC7748). */
    const td_u8 *n;   /* Prime which is the order of G point. It is q (RFC5639), n (FIPS, RFC7748). */
    td_u32 h;         /* Cofactor, which is the order of the elliptic curve divided by the order of the point G.
                         It is h (RFC5639), h (FIPS), Cofactor (RFC7748). */
    drv_pke_len ksize;         /* Ecc key size in bytes. It corresponds to the size in bytes of the prime. */
    drv_pke_ecc_curve_type ecc_type; /* Type of ECC curve */
} drv_pke_ecc_curve;

typedef struct {
    td_u32  length;
    td_u8  *data;
    drv_pke_buffer_secure buf_sec;
} drv_pke_msg;

/* * RSA private key struct */
typedef struct {
    td_u8 *n;          /* *< public modulus */
    td_u8 *e;          /* *< public exponent */
    td_u8 *d;          /* *< private exponent */
    td_u8 *p;          /* *< 1st prime factor */
    td_u8 *q;          /* *< 2nd prime factor */
    td_u8 *dp;         /* *< D % (P - 1) */
    td_u8 *dq;         /* *< D % (Q - 1) */
    td_u8 *qp;         /* *< 1 / (Q % P) */
    td_u16 n_len;      /* *< length of public modulus */
    td_u16 e_len;      /* *< length of public exponent */
    td_u16 d_len;      /* *< length of private exponent */
    td_u16 p_len;      /* *< length of 1st prime factor,should be half of u16NLen */
    td_u16 q_len;      /* *< length of 2nd prime factor,should be half of u16NLen */
    td_u16 dp_len;     /* *< length of D % (P - 1),should be half of u16NLen */
    td_u16 dq_len;     /* *< length of D % (Q - 1),should be half of u16NLen */
    td_u16 qp_len;     /* *< length of 1 / (Q % P),should be half of u16NLen */
} drv_pke_rsa_priv_key;

/* * struct of RSA public key */
typedef struct {
    td_u8  *n;            /* point to public modulus */
    td_u8  *e;            /* point to public exponent */
    td_u16 len;           /* length of public modulus, max value is 512Byte */
} drv_pke_rsa_pub_key;

typedef struct {
    const td_u8 *mont_a;    /* the montgomerized of parameter a(RFC5639, FIPS, SM2), a24(RFC7748), d(RFC8032). */
    const td_u8 *mont_b;    /* the montgomerized of parameter b(RFC5639, FIPS, SM2), N/A(RFC7748), sqrt_m1(RFC8032). */
    const td_u8 *mont_1_p;  /* the montgomerized of const value 1 (modp) */
    const td_u8 *mont_1_n;  /* the montgomerized of const value 1 (modn) */
    const td_u8 *rrp;  /* the montgomery parameter when modulur is p */
    const td_u8 *rrn;  /* the montgomery parameter when modulur is n */
    const td_u8 *const_1;
    const td_u8 *const_0;
    const td_u32 *mont_param_n;  /* the montgomerized parameter when the modulur is n. */
    const td_u32 *mont_param_p;  /* the montgomerized parameter when the modulur is p. */
    const td_u8 *n_minus_2;     /* the const value of n minus 2 */
    const td_u8 *p_minus_2;     /* the const value of p minus 2 */
} pke_ecc_init_param;

typedef struct {
    const drv_pke_ecc_curve *curve_param;
    const pke_ecc_init_param *default_param;
} pke_default_parameters;

/* Here the ram should have been set into the address, and the instructions have shown which DRAM will be used for
calculate. */
typedef struct {
    td_u32 instr_addr;     /* the start address for instructions, which is ROM or SRAM address. */
    td_u32 instr_num;       /* the number of instructions from the instr_addr, which will be used for calculate. */
} rom_lib;

#endif