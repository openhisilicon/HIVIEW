/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */

#include "ot_common.h"

#ifndef OT_VO_EXPORT_H
#define OT_VO_EXPORT_H

typedef struct {
    void (*vo_notify)(int mod_id, int dev);
    void (*vo_user_notify)(int mod_id, int dev);
} ot_vo_export_callback;

typedef td_s32 ot_vo_register_export_callback(ot_vo_export_callback *export_callback);

typedef struct {
    ot_vo_register_export_callback *vo_register_export_callback;
} ot_vo_export_symbol;

ot_vo_export_symbol *ot_vo_get_export_symbol(td_void);
td_void ot_vo_enable_dev_export(ot_vo_dev dev);
td_void ot_vo_disable_dev_export(ot_vo_dev dev);

#endif /* OT_VO_EXPORT_H */
