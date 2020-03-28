/**
 * @file      hi_product_scene_ini2bin.c
 * @brief     ini2bin implementation
 *
 * Copyright (c) 2017 Huawei Tech.Co.,Ltd
 *
 * @author    HiMobileCam Reference Develop Team
 * @date      2017/12/13
 * @version   1.0

 */

#include <stdio.h>
#include <sys/time.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include "hi_scene_setparam.h"
#include "hi_confaccess.h"
#include "hi_scenecomm.h"
#include "hi_scene_loadparam.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/** param config file path */
#define SCENE_INIPARAM  "sceneparamini"
//#define SCENETOOL_INIPARAM_PATH "./CFG_SENSOR_TYPE0/param/config_cfgaccess_hd.ini"
#define SCENE_INI_SCENEMODE "scene_param_"
#define SCENE_INIPARAM_MODULE_NAME_LEN (64)
#define SCENE_INIPARAM_NODE_NAME_LEN  (128)
#define SCENETOOL_MAX_FILESIZE (128)

#define SCENE_INI_VIDEOMODE "scene_mode"

#define SCENE_INIPARAM_CHECK_LOAD_RESULT(ret, name) \
    do{ \
        if (HI_SUCCESS != ret){   \
            MLOGE(" Load [%s] failed\n", name); \
            return HI_FAILURE;  \
        }   \
    }while(0)

HI_BOOL g_bLogOn = 1;

/** param */
HI_SCENE_PARAM_S s_stSceneParamCfg;

static HI_S64 s_as64LineNum[5000];

HI_SCENE_VIDEO_MODE_S s_stVideoMode;

static HI_S32  SCENE_GetNumbersInOneLine(HI_CHAR *pszInputLine)
{
    HI_CHAR    *pszVRBegin     = pszInputLine;
    HI_CHAR    *pszVREnd       = pszVRBegin;
    HI_U32      u32PartCount = 0;
    HI_CHAR     szPart[20] = {0};
    HI_U32      u32WholeCount = 0;
    HI_S32      s32Length = strlen(pszInputLine);
    HI_U64      u64Hex;

    HI_S32      i = 0;
    HI_BOOL     bHex = HI_FALSE;
    memset(s_as64LineNum, 0 , sizeof(HI_S64) * 5000);
    while ((pszVREnd != NULL))
    {
        if ((u32WholeCount > s32Length) || (u32WholeCount == s32Length))
        {
            break;
        }

        while ((*pszVREnd != '|') && (*pszVREnd != '\0') && (*pszVREnd != ','))
        {
            if (*pszVREnd == 'x')
            {
                bHex = HI_TRUE;
            }
            pszVREnd++;
            u32PartCount++;
            u32WholeCount++;
        }

        memcpy(szPart, pszVRBegin, u32PartCount);

        if (bHex == HI_TRUE)
        {
            HI_CHAR *pszstr;
            u64Hex = (HI_U64)strtoll(szPart + 2, &pszstr, 16);
            s_as64LineNum[i] = u64Hex;
        }
        else
        {
            s_as64LineNum[i] = atoll(szPart);
        }

        memset(szPart, 0, 20);
        u32PartCount = 0;
        pszVREnd++;
        pszVRBegin = pszVREnd;
        u32WholeCount++;
        i++;
    }

    return i;
}

HI_S32 SCENE_LoadAeWeightTab(const HI_CHAR *pszIniModule, HI_SCENE_STATIC_STATISTICSCFG_S *pstStaticStatistics)
{

    HI_S32 s32Ret = HI_SUCCESS;
    HI_CHAR *pszString = NULL;
    HI_U32 u32IdxM = 0;
    HI_U32 u32IdxN = 0;
    HI_CHAR aszIniNodeName[SCENE_INIPARAM_NODE_NAME_LEN] = {0,};

    snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "static_aeweight:ExpWeight_0");/*AEWeight*/
    s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
    SCENE_INIPARAM_CHECK_LOAD_RESULT(s32Ret, aszIniNodeName);
    if (HI_NULL != pszString)
    {
        free(pszString);
        pszString = HI_NULL;
        for (u32IdxM = 0; u32IdxM < AE_ZONE_ROW; u32IdxM++)
        {
            snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "static_aeweight:ExpWeight_%d", u32IdxM);
            s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
            if (HI_SUCCESS != s32Ret)
            {
                MLOGE("load %s failed\n", aszIniNodeName);
                return HI_FAILURE;
            }
            if (NULL != pszString)
            {
                SCENE_GetNumbersInOneLine(pszString);
                for (u32IdxN = 0; u32IdxN < AE_ZONE_COLUMN; u32IdxN++)
                {
                    pstStaticStatistics->au8AEWeight[u32IdxM][u32IdxN] = (HI_U8)s_as64LineNum[u32IdxN];
                }
                free(pszString);
                pszString = HI_NULL;
            }
        }
    }
    return HI_SUCCESS;
}

HI_S32 SCENE_LoadStaticAE(const HI_CHAR *pszIniModule, HI_SCENE_STATIC_AE_S *pstStaticAe)
{
    HI_S32 s32Ret = HI_SUCCESS;
    HI_CHAR *pszString = HI_NULL;
    HI_S32 s32Value = 0;
    HI_CHAR aszIniNodeName[SCENE_INIPARAM_NODE_NAME_LEN] = {0,};


    snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "static_ae:AERunInterval");/*AERunInterval*/
    s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
    SCENE_INIPARAM_CHECK_LOAD_RESULT(s32Ret, aszIniNodeName);
    if (HI_NULL != pszString)
    {
        free(pszString);
        pszString = HI_NULL;
        s32Ret = HI_CONFACCESS_GetInt(SCENE_INIPARAM, pszIniModule, "static_ae:AERunInterval", 0, &s32Value);
        if (HI_SUCCESS != s32Ret)
        {
            MLOGE("load static_ae:AERunInterval failed\n");
            return HI_FAILURE;
        }
        pstStaticAe->u8AERunInterval = (HI_U8)s32Value;
    }

    snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "static_ae:AERouteExValid");/*AERouteExValid*/
    s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
    SCENE_INIPARAM_CHECK_LOAD_RESULT(s32Ret, aszIniNodeName);
    if (HI_NULL != pszString)
    {
        free(pszString);
        pszString = HI_NULL;
        s32Ret = HI_CONFACCESS_GetInt(SCENE_INIPARAM, pszIniModule, "static_ae:AERouteExValid", 0, &s32Value);

        if (HI_SUCCESS != s32Ret)
        {
            MLOGE("load static_ae:AutoExpTimeMax failed\n");
            return HI_FAILURE;
        }
        pstStaticAe->bAERouteExValid = (HI_BOOL)s32Value;
    }

    snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "static_ae:AutoSysGainMax");/*AutoSysGainMax*/
    s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
    SCENE_INIPARAM_CHECK_LOAD_RESULT(s32Ret, aszIniNodeName);
    if (HI_NULL != pszString)
    {
        free(pszString);
        pszString = HI_NULL;
        s32Ret = HI_CONFACCESS_GetInt(SCENE_INIPARAM, pszIniModule, "static_ae:AutoSysGainMax", 0, &s32Value);
        if (HI_SUCCESS != s32Ret)
        {
            MLOGE("load static_ae:AutoSysGainMax failed\n");
            return HI_FAILURE;
        }
        pstStaticAe->u32AutoSysGainMax = (HI_U32)s32Value;
    }

    snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "static_ae:AutoSpeed");/*AutoSpeed*/
    s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
    SCENE_INIPARAM_CHECK_LOAD_RESULT(s32Ret, aszIniNodeName);
    if (HI_NULL != pszString)
    {
        free(pszString);
        pszString = HI_NULL;
        s32Ret = HI_CONFACCESS_GetInt(SCENE_INIPARAM, pszIniModule, "static_ae:AutoSpeed", 0, &s32Value);
        if (HI_SUCCESS != s32Ret)
        {
            MLOGE("load static_ae:AutoSpeed failed\n");
            return HI_FAILURE;
        }
        pstStaticAe->u8AutoSpeed = (HI_U8)s32Value;
    }

    snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "static_ae:AutoTolerance");/*AutoTolerance*/
    s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
    SCENE_INIPARAM_CHECK_LOAD_RESULT(s32Ret, aszIniNodeName);
    if (HI_NULL != pszString)
    {
        free(pszString);
        pszString = HI_NULL;
        s32Ret = HI_CONFACCESS_GetInt(SCENE_INIPARAM, pszIniModule, "static_ae:AutoTolerance", 0, &s32Value);
        if (HI_SUCCESS != s32Ret)
        {
            MLOGE("load static_ae:AutoTolerance failed\n");
            return HI_FAILURE;
        }
        pstStaticAe->u8AutoTolerance = (HI_U32)s32Value;
    }

    snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "static_ae:AutoBlackDelayFrame");/*AutoBlackDelayFrame*/
    s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
    SCENE_INIPARAM_CHECK_LOAD_RESULT(s32Ret, aszIniNodeName);
    if (HI_NULL != pszString)
    {
        free(pszString);
        pszString = HI_NULL;
        s32Ret = HI_CONFACCESS_GetInt(SCENE_INIPARAM, pszIniModule, "static_ae:AutoBlackDelayFrame", 0, &s32Value);
        if (HI_SUCCESS != s32Ret)
        {
            MLOGE("load static_ae:AutoBlackDelayFrame failed\n");
            return HI_FAILURE;
        }
        pstStaticAe->u16AutoBlackDelayFrame = (HI_U16)s32Value;
    }

    snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "static_ae:AutoWhiteDelayFrame");/*AutoWhiteDelayFrame*/
    s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
    SCENE_INIPARAM_CHECK_LOAD_RESULT(s32Ret, aszIniNodeName);
    if (HI_NULL != pszString)
    {
        free(pszString);
        pszString = HI_NULL;
        s32Ret = HI_CONFACCESS_GetInt(SCENE_INIPARAM, pszIniModule, "static_ae:AutoWhiteDelayFrame", 0, &s32Value);
        if (HI_SUCCESS != s32Ret)
        {
            MLOGE("load static_ae:AutoWhiteDelayFrame failed\n");
            return HI_FAILURE;
        }
        pstStaticAe->u16AutoWhiteDelayFrame = (HI_U16)s32Value;
    }

    return HI_SUCCESS;
}

HI_S32 SCENE_LoadStaticAERoute(const HI_CHAR *pszIniModule, HI_SCENE_STATIC_AEROUTE_S *pstStaticAeRoute)
{

    HI_S32 s32Ret = HI_SUCCESS;
    HI_CHAR *pszString = NULL;
    HI_S32 s32Value = 0;
	HI_U32 u32IdxM;
    HI_CHAR aszIniNodeName[SCENE_INIPARAM_NODE_NAME_LEN] = {0,};

    snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "static_aeroute:TotalNum");/*TotalNum*/
    s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
    SCENE_INIPARAM_CHECK_LOAD_RESULT(s32Ret, aszIniNodeName);
    if (HI_NULL != pszString)
    {
        free(pszString);
        pszString = HI_NULL;
        s32Ret = HI_CONFACCESS_GetInt(SCENE_INIPARAM, pszIniModule, "static_aeroute:TotalNum", 0, &s32Value);
        if (HI_SUCCESS != s32Ret)
        {
            MLOGE("load static_aeroute:u32TotalNum failed\n");
            return HI_FAILURE;
        }
        pstStaticAeRoute->u32TotalNum = (HI_U32)s32Value;
    }

    snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "static_aeroute:RouteIntTime");/*RouteIntTime*/
    s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
    SCENE_INIPARAM_CHECK_LOAD_RESULT(s32Ret, aszIniNodeName);
    if (HI_NULL != pszString)
    {
        SCENE_GetNumbersInOneLine(pszString);
        for (u32IdxM = 0; u32IdxM < pstStaticAeRoute->u32TotalNum; u32IdxM++)
        {
            pstStaticAeRoute->au32IntTime[u32IdxM] = s_as64LineNum[u32IdxM];
        }
        free(pszString);
        pszString = HI_NULL;
    }

    snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "static_aeroute:RouteSysGain");/*RouteSysGain*/
    s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
    SCENE_INIPARAM_CHECK_LOAD_RESULT(s32Ret, aszIniNodeName);
    if (HI_NULL != pszString)
    {
        SCENE_GetNumbersInOneLine(pszString);
        for (u32IdxM = 0; u32IdxM < pstStaticAeRoute->u32TotalNum; u32IdxM++)
        {
            pstStaticAeRoute->au32SysGain[u32IdxM] = s_as64LineNum[u32IdxM];
        }
        free(pszString);
        pszString = HI_NULL;
    }

    return HI_SUCCESS;
}

HI_S32 SCENE_LoadStaticAeRouteEX(const HI_CHAR *pszIniModule, HI_SCENE_STATIC_AEROUTEEX_S *pstStaticAeRouteEx)
{
    HI_S32 s32Ret = HI_SUCCESS;
    HI_CHAR *pszString = NULL;
    HI_U32 u32IdxM = 0;
    HI_S32 s32Value = 0;
    HI_CHAR aszIniNodeName[SCENE_INIPARAM_NODE_NAME_LEN] = {0,};

    snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "static_aerouteex:TotalNum");/*Tolerance*/
    s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
    SCENE_INIPARAM_CHECK_LOAD_RESULT(s32Ret, aszIniNodeName);
    if (HI_NULL != pszString)
    {
        free(pszString);
        pszString = HI_NULL;
        s32Ret = HI_CONFACCESS_GetInt(SCENE_INIPARAM, pszIniModule, "static_aerouteex:TotalNum", 0, &s32Value);
        if (HI_SUCCESS != s32Ret)
        {
            MLOGE("load static_aerouteex:TotalNum failed\n");
            return HI_FAILURE;
        }
        pstStaticAeRouteEx->u32TotalNum = (HI_U32)s32Value;
    }

    snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "static_aerouteex:RouteEXIntTime");/*RouteEXIntTime*/
    s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
    SCENE_INIPARAM_CHECK_LOAD_RESULT(s32Ret, aszIniNodeName);
    if (HI_NULL != pszString)
    {
        SCENE_GetNumbersInOneLine(pszString);
        for (u32IdxM = 0; u32IdxM < pstStaticAeRouteEx->u32TotalNum; u32IdxM++)
        {
            pstStaticAeRouteEx->au32IntTime[u32IdxM] = s_as64LineNum[u32IdxM];
        }
        free(pszString);
        pszString = HI_NULL;
    }

    snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "static_aerouteex:RouteEXAGain");/*RouteEXAGain*/
    s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
    SCENE_INIPARAM_CHECK_LOAD_RESULT(s32Ret, aszIniNodeName);
    if (HI_NULL != pszString)
    {
        SCENE_GetNumbersInOneLine(pszString);
        for (u32IdxM = 0; u32IdxM < pstStaticAeRouteEx->u32TotalNum; u32IdxM++)
        {
            pstStaticAeRouteEx->au32Again[u32IdxM] = s_as64LineNum[u32IdxM];
        }
        free(pszString);
        pszString = HI_NULL;
    }

    snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "static_aerouteex:RouteEXDGain");/*RouteEXDGain*/
    s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
    SCENE_INIPARAM_CHECK_LOAD_RESULT(s32Ret, aszIniNodeName);
    if (HI_NULL != pszString)
    {
        SCENE_GetNumbersInOneLine(pszString);
        for (u32IdxM = 0; u32IdxM < pstStaticAeRouteEx->u32TotalNum; u32IdxM++)
        {
            pstStaticAeRouteEx->au32Dgain[u32IdxM] = s_as64LineNum[u32IdxM];
        }
        free(pszString);
        pszString = HI_NULL;
    }

    snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "static_aerouteex:RouteEXISPDGain");/*RouteEXISPDGain*/
    s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
    SCENE_INIPARAM_CHECK_LOAD_RESULT(s32Ret, aszIniNodeName);
    if (HI_NULL != pszString)
    {
        SCENE_GetNumbersInOneLine(pszString);
        for (u32IdxM = 0; u32IdxM < pstStaticAeRouteEx->u32TotalNum; u32IdxM++)
        {
            pstStaticAeRouteEx->au32IspDgain[u32IdxM] = s_as64LineNum[u32IdxM];
        }
        free(pszString);
        pszString = HI_NULL;
    }

    return HI_SUCCESS;
}

HI_S32 SCENE_LoadDynamicAE(const HI_CHAR *pszIniModule, HI_SCENE_DYNAMIC_AE_S *pstDynamicAe)
{
    HI_S32 s32Ret = HI_SUCCESS;
    HI_CHAR *pszString = NULL;
    HI_S32 s32Value = 0;
    HI_U32 u32IdxM = 0;
    HI_CHAR aszIniNodeName[SCENE_INIPARAM_NODE_NAME_LEN] = {0,};

    snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "dynamic_ae:aeExpCount");/*aeExpCount*/
    s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
    SCENE_INIPARAM_CHECK_LOAD_RESULT(s32Ret, aszIniNodeName);
    if (HI_NULL != pszString)
    {
        free(pszString);
        pszString = HI_NULL;
        s32Ret = HI_CONFACCESS_GetInt(SCENE_INIPARAM, pszIniModule, "dynamic_ae:aeExpCount", 0, &s32Value);
        if (HI_SUCCESS != s32Ret)
        {
            MLOGE("load dynamic_ae:aeExpCount failed\n");
            return HI_FAILURE;
        }
        pstDynamicAe->u8AEExposureCnt = (HI_U8)s32Value;
    }

    snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "dynamic_ae:aeExpLtoHThresh");/*aeExpLtoHThresh*/
    s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
    SCENE_INIPARAM_CHECK_LOAD_RESULT(s32Ret, aszIniNodeName);
    if (HI_NULL != pszString)
    {
        SCENE_GetNumbersInOneLine(pszString);
        for (u32IdxM = 0; u32IdxM < pstDynamicAe->u8AEExposureCnt; u32IdxM++)
        {
            pstDynamicAe->au64ExpLtoHThresh[u32IdxM] = s_as64LineNum[u32IdxM];
        }
        free(pszString);
        pszString = HI_NULL;
    }

    snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "dynamic_ae:aeExpHtoLThresh");/*aeExpHtoLThresh*/
    s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
    SCENE_INIPARAM_CHECK_LOAD_RESULT(s32Ret, aszIniNodeName);
    if (HI_NULL != pszString)
    {
        SCENE_GetNumbersInOneLine(pszString);
        for (u32IdxM = 0; u32IdxM < pstDynamicAe->u8AEExposureCnt; u32IdxM++)
        {
            pstDynamicAe->au64ExpHtoLThresh[u32IdxM] = s_as64LineNum[u32IdxM];
        }
        free(pszString);
        pszString = HI_NULL;
    }

    snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "dynamic_ae:AutoCompesation");/*AutoCompensation*/
    s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
    SCENE_INIPARAM_CHECK_LOAD_RESULT(s32Ret, aszIniNodeName);
    if (HI_NULL != pszString)
    {
        SCENE_GetNumbersInOneLine(pszString);
        for (u32IdxM = 0; u32IdxM < pstDynamicAe->u8AEExposureCnt; u32IdxM++)
        {
            pstDynamicAe->au8AutoCompensation[u32IdxM] = s_as64LineNum[u32IdxM];
        }
        free(pszString);
        pszString = HI_NULL;
    }

    snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "dynamic_ae:AutoHistOffset");/*AutoHistOffset*/
    s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
    SCENE_INIPARAM_CHECK_LOAD_RESULT(s32Ret, aszIniNodeName);
    if (HI_NULL != pszString)
    {
        SCENE_GetNumbersInOneLine(pszString);
        for (u32IdxM = 0; u32IdxM < pstDynamicAe->u8AEExposureCnt; u32IdxM++)
        {
            pstDynamicAe->au8AutoMaxHistOffset[u32IdxM] = s_as64LineNum[u32IdxM];
        }
        free(pszString);
        pszString = HI_NULL;
    }

    snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "dynamic_ae:AutoExpRatioMax");/*AutoExpRatioMax*/
    s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
    SCENE_INIPARAM_CHECK_LOAD_RESULT(s32Ret, aszIniNodeName);
    if (HI_NULL != pszString)
    {
        SCENE_GetNumbersInOneLine(pszString);
        for (u32IdxM = 0; u32IdxM < pstDynamicAe->u8AEExposureCnt; u32IdxM++)
        {
            pstDynamicAe->au32AutoExpRatioMax[u32IdxM] = s_as64LineNum[u32IdxM];
        }
        free(pszString);
        pszString = HI_NULL;
    }

    snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "dynamic_ae:AutoExpRatioMin");/*AutoExpRatioMin*/
    s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
    SCENE_INIPARAM_CHECK_LOAD_RESULT(s32Ret, aszIniNodeName);
    if (HI_NULL != pszString)
    {
        SCENE_GetNumbersInOneLine(pszString);
        for (u32IdxM = 0; u32IdxM < pstDynamicAe->u8AEExposureCnt; u32IdxM++)
        {
            pstDynamicAe->au32AutoExpRatioMin[u32IdxM] = s_as64LineNum[u32IdxM];
        }
        free(pszString);
        pszString = HI_NULL;
    }

    return HI_SUCCESS;
}


HI_S32 SCENE_LoadStaticAWB(const HI_CHAR *pszIniModule, HI_SCENE_STATIC_AWB_S *pstStaticAwb)
{
    HI_S32 s32Ret = HI_SUCCESS;
    HI_CHAR *pszString = NULL;
    HI_U32 u32IdxM = 0;
    HI_S32 s32Value = 0;
    HI_CHAR aszIniNodeName[SCENE_INIPARAM_NODE_NAME_LEN] = {0,};

	snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "static_awb:AutoStaticWb");/*AutoStaticWb*/
    s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
    SCENE_INIPARAM_CHECK_LOAD_RESULT(s32Ret, aszIniNodeName);
    if (HI_NULL != pszString)
    {
        SCENE_GetNumbersInOneLine(pszString);
        for (u32IdxM = 0; u32IdxM < 4; u32IdxM++)
        {
            pstStaticAwb->au16AutoStaticWB[u32IdxM] = s_as64LineNum[u32IdxM];
        }
        free(pszString);
        pszString = HI_NULL;
    }

    snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "static_awb:AutoCurvePara");/*AutoCurvePara*/
    s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
    SCENE_INIPARAM_CHECK_LOAD_RESULT(s32Ret, aszIniNodeName);
    if (HI_NULL != pszString)
    {
        SCENE_GetNumbersInOneLine(pszString);
        for (u32IdxM = 0; u32IdxM < 6; u32IdxM++)
        {
            pstStaticAwb->as32AutoCurvePara[u32IdxM] = s_as64LineNum[u32IdxM];
        }
        free(pszString);
        pszString = HI_NULL;
    }

    snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "static_awb:AutoSpeed");/*AutoSpeed*/
    s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
    SCENE_INIPARAM_CHECK_LOAD_RESULT(s32Ret, aszIniNodeName);
    if (HI_NULL != pszString)
    {
        free(pszString);
        pszString = HI_NULL;
        s32Ret = HI_CONFACCESS_GetInt(SCENE_INIPARAM, pszIniModule, "static_awb:AutoSpeed", 0, &s32Value);
        if (HI_SUCCESS != s32Ret)
        {
            MLOGE("load static_awb:AutoSpeed failed\n");
            return HI_FAILURE;
        }
        pstStaticAwb->u16AutoSpeed = (HI_U16)s32Value;
    }

    snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "static_awb:AutoLowColorTemp");/*AutoLowColorTemp*/
    s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
    SCENE_INIPARAM_CHECK_LOAD_RESULT(s32Ret, aszIniNodeName);
    if (HI_NULL != pszString)
    {
        free(pszString);
        pszString = HI_NULL;
        s32Ret = HI_CONFACCESS_GetInt(SCENE_INIPARAM, pszIniModule, "static_awb:AutoLowColorTemp", 0, &s32Value);
        if (HI_SUCCESS != s32Ret)
        {
            MLOGE("load static_awb:AutoLowColorTemp failed\n");
            return HI_FAILURE;
        }
        pstStaticAwb->u16AutoLowColorTemp = (HI_U16)s32Value;
    }

    snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "static_awb:AutoCrMax");/*AutoCrMax*/
    s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
    SCENE_INIPARAM_CHECK_LOAD_RESULT(s32Ret, aszIniNodeName);
    if (HI_NULL != pszString)
    {
        SCENE_GetNumbersInOneLine(pszString);
        for (u32IdxM = 0; u32IdxM < ISP_AUTO_ISO_STRENGTH_NUM; u32IdxM++)
        {
            pstStaticAwb->au16AutoCrMax[u32IdxM] = s_as64LineNum[u32IdxM];
        }
        free(pszString);
        pszString = HI_NULL;
    }

    snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "static_awb:AutoCrMin");/*AutoCrMin*/
    s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
    SCENE_INIPARAM_CHECK_LOAD_RESULT(s32Ret, aszIniNodeName);
    if (HI_NULL != pszString)
    {
        SCENE_GetNumbersInOneLine(pszString);
        for (u32IdxM = 0; u32IdxM < ISP_AUTO_ISO_STRENGTH_NUM; u32IdxM++)
        {
            pstStaticAwb->au16AutoCrMin[u32IdxM] = s_as64LineNum[u32IdxM];
        }
        free(pszString);
        pszString = HI_NULL;
    }

    snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "static_awb:AutoCbMax");/*AutoCbMax*/
    s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
    SCENE_INIPARAM_CHECK_LOAD_RESULT(s32Ret, aszIniNodeName);
    if (HI_NULL != pszString)
    {
        SCENE_GetNumbersInOneLine(pszString);
        for (u32IdxM = 0; u32IdxM < ISP_AUTO_ISO_STRENGTH_NUM; u32IdxM++)
        {
            pstStaticAwb->au16AutoCbMax[u32IdxM] = s_as64LineNum[u32IdxM];
        }
        free(pszString);
        pszString = HI_NULL;
    }

    snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "static_awb:AutoCbMin");/*AutoCbMin*/
    s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
    SCENE_INIPARAM_CHECK_LOAD_RESULT(s32Ret, aszIniNodeName);
    if (HI_NULL != pszString)
    {
        SCENE_GetNumbersInOneLine(pszString);
        for (u32IdxM = 0; u32IdxM < ISP_AUTO_ISO_STRENGTH_NUM; u32IdxM++)
        {
            pstStaticAwb->au16AutoCbMin[u32IdxM] = s_as64LineNum[u32IdxM];
        }
        free(pszString);
        pszString = HI_NULL;
    }

    return HI_SUCCESS;
}

HI_S32 SCENE_LoadStaticAWBEX(const HI_CHAR *pszIniModule, HI_SCENE_STATIC_AWBEX_S *pstStaticAwbEx)
{
    HI_S32 s32Ret = HI_SUCCESS;
    HI_CHAR *pszString = NULL;
    HI_S32 s32Value = 0;
    HI_U32 u32IdxM = 0;
    HI_CHAR aszIniNodeName[SCENE_INIPARAM_NODE_NAME_LEN] = {0,};

    snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "static_awbex:Tolerance");/*Tolerance*/
    s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
    SCENE_INIPARAM_CHECK_LOAD_RESULT(s32Ret, aszIniNodeName);
    if (HI_NULL != pszString)
    {
        s32Ret = HI_CONFACCESS_GetInt(SCENE_INIPARAM, pszIniModule, "static_awbex:Tolerance", 0, &s32Value);
        if (HI_SUCCESS != s32Ret)
        {
            MLOGE("load static_awbex:Tolerance failed\n");
            return HI_FAILURE;
        }
        pstStaticAwbEx->u8Tolerance = (HI_U8)s32Value;

        free(pszString);
        pszString = HI_NULL;
    }

    snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "static_awbex:OutThresh");/*OutThresh*/
    s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
    SCENE_INIPARAM_CHECK_LOAD_RESULT(s32Ret, aszIniNodeName);
    if (HI_NULL != pszString)
    {
        s32Ret = HI_CONFACCESS_GetInt(SCENE_INIPARAM, pszIniModule, "static_awbex:OutThresh", 0, &s32Value);
        if (HI_SUCCESS != s32Ret)
        {
            MLOGE("load static_awbex:OutThresh failed\n");
            return HI_FAILURE;
        }
        pstStaticAwbEx->u32OutThresh = (HI_U32)s32Value;

        free(pszString);
        pszString = HI_NULL;
    }

    snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "static_awbex:LowStop");/*LowStop*/
    s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
    SCENE_INIPARAM_CHECK_LOAD_RESULT(s32Ret, aszIniNodeName);
    if (HI_NULL != pszString)
    {
        s32Ret = HI_CONFACCESS_GetInt(SCENE_INIPARAM, pszIniModule, "static_awbex:LowStop", 0, &s32Value);
        if (HI_SUCCESS != s32Ret)
        {
            MLOGE("load static_awbex:LowStop failed\n");
            return HI_FAILURE;
        }
        pstStaticAwbEx->u16LowStop = (HI_U16)s32Value;

        free(pszString);
        pszString = HI_NULL;
    }

    snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "static_awbex:HighStart");/*HighStart*/
    s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
    SCENE_INIPARAM_CHECK_LOAD_RESULT(s32Ret, aszIniNodeName);
    if (HI_NULL != pszString)
    {
        free(pszString);
        pszString = HI_NULL;
        s32Ret = HI_CONFACCESS_GetInt(SCENE_INIPARAM, pszIniModule, "static_awbex:HighStart", 0, &s32Value);
        if (HI_SUCCESS != s32Ret)
        {
            MLOGE("load static_awbex:HighStart failed\n");
            return HI_FAILURE;
        }
        pstStaticAwbEx->u16HighStart = (HI_U16)s32Value;
    }

    snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "static_awbex:HighStop");/*HighStop*/
    s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
    SCENE_INIPARAM_CHECK_LOAD_RESULT(s32Ret, aszIniNodeName);
    if (HI_NULL != pszString)
    {
        s32Ret = HI_CONFACCESS_GetInt(SCENE_INIPARAM, pszIniModule, "static_awbex:HighStop", 0, &s32Value);
        if (HI_SUCCESS != s32Ret)
        {
            MLOGE("load static_awbex:HighStop failed\n");
            return HI_FAILURE;
        }
        pstStaticAwbEx->u16HighStop = (HI_U16)s32Value;

        free(pszString);
        pszString = HI_NULL;
    }

    snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "static_awbex:MultiLightSourceEn");/*MultiLightSourceEn*/
    s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
    SCENE_INIPARAM_CHECK_LOAD_RESULT(s32Ret, aszIniNodeName);
    if (HI_NULL != pszString)
    {
        s32Ret = HI_CONFACCESS_GetInt(SCENE_INIPARAM, pszIniModule, "static_awbex:MultiLightSourceEn", 0, &s32Value);
        if (HI_SUCCESS != s32Ret)
        {
            MLOGE("load static_awbex:MultiLightSourceEn failed\n");
            return HI_FAILURE;
        }
        pstStaticAwbEx->bMultiLightSourceEn = s32Value;

        free(pszString);
        pszString = HI_NULL;
    }

    snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "static_awbex:MultiCTWt");/*MultiCTWt*/
    s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
    SCENE_INIPARAM_CHECK_LOAD_RESULT(s32Ret, aszIniNodeName);
    if (HI_NULL != pszString)
    {
        SCENE_GetNumbersInOneLine(pszString);
        for (u32IdxM = 0; u32IdxM < 8; u32IdxM++)
        {
            pstStaticAwbEx->au16MultiCTWt[u32IdxM] = s_as64LineNum[u32IdxM];
        }

        free(pszString);
        pszString = HI_NULL;
    }

    return HI_SUCCESS;
}

HI_S32 SCENE_LoadStaticGlobalCac(const HI_CHAR *pszIniModule, HI_SCENE_STATIC_GLOBALCAC_S *pstStaticGlobalCac)
{
    HI_S32 s32Ret = HI_SUCCESS;
    HI_CHAR *pszString = NULL;
    HI_S32 s32Value = 0;
    HI_CHAR aszIniNodeName[SCENE_INIPARAM_NODE_NAME_LEN] = {0,};

    snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "static_globalcac:GlobalCacEnable");/*GlobalCacEnable*/
    s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
    SCENE_INIPARAM_CHECK_LOAD_RESULT(s32Ret, aszIniNodeName);
    if (HI_NULL != pszString)
    {
        free(pszString);
        pszString = HI_NULL;
        s32Ret = HI_CONFACCESS_GetInt(SCENE_INIPARAM, pszIniModule, "static_globalcac:GlobalCacEnable", 0, &s32Value);
        if (HI_SUCCESS != s32Ret)
        {
            MLOGE("load static_globalcac:GlobalCacEnable failed\n");
            return HI_FAILURE;
        }
        pstStaticGlobalCac->bEnable = (HI_BOOL)s32Value;
    }

    snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "static_globalcac:VerCoordinate");/*VerCoordinate*/
    s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
    SCENE_INIPARAM_CHECK_LOAD_RESULT(s32Ret, aszIniNodeName);
    if (HI_NULL != pszString)
    {
        free(pszString);
        pszString = HI_NULL;
        s32Ret = HI_CONFACCESS_GetInt(SCENE_INIPARAM, pszIniModule, "static_globalcac:VerCoordinate", 0, &s32Value);
        if (HI_SUCCESS != s32Ret)
        {
            MLOGE("load static_globalcac:VerCoordinate failed\n");
            return HI_FAILURE;
        }
        pstStaticGlobalCac->u16VerCoordinate = (HI_U16)s32Value;
    }

    snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "static_globalcac:HorCoordinate");/*HorCoordinate*/
    s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
    SCENE_INIPARAM_CHECK_LOAD_RESULT(s32Ret, aszIniNodeName);
    if (HI_NULL != pszString)
    {
        free(pszString);
        pszString = HI_NULL;
        s32Ret = HI_CONFACCESS_GetInt(SCENE_INIPARAM, pszIniModule, "static_globalcac:HorCoordinate", 0, &s32Value);
        if (HI_SUCCESS != s32Ret)
        {
            MLOGE("load static_globalcac:HorCoordinate failed\n");
            return HI_FAILURE;
        }
        pstStaticGlobalCac->u16HorCoordinate = (HI_U16)s32Value;
    }

    snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "static_globalcac:ParamRedA");/*ParamRedA*/
    s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
    SCENE_INIPARAM_CHECK_LOAD_RESULT(s32Ret, aszIniNodeName);
    if (HI_NULL != pszString)
    {
        free(pszString);
        pszString = HI_NULL;
        s32Ret = HI_CONFACCESS_GetInt(SCENE_INIPARAM, pszIniModule, "static_globalcac:ParamRedA", 0, &s32Value);
        if (HI_SUCCESS != s32Ret)
        {
            MLOGE("load static_globalcac:ParamRedA failed\n");
            return HI_FAILURE;
        }
        pstStaticGlobalCac->s16ParamRedA = (HI_S16)s32Value;
    }

    snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "static_globalcac:ParamRedB");/*ParamRedB*/
    s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
    SCENE_INIPARAM_CHECK_LOAD_RESULT(s32Ret, aszIniNodeName);
    if (HI_NULL != pszString)
    {
        free(pszString);
        pszString = HI_NULL;
        s32Ret = HI_CONFACCESS_GetInt(SCENE_INIPARAM, pszIniModule, "static_globalcac:ParamRedB", 0, &s32Value);
        if (HI_SUCCESS != s32Ret)
        {
            MLOGE("load static_globalcac:ParamRedB failed\n");
            return HI_FAILURE;
        }
        pstStaticGlobalCac->s16ParamRedB = (HI_S16)s32Value;
    }

    snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "static_globalcac:ParamRedC");/*ParamRedC*/
    s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
    SCENE_INIPARAM_CHECK_LOAD_RESULT(s32Ret, aszIniNodeName);
    if (HI_NULL != pszString)
    {
        free(pszString);
        pszString = HI_NULL;
        s32Ret = HI_CONFACCESS_GetInt(SCENE_INIPARAM, pszIniModule, "static_globalcac:ParamRedC", 0, &s32Value);
        if (HI_SUCCESS != s32Ret)
        {
            MLOGE("load static_globalcac:ParamRedC failed\n");
            return HI_FAILURE;
        }
        pstStaticGlobalCac->s16ParamRedC = (HI_S16)s32Value;
    }

    snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "static_globalcac:ParamBlueA");/*ParamBlueA*/
    s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
    SCENE_INIPARAM_CHECK_LOAD_RESULT(s32Ret, aszIniNodeName);
    if (HI_NULL != pszString)
    {
        free(pszString);
        pszString = HI_NULL;
        s32Ret = HI_CONFACCESS_GetInt(SCENE_INIPARAM, pszIniModule, "static_globalcac:ParamBlueA", 0, &s32Value);
        if (HI_SUCCESS != s32Ret)
        {
            MLOGE("load static_globalcac:ParamBlueA failed\n");
            return HI_FAILURE;
        }
        pstStaticGlobalCac->s16ParamBlueA = (HI_S16)s32Value;
    }

    snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "static_globalcac:ParamBlueB");/*ParamBlueB*/
    s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
    SCENE_INIPARAM_CHECK_LOAD_RESULT(s32Ret, aszIniNodeName);
    if (HI_NULL != pszString)
    {
        free(pszString);
        pszString = HI_NULL;
        s32Ret = HI_CONFACCESS_GetInt(SCENE_INIPARAM, pszIniModule, "static_globalcac:ParamBlueB", 0, &s32Value);
        if (HI_SUCCESS != s32Ret)
        {
            MLOGE("load static_globalcac:ParamBlueB failed\n");
            return HI_FAILURE;
        }
        pstStaticGlobalCac->s16ParamBlueB = (HI_S16)s32Value;
    }

    snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "static_globalcac:ParamBlueC");/*ParamBlueC*/
    s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
    SCENE_INIPARAM_CHECK_LOAD_RESULT(s32Ret, aszIniNodeName);
    if (HI_NULL != pszString)
    {
        free(pszString);
        pszString = HI_NULL;
        s32Ret = HI_CONFACCESS_GetInt(SCENE_INIPARAM, pszIniModule, "static_globalcac:ParamBlueC", 0, &s32Value);
        if (HI_SUCCESS != s32Ret)
        {
            MLOGE("load static_globalcac:ParamBlueC failed\n");
            return HI_FAILURE;
        }
        pstStaticGlobalCac->s16ParamBlueC = (HI_S16)s32Value;
    }

    snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "static_globalcac:VerNormShift");/*VerNormShift*/
    s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
    SCENE_INIPARAM_CHECK_LOAD_RESULT(s32Ret, aszIniNodeName);
    if (HI_NULL != pszString)
    {
        free(pszString);
        pszString = HI_NULL;
        s32Ret = HI_CONFACCESS_GetInt(SCENE_INIPARAM, pszIniModule, "static_globalcac:VerNormShift", 0, &s32Value);
        if (HI_SUCCESS != s32Ret)
        {
            MLOGE("load static_globalcac:VerNormShift failed\n");
            return HI_FAILURE;
        }
        pstStaticGlobalCac->u8VerNormShift = (HI_U8)s32Value;
    }

    snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "static_globalcac:VerNormFactor");/*VerNormFactor*/
    s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
    SCENE_INIPARAM_CHECK_LOAD_RESULT(s32Ret, aszIniNodeName);
    if (HI_NULL != pszString)
    {
        free(pszString);
        pszString = HI_NULL;
        s32Ret = HI_CONFACCESS_GetInt(SCENE_INIPARAM, pszIniModule, "static_globalcac:VerNormFactor", 0, &s32Value);
        if (HI_SUCCESS != s32Ret)
        {
            MLOGE("load static_globalcac:ParamRedC failed\n");
            return HI_FAILURE;
        }
        pstStaticGlobalCac->u8VerNormFactor = (HI_U8)s32Value;
    }

    snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "static_globalcac:HorNormShift");/*HorNormShift*/
    s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
    SCENE_INIPARAM_CHECK_LOAD_RESULT(s32Ret, aszIniNodeName);
    if (HI_NULL != pszString)
    {
        free(pszString);
        pszString = HI_NULL;
        s32Ret = HI_CONFACCESS_GetInt(SCENE_INIPARAM, pszIniModule, "static_globalcac:HorNormShift", 0, &s32Value);
        if (HI_SUCCESS != s32Ret)
        {
            MLOGE("load static_globalcac:HorNormShift failed\n");
            return HI_FAILURE;
        }
        pstStaticGlobalCac->u8HorNormShift = (HI_U8)s32Value;
    }

    snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "static_globalcac:HorNormFactor");/*HorNormFactor*/
    s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
    SCENE_INIPARAM_CHECK_LOAD_RESULT(s32Ret, aszIniNodeName);
    if (HI_NULL != pszString)
    {
        free(pszString);
        pszString = HI_NULL;
        s32Ret = HI_CONFACCESS_GetInt(SCENE_INIPARAM, pszIniModule, "static_globalcac:HorNormFactor", 0, &s32Value);
        if (HI_SUCCESS != s32Ret)
        {
            MLOGE("load static_globalcac:HorNormFactor failed\n");
            return HI_FAILURE;
        }
        pstStaticGlobalCac->u8HorNormFactor = (HI_U8)s32Value;
    }

    snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "static_globalcac:CorVarThr");/*CorVarThr*/
    s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
    SCENE_INIPARAM_CHECK_LOAD_RESULT(s32Ret, aszIniNodeName);
    if (HI_NULL != pszString)
    {
        free(pszString);
        pszString = HI_NULL;
        s32Ret = HI_CONFACCESS_GetInt(SCENE_INIPARAM, pszIniModule, "static_globalcac:CorVarThr", 0, &s32Value);
        if (HI_SUCCESS != s32Ret)
        {
            MLOGE("load static_globalcac:CorVarThr failed\n");
            return HI_FAILURE;
        }
        pstStaticGlobalCac->u16CorVarThr = (HI_U16)s32Value;
    }

    return HI_SUCCESS;
}

HI_S32 SCENE_LoadStaticLocalCac(const HI_CHAR *pszIniModule, HI_SCENE_STATIC_LOCALCAC_S *pstStaticLocalCac)
{
    HI_S32 s32Ret = HI_SUCCESS;
    HI_CHAR *pszString = NULL;
    HI_S32 s32Value = 0;
    HI_CHAR aszIniNodeName[SCENE_INIPARAM_NODE_NAME_LEN] = {0,};

    snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "static_localcac:LocalCacEnable");/*LocalCacEnable*/
    s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
    SCENE_INIPARAM_CHECK_LOAD_RESULT(s32Ret, aszIniNodeName);
    if (HI_NULL != pszString)
    {
        free(pszString);
        pszString = HI_NULL;
        s32Ret = HI_CONFACCESS_GetInt(SCENE_INIPARAM, pszIniModule, "static_localcac:LocalCacEnable", 0, &s32Value);
        if (HI_SUCCESS != s32Ret)
        {
            MLOGE("load static_localcac:LocalCacEnable failed\n");
            return HI_FAILURE;
        }
        pstStaticLocalCac->bEnable = (HI_BOOL)s32Value;
    }

    snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "static_localcac:PurpleDetRange");/*PurpleDetRange*/
    s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
    SCENE_INIPARAM_CHECK_LOAD_RESULT(s32Ret, aszIniNodeName);
    if (HI_NULL != pszString)
    {
        free(pszString);
        pszString = HI_NULL;
        s32Ret = HI_CONFACCESS_GetInt(SCENE_INIPARAM, pszIniModule, "static_localcac:PurpleDetRange", 0, &s32Value);
        if (HI_SUCCESS != s32Ret)
        {
            MLOGE("load static_localcac:u16PurpleDetRange failed\n");
            return HI_FAILURE;
        }
        pstStaticLocalCac->u16PurpleDetRange = (HI_U16)s32Value;
    }

    snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "static_localcac:VarThr");/*VarThr*/
    s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
    SCENE_INIPARAM_CHECK_LOAD_RESULT(s32Ret, aszIniNodeName);
    if (HI_NULL != pszString)
    {
        free(pszString);
        pszString = HI_NULL;
        s32Ret = HI_CONFACCESS_GetInt(SCENE_INIPARAM, pszIniModule, "static_localcac:VarThr", 0, &s32Value);
        if (HI_SUCCESS != s32Ret)
        {
            MLOGE("load static_localcac:HorCoordinate failed\n");
            return HI_FAILURE;
        }
        pstStaticLocalCac->u16VarThr = (HI_U16)s32Value;
    }

    return HI_SUCCESS;
}


HI_S32 SCENE_LoadStaticWdrExposure(const HI_CHAR *pszIniModule, HI_SCENE_STATIC_WDREXPOSURE_S *pstStaticWdrExposure)
{
    HI_S32 s32Ret = HI_SUCCESS;
    HI_CHAR *pszString = NULL;
    HI_U32 u32IdxM = 0;
    HI_S32 s32Value = 0;
    HI_CHAR aszIniNodeName[SCENE_INIPARAM_NODE_NAME_LEN] = {0,};

    snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "static_wdrexposure:ExpRatioType");/*ExpRatioType*/
    s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
    SCENE_INIPARAM_CHECK_LOAD_RESULT(s32Ret, aszIniNodeName);
    if (HI_NULL != pszString)
    {
        free(pszString);
        pszString = HI_NULL;
        s32Ret = HI_CONFACCESS_GetInt(SCENE_INIPARAM, pszIniModule, "static_wdrexposure:ExpRatioType", 0, &s32Value);
        if (HI_SUCCESS != s32Ret)
        {
            MLOGE("load static_wdrexposure:ExpRatioType failed\n");
            return HI_FAILURE;
        }
        pstStaticWdrExposure->u8ExpRatioType = (HI_U8)s32Value;
    }

    snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "static_wdrexposure:ExpRatioMax");/*ExpRatioMax*/
    s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
    SCENE_INIPARAM_CHECK_LOAD_RESULT(s32Ret, aszIniNodeName);
    if (HI_NULL != pszString)
    {
        free(pszString);
        pszString = HI_NULL;
        s32Ret = HI_CONFACCESS_GetInt(SCENE_INIPARAM, pszIniModule, "static_wdrexposure:ExpRatioMax", 0, &s32Value);
        if (HI_SUCCESS != s32Ret)
        {
            MLOGE("load static_wdrexposure:ExpRatioMax failed\n");
            return HI_FAILURE;
        }
        pstStaticWdrExposure->u32ExpRatioMax = (HI_U32)s32Value;
    }

    snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "static_wdrexposure:ExpRatioMin");/*ExpRatioMin*/
    s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
    SCENE_INIPARAM_CHECK_LOAD_RESULT(s32Ret, aszIniNodeName);
    if (HI_NULL != pszString)
    {
        free(pszString);
        pszString = HI_NULL;
        s32Ret = HI_CONFACCESS_GetInt(SCENE_INIPARAM, pszIniModule, "static_wdrexposure:ExpRatioMin", 0, &s32Value);
        if (HI_SUCCESS != s32Ret)
        {
            MLOGE("load static_wdrexposure:ExpRatioMin failed\n");
            return HI_FAILURE;
        }
        pstStaticWdrExposure->u32ExpRatioMin = (HI_U32)s32Value;
    }

    snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "static_wdrexposure:ExpRatio");/*ExpRatio*/
    s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
    SCENE_INIPARAM_CHECK_LOAD_RESULT(s32Ret, aszIniNodeName);
    if (HI_NULL != pszString)
    {
        SCENE_GetNumbersInOneLine(pszString);
        for (u32IdxM = 0; u32IdxM < EXP_RATIO_NUM; u32IdxM++)
        {
            pstStaticWdrExposure->au32ExpRatio[u32IdxM] = s_as64LineNum[u32IdxM];
        }
        free(pszString);
        pszString = HI_NULL;
    }

    snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "static_wdrexposure:RefRatioUp");/*u32RefRatioUp*/
    s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
    SCENE_INIPARAM_CHECK_LOAD_RESULT(s32Ret, aszIniNodeName);
    if (HI_NULL != pszString)
    {
        free(pszString);
        pszString = HI_NULL;
        s32Ret = HI_CONFACCESS_GetInt(SCENE_INIPARAM, pszIniModule, "static_wdrexposure:RefRatioUp", 0, &s32Value);
        if (HI_SUCCESS != s32Ret)
        {
            MLOGE("load static_wdrexposure:ExpRatioMin failed\n");
            return HI_FAILURE;
        }
        pstStaticWdrExposure->u32RefRatioUp = (HI_U32)s32Value;
    }

    snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "static_wdrexposure:RefRatioDn");/*u32RefRatioDn*/
    s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
    SCENE_INIPARAM_CHECK_LOAD_RESULT(s32Ret, aszIniNodeName);
    if (HI_NULL != pszString)
    {
        free(pszString);
        pszString = HI_NULL;
        s32Ret = HI_CONFACCESS_GetInt(SCENE_INIPARAM, pszIniModule, "static_wdrexposure:RefRatioDn", 0, &s32Value);
        if (HI_SUCCESS != s32Ret)
        {
            MLOGE("load static_wdrexposure:RefRatioDn failed\n");
            return HI_FAILURE;
        }
        pstStaticWdrExposure->u32RefRatioDn = (HI_U32)s32Value;
    }

    snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "static_wdrexposure:ExpTh");/*u32ExpTh*/
    s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
    SCENE_INIPARAM_CHECK_LOAD_RESULT(s32Ret, aszIniNodeName);
    if (HI_NULL != pszString)
    {
        free(pszString);
        pszString = HI_NULL;
        s32Ret = HI_CONFACCESS_GetInt(SCENE_INIPARAM, pszIniModule, "static_wdrexposure:ExpTh", 0, &s32Value);
        if (HI_SUCCESS != s32Ret)
        {
            MLOGE("load static_wdrexposure:ExpTh failed\n");
            return HI_FAILURE;
        }
        pstStaticWdrExposure->u32ExpTh = (HI_U32)s32Value;
    }

    return HI_SUCCESS;
}

