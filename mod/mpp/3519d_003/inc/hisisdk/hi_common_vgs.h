/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */

#ifndef HI_COMMON_VGS_H
#define HI_COMMON_VGS_H

#include "hi_type.h"
#include "hi_errno.h"
#include "hi_common_video.h"
#include "ot_common_vgs.h"

#ifdef __cplusplus
extern "C" {
#endif

#define HI_ERR_VGS_NO_BUF OT_ERR_VGS_NO_BUF
#define HI_ERR_VGS_BUF_EMPTY OT_ERR_VGS_BUF_EMPTY
#define HI_ERR_VGS_NULL_PTR OT_ERR_VGS_NULL_PTR
#define HI_ERR_VGS_ILLEGAL_PARAM OT_ERR_VGS_ILLEGAL_PARAM
#define HI_ERR_VGS_BUF_FULL OT_ERR_VGS_BUF_FULL
#define HI_ERR_VGS_NOT_READY OT_ERR_VGS_NOT_READY
#define HI_ERR_VGS_NOT_SUPPORT OT_ERR_VGS_NOT_SUPPORT
#define HI_ERR_VGS_NOT_PERM OT_ERR_VGS_NOT_PERM
#define HI_VGS_TASK_PRIVATE_DATA_LEN OT_VGS_TASK_PRIVATE_DATA_LEN
#define HI_VGS_CLUT_LEN OT_VGS_CLUT_LEN
#define HI_VGS_STITCH_NUM OT_VGS_STITCH_NUM
typedef ot_vgs_handle hi_vgs_handle;
typedef ot_vgs_task_attr hi_vgs_task_attr;
typedef ot_vgs_stitch_task_attr hi_vgs_stitch_task_attr;
typedef ot_vgs_line hi_vgs_line;
#define HI_VGS_OSD_INVERTED_COLOR_NONE OT_VGS_OSD_INVERTED_COLOR_NONE
#define HI_VGS_OSD_INVERTED_COLOR_RGB OT_VGS_OSD_INVERTED_COLOR_RGB
#define HI_VGS_OSD_INVERTED_COLOR_ALPHA OT_VGS_OSD_INVERTED_COLOR_ALPHA
#define HI_VGS_OSD_INVERTED_COLOR_ALL OT_VGS_OSD_INVERTED_COLOR_ALL
#define HI_VGS_OSD_INVERTED_COLOR_BUTT OT_VGS_OSD_INVERTED_COLOR_BUTT
typedef ot_vgs_osd_inverted_color hi_vgs_osd_inverted_color;
typedef ot_vgs_osd hi_vgs_osd;
typedef ot_vgs_aspect_ratio hi_vgs_aspect_ratio;
typedef ot_vgs_online hi_vgs_online;
#define HI_VGS_SCALE_COEF_NORM OT_VGS_SCALE_COEF_NORM
#define HI_VGS_SCALE_COEF_TAP2 OT_VGS_SCALE_COEF_TAP2
#define HI_VGS_SCALE_COEF_TAP4 OT_VGS_SCALE_COEF_TAP4
#define HI_VGS_SCALE_COEF_TAP6 OT_VGS_SCALE_COEF_TAP6
#define HI_VGS_SCALE_COEF_TAP8 OT_VGS_SCALE_COEF_TAP8
#define HI_VGS_SCALE_COEF_BUTT OT_VGS_SCALE_COEF_BUTT
typedef ot_vgs_scale_coef_mode hi_vgs_scale_coef_mode;

#ifdef __cplusplus
}
#endif
#endif /* HI_COMMON_VGS_H */
