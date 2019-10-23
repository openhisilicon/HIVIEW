/******************************************************************************

  Copyright (C), 2013-2030, Hisilicon Tech. Co., Ltd.

 ******************************************************************************
  File Name     : mpi_vi.h
  Version       : Initial Draft
  Author        : Hisilicon multimedia software group
  Created       : 2013/05/08
  Description   :
  History       :
  1.Date        : 2013/05/08
    Author      :
    Modification: Created file

******************************************************************************/
#ifndef __MPI_VI_H__
#define __MPI_VI_H__

#include "hi_comm_vi.h"
#include "hi_comm_fisheye.h"

#ifdef __cplusplus
#if __cplusplus
extern "C"
{
#endif
#endif /* __cplusplus */

HI_S32 HI_MPI_VI_SetDevAttr(VI_DEV ViDev, const VI_DEV_ATTR_S *pstDevAttr);
HI_S32 HI_MPI_VI_GetDevAttr(VI_DEV ViDev, VI_DEV_ATTR_S *pstDevAttr);

HI_S32 HI_MPI_VI_EnableDev(VI_DEV ViDev);
HI_S32 HI_MPI_VI_DisableDev(VI_DEV ViDev);

HI_S32 HI_MPI_VI_SetChnAttr(VI_CHN ViChn, const VI_CHN_ATTR_S *pstAttr);
HI_S32 HI_MPI_VI_GetChnAttr(VI_CHN ViChn, VI_CHN_ATTR_S *pstAttr);

HI_S32 HI_MPI_VI_EnableChn(VI_CHN ViChn);
HI_S32 HI_MPI_VI_DisableChn(VI_CHN ViChn);

HI_S32 HI_MPI_VI_GetFrame(VI_CHN ViChn, VIDEO_FRAME_INFO_S *pstFrameInfo, HI_S32 s32MilliSec);
HI_S32 HI_MPI_VI_ReleaseFrame(VI_CHN ViChn, VIDEO_FRAME_INFO_S *pstFrameInfo);
HI_S32 HI_MPI_VI_SetFrameDepth(VI_CHN ViChn, HI_U32 u32Depth);
HI_S32 HI_MPI_VI_GetFrameDepth(VI_CHN ViChn, HI_U32 *pu32Depth);

HI_S32 HI_MPI_VI_SetUserPic(VI_CHN ViChn, VI_USERPIC_ATTR_S *pstUsrPic);
HI_S32 HI_MPI_VI_EnableUserPic(VI_CHN ViChn);
HI_S32 HI_MPI_VI_DisableUserPic(VI_CHN ViChn);

/* Normally, these functions are not necessary in typical business */
HI_S32 HI_MPI_VI_BindChn(VI_CHN ViChn, const VI_CHN_BIND_ATTR_S *pstChnBindAttr);
HI_S32 HI_MPI_VI_UnBindChn(VI_CHN ViChn);
HI_S32 HI_MPI_VI_GetChnBind(VI_CHN ViChn, VI_CHN_BIND_ATTR_S *pstChnBindAttr);

HI_S32 HI_MPI_VI_SetDevAttrEx(VI_DEV ViDev, const VI_DEV_ATTR_EX_S *pstDevAttrEx);
HI_S32 HI_MPI_VI_GetDevAttrEx(VI_DEV ViDev, VI_DEV_ATTR_EX_S *pstDevAttrEx);

HI_S32 HI_MPI_VI_GetFd(VI_CHN ViChn);
HI_S32 HI_MPI_VI_GetDevFd(VI_DEV ViDev);
HI_S32 HI_MPI_VI_Query(VI_CHN ViChn, VI_CHN_STAT_S *pstStat);

HI_S32 HI_MPI_VI_EnableChnInterrupt(VI_CHN ViChn);
HI_S32 HI_MPI_VI_DisableChnInterrupt(VI_CHN ViChn);

HI_S32 HI_MPI_VI_SetFlashConfig(VI_DEV ViDev, const VI_FLASH_CONFIG_S *pstFlashConfig);
HI_S32 HI_MPI_VI_GetFlashConfig(VI_DEV ViDev, VI_FLASH_CONFIG_S *pstFlashConfig);
HI_S32 HI_MPI_VI_TriggerFlash(VI_DEV ViDev, HI_BOOL bEnable);

HI_S32 HI_MPI_VI_SetExtChnAttr(VI_CHN ViChn, const VI_EXT_CHN_ATTR_S *pstExtChnAttr);
HI_S32 HI_MPI_VI_GetExtChnAttr(VI_CHN ViChn, VI_EXT_CHN_ATTR_S *pstExtChnAttr);

HI_S32 HI_MPI_VI_SetExtChnCrop(VI_CHN ViChn, const CROP_INFO_S *pstExtChnCrop);
HI_S32 HI_MPI_VI_GetExtChnCrop(VI_CHN ViChn, CROP_INFO_S *pstExtChnCrop);

HI_S32 HI_MPI_VI_SetLDCAttr(VI_CHN ViChn, const VI_LDC_ATTR_S *pstLDCAttr);
HI_S32 HI_MPI_VI_GetLDCAttr(VI_CHN ViChn, VI_LDC_ATTR_S *pstLDCAttr);

HI_S32 HI_MPI_VI_SetCSCAttr(VI_DEV ViDev, const VI_CSC_ATTR_S *pstCSCAttr);
HI_S32 HI_MPI_VI_GetCSCAttr(VI_DEV ViDev, VI_CSC_ATTR_S *pstCSCAttr);

HI_S32 HI_MPI_VI_SetRotate(VI_CHN ViChn, const ROTATE_E enRotate);
HI_S32 HI_MPI_VI_GetRotate(VI_CHN ViChn, ROTATE_E *penRotate);

HI_S32 HI_MPI_VI_SetRotateEx(VI_CHN ViChn, const VI_ROTATE_EX_ATTR_S* pstViRotateExAttr);
HI_S32 HI_MPI_VI_GetRotateEx(VI_CHN ViChn, VI_ROTATE_EX_ATTR_S *pstViRotateExAttr);

HI_S32 HI_MPI_VI_GetChnLuma(VI_CHN ViChn, VI_CHN_LUM_S *pstLuma);

HI_S32 HI_MPI_VI_SetWDRAttr(VI_DEV ViDev, const VI_WDR_ATTR_S *pstWDRAttr);
HI_S32 HI_MPI_VI_GetWDRAttr(VI_DEV ViDev, VI_WDR_ATTR_S *pstWDRAttr);

HI_S32 HI_MPI_VI_SetVCNumber(VI_DEV ViDev, const VI_VC_NUMBER_S *pstVCNumber);
HI_S32 HI_MPI_VI_GetVCNumber(VI_DEV ViDev, VI_VC_NUMBER_S *pstVCNumber);

HI_S32 HI_MPI_VI_SetFisheyeDevConfig(VI_DEV ViDev, const FISHEYE_CONFIG_S *pstFisheyeConfig);
HI_S32 HI_MPI_VI_GetFisheyeDevConfig(VI_DEV ViDev, FISHEYE_CONFIG_S *pstFisheyeConfig);

HI_S32 HI_MPI_VI_SetFisheyeAttr(VI_CHN ViChn, const FISHEYE_ATTR_S *pstFisheyeAttr);
HI_S32 HI_MPI_VI_GetFisheyeAttr(VI_CHN ViChn, FISHEYE_ATTR_S *pstFisheyeAttr);

HI_S32 HI_MPI_VI_SetSpreadAttr(VI_CHN ViChn, const SPREAD_ATTR_S *pstSpreadAttr);
HI_S32 HI_MPI_VI_GetSpreadAttr(VI_CHN ViChn, SPREAD_ATTR_S *pstSpreadAttr);

HI_S32 HI_MPI_VI_SetDevDumpAttr(VI_DEV ViDev, const VI_DUMP_ATTR_S *pstDumpAttr);
HI_S32 HI_MPI_VI_GetDevDumpAttr(VI_DEV ViDev, VI_DUMP_ATTR_S *pstDumpAttr);

HI_S32 HI_MPI_VI_EnableBayerDump(VI_DEV ViDev);
HI_S32 HI_MPI_VI_DisableBayerDump(VI_DEV ViDev);

HI_S32 HI_MPI_VI_EnableBayerRead(VI_DEV ViDev);
HI_S32 HI_MPI_VI_DisableBayerRead(VI_DEV ViDev);
HI_S32 HI_MPI_VI_SendBayerData(VI_DEV ViDev, const VI_RAW_DATA_INFO_S  *pstRawData, HI_S32 s32MilliSec);
HI_S32 HI_MPI_VI_SendBayerDataEx(VI_DEV ViDev, const VI_RAW_DATA_INFO_S *pastRawData[], HI_U32 u32DataNum, HI_S32 s32MilliSec);

HI_S32 HI_MPI_VI_SetDCIParam(VI_DEV ViDev, const VI_DCI_PARAM_S *pstDciParam);
HI_S32 HI_MPI_VI_GetDCIParam(VI_DEV ViDev, VI_DCI_PARAM_S *pstDciParam);

HI_S32 HI_MPI_VI_SetModParam(const VI_MOD_PARAM_S *pstModParam);
HI_S32 HI_MPI_VI_GetModParam(VI_MOD_PARAM_S *pstModParam);

HI_S32 HI_MPI_VI_CloseFd(HI_VOID);

HI_S32 HI_MPI_VI_SetDISConfig(VI_CHN ViChn, const VI_DIS_CONFIG_S *pstDISConfig);
HI_S32 HI_MPI_VI_GetDISConfig(VI_CHN ViChn, VI_DIS_CONFIG_S *pstDISConfig);
HI_S32 HI_MPI_VI_DISInit(VI_CHN ViChn);
HI_S32 HI_MPI_VI_SetDISAttr(VI_CHN ViChn, const VI_DIS_ATTR_S *pstDISAttr);
HI_S32 HI_MPI_VI_GetDISAttr(VI_CHN ViChn, VI_DIS_ATTR_S *pstDISAttr);
HI_S32 HI_MPI_VI_DISRun(VI_CHN ViChn);
HI_S32 HI_MPI_VI_DISExit(VI_CHN ViChn);
HI_S32 HI_MPI_VI_RegisterDISCallback(VI_CHN ViChn, VI_DIS_CALLBACK_S *pstDISCallback);
HI_S32 HI_MPI_VI_UnRegisterDISCallback(VI_CHN ViChn);
HI_S32 HI_MPI_VI_SetDISDebug(HI_BOOL bDebug);


/*These functions only apply to hi3519v101*/
HI_S32 HI_MPI_VI_SetStitchCorrectionAttr(VI_CHN ViChn, const VI_STITCH_CORRECTION_ATTR_S *pstCorretionAttr);
HI_S32 HI_MPI_VI_GetStitchCorrectionAttr(VI_CHN ViChn, VI_STITCH_CORRECTION_ATTR_S *pstCorretionAttr);
HI_S32 HI_MPI_VI_BindDev(VI_DEV ViDev,  const VI_DEV_BIND_ATTR_S *pstDevBindAttr);
HI_S32 HI_MPI_VI_GetDevBind(VI_DEV ViDev,VI_DEV_BIND_ATTR_S *pstDevBindAttr);
HI_S32 HI_MPI_VI_SetSnapAttr(VI_DEV ViDev, const VI_SNAP_ATTR_S *pstSnapAttr);
HI_S32 HI_MPI_VI_GetSnapAttr(VI_DEV ViDev, VI_SNAP_ATTR_S *pstSnapAttr);
HI_S32 HI_MPI_VI_EnableSnap(VI_DEV ViDev);
HI_S32 HI_MPI_VI_DisableSnap(VI_DEV ViDev);
HI_S32 HI_MPI_VI_TriggerSnap(VI_DEV ViDev);
HI_S32 HI_MPI_VI_MutliTrigger(HI_VOID);
HI_S32 HI_MPI_VI_GetSnapRaw(VI_DEV ViDev, VI_RAW_FRAME_INFO_S *pstFrameInfo, HI_S32 s32MilliSec);
HI_S32 HI_MPI_VI_ReleaseSnapRaw(VI_DEV ViDev, VI_RAW_FRAME_INFO_S *pstFrameInfo);
HI_S32 HI_MPI_VI_SendSnapRaw(VI_DEV ViDev, const VI_RAW_FRAME_INFO_S *pstFrameInfo,  HI_S32 s32MilliSec);
HI_S32 HI_MPI_VI_SetBayerReadAttr(VI_DEV ViDev, const VI_BAYER_READ_ATTR_S *pstBayReadAttr);
HI_S32 HI_MPI_VI_GetBayerReadAttr(VI_DEV ViDev, VI_BAYER_READ_ATTR_S *pstBayReadAttr);

/*These functions only bt1120/BT656 get Af statistics info,only support hi3519V101*/
HI_S32 HI_MPI_VI_SetAFStatisticsConfig(VI_CHN ViChn, const ISP_FOCUS_STATISTICS_CFG_S *pstAFStatConfig);
HI_S32 HI_MPI_VI_GetAFStatisticsConfig(VI_CHN ViChn, ISP_FOCUS_STATISTICS_CFG_S *pstAFStatConfig);
HI_S32 HI_MPI_VI_GetAFStatistics(VI_CHN ViChn, ISP_AF_STAT_S *pstAFStatistics);
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /*__MPI_VI_H__ */

