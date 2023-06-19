/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */

#ifndef __HI_COMMON_SNAP_H__
#define __HI_COMMON_SNAP_H__

#include "hi_common.h"
#include "hi_common_video.h"
#include "hi_common_isp.h"
#include "ot_common_snap.h"


#ifdef __cplusplus
extern "C" {
#endif

#define HI_ERR_SNAP_INVALID_PIPE_ID OT_ERR_SNAP_INVALID_PIPE_ID
#define HI_ERR_SNAP_ILLEGAL_PARAM OT_ERR_SNAP_ILLEGAL_PARAM
#define HI_ERR_SNAP_NULL_PTR OT_ERR_SNAP_NULL_PTR
#define HI_ERR_SNAP_NOT_SUPPORT OT_ERR_SNAP_NOT_SUPPORT
#define HI_ERR_SNAP_NOT_PERM OT_ERR_SNAP_NOT_PERM
#define HI_ERR_SNAP_NO_MEM OT_ERR_SNAP_NO_MEM
#define HI_ERR_SNAP_NOT_READY OT_ERR_SNAP_NOT_READY
#define HI_SNAP_TYPE_NORM OT_SNAP_TYPE_NORM
#define HI_SNAP_TYPE_PRO OT_SNAP_TYPE_PRO
#define HI_SNAP_TYPE_BUTT OT_SNAP_TYPE_BUTT
typedef ot_snap_type hi_snap_type;
typedef ot_snap_norm_attr hi_snap_norm_attr;
typedef ot_snap_pro_manual_param hi_snap_pro_manual_param;
typedef ot_snap_pro_auto_param hi_snap_pro_auto_param;
typedef ot_snap_pro_param hi_snap_pro_param;
typedef ot_snap_pro_attr hi_snap_pro_attr;
typedef ot_snap_attr hi_snap_attr;

#ifdef __cplusplus
}
#endif
#endif /* __HI_COMMON_SNAP_H__ */
