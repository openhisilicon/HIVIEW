/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */

#ifndef HI_SCENECOMM_H
#define HI_SCENECOMM_H

#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include "hi_type.h"
#include "ot_scenecomm_log.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* * \addtogroup     APPCOMM */
/* * @{ */ /* * <!-- [APPCOMM] */

/* * Invalid FD */
#define HI_SCENECOMM_FD_INVALID_VAL (-1)

/* * General String Length */
#define HI_SCENECOMM_COMM_STR_LEN (32 + 1)

/* * Path Maximum Length */
#define HI_SCENECOMM_MAX_PATH_LEN 64

/* * FileName Maximum Length */
#define HI_SCENECOMM_MAX_FILENAME_LEN 64

/* * Pointer Check */
#define ot_scenecomm_check_pointer_return(p, errcode) do { \
        if ((p) == NULL) {                                 \
            scene_loge("null pointer\n");                  \
            return errcode;                                \
        }                                                  \
    } while (0)

/* * Pointer Check return but without return value */
#define ot_scenecomm_check_pointer_return_no_value(p) do { \
        if ((p) == NULL) {                                 \
            scene_loge("null pointer\n");                  \
            return;                                        \
        }                                                  \
    } while (0)

/* * Expression Check */
#define ot_scenecomm_check_expr_return(expr, errcode) do {      \
        if ((expr) == 0) {                                      \
            scene_loge(RED " expr[%s] false" NONE "\n", #expr); \
            return errcode;                                     \
        }                                                       \
    } while (0)

/* * Expression Check */
#define ot_scenecomm_expr_true_return(expr, errcode) do {       \
        if ((expr) == 1) {                                      \
            scene_loge(RED " expr[%s] false" NONE "\n", #expr); \
            return errcode;                                     \
        }                                                       \
    } while (0)

/* * Return Result Check */
#define ot_scenecomm_check_return(ret, errcode) do {     \
        if (ret != HI_SUCCESS) {                         \
            scene_loge(RED " ret[%08x]" NONE "\n", ret); \
            return errcode;                              \
        }                                                \
    } while (0)
/* * Return Result UnReturn */
#define ot_scenecomm_check_unreturn(ret) do {            \
        if (ret != HI_SUCCESS) {                         \
            scene_loge(RED " ret[%08x]" NONE "\n", ret); \
        }                                                \
    } while (0)


#define ot_scenecomm_check(ret, errcode) do {                                 \
        if (ret != HI_SUCCESS) {                                              \
            scene_loge(RED " ret[%08x] errcode[%x]" NONE "\n", ret, errcode); \
        }                                                                     \
    } while (0)


/* * Return Result Check With ErrInformation */
#define ot_scenecomm_check_return_with_errinfo(ret, errcode, errstring) do { \
        if (ret != HI_SUCCESS) {                                             \
            scene_loge(RED " [%s] failed[%08x]" NONE "\n", errstring, ret);  \
            return errcode;                                                  \
        }                                                                    \
    } while (0)

/* * Range Check */
#define ot_scenecomm_check_range(value, min, max) (((value) <= (max) && (value) >= (min)) ? 1 : 0)

/* * Memory Safe Free */
#define ot_scenecomm_safe_free(p) do { \
        if ((p) != NULL) {             \
            free(p);                   \
            (p) = NULL;                \
        }                              \
    } while (0)

/* * Value Align */
#define HI_SCENECOMM_ALIGN(value, base) (((value) + (base)-1) / (base) * (base))

/* * strcmp enum string and value */
#define ot_scenecomm_strcmp_enum(enumStr, enumValue) strcmp(enumStr, #enumValue)

#ifndef MAX
#define MAX(a, b) (((a) < (b)) ? (b) : (a))
#endif

#ifndef MIN
#define MIN(a, b) (((a) > (b)) ? (b) : (a))
#endif

/* * Mutex Lock */
#ifndef hi_mutex_init_lock
#define hi_mutex_init_lock(mutex) do {          \
        (void)pthread_mutex_init(&mutex, NULL); \
    } while (0)
#endif
#ifndef hi_mutex_lock
#define hi_mutex_lock(mutex) do {         \
        (void)pthread_mutex_lock(&mutex); \
    } while (0)
#endif
#ifndef hi_mutex_unlock
#define hi_mutex_unlock(mutex) do {         \
        (void)pthread_mutex_unlock(&mutex); \
    } while (0)
#endif
#ifndef hi_mutex_destroy
#define hi_mutex_destroy(mutex) do {         \
        (void)pthread_mutex_destroy(&mutex); \
    } while (0)
#endif

/* * SCENE Error BaseId : [28bit~31bit] unique */
#define HI_SCENECOMM_ERR_BASEID (0x80000000L)

/* * SCENE Error Code Rule [ --base[4bit]--|--module[8bit]--|--error[20bit]--]
 * module : module enum value [HI_APP_MOD_E]
 * event_code : event code in specified module, unique in module
 */
#define HI_SCENECOMM_ERR_ID(module, err) ((hi_s32)((HI_SCENECOMM_ERR_BASEID) | ((module) << 20) | (err)))

/* * App Module ID */
typedef enum {
    HI_SCENE_MOD_SCENE = 0,
    HI_SCENE_MOD_CONFACCESS,
    HI_SCENE_MOD_BUTT
} ot_scene_mod;

#ifndef ARRAY_SIZE
#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))
#endif

#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /* End of #ifndef HI_APPCOMM_H */
