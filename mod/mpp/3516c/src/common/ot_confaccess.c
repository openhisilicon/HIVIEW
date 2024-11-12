/*
  Copyright (c), 2001-2024, Shenshu Tech. Co., Ltd.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libgen.h>
#include <fcntl.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "hi_type.h"
#include "securec.h"
#include "ini_parser.h"
#include "list.h"
#include "ot_confaccess.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* redefine module name */
#ifdef HI_MODULE
#undef HI_MODULE
#endif
#define HI_MODULE "CONFACCESS"

#define CONFACCESS_DEFAULT_MODULE_CFGPATH "./" /* <default module cfg path */
#define CONFACCESS_PATH_SEPARATOR '/'          /* <path separator */

/*  module node information */
typedef struct {
    struct list_head list;
    hi_char name[HI_CONFACCESS_NAME_MAX_LEN];     /* <module name */
    hi_char path[HI_CONFACCESS_PATH_MAX_LEN];     /* <module path */
    hi_char ini_file[HI_CONFACCESS_PATH_MAX_LEN]; /* <module ini filename, not included path */
    ini_dictionary *ini_dir;
} conf_access_module;

/* cfg node information */
typedef struct {
    struct list_head list;
    hi_char cfg_name[HI_CONFACCESS_NAME_MAX_LEN];
    hi_char cfg_path[HI_CONFACCESS_PATH_MAX_LEN];
    conf_access_module module;
    pthread_mutex_t mutex_lock;
} conf_access_node;

/* cfg list */
static struct list_head g_config_access_list = LIST_HEAD_INIT(g_config_access_list);

/* configure count in list */
static hi_s32 g_conf_access_count = 0;

static hi_void confaccess_del_module_list(conf_access_node *cfg_node)
{
    struct list_head *pos = NULL;
    conf_access_module *module = NULL;

    list_for_each(pos, &(cfg_node->module.list)) {
        list_del(pos);
        module = list_entry((unsigned long)(uintptr_t)pos, conf_access_module, list);
        scene_logd("Module: inifile[%s]\n", module->ini_file);
        free_ini_info_dict(module->ini_dir);
        module->ini_dir = NULL;
        ot_scenecomm_safe_free(module);
        pos = &(cfg_node->module.list);
    }
}

static hi_s32 confaccess_get_all_modules_info(hi_s32 module_num, conf_access_node *cfg_pos, const hi_char *cfg_path_buf)
{
    for (hi_s32 index = 0; index < module_num; ++index) {
        /* Malloc ModuleNode Memory */
        conf_access_module *module_info = (conf_access_module *)malloc(sizeof(conf_access_module));
        if (module_info == NULL) {
            scene_loge("malloc failed\n");
            return HI_CONFACCESS_EMALLOC;
        }

        /* Module Name */
        hi_char module_node_key[HI_CONFACCESS_KEY_MAX_LEN] = {0};
        snprintf_truncated_s(module_node_key, HI_CONFACCESS_KEY_MAX_LEN, "module:module%d", index + 1);
        const hi_char *value = ini_get_string(cfg_pos->module.ini_dir, (const char *)module_node_key, NULL);
        if (value == NULL) {
            scene_loge("Load %s failed\n", module_node_key);
            ot_scenecomm_safe_free(module_info);
            continue;
        }
        snprintf_truncated_s(module_info->name, HI_CONFACCESS_NAME_MAX_LEN, "%s", value);

        /* Module Path */
        snprintf_truncated_s(module_info->path, HI_CONFACCESS_PATH_MAX_LEN, "%s/", cfg_path_buf);

        if ((strlen(module_info->path) > 1) &&
            ((module_info->path[strlen(module_info->path) - 1]) == CONFACCESS_PATH_SEPARATOR)) {
            module_info->path[strlen(module_info->path) - 1] = '\0';
        }

        /* Module IniFile */
        snprintf_truncated_s(module_node_key, HI_CONFACCESS_KEY_MAX_LEN, "%s:cfg_filename", module_info->name);
        value = ini_get_string(cfg_pos->module.ini_dir, (const char *)module_node_key, NULL);
        if (value == NULL) {
            scene_loge("Load %s failed\n", module_node_key);
            ot_scenecomm_safe_free(module_info);
            continue;
        }

        snprintf_truncated_s(module_info->ini_file, HI_CONFACCESS_PATH_MAX_LEN, "%s/%s", module_info->path, value);

        /* Module IniDir */
        module_info->ini_dir = ini_process_file(module_info->ini_file);
        if (module_info->ini_dir == NULL) {
            scene_loge("Load %s failed, ini errorId %#x\n", module_info->ini_file, ini_get_error_id());
            ot_scenecomm_safe_free(module_info);
            continue;
        }

        /* Add ModuleNode to list */
        list_add(&(module_info->list), &(cfg_pos->module.list));
    }

    return HI_SUCCESS;
}

static hi_s32 confaccess_init(const hi_char *cfg_name, const hi_char *cfg_path, hi_u32 *out_module_num)
{
    hi_char cfg_path_buf[HI_CONFACCESS_PATH_MAX_LEN];
    /* Malloc CfgNode Memory */
    conf_access_node *cfg_pos = (conf_access_node *)calloc(1, sizeof(conf_access_node));
    ot_scenecomm_check_pointer_return(cfg_pos, HI_CONFACCESS_EMALLOC);

    /* Record CfgNode Information */
    snprintf_truncated_s(cfg_pos->cfg_name, HI_CONFACCESS_NAME_MAX_LEN, "%s", cfg_name);
    snprintf_truncated_s(cfg_pos->cfg_path, HI_CONFACCESS_PATH_MAX_LEN, "%s", cfg_path);
    hi_mutex_init_lock(cfg_pos->mutex_lock);
    hi_mutex_lock(cfg_pos->mutex_lock);
    cfg_pos->module.list.next = &(cfg_pos->module.list);
    cfg_pos->module.list.prev = &(cfg_pos->module.list);
    cfg_pos->module.ini_dir = ini_process_file(cfg_path);
    if (cfg_pos->module.ini_dir == NULL) {
        scene_loge("load %s failed\n", cfg_path);
        goto exit;
    }

    snprintf_truncated_s(cfg_pos->module.ini_file, HI_CONFACCESS_PATH_MAX_LEN, "%s", cfg_path);
    snprintf_truncated_s(cfg_path_buf, HI_CONFACCESS_PATH_MAX_LEN, "%s", cfg_path);
#ifndef __LITEOS__
    dirname(cfg_path_buf);
#else
    hi_char *dir_pos = strrchr(cfg_path, CONFACCESS_PATH_SEPARATOR);
    if (dir_pos - cfg_path + 1 > HI_CONFACCESS_PATH_MAX_LEN) {
        scene_loge("memcpy_s failed\n");
        goto exit;
    }

    cfg_path_buf[dir_pos - cfg_path + 1] = 0;
    scene_logd("file path: %s\n", cfg_path_buf);
#endif

    /* Get Module Count and Default Path */
    *out_module_num = (hi_u32)ini_get_int(cfg_pos->module.ini_dir, (hi_char *)"module:module_num", 0);
#ifndef __LITEOS__
    scene_logd("ModuleNum[%d], DefaultPath[%s]\n", (*out_module_num), cfg_path_buf);
#endif

    /* Get All Module Information */
    hi_s32 ret = confaccess_get_all_modules_info((*out_module_num), cfg_pos, (const char *)cfg_path_buf);
    if (ret != HI_SUCCESS) {
        confaccess_del_module_list(cfg_pos);
        free_ini_info_dict(cfg_pos->module.ini_dir);
        goto exit;
    }
    hi_mutex_unlock(cfg_pos->mutex_lock);

    /* Add CfgNode to list */
    list_add(&(cfg_pos->list), &(g_config_access_list));

    g_conf_access_count++;
    scene_logd("CfgName[%s] ModuleCnt[%d] Count[%d]\n", cfg_pos->cfg_name, *out_module_num, g_conf_access_count);
    return HI_SUCCESS;

exit:
    hi_mutex_unlock(cfg_pos->mutex_lock);
    hi_mutex_destroy(cfg_pos->mutex_lock);
    ot_scenecomm_safe_free(cfg_pos);
    return HI_FAILURE;
}

