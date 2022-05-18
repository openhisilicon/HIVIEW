/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2019-2020. All rights reserved.
 * Description: export function header file.
 * Author: Hisilicon multimedia software group
 * Create: 2019/06/28
 */

#include "hi_common.h"

#ifndef __HI_VO_EXPORT_H__
#define __HI_VO_EXPORT_H__

typedef struct {
    void (*vo_notify)(int mod_id, int dev);
}hi_vo_export_callback;

typedef hi_s32 hi_vo_register_export_callback(hi_vo_export_callback *export_callback);

typedef struct {
    hi_vo_register_export_callback *vo_register_export_callback;
}hi_vo_export_symbol;

hi_vo_export_symbol *hi_vo_get_export_symbol(hi_void);
hi_void hi_vo_enable_dev_export(hi_vo_dev dev);
hi_void hi_vo_disable_dev_export(hi_vo_dev dev);

#endif /* __HI_VO_EXPORT_H__ */
