/******************************************************************************

  Copyright (C), 2001-2011, Hisilicon Tech. Co., Ltd.

 ******************************************************************************
  File Name     : mpi_isp.h
  Version       : Initial Draft
  Author        : Hisilicon multimedia software group
  Created       : 2010/12/20
  Description   : 
  History       :
  1.Date        : 2010/12/20
    Author      : x00100808
    Modification: Created file

******************************************************************************/

#ifndef __MPI_ISP_H__
#define __MPI_ISP_H__

#include "hi_comm_isp.h"
#include "hi_comm_sns.h"
#include "hi_comm_3a.h"
#include "hi_comm_video.h"


#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* End of #ifdef __cplusplus */

/* Firmware Main Operation */
HI_S32 HI_MPI_ISP_Init(ISP_DEV IspDev);
HI_S32 HI_MPI_ISP_MemInit(ISP_DEV IspDev);
HI_S32 HI_MPI_ISP_Run(ISP_DEV IspDev);
HI_S32 HI_MPI_ISP_Exit(ISP_DEV IspDev);

HI_S32 HI_MPI_ISP_SetISPPipeDiffAttr(ISP_DEV IspDev, ISP_PIPE_DIFF_ATTR_S *pstDiffAttr);
HI_S32 HI_MPI_ISP_GetISPPipeDiffAttr(ISP_DEV IspDev, ISP_PIPE_DIFF_ATTR_S *pstDiffAttr);

HI_S32 HI_MPI_ISP_SensorRegCallBack(ISP_DEV IspDev, SENSOR_ID SensorId, ISP_SENSOR_REGISTER_S *pstRegister);
HI_S32 HI_MPI_ISP_SensorUnRegCallBack(ISP_DEV IspDev, SENSOR_ID SensorId);

/* if have registered multy libs, set bind attr to appoint the active lib. */
HI_S32 HI_MPI_ISP_SetBindAttr(ISP_DEV IspDev, const ISP_BIND_ATTR_S *pstBindAttr);
HI_S32 HI_MPI_ISP_GetBindAttr(ISP_DEV IspDev, ISP_BIND_ATTR_S *pstBindAttr);
HI_S32 HI_MPI_ISP_AELibRegCallBack(ISP_DEV IspDev, ALG_LIB_S *pstAeLib,
        ISP_AE_REGISTER_S *pstRegister);
HI_S32 HI_MPI_ISP_AWBLibRegCallBack(ISP_DEV IspDev, ALG_LIB_S *pstAwbLib,
        ISP_AWB_REGISTER_S *pstRegister);
HI_S32 HI_MPI_ISP_AFLibRegCallBack(ISP_DEV IspDev, ALG_LIB_S *pstAfLib,
        ISP_AF_REGISTER_S *pstRegister);
HI_S32 HI_MPI_ISP_AELibUnRegCallBack(ISP_DEV IspDev, ALG_LIB_S *pstAeLib);
HI_S32 HI_MPI_ISP_AWBLibUnRegCallBack(ISP_DEV IspDev, ALG_LIB_S *pstAwbLib);
HI_S32 HI_MPI_ISP_AFLibUnRegCallBack(ISP_DEV IspDev, ALG_LIB_S *pstAfLib);

HI_S32 HI_MPI_ISP_SetPubAttr(ISP_DEV IspDev, const ISP_PUB_ATTR_S *pstPubAttr);
HI_S32 HI_MPI_ISP_GetPubAttr(ISP_DEV IspDev, ISP_PUB_ATTR_S *pstPubAttr);

HI_S32 HI_MPI_ISP_SetHvSync(ISP_DEV IspDev, const ISP_SLAVE_SNS_SYNC_S *pstSnsSync);
HI_S32 HI_MPI_ISP_GetHvSync(ISP_DEV IspDev, ISP_SLAVE_SNS_SYNC_S *pstSnsSync);

HI_S32 HI_MPI_ISP_SetFMWState(ISP_DEV IspDev, const ISP_FMW_STATE_E enState);
HI_S32 HI_MPI_ISP_GetFMWState(ISP_DEV IspDev, ISP_FMW_STATE_E *penState);

