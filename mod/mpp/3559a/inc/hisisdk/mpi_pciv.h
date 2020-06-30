/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2009-2019. All rights reserved.
 * Description   : mpi functions declaration
 * Author        : Hisilicon Hi3511 MPP Team
 * Created       : 2009/06/23
 */
#ifndef __MPI_PCIV_H__
#define __MPI_PCIV_H__

#include "hi_comm_pciv.h"

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

/*****************************************************************************
 Description     : Create and initialize the pciv channel.
 Input           : pcivChn      ** The pciv channel id between [0, PCIV_MAX_CHN_NUM)
                   pPcivAttr    ** The attribute of pciv channel
 Output          : None
 Return Value    : HI_SUCCESS if success.
                   HI_FAILURE if failure

 See Also        : HI_MPI_PCIV_Destroy
*****************************************************************************/
HI_S32 HI_MPI_PCIV_Create(PCIV_CHN pcivChn, const PCIV_ATTR_S *pPcivAttr);

/*****************************************************************************
 Description     : Destroy the pciv channel
 Input           : pcivChn  ** The pciv channel id
 Output          : None
 Return Value    : HI_SUCCESS if success.
                   HI_FAILURE if failure

 See Also        : HI_MPI_PCIV_Create
*****************************************************************************/
HI_S32 HI_MPI_PCIV_Destroy(PCIV_CHN pcivChn);

/*****************************************************************************
 Description     : Set the attribute of pciv channel
 Input           : pcivChn    ** The pciv channel id
                   pPcivAttr  ** The attribute of pciv channel
 Output          : None
 Return Value    : HI_SUCCESS if success.
                   HI_FAILURE if failure

 See Also        : HI_MPI_PCIV_GetAttr
*****************************************************************************/
HI_S32 HI_MPI_PCIV_SetAttr(PCIV_CHN pcivChn, const PCIV_ATTR_S *pPcivAttr);

/*****************************************************************************
 Description     : Get the attribute of pciv channel
 Input           : pcivChn    ** The pciv channel id
 Output          : pPcivAttr  ** The attribute of pciv channel
 Return Value    : HI_SUCCESS if success.
                   HI_FAILURE if failure

 See Also        : HI_MPI_PCIV_SetAttr
*****************************************************************************/
HI_S32 HI_MPI_PCIV_GetAttr(PCIV_CHN pcivChn, PCIV_ATTR_S *pPcivAttr);


/*****************************************************************************
 Description     : Start to send or receive video frame
 Input           : pcivChn    ** The pciv channel id
 Output          : None
 Return Value    : HI_SUCCESS if success.
                   HI_FAILURE if failure

 See Also        : HI_MPI_PCIV_Stop
*****************************************************************************/
HI_S32 HI_MPI_PCIV_Start(PCIV_CHN pcivChn);

/*****************************************************************************
 Description     : Stop send or receive video frame
 Input           : pcivChn    ** The pciv channel id
 Output          : None
 Return Value    : HI_SUCCESS if success.
                   HI_FAILURE if failure

 See Also        : HI_MPI_PCIV_Start
*****************************************************************************/
HI_S32 HI_MPI_PCIV_Stop(PCIV_CHN pcivChn);

/*****************************************************************************
 Description     : Create a series of dma task
 Input           : pTask    ** The task list to create
 Output          : None
 Return Value    : HI_SUCCESS if success.
                   HI_FAILURE if failure

 See Also        : None
*****************************************************************************/
HI_S32  HI_MPI_PCIV_DmaTask(PCIV_DMA_TASK_S *pTask);

/*****************************************************************************
 Description     : Alloc 'u32BlkSize' bytes memory and give the physical address
                   The memory used by PCI must be located within the PCI window,
                   So you should call this function to alloc it.
 Input           : u32BlkSize    ** The size of each memory block
                   u32BlkCnt     ** The count of memory block
 Output          : u64PhyAddr    ** The physical address of the memory
 Return Value    : HI_SUCCESS if success.
                   HI_FAILURE if failure

 See Also        : None
*****************************************************************************/
HI_S32  HI_MPI_PCIV_Malloc(HI_U32 u32BlkSize, HI_U32 u32BlkCnt, HI_U64 au64PhyAddr[]);

/*****************************************************************************
 Description     : free memory with the physical address
 Input             : u32BlkCnt      ** The count of memory block
                     u64PhyAddr     ** The physical address of the memory
 Output           : None
 Return Value    : HI_SUCCESS if success.
                   HI_FAILURE if failure

 See Also        : None
*****************************************************************************/
HI_S32  HI_MPI_PCIV_Free(HI_U32 u32BlkCnt, const HI_U64 au64PhyAddr[]);

