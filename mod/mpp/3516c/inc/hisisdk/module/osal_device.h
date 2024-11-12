/*
  Copyright (c), 2001-2024, Shenshu Tech. Co., Ltd.
 */

/**
 * @defgroup osal_device osal_device
 */
#ifndef __OSAL_DEVICE_H__
#define __OSAL_DEVICE_H__

#include "module/osal_wait.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define OSAL_POLLIN 0x0001U
#define OSAL_POLLPRI 0x0002U
#define OSAL_POLLOUT 0x0004U
#define OSAL_POLLERR 0x0008U
#define OSAL_POLLHUP 0x0010U
#define OSAL_POLLNVAL 0x0020U
#define OSAL_POLLRDNORM 0x0040U
#define OSAL_POLLRDBAND 0x0080U
#define OSAL_POLLWRNORM 0x0100U

typedef struct osal_poll_ {
    void *poll_table;
    void *data;
} osal_poll;

typedef struct osal_ioctl_cmd_ {
    unsigned int cmd;
    int (*handler)(unsigned int cmd, void *arg, void *private_data);
} osal_ioctl_cmd;

typedef struct osal_vm_ {
    void *vm;
} osal_vm;

typedef struct osal_fileops_ {
    int (*open)(void *private_data);
    int (*read)(char *buf, int size, long *offset, void *private_data);
    int (*write)(const char *buf, int size, long *offset, void *private_data);
    long (*llseek)(long offset, int whence, void *private_data);
    int (*release)(void *private_data);
    unsigned int (*poll)(osal_poll *osal_poll, void *private_data);
    int (*mmap)(osal_vm *vm, unsigned long start, unsigned long end, unsigned long vm_pgoff, void *private_data);
    int (*fasync)(int fd, void *filp, int mode);
    osal_ioctl_cmd *cmd_list;
    unsigned int cmd_cnt;
} osal_fileops;

typedef struct osal_pmops_ {
    int (*pm_suspend)(void *private_data);
    int (*pm_resume_early)(void *private_data);
    int (*pm_resume)(void *private_data);
    int (*pm_lowpower_enter)(void *private_data);
    int (*pm_lowpower_exit)(void *private_data);
    int (*pm_poweroff)(void *private_data);
    void *private_data;
} osal_pmops;

#define OSAL_DEV_NAME_LEN 32
typedef struct osal_dev_ {
    char name[OSAL_DEV_NAME_LEN];
    int minor;
    unsigned int parent_minor;
    osal_fileops *fops;
    osal_pmops *pmops;
    void *dev;
    void *owner;
} osal_dev;

#define OSAL_NOCACHE 0
#define OSAL_CACHE 1

typedef enum osal_kobject_action_ {
    OSAL_KOBJ_ADD,
    OSAL_KOBJ_REMOVE,
    OSAL_KOBJ_CHANGE,
    OSAL_KOBJ_MOVE,
    OSAL_KOBJ_ONLINE,
    OSAL_KOBJ_OFFLINE,
    OSAL_KOBJ_BIND,
    OSAL_KOBJ_UNBIND,
    OSAL_KOBJ_MAX
} osal_kobject_action;

/**
 * @ingroup osal_device
 * @brief Invoke the low-power callback function of all devices.
 *
 * @par Description:
 * Invoke the low-power callback function of all devices,
 *
 * @par Support System:
 * linux.
 */
void osal_pm_lowpower_enter(void);

/**
 * @ingroup osal_device
 * @brief Invoke the low-power-exit callback function of all devices.
 *
 * @par Description:
 * Invoke the low-power-exit callback function of all devices.
 *
 * @par Support System:
 * linux.
 */
void osal_pm_lowpower_exit(void);

/**
 * @ingroup osal_device
 * @brief Returns a pointer of the osal_dev type for which memory is applied.
 *
 * @attention Must be freed with osal_dev_destroy.
 *
 * @param name [in] The name of device.
 *
 * @return Pointer to osal_dev.
 *
 * @par Support System:
 * linux liteos freertos.
 */
osal_dev *osal_dev_create(const char *name);

/**
 * @ingroup osal_device
 * @brief Free the dev's memory.
 *
 * @par Description:
 * Free the dev's memory that created by osal_dev_create.
 * @param dev [in] The result of osal_dev_create.
 * @attention this api may free @dev,it should be from osal_dev_create.caller should set NULL to @dev after call api
 *
 * @return OSAL_SUCCESS/OSAL_FAILURE
 *
 * @par Support System:
 * linux liteos freertos.
 */
int osal_dev_destroy(osal_dev *dev);

/**
 * @ingroup osal_device
 * @brief register device.
 *
 * @attention The input pool parameter must be created by func osal_dev_create.
 *
 * @return OSAL_SUCCESS/OSAL_FAILURE
 *
 * @par Support System:
 * linux liteos freertos.
 */
int osal_dev_register(osal_dev *dev);

/**
 * @ingroup osal_device
 * @brief unregister device.
 *
 * @attention The input pool parameter must be created by func osal_dev_create.
 *
 * @par Support System:
 * linux liteos freertos.
 */
void osal_dev_unregister(osal_dev *dev);

/**
 * @ingroup osal_device
 * @brief Set the device to wake up asynchronously.
 *
 * @par Description:
 * Set the device to wake up asynchronously.
 *
 * @param minor [in] The minor of device.
 *
 * @par Support System:
 * linux.
 */
void osal_device_set_async(unsigned int minor);

/**
 * @ingroup osal_device
 * @brief Adds the current process to the wait list specified by the wait parameter.
 *
 * @par Support System:
 * linux liteos.
 */
void osal_poll_wait(osal_poll *table, osal_wait *wait);

