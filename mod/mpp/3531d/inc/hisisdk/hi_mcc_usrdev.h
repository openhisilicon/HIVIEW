/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2011-2019. All rights reserved.
 * Description: mcc functions declaration
 * Author: Hisilicon multimedia software group
 * Create: 2011/01/01
 */

#include <linux/ioctl.h>

#ifndef __HI_MCC_USERDEV_H_
#define __HI_MCC_USERDEV_H__

#define MCC_USR_MODULE_NAME     "MCC_USR"
#define MCC_USR_VERSION         __DATE__", "__TIME__

#define HISI_MAX_MAP_DEV        0x1f

#define NORMAL_MESSAGE_SIZE     128

#define MAXIMAL_MESSAGE_SIZE    0x1000

struct hi_mcc_handle_attr {
    int target_id;
    int port;
    int priority;

    /* to store remote device ids */
    int remote_id[HISI_MAX_MAP_DEV];
};

#define HI_IOC_MCC_BASE             'M'

/* Create a new mcc handle. A file descriptor is only used once for one mcc handle. */
#define HI_MCC_IOC_CONNECT          _IOW(HI_IOC_MCC_BASE, 1, struct hi_mcc_handle_attr)

#define HI_MCC_IOC_CHECK            _IOW(HI_IOC_MCC_BASE, 2, struct hi_mcc_handle_attr)

#define HI_MCC_IOC_GET_LOCAL_ID     _IOW(HI_IOC_MCC_BASE, 4, struct hi_mcc_handle_attr)

#define HI_MCC_IOC_GET_REMOTE_ID    _IOW(HI_IOC_MCC_BASE, 5, struct hi_mcc_handle_attr)

#define HI_MCC_IOC_ATTR_INIT        _IOW(HI_IOC_MCC_BASE, 6, struct hi_mcc_handle_attr)

#ifdef __KERNEL__
#include <linux/module.h>
#include <linux/kernel.h>
#include <asm/ioctl.h>
#include <linux/wait.h>

#define HI_MCC_DEBUG        4
#define HI_MCC_INFO         3
#define HI_MCC_ERR          2
#define HI_MCC_FATAL        1
#define HI_MCC_DBG_LEVEL    2
#define hi_mcc_trace(level, s, params...) do{ if(level <= HI_MCC_DBG_LEVEL)\
    printk(KERN_INFO "[%s, %d]: " s "\n", __FUNCTION__, __LINE__, ##params);\
} while (0)

struct hios_mcc_handle {
    hi_ulong pci_handle;   /* pci handle */
    struct list_head mem_list;  /* mem list  */
    wait_queue_head_t wait;
};

typedef struct hios_mcc_handle  hios_mcc_handle_t;

struct hios_mcc_handle_opt {
    int (*recvfrom_notify)(hi_void *handle, hi_void *buf, hi_u32 data_len);
    hi_ulong data;
};

typedef struct hios_mcc_handle_opt hios_mcc_handle_opt_t;

hios_mcc_handle_t *hios_mcc_open(struct hi_mcc_handle_attr *attr);

void hios_mcc_handle_attr_init(struct hi_mcc_handle_attr *attr);

int hios_mcc_sendto(hios_mcc_handle_t *handle, const hi_void *buf, hi_u32 len);

int hios_mcc_sendto_user(hios_mcc_handle_t *handle, const hi_void *buf, hi_u32 len, hi_s32 flag);

int hios_mcc_close(hios_mcc_handle_t *handle);

int hios_mcc_getopt(hios_mcc_handle_t *handle, hios_mcc_handle_opt_t *opt);

int hios_mcc_setopt(hios_mcc_handle_t *handle, const hios_mcc_handle_opt_t *opt);

int hios_mcc_getlocalid(hios_mcc_handle_t *handle);

int hios_mcc_getremoteids(hi_s32 ids[], hios_mcc_handle_t *handle);

int hios_mcc_check_remote(hi_s32 remote_id, hios_mcc_handle_t *handle);

int hios_mcc_register(hi_void);

#endif  /* __KERNEL__ */

#endif  /* __HI_MCC_USERDEV_H__ */
