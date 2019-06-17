/******************************************************************************

  Copyright (C), 2001-2011, Hisilicon Tech. Co., Ltd.

 ******************************************************************************
  File Name     : viu_ext.h
  Version       : Initial Draft
  Author        : Hisilicon multimedia software group
  Created       : 2015/01/15
  Description   : 
  History       :
  1.Date        : 2015/01/15
    Author      : w00250866
    Modification: Created file

******************************************************************************/
#ifndef __VIU_EXT_H__
#define __VIU_EXT_H__

#include "hi_type.h"

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* End of #ifdef __cplusplus */

typedef struct hiVIU_FLASH_CALLBACK_S
{
    HI_S32  (*pfnViuGetFlashStatus) (HI_VOID);
} VIU_FLASH_CALLBACK_S;

typedef struct hiVIU_DUMPRAW_CALLBACK_S
{
    HI_S32  (*pfnViuGetRawStatus) (HI_S32 ViDev,VIDEO_FRAME_INFO_S  *pstFrameInfo);
} VIU_DUMPRAW_CALLBACK_S;

typedef HI_S32 FN_VI_RegisterFlashCallBack(HI_S32 ViDev, VIU_FLASH_CALLBACK_S *pstFlashCb);
typedef HI_S32 FN_VI_RegisterDumpRawCallBack(HI_S32 ViDev, VIU_DUMPRAW_CALLBACK_S *pstDumpRawCb);


typedef struct hiVIU_EXPORT_FUNC_S
{
    FN_VI_RegisterFlashCallBack        *pfnViRegisterFlashCallBack;
    FN_VI_RegisterDumpRawCallBack       *pfnViRegisterDumpRawCallBack;
}VIU_EXPORT_FUNC_S;


extern VIU_EXPORT_FUNC_S g_stViuExpFunc;

#define CKFN_VIU_RegisterFlashCallBack()\
    (NULL != g_stViuExpFunc.pfnViRegisterFlashCallBack)
#define CALL_VIU_RegisterFlashCallBack(ViDev,pstFlashCb)\
    g_stViuExpFunc.pfnViRegisterFlashCallBack(ViDev,pstFlashCb)


#define CKFN_VIU_RegisterDumpRawCallBack()\
    (NULL != g_stViuExpFunc.pfnViRegisterDumpRawCallBack)
#define CALL_VIU_RegisterDumpRawCallBack(ViDev,pstDumpRawCb)\
    g_stViuExpFunc.pfnViRegisterDumpRawCallBack(ViDev,pstDumpRawCb)

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif

