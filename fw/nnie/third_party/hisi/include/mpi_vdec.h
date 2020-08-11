/*
* Copyright (c) Hisilicon Technologies Co., Ltd. 2012-2019. All rights reserved.
* Description:mpi_vdec.c header file
* Author: Hisilicon multimedia software group
* Create: 2016/07/15
*/

#ifndef __MPI_VDEC_H__
#define __MPI_VDEC_H__

#include "hi_common.h"
#include "hi_comm_video.h"
#include "hi_comm_vb.h"
#include "hi_comm_vdec.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */

HI_S32 HI_MPI_VDEC_CreateChn(VDEC_CHN VdChn, const VDEC_CHN_ATTR_S *pstAttr);
HI_S32 HI_MPI_VDEC_DestroyChn(VDEC_CHN VdChn);

HI_S32 HI_MPI_VDEC_GetChnAttr(VDEC_CHN VdChn, VDEC_CHN_ATTR_S *pstAttr);
HI_S32 HI_MPI_VDEC_SetChnAttr(VDEC_CHN VdChn, const VDEC_CHN_ATTR_S *pstAttr);

HI_S32 HI_MPI_VDEC_StartRecvStream(VDEC_CHN VdChn);
HI_S32 HI_MPI_VDEC_StopRecvStream(VDEC_CHN VdChn);

HI_S32 HI_MPI_VDEC_QueryStatus(VDEC_CHN VdChn, VDEC_CHN_STATUS_S *pstStatus);

HI_S32 HI_MPI_VDEC_GetFd(VDEC_CHN VdChn);
HI_S32 HI_MPI_VDEC_CloseFd(VDEC_CHN VdChn);

HI_S32 HI_MPI_VDEC_ResetChn(VDEC_CHN VdChn);

HI_S32 HI_MPI_VDEC_SetChnParam(VDEC_CHN VdChn, const VDEC_CHN_PARAM_S *pstParam);
HI_S32 HI_MPI_VDEC_GetChnParam(VDEC_CHN VdChn, VDEC_CHN_PARAM_S *pstParam);

HI_S32 HI_MPI_VDEC_SetProtocolParam(VDEC_CHN VdChn, const VDEC_PRTCL_PARAM_S *pstParam);
HI_S32 HI_MPI_VDEC_GetProtocolParam(VDEC_CHN VdChn, VDEC_PRTCL_PARAM_S *pstParam);

/* s32MilliSec: -1 is block,0 is no block,other positive number is timeout */
HI_S32 HI_MPI_VDEC_SendStream(VDEC_CHN VdChn, const VDEC_STREAM_S *pstStream, HI_S32 s32MilliSec);

HI_S32 HI_MPI_VDEC_GetFrame(VDEC_CHN VdChn, VIDEO_FRAME_INFO_S *pstFrameInfo, HI_S32 s32MilliSec);
HI_S32 HI_MPI_VDEC_ReleaseFrame(VDEC_CHN VdChn, const VIDEO_FRAME_INFO_S *pstFrameInfo);

HI_S32 HI_MPI_VDEC_GetUserData(VDEC_CHN VdChn, VDEC_USERDATA_S *pstUserData, HI_S32 s32MilliSec);
HI_S32 HI_MPI_VDEC_ReleaseUserData(VDEC_CHN VdChn, const VDEC_USERDATA_S *pstUserData);

HI_S32 HI_MPI_VDEC_SetUserPic(VDEC_CHN VdChn, const VIDEO_FRAME_INFO_S *pstUsrPic);
HI_S32 HI_MPI_VDEC_EnableUserPic(VDEC_CHN VdChn, HI_BOOL bInstant);
HI_S32 HI_MPI_VDEC_DisableUserPic(VDEC_CHN VdChn);

HI_S32 HI_MPI_VDEC_SetDisplayMode(VDEC_CHN VdChn, VIDEO_DISPLAY_MODE_E enDisplayMode);
HI_S32 HI_MPI_VDEC_GetDisplayMode(VDEC_CHN VdChn, VIDEO_DISPLAY_MODE_E *penDisplayMode);

HI_S32 HI_MPI_VDEC_SetRotation(VDEC_CHN VdChn, ROTATION_E enRotation);
HI_S32 HI_MPI_VDEC_GetRotation(VDEC_CHN VdChn, ROTATION_E *penRotation);

HI_S32 HI_MPI_VDEC_AttachVbPool(VDEC_CHN VdChn, const VDEC_CHN_POOL_S *pstPool);
HI_S32 HI_MPI_VDEC_DetachVbPool(VDEC_CHN VdChn);

HI_S32 HI_MPI_VDEC_SetUserDataAttr(VDEC_CHN VdChn, const VDEC_USER_DATA_ATTR_S *pstUserDataAttr);
HI_S32 HI_MPI_VDEC_GetUserDataAttr(VDEC_CHN VdChn, VDEC_USER_DATA_ATTR_S *pstUserDataAttr);

HI_S32 HI_MPI_VDEC_SetModParam(const VDEC_MOD_PARAM_S *pstModParam);
HI_S32 HI_MPI_VDEC_GetModParam(VDEC_MOD_PARAM_S *pstModParam);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif /* End of #ifndef  __MPI_VDEC_H__ */


