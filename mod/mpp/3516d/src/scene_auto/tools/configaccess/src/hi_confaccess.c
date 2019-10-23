/**
 * @file      hi_confaccess.c
 * @brief     configure item access module
 *
 * Copyright (c) 2017 Huawei Tech.Co.,Ltd
 *
 * @author    HiMobileCam Reference Develop Team
 * @date      2017/12/15
 * @version   1.0

 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hi_type.h"
#include <libgen.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <pthread.h>

#include "hi_confaccess.h"
#include "iniparser.h"
#include "hi_list.h"
#include "hi_scenecomm.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

/** redefine module name */
#ifdef HI_MODULE
#undef HI_MODULE
#endif
#define HI_MODULE "CONFACCESS"


#define CONFACCESS_DEFAULT_MODULE_CFGPATH "./" /**<default module cfg path */
#define CONFACCESS_PATH_SEPARATOR '/'          /**<path separator */

/** module node information */
typedef struct tagCONFACCESS_MODULE_S
{
    HI_List_Head_S stList;
    HI_CHAR szName[HI_CONFACCESS_NAME_MAX_LEN];    /**<module name */
    HI_CHAR szPath[HI_CONFACCESS_PATH_MAX_LEN];    /**<module path */
    HI_CHAR szIniFile[HI_CONFACCESS_PATH_MAX_LEN]; /**<module ini filename, not included path */
    dictionary *pstIniDir;
} CONFACCESS_MODULE_S;

/** cfg node information */
typedef struct tagCONFACCESS_CFG_NODE_S
{
    HI_List_Head_S stList;
    HI_CHAR szCfgName[HI_CONFACCESS_NAME_MAX_LEN];
    HI_CHAR szCfgPath[HI_CONFACCESS_PATH_MAX_LEN];
    CONFACCESS_MODULE_S stModule;
    pthread_mutex_t MutexLock;
} CONFACCESS_CFG_NODE_S;

/** cfg list */
static HI_List_Head_S s_stCONFACCESSList = HI_LIST_INIT_HEAD_DEFINE(s_stCONFACCESSList);

/** configure count in list */
static HI_S32 s_s32CONFACCESSCount = 0;


static HI_VOID CONFACCESS_DelModuleList(CONFACCESS_CFG_NODE_S *pstCfgNode)
{
    HI_List_Head_S *pstModuleList = NULL;
    CONFACCESS_MODULE_S *pstModulePos = NULL;
    HI_S32 s32ModuleCnt = 0;

    HI_List_For_Each(pstModuleList, &(pstCfgNode->stModule.stList))
    {
        HI_List_Del(pstModuleList);
        pstModulePos = HI_LIST_ENTRY(pstModuleList, CONFACCESS_MODULE_S, stList);
        MLOGD("Module: inifile[%s]\n", pstModulePos->szIniFile);
        s32ModuleCnt++;
        iniparser_freedict(pstModulePos->pstIniDir);
        pstModulePos->pstIniDir = NULL;
        HI_SCENECOMM_SAFE_FREE(pstModulePos);
        pstModuleList = &(pstCfgNode->stModule.stList);
    }
    MLOGD("CfgName[%s] ModuleCnt[%d]\n", pstCfgNode->szCfgName, s32ModuleCnt);
}

