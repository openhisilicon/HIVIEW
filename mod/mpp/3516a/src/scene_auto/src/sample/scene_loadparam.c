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
#define SCENETOOL_MAX_FILESIZE (256)

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

    snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "static_ae:AutoExpTimeMax");/*AutoExpTimeMax*/
    s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
    SCENE_INIPARAM_CHECK_LOAD_RESULT(s32Ret, aszIniNodeName);
    if (HI_NULL != pszString)
    {
        free(pszString);
        pszString = HI_NULL;
        s32Ret = HI_CONFACCESS_GetInt(SCENE_INIPARAM, pszIniModule, "static_ae:AutoExpTimeMax", 0, &s32Value);
        if (HI_SUCCESS != s32Ret)
        {
            MLOGE("load static_ae:AutoExpTimeMax failed\n");
            return HI_FAILURE;
        }
        pstStaticAe->u32AutoExpTimeMax = (HI_U32)s32Value;
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
#if 0
    HI_S32 s32Ret = HI_SUCCESS;
    HI_CHAR *pszString = NULL;
    HI_S32 s32Value = 0;
    HI_U32 u32IdxM = 0;
    HI_CHAR aszIniNodeName[SCENE_INIPARAM_NODE_NAME_LEN] = {0,};

    snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "static_aeroute:u32TotalNum");/*TotalNum*/
    s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
    SCENE_INIPARAM_CHECK_LOAD_RESULT(s32Ret, aszIniNodeName);
    if (HI_NULL != pszString)
    {
        free(pszString);
        pszString = HI_NULL;
        s32Ret = HI_CONFACCESS_GetInt(SCENE_INIPARAM, pszIniModule, "static_aeroute:u32TotalNum", 0, &s32Value);
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
#endif
    return HI_SUCCESS;
}

HI_S32 SCENE_LoadStaticAWB(const HI_CHAR *pszIniModule, HI_SCENE_STATIC_AWB_S *pstStaticAwb)
{
#if 0
    HI_S32 s32Ret = HI_SUCCESS;
    HI_CHAR *pszString = NULL;
    HI_U32 u32IdxM = 0;
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

    HI_S32 s32Ret = HI_SUCCESS;
    HI_CHAR *pszString = NULL;
    HI_U32 u32IdxM = 0;
    HI_CHAR aszIniNodeName[SCENE_INIPARAM_NODE_NAME_LEN] = {0,};
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
#endif
    return HI_SUCCESS;
}

HI_S32 SCENE_LoadStaticAWBEX(const HI_CHAR *pszIniModule, HI_SCENE_STATIC_AWBEX_S *pstStaticAwbEx)
{
#if 0
    HI_S32 s32Ret = HI_SUCCESS;
    HI_CHAR *pszString = NULL;
    HI_S32 s32Value = 0;
    HI_CHAR aszIniNodeName[SCENE_INIPARAM_NODE_NAME_LEN] = {0,};

    snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "static_awbex:Tolerance");/*Tolerance*/
    s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
    SCENE_INIPARAM_CHECK_LOAD_RESULT(s32Ret, aszIniNodeName);
    if (HI_NULL != pszString)
    {
        free(pszString);
        pszString = HI_NULL;
        s32Ret = HI_CONFACCESS_GetInt(SCENE_INIPARAM, pszIniModule, "static_awbex:Tolerance", 0, &s32Value);
        if (HI_SUCCESS != s32Ret)
        {
            MLOGE("load static_awbex:Tolerance failed\n");
            return HI_FAILURE;
        }
        pstStaticAwbEx->u8Tolerance = (HI_U8)s32Value;
    }

    snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "static_awbex:OutThresh");/*OutThresh*/
    s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
    SCENE_INIPARAM_CHECK_LOAD_RESULT(s32Ret, aszIniNodeName);
    if (HI_NULL != pszString)
    {
        free(pszString);
        pszString = HI_NULL;
        s32Ret = HI_CONFACCESS_GetInt(SCENE_INIPARAM, pszIniModule, "static_awbex:OutThresh", 0, &s32Value);
        if (HI_SUCCESS != s32Ret)
        {
            MLOGE("load static_awbex:OutThresh failed\n");
            return HI_FAILURE;
        }
        pstStaticAwbEx->u32OutThresh = (HI_U32)s32Value;
    }

    snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "static_awbex:LowStop");/*LowStop*/
    s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
    SCENE_INIPARAM_CHECK_LOAD_RESULT(s32Ret, aszIniNodeName);
    if (HI_NULL != pszString)
    {
        free(pszString);
        pszString = HI_NULL;
        s32Ret = HI_CONFACCESS_GetInt(SCENE_INIPARAM, pszIniModule, "static_awbex:LowStop", 0, &s32Value);
        if (HI_SUCCESS != s32Ret)
        {
            MLOGE("load static_awbex:LowStop failed\n");
            return HI_FAILURE;
        }
        pstStaticAwbEx->u16LowStop = (HI_U16)s32Value;
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
        free(pszString);
        pszString = HI_NULL;
        s32Ret = HI_CONFACCESS_GetInt(SCENE_INIPARAM, pszIniModule, "static_awbex:HighStop", 0, &s32Value);
        if (HI_SUCCESS != s32Ret)
        {
            MLOGE("load static_awbex:HighStop failed\n");
            return HI_FAILURE;
        }
        pstStaticAwbEx->u16HighStop = (HI_U16)s32Value;
    }

    snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "static_awbex:MultiLightSourceEn");/*MultiLightSourceEn*/
    s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
    SCENE_INIPARAM_CHECK_LOAD_RESULT(s32Ret, aszIniNodeName);
    if (HI_NULL != pszString)
    {
        free(pszString);
        pszString = HI_NULL;
        s32Ret = HI_CONFACCESS_GetInt(SCENE_INIPARAM, pszIniModule, "static_awbex:MultiLightSourceEn", 0, &s32Value);
        if (HI_SUCCESS != s32Ret)
        {
            MLOGE("load static_awbex:MultiLightSourceEn failed\n");
            return HI_FAILURE;
        }
        pstStaticAwbEx->bMultiLightSourceEn = s32Value;
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
#endif
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

HI_S32 SCENE_LoadStaticCCM(const HI_CHAR *pszIniModule, HI_SCENE_STATIC_CCM_S *pstStaticCcm)
{
    HI_S32 s32Ret = HI_SUCCESS;
    HI_CHAR *pszString = NULL;
    HI_S32 s32Value = 0;
    HI_CHAR aszIniNodeName[SCENE_INIPARAM_NODE_NAME_LEN] = {0,};

    snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "static_ccm:ISOActEn");/*ISOActEn*/
    s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
    SCENE_INIPARAM_CHECK_LOAD_RESULT(s32Ret, aszIniNodeName);
    if (HI_NULL != pszString)
    {
        free(pszString);
        pszString = HI_NULL;
        s32Ret = HI_CONFACCESS_GetInt(SCENE_INIPARAM, pszIniModule, "static_ccm:ISOActEn", 0, &s32Value);
        if (HI_SUCCESS != s32Ret)
        {
            MLOGE("load static_ccm:ISOActEn failed\n");
            return HI_FAILURE;
        }
        pstStaticCcm->bISOActEn = (HI_BOOL)s32Value;
    }

    snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "static_ccm:TempActEn");/*TempActEn*/
    s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
    SCENE_INIPARAM_CHECK_LOAD_RESULT(s32Ret, aszIniNodeName);
    if (HI_NULL != pszString)
    {
        free(pszString);
        pszString = HI_NULL;
        s32Ret = HI_CONFACCESS_GetInt(SCENE_INIPARAM, pszIniModule, "static_ccm:TempActEn", 0, &s32Value);
        if (HI_SUCCESS != s32Ret)
        {
            MLOGE("load static_ccm:TempActEn failed\n");
            return HI_FAILURE;
        }
        pstStaticCcm->bTempActEn = (HI_BOOL)s32Value;
    }

    return HI_SUCCESS;
}


HI_S32 SCENE_LoadStaticLDCI(const HI_CHAR *pszIniModule, HI_SCENE_STATIC_LDCI_S *pstStaticLdci)
{
#if 0
    HI_S32 s32Ret = HI_SUCCESS;
    HI_CHAR *pszString = NULL;
    HI_S32 s32Value = 0;
    HI_CHAR aszIniNodeName[SCENE_INIPARAM_NODE_NAME_LEN] = {0,};
    snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "static_ldci:bEnable");/*bEnable*/
    s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
    SCENE_INIPARAM_CHECK_LOAD_RESULT(s32Ret, aszIniNodeName);
    if (HI_NULL != pszString)
    {
        free(pszString);
        pszString = HI_NULL;
        s32Ret = HI_CONFACCESS_GetInt(SCENE_INIPARAM, pszIniModule, "static_ldci:bEnable", 0, &s32Value);
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

    snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "static_ldci:ManualBlcCtrl");/*ManualBlcCtrl*/
    s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
    SCENE_INIPARAM_CHECK_LOAD_RESULT(s32Ret, aszIniNodeName);
    if (HI_NULL != pszString)
    {
        free(pszString);
        pszString = HI_NULL;
        s32Ret = HI_CONFACCESS_GetInt(SCENE_INIPARAM, pszIniModule, "static_ldci:ManualBlcCtrl", 0, &s32Value);
        if (HI_SUCCESS != s32Ret)
        {
            MLOGE("load static_ldci:ManualBlcCtrl failed\n");
            return HI_FAILURE;
        }
        pstStaticLdci->u16ManualBlcCtrl = (HI_U16)s32Value;
    }

    snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "static_ldci:ManualHePosSigma");/*ManualHePosSigma*/
    s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
    SCENE_INIPARAM_CHECK_LOAD_RESULT(s32Ret, aszIniNodeName);
    if (HI_NULL != pszString)
    {
        free(pszString);
        pszString = HI_NULL;
        s32Ret = HI_CONFACCESS_GetInt(SCENE_INIPARAM, pszIniModule, "static_ldci:ManualHePosSigma", 0, &s32Value);
        if (HI_SUCCESS != s32Ret)
        {
            MLOGE("load static_ldci:ManualHePosSigma failed\n");
            return HI_FAILURE;
        }
        pstStaticLdci->u8ManualHePosSigma = (HI_U16)s32Value;
    }

    snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "static_ldci:ManualHePosMean");/*ManualHePosMean*/
    s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
    SCENE_INIPARAM_CHECK_LOAD_RESULT(s32Ret, aszIniNodeName);
    if (HI_NULL != pszString)
    {
        free(pszString);
        pszString = HI_NULL;
        s32Ret = HI_CONFACCESS_GetInt(SCENE_INIPARAM, pszIniModule, "static_ldci:ManualHePosMean", 0, &s32Value);
        if (HI_SUCCESS != s32Ret)
        {
            MLOGE("load static_ldci:ManualHePosMean failed\n");
            return HI_FAILURE;
        }
        pstStaticLdci->u8ManualHePosMean = (HI_U8)s32Value;
    }

    snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "static_ldci:ManualHeNegSigma");/*ManualHeNegSigma*/
    s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
    SCENE_INIPARAM_CHECK_LOAD_RESULT(s32Ret, aszIniNodeName);
    if (HI_NULL != pszString)
    {
        free(pszString);
        pszString = HI_NULL;
        s32Ret = HI_CONFACCESS_GetInt(SCENE_INIPARAM, pszIniModule, "static_ldci:ManualHeNegSigma", 0, &s32Value);
        if (HI_SUCCESS != s32Ret)
        {
            MLOGE("load static_ldci:ManualHeNegSigma failed\n");
            return HI_FAILURE;
        }
        pstStaticLdci->u8ManualHeNegSigma = (HI_U8)s32Value;
    }

    snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "static_ldci:ManualHeNegWgt");/*ManualHeNegWgt*/
    s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
    SCENE_INIPARAM_CHECK_LOAD_RESULT(s32Ret, aszIniNodeName);
    if (HI_NULL != pszString)
    {
        free(pszString);
        pszString = HI_NULL;
        s32Ret = HI_CONFACCESS_GetInt(SCENE_INIPARAM, pszIniModule, "static_ldci:ManualHeNegWgt", 0, &s32Value);
        if (HI_SUCCESS != s32Ret)
        {
            MLOGE("load static_ldci:ManualHeNegWgt failed\n");
            return HI_FAILURE;
        }
        pstStaticLdci->u8ManualHeNegWgt = (HI_U8)s32Value;
    }

    snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "static_ldci:ManualHeNegMean");/*ManualHeNegMean */
    s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
    SCENE_INIPARAM_CHECK_LOAD_RESULT(s32Ret, aszIniNodeName);
    if (HI_NULL != pszString)
    {
        free(pszString);
        pszString = HI_NULL;
        s32Ret = HI_CONFACCESS_GetInt(SCENE_INIPARAM, pszIniModule, "static_ldci:ManualHeNegMean", 0, &s32Value);
        if (HI_SUCCESS != s32Ret)
        {
            MLOGE("load static_ldci:ManualHeNegMean failed\n");
            return HI_FAILURE;
        }
        pstStaticLdci->u8ManualHeNegMean = (HI_U8)s32Value;
    }
#endif
    return HI_SUCCESS;
}

