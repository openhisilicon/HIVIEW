/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */

#ifndef __HI_COMMON_REGION_H__
#define __HI_COMMON_REGION_H__

#include "hi_common.h"
#include "hi_common_video.h"
#include "hi_errno.h"
#include "hi_defines.h"
#include "ot_common_region.h"

#ifdef __cplusplus
extern "C" {
#endif

#define HI_RGN_MAX_BMP_UPDATE_NUM OT_RGN_MAX_BMP_UPDATE_NUM
#define HI_RGN_BATCH_HANDLE_MAX OT_RGN_BATCH_HANDLE_MAX
#define HI_RGN_QUAD_POINT_NUM OT_RGN_QUAD_POINT_NUM
#define HI_RGN_LINE_POINT_NUM OT_RGN_LINE_POINT_NUM
#define HI_RGN_CLUT_NUM OT_RGN_CLUT_NUM
typedef ot_rgn_handle hi_rgn_handle;
typedef ot_rgn_handle_grp hi_rgn_handle_grp;
#define HI_RGN_OVERLAY OT_RGN_OVERLAY
#define HI_RGN_COVER OT_RGN_COVER
#define HI_RGN_OVERLAYEX OT_RGN_OVERLAYEX
#define HI_RGN_COVEREX OT_RGN_COVEREX
#define HI_RGN_LINE OT_RGN_LINE
#define HI_RGN_MOSAIC OT_RGN_MOSAIC
#define HI_RGN_MOSAICEX OT_RGN_MOSAICEX
#define HI_RGN_CORNER_RECTEX OT_RGN_CORNER_RECTEX
#define HI_RGN_BUTT OT_RGN_BUTT
typedef ot_rgn_type hi_rgn_type;
#define HI_RGN_ATTACH_JPEG_MAIN OT_RGN_ATTACH_JPEG_MAIN
#define HI_RGN_ATTACH_JPEG_MPF0 OT_RGN_ATTACH_JPEG_MPF0
#define HI_RGN_ATTACH_JPEG_MPF1 OT_RGN_ATTACH_JPEG_MPF1
#define HI_RGN_ATTACH_JPEG_BUTT OT_RGN_ATTACH_JPEG_BUTT
typedef ot_rgn_attach_dst hi_rgn_attach_dst;
typedef ot_rgn_overlay_qp_info hi_rgn_overlay_qp_info;
typedef ot_rgn_overlay_attr hi_rgn_overlay_attr;
typedef ot_rgn_overlay_chn_attr hi_rgn_overlay_chn_attr;
typedef ot_rgn_overlayex_attr hi_rgn_overlayex_attr;
typedef ot_rgn_overlayex_chn_attr hi_rgn_overlayex_chn_attr;
typedef ot_rgn_cover_chn_attr hi_rgn_cover_chn_attr;
typedef ot_rgn_coverex_chn_attr hi_rgn_coverex_chn_attr;
typedef ot_rgn_mosaic_chn_attr hi_rgn_mosaic_chn_attr;
typedef ot_rgn_mosaicex_chn_attr hi_rgn_mosaicex_chn_attr;
typedef ot_rgn_type_attr hi_rgn_type_attr;
typedef ot_rgn_line_chn_attr hi_rgn_line_chn_attr;
typedef ot_rgn_corner_rect_chn_attr hi_rgn_corner_rect_chn_attr;
typedef ot_rgn_type_chn_attr hi_rgn_type_chn_attr;
typedef ot_rgn_attr hi_rgn_attr;
typedef ot_rgn_chn_attr hi_rgn_chn_attr;
typedef ot_rgn_canvas_info hi_rgn_canvas_info;
#define HI_ERR_RGN_INVALID_DEV_ID OT_ERR_RGN_INVALID_DEV_ID
#define HI_ERR_RGN_INVALID_CHN_ID OT_ERR_RGN_INVALID_CHN_ID
#define HI_ERR_RGN_ILLEGAL_PARAM OT_ERR_RGN_ILLEGAL_PARAM
#define HI_ERR_RGN_EXIST OT_ERR_RGN_EXIST
#define HI_ERR_RGN_UNEXIST OT_ERR_RGN_UNEXIST
#define HI_ERR_RGN_NULL_PTR OT_ERR_RGN_NULL_PTR
#define HI_ERR_RGN_NOT_CFG OT_ERR_RGN_NOT_CFG
#define HI_ERR_RGN_NOT_SUPPORT OT_ERR_RGN_NOT_SUPPORT
#define HI_ERR_RGN_NOT_PERM OT_ERR_RGN_NOT_PERM
#define HI_ERR_RGN_NO_MEM OT_ERR_RGN_NO_MEM
#define HI_ERR_RGN_NO_BUF OT_ERR_RGN_NO_BUF
#define HI_ERR_RGN_BUF_EMPTY OT_ERR_RGN_BUF_EMPTY
#define HI_ERR_RGN_BUF_FULL OT_ERR_RGN_BUF_FULL
#define HI_ERR_RGN_BAD_ADDR OT_ERR_RGN_BAD_ADDR
#define HI_ERR_RGN_BUSY OT_ERR_RGN_BUSY
#define HI_ERR_RGN_NOT_READY OT_ERR_RGN_NOT_READY

#ifdef __cplusplus
}
#endif
#endif /* __HI_COMMON_REGION_H__ */