HI_S32 SCENE_LoadDynamicFsWdr(const HI_CHAR *pszIniModule, HI_SCENE_DYNAMIC_FSWDR_S *pstDynamicFsWdr)
{
    HI_S32 s32Ret = HI_SUCCESS;
    HI_CHAR *pszString = NULL;
    HI_S32 s32Value = 0;
    HI_U32 u32IdxM = 0;
    HI_CHAR aszIniNodeName[SCENE_INIPARAM_NODE_NAME_LEN] = {0,};

    snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "dynamic_fswdr:IsoCnt");/*IsoCnt*/
    s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
    SCENE_INIPARAM_CHECK_LOAD_RESULT(s32Ret, aszIniNodeName);
    if (HI_NULL != pszString)
    {
        free(pszString);
        pszString = HI_NULL;
        s32Ret = HI_CONFACCESS_GetInt(SCENE_INIPARAM, pszIniModule, "dynamic_fswdr:IsoCnt", 0, &s32Value);
        if (HI_SUCCESS != s32Ret)
        {
            MLOGE("load dynamic_fswdr:IsoCnt failed\n");
            return HI_FAILURE;
        }
        pstDynamicFsWdr->u32IsoCnt = (HI_U32)s32Value;
    }

    snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "dynamic_fswdr:ExpRation");/*ExpRation*/
    s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
    SCENE_INIPARAM_CHECK_LOAD_RESULT(s32Ret, aszIniNodeName);
    if (HI_NULL != pszString)
    {
        SCENE_GetNumbersInOneLine(pszString);
        for (u32IdxM = 0; u32IdxM < pstDynamicFsWdr->u32IsoCnt; u32IdxM++)
        {
            pstDynamicFsWdr->au32ExpRation[u32IdxM] = s_as64LineNum[u32IdxM];
        }
        free(pszString);
        pszString = HI_NULL;
    }

    snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "dynamic_fswdr:MotionComp");/*MotionComp*/
    s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
    SCENE_INIPARAM_CHECK_LOAD_RESULT(s32Ret, aszIniNodeName);
    if (HI_NULL != pszString)
    {
        SCENE_GetNumbersInOneLine(pszString);
        for (u32IdxM = 0; u32IdxM < pstDynamicFsWdr->u32IsoCnt; u32IdxM++)
        {
            pstDynamicFsWdr->au8MotionComp[u32IdxM] = s_as64LineNum[u32IdxM];
        }
        free(pszString);
        pszString = HI_NULL;
    }

    snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "dynamic_fswdr:ISOLtoHThresh");/*ISOLtoHThresh*/
    s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
    SCENE_INIPARAM_CHECK_LOAD_RESULT(s32Ret, aszIniNodeName);
    if (HI_NULL != pszString)
    {
        SCENE_GetNumbersInOneLine(pszString);
        for (u32IdxM = 0; u32IdxM < pstDynamicFsWdr->u32IsoCnt; u32IdxM++)
        {
            pstDynamicFsWdr->au32ISOLtoHThresh[u32IdxM] = s_as64LineNum[u32IdxM];
        }
        free(pszString);
        pszString = HI_NULL;
    }

    snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "dynamic_fswdr:BnrStr");/*BnrStr*/
    s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
    SCENE_INIPARAM_CHECK_LOAD_RESULT(s32Ret, aszIniNodeName);
    if (HI_NULL != pszString)
    {
        SCENE_GetNumbersInOneLine(pszString);
        for (u32IdxM = 0; u32IdxM < pstDynamicFsWdr->u32IsoCnt; u32IdxM++)
        {
            pstDynamicFsWdr->au8BnrStr[u32IdxM] = s_as64LineNum[u32IdxM];
        }
        free(pszString);
        pszString = HI_NULL;
    }

    snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "dynamic_fswdr:BnrMode");/*BnrMode*/
    s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
    SCENE_INIPARAM_CHECK_LOAD_RESULT(s32Ret, aszIniNodeName);
    if (HI_NULL != pszString)
    {
        SCENE_GetNumbersInOneLine(pszString);
        for (u32IdxM = 0; u32IdxM < pstDynamicFsWdr->u32IsoCnt; u32IdxM++)
        {
            pstDynamicFsWdr->au8BnrMode[u32IdxM] = s_as64LineNum[u32IdxM];
        }
        free(pszString);
        pszString = HI_NULL;
    }
    return HI_SUCCESS;
}

HI_S32 SCENE_LoadDynamicWdrExposure(const HI_CHAR *pszIniModule, HI_SCENE_DYNAMIC_WDREXPOSURE_S *pstDynamicWdrExposures)
{
	HI_S32 s32Ret = HI_SUCCESS;
	HI_CHAR *pszString = NULL;
	HI_S32 s32Value = 0;
	HI_U32 u32IdxM = 0;
	HI_CHAR aszIniNodeName[SCENE_INIPARAM_NODE_NAME_LEN] = {0,};

	snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "dynamic_wdrexposure:aeExpCount");/*aeExpCount*/
	s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
	SCENE_INIPARAM_CHECK_LOAD_RESULT(s32Ret, aszIniNodeName);
	if (HI_NULL != pszString)
	{
		free(pszString);
		pszString = HI_NULL;
		s32Ret = HI_CONFACCESS_GetInt(SCENE_INIPARAM, pszIniModule, "dynamic_wdrexposure:aeExpCount", 0, &s32Value);
		if (HI_SUCCESS != s32Ret)
		{
			MLOGE("load dynamic_wdrexposure:aeExpCount failed\n");
			return HI_FAILURE;
		}
		pstDynamicWdrExposures->u8AEExposureCnt = (HI_U8)s32Value;
	}

	snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "dynamic_wdrexposure:aeExpLtoHThresh");/*aeExpLtoHThresh*/
	s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
	SCENE_INIPARAM_CHECK_LOAD_RESULT(s32Ret, aszIniNodeName);
	if (HI_NULL != pszString)
	{
		SCENE_GetNumbersInOneLine(pszString);
		for (u32IdxM = 0; u32IdxM < pstDynamicWdrExposures->u8AEExposureCnt; u32IdxM++)
		{
			pstDynamicWdrExposures->au64ExpLtoHThresh[u32IdxM] = s_as64LineNum[u32IdxM];
		}
		free(pszString);
		pszString = HI_NULL;
	}

	snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "dynamic_wdrexposure:aeExpHtoLThresh");/*aeExpHtoLThresh*/
	s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
	SCENE_INIPARAM_CHECK_LOAD_RESULT(s32Ret, aszIniNodeName);
	if (HI_NULL != pszString)
	{
		SCENE_GetNumbersInOneLine(pszString);
		for (u32IdxM = 0; u32IdxM < pstDynamicWdrExposures->u8AEExposureCnt; u32IdxM++)
		{
			pstDynamicWdrExposures->au64ExpHtoLThresh[u32IdxM] = s_as64LineNum[u32IdxM];
		}
		free(pszString);
		pszString = HI_NULL;
	}

	snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "dynamic_wdrexposure:ExpCompensation");/*ExpCompensation*/
	s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
	SCENE_INIPARAM_CHECK_LOAD_RESULT(s32Ret, aszIniNodeName);
	if (HI_NULL != pszString)
	{
		SCENE_GetNumbersInOneLine(pszString);
		for (u32IdxM = 0; u32IdxM < pstDynamicWdrExposures->u8AEExposureCnt; u32IdxM++)
		{
			pstDynamicWdrExposures->ExpCompensation[u32IdxM] = s_as64LineNum[u32IdxM];
		}
		free(pszString);
		pszString = HI_NULL;
	}

	snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "dynamic_wdrexposure:MaxHistOffset");/*MaxHistOffset*/
	s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
	SCENE_INIPARAM_CHECK_LOAD_RESULT(s32Ret, aszIniNodeName);
	if (HI_NULL != pszString)
	{
		SCENE_GetNumbersInOneLine(pszString);
		for (u32IdxM = 0; u32IdxM < pstDynamicWdrExposures->u8AEExposureCnt; u32IdxM++)
		{
			pstDynamicWdrExposures->MaxHistOffset[u32IdxM] = s_as64LineNum[u32IdxM];
		}
		free(pszString);
		pszString = HI_NULL;
	}

	return HI_SUCCESS;
}

HI_S32 SCENE_LoadStaticSaturation(const HI_CHAR *pszIniModule, HI_SCENE_STATIC_SATURATION_S *pstStaticSaturation)
{
    HI_S32 s32Ret = HI_SUCCESS;
    HI_CHAR *pszString = NULL;
    HI_U32 u32IdxM = 0;
    HI_CHAR aszIniNodeName[SCENE_INIPARAM_NODE_NAME_LEN] = {0,};
    snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "static_saturation:AutoSat");/*AutoSat*/
    s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
    SCENE_INIPARAM_CHECK_LOAD_RESULT(s32Ret, aszIniNodeName);
    if (HI_NULL != pszString)
    {
        SCENE_GetNumbersInOneLine(pszString);
        for (u32IdxM = 0; u32IdxM < ISP_AUTO_ISO_STRENGTH_NUM; u32IdxM++)
        {
            pstStaticSaturation->au8AutoSat[u32IdxM] = s_as64LineNum[u32IdxM];
        }
        free(pszString);
        pszString = HI_NULL;
    }

    return HI_SUCCESS;
}

HI_S32 SCENE_LoadStaticCCM(const HI_CHAR *pszIniModule, HI_SCENE_STATIC_CCM_S* pstStaticCCM)
{
    HI_S32 s32Ret = HI_SUCCESS;
    HI_CHAR *pszString = NULL;
    HI_S32 s32Value = 0;
    HI_U32 u32IdxM = 0;
    HI_U32 u32IdxN = 0;
    HI_CHAR aszIniNodeName[SCENE_INIPARAM_NODE_NAME_LEN] = {0,};

    snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "static_ccm:TotalNum");/*TotalNum*/
    s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
    SCENE_INIPARAM_CHECK_LOAD_RESULT(s32Ret, aszIniNodeName);
    if (HI_NULL != pszString)
    {
        free(pszString);
        pszString = HI_NULL;
        s32Ret = HI_CONFACCESS_GetInt(SCENE_INIPARAM, pszIniModule, "static_ccm:TotalNum", 0, &s32Value);
        if (HI_SUCCESS != s32Ret)
        {
            MLOGE("load static_ccm:TotalNum failed\n");
            return HI_FAILURE;
        }

        pstStaticCCM->u32TotalNum = (HI_U32)s32Value;
    }


    snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "static_ccm:AutoColorTemp");/*AutoColorTemp*/
    s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
    SCENE_INIPARAM_CHECK_LOAD_RESULT(s32Ret, aszIniNodeName);
    if (HI_NULL != pszString)
    {
        SCENE_GetNumbersInOneLine(pszString);
        for (u32IdxM = 0; u32IdxM < pstStaticCCM->u32TotalNum; u32IdxM++)
        {
            pstStaticCCM->au16AutoColorTemp[u32IdxM] = s_as64LineNum[u32IdxM];
        }
        free(pszString);
        pszString = HI_NULL;
    }

    snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "static_ccm:AutoCCMTable_0");/*AutoCCMTable_0*/
    s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
    SCENE_INIPARAM_CHECK_LOAD_RESULT(s32Ret, aszIniNodeName);
    if (HI_NULL != pszString)
    {
        free(pszString);
        pszString = HI_NULL;
        for (u32IdxM = 0; u32IdxM < pstStaticCCM->u32TotalNum; u32IdxM++)
        {
            snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "static_ccm:AutoCCMTable_%d", u32IdxM);
            s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
            if (HI_SUCCESS != s32Ret)
            {
                MLOGE("load %s failed\n", aszIniNodeName);
                return HI_FAILURE;
            }
            if (NULL != pszString)
            {
                SCENE_GetNumbersInOneLine(pszString);
                for (u32IdxN = 0; u32IdxN < CCM_MATRIX_SIZE; u32IdxN++)
                {
                    pstStaticCCM->au16AutoCCM[u32IdxM][u32IdxN] = (HI_U16)s_as64LineNum[u32IdxN];
                }
                free(pszString);
                pszString = HI_NULL;
            }
        }
    }

    return HI_SUCCESS;
}


HI_S32 SCENE_LoadStaticLDCI(const HI_CHAR *pszIniModule, HI_SCENE_STATIC_LDCI_S *pstStaticLdci)
{
    HI_S32 s32Ret = HI_SUCCESS;
    HI_CHAR *pszString = NULL;
    HI_S32 s32Value = 0;
    HI_U32 u32IdxM = 0;
    HI_CHAR aszIniNodeName[SCENE_INIPARAM_NODE_NAME_LEN] = {0,};
    snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "static_ldci:Enable");/*Enable*/
    s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
    SCENE_INIPARAM_CHECK_LOAD_RESULT(s32Ret, aszIniNodeName);
    if (HI_NULL != pszString)
    {
        free(pszString);
        pszString = HI_NULL;
        s32Ret = HI_CONFACCESS_GetInt(SCENE_INIPARAM, pszIniModule, "static_ldci:Enable", 0, &s32Value);
        if (HI_SUCCESS != s32Ret)
        {
            MLOGE("load static_ldci:bEnable failed\n");
            return HI_FAILURE;
        }
        pstStaticLdci->bEnable = (HI_BOOL)s32Value;
    }

    snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "static_ldci:LDCIOpType");/*LDCIOpType*/
    s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
    SCENE_INIPARAM_CHECK_LOAD_RESULT(s32Ret, aszIniNodeName);
    if (HI_NULL != pszString)
    {
        free(pszString);
        pszString = HI_NULL;
        s32Ret = HI_CONFACCESS_GetInt(SCENE_INIPARAM, pszIniModule, "static_ldci:LDCIOpType", 0, &s32Value);
        if (HI_SUCCESS != s32Ret)
        {
            MLOGE("load static_ldci:LDCIOpType failed\n");
            return HI_FAILURE;
        }
        pstStaticLdci->u8LDCIOpType = (HI_U8)s32Value;
    }

    snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "static_ldci:LDCIGaussLPFSigma");/*LDCIGaussLPFSigma*/
    s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
    SCENE_INIPARAM_CHECK_LOAD_RESULT(s32Ret, aszIniNodeName);
    if (HI_NULL != pszString)
    {
        free(pszString);
        pszString = HI_NULL;
        s32Ret = HI_CONFACCESS_GetInt(SCENE_INIPARAM, pszIniModule, "static_ldci:LDCIGaussLPFSigma", 0, &s32Value);
        if (HI_SUCCESS != s32Ret)
        {
            MLOGE("load static_ldci:LDCIGaussLPFSigma failed\n");
            return HI_FAILURE;
        }
        pstStaticLdci->u8GaussLPFSigma = (HI_U8)s32Value;
    }

        snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "static_ldci:AutoHePosWgt");/*AutoHePosWgt*/
        s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
        SCENE_INIPARAM_CHECK_LOAD_RESULT(s32Ret, aszIniNodeName);
        if (HI_NULL != pszString)
        {
            SCENE_GetNumbersInOneLine(pszString);
            for (u32IdxM = 0; u32IdxM < ISP_AUTO_ISO_STRENGTH_NUM; u32IdxM++)
            {
                pstStaticLdci->au8AutoHePosWgt[u32IdxM] = s_as64LineNum[u32IdxM];
            }
            free(pszString);
            pszString = HI_NULL;
        }

        snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "static_ldci:AutoHePosSigma");/*AutoHePosSigma*/
        s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
        SCENE_INIPARAM_CHECK_LOAD_RESULT(s32Ret, aszIniNodeName);
        if (HI_NULL != pszString)
        {
            SCENE_GetNumbersInOneLine(pszString);
            for (u32IdxM = 0; u32IdxM < ISP_AUTO_ISO_STRENGTH_NUM; u32IdxM++)
            {
                pstStaticLdci->au8AutoHePosSigma[u32IdxM] = s_as64LineNum[u32IdxM];
            }
            free(pszString);
            pszString = HI_NULL;
        }

        snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "static_ldci:AutoHePosMean");/*AutoHePosMean*/
        s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
        SCENE_INIPARAM_CHECK_LOAD_RESULT(s32Ret, aszIniNodeName);
        if (HI_NULL != pszString)
        {
            SCENE_GetNumbersInOneLine(pszString);
            for (u32IdxM = 0; u32IdxM < ISP_AUTO_ISO_STRENGTH_NUM; u32IdxM++)
            {
                pstStaticLdci->au8AutoHePosMean[u32IdxM] = s_as64LineNum[u32IdxM];
            }
            free(pszString);
            pszString = HI_NULL;
        }

        snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "static_ldci:AutoHeNegWgt");/*AutoHeNegWgt*/
        s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
        SCENE_INIPARAM_CHECK_LOAD_RESULT(s32Ret, aszIniNodeName);
        if (HI_NULL != pszString)
        {
            SCENE_GetNumbersInOneLine(pszString);
            for (u32IdxM = 0; u32IdxM < ISP_AUTO_ISO_STRENGTH_NUM; u32IdxM++)
            {
                pstStaticLdci->au8AutoHeNegWgt[u32IdxM] = s_as64LineNum[u32IdxM];
            }
            free(pszString);
            pszString = HI_NULL;
        }

        snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "static_ldci:AutoHeNegSigma");/*AutoHeNegSigma*/
        s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
        SCENE_INIPARAM_CHECK_LOAD_RESULT(s32Ret, aszIniNodeName);
        if (HI_NULL != pszString)
        {
            SCENE_GetNumbersInOneLine(pszString);
            for (u32IdxM = 0; u32IdxM < ISP_AUTO_ISO_STRENGTH_NUM; u32IdxM++)
            {
                pstStaticLdci->au8AutoHeNegSigma[u32IdxM] = s_as64LineNum[u32IdxM];
            }
            free(pszString);
            pszString = HI_NULL;
        }

        snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "static_ldci:AutoHeNegMean");/*AutoHeNegMean*/
        s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
        SCENE_INIPARAM_CHECK_LOAD_RESULT(s32Ret, aszIniNodeName);
        if (HI_NULL != pszString)
        {
            SCENE_GetNumbersInOneLine(pszString);
            for (u32IdxM = 0; u32IdxM < ISP_AUTO_ISO_STRENGTH_NUM; u32IdxM++)
            {
                pstStaticLdci->au8AutoHeNegMean[u32IdxM] = s_as64LineNum[u32IdxM];
            }
            free(pszString);
            pszString = HI_NULL;
        }

        snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "static_ldci:AutoBlcCtrl");/*AutoBlcCtrl*/
        s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
        SCENE_INIPARAM_CHECK_LOAD_RESULT(s32Ret, aszIniNodeName);
        if (HI_NULL != pszString)
        {
            SCENE_GetNumbersInOneLine(pszString);
            for (u32IdxM = 0; u32IdxM < ISP_AUTO_ISO_STRENGTH_NUM; u32IdxM++)
            {
                pstStaticLdci->au16AutoBlcCtrl[u32IdxM] = s_as64LineNum[u32IdxM];
            }
            free(pszString);
            pszString = HI_NULL;
        }

    return HI_SUCCESS;
}

HI_S32 SCENE_LoadDynamicLDCI(const HI_CHAR *pszIniModule, HI_SCENE_DYNAMIC_LDCI_S *pstDynamicLdci)
{
    HI_S32 s32Ret = HI_SUCCESS;
    HI_CHAR *pszString = NULL;
    HI_S32 s32Value = 0;
    HI_U32 u32IdxM = 0;
    HI_CHAR aszIniNodeName[SCENE_INIPARAM_NODE_NAME_LEN] = {0,};

    snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "dynamic_ldci:EnableCount");/*EnableCount*/
    s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
    SCENE_INIPARAM_CHECK_LOAD_RESULT(s32Ret, aszIniNodeName);
    if (HI_NULL != pszString)
    {
        free(pszString);
        pszString = HI_NULL;
        s32Ret = HI_CONFACCESS_GetInt(SCENE_INIPARAM, pszIniModule, "dynamic_ldci:EnableCount", 0, &s32Value);
        if (HI_SUCCESS != s32Ret)
        {
            MLOGE("load dynamic_gamma:ISOThreshold failed\n");
            return HI_FAILURE;
        }
        pstDynamicLdci->u32EnableCnt = (HI_U32)s32Value;
    }

    snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "dynamic_ldci:EnableExpThreshLtoH");/*EnableExpThreshLtoH*/
    s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
    SCENE_INIPARAM_CHECK_LOAD_RESULT(s32Ret, aszIniNodeName);
    if (HI_NULL != pszString)
    {
        SCENE_GetNumbersInOneLine(pszString);
        for (u32IdxM = 0; u32IdxM < pstDynamicLdci->u32EnableCnt; u32IdxM++)
        {
            pstDynamicLdci->au64EnableExpThreshLtoH[u32IdxM] = s_as64LineNum[u32IdxM];
        }
        free(pszString);
        pszString = HI_NULL;
    }

    snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "dynamic_ldci:Enable");/*Enable*/
    s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
    SCENE_INIPARAM_CHECK_LOAD_RESULT(s32Ret, aszIniNodeName);
    if (HI_NULL != pszString)
    {
        SCENE_GetNumbersInOneLine(pszString);
        for (u32IdxM = 0; u32IdxM < pstDynamicLdci->u32EnableCnt; u32IdxM++)
        {
            pstDynamicLdci->au8Enable[u32IdxM] = s_as64LineNum[u32IdxM];
        }
        free(pszString);
        pszString = HI_NULL;
    }

    snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "dynamic_ldci:ExpThreshCnt");/*ExpThreshCnt*/
    s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
    SCENE_INIPARAM_CHECK_LOAD_RESULT(s32Ret, aszIniNodeName);
    if (HI_NULL != pszString)
    {
        free(pszString);
        pszString = HI_NULL;
        s32Ret = HI_CONFACCESS_GetInt(SCENE_INIPARAM, pszIniModule, "dynamic_ldci:ExpThreshCnt", 0, &s32Value);
        if (HI_SUCCESS != s32Ret)
        {
            MLOGE("load dynamic_gamma:ExpThreshCnt failed\n");
            return HI_FAILURE;
        }
        pstDynamicLdci->u32ExpThreshCnt = (HI_U32)s32Value;
    }

    snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "dynamic_ldci:ExpThreshLtoH");/*ExpThreshLtoH*/
    s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
    SCENE_INIPARAM_CHECK_LOAD_RESULT(s32Ret, aszIniNodeName);
    if (HI_NULL != pszString)
    {
        SCENE_GetNumbersInOneLine(pszString);
        for (u32IdxM = 0; u32IdxM < pstDynamicLdci->u32ExpThreshCnt; u32IdxM++)
        {
            pstDynamicLdci->au64ExpThreshLtoH[u32IdxM] = s_as64LineNum[u32IdxM];
        }
        free(pszString);
        pszString = HI_NULL;
    }

    snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "dynamic_ldci:ManualLDCIHePosWgt");/*ManualLDCIHePosWgt*/
    s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
    SCENE_INIPARAM_CHECK_LOAD_RESULT(s32Ret, aszIniNodeName);
    if (HI_NULL != pszString)
    {
        SCENE_GetNumbersInOneLine(pszString);
        for (u32IdxM = 0; u32IdxM < pstDynamicLdci->u32ExpThreshCnt; u32IdxM++)
        {
            pstDynamicLdci->au8ManualLDCIHePosWgt[u32IdxM] = s_as64LineNum[u32IdxM];
        }
        free(pszString);
        pszString = HI_NULL;
    }
    return HI_SUCCESS;
}


