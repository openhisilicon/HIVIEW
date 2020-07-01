/******************************************************************************

  Copyright (C), 2017-2018, Hisilicon Tech. Co., Ltd.

 ******************************************************************************
  File Name     : mpi_nnie.h
  Version       : Initial Draft
  Author        : Hisilicon multimedia software (SVP) group
  Created       : 2017/06/06
  Description   :
  History       :
  1.Date        : 2017/06/06
    Author      :
    Modification: Created file

******************************************************************************/
#ifndef _HI_MPI_NNIE_H_
#define _HI_MPI_NNIE_H_

#include "hi_nnie.h"

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* End of #ifdef __cplusplus */

/*****************************************************************************
*   Prototype    : HI_MPI_SVP_NNIE_LoadModel
*   Description  : Load cnn model
*   Parameters   : const SVP_SRC_MEM_INFO_S   *pstModelBuf    Input model buf, can't be freed
*                                                               until invoke the unload function.
*                  SVP_NNIE_MODEL_S           *pstModel       Output model struct.
*   Return Value : HI_SUCCESS: Success; Error codes: Failure.
*   Spec         :
*   History:
*
*       1.  Date         : 2017-10-06
*          Author        :
*          Modification  : Created function
*
*****************************************************************************/
HI_S32 HI_MPI_SVP_NNIE_LoadModel(const SVP_SRC_MEM_INFO_S *pstModelBuf, SVP_NNIE_MODEL_S *pstModel);

/*****************************************************************************
*   Prototype    : HI_MPI_SVP_NNIE_GetTskBufSize
*   Description  : Get task buffer size
*   Parameters   : HI_U32            u32MaxInputNum   Maximum input num,  CNN_Forword input data num cann't
*                                                        be more than it.
*                  HI_U32            u32MaxBboxNum    Maximum Bbox num, the RPN's output bbox num, should
*                                                        be less or equal to the compiler's correspond value.
*                  SVP_NNIE_MODEL_S  *pstModel        The model from Loadmodel.
*                  HI_U32            au32TskBufSize[] The task relate auxiliary buffer array.
*                  HI_U32            u32NetSegNum     The au32TskBufSize array element num.
*   Return Value: HI_SUCCESS: Success; Error codes: Failure.
*   Spec         :
*   History:
*
*       1.  Date         : 2017-10-06
*          Author        :
*          Modification  : Created function
*
*****************************************************************************/
HI_S32 HI_MPI_SVP_NNIE_GetTskBufSize(HI_U32 u32MaxInputNum, HI_U32 u32MaxBboxNum,
    const SVP_NNIE_MODEL_S *pstModel, HI_U32 au32TskBufSize[], HI_U32 u32NetSegNum);

/*****************************************************************************
*   Prototype    : HI_MPI_SVP_NNIE_Forward
*   Description  : Perform CNN prediction on input sample(s), and output responses for corresponding sample(s)
*   Parameters   : SVP_NNIE_HANDLE                  *phSvpNnieHandle      Returned handle ID of a task
*                  const SVP_SRC_BLOB_S              astSrc[]             Input node array.
*                  const SVP_NNIE_MODEL_S            pstModel             CNN model data
*                  const SVP_DST_BLOB_S              astDst[]             Output node array
*                  const SVP_NNIE_FORWARD_CTRL_S    *pstForwardCtrl       Ctrl prameters
*                  HI_BOOL                           bInstant             Flag indicating whether to generate an interrupt.
*                                                                           If the output result blocks the next operation,
*                                                                           set bInstant to HI_TRUE.
*   Spec         :
*   Return Value: HI_SUCCESS: Success; Error codes: Failure.
*   Spec         :
*   History:
*
*       1.  Date         : 2017-06-06
*          Author        :
*          Modification  : Created function
*
*****************************************************************************/
HI_S32 HI_MPI_SVP_NNIE_Forward(SVP_NNIE_HANDLE *phSvpNnieHandle,
    const SVP_SRC_BLOB_S astSrc[], const SVP_NNIE_MODEL_S *pstModel, const SVP_DST_BLOB_S astDst[],
    const SVP_NNIE_FORWARD_CTRL_S *pstForwardCtrl, HI_BOOL bInstant);

/*****************************************************************************
*   Prototype    : HI_MPI_SVP_NNIE_ForwardWithBbox
*   Description  : Perform CNN prediction on input sample(s), and output responses for corresponding sample(s)
*   Parameters   : SVP_NNIE_HANDLE                         *pNnieHandle    Returned handle ID of a task
*                  const SVP_SRC_BLOB_S                    astSrc[]        Input nodes' array.
*                  const SVP_SRC_BLOB_S                    astBbox[]       Input nodes' Bbox array.
*                  const SVP_NNIE_MODEL_S                  pstModel        CNN model data
*                  const SVP_DST_BLOB_S                    astDst[]        Output node array
*                  const SVP_NNIE_FORWARD_WITHBBOX_CTRL_S *pstForwardCtrl  Ctrl prameters
*                  HI_BOOL                                 bInstant        Flag indicating whether to generate an interrupt.
*                                                                            If the output result blocks the next operation,
*                                                                            set bInstant to HI_TRUE.
*   Spec         :
*   Return Value: HI_SUCCESS: Success; Error codes: Failure.
*   Spec         :
*   History:
*
*       1.  Date         : 2017-08-09
*          Author        :
*          Modification  : Created function
*
*****************************************************************************/
HI_S32 HI_MPI_SVP_NNIE_ForwardWithBbox(SVP_NNIE_HANDLE *phSvpNnieHandle,
    const SVP_SRC_BLOB_S astSrc[], const SVP_SRC_BLOB_S astBbox[], const SVP_NNIE_MODEL_S *pstModel,
    const SVP_DST_BLOB_S astDst[], const SVP_NNIE_FORWARD_WITHBBOX_CTRL_S *pstForwardCtrl, HI_BOOL bInstant);

/*****************************************************************************
*   Prototype    : HI_MPI_SVP_NNIE_UnloadModel
*   Description  : Unload cnn model
*   Parameters   : SVP_NNIE_MODEL_S         *pstModel          Output model
*
*   Return Value : HI_SUCCESS: Success; Error codes: Failure.
*   Spec         :
*   History:
*
*       1.  Date         : 2017-06-06
*           Author       :
*           Modification : Created function
*
*****************************************************************************/
HI_S32 HI_MPI_SVP_NNIE_UnloadModel(SVP_NNIE_MODEL_S *pstModel);

/*****************************************************************************
*   Prototype    : HI_MPI_SVP_NNIE_Query
*   Description  : This API is used to query the status of a function runed on nnie.
                   In block mode, the system waits until the function that is being queried is called.
                   In non-block mode, the current status is queried and no action is taken.
*   Parameters   : SVP_NNIE_ID_E        enNnieId       NNIE Id
*                  SVP_NNIE_HANDLE      nnieHandle     nnieHandle of a called function. It is entered by users.
*                  HI_BOOL              *pbFinish      Returned status
*                  HI_BOOL              bBlock         Flag indicating the block mode or non-block mode
*   Return Value : HI_SUCCESS: Success;Error codes: Failure.
*   Spec         :
*   History:
*
*       1.  Date         : 2017-06-06
*          Author        :
*          Modification  : Created function
*
*****************************************************************************/
HI_S32 HI_MPI_SVP_NNIE_Query(SVP_NNIE_ID_E enNnieId, SVP_NNIE_HANDLE svpNnieHandle,
    HI_BOOL *pbFinish, HI_BOOL bBlock);


/*****************************************************************************
*   Prototype    : HI_MPI_SVP_NNIE_AddTskBuf
*   Description  : This API is used to record tskbuf in tskbuf array.
                   If the tskbuf has been recored, when user send the task with recored tskbuf,
                   the phyaddr will not be mapped again, this will improve the execution efficiency
*   Parameters   :
*                  const SVP_MEM_INFO_S* pstTskBuf    TskBuf
*   Return Value : HI_SUCCESS: Success;Error codes: Failure.
*   Spec         :
*   History:
*
*       1.  Date         : 2018-08-20
*          Author        :
*          Modification  : Created function
*
*****************************************************************************/
HI_S32 HI_MPI_SVP_NNIE_AddTskBuf(const SVP_MEM_INFO_S* pstTskBuf);


/*****************************************************************************
*   Prototype    : HI_MPI_SVP_NNIE_RemoveTskBuf
*   Description  : This API is used to remove tskbuf from tskBuf array and umap viraddr.
                   If user no longer uses the recored tskbuf, the recored tskbuf must be removed
*   Parameters   :
*                  const SVP_MEM_INFO_S* pstTskBuf    TskBuf
*   Return Value : HI_SUCCESS: Success;Error codes: Failure.
*   Spec         :
*   History:
*
*       1.  Date         : 2018-08-20
*          Author        :
*          Modification  : Created function
*
*****************************************************************************/
HI_S32 HI_MPI_SVP_NNIE_RemoveTskBuf(const SVP_MEM_INFO_S* pstTskBuf);


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
#endif/*_HI_MPI_NNIE_H_*/