/*****************************************************************************
 Description     : Alloc 'u32BlkSize' bytes memory and give the physical address for one pciv channel
                   The memory used by PCI host, pciv slave cann't use this interface.
 Input           : pcivChn       **pciv channel
                   u32BlkSize    ** The size of each memory block
                   u32BlkCnt     ** The count of memory block
 Output          : u64PhyAddr    ** The physical address of the memory
 Return Value    : HI_SUCCESS if success.
                   HI_FAILURE if failure

 See Also        : None
*****************************************************************************/
HI_S32  HI_MPI_PCIV_MallocChnBuffer(PCIV_CHN pcivChn, HI_U32 u32BlkSize, HI_U32 u32BlkCnt, HI_U64 au64PhyAddr[]);

/*****************************************************************************
 Description     : free pciv channel memory with the physical address
                   The memory used by PCI host, pciv slave cann't use this interface.
 Input           : pcivChn    ** pciv channel
                   u32BlkCnt  ** The count of memory block
 Output          : None
 Return Value    : HI_SUCCESS if success.
                   HI_FAILURE if failure

 See Also        : None
*****************************************************************************/
HI_S32  HI_MPI_PCIV_FreeChnBuffer(PCIV_CHN pcivChn, HI_U32 u32BlkCnt);


/*****************************************************************************
 Description     : Get the board ID of this board
 Input           : None
 Output          : None
 Return Value    : The board ID if success
                   HI_FAILURE or HI_ERR_PCIV_SYS_NOTREADY  if failure

 See Also        : HI_MPI_PCIV_GetBaseWindow
*****************************************************************************/
HI_S32  HI_MPI_PCIV_GetLocalId(HI_VOID);

/*****************************************************************************
 Description     : Enum all the connected chip. Need check the invalid value (-1)
 Input           : s32ChipID  ** The chip id array
 Output          : None
 Return Value    : HI_SUCCESS if success.
                   HI_FAILURE if failure

 See Also        : HI_MPI_PCIV_GetLocalId
                   HI_MPI_PCIV_GetBaseWindow
*****************************************************************************/
HI_S32 HI_MPI_PCIV_EnumChip(HI_S32 as32ChipID[PCIV_MAX_CHIPNUM]);

/*****************************************************************************
 Description  : On the host, you can get all the slave borad's NP,PF and CFG window
                On the slave, you can only get the PF AHB Addres of itself.
 Input        : s32ChipId     ** The chip Id which you want to access
 Output       : pBase         ** On host  pBase->u32NpWinBase,
                                          pBase->u32PfWinBase,
                                          pBase->u32CfgWinBase
                                 On Slave pBase->u32PfAHBAddr
 Return Value : HI_SUCCESS if success.
                HI_FAILURE if failure

 See Also     : HI_MPI_PCIV_GetLocalId
*****************************************************************************/
HI_S32  HI_MPI_PCIV_GetBaseWindow(HI_S32 s32ChipId, PCIV_BASEWINDOW_S *pBase);

/*****************************************************************************
 Description  : Only on the slave chip, you need to create some VB Pool.
                Those pool will bee created on the PCI Window Zone.
 Input        : pCfg.u32PoolCount ** The total number of pool want to create
                pCfg.u32BlkSize[] ** The size of each VB block
                pCfg.u32BlkCount[]** The number of each VB block

 Output       : None
 Return Value : HI_SUCCESS if success.
                HI_ERR_PCIV_SYS_NOTREADY
                HI_ERR_PCIV_NULL_PTR
                HI_ERR_PCIV_NOMEM
                HI_ERR_PCIV_BUSY
                HI_ERR_PCIV_NOT_SUPPORT
                HI_FAILURE

 See Also     : HI_MPI_PCIV_GetLocalId
*****************************************************************************/
HI_S32 HI_MPI_PCIV_WinVbCreate(const PCIV_WINVBCFG_S *pCfg);

/*****************************************************************************
 Description  : Destroy the pools which's size is equal to the pCfg.u32BlkSize[]
 Input        : pCfg.u32PoolCount ** The total number of pool want to destroy
                pCfg.u32BlkSize[] ** The size of each VB block
                pCfg.u32BlkCount[]** Don't care this parament

 Output       : None
 Return Value : HI_SUCCESS if success.
                HI_ERR_PCIV_SYS_NOTREADY
                HI_ERR_PCIV_NOT_SUPPORT
                HI_FAILURE

 See Also     : HI_MPI_PCIV_GetLocalId
*****************************************************************************/
HI_S32 HI_MPI_PCIV_WinVbDestroy(HI_VOID);

/*****************************************************************************
 Description  : Show the image in VO device from PCI
 Input        : pcivChn ** The pciv channel id
 Output       : None
 Return Value :
*****************************************************************************/
HI_S32 HI_MPI_PCIV_Show(PCIV_CHN pcivChn);

/*****************************************************************************
 Description  : Hide the image in VO device from PCI
 Input        : pcivChn ** The pciv channel id
 Output       : None
 Return Value :
*****************************************************************************/
HI_S32 HI_MPI_PCIV_Hide(PCIV_CHN pcivChn);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __MPI_VENC_H__ */