hi_s32 ot_confaccess_init(const hi_char *cfg_name, const hi_char *cfg_path, hi_u32 *module_num)
{
    ot_scenecomm_check_pointer_return(cfg_name, HI_CONFACCESS_EINVAL);
    ot_scenecomm_check_pointer_return(cfg_path, HI_CONFACCESS_EINVAL);
    ot_scenecomm_check_expr_return(strlen(cfg_name) < HI_CONFACCESS_NAME_MAX_LEN, HI_CONFACCESS_EINVAL);
    ot_scenecomm_check_expr_return(strlen(cfg_path) < HI_CONFACCESS_PATH_MAX_LEN, HI_CONFACCESS_EINVAL);

    /* Check CfgName Exist or not */
    struct list_head *pos = NULL;
    conf_access_node *cfg_node = NULL;
    list_for_each(pos, &(g_config_access_list)) {
        cfg_node = list_entry((unsigned long)(uintptr_t)pos, conf_access_node, list);
        if (strncmp(cfg_node->cfg_name, cfg_name, HI_CONFACCESS_NAME_MAX_LEN) == 0) {
            scene_logw("%s already be inited\n", cfg_name);
            return HI_CONFACCESS_EREINIT;
        }
    }

    /* Init Cfg */
    if (g_conf_access_count == 0) {
        g_config_access_list.next = &(g_config_access_list);
        g_config_access_list.prev = &(g_config_access_list);
    }

    hi_s32 ret = confaccess_init(cfg_name, cfg_path, module_num);
    ot_scenecomm_check_return_with_errinfo(ret, ret, "AddCfg");
    return HI_SUCCESS;
}

hi_s32 ot_confaccess_deinit(const hi_char *cfg_name)
{
    ot_scenecomm_check_pointer_return(cfg_name, HI_CONFACCESS_EINVAL);
    ot_scenecomm_check_expr_return(strlen(cfg_name) < HI_CONFACCESS_NAME_MAX_LEN, HI_CONFACCESS_EINVAL);
    ot_scenecomm_check_expr_return(g_conf_access_count > 0, HI_CONFACCESS_ENOTINIT);

    struct list_head *node_pos = NULL;
    struct list_head *module_pos = NULL;
    conf_access_node *cfg_node = NULL;
    conf_access_module *cfg_module = NULL;

    list_for_each(node_pos, &(g_config_access_list)) {
        cfg_node = list_entry((unsigned long)(uintptr_t)node_pos, conf_access_node, list);
        if (strncmp(cfg_node->cfg_name, cfg_name, HI_CONFACCESS_NAME_MAX_LEN) == 0) {
            hi_mutex_lock(cfg_node->mutex_lock);
            list_del(node_pos);
            list_for_each(module_pos, &(cfg_node->module.list)) {
                list_del(module_pos);
                cfg_module = list_entry((unsigned long)(uintptr_t)module_pos, conf_access_module, list);
                free_ini_info_dict(cfg_module->ini_dir);
                cfg_module->ini_dir = NULL;
                ot_scenecomm_safe_free(cfg_module);
                module_pos = &(cfg_node->module.list);
            }
            free_ini_info_dict(cfg_node->module.ini_dir);
            cfg_node->module.ini_dir = NULL;
            hi_mutex_unlock(cfg_node->mutex_lock);
            hi_mutex_destroy(cfg_node->mutex_lock);
            ot_scenecomm_safe_free(cfg_node);

            node_pos = &(g_config_access_list);
            g_conf_access_count--;
            scene_logd("Now CfgList count[%d]\n", g_conf_access_count);
            return HI_SUCCESS;
        }
    }
    return HI_CONFACCESS_ECFG_NOTEXIST;
}

