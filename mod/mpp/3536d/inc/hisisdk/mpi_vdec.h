/******************************************************************************

  Copyright (C), 2013, Hisilicon Tech. Co., Ltd.

 ******************************************************************************
  File Name     : mpi_vdec.h
  Version       : Initial Draft
  Author        : Hisilicon multimedia software group
  Created       : 2013/05/08
  Description   : MPP Programe Interface for video decode
  History       :
  1.Date        : 2013/05/08
    Author      : l00226816
    Modification: Created file
******************************************************************************/
#ifndef  __MPI_VDEC_H__
#define  __MPI_VDEC_H__

#include "hi_common.h"
#include "hi_comm_video.h"
#include "hi_comm_vb.h"
#include "hi_comm_vdec.h"

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* End of #ifdef __cplusplus */


HI_S32 HI_MPI_VDEC_CreateChn(VDEC_CHN VdChn, const VDEC_CHN_ATTR_S *pstAttr);
HI_S32 HI_MPI_VDEC_DestroyChn(VDEC_CHN VdChn);

HI_S32 HI_MPI_VDEC_GetChnAttr(VDEC_CHN VdChn, VDEC_CHN_ATTR_S *pstAttr);

HI_S32 HI_MPI_VDEC_StartRecvStream(VDEC_CHN VdChn);
HI_S32 HI_MPI_VDEC_StopRecvStream(VDEC_CHN VdChn);

HI_S32 HI_MPI_VDEC_Query(VDEC_CHN VdChn,VDEC_CHN_STAT_S *pstStat);

HI_S32 HI_MPI_VDEC_GetFd(VDEC_CHN VdChn);
HI_S32 HI_MPI_VDEC_CloseFd(VDEC_CHN VdChn);

HI_S32 HI_MPI_VDEC_ResetChn(VDEC_CHN VdChn);

HI_S32 HI_MPI_VDEC_SetChnParam(VDEC_CHN VdChn, VDEC_CHN_PARAM_S* pstParam);
HI_S32 HI_MPI_VDEC_GetChnParam(VDEC_CHN VdChn, VDEC_CHN_PARAM_S* pstParam);

HI_S32 HI_MPI_VDEC_SetProtocolParam(VDEC_CHN VdChn,VDEC_PRTCL_PARAM_S *pstParam);
HI_S32 HI_MPI_VDEC_GetProtocolParam(VDEC_CHN VdChn,VDEC_PRTCL_PARAM_S *pstParam);

/* s32MilliSec: -1 is block,0 is no block,other positive number is timeout */
HI_S32 HI_MPI_VDEC_SendStream(VDEC_CHN VdChn, const VDEC_STREAM_S *pstStream, HI_S32 s32MilliSec);
HI_S32 HI_MPI_VDEC_SendStreamEx(VDEC_CHN VdChn, const VDEC_STREAM_EX_S *pstStream, HI_S32 s32MilliSec);

HI_S32 HI_MPI_VDEC_GetImage(VDEC_CHN VdChn, VIDEO_FRAME_INFO_S *pstFrameInfo,HI_S32 s32MilliSec);
HI_S32 HI_MPI_VDEC_ReleaseImage(VDEC_CHN VdChn, VIDEO_FRAME_INFO_S *pstFrameInfo);

HI_S32 HI_MPI_VDEC_GetUserData(VDEC_CHN VdChn, VDEC_USERDATA_S *pstUserData, HI_S32 s32MilliSec);
HI_S32 HI_MPI_VDEC_ReleaseUserData(VDEC_CHN VdChn, VDEC_USERDATA_S *pstUserData);

HI_S32 HI_MPI_VDEC_SetRotate(VDEC_CHN VdChn, ROTATE_E enRotate);
HI_S32 HI_MPI_VDEC_GetRotate(VDEC_CHN VdChn, ROTATE_E *penRotate);

HI_S32 HI_MPI_VDEC_GetChnLuma(VDEC_CHN VdChn, VDEC_CHN_LUM_S *pstLuma);

HI_S32 HI_MPI_VDEC_SetUserPic(VDEC_CHN VdChn, VIDEO_FRAME_INFO_S *pstUsrPic);
HI_S32 HI_MPI_VDEC_EnableUserPic(VDEC_CHN VdChn, HI_BOOL bInstant);
HI_S32 HI_MPI_VDEC_DisableUserPic(VDEC_CHN VdChn);

HI_S32 HI_MPI_VDEC_SetDisplayMode(VDEC_CHN VdChn, VIDEO_DISPLAY_MODE_E enDisplayMode);
HI_S32 HI_MPI_VDEC_GetDisplayMode(VDEC_CHN VdChn, VIDEO_DISPLAY_MODE_E *penDisplayMode);

HI_S32 HI_MPI_VDEC_SetChnVBCnt(VDEC_CHN VdChn, HI_U32 u32BlkCnt);
HI_S32 HI_MPI_VDEC_GetChnVBCnt(VDEC_CHN VdChn, HI_U32 *pu32BlkCnt);

HI_S32 HI_MPI_VDEC_AttachVbPool(VDEC_CHN VdChn, VDEC_CHN_POOL_S *pstPool);
HI_S32 HI_MPI_VDEC_DetachVbPool(VDEC_CHN VdChn);

HI_S32 HI_MPI_VDEC_SetModParam(VDEC_MOD_PARAM_S *pstModParam);
HI_S32 HI_MPI_VDEC_GetModParam(VDEC_MOD_PARAM_S *pstModParam);


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif /* End of #ifndef  __MPI_VDEC_H__ */

