/*
  Copyright (c), 2001-2024, Shenshu Tech. Co., Ltd.
 */

#ifndef HI_COMMON_PSFM_H
#define HI_COMMON_PSFM_H

#include "hi_common.h"
#include "hi_common_video.h"
#include "ot_common_psfm.h"

#ifdef __cplusplus
extern "C" {
#endif

#define HI_ERR_PSFM_INVALID_GRP_ID  OT_ERR_PSFM_INVALID_GRP_ID
#define HI_ERR_PSFM_INVALID_CHN_ID  OT_ERR_PSFM_INVALID_CHN_ID
#define HI_ERR_PSFM_ILLEGAL_PARAM   OT_ERR_PSFM_ILLEGAL_PARAM
#define HI_ERR_PSFM_EXIST           OT_ERR_PSFM_EXIST
#define HI_ERR_PSFM_UNEXIST         OT_ERR_PSFM_UNEXIST
#define HI_ERR_PSFM_NULL_PTR        OT_ERR_PSFM_NULL_PTR
#define HI_ERR_PSFM_NOT_CFG         OT_ERR_PSFM_NOT_CFG
#define HI_ERR_PSFM_NOT_SUPPORT     OT_ERR_PSFM_NOT_SUPPORT
#define HI_ERR_PSFM_NOT_START       OT_ERR_PSFM_NOT_START
#define HI_ERR_PSFM_NOT_STOP        OT_ERR_PSFM_NOT_STOP
#define HI_ERR_PSFM_NO_MEM          OT_ERR_PSFM_NO_MEM
#define HI_ERR_PSFM_BUF_EMPTY       OT_ERR_PSFM_BUF_EMPTY
#define HI_ERR_PSFM_BUF_FULL        OT_ERR_PSFM_BUF_FULL
#define HI_ERR_PSFM_NOT_READY       OT_ERR_PSFM_NOT_READY
#define HI_ERR_PSFM_TIMEOUT         OT_ERR_PSFM_TIMEOUT
#define HI_ERR_PSFM_BUSY            OT_ERR_PSFM_BUSY

#define HI_PSFM_MAX_GRP_NUM OT_PSFM_MAX_GRP_NUM
#define HI_PSFM_MAX_CHN_NUM OT_PSFM_MAX_CHN_NUM

#define HI_PSFM_MIN_CHN_WIDTH OT_PSFM_MIN_CHN_WIDTH
#define HI_PSFM_MIN_CHN_HEIGHT OT_PSFM_MIN_CHN_HEIGHT
#define HI_PSFM_MAX_CHN_WIDTH OT_PSFM_MAX_CHN_WIDTH
#define HI_PSFM_MAX_CHN_HEIGHT OT_PSFM_MAX_CHN_HEIGHT

typedef ot_psfm_grp hi_psfm_grp;
typedef ot_psfm_chn hi_psfm_chn;

typedef ot_psfm_ad_header_info hi_psfm_ad_header_info;

typedef ot_psfm_ad_private_info hi_psfm_ad_private_info;

typedef ot_psfm_grp_attr hi_psfm_grp_attr;

typedef ot_psfm_chn_attr hi_psfm_chn_attr;

typedef ot_psfm_frame_source_callback hi_psfm_frame_source_callback;

#ifdef __cplusplus
}
#endif
#endif /* HI_COMMON_PSFM_H */
