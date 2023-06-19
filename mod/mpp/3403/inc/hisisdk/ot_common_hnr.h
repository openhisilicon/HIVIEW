/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */

#ifndef OT_COMMON_HNR_H
#define OT_COMMON_HNR_H

#include "ot_common.h"
#include "ot_common_video.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/* GENERAL STRUCTURES */
/*
 * HNR Error inner Code
 */
typedef enum {
    OT_ERR_CODE_HNR_BIN_NOT_MATACH  = 0x50, /* bin not matach */
    OT_ERR_CODE_HNR_BUTT            = 0x51,
} ot_hnr_err_code;

#define OT_HNR_MAX_CFG_NUM  32

#define OT_ERR_HNR_INVALID_PIPE_ID OT_DEFINE_ERR(OT_ID_HNR, OT_ERR_LEVEL_ERROR, OT_ERR_INVALID_PIPE_ID)
#define OT_ERR_HNR_ILLEGAL_PARAM   OT_DEFINE_ERR(OT_ID_HNR, OT_ERR_LEVEL_ERROR, OT_ERR_ILLEGAL_PARAM)
#define OT_ERR_HNR_NULL_PTR        OT_DEFINE_ERR(OT_ID_HNR, OT_ERR_LEVEL_ERROR, OT_ERR_NULL_PTR)
#define OT_ERR_HNR_NOT_SUPPORT     OT_DEFINE_ERR(OT_ID_HNR, OT_ERR_LEVEL_ERROR, OT_ERR_NOT_SUPPORT)
#define OT_ERR_HNR_NOT_PERM        OT_DEFINE_ERR(OT_ID_HNR, OT_ERR_LEVEL_ERROR, OT_ERR_NOT_PERM)
#define OT_ERR_HNR_NO_MEM          OT_DEFINE_ERR(OT_ID_HNR, OT_ERR_LEVEL_ERROR, OT_ERR_NO_MEM)
#define OT_ERR_HNR_NOT_READY       OT_DEFINE_ERR(OT_ID_HNR, OT_ERR_LEVEL_ERROR, OT_ERR_NOT_READY)
#define OT_ERR_HNR_BIN_NOT_MATACH  OT_DEFINE_ERR(OT_ID_HNR, OT_ERR_LEVEL_ERROR, OT_ERR_CODE_HNR_BIN_NOT_MATACH)

/* Mem information */
typedef struct {
    td_phys_addr_t phys_addr; /* RW; The physical address of the memory */
    td_void *virt_addr; /* RW; The virtual address of the memory */
    td_u32  size;      /* RW; The size of memory */
} ot_hnr_mem_info;

typedef struct {
    ot_hnr_mem_info mem_info;
    ot_size image_size;
    td_bool is_wdr_mode;
} ot_hnr_cfg;

typedef struct {
    td_u32 sfs; /* RW; spatial filter strength. Range:[0, 31]; Format:5.0 */
    td_u32 tfs; /* RW; temporal filter strength. Range:[0, 31]; Format:5.0 */
} ot_hnr_param;

typedef struct {
    ot_hnr_param param;
} ot_hnr_manual_attr;

typedef struct {
    td_u32 param_num;
    td_u32 *iso;
    ot_hnr_param *param;
} ot_hnr_auto_attr;

typedef struct {
    td_bool enable; /* RW; Range:[0x0, 0x1]; Format:1.0 */
    td_bool bnr_bypass; /* RW; Range:[0x0, 0x1]; Format:1.0 */
    td_bool normal_blend; /* RW; only to normal mode, Range:[0x0, 0x1]; Format:1.0 */
    ot_op_mode op_type;
    ot_hnr_manual_attr manual_attr;
    ot_hnr_auto_attr auto_attr;
} ot_hnr_attr;

typedef enum {
    OT_HNR_REF_MODE_NORM = 0,
    OT_HNR_REF_MODE_NONE,
    OT_HNR_REF_MODE_NONE_ADVANCED,
    OT_HNR_REF_MODE_BUTT
} ot_hnr_ref_mode;

typedef struct {
    ot_hnr_ref_mode ref_mode;
} ot_hnr_alg_cfg;

typedef struct {
    td_u32 cpu_id; /* RW; pthread of hnr bind to cpu core, Range:[0, 3]; Format:2.0 */
} ot_hnr_thread_attr;

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* OT_COMMON_HNR_H */

