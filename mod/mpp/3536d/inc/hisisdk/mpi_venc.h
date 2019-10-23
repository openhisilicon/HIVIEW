/******************************************************************************

  Copyright (C), 2001-2012, Hisilicon Tech. Co., Ltd.

 ******************************************************************************
  File Name     : mpi_venc.h
  Version       : Initial Draft
  Author        : Hisilicon Hi35xx MPP Team
  Created       : 2006/11/22
  Last Modified :
  Description   : mpi functions declaration
  Function List :
  History       :
******************************************************************************/
#ifndef __MPI_VENC_H__
#define __MPI_VENC_H__

#include "hi_common.h"
#include "hi_comm_video.h"
#include "hi_comm_venc.h"
#include "hi_comm_vb.h"


#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

HI_S32 HI_MPI_VENC_CreateChn(VENC_CHN VeChn, const VENC_CHN_ATTR_S *pstAttr);
HI_S32 HI_MPI_VENC_DestroyChn(VENC_CHN VeChn);
HI_S32 HI_MPI_VENC_ResetChn(VENC_CHN VeChn);

HI_S32 HI_MPI_VENC_StartRecvPic(VENC_CHN VeChn);
HI_S32 HI_MPI_VENC_StartRecvPicEx(VENC_CHN VeChn, VENC_RECV_PIC_PARAM_S *pstRecvParam);
HI_S32 HI_MPI_VENC_StopRecvPic(VENC_CHN VeChn);

HI_S32 HI_MPI_VENC_Query(VENC_CHN VeChn, VENC_CHN_STAT_S *pstStat);

HI_S32 HI_MPI_VENC_SetChnAttr(VENC_CHN VeChn, const VENC_CHN_ATTR_S *pstAttr);
HI_S32 HI_MPI_VENC_GetChnAttr(VENC_CHN VeChn, VENC_CHN_ATTR_S *pstAttr);
/*-1:bolck  0:nonblock   >0 : overtime   */
HI_S32 HI_MPI_VENC_GetStream(VENC_CHN VeChn, VENC_STREAM_S *pstStream, HI_S32 s32MilliSec);
HI_S32 HI_MPI_VENC_ReleaseStream(VENC_CHN VeChn, VENC_STREAM_S *pstStream);

HI_S32 HI_MPI_VENC_InsertUserData(VENC_CHN VeChn, HI_U8 *pu8Data, HI_U32 u32Len);
/*-1:bolck  0:nonblock   >0 : overtime   */
HI_S32 HI_MPI_VENC_SendFrame(VENC_CHN VeChn, VIDEO_FRAME_INFO_S *pstFrame ,HI_S32 s32MilliSec);
HI_S32 HI_MPI_VENC_SendFrameEx(VENC_CHN VeChn, USER_FRAME_INFO_S *pstFrame, HI_S32 s32MilliSec);

HI_S32 HI_MPI_VENC_SetMaxStreamCnt(VENC_CHN VeChn, HI_U32 u32MaxStrmCnt);
HI_S32 HI_MPI_VENC_GetMaxStreamCnt(VENC_CHN VeChn, HI_U32 *pu32MaxStrmCnt);

HI_S32 HI_MPI_VENC_RequestIDR(VENC_CHN VeChn,HI_BOOL bInstant);

HI_S32 HI_MPI_VENC_GetFd(VENC_CHN VeChn);
HI_S32 HI_MPI_VENC_CloseFd(VENC_CHN VeChn);

HI_S32 HI_MPI_VENC_SetRoiCfg(VENC_CHN VeChn, VENC_ROI_CFG_S *pstVencRoiCfg);
HI_S32 HI_MPI_VENC_GetRoiCfg(VENC_CHN VeChn, HI_U32 u32Index, VENC_ROI_CFG_S *pstVencRoiCfg);

HI_S32 HI_MPI_VENC_SetRoiCfgEx(VENC_CHN VeChn, VENC_ROI_CFG_EX_S *pstVencRoiCfg);
HI_S32 HI_MPI_VENC_GetRoiCfgEx(VENC_CHN VeChn, HI_U32 u32Index, VENC_ROI_CFG_EX_S *pstVencRoiCfg);

HI_S32 HI_MPI_VENC_SetRoiBgFrameRate(VENC_CHN VeChn, const VENC_ROIBG_FRAME_RATE_S *pstRoiBgFrmRate);
HI_S32 HI_MPI_VENC_GetRoiBgFrameRate(VENC_CHN VeChn, VENC_ROIBG_FRAME_RATE_S *pstRoiBgFrmRate);

HI_S32 HI_MPI_VENC_SetH264SliceSplit(VENC_CHN VeChn, const VENC_PARAM_H264_SLICE_SPLIT_S *pstSliceSplit);
HI_S32 HI_MPI_VENC_GetH264SliceSplit(VENC_CHN VeChn, VENC_PARAM_H264_SLICE_SPLIT_S *pstSliceSplit);

