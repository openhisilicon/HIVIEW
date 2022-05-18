/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2019-2020. All rights reserved.
 * Description   : hi_mpi_cipher.h
 * Author        : Hisilicon multimedia software group
 * Create        : 2019-05-20
 */

#ifndef __HI_MPI_CIPHER_H__
#define __HI_MPI_CIPHER_H__

#include "hi_common_cipher.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

/*
 * attention \n
 * This API is used to start the cipher device.
 * param N/A
 * retval ::HI_SUCCESS        Call this API successful.
 * retval ::HI_FAILURE        Call this API fails.
 * retval ::HI_ERR_CIPHER_FAILED_INIT  The cipher device fails to be initialized.
 * see \n
 * N/A
 */
hi_s32 hi_mpi_cipher_init(hi_void);

/*
 * brief  Deinit the cipher device.
 * attention \n
 * This API is used to stop the cipher device. If this API is called repeatedly, HI_SUCCESS is returned, but only the
 * first operation takes effect.
 *
 * param N/A
 * retval ::HI_SUCCESS        Call this API successful.
 * retval ::HI_FAILURE        Call this API fails.
 * retval ::HI_ERR_CIPHER_NOT_INIT  The cipher device is not initialized.
 * see \n
 * N/A
 */
hi_s32 hi_mpi_cipher_deinit(hi_void);

/*
 * brief Obtain a cipher handle for encryption and decryption.
 *
 * param[out] handle          Cipher handle
 * param[in]  cipher_attr     Cipher attributes
 * retval ::HI_SUCCESS        Call this API successful.
 * retval ::HI_FAILURE        Call this API fails.
 * retval ::HI_ERR_CIPHER_NOT_INIT  The cipher device is not initialized.
 * retval ::HI_ERR_CIPHER_INVALID_POINT  The pointer is null.
 * retval ::HI_ERR_CIPHER_FAILED_GETHANDLE  The cipher handle fails to be obtained, because there are no available
 * cipher handles.
 * see \n
 * N/A
 */
hi_s32 hi_mpi_cipher_create_handle(hi_handle *handle, const hi_cipher_attr *cipher_attr);

/*
 * attention \n
 * This API is used to destroy existing cipher handles.
 *
 * param[in] handle           Cipher handle
 * retval ::HI_SUCCESS        Call this API successful.
 * retval ::HI_FAILURE        Call this API fails.
 * retval ::HI_ERR_CIPHER_NOT_INIT  The cipher device is not initialized.
 * see \n
 * N/A
 */
hi_s32 hi_mpi_cipher_destroy_handle(hi_handle handle);

/*
 * brief Configures the cipher control information.
 * attention \n
 * Before encryption or decryption, you must call this API to configure the cipher control information.
 * The first 64-bit data and the last 64-bit data should not be the same when using TDES algorithm.
 *
 * param[in] handle           Cipher handle.
 * param[in] ctrl             Cipher control information.
 * retval ::HI_SUCCESS        Call this API successful.
 * retval ::HI_FAILURE        Call this API fails.
 * retval ::HI_ERR_CIPHER_NOT_INIT  The cipher device is not initialized.
 * retval ::HI_ERR_CIPHER_INVALID_POINT  The pointer is null.
 * retval ::HI_ERR_CIPHER_INVALID_PARAM  The parameter is invalid.
 * retval ::HI_ERR_CIPHER_INVALID_HANDLE  The handle is invalid.
 * see \n
 * N/A
 */
hi_s32 hi_mpi_cipher_cfg_handle(hi_handle handle, const hi_cipher_ctrl *ctrl);

/*
 * brief Configures the cipher control information.
 * attention \n
 * Before encryption or decryption, you must call this API to configure the cipher control information.
 * The first 64-bit data and the last 64-bit data should not be the same when using TDES algorithm.
 *
 * param[in] handle           Cipher handle.
 * param[in] ctrl_ex          Cipher control information.
 * retval ::HI_SUCCESS        Call this API successful.
 * retval ::HI_FAILURE        Call this API fails.
 * retval ::HI_ERR_CIPHER_NOT_INIT  The cipher device is not initialized.
 * retval ::HI_ERR_CIPHER_INVALID_POINT  The pointer is null.
 * retval ::HI_ERR_CIPHER_INVALID_PARAM  The parameter is invalid.
 * retval ::HI_ERR_CIPHER_INVALID_HANDLE  The handle is invalid.
 * see \n
 * N/A
 */
hi_s32 hi_mpi_cipher_cfg_handle_ex(hi_handle handle, const hi_cipher_ctrl_ex *ctrl_ex);

/*
 * brief Performs encryption.
 *
 * attention \n
 * This API is used to perform encryption by using the cipher module.
 * The length of the encrypted data should be a multiple of 8 in TDES mode and 16 in AES mode. Besides, the length can
 * not be bigger than 0xFFFFF.After this operation, the result will affect next operation.If you want to remove vector,
 * you need to config IV(config pstCtrl->stChgFlags.bits_IV with 1) by transfering HI_UNF_CIPHER_ConfigHandle.
 * param[in] handle           Cipher handle
 * param[in] src_phys_addr    Physical address of the source data
 * param[in] dst_phys_addr    Physical address of the target data
 * param[in] byte_len         Length of the encrypted data
 * retval ::HI_SUCCESS        Call this API successful.
 * retval ::HI_FAILURE        Call this API fails.
 * retval ::HI_ERR_CIPHER_NOT_INIT  The cipher device is not initialized.
 * retval ::HI_ERR_CIPHER_INVALID_PARAM  The parameter is invalid.
 * retval ::HI_ERR_CIPHER_INVALID_HANDLE  The handle is invalid.
 * see \n
 * N/A
 */
hi_s32 hi_mpi_cipher_encrypt(hi_handle handle, hi_phys_addr_t src_phys_addr, hi_phys_addr_t dst_phys_addr,
    hi_u32 byte_len);

/*
 * brief Performs decryption.
 *
 * attention \n
 * This API is used to perform decryption by using the cipher module.
 * The length of the decrypted data should be a multiple of 8 in TDES mode and 16 in AES mode. Besides, the length can
 * not be bigger than 0xFFFFF.After this operation, the result will affect next operation.If you want to remove vector,
 * you need to config IV(config pstCtrl->stChgFlags.bits_IV with 1) by transfering HI_UNF_CIPHER_ConfigHandle.
 * param[in] handle           Cipher handle.
 * param[in] src_phys_addr    Physical address of the source data.
 * param[in] dst_phys_addr    Physical address of the target data.
 * param[in] byte_len         Length of the decrypted data
 * retval ::HI_SUCCESS        Call this API successful.
 * retval ::HI_FAILURE        Call this API fails.
 * retval ::HI_ERR_CIPHER_NOT_INIT  The cipher device is not initialized.
 * retval ::HI_ERR_CIPHER_INVALID_PARAM  The parameter is invalid.
 * retval ::HI_ERR_CIPHER_INVALID_HANDLE  The handle is invalid.
 * see \n
 * N/A
 */
hi_s32 hi_mpi_cipher_decrypt(hi_handle handle, hi_phys_addr_t src_phys_addr, hi_phys_addr_t dst_phys_addr,
    hi_u32 byte_len);

/*
 * brief Performs encryption.
 *
 * attention \n
 * This API is used to perform encryption by using the cipher module.
 * The length of the encrypted data should be a multiple of 8 in TDES mode and 16 in AES mode. Besides, the length can
 * not be bigger than 0xFFFFF.After this operation, the result will affect next operation.If you want to remove vector,
 * you need to config IV(config pstCtrl->stChgFlags.bits_IV with 1) by transfering HI_UNF_CIPHER_ConfigHandle.
 * param[in] handle           Cipher handle
 * param[in] src_data:        buffer of the source data.
 * param[out] dst_data:      buffer of the target data
 * param[in] byte_len      Length of the encrypted data
 * retval ::HI_SUCCESS        Call this API successful.
 * retval ::HI_FAILURE        Call this API fails.
 * retval ::HI_ERR_CIPHER_NOT_INIT  The cipher device is not initialized.
 * retval ::HI_ERR_CIPHER_INVALID_PARAM  The parameter is invalid.
 * retval ::HI_ERR_CIPHER_INVALID_HANDLE  The handle is invalid.
 * see \n
 * N/A
 */
hi_s32 hi_mpi_cipher_encrypt_virt(hi_handle handle, const hi_u8 *src_data, hi_u8 *dst_data, hi_u32 byte_len);

/*
 * brief Performs decryption.
 *
 * attention \n
 * This API is used to perform decryption by using the cipher module.
 * The length of the decrypted data should be a multiple of 8 in TDES mode and 16 in AES mode. Besides, the length can
 * not be bigger than 0xFFFFF.After this operation, the result will affect next operation.If you want to remove vector,
 * you need to config IV(config pstCtrl->stChgFlags.bits_IV with 1) by transfering HI_UNF_CIPHER_ConfigHandle.
 * param[in] handle           Cipher handle.
 * param[in] src_data:        buffer of the source data.
 * param[out] dst_data:       buffer of the target data
 * param[in] byte_len         Length of the decrypted data
 * retval ::HI_SUCCESS        Call this API successful.
 * retval ::HI_FAILURE        Call this API fails.
 * retval ::HI_ERR_CIPHER_NOT_INIT  The cipher device is not initialized.
 * retval ::HI_ERR_CIPHER_INVALID_PARAM  The parameter is invalid.
 * retval ::HI_ERR_CIPHER_INVALID_HANDLE  The handle is invalid.
 * see \n
 * N/A
 */
hi_s32 hi_mpi_cipher_decrypt_virt(hi_handle handle, const hi_u8 *src_data, hi_u8 *dst_data, hi_u32 byte_len);

/*
 * brief Encrypt multiple packaged data.
 * attention \n
 * You can not encrypt more than 128 data package one time. When HI_ERR_CIPHER_BUSY return, the data package you send
 * will not be deal, the customer should decrease the number of data package or run cipher again.Note:When encrypting
 * more than one packaged data, every one package will be calculated using initial vector configured by
 * hi_mpi_cipher_cfg_handle. Previous result will not affect the later result.
 * param[in] handle           cipher handle
 * param[in] data_pack        data package ready for cipher
 * param[in] data_pack_num    number of package ready for cipher
 * retval ::HI_SUCCESS        Call this API successful.
 * retval ::HI_FAILURE        Call this API fails.
 * retval ::HI_ERR_CIPHER_NOT_INIT  cipher device have not been initialized
 * retval ::HI_ERR_CIPHER_INVALID_PARAM  parameter error
 * retval ::HI_ERR_CIPHER_INVALID_HANDLE  handle invalid
 * retval ::HI_ERR_CIPHER_BUSY  hardware is busy, it can not deal with all data package once time
 * see \n
 * N/A
 */
hi_s32 hi_mpi_cipher_encrypt_multi_pack(hi_handle handle, const hi_cipher_data *data_pack, hi_u32 data_pack_num);

/*
 * brief Decrypt multiple packaged data.
 * attention \n
 * You can not decrypt more than 128 data package one time.When HI_ERR_CIPHER_BUSY return, the data package you send
 * will not be deal, the custmer should decrease the number of data package or run cipher again.Note:When decrypting
 * more than one packaged data, every one package will be calculated using initial vector configured by
 * hi_mpi_cipher_cfg_handle.Previous result will not affect the later result.
 * param[in] handle           cipher handle
 * param[in] data_pack        data package ready for cipher
 * param[in] data_pack_num    number of package ready for cipher
 * retval ::HI_SUCCESS        Call this API successful.
 * retval ::HI_FAILURE        Call this API fails.
 * retval ::HI_ERR_CIPHER_NOT_INIT  cipher device have not been initialized
 * retval ::HI_ERR_CIPHER_INVALID_PARAM  parameter error
 * retval ::HI_ERR_CIPHER_INVALID_HANDLE  handle invalid
 * retval ::HI_ERR_CIPHER_BUSY  hardware is busy, it can not deal with all data package once time
 * see \n
 * N/A
 */
hi_s32 hi_mpi_cipher_decrypt_multi_pack(hi_handle handle, const hi_cipher_data *data_pack, hi_u32 data_pack_num);

