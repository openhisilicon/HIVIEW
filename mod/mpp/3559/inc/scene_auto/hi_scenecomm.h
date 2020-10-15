/**
* @file    hi_appcomm.h
* @brief   application common header.
*
* Copyright (c) 2017 Huawei Tech.Co.,Ltd
*
* @author    HiMobileCam Reference Develop Team
* @date      2017/12/12
* @version   1.0

*/
#ifndef __HI_APPCOMM_H__
#define __HI_APPCOMM_H__

#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include "hi_type.h"
#include "hi_scenecomm_log.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

/** \addtogroup     APPCOMM */
/** @{ */  /** <!-- [APPCOMM] */

/** Invalid FD */
#define HI_SCENECOMM_FD_INVALID_VAL (-1)

/** General String Length */
#define HI_SCENECOMM_COMM_STR_LEN (32+1)

/** Path Maximum Length */
#define HI_SCENECOMM_MAX_PATH_LEN (64)

/** FileName Maximum Length */
#define HI_SCENECOMM_MAX_FILENAME_LEN (64)

/** Pointer Check */
#define HI_SCENECOMM_CHECK_POINTER(p, errcode) \
    do { if (!(p)) { \
            MLOGE("null pointer\n");\
            return errcode; \
        } }while(0)

/** Expression Check */
#define HI_SCENECOMM_CHECK_EXPR(expr, errcode) \
    do { \
        if (!(expr)){   \
            MLOGE(RED" expr[%s] false"NONE"\n", #expr); \
            return errcode;  \
        }   \
    }while(0)

/** Expression Check With ErrInformation */
#define HI_SCENECOMM_CHECK_EXPR_WITH_ERRINFO(expr, errcode, errstring) \
    do { \
        if (!(expr)){   \
            MLOGE(RED" [%s] failed"NONE"\n", errstring); \
            return errcode;  \
        }   \
    }while(0)

/** Return Result Check */
#define HI_SCENECOMM_CHECK_RETURN(ret, errcode) \
    do { \
        if (HI_SUCCESS != ret){   \
            MLOGE(RED" ret[%08x]"NONE"\n", ret); \
            return errcode;  \
        }   \
    }while(0)

#define HI_SCENECOMM_CHECK(ret, errcode) \
    do { \
        if (HI_SUCCESS != ret){   \
            MLOGE(RED" ret[%08x] errcode[%x]"NONE"\n", ret, errcode); \
        }   \
    }while(0)


/** Return Result Check With ErrInformation */
#define HI_SCENECOMM_CHECK_RETURN_WITH_ERRINFO(ret, errcode, errstring) \
    do { \
        if (HI_SUCCESS != ret){   \
            MLOGE(RED" [%s] failed[%08x]"NONE"\n", errstring, ret); \
            return errcode;  \
        }   \
    }while(0)

/** Range Check */
#define HI_SCENECOMM_CHECK_RANGE(value, min, max) (((value) <= (max) && (value) >= (min)) ? 1 : 0)

/** Memory Safe Free */
#define HI_SCENECOMM_SAFE_FREE(p) do { if (NULL != (p)){ free(p); (p) = NULL; } }while(0)

/** Value Align */
#define HI_SCENECOMM_ALIGN(value, base) (((value)+(base) - 1)/(base)*(base))

/** strcmp enum string and value */
#define HI_SCENECOMM_STRCMP_ENUM(enumStr, enumValue) strcmp(enumStr, #enumValue)

#ifndef MAX
#define MAX(a, b) (((a) < (b)) ? (b) : (a))
#endif

#ifndef MIN
#define MIN(a, b) (((a) > (b)) ? (b) : (a))
#endif

/** Mutex Lock */
#define HI_MUTEX_INIT_LOCK(mutex)  do { (void) pthread_mutex_init(&mutex, NULL); } while(0)
#define HI_MUTEX_LOCK(mutex)       do { (void) pthread_mutex_lock(&mutex); } while(0)
#define HI_MUTEX_UNLOCK(mutex)     do { (void) pthread_mutex_unlock(&mutex); } while(0)
#define HI_MUTEX_DESTROY(mutex)    do { (void) pthread_mutex_destroy(&mutex); } while(0)

/** SCENE Error BaseId : [28bit~31bit] unique */
#define HI_SCENECOMM_ERR_BASEID (0x80000000L)

/** SCENE Error Code Rule [ --base[4bit]--|--module[8bit]--|--error[20bit]--]
    * module : module enum value [HI_APP_MOD_E]
    * event_code : event code in specified module, unique in module
    */
#define HI_SCENECOMM_ERR_ID( module, err) \
    ((HI_S32)( (HI_SCENECOMM_ERR_BASEID) | ((module) << 20 ) | (err) ))

/** App Module ID */
typedef enum hiSCENE_MOD_E
{
    HI_SCENE_MOD_SCENE = 0,
    HI_SCENE_MOD_CONFACCESS,
    HI_SCENE_MOD_BUTT
} HI_SCENE_MOD_E;

/** @}*/  /** <!-- ==== SCENECOMM End ====*/

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


#endif /* End of #ifndef __HI_APPCOMM_H__ */

