/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */

#ifndef __HI_COMMON_GDC_H__
#define __HI_COMMON_GDC_H__

#include "hi_type.h"
#include "hi_common.h"
#include "hi_errno.h"
#include "hi_common_video.h"
#include "ot_common_gdc.h"


#ifdef __cplusplus
extern "C" {
#endif

#define HI_ERR_GDC_NO_BUF OT_ERR_GDC_NO_BUF
#define HI_ERR_GDC_BUF_EMPTY OT_ERR_GDC_BUF_EMPTY
#define HI_ERR_GDC_NULL_PTR OT_ERR_GDC_NULL_PTR
#define HI_ERR_GDC_ILLEGAL_PARAM OT_ERR_GDC_ILLEGAL_PARAM
#define HI_ERR_GDC_BUF_FULL OT_ERR_GDC_BUF_FULL
#define HI_ERR_GDC_NOT_READY OT_ERR_GDC_NOT_READY
#define HI_ERR_GDC_NOT_SUPPORT OT_ERR_GDC_NOT_SUPPORT
#define HI_ERR_GDC_NOT_PERM OT_ERR_GDC_NOT_PERM
#define HI_ERR_GDC_BUSY OT_ERR_GDC_BUSY
#define HI_ERR_GDC_INVALID_CHN_ID OT_ERR_GDC_INVALID_CHN_ID
#define HI_ERR_GDC_CHN_UNEXIST OT_ERR_GDC_CHN_UNEXIST
#define HI_GDC_PRIVATE_DATA_NUM OT_GDC_PRIVATE_DATA_NUM
#define HI_GDC_PMF_COEF_NUM OT_GDC_PMF_COEF_NUM
#define HI_FISHEYE_MAX_RGN_NUM OT_FISHEYE_MAX_RGN_NUM
#define HI_FISHEYE_LMF_COEF_NUM OT_FISHEYE_LMF_COEF_NUM
typedef ot_gdc_handle hi_gdc_handle;
typedef ot_gdc_task_attr hi_gdc_task_attr;
#define HI_FISHEYE_MOUNT_MODE_DESKTOP OT_FISHEYE_MOUNT_MODE_DESKTOP
#define HI_FISHEYE_MOUNT_MODE_CEILING OT_FISHEYE_MOUNT_MODE_CEILING
#define HI_FISHEYE_MOUNT_MODE_WALL OT_FISHEYE_MOUNT_MODE_WALL
#define HI_FISHEYE_MOUNT_MODE_BUTT OT_FISHEYE_MOUNT_MODE_BUTT
typedef ot_fisheye_mount_mode hi_fisheye_mount_mode;
#define HI_FISHEYE_VIEW_MODE_360_PANORAMA OT_FISHEYE_VIEW_MODE_360_PANORAMA
#define HI_FISHEYE_VIEW_MODE_180_PANORAMA OT_FISHEYE_VIEW_MODE_180_PANORAMA
#define HI_FISHEYE_VIEW_MODE_NORM OT_FISHEYE_VIEW_MODE_NORM
#define HI_FISHEYE_VIEW_MODE_NO_TRANS OT_FISHEYE_VIEW_MODE_NO_TRANS
#define HI_FISHEYE_VIEW_MODE_BUTT OT_FISHEYE_VIEW_MODE_BUTT
typedef ot_fisheye_view_mode hi_fisheye_view_mode;
typedef ot_fisheye_rgn_attr hi_fisheye_rgn_attr;
typedef ot_fisheye_rgn_attr_ex hi_fisheye_rgn_attr_ex;
typedef ot_fisheye_attr hi_fisheye_attr;
typedef ot_fisheye_attr_ex hi_fisheye_attr_ex;
typedef ot_fisheye_correction_attr hi_fisheye_correction_attr;
typedef ot_fisheye_cfg hi_fisheye_cfg;
typedef ot_gdc_fisheye_job_cfg hi_gdc_fisheye_job_cfg;
typedef ot_gdc_pmf_attr hi_gdc_pmf_attr;
typedef ot_gdc_fisheye_point_query_attr hi_gdc_fisheye_point_query_attr;

#ifdef __cplusplus
}
#endif
#endif /* __HI_COMMON_GDC_H__ */
