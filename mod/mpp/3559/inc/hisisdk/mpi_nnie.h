/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2017-2019. All rights reserved.
 * Description: NNIE MPI header file
 * Author: Hisilicon multimedia software (SVP) group
 * Create: 2017-06-06
 */

#ifndef _HI_MPI_NNIE_H_
#define _HI_MPI_NNIE_H_

#include "hi_nnie.h"

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* End of #ifdef __cplusplus */

HI_S32 HI_MPI_SVP_NNIE_LoadModel(const SVP_SRC_MEM_INFO_S *pstModelBuf, SVP_NNIE_MODEL_S *pstModel);

HI_S32 HI_MPI_SVP_NNIE_GetTskBufSize(HI_U32 u32MaxInputNum, HI_U32 u32MaxBboxNum,
    const SVP_NNIE_MODEL_S *pstModel, HI_U32 au32TskBufSize[], HI_U32 u32NetSegNum);

HI_S32 HI_MPI_SVP_NNIE_Forward(SVP_NNIE_HANDLE *phSvpNnieHandle,
    const SVP_SRC_BLOB_S astSrc[], const SVP_NNIE_MODEL_S *pstModel, const SVP_DST_BLOB_S astDst[],
    const SVP_NNIE_FORWARD_CTRL_S *pstForwardCtrl, HI_BOOL bInstant);

HI_S32 HI_MPI_SVP_NNIE_ForwardWithBbox(SVP_NNIE_HANDLE *phSvpNnieHandle,
    const SVP_SRC_BLOB_S astSrc[], const SVP_SRC_BLOB_S astBbox[], const SVP_NNIE_MODEL_S *pstModel,
    const SVP_DST_BLOB_S astDst[], const SVP_NNIE_FORWARD_WITHBBOX_CTRL_S *pstForwardCtrl, HI_BOOL bInstant);

HI_S32 HI_MPI_SVP_NNIE_UnloadModel(SVP_NNIE_MODEL_S *pstModel);

HI_S32 HI_MPI_SVP_NNIE_Query(SVP_NNIE_ID_E enNnieId, SVP_NNIE_HANDLE svpNnieHandle,
    HI_BOOL *pbFinish, HI_BOOL bBlock);

HI_S32 HI_MPI_SVP_NNIE_AddTskBuf(const SVP_MEM_INFO_S* pstTskBuf);

HI_S32 HI_MPI_SVP_NNIE_RemoveTskBuf(const SVP_MEM_INFO_S* pstTskBuf);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
#endif /* _HI_MPI_NNIE_H_ */

