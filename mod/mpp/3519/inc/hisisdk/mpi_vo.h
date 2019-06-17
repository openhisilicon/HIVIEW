/******************************************************************************

  Copyright (C), 2001-2011, Hisilicon Tech. Co., Ltd.

 ******************************************************************************
  File Name     : mpi_vo.h
  Version       : Initial Draft
  Author        : Hisilicon multimedia software group
  Created       : 2013/05/08
  Description   : 
  History       :
  1.Date        : 2013/05/08
    Author      : d00224772
    Modification: Created file

******************************************************************************/
#ifndef __MPI_VO_H__
#define __MPI_VO_H__

#include "hi_comm_vo.h"

#ifdef __cplusplus
#if __cplusplus
extern "C"
{
#endif
#endif /* __cplusplus */

/* Device Settings */

HI_S32 HI_MPI_VO_Enable (VO_DEV VoDev);
HI_S32 HI_MPI_VO_Disable(VO_DEV VoDev);

HI_S32 HI_MPI_VO_SetPubAttr(VO_DEV VoDev, const VO_PUB_ATTR_S *pstPubAttr);
HI_S32 HI_MPI_VO_GetPubAttr(VO_DEV VoDev, VO_PUB_ATTR_S *pstPubAttr);

HI_S32 HI_MPI_VO_CloseFd(HI_VOID);

/* Video Settings */

HI_S32 HI_MPI_VO_EnableVideoLayer (VO_LAYER VoLayer);
HI_S32 HI_MPI_VO_DisableVideoLayer(VO_LAYER VoLayer);

HI_S32 HI_MPI_VO_SetVideoLayerAttr(VO_LAYER VoLayer, const VO_VIDEO_LAYER_ATTR_S *pstLayerAttr);
HI_S32 HI_MPI_VO_GetVideoLayerAttr(VO_LAYER VoLayer, VO_VIDEO_LAYER_ATTR_S *pstLayerAttr);

HI_S32 HI_MPI_VO_SetVideoLayerPriority(VO_LAYER VoLayer, HI_U32 u32Priority);
HI_S32 HI_MPI_VO_GetVideoLayerPriority(VO_LAYER VoLayer, HI_U32 *pu32Priority);

HI_S32 HI_MPI_VO_SetVideoLayerCSC(VO_LAYER VoLayer, const VO_CSC_S *pstVideoCSC);
HI_S32 HI_MPI_VO_GetVideoLayerCSC(VO_LAYER VoLayer, VO_CSC_S *pstVideoCSC);

HI_S32 HI_MPI_VO_SetVideoLayerPartitionMode(VO_LAYER VoLayer, VO_PART_MODE_E enPartMode);
HI_S32 HI_MPI_VO_GetVideoLayerPartitionMode(VO_LAYER VoLayer, VO_PART_MODE_E *penPartMode);

HI_S32 HI_MPI_VO_SetAttrBegin(VO_LAYER VoLayer);
HI_S32 HI_MPI_VO_SetAttrEnd  (VO_LAYER VoLayer);

HI_S32 HI_MPI_VO_SetPlayToleration(VO_LAYER VoLayer, HI_U32 u32Toleration);
HI_S32 HI_MPI_VO_GetPlayToleration(VO_LAYER VoLayer, HI_U32 *pu32Toleration);

HI_S32 HI_MPI_VO_GetScreenFrame    (VO_LAYER VoLayer, VIDEO_FRAME_INFO_S *pstVFrame, HI_S32 s32MilliSec);
HI_S32 HI_MPI_VO_ReleaseScreenFrame(VO_LAYER VoLayer, VIDEO_FRAME_INFO_S *pstVFrame);

HI_S32 HI_MPI_VO_SetDispBufLen(VO_LAYER VoLayer, HI_U32 u32BufLen);
HI_S32 HI_MPI_VO_GetDispBufLen(VO_LAYER VoLayer, HI_U32 *pu32BufLen);

/* General Operation of Channel */

HI_S32 HI_MPI_VO_EnableChn (VO_LAYER VoLayer, VO_CHN VoChn);
HI_S32 HI_MPI_VO_DisableChn(VO_LAYER VoLayer, VO_CHN VoChn);

HI_S32 HI_MPI_VO_SetChnAttr(VO_LAYER VoLayer, VO_CHN VoChn, const VO_CHN_ATTR_S *pstChnAttr);
HI_S32 HI_MPI_VO_GetChnAttr(VO_LAYER VoLayer, VO_CHN VoChn, VO_CHN_ATTR_S *pstChnAttr);

HI_S32 HI_MPI_VO_SetChnField(VO_LAYER VoLayer, VO_CHN VoChn, const VO_DISPLAY_FIELD_E enField);
HI_S32 HI_MPI_VO_GetChnField(VO_LAYER VoLayer, VO_CHN VoChn, VO_DISPLAY_FIELD_E *pField);

HI_S32 HI_MPI_VO_SetChnFrameRate(VO_LAYER VoLayer, VO_CHN VoChn, HI_S32 s32ChnFrmRate);
HI_S32 HI_MPI_VO_GetChnFrameRate(VO_LAYER VoLayer, VO_CHN VoChn, HI_S32 *ps32ChnFrmRate);

HI_S32 HI_MPI_VO_GetChnFrame(VO_LAYER VoLayer, VO_CHN VoChn, VIDEO_FRAME_INFO_S *pstFrame, HI_S32 s32MilliSec);
HI_S32 HI_MPI_VO_ReleaseChnFrame(VO_LAYER VoLayer, VO_CHN VoChn, const VIDEO_FRAME_INFO_S *pstFrame);

HI_S32 HI_MPI_VO_PauseChn (VO_LAYER VoLayer, VO_CHN VoChn);
HI_S32 HI_MPI_VO_ResumeChn(VO_LAYER VoLayer, VO_CHN VoChn);
HI_S32 HI_MPI_VO_StepChn(VO_LAYER VoLayer, VO_CHN VoChn);
HI_S32 HI_MPI_VO_RefreshChn( VO_LAYER VoLayer, VO_CHN VoChn);

HI_S32 HI_MPI_VO_ShowChn(VO_LAYER VoLayer, VO_CHN VoChn);
HI_S32 HI_MPI_VO_HideChn(VO_LAYER VoLayer, VO_CHN VoChn);

HI_S32 HI_MPI_VO_SetZoomInWindow(VO_LAYER VoLayer, VO_CHN VoChn, const VO_ZOOM_ATTR_S *pstZoomAttr);
HI_S32 HI_MPI_VO_GetZoomInWindow(VO_LAYER VoLayer, VO_CHN VoChn, VO_ZOOM_ATTR_S *pstZoomAttr);

HI_S32 HI_MPI_VO_GetChnPts   (VO_LAYER VoLayer, VO_CHN VoChn, HI_U64 *pu64ChnPts);
HI_S32 HI_MPI_VO_QueryChnStat(VO_LAYER VoLayer, VO_CHN VoChn, VO_QUERY_STATUS_S *pstStatus);

HI_S32 HI_MPI_VO_SendFrame(VO_LAYER VoLayer, VO_CHN VoChn, VIDEO_FRAME_INFO_S *pstVFrame, HI_S32 s32MilliSec);

HI_S32 HI_MPI_VO_ClearChnBuffer(VO_LAYER VoLayer, VO_CHN VoChn, HI_BOOL bClrAll);

HI_S32 HI_MPI_VO_SetChnBorder(VO_LAYER VoLayer, VO_CHN VoChn, const VO_BORDER_S *pstBorder);
HI_S32 HI_MPI_VO_GetChnBorder(VO_LAYER VoLayer, VO_CHN VoChn, VO_BORDER_S *pstBorder);

HI_S32 HI_MPI_VO_SetChnReceiveThreshold(VO_LAYER VoLayer, VO_CHN VoChn, HI_U32 u32Threshold);
HI_S32 HI_MPI_VO_GetChnReceiveThreshold(VO_LAYER VoLayer, VO_CHN VoChn, HI_U32 *pu32Threshold);

HI_S32 HI_MPI_VO_GetChnRegionLuma(VO_LAYER VoLayer, VO_CHN VoChn, VO_REGION_INFO_S *pstRegionInfo,
                                                HI_U32 *pu32LumaData, HI_S32 s32MilliSec);
/* GraphicLayer setting */


HI_S32 HI_MPI_VO_SetGraphicLayerCSC(GRAPHIC_LAYER GraphicLayer, const VO_CSC_S *pstCSC);
HI_S32 HI_MPI_VO_GetGraphicLayerCSC(GRAPHIC_LAYER GraphicLayer, VO_CSC_S *pstCSC);

HI_S32 HI_MPI_VO_SetDevFrameRate(VO_DEV VoDev, HI_U32 u32FrameRate);
HI_S32 HI_MPI_VO_GetDevFrameRate(VO_DEV VoDev, HI_U32 *pu32FrameRate);

HI_S32 HI_MPI_VO_SetVideoLayerRotate(VO_LAYER VoLayer, const ROTATE_E enRotate);
HI_S32 HI_MPI_VO_GetVideoLayerRotate(VO_LAYER VoLayer, ROTATE_E * penRotate);




#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /*__MPI_VO_H__ */