static HI_S32 CONFACCESS_Init(const HI_CHAR *pszCfgName, const HI_CHAR *pszCfgPath, HI_U32 *pu32ModuleNum)
{
    CONFACCESS_CFG_NODE_S *pstCfgPos = NULL;
    HI_CHAR aszCfgPath[HI_CONFACCESS_PATH_MAX_LEN];
    /* Malloc CfgNode Memory */
    pstCfgPos = (CONFACCESS_CFG_NODE_S *)malloc(sizeof(CONFACCESS_CFG_NODE_S));
    HI_SCENECOMM_CHECK_POINTER(pstCfgPos, HI_CONFACCESS_EMALLOC);
    memset(pstCfgPos, 0, sizeof(CONFACCESS_CFG_NODE_S));

    /* Record CfgNode Information */
    snprintf(pstCfgPos->szCfgName, HI_CONFACCESS_NAME_MAX_LEN, "%s", pszCfgName);
    snprintf(pstCfgPos->szCfgPath, HI_CONFACCESS_PATH_MAX_LEN, "%s", pszCfgPath);
    HI_MUTEX_INIT_LOCK(pstCfgPos->MutexLock);
    HI_MUTEX_LOCK(pstCfgPos->MutexLock);
    pstCfgPos->stModule.stList.next = &(pstCfgPos->stModule.stList);
    pstCfgPos->stModule.stList.prev = &(pstCfgPos->stModule.stList);
    pstCfgPos->stModule.pstIniDir = iniparser_load(pszCfgPath);
    if (NULL == pstCfgPos->stModule.pstIniDir)
    {
        MLOGE("load %s failed\n", pszCfgPath);
        HI_MUTEX_UNLOCK(pstCfgPos->MutexLock);
        HI_MUTEX_DESTROY(pstCfgPos->MutexLock);
        HI_SCENECOMM_SAFE_FREE(pstCfgPos);
        return HI_FAILURE;
    }
    snprintf(pstCfgPos->stModule.szIniFile, HI_CONFACCESS_PATH_MAX_LEN, "%s", pszCfgPath);

    strncpy(aszCfgPath, pszCfgPath, strlen(pszCfgPath));
    dirname(aszCfgPath);

    /* Get Module Count and Default Path */
    HI_S32 s32ModuleNum = iniparser_getint(pstCfgPos->stModule.pstIniDir, (HI_CHAR *)"module:module_num", 0);;
    *pu32ModuleNum = s32ModuleNum;
    MLOGD("ModuleNum[%d], DefaultPath[%s]\n", s32ModuleNum, pazModuleDefPath);

    /* Get All Module Information */
    HI_S32 s32Idx;
    HI_S32 s32ModuleCnt = 0;
    for (s32Idx = 0; s32Idx < s32ModuleNum; ++s32Idx)
    {
        /* Malloc ModuleNode Memory */
        CONFACCESS_MODULE_S *pstModuleInfo = (CONFACCESS_MODULE_S *)malloc(sizeof(CONFACCESS_MODULE_S));
        if (NULL == pstModuleInfo)
        {
            MLOGE("malloc failed\n");
            CONFACCESS_DelModuleList(pstCfgPos);
            iniparser_freedict(pstCfgPos->stModule.pstIniDir);
            HI_MUTEX_UNLOCK(pstCfgPos->MutexLock);
            HI_MUTEX_DESTROY(pstCfgPos->MutexLock);
            HI_SCENECOMM_SAFE_FREE(pstCfgPos);
            return HI_CONFACCESS_EMALLOC;
        }

        /* Module Name */
        const HI_CHAR *pazString = NULL;
        HI_CHAR szModuleNodeKey[HI_CONFACCESS_KEY_MAX_LEN] = {0,};
        snprintf(szModuleNodeKey, HI_CONFACCESS_KEY_MAX_LEN, "module:module%d", s32Idx + 1);
        pazString = iniparser_getstring(pstCfgPos->stModule.pstIniDir, szModuleNodeKey, NULL);
        if (NULL == pazString)
        {
            MLOGE("Load %s failed\n", szModuleNodeKey);
            HI_SCENECOMM_SAFE_FREE(pstModuleInfo);
            continue;
        }
        snprintf(pstModuleInfo->szName, HI_CONFACCESS_NAME_MAX_LEN, "%s", pazString);

        /* Module Path */
        snprintf(pstModuleInfo->szPath, HI_CONFACCESS_PATH_MAX_LEN, "%s/", aszCfgPath);

        if ((strlen(pstModuleInfo->szPath) > 1)
            && ((pstModuleInfo->szPath[strlen(pstModuleInfo->szPath) - 1]) == CONFACCESS_PATH_SEPARATOR))
        {
            pstModuleInfo->szPath[strlen(pstModuleInfo->szPath) - 1] = '\0';
        }

        /* Module IniFile */
        snprintf(szModuleNodeKey, HI_CONFACCESS_KEY_MAX_LEN, "%s:cfg_filename", pstModuleInfo->szName);
        pazString = iniparser_getstring(pstCfgPos->stModule.pstIniDir, szModuleNodeKey, NULL);
        if (NULL == pazString)
        {
            MLOGE("Load %s failed\n", szModuleNodeKey);
            HI_SCENECOMM_SAFE_FREE(pstModuleInfo);
            continue;
        }

        snprintf(pstModuleInfo->szIniFile, HI_CONFACCESS_PATH_MAX_LEN, "%s%c%s",
                 pstModuleInfo->szPath, CONFACCESS_PATH_SEPARATOR, pazString);



        /* Module IniDir */
        //MLOGD("Module[%02d] IniFile[%s]\n", s32Idx, pstModuleInfo->szIniFile);
        pstModuleInfo->pstIniDir = iniparser_load(pstModuleInfo->szIniFile);
        if (NULL == pstModuleInfo->pstIniDir)
        {
            MLOGE("Load %s failed\n", pstModuleInfo->szIniFile);
            HI_SCENECOMM_SAFE_FREE(pstModuleInfo);
            continue;
        }

        /* Add ModuleNode to list */
        HI_List_Add(&(pstModuleInfo->stList), &(pstCfgPos->stModule.stList));
        s32ModuleCnt++;
    }
    HI_MUTEX_UNLOCK(pstCfgPos->MutexLock);

    /* Add CfgNode to list */
    HI_List_Add(&(pstCfgPos->stList), &(s_stCONFACCESSList));

    s_s32CONFACCESSCount++;
    MLOGD("New CfgName[%s] ModuleCnt[%d]\n", pstCfgPos->szCfgName, s32ModuleCnt);
    MLOGD("Now CfgList Count[%d]\n", s_s32CONFACCESSCount);
    return HI_SUCCESS;
}

HI_S32 HI_CONFACCESS_Init(const HI_CHAR *pszCfgName, const HI_CHAR *pszCfgPath, HI_U32 *pu32ModuleNum)
{
    HI_SCENECOMM_CHECK_POINTER(pszCfgName, HI_CONFACCESS_EINVAL);
    HI_SCENECOMM_CHECK_POINTER(pszCfgPath, HI_CONFACCESS_EINVAL);
    HI_SCENECOMM_CHECK_EXPR(strlen(pszCfgName) < HI_CONFACCESS_NAME_MAX_LEN, HI_CONFACCESS_EINVAL);
    HI_SCENECOMM_CHECK_EXPR(strlen(pszCfgPath) < HI_CONFACCESS_PATH_MAX_LEN, HI_CONFACCESS_EINVAL);

    HI_S32 s32Ret = HI_SUCCESS;

    /* Check CfgName Exist or not */
    HI_List_Head_S *pstList = NULL;
    CONFACCESS_CFG_NODE_S *pstCfgPos = NULL;
    HI_List_For_Each(pstList, &(s_stCONFACCESSList))
    {
        pstCfgPos = HI_LIST_ENTRY(pstList, CONFACCESS_CFG_NODE_S, stList);
        if (0 == strncmp(pstCfgPos->szCfgName, pszCfgName, HI_CONFACCESS_NAME_MAX_LEN))
        {
            MLOGW("%s already be inited\n", pszCfgName);
            return HI_CONFACCESS_EREINIT;
        }
    }

    /* Init Cfg */
    if (0 == s_s32CONFACCESSCount)
    {
        s_stCONFACCESSList.next = &(s_stCONFACCESSList);
        s_stCONFACCESSList.prev = &(s_stCONFACCESSList);
    }

    s32Ret = CONFACCESS_Init(pszCfgName, pszCfgPath, pu32ModuleNum);
    HI_SCENECOMM_CHECK_RETURN_WITH_ERRINFO(s32Ret, s32Ret, "AddCfg");
    return HI_SUCCESS;
}