HI_S32 HI_MPI_VENC_SetH264InterPred(VENC_CHN VeChn, const VENC_PARAM_H264_INTER_PRED_S *pstH264InterPred);
HI_S32 HI_MPI_VENC_GetH264InterPred(VENC_CHN VeChn, VENC_PARAM_H264_INTER_PRED_S *pstH264InterPred);

HI_S32 HI_MPI_VENC_SetH264IntraPred(VENC_CHN VeChn, const VENC_PARAM_H264_INTRA_PRED_S *pstH264IntraPred);
HI_S32 HI_MPI_VENC_GetH264IntraPred(VENC_CHN VeChn, VENC_PARAM_H264_INTRA_PRED_S *pstH264IntraPred);

HI_S32 HI_MPI_VENC_SetH264Trans(VENC_CHN VeChn, const VENC_PARAM_H264_TRANS_S *pstH264Trans);
HI_S32 HI_MPI_VENC_GetH264Trans(VENC_CHN VeChn, VENC_PARAM_H264_TRANS_S *pstH264Trans);

HI_S32 HI_MPI_VENC_SetH264Entropy(VENC_CHN VeChn, const VENC_PARAM_H264_ENTROPY_S *pstH264EntropyEnc);
HI_S32 HI_MPI_VENC_GetH264Entropy(VENC_CHN VeChn, VENC_PARAM_H264_ENTROPY_S *pstH264EntropyEnc);

HI_S32 HI_MPI_VENC_SetH264Poc(VENC_CHN VeChn, const VENC_PARAM_H264_POC_S *pstH264Poc);
HI_S32 HI_MPI_VENC_GetH264Poc(VENC_CHN VeChn, VENC_PARAM_H264_POC_S *pstH264Poc);

HI_S32 HI_MPI_VENC_SetH264Dblk(VENC_CHN VeChn, const VENC_PARAM_H264_DBLK_S *pstH264Dblk);
HI_S32 HI_MPI_VENC_GetH264Dblk(VENC_CHN VeChn, VENC_PARAM_H264_DBLK_S *pstH264Dblk);

HI_S32 HI_MPI_VENC_SetH264Vui(VENC_CHN VeChn, const VENC_PARAM_H264_VUI_S *pstH264Vui);
HI_S32 HI_MPI_VENC_GetH264Vui(VENC_CHN VeChn, VENC_PARAM_H264_VUI_S *pstH264Vui);

HI_S32 HI_MPI_VENC_SetJpegParam(VENC_CHN VeChn, const VENC_PARAM_JPEG_S *pstJpegParam);
HI_S32 HI_MPI_VENC_GetJpegParam(VENC_CHN VeChn, VENC_PARAM_JPEG_S *pstJpegParam);


HI_S32 HI_MPI_VENC_SetMjpegParam(VENC_CHN VeChn, const VENC_PARAM_MJPEG_S *pstMjpegParam);
HI_S32 HI_MPI_VENC_GetMjpegParam(VENC_CHN VeChn, VENC_PARAM_MJPEG_S *pstMjpegParam);

HI_S32 HI_MPI_VENC_SetFrameRate(VENC_CHN VeChn, const VENC_FRAME_RATE_S *pstFrameRate);
HI_S32 HI_MPI_VENC_GetFrameRate(VENC_CHN VeChn, VENC_FRAME_RATE_S *pstFrameRate);

HI_S32 HI_MPI_VENC_GetRcParam(VENC_CHN VeChn, VENC_RC_PARAM_S *pstRcParam);
HI_S32 HI_MPI_VENC_SetRcParam(VENC_CHN VeChn, const VENC_RC_PARAM_S *pstRcParam);


HI_S32 HI_MPI_VENC_SetRefParam(VENC_CHN VeChn, const VENC_PARAM_REF_S *pstRefParam);
HI_S32 HI_MPI_VENC_GetRefParam(VENC_CHN VeChn, VENC_PARAM_REF_S *pstRefParam);

HI_S32 HI_MPI_VENC_SetColor2Grey(VENC_CHN VeChn, const VENC_COLOR2GREY_S *pstChnColor2Grey);
HI_S32 HI_MPI_VENC_GetColor2Grey(VENC_CHN VeChn, VENC_COLOR2GREY_S *pstChnColor2Grey);

HI_S32 HI_MPI_VENC_SetCrop(VENC_CHN VeChn, const VENC_CROP_CFG_S *pstCropCfg);
HI_S32 HI_MPI_VENC_GetCrop(VENC_CHN VeChn, VENC_CROP_CFG_S *pstCropCfg);
HI_S32 HI_MPI_VENC_EnableIDR(VENC_CHN VeChn, HI_BOOL bEnableIDR);

HI_S32 HI_MPI_VENC_SetH264IdrPicId( VENC_CHN VeChn, VENC_H264_IDRPICID_CFG_S* pstH264eIdrPicIdCfg );
HI_S32 HI_MPI_VENC_GetH264IdrPicId( VENC_CHN VeChn, VENC_H264_IDRPICID_CFG_S* pstH264eIdrPicIdCfg );
HI_S32 HI_MPI_VENC_GetStreamBufInfo(VENC_CHN VeChn, VENC_STREAM_BUF_INFO_S *pstStreamBufInfo);


HI_S32 HI_MPI_VENC_SetH265SliceSplit(VENC_CHN VeChn, const VENC_PARAM_H265_SLICE_SPLIT_S *pstSliceSplit);
HI_S32 HI_MPI_VENC_GetH265SliceSplit(VENC_CHN VeChn, VENC_PARAM_H265_SLICE_SPLIT_S *pstSliceSplit);


HI_S32 HI_MPI_VENC_SetH265PredUnit(VENC_CHN VeChn, const VENC_PARAM_H265_PU_S *pstPredUnit);
HI_S32 HI_MPI_VENC_GetH265PredUnit(VENC_CHN VeChn, VENC_PARAM_H265_PU_S *pstPredUnit);


HI_S32 HI_MPI_VENC_SetH265Trans(VENC_CHN VeChn, const VENC_PARAM_H265_TRANS_S *pstH265Trans);
HI_S32 HI_MPI_VENC_GetH265Trans(VENC_CHN VeChn, VENC_PARAM_H265_TRANS_S *pstH265Trans);

HI_S32 HI_MPI_VENC_SetH265Entropy(VENC_CHN VeChn, const VENC_PARAM_H265_ENTROPY_S *pstH265Entropy);
HI_S32 HI_MPI_VENC_GetH265Entropy(VENC_CHN VeChn, VENC_PARAM_H265_ENTROPY_S *pstH265Entropy);

HI_S32 HI_MPI_VENC_SetH265Dblk(VENC_CHN VeChn, const VENC_PARAM_H265_DBLK_S *pstH265Dblk);
HI_S32 HI_MPI_VENC_GetH265Dblk(VENC_CHN VeChn, VENC_PARAM_H265_DBLK_S *pstH265Dblk);

HI_S32 HI_MPI_VENC_SetH265Sao(VENC_CHN VeChn, const VENC_PARAM_H265_SAO_S *pstH265Sao);
HI_S32 HI_MPI_VENC_GetH265Sao(VENC_CHN VeChn, VENC_PARAM_H265_SAO_S *pstH265Sao);


HI_S32 HI_MPI_VENC_SetH265Timing(VENC_CHN VeChn, const VENC_PARAM_H265_TIMING_S *pstH265Timing);
HI_S32 HI_MPI_VENC_GetH265Timing(VENC_CHN VeChn, VENC_PARAM_H265_TIMING_S *pstH265Timing);
    
HI_S32 HI_MPI_VENC_SetH265Vui(VENC_CHN VeChn, const VENC_PARAM_H265_VUI_S *pstH265Vui);
HI_S32 HI_MPI_VENC_GetH265Vui(VENC_CHN VeChn, VENC_PARAM_H265_VUI_S *pstH265Vui);

HI_S32 HI_MPI_VENC_SetFrameLostStrategy(VENC_CHN VeChn, const VENC_PARAM_FRAMELOST_S *pstFrmLostParam);
HI_S32 HI_MPI_VENC_GetFrameLostStrategy(VENC_CHN VeChn, VENC_PARAM_FRAMELOST_S *pstFrmLostParam);


HI_S32 HI_MPI_VENC_SetSuperFrameCfg(VENC_CHN VeChn, const VENC_SUPERFRAME_CFG_S *pstSuperFrmParam);
HI_S32 HI_MPI_VENC_GetSuperFrameCfg(VENC_CHN VeChn,VENC_SUPERFRAME_CFG_S *pstSuperFrmParam);

HI_S32 HI_MPI_VENC_AttachVbPool(VENC_CHN VeChn, VB_POOL hVbPool);
HI_S32 HI_MPI_VENC_DetachVbPool(VENC_CHN VeChn);

HI_S32 HI_MPI_VENC_SetIntraRefresh(VENC_CHN VeChn, VENC_PARAM_INTRA_REFRESH_S *pstIntraRefresh);
HI_S32 HI_MPI_VENC_GetIntraRefresh(VENC_CHN VeChn, VENC_PARAM_INTRA_REFRESH_S *pstIntraRefresh);

HI_S32 HI_MPI_VENC_GetSSERegion(VENC_CHN VeChn, HI_U32 u32Index,VENC_SSE_CFG_S *pstSSECfg);
HI_S32 HI_MPI_VENC_SetSSERegion(VENC_CHN VeChn, const VENC_SSE_CFG_S * pstSSECfg);
HI_S32 HI_MPI_VENC_SetModParam(VENC_PARAM_MOD_S *pstModParam);
HI_S32 HI_MPI_VENC_GetModParam(VENC_PARAM_MOD_S *pstModParam);

HI_S32 HI_MPI_VENC_EnableAdvSmartP( VENC_CHN VeChn, HI_BOOL bEnableAdvSmartP );


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __MPI_VENC_H__ */

