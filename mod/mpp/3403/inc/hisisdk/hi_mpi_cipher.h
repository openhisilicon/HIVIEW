/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */

#ifndef __HI_MPI_CIPHER_H__
#define __HI_MPI_CIPHER_H__

#include "hi_common_cipher.h"

#ifdef __cplusplus
extern "C" {
#endif

hi_s32 hi_mpi_cipher_init(hi_void);

hi_s32 hi_mpi_cipher_deinit(hi_void);

hi_s32 hi_mpi_cipher_create(hi_handle *handle, const hi_cipher_attr *cipher_attr);

hi_s32 hi_mpi_cipher_destroy(hi_handle handle);

hi_s32 hi_mpi_cipher_set_cfg(hi_handle handle, const hi_cipher_ctrl *ctrl);

hi_s32 hi_mpi_cipher_get_cfg(hi_handle handle, hi_cipher_ctrl *ctrl);

hi_s32 hi_mpi_cipher_encrypt(hi_handle handle,
    hi_phys_addr_t src_phys_addr, hi_phys_addr_t dst_phys_addr, hi_u32 byte_len);

hi_s32 hi_mpi_cipher_decrypt(hi_handle handle,
    hi_phys_addr_t src_phys_addr, hi_phys_addr_t dst_phys_addr, hi_u32 byte_len);

hi_s32 hi_mpi_cipher_encrypt_virt(hi_handle handle, const hi_u8 *src_data, hi_u8 *dst_data, hi_u32 byte_len);

hi_s32 hi_mpi_cipher_decrypt_virt(hi_handle handle, const hi_u8 *src_data, hi_u8 *dst_data, hi_u32 byte_len);

hi_s32 hi_mpi_cipher_encrypt_multi_pack(hi_handle handle, const hi_cipher_data *data_pack, hi_u32 data_pack_num);

hi_s32 hi_mpi_cipher_decrypt_multi_pack(hi_handle handle, const hi_cipher_data *data_pack, hi_u32 data_pack_num);

hi_s32 hi_mpi_cipher_get_tag(hi_handle handle, hi_u8 *tag, hi_u32 tag_len);

hi_s32 hi_mpi_cipher_hash_init(const hi_cipher_hash_attr *hash_attr, hi_handle *handle);

hi_s32 hi_mpi_cipher_hash_update(hi_handle handle, const hi_u8 *in_data, hi_u32 in_data_len);

hi_s32 hi_mpi_cipher_hash_final(hi_handle handle, hi_u8 *out_hash, hi_u32 out_hash_len);

hi_s32 hi_mpi_cipher_get_random_num(hi_u32 *random_num);

hi_s32 hi_mpi_cipher_rsa_public_encrypt(hi_cipher_rsa_scheme scheme,
    hi_cipher_hash_type sha_type, const hi_cipher_rsa_public_key *rsa_key,
    const hi_cipher_common_data *plain_txt, hi_cipher_common_data *cipher_txt);

hi_s32 hi_mpi_cipher_rsa_private_decrypt(hi_cipher_rsa_scheme scheme,
    hi_cipher_hash_type sha_type, const hi_cipher_rsa_private_key *rsa_key,
    const hi_cipher_common_data *cipher_txt, hi_cipher_common_data *plain_txt);

hi_s32 hi_mpi_cipher_rsa_private_encrypt(hi_cipher_rsa_scheme scheme,
    hi_cipher_hash_type sha_type, const hi_cipher_rsa_private_key *rsa_key,
    const hi_cipher_common_data *plain_txt, hi_cipher_common_data *cipher_txt);

hi_s32 hi_mpi_cipher_rsa_public_decrypt(hi_cipher_rsa_scheme scheme,
    hi_cipher_hash_type sha_type, const hi_cipher_rsa_public_key *rsa_key,
    const hi_cipher_common_data *cipher_txt, hi_cipher_common_data *plain_txt);

hi_s32 hi_mpi_cipher_rsa_sign(hi_cipher_rsa_scheme scheme,
    hi_cipher_hash_type sha_type, const hi_cipher_rsa_private_key *rsa_key,
    const hi_cipher_sign_in_data *rsa_data, hi_cipher_common_data *sign_data);

hi_s32 hi_mpi_cipher_rsa_verify(hi_cipher_rsa_scheme scheme,
    hi_cipher_hash_type sha_type, const hi_cipher_rsa_public_key *rsa_key,
    const hi_cipher_sign_in_data *rsa_data, const hi_cipher_common_data *sign_data);

hi_s32 hi_mpi_cipher_sm2_encrypt(const hi_cipher_sm2_public_key *sm2_key,
    const hi_cipher_common_data *plain_txt, hi_cipher_common_data *cipher_txt);

hi_s32 hi_mpi_cipher_sm2_decrypt(const hi_cipher_sm2_private_key *sm2_key,
    const hi_cipher_common_data *cipher_txt, hi_cipher_common_data *plain_txt);

hi_s32 hi_mpi_cipher_sm2_sign(const hi_cipher_sm2_sign *sm2_sign,
    const hi_cipher_sign_in_data *sm2_data, hi_cipher_sm2_sign_data *sign_data);

hi_s32 hi_mpi_cipher_sm2_verify(const hi_cipher_sm2_verify *sm2_verify,
    const hi_cipher_sign_in_data *sm2_data, const hi_cipher_sm2_sign_data *sign_data);

hi_s32 hi_mpi_keyslot_create(const hi_keyslot_attr *attr, hi_handle *keyslot);

hi_s32 hi_mpi_keyslot_destroy(hi_handle keyslot);

hi_s32 hi_mpi_cipher_attach(hi_handle cipher, hi_handle keyslot);

hi_s32 hi_mpi_cipher_detach(hi_handle cipher, hi_handle keyslot);

#ifdef __cplusplus
}
#endif

#endif /* __HI_MPI_CIPHER_H__ */
