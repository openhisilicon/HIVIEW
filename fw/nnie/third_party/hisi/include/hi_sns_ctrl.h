/*
* Copyright (C) Hisilicon Technologies Co., Ltd. 2012-2019. All rights reserved.
* Description:
* Author: Hisilicon multimedia software group
* Create: 2011/06/28
*/


#ifndef __HI_SNS_CTRL_H__
#define __HI_SNS_CTRL_H__

#include "hi_type.h"
#include "hi_comm_3a.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */


typedef struct hiISP_SNS_STATE_S {
    HI_BOOL     bInit;                  /* HI_TRUE: Sensor init */
    HI_BOOL     bSyncInit;              /* HI_TRUE: Sync Reg init */
    HI_U8       u8ImgMode;
    HI_U8       u8Hdr;               /* HI_TRUE: HDR enbale */
    WDR_MODE_E  enWDRMode;

    ISP_SNS_REGS_INFO_S astRegsInfo[2]; /* [0]: Sensor reg info of cur-frame; [1]: Sensor reg info of pre-frame ; */

    HI_U32      au32FL[2];              /* [0]: FullLines of cur-frame; [1]: Pre FullLines of pre-frame */
    HI_U32      u32FLStd;               /* FullLines std */
    HI_U32      au32WDRIntTime[4];
    HI_U32      au32SensorWBGain[ISP_BAYER_CHN_NUM];
} ISP_SNS_STATE_S;

typedef enum hiISP_SNS_MIRRORFLIP_TYPE_E {
    ISP_SNS_NORMAL      = 0,
    ISP_SNS_MIRROR      = 1,
    ISP_SNS_FLIP        = 2,
    ISP_SNS_MIRROR_FLIP = 3,
    ISP_SNS_BUTT
} ISP_SNS_MIRRORFLIP_TYPE_E;

typedef struct hiISP_SNS_BUS_EX_S {
    char bus_addr;
} ISP_SNS_BUS_EX_S;

typedef struct hiISP_SNS_OBJ_S {
    HI_S32 (*pfnRegisterCallback)(VI_PIPE ViPipe, ALG_LIB_S *pstAeLib, ALG_LIB_S *pstAwbLib);
    HI_S32 (*pfnUnRegisterCallback)(VI_PIPE ViPipe, ALG_LIB_S *pstAeLib, ALG_LIB_S *pstAwbLib);
    HI_S32 (*pfnSetBusInfo)(VI_PIPE ViPipe, ISP_SNS_COMMBUS_U unSNSBusInfo);
    HI_S32 (*pfnSetBusExInfo)(VI_PIPE ViPipe, ISP_SNS_BUS_EX_S *pstSerdesInfo);
    HI_VOID (*pfnStandby)(VI_PIPE ViPipe);
    HI_VOID (*pfnRestart)(VI_PIPE ViPipe);
    HI_VOID (*pfnMirrorFlip)(VI_PIPE ViPipe, ISP_SNS_MIRRORFLIP_TYPE_E eSnsMirrorFlip);
    HI_S32 (*pfnWriteReg)(VI_PIPE ViPipe, HI_S32 s32Addr, HI_S32 s32Data);
    HI_S32 (*pfnReadReg)(VI_PIPE ViPipe, HI_S32 s32Addr);
    HI_S32 (*pfnSetInit)(VI_PIPE ViPipe, ISP_INIT_ATTR_S *pstInitAttr);
} ISP_SNS_OBJ_S;

extern ISP_SNS_OBJ_S stSnsMn34220Obj;
extern ISP_SNS_OBJ_S stSnsImx377Obj;
extern ISP_SNS_OBJ_S stSnsImx299Obj;
extern ISP_SNS_OBJ_S stSnsImx477Obj;
extern ISP_SNS_OBJ_S stSnsImx299SlvsObj;
extern ISP_SNS_OBJ_S stSnsImx290Obj;
extern ISP_SNS_OBJ_S stSnsImx327Obj;
extern ISP_SNS_OBJ_S stSnsImx327_2l_Obj;
extern ISP_SNS_OBJ_S stSnsImx334Obj;
extern ISP_SNS_OBJ_S stSnsOV2718Obj;
extern ISP_SNS_OBJ_S stSnsAr0237Obj;

extern ISP_SNS_OBJ_S stSnsCmv50000Obj;
extern ISP_SNS_OBJ_S stSnsImx277SlvsObj;
extern ISP_SNS_OBJ_S stSnsImx117Obj;
extern ISP_SNS_OBJ_S stSnsImx290SlaveObj;
extern ISP_SNS_OBJ_S stSnsImx334SlaveObj;
extern ISP_SNS_OBJ_S stSnsImx226Obj;
extern ISP_SNS_OBJ_S stSnsImx335Obj;
extern ISP_SNS_OBJ_S stSnsImx307Obj;
extern ISP_SNS_OBJ_S stSnsImx307_2l_Obj;
extern ISP_SNS_OBJ_S stSnsImx458Obj;
extern ISP_SNS_OBJ_S stSnsSc4236Obj;
extern ISP_SNS_OBJ_S stSnsSc4210Obj;
extern ISP_SNS_OBJ_S stSnsSc2231Obj;
extern ISP_SNS_OBJ_S stSnsSc2235Obj;
extern ISP_SNS_OBJ_S stSnsSc3235Obj;
extern ISP_SNS_OBJ_S stSnsOs04b10_2lObj;
extern ISP_SNS_OBJ_S stSnsOs05aObj;
extern ISP_SNS_OBJ_S stSnsOS08A10Obj;
extern ISP_SNS_OBJ_S stSnsOs05a_2lObj;
extern ISP_SNS_OBJ_S stSnsGc2053Obj;
extern ISP_SNS_OBJ_S stSnsSharp8kObj;
extern ISP_SNS_OBJ_S stSnsOv12870Obj;
extern ISP_SNS_OBJ_S stSnsGc2053ForCarObj;
extern ISP_SNS_OBJ_S stSnsImx415Obj;

#define CMOS_CHECK_POINTER(ptr)\
    do {\
        if (ptr == HI_NULL)\
        {\
            ISP_ERR_TRACE("Null Pointer!\n");\
            return HI_ERR_ISP_NULL_PTR;\
        }\
    }while(0)

#define CMOS_CHECK_POINTER_VOID(ptr)\
    do {\
        if (ptr == HI_NULL)\
        {\
            ISP_ERR_TRACE("Null Pointer!\n");\
            return;\
        }\
    }while(0)

#define SENSOR_FREE(ptr)\
    do{\
        if (ptr != HI_NULL)\
        {\
            free(ptr);\
            ptr = HI_NULL;\
        }\
    } while (0)


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif /* __HI_SNS_CTRL_H__ */
