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
HI_S32 HI_MPI_VO_SetPubAttr(VO_DEV VoDev, const VO_PUB_ATTR_S *pstPubAttr);
HI_S32 HI_MPI_VO_GetPubAttr(VO_DEV VoDev, VO_PUB_ATTR_S *pstPubAttr);

HI_S32 HI_MPI_VO_Enable (VO_DEV VoDev);
HI_S32 HI_MPI_VO_Disable(VO_DEV VoDev);

HI_S32 HI_MPI_VO_CloseFd(HI_VOID);

/* Video Settings */

HI_S32 HI_MPI_VO_SetVideoLayerAttr(VO_LAYER VoLayer, const VO_VIDEO_LAYER_ATTR_S *pstLayerAttr);
HI_S32 HI_MPI_VO_GetVideoLayerAttr(VO_LAYER VoLayer, VO_VIDEO_LAYER_ATTR_S *pstLayerAttr);

HI_S32 HI_MPI_VO_SetVideoLayerCompressAttr(VO_LAYER VoLayer, const VO_COMPRESS_ATTR_S *pstCompressAttr);
HI_S32 HI_MPI_VO_GetVideoLayerCompressAttr(VO_LAYER VoLayer, VO_COMPRESS_ATTR_S *pstCompressAttr);

HI_S32 HI_MPI_VO_EnableVideoLayer (VO_LAYER VoLayer);
HI_S32 HI_MPI_VO_DisableVideoLayer(VO_LAYER VoLayer);

HI_S32 HI_MPI_VO_BindVideoLayer(VO_LAYER VoLayer, VO_DEV VoDev);
HI_S32 HI_MPI_VO_UnBindVideoLayer(VO_LAYER VoLayer, VO_DEV VoDev);

HI_S32 HI_MPI_VO_SetVideoLayerPriority(VO_LAYER VoLayer, HI_U32 u32Priority);
HI_S32 HI_MPI_VO_GetVideoLayerPriority(VO_LAYER VoLayer, HI_U32 *pu32Priority);

HI_S32 HI_MPI_VO_SetVideoLayerCSC(VO_LAYER VoLayer, const VO_CSC_S *pstVideoCSC);
HI_S32 HI_MPI_VO_GetVideoLayerCSC(VO_LAYER VoLayer, VO_CSC_S *pstVideoCSC);

HI_S32 HI_MPI_VO_SetVideoLayerPartitionMode(VO_LAYER VoLayer, VO_PART_MODE_E enPartMode);
HI_S32 HI_MPI_VO_GetVideoLayerPartitionMode(VO_LAYER VoLayer, VO_PART_MODE_E *penPartMode);

HI_S32 HI_MPI_VO_SetAttrBegin(VO_LAYER VoLayer);
HI_S32 HI_MPI_VO_SetAttrEnd  (VO_LAYER VoLayer);

HI_S32 HI_MPI_VO_GetScreenFrame(VO_LAYER VoLayer, VIDEO_FRAME_INFO_S *pstVFrame, HI_S32 s32MilliSec);
HI_S32 HI_MPI_VO_ReleaseScreenFrame(VO_LAYER VoLayer, VIDEO_FRAME_INFO_S *pstVFrame);

HI_S32 HI_MPI_VO_SetDispBufLen(VO_LAYER VoLayer, HI_U32 u32BufLen);
HI_S32 HI_MPI_VO_GetDispBufLen(VO_LAYER VoLayer, HI_U32 *pu32BufLen);

/* General Operation of Channel */

HI_S32 HI_MPI_VO_EnableChn (VO_LAYER VoLayer, VO_CHN VoChn);
HI_S32 HI_MPI_VO_DisableChn(VO_LAYER VoLayer, VO_CHN VoChn);

HI_S32 HI_MPI_VO_SetChnAttr(VO_LAYER VoLayer, VO_CHN VoChn, const VO_CHN_ATTR_S *pstChnAttr);
HI_S32 HI_MPI_VO_GetChnAttr(VO_LAYER VoLayer, VO_CHN VoChn, VO_CHN_ATTR_S *pstChnAttr);

HI_S32 HI_MPI_VO_SetChnParam(VO_LAYER VoLayer, VO_CHN VoChn, const VO_CHN_PARAM_S *pstChnParam);
HI_S32 HI_MPI_VO_GetChnParam(VO_LAYER VoLayer, VO_CHN VoChn, VO_CHN_PARAM_S *pstChnParam);

HI_S32 HI_MPI_VO_SetChnDispPos(VO_LAYER VoLayer, VO_CHN VoChn, const POINT_S *pstDispPos);
HI_S32 HI_MPI_VO_GetChnDispPos(VO_LAYER VoLayer, VO_CHN VoChn, POINT_S *pstDispPos);

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

HI_S32 HI_MPI_VO_SetPlayToleration(VO_LAYER VoLayer, HI_U32 u32Toleration);
HI_S32 HI_MPI_VO_GetPlayToleration(VO_LAYER VoLayer, HI_U32 *pu32Toleration);

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
/* WBC setting */

