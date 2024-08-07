/*
  Copyright (c), 2001-2024, Shenshu Tech. Co., Ltd.
 */
#ifndef OT_VDEC_EXPORT_H
#define OT_VDEC_EXPORT_H

#include "ot_common.h"

typedef struct {
    void (*vdec_notify)(int mod_id, int chn, int event_type, void *args);
    int (*vdec_set_scheduler_dec)(int *sched_period);
    int (*vdec_set_scheduler_stm)(int *sched_period);
    int (*vdec_set_scheduler_syn)(int *sched_period);
} ot_vdec_export_callback;

typedef td_s32 ot_vdec_register_export_callback(const ot_vdec_export_callback *export_callback);

typedef struct {
    ot_vdec_register_export_callback *register_export_callback;
} ot_vdec_export_symbol;

ot_vdec_export_symbol *ot_vdec_get_export_symbol(td_void);

#endif /* OT_VDEC_EXPORT_H */