hi_s32 ot_confaccess_get_string(const hi_char *cfg_name, const hi_char *module, const hi_char *conf_item,
    const hi_char *default_val, hi_char ** const value_vector)
{
    ot_scenecomm_check_pointer_return(cfg_name, HI_CONFACCESS_EINVAL);
    ot_scenecomm_check_pointer_return(module, HI_CONFACCESS_EINVAL);
    ot_scenecomm_check_pointer_return(conf_item, HI_CONFACCESS_EINVAL);
    ot_scenecomm_check_pointer_return(value_vector, HI_CONFACCESS_EINVAL);
    ot_scenecomm_check_expr_return(strlen(cfg_name) < HI_CONFACCESS_NAME_MAX_LEN, HI_CONFACCESS_EINVAL);
    ot_scenecomm_check_expr_return(strlen(module) < HI_CONFACCESS_NAME_MAX_LEN, HI_CONFACCESS_EINVAL);
    ot_scenecomm_check_expr_return(strlen(conf_item) < HI_CONFACCESS_KEY_MAX_LEN, HI_CONFACCESS_EINVAL);

    struct list_head *node_pos = NULL;
    struct list_head *module_pos = NULL;
    conf_access_node *cfg_node = NULL;
    conf_access_module *cfg_module = NULL;

    list_for_each(node_pos, &(g_config_access_list)) {
        cfg_node = list_entry((unsigned long)(uintptr_t)node_pos, conf_access_node, list);
        if (strncmp(cfg_node->cfg_name, cfg_name, HI_CONFACCESS_NAME_MAX_LEN) != 0) {
            continue;
        }
        hi_mutex_lock(cfg_node->mutex_lock);
        list_for_each(module_pos, &(cfg_node->module.list)) {
            cfg_module = list_entry((unsigned long)(uintptr_t)module_pos, conf_access_module, list);
            if (strncmp(cfg_module->name, module, HI_CONFACCESS_NAME_MAX_LEN) != 0) {
                continue;
            }
            *value_vector = (hi_char *)ini_get_string(cfg_module->ini_dir, conf_item, default_val);
            if (*value_vector != NULL) {
                *value_vector = strdup(*value_vector);
            }
            hi_mutex_unlock(cfg_node->mutex_lock);
            return HI_SUCCESS;
        }
        hi_mutex_unlock(cfg_node->mutex_lock);
        return HI_CONFACCESS_EMOD_NOTEXIST;
    }
    return HI_CONFACCESS_ECFG_NOTEXIST;
}

hi_s32 ot_confaccess_get_str(const hi_char *cfg_name, const hi_char *module, const hi_char *conf_item,
    hi_char **value_vector)
{
    return ot_confaccess_get_string(cfg_name, module, conf_item, NULL, value_vector);
}

hi_s32 ot_confaccess_get_int(const hi_char *cfg_name, const hi_char *module, const hi_char *conf_item,
    hi_s32 default_val, hi_s32 * const out_value)
{
    ot_scenecomm_check_pointer_return(out_value, HI_CONFACCESS_EINVAL);

    hi_char *value = NULL;
    hi_s32 ret = ot_confaccess_get_string(cfg_name, module, conf_item, NULL, &value);
    ot_scenecomm_check_return(ret, ret);

    if (value == NULL) {
        *out_value = default_val;
    } else {
        *out_value = (hi_s32)strtol(value, NULL, 10); /* base 10 */
        ot_scenecomm_safe_free(value);
    }
    return HI_SUCCESS;
}

hi_s32 ot_confaccess_get_long_long(const hi_char *cfg_name, const hi_char *module, const hi_char *conf_item,
    hi_s32 default_val, hi_s64 * const out_value)
{
    ot_scenecomm_check_pointer_return(out_value, HI_CONFACCESS_EINVAL);

    hi_char *value = NULL;
    hi_s32 ret;
    ret = ot_confaccess_get_string(cfg_name, module, conf_item, NULL, &value);
    ot_scenecomm_check_return(ret, ret);
    if (ret != HI_SUCCESS) {
        return ret;
    }

    if (value == NULL) {
        *out_value = (long long)default_val;
    } else {
        *out_value = strtoll(value, NULL, 10); /* base 10 */
        ot_scenecomm_safe_free(value);
    }

    return ret;
}