/*
 * brief Get the cipher control information.
 *
 * param[in] handle           Cipher handle.
 * param[in] ctrl             Cipher control information, can not get key.
 * retval ::HI_SUCCESS        Call this API successful.
 * retval ::HI_FAILURE        Call this API fails.
 * retval ::HI_ERR_CIPHER_NOT_INIT  The cipher device is not initialized.
 * retval ::HI_ERR_CIPHER_INVALID_POINT  The pointer is null.
 * retval ::HI_ERR_CIPHER_INVALID_PARAM  The parameter is invalid.
 * retval ::HI_ERR_CIPHER_INVALID_HANDLE  The handle is invalid.
 * see \n
 * N/A
 */
hi_s32 hi_mpi_cipher_get_handle_cfg(hi_handle handle, hi_cipher_ctrl *ctrl);

/*
 * brief Encrypt the clean key data by KLAD.
 * attention \n
 * N/A
 * param[in] root_key         klad root key.
 * param[in] target           clean key.
 * param[in] clean_key        the module who to use this key.
 * param[out] ecnrypt_key     encrypt key.
 * param[in] key_len clean    key.
 * retval ::HI_SUCCESS        Call this API successful.
 * retval ::HI_FAILURE        Call this API fails.
 * retval ::HI_ERR_CIPHER_NOT_INIT  The cipher device is not initialized.
 * retval ::HI_ERR_CIPHER_INVALID_POINT  The pointer is null.
 * retval ::HI_ERR_CIPHER_INVALID_PARAM  The parameter is invalid.
 * retval ::HI_ERR_CIPHER_INVALID_HANDLE  The handle is invalid.
 * see \n
 * N/A
 */
hi_s32 hi_mpi_cipher_klad_encrypt_key(hi_cipher_ca_type root_key, hi_cipher_klad_target target, const hi_u8 *clean_key,
    hi_u8 *ecnrypt_key, hi_u32 key_len);

/*
 * brief Init the hash module, if other program is using the hash module, the API will return failure.
 *
 * attention \n
 * N/A
 * param[in] hash_attr:       The hash calculating structure input.
 * param[out] handle:         The out hash handle.
 * retval ::HI_SUCCESS        Call this API successful.
 * retval ::HI_FAILURE        Call this API fails.
 * see \n
 * N/A
 */
hi_s32 hi_mpi_cipher_hash_init(const hi_cipher_hash_attr *hash_attr, hi_handle *handle);


/*
 * brief Calculate the hash, if the size of the data to be calculated is very big and the DDR ram is not enough,
 * this API can calculate the data one block by one block. Attention: The input block length must be 64bytes aligned
 * except for the last block.
 *
 * attention \n
 * N/A
 * param[in] handle:          Hash handle.
 * param[in] in_data:         The input data buffer.
 * param[in] in_data_len:     The input data length, attention: the block length input must be 64bytes aligned except
 * the last block!
 * retval ::HI_SUCCESS        Call this API successful.
 * retval ::HI_FAILURE        Call this API fails.
 * see \n
 * N/A
 */
hi_s32 hi_mpi_cipher_hash_update(hi_handle handle, const hi_u8 *in_data, hi_u32 in_data_len);

/*
 * brief Get the final hash value, after calculate all of the data, call this API to get the final hash value and close
 * the handle.If there is some reason need to interrupt the calculation, this API should also be call to close the
 * handle.
 *
 * attention \n
 * N/A
 * param[in]  handle:         Hash handle.
 * param[out] out_hash:       The final output hash value.
 * param[in]  out_hash_buf_len:  The final output hash buffer length.
 * param[out] out_hash_len:   The final output hash length.
 * retval ::HI_SUCCESS        Call this API successful.
 * retval ::HI_FAILURE        Call this API fails.
 *
 * see \n
 * N/A
 */
hi_s32 hi_mpi_cipher_hash_final(hi_handle handle, hi_u8 *out_hash, hi_u32 out_hash_buf_len, hi_u32 *out_hash_len);

/*
 * brief Get the tag data of CCM/GCM.
 *
 * attention \n
 * This API is used to get the tag data of CCM/GCM.
 * param[in] handle           cipher handle
 * param[out] tag tag         data of CCM/GCM
 * param[in]  tag_buf_len     tag buf length of CCM/GCM.
 * param[out] tag_len tag     data length of CCM/GCM.
 * retval ::HI_SUCCESS        Call this API succussful.
 * retval ::HI_FAILURE        Call this API fails.
 * retval ::HI_ERR_CIPHER_NOT_INIT  The cipher device is not initialized.
 * retval ::HI_ERR_CIPHER_INVALID_PARAM  The parameter is invalid.
 * see \n
 * N/A
 */
