/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */

#ifndef HI_MPI_CIPHER_H
#define HI_MPI_CIPHER_H

#include "hi_type.h"
#include "crypto_type.h"
#include "crypto_hash_struct.h"
#include "crypto_kdf_struct.h"
#include "crypto_pke_struct.h"
#include "crypto_symc_struct.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/* HASH */
hi_s32 hi_mpi_cipher_hash_init(hi_void);

hi_s32 hi_mpi_cipher_hash_deinit(hi_void);

hi_s32 hi_mpi_cipher_hash_create(hi_handle *mpi_hash_handle, const crypto_hash_attr *hash_attr);

hi_s32 hi_mpi_cipher_hash_update(hi_handle mpi_hash_handle, const crypto_buf_attr *src_buf, const hi_u32 len);

hi_s32 hi_mpi_cipher_hash_finish(hi_handle mpi_hash_handle, hi_u8 *virt_addr, hi_u32 buffer_len, hi_u32 *result_len);

hi_s32 hi_mpi_cipher_hash_get(hi_handle mpi_hash_handle, crypto_hash_clone_ctx *hash_clone_ctx);

hi_s32 hi_mpi_cipher_hash_set(hi_handle mpi_hash_handle, const crypto_hash_clone_ctx *hash_clone_ctx);

hi_s32 hi_mpi_cipher_hash_destroy(hi_handle mpi_hash_handle);

/* KDF */
hi_s32 hi_mpi_cipher_pbkdf2(const crypto_kdf_pbkdf2_param *param, hi_u8 *out, const hi_u32 out_len);

/* PKE */
hi_s32 hi_mpi_cipher_pke_init(hi_void);
hi_s32 hi_mpi_cipher_pke_deinit(hi_void);

/*
 * input_priv_key: Could be NULL.
 */
hi_s32 hi_mpi_cipher_pke_ecc_gen_key(drv_pke_ecc_curve_type curve_type, const drv_pke_data *input_priv_key,
    const drv_pke_data *output_priv_key, const drv_pke_ecc_point *output_pub_key);

hi_s32 hi_mpi_cipher_pke_ecc_gen_ecdh_key(drv_pke_ecc_curve_type curve_type, const drv_pke_ecc_point *input_pub_key,
    const drv_pke_data *input_priv_key, const drv_pke_data *output_shared_key);

hi_s32 hi_mpi_cipher_pke_ecdsa_sign(drv_pke_ecc_curve_type curve_type, const drv_pke_data *priv_key,
    const drv_pke_data *hash, const drv_pke_ecc_sig *sig);

hi_s32 hi_mpi_cipher_pke_ecdsa_verify(drv_pke_ecc_curve_type curve_type, const drv_pke_ecc_point *pub_key,
    const drv_pke_data *hash, const drv_pke_ecc_sig *sig);

hi_s32 hi_mpi_cipher_pke_eddsa_sign(drv_pke_ecc_curve_type curve_type, const drv_pke_data *priv_key,
    const drv_pke_msg *msg, const drv_pke_ecc_sig *sig);

hi_s32 hi_mpi_cipher_pke_eddsa_verify(drv_pke_ecc_curve_type curve_type, const drv_pke_ecc_point *pub_key,
    const drv_pke_msg *msg, const drv_pke_ecc_sig *sig);

hi_s32 hi_mpi_cipher_pke_check_dot_on_curve(drv_pke_ecc_curve_type curve_type, const drv_pke_ecc_point *pub_key,
    hi_bool *is_on_curve);

hi_s32 hi_mpi_cipher_pke_sm2_dsa_hash(const drv_pke_data *sm2_id, const drv_pke_ecc_point *pub_key,
    const drv_pke_msg *msg, drv_pke_data *hash);

hi_s32 hi_mpi_cipher_pke_sm2_public_encrypt(const drv_pke_ecc_point *pub_key, const drv_pke_data *plain_text,
    drv_pke_data *cipher_text);