/**
 * @ingroup osal_device
 * @brief Wakes up the current process from the wait list specified by the wait parameter.
 *
 * @par Support System:
 * liteos.
 */
void osal_notify_poll(osal_wait *wait);

/**
 * @ingroup osal_device
 * @brief remap kernel memory to userspace.
 *
 * @return The value 0 indicates success, and other values indicate failure.
 *
 * @par Support System:
 * linux liteos.
 */
int osal_remap_pfn_range(osal_vm *vm, unsigned long addr, unsigned long pfn, unsigned long size);

/**
 * @ingroup osal_device
 * @brief try to freeze the current task.
 *
 * @return True is returned for success and false is returned for failure.
 *
 * @par Support System:
 * linux liteos.
 */
int osal_try_to_freeze(void);

/**
 * @ingroup osal_device
 * @brief make current task freezable and try to freeze the current task.
 *
 * @return True is returned for success and false is returned for failure.
 *
 * @par Support System:
 * linux liteos.
 */
int osal_set_freezable(void);

/**
 * @ingroup osal_device
 * @brief send an uevent with environmental data.
 *
 * @return Returns 0 if kobject_uevent_env() is completed with success or the corresponding error when it fails.
 *
 * @par Support System:
 * linux liteos.
 */
int osal_kobject_uevent_env(osal_dev *dev, osal_kobject_action action, char *envp[]);

/**
 * @ingroup osal_device
 * @brief Setting up the character device driver's fasync queue.
 *
 * @par Description:
 * Setting up the character device driver's fasync queue.
 *
 * @return It returns negative on error, 0 if it did no changes and positive if it added/deleted the entry.
 *
 * @par Support System:
 * linux.
 */
int osal_fasync_helper(int fd, void *filp, int mode, void **fapp);

/**
 * @ingroup osal_device
 *
 * @brief Send asynchronous notifications.
 *
 * @par Description:
 * The SIGIO signal is sent to the application layer,
 * and the application layer triggers the function corresponding to the SIGIO signal.
 *
 * @par Support System:
 * linux.
 */
void osal_fasync_notify(void **fapp, int sig, int band);

/**
 * @ingroup osal_device
 * @brief get nocached prot_page.
 *
 * @par Description:
 * get nocached prot_page.
 *
 * @par Support System:
 * linux.
 */
void osal_pgprot_noncached(osal_vm *vm);

/**
 * @ingroup osal_device
 * @brief get cached prot_page.
 *
 * @par Description:
 * get cached prot_page.
 *
 * @par Support System:
 * linux.
 */
void osal_pgprot_cached(osal_vm *vm);

/**
 * @ingroup osal_device
 * @brief get writecombine prot_page.
 *
 * @par Description:
 * get writecombine prot_page.
 *
 * @par Support System:
 * linux.
 */
void osal_pgprot_writecombine(osal_vm *vm);

typedef struct {
    unsigned long a0;
    unsigned long a1;
    unsigned long a2;
    unsigned long a3;
    unsigned long a4;
    unsigned long a5;
    unsigned long a6;
    unsigned long a7;
} osal_smccc_info;

typedef struct {
    unsigned long a0;
    unsigned long a1;
    unsigned long a2;
    unsigned long a3;
} osal_smccc_res;

/**
 * @ingroup osal_device
 * @brief make SMC calls.
 *
 * @par Description:
 * This function is used to make SMC calls following SMC Calling Convention.
 *
 * @par Support System:
 * linux.
 */
void osal_smccc_smc(const osal_smccc_info *info, osal_smccc_res *res);

/**
 * @ingroup osal_device
 * @brief open_device.
 *
 * @par Description:
 * Invoke the callback function open set by the caller.
 *
 * @return return the device file's fd.
 *
 * @par Support System:
 * linux(userspace).
 */
int osal_opendev(const char *path, int flag, ...);

/**
 * @ingroup osal_device
 * @brief close device.
 *
 * @par Description:
 * Call the callback function release given by caller.
 *
 * @return OSAL_SUCCESS/OSAL_FAILURE
 *
 * @par Support System:
 * linux(userspace).
 */
int osal_closedev(int fd);

/**
 * @ingroup osal_device
 * @brief read device.
 *
 * @par Description:
 * Call the callback function read given by caller.
 *
 * @return return the callback functon's return value, or -1 when fd is incorrect.
 *
 * @par Support System:
 * linux(userspace).
 */
int osal_readdev(int fd, void *buf, unsigned long count);

/**
 * @ingroup osal_device
 * @brief write device.
 *
 * @par Description:
 * Call the callback function write given by caller.
 *
 * @return return the callback functon's return value, or -1 when fd is incorrect.
 *
 * @par Support System:
 * linux(userspace).
 */
int osal_writedev(int fd, const void *buf, unsigned long count);

/**
 * @ingroup osal_device
 * @brief Perform other operations on the device.
 *
 * @par Description:
 * Call the callback function osal_ioctl_cmd->handler given by caller.
 *
 * @return return the callback functon's return value, or -1 when fd is incorrect.
 *
 * @par Support System:
 * linux(userspace).
 */
int osal_ioctldev(int fd, unsigned int cmd, ...);

// //////////////////////////// userspace interface //////////////////////////////
/**
 * @ingroup osal_device
 * @brief osal linux userspace init.
 *
 * @par Description:
 * osal linux userspace init.
 *
 * @return OSAL_SUCCESS/OSAL_FAILURE
 *
 * @par Support System:
 * linux(userspace).
 */
int osal_init(void);

/**
 * @ingroup osal_device
 * @brief osal linux userspace exit.
 *
 * @par Description:
 * osal linux userspace exit.
 *
 * @par Support System:
 * linux(userspace).
 */
void osal_exit(void);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
#endif /* __OSAL_DEVICE_H__ */
