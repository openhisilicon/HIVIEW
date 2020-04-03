/******************************************************************************

  Copyright (C), 2001-2017, Hisilicon Tech. Co., Ltd.

 ******************************************************************************
  File Name     : sample_dsp_enca.h
  Version       : Initial Draft
  Author        : Hisilicon multimedia software group
  Created       : 2017/11/06
  Description   : Encapsulate mpi by HI_MPI_SVP_DSP_RPC
  History       :
  1.Date        : 2017/11/06
    Author      : c00211359
    Modification: Created file

******************************************************************************/
#ifndef __SAMPLE_DSP_ENCA_H__
#define __SAMPLE_DSP_ENCA_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "hi_dsp.h"

/*****************************************************************************
*    Prototype     : SAMPLE_SVP_DSP_ENCA_Dilate3x3
*    Description : Encapsulate Dilate 3x3
*
*    Parameters     :  SVP_DSP_HANDLE      *phHandle           Handle
*                      SVP_SRC_IMAGE_S     *pstSrc             Input image
*                      SVP_DST_IMAGE_S     *pstDst             Output image
*                      SVP_DSP_ID_E         enDspId             DSP Core ID.
*                      SVP_DSP_PRI_E        enPri               Priority
*                      SVP_MEM_INFO_S      *pstAssistBuf       Assist buffer
*
*    Return Value : HI_SUCCESS: Success;Error codes: Failure.
*    Spec         :
*
*    History:
*
*        1.    Date         : 2017-11-06
*            Author         :
*            Modification : Created function
*
****************************************************************************/
HI_S32 SAMPLE_SVP_DSP_ENCA_Dilate3x3(SVP_DSP_HANDLE *phHandle,SVP_DSP_ID_E enDspId,SVP_DSP_PRI_E enPri,
                    SVP_SRC_IMAGE_S *pstSrc,SVP_DST_IMAGE_S *pstDst,SVP_MEM_INFO_S *pstAssistBuf);

/*****************************************************************************
*    Prototype     : SAMPLE_SVP_DSP_ENCA_Erode3x3
*    Description : Encapsulate Erode 3x3
*
*    Parameters     :  SVP_DSP_HANDLE      *phHandle           Handle
*                      SVP_SRC_IMAGE_S     *pstSrc             Input image
*                      SVP_DST_IMAGE_S     *pstDst             Output image
*                      SVP_DSP_ID_E         enDspId            DSP Core ID.
*                      SVP_DSP_PRI_E        enPri              Priority
*                      SVP_MEM_INFO_S      *pstAssistBuf       Assist buffer
*
*    Return Value : HI_SUCCESS: Success;Error codes: Failure.
*    Spec         :
*
*    History:
*
*        1.    Date         : 2017-11-06
*            Author         :
*            Modification : Created function
*
****************************************************************************/
HI_S32 SAMPLE_SVP_DSP_ENCA_Erode3x3(SVP_DSP_HANDLE *phHandle,SVP_DSP_ID_E enDspId,SVP_DSP_PRI_E enPri,
                    SVP_SRC_IMAGE_S *pstSrc,SVP_DST_IMAGE_S *pstDst,SVP_MEM_INFO_S *pstAssistBuf);

#ifdef __cplusplus
}
#endif

#endif
