/******************************************************************************

  Copyright (C), 2017, Hisilicon Tech. Co., Ltd.

 ******************************************************************************
  File Name     : mpi_vi.h
  Version       : Initial Draft
  Author        : Hisilicon multimedia software group
  Created       : 2017/06/16
  Description   :
  History       :


******************************************************************************/
#ifndef __MPI_SNAP_H__
#define __MPI_SNAP_H__

#include "hi_comm_video.h"
#include "hi_comm_snap.h"

#ifdef __cplusplus
#if __cplusplus
extern "C"
{
#endif
#endif /* __cplusplus */


HI_S32 HI_MPI_SNAP_SetPipeAttr(VI_PIPE ViPipe, const SNAP_ATTR_S *pstSnapAttr);
HI_S32 HI_MPI_SNAP_GetPipeAttr(VI_PIPE ViPipe, SNAP_ATTR_S *pstSnapAttr);
HI_S32 HI_MPI_SNAP_EnablePipe(VI_PIPE ViPipe);
HI_S32 HI_MPI_SNAP_DisablePipe(VI_PIPE ViPipe);
HI_S32 HI_MPI_SNAP_TriggerPipe(VI_PIPE ViPipe);

HI_S32 HI_MPI_SNAP_MultiTrigger(VI_STITCH_GRP StitchGrp);

/* HDR */
HI_S32 HI_MPI_SNAP_SetProSharpenParam(VI_PIPE ViPipe, const ISP_PRO_SHARPEN_PARAM_S *pstIspShpParam);
HI_S32 HI_MPI_SNAP_GetProSharpenParam(VI_PIPE ViPipe, ISP_PRO_SHARPEN_PARAM_S *pstIspShpParam);
HI_S32 HI_MPI_SNAP_SetProBNRParam(VI_PIPE ViPipe, const ISP_PRO_BNR_PARAM_S *pstNrParma);
HI_S32 HI_MPI_SNAP_GetProBNRParam(VI_PIPE ViPipe, ISP_PRO_BNR_PARAM_S *pstNrParma);


HI_S32 HI_MPI_SNAP_SetBNRRawDumpAttr(VI_PIPE ViPipe, const BNR_DUMP_ATTR_S* pstBnrDumpAttr);
HI_S32 HI_MPI_SNAP_GetBNRRawDumpAttr(VI_PIPE ViPipe, BNR_DUMP_ATTR_S* pstBnrDumpAttr);

HI_S32 HI_MPI_SNAP_GetBNRRaw(VI_PIPE ViPipe, VIDEO_FRAME_INFO_S *pstVideoFrame, HI_S32 s32MilliSec);
HI_S32 HI_MPI_SNAP_ReleaseBNRRaw(VI_PIPE ViPipe, const VIDEO_FRAME_INFO_S *pstVideoFrame);


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /*__MPI_SNAP_H__ */


