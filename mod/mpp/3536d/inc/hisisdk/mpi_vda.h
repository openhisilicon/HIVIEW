/******************************************************************************

  Copyright (C), 2001-2011, Hisilicon Tech. Co., Ltd.

 ******************************************************************************
  File Name     : mpi_vda.h
  Version       : Initial Draft
  Author        : Hisilicon multimedia software group
  Created       : 2013/05/09
  Description   : 
  History       :
  1.Date        : 2013/05/09
    Author      : x00100808
    Modification: Created file

******************************************************************************/
#ifndef __MPI_VDA_H__
#define __MPI_VDA_H__

#include "hi_comm_vda.h"

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* End of #ifdef __cplusplus */

HI_S32 HI_MPI_VDA_CreateChn(VDA_CHN VdaChn, const VDA_CHN_ATTR_S *pstAttr);
HI_S32 HI_MPI_VDA_DestroyChn(VDA_CHN VdaChn);

HI_S32 HI_MPI_VDA_GetChnAttr(VDA_CHN VdaChn, VDA_CHN_ATTR_S *pstAttr);
HI_S32 HI_MPI_VDA_SetChnAttr(VDA_CHN VdaChn, const VDA_CHN_ATTR_S *pstAttr);

HI_S32 HI_MPI_VDA_StartRecvPic(VDA_CHN VdaChn);
HI_S32 HI_MPI_VDA_StopRecvPic(VDA_CHN VdaChn);

HI_S32 HI_MPI_VDA_GetData(VDA_CHN VdaChn, VDA_DATA_S *pstVdaData, HI_S32 s32MilliSec);
HI_S32 HI_MPI_VDA_ReleaseData(VDA_CHN VdaChn, const VDA_DATA_S* pstVdaData);

HI_S32 HI_MPI_VDA_ResetOdRegion(VDA_CHN VdaChn, HI_S32 s32RgnIndex);

HI_S32 HI_MPI_VDA_Query(VDA_CHN VdaChn, VDA_CHN_STAT_S *pstChnStat);

HI_S32 HI_MPI_VDA_GetFd(VDA_CHN VdaChn);

HI_S32 HI_MPI_VDA_UpdateRef(VDA_CHN VdaChn, const VIDEO_FRAME_INFO_S *pstRefFrame);

HI_S32 HI_MPI_VDA_SendPic(VDA_CHN VdaChn, const VIDEO_FRAME_INFO_S *pstUserFrame, HI_S32 s32MilliSec);


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif /* End of #ifndef __MPI_VDA_H__ */

