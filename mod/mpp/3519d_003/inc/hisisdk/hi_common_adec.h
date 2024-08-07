/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */

#ifndef HI_COMMON_ADEC_H
#define HI_COMMON_ADEC_H

#include "hi_type.h"
#include "hi_common.h"
#include "hi_common_aio.h"
#include "ot_common_adec.h"

#ifdef __cplusplus
extern "C" {
#endif

#define HI_MAX_DECODER_NAME_LEN OT_MAX_DECODER_NAME_LEN
typedef ot_adec_attr_g711 hi_adec_attr_g711;
typedef ot_adec_attr_g726 hi_adec_attr_g726;
typedef ot_adec_attr_adpcm hi_adec_attr_adpcm;
typedef ot_adec_attr_lpcm hi_adec_attr_lpcm;
#define HI_ADEC_MODE_PACK OT_ADEC_MODE_PACK
#define HI_ADEC_MODE_STREAM OT_ADEC_MODE_STREAM
#define HI_ADEC_MODE_BUTT OT_ADEC_MODE_BUTT
typedef ot_adec_mode hi_adec_mode;
typedef ot_adec_chn_attr hi_adec_chn_attr;
typedef ot_adec_chn_state hi_adec_chn_state;
typedef ot_adec_buf_info hi_adec_buf_info;
typedef ot_adec_decoder hi_adec_decoder;
#define HI_ADEC_ERR_DECODER_ERR OT_ADEC_ERR_DECODER_ERR
#define HI_ADEC_ERR_BUF_LACK OT_ADEC_ERR_BUF_LACK
typedef ot_adec_err_code hi_adec_err_code;
#define HI_ERR_ADEC_INVALID_DEV_ID OT_ERR_ADEC_INVALID_DEV_ID
#define HI_ERR_ADEC_INVALID_CHN_ID OT_ERR_ADEC_INVALID_CHN_ID
#define HI_ERR_ADEC_ILLEGAL_PARAM OT_ERR_ADEC_ILLEGAL_PARAM
#define HI_ERR_ADEC_EXIST OT_ERR_ADEC_EXIST
#define HI_ERR_ADEC_UNEXIST OT_ERR_ADEC_UNEXIST
#define HI_ERR_ADEC_NULL_PTR OT_ERR_ADEC_NULL_PTR
#define HI_ERR_ADEC_NOT_CFG OT_ERR_ADEC_NOT_CFG
#define HI_ERR_ADEC_NOT_SUPPORT OT_ERR_ADEC_NOT_SUPPORT
#define HI_ERR_ADEC_NOT_PERM OT_ERR_ADEC_NOT_PERM
#define HI_ERR_ADEC_NO_MEM OT_ERR_ADEC_NO_MEM
#define HI_ERR_ADEC_NO_BUF OT_ERR_ADEC_NO_BUF
#define HI_ERR_ADEC_BUF_EMPTY OT_ERR_ADEC_BUF_EMPTY
#define HI_ERR_ADEC_BUF_FULL OT_ERR_ADEC_BUF_FULL
#define HI_ERR_ADEC_NOT_READY OT_ERR_ADEC_NOT_READY
#define HI_ERR_ADEC_DECODER_ERR OT_ERR_ADEC_DECODER_ERR
#define HI_ERR_ADEC_BUF_LACK OT_ERR_ADEC_BUF_LACK

#ifdef __cplusplus
}
#endif
#endif /* HI_COMMON_ADEC_H */
