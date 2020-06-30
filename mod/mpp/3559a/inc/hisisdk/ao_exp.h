/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: ao_ext.h
 * Author: Hisilicon multimedia software group
 * Create: 2019/03/05
 * History               :
 * 1.Date                :   2019/03/05
 *   Modification        :   Created file
 */

#ifndef __HI_AO_EXP_H__
#define __HI_AO_EXP_H__

#include "hi_common.h"

typedef struct hiAO_EXPORT_CALLBACK_S {
    void  (*pfnAoNotify) (int aodev);
} AO_EXPORT_CALLBACK_S;

typedef HI_S32 FN_AO_RegisterExternCallback(AO_EXPORT_CALLBACK_S *pstExpCallback);

typedef struct hiAO_EXPORT_SYMBOL_S {
    FN_AO_RegisterExternCallback         *pfnAoRegisterExpCallback;
} AO_EXPORT_SYMBOL_S;


#endif  /* __HI_AO_EXP_H__ */

