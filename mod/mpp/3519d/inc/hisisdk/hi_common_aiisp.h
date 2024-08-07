/*
  Copyright (c), 2001-2024, Shenshu Tech. Co., Ltd.
 */

#ifndef __HI_COMMON_AIISP_H__
#define __HI_COMMON_AIISP_H__

#include "hi_common.h"
#include "hi_common_video.h"
#include "ot_common_aiisp.h"
#include "ot_aiisp.h"

#ifdef __cplusplus
extern "C" {
#endif

#define HI_AIISP_MAX_MODEL_NUM               OT_AIISP_MAX_MODEL_NUM
#define HI_AIISP_AUTO_ISO_NUM                OT_AIISP_AUTO_ISO_NUM
#define HI_AIISP_MAX_DEPTH                   OT_AIISP_MAX_DEPTH
#define HI_AIISP_MAX_NAME_LEN                OT_AIISP_MAX_NAME_LEN

#define HI_ERR_AIISP_INVALID_PIPE_ID         OT_ERR_AIISP_INVALID_PIPE_ID
#define HI_ERR_AIISP_ILLEGAL_PARAM           OT_ERR_AIISP_ILLEGAL_PARAM
#define HI_ERR_AIISP_NULL_PTR                OT_ERR_AIISP_NULL_PTR
#define HI_ERR_AIISP_NOT_SUPPORT             OT_ERR_AIISP_NOT_SUPPORT
#define HI_ERR_AIISP_NOT_PERM                OT_ERR_AIISP_NOT_PERM
#define HI_ERR_AIISP_NO_MEM                  OT_ERR_AIISP_NO_MEM
#define HI_ERR_AIISP_NOT_READY               OT_ERR_AIISP_NOT_READY

#define HI_AIISP_TYPE_AIBNR                  OT_AIISP_TYPE_AIBNR
#define HI_AIISP_TYPE_AIDRC                  OT_AIISP_TYPE_AIDRC
#define HI_AIISP_TYPE_AIDM                   OT_AIISP_TYPE_AIDM
#define HI_AIISP_TYPE_AI3DNR                 OT_AIISP_TYPE_AI3DNR
#define HI_AIISP_TYPE_AIDESTRIP              OT_AIISP_TYPE_AIDESTRIP
#define HI_AIISP_TYPE_NUC                    OT_AIISP_TYPE_NUC
#define HI_AIISP_TYPE_BUTT                   OT_AIISP_TYPE_BUTT

typedef ot_aiisp_type                        hi_aiisp_type;
typedef ot_aiisp_mem_info                    hi_aiisp_mem_info;
typedef ot_aiisp_model                       hi_aiisp_model;
typedef ot_aiisp_thread_attr                 hi_aiisp_thread_attr;

#ifdef __cplusplus
}
#endif

#endif /* __HI_COMMON_AIISP_H__ */
