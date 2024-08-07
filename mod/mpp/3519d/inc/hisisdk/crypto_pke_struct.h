/*
  Copyright (c), 2001-2024, Shenshu Tech. Co., Ltd.
 */

#ifndef CRYPTO_PKE_STRUCT_H
#define CRYPTO_PKE_STRUCT_H

#include "crypto_common_struct.h"

typedef enum {
    DRV_PKE_LEN_256 = 32,
    DRV_PKE_LEN_384 = 48,
    DRV_PKE_LEN_512 = 64,
    DRV_PKE_LEN_521 = 68,
    DRV_PKE_LEN_576 = 72,
    DRV_PKE_LEN_2048 = 256,
    DRV_PKE_LEN_3072 = 384,
    DRV_PKE_LEN_4096 = 512,
    DRV_PKE_LEN_MAX,
    DRV_PKE_LEN_INVALID = 0xffffffff,
} drv_pke_len;

typedef enum {
    DRV_PKE_ECC_TYPE_RFC5639_P256 = 0,      /* RFC 5639 - Brainpool P256/384/512 */
    DRV_PKE_ECC_TYPE_RFC5639_P384,          /* RFC 5639 - Brainpool P256/384/512 */
    DRV_PKE_ECC_TYPE_RFC5639_P512,          /* RFC 5639 - Brainpool P256/384/512 */
    DRV_PKE_ECC_TYPE_FIPS_P256K,            /* NIST FIPS 186-4 P256/384/521, suggest not to use */
    DRV_PKE_ECC_TYPE_FIPS_P256R,            /* NIST FIPS 186-4 P256/384/521, suggest not to use */
    DRV_PKE_ECC_TYPE_FIPS_P384R,            /* NIST FIPS 186-4 P256/384/521, suggest not to use */
    DRV_PKE_ECC_TYPE_FIPS_P521R,            /* NIST FIPS 186-4 P256/384/521, suggest not to use */
    DRV_PKE_ECC_TYPE_RFC7748,               /* RFC 7748 - Curve25519 */
    DRV_PKE_ECC_TYPE_RFC8032,               /* RFC 8032 - ED25519 */
    DRV_PKE_ECC_TYPE_SM2,                   /* GMT 0003.2-2012 */
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
    const td_u8 *a;   /* Curve parameter a. It is A (RFC5639), c (FIPS), A24 (RFC7748). */
    const td_u8 *b;   /* Curve parameter b. It is B (RFC5639), b (FIPS), N/A (RFC7748). */
    const td_u8 *gx;  /* X coordinates of G which is a base point on the curve.
                         It is x (RFC5639), Gx (FIPS), U(P) (RFC7748). */
    const td_u8 *gy;  /* Y coordinates of G which is a base point on the curve.
                         It is y (RFC5639), Gy (FIPS), N/A (RFC7748). */
    const td_u8 *n;   /* Prime which is the order of G point. It is q (RFC5639), n (FIPS), N/A (RFC7748). */
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

#endif