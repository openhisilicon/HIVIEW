/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */

#include <linux/ioctl.h>

#ifndef OT_MCC_USERDEV_H
#define OT_MCC_USERDEV_H

#define MCC_USR_MODULE_NAME     "MCC_USR"
#define MCC_USR_VERSION         __DATE__", "__TIME__

#define VENDOR_MAX_MAP_DEV    0x1f

#define NORMAL_MESSAGE_SIZE     128

#define MAXIMAL_MESSAGE_SIZE    0x1000

struct ot_mcc_handle_attr {
    int target_id;
    int port;
    int priority;

    /* to store remote device ids */
    int remote_id[VENDOR_MAX_MAP_DEV];
};

#define OT_IOC_MCC_BASE             'M'

/* Create a new mcc handle. A file descriptor is only used once for one mcc handle. */
#define OT_MCC_IOC_CONNECT          _IOW(OT_IOC_MCC_BASE, 1, struct ot_mcc_handle_attr)

#define OT_MCC_IOC_CHECK            _IOW(OT_IOC_MCC_BASE, 2, struct ot_mcc_handle_attr)

#define OT_MCC_IOC_GET_LOCAL_ID     _IOW(OT_IOC_MCC_BASE, 4, struct ot_mcc_handle_attr)

#define OT_MCC_IOC_GET_REMOTE_ID    _IOW(OT_IOC_MCC_BASE, 5, struct ot_mcc_handle_attr)

#define OT_MCC_IOC_ATTR_INIT        _IOW(OT_IOC_MCC_BASE, 6, struct ot_mcc_handle_attr)

#ifdef __KERNEL__
#include <linux/module.h>
#include <linux/kernel.h>
#include <asm/ioctl.h>
#include <linux/wait.h>

#define OT_MCC_DEBUG        4
#define OT_MCC_INFO         3
#define OT_MCC_ERR          2
#define OT_MCC_FATAL        1
#define OT_MCC_DBG_LEVEL    2
#define ot_mcc_trace(level, s, params...) \
do { \
    if (level <= OT_MCC_DBG_LEVEL) { \
        printk(KERN_INFO "[%s, %d]: " s "\n", __FUNCTION__, __LINE__, ##params); \
    } \
} while (0)

struct os_mcc_handle {
    td_ulong pci_handle;   /* pci handle */
    struct list_head mem_list;  /* mem list  */
    wait_queue_head_t wait;
};

typedef struct os_mcc_handle  os_mcc_handle_t;

struct os_mcc_handle_opt {
    int (*recvfrom_notify)(td_void *handle, td_void *buf, td_u32 data_len);
    td_ulong data;
};

typedef struct os_mcc_handle_opt os_mcc_handle_opt_t;

os_mcc_handle_t *os_mcc_open(struct ot_mcc_handle_attr *attr);

void os_mcc_handle_attr_init(struct ot_mcc_handle_attr *attr);

int os_mcc_sendto(os_mcc_handle_t *handle, const td_void *buf, td_u32 len);

int os_mcc_sendto_user(os_mcc_handle_t *handle, const td_void *buf, td_u32 len, td_s32 flag);

int os_mcc_close(os_mcc_handle_t *handle);

int os_mcc_getopt(os_mcc_handle_t *handle, os_mcc_handle_opt_t *opt);

int os_mcc_setopt(os_mcc_handle_t *handle, const os_mcc_handle_opt_t *opt);

int os_mcc_getlocalid(os_mcc_handle_t *handle);

int os_mcc_getremoteids(td_s32 ids[], os_mcc_handle_t *handle);

int os_mcc_check_remote(td_s32 remote_id, os_mcc_handle_t *handle);

int os_mcc_register(td_void);

#endif  /* __KERNEL__ */

#endif  /* OT_MCC_USERDEV_H */
