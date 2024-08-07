/*
  Copyright (c), 2001-2024, Shenshu Tech. Co., Ltd.
 */

#ifndef MOD_EXT_H
#define MOD_EXT_H

#include "ot_osal.h"

#include "ot_type.h"
#include "ot_common.h"

#define MAX_MPP_MODULES OT_ID_BUTT

#define VERSION_MAGIC 20210823

#define MAX_MOD_NAME    16

typedef enum {
    MOD_NOTICE_STOP = 0x11,
} mod_notice_id;

typedef enum {
    MOD_STATE_FREE = 0x11,
    MOD_STATE_BUSY = 0X22,
} mod_state;

typedef td_s32 fn_mod_init(td_void *);
typedef td_void fn_mod_exit(td_void);
typedef td_void fn_mod_notify(mod_notice_id notice_id);
typedef td_void fn_mod_query_state(mod_state *state);
typedef td_u32 fn_mod_ver_checker(td_void);

typedef struct {
    struct osal_list_head list;

    td_char mod_name[MAX_MOD_NAME];
    ot_mod_id mod_id;

    fn_mod_init *pfn_init;
    fn_mod_exit *pfn_exit;
    fn_mod_query_state *pfn_query_state;
    fn_mod_notify *pfn_notify;
    fn_mod_ver_checker *pfn_ver_checker;

    td_bool inited;

    td_void *export_funcs;
    td_void *data;

    td_char *version;
} umap_module;

td_char *cmpi_get_module_name(ot_mod_id mod_id);
td_void *cmpi_get_module_func_by_id(ot_mod_id mod_id);

td_void cmpi_stop_modules(td_void);
td_s32 cmpi_query_modules(td_void);
td_s32 cmpi_init_modules(td_void);
td_void cmpi_exit_modules(td_void);

td_void cmpi_stop_modules_for_audio(td_void);
td_s32 cmpi_query_modules_for_audio(td_void);
td_s32 cmpi_init_modules_for_audio(td_void);
td_void cmpi_exit_modules_for_audio(td_void);

td_s32 cmpi_register_module(umap_module *module);
td_void cmpi_unregister_module(ot_mod_id mod_id);

#define func_entry(type, id) ((type *)cmpi_get_module_func_by_id(id))
#define check_func_entry(id) (cmpi_get_module_func_by_id(id) != NULL)
#define func_entry_null(id) (!check_func_entry(id))

#endif /*  MOD_EXT_H */
