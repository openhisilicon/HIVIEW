
/******************************************************************************
Copyright (C), 2016-2011, Hisilicon Tech. Co., Ltd.
******************************************************************************
File Name     : mpi_region.h
Version       : Initial Draft
Author        : Hisilicon multimedia software group
Created       : 2016/11/15
Last Modified :
Description   : MPP Programe Interface for region moudle
Function List :
******************************************************************************/


#ifndef __MPI_REGION_H__
#define __MPI_REGION_H__

#include "hi_comm_region.h"

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* End of #ifdef __cplusplus */

HI_S32 HI_MPI_RGN_Create(RGN_HANDLE Handle, const RGN_ATTR_S *pstRegion);
HI_S32 HI_MPI_RGN_Destroy(RGN_HANDLE Handle);

HI_S32 HI_MPI_RGN_GetAttr(RGN_HANDLE Handle, RGN_ATTR_S *pstRegion);
HI_S32 HI_MPI_RGN_SetAttr(RGN_HANDLE Handle, const RGN_ATTR_S *pstRegion);

HI_S32 HI_MPI_RGN_SetBitMap(RGN_HANDLE Handle, const BITMAP_S *pstBitmap);

HI_S32 HI_MPI_RGN_AttachToChn(RGN_HANDLE Handle, const MPP_CHN_S *pstChn, const RGN_CHN_ATTR_S *pstChnAttr);
HI_S32 HI_MPI_RGN_DetachFromChn(RGN_HANDLE Handle, const MPP_CHN_S *pstChn);

HI_S32 HI_MPI_RGN_SetDisplayAttr(RGN_HANDLE Handle, const MPP_CHN_S *pstChn, const RGN_CHN_ATTR_S *pstChnAttr);
HI_S32 HI_MPI_RGN_GetDisplayAttr(RGN_HANDLE Handle, const MPP_CHN_S *pstChn, RGN_CHN_ATTR_S *pstChnAttr);

HI_S32 HI_MPI_RGN_GetCanvasInfo(RGN_HANDLE Handle, RGN_CANVAS_INFO_S *pstCanvasInfo);
HI_S32 HI_MPI_RGN_UpdateCanvas(RGN_HANDLE Handle);

HI_S32 HI_MPI_RGN_BatchBegin(RGN_HANDLEGROUP *pu32Group,HI_U32 u32Num,const RGN_HANDLE handle[]);
HI_S32 HI_MPI_RGN_BatchEnd(RGN_HANDLEGROUP u32Group);

HI_S32 HI_MPI_RGN_GetFd(HI_VOID);
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif /* End of #ifndef __MPI_REGION_H__ */