HI_S32 SCENE_LoadStaticDrc(const HI_CHAR *pszIniModule, HI_SCENE_STATIC_DRC_S *pstStaticDrc)
{
    HI_S32 s32Ret = HI_SUCCESS;
    HI_CHAR *pszString = NULL;
    HI_U32 u32IdxM = 0;
    HI_S32 s32Value = 0;
    HI_CHAR aszIniNodeName[SCENE_INIPARAM_NODE_NAME_LEN] = {0,};

    snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "static_drc:Enable");/*Enable*/
    s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
    SCENE_INIPARAM_CHECK_LOAD_RESULT(s32Ret, aszIniNodeName);
    if (HI_NULL != pszString)
    {
        free(pszString);
        pszString = HI_NULL;
        s32Ret = HI_CONFACCESS_GetInt(SCENE_INIPARAM, pszIniModule, "static_drc:Enable", 0, &s32Value);
        if (HI_SUCCESS != s32Ret)
        {
            MLOGE("load static_drc:Enable failed\n");
            return HI_FAILURE;
        }
        pstStaticDrc->bEnable = (HI_BOOL)s32Value;
    }

    snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "static_drc:CurveSelect");/*CurveSelect*/
    s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
    SCENE_INIPARAM_CHECK_LOAD_RESULT(s32Ret, aszIniNodeName);
    if (HI_NULL != pszString)
    {
        free(pszString);
        pszString = HI_NULL;
        s32Ret = HI_CONFACCESS_GetInt(SCENE_INIPARAM, pszIniModule, "static_drc:CurveSelect", 0, &s32Value);
        if (HI_SUCCESS != s32Ret)
        {
            MLOGE("load static_drc:CurveSelect failed\n");
            return HI_FAILURE;
        }
        pstStaticDrc->u8CurveSelect = (HI_U8)s32Value;
    }

    snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "static_drc:DRCOpType");/*DRCOpType*/
    s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
    SCENE_INIPARAM_CHECK_LOAD_RESULT(s32Ret, aszIniNodeName);
    if (HI_NULL != pszString)
    {
        free(pszString);
        pszString = HI_NULL;
        s32Ret = HI_CONFACCESS_GetInt(SCENE_INIPARAM, pszIniModule, "static_drc:DRCOpType", 0, &s32Value);
        if (HI_SUCCESS != s32Ret)
        {
            MLOGE("load static_drc:DRCOpType failed\n");
            return HI_FAILURE;
        }
        pstStaticDrc->u8DRCOpType = (HI_U8)s32Value;
    }

    snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "static_drc:DRCAutoStr");/*DRCAutoStr*/
    s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
    SCENE_INIPARAM_CHECK_LOAD_RESULT(s32Ret, aszIniNodeName);
    if (HI_NULL != pszString)
    {
        free(pszString);
        pszString = HI_NULL;
        s32Ret = HI_CONFACCESS_GetInt(SCENE_INIPARAM, pszIniModule, "static_drc:DRCAutoStr", 0, &s32Value);
        if (HI_SUCCESS != s32Ret)
        {
            MLOGE("load static_drc:DRCAutoStr failed\n");
            return HI_FAILURE;
        }
        pstStaticDrc->u16AutoStrength = (HI_U16)s32Value;
    }

    snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "static_drc:DRCAutoStrMin");/*DRCAutoStrMin*/
    s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
    SCENE_INIPARAM_CHECK_LOAD_RESULT(s32Ret, aszIniNodeName);
    if (HI_NULL != pszString)
    {
        free(pszString);
        pszString = HI_NULL;
        s32Ret = HI_CONFACCESS_GetInt(SCENE_INIPARAM, pszIniModule, "static_drc:DRCAutoStrMin", 0, &s32Value);
        if (HI_SUCCESS != s32Ret)
        {
            MLOGE("load static_drc:DRCAutoStrMin failed\n");
            return HI_FAILURE;
        }
        pstStaticDrc->u16AutoStrengthMin = (HI_U16)s32Value;
    }

    snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "static_drc:DRCAutoStrMax");/*DRCAutoStrMax*/
    s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
    SCENE_INIPARAM_CHECK_LOAD_RESULT(s32Ret, aszIniNodeName);
    if (HI_NULL != pszString)
    {
        free(pszString);
        pszString = HI_NULL;
        s32Ret = HI_CONFACCESS_GetInt(SCENE_INIPARAM, pszIniModule, "static_drc:DRCAutoStrMax", 0, &s32Value);
        if (HI_SUCCESS != s32Ret)
        {
            MLOGE("load static_drc:DRCAutoStrMax failed\n");
            return HI_FAILURE;
        }
        pstStaticDrc->u16AutoStrengthMax = (HI_U16)s32Value;
    }

    snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "static_drc:DRCToneMappingValue");/*DRCToneMappingValue*/
    s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
    SCENE_INIPARAM_CHECK_LOAD_RESULT(s32Ret, aszIniNodeName);
    if (HI_NULL != pszString)
    {
        SCENE_GetNumbersInOneLine(pszString);
        for (u32IdxM = 0; u32IdxM < HI_ISP_DRC_TM_NODE_NUM; u32IdxM++)
        {
            pstStaticDrc->au16ToneMappingValue[u32IdxM] = s_as64LineNum[u32IdxM];
        }
        free(pszString);
        pszString = HI_NULL;
    }

    return HI_SUCCESS;
}

HI_S32 SCENE_LoadDynamicDrc(const HI_CHAR *pszIniModule, HI_SCENE_DYNAMIC_DRC_S *pstDynamicDrc)
{
    HI_S32 s32Ret = HI_SUCCESS;
    HI_CHAR *pszString = NULL;
    HI_S32 s32Value = 0;
    HI_U32 u32IdxM = 0;
    HI_U32 u32IdxN = 0;
    HI_CHAR aszIniNodeName[SCENE_INIPARAM_NODE_NAME_LEN] = {0,};

    snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "dynamic_drc:RationCnt");/*RationCnt*/
    s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
    SCENE_INIPARAM_CHECK_LOAD_RESULT(s32Ret, aszIniNodeName);
    if (HI_NULL != pszString)
    {
        free(pszString);
        pszString = HI_NULL;
        s32Ret = HI_CONFACCESS_GetInt(SCENE_INIPARAM, pszIniModule, "dynamic_drc:RationCnt", 0, &s32Value);
        if (HI_SUCCESS != s32Ret)
        {
            MLOGE("load dynamic_drc:RationCnt failed\n");
            return HI_FAILURE;
        }
        pstDynamicDrc->u32RationCount = (HI_U32)s32Value;
    }

    snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "dynamic_drc:IsoCnt");/*IsoCnt*/
    s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
    SCENE_INIPARAM_CHECK_LOAD_RESULT(s32Ret, aszIniNodeName);
    if (HI_NULL != pszString)
    {
        free(pszString);
        pszString = HI_NULL;
        s32Ret = HI_CONFACCESS_GetInt(SCENE_INIPARAM, pszIniModule, "dynamic_drc:IsoCnt", 0, &s32Value);
        if (HI_SUCCESS != s32Ret)
        {
            MLOGE("load dynamic_drc:IsoCnt failed\n");
            return HI_FAILURE;
        }
        pstDynamicDrc->u32ISOCount = (HI_U32)s32Value;
    }

    snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "dynamic_drc:RationLevel");/*RationLevel*/
    s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
    SCENE_INIPARAM_CHECK_LOAD_RESULT(s32Ret, aszIniNodeName);
    if (HI_NULL != pszString)
    {
        SCENE_GetNumbersInOneLine(pszString);
        for (u32IdxM = 0; u32IdxM < pstDynamicDrc->u32RationCount; u32IdxM++)
        {
            pstDynamicDrc->au32RationLevel[u32IdxM] = s_as64LineNum[u32IdxM];
        }
        free(pszString);
        pszString = HI_NULL;
    }

    snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "dynamic_drc:IsoLevel");/*IsoLevel*/
    s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
    SCENE_INIPARAM_CHECK_LOAD_RESULT(s32Ret, aszIniNodeName);
    if (HI_NULL != pszString)
    {
        SCENE_GetNumbersInOneLine(pszString);
        for (u32IdxM = 0; u32IdxM < pstDynamicDrc->u32ISOCount; u32IdxM++)
        {
            pstDynamicDrc->au32ISOLevel[u32IdxM] = s_as64LineNum[u32IdxM];
        }
        free(pszString);
        pszString = HI_NULL;
    }

	snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "dynamic_drc:RefRatioCount");/*RefRatioCount*/
    s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
    SCENE_INIPARAM_CHECK_LOAD_RESULT(s32Ret, aszIniNodeName);
    if (HI_NULL != pszString)
    {
        free(pszString);
        pszString = HI_NULL;
        s32Ret = HI_CONFACCESS_GetInt(SCENE_INIPARAM, pszIniModule, "dynamic_drc:RefRatioCount", 0, &s32Value);
        if (HI_SUCCESS != s32Ret)
        {
            MLOGE("load dynamic_drc:RefRatioCount failed\n");
            return HI_FAILURE;
        }
        pstDynamicDrc->u32RefRatioCount = (HI_U32)s32Value;
    }

	snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "dynamic_drc:RefRatioLtoH");/*RefRatioLtoH*/
    s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
    SCENE_INIPARAM_CHECK_LOAD_RESULT(s32Ret, aszIniNodeName);
    if (HI_NULL != pszString)
    {
        SCENE_GetNumbersInOneLine(pszString);
        for (u32IdxM = 0; u32IdxM < pstDynamicDrc->u32ISOCount; u32IdxM++)
        {
            pstDynamicDrc->au32RefRatioLtoH[u32IdxM] = s_as64LineNum[u32IdxM];
        }
        free(pszString);
        pszString = HI_NULL;
    }

	snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "dynamic_drc:RefRatioAlpha");/*RefRatioAlpha*/
    s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
    SCENE_INIPARAM_CHECK_LOAD_RESULT(s32Ret, aszIniNodeName);
    if (HI_NULL != pszString)
    {
        SCENE_GetNumbersInOneLine(pszString);
        for (u32IdxM = 0; u32IdxM < pstDynamicDrc->u32ISOCount; u32IdxM++)
        {
            pstDynamicDrc->u32RefRatioAlpha[u32IdxM] = s_as64LineNum[u32IdxM];
        }
        free(pszString);
        pszString = HI_NULL;
    }

    //snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "thread_drc:ExpThreshLtoH");/*ExpThreshLtoH*/
    //s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
    //SCENE_INIPARAM_CHECK_LOAD_RESULT(s32Ret, aszIniNodeName);
    //if (HI_NULL != pszString)
    //{
    //    SCENE_GetNumbersInOneLine(pszString);
    //    for (u32IdxM = 0; u32IdxM < pstDynamicDrc->u32ExpCount; u32IdxM++)
    //    {
    //        pstDynamicDrc->au64ExpThreshLtoH[u32IdxM] = s_as64LineNum[u32IdxM];
    //    }
    //    free(pszString);
    //    pszString = HI_NULL;
    //}

    snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "dynamic_drc:Interval");/*Interval*/
    s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
    SCENE_INIPARAM_CHECK_LOAD_RESULT(s32Ret, aszIniNodeName);
    if (HI_NULL != pszString)
    {
        free(pszString);
        pszString = HI_NULL;
        s32Ret = HI_CONFACCESS_GetInt(SCENE_INIPARAM, pszIniModule, "dynamic_drc:Interval", 0, &s32Value);
        if (HI_SUCCESS != s32Ret)
        {
            MLOGE("load dynamic_drc:Interval failed\n");
            return HI_FAILURE;
        }
        pstDynamicDrc->u32Interval = (HI_U32)s32Value;
    }

    snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "dynamic_drc:Enable");/*Enable*/
    s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
    SCENE_INIPARAM_CHECK_LOAD_RESULT(s32Ret, aszIniNodeName);
    if (HI_NULL != pszString)
    {
        free(pszString);
        pszString = HI_NULL;
        s32Ret = HI_CONFACCESS_GetInt(SCENE_INIPARAM, pszIniModule, "dynamic_drc:Enable", 0, &s32Value);
        if (HI_SUCCESS != s32Ret)
        {
            MLOGE("load dynamic_drc:Enable failed\n");
            return HI_FAILURE;
        }
        pstDynamicDrc->bEnable = (HI_BOOL)s32Value;
    }

    snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "dynamic_drc:DRCToneMappingValue1");/*DRCToneMapping_Curve1*/
    s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
    SCENE_INIPARAM_CHECK_LOAD_RESULT(s32Ret, aszIniNodeName);
    if (HI_NULL != pszString)
    {
        SCENE_GetNumbersInOneLine(pszString);
        for (u32IdxM = 0; u32IdxM < HI_ISP_DRC_TM_NODE_NUM; u32IdxM++)
        {
            pstDynamicDrc->au16ToneMappingValue1[u32IdxM] = s_as64LineNum[u32IdxM];
        }
        free(pszString);
        pszString = HI_NULL;
    }

    snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "dynamic_drc:DRCToneMappingValue2");/*DRCToneMappingValue_Curve2*/
    s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
    SCENE_INIPARAM_CHECK_LOAD_RESULT(s32Ret, aszIniNodeName);
    if (HI_NULL != pszString)
    {
        SCENE_GetNumbersInOneLine(pszString);
        for (u32IdxM = 0; u32IdxM < HI_ISP_DRC_TM_NODE_NUM; u32IdxM++)
        {
            pstDynamicDrc->au16ToneMappingValue2[u32IdxM] = s_as64LineNum[u32IdxM];
        }
        free(pszString);
        pszString = HI_NULL;
    }

    snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "dynamic_drc:LocalMixingBrightMax");/*LocalMixingBrightMax*/
    s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
    SCENE_INIPARAM_CHECK_LOAD_RESULT(s32Ret, aszIniNodeName);
    if (HI_NULL != pszString)
    {
        SCENE_GetNumbersInOneLine(pszString);
        for (u32IdxM = 0; u32IdxM < pstDynamicDrc->u32ISOCount; u32IdxM++)
        {
            pstDynamicDrc->au8LocalMixingBrightMax[u32IdxM] = s_as64LineNum[u32IdxM];
        }
        free(pszString);
        pszString = HI_NULL;
    }

    snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "dynamic_drc:LocalMixingBrightMin");/*LocalMixingBrightMin*/
    s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
    SCENE_INIPARAM_CHECK_LOAD_RESULT(s32Ret, aszIniNodeName);
    if (HI_NULL != pszString)
    {
        SCENE_GetNumbersInOneLine(pszString);
        for (u32IdxM = 0; u32IdxM < pstDynamicDrc->u32ISOCount; u32IdxM++)
        {
            pstDynamicDrc->au8LocalMixingBrightMin[u32IdxM] = s_as64LineNum[u32IdxM];
        }
        free(pszString);
        pszString = HI_NULL;
    }

    snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "dynamic_drc:LocalMixingDarkMax");/*LocalMixingDarkMax*/
    s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
    SCENE_INIPARAM_CHECK_LOAD_RESULT(s32Ret, aszIniNodeName);
    if (HI_NULL != pszString)
    {
        SCENE_GetNumbersInOneLine(pszString);
        for (u32IdxM = 0; u32IdxM < pstDynamicDrc->u32ISOCount; u32IdxM++)
        {
            pstDynamicDrc->au8LocalMixingDarkMax[u32IdxM] = s_as64LineNum[u32IdxM];
        }
        free(pszString);
        pszString = HI_NULL;
    }

    snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "dynamic_drc:LocalMixingDarkMin");/*LocalMixingDarkMin*/
    s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
    SCENE_INIPARAM_CHECK_LOAD_RESULT(s32Ret, aszIniNodeName);
    if (HI_NULL != pszString)
    {
        SCENE_GetNumbersInOneLine(pszString);
        for (u32IdxM = 0; u32IdxM < pstDynamicDrc->u32ISOCount; u32IdxM++)
        {
            pstDynamicDrc->au8LocalMixingDarkMin[u32IdxM] = s_as64LineNum[u32IdxM];
        }
        free(pszString);
        pszString = HI_NULL;
    }

    snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "dynamic_drc:DetailAdjustFactor");/*DetailAdjustFactor*/
    s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
    SCENE_INIPARAM_CHECK_LOAD_RESULT(s32Ret, aszIniNodeName);
    if (HI_NULL != pszString)
    {
        SCENE_GetNumbersInOneLine(pszString);
        for (u32IdxM = 0; u32IdxM < pstDynamicDrc->u32ISOCount; u32IdxM++)
        {
            pstDynamicDrc->as8DetailAdjustFactor[u32IdxM] = s_as64LineNum[u32IdxM];
        }
        free(pszString);
        pszString = HI_NULL;
    }

    snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "dynamic_drc:SpatialFltCoef");/*SpatialFltCoef*/
    s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
    SCENE_INIPARAM_CHECK_LOAD_RESULT(s32Ret, aszIniNodeName);
    if (HI_NULL != pszString)
    {
        SCENE_GetNumbersInOneLine(pszString);
        for (u32IdxM = 0; u32IdxM < pstDynamicDrc->u32ISOCount; u32IdxM++)
        {
            pstDynamicDrc->au8SpatialFltCoef[u32IdxM] = s_as64LineNum[u32IdxM];
        }
        free(pszString);
        pszString = HI_NULL;
    }

    snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "dynamic_drc:RangeFltCoef");/*RangeFltCoef*/
    s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
    SCENE_INIPARAM_CHECK_LOAD_RESULT(s32Ret, aszIniNodeName);
    if (HI_NULL != pszString)
    {
        SCENE_GetNumbersInOneLine(pszString);
        for (u32IdxM = 0; u32IdxM < pstDynamicDrc->u32ISOCount; u32IdxM++)
        {
            pstDynamicDrc->au8RangeFltCoef[u32IdxM] = s_as64LineNum[u32IdxM];
        }
        free(pszString);
        pszString = HI_NULL;
    }

    snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "dynamic_drc:GradRevMax");/*GradRevMax*/
    s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
    SCENE_INIPARAM_CHECK_LOAD_RESULT(s32Ret, aszIniNodeName);
    if (HI_NULL != pszString)
    {
        SCENE_GetNumbersInOneLine(pszString);
        for (u32IdxM = 0; u32IdxM < pstDynamicDrc->u32ISOCount; u32IdxM++)
        {
            pstDynamicDrc->au8GradRevMax[u32IdxM] = s_as64LineNum[u32IdxM];
        }
        free(pszString);
        pszString = HI_NULL;
    }

    snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "dynamic_drc:GradRevThr");/*GradRevThr*/
    s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
    SCENE_INIPARAM_CHECK_LOAD_RESULT(s32Ret, aszIniNodeName);
    if (HI_NULL != pszString)
    {
        SCENE_GetNumbersInOneLine(pszString);
        for (u32IdxM = 0; u32IdxM < pstDynamicDrc->u32ISOCount; u32IdxM++)
        {
            pstDynamicDrc->au8GradRevThr[u32IdxM] = s_as64LineNum[u32IdxM];
        }
        free(pszString);
        pszString = HI_NULL;
    }

    snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "dynamic_drc:Compress");/*Compress*/
    s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
    SCENE_INIPARAM_CHECK_LOAD_RESULT(s32Ret, aszIniNodeName);
    if (HI_NULL != pszString)
    {
        SCENE_GetNumbersInOneLine(pszString);
        for (u32IdxM = 0; u32IdxM < pstDynamicDrc->u32ISOCount; u32IdxM++)
        {
            pstDynamicDrc->au8Compress[u32IdxM] = s_as64LineNum[u32IdxM];
        }
        free(pszString);
        pszString = HI_NULL;
    }

    snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "dynamic_drc:Stretch");/*Stretch*/
    s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
    SCENE_INIPARAM_CHECK_LOAD_RESULT(s32Ret, aszIniNodeName);
    if (HI_NULL != pszString)
    {
        SCENE_GetNumbersInOneLine(pszString);
        for (u32IdxM = 0; u32IdxM < pstDynamicDrc->u32ISOCount; u32IdxM++)
        {
            pstDynamicDrc->au8Stretch[u32IdxM] = s_as64LineNum[u32IdxM];
        }
        free(pszString);
        pszString = HI_NULL;
    }

    snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "dynamic_drc:AutoStrength_0");/*AutoStrength_0*/
    s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
    SCENE_INIPARAM_CHECK_LOAD_RESULT(s32Ret, aszIniNodeName);
    if (HI_NULL != pszString)
    {
        free(pszString);
        pszString = HI_NULL;
        for (u32IdxM = 0; u32IdxM < pstDynamicDrc->u32ISOCount; u32IdxM++)
        {
            snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "dynamic_drc:AutoStrength_%d", u32IdxM);
            s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
            if (HI_SUCCESS != s32Ret)
            {
                MLOGE("load %s failed\n", aszIniNodeName);
                return HI_FAILURE;
            }
            if (NULL != pszString)
            {
                SCENE_GetNumbersInOneLine(pszString);
                for (u32IdxN = 0; u32IdxN < pstDynamicDrc->u32RationCount; u32IdxN++)
                {
                    pstDynamicDrc->u16AutoStrength[u32IdxM][u32IdxN] = (HI_U16)s_as64LineNum[u32IdxN];
                }
                free(pszString);
                pszString = HI_NULL;
            }
        }
    }
    return HI_SUCCESS;
}

