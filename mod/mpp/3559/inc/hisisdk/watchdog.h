/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2006-2019. All rights reserved.
 * Description: Head file of watchdog.
 * Author: Hisilicon multimedia software group
 * Create: 2006-12-16
 */

#ifndef _LINUX_WATCHDOG_H
#define _LINUX_WATCHDOG_H

#include <linux/types.h>

#ifdef __cplusplus
extern "C"{
#endif /* __cplusplus */

int watchdog_init(void);
void watchdog_exit(void);

#define    WATCHDOG_IOCTL_BASE    'W'

struct watchdog_info {
    unsigned int options;             /* Options the card/driver supports */
    unsigned int firmware_version;    /* Firmware version of the card */
    unsigned char identity[32];       /* 32 bit Identity of the board */
};

#define WDIOC_GETSUPPORT     _IOR(WATCHDOG_IOCTL_BASE, 0, struct watchdog_info)
#define WDIOC_GETSTATUS      _IOR(WATCHDOG_IOCTL_BASE, 1, int)
#define WDIOC_GETBOOTSTATUS  _IOR(WATCHDOG_IOCTL_BASE, 2, int)

#define WDIOC_SETOPTIONS     _IOWR(WATCHDOG_IOCTL_BASE, 4, int)
#define WDIOC_KEEPALIVE      _IO(WATCHDOG_IOCTL_BASE, 5)
#define WDIOC_SETTIMEOUT     _IOWR(WATCHDOG_IOCTL_BASE, 6, int)
#define WDIOC_GETTIMEOUT     _IOR(WATCHDOG_IOCTL_BASE, 7, int)

#define WDIOF_UNKNOWN        (-1)    /* Unknown flag error */
#define WDIOS_UNKNOWN        (-1)    /* Unknown status error */

#define WDIOF_OVERHEAT       0x0001    /* Reset due to CPU overheat */
#define WDIOF_FANFAULT       0x0002    /* Fan failed */
#define WDIOF_EXTERN1        0x0004    /* External relay 1 */
#define WDIOF_EXTERN2        0x0008    /* External relay 2 */
#define WDIOF_POWERUNDER     0x0010    /* Power bad/power fault */
#define WDIOF_CARDRESET      0x0020    /* Card previously reset the CPU */
#define WDIOF_POWEROVER      0x0040    /* Power over voltage */
#define WDIOF_SETTIMEOUT     0x0080    /* Set timeout (in seconds) */
#define WDIOF_MAGICCLOSE     0x0100    /* Supports magic close char */
#define WDIOF_PRETIMEOUT     0x0200    /* Pretimeout (in seconds), get/set */
#define WDIOF_KEEPALIVEPING  0x8000    /* Keep alive ping reply */

#define WDIOS_DISABLECARD    0x0001    /* Turn off the watchdog timer */
#define WDIOS_ENABLECARD     0x0002    /* Turn on the watchdog timer */
#define WDIOS_TEMPPANIC      0x0004    /* Kernel panic on temperature trip */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
