/*
  Copyright (c), 2001-2024, Shenshu Tech. Co., Ltd.
 */

#ifndef OT_COMMON_UVC_H
#define OT_COMMON_UVC_H

#include "ot_errno.h"
#include "ot_common.h"
#include "ot_common_video.h"

#ifdef __cplusplus
extern "C" {
#endif

#define OT_ERR_UVC_NULL_PTR         OT_DEFINE_ERR(OT_ID_UVC, OT_ERR_LEVEL_ERROR, OT_ERR_NULL_PTR)
#define OT_ERR_UVC_NOTREADY         OT_DEFINE_ERR(OT_ID_UVC, OT_ERR_LEVEL_ERROR, OT_ERR_NOT_READY)
#define OT_ERR_UVC_INVALID_CHNID    OT_DEFINE_ERR(OT_ID_UVC, OT_ERR_LEVEL_ERROR, OT_ERR_INVALID_CHN_ID)
#define OT_ERR_UVC_EXIST            OT_DEFINE_ERR(OT_ID_UVC, OT_ERR_LEVEL_ERROR, OT_ERR_EXIST)
#define OT_ERR_UVC_UNEXIST          OT_DEFINE_ERR(OT_ID_UVC, OT_ERR_LEVEL_ERROR, OT_ERR_UNEXIST)
#define OT_ERR_UVC_NOT_SUPPORT      OT_DEFINE_ERR(OT_ID_UVC, OT_ERR_LEVEL_ERROR, OT_ERR_NOT_SUPPORT)
#define OT_ERR_UVC_NOT_PERM         OT_DEFINE_ERR(OT_ID_UVC, OT_ERR_LEVEL_ERROR, OT_ERR_NOT_PERM)
#define OT_ERR_UVC_NOMEM            OT_DEFINE_ERR(OT_ID_UVC, OT_ERR_LEVEL_ERROR, OT_ERR_NO_MEM)
#define OT_ERR_UVC_NOBUF            OT_DEFINE_ERR(OT_ID_UVC, OT_ERR_LEVEL_ERROR, OT_ERR_NO_BUF)
#define OT_ERR_UVC_ILLEGAL_PARAM    OT_DEFINE_ERR(OT_ID_UVC, OT_ERR_LEVEL_ERROR, OT_ERR_ILLEGAL_PARAM)
#define OT_ERR_UVC_BUSY             OT_DEFINE_ERR(OT_ID_UVC, OT_ERR_LEVEL_ERROR, OT_ERR_BUSY)
#define OT_ERR_UVC_BUF_EMPTY        OT_DEFINE_ERR(OT_ID_UVC, OT_ERR_LEVEL_ERROR, OT_ERR_BUF_EMPTY)

typedef enum {
    OT_UVC_FORMAT_YUYV = 0,
    OT_UVC_FORMAT_NV21,
    OT_UVC_FORMAT_NV12,
    OT_UVC_FORMAT_MJPEG,
    OT_UVC_FORMAT_H264,
    OT_UVC_FORMAT_H265,
    OT_UVC_FORMAT_BUTT
} ot_uvc_format;

/* VENC stream data input mode.(YUV data always send by VPSS bind UVC) */
typedef enum {
    OT_UVC_MPP_BIND_UVC, /* zero-copy, venc bind uvc module */
    OT_UVC_SEND_VENC_STREAM, /* zero-copy, only the stream got from VENC can be send to UVC */
    OT_UVC_SEND_YUV_FRAME,   /* zero-copy, send yuv frame(ot_video_frame_info) to UVC */
    OT_UVC_SEND_USER_STREAM,   /* copy, support stream on user buffer */
    OT_UVC_DATA_INPUT_MODE_BUTT
} ot_uvc_data_input_mode;

typedef struct {
    ot_uvc_format   uvc_format;
    ot_uvc_data_input_mode data_mode;
    td_u32 buffer_size;  /* by KB, for ss_mpi_uvc_send_user_stream() only */
} ot_uvc_chn_attr;

typedef struct {
    td_u64 pts;
    td_void *addr;
    td_u32 len;
    td_bool is_frame_end;
} ot_uvc_user_stream;

#ifdef __cplusplus
}
#endif

#endif /* end of #ifndef OT_COMMON_UVC_H */