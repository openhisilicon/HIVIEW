/******************************************************************************

  Copyright (C), 2001-2011, Hisilicon Tech. Co., Ltd.

 ******************************************************************************
  File Name     : vou_ext.h
  Version       : Initial Draft
  Author        : Hisilicon multimedia software group
  Created       : 2011/06/28
  Description   :
  History       :
  1.Date        : 2011/06/28
    Author      : n00168968/z00185248/l00181524
    Modification: Created file

******************************************************************************/

#include "hi_common.h"

#ifndef __HI_VOU_EXP_H__
#define __HI_VOU_EXP_H__

typedef struct hiVOU_EXPORT_CALLBACK_S
{
    void  (*pfnVoNotify) (int module_id, int vodev);
} VOU_EXPORT_CALLBACK_S;

typedef HI_S32 FN_VO_RegisterExternCallback(VOU_EXPORT_CALLBACK_S *pstExpCallback);

typedef struct hiVOU_EXPORT_SYMBOL_S
{
    FN_VO_RegisterExternCallback         *pfnVoRegisterExpCallback;
}VOU_EXPORT_SYMBOL_S;


#endif  /* __HI_VOU_EXP_H__ */