HI_S32 HI_CONFACCESS_Deinit(const HI_CHAR *pszCfgName)
{
    HI_SCENECOMM_CHECK_POINTER(pszCfgName, HI_CONFACCESS_EINVAL);
    HI_SCENECOMM_CHECK_EXPR(strlen(pszCfgName) < HI_CONFACCESS_NAME_MAX_LEN, HI_CONFACCESS_EINVAL);
    HI_SCENECOMM_CHECK_EXPR(s_s32CONFACCESSCount > 0, HI_CONFACCESS_ENOTINIT);

    HI_List_Head_S *pstCfgList = NULL;
    HI_List_Head_S *pstModuleList = NULL;
    CONFACCESS_CFG_NODE_S *pstCfgPos = NULL;
    CONFACCESS_MODULE_S *pstModulePos = NULL;
    HI_S32 s32ModuleCnt = 0;

    HI_List_For_Each (pstCfgList, &(s_stCONFACCESSList))
    {
        pstCfgPos = HI_LIST_ENTRY(pstCfgList, CONFACCESS_CFG_NODE_S, stList);
        if (0 == strncmp(pstCfgPos->szCfgName, pszCfgName, HI_CONFACCESS_NAME_MAX_LEN))
        {
            HI_MUTEX_LOCK(pstCfgPos->MutexLock);
            HI_List_Del(pstCfgList);
            HI_List_For_Each(pstModuleList, &(pstCfgPos->stModule.stList))
            {
                HI_List_Del(pstModuleList);
                pstModulePos = HI_LIST_ENTRY(pstModuleList, CONFACCESS_MODULE_S, stList);
                //MLOGD("Module: inifile[%s]\n", pstModulePos->szIniFile);
                s32ModuleCnt++;
                iniparser_freedict(pstModulePos->pstIniDir);
                pstModulePos->pstIniDir = NULL;
                HI_SCENECOMM_SAFE_FREE(pstModulePos);
                pstModuleList = &(pstCfgPos->stModule.stList);
            }
            MLOGD("CfgName[%s] ModuleCnt[%d]\n", pstCfgPos->szCfgName, s32ModuleCnt);
            iniparser_freedict(pstCfgPos->stModule.pstIniDir);
            pstCfgPos->stModule.pstIniDir = NULL;
            HI_MUTEX_UNLOCK(pstCfgPos->MutexLock);
            HI_MUTEX_DESTROY(pstCfgPos->MutexLock);
            HI_SCENECOMM_SAFE_FREE(pstCfgPos);

            pstCfgList = &(s_stCONFACCESSList);
            s_s32CONFACCESSCount--;
            MLOGD("Now CfgList count[%d]\n", s_s32CONFACCESSCount);
            return HI_SUCCESS;
        }
    }
    return HI_CONFACCESS_ECFG_NOTEXIST;
}

HI_S32 HI_CONFACCESS_GetString(const HI_CHAR *pszCfgName, const HI_CHAR *pszModule,
                               const HI_CHAR *pszConfItem, HI_CHAR *pszDefault, HI_CHAR **const ppszValue)
{
    HI_SCENECOMM_CHECK_POINTER(pszCfgName, HI_CONFACCESS_EINVAL);
    HI_SCENECOMM_CHECK_POINTER(pszModule, HI_CONFACCESS_EINVAL);
    HI_SCENECOMM_CHECK_POINTER(pszConfItem, HI_CONFACCESS_EINVAL);
    HI_SCENECOMM_CHECK_POINTER(ppszValue, HI_CONFACCESS_EINVAL);
    HI_SCENECOMM_CHECK_EXPR(strlen(pszCfgName) < HI_CONFACCESS_NAME_MAX_LEN, HI_CONFACCESS_EINVAL);
    HI_SCENECOMM_CHECK_EXPR(strlen(pszModule) < HI_CONFACCESS_NAME_MAX_LEN, HI_CONFACCESS_EINVAL);
    HI_SCENECOMM_CHECK_EXPR(strlen(pszConfItem) < HI_CONFACCESS_KEY_MAX_LEN, HI_CONFACCESS_EINVAL);

    HI_List_Head_S *pstCfgList = NULL;
    HI_List_Head_S *pstModuleList = NULL;
    CONFACCESS_CFG_NODE_S *pstCfgPos = NULL;
    CONFACCESS_MODULE_S *pstModulePos = NULL;

    HI_List_For_Each(pstCfgList, &(s_stCONFACCESSList))
    {
        pstCfgPos = HI_LIST_ENTRY(pstCfgList, CONFACCESS_CFG_NODE_S, stList);
        if (0 == strncmp(pstCfgPos->szCfgName, pszCfgName, HI_CONFACCESS_NAME_MAX_LEN))
        {
            HI_MUTEX_LOCK(pstCfgPos->MutexLock);
            HI_List_For_Each(pstModuleList, &(pstCfgPos->stModule.stList))
            {
                pstModulePos = HI_LIST_ENTRY(pstModuleList, CONFACCESS_MODULE_S, stList);
                if (0 == strncmp(pstModulePos->szName, pszModule, HI_CONFACCESS_NAME_MAX_LEN))
                {
                    *ppszValue = (HI_CHAR *)iniparser_getstring(pstModulePos->pstIniDir, pszConfItem, pszDefault);
                    if (NULL != *ppszValue)
                    {
                        *ppszValue = strdup(*ppszValue);
                    }
                    HI_MUTEX_UNLOCK(pstCfgPos->MutexLock);
                    return HI_SUCCESS;
                }
            }
            HI_MUTEX_UNLOCK(pstCfgPos->MutexLock);
            return HI_CONFACCESS_EMOD_NOTEXIST;
        }
    }
    return HI_CONFACCESS_ECFG_NOTEXIST;
}