HI_S32 SCENE_LoadStaticDehaze(const HI_CHAR *pszIniModule, HI_SCENE_STATIC_DEHAZE_S *pstStaticDeHaze)
{
    HI_S32 s32Ret = HI_SUCCESS;
    HI_CHAR *pszString = NULL;
    HI_U32 u32IdxM = 0;
    HI_S32 s32Value = 0;
    HI_CHAR aszIniNodeName[SCENE_INIPARAM_NODE_NAME_LEN] = {0,};

    snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "static_dehaze:Enable");/*Enable*/
    s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
    SCENE_INIPARAM_CHECK_LOAD_RESULT(s32Ret, aszIniNodeName);
    if (HI_NULL != pszString)
    {
        free(pszString);
        pszString = HI_NULL;
        s32Ret = HI_CONFACCESS_GetInt(SCENE_INIPARAM, pszIniModule, "static_dehaze:Enable", 0, &s32Value);
        if (HI_SUCCESS != s32Ret)
        {
            MLOGE("load static_dehaze:bEnable failed\n");
            return HI_FAILURE;
        }
        pstStaticDeHaze->bEnable = (HI_BOOL)s32Value;
    }

    snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "static_dehaze:DehazeOpType");/*DehazeOpType*/
    s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
    SCENE_INIPARAM_CHECK_LOAD_RESULT(s32Ret, aszIniNodeName);
    if (HI_NULL != pszString)
    {
        free(pszString);
        pszString = HI_NULL;
        s32Ret = HI_CONFACCESS_GetInt(SCENE_INIPARAM, pszIniModule, "static_dehaze:DehazeOpType", 0, &s32Value);
        if (HI_SUCCESS != s32Ret)
        {
            MLOGE("load static_dehaze:DehazeOpType failed\n");
            return HI_FAILURE;
        }
        pstStaticDeHaze->u8DehazeOpType = (HI_U8)s32Value;
    }

    snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "static_dehaze:DehazeUserLutEnable");/*DehazeUserLutEnable*/
    s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
    SCENE_INIPARAM_CHECK_LOAD_RESULT(s32Ret, aszIniNodeName);
    if (HI_NULL != pszString)
    {
        free(pszString);
        pszString = HI_NULL;
        s32Ret = HI_CONFACCESS_GetInt(SCENE_INIPARAM, pszIniModule, "static_dehaze:DehazeUserLutEnable", 0, &s32Value);
        if (HI_SUCCESS != s32Ret)
        {
            MLOGE("load static_dehaze:bDehazeUserLutEnable failed\n");
            return HI_FAILURE;
        }
        pstStaticDeHaze->bUserLutEnable = (HI_BOOL)s32Value;
    }

    snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "static_dehaze:DehazeLut");/*DehazeLut*/
    s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
    SCENE_INIPARAM_CHECK_LOAD_RESULT(s32Ret, aszIniNodeName);
    if (HI_NULL != pszString)
    {
        SCENE_GetNumbersInOneLine(pszString);
        for (u32IdxM = 0; u32IdxM < 256; u32IdxM++)
        {
            pstStaticDeHaze->au8DehazeLut[u32IdxM] = s_as64LineNum[u32IdxM];
        }
        free(pszString);
        pszString = HI_NULL;
    }

    return HI_SUCCESS;
}

HI_S32 SCENE_LoadDynamicDehaze(const HI_CHAR *pszIniModule, HI_SCENE_DYNAMIC_DEHAZE_S *pstDynamicDehaze)
{
    HI_S32 s32Ret = HI_SUCCESS;
    HI_CHAR *pszString = NULL;
    HI_S32 s32Value = 0;
    HI_U32 u32IdxM = 0;
    HI_CHAR aszIniNodeName[SCENE_INIPARAM_NODE_NAME_LEN] = {0,};

    snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "dynamic_dehaze:ExpThreshCnt");/*ExpThreshCnt*/
    s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
    SCENE_INIPARAM_CHECK_LOAD_RESULT(s32Ret, aszIniNodeName);
    if (HI_NULL != pszString)
    {
        free(pszString);
        pszString = HI_NULL;
        s32Ret = HI_CONFACCESS_GetInt(SCENE_INIPARAM, pszIniModule, "dynamic_dehaze:ExpThreshCnt", 0, &s32Value);
        if (HI_SUCCESS != s32Ret)
        {
            MLOGE("load dynamic_dehaze:ExpThreshCnt failed\n");
            return HI_FAILURE;
        }
        pstDynamicDehaze->u32ExpThreshCnt = (HI_U32)s32Value;
    }

    snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "dynamic_dehaze:ExpThreshLtoH");/*ExpThreshLtoH*/
    s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
    SCENE_INIPARAM_CHECK_LOAD_RESULT(s32Ret, aszIniNodeName);
    if (HI_NULL != pszString)
    {
        SCENE_GetNumbersInOneLine(pszString);
        for (u32IdxM = 0; u32IdxM < pstDynamicDehaze->u32ExpThreshCnt; u32IdxM++)
        {
            pstDynamicDehaze->au64ExpThreshLtoH[u32IdxM] = s_as64LineNum[u32IdxM];
        }
        free(pszString);
        pszString = HI_NULL;
    }

    snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "dynamic_dehaze:ManualDehazeStr");/*ManualDehazeStr*/
    s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
    SCENE_INIPARAM_CHECK_LOAD_RESULT(s32Ret, aszIniNodeName);
    if (HI_NULL != pszString)
    {
        SCENE_GetNumbersInOneLine(pszString);
        for (u32IdxM = 0; u32IdxM < pstDynamicDehaze->u32ExpThreshCnt; u32IdxM++)
        {
            pstDynamicDehaze->au8ManualStrength[u32IdxM] = s_as64LineNum[u32IdxM];
        }
        free(pszString);
        pszString = HI_NULL;
    }
    return HI_SUCCESS;
}

HI_S32 SCENE_LoadStatic3DNR(const HI_CHAR *pszIniModule, HI_SCENE_STATIC_3DNR_S *pstStatic3DNR)
{
    HI_S32 s32Ret = HI_SUCCESS;
    HI_CHAR *pszString = NULL;
    HI_U32 u32IdxM = 0;
    HI_S32 s32Value = 0;
    HI_CHAR aszIniNodeName[SCENE_INIPARAM_NODE_NAME_LEN] = {0,};

    snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "static_3dnr:3DNRCount");/*3DNRCount*/
    s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
    SCENE_INIPARAM_CHECK_LOAD_RESULT(s32Ret, aszIniNodeName);
    if (HI_NULL != pszString)
    {
        free(pszString);
        pszString = HI_NULL;
        s32Ret = HI_CONFACCESS_GetInt(SCENE_INIPARAM, pszIniModule, "static_3dnr:3DNRCount", 0, &s32Value);
        if (HI_SUCCESS != s32Ret)
        {
            MLOGE("load static_3dnr:3DNRCount failed\n");
            return HI_FAILURE;
        }
        pstStatic3DNR->u32ThreeDNRCount = (HI_U32)s32Value;
    }

    snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "static_3dnr:IsoThresh");/*IsoThresh*/
    s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
    SCENE_INIPARAM_CHECK_LOAD_RESULT(s32Ret, aszIniNodeName);
    if (HI_NULL != pszString)
    {
        SCENE_GetNumbersInOneLine(pszString);
        for (u32IdxM = 0; u32IdxM < pstStatic3DNR->u32ThreeDNRCount; u32IdxM++)
        {
            pstStatic3DNR->au32ThreeDNRIso[u32IdxM] = s_as64LineNum[u32IdxM];
        }
        free(pszString);
        pszString = HI_NULL;
    }

    snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "static_3dnr:3DnrParam_0");/*3DnrParam_0*/
    s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
    SCENE_INIPARAM_CHECK_LOAD_RESULT(s32Ret, aszIniNodeName);
    if (HI_NULL != pszString)
    {
        free(pszString);
        pszString = HI_NULL;
        for (u32IdxM = 0; u32IdxM < pstStatic3DNR->u32ThreeDNRCount; u32IdxM++)
        {
            snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "static_3dnr:3DnrParam_%d", u32IdxM);
            s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
            if (HI_SUCCESS != s32Ret)
            {
                MLOGE("load %s failed\n", aszIniNodeName);
                return HI_FAILURE;
            }
            if (NULL != pszString)
            {
                MCREn = 0;
                MCRTmp = 0;
                HI_SCENE_3DNR_S* pX = &(pstStatic3DNR->astThreeDNRValue[u32IdxM]);
                tV56xIEy*  pi = pX->IEy;
                tV56xSFy*  ps = pX->SFy;
                tV56xMDy*  pm = pX->MDy;
                tV56xTFy*  pt = pX->TFy;

#undef  _tmprt1_
#define _tmprt1_( ps, X )  ps[0].X, ps[1].X, ps[2].X, ps[3].X

#undef  _tmprt2_
#define _tmprt2_( ps,    X,Y )  ps[0].X,ps[0].Y,  ps[1].X,ps[1].Y,  ps[2].X,ps[2].Y,  ps[3].X, ps[3].Y
#undef  _tmprt2y
#define _tmprt2y( ps,    X,Y )  ps[0].X,ps[0].Y,  ps[1].X,ps[1].Y

#undef  _t3_
#define _t3_( ie, n,   X,Y,Z )            ie[n].X, ie[n].Y, ie[n].Z
#undef  _t4_
#define _t4_( ie, n, K,X,Y,Z )   ie[n].K, ie[n].X, ie[n].Y, ie[n].Z

#undef  _tmprt3y
#define _tmprt3y( ps,    X,Y,Z )  _t3_( ps,0,  X,Y,Z ),_t3_( ps,1,  X,Y,Z )
#undef  _tmprt3_
#define _tmprt3_( ps,    X,Y,Z )  _t3_( ps,0,  X,Y,Z ),_t3_( ps,1,  X,Y,Z ),_t3_( ps,2,  X,Y,Z ),_t3_( ps,3,  X,Y,Z )
#undef  _tmprt4_
#define _tmprt4_( ps,  K,X,Y,Z )  _t4_( ps,0,K,X,Y,Z ),_t4_( ps,1,K,X,Y,Z ),_t4_( ps,2,K,X,Y,Z ),_t4_( ps,3,K,X,Y,Z )

                sscanf(pszString,
                       " -en                %3d |             %3d |             %3d |             %3d"
                       "                        |                 |                 |                "
                       " -nXsf2     %3d:%3d:%3d |     %3d:%3d:%3d |     %3d:%3d:%3d |     %3d:%3d:%3d"
                       "                        |                 |                 |                "
                       " -nXsf4     %3d:%3d:%3d |     %3d:%3d:%3d |     %3d:%3d:%3d |     %3d:%3d:%3d"
                       " -bwsf4             %3d |             %3d |             %3d |             %3d"
                       " -kmsf4                 |                 |             %3d |             %3d"
                       "                        |                 |                 |                "
                       " -nXsf5 %3d:%3d:%3d:%3d |                 |                 | %3d:%3d:%3d:%3d"
                       " -dzsf5             %3d |                 |                 |             %3d"
                       "                        |                 |                 |                "
                       " -nXsf6     %3d:%3d:%3d |     %3d:%3d:%3d |     %3d:%3d:%3d |     %3d:%3d:%3d"
                       "                        |                 |                 |                "
                       " -trith             %3d |             %3d |             %3d |             %3d"
                       " -nXsfn %3d:%3d:%3d:%3d | %3d:%3d:%3d:%3d | %3d:%3d:%3d:%3d | %3d:%3d:%3d:%3d"
                       " -nXsth     %3d:%3d:%3d |     %3d:%3d:%3d |     %3d:%3d:%3d |     %3d:%3d:%3d"
                       " -tedge     %3d         |     %3d         |     %3d         |                "
                       "                        |                 |                 |                "
                       " -sfr   (0)         %3d |             %3d |             %3d |             %3d"
                       "                        |                 |                 |                "
                       " -ref                  -+-    %3d        -+-                |                "
                       " -mcnr          %3d     |                 |                 |                "
                       "                        |                 |                 |                "
                       " -nXstr (1)             |         %3d:%3d |         %3d:%3d |                "
                       "                        |                 |                 |                "
                       " -nXtss                 |         %3d:%3d |         %3d:%3d |                "
                       " -nXtfs (3)             |         %3d:%3d |         %3d:%3d |                "
                       "                        |                 |                 |                "
                       " -nXtfr (2)             |     %3d:%3d:    |     %3d:%3d:    |                "
                       "                        |     %3d:%3d:%3d |     %3d:%3d:%3d |                "
                       "                        |                 |                 |                "
                       " -mXid                  |     %3d:%3d:%3d |     %3d:%3d:%3d +-----------------"
                       "                        |                 |                 |   -sfc      %3d"
                       " -mXdb                  |         %3d:%3d |         %3d:%3d |   -tfc      %3d"
                       " -madz                  |             %3d |             %3d |                "
                       "                        |                 |                 |   -csfs     %3d"
                       " -math                  |             %3d |             %3d |   -csfr     %3d"
                       " -mate                  |             %3d |             %3d |   -ciir     %3d"
                       " -mabw                  |             %3d |             %3d |                "
                       " -matw                  |             %3d |             %3d |   -ctfs     %3d"
                       " -masw                  |             %3d |             %3d |   -ctfp     %3d"
                       " -maxn                  |             %3d |             %3d |   -ctfr     %3d",
                       _tmprt1_( &ps, NRyEn ),
                       _tmprt3_( &ps, SFS2, SFT2, SBR2 ),
                       _tmprt3_( &ps, SFS4, SFT4, SBR4 ),
                       _tmprt1_( &ps, BWSF4 ),
                       &ps[2].kMode, &ps[3].kMode,
                       _t4_( &pi, 0, IES0, IES1, IES2, IES3 ), _t4_( &pi, 1, IES0, IES1, IES2, IES3 ),
                       &pi[0].IEDZ, &pi[1].IEDZ,
                       _tmprt3_( &ps, SPN6, SBN6, PBR6 ),
                       _tmprt1_( &ps, TriTh ),
                       _tmprt4_( &ps, SFN0, SFN1, SFN2, SFN3 ),
                       _tmprt3_( &ps,       STH1, STH2, STH3 ),
                       &ps[0].tEdge, &ps[1].tEdge, &ps[2].tEdge,
                       _tmprt1_( &ps, SFR ),
                       &pt[0].bRef,
                       &MCREn,
                       _tmprt2y( &pt, STR, SDZ ),
                       _tmprt2y( &pt, TSS, TSI ),
                       _tmprt2y( &pt, TFS, TDZ ),
                       _tmprt2y( &pt, TFR[0], TFR[1] ),
                       _tmprt3y( &pt, TFR[2], TFR[3], TFR[4] ),
                       _tmprt3y( &pm, MAI0,   MAI1,   MAI2   ),
                       &pX->NRc.SFC,
                       _tmprt2y( &pm, MADK, MABR ), &pX->NRc.TFC,
                       &pm[0].MADZ, &pm[1].MADZ,
                       &pX->NRc.CSFS,
                       &pm[0].MATH, &pm[1].MATH, &pX->NRc.CSFR,
                       &pm[0].MATE, &pm[1].MATE, &pX->NRc.CIIR,
                       &pm[0].MABW, &pm[1].MABW,
                       &pm[0].MATW, &pm[1].MATW, &pX->NRc.CTFS,
                       &pm[0].MASW, &pm[1].MASW, &pX->NRc.TFC,
                       &pm[0].MAXN, &pm[1].MAXN, &pX->NRc.CTFR
                      );
#if 0
                printf("\n**********************************DBG***************************************");
                printf("\n -en                %3d |             %3d |             %3d |             %3d",  _tmprt1_( ps, NRyEn ));
                printf("\n                        |                 |                 |                ");
                printf("\n -nXsf2     %3d:%3d:%3d |     %3d:%3d:%3d |     %3d:%3d:%3d |     %3d:%3d:%3d", _tmprt3_( ps, SFS2, SFT2, SBR2 ));
                printf("\n                        |                 |                 |                ");
                printf("\n -nXsf4     %3d:%3d:%3d |     %3d:%3d:%3d |     %3d:%3d:%3d |     %3d:%3d:%3d", _tmprt3_( ps, SFS4, SFT4, SBR4 ));
                printf("\n -bwsf4             %3d |             %3d |             %3d |             %3d", _tmprt1_( ps, BWSF4 ));
                printf("\n -kmsf4                 |                 |             %3d |             %3d", ps[2].kMode, ps[3].kMode);
                printf("\n                        |                 |                 |                ");
                printf("\n -nXsf5 %3d:%3d:%3d:%3d |                 |                 | %3d:%3d:%3d:%3d", _t4_( pi, 0, IES0, IES1, IES2, IES3 ), _t4_( pi, 1, IES0, IES1, IES2, IES3 ));
                printf("\n -dzsf5             %3d |                 |                 |             %3d",       pi[0].IEDZ, pi[1].IEDZ );
                printf("\n                        |                 |                 |                ");
                printf("\n -nXsf6     %3d:%3d:%3d |     %3d:%3d:%3d |     %3d:%3d:%3d |     %3d:%3d:%3d", _tmprt3_( ps, SPN6, SBN6, PBR6 ));
                printf("\n                        |                 |                 |                ");
                printf("\n -trith             %3d |             %3d |             %3d |             %3d", _tmprt1_( ps, TriTh ));
                printf("\n -nXsfn %3d:%3d:%3d:%3d | %3d:%3d:%3d:%3d | %3d:%3d:%3d:%3d | %3d:%3d:%3d:%3d", _tmprt4_( ps, SFN0, SFN1, SFN2, SFN3 ));
                printf("\n -nXsth     %3d:%3d:%3d |     %3d:%3d:%3d |     %3d:%3d:%3d |     %3d:%3d:%3d", _tmprt3_( ps,      STH1, STH2, STH3 ));
                printf("\n -tedge     %3d         |     %3d         |     %3d         |                ", ps[0].tEdge, ps[1].tEdge, ps[2].tEdge);
                printf("\n                        |                 |                 |                ");
                printf("\n -sfr   (0)         %3d |             %3d |             %3d |             %3d", _tmprt1_( ps, SFR ));
                printf("\n                        |                 |                 |                ");
                printf("\n -ref                  -+-    %3d        -+-                |                ", pt[0].bRef );
                printf("\n -mcnr          %3d     |                 |                 |                ", MCREn);
                printf("\n                        |                 |                 |                ");
                printf("\n -nXstr (1)             |         %3d:%3d |         %3d:%3d |                ", _tmprt2y( pt, STR, SDZ  ));
                printf("\n                        |                 |                 |                ");
                printf("\n -nXtss                 |         %3d:%3d |         %3d:%3d |                ", _tmprt2y( pt, TSS, TSI ));
                printf("\n -nXtfs (3)             |         %3d:%3d |         %3d:%3d |                ", _tmprt2y( pt, TFS, TDZ ));
                printf("\n                        |                 |                 |                ");
                printf("\n -nXtfr (2)             |     %3d:%3d:    |     %3d:%3d:    |                ", _tmprt2y( pt, TFR[0], TFR[1] ));
                printf("\n                        |     %3d:%3d:%3d |     %3d:%3d:%3d |                ", _tmprt3y( pt, TFR[2], TFR[3], TFR[4] ));
                printf("\n                        |                 |                 |                ");
                printf("\n -mXid                  |     %3d:%3d:%3d |     %3d:%3d:%3d +-----------------", _tmprt3y( pm, MAI0,   MAI1,   MAI2   ));
                printf("\n                        |                 |                 |   -sfc      %3d",                            pX->NRc.SFC );
                printf("\n -mXdb                  |         %3d:%3d |         %3d:%3d |   -tfc      %3d", _tmprt2y( pm, MADK, MABR ), pX->NRc.TFC );
                printf("\n -madz                  |             %3d |             %3d |                ",  pm[0].MADZ, pm[1].MADZ                );
                printf("\n                        |                 |                 |   -csfs     %3d",                            pX->NRc.CSFS);
                printf("\n -math                  |             %3d |             %3d |   -csfr     %3d",  pm[0].MATH, pm[1].MATH,   pX->NRc.CSFR);
                printf("\n -mate                  |             %3d |             %3d |   -ciir     %3d",  pm[0].MATE, pm[1].MATE,   pX->NRc.CIIR);
                printf("\n -mabw                  |             %3d |             %3d |                ",  pm[0].MABW, pm[1].MABW                );
                printf("\n -matw                  |             %3d |             %3d |   -ctfs     %3d",  pm[0].MATW, pm[1].MATW,   pX->NRc.CTFS);
                printf("\n -masw                  |             %3d |             %3d |   -ctfp     %3d",  pm[0].MASW, pm[1].MASW,   pX->NRc.TFC );
                printf("\n -maxn                  |             %3d |             %3d |   -ctfr     %3d",  pm[0].MAXN, pm[1].MAXN,   pX->NRc.CTFR);
                printf("\n**********************************DBG***************************************");
#endif
                free(pszString);
                pszString = HI_NULL;
            }
        }
    }

    return HI_SUCCESS;
}


HI_S32 SCENE_LoadStaticCSC(const HI_CHAR *pszIniModule, HI_SCENE_STATIC_CSC_S *pstStaticCSC)
{

    HI_S32 s32Ret = HI_SUCCESS;
    HI_CHAR *pszString = NULL;
    HI_S32 s32Value = 0;
    HI_CHAR aszIniNodeName[SCENE_INIPARAM_NODE_NAME_LEN] = {0,};
    snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "static_csc:Enable");/*Enable*/
    s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
    SCENE_INIPARAM_CHECK_LOAD_RESULT(s32Ret, aszIniNodeName);
    if (HI_NULL != pszString)
    {
        free(pszString);
        pszString = HI_NULL;
        s32Ret = HI_CONFACCESS_GetInt(SCENE_INIPARAM, pszIniModule, "static_csc:Enable", 0, &s32Value);
        if (HI_SUCCESS != s32Ret)
        {
            MLOGE("load static_csc:Enable failed\n");
            return HI_FAILURE;
        }
        pstStaticCSC->bEnable = s32Value;
    }

    snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "static_csc:Hue");/*Hue*/
    s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
    SCENE_INIPARAM_CHECK_LOAD_RESULT(s32Ret, aszIniNodeName);
    if (HI_NULL != pszString)
    {
        free(pszString);
        pszString = HI_NULL;
        s32Ret = HI_CONFACCESS_GetInt(SCENE_INIPARAM, pszIniModule, "static_csc:Hue", 0, &s32Value);
        if (HI_SUCCESS != s32Ret)
        {
            MLOGE("load static_csc:Hue failed\n");
            return HI_FAILURE;
        }
        pstStaticCSC->u8Hue = (HI_U8)s32Value;
    }

    snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "static_csc:Luma");/*Luma*/
    s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
    SCENE_INIPARAM_CHECK_LOAD_RESULT(s32Ret, aszIniNodeName);
    if (HI_NULL != pszString)
    {
        free(pszString);
        pszString = HI_NULL;
        s32Ret = HI_CONFACCESS_GetInt(SCENE_INIPARAM, pszIniModule, "static_csc:Luma", 0, &s32Value);
        if (HI_SUCCESS != s32Ret)
        {
            MLOGE("load static_csc:Luma failed\n");
            return HI_FAILURE;
        }
        pstStaticCSC->u8Luma = (HI_U8)s32Value;
    }

    snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "static_csc:Contrast");/*Contrast*/
    s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
    SCENE_INIPARAM_CHECK_LOAD_RESULT(s32Ret, aszIniNodeName);
    if (HI_NULL != pszString)
    {
        free(pszString);
        pszString = HI_NULL;
        s32Ret = HI_CONFACCESS_GetInt(SCENE_INIPARAM, pszIniModule, "static_csc:Contrast", 0, &s32Value);
        if (HI_SUCCESS != s32Ret)
        {
            MLOGE("load static_csc:Contrast failed\n");
            return HI_FAILURE;
        }
        pstStaticCSC->u8Contr = (HI_U8)s32Value;
    }

    snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "static_csc:Saturation");/*Saturation*/
    s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
    SCENE_INIPARAM_CHECK_LOAD_RESULT(s32Ret, aszIniNodeName);
    if (HI_NULL != pszString)
    {
        free(pszString);
        pszString = HI_NULL;
        s32Ret = HI_CONFACCESS_GetInt(SCENE_INIPARAM, pszIniModule, "static_csc:Saturation", 0, &s32Value);
        if (HI_SUCCESS != s32Ret)
        {
            MLOGE("load static_csc:Saturation failed\n");
            return HI_FAILURE;
        }
        pstStaticCSC->u8Satu = (HI_U8)s32Value;
    }

    snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "static_csc:ColorGamut");/*ColorGamut*/
    s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
    SCENE_INIPARAM_CHECK_LOAD_RESULT(s32Ret, aszIniNodeName);
    if (HI_NULL != pszString)
    {
        free(pszString);
        pszString = HI_NULL;
        s32Ret = HI_CONFACCESS_GetInt(SCENE_INIPARAM, pszIniModule, "static_csc:ColorGamut", 0, &s32Value);
        if (HI_SUCCESS != s32Ret)
        {
            MLOGE("load static_csc:Saturation failed\n");
            return HI_FAILURE;
        }
        pstStaticCSC->enColorGamut = s32Value;
    }

    return HI_SUCCESS;
}

HI_S32 SCENE_LoadStaticNr(const HI_CHAR *pszIniModule, HI_SCENE_STATIC_NR_S *pstStaticNr)
{
    HI_S32 s32Ret = HI_SUCCESS;
    HI_CHAR *pszString = NULL;
    HI_S32 s32Value = 0;
    HI_CHAR aszIniNodeName[SCENE_INIPARAM_NODE_NAME_LEN] = {0,};
    snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "static_nr:Enable");/*Enable*/
    s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
    SCENE_INIPARAM_CHECK_LOAD_RESULT(s32Ret, aszIniNodeName);
    if (HI_NULL != pszString)
    {
        free(pszString);
        pszString = HI_NULL;
        s32Ret = HI_CONFACCESS_GetInt(SCENE_INIPARAM, pszIniModule, "static_nr:Enable", 0, &s32Value);
        if (HI_SUCCESS != s32Ret)
        {
            MLOGE("load static_nr:Enable failed\n");
            return HI_FAILURE;
        }
        pstStaticNr->bEnable = (HI_BOOL)s32Value;
    }

    return HI_SUCCESS;
}

HI_S32 SCENE_LoadStaticSharpen(const HI_CHAR *pszIniModule, HI_SCENE_STATIC_SHARPEN_S* pstStaticSharpen)
{
    HI_S32 s32Ret = HI_SUCCESS;
    HI_CHAR *pszString = NULL;
    HI_U32 u32IdxM = 0;
    HI_U32 u32IdxN = 0;
    HI_S32 s32Value = 0;
    HI_CHAR aszIniNodeName[SCENE_INIPARAM_NODE_NAME_LEN] = {0,};
    snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "static_sharpen:Enable");/*Enable*/
    s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
    SCENE_INIPARAM_CHECK_LOAD_RESULT(s32Ret, aszIniNodeName);
    if (HI_NULL != pszString)
    {
        free(pszString);
        pszString = HI_NULL;
        s32Ret = HI_CONFACCESS_GetInt(SCENE_INIPARAM, pszIniModule, "static_sharpen:Enable", 0, &s32Value);
        if (HI_SUCCESS != s32Ret)
        {
            MLOGE("load static_shading:bEnable failed\n");
            return HI_FAILURE;
        }
        pstStaticSharpen->bEnable= (HI_BOOL)s32Value;
    }


    snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "static_sharpen:AutoTextureStr_0");/*AutoTextureStr*/
    s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
    SCENE_INIPARAM_CHECK_LOAD_RESULT(s32Ret, aszIniNodeName);
    if (HI_NULL != pszString)
    {
        free(pszString);
        pszString = HI_NULL;
        for (u32IdxM = 0; u32IdxM < ISP_SHARPEN_GAIN_NUM; u32IdxM++)
        {
            snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "static_sharpen:AutoTextureStr_%d", u32IdxM);
            s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
            if (HI_SUCCESS != s32Ret)
            {
                MLOGE("load %s failed\n", aszIniNodeName);
                return HI_FAILURE;
            }
            if (NULL != pszString)
            {
                SCENE_GetNumbersInOneLine(pszString);
                for (u32IdxN = 0; u32IdxN < ISP_AUTO_ISO_STRENGTH_NUM; u32IdxN++)
                {
                    pstStaticSharpen->au16AutoTextureStr[u32IdxM][u32IdxN] = (HI_U16)s_as64LineNum[u32IdxN];
                }
                free(pszString);
                pszString = HI_NULL;
            }
        }
    }

    snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "static_sharpen:AutoEdgeStr_0");/*AutoEdgeStr*/
    s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
    SCENE_INIPARAM_CHECK_LOAD_RESULT(s32Ret, aszIniNodeName);
    if (HI_NULL != pszString)
    {
        free(pszString);
        pszString = HI_NULL;
        for (u32IdxM = 0; u32IdxM < ISP_SHARPEN_GAIN_NUM; u32IdxM++)
        {
            snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "static_sharpen:AutoEdgeStr_%d", u32IdxM);
            s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
            if (HI_SUCCESS != s32Ret)
            {
                MLOGE("load %s failed\n", aszIniNodeName);
                return HI_FAILURE;
            }
            if (NULL != pszString)
            {
                SCENE_GetNumbersInOneLine(pszString);
                for (u32IdxN = 0; u32IdxN < ISP_AUTO_ISO_STRENGTH_NUM; u32IdxN++)
                {
                    pstStaticSharpen->au16AutoEdgeStr[u32IdxM][u32IdxN] = (HI_U16)s_as64LineNum[u32IdxN];
                }
                free(pszString);
                pszString = HI_NULL;
            }
        }
    }

    snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "static_sharpen:AutoOverShoot");/*AutoOverShoot*/
    s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
    SCENE_INIPARAM_CHECK_LOAD_RESULT(s32Ret, aszIniNodeName);
    if (HI_NULL != pszString)
    {
        SCENE_GetNumbersInOneLine(pszString);
        for (u32IdxM = 0; u32IdxM < ISP_AUTO_ISO_STRENGTH_NUM; u32IdxM++)
        {
            pstStaticSharpen->au8AutoOverShoot[u32IdxM] = s_as64LineNum[u32IdxM];
        }
        free(pszString);
        pszString = HI_NULL;
    }

    snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "static_sharpen:AutoUnderShoot");/*AutoUnderShoot*/
    s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
    SCENE_INIPARAM_CHECK_LOAD_RESULT(s32Ret, aszIniNodeName);
    if (HI_NULL != pszString)
    {
        SCENE_GetNumbersInOneLine(pszString);
        for (u32IdxM = 0; u32IdxM < ISP_AUTO_ISO_STRENGTH_NUM; u32IdxM++)
        {
            pstStaticSharpen->au8AutoUnderShoot[u32IdxM] = s_as64LineNum[u32IdxM];
        }
        free(pszString);
        pszString = HI_NULL;
    }

    snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "static_sharpen:AutoShootSupStr");/*AutoShootSupStr*/
    s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
    SCENE_INIPARAM_CHECK_LOAD_RESULT(s32Ret, aszIniNodeName);
    if (HI_NULL != pszString)
    {
        SCENE_GetNumbersInOneLine(pszString);
        for (u32IdxM = 0; u32IdxM < ISP_AUTO_ISO_STRENGTH_NUM; u32IdxM++)
        {
            pstStaticSharpen->au8ShootSupStr[u32IdxM] = s_as64LineNum[u32IdxM];
        }
        free(pszString);
        pszString = HI_NULL;
    }

    snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "static_sharpen:AutoTextureFreq");/*AutoTextureFreq*/
    s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
    SCENE_INIPARAM_CHECK_LOAD_RESULT(s32Ret, aszIniNodeName);
    if (HI_NULL != pszString)
    {
        SCENE_GetNumbersInOneLine(pszString);
        for (u32IdxM = 0; u32IdxM < ISP_AUTO_ISO_STRENGTH_NUM; u32IdxM++)
        {
            pstStaticSharpen->au16AutoTextureFreq[u32IdxM] = s_as64LineNum[u32IdxM];
        }
        free(pszString);
        pszString = HI_NULL;
    }

    snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "static_sharpen:AutoEdgeFreq");/*AutoEdgeFreq*/
    s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
    SCENE_INIPARAM_CHECK_LOAD_RESULT(s32Ret, aszIniNodeName);
    if (HI_NULL != pszString)
    {
        SCENE_GetNumbersInOneLine(pszString);
        for (u32IdxM = 0; u32IdxM < ISP_AUTO_ISO_STRENGTH_NUM; u32IdxM++)
        {
            pstStaticSharpen->au16AutoEdgeFreq[u32IdxM] = s_as64LineNum[u32IdxM];
        }
        free(pszString);
        pszString = HI_NULL;
    }

    snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "static_sharpen:AutoDetailCtrl");/*AutoDetailCtrl*/
    s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
    SCENE_INIPARAM_CHECK_LOAD_RESULT(s32Ret, aszIniNodeName);
    if (HI_NULL != pszString)
    {
        SCENE_GetNumbersInOneLine(pszString);
        for (u32IdxM = 0; u32IdxM < ISP_AUTO_ISO_STRENGTH_NUM; u32IdxM++)
        {
            pstStaticSharpen->au8AutoDetailCtrl[u32IdxM] = s_as64LineNum[u32IdxM];
        }
        free(pszString);
        pszString = HI_NULL;
    }

    snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "static_sharpen:AutoEdgeFiltStr");/*AutoEdgeFiltStr*/
    s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
    SCENE_INIPARAM_CHECK_LOAD_RESULT(s32Ret, aszIniNodeName);
    if (HI_NULL != pszString)
    {
        SCENE_GetNumbersInOneLine(pszString);
        for (u32IdxM = 0; u32IdxM < ISP_AUTO_ISO_STRENGTH_NUM; u32IdxM++)
        {
            pstStaticSharpen->au8EdgeFiltStr[u32IdxM] = s_as64LineNum[u32IdxM];
        }
        free(pszString);
        pszString = HI_NULL;
    }

    snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "static_sharpen:AutoRGain");/*AutoRGain*/
    s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
    SCENE_INIPARAM_CHECK_LOAD_RESULT(s32Ret, aszIniNodeName);
    if (HI_NULL != pszString)
    {
        SCENE_GetNumbersInOneLine(pszString);
        for (u32IdxM = 0; u32IdxM < ISP_AUTO_ISO_STRENGTH_NUM; u32IdxM++)
        {
            pstStaticSharpen->au8RGain[u32IdxM] = s_as64LineNum[u32IdxM];
        }
        free(pszString);
        pszString = HI_NULL;
    }

    snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "static_sharpen:AutoBGain");/*AutoBGain*/
    s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
    SCENE_INIPARAM_CHECK_LOAD_RESULT(s32Ret, aszIniNodeName);
    if (HI_NULL != pszString)
    {
        SCENE_GetNumbersInOneLine(pszString);
        for (u32IdxM = 0; u32IdxM < ISP_AUTO_ISO_STRENGTH_NUM; u32IdxM++)
        {
            pstStaticSharpen->au8BGain[u32IdxM] = s_as64LineNum[u32IdxM];
        }
        free(pszString);
        pszString = HI_NULL;
    }

    snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "static_sharpen:AutoSkinGain");/*AutoSkinGain*/
    s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
    SCENE_INIPARAM_CHECK_LOAD_RESULT(s32Ret, aszIniNodeName);
    if (HI_NULL != pszString)
    {
        SCENE_GetNumbersInOneLine(pszString);
        for (u32IdxM = 0; u32IdxM < ISP_AUTO_ISO_STRENGTH_NUM; u32IdxM++)
        {
            pstStaticSharpen->au8SkinGain[u32IdxM] = s_as64LineNum[u32IdxM];
        }
        free(pszString);
        pszString = HI_NULL;
    }

    return HI_SUCCESS;
}

HI_S32 SCENE_LoadStaticClut(const HI_CHAR *pszIniModule, HI_SCENE_STATIC_CLUT_S* pstStaticClut)
{
    HI_S32 s32Ret = HI_SUCCESS;
    HI_CHAR *pszString = NULL;
    HI_S32 s32Value = 0;
    HI_CHAR aszIniNodeName[SCENE_INIPARAM_NODE_NAME_LEN] = {0,};


    snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "static_clut:Enable");/*Enable*/
    s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
    SCENE_INIPARAM_CHECK_LOAD_RESULT(s32Ret, aszIniNodeName);
    if (HI_NULL != pszString)
    {
        free(pszString);
        pszString = HI_NULL;
        s32Ret = HI_CONFACCESS_GetInt(SCENE_INIPARAM, pszIniModule, "static_clut:Enable", 0, &s32Value);
        if (HI_SUCCESS != s32Ret)
        {
            MLOGE("load static_shading:bEnable failed\n");
            return HI_FAILURE;
        }
        pstStaticClut->bEnable= (HI_BOOL)s32Value;
    }

    return HI_SUCCESS;
}

HI_S32 SCENE_LoadStaticCa(const HI_CHAR *pszIniModule, HI_SCENE_STATIC_CA_S* pstStaticCa)
{
    HI_S32 s32Ret = HI_SUCCESS;
    HI_CHAR *pszString = NULL;
    HI_S32 s32Value = 0;
    HI_CHAR aszIniNodeName[SCENE_INIPARAM_NODE_NAME_LEN] = {0,};


    snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "static_ca:Enable");/*Enable*/
    s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
    SCENE_INIPARAM_CHECK_LOAD_RESULT(s32Ret, aszIniNodeName);
    if (HI_NULL != pszString)
    {
        free(pszString);
        pszString = HI_NULL;
        s32Ret = HI_CONFACCESS_GetInt(SCENE_INIPARAM, pszIniModule, "static_ca:Enable", 0, &s32Value);
        if (HI_SUCCESS != s32Ret)
        {
            MLOGE("load static_ca:bEnable failed\n");
            return HI_FAILURE;
        }
        pstStaticCa->bEnable= (HI_BOOL)s32Value;
    }

    return HI_SUCCESS;
}

HI_S32 SCENE_LoadStaticCrossTalk(const HI_CHAR *pszIniModule, HI_SCENE_STATIC_CROSSTALK_S* pstStaticCrossTalk)
{
    HI_S32 s32Ret = HI_SUCCESS;
    HI_CHAR *pszString = NULL;
    HI_S32 s32Value = 0;
    HI_CHAR aszIniNodeName[SCENE_INIPARAM_NODE_NAME_LEN] = {0,};
    snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "static_crosstalk:Enable");/*Enable*/
    s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
    SCENE_INIPARAM_CHECK_LOAD_RESULT(s32Ret, aszIniNodeName);
    if (HI_NULL != pszString)
    {
        free(pszString);
        pszString = HI_NULL;
        s32Ret = HI_CONFACCESS_GetInt(SCENE_INIPARAM, pszIniModule, "static_crosstalk:Enable", 0, &s32Value);
        if (HI_SUCCESS != s32Ret)
        {
            MLOGE("load static_crosstalk:bEnable failed\n");
            return HI_FAILURE;
        }
        pstStaticCrossTalk->bEnable= (HI_BOOL)s32Value;
    }

    return HI_SUCCESS;
}

HI_S32 SCENE_LoadDynamicGamma(const HI_CHAR *pszIniModule, HI_SCENE_DYNAMIC_GAMMA_S *pstDynamicGamma)
{
    HI_S32 s32Ret = HI_SUCCESS;
    HI_CHAR *pszString = NULL;
    HI_S32 s32Value = 0;
    HI_U32 u32IdxM = 0;
    HI_U32 u32IdxN = 0;
    HI_CHAR aszIniNodeName[SCENE_INIPARAM_NODE_NAME_LEN] = {0,};

    snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "dynamic_gamma:TotalNum");/*TotalNum*/
    s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
    SCENE_INIPARAM_CHECK_LOAD_RESULT(s32Ret, aszIniNodeName);
    if (HI_NULL != pszString)
    {
        free(pszString);
        pszString = HI_NULL;
        s32Ret = HI_CONFACCESS_GetInt(SCENE_INIPARAM, pszIniModule, "dynamic_gamma:TotalNum", 0, &s32Value);
        if (HI_SUCCESS != s32Ret)
        {
            MLOGE("load dynamic_gamma:TotalNum failed\n");
            return HI_FAILURE;
        }
        pstDynamicGamma->u32TotalNum = (HI_U32)s32Value;
    }

    snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "dynamic_gamma:Interval");/*Interval*/
    s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
    SCENE_INIPARAM_CHECK_LOAD_RESULT(s32Ret, aszIniNodeName);
    if (HI_NULL != pszString)
    {
        free(pszString);
        pszString = HI_NULL;
        s32Ret = HI_CONFACCESS_GetInt(SCENE_INIPARAM, pszIniModule, "dynamic_gamma:Interval", 0, &s32Value);
        if (HI_SUCCESS != s32Ret)
        {
            MLOGE("load dynamic_gamma:Interval failed\n");
            return HI_FAILURE;
        }
        pstDynamicGamma->u32InterVal = (HI_U32)s32Value;
    }

    snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "dynamic_gamma:gammaExpThreshLtoH");/*ExpThreshLtoH*/
    s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
    SCENE_INIPARAM_CHECK_LOAD_RESULT(s32Ret, aszIniNodeName);
    if (HI_NULL != pszString)
    {
        SCENE_GetNumbersInOneLine(pszString);
        for (u32IdxM = 0; u32IdxM < pstDynamicGamma->u32TotalNum; u32IdxM++)
        {
            pstDynamicGamma->au64ExpThreshLtoH[u32IdxM] = s_as64LineNum[u32IdxM];
        }
        free(pszString);
        pszString = HI_NULL;
    }

    snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "dynamic_gamma:gammaExpThreshHtoL");/*ExpThreshHtoL*/
    s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
    SCENE_INIPARAM_CHECK_LOAD_RESULT(s32Ret, aszIniNodeName);
    if (HI_NULL != pszString)
    {
        SCENE_GetNumbersInOneLine(pszString);
        for (u32IdxM = 0; u32IdxM < pstDynamicGamma->u32TotalNum; u32IdxM++)
        {
            pstDynamicGamma->au64ExpThreshHtoL[u32IdxM] = s_as64LineNum[u32IdxM];
        }
        free(pszString);
        pszString = HI_NULL;
    }

    snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "dynamic_gamma:Table_0");/*Table_0*/
    s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
    SCENE_INIPARAM_CHECK_LOAD_RESULT(s32Ret, aszIniNodeName);
    if (HI_NULL != pszString)
    {
        free(pszString);
        pszString = HI_NULL;
        for (u32IdxM = 0; u32IdxM < pstDynamicGamma->u32TotalNum; u32IdxM++)
        {
            snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "dynamic_gamma:Table_%d", u32IdxM);
            s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
            if (HI_SUCCESS != s32Ret)
            {
                MLOGE("load %s failed\n", aszIniNodeName);
                return HI_FAILURE;
            }
            if (NULL != pszString)
            {
                SCENE_GetNumbersInOneLine(pszString);
                for (u32IdxN = 0; u32IdxN < GAMMA_NODE_NUM; u32IdxN++)
                {
                    pstDynamicGamma->au16Table[u32IdxM][u32IdxN] = (HI_U16)s_as64LineNum[u32IdxN];
                }
                free(pszString);
                pszString = HI_NULL;
            }
        }
    }

    return HI_SUCCESS;
}

HI_S32 SCENE_LoadStaticShading(const HI_CHAR *pszIniModule, HI_SCENE_STATIC_SHADING_S *pstStaticShading)
{
    HI_S32 s32Ret = HI_SUCCESS;
    HI_CHAR *pszString = NULL;
    HI_S32 s32Value = 0;
    HI_CHAR aszIniNodeName[SCENE_INIPARAM_NODE_NAME_LEN] = {0,};

    snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "static_shading:Enable");/*Enable*/
    s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
    SCENE_INIPARAM_CHECK_LOAD_RESULT(s32Ret, aszIniNodeName);
    if (HI_NULL != pszString)
    {
        free(pszString);
        pszString = HI_NULL;
        s32Ret = HI_CONFACCESS_GetInt(SCENE_INIPARAM, pszIniModule, "static_shading:Enable", 0, &s32Value);
        if (HI_SUCCESS != s32Ret)
        {
            MLOGE("load static_shading:bEnable failed\n");
            return HI_FAILURE;
        }
        pstStaticShading->bEnable = (HI_BOOL)s32Value;
    }

    return HI_SUCCESS;
}

HI_S32 SCENE_LoadDynamicShading(const HI_CHAR *pszIniModule, HI_SCENE_DYNAMIC_SHADING_S *pstDynamicShading)
{
    HI_S32 s32Ret = HI_SUCCESS;
    HI_CHAR *pszString = NULL;
    HI_S32 s32Value = 0;
    HI_U32 u32IdxM = 0;
    HI_CHAR aszIniNodeName[SCENE_INIPARAM_NODE_NAME_LEN] = {0,};

    snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "dynamic_shading:ExpThreshCnt");/*ExpThreshCnt*/
    s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
    SCENE_INIPARAM_CHECK_LOAD_RESULT(s32Ret, aszIniNodeName);
    if (HI_NULL != pszString)
    {
        free(pszString);
        pszString = HI_NULL;
        s32Ret = HI_CONFACCESS_GetInt(SCENE_INIPARAM, pszIniModule, "dynamic_shading:ExpThreshCnt", 0, &s32Value);
        if (HI_SUCCESS != s32Ret)
        {
            MLOGE("load dynamic_gamma:TotalNum failed\n");
            return HI_FAILURE;
        }
        pstDynamicShading->u32ExpThreshCnt = s32Value;
    }

    snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "dynamic_shading:ExpThreshLtoH");/*ExpThreshLtoH*/
    s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
    SCENE_INIPARAM_CHECK_LOAD_RESULT(s32Ret, aszIniNodeName);
    if (HI_NULL != pszString)
    {
        SCENE_GetNumbersInOneLine(pszString);
        for (u32IdxM = 0; u32IdxM < pstDynamicShading->u32ExpThreshCnt; u32IdxM++)
        {
            pstDynamicShading->au64ExpThreshLtoH[u32IdxM] = s_as64LineNum[u32IdxM];
        }
        free(pszString);
        pszString = HI_NULL;
    }

    snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "dynamic_shading:ManualStrength");/*ManualStrength*/
    s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
    SCENE_INIPARAM_CHECK_LOAD_RESULT(s32Ret, aszIniNodeName);
    if (HI_NULL != pszString)
    {
        SCENE_GetNumbersInOneLine(pszString);
        for (u32IdxM = 0; u32IdxM < pstDynamicShading->u32ExpThreshCnt; u32IdxM++)
        {
            pstDynamicShading->au16ManualStrength[u32IdxM] = (HI_U16)s_as64LineNum[u32IdxM];
        }
        free(pszString);
        pszString = HI_NULL;
    }

    return HI_SUCCESS;
}

HI_S32 SCENE_LoadDynamicFalseColor(const HI_CHAR *pszIniModule, HI_SCENE_DYNAMIC_FALSECOLOR_S *pstDynamicFalseColor)
{

    HI_S32 s32Ret = HI_SUCCESS;
    HI_CHAR *pszString = NULL;
    HI_S32 s32Value = 0;
    HI_U32 u32IdxM = 0;
    HI_CHAR aszIniNodeName[SCENE_INIPARAM_NODE_NAME_LEN] = {0,};
    snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "dynamic_falsecolor:TotalNum");/*FalsecolorExpThresh*/
    s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
    SCENE_INIPARAM_CHECK_LOAD_RESULT(s32Ret, aszIniNodeName);
    if (HI_NULL != pszString)
    {
        free(pszString);
        pszString = HI_NULL;
        s32Ret = HI_CONFACCESS_GetInt(SCENE_INIPARAM, pszIniModule, "dynamic_falsecolor:TotalNum", 0, &s32Value);
        if (HI_SUCCESS != s32Ret)
        {
            MLOGE("load dynamic_falsecolor:TotalNum failed\n");
            return HI_FAILURE;
        }
        pstDynamicFalseColor->u32TotalNum = (HI_U32)s32Value;
    }

    snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "dynamic_falsecolor:FalsecolorExpThresh");/*TotalNum*/
    s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
    SCENE_INIPARAM_CHECK_LOAD_RESULT(s32Ret, aszIniNodeName);
    if (HI_NULL != pszString)
    {
        SCENE_GetNumbersInOneLine(pszString);
        for (u32IdxM = 0; u32IdxM < pstDynamicFalseColor->u32TotalNum; u32IdxM++)
        {
            pstDynamicFalseColor->au32FalsecolorExpThresh[u32IdxM] = s_as64LineNum[u32IdxM];
        }
        free(pszString);
        pszString = HI_NULL;
    }

    snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "dynamic_falsecolor:ManualStrength");/*ManualStrength*/
    s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
    SCENE_INIPARAM_CHECK_LOAD_RESULT(s32Ret, aszIniNodeName);
    if (HI_NULL != pszString)
    {
        SCENE_GetNumbersInOneLine(pszString);
        for (u32IdxM = 0; u32IdxM < pstDynamicFalseColor->u32TotalNum; u32IdxM++)
        {
            pstDynamicFalseColor->au8ManualStrength[u32IdxM] = s_as64LineNum[u32IdxM];
        }
        free(pszString);
        pszString = HI_NULL;
    }

    return HI_SUCCESS;
}