hi_s32 ot_confaccess_get_double(const hi_char *cfg_name, const hi_char *module, const hi_char *conf_item,
    hi_double default_val, hi_double * const out_value)
{
    ot_scenecomm_check_pointer_return(out_value, HI_CONFACCESS_EINVAL);

    hi_char *value = NULL;
    hi_s32 ret = ot_confaccess_get_string(cfg_name, module, conf_item, NULL, &value);
    ot_scenecomm_check_return(ret, ret);

    if (value == NULL) {
        *out_value = default_val;
    } else {
        *out_value = strtof(value, NULL);
        ot_scenecomm_safe_free(value);
    }
    return HI_SUCCESS;
}


hi_s32 ot_confaccess_get_bool(const hi_char *cfg_name, const hi_char *module, const hi_char *conf_item,
    hi_bool default_val, hi_bool *out_value)
{
    ot_scenecomm_check_pointer_return(out_value, HI_CONFACCESS_EINVAL);

    hi_char *value = NULL;
    hi_s32 ret = ot_confaccess_get_string(cfg_name, module, conf_item, NULL, &value);
    ot_scenecomm_check_return(ret, ret);

    if (value == NULL) {
        *out_value = default_val;
    } else {
        if ((value[0] == 'y') || (value[0] == 'Y') || (value[0] == '1') || (value[0] == 't') || (value[0] == 'T')) {
            *out_value = HI_TRUE;
        } else if ((value[0] == 'n') || (value[0] == 'N') || (value[0] == '0') || (value[0] == 'f') ||
            value[0] == 'F') {
            *out_value = HI_FALSE;
        } else {
            *out_value = default_val;
        }
        ot_scenecomm_safe_free(value);
    }
    return HI_SUCCESS;
}

hi_s32 ot_confaccess_set_string(const hi_char *cfg_name, const hi_char *module, const hi_char *conf_item,
    const hi_char *value)
{
    ot_scenecomm_check_pointer_return(cfg_name, HI_CONFACCESS_EINVAL);
    ot_scenecomm_check_pointer_return(module, HI_CONFACCESS_EINVAL);
    ot_scenecomm_check_pointer_return(conf_item, HI_CONFACCESS_EINVAL);
    ot_scenecomm_check_pointer_return(value, HI_CONFACCESS_EINVAL);
    ot_scenecomm_check_expr_return(strlen(cfg_name) < HI_CONFACCESS_NAME_MAX_LEN, HI_CONFACCESS_EINVAL);
    ot_scenecomm_check_expr_return(strlen(module) < HI_CONFACCESS_NAME_MAX_LEN, HI_CONFACCESS_EINVAL);
    ot_scenecomm_check_expr_return(strlen(conf_item) < HI_CONFACCESS_KEY_MAX_LEN, HI_CONFACCESS_EINVAL);

    struct list_head *node_pos = NULL;
    struct list_head *module_pos = NULL;
    conf_access_node *cfg_node = NULL;
    conf_access_module *cfg_module = NULL;

    list_for_each(node_pos, &(g_config_access_list)) {
        cfg_node = list_entry((unsigned long)(uintptr_t)node_pos, conf_access_node, list);
        if (strncmp(cfg_node->cfg_name, cfg_name, HI_CONFACCESS_NAME_MAX_LEN) != 0) {
            continue;
        }
        hi_mutex_lock(cfg_node->mutex_lock);
        list_for_each(module_pos, &(cfg_node->module.list)) {
            cfg_module = list_entry((unsigned long)(uintptr_t)module_pos, conf_access_module, list);
            if (strncmp(cfg_module->name, module, HI_CONFACCESS_NAME_MAX_LEN) != 0) {
                continue;
            }
            if (set_val_for_dict(cfg_module->ini_dir, conf_item, value) != 0) {
                scene_loge("module[%s] confitem[%s] not exist\n", module, conf_item);
                hi_mutex_unlock(cfg_node->mutex_lock);
                return HI_CONFACCESS_EITEM_NOTEXIST;
            }
            hi_mutex_unlock(cfg_node->mutex_lock);
            return HI_SUCCESS;
        }
        hi_mutex_unlock(cfg_node->mutex_lock);
        return HI_CONFACCESS_EMOD_NOTEXIST;
    }
    return HI_CONFACCESS_ECFG_NOTEXIST;
}

#ifdef __cplusplus
}
#endif /* __cplusplus */
