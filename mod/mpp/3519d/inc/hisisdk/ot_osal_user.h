/*
  Copyright (c), 2001-2024, Shenshu Tech. Co., Ltd.
 */

#ifndef __OT_OSAL_USER_H__
#define __OT_OSAL_USER_H__

#include "autoconf.h"
#include "ot_osal.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#ifdef CONFIG_USER_SPACE // user space
#define OSAL_OPEN     osal_opendev
#define OSAL_READ     osal_readdev
#define OSAL_WRITE    osal_writedev
#define OSAL_IOCTL    osal_ioctldev
#define OSAL_CLOSE    osal_closedev
#define OSAL_MMAP     osal_mmapdev

#else // kernel space
#define OSAL_OPEN     open
#define OSAL_READ     read
#define OSAL_WRITE    write
#define OSAL_IOCTL    ioctl
#define OSAL_CLOSE    close
#define OSAL_MMAP     mmap
#endif

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif // __OT_OSAL_USER_H__