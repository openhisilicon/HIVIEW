/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */

#ifndef __HI_COMMON_HNR_H__
#define __HI_COMMON_HNR_H__

#include "hi_common.h"
#include "hi_common_video.h"
#include "ot_common_hnr.h"

#ifdef __cplusplus
extern "C" {
#endif

#define HI_ERR_CODE_HNR_BIN_NOT_MATACH OT_ERR_CODE_HNR_BIN_NOT_MATACH /* inner errno */
#define HI_ERR_CODE_HNR_BUTT OT_ERR_CODE_HNR_BUTT /* inner errno */

#define HI_HNR_MAX_CFG_NUM OT_HNR_MAX_CFG_NUM
#define HI_ERR_HNR_INVALID_PIPE_ID OT_ERR_HNR_INVALID_PIPE_ID
#define HI_ERR_HNR_ILLEGAL_PARAM OT_ERR_HNR_ILLEGAL_PARAM
#define HI_ERR_HNR_NULL_PTR OT_ERR_HNR_NULL_PTR
#define HI_ERR_HNR_NOT_SUPPORT OT_ERR_HNR_NOT_SUPPORT
#define HI_ERR_HNR_NOT_PERM OT_ERR_HNR_NOT_PERM
#define HI_ERR_HNR_NO_MEM OT_ERR_HNR_NO_MEM
#define HI_ERR_HNR_NOT_READY OT_ERR_HNR_NOT_READY
#define HI_ERR_HNR_BIN_NOT_MATACH OT_ERR_HNR_BIN_NOT_MATACH
typedef ot_hnr_mem_info hi_hnr_mem_info;
typedef ot_hnr_cfg hi_hnr_cfg;
typedef ot_hnr_param hi_hnr_param;
typedef ot_hnr_manual_attr hi_hnr_manual_attr;
typedef ot_hnr_auto_attr hi_hnr_auto_attr;
typedef ot_hnr_attr hi_hnr_attr;
typedef ot_hnr_alg_cfg hi_hnr_alg_cfg;
#define HI_HNR_REF_MODE_NORM OT_HNR_REF_MODE_NORM
#define HI_HNR_REF_MODE_NONE OT_HNR_REF_MODE_NONE
#define HI_HNR_REF_MODE_NONE_ADVANCED OT_HNR_REF_MODE_NONE_ADVANCED
#define HI_HNR_REF_MODE_BUTT OT_HNR_REF_MODE_BUTT
typedef ot_hnr_ref_mode hi_hnr_ref_mode;
typedef ot_hnr_err_code hi_hnr_err_code; /* inner errno */
typedef ot_hnr_thread_attr hi_hnr_thread_attr;

#ifdef __cplusplus
}
#endif

#endif /* __HI_COMMON_HNR_H__ */
