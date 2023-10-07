/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */

#ifndef __HI_MPI_UVC_H__
#define __HI_MPI_UVC_H__

#include "hi_common.h"
#include "hi_common_uvc.h"
#include "hi_common_venc.h"

#ifdef __cplusplus
extern "C" {
#endif

hi_s32 hi_mpi_uvc_create_chn(hi_uvc_chn uvc_chn, const hi_uvc_chn_attr *chn_attr);
hi_s32 hi_mpi_uvc_destroy_chn(hi_uvc_chn uvc_chn);

hi_s32 hi_mpi_uvc_start_chn(hi_uvc_chn uvc_chn);
hi_s32 hi_mpi_uvc_stop_chn(hi_uvc_chn uvc_chn);

hi_s32 hi_mpi_uvc_get_chn_attr(hi_uvc_chn uvc_chn, hi_uvc_chn_attr *chn_attr);
hi_s32 hi_mpi_uvc_send_stream(hi_uvc_chn uvc_chn, hi_venc_chn venc_chn, const hi_venc_stream *stream, hi_s32 milli_sec);
hi_s32 hi_mpi_uvc_send_user_stream(hi_uvc_chn uvc_chn, const hi_uvc_user_stream *user_stream, hi_s32 milli_sec);

#ifdef __cplusplus
}
#endif

#endif /* __HI_MPI_UVC_H__ */
