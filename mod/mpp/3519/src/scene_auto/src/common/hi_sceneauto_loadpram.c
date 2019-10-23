#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <sys/prctl.h>

#include "iniparser.h"
#include "hi_sceneauto_comm.h"


#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

#ifndef SSCANF_RETURN
#define SSCANF_RETURN(str, format...)\
    do{\
        if (HI_FAILURE == sscanf(str, ##format))\
        {\
            printf("func:%s,line:%d, sscanf failed!\n",__FUNCTION__,__LINE__);\
        }\
    }while(0);
#endif

extern HI_SCENEAUTO_PARAM_S g_stSceneautoParam;
static dictionary* g_Sceneautodictionary = NULL;

static int MAEWeight[256];
static int  Weight(const char* b)
{
    const  char*    pszVRBegin     = b;
    const char*    pszVREnd       = pszVRBegin;
    int      u32Count = 0;
    char     temp[20];
    int      mycount = 0;
    int      length = strlen(b);
    unsigned int re;
    memset(temp, 0, 20);
    int i = 0;
    HI_BOOL bx = HI_FALSE;

    while ((pszVREnd != NULL))
    {
        if ((mycount > length) || (mycount == length))
        {
            break;
        }
        while ((*pszVREnd != '|') && (*pszVREnd != '\0') && (*pszVREnd != ','))
        {
            if (*pszVREnd == 'x')
            {
                bx = HI_TRUE;
            }
            pszVREnd++;
            u32Count++;
            mycount++;
        }
        memcpy(temp, pszVRBegin, u32Count);

        if (bx == HI_TRUE)
        {
            char* str;
            re = (int)strtol(temp + 2, &str, 16);
            MAEWeight[i] = re;

        }
        else
        {
            MAEWeight[i] = atoi(temp);
        }
        memset(temp, 0, 20);
        u32Count = 0;
        pszVREnd++;
        pszVRBegin = pszVREnd;
        mycount++;
        i++;
    }
    return i;
}

HI_S32 Scanf3dnrParam(HI_CHAR* pszTempStr, HI_SCENEAUTO_3DNR_S *pst3dnrParam)
{
	if ((NULL == pszTempStr)||(NULL == pst3dnrParam))
	{
		return HI_FAILURE;
	}

	SSCANF_RETURN(pszTempStr, 	"   sf0_bright  ( %3d )      sfx_bright    ( %3d,%3d )      sfk_bright ( %3d )"
					    "   sf0_dark    ( %3d )      sfx_dark      ( %3d,%3d )      sfk_dark   ( %3d )"
					    "  ----------------------+                                  sfk_pro    ( %3d )"
					    "                        |   tfx_moving    ( %3d,%3d )      sfk_ratio  ( %3d )"
					    "   csf_strength( %3d )  |   tfx_md_thresh ( %3d,%3d )                        "
					    "   ctf_strength( %3d )  |   tfx_md_profile( %3d,%3d )    sf_ed_thresh ( %3d )"
					    "   ctf_range   ( %3d )  |   tfx_md_type   ( %3d,%3d )    sf_bsc_freq  ( %3d )"
					    "                        |   tfx_still     ( %3d,%3d )    tf_profile   ( %3d )",
					    &pst3dnrParam->sf0_bright, &pst3dnrParam->sf1_bright, &pst3dnrParam->sf2_bright, &pst3dnrParam->sfk_bright,
					    &pst3dnrParam->sf0_dark, &pst3dnrParam->sf1_dark, &pst3dnrParam->sf2_dark, &pst3dnrParam->sfk_dark,
					    &pst3dnrParam->sfk_profile,
						&pst3dnrParam->tf1_moving, &pst3dnrParam->tf2_moving, &pst3dnrParam->sfk_ratio,
						&pst3dnrParam->csf_strength, &pst3dnrParam->tf1_md_thresh, &pst3dnrParam->tf2_md_thresh,
						&pst3dnrParam->ctf_strength, &pst3dnrParam->tf1_md_profile, &pst3dnrParam->tf2_md_profile, &pst3dnrParam->sf_ed_thresh,
						&pst3dnrParam->ctf_range, &pst3dnrParam->tf1_md_type, &pst3dnrParam->tf2_md_type, &pst3dnrParam->sf_bsc_freq,
						&pst3dnrParam->tf1_still, &pst3dnrParam->tf2_still, &pst3dnrParam->tf_profile);

	return HI_SUCCESS;
}

HI_S32 Scanf3dnrxParam(HI_CHAR* pszTempStr, HI_SCENEAUTO_3DNR_X_S *pst3dnrxParam)
{
	if ((NULL == pszTempStr)||(NULL == pst3dnrxParam))
	{
		return HI_FAILURE;
	}

    HI_SCENEAUTO_3DNR_X_S *pX = pst3dnrxParam;
    tSceAutoV19yIEy  *pI = &( pX->IEy );
    tSceAutoV19ySFy  *pS =    pX->SFy;
    tSceAutoV19yMDy  *pM =    pX->MDy;
    tSceAutoV19yTFy  *pT =    pX->TFy;

	SSCANF_RETURN(pszTempStr, 	"                      |                 |                 |    -ktype   %3d )"
					    "  IES  %4d           |                 |                 |    -krfw   %4d )"
					    "  IET  %4d           |                 |                 |    -kbig   %4d )"
					    "  IEB  %4d           |                 |                 |    -kpro   %4d )"
					    "                      |                 |                 |                 )"
					    "  SBS  %4d:%4d:%4d |  %4d:%4d:%4d |  %4d:%4d:%4d |  %4d:%4d:%4d )"
					    "  SDS  %4d:%4d:%4d |  %4d:%4d:%4d |  %4d:%4d:%4d |  %4d:%4d:%4d )"
					    "                      |                 |                 |                 )"
					    "  STH  %4d:%4d:%4d |  %4d:%4d:%4d |  %4d:%4d:%4d |  %4d:%4d:%4d )"
					    "  SBF  %4d:%4d:%4d |  %4d:%4d:%4d |  %4d:%4d:%4d |  %4d:%4d:%4d )"
					    "  PRO  %4d:%4d:%4d |  %4d:%4d:%4d |  %4d:%4d:%4d |  %4d:%4d:%4d )"
					    "                      |                 |                 |                 )"
					    "  SSLP           %4d |            %4d |            %4d |            %4d )"
					    "  VRTO           %4d |            %4d |            %4d |            %4d )"
					    "                      |                 |                 |                 )"
					    "  En #             on |              on |              on |              on )"
					    "                      |                 |                 |                 )"
					    "  -ttype         %4d |            %4d |            %4d |            %4d )"
					    "                      |                 |                 |                 )"
					    "  SFR  %2d:%3d:%3d:%3d |  %2d:%3d:%3d:%3d |  %2d:%3d:%3d:%3d |  %2d:%3d:%3d:%3d )"
					    "                      |                 |                 |                 )"
					    "  STR            %4d |            %4d |            %4d |            %4d )"
					    "                      |                 |                 |                 )"
					    "  TFS            %4d |            %4d |            %4d |                 )"
					    "  TFR  %2d:%3d:%3d:%3d |  %2d:%3d:%3d:%3d |  %2d:%3d:%3d:%3d |                 )"
					    "  TSS            %4d |            %4d |            %4d |                 )"
					    "  SDZ            %4d |            %4d |            %4d |                 )"
					    "                      |                 |                 |                 )"
					    "  MTFR %2d:%3d:%3d:%3d |  %2d:%3d:%3d:%3d |  %2d:%3d:%3d:%3d |                 )"
					    "  MDDZ           %4d |            %4d |            %4d |                 )"
					    "                      |                 |                 |                 )"
					    "  MATH           %4d |            %4d |            %4d |                 )"
					    "  MATE           %4d |            %4d |            %4d +-----------------)"
					    "  MABW           %4d |            %4d |            %4d |       -sfc  %3d )"
					    "  MATW           %4d |            %4d |            %4d |       -trc  %3d )"
					    "  MASW           %4d |            %4d |            %4d |       -tfc  %3d )"
					    "  MAXN           %4d |            %4d |            %4d |       -tpc  %3d )"
                        ,
                        &pS[3].SFkType,
                        &pI->IES, &pS[3].SFkRfw,
                        &pI->IET, &pS[3].SFkBig,
                        &pI->IEB, &pS[3].kProDD,
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
                        &pS[0].SBF0,   &pS[0].SBF1,   &pS[0].SBF2,
                        &pS[1].SBF0,   &pS[1].SBF1,   &pS[1].SBF2,
                        &pS[2].SBF0,   &pS[2].SBF1,   &pS[2].SBF2,
                        &pS[3].SBF0,   &pS[3].SBF1,   &pS[3].SBF2,
                        &pS[0].horPro, &pS[0].verPro, &pS[0].kProDD,
                        &pS[1].horPro, &pS[1].verPro, &pS[1].kProDD,
                        &pS[2].horPro, &pS[2].verPro, &pS[2].kProDD,
                        &pS[3].horPro, &pS[3].verPro, &pS[3].kProDD,
                        &pS[0].SSLP,&pS[1].SSLP,&pS[2].SSLP,&pS[3].SSLP,
                        &pS[0].VRTO,&pS[1].VRTO,&pS[2].VRTO,&pS[3].VRTO,
                        &pT[0].TFT, &pT[1].TFT, &pT[2].TFT, &pT[3].TFT,
                        &pT[0].SFR[0], &pT[0].SFR[1], &pT[0].SFR[2], &pT[0].SFR[3],
                        &pT[1].SFR[0], &pT[1].SFR[1], &pT[1].SFR[2], &pT[1].SFR[3],
                        &pT[2].SFR[0], &pT[2].SFR[1], &pT[2].SFR[2], &pT[2].SFR[3],
                        &pT[3].SFR[0], &pT[3].SFR[1], &pT[3].SFR[2], &pT[3].SFR[3],
                        &pT[0].STR, &pT[1].STR, &pT[2].STR, &pT[3].STR,
                        &pT[0].TFS, &pT[1].TFS, &pT[2].TFS,
                        &pT[0].TFR[0], &pT[0].TFR[1], &pT[0].TFR[2], &pT[0].TFR[3],
                        &pT[1].TFR[0], &pT[1].TFR[1], &pT[1].TFR[2], &pT[1].TFR[3],
                        &pT[2].TFR[0], &pT[2].TFR[1], &pT[2].TFR[2], &pT[2].TFR[3],
                        &pT[0].TSS, &pT[1].TSS, &pT[2].TSS,
                        &pT[0].SDZ, &pT[1].SDZ, &pT[2].SDZ,
                        &pT[0].MTFR[0], &pT[0].MTFR[1], &pT[0].MTFR[2], &pT[0].MTFR[3],
                        &pT[1].MTFR[0], &pT[1].MTFR[1], &pT[1].MTFR[2], &pT[1].MTFR[3],
                        &pT[2].MTFR[0], &pT[2].MTFR[1], &pT[2].MTFR[2], &pT[2].MTFR[3],
                        &pT[0].MDDZ, &pT[1].MDDZ, &pT[2].MDDZ,
                        &pM[0].MATH,&pM[1].MATH,&pM[2].MATH,
                        &pM[0].MATE,&pM[1].MATE,&pM[2].MATE,
                        &pM[0].MABW,&pM[1].MABW,&pM[2].MABW, &pX->NRc.SFC,
                        &pM[0].MATW,&pM[1].MATW,&pM[2].MATW, &pX->NRc.TRC,
                        &pM[0].MASW,&pM[1].MASW,&pM[2].MASW, &pX->NRc.TFC,
                        &pM[0].MAXN,&pM[1].MAXN,&pM[2].MAXN, &pX->NRc.TPC);


	return HI_SUCCESS;
}

HI_S32 Sceneauto_LoadFswdrParam()
{
	HI_S32 s32Temp;
    HI_S32 i;
    HI_CHAR szTempStr[64] = {0};
    HI_CHAR* pszTempStr = HI_NULL;

    /**************FSWDR:DRCStrength**************/
    s32Temp = 0;
    s32Temp = iniparser_getint(g_Sceneautodictionary, "FSWDR:DRCStrength", HI_FAILURE);
    if (HI_FAILURE == s32Temp)
    {
        printf("FSWDR:DRCStrength failed\n");
        return HI_FAILURE;
    }
    g_stSceneautoParam.stFswdrParam.u32DRCStrength = s32Temp;

	/**************FSWDR:ISOUpTh**************/
    s32Temp = 0;
    s32Temp = iniparser_getint(g_Sceneautodictionary, "FSWDR:ISOUpTh", HI_FAILURE);
    if (HI_FAILURE == s32Temp)
    {
        printf("FSWDR:ISOUpTh failed\n");
        return HI_FAILURE;
    }
    g_stSceneautoParam.stFswdrParam.u32ISOUpTh = s32Temp;

    /**************FSWDR:ISODnTh**************/
    s32Temp = 0;
    s32Temp = iniparser_getint(g_Sceneautodictionary, "FSWDR:ISODnTh", HI_FAILURE);
    if (HI_FAILURE == s32Temp)
    {
        printf("FSWDR:ISODnTh failed\n");
        return HI_FAILURE;
    }
    g_stSceneautoParam.stFswdrParam.u32ISODnTh = s32Temp;

    /**************FSWDR:RefExpRatioTh**************/
    s32Temp = 0;
    s32Temp = iniparser_getint(g_Sceneautodictionary, "FSWDR:RefExpRatioTh", HI_FAILURE);
    if (HI_FAILURE == s32Temp)
    {
        printf("FSWDR:RefExpRatioTh failed\n");
        return HI_FAILURE;
    }
    g_stSceneautoParam.stFswdrParam.u32RefExpRatioTh = s32Temp;

	/**************FSWDR:ExpCount**************/
    s32Temp = 0;
    s32Temp = iniparser_getint(g_Sceneautodictionary, "FSWDR:ExpCount", HI_FAILURE);
    if (HI_FAILURE == s32Temp)
    {
        printf("FSWDR:ExpCount failed\n");
        return HI_FAILURE;
    }
    g_stSceneautoParam.stFswdrParam.u32ExpCount = s32Temp;

    CHECK_MALLOC_SIZE(s32Temp);
    g_stSceneautoParam.stFswdrParam.pu32ExpThreshLtoH = (HI_U32*)malloc(s32Temp * sizeof(HI_U32));
    CHECK_NULL_PTR(g_stSceneautoParam.stFswdrParam.pu32ExpThreshLtoH);
    g_stSceneautoParam.stFswdrParam.pu32ExpThreshHtoL = (HI_U32*)malloc(s32Temp * sizeof(HI_U32));
    CHECK_NULL_PTR(g_stSceneautoParam.stFswdrParam.pu32ExpThreshHtoL);
    g_stSceneautoParam.stFswdrParam.pu8ExpCompensation = (HI_U8*)malloc(s32Temp * sizeof(HI_U8));
    CHECK_NULL_PTR(g_stSceneautoParam.stFswdrParam.pu8ExpCompensation);
    g_stSceneautoParam.stFswdrParam.pu8MaxHistOffset = (HI_U8*)malloc(s32Temp * sizeof(HI_U8));
	CHECK_NULL_PTR(g_stSceneautoParam.stFswdrParam.pu8MaxHistOffset);

	/**************FSWDR:ExpThreshLtoH**************/
    pszTempStr = iniparser_getstr(g_Sceneautodictionary, "FSWDR:ExpThreshLtoH");
    if (NULL == pszTempStr)
    {
        printf("FSWDR:ExpThreshLtoH error\n");
        return HI_FAILURE;
    }
    Weight(pszTempStr);
    for (i = 0; i < g_stSceneautoParam.stFswdrParam.u32ExpCount; i++)
    {
        g_stSceneautoParam.stFswdrParam.pu32ExpThreshLtoH[i] = MAEWeight[i];
    }

    /**************FSWDR:ExpThreshHtoL**************/
    pszTempStr = iniparser_getstr(g_Sceneautodictionary, "FSWDR:ExpThreshHtoL");
    if (NULL == pszTempStr)
    {
        printf("FSWDR:ExpThreshHtoL error\n");
        return HI_FAILURE;
    }
    Weight(pszTempStr);
    for (i = 0; i < g_stSceneautoParam.stFswdrParam.u32ExpCount; i++)
    {
        g_stSceneautoParam.stFswdrParam.pu32ExpThreshHtoL[i] = MAEWeight[i];
    }

    /**************FSWDR:ExpCompensation**************/
    pszTempStr = iniparser_getstr(g_Sceneautodictionary, "FSWDR:ExpCompensation");
    if (NULL == pszTempStr)
    {
        printf("FSWDR:ExpCompensation error\n");
        return HI_FAILURE;
    }
    Weight(pszTempStr);
    for (i = 0; i < g_stSceneautoParam.stFswdrParam.u32ExpCount; i++)
    {
        g_stSceneautoParam.stFswdrParam.pu8ExpCompensation[i] = MAEWeight[i];
    }

    /**************FSWDR:MaxHistOffset**************/
    pszTempStr = iniparser_getstr(g_Sceneautodictionary, "FSWDR:MaxHistOffset");
    if (NULL == pszTempStr)
    {
        printf("FSWDR:MaxHistOffset error\n");
        return HI_FAILURE;
    }
    Weight(pszTempStr);
    for (i = 0; i < g_stSceneautoParam.stFswdrParam.u32ExpCount; i++)
    {
        g_stSceneautoParam.stFswdrParam.pu8MaxHistOffset[i] = MAEWeight[i];
    }

	/**************FSWDR:3DnrIsoCount**************/
    s32Temp = 0;
	s32Temp = iniparser_getint(g_Sceneautodictionary, "FSWDR:3DnrIsoCount", HI_FAILURE);
    if (HI_FAILURE == s32Temp)
    {
    	printf("FSWDR:3DnrIsoCount failed\n");
        return HI_FAILURE;
    }
    g_stSceneautoParam.stFswdrParam.stFSWDR3dnr.u323DnrIsoCount = s32Temp;
    CHECK_MALLOC_SIZE(s32Temp);
    g_stSceneautoParam.stFswdrParam.stFSWDR3dnr.pu323DnrIsoThresh = (HI_U32*)malloc(s32Temp * sizeof(HI_U32));
    CHECK_NULL_PTR(g_stSceneautoParam.stFswdrParam.stFSWDR3dnr.pu323DnrIsoThresh);
    g_stSceneautoParam.stFswdrParam.stFSWDR3dnr.pst3dnrParam = (HI_SCENEAUTO_3DNR_S*)malloc(s32Temp * sizeof(HI_SCENEAUTO_3DNR_S));
    CHECK_NULL_PTR(g_stSceneautoParam.stFswdrParam.stFSWDR3dnr.pst3dnrParam);

    /**************FSWDR:3DnrIsoThresh**************/
    snprintf(szTempStr, sizeof(szTempStr), "FSWDR:3DnrIsoThresh");
	pszTempStr = iniparser_getstr(g_Sceneautodictionary, szTempStr);
    if (NULL == pszTempStr)
    {
        printf("FSWDR:3DnrIsoThresh failed\n");
        return HI_FAILURE;
    }
    Weight(pszTempStr);
    for (i = 0; i < g_stSceneautoParam.stFswdrParam.stFSWDR3dnr.u323DnrIsoCount; i++)
    {
        g_stSceneautoParam.stFswdrParam.stFSWDR3dnr.pu323DnrIsoThresh[i] = MAEWeight[i];
    }

    for (i = 0; i < g_stSceneautoParam.stFswdrParam.stFSWDR3dnr.u323DnrIsoCount; i++)
	{
		/**************FSWDR:3DnrParam**************/
		snprintf(szTempStr, sizeof(szTempStr), "FSWDR:3DnrParam_%d", i);
		pszTempStr = iniparser_getstr(g_Sceneautodictionary, szTempStr);
		if (NULL == pszTempStr)
		{
		    printf("FSWDR::3DnrParam_%d failed\n", i);
		    return HI_FAILURE;
		}

		s32Temp = Scanf3dnrParam(pszTempStr, (g_stSceneautoParam.stFswdrParam.stFSWDR3dnr.pst3dnrParam + i));
		if (HI_FAILURE == s32Temp)
	    {
	        printf("Scanf3dnrParam failed\n");
	        return HI_FAILURE;
	    }
	}

	return HI_SUCCESS;
}

HI_S32 Sceneauto_LoadTrafficParam()
{
	HI_S32 s32Temp;
    HI_S32 i;
    HI_S32 s32Offset;
    HI_CHAR szTempStr[64] = {0};
    HI_CHAR* pszTempStr = HI_NULL;

    /**************TRAFFIC:DRCEnable**************/
    s32Temp = 0;
    s32Temp = iniparser_getint(g_Sceneautodictionary, "TRAFFIC:DRCEnable", HI_FAILURE);
    if (HI_FAILURE == s32Temp)
    {
        printf("TRAFFIC:DRCEnable failed\n");
        return HI_FAILURE;
    }
    g_stSceneautoParam.stTrafficParam.bDRCEnable = (HI_BOOL)s32Temp;

	/**************TRAFFIC:DRCManulEnable**************/
    s32Temp = 0;
    s32Temp = iniparser_getint(g_Sceneautodictionary, "TRAFFIC:DRCManulEnable", HI_FAILURE);
    if (HI_FAILURE == s32Temp)
    {
        printf("TRAFFIC:DRCManulEnable failed\n");
        return HI_FAILURE;
    }
    g_stSceneautoParam.stTrafficParam.bDRCManulEnable = (HI_BOOL)s32Temp;

    /**************TRAFFIC:DRCStrengthTarget**************/
    s32Temp = 0;
    s32Temp = iniparser_getint(g_Sceneautodictionary, "TRAFFIC:DRCStrengthTarget", HI_FAILURE);
    if (HI_FAILURE == s32Temp)
    {
        printf("TRAFFIC:DRCStrengthTarget failed\n");
        return HI_FAILURE;
    }
    g_stSceneautoParam.stTrafficParam.u32DRCStrengthTarget = s32Temp;

    /**************TRAFFIC:DRCAutoStrength**************/
    s32Temp = 0;
    s32Temp = iniparser_getint(g_Sceneautodictionary, "TRAFFIC:DRCAutoStrength", HI_FAILURE);
    if (HI_FAILURE == s32Temp)
    {
        printf("TRAFFIC:DRCAutoStrength failed\n");
        return HI_FAILURE;
    }
    g_stSceneautoParam.stTrafficParam.u32DRCAutoStrength = s32Temp;

    /**************TRAFFIC:u32WhiteLevel**************/
    s32Temp = 0;
    s32Temp = iniparser_getint(g_Sceneautodictionary, "TRAFFIC:u32WhiteLevel", HI_FAILURE);
    if (HI_FAILURE == s32Temp)
    {
        printf("TRAFFIC:u32WhiteLevel failed\n");
        return HI_FAILURE;
    }
    g_stSceneautoParam.stTrafficParam.u32WhiteLevel = s32Temp;

    /**************TRAFFIC:u32BlackLevel**************/
    s32Temp = 0;
    s32Temp = iniparser_getint(g_Sceneautodictionary, "TRAFFIC:u32BlackLevel", HI_FAILURE);
    if (HI_FAILURE == s32Temp)
    {
        printf("TRAFFIC:u32BlackLevel failed\n");
        return HI_FAILURE;
    }
    g_stSceneautoParam.stTrafficParam.u32BlackLevel = s32Temp;

    /**************TRAFFIC:u32Asymmetry**************/
    s32Temp = 0;
    s32Temp = iniparser_getint(g_Sceneautodictionary, "TRAFFIC:u32Asymmetry", HI_FAILURE);
    if (HI_FAILURE == s32Temp)
    {
        printf("TRAFFIC:u32Asymmetry failed\n");
        return HI_FAILURE;
    }
    g_stSceneautoParam.stTrafficParam.u32Asymmetry = s32Temp;

    /**************TRAFFIC:u32BrightEnhance**************/
    s32Temp = 0;
    s32Temp = iniparser_getint(g_Sceneautodictionary, "TRAFFIC:u32BrightEnhance", HI_FAILURE);
    if (HI_FAILURE == s32Temp)
    {
        printf("TRAFFIC:u32BrightEnhance failed\n");
        return HI_FAILURE;
    }
    g_stSceneautoParam.stTrafficParam.u32BrightEnhance = s32Temp;

    /**************TRAFFIC:u8FilterMux**************/
    s32Temp = 0;
    s32Temp = iniparser_getint(g_Sceneautodictionary, "TRAFFIC:u8FilterMux", HI_FAILURE);
    if (HI_FAILURE == s32Temp)
    {
        printf("TRAFFIC:u8FilterMux failed\n");
        return HI_FAILURE;
    }
    g_stSceneautoParam.stTrafficParam.u8FilterMux = s32Temp;

    /**************TRAFFIC:u32SlopeMax**************/
    s32Temp = 0;
    s32Temp = iniparser_getint(g_Sceneautodictionary, "TRAFFIC:u32SlopeMax", HI_FAILURE);
    if (HI_FAILURE == s32Temp)
    {
        printf("TRAFFIC:u32SlopeMax failed\n");
        return HI_FAILURE;
    }
    g_stSceneautoParam.stTrafficParam.u32SlopeMax = s32Temp;

    /**************TRAFFIC:u32SlopeMin**************/
    s32Temp = 0;
    s32Temp = iniparser_getint(g_Sceneautodictionary, "TRAFFIC:u32SlopeMin", HI_FAILURE);
    if (HI_FAILURE == s32Temp)
    {
        printf("TRAFFIC:u32SlopeMin failed\n");
        return HI_FAILURE;
    }
    g_stSceneautoParam.stTrafficParam.u32SlopeMin = s32Temp;

    /**************TRAFFIC:u32VarianceIntensity**************/
    s32Temp = 0;
    s32Temp = iniparser_getint(g_Sceneautodictionary, "TRAFFIC:u32VarianceIntensity", HI_FAILURE);
    if (HI_FAILURE == s32Temp)
    {
        printf("TRAFFIC:u32VarianceIntensity failed\n");
        return HI_FAILURE;
    }
    g_stSceneautoParam.stTrafficParam.u32VarianceIntensity = s32Temp;

    /**************TRAFFIC:u32VarianceSpace**************/
    s32Temp = 0;
    s32Temp = iniparser_getint(g_Sceneautodictionary, "TRAFFIC:u32VarianceSpace", HI_FAILURE);
    if (HI_FAILURE == s32Temp)
    {
        printf("TRAFFIC:u32VarianceSpace failed\n");
        return HI_FAILURE;
    }
    g_stSceneautoParam.stTrafficParam.u32VarianceSpace = s32Temp;

    /**************TRAFFIC:u32BrightPr**************/
    s32Temp = 0;
    s32Temp = iniparser_getint(g_Sceneautodictionary, "TRAFFIC:u32BrightPr", HI_FAILURE);
    if (HI_FAILURE == s32Temp)
    {
        printf("TRAFFIC:u32BrightPr failed\n");
        return HI_FAILURE;
    }
    g_stSceneautoParam.stTrafficParam.u32BrightPr = s32Temp;

    /**************TRAFFIC:u32Contrast**************/
    s32Temp = 0;
    s32Temp = iniparser_getint(g_Sceneautodictionary, "TRAFFIC:u32Contrast", HI_FAILURE);
    if (HI_FAILURE == s32Temp)
    {
        printf("TRAFFIC:u32Contrast failed\n");
        return HI_FAILURE;
    }
    g_stSceneautoParam.stTrafficParam.u32Contrast = s32Temp;

    /**************TRAFFIC:u32DarkEnhance**************/
    s32Temp = 0;
    s32Temp = iniparser_getint(g_Sceneautodictionary, "TRAFFIC:u32DarkEnhance", HI_FAILURE);
    if (HI_FAILURE == s32Temp)
    {
        printf("TRAFFIC:u32DarkEnhance failed\n");
        return HI_FAILURE;
    }
    g_stSceneautoParam.stTrafficParam.u32DarkEnhance = s32Temp;

    /**************TRAFFIC:u32Svariance**************/
    s32Temp = 0;
    s32Temp = iniparser_getint(g_Sceneautodictionary, "TRAFFIC:u32Svariance", HI_FAILURE);
    if (HI_FAILURE == s32Temp)
    {
        printf("TRAFFIC:u32Svariance failed\n");
        return HI_FAILURE;
    }
    g_stSceneautoParam.stTrafficParam.u32Svariance = s32Temp;

    /**************TRAFFIC:ExpCompensation**************/
    s32Temp = 0;
    s32Temp = iniparser_getint(g_Sceneautodictionary, "TRAFFIC:u8ExpCompensation", HI_FAILURE);
    if (HI_FAILURE == s32Temp)
    {
    	printf("TRAFFIC:ExpCompensation failed\n");
    }
    g_stSceneautoParam.stTrafficParam.u8ExpCompensation = s32Temp;

    /**************TRAFFIC:MaxHistoffset **************/
    s32Temp = 0;
    s32Temp = iniparser_getint(g_Sceneautodictionary, "TRAFFIC:u8MaxHistoffset", HI_FAILURE);
    if (HI_FAILURE == s32Temp)
    {
    	printf("TRAFFIC:MaxHistoffset failed\n");
    }
    g_stSceneautoParam.stTrafficParam.u8MaxHistoffset = s32Temp;

	/**************TRAFFIC:u8ExpRatioType**************/
    s32Temp = 0;
    s32Temp = iniparser_getint(g_Sceneautodictionary, "TRAFFIC:u8ExpRatioType", HI_FAILURE);
    if (HI_FAILURE == s32Temp)
    {
        printf("TRAFFIC:u8ExpRatioType failed\n");
        return HI_FAILURE;
    }
    g_stSceneautoParam.stTrafficParam.u8ExpRatioType = s32Temp;

    /**************TRAFFIC:au32ExpRatio**************/
    pszTempStr = iniparser_getstr(g_Sceneautodictionary, "TRAFFIC:au32ExpRatio");
    if (NULL == pszTempStr)
    {
        printf("TRAFFIC:au32ExpRatio error\n");
        return HI_FAILURE;
    }
    Weight(pszTempStr);
    for (i = 0; i < 3; i++)
    {
        g_stSceneautoParam.stTrafficParam.au32ExpRatio[i] = MAEWeight[i];
    }

    /**************TRAFFIC:u32ExpRatioMax**************/
    s32Temp = 0;
    s32Temp = iniparser_getint(g_Sceneautodictionary, "TRAFFIC:u32ExpRatioMax", HI_FAILURE);
    if (HI_FAILURE == s32Temp)
    {
        printf("TRAFFIC:u32ExpRatioMax failed\n");
        return HI_FAILURE;
    }
    g_stSceneautoParam.stTrafficParam.u32ExpRatioMax = s32Temp;

    /**************TRAFFIC:u32ExpRatioMin**************/
    s32Temp = 0;
    s32Temp = iniparser_getint(g_Sceneautodictionary, "TRAFFIC:u32ExpRatioMin", HI_FAILURE);
    if (HI_FAILURE == s32Temp)
    {
        printf("TRAFFIC:u32ExpRatioMin failed\n");
        return HI_FAILURE;
    }
    g_stSceneautoParam.stTrafficParam.u32ExpRatioMin = s32Temp;

    /**************TRAFFIC:au8TextureSt**************/
    pszTempStr = iniparser_getstr(g_Sceneautodictionary, "TRAFFIC:au8TextureSt");
    if (NULL == pszTempStr)
    {
        printf("TRAFFIC:au8TextureSt error\n");
        return HI_FAILURE;
    }
    Weight(pszTempStr);
    for (i = 0; i < EXPOSURE_LEVEL; i++)
    {
        g_stSceneautoParam.stTrafficParam.au8TextureSt[i] = MAEWeight[i];
    }

    /**************TRAFFIC:au8EdgeSt**************/
    pszTempStr = iniparser_getstr(g_Sceneautodictionary, "TRAFFIC:au8EdgeSt");
    if (NULL == pszTempStr)
    {
        printf("TRAFFIC:au8EdgeSt error\n");
        return HI_FAILURE;
    }
    Weight(pszTempStr);
    for (i = 0; i < EXPOSURE_LEVEL; i++)
    {
        g_stSceneautoParam.stTrafficParam.au8EdgeSt[i] = MAEWeight[i];
    }

    /**************TRAFFIC:au8OverShoot**************/
    pszTempStr = iniparser_getstr(g_Sceneautodictionary, "TRAFFIC:au8OverShoot");
    if (NULL == pszTempStr)
    {
        printf("TRAFFIC:au8OverShoot error\n");
        return HI_FAILURE;
    }
    Weight(pszTempStr);
    for (i = 0; i < EXPOSURE_LEVEL; i++)
    {
        g_stSceneautoParam.stTrafficParam.au8OverShoot[i] = MAEWeight[i];
    }

    /**************TRAFFIC:au8UnderShoot**************/
    pszTempStr = iniparser_getstr(g_Sceneautodictionary, "TRAFFIC:au8UnderShoot");
    if (NULL == pszTempStr)
    {
        printf("TRAFFIC:au8UnderShoot error\n");
        return HI_FAILURE;
    }
    Weight(pszTempStr);
    for (i = 0; i < EXPOSURE_LEVEL; i++)
    {
        g_stSceneautoParam.stTrafficParam.au8UnderShoot[i] = MAEWeight[i];
    }

    /**************TRAFFIC:au8TextureThd**************/
    pszTempStr = iniparser_getstr(g_Sceneautodictionary, "TRAFFIC:au8TextureThd");
    if (NULL == pszTempStr)
    {
        printf("TRAFFIC:au8TextureThd error\n");
        return HI_FAILURE;
    }
    Weight(pszTempStr);
    for (i = 0; i < EXPOSURE_LEVEL; i++)
    {
        g_stSceneautoParam.stTrafficParam.au8TextureThd[i] = MAEWeight[i];
    }

    /**************TRAFFIC:au8EdgeThd**************/
    pszTempStr = iniparser_getstr(g_Sceneautodictionary, "TRAFFIC:au8EdgeThd");
    if (NULL == pszTempStr)
    {
        printf("TRAFFIC:au8EdgeThd error\n");
        return HI_FAILURE;
    }
    Weight(pszTempStr);
    for (i = 0; i < EXPOSURE_LEVEL; i++)
    {
        g_stSceneautoParam.stTrafficParam.au8EdgeThd[i] = MAEWeight[i];
    }

    /**************TRAFFIC:au8DetailCtrl**************/
    pszTempStr = iniparser_getstr(g_Sceneautodictionary, "TRAFFIC:au8DetailCtrl");
    if (NULL == pszTempStr)
    {
        printf("TRAFFIC:au8DetailCtrl error\n");
        return HI_FAILURE;
    }
    Weight(pszTempStr);
    for (i = 0; i < EXPOSURE_LEVEL; i++)
    {
        g_stSceneautoParam.stTrafficParam.au8DetailCtrl[i] = MAEWeight[i];
    }

    /**************TRAFFIC:gamma**************/
    s32Offset = 0;
    snprintf(szTempStr, sizeof(szTempStr), "TRAFFIC:gamma_0");
    pszTempStr = iniparser_getstr(g_Sceneautodictionary, szTempStr);
    if (NULL == pszTempStr)
    {
        printf("TRAFFIC:gamma_0 error\n");
        return HI_FAILURE;
    }
    s32Temp = Weight(pszTempStr);
    for (i = 0; i < s32Temp; i++)
    {
        g_stSceneautoParam.stTrafficParam.u16GammaTable[i] = MAEWeight[i];
    }
    s32Offset += s32Temp;

    snprintf(szTempStr, sizeof(szTempStr), "TRAFFIC:gamma_1");
    pszTempStr = iniparser_getstr(g_Sceneautodictionary, szTempStr);
    if (NULL == pszTempStr)
    {
        printf("TRAFFIC:gamma_1 error\n");
        return HI_FAILURE;
    }
    s32Temp = Weight(pszTempStr);
    for (i = 0; i < s32Temp; i++)
    {
        g_stSceneautoParam.stTrafficParam.u16GammaTable[s32Offset + i] = MAEWeight[i];
    }
    s32Offset += s32Temp;

    snprintf(szTempStr, sizeof(szTempStr), "TRAFFIC:gamma_2");
    pszTempStr = iniparser_getstr(g_Sceneautodictionary, szTempStr);
    if (NULL == pszTempStr)
    {
        printf("TRAFFIC:gamma_2 error\n");
        return HI_FAILURE;
    }
    s32Temp = Weight(pszTempStr);
    for (i = 0; i < s32Temp; i++)
    {
        g_stSceneautoParam.stTrafficParam.u16GammaTable[s32Offset + i] = MAEWeight[i];
    }

	/**************TRAFFIC:3DnrIsoCount**************/
    s32Temp = 0;
	s32Temp = iniparser_getint(g_Sceneautodictionary, "TRAFFIC:3DnrIsoCount", HI_FAILURE);
    if (HI_FAILURE == s32Temp)
    {
    	printf("TRAFFIC:3DnrIsoCount failed\n");
        return HI_FAILURE;
    }
    g_stSceneautoParam.stTrafficParam.stTraffic3dnr.u323DnrIsoCount = s32Temp;
    CHECK_MALLOC_SIZE(s32Temp);
    g_stSceneautoParam.stTrafficParam.stTraffic3dnr.pu323DnrIsoThresh = (HI_U32*)malloc(s32Temp * sizeof(HI_U32));
    CHECK_NULL_PTR(g_stSceneautoParam.stTrafficParam.stTraffic3dnr.pu323DnrIsoThresh);
	g_stSceneautoParam.stTrafficParam.stTraffic3dnr.pst3dnrParam = (HI_SCENEAUTO_3DNR_S*)malloc(s32Temp * sizeof(HI_SCENEAUTO_3DNR_S));
    CHECK_NULL_PTR(g_stSceneautoParam.stTrafficParam.stTraffic3dnr.pst3dnrParam);

    /**************TRAFFIC:3DnrIsoThresh**************/
    snprintf(szTempStr, sizeof(szTempStr), "TRAFFIC:3DnrIsoThresh");
	pszTempStr = iniparser_getstr(g_Sceneautodictionary, szTempStr);
    if (NULL == pszTempStr)
    {
        printf("TRAFFIC:3DnrIsoThresh failed\n");
        return HI_FAILURE;
    }
    Weight(pszTempStr);
    for (i = 0; i < g_stSceneautoParam.stTrafficParam.stTraffic3dnr.u323DnrIsoCount; i++)
    {
        g_stSceneautoParam.stTrafficParam.stTraffic3dnr.pu323DnrIsoThresh[i] = MAEWeight[i];
    }

    for (i = 0; i < g_stSceneautoParam.stTrafficParam.stTraffic3dnr.u323DnrIsoCount; i++)
	{
		/**************TRAFFIC:3DnrParam**************/
		snprintf(szTempStr, sizeof(szTempStr), "TRAFFIC:3DnrParam_%d", i);
		pszTempStr = iniparser_getstr(g_Sceneautodictionary, szTempStr);
		if (NULL == pszTempStr)
		{
		    printf("TRAFFIC::3DnrParam_%d failed\n", i);
		    return HI_FAILURE;
		}

		s32Temp = Scanf3dnrParam(pszTempStr, (g_stSceneautoParam.stTrafficParam.stTraffic3dnr.pst3dnrParam + i));
		if (HI_FAILURE == s32Temp)
	    {
	        printf("Scanf3dnrParam failed\n");
	        return HI_FAILURE;
	    }
	}


	return HI_SUCCESS;
}

#if 0 /* For Debug */
void PrintNRx( const HI_SCENEAUTO_3DNR_X_S *pst3dnrxParam )
{
    HI_SCENEAUTO_3DNR_X_S *pX = pst3dnrxParam;
    tSceAutoV19yIEy  *pI = &( pX->IEy );
    tSceAutoV19ySFy  *pS =    pX->SFy;
    tSceAutoV19yMDy  *pM =    pX->MDy;
    tSceAutoV19yTFy  *pT =    pX->TFy;

    printf("\n\n                             sizeof( X ) = %d ", sizeof(*pX) );
    printf("\n*******************************************************************************");
    printf("\n                      |                 |                 |    -ktype   %3d )",pS[3].SFkType );

    printf("\n  IES  %4d           |                 |                 |    -krfw   %4d )", pI->IES,pS[3].SFkRfw );
    printf("\n  IET  %4d           |                 |                 |    -kbig   %4d )", pI->IET,pS[3].SFkBig );
    printf("\n  IEB  %4d           |                 |                 |    -kpro   %4d )", pI->IEB,pS[3].kProDD );

    printf("\n                      |                 |                 |                 )" );

    printf("\n  SBS  %4d:%4d:%4d |  %4d:%4d:%4d |  %4d:%4d:%4d |  %4d:%4d:%4d )",  pS[0].SBS[0], pS[0].SBS[1], pS[0].SBS[2],
                                                                                  pS[1].SBS[0], pS[1].SBS[1], pS[1].SBS[2],
                                                                                  pS[2].SBS[0], pS[2].SBS[1], pS[2].SBS[2],
                                                                                  pS[3].SBS[0], pS[3].SBS[1], pS[3].SBS[2] );
    printf("\n  SDS  %4d:%4d:%4d |  %4d:%4d:%4d |  %4d:%4d:%4d |  %4d:%4d:%4d )",  pS[0].SDS[0], pS[0].SDS[1], pS[0].SDS[2],
                                                                                  pS[1].SDS[0], pS[1].SDS[1], pS[1].SDS[2],
                                                                                  pS[2].SDS[0], pS[2].SDS[1], pS[2].SDS[2],
                                                                                  pS[3].SDS[0], pS[3].SDS[1], pS[3].SDS[2] );

    printf("\n                      |                 |                 |                 )" );

    printf("\n  STH  %4d:%4d:%4d |  %4d:%4d:%4d |  %4d:%4d:%4d |  %4d:%4d:%4d )",  pS[0].STH[0], pS[0].STH[1], pS[0].STH[2],
                                                                                  pS[1].STH[0], pS[1].STH[1], pS[1].STH[2],
                                                                                  pS[2].STH[0], pS[2].STH[1], pS[2].STH[2],
                                                                                  pS[3].STH[0], pS[3].STH[1], pS[3].STH[2] );
    printf("\n  SBF  %4d:%4d:%4d |  %4d:%4d:%4d |  %4d:%4d:%4d |  %4d:%4d:%4d )",  pS[0].SBF0,   pS[0].SBF1,   pS[0].SBF2,
                                                                                  pS[1].SBF0,   pS[1].SBF1,   pS[1].SBF2,
                                                                                  pS[2].SBF0,   pS[2].SBF1,   pS[2].SBF2,
                                                                                  pS[3].SBF0,   pS[3].SBF1,   pS[3].SBF2 );
    printf("\n  PRO  %4d:%4d:%4d |  %4d:%4d:%4d |  %4d:%4d:%4d |  %4d:%4d:%4d )",  pS[0].horPro, pS[0].verPro, pS[0].kProDD,
                                                                                     pS[1].horPro, pS[1].verPro, pS[1].kProDD,
                                                                                     pS[2].horPro, pS[2].verPro, pS[2].kProDD,
                                                                                     pS[3].horPro, pS[3].verPro, pS[3].kProDD );

    printf("\n                      |                 |                 |                 )" );

    printf("\n  SSLP           %4d |            %4d |            %4d |            %4d )",  pS[0].SSLP,pS[1].SSLP,pS[2].SSLP,pS[3].SSLP );
    printf("\n  VRTO           %4d |            %4d |            %4d |            %4d )",  pS[0].VRTO,pS[1].VRTO,pS[2].VRTO,pS[3].VRTO );

    printf("\n                      |                 |                 |                 )" );

    printf("\n  En #             on |             %s |          %s |              on )", (pS[3].SFkBig && (pS[3].SFkType >= 2)) ?    "OFF":   " on",
                                                                                                         (pS[3].SFkType == 2)  ? "BYPASS":"    on");
    printf("\n                      |                 |                 |                 )" );

    printf("\n  -ttype         %4d |            %4d |            %4d |            %4d )",  pT[0].TFT, pT[1].TFT, pT[2].TFT, pT[3].TFT );

    printf("\n                      |                 |                 |                 )" );

    if (1) { int  i,  j, m[4][4] = { { -1,-1,-1,-1 }, { -1,-1,-1,-1 }, { -1,-1,-1,-1 }, { -1,-1,-1,-1 } };
           for (i = 0; i < 4; i++) { if (!pT[i].TFT) { for (j = 0; j < 4; j++) m[i][j] = pT[i].SFR[j]; }}

    printf("\n  SFR  %2d:%3d:%3d:%3d |  %2d:%3d:%3d:%3d |  %2d:%3d:%3d:%3d |  %2d:%3d:%3d:%3d )",m[0][0],m[0][1],m[0][2],m[0][3],
                                                                                                m[1][0],m[1][1],m[1][2],m[1][3],
                                                                                                m[2][0],m[2][1],m[2][2],m[2][3],
                                                                                                m[3][0],m[3][1],m[3][2],m[3][3] ); }

    printf("\n                      |                 |                 |                 )" );


    printf("\n  STR            %4d |            %4d |            %4d |            %4d )",(1 == pT[0].TFT) ? pT[0].STR : -1,
                                                                                        (1 == pT[1].TFT) ? pT[1].STR : -1,
                                                                                        (1 == pT[2].TFT) ? pT[2].STR : -1,
                                                                                        (1 == pT[3].TFT) ? pT[3].STR : -1 );

    printf("\n                      |                 |                 |                 )" );

    printf("\n  TFS            %4d |            %4d |            %4d |                 )",(pT[0].TFT > 1) ? pT[0].TFS : -1,
                                                                                            (pT[1].TFT > 1) ? pT[1].TFS : -1,
                                                                                            (pT[2].TFT > 1) ? pT[2].TFS : -1 );

    if (1) { int  i,  j, m[3][4] = { { -1, -1, -1, -1 }, { -1, -1, -1, -1 }, { -1, -1, -1, -1 } };
           for (i = 0; i < 3; i++) { if (2 == pT[i].TFT) { for (j = 0; j < 4; j++) m[i][j] = pT[i].TFR[j]; }}

    printf("\n  TFR  %2d:%3d:%3d:%3d |  %2d:%3d:%3d:%3d |  %2d:%3d:%3d:%3d |                 )", m[0][0],m[0][1],m[0][2],m[0][3],
                                                                                                m[1][0],m[1][1],m[1][2],m[1][3],
                                                                                                m[2][0],m[2][1],m[2][2],m[2][3] ); }

    printf("\n  TSS            %4d |            %4d |            %4d |                 )",(pT[0].TFT > 1) ? pT[0].TSS : -1,
                                                                                         (pT[1].TFT > 1) ? pT[1].TSS : -1,
                                                                                         (pT[2].TFT > 1) ? pT[2].TSS : -1 );

    printf("\n  SDZ            %4d |            %4d |            %4d |                 )",(pT[0].TFT > 1) ? pT[0].SDZ : -1,
                                                                                         (pT[1].TFT > 1) ? pT[1].SDZ : -1,
                                                                                         (pT[2].TFT > 1) ? pT[2].SDZ : -1 );

    printf("\n                      |                 |                 |                 )" );


    if (1) { int  i,  j, m[3][4] = { { -1, -1, -1, -1 }, { -1, -1, -1, -1 }, { -1, -1, -1, -1 } };
           for (i = 0; i < 3; i++) { if (2 == pT[i].TFT) { for (j = 0; j < 4; j++) m[i][j] = pT[i].MTFR[j]; }}

    printf("\n  MTFR %2d:%3d:%3d:%3d |  %2d:%3d:%3d:%3d |  %2d:%3d:%3d:%3d |                 )", m[0][0],m[0][1],m[0][2],m[0][3],
                                                                                                m[1][0],m[1][1],m[1][2],m[1][3],
                                                                                                m[2][0],m[2][1],m[2][2],m[2][3] ); }

    printf("\n  MDDZ           %4d |            %4d |            %4d |                 )",(pT[0].TFT > 1) ? pT[0].MDDZ : -1,
                                                                                         (pT[1].TFT > 1) ? pT[1].MDDZ : -1,
                                                                                         (pT[2].TFT > 1) ? pT[2].MDDZ : -1 );

    printf("\n                      |                 |                 |                 )" );

    printf("\n  MATH           %4d |            %4d |            %4d |                 )", pM[0].MATH,pM[1].MATH,pM[2].MATH );
    printf("\n  MATE           %4d |            %4d |            %4d +-----------------)", pM[0].MATE,pM[1].MATE,pM[2].MATE );
    printf("\n  MABW           %4d |            %4d |            %4d |       -sfc  %3d )", pM[0].MABW,pM[1].MABW,pM[2].MABW, pX->NRc.SFC );
    printf("\n  MATW           %4d |            %4d |            %4d |       -trc  %3d )", pM[0].MATW,pM[1].MATW,pM[2].MATW, pX->NRc.TRC );
    printf("\n  MASW           %4d |            %4d |            %4d |       -tfc  %3d )", pM[0].MASW,pM[1].MASW,pM[2].MASW, pX->NRc.TFC );
    printf("\n  MAXN           %4d |            %4d |            %4d |       -tpc  %3d )", pM[0].MAXN,pM[1].MAXN,pM[2].MAXN, pX->NRc.TPC );

    printf("\n*******************************************************************************\n");
}
#endif

HI_S32 Sceneauto_LoadFaceParam()
{
	HI_S32 s32Temp;
    HI_S32 i;
    HI_S32 s32Offset;
    HI_CHAR szTempStr[64] = {0};
    HI_CHAR* pszTempStr = HI_NULL;

    /**************FACE:au8TextureSt**************/
    pszTempStr = iniparser_getstr(g_Sceneautodictionary, "FACE:au8TextureSt");
    if (NULL == pszTempStr)
    {
        printf("FACE:au8TextureSt error\n");
        return HI_FAILURE;
    }
    Weight(pszTempStr);
    for (i = 0; i < EXPOSURE_LEVEL; i++)
    {
        g_stSceneautoParam.stFaceParam.au8TextureSt[i] = MAEWeight[i];
    }

    /**************FACE:au8EdgeSt**************/
    pszTempStr = iniparser_getstr(g_Sceneautodictionary, "FACE:au8EdgeSt");
    if (NULL == pszTempStr)
    {
        printf("FACE:au8EdgeSt error\n");
        return HI_FAILURE;
    }
    Weight(pszTempStr);
    for (i = 0; i < EXPOSURE_LEVEL; i++)
    {
        g_stSceneautoParam.stFaceParam.au8EdgeSt[i] = MAEWeight[i];
    }

    /**************FACE:au8OverShoot**************/
    pszTempStr = iniparser_getstr(g_Sceneautodictionary, "FACE:au8OverShoot");
    if (NULL == pszTempStr)
    {
        printf("FACE:au8OverShoot error\n");
        return HI_FAILURE;
    }
    Weight(pszTempStr);
    for (i = 0; i < EXPOSURE_LEVEL; i++)
    {
        g_stSceneautoParam.stFaceParam.au8OverShoot[i] = MAEWeight[i];
    }

    /**************FACE:au8UnderShoot**************/
    pszTempStr = iniparser_getstr(g_Sceneautodictionary, "FACE:au8UnderShoot");
    if (NULL == pszTempStr)
    {
        printf("FACE:au8UnderShoot error\n");
        return HI_FAILURE;
    }
    Weight(pszTempStr);
    for (i = 0; i < EXPOSURE_LEVEL; i++)
    {
        g_stSceneautoParam.stFaceParam.au8UnderShoot[i] = MAEWeight[i];
    }

    /**************FACE:au8TextureThd**************/
    pszTempStr = iniparser_getstr(g_Sceneautodictionary, "FACE:au8TextureThd");
    if (NULL == pszTempStr)
    {
        printf("FACE:au8TextureThd error\n");
        return HI_FAILURE;
    }
    Weight(pszTempStr);
    for (i = 0; i < EXPOSURE_LEVEL; i++)
    {
        g_stSceneautoParam.stFaceParam.au8TextureThd[i] = MAEWeight[i];
    }

    /**************FACE:au8EdgeThd**************/
    pszTempStr = iniparser_getstr(g_Sceneautodictionary, "FACE:au8EdgeThd");
    if (NULL == pszTempStr)
    {
        printf("FACE:au8EdgeThd error\n");
        return HI_FAILURE;
    }
    Weight(pszTempStr);
    for (i = 0; i < EXPOSURE_LEVEL; i++)
    {
        g_stSceneautoParam.stFaceParam.au8EdgeThd[i] = MAEWeight[i];
    }

    /**************FACE:au8DetailCtrl**************/
    pszTempStr = iniparser_getstr(g_Sceneautodictionary, "FACE:au8DetailCtrl");
    if (NULL == pszTempStr)
    {
        printf("FACE:au8DetailCtrl error\n");
        return HI_FAILURE;
    }
    Weight(pszTempStr);
    for (i = 0; i < EXPOSURE_LEVEL; i++)
    {
        g_stSceneautoParam.stFaceParam.au8DetailCtrl[i] = MAEWeight[i];
    }

    /**************IR:gamma**************/
    s32Offset = 0;
    snprintf(szTempStr, sizeof(szTempStr), "FACE:gamma_0");
    pszTempStr = iniparser_getstr(g_Sceneautodictionary, szTempStr);
    if (NULL == pszTempStr)
    {
        printf("FACE:gamma_0 error\n");
        return HI_FAILURE;
    }
    s32Temp = Weight(pszTempStr);
    for (i = 0; i < s32Temp; i++)
    {
        g_stSceneautoParam.stFaceParam.u16GammaTable[i] = MAEWeight[i];
    }
    s32Offset += s32Temp;

    snprintf(szTempStr, sizeof(szTempStr), "FACE:gamma_1");
    pszTempStr = iniparser_getstr(g_Sceneautodictionary, szTempStr);
    if (NULL == pszTempStr)
    {
        printf("FACE:gamma_1 error\n");
        return HI_FAILURE;
    }
    s32Temp = Weight(pszTempStr);
    for (i = 0; i < s32Temp; i++)
    {
        g_stSceneautoParam.stFaceParam.u16GammaTable[s32Offset + i] = MAEWeight[i];
    }
    s32Offset += s32Temp;

    snprintf(szTempStr, sizeof(szTempStr), "FACE:gamma_2");
    pszTempStr = iniparser_getstr(g_Sceneautodictionary, szTempStr);
    if (NULL == pszTempStr)
    {
        printf("FACE:gamma_2 error\n");
        return HI_FAILURE;
    }
    s32Temp = Weight(pszTempStr);
    for (i = 0; i < s32Temp; i++)
    {
        g_stSceneautoParam.stFaceParam.u16GammaTable[s32Offset + i] = MAEWeight[i];
    }

	/**************FACE:3DnrIsoCount**************/
    s32Temp = 0;
	s32Temp = iniparser_getint(g_Sceneautodictionary, "FACE:3DnrIsoCount", HI_FAILURE);
    if (HI_FAILURE == s32Temp)
    {
    	printf("FACE:3DnrIsoCount failed\n");
        return HI_FAILURE;
    }
    g_stSceneautoParam.stFaceParam.stFace3dnrx.u323DnrIsoCount = s32Temp;
    CHECK_MALLOC_SIZE(s32Temp);
    g_stSceneautoParam.stFaceParam.stFace3dnrx.pu323DnrIsoThresh = (HI_U32*)malloc(s32Temp * sizeof(HI_U32));
    CHECK_NULL_PTR(g_stSceneautoParam.stFaceParam.stFace3dnrx.pu323DnrIsoThresh);
	g_stSceneautoParam.stFaceParam.stFace3dnrx.pst3dnrxParam = (HI_SCENEAUTO_3DNR_X_S*)malloc(s32Temp * sizeof(HI_SCENEAUTO_3DNR_X_S));
    CHECK_NULL_PTR(g_stSceneautoParam.stFaceParam.stFace3dnrx.pst3dnrxParam);

    /**************FACE:3DnrIsoThresh**************/
    snprintf(szTempStr, sizeof(szTempStr), "FACE:3DnrIsoThresh");
	pszTempStr = iniparser_getstr(g_Sceneautodictionary, szTempStr);
    if (NULL == pszTempStr)
    {
        printf("FACE:3DnrIsoThresh failed\n");
        return HI_FAILURE;
    }
    Weight(pszTempStr);
    for (i = 0; i < g_stSceneautoParam.stFaceParam.stFace3dnrx.u323DnrIsoCount; i++)
    {
        g_stSceneautoParam.stFaceParam.stFace3dnrx.pu323DnrIsoThresh[i] = MAEWeight[i];
    }

    for (i = 0; i < g_stSceneautoParam.stFaceParam.stFace3dnrx.u323DnrIsoCount; i++)
	{
		/**************FACE:3DnrParam**************/
		snprintf(szTempStr, sizeof(szTempStr), "FACE:3DnrXParam_%d", i);
		pszTempStr = iniparser_getstr(g_Sceneautodictionary, szTempStr);
		if (NULL == pszTempStr)
		{
		    printf("FACE::3DnrParam_%d failed\n", i);
		    return HI_FAILURE;
		}

        memset((g_stSceneautoParam.stFaceParam.stFace3dnrx.pst3dnrxParam + i), 0, sizeof(HI_SCENEAUTO_3DNR_X_S));
		s32Temp = Scanf3dnrxParam(pszTempStr, (g_stSceneautoParam.stFaceParam.stFace3dnrx.pst3dnrxParam + i));
		if (HI_FAILURE == s32Temp)
	    {
	        printf("Scanf3dnrParam failed\n");
	        return HI_FAILURE;
	    }
	}

#if 0 /* For Debug */
        for (i = 0; i < g_stSceneautoParam.stFaceParam.stFace3dnrx.u323DnrIsoCount; i++)
        {
            printf("ISO:%d******************************************\n", g_stSceneautoParam.stFaceParam.stFace3dnrx.pu323DnrIsoThresh[i]);
            PrintNRx(&g_stSceneautoParam.stFaceParam.stFace3dnrx.pst3dnrxParam[i]);
        }
#endif

	return HI_SUCCESS;
}

HI_S32 Sceneauto_LoadHLCParam()
{
	HI_S32 s32Temp;
    HI_S32 i;
    HI_S32 s32Offset;
    HI_CHAR szTempStr[64] = {0};
    HI_CHAR* pszTempStr = HI_NULL;

	/**************HLC:u8ExpRatioType**************/
    s32Temp = 0;
    s32Temp = iniparser_getint(g_Sceneautodictionary, "HLC:u8ExpRatioType", HI_FAILURE);
    if (HI_FAILURE == s32Temp)
    {
        printf("HLC:u8ExpRatioType failed\n");
        return HI_FAILURE;
    }
    g_stSceneautoParam.stHlcParam.u8ExpRatioType = s32Temp;

	/**************HLC:au32ExpRatio**************/
    pszTempStr = iniparser_getstr(g_Sceneautodictionary, "HLC:au32ExpRatio");
    if (NULL == pszTempStr)
    {
        printf("HLC:au32ExpRatio error\n");
        return HI_FAILURE;
    }
    Weight(pszTempStr);
    for (i = 0; i < 3; i++)
    {
    	g_stSceneautoParam.stHlcParam.au32ExpRatio[i] = MAEWeight[i];
    }

    /**************HLC:u32ExpRatioMax**************/
    s32Temp = 0;
    s32Temp = iniparser_getint(g_Sceneautodictionary, "HLC:u32ExpRatioMax", HI_FAILURE);
    if (HI_FAILURE == s32Temp)
    {
        printf("HLC:u32ExpRatioMax failed\n");
        return HI_FAILURE;
    }
    g_stSceneautoParam.stHlcParam.u32ExpRatioMax = s32Temp;

    /**************HLC:u32ExpRatioMin**************/
    s32Temp = 0;
    s32Temp = iniparser_getint(g_Sceneautodictionary, "HLC:u32ExpRatioMin", HI_FAILURE);
    if (HI_FAILURE == s32Temp)
    {
        printf("HLC:u32ExpRatioMin failed\n");
        return HI_FAILURE;
    }
    g_stSceneautoParam.stHlcParam.u32ExpRatioMin = s32Temp;

    /**************HLC:ExpCompensation**************/
    s32Temp = 0;
    s32Temp = iniparser_getint(g_Sceneautodictionary, "HLC:ExpCompensation", HI_FAILURE);
    if (HI_FAILURE == s32Temp)
    {
    	printf("HLC:ExpCompensation failed\n");
    }
    g_stSceneautoParam.stHlcParam.u8ExpCompensation = s32Temp;

    /**************HLC:BlackDelayFrame**************/
    s32Temp = 0;
    s32Temp = iniparser_getint(g_Sceneautodictionary, "HLC:BlackDelayFrame", HI_FAILURE);
    if (HI_FAILURE == s32Temp)
    {
        printf("HLC:BlackDelayFrame failed\n");
        return HI_FAILURE;
    }
    g_stSceneautoParam.stHlcParam.u16BlackDelayFrame = s32Temp;

    /**************HLC:WhiteDelayFrame**************/
    s32Temp = 0;
    s32Temp = iniparser_getint(g_Sceneautodictionary, "HLC:WhiteDelayFrame", HI_FAILURE);
    if (HI_FAILURE == s32Temp)
    {
        printf("HLC:WhiteDelayFrame failed\n");
        return HI_FAILURE;
    }
    g_stSceneautoParam.stHlcParam.u16WhiteDelayFrame = s32Temp;

    /**************HLC:u8Speed**************/
    s32Temp = 0;
    s32Temp = iniparser_getint(g_Sceneautodictionary, "HLC:u8Speed", HI_FAILURE);
    if (HI_FAILURE == s32Temp)
    {
        printf("HLC:u8Speed failed\n");
        return HI_FAILURE;
    }
    g_stSceneautoParam.stHlcParam.u8Speed = s32Temp;

    /**************HLC:HistRatioSlope**************/
    s32Temp = 0;
    s32Temp = iniparser_getint(g_Sceneautodictionary, "HLC:HistRatioSlope", HI_FAILURE);
    if (HI_FAILURE == s32Temp)
    {
        printf("HLC:HistRatioSlope failed\n");
        return HI_FAILURE;
    }
    g_stSceneautoParam.stHlcParam.u16HistRatioSlope = s32Temp;

    /**************HLC:MaxHistOffset**************/
    s32Temp = 0;
    s32Temp = iniparser_getint(g_Sceneautodictionary, "HLC:MaxHistOffset", HI_FAILURE);
    if (HI_FAILURE == s32Temp)
    {
        printf("HLC:MaxHistOffset failed\n");
        return HI_FAILURE;
    }
    g_stSceneautoParam.stHlcParam.u8MaxHistOffset = s32Temp;

    /**************HLC:u8Tolerance**************/
    s32Temp = 0;
    s32Temp = iniparser_getint(g_Sceneautodictionary, "HLC:u8Tolerance", HI_FAILURE);
    if (HI_FAILURE == s32Temp)
    {
        printf("HLC:u8Tolerance failed\n");
        return HI_FAILURE;
    }
    g_stSceneautoParam.stHlcParam.u8Tolerance = s32Temp;

	/**************HLC:DCIEnable**************/
    s32Temp = 0;
    s32Temp = iniparser_getint(g_Sceneautodictionary, "HLC:DCIEnable", HI_FAILURE);
    if (HI_FAILURE == s32Temp)
    {
        printf("HLC:DCIEnable failed\n");
        return HI_FAILURE;
    }
    g_stSceneautoParam.stHlcParam.bDCIEnable = (HI_BOOL)s32Temp;

    /**************HLC:DCIBlackGain**************/
    s32Temp = 0;
    s32Temp = iniparser_getint(g_Sceneautodictionary, "HLC:DCIBlackGain", HI_FAILURE);
    if (HI_FAILURE == s32Temp)
    {
        printf("HLC:DCIBlackGain failed\n");
        return HI_FAILURE;
    }
    g_stSceneautoParam.stHlcParam.u32DCIBlackGain = s32Temp;

    /**************HLC:DCIContrastGain**************/
    s32Temp = 0;
    s32Temp = iniparser_getint(g_Sceneautodictionary, "HLC:DCIContrastGain", HI_FAILURE);
    if (HI_FAILURE == s32Temp)
    {
        printf("HLC:DCIContrastGain failed\n");
        return HI_FAILURE;
    }
    g_stSceneautoParam.stHlcParam.u32DCIContrastGain = s32Temp;

    /**************HLC:DCILightGain**************/
    s32Temp = 0;
    s32Temp = iniparser_getint(g_Sceneautodictionary, "HLC:DCILightGain", HI_FAILURE);
    if (HI_FAILURE == s32Temp)
    {
        printf("HLC:DCILightGain failed\n");
        return HI_FAILURE;
    }
    g_stSceneautoParam.stHlcParam.u32DCILightGain = s32Temp;

    /**************HLC:DRCEnable**************/
    s32Temp = 0;
    s32Temp = iniparser_getint(g_Sceneautodictionary, "HLC:DRCEnable", HI_FAILURE);
    if (HI_FAILURE == s32Temp)
    {
        printf("HLC:DRCEnable failed\n");
        return HI_FAILURE;
    }
    g_stSceneautoParam.stHlcParam.bDRCEnable = (HI_BOOL)s32Temp;

    /**************HLC:DRCManulEnable**************/
    s32Temp = 0;
    s32Temp = iniparser_getint(g_Sceneautodictionary, "HLC:DRCManulEnable", HI_FAILURE);
    if (HI_FAILURE == s32Temp)
    {
        printf("HLC:DRCManulEnable failed\n");
        return HI_FAILURE;
    }
    g_stSceneautoParam.stHlcParam.bDRCManulEnable = (HI_BOOL)s32Temp;

    /**************HLC:DRCStrengthTarget**************/
    s32Temp = 0;
    s32Temp = iniparser_getint(g_Sceneautodictionary, "HLC:DRCStrengthTarget", HI_FAILURE);
    if (HI_FAILURE == s32Temp)
    {
        printf("HLC:DRCStrengthTarget failed\n");
        return HI_FAILURE;
    }
    g_stSceneautoParam.stHlcParam.u32DRCStrengthTarget = s32Temp;

    /**************HLC:DRCAutoStrength**************/
    s32Temp = 0;
    s32Temp = iniparser_getint(g_Sceneautodictionary, "HLC:DRCAutoStrength", HI_FAILURE);
    if (HI_FAILURE == s32Temp)
    {
        printf("HLC:DRCAutoStrength failed\n");
        return HI_FAILURE;
    }
    g_stSceneautoParam.stHlcParam.u32DRCAutoStrength = s32Temp;

    /**************HLC:u32WhiteLevel**************/
    s32Temp = 0;
    s32Temp = iniparser_getint(g_Sceneautodictionary, "HLC:u32WhiteLevel", HI_FAILURE);
    if (HI_FAILURE == s32Temp)
    {
        printf("HLC:u32WhiteLevel failed\n");
        return HI_FAILURE;
    }
    g_stSceneautoParam.stHlcParam.u32WhiteLevel = s32Temp;

    /**************HLC:u32BlackLevel**************/
    s32Temp = 0;
    s32Temp = iniparser_getint(g_Sceneautodictionary, "HLC:u32BlackLevel", HI_FAILURE);
    if (HI_FAILURE == s32Temp)
    {
        printf("HLC:u32BlackLevel failed\n");
        return HI_FAILURE;
    }
    g_stSceneautoParam.stHlcParam.u32BlackLevel = s32Temp;

    /**************HLC:u32Asymmetry**************/
    s32Temp = 0;
    s32Temp = iniparser_getint(g_Sceneautodictionary, "HLC:u32Asymmetry", HI_FAILURE);
    if (HI_FAILURE == s32Temp)
    {
        printf("HLC:u32Asymmetry failed\n");
        return HI_FAILURE;
    }
    g_stSceneautoParam.stHlcParam.u32Asymmetry = s32Temp;

    /**************HLC:u32BrightEnhance**************/
    s32Temp = 0;
    s32Temp = iniparser_getint(g_Sceneautodictionary, "HLC:u32BrightEnhance", HI_FAILURE);
    if (HI_FAILURE == s32Temp)
    {
        printf("HLC:u32BrightEnhance failed\n");
        return HI_FAILURE;
    }
    g_stSceneautoParam.stHlcParam.u32BrightEnhance = s32Temp;

    /**************HLC:u8FilterMux**************/
    s32Temp = 0;
    s32Temp = iniparser_getint(g_Sceneautodictionary, "HLC:u8FilterMux", HI_FAILURE);
    if (HI_FAILURE == s32Temp)
    {
        printf("HLC:u8FilterMux failed\n");
        return HI_FAILURE;
    }
    g_stSceneautoParam.stHlcParam.u8FilterMux = s32Temp;

    /**************HLC:u32SlopeMax**************/
    s32Temp = 0;
    s32Temp = iniparser_getint(g_Sceneautodictionary, "HLC:u32SlopeMax", HI_FAILURE);
    if (HI_FAILURE == s32Temp)
    {
        printf("HLC:u32SlopeMax failed\n");
        return HI_FAILURE;
    }
    g_stSceneautoParam.stHlcParam.u32SlopeMax = s32Temp;

    /**************HLC:u32SlopeMin**************/
    s32Temp = 0;
    s32Temp = iniparser_getint(g_Sceneautodictionary, "HLC:u32SlopeMin", HI_FAILURE);
    if (HI_FAILURE == s32Temp)
    {
        printf("HLC:u32SlopeMin failed\n");
        return HI_FAILURE;
    }
    g_stSceneautoParam.stHlcParam.u32SlopeMin = s32Temp;

    /**************HLC:u32VarianceIntensity**************/
    s32Temp = 0;
    s32Temp = iniparser_getint(g_Sceneautodictionary, "HLC:u32VarianceIntensity", HI_FAILURE);
    if (HI_FAILURE == s32Temp)
    {
        printf("HLC:u32VarianceIntensity failed\n");
        return HI_FAILURE;
    }
    g_stSceneautoParam.stHlcParam.u32VarianceIntensity = s32Temp;

    /**************HLC:u32VarianceSpace**************/
    s32Temp = 0;
    s32Temp = iniparser_getint(g_Sceneautodictionary, "HLC:u32VarianceSpace", HI_FAILURE);
    if (HI_FAILURE == s32Temp)
    {
        printf("HLC:u32VarianceSpace failed\n");
        return HI_FAILURE;
    }
    g_stSceneautoParam.stHlcParam.u32VarianceSpace = s32Temp;

    /**************HLC:u32BrightPr**************/
    s32Temp = 0;
    s32Temp = iniparser_getint(g_Sceneautodictionary, "HLC:u32BrightPr", HI_FAILURE);
    if (HI_FAILURE == s32Temp)
    {
        printf("HLC:u32BrightPr failed\n");
        return HI_FAILURE;
    }
    g_stSceneautoParam.stHlcParam.u32BrightPr = s32Temp;

    /**************HLC:u32Contrast**************/
    s32Temp = 0;
    s32Temp = iniparser_getint(g_Sceneautodictionary, "HLC:u32Contrast", HI_FAILURE);
    if (HI_FAILURE == s32Temp)
    {
        printf("HLC:u32Contrast failed\n");
        return HI_FAILURE;
    }
    g_stSceneautoParam.stHlcParam.u32Contrast = s32Temp;

    /**************HLC:u32DarkEnhance**************/
    s32Temp = 0;
    s32Temp = iniparser_getint(g_Sceneautodictionary, "HLC:u32DarkEnhance", HI_FAILURE);
    if (HI_FAILURE == s32Temp)
    {
        printf("HLC:u32DarkEnhance failed\n");
        return HI_FAILURE;
    }
    g_stSceneautoParam.stHlcParam.u32DarkEnhance = s32Temp;

    /**************HLC:u32Svariance**************/
    s32Temp = 0;
    s32Temp = iniparser_getint(g_Sceneautodictionary, "HLC:u32Svariance", HI_FAILURE);
    if (HI_FAILURE == s32Temp)
    {
        printf("HLC:u32Svariance failed\n");
        return HI_FAILURE;
    }
    g_stSceneautoParam.stHlcParam.u32Svariance = s32Temp;

    /**************HLC:Saturation**************/
    pszTempStr = iniparser_getstr(g_Sceneautodictionary, "HLC:Saturation");
    if (NULL == pszTempStr)
    {
        printf("HLC:Saturation error\n");
        return HI_FAILURE;
    }
    Weight(pszTempStr);
    for (i = 0; i < 16; i++)
    {
        g_stSceneautoParam.stHlcParam.u8Saturation[i] = MAEWeight[i];
    }

    /**************HLC:gamma**************/
    s32Offset = 0;
    snprintf(szTempStr, sizeof(szTempStr), "HLC:gamma_0");
    pszTempStr = iniparser_getstr(g_Sceneautodictionary, szTempStr);
    if (NULL == pszTempStr)
    {
        printf("HLC:gamma_0 error\n");
        return HI_FAILURE;
    }
    s32Temp = Weight(pszTempStr);
    for (i = 0; i < s32Temp; i++)
    {
        g_stSceneautoParam.stHlcParam.u16GammaTable[i] = MAEWeight[i];
    }
    s32Offset += s32Temp;

    snprintf(szTempStr, sizeof(szTempStr), "HLC:gamma_1");
    pszTempStr = iniparser_getstr(g_Sceneautodictionary, szTempStr);
    if (NULL == pszTempStr)
    {
        printf("HLC:gamma_1 error\n");
        return HI_FAILURE;
    }
    s32Temp = Weight(pszTempStr);
    for (i = 0; i < s32Temp; i++)
    {
        g_stSceneautoParam.stHlcParam.u16GammaTable[s32Offset + i] = MAEWeight[i];
    }
    s32Offset += s32Temp;

    snprintf(szTempStr, sizeof(szTempStr), "HLC:gamma_2");
    pszTempStr = iniparser_getstr(g_Sceneautodictionary, szTempStr);
    if (NULL == pszTempStr)
    {
        printf("HLC:gamma_2 error\n");
        return HI_FAILURE;
    }
    s32Temp = Weight(pszTempStr);
    for (i = 0; i < s32Temp; i++)
    {
        g_stSceneautoParam.stHlcParam.u16GammaTable[s32Offset + i] = MAEWeight[i];
    }

    /**************HLC:3DnrIsoCount**************/
    s32Temp = 0;
	s32Temp = iniparser_getint(g_Sceneautodictionary, "HLC:3DnrIsoCount", HI_FAILURE);
    if (HI_FAILURE == s32Temp)
    {
    	printf("HLC:3DnrIsoCount failed\n");
        return HI_FAILURE;
    }
    g_stSceneautoParam.stHlcParam.stHlc3dnr.u323DnrIsoCount = s32Temp;
    CHECK_MALLOC_SIZE(s32Temp);
    g_stSceneautoParam.stHlcParam.stHlc3dnr.pu323DnrIsoThresh = (HI_U32*)malloc(s32Temp * sizeof(HI_U32));
    CHECK_NULL_PTR(g_stSceneautoParam.stHlcParam.stHlc3dnr.pu323DnrIsoThresh);
    g_stSceneautoParam.stHlcParam.stHlc3dnr.pst3dnrParam = (HI_SCENEAUTO_3DNR_S*)malloc(s32Temp * sizeof(HI_SCENEAUTO_3DNR_S));
    CHECK_NULL_PTR(g_stSceneautoParam.stHlcParam.stHlc3dnr.pst3dnrParam);

    /**************HLC:3DnrIsoThresh**************/
    snprintf(szTempStr, sizeof(szTempStr), "HLC:3DnrIsoThresh");
	pszTempStr = iniparser_getstr(g_Sceneautodictionary, szTempStr);
    if (NULL == pszTempStr)
    {
        printf("HLC:3DnrIsoThresh failed\n");
        return HI_FAILURE;
    }
    Weight(pszTempStr);
    for (i = 0; i < g_stSceneautoParam.stHlcParam.stHlc3dnr.u323DnrIsoCount; i++)
    {
        g_stSceneautoParam.stHlcParam.stHlc3dnr.pu323DnrIsoThresh[i] = MAEWeight[i];
    }


    for (i = 0; i < g_stSceneautoParam.stHlcParam.stHlc3dnr.u323DnrIsoCount; i++)
	{
		/**************HLC:3DnrParam**************/
		snprintf(szTempStr, sizeof(szTempStr), "HLC:3DnrParam_%d", i);
		pszTempStr = iniparser_getstr(g_Sceneautodictionary, szTempStr);
		if (NULL == pszTempStr)
		{
		    printf("HLC::3DnrParam_%d failed\n", i);
		    return HI_FAILURE;
		}

		s32Temp = Scanf3dnrParam(pszTempStr, (g_stSceneautoParam.stHlcParam.stHlc3dnr.pst3dnrParam + i));
		if (HI_FAILURE == s32Temp)
	    {
	        printf("Scanf3dnrParam failed\n");
	        return HI_FAILURE;
	    }
	}

	return HI_SUCCESS;
}

HI_S32 Sceneauto_LoadIRParam()
{
	HI_S32 s32Temp;
    HI_S32 i, j;
    HI_S32 s32Offset;
    HI_CHAR szTempStr[64] = {0};
    HI_CHAR* pszTempStr = HI_NULL;

	/**************IR:ExpCount**************/
	s32Temp = 0;
    s32Temp = iniparser_getint(g_Sceneautodictionary, "IR:ExpCount", HI_FAILURE);
    if (HI_FAILURE == s32Temp)
    {
        printf("IR:ExpCount failed\n");
        return HI_FAILURE;
    }
    g_stSceneautoParam.stIrParam.u32ExpCount = s32Temp;

    CHECK_MALLOC_SIZE(s32Temp);
	g_stSceneautoParam.stIrParam.pu32ExpThreshLtoH = (HI_U32*)malloc(s32Temp * sizeof(HI_U32));
    CHECK_NULL_PTR(g_stSceneautoParam.stIrParam.pu32ExpThreshLtoH);
    g_stSceneautoParam.stIrParam.pu32ExpThreshHtoL = (HI_U32*)malloc(s32Temp * sizeof(HI_U32));
    CHECK_NULL_PTR(g_stSceneautoParam.stIrParam.pu32ExpThreshHtoL);
    g_stSceneautoParam.stIrParam.pu8ExpCompensation = (HI_U8*)malloc(s32Temp * sizeof(HI_U8));
    CHECK_NULL_PTR(g_stSceneautoParam.stIrParam.pu8ExpCompensation);
    g_stSceneautoParam.stIrParam.pu8MaxHistOffset = (HI_U8*)malloc(s32Temp * sizeof(HI_U8));
	CHECK_NULL_PTR(g_stSceneautoParam.stIrParam.pu8MaxHistOffset);

	/**************IR:ExpThreshLtoH**************/
    pszTempStr = iniparser_getstr(g_Sceneautodictionary, "IR:ExpThreshLtoH");
    if (NULL == pszTempStr)
    {
        printf("IR:ExpThreshLtoH error\n");
        return HI_FAILURE;
    }
    Weight(pszTempStr);
    for (i = 0; i < g_stSceneautoParam.stIrParam.u32ExpCount; i++)
    {
        g_stSceneautoParam.stIrParam.pu32ExpThreshLtoH[i] = MAEWeight[i];
    }

    /**************IR:ExpThreshHtoL**************/
    pszTempStr = iniparser_getstr(g_Sceneautodictionary, "IR:ExpThreshHtoL");
    if (NULL == pszTempStr)
    {
        printf("IR:ExpThreshHtoL error\n");
        return HI_FAILURE;
    }
    Weight(pszTempStr);
    for (i = 0; i < g_stSceneautoParam.stIrParam.u32ExpCount; i++)
    {
        g_stSceneautoParam.stIrParam.pu32ExpThreshHtoL[i] = MAEWeight[i];
    }

    /**************IR:ExpCompensation**************/
    pszTempStr = iniparser_getstr(g_Sceneautodictionary, "IR:ExpCompensation");
    if (NULL == pszTempStr)
    {
        printf("IR:ExpCompensation error\n");
        return HI_FAILURE;
    }
    Weight(pszTempStr);
    for (i = 0; i < g_stSceneautoParam.stIrParam.u32ExpCount; i++)
    {
        g_stSceneautoParam.stIrParam.pu8ExpCompensation[i] = MAEWeight[i];
    }

    /**************IR:MaxHistOffset**************/
    pszTempStr = iniparser_getstr(g_Sceneautodictionary, "IR:MaxHistOffset");
    if (NULL == pszTempStr)
    {
        printf("IR:MaxHistOffset error\n");
        return HI_FAILURE;
    }
    Weight(pszTempStr);
    for (i = 0; i < g_stSceneautoParam.stIrParam.u32ExpCount; i++)
    {
        g_stSceneautoParam.stIrParam.pu8MaxHistOffset[i] = MAEWeight[i];
    }

    /**************IR:u16HistRatioSlope**************/
    s32Temp = 0;
    s32Temp = iniparser_getint(g_Sceneautodictionary, "IR:u16HistRatioSlope", HI_FAILURE);
    if (HI_FAILURE == s32Temp)
    {
        printf("IR:u16HistRatioSlope failed\n");
        return HI_FAILURE;
    }
    g_stSceneautoParam.stIrParam.u16HistRatioSlope = s32Temp;

	/**************IR:BlackDelayFrame**************/
    s32Temp = 0;
    s32Temp = iniparser_getint(g_Sceneautodictionary, "IR:BlackDelayFrame", HI_FAILURE);
    if (HI_FAILURE == s32Temp)
    {
        printf("IR:BlackDelayFrame failed\n");
        return HI_FAILURE;
    }
    g_stSceneautoParam.stIrParam.u16BlackDelayFrame = s32Temp;

    /**************IR:WhiteDelayFrame**************/
    s32Temp = 0;
    s32Temp = iniparser_getint(g_Sceneautodictionary, "IR:WhiteDelayFrame", HI_FAILURE);
    if (HI_FAILURE == s32Temp)
    {
        printf("IR:WhiteDelayFrame failed\n");
        return HI_FAILURE;
    }
    g_stSceneautoParam.stIrParam.u16WhiteDelayFrame = s32Temp;

    /**************IR:u8Tolerance**************/
    s32Temp = 0;
    s32Temp = iniparser_getint(g_Sceneautodictionary, "IR:u8Tolerance", HI_FAILURE);
    if (HI_FAILURE == s32Temp)
    {
        printf("IR:u8Tolerance failed\n");
        return HI_FAILURE;
    }
    g_stSceneautoParam.stIrParam.u8Tolerance = s32Temp;

    /**************IR:u8Speed**************/
    s32Temp = 0;
    s32Temp = iniparser_getint(g_Sceneautodictionary, "IR:u8Speed", HI_FAILURE);
    if (HI_FAILURE == s32Temp)
    {
        printf("IR:u8Speed failed\n");
        return HI_FAILURE;
    }
    g_stSceneautoParam.stIrParam.u8Speed = s32Temp;


	//IR:WEIGHT
    for(i = 0; i < AE_WEIGHT_ROW; i++)
    {
        snprintf(szTempStr, sizeof(szTempStr), "IR:expweight_%d", i);
        pszTempStr = iniparser_getstr(g_Sceneautodictionary, szTempStr);
        if (NULL == pszTempStr)
        {
            printf("IR:expweight_%d error\n", i);
            return HI_FAILURE;
        }
        Weight(pszTempStr);
        for (j = 0; j < AE_WEIGHT_COLUMN; j++)
        {
            g_stSceneautoParam.stIrParam.au8Weight[i][j] = MAEWeight[j];
        }
    }

    /**************IR:DCIEnable**************/
    s32Temp = 0;
    s32Temp = iniparser_getint(g_Sceneautodictionary, "IR:DCIEnable", HI_FAILURE);
    if (HI_FAILURE == s32Temp)
    {
        printf("IR:DCIEnable failed\n");
        return HI_FAILURE;
    }
    g_stSceneautoParam.stIrParam.bDCIEnable = (HI_BOOL)s32Temp;

    /**************IR:DCIBlackGain**************/
    s32Temp = 0;
    s32Temp = iniparser_getint(g_Sceneautodictionary, "IR:DCIBlackGain", HI_FAILURE);
    if (HI_FAILURE == s32Temp)
    {
        printf("IR:DCIBlackGain failed\n");
        return HI_FAILURE;
    }
    g_stSceneautoParam.stIrParam.u32DCIBlackGain = (HI_BOOL)s32Temp;

    /**************IR:DCIContrastGain**************/
    s32Temp = 0;
    s32Temp = iniparser_getint(g_Sceneautodictionary, "IR:DCIContrastGain", HI_FAILURE);
    if (HI_FAILURE == s32Temp)
    {
        printf("IR:DCIContrastGain failed\n");
        return HI_FAILURE;
    }
    g_stSceneautoParam.stIrParam.u32DCIContrastGain = (HI_BOOL)s32Temp;

    /**************IR:DCILightGain**************/
    s32Temp = 0;
    s32Temp = iniparser_getint(g_Sceneautodictionary, "IR:DCILightGain", HI_FAILURE);
    if (HI_FAILURE == s32Temp)
    {
        printf("IR:DCILightGain failed\n");
        return HI_FAILURE;
    }
    g_stSceneautoParam.stIrParam.u32DCILightGain = (HI_BOOL)s32Temp;

    /**************IR:IRu16Slope**************/
    s32Temp = 0;
    s32Temp = iniparser_getint(g_Sceneautodictionary, "IR:IRu16Slope", HI_FAILURE);
    if (HI_FAILURE == s32Temp)
    {
        printf("IR:IRu16Slope failed\n");
        return HI_FAILURE;
    }
    g_stSceneautoParam.stIrParam.u16Slope = (HI_BOOL)s32Temp;

    /**************IR:au16LumThresh**************/
    pszTempStr = iniparser_getstr(g_Sceneautodictionary, "IR:au16LumThresh");
    if (NULL == pszTempStr)
    {
        printf("IR:au16LumThresh error\n");
        return HI_FAILURE;
    }
    Weight(pszTempStr);
    for (i = 0; i < EXPOSURE_LEVEL; i++)
    {
        g_stSceneautoParam.stIrParam.au16LumThresh[i] = MAEWeight[i];
    }

    /**************IR:au8SharpenD**************/
    pszTempStr = iniparser_getstr(g_Sceneautodictionary, "IR:au8SharpenD");
    if (NULL == pszTempStr)
    {
        printf("IR:au8SharpenD error\n");
        return HI_FAILURE;
    }
    Weight(pszTempStr);
    for (i = 0; i < EXPOSURE_LEVEL; i++)
    {
        g_stSceneautoParam.stIrParam.au8SharpenD[i] = MAEWeight[i];
    }

    /**************IR:au8SharpenUd**************/
    pszTempStr = iniparser_getstr(g_Sceneautodictionary, "IR:au8SharpenUd");
    if (NULL == pszTempStr)
    {
        printf("IR:au8SharpenUd error\n");
        return HI_FAILURE;
    }
    Weight(pszTempStr);
    for (i = 0; i < EXPOSURE_LEVEL; i++)
    {
        g_stSceneautoParam.stIrParam.au8SharpenUd[i] = MAEWeight[i];
    }

    /**************IR:au8TextureSt**************/
    pszTempStr = iniparser_getstr(g_Sceneautodictionary, "IR:au8TextureSt");
    if (NULL == pszTempStr)
    {
        printf("IR:au8TextureSt error\n");
        return HI_FAILURE;
    }
    Weight(pszTempStr);
    for (i = 0; i < EXPOSURE_LEVEL; i++)
    {
        g_stSceneautoParam.stIrParam.au8TextureSt[i] = MAEWeight[i];
    }

    /**************IR:au8EdgeSt**************/
    pszTempStr = iniparser_getstr(g_Sceneautodictionary, "IR:au8EdgeSt");
    if (NULL == pszTempStr)
    {
        printf("IR:au8EdgeSt error\n");
        return HI_FAILURE;
    }
    Weight(pszTempStr);
    for (i = 0; i < EXPOSURE_LEVEL; i++)
    {
        g_stSceneautoParam.stIrParam.au8EdgeSt[i] = MAEWeight[i];
    }

    /**************IR:au8OverShoot**************/
    pszTempStr = iniparser_getstr(g_Sceneautodictionary, "IR:au8OverShoot");
    if (NULL == pszTempStr)
    {
        printf("IR:au8OverShoot error\n");
        return HI_FAILURE;
    }
    Weight(pszTempStr);
    for (i = 0; i < EXPOSURE_LEVEL; i++)
    {
        g_stSceneautoParam.stIrParam.au8OverShoot[i] = MAEWeight[i];
    }

    /**************IR:au8UnderShoot**************/
    pszTempStr = iniparser_getstr(g_Sceneautodictionary, "IR:au8UnderShoot");
    if (NULL == pszTempStr)
    {
        printf("IR:au8UnderShoot error\n");
        return HI_FAILURE;
    }
    Weight(pszTempStr);
    for (i = 0; i < EXPOSURE_LEVEL; i++)
    {
        g_stSceneautoParam.stIrParam.au8UnderShoot[i] = MAEWeight[i];
    }

    /**************IR:gamma**************/
    s32Offset = 0;
    snprintf(szTempStr, sizeof(szTempStr), "IR:gamma_0");
    pszTempStr = iniparser_getstr(g_Sceneautodictionary, szTempStr);
    if (NULL == pszTempStr)
    {
        printf("IR:gamma_0 error\n");
        return HI_FAILURE;
    }
    s32Temp = Weight(pszTempStr);
    for (i = 0; i < s32Temp; i++)
    {
        g_stSceneautoParam.stIrParam.u16GammaTable[i] = MAEWeight[i];
    }
    s32Offset += s32Temp;

    snprintf(szTempStr, sizeof(szTempStr), "IR:gamma_1");
    pszTempStr = iniparser_getstr(g_Sceneautodictionary, szTempStr);
    if (NULL == pszTempStr)
    {
        printf("IR:gamma_1 error\n");
        return HI_FAILURE;
    }
    s32Temp = Weight(pszTempStr);
    for (i = 0; i < s32Temp; i++)
    {
        g_stSceneautoParam.stIrParam.u16GammaTable[s32Offset + i] = MAEWeight[i];
    }
    s32Offset += s32Temp;

    snprintf(szTempStr, sizeof(szTempStr), "IR:gamma_2");
    pszTempStr = iniparser_getstr(g_Sceneautodictionary, szTempStr);
    if (NULL == pszTempStr)
    {
        printf("IR:gamma_2 error\n");
        return HI_FAILURE;
    }
    s32Temp = Weight(pszTempStr);
    for (i = 0; i < s32Temp; i++)
    {
        g_stSceneautoParam.stIrParam.u16GammaTable[s32Offset + i] = MAEWeight[i];
    }

    /**************IR:3DnrIsoCount**************/
    s32Temp = 0;
	s32Temp = iniparser_getint(g_Sceneautodictionary, "IR:3DnrIsoCount", HI_FAILURE);
    if (HI_FAILURE == s32Temp)
    {
    	printf("IR:3DnrIsoCount failed\n");
        return HI_FAILURE;
    }
    g_stSceneautoParam.stIrParam.stIr3dnr.u323DnrIsoCount = s32Temp;
    CHECK_MALLOC_SIZE(s32Temp);
    g_stSceneautoParam.stIrParam.stIr3dnr.pu323DnrIsoThresh = (HI_U32*)malloc(s32Temp * sizeof(HI_U32));
    CHECK_NULL_PTR(g_stSceneautoParam.stIrParam.stIr3dnr.pu323DnrIsoThresh);
	g_stSceneautoParam.stIrParam.stIr3dnr.pst3dnrParam = (HI_SCENEAUTO_3DNR_S*)malloc(s32Temp * sizeof(HI_SCENEAUTO_3DNR_S));
    CHECK_NULL_PTR(g_stSceneautoParam.stIrParam.stIr3dnr.pst3dnrParam);

    /**************IR:3DnrIsoThresh**************/
    snprintf(szTempStr, sizeof(szTempStr), "IR:3DnrIsoThresh");
	pszTempStr = iniparser_getstr(g_Sceneautodictionary, szTempStr);
    if (NULL == pszTempStr)
    {
        printf("IR:3DnrIsoThresh failed\n");
        return HI_FAILURE;
    }
    Weight(pszTempStr);
    for (i = 0; i < g_stSceneautoParam.stIrParam.stIr3dnr.u323DnrIsoCount; i++)
    {
        g_stSceneautoParam.stIrParam.stIr3dnr.pu323DnrIsoThresh[i] = MAEWeight[i];
    }

    for (i = 0; i < g_stSceneautoParam.stIrParam.stIr3dnr.u323DnrIsoCount; i++)
	{
		/**************IR:3DnrParam**************/
		snprintf(szTempStr, sizeof(szTempStr), "IR:3DnrParam_%d", i);
		pszTempStr = iniparser_getstr(g_Sceneautodictionary, szTempStr);
		if (NULL == pszTempStr)
		{
		    printf("IR::3DnrParam_%d failed\n", i);
		    return HI_FAILURE;
		}

		s32Temp = Scanf3dnrParam(pszTempStr, (g_stSceneautoParam.stIrParam.stIr3dnr.pst3dnrParam + i));
		if (HI_FAILURE == s32Temp)
	    {
	        printf("Scanf3dnrParam failed\n");
	        return HI_FAILURE;
	    }
	}

	return HI_SUCCESS;
}

HI_S32 Sceneauto_Load3dnr(HI_S32 s32Index)
{
	HI_S32 s32Temp;
    HI_S32 i;
    HI_CHAR szTempStr[64] = {0};
    HI_CHAR* pszTempStr = HI_NULL;

	/**************3DNR:3DnrIsoCount**************/
    s32Temp = 0;
    snprintf(szTempStr, sizeof(szTempStr), "VPSS_PARAM_%d:3DnrIsoCount", s32Index);
    s32Temp = iniparser_getint(g_Sceneautodictionary, szTempStr, HI_FAILURE);
    if (HI_FAILURE == s32Temp)
    {
        printf("VPSS_PARAM_%d:3DnrIsoCount failed\n", s32Index);
        return HI_FAILURE;
    }
    g_stSceneautoParam.pstVpssParam[s32Index].st3dnrParam.u323DnrIsoCount = s32Temp;

    CHECK_MALLOC_SIZE(s32Temp);
    g_stSceneautoParam.pstVpssParam[s32Index].st3dnrParam.pu323DnrIsoThresh = (HI_U32*)malloc(s32Temp * sizeof(HI_U32));
    CHECK_NULL_PTR(g_stSceneautoParam.pstVpssParam[s32Index].st3dnrParam.pu323DnrIsoThresh);
    g_stSceneautoParam.pstVpssParam[s32Index].st3dnrParam.pst3dnrParam = (HI_SCENEAUTO_3DNR_S*)malloc(s32Temp * sizeof(HI_SCENEAUTO_3DNR_S));
    CHECK_NULL_PTR(g_stSceneautoParam.pstVpssParam[s32Index].st3dnrParam.pst3dnrParam);


    /**************3DNR:3DnrIsoThresh**************/
    snprintf(szTempStr, sizeof(szTempStr), "VPSS_PARAM_%d:3DnrIsoThresh", s32Index);
	pszTempStr = iniparser_getstr(g_Sceneautodictionary, szTempStr);
    if (NULL == pszTempStr)
    {
        printf("VPSS_PARAM_%d:3DnrIsoThresh failed\n\n", s32Index);
        return HI_FAILURE;
    }
    Weight(pszTempStr);
    for (i = 0; i < g_stSceneautoParam.pstVpssParam[s32Index].st3dnrParam.u323DnrIsoCount; i++)
    {
        g_stSceneautoParam.pstVpssParam[s32Index].st3dnrParam.pu323DnrIsoThresh[i] = MAEWeight[i];
    }

	for (i = 0; i < g_stSceneautoParam.pstVpssParam[s32Index].st3dnrParam.u323DnrIsoCount; i++)
	{
		/**************3DNR:3DnrParam**************/
		snprintf(szTempStr, sizeof(szTempStr), "VPSS_PARAM_%d:3DnrParam_%d", s32Index, i);
		pszTempStr = iniparser_getstr(g_Sceneautodictionary, szTempStr);
		if (NULL == pszTempStr)
		{
		    printf("VPSS_PARAM_%d:3DnrParam_%d failed\n", s32Index, i);
		    return HI_FAILURE;
		}

		s32Temp = Scanf3dnrParam(pszTempStr, (g_stSceneautoParam.pstVpssParam[s32Index].st3dnrParam.pst3dnrParam + i));
		if (HI_FAILURE == s32Temp)
	    {
	        printf("Scanf3dnrParam failed\n");
	        return HI_FAILURE;
	    }
	}

	return HI_SUCCESS;
}

HI_S32 Sceneauto_Load3dnrX(HI_S32 s32Index)
{
	HI_S32 s32Temp;
    HI_S32 i;
    HI_CHAR szTempStr[64] = {0};
    HI_CHAR* pszTempStr = HI_NULL;

	/**************3DNR:3DnrIsoCount**************/
    s32Temp = 0;
    snprintf(szTempStr, sizeof(szTempStr), "VPSS_PARAM_%d:3DnrIsoCount", s32Index);
    s32Temp = iniparser_getint(g_Sceneautodictionary, szTempStr, HI_FAILURE);
    if (HI_FAILURE == s32Temp)
    {
        printf("VPSS_PARAM_%d:3DnrIsoCount failed\n", s32Index);
        return HI_FAILURE;
    }
    g_stSceneautoParam.pstVpssParam[s32Index].st3dnrParamX.u323DnrIsoCount = s32Temp;

    CHECK_MALLOC_SIZE(s32Temp);
    g_stSceneautoParam.pstVpssParam[s32Index].st3dnrParamX.pu323DnrIsoThresh = (HI_U32*)malloc(s32Temp * sizeof(HI_U32));
    CHECK_NULL_PTR(g_stSceneautoParam.pstVpssParam[s32Index].st3dnrParamX.pu323DnrIsoThresh);
    g_stSceneautoParam.pstVpssParam[s32Index].st3dnrParamX.pst3dnrxParam = (HI_SCENEAUTO_3DNR_X_S*)malloc(s32Temp * sizeof(HI_SCENEAUTO_3DNR_X_S));
    CHECK_NULL_PTR(g_stSceneautoParam.pstVpssParam[s32Index].st3dnrParamX.pst3dnrxParam);

    /**************3DNR:3DnrIsoThresh**************/
    snprintf(szTempStr, sizeof(szTempStr), "VPSS_PARAM_%d:3DnrIsoThresh", s32Index);
	pszTempStr = iniparser_getstr(g_Sceneautodictionary, szTempStr);
    if (NULL == pszTempStr)
    {
        printf("VPSS_PARAM_%d:3DnrIsoThresh failed\n\n", s32Index);
        return HI_FAILURE;
    }
    Weight(pszTempStr);
    for (i = 0; i < g_stSceneautoParam.pstVpssParam[s32Index].st3dnrParamX.u323DnrIsoCount; i++)
    {
        g_stSceneautoParam.pstVpssParam[s32Index].st3dnrParamX.pu323DnrIsoThresh[i] = MAEWeight[i];
    }

	for (i = 0; i < g_stSceneautoParam.pstVpssParam[s32Index].st3dnrParamX.u323DnrIsoCount; i++)
	{
		/**************3DNR:3DnrParam**************/
		snprintf(szTempStr, sizeof(szTempStr), "VPSS_PARAM_%d:3DnrParam_%d", s32Index, i);
		pszTempStr = iniparser_getstr(g_Sceneautodictionary, szTempStr);
		if (NULL == pszTempStr)
		{
		    printf("VPSS_PARAM_%d:3DnrParam_%d failed\n", s32Index, i);
		    return HI_FAILURE;
		}

        memset((g_stSceneautoParam.pstVpssParam[s32Index].st3dnrParamX.pst3dnrxParam + i), 0, sizeof(HI_SCENEAUTO_3DNR_X_S));
		s32Temp = Scanf3dnrxParam(pszTempStr, (g_stSceneautoParam.pstVpssParam[s32Index].st3dnrParamX.pst3dnrxParam + i));
		if (HI_FAILURE == s32Temp)
	    {
	        printf("Scanf3dnrParamX failed\n");
	        return HI_FAILURE;
	    }
	}
	return HI_SUCCESS;
}

HI_S32 Sceneauto_LoadVpssParam()
{
	HI_S32 s32Ret = HI_SUCCESS;
	HI_S32 s32Temp = 0;
	HI_S32 s32Index;
	HI_CHAR szTempStr[64] = {0};

    CHECK_MALLOC_SIZE((g_stSceneautoParam.stCommParam.s32VpssGrpCount) * sizeof(HI_SCENEAUTO_VPSSPARAM_S));
	g_stSceneautoParam.pstVpssParam = (HI_SCENEAUTO_VPSSPARAM_S*)malloc((g_stSceneautoParam.stCommParam.s32VpssGrpCount) * sizeof(HI_SCENEAUTO_VPSSPARAM_S));
	CHECK_NULL_PTR(g_stSceneautoParam.pstVpssParam);

	for (s32Index = 0; s32Index < g_stSceneautoParam.stCommParam.s32VpssGrpCount; s32Index++)
	{
		snprintf(szTempStr, sizeof(szTempStr), "VPSS_PARAM_%d:VPSS_GRP", s32Index);
		s32Temp = 0;
        s32Temp = iniparser_getint(g_Sceneautodictionary, szTempStr, HI_FAILURE);
        if (HI_FAILURE == s32Temp)
        {
            printf("VPSS_PARAM_%d:VPSS_GRP failed\n", s32Index);
            return HI_FAILURE;
        }
        g_stSceneautoParam.pstVpssParam[s32Index].VpssGrp = s32Temp;

		snprintf(szTempStr, sizeof(szTempStr), "VPSS_PARAM_%d:3DnrSel", s32Index);
		s32Temp = 0;
        s32Temp = iniparser_getint(g_Sceneautodictionary, szTempStr, HI_FAILURE);
        if (HI_FAILURE == s32Temp)
        {
            printf("VPSS_PARAM_%d:3DnrSel failed\n", s32Index);
            return HI_FAILURE;
        }
        g_stSceneautoParam.pstVpssParam[s32Index].u32NrSel = s32Temp;

        if (0 == g_stSceneautoParam.pstVpssParam[s32Index].u32NrSel)
        {
    		s32Ret = Sceneauto_Load3dnr(s32Index);
    		if (HI_SUCCESS != s32Ret)
    		{
         		printf("(s32Index: %d) Sceneauto_Load3dnr failed\n", s32Index);
                return HI_FAILURE;
    		}
        }
        else if (1 == g_stSceneautoParam.pstVpssParam[s32Index].u32NrSel)
        {
    		s32Ret = Sceneauto_Load3dnrX(s32Index);
    		if (HI_SUCCESS != s32Ret)
    		{
         		printf("(s32Index: %d) Sceneauto_Load3dnrX failed\n", s32Index);
                return HI_FAILURE;
    		}
		}
	}

	return HI_SUCCESS;
}

HI_S32 Sceneauto_LoadDCI(HI_S32 s32Index)
{
	HI_S32 s32Temp;
    HI_CHAR szTempStr[64] = {0};

    /**************DCI:DCIEnable**************/
    s32Temp = 0;
    snprintf(szTempStr, sizeof(szTempStr), "VI_PARAM_%d:DCIEnable", s32Index);
    s32Temp = iniparser_getint(g_Sceneautodictionary, szTempStr, HI_FAILURE);
    if (HI_FAILURE == s32Temp)
    {
        printf("VI_PARAM_%d:DCIEnable failed\n", s32Index);
        return HI_FAILURE;
    }
    g_stSceneautoParam.pstViParam[s32Index].stDciParam.bDCIEnable = (HI_BOOL)s32Temp;

	/**************DCI:DCIBlackGain**************/
    s32Temp = 0;
    snprintf(szTempStr, sizeof(szTempStr), "VI_PARAM_%d:DCIBlackGain", s32Index);
    s32Temp = iniparser_getint(g_Sceneautodictionary, szTempStr, HI_FAILURE);
    if (HI_FAILURE == s32Temp)
    {
        printf("VI_PARAM_%d:DCIBlackGain failed\n", s32Index);
        return HI_FAILURE;
    }
    g_stSceneautoParam.pstViParam[s32Index].stDciParam.u32DCIBlackGain = s32Temp;

    /**************DCI:DCIContrastGain**************/
    s32Temp = 0;
    snprintf(szTempStr, sizeof(szTempStr), "VI_PARAM_%d:DCIContrastGain", s32Index);
    s32Temp = iniparser_getint(g_Sceneautodictionary, szTempStr, HI_FAILURE);
    if (HI_FAILURE == s32Temp)
    {
        printf("VI_PARAM_%d:DCIContrastGain failed\n", s32Index);
        return HI_FAILURE;
    }
    g_stSceneautoParam.pstViParam[s32Index].stDciParam.u32DCIContrastGain = s32Temp;

    /**************DCI:DCILightGain**************/
    s32Temp = 0;
    snprintf(szTempStr, sizeof(szTempStr), "VI_PARAM_%d:DCILightGain", s32Index);
    s32Temp = iniparser_getint(g_Sceneautodictionary, szTempStr, HI_FAILURE);
    if (HI_FAILURE == s32Temp)
    {
        printf("VI_PARAM_%d:DCILightGain failed\n", s32Index);
        return HI_FAILURE;
    }
    g_stSceneautoParam.pstViParam[s32Index].stDciParam.u32DCILightGain = s32Temp;


	return HI_SUCCESS;
}

HI_S32 Sceneauto_LoadViParam()
{
	HI_S32 s32Ret = HI_SUCCESS;
	HI_S32 s32Temp = 0;
	HI_S32 s32Index;
	HI_CHAR szTempStr[64] = {0};

    CHECK_MALLOC_SIZE((g_stSceneautoParam.stCommParam.s32ViDevCount) * sizeof(HI_SCENEAUTO_VIPARAM_S));
	g_stSceneautoParam.pstViParam = (HI_SCENEAUTO_VIPARAM_S*)malloc((g_stSceneautoParam.stCommParam.s32ViDevCount) * sizeof(HI_SCENEAUTO_VIPARAM_S));
	CHECK_NULL_PTR(g_stSceneautoParam.pstViParam);

	for (s32Index = 0; s32Index < g_stSceneautoParam.stCommParam.s32ViDevCount; s32Index++)
	{
		snprintf(szTempStr, sizeof(szTempStr), "VI_PARAM_%d:VI_DEV", s32Index);
		s32Temp = 0;
        s32Temp = iniparser_getint(g_Sceneautodictionary, szTempStr, HI_FAILURE);
        if (HI_FAILURE == s32Temp)
        {
            printf("VI_PARAM_%d:VI_DEV failed\n", s32Index);
            return HI_FAILURE;
        }
       	g_stSceneautoParam.pstViParam[s32Index].ViDev = s32Temp;

		s32Ret = Sceneauto_LoadDCI(s32Index);
		if (HI_SUCCESS != s32Ret)
		{
     		printf("(s32Index: %d) Sceneauto_LoadDCI failed\n", s32Index);
            return HI_FAILURE;
		}
	}

	return HI_SUCCESS;
}

HI_S32 Sceneauto_LoadGamma(HI_S32 s32Index)
{
	HI_S32 s32Temp;
    HI_S32 s32Offset;
    HI_S32 i, j;
    HI_CHAR szTempStr[64] = {0};
    HI_CHAR* pszTempStr = HI_NULL;

    /**************GAMMA:gammaDelayCount**************/
    s32Temp = 0;
    snprintf(szTempStr, sizeof(szTempStr), "ISP_PARAM_%d:gammaDelayCount", s32Index);
    s32Temp = iniparser_getint(g_Sceneautodictionary, szTempStr, HI_FAILURE);
    if (HI_FAILURE == s32Temp)
    {
        printf("ISP_PARAM_%d:gammaDelayCount failed\n", s32Index);
        return HI_FAILURE;
    }
    g_stSceneautoParam.pstIspParam[s32Index].stGammaParam.u32DelayCount = s32Temp;

    /**************GAMMA:gammaInterval**************/
    s32Temp = 0;
    snprintf(szTempStr, sizeof(szTempStr), "ISP_PARAM_%d:gammaInterval", s32Index);
    s32Temp = iniparser_getint(g_Sceneautodictionary, szTempStr, HI_FAILURE);
    if (HI_FAILURE == s32Temp)
    {
        printf("ISP_PARAM_%d:gammaInterval failed\n", s32Index);
        return HI_FAILURE;
    }
    g_stSceneautoParam.pstIspParam[s32Index].stGammaParam.u32Interval = s32Temp;

    /**************GAMMA:gammaExpCount**************/
    s32Temp = 0;
    snprintf(szTempStr, sizeof(szTempStr), "ISP_PARAM_%d:gammaExpCount", s32Index);
    s32Temp = iniparser_getint(g_Sceneautodictionary, szTempStr, HI_FAILURE);
    if (HI_FAILURE == s32Temp)
    {
        printf("ISP_PARAM_%d:gammaExpCount failed\n", s32Index);
        return HI_FAILURE;
    }
    g_stSceneautoParam.pstIspParam[s32Index].stGammaParam.u32ExpCount = s32Temp;

    CHECK_MALLOC_SIZE(s32Temp);
	g_stSceneautoParam.pstIspParam[s32Index].stGammaParam.pu32ExpThreshLtoH = (HI_U32*)malloc(s32Temp * sizeof(HI_U32));
	CHECK_NULL_PTR(g_stSceneautoParam.pstIspParam[s32Index].stGammaParam.pu32ExpThreshLtoH);
	g_stSceneautoParam.pstIspParam[s32Index].stGammaParam.pu32ExpThreshHtoL= (HI_U32*)malloc(s32Temp * sizeof(HI_U32));
	CHECK_NULL_PTR(g_stSceneautoParam.pstIspParam[s32Index].stGammaParam.pu32ExpThreshHtoL);
	g_stSceneautoParam.pstIspParam[s32Index].stGammaParam.pstGamma = (HI_SCENEAUTO_GAMMA_S*)malloc(s32Temp * sizeof(HI_SCENEAUTO_GAMMA_S));
	CHECK_NULL_PTR(g_stSceneautoParam.pstIspParam[s32Index].stGammaParam.pstGamma);

	/**************GAMMA:gammaExpThreshLtoH**************/
    snprintf(szTempStr, sizeof(szTempStr), "ISP_PARAM_%d:gammaExpThreshLtoH", s32Index);
	pszTempStr = iniparser_getstr(g_Sceneautodictionary, szTempStr);
    if (NULL == pszTempStr)
    {
        printf("ISP_PARAM_%d:gammaExpThreshLtoH failed\n", s32Index);
        return HI_FAILURE;
    }
    Weight(pszTempStr);
    for (i = 0; i < g_stSceneautoParam.pstIspParam[s32Index].stGammaParam.u32ExpCount; i++)
    {
    	g_stSceneautoParam.pstIspParam[s32Index].stGammaParam.pu32ExpThreshLtoH[i] = MAEWeight[i];
    }

    /**************GAMMA:gammaExpThreshHtoL**************/
    snprintf(szTempStr, sizeof(szTempStr), "ISP_PARAM_%d:gammaExpThreshHtoL", s32Index);
	pszTempStr = iniparser_getstr(g_Sceneautodictionary, szTempStr);
    if (NULL == pszTempStr)
    {
        printf("ISP_PARAM_%d:gammaExpThreshHtoL failed\n", s32Index);
        return HI_FAILURE;
    }
    Weight(pszTempStr);
    for (i = 0; i < g_stSceneautoParam.pstIspParam[s32Index].stGammaParam.u32ExpCount; i++)
    {
    	g_stSceneautoParam.pstIspParam[s32Index].stGammaParam.pu32ExpThreshHtoL[i] = MAEWeight[i];
    }

    /**************GAMMA:gammatable**************/
    for (i = 0; i < g_stSceneautoParam.pstIspParam[s32Index].stGammaParam.u32ExpCount; i++)
    {
        s32Offset = 0;

        snprintf(szTempStr, sizeof(szTempStr), "ISP_PARAM_%d:gamma.0_%d", s32Index, i);
        pszTempStr = iniparser_getstr(g_Sceneautodictionary, szTempStr);
        if (NULL == pszTempStr)
        {
            printf("ISP_PARAM_%d:gamma.0_%d error\n", s32Index, i);
            return HI_FAILURE;
        }
        s32Temp = Weight(pszTempStr);
        for (j = 0; j < s32Temp; j++)
        {
        	g_stSceneautoParam.pstIspParam[s32Index].stGammaParam.pstGamma[i].u16Table[j] = MAEWeight[j];
        }
        s32Offset += s32Temp;

        snprintf(szTempStr, sizeof(szTempStr), "ISP_PARAM_%d:gamma.1_%d", s32Index, i);
        pszTempStr = iniparser_getstr(g_Sceneautodictionary, szTempStr);
        if (NULL == pszTempStr)
        {
            printf("ISP_PARAM_%d:gamma.1_%d error\n", s32Index, i);
            return HI_FAILURE;
        }
        s32Temp = Weight(pszTempStr);
        for (j = 0; j < s32Temp; j++)
        {
            g_stSceneautoParam.pstIspParam[s32Index].stGammaParam.pstGamma[i].u16Table[s32Offset + j] = MAEWeight[j];
        }
        s32Offset += s32Temp;

        snprintf(szTempStr, sizeof(szTempStr), "ISP_PARAM_%d:gamma.2_%d", s32Index, i);
        pszTempStr = iniparser_getstr(g_Sceneautodictionary, szTempStr);
        if (NULL == pszTempStr)
        {
            printf("ISP_PARAM_%d:gamma.2_%d error\n", s32Index, i);
            return HI_FAILURE;
        }
        s32Temp = Weight(pszTempStr);
        for (j = 0; j < s32Temp; j++)
        {
            g_stSceneautoParam.pstIspParam[s32Index].stGammaParam.pstGamma[i].u16Table[s32Offset + j] = MAEWeight[j];
        }

        g_stSceneautoParam.pstIspParam[s32Index].stGammaParam.pstGamma[i].u8CurveType = 2;
    }

	return HI_SUCCESS;
}

HI_S32 Sceneauto_LoadFalseColor(HI_S32 s32Index)
{
	HI_S32 s32Temp;
    HI_S32 i;
    HI_CHAR szTempStr[64] = {0};
    HI_CHAR* pszTempStr = HI_NULL;

	/**************FALSECOLOR:falsecolorExpCount**************/
	s32Temp = 0;
    snprintf(szTempStr, sizeof(szTempStr), "ISP_PARAM_%d:falsecolorExpCount", s32Index);
    s32Temp = iniparser_getint(g_Sceneautodictionary, szTempStr, HI_FAILURE);
    if (HI_FAILURE == s32Temp)
    {
        printf("ISP_PARAM_%d:falsecolorExpCount failed\n", s32Index);
        return HI_FAILURE;
    }
    g_stSceneautoParam.pstIspParam[s32Index].stFalseColorParam.u32ExpCount = s32Temp;

    CHECK_MALLOC_SIZE(s32Temp);
    g_stSceneautoParam.pstIspParam[s32Index].stFalseColorParam.pu32ExpThresh = (HI_U32*)malloc(s32Temp * sizeof(HI_U32));
    CHECK_NULL_PTR(g_stSceneautoParam.pstIspParam[s32Index].stFalseColorParam.pu32ExpThresh);
    g_stSceneautoParam.pstIspParam[s32Index].stFalseColorParam.pstFalseColor = (HI_SCENEAUTO_FALSECOLOR_S*)malloc(s32Temp * sizeof(HI_SCENEAUTO_FALSECOLOR_S));
    CHECK_NULL_PTR(g_stSceneautoParam.pstIspParam[s32Index].stFalseColorParam.pstFalseColor);

	/**************FALSECOLOR:falsecolorExpThresh**************/
    snprintf(szTempStr, sizeof(szTempStr), "ISP_PARAM_%d:falsecolorExpThresh", s32Index);
	pszTempStr = iniparser_getstr(g_Sceneautodictionary, szTempStr);
    if (NULL == pszTempStr)
    {
        printf("ISP_PARAM_%d:falsecolorExpThresh failed\n", s32Index);
        return HI_FAILURE;
    }
    Weight(pszTempStr);
    for (i = 0; i < g_stSceneautoParam.pstIspParam[s32Index].stFalseColorParam.u32ExpCount; i++)
    {
    	g_stSceneautoParam.pstIspParam[s32Index].stFalseColorParam.pu32ExpThresh[i] = MAEWeight[i];
    }

    /**************FALSECOLOR:u8Strength**************/
    snprintf(szTempStr, sizeof(szTempStr), "ISP_PARAM_%d:u8Strength", s32Index);
	pszTempStr = iniparser_getstr(g_Sceneautodictionary, szTempStr);
    if (NULL == pszTempStr)
    {
        printf("ISP_PARAM_%d:u8Strength failed\n", s32Index);
        return HI_FAILURE;
    }
    Weight(pszTempStr);
    for (i = 0; i < g_stSceneautoParam.pstIspParam[s32Index].stFalseColorParam.u32ExpCount; i++)
    {
    	g_stSceneautoParam.pstIspParam[s32Index].stFalseColorParam.pstFalseColor[i].u8Strength = MAEWeight[i];
    }

	return HI_SUCCESS;
}

HI_S32 Sceneauto_LoadDp(HI_S32 s32Index)
{
	HI_S32 s32Temp;
    HI_S32 i;
    HI_CHAR szTempStr[64] = {0};
    HI_CHAR* pszTempStr = HI_NULL;

	/**************DP:dpExpCount**************/
	s32Temp = 0;
    snprintf(szTempStr, sizeof(szTempStr), "ISP_PARAM_%d:dpExpCount", s32Index);
    s32Temp = iniparser_getint(g_Sceneautodictionary, szTempStr, HI_FAILURE);
    if (HI_FAILURE == s32Temp)
    {
        printf("ISP_PARAM_%d:dpExpCount failed\n", s32Index);
        return HI_FAILURE;
    }
    g_stSceneautoParam.pstIspParam[s32Index].stDpParam.u32ExpCount = s32Temp;

    CHECK_MALLOC_SIZE(s32Temp);
    g_stSceneautoParam.pstIspParam[s32Index].stDpParam.pu32ExpThresh = (HI_U32*)malloc(s32Temp * sizeof(HI_U32));
    CHECK_NULL_PTR(g_stSceneautoParam.pstIspParam[s32Index].stDpParam.pu32ExpThresh);
    g_stSceneautoParam.pstIspParam[s32Index].stDpParam.pstDPAttr = (HI_SCENEAUTO_DPATTR_S*)malloc(s32Temp * sizeof(HI_SCENEAUTO_DPATTR_S));
    CHECK_NULL_PTR(g_stSceneautoParam.pstIspParam[s32Index].stDpParam.pstDPAttr);

    /**************DP:dpExpThresh**************/
    snprintf(szTempStr, sizeof(szTempStr), "ISP_PARAM_%d:dpExpThresh", s32Index);
	pszTempStr = iniparser_getstr(g_Sceneautodictionary, szTempStr);
    if (NULL == pszTempStr)
    {
        printf("ISP_PARAM_%d:dpExpThresh failed\n", s32Index);
        return HI_FAILURE;
    }
    Weight(pszTempStr);
    for (i = 0; i < g_stSceneautoParam.pstIspParam[s32Index].stDpParam.u32ExpCount; i++)
    {
    	g_stSceneautoParam.pstIspParam[s32Index].stDpParam.pu32ExpThresh[i] = MAEWeight[i];
    }

    /**************DP:u16Slop**************/
    snprintf(szTempStr, sizeof(szTempStr), "ISP_PARAM_%d:u16Slop", s32Index);
	pszTempStr = iniparser_getstr(g_Sceneautodictionary, szTempStr);
    if (NULL == pszTempStr)
    {
        printf("ISP_PARAM_%d:u16Slop failed\n", s32Index);
        return HI_FAILURE;
    }
    Weight(pszTempStr);
    for (i = 0; i < g_stSceneautoParam.pstIspParam[s32Index].stDpParam.u32ExpCount; i++)
    {
    	g_stSceneautoParam.pstIspParam[s32Index].stDpParam.pstDPAttr[i].u16Slope = MAEWeight[i];
    }

    /**************DP:u16Thresh**************/
    snprintf(szTempStr, sizeof(szTempStr), "ISP_PARAM_%d:u16Thresh", s32Index);
	pszTempStr = iniparser_getstr(g_Sceneautodictionary, szTempStr);
    if (NULL == pszTempStr)
    {
        printf("ISP_PARAM_%d:u16Thresh failed\n", s32Index);
        return HI_FAILURE;
    }
    Weight(pszTempStr);
    for (i = 0; i < g_stSceneautoParam.pstIspParam[s32Index].stDpParam.u32ExpCount; i++)
    {
    	g_stSceneautoParam.pstIspParam[s32Index].stDpParam.pstDPAttr[i].u16Thresh = MAEWeight[i];
    }

	return HI_SUCCESS;
}

HI_S32 Sceneauto_LoadSharpen(HI_S32 s32Index)
{
	HI_S32 s32Temp;
    HI_S32 i;
    HI_CHAR szTempStr[64] = {0};
    HI_CHAR* pszTempStr = HI_NULL;

	/**************SHARPEN:defSharpenD**************/
    snprintf(szTempStr, sizeof(szTempStr), "ISP_PARAM_%d:defSharpenD", s32Index);
	pszTempStr = iniparser_getstr(g_Sceneautodictionary, szTempStr);
    if (NULL == pszTempStr)
    {
        printf("ISP_PARAM_%d:defSharpenD failed\n", s32Index);
        return HI_FAILURE;
    }
    Weight(pszTempStr);
    for (i = 0; i < 16; i++)
    {
    	g_stSceneautoParam.pstIspParam[s32Index].stSharpenParam.astDefaultSharpen[i].u8SharpenD = MAEWeight[i];
    }

    /**************SHARPEN:defSharpenUd**************/
    snprintf(szTempStr, sizeof(szTempStr), "ISP_PARAM_%d:defSharpenUd", s32Index);
	pszTempStr = iniparser_getstr(g_Sceneautodictionary, szTempStr);
    if (NULL == pszTempStr)
    {
        printf("ISP_PARAM_%d:defSharpenUd failed\n", s32Index);
        return HI_FAILURE;
    }
    Weight(pszTempStr);
    for (i = 0; i < 16; i++)
    {
    	g_stSceneautoParam.pstIspParam[s32Index].stSharpenParam.astDefaultSharpen[i].u8SharpenUd = MAEWeight[i];
    }

    /**************SHARPEN:defTextureSt**************/
    snprintf(szTempStr, sizeof(szTempStr), "ISP_PARAM_%d:defTextureSt", s32Index);
	pszTempStr = iniparser_getstr(g_Sceneautodictionary, szTempStr);
    if (NULL == pszTempStr)
    {
        printf("ISP_PARAM_%d:defTextureSt failed\n", s32Index);
        return HI_FAILURE;
    }
    Weight(pszTempStr);
    for (i = 0; i < 16; i++)
    {
    	g_stSceneautoParam.pstIspParam[s32Index].stSharpenParam.astDefaultSharpen[i].u8TextureSt = MAEWeight[i];
    }

    /**************SHARPEN:defEdgeSt**************/
    snprintf(szTempStr, sizeof(szTempStr), "ISP_PARAM_%d:defEdgeSt", s32Index);
	pszTempStr = iniparser_getstr(g_Sceneautodictionary, szTempStr);
    if (NULL == pszTempStr)
    {
        printf("ISP_PARAM_%d:defEdgeSt failed\n", s32Index);
        return HI_FAILURE;
    }
    Weight(pszTempStr);
    for (i = 0; i < 16; i++)
    {
    	g_stSceneautoParam.pstIspParam[s32Index].stSharpenParam.astDefaultSharpen[i].u8EdgeSt = MAEWeight[i];
    }

    /**************SHARPEN:defOverShoot**************/
    snprintf(szTempStr, sizeof(szTempStr), "ISP_PARAM_%d:defOverShoot", s32Index);
	pszTempStr = iniparser_getstr(g_Sceneautodictionary, szTempStr);
    if (NULL == pszTempStr)
    {
        printf("ISP_PARAM_%d:defOverShoot failed\n", s32Index);
        return HI_FAILURE;
    }
    Weight(pszTempStr);
    for (i = 0; i < 16; i++)
    {
    	g_stSceneautoParam.pstIspParam[s32Index].stSharpenParam.astDefaultSharpen[i].u8OverShoot = MAEWeight[i];
    }

    /**************SHARPEN:defUnderShoot**************/
    snprintf(szTempStr, sizeof(szTempStr), "ISP_PARAM_%d:defUnderShoot", s32Index);
	pszTempStr = iniparser_getstr(g_Sceneautodictionary, szTempStr);
    if (NULL == pszTempStr)
    {
        printf("ISP_PARAM_%d:defUnderShoot failed\n", s32Index);
        return HI_FAILURE;
    }
    Weight(pszTempStr);
    for (i = 0; i < 16; i++)
    {
    	g_stSceneautoParam.pstIspParam[s32Index].stSharpenParam.astDefaultSharpen[i].u8UnderShoot = MAEWeight[i];
    }

    /**************SHARPEN:defDetailCtrl**************/
    snprintf(szTempStr, sizeof(szTempStr), "ISP_PARAM_%d:defDetailCtrl", s32Index);
	pszTempStr = iniparser_getstr(g_Sceneautodictionary, szTempStr);
    if (NULL == pszTempStr)
    {
        printf("ISP_PARAM_%d:defDetailCtrl failed\n", s32Index);
        return HI_FAILURE;
    }
    Weight(pszTempStr);
    for (i = 0; i < 16; i++)
    {
    	g_stSceneautoParam.pstIspParam[s32Index].stSharpenParam.astDefaultSharpen[i].u8DetailCtrl = MAEWeight[i];
    }

	/**************SHARPEN:sharpenExpCount**************/
	s32Temp = 0;
    snprintf(szTempStr, sizeof(szTempStr), "ISP_PARAM_%d:sharpenExpCount", s32Index);
    s32Temp = iniparser_getint(g_Sceneautodictionary, szTempStr, HI_FAILURE);
    if (HI_FAILURE == s32Temp)
    {
        printf("ISP_PARAM_%d:sharpenExpCount failed\n", s32Index);
        return HI_FAILURE;
    }
    g_stSceneautoParam.pstIspParam[s32Index].stSharpenParam.u32ExpCount = s32Temp;

    CHECK_MALLOC_SIZE(s32Temp);
    g_stSceneautoParam.pstIspParam[s32Index].stSharpenParam.pu32ExpThresh = (HI_U32*)malloc(s32Temp * sizeof(HI_U32));
    CHECK_NULL_PTR(g_stSceneautoParam.pstIspParam[s32Index].stSharpenParam.pu32ExpThresh);
    g_stSceneautoParam.pstIspParam[s32Index].stSharpenParam.pstDynamicSharpen = (HI_SCENEAUTO_SHARPEN_S*)malloc(s32Temp * sizeof(HI_SCENEAUTO_SHARPEN_S));
    CHECK_NULL_PTR(g_stSceneautoParam.pstIspParam[s32Index].stSharpenParam.pstDynamicSharpen);

	/**************SHARPEN:sharpenExpThresh**************/
    snprintf(szTempStr, sizeof(szTempStr), "ISP_PARAM_%d:sharpenExpThresh", s32Index);
	pszTempStr = iniparser_getstr(g_Sceneautodictionary, szTempStr);
    if (NULL == pszTempStr)
    {
        printf("ISP_PARAM_%d:sharpenExpThresh failed\n", s32Index);
        return HI_FAILURE;
    }
    Weight(pszTempStr);
    for (i = 0; i < g_stSceneautoParam.pstIspParam[s32Index].stSharpenParam.u32ExpCount; i++)
    {
    	g_stSceneautoParam.pstIspParam[s32Index].stSharpenParam.pu32ExpThresh[i] = MAEWeight[i];
    }

    /**************SHARPEN:dynSharpenD**************/
    snprintf(szTempStr, sizeof(szTempStr), "ISP_PARAM_%d:dynSharpenD", s32Index);
	pszTempStr = iniparser_getstr(g_Sceneautodictionary, szTempStr);
    if (NULL == pszTempStr)
    {
        printf("ISP_PARAM_%d:dynSharpenD failed\n", s32Index);
        return HI_FAILURE;
    }
    Weight(pszTempStr);
    for (i = 0; i < g_stSceneautoParam.pstIspParam[s32Index].stSharpenParam.u32ExpCount; i++)
    {
    	g_stSceneautoParam.pstIspParam[s32Index].stSharpenParam.pstDynamicSharpen[i].u8SharpenD = MAEWeight[i];
    }

    /**************SHARPEN:dynSharpenUd**************/
    snprintf(szTempStr, sizeof(szTempStr), "ISP_PARAM_%d:dynSharpenUd", s32Index);
	pszTempStr = iniparser_getstr(g_Sceneautodictionary, szTempStr);
    if (NULL == pszTempStr)
    {
        printf("ISP_PARAM_%d:dynSharpenUd failed\n", s32Index);
        return HI_FAILURE;
    }
    Weight(pszTempStr);
    for (i = 0; i < g_stSceneautoParam.pstIspParam[s32Index].stSharpenParam.u32ExpCount; i++)
    {
    	g_stSceneautoParam.pstIspParam[s32Index].stSharpenParam.pstDynamicSharpen[i].u8SharpenUd = MAEWeight[i];
    }

    /**************SHARPEN:dynTextureSt**************/
    snprintf(szTempStr, sizeof(szTempStr), "ISP_PARAM_%d:dynTextureSt", s32Index);
	pszTempStr = iniparser_getstr(g_Sceneautodictionary, szTempStr);
    if (NULL == pszTempStr)
    {
        printf("ISP_PARAM_%d:dynTextureSt failed\n", s32Index);
        return HI_FAILURE;
    }
    Weight(pszTempStr);
    for (i = 0; i < g_stSceneautoParam.pstIspParam[s32Index].stSharpenParam.u32ExpCount; i++)
    {
    	g_stSceneautoParam.pstIspParam[s32Index].stSharpenParam.pstDynamicSharpen[i].u8TextureSt = MAEWeight[i];
    }

    /**************SHARPEN:dynEdgeSt**************/
    snprintf(szTempStr, sizeof(szTempStr), "ISP_PARAM_%d:dynEdgeSt", s32Index);
	pszTempStr = iniparser_getstr(g_Sceneautodictionary, szTempStr);
    if (NULL == pszTempStr)
    {
        printf("ISP_PARAM_%d:dynEdgeSt failed\n", s32Index);
        return HI_FAILURE;
    }
    Weight(pszTempStr);
    for (i = 0; i < g_stSceneautoParam.pstIspParam[s32Index].stSharpenParam.u32ExpCount; i++)
    {
    	g_stSceneautoParam.pstIspParam[s32Index].stSharpenParam.pstDynamicSharpen[i].u8EdgeSt = MAEWeight[i];
    }

    /**************SHARPEN:dynOverShoot**************/
    snprintf(szTempStr, sizeof(szTempStr), "ISP_PARAM_%d:dynOverShoot", s32Index);
	pszTempStr = iniparser_getstr(g_Sceneautodictionary, szTempStr);
    if (NULL == pszTempStr)
    {
        printf("ISP_PARAM_%d:dynOverShoot failed\n", s32Index);
        return HI_FAILURE;
    }
    Weight(pszTempStr);
    for (i = 0; i < g_stSceneautoParam.pstIspParam[s32Index].stSharpenParam.u32ExpCount; i++)
    {
    	g_stSceneautoParam.pstIspParam[s32Index].stSharpenParam.pstDynamicSharpen[i].u8OverShoot = MAEWeight[i];
    }

    /**************SHARPEN:dynUnderShoot**************/
    snprintf(szTempStr, sizeof(szTempStr), "ISP_PARAM_%d:dynUnderShoot", s32Index);
	pszTempStr = iniparser_getstr(g_Sceneautodictionary, szTempStr);
    if (NULL == pszTempStr)
    {
        printf("ISP_PARAM_%d:dynUnderShoot failed\n", s32Index);
        return HI_FAILURE;
    }
    Weight(pszTempStr);
    for (i = 0; i < g_stSceneautoParam.pstIspParam[s32Index].stSharpenParam.u32ExpCount; i++)
    {
    	g_stSceneautoParam.pstIspParam[s32Index].stSharpenParam.pstDynamicSharpen[i].u8UnderShoot = MAEWeight[i];
    }

	/**************SHARPEN:dynDetailCtrl**************/
    snprintf(szTempStr, sizeof(szTempStr), "ISP_PARAM_%d:dynDetailCtrl", s32Index);
	pszTempStr = iniparser_getstr(g_Sceneautodictionary, szTempStr);
    if (NULL == pszTempStr)
    {
        printf("ISP_PARAM_%d:dynDetailCtrl failed\n", s32Index);
        return HI_FAILURE;
    }
    Weight(pszTempStr);
    for (i = 0; i < g_stSceneautoParam.pstIspParam[s32Index].stSharpenParam.u32ExpCount; i++)
    {
    	g_stSceneautoParam.pstIspParam[s32Index].stSharpenParam.pstDynamicSharpen[i].u8DetailCtrl = MAEWeight[i];
    }

	return HI_SUCCESS;
}

HI_S32 Sceneauto_LoadDemosaic(HI_S32 s32Index)
{
	HI_S32 s32Temp;
    HI_S32 i;
    HI_CHAR szTempStr[64] = {0};
    HI_CHAR* pszTempStr = HI_NULL;

	/**************DEMOSAIC:demosaicExpCount**************/
	s32Temp = 0;
    snprintf(szTempStr, sizeof(szTempStr), "ISP_PARAM_%d:demosaicExpCount", s32Index);
    s32Temp = iniparser_getint(g_Sceneautodictionary, szTempStr, HI_FAILURE);
    if (HI_FAILURE == s32Temp)
    {
        printf("ISP_PARAM_%d:demosaicExpCount failed\n", s32Index);
        return HI_FAILURE;
    }
    g_stSceneautoParam.pstIspParam[s32Index].stDemosaicParam.u32ExpCount = s32Temp;

    CHECK_MALLOC_SIZE(s32Temp);
    g_stSceneautoParam.pstIspParam[s32Index].stDemosaicParam.pu32ExpThresh = (HI_U32*)malloc(s32Temp * sizeof(HI_U32));
    CHECK_NULL_PTR(g_stSceneautoParam.pstIspParam[s32Index].stDemosaicParam.pu32ExpThresh);
    g_stSceneautoParam.pstIspParam[s32Index].stDemosaicParam.pstDemosaic = (HI_SCENEAUTO_DEMOSAIC_S*)malloc(s32Temp * sizeof(HI_SCENEAUTO_DEMOSAIC_S));
    CHECK_NULL_PTR(g_stSceneautoParam.pstIspParam[s32Index].stDemosaicParam.pstDemosaic);

	/**************DEMOSAIC:drThresh**************/
	snprintf(szTempStr, sizeof(szTempStr), "ISP_PARAM_%d:demosaicExpThresh", s32Index);
	pszTempStr = iniparser_getstr(g_Sceneautodictionary, szTempStr);
    if (NULL == pszTempStr)
    {
        printf("ISP_PARAM_%d:demosaicExpThresh failed\n", s32Index);
        return HI_FAILURE;
    }
    Weight(pszTempStr);
    for (i = 0; i < g_stSceneautoParam.pstIspParam[s32Index].stDemosaicParam.u32ExpCount; i++)
    {
    	g_stSceneautoParam.pstIspParam[s32Index].stDemosaicParam.pu32ExpThresh[i] = MAEWeight[i];
    }

	/**************DEMOSAIC:u8UuSlpoe**************/
	snprintf(szTempStr, sizeof(szTempStr), "ISP_PARAM_%d:u8UuSlpoe", s32Index);
	pszTempStr = iniparser_getstr(g_Sceneautodictionary, szTempStr);
    if (NULL == pszTempStr)
    {
        printf("ISP_PARAM_%d:u8UuSlpoe failed\n", s32Index);
        return HI_FAILURE;
    }
    Weight(pszTempStr);
    for (i = 0; i < g_stSceneautoParam.pstIspParam[s32Index].stDemosaicParam.u32ExpCount; i++)
    {
    	g_stSceneautoParam.pstIspParam[s32Index].stDemosaicParam.pstDemosaic[i].u8UuSlope = MAEWeight[i];
    }

    /**************DEMOSAIC:u8AaSlope**************/
	snprintf(szTempStr, sizeof(szTempStr), "ISP_PARAM_%d:u8AaSlope", s32Index);
	pszTempStr = iniparser_getstr(g_Sceneautodictionary, szTempStr);
    if (NULL == pszTempStr)
    {
        printf("ISP_PARAM_%d:u8AaSlope failed\n", s32Index);
        return HI_FAILURE;
    }
    Weight(pszTempStr);
    for (i = 0; i < g_stSceneautoParam.pstIspParam[s32Index].stDemosaicParam.u32ExpCount; i++)
    {
    	g_stSceneautoParam.pstIspParam[s32Index].stDemosaicParam.pstDemosaic[i].u8AaSlope = MAEWeight[i];
    }

    /**************DEMOSAIC:u8VaSlope**************/
	snprintf(szTempStr, sizeof(szTempStr), "ISP_PARAM_%d:u8VaSlope", s32Index);
	pszTempStr = iniparser_getstr(g_Sceneautodictionary, szTempStr);
    if (NULL == pszTempStr)
    {
        printf("ISP_PARAM_%d:u8VaSlope failed\n", s32Index);
        return HI_FAILURE;
    }
    Weight(pszTempStr);
    for (i = 0; i < g_stSceneautoParam.pstIspParam[s32Index].stDemosaicParam.u32ExpCount; i++)
    {
    	g_stSceneautoParam.pstIspParam[s32Index].stDemosaicParam.pstDemosaic[i].u8VaSlope = MAEWeight[i];
    }

    /**************DEMOSAIC:u8VhSlope**************/
	snprintf(szTempStr, sizeof(szTempStr), "ISP_PARAM_%d:u8VhSlope", s32Index);
	pszTempStr = iniparser_getstr(g_Sceneautodictionary, szTempStr);
    if (NULL == pszTempStr)
    {
        printf("ISP_PARAM_%d:u8VhSlope failed\n", s32Index);
        return HI_FAILURE;
    }
    Weight(pszTempStr);
    for (i = 0; i < g_stSceneautoParam.pstIspParam[s32Index].stDemosaicParam.u32ExpCount; i++)
    {
    	g_stSceneautoParam.pstIspParam[s32Index].stDemosaicParam.pstDemosaic[i].u8VhSlope = MAEWeight[i];
    }

	return HI_SUCCESS;
}

HI_S32 Sceneauto_LoadDRC(HI_S32 s32Index)
{
	HI_S32 s32Temp;
    HI_S32 i;
    HI_CHAR szTempStr[64] = {0};
    HI_CHAR* pszTempStr = HI_NULL;

    /**************DRC:drCount**************/
	s32Temp = 0;
    snprintf(szTempStr, sizeof(szTempStr), "ISP_PARAM_%d:drCount", s32Index);
    s32Temp = iniparser_getint(g_Sceneautodictionary, szTempStr, HI_FAILURE);
    if (HI_FAILURE == s32Temp)
    {
        printf("ISP_PARAM_%d:drCount failed\n", s32Index);
        return HI_FAILURE;
    }
    g_stSceneautoParam.pstIspParam[s32Index].stDrcParam.u32DRCount = s32Temp;

    CHECK_MALLOC_SIZE(s32Temp);
    g_stSceneautoParam.pstIspParam[s32Index].stDrcParam.pu32DRThresh = (HI_U32*)malloc(s32Temp * sizeof(HI_U32));
    CHECK_NULL_PTR(g_stSceneautoParam.pstIspParam[s32Index].stDrcParam.pu32DRThresh);
    g_stSceneautoParam.pstIspParam[s32Index].stDrcParam.pstDRCAttr = (HI_SCENEAUTO_DRC_S*)malloc(s32Temp * sizeof(HI_SCENEAUTO_DRC_S));
    CHECK_NULL_PTR(g_stSceneautoParam.pstIspParam[s32Index].stDrcParam.pstDRCAttr);

    /**************DRC:drThresh**************/
	snprintf(szTempStr, sizeof(szTempStr), "ISP_PARAM_%d:drThresh", s32Index);
	pszTempStr = iniparser_getstr(g_Sceneautodictionary, szTempStr);
    if (NULL == pszTempStr)
    {
        printf("ISP_PARAM_%d:drThresh failed\n", s32Index);
        return HI_FAILURE;
    }
    Weight(pszTempStr);
    for (i = 0; i < g_stSceneautoParam.pstIspParam[s32Index].stDrcParam.u32DRCount; i++)
    {
    	g_stSceneautoParam.pstIspParam[s32Index].stDrcParam.pu32DRThresh[i] = MAEWeight[i];
    }

	/**************DRC:bEnable**************/
	snprintf(szTempStr, sizeof(szTempStr), "ISP_PARAM_%d:bEnable", s32Index);
	pszTempStr = iniparser_getstr(g_Sceneautodictionary, szTempStr);
    if (NULL == pszTempStr)
    {
        printf("ISP_PARAM_%d:bEnable failed\n", s32Index);
        return HI_FAILURE;
    }
    Weight(pszTempStr);
    for (i = 0; i < g_stSceneautoParam.pstIspParam[s32Index].stDrcParam.u32DRCount; i++)
    {
    	g_stSceneautoParam.pstIspParam[s32Index].stDrcParam.pstDRCAttr[i].bEnable = (HI_BOOL)MAEWeight[i];
    }

    /**************DRC:bEnable**************/
	snprintf(szTempStr, sizeof(szTempStr), "ISP_PARAM_%d:u8OpType", s32Index);
	pszTempStr = iniparser_getstr(g_Sceneautodictionary, szTempStr);
    if (NULL == pszTempStr)
    {
        printf("ISP_PARAM_%d:u8OpType failed\n", s32Index);
        return HI_FAILURE;
    }
    Weight(pszTempStr);
    for (i = 0; i < g_stSceneautoParam.pstIspParam[s32Index].stDrcParam.u32DRCount; i++)
    {
    	g_stSceneautoParam.pstIspParam[s32Index].stDrcParam.pstDRCAttr[i].u8OpType = MAEWeight[i];
    }

	/**************DRC:u32Strength**************/
	snprintf(szTempStr, sizeof(szTempStr), "ISP_PARAM_%d:u32Strength", s32Index);
	pszTempStr = iniparser_getstr(g_Sceneautodictionary, szTempStr);
    if (NULL == pszTempStr)
    {
        printf("ISP_PARAM_%d:u32Strength failed\n", s32Index);
        return HI_FAILURE;
    }
    Weight(pszTempStr);
    for (i = 0; i < g_stSceneautoParam.pstIspParam[s32Index].stDrcParam.u32DRCount; i++)
    {
    	g_stSceneautoParam.pstIspParam[s32Index].stDrcParam.pstDRCAttr[i].u32Strength = MAEWeight[i];
    }

    /**************DRC:u32DRCAutoStrength**************/
	snprintf(szTempStr, sizeof(szTempStr), "ISP_PARAM_%d:u32DRCAutoStrength", s32Index);
	pszTempStr = iniparser_getstr(g_Sceneautodictionary, szTempStr);
    if (NULL == pszTempStr)
    {
        printf("ISP_PARAM_%d:u32DRCAutoStrength failed\n", s32Index);
        return HI_FAILURE;
    }
    Weight(pszTempStr);
    for (i = 0; i < g_stSceneautoParam.pstIspParam[s32Index].stDrcParam.u32DRCount; i++)
    {
    	g_stSceneautoParam.pstIspParam[s32Index].stDrcParam.pstDRCAttr[i].u32DRCAutoStrength = MAEWeight[i];
    }

    /**************DRC:u32WhiteLevel**************/
	snprintf(szTempStr, sizeof(szTempStr), "ISP_PARAM_%d:u32WhiteLevel", s32Index);
	pszTempStr = iniparser_getstr(g_Sceneautodictionary, szTempStr);
    if (NULL == pszTempStr)
    {
        printf("ISP_PARAM_%d:u32WhiteLevel failed\n", s32Index);
        return HI_FAILURE;
    }
    Weight(pszTempStr);
    for (i = 0; i < g_stSceneautoParam.pstIspParam[s32Index].stDrcParam.u32DRCount; i++)
    {
    	g_stSceneautoParam.pstIspParam[s32Index].stDrcParam.pstDRCAttr[i].u32WhiteLevel = MAEWeight[i];
    }

    /**************DRC:u32BlackLevel**************/
	snprintf(szTempStr, sizeof(szTempStr), "ISP_PARAM_%d:u32BlackLevel", s32Index);
	pszTempStr = iniparser_getstr(g_Sceneautodictionary, szTempStr);
    if (NULL == pszTempStr)
    {
        printf("ISP_PARAM_%d:u32BlackLevel failed\n", s32Index);
        return HI_FAILURE;
    }
    Weight(pszTempStr);
    for (i = 0; i < g_stSceneautoParam.pstIspParam[s32Index].stDrcParam.u32DRCount; i++)
    {
    	g_stSceneautoParam.pstIspParam[s32Index].stDrcParam.pstDRCAttr[i].u32BlackLevel = MAEWeight[i];
    }

    /**************DRC:u32Asymmetry**************/
	snprintf(szTempStr, sizeof(szTempStr), "ISP_PARAM_%d:u32Asymmetry", s32Index);
	pszTempStr = iniparser_getstr(g_Sceneautodictionary, szTempStr);
    if (NULL == pszTempStr)
    {
        printf("ISP_PARAM_%d:u32Asymmetry failed\n", s32Index);
        return HI_FAILURE;
    }
    Weight(pszTempStr);
    for (i = 0; i < g_stSceneautoParam.pstIspParam[s32Index].stDrcParam.u32DRCount; i++)
    {
    	g_stSceneautoParam.pstIspParam[s32Index].stDrcParam.pstDRCAttr[i].u32Asymmetry = MAEWeight[i];
    }

    /**************DRC:u32BrightEnhance**************/
	snprintf(szTempStr, sizeof(szTempStr), "ISP_PARAM_%d:u32BrightEnhance", s32Index);
	pszTempStr = iniparser_getstr(g_Sceneautodictionary, szTempStr);
    if (NULL == pszTempStr)
    {
        printf("ISP_PARAM_%d:u32BrightEnhance failed\n", s32Index);
        return HI_FAILURE;
    }
    Weight(pszTempStr);
    for (i = 0; i < g_stSceneautoParam.pstIspParam[s32Index].stDrcParam.u32DRCount; i++)
    {
    	g_stSceneautoParam.pstIspParam[s32Index].stDrcParam.pstDRCAttr[i].u32BrightEnhance = MAEWeight[i];
    }

    /**************DRC:u8FilterMux**************/
	snprintf(szTempStr, sizeof(szTempStr), "ISP_PARAM_%d:u8FilterMux", s32Index);
	pszTempStr = iniparser_getstr(g_Sceneautodictionary, szTempStr);
    if (NULL == pszTempStr)
    {
        printf("ISP_PARAM_%d:u8FilterMux failed\n", s32Index);
        return HI_FAILURE;
    }
    Weight(pszTempStr);
    for (i = 0; i < g_stSceneautoParam.pstIspParam[s32Index].stDrcParam.u32DRCount; i++)
    {
    	g_stSceneautoParam.pstIspParam[s32Index].stDrcParam.pstDRCAttr[i].u8FilterMux = MAEWeight[i];
    }

    /**************DRC:u32SlopeMax**************/
	snprintf(szTempStr, sizeof(szTempStr), "ISP_PARAM_%d:u32SlopeMax", s32Index);
	pszTempStr = iniparser_getstr(g_Sceneautodictionary, szTempStr);
    if (NULL == pszTempStr)
    {
        printf("ISP_PARAM_%d:u32SlopeMax failed\n", s32Index);
        return HI_FAILURE;
    }
    Weight(pszTempStr);
    for (i = 0; i < g_stSceneautoParam.pstIspParam[s32Index].stDrcParam.u32DRCount; i++)
    {
    	g_stSceneautoParam.pstIspParam[s32Index].stDrcParam.pstDRCAttr[i].u32SlopeMax = MAEWeight[i];
    }

    /**************DRC:u32SlopeMin**************/
	snprintf(szTempStr, sizeof(szTempStr), "ISP_PARAM_%d:u32SlopeMin", s32Index);
	pszTempStr = iniparser_getstr(g_Sceneautodictionary, szTempStr);
    if (NULL == pszTempStr)
    {
        printf("ISP_PARAM_%d:u32SlopeMin failed\n", s32Index);
        return HI_FAILURE;
    }
    Weight(pszTempStr);
    for (i = 0; i < g_stSceneautoParam.pstIspParam[s32Index].stDrcParam.u32DRCount; i++)
    {
    	g_stSceneautoParam.pstIspParam[s32Index].stDrcParam.pstDRCAttr[i].u32SlopeMin = MAEWeight[i];
    }

    /**************DRC:u32VarianceSpace**************/
	snprintf(szTempStr, sizeof(szTempStr), "ISP_PARAM_%d:u32VarianceSpace", s32Index);
	pszTempStr = iniparser_getstr(g_Sceneautodictionary, szTempStr);
    if (NULL == pszTempStr)
    {
        printf("ISP_PARAM_%d:u32VarianceSpace failed\n", s32Index);
        return HI_FAILURE;
    }
    Weight(pszTempStr);
    for (i = 0; i < g_stSceneautoParam.pstIspParam[s32Index].stDrcParam.u32DRCount; i++)
    {
    	g_stSceneautoParam.pstIspParam[s32Index].stDrcParam.pstDRCAttr[i].u32VarianceSpace = MAEWeight[i];
    }

    /**************DRC:u32VarianceIntensity**************/
	snprintf(szTempStr, sizeof(szTempStr), "ISP_PARAM_%d:u32VarianceIntensity", s32Index);
	pszTempStr = iniparser_getstr(g_Sceneautodictionary, szTempStr);
    if (NULL == pszTempStr)
    {
        printf("ISP_PARAM_%d:u32VarianceIntensity failed\n", s32Index);
        return HI_FAILURE;
    }
    Weight(pszTempStr);
    for (i = 0; i < g_stSceneautoParam.pstIspParam[s32Index].stDrcParam.u32DRCount; i++)
    {
    	g_stSceneautoParam.pstIspParam[s32Index].stDrcParam.pstDRCAttr[i].u32VarianceIntensity = MAEWeight[i];
    }

    /**************DRC:u32Contrast**************/
	snprintf(szTempStr, sizeof(szTempStr), "ISP_PARAM_%d:u32Contrast", s32Index);
	pszTempStr = iniparser_getstr(g_Sceneautodictionary, szTempStr);
    if (NULL == pszTempStr)
    {
        printf("ISP_PARAM_%d:u32Contrast failed\n", s32Index);
        return HI_FAILURE;
    }
    Weight(pszTempStr);
    for (i = 0; i < g_stSceneautoParam.pstIspParam[s32Index].stDrcParam.u32DRCount; i++)
    {
    	g_stSceneautoParam.pstIspParam[s32Index].stDrcParam.pstDRCAttr[i].u32Contrast = MAEWeight[i];
    }

    /**************DRC:u32BrightPr**************/
	snprintf(szTempStr, sizeof(szTempStr), "ISP_PARAM_%d:u32BrightPr", s32Index);
	pszTempStr = iniparser_getstr(g_Sceneautodictionary, szTempStr);
    if (NULL == pszTempStr)
    {
        printf("ISP_PARAM_%d:u32BrightPr failed\n", s32Index);
        return HI_FAILURE;
    }
    Weight(pszTempStr);
    for (i = 0; i < g_stSceneautoParam.pstIspParam[s32Index].stDrcParam.u32DRCount; i++)
    {
    	g_stSceneautoParam.pstIspParam[s32Index].stDrcParam.pstDRCAttr[i].u32BrightPr = MAEWeight[i];
    }

    /**************DRC:u32Svariance**************/
	snprintf(szTempStr, sizeof(szTempStr), "ISP_PARAM_%d:u32Svariance", s32Index);
	pszTempStr = iniparser_getstr(g_Sceneautodictionary, szTempStr);
    if (NULL == pszTempStr)
    {
        printf("ISP_PARAM_%d:u32Svariance failed\n", s32Index);
        return HI_FAILURE;
    }
    Weight(pszTempStr);
    for (i = 0; i < g_stSceneautoParam.pstIspParam[s32Index].stDrcParam.u32DRCount; i++)
    {
    	g_stSceneautoParam.pstIspParam[s32Index].stDrcParam.pstDRCAttr[i].u32Svariance = MAEWeight[i];
    }

    /**************DRC:u32DarkEnhance**************/
	snprintf(szTempStr, sizeof(szTempStr), "ISP_PARAM_%d:u32DarkEnhance", s32Index);
	pszTempStr = iniparser_getstr(g_Sceneautodictionary, szTempStr);
    if (NULL == pszTempStr)
    {
        printf("ISP_PARAM_%d:u32DarkEnhance failed\n", s32Index);
        return HI_FAILURE;
    }
    Weight(pszTempStr);
    for (i = 0; i < g_stSceneautoParam.pstIspParam[s32Index].stDrcParam.u32DRCount; i++)
    {
    	g_stSceneautoParam.pstIspParam[s32Index].stDrcParam.pstDRCAttr[i].u32DarkEnhance = MAEWeight[i];
    }

	return HI_SUCCESS;
}

HI_S32 Sceneauto_LoadAE(HI_S32 s32Index)
{
	HI_S32 s32Temp;
    HI_S32 i;
    HI_CHAR szTempStr[64] = {0};
    HI_CHAR* pszTempStr = HI_NULL;

    /**************AE:aeRunInterval**************/
    s32Temp = 0;
    snprintf(szTempStr, sizeof(szTempStr), "ISP_PARAM_%d:aeRunInterval", s32Index);
    s32Temp = iniparser_getint(g_Sceneautodictionary, szTempStr, HI_FAILURE);
    if (HI_FAILURE == s32Temp)
    {
        printf("ISP_PARAM_%d:aeRunInterval failed\n", s32Index);
        return HI_FAILURE;
    }
    g_stSceneautoParam.pstIspParam[s32Index].stAeParam.u8AERunInterval = s32Temp;

	/**************AE:u32TotalNum**************/
	s32Temp = 0;
	snprintf(szTempStr, sizeof(szTempStr), "ISP_PARAM_%d:u32TotalNum", s32Index);
	s32Temp = iniparser_getint(g_Sceneautodictionary, szTempStr, HI_FAILURE);
    if (HI_FAILURE == s32Temp)
    {
        printf("ISP_PARAM_%d:u32TotalNum failed\n", s32Index);
        return HI_FAILURE;
    }
    g_stSceneautoParam.pstIspParam[s32Index].stAeParam.stAeRoute.u32TotalNum = s32Temp;

    CHECK_MALLOC_SIZE(s32Temp);
	g_stSceneautoParam.pstIspParam[s32Index].stAeParam.stAeRoute.pstRuteNode = (HI_SCENEAUTO_ROUTENODE_S*)malloc(s32Temp * sizeof(HI_SCENEAUTO_ROUTENODE_S));
	CHECK_NULL_PTR(g_stSceneautoParam.pstIspParam[s32Index].stAeParam.stAeRoute.pstRuteNode);

	/**************AE:u32IntTime**************/
	snprintf(szTempStr, sizeof(szTempStr), "ISP_PARAM_%d:u32IntTime", s32Index);
	pszTempStr = iniparser_getstr(g_Sceneautodictionary, szTempStr);
    if (NULL == pszTempStr)
    {
        printf("ISP_PARAM_%d:u32IntTime failed\n", s32Index);
        return HI_FAILURE;
    }
    Weight(pszTempStr);
    for (i = 0; i < g_stSceneautoParam.pstIspParam[s32Index].stAeParam.stAeRoute.u32TotalNum; i++)
    {
    	g_stSceneautoParam.pstIspParam[s32Index].stAeParam.stAeRoute.pstRuteNode[i].u32IntTime = MAEWeight[i];
    }

    /**************AE:u32SysGain**************/
	snprintf(szTempStr, sizeof(szTempStr), "ISP_PARAM_%d:u32SysGain", s32Index);
	pszTempStr = iniparser_getstr(g_Sceneautodictionary, szTempStr);
    if (NULL == pszTempStr)
    {
        printf("ISP_PARAM_%d:u32SysGain failed\n", s32Index);
        return HI_FAILURE;
    }
    Weight(pszTempStr);
    for (i = 0; i < g_stSceneautoParam.pstIspParam[s32Index].stAeParam.stAeRoute.u32TotalNum; i++)
    {
    	g_stSceneautoParam.pstIspParam[s32Index].stAeParam.stAeRoute.pstRuteNode[i].u32SysGain = MAEWeight[i];
    }

	/**************AE:aeBitrateCount**************/
	s32Temp = 0;
    snprintf(szTempStr, sizeof(szTempStr), "ISP_PARAM_%d:aeBitrateCount", s32Index);
    s32Temp = iniparser_getint(g_Sceneautodictionary, szTempStr, HI_FAILURE);
    if (HI_FAILURE == s32Temp)
    {
        printf("ISP_PARAM_%d:aeBitrateCount failed\n", s32Index);
        return HI_FAILURE;
    }
    g_stSceneautoParam.pstIspParam[s32Index].stAeParam.u32BitrateCount = s32Temp;

    CHECK_MALLOC_SIZE(s32Temp);
    g_stSceneautoParam.pstIspParam[s32Index].stAeParam.pu32BitrateThresh = (HI_U32*)malloc(s32Temp * sizeof(HI_U32));
    CHECK_NULL_PTR(g_stSceneautoParam.pstIspParam[s32Index].stAeParam.pu32BitrateThresh);
    g_stSceneautoParam.pstIspParam[s32Index].stAeParam.pstAERelatedBit = (HI_SCENEAUTO_AERELATEDBIT_S*)malloc(s32Temp * sizeof(HI_SCENEAUTO_AERELATEDBIT_S));
	CHECK_NULL_PTR(g_stSceneautoParam.pstIspParam[s32Index].stAeParam.pstAERelatedBit);

	/**************AE:aeBitrateThresh**************/
	snprintf(szTempStr, sizeof(szTempStr), "ISP_PARAM_%d:aeBitrateThresh", s32Index);
	pszTempStr = iniparser_getstr(g_Sceneautodictionary, szTempStr);
    if (NULL == pszTempStr)
    {
        printf("ISP_PARAM_%d:aeBitrateThresh failed\n", s32Index);
        return HI_FAILURE;
    }
    Weight(pszTempStr);
    for (i = 0; i < g_stSceneautoParam.pstIspParam[s32Index].stAeParam.u32BitrateCount; i++)
    {
    	g_stSceneautoParam.pstIspParam[s32Index].stAeParam.pu32BitrateThresh[i] = MAEWeight[i];
    }

	/**************AE:u8Speed**************/
    snprintf(szTempStr, sizeof(szTempStr), "ISP_PARAM_%d:u8Speed", s32Index);
	pszTempStr = iniparser_getstr(g_Sceneautodictionary, szTempStr);
    if (NULL == pszTempStr)
    {
        printf("ISP_PARAM_%d:u8Speed failed\n", s32Index);
        return HI_FAILURE;
    }
    Weight(pszTempStr);
    for (i = 0; i < g_stSceneautoParam.pstIspParam[s32Index].stAeParam.u32BitrateCount; i++)
    {
    	g_stSceneautoParam.pstIspParam[s32Index].stAeParam.pstAERelatedBit[i].u8Speed = MAEWeight[i];
    }

    /**************AE:u8Tolerance**************/
    snprintf(szTempStr, sizeof(szTempStr), "ISP_PARAM_%d:u8Tolerance", s32Index);
	pszTempStr = iniparser_getstr(g_Sceneautodictionary, szTempStr);
    if (NULL == pszTempStr)
    {
        printf("ISP_PARAM_%d:u8Tolerance failed\n", s32Index);
        return HI_FAILURE;
    }
    Weight(pszTempStr);
    for (i = 0; i < g_stSceneautoParam.pstIspParam[s32Index].stAeParam.u32BitrateCount; i++)
    {
    	g_stSceneautoParam.pstIspParam[s32Index].stAeParam.pstAERelatedBit[i].u8Tolerance = MAEWeight[i];
    }

    /**************AE:u16BlackDelayFrame**************/
    snprintf(szTempStr, sizeof(szTempStr), "ISP_PARAM_%d:u16BlackDelayFrame", s32Index);
	pszTempStr = iniparser_getstr(g_Sceneautodictionary, szTempStr);
    if (NULL == pszTempStr)
    {
        printf("ISP_PARAM_%d:u16BlackDelayFrame failed\n", s32Index);
        return HI_FAILURE;
    }
    Weight(pszTempStr);
    for (i = 0; i < g_stSceneautoParam.pstIspParam[s32Index].stAeParam.u32BitrateCount; i++)
    {
    	g_stSceneautoParam.pstIspParam[s32Index].stAeParam.pstAERelatedBit[i].u16BlackDelayFrame = MAEWeight[i];
    }

    /**************AE:u16WhiteDelayFrame**************/
    snprintf(szTempStr, sizeof(szTempStr), "ISP_PARAM_%d:u16WhiteDelayFrame", s32Index);
	pszTempStr = iniparser_getstr(g_Sceneautodictionary, szTempStr);
    if (NULL == pszTempStr)
    {
        printf("ISP_PARAM_%d:u16WhiteDelayFrame failed\n", s32Index);
        return HI_FAILURE;
    }
    Weight(pszTempStr);
    for (i = 0; i < g_stSceneautoParam.pstIspParam[s32Index].stAeParam.u32BitrateCount; i++)
    {
    	g_stSceneautoParam.pstIspParam[s32Index].stAeParam.pstAERelatedBit[i].u16WhiteDelayFrame = MAEWeight[i];
    }

    /**************AE:u32SysGainMax**************/
    snprintf(szTempStr, sizeof(szTempStr), "ISP_PARAM_%d:u32SysGainMax", s32Index);
	pszTempStr = iniparser_getstr(g_Sceneautodictionary, szTempStr);
    if (NULL == pszTempStr)
    {
        printf("ISP_PARAM_%d:u32SysGainMax failed\n", s32Index);
        return HI_FAILURE;
    }
    Weight(pszTempStr);
    for (i = 0; i < g_stSceneautoParam.pstIspParam[s32Index].stAeParam.u32BitrateCount; i++)
    {
    	g_stSceneautoParam.pstIspParam[s32Index].stAeParam.pstAERelatedBit[i].u32SysGainMax = MAEWeight[i];
    }

	/**************AE:aeExpCount**************/
	s32Temp = 0;
	snprintf(szTempStr, sizeof(szTempStr), "ISP_PARAM_%d:aeExpCount", s32Index);
    s32Temp = iniparser_getint(g_Sceneautodictionary, szTempStr, HI_FAILURE);
    if (HI_FAILURE == s32Temp)
    {
        printf("ISP_PARAM_%d:aeExpCount failed\n", s32Index);
        return HI_FAILURE;
    }
    g_stSceneautoParam.pstIspParam[s32Index].stAeParam.u32ExpCount = s32Temp;

    CHECK_MALLOC_SIZE(s32Temp);
	g_stSceneautoParam.pstIspParam[s32Index].stAeParam.pu32AEExpHtoLThresh = (HI_U32*)malloc(s32Temp * sizeof(HI_U32));
    CHECK_NULL_PTR(g_stSceneautoParam.pstIspParam[s32Index].stAeParam.pu32AEExpHtoLThresh);
    g_stSceneautoParam.pstIspParam[s32Index].stAeParam.pu32AEExpLtoHThresh = (HI_U32*)malloc(s32Temp * sizeof(HI_U32));
    CHECK_NULL_PTR(g_stSceneautoParam.pstIspParam[s32Index].stAeParam.pu32AEExpLtoHThresh);
    g_stSceneautoParam.pstIspParam[s32Index].stAeParam.pstAERelatedExp = (HI_SCENEAUTO_AERELATEDEXP_S*)malloc(s32Temp * sizeof(HI_SCENEAUTO_AERELATEDEXP_S));
	CHECK_NULL_PTR(g_stSceneautoParam.pstIspParam[s32Index].stAeParam.pstAERelatedExp);

	/**************AE:aeExpHtoLThresh**************/
	snprintf(szTempStr, sizeof(szTempStr), "ISP_PARAM_%d:aeExpHtoLThresh", s32Index);
	pszTempStr = iniparser_getstr(g_Sceneautodictionary, szTempStr);
    if (NULL == pszTempStr)
    {
        printf("ISP_PARAM_%d:aeExpHtoLThresh failed\n", s32Index);
        return HI_FAILURE;
    }
    Weight(pszTempStr);
    for (i = 0; i < g_stSceneautoParam.pstIspParam[s32Index].stAeParam.u32ExpCount; i++)
    {
    	g_stSceneautoParam.pstIspParam[s32Index].stAeParam.pu32AEExpHtoLThresh[i] = MAEWeight[i];
    }

   	/**************AE:aeExpLtoHThresh**************/
	snprintf(szTempStr, sizeof(szTempStr), "ISP_PARAM_%d:aeExpLtoHThresh", s32Index);
	pszTempStr = iniparser_getstr(g_Sceneautodictionary, szTempStr);
    if (NULL == pszTempStr)
    {
        printf("ISP_PARAM_%d:aeExpLtoHThresh failed\n", s32Index);
        return HI_FAILURE;
    }
    Weight(pszTempStr);
    for (i = 0; i < g_stSceneautoParam.pstIspParam[s32Index].stAeParam.u32ExpCount; i++)
    {
    	g_stSceneautoParam.pstIspParam[s32Index].stAeParam.pu32AEExpLtoHThresh[i] = MAEWeight[i];
    }

    /**************AE:aeCompesation**************/
	snprintf(szTempStr, sizeof(szTempStr), "ISP_PARAM_%d:aeCompesation", s32Index);
	pszTempStr = iniparser_getstr(g_Sceneautodictionary, szTempStr);
    if (NULL == pszTempStr)
    {
        printf("ISP_PARAM_%d:aeCompesation failed\n", s32Index);
        return HI_FAILURE;
    }
    Weight(pszTempStr);
    for (i = 0; i < g_stSceneautoParam.pstIspParam[s32Index].stAeParam.u32ExpCount; i++)
    {
    	g_stSceneautoParam.pstIspParam[s32Index].stAeParam.pstAERelatedExp[i].u8AECompesation = MAEWeight[i];
    }

	/**************AE:aeHistOffset**************/
	snprintf(szTempStr, sizeof(szTempStr), "ISP_PARAM_%d:aeHistOffset", s32Index);
	pszTempStr = iniparser_getstr(g_Sceneautodictionary, szTempStr);
    if (NULL == pszTempStr)
    {
        printf("ISP_PARAM_%d:aeHistOffset failed\n", s32Index);
        return HI_FAILURE;
    }
    Weight(pszTempStr);
    for (i = 0; i < g_stSceneautoParam.pstIspParam[s32Index].stAeParam.u32ExpCount; i++)
    {
    	g_stSceneautoParam.pstIspParam[s32Index].stAeParam.pstAERelatedExp[i].u8AEHistOffset = MAEWeight[i];
    }
	return HI_SUCCESS;
}

HI_S32 Sceneauto_LoadIspParam()
{
	HI_S32 s32Ret = HI_SUCCESS;
	HI_S32 s32Temp = 0;
	HI_S32 s32Index;
	HI_CHAR szTempStr[64] = {0};

    CHECK_MALLOC_SIZE((g_stSceneautoParam.stCommParam.s32IspDevCount) * sizeof(HI_SCENEAUTO_ISPPARAM_S));
	g_stSceneautoParam.pstIspParam = (HI_SCENEAUTO_ISPPARAM_S *)malloc((g_stSceneautoParam.stCommParam.s32IspDevCount) * sizeof(HI_SCENEAUTO_ISPPARAM_S));
	CHECK_NULL_PTR(g_stSceneautoParam.pstIspParam);

	for (s32Index = 0; s32Index < g_stSceneautoParam.stCommParam.s32IspDevCount; s32Index++)
	{
		snprintf(szTempStr, sizeof(szTempStr), "ISP_PARAM_%d:ISP_DEV", s32Index);
		s32Temp = 0;
        s32Temp = iniparser_getint(g_Sceneautodictionary, szTempStr, HI_FAILURE);
        if (HI_FAILURE == s32Temp)
        {
            printf("ISP_PARAM_%d:ISP_DEV failed\n", s32Index);
            return HI_FAILURE;
        }
        g_stSceneautoParam.pstIspParam[s32Index].IspDev = s32Temp;
        ISP_CHECK_DEV(g_stSceneautoParam.pstIspParam[s32Index].IspDev);

		s32Ret = Sceneauto_LoadAE(s32Index);
		if (HI_SUCCESS != s32Ret)
		{
     		printf("(s32Index: %d) Sceneauto_LoadAE failed\n", s32Index);
            return HI_FAILURE;
		}

		s32Ret = Sceneauto_LoadDRC(s32Index);
		if (HI_SUCCESS != s32Ret)
		{
     		printf("(s32Index: %d) Sceneauto_LoadDRC failed\n", s32Index);
            return HI_FAILURE;
		}

		s32Ret = Sceneauto_LoadDemosaic(s32Index);
		if (HI_SUCCESS != s32Ret)
		{
     		printf("(s32Index: %d) Sceneauto_LoadDemosaic failed\n", s32Index);
            return HI_FAILURE;
		}

		s32Ret = Sceneauto_LoadSharpen(s32Index);
		if (HI_SUCCESS != s32Ret)
		{
     		printf("(s32Index: %d) Sceneauto_LoadSharpen failed\n", s32Index);
            return HI_FAILURE;
		}

		s32Ret = Sceneauto_LoadDp(s32Index);
		if (HI_SUCCESS != s32Ret)
		{
     		printf("(s32Index: %d) Sceneauto_LoadDp failed\n", s32Index);
            return HI_FAILURE;
		}

		s32Ret = Sceneauto_LoadFalseColor(s32Index);
		if (HI_SUCCESS != s32Ret)
		{
     		printf("(s32Index: %d) Sceneauto_LoadDp failed\n", s32Index);
            return HI_FAILURE;
		}

		s32Ret = Sceneauto_LoadGamma(s32Index);
		if (HI_SUCCESS != s32Ret)
		{
     		printf("(s32Index: %d) Sceneauto_LoadGamma failed\n", s32Index);
            return HI_FAILURE;
		}
	}

	return HI_SUCCESS;
}

HI_S32 Sceneauto_LoadCommon()
{
    HI_S32 s32Temp;

    /**************common:IspDevCount**************/
    s32Temp = 0;
    s32Temp = iniparser_getint(g_Sceneautodictionary, "common:IspDevCount", HI_FAILURE);
    if (HI_FAILURE == s32Temp)
    {
        printf("common:IspDevCount failed\n");
        return HI_FAILURE;
    }
    g_stSceneautoParam.stCommParam.s32IspDevCount = s32Temp;
    CHECK_ISP_DEV_COUNT(g_stSceneautoParam.stCommParam.s32IspDevCount);

    /**************common:ViDevCount**************/
    s32Temp = 0;
    s32Temp = iniparser_getint(g_Sceneautodictionary, "common:ViDevCount", HI_FAILURE);
    if (HI_FAILURE == s32Temp)
    {
        printf("common:ViDevCount failed\n");
        return HI_FAILURE;
    }
    g_stSceneautoParam.stCommParam.s32ViDevCount = s32Temp;
    CHECK_ISP_DEV_COUNT(g_stSceneautoParam.stCommParam.s32ViDevCount);

    /**************common:VpssGrpCount**************/
    s32Temp = 0;
    s32Temp = iniparser_getint(g_Sceneautodictionary, "common:VpssGrpCount", HI_FAILURE);
    if (HI_FAILURE == s32Temp)
    {
        printf("common:VpssGrpCount failed\n");
        return HI_FAILURE;
    }
    g_stSceneautoParam.stCommParam.s32VpssGrpCount = s32Temp;
    CHECK_ISP_DEV_COUNT(g_stSceneautoParam.stCommParam.s32VpssGrpCount);

    /**************common:ave_lum_thresh**************/
    s32Temp = 0;
    s32Temp = iniparser_getint(g_Sceneautodictionary, "common:ave_lum_thresh", HI_FAILURE);
    if (HI_FAILURE == s32Temp)
    {
        printf("common:ave_lum_thresh failed\n");
        return HI_FAILURE;
    }
    g_stSceneautoParam.stCommParam.u32AveLumThresh = s32Temp;

    /**************common:delta_dis_expthresh**************/
    s32Temp = 0;
    s32Temp = iniparser_getint(g_Sceneautodictionary, "common:delta_dis_expthresh", HI_FAILURE);
    if (HI_FAILURE == s32Temp)
    {
        printf("common:delta_dis_expthresh failed\n");
        return HI_FAILURE;
    }
    g_stSceneautoParam.stCommParam.u32DeltaDisExpThresh = s32Temp;

    /**************common:u32DRCStrengthThresh**************/
    s32Temp = 0;
    s32Temp = iniparser_getint(g_Sceneautodictionary, "common:u32DRCStrengthThresh", HI_FAILURE);
    if (HI_FAILURE == s32Temp)
    {
        printf("common:u32DRCStrengthThresh failed\n");
        return HI_FAILURE;
    }
    g_stSceneautoParam.stCommParam.u32DRCStrengthThresh = s32Temp;

    return HI_SUCCESS;
}

HI_S32 Sceneauto_LoadINIPara()
{
    HI_S32 s32Ret = HI_SUCCESS;

    s32Ret = Sceneauto_LoadCommon();
    if (HI_SUCCESS != s32Ret)
    {
        printf("Sceneauto_LoadCommon failed!\n");
        return HI_FAILURE;
    }

	s32Ret = Sceneauto_LoadIspParam();
	if (HI_SUCCESS != s32Ret)
    {
        printf("Sceneauto_LoadIspParam failed!\n");
        return HI_FAILURE;
    }

	s32Ret = Sceneauto_LoadViParam();
	if (HI_SUCCESS != s32Ret)
    {
        printf("Sceneauto_LoadViParam failed!\n");
        return HI_FAILURE;
    }

    s32Ret = Sceneauto_LoadVpssParam();
    if (HI_SUCCESS != s32Ret)
    {
        printf("Sceneauto_LoadVpssParam failed!\n");
        return HI_FAILURE;
    }

	s32Ret = Sceneauto_LoadIRParam();
    if (HI_SUCCESS != s32Ret)
    {
        printf("Sceneauto_LoadIRParam failed!\n");
        return HI_FAILURE;
    }

	s32Ret = Sceneauto_LoadHLCParam();
    if (HI_SUCCESS != s32Ret)
    {
        printf("Sceneauto_LoadHLCParam failed!\n");
        return HI_FAILURE;
    }

    s32Ret = Sceneauto_LoadTrafficParam();
    if (HI_SUCCESS != s32Ret)
    {
        printf("Sceneauto_LoadTrafficParam failed!\n");
        return HI_FAILURE;
    }

    s32Ret = Sceneauto_LoadFswdrParam();
    if (HI_SUCCESS != s32Ret)
    {
        printf("Sceneauto_LoadFswdrParam failed!\n");
        return HI_FAILURE;
    }

    s32Ret = Sceneauto_LoadFaceParam();
    if (HI_SUCCESS != s32Ret)
    {
        printf("Sceneauto_LoadFaceParam failed!\n");
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}


HI_S32 Sceneauto_LoadFile(const HI_CHAR* pszFILENAME)
{
    if (NULL != g_Sceneautodictionary)
    {
        g_Sceneautodictionary = NULL;
    }
    else
    {
        g_Sceneautodictionary = iniparser_load(pszFILENAME);
        if (NULL == g_Sceneautodictionary)
        {
            printf("%s ini load failed\n", pszFILENAME);
            return HI_FAILURE;
        }
    }
    return HI_SUCCESS;
}

HI_VOID Sceneauto_FreeDict()
{
    if (NULL != g_Sceneautodictionary)
    {
        iniparser_freedict(g_Sceneautodictionary);
    }
    g_Sceneautodictionary = NULL;
}

HI_VOID Sceneauto_FreeFswdrParam()
{
	FREE_PTR(g_stSceneautoParam.stFswdrParam.stFSWDR3dnr.pst3dnrParam);
    FREE_PTR(g_stSceneautoParam.stFswdrParam.stFSWDR3dnr.pu323DnrIsoThresh);
	FREE_PTR(g_stSceneautoParam.stFswdrParam.pu8MaxHistOffset);
	FREE_PTR(g_stSceneautoParam.stFswdrParam.pu8ExpCompensation);
	FREE_PTR(g_stSceneautoParam.stFswdrParam.pu32ExpThreshHtoL);
	FREE_PTR(g_stSceneautoParam.stFswdrParam.pu32ExpThreshLtoH);
}

HI_VOID Sceneauto_FreeTrafficParam()
{
	FREE_PTR(g_stSceneautoParam.stTrafficParam.stTraffic3dnr.pst3dnrParam);
    FREE_PTR(g_stSceneautoParam.stTrafficParam.stTraffic3dnr.pu323DnrIsoThresh);
}

HI_VOID Sceneauto_FreeFaceParam()
{
	FREE_PTR(g_stSceneautoParam.stFaceParam.stFace3dnrx.pst3dnrxParam);
    FREE_PTR(g_stSceneautoParam.stFaceParam.stFace3dnrx.pu323DnrIsoThresh);
}

HI_VOID Sceneauto_FreeHlcParam()
{
	FREE_PTR(g_stSceneautoParam.stHlcParam.stHlc3dnr.pst3dnrParam);
    FREE_PTR(g_stSceneautoParam.stHlcParam.stHlc3dnr.pu323DnrIsoThresh);
}

HI_VOID Sceneauto_FreeIrParam()
{
	FREE_PTR(g_stSceneautoParam.stIrParam.stIr3dnr.pst3dnrParam);
    FREE_PTR(g_stSceneautoParam.stIrParam.stIr3dnr.pu323DnrIsoThresh);
	FREE_PTR(g_stSceneautoParam.stIrParam.pu8MaxHistOffset);
	FREE_PTR(g_stSceneautoParam.stIrParam.pu8ExpCompensation);
	FREE_PTR(g_stSceneautoParam.stIrParam.pu32ExpThreshHtoL);
	FREE_PTR(g_stSceneautoParam.stIrParam.pu32ExpThreshLtoH);
}

HI_VOID Sceneauto_FreeVpssParam()
{
	HI_S32 i;
	if (NULL != g_stSceneautoParam.pstVpssParam)
	{
		for (i = 0; i < g_stSceneautoParam.stCommParam.s32VpssGrpCount; i++)
		{
            if (0 == g_stSceneautoParam.pstVpssParam[i].u32NrSel)
            {
			    FREE_PTR(g_stSceneautoParam.pstVpssParam[i].st3dnrParam.pst3dnrParam);
                FREE_PTR(g_stSceneautoParam.pstVpssParam[i].st3dnrParam.pu323DnrIsoThresh);
            }
            else if (1 == g_stSceneautoParam.pstVpssParam[i].u32NrSel)
            {
			    FREE_PTR(g_stSceneautoParam.pstVpssParam[i].st3dnrParamX.pst3dnrxParam);
                FREE_PTR(g_stSceneautoParam.pstVpssParam[i].st3dnrParamX.pu323DnrIsoThresh);
            }
		}

		free(g_stSceneautoParam.pstVpssParam);
		g_stSceneautoParam.pstVpssParam = NULL;
	}

}

HI_VOID Sceneauto_FreeViParam()
{
	FREE_PTR(g_stSceneautoParam.pstViParam);
}

HI_VOID Sceneauto_FreeIspParam()
{
	HI_S32 i;
	if (NULL != g_stSceneautoParam.pstIspParam)
	{
		for (i = 0; i < g_stSceneautoParam.stCommParam.s32IspDevCount; i++)
		{
			FREE_PTR(g_stSceneautoParam.pstIspParam[i].stGammaParam.pstGamma);

			FREE_PTR(g_stSceneautoParam.pstIspParam[i].stGammaParam.pu32ExpThreshHtoL);

			FREE_PTR(g_stSceneautoParam.pstIspParam[i].stGammaParam.pu32ExpThreshLtoH);

			FREE_PTR(g_stSceneautoParam.pstIspParam[i].stFalseColorParam.pstFalseColor);

			FREE_PTR(g_stSceneautoParam.pstIspParam[i].stFalseColorParam.pu32ExpThresh);

			FREE_PTR(g_stSceneautoParam.pstIspParam[i].stDpParam.pstDPAttr);

			FREE_PTR(g_stSceneautoParam.pstIspParam[i].stDpParam.pu32ExpThresh);

			FREE_PTR(g_stSceneautoParam.pstIspParam[i].stSharpenParam.pstDynamicSharpen);

			FREE_PTR(g_stSceneautoParam.pstIspParam[i].stSharpenParam.pu32ExpThresh);

			FREE_PTR(g_stSceneautoParam.pstIspParam[i].stDemosaicParam.pstDemosaic);

			FREE_PTR(g_stSceneautoParam.pstIspParam[i].stDemosaicParam.pu32ExpThresh);

			FREE_PTR(g_stSceneautoParam.pstIspParam[i].stDrcParam.pstDRCAttr);

			FREE_PTR(g_stSceneautoParam.pstIspParam[i].stDrcParam.pu32DRThresh);

			FREE_PTR(g_stSceneautoParam.pstIspParam[i].stAeParam.pstAERelatedExp);

			FREE_PTR(g_stSceneautoParam.pstIspParam[i].stAeParam.pu32AEExpLtoHThresh);

			FREE_PTR(g_stSceneautoParam.pstIspParam[i].stAeParam.pu32AEExpHtoLThresh);

			FREE_PTR(g_stSceneautoParam.pstIspParam[i].stAeParam.pstAERelatedBit);

			FREE_PTR(g_stSceneautoParam.pstIspParam[i].stAeParam.pu32BitrateThresh);

			FREE_PTR(g_stSceneautoParam.pstIspParam[i].stAeParam.stAeRoute.pstRuteNode);
		}

		free(g_stSceneautoParam.pstIspParam);
		g_stSceneautoParam.pstIspParam = NULL;
	}
}

HI_VOID Sceneauto_FreeMem()
{
	Sceneauto_FreeIspParam();

	Sceneauto_FreeViParam();

	Sceneauto_FreeVpssParam();

	Sceneauto_FreeIrParam();

	Sceneauto_FreeHlcParam();

	Sceneauto_FreeTrafficParam();

	Sceneauto_FreeFswdrParam();
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

