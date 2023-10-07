/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */

#ifndef HI_MPI_KM_H
#define HI_MPI_KM_H

#include "hi_type.h"
#include "crypto_km_struct.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

hi_s32 hi_mpi_km_init(hi_void);
hi_s32 hi_mpi_km_deinit(hi_void);

/* Keyslot. */
hi_s32 hi_mpi_keyslot_create(crypto_handle *mpi_keyslot_handle, km_keyslot_type keyslot_type);
hi_s32 hi_mpi_keyslot_destroy(crypto_handle mpi_keyslot_handle);

/* Klad. */
hi_s32 hi_mpi_klad_create(crypto_handle *mpi_klad_handle);
hi_s32 hi_mpi_klad_destroy(crypto_handle mpi_klad_handle);

hi_s32 hi_mpi_klad_attach(crypto_handle mpi_klad_handle, km_klad_dest_type klad_type,
    crypto_handle mpi_keyslot_handle);
hi_s32 hi_mpi_klad_detach(crypto_handle mpi_klad_handle, km_klad_dest_type klad_type,
    crypto_handle mpi_keyslot_handle);

hi_s32 hi_mpi_klad_set_attr(crypto_handle mpi_klad_handle, const km_klad_attr *attr);
hi_s32 hi_mpi_klad_get_attr(crypto_handle mpi_klad_handle, km_klad_attr *attr);

hi_s32 hi_mpi_klad_set_session_key(crypto_handle mpi_klad_handle, const km_klad_session_key *key);
hi_s32 hi_mpi_klad_set_content_key(crypto_handle mpi_klad_handle, const km_klad_content_key *key);

hi_s32 hi_mpi_klad_set_clear_key(crypto_handle mpi_klad_handle, const km_klad_clear_key *key);

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif