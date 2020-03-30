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
#include "hi_scene_autogenerate.h"
#include "hi_scene_paramtype_autogenerate.h"
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

HI_S32 SCENE_LoadStaticAE(const HI_CHAR *pszIniModule, HI_SCENE_STATIC_AE_S *pstStaticAe)
{
    HI_S32 s32Ret = HI_SUCCESS;
    HI_CHAR *pszString = HI_NULL;
    HI_S32 s32Value = 0;
    HI_CHAR aszIniNodeName[SCENE_INIPARAM_NODE_NAME_LEN] = {0,};

#ifdef CFG_STA_AE_INTERVAL
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
#endif

#ifdef CFG_STA_AE_EXVALID
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
#endif

#ifdef CFG_STA_AE_SYSGAINMAX
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
#endif

#ifdef CFG_STA_AE_EXPTIMEMAX
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
#endif


#ifdef CFG_STA_AE_SPEED
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
#endif

#ifdef CFG_STA_AE_TOLERANCE
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
#endif

#ifdef CFG_STA_AE_BLACKDELAYFRAME
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
#endif

#ifdef CFG_STA_AE_WHITEDELAYFRAME
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
#endif

    return HI_SUCCESS;
}

HI_S32 SCENE_LoadStaticAERoute(const HI_CHAR *pszIniModule, HI_SCENE_STATIC_AEROUTE_S *pstStaticAeRoute)
{

#ifdef CFG_STA_AEROUTE_TOTALNUM
    HI_S32 s32Ret = HI_SUCCESS;
    HI_CHAR *pszString = NULL;
    HI_S32 s32Value = 0;
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
#endif

#ifdef CFG_STA_AEROUTE_INTTIME
    HI_S32 s32Ret = HI_SUCCESS;
    HI_CHAR *pszString = NULL;
    HI_U32 u32IdxM = 0;
    HI_CHAR aszIniNodeName[SCENE_INIPARAM_NODE_NAME_LEN] = {0,};
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
#endif

#ifdef CFG_STA_AEROUTE_SYSGAIN
    HI_S32 s32Ret = HI_SUCCESS;
    HI_CHAR *pszString = NULL;
    HI_U32 u32IdxM = 0;
    HI_CHAR aszIniNodeName[SCENE_INIPARAM_NODE_NAME_LEN] = {0,};
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
#ifdef CFG_STA_AWB_STATICWB
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
#endif

#ifdef CFG_STA_AWB_CURVEPARA
    HI_S32 s32Ret = HI_SUCCESS;
    HI_CHAR *pszString = NULL;
    HI_U32 u32IdxM = 0;
    HI_CHAR aszIniNodeName[SCENE_INIPARAM_NODE_NAME_LEN] = {0,};
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
#endif

#ifdef CFG_STA_AWB_SPEED
    HI_S32 s32Ret = HI_SUCCESS;
    HI_CHAR *pszString = NULL;
    HI_S32 s32Value = 0;
    HI_CHAR aszIniNodeName[SCENE_INIPARAM_NODE_NAME_LEN] = {0,};
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
#endif

#ifdef CFG_STA_AWB_LOWCOLORTEMP
    HI_S32 s32Ret = HI_SUCCESS;
    HI_CHAR *pszString = NULL;
    HI_S32 s32Value = 0;
    HI_CHAR aszIniNodeName[SCENE_INIPARAM_NODE_NAME_LEN] = {0,};
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
#endif

#ifdef CFG_STA_AWB_CRMAX
    HI_S32 s32Ret = HI_SUCCESS;
    HI_CHAR *pszString = NULL;
    HI_U32 u32IdxM = 0;
    HI_CHAR aszIniNodeName[SCENE_INIPARAM_NODE_NAME_LEN] = {0,};
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
#endif

#ifdef CFG_STA_AWB_CRMIN
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
#endif

#ifdef CFG_STA_AWB_CBMAX
    HI_S32 s32Ret = HI_SUCCESS;
    HI_CHAR *pszString = NULL;
    HI_U32 u32IdxM = 0;
    HI_CHAR aszIniNodeName[SCENE_INIPARAM_NODE_NAME_LEN] = {0,};
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
#endif

#ifdef CFG_STA_AWB_CBMIN
    HI_S32 s32Ret = HI_SUCCESS;
    HI_CHAR *pszString = NULL;
    HI_U32 u32IdxM = 0;
    HI_CHAR aszIniNodeName[SCENE_INIPARAM_NODE_NAME_LEN] = {0,};
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
#ifdef CFG_STA_AWBEX_TOLERANCE
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
#endif

#ifdef CFG_STA_AWBEX_OUTTHRESH
    HI_S32 s32Ret = HI_SUCCESS;
    HI_CHAR *pszString = NULL;
    HI_S32 s32Value = 0;
    HI_CHAR aszIniNodeName[SCENE_INIPARAM_NODE_NAME_LEN] = {0,};
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
#endif

#ifdef CFG_STA_AWBEX_LOWSTOP
    HI_S32 s32Ret = HI_SUCCESS;
    HI_CHAR *pszString = NULL;
    HI_S32 s32Value = 0;
    HI_CHAR aszIniNodeName[SCENE_INIPARAM_NODE_NAME_LEN] = {0,};
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
#endif

#ifdef CFG_STA_AWBEX_HIGHSTART
    HI_S32 s32Ret = HI_SUCCESS;
    HI_CHAR *pszString = NULL;
    HI_S32 s32Value = 0;
    HI_CHAR aszIniNodeName[SCENE_INIPARAM_NODE_NAME_LEN] = {0,};
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
#endif

#ifdef CFG_STA_AWBEX_HIGHSTOP
    HI_S32 s32Ret = HI_SUCCESS;
    HI_CHAR *pszString = NULL;
    HI_S32 s32Value = 0;
    HI_CHAR aszIniNodeName[SCENE_INIPARAM_NODE_NAME_LEN] = {0,};
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
#endif

#ifdef CFG_STA_AWBEX_MULTILIGHTSOURCEEN
    HI_S32 s32Ret = HI_SUCCESS;
    HI_CHAR *pszString = NULL;
    HI_S32 s32Value = 0;
    HI_CHAR aszIniNodeName[SCENE_INIPARAM_NODE_NAME_LEN] = {0,};
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
#endif

#ifdef CFG_STA_AWBEX_MULTICTWT
    HI_S32 s32Ret = HI_SUCCESS;
    HI_CHAR *pszString = NULL;
    HI_U32 u32IdxM = 0;
    HI_CHAR aszIniNodeName[SCENE_INIPARAM_NODE_NAME_LEN] = {0,};
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

#ifdef CFG_STA_AEROUTEEX_TOTALNUM
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
#endif

#ifdef CFG_STA_AEROUTEEX_ROUTEEXINTTIME
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
#endif

#ifdef CFG_STA_AEROUTEEX_ROUTEEXAGAIN
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
#endif

#ifdef CFG_STA_AEROUTEEX_ROUTEEXDGAIN
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
#endif

#ifdef CFG_STA_AEROUTEEX_ROUTEEXISPDGAIN
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
#endif

    return HI_SUCCESS;
}

HI_S32 SCENE_LoadStaticGlobalCac(const HI_CHAR *pszIniModule, HI_SCENE_STATIC_GLOBALCAC_S *pstStaticGlobalCac)
{
    HI_S32 s32Ret = HI_SUCCESS;
    HI_CHAR *pszString = NULL;
    HI_S32 s32Value = 0;
    HI_CHAR aszIniNodeName[SCENE_INIPARAM_NODE_NAME_LEN] = {0,};

#ifdef CFG_STA_GLOBALCAC_ENABLE
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
#endif

#ifdef CFG_STA_GLOBALCAC_VERCOORDINATE
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
#endif

#ifdef CFG_STA_GLOBALCAC_HORCOORDINATE
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
#endif

#ifdef CFG_STA_GLOBALCAC_PARAMREDA
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
#endif

#ifdef CFG_STA_GLOBALCAC_PARAMREDB
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
#endif

#ifdef CFG_STA_GLOBALCAC_PARAMREDC
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
#endif

#ifdef CFG_STA_GLOBALCAC_PARAMBLUEA
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
#endif

#ifdef CFG_STA_GLOBALCAC_PARAMBLUEB
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
#endif

#ifdef CFG_STA_GLOBALCAC_PARAMBLUEC
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
#endif

#ifdef CFG_STA_GLOBALCAC_VERNORMSHIFT
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
#endif

#ifdef CFG_STA_GLOBALCAC_VERNORMFACTOR
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
#endif

#ifdef CFG_STA_GLOBALCAC_HORNORMSHIFT
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
#endif


#ifdef CFG_STA_GLOBALCAC_HORNORMFACTOR
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
#endif

#ifdef CFG_STA_GLOBALCAC_CORVARTHR
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
#endif

    return HI_SUCCESS;
}

HI_S32 SCENE_LoadStaticWdrExposure(const HI_CHAR *pszIniModule, HI_SCENE_STATIC_WDREXPOSURE_S *pstStaticWdrExposure)
{
    HI_S32 s32Ret = HI_SUCCESS;
    HI_CHAR *pszString = NULL;
    HI_U32 u32IdxM = 0;
    HI_S32 s32Value = 0;
    HI_CHAR aszIniNodeName[SCENE_INIPARAM_NODE_NAME_LEN] = {0,};

#ifdef CFG_STA_WDREXPOSURE_EXPRATIOTYPE
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
#endif

#ifdef CFG_STA_WDREXPOSURE_EXPRATIOMAX
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
#endif

#ifdef CFG_STA_WDREXPOSURE_EXPRATIOMIN
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
#endif

#ifdef CFG_STA_WDREXPOSURE_EXPRATIO
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
#endif

    return HI_SUCCESS;
}

HI_S32 SCENE_LoadStaticSaturation(const HI_CHAR *pszIniModule, HI_SCENE_STATIC_SATURATION_S *pstStaticSaturation)
{
#ifdef CFG_STA_STATURATION_AUTOSAT
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
#endif

    return HI_SUCCESS;
}

HI_S32 SCENE_LoadStaticLDCI(const HI_CHAR *pszIniModule, HI_SCENE_STATIC_LDCI_S *pstStaticLdci)
{
#ifdef CFG_STA_LDCI_BENABLE
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
#endif

#ifdef CFG_STA_LDCI_LDCIOPTYPE
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
#endif

#ifdef CFG_STA_LDCI_LDCIGAUSSLPFSIGMA
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
#endif

#ifdef CFG_STA_LDCI_MANUALBLCCTRL
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
#endif

#ifdef CFG_STA_LDCI_MANUALHEPOSSIGMA
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
#endif

#ifdef CFG_STA_LDCI_MANUALHEPOSMEAN
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
#endif

#ifdef CFG_STA_LDCI_MANUALHENEGSIGMA
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
#endif

#ifdef CFG_STA_LDCI_MANUALHENEGWGT
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
#endif

#ifdef CFG_STA_LDCI_MANUALHENEGMEAN
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

#ifdef CFG_STA_DRC_BENABLE
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
#endif

#ifdef CFG_STA_DRC_CURVESELECT
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
#endif

#ifdef CFG_STA_DRC_DRCOPTYPE
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
#endif

#ifdef CFG_STA_DRC_DRCAUTOSTR
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
#endif

#ifdef CFG_STA_DRC_DRCAUTOSTRMIN
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
#endif


#ifdef CFG_STA_DRC_DRCAUTOSTRMAX
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
#endif

#ifdef CFG_STA_DRC_DRCTONEMAPPINGVALUE
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
#endif

    return HI_SUCCESS;
}

HI_S32 SCENE_LoadStaticDehaze(const HI_CHAR *pszIniModule, HI_SCENE_STATIC_DEHAZE_S *pstStaticDeHaze)
{
    HI_S32 s32Ret = HI_SUCCESS;
    HI_CHAR *pszString = NULL;
    HI_U32 u32IdxM = 0;
    HI_S32 s32Value = 0;
    HI_CHAR aszIniNodeName[SCENE_INIPARAM_NODE_NAME_LEN] = {0,};

#ifdef CFG_STA_DEHAZE_BENABLE
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
#endif


#ifdef CFG_STA_DEHAZE_DEHAZEOPTYPE
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
#endif

#ifdef CFG_STA_DEHAZE_BDEHAZEUSERLUTENABLE
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
#endif

#ifdef CFG_STA_DEHAZE_DEHAZELUT
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
#endif

    return HI_SUCCESS;
}

HI_S32 SCENE_LoadStaticStatistics(const HI_CHAR *pszIniModule, HI_SCENE_STATIC_STATISTICSCFG_S *pstStaticStatistics)
{
#ifdef CFG_STA_STATISTICS_EXPWEIGHT

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
#endif
    return HI_SUCCESS;
}

HI_S32 SCENE_LoadStaticThreeDNR(const HI_CHAR *pszIniModule, HI_SCENE_STATIC_THREEDNR_S *pstStaticThreeDnr)
{
    HI_S32 s32Ret = HI_SUCCESS;
    HI_CHAR *pszString = NULL;
    HI_U32 u32IdxM = 0;
    HI_S32 s32Value = 0;
    HI_CHAR aszIniNodeName[SCENE_INIPARAM_NODE_NAME_LEN] = {0,};

#ifdef CFG_STA_THREEDNR_THREEDNRCOUNT
    snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "static_threednr:ThreeDNRCount");/*ThreeDNRCount*/
    s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
    SCENE_INIPARAM_CHECK_LOAD_RESULT(s32Ret, aszIniNodeName);
    if (HI_NULL != pszString)
    {
        free(pszString);
        pszString = HI_NULL;
        s32Ret = HI_CONFACCESS_GetInt(SCENE_INIPARAM, pszIniModule, "static_threednr:ThreeDNRCount", 0, &s32Value);
        if (HI_SUCCESS != s32Ret)
        {
            MLOGE("load static_threednr:ThreeDNRCount failed\n");
            return HI_FAILURE;
        }
        pstStaticThreeDnr->u32ThreeDNRCount = (HI_U32)s32Value;
    }
#endif

#ifdef CFG_STA_THREEDNR_ISOTHRESH
    snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "static_threednr:IsoThresh");/*IsoThresh*/
    s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
    SCENE_INIPARAM_CHECK_LOAD_RESULT(s32Ret, aszIniNodeName);
    if (HI_NULL != pszString)
    {
        SCENE_GetNumbersInOneLine(pszString);
        for (u32IdxM = 0; u32IdxM < pstStaticThreeDnr->u32ThreeDNRCount; u32IdxM++)
        {
            pstStaticThreeDnr->au32ThreeDNRIso[u32IdxM] = s_as64LineNum[u32IdxM];
        }
        free(pszString);
        pszString = HI_NULL;
    }
#endif

#ifdef CFG_STA_THREEDNR_3DNRPARAM
    snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "static_threednr:3DnrParam_0");/*3DnrParam_0*/
    s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
    SCENE_INIPARAM_CHECK_LOAD_RESULT(s32Ret, aszIniNodeName);
    if (HI_NULL != pszString)
    {
        free(pszString);
        pszString = HI_NULL;
        for (u32IdxM = 0; u32IdxM < pstStaticThreeDnr->u32ThreeDNRCount; u32IdxM++)
        {
            snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "static_threednr:3DnrParam_%d", u32IdxM);
            s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
            if (HI_SUCCESS != s32Ret)
            {
                MLOGE("load %s failed\n", aszIniNodeName);
                return HI_FAILURE;
            }
            if (NULL != pszString)
            {
                HI_SCENE_3DNR_S *pX = &(pstStaticThreeDnr->astThreeDNRValue[u32IdxM]);
                tV59aSceIEy  *pI = &( pX->IEy );
                tV59aSceSFy  *pS =    pX->SFy;
                tV59aSceMDy  *pM =    pX->MDy;
                tV59aSceTFy  *pT =    pX->TFy;
                HI_S32  *pB2 = pX->SBSk2;
                HI_S32  *pD2 = pX->SDSk2;
                HI_S32  *pB3 = pX->SBSk3;
                HI_S32  *pD3 = pX->SDSk3;
                HI_S32  *pBT = pX->BriThr;

                sscanf(pszString,
                       "  IES       %4d      |                 |                 |                  "
                       "  IESS      %4d      |                 |                 |                  "
                       "  IEDZ      %4d      |                 |                 |                  "
                       "                      |                 |                 |                  "
                       "  SFT             %3d |             %3d |             %3d |             %3d  "
                       "  SBS  %4d:%4d:%4d |  %4d:%4d:%4d |  %4d:%4d:%4d |  %4d:%4d:%4d  "
                       "  SDS  %4d:%4d:%4d |  %4d:%4d:%4d |  %4d:%4d:%4d |  %4d:%4d:%4d  "
                       "  STH  %4d:%4d:%4d |  %4d:%4d:%4d |  %4d:%4d:%4d |  %4d:%4d:%4d  "
                       "  STHp            %3d |             %3d |             %3d |                  "
                       "  SBF             %3d |             %3d |             %3d |             %3d  "
                       "  SFRi                |                 |                 | %3d:%3d:%3d:%3d  "
                       "                      |                 |                 |                  "
                       "  kPro                |                 |             %3d |             %3d  "
                       "  kTab                |                 |             %3d |             %3d  "
                       "                      |                 |                 |                  "
                       "  kSFT                |                 |                 |             %3d  "
                       "  kSBS                |                 |                 |  %4d:%4d:%4d  "
                       "  kSDS                |                 |                 |  %4d:%4d:%4d  "
                       "  kSTH                |                 |                 |  %4d:%4d:%4d  "
                       "                      |                 |                 |                  "
                       "  SBFk                |                 |                 |             %3d  "
                       "  SFRk                |                 |                 | %3d:%3d:%3d:%3d  "
                       "                      |                 |                 |                  "
                       "  nRef  -ref  %3d  ---+--->          ---+--->             | HdgType -ht %3d  "
                       "                      |                 |                 | HdgMode -hm %3d  "
                       "  STR             %3d |             %3d |             %3d | HdgIES  -hi %3d  "
                       "                      |                 |                 | HdgWnd  -hw %3d  "
                       "  TFS                 |             %3d |             %3d | HdgSFR  -hs %3d  "
                       "  TDZ                 |             %3d |             %3d |                  "
                       "  TDX                 |             %3d |             %3d | BriType -bt %3d  "
                       "  TFR                 | %3d:%3d:%3d:%3d | %3d:%3d:%3d:%3d |                  "
                       "                      |                 |                 | nOut  -out  %3d  "
                       "  MATH                |            %4d |            %4d +------------------"
                       "  MATE                |            %4d |            %4d | sfc  %4d tfc  %2d"
                       "  MABW                |            %4d |            %4d |                  "
                       "  MATW                |            %4d |            %4d | csfs %4d ctfs %2d"
                       "  MASW                |            %4d |            %4d | csfk %4d ctfp %2d"
                       "  MAXN                |            %4d |            %4d | ciir %4d ctfr %2d"
                       "*****************************************************************************"
                       "                                                                             "
                       "  SBSk2  -k2b0    %6d %6d %6d %6d    %6d %6d %6d %6d "
                       "         -k2b8    %6d %6d %6d %6d    %6d %6d %6d %6d "
                       "         -k2b16   %6d %6d %6d %6d    %6d %6d %6d %6d "
                       "         -k2b24   %6d %6d %6d %6d    %6d %6d %6d %6d "
                       "                                                                             "
                       "  SDSk2  -k2d0    %6d %6d %6d %6d    %6d %6d %6d %6d "
                       "         -k2d8    %6d %6d %6d %6d    %6d %6d %6d %6d "
                       "         -k2d16   %6d %6d %6d %6d    %6d %6d %6d %6d "
                       "         -k2d24   %6d %6d %6d %6d    %6d %6d %6d %6d "
                       "                                                                             "
                       "  SBSk3  -k3b0    %6d %6d %6d %6d    %6d %6d %6d %6d "
                       "         -k3b8    %6d %6d %6d %6d    %6d %6d %6d %6d "
                       "         -k3b16   %6d %6d %6d %6d    %6d %6d %6d %6d "
                       "         -k3b24   %6d %6d %6d %6d    %6d %6d %6d %6d "
                       "                                                                             "
                       "  SDSk3  -k3d0    %6d %6d %6d %6d    %6d %6d %6d %6d "
                       "         -k3d8    %6d %6d %6d %6d    %6d %6d %6d %6d "
                       "         -k3d16   %6d %6d %6d %6d    %6d %6d %6d %6d "
                       "         -k3d24   %6d %6d %6d %6d    %6d %6d %6d %6d "
                       "                                                                             "
                       "  BriThr  -bth0   %6d %6d %6d %6d    %6d %6d %6d %6d "
                       "          -bth8   %6d %6d %6d %6d    %6d %6d %6d %6d ",
                       &pI->IES,
                       &pI->IESS,
                       &pI->IEDZ,
                       &pS[0].SFT, &pS[1].SFT, &pS[2].SFT, &pS[3].SFT,
                       &pS[0].SBS[0], &pS[0].SBS[1], &pS[0].SBS[2],
                       &pS[1].SBS[0], &pS[1].SBS[1], &pS[1].SBS[2],
                       &pS[2].SBS[0], &pS[2].SBS[1], &pS[2].SBS[2],
                       &pS[3].SBS[0], &pS[3].SBS[1], &pS[3].SBS[2],
                       &pS[0].SDS[0], &pS[0].SDS[1], &pS[0].SDS[2],
                       &pS[1].SDS[0], &pS[1].SDS[1], &pS[1].SDS[2],
                       &pS[2].SDS[0], &pS[2].SDS[1], &pS[2].SDS[2],
                       &pS[3].SDS[0], &pS[3].SDS[1], &pS[3].SDS[2],
                       &pS[0].STH[0], &pS[0].STH[1], &pS[0].STH[2],
                       &pS[1].STH[0], &pS[1].STH[1], &pS[1].STH[2],
                       &pS[2].STH[0], &pS[2].STH[1], &pS[2].STH[2],
                       &pS[3].STH[0], &pS[3].STH[1], &pS[3].STH[2],
                       &pS[0].STHp, &pS[1].STHp, &pS[2].STHp,
                       &pS[0].SBF,  &pS[1].SBF,  &pS[2].SBF,  &pS[3].SBF,
                       &pX->SFRi[0], &pX->SFRi[1], &pX->SFRi[2], &pX->SFRi[3],
                       &pS[2].kPro,  &pS[4].kPro,
                       &pX->kTab2, &pX->kTab3,
                       &pS[4].SFT,
                       &pS[4].SBS[0], &pS[4].SBS[1], &pS[4].SBS[2],
                       &pS[4].SDS[0], &pS[4].SDS[1], &pS[4].SDS[2],
                       &pS[4].STH[0], &pS[4].STH[1], &pS[4].STH[2],
                       &pS[4].SBF,
                       &pX->SFRk[0], &pX->SFRk[1], &pX->SFRk[2], &pX->SFRk[3],
                       &pX->nRef,                     &pX->HdgType,
                       &pX->HdgMode,
                       &pS[0].STR, &pS[1].STR, &pS[2].STR, &pX->HdgIES,
                       &pX->HdgWnd,
                       &pT[0].TFS, &pT[1].TFS,          &pX->HdgSFR,
                       &pT[0].TDZ, &pT[1].TDZ,
                       &pT[0].TDX, &pT[1].TDX,          &pX->BriType,
                       &pT[0].TFR[0], &pT[0].TFR[1], &pT[0].TFR[2], &pT[0].TFR[3],
                       &pT[1].TFR[0], &pT[1].TFR[1], &pT[1].TFR[2], &pT[1].TFR[3],
                       &pX->nOut,
                       &pM[0].MATH, &pM[1].MATH,
                       &pM[0].MATE, &pM[1].MATE, &pX->NRc.SFC, &pX->NRc.TFC,
                       &pM[0].MABW, &pM[1].MABW,
                       &pM[0].MATW, &pM[1].MATW, &pX->NRc.CSFS, &pX->NRc.CTFS,
                       &pM[0].MASW, &pM[1].MASW, &pX->NRc.CSFk, &pX->NRc.TFC,
                       &pM[0].MAXN, &pM[1].MAXN, &pX->NRc.CIIR, &pX->NRc.CTFR,
                       &pB2[0x00], &pB2[0x01], &pB2[0x02], &pB2[0x03], &pB2[0x04], &pB2[0x05], &pB2[0x06], &pB2[0x07],
                       &pB2[0x08], &pB2[0x09], &pB2[0x0A], &pB2[0x0B], &pB2[0x0C], &pB2[0x0D], &pB2[0x0E], &pB2[0x0F],
                       &pB2[0x10], &pB2[0x11], &pB2[0x12], &pB2[0x13], &pB2[0x14], &pB2[0x15], &pB2[0x16], &pB2[0x17],
                       &pB2[0x18], &pB2[0x19], &pB2[0x1A], &pB2[0x1B], &pB2[0x1C], &pB2[0x1D], &pB2[0x1E], &pB2[0x1F],
                       &pD2[0x00], &pD2[0x01], &pD2[0x02], &pD2[0x03], &pD2[0x04], &pD2[0x05], &pD2[0x06], &pD2[0x07],
                       &pD2[0x08], &pD2[0x09], &pD2[0x0A], &pD2[0x0B], &pD2[0x0C], &pD2[0x0D], &pD2[0x0E], &pD2[0x0F],
                       &pD2[0x10], &pD2[0x11], &pD2[0x12], &pD2[0x13], &pD2[0x14], &pD2[0x15], &pD2[0x16], &pD2[0x17],
                       &pD2[0x18], &pD2[0x19], &pD2[0x1A], &pD2[0x1B], &pD2[0x1C], &pD2[0x1D], &pD2[0x1E], &pD2[0x1F],
                       &pB3[0x00], &pB3[0x01], &pB3[0x02], &pB3[0x03], &pB3[0x04], &pB3[0x05], &pB3[0x06], &pB3[0x07],
                       &pB3[0x08], &pB3[0x09], &pB3[0x0A], &pB3[0x0B], &pB3[0x0C], &pB3[0x0D], &pB3[0x0E], &pB3[0x0F],
                       &pB3[0x10], &pB3[0x11], &pB3[0x12], &pB3[0x13], &pB3[0x14], &pB3[0x15], &pB3[0x16], &pB3[0x17],
                       &pB3[0x18], &pB3[0x19], &pB3[0x1A], &pB3[0x1B], &pB3[0x1C], &pB3[0x1D], &pB3[0x1E], &pB3[0x1F],
                       &pD3[0x00], &pD3[0x01], &pD3[0x02], &pD3[0x03], &pD3[0x04], &pD3[0x05], &pD3[0x06], &pD3[0x07],
                       &pD3[0x08], &pD3[0x09], &pD3[0x0A], &pD3[0x0B], &pD3[0x0C], &pD3[0x0D], &pD3[0x0E], &pD3[0x0F],
                       &pD3[0x10], &pD3[0x11], &pD3[0x12], &pD3[0x13], &pD3[0x14], &pD3[0x15], &pD3[0x16], &pD3[0x17],
                       &pD3[0x18], &pD3[0x19], &pD3[0x1A], &pD3[0x1B], &pD3[0x1C], &pD3[0x1D], &pD3[0x1E], &pD3[0x1F],
                       &pBT[0x00], &pBT[0x01], &pBT[0x02], &pBT[0x03], &pBT[0x04], &pBT[0x05], &pBT[0x06], &pBT[0x07],
                       &pBT[0x08], &pBT[0x09], &pBT[0x0A], &pBT[0x0B], &pBT[0x0C], &pBT[0x0D], &pBT[0x0E], &pBT[0x0F]
                      );

                free(pszString);
                pszString = HI_NULL;
            }
        }
    }
#endif

    return HI_SUCCESS;
}


HI_S32 SCENE_LoadStaticCSC(const HI_CHAR *pszIniModule, HI_SCENE_STATIC_CSC_S *pstStaticCSC)
{

#ifdef CFG_STA_CSC_ENABLE
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
#endif

#ifdef CFG_STA_CSC_HUE
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
#endif

#ifdef CFG_STA_CSC_LUMA
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
#endif

#ifdef CFG_STA_CSC_CONTRAST
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
#endif

#ifdef CFG_STA_CSC_SATURATION
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
#endif

#ifdef CFG_STA_CSC_COLORGAMUT
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

HI_S32 SCENE_LoadStaticNr(const HI_CHAR *pszIniModule, HI_SCENE_STATIC_NR_S *pstStaticNr)
{
#ifdef CFG_STA_NR_ENABLE
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
#endif

    return HI_SUCCESS;
}

HI_S32 SCENE_LoadStaticShading(const HI_CHAR *pszIniModule, HI_SCENE_STATIC_SHADING_S *pstStaticShading)
{
    HI_S32 s32Ret = HI_SUCCESS;
    HI_CHAR *pszString = NULL;
    HI_S32 s32Value = 0;
    HI_CHAR aszIniNodeName[SCENE_INIPARAM_NODE_NAME_LEN] = {0,};

#ifdef CFG_STA_SHARDING_ENABLE
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
#endif

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

#ifdef CFG_DYM_GAMMA_TOTALNUM
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
#endif

#ifdef CFG_DYM_GAMMA_INTERVAL
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
#endif

#ifdef CFG_DYM_GAMMA_EXPTHRESHLTOH
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
#endif

#ifdef CFG_DYM_GAMMA_EXPTHRESHHTOL
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
#endif

#ifdef CFG_DYM_GAMMA_EXPTHRESH
    snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "dynamic_gamma:gammaExpThresh");/*GammaExpThresh*/
    s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
    SCENE_INIPARAM_CHECK_LOAD_RESULT(s32Ret, aszIniNodeName);
    if (HI_NULL != pszString)
    {
        SCENE_GetNumbersInOneLine(pszString);
        for (u32IdxM = 0; u32IdxM < pstDynamicGamma->u32TotalNum; u32IdxM++)
        {
            pstDynamicGamma->au32GammaExpThresh[u32IdxM] = s_as64LineNum[u32IdxM];
        }
        free(pszString);
        pszString = HI_NULL;
    }
#endif

#ifdef CFG_DYM_GAMMA_TABLE
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
#endif

    return HI_SUCCESS;
}

HI_S32 SCENE_LoadDynamicShading(const HI_CHAR *pszIniModule, HI_SCENE_DYNAMIC_SHADING_S *pstDynamicShading)
{
    HI_S32 s32Ret = HI_SUCCESS;
    HI_CHAR *pszString = NULL;
    HI_S32 s32Value = 0;
    HI_U32 u32IdxM = 0;
    HI_CHAR aszIniNodeName[SCENE_INIPARAM_NODE_NAME_LEN] = {0,};

#ifdef CFG_DYM_SHADING_EXPTHRESHCNT
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
#endif

#ifdef CFG_DYM_SHADING_EXPTHRESHLTOH
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
#endif

#ifdef CFG_DYM_SHADING_MANUALSTRENGTH
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
#endif

    return HI_SUCCESS;
}

HI_S32 SCENE_LoadDynamicLDCI(const HI_CHAR *pszIniModule, HI_SCENE_DYNAMIC_LDCI_S *pstDynamicLdci)
{
    HI_S32 s32Ret = HI_SUCCESS;
    HI_CHAR *pszString = NULL;
    HI_S32 s32Value = 0;
    HI_U32 u32IdxM = 0;
    HI_CHAR aszIniNodeName[SCENE_INIPARAM_NODE_NAME_LEN] = {0,};

#ifdef CFG_DYM_LDCI_ENABLECNT
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
#endif

#ifdef CFG_DYM_LDCI_ENABLEEXPTHRESHLTOH
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
#endif

#ifdef CFG_DYM_LDCI_ENABLE
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
#endif

#ifdef CFG_DYM_LDCI_EXPTHRESHCNT
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
#endif

#ifdef CFG_DYM_LDCI_XPTHRESHLTOH
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
#endif

#ifdef CFG_DYM_LDCI_MANUALHEPOSWGT
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
#endif
    return HI_SUCCESS;
}

HI_S32 SCENE_LoadDynamicDehaze(const HI_CHAR *pszIniModule, HI_SCENE_DYNAMIC_DEHAZE_S *pstDynamicDehaze)
{
    HI_S32 s32Ret = HI_SUCCESS;
    HI_CHAR *pszString = NULL;
    HI_S32 s32Value = 0;
    HI_U32 u32IdxM = 0;
    HI_CHAR aszIniNodeName[SCENE_INIPARAM_NODE_NAME_LEN] = {0,};

#ifdef CFG_DYM_DEHAZE_EXPTHRESHCNT
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
#endif

#ifdef CFG_DYM_DEHAZE_EXPTHRESHLTOH
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
#endif

#ifdef CFG_DYM_DEHAZE_MANUALDEHAZESTR
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
#endif
    return HI_SUCCESS;
}

HI_S32 SCENE_LoadDynamicFsWdr(const HI_CHAR *pszIniModule, HI_SCENE_DYNAMIC_FSWDR_S *pstDynamicFsWdr)
{
    HI_S32 s32Ret = HI_SUCCESS;
    HI_CHAR *pszString = NULL;
    HI_S32 s32Value = 0;
    HI_U32 u32IdxM = 0;
    HI_CHAR aszIniNodeName[SCENE_INIPARAM_NODE_NAME_LEN] = {0,};

#ifdef CFG_DYM_FSWDR_ISOCNT
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
#endif

#ifdef CFG_DYM_FSWDR_EXPRATION
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
#endif

#ifdef CFG_DYM_FSWDR_MOTIONCOMP
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
#endif

#ifdef CFG_DYM_FSWDR_ISOTHRESHLTOH
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
#endif

#ifdef CFG_DYM_FSWDR_BNRSTR
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
#endif

#ifdef CFG_DYM_FSWDR_BNRMODE
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
#endif
    return HI_SUCCESS;
}

HI_S32 SCENE_LoadDynamicHdrOETF(const HI_CHAR *pszIniModule, HI_SCENE_DYNAMIC_HDROETF_S *pstDynamicHdrOetf)
{
#ifdef CFG_DYM_HDROTEF_HDRCNT
    HI_S32 s32Ret = HI_SUCCESS;
    HI_CHAR *pszString = NULL;
    HI_S32 s32Value = 0;
    HI_U32 u32IdxM = 0;
    HI_CHAR aszIniNodeName[SCENE_INIPARAM_NODE_NAME_LEN] = {0,};
    snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "dynamic_hdroetf:HDRCount");/*HDRCount*/
    s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
    SCENE_INIPARAM_CHECK_LOAD_RESULT(s32Ret, aszIniNodeName);
    if (HI_NULL != pszString)
    {
        free(pszString);
        pszString = HI_NULL;
        s32Ret = HI_CONFACCESS_GetInt(SCENE_INIPARAM, pszIniModule, "dynamic_hdroetf:HDRCount", 0, &s32Value);
        if (HI_SUCCESS != s32Ret)
        {
            MLOGE("load dynamic_hdroetf:HDRCount failed\n");
            return HI_FAILURE;
        }
        pstDynamicHdrOetf->u32HDRCount = (HI_U32)s32Value;
    }
#endif

#ifdef CFG_DYM_HDROTEF_BRIGHTRATIONTHRESH
    snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "dynamic_hdroetf:BrightRatioThresh");/*BrightRatioThresh*/
    s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
    SCENE_INIPARAM_CHECK_LOAD_RESULT(s32Ret, aszIniNodeName);
    if (HI_NULL != pszString)
    {
        SCENE_GetNumbersInOneLine(pszString);
        for (u32IdxM = 0; u32IdxM < pstDynamicHdrOetf->u32HDRCount; u32IdxM++)
        {
            pstDynamicHdrOetf->au32BrightRatioThresh[u32IdxM] = s_as64LineNum[u32IdxM];
        }
        free(pszString);
        pszString = HI_NULL;
    }
#endif

#ifdef CFG_DYM_HDROTEF_ENABLE
    snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "dynamic_hdroetf:bEnable");/*bEnable*/
    s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
    SCENE_INIPARAM_CHECK_LOAD_RESULT(s32Ret, aszIniNodeName);
    if (HI_NULL != pszString)
    {
        free(pszString);
        pszString = HI_NULL;
        s32Ret = HI_CONFACCESS_GetInt(SCENE_INIPARAM, pszIniModule, "dynamic_hdroetf:bEnable", 0, &s32Value);
        if (HI_SUCCESS != s32Ret)
        {
            MLOGE("load dynamic_hdroetf:bEnable failed\n");
            return HI_FAILURE;
        }
        pstDynamicHdrOetf->bEnable = (HI_BOOL)s32Value;
    }
#endif

#ifdef CFG_DYM_HDROTEF_MAXLUM
    snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "dynamic_hdroetf:MaxLum");/*MaxLum*/
    s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
    SCENE_INIPARAM_CHECK_LOAD_RESULT(s32Ret, aszIniNodeName);
    if (HI_NULL != pszString)
    {
        SCENE_GetNumbersInOneLine(pszString);
        for (u32IdxM = 0; u32IdxM < pstDynamicHdrOetf->u32HDRCount; u32IdxM++)
        {
            pstDynamicHdrOetf->au32MaxLum[u32IdxM] = s_as64LineNum[u32IdxM];
        }
        free(pszString);
        pszString = HI_NULL;
    }
#endif

#ifdef CFG_DYM_HDROTEF_CURLUM
    snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "dynamic_hdroetf:CurLum");/*CurLum*/
    s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
    SCENE_INIPARAM_CHECK_LOAD_RESULT(s32Ret, aszIniNodeName);
    if (HI_NULL != pszString)
    {
        SCENE_GetNumbersInOneLine(pszString);
        for (u32IdxM = 0; u32IdxM < pstDynamicHdrOetf->u32HDRCount; u32IdxM++)
        {
            pstDynamicHdrOetf->au32CurLum[u32IdxM] = s_as64LineNum[u32IdxM];
        }
        free(pszString);
        pszString = HI_NULL;
    }
#endif
    return HI_SUCCESS;
}

HI_S32 SCENE_LoadDynamicHdrTm(const HI_CHAR *pszIniModule, HI_SCENE_DYNAMIC_HDRTM_S *pstDynamicHdrTm)
{
#ifdef CFG_DYM_HDRTM_HDRCNT
    HI_S32 s32Ret = HI_SUCCESS;
    HI_CHAR *pszString = NULL;
    HI_S32 s32Value = 0;
    HI_U32 u32IdxM = 0;
    HI_U32 u32IdxN = 0;
    HI_CHAR aszIniNodeName[SCENE_INIPARAM_NODE_NAME_LEN] = {0,};
    snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "dynamic_hdrtm:HDRCount");/*HDRCount*/
    s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
    SCENE_INIPARAM_CHECK_LOAD_RESULT(s32Ret, aszIniNodeName);
    if (HI_NULL != pszString)
    {
        free(pszString);
        pszString = HI_NULL;
        s32Ret = HI_CONFACCESS_GetInt(SCENE_INIPARAM, pszIniModule, "dynamic_hdrtm:HDRCount", 0, &s32Value);
        if (HI_SUCCESS != s32Ret)
        {
            MLOGE("load dynamic_hdroetf:HDRCount failed\n");
            return HI_FAILURE;
        }
        pstDynamicHdrTm->u32HDRCount = (HI_U32)s32Value;
    }
#endif

#ifdef CFG_DYM_HDRTM_BRIGHTRATIOTHRESH
    snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "dynamic_hdrtm:BrightRatioThresh");/*BrightRatioThresh*/
    s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
    SCENE_INIPARAM_CHECK_LOAD_RESULT(s32Ret, aszIniNodeName);
    if (HI_NULL != pszString)
    {
        SCENE_GetNumbersInOneLine(pszString);
        for (u32IdxM = 0; u32IdxM < pstDynamicHdrTm->u32HDRCount; u32IdxM++)
        {
            pstDynamicHdrTm->au32BrightRatioThresh[u32IdxM] = s_as64LineNum[u32IdxM];
        }
        free(pszString);
        pszString = HI_NULL;
    }
#endif

#ifdef CFG_DYM_HDRTM_ENABLE
    snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "dynamic_hdrtm:bEnable");/*bEnable*/
    s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
    SCENE_INIPARAM_CHECK_LOAD_RESULT(s32Ret, aszIniNodeName);
    {
        free(pszString);
        pszString = HI_NULL;
        s32Ret = HI_CONFACCESS_GetInt(SCENE_INIPARAM, pszIniModule, "dynamic_hdrtm:bEnable", 0, &s32Value);
        if (HI_SUCCESS != s32Ret)
        {
            MLOGE("load dynamic_hdroetf:bEnable failed\n");
            return HI_FAILURE;
        }
        pstDynamicHdrTm->bEnable = (HI_BOOL)s32Value;
    }
#endif

#ifdef CFG_DYM_HDRTM_TMCUBIC
    snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "dynamic_hdrtm:TMCubicX_0");/*TMCubicX_0*/
    s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
    SCENE_INIPARAM_CHECK_LOAD_RESULT(s32Ret, aszIniNodeName);
    {
        free(pszString);
        pszString = HI_NULL;
        for (u32IdxM = 0; u32IdxM < 5; u32IdxM++)
        {
            snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "dynamic_hdrtm:TMCubicX_%d", u32IdxM);
            s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
            if (HI_SUCCESS != s32Ret)
            {
                MLOGE("load %s failed\n", aszIniNodeName);
                return HI_FAILURE;
            }
            if (NULL != pszString)
            {
                SCENE_GetNumbersInOneLine(pszString);
                for (u32IdxN = 0; u32IdxN < pstDynamicHdrTm->u32HDRCount; u32IdxN++)
                {
                    pstDynamicHdrTm->astCubicPoint[u32IdxM][u32IdxN].u16X = s_as64LineNum[u32IdxN];
                }
                free(pszString);
                pszString = HI_NULL;
            }
        }

        for (u32IdxM = 0; u32IdxM < 5; u32IdxM++)
        {
            snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "dynamic_hdrtm:TMCubicY_%d", u32IdxM);
            s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
            if (HI_SUCCESS != s32Ret)
            {
                MLOGE("load %s failed\n", aszIniNodeName);
                return HI_FAILURE;
            }
            if (NULL != pszString)
            {
                SCENE_GetNumbersInOneLine(pszString);
                for (u32IdxN = 0; u32IdxN < pstDynamicHdrTm->u32HDRCount; u32IdxN++)
                {
                    pstDynamicHdrTm->astCubicPoint[u32IdxM][u32IdxN].u16Y = s_as64LineNum[u32IdxN];
                }
                free(pszString);
                pszString = HI_NULL;
            }
        }

        for (u32IdxM = 0; u32IdxM < 5; u32IdxM++)
        {
            snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "dynamic_hdrtm:TMCubicSlope_%d", u32IdxM);
            s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
            if (HI_SUCCESS != s32Ret)
            {
                MLOGE("load %s failed\n", aszIniNodeName);
                return HI_FAILURE;
            }
            if (NULL != pszString)
            {
                SCENE_GetNumbersInOneLine(pszString);
                for (u32IdxN = 0; u32IdxN < pstDynamicHdrTm->u32HDRCount; u32IdxN++)
                {
                    pstDynamicHdrTm->astCubicPoint[u32IdxM][u32IdxN].u16Slope = s_as64LineNum[u32IdxN];
                }
                free(pszString);
                pszString = HI_NULL;
            }
        }
    }
#endif
    return HI_SUCCESS;
}

HI_S32 SCENE_LoadDynamicHdrDrc(const HI_CHAR *pszIniModule, HI_SCENE_DYNAMIC_HDRDRC_S *pstDynamicHdrDrc)
{

#ifdef CFG_DYM_HDRDRC_HDRCNT
    HI_S32 s32Ret = HI_SUCCESS;
    HI_CHAR *pszString = NULL;
    HI_S32 s32Value = 0;
    HI_U32 u32IdxM = 0;
    HI_U32 u32IdxN = 0;
    HI_CHAR aszIniNodeName[SCENE_INIPARAM_NODE_NAME_LEN] = {0,};
    snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "dynamic_hdrdrc:HDRCount");/*HDRCount*/
    s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
    SCENE_INIPARAM_CHECK_LOAD_RESULT(s32Ret, aszIniNodeName);
    if (HI_NULL != pszString)
    {
        free(pszString);
        pszString = HI_NULL;
        s32Ret = HI_CONFACCESS_GetInt(SCENE_INIPARAM, pszIniModule, "dynamic_hdrdrc:HDRCount", 0, &s32Value);
        if (HI_SUCCESS != s32Ret)
        {
            MLOGE("load dynamic_hdroetf:HDRCount failed\n");
            return HI_FAILURE;
        }
        pstDynamicHdrDrc->u32HDRCount = (HI_U32)s32Value;
    }
#endif

#ifdef CFG_DYM_HDRDRC_BRIGHTRATIOTHRESH
    snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "dynamic_hdrdrc:BrightRatioThresh");/*BrightRatioThresh*/
    s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
    SCENE_INIPARAM_CHECK_LOAD_RESULT(s32Ret, aszIniNodeName);
    if (HI_NULL != pszString)
    {
        SCENE_GetNumbersInOneLine(pszString);
        for (u32IdxM = 0; u32IdxM < pstDynamicHdrDrc->u32HDRCount; u32IdxM++)
        {
            pstDynamicHdrDrc->au32BrightRatioThresh[u32IdxM] = s_as64LineNum[u32IdxM];
        }
        free(pszString);
        pszString = HI_NULL;
    }
#endif

#ifdef CFG_DYM_HDRDRC_ENABLE
    snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "dynamic_hdrdrc:bEnable");/*bEnable*/
    s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
    SCENE_INIPARAM_CHECK_LOAD_RESULT(s32Ret, aszIniNodeName);
    {
        free(pszString);
        pszString = HI_NULL;
        s32Ret = HI_CONFACCESS_GetInt(SCENE_INIPARAM, pszIniModule, "dynamic_hdrdrc:bEnable", 0, &s32Value);
        if (HI_SUCCESS != s32Ret)
        {
            MLOGE("load dynamic_hdrdrc:bEnable failed\n");
            return HI_FAILURE;
        }
        pstDynamicHdrDrc->bEnable = (HI_BOOL)s32Value;
    }
#endif

#ifdef CFG_DYM_HDRDRC_DRCCUBIC
    snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "dynamic_hdrdrc:DRCCubicX_0");/*TMCubicX_0*/
    s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
    SCENE_INIPARAM_CHECK_LOAD_RESULT(s32Ret, aszIniNodeName);
    {
        free(pszString);
        pszString = HI_NULL;
        for (u32IdxM = 0; u32IdxM < 5; u32IdxM++)
        {
            snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "dynamic_hdrdrc:DRCCubicX_%d", u32IdxM);
            s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
            if (HI_SUCCESS != s32Ret)
            {
                MLOGE("load %s failed\n", aszIniNodeName);
                return HI_FAILURE;
            }
            if (NULL != pszString)
            {
                SCENE_GetNumbersInOneLine(pszString);
                for (u32IdxN = 0; u32IdxN < pstDynamicHdrDrc->u32HDRCount; u32IdxN++)
                {
                    pstDynamicHdrDrc->astCubicPoint[u32IdxM][u32IdxN].u16X = s_as64LineNum[u32IdxN];
                }
                free(pszString);
                pszString = HI_NULL;
            }
        }

        for (u32IdxM = 0; u32IdxM < 5; u32IdxM++)
        {
            snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "dynamic_hdrdrc:DRCCubicY_%d", u32IdxM);
            s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
            if (HI_SUCCESS != s32Ret)
            {
                MLOGE("load %s failed\n", aszIniNodeName);
                return HI_FAILURE;
            }
            if (NULL != pszString)
            {
                SCENE_GetNumbersInOneLine(pszString);
                for (u32IdxN = 0; u32IdxN < pstDynamicHdrDrc->u32HDRCount; u32IdxN++)
                {
                    pstDynamicHdrDrc->astCubicPoint[u32IdxM][u32IdxN].u16Y = s_as64LineNum[u32IdxN];
                }
                free(pszString);
                pszString = HI_NULL;
            }
        }

        for (u32IdxM = 0; u32IdxM < 5; u32IdxM++)
        {
            snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "dynamic_hdrdrc:DRCCubicSlope_%d", u32IdxM);
            s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
            if (HI_SUCCESS != s32Ret)
            {
                MLOGE("load %s failed\n", aszIniNodeName);
                return HI_FAILURE;
            }
            if (NULL != pszString)
            {
                SCENE_GetNumbersInOneLine(pszString);
                for (u32IdxN = 0; u32IdxN < pstDynamicHdrDrc->u32HDRCount; u32IdxN++)
                {
                    pstDynamicHdrDrc->astCubicPoint[u32IdxM][u32IdxN].u16Slope = s_as64LineNum[u32IdxN];
                }
                free(pszString);
                pszString = HI_NULL;
            }
        }
    }
#endif
    return HI_SUCCESS;
}

HI_S32 SCENE_LoadDynamicAE(const HI_CHAR *pszIniModule, HI_SCENE_DYNAMIC_AE_S *pstDynamicAe)
{
    HI_S32 s32Ret = HI_SUCCESS;
    HI_CHAR *pszString = NULL;
    HI_S32 s32Value = 0;
    HI_U32 u32IdxM = 0;
    HI_CHAR aszIniNodeName[SCENE_INIPARAM_NODE_NAME_LEN] = {0,};

#ifdef CFG_DYM_AE_EXPCNT
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
#endif

#ifdef CFG_DYM_AE_EXPTHRESHLTOH
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
#endif

#ifdef CFG_DYM_AE_EXPTHRESHHTOL
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
#endif

#ifdef CFG_DYM_AE_COMPENSATION
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
#endif

#ifdef CFG_DYM_AE_AUTOHISTOFFSET
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
#endif

#ifdef CFG_DYM_AE_AUTOEXPRATIOMAX
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
#endif

#ifdef CFG_DYM_AE_AUTOEXPRATIOMIN
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
#endif

    return HI_SUCCESS;
}

HI_S32 SCENE_LoadDynamicFalseColor(const HI_CHAR *pszIniModule, HI_SCENE_DYNAMIC_FALSECOLOR_S *pstDynamicFalseColor)
{

#ifdef CFG_DYM_FALSECOLOR_EXPTHRESH
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
#endif

#ifdef CFG_DYM_FALSECOLOR_TOTALNUM
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
#endif

#ifdef CFG_DYM_FALSECOLOR_MANUALSTRENGTH
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
#endif

    return HI_SUCCESS;
}

HI_S32 SCENE_LoadDynamicDemosaic(const HI_CHAR *pszIniModule, HI_SCENE_DYNAMIC_DEMOSAIC_S *pstDynamicDemosaic)
{

#ifdef CFG_DYM_DEMOSAIC_TOTALNUM
    HI_S32 s32Ret = HI_SUCCESS;
    HI_CHAR *pszString = NULL;
    HI_S32 s32Value = 0;
    HI_U32 u32IdxM = 0;
    HI_CHAR aszIniNodeName[SCENE_INIPARAM_NODE_NAME_LEN] = {0,};
    snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "dynamic_demosaic:TotalNum");/*TotalNum*/
    s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
    SCENE_INIPARAM_CHECK_LOAD_RESULT(s32Ret, aszIniNodeName);
    if (HI_NULL != pszString)
    {
        free(pszString);
        pszString = HI_NULL;
        s32Ret = HI_CONFACCESS_GetInt(SCENE_INIPARAM, pszIniModule, "dynamic_demosaic:TotalNum", 0, &s32Value);
        if (HI_SUCCESS != s32Ret)
        {
            MLOGE("load dynamic_demosaic:TotalNum failed\n");
            return HI_FAILURE;
        }
        pstDynamicDemosaic->u32TotalNum = (HI_U32)s32Value;
    }
#endif

#ifdef CFG_DYM_DEMOSAIC_DEMOSAICEXPTHRESH
    snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "dynamic_demosaic:DemosaicExpThresh");/*DemosaicExpThresh*/
    s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
    SCENE_INIPARAM_CHECK_LOAD_RESULT(s32Ret, aszIniNodeName);
    if (HI_NULL != pszString)
    {
        SCENE_GetNumbersInOneLine(pszString);
        for (u32IdxM = 0; u32IdxM < pstDynamicDemosaic->u32TotalNum; u32IdxM++)
        {
            pstDynamicDemosaic->au32DemosaicExpThresh[u32IdxM] = s_as64LineNum[u32IdxM];
        }
        free(pszString);
        pszString = HI_NULL;
    }
#endif

#ifdef CFG_DYM_DEMOSAIC_MANUALANTIALIASTHR
    snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "dynamic_demosaic:ManualAntiAliasThr");/*ManualAntiAliasThr*/
    s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
    SCENE_INIPARAM_CHECK_LOAD_RESULT(s32Ret, aszIniNodeName);
    if (HI_NULL != pszString)
    {
        SCENE_GetNumbersInOneLine(pszString);
        for (u32IdxM = 0; u32IdxM < pstDynamicDemosaic->u32TotalNum; u32IdxM++)
        {
            pstDynamicDemosaic->au16ManualAntiAliasThr[u32IdxM] = s_as64LineNum[u32IdxM];
        }
        free(pszString);
        pszString = HI_NULL;
    }
#endif

    return HI_SUCCESS;
}

HI_S32 SCENE_LoadDynamicDIS(const HI_CHAR *pszIniModule, HI_SCENE_DYNAMIC_DIS_S *pstDynamicDis)
{
#ifdef CFG_DYM_DIS_TOTALNUM
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
#endif

#ifdef CFG_DYM_DIS_DISISOTHRESH
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
#endif

#ifdef CFG_DYM_DIS_MOVINGSUBJECTLEVEL
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

HI_S32 SCENE_LoadThreadDrc(const HI_CHAR *pszIniModule, HI_SCENE_THREAD_DRC_S *pstThreadDrc)
{
    HI_S32 s32Ret = HI_SUCCESS;
    HI_CHAR *pszString = NULL;
    HI_S32 s32Value = 0;
    HI_U32 u32IdxM = 0;
    HI_U32 u32IdxN = 0;
    HI_CHAR aszIniNodeName[SCENE_INIPARAM_NODE_NAME_LEN] = {0,};

#ifdef CFG_DYM_THREADDRC_RATIONCNT
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
#endif

#ifdef CFG_DYM_THREADDRC_RATIONLEVE
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
#endif

#ifdef CFG_DYM_THREADDRC_EXPCNT
    snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "thread_drc:ExpCnt");/*ExpCnt*/
    s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
    SCENE_INIPARAM_CHECK_LOAD_RESULT(s32Ret, aszIniNodeName);
    if (HI_NULL != pszString)
    {
        free(pszString);
        pszString = HI_NULL;
        s32Ret = HI_CONFACCESS_GetInt(SCENE_INIPARAM, pszIniModule, "thread_drc:ExpCnt", 0, &s32Value);
        if (HI_SUCCESS != s32Ret)
        {
            MLOGE("load thread_drc:ExpCnt failed\n");
            return HI_FAILURE;
        }
        pstThreadDrc->u32ExpCount = (HI_U32)s32Value;
    }
#endif

#ifdef CFG_DYM_THREADDRC_EXP
    snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "thread_drc:ExpThreshLtoH");/*ExpThreshLtoH*/
    s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
    SCENE_INIPARAM_CHECK_LOAD_RESULT(s32Ret, aszIniNodeName);
    if (HI_NULL != pszString)
    {
        SCENE_GetNumbersInOneLine(pszString);
        for (u32IdxM = 0; u32IdxM < pstThreadDrc->u32ExpCount; u32IdxM++)
        {
            pstThreadDrc->au64ExpThreshLtoH[u32IdxM] = s_as64LineNum[u32IdxM];
        }
        free(pszString);
        pszString = HI_NULL;
    }
#endif

#ifdef CFG_DYM_THREADDRC_ALPHA
    snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "thread_drc:Alpha");/*Alpha*/
    s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
    SCENE_INIPARAM_CHECK_LOAD_RESULT(s32Ret, aszIniNodeName);
    if (HI_NULL != pszString)
    {
        SCENE_GetNumbersInOneLine(pszString);
        for (u32IdxM = 0; u32IdxM < pstThreadDrc->u32ExpCount; u32IdxM++)
        {
            pstThreadDrc->u32Aplha[u32IdxM] = s_as64LineNum[u32IdxM];
        }
        free(pszString);
        pszString = HI_NULL;
    }
#endif

#ifdef CFG_DYM_THREADDRC_ISOCNT
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
#endif

#ifdef CFG_DYM_THREADDRC_ISOLEVEL
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
#endif

#ifdef CFG_DYM_THREADDRC_INTERVAL
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
#endif

#ifdef CFG_DYM_THREADDRC_ENABLE
    snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "thread_drc:Enable");/*Enable*/
    s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
    SCENE_INIPARAM_CHECK_LOAD_RESULT(s32Ret, aszIniNodeName);
    if (HI_NULL != pszString)
    {
        free(pszString);
        pszString = HI_NULL;
        s32Ret = HI_CONFACCESS_GetInt(SCENE_INIPARAM, pszIniModule, "thread_drc:Enable", 0, &s32Value);
        if (HI_SUCCESS != s32Ret)
        {
            MLOGE("load thread_drc:Enable failed\n");
            return HI_FAILURE;
        }
        pstThreadDrc->bEnable = (HI_BOOL)s32Value;
    }
#endif

#ifdef CFG_DYM_THREADDRC_DRCTONEMAPPINGVALUE1
    snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "thread_drc:DRCToneMappingValue1");/*DRCToneMapping_Curve1*/
    s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
    SCENE_INIPARAM_CHECK_LOAD_RESULT(s32Ret, aszIniNodeName);
    if (HI_NULL != pszString)
    {
        SCENE_GetNumbersInOneLine(pszString);
        for (u32IdxM = 0; u32IdxM < HI_ISP_DRC_TM_NODE_NUM; u32IdxM++)
        {
            pstThreadDrc->au16ToneMappingValue1[u32IdxM] = s_as64LineNum[u32IdxM];
        }
        free(pszString);
        pszString = HI_NULL;
    }
#endif

#ifdef CFG_DYM_THREADDRC_DRCTONEMAPPINGVALUE2
    snprintf(aszIniNodeName, SCENE_INIPARAM_NODE_NAME_LEN, "thread_drc:DRCToneMappingValue2");/*DRCToneMappingValue_Curve2*/
    s32Ret = HI_CONFACCESS_GetString(SCENE_INIPARAM, pszIniModule, aszIniNodeName, NULL, &pszString);
    SCENE_INIPARAM_CHECK_LOAD_RESULT(s32Ret, aszIniNodeName);
    if (HI_NULL != pszString)
    {
        SCENE_GetNumbersInOneLine(pszString);
        for (u32IdxM = 0; u32IdxM < HI_ISP_DRC_TM_NODE_NUM; u32IdxM++)
        {
            pstThreadDrc->au16ToneMappingValue2[u32IdxM] = s_as64LineNum[u32IdxM];
        }
        free(pszString);
        pszString = HI_NULL;
    }
#endif

#ifdef CFG_DYM_THREADDRC_LOCALMIXINGBRIGHTMAX
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
#endif

#ifdef CFG_DYM_THREADDRC_LOCALMIXINGBRIGHTMIN
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
#endif

#ifdef CFG_DYM_THREADDRC_LOCALMIXINGDARKMAX
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
#endif

#ifdef CFG_DYM_THREADDRC_LOCALMIXINGDARKMIN
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
#endif

#ifdef CFG_DYM_THREADDRC_DETAILADJUSTFACTOR
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
#endif

#ifdef CFG_DYM_THREADDRC_DSPATIALFLTCOEF
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
#endif

#ifdef CFG_DYM_THREADDRC_RANGEFLTCOEF
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
#endif

#ifdef CFG_DYM_THREADDRC_GRADREVMAX
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
#endif

#ifdef CFG_DYM_THREADDRC_GRADREVTHR
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
#endif

#ifdef CFG_DYM_THREADDRC_COMPRESS
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
#endif

#ifdef CFG_DYM_THREADDRC_STRETCH
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
#endif

#ifdef CFG_DYM_THREADDRC_AUTOSTRENGTH
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
#endif
    return HI_SUCCESS;
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

    s32Ret = SCENE_LoadStaticThreeDNR(pszIniModule, &pstSceneParam->stStaticThreeDNR);
    if (HI_SUCCESS != s32Ret)
    {
        MLOGE("SCENE_LoadStaticThreeDNR failed\n");
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
    s32Ret = SCENE_LoadDynamicHdrOETF(pszIniModule, &pstSceneParam->stDynamicHDROETF);
    if (HI_SUCCESS != s32Ret)
    {
        MLOGE("SCENE_LoadDynamicHdrOETF failed\n");
        return HI_FAILURE;
    }
    s32Ret = SCENE_LoadDynamicHdrTm(pszIniModule, &pstSceneParam->stDynamicHDRTm);
    if (HI_SUCCESS != s32Ret)
    {
        MLOGE("SCENE_LoadDynamicHdrTm failed\n");
        return HI_FAILURE;
    }
    s32Ret = SCENE_LoadDynamicHdrDrc(pszIniModule, &pstSceneParam->stDynamicHDRDRC);
    if (HI_SUCCESS != s32Ret)
    {
        MLOGE("SCENE_LoadDynamicHdrDrc failed\n");
        return HI_FAILURE;
    }

    s32Ret = SCENE_LoadDynamicAE(pszIniModule, &pstSceneParam->stDynamicAe);
    if (HI_SUCCESS != s32Ret)
    {
        MLOGE("SCENE_LoadDynamicAE failed\n");
        return HI_FAILURE;
    }


    s32Ret = SCENE_LoadDynamicFalseColor(pszIniModule, &pstSceneParam->stDynamicFalsecolor);
    if (HI_SUCCESS != s32Ret)
    {
        MLOGE("SCENE_LoadDynamicFalseColor failed\n");
        return HI_FAILURE;
    }

    s32Ret = SCENE_LoadDynamicDemosaic(pszIniModule, &pstSceneParam->stDynamicDemosaic);
    if (HI_SUCCESS != s32Ret)
    {
        MLOGE("SCENE_LoadDynamicDemosaic failed\n");
        return HI_FAILURE;
    }

    s32Ret = SCENE_LoadDynamicDIS(pszIniModule, &pstSceneParam->stDynamicDis);
    if (HI_SUCCESS != s32Ret)
    {
        MLOGE("SCENE_LoadDynamicDIS failed\n");
        return HI_FAILURE;
    }

    s32Ret = SCENE_LoadThreadDrc(pszIniModule, &pstSceneParam->stThreadDrc);
    if (HI_SUCCESS != s32Ret)
    {
        MLOGE("SCENE_LoadThreadDrc failed\n");
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

