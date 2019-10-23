/******************************************************************************

  Copyright (C), 2001-2011, Hisilicon Tech. Co., Ltd.

 ******************************************************************************
  File Name     : mpi_vpss.h
  Version       : Initial Draft
  Author        : 
  Created       : 
  Last Modified :
  Description   : mpi functions declaration
  Function List :
  History       :
  1.Date        : 20130508
    Author      : l00183122
    Modification: Create
******************************************************************************/
#ifndef __MPI_VPSS_H__
#define __MPI_VPSS_H__

#include "hi_common.h"
#include "hi_comm_video.h"
#include "hi_comm_vpss.h"
#include "hi_comm_vb.h"

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */


HI_S32 HI_MPI_VPSS_CreateGrp(VPSS_GRP VpssGrp, VPSS_GRP_ATTR_S *pstGrpAttr);
HI_S32 HI_MPI_VPSS_DestroyGrp(VPSS_GRP VpssGrp);

HI_S32 HI_MPI_VPSS_StartGrp(VPSS_GRP VpssGrp);
HI_S32 HI_MPI_VPSS_StopGrp(VPSS_GRP VpssGrp);

HI_S32 HI_MPI_VPSS_ResetGrp(VPSS_GRP VpssGrp);

HI_S32 HI_MPI_VPSS_EnableChn(VPSS_GRP VpssGrp, VPSS_CHN s32VpssChnl);
HI_S32 HI_MPI_VPSS_DisableChn(VPSS_GRP VpssGrp, VPSS_CHN s32VpssChnl);

HI_S32 HI_MPI_VPSS_GetGrpAttr(VPSS_GRP VpssGrp, VPSS_GRP_ATTR_S *pstGrpAttr);
HI_S32 HI_MPI_VPSS_SetGrpAttr(VPSS_GRP VpssGrp, VPSS_GRP_ATTR_S *pstGrpAttr);

HI_S32 HI_MPI_VPSS_EnableBackupFrame(VPSS_GRP VpssGrp);
HI_S32 HI_MPI_VPSS_DisableBackupFrame(VPSS_GRP VpssGrp);


HI_S32 HI_MPI_VPSS_GetChnAttr(VPSS_GRP VpssGrp, VPSS_CHN VpssChn, VPSS_CHN_ATTR_S *pstChnAttr);
HI_S32 HI_MPI_VPSS_SetChnAttr(VPSS_GRP VpssGrp, VPSS_CHN VpssChn, VPSS_CHN_ATTR_S *pstChnAttr);

HI_S32 HI_MPI_VPSS_SetChnParam(VPSS_GRP VpssGrp, VPSS_CHN VpssChn, VPSS_CHN_PARAM_S *pstChnParam);
HI_S32 HI_MPI_VPSS_GetChnParam(VPSS_GRP VpssGrp, VPSS_CHN VpssChn, VPSS_CHN_PARAM_S *pstChnParam);

HI_S32 HI_MPI_VPSS_SetGrpParam(VPSS_GRP VpssGrp, VPSS_GRP_PARAM_S *pstVpssParam);
HI_S32 HI_MPI_VPSS_GetGrpParam(VPSS_GRP VpssGrp, VPSS_GRP_PARAM_S *pstVpssParam);

HI_S32 HI_MPI_VPSS_SetGrpAdvancedParam(VPSS_GRP VpssGrp, VPSS_NR_ADVANCED_PARAM_S *pstVpssParam);
HI_S32 HI_MPI_VPSS_GetGrpAdvancedParam(VPSS_GRP VpssGrp, VPSS_NR_ADVANCED_PARAM_S *pstVpssParam);

HI_S32 HI_MPI_VPSS_SetGrpCrop(VPSS_GRP VpssGrp,  VPSS_CROP_INFO_S *pstCropInfo);
HI_S32 HI_MPI_VPSS_GetGrpCrop(VPSS_GRP VpssGrp,  VPSS_CROP_INFO_S *pstCropInfo);

HI_S32 HI_MPI_VPSS_SetChnMode(VPSS_GRP VpssGrp, VPSS_CHN VpssChn, VPSS_CHN_MODE_S *pstVpssMode);
HI_S32 HI_MPI_VPSS_GetChnMode(VPSS_GRP VpssGrp, VPSS_CHN VpssChn, VPSS_CHN_MODE_S *pstVpssMode);

HI_S32 HI_MPI_VPSS_SetDepth(VPSS_GRP VpssGrp, VPSS_CHN VpssChn, HI_U32 u32Depth); 
HI_S32 HI_MPI_VPSS_GetDepth(VPSS_GRP VpssGrp, VPSS_CHN VpssChn, HI_U32 *pu32Depth);

HI_S32 HI_MPI_VPSS_SendFrame(VPSS_GRP VpssGrp,  VIDEO_FRAME_INFO_S *pstVideoFrame, HI_S32 s32MilliSec);

HI_S32 HI_MPI_VPSS_GetChnFrame(VPSS_GRP VpssGrp, VPSS_CHN VpssChn, 
                               VIDEO_FRAME_INFO_S *pstVideoFrame, HI_S32 s32MilliSec);
HI_S32 HI_MPI_VPSS_ReleaseChnFrame(VPSS_GRP VpssGrp, VPSS_CHN VpssChn,  VIDEO_FRAME_INFO_S *pstVideoFrame);

HI_S32 HI_MPI_VPSS_GetGrpFrame(VPSS_GRP VpssGrp, VIDEO_FRAME_INFO_S *pstVideoFrame, HI_U32 u32FrameIndex);
HI_S32 HI_MPI_VPSS_ReleaseGrpFrame(VPSS_GRP VpssGrp,  VIDEO_FRAME_INFO_S *pstVideoFrame);

HI_S32 HI_MPI_VPSS_SetGrpDelay(VPSS_GRP VpssGrp, HI_U32 u32Delay);
HI_S32 HI_MPI_VPSS_GetGrpDelay(VPSS_GRP VpssGrp, HI_U32 *pu32Delay);

HI_S32 HI_MPI_VPSS_SetPreScale(VPSS_GRP VpssGrp,VPSS_PRESCALE_INFO_S *pstPreScaleInfo);
HI_S32 HI_MPI_VPSS_GetPreScale(VPSS_GRP VpssGrp,VPSS_PRESCALE_INFO_S *pstPreScaleInfo);

HI_S32 HI_MPI_VPSS_SetGrpSizer(VPSS_GRP VpssGrp, VPSS_SIZER_INFO_S *pstVpssSizerInfo);
HI_S32 HI_MPI_VPSS_GetGrpSizer(VPSS_GRP VpssGrp, VPSS_SIZER_INFO_S *pstVpssSizerInfo);


HI_S32 HI_MPI_VPSS_SetGrpFrameRate(VPSS_GRP VpssGrp, VPSS_FRAME_RATE_S *pstVpssFrameRate);
HI_S32 HI_MPI_VPSS_GetGrpFrameRate(VPSS_GRP VpssGrp, VPSS_FRAME_RATE_S *pstVpssFrameRate);

HI_S32 HI_MPI_VPSS_SetChnOverlay(VPSS_GRP VpssGrp,VPSS_CHN VpssChn, HI_U32 u32OverlayMask);
HI_S32 HI_MPI_VPSS_GetChnOverlay(VPSS_GRP VpssGrp,VPSS_CHN VpssChn, HI_U32 *pu32OverlayMask);


HI_S32 HI_MPI_VPSS_GetGrpRegionLuma(VPSS_GRP VpssGrp,VPSS_REGION_INFO_S *pstRegionInfo,
                                                HI_U32 *pu32LumaData,HI_S32 s32MilliSec);
HI_S32 HI_MPI_VPSS_GetChnRegionLuma(VPSS_GRP VpssGrp, VPSS_CHN VpssChn, VPSS_REGION_INFO_S *pstRegionInfo,
                                                HI_U32 *pu32LumaData,HI_S32 s32MilliSec);


HI_S32 HI_MPI_VPSS_EnableUserFrameRateCtrl(VPSS_GRP VpssGrp);
HI_S32 HI_MPI_VPSS_DisableUserFrameRateCtrl(VPSS_GRP VpssGrp);


HI_S32 HI_MPI_VPSS_SetPreScaleMode(VPSS_GRP VpssGrp, VPSS_PRESCALE_MODE_E enVpssPreScaleMode);
HI_S32 HI_MPI_VPSS_GetPreScaleMode(VPSS_GRP VpssGrp, VPSS_PRESCALE_MODE_E* penVpssPreScaleMode);

HI_S32 HI_MPI_VPSS_AttachVbPool(VPSS_GRP VpssGrp, VPSS_CHN VpssChn, VB_POOL hVbPool);
HI_S32 HI_MPI_VPSS_DetachVbPool(VPSS_GRP VpssGrp, VPSS_CHN VpssChn);

HI_S32 HI_MPI_VPSS_SetModParam(VPSS_MOD_PARAM_S *pstModParam);
HI_S32 HI_MPI_VPSS_GetModParam(VPSS_MOD_PARAM_S *pstModParam);

HI_S32 HI_MPI_VPSS_SetRotate(VPSS_GRP VpssGrp, VPSS_CHN VpssChn, ROTATE_E enRotate);
HI_S32 HI_MPI_VPSS_GetRotate(VPSS_GRP VpssGrp, VPSS_CHN VpssChn, ROTATE_E *penRotate);


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __MPI_VPSS_H__ */

