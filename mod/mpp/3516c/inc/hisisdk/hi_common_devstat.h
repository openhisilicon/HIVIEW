/*
  Copyright (c), 2001-2024, Shenshu Tech. Co., Ltd.
 */

#ifndef _HI_COMMON_DEVSTAT_H
#define _HI_COMMON_DEVSTAT_H

#include "ot_common_devstat.h"

#ifdef __cplusplus
extern "C" {
#endif

#define HI_ERR_DEVSTAT_NULL_PTR OT_ERR_DEVSTAT_NULL_PTR
#define HI_ERR_DEVSTAT_NOT_READY OT_ERR_DEVSTAT_NOT_READY
#define HI_ERR_DEVSTAT_EXIST OT_ERR_DEVSTAT_EXIST
#define HI_ERR_DEVSTAT_UNEXIST OT_ERR_DEVSTAT_UNEXIST
#define HI_ERR_DEVSTAT_NOT_SUPPORT OT_ERR_DEVSTAT_NOT_SUPPORT
#define HI_ERR_DEVSTAT_NOT_PERM OT_ERR_DEVSTAT_NOT_PERM
#define HI_ERR_DEVSTAT_NOMEM OT_ERR_DEVSTAT_NOMEM
#define HI_ERR_DEVSTAT_NOBUF OT_ERR_DEVSTAT_NOBUF
#define HI_ERR_DEVSTAT_ILLEGAL_PARAM OT_ERR_DEVSTAT_ILLEGAL_PARAM
#define HI_ERR_DEVSTAT_BUSY OT_ERR_DEVSTAT_BUSY
#define HI_ERR_DEVSTAT_BUF_EMPTY OT_ERR_DEVSTAT_BUF_EMPTY
typedef ot_devstat_ddr_status hi_devstat_ddr_status;
typedef ot_devstat_fmc_status hi_devstat_fmc_status;
#define HI_DEVSTAT_DEVICE_TYPE_DDR OT_DEVSTAT_DEVICE_TYPE_DDR
#define HI_DEVSTAT_DEVICE_TYPE_FMC OT_DEVSTAT_DEVICE_TYPE_FMC
#define HI_DEVSTAT_DEVICE_TYPE_BUTT OT_DEVSTAT_DEVICE_TYPE_BUTT
typedef ot_devstat_device_type hi_devstat_device_type;
typedef ot_devstat_device_status hi_devstat_device_status;

#ifdef __cplusplus
}
#endif

#endif /* _HI_COMMON_DEVSTAT_H */