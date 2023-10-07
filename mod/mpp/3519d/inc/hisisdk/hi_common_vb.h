/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */

#ifndef HI_COMMON_VB_H
#define HI_COMMON_VB_H

#include "hi_type.h"
#include "hi_errno.h"
#include "hi_debug.h"
#include "ot_common_vb.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef ot_vb_pool hi_vb_pool;
typedef ot_vb_blk hi_vb_blk;
#define HI_VB_INVALID_POOL_ID OT_VB_INVALID_POOL_ID
#define HI_VB_INVALID_HANDLE OT_VB_INVALID_HANDLE
#define HI_VB_MAX_COMMON_POOLS OT_VB_MAX_COMMON_POOLS
#define HI_VB_MAX_MOD_COMMON_POOLS OT_VB_MAX_MOD_COMMON_POOLS
#define HI_VB_MAX_USER OT_VB_MAX_USER
#define HI_RESERVE_MMZ_NAME OT_RESERVE_MMZ_NAME
#define HI_VB_UID_VI OT_VB_UID_VI
#define HI_VB_UID_VO OT_VB_UID_VO
#define HI_VB_UID_VGS OT_VB_UID_VGS
#define HI_VB_UID_VENC OT_VB_UID_VENC
#define HI_VB_UID_VDEC OT_VB_UID_VDEC
#define HI_VB_UID_H265E OT_VB_UID_H265E
#define HI_VB_UID_H264E OT_VB_UID_H264E
#define HI_VB_UID_VPSS OT_VB_UID_VPSS
#define HI_VB_UID_DIS OT_VB_UID_DIS
#define HI_VB_UID_USER OT_VB_UID_USER
#define HI_VB_UID_PCIV OT_VB_UID_PCIV
#define HI_VB_UID_VFMW OT_VB_UID_VFMW
#define HI_VB_UID_GDC OT_VB_UID_GDC
#define HI_VB_UID_AVS OT_VB_UID_AVS
#define HI_VB_UID_DPU_RECT OT_VB_UID_DPU_RECT
#define HI_VB_UID_DPU_MATCH OT_VB_UID_DPU_MATCH
#define HI_VB_UID_MCF OT_VB_UID_MCF
#define HI_VB_UID_VDA OT_VB_UID_VDA
#define HI_VB_UID_VPP OT_VB_UID_VPP
#define HI_VB_UID_COMMON OT_VB_UID_COMMON
#define HI_VB_UID_AIISP OT_VB_UID_AIISP
#define HI_VB_UID_UVC OT_VB_UID_UVC
#define HI_VB_UID_BUTT OT_VB_UID_BUTT
typedef ot_vb_uid hi_vb_uid;
#define HI_VB_REMAP_MODE_NONE OT_VB_REMAP_MODE_NONE
#define HI_VB_REMAP_MODE_NOCACHE OT_VB_REMAP_MODE_NOCACHE
#define HI_VB_REMAP_MODE_CACHED OT_VB_REMAP_MODE_CACHED
#define HI_VB_REMAP_MODE_BUTT OT_VB_REMAP_MODE_BUTT
typedef ot_vb_remap_mode hi_vb_remap_mode;
#define HI_VB_POOL_TYPE_COMMON OT_VB_POOL_TYPE_COMMON
#define HI_VB_POOL_TYPE_PRIVATE OT_VB_POOL_TYPE_PRIVATE
#define HI_VB_POOL_TYPE_MODULE OT_VB_POOL_TYPE_MODULE
#define HI_VB_POOL_TYPE_USER OT_VB_POOL_TYPE_USER
#define HI_VB_POOL_TYPE_BUTT OT_VB_POOL_TYPE_BUTT
typedef ot_vb_pool_type hi_vb_pool_type;
typedef ot_vb_pool_cfg hi_vb_pool_cfg;
typedef ot_vb_cfg hi_vb_cfg;
typedef ot_vb_pool_status hi_vb_pool_status;
typedef ot_vb_supplement_cfg hi_vb_supplement_cfg;
typedef ot_vb_pool_info hi_vb_pool_info;
typedef ot_vb_common_pools_id hi_vb_common_pools_id;
#define HI_VB_SUPPLEMENT_JPEG_MASK OT_VB_SUPPLEMENT_JPEG_MASK
#define HI_VB_SUPPLEMENT_MOTION_DATA_MASK OT_VB_SUPPLEMENT_MOTION_DATA_MASK
#define HI_VB_SUPPLEMENT_DNG_MASK OT_VB_SUPPLEMENT_DNG_MASK
#define HI_VB_SUPPLEMENT_BNR_MOT_MASK OT_VB_SUPPLEMENT_BNR_MOT_MASK
#define HI_ERR_VB_NULL_PTR OT_ERR_VB_NULL_PTR
#define HI_ERR_VB_NO_MEM OT_ERR_VB_NO_MEM
#define HI_ERR_VB_NO_BUF OT_ERR_VB_NO_BUF
#define HI_ERR_VB_UNEXIST OT_ERR_VB_UNEXIST
#define HI_ERR_VB_ILLEGAL_PARAM OT_ERR_VB_ILLEGAL_PARAM
#define HI_ERR_VB_NOT_READY OT_ERR_VB_NOT_READY
#define HI_ERR_VB_BUSY OT_ERR_VB_BUSY
#define HI_ERR_VB_NOT_PERM OT_ERR_VB_NOT_PERM
#define HI_ERR_VB_SIZE_NOT_ENOUGH OT_ERR_VB_SIZE_NOT_ENOUGH
#define HI_ERR_VB_NOT_SUPPORT OT_ERR_VB_NOT_SUPPORT
#define hi_trace_vb ot_trace_vb

#ifdef __cplusplus
}
#endif
#endif /* HI_COMMON_VB_H */
