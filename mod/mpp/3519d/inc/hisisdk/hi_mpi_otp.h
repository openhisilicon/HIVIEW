/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */

#ifndef HI_MPI_OTP_H
#define HI_MPI_OTP_H

#include "hi_type.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

hi_s32 hi_mpi_otp_init(hi_void);

hi_s32 hi_mpi_otp_deinit(hi_void);

hi_s32 hi_mpi_otp_read_word(hi_u32 offset, hi_u32 *data);

hi_s32 hi_mpi_otp_read_byte(hi_u32 offset, hi_u8 *data);

hi_s32 hi_mpi_otp_write_byte(hi_u32 offset, hi_u8 data);

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif