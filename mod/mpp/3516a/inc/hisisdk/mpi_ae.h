/******************************************************************************

  Copyright (C), 2001-2011, Hisilicon Tech. Co., Ltd.

 ******************************************************************************
  File Name     : mpi_ae.h
  Version       : Initial Draft
  Author        : Hisilicon multimedia software group
  Created       : 2014/04/01
  Description   : 
  History       :
  1.Date        : 2014/04/01
    Author      : h00191408
    Modification: Created file

******************************************************************************/
#ifndef __MPI_AE_H__
#define __MPI_AE_H__

#include "hi_comm_isp.h"
#include "hi_comm_3a.h"
#include "hi_ae_comm.h"

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* End of #ifdef __cplusplus */


/* The interface of ae lib register to isp. */
HI_S32 HI_MPI_AE_Register(ISP_DEV IspDev, ALG_LIB_S *pstAeLib);
HI_S32 HI_MPI_AE_UnRegister(ISP_DEV IspDev, ALG_LIB_S *pstAeLib);

/* The callback function of sensor register to ae lib. */
HI_S32 HI_MPI_AE_SensorRegCallBack(ISP_DEV IspDev, ALG_LIB_S *pstAeLib, SENSOR_ID SensorId,
    AE_SENSOR_REGISTER_S *pstRegister);
HI_S32 HI_MPI_AE_SensorUnRegCallBack(ISP_DEV IspDev, ALG_LIB_S *pstAeLib, SENSOR_ID SensorId);
    
HI_S32 HI_MPI_ISP_SetExposureAttr(ISP_DEV IspDev, const ISP_EXPOSURE_ATTR_S *pstExpAttr);
HI_S32 HI_MPI_ISP_GetExposureAttr(ISP_DEV IspDev, ISP_EXPOSURE_ATTR_S *pstExpAttr);

HI_S32 HI_MPI_ISP_SetWDRExposureAttr(ISP_DEV IspDev, const ISP_WDR_EXPOSURE_ATTR_S *pstWDRExpAttr);
HI_S32 HI_MPI_ISP_GetWDRExposureAttr(ISP_DEV IspDev, ISP_WDR_EXPOSURE_ATTR_S *pstWDRExpAttr);

HI_S32 HI_MPI_ISP_SetAERouteAttr(ISP_DEV IspDev, const ISP_AE_ROUTE_S *pstAERouteAttr);
HI_S32 HI_MPI_ISP_GetAERouteAttr(ISP_DEV IspDev, ISP_AE_ROUTE_S *pstAERouteAttr);

HI_S32 HI_MPI_ISP_QueryExposureInfo(ISP_DEV IspDev, ISP_EXP_INFO_S *pstExpInfo);

HI_S32 HI_MPI_ISP_SetIrisAttr(ISP_DEV IspDev, const ISP_IRIS_ATTR_S *pstIrisAttr);
HI_S32 HI_MPI_ISP_GetIrisAttr(ISP_DEV IspDev, ISP_IRIS_ATTR_S *pstIrisAttr);

HI_S32 HI_MPI_ISP_SetDcirisAttr(ISP_DEV IspDev, const ISP_DCIRIS_ATTR_S *pstDcirisAttr);
HI_S32 HI_MPI_ISP_GetDcirisAttr(ISP_DEV IspDev, ISP_DCIRIS_ATTR_S *pstDcirisAttr);

HI_S32 HI_MPI_ISP_SetPirisAttr(ISP_DEV IspDev, const ISP_PIRIS_ATTR_S *pstPirisAttr);
HI_S32 HI_MPI_ISP_GetPirisAttr(ISP_DEV IspDev, ISP_PIRIS_ATTR_S *pstPirisAttr);

HI_S32 HI_MPI_ISP_SetAERouteAttrEx(ISP_DEV IspDev, const ISP_AE_ROUTE_EX_S *pstAERouteAttrEx);
HI_S32 HI_MPI_ISP_GetAERouteAttrEx(ISP_DEV IspDev, ISP_AE_ROUTE_EX_S *pstAERouteAttrEx);




#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif


