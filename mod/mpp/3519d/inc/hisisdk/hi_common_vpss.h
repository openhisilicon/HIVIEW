/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */

#ifndef HI_COMMON_VPSS_H
#define HI_COMMON_VPSS_H

#include "hi_type.h"
#include "hi_common.h"
#include "hi_errno.h"
#include "hi_common_video.h"
#include "hi_common_sys.h"
#include "ot_common_vpss.h"

#ifdef __cplusplus
extern "C" {
#endif

#define HI_ERR_VPSS_NULL_PTR OT_ERR_VPSS_NULL_PTR
#define HI_ERR_VPSS_NOT_READY OT_ERR_VPSS_NOT_READY
#define HI_ERR_VPSS_INVALID_DEV_ID OT_ERR_VPSS_INVALID_DEV_ID
#define HI_ERR_VPSS_INVALID_CHN_ID OT_ERR_VPSS_INVALID_CHN_ID
#define HI_ERR_VPSS_EXIST OT_ERR_VPSS_EXIST
#define HI_ERR_VPSS_UNEXIST OT_ERR_VPSS_UNEXIST
#define HI_ERR_VPSS_NOT_SUPPORT OT_ERR_VPSS_NOT_SUPPORT
#define HI_ERR_VPSS_NOT_PERM OT_ERR_VPSS_NOT_PERM
#define HI_ERR_VPSS_NO_MEM OT_ERR_VPSS_NO_MEM
#define HI_ERR_VPSS_NO_BUF OT_ERR_VPSS_NO_BUF
#define HI_ERR_VPSS_SIZE_NOT_ENOUGH OT_ERR_VPSS_SIZE_NOT_ENOUGH
#define HI_ERR_VPSS_ILLEGAL_PARAM OT_ERR_VPSS_ILLEGAL_PARAM
#define HI_ERR_VPSS_BUSY OT_ERR_VPSS_BUSY
#define HI_ERR_VPSS_BUF_EMPTY OT_ERR_VPSS_BUF_EMPTY
#define HI_VPSS_CHN0 OT_VPSS_CHN0
#define HI_VPSS_CHN1 OT_VPSS_CHN1
#define HI_VPSS_CHN2 OT_VPSS_CHN2
#define HI_VPSS_CHN3 OT_VPSS_CHN3
#define HI_VPSS_DIRECT_CHN OT_VPSS_DIRECT_CHN
#define HI_VPSS_INVALID_CHN OT_VPSS_INVALID_CHN
#define HI_VPSS_DEI_MODE_OFF OT_VPSS_DEI_MODE_OFF
#define HI_VPSS_DEI_MODE_ON OT_VPSS_DEI_MODE_ON
#define HI_VPSS_DEI_MODE_AUTO OT_VPSS_DEI_MODE_AUTO
#define HI_VPSS_DEI_MODE_BUTT OT_VPSS_DEI_MODE_BUTT
typedef ot_vpss_dei_mode hi_vpss_dei_mode;
typedef ot_vpss_grp_attr hi_vpss_grp_attr;
#define HI_VPSS_CHN_MODE_AUTO OT_VPSS_CHN_MODE_AUTO
#define HI_VPSS_CHN_MODE_USER OT_VPSS_CHN_MODE_USER
#define HI_VPSS_CHN_MODE_BUTT OT_VPSS_CHN_MODE_BUTT
typedef ot_vpss_chn_mode hi_vpss_chn_mode;
typedef ot_vpss_chn_attr hi_vpss_chn_attr;
typedef ot_vpss_ext_chn_attr hi_vpss_ext_chn_attr;
typedef ot_vpss_crop_info hi_vpss_crop_info;
typedef ot_vpss_grp_param hi_vpss_grp_param;
typedef ot_vpss_out_lut_cfg hi_vpss_out_lut_cfg;
typedef ot_vpss_grp_cfg hi_vpss_grp_cfg;
typedef ot_vpss_rgn_info hi_vpss_rgn_info;
typedef ot_vpss_mod_param hi_vpss_mod_param;

#ifdef __cplusplus
}
#endif
#endif /* HI_COMMON_VPSS_H */
