/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */

#ifndef HI_COMMON_AENC_H
#define HI_COMMON_AENC_H

#include "hi_type.h"
#include "hi_common.h"
#include "hi_common_aio.h"
#include "ot_common_aenc.h"

#ifdef __cplusplus
extern "C" {
#endif

#define HI_MAX_ENCODER_NAME_LEN OT_MAX_ENCODER_NAME_LEN
typedef ot_aenc_attr_g711 hi_aenc_attr_g711;
typedef ot_aenc_attr_g726 hi_aenc_attr_g726;
typedef ot_aenc_attr_adpcm hi_aenc_attr_adpcm;
typedef ot_aenc_attr_lpcm hi_aenc_attr_lpcm;
typedef ot_aenc_buf_info hi_aenc_buf_info;
typedef ot_aenc_encoder hi_aenc_encoder;
typedef ot_aenc_chn_attr hi_aenc_chn_attr;
#define HI_AENC_ERR_ENCODER_ERR OT_AENC_ERR_ENCODER_ERR
#define HI_AENC_ERR_VQE_ERR OT_AENC_ERR_VQE_ERR
typedef ot_aenc_err_code hi_aenc_err_code;
#define HI_ERR_AENC_INVALID_DEV_ID OT_ERR_AENC_INVALID_DEV_ID
#define HI_ERR_AENC_INVALID_CHN_ID OT_ERR_AENC_INVALID_CHN_ID
#define HI_ERR_AENC_ILLEGAL_PARAM OT_ERR_AENC_ILLEGAL_PARAM
#define HI_ERR_AENC_EXIST OT_ERR_AENC_EXIST
#define HI_ERR_AENC_UNEXIST OT_ERR_AENC_UNEXIST
#define HI_ERR_AENC_NULL_PTR OT_ERR_AENC_NULL_PTR
#define HI_ERR_AENC_NOT_CFG OT_ERR_AENC_NOT_CFG
#define HI_ERR_AENC_NOT_SUPPORT OT_ERR_AENC_NOT_SUPPORT
#define HI_ERR_AENC_NOT_PERM OT_ERR_AENC_NOT_PERM
#define HI_ERR_AENC_NO_MEM OT_ERR_AENC_NO_MEM
#define HI_ERR_AENC_NO_BUF OT_ERR_AENC_NO_BUF
#define HI_ERR_AENC_BUF_EMPTY OT_ERR_AENC_BUF_EMPTY
#define HI_ERR_AENC_BUF_FULL OT_ERR_AENC_BUF_FULL
#define HI_ERR_AENC_NOT_READY OT_ERR_AENC_NOT_READY
#define HI_ERR_AENC_ENCODER_ERR OT_ERR_AENC_ENCODER_ERR
#define HI_ERR_AENC_VQE_ERR OT_ERR_AENC_VQE_ERR

#ifdef __cplusplus
}
#endif
#endif /* HI_COMMON_AENC_H */
