/*
  Copyright (c), 2001-2024, Shenshu Tech. Co., Ltd.
 */

/**
 * @defgroup osal_proc osal_proc
 */
#ifndef __OSAL_PROC_H__
#define __OSAL_PROC_H__

#include "osal_list.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define OSAL_PROC_NAME_LENGTH 32
#define PROC_CMD_SINGEL_LENGTH_MAX 32
#define PROC_CMD_ALL_LENGTH_MAX 128
#define PROC_CMD_NUM_MAX 16

typedef struct osal_proc_cmd_ {
    char name[OSAL_PROC_NAME_LENGTH];
    int (*handler)(unsigned int argc, char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], void *private_data);
} osal_proc_cmd;

typedef struct osal_proc_dir_entry {
    char name[OSAL_PROC_NAME_LENGTH];
    unsigned int cmd_cnt;
    osal_proc_cmd *cmd_list;
    void *proc_dir_entry;
    int (*open)(struct osal_proc_dir_entry *entry);
    int (*read)(struct osal_proc_dir_entry *entry);
    int (*write)(struct osal_proc_dir_entry *entry, const char *buf, int count, long long *);
    void *private_data;
    void *seqfile;
    struct osal_list_head node;
} osal_proc_entry;

/**
 * @ingroup osal_proc
 * @brief Create a directory in /proc.
 *
 * @par Description:
 * Create a directory in /proc.
 *
 * @attention
 * This interface should be invoked only once.
 * That is, in the /proc directory, you can only have one directory created by yourself at the same time.
 *
 * @param name [in] Name of the directory to be created.
 *
 * @par Support System:
 * linux liteos.
 */
void osal_proc_init(const char *name);

/**
 * @ingroup osal_proc
 * @brief Delete the directory created by osal_proc_init() and all the files in the directory.
 *
 * @par Description:
 * Delete the directory created by osal_proc_init() and all the files in the directory.
 *
 * @param name [in] Name of the directory to be deleted, created by osal_proc_init().
 * @par Support System:
 * linux liteos.
 */
void osal_proc_exit(const char *name);

/**
 * @ingroup osal_proc
 * @brief Delete the file created by the function osal_create_proc_entry.
 *
 * @par Description:
 * Delete the file created by the function osal_create_proc_entry.
 *
 * @param name   [in] The file name, same as the first parameter of osal_create_proc_entry.
 * @param parent [in] osal_proc_entry*, returned by the osal_create_proc_entry.
 *
 * @par Support System:
 * linux liteos.
 */
void osal_remove_proc_entry(const char *name, osal_proc_entry *parent);

/**
 * @ingroup osal_proc
 * @brief Create a file in the directory created by the function osal_proc_init.
 *
 * @par Description:
 * Create a file in the directory created by the function osal_proc_init.
 *
 * @param name   [in] Name of the directory to be created.
 * @param parent [in] Reserved parameter, not used for now.
 *
 * @return osal_proc_entry*, parent directory of the file to be created.
 * Generally, it stands for "/proc/xxx", where xxx is the first parameter of osal_proc_init.
 *
 * @par Support System:
 * linux liteos.
 */
osal_proc_entry *osal_create_proc_entry(const char *name, osal_proc_entry *parent);

#if defined(__LITEOS__)
#define osal_seq_printf(seqfile, fmt, ...) seq_printf(seqfile, fmt, ##__VA_ARGS__)
#else
void osal_seq_printf(void *seqfile, const char *fmt, ...) __attribute__((__format__ (__printf__, 2, 3)));
#endif

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
#endif /* __OSAL_PROC_H__ */
