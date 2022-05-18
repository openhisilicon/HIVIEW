/*
* Copyright (c) Hisilicon Technologies Co., Ltd. 2019-2020. All rights reserved.
* Description:Initial Draft
* Author: Hisilicon multimedia software group
* Create: 2019/07/15
*/


#ifndef __HI_COMMON_VB_H__
#define __HI_COMMON_VB_H__

#include "hi_type.h"
#include "hi_errno.h"
#include "hi_debug.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif


typedef hi_u32 hi_vb_pool;
typedef hi_u32 hi_vb_blk;


#define HI_VB_INVALID_POOL_ID              (-1U)
#define HI_VB_INVALID_HANDLE              (-1U)

#define HI_VB_MAX_COMMON_POOLS              16
#define HI_VB_MAX_MOD_COMMON_POOLS          16

/* user ID for VB */
#define HI_VB_MAX_USER                    HI_VB_UID_BUTT
#define HI_RESERVE_MMZ_NAME               "window"

typedef enum {
    HI_VB_UID_VI = 0,
    HI_VB_UID_VO = 1,
    HI_VB_UID_VGS = 2,
    HI_VB_UID_VENC = 3,
    HI_VB_UID_VDEC = 4,
    HI_VB_UID_H265E = 5,
    HI_VB_UID_H264E = 6,
    HI_VB_UID_JPEGE = 7,
    HI_VB_UID_JPEGD = 8,
    HI_VB_UID_VPSS = 9,
    HI_VB_UID_DIS = 10,
    HI_VB_UID_USER = 11,
    HI_VB_UID_PCIV = 12,
    HI_VB_UID_AI = 13,
    HI_VB_UID_AENC = 14,
    HI_VB_UID_RC = 15,
    HI_VB_UID_VFMW = 16,
    HI_VB_UID_GDC = 17,
    HI_VB_UID_AVS = 18,
    HI_VB_UID_DPU_RECT = 19,
    HI_VB_UID_DPU_MATCH = 20,
    HI_VB_UID_MCF = 21,
    HI_VB_UID_VDA  = 22,
    HI_VB_UID_BUTT = 23,
} hi_vb_uid;

typedef enum {
    HI_VB_REMAP_MODE_NONE = 0, /* no remap */
    HI_VB_REMAP_MODE_NOCACHE = 1, /* no cache remap */
    HI_VB_REMAP_MODE_CACHED = 2, /* cache remap, if you use this mode, you should flush cache by yourself */
    HI_VB_REMAP_MODE_BUTT
} hi_vb_remap_mode;


/* Generall common pool use this owner id, module common pool use VB_UID as owner id */
#define HI_POOL_OWNER_COMMON              -1

/* Private pool use this owner id */
#define HI_POOL_OWNER_PRIVATE             -2


typedef struct {
    hi_u64 blk_size;
    hi_u32 blk_cnt;
    hi_vb_remap_mode remap_mode;
    hi_char mmz_name[HI_MAX_MMZ_NAME_LEN];
} hi_vb_pool_cfg;

typedef struct {
    hi_u32 max_pool_cnt;
    hi_vb_pool_cfg common_pool[HI_VB_MAX_COMMON_POOLS];
} hi_vb_cfg;

typedef struct {
    hi_bool is_common_pool;
    hi_u32 blk_cnt;
    hi_u32 free_blk_cnt;
} hi_vb_pool_status;

typedef struct {
    hi_u32 supplement_cfg;
} hi_vb_supplement_cfg;

typedef struct {
    hi_u32 blk_cnt;
    hi_u64 blk_size;
    hi_u64 pool_size;
    hi_phys_addr_t pool_phy_addr;
    hi_vb_remap_mode remap_mode;
} hi_vb_pool_info;

#define HI_VB_SUPPLEMENT_JPEG_MASK        0x1
#define HI_VB_SUPPLEMENT_ISP_INFO_MASK    0x2
#define HI_VB_SUPPLEMENT_MOTION_DATA_MASK 0x4
#define HI_VB_SUPPLEMENT_DNG_MASK         0x8
#define HI_VB_SUPPLEMENT_MISC_MASK        0x10


#define HI_ERR_VB_NULL_PTR             HI_DEFINE_ERR(HI_ID_VB, HI_ERR_LEVEL_ERROR, HI_ERR_NULL_PTR)
#define HI_ERR_VB_NO_MEM               HI_DEFINE_ERR(HI_ID_VB, HI_ERR_LEVEL_ERROR, HI_ERR_NO_MEM)
#define HI_ERR_VB_NO_BUF               HI_DEFINE_ERR(HI_ID_VB, HI_ERR_LEVEL_ERROR, HI_ERR_NO_BUF)
#define HI_ERR_VB_UNEXIST              HI_DEFINE_ERR(HI_ID_VB, HI_ERR_LEVEL_ERROR, HI_ERR_UNEXIST)
#define HI_ERR_VB_ILLEGAL_PARAM        HI_DEFINE_ERR(HI_ID_VB, HI_ERR_LEVEL_ERROR, HI_ERR_ILLEGAL_PARAM)
#define HI_ERR_VB_NOT_READY            HI_DEFINE_ERR(HI_ID_VB, HI_ERR_LEVEL_ERROR, HI_ERR_NOT_READY)
#define HI_ERR_VB_BUSY                 HI_DEFINE_ERR(HI_ID_VB, HI_ERR_LEVEL_ERROR, HI_ERR_BUSY)
#define HI_ERR_VB_NOT_PERM             HI_DEFINE_ERR(HI_ID_VB, HI_ERR_LEVEL_ERROR, HI_ERR_NOT_PERM)
#define HI_ERR_VB_SIZE_NOT_ENOUGH      HI_DEFINE_ERR(HI_ID_VB, HI_ERR_LEVEL_ERROR, HI_ERR_SIZE_NOT_ENOUGH)


#define hi_trace_vb(level, fmt, ...)                                                                         \
        do {                                                                                                     \
            HI_TRACE(level, HI_ID_VB, "[Func]:%s [Line]:%d [Info]:" fmt, __FUNCTION__, __LINE__, ##__VA_ARGS__); \
        } while (0)


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __HI_COMMON_VB_H_ */