HI_S32 HI_MPI_ISP_SetWDRMode(ISP_DEV IspDev, const ISP_WDR_MODE_S *pstWDRMode);
HI_S32 HI_MPI_ISP_GetWDRMode(ISP_DEV IspDev, ISP_WDR_MODE_S *pstWDRMode);

HI_S32 HI_MPI_ISP_SetModuleControl(ISP_DEV IspDev, const ISP_MODULE_CTRL_U *punModCtrl);
HI_S32 HI_MPI_ISP_GetModuleControl(ISP_DEV IspDev, ISP_MODULE_CTRL_U *punModCtrl);

/* General Function Settings */
HI_S32 HI_MPI_ISP_SetDRCAttr(ISP_DEV IspDev, const ISP_DRC_ATTR_S *pstDRC);
HI_S32 HI_MPI_ISP_GetDRCAttr(ISP_DEV IspDev, ISP_DRC_ATTR_S *pstDRC);

HI_S32 HI_MPI_ISP_SetFSWDRAttr(ISP_DEV IspDev, const ISP_WDR_FS_ATTR_S *pstFSWDRAttr);
HI_S32 HI_MPI_ISP_GetFSWDRAttr(ISP_DEV IspDev, ISP_WDR_FS_ATTR_S *pstFSWDRAttr);

HI_S32 HI_MPI_ISP_SetInputFormatterAttr(ISP_DEV IspDev, const ISP_INPUT_FORMATTER_ATTR_S *pstIFAttr);
HI_S32 HI_MPI_ISP_GetInputFormatterAttr(ISP_DEV IspDev, ISP_INPUT_FORMATTER_ATTR_S *pstIFAttr);

HI_S32 HI_MPI_ISP_SetDPCalibrate(ISP_DEV IspDev, const ISP_DP_STATIC_CALIBRATE_S *pstDPCalibrate);
HI_S32 HI_MPI_ISP_GetDPCalibrate(ISP_DEV IspDev, ISP_DP_STATIC_CALIBRATE_S *pstDPCalibrate);

HI_S32 HI_MPI_ISP_SetDPAttr(ISP_DEV IspDev, const ISP_DP_ATTR_S *pstDPAttr);
HI_S32 HI_MPI_ISP_GetDPAttr(ISP_DEV IspDev, ISP_DP_ATTR_S *pstDPAttr);

HI_S32 HI_MPI_ISP_SetDISAttr(ISP_DEV IspDev, const ISP_DIS_ATTR_S *pstDISAttr);
HI_S32 HI_MPI_ISP_GetDISAttr(ISP_DEV IspDev, ISP_DIS_ATTR_S *pstDISAttr);

HI_S32 HI_MPI_ISP_SetRadialShadingAttr(ISP_DEV IspDev, const ISP_RADIAL_SHADING_ATTR_S *pstRadialShadingAttr);
HI_S32 HI_MPI_ISP_GetRadialShadingAttr(ISP_DEV IspDev, ISP_RADIAL_SHADING_ATTR_S *pstRadialShadingAttr);

HI_S32 HI_MPI_ISP_SetMeshShadingAttr(ISP_DEV IspDev, const ISP_MESH_SHADING_ATTR_S *pstMeshShadingAttr);
HI_S32 HI_MPI_ISP_GetMeshShadingAttr(ISP_DEV IspDev, ISP_MESH_SHADING_ATTR_S *pstMeshShadingAttr);

HI_S32 HI_MPI_ISP_SetNRAttr(ISP_DEV IspDev, const ISP_NR_ATTR_S *pstNRAttr);
HI_S32 HI_MPI_ISP_GetNRAttr(ISP_DEV IspDev, ISP_NR_ATTR_S *pstNRAttr);

HI_S32 HI_MPI_ISP_SetNPTable(ISP_DEV IspDev, const ISP_NP_TABLE_S *pstNPTable);
HI_S32 HI_MPI_ISP_GetNPTable(ISP_DEV IspDev, ISP_NP_TABLE_S *pstNPTable);

