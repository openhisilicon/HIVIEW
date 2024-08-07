/*
  Copyright (c), 2001-2024, Shenshu Tech. Co., Ltd.
 */

#ifndef OT_VO_EXPORT_H
#define OT_VO_EXPORT_H

#include "ot_common.h"

typedef struct {
    void (*vo_refresh_disp_notify)(int mod_id, int dev);
    void (*vo_user_notify)(int mod_id, int dev);
} ot_vo_export_callback;

typedef td_s32 ot_vo_register_export_callback(ot_vo_export_callback *export_callback);

typedef struct {
    ot_vo_register_export_callback *vo_register_export_callback;
} ot_vo_export_symbol;

ot_vo_export_symbol *ot_vo_get_export_symbol(td_void);

#endif /* OT_VO_EXPORT_H */