HI_S32 SCENE_LoadStaticDrc(const HI_CHAR *pszIniModule, HI_SCENE_STATIC_DRC_S *pstStaticDrc)
{
    HI_S32 s32Ret = HI_SUCCESS;
    HI_CHAR *pszString = NULL;
    HI_U32 u32IdxM = 0;
    HI_S32 s32Value = 0;
    HI_CHAR aszIniNodeName[SCENE_INIPARAM_NODE_NAME_LEN] = {0,};

    snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "static_drc:bEnable");/*bEnable*/
    s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
    SCENE_INIPARAM_CHECK_LOAD_RESULT(s32Ret, aszIniNodeName);
    if (HI_NULL != pszString)
    {
        free(pszString);
        pszString = HI_NULL;
        s32Ret = HI_CONFACCESS_GetInt(SCENE_INIPARAM, pszIniModule, "static_drc:bEnable", 0, &s32Value);
        if (HI_SUCCESS != s32Ret)
        {
            MLOGE("load static_drc:bEnable failed\n");
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

    snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "static_drc:DRCColorCorrectionLut");/*DRCColorCorrectionLut*/
    s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
    SCENE_INIPARAM_CHECK_LOAD_RESULT(s32Ret, aszIniNodeName);
    if (HI_NULL != pszString)
    {
        SCENE_GetNumbersInOneLine(pszString);
        for (u32IdxM = 0; u32IdxM < HI_ISP_DRC_CC_NODE_NUM; u32IdxM++)
        {
            pstStaticDrc->au16ColorCorrectionLut[u32IdxM] = s_as64LineNum[u32IdxM];
        }
        free(pszString);
        pszString = HI_NULL;
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

    snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "static_dehaze:bEnable");/*bEnable*/
    s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
    SCENE_INIPARAM_CHECK_LOAD_RESULT(s32Ret, aszIniNodeName);
    if (HI_NULL != pszString)
    {
        free(pszString);
        pszString = HI_NULL;
        s32Ret = HI_CONFACCESS_GetInt(SCENE_INIPARAM, pszIniModule, "static_dehaze:bEnable", 0, &s32Value);
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

    snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "static_dehaze:bDehazeUserLutEnable");/*bDehazeUserLutEnable*/
    s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
    SCENE_INIPARAM_CHECK_LOAD_RESULT(s32Ret, aszIniNodeName);
    if (HI_NULL != pszString)
    {
        free(pszString);
        pszString = HI_NULL;
        s32Ret = HI_CONFACCESS_GetInt(SCENE_INIPARAM, pszIniModule, "static_dehaze:bDehazeUserLutEnable", 0, &s32Value);
        if (HI_SUCCESS != s32Ret)
        {
            MLOGE("load static_dehaze:bDehazeUserLutEnable failed\n");
            return HI_FAILURE;
        }
        pstStaticDeHaze->bUserLutEnable = (HI_BOOL)s32Value;
    }

    snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "static_dehaze:DehazeLut");/*bDehazeUserLutEnable*/
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

HI_S32 SCENE_LoadStaticStatistics(const HI_CHAR *pszIniModule, HI_SCENE_STATIC_STATISTICSCFG_S *pstStaticStatistics)
{
    HI_S32 s32Ret = HI_SUCCESS;
    HI_CHAR *pszString = NULL;
    HI_U32 u32IdxM = 0;
    HI_U32 u32IdxN = 0;
    HI_CHAR aszIniNodeName[SCENE_INIPARAM_NODE_NAME_LEN] = {0,};

    snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "static_statistics:ExpWeight_0");/*AEWeight*/
    s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
    SCENE_INIPARAM_CHECK_LOAD_RESULT(s32Ret, aszIniNodeName);
    if (HI_NULL != pszString)
    {
        free(pszString);
        pszString = HI_NULL;
        for (u32IdxM = 0; u32IdxM < AE_ZONE_ROW; u32IdxM++)
        {
            snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "static_statistics:ExpWeight_%d", u32IdxM);
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

HI_S32 SCENE_LoadStaticCSC(const HI_CHAR *pszIniModule, HI_SCENE_STATIC_CSC_S *pstStaticCSC)
{
#if 0
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

    HI_S32 s32Ret = HI_SUCCESS;
    HI_CHAR *pszString = NULL;
    HI_S32 s32Value = 0;
    HI_CHAR aszIniNodeName[SCENE_INIPARAM_NODE_NAME_LEN] = {0,};
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

    HI_S32 s32Ret = HI_SUCCESS;
    HI_CHAR *pszString = NULL;
    HI_S32 s32Value = 0;
    HI_CHAR aszIniNodeName[SCENE_INIPARAM_NODE_NAME_LEN] = {0,};
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

    HI_S32 s32Ret = HI_SUCCESS;
    HI_CHAR *pszString = NULL;
    HI_S32 s32Value = 0;
    HI_CHAR aszIniNodeName[SCENE_INIPARAM_NODE_NAME_LEN] = {0,};
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

    HI_S32 s32Ret = HI_SUCCESS;
    HI_CHAR *pszString = NULL;
    HI_S32 s32Value = 0;
    HI_CHAR aszIniNodeName[SCENE_INIPARAM_NODE_NAME_LEN] = {0,};
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

    HI_S32 s32Ret = HI_SUCCESS;
    HI_CHAR *pszString = NULL;
    HI_S32 s32Value = 0;
    HI_CHAR aszIniNodeName[SCENE_INIPARAM_NODE_NAME_LEN] = {0,};
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
#endif
    return HI_SUCCESS;
}

HI_S32 SCENE_LoadStaticShading(const HI_CHAR *pszIniModule, HI_SCENE_STATIC_SHADING_S *pstStaticShading)
{
    HI_S32 s32Ret = HI_SUCCESS;
    HI_CHAR *pszString = NULL;
    HI_S32 s32Value = 0;
    HI_CHAR aszIniNodeName[SCENE_INIPARAM_NODE_NAME_LEN] = {0,};

    snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "static_shading:bEnable");/*bEnable*/
    s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
    SCENE_INIPARAM_CHECK_LOAD_RESULT(s32Ret, aszIniNodeName);
    if (HI_NULL != pszString)
    {
        free(pszString);
        pszString = HI_NULL;
        s32Ret = HI_CONFACCESS_GetInt(SCENE_INIPARAM, pszIniModule, "static_shading:bEnable", 0, &s32Value);
        if (HI_SUCCESS != s32Ret)
        {
            MLOGE("load static_shading:bEnable failed\n");
            return HI_FAILURE;
        }
        pstStaticShading->bEnable = (HI_BOOL)s32Value;
    }

    return HI_SUCCESS;
}

static HI_S32 g_enGmc = NR_MOTION_MODE_NORMAL;