HI_S32 HI_MPI_ISP_SetColorToneAttr(ISP_DEV IspDev, const ISP_COLOR_TONE_ATTR_S *pstCTAttr);
HI_S32 HI_MPI_ISP_GetColorToneAttr(ISP_DEV IspDev, ISP_COLOR_TONE_ATTR_S *pstCTAttr);

HI_S32 HI_MPI_ISP_SetGammaAttr(ISP_DEV IspDev, const ISP_GAMMA_ATTR_S *pstGammaAttr);
HI_S32 HI_MPI_ISP_GetGammaAttr(ISP_DEV IspDev, ISP_GAMMA_ATTR_S *pstGammaAttr);

HI_S32 HI_MPI_ISP_SetGammaFEAttr(ISP_DEV IspDev, const ISP_GAMMAFE_ATTR_S *pstGammaFEAttr);
HI_S32 HI_MPI_ISP_GetGammaFEAttr(ISP_DEV IspDev, ISP_GAMMAFE_ATTR_S *pstGammaFEAttr);

HI_S32 HI_MPI_ISP_SetBayerSharpenAttr(ISP_DEV IspDev, const ISP_BAYER_SHARPEN_ATTR_S *pstBayerShpAttr);
HI_S32 HI_MPI_ISP_GetBayerSharpenAttr(ISP_DEV IspDev, ISP_BAYER_SHARPEN_ATTR_S *pstBayerShpAttr);

HI_S32 HI_MPI_ISP_SetYuvSharpenAttr(ISP_DEV IspDev, const ISP_YUV_SHARPEN_ATTR_S *pstYuvShpAttr);
HI_S32 HI_MPI_ISP_GetYuvSharpenAttr(ISP_DEV IspDev, ISP_YUV_SHARPEN_ATTR_S *pstYuvShpAttr);

HI_S32 HI_MPI_ISP_SetCrosstalkAttr(ISP_DEV IspDev, const ISP_CR_ATTR_S *pstCRAttr);
HI_S32 HI_MPI_ISP_GetCrosstalkAttr(ISP_DEV IspDev, ISP_CR_ATTR_S *pstCRAttr);

HI_S32 HI_MPI_ISP_SetAntiFalseColorAttr(ISP_DEV IspDev, const ISP_ANTI_FALSECOLOR_S *pstAntiFC);
HI_S32 HI_MPI_ISP_GetAntiFalseColorAttr(ISP_DEV IspDev, ISP_ANTI_FALSECOLOR_S *pstAntiFC);

HI_S32 HI_MPI_ISP_SetDemosaicAttr(ISP_DEV IspDev, const ISP_DEMOSAIC_ATTR_S *pstDemosaicAttr);
HI_S32 HI_MPI_ISP_GetDemosaicAttr(ISP_DEV IspDev, ISP_DEMOSAIC_ATTR_S *pstDemosaicAttr);

HI_S32 HI_MPI_ISP_SetBlackLevelAttr(ISP_DEV IspDev, const ISP_BLACK_LEVEL_S *pstBlackLevel);
HI_S32 HI_MPI_ISP_GetBlackLevelAttr(ISP_DEV IspDev, ISP_BLACK_LEVEL_S *pstBlackLevel);

/* block, return fpn frame when this is function is called. */
HI_S32 HI_MPI_ISP_FPNCalibrate(ISP_DEV IspDev, ISP_FPN_CALIBRATE_ATTR_S *pstCalibrateAttr);
HI_S32 HI_MPI_ISP_SetFPNAttr(ISP_DEV IspDev, const ISP_FPN_ATTR_S *pstFPNAttr);
HI_S32 HI_MPI_ISP_GetFPNAttr(ISP_DEV IspDev, ISP_FPN_ATTR_S *pstFPNAttr);

HI_S32 HI_MPI_ISP_SetDeFogAttr(ISP_DEV IspDev, const ISP_DEFOG_ATTR_S *pstDefogAttr);
HI_S32 HI_MPI_ISP_GetDeFogAttr(ISP_DEV IspDev, ISP_DEFOG_ATTR_S *pstDefogAttr);

