/******************************************************************************
 
  Copyright (C), 2001-2011, Hisilicon Tech. Co., Ltd.
 
 ******************************************************************************
  File Name     : hi3511_ext.h
  Version       : Initial Draft
  Author        : Hisilicon multimedia software group
  Created       : 2006/12/22
  Description   : 
  History       :
  1.Date        : 2006/12/22
    Author      : c42025
    Modification: Created file

  2.Date        : 2008/02/21
    Author      : c42025
    Modification: add function CMPI_QueryModules, for solving AE6D02922 

  3.Date        : 2008/02/26
    Author      : c42025
    Modification: Defect Form : AE6D02922, add two functions "FN_MOD_Notify"
    and "FN_MOD_QueryState" .

  4.Date        : 2008/03/03
    Author      : c42025
    Modification: add structure definition "MOD_NAME_S"

******************************************************************************/
#include "hi_osal.h"

#include "hi_type.h"
#include "hi_common.h"


#ifndef __MOD_EXT_H__
#define __MOD_EXT_H__

#define MAX_MPP_MODULES HI_ID_BUTT

#define MAX_MOD_NAME    16

typedef enum hiMOD_NOTICE_ID_E
{
    MOD_NOTICE_STOP = 0x11,
}MOD_NOTICE_ID_E;

typedef enum hiMOD_STATE_E
{
    MOD_STATE_FREE = 0x11,
    MOD_STATE_BUSY = 0X22,
}MOD_STATE_E;

typedef HI_S32 FN_MOD_Init(HI_VOID *);
typedef HI_VOID FN_MOD_Exit(HI_VOID);
typedef HI_VOID FN_MOD_Notify(MOD_NOTICE_ID_E enNoticeId);
typedef HI_VOID FN_MOD_QueryState(MOD_STATE_E *pstState);
typedef HI_U32 FN_MOD_VerChecker(HI_VOID);

typedef struct hiMPP_MODULE_S
{
    struct osal_list_head list;
    
    HI_CHAR aModName[MAX_MOD_NAME];
    MOD_ID_E enModId;
    
    FN_MOD_Init         *pfnInit;
    FN_MOD_Exit         *pfnExit;
    FN_MOD_QueryState   *pfnQueryState;
    FN_MOD_Notify       *pfnNotify;
    FN_MOD_VerChecker   *pfnVerChecker;

    HI_BOOL  bInited;
        
    HI_VOID *pstExportFuncs;    
    HI_VOID *pData;

    HI_CHAR *pVersion;
}UMAP_MODULE_S;


extern HI_CHAR *CMPI_GetModuleName(MOD_ID_E enModId);
extern UMAP_MODULE_S *CMPI_GetModuleById(MOD_ID_E enModId);
extern HI_VOID *CMPI_GetModuleFuncById(MOD_ID_E enModId);

extern HI_VOID CMPI_StopModules(HI_VOID);
extern HI_S32  CMPI_QueryModules(HI_VOID);
extern HI_S32  CMPI_InitModules(HI_VOID);
extern HI_VOID CMPI_ExitModules(HI_VOID);
extern HI_S32  CMPI_RegisterModule(UMAP_MODULE_S *pstModules);
extern HI_VOID CMPI_UnRegisterModule(MOD_ID_E enModId);

#define FUNC_ENTRY(type,id)  ((type*)CMPI_GetModuleFuncById(id))
#define CHECK_FUNC_ENTRY(id) (CMPI_GetModuleFuncById(id) != NULL)
#define FUNC_ENTRY_NULL(id)  (!CHECK_FUNC_ENTRY(id))


#endif /*  __MOD_EXT_H__ */