#if 0
static void PrintNRx(HI_SCENE_3DNR_VI_S *pVIX, HI_SCENE_3DNR_VPSS_S *pVPX)
{
    HI_SCENE_3DNR_VI_IEy  *vi_pi   = &( pVIX->IEy );
    HI_SCENE_3DNR_VI_SFy  *vi_ps   = &( pVIX->SFy );
    HI_SCENE_3DNR_VPSS_IEy  *pi = pVPX->IEy;
    HI_SCENE_3DNR_VPSS_SFy  *ps = pVPX->SFy;
    HI_SCENE_3DNR_VPSS_MDy  *pm = pVPX->MDy;
    HI_SCENE_3DNR_VPSS_TFy  *pt = pVPX->TFy;
    HI_SCENE_3DNR_VPSS_RFs  *pr = &pVPX->RFs;

#undef  _tmprt1_
#define _tmprt1_( ps,    X )  vi_ps->X, ps[0].X, ps[1].X, ps[2].X
#undef  _tmprt1y
#define _tmprt1y( ps,    X )  ps[0].X, ps[1].X
#undef  _tmprt1z
#define _tmprt1z( ps,a,b,X )  ps[a].X, ps[b].X

#undef  _tmprt2_
#define _tmprt2_( ps,    X,Y )  vi_ps->X, vi_ps->Y,  ps[0].X,ps[0].Y,  ps[1].X,ps[1].Y,  ps[2].X, ps[2].Y
#undef  _tmprt2y
#define _tmprt2y( ps,    X,Y )  vi_ps->X, vi_ps->Y,  ps[0].X,ps[0].Y,  ps[1].X,ps[1].Y
#undef  _tmprt2z
#define _tmprt2z( ps,a,b,X,Y )  ps[a].X,ps[a].Y,  ps[b].X,ps[b].Y

#undef  _t3_
#define _t3_( ie, n,   X,Y,Z )            ie[n].X, ie[n].Y, ie[n].Z
#undef  _t4_
#define _t4_( ie, n, K,X,Y,Z )   ie[n].K, ie[n].X, ie[n].Y, ie[n].Z

#undef  _tmprt3z
#define _tmprt3z( ps,a,b,X,Y,Z )  _t3_( ps,a,  X,Y,Z ),_t3_( ps,b,  X,Y,Z )
#undef  _tmprt3y
#define _tmprt3y( ps,    X,Y,Z )  _t3_( ps,0,  X,Y,Z ),_t3_( ps,1,  X,Y,Z ),_t3_( ps,2,  X,Y,Z )
#undef  _tmprt3x
#define _tmprt3x( ps,    X,Y,Z )  _t3_( ps,1,  X,Y,Z ),_t3_( ps,2,  X,Y,Z ),_t3_( ps,3,  X,Y,Z )
#undef  _tmprt3_
#define _tmprt3_( ps,    X,Y,Z )  _t3_( ps,0,  X,Y,Z ),_t3_( ps,1,  X,Y,Z ),_t3_( ps,2,  X,Y,Z ),_t3_( ps,3,  X,Y,Z )
#undef  _tmprt3x_
#define _tmprt3x_( ps,    X,Y,Z )  vi_ps->X, vi_ps->Y, vi_ps->Z ,_t3_( ps,0,  X,Y,Z ),_t3_( ps,1,  X,Y,Z ),_t3_( ps,2,  X,Y,Z )
#undef  _tmprt4_
#define _tmprt4_( ps,  K,X,Y,Z )  _t4_( ps,0,K,X,Y,Z ),_t4_( ps,1,K,X,Y,Z ),_t4_( ps,2,K,X,Y,Z ),_t4_( ps,3,K,X,Y,Z )
#undef  _tmprt4x_
#define _tmprt4x_( ps,  K,X,Y,Z )  vi_ps->K,vi_ps->X,vi_ps->Y,vi_ps->Z,_t4_( ps,0,K,X,Y,Z ),_t4_( ps,1,K,X,Y,Z ),_t4_( ps,2,K,X,Y,Z )

    printf("\n****************************NRy (n0sfx to n3sfx)******************************");
    printf("\n -en                    |             %3d |             %3d |             %3d ",
                                                  ps[0].NRyEn,      ps[1].NRyEn,     ps[2].NRyEn);
    printf("\n -nXsf1     %3d:%3d:%3d |     %3d:%3d:%3d |     %3d:%3d:%3d |     %3d:%3d:%3d ",
            vi_ps->SFS1, vi_ps->SFT1, vi_ps->SBR1, _tmprt3y( ps, SFS1, SFT1, SBR1 ));
    printf("\n -nXsf2     %3d:%3d:%3d |     %3d:%3d:%3d |     %3d:%3d:%3d |     %3d:%3d:%3d ",
            vi_ps->SFS2, vi_ps->SFT2, vi_ps->SBR2, _tmprt3y( ps, SFS2, SFT2, SBR2 ));
    printf("\n -nXsf4     %3d:%3d:%3d |     %3d:%3d:%3d |     %3d:%3d:%3d |     %3d:%3d:%3d ",
            vi_ps->SFS4, vi_ps->SFT4, vi_ps->SBR4, _tmprt3y( ps, SFS4, SFT4, SBR4 ));
    printf("\n -bwsf4             %3d |             %3d |                 |                 ", vi_ps->BWSF4, ps[0].BWSF4);
    printf("\n -kmsf4                 |                 |             %3d |             %3d ", ps[1].kMode,  ps[2].kMode);
    printf("\n -nXsf5 %3d:%3d:%3d:%3d |  %3d:%3d:%3d:%3d|  %3d:%3d:%3d:%3d| %3d:%3d:%3d:%3d ",  vi_pi->IES0, vi_pi->IES1, vi_pi->IES2, vi_pi->IES3 ,
                                                                                                                        _t4_( pi, 0, IES0, IES1, IES2, IES3),
                                                                                                                        _t4_( pi, 1, IES0, IES1, IES2, IES3),
                                                                                                                        _t4_( pi, 2, IES0, IES1, IES2, IES3));
    printf("\n -dzsf5             %3d |              %3d|              %3d|             %3d ",  vi_pi->IEDZ, pi[0].IEDZ, pi[1].IEDZ, pi[2].IEDZ);
    printf("\n -nXsf6 %3d:%3d:%3d:%3d | %3d:%3d:%3d:%3d | %3d:%3d:%3d:%3d | %3d:%3d:%3d:%3d ", _tmprt4x_( ps, SPN6, SBN6, PBR6, JMODE ));
    printf("\n -nXsfr6    %3d:%3d:%3d |     %3d:%3d:%3d |     %3d:%3d:%3d |     %3d:%3d:%3d ", _tmprt3x_( ps, SFR6[0], SFR6[1], SFR6[2]));
    printf("\n -SelRt         %3d:%3d |                 |                 |                 ", vi_ps->SRT0, vi_ps->SRT1);
    printf("\n -DeRt          %3d:%3d |                 |                 |                 ", vi_ps->DeRate, vi_ps->DeIdx);
    printf("\n                        |                 |                 |                 ");
    printf("\n                        |                 |                 |                 ");
    printf("\n -TriTh             %3d |             %3d |             %3d |             %3d ", vi_ps->TriTh, ps[0].TriTh, ps[1].TriTh, ps[2].TriTh);
    if ( ps->TriTh )
    {
        printf("\n -nXsfn %3d:%3d:%3d:%3d | %3d:%3d:%3d:%3d | %3d:%3d:%3d:%3d | %3d:%3d:%3d:%3d ", _tmprt4x_(ps, SFN0, SFN1, SFN2, SFN3));
        printf("\n -nXsth     %3d:%3d:%3d |     %3d:%3d:%3d |     %3d:%3d:%3d |     %3d:%3d:%3d ", _tmprt3x_(ps,       STH1, STH2, STH3));
    }
    else
    {
        printf("\n -nXsfn     %3d:%3d:%3d |     %3d:%3d:%3d |     %3d:%3d:%3d |     %3d:%3d:%3d ", _tmprt3x_(ps, SFN0, SFN1,  SFN3));
        printf("\n -nXsth         %3d:%3d |         %3d:%3d |         %3d:%3d |         %3d:%3d ", _tmprt2_(ps,        STH1,  STH3));
    }
    printf("\n -sfr    (0)        %3d |             %3d |             %3d |             %3d ", _tmprt1_( ps, SFR ));
    printf("\n                        |                 |                 |                 ");
    printf("\n -tedge                 |             %3d |             %3d |                 ", pt[0].tEdge, pt[1].tEdge);
    printf("\n                        |                 |                 |                 ");
    printf("\n -ref                   |         %3d:%3d |                 |                 ", pt[0].bRef, g_enGmc);
    printf("\n -refUpt                |             %3d |                 |                 ", pr->RFUI);
    printf("\n -rftIdx                |             %3d |                 |                 ", pt[0].RFI);
    printf("\n -refCtl                |         %3d:%3d |                 |                 ", pr->RFDZ, pr->RFSLP);
    printf("\n                        |                 |                 |                 ");
    printf("\n -biPath                |             %3d |             %3d |                 ", pm[0].biPath, pm[1].biPath);
    printf("\n -nXstr   (1)           |         %3d:%3d |         %3d:%3d |                 ", _tmprt2z(pt, 0, 1, STR0, STR1));
    printf("\n -nXsdz                 |         %3d:%3d |         %3d:%3d |                 ", _tmprt2z(pt, 0, 1, SDZ0, SDZ1));
    printf("\n                        |                 |                 |                 ");
    printf("\n -nXtss                 |         %3d:%3d |         %3d:%3d |                 ", _tmprt2z(pt, 0, 1, TSS0, TSS1));
    printf("\n -nXtsi                 |         %3d:%3d |         %3d:%3d |                 ", _tmprt2z(pt, 0, 1, TSI0, TSI1));
    printf("\n -nXtfs                 |         %3d:%3d |         %3d:%3d |                 ", _tmprt2z(pt, 0, 1, TFS0, TFS1));
	printf("\n -nXdzm                 |         %3d:%3d |         %3d:%3d |                 ", _tmprt2z(pt, 0, 1, DZMode0, DZMode1));
    printf("\n -nXtdz   (3)           |         %3d:%3d |         %3d:%3d |                 ", _tmprt2z(pt, 0, 1, TDZ0, TDZ1));
    printf("\n -nXtdx                 |         %3d:%3d |         %3d:%3d |                 ", _tmprt2z(pt, 0, 1, TDX0, TDX1));
    printf("\n                        |                 |                 |                 ");
    printf("\n -nXtfr0  (2)           |     %3d:%3d:%3d |     %3d:%3d:%3d |                 ", _tmprt3z(pt, 0, 1, TFR0[0], TFR0[1], TFR0[2]));
    printf("\n                        |     %3d:%3d:%3d |     %3d:%3d:%3d |                 ", _tmprt3z(pt, 0, 1, TFR0[3], TFR0[4], TFR0[5]));
    printf("\n -nXtfr1  (2)           |     %3d:%3d:%3d |     %3d:%3d:%3d |                 ", _tmprt3z(pt, 0, 1, TFR1[0], TFR1[1], TFR1[2]));
    printf("\n                        |     %3d:%3d:%3d |     %3d:%3d:%3d |                 ", _tmprt3z(pt, 0, 1, TFR1[3], TFR1[4], TFR1[5]));
    printf("\n                        |                 |                 |                 ");
    printf("\n -mXid0                 |     %3d:%3d:%3d |     %3d:%3d:%3d |                 ", _tmprt3z(pm, 0, 1, MAI00, MAI01, MAI02));
    printf("\n -mXid1                 |     %3d:%3d:%3d |     %3d:%3d:%3d |                 ", _tmprt3z(pm, 0, 1, MAI10, MAI11, MAI12));
    printf("\n -mXmadz                |         %3d:%3d |         %3d:%3d |                 ", pm[0].MADZ0, pm[0].MADZ1, pm[1].MADZ0, pm[1].MADZ1);
	printf("\n -mXmabr                |         %3d:%3d |         %3d:%3d |                 ", pm[0].MABR0, pm[0].MABR1, pm[1].MABR0, pm[1].MABR1);
    printf("\n                        |                 |                 +----------------+");
    printf("\n -AdvMath               |         %3d     |                 |  -sfc      %3d  ", pr->advMATH, pVPX->pNRc.SFC );
    printf("\n -mXmath                |         %3d:%3d |         %3d:%3d |                 ", _tmprt2z(pm, 0, 1, MATH0, MATH1));
    printf("\n                        |                 |                 |  -ctfs     %3d  ", pVPX->pNRc.CTFS);
    printf("\n                        |                 |                 |  -tfc      %3d  ", pVPX->pNRc.TFC);
    printf("\n -mXmate                |         %3d:%3d |         %3d:%3d |                 ", _tmprt2z(pm, 0, 1, MATE0, MATE1));
    printf("\n -mXmabw                |         %3d:%3d |         %3d:%3d |                 ", _tmprt2z(pm, 0, 1, MABW0, MABW1));
    printf("\n -mXmatw                |             %3d |             %3d |                 ",  pm[0].MATW, pm[1].MATW);
	printf("\n -mXmasw                |             %3d |             %3d |                 ",  pm[0].MASW, pm[1].MASW);
    printf("\n******************************************************************************\n");
    if ((pm[0].MADZ0 > 0) || (pm[0].MADZ1 > 0))
    {
        const HI_U8 *p = pm[0].MABR0;
        //printf("\n----------------------+-----------------+-----------------+-------------------");
        //printf("\n  m1mabr0 -m1r00    %6d %6d %6d %6d    %6d %6d %6d %6d ", p[0x00], p[0x01], p[0x02], p[0x03], p[0x04], p[0x05], p[0x06], p[0x07]);
        //printf("\n          -m1r08    %6d %6d %6d %6d    %6d %6d %6d %6d ", p[0x08], p[0x09], p[0x0A], p[0x0B], p[0x0C], p[0x0D], p[0x0E], p[0x0F]);
        p = pm[0].MABR1;
        //printf("\n");
        //printf("\n  m1mabr1 -m1r10    %6d %6d %6d %6d    %6d %6d %6d %6d ", p[0x00], p[0x01], p[0x02], p[0x03], p[0x04], p[0x05], p[0x06], p[0x07]);
        //printf("\n          -m1r18    %6d %6d %6d %6d    %6d %6d %6d %6d ", p[0x08], p[0x09], p[0x0A], p[0x0B], p[0x0C], p[0x0D], p[0x0E], p[0x0F]);
    }
    if ((pm[1].MADZ0 > 0) || (pm[1].MADZ1 > 0))
    {
        const HI_U8 *p = pm[1].MABR0;
        //printf("\n");
        //printf("\n  m2mabr0 -m2r10    %6d %6d %6d %6d    %6d %6d %6d %6d ", p[0x00], p[0x01], p[0x02], p[0x03], p[0x04], p[0x05], p[0x06], p[0x07]);
        //printf("\n          -m2r18    %6d %6d %6d %6d    %6d %6d %6d %6d ", p[0x08], p[0x09], p[0x0A], p[0x0B], p[0x0C], p[0x0D], p[0x0E], p[0x0F]);
        p = pm[1].MABR1;
        //printf("\n");
        //printf("\n  m2mabr1 -m2r10    %6d %6d %6d %6d    %6d %6d %6d %6d ", p[0x00], p[0x01], p[0x02], p[0x03], p[0x04], p[0x05], p[0x06], p[0x07]);
        //printf("\n          -m2r18    %6d %6d %6d %6d    %6d %6d %6d %6d ", p[0x08], p[0x09], p[0x0A], p[0x0B], p[0x0C], p[0x0D], p[0x0E], p[0x0F]);
    }

    if ((ps[1].kMode > 1) || (ps[2].kMode > 1))
    {
        const HI_U16 *p;
        printf("\n----------------------+-----------------+----------------+");

        if (ps[1].kMode > 1) {

            p = ps[1].SBSk;
            printf("\n");
            printf("\n  SBSk   -k2b0    %6d %6d %6d %6d    %6d %6d %6d %6d ", p[0x00], p[0x01], p[0x02], p[0x03], p[0x04], p[0x05], p[0x06], p[0x07]);
            printf("\n         -k2b8    %6d %6d %6d %6d    %6d %6d %6d %6d ", p[0x08], p[0x09], p[0x0A], p[0x0B], p[0x0C], p[0x0D], p[0x0E], p[0x0F]);
            printf("\n         -k2b16   %6d %6d %6d %6d    %6d %6d %6d %6d ", p[0x10], p[0x11], p[0x12], p[0x13], p[0x14], p[0x15], p[0x16], p[0x17]);
            printf("\n         -k2b24   %6d %6d %6d %6d    %6d %6d %6d %6d ", p[0x18], p[0x19], p[0x1A], p[0x1B], p[0x1C], p[0x1D], p[0x1E], p[0x1F]);

            p = ps[1].SDSk;
            printf("\n");
            printf("\n  SDSk   -k2d0    %6d %6d %6d %6d    %6d %6d %6d %6d ", p[0x00], p[0x01], p[0x02], p[0x03], p[0x04], p[0x05], p[0x06], p[0x07]);
            printf("\n         -k2d8    %6d %6d %6d %6d    %6d %6d %6d %6d ", p[0x08], p[0x09], p[0x0A], p[0x0B], p[0x0C], p[0x0D], p[0x0E], p[0x0F]);
            printf("\n         -k2d16   %6d %6d %6d %6d    %6d %6d %6d %6d ", p[0x10], p[0x11], p[0x12], p[0x13], p[0x14], p[0x15], p[0x16], p[0x17]);
            printf("\n         -k2d24   %6d %6d %6d %6d    %6d %6d %6d %6d ", p[0x18], p[0x19], p[0x1A], p[0x1B], p[0x1C], p[0x1D], p[0x1E], p[0x1F]);
        }

        if (ps[2].kMode > 1)
        {
            p = ps[2].SBSk;
            printf("\n");
            printf("\n  SBSk   -k3b0    %6d %6d %6d %6d    %6d %6d %6d %6d ", p[0x00], p[0x01], p[0x02], p[0x03], p[0x04], p[0x05], p[0x06], p[0x07]);
            printf("\n         -k3b8    %6d %6d %6d %6d    %6d %6d %6d %6d ", p[0x08], p[0x09], p[0x0A], p[0x0B], p[0x0C], p[0x0D], p[0x0E], p[0x0F]);
            printf("\n         -k3b16   %6d %6d %6d %6d    %6d %6d %6d %6d ", p[0x10], p[0x11], p[0x12], p[0x13], p[0x14], p[0x15], p[0x16], p[0x17]);
            printf("\n         -k3b24   %6d %6d %6d %6d    %6d %6d %6d %6d ", p[0x18], p[0x19], p[0x1A], p[0x1B], p[0x1C], p[0x1D], p[0x1E], p[0x1F]);

            p = ps[2].SDSk;
            printf("\n");
            printf("\n  SDSk   -k3d0    %6d %6d %6d %6d    %6d %6d %6d %6d ", p[0x00], p[0x01], p[0x02], p[0x03], p[0x04], p[0x05], p[0x06], p[0x07]);
            printf("\n         -k3d8    %6d %6d %6d %6d    %6d %6d %6d %6d ", p[0x08], p[0x09], p[0x0A], p[0x0B], p[0x0C], p[0x0D], p[0x0E], p[0x0F]);
            printf("\n         -k3d16   %6d %6d %6d %6d    %6d %6d %6d %6d ", p[0x10], p[0x11], p[0x12], p[0x13], p[0x14], p[0x15], p[0x16], p[0x17]);
            printf("\n         -k3d24   %6d %6d %6d %6d    %6d %6d %6d %6d ", p[0x18], p[0x19], p[0x1A], p[0x1B], p[0x1C], p[0x1D], p[0x1E], p[0x1F]);
        }
    }

    printf("\n******************************************************************************\n");
}
#endif
HI_S32 SCENE_LoadDynamicThreeDNR(const HI_CHAR *pszIniModule, HI_SCENE_DYNAMIC_THREEDNR_S *pstDynamicThreeDnr)
{
    HI_S32 s32Ret = HI_SUCCESS;
    HI_CHAR *pszString = NULL;
    HI_U32 u32IdxM = 0;
    HI_S32 s32Value = 0;
    HI_CHAR aszIniNodeName[SCENE_INIPARAM_NODE_NAME_LEN] = {0,};

    snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "dynamic_threednr:ThreeDNRCount");/*ThreeDNRCount*/
    s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
    SCENE_INIPARAM_CHECK_LOAD_RESULT(s32Ret, aszIniNodeName);
    if (HI_NULL != pszString)
    {
        free(pszString);
        pszString = HI_NULL;
        s32Ret = HI_CONFACCESS_GetInt(SCENE_INIPARAM, pszIniModule, "dynamic_threednr:ThreeDNRCount", 0, &s32Value);
        if (HI_SUCCESS != s32Ret)
        {
            MLOGE("load dynamic_threednr:ThreeDNRCount failed\n");
            return HI_FAILURE;
        }
        pstDynamicThreeDnr->u32ThreeDNRCount = (HI_U32)s32Value;
    }

    snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "dynamic_threednr:VI_3DNRStartPoint");/*VI_3DNRStartPoint*/
    s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
    SCENE_INIPARAM_CHECK_LOAD_RESULT(s32Ret, aszIniNodeName);
    if (HI_NULL != pszString)
    {
        free(pszString);
        pszString = HI_NULL;
        s32Ret = HI_CONFACCESS_GetInt(SCENE_INIPARAM, pszIniModule, "dynamic_threednr:VI_3DNRStartPoint", 0, &s32Value);
        if (HI_SUCCESS != s32Ret)
        {
            MLOGE("load dynamic_threednr:VI_3DNRStartPoint failed\n");
            return HI_FAILURE;
        }
        pstDynamicThreeDnr->u16VI_3DNRStartPoint = (HI_U32)s32Value;
    }

    snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "dynamic_threednr:IsoThresh");/*IsoThresh*/
    s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
    SCENE_INIPARAM_CHECK_LOAD_RESULT(s32Ret, aszIniNodeName);
    if (HI_NULL != pszString)
    {
        SCENE_GetNumbersInOneLine(pszString);
        for (u32IdxM = 0; u32IdxM < pstDynamicThreeDnr->u32ThreeDNRCount; u32IdxM++)
        {
            pstDynamicThreeDnr->au32ThreeDNRIso[u32IdxM] = s_as64LineNum[u32IdxM];
        }
        free(pszString);
        pszString = HI_NULL;
    }

    snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "dynamic_threednr:3DnrParam_0");/*3DnrParam_0*/
    s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
    SCENE_INIPARAM_CHECK_LOAD_RESULT(s32Ret, aszIniNodeName);
    if (HI_NULL != pszString)
    {
        free(pszString);
        pszString = HI_NULL;
        for (u32IdxM = 0; u32IdxM < pstDynamicThreeDnr->u32ThreeDNRCount; u32IdxM++)
        {
            snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "dynamic_threednr:3DnrParam_%d", u32IdxM);
            s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
            if (HI_SUCCESS != s32Ret)
            {
                MLOGE("load %s failed\n", aszIniNodeName);
                return HI_FAILURE;
            }
            if (NULL != pszString)
            {
                HI_SCENE_3DNR_VI_S *pVIX      = &(pstDynamicThreeDnr->astThreeDNRVIValue[u32IdxM]);
                HI_SCENE_3DNR_VPSS_S *pVPX    = &(pstDynamicThreeDnr->astThreeDNRVPSSValue[u32IdxM]);
                HI_SCENE_3DNR_VI_IEy *pViI    = &(pVIX->IEy);
                HI_SCENE_3DNR_VI_SFy *pViS    = &(pVIX->SFy);
                HI_SCENE_3DNR_VPSS_IEy  *pVpI = pVPX->IEy;
                HI_SCENE_3DNR_VPSS_SFy  *pVpS = pVPX->SFy;
                HI_SCENE_3DNR_VPSS_MDy  *pVpM = pVPX->MDy;
                HI_SCENE_3DNR_VPSS_TFy  *pVpT = pVPX->TFy;
                HI_SCENE_3DNR_VPSS_RFs  *pVpR = &pVPX->RFs;

                sscanf(pszString,
                       " -en                    |             %3d |             %3d |             %3d "
                       " -nXsf1     %3d:%3d:%3d |     %3d:%3d:%3d |     %3d:%3d:%3d |     %3d:%3d:%3d "
                       " -nXsf2     %3d:%3d:%3d |     %3d:%3d:%3d |     %3d:%3d:%3d |     %3d:%3d:%3d "
                       " -nXsf4     %3d:%3d:%3d |     %3d:%3d:%3d |     %3d:%3d:%3d |     %3d:%3d:%3d "
                       " -bwsf4             %3d |             %3d |                 |                 "
                       " -kmsf4                 |                 |             %3d |             %3d "
                       " -nXsf5 %3d:%3d:%3d:%3d | %3d:%3d:%3d:%3d | %3d:%3d:%3d:%3d | %3d:%3d:%3d:%3d "
                       " -dzsf5             %3d |             %3d |             %3d |             %4d "
                       " -nXsf6 %3d:%3d:%3d:%3d | %3d:%3d:%3d:%3d | %3d:%3d:%3d:%3d | %3d:%3d:%3d:%3d "
                       " -nXsfr6    %3d:%3d:%3d |     %3d:%3d:%3d |     %3d:%3d:%3d |     %3d:%3d:%3d "
                       "                        |                 |                 |                 "
                       " -SelRt         %3d:%3d |                 |                 |                 "
                       " -DeRt          %3d:%3d |                 |                 |                 "
                       "                        |                 |                 |                 "
                       " -TriTh             %3d |             %3d |             %3d |             %3d "
                       " -nXsfn     %3d:%3d:%3d |     %3d:%3d:%3d |     %3d:%3d:%3d |     %3d:%3d:%3d "
                       " -nXsth         %3d:%3d |         %3d:%3d |         %3d:%3d |         %3d:%3d "
                       " -sfr    (0)        %3d |             %3d |             %3d |             %3d "
                       "                        |                 |                 |                 "
                       " -tedge                 |             %3d |             %3d |                 "
                       "                        |                 |                 |                 "
                       " -ref                   |         %3d:%3d |                 |                 "
                       " -refUpt                |             %3d |                 |                 "
                       " -rftIdx                |             %3d |                 |                 "
                       " -refCtl                |         %3d:%3d |                 |                 "
                       "                        |                 |                 |                 "
                       " -biPath                |             %3d |             %3d |                 "
                       " -nXstr   (1)           |         %3d:%3d |         %3d:%3d |                 "
                       " -nXsdz                 |         %3d:%3d |         %3d:%3d |                 "
                       "                        |                 |                 |                 "
                       " -nXtss                 |         %3d:%3d |         %3d:%3d |                 "
                       " -nXtsi                 |         %3d:%3d |         %3d:%3d |                 "
                       " -nXtfs                 |         %3d:%3d |         %3d:%3d |                 "
                       " -nXdzm                 |         %3d:%3d |         %3d:%3d |                 "
                       " -nXtdz   (3)           |         %3d:%3d |         %3d:%3d |                 "
                       " -nXtdx                 |         %3d:%3d |         %3d:%3d |                 "
                       "                        |                 |                 |                 "
                       " -nXtfr0  (2)           |     %3d:%3d:%3d |     %3d:%3d:%3d |                 "
                       "                        |     %3d:%3d:%3d |     %3d:%3d:%3d |                 "
                       " -nXtfr1  (2)           |     %3d:%3d:%3d |     %3d:%3d:%3d |                 "
                       "                        |     %3d:%3d:%3d |     %3d:%3d:%3d |                 "
                       "                        |                 |                 |                 "
                       " -mXid0                 |     %3d:%3d:%3d |     %3d:%3d:%3d |                 "
                       " -mXid1                 |     %3d:%3d:%3d |     %3d:%3d:%3d |                 "
                       " -mXmadz                |         %3d:%3d |         %3d:%3d |                 "
                       " -mXmabr                |         %3d:%3d |         %3d:%3d |                 "
                       "                        |                 |                 +------pNRc------+"
                       " -AdvMath               |         %3d     |                 |  -sfc      %3d  "
                       " -mXmath                |         %3d:%3d |         %3d:%3d |                 "
                       "                        |                 |                 |  -ctfs     %3d  "
                       "                        |                 |                 |  -tfc      %3d  "
                       " -mXmate                |         %3d:%3d |         %3d:%3d |                 "
                       " -mXmabw                |         %3d:%3d |         %3d:%3d |                 "
                       " -mXmatw                |             %3d |             %3d |                 "
                       " -mXmasw                |             %3d |             %3d |                 ",
                       &pVpS[0].NRyEn, &pVpS[1].NRyEn, &pVpS[2].NRyEn,
                       &pViS->SFS1, &pViS->SFT1, &pViS->SBR1, &pVpS[0].SFS1, &pVpS[0].SFT1, &pVpS[0].SBR1, &pVpS[1].SFS1, &pVpS[1].SFT1, &pVpS[1].SBR1, &pVpS[2].SFS1, &pVpS[2].SFT1, &pVpS[2].SBR1,
                       &pViS->SFS2, &pViS->SFT2, &pViS->SBR2, &pVpS[0].SFS2, &pVpS[0].SFT2, &pVpS[0].SBR2, &pVpS[1].SFS2, &pVpS[1].SFT2, &pVpS[1].SBR2, &pVpS[2].SFS2, &pVpS[2].SFT2, &pVpS[2].SBR2,
                       &pViS->SFS4, &pViS->SFT4, &pViS->SBR4, &pVpS[0].SFS4, &pVpS[0].SFT4, &pVpS[0].SBR4, &pVpS[1].SFS4, &pVpS[1].SFT4, &pVpS[1].SBR4, &pVpS[2].SFS4, &pVpS[2].SFT4, &pVpS[2].SBR4,
                       &pViS->BWSF4, &pVpS[0].BWSF4,
                       &pVpS[1].kMode, &pVpS[2].kMode,
                       &pViI->IES0, &pViI->IES1, &pViI->IES2, &pViI->IES3, &pVpI[0].IES0, &pVpI[0].IES1, &pVpI[0].IES2, &pVpI[0].IES3, &pVpI[1].IES0, &pVpI[1].IES1, &pVpI[1].IES2, &pVpI[1].IES3, &pVpI[2].IES0, &pVpI[2].IES1, &pVpI[2].IES2, &pVpI[2].IES3,
                       &pViI->IEDZ, &pVpI[0].IEDZ, &pVpI[1].IEDZ, &pVpI[2].IEDZ,
                       &pViS->SPN6, &pViS->SBN6, &pViS->PBR6, &pViS->JMODE, &pVpS[0].SPN6, &pVpS[0].SBN6, &pVpS[0].PBR6, &pVpS[0].JMODE, &pVpS[1].SPN6, &pVpS[1].SBN6, &pVpS[1].PBR6, &pVpS[1].JMODE, &pVpS[2].SPN6, &pVpS[2].SBN6, &pVpS[2].PBR6, &pVpS[2].JMODE,
                       &pViS->SFR6[0], &pViS->SFR6[1], &pViS->SFR6[2], &pVpS[0].SFR6[0], &pVpS[0].SFR6[1], &pVpS[0].SFR6[2], &pVpS[1].SFR6[0], &pVpS[1].SFR6[1], &pVpS[1].SFR6[2], &pVpS[2].SFR6[0], &pVpS[2].SFR6[1], &pVpS[2].SFR6[2],
                       &pViS->SRT0, &pViS->SRT1,
                       &pViS->DeRate, &pViS->DeIdx,
                       &pViS->TriTh, &pVpS[0].TriTh, &pVpS[1].TriTh, &pVpS[2].TriTh,
                       &pViS->SFN0, &pViS->SFN1, &pViS->SFN3, &pVpS[0].SFN0, &pVpS[0].SFN1, &pVpS[0].SFN3, &pVpS[1].SFN0, &pVpS[1].SFN1, &pVpS[1].SFN3, &pVpS[2].SFN0, &pVpS[2].SFN1, &pVpS[2].SFN3,
                       &pViS->STH1, &pViS->STH3, &pVpS[0].STH1, &pVpS[0].STH3, &pVpS[1].STH1, &pVpS[1].STH3, &pVpS[2].STH1, &pVpS[2].STH3,
                       &pViS->SFR, &pVpS[0].SFR, &pVpS[1].SFR, &pVpS[2].SFR,
                       &pVpT[0].tEdge, &pVpT[1].tEdge,
                       &pVpT[0].bRef, &g_enGmc,
                       &pVpR[0].RFUI,
                       &pVpT[0].RFI,
                       &pVpR[0].RFDZ, &pVpR[0].RFSLP,
                       &pVpM[0].biPath, &pVpM[1].biPath,
                       &pVpT[0].STR0, &pVpT[0].STR1, &pVpT[1].STR0, &pVpT[1].STR1,
                       &pVpT[0].SDZ0, &pVpT[0].SDZ1, &pVpT[1].SDZ0, &pVpT[1].SDZ1,
                       &pVpT[0].TSS0, &pVpT[0].TSS1, &pVpT[1].TSS0, &pVpT[1].TSS1,
                       &pVpT[0].TSI0, &pVpT[0].TSI1, &pVpT[1].TSI0, &pVpT[1].TSI1,
                       &pVpT[0].TFS0, &pVpT[0].TFS1, &pVpT[1].TFS0, &pVpT[1].TFS1,
                       &pVpT[0].DZMode0, &pVpT[0].DZMode1, &pVpT[1].DZMode0, &pVpT[1].DZMode1,
                       &pVpT[0].TDZ0, &pVpT[0].TDZ1, &pVpT[1].TDZ0, &pVpT[1].TDZ1,
                       &pVpT[0].TDX0, &pVpT[0].TDX1, &pVpT[1].TDX0, &pVpT[1].TDX1,
                       &pVpT[0].TFR0[0], &pVpT[0].TFR0[1], &pVpT[0].TFR0[2], &pVpT[1].TFR0[0], &pVpT[1].TFR0[1], &pVpT[1].TFR0[2],
                       &pVpT[0].TFR0[3], &pVpT[0].TFR0[4], &pVpT[0].TFR0[5], &pVpT[1].TFR0[3], &pVpT[1].TFR0[4], &pVpT[1].TFR0[5],
                       &pVpT[0].TFR1[0], &pVpT[0].TFR1[1], &pVpT[0].TFR1[2], &pVpT[1].TFR1[0], &pVpT[1].TFR1[1], &pVpT[1].TFR1[2],
                       &pVpT[0].TFR1[3], &pVpT[0].TFR1[4], &pVpT[0].TFR1[5], &pVpT[1].TFR1[3], &pVpT[1].TFR1[4], &pVpT[1].TFR1[5],
                       &pVpM[0].MAI00, &pVpM[0].MAI01, &pVpM[0].MAI02, &pVpM[1].MAI00, &pVpM[1].MAI01, &pVpM[1].MAI02,
                       &pVpM[0].MAI10, &pVpM[0].MAI11, &pVpM[0].MAI12, &pVpM[1].MAI10, &pVpM[1].MAI11, &pVpM[1].MAI12,
                       &pVpM[0].MADZ0, &pVpM[0].MADZ1, &pVpM[1].MADZ0, &pVpM[1].MADZ1,
                       &pVpM[0].MABR0, &pVpM[0].MABR1, &pVpM[1].MABR0, &pVpM[1].MABR1,
                       &pVpR[0].advMATH, &pVPX->pNRc.SFC,
                       &pVpM[0].MATH0, &pVpM[0].MATH1, &pVpM[1].MATH0, &pVpM[1].MATH1,
                       &pVPX->pNRc.CTFS,
                       &pVPX->pNRc.TFC,
                       &pVpM[0].MATE0, &pVpM[0].MATE1, &pVpM[1].MATE0, &pVpM[1].MATE1,
                       &pVpM[0].MABW0, &pVpM[0].MABW1, &pVpM[1].MABW0, &pVpM[1].MABW1,
                       &pVpM[0].MATW, &pVpM[1].MATW,
                       &pVpM[0].MASW, &pVpM[1].MASW
                      );
                free(pszString);
                pszString = HI_NULL;

                #if 0
                    printf("ThreeDNRIso = %d, u32IdxM = %d\n", pstDynamicThreeDnr->au32ThreeDNRIso[u32IdxM], u32IdxM);
                    PrintNRx(pVIX, pVPX);
                #endif
            }
        }
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

    snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "dynamic_fswdr:ISOModeChange");/*ISOModeChange*/
    s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
    SCENE_INIPARAM_CHECK_LOAD_RESULT(s32Ret, aszIniNodeName);
    if (HI_NULL != pszString)
    {
        SCENE_GetNumbersInOneLine(pszString);
        for (u32IdxM = 0; u32IdxM < pstDynamicFsWdr->u32IsoCnt; u32IdxM++)
        {
            pstDynamicFsWdr->au32ISOModeChange[u32IdxM] = s_as64LineNum[u32IdxM];
        }
        free(pszString);
        pszString = HI_NULL;
    }

        snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "dynamic_fswdr:MergeMode");/*MergeMode*/
        s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
        SCENE_INIPARAM_CHECK_LOAD_RESULT(s32Ret, aszIniNodeName);
        if (HI_NULL != pszString)
        {
            SCENE_GetNumbersInOneLine(pszString);
            for (u32IdxM = 0; u32IdxM < pstDynamicFsWdr->u32IsoCnt; u32IdxM++)
            {
                pstDynamicFsWdr->au8MergeMode[u32IdxM] = s_as64LineNum[u32IdxM];
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

HI_S32 SCENE_LoadStaticSharpen(const HI_CHAR *pszIniModule, HI_SCENE_STATIC_SHARPEN_S *pstStaticSharpen)
{
    HI_S32 s32Ret = HI_SUCCESS;
    HI_CHAR *pszString = NULL;
    HI_U32 i = 0, u32IdxN = 0;
    HI_CHAR aszIniNodeName[SCENE_INIPARAM_NODE_NAME_LEN] = {0,};

    snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "static_sharpen:OverShoot");/*OverShoot*/
    s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
    SCENE_INIPARAM_CHECK_LOAD_RESULT(s32Ret, aszIniNodeName);
    if (HI_NULL != pszString)
    {
        SCENE_GetNumbersInOneLine(pszString);
        for (i = 0; i < ISP_AUTO_ISO_STRENGTH_NUM; i++)
        {
            pstStaticSharpen->au8OverShoot[i] = s_as64LineNum[i];
        }
        free(pszString);
        pszString = HI_NULL;
    }

    snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "static_sharpen:UnderShoot");/*UnderShoot*/
    s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
    SCENE_INIPARAM_CHECK_LOAD_RESULT(s32Ret, aszIniNodeName);
    if (HI_NULL != pszString)
    {
        SCENE_GetNumbersInOneLine(pszString);
        for (i = 0; i < ISP_AUTO_ISO_STRENGTH_NUM; i++)
        {
            pstStaticSharpen->au8UnderShoot[i] = s_as64LineNum[i];

        }
        free(pszString);
        pszString = HI_NULL;
    }

    snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "static_sharpen:TextureStrTable0");/*TextureStrTable0*/
    s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
    SCENE_INIPARAM_CHECK_LOAD_RESULT(s32Ret, aszIniNodeName);
    if (HI_NULL != pszString)
    {
        free(pszString);
        pszString = HI_NULL;
        for (i = 0; i < ISP_AUTO_ISO_STRENGTH_NUM; i++)
        {
            snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "static_sharpen:TextureStrTable%d", i);
            s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
            if (HI_SUCCESS != s32Ret)
            {
                MLOGE("load %s failed\n", aszIniNodeName);
                return HI_FAILURE;
            }
            if (NULL != pszString)
            {
                SCENE_GetNumbersInOneLine(pszString);
                for (u32IdxN = 0; u32IdxN < ISP_SHARPEN_GAIN_NUM; u32IdxN++)
                {
                    pstStaticSharpen->au16TextureStr[i][u32IdxN] = (HI_U16)s_as64LineNum[u32IdxN];
                }
                free(pszString);
                pszString = HI_NULL;
            }
        }
    }

    snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "static_sharpen:EdgeStrTable0");/*EdgeStrTable0*/
    s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
    SCENE_INIPARAM_CHECK_LOAD_RESULT(s32Ret, aszIniNodeName);
    if (HI_NULL != pszString)
    {
        free(pszString);
        pszString = HI_NULL;
        for (i = 0; i < ISP_AUTO_ISO_STRENGTH_NUM; i++)
        {
            snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "static_sharpen:EdgeStrTable%d", i);
            s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
            if (HI_SUCCESS != s32Ret)
            {
                MLOGE("load %s failed\n", aszIniNodeName);
                return HI_FAILURE;
            }
            if (NULL != pszString)
            {
                SCENE_GetNumbersInOneLine(pszString);
                for (u32IdxN = 0; u32IdxN < ISP_SHARPEN_GAIN_NUM; u32IdxN++)
                {
                    pstStaticSharpen->au16EdgeStr[i][u32IdxN] = (HI_U16)s_as64LineNum[u32IdxN];
                }
                free(pszString);
                pszString = HI_NULL;
            }
        }
    }

    return HI_SUCCESS;
}

