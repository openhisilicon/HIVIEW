/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: ao export for user
 * Author: Hisilicon multimedia software group
 * Create: 2019/03/05
 * History               :
 * 1.Date                :   2019/03/05
 *   Modification        :   Created file
 */

#ifndef __HI_AO_EXPORT_H__
#define __HI_AO_EXPORT_H__

#include "hi_common.h"

typedef struct {
    void (*ao_notify) (int ao_dev);
} hi_ao_export_callback;

typedef hi_s32 hi_ao_register_export_callback(hi_ao_export_callback *export_callback);

typedef struct {
    hi_ao_register_export_callback *register_export_callback;
} hi_ao_export_symbol;

hi_ao_export_symbol *hi_ao_get_export_symbol(hi_void);

#endif  /* __HI_AO_EXPORT_H__ */

