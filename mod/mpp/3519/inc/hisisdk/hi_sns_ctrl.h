/******************************************************************************

  Copyright (C), 2001-2011, Hisilicon Tech. Co., Ltd.

 ******************************************************************************
  File Name     : hi_sns_ctrl.h
  Version       : Initial Draft
  Author        : Hisilicon multimedia software group
  Created       : 2011/01/10
  Description   : 
  History       :
  1.Date        : 2011/01/10
    Author      : x00100808
    Modification: Created file

******************************************************************************/

#ifndef __HI_SNS_CTRL_H__
#define __HI_SNS_CTRL_H__

#include "hi_type.h"
#include "hi_comm_3a.h"
#include "hi_comm_isp.h"

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* End of #ifdef __cplusplus */


//#define DOUBLE_SENSOR
//#define ISP_MAX_DEV_NUM     2

typedef struct hiISP_SNS_STATE_S
{
    HI_BOOL     bInit;                  /* HI_TRUE: Sensor init */
    HI_BOOL     bSyncInit;              /* HI_TRUE: Sync Reg init */
    HI_U8       u8ImgMode;
    WDR_MODE_E  enWDRMode;

    ISP_SNS_REGS_INFO_S astRegsInfo[2]; /* [0]: Sensor reg info of cur-frame; [1]: Sensor reg info of pre-frame ; */

    HI_U32      au32FL[2];              /* [0]: FullLines of cur-frame; [1]: Pre FullLines of pre-frame */
    HI_U32      u32FLStd;               /* FullLines std */
    HI_U32      au32WDRIntTime[4];
} ISP_SNS_STATE_S;

typedef struct hiISP_SNS_OBJ_S
{
    HI_S32  (*pfnRegisterCallback)(ISP_DEV IspDev, ALG_LIB_S *pstAeLib, ALG_LIB_S *pstAwbLib);
    HI_S32  (*pfnUnRegisterCallback)(ISP_DEV IspDev, ALG_LIB_S *pstAeLib, ALG_LIB_S *pstAwbLib);
    HI_S32  (*pfnSetBusInfo)(ISP_DEV IspDev, ISP_SNS_COMMBUS_U unSNSBusInfo);
    HI_VOID (*pfnStandby)(ISP_DEV IspDev);
    HI_VOID (*pfnRestart)(ISP_DEV IspDev);
    HI_S32  (*pfnWriteReg)(ISP_DEV IspDev, HI_S32 s32Addr, HI_S32 s32Data);
    HI_S32  (*pfnReadReg)(ISP_DEV IspDev, HI_S32 s32Addr);
    HI_S32  (*pfnSetInit)(ISP_DEV IspDev, ISP_INIT_ATTR_S *pstInitAttr);
} ISP_SNS_OBJ_S;

extern ISP_SNS_OBJ_S stSnsOs05aObj;
extern ISP_SNS_OBJ_S stSnsImx117Obj;
extern ISP_SNS_OBJ_S stSnsImx326Obj;
extern ISP_SNS_OBJ_S stSnsImx326SlaveObj;
extern ISP_SNS_OBJ_S stSnsImx226Obj;
extern ISP_SNS_OBJ_S stSnsImx274Obj;
extern ISP_SNS_OBJ_S stSnsImx290Obj;
extern ISP_SNS_OBJ_S stSnsOv4689SlaveObj;
extern ISP_SNS_OBJ_S stSnsOv4689Obj;
extern ISP_SNS_OBJ_S stSnsMn34120Obj;
extern ISP_SNS_OBJ_S stSnsMn34220Obj;
extern ISP_SNS_OBJ_S stSnsImx185Obj;
extern ISP_SNS_OBJ_S stSnsImx274MipiObj;
extern ISP_SNS_OBJ_S stSnsImx377Obj;

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif /* __HI_SNS_CTRL_H__ */