HI_S32 SCENE_LoadDynamicDIS(const HI_CHAR *pszIniModule, HI_SCENE_DYNAMIC_DIS_S *pstDynamicDis)
{
    HI_S32 s32Ret = HI_SUCCESS;
    HI_CHAR *pszString = NULL;
    HI_S32 s32Value = 0;
    HI_U32 u32IdxM = 0;
    HI_CHAR aszIniNodeName[SCENE_INIPARAM_NODE_NAME_LEN] = {0,};
    snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "dynamic_dis:TotalNum");/*TotalNum*/
    s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
    SCENE_INIPARAM_CHECK_LOAD_RESULT(s32Ret, aszIniNodeName);
    if (HI_NULL != pszString)
    {
        free(pszString);
        pszString = HI_NULL;
        s32Ret = HI_CONFACCESS_GetInt(SCENE_INIPARAM, pszIniModule, "dynamic_dis:TotalNum", 0, &s32Value);
        if (HI_SUCCESS != s32Ret)
        {
            MLOGE("load dynamic_dis:TotalNum failed\n");
            return HI_FAILURE;
        }
        pstDynamicDis->u32TotalNum = (HI_U32)s32Value;
    }

    snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "dynamic_dis:DISIsoThresh");/*DISIsoThresh*/
    s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
    SCENE_INIPARAM_CHECK_LOAD_RESULT(s32Ret, aszIniNodeName);
    if (HI_NULL != pszString)
    {
        SCENE_GetNumbersInOneLine(pszString);
        for (u32IdxM = 0; u32IdxM < pstDynamicDis->u32TotalNum; u32IdxM++)
        {
            pstDynamicDis->au32DISIsoThresh[u32IdxM] = s_as64LineNum[u32IdxM];
        }
        free(pszString);
        pszString = HI_NULL;
    }

    snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "dynamic_dis:MovingSubjectLevel");/*MovingSubjectLevel*/
    s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
    SCENE_INIPARAM_CHECK_LOAD_RESULT(s32Ret, aszIniNodeName);
    if (HI_NULL != pszString)
    {
        SCENE_GetNumbersInOneLine(pszString);
        for (u32IdxM = 0; u32IdxM < pstDynamicDis->u32TotalNum; u32IdxM++)
        {
            pstDynamicDis->au32MovingSubjectLevel[u32IdxM] = s_as64LineNum[u32IdxM];
        }
        free(pszString);
        pszString = HI_NULL;
    }

    return HI_SUCCESS;
}

HI_S32 SCENE_LoadModuleState(const HI_CHAR *pszIniModule, HI_SCENE_MODULE_STATE_S *pstModuleState)
{
	HI_S32 s32Ret = HI_SUCCESS;
    HI_CHAR *pszString = HI_NULL;
    HI_S32 s32Value = 0;
    HI_CHAR aszIniNodeName[SCENE_INIPARAM_NODE_NAME_LEN] = {0,};

    snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "module_state:bStaticAE");/*bStaticAE*/
    s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
    SCENE_INIPARAM_CHECK_LOAD_RESULT(s32Ret, aszIniNodeName);
    if (HI_NULL != pszString)
    {
        free(pszString);
        pszString = HI_NULL;
        s32Ret = HI_CONFACCESS_GetInt(SCENE_INIPARAM, pszIniModule, "module_state:bStaticAE", 0, &s32Value);
        if (HI_SUCCESS != s32Ret)
        {
            MLOGE("load module_state:AERunInterval failed\n");
            return HI_FAILURE;
        }
        pstModuleState->bStaticAE = (HI_BOOL)s32Value;
    }

	snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "module_state:bDynamicAE");/*bDynamicAE*/
    s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
    SCENE_INIPARAM_CHECK_LOAD_RESULT(s32Ret, aszIniNodeName);
    if (HI_NULL != pszString)
    {
        free(pszString);
        pszString = HI_NULL;
        s32Ret = HI_CONFACCESS_GetInt(SCENE_INIPARAM, pszIniModule, "module_state:bDynamicAE", 0, &s32Value);
        if (HI_SUCCESS != s32Ret)
        {
            MLOGE("load module_state:bDynamicAE failed\n");
            return HI_FAILURE;
        }
        pstModuleState->bDynamicAE = (HI_BOOL)s32Value;
    }

	snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "module_state:bStaticStatistics");/*bStaticStatistics*/
    s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
    SCENE_INIPARAM_CHECK_LOAD_RESULT(s32Ret, aszIniNodeName);
    if (HI_NULL != pszString)
    {
        free(pszString);
        pszString = HI_NULL;
        s32Ret = HI_CONFACCESS_GetInt(SCENE_INIPARAM, pszIniModule, "module_state:bAeWeightTab", 0, &s32Value);
        if (HI_SUCCESS != s32Ret)
        {
            MLOGE("load module_state:bAeWeightTab failed\n");
            return HI_FAILURE;
        }
        pstModuleState->bAeWeightTab = (HI_BOOL)s32Value;
    }

	snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "module_state:bStaticGlobalCac");/*bStaticGlobalCac*/
    s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
    SCENE_INIPARAM_CHECK_LOAD_RESULT(s32Ret, aszIniNodeName);
    if (HI_NULL != pszString)
    {
        free(pszString);
        pszString = HI_NULL;
        s32Ret = HI_CONFACCESS_GetInt(SCENE_INIPARAM, pszIniModule, "module_state:bStaticGlobalCac", 0, &s32Value);
        if (HI_SUCCESS != s32Ret)
        {
            MLOGE("load module_state:bStaticGlobalCac failed\n");
            return HI_FAILURE;
        }
        pstModuleState->bStaticGlobalCac = (HI_BOOL)s32Value;
    }

	snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "module_state:bStaticLocalCac");/*bStaticLocalCac*/
    s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
    SCENE_INIPARAM_CHECK_LOAD_RESULT(s32Ret, aszIniNodeName);
    if (HI_NULL != pszString)
    {
        free(pszString);
        pszString = HI_NULL;
        s32Ret = HI_CONFACCESS_GetInt(SCENE_INIPARAM, pszIniModule, "module_state:bStaticLocalCac", 0, &s32Value);
        if (HI_SUCCESS != s32Ret)
        {
            MLOGE("load module_state:bStaticLocalCac failed\n");
            return HI_FAILURE;
        }
        pstModuleState->bStaticLocalCac = (HI_BOOL)s32Value;
    }

	snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "module_state:bStaticWdrExposure");/*bStaticWdrExposure*/
    s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
    SCENE_INIPARAM_CHECK_LOAD_RESULT(s32Ret, aszIniNodeName);
    if (HI_NULL != pszString)
    {
        free(pszString);
        pszString = HI_NULL;
        s32Ret = HI_CONFACCESS_GetInt(SCENE_INIPARAM, pszIniModule, "module_state:bStaticWdrExposure", 0, &s32Value);
        if (HI_SUCCESS != s32Ret)
        {
            MLOGE("load module_state:bStaticWdrExposure failed\n");
            return HI_FAILURE;
        }
        pstModuleState->bStaticWdrExposure = (HI_BOOL)s32Value;
    }

	snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "module_state:bStaticAWB");/*bStaticAWB*/
    s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
    SCENE_INIPARAM_CHECK_LOAD_RESULT(s32Ret, aszIniNodeName);
    if (HI_NULL != pszString)
    {
        free(pszString);
        pszString = HI_NULL;
        s32Ret = HI_CONFACCESS_GetInt(SCENE_INIPARAM, pszIniModule, "module_state:bStaticAWB", 0, &s32Value);
        if (HI_SUCCESS != s32Ret)
        {
            MLOGE("load module_state:bStaticAWB failed\n");
            return HI_FAILURE;
        }
        pstModuleState->bStaticAWB = (HI_BOOL)s32Value;
    }

	snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "module_state:bStaticAWBEx");/*bStaticAWBEx*/
    s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
    SCENE_INIPARAM_CHECK_LOAD_RESULT(s32Ret, aszIniNodeName);
    if (HI_NULL != pszString)
    {
        free(pszString);
        pszString = HI_NULL;
        s32Ret = HI_CONFACCESS_GetInt(SCENE_INIPARAM, pszIniModule, "module_state:bStaticAWBEx", 0, &s32Value);
        if (HI_SUCCESS != s32Ret)
        {
            MLOGE("load module_state:bStaticAWBEx failed\n");
            return HI_FAILURE;
        }
        pstModuleState->bStaticAWBEx = (HI_BOOL)s32Value;
    }

	snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "module_state:bStaticDRC");/*bStaticDRC*/
    s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
    SCENE_INIPARAM_CHECK_LOAD_RESULT(s32Ret, aszIniNodeName);
    if (HI_NULL != pszString)
    {
        free(pszString);
        pszString = HI_NULL;
        s32Ret = HI_CONFACCESS_GetInt(SCENE_INIPARAM, pszIniModule, "module_state:bStaticDRC", 0, &s32Value);
        if (HI_SUCCESS != s32Ret)
        {
            MLOGE("load module_state:bStaticDRC failed\n");
            return HI_FAILURE;
        }
        pstModuleState->bStaticDRC = (HI_BOOL)s32Value;
    }

	snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "module_state:bStaticDehaze");/*bStaticDehaze*/
    s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
    SCENE_INIPARAM_CHECK_LOAD_RESULT(s32Ret, aszIniNodeName);
    if (HI_NULL != pszString)
    {
        free(pszString);
        pszString = HI_NULL;
        s32Ret = HI_CONFACCESS_GetInt(SCENE_INIPARAM, pszIniModule, "module_state:bStaticDehaze", 0, &s32Value);
        if (HI_SUCCESS != s32Ret)
        {
            MLOGE("load module_state:bStaticDehaze failed\n");
            return HI_FAILURE;
        }
        pstModuleState->bStaticDehaze = (HI_BOOL)s32Value;
    }

	snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "module_state:bStaticLdci");/*bStaticLdci*/
    s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
    SCENE_INIPARAM_CHECK_LOAD_RESULT(s32Ret, aszIniNodeName);
    if (HI_NULL != pszString)
    {
        free(pszString);
        pszString = HI_NULL;
        s32Ret = HI_CONFACCESS_GetInt(SCENE_INIPARAM, pszIniModule, "module_state:bStaticLdci", 0, &s32Value);
        if (HI_SUCCESS != s32Ret)
        {
            MLOGE("load module_state:bStaticLdci failed\n");
            return HI_FAILURE;
        }
        pstModuleState->bStaticLdci = (HI_BOOL)s32Value;
    }

	snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "module_state:bStatic3DNR");/*bStatic3DNR*/
    s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
    SCENE_INIPARAM_CHECK_LOAD_RESULT(s32Ret, aszIniNodeName);
    if (HI_NULL != pszString)
    {
        free(pszString);
        pszString = HI_NULL;
        s32Ret = HI_CONFACCESS_GetInt(SCENE_INIPARAM, pszIniModule, "module_state:bStatic3DNR", 0, &s32Value);
        if (HI_SUCCESS != s32Ret)
        {
            MLOGE("load module_state:bStatic3DNR failed\n");
            return HI_FAILURE;
        }
        pstModuleState->bStatic3DNR = (HI_BOOL)s32Value;
    }

	snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "module_state:bDyanamic3DNR");/*bDyanamic3DNR*/
    s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
    SCENE_INIPARAM_CHECK_LOAD_RESULT(s32Ret, aszIniNodeName);
    if (HI_NULL != pszString)
    {
        free(pszString);
        pszString = HI_NULL;
        s32Ret = HI_CONFACCESS_GetInt(SCENE_INIPARAM, pszIniModule, "module_state:bDyanamic3DNR", 0, &s32Value);
        if (HI_SUCCESS != s32Ret)
        {
            MLOGE("load module_state:bDyanamic3DNR failed\n");
            return HI_FAILURE;
        }
        pstModuleState->bDyanamic3DNR = (HI_BOOL)s32Value;
    }

	snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "module_state:bStaticSaturation");/*bStaticSaturation*/
    s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
    SCENE_INIPARAM_CHECK_LOAD_RESULT(s32Ret, aszIniNodeName);
    if (HI_NULL != pszString)
    {
        free(pszString);
        pszString = HI_NULL;
        s32Ret = HI_CONFACCESS_GetInt(SCENE_INIPARAM, pszIniModule, "module_state:bStaticSaturation", 0, &s32Value);
        if (HI_SUCCESS != s32Ret)
        {
            MLOGE("load module_state:bStaticSaturation failed\n");
            return HI_FAILURE;
        }
        pstModuleState->bStaticSaturation = (HI_BOOL)s32Value;
    }

	snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "module_state:bStaticCCM");/*bStaticCCM*/
    s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
    SCENE_INIPARAM_CHECK_LOAD_RESULT(s32Ret, aszIniNodeName);
    if (HI_NULL != pszString)
    {
        free(pszString);
        pszString = HI_NULL;
        s32Ret = HI_CONFACCESS_GetInt(SCENE_INIPARAM, pszIniModule, "module_state:bStaticCCM", 0, &s32Value);
        if (HI_SUCCESS != s32Ret)
        {
            MLOGE("load module_state:bStaticCCM failed\n");
            return HI_FAILURE;
        }
        pstModuleState->bStaticCCM = (HI_BOOL)s32Value;
    }

	snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "module_state:bStaticNr");/*bStaticNr*/
    s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
    SCENE_INIPARAM_CHECK_LOAD_RESULT(s32Ret, aszIniNodeName);
    if (HI_NULL != pszString)
    {
        free(pszString);
        pszString = HI_NULL;
        s32Ret = HI_CONFACCESS_GetInt(SCENE_INIPARAM, pszIniModule, "module_state:bStaticNr", 0, &s32Value);
        if (HI_SUCCESS != s32Ret)
        {
            MLOGE("load module_state:bStaticNr failed\n");
            return HI_FAILURE;
        }
        pstModuleState->bStaticNr = (HI_BOOL)s32Value;
    }

	snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "module_state:bDynamicNr");/*bDynamicNr*/
    s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
    SCENE_INIPARAM_CHECK_LOAD_RESULT(s32Ret, aszIniNodeName);
    if (HI_NULL != pszString)
    {
        free(pszString);
        pszString = HI_NULL;
        s32Ret = HI_CONFACCESS_GetInt(SCENE_INIPARAM, pszIniModule, "module_state:bDynamicNr", 0, &s32Value);
        if (HI_SUCCESS != s32Ret)
        {
            MLOGE("load module_state:bDynamicNr failed\n");
            return HI_FAILURE;
        }
        pstModuleState->bDynamicNr = (HI_BOOL)s32Value;
    }

	snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "module_state:bDynamicDpc");/*bDynamicDpc*/
    s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
    SCENE_INIPARAM_CHECK_LOAD_RESULT(s32Ret, aszIniNodeName);
    if (HI_NULL != pszString)
    {
        free(pszString);
        pszString = HI_NULL;
        s32Ret = HI_CONFACCESS_GetInt(SCENE_INIPARAM, pszIniModule, "module_state:bDynamicDpc", 0, &s32Value);
        if (HI_SUCCESS != s32Ret)
        {
            MLOGE("load module_state:bDynamicDpc failed\n");
            return HI_FAILURE;
        }
        pstModuleState->bDynamicDpc = (HI_BOOL)s32Value;
    }

	snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "module_state:bDynamicBLC");/*bDynamicBLC*/
    s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
    SCENE_INIPARAM_CHECK_LOAD_RESULT(s32Ret, aszIniNodeName);
    if (HI_NULL != pszString)
    {
        free(pszString);
        pszString = HI_NULL;
        s32Ret = HI_CONFACCESS_GetInt(SCENE_INIPARAM, pszIniModule, "module_state:bDynamicBLC", 0, &s32Value);
        if (HI_SUCCESS != s32Ret)
        {
            MLOGE("load module_state:bDynamicBLC failed\n");
            return HI_FAILURE;
        }
        pstModuleState->bDynamicBLC = (HI_BOOL)s32Value;
    }

	snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "module_state:bStaticShading");/*bStaticShading*/
    s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
    SCENE_INIPARAM_CHECK_LOAD_RESULT(s32Ret, aszIniNodeName);
    if (HI_NULL != pszString)
    {
        free(pszString);
        pszString = HI_NULL;
        s32Ret = HI_CONFACCESS_GetInt(SCENE_INIPARAM, pszIniModule, "module_state:bStaticShading", 0, &s32Value);
        if (HI_SUCCESS != s32Ret)
        {
            MLOGE("load module_state:bStaticShading failed\n");
            return HI_FAILURE;
        }
        pstModuleState->bStaticShading = (HI_BOOL)s32Value;
    }

	snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "module_state:bStaticCSC");/*bStaticCSC*/
    s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
    SCENE_INIPARAM_CHECK_LOAD_RESULT(s32Ret, aszIniNodeName);
    if (HI_NULL != pszString)
    {
        free(pszString);
        pszString = HI_NULL;
        s32Ret = HI_CONFACCESS_GetInt(SCENE_INIPARAM, pszIniModule, "module_state:bStaticCSC", 0, &s32Value);
        if (HI_SUCCESS != s32Ret)
        {
            MLOGE("load module_state:bStaticCSC failed\n");
            return HI_FAILURE;
        }
        pstModuleState->bStaticCSC = (HI_BOOL)s32Value;
    }

	snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "module_state:bStaticSharpen");/*bStaticSharpen*/
    s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
    SCENE_INIPARAM_CHECK_LOAD_RESULT(s32Ret, aszIniNodeName);
    if (HI_NULL != pszString)
    {
        free(pszString);
        pszString = HI_NULL;
        s32Ret = HI_CONFACCESS_GetInt(SCENE_INIPARAM, pszIniModule, "module_state:bStaticSharpen", 0, &s32Value);
        if (HI_SUCCESS != s32Ret)
        {
            MLOGE("load module_state:bStaticSharpen failed\n");
            return HI_FAILURE;
        }
        pstModuleState->bStaticSharpen = (HI_BOOL)s32Value;
    }

	snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "module_state:bStaticClut");/*bStaticClut*/
    s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
    SCENE_INIPARAM_CHECK_LOAD_RESULT(s32Ret, aszIniNodeName);
    if (HI_NULL != pszString)
    {
        free(pszString);
        pszString = HI_NULL;
        s32Ret = HI_CONFACCESS_GetInt(SCENE_INIPARAM, pszIniModule, "module_state:bStaticClut", 0, &s32Value);
        if (HI_SUCCESS != s32Ret)
        {
            MLOGE("load module_state:bStaticClut failed\n");
            return HI_FAILURE;
        }
        pstModuleState->bStaticClut = (HI_BOOL)s32Value;
    }

	snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "module_state:bStaticCa");/*bStaticCa*/
    s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
    SCENE_INIPARAM_CHECK_LOAD_RESULT(s32Ret, aszIniNodeName);
    if (HI_NULL != pszString)
    {
        free(pszString);
        pszString = HI_NULL;
        s32Ret = HI_CONFACCESS_GetInt(SCENE_INIPARAM, pszIniModule, "module_state:bStaticCa", 0, &s32Value);
        if (HI_SUCCESS != s32Ret)
        {
            MLOGE("load module_state:bStaticCa failed\n");
            return HI_FAILURE;
        }
        pstModuleState->bStaticCa = (HI_BOOL)s32Value;
    }

	snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "module_state:bStaticCrosstalk");/*bStaticCrosstalk*/
    s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
    SCENE_INIPARAM_CHECK_LOAD_RESULT(s32Ret, aszIniNodeName);
    if (HI_NULL != pszString)
    {
        free(pszString);
        pszString = HI_NULL;
        s32Ret = HI_CONFACCESS_GetInt(SCENE_INIPARAM, pszIniModule, "module_state:bStaticCrosstalk", 0, &s32Value);
        if (HI_SUCCESS != s32Ret)
        {
            MLOGE("load module_state:bStaticCrosstalk failed\n");
            return HI_FAILURE;
        }
        pstModuleState->bStaticCrosstalk = (HI_BOOL)s32Value;
    }

	snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "module_state:bDynamicGamma");/*bDynamicGamma*/
    s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
    SCENE_INIPARAM_CHECK_LOAD_RESULT(s32Ret, aszIniNodeName);
    if (HI_NULL != pszString)
    {
        free(pszString);
        pszString = HI_NULL;
        s32Ret = HI_CONFACCESS_GetInt(SCENE_INIPARAM, pszIniModule, "module_state:bDynamicGamma", 0, &s32Value);
        if (HI_SUCCESS != s32Ret)
        {
            MLOGE("load module_state:bDynamicGamma failed\n");
            return HI_FAILURE;
        }
        pstModuleState->bDynamicGamma = (HI_BOOL)s32Value;
    }

	snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "module_state:bDynamicWdrExposure");/*bDynamicWdrExposure*/
    s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
    SCENE_INIPARAM_CHECK_LOAD_RESULT(s32Ret, aszIniNodeName);
    if (HI_NULL != pszString)
    {
        free(pszString);
        pszString = HI_NULL;
        s32Ret = HI_CONFACCESS_GetInt(SCENE_INIPARAM, pszIniModule, "module_state:bDynamicWdrExposure", 0, &s32Value);
        if (HI_SUCCESS != s32Ret)
        {
            MLOGE("load module_state:bDynamicWdrExposure failed\n");
            return HI_FAILURE;
        }
        pstModuleState->bDynamicWdrExposure = (HI_BOOL)s32Value;
    }

	snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "module_state:bDynamicShading");/*bDynamicShading*/
    s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
    SCENE_INIPARAM_CHECK_LOAD_RESULT(s32Ret, aszIniNodeName);
    if (HI_NULL != pszString)
    {
        free(pszString);
        pszString = HI_NULL;
        s32Ret = HI_CONFACCESS_GetInt(SCENE_INIPARAM, pszIniModule, "module_state:bDynamicShading", 0, &s32Value);
        if (HI_SUCCESS != s32Ret)
        {
            MLOGE("load module_state:bDynamicShading failed\n");
            return HI_FAILURE;
        }
        pstModuleState->bDynamicShading = (HI_BOOL)s32Value;
    }

	snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "module_state:bDynamicLdci");/*bDynamicLdci*/
    s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
    SCENE_INIPARAM_CHECK_LOAD_RESULT(s32Ret, aszIniNodeName);
    if (HI_NULL != pszString)
    {
        free(pszString);
        pszString = HI_NULL;
        s32Ret = HI_CONFACCESS_GetInt(SCENE_INIPARAM, pszIniModule, "module_state:bDynamicLdci", 0, &s32Value);
        if (HI_SUCCESS != s32Ret)
        {
            MLOGE("load module_state:bDynamicLdci failed\n");
            return HI_FAILURE;
        }
        pstModuleState->bDynamicLdci = (HI_BOOL)s32Value;
    }

	snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "module_state:bDynamicDehaze");/*bDynamicDehaze*/
    s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
    SCENE_INIPARAM_CHECK_LOAD_RESULT(s32Ret, aszIniNodeName);
    if (HI_NULL != pszString)
    {
        free(pszString);
        pszString = HI_NULL;
        s32Ret = HI_CONFACCESS_GetInt(SCENE_INIPARAM, pszIniModule, "module_state:bDynamicDehaze", 0, &s32Value);
        if (HI_SUCCESS != s32Ret)
        {
            MLOGE("load module_state:bDynamicDehaze failed\n");
            return HI_FAILURE;
        }
        pstModuleState->bDynamicDehaze = (HI_BOOL)s32Value;
    }

	snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "module_state:bDynamicFSWDR");/*bDynamicFSWDR*/
    s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
    SCENE_INIPARAM_CHECK_LOAD_RESULT(s32Ret, aszIniNodeName);
    if (HI_NULL != pszString)
    {
        free(pszString);
        pszString = HI_NULL;
        s32Ret = HI_CONFACCESS_GetInt(SCENE_INIPARAM, pszIniModule, "module_state:bDynamicFSWDR", 0, &s32Value);
        if (HI_SUCCESS != s32Ret)
        {
            MLOGE("load module_state:bDynamicFSWDR failed\n");
            return HI_FAILURE;
        }
        pstModuleState->bDynamicFSWDR = (HI_BOOL)s32Value;
    }

	snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "module_state:bDynamicFalseColor");/*bDynamicFalseColor*/
    s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
    SCENE_INIPARAM_CHECK_LOAD_RESULT(s32Ret, aszIniNodeName);
    if (HI_NULL != pszString)
    {
        free(pszString);
        pszString = HI_NULL;
        s32Ret = HI_CONFACCESS_GetInt(SCENE_INIPARAM, pszIniModule, "module_state:bDynamicFalseColor", 0, &s32Value);
        if (HI_SUCCESS != s32Ret)
        {
            MLOGE("load module_state:bDynamicFalseColor failed\n");
            return HI_FAILURE;
        }
        pstModuleState->bDynamicFalseColor = (HI_BOOL)s32Value;
    }

	snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "module_state:bDynamicDis");/*bDynamicDis*/
    s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
    SCENE_INIPARAM_CHECK_LOAD_RESULT(s32Ret, aszIniNodeName);
    if (HI_NULL != pszString)
    {
        free(pszString);
        pszString = HI_NULL;
        s32Ret = HI_CONFACCESS_GetInt(SCENE_INIPARAM, pszIniModule, "module_state:bDynamicDis", 0, &s32Value);
        if (HI_SUCCESS != s32Ret)
        {
            MLOGE("load module_state:bDynamicDis failed\n");
            return HI_FAILURE;
        }
        pstModuleState->bDynamicDIS = (HI_BOOL)s32Value;
    }

	snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "module_state:bDynamicDrc");/*bDynamicDrc*/
    s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
    SCENE_INIPARAM_CHECK_LOAD_RESULT(s32Ret, aszIniNodeName);
    if (HI_NULL != pszString)
    {
        free(pszString);
        pszString = HI_NULL;
        s32Ret = HI_CONFACCESS_GetInt(SCENE_INIPARAM, pszIniModule, "module_state:bDynamicDrc", 0, &s32Value);
        if (HI_SUCCESS != s32Ret)
        {
            MLOGE("load module_state:bDynamicDrc failed\n");
            return HI_FAILURE;
        }
        pstModuleState->bDynamicDrc = (HI_BOOL)s32Value;
    }

	return s32Ret;
}

HI_S32 SCENE_LoadSceneParam(const HI_CHAR *pszIniModule, HI_SCENE_PIPE_PARAM_S *pstSceneParam)
{
    HI_S32 s32Ret = HI_SUCCESS;

    s32Ret = SCENE_LoadStaticAE(pszIniModule, &pstSceneParam->stStaticAe);
    if (HI_SUCCESS != s32Ret)
    {
        MLOGE("SCENE_LoadStaticAE failed\n");
        return HI_FAILURE;
    }

    s32Ret = SCENE_LoadDynamicAE(pszIniModule, &pstSceneParam->stDynamicAe);
    if (HI_SUCCESS != s32Ret)
    {
        MLOGE("SCENE_LoadDynamicAE failed\n");
        return HI_FAILURE;
    }

    s32Ret = SCENE_LoadStaticAeRouteEX(pszIniModule, &pstSceneParam->stStaticAeRouteEx);
    if (HI_SUCCESS != s32Ret)
    {
        MLOGE("SCENE_LoadStaticAeRouteEX failed\n");
        return HI_FAILURE;
    }

    s32Ret = SCENE_LoadAeWeightTab(pszIniModule, &pstSceneParam->stStaticStatistics);
    if (HI_SUCCESS != s32Ret)
    {
        MLOGE("SCENE_LoadAeWeightTab failed\n");
        return HI_FAILURE;
    }

    s32Ret = SCENE_LoadStaticAWB(pszIniModule, &pstSceneParam->stStaticAwb);
    if (HI_SUCCESS != s32Ret)
    {
        MLOGE("SCENE_LoadStaticAWB failed\n");
        return HI_FAILURE;
    }

    s32Ret = SCENE_LoadStaticAWBEX(pszIniModule, &pstSceneParam->stStaticAwbEx);
    if (HI_SUCCESS != s32Ret)
    {
        MLOGE("SCENE_LoadStaticAWBEX failed\n");
        return HI_FAILURE;
    }

    s32Ret = SCENE_LoadStaticGlobalCac(pszIniModule, &pstSceneParam->stStaticGlobalCac);
    if (HI_SUCCESS != s32Ret)
    {
        MLOGE("SCENE_LoadStaticGlobalCac failed\n");
        return HI_FAILURE;
    }

	s32Ret = SCENE_LoadStaticLocalCac(pszIniModule, &pstSceneParam->stStaticLocalCac);
    if (HI_SUCCESS != s32Ret)
    {
        MLOGE("SCENE_LoadStaticLocalCac failed\n");
        return HI_FAILURE;
    }

    s32Ret = SCENE_LoadStaticWdrExposure(pszIniModule, &pstSceneParam->stStaticWdrExposure);
    if (HI_SUCCESS != s32Ret)
    {
        MLOGE("SCENE_LoadStaticWdrExposure failed\n");
        return HI_FAILURE;
    }

    s32Ret = SCENE_LoadStaticSaturation(pszIniModule, &pstSceneParam->stStaticSaturation);
    if (HI_SUCCESS != s32Ret)
    {
        MLOGE("SCENE_LoadStaticSaturation failed\n");
        return HI_FAILURE;
    }

    s32Ret = SCENE_LoadStaticDrc(pszIniModule, &pstSceneParam->stStaticDrc);
    if (HI_SUCCESS != s32Ret)
    {
        MLOGE("SCENE_LoadStaticDrc failed\n");
        return HI_FAILURE;
    }

    s32Ret = SCENE_LoadStaticDehaze(pszIniModule, &pstSceneParam->stStaticDehaze);
    if (HI_SUCCESS != s32Ret)
    {
        MLOGE("SCENE_LoadStaticDehaze failed\n");
        return HI_FAILURE;
    }

    s32Ret = SCENE_LoadStatic3DNR(pszIniModule, &pstSceneParam->stStaticThreeDNR);
    if (HI_SUCCESS != s32Ret)
    {
        MLOGE("SCENE_LoadStatic3DNR failed\n");
        return HI_FAILURE;
    }

    s32Ret = SCENE_LoadStaticCSC(pszIniModule, &pstSceneParam->stStaticCsc);
    if (HI_SUCCESS != s32Ret)
    {
        MLOGE("SCENE_LoadStaticCSC failed\n");
        return HI_FAILURE;
    }

    s32Ret = SCENE_LoadStaticLDCI(pszIniModule, &pstSceneParam->stStaticLdci);
    if (HI_SUCCESS != s32Ret)
    {
        MLOGE("SCENE_LoadStaticLDCI failed\n");
        return HI_FAILURE;
    }

    s32Ret = SCENE_LoadStaticNr(pszIniModule, &pstSceneParam->stStaticNr);
    if (HI_SUCCESS != s32Ret)
    {
        MLOGE("SCENE_LoadStaticNr failed\n");
        return HI_FAILURE;
    }

    s32Ret = SCENE_LoadStaticShading(pszIniModule, &pstSceneParam->stStaticShading);
    if (HI_SUCCESS != s32Ret)
    {
        MLOGE("SCENE_LoadStaticShading failed\n");
        return HI_FAILURE;
    }

	s32Ret = SCENE_LoadDynamicWdrExposure(pszIniModule, &pstSceneParam->stDynamicWdrExposure);
    if (HI_SUCCESS != s32Ret)
    {
        MLOGE("SCENE_LoadDynamicWdrExposure failed\n");
        return HI_FAILURE;
    }

    s32Ret = SCENE_LoadDynamicGamma(pszIniModule, &pstSceneParam->stDynamicGamma);
    if (HI_SUCCESS != s32Ret)
    {
        MLOGE("SCENE_LoadDynamicGamma failed\n");
        return HI_FAILURE;
    }

	s32Ret = SCENE_LoadStaticCCM(pszIniModule, &pstSceneParam->stStaticCcm);
	if (HI_SUCCESS != s32Ret)
    {
        MLOGE("SCENE_LoadStaticCCM failed\n");
        return HI_FAILURE;
    }

    s32Ret = SCENE_LoadDynamicShading(pszIniModule, &pstSceneParam->stDynamicShading);
    if (HI_SUCCESS != s32Ret)
    {
        MLOGE("SCENE_LoadDynamicShading failed\n");
        return HI_FAILURE;
    }

    s32Ret = SCENE_LoadDynamicLDCI(pszIniModule, &pstSceneParam->stDynamicLDCI);
    if (HI_SUCCESS != s32Ret)
    {
        MLOGE("SCENE_LoadDynamicLDCI failed\n");
        return HI_FAILURE;
    }

    s32Ret = SCENE_LoadDynamicDehaze(pszIniModule, &pstSceneParam->stDynamicDehaze);
    if (HI_SUCCESS != s32Ret)
    {
        MLOGE("SCENE_LoadDynamicDehaze failed\n");
        return HI_FAILURE;
    }

    s32Ret = SCENE_LoadDynamicFsWdr(pszIniModule, &pstSceneParam->stDynamicFSWDR);
    if (HI_SUCCESS != s32Ret)
    {
        MLOGE("SCENE_LoadDynamicFsWdr failed\n");
        return HI_FAILURE;
    }

    s32Ret = SCENE_LoadDynamicFalseColor(pszIniModule, &pstSceneParam->stDynamicFalsecolor);
    if (HI_SUCCESS != s32Ret)
    {
        MLOGE("SCENE_LoadDynamicFalseColor failed\n");
        return HI_FAILURE;
    }

    s32Ret = SCENE_LoadDynamicDIS(pszIniModule, &pstSceneParam->stDynamicDis);
    if (HI_SUCCESS != s32Ret)
    {
        MLOGE("SCENE_LoadDynamicDIS failed\n");
        return HI_FAILURE;
    }

    s32Ret = SCENE_LoadDynamicDrc(pszIniModule, &pstSceneParam->stDynamicDrc);
    if (HI_SUCCESS != s32Ret)
    {
        MLOGE("SCENE_LoadThreadDrc failed\n");
        return HI_FAILURE;
    }

	s32Ret = SCENE_LoadModuleState(pszIniModule, &pstSceneParam->stModuleState);
	if (HI_SUCCESS != s32Ret)
	{
		MLOGE("SCENE_LoadModuleState failed\n");
		return HI_FAILURE;
	}

    return HI_SUCCESS;
}



HI_S32 SCENE_LoadSceneConf(HI_SCENE_PIPE_PARAM_S *pstScenePipeParam, HI_S32 s32Count)
{
    HI_S32 s32Ret = HI_SUCCESS;
    HI_U32 u32ModeIdx = 0;
    HI_CHAR aszIniModuleName[SCENE_INIPARAM_MODULE_NAME_LEN] = {0,};

    for (u32ModeIdx = 0; u32ModeIdx < s32Count; u32ModeIdx++)
    {
        snprintf(aszIniModuleName, SCENE_INIPARAM_MODULE_NAME_LEN, "%s%d", SCENE_INI_SCENEMODE, u32ModeIdx);

        s32Ret = SCENE_LoadSceneParam(aszIniModuleName, pstScenePipeParam + u32ModeIdx);
        if (HI_SUCCESS != s32Ret)
        {
            MLOGE("load scene[%d] config failed\n", u32ModeIdx);
            return HI_FAILURE;
        }
    }

    return HI_SUCCESS;
}

HI_S32 SCENE_LoadVideoParam(const HI_CHAR *pszIniModule, HI_SCENE_VIDEO_MODE_S *pstVideoMode)
{
    HI_S32 s32Ret = HI_SUCCESS;
    HI_CHAR *pszString = HI_NULL;
    HI_S32 s32Value = 0;
    HI_U32 u32ModeIdx = 0;
    HI_S32 i = 0;
    HI_CHAR aszIniNodeName[SCENE_INIPARAM_NODE_NAME_LEN] = {0,};

    for (i = 0; i < SCENE_MAX_VIDEOMODE; i++)
    {
        snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "pipe_comm_%d:SCENE_MODE", i); /*SCENE_MODE*/
        s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
        SCENE_INIPARAM_CHECK_LOAD_RESULT(s32Ret, aszIniNodeName);
        if (HI_NULL != pszString)
        {
            free(pszString);
            pszString = HI_NULL;
            s32Ret = HI_CONFACCESS_GetInt(SCENE_INIPARAM, pszIniModule, aszIniNodeName, 0, &s32Value);
            if (HI_SUCCESS != s32Ret)
            {
                MLOGE("load pipe_comm:SCENE_MODE failed\n");
                return HI_FAILURE;
            }
            (pstVideoMode->astVideoMode + i)->enPipeMode = (HI_U8)s32Value;
        }

        for (u32ModeIdx = 0; u32ModeIdx < HI_SCENE_PIPE_MAX_NUM; u32ModeIdx++)
        {
            snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "pipe_%d_%d:Enable", i, u32ModeIdx);/*SCENE_MODE*/
            s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
            SCENE_INIPARAM_CHECK_LOAD_RESULT(s32Ret, aszIniNodeName);
            if (HI_NULL != pszString)
            {
                free(pszString);
                pszString = HI_NULL;
                s32Ret = HI_CONFACCESS_GetInt(SCENE_INIPARAM, pszIniModule, aszIniNodeName, 0, &s32Value);
                if (HI_SUCCESS != s32Ret)
                {
                    MLOGE("load pipe:Enable failed\n");
                    return HI_FAILURE;
                }
                (pstVideoMode->astVideoMode + i)->astPipeAttr[u32ModeIdx].bEnable = (HI_BOOL)s32Value;
            }


            snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "pipe_%d_%d:VcapPipeHdl", i, u32ModeIdx);/*SCENE_MODE*/
            s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
            SCENE_INIPARAM_CHECK_LOAD_RESULT(s32Ret, aszIniNodeName);
            if (HI_NULL != pszString)
            {
                free(pszString);
                pszString = HI_NULL;
                s32Ret = HI_CONFACCESS_GetInt(SCENE_INIPARAM, pszIniModule, aszIniNodeName, 0, &s32Value);
                if (HI_SUCCESS != s32Ret)
                {
                    MLOGE("load pipe:VcapPipeHdl failed\n");
                    return HI_FAILURE;
                }
                (pstVideoMode->astVideoMode + i)->astPipeAttr[u32ModeIdx].VcapPipeHdl = (HI_HANDLE)s32Value;
            }


            snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "pipe_%d_%d:MainPipeHdl", i, u32ModeIdx);/*SCENE_MODE*/
            s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
            SCENE_INIPARAM_CHECK_LOAD_RESULT(s32Ret, aszIniNodeName);
            if (HI_NULL != pszString)
            {
                free(pszString);
                pszString = HI_NULL;
                s32Ret = HI_CONFACCESS_GetInt(SCENE_INIPARAM, pszIniModule, aszIniNodeName, 0, &s32Value);
                if (HI_SUCCESS != s32Ret)
                {
                    MLOGE("load pipe:MainPipeHdl failed\n");
                    return HI_FAILURE;
                }
                (pstVideoMode->astVideoMode + i)->astPipeAttr[u32ModeIdx].MainPipeHdl = (HI_HANDLE)s32Value;
            }

            snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "pipe_%d_%d:PipeChnHdl", i, u32ModeIdx);/*SCENE_MODE*/
            s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
            SCENE_INIPARAM_CHECK_LOAD_RESULT(s32Ret, aszIniNodeName);
            if (HI_NULL != pszString)
            {
                free(pszString);
                pszString = HI_NULL;
                s32Ret = HI_CONFACCESS_GetInt(SCENE_INIPARAM, pszIniModule, aszIniNodeName, 0, &s32Value);
                if (HI_SUCCESS != s32Ret)
                {
                    MLOGE("load pipe:Enable PipeChnHdl\n");
                    return HI_FAILURE;
                }
                (pstVideoMode->astVideoMode + i)->astPipeAttr[u32ModeIdx].PipeChnHdl = (HI_HANDLE)s32Value;
            }

            snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "pipe_%d_%d:VpssHdl", i, u32ModeIdx);/*SCENE_MODE*/
            s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
            SCENE_INIPARAM_CHECK_LOAD_RESULT(s32Ret, aszIniNodeName);
            if (HI_NULL != pszString)
            {
                free(pszString);
                pszString = HI_NULL;
                s32Ret = HI_CONFACCESS_GetInt(SCENE_INIPARAM, pszIniModule, aszIniNodeName, 0, &s32Value);
                if (HI_SUCCESS != s32Ret)
                {
                    MLOGE("load pipe:VpssHdl failed\n");
                    return HI_FAILURE;
                }
                (pstVideoMode->astVideoMode + i)->astPipeAttr[u32ModeIdx].VpssHdl = (HI_HANDLE)s32Value;
            }

            snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "pipe_%d_%d:VportHdl", i, u32ModeIdx);/*SCENE_MODE*/
            s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
            SCENE_INIPARAM_CHECK_LOAD_RESULT(s32Ret, aszIniNodeName);
            if (HI_NULL != pszString)
            {
                free(pszString);
                pszString = HI_NULL;
                s32Ret = HI_CONFACCESS_GetInt(SCENE_INIPARAM, pszIniModule, aszIniNodeName, 0, &s32Value);
                if (HI_SUCCESS != s32Ret)
                {
                    MLOGE("load pipe:VportHdl failed\n");
                    return HI_FAILURE;
                }
                (pstVideoMode->astVideoMode + i)->astPipeAttr[u32ModeIdx].VPortHdl = (HI_HANDLE)s32Value;
            }

            snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "pipe_%d_%d:VencHdl", i, u32ModeIdx);/*SCENE_MODE*/
            s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
            SCENE_INIPARAM_CHECK_LOAD_RESULT(s32Ret, aszIniNodeName);
            if (HI_NULL != pszString)
            {
                free(pszString);
                pszString = HI_NULL;
                s32Ret = HI_CONFACCESS_GetInt(SCENE_INIPARAM, pszIniModule, aszIniNodeName, 0, &s32Value);
                if (HI_SUCCESS != s32Ret)
                {
                    MLOGE("load pipe:VencHdl failed\n");
                    return HI_FAILURE;
                }
                (pstVideoMode->astVideoMode + i)->astPipeAttr[u32ModeIdx].VencHdl = (HI_HANDLE)s32Value;
            }

            snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "pipe_%d_%d:PipeParamIndex", i, u32ModeIdx);/*SCENE_MODE*/
            s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
            SCENE_INIPARAM_CHECK_LOAD_RESULT(s32Ret, aszIniNodeName);
            if (HI_NULL != pszString)
            {
                free(pszString);
                pszString = HI_NULL;
                s32Ret = HI_CONFACCESS_GetInt(SCENE_INIPARAM, pszIniModule, aszIniNodeName, 0, &s32Value);
                if (HI_SUCCESS != s32Ret)
                {
                    MLOGE("load pipe:PipeParamIndex failed\n");
                    return HI_FAILURE;
                }
                (pstVideoMode->astVideoMode + i)->astPipeAttr[u32ModeIdx].u8PipeParamIndex = (HI_U8)s32Value;
            }

            snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "pipe_%d_%d:PipeType", i, u32ModeIdx);/*SCENE_MODE*/
            s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
            SCENE_INIPARAM_CHECK_LOAD_RESULT(s32Ret, aszIniNodeName);
            if (HI_NULL != pszString)
            {
                free(pszString);
                pszString = HI_NULL;
                s32Ret = HI_CONFACCESS_GetInt(SCENE_INIPARAM, pszIniModule, aszIniNodeName, 0, &s32Value);
                if (HI_SUCCESS != s32Ret)
                {
                    MLOGE("load pipe:PipeType failed\n");
                    return HI_FAILURE;
                }
                (pstVideoMode->astVideoMode + i)->astPipeAttr[u32ModeIdx].enPipeType = (HI_SCENE_PIPE_TYPE_E)s32Value;

            }
        }
    }
    return HI_SUCCESS;
}

HI_S32 SCENE_LoadVideoConf(HI_SCENE_VIDEO_MODE_S *pstVideoMode)
{
    HI_S32 s32Ret = HI_SUCCESS;
    HI_CHAR aszIniModuleName[SCENE_INIPARAM_MODULE_NAME_LEN] = {0,};

    snprintf(aszIniModuleName, SCENE_INIPARAM_MODULE_NAME_LEN, "%s", SCENE_INI_VIDEOMODE);

    s32Ret = SCENE_LoadVideoParam(aszIniModuleName, pstVideoMode);

    if (HI_SUCCESS != s32Ret)
    {
        MLOGE("load videomode config failed\n");
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

HI_S32 HI_SCENE_CreateParam(HI_CHAR *pdirname, HI_SCENE_PARAM_S *pstSceneParam, HI_SCENE_VIDEO_MODE_S *pstVideoMode)
{
    HI_S32 s32Ret = HI_SUCCESS;
    HI_U32 u32ModuleNum = 0;
    HI_CHAR acInipath[SCENETOOL_MAX_FILESIZE] = {0};

    /* Load Product Ini Configure */
    if (HI_NULL == pstSceneParam || HI_NULL == pstVideoMode)
    {
        MLOGE("Null Pointer.");
        return HI_SUCCESS;
    }
    
    //maohw snprintf(acInipath, SCENETOOL_MAX_FILESIZE, "%s%s", pdirname, "/config_cfgaccess_hd.ini");
    snprintf(acInipath, SCENETOOL_MAX_FILESIZE, "%s", pdirname);
    
    printf("The iniPath is %s.\n", acInipath);

    s32Ret = HI_CONFACCESS_Init(SCENE_INIPARAM, acInipath, &u32ModuleNum);
    if (HI_SUCCESS != s32Ret)
    {
        MLOGE("load ini [%s] failed [%08x]\n", acInipath, s32Ret);
        return HI_FAILURE;
    }

    memset(&s_stSceneParamCfg, 0, sizeof(HI_SCENE_PARAM_S));
    s32Ret = SCENE_LoadSceneConf(s_stSceneParamCfg.astPipeParam, u32ModuleNum - 1);
    if (HI_SUCCESS != s32Ret)
    {
        MLOGE("SCENE_LoadConf failed!\n");
        return HI_FAILURE;
    }
    memcpy(pstSceneParam , &s_stSceneParamCfg, sizeof(HI_SCENE_PARAM_S));


    memset(&s_stVideoMode, 0, sizeof(HI_SCENE_MODE_S));
    s32Ret = SCENE_LoadVideoConf(&s_stVideoMode);
    if (HI_SUCCESS != s32Ret)
    {
        MLOGE("SCENE_LoadConf failed!\n");
        return HI_FAILURE;
    }
    memcpy(pstVideoMode, &s_stVideoMode, sizeof(HI_SCENE_VIDEO_MODE_S));

    s32Ret = HI_CONFACCESS_Deinit(SCENE_INIPARAM);
    if (HI_SUCCESS != s32Ret)
    {
        MLOGE("load ini [%s] failed [%08x]\n", acInipath, s32Ret);
        return HI_FAILURE;
    }
    return HI_SUCCESS;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

