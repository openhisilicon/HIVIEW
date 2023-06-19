/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */

#ifndef OT_COMMON_VB_H
#define OT_COMMON_VB_H

#include "ot_type.h"
#include "ot_errno.h"
#include "ot_debug.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef td_u32 ot_vb_pool;
typedef td_u32 ot_vb_blk;

#define OT_VB_INVALID_POOL_ID              (-1U)
#define OT_VB_INVALID_HANDLE              (-1U)

#define OT_VB_MAX_COMMON_POOLS              16
#define OT_VB_MAX_MOD_COMMON_POOLS          16

/* user ID for VB */
#define OT_VB_MAX_USER                    OT_VB_UID_BUTT
#define OT_RESERVE_MMZ_NAME               "window"

typedef enum {
    OT_VB_UID_VI = 0,
    OT_VB_UID_VO = 1,
    OT_VB_UID_VGS = 2,
    OT_VB_UID_VENC = 3,
    OT_VB_UID_VDEC = 4,
    OT_VB_UID_H265E = 5,
    OT_VB_UID_H264E = 6,
    OT_VB_UID_JPEGE = 7,
    OT_VB_UID_JPEGD = 8,
    OT_VB_UID_VPSS = 9,
    OT_VB_UID_DIS = 10,
    OT_VB_UID_USER = 11,
    OT_VB_UID_PCIV = 12,
    OT_VB_UID_AI = 13,
    OT_VB_UID_AENC = 14,
    OT_VB_UID_RC = 15,
    OT_VB_UID_VFMW = 16,
    OT_VB_UID_GDC = 17,
    OT_VB_UID_AVS = 18,
    OT_VB_UID_DPU_RECT = 19,
    OT_VB_UID_DPU_MATCH = 20,
    OT_VB_UID_MCF = 21,
    OT_VB_UID_VDA = 22,
    OT_VB_UID_VPP = 23,
    OT_VB_UID_COMMON = 24,
    OT_VB_UID_UVC = 25,
    OT_VB_UID_VDEC_ADAPT = 26,
    OT_VB_UID_BUTT = 27,
} ot_vb_uid;

typedef enum {
    OT_VB_REMAP_MODE_NONE = 0, /* no remap */
    OT_VB_REMAP_MODE_NOCACHE = 1, /* no cache remap */
    OT_VB_REMAP_MODE_CACHED = 2, /* cache remap, if you use this mode, you should flush cache by yourself */
    OT_VB_REMAP_MODE_BUTT
} ot_vb_remap_mode;

/* General common pool use this owner id, module common pool use VB_UID as owner id */
#define OT_POOL_OWNER_COMMON              (-1)

/* Private pool use this owner id */
#define OT_POOL_OWNER_PRIVATE             (-2)
#define OT_POOL_OWNER_MODULE              (-3)
#define OT_POOL_OWNER_USER                (-4)

typedef struct {
    td_u64 blk_size;
    td_u32 blk_cnt;
    ot_vb_remap_mode remap_mode;
    td_char mmz_name[OT_MAX_MMZ_NAME_LEN];
} ot_vb_pool_cfg;

typedef struct {
    td_u32 max_pool_cnt;
    ot_vb_pool_cfg common_pool[OT_VB_MAX_COMMON_POOLS];
} ot_vb_cfg;

typedef struct {
    td_bool is_common_pool;
    td_u32 blk_cnt;
    td_u32 free_blk_cnt;
} ot_vb_pool_status;

typedef struct {
    td_u32 supplement_cfg;
} ot_vb_supplement_cfg;

typedef struct {
    td_u32 blk_cnt;
    td_u64 blk_size;
    td_u64 pool_size;
    td_phys_addr_t pool_phy_addr;
    ot_vb_remap_mode remap_mode;
} ot_vb_pool_info;

#define OT_VB_SUPPLEMENT_JPEG_MASK        0x1
#define OT_VB_SUPPLEMENT_MOTION_DATA_MASK 0x2
#define OT_VB_SUPPLEMENT_DNG_MASK         0x4
#define OT_VB_SUPPLEMENT_MISC_MASK        0x8
#define OT_VB_SUPPLEMENT_BNR_MOT_MASK     0x10

#define OT_ERR_VB_NULL_PTR             OT_DEFINE_ERR(OT_ID_VB, OT_ERR_LEVEL_ERROR, OT_ERR_NULL_PTR)
#define OT_ERR_VB_NO_MEM               OT_DEFINE_ERR(OT_ID_VB, OT_ERR_LEVEL_ERROR, OT_ERR_NO_MEM)
#define OT_ERR_VB_NO_BUF               OT_DEFINE_ERR(OT_ID_VB, OT_ERR_LEVEL_ERROR, OT_ERR_NO_BUF)
#define OT_ERR_VB_UNEXIST              OT_DEFINE_ERR(OT_ID_VB, OT_ERR_LEVEL_ERROR, OT_ERR_UNEXIST)
#define OT_ERR_VB_ILLEGAL_PARAM        OT_DEFINE_ERR(OT_ID_VB, OT_ERR_LEVEL_ERROR, OT_ERR_ILLEGAL_PARAM)
#define OT_ERR_VB_NOT_READY            OT_DEFINE_ERR(OT_ID_VB, OT_ERR_LEVEL_ERROR, OT_ERR_NOT_READY)
#define OT_ERR_VB_BUSY                 OT_DEFINE_ERR(OT_ID_VB, OT_ERR_LEVEL_ERROR, OT_ERR_BUSY)
#define OT_ERR_VB_NOT_PERM             OT_DEFINE_ERR(OT_ID_VB, OT_ERR_LEVEL_ERROR, OT_ERR_NOT_PERM)
#define OT_ERR_VB_SIZE_NOT_ENOUGH      OT_DEFINE_ERR(OT_ID_VB, OT_ERR_LEVEL_ERROR, OT_ERR_SIZE_NOT_ENOUGH)
#define OT_ERR_VB_NOT_SUPPORT          OT_DEFINE_ERR(OT_ID_VB, OT_ERR_LEVEL_ERROR, OT_ERR_NOT_SUPPORT)

#define ot_trace_vb(level, fmt, ...)  \
    OT_TRACE(level, OT_ID_VB, "[Func]:%s [Line]:%d [Info]:" fmt, __FUNCTION__, __LINE__, ##__VA_ARGS__)

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* OT_COMMON_VB_H */

