/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2017-2019. All rights reserved.
 * Description: Deep process unit of rectification.
 * Author: Guo
 * Create: 2017-11-10
 */

#ifndef __MPI_RECT_H__
#define __MPI_RECT_H__

#include "hi_comm_dpu_rect.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

HI_S32 HI_MPI_DPU_RECT_GetLutSize(const HI_CHAR *pchFileName, HI_U32 *pu32Size);
HI_S32 HI_MPI_DPU_RECT_LoadLut(const HI_CHAR *pchFileName,
    const DPU_RECT_MEM_INFO_S *pstLutMem, DPU_RECT_LUT_ID *pDpuRectLutId);
HI_S32 HI_MPI_DPU_RECT_UnloadLut(DPU_RECT_LUT_ID DpuRectLutId);

HI_S32 HI_MPI_DPU_RECT_CreateGrp(DPU_RECT_GRP DpuRectGrp, const DPU_RECT_GRP_ATTR_S *pstGrpAttr);
HI_S32 HI_MPI_DPU_RECT_DestroyGrp(DPU_RECT_GRP DpuRectGrp);

HI_S32 HI_MPI_DPU_RECT_SetGrpAttr(DPU_RECT_GRP DpuRectGrp, const DPU_RECT_GRP_ATTR_S *pstGrpAttr);
HI_S32 HI_MPI_DPU_RECT_GetGrpAttr(DPU_RECT_GRP DpuRectGrp, DPU_RECT_GRP_ATTR_S *pstGrpAttr);

HI_S32 HI_MPI_DPU_RECT_SetChnAttr(DPU_RECT_GRP DpuRectGrp, DPU_RECT_CHN DpuRectChn,
    const DPU_RECT_CHN_ATTR_S *pstChnAttr);
HI_S32 HI_MPI_DPU_RECT_GetChnAttr(DPU_RECT_GRP DpuRectGrp, DPU_RECT_CHN DpuRectChn,
    DPU_RECT_CHN_ATTR_S *pstChnAttr);

HI_S32 HI_MPI_DPU_RECT_EnableChn(DPU_RECT_GRP DpuRectGrp, DPU_RECT_CHN DpuRectChn);
HI_S32 HI_MPI_DPU_RECT_DisableChn(DPU_RECT_GRP DpuRectGrp, DPU_RECT_CHN DpuRectChn);

HI_S32 HI_MPI_DPU_RECT_StartGrp(DPU_RECT_GRP DpuRectGrp);
HI_S32 HI_MPI_DPU_RECT_StopGrp(DPU_RECT_GRP DpuRectGrp);

HI_S32 HI_MPI_DPU_RECT_GetFrame(DPU_RECT_GRP DpuRectGrp, VIDEO_FRAME_INFO_S *pstSrcLeftFrame,
    VIDEO_FRAME_INFO_S *pstSrcRightFrame, VIDEO_FRAME_INFO_S *pstDstLeftFrame,
    VIDEO_FRAME_INFO_S *pstDstRightFrame, HI_S32 s32MilliSec);
HI_S32 HI_MPI_DPU_RECT_ReleaseFrame(DPU_RECT_GRP DpuRectGrp,
    const VIDEO_FRAME_INFO_S *pstSrcLeftFrame, const VIDEO_FRAME_INFO_S *pstSrcRightFrame,
    const VIDEO_FRAME_INFO_S *pstDstLeftFrame, const VIDEO_FRAME_INFO_S *pstDstRightFrame);

HI_S32 HI_MPI_DPU_RECT_SendFrame(DPU_RECT_GRP DpuRectGrp, const VIDEO_FRAME_INFO_S *pstLeftFrame,
    const VIDEO_FRAME_INFO_S *pstRightFrame, HI_S32 s32MilliSec);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __MPI_RECT_H__ */

