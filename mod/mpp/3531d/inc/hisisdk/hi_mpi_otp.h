/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2020-2020. All rights reserved.
 * Description: Head file for mpi otp.
 * Author     : Hisilicon multimedia software group
 * Create     : 2020-06-01
 */

#ifndef __HI_MPI_OTP_H__
#define __HI_MPI_OTP_H__

#include "hi_type.h"
#include "hi_common_otp.h"

hi_s32 hi_mpi_otp_init(hi_void);

hi_s32 hi_mpi_otp_deinit(hi_void);

hi_s32 hi_mpi_otp_set_user_data(const hi_char *field_name,
    hi_u32 offset, const hi_u8 *value, hi_u32 value_len);

hi_s32 hi_mpi_otp_get_user_data(const hi_char *field_name,
    hi_u32 offset, hi_u8 *value, hi_u32 value_len);

hi_s32 hi_mpi_otp_burn_product_pv(const hi_otp_burn_pv_item *pv, hi_u32 num);

hi_s32 hi_mpi_otp_read_product_pv(hi_otp_burn_pv_item *pv, hi_u32 num);

hi_s32 hi_mpi_otp_get_key_verify_status(const hi_char *key_name, hi_bool *status);

#endif /* __HI_MPI_OTP_H__ */