HI_S32 SCENE_LoadStaticDemosaic(const HI_CHAR *pszIniModule, HI_SCENE_STATIC_DEMOSAIC_S *pstStaticDemosaic)
{
    HI_S32 s32Ret = HI_SUCCESS;
    HI_CHAR *pszString = NULL;
    HI_U32 i = 0;
    HI_CHAR aszIniNodeName[SCENE_INIPARAM_NODE_NAME_LEN] = {0,};

    snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "static_demosaic:NonDirMFDetailEhcStr");/*NonDirMFDetailEhcStr*/
    s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
    SCENE_INIPARAM_CHECK_LOAD_RESULT(s32Ret, aszIniNodeName);
    if (HI_NULL != pszString)
    {
        SCENE_GetNumbersInOneLine(pszString);
        for (i = 0; i < ISP_AUTO_ISO_STRENGTH_NUM; i++)
        {
            pstStaticDemosaic->au8NonDirMFDetailEhcStr[i] = s_as64LineNum[i];
        }
        free(pszString);
        pszString = HI_NULL;
    }

    return HI_SUCCESS;
}

HI_S32 SCENE_LoadDynamicBayernr(const HI_CHAR *pszIniModule, HI_SCENE_DYNAMIC_BAYERNR_S *pstDynamicBayernr)
{
    HI_S32 s32Ret = HI_SUCCESS;
    HI_CHAR *pszString = NULL;
    HI_S32 s32Value = 0;
    HI_U32 u32IdxM = 0, u32IdxN = 0;
    HI_CHAR aszIniNodeName[SCENE_INIPARAM_NODE_NAME_LEN] = {0,};

    snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "dynamic_bayernr:IsoCount");/*IsoCount*/
    s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
    SCENE_INIPARAM_CHECK_LOAD_RESULT(s32Ret, aszIniNodeName);
    if (HI_NULL != pszString)
    {
        free(pszString);
        pszString = HI_NULL;
        s32Ret = HI_CONFACCESS_GetInt(SCENE_INIPARAM, pszIniModule, "dynamic_bayernr:IsoCount", 0, &s32Value);
        if (HI_SUCCESS != s32Ret)
        {
            MLOGE("load dynamic_bayernr: IsoCount failed\n");
            return HI_FAILURE;
        }
        pstDynamicBayernr->u32IsoCount = (HI_U32)s32Value;
    }

    snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "dynamic_bayernr:IsoLevel");/*IsoLevel*/
    s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
    SCENE_INIPARAM_CHECK_LOAD_RESULT(s32Ret, aszIniNodeName);
    if (HI_NULL != pszString)
    {
        SCENE_GetNumbersInOneLine(pszString);
        for (u32IdxM = 0; u32IdxM < pstDynamicBayernr->u32IsoCount; u32IdxM++)
        {
            pstDynamicBayernr->au32IsoLevel[u32IdxM] = s_as64LineNum[u32IdxM];
        }
        free(pszString);
        pszString = HI_NULL;
    }

        snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "dynamic_bayernr:CoringWgt");/*CoringWgt*/
        s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
        SCENE_INIPARAM_CHECK_LOAD_RESULT(s32Ret, aszIniNodeName);
        if (HI_NULL != pszString)
        {
            SCENE_GetNumbersInOneLine(pszString);
            for (u32IdxM = 0; u32IdxM < ISP_AUTO_ISO_STRENGTH_NUM; u32IdxM++)
            {
                pstDynamicBayernr->au16CoringWgt[u32IdxM] = s_as64LineNum[u32IdxM];
            }
            free(pszString);
            pszString = HI_NULL;
        }

        snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "dynamic_bayernr:FineStr");/*FineStr*/
        s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
        SCENE_INIPARAM_CHECK_LOAD_RESULT(s32Ret, aszIniNodeName);
        if (HI_NULL != pszString)
        {
            SCENE_GetNumbersInOneLine(pszString);
            for (u32IdxM = 0; u32IdxM < ISP_AUTO_ISO_STRENGTH_NUM; u32IdxM++)
            {
                pstDynamicBayernr->au8FineStr[u32IdxM] = s_as64LineNum[u32IdxM];
            }
            free(pszString);
            pszString = HI_NULL;
        }

        snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "dynamic_bayernr:CoringRatioTable0");/*CoringRatioTable0*/
        s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
        SCENE_INIPARAM_CHECK_LOAD_RESULT(s32Ret, aszIniNodeName);
        if (HI_NULL != pszString)
        {
            free(pszString);
            pszString = HI_NULL;
            for (u32IdxM = 0; u32IdxM < pstDynamicBayernr->u32IsoCount; u32IdxM++)
            {
                snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "dynamic_bayernr:CoringRatioTable%d", u32IdxM);
                s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
                if (HI_SUCCESS != s32Ret)
                {
                    MLOGE("load %s failed\n", aszIniNodeName);
                    return HI_FAILURE;
                }
                if (NULL != pszString)
                {
                    SCENE_GetNumbersInOneLine(pszString);
                    for (u32IdxN = 0; u32IdxN < HI_ISP_BAYERNR_LUT_LENGTH; u32IdxN++)
                    {
                        pstDynamicBayernr->au16CoringRatio[u32IdxM][u32IdxN] = (HI_U16)s_as64LineNum[u32IdxN];
                    }
                    free(pszString);
                    pszString = HI_NULL;
                }
            }
        }

    return HI_SUCCESS;
}

HI_S32 SCENE_LoadStaticDetail(const HI_CHAR *pszIniModule, HI_SCENE_STATIC_DETAIL_S *pstStaticDetail)
{
    HI_S32 s32Ret = HI_SUCCESS;
    HI_CHAR *pszString = NULL;
    HI_U32 i = 0;
    HI_CHAR aszIniNodeName[SCENE_INIPARAM_NODE_NAME_LEN] = {0,};

    snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "static_detail:GlobalGain");/*GlobalGain*/
    s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
    SCENE_INIPARAM_CHECK_LOAD_RESULT(s32Ret, aszIniNodeName);
    if (HI_NULL != pszString)
    {
        SCENE_GetNumbersInOneLine(pszString);
        for (i = 0; i < ISP_AUTO_ISO_STRENGTH_NUM; i++)
        {
            pstStaticDetail->au16GlobalGain[i] = s_as64LineNum[i];
        }
        free(pszString);
        pszString = HI_NULL;
    }

    return HI_SUCCESS;
}

HI_S32 SCENE_LoadDynamicDIS(const HI_CHAR *pszIniModule, HI_SCENE_DYNAMIC_DIS_S *pstDynamicDis)
{
#if 0
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
#endif
    return HI_SUCCESS;
}

HI_S32 SCENE_LoadDynamicLinearDrc(const HI_CHAR *pszIniModule, HI_SCENE_DYNAMIC_LINEARDRC_S *pstDynamicLinearDrc)
{
    HI_S32 s32Ret = HI_SUCCESS;
    HI_CHAR *pszString = NULL;
    HI_S32 s32Value = 0;
    HI_U32 u32IdxM = 0;
    HI_CHAR aszIniNodeName[SCENE_INIPARAM_NODE_NAME_LEN] = {0,};

    snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "linear_drc:Enable");/*Enable*/
    s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
    SCENE_INIPARAM_CHECK_LOAD_RESULT(s32Ret, aszIniNodeName);
    if (HI_NULL != pszString)
    {
        free(pszString);
        pszString = HI_NULL;
        s32Ret = HI_CONFACCESS_GetInt(SCENE_INIPARAM, pszIniModule, "linear_drc:Enable", 0, &s32Value);
        if (HI_SUCCESS != s32Ret)
        {
            MLOGE("load linear_drc:bEnable failed\n");
            return HI_FAILURE;
        }
        pstDynamicLinearDrc->bEnable = (HI_BOOL)s32Value;
    }

    snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "linear_drc:IsoCnt");  /*IsoCnt*/
    s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
    SCENE_INIPARAM_CHECK_LOAD_RESULT(s32Ret, aszIniNodeName);
    if (HI_NULL != pszString)
    {
        free(pszString);
        pszString = HI_NULL;
        s32Ret = HI_CONFACCESS_GetInt(SCENE_INIPARAM, pszIniModule, "linear_drc:IsoCnt", 0, &s32Value);
        if (HI_SUCCESS != s32Ret)
        {
            MLOGE("load linear_drc:IsoCnt failed\n");
            return HI_FAILURE;
        }
        pstDynamicLinearDrc->u32ISOCount = (HI_U32)s32Value;
    }

    snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "linear_drc:IsoLevel"); /*IsoLevel*/
    s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
    SCENE_INIPARAM_CHECK_LOAD_RESULT(s32Ret, aszIniNodeName);
    if (HI_NULL != pszString)
    {
        SCENE_GetNumbersInOneLine(pszString);
        for (u32IdxM = 0; u32IdxM < pstDynamicLinearDrc->u32ISOCount; u32IdxM++)
        {
            pstDynamicLinearDrc->au32ISOLevel[u32IdxM] = s_as64LineNum[u32IdxM];
        }
        free(pszString);
        pszString = HI_NULL;
    }

    snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "linear_drc:LocalMixingBrightMax");/*LocalMixingBrightMax*/
    s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
    SCENE_INIPARAM_CHECK_LOAD_RESULT(s32Ret, aszIniNodeName);
    if (HI_NULL != pszString)
    {
        SCENE_GetNumbersInOneLine(pszString);
        for (u32IdxM = 0; u32IdxM < pstDynamicLinearDrc->u32ISOCount; u32IdxM++)
        {
            pstDynamicLinearDrc->au8LocalMixingBrightMax[u32IdxM] = s_as64LineNum[u32IdxM];
        }
        free(pszString);
        pszString = HI_NULL;
    }

    snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "linear_drc:LocalMixingBrightMin");/*LocalMixingBrightMin*/
    s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
    SCENE_INIPARAM_CHECK_LOAD_RESULT(s32Ret, aszIniNodeName);
    if (HI_NULL != pszString)
    {
        SCENE_GetNumbersInOneLine(pszString);
        for (u32IdxM = 0; u32IdxM < pstDynamicLinearDrc->u32ISOCount; u32IdxM++)
        {
            pstDynamicLinearDrc->au8LocalMixingBrightMin[u32IdxM] = s_as64LineNum[u32IdxM];
        }
        free(pszString);
        pszString = HI_NULL;
    }

    snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "linear_drc:LocalMixingDarkMax");/*LocalMixingDarkMax*/
    s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
    SCENE_INIPARAM_CHECK_LOAD_RESULT(s32Ret, aszIniNodeName);
    if (HI_NULL != pszString)
    {
        SCENE_GetNumbersInOneLine(pszString);
        for (u32IdxM = 0; u32IdxM < pstDynamicLinearDrc->u32ISOCount; u32IdxM++)
        {
            pstDynamicLinearDrc->au8LocalMixingDarkMax[u32IdxM] = s_as64LineNum[u32IdxM];
        }
        free(pszString);
        pszString = HI_NULL;
    }

    snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "linear_drc:LocalMixingDarkMin");/*LocalMixingDarkMin*/
    s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
    SCENE_INIPARAM_CHECK_LOAD_RESULT(s32Ret, aszIniNodeName);
    if (HI_NULL != pszString)
    {
        SCENE_GetNumbersInOneLine(pszString);
        for (u32IdxM = 0; u32IdxM < pstDynamicLinearDrc->u32ISOCount; u32IdxM++)
        {
            pstDynamicLinearDrc->au8LocalMixingDarkMin[u32IdxM] = s_as64LineNum[u32IdxM];
        }
        free(pszString);
        pszString = HI_NULL;
    }

    snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "linear_drc:BrightGainLmt");/*BrightGainLmt*/
    s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
    SCENE_INIPARAM_CHECK_LOAD_RESULT(s32Ret, aszIniNodeName);
    if (HI_NULL != pszString)
    {
        SCENE_GetNumbersInOneLine(pszString);
        for (u32IdxM = 0; u32IdxM < pstDynamicLinearDrc->u32ISOCount; u32IdxM++)
        {
            pstDynamicLinearDrc->au8BrightGainLmt[u32IdxM] = s_as64LineNum[u32IdxM];
        }
        free(pszString);
        pszString = HI_NULL;
    }

    snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "linear_drc:BrightGainLmtStep"); /*BrightGainLmtStep*/
    s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
    SCENE_INIPARAM_CHECK_LOAD_RESULT(s32Ret, aszIniNodeName);
    if (HI_NULL != pszString)
    {
        SCENE_GetNumbersInOneLine(pszString);
        for (u32IdxM = 0; u32IdxM < pstDynamicLinearDrc->u32ISOCount; u32IdxM++)
        {
            pstDynamicLinearDrc->au8BrightGainLmtStep[u32IdxM] = s_as64LineNum[u32IdxM];
        }
        free(pszString);
        pszString = HI_NULL;
    }

    snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "linear_drc:DarkGainLmtY");/*DarkGainLmtY*/
    s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
    SCENE_INIPARAM_CHECK_LOAD_RESULT(s32Ret, aszIniNodeName);
    if (HI_NULL != pszString)
    {
        SCENE_GetNumbersInOneLine(pszString);
        for (u32IdxM = 0; u32IdxM < pstDynamicLinearDrc->u32ISOCount; u32IdxM++)
        {
            pstDynamicLinearDrc->au8DarkGainLmtY[u32IdxM] = s_as64LineNum[u32IdxM];
        }
        free(pszString);
        pszString = HI_NULL;
    }

    snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "linear_drc:DarkGainLmtC");/*DarkGainLmtC*/
    s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
    SCENE_INIPARAM_CHECK_LOAD_RESULT(s32Ret, aszIniNodeName);
    if (HI_NULL != pszString)
    {
        SCENE_GetNumbersInOneLine(pszString);
        for (u32IdxM = 0; u32IdxM < pstDynamicLinearDrc->u32ISOCount; u32IdxM++)
        {
            pstDynamicLinearDrc->au8DarkGainLmtC[u32IdxM] = s_as64LineNum[u32IdxM];
        }
        free(pszString);
        pszString = HI_NULL;
    }

    snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "linear_drc:ContrastControl");/*ContrastControl*/
    s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
    SCENE_INIPARAM_CHECK_LOAD_RESULT(s32Ret, aszIniNodeName);
    if (HI_NULL != pszString)
    {
        SCENE_GetNumbersInOneLine(pszString);
        for (u32IdxM = 0; u32IdxM < pstDynamicLinearDrc->u32ISOCount; u32IdxM++)
        {
            pstDynamicLinearDrc->au8ContrastControl[u32IdxM] = s_as64LineNum[u32IdxM];
        }
        free(pszString);
        pszString = HI_NULL;
    }

    snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "linear_drc:DetailAdjustFactor");/*DetailAdjustFactor*/
    s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
    SCENE_INIPARAM_CHECK_LOAD_RESULT(s32Ret, aszIniNodeName);
    if (HI_NULL != pszString)
    {
        SCENE_GetNumbersInOneLine(pszString);
        for (u32IdxM = 0; u32IdxM < pstDynamicLinearDrc->u32ISOCount; u32IdxM++)
        {
            pstDynamicLinearDrc->as8DetailAdjustFactor[u32IdxM] = s_as64LineNum[u32IdxM];
        }
        free(pszString);
        pszString = HI_NULL;
    }

    snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "linear_drc:Asymmetry");/*Asymmetry*/
    s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
    SCENE_INIPARAM_CHECK_LOAD_RESULT(s32Ret, aszIniNodeName);
    if (HI_NULL != pszString)
    {
        SCENE_GetNumbersInOneLine(pszString);
        for (u32IdxM = 0; u32IdxM < pstDynamicLinearDrc->u32ISOCount; u32IdxM++)
        {
            pstDynamicLinearDrc->au8Asymmetry[u32IdxM] = s_as64LineNum[u32IdxM];
        }
        free(pszString);
        pszString = HI_NULL;
    }

    snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "linear_drc:SecondPole"); /*SecondPole*/
    s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
    SCENE_INIPARAM_CHECK_LOAD_RESULT(s32Ret, aszIniNodeName);
    if (HI_NULL != pszString)
    {
        SCENE_GetNumbersInOneLine(pszString);
        for (u32IdxM = 0; u32IdxM < pstDynamicLinearDrc->u32ISOCount; u32IdxM++)
        {
            pstDynamicLinearDrc->au8SecondPole[u32IdxM] = s_as64LineNum[u32IdxM];
        }
        free(pszString);
        pszString = HI_NULL;
    }

    snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "linear_drc:Compress"); /*Compress*/
    s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
    SCENE_INIPARAM_CHECK_LOAD_RESULT(s32Ret, aszIniNodeName);
    if (HI_NULL != pszString)
    {
        SCENE_GetNumbersInOneLine(pszString);
        for (u32IdxM = 0; u32IdxM < pstDynamicLinearDrc->u32ISOCount; u32IdxM++)
        {
            pstDynamicLinearDrc->au8Compress[u32IdxM] = s_as64LineNum[u32IdxM];
        }
        free(pszString);
        pszString = HI_NULL;
    }

    snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "linear_drc:Stretch");/*Stretch*/
    s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
    SCENE_INIPARAM_CHECK_LOAD_RESULT(s32Ret, aszIniNodeName);
    if (HI_NULL != pszString)
    {
        SCENE_GetNumbersInOneLine(pszString);
        for (u32IdxM = 0; u32IdxM < pstDynamicLinearDrc->u32ISOCount; u32IdxM++)
        {
            pstDynamicLinearDrc->au8Stretch[u32IdxM] = s_as64LineNum[u32IdxM];
        }
        free(pszString);
        pszString = HI_NULL;
    }

    snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "linear_drc:Strength");/*Strength*/
    s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
    SCENE_INIPARAM_CHECK_LOAD_RESULT(s32Ret, aszIniNodeName);
    if (HI_NULL != pszString)
    {
        SCENE_GetNumbersInOneLine(pszString);
        for (u32IdxM = 0; u32IdxM < pstDynamicLinearDrc->u32ISOCount; u32IdxM++)
        {
            pstDynamicLinearDrc->au16Strength[u32IdxM] = s_as64LineNum[u32IdxM];
        }
        free(pszString);
        pszString = HI_NULL;
    }
    return HI_SUCCESS;
}

HI_S32 SCENE_LoadThreadDrc(const HI_CHAR *pszIniModule, HI_SCENE_THREAD_DRC_S *pstThreadDrc)
{
    HI_S32 s32Ret = HI_SUCCESS;
    HI_CHAR *pszString = NULL;
    HI_S32 s32Value = 0;
    HI_U32 u32IdxM = 0;
    HI_U32 u32IdxN = 0;
    HI_CHAR aszIniNodeName[SCENE_INIPARAM_NODE_NAME_LEN] = {0,};

    snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "thread_drc:RationCnt");/*RationCnt*/
    s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
    SCENE_INIPARAM_CHECK_LOAD_RESULT(s32Ret, aszIniNodeName);
    if (HI_NULL != pszString)
    {
        free(pszString);
        pszString = HI_NULL;
        s32Ret = HI_CONFACCESS_GetInt(SCENE_INIPARAM, pszIniModule, "thread_drc:RationCnt", 0, &s32Value);
        if (HI_SUCCESS != s32Ret)
        {
            MLOGE("load thread_drc:RationCnt failed\n");
            return HI_FAILURE;
        }
        pstThreadDrc->u32RationCount = (HI_U32)s32Value;
    }

    snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "thread_drc:RationLevel");/*RationLevel*/
    s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
    SCENE_INIPARAM_CHECK_LOAD_RESULT(s32Ret, aszIniNodeName);
    if (HI_NULL != pszString)
    {
        SCENE_GetNumbersInOneLine(pszString);
        for (u32IdxM = 0; u32IdxM < pstThreadDrc->u32RationCount; u32IdxM++)
        {
            pstThreadDrc->au32RationLevel[u32IdxM] = s_as64LineNum[u32IdxM];
        }
        free(pszString);
        pszString = HI_NULL;
    }

    snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "thread_drc:RefRatioCnt");/*RefRatioCnt*/
    s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
    SCENE_INIPARAM_CHECK_LOAD_RESULT(s32Ret, aszIniNodeName);
    if (HI_NULL != pszString)
    {
        free(pszString);
        pszString = HI_NULL;
        s32Ret = HI_CONFACCESS_GetInt(SCENE_INIPARAM, pszIniModule, "thread_drc:RefRatioCnt", 0, &s32Value);
        if (HI_SUCCESS != s32Ret)
        {
            MLOGE("load thread_drc:ExpCnt failed\n");
            return HI_FAILURE;
        }
        pstThreadDrc->u32RefRatioCount = (HI_U32)s32Value;
    }

    snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "thread_drc:RefRatioLtoH");/*RefRatioLtoH*/
    s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
    SCENE_INIPARAM_CHECK_LOAD_RESULT(s32Ret, aszIniNodeName);
    if (HI_NULL != pszString)
    {
        SCENE_GetNumbersInOneLine(pszString);
        for (u32IdxM = 0; u32IdxM < pstThreadDrc->u32RefRatioCount; u32IdxM++)
        {
            pstThreadDrc->au32RefRatioLtoH[u32IdxM] = s_as64LineNum[u32IdxM];
        }
        free(pszString);
        pszString = HI_NULL;
    }

    snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "thread_drc:RefRatioAlpha");/*RefRatioAlpha*/
    s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
    SCENE_INIPARAM_CHECK_LOAD_RESULT(s32Ret, aszIniNodeName);
    if (HI_NULL != pszString)
    {
        SCENE_GetNumbersInOneLine(pszString);
        for (u32IdxM = 0; u32IdxM < pstThreadDrc->u32RefRatioCount; u32IdxM++)
        {
            pstThreadDrc->u32RefRatioAlpha[u32IdxM] = s_as64LineNum[u32IdxM];
        }
        free(pszString);
        pszString = HI_NULL;
    }

    snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "thread_drc:IsoCnt");/*IsoCnt*/
    s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
    SCENE_INIPARAM_CHECK_LOAD_RESULT(s32Ret, aszIniNodeName);
    if (HI_NULL != pszString)
    {
        free(pszString);
        pszString = HI_NULL;
        s32Ret = HI_CONFACCESS_GetInt(SCENE_INIPARAM, pszIniModule, "thread_drc:IsoCnt", 0, &s32Value);
        if (HI_SUCCESS != s32Ret)
        {
            MLOGE("load thread_drc:IsoCnt failed\n");
            return HI_FAILURE;
        }
        pstThreadDrc->u32ISOCount = (HI_U32)s32Value;
    }

    snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "thread_drc:IsoLevel");/*IsoLevel*/
    s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
    SCENE_INIPARAM_CHECK_LOAD_RESULT(s32Ret, aszIniNodeName);
    if (HI_NULL != pszString)
    {
        SCENE_GetNumbersInOneLine(pszString);
        for (u32IdxM = 0; u32IdxM < pstThreadDrc->u32ISOCount; u32IdxM++)
        {
            pstThreadDrc->au32ISOLevel[u32IdxM] = s_as64LineNum[u32IdxM];
        }
        free(pszString);
        pszString = HI_NULL;
    }

    snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "thread_drc:Interval");/*Interval*/
    s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
    SCENE_INIPARAM_CHECK_LOAD_RESULT(s32Ret, aszIniNodeName);
    if (HI_NULL != pszString)
    {
        free(pszString);
        pszString = HI_NULL;
        s32Ret = HI_CONFACCESS_GetInt(SCENE_INIPARAM, pszIniModule, "thread_drc:Interval", 0, &s32Value);
        if (HI_SUCCESS != s32Ret)
        {
            MLOGE("load thread_drc:Interval failed\n");
            return HI_FAILURE;
        }
        pstThreadDrc->u32Interval = (HI_U32)s32Value;
    }

    snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "thread_drc:DRCTMValueLow");/*DRCTMValueLow*/
    s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
    SCENE_INIPARAM_CHECK_LOAD_RESULT(s32Ret, aszIniNodeName);
    if (HI_NULL != pszString)
    {
        SCENE_GetNumbersInOneLine(pszString);
        for (u32IdxM = 0; u32IdxM < HI_ISP_DRC_TM_NODE_NUM; u32IdxM++)
        {
            pstThreadDrc->au16TMValueLow[u32IdxM] = s_as64LineNum[u32IdxM];
        }
        free(pszString);
        pszString = HI_NULL;
    }

    snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "thread_drc:DRCTMValueHigh");/*DRCTMValueHigh*/
    s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
    SCENE_INIPARAM_CHECK_LOAD_RESULT(s32Ret, aszIniNodeName);
    if (HI_NULL != pszString)
    {
        SCENE_GetNumbersInOneLine(pszString);
        for (u32IdxM = 0; u32IdxM < HI_ISP_DRC_TM_NODE_NUM; u32IdxM++)
        {
            pstThreadDrc->au16TMValueHigh[u32IdxM] = s_as64LineNum[u32IdxM];
        }
        free(pszString);
        pszString = HI_NULL;
    }

    snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "thread_drc:LocalMixingBrightMax");/*LocalMixingBrightMax*/
    s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
    SCENE_INIPARAM_CHECK_LOAD_RESULT(s32Ret, aszIniNodeName);
    if (HI_NULL != pszString)
    {
        SCENE_GetNumbersInOneLine(pszString);
        for (u32IdxM = 0; u32IdxM < pstThreadDrc->u32ISOCount; u32IdxM++)
        {
            pstThreadDrc->au8LocalMixingBrightMax[u32IdxM] = s_as64LineNum[u32IdxM];
        }
        free(pszString);
        pszString = HI_NULL;
    }

    snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "thread_drc:LocalMixingBrightMin");/*LocalMixingBrightMin*/
    s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
    SCENE_INIPARAM_CHECK_LOAD_RESULT(s32Ret, aszIniNodeName);
    if (HI_NULL != pszString)
    {
        SCENE_GetNumbersInOneLine(pszString);
        for (u32IdxM = 0; u32IdxM < pstThreadDrc->u32ISOCount; u32IdxM++)
        {
            pstThreadDrc->au8LocalMixingBrightMin[u32IdxM] = s_as64LineNum[u32IdxM];
        }
        free(pszString);
        pszString = HI_NULL;
    }

    snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "thread_drc:LocalMixingDarkMax");/*LocalMixingDarkMax*/
    s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
    SCENE_INIPARAM_CHECK_LOAD_RESULT(s32Ret, aszIniNodeName);
    if (HI_NULL != pszString)
    {
        SCENE_GetNumbersInOneLine(pszString);
        for (u32IdxM = 0; u32IdxM < pstThreadDrc->u32ISOCount; u32IdxM++)
        {
            pstThreadDrc->au8LocalMixingDarkMax[u32IdxM] = s_as64LineNum[u32IdxM];
        }
        free(pszString);
        pszString = HI_NULL;
    }

    snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "thread_drc:LocalMixingDarkMin");/*LocalMixingDarkMin*/
    s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
    SCENE_INIPARAM_CHECK_LOAD_RESULT(s32Ret, aszIniNodeName);
    if (HI_NULL != pszString)
    {
        SCENE_GetNumbersInOneLine(pszString);
        for (u32IdxM = 0; u32IdxM < pstThreadDrc->u32ISOCount; u32IdxM++)
        {
            pstThreadDrc->au8LocalMixingDarkMin[u32IdxM] = s_as64LineNum[u32IdxM];
        }
        free(pszString);
        pszString = HI_NULL;
    }

    snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "thread_drc:DarkGainLmtY");/*DarkGainLmtY*/
    s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
    SCENE_INIPARAM_CHECK_LOAD_RESULT(s32Ret, aszIniNodeName);
    if (HI_NULL != pszString)
    {
        SCENE_GetNumbersInOneLine(pszString);
        for (u32IdxM = 0; u32IdxM < pstThreadDrc->u32ISOCount; u32IdxM++)
        {
            pstThreadDrc->au8DarkGainLmtY[u32IdxM] = s_as64LineNum[u32IdxM];
        }
        free(pszString);
        pszString = HI_NULL;
    }

    snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "thread_drc:DarkGainLmtC");/*DarkGainLmtC*/
    s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
    SCENE_INIPARAM_CHECK_LOAD_RESULT(s32Ret, aszIniNodeName);
    if (HI_NULL != pszString)
    {
        SCENE_GetNumbersInOneLine(pszString);
        for (u32IdxM = 0; u32IdxM < pstThreadDrc->u32ISOCount; u32IdxM++)
        {
            pstThreadDrc->au8DarkGainLmtC[u32IdxM] = s_as64LineNum[u32IdxM];
        }
        free(pszString);
        pszString = HI_NULL;
    }

    snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "thread_drc:DetailAdjustFactor");/*DetailAdjustFactor*/
    s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
    SCENE_INIPARAM_CHECK_LOAD_RESULT(s32Ret, aszIniNodeName);
    if (HI_NULL != pszString)
    {
        SCENE_GetNumbersInOneLine(pszString);
        for (u32IdxM = 0; u32IdxM < pstThreadDrc->u32ISOCount; u32IdxM++)
        {
            pstThreadDrc->as8DetailAdjustFactor[u32IdxM] = s_as64LineNum[u32IdxM];
        }
        free(pszString);
        pszString = HI_NULL;
    }

    snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "thread_drc:SpatialFltCoef");/*SpatialFltCoef*/
    s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
    SCENE_INIPARAM_CHECK_LOAD_RESULT(s32Ret, aszIniNodeName);
    if (HI_NULL != pszString)
    {
        SCENE_GetNumbersInOneLine(pszString);
        for (u32IdxM = 0; u32IdxM < pstThreadDrc->u32ISOCount; u32IdxM++)
        {
            pstThreadDrc->au8SpatialFltCoef[u32IdxM] = s_as64LineNum[u32IdxM];
        }
        free(pszString);
        pszString = HI_NULL;
    }

    snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "thread_drc:RangeFltCoef");/*RangeFltCoef*/
    s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
    SCENE_INIPARAM_CHECK_LOAD_RESULT(s32Ret, aszIniNodeName);
    if (HI_NULL != pszString)
    {
        SCENE_GetNumbersInOneLine(pszString);
        for (u32IdxM = 0; u32IdxM < pstThreadDrc->u32ISOCount; u32IdxM++)
        {
            pstThreadDrc->au8RangeFltCoef[u32IdxM] = s_as64LineNum[u32IdxM];
        }
        free(pszString);
        pszString = HI_NULL;
    }

    snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "thread_drc:GradRevMax");/*GradRevMax*/
    s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
    SCENE_INIPARAM_CHECK_LOAD_RESULT(s32Ret, aszIniNodeName);
    if (HI_NULL != pszString)
    {
        SCENE_GetNumbersInOneLine(pszString);
        for (u32IdxM = 0; u32IdxM < pstThreadDrc->u32ISOCount; u32IdxM++)
        {
            pstThreadDrc->au8GradRevMax[u32IdxM] = s_as64LineNum[u32IdxM];
        }
        free(pszString);
        pszString = HI_NULL;
    }

    snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "thread_drc:GradRevThr");/*GradRevThr*/
    s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
    SCENE_INIPARAM_CHECK_LOAD_RESULT(s32Ret, aszIniNodeName);
    if (HI_NULL != pszString)
    {
        SCENE_GetNumbersInOneLine(pszString);
        for (u32IdxM = 0; u32IdxM < pstThreadDrc->u32ISOCount; u32IdxM++)
        {
            pstThreadDrc->au8GradRevThr[u32IdxM] = s_as64LineNum[u32IdxM];
        }
        free(pszString);
        pszString = HI_NULL;
    }

    snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "thread_drc:Compress");/*Compress*/
    s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
    SCENE_INIPARAM_CHECK_LOAD_RESULT(s32Ret, aszIniNodeName);
    if (HI_NULL != pszString)
    {
        SCENE_GetNumbersInOneLine(pszString);
        for (u32IdxM = 0; u32IdxM < pstThreadDrc->u32ISOCount; u32IdxM++)
        {
            pstThreadDrc->au8Compress[u32IdxM] = s_as64LineNum[u32IdxM];
        }
        free(pszString);
        pszString = HI_NULL;
    }

    snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "thread_drc:Stretch");/*Stretch*/
    s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
    SCENE_INIPARAM_CHECK_LOAD_RESULT(s32Ret, aszIniNodeName);
    if (HI_NULL != pszString)
    {
        SCENE_GetNumbersInOneLine(pszString);
        for (u32IdxM = 0; u32IdxM < pstThreadDrc->u32ISOCount; u32IdxM++)
        {
            pstThreadDrc->au8Stretch[u32IdxM] = s_as64LineNum[u32IdxM];
        }
        free(pszString);
        pszString = HI_NULL;
    }

    snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "thread_drc:AutoStrength_0");/*AutoStrength_0*/
    s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
    SCENE_INIPARAM_CHECK_LOAD_RESULT(s32Ret, aszIniNodeName);
    if (HI_NULL != pszString)
    {
        free(pszString);
        pszString = HI_NULL;
        for (u32IdxM = 0; u32IdxM < pstThreadDrc->u32ISOCount; u32IdxM++)
        {
            snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "thread_drc:AutoStrength_%d", u32IdxM);
            s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
            if (HI_SUCCESS != s32Ret)
            {
                MLOGE("load %s failed\n", aszIniNodeName);
                return HI_FAILURE;
            }
            if (NULL != pszString)
            {
                SCENE_GetNumbersInOneLine(pszString);
                for (u32IdxN = 0; u32IdxN < pstThreadDrc->u32RationCount; u32IdxN++)
                {
                    pstThreadDrc->u16AutoStrength[u32IdxM][u32IdxN] = (HI_U16)s_as64LineNum[u32IdxN];
                }
                free(pszString);
                pszString = HI_NULL;
            }
        }
    }

    return HI_SUCCESS;
}

HI_S32 SCENE_LoadDynamicVencBitrate(const HI_CHAR *pszIniModule, HI_SCENE_DYNAMIC_VENCBITRATE_S *pstDynamicVenc)
{
    HI_S32 s32Ret = HI_SUCCESS;
    HI_CHAR *pszString = NULL;
    HI_S32 s32Value = 0;
    HI_U32 u32IdxM = 0;
    HI_CHAR aszIniNodeName[SCENE_INIPARAM_NODE_NAME_LEN] = {0,};

    snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "dynamic_vencbitrate:IsoThreshCnt");/*IsoThreshCnt*/
    s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
    SCENE_INIPARAM_CHECK_LOAD_RESULT(s32Ret, aszIniNodeName);
    if (HI_NULL != pszString)
    {
        free(pszString);
        pszString = HI_NULL;
        s32Ret = HI_CONFACCESS_GetInt(SCENE_INIPARAM, pszIniModule, "dynamic_vencbitrate:IsoThreshCnt", 0, &s32Value);
        if (HI_SUCCESS != s32Ret)
        {
            MLOGE("load dynamic_vencbitrate:TotalNum failed\n");
            return HI_FAILURE;
        }
        pstDynamicVenc->u32IsoThreshCnt = s32Value;
    }

    snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "dynamic_vencbitrate:IsoThreshLtoH");/*IsoThreshLtoH*/
    s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
    SCENE_INIPARAM_CHECK_LOAD_RESULT(s32Ret, aszIniNodeName);
    if (HI_NULL != pszString)
    {
        SCENE_GetNumbersInOneLine(pszString);
        for (u32IdxM = 0; u32IdxM < pstDynamicVenc->u32IsoThreshCnt; u32IdxM++)
        {
            pstDynamicVenc->au32IsoThreshLtoH[u32IdxM] = s_as64LineNum[u32IdxM];
        }
        free(pszString);
        pszString = HI_NULL;
    }

    snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "dynamic_vencbitrate:ManualPercent");/*ManualPercent*/
    s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
    SCENE_INIPARAM_CHECK_LOAD_RESULT(s32Ret, aszIniNodeName);
    if (HI_NULL != pszString)
    {
        SCENE_GetNumbersInOneLine(pszString);
        for (u32IdxM = 0; u32IdxM < pstDynamicVenc->u32IsoThreshCnt; u32IdxM++)
        {
            pstDynamicVenc->au16ManualPercent[u32IdxM] = (HI_U16)s_as64LineNum[u32IdxM];
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

	snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "module_state:bStaticAERouteEx");/*bStaticAERouteEx*/
    s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
    SCENE_INIPARAM_CHECK_LOAD_RESULT(s32Ret, aszIniNodeName);
    if (HI_NULL != pszString)
    {
        free(pszString);
        pszString = HI_NULL;
        s32Ret = HI_CONFACCESS_GetInt(SCENE_INIPARAM, pszIniModule, "module_state:bStaticAERouteEx", 0, &s32Value);
        if (HI_SUCCESS != s32Ret)
        {
            MLOGE("load module_state:bStaticAERouteEx failed\n");
            return HI_FAILURE;
        }
        pstModuleState->bStaticAERouteEx = (HI_BOOL)s32Value;
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

	snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "module_state:bStaticStatistics");/*bStaticStatistics*/
    s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
    SCENE_INIPARAM_CHECK_LOAD_RESULT(s32Ret, aszIniNodeName);
    if (HI_NULL != pszString)
    {
        free(pszString);
        pszString = HI_NULL;
        s32Ret = HI_CONFACCESS_GetInt(SCENE_INIPARAM, pszIniModule, "module_state:bStaticStatistics", 0, &s32Value);
        if (HI_SUCCESS != s32Ret)
        {
            MLOGE("load module_state:bStaticStatistics failed\n");
            return HI_FAILURE;
        }
        pstModuleState->bStaticStatistics = (HI_BOOL)s32Value;
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

	snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "module_state:bDynamicThreeDNR");/*bDynamicThreeDNR*/
    s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
    SCENE_INIPARAM_CHECK_LOAD_RESULT(s32Ret, aszIniNodeName);
    if (HI_NULL != pszString)
    {
        free(pszString);
        pszString = HI_NULL;
        s32Ret = HI_CONFACCESS_GetInt(SCENE_INIPARAM, pszIniModule, "module_state:bDynamicThreeDNR", 0, &s32Value);
        if (HI_SUCCESS != s32Ret)
        {
            MLOGE("load module_state:bDynamicThreeDNR failed\n");
            return HI_FAILURE;
        }
        pstModuleState->bDynamicThreeDNR = (HI_BOOL)s32Value;
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
	snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "module_state:bDynamicIsoVenc");/*bDynamicIsoVenc*/
    s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
    SCENE_INIPARAM_CHECK_LOAD_RESULT(s32Ret, aszIniNodeName);
    if (HI_NULL != pszString)
    {
        free(pszString);
        pszString = HI_NULL;
        s32Ret = HI_CONFACCESS_GetInt(SCENE_INIPARAM, pszIniModule, "module_state:bDynamicIsoVenc", 0, &s32Value);
        if (HI_SUCCESS != s32Ret)
        {
            MLOGE("load module_state:bDynamicIsoVenc failed\n");
            return HI_FAILURE;
        }
        pstModuleState->bDynamicIsoVenc = (HI_BOOL)s32Value;
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

	snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "module_state:bStaticDemosaic");/*bStaticDemosaic*/
    s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
    SCENE_INIPARAM_CHECK_LOAD_RESULT(s32Ret, aszIniNodeName);
    if (HI_NULL != pszString)
    {
        free(pszString);
        pszString = HI_NULL;
        s32Ret = HI_CONFACCESS_GetInt(SCENE_INIPARAM, pszIniModule, "module_state:bStaticDemosaic", 0, &s32Value);
        if (HI_SUCCESS != s32Ret)
        {
            MLOGE("load module_state:bStaticDemosaic failed\n");
            return HI_FAILURE;
        }
        pstModuleState->bStaticDemosaic = (HI_BOOL)s32Value;
    }

	snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "module_state:bDynamicBayernr");/*bDynamicBayernr*/
    s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
    SCENE_INIPARAM_CHECK_LOAD_RESULT(s32Ret, aszIniNodeName);
    if (HI_NULL != pszString)
    {
        free(pszString);
        pszString = HI_NULL;
        s32Ret = HI_CONFACCESS_GetInt(SCENE_INIPARAM, pszIniModule, "module_state:bDynamicBayernr", 0, &s32Value);
        if (HI_SUCCESS != s32Ret)
        {
            MLOGE("load module_state:bDynamicBayernr failed\n");
            return HI_FAILURE;
        }
        pstModuleState->bDynamicBayernr = (HI_BOOL)s32Value;
    }

	snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "module_state:bStaticDetail");/*bStaticDetail*/
    s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
    SCENE_INIPARAM_CHECK_LOAD_RESULT(s32Ret, aszIniNodeName);
    if (HI_NULL != pszString)
    {
        free(pszString);
        pszString = HI_NULL;
        s32Ret = HI_CONFACCESS_GetInt(SCENE_INIPARAM, pszIniModule, "module_state:bStaticDetail", 0, &s32Value);
        if (HI_SUCCESS != s32Ret)
        {
            MLOGE("load module_state:bStaticDetail failed\n");
            return HI_FAILURE;
        }
        pstModuleState->bStaticDetail = (HI_BOOL)s32Value;
    }

	snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "module_state:bDynamicDIS");/*bDynamicDIS*/
    s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
    SCENE_INIPARAM_CHECK_LOAD_RESULT(s32Ret, aszIniNodeName);
    if (HI_NULL != pszString)
    {
        free(pszString);
        pszString = HI_NULL;
        s32Ret = HI_CONFACCESS_GetInt(SCENE_INIPARAM, pszIniModule, "module_state:bDynamicDIS", 0, &s32Value);
        if (HI_SUCCESS != s32Ret)
        {
            MLOGE("load module_state:bDynamicDIS failed\n");
            return HI_FAILURE;
        }
        pstModuleState->bDynamicDIS = (HI_BOOL)s32Value;
    }

	snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "module_state:bStaticDrc");/*bStaticDrc*/
    s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
    SCENE_INIPARAM_CHECK_LOAD_RESULT(s32Ret, aszIniNodeName);
    if (HI_NULL != pszString)
    {
        free(pszString);
        pszString = HI_NULL;
        s32Ret = HI_CONFACCESS_GetInt(SCENE_INIPARAM, pszIniModule, "module_state:bStaticDrc", 0, &s32Value);
        if (HI_SUCCESS != s32Ret)
        {
            MLOGE("load module_state:bStaticDrc failed\n");
            return HI_FAILURE;
        }
        pstModuleState->bStaticDrc = (HI_BOOL)s32Value;
    }

	snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "module_state:bDynamicLinearDrc");/*bDynamicLinearDrc*/
    s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
    SCENE_INIPARAM_CHECK_LOAD_RESULT(s32Ret, aszIniNodeName);
    if (HI_NULL != pszString)
    {
        free(pszString);
        pszString = HI_NULL;
        s32Ret = HI_CONFACCESS_GetInt(SCENE_INIPARAM, pszIniModule, "module_state:bDynamicLinearDrc", 0, &s32Value);
        if (HI_SUCCESS != s32Ret)
        {
            MLOGE("load module_state:bDynamicLinearDrc failed\n");
            return HI_FAILURE;
        }
        pstModuleState->bDynamicLinearDrc = (HI_BOOL)s32Value;
    }

    snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "module_state:bDynamicThreadDrc");/*bDynamicThreadDrc*/
    s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
    SCENE_INIPARAM_CHECK_LOAD_RESULT(s32Ret, aszIniNodeName);
    if (HI_NULL != pszString)
    {
        free(pszString);
        pszString = HI_NULL;
        s32Ret = HI_CONFACCESS_GetInt(SCENE_INIPARAM, pszIniModule, "module_state:bStaticDrc", 0, &s32Value);
        if (HI_SUCCESS != s32Ret)
        {
            MLOGE("load module_state:bDynamicThreadDrc failed\n");
            return HI_FAILURE;
        }
        pstModuleState->bDynamicThreadDrc = (HI_BOOL)s32Value;
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

    s32Ret = SCENE_LoadStaticAERoute(pszIniModule, &pstSceneParam->stStaticAeRoute);
    if (HI_SUCCESS != s32Ret)
    {
        MLOGE("SCENE_LoadStaticAERoute failed\n");
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

    s32Ret = SCENE_LoadStaticAeRouteEX(pszIniModule, &pstSceneParam->stStaticAeRouteEx);
    if (HI_SUCCESS != s32Ret)
    {
        MLOGE("SCENE_LoadStaticAeRouteEX failed\n");
        return HI_FAILURE;
    }

    s32Ret = SCENE_LoadStaticGlobalCac(pszIniModule, &pstSceneParam->stStaticGlobalCac);
    if (HI_SUCCESS != s32Ret)
    {
        MLOGE("SCENE_LoadStaticGlobalCac failed\n");
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

    s32Ret = SCENE_LoadStaticCCM(pszIniModule, &pstSceneParam->stStaticCcm);
    if (HI_SUCCESS != s32Ret)
    {
        MLOGE("SCENE_LoadStaticCCM failed\n");
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

    s32Ret = SCENE_LoadStaticStatistics(pszIniModule, &pstSceneParam->stStaticStatistics);
    if (HI_SUCCESS != s32Ret)
    {
        MLOGE("SCENE_LoadStaticStatistics failed\n");
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
    s32Ret = SCENE_LoadStaticShading(pszIniModule, &pstSceneParam->stStaticShading);
    if (HI_SUCCESS != s32Ret)
    {
        MLOGE("SCENE_LoadStaticShading failed\n");
        return HI_FAILURE;
    }

    s32Ret = SCENE_LoadDynamicThreeDNR(pszIniModule, &pstSceneParam->stDynamicThreeDNR);
    if (HI_SUCCESS != s32Ret)
    {
        MLOGE("SCENE_LoadDynamicThreeDNR failed\n");
        return HI_FAILURE;
    }

    s32Ret = SCENE_LoadDynamicGamma(pszIniModule, &pstSceneParam->stDynamicGamma);
    if (HI_SUCCESS != s32Ret)
    {
        MLOGE("SCENE_LoadDynamicGamma failed\n");
        return HI_FAILURE;
    }

    s32Ret = SCENE_LoadDynamicShading(pszIniModule, &pstSceneParam->stDynamicShading);
    if (HI_SUCCESS != s32Ret)
    {
        MLOGE("SCENE_LoadDynamicShading failed\n");
        return HI_FAILURE;
    }
    s32Ret = SCENE_LoadDynamicVencBitrate(pszIniModule, &pstSceneParam->stDynamicVencBitrate);
    if (HI_SUCCESS != s32Ret)
    {
        MLOGE("SCENE_LoadDynamicVencBitrate failed\n");
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

    s32Ret = SCENE_LoadDynamicAE(pszIniModule, &pstSceneParam->stDynamicAe);
    if (HI_SUCCESS != s32Ret)
    {
        MLOGE("SCENE_LoadDynamicAE failed\n");
        return HI_FAILURE;
    }

    s32Ret = SCENE_LoadStaticSharpen(pszIniModule, &pstSceneParam->stStaticSharpen);
    if (HI_SUCCESS != s32Ret)
    {
        MLOGE("SCENE_LoadStaticSharpen failed\n");
        return HI_FAILURE;
    }

    s32Ret = SCENE_LoadStaticDemosaic(pszIniModule, &pstSceneParam->stStaticDemosaic);
    if (HI_SUCCESS != s32Ret)
    {
        MLOGE("SCENE_LoadStaticDemosaic failed\n");
        return HI_FAILURE;
    }

    s32Ret = SCENE_LoadDynamicBayernr(pszIniModule, &pstSceneParam->stDynamicBayernr);
    if (HI_SUCCESS != s32Ret)
    {
        MLOGE("SCENE_LoadDynamicBayernr failed\n");
        return HI_FAILURE;
    }

    s32Ret = SCENE_LoadStaticDetail(pszIniModule, &pstSceneParam->stStaticDetail);
    if (HI_SUCCESS != s32Ret)
    {
        MLOGE("SCENE_LoadStaticDetail failed\n");
        return HI_FAILURE;
    }

    s32Ret = SCENE_LoadDynamicDIS(pszIniModule, &pstSceneParam->stDynamicDis);
    if (HI_SUCCESS != s32Ret)
    {
        MLOGE("SCENE_LoadDynamicDIS failed\n");
        return HI_FAILURE;
    }

    s32Ret = SCENE_LoadDynamicLinearDrc(pszIniModule, &pstSceneParam->stDynamicLinearDrc);
    if (HI_SUCCESS != s32Ret)
    {
        MLOGE("SCENE_LoadThreadDrc failed\n");
        return HI_FAILURE;
    }

    s32Ret = SCENE_LoadThreadDrc(pszIniModule, &pstSceneParam->stThreadDrc);
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

