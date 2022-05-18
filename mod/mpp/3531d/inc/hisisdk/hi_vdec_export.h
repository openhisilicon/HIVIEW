/*
* Copyright (c) Hisilicon Technologies Co., Ltd. 2019. All rights reserved.
* Description:vdec exp function file
* Author: Hisilicon multimedia software group
* Create: 2019/06/15
*/

#include "hi_common.h"

#ifndef __HI_VDEC_EXPORT_H__
#define __HI_VDEC_EXPORT_H__

typedef struct {
    void (*vdec_notify)(int mod_id, int chn, int event_type, void *args);
    int (*vdec_set_scheduler_dec)(void);
    int (*vdec_set_scheduler_stm)(void);
    int (*vdec_set_scheduler_syn)(void);
} hi_vdec_export_callback;

typedef hi_s32 hi_vdec_register_export_callback(hi_vdec_export_callback *export_callback);

typedef struct {
    hi_vdec_register_export_callback *register_export_callback;
} hi_vdec_export_symbol;

hi_vdec_export_symbol *hi_vdec_get_export_symbol(hi_void);

#endif /* __HI_VDEC_EXPORT_H__ */

