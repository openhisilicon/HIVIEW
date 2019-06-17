/******************************************************************************

  Copyright (C), 2001-2011, Hisilicon Tech. Co., Ltd.

 ******************************************************************************
  File Name     : mpi_awb.h
  Version       : Initial Draft
  Author        : Hisilicon multimedia software group
  Created       : 2012/12/19
  Description   : 
  History       :
  1.Date        : 2012/12/19
    Author      : n00168968
    Modification: Created file

******************************************************************************/
#ifndef __MPI_AWB_H__
#define __MPI_AWB_H__

#include "hi_comm_isp.h"
#include "hi_comm_3a.h"
#include "hi_awb_comm.h"

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* End of #ifdef __cplusplus */

/* The interface of awb lib register to isp. */
HI_S32 HI_MPI_AWB_Register(ISP_DEV IspDev, ALG_LIB_S *pstAwbLib);
HI_S32 HI_MPI_AWB_UnRegister(ISP_DEV IspDev, ALG_LIB_S *pstAwbLib);

/* The callback function of sensor register to awb lib. */
HI_S32 HI_MPI_AWB_SensorRegCallBack(ISP_DEV IspDev, ALG_LIB_S *pstAwbLib, SENSOR_ID SensorId,
    AWB_SENSOR_REGISTER_S *pstRegister);
HI_S32 HI_MPI_AWB_SensorUnRegCallBack(ISP_DEV IspDev, ALG_LIB_S *pstAwbLib, SENSOR_ID SensorId);

HI_S32 HI_MPI_ISP_SetWBAttr(ISP_DEV IspDev, const ISP_WB_ATTR_S *pstWBAttr);
HI_S32 HI_MPI_ISP_GetWBAttr(ISP_DEV IspDev, ISP_WB_ATTR_S *pstWBAttr);

HI_S32 HI_MPI_ISP_SetAWBAttrEx(ISP_DEV IspDev, ISP_AWB_ATTR_EX_S *pstAWBAttrEx);
HI_S32 HI_MPI_ISP_GetAWBAttrEx(ISP_DEV IspDev, ISP_AWB_ATTR_EX_S *pstAWBAttrEx);

HI_S32 HI_MPI_ISP_SetCCMAttr(ISP_DEV IspDev, const ISP_COLORMATRIX_ATTR_S *pstCCMAttr);
HI_S32 HI_MPI_ISP_GetCCMAttr(ISP_DEV IspDev, ISP_COLORMATRIX_ATTR_S *pstCCMAttr);

HI_S32 HI_MPI_ISP_SetSaturationAttr(ISP_DEV IspDev, const ISP_SATURATION_ATTR_S *pstSatAttr);
HI_S32 HI_MPI_ISP_GetSaturationAttr(ISP_DEV IspDev, ISP_SATURATION_ATTR_S *pstSatAttr);

HI_S32 HI_MPI_ISP_QueryWBInfo(ISP_DEV IspDev, ISP_WB_INFO_S *pstWBInfo);
HI_S32 HI_MPI_ISP_CalGainByTemp(ISP_DEV IspDev, const ISP_WB_ATTR_S *pstWBAttr, HI_U16 u16ColorTemp, HI_S16 s16Shift, HI_U16 *pu16AWBGain);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif

