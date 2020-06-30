/******************************************************************************

  Copyright (C), 2001-2017, Hisilicon Tech. Co., Ltd.

 ******************************************************************************
  File Name     : mpi_dsp.h
  Version       : Initial Draft
  Author        : Hisilicon multimedia software (SVP) group
  Created       : 2017/10/17
  Description   :
  History       :
  1.Date        : 2017/10/17
    Author      :
    Modification: Created file
******************************************************************************/
#ifndef _HI_MPI_DSP_H_
#define _HI_MPI_DSP_H_

#include "hi_dsp.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */

/*****************************************************************************
*   Prototype    : HI_MPI_SVP_DSP_LoadBin
*   Description  : Load DSP Bin
*
*   Parameters   : const HI_CHAR       *pszBinFileName       Bin file.
*                  SVP_DSP_MEM_TYPE_E  enMemType             DSP memory type.
*
*
*
*   Return Value : HI_SUCCESS: Success;Error codes: Failure.
*   Spec         :
*
*   History:
*
*       1.  Date         : 2017-10-17
*           Author       :
*           Modification : Created function
*
****************************************************************************/
HI_S32 HI_MPI_SVP_DSP_LoadBin(const HI_CHAR *pszBinFileName, SVP_DSP_MEM_TYPE_E enMemType);

/*****************************************************************************
*   Prototype    : HI_MPI_SVP_DSP_EnableCore
*   Description  : Enable DSP Core
*
*   Parameters   : SVP_DSP_ID_E        enDspId               DSP Core ID.
*
*
*
*
*   Return Value : HI_SUCCESS: Success;Error codes: Failure.
*   Spec         :
*
*   History:
*
*       1.  Date         : 2017-10-17
*           Author       :
*           Modification : Created function
*
****************************************************************************/
HI_S32 HI_MPI_SVP_DSP_EnableCore(SVP_DSP_ID_E enDspId);

/*****************************************************************************
*   Prototype    : HI_MPI_SVP_DSP_DisableCore
*   Description  : Disable DSP Core
*
*   Parameters   : SVP_DSP_ID_E        enDspId               DSP Core ID.
*
*
*
*
*   Return Value : HI_SUCCESS: Success;Error codes: Failure.
*   Spec         :
*
*   History:
*
*       1.  Date         : 2017-10-17
*           Author       :
*           Modification : Created function
*
****************************************************************************/
HI_S32 HI_MPI_SVP_DSP_DisableCore(SVP_DSP_ID_E enDspId);

/*****************************************************************************
*   Prototype    : HI_MPI_SVP_DSP_RPC
*   Description  : Remote process control
*
*   Parameters   : SVP_DSP_HANDLE               *phHandle       Handle
*                  const SVP_DSP_MESSAGE_S      *pstMsg         Message
*                  SVP_DSP_ID_E                 enDspId         DSP Core ID.
*                  SVP_DSP_PRI_E                enPri           Priority
*
*
*
*   Return Value : HI_SUCCESS: Success;Error codes: Failure.
*   Spec         :
*
*   History:
*
*       1.  Date         : 2017-10-17
*           Author       :
*           Modification : Created function
*
****************************************************************************/
HI_S32 HI_MPI_SVP_DSP_RPC(SVP_DSP_HANDLE *phHandle, const SVP_DSP_MESSAGE_S *pstMsg,
                          SVP_DSP_ID_E enDspId, SVP_DSP_PRI_E enPri);

/*****************************************************************************
*   Prototype    : HI_MPI_SVP_DSP_Query
*   Description  : This API is used to query the status of a called function by using the returned hHandle of the function.
                   In block mode, the system waits until the task is finish or query timeout.
                   In non-block mode, the current status is queried and no action is taken.
*   Parameters   : SVP_DSP_ID_E          enDspId          DSP Core ID.
*                  SVP_DSP_HANDLE        hHandle          Handle.
*                  HI_BOOL               *pbFinish         Returned status
*                  HI_BOOL               bBlock            Flag indicating the block mode or non-block mode
*   Return Value : HI_SUCCESS: Success;Error codes: Failure.
*   Spec         :
*   History:
*
*       1.  Date         : 2017-10-17
*           Author       :
*           Modification : Created function
*
*****************************************************************************/
HI_S32 HI_MPI_SVP_DSP_Query(SVP_DSP_ID_E enDspId, SVP_DSP_HANDLE hHandle, HI_BOOL *pbFinish, HI_BOOL bBlock);

/*****************************************************************************
*   Prototype    : HI_MPI_SVP_DSP_PowerOn
*   Description  : Enable DSP Core.
*   Parameters   : SVP_DSP_ID_E          enDspId          DSP Core ID.
*   Return Value : HI_SUCCESS: Success;Error codes: Failure.
*   Spec         :
*   History:
*
*       1.  Date         : 2017-10-17
*           Author       :
*           Modification : Created function
*
*****************************************************************************/
HI_S32 HI_MPI_SVP_DSP_PowerOn(SVP_DSP_ID_E enDspId);

/*****************************************************************************
*   Prototype    : HI_MPI_SVP_DSP_PowerOff
*   Description  : Disable DSP Core.
*   Parameters   : SVP_DSP_ID_E          enDspId          DSP Core ID.
*   Return Value : HI_SUCCESS: Success;Error codes: Failure.
*   Spec         :
*   History:
*
*       1.  Date         : 2017-10-17
*           Author       :
*           Modification : Created function
*
*****************************************************************************/
HI_S32 HI_MPI_SVP_DSP_PowerOff(SVP_DSP_ID_E enDspId);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
#endif /* _HI_MPI_DSP_H_ */
