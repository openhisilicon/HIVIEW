/******************************************************************************

  Copyright (C), 2014-2018, Hisilicon Tech. Co., Ltd.

 ******************************************************************************
  File Name     : mpi_hdmi.h
  Version       : Initial Draft
  Author        : Hisilicon multimedia software group
  Created       : 2011/12/28
  Description   :
  History       :
  1.Date        : 2011/12/28
    Author      :
    Modification: Created file
  2.Date        : 2015/01/13
    Author      :
    Modification: 1)change HI_MPI_HDMI_Init param
                  2)change HI_MPI_HDMI_Open param
                  3)add HI_MPI_HDMI_RegCallbackFunc
                  4)add HI_MPI_HDMI_UnRegCallbackFunc

******************************************************************************/

#ifndef __MPI_HDMI_H__
#define __MPI_HDMI_H__

#include "hi_comm_hdmi.h"

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* End of #ifdef __cplusplus */


HI_S32 HI_MPI_HDMI_Init(HI_VOID);
HI_S32 HI_MPI_HDMI_DeInit(HI_VOID);

HI_S32 HI_MPI_HDMI_Open(HI_HDMI_ID_E enHdmi);
HI_S32 HI_MPI_HDMI_Close(HI_HDMI_ID_E enHdmi);
HI_S32 HI_MPI_HDMI_GetSinkCapability(HI_HDMI_ID_E enHdmi, HI_HDMI_SINK_CAPABILITY_S *pstSinkCap);
HI_S32 HI_MPI_HDMI_SetAttr(HI_HDMI_ID_E enHdmi, const HI_HDMI_ATTR_S *pstAttr);
HI_S32 HI_MPI_HDMI_GetAttr(HI_HDMI_ID_E enHdmi, HI_HDMI_ATTR_S *pstAttr);
HI_S32 HI_MPI_HDMI_Start(HI_HDMI_ID_E enHdmi);
HI_S32 HI_MPI_HDMI_Stop(HI_HDMI_ID_E enHdmi);
HI_S32 HI_MPI_HDMI_Force_GetEDID(HI_HDMI_ID_E enHdmi, HI_HDMI_EDID_S *pstEdidData);
HI_S32 HI_MPI_HDMI_RegCallbackFunc(HI_HDMI_ID_E enHdmi, const HI_HDMI_CALLBACK_FUNC_S *pstCallbackFunc);
HI_S32 HI_MPI_HDMI_UnRegCallbackFunc(HI_HDMI_ID_E enHdmi, const HI_HDMI_CALLBACK_FUNC_S *pstCallbackFunc);

HI_S32 HI_MPI_HDMI_RegCECCallBackFunc(HI_HDMI_ID_E enHdmi, const HI_HDMI_CECCALLBACK_FUNC_S *pstCECCallback);
HI_S32 HI_MPI_HDMI_UnRegCECCallBackFunc(HI_HDMI_ID_E enHdmi, const HI_HDMI_CECCALLBACK_FUNC_S *pstCECCallback);
HI_S32 HI_MPI_HDMI_SetCECCommand(HI_HDMI_ID_E enHdmi, const HI_HDMI_CEC_CMD_S *pCECCmd);
HI_S32 HI_MPI_HDMI_CECStatus(HI_HDMI_ID_E enHdmi, HI_HDMI_CEC_STATUS_S *pStatus);
HI_S32 HI_MPI_HDMI_CEC_Enable(HI_HDMI_ID_E enHdmi);
HI_S32 HI_MPI_HDMI_CEC_Disable(HI_HDMI_ID_E enHdmi);

/* Module Parameter */
HI_S32 HI_MPI_HDMI_SetModParam(HI_HDMI_ID_E enHdmi, const HI_HDMI_MOD_PARAM_S *pstModParam);
HI_S32 HI_MPI_HDMI_GetModParam(HI_HDMI_ID_E enHdmi, HI_HDMI_MOD_PARAM_S *pstModParam);

/* Normally, these functions are not necessary */
HI_S32 HI_MPI_HDMI_SetInfoFrame(HI_HDMI_ID_E enHdmi, const HI_HDMI_INFOFRAME_S *pstInfoFrame);
HI_S32 HI_MPI_HDMI_GetInfoFrame(HI_HDMI_ID_E enHdmi, HI_HDMI_INFOFRAME_TYPE_E enInfoFrameType, HI_HDMI_INFOFRAME_S *pstInfoFrame);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif /*__MPI_HDMI_H__ */

