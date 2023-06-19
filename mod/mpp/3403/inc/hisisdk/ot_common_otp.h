/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */

#ifndef OT_COMMON_OTP_H
#define OT_COMMON_OTP_H

#include "ot_type.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* OTP error code value */
#define OT_ERR_OTP_NOT_INIT                                 0x804E0001
#define OT_ERR_OTP_NULL_PTR                                 0x804E0002
#define OT_ERR_OTP_BUSY                                     0x804E0003
#define OT_ERR_OTP_FAILED_INIT                              0x804E0004
#define OT_ERR_OTP_FAILED_MEM                               0x804E0005
#define OT_ERR_OTP_FAILED_SEC_FUNC                          0x804E0006
#define OT_ERR_OTP_INVALID_PARAM                            0x804E0007
#define OT_ERR_OTP_INVALID_FIELD_NAME                       0x804E0008
#define OT_ERR_OTP_ZONE_ALREADY_SET                         0x804E0009
#define OT_ERR_OTP_ZONE_LOCKED                              0x804E000A
#define OT_ERR_OTP_ZONE_NO_PERMIT                           0x804E000B
#define OT_ERR_OTP_WAIT_TIMEOUT                             0x804E000C
#define OT_ERR_OTP_FUNC_UNSUPPORT                           0x804E000D

#define OT_OTP_PV_NAME_MAX_LEN                              32
#define OT_OTP_PV_VALUE_MAX_LEN                             32

/* persistent value struct */
typedef struct {
    td_bool burn;                                   /* Burn OTP fuse or read OTP fuse. */
    td_char field_name[OT_OTP_PV_NAME_MAX_LEN];     /* OTP fuse name. */
    td_u32  value_len;                              /* OTP fuse value bit length. */
    td_u8   value[OT_OTP_PV_VALUE_MAX_LEN];         /* OTP fuse value. */
    td_bool lock;                                   /* Burn OTP fuse lock or not,
                                                      If this item not support lock. ignore it */
} ot_otp_burn_pv_item;

typedef enum {
    OT_OTP_STA_ALL_UNLOCKED = 0,                    /* user data area is all unlock. */
    OT_OTP_STA_PARTIAL_LOCKED,                      /* user data area is partial unlock. */
    OT_OTP_STA_ALL_LOCKED,                          /* user data area is all lock. */
    OT_OTP_STA_BUTT,                                /* invalid param. */
} ot_otp_lock_status;

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* OT_COMMON_OTP_H */
