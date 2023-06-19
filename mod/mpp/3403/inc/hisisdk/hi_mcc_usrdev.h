/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */

#ifndef __HI_MCC_USRDEV_H_
#define __HI_MCC_USRDEV_H__

#include <linux/ioctl.h>
#include "ot_mcc_usrdev.h"

#define hi_mcc_handle_attr ot_mcc_handle_attr

#define HI_IOC_MCC_BASE OT_IOC_MCC_BASE

#define HI_MCC_IOC_CONNECT       OT_MCC_IOC_CONNECT
#define HI_MCC_IOC_CHECK         OT_MCC_IOC_CHECK
#define HI_MCC_IOC_GET_LOCAL_ID  OT_MCC_IOC_GET_LOCAL_ID
#define HI_MCC_IOC_GET_REMOTE_ID OT_MCC_IOC_GET_REMOTE_ID
#define HI_MCC_IOC_ATTR_INIT     OT_MCC_IOC_ATTR_INIT

#endif  /* __HI_MCC_USRDEV_H__ */