hi_s32 hi_mpi_cipher_get_tag(hi_handle handle, hi_u8 *tag, hi_u32 tag_buf_len, hi_u32 *tag_len);

/*
 * brief Get the random number.
 *
 * attention \n
 * This API is used to obtain the random number from the hardware.
 *
 * param[out] random_num      Point to the random number.
 * retval ::HI_SUCCESS        Call this API successful.
 * retval ::HI_FAILURE        Call this API fails.
 *
 * see \n
 * N/A
 */
hi_s32 hi_mpi_cipher_get_random_num(hi_u32 *random_num);

/*
 * brief RSA encryption a plaintext with a RSA public key.
 *
 * attention \n
 * N/A
 *
 * param[in]  rsa_encrypt:    encryption struct.
 * param[in]  rsa_data:       rsa data struct.
 * retval ::HI_SUCCESS        Call this API successful.
 * retval ::HI_FAILURE        Call this API fails.
 *
 * see \n
 * N/A
 */
hi_s32 hi_mpi_cipher_rsa_pub_encrypt(const hi_cipher_rsa_pub_encrypt *rsa_encrypt, hi_cipher_rsa_crypt *rsa_data);

/*
 * brief RSA decryption a ciphertext with a RSA private key.
 *
 * attention \n
 * N/A
 *
 * param[in]  rsa_decrypt:    decryption struct.
 * param[in]  rsa_data:       rsa data struct.
 * retval ::HI_SUCCESS        Call this API successful.
 * retval ::HI_FAILURE        Call this API fails.
 *
 * see \n
 * N/A
 */
hi_s32 hi_mpi_cipher_rsa_private_decrypt(const hi_cipher_rsa_private_encrypt *rsa_decrypt,
    hi_cipher_rsa_crypt *rsa_data);

/*
 * brief RSA signature a context with appendix, where a signer's RSA private key is used.
 *
 * attention \n
 * N/A
 * param[in]  rsa_sign:       signature struct.
 * param[in]  sign_data:      sign data struct.
 * retval ::HI_SUCCESS        Call this API successful.
 * retval ::HI_FAILURE        Call this API fails.
 * see \n
 * N/A
 */
hi_s32 hi_mpi_cipher_rsa_sign(const hi_cipher_rsa_sign *rsa_sign, hi_cipher_sign_data *sign_data);

/*
 * brief RSA signature verification a context with appendix, where a signer's RSA public key is used.
 *
 * attention \n
 * N/A
 *
 * param[in] rsa_verify:      signature verification struct.
 * param[in] verify_data:     verify data struct;
 * retval ::HI_SUCCESS        Call this API successful.
 * retval ::HI_FAILURE        Call this API fails.
 *
 * see \n
 * N/A
 */
hi_s32 hi_mpi_cipher_rsa_verify(const hi_cipher_rsa_verify *rsa_verify, hi_cipher_verify_data *verify_data);

/*
 * brief RSA encryption a plaintext with a RSA private key.
 *
 * attention \n
 * N/A
 *
 * param[in]  rsa_encrypt:    encryption struct.
 * param[in]  rsa_data:       rsa data struct.
 * retval ::HI_SUCCESS        Call this API successful.
 * retval ::HI_FAILURE        Call this API fails.
 *
 * see \n
 * N/A
 */
hi_s32 hi_mpi_cipher_rsa_private_encrypt(const hi_cipher_rsa_private_encrypt *rsa_encrypt,
    hi_cipher_rsa_crypt *rsa_data);

/*
 * brief RSA decryption a ciphertext with a RSA public key.
 *
 * attention \n
 * N/A
 *
 * param[in]  rsa_decrypt:    decryption struct.
 * param[in]  rsa_data:       rsa data struct.
 * retval ::HI_SUCCESS        Call this API successful.
 * retval ::HI_FAILURE        Call this API fails.
 *
 * see \n
 * N/A
 */
hi_s32 hi_mpi_cipher_rsa_pub_decrypt(const hi_cipher_rsa_pub_encrypt *rsa_decrypt, hi_cipher_rsa_crypt *rsa_data);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __hi_mpi_cipher_h__ */
