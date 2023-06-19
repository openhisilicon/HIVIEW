/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */

#ifndef __HI_MPI_OTP_H__
#define __HI_MPI_OTP_H__

#include "hi_common_otp.h"

#ifdef __cplusplus
extern "C" {
#endif

hi_s32 hi_mpi_otp_init(hi_void);

hi_s32 hi_mpi_otp_deinit(hi_void);

hi_s32 hi_mpi_otp_set_user_data(const hi_char *field_name,
    hi_u32 offset, const hi_u8 *value, hi_u32 value_len);

hi_s32 hi_mpi_otp_get_user_data(const hi_char *field_name,
    hi_u32 offset, hi_u8 *value, hi_u32 value_len);

hi_s32 hi_mpi_otp_burn_product_pv(const hi_otp_burn_pv_item *pv, hi_u32 num);

hi_s32 hi_mpi_otp_read_product_pv(hi_otp_burn_pv_item *pv, hi_u32 num);

hi_s32 hi_mpi_otp_get_key_verify_status(const hi_char *key_name, hi_bool *status);

hi_s32 hi_mpi_otp_set_user_data_lock(const hi_char *field_name,
    hi_u32 offset, hi_u32 value_len);

hi_s32 hi_mpi_otp_get_user_data_lock(const hi_char *field_name,
    hi_u32 offset, hi_u32 value_len, hi_otp_lock_status *lock);

#ifdef __cplusplus
}
#endif

#endif /* __HI_MPI_OTP_H__ */