HI_S32 HI_MPI_VO_SetWbcSource(VO_WBC VoWbc, const VO_WBC_SOURCE_S *pstWbcSource);
HI_S32 HI_MPI_VO_GetWbcSource(VO_WBC VoWbc, VO_WBC_SOURCE_S *pstWbcSources);

HI_S32 HI_MPI_VO_EnableWbc(VO_WBC VoWbc);
HI_S32 HI_MPI_VO_DisableWbc(VO_WBC VoWbc);

HI_S32 HI_MPI_VO_SetWbcAttr(VO_WBC VoWbc, const VO_WBC_ATTR_S *pstWbcAttr);
HI_S32 HI_MPI_VO_GetWbcAttr(VO_WBC VoWbc, VO_WBC_ATTR_S *pstWbcAttr);

HI_S32 HI_MPI_VO_SetWbcMode(VO_WBC VoWbc, VO_WBC_MODE_E enWbcMode);
HI_S32 HI_MPI_VO_GetWbcMode(VO_WBC VoWbc, VO_WBC_MODE_E *penWbcMode);

HI_S32 HI_MPI_VO_SetWbcDepth(VO_WBC VoWbc, HI_U32 u32Depth);
HI_S32 HI_MPI_VO_GetWbcDepth(VO_WBC VoWbc, HI_U32 *pu32Depth);

HI_S32 HI_MPI_VO_GetWbcFrame(VO_WBC VoWbc, VIDEO_FRAME_INFO_S *pstVFrame, HI_S32 s32MilliSec);
HI_S32 HI_MPI_VO_ReleaseWbcFrame(VO_WBC VoWbc, VIDEO_FRAME_INFO_S *pstVFrame);

/* GraphicLayer setting */

HI_S32 HI_MPI_VO_BindGraphicLayer(GRAPHIC_LAYER GraphicLayer, VO_DEV VoDev);
HI_S32 HI_MPI_VO_UnBindGraphicLayer(GRAPHIC_LAYER GraphicLayer, VO_DEV VoDev);

HI_S32 HI_MPI_VO_SetGraphicLayerCSC(GRAPHIC_LAYER GraphicLayer, const VO_CSC_S *pstCSC);
HI_S32 HI_MPI_VO_GetGraphicLayerCSC(GRAPHIC_LAYER GraphicLayer, VO_CSC_S *pstCSC);

/* Cascade setting */

HI_S32 HI_MPI_VO_SetCascadeAttr(const VO_CAS_ATTR_S *pstCasAttr);
HI_S32 HI_MPI_VO_GetCascadeAttr(VO_CAS_ATTR_S *pstCasAttr);

HI_S32 HI_MPI_VO_EnableCascadeDev (VO_DEV VoCasDev);
HI_S32 HI_MPI_VO_DisableCascadeDev(VO_DEV VoCasDev);

HI_S32 HI_MPI_VO_SetCascadePattern(VO_DEV VoCasDev, HI_U32 u32Pattern);
HI_S32 HI_MPI_VO_GetCascadePattern(VO_DEV VoCasDev, HI_U32 *pu32Pattern);

HI_S32 HI_MPI_VO_CascadePosBindChn(HI_U32 u32Pos, VO_DEV VoCasDev, VO_CHN VoChn);
HI_S32 HI_MPI_VO_CascadePosUnBindChn(HI_U32 u32Pos, VO_DEV VoCasDev, VO_CHN VoChn);

HI_S32 HI_MPI_VO_EnableCascade (HI_VOID);
HI_S32 HI_MPI_VO_DisableCascade(HI_VOID);

/* VGA setting */

HI_S32 HI_MPI_VO_GetVgaParam(VO_DEV VoDev, VO_VGA_PARAM_S *pstVgaParam);
HI_S32 HI_MPI_VO_SetVgaParam(VO_DEV VoDev, VO_VGA_PARAM_S *pstVgaParam);

HI_S32 HI_MPI_VO_SetDevFrameRate(VO_DEV VoDev, HI_U32 u32FrameRate);
HI_S32 HI_MPI_VO_GetDevFrameRate(VO_DEV VoDev, HI_U32 *pu32FrameRate);

HI_S32 HI_MPI_VO_EnableRecvFrameRateMatch (VO_LAYER VoLayer, VO_CHN VoChn);
HI_S32 HI_MPI_VO_DisableRecvFrameRateMatch (VO_LAYER VoLayer, VO_CHN VoChn);

/* HDMI setting */
HI_S32 HI_MPI_VO_GetHdmiParam(VO_DEV VoDev, VO_HDMI_PARAM_S *pstHdmiParam);
HI_S32 HI_MPI_VO_SetHdmiParam(VO_DEV VoDev, VO_HDMI_PARAM_S *pstHdmiParam);

HI_S32 HI_MPI_VO_SetVtth(VO_DEV VoDev, HI_U32 u32Vtth);
HI_S32 HI_MPI_VO_GetVtth(VO_DEV VoDev, HI_U32* pu32Vtth);

HI_S32 HI_MPI_VO_QueryDevIntfStatus(VO_INTF_TYPE_E enDevInter,VO_DEVINTF_STATUS_S* pstStatus);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /*__MPI_VO_H__ */