hi_s32 hi_mpi_cipher_pke_sm2_private_decrypt(const drv_pke_data *priv_key, const drv_pke_data *cipher_text,
    drv_pke_data *plain_text);

/* RSA. */
hi_s32 hi_mpi_cipher_pke_rsa_sign(const drv_pke_rsa_priv_key *priv_key, drv_pke_rsa_scheme scheme,
    drv_pke_hash_type hash_type, const drv_pke_data *input_hash,
    drv_pke_data *sign);

hi_s32 hi_mpi_cipher_pke_rsa_verify(const drv_pke_rsa_pub_key *pub_key, drv_pke_rsa_scheme scheme,
    drv_pke_hash_type hash_type, drv_pke_data *input_hash, const drv_pke_data *sig);

hi_s32 hi_mpi_cipher_pke_rsa_public_encrypt(drv_pke_rsa_scheme scheme, drv_pke_hash_type hash_type,
    const drv_pke_rsa_pub_key *pub_key, const drv_pke_data *input, const drv_pke_data *label,
    drv_pke_data *output);

hi_s32 hi_mpi_cipher_pke_rsa_private_decrypt(drv_pke_rsa_scheme scheme, drv_pke_hash_type hash_type,
    const drv_pke_rsa_priv_key *priv_key, const drv_pke_data *input, const drv_pke_data *label,
    drv_pke_data *output);

/* SYMC */
hi_s32 hi_mpi_cipher_symc_init(hi_void);

hi_s32 hi_mpi_cipher_symc_deinit(hi_void);

hi_s32 hi_mpi_cipher_symc_create(hi_handle *symc_handle, const crypto_symc_attr *symc_attr);

hi_s32 hi_mpi_cipher_symc_destroy(hi_handle symc_handle);

hi_s32 hi_mpi_cipher_symc_set_config(hi_handle symc_handle, const crypto_symc_ctrl_t *symc_ctrl);

hi_s32 hi_mpi_cipher_symc_attach(hi_handle symc_handle, hi_handle keyslot_handle);

hi_s32 hi_mpi_cipher_symc_encrypt(hi_handle symc_handle, const crypto_buf_attr *src_buf,
    const crypto_buf_attr *dst_buf, hi_u32 length);

hi_s32 hi_mpi_cipher_symc_decrypt(hi_handle symc_handle, const crypto_buf_attr *src_buf,
    const crypto_buf_attr *dst_buf, hi_u32 length);

hi_s32 hi_mpi_cipher_symc_encrypt_multi(hi_handle symc_handle, const crypto_symc_ctrl_t *symc_ctrl,
    const crypto_symc_pack *src_buf_pack, const crypto_symc_pack *dst_buf_pack, hi_u32 pack_num);

hi_s32 hi_mpi_cipher_symc_decrypt_multi(hi_handle symc_handle, const crypto_symc_ctrl_t *symc_ctrl,
    const crypto_symc_pack *src_buf_pack, const crypto_symc_pack *dst_buf_pack, hi_u32 pack_num);

hi_s32 hi_mpi_cipher_symc_get_tag(hi_handle symc_handle, hi_u8 *tag, hi_u32 tag_length);

hi_s32 hi_mpi_cipher_mac_start(hi_handle *symc_handle, const crypto_symc_mac_attr *mac_attr);

hi_s32 hi_mpi_cipher_mac_update(hi_handle symc_handle, const crypto_buf_attr *src_buf, hi_u32 length);

hi_s32 hi_mpi_cipher_mac_finish(hi_handle symc_handle, hi_u8 *mac, hi_u32 *mac_length);

/* TRNG */
hi_s32 hi_mpi_cipher_trng_get_random(hi_u32 *randnum);

hi_s32 hi_mpi_cipher_trng_get_multi_random(hi_u32 size, hi_u8 *randnum);

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif