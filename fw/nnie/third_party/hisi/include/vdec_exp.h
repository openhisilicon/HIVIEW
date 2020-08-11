/*
* Copyright (c) Hisilicon Technologies Co., Ltd. 2016-2019. All rights reserved.
* Description:vdec exp function file
* Author: Hisilicon multimedia software group
* Create: 2016/07/15
*/

#include "hi_common.h"

#ifndef __HI_VDEC_EXP_H__
#define __HI_VDEC_EXP_H__

typedef struct hiVDEC_EXPORT_CALLBACK_S {
    void (*pfnVdecNotify)(int module_id, int channel, int event_type, void *args);
    int (*pfnVdecSetscheduler_Dec)(void);
    int (*pfnVdecSetscheduler_Stm)(void);
    int (*pfnVdecSetscheduler_Syn)(void);
} VDEC_EXPORT_CALLBACK_S;

typedef HI_S32 FN_VDEC_RegisterExpCallback(VDEC_EXPORT_CALLBACK_S *pstExpCallback);

typedef struct hiVDEC_EXPORT_SYMBOL_S {
    FN_VDEC_RegisterExpCallback *pfnVdecRegisterExpCallback;
} VDEC_EXPORT_SYMBOL_S;

#endif /* __HI_VOU_EXP_H__ */


