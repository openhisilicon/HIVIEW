/*
  Copyright (c), 2001-2024, Shenshu Tech. Co., Ltd.
 */

#ifndef OT_COMMON_AIISP_H
#define OT_COMMON_AIISP_H

#include "ot_common.h"
#include "ot_common_video.h"
#include "ot_errno.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/* GENERAL STRUCTURES */
#define OT_AIISP_MAX_MODEL_NUM 16
#define OT_AIISP_AUTO_ISO_NUM 16
#define OT_AIISP_MAX_DEPTH 8
#define OT_AIISP_MAX_NAME_LEN 32

#define OT_ERR_AIISP_INVALID_PIPE_ID OT_DEFINE_ERR(OT_ID_AIISP, OT_ERR_LEVEL_ERROR, OT_ERR_INVALID_PIPE_ID)
#define OT_ERR_AIISP_ILLEGAL_PARAM OT_DEFINE_ERR(OT_ID_AIISP, OT_ERR_LEVEL_ERROR, OT_ERR_ILLEGAL_PARAM)
#define OT_ERR_AIISP_NULL_PTR OT_DEFINE_ERR(OT_ID_AIISP, OT_ERR_LEVEL_ERROR, OT_ERR_NULL_PTR)
#define OT_ERR_AIISP_NOT_SUPPORT OT_DEFINE_ERR(OT_ID_AIISP, OT_ERR_LEVEL_ERROR, OT_ERR_NOT_SUPPORT)
#define OT_ERR_AIISP_NOT_PERM OT_DEFINE_ERR(OT_ID_AIISP, OT_ERR_LEVEL_ERROR, OT_ERR_NOT_PERM)
#define OT_ERR_AIISP_NO_MEM OT_DEFINE_ERR(OT_ID_AIISP, OT_ERR_LEVEL_ERROR, OT_ERR_NO_MEM)
#define OT_ERR_AIISP_NOT_READY OT_DEFINE_ERR(OT_ID_AIISP, OT_ERR_LEVEL_ERROR, OT_ERR_NOT_READY)

typedef enum {
    OT_AIISP_TYPE_AIBNR = 0,
    OT_AIISP_TYPE_AIDRC = 1,
    OT_AIISP_TYPE_AIDM = 2,
    OT_AIISP_TYPE_AI3DNR = 3,
    OT_AIISP_TYPE_AIDESTRIP = 4,
    OT_AIISP_TYPE_NUC = 5,
    OT_AIISP_TYPE_BUTT
} ot_aiisp_type;

typedef struct {
    td_phys_addr_t phys_addr; /* RW; The physical address of the memory */
    td_void *virt_addr;       /* RW; The virtual address of the memory */
    td_u32 size;              /* RW; The size of memory */
} ot_aiisp_mem_info;

typedef struct {
    td_bool preempted_en;
    ot_aiisp_mem_info mem_info;
    ot_size image_size;
} ot_aiisp_model;

typedef struct {
    td_s32 cpu_id; /* RW; pthread of aiisp bind to cpu core, Range:[0, 1]; Format:1.0 */
} ot_aiisp_thread_attr;

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* OT_COMMON_AIISP_H */
