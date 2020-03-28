
/******************************************************************************
Copyright (C), 2016, Hisilicon Tech. Co., Ltd.
******************************************************************************
File Name     : mpi_vb.h
Version       : Initial Draft
Author        : Hisilicon multimedia software group
Created       : 2016/07/15
Last Modified :
Description   : MPP Programe Interface for video buffer management moudle
Function List :
******************************************************************************/

#ifndef __MPI_VB_H__
#define __MPI_VB_H__

#include "hi_comm_vb.h"
#include "hi_comm_video.h"

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* End of #ifdef __cplusplus */

VB_POOL HI_MPI_VB_CreatePool(VB_POOL_CONFIG_S *pstVbPoolCfg);
HI_S32 HI_MPI_VB_DestroyPool(VB_POOL Pool);

VB_BLK HI_MPI_VB_GetBlock(VB_POOL Pool, HI_U64 u64BlkSize,const HI_CHAR *pcMmzName);
HI_S32 HI_MPI_VB_ReleaseBlock(VB_BLK Block);

VB_BLK HI_MPI_VB_PhysAddr2Handle(HI_U64 u64PhyAddr);
HI_U64 HI_MPI_VB_Handle2PhysAddr(VB_BLK Block);
VB_POOL HI_MPI_VB_Handle2PoolId(VB_BLK Block);

HI_S32 HI_MPI_VB_InquireUserCnt(VB_BLK Block);

HI_S32 HI_MPI_VB_GetSupplementAddr(VB_BLK Block, VIDEO_SUPPLEMENT_S *pstSupplement);
HI_S32 HI_MPI_VB_SetSupplementConfig(const VB_SUPPLEMENT_CONFIG_S *pstSupplementConfig);
HI_S32 HI_MPI_VB_GetSupplementConfig(VB_SUPPLEMENT_CONFIG_S *pstSupplementConfig);


HI_S32 HI_MPI_VB_Init(HI_VOID);
HI_S32 HI_MPI_VB_Exit(HI_VOID);
HI_S32 HI_MPI_VB_SetConfig(const VB_CONFIG_S *pstVbConfig);
HI_S32 HI_MPI_VB_GetConfig(VB_CONFIG_S *pstVbConfig);

HI_S32 HI_MPI_VB_MmapPool(VB_POOL Pool);
HI_S32 HI_MPI_VB_MunmapPool(VB_POOL Pool);

HI_S32 HI_MPI_VB_GetBlockVirAddr(VB_POOL Pool, HI_U64 u64PhyAddr, HI_VOID **ppVirAddr);

HI_S32 HI_MPI_VB_InitModCommPool(VB_UID_E enVbUid);
HI_S32 HI_MPI_VB_ExitModCommPool(VB_UID_E enVbUid);

HI_S32 HI_MPI_VB_SetModPoolConfig(VB_UID_E enVbUid, const VB_CONFIG_S *pstVbConfig);
HI_S32 HI_MPI_VB_GetModPoolConfig(VB_UID_E enVbUid, VB_CONFIG_S *pstVbConfig);


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif /*__MPI_VI_H__ */