HI_S32 HI_CONFACCESS_GetStr(const HI_CHAR *pszCfgName, const HI_CHAR *pszModule,
                            const HI_CHAR  *pszConfItem, HI_CHAR **ppszValue)
{
    return HI_CONFACCESS_GetString(pszCfgName, pszModule, pszConfItem, NULL, ppszValue);
}

HI_S32 HI_CONFACCESS_GetInt(const HI_CHAR *pszCfgName, const HI_CHAR *pszModule,
                            const HI_CHAR *pszConfItem, HI_S32 s32Notfound, HI_S32 *const ps32Value)
{
    HI_CHAR *pszValue = NULL;
    HI_S32 s32Ret = HI_CONFACCESS_GetString(pszCfgName, pszModule, pszConfItem, NULL, &pszValue);
    HI_SCENECOMM_CHECK_RETURN(s32Ret, s32Ret);

    if (NULL == pszValue)
    {
        *ps32Value = s32Notfound;
    }
    else
    {
        *ps32Value = atoi(pszValue);
        HI_SCENECOMM_SAFE_FREE(pszValue);
    }
    return HI_SUCCESS;
}

HI_S32 HI_CONFACCESS_GetLonglong(const HI_CHAR *pszCfgName, const HI_CHAR *pszModule,
                                 const HI_CHAR  *pszConfItem, HI_S32 s32Notfound, HI_S64 *const ps64Value)
{
    HI_CHAR *pszValue = NULL;
    HI_S32 s32Rtn;
    s32Rtn = HI_CONFACCESS_GetString(pszCfgName, pszModule, pszConfItem, NULL, &pszValue);
    HI_SCENECOMM_CHECK_RETURN(s32Rtn, s32Rtn);
    if (HI_SUCCESS != s32Rtn)
    {
        return s32Rtn;
    }

    if (NULL == pszValue)
    {
        *ps64Value = (long long)s32Notfound;
    }
    else
    {
        *ps64Value = atoll(pszValue);
        HI_SCENECOMM_SAFE_FREE(pszValue);
    }

    return s32Rtn;
}


HI_S32 HI_CONFACCESS_GetDouble(const HI_CHAR *pszCfgName, const HI_CHAR *pszModule,
                               const HI_CHAR *pszConfItem, HI_DOUBLE dNotfound, HI_DOUBLE *const pdValue)
{
    HI_CHAR *pszValue = NULL;
    HI_S32 s32Ret = HI_CONFACCESS_GetString(pszCfgName, pszModule, pszConfItem, NULL, &pszValue);
    HI_SCENECOMM_CHECK_RETURN(s32Ret, s32Ret);

    if (NULL == pszValue)
    {
        *pdValue = dNotfound;
    }
    else
    {
        *pdValue = atof(pszValue);
        HI_SCENECOMM_SAFE_FREE(pszValue);
    }
    return HI_SUCCESS;
}


