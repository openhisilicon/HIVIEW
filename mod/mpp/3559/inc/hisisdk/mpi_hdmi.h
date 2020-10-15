/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2019. All rights reserved.
 * Description: Interface defination of HDMI
 * Author: Hisilicon multimedia software group
 * Create: 2019/07/06
 */
#ifndef __MPI_HDMI_H__
#define __MPI_HDMI_H__

#include "hi_comm_hdmi.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

hi_s32 HI_MPI_HDMI_Init(hi_void);

hi_s32 HI_MPI_HDMI_DeInit(hi_void);

hi_s32 HI_MPI_HDMI_Open(HI_HDMI_ID_E enHdmi);

hi_s32 HI_MPI_HDMI_Close(HI_HDMI_ID_E enHdmi);

hi_s32 HI_MPI_HDMI_GetSinkCapability(HI_HDMI_ID_E enHdmi, HI_HDMI_SINK_CAPABILITY_S *pstSinkCap);

hi_s32 HI_MPI_HDMI_SetAttr(HI_HDMI_ID_E enHdmi, const HI_HDMI_ATTR_S *pstAttr);

hi_s32 HI_MPI_HDMI_GetAttr(HI_HDMI_ID_E enHdmi, HI_HDMI_ATTR_S *pstAttr);

hi_s32 HI_MPI_HDMI_Start(HI_HDMI_ID_E enHdmi);

hi_s32 HI_MPI_HDMI_Stop(HI_HDMI_ID_E enHdmi);

hi_s32 HI_MPI_HDMI_Force_GetEDID(HI_HDMI_ID_E enHdmi, HI_HDMI_EDID_S *pstEdidData);

hi_s32 HI_MPI_HDMI_RegCallbackFunc(HI_HDMI_ID_E enHdmi, const HI_HDMI_CALLBACK_FUNC_S *pstCallbackFunc);

hi_s32 HI_MPI_HDMI_UnRegCallbackFunc(HI_HDMI_ID_E enHdmi, const HI_HDMI_CALLBACK_FUNC_S *pstCallbackFunc);

hi_s32 HI_MPI_HDMI_RegCECCallBackFunc(HI_HDMI_ID_E enHdmi, const HI_HDMI_CECCALLBACK_FUNC_S *pstCECCallback);

hi_s32 HI_MPI_HDMI_UnRegCECCallBackFunc(HI_HDMI_ID_E enHdmi, const HI_HDMI_CECCALLBACK_FUNC_S *pstCECCallback);

hi_s32 HI_MPI_HDMI_SetCECCommand(HI_HDMI_ID_E enHdmi, const HI_HDMI_CEC_CMD_S *pCECCmd);

hi_s32 HI_MPI_HDMI_CECStatus(HI_HDMI_ID_E enHdmi, HI_HDMI_CEC_STATUS_S *pStatus);

hi_s32 HI_MPI_HDMI_CEC_Enable(HI_HDMI_ID_E enHdmi);

hi_s32 HI_MPI_HDMI_CEC_Disable(HI_HDMI_ID_E enHdmi);

hi_s32 HI_MPI_HDMI_SetModParam(HI_HDMI_ID_E enHdmi, const HI_HDMI_MOD_PARAM_S *pstModParam);

hi_s32 HI_MPI_HDMI_GetModParam(HI_HDMI_ID_E enHdmi, HI_HDMI_MOD_PARAM_S *pstModParam);

/* Normally, these functions are not necessary */
hi_s32 HI_MPI_HDMI_SetInfoFrame(HI_HDMI_ID_E enHdmi, const HI_HDMI_INFOFRAME_S *pstInfoFrame);

hi_s32 HI_MPI_HDMI_GetInfoFrame(HI_HDMI_ID_E enHdmi,
    HI_HDMI_INFOFRAME_TYPE_E enInfoFrameType, HI_HDMI_INFOFRAME_S *pstInfoFrame);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
#endif /* __MPI_HDMI_H__ */

