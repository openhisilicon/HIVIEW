/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2017-2019. All rights reserved.
 * Description: hdr function header file
 * Author: Hisilicon multimedia software group
 * Create: 2017-04-01
 */

#ifndef __MPI_HDR_H__
#define __MPI_HDR_H__

#include "hi_common.h"
#include "hi_comm_video.h"
#include "hi_comm_hdr.h"
#include "hi_comm_vpss.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

HI_S32 HI_MPI_HDR_SetOETFParam(VI_PIPE ViPipe, VPSS_GRP VpssGrp, VPSS_CHN VpssChn,
                               const HDR_OETF_PARAM_S *pstOETFParam);
HI_S32 HI_MPI_HDR_GetOETFParam(VI_PIPE ViPipe, VPSS_GRP VpssGrp, VPSS_CHN VpssChn, HDR_OETF_PARAM_S *pstOETFParam);

HI_S32 HI_MPI_HDR_SetTMParam(VI_PIPE ViPipe, VPSS_GRP VpssGrp, VPSS_CHN VpssChn, const HDR_TM_PARAM_S *pstTmParam);
HI_S32 HI_MPI_HDR_GetTMParam(VI_PIPE ViPipe, VPSS_GRP VpssGrp, VPSS_CHN VpssChn, HDR_TM_PARAM_S *pstTmParam);

HI_S32 HI_MPI_HDR_SetCSCParam(VI_PIPE ViPipe, VPSS_GRP VpssGrp, VPSS_CHN VpssChn, const HDR_CSC_PARAM_S *pstCscParam);
HI_S32 HI_MPI_HDR_GetCSCParam(VI_PIPE ViPipe, VPSS_GRP VpssGrp, VPSS_CHN VpssChn, HDR_CSC_PARAM_S *pstCscParam);

HI_S32 HI_MPI_HDR_SetCCMParam(VI_PIPE ViPipe, VPSS_GRP VpssGrp, VPSS_CHN VpssChn, const HDR_CCM_PARAM_S *pstCcmParam);
HI_S32 HI_MPI_HDR_GetCCMParam(VI_PIPE ViPipe, VPSS_GRP VpssGrp, VPSS_CHN VpssChn, HDR_CCM_PARAM_S *pstCcmParam);

HI_S32 HI_MPI_HDR_SetCLUTParam(VI_PIPE ViPipe, VPSS_GRP VpssGrp, VPSS_CHN VpssChn,
                               const HDR_CLUT_PARAM_S *pstClutParam);
HI_S32 HI_MPI_HDR_GetCLUTParam(VI_PIPE ViPipe, VPSS_GRP VpssGrp, VPSS_CHN VpssChn, HDR_CLUT_PARAM_S *pstClutParam);
HI_S32 HI_MPI_HDR_SetCLUTCoef(VI_PIPE ViPipe, VPSS_GRP VpssGrp, VPSS_CHN VpssChn, const HDR_CLUT_LUT_S *pstClutLut);
HI_S32 HI_MPI_HDR_GetCLUTCoef(VI_PIPE ViPipe, VPSS_GRP VpssGrp, VPSS_CHN VpssChn, HDR_CLUT_LUT_S *pstClutLut);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __MPI_HDR_H__ */

