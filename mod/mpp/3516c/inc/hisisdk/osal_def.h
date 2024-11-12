/*
  Copyright (c), 2001-2024, Shenshu Tech. Co., Ltd.
 */

#ifndef __OSAL_DEF_H__
#define __OSAL_DEF_H__

#ifdef __KERNEL__
#include <linux/module.h>
#endif

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#ifdef __KERNEL__
#define OSAL_THIS_MODULE THIS_MODULE
#define osal_module_export(_symbol) EXPORT_SYMBOL(_symbol)
#define osal_module_param(name, type, perm) module_param(name, type, perm)
#define OSAL_MODULE_PARM_DESC(_parm, desc) MODULE_PARM_DESC(_perm, desc)
#define osal_module_init(x) module_init(x)
#define osal_module_exit(x) module_exit(x)
#define OSAL_MODULE_AUTHOR(_author) MODULE_AUTHOR(_author)
#define OSAL_MODULE_DESCRIPTION(_description) MODULE_DESCRIPTION(_description)
#define OSAL_MODULE_LICENSE(_license) MODULE_LICENSE(_license)
#define OSAL_MODULE_VERSION(_version) MODULE_VERSION(_version)
#else
#define OSAL_THIS_MODULE
#define osal_module_export(_symbol)
#define osal_module_param(name, type, perm)
#define OSAL_MODULE_PARM_DESC(_parm, desc)
#define osal_module_init(x)
#define osal_module_exit(x)
#define OSAL_MODULE_AUTHOR(_author)
#define OSAL_MODULE_DESCRIPTION(_description)
#define OSAL_MODULE_LICENSE(_license)
#define OSAL_MODULE_VERSION(_version)
#endif

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif // __OSAL_DEF_H__