HI_S32 HI_CONFACCESS_GetBool(const HI_CHAR *pszCfgName, const HI_CHAR *pszModule,
                             const HI_CHAR *pszConfItem, HI_BOOL bNotfound, HI_BOOL *pbValue)
{
    HI_CHAR *pszString = NULL;
    HI_S32 s32Ret = HI_CONFACCESS_GetString(pszCfgName, pszModule, pszConfItem, NULL, &pszString);
    HI_SCENECOMM_CHECK_RETURN(s32Ret, s32Ret);

    if (NULL == pszString)
    {
        *pbValue = bNotfound;
    }
    else
    {
        if ((pszString[0] == 'y') || (pszString[0] == 'Y')
            || (pszString[0] == '1') || (pszString[0] == 't') || (pszString[0] == 'T'))
        {
            *pbValue = HI_TRUE;
        }
        else if ((pszString[0] == 'n') || (pszString[0] == 'N')
                 || (pszString[0] == '0') || (pszString[0] == 'f') || pszString[0] == 'F')
        {
            *pbValue = HI_FALSE;
        }
        else
        {
            *pbValue = bNotfound ;
        }
        HI_SCENECOMM_SAFE_FREE(pszString);
    }
    return HI_SUCCESS;
}

HI_S32 HI_CONFACCESS_SetString(const HI_CHAR *pszCfgName, const HI_CHAR *pszModule,
                               const HI_CHAR *pszConfItem, const HI_CHAR *pszValue)
{
    HI_SCENECOMM_CHECK_POINTER(pszCfgName, HI_CONFACCESS_EINVAL);
    HI_SCENECOMM_CHECK_POINTER(pszModule, HI_CONFACCESS_EINVAL);
    HI_SCENECOMM_CHECK_POINTER(pszConfItem, HI_CONFACCESS_EINVAL);
    HI_SCENECOMM_CHECK_POINTER(pszValue, HI_CONFACCESS_EINVAL);
    HI_SCENECOMM_CHECK_EXPR(strlen(pszCfgName) < HI_CONFACCESS_NAME_MAX_LEN, HI_CONFACCESS_EINVAL);
    HI_SCENECOMM_CHECK_EXPR(strlen(pszModule) < HI_CONFACCESS_NAME_MAX_LEN, HI_CONFACCESS_EINVAL);
    HI_SCENECOMM_CHECK_EXPR(strlen(pszConfItem) < HI_CONFACCESS_KEY_MAX_LEN, HI_CONFACCESS_EINVAL);

    HI_List_Head_S *pstCfgList = NULL;
    HI_List_Head_S *pstModuleList = NULL;
    CONFACCESS_CFG_NODE_S *pstCfgPos = NULL;
    CONFACCESS_MODULE_S *pstModulePos = NULL;

    HI_List_For_Each(pstCfgList, &(s_stCONFACCESSList))
    {
        pstCfgPos = HI_LIST_ENTRY(pstCfgList, CONFACCESS_CFG_NODE_S, stList);
        if (0 == strncmp(pstCfgPos->szCfgName, pszCfgName, HI_CONFACCESS_NAME_MAX_LEN))
        {
            HI_MUTEX_LOCK(pstCfgPos->MutexLock);
            HI_List_For_Each(pstModuleList, &(pstCfgPos->stModule.stList))
            {
                pstModulePos = HI_LIST_ENTRY(pstModuleList, CONFACCESS_MODULE_S, stList);
                if (0 == strncmp(pstModulePos->szName, pszModule, HI_CONFACCESS_NAME_MAX_LEN))
                {
                    if (0 != iniparser_set(pstModulePos->pstIniDir, pszConfItem, pszValue))
                    {
                        MLOGE("module[%s] confitem[%s] not exist\n", pszModule, pszConfItem);
                        HI_MUTEX_UNLOCK(pstCfgPos->MutexLock);
                        return HI_CONFACCESS_EITEM_NOTEXIST;
                    }
                    HI_MUTEX_UNLOCK(pstCfgPos->MutexLock);
                    return HI_SUCCESS;
                }
            }
            HI_MUTEX_UNLOCK(pstCfgPos->MutexLock);
            return HI_CONFACCESS_EMOD_NOTEXIST;
        }
    }
    return HI_CONFACCESS_ECFG_NOTEXIST;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

