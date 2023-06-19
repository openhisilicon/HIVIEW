/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */

#ifndef __HI_COMM_UVC_H__
#define __HI_COMM_UVC_H__

#include "hi_common.h"
#include "hi_common_video.h"
#include "ot_common_uvc.h"

#ifdef __cplusplus
extern "C" {
#endif

#define HI_ERR_UVC_NULL_PTR OT_ERR_UVC_NULL_PTR
#define HI_ERR_UVC_NOTREADY OT_ERR_UVC_NOTREADY
#define HI_ERR_UVC_INVALID_CHNID OT_ERR_UVC_INVALID_CHNID
#define HI_ERR_UVC_EXIST OT_ERR_UVC_EXIST
#define HI_ERR_UVC_UNEXIST OT_ERR_UVC_UNEXIST
#define HI_ERR_UVC_NOT_SUPPORT OT_ERR_UVC_NOT_SUPPORT
#define HI_ERR_UVC_NOT_PERM OT_ERR_UVC_NOT_PERM
#define HI_ERR_UVC_NOMEM OT_ERR_UVC_NOMEM
#define HI_ERR_UVC_NOBUF OT_ERR_UVC_NOBUF
#define HI_ERR_UVC_ILLEGAL_PARAM OT_ERR_UVC_ILLEGAL_PARAM
#define HI_ERR_UVC_BUSY OT_ERR_UVC_BUSY
#define HI_ERR_UVC_BUF_EMPTY OT_ERR_UVC_BUF_EMPTY

#define HI_UVC_FORMAT_YUYV OT_UVC_FORMAT_YUYV
#define HI_UVC_FORMAT_NV21 OT_UVC_FORMAT_NV21
#define HI_UVC_FORMAT_NV12 OT_UVC_FORMAT_NV12
#define HI_UVC_FORMAT_MJPEG OT_UVC_FORMAT_MJPEG
#define HI_UVC_FORMAT_H264 OT_UVC_FORMAT_H264
#define HI_UVC_FORMAT_H265 OT_UVC_FORMAT_H265
#define HI_UVC_FORMAT_BUTT OT_UVC_FORMAT_BUTT

typedef ot_uvc_format hi_uvc_format;
typedef ot_uvc_chn_attr hi_uvc_chn_attr;

#ifdef __cplusplus
}
#endif

#endif /* end of #ifndef __HI_COMM_UVC_H__ */