HI_S32 HI_MPI_ISP_SetDCFInfo(ISP_DEV IspDev, const ISP_DCF_INFO_S *pstIspDCF);
HI_S32 HI_MPI_ISP_GetDCFInfo(ISP_DEV IspDev, ISP_DCF_INFO_S *pstIspDCF);

HI_S32 HI_MPI_ISP_SetAcmAttr(ISP_DEV IspDev, ISP_ACM_ATTR_S *pstAcmAttr);
HI_S32 HI_MPI_ISP_GetAcmAttr(ISP_DEV IspDev, ISP_ACM_ATTR_S *pstAcmAttr);

HI_S32 HI_MPI_ISP_SetAcmCoeff(ISP_DEV IspDev, ISP_ACM_LUT_S *pstAcmCoef);
HI_S32 HI_MPI_ISP_GetAcmCoeff(ISP_DEV IspDev, ISP_ACM_LUT_S *pstAcmCoef);

HI_S32 HI_MPI_ISP_QueryInnerStateInfo(ISP_DEV IspDev, ISP_INNER_STATE_INFO_S *pstInnerStateInfo);

HI_S32 HI_MPI_ISP_SetStatisticsConfig(ISP_DEV IspDev, const ISP_STATISTICS_CFG_S *pstStatCfg);
HI_S32 HI_MPI_ISP_GetStatisticsConfig(ISP_DEV IspDev, ISP_STATISTICS_CFG_S *pstStatCfg);
HI_S32 HI_MPI_ISP_GetStatistics(ISP_DEV IspDev, ISP_STATISTICS_S *pstStat);

HI_S32 HI_MPI_ISP_SetRegister(ISP_DEV IspDev, HI_U32 u32Addr, HI_U32 u32Value);
HI_S32 HI_MPI_ISP_GetRegister(ISP_DEV IspDev, HI_U32 u32Addr, HI_U32 *pu32Value);

HI_S32 HI_MPI_ISP_SetMode(ISP_DEV IspDev, const ISP_MODE_E enIspMode);
HI_S32 HI_MPI_ISP_GetMode(ISP_DEV IspDev, ISP_MODE_E *penIspMode);

HI_S32 HI_MPI_ISP_GetVDTimeOut(ISP_DEV IspDev, ISP_VD_INFO_S *pstIspVdInfo, HI_U32 u32MilliSec);
HI_S32 HI_MPI_ISP_GetVDEndTimeOut(ISP_DEV IspDev, ISP_VD_INFO_S *pstIspVdInfo, HI_U32 u32MilliSec);

HI_S32 HI_MPI_ISP_GetISPRegAttr(ISP_DEV IspDev, ISP_REG_ATTR_S * pstIspRegAttr);

HI_S32 HI_MPI_ISP_SetDebug(ISP_DEV IspDev, const ISP_DEBUG_INFO_S * pstIspDebug);
HI_S32 HI_MPI_ISP_GetDebug(ISP_DEV IspDev, ISP_DEBUG_INFO_S * pstIspDebug);

HI_S32 HI_MPI_ISP_SetModParam(ISP_DEV IspDev, const ISP_MOD_PARAM_S *pstIspModParam);
HI_S32 HI_MPI_ISP_GetModParam(ISP_DEV IspDev, ISP_MOD_PARAM_S *pstIspModParam);

HI_S32 HI_MPI_ISP_SetSnapAttr(ISP_DEV IspDev, const ISP_SNAP_ATTR_S *pstSnapAttr);
HI_S32 HI_MPI_ISP_GetSnapAttr(ISP_DEV IspDev, ISP_SNAP_ATTR_S *pstSnapAttr);

HI_S32 HI_MPI_ISP_MeshShadingCalibration(HI_U16* pu16SrcRaw, 
    ISP_LSC_CALIBRATION_CFG_S* pstLSCCaliCfg , ISP_MESH_SHADING_TABLE_S stMeshShadingResult);

HI_S32 HI_MPI_ISP_GetLightboxGain(ISP_DEV IspDev, ISP_AWB_Calibration_Gain_S *pstAWBCalibrationGain);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif /*__MPI_ISP_H__ */

