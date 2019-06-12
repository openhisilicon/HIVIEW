#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <sys/prctl.h>
#include "hi_type.h"
//#include "hi_confaccess.h"
#include "iniparser.h"
#include "hi_sceneauto_comm.h"
#include "hi_sceneauto_comm_ext.h"
#include "hi_sceneauto_define.h"
#include "hi_srdk_sceneauto_define_ext.h"
#include "hi_srdk_sceneauto_ext.h"


#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

#define MAX_LEVEL   32
//#define DRCSTREGHT_THRESH 48

static dictionary* g_Sceneautodictionary = NULL;
HI_BOOL g_bSceneautoInit = HI_FALSE;
HI_BOOL g_bSceneautoStart = HI_FALSE;
HI_BOOL g_bNormalThreadFlag = HI_FALSE;
HI_BOOL g_bSpecialThreadFlag = HI_FALSE;
SCENEAUTO_INIPARA_S g_stINIPara;
SCENEAUTO_PREVIOUSPARA_S g_stPreviousPara;
SRDK_SCENEAUTO_SEPCIAL_SCENE_E g_eSpecialScene;
ADPT_SCENEAUTO_VENC_RCMODE_E g_eVencRcMode;

pthread_mutex_t g_stSceneautoLock = PTHREAD_MUTEX_INITIALIZER;

pthread_t g_pthSceneAutoNormal;
pthread_t g_pthSceneAutoSpecial;

#define CHECK_SCENEAUTO_INIT()\
    do{\
        pthread_mutex_lock(&g_stSceneautoLock);\
        if (HI_FALSE == g_bSceneautoInit)\
        {\
            printf("func:%s,line:%d, please init sceneauto first!\n",__FUNCTION__,__LINE__);\
            pthread_mutex_unlock(&g_stSceneautoLock);\
            return HI_FAILURE;\
        }\
        pthread_mutex_unlock(&g_stSceneautoLock);\
    }while(0)

#define CHECK_ISP_RUN(chn) \
    do{\
        pthread_mutex_lock(&g_stSceneautoLock);\
        if(HI_FALSE == g_bSceneautoStart)\
        {\
            printf("func:%s,line:%d, please make sceneauto run !\n",__FUNCTION__,__LINE__);\
            pthread_mutex_unlock(&g_stSceneautoLock);\
            return HI_FAILURE;\
        }\
        pthread_mutex_unlock(&g_stSceneautoLock);\
    }while(0)

#define CHECK_NULL_PTR(ptr)\
    do{\
        if(NULL == ptr)\
        {\
            printf("func:%s,line:%d, NULL pointer\n",__FUNCTION__,__LINE__);\
            return HI_FAILURE;\
        }\
    }while(0)

static __inline int iClip2(int x, int b)       {{ if (x < 0) x = 0; };{ if (x > b) x = b; }; return x; }
static __inline int iMin2(int a, int b) {{ if (a > b) a = b; }; return a; }
static __inline int iMax2(int a, int b) {{ if (a > b) b = a; }; return b; }
static int MAEWeight[128];
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

HI_S32 InterpolationCalculate(HI_S32 s32Value1,
                              HI_S32 s32Value2,
                              HI_S32 s32Data1,
                              HI_S32 s32Data2,
                              HI_S32 s32Data)
{
    HI_S32 s32Result;

    if (s32Data2 == s32Data1)
    {
        s32Result = s32Value2;
    }
    else
    {
        s32Result = ((s32Value2 - s32Value1) * (s32Data - s32Data1)) / (s32Data2 - s32Data1) + s32Value1;
    }
    return s32Result;
}

static int GetDPsOrDSs(int S, int thresh, int ratio)
{
  int  i = (((S < thresh) ? S : thresh) << 4); 

  int  j = ((ratio >= 32) ?  (S << 4) : 
           ((ratio >  0 ) ? ((S * ratio + 1) >> 1) : 0));

  if (j > i) { i = j; }
  return  i;
}

static int MapMTFS(int *pp, int *ps, const SCENEAUTO_INIPARAM_3DNRCFG_S *pst3dnrcfg)
{
    if (pst3dnrcfg == NULL)
    {
        printf("PE is NULL!\n");
        return HI_FAILURE;
    }

    if (1)
    {
        int dp = GetDPsOrDSs(pst3dnrcfg->u8SBS[2], pst3dnrcfg->u8TextThr, pst3dnrcfg->u8SBT[2]);
        int ds = GetDPsOrDSs(pst3dnrcfg->u8SDS[2], pst3dnrcfg->u8TextThr, pst3dnrcfg->u8SDT[2]);
        int k;
        
        if ( pst3dnrcfg->u8MTFS < 128) 
        { 
            k = iMin2( pst3dnrcfg->u8MTFS, 16 ); 
        } 
        else if ((pst3dnrcfg->u8SBT[2] >  32) || (pst3dnrcfg->u8SDT[2] > 32)) 
        {   
            k = 0; 
        } 
        else
        {
            int  i = (iMax2(pst3dnrcfg->u8SBS[2],pst3dnrcfg->u8SDS[2]) << 4), j = iMax2(dp,ds);   
            k  = (( (i << 1)+ j     +(1 << 6) ) >> 7);
            k += (( (i << 1)+(j * 3)+(1 << 8) ) >> 9);
            k  = iClip2( 34 - k * 3, 16 ); 
        }

    if (pp != NULL) { *pp = dp; }
    if (ps != NULL) { *ps = ds; }

    return  k;
  }
}

static int MapISO(int iso)
{
  int   j,  i = (iso >= 200);
  i += ( (iso >= (200 << 1)) + (iso >= (400 << 1)) + (iso >= (400 << 2)) + (iso >= (400 << 3)) + (iso >= (400 << 4)) );
  i += ( (iso >= (400 << 5)) + (iso >= (400 << 6)) + (iso >= (400 << 7)) + (iso >= (400 << 8)) + (iso >= (400 << 9)) );
  j  = ( (iso >  (112 << i)) + (iso >  (125 << i)) + (iso >  (141 << i)) + (iso >  (158 << i)) + (iso >  (178 << i)) );
  return (i * 6 + j + (iso >= 25) + (iso >= 50) + (iso >= 100));  
}
HI_VOID Interpolate(SCENEAUTO_INIPARAM_3DNRCFG_S *pst3dnrcfg, HI_U32 u32Mid, 
                                const SCENEAUTO_INIPARAM_3DNRCFG_S *pstL3dnrcfg, HI_U32 u32Left,  
                                const SCENEAUTO_INIPARAM_3DNRCFG_S *pstR3dnrcfg, HI_U32 u32Right)
{
  int   k, left, right, i = ((u32Mid > 3) ? MapISO(u32Mid) : iMin2(71,-u32Mid));  
  left  = ((u32Left  > 3) ? MapISO(u32Left) : iMin2(71,-u32Left) ); if (i <= left)  { *pst3dnrcfg = *pstL3dnrcfg; return; }
  right = ((u32Right > 3) ? MapISO(u32Right): iMin2(71,-u32Right)); if (i >= right) { *pst3dnrcfg = *pstR3dnrcfg; return; }
  k = (right - left); *pst3dnrcfg = *(( (i+((k * 3) >> 2)) < right ) ? pstL3dnrcfg : pstR3dnrcfg);
  if (pstL3dnrcfg->u16POST == pstR3dnrcfg->u16POST)
  {
    pst3dnrcfg->u16POST =                  pstL3dnrcfg->u16POST;
    pst3dnrcfg->u16PSFS = ( ((right - i) * pstL3dnrcfg->u16PSFS + (i - left) * pstR3dnrcfg->u16PSFS + (k >> 1)) / k ); 
    if ((pstL3dnrcfg->u8SHP[0] > 64) == (pstR3dnrcfg->u8SHP[0] > 64))
    {                                                                                                     
      pst3dnrcfg->u8SBS[0] = ( ((right - i) * pstL3dnrcfg->u8SBS[0] + (i - left) * pstR3dnrcfg->u8SBS[0] + (k >> 1)) / k ); 
      pst3dnrcfg->u8SDS[0] = ( ((right - i) * pstL3dnrcfg->u8SDS[0] + (i - left) * pstR3dnrcfg->u8SDS[0] + (k >> 1)) / k ); 
      pst3dnrcfg->u8SBT[0] = ( ((right - i) * pstL3dnrcfg->u8SBT[0] + (i - left) * pstR3dnrcfg->u8SBT[0] + (k >> 1)) / k ); 
      pst3dnrcfg->u8SDT[0] = ( ((right - i) * pstL3dnrcfg->u8SDT[0] + (i - left) * pstR3dnrcfg->u8SDT[0] + (k >> 1)) / k ); 
      pst3dnrcfg->u8SHP[0] = ( ((right - i) * pstL3dnrcfg->u8SHP[0] + (i - left) * pstR3dnrcfg->u8SHP[0] + (k >> 1)) / k ); 
      pst3dnrcfg->u16TFR[0] = ( ((right - i) * pstL3dnrcfg->u16TFR[0] + (i - left) * pstR3dnrcfg->u16TFR[0] + (k >> 1)) / k ); 
      pst3dnrcfg->u16TFS[0] = ( ((right - i) * pstL3dnrcfg->u16TFS[0] + (i - left) * pstR3dnrcfg->u16TFS[0] + (k >> 1)) / k ); 
      pst3dnrcfg->u16MDZ[0] = ( ((right - i) * pstL3dnrcfg->u16MDZ[0] + (i - left) * pstR3dnrcfg->u16MDZ[0] + (k >> 1)) / k ); 
    }
    if ((pstL3dnrcfg->u8SHP[1] > 64) == (pstR3dnrcfg->u8SHP[1] > 64))
    {                                                                                                     
      pst3dnrcfg->u8SBS[1] = ( ((right - i) * pstL3dnrcfg->u8SBS[1] + (i - left) * pstR3dnrcfg->u8SBS[1] + (k >> 1)) / k ); 
      pst3dnrcfg->u8SDS[1] = ( ((right - i) * pstL3dnrcfg->u8SDS[1] + (i - left) * pstR3dnrcfg->u8SDS[1] + (k >> 1)) / k ); 
      pst3dnrcfg->u8SBT[1] = ( ((right - i) * pstL3dnrcfg->u8SBT[1] + (i - left) * pstR3dnrcfg->u8SBT[1] + (k >> 1)) / k ); 
      pst3dnrcfg->u8SDT[1] = ( ((right - i) * pstL3dnrcfg->u8SDT[1] + (i - left) * pstR3dnrcfg->u8SDT[1] + (k >> 1)) / k ); 
      pst3dnrcfg->u8SHP[1] = ( ((right - i) * pstL3dnrcfg->u8SHP[1] + (i - left) * pstR3dnrcfg->u8SHP[1] + (k >> 1)) / k ); 
      pst3dnrcfg->u16TFR[1] = ( ((right - i) * pstL3dnrcfg->u16TFR[1] + (i - left) * pstR3dnrcfg->u16TFR[1] + (k >> 1)) / k ); 
      pst3dnrcfg->u16TFS[1] = ( ((right - i) * pstL3dnrcfg->u16TFS[1] + (i - left) * pstR3dnrcfg->u16TFS[1] + (k >> 1)) / k ); 
      pst3dnrcfg->u16MDZ[1] = ( ((right - i) * pstL3dnrcfg->u16MDZ[1] + (i - left) * pstR3dnrcfg->u16MDZ[1] + (k >> 1)) / k ); 
      pst3dnrcfg->u8PostROW= ( ((right - i) * pstL3dnrcfg->u8PostROW  + (i - left) * pstR3dnrcfg->u8PostROW  + (k >> 1)) / k ); 
    }
    pst3dnrcfg->u8SBS[2] = ( ((right - i) * pstL3dnrcfg->u8SBS[2] + (i - left) * pstR3dnrcfg->u8SBS[2] + (k >> 1)) / k ); 
    pst3dnrcfg->u8SDS[2] = ( ((right - i) * pstL3dnrcfg->u8SDS[2] + (i - left) * pstR3dnrcfg->u8SDS[2] + (k >> 1)) / k ); 
    pst3dnrcfg->u8SBT[2] = ( ((right - i) * pstL3dnrcfg->u8SBT[2] + (i - left) * pstR3dnrcfg->u8SBT[2] + (k >> 1)) / k ); 
    pst3dnrcfg->u8SDT[2] = ( ((right - i) * pstL3dnrcfg->u8SDT[2] + (i - left) * pstR3dnrcfg->u8SDT[2] + (k >> 1)) / k ); 
    pst3dnrcfg->u8SHP[2] = (( iMin2(pstL3dnrcfg->u8SHP[2], 64) * (right - i) + 
                       iMin2(pstR3dnrcfg->u8SHP[2], 64) * (i  - left) + (k >> 1) ) / k); 
    pst3dnrcfg->u16TFR[2] = ( ((right - i) * pstL3dnrcfg->u16TFR[2] + (i - left) * pstR3dnrcfg->u16TFR[2]  + (k >> 1)) / k ); 
    pst3dnrcfg->u16TFS[2] = ( ((right - i) * pstL3dnrcfg->u16TFS[2] + (i - left) * pstR3dnrcfg->u16TFS[2]  + (k >> 1)) / k ); 
    pst3dnrcfg->u16MDZ[2] = ( ((right - i) * pstL3dnrcfg->u16MDZ[2] + (i - left) * pstR3dnrcfg->u16MDZ[2]  + (k >> 1)) / k ); 
    pst3dnrcfg->u8SFC = ( ((right - i) * pstL3dnrcfg->u8SFC + (i - left) * pstR3dnrcfg->u8SFC + (k >> 1)) / k ); 
    pst3dnrcfg->u8TFC = ( ((right - i) * pstL3dnrcfg->u8TFC + (i - left) * pstR3dnrcfg->u8TFC + (k >> 1)) / k ); 
    pst3dnrcfg->u8ExTfThr = ( ((right - i) * pstL3dnrcfg->u8ExTfThr + (i - left) * pstR3dnrcfg->u8ExTfThr + (k >> 1)) / k ); 
    pst3dnrcfg->u8TextThr = ( ((right - i) * pstL3dnrcfg->u8TextThr + (i - left) * pstR3dnrcfg->u8TextThr + (k >> 1)) / k ); 
    pst3dnrcfg->u8MDAF = ( ((right - i) * pstL3dnrcfg->u8MDAF + (i - left) * pstR3dnrcfg->u8MDAF + (k >> 1)) / k ); 
    pst3dnrcfg->u8MABW = ( ((right - i) * pstL3dnrcfg->u8MABW + (i - left) * pstR3dnrcfg->u8MABW + (k >> 1)) / k ); 
    pst3dnrcfg->u8MATW = ( ((right - i) * pstL3dnrcfg->u8MATW + (i - left) * pstR3dnrcfg->u8MATW + (k >> 1)) / k ); 
    pst3dnrcfg->u8MTFS = (( MapMTFS( NULL, NULL, pstL3dnrcfg ) * (right - i) + 
                  MapMTFS( NULL, NULL, pstR3dnrcfg ) * (i  - left) + (k >> 1) ) / k); 
  }
} 

HI_S32 SceneAuto_SetDRC()
{
    HI_S32 s32Ret = HI_SUCCESS;
    HI_S32 s32IspDev;
    ADPT_SCENEAUTO_DRCATTR_S stAdptDRCAttr;
    s32IspDev = g_stINIPara.stMpInfo.s32IspDev;
    s32Ret = CommSceneautoGetDRCAttr(s32IspDev, &stAdptDRCAttr);
    if (HI_SUCCESS != s32Ret)
    {
        printf("CommSceneautoGetDRCAttr failed\n");
        return HI_FAILURE;
    }
    stAdptDRCAttr.bEnable = g_stINIPara.stDRC.bDRCEnable;
    stAdptDRCAttr.bManulEnable = g_stINIPara.stDRC.bDRCManulEnable;
    stAdptDRCAttr.u32Strength = g_stINIPara.stDRC.u32Strength;
    stAdptDRCAttr.u32SlopeMax = g_stINIPara.stDRC.u32SlopeMax;
    stAdptDRCAttr.u32SlopeMin = g_stINIPara.stDRC.u32SlopeMin;
    stAdptDRCAttr.u32VarianceSpace = g_stINIPara.stDRC.u32VarianceSpace;
    stAdptDRCAttr.u32VarianceIntensity = g_stINIPara.stDRC.u32VarianceIntensity;
    stAdptDRCAttr.u32WhiteLevel = g_stINIPara.stDRC.u32WhiteLevel;
    stAdptDRCAttr.u32BlackLevel = g_stINIPara.stDRC.u32BlackLevel;
    s32Ret = CommSceneautoSetDRCAttr(s32IspDev, &stAdptDRCAttr);
    if (HI_SUCCESS != s32Ret)
    {
        printf("CommSceneautoSetDRCAttr failed\n");
        return HI_FAILURE;
    }
    return HI_SUCCESS;
}
HI_S32 SceneAuto_SetDynamic()
{
    HI_S32 s32Ret = HI_SUCCESS;
    HI_S32 i;
    HI_S32 s32IspDev;
    ADPT_SCENEAUTO_AEROUTE_S stAdptAERoute;

    s32IspDev = g_stINIPara.stMpInfo.s32IspDev;

    stAdptAERoute.u32TotalNum = g_stINIPara.stFastDynamic.s32TotalNum;
    for (i = 0; i < g_stINIPara.stFastDynamic.s32TotalNum; i++)
    {
        stAdptAERoute.astRouteNode[i].u32IntTime = g_stINIPara.stFastDynamic.pstRouteNode[i].u32IntTime;
        stAdptAERoute.astRouteNode[i].u32SysGain = g_stINIPara.stFastDynamic.pstRouteNode[i].u32SysGain;
    }

    s32Ret = CommSceneautoSetAERoute(s32IspDev, &stAdptAERoute);
    if (HI_SUCCESS != s32Ret)
    {
        printf("CommSceneautoSetAERoute failed\n");
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

HI_S32 SceneAuto_SetHLC()
{
    HI_S32 s32Ret = HI_SUCCESS;
    HI_S32 i;
    HI_S32 s32IspDev;
    HI_S32 s32ViDev;
    ADPT_SCENEAUTO_AEATTR_S stAdptAEAttr;
    ADPT_SCENEAUTO_DCIPARAM_S stAdptDCIParam;
    ADPT_SCENEAUTO_DRCATTR_S stAdptDRCAttr;
    ADPT_SCENEAUTO_SATURATION_S stAdptSaturation;
    ADPT_SCENEAUTO_GAMMA_S stAdptGamma;
    ADPT_SCENEAUTO_SHARPEN_S stAdptSharpen;

    s32IspDev = g_stINIPara.stMpInfo.s32IspDev;
    s32ViDev = g_stINIPara.stMpInfo.s32ViDev;

    //AE
    s32Ret = CommSceneautoGetAEAttr(s32IspDev, &stAdptAEAttr);
    if (HI_SUCCESS != s32Ret)
    {
        printf("CommSceneautoGetAEAttr failed\n");
        return HI_FAILURE;
    }
    stAdptAEAttr.u8ExpCompensation = g_stINIPara.stHLC.u8ExpCompensation;
    stAdptAEAttr.u16BlackDelayFrame = g_stINIPara.stHLC.u16BlackDelayFrame;
    stAdptAEAttr.u16WhiteDelayFrame = g_stINIPara.stHLC.u16WhiteDelayFrame;
    stAdptAEAttr.u8Speed = g_stINIPara.stHLC.u8Speed;
    stAdptAEAttr.u8Tolerance = g_stINIPara.stHLC.u8Tolerance;
    stAdptAEAttr.u16HistRatioSlope = g_stINIPara.stHLC.u16HistRatioSlope;
    stAdptAEAttr.u8MaxHistOffset = g_stINIPara.stHLC.u8MaxHistOffset;
    s32Ret = CommSceneautoSetAEAttr(s32IspDev, &stAdptAEAttr);
    if (HI_SUCCESS != s32Ret)
    {
        printf("CommSceneautoSetAEAttr failed\n");
        return HI_FAILURE;
    }

    //DCI
    s32Ret = CommSceneautoGetDCIParam(s32ViDev, &stAdptDCIParam);
    if (HI_SUCCESS != s32Ret)
    {
        printf("CommSceneautoGetDCIParam failed\n");
        return HI_FAILURE;
    }
    stAdptDCIParam.bEnable = g_stINIPara.stHLC.bDCIEnable;
    stAdptDCIParam.u32BlackGain = g_stINIPara.stHLC.u32DCIBlackGain;
    stAdptDCIParam.u32ContrastGain = g_stINIPara.stHLC.u32DCIContrastGain;
    stAdptDCIParam.u32LightGain = g_stINIPara.stHLC.u32DCILightGain;
    s32Ret = CommSceneautoSetDCIParam(s32ViDev, &stAdptDCIParam);
    if (HI_SUCCESS != s32Ret)
    {
        printf("CommSceneautoSetDCIParam failed\n");
        return HI_FAILURE;
    }

    //DRC
    s32Ret = CommSceneautoGetDRCAttr(s32IspDev, &stAdptDRCAttr);
    if (HI_SUCCESS != s32Ret)
    {
        printf("CommSceneautoGetDRCAttr failed\n");
        return HI_FAILURE;
    }
    stAdptDRCAttr.bEnable = g_stINIPara.stHLC.bDRCEnable;
    stAdptDRCAttr.bManulEnable = g_stINIPara.stHLC.bDRCManulEnable;
    stAdptDRCAttr.u32Strength = g_stINIPara.stHLC.u32DRCStrengthTarget;
    s32Ret = CommSceneautoSetDRCAttr(s32IspDev, &stAdptDRCAttr);
    if (HI_SUCCESS != s32Ret)
    {
        printf("CommSceneautoSetDRCAttr failed\n");
        return HI_FAILURE;
    }

    //SATURATION
    s32Ret = CommSceneautoGetSaturation(s32IspDev, &stAdptSaturation);
    if (HI_SUCCESS != s32Ret)
    {
        printf("CommSceneautoGetSaturation failed\n");
        return HI_FAILURE;
    }
    stAdptSaturation.u8OpType = 0;
    for (i = 0; i < 16; i++)
    {
        stAdptSaturation.au8AutoSat[i] = g_stINIPara.stHLC.u8Saturation[i];
    }
    s32Ret = CommSceneautoSetSaturation(s32IspDev, &stAdptSaturation);
    if (HI_SUCCESS != s32Ret)
    {
        printf("CommSceneautoSetSaturation failed\n");
        return HI_FAILURE;
    }

    //GAMMA
    for (i = 0; i < 257; i++)
    {
        stAdptGamma.au16GammaTable[i] = g_stINIPara.stHLC.u16GammaTable[i];
    }
    stAdptGamma.u8CurveType = 2;
    s32Ret = CommSceneautoSetGamma(s32IspDev, &stAdptGamma);
    if (HI_SUCCESS != s32Ret)
    {
        printf("CommSceneautoSetGamma failed\n");
        return HI_FAILURE;
    }

    //sharpen
    s32Ret = CommSceneautoGetSharpen(s32IspDev, &stAdptSharpen);
    if (HI_SUCCESS != s32Ret)
    {
        printf("CommSceneautoGetSharpen failed\n");
        return HI_FAILURE;
    }
    for (i = 0; i < 16; i++)
    {        
        stAdptSharpen.au8SharpenRGB[i] = g_stINIPara.stHLC.au8SharpenRGB[i];
        stAdptSharpen.au8SharpenUd[i] = g_stINIPara.stHLC.au8SharpenUd[i];
    }
    s32Ret = CommSceneautoSetSharpen(s32IspDev, &stAdptSharpen);
    if (HI_SUCCESS != s32Ret)
    {
        printf("CommSceneautoSetSharpen failed\n");
        return HI_FAILURE;
    }
    return HI_SUCCESS;
}

HI_S32 SceneAuto_SetIR()
{
    HI_S32 s32Ret = HI_SUCCESS;
    HI_S32 i,j;
    HI_S32 s32IspDev;
    HI_S32 s32ViDev;
    ADPT_SCENEAUTO_AEATTR_S stAdptAEAttr;
    ADPT_SCENEAUTO_DCIPARAM_S stAdptDCIParam;
    ADPT_SCENEAUTO_SATURATION_S stSaturation;
    ADPT_SCENEAUTO_DEMOSAIC_S stAdptDemosaic;
    ADPT_SCENEAUTO_SHARPEN_S stAdptSharpen;
    ADPT_SCENEAUTO_GAMMA_S stAdptGamma;
    ADPT_SCENEAUTO_DP_S stAdptDP;

    s32IspDev = g_stINIPara.stMpInfo.s32IspDev;
    s32ViDev = g_stINIPara.stMpInfo.s32ViDev;


    s32Ret = CommSceneautoGetAEAttr(s32IspDev, &stAdptAEAttr);
    if (HI_SUCCESS != s32Ret)
    {
        printf("CommSceneautoGetAEAttr failed\n");
        return HI_FAILURE;
    }
    //stAdptAEAttr.u8ExpCompensation = g_stINIPara.stIR.u8ExpCompensation;
    stAdptAEAttr.u16HistRatioSlope = g_stINIPara.stIR.u16HistRatioSlope;
    stAdptAEAttr.u16BlackDelayFrame = g_stINIPara.stIR.u16BlackDelayFrame;
    stAdptAEAttr.u16WhiteDelayFrame = g_stINIPara.stIR.u16WhiteDelayFrame;
    stAdptAEAttr.u8Speed = g_stINIPara.stIR.u8Speed;
    //stAdptAEAttr.u8MaxHistOffset = g_stINIPara.stIR.u8MaxHistOffset;
    stAdptAEAttr.u8Tolerance = g_stINIPara.stIR.u8Tolerance;
    for (i = 0; i < AE_WEIGHT_ROW; i++)
    {
        for (j = 0; j < AE_WEIGHT_COLUMN; j++)
        {
            stAdptAEAttr.au8AeWeight[i][j] = g_stINIPara.stIR.au8Weight[i][j];
        }
    }
    s32Ret = CommSceneautoSetAEAttr(s32IspDev, &stAdptAEAttr);
    if (HI_SUCCESS != s32Ret)
    {
        printf("CommSceneautoSetAEAttr failed\n");
        return HI_FAILURE;
    }

    s32Ret = CommSceneautoGetDCIParam(s32ViDev, &stAdptDCIParam);
    if (HI_SUCCESS != s32Ret)
    {
        printf("CommSceneautoGetDCIParam failed\n");
        return HI_FAILURE;
    }
    stAdptDCIParam.bEnable = g_stINIPara.stIR.bDCIEnable;
    stAdptDCIParam.u32BlackGain = g_stINIPara.stIR.u32DCIBlackGain;
    stAdptDCIParam.u32ContrastGain = g_stINIPara.stIR.u32DCIContrastGain;
    stAdptDCIParam.u32LightGain = g_stINIPara.stIR.u32DCILightGain;
    s32Ret = CommSceneautoSetDCIParam(s32ViDev, &stAdptDCIParam);
    if (HI_SUCCESS != s32Ret)
    {
        printf("CommSceneautoSetDCIParam failed\n");
        return HI_FAILURE;
    }
    s32Ret = CommSceneautoGetDP(s32ViDev, &stAdptDP);
    if (HI_SUCCESS != s32Ret)
    {
        printf("CommSceneautoGetDP failed\n");
        return HI_FAILURE;
    }
    stAdptDP.u16Slope = g_stINIPara.stIR.u16Slope;
    s32Ret = CommSceneautoSetDP(s32ViDev, &stAdptDP);
    if (HI_SUCCESS != s32Ret)
    {
        printf("CommSceneautoSetDP failed\n");
        return HI_FAILURE;
    }

    s32Ret = CommSceneautoGetSaturation(s32IspDev, &stSaturation);
    if (HI_SUCCESS != s32Ret)
    {
        printf("CommSceneautoGetSaturation failed\n");
        return HI_FAILURE;
    }
    stSaturation.u8OpType = 1;
    stSaturation.u8ManualSat = 0;
    s32Ret = CommSceneautoSetSaturation(s32IspDev, &stSaturation);
    if (HI_SUCCESS != s32Ret)
    {
        printf("CommSceneautoSetSaturation failed\n");
        return HI_FAILURE;
    }

    s32Ret = CommSceneautoGetDemosaic(s32IspDev, &stAdptDemosaic);
    if (HI_SUCCESS != s32Ret)
    {
        printf("CommSceneautoGetDemosaic failed\n");
        return HI_FAILURE;
    }
    for (i = 0; i < 16; i++)
    {
        stAdptDemosaic.au8LumThresh[i] = g_stINIPara.stIR.au8LumThresh[i];
    }
    s32Ret = CommSceneautoSetDemosaic(s32IspDev, &stAdptDemosaic);
    if (HI_SUCCESS != s32Ret)
    {
        printf("CommSceneautoSetDemosaic failed\n");
        return HI_FAILURE;
    }

    s32Ret = CommSceneautoGetSharpen(s32IspDev, &stAdptSharpen);
    if (HI_SUCCESS != s32Ret)
    {
        printf("CommSceneautoGetSharpen failed\n");
        return HI_FAILURE;
    }
    for (i = 0; i < 16; i++)
    {
        stAdptSharpen.au8SharpenD[i] = g_stINIPara.stIR.au8SharpenD[i];
        stAdptSharpen.au8SharpenRGB[i] = g_stINIPara.stIR.au8SharpenRGB[i];
        stAdptSharpen.au8SharpenUd[i] = g_stINIPara.stIR.au8SharpenUd[i];
    }
    s32Ret = CommSceneautoSetSharpen(s32IspDev, &stAdptSharpen);
    if (HI_SUCCESS != s32Ret)
    {
        printf("CommSceneautoSetSharpen failed\n");
        return HI_FAILURE;
    }

    for (i = 0; i < 257; i++)
    {
        stAdptGamma.au16GammaTable[i] = g_stINIPara.stIR.u16GammaTable[i];
    }
    stAdptGamma.u8CurveType = 2;
    s32Ret = CommSceneautoSetGamma(s32IspDev, &stAdptGamma);
    if (HI_SUCCESS != s32Ret)
    {
        printf("CommSceneautoSetGamma failed\n");
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

HI_S32 SceneAuto_SetBLC()
{
    HI_S32 s32Ret = HI_SUCCESS;
    HI_S32 s32IspDev;
    ADPT_SCENEAUTO_AEATTR_S stAdptAEAttr;

    s32IspDev = g_stINIPara.stMpInfo.s32IspDev;

    s32Ret = CommSceneautoGetAEAttr(s32IspDev, &stAdptAEAttr);
    if (HI_SUCCESS != s32Ret)
    {
        printf("CommSceneautoGetAEAttr failed\n");
        return HI_FAILURE;
    }

    stAdptAEAttr.u8AEStrategyMode = g_stINIPara.stBLC.u8AEStrategyMode;
    stAdptAEAttr.u16HistRatioSlope = g_stINIPara.stBLC.u16HistRatioSlope;
    stAdptAEAttr.u8MaxHistOffset = g_stINIPara.stBLC.u8MaxHistOffset;

    s32Ret = CommSceneautoSetAEAttr(s32IspDev, &stAdptAEAttr);
    if (HI_SUCCESS != s32Ret)
    {
        printf("CommSceneautoSetAEAttr failed\n");
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

HI_S32 SceneAuto_HLCAutoOn()
{
    HI_S32 s32Ret = HI_SUCCESS;
    HI_S32 i;
    HI_S32 s32IspDev;
    ADPT_SCENEAUTO_DRCATTR_S stAdptDRCAttr;
    s32IspDev = g_stINIPara.stMpInfo.s32IspDev;

    s32Ret = CommSceneautoGetDRCAttr(s32IspDev, &stAdptDRCAttr);
    if (HI_SUCCESS != s32Ret)
    {
        printf("CommSceneautoGetDRCAttr failed\n");
        return HI_FAILURE;
    }
    stAdptDRCAttr.bEnable = HI_FALSE;
    stAdptDRCAttr.bManulEnable = HI_TRUE;
    stAdptDRCAttr.u32Strength = 0;
    s32Ret = CommSceneautoSetDRCAttr(s32IspDev, &stAdptDRCAttr);
    if (HI_SUCCESS != s32Ret)
    {
        printf("CommSceneautoSetDRCAttr failed\n");
        return HI_FAILURE;
    }

    usleep(100000);
    s32Ret = CommSceneautoGetDRCAttr(s32IspDev, &stAdptDRCAttr);
    if (HI_SUCCESS != s32Ret)
    {
        printf("CommSceneautoGetDRCAttr failed\n");
        return HI_FAILURE;
    }
    stAdptDRCAttr.bEnable = HI_TRUE;
    stAdptDRCAttr.bManulEnable = HI_TRUE;
    stAdptDRCAttr.u32Strength = 0;
    s32Ret = CommSceneautoSetDRCAttr(s32IspDev, &stAdptDRCAttr);
    if (HI_SUCCESS != s32Ret)
    {
        printf("CommSceneautoSetDRCAttr failed\n");
        return HI_FAILURE;
    }

    s32Ret = CommSceneautoGetDRCAttr(s32IspDev, &stAdptDRCAttr);
    if (HI_SUCCESS != s32Ret)
    {
        printf("CommSceneautoGetDRCAttr failed\n");
        return HI_FAILURE;
    }
    stAdptDRCAttr.bEnable = HI_TRUE;
    stAdptDRCAttr.bManulEnable = HI_TRUE;
    stAdptDRCAttr.u32Strength = 0;
    s32Ret = CommSceneautoSetDRCAttr(s32IspDev, &stAdptDRCAttr);
    if (HI_SUCCESS != s32Ret)
    {
        printf("CommSceneautoSetDRCAttr failed\n");
        return HI_FAILURE;
    }


    for (i = 1; i < 128; i++)
    {
        s32Ret = CommSceneautoGetDRCAttr(s32IspDev, &stAdptDRCAttr);
        if (HI_SUCCESS != s32Ret)
        {
            printf("CommSceneautoGetDRCAttr failed\n");
            return HI_FAILURE;
        }
        stAdptDRCAttr.bEnable = HI_TRUE;
        stAdptDRCAttr.bManulEnable = HI_TRUE;
        stAdptDRCAttr.u32Strength = i;
        s32Ret = CommSceneautoSetDRCAttr(s32IspDev, &stAdptDRCAttr);
        if (HI_SUCCESS != s32Ret)
        {
            printf("CommSceneautoSetDRCAttr failed\n");
            return HI_FAILURE;
        }
        usleep(5000);
    }

    return HI_SUCCESS;
}

HI_S32 SceneAuto_HLCAutoOff()
{
    HI_S32 s32Ret = HI_SUCCESS;
    HI_S32 i;
    HI_S32 s32IspDev;
    ADPT_SCENEAUTO_DRCATTR_S stAdptDRCAttr;
    s32IspDev = g_stINIPara.stMpInfo.s32IspDev;

    s32Ret = CommSceneautoGetDRCAttr(s32IspDev, &stAdptDRCAttr);
    if (HI_SUCCESS != s32Ret)
    {
        printf("CommSceneautoGetDRCAttr failed\n");
        return HI_FAILURE;
    }

    for (i = stAdptDRCAttr.u32Strength; i >= 0; i--)
    {
        stAdptDRCAttr.u32Strength = i;
        s32Ret = CommSceneautoSetDRCAttr(s32IspDev, &stAdptDRCAttr);
        if (HI_SUCCESS != s32Ret)
        {
            printf("CommSceneautoSetDRCAttr failed\n");
            return HI_FAILURE;
        }
        usleep(5000);
    }

    stAdptDRCAttr.bEnable = HI_FALSE;
    stAdptDRCAttr.bManulEnable = HI_FALSE;
    stAdptDRCAttr.u32Strength = 0;
    s32Ret = CommSceneautoSetDRCAttr(s32IspDev, &stAdptDRCAttr);
    if (HI_SUCCESS != s32Ret)
    {
        printf("CommSceneautoSetDRCAttr failed\n");
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

HI_S32 SceneAuto_SetDemosaic(HI_S32 s32IspDev, HI_S32 s32DemosaicExpLevel, HI_S32 s32DemosaicBitrateLevel)
{
    HI_S32 s32Ret = HI_SUCCESS;
    ADPT_SCENEAUTO_DEMOSAIC_S stAdptDemosaic;

    s32Ret = CommSceneautoGetDemosaic(s32IspDev, &stAdptDemosaic);
    if (HI_SUCCESS != s32Ret)
    {
        printf("CommSceneautoGetDemosaic failed\n");
        return HI_FAILURE;
    }

    stAdptDemosaic.u8UuSlope = g_stINIPara.stIniDemosaic.pstDemosaic[s32DemosaicBitrateLevel * g_stINIPara.stIniDemosaic.s32ExpCount + s32DemosaicExpLevel].u8UuSlope;
    stAdptDemosaic.u8VaSlope = g_stINIPara.stIniDemosaic.pstDemosaic[s32DemosaicBitrateLevel * g_stINIPara.stIniDemosaic.s32ExpCount + s32DemosaicExpLevel].u8VaSlope;
    stAdptDemosaic.u8AaSlope = g_stINIPara.stIniDemosaic.pstDemosaic[s32DemosaicBitrateLevel * g_stINIPara.stIniDemosaic.s32ExpCount + s32DemosaicExpLevel].u8AaSlope;
    stAdptDemosaic.u8VhSlope = g_stINIPara.stIniDemosaic.pstDemosaic[s32DemosaicBitrateLevel * g_stINIPara.stIniDemosaic.s32ExpCount + s32DemosaicExpLevel].u8VhSlope;

    s32Ret = CommSceneautoSetDemosaic(s32IspDev, &stAdptDemosaic);
    if (HI_SUCCESS != s32Ret)
    {
        printf("CommSceneautoSetDemosaic failed\n");
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

HI_S32 SceneAuto_SetAERealtedBit(HI_S32 s32IspDev, HI_S32 s32AEBitrateLevel)
{
    HI_S32 s32Ret = HI_SUCCESS;
    ADPT_SCENEAUTO_AEATTR_S stAdptAEAttr;

    if ((SRDK_SCENEAUTO_SPECIAL_SCENE_IR != g_eSpecialScene) && (SRDK_SCENEAUTO_SPECIAL_SCENE_BLC != g_eSpecialScene) && (SRDK_SCENEAUTO_SPECIAL_SCENE_HLC != g_eSpecialScene))
    {
        s32Ret = CommSceneautoGetAEAttr(s32IspDev, &stAdptAEAttr);
        if (HI_SUCCESS != s32Ret)
        {
            printf("CommSceneautoGetAEAttr failed\n");
            return HI_FAILURE;
        }

        stAdptAEAttr.u8Speed = g_stINIPara.stIniAE.pstAERelatedBit[s32AEBitrateLevel].u8Speed;
        stAdptAEAttr.u8Tolerance = g_stINIPara.stIniAE.pstAERelatedBit[s32AEBitrateLevel].u8Tolerance;
        stAdptAEAttr.u16BlackDelayFrame = g_stINIPara.stIniAE.pstAERelatedBit[s32AEBitrateLevel].u16BlackDelayFrame;
        stAdptAEAttr.u16WhiteDelayFrame = g_stINIPara.stIniAE.pstAERelatedBit[s32AEBitrateLevel].u16WhiteDelayFrame;
        stAdptAEAttr.u32SysGainMax = g_stINIPara.stIniAE.pstAERelatedBit[s32AEBitrateLevel].u32SysGainMax;

        s32Ret = CommSceneautoSetAEAttr(s32IspDev, &stAdptAEAttr);
        if (HI_SUCCESS != s32Ret)
        {
            printf("CommSceneautoSetAEAttr failed\n");
            return HI_FAILURE;
        }
    }

    return HI_SUCCESS;
}

HI_S32 SceneAuto_SetSharpen(HI_S32 s32IspDev, HI_S32 s32SharpenBitrateLevel)
{
    HI_S32 s32Ret = HI_SUCCESS;
    HI_S32 i;
    ADPT_SCENEAUTO_SHARPEN_S stAdptSharpen;

    if ((SRDK_SCENEAUTO_SPECIAL_SCENE_IR != g_eSpecialScene)&&(SRDK_SCENEAUTO_SPECIAL_SCENE_HLC != g_eSpecialScene))
    {
        s32Ret = CommSceneautoGetSharpen(s32IspDev, &stAdptSharpen);
        if (HI_SUCCESS != s32Ret)
        {
            printf("CommSceneautoGetSharpen failed\n");
            return HI_FAILURE;
        }

        for (i = 0; i < EXPOSURE_LEVEL; i++)
        {
            stAdptSharpen.au8SharpenD[i] = g_stINIPara.stIniSharpen.pstSharpen[s32SharpenBitrateLevel].au8SharpenD[i];
            stAdptSharpen.au8SharpenRGB[i] = g_stINIPara.stIniSharpen.pstSharpen[s32SharpenBitrateLevel].au8SharpenRGB[i];
            stAdptSharpen.au8SharpenUd[i] = g_stINIPara.stIniSharpen.pstSharpen[s32SharpenBitrateLevel].au8SharpenUd[i];
        }

        s32Ret = CommSceneautoSetSharpen(s32IspDev, &stAdptSharpen);
        if (HI_SUCCESS != s32Ret)
        {
            printf("CommSceneautoSetSharpen failed\n");
            return HI_FAILURE;
        }
    }

    return HI_SUCCESS;
}

HI_S32 SceneAuto_SetH265FaceCfg(HI_S32 s32VencChn, HI_S32 s32H265FaceCfgExpLevel, HI_S32 s32H265FaceCfgBitrateLevel)
{
    HI_S32 s32Ret = HI_SUCCESS;
    ADPT_SCENEAUTO_H265_FACE_CFG_S stAdptH265FaceCfg;
    s32Ret = CommSceneautoGetH265FaceCfg(s32VencChn, &stAdptH265FaceCfg);
    if (HI_SUCCESS != s32Ret)
    {
        printf("CommSceneautoGetH265FaceCfg failed\n");
        return HI_FAILURE;
    }
    stAdptH265FaceCfg.u8NormIntra4RdCost_I  = g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.pstH265VencFaceCfg[s32H265FaceCfgBitrateLevel * g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.s32ExpCount + s32H265FaceCfgExpLevel].u8NormIntra4RdCost_I;
    stAdptH265FaceCfg.u8NormIntra8RdCost_I  = g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.pstH265VencFaceCfg[s32H265FaceCfgBitrateLevel * g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.s32ExpCount + s32H265FaceCfgExpLevel].u8NormIntra8RdCost_I;
    stAdptH265FaceCfg.u8NormIntra16RdCost_I = g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.pstH265VencFaceCfg[s32H265FaceCfgBitrateLevel * g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.s32ExpCount + s32H265FaceCfgExpLevel].u8NormIntra16RdCost_I;
    stAdptH265FaceCfg.u8NormIntra32RdCost_I = g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.pstH265VencFaceCfg[s32H265FaceCfgBitrateLevel * g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.s32ExpCount + s32H265FaceCfgExpLevel].u8NormIntra32RdCost_I;
    stAdptH265FaceCfg.u8SkinIntra4RdCost_I  = g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.pstH265VencFaceCfg[s32H265FaceCfgBitrateLevel * g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.s32ExpCount + s32H265FaceCfgExpLevel].u8SkinIntra4RdCost_I;
    stAdptH265FaceCfg.u8SkinIntra8RdCost_I  = g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.pstH265VencFaceCfg[s32H265FaceCfgBitrateLevel * g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.s32ExpCount + s32H265FaceCfgExpLevel].u8SkinIntra8RdCost_I;
    stAdptH265FaceCfg.u8SkinIntra16RdCost_I = g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.pstH265VencFaceCfg[s32H265FaceCfgBitrateLevel * g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.s32ExpCount + s32H265FaceCfgExpLevel].u8SkinIntra16RdCost_I;
    stAdptH265FaceCfg.u8SkinIntra32RdCost_I = g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.pstH265VencFaceCfg[s32H265FaceCfgBitrateLevel * g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.s32ExpCount + s32H265FaceCfgExpLevel].u8SkinIntra32RdCost_I;
    stAdptH265FaceCfg.u8HedgeIntra4RdCost_I = g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.pstH265VencFaceCfg[s32H265FaceCfgBitrateLevel * g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.s32ExpCount + s32H265FaceCfgExpLevel].u8HedgeIntra4RdCost_I;
    stAdptH265FaceCfg.u8HedgeIntra8RdCost_I = g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.pstH265VencFaceCfg[s32H265FaceCfgBitrateLevel * g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.s32ExpCount + s32H265FaceCfgExpLevel].u8HedgeIntra8RdCost_I;
    stAdptH265FaceCfg.u8HedgeIntra16RdCost_I = g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.pstH265VencFaceCfg[s32H265FaceCfgBitrateLevel * g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.s32ExpCount + s32H265FaceCfgExpLevel].u8HedgeIntra16RdCost_I;
    stAdptH265FaceCfg.u8HedgeIntra32RdCost_I = g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.pstH265VencFaceCfg[s32H265FaceCfgBitrateLevel * g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.s32ExpCount + s32H265FaceCfgExpLevel].u8HedgeIntra32RdCost_I;
    stAdptH265FaceCfg.u8NormIntra4RdCost_P  = g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.pstH265VencFaceCfg[s32H265FaceCfgBitrateLevel * g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.s32ExpCount + s32H265FaceCfgExpLevel].u8NormIntra4RdCost_P;
    stAdptH265FaceCfg.u8NormIntra8RdCost_P  = g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.pstH265VencFaceCfg[s32H265FaceCfgBitrateLevel * g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.s32ExpCount + s32H265FaceCfgExpLevel].u8NormIntra8RdCost_P;
    stAdptH265FaceCfg.u8NormIntra16RdCost_P = g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.pstH265VencFaceCfg[s32H265FaceCfgBitrateLevel * g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.s32ExpCount + s32H265FaceCfgExpLevel].u8NormIntra16RdCost_P;
    stAdptH265FaceCfg.u8NormIntra32RdCost_P = g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.pstH265VencFaceCfg[s32H265FaceCfgBitrateLevel * g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.s32ExpCount + s32H265FaceCfgExpLevel].u8NormIntra32RdCost_P;
    stAdptH265FaceCfg.u8SkinIntra4RdCost_P  = g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.pstH265VencFaceCfg[s32H265FaceCfgBitrateLevel * g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.s32ExpCount + s32H265FaceCfgExpLevel].u8SkinIntra4RdCost_P;
    stAdptH265FaceCfg.u8SkinIntra8RdCost_P  = g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.pstH265VencFaceCfg[s32H265FaceCfgBitrateLevel * g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.s32ExpCount + s32H265FaceCfgExpLevel].u8SkinIntra8RdCost_P;
    stAdptH265FaceCfg.u8SkinIntra16RdCost_P = g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.pstH265VencFaceCfg[s32H265FaceCfgBitrateLevel * g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.s32ExpCount + s32H265FaceCfgExpLevel].u8SkinIntra16RdCost_P;
    stAdptH265FaceCfg.u8SkinIntra32RdCost_P = g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.pstH265VencFaceCfg[s32H265FaceCfgBitrateLevel * g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.s32ExpCount + s32H265FaceCfgExpLevel].u8SkinIntra32RdCost_P;
    stAdptH265FaceCfg.u8HedgeIntra4RdCost_P = g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.pstH265VencFaceCfg[s32H265FaceCfgBitrateLevel * g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.s32ExpCount + s32H265FaceCfgExpLevel].u8HedgeIntra4RdCost_P;
    stAdptH265FaceCfg.u8HedgeIntra8RdCost_P = g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.pstH265VencFaceCfg[s32H265FaceCfgBitrateLevel * g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.s32ExpCount + s32H265FaceCfgExpLevel].u8HedgeIntra8RdCost_P;
    stAdptH265FaceCfg.u8HedgeIntra16RdCost_P = g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.pstH265VencFaceCfg[s32H265FaceCfgBitrateLevel * g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.s32ExpCount + s32H265FaceCfgExpLevel].u8HedgeIntra16RdCost_P;
    stAdptH265FaceCfg.u8HedgeIntra32RdCost_P = g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.pstH265VencFaceCfg[s32H265FaceCfgBitrateLevel * g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.s32ExpCount + s32H265FaceCfgExpLevel].u8HedgeIntra32RdCost_P;
    stAdptH265FaceCfg.u8NormFme8RdCost_P    = g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.pstH265VencFaceCfg[s32H265FaceCfgBitrateLevel * g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.s32ExpCount + s32H265FaceCfgExpLevel].u8NormFme8RdCost_P;
    stAdptH265FaceCfg.u8NormFme16RdCost_P   = g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.pstH265VencFaceCfg[s32H265FaceCfgBitrateLevel * g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.s32ExpCount + s32H265FaceCfgExpLevel].u8NormFme16RdCost_P;
    stAdptH265FaceCfg.u8NormFme32RdCost_P   = g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.pstH265VencFaceCfg[s32H265FaceCfgBitrateLevel * g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.s32ExpCount + s32H265FaceCfgExpLevel].u8NormFme32RdCost_P;
    stAdptH265FaceCfg.u8NormFme64RdCost_P   = g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.pstH265VencFaceCfg[s32H265FaceCfgBitrateLevel * g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.s32ExpCount + s32H265FaceCfgExpLevel].u8NormFme64RdCost_P;
    stAdptH265FaceCfg.u8SkinFme8RdCost_P    = g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.pstH265VencFaceCfg[s32H265FaceCfgBitrateLevel * g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.s32ExpCount + s32H265FaceCfgExpLevel].u8SkinFme8RdCost_P;
    stAdptH265FaceCfg.u8SkinFme16RdCost_P   = g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.pstH265VencFaceCfg[s32H265FaceCfgBitrateLevel * g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.s32ExpCount + s32H265FaceCfgExpLevel].u8SkinFme16RdCost_P;
    stAdptH265FaceCfg.u8SkinFme32RdCost_P   = g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.pstH265VencFaceCfg[s32H265FaceCfgBitrateLevel * g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.s32ExpCount + s32H265FaceCfgExpLevel].u8SkinFme32RdCost_P;
    stAdptH265FaceCfg.u8SkinFme64RdCost_P   = g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.pstH265VencFaceCfg[s32H265FaceCfgBitrateLevel * g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.s32ExpCount + s32H265FaceCfgExpLevel].u8SkinFme64RdCost_P;
    stAdptH265FaceCfg.u8HedgeFme8RdCost_P   = g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.pstH265VencFaceCfg[s32H265FaceCfgBitrateLevel * g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.s32ExpCount + s32H265FaceCfgExpLevel].u8HedgeFme8RdCost_P;
    stAdptH265FaceCfg.u8HedgeFme16RdCost_P  = g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.pstH265VencFaceCfg[s32H265FaceCfgBitrateLevel * g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.s32ExpCount + s32H265FaceCfgExpLevel].u8HedgeFme16RdCost_P;
    stAdptH265FaceCfg.u8HedgeFme32RdCost_P  = g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.pstH265VencFaceCfg[s32H265FaceCfgBitrateLevel * g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.s32ExpCount + s32H265FaceCfgExpLevel].u8HedgeFme32RdCost_P;
    stAdptH265FaceCfg.u8HedgeFme64RdCost_P  = g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.pstH265VencFaceCfg[s32H265FaceCfgBitrateLevel * g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.s32ExpCount + s32H265FaceCfgExpLevel].u8HedgeFme64RdCost_P;
    stAdptH265FaceCfg.u8NormMerg8RdCost_P   = g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.pstH265VencFaceCfg[s32H265FaceCfgBitrateLevel * g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.s32ExpCount + s32H265FaceCfgExpLevel].u8NormMerg8RdCost_P;
    stAdptH265FaceCfg.u8NormMerg16RdCost_P  = g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.pstH265VencFaceCfg[s32H265FaceCfgBitrateLevel * g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.s32ExpCount + s32H265FaceCfgExpLevel].u8NormMerg16RdCost_P;
    stAdptH265FaceCfg.u8NormMerg32RdCost_P  = g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.pstH265VencFaceCfg[s32H265FaceCfgBitrateLevel * g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.s32ExpCount + s32H265FaceCfgExpLevel].u8NormMerg32RdCost_P;
    stAdptH265FaceCfg.u8NormMerg64RdCost_P  = g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.pstH265VencFaceCfg[s32H265FaceCfgBitrateLevel * g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.s32ExpCount + s32H265FaceCfgExpLevel].u8NormMerg64RdCost_P;
    stAdptH265FaceCfg.u8SkinMerg8RdCost_P   = g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.pstH265VencFaceCfg[s32H265FaceCfgBitrateLevel * g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.s32ExpCount + s32H265FaceCfgExpLevel].u8SkinMerg8RdCost_P;
    stAdptH265FaceCfg.u8SkinMerg16RdCost_P  = g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.pstH265VencFaceCfg[s32H265FaceCfgBitrateLevel * g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.s32ExpCount + s32H265FaceCfgExpLevel].u8SkinMerg16RdCost_P;
    stAdptH265FaceCfg.u8SkinMerg32RdCost_P  = g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.pstH265VencFaceCfg[s32H265FaceCfgBitrateLevel * g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.s32ExpCount + s32H265FaceCfgExpLevel].u8SkinMerg32RdCost_P;
    stAdptH265FaceCfg.u8SkinMerg64RdCost_P  = g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.pstH265VencFaceCfg[s32H265FaceCfgBitrateLevel * g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.s32ExpCount + s32H265FaceCfgExpLevel].u8SkinMerg64RdCost_P;
    stAdptH265FaceCfg.u8HedgeMerg8RdCost_P  = g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.pstH265VencFaceCfg[s32H265FaceCfgBitrateLevel * g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.s32ExpCount + s32H265FaceCfgExpLevel].u8HedgeMerg8RdCost_P;
    stAdptH265FaceCfg.u8HedgeMerg16RdCost_P = g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.pstH265VencFaceCfg[s32H265FaceCfgBitrateLevel * g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.s32ExpCount + s32H265FaceCfgExpLevel].u8HedgeMerg16RdCost_P;
    stAdptH265FaceCfg.u8HedgeMerg32RdCost_P = g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.pstH265VencFaceCfg[s32H265FaceCfgBitrateLevel * g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.s32ExpCount + s32H265FaceCfgExpLevel].u8HedgeMerg32RdCost_P;
    stAdptH265FaceCfg.u8HedgeMerg64RdCost_P = g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.pstH265VencFaceCfg[s32H265FaceCfgBitrateLevel * g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.s32ExpCount + s32H265FaceCfgExpLevel].u8HedgeMerg64RdCost_P;
    stAdptH265FaceCfg.bSkinEn_I             = g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.pstH265VencFaceCfg[s32H265FaceCfgBitrateLevel * g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.s32ExpCount + s32H265FaceCfgExpLevel].bSkinEn_I;
    stAdptH265FaceCfg.u32SkinQpDelta_I      = g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.pstH265VencFaceCfg[s32H265FaceCfgBitrateLevel * g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.s32ExpCount + s32H265FaceCfgExpLevel].u32SkinQpDelta_I;
    stAdptH265FaceCfg.u8SkinUMax_I          = g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.pstH265VencFaceCfg[s32H265FaceCfgBitrateLevel * g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.s32ExpCount + s32H265FaceCfgExpLevel].u8SkinUMax_I;
    stAdptH265FaceCfg.u8SkinUMin_I          = g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.pstH265VencFaceCfg[s32H265FaceCfgBitrateLevel * g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.s32ExpCount + s32H265FaceCfgExpLevel].u8SkinUMin_I;
    stAdptH265FaceCfg.u8SkinVMax_I          = g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.pstH265VencFaceCfg[s32H265FaceCfgBitrateLevel * g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.s32ExpCount + s32H265FaceCfgExpLevel].u8SkinVMax_I;
    stAdptH265FaceCfg.u8SkinVMin_I          = g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.pstH265VencFaceCfg[s32H265FaceCfgBitrateLevel * g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.s32ExpCount + s32H265FaceCfgExpLevel].u8SkinVMin_I;
    stAdptH265FaceCfg.u32SkinNum_I          = g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.pstH265VencFaceCfg[s32H265FaceCfgBitrateLevel * g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.s32ExpCount + s32H265FaceCfgExpLevel].u32SkinNum_I;
    stAdptH265FaceCfg.bSkinEn_P             = g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.pstH265VencFaceCfg[s32H265FaceCfgBitrateLevel * g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.s32ExpCount + s32H265FaceCfgExpLevel].bSkinEn_P;
    stAdptH265FaceCfg.u32SkinQpDelta_P      = g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.pstH265VencFaceCfg[s32H265FaceCfgBitrateLevel * g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.s32ExpCount + s32H265FaceCfgExpLevel].u32SkinQpDelta_P;
    stAdptH265FaceCfg.u8SkinUMax_P          = g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.pstH265VencFaceCfg[s32H265FaceCfgBitrateLevel * g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.s32ExpCount + s32H265FaceCfgExpLevel].u8SkinUMax_P;
    stAdptH265FaceCfg.u8SkinUMin_P          = g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.pstH265VencFaceCfg[s32H265FaceCfgBitrateLevel * g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.s32ExpCount + s32H265FaceCfgExpLevel].u8SkinUMin_P;
    stAdptH265FaceCfg.u8SkinVMax_P          = g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.pstH265VencFaceCfg[s32H265FaceCfgBitrateLevel * g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.s32ExpCount + s32H265FaceCfgExpLevel].u8SkinVMax_P;
    stAdptH265FaceCfg.u8SkinVMin_P          = g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.pstH265VencFaceCfg[s32H265FaceCfgBitrateLevel * g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.s32ExpCount + s32H265FaceCfgExpLevel].u8SkinVMin_P;
    stAdptH265FaceCfg.u32SkinNum_P          = g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.pstH265VencFaceCfg[s32H265FaceCfgBitrateLevel * g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.s32ExpCount + s32H265FaceCfgExpLevel].u32SkinNum_P;
    stAdptH265FaceCfg.u8HedgeThr_I          = g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.pstH265VencFaceCfg[s32H265FaceCfgBitrateLevel * g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.s32ExpCount + s32H265FaceCfgExpLevel].u8HedgeThr_I;
    stAdptH265FaceCfg.u8HedgeCnt_I          = g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.pstH265VencFaceCfg[s32H265FaceCfgBitrateLevel * g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.s32ExpCount + s32H265FaceCfgExpLevel].u8HedgeCnt_I;
    stAdptH265FaceCfg.bStroEdgeEn_I         = g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.pstH265VencFaceCfg[s32H265FaceCfgBitrateLevel * g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.s32ExpCount + s32H265FaceCfgExpLevel].bStroEdgeEn_I;
    stAdptH265FaceCfg.u32StroEdgeQpDelta_I  = g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.pstH265VencFaceCfg[s32H265FaceCfgBitrateLevel * g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.s32ExpCount + s32H265FaceCfgExpLevel].u32StroEdgeQpDelta_I;
    stAdptH265FaceCfg.u8HedgeThr_P          = g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.pstH265VencFaceCfg[s32H265FaceCfgBitrateLevel * g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.s32ExpCount + s32H265FaceCfgExpLevel].u8HedgeThr_P;
    stAdptH265FaceCfg.u8HedgeCnt_P          = g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.pstH265VencFaceCfg[s32H265FaceCfgBitrateLevel * g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.s32ExpCount + s32H265FaceCfgExpLevel].u8HedgeCnt_P;
    stAdptH265FaceCfg.bStroEdgeEn_P         = g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.pstH265VencFaceCfg[s32H265FaceCfgBitrateLevel * g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.s32ExpCount + s32H265FaceCfgExpLevel].bStroEdgeEn_P;
    stAdptH265FaceCfg.u32StroEdgeQpDelta_P  = g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.pstH265VencFaceCfg[s32H265FaceCfgBitrateLevel * g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.s32ExpCount + s32H265FaceCfgExpLevel].u32StroEdgeQpDelta_P;
    stAdptH265FaceCfg.bImproveEn_I          = g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.pstH265VencFaceCfg[s32H265FaceCfgBitrateLevel * g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.s32ExpCount + s32H265FaceCfgExpLevel].bImproveEn_I;
    stAdptH265FaceCfg.bImproveEn_P          = g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.pstH265VencFaceCfg[s32H265FaceCfgBitrateLevel * g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.s32ExpCount + s32H265FaceCfgExpLevel].bImproveEn_P;
    stAdptH265FaceCfg.u32Norm32MaxNum_P     = g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.pstH265VencFaceCfg[s32H265FaceCfgBitrateLevel * g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.s32ExpCount + s32H265FaceCfgExpLevel].u32Norm32MaxNum_P;
    stAdptH265FaceCfg.u32Norm16MaxNum_P     = g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.pstH265VencFaceCfg[s32H265FaceCfgBitrateLevel * g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.s32ExpCount + s32H265FaceCfgExpLevel].u32Norm16MaxNum_P;
    stAdptH265FaceCfg.u32Norm32ProtectNum_P = g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.pstH265VencFaceCfg[s32H265FaceCfgBitrateLevel * g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.s32ExpCount + s32H265FaceCfgExpLevel].u32Norm32ProtectNum_P;
    stAdptH265FaceCfg.u32Norm16ProtectNum_P = g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.pstH265VencFaceCfg[s32H265FaceCfgBitrateLevel * g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.s32ExpCount + s32H265FaceCfgExpLevel].u32Norm16ProtectNum_P;
    stAdptH265FaceCfg.u32Skin32MaxNum_P     = g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.pstH265VencFaceCfg[s32H265FaceCfgBitrateLevel * g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.s32ExpCount + s32H265FaceCfgExpLevel].u32Skin32MaxNum_P;
    stAdptH265FaceCfg.u32Skin16MaxNum_P     = g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.pstH265VencFaceCfg[s32H265FaceCfgBitrateLevel * g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.s32ExpCount + s32H265FaceCfgExpLevel].u32Skin16MaxNum_P;
    stAdptH265FaceCfg.u32Skin32ProtectNum_P = g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.pstH265VencFaceCfg[s32H265FaceCfgBitrateLevel * g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.s32ExpCount + s32H265FaceCfgExpLevel].u32Skin32ProtectNum_P;
    stAdptH265FaceCfg.u32Skin16ProtectNum_P = g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.pstH265VencFaceCfg[s32H265FaceCfgBitrateLevel * g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.s32ExpCount + s32H265FaceCfgExpLevel].u32Skin16ProtectNum_P;
    stAdptH265FaceCfg.u32Still32MaxNum_P    = g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.pstH265VencFaceCfg[s32H265FaceCfgBitrateLevel * g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.s32ExpCount + s32H265FaceCfgExpLevel].u32Still32MaxNum_P;
    stAdptH265FaceCfg.u32Still16MaxNum_P    = g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.pstH265VencFaceCfg[s32H265FaceCfgBitrateLevel * g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.s32ExpCount + s32H265FaceCfgExpLevel].u32Still16MaxNum_P;
    stAdptH265FaceCfg.u32Still32ProtectNum_P = g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.pstH265VencFaceCfg[s32H265FaceCfgBitrateLevel * g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.s32ExpCount + s32H265FaceCfgExpLevel].u32Still32ProtectNum_P;
    stAdptH265FaceCfg.u32Still16ProtectNum_P = g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.pstH265VencFaceCfg[s32H265FaceCfgBitrateLevel * g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.s32ExpCount + s32H265FaceCfgExpLevel].u32Still16ProtectNum_P;
    stAdptH265FaceCfg.u32Hedge32MaxNum_P    = g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.pstH265VencFaceCfg[s32H265FaceCfgBitrateLevel * g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.s32ExpCount + s32H265FaceCfgExpLevel].u32Hedge32MaxNum_P;
    stAdptH265FaceCfg.u32Hedge16MaxNum_P    = g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.pstH265VencFaceCfg[s32H265FaceCfgBitrateLevel * g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.s32ExpCount + s32H265FaceCfgExpLevel].u32Hedge16MaxNum_P;
    stAdptH265FaceCfg.u32Hedge32ProtectNum_P = g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.pstH265VencFaceCfg[s32H265FaceCfgBitrateLevel * g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.s32ExpCount + s32H265FaceCfgExpLevel].u32Hedge32ProtectNum_P;
    stAdptH265FaceCfg.u32Hedge16ProtectNum_P = g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.pstH265VencFaceCfg[s32H265FaceCfgBitrateLevel * g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.s32ExpCount + s32H265FaceCfgExpLevel].u32Hedge16ProtectNum_P;
    s32Ret = CommSceneautoSetH265FaceCfg(s32VencChn, &stAdptH265FaceCfg);
    if (HI_SUCCESS != s32Ret)
    {
        printf("CommSceneautoSetH265FaceCfg failed\n");
        return HI_FAILURE;
    }
    return HI_SUCCESS;
}
HI_S32 SceneAuto_SetH265RcParam(HI_S32 s32VencChn, HI_S32 s32VencBitrateLevel)
{
    HI_S32 s32Ret = HI_SUCCESS;
    HI_S32 i;
    ADPT_SCENEAUTO_H265_RCPARAM_S stAdptH265RCParam;
    s32Ret = CommSceneautoGetH265RcParam(s32VencChn, &stAdptH265RCParam);
    if (HI_SUCCESS != s32Ret)
    {
        printf("CommSceneautoGetH265RcParam failed\n");
        return HI_FAILURE;
    }
    stAdptH265RCParam.u32QpDelta = g_stINIPara.stIniH265Venc.stIniH265VencRcParam.pstH265VencRcParam[s32VencBitrateLevel].u32DeltaQP;
    stAdptH265RCParam.s32IPQPDelta = g_stINIPara.stIniH265Venc.stIniH265VencRcParam.pstH265VencRcParam[s32VencBitrateLevel].s32IPQPDelta;
    for (i = 0; i < 12; i++)
    {
        stAdptH265RCParam.u32ThrdI[i] = g_stINIPara.stIniH265Venc.stIniH265VencRcParam.pstH265VencRcParam[s32VencBitrateLevel].u32ThrdI[i];
        stAdptH265RCParam.u32ThrdP[i] = g_stINIPara.stIniH265Venc.stIniH265VencRcParam.pstH265VencRcParam[s32VencBitrateLevel].u32ThrdP[i];
    }
    s32Ret = CommSceneautoSetH265RcParam(s32VencChn, &stAdptH265RCParam);
    if (HI_SUCCESS != s32Ret)
    {
        printf("CommSceneautoSetH265RcParam failed\n");
        return HI_FAILURE;
    }
    return HI_SUCCESS;
}
HI_S32 SceneAuto_SetH264RcParam(HI_S32 s32VencChn, HI_S32 s32VencBitrateLevel)
{
    HI_S32 s32Ret = HI_SUCCESS;
    HI_S32 i;
    ADPT_SCENEAUTO_H264_RCPARAM_S stAdptH264RCParam;

    s32Ret = CommSceneautoGetH264RcParam(s32VencChn, &stAdptH264RCParam);
    if (HI_SUCCESS != s32Ret)
    {
        printf("CommSceneautoGetH264RcParam failed\n");
        return HI_FAILURE;
    }

    stAdptH264RCParam.u32QpDelta = g_stINIPara.stIniH264Venc.pstH264Venc[s32VencBitrateLevel].u32DeltaQP;
    for (i = 0; i < 12; i++)
    {
        stAdptH264RCParam.u32ThrdI[i] = g_stINIPara.stIniH264Venc.pstH264Venc[s32VencBitrateLevel].u32ThrdI[i];
        stAdptH264RCParam.u32ThrdP[i] = g_stINIPara.stIniH264Venc.pstH264Venc[s32VencBitrateLevel].u32ThrdP[i];
    }
    stAdptH264RCParam.s32IPQPDelta = g_stINIPara.stIniH264Venc.pstH264Venc[s32VencBitrateLevel].s32IPQPDelta;

    s32Ret = CommSceneautoSetH264RcParam(s32VencChn, &stAdptH264RCParam);
    if (HI_SUCCESS != s32Ret)
    {
        printf("CommSceneautoSetH264RcParam failed\n");
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

HI_S32 SceneAuto_SetH264Debolck(HI_S32 s32VencChn, HI_S32 s32VencBitrateLevel)
{
    HI_S32 s32Ret = HI_SUCCESS;
    ADPT_SCENEAUTO_H264_DEBLOCK_S stAdptH264Deblock;

    s32Ret = CommSceneautoGetH24Deblock(s32VencChn, &stAdptH264Deblock);
    if (HI_SUCCESS != s32Ret)
    {
        printf("CommSceneautoSetH24Deblock failed\n");
        return HI_FAILURE;
    }

    stAdptH264Deblock.disable_deblocking_filter_idc = g_stINIPara.stIniH264Venc.pstH264Venc[s32VencBitrateLevel].stH264Dblk.disable_deblocking_filter_idc;
    stAdptH264Deblock.slice_alpha_c0_offset_div2 = g_stINIPara.stIniH264Venc.pstH264Venc[s32VencBitrateLevel].stH264Dblk.slice_alpha_c0_offset_div2;
    stAdptH264Deblock.slice_beta_offset_div2 = g_stINIPara.stIniH264Venc.pstH264Venc[s32VencBitrateLevel].stH264Dblk.slice_beta_offset_div2;
    s32Ret = CommSceneautoSetH24Deblock(s32VencChn, &stAdptH264Deblock);
    if (HI_SUCCESS != s32Ret)
    {
        printf("CommSceneautoSetH24Deblock failed\n");
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

HI_S32 SceneAuto_SetH264Trans(HI_S32  s32VencChn, HI_S32 s32VencBitrateLevel)
{
    HI_S32 s32Ret = HI_SUCCESS;
    ADPT_SCENEAUTO_H264TRANS_S stAdptH264Trans;

    s32Ret = CommSceneautoGetH264Trans(s32VencChn, &stAdptH264Trans);
    if (HI_SUCCESS != s32Ret)
    {
        printf("CommSceneautoGetH264Trans failed\n");
        return HI_FAILURE;
    }

    stAdptH264Trans.chroma_qp_index_offset = g_stINIPara.stIniH264Venc.pstH264Venc[s32VencBitrateLevel].s32chroma_qp_index_offset;
    s32Ret = CommSceneautoSetH264Trans(s32VencChn, &stAdptH264Trans);
    if (HI_SUCCESS != s32Ret)
    {
        printf("CommSceneautoSetH264Trans failed\n");
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

HI_S32 SceneAuto_SetDP(HI_S32 s32IspDev, HI_S32 s32DpExpLevel)
{
    HI_S32 s32Ret = HI_SUCCESS;
    ADPT_SCENEAUTO_DP_S stAdptDp;

    if ((SRDK_SCENEAUTO_SPECIAL_SCENE_IR != g_eSpecialScene))
    {
        s32Ret = CommSceneautoGetDP(s32IspDev, &stAdptDp);
        if (HI_SUCCESS != s32Ret)
        {
            printf("CommSceneautoGetDP failed\n");
            return HI_FAILURE;
        }

        stAdptDp.u16Slope = g_stINIPara.stIniDP.pstDPAttr[s32DpExpLevel].u16Slope;
        s32Ret = CommSceneautoSetDP(s32IspDev, &stAdptDp);
        if (HI_SUCCESS != s32Ret)
        {
            printf("CommSceneautoSetDP failed\n");
            return HI_FAILURE;
        }
    }
    return HI_SUCCESS;
}

HI_S32 SceneAuto_SetGamma(HI_S32 s32IspDev, HI_S32 s32GammaExpLevel, HI_S32 s32LastGammaExpLevel)
{
    HI_S32 s32Ret = HI_SUCCESS;
    HI_U32 u32Temp;
    HI_S32 i, j;
    ADPT_SCENEAUTO_GAMMA_S stAdptGamma;

    if ((SRDK_SCENEAUTO_SPECIAL_SCENE_IR != g_eSpecialScene) && (SRDK_SCENEAUTO_SPECIAL_SCENE_HLC != g_eSpecialScene))
    {
        for (j = 0; j < g_stINIPara.stIniGamma.s32Interval; j++)
        {
            for (i = 0; i < 257; i++)
            {
                if (g_stINIPara.stIniGamma.pstGamma[s32LastGammaExpLevel].u16Table[i] > g_stINIPara.stIniGamma.pstGamma[s32GammaExpLevel].u16Table[i])
                {
                    u32Temp = ((HI_U32)(g_stINIPara.stIniGamma.pstGamma[s32LastGammaExpLevel].u16Table[i] - g_stINIPara.stIniGamma.pstGamma[s32GammaExpLevel].u16Table[i])) << 8;
                    u32Temp = (u32Temp * (j + 1)) / g_stINIPara.stIniGamma.s32Interval;
                    u32Temp = u32Temp >> 8;
                    stAdptGamma.au16GammaTable[i] = g_stINIPara.stIniGamma.pstGamma[s32LastGammaExpLevel].u16Table[i] - (HI_U32)u32Temp;
                }
                else
                {
                    u32Temp = ((HI_U32)(g_stINIPara.stIniGamma.pstGamma[s32GammaExpLevel].u16Table[i] - g_stINIPara.stIniGamma.pstGamma[s32LastGammaExpLevel].u16Table[i])) << 8;
                    u32Temp = (u32Temp * (j + 1)) / g_stINIPara.stIniGamma.s32Interval;
                    u32Temp = u32Temp >> 8;
                    stAdptGamma.au16GammaTable[i] = g_stINIPara.stIniGamma.pstGamma[s32LastGammaExpLevel].u16Table[i] + (HI_U32)u32Temp;
                }
            }
            stAdptGamma.u8CurveType = g_stINIPara.stIniGamma.pstGamma[s32GammaExpLevel].u8CurveType;

            s32Ret = CommSceneautoSetGamma(s32IspDev, &stAdptGamma);
            if (HI_SUCCESS != s32Ret)
            {
                printf("CommSceneautoSetGamma failed\n");
                return HI_FAILURE;
            }
            usleep(80000);
        }
    }
    return HI_SUCCESS;
}
HI_S32 SceneAuto_SetDefaultGamma()
{
    HI_S32 s32Ret = HI_SUCCESS;
    HI_S32 i;
    ADPT_SCENEAUTO_GAMMA_S stAdptGamma;
    HI_S32 s32IspDev;

    s32IspDev = g_stINIPara.stMpInfo.s32IspDev;

    for (i = 0; i < 257; i++)
    {
        stAdptGamma.au16GammaTable[i] = g_stINIPara.stIniGamma.pstGamma[0].u16Table[i];
    }
    stAdptGamma.u8CurveType = 2;

    s32Ret = CommSceneautoSetGamma(s32IspDev, &stAdptGamma);
    if (HI_SUCCESS != s32Ret)
    {
        printf("CommSceneautoSetGamma failed\n");
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

HI_S32 Scenauto_SetIRAERealtedExp(HI_S32 s32IspDev, HI_S32 s32IRAECurPos)
{
    HI_S32 s32Ret = HI_SUCCESS;
    ADPT_SCENEAUTO_AEATTR_S stAdptAEAttr;

    s32Ret = CommSceneautoGetAEAttr(s32IspDev, &stAdptAEAttr);
    if (HI_SUCCESS != s32Ret)
    {
        printf("CommSceneautoGetAEAttr failed\n");
        return HI_FAILURE;
    }

    stAdptAEAttr.u8ExpCompensation = g_stINIPara.stIR.pu8ExpCompensation[s32IRAECurPos];
    stAdptAEAttr.u8MaxHistOffset = g_stINIPara.stIR.pu8MaxHistOffset[s32IRAECurPos];

    s32Ret = CommSceneautoSetAEAttr(s32IspDev, &stAdptAEAttr);
    if (HI_SUCCESS != s32Ret)
    {
        printf("CommSceneautoSetAEAttr failed\n");
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

HI_S32 Scenauto_SetAERealtedExp(HI_S32 s32IspDev, HI_S32 s32AECurPos)
{
    HI_S32 s32Ret = HI_SUCCESS;
    ADPT_SCENEAUTO_AEATTR_S stAdptAEAttr;

    if ((SRDK_SCENEAUTO_SPECIAL_SCENE_IR != g_eSpecialScene) && (SRDK_SCENEAUTO_SPECIAL_SCENE_BLC != g_eSpecialScene) && (SRDK_SCENEAUTO_SPECIAL_SCENE_HLC != g_eSpecialScene))
    {
        s32Ret = CommSceneautoGetAEAttr(s32IspDev, &stAdptAEAttr);
        if (HI_SUCCESS != s32Ret)
        {
            printf("CommSceneautoGetAEAttr failed\n");
            return HI_FAILURE;
        }

        stAdptAEAttr.u8ExpCompensation = g_stINIPara.stIniAE.pstAERelatedExp[s32AECurPos].u8AECompesation;
        stAdptAEAttr.u8MaxHistOffset = g_stINIPara.stIniAE.pstAERelatedExp[s32AECurPos].u8AEHistOffset;;

        s32Ret = CommSceneautoSetAEAttr(s32IspDev, &stAdptAEAttr);
        if (HI_SUCCESS != s32Ret)
        {
            printf("CommSceneautoSetAEAttr failed\n");
            return HI_FAILURE;
        }
    }

    return HI_SUCCESS;
}

HI_S32 Sceneauto_Set3DNR(HI_U32 s32VpssGrp, SCENEAUTO_INIPARAM_3DNRCFG_S stSceneauto3dnr)
{
    HI_S32 s32Ret = HI_SUCCESS;
    HI_S32 s32Index;
    ADPT_SCENEAUTO_3DNR stAdptSceneauto3dnr;
  
    stAdptSceneauto3dnr.u8SFC = stSceneauto3dnr.u8SFC;
    stAdptSceneauto3dnr.u8TFC = stSceneauto3dnr.u8TFC;
    stAdptSceneauto3dnr.u16PSFS = stSceneauto3dnr.u16PSFS;
    stAdptSceneauto3dnr.u16POST = stSceneauto3dnr.u16POST;
        for (s32Index = 0; s32Index < 3; s32Index++)
        {
        stAdptSceneauto3dnr.u8SBS[s32Index] = stSceneauto3dnr.u8SBS[s32Index];
        stAdptSceneauto3dnr.u8SBT[s32Index] = stSceneauto3dnr.u8SBT[s32Index];
        stAdptSceneauto3dnr.u8SDS[s32Index] = stSceneauto3dnr.u8SDS[s32Index];
        stAdptSceneauto3dnr.u8SDT[s32Index] = stSceneauto3dnr.u8SDT[s32Index];
        stAdptSceneauto3dnr.u8SHP[s32Index] = stSceneauto3dnr.u8SHP[s32Index];
        stAdptSceneauto3dnr.u16TFS[s32Index] = stSceneauto3dnr.u16TFS[s32Index];
        stAdptSceneauto3dnr.u16TFR[s32Index] = stSceneauto3dnr.u16TFR[s32Index];
        stAdptSceneauto3dnr.u16SBF[s32Index] = stSceneauto3dnr.u16SBF[s32Index];
        stAdptSceneauto3dnr.u16MDZ[s32Index] = stSceneauto3dnr.u16MDZ[s32Index];
        }        
    #if 1
    stAdptSceneauto3dnr.u8MABW = stSceneauto3dnr.u8MABW;
    stAdptSceneauto3dnr.u8PostROW = stSceneauto3dnr.u8PostROW;
    stAdptSceneauto3dnr.u8MATW = stSceneauto3dnr.u8MATW;
    stAdptSceneauto3dnr.u8MDAF = stSceneauto3dnr.u8MDAF;
    stAdptSceneauto3dnr.u8TextThr = stSceneauto3dnr.u8TextThr;
    stAdptSceneauto3dnr.u8MTFS = stSceneauto3dnr.u8MTFS;
    stAdptSceneauto3dnr.u8ExTfThr = stSceneauto3dnr.u8ExTfThr;
    #endif
    s32Ret = CommSceneautoSet3DNR(s32VpssGrp, &stAdptSceneauto3dnr);
    if (HI_SUCCESS != s32Ret)
    {
        printf("CommSceneautoSet3DNR failed\n");
        return HI_FAILURE;
    }
    return HI_SUCCESS;
}

HI_S32 Sceneauto_IVEStop()
{
    HI_S32 s32Ret = HI_SUCCESS;

    s32Ret = CommSceneautoIVEStop();
    if (HI_SUCCESS != s32Ret)
    {
        printf("CommSceneautoIVEStop failed\n");
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

HI_S32 Sceneauto_IVEStart()
{
    HI_S32 s32Ret = HI_SUCCESS;
    HI_S32 s32VpssGrp;
    HI_S32 s32VpssChn;

    s32VpssGrp = g_stINIPara.stMpInfo.s32VpssGrp;
    s32VpssChn = g_stINIPara.stMpInfo.s32VpssChn;
    s32Ret = CommSceneautoIVEStart(s32VpssGrp, s32VpssChn);
    if (HI_SUCCESS != s32Ret)
    {
        printf("CommSceneautoIVEStart failed\n");
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

HI_S32 SceneAuto_SetAERoute()
{
    HI_S32 s32Ret = HI_SUCCESS;
    HI_S32 i;
    HI_S32 s32IspDev;
    ADPT_SCENEAUTO_AEROUTE_S stAdptAERoute;

    s32IspDev = g_stINIPara.stMpInfo.s32IspDev;


    stAdptAERoute.u32TotalNum = g_stINIPara.stNormalDynamic.s32TotalNum;
    for (i = 0; i < g_stINIPara.stNormalDynamic.s32TotalNum; i++)
    {
        stAdptAERoute.astRouteNode[i].u32IntTime = g_stINIPara.stNormalDynamic.pstRouteNode[i].u32IntTime;
        stAdptAERoute.astRouteNode[i].u32SysGain = g_stINIPara.stNormalDynamic.pstRouteNode[i].u32SysGain;
    }

    s32Ret = CommSceneautoSetAERoute(s32IspDev, &stAdptAERoute);
    if (HI_SUCCESS != s32Ret)
    {
        printf("CommSceneautoSetAERoute failed\n");
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

HI_S32 SceneAuto_SetPreviousPara()
{
    HI_S32 s32Ret = HI_SUCCESS;
    HI_S32 i,j;
    HI_S32 s32IspDev;
    HI_S32 s32ViDev;
    HI_S32 s32VencChn;
    ADPT_SCENEAUTO_DEMOSAIC_S stAdptDemosaic;
    ADPT_SCENEAUTO_SHARPEN_S stAdptSharpen;
    ADPT_SCENEAUTO_DP_S stAdptDP;
    ADPT_SCENEAUTO_GAMMA_S stAdptGamma;
    ADPT_SCENEAUTO_H264_DEBLOCK_S stAdptH264Deblock;
    ADPT_SCENEAUTO_H264TRANS_S stAdptH264Trans;
    ADPT_SCENEAUTO_H264_RCPARAM_S stAdptH264RcParam;
    ADPT_SCENEAUTO_H265_RCPARAM_S stAdptH265RcParam;
    ADPT_SCENEAUTO_H265_FACE_CFG_S stAdptH265FaceCfg;
    ADPT_SCENEAUTO_AEROUTE_S stAdptAERoute;
    ADPT_SCENEAUTO_DCIPARAM_S stAdptDCIPara;
    ADPT_SCENEAUTO_DRCATTR_S stAdptDRCAttr;
    ADPT_SCENEAUTO_SATURATION_S stAdptSaturation;
    ADPT_SCENEAUTO_DIS_ATTR_S stAdptDISAttr;
    ADPT_SCENEAUTO_AEATTR_S stAdptAEAttr;
    ADPT_SCENEAUTO_VENC_ATTR_S stAdptVencAttr;
    ADPT_SCENEAUTO_CCM_ATTR_S stAdptCcmAttr;
    ADPT_SCENEAUTO_ACM_ATTR_S stAdptAcmAttr;

    s32IspDev = g_stINIPara.stMpInfo.s32IspDev;
    s32ViDev = g_stINIPara.stMpInfo.s32ViDev;
    s32VencChn = g_stINIPara.stMpInfo.s32VencChn;

    //demosaic
    stAdptDemosaic.u8AaSlope = g_stPreviousPara.stDemosaic.u8AaSlope;
    stAdptDemosaic.u8UuSlope = g_stPreviousPara.stDemosaic.u8UuSlope;
    stAdptDemosaic.u8VaSlope = g_stPreviousPara.stDemosaic.u8VaSlope;
    stAdptDemosaic.u8VhSlope = g_stPreviousPara.stDemosaic.u8VhSlope;
    for (i = 0; i < 16; i++)
    {
        stAdptDemosaic.au8LumThresh[i] = g_stPreviousPara.stDemosaic.au8LumThresh[i];
    }
    s32Ret = CommSceneautoSetDemosaic(s32IspDev, &stAdptDemosaic);
    if (HI_SUCCESS != s32Ret)
    {
        printf("Adpt_GetDemosaic failed\n");
        return HI_FAILURE;
    }

    //sharpen
    for (i =  0; i < 16; i++)
    {
        stAdptSharpen.au8SharpenD[i] = g_stPreviousPara.stSharpen.au8SharpenD[i];
        stAdptSharpen.au8SharpenRGB[i] = g_stPreviousPara.stSharpen.au8SharpenRGB[i];
        stAdptSharpen.au8SharpenUd[i] = g_stPreviousPara.stSharpen.au8SharpenUd[i];
    }
    s32Ret = CommSceneautoSetSharpen(s32IspDev, &stAdptSharpen);
    if (HI_SUCCESS != s32Ret)
    {
        printf("Adpt_GetDemosaic failed\n");
        return HI_FAILURE;
    }

    //DP
    stAdptDP.u16Slope = g_stPreviousPara.stDP.u16Slope;
    s32Ret = CommSceneautoSetDP(s32IspDev, &stAdptDP);
    if (HI_SUCCESS != s32Ret)
    {
        printf("CommSceneautoGetDP failed\n");
        return HI_FAILURE;
    }

    //gamma
    stAdptGamma.u8CurveType = g_stPreviousPara.stGamma.u8CurveType;
    for (i = 0 ; i < 257; i++)
    {
        stAdptGamma.au16GammaTable[i] = g_stPreviousPara.stGamma.u16Table[i];
    }
    s32Ret = CommSceneautoSetGamma(s32IspDev, &stAdptGamma);
    if (HI_SUCCESS != s32Ret)
    {
        printf("CommSceneautoGetGamma failed\n");
        return HI_FAILURE;
    }

    //ccm
    stAdptCcmAttr.u16HighColorTemp = g_stPreviousPara.stCcm.u16HighColorTemp;
    stAdptCcmAttr.u16MidColorTemp = g_stPreviousPara.stCcm.u16MidColorTemp;
    stAdptCcmAttr.u16LowColorTemp = g_stPreviousPara.stCcm.u16LowColorTemp;
    for (i = 0; i < 9; i++)
    {
        stAdptCcmAttr.au16HighCCM[i] = g_stPreviousPara.stCcm.au16HighCCM[i];
        stAdptCcmAttr.au16MidCCM[i] = g_stPreviousPara.stCcm.au16MidCCM[i];
        stAdptCcmAttr.au16LowCCM[i] = g_stPreviousPara.stCcm.au16LowCCM[i];
    }
    s32Ret = CommSceneautoSetCcmAttr(s32IspDev, &stAdptCcmAttr);
    if (HI_SUCCESS != s32Ret)
    {
        printf("CommSceneautoGetCcmAttr failed\n");
        return HI_FAILURE;
    }

    //acm
    stAdptAcmAttr.bEnable = g_stPreviousPara.stAcm.bEnable;
    stAdptAcmAttr.u32CbcrThr = g_stPreviousPara.stAcm.u32CbcrThr;
    stAdptAcmAttr.u32GainHue = g_stPreviousPara.stAcm.u32GainHue;
    stAdptAcmAttr.u32GainLuma = g_stPreviousPara.stAcm.u32GainLuma;
    stAdptAcmAttr.u32GainSat = g_stPreviousPara.stAcm.u32GainSat;
    s32Ret = CommSceneautoSetAcmAttr(s32IspDev, &stAdptAcmAttr);
    if (HI_SUCCESS != s32Ret)
    {
        printf("CommSceneautoGetAcmAttr failed\n");
        return HI_FAILURE;
    }

    //venc
    switch (g_eVencRcMode)
    {
        case ADPT_SCENEAUTO_RCMODE_H264:
            s32Ret = CommSceneautoGetVencAttr(s32VencChn, &stAdptVencAttr);
            if (HI_SUCCESS != s32Ret)
            {
                printf("CommSceneautoGetVencAttr failed\n");
                return HI_FAILURE;
            }
            if (ADPT_SCENEAUTO_RCMODE_H264 == stAdptVencAttr.eRcMode)
            {
                stAdptH264Deblock.disable_deblocking_filter_idc = g_stPreviousPara.stH264Venc.stH264Dblk.disable_deblocking_filter_idc;
                stAdptH264Deblock.slice_alpha_c0_offset_div2 = g_stPreviousPara.stH264Venc.stH264Dblk.slice_alpha_c0_offset_div2;
                stAdptH264Deblock.slice_beta_offset_div2 = g_stPreviousPara.stH264Venc.stH264Dblk.slice_beta_offset_div2;
                s32Ret = CommSceneautoSetH24Deblock(s32VencChn, &stAdptH264Deblock);
                if (HI_SUCCESS != s32Ret)
                {
                    printf("CommSceneautoSetH24Deblock failed\n");
                    return HI_FAILURE;
                }

                stAdptH264Trans.chroma_qp_index_offset = g_stPreviousPara.stH264Venc.s32chroma_qp_index_offset;
                s32Ret = CommSceneautoSetH264Trans(s32VencChn, &stAdptH264Trans);
                if (HI_SUCCESS != s32Ret)
                {
                    printf("CommSceneautoSetH264Trans failed\n");
                    return HI_FAILURE;
                }

                stAdptH264RcParam.s32IPQPDelta = g_stPreviousPara.stH264Venc.s32IPQPDelta;
                stAdptH264RcParam.u32QpDelta = g_stPreviousPara.stH264Venc.u32DeltaQP;
                for (i = 0; i < 12; i++)
                {
                    stAdptH264RcParam.u32ThrdI[i] = g_stPreviousPara.stH264Venc.u32ThrdI[i];
                    stAdptH264RcParam.u32ThrdP[i] = g_stPreviousPara.stH264Venc.u32ThrdP[i];
                }
                s32Ret = CommSceneautoSetH264RcParam(s32VencChn, &stAdptH264RcParam);
                if (HI_SUCCESS != s32Ret)
                {
                    printf("CommSceneautoSetH264RcParam failed\n");
                    return HI_FAILURE;
                }
            }
            break;
        case ADPT_SCENEAUTO_RCMODE_H265:
            s32Ret = CommSceneautoGetVencAttr(s32VencChn, &stAdptVencAttr);
            if (HI_SUCCESS != s32Ret)
            {
                printf("CommSceneautoGetVencAttr failed\n");
                return HI_FAILURE;
            }
            if (ADPT_SCENEAUTO_RCMODE_H265 == stAdptVencAttr.eRcMode)
            {
                stAdptH265RcParam.u32QpDelta = g_stPreviousPara.stH265Venc.stH265VencRcParam.u32DeltaQP;
                stAdptH265RcParam.s32IPQPDelta = g_stPreviousPara.stH265Venc.stH265VencRcParam.s32IPQPDelta;
                for (i = 0; i < 12; i++)
                {
                    stAdptH265RcParam.u32ThrdI[i] = g_stPreviousPara.stH265Venc.stH265VencRcParam.u32ThrdI[i];
                    stAdptH265RcParam.u32ThrdP[i] = g_stPreviousPara.stH265Venc.stH265VencRcParam.u32ThrdP[i];
                }
                s32Ret = CommSceneautoSetH265RcParam(s32VencChn, &stAdptH265RcParam);
                if (HI_SUCCESS != s32Ret)
                {
                    printf("CommSceneautoGetH265RcParam failed\n");
                    return HI_FAILURE;
                }
                {
                    stAdptH265FaceCfg.u8NormIntra4RdCost_I   = g_stPreviousPara.stH265Venc.stH265VencFaceCfg.u8NormIntra4RdCost_I;
                    stAdptH265FaceCfg.u8NormIntra8RdCost_I   = g_stPreviousPara.stH265Venc.stH265VencFaceCfg.u8NormIntra8RdCost_I;
                    stAdptH265FaceCfg.u8NormIntra16RdCost_I  = g_stPreviousPara.stH265Venc.stH265VencFaceCfg.u8NormIntra16RdCost_I;
                    stAdptH265FaceCfg.u8NormIntra32RdCost_I  = g_stPreviousPara.stH265Venc.stH265VencFaceCfg.u8NormIntra32RdCost_I;
                    stAdptH265FaceCfg.u8SkinIntra4RdCost_I   = g_stPreviousPara.stH265Venc.stH265VencFaceCfg.u8SkinIntra4RdCost_I;
                    stAdptH265FaceCfg.u8SkinIntra8RdCost_I   = g_stPreviousPara.stH265Venc.stH265VencFaceCfg.u8SkinIntra8RdCost_I;
                    stAdptH265FaceCfg.u8SkinIntra16RdCost_I  = g_stPreviousPara.stH265Venc.stH265VencFaceCfg.u8SkinIntra16RdCost_I;
                    stAdptH265FaceCfg.u8SkinIntra32RdCost_I  = g_stPreviousPara.stH265Venc.stH265VencFaceCfg.u8SkinIntra32RdCost_I;
                    stAdptH265FaceCfg.u8HedgeIntra4RdCost_I  = g_stPreviousPara.stH265Venc.stH265VencFaceCfg.u8HedgeIntra4RdCost_I;
                    stAdptH265FaceCfg.u8HedgeIntra8RdCost_I  = g_stPreviousPara.stH265Venc.stH265VencFaceCfg.u8HedgeIntra8RdCost_I;
                    stAdptH265FaceCfg.u8HedgeIntra16RdCost_I = g_stPreviousPara.stH265Venc.stH265VencFaceCfg.u8HedgeIntra16RdCost_I;
                    stAdptH265FaceCfg.u8HedgeIntra32RdCost_I = g_stPreviousPara.stH265Venc.stH265VencFaceCfg.u8HedgeIntra32RdCost_I;
                    stAdptH265FaceCfg.u8NormIntra4RdCost_P   = g_stPreviousPara.stH265Venc.stH265VencFaceCfg.u8NormIntra4RdCost_P;
                    stAdptH265FaceCfg.u8NormIntra8RdCost_P   = g_stPreviousPara.stH265Venc.stH265VencFaceCfg.u8NormIntra8RdCost_P;
                    stAdptH265FaceCfg.u8NormIntra16RdCost_P  = g_stPreviousPara.stH265Venc.stH265VencFaceCfg.u8NormIntra16RdCost_P;
                    stAdptH265FaceCfg.u8NormIntra32RdCost_P  = g_stPreviousPara.stH265Venc.stH265VencFaceCfg.u8NormIntra32RdCost_P;
                    stAdptH265FaceCfg.u8SkinIntra4RdCost_P   = g_stPreviousPara.stH265Venc.stH265VencFaceCfg.u8SkinIntra4RdCost_P;
                    stAdptH265FaceCfg.u8SkinIntra8RdCost_P   = g_stPreviousPara.stH265Venc.stH265VencFaceCfg.u8SkinIntra8RdCost_P;
                    stAdptH265FaceCfg.u8SkinIntra16RdCost_P  = g_stPreviousPara.stH265Venc.stH265VencFaceCfg.u8SkinIntra16RdCost_P;
                    stAdptH265FaceCfg.u8SkinIntra32RdCost_P  = g_stPreviousPara.stH265Venc.stH265VencFaceCfg.u8SkinIntra32RdCost_P;
                    stAdptH265FaceCfg.u8HedgeIntra4RdCost_P  = g_stPreviousPara.stH265Venc.stH265VencFaceCfg.u8HedgeIntra4RdCost_P;
                    stAdptH265FaceCfg.u8HedgeIntra8RdCost_P  = g_stPreviousPara.stH265Venc.stH265VencFaceCfg.u8HedgeIntra8RdCost_P;
                    stAdptH265FaceCfg.u8HedgeIntra16RdCost_P = g_stPreviousPara.stH265Venc.stH265VencFaceCfg.u8HedgeIntra16RdCost_P;
                    stAdptH265FaceCfg.u8HedgeIntra32RdCost_P = g_stPreviousPara.stH265Venc.stH265VencFaceCfg.u8HedgeIntra32RdCost_P;
                    stAdptH265FaceCfg.u8NormFme8RdCost_P     = g_stPreviousPara.stH265Venc.stH265VencFaceCfg.u8NormFme8RdCost_P;
                    stAdptH265FaceCfg.u8NormFme16RdCost_P    = g_stPreviousPara.stH265Venc.stH265VencFaceCfg.u8NormFme16RdCost_P;
                    stAdptH265FaceCfg.u8NormFme32RdCost_P    = g_stPreviousPara.stH265Venc.stH265VencFaceCfg.u8NormFme32RdCost_P;
                    stAdptH265FaceCfg.u8NormFme64RdCost_P    = g_stPreviousPara.stH265Venc.stH265VencFaceCfg.u8NormFme64RdCost_P;
                    stAdptH265FaceCfg.u8SkinFme8RdCost_P     = g_stPreviousPara.stH265Venc.stH265VencFaceCfg.u8SkinFme8RdCost_P;
                    stAdptH265FaceCfg.u8SkinFme16RdCost_P    = g_stPreviousPara.stH265Venc.stH265VencFaceCfg.u8SkinFme16RdCost_P;
                    stAdptH265FaceCfg.u8SkinFme32RdCost_P    = g_stPreviousPara.stH265Venc.stH265VencFaceCfg.u8SkinFme32RdCost_P;
                    stAdptH265FaceCfg.u8SkinFme64RdCost_P    = g_stPreviousPara.stH265Venc.stH265VencFaceCfg.u8SkinFme64RdCost_P;
                    stAdptH265FaceCfg.u8HedgeFme8RdCost_P    = g_stPreviousPara.stH265Venc.stH265VencFaceCfg.u8HedgeFme8RdCost_P;
                    stAdptH265FaceCfg.u8HedgeFme16RdCost_P   = g_stPreviousPara.stH265Venc.stH265VencFaceCfg.u8HedgeFme16RdCost_P;
                    stAdptH265FaceCfg.u8HedgeFme32RdCost_P   = g_stPreviousPara.stH265Venc.stH265VencFaceCfg.u8HedgeFme32RdCost_P;
                    stAdptH265FaceCfg.u8HedgeFme64RdCost_P   = g_stPreviousPara.stH265Venc.stH265VencFaceCfg.u8HedgeFme64RdCost_P;
                    stAdptH265FaceCfg.u8NormMerg8RdCost_P    = g_stPreviousPara.stH265Venc.stH265VencFaceCfg.u8NormMerg8RdCost_P;
                    stAdptH265FaceCfg.u8NormMerg16RdCost_P   = g_stPreviousPara.stH265Venc.stH265VencFaceCfg.u8NormMerg16RdCost_P;
                    stAdptH265FaceCfg.u8NormMerg32RdCost_P   = g_stPreviousPara.stH265Venc.stH265VencFaceCfg.u8NormMerg32RdCost_P;
                    stAdptH265FaceCfg.u8NormMerg64RdCost_P   = g_stPreviousPara.stH265Venc.stH265VencFaceCfg.u8NormMerg64RdCost_P;
                    stAdptH265FaceCfg.u8SkinMerg8RdCost_P    = g_stPreviousPara.stH265Venc.stH265VencFaceCfg.u8SkinMerg8RdCost_P;
                    stAdptH265FaceCfg.u8SkinMerg16RdCost_P   = g_stPreviousPara.stH265Venc.stH265VencFaceCfg.u8SkinMerg16RdCost_P;
                    stAdptH265FaceCfg.u8SkinMerg32RdCost_P   = g_stPreviousPara.stH265Venc.stH265VencFaceCfg.u8SkinMerg32RdCost_P;
                    stAdptH265FaceCfg.u8SkinMerg64RdCost_P   = g_stPreviousPara.stH265Venc.stH265VencFaceCfg.u8SkinMerg64RdCost_P;
                    stAdptH265FaceCfg.u8HedgeMerg8RdCost_P   = g_stPreviousPara.stH265Venc.stH265VencFaceCfg.u8HedgeMerg8RdCost_P;
                    stAdptH265FaceCfg.u8HedgeMerg16RdCost_P  = g_stPreviousPara.stH265Venc.stH265VencFaceCfg.u8HedgeMerg16RdCost_P;
                    stAdptH265FaceCfg.u8HedgeMerg32RdCost_P  = g_stPreviousPara.stH265Venc.stH265VencFaceCfg.u8HedgeMerg32RdCost_P;
                    stAdptH265FaceCfg.u8HedgeMerg64RdCost_P  = g_stPreviousPara.stH265Venc.stH265VencFaceCfg.u8HedgeMerg64RdCost_P;
                    stAdptH265FaceCfg.bSkinEn_I              = g_stPreviousPara.stH265Venc.stH265VencFaceCfg.bSkinEn_I;
                    stAdptH265FaceCfg.u32SkinQpDelta_I       = g_stPreviousPara.stH265Venc.stH265VencFaceCfg.u32SkinQpDelta_I;
                    stAdptH265FaceCfg.u8SkinUMax_I           = g_stPreviousPara.stH265Venc.stH265VencFaceCfg.u8SkinUMax_I;
                    stAdptH265FaceCfg.u8SkinUMin_I           = g_stPreviousPara.stH265Venc.stH265VencFaceCfg.u8SkinUMin_I;
                    stAdptH265FaceCfg.u8SkinVMax_I           = g_stPreviousPara.stH265Venc.stH265VencFaceCfg.u8SkinVMax_I;
                    stAdptH265FaceCfg.u8SkinVMin_I           = g_stPreviousPara.stH265Venc.stH265VencFaceCfg.u8SkinVMin_I;
                    stAdptH265FaceCfg.u32SkinNum_I           = g_stPreviousPara.stH265Venc.stH265VencFaceCfg.u32SkinNum_I;
                    stAdptH265FaceCfg.bSkinEn_P              = g_stPreviousPara.stH265Venc.stH265VencFaceCfg.bSkinEn_P;
                    stAdptH265FaceCfg.u32SkinQpDelta_P       = g_stPreviousPara.stH265Venc.stH265VencFaceCfg.u32SkinQpDelta_P;
                    stAdptH265FaceCfg.u8SkinUMax_P           = g_stPreviousPara.stH265Venc.stH265VencFaceCfg.u8SkinUMax_P;
                    stAdptH265FaceCfg.u8SkinUMin_P           = g_stPreviousPara.stH265Venc.stH265VencFaceCfg.u8SkinUMin_P;
                    stAdptH265FaceCfg.u8SkinVMax_P           = g_stPreviousPara.stH265Venc.stH265VencFaceCfg.u8SkinVMax_P;
                    stAdptH265FaceCfg.u8SkinVMin_P           = g_stPreviousPara.stH265Venc.stH265VencFaceCfg.u8SkinVMin_P;
                    stAdptH265FaceCfg.u32SkinNum_P           = g_stPreviousPara.stH265Venc.stH265VencFaceCfg.u32SkinNum_P;
                    stAdptH265FaceCfg.u8HedgeThr_I           = g_stPreviousPara.stH265Venc.stH265VencFaceCfg.u8HedgeThr_I;
                    stAdptH265FaceCfg.u8HedgeCnt_I           = g_stPreviousPara.stH265Venc.stH265VencFaceCfg.u8HedgeCnt_I;
                    stAdptH265FaceCfg.bStroEdgeEn_I          = g_stPreviousPara.stH265Venc.stH265VencFaceCfg.bStroEdgeEn_I;
                    stAdptH265FaceCfg.u32StroEdgeQpDelta_I   = g_stPreviousPara.stH265Venc.stH265VencFaceCfg.u32StroEdgeQpDelta_I;
                    stAdptH265FaceCfg.u8HedgeThr_P           = g_stPreviousPara.stH265Venc.stH265VencFaceCfg.u8HedgeThr_P;
                    stAdptH265FaceCfg.u8HedgeCnt_P           = g_stPreviousPara.stH265Venc.stH265VencFaceCfg.u8HedgeCnt_P;
                    stAdptH265FaceCfg.bStroEdgeEn_P          = g_stPreviousPara.stH265Venc.stH265VencFaceCfg.bStroEdgeEn_P;
                    stAdptH265FaceCfg.u32StroEdgeQpDelta_P   = g_stPreviousPara.stH265Venc.stH265VencFaceCfg.u32StroEdgeQpDelta_P;
                    stAdptH265FaceCfg.bImproveEn_I           = g_stPreviousPara.stH265Venc.stH265VencFaceCfg.bImproveEn_I;
                    stAdptH265FaceCfg.bImproveEn_P           = g_stPreviousPara.stH265Venc.stH265VencFaceCfg.bImproveEn_P;
                    stAdptH265FaceCfg.u32Norm32MaxNum_P      = g_stPreviousPara.stH265Venc.stH265VencFaceCfg.u32Norm32MaxNum_P;
                    stAdptH265FaceCfg.u32Norm16MaxNum_P      = g_stPreviousPara.stH265Venc.stH265VencFaceCfg.u32Norm16MaxNum_P;
                    stAdptH265FaceCfg.u32Norm32ProtectNum_P  = g_stPreviousPara.stH265Venc.stH265VencFaceCfg.u32Norm32ProtectNum_P;
                    stAdptH265FaceCfg.u32Norm16ProtectNum_P  = g_stPreviousPara.stH265Venc.stH265VencFaceCfg.u32Norm16ProtectNum_P;
                    stAdptH265FaceCfg.u32Skin32MaxNum_P      = g_stPreviousPara.stH265Venc.stH265VencFaceCfg.u32Skin32MaxNum_P;
                    stAdptH265FaceCfg.u32Skin16MaxNum_P      = g_stPreviousPara.stH265Venc.stH265VencFaceCfg.u32Skin16MaxNum_P;
                    stAdptH265FaceCfg.u32Skin32ProtectNum_P  = g_stPreviousPara.stH265Venc.stH265VencFaceCfg.u32Skin32ProtectNum_P;
                    stAdptH265FaceCfg.u32Skin16ProtectNum_P  = g_stPreviousPara.stH265Venc.stH265VencFaceCfg.u32Skin16ProtectNum_P;
                    stAdptH265FaceCfg.u32Still32MaxNum_P     = g_stPreviousPara.stH265Venc.stH265VencFaceCfg.u32Still32MaxNum_P;
                    stAdptH265FaceCfg.u32Still16MaxNum_P     = g_stPreviousPara.stH265Venc.stH265VencFaceCfg.u32Still16MaxNum_P;
                    stAdptH265FaceCfg.u32Still32ProtectNum_P = g_stPreviousPara.stH265Venc.stH265VencFaceCfg.u32Still32ProtectNum_P;
                    stAdptH265FaceCfg.u32Still16ProtectNum_P = g_stPreviousPara.stH265Venc.stH265VencFaceCfg.u32Still16ProtectNum_P;
                    stAdptH265FaceCfg.u32Hedge32MaxNum_P     = g_stPreviousPara.stH265Venc.stH265VencFaceCfg.u32Hedge32MaxNum_P;
                    stAdptH265FaceCfg.u32Hedge16MaxNum_P     = g_stPreviousPara.stH265Venc.stH265VencFaceCfg.u32Hedge16MaxNum_P;
                    stAdptH265FaceCfg.u32Hedge32ProtectNum_P = g_stPreviousPara.stH265Venc.stH265VencFaceCfg.u32Hedge32ProtectNum_P;
                    stAdptH265FaceCfg.u32Hedge16ProtectNum_P = g_stPreviousPara.stH265Venc.stH265VencFaceCfg.u32Hedge16ProtectNum_P;
                }
                s32Ret = CommSceneautoSetH265FaceCfg(s32VencChn, &stAdptH265FaceCfg);
                if (HI_SUCCESS != s32Ret)
                {
                    printf("CommSceneautoSetH265FaceCfg failed\n");
                    return HI_FAILURE;
                }
            }
            break;
        default:
            break;
    }

    //AERoute
    stAdptAERoute.u32TotalNum = g_stPreviousPara.stAERoute.u32TotalNum;
    for (i = 0; i < 8; i++)
    {
        stAdptAERoute.astRouteNode[i].u32IntTime = g_stPreviousPara.stAERoute.astRouteNode[i].u32IntTime;
        stAdptAERoute.astRouteNode[i].u32SysGain = g_stPreviousPara.stAERoute.astRouteNode[i].u32SysGain;
    }
    s32Ret = CommSceneautoSetAERoute(s32IspDev, &stAdptAERoute);
    if (HI_SUCCESS != s32Ret)
    {
        printf("CommSceneautoGetAERoute failed\n");
        return HI_FAILURE;
    }

    //DCI
    stAdptDCIPara.bEnable = g_stPreviousPara.stDCIParam.bEanble;
    stAdptDCIPara.u32BlackGain = g_stPreviousPara.stDCIParam.u32BlackGain;
    stAdptDCIPara.u32ContrastGain = g_stPreviousPara.stDCIParam.u32ContrastGain;
    stAdptDCIPara.u32LightGain = g_stPreviousPara.stDCIParam.u32LightGain;
    s32Ret = CommSceneautoSetDCIParam(s32ViDev, &stAdptDCIPara);
    if (HI_SUCCESS != s32Ret)
    {
        printf("CommSceneautoGetDCIParam failed\n");
        return HI_FAILURE;
    }

    //DRC
    stAdptDRCAttr.bEnable = g_stPreviousPara.stDRCAttr.bEnable;
    stAdptDRCAttr.bManulEnable = g_stPreviousPara.stDRCAttr.bManulEnable;
    stAdptDRCAttr.u32Strength = g_stPreviousPara.stDRCAttr.s32Strength;
    stAdptDRCAttr.u32SlopeMax = g_stPreviousPara.stDRCAttr.u32SlopeMax;
    stAdptDRCAttr.u32SlopeMin = g_stPreviousPara.stDRCAttr.u32SlopeMin;
    stAdptDRCAttr.u32VarianceSpace = g_stPreviousPara.stDRCAttr.u32VarianceSpace;
    stAdptDRCAttr.u32VarianceIntensity = g_stPreviousPara.stDRCAttr.u32VarianceIntensity;
    stAdptDRCAttr.u32WhiteLevel = g_stPreviousPara.stDRCAttr.u32WhiteLevel;
    stAdptDRCAttr.u32BlackLevel = g_stPreviousPara.stDRCAttr.u32BlackLevel;
    s32Ret = CommSceneautoSetDRCAttr(s32IspDev, &stAdptDRCAttr);
    if (HI_SUCCESS != s32Ret)
    {
        printf("CommSceneautoGetDRCAttr failed\n");
        return HI_FAILURE;
    }

    //Saturation
    stAdptSaturation.u8OpType = g_stPreviousPara.stSaturation.u8OpType;
    stAdptSaturation.u8ManualSat = g_stPreviousPara.stSaturation.u8ManualSat;
    for (i = 0; i < 16; i++)
    {
        stAdptSaturation.au8AutoSat[i] = g_stPreviousPara.stSaturation.au8AutoSat[i];
    }
    s32Ret = CommSceneautoSetSaturation(s32IspDev, &stAdptSaturation);
    if (HI_SUCCESS != s32Ret)
    {
        printf("CommSceneautoGetDRCAttr failed\n");
        return HI_FAILURE;
    }

    //DIS
    stAdptDISAttr.bEnable = g_stPreviousPara.stDis.bEnable;
    s32Ret = CommSceneautoSetDISAttr(s32IspDev, &stAdptDISAttr);
    if (HI_SUCCESS != s32Ret)
    {
        printf("CommSceneautoGetDRCAttr failed\n");
        return HI_FAILURE;
    }

    //AE
    stAdptAEAttr.u8ExpCompensation = g_stPreviousPara.stAEAttr.u8ExpCompensation;
    stAdptAEAttr.u16BlackDelayFrame = g_stPreviousPara.stAEAttr.u16BlackDelayFrame;
    stAdptAEAttr.u16WhiteDelayFrame = g_stPreviousPara.stAEAttr.u16WhiteDelayFrame;
    stAdptAEAttr.u8AEStrategyMode = g_stPreviousPara.stAEAttr.u8AEStrategyMode;
    stAdptAEAttr.u8MaxHistOffset = g_stPreviousPara.stAEAttr.u8MaxHistOffset;
    stAdptAEAttr.u16HistRatioSlope = g_stPreviousPara.stAEAttr.u16HistRatioSlope;
    stAdptAEAttr.u8Speed = g_stPreviousPara.stAEAttr.u8Speed;
    stAdptAEAttr.u8Tolerance = g_stPreviousPara.stAEAttr.u8Tolerance;
    stAdptAEAttr.u32SysGainMax = g_stPreviousPara.stAEAttr.u32SysGainMax;
    stAdptAEAttr.u8AERunInterval = g_stPreviousPara.stAEAttr.u8AERunInterval;
    for (i = 0; i < AE_WEIGHT_ROW; i++)
    {
        for (j = 0; j < AE_WEIGHT_COLUMN; j++)
        {
            stAdptAEAttr.au8AeWeight[i][j] = g_stPreviousPara.stAEAttr.au8AeWeight[i][j];
        }
    }    
    s32Ret = CommSceneautoSetAEAttr(s32IspDev, &stAdptAEAttr);
    if (HI_SUCCESS != s32Ret)
    {
        printf("CommSceneautoGetDRCAttr failed\n");
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

HI_S32 Sceneauto_GetPreviousPara()
{
    HI_S32 s32Ret = HI_SUCCESS;
    HI_S32 i,j;
    HI_S32 s32IspDev;
    HI_S32 s32ViDev;
    HI_S32 s32VencChn;
    ADPT_SCENEAUTO_DEMOSAIC_S stAdptDemosaic;
    ADPT_SCENEAUTO_SHARPEN_S stAdptSharpen;
    ADPT_SCENEAUTO_DP_S stAdptDP;
    ADPT_SCENEAUTO_GAMMA_S stAdptGamma;
    ADPT_SCENEAUTO_H264_DEBLOCK_S stAdptH264Deblock;
    ADPT_SCENEAUTO_H264TRANS_S stAdptH264Trans;
    ADPT_SCENEAUTO_H264_RCPARAM_S stAdptH264RcParam;
    ADPT_SCENEAUTO_H265_RCPARAM_S stAdptH265RcParam;
    ADPT_SCENEAUTO_H265_FACE_CFG_S stAdptH265FaceCfg;
    ADPT_SCENEAUTO_AEROUTE_S stAdptAERoute;
    ADPT_SCENEAUTO_DCIPARAM_S stAdptDCIPara;
    ADPT_SCENEAUTO_DRCATTR_S stAdptDRCAttr;
    ADPT_SCENEAUTO_SATURATION_S stAdptSaturation;
    ADPT_SCENEAUTO_DIS_ATTR_S stAdptDISAttr;
    ADPT_SCENEAUTO_AEATTR_S stAdptAEAttr;
    ADPT_SCENEAUTO_VENC_ATTR_S stAdptVencAttr;
    ADPT_SCENEAUTO_CCM_ATTR_S stAdptCcmAttr;
    ADPT_SCENEAUTO_ACM_ATTR_S stAdptAcmAttr;

    s32IspDev = g_stINIPara.stMpInfo.s32IspDev;
    s32ViDev = g_stINIPara.stMpInfo.s32ViDev;
    s32VencChn = g_stINIPara.stMpInfo.s32VencChn;

    //demosaic
    s32Ret = CommSceneautoGetDemosaic(s32IspDev, &stAdptDemosaic);
    if (HI_SUCCESS != s32Ret)
    {
        printf("Adpt_GetDemosaic failed\n");
        return HI_FAILURE;
    }
    g_stPreviousPara.stDemosaic.u8AaSlope = stAdptDemosaic.u8AaSlope;
    g_stPreviousPara.stDemosaic.u8UuSlope = stAdptDemosaic.u8UuSlope;
    g_stPreviousPara.stDemosaic.u8VaSlope = stAdptDemosaic.u8VaSlope;
    g_stPreviousPara.stDemosaic.u8VhSlope = stAdptDemosaic.u8VhSlope;
    for (i = 0; i < 16; i++)
    {
        g_stPreviousPara.stDemosaic.au8LumThresh[i] = stAdptDemosaic.au8LumThresh[i];
    }

    //sharpen
    s32Ret = CommSceneautoGetSharpen(s32IspDev, &stAdptSharpen);
    if (HI_SUCCESS != s32Ret)
    {
        printf("Adpt_GetDemosaic failed\n");
        return HI_FAILURE;
    }
    for (i =  0; i < 16; i++)
    {
        g_stPreviousPara.stSharpen.au8SharpenD[i] = stAdptSharpen.au8SharpenD[i];
        g_stPreviousPara.stSharpen.au8SharpenRGB[i] = stAdptSharpen.au8SharpenRGB[i];
        g_stPreviousPara.stSharpen.au8SharpenUd[i] = stAdptSharpen.au8SharpenUd[i];
    }

    //DP
    s32Ret = CommSceneautoGetDP(s32IspDev, &stAdptDP);
    if (HI_SUCCESS != s32Ret)
    {
        printf("CommSceneautoGetDP failed\n");
        return HI_FAILURE;
    }
    g_stPreviousPara.stDP.u16Slope = stAdptDP.u16Slope;

    //gamma
    s32Ret = CommSceneautoGetGamma(s32IspDev, &stAdptGamma);
    if (HI_SUCCESS != s32Ret)
    {
        printf("CommSceneautoGetGamma failed\n");
        return HI_FAILURE;
    }
    g_stPreviousPara.stGamma.u8CurveType = stAdptGamma.u8CurveType;
    for (i = 0 ; i < 257; i++)
    {
        g_stPreviousPara.stGamma.u16Table[i] = stAdptGamma.au16GammaTable[i];
    }

    //ccm
    s32Ret = CommSceneautoGetCcmAttr(s32IspDev, &stAdptCcmAttr);
    if (HI_SUCCESS != s32Ret)
    {
        printf("CommSceneautoGetCcmAttr failed\n");
        return HI_FAILURE;
    }
    g_stPreviousPara.stCcm.u16HighColorTemp = stAdptCcmAttr.u16HighColorTemp;
    g_stPreviousPara.stCcm.u16MidColorTemp = stAdptCcmAttr.u16MidColorTemp;
    g_stPreviousPara.stCcm.u16LowColorTemp = stAdptCcmAttr.u16LowColorTemp;
    for (i = 0; i < 9; i++)
    {
        g_stPreviousPara.stCcm.au16HighCCM[i] = stAdptCcmAttr.au16HighCCM[i];
        g_stPreviousPara.stCcm.au16MidCCM[i] = stAdptCcmAttr.au16MidCCM[i];
        g_stPreviousPara.stCcm.au16LowCCM[i] = stAdptCcmAttr.au16LowCCM[i];
    }

    //acm
    s32Ret = CommSceneautoGetAcmAttr(s32IspDev, &stAdptAcmAttr);
    if (HI_SUCCESS != s32Ret)
    {
        printf("CommSceneautoGetAcmAttr failed\n");
        return HI_FAILURE;
    }
    g_stPreviousPara.stAcm.bEnable = stAdptAcmAttr.bEnable;
    g_stPreviousPara.stAcm.u32CbcrThr = stAdptAcmAttr.u32CbcrThr;
    g_stPreviousPara.stAcm.u32GainHue = stAdptAcmAttr.u32GainHue;
    g_stPreviousPara.stAcm.u32GainLuma = stAdptAcmAttr.u32GainLuma;
    g_stPreviousPara.stAcm.u32GainSat = stAdptAcmAttr.u32GainSat;

    //venc
    s32Ret = CommSceneautoGetVencAttr(s32VencChn, &stAdptVencAttr);
    if (HI_SUCCESS != s32Ret)
    {
        printf("CommSceneautoGetVencAttr failed\n");
        return HI_FAILURE;
    }

    switch (stAdptVencAttr.eRcMode)
    {
        case ADPT_SCENEAUTO_RCMODE_H264:
            g_eVencRcMode = ADPT_SCENEAUTO_RCMODE_H264;

            s32Ret = CommSceneautoGetH24Deblock(s32VencChn, &stAdptH264Deblock);
            if (HI_SUCCESS != s32Ret)
            {
                printf("CommSceneautoGetH24Deblock failed\n");
                return HI_FAILURE;
            }
            g_stPreviousPara.stH264Venc.stH264Dblk.disable_deblocking_filter_idc = stAdptH264Deblock.disable_deblocking_filter_idc;
            g_stPreviousPara.stH264Venc.stH264Dblk.slice_alpha_c0_offset_div2 = stAdptH264Deblock.slice_alpha_c0_offset_div2;
            g_stPreviousPara.stH264Venc.stH264Dblk.slice_beta_offset_div2 = stAdptH264Deblock.slice_beta_offset_div2;

            s32Ret = CommSceneautoGetH264Trans(s32VencChn, &stAdptH264Trans);
            if (HI_SUCCESS != s32Ret)
            {
                printf("CommSceneautoGetH264Trans failed\n");
                return HI_FAILURE;
            }
            g_stPreviousPara.stH264Venc.s32chroma_qp_index_offset = stAdptH264Trans.chroma_qp_index_offset;

            s32Ret = CommSceneautoGetH264RcParam(s32VencChn, &stAdptH264RcParam);
            if (HI_SUCCESS != s32Ret)
            {
                printf("CommSceneautoGetRcParam failed\n");
                return HI_FAILURE;
            }
            g_stPreviousPara.stH264Venc.s32IPQPDelta = stAdptH264RcParam.s32IPQPDelta;
            g_stPreviousPara.stH264Venc.u32DeltaQP = stAdptH264RcParam.u32QpDelta;
            for (i = 0; i < 12; i++)
            {
                g_stPreviousPara.stH264Venc.u32ThrdI[i] = stAdptH264RcParam.u32ThrdI[i];
                g_stPreviousPara.stH264Venc.u32ThrdP[i] = stAdptH264RcParam.u32ThrdP[i];
            }

            break;
        case ADPT_SCENEAUTO_RCMODE_H265:
            g_eVencRcMode = ADPT_SCENEAUTO_RCMODE_H265;
            s32Ret = CommSceneautoGetH265RcParam(s32VencChn, &stAdptH265RcParam);
            if (HI_SUCCESS != s32Ret)
            {
                printf("CommSceneautoGetH265RcParam failed\n");
                return HI_FAILURE;
            }
            g_stPreviousPara.stH265Venc.stH265VencRcParam.u32DeltaQP = stAdptH265RcParam.u32QpDelta;
            g_stPreviousPara.stH265Venc.stH265VencRcParam.s32IPQPDelta = stAdptH265RcParam.s32IPQPDelta;
            for (i = 0; i < 12; i++)
            {
                g_stPreviousPara.stH265Venc.stH265VencRcParam.u32ThrdI[i] = stAdptH265RcParam.u32ThrdI[i];
                g_stPreviousPara.stH265Venc.stH265VencRcParam.u32ThrdP[i] = stAdptH265RcParam.u32ThrdP[i];
            }
            s32Ret = CommSceneautoGetH265FaceCfg(s32VencChn, &stAdptH265FaceCfg);
            if (HI_SUCCESS != s32Ret)
            {
                printf("CommSceneautoGetH265FaceCfg failed\n");
                return HI_FAILURE;
            }
            {
                g_stPreviousPara.stH265Venc.stH265VencFaceCfg.u8NormIntra4RdCost_I   = stAdptH265FaceCfg.u8NormIntra4RdCost_I;
                g_stPreviousPara.stH265Venc.stH265VencFaceCfg.u8NormIntra8RdCost_I   = stAdptH265FaceCfg.u8NormIntra8RdCost_I;
                g_stPreviousPara.stH265Venc.stH265VencFaceCfg.u8NormIntra16RdCost_I  = stAdptH265FaceCfg.u8NormIntra16RdCost_I;
                g_stPreviousPara.stH265Venc.stH265VencFaceCfg.u8NormIntra32RdCost_I  = stAdptH265FaceCfg.u8NormIntra32RdCost_I;
                g_stPreviousPara.stH265Venc.stH265VencFaceCfg.u8SkinIntra4RdCost_I   = stAdptH265FaceCfg.u8SkinIntra4RdCost_I;
                g_stPreviousPara.stH265Venc.stH265VencFaceCfg.u8SkinIntra8RdCost_I   = stAdptH265FaceCfg.u8SkinIntra8RdCost_I;
                g_stPreviousPara.stH265Venc.stH265VencFaceCfg.u8SkinIntra16RdCost_I  = stAdptH265FaceCfg.u8SkinIntra16RdCost_I;
                g_stPreviousPara.stH265Venc.stH265VencFaceCfg.u8SkinIntra32RdCost_I  = stAdptH265FaceCfg.u8SkinIntra32RdCost_I;
                g_stPreviousPara.stH265Venc.stH265VencFaceCfg.u8HedgeIntra4RdCost_I  = stAdptH265FaceCfg.u8HedgeIntra4RdCost_I;
                g_stPreviousPara.stH265Venc.stH265VencFaceCfg.u8HedgeIntra8RdCost_I  = stAdptH265FaceCfg.u8HedgeIntra8RdCost_I;
                g_stPreviousPara.stH265Venc.stH265VencFaceCfg.u8HedgeIntra16RdCost_I = stAdptH265FaceCfg.u8HedgeIntra16RdCost_I;
                g_stPreviousPara.stH265Venc.stH265VencFaceCfg.u8HedgeIntra32RdCost_I = stAdptH265FaceCfg.u8HedgeIntra32RdCost_I;
                g_stPreviousPara.stH265Venc.stH265VencFaceCfg.u8NormIntra4RdCost_P   = stAdptH265FaceCfg.u8NormIntra4RdCost_P;
                g_stPreviousPara.stH265Venc.stH265VencFaceCfg.u8NormIntra8RdCost_P   = stAdptH265FaceCfg.u8NormIntra8RdCost_P;
                g_stPreviousPara.stH265Venc.stH265VencFaceCfg.u8NormIntra16RdCost_P  = stAdptH265FaceCfg.u8NormIntra16RdCost_P;
                g_stPreviousPara.stH265Venc.stH265VencFaceCfg.u8NormIntra32RdCost_P  = stAdptH265FaceCfg.u8NormIntra32RdCost_P;
                g_stPreviousPara.stH265Venc.stH265VencFaceCfg.u8SkinIntra4RdCost_P   = stAdptH265FaceCfg.u8SkinIntra4RdCost_P;
                g_stPreviousPara.stH265Venc.stH265VencFaceCfg.u8SkinIntra8RdCost_P   = stAdptH265FaceCfg.u8SkinIntra8RdCost_P;
                g_stPreviousPara.stH265Venc.stH265VencFaceCfg.u8SkinIntra16RdCost_P  = stAdptH265FaceCfg.u8SkinIntra16RdCost_P;
                g_stPreviousPara.stH265Venc.stH265VencFaceCfg.u8SkinIntra32RdCost_P  = stAdptH265FaceCfg.u8SkinIntra32RdCost_P;
                g_stPreviousPara.stH265Venc.stH265VencFaceCfg.u8HedgeIntra4RdCost_P  = stAdptH265FaceCfg.u8HedgeIntra4RdCost_P;
                g_stPreviousPara.stH265Venc.stH265VencFaceCfg.u8HedgeIntra8RdCost_P  = stAdptH265FaceCfg.u8HedgeIntra8RdCost_P;
                g_stPreviousPara.stH265Venc.stH265VencFaceCfg.u8HedgeIntra16RdCost_P = stAdptH265FaceCfg.u8HedgeIntra16RdCost_P;
                g_stPreviousPara.stH265Venc.stH265VencFaceCfg.u8HedgeIntra32RdCost_P = stAdptH265FaceCfg.u8HedgeIntra32RdCost_P;
                g_stPreviousPara.stH265Venc.stH265VencFaceCfg.u8NormFme8RdCost_P     = stAdptH265FaceCfg.u8NormFme8RdCost_P;
                g_stPreviousPara.stH265Venc.stH265VencFaceCfg.u8NormFme16RdCost_P    = stAdptH265FaceCfg.u8NormFme16RdCost_P;
                g_stPreviousPara.stH265Venc.stH265VencFaceCfg.u8NormFme32RdCost_P    = stAdptH265FaceCfg.u8NormFme32RdCost_P;
                g_stPreviousPara.stH265Venc.stH265VencFaceCfg.u8NormFme64RdCost_P    = stAdptH265FaceCfg.u8NormFme64RdCost_P;
                g_stPreviousPara.stH265Venc.stH265VencFaceCfg.u8SkinFme8RdCost_P     = stAdptH265FaceCfg.u8SkinFme8RdCost_P;
                g_stPreviousPara.stH265Venc.stH265VencFaceCfg.u8SkinFme16RdCost_P    = stAdptH265FaceCfg.u8SkinFme16RdCost_P;
                g_stPreviousPara.stH265Venc.stH265VencFaceCfg.u8SkinFme32RdCost_P    = stAdptH265FaceCfg.u8SkinFme32RdCost_P;
                g_stPreviousPara.stH265Venc.stH265VencFaceCfg.u8SkinFme64RdCost_P    = stAdptH265FaceCfg.u8SkinFme64RdCost_P;
                g_stPreviousPara.stH265Venc.stH265VencFaceCfg.u8HedgeFme8RdCost_P    = stAdptH265FaceCfg.u8HedgeFme8RdCost_P;
                g_stPreviousPara.stH265Venc.stH265VencFaceCfg.u8HedgeFme16RdCost_P   = stAdptH265FaceCfg.u8HedgeFme16RdCost_P;
                g_stPreviousPara.stH265Venc.stH265VencFaceCfg.u8HedgeFme32RdCost_P   = stAdptH265FaceCfg.u8HedgeFme32RdCost_P;
                g_stPreviousPara.stH265Venc.stH265VencFaceCfg.u8HedgeFme64RdCost_P   = stAdptH265FaceCfg.u8HedgeFme64RdCost_P;
                g_stPreviousPara.stH265Venc.stH265VencFaceCfg.u8NormMerg8RdCost_P    = stAdptH265FaceCfg.u8NormMerg8RdCost_P;
                g_stPreviousPara.stH265Venc.stH265VencFaceCfg.u8NormMerg16RdCost_P   = stAdptH265FaceCfg.u8NormMerg16RdCost_P;
                g_stPreviousPara.stH265Venc.stH265VencFaceCfg.u8NormMerg32RdCost_P   = stAdptH265FaceCfg.u8NormMerg32RdCost_P;
                g_stPreviousPara.stH265Venc.stH265VencFaceCfg.u8NormMerg64RdCost_P   = stAdptH265FaceCfg.u8NormMerg64RdCost_P;
                g_stPreviousPara.stH265Venc.stH265VencFaceCfg.u8SkinMerg8RdCost_P    = stAdptH265FaceCfg.u8SkinMerg8RdCost_P;
                g_stPreviousPara.stH265Venc.stH265VencFaceCfg.u8SkinMerg16RdCost_P   = stAdptH265FaceCfg.u8SkinMerg16RdCost_P;
                g_stPreviousPara.stH265Venc.stH265VencFaceCfg.u8SkinMerg32RdCost_P   = stAdptH265FaceCfg.u8SkinMerg32RdCost_P;
                g_stPreviousPara.stH265Venc.stH265VencFaceCfg.u8SkinMerg64RdCost_P   = stAdptH265FaceCfg.u8SkinMerg64RdCost_P;
                g_stPreviousPara.stH265Venc.stH265VencFaceCfg.u8HedgeMerg8RdCost_P   = stAdptH265FaceCfg.u8HedgeMerg8RdCost_P;
                g_stPreviousPara.stH265Venc.stH265VencFaceCfg.u8HedgeMerg16RdCost_P  = stAdptH265FaceCfg.u8HedgeMerg16RdCost_P;
                g_stPreviousPara.stH265Venc.stH265VencFaceCfg.u8HedgeMerg32RdCost_P  = stAdptH265FaceCfg.u8HedgeMerg32RdCost_P;
                g_stPreviousPara.stH265Venc.stH265VencFaceCfg.u8HedgeMerg64RdCost_P  = stAdptH265FaceCfg.u8HedgeMerg64RdCost_P;
                g_stPreviousPara.stH265Venc.stH265VencFaceCfg.bSkinEn_I              = stAdptH265FaceCfg.bSkinEn_I;
                g_stPreviousPara.stH265Venc.stH265VencFaceCfg.u32SkinQpDelta_I       = stAdptH265FaceCfg.u32SkinQpDelta_I;
                g_stPreviousPara.stH265Venc.stH265VencFaceCfg.u8SkinUMax_I           = stAdptH265FaceCfg.u8SkinUMax_I;
                g_stPreviousPara.stH265Venc.stH265VencFaceCfg.u8SkinUMin_I           = stAdptH265FaceCfg.u8SkinUMin_I;
                g_stPreviousPara.stH265Venc.stH265VencFaceCfg.u8SkinVMax_I           = stAdptH265FaceCfg.u8SkinVMax_I;
                g_stPreviousPara.stH265Venc.stH265VencFaceCfg.u8SkinVMin_I           = stAdptH265FaceCfg.u8SkinVMin_I;
                g_stPreviousPara.stH265Venc.stH265VencFaceCfg.u32SkinNum_I           = stAdptH265FaceCfg.u32SkinNum_I;
                g_stPreviousPara.stH265Venc.stH265VencFaceCfg.bSkinEn_P              = stAdptH265FaceCfg.bSkinEn_P;
                g_stPreviousPara.stH265Venc.stH265VencFaceCfg.u32SkinQpDelta_P       = stAdptH265FaceCfg.u32SkinQpDelta_P;
                g_stPreviousPara.stH265Venc.stH265VencFaceCfg.u8SkinUMax_P           = stAdptH265FaceCfg.u8SkinUMax_P;
                g_stPreviousPara.stH265Venc.stH265VencFaceCfg.u8SkinUMin_P           = stAdptH265FaceCfg.u8SkinUMin_P;
                g_stPreviousPara.stH265Venc.stH265VencFaceCfg.u8SkinVMax_P           = stAdptH265FaceCfg.u8SkinVMax_P;
                g_stPreviousPara.stH265Venc.stH265VencFaceCfg.u8SkinVMin_P           = stAdptH265FaceCfg.u8SkinVMin_P;
                g_stPreviousPara.stH265Venc.stH265VencFaceCfg.u32SkinNum_P           = stAdptH265FaceCfg.u32SkinNum_P;
                g_stPreviousPara.stH265Venc.stH265VencFaceCfg.u8HedgeThr_I           = stAdptH265FaceCfg.u8HedgeThr_I;
                g_stPreviousPara.stH265Venc.stH265VencFaceCfg.u8HedgeCnt_I           = stAdptH265FaceCfg.u8HedgeCnt_I;
                g_stPreviousPara.stH265Venc.stH265VencFaceCfg.bStroEdgeEn_I          = stAdptH265FaceCfg.bStroEdgeEn_I;
                g_stPreviousPara.stH265Venc.stH265VencFaceCfg.u32StroEdgeQpDelta_I   = stAdptH265FaceCfg.u32StroEdgeQpDelta_I;
                g_stPreviousPara.stH265Venc.stH265VencFaceCfg.u8HedgeThr_P           = stAdptH265FaceCfg.u8HedgeThr_P;
                g_stPreviousPara.stH265Venc.stH265VencFaceCfg.u8HedgeCnt_P           = stAdptH265FaceCfg.u8HedgeCnt_P;
                g_stPreviousPara.stH265Venc.stH265VencFaceCfg.bStroEdgeEn_P          = stAdptH265FaceCfg.bStroEdgeEn_P;
                g_stPreviousPara.stH265Venc.stH265VencFaceCfg.u32StroEdgeQpDelta_P   = stAdptH265FaceCfg.u32StroEdgeQpDelta_P;
                g_stPreviousPara.stH265Venc.stH265VencFaceCfg.bImproveEn_I           = stAdptH265FaceCfg.bImproveEn_I;
                g_stPreviousPara.stH265Venc.stH265VencFaceCfg.bImproveEn_P           = stAdptH265FaceCfg.bImproveEn_P;
                g_stPreviousPara.stH265Venc.stH265VencFaceCfg.u32Norm32MaxNum_P      = stAdptH265FaceCfg.u32Norm32MaxNum_P;
                g_stPreviousPara.stH265Venc.stH265VencFaceCfg.u32Norm16MaxNum_P      = stAdptH265FaceCfg.u32Norm16MaxNum_P;
                g_stPreviousPara.stH265Venc.stH265VencFaceCfg.u32Norm32ProtectNum_P  = stAdptH265FaceCfg.u32Norm32ProtectNum_P;
                g_stPreviousPara.stH265Venc.stH265VencFaceCfg.u32Norm16ProtectNum_P  = stAdptH265FaceCfg.u32Norm16ProtectNum_P;
                g_stPreviousPara.stH265Venc.stH265VencFaceCfg.u32Skin32MaxNum_P      = stAdptH265FaceCfg.u32Skin32MaxNum_P;
                g_stPreviousPara.stH265Venc.stH265VencFaceCfg.u32Skin16MaxNum_P      = stAdptH265FaceCfg.u32Skin16MaxNum_P;
                g_stPreviousPara.stH265Venc.stH265VencFaceCfg.u32Skin32ProtectNum_P  = stAdptH265FaceCfg.u32Skin32ProtectNum_P;
                g_stPreviousPara.stH265Venc.stH265VencFaceCfg.u32Skin16ProtectNum_P  = stAdptH265FaceCfg.u32Skin16ProtectNum_P;
                g_stPreviousPara.stH265Venc.stH265VencFaceCfg.u32Still32MaxNum_P     = stAdptH265FaceCfg.u32Still32MaxNum_P;
                g_stPreviousPara.stH265Venc.stH265VencFaceCfg.u32Still16MaxNum_P     = stAdptH265FaceCfg.u32Still16MaxNum_P;
                g_stPreviousPara.stH265Venc.stH265VencFaceCfg.u32Still32ProtectNum_P = stAdptH265FaceCfg.u32Still32ProtectNum_P;
                g_stPreviousPara.stH265Venc.stH265VencFaceCfg.u32Still16ProtectNum_P = stAdptH265FaceCfg.u32Still16ProtectNum_P;
                g_stPreviousPara.stH265Venc.stH265VencFaceCfg.u32Hedge32MaxNum_P     = stAdptH265FaceCfg.u32Hedge32MaxNum_P;
                g_stPreviousPara.stH265Venc.stH265VencFaceCfg.u32Hedge16MaxNum_P     = stAdptH265FaceCfg.u32Hedge16MaxNum_P;
                g_stPreviousPara.stH265Venc.stH265VencFaceCfg.u32Hedge32ProtectNum_P = stAdptH265FaceCfg.u32Hedge32ProtectNum_P;
                g_stPreviousPara.stH265Venc.stH265VencFaceCfg.u32Hedge16ProtectNum_P = stAdptH265FaceCfg.u32Hedge16ProtectNum_P;
            }
            break;
        default:
            break;
    }

    //AERoute
    s32Ret = CommSceneautoGetAERoute(s32IspDev, &stAdptAERoute);
    if (HI_SUCCESS != s32Ret)
    {
        printf("CommSceneautoGetAERoute failed\n");
        return HI_FAILURE;
    }
    g_stPreviousPara.stAERoute.u32TotalNum = stAdptAERoute.u32TotalNum;
    for (i = 0; i < 8; i++)
    {
        g_stPreviousPara.stAERoute.astRouteNode[i].u32IntTime = stAdptAERoute.astRouteNode[i].u32IntTime;
        g_stPreviousPara.stAERoute.astRouteNode[i].u32SysGain = stAdptAERoute.astRouteNode[i].u32SysGain;
    }

    //DCI
    s32Ret = CommSceneautoGetDCIParam(s32ViDev, &stAdptDCIPara);
    if (HI_SUCCESS != s32Ret)
    {
        printf("CommSceneautoGetDCIParam failed\n");
        return HI_FAILURE;
    }
    g_stPreviousPara.stDCIParam.bEanble = stAdptDCIPara.bEnable;
    g_stPreviousPara.stDCIParam.u32BlackGain = stAdptDCIPara.u32BlackGain;
    g_stPreviousPara.stDCIParam.u32ContrastGain = stAdptDCIPara.u32ContrastGain;
    g_stPreviousPara.stDCIParam.u32LightGain = stAdptDCIPara.u32LightGain;

    //DRC
    s32Ret = CommSceneautoGetDRCAttr(s32IspDev, &stAdptDRCAttr);
    if (HI_SUCCESS != s32Ret)
    {
        printf("CommSceneautoGetDRCAttr failed\n");
        return HI_FAILURE;
    }
    g_stPreviousPara.stDRCAttr.bEnable = stAdptDRCAttr.bEnable;
    g_stPreviousPara.stDRCAttr.bManulEnable = stAdptDRCAttr.bManulEnable;
    g_stPreviousPara.stDRCAttr.s32Strength = stAdptDRCAttr.u32Strength;
    g_stPreviousPara.stDRCAttr.u32SlopeMax = stAdptDRCAttr.u32SlopeMax;
    g_stPreviousPara.stDRCAttr.u32SlopeMin = stAdptDRCAttr.u32SlopeMin;
    g_stPreviousPara.stDRCAttr.u32VarianceSpace = stAdptDRCAttr.u32VarianceSpace;
    g_stPreviousPara.stDRCAttr.u32VarianceIntensity = stAdptDRCAttr.u32VarianceIntensity;
    g_stPreviousPara.stDRCAttr.u32WhiteLevel = stAdptDRCAttr.u32WhiteLevel;
    g_stPreviousPara.stDRCAttr.u32BlackLevel = stAdptDRCAttr.u32BlackLevel;

    //Saturation
    s32Ret = CommSceneautoGetSaturation(s32IspDev, &stAdptSaturation);
    if (HI_SUCCESS != s32Ret)
    {
        printf("CommSceneautoGetDRCAttr failed\n");
        return HI_FAILURE;
    }
    g_stPreviousPara.stSaturation.u8OpType = stAdptSaturation.u8OpType;
    g_stPreviousPara.stSaturation.u8ManualSat = stAdptSaturation.u8ManualSat;
    for (i = 0; i < 16; i++)
    {
        g_stPreviousPara.stSaturation.au8AutoSat[i] = stAdptSaturation.au8AutoSat[i];
    }

    //DIS
    s32Ret = CommSceneautoGetDISAttr(s32IspDev, &stAdptDISAttr);
    if (HI_SUCCESS != s32Ret)
    {
        printf("CommSceneautoGetDISAttr failed\n");
        return HI_FAILURE;
    }
    g_stPreviousPara.stDis.bEnable = stAdptDISAttr.bEnable;

    //AE
    s32Ret = CommSceneautoGetAEAttr(s32IspDev, &stAdptAEAttr);
    if (HI_SUCCESS != s32Ret)
    {
        printf("CommSceneautoGetDRCAttr failed\n");
        return HI_FAILURE;
    }
    g_stPreviousPara.stAEAttr.u8ExpCompensation = stAdptAEAttr.u8ExpCompensation;
    g_stPreviousPara.stAEAttr.u16BlackDelayFrame = stAdptAEAttr.u16BlackDelayFrame;
    g_stPreviousPara.stAEAttr.u16WhiteDelayFrame = stAdptAEAttr.u16WhiteDelayFrame;
    g_stPreviousPara.stAEAttr.u8AEStrategyMode = stAdptAEAttr.u8AEStrategyMode;
    g_stPreviousPara.stAEAttr.u8MaxHistOffset = stAdptAEAttr.u8MaxHistOffset;
    g_stPreviousPara.stAEAttr.u16HistRatioSlope = stAdptAEAttr.u16HistRatioSlope;
    g_stPreviousPara.stAEAttr.u8Speed = stAdptAEAttr.u8Speed;
    g_stPreviousPara.stAEAttr.u8Tolerance = stAdptAEAttr.u8Tolerance;
    g_stPreviousPara.stAEAttr.u32SysGainMax = stAdptAEAttr.u32SysGainMax;
    g_stPreviousPara.stAEAttr.u8AERunInterval = stAdptAEAttr.u8AERunInterval;
    for (i = 0; i < AE_WEIGHT_ROW; i++)
    {
        for (j = 0; j < AE_WEIGHT_COLUMN; j++)
        {
            g_stPreviousPara.stAEAttr.au8AeWeight[i][j] = stAdptAEAttr.au8AeWeight[i][j];
        }
    }
    return HI_SUCCESS;
}

HI_VOID Sceneauto_FreeMem()
{

    if (NULL != g_stINIPara.stIniAE.pu32BitrateThresh)
    {
        free(g_stINIPara.stIniAE.pu32BitrateThresh);
        g_stINIPara.stIniAE.pu32BitrateThresh = NULL;
    }

    if (NULL != g_stINIPara.stIniAE.pu32AEExpDtoLThresh)
    {
        free(g_stINIPara.stIniAE.pu32AEExpDtoLThresh);
        g_stINIPara.stIniAE.pu32AEExpDtoLThresh = NULL;
    }

    if (NULL != g_stINIPara.stIniAE.pu32AEExpLtoDThresh)
    {
        free(g_stINIPara.stIniAE.pu32AEExpLtoDThresh);
        g_stINIPara.stIniAE.pu32AEExpLtoDThresh = NULL;
    }

    if (NULL != g_stINIPara.stIniAE.pstAERelatedBit)
    {
        free(g_stINIPara.stIniAE.pstAERelatedBit);
        g_stINIPara.stIniAE.pstAERelatedBit = NULL;
    }

    if (NULL != g_stINIPara.stIniAE.pstAERelatedExp)
    {
        free(g_stINIPara.stIniAE.pstAERelatedExp);
        g_stINIPara.stIniAE.pstAERelatedExp = NULL;
    }

    if (NULL != g_stINIPara.stIniDemosaic.pu32BitrateThresh)
    {
        free(g_stINIPara.stIniDemosaic.pu32BitrateThresh);
        g_stINIPara.stIniDemosaic.pu32BitrateThresh = NULL;
    }

    if (NULL != g_stINIPara.stIniDemosaic.pu32ExpThresh)
    {
        free(g_stINIPara.stIniDemosaic.pu32ExpThresh);
        g_stINIPara.stIniDemosaic.pu32ExpThresh = NULL;
    }

    if (NULL != g_stINIPara.stIniDemosaic.pstDemosaic)
    {
        free(g_stINIPara.stIniDemosaic.pstDemosaic);
        g_stINIPara.stIniDemosaic.pstDemosaic = NULL;
    }

    if (NULL != g_stINIPara.stIniSharpen.pu32BitrateThresh)
    {
        free(g_stINIPara.stIniSharpen.pu32BitrateThresh);
        g_stINIPara.stIniSharpen.pu32BitrateThresh = NULL;
    }

    if (NULL != g_stINIPara.stIniSharpen.pstSharpen)
    {
        free(g_stINIPara.stIniSharpen.pstSharpen);
        g_stINIPara.stIniSharpen.pstSharpen = NULL;
    }

    if (NULL != g_stINIPara.stIniDP.pu32ExpThresh)
    {
        free(g_stINIPara.stIniDP.pu32ExpThresh);
        g_stINIPara.stIniDP.pu32ExpThresh = NULL;
    }

    if (NULL != g_stINIPara.stIniDP.pstDPAttr)
    {
        free(g_stINIPara.stIniDP.pstDPAttr);
        g_stINIPara.stIniDP.pstDPAttr = NULL;
    }

    if (NULL != g_stINIPara.stIniGamma.pu32ExpThreshDtoL)
    {
        free(g_stINIPara.stIniGamma.pu32ExpThreshDtoL);
        g_stINIPara.stIniGamma.pu32ExpThreshDtoL = NULL;

    }

    if (NULL != g_stINIPara.stIniGamma.pu32ExpThreshLtoD)
    {
        free(g_stINIPara.stIniGamma.pu32ExpThreshLtoD);
        g_stINIPara.stIniGamma.pu32ExpThreshLtoD = NULL;

    }

    if (NULL != g_stINIPara.stIniGamma.pstGamma)
    {
        free(g_stINIPara.stIniGamma.pstGamma);
        g_stINIPara.stIniGamma.pstGamma = NULL;
    }

    if (NULL != g_stINIPara.stIniH264Venc.pu32BitrateThresh)
    {
        free(g_stINIPara.stIniH264Venc.pu32BitrateThresh);
        g_stINIPara.stIniH264Venc.pu32BitrateThresh = NULL;
    }

    if (NULL != g_stINIPara.stIniH264Venc.pstH264Venc)
    {
        free(g_stINIPara.stIniH264Venc.pstH264Venc);
        g_stINIPara.stIniH264Venc.pstH264Venc = NULL;
    }



    if (NULL != g_stINIPara.stFastDynamic.pstRouteNode)
    {
        free(g_stINIPara.stFastDynamic.pstRouteNode);
        g_stINIPara.stFastDynamic.pstRouteNode = NULL;
    }

    if (NULL != g_stINIPara.stNormalDynamic.pstRouteNode)
    {
        free(g_stINIPara.stNormalDynamic.pstRouteNode);
        g_stINIPara.stNormalDynamic.pstRouteNode = NULL;
    }

    if (NULL != g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.pu32ExpThresh)
    {
        free(g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.pu32ExpThresh);
        g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.pu32ExpThresh = NULL;
    }

    if (NULL != g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.pu32BitrateThresh)
    {
        free(g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.pu32BitrateThresh);
        g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.pu32BitrateThresh = NULL;
    }

    if (NULL != g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.pstH265VencFaceCfg)
    {
        free(g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.pstH265VencFaceCfg);
        g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.pstH265VencFaceCfg = NULL;
    }

    if (NULL != g_stINIPara.stIniH265Venc.stIniH265VencRcParam.pu32BitrateThresh)
    {
        free(g_stINIPara.stIniH265Venc.stIniH265VencRcParam.pu32BitrateThresh);
        g_stINIPara.stIniH265Venc.stIniH265VencRcParam.pu32BitrateThresh = NULL;
    }

    if (NULL != g_stINIPara.stIniH265Venc.stIniH265VencRcParam.pstH265VencRcParam)
    {
        free(g_stINIPara.stIniH265Venc.stIniH265VencRcParam.pstH265VencRcParam);
        g_stINIPara.stIniH265Venc.stIniH265VencRcParam.pstH265VencRcParam = NULL;
    }

    if (NULL != g_stINIPara.stIR.pu32ExpThreshHtoL)
    {
        free(g_stINIPara.stIR.pu32ExpThreshHtoL);
        g_stINIPara.stIR.pu32ExpThreshHtoL = NULL;
    }

    if (NULL != g_stINIPara.stIR.pu32ExpThreshLtoH)
    {
        free(g_stINIPara.stIR.pu32ExpThreshLtoH);
        g_stINIPara.stIR.pu32ExpThreshLtoH = NULL;
    }

    if (NULL != g_stINIPara.stIR.pu8ExpCompensation)
    {
        free(g_stINIPara.stIR.pu8ExpCompensation);
        g_stINIPara.stIR.pu8ExpCompensation = NULL;
    }

    if (NULL != g_stINIPara.stIR.pu8MaxHistOffset)
    {
        free(g_stINIPara.stIR.pu8MaxHistOffset);
        g_stINIPara.stIR.pu8MaxHistOffset = NULL;
    }

    if (NULL != g_stINIPara.stIR.pu323DnrIsoThresh)
    {
        free(g_stINIPara.stIR.pu323DnrIsoThresh);
        g_stINIPara.stIR.pu323DnrIsoThresh = NULL;
    }

    if (NULL != g_stINIPara.stIR.pst3dnrParam)
    {
        free(g_stINIPara.stIR.pst3dnrParam);
        g_stINIPara.stIR.pst3dnrParam = NULL;
    }
    
    if (NULL != g_stINIPara.stHLC.pu323DnrIsoThresh)
    {
        free(g_stINIPara.stHLC.pu323DnrIsoThresh);
        g_stINIPara.stHLC.pu323DnrIsoThresh = NULL;
    }
    if (NULL != g_stINIPara.stHLC.pst3dnrParam)
    {
        free(g_stINIPara.stHLC.pst3dnrParam);
        g_stINIPara.stHLC.pst3dnrParam = NULL;
    }

    if (NULL != g_stINIPara.stIni3dnr.pu323DnrIsoThresh)
    {
        free(g_stINIPara.stIni3dnr.pu323DnrIsoThresh);
        g_stINIPara.stIni3dnr.pu323DnrIsoThresh = NULL;
    }

    if (NULL != g_stINIPara.stIni3dnr.pst3dnrParam)
    {
        free(g_stINIPara.stIni3dnr.pst3dnrParam);
        g_stINIPara.stIni3dnr.pst3dnrParam = NULL;
    }
}

HI_S32 Sceneauto_LoadINIPara()
{
    HI_S32 s32Temp = 0;
    HI_S32 s32Offset = 0;
    HI_S32 i, j;
    HI_CHAR szTempStr[128];
    HI_CHAR* pszTempStr;

    /**************common:IspDev**************/
    s32Temp = 0;
    s32Temp = iniparser_getint(g_Sceneautodictionary, "common:IspDev", HI_FAILURE);
    if (HI_FAILURE == s32Temp)
    {
        printf("common:IspDev failed\n");
        return HI_FAILURE;
    }
    g_stINIPara.stMpInfo.s32IspDev = s32Temp;

    /**************common:ViDev**************/
    s32Temp = 0;
    s32Temp = iniparser_getint(g_Sceneautodictionary, "common:ViDev", HI_FAILURE);
    if (HI_FAILURE == s32Temp)
    {
        printf("common:ViDev failed\n");
        return HI_FAILURE;
    }
    g_stINIPara.stMpInfo.s32ViDev = s32Temp;

    /**************common:ViChn**************/
    s32Temp = 0;
    s32Temp = iniparser_getint(g_Sceneautodictionary, "common:ViChn", HI_FAILURE);
    if (HI_FAILURE == s32Temp)
    {
        printf("common:ViChn failed\n");
        return HI_FAILURE;
    }
    g_stINIPara.stMpInfo.s32ViChn = s32Temp;

    /**************common:VpssGrp**************/
    s32Temp = 0;
    s32Temp = iniparser_getint(g_Sceneautodictionary, "common:VpssGrp", HI_FAILURE);
    if (HI_FAILURE == s32Temp)
    {
        printf("common:VpssGrp failed\n");
        return HI_FAILURE;
    }
    g_stINIPara.stMpInfo.s32VpssGrp = s32Temp;

    /**************common:VpssChn**************/
    s32Temp = 0;
    s32Temp = iniparser_getint(g_Sceneautodictionary, "common:VpssChn", HI_FAILURE);
    if (HI_FAILURE == s32Temp)
    {
        printf("common:VpssChn failed\n");
        return HI_FAILURE;
    }
    g_stINIPara.stMpInfo.s32VpssChn = s32Temp;

    /**************common:VencGrp**************/
    s32Temp = 0;
    s32Temp = iniparser_getint(g_Sceneautodictionary, "common:VencGrp", HI_FAILURE);
    if (HI_FAILURE == s32Temp)
    {
        printf("common:VencGrp failed\n");
        return HI_FAILURE;
    }
    g_stINIPara.stMpInfo.s32VencGrp = s32Temp;

    /**************common:VencChn**************/
    s32Temp = 0;
    s32Temp = iniparser_getint(g_Sceneautodictionary, "common:VencChn", HI_FAILURE);
    if (HI_FAILURE == s32Temp)
    {
        printf("common:VencChn failed\n");
        return HI_FAILURE;
    }
    g_stINIPara.stMpInfo.s32VencChn = s32Temp;

    /**************common:IVE_Enable**************/
    s32Temp = 0;
    s32Temp = iniparser_getint(g_Sceneautodictionary, "common:IVE_Enable", HI_FAILURE);
    if (HI_FAILURE == s32Temp)
    {
        printf("common:IVE_Enable failed\n");
        return HI_FAILURE;
    }
    g_stINIPara.stThreshValue.bIVEEnable = (HI_BOOL)s32Temp;
    /**************common:HLC_AutoEnable**************/
    s32Temp = 0;
    s32Temp = iniparser_getint(g_Sceneautodictionary, "common:HLC_AutoEnable", HI_FAILURE);
    if (HI_FAILURE == s32Temp)
    {
        printf("common:HLC_AutoEnable failed\n");
        return HI_FAILURE;
    }
    g_stINIPara.stThreshValue.bHLCAutoEnable = (HI_BOOL)s32Temp;

    /**************common:HLC_thr_on**************/
    s32Temp = 0;
    s32Temp = iniparser_getint(g_Sceneautodictionary, "common:HLC_thr_on", HI_FAILURE);
    if (HI_FAILURE == s32Temp)
    {
        printf("common:HLC_thr_on failed\n");
        return HI_FAILURE;
    }
    g_stINIPara.stThreshValue.u32HLCOnThresh = s32Temp;

    /**************common:HLC_thr_off**************/
    s32Temp = 0;
    s32Temp = iniparser_getint(g_Sceneautodictionary, "common:HLC_thr_off", HI_FAILURE);
    if (HI_FAILURE == s32Temp)
    {
        printf("common:HLC_thr_off failed\n");
        return HI_FAILURE;
    }
    g_stINIPara.stThreshValue.u32HLCOffThresh = s32Temp;

    /**************common:HLC_tolerance**************/
    s32Temp = 0;
    s32Temp = iniparser_getint(g_Sceneautodictionary, "common:HLC_tolerance", HI_FAILURE);
    if (HI_FAILURE == s32Temp)
    {
        printf("common:HLC_tolerance failed\n");
        return HI_FAILURE;
    }
    g_stINIPara.stThreshValue.u32HLCTolerance = s32Temp;

    /**************common:HLC_expthr**************/
    s32Temp = 0;
    s32Temp = iniparser_getint(g_Sceneautodictionary, "common:HLC_expthr", HI_FAILURE);
    if (HI_FAILURE == s32Temp)
    {
        printf("common:HLC_expthr failed\n");
        return HI_FAILURE;
    }
    g_stINIPara.stThreshValue.u32HLCExpThresh = s32Temp;

    /**************common:HLC_count**************/
    s32Temp = 0;
    s32Temp = iniparser_getint(g_Sceneautodictionary, "common:HLC_count", HI_FAILURE);
    if (HI_FAILURE == s32Temp)
    {
        printf("common:HLC_count failed\n");
        return HI_FAILURE;
    }
    g_stINIPara.stThreshValue.u32HLCCount = s32Temp;

    /**************common:ave_lum_thresh**************/
    s32Temp = 0;
    s32Temp = iniparser_getint(g_Sceneautodictionary, "common:ave_lum_thresh", HI_FAILURE);
    if (HI_FAILURE == s32Temp)
    {
        printf("common:ave_lum_thresh failed\n");
        return HI_FAILURE;
    }
    g_stINIPara.stThreshValue.u32AveLumThresh = s32Temp;

    /**************common:delta_dis_expthresh**************/
    s32Temp = 0;
    s32Temp = iniparser_getint(g_Sceneautodictionary, "common:delta_dis_expthresh", HI_FAILURE);
    if (HI_FAILURE == s32Temp)
    {
        printf("common:delta_dis_expthresh failed\n");
        return HI_FAILURE;
    }
    g_stINIPara.stThreshValue.u32DeltaDisExpThreash = s32Temp;

    /**************common:fpn_exp_thresh**************/
    s32Temp = 0;
    s32Temp = iniparser_getint(g_Sceneautodictionary, "common:fpn_exp_thresh", HI_FAILURE);
    if (HI_FAILURE == s32Temp)
    {
        printf("common:fpn_exp_thresh failed\n");
        return HI_FAILURE;
    }
    g_stINIPara.stThreshValue.u32FpnExpThresh = s32Temp;

    /**************common:dci_strength_lut**************/
    pszTempStr = iniparser_getstr(g_Sceneautodictionary, "common:dci_strength_lut");
    if (NULL == pszTempStr)
    {
        printf("common:dci_strength_lut error\n");
        return HI_FAILURE;
    }
    s32Temp = Weight(pszTempStr);
    for (i = 0; i < s32Temp; i++)
    {
        g_stINIPara.au8DciStrengthLut[i] = MAEWeight[i];
    }

    /**************common:u32DRCStrengthThresh**************/
    s32Temp = 0;
    s32Temp = iniparser_getint(g_Sceneautodictionary, "common:u32DRCStrengthThresh", HI_FAILURE);
    if (HI_FAILURE == s32Temp)
    {
        printf("common:u32DRCStrengthThresh failed\n");
        return HI_FAILURE;
    }
    g_stINIPara.stThreshValue.u32DRCStrengthThresh = s32Temp;

    /****************nrproflie****************/
    //nrprofile:nplut_default_1
    pszTempStr = iniparser_getstr(g_Sceneautodictionary, "nrprofile:nplut_default_1");
    if (NULL == pszTempStr)
    {
        printf("nrprofile:nplut_default_1 error\n");
        return HI_FAILURE;
    }
    s32Temp = Weight(pszTempStr);
    for (i = 0; i < 128; i++)
    {
        g_stINIPara.stIniNRProfile.au8NpDefault1[i] = MAEWeight[i];
    }

    //nrprofile:nplut_default_2
    pszTempStr = iniparser_getstr(g_Sceneautodictionary, "nrprofile:nplut_default_2");
    if (NULL == pszTempStr)
    {
        printf("nrprofile:nplut_default_2 error\n");
        return HI_FAILURE;
    }
    s32Temp = Weight(pszTempStr);
    for (i = 0; i < 128; i++)
    {
        g_stINIPara.stIniNRProfile.au8NpDefault2[i] = MAEWeight[i];
    }

    //nrprofile:nplut_1
    pszTempStr = iniparser_getstr(g_Sceneautodictionary, "nrprofile:nplut_1");
    if (NULL == pszTempStr)
    {
        printf("nrprofile:nplut_1 error\n");
        return HI_FAILURE;
    }
    s32Temp = Weight(pszTempStr);
    for (i = 0; i < 128; i++)
    {
        g_stINIPara.stIniNRProfile.au8Np1[i] = MAEWeight[i];
    }

    //nrprofile:nplut_2
    pszTempStr = iniparser_getstr(g_Sceneautodictionary, "nrprofile:nplut_2");
    if (NULL == pszTempStr)
    {
        printf("nrprofile:nplut_2 error\n");
        return HI_FAILURE;
    }
    s32Temp = Weight(pszTempStr);
    for (i = 0; i < 128; i++)
    {
        g_stINIPara.stIniNRProfile.au8Np2[i] = MAEWeight[i];
    }

    //nrprofile:explow
    s32Temp = 0;
    s32Temp = iniparser_getint(g_Sceneautodictionary, "nrprofile:explow", HI_FAILURE);
    if (HI_FAILURE == s32Temp)
    {
        printf("nrprofile:explow failed\n");
        return HI_FAILURE;
    }
    g_stINIPara.stIniNRProfile.u32ExpLow = s32Temp;

    //nrprofile:exphigh
    s32Temp = 0;
    s32Temp = iniparser_getint(g_Sceneautodictionary, "nrprofile:exphigh", HI_FAILURE);
    if (HI_FAILURE == s32Temp)
    {
        printf("nrprofile:exphigh failed\n");
        return HI_FAILURE;
    }
    g_stINIPara.stIniNRProfile.u32ExpHigh = s32Temp;

    /**************AE**************/
    //AE:aeRunInterval
    s32Temp = 0;
    s32Temp = iniparser_getint(g_Sceneautodictionary, "AE:aeRunInterval", HI_FAILURE);
    if (HI_FAILURE == s32Temp)
    {
        printf("AE:aeRunInterval failed\n");
        return HI_FAILURE;
    }
    g_stINIPara.stIniAE.u8AERunInterval = s32Temp;
    //AE:aeBitrateCount
    s32Temp = 0;
    s32Temp = iniparser_getint(g_Sceneautodictionary, "AE:aeBitrateCount", HI_FAILURE);
    if (HI_FAILURE == s32Temp)
    {
        printf("AE:aeBitrateCount failed\n");
        return HI_FAILURE;
    }
    g_stINIPara.stIniAE.s32BitrateCount = s32Temp;
    g_stINIPara.stIniAE.pu32BitrateThresh = (HI_U32*)malloc((g_stINIPara.stIniAE.s32BitrateCount) * sizeof(HI_U32));
    CHECK_NULL_PTR(g_stINIPara.stIniAE.pu32BitrateThresh);
    g_stINIPara.stIniAE.pstAERelatedBit = (SCENEAUTO_AERELATEDBIT_S*)malloc((g_stINIPara.stIniAE.s32BitrateCount) * sizeof(SCENEAUTO_AERELATEDBIT_S));
    CHECK_NULL_PTR(g_stINIPara.stIniAE.pstAERelatedBit);

    //AE:aeBitrateThresh
    pszTempStr = iniparser_getstr(g_Sceneautodictionary, "AE:aeBitrateThresh");
    if (NULL == pszTempStr)
    {
        printf("AE:aeBitrateThresh error\n");
        return HI_FAILURE;
    }
    s32Temp = Weight(pszTempStr);
    for (i = 0; i < g_stINIPara.stIniAE.s32BitrateCount; i++)
    {
        g_stINIPara.stIniAE.pu32BitrateThresh[i] = MAEWeight[i];
    }

    for (i = 0; i < g_stINIPara.stIniAE.s32BitrateCount; i++)
    {
        //AE:u8Speed
        snprintf(szTempStr, sizeof(szTempStr), "AE:u8Speed_%d", i);
        s32Temp = 0;
        s32Temp = iniparser_getint(g_Sceneautodictionary, szTempStr, HI_FAILURE);
        if (HI_FAILURE == s32Temp)
        {
            printf("AE:u8Speed_%d failed\n", i);
            return HI_FAILURE;
        }
        g_stINIPara.stIniAE.pstAERelatedBit[i].u8Speed = s32Temp;

        //AE:u8Tolerance
        snprintf(szTempStr, sizeof(szTempStr), "AE:u8Tolerance_%d", i);
        s32Temp = 0;
        s32Temp = iniparser_getint(g_Sceneautodictionary, szTempStr, HI_FAILURE);
        if (HI_FAILURE == s32Temp)
        {
            printf("AE:u8Tolerance_%d failed\n", i);
            return HI_FAILURE;
        }
        g_stINIPara.stIniAE.pstAERelatedBit[i].u8Tolerance = s32Temp;

        //AE:u16BlackDelayFrame
        snprintf(szTempStr, sizeof(szTempStr), "AE:u16BlackDelayFrame_%d", i);
        s32Temp = 0;
        s32Temp = iniparser_getint(g_Sceneautodictionary, szTempStr, HI_FAILURE);
        if (HI_FAILURE == s32Temp)
        {
            printf("AE:u16BlackDelayFrame_%d failed\n", i);
            return HI_FAILURE;
        }
        g_stINIPara.stIniAE.pstAERelatedBit[i].u16BlackDelayFrame = s32Temp;

        //AE:u16WhiteDelayFrame
        snprintf(szTempStr, sizeof(szTempStr), "AE:u16WhiteDelayFrame_%d", i);
        s32Temp = 0;
        s32Temp = iniparser_getint(g_Sceneautodictionary, szTempStr, HI_FAILURE);
        if (HI_FAILURE == s32Temp)
        {
            printf("AE:u16WhiteDelayFrame_%d failed\n", i);
            return HI_FAILURE;
        }
        g_stINIPara.stIniAE.pstAERelatedBit[i].u16WhiteDelayFrame = s32Temp;
        snprintf(szTempStr, sizeof(szTempStr), "AE:u32SysGainMax_%d", i);
        s32Temp = 0;
        s32Temp = iniparser_getint(g_Sceneautodictionary, szTempStr, HI_FAILURE);
        if (HI_FAILURE == s32Temp)
        {
            printf("AE:u32SysGainMax_%d failed\n", i);
            return HI_FAILURE;
        }
        g_stINIPara.stIniAE.pstAERelatedBit[i].u32SysGainMax = s32Temp;
    }
    //AE:aeExpCount
    s32Temp = 0;
    s32Temp = iniparser_getint(g_Sceneautodictionary, "AE:aeExpCount", HI_FAILURE);
    if (HI_FAILURE == s32Temp)
    {
        printf("AE:aeExpCount failed\n");
        return HI_FAILURE;
    }
    g_stINIPara.stIniAE.s32ExpCount = s32Temp;
    g_stINIPara.stIniAE.pu32AEExpDtoLThresh = (HI_U32*)malloc((g_stINIPara.stIniAE.s32ExpCount) * sizeof(HI_U32));
    CHECK_NULL_PTR(g_stINIPara.stIniAE.pu32AEExpDtoLThresh);
    g_stINIPara.stIniAE.pu32AEExpLtoDThresh = (HI_U32*)malloc((g_stINIPara.stIniAE.s32ExpCount) * sizeof(HI_U32));
    CHECK_NULL_PTR(g_stINIPara.stIniAE.pu32AEExpLtoDThresh);
    g_stINIPara.stIniAE.pstAERelatedExp = (SCENEAUTO_AERELATEDEXP_S*)malloc((g_stINIPara.stIniAE.s32ExpCount) * sizeof(SCENEAUTO_AERELATEDEXP_S));
    CHECK_NULL_PTR(g_stINIPara.stIniAE.pstAERelatedExp );
    //AE:aeExpDtoLThresh
    pszTempStr = iniparser_getstr(g_Sceneautodictionary, "AE:aeExpDtoLThresh");
    if (NULL == pszTempStr)
    {
        printf("AE:aeExpDtoLThresh error\n");
        return HI_FAILURE;
    }
    s32Temp = Weight(pszTempStr);
    for (i = 0; i < g_stINIPara.stIniAE.s32ExpCount; i++)
    {
        g_stINIPara.stIniAE.pu32AEExpDtoLThresh[i] = MAEWeight[i];
    }
    //AE:aeExpLtoDThresh
    pszTempStr = iniparser_getstr(g_Sceneautodictionary, "AE:aeExpLtoDThresh");
    if (NULL == pszTempStr)
    {
        printf("AE:aeExpLtoDThresh error\n");
        return HI_FAILURE;
    }
    s32Temp = Weight(pszTempStr);
    for (i = 0; i < g_stINIPara.stIniAE.s32ExpCount; i++)
    {
        g_stINIPara.stIniAE.pu32AEExpLtoDThresh[i] = MAEWeight[i];
    }
    //AE:aeCompesation
    pszTempStr = iniparser_getstr(g_Sceneautodictionary, "AE:aeCompesation");
    if (NULL == pszTempStr)
    {
        printf("AE:aeCompesation error\n");
        return HI_FAILURE;
    }
    s32Temp = Weight(pszTempStr);
    for (i = 0; i < g_stINIPara.stIniAE.s32ExpCount; i++)
    {
        g_stINIPara.stIniAE.pstAERelatedExp[i].u8AECompesation = MAEWeight[i];
    }
    //AE:aeHistOffset
    pszTempStr = iniparser_getstr(g_Sceneautodictionary, "AE:aeHistOffset");
    if (NULL == pszTempStr)
    {
        printf("AE:aeHistOffset error\n");
        return HI_FAILURE;
    }
    s32Temp = Weight(pszTempStr);
    for (i = 0; i < g_stINIPara.stIniAE.s32ExpCount; i++)
    {
        g_stINIPara.stIniAE.pstAERelatedExp[i].u8AEHistOffset = MAEWeight[i];
    }
    /**************demosaic**************/
    //demosaic:BitrateCount
    s32Temp = 0;
    s32Temp = iniparser_getint(g_Sceneautodictionary, "demosaic:BitrateCount", HI_FAILURE);
    if (HI_FAILURE == s32Temp)
    {
        printf("demosaic:BitrateCount failed\n");
        return HI_FAILURE;
    }
    g_stINIPara.stIniDemosaic.s32BitrateCount = s32Temp;
    g_stINIPara.stIniDemosaic.pu32BitrateThresh = (HI_U32*)malloc((g_stINIPara.stIniDemosaic.s32BitrateCount) * sizeof(HI_U32));
    CHECK_NULL_PTR(g_stINIPara.stIniDemosaic.pu32BitrateThresh);
    //demosaic:BitrateThresh
    pszTempStr = iniparser_getstr(g_Sceneautodictionary, "demosaic:BitrateThresh");
    if (NULL == pszTempStr)
    {
        printf("demosaic:BitrateThresh error\n");
        return HI_FAILURE;
    }
    s32Temp = Weight(pszTempStr);
    for (i = 0; i < g_stINIPara.stIniDemosaic.s32BitrateCount; i++)
    {
        g_stINIPara.stIniDemosaic.pu32BitrateThresh[i] = MAEWeight[i];
    }
    //demosaic:ExpCount
    s32Temp = 0;
    s32Temp = iniparser_getint(g_Sceneautodictionary, "demosaic:ExpCount", HI_FAILURE);
    if (HI_FAILURE == s32Temp)
    {
        printf("demosaic:ExpCount failed\n");
        return HI_FAILURE;
    }
    g_stINIPara.stIniDemosaic.s32ExpCount = s32Temp;
    g_stINIPara.stIniDemosaic.pu32ExpThresh = (HI_U32*)malloc((g_stINIPara.stIniDemosaic.s32ExpCount) * sizeof(HI_U32));
    CHECK_NULL_PTR(g_stINIPara.stIniDemosaic.pu32ExpThresh);
    //demosaic:ExpThresh
    pszTempStr = iniparser_getstr(g_Sceneautodictionary, "demosaic:ExpThresh");
    if (NULL == pszTempStr)
    {
        printf("demosaic:ExpThresh error\n");
        return HI_FAILURE;
    }
    s32Temp = Weight(pszTempStr);
    for (i = 0; i < g_stINIPara.stIniDemosaic.s32ExpCount; i++)
    {
        g_stINIPara.stIniDemosaic.pu32ExpThresh[i] = MAEWeight[i];
    }

    g_stINIPara.stIniDemosaic.pstDemosaic = (SCENEAUTO_DEMOSAIC_S*)malloc((g_stINIPara.stIniDemosaic.s32BitrateCount) * (g_stINIPara.stIniDemosaic.s32ExpCount) * sizeof(SCENEAUTO_DEMOSAIC_S));
    CHECK_NULL_PTR(g_stINIPara.stIniDemosaic.pstDemosaic);

    for (i = 0; i < g_stINIPara.stIniDemosaic.s32BitrateCount; i++)
    {
        //demosaic:UuSlpoe
        snprintf(szTempStr, sizeof(szTempStr), "demosaic:UuSlpoe_%d", i);
        pszTempStr = iniparser_getstr(g_Sceneautodictionary, szTempStr);
        if (NULL == pszTempStr)
        {
            printf("demosaic:UuSlpoe_%d error\n", i);
            return HI_FAILURE;
        }
        s32Temp = Weight(pszTempStr);
        for (j = 0; j < g_stINIPara.stIniDemosaic.s32ExpCount; j++)
        {
            g_stINIPara.stIniDemosaic.pstDemosaic[i * g_stINIPara.stIniDemosaic.s32ExpCount + j].u8UuSlope = MAEWeight[j];
        }

        //demosaic:VaSlope
        snprintf(szTempStr, sizeof(szTempStr), "demosaic:VaSlope_%d", i);
        pszTempStr = iniparser_getstr(g_Sceneautodictionary, szTempStr);
        if (NULL == pszTempStr)
        {
            printf("demosaic:VaSlope_%d error\n", i);
            return HI_FAILURE;
        }
        s32Temp = Weight(pszTempStr);
        for (j = 0; j < g_stINIPara.stIniDemosaic.s32ExpCount; j++)
        {
            g_stINIPara.stIniDemosaic.pstDemosaic[i * g_stINIPara.stIniDemosaic.s32ExpCount + j].u8VaSlope = MAEWeight[j];
        }

        //demosaic:AaSlope
        snprintf(szTempStr, sizeof(szTempStr), "demosaic:AaSlope_%d", i);
        pszTempStr = iniparser_getstr(g_Sceneautodictionary, szTempStr);
        if (NULL == pszTempStr)
        {
            printf("demosaic:AaSlope_%d error\n", i);
            return HI_FAILURE;
        }
        s32Temp = Weight(pszTempStr);
        for (j = 0; j < g_stINIPara.stIniDemosaic.s32ExpCount; j++)
        {
            g_stINIPara.stIniDemosaic.pstDemosaic[i * g_stINIPara.stIniDemosaic.s32ExpCount + j].u8AaSlope = MAEWeight[j];
        }

        //demosaic:VhSlope
        snprintf(szTempStr, sizeof(szTempStr), "demosaic:VhSlope_%d", i);
        pszTempStr = iniparser_getstr(g_Sceneautodictionary, szTempStr);
        if (NULL == pszTempStr)
        {
            printf("demosaic:VhSlope_%d error\n", i);
            return HI_FAILURE;
        }
        s32Temp = Weight(pszTempStr);
        for (j = 0; j < g_stINIPara.stIniDemosaic.s32ExpCount; j++)
        {
            g_stINIPara.stIniDemosaic.pstDemosaic[i * g_stINIPara.stIniDemosaic.s32ExpCount + j].u8VhSlope = MAEWeight[j];
        }
    }
    /**************sharpen**************/
    //sharpen:sharpenBitrateCount
    s32Temp = 0;
    s32Temp = iniparser_getint(g_Sceneautodictionary, "sharpen:sharpenBitrateCount", HI_FAILURE);
    if (HI_FAILURE == s32Temp)
    {
        printf("sharpen:sharpenBitrateCount failed\n");
        return HI_FAILURE;
    }
    g_stINIPara.stIniSharpen.s32BitrateCount = s32Temp;
    g_stINIPara.stIniSharpen.pu32BitrateThresh = (HI_U32*)malloc((g_stINIPara.stIniSharpen.s32BitrateCount) * sizeof(HI_U32));
    CHECK_NULL_PTR(g_stINIPara.stIniSharpen.pu32BitrateThresh);

    //sharpen:sharpenBitrateThresh
    pszTempStr = iniparser_getstr(g_Sceneautodictionary, "sharpen:sharpenBitrateThresh");
    if (NULL == pszTempStr)
    {
        printf("sharpen:sharpenBitrateThresh error\n");
        return HI_FAILURE;
    }
    s32Temp = Weight(pszTempStr);
    for (i = 0; i < g_stINIPara.stIniSharpen.s32BitrateCount; i++)
    {
        g_stINIPara.stIniSharpen.pu32BitrateThresh[i] = MAEWeight[i];
    }

    g_stINIPara.stIniSharpen.pstSharpen = (SCENEAUTO_SHARPEN_S*)malloc((g_stINIPara.stIniSharpen.s32BitrateCount) * sizeof(SCENEAUTO_SHARPEN_S));
    CHECK_NULL_PTR(g_stINIPara.stIniSharpen.pstSharpen);

    for (i = 0; i < g_stINIPara.stIniSharpen.s32BitrateCount; i++)
    {
        //sharpen:SharpenD
        snprintf(szTempStr, sizeof(szTempStr), "sharpen:SharpenD_%d", i);
        pszTempStr = iniparser_getstr(g_Sceneautodictionary, szTempStr);
        if (NULL == pszTempStr)
        {
            printf("sharpen:SharpenD_%d error\n", i);
            return HI_FAILURE;
        }
        s32Temp = Weight(pszTempStr);
        for (j = 0; j < EXPOSURE_LEVEL; j++)
        {
            g_stINIPara.stIniSharpen.pstSharpen[i].au8SharpenD[j] = MAEWeight[j];
        }

        //sharpen:SharpenRGB
        snprintf(szTempStr, sizeof(szTempStr), "sharpen:SharpenRGB_%d", i);
        pszTempStr = iniparser_getstr(g_Sceneautodictionary, szTempStr);
        if (NULL == pszTempStr)
        {
            printf("sharpen:SharpenRGB_%d error\n", i);
            return HI_FAILURE;
        }
        s32Temp = Weight(pszTempStr);
        for (j = 0; j < EXPOSURE_LEVEL; j++)
        {
            g_stINIPara.stIniSharpen.pstSharpen[i].au8SharpenRGB[j] = MAEWeight[j];
        }

        //sharpen:SharpenUd
        snprintf(szTempStr, sizeof(szTempStr), "sharpen:SharpenUd_%d", i);
        pszTempStr = iniparser_getstr(g_Sceneautodictionary, szTempStr);
        if (NULL == pszTempStr)
        {
            printf("sharpen:SharpenUd_%d error\n", i);
            return HI_FAILURE;
        }
        s32Temp = Weight(pszTempStr);
        for (j = 0; j < EXPOSURE_LEVEL; j++)
        {
            g_stINIPara.stIniSharpen.pstSharpen[i].au8SharpenUd[j] = MAEWeight[j];
        }
    }
    /**************dp**************/
    //dp:ExpCount
    s32Temp = 0;
    s32Temp = iniparser_getint(g_Sceneautodictionary, "dp:ExpCount", HI_FAILURE);
    if (HI_FAILURE == s32Temp)
    {
        printf("dp:ExpCount failed\n");
        return HI_FAILURE;
    }
    g_stINIPara.stIniDP.s32ExpCount = s32Temp;
    g_stINIPara.stIniDP.pu32ExpThresh = (HI_U32*)malloc((g_stINIPara.stIniDP.s32ExpCount) * sizeof(HI_U32));
    CHECK_NULL_PTR(g_stINIPara.stIniDP.pu32ExpThresh);
    g_stINIPara.stIniDP.pstDPAttr = (SCENEAUTO_DEPATTR_S*)malloc((g_stINIPara.stIniDP.s32ExpCount) * sizeof(SCENEAUTO_DEPATTR_S));
    CHECK_NULL_PTR(g_stINIPara.stIniDP.pstDPAttr);
    //dp:ExpThresh
    pszTempStr = iniparser_getstr(g_Sceneautodictionary, "dp:ExpThresh");
    if (NULL == pszTempStr)
    {
        printf("dp:ExpThresh error\n");
        return HI_FAILURE;
    }
    s32Temp = Weight(pszTempStr);
    for (i = 0; i < g_stINIPara.stIniDP.s32ExpCount; i++)
    {
        g_stINIPara.stIniDP.pu32ExpThresh[i] = MAEWeight[i];
    }
    //dp:u16Slop
    pszTempStr = iniparser_getstr(g_Sceneautodictionary, "dp:u16Slop");
    if (NULL == pszTempStr)
    {
        printf("dp:u16Slop error\n");
        return HI_FAILURE;
    }
    s32Temp = Weight(pszTempStr);
    for (i = 0; i < g_stINIPara.stIniDP.s32ExpCount; i++)
    {
        g_stINIPara.stIniDP.pstDPAttr[i].u16Slope = MAEWeight[i];
    }
    /**************gamma**************/
    //gamma:DelayCount
    s32Temp = 0;
    s32Temp = iniparser_getint(g_Sceneautodictionary, "gamma:DelayCount", HI_FAILURE);
    if (HI_FAILURE == s32Temp)
    {
        printf("gamma:DelayCount failed\n");
        return HI_FAILURE;
    }
    g_stINIPara.stIniGamma.u32DelayCount = s32Temp;

    //gamma:Interval
    s32Temp = 0;
    s32Temp = iniparser_getint(g_Sceneautodictionary, "gamma:Interval", HI_FAILURE);
    if (HI_FAILURE == s32Temp)
    {
        printf("gamma:Interval failed\n");
        return HI_FAILURE;
    }
    g_stINIPara.stIniGamma.s32Interval = s32Temp;

    //gamma:ExpCount
    s32Temp = 0;
    s32Temp = iniparser_getint(g_Sceneautodictionary, "gamma:ExpCount", HI_FAILURE);
    if (HI_FAILURE == s32Temp)
    {
        printf("gamma:ExpCount failed\n");
        return HI_FAILURE;
    }
    g_stINIPara.stIniGamma.s32ExpCount = s32Temp;

    g_stINIPara.stIniGamma.pu32ExpThreshLtoD = (HI_U32*)malloc((g_stINIPara.stIniGamma.s32ExpCount) * sizeof(HI_U32));
    CHECK_NULL_PTR(g_stINIPara.stIniGamma.pu32ExpThreshLtoD);
    g_stINIPara.stIniGamma.pu32ExpThreshDtoL = (HI_U32*)malloc((g_stINIPara.stIniGamma.s32ExpCount) * sizeof(HI_U32));
    CHECK_NULL_PTR(g_stINIPara.stIniGamma.pu32ExpThreshDtoL);
    g_stINIPara.stIniGamma.pstGamma = (SCENEAUTO_GAMMA_S*)malloc((g_stINIPara.stIniGamma.s32ExpCount) * sizeof(SCENEAUTO_GAMMA_S));
    CHECK_NULL_PTR(g_stINIPara.stIniGamma.pstGamma);
    //gamma:ExpThresh
    pszTempStr = iniparser_getstr(g_Sceneautodictionary, "gamma:ExpThreshLtoD");
    if (NULL == pszTempStr)
    {
        printf("gamma:ExpThreshLtoD error\n");
        return HI_FAILURE;
    }
    s32Temp = Weight(pszTempStr);
    for (i = 0; i < g_stINIPara.stIniGamma.s32ExpCount; i++)
    {
        g_stINIPara.stIniGamma.pu32ExpThreshLtoD[i] = MAEWeight[i];
    }
    //gamma:ExpThreshDtoL
    pszTempStr = iniparser_getstr(g_Sceneautodictionary, "gamma:ExpThreshDtoL");
    if (NULL == pszTempStr)
    {
        printf("gamma:ExpThreshDtoL error\n");
        return HI_FAILURE;
    }
    s32Temp = Weight(pszTempStr);
    for (i = 0; i < g_stINIPara.stIniGamma.s32ExpCount; i++)
    {
        g_stINIPara.stIniGamma.pu32ExpThreshDtoL[i] = MAEWeight[i];
    }
    //gammatable
    for (i = 0; i < g_stINIPara.stIniGamma.s32ExpCount; i++)
    {
        s32Offset = 0;

        snprintf(szTempStr, sizeof(szTempStr), "gamma:gamma.0_%d", i);
        pszTempStr = iniparser_getstr(g_Sceneautodictionary, szTempStr);
        if (NULL == pszTempStr)
        {
            printf("gamma:gamma.0_%d error\n", i);
            return HI_FAILURE;
        }
        s32Temp = Weight(pszTempStr);
        for (j = 0; j < s32Temp; j++)
        {
            g_stINIPara.stIniGamma.pstGamma[i].u16Table[j] = MAEWeight[j];
        }
        s32Offset += s32Temp;

        snprintf(szTempStr, sizeof(szTempStr), "gamma:gamma.1_%d", i);
        pszTempStr = iniparser_getstr(g_Sceneautodictionary, szTempStr);
        if (NULL == pszTempStr)
        {
            printf("gamma:gamma.1_%d error\n", i);
            return HI_FAILURE;
        }
        s32Temp = Weight(pszTempStr);
        for (j = 0; j < s32Temp; j++)
        {
            g_stINIPara.stIniGamma.pstGamma[i].u16Table[s32Offset + j] = MAEWeight[j];
        }
        s32Offset += s32Temp;

        snprintf(szTempStr, sizeof(szTempStr), "gamma:gamma.2_%d", i);
        pszTempStr = iniparser_getstr(g_Sceneautodictionary, szTempStr);
        if (NULL == pszTempStr)
        {
            printf("gamma:gamma.2_%d error\n", i);
            return HI_FAILURE;
        }
        s32Temp = Weight(pszTempStr);
        for (j = 0; j < s32Temp; j++)
        {
            g_stINIPara.stIniGamma.pstGamma[i].u16Table[s32Offset + j] = MAEWeight[j];
        }

        g_stINIPara.stIniGamma.pstGamma[i].u8CurveType = 2;
    }

    /**************h264venc**************/
    //h264venc:vencBitrateCount
    s32Temp = 0;
    s32Temp = iniparser_getint(g_Sceneautodictionary, "h264venc:vencBitrateCount", HI_FAILURE);
    if (HI_FAILURE == s32Temp)
    {
        printf("h264venc:vencBitrateCount failed\n");
        return HI_FAILURE;
    }
    g_stINIPara.stIniH264Venc.s32BitrateCount = s32Temp;
    g_stINIPara.stIniH264Venc.pu32BitrateThresh = (HI_U32*)malloc((g_stINIPara.stIniH264Venc.s32BitrateCount) * sizeof(HI_U32));
    CHECK_NULL_PTR(g_stINIPara.stIniH264Venc.pu32BitrateThresh);
    g_stINIPara.stIniH264Venc.pstH264Venc = (SCENEAUTO_H264VENC_S*)malloc((g_stINIPara.stIniH264Venc.s32BitrateCount) * sizeof(SCENEAUTO_H264VENC_S));
    CHECK_NULL_PTR(g_stINIPara.stIniH264Venc.pstH264Venc);
    //h264venc:vencBitrateThresh
    pszTempStr = iniparser_getstr(g_Sceneautodictionary, "h264venc:vencBitrateThresh");
    if (NULL == pszTempStr)
    {
        printf("h264venc:vencBitrateThresh error\n");
        return HI_FAILURE;
    }
    s32Temp = Weight(pszTempStr);
    for (i = 0; i < g_stINIPara.stIniH264Venc.s32BitrateCount; i++)
    {
        g_stINIPara.stIniH264Venc.pu32BitrateThresh[i] = MAEWeight[i];
    }

    for (i = 0; i < g_stINIPara.stIniH264Venc.s32BitrateCount; i++)
    {
        //h264venc:chroma_qp_index_offset
        snprintf(szTempStr, sizeof(szTempStr), "h264venc:chroma_qp_index_offset_%d", i);
        s32Temp = 0;
        s32Temp = iniparser_getint(g_Sceneautodictionary, szTempStr, HI_FAILURE);
        if (HI_FAILURE == s32Temp)
        {
            printf("h264venc:chroma_qp_index_offset_%d failed\n", i);
            return HI_FAILURE;
        }
        g_stINIPara.stIniH264Venc.pstH264Venc[i].s32chroma_qp_index_offset = s32Temp;

        //h264venc:disable_deblocking_filter_idc
        snprintf(szTempStr, sizeof(szTempStr), "h264venc:disable_deblocking_filter_idc_%d", i);
        s32Temp = 0;
        s32Temp = iniparser_getint(g_Sceneautodictionary, szTempStr, HI_FAILURE);
        if (HI_FAILURE == s32Temp)
        {
            printf("h264venc:disable_deblocking_filter_idc_%d failed\n", i);
            return HI_FAILURE;
        }
        g_stINIPara.stIniH264Venc.pstH264Venc[i].stH264Dblk.disable_deblocking_filter_idc = s32Temp;

        //h264venc:slice_alpha_c0_offset_div2
        snprintf(szTempStr, sizeof(szTempStr), "h264venc:slice_alpha_c0_offset_div2_%d", i);
        s32Temp = 0;
        s32Temp = iniparser_getint(g_Sceneautodictionary, szTempStr, HI_FAILURE);
        if (HI_FAILURE == s32Temp)
        {
            printf("h264venc:slice_alpha_c0_offset_div2_%d failed\n", i);
            return HI_FAILURE;
        }
        g_stINIPara.stIniH264Venc.pstH264Venc[i].stH264Dblk.slice_alpha_c0_offset_div2 = s32Temp;

        //h264venc:slice_beta_offset_div2
        snprintf(szTempStr, sizeof(szTempStr), "h264venc:slice_beta_offset_div2_%d", i);
        s32Temp = 0;
        s32Temp = iniparser_getint(g_Sceneautodictionary, szTempStr, HI_FAILURE);
        if (HI_FAILURE == s32Temp)
        {
            printf("h264venc:slice_beta_offset_div2_%d failed\n", i);
            return HI_FAILURE;
        }
        g_stINIPara.stIniH264Venc.pstH264Venc[i].stH264Dblk.slice_beta_offset_div2 = s32Temp;

        //h264venc:u32DeltaQP
        snprintf(szTempStr, sizeof(szTempStr), "h264venc:u32DeltaQP_%d", i);
        s32Temp = 0;
        s32Temp = iniparser_getint(g_Sceneautodictionary, szTempStr, HI_FAILURE);
        if (HI_FAILURE == s32Temp)
        {
            printf("h264venc:u32DeltaQP_%d failed\n", i);
            return HI_FAILURE;
        }
        g_stINIPara.stIniH264Venc.pstH264Venc[i].u32DeltaQP = s32Temp;

        //h264venc:s32IPQPDelta
        snprintf(szTempStr, sizeof(szTempStr), "h264venc:s32IPQPDelta_%d", i);
        s32Temp = 0;
        s32Temp = iniparser_getint(g_Sceneautodictionary, szTempStr, HI_FAILURE);
        if (HI_FAILURE == s32Temp)
        {
            printf("h264venc:s32IPQPDelta_%d failed\n", i);
            return HI_FAILURE;
        }
        g_stINIPara.stIniH264Venc.pstH264Venc[i].s32IPQPDelta = s32Temp;

        //h264venc:ThreshI
        snprintf(szTempStr, sizeof(szTempStr), "h264venc:ThreshI_%d", i);
        pszTempStr = iniparser_getstr(g_Sceneautodictionary, szTempStr);
        if (NULL == pszTempStr)
        {
            printf("h264venc:ThreshI_%d error\n", i);
            return HI_FAILURE;
        }
        s32Temp = Weight(pszTempStr);
        for (j = 0; j < 12; j++)
        {
            g_stINIPara.stIniH264Venc.pstH264Venc[i].u32ThrdI[j] = MAEWeight[j];
        }

        //h264venc:ThreshP
        snprintf(szTempStr, sizeof(szTempStr), "h264venc:ThreshP_%d", i);
        pszTempStr = iniparser_getstr(g_Sceneautodictionary, szTempStr);
        if (NULL == pszTempStr)
        {
            printf("h264venc:ThreshP_%d error\n", i);
            return HI_FAILURE;
        }
        s32Temp = Weight(pszTempStr);
        for (j = 0; j < 12; j++)
        {
            g_stINIPara.stIniH264Venc.pstH264Venc[i].u32ThrdP[j] = MAEWeight[j];
        }

    }
    /**************h265venc**************/
    //h265venc:vencBitrateCount
    s32Temp = 0;
    s32Temp = iniparser_getint(g_Sceneautodictionary, "h265venc:vencBitrateCount", HI_FAILURE);
    if (HI_FAILURE == s32Temp)
    {
        printf("h264venc:vencBitrateCount failed\n");
        return HI_FAILURE;
    }
    g_stINIPara.stIniH265Venc.stIniH265VencRcParam.s32BitrateCount = s32Temp;
    g_stINIPara.stIniH265Venc.stIniH265VencRcParam.pu32BitrateThresh = (HI_U32*)malloc((g_stINIPara.stIniH265Venc.stIniH265VencRcParam.s32BitrateCount) * sizeof(HI_U32));
    CHECK_NULL_PTR(g_stINIPara.stIniH265Venc.stIniH265VencRcParam.pu32BitrateThresh);
    g_stINIPara.stIniH265Venc.stIniH265VencRcParam.pstH265VencRcParam = (SCENEAUTO_H265VENC_RCPARAM_S*)malloc((g_stINIPara.stIniH265Venc.stIniH265VencRcParam.s32BitrateCount) * sizeof(SCENEAUTO_H265VENC_RCPARAM_S));
    CHECK_NULL_PTR(g_stINIPara.stIniH265Venc.stIniH265VencRcParam.pstH265VencRcParam);

    //h265venc:vencBitrateThresh
    pszTempStr = iniparser_getstr(g_Sceneautodictionary, "h265venc:vencBitrateThresh");
    if (NULL == pszTempStr)
    {
        printf("h265venc:vencBitrateThresh error\n");
        return HI_FAILURE;
    }
    s32Temp = Weight(pszTempStr);
    for (i = 0; i < g_stINIPara.stIniH265Venc.stIniH265VencRcParam.s32BitrateCount; i++)
    {
        g_stINIPara.stIniH265Venc.stIniH265VencRcParam.pu32BitrateThresh[i] = MAEWeight[i];
    }

    for (i = 0; i < g_stINIPara.stIniH265Venc.stIniH265VencRcParam.s32BitrateCount; i++)
    {
        //h265venc:u32RowQpDelta
        snprintf(szTempStr, sizeof(szTempStr), "h265venc:u32RowQpDelta_%d", i);
        s32Temp = 0;
        s32Temp = iniparser_getint(g_Sceneautodictionary, szTempStr, HI_FAILURE);
        if (HI_FAILURE == s32Temp)
        {
            printf("h265venc:u32RowQpDelta_%d failed\n", i);
            return HI_FAILURE;
        }
        g_stINIPara.stIniH265Venc.stIniH265VencRcParam.pstH265VencRcParam[i].u32DeltaQP = s32Temp;

        //h265venc:s32IPQPDelta
        snprintf(szTempStr, sizeof(szTempStr), "h265venc:s32IPQPDelta_%d", i);
        s32Temp = 0;
        s32Temp = iniparser_getint(g_Sceneautodictionary, szTempStr, HI_FAILURE);
        if (HI_FAILURE == s32Temp)
        {
            printf("h265venc:s32IPQPDelta_%d failed\n", i);
            return HI_FAILURE;
        }
        g_stINIPara.stIniH265Venc.stIniH265VencRcParam.pstH265VencRcParam[i].s32IPQPDelta = s32Temp;
        //h265venc:ThreshI
        snprintf(szTempStr, sizeof(szTempStr), "h265venc:ThreshI_%d", i);
        pszTempStr = iniparser_getstr(g_Sceneautodictionary, szTempStr);
        if (NULL == pszTempStr)
        {
            printf("h265venc:ThreshI_%d error\n", i);
            return HI_FAILURE;
        }
        s32Temp = Weight(pszTempStr);
        for (j = 0; j < 12; j++)
        {
            g_stINIPara.stIniH265Venc.stIniH265VencRcParam.pstH265VencRcParam[i].u32ThrdI[j] = MAEWeight[j];
        }

        //h265venc:ThreshP
        snprintf(szTempStr, sizeof(szTempStr), "h265venc:ThreshP_%d", i);
        pszTempStr = iniparser_getstr(g_Sceneautodictionary, szTempStr);
        if (NULL == pszTempStr)
        {
            printf("h265venc:ThreshP_%d error\n", i);
            return HI_FAILURE;
        }
        s32Temp = Weight(pszTempStr);
        for (j = 0; j < 12; j++)
        {
            g_stINIPara.stIniH265Venc.stIniH265VencRcParam.pstH265VencRcParam[i].u32ThrdP[j] = MAEWeight[j];
        }
    }

    //h265venc_face_cfg
    //h265venc:vencBitrateCount
    s32Temp = 0;
    s32Temp = iniparser_getint(g_Sceneautodictionary, "h265venc:vencBitrateCount", HI_FAILURE);
    if (HI_FAILURE == s32Temp)
    {
        printf("h265venc:vencBitrateCount failed\n");
        return HI_FAILURE;
    }
    g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.s32BitrateCount = s32Temp;
    g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.pu32BitrateThresh = (HI_U32*)malloc((g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.s32BitrateCount) * sizeof(HI_U32));
    CHECK_NULL_PTR(g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.pu32BitrateThresh);
    //h265venc:vencBitrateThresh
    pszTempStr = iniparser_getstr(g_Sceneautodictionary, "h265venc:vencBitrateThresh");
    if (NULL == pszTempStr)
    {
        printf("h265venc:vencBitrateThresh error\n");
        return HI_FAILURE;
    }
    s32Temp = Weight(pszTempStr);
    for (i = 0; i < g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.s32BitrateCount; i++)
    {
        g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.pu32BitrateThresh[i] = MAEWeight[i];
    }
    //h265venc:ExpCount
    s32Temp = 0;
    s32Temp = iniparser_getint(g_Sceneautodictionary, "h265venc:ExpCount", HI_FAILURE);
    if (HI_FAILURE == s32Temp)
    {
        printf("h265venc:ExpCount failed\n");
        return HI_FAILURE;
    }
    g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.s32ExpCount = s32Temp;
    g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.pu32ExpThresh = (HI_U32*)malloc((g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.s32ExpCount) * sizeof(HI_U32));
    CHECK_NULL_PTR(g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.pu32ExpThresh);
    //h265venc:ExpThresh
    pszTempStr = iniparser_getstr(g_Sceneautodictionary, "h265venc:ExpThresh");
    if (NULL == pszTempStr)
    {
        printf("h265venc:ExpThresh error\n");
        return HI_FAILURE;
    }
    s32Temp = Weight(pszTempStr);
    for (i = 0; i < g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.s32ExpCount; i++)
    {
        g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.pu32ExpThresh[i] = MAEWeight[i];
    }

    g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.pstH265VencFaceCfg = (SCENEAUTO_H265VENC_FACECFG_S*)malloc((g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.s32BitrateCount) * (g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.s32ExpCount) * sizeof(SCENEAUTO_H265VENC_FACECFG_S));
    CHECK_NULL_PTR(g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.pstH265VencFaceCfg);

    for (i = 0; i < g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.s32BitrateCount; i++)
    {
        //h265venc:u8NormIntra4RdCost_I
        snprintf(szTempStr, sizeof(szTempStr), "h265venc:u8NormIntra4RdCost_I_%d", i);
        pszTempStr = iniparser_getstr(g_Sceneautodictionary, szTempStr);
        if (NULL == pszTempStr)
        {
            printf("h265venc:u8NormIntra4RdCost_I_%d error\n", i);
            return HI_FAILURE;
        }
        s32Temp = Weight(pszTempStr);
        for (j = 0; j < g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.s32ExpCount; j++)
        {
            g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.pstH265VencFaceCfg[i * g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.s32ExpCount + j].u8NormIntra4RdCost_I = MAEWeight[j];
        }
        //h265venc:u8NormIntra8RdCost_I
        snprintf(szTempStr, sizeof(szTempStr), "h265venc:u8NormIntra8RdCost_I_%d", i);
        pszTempStr = iniparser_getstr(g_Sceneautodictionary, szTempStr);
        if (NULL == pszTempStr)
        {
            printf("h265venc:u8NormIntra8RdCost_I_%d error\n", i);
            return HI_FAILURE;
        }
        s32Temp = Weight(pszTempStr);
        for (j = 0; j < g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.s32ExpCount; j++)
        {
            g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.pstH265VencFaceCfg[i * g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.s32ExpCount + j].u8NormIntra8RdCost_I = MAEWeight[j];
        }
        //h265venc:u8NormIntra16RdCost_I
        snprintf(szTempStr, sizeof(szTempStr), "h265venc:u8NormIntra16RdCost_I_%d", i);
        pszTempStr = iniparser_getstr(g_Sceneautodictionary, szTempStr);
        if (NULL == pszTempStr)
        {
            printf("h265venc:u8NormIntra16RdCost_I_%d error\n", i);
            return HI_FAILURE;
        }
        s32Temp = Weight(pszTempStr);
        for (j = 0; j < g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.s32ExpCount; j++)
        {
            g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.pstH265VencFaceCfg[i * g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.s32ExpCount + j].u8NormIntra16RdCost_I = MAEWeight[j];
        }
        //h265venc:u8NormIntra32RdCost_I
        snprintf(szTempStr, sizeof(szTempStr), "h265venc:u8NormIntra32RdCost_I_%d", i);
        pszTempStr = iniparser_getstr(g_Sceneautodictionary, szTempStr);
        if (NULL == pszTempStr)
        {
            printf("h265venc:u8NormIntra32RdCost_I_%d error\n", i);
            return HI_FAILURE;
        }
        s32Temp = Weight(pszTempStr);
        for (j = 0; j < g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.s32ExpCount; j++)
        {
            g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.pstH265VencFaceCfg[i * g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.s32ExpCount + j].u8NormIntra32RdCost_I = MAEWeight[j];
        }
        //h265venc:u8SkinIntra4RdCost_I
        snprintf(szTempStr, sizeof(szTempStr), "h265venc:u8SkinIntra4RdCost_I_%d", i);
        pszTempStr = iniparser_getstr(g_Sceneautodictionary, szTempStr);
        if (NULL == pszTempStr)
        {
            printf("h265venc:u8SkinIntra4RdCost_I_%d error\n", i);
            return HI_FAILURE;
        }
        s32Temp = Weight(pszTempStr);
        for (j = 0; j < g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.s32ExpCount; j++)
        {
            g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.pstH265VencFaceCfg[i * g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.s32ExpCount + j].u8SkinIntra4RdCost_I = MAEWeight[j];
        }
        //h265venc:u8SkinIntra8RdCost_I
        snprintf(szTempStr, sizeof(szTempStr), "h265venc:u8SkinIntra8RdCost_I_%d", i);
        pszTempStr = iniparser_getstr(g_Sceneautodictionary, szTempStr);
        if (NULL == pszTempStr)
        {
            printf("h265venc:u8SkinIntra8RdCost_I_%d error\n", i);
            return HI_FAILURE;
        }
        s32Temp = Weight(pszTempStr);
        for (j = 0; j < g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.s32ExpCount; j++)
        {
            g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.pstH265VencFaceCfg[i * g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.s32ExpCount + j].u8SkinIntra8RdCost_I = MAEWeight[j];
        }
        //h265venc:u8SkinIntra16RdCost_I
        snprintf(szTempStr, sizeof(szTempStr), "h265venc:u8SkinIntra16RdCost_I_%d", i);
        pszTempStr = iniparser_getstr(g_Sceneautodictionary, szTempStr);
        if (NULL == pszTempStr)
        {
            printf("h265venc:u8SkinIntra16RdCost_I_%d error\n", i);
            return HI_FAILURE;
        }
        s32Temp = Weight(pszTempStr);
        for (j = 0; j < g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.s32ExpCount; j++)
        {
            g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.pstH265VencFaceCfg[i * g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.s32ExpCount + j].u8SkinIntra16RdCost_I = MAEWeight[j];
        }
        //h265venc:u8SkinIntra32RdCost_I
        snprintf(szTempStr, sizeof(szTempStr), "h265venc:u8SkinIntra32RdCost_I_%d", i);
        pszTempStr = iniparser_getstr(g_Sceneautodictionary, szTempStr);
        if (NULL == pszTempStr)
        {
            printf("h265venc:u8SkinIntra32RdCost_I_%d error\n", i);
            return HI_FAILURE;
        }
        s32Temp = Weight(pszTempStr);
        for (j = 0; j < g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.s32ExpCount; j++)
        {
            g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.pstH265VencFaceCfg[i * g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.s32ExpCount + j].u8SkinIntra32RdCost_I = MAEWeight[j];
        }
        //h265venc:u8HedgeIntra4RdCost_I
        snprintf(szTempStr, sizeof(szTempStr), "h265venc:u8HedgeIntra4RdCost_I_%d", i);
        pszTempStr = iniparser_getstr(g_Sceneautodictionary, szTempStr);
        if (NULL == pszTempStr)
        {
            printf("h265venc:u8HedgeIntra4RdCost_I_%d error\n", i);
            return HI_FAILURE;
        }
        s32Temp = Weight(pszTempStr);
        for (j = 0; j < g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.s32ExpCount; j++)
        {
            g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.pstH265VencFaceCfg[i * g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.s32ExpCount + j].u8HedgeIntra4RdCost_I = MAEWeight[j];
        }
        //h265venc:u8HedgeIntra8RdCost_I
        snprintf(szTempStr, sizeof(szTempStr), "h265venc:u8HedgeIntra8RdCost_I_%d", i);
        pszTempStr = iniparser_getstr(g_Sceneautodictionary, szTempStr);
        if (NULL == pszTempStr)
        {
            printf("h265venc:u8HedgeIntra8RdCost_I_%d error\n", i);
            return HI_FAILURE;
        }
        s32Temp = Weight(pszTempStr);
        for (j = 0; j < g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.s32ExpCount; j++)
        {
            g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.pstH265VencFaceCfg[i * g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.s32ExpCount + j].u8HedgeIntra8RdCost_I = MAEWeight[j];
        }
        //h265venc:u8HedgeIntra16RdCost_I
        snprintf(szTempStr, sizeof(szTempStr), "h265venc:u8HedgeIntra16RdCost_I_%d", i);
        pszTempStr = iniparser_getstr(g_Sceneautodictionary, szTempStr);
        if (NULL == pszTempStr)
        {
            printf("h265venc:u8HedgeIntra16RdCost_I_%d error\n", i);
            return HI_FAILURE;
        }
        s32Temp = Weight(pszTempStr);
        for (j = 0; j < g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.s32ExpCount; j++)
        {
            g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.pstH265VencFaceCfg[i * g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.s32ExpCount + j].u8HedgeIntra16RdCost_I = MAEWeight[j];
        }
        //h265venc:u8HedgeIntra32RdCost_I
        snprintf(szTempStr, sizeof(szTempStr), "h265venc:u8HedgeIntra32RdCost_I_%d", i);
        pszTempStr = iniparser_getstr(g_Sceneautodictionary, szTempStr);
        if (NULL == pszTempStr)
        {
            printf("h265venc:u8HedgeIntra32RdCost_I_%d error\n", i);
            return HI_FAILURE;
        }
        s32Temp = Weight(pszTempStr);
        for (j = 0; j < g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.s32ExpCount; j++)
        {
            g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.pstH265VencFaceCfg[i * g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.s32ExpCount + j].u8HedgeIntra32RdCost_I = MAEWeight[j];
        }
        //h265venc:u8NormIntra4RdCost_P
        snprintf(szTempStr, sizeof(szTempStr), "h265venc:u8NormIntra4RdCost_P_%d", i);
        pszTempStr = iniparser_getstr(g_Sceneautodictionary, szTempStr);
        if (NULL == pszTempStr)
        {
            printf("h265venc:u8NormIntra4RdCost_P_%d error\n", i);
            return HI_FAILURE;
        }
        s32Temp = Weight(pszTempStr);
        for (j = 0; j < g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.s32ExpCount; j++)
        {
            g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.pstH265VencFaceCfg[i * g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.s32ExpCount + j].u8NormIntra4RdCost_P = MAEWeight[j];
        }
        //h265venc:u8NormIntra8RdCost_P
        snprintf(szTempStr, sizeof(szTempStr), "h265venc:u8NormIntra8RdCost_P_%d", i);
        pszTempStr = iniparser_getstr(g_Sceneautodictionary, szTempStr);
        if (NULL == pszTempStr)
        {
            printf("h265venc:u8NormIntra8RdCost_P_%d error\n", i);
            return HI_FAILURE;
        }
        s32Temp = Weight(pszTempStr);
        for (j = 0; j < g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.s32ExpCount; j++)
        {
            g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.pstH265VencFaceCfg[i * g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.s32ExpCount + j].u8NormIntra8RdCost_P = MAEWeight[j];
        }
        //h265venc:u8NormIntra16RdCost_P
        snprintf(szTempStr, sizeof(szTempStr), "h265venc:u8NormIntra16RdCost_P_%d", i);
        pszTempStr = iniparser_getstr(g_Sceneautodictionary, szTempStr);
        if (NULL == pszTempStr)
        {
            printf("h265venc:u8NormIntra16RdCost_P_%d error\n", i);
            return HI_FAILURE;
        }
        s32Temp = Weight(pszTempStr);
        for (j = 0; j < g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.s32ExpCount; j++)
        {
            g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.pstH265VencFaceCfg[i * g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.s32ExpCount + j].u8NormIntra16RdCost_P = MAEWeight[j];
        }
        //h265venc:u8NormIntra32RdCost_P
        snprintf(szTempStr, sizeof(szTempStr), "h265venc:u8NormIntra32RdCost_P_%d", i);
        pszTempStr = iniparser_getstr(g_Sceneautodictionary, szTempStr);
        if (NULL == pszTempStr)
        {
            printf("h265venc:u8NormIntra32RdCost_P_%d error\n", i);
            return HI_FAILURE;
        }
        s32Temp = Weight(pszTempStr);
        for (j = 0; j < g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.s32ExpCount; j++)
        {
            g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.pstH265VencFaceCfg[i * g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.s32ExpCount + j].u8NormIntra32RdCost_P = MAEWeight[j];
        }
        //h265venc:u8SkinIntra4RdCost_P
        snprintf(szTempStr, sizeof(szTempStr), "h265venc:u8SkinIntra4RdCost_P_%d", i);
        pszTempStr = iniparser_getstr(g_Sceneautodictionary, szTempStr);
        if (NULL == pszTempStr)
        {
            printf("h265venc:u8SkinIntra4RdCost_P_%d error\n", i);
            return HI_FAILURE;
        }
        s32Temp = Weight(pszTempStr);
        for (j = 0; j < g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.s32ExpCount; j++)
        {
            g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.pstH265VencFaceCfg[i * g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.s32ExpCount + j].u8SkinIntra4RdCost_P = MAEWeight[j];
        }
        //h265venc:u8SkinIntra8RdCost_P
        snprintf(szTempStr, sizeof(szTempStr), "h265venc:u8SkinIntra8RdCost_P_%d", i);
        pszTempStr = iniparser_getstr(g_Sceneautodictionary, szTempStr);
        if (NULL == pszTempStr)
        {
            printf("h265venc:u8SkinIntra8RdCost_P_%d error\n", i);
            return HI_FAILURE;
        }
        s32Temp = Weight(pszTempStr);
        for (j = 0; j < g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.s32ExpCount; j++)
        {
            g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.pstH265VencFaceCfg[i * g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.s32ExpCount + j].u8SkinIntra8RdCost_P = MAEWeight[j];
        }
        //h265venc:u8SkinIntra16RdCost_P
        snprintf(szTempStr, sizeof(szTempStr), "h265venc:u8SkinIntra16RdCost_P_%d", i);
        pszTempStr = iniparser_getstr(g_Sceneautodictionary, szTempStr);
        if (NULL == pszTempStr)
        {
            printf("h265venc:u8SkinIntra16RdCost_P_%d error\n", i);
            return HI_FAILURE;
        }
        s32Temp = Weight(pszTempStr);
        for (j = 0; j < g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.s32ExpCount; j++)
        {
            g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.pstH265VencFaceCfg[i * g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.s32ExpCount + j].u8SkinIntra16RdCost_P = MAEWeight[j];
        }
        //h265venc:u8SkinIntra32RdCost_P
        snprintf(szTempStr, sizeof(szTempStr), "h265venc:u8SkinIntra32RdCost_P_%d", i);
        pszTempStr = iniparser_getstr(g_Sceneautodictionary, szTempStr);
        if (NULL == pszTempStr)
        {
            printf("h265venc:u8SkinIntra32RdCost_P_%d error\n", i);
            return HI_FAILURE;
        }
        s32Temp = Weight(pszTempStr);
        for (j = 0; j < g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.s32ExpCount; j++)
        {
            g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.pstH265VencFaceCfg[i * g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.s32ExpCount + j].u8SkinIntra32RdCost_P = MAEWeight[j];
        }
        //h265venc:u8HedgeIntra4RdCost_P
        snprintf(szTempStr, sizeof(szTempStr), "h265venc:u8HedgeIntra4RdCost_P_%d", i);
        pszTempStr = iniparser_getstr(g_Sceneautodictionary, szTempStr);
        if (NULL == pszTempStr)
        {
            printf("h265venc:u8HedgeIntra4RdCost_P_%d error\n", i);
            return HI_FAILURE;
        }
        s32Temp = Weight(pszTempStr);
        for (j = 0; j < g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.s32ExpCount; j++)
        {
            g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.pstH265VencFaceCfg[i * g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.s32ExpCount + j].u8HedgeIntra4RdCost_P = MAEWeight[j];
        }
        //h265venc:u8HedgeIntra8RdCost_P
        snprintf(szTempStr, sizeof(szTempStr), "h265venc:u8HedgeIntra8RdCost_P_%d", i);
        pszTempStr = iniparser_getstr(g_Sceneautodictionary, szTempStr);
        if (NULL == pszTempStr)
        {
            printf("h265venc:u8HedgeIntra8RdCost_P_%d error\n", i);
            return HI_FAILURE;
        }
        s32Temp = Weight(pszTempStr);
        for (j = 0; j < g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.s32ExpCount; j++)
        {
            g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.pstH265VencFaceCfg[i * g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.s32ExpCount + j].u8HedgeIntra8RdCost_P = MAEWeight[j];
        }
        //h265venc:u8HedgeIntra16RdCost_P
        snprintf(szTempStr, sizeof(szTempStr), "h265venc:u8HedgeIntra16RdCost_P_%d", i);
        pszTempStr = iniparser_getstr(g_Sceneautodictionary, szTempStr);
        if (NULL == pszTempStr)
        {
            printf("h265venc:u8HedgeIntra16RdCost_P_%d error\n", i);
            return HI_FAILURE;
        }
        s32Temp = Weight(pszTempStr);
        for (j = 0; j < g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.s32ExpCount; j++)
        {
            g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.pstH265VencFaceCfg[i * g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.s32ExpCount + j].u8HedgeIntra16RdCost_P = MAEWeight[j];
        }
        //h265venc:u8HedgeIntra32RdCost_P
        snprintf(szTempStr, sizeof(szTempStr), "h265venc:u8HedgeIntra32RdCost_P_%d", i);
        pszTempStr = iniparser_getstr(g_Sceneautodictionary, szTempStr);
        if (NULL == pszTempStr)
        {
            printf("h265venc:u8HedgeIntra32RdCost_P_%d error\n", i);
            return HI_FAILURE;
        }
        s32Temp = Weight(pszTempStr);
        for (j = 0; j < g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.s32ExpCount; j++)
        {
            g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.pstH265VencFaceCfg[i * g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.s32ExpCount + j].u8HedgeIntra32RdCost_P = MAEWeight[j];
        }
        //h265venc:u8NormFme8RdCost_P
        snprintf(szTempStr, sizeof(szTempStr), "h265venc:u8NormFme8RdCost_P_%d", i);
        pszTempStr = iniparser_getstr(g_Sceneautodictionary, szTempStr);
        if (NULL == pszTempStr)
        {
            printf("h265venc:u8NormFme8RdCost_P_%d error\n", i);
            return HI_FAILURE;
        }
        s32Temp = Weight(pszTempStr);
        for (j = 0; j < g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.s32ExpCount; j++)
        {
            g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.pstH265VencFaceCfg[i * g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.s32ExpCount + j].u8NormFme8RdCost_P = MAEWeight[j];
        }
        //h265venc:u8NormFme16RdCost_P
        snprintf(szTempStr, sizeof(szTempStr), "h265venc:u8NormFme16RdCost_P_%d", i);
        pszTempStr = iniparser_getstr(g_Sceneautodictionary, szTempStr);
        if (NULL == pszTempStr)
        {
            printf("h265venc:u8NormFme16RdCost_P_%d error\n", i);
            return HI_FAILURE;
        }
        s32Temp = Weight(pszTempStr);
        for (j = 0; j < g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.s32ExpCount; j++)
        {
            g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.pstH265VencFaceCfg[i * g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.s32ExpCount + j].u8NormFme16RdCost_P = MAEWeight[j];
        }
        //h265venc:u8NormFme32RdCost_P
        snprintf(szTempStr, sizeof(szTempStr), "h265venc:u8NormFme32RdCost_P_%d", i);
        pszTempStr = iniparser_getstr(g_Sceneautodictionary, szTempStr);
        if (NULL == pszTempStr)
        {
            printf("h265venc:u8NormFme32RdCost_P_%d error\n", i);
            return HI_FAILURE;
        }
        s32Temp = Weight(pszTempStr);
        for (j = 0; j < g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.s32ExpCount; j++)
        {
            g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.pstH265VencFaceCfg[i * g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.s32ExpCount + j].u8NormFme32RdCost_P = MAEWeight[j];
        }
        //h265venc:u8NormFme64RdCost_P
        snprintf(szTempStr, sizeof(szTempStr), "h265venc:u8NormFme64RdCost_P_%d", i);
        pszTempStr = iniparser_getstr(g_Sceneautodictionary, szTempStr);
        if (NULL == pszTempStr)
        {
            printf("h265venc:u8NormFme64RdCost_P_%d error\n", i);
            return HI_FAILURE;
        }
        s32Temp = Weight(pszTempStr);
        for (j = 0; j < g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.s32ExpCount; j++)
        {
            g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.pstH265VencFaceCfg[i * g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.s32ExpCount + j].u8NormFme64RdCost_P = MAEWeight[j];
        }
        snprintf(szTempStr, sizeof(szTempStr), "h265venc:u8SkinFme8RdCost_P_%d", i);
        pszTempStr = iniparser_getstr(g_Sceneautodictionary, szTempStr);
        if (NULL == pszTempStr)
        {
            printf("h265venc:u8SkinFme8RdCost_P_%d error\n", i);
            return HI_FAILURE;
        }
        s32Temp = Weight(pszTempStr);
        for (j = 0; j < g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.s32ExpCount; j++)
        {
            g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.pstH265VencFaceCfg[i * g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.s32ExpCount + j].u8SkinFme8RdCost_P = MAEWeight[j];
        }
        snprintf(szTempStr, sizeof(szTempStr), "h265venc:u8SkinFme16RdCost_P_%d", i);
        pszTempStr = iniparser_getstr(g_Sceneautodictionary, szTempStr);
        if (NULL == pszTempStr)
        {
            printf("h265venc:u8SkinFme16RdCost_P_%d error\n", i);
            return HI_FAILURE;
        }
        s32Temp = Weight(pszTempStr);
        for (j = 0; j < g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.s32ExpCount; j++)
        {
            g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.pstH265VencFaceCfg[i * g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.s32ExpCount + j].u8SkinFme16RdCost_P = MAEWeight[j];
        }
        snprintf(szTempStr, sizeof(szTempStr), "h265venc:u8SkinFme32RdCost_P_%d", i);
        pszTempStr = iniparser_getstr(g_Sceneautodictionary, szTempStr);
        if (NULL == pszTempStr)
        {
            printf("h265venc:u8SkinFme32RdCost_P_%d error\n", i);
            return HI_FAILURE;
        }
        s32Temp = Weight(pszTempStr);
        for (j = 0; j < g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.s32ExpCount; j++)
        {
            g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.pstH265VencFaceCfg[i * g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.s32ExpCount + j].u8SkinFme32RdCost_P = MAEWeight[j];
        }
        snprintf(szTempStr, sizeof(szTempStr), "h265venc:u8SkinFme64RdCost_P_%d", i);
        pszTempStr = iniparser_getstr(g_Sceneautodictionary, szTempStr);
        if (NULL == pszTempStr)
        {
            printf("h265venc:u8SkinFme64RdCost_P_%d error\n", i);
            return HI_FAILURE;
        }
        s32Temp = Weight(pszTempStr);
        for (j = 0; j < g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.s32ExpCount; j++)
        {
            g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.pstH265VencFaceCfg[i * g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.s32ExpCount + j].u8SkinFme64RdCost_P = MAEWeight[j];
        }
        //h265venc:u8HedgeFme8RdCost_P
        snprintf(szTempStr, sizeof(szTempStr), "h265venc:u8HedgeFme8RdCost_P_%d", i);
        pszTempStr = iniparser_getstr(g_Sceneautodictionary, szTempStr);
        if (NULL == pszTempStr)
        {
            printf("h265venc:u8HedgeFme8RdCost_P_%d error\n", i);
            return HI_FAILURE;
        }
        s32Temp = Weight(pszTempStr);
        for (j = 0; j < g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.s32ExpCount; j++)
        {
            g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.pstH265VencFaceCfg[i * g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.s32ExpCount + j].u8HedgeFme8RdCost_P = MAEWeight[j];
        }
        //h265venc:u8HedgeFme16RdCost_P
        snprintf(szTempStr, sizeof(szTempStr), "h265venc:u8HedgeFme16RdCost_P_%d", i);
        pszTempStr = iniparser_getstr(g_Sceneautodictionary, szTempStr);
        if (NULL == pszTempStr)
        {
            printf("h265venc:u8HedgeFme16RdCost_P_%d error\n", i);
            return HI_FAILURE;
        }
        s32Temp = Weight(pszTempStr);
        for (j = 0; j < g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.s32ExpCount; j++)
        {
            g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.pstH265VencFaceCfg[i * g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.s32ExpCount + j].u8HedgeFme16RdCost_P = MAEWeight[j];
        }
        //h265venc:u8HedgeFme32RdCost_P
        snprintf(szTempStr, sizeof(szTempStr), "h265venc:u8HedgeFme32RdCost_P_%d", i);
        pszTempStr = iniparser_getstr(g_Sceneautodictionary, szTempStr);
        if (NULL == pszTempStr)
        {
            printf("h265venc:u8HedgeFme32RdCost_P_%d error\n", i);
            return HI_FAILURE;
        }
        s32Temp = Weight(pszTempStr);
        for (j = 0; j < g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.s32ExpCount; j++)
        {
            g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.pstH265VencFaceCfg[i * g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.s32ExpCount + j].u8HedgeFme32RdCost_P = MAEWeight[j];
        }
        //h265venc:u8HedgeFme64RdCost_P
        snprintf(szTempStr, sizeof(szTempStr), "h265venc:u8HedgeFme64RdCost_P_%d", i);
        pszTempStr = iniparser_getstr(g_Sceneautodictionary, szTempStr);
        if (NULL == pszTempStr)
        {
            printf("h265venc:u8HedgeFme64RdCost_P_%d error\n", i);
            return HI_FAILURE;
        }
        s32Temp = Weight(pszTempStr);
        for (j = 0; j < g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.s32ExpCount; j++)
        {
            g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.pstH265VencFaceCfg[i * g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.s32ExpCount + j].u8HedgeFme64RdCost_P = MAEWeight[j];
        }
        //h265venc:u8NormMerg8RdCost_P
        snprintf(szTempStr, sizeof(szTempStr), "h265venc:u8NormMerg8RdCost_P_%d", i);
        pszTempStr = iniparser_getstr(g_Sceneautodictionary, szTempStr);
        if (NULL == pszTempStr)
        {
            printf("h265venc:u8NormMerg8RdCost_P_%d error\n", i);
            return HI_FAILURE;
        }
        s32Temp = Weight(pszTempStr);
        for (j = 0; j < g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.s32ExpCount; j++)
        {
            g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.pstH265VencFaceCfg[i * g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.s32ExpCount + j].u8NormMerg8RdCost_P = MAEWeight[j];
        }
        //h265venc:u8NormMerg16RdCost_P
        snprintf(szTempStr, sizeof(szTempStr), "h265venc:u8NormMerg16RdCost_P_%d", i);
        pszTempStr = iniparser_getstr(g_Sceneautodictionary, szTempStr);
        if (NULL == pszTempStr)
        {
            printf("h265venc:u8NormMerg16RdCost_P_%d error\n", i);
            return HI_FAILURE;
        }
        s32Temp = Weight(pszTempStr);
        for (j = 0; j < g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.s32ExpCount; j++)
        {
            g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.pstH265VencFaceCfg[i * g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.s32ExpCount + j].u8NormMerg16RdCost_P = MAEWeight[j];
        }
        //h265venc:u8NormMerg32RdCost_P
        snprintf(szTempStr, sizeof(szTempStr), "h265venc:u8NormMerg32RdCost_P_%d", i);
        pszTempStr = iniparser_getstr(g_Sceneautodictionary, szTempStr);
        if (NULL == pszTempStr)
        {
            printf("h265venc:u8NormMerg32RdCost_P_%d error\n", i);
            return HI_FAILURE;
        }
        s32Temp = Weight(pszTempStr);
        for (j = 0; j < g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.s32ExpCount; j++)
        {
            g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.pstH265VencFaceCfg[i * g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.s32ExpCount + j].u8NormMerg32RdCost_P = MAEWeight[j];
        }
        //h265venc:u8NormMerg64RdCost_P
        snprintf(szTempStr, sizeof(szTempStr), "h265venc:u8NormMerg64RdCost_P_%d", i);
        pszTempStr = iniparser_getstr(g_Sceneautodictionary, szTempStr);
        if (NULL == pszTempStr)
        {
            printf("h265venc:u8NormMerg64RdCost_P_%d error\n", i);
            return HI_FAILURE;
        }
        s32Temp = Weight(pszTempStr);
        for (j = 0; j < g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.s32ExpCount; j++)
        {
            g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.pstH265VencFaceCfg[i * g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.s32ExpCount + j].u8NormMerg64RdCost_P = MAEWeight[j];
        }
        snprintf(szTempStr, sizeof(szTempStr), "h265venc:u8SkinMerg8RdCost_P_%d", i);
        pszTempStr = iniparser_getstr(g_Sceneautodictionary, szTempStr);
        if (NULL == pszTempStr)
        {
            printf("h265venc:u8SkinMerg8RdCost_P_%d error\n", i);
            return HI_FAILURE;
        }
        s32Temp = Weight(pszTempStr);
        for (j = 0; j < g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.s32ExpCount; j++)
        {
            g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.pstH265VencFaceCfg[i * g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.s32ExpCount + j].u8SkinMerg8RdCost_P = MAEWeight[j];
        }
        snprintf(szTempStr, sizeof(szTempStr), "h265venc:u8SkinMerg16RdCost_P_%d", i);
        pszTempStr = iniparser_getstr(g_Sceneautodictionary, szTempStr);
        if (NULL == pszTempStr)
        {
            printf("h265venc:u8SkinMerg16RdCost_P_%d error\n", i);
            return HI_FAILURE;
        }
        s32Temp = Weight(pszTempStr);
        for (j = 0; j < g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.s32ExpCount; j++)
        {
            g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.pstH265VencFaceCfg[i * g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.s32ExpCount + j].u8SkinMerg16RdCost_P = MAEWeight[j];
        }
        snprintf(szTempStr, sizeof(szTempStr), "h265venc:u8SkinMerg32RdCost_P_%d", i);
        pszTempStr = iniparser_getstr(g_Sceneautodictionary, szTempStr);
        if (NULL == pszTempStr)
        {
            printf("h265venc:u8SkinMerg32RdCost_P_%d error\n", i);
            return HI_FAILURE;
        }
        s32Temp = Weight(pszTempStr);
        for (j = 0; j < g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.s32ExpCount; j++)
        {
            g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.pstH265VencFaceCfg[i * g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.s32ExpCount + j].u8SkinMerg32RdCost_P = MAEWeight[j];
        }
        snprintf(szTempStr, sizeof(szTempStr), "h265venc:u8SkinMerg64RdCost_P_%d", i);
        pszTempStr = iniparser_getstr(g_Sceneautodictionary, szTempStr);
        if (NULL == pszTempStr)
        {
            printf("h265venc:u8SkinMerg64RdCost_P_%d error\n", i);
            return HI_FAILURE;
        }
        s32Temp = Weight(pszTempStr);
        for (j = 0; j < g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.s32ExpCount; j++)
        {
            g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.pstH265VencFaceCfg[i * g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.s32ExpCount + j].u8SkinMerg64RdCost_P = MAEWeight[j];
        }
        //h265venc:u8HedgeMerg8RdCost_P
        snprintf(szTempStr, sizeof(szTempStr), "h265venc:u8HedgeMerg8RdCost_P_%d", i);
        pszTempStr = iniparser_getstr(g_Sceneautodictionary, szTempStr);
        if (NULL == pszTempStr)
        {
            printf("h265venc:u8HedgeMerg8RdCost_P_%d error\n", i);
            return HI_FAILURE;
        }
        s32Temp = Weight(pszTempStr);
        for (j = 0; j < g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.s32ExpCount; j++)
        {
            g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.pstH265VencFaceCfg[i * g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.s32ExpCount + j].u8HedgeMerg8RdCost_P = MAEWeight[j];
        }
        //h265venc:u8HedgeMerg16RdCost_P
        snprintf(szTempStr, sizeof(szTempStr), "h265venc:u8HedgeMerg16RdCost_P_%d", i);
        pszTempStr = iniparser_getstr(g_Sceneautodictionary, szTempStr);
        if (NULL == pszTempStr)
        {
            printf("h265venc:u8HedgeMerg16RdCost_P_%d error\n", i);
            return HI_FAILURE;
        }
        s32Temp = Weight(pszTempStr);
        for (j = 0; j < g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.s32ExpCount; j++)
        {
            g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.pstH265VencFaceCfg[i * g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.s32ExpCount + j].u8HedgeMerg16RdCost_P = MAEWeight[j];
        }
        //h265venc:u8HedgeMerg32RdCost_P
        snprintf(szTempStr, sizeof(szTempStr), "h265venc:u8HedgeMerg32RdCost_P_%d", i);
        pszTempStr = iniparser_getstr(g_Sceneautodictionary, szTempStr);
        if (NULL == pszTempStr)
        {
            printf("h265venc:u8HedgeMerg32RdCost_P_%d error\n", i);
            return HI_FAILURE;
        }
        s32Temp = Weight(pszTempStr);
        for (j = 0; j < g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.s32ExpCount; j++)
        {
            g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.pstH265VencFaceCfg[i * g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.s32ExpCount + j].u8HedgeMerg32RdCost_P = MAEWeight[j];
        }
        //h265venc:u8HedgeMerg64RdCost_P
        snprintf(szTempStr, sizeof(szTempStr), "h265venc:u8HedgeMerg64RdCost_P_%d", i);
        pszTempStr = iniparser_getstr(g_Sceneautodictionary, szTempStr);
        if (NULL == pszTempStr)
        {
            printf("h265venc:u8HedgeMerg64RdCost_P_%d error\n", i);
            return HI_FAILURE;
        }
        s32Temp = Weight(pszTempStr);
        for (j = 0; j < g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.s32ExpCount; j++)
        {
            g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.pstH265VencFaceCfg[i * g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.s32ExpCount + j].u8HedgeMerg64RdCost_P = MAEWeight[j];
        }
        //h265venc:bSkinEn_I
        snprintf(szTempStr, sizeof(szTempStr), "h265venc:bSkinEn_I_%d", i);
        pszTempStr = iniparser_getstr(g_Sceneautodictionary, szTempStr);
        if (NULL == pszTempStr)
        {
            printf("h265venc:bSkinEn_I_%d error\n", i);
            return HI_FAILURE;
        }
        s32Temp = Weight(pszTempStr);
        for (j = 0; j < g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.s32ExpCount; j++)
        {
            g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.pstH265VencFaceCfg[i * g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.s32ExpCount + j].bSkinEn_I = (HI_BOOL)MAEWeight[j];
        }
        //h265venc:u32SkinQpDelta_I
        snprintf(szTempStr, sizeof(szTempStr), "h265venc:u32SkinQpDelta_I_%d", i);
        pszTempStr = iniparser_getstr(g_Sceneautodictionary, szTempStr);
        if (NULL == pszTempStr)
        {
            printf("h265venc:u32SkinQpDelta_I_%d error\n", i);
            return HI_FAILURE;
        }
        s32Temp = Weight(pszTempStr);
        for (j = 0; j < g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.s32ExpCount; j++)
        {
            g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.pstH265VencFaceCfg[i * g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.s32ExpCount + j].u32SkinQpDelta_I = MAEWeight[j];
        }
        //h265venc:u8SkinUMax_I
        snprintf(szTempStr, sizeof(szTempStr), "h265venc:u8SkinUMax_I_%d", i);
        pszTempStr = iniparser_getstr(g_Sceneautodictionary, szTempStr);
        if (NULL == pszTempStr)
        {
            printf("h265venc:u8SkinUMax_I_%d error\n", i);
            return HI_FAILURE;
        }
        s32Temp = Weight(pszTempStr);
        for (j = 0; j < g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.s32ExpCount; j++)
        {
            g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.pstH265VencFaceCfg[i * g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.s32ExpCount + j].u8SkinUMax_I = MAEWeight[j];
        }
        //h265venc:u8SkinUMin_I
        snprintf(szTempStr, sizeof(szTempStr), "h265venc:u8SkinUMin_I_%d", i);
        pszTempStr = iniparser_getstr(g_Sceneautodictionary, szTempStr);
        if (NULL == pszTempStr)
        {
            printf("h265venc:u8SkinUMin_I_%d error\n", i);
            return HI_FAILURE;
        }
        s32Temp = Weight(pszTempStr);
        for (j = 0; j < g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.s32ExpCount; j++)
        {
            g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.pstH265VencFaceCfg[i * g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.s32ExpCount + j].u8SkinUMin_I = MAEWeight[j];
        }
        //h265venc:u8SkinVMax_I
        snprintf(szTempStr, sizeof(szTempStr), "h265venc:u8SkinVMax_I_%d", i);
        pszTempStr = iniparser_getstr(g_Sceneautodictionary, szTempStr);
        if (NULL == pszTempStr)
        {
            printf("h265venc:u8SkinVMax_I_%d error\n", i);
            return HI_FAILURE;
        }
        s32Temp = Weight(pszTempStr);
        for (j = 0; j < g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.s32ExpCount; j++)
        {
            g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.pstH265VencFaceCfg[i * g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.s32ExpCount + j].u8SkinVMax_I = MAEWeight[j];
        }
        //h265venc:u8SkinVMin_I
        snprintf(szTempStr, sizeof(szTempStr), "h265venc:u8SkinVMin_I_%d", i);
        pszTempStr = iniparser_getstr(g_Sceneautodictionary, szTempStr);
        if (NULL == pszTempStr)
        {
            printf("h265venc:u8SkinVMin_I_%d error\n", i);
            return HI_FAILURE;
        }
        s32Temp = Weight(pszTempStr);
        for (j = 0; j < g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.s32ExpCount; j++)
        {
            g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.pstH265VencFaceCfg[i * g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.s32ExpCount + j].u8SkinVMin_I = MAEWeight[j];
        }
        //h265venc:u32SkinNum_I
        snprintf(szTempStr, sizeof(szTempStr), "h265venc:u32SkinNum_I_%d", i);
        pszTempStr = iniparser_getstr(g_Sceneautodictionary, szTempStr);
        if (NULL == pszTempStr)
        {
            printf("h265venc:u32SkinNum_I_%d error\n", i);
            return HI_FAILURE;
        }
        s32Temp = Weight(pszTempStr);
        for (j = 0; j < g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.s32ExpCount; j++)
        {
            g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.pstH265VencFaceCfg[i * g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.s32ExpCount + j].u32SkinNum_I = MAEWeight[j];
        }
        //h265venc:bSkinEn_P
        snprintf(szTempStr, sizeof(szTempStr), "h265venc:bSkinEn_P_%d", i);
        pszTempStr = iniparser_getstr(g_Sceneautodictionary, szTempStr);
        if (NULL == pszTempStr)
        {
            printf("h265venc:bSkinEn_P_%d error\n", i);
            return HI_FAILURE;
        }
        s32Temp = Weight(pszTempStr);
        for (j = 0; j < g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.s32ExpCount; j++)
        {
            g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.pstH265VencFaceCfg[i * g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.s32ExpCount + j].bSkinEn_P = (HI_BOOL)MAEWeight[j];
        }
        //h265venc:u32SkinQpDelta_P
        snprintf(szTempStr, sizeof(szTempStr), "h265venc:u32SkinQpDelta_P_%d", i);
        pszTempStr = iniparser_getstr(g_Sceneautodictionary, szTempStr);
        if (NULL == pszTempStr)
        {
            printf("h265venc:u32SkinQpDelta_P_%d error\n", i);
            return HI_FAILURE;
        }
        s32Temp = Weight(pszTempStr);
        for (j = 0; j < g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.s32ExpCount; j++)
        {
            g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.pstH265VencFaceCfg[i * g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.s32ExpCount + j].u32SkinQpDelta_P = MAEWeight[j];
        }
        //h265venc:u8SkinUMax_P
        snprintf(szTempStr, sizeof(szTempStr), "h265venc:u8SkinUMax_P_%d", i);
        pszTempStr = iniparser_getstr(g_Sceneautodictionary, szTempStr);
        if (NULL == pszTempStr)
        {
            printf("h265venc:u8SkinUMax_P_%d error\n", i);
            return HI_FAILURE;
        }
        s32Temp = Weight(pszTempStr);
        for (j = 0; j < g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.s32ExpCount; j++)
        {
            g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.pstH265VencFaceCfg[i * g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.s32ExpCount + j].u8SkinUMax_P = MAEWeight[j];
        }
        //h265venc:u8SkinUMin_P
        snprintf(szTempStr, sizeof(szTempStr), "h265venc:u8SkinUMin_P_%d", i);
        pszTempStr = iniparser_getstr(g_Sceneautodictionary, szTempStr);
        if (NULL == pszTempStr)
        {
            printf("h265venc:u8SkinUMin_P_%d error\n", i);
            return HI_FAILURE;
        }
        s32Temp = Weight(pszTempStr);
        for (j = 0; j < g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.s32ExpCount; j++)
        {
            g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.pstH265VencFaceCfg[i * g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.s32ExpCount + j].u8SkinUMin_P = MAEWeight[j];
        }
        //h265venc:u8SkinVMax_P
        snprintf(szTempStr, sizeof(szTempStr), "h265venc:u8SkinVMax_P_%d", i);
        pszTempStr = iniparser_getstr(g_Sceneautodictionary, szTempStr);
        if (NULL == pszTempStr)
        {
            printf("h265venc:u8SkinVMax_P_%d error\n", i);
            return HI_FAILURE;
        }
        s32Temp = Weight(pszTempStr);
        for (j = 0; j < g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.s32ExpCount; j++)
        {
            g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.pstH265VencFaceCfg[i * g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.s32ExpCount + j].u8SkinVMax_P = MAEWeight[j];
        }
        //h265venc:u8SkinVMin_P
        snprintf(szTempStr, sizeof(szTempStr), "h265venc:u8SkinVMin_P_%d", i);
        pszTempStr = iniparser_getstr(g_Sceneautodictionary, szTempStr);
        if (NULL == pszTempStr)
        {
            printf("h265venc:u8SkinVMin_P_%d error\n", i);
            return HI_FAILURE;
        }
        s32Temp = Weight(pszTempStr);
        for (j = 0; j < g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.s32ExpCount; j++)
        {
            g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.pstH265VencFaceCfg[i * g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.s32ExpCount + j].u8SkinVMin_P = MAEWeight[j];
        }
        //h265venc:u32SkinNum_P
        snprintf(szTempStr, sizeof(szTempStr), "h265venc:u32SkinNum_P_%d", i);
        pszTempStr = iniparser_getstr(g_Sceneautodictionary, szTempStr);
        if (NULL == pszTempStr)
        {
            printf("h265venc:u32SkinNum_P_%d error\n", i);
            return HI_FAILURE;
        }
        s32Temp = Weight(pszTempStr);
        for (j = 0; j < g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.s32ExpCount; j++)
        {
            g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.pstH265VencFaceCfg[i * g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.s32ExpCount + j].u32SkinNum_P = MAEWeight[j];
        }
        //h265venc:u8HedgeThr_I
        snprintf(szTempStr, sizeof(szTempStr), "h265venc:u8HedgeThr_I_%d", i);
        pszTempStr = iniparser_getstr(g_Sceneautodictionary, szTempStr);
        if (NULL == pszTempStr)
        {
            printf("h265venc:u8HedgeThr_I_%d error\n", i);
            return HI_FAILURE;
        }
        s32Temp = Weight(pszTempStr);
        for (j = 0; j < g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.s32ExpCount; j++)
        {
            g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.pstH265VencFaceCfg[i * g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.s32ExpCount + j].u8HedgeThr_I = MAEWeight[j];
        }
        //h265venc:u8HedgeCnt_I
        snprintf(szTempStr, sizeof(szTempStr), "h265venc:u8HedgeCnt_I_%d", i);
        pszTempStr = iniparser_getstr(g_Sceneautodictionary, szTempStr);
        if (NULL == pszTempStr)
        {
            printf("h265venc:u8HedgeCnt_I_%d error\n", i);
            return HI_FAILURE;
        }
        s32Temp = Weight(pszTempStr);
        for (j = 0; j < g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.s32ExpCount; j++)
        {
            g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.pstH265VencFaceCfg[i * g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.s32ExpCount + j].u8HedgeCnt_I = MAEWeight[j];
        }
        //h265venc:bStroEdgeEn_I
        snprintf(szTempStr, sizeof(szTempStr), "h265venc:bStroEdgeEn_I_%d", i);
        pszTempStr = iniparser_getstr(g_Sceneautodictionary, szTempStr);
        if (NULL == pszTempStr)
        {
            printf("h265venc:bStroEdgeEn_I_%d error\n", i);
            return HI_FAILURE;
        }
        s32Temp = Weight(pszTempStr);
        for (j = 0; j < g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.s32ExpCount; j++)
        {
            g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.pstH265VencFaceCfg[i * g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.s32ExpCount + j].bStroEdgeEn_I = (HI_BOOL)MAEWeight[j];
        }
        //h265venc:u32StroEdgeQpDelta_I
        snprintf(szTempStr, sizeof(szTempStr), "h265venc:u32StroEdgeQpDelta_I_%d", i);
        pszTempStr = iniparser_getstr(g_Sceneautodictionary, szTempStr);
        if (NULL == pszTempStr)
        {
            printf("h265venc:u32StroEdgeQpDelta_I_%d error\n", i);
            return HI_FAILURE;
        }
        s32Temp = Weight(pszTempStr);
        for (j = 0; j < g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.s32ExpCount; j++)
        {
            g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.pstH265VencFaceCfg[i * g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.s32ExpCount + j].u32StroEdgeQpDelta_I = MAEWeight[j];
        }
        //h265venc:u8HedgeThr_P
        snprintf(szTempStr, sizeof(szTempStr), "h265venc:u8HedgeThr_P_%d", i);
        pszTempStr = iniparser_getstr(g_Sceneautodictionary, szTempStr);
        if (NULL == pszTempStr)
        {
            printf("h265venc:u8HedgeThr_P_%d error\n", i);
            return HI_FAILURE;
        }
        s32Temp = Weight(pszTempStr);
        for (j = 0; j < g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.s32ExpCount; j++)
        {
            g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.pstH265VencFaceCfg[i * g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.s32ExpCount + j].u8HedgeThr_P = MAEWeight[j];
        }
        //h265venc:u8HedgeCnt_P
        snprintf(szTempStr, sizeof(szTempStr), "h265venc:u8HedgeCnt_P_%d", i);
        pszTempStr = iniparser_getstr(g_Sceneautodictionary, szTempStr);
        if (NULL == pszTempStr)
        {
            printf("h265venc:u8HedgeCnt_P_%d error\n", i);
            return HI_FAILURE;
        }
        s32Temp = Weight(pszTempStr);
        for (j = 0; j < g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.s32ExpCount; j++)
        {
            g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.pstH265VencFaceCfg[i * g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.s32ExpCount + j].u8HedgeCnt_P = MAEWeight[j];
        }
        //h265venc:bStroEdgeEn_P
        snprintf(szTempStr, sizeof(szTempStr), "h265venc:bStroEdgeEn_P_%d", i);
        pszTempStr = iniparser_getstr(g_Sceneautodictionary, szTempStr);
        if (NULL == pszTempStr)
        {
            printf("h265venc:bStroEdgeEn_P_%d error\n", i);
            return HI_FAILURE;
        }
        s32Temp = Weight(pszTempStr);
        for (j = 0; j < g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.s32ExpCount; j++)
        {
            g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.pstH265VencFaceCfg[i * g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.s32ExpCount + j].bStroEdgeEn_P = (HI_BOOL)MAEWeight[j];
        }
        //h265venc:u32StroEdgeQpDelta_P
        snprintf(szTempStr, sizeof(szTempStr), "h265venc:u32StroEdgeQpDelta_P_%d", i);
        pszTempStr = iniparser_getstr(g_Sceneautodictionary, szTempStr);
        if (NULL == pszTempStr)
        {
            printf("h265venc:u32StroEdgeQpDelta_P_%d error\n", i);
            return HI_FAILURE;
        }
        s32Temp = Weight(pszTempStr);
        for (j = 0; j < g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.s32ExpCount; j++)
        {
            g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.pstH265VencFaceCfg[i * g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.s32ExpCount + j].u32StroEdgeQpDelta_P = MAEWeight[j];
        }
        //h265venc:bImproveEn_I
        snprintf(szTempStr, sizeof(szTempStr), "h265venc:bImproveEn_I_%d", i);
        pszTempStr = iniparser_getstr(g_Sceneautodictionary, szTempStr);
        if (NULL == pszTempStr)
        {
            printf("h265venc:bImproveEn_I_%d error\n", i);
            return HI_FAILURE;
        }
        s32Temp = Weight(pszTempStr);
        for (j = 0; j < g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.s32ExpCount; j++)
        {
            g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.pstH265VencFaceCfg[i * g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.s32ExpCount + j].bImproveEn_I = (HI_BOOL)MAEWeight[j];
        }
        //h265venc:bImproveEn_P
        snprintf(szTempStr, sizeof(szTempStr), "h265venc:bImproveEn_P_%d", i);
        pszTempStr = iniparser_getstr(g_Sceneautodictionary, szTempStr);
        if (NULL == pszTempStr)
        {
            printf("h265venc:bImproveEn_P_%d error\n", i);
            return HI_FAILURE;
        }
        s32Temp = Weight(pszTempStr);
        for (j = 0; j < g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.s32ExpCount; j++)
        {
            g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.pstH265VencFaceCfg[i * g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.s32ExpCount + j].bImproveEn_P = (HI_BOOL)MAEWeight[j];
        }
        //h265venc:u32Norm32MaxNum_P
        snprintf(szTempStr, sizeof(szTempStr), "h265venc:u32Norm32MaxNum_P_%d", i);
        pszTempStr = iniparser_getstr(g_Sceneautodictionary, szTempStr);
        if (NULL == pszTempStr)
        {
            printf("h265venc:u32Norm32MaxNum_P_%d error\n", i);
            return HI_FAILURE;
        }
        s32Temp = Weight(pszTempStr);
        for (j = 0; j < g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.s32ExpCount; j++)
        {
            g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.pstH265VencFaceCfg[i * g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.s32ExpCount + j].u32Norm32MaxNum_P = MAEWeight[j];
        }
        //h265venc:u32Norm16MaxNum_P
        snprintf(szTempStr, sizeof(szTempStr), "h265venc:u32Norm16MaxNum_P_%d", i);
        pszTempStr = iniparser_getstr(g_Sceneautodictionary, szTempStr);
        if (NULL == pszTempStr)
        {
            printf("h265venc:u32Norm16MaxNum_P_%d error\n", i);
            return HI_FAILURE;
        }
        s32Temp = Weight(pszTempStr);
        for (j = 0; j < g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.s32ExpCount; j++)
        {
            g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.pstH265VencFaceCfg[i * g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.s32ExpCount + j].u32Norm16MaxNum_P = MAEWeight[j];
        }
        //h265venc:u32Norm32ProtectNum_P
        snprintf(szTempStr, sizeof(szTempStr), "h265venc:u32Norm32ProtectNum_P_%d", i);
        pszTempStr = iniparser_getstr(g_Sceneautodictionary, szTempStr);
        if (NULL == pszTempStr)
        {
            printf("h265venc:u32Norm32ProtectNum_P_%d error\n", i);
            return HI_FAILURE;
        }
        s32Temp = Weight(pszTempStr);
        for (j = 0; j < g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.s32ExpCount; j++)
        {
            g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.pstH265VencFaceCfg[i * g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.s32ExpCount + j].u32Norm32ProtectNum_P = MAEWeight[j];
        }
        //h265venc:u32Norm16ProtectNum_P
        snprintf(szTempStr, sizeof(szTempStr), "h265venc:u32Norm16ProtectNum_P_%d", i);
        pszTempStr = iniparser_getstr(g_Sceneautodictionary, szTempStr);
        if (NULL == pszTempStr)
        {
            printf("h265venc:u32Norm16ProtectNum_P_%d error\n", i);
            return HI_FAILURE;
        }
        s32Temp = Weight(pszTempStr);
        for (j = 0; j < g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.s32ExpCount; j++)
        {
            g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.pstH265VencFaceCfg[i * g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.s32ExpCount + j].u32Norm16ProtectNum_P = MAEWeight[j];
        }
        //h265venc:u32Skin32MaxNum_P
        snprintf(szTempStr, sizeof(szTempStr), "h265venc:u32Skin32MaxNum_P_%d", i);
        pszTempStr = iniparser_getstr(g_Sceneautodictionary, szTempStr);
        if (NULL == pszTempStr)
        {
            printf("h265venc:u32Skin32MaxNum_P_%d error\n", i);
            return HI_FAILURE;
        }
        s32Temp = Weight(pszTempStr);
        for (j = 0; j < g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.s32ExpCount; j++)
        {
            g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.pstH265VencFaceCfg[i * g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.s32ExpCount + j].u32Skin32MaxNum_P = MAEWeight[j];
        }
        //h265venc:u32Skin16MaxNum_P
        snprintf(szTempStr, sizeof(szTempStr), "h265venc:u32Skin16MaxNum_P_%d", i);
        pszTempStr = iniparser_getstr(g_Sceneautodictionary, szTempStr);
        if (NULL == pszTempStr)
        {
            printf("h265venc:u32Skin16MaxNum_P_%d error\n", i);
            return HI_FAILURE;
        }
        s32Temp = Weight(pszTempStr);
        for (j = 0; j < g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.s32ExpCount; j++)
        {
            g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.pstH265VencFaceCfg[i * g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.s32ExpCount + j].u32Skin16MaxNum_P = MAEWeight[j];
        }
        //h265venc:u32Skin32ProtectNum_P
        snprintf(szTempStr, sizeof(szTempStr), "h265venc:u32Skin32ProtectNum_P_%d", i);
        pszTempStr = iniparser_getstr(g_Sceneautodictionary, szTempStr);
        if (NULL == pszTempStr)
        {
            printf("h265venc:u32Skin32ProtectNum_P_%d error\n", i);
            return HI_FAILURE;
        }
        s32Temp = Weight(pszTempStr);
        for (j = 0; j < g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.s32ExpCount; j++)
        {
            g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.pstH265VencFaceCfg[i * g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.s32ExpCount + j].u32Skin32ProtectNum_P = MAEWeight[j];
        }
        //h265venc:u32Skin16ProtectNum_P
        snprintf(szTempStr, sizeof(szTempStr), "h265venc:u32Skin16ProtectNum_P_%d", i);
        pszTempStr = iniparser_getstr(g_Sceneautodictionary, szTempStr);
        if (NULL == pszTempStr)
        {
            printf("h265venc:u32Skin16ProtectNum_P_%d error\n", i);
            return HI_FAILURE;
        }
        s32Temp = Weight(pszTempStr);
        for (j = 0; j < g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.s32ExpCount; j++)
        {
            g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.pstH265VencFaceCfg[i * g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.s32ExpCount + j].u32Skin16ProtectNum_P = MAEWeight[j];
        }
        //h265venc:u32Still32MaxNum_P
        snprintf(szTempStr, sizeof(szTempStr), "h265venc:u32Still32MaxNum_P_%d", i);
        pszTempStr = iniparser_getstr(g_Sceneautodictionary, szTempStr);
        if (NULL == pszTempStr)
        {
            printf("h265venc:u32Still32MaxNum_P_%d error\n", i);
            return HI_FAILURE;
        }
        s32Temp = Weight(pszTempStr);
        for (j = 0; j < g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.s32ExpCount; j++)
        {
            g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.pstH265VencFaceCfg[i * g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.s32ExpCount + j].u32Still32MaxNum_P = MAEWeight[j];
        }
        //h265venc:u32Still16MaxNum_P
        snprintf(szTempStr, sizeof(szTempStr), "h265venc:u32Still16MaxNum_P_%d", i);
        pszTempStr = iniparser_getstr(g_Sceneautodictionary, szTempStr);
        if (NULL == pszTempStr)
        {
            printf("h265venc:u32Still16MaxNum_P_%d error\n", i);
            return HI_FAILURE;
        }
        s32Temp = Weight(pszTempStr);
        for (j = 0; j < g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.s32ExpCount; j++)
        {
            g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.pstH265VencFaceCfg[i * g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.s32ExpCount + j].u32Still16MaxNum_P = MAEWeight[j];
        }
        //h265venc:u32Still32ProtectNum_P
        snprintf(szTempStr, sizeof(szTempStr), "h265venc:u32Still32ProtectNum_P_%d", i);
        pszTempStr = iniparser_getstr(g_Sceneautodictionary, szTempStr);
        if (NULL == pszTempStr)
        {
            printf("h265venc:u32Still32ProtectNum_P_%d error\n", i);
            return HI_FAILURE;
        }
        s32Temp = Weight(pszTempStr);
        for (j = 0; j < g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.s32ExpCount; j++)
        {
            g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.pstH265VencFaceCfg[i * g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.s32ExpCount + j].u32Still32ProtectNum_P = MAEWeight[j];
        }
        //h265venc:u32Still16ProtectNum_P
        snprintf(szTempStr, sizeof(szTempStr), "h265venc:u32Still16ProtectNum_P_%d", i);
        pszTempStr = iniparser_getstr(g_Sceneautodictionary, szTempStr);
        if (NULL == pszTempStr)
        {
            printf("h265venc:u32Still16ProtectNum_P_%d error\n", i);
            return HI_FAILURE;
        }
        s32Temp = Weight(pszTempStr);
        for (j = 0; j < g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.s32ExpCount; j++)
        {
            g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.pstH265VencFaceCfg[i * g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.s32ExpCount + j].u32Still16ProtectNum_P = MAEWeight[j];
        }
        //h265venc:u32Hedge32MaxNum_P
        snprintf(szTempStr, sizeof(szTempStr), "h265venc:u32Hedge32MaxNum_P_%d", i);
        pszTempStr = iniparser_getstr(g_Sceneautodictionary, szTempStr);
        if (NULL == pszTempStr)
        {
            printf("h265venc:u32Hedge32MaxNum_P_%d error\n", i);
            return HI_FAILURE;
        }
        s32Temp = Weight(pszTempStr);
        for (j = 0; j < g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.s32ExpCount; j++)
        {
            g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.pstH265VencFaceCfg[i * g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.s32ExpCount + j].u32Hedge32MaxNum_P = MAEWeight[j];
        }
        //h265venc:u32Hedge16MaxNum_P
        snprintf(szTempStr, sizeof(szTempStr), "h265venc:u32Hedge16MaxNum_P_%d", i);
        pszTempStr = iniparser_getstr(g_Sceneautodictionary, szTempStr);
        if (NULL == pszTempStr)
        {
            printf("h265venc:u32Hedge16MaxNum_P_%d error\n", i);
            return HI_FAILURE;
        }
        s32Temp = Weight(pszTempStr);
        for (j = 0; j < g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.s32ExpCount; j++)
        {
            g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.pstH265VencFaceCfg[i * g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.s32ExpCount + j].u32Hedge16MaxNum_P = MAEWeight[j];
        }
        //h265venc:u32Hedge32ProtectNum_P
        snprintf(szTempStr, sizeof(szTempStr), "h265venc:u32Hedge32ProtectNum_P_%d", i);
        pszTempStr = iniparser_getstr(g_Sceneautodictionary, szTempStr);
        if (NULL == pszTempStr)
        {
            printf("h265venc:u32Hedge32ProtectNum_P_%d error\n", i);
            return HI_FAILURE;
        }
        s32Temp = Weight(pszTempStr);
        for (j = 0; j < g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.s32ExpCount; j++)
        {
            g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.pstH265VencFaceCfg[i * g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.s32ExpCount + j].u32Hedge32ProtectNum_P = MAEWeight[j];
        }
        //h265venc:u32Hedge16ProtectNum_P
        snprintf(szTempStr, sizeof(szTempStr), "h265venc:u32Hedge16ProtectNum_P_%d", i);
        pszTempStr = iniparser_getstr(g_Sceneautodictionary, szTempStr);
        if (NULL == pszTempStr)
        {
            printf("h265venc:u32Hedge16ProtectNum_P_%d error\n", i);
            return HI_FAILURE;
        }
        s32Temp = Weight(pszTempStr);
        for (j = 0; j < g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.s32ExpCount; j++)
        {
            g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.pstH265VencFaceCfg[i * g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.s32ExpCount + j].u32Hedge16ProtectNum_P = MAEWeight[j];
        }
    }

    /**************3dnr**************/
    //3dnr:3DnrIsoCount
    s32Temp = 0;
	s32Temp = iniparser_getint(g_Sceneautodictionary, "3dnr:3DnrIsoCount", HI_FAILURE);
    if (HI_FAILURE == s32Temp)
    {
    	printf("3dnr:3DnrIsoCount failed\n");
        return HI_FAILURE;
    }    
    g_stINIPara.stIni3dnr.s323DnrIsoCount = s32Temp;
    g_stINIPara.stIni3dnr.pu323DnrIsoThresh = (HI_U32 *)malloc((g_stINIPara.stIni3dnr.s323DnrIsoCount) * sizeof(HI_U32));
    CHECK_NULL_PTR(g_stINIPara.stIni3dnr.pu323DnrIsoThresh);
    g_stINIPara.stIni3dnr.pst3dnrParam = (SCENEAUTO_INIPARAM_3DNRCFG_S *)malloc((g_stINIPara.stIni3dnr.s323DnrIsoCount) * sizeof(SCENEAUTO_INIPARAM_3DNRCFG_S));
    CHECK_NULL_PTR(g_stINIPara.stIni3dnr.pst3dnrParam);

    //3dnr:3DnrIsoThresh
    pszTempStr = iniparser_getstr(g_Sceneautodictionary, "3dnr:3DnrIsoThresh");
    if (NULL == pszTempStr)
    {
        printf("3dnr:3DnrIsoThresh error\n");
        return HI_FAILURE;
    }
    s32Temp = Weight(pszTempStr);
    for (i = 0; i < g_stINIPara.stIni3dnr.s323DnrIsoCount; i++)
    {
        g_stINIPara.stIni3dnr.pu323DnrIsoThresh[i] = MAEWeight[i];
    }

    for (i = 0; i < g_stINIPara.stIni3dnr.s323DnrIsoCount; i++)
    {
        //3dnr:SBS
        snprintf(szTempStr, sizeof(szTempStr), "3dnr:SBS_%d", i);
        pszTempStr = iniparser_getstr(g_Sceneautodictionary, szTempStr);
        if (NULL == pszTempStr)
        {
            printf("3dnr:SBS_%d error\n", i);
            return HI_FAILURE;
        }
        s32Temp = Weight(pszTempStr);
        for (j = 0; j < 3; j++)
        {
            g_stINIPara.stIni3dnr.pst3dnrParam[i].u8SBS[j] = MAEWeight[j];
        }
        //3dnr:PSFS
        snprintf(szTempStr, sizeof(szTempStr), "3dnr:PSFS_%d", i);
        s32Temp = 0;
        s32Temp = iniparser_getint(g_Sceneautodictionary, szTempStr, HI_FAILURE);
        if (HI_FAILURE == s32Temp)
        {
            printf("3dnr:PSFS_%d failed\n", i);
            return HI_FAILURE;
        }
        g_stINIPara.stIni3dnr.pst3dnrParam[i].u16PSFS = s32Temp;
        //3dnr:SBT
        snprintf(szTempStr, sizeof(szTempStr), "3dnr:SBT_%d", i);
        pszTempStr = iniparser_getstr(g_Sceneautodictionary, szTempStr);
        if (NULL == pszTempStr)
        {
            printf("3dnr:SBT_%d error\n", i);
            return HI_FAILURE;
        }
        s32Temp = Weight(pszTempStr);
        for (j = 0; j < 3; j++)
        {
            g_stINIPara.stIni3dnr.pst3dnrParam[i].u8SBT[j] = MAEWeight[j];
        }
        //3dnr:SDS
        snprintf(szTempStr, sizeof(szTempStr), "3dnr:SDS_%d", i);
        pszTempStr = iniparser_getstr(g_Sceneautodictionary, szTempStr);
        if (NULL == pszTempStr)
        {
            printf("3dnr:SDS_%d error\n", i);
            return HI_FAILURE;
        }
        s32Temp = Weight(pszTempStr);
        for (j = 0; j < 3; j++)
        {
            g_stINIPara.stIni3dnr.pst3dnrParam[i].u8SDS[j] = MAEWeight[j];
        }
        //3dnr:SDT
        snprintf(szTempStr, sizeof(szTempStr), "3dnr:SDT_%d", i);
        pszTempStr = iniparser_getstr(g_Sceneautodictionary, szTempStr);
        if (NULL == pszTempStr)
        {
            printf("3dnr:SDT_%d error\n", i);
            return HI_FAILURE;
        }
        s32Temp = Weight(pszTempStr);
        for (j = 0; j < 3; j++)
        {
            g_stINIPara.stIni3dnr.pst3dnrParam[i].u8SDT[j] = MAEWeight[j];
        }
        //3dnr:SBF
        snprintf(szTempStr, sizeof(szTempStr), "3dnr:SBF_%d", i);
        pszTempStr = iniparser_getstr(g_Sceneautodictionary, szTempStr);
        if (NULL == pszTempStr)
        {
            printf("3dnr:SBF_%d error\n", i);
            return HI_FAILURE;
        }
        s32Temp = Weight(pszTempStr);
        for (j = 0; j < 3; j++)
        {
            g_stINIPara.stIni3dnr.pst3dnrParam[i].u16SBF[j] = MAEWeight[j];
        }
        //3dnr:SFC
        snprintf(szTempStr, sizeof(szTempStr), "3dnr:SFC_%d", i);
        s32Temp = 0;
        s32Temp = iniparser_getint(g_Sceneautodictionary, szTempStr, HI_FAILURE);
        if (HI_FAILURE == s32Temp)
        {
            printf("3dnr:SFC_%d failed\n", i);
            return HI_FAILURE;
        }
        g_stINIPara.stIni3dnr.pst3dnrParam[i].u8SFC = s32Temp;
    //3dnr:hlcdatafile
        snprintf(szTempStr, sizeof(szTempStr), "3dnr:SHP_%d", i);
        pszTempStr = iniparser_getstr(g_Sceneautodictionary, szTempStr);
        if (NULL == pszTempStr)
        {
            printf("3dnr:SHP_%d error\n", i);
            return HI_FAILURE;
        }
        s32Temp = Weight(pszTempStr);
        for (j = 0; j < 3; j++)
        {
            g_stINIPara.stIni3dnr.pst3dnrParam[i].u8SHP[j] = MAEWeight[j];
        }
        //3dnr:TFC
        snprintf(szTempStr, sizeof(szTempStr), "3dnr:TFC_%d", i);
        s32Temp = 0;
        s32Temp = iniparser_getint(g_Sceneautodictionary, szTempStr, HI_FAILURE);
        if (HI_FAILURE == s32Temp)
        {
            printf("3dnr:TFC_%d failed\n", i);
            return HI_FAILURE;
        }
        g_stINIPara.stIni3dnr.pst3dnrParam[i].u8TFC = s32Temp;
        //3dnr:TFS
        snprintf(szTempStr, sizeof(szTempStr), "3dnr:TFS_%d", i);
        pszTempStr = iniparser_getstr(g_Sceneautodictionary, szTempStr);
        if (NULL == pszTempStr)
        {
            printf("3dnr:TFS_%d error\n", i);
            return HI_FAILURE;
        }
        s32Temp = Weight(pszTempStr);
        for (j = 0; j < 3; j++)
        {
            g_stINIPara.stIni3dnr.pst3dnrParam[i].u16TFS[j] = MAEWeight[j];
        }
        //3dnr:TFR
        snprintf(szTempStr, sizeof(szTempStr), "3dnr:TFR_%d", i);
        pszTempStr = iniparser_getstr(g_Sceneautodictionary, szTempStr);
        if (NULL == pszTempStr)
        {
            printf("3dnr:TFR_%d error\n", i);
            return HI_FAILURE;
        }
        s32Temp = Weight(pszTempStr);
        for (j = 0; j < 3; j++)
        {
            g_stINIPara.stIni3dnr.pst3dnrParam[i].u16TFR[j] = MAEWeight[j];
        }
        //3dnr:MDZ
        snprintf(szTempStr, sizeof(szTempStr), "3dnr:MDZ_%d", i);
        pszTempStr = iniparser_getstr(g_Sceneautodictionary, szTempStr);
        if (NULL == pszTempStr)
        {
            printf("3dnr:MDZ_%d error\n", i);
            return HI_FAILURE;
        }
        s32Temp = Weight(pszTempStr);
        for (j = 0; j < 3; j++)
        {
            g_stINIPara.stIni3dnr.pst3dnrParam[i].u16MDZ[j] = MAEWeight[j];
        }
        //3dnr:POST
        snprintf(szTempStr, sizeof(szTempStr), "3dnr:POST_%d", i);
        s32Temp = 0;
        s32Temp = iniparser_getint(g_Sceneautodictionary, szTempStr, HI_FAILURE);
        if (HI_FAILURE == s32Temp)
        {
            printf("3dnr:POST_%d failed\n", i);
            return HI_FAILURE;
        }
        g_stINIPara.stIni3dnr.pst3dnrParam[i].u16POST = s32Temp;


    }

    /**************BLC**************/
    //BLC:AEStrategyMode
    s32Temp = 0;
    s32Temp = iniparser_getint(g_Sceneautodictionary, "BLC:AEStrategyMode", HI_FAILURE);
    if (HI_FAILURE == s32Temp)
    {
        printf("BLC:AEStrategyMode failed\n");
        return HI_FAILURE;
    }
    g_stINIPara.stBLC.u8AEStrategyMode = s32Temp;
    //BLC:MaxHistOffset
    s32Temp = 0;
    s32Temp = iniparser_getint(g_Sceneautodictionary, "BLC:MaxHistOffset", HI_FAILURE);
    if (HI_FAILURE == s32Temp)
    {
        printf("BLC:MaxHistOffset failed\n");
        return HI_FAILURE;
    }
    g_stINIPara.stBLC.u8MaxHistOffset = s32Temp;
    //BLC:HistRatioSlope
    s32Temp = 0;
    s32Temp = iniparser_getint(g_Sceneautodictionary, "BLC:HistRatioSlope", HI_FAILURE);
    if (HI_FAILURE == s32Temp)
    {
        printf("BLC:HistRatioSlope failed\n");
        return HI_FAILURE;
    }
    g_stINIPara.stBLC.u16HistRatioSlope = s32Temp;
    /**************DYNAMIC**************/
    //DYNAMIC:TotalNum_fast
    s32Temp = 0;
    s32Temp = iniparser_getint(g_Sceneautodictionary, "DYNAMIC:TotalNum_fast", HI_FAILURE);
    if (HI_FAILURE == s32Temp)
    {
        printf("DYNAMIC:TotalNum_fast failed\n");
        return HI_FAILURE;
    }
    g_stINIPara.stFastDynamic.s32TotalNum = s32Temp;
    g_stINIPara.stFastDynamic.pstRouteNode = (SCENEAUTO_ROUTE_NODE_S*)malloc((g_stINIPara.stFastDynamic.s32TotalNum) * sizeof (SCENEAUTO_ROUTE_NODE_S));
    CHECK_NULL_PTR(g_stINIPara.stFastDynamic.pstRouteNode);
    //DYNAMIC:IntTime_fast
    pszTempStr = iniparser_getstr(g_Sceneautodictionary, "DYNAMIC:IntTime_fast");
    if (NULL == pszTempStr)
    {
        printf("DYNAMIC:IntTime_fast error\n");
        return HI_FAILURE;
    }
    s32Temp = Weight(pszTempStr);
    for (i = 0; i < g_stINIPara.stFastDynamic.s32TotalNum; i++)
    {
        g_stINIPara.stFastDynamic.pstRouteNode[i].u32IntTime = MAEWeight[i];
    }
    //DYNAMIC:SysGain_fast
    pszTempStr = iniparser_getstr(g_Sceneautodictionary, "DYNAMIC:SysGain_fast");
    if (NULL == pszTempStr)
    {
        printf("DYNAMIC:SysGain_fast error\n");
        return HI_FAILURE;
    }
    s32Temp = Weight(pszTempStr);
    for (i = 0; i < g_stINIPara.stFastDynamic.s32TotalNum; i++)
    {
        g_stINIPara.stFastDynamic.pstRouteNode[i].u32SysGain = MAEWeight[i];
    }
    //DYNAMIC:TotalNum_normal
    s32Temp = 0;
    s32Temp = iniparser_getint(g_Sceneautodictionary, "DYNAMIC:TotalNum_normal", HI_FAILURE);
    if (HI_FAILURE == s32Temp)
    {
        printf("DYNAMIC:TotalNum_normal failed\n");
        return HI_FAILURE;
    }
    g_stINIPara.stNormalDynamic.s32TotalNum = s32Temp;
    g_stINIPara.stNormalDynamic.pstRouteNode = (SCENEAUTO_ROUTE_NODE_S*)malloc((g_stINIPara.stFastDynamic.s32TotalNum) * sizeof (SCENEAUTO_ROUTE_NODE_S));
    CHECK_NULL_PTR(g_stINIPara.stNormalDynamic.pstRouteNode);
    //DYNAMIC:IntTime_normal
    pszTempStr = iniparser_getstr(g_Sceneautodictionary, "DYNAMIC:IntTime_normal");
    if (NULL == pszTempStr)
    {
        printf("DYNAMIC:IntTime_normal error\n");
        return HI_FAILURE;
    }
    s32Temp = Weight(pszTempStr);
    for (i = 0; i < g_stINIPara.stNormalDynamic.s32TotalNum; i++)
    {
        g_stINIPara.stNormalDynamic.pstRouteNode[i].u32IntTime = MAEWeight[i];
    }
    //DYNAMIC:SysGain_normal
    pszTempStr = iniparser_getstr(g_Sceneautodictionary, "DYNAMIC:SysGain_normal");
    if (NULL == pszTempStr)
    {
        printf("DYNAMIC:SysGain_normal error\n");
        return HI_FAILURE;
    }
    s32Temp = Weight(pszTempStr);
    for (i = 0; i < g_stINIPara.stNormalDynamic.s32TotalNum; i++)
    {
        g_stINIPara.stNormalDynamic.pstRouteNode[i].u32SysGain = MAEWeight[i];
    }
    /**************HLC**************/
    //HLC:ExpCompensation
    s32Temp = 0;
    s32Temp = iniparser_getint(g_Sceneautodictionary, "HLC:ExpCompensation", HI_FAILURE);
    if (HI_FAILURE == s32Temp)
    {
        printf("HLC:ExpCompensation failed\n");
        return HI_FAILURE;
    }
    g_stINIPara.stHLC.u8ExpCompensation = s32Temp;
    //HLC:Saturation
    pszTempStr = iniparser_getstr(g_Sceneautodictionary, "HLC:Saturation");
    if (NULL == pszTempStr)
    {
        printf("HLC:Saturation error\n");
        return HI_FAILURE;
    }
    s32Temp = Weight(pszTempStr);
    for (i = 0; i < 16; i++)
    {
        g_stINIPara.stHLC.u8Saturation[i] = MAEWeight[i];
    }
    //HLC:BlackDelayFrame
    s32Temp = 0;
    s32Temp = iniparser_getint(g_Sceneautodictionary, "HLC:BlackDelayFrame", HI_FAILURE);
    if (HI_FAILURE == s32Temp)
    {
        printf("HLC:BlackDelayFrame failed\n");
        return HI_FAILURE;
    }
    g_stINIPara.stHLC.u16BlackDelayFrame = s32Temp;
    //HLC:WhiteDelayFrame
    s32Temp = 0;
    s32Temp = iniparser_getint(g_Sceneautodictionary, "HLC:WhiteDelayFrame", HI_FAILURE);
    if (HI_FAILURE == s32Temp)
    {
        printf("HLC:WhiteDelayFrame failed\n");
        return HI_FAILURE;
    }
    g_stINIPara.stHLC.u16WhiteDelayFrame = s32Temp;

    //HLC:u8Speed
    s32Temp = 0;
    s32Temp = iniparser_getint(g_Sceneautodictionary, "HLC:u8Speed", HI_FAILURE);
    if (HI_FAILURE == s32Temp)
    {
        printf("HLC:u8Speed failed\n");
        return HI_FAILURE;
    }
    g_stINIPara.stHLC.u8Speed = s32Temp;
    //HLC:HistRatioSlope
    s32Temp = 0;
    s32Temp = iniparser_getint(g_Sceneautodictionary, "HLC:HistRatioSlope", HI_FAILURE);
    if (HI_FAILURE == s32Temp)
    {
        printf("HLC:HistRatioSlope failed\n");
        return HI_FAILURE;
    }
    g_stINIPara.stHLC.u16HistRatioSlope = s32Temp;
    //HLC:MaxHistOffset
    s32Temp = 0;
    s32Temp = iniparser_getint(g_Sceneautodictionary, "HLC:MaxHistOffset", HI_FAILURE);
    if (HI_FAILURE == s32Temp)
    {
        printf("HLC:MaxHistOffset failed\n");
        return HI_FAILURE;
    }
    g_stINIPara.stHLC.u8MaxHistOffset = s32Temp;
    //HLC:u8Tolerance
    s32Temp = 0;
    s32Temp = iniparser_getint(g_Sceneautodictionary, "HLC:u8Tolerance", HI_FAILURE);
    if (HI_FAILURE == s32Temp)
    {
        printf("HLC:u8Tolerance failed\n");
        return HI_FAILURE;
    }
    g_stINIPara.stHLC.u8Tolerance = s32Temp;
    //HLC:DCIEnable
    s32Temp = 0;
    s32Temp = iniparser_getint(g_Sceneautodictionary, "HLC:DCIEnable", HI_FAILURE);
    if (HI_FAILURE == s32Temp)
    {
        printf("HLC:DCIEnable failed\n");
        return HI_FAILURE;
    }
    g_stINIPara.stHLC.bDCIEnable = (HI_BOOL)s32Temp;
    //HLC:DCIBlackGain
    s32Temp = 0;
    s32Temp = iniparser_getint(g_Sceneautodictionary, "HLC:DCIBlackGain", HI_FAILURE);
    if (HI_FAILURE == s32Temp)
    {
        printf("HLC:DCIBlackGain failed\n");
        return HI_FAILURE;
    }
    g_stINIPara.stHLC.u32DCIBlackGain = s32Temp;
    //HLC:DCIContrastGain
    s32Temp = 0;
    s32Temp = iniparser_getint(g_Sceneautodictionary, "HLC:DCIContrastGain", HI_FAILURE);
    if (HI_FAILURE == s32Temp)
    {
        printf("HLC:DCIContrastGain failed\n");
        return HI_FAILURE;
    }
    g_stINIPara.stHLC.u32DCIContrastGain = s32Temp;
    //HLC:DCILightGain
    s32Temp = 0;
    s32Temp = iniparser_getint(g_Sceneautodictionary, "HLC:DCILightGain", HI_FAILURE);
    if (HI_FAILURE == s32Temp)
    {
        printf("HLC:DCILightGain failed\n");
        return HI_FAILURE;
    }
    g_stINIPara.stHLC.u32DCILightGain = s32Temp;
    //HLC:DCILightGain
    s32Temp = 0;
    s32Temp = iniparser_getint(g_Sceneautodictionary, "HLC:DCILightGain", HI_FAILURE);
    if (HI_FAILURE == s32Temp)
    {
        printf("HLC:DCILightGain failed\n");
        return HI_FAILURE;
    }
    g_stINIPara.stHLC.u32DCILightGain = s32Temp;
    //HLC:DRCEnable
    s32Temp = 0;
    s32Temp = iniparser_getint(g_Sceneautodictionary, "HLC:DRCEnable", HI_FAILURE);
    if (HI_FAILURE == s32Temp)
    {
        printf("HLC:DRCEnable failed\n");
        return HI_FAILURE;
    }
    g_stINIPara.stHLC.bDRCEnable = (HI_BOOL)s32Temp;
    //HLC:DRCManulEnable
    s32Temp = 0;
    s32Temp = iniparser_getint(g_Sceneautodictionary, "HLC:DRCManulEnable", HI_FAILURE);
    if (HI_FAILURE == s32Temp)
    {
        printf("HLC:DRCManulEnable failed\n");
        return HI_FAILURE;
    }
    g_stINIPara.stHLC.bDRCManulEnable = (HI_BOOL)s32Temp;
    //HLC:DRCStrengthTarget
    s32Temp = 0;
    s32Temp = iniparser_getint(g_Sceneautodictionary, "HLC:DRCStrengthTarget", HI_FAILURE);
    if (HI_FAILURE == s32Temp)
    {
        printf("HLC:DRCStrengthTarget failed\n");
        return HI_FAILURE;
    }
    g_stINIPara.stHLC.u32DRCStrengthTarget = s32Temp;
    //HLC:gamma
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
        g_stINIPara.stHLC.u16GammaTable[i] = MAEWeight[i];
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
        g_stINIPara.stHLC.u16GammaTable[s32Offset + i] = MAEWeight[i];
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
        g_stINIPara.stHLC.u16GammaTable[s32Offset + i] = MAEWeight[i];
    }
    /**************IR**************/
    //IR:ExpCompensation
    s32Temp = 0;
    s32Temp = iniparser_getint(g_Sceneautodictionary, "IR:ExpCount", HI_FAILURE);
    if (HI_FAILURE == s32Temp)
    {
        printf("IR:ExpCount failed\n");
        return HI_FAILURE;
    }
    g_stINIPara.stIR.s32ExpCount = s32Temp;
    g_stINIPara.stIR.pu32ExpThreshLtoH = (HI_U32*)malloc((g_stINIPara.stIR.s32ExpCount) * sizeof(HI_U32));
    CHECK_NULL_PTR(g_stINIPara.stIR.pu32ExpThreshLtoH);
    g_stINIPara.stIR.pu32ExpThreshHtoL = (HI_U32*)malloc((g_stINIPara.stIR.s32ExpCount) * sizeof(HI_U32));
    CHECK_NULL_PTR(g_stINIPara.stIR.pu32ExpThreshHtoL);
    g_stINIPara.stIR.pu8ExpCompensation = (HI_U8*)malloc((g_stINIPara.stIR.s32ExpCount) * sizeof(HI_U8));
    CHECK_NULL_PTR(g_stINIPara.stIR.pu8ExpCompensation);
    g_stINIPara.stIR.pu8MaxHistOffset = (HI_U8*)malloc((g_stINIPara.stIR.s32ExpCount) * sizeof(HI_U8));
    CHECK_NULL_PTR(g_stINIPara.stIR.pu8MaxHistOffset);

    //IR:ExpThreshLtoH
    pszTempStr = iniparser_getstr(g_Sceneautodictionary, "IR:ExpThreshLtoH");
    if (NULL == pszTempStr)
    {
        printf("IR:ExpThreshLtoH error\n");
        return HI_FAILURE;
    }
    s32Temp = Weight(pszTempStr);
    for (i = 0; i < g_stINIPara.stIR.s32ExpCount; i++)
    {
        g_stINIPara.stIR.pu32ExpThreshLtoH[i] = MAEWeight[i];
    }

    //IR:ExpThreshHtoL
    pszTempStr = iniparser_getstr(g_Sceneautodictionary, "IR:ExpThreshHtoL");
    if (NULL == pszTempStr)
    {
        printf("IR:ExpThreshHtoL error\n");
        return HI_FAILURE;
    }
    s32Temp = Weight(pszTempStr);
    for (i = 0; i < g_stINIPara.stIR.s32ExpCount; i++)
    {
        g_stINIPara.stIR.pu32ExpThreshHtoL[i] = MAEWeight[i];
    }

    //IR:ExpCompensation
    pszTempStr = iniparser_getstr(g_Sceneautodictionary, "IR:ExpCompensation");
    if (NULL == pszTempStr)
    {
        printf("IR:ExpCompensation error\n");
        return HI_FAILURE;
    }
    s32Temp = Weight(pszTempStr);
    for (i = 0; i < g_stINIPara.stIR.s32ExpCount; i++)
    {
        g_stINIPara.stIR.pu8ExpCompensation[i] = MAEWeight[i];
    }

    //IR:MaxHistOffset
    pszTempStr = iniparser_getstr(g_Sceneautodictionary, "IR:MaxHistOffset");
    if (NULL == pszTempStr)
    {
        printf("IR:MaxHistOffset error\n");
        return HI_FAILURE;
    }
    s32Temp = Weight(pszTempStr);
    for (i = 0; i < g_stINIPara.stIR.s32ExpCount; i++)
    {
        g_stINIPara.stIR.pu8MaxHistOffset[i] = MAEWeight[i];
    }

    //IR:u16HistRatioSlope
    s32Temp = 0;
    s32Temp = iniparser_getint(g_Sceneautodictionary, "IR:u16HistRatioSlope", HI_FAILURE);
    if (HI_FAILURE == s32Temp)
    {
        printf("IR:u16HistRatioSlope failed\n");
        return HI_FAILURE;
    }
    g_stINIPara.stIR.u16HistRatioSlope = s32Temp;

    //IR:BlackDelayFrame
    s32Temp = 0;
    s32Temp = iniparser_getint(g_Sceneautodictionary, "IR:BlackDelayFrame", HI_FAILURE);
    if (HI_FAILURE == s32Temp)
    {
        printf("IR:BlackDelayFrame failed\n");
        return HI_FAILURE;
    }
    g_stINIPara.stIR.u16BlackDelayFrame = s32Temp;

    //IR:WhiteDelayFrame
    s32Temp = 0;
    s32Temp = iniparser_getint(g_Sceneautodictionary, "IR:WhiteDelayFrame", HI_FAILURE);
    if (HI_FAILURE == s32Temp)
    {
        printf("IR:WhiteDelayFrame failed\n");
        return HI_FAILURE;
    }
    g_stINIPara.stIR.u16WhiteDelayFrame = s32Temp;

    //IR:u8Tolerance
    s32Temp = 0;
    s32Temp = iniparser_getint(g_Sceneautodictionary, "IR:u8Tolerance", HI_FAILURE);
    if (HI_FAILURE == s32Temp)
    {
        printf("IR:u8Tolerance failed\n");
        return HI_FAILURE;
    }
    g_stINIPara.stIR.u8Tolerance = s32Temp;
    //IR:u8Speed
    s32Temp = 0;
    s32Temp = iniparser_getint(g_Sceneautodictionary, "IR:u8Speed", HI_FAILURE);
    if (HI_FAILURE == s32Temp)
    {
        printf("IR:u8Speed failed\n");
        return HI_FAILURE;
    }
    g_stINIPara.stIR.u8Speed = s32Temp;

    //IR:DCIEnable
    s32Temp = 0;
    s32Temp = iniparser_getint(g_Sceneautodictionary, "IR:DCIEnable", HI_FAILURE);
    if (HI_FAILURE == s32Temp)
    {
        printf("IR:DCIEnable failed\n");
        return HI_FAILURE;
    }
    g_stINIPara.stIR.bDCIEnable = (HI_BOOL)s32Temp;

    //IR:DCIBlackGain
    s32Temp = 0;
    s32Temp = iniparser_getint(g_Sceneautodictionary, "IR:DCIBlackGain", HI_FAILURE);
    if (HI_FAILURE == s32Temp)
    {
        printf("IR:DCIBlackGain failed\n");
        return HI_FAILURE;
    }
    g_stINIPara.stIR.u32DCIBlackGain = s32Temp;

    //IR:DCIContrastGain
    s32Temp = 0;
    s32Temp = iniparser_getint(g_Sceneautodictionary, "IR:DCIContrastGain", HI_FAILURE);
    if (HI_FAILURE == s32Temp)
    {
        printf("IR:DCIContrastGain failed\n");
        return HI_FAILURE;
    }
    g_stINIPara.stIR.u32DCIContrastGain = s32Temp;

    //IR:DCILightGain
    s32Temp = 0;
    s32Temp = iniparser_getint(g_Sceneautodictionary, "IR:DCILightGain", HI_FAILURE);
    if (HI_FAILURE == s32Temp)
    {
        printf("IR:DCILightGain failed\n");
        return HI_FAILURE;
    }
    g_stINIPara.stIR.u32DCILightGain = s32Temp;
    s32Temp = 0;
    s32Temp = iniparser_getint(g_Sceneautodictionary, "IR:IRu16Slope", HI_FAILURE);
    if (HI_FAILURE == s32Temp)
    {
        printf("IR:IRu16Slope failed\n");
        return HI_FAILURE;
    }
    g_stINIPara.stIR.u16Slope = s32Temp;

    //IR:au8LumThresh
    pszTempStr = iniparser_getstr(g_Sceneautodictionary, "IR:au8LumThresh");
    if (NULL == pszTempStr)
    {
        printf("IR:au8LumThresh error\n");
        return HI_FAILURE;
    }
    s32Temp = Weight(pszTempStr);
    for (i = 0; i < 16; i++)
    {
        g_stINIPara.stIR.au8LumThresh[i] = MAEWeight[i];
    }

    //IR:au8SharpenD
    pszTempStr = iniparser_getstr(g_Sceneautodictionary, "IR:au8SharpenD");
    if (NULL == pszTempStr)
    {
        printf("IR:au8SharpenD error\n");
        return HI_FAILURE;
    }
    s32Temp = Weight(pszTempStr);
    for (i = 0; i < 16; i++)
    {
        g_stINIPara.stIR.au8SharpenD[i] = MAEWeight[i];
    }

    //IR:au8SharpenUd
    pszTempStr = iniparser_getstr(g_Sceneautodictionary, "IR:au8SharpenUd");
    if (NULL == pszTempStr)
    {
        printf("IR:au8SharpenUd error\n");
        return HI_FAILURE;
    }
    s32Temp = Weight(pszTempStr);
    for (i = 0; i < 16; i++)
    {
        g_stINIPara.stIR.au8SharpenUd[i] = MAEWeight[i];
    }

    //IR:au8SharpenRGB
    pszTempStr = iniparser_getstr(g_Sceneautodictionary, "IR:au8SharpenRGB");
    if (NULL == pszTempStr)
    {
        printf("IR:au8SharpenRGB error\n");
        return HI_FAILURE;
    }
    s32Temp = Weight(pszTempStr);
    for (i = 0; i < 16; i++)
    {
        g_stINIPara.stIR.au8SharpenRGB[i] = MAEWeight[i];
    }

    //IR:gamma
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
        g_stINIPara.stIR.u16GammaTable[i] = MAEWeight[i];
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
        g_stINIPara.stIR.u16GammaTable[s32Offset + i] = MAEWeight[i];
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
        g_stINIPara.stIR.u16GammaTable[s32Offset + i] = MAEWeight[i];
    }
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
        s32Temp = Weight(pszTempStr);
        for (j = 0; j < AE_WEIGHT_COLUMN; j++)
        {
            g_stINIPara.stIR.au8Weight[i][j] = MAEWeight[j];
        }
    }

    //IR:3DnrIsoCount
    s32Temp = 0;
	s32Temp = iniparser_getint(g_Sceneautodictionary, "IR:3DnrIsoCount", HI_FAILURE);
    if (HI_FAILURE == s32Temp)
    {
    	printf("IR:3DnrIsoCount failed\n");
        return HI_FAILURE;
    }    
    g_stINIPara.stIR.s323DnrIsoCount = s32Temp;
    g_stINIPara.stIR.pu323DnrIsoThresh = (HI_U32 *)malloc((g_stINIPara.stIR.s323DnrIsoCount) * sizeof(HI_U32));
    CHECK_NULL_PTR(g_stINIPara.stIR.pu323DnrIsoThresh);
    g_stINIPara.stIR.pst3dnrParam = (SCENEAUTO_INIPARAM_3DNRCFG_S *)malloc((g_stINIPara.stIR.s323DnrIsoCount) * sizeof(SCENEAUTO_INIPARAM_3DNRCFG_S));
    CHECK_NULL_PTR(g_stINIPara.stIR.pst3dnrParam);

    //IR:3DnrIsoThresh
    pszTempStr = iniparser_getstr(g_Sceneautodictionary, "IR:3DnrIsoThresh");
    if (NULL == pszTempStr)
    {
        printf("IR:3DnrIsoThresh error\n");
        return HI_FAILURE;
    }
    s32Temp = Weight(pszTempStr);
    for (i = 0; i < g_stINIPara.stIR.s323DnrIsoCount; i++)
    {
        g_stINIPara.stIR.pu323DnrIsoThresh[i] = MAEWeight[i];
    }
    for (i = 0; i < g_stINIPara.stIR.s323DnrIsoCount; i++)
    {
        //IR:SBS
        snprintf(szTempStr, sizeof(szTempStr), "IR:SBS_%d", i);
        pszTempStr = iniparser_getstr(g_Sceneautodictionary, szTempStr);
        if (NULL == pszTempStr)
        {
            printf("IR:SBS_%d error\n", i);
            return HI_FAILURE;
        }
        s32Temp = Weight(pszTempStr);
        for (j = 0; j < 3; j++)
        {
            g_stINIPara.stIR.pst3dnrParam[i].u8SBS[j] = MAEWeight[j];
        }
        //IR:PSFS
        snprintf(szTempStr, sizeof(szTempStr), "IR:PSFS_%d", i);
        s32Temp = 0;
        s32Temp = iniparser_getint(g_Sceneautodictionary, szTempStr, HI_FAILURE);
        if (HI_FAILURE == s32Temp)
        {
            printf("IR:PSFS_%d failed\n", i);
            return HI_FAILURE;
        }
        g_stINIPara.stIR.pst3dnrParam[i].u16PSFS = s32Temp;
        //IR:SBT
        snprintf(szTempStr, sizeof(szTempStr), "IR:SBT_%d", i);
        pszTempStr = iniparser_getstr(g_Sceneautodictionary, szTempStr);
        if (NULL == pszTempStr)
        {
            printf("IR:SBT_%d error\n", i);
            return HI_FAILURE;
        }
        s32Temp = Weight(pszTempStr);
        for (j = 0; j < 3; j++)
        {
            g_stINIPara.stIR.pst3dnrParam[i].u8SBT[j] = MAEWeight[j];
        }
        //IR:SDS
        snprintf(szTempStr, sizeof(szTempStr), "IR:SDS_%d", i);
        pszTempStr = iniparser_getstr(g_Sceneautodictionary, szTempStr);
        if (NULL == pszTempStr)
        {
            printf("IR:SDS_%d error\n", i);
            return HI_FAILURE;
        }
        s32Temp = Weight(pszTempStr);
        for (j = 0; j < 3; j++)
        {
            g_stINIPara.stIR.pst3dnrParam[i].u8SDS[j] = MAEWeight[j];
        }
        //IR:SDT
        snprintf(szTempStr, sizeof(szTempStr), "IR:SDT_%d", i);
        pszTempStr = iniparser_getstr(g_Sceneautodictionary, szTempStr);
        if (NULL == pszTempStr)
        {
            printf("IR:SDT_%d error\n", i);
            return HI_FAILURE;
        }
        s32Temp = Weight(pszTempStr);
        for (j = 0; j < 3; j++)
        {
            g_stINIPara.stIR.pst3dnrParam[i].u8SDT[j] = MAEWeight[j];
        }
        //IR:SBF
        snprintf(szTempStr, sizeof(szTempStr), "IR:SBF_%d", i);
        pszTempStr = iniparser_getstr(g_Sceneautodictionary, szTempStr);
        if (NULL == pszTempStr)
        {
            printf("IR:SBF_%d error\n", i);
            return HI_FAILURE;
        }
        s32Temp = Weight(pszTempStr);
        for (j = 0; j < 3; j++)
        {
            g_stINIPara.stIR.pst3dnrParam[i].u16SBF[j] = MAEWeight[j];
        }
        //IR:SFC
        snprintf(szTempStr, sizeof(szTempStr), "IR:SFC_%d", i);
        s32Temp = 0;
        s32Temp = iniparser_getint(g_Sceneautodictionary, szTempStr, HI_FAILURE);
        if (HI_FAILURE == s32Temp)
        {
            printf("IR:SFC_%d failed\n", i);
            return HI_FAILURE;
        }
        g_stINIPara.stIR.pst3dnrParam[i].u8SFC = s32Temp;
        //IR:SHP
        snprintf(szTempStr, sizeof(szTempStr), "IR:SHP_%d", i);
        pszTempStr = iniparser_getstr(g_Sceneautodictionary, szTempStr);
        if (NULL == pszTempStr)
        {
            printf("IR:SHP_%d error\n", i);
            return HI_FAILURE;
        }
        s32Temp = Weight(pszTempStr);
        for (j = 0; j < 3; j++)
        {
            g_stINIPara.stIR.pst3dnrParam[i].u8SHP[j] = MAEWeight[j];
        }
        //IR:TFC
        snprintf(szTempStr, sizeof(szTempStr), "IR:TFC_%d", i);
        s32Temp = 0;
        s32Temp = iniparser_getint(g_Sceneautodictionary, szTempStr, HI_FAILURE);
        if (HI_FAILURE == s32Temp)
        {
            printf("IR:TFC_%d failed\n", i);
            return HI_FAILURE;
        }
        g_stINIPara.stIR.pst3dnrParam[i].u8TFC = s32Temp;
        //IR:TFS
        snprintf(szTempStr, sizeof(szTempStr), "IR:TFS_%d", i);
        pszTempStr = iniparser_getstr(g_Sceneautodictionary, szTempStr);
        if (NULL == pszTempStr)
        {
            printf("IR:TFS_%d error\n", i);
            return HI_FAILURE;
        }
        s32Temp = Weight(pszTempStr);
        for (j = 0; j < 3; j++)
        {
            g_stINIPara.stIR.pst3dnrParam[i].u16TFS[j] = MAEWeight[j];
        }
        //IR:TFR
        snprintf(szTempStr, sizeof(szTempStr), "IR:TFR_%d", i);
        pszTempStr = iniparser_getstr(g_Sceneautodictionary, szTempStr);
        if (NULL == pszTempStr)
        {
            printf("IR:TFR_%d error\n", i);
            return HI_FAILURE;
        }
        s32Temp = Weight(pszTempStr);
        for (j = 0; j < 3; j++)
        {
            g_stINIPara.stIR.pst3dnrParam[i].u16TFR[j] = MAEWeight[j];
        }
        //IR:MDZ
        snprintf(szTempStr, sizeof(szTempStr), "IR:MDZ_%d", i);
        pszTempStr = iniparser_getstr(g_Sceneautodictionary, szTempStr);
        if (NULL == pszTempStr)
        {
            printf("IR:MDZ_%d error\n", i);
            return HI_FAILURE;
        }
        s32Temp = Weight(pszTempStr);
        for (j = 0; j < 3; j++)
        {
            g_stINIPara.stIR.pst3dnrParam[i].u16MDZ[j] = MAEWeight[j];
        }
        //IR:POST
        snprintf(szTempStr, sizeof(szTempStr), "IR:POST_%d", i);
        s32Temp = 0;
        s32Temp = iniparser_getint(g_Sceneautodictionary, szTempStr, HI_FAILURE);
        if (HI_FAILURE == s32Temp)
        {
            printf("IR:POST_%d failed\n", i);
            return HI_FAILURE;
        }
        g_stINIPara.stIR.pst3dnrParam[i].u16POST = s32Temp;  
    }    
    //HLC:3DnrIsoCount
    s32Temp = 0;
	s32Temp = iniparser_getint(g_Sceneautodictionary, "HLC:3DnrIsoCount", HI_FAILURE);
    if (HI_FAILURE == s32Temp)
    {
    	printf("HLC:3DnrIsoCount failed\n");
        return HI_FAILURE;
    }
    g_stINIPara.stHLC.s323DnrIsoCount = s32Temp;
    g_stINIPara.stHLC.pu323DnrIsoThresh = (HI_U32 *)malloc((g_stINIPara.stHLC.s323DnrIsoCount) * sizeof(HI_U32));
    CHECK_NULL_PTR(g_stINIPara.stHLC.pu323DnrIsoThresh);
    g_stINIPara.stHLC.pst3dnrParam = (SCENEAUTO_INIPARAM_3DNRCFG_S *)malloc((g_stINIPara.stHLC.s323DnrIsoCount) * sizeof(SCENEAUTO_INIPARAM_3DNRCFG_S));
    CHECK_NULL_PTR(g_stINIPara.stHLC.pst3dnrParam);
    //HLC:3DnrIsoThresh
    pszTempStr = iniparser_getstr(g_Sceneautodictionary, "HLC:3DnrIsoThresh");
    if (NULL == pszTempStr)
    {
        printf("HLC:3DnrIsoThresh error\n");
        return HI_FAILURE;
    }
    s32Temp = Weight(pszTempStr);
    for (i = 0; i < g_stINIPara.stHLC.s323DnrIsoCount; i++)
    {
        g_stINIPara.stHLC.pu323DnrIsoThresh[i] = MAEWeight[i];
    }
    for (i = 0; i < g_stINIPara.stHLC.s323DnrIsoCount; i++)
    {
        //HLC:SBS
        snprintf(szTempStr, sizeof(szTempStr), "HLC:SBS_%d", i);
        pszTempStr = iniparser_getstr(g_Sceneautodictionary, szTempStr);
        if (NULL == pszTempStr)
        {
            printf("HLC:SBS_%d error\n", i);
            return HI_FAILURE;
        }
        s32Temp = Weight(pszTempStr);
        for (j = 0; j < 3; j++)
        {
            g_stINIPara.stHLC.pst3dnrParam[i].u8SBS[j] = MAEWeight[j];
        }
        //HLC:PSFS
        snprintf(szTempStr, sizeof(szTempStr), "HLC:PSFS_%d", i);
        s32Temp = 0;
        s32Temp = iniparser_getint(g_Sceneautodictionary, szTempStr, HI_FAILURE);
        if (HI_FAILURE == s32Temp)
        {
            printf("HLC:PSFS_%d failed\n", i);
            return HI_FAILURE;
        }
        g_stINIPara.stHLC.pst3dnrParam[i].u16PSFS = s32Temp;
        //HLC:SBT
        snprintf(szTempStr, sizeof(szTempStr), "HLC:SBT_%d", i);
        pszTempStr = iniparser_getstr(g_Sceneautodictionary, szTempStr);
        if (NULL == pszTempStr)
        {
            printf("HLC:SBT_%d error\n", i);
            return HI_FAILURE;
        }
        s32Temp = Weight(pszTempStr);
        for (j = 0; j < 3; j++)
        {
            g_stINIPara.stHLC.pst3dnrParam[i].u8SBT[j] = MAEWeight[j];
        }
        //HLC:SDS
        snprintf(szTempStr, sizeof(szTempStr), "HLC:SDS_%d", i);
        pszTempStr = iniparser_getstr(g_Sceneautodictionary, szTempStr);
        if (NULL == pszTempStr)
        {
            printf("HLC:SDS_%d error\n", i);
            return HI_FAILURE;
        }
        s32Temp = Weight(pszTempStr);
        for (j = 0; j < 3; j++)
        {
            g_stINIPara.stHLC.pst3dnrParam[i].u8SDS[j] = MAEWeight[j];
        }
        //HLC:SDT
        snprintf(szTempStr, sizeof(szTempStr), "HLC:SDT_%d", i);
        pszTempStr = iniparser_getstr(g_Sceneautodictionary, szTempStr);
        if (NULL == pszTempStr)
        {
            printf("HLC:SDT_%d error\n", i);
            return HI_FAILURE;
        }
        s32Temp = Weight(pszTempStr);
        for (j = 0; j < 3; j++)
        {
            g_stINIPara.stHLC.pst3dnrParam[i].u8SDT[j] = MAEWeight[j];
        }
        //HLC:SBF
        snprintf(szTempStr, sizeof(szTempStr), "HLC:SBF_%d", i);
        pszTempStr = iniparser_getstr(g_Sceneautodictionary, szTempStr);
        if (NULL == pszTempStr)
        {
            printf("HLC:SBF_%d error\n", i);
            return HI_FAILURE;
        }
        s32Temp = Weight(pszTempStr);
        for (j = 0; j < 3; j++)
        {
            g_stINIPara.stHLC.pst3dnrParam[i].u16SBF[j] = MAEWeight[j];
        }
        //HLC:SFC
        snprintf(szTempStr, sizeof(szTempStr), "HLC:SFC_%d", i);
        s32Temp = 0;
        s32Temp = iniparser_getint(g_Sceneautodictionary, szTempStr, HI_FAILURE);
        if (HI_FAILURE == s32Temp)
        {
            printf("HLC:SFC_%d failed\n", i);
            return HI_FAILURE;
        }
        g_stINIPara.stHLC.pst3dnrParam[i].u8SFC = s32Temp;
        //HLC:SHP
        snprintf(szTempStr, sizeof(szTempStr), "HLC:SHP_%d", i);
        pszTempStr = iniparser_getstr(g_Sceneautodictionary, szTempStr);
        if (NULL == pszTempStr)
        {
            printf("HLC:SHP_%d error\n", i);
            return HI_FAILURE;
        }
        s32Temp = Weight(pszTempStr);
        for (j = 0; j < 3; j++)
        {
            g_stINIPara.stHLC.pst3dnrParam[i].u8SHP[j] = MAEWeight[j];
        }
        //HLC:TFC
        snprintf(szTempStr, sizeof(szTempStr), "HLC:TFC_%d", i);
        s32Temp = 0;
        s32Temp = iniparser_getint(g_Sceneautodictionary, szTempStr, HI_FAILURE);
        if (HI_FAILURE == s32Temp)
        {
            printf("HLC:TFC_%d failed\n", i);
            return HI_FAILURE;
        }
        g_stINIPara.stHLC.pst3dnrParam[i].u8TFC = s32Temp;
        //HLC:TFS
        snprintf(szTempStr, sizeof(szTempStr), "HLC:TFS_%d", i);
        pszTempStr = iniparser_getstr(g_Sceneautodictionary, szTempStr);
        if (NULL == pszTempStr)
        {
            printf("HLC:TFS_%d error\n", i);
            return HI_FAILURE;
        }
        s32Temp = Weight(pszTempStr);
        for (j = 0; j < 3; j++)
        {
            g_stINIPara.stHLC.pst3dnrParam[i].u16TFS[j] = MAEWeight[j];
        }
        //HLC:TFR
        snprintf(szTempStr, sizeof(szTempStr), "HLC:TFR_%d", i);
        pszTempStr = iniparser_getstr(g_Sceneautodictionary, szTempStr);
        if (NULL == pszTempStr)
        {
            printf("HLC:TFR_%d error\n", i);
            return HI_FAILURE;
        }
        s32Temp = Weight(pszTempStr);
        for (j = 0; j < 3; j++)
        {
            g_stINIPara.stHLC.pst3dnrParam[i].u16TFR[j] = MAEWeight[j];
        }
        //HLC:MDZ
        snprintf(szTempStr, sizeof(szTempStr), "HLC:MDZ_%d", i);
        pszTempStr = iniparser_getstr(g_Sceneautodictionary, szTempStr);
        if (NULL == pszTempStr)
        {
            printf("HLC:MDZ_%d error\n", i);
            return HI_FAILURE;
        }
        s32Temp = Weight(pszTempStr);
        for (j = 0; j < 3; j++)
        {
            g_stINIPara.stHLC.pst3dnrParam[i].u16MDZ[j] = MAEWeight[j];
        }
        //HLC:POST
        snprintf(szTempStr, sizeof(szTempStr), "HLC:POST_%d", i);
        s32Temp = 0;
        s32Temp = iniparser_getint(g_Sceneautodictionary, szTempStr, HI_FAILURE);
        if (HI_FAILURE == s32Temp)
        {
            printf("HLC:POST_%d failed\n", i);
            return HI_FAILURE;
        }
        g_stINIPara.stHLC.pst3dnrParam[i].u16POST = s32Temp;
    }
    //HLC:au8SharpenUd
    pszTempStr = iniparser_getstr(g_Sceneautodictionary, "HLC:au8SharpenUd");
    if(NULL == pszTempStr)
    {
        printf("HLC:au8SharpenUd error\n");
        return HI_FAILURE;
    }
    s32Temp = Weight(pszTempStr);
    for (i = 0; i < 16; i++)
    {
        g_stINIPara.stHLC.au8SharpenUd[i] = MAEWeight[i];
    } 
    //HLC:au8SharpenRGB
    pszTempStr = iniparser_getstr(g_Sceneautodictionary, "HLC:au8SharpenRGB");
    if(NULL == pszTempStr)
    {
        printf("HLC:au8SharpenRGB error\n");
        return HI_FAILURE;
    }
    s32Temp = Weight(pszTempStr);
    for (i = 0; i < 16; i++)
    {
        g_stINIPara.stHLC.au8SharpenRGB[i] = MAEWeight[i];
    } 
    s32Temp = 0;
    s32Temp = iniparser_getint(g_Sceneautodictionary, "DRC:DRCEnable", HI_FAILURE);
    if (HI_FAILURE == s32Temp)
    {
        printf("DRC:DRCEnable failed\n");
        return HI_FAILURE;
    }
    g_stINIPara.stDRC.bDRCEnable = (HI_BOOL)s32Temp;
    s32Temp = 0;
    s32Temp = iniparser_getint(g_Sceneautodictionary, "DRC:DRCManulEnable", HI_FAILURE);
    if (HI_FAILURE == s32Temp)
    {
        printf("DRC:DRCManulEnable failed\n");
        return HI_FAILURE;
    }
    g_stINIPara.stDRC.bDRCManulEnable = (HI_BOOL)s32Temp;
    s32Temp = 0;
    s32Temp = iniparser_getint(g_Sceneautodictionary, "DRC:DRCStrengthTarget", HI_FAILURE);
    if (HI_FAILURE == s32Temp)
    {
        printf("DRC:DRCStrengthTarget failed\n");
        return HI_FAILURE;
    }
    g_stINIPara.stDRC.u32Strength = (HI_BOOL)s32Temp;
    s32Temp = 0;
    s32Temp = iniparser_getint(g_Sceneautodictionary, "DRC:DRCu32SlopeMax", HI_FAILURE);
    if (HI_FAILURE == s32Temp)
    {
        printf("DRC:DRCu32SlopeMax failed\n");
        return HI_FAILURE;
    }
    g_stINIPara.stDRC.u32SlopeMax = (HI_BOOL)s32Temp;
    s32Temp = 0;
    s32Temp = iniparser_getint(g_Sceneautodictionary, "DRC:DRCu32SlopeMin", HI_FAILURE);
    if (HI_FAILURE == s32Temp)
    {
        printf("DRC:DRCu32SlopeMin failed\n");
        return HI_FAILURE;
    }
    g_stINIPara.stDRC.u32SlopeMin = (HI_BOOL)s32Temp;
    s32Temp = 0;
    s32Temp = iniparser_getint(g_Sceneautodictionary, "DRC:DRCu32VarianceSpace", HI_FAILURE);
    if (HI_FAILURE == s32Temp)
    {
        printf("DRC:DRCu32VarianceSpace failed\n");
        return HI_FAILURE;
    }
    g_stINIPara.stDRC.u32VarianceSpace = (HI_BOOL)s32Temp;
    s32Temp = 0;
    s32Temp = iniparser_getint(g_Sceneautodictionary, "DRC:DRCu32VarianceIntensity", HI_FAILURE);
    if (HI_FAILURE == s32Temp)
    {
        printf("DRC:DRCu32VarianceIntensity failed\n");
        return HI_FAILURE;
    }
    g_stINIPara.stDRC.u32VarianceIntensity = (HI_BOOL)s32Temp;
    s32Temp = 0;
    s32Temp = iniparser_getint(g_Sceneautodictionary, "DRC:DRCu32WhiteLevel", HI_FAILURE);
    if (HI_FAILURE == s32Temp)
    {
        printf("DRC:DRCu32WhiteLevel failed\n");
        return HI_FAILURE;
    }
    g_stINIPara.stDRC.u32WhiteLevel = (HI_BOOL)s32Temp;
    s32Temp = 0;
    s32Temp = iniparser_getint(g_Sceneautodictionary, "DRC:DRCu32BlackLevel", HI_FAILURE);
    if (HI_FAILURE == s32Temp)
    {
        printf("DRC:DRCu32BlackLevel failed\n");
        return HI_FAILURE;
    }
    g_stINIPara.stDRC.u32BlackLevel = (HI_BOOL)s32Temp;
    s32Temp = 0;
    s32Temp = iniparser_getint(g_Sceneautodictionary, "DRC:DRCu32BlackLevel", HI_FAILURE);
    if (HI_FAILURE == s32Temp)
    {
        printf("DRC:DRCu32BlackLevel failed\n");
        return HI_FAILURE;
    }
    g_stINIPara.stDRC.u32BlackLevel = (HI_BOOL)s32Temp;

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

void* SceneAuto_NormalThread(void* pVoid)
{
    HI_S32 s32Ret = HI_SUCCESS;
    HI_S32 i;
    HI_S32 s32AECurPos = 0;
    HI_S32 s32IRAECurPos = 0;
    HI_U32 u32NRProfileVaule;
    HI_U32 u32NRProfileAddr;
    HI_U32 u32Exposure = 0;
    HI_U32 u32Iso = 0;
    HI_U32 u32LastExposure = (MAX_LEVEL + 1);
    HI_S32 s32GammaExpLevel = 0;
    HI_S32 s32LastGammaExpLevel = 0;
    HI_U32 u32GammaExposure = 0;
    HI_U32 u32LastGammaExposure = 0;
    HI_U32 u32GammaCount = 0;
    HI_S32 s32DpExpLevel = 0;
    HI_S32 s32LastDpExpLevel = (MAX_LEVEL + 1);
    HI_S32 s32DemosaicExpLevel = 0;
    HI_S32 s32LastDemosaicExpLevel = (MAX_LEVEL + 1);
    HI_U32 u32Bitrate = 0;
    HI_U32 u32LastBitrate = (MAX_LEVEL + 1);
    HI_S32 s32VencBitrateLevel = 0;
    HI_S32 s32LastVencBitrateLevel = (MAX_LEVEL + 1);
    HI_S32 s32SharpenBitrateLevel = 0;
    HI_S32 s32LastSharpenBitrateLevel = (MAX_LEVEL + 1);
    HI_S32 s32AEBitrateLevel = 0;
    HI_S32 s32LastAEBitrateLevel = (MAX_LEVEL + 1);
    HI_S32 s32DemosaicBitrateLevel = 0;
    HI_S32 s32LastDemosaicBitrateLevel = (MAX_LEVEL + 1);
    HI_S32 s32H265FaceCfgBitrateLevel = 0;
    HI_S32 s32LastH265FaceCfgBitrateLevel = (MAX_LEVEL + 1);
    HI_S32 s32H265FaceCfgExpLevel = 0;
    HI_S32 s32LastH265FaceCfgExpLevel = (MAX_LEVEL + 1);
    HI_S32 s32IspDev = 0;
    HI_S32 s32VencChn = 0;
    HI_S32 s32VpssGrp = 0;
    HI_S32 s32ViDev = 0;
    HI_U64 u64Temp = 0;
    HI_U8 u8DciStrength = 0;
    HI_BOOL bDefogState = HI_FALSE;
    HI_BOOL bDciEnable;
    HI_U32 u32BlackGain;
    HI_U32 u32ContrastGain;
    HI_U32 u32LightGain;
    ADPT_SCENEAUTO_EXPOSUREINFO_S stAdptExposureInfo;
    ADPT_SCENEAUTO_DRCATTR_S stAdptDrcAttr;
    ADPT_SCENEAUTO_WDRATTR_S stAdptWdrAttr;
    ADPT_SCENEAUTO_STATEINFO_S stAdptStatInfo;
    ADPT_SCENEAUTO_DEFOG_ATTR_S stAdptDefogAttr;
    ADPT_SCENEAUTO_DCIPARAM_S stAdptDciParam;
    ADPT_SCENEAUTO_VENC_ATTR_S stAdptVencAttr;
    HI_S32 s32IsoLevel;
    SCENEAUTO_INIPARAM_3DNRCFG_S stSceneauto3dnr;

    SRDK_SCENEAUTO_SEPCIAL_SCENE_E eSpecialScene;
    eSpecialScene = g_eSpecialScene;

    prctl(PR_SET_NAME, (unsigned long)"SceneautoNormalThread", 0, 0, 0);
    s32IspDev = g_stINIPara.stMpInfo.s32IspDev;
    s32VencChn = g_stINIPara.stMpInfo.s32VencChn;
    s32VpssGrp = g_stINIPara.stMpInfo.s32VpssGrp;
    s32ViDev = g_stINIPara.stMpInfo.s32ViDev;
    s32Ret = CommSceneautoGetDCIParam(s32ViDev, &stAdptDciParam);
    if (HI_SUCCESS != s32Ret)
    {
        printf("CommSceneautoGetDCIParam failed\n");
    }
    bDciEnable = stAdptDciParam.bEnable;
    u32BlackGain = stAdptDciParam.u32BlackGain;
    u32ContrastGain = stAdptDciParam.u32ContrastGain;
    u32LightGain = stAdptDciParam.u32LightGain;

    
    
    while (HI_TRUE == g_bNormalThreadFlag)
    {
        //get the exposure value
        s32Ret = CommSceneautoGetExposureInfo(s32IspDev, &stAdptExposureInfo);
        if (HI_SUCCESS != s32Ret)
        {
            printf("CommSceneautoGetExposureInfo failed\n");
        }
        u64Temp = ((HI_U64)stAdptExposureInfo.u32AGain * (HI_U64)stAdptExposureInfo.u32DGain * (HI_U64)stAdptExposureInfo.u32ISPDGain) >> 30;
        u32Exposure = (HI_U32)u64Temp * stAdptExposureInfo.u32ExpTime;


        s32Ret = CommSceneautoGetDRCAttr(s32IspDev, &stAdptDrcAttr);
        if (HI_SUCCESS)
        {
            printf("CommSceneautoGetDRCAttr failed\n");
        }
        s32Ret = CommSceneautoGetWDRAttr(s32IspDev, &stAdptWdrAttr);
        if (HI_SUCCESS)
        {
            printf("CommSceneautoGetWDRAttr failed\n");
        }

        //exposure value if different in WDR
        if (stAdptWdrAttr.u8WdrMode != 0)
        {
            s32Ret = CommSceneautoGetQueryInnerStateInfo(s32IspDev, &stAdptStatInfo);
            if (HI_SUCCESS)
            {
                printf("CommSceneautoGetQueryInnerStateInfo failed\n");
            }

            if (stAdptStatInfo.u32DRCStrengthActual < g_stINIPara.stThreshValue.u32DRCStrengthThresh)
            {
                stAdptStatInfo.u32DRCStrengthActual = g_stINIPara.stThreshValue.u32DRCStrengthThresh;
            }

            u64Temp = ((HI_U64)stAdptExposureInfo.u32AGain * (HI_U64)stAdptExposureInfo.u32DGain * (HI_U64)stAdptExposureInfo.u32ISPDGain * 100) >> 30;
            u64Temp = (u64Temp * stAdptStatInfo.u32DRCStrengthActual) / g_stINIPara.stThreshValue.u32DRCStrengthThresh;
            u32Exposure = (HI_U32)u64Temp;
            //u32Exposure = (u32Exposure * stAdptStatInfo.u32DRCStrengthActual) / g_stINIPara.stThreshValue.u32DRCStrengthThresh;

            u32Iso = u32Exposure;
        }
        else
        {
            if (stAdptDrcAttr.bEnable == HI_TRUE)
            {
                s32Ret = CommSceneautoGetQueryInnerStateInfo(s32IspDev, &stAdptStatInfo);
                if (HI_SUCCESS)
                {
                    printf("CommSceneautoGetQueryInnerStateInfo failed\n");
                }

                if (stAdptStatInfo.u32DRCStrengthActual < g_stINIPara.stThreshValue.u32DRCStrengthThresh)
                {
                    stAdptStatInfo.u32DRCStrengthActual = g_stINIPara.stThreshValue.u32DRCStrengthThresh;
                }

                u32Exposure = (u32Exposure * stAdptStatInfo.u32DRCStrengthActual) / g_stINIPara.stThreshValue.u32DRCStrengthThresh;
            }

            u32Iso = 100 * (u32Exposure / stAdptExposureInfo.u32ExpTime);

        }

        //set gamma
        u32GammaExposure = u32Exposure;
        if (u32GammaExposure != u32LastGammaExposure)
        {

            if (u32GammaExposure > u32LastGammaExposure)
            {
                for (s32GammaExpLevel = 0; s32GammaExpLevel < g_stINIPara.stIniGamma.s32ExpCount; s32GammaExpLevel++)
                {
                    if (u32GammaExposure <= g_stINIPara.stIniGamma.pu32ExpThreshLtoD[s32GammaExpLevel])
                    {
                        break;
                    }
                }
                if (s32GammaExpLevel >= g_stINIPara.stIniGamma.s32ExpCount)
                {
                    s32GammaExpLevel = g_stINIPara.stIniGamma.s32ExpCount - 1;
                }
            }
            else if (u32GammaExposure < u32LastGammaExposure)
            {
                for (s32GammaExpLevel = (g_stINIPara.stIniGamma.s32ExpCount - 1); s32GammaExpLevel >= 0; s32GammaExpLevel--)
                {
                    if (u32GammaExposure > g_stINIPara.stIniGamma.pu32ExpThreshDtoL[s32GammaExpLevel])
                    {
                        break;
                    }
                }
                if (s32GammaExpLevel <= 0)
                {
                    s32GammaExpLevel = 0;
                }
            }
            if (s32GammaExpLevel != s32LastGammaExpLevel)
            {
                u32GammaCount++;
                if (u32GammaCount > g_stINIPara.stIniGamma.u32DelayCount)
                {
                    s32Ret = SceneAuto_SetGamma(s32IspDev, s32GammaExpLevel, s32LastGammaExpLevel);
                    if (HI_SUCCESS != s32Ret)
                    {
                        printf("SceneAuto_SetGamma failed\n");
                    }
                    u32GammaCount = 0;
                    s32LastGammaExpLevel = s32GammaExpLevel;
                    u32LastGammaExposure = u32GammaExposure;
                }
            }
            else
            {
                u32GammaCount = 0;
            }
        }
        if ((u32Exposure != u32LastExposure) ||
            (eSpecialScene != g_eSpecialScene))
        {
            //setting 3DNR param
            if (SRDK_SCENEAUTO_SPECIAL_SCENE_HLC == g_eSpecialScene)
            {
                for (s32IsoLevel = 0; s32IsoLevel < g_stINIPara.stHLC.s323DnrIsoCount; s32IsoLevel++)
                {
                    if (u32Iso <= g_stINIPara.stHLC.pu323DnrIsoThresh[s32IsoLevel])
                    {
                        break;
                    }
                }
                if (s32IsoLevel == g_stINIPara.stHLC.s323DnrIsoCount)
                {
                    s32IsoLevel = g_stINIPara.stHLC.s323DnrIsoCount - 1;
                }

                Interpolate(&stSceneauto3dnr, u32Iso, 
                        &g_stINIPara.stHLC.pst3dnrParam[s32IsoLevel - 1], g_stINIPara.stHLC.pu323DnrIsoThresh[s32IsoLevel - 1],
                        &g_stINIPara.stHLC.pst3dnrParam[s32IsoLevel], g_stINIPara.stHLC.pu323DnrIsoThresh[s32IsoLevel]);
                
            }
            else if (SRDK_SCENEAUTO_SPECIAL_SCENE_IR == g_eSpecialScene)
            {
                for (s32IsoLevel = 0; s32IsoLevel < g_stINIPara.stIR.s323DnrIsoCount; s32IsoLevel++)
                {
                    if (u32Iso <= g_stINIPara.stIR.pu323DnrIsoThresh[s32IsoLevel])
                    {
                        break;
                    }
                }
                if (s32IsoLevel == g_stINIPara.stIR.s323DnrIsoCount)
                {
                    s32IsoLevel = g_stINIPara.stIR.s323DnrIsoCount - 1;
                }

                Interpolate(&stSceneauto3dnr, u32Iso, 
                        &g_stINIPara.stIR.pst3dnrParam[s32IsoLevel - 1], g_stINIPara.stIR.pu323DnrIsoThresh[s32IsoLevel - 1],
                        &g_stINIPara.stIR.pst3dnrParam[s32IsoLevel], g_stINIPara.stIR.pu323DnrIsoThresh[s32IsoLevel]);  
                        
                
            }
            else
            {
                for (s32IsoLevel = 0; s32IsoLevel < g_stINIPara.stIni3dnr.s323DnrIsoCount; s32IsoLevel++)
                {
                    if (u32Iso <= g_stINIPara.stIni3dnr.pu323DnrIsoThresh[s32IsoLevel])
                    {
                        break;
                    }
                }
                if (s32IsoLevel == g_stINIPara.stIni3dnr.s323DnrIsoCount)
                {
                    s32IsoLevel = g_stINIPara.stIni3dnr.s323DnrIsoCount - 1;
                }

                Interpolate(&stSceneauto3dnr, u32Iso, 
                        &g_stINIPara.stIni3dnr.pst3dnrParam[s32IsoLevel - 1], g_stINIPara.stIni3dnr.pu323DnrIsoThresh[s32IsoLevel - 1],
                        &g_stINIPara.stIni3dnr.pst3dnrParam[s32IsoLevel], g_stINIPara.stIni3dnr.pu323DnrIsoThresh[s32IsoLevel]);
                
            }
            
            s32Ret = Sceneauto_Set3DNR(s32VpssGrp, stSceneauto3dnr);
            if (HI_SUCCESS != s32Ret)
            {
                printf("Sceneauto_SetNormal3DNR failed\n");
            }

            
            //setting AE param
            if (SRDK_SCENEAUTO_SPECIAL_SCENE_IR == g_eSpecialScene)
            {
                if (u32Exposure > u32LastExposure)
                {
                    for (i = s32IRAECurPos; i < g_stINIPara.stIR.s32ExpCount; i++)
                    {
                        if (u32Exposure > g_stINIPara.stIR.pu32ExpThreshLtoH[i])
                        {
                            s32IRAECurPos = i + 1;
                        }
                        else
                        {
                            break;
                        }
                    }
                    if (s32IRAECurPos >= g_stINIPara.stIR.s32ExpCount)
                    {
                        s32IRAECurPos = g_stINIPara.stIR.s32ExpCount - 1;
                    }
                }
                else if (u32Exposure < u32LastExposure)
                {
                    for (i = s32IRAECurPos; i >= 0; i--)
                    {
                        if (u32Exposure < g_stINIPara.stIR.pu32ExpThreshHtoL[i])
                        {
                            s32IRAECurPos = i;
                        }
                        else
                        {
                            break;
                        }
                    }
                }
                s32Ret = Scenauto_SetIRAERealtedExp(s32IspDev, s32IRAECurPos);
                if (HI_SUCCESS != s32Ret)
                {
                    printf("Scenauto_SetAERealtedExp failed\n");
                }
            }
            else
            {
                if (u32Exposure > u32LastExposure)
                {
                    for (i = s32AECurPos; i < g_stINIPara.stIniAE.s32ExpCount; i++)
                    {
                        if (u32Exposure > g_stINIPara.stIniAE.pu32AEExpLtoDThresh[i])
                        {
                            s32AECurPos = i + 1;
                        }
                        else
                        {
                            break;
                        }
                    }

                    if (s32AECurPos >= g_stINIPara.stIniAE.s32ExpCount)
                    {
                        s32AECurPos = g_stINIPara.stIniAE.s32ExpCount - 1;
                    }
                }
                else if (u32Exposure < u32LastExposure)
                {
                    for (i = s32AECurPos; i >= 0; i--)
                    {
                        if (u32Exposure < g_stINIPara.stIniAE.pu32AEExpDtoLThresh[i])
                        {
                            s32AECurPos = i;
                        }
                        else
                        {
                            break;
                        }
                    }
                }
                s32Ret = Scenauto_SetAERealtedExp(s32IspDev, s32AECurPos);
                if (HI_SUCCESS != s32Ret)
                {
                    printf("Scenauto_SetAERealtedExp failed\n");
                }
            }


            //setting DP
            for (s32DpExpLevel = 0; s32DpExpLevel < g_stINIPara.stIniDP.s32ExpCount; s32DpExpLevel++)
            {
                if (u32Exposure <= g_stINIPara.stIniDP.pu32ExpThresh[s32DpExpLevel])
                {
                    break;
                }
            }
            if (s32DpExpLevel == g_stINIPara.stIniDP.s32ExpCount)
            {
                s32DpExpLevel = g_stINIPara.stIniDP.s32ExpCount - 1;
            }

            if (s32DpExpLevel != s32LastDpExpLevel)
            {
                s32LastDpExpLevel = s32DpExpLevel;
                s32Ret = SceneAuto_SetDP(s32IspDev, s32DpExpLevel);
                if (HI_SUCCESS != s32Ret)
                {
                    printf("SceneAuto_SetDP failed\n");
                }
            }

            //setting nrprofile
            if ((u32Exposure >= g_stINIPara.stIniNRProfile.u32ExpLow) && (u32Exposure <= g_stINIPara.stIniNRProfile.u32ExpHigh))
            {
                for (i = 0; i < 128; i = i + 4)
                {
                    u32NRProfileVaule = ((HI_U32)g_stINIPara.stIniNRProfile.au8Np1[i])
                                        + (((HI_U32)g_stINIPara.stIniNRProfile.au8Np1[i + 1]) << 8)
                                        + (((HI_U32)g_stINIPara.stIniNRProfile.au8Np1[i + 2]) << 16)
                                        + (((HI_U32)g_stINIPara.stIniNRProfile.au8Np1[i + 3]) << 24);

                    u32NRProfileAddr = 0x205A0900 + i;

                    s32Ret = CommSceneautoSetRegister(s32IspDev, u32NRProfileAddr, u32NRProfileVaule);
                    if (HI_SUCCESS != s32Ret)
                    {
                        printf("CommSceneautoSetRegister failed\n");
                    }

                    u32NRProfileVaule = ((HI_U32)g_stINIPara.stIniNRProfile.au8Np2[i])
                                        + (((HI_U32)g_stINIPara.stIniNRProfile.au8Np2[i + 1]) << 8)
                                        + (((HI_U32)g_stINIPara.stIniNRProfile.au8Np2[i + 2]) << 16)
                                        + (((HI_U32)g_stINIPara.stIniNRProfile.au8Np2[i + 3]) << 24);

                    u32NRProfileAddr = 0x205A0C00 + i;

                    s32Ret = CommSceneautoSetRegister(s32IspDev, u32NRProfileAddr, u32NRProfileVaule);
                    if (HI_SUCCESS != s32Ret)
                    {
                        printf("CommSceneautoSetRegister failed\n");
                    }

                }
            }
            else
            {
                for (i = 0; i < 128; i = i + 4)
                {
                    u32NRProfileVaule = ((HI_U32)g_stINIPara.stIniNRProfile.au8NpDefault1[i])
                                        + (((HI_U32)g_stINIPara.stIniNRProfile.au8NpDefault1[i + 1]) << 8)
                                        + (((HI_U32)g_stINIPara.stIniNRProfile.au8NpDefault1[i + 2]) << 16)
                                        + (((HI_U32)g_stINIPara.stIniNRProfile.au8NpDefault1[i + 3]) << 24);

                    u32NRProfileAddr = 0x205A0900 + i;

                    s32Ret = CommSceneautoSetRegister(s32IspDev, u32NRProfileAddr, u32NRProfileVaule);
                    if (HI_SUCCESS != s32Ret)
                    {
                        printf("CommSceneautoSetRegister failed\n");
                    }

                    u32NRProfileVaule = ((HI_U32)g_stINIPara.stIniNRProfile.au8NpDefault2[i])
                                        + (((HI_U32)g_stINIPara.stIniNRProfile.au8NpDefault2[i + 1]) << 8)
                                        + (((HI_U32)g_stINIPara.stIniNRProfile.au8NpDefault2[i + 2]) << 16)
                                        + (((HI_U32)g_stINIPara.stIniNRProfile.au8NpDefault2[i + 3]) << 24);

                    u32NRProfileAddr = 0x205A0C00 + i;

                    s32Ret = CommSceneautoSetRegister(s32IspDev, u32NRProfileAddr, u32NRProfileVaule);
                    if (HI_SUCCESS != s32Ret)
                    {
                        printf("CommSceneautoSetRegister failed\n");
                    }

                }
            }

        }

        //get the bitrate value
        s32Ret = CommSceneautoGetVencAttr(s32VencChn, &stAdptVencAttr);
        if (HI_SUCCESS != s32Ret)
        {
            printf("HI_SceneAuto_GetBitrate failed\n");
        }
        u32Bitrate = stAdptVencAttr.u32BitRate;

        if (u32Bitrate != u32LastBitrate)
        {
            s32Ret = CommSceneautoGetVencAttr(s32VencChn, &stAdptVencAttr);
            if (HI_SUCCESS != s32Ret)
            {
                printf("HI_SceneAuto_GetBitrate failed\n");
            }
            switch (stAdptVencAttr.eRcMode)
            {
                case ADPT_SCENEAUTO_RCMODE_H264:
                    for (s32VencBitrateLevel = 0; s32VencBitrateLevel < g_stINIPara.stIniH264Venc.s32BitrateCount; s32VencBitrateLevel++)
                    {
                        if (u32Bitrate <= g_stINIPara.stIniH264Venc.pu32BitrateThresh[s32VencBitrateLevel])
                        {
                            break;
                        }
                    }
                    if (s32VencBitrateLevel == g_stINIPara.stIniH264Venc.s32BitrateCount)
                    {
                        s32VencBitrateLevel = g_stINIPara.stIniH264Venc.s32BitrateCount - 1;
                    }

                    if (s32VencBitrateLevel != s32LastVencBitrateLevel)
                    {
                        s32LastVencBitrateLevel = s32VencBitrateLevel;

                        s32Ret = SceneAuto_SetH264Trans(s32VencChn, s32VencBitrateLevel);
                        if (HI_SUCCESS != s32Ret)
                        {
                            printf("SceneAuto_SetH264Trans failed\n");
                        }

                        s32Ret = SceneAuto_SetH264Debolck(s32VencChn, s32VencBitrateLevel);
                        if (HI_SUCCESS != s32Ret)
                        {
                            printf("SceneAuto_SetH264Debolck failed\n");
                        }

                        s32Ret = SceneAuto_SetH264RcParam(s32VencChn, s32VencBitrateLevel);
                        if (HI_SUCCESS != s32Ret)
                        {
                            printf("SceneAuto_SetH264RcParam failed\n");
                        }
                    }
                    break;
                case ADPT_SCENEAUTO_RCMODE_H265:
                    for (s32VencBitrateLevel = 0; s32VencBitrateLevel < g_stINIPara.stIniH265Venc.stIniH265VencRcParam.s32BitrateCount; s32VencBitrateLevel++)
                    {
                        if (u32Bitrate <= g_stINIPara.stIniH265Venc.stIniH265VencRcParam.pu32BitrateThresh[s32VencBitrateLevel])
                        {
                            break;
                        }
                    }
                    if (s32VencBitrateLevel == g_stINIPara.stIniH265Venc.stIniH265VencRcParam.s32BitrateCount)
                    {
                        s32VencBitrateLevel = g_stINIPara.stIniH265Venc.stIniH265VencRcParam.s32BitrateCount - 1;
                    }
                    if (s32VencBitrateLevel != s32LastVencBitrateLevel)
                    {
                        s32LastVencBitrateLevel = s32VencBitrateLevel;
                        s32Ret = SceneAuto_SetH265RcParam(s32VencChn, s32VencBitrateLevel);
                        if (HI_SUCCESS != s32Ret)
                        {
                            printf("SceneAuto_SetH265RcParam failed\n");
                        }
                    }
                    break;
                default:
                    break;
            }

            for (s32SharpenBitrateLevel = 0; s32SharpenBitrateLevel < g_stINIPara.stIniSharpen.s32BitrateCount; s32SharpenBitrateLevel++)
            {
                if (u32Bitrate <= g_stINIPara.stIniSharpen.pu32BitrateThresh[s32SharpenBitrateLevel])
                {
                    break;
                }

            }
            if (s32SharpenBitrateLevel == g_stINIPara.stIniSharpen.s32BitrateCount)
            {
                s32SharpenBitrateLevel = g_stINIPara.stIniSharpen.s32BitrateCount - 1;
            }

            if (s32SharpenBitrateLevel != s32LastSharpenBitrateLevel)
            {
                s32LastSharpenBitrateLevel = s32SharpenBitrateLevel;

                s32Ret = SceneAuto_SetSharpen(s32IspDev, s32SharpenBitrateLevel);
                if (HI_SUCCESS != s32Ret)
                {
                    printf("SceneAuto_SetSharpen failed\n");
                }
            }

            for (s32AEBitrateLevel = 0; s32AEBitrateLevel < g_stINIPara.stIniAE.s32BitrateCount; s32AEBitrateLevel++)
            {
                if (u32Bitrate <= g_stINIPara.stIniAE.pu32BitrateThresh[s32AEBitrateLevel])
                {
                    break;
                }

            }
            if (s32AEBitrateLevel == g_stINIPara.stIniAE.s32BitrateCount)
            {
                s32AEBitrateLevel = g_stINIPara.stIniAE.s32BitrateCount - 1;
            }

            if (s32AEBitrateLevel != s32LastAEBitrateLevel)
            {
                s32LastAEBitrateLevel = s32AEBitrateLevel;

                s32Ret = SceneAuto_SetAERealtedBit(s32IspDev, s32AEBitrateLevel);
                if (HI_SUCCESS != s32Ret)
                {
                    printf("SceneAuto_SetAERealtedBit failed\n");
                }
            }
        }

        if ((u32Bitrate != u32LastBitrate) || (u32Exposure != u32LastExposure))
        {
            for (s32DemosaicBitrateLevel = 0; s32DemosaicBitrateLevel < g_stINIPara.stIniDemosaic.s32BitrateCount; s32DemosaicBitrateLevel++)
            {
                if (u32Bitrate <= g_stINIPara.stIniDemosaic.pu32BitrateThresh[s32DemosaicBitrateLevel])
                {
                    break;
                }

            }
            if (s32DemosaicBitrateLevel == g_stINIPara.stIniDemosaic.s32BitrateCount)
            {
                s32DemosaicBitrateLevel = g_stINIPara.stIniDemosaic.s32BitrateCount - 1;
            }

            for (s32DemosaicExpLevel = 0; s32DemosaicExpLevel < g_stINIPara.stIniDemosaic.s32ExpCount; s32DemosaicExpLevel++)
            {
                if (u32Exposure <= g_stINIPara.stIniDemosaic.pu32ExpThresh[s32DemosaicExpLevel])
                {
                    break;
                }
            }
            if (s32DemosaicExpLevel == g_stINIPara.stIniDemosaic.s32ExpCount)
            {
                s32DemosaicExpLevel = g_stINIPara.stIniDemosaic.s32ExpCount - 1;
            }

            if ((s32DemosaicBitrateLevel != s32LastDemosaicBitrateLevel) || (s32DemosaicExpLevel != s32LastDemosaicExpLevel))
            {
                s32LastDemosaicBitrateLevel = s32DemosaicBitrateLevel;
                s32LastDemosaicExpLevel = s32DemosaicExpLevel;

                s32Ret = SceneAuto_SetDemosaic(s32IspDev, s32DemosaicExpLevel, s32DemosaicBitrateLevel);
                if (HI_SUCCESS != s32Ret)
                {
                    printf("SceneAuto_SetDemosaic failed\n");
                }
            }
            s32Ret = CommSceneautoGetVencAttr(s32VencChn, &stAdptVencAttr);
            if (HI_SUCCESS != s32Ret)
            {
                printf("HI_SceneAuto_GetBitrate failed\n");
            }
            if (ADPT_SCENEAUTO_RCMODE_H265 == stAdptVencAttr.eRcMode)
            {
                for (s32H265FaceCfgBitrateLevel = 0; s32H265FaceCfgBitrateLevel < g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.s32BitrateCount; s32H265FaceCfgBitrateLevel++)
                {
                    if (u32Bitrate <= g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.pu32BitrateThresh[s32H265FaceCfgBitrateLevel])
                    {
                        break;
                    }

                }
                if (s32H265FaceCfgBitrateLevel == g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.s32BitrateCount)
                {
                    s32H265FaceCfgBitrateLevel = g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.s32BitrateCount - 1;
                }
                for (s32H265FaceCfgExpLevel = 0; s32H265FaceCfgExpLevel < g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.s32ExpCount; s32H265FaceCfgExpLevel++)
                {
                    if (u32Exposure <= g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.pu32ExpThresh[s32H265FaceCfgExpLevel])
                    {
                        break;
                    }
                }
                if (s32H265FaceCfgExpLevel == g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.s32ExpCount)
                {
                    s32H265FaceCfgExpLevel = g_stINIPara.stIniH265Venc.stIniH265VencFaceCfg.s32ExpCount - 1;
                }
                if ((s32H265FaceCfgBitrateLevel != s32LastH265FaceCfgBitrateLevel) || (s32H265FaceCfgExpLevel != s32LastH265FaceCfgExpLevel))
                {
                    s32LastH265FaceCfgBitrateLevel = s32H265FaceCfgBitrateLevel;
                    s32LastH265FaceCfgExpLevel = s32H265FaceCfgExpLevel;
                    s32Ret = SceneAuto_SetH265FaceCfg(s32VencChn, s32H265FaceCfgExpLevel, s32H265FaceCfgBitrateLevel);
                    if (HI_SUCCESS != s32Ret)
                    {
                        printf("SceneAuto_SetH265FaceCfg failed\n");
                    }
                }
            }
        }



        u32LastExposure = u32Exposure;
        u32LastBitrate = u32Bitrate;
        usleep(1000000);

        //DCI param when defog is on
        s32Ret = CommSceneautoGetDefogAttr(s32IspDev, &stAdptDefogAttr);
        if (HI_SUCCESS != s32Ret)
        {
            printf("CommSceneautoGetDefogAttr failed\n");
        }
        if (stAdptDefogAttr.bEnable == HI_TRUE)
        {
            s32Ret = CommSceneautoGetQueryInnerStateInfo(s32IspDev, &stAdptStatInfo);
            if (HI_SUCCESS != s32Ret)
            {
                printf("CommSceneautoGetQueryInnerStateInfo failed\n");
            }

            if ((stAdptStatInfo.u32DefogStrengthActual <= 128) || (stAdptStatInfo.u32DefogStrengthActual >= 240))
            {
                u8DciStrength = 32;
            }
            else
            {
                u8DciStrength = g_stINIPara.au8DciStrengthLut[stAdptStatInfo.u32DefogStrengthActual - 129];
            }

            s32Ret = CommSceneautoGetDCIParam(s32ViDev, &stAdptDciParam);
            if (HI_SUCCESS != s32Ret)
            {
                printf("CommSceneautoGetDCIParam failed\n");
            }
            if (HI_FALSE == bDefogState)
            {
                bDciEnable = stAdptDciParam.bEnable;
                u32BlackGain = stAdptDciParam.u32BlackGain;
                u32ContrastGain = stAdptDciParam.u32ContrastGain;
                u32LightGain = stAdptDciParam.u32LightGain;
                bDefogState = HI_TRUE;
            }
            stAdptDciParam.bEnable = HI_TRUE;
            stAdptDciParam.u32BlackGain = u8DciStrength;
            stAdptDciParam.u32ContrastGain = u8DciStrength;
            stAdptDciParam.u32LightGain = u8DciStrength;
            s32Ret = CommSceneautoSetDCIParam(s32ViDev, &stAdptDciParam);
            if (HI_SUCCESS != s32Ret)
            {
                printf("CommSceneautoSetDCIParam failed\n");
            }
        }
        else
        {
            if (HI_TRUE == bDefogState)
            {
                s32Ret = CommSceneautoGetDCIParam(s32ViDev, &stAdptDciParam);
                if (HI_SUCCESS != s32Ret)
                {
                    printf("CommSceneautoGetDCIParam failed\n");
                }
                stAdptDciParam.bEnable = bDciEnable;
                stAdptDciParam.u32BlackGain = u32BlackGain;
                stAdptDciParam.u32ContrastGain = u32ContrastGain;
                stAdptDciParam.u32LightGain = u32LightGain;
                s32Ret = CommSceneautoSetDCIParam(s32ViDev, &stAdptDciParam);
                if (HI_SUCCESS != s32Ret)
                {
                    printf("CommSceneautoSetDCIParam failed\n");
                }
                bDefogState = HI_FALSE;
            }
        }
    }

    return NULL;
}

void* SceneAuto_SpecialThread(void* pVoid)
{
    HI_S32 s32Ret = HI_SUCCESS;
    HI_S32 s32IspDev;
    HI_U64 u64Temp;
    HI_U32 u32Exposure = 0;
    HI_U32 u32LastExposure = 0;
    HI_U32 u32DeltaExposure = 0;
    HI_U8 u8AveLum = 0;
    HI_U8 u8LastAveLum = 0;
    HI_U8 u8DelataAveLum = 0;
    HI_U8 u8Count = 0;
    HI_U32 u32HistSum = 0;
    HI_U32 u32HLCDeltaExp = 0;
    HI_BOOL bHLCState = HI_FALSE;
    HI_U8 u8HLCCount = 0;
    ADPT_SCENEAUTO_EXPOSUREINFO_S stAdptExposureInfo;
    ADPT_SCENEAUTO_DIS_ATTR_S stAdptDisAttr;
    stAdptDisAttr.bEnable = HI_TRUE;
    prctl(PR_SET_NAME, (unsigned long)"SceneautoSpecialThread", 0, 0, 0);

    s32IspDev = g_stINIPara.stMpInfo.s32IspDev;

    s32Ret = CommSceneautoGetExposureInfo(s32IspDev, &stAdptExposureInfo);
    if (HI_SUCCESS != s32Ret)
    {
        printf("CommSceneautoGetExposureInfo failed\n");
    }
    u64Temp = ((HI_U64)stAdptExposureInfo.u32AGain * (HI_U64)stAdptExposureInfo.u32DGain * (HI_U64)stAdptExposureInfo.u32ISPDGain) >> 30;
    u32Exposure = (HI_U32)u64Temp * stAdptExposureInfo.u32ExpTime;
    u8AveLum = stAdptExposureInfo.u8AveLum;
    u32LastExposure = u32Exposure;
    u8LastAveLum = u8AveLum;

    while (g_bSpecialThreadFlag == HI_TRUE)
    {
        s32Ret = CommSceneautoGetExposureInfo(s32IspDev, &stAdptExposureInfo);
        if (HI_SUCCESS != s32Ret)
        {
            printf("CommSceneautoGetExposureInfo failed\n");
        }
        u64Temp = ((HI_U64)stAdptExposureInfo.u32AGain * (HI_U64)stAdptExposureInfo.u32DGain * (HI_U64)stAdptExposureInfo.u32ISPDGain) >> 30;
        u32Exposure = (HI_U32)u64Temp * stAdptExposureInfo.u32ExpTime;
        u8AveLum = stAdptExposureInfo.u8AveLum;

        if (u32LastExposure == 0)
        {
            u32LastExposure = u32Exposure;
            continue;
        }

        //HLC start
        if (HI_TRUE == g_stINIPara.stThreshValue.bHLCAutoEnable)
        {
            pthread_mutex_lock(&g_stSceneautoLock);
            if (g_eSpecialScene == SRDK_SCENEAUTO_SPECIAL_SCENE_NONE)
            {
                if (u32Exposure > u32LastExposure)
                {
                    u32HLCDeltaExp = u32Exposure - u32LastExposure;
                }
                else
                {
                    u32HLCDeltaExp = u32LastExposure - u32Exposure;
                }

                s32Ret = CommSceneautoGetExposureInfo(s32IspDev, &stAdptExposureInfo);
                if (HI_SUCCESS != s32Ret)
                {
                    printf("CommSceneautoGetExposureInfo failed\n");
                }
                u32HistSum = stAdptExposureInfo.u32Hist256Value[0]
                             + stAdptExposureInfo.u32Hist256Value[1]
                             + stAdptExposureInfo.u32Hist256Value[2]
                             + stAdptExposureInfo.u32Hist256Value[3];

                if ((u32HLCDeltaExp < g_stINIPara.stThreshValue.u32HLCTolerance)
                    && (u32Exposure < g_stINIPara.stThreshValue.u32HLCExpThresh))
                {
                    if (HI_FALSE == bHLCState)
                    {
                        if (u32HistSum > g_stINIPara.stThreshValue.u32HLCOnThresh)
                        {
                            u8HLCCount++;
                            if (u8HLCCount > g_stINIPara.stThreshValue.u32HLCCount)
                            {
                                s32Ret = SceneAuto_HLCAutoOn();
                                if (HI_SUCCESS != s32Ret)
                                {
                                    printf("SceneAuto_SetHLC failed\n");
                                }
                                bHLCState = HI_TRUE;
                                u8HLCCount = 0;
                                printf("\n\n-----------------------HLC ON------------------------\n\n");
                            }
                        }
                        else
                        {
                            u8HLCCount = 0;
                        }

                    }
                    else
                    {
                        if (u32HistSum < g_stINIPara.stThreshValue.u32HLCOffThresh)
                        {
                            u8HLCCount++;
                            if (u8HLCCount > g_stINIPara.stThreshValue.u32HLCCount)
                            {
                                s32Ret = SceneAuto_HLCAutoOff();
                                if (HI_SUCCESS != s32Ret)
                                {
                                    printf("SceneAuto_SetHLC failed\n");
                                }
                                bHLCState = HI_FALSE;
                                printf("\n\n-----------------------HLC OFF------------------------\n\n");
                                u8HLCCount = 0;
                            }
                        }
                        else
                        {
                            u8HLCCount = 0;
                        }
                    }
                }
                else
                {
                    if ((HI_TRUE == bHLCState) && (u32HistSum < g_stINIPara.stThreshValue.u32HLCOffThresh))
                    {
                        s32Ret = SceneAuto_HLCAutoOff();
                        if (HI_SUCCESS != s32Ret)
                        {
                            printf("SceneAuto_HLCOff failed\n");
                        }
                        bHLCState =  HI_FALSE;
                        u8HLCCount = 0;
                        printf("\n\n-----------------------HLC OFF------------------------\n\n");
                    }
                }
            }
            pthread_mutex_unlock(&g_stSceneautoLock);
        }

        if ((u32LastExposure != u32Exposure) || (u8LastAveLum != u8AveLum))
        {
            if (u32Exposure > u32LastExposure)
            {
                u32DeltaExposure = (u32Exposure * 256 / u32LastExposure) - 256;
            }
            else
            {
                u32DeltaExposure = (u32LastExposure  * 256 / u32Exposure) - 256;
            }

            if (u8AveLum >= u8LastAveLum)
            {
                u8DelataAveLum = u8AveLum - u8LastAveLum;
            }
            else
            {
                u8DelataAveLum = u8LastAveLum - u8AveLum;
            }

            if (g_stPreviousPara.stDis.bEnable == HI_TRUE)
            {


                if ((u32DeltaExposure > g_stINIPara.stThreshValue.u32DeltaDisExpThreash) || (u8DelataAveLum > g_stINIPara.stThreshValue.u32AveLumThresh))
                {

                    if (stAdptDisAttr.bEnable == HI_TRUE)
                    {
                        printf("\n\n----------------DIS DISABLE--------------------\n\n");
                        stAdptDisAttr.bEnable = HI_FALSE;
                        s32Ret = CommSceneautoSetDISAttr(s32IspDev, &stAdptDisAttr);
                        if (HI_SUCCESS != s32Ret)
                        {
                            printf("CommSceneautoSetDISAttr failed\n");
                        }
                        sleep(1);
                    }
                    u8Count = 0;
                }
                else
                {
                    if (stAdptDisAttr.bEnable == HI_FALSE)
                    {
                        u8Count++;
                        if (u8Count > 7)
                        {
                            printf("\n\n----------------DIS ENABLE--------------------\n\n");
                            stAdptDisAttr.bEnable = HI_TRUE;
                            s32Ret = CommSceneautoSetDISAttr(s32IspDev, &stAdptDisAttr);
                            if (HI_SUCCESS != s32Ret)
                            {
                                printf("CommSceneautoSetDISAttr failed\n");
                            }
                            u8Count = 0;
                        }
                    }

                }
            }

#if 0
            if (u32Exposure > g_stINIPara.stThreshValue.u32FpnExpThresh)
            {
                stAdptFpnAttr.bEnable = HI_FALSE;
                s32Ret = CommSceneautoSetFPNAttr(s32IspDev, &stAdptFpnAttr);
                if (HI_SUCCESS != s32Ret)
                {
                    printf("CommSceneautoSetFPNAttr failed\n");
                }
            }
            else
            {
                stAdptFpnAttr.bEnable = HI_TRUE;
                s32Ret = CommSceneautoSetFPNAttr(s32IspDev, &stAdptFpnAttr);
                if (HI_SUCCESS != s32Ret)
                {
                    printf("CommSceneautoSetFPNAttr failed\n");
                }
            }
#endif

            u8LastAveLum = u8AveLum;
            u32LastExposure = u32Exposure;
        }

        usleep(40000);
    }

    return NULL;
}

HI_S32 HI_SRDK_SCENEAUTO_GetSpecialMode(SRDK_SCENEAUTO_SEPCIAL_SCENE_E* peSpecialScene)
{
    pthread_mutex_lock(&g_stSceneautoLock);
    *peSpecialScene = g_eSpecialScene;
    pthread_mutex_unlock(&g_stSceneautoLock);
    return HI_SUCCESS;
}

HI_S32 HI_SRDK_SCENEAUTO_SetSpecialMode(const SRDK_SCENEAUTO_SEPCIAL_SCENE_E* peSpecialScene)
{
    HI_S32 s32Ret = HI_SUCCESS;
    CHECK_SCENEAUTO_INIT();
    CHECK_NULL_PTR(peSpecialScene);

    printf("*peSpecialScene = %d\n", *peSpecialScene);

    if ((*peSpecialScene != SRDK_SCENEAUTO_SPECIAL_SCENE_BLC) &&
        (*peSpecialScene != SRDK_SCENEAUTO_SPECIAL_SCENE_IR) &&
        (*peSpecialScene != SRDK_SCENEAUTO_SPECIAL_SCENE_HLC) &&
        (*peSpecialScene != SRDK_SCENEAUTO_SPECIAL_SCENE_DYNAMIC) &&
        (*peSpecialScene != SRDK_SCENEAUTO_SPECIAL_SCENE_NONE) &&
        (*peSpecialScene != SRDK_SCENEAUTO_SPECIAL_SCENE_DRC))
    {
        printf("error input! unknow special scene!");
        return HI_FAILURE;
    }

    pthread_mutex_lock(&g_stSceneautoLock);
    SRDK_SCENEAUTO_SEPCIAL_SCENE_E eSpecialScene = *peSpecialScene;

    if (g_eSpecialScene != eSpecialScene)
    {
        if ((HI_TRUE != g_bSceneautoStart) && (SRDK_SCENEAUTO_SPECIAL_SCENE_NONE == g_eSpecialScene))
        {
            s32Ret = Sceneauto_GetPreviousPara();
            if (HI_SUCCESS != s32Ret)
            {
                printf("Sceneauto_GetPreviousPara failed\n");
                pthread_mutex_unlock(&g_stSceneautoLock);
                return HI_FAILURE;
            }
        }
        else
        {
            s32Ret = SceneAuto_SetPreviousPara();
            if (HI_SUCCESS != s32Ret)
            {
                pthread_mutex_unlock(&g_stSceneautoLock);
                printf("SceneAuto_SetPreviousPara failed\n");
                return HI_FAILURE;
            }
        }

        switch (eSpecialScene)
        {
            case SRDK_SCENEAUTO_SPECIAL_SCENE_BLC:
                s32Ret = SceneAuto_SetBLC();
                if (HI_SUCCESS != s32Ret)
                {
                    printf("SceneAuto_SetBLC failed\n");
                    pthread_mutex_unlock(&g_stSceneautoLock);
                    return HI_FAILURE;
                }

                break;
            case SRDK_SCENEAUTO_SPECIAL_SCENE_IR:
                s32Ret = SceneAuto_SetIR();
                if (HI_SUCCESS != s32Ret)
                {
                    printf("SceneAuto_SetBLC failed\n");
                    pthread_mutex_unlock(&g_stSceneautoLock);
                    return HI_FAILURE;
                }
                break;
            case SRDK_SCENEAUTO_SPECIAL_SCENE_HLC:
                s32Ret = SceneAuto_SetHLC();
                if (HI_SUCCESS != s32Ret)
                {
                    printf("SceneAuto_SetHLC failed\n");
                    pthread_mutex_unlock(&g_stSceneautoLock);
                    return HI_FAILURE;
                }
                break;
            case SRDK_SCENEAUTO_SPECIAL_SCENE_DYNAMIC:
                s32Ret = SceneAuto_SetDynamic();
                if (HI_SUCCESS != s32Ret)
                {
                    printf("SceneAuto_SetDynamic failed\n");
                    pthread_mutex_unlock(&g_stSceneautoLock);
                    return HI_FAILURE;
                }
                break;
            case SRDK_SCENEAUTO_SPECIAL_SCENE_DRC:
                s32Ret = SceneAuto_SetDRC();
                if (HI_SUCCESS != s32Ret)
                {
                    printf("SceneAuto_SetDRC failed\n");
                    pthread_mutex_unlock(&g_stSceneautoLock);
                    return HI_FAILURE;
                }
                break;
            case SRDK_SCENEAUTO_SPECIAL_SCENE_NONE:
                s32Ret = SceneAuto_SetPreviousPara();
                if (HI_SUCCESS != s32Ret)
                {
                    pthread_mutex_unlock(&g_stSceneautoLock);
                    printf("SceneAuto_SetPreviousPara failed\n");
                    return HI_FAILURE;
                }
                break;
            default:
                printf("unkonw choice\n");
                break;
        }

        g_eSpecialScene = eSpecialScene;
    }
    pthread_mutex_unlock(&g_stSceneautoLock);
    return HI_SUCCESS;
}

HI_S32 HI_SRDK_SCENEAUTO_Stop()
{
    HI_S32 s32Ret = HI_SUCCESS;
    pthread_mutex_lock(&g_stSceneautoLock);
    if (HI_FALSE == g_bSceneautoStart)
    {
        printf("SRDK SCENEAUTO Module has been stopped already!\n");
        pthread_mutex_unlock(&g_stSceneautoLock);
        return HI_SUCCESS;
    }

    g_bNormalThreadFlag = HI_FALSE;

    g_bSpecialThreadFlag = HI_FALSE;
    (void)pthread_join(g_pthSceneAutoNormal, NULL);
    (void)pthread_join(g_pthSceneAutoSpecial, NULL);

    if (HI_TRUE == g_stINIPara.stThreshValue.bIVEEnable)
    {
        s32Ret = Sceneauto_IVEStop();
        if (HI_SUCCESS != s32Ret)
        {
            pthread_mutex_unlock(&g_stSceneautoLock);
            printf("SceneAuto_SetPreviousPara failed\n");
            return HI_FAILURE;
        }
    }
    s32Ret = SceneAuto_SetPreviousPara();
    if (HI_SUCCESS != s32Ret)
    {
        pthread_mutex_unlock(&g_stSceneautoLock);
        printf("SceneAuto_SetPreviousPara failed\n");
        return HI_FAILURE;
    }

    g_bSceneautoStart = HI_FALSE;
    pthread_mutex_unlock(&g_stSceneautoLock);

    printf("SRDK SCENEAUTO Module has been stopped successfully!\n");

    return HI_SUCCESS;
}

HI_S32 HI_SRDK_SCENEAUTO_Start()
{
    HI_S32 s32Ret = HI_SUCCESS;
    HI_S32 s32ViDev;
    HI_S32 s32IspDev;
    ADPT_SCENEAUTO_DCIPARAM_S stAdptDCIParam;
    ADPT_SCENEAUTO_AEATTR_S stAdptAEAttr;
    CHECK_SCENEAUTO_INIT();

    pthread_mutex_lock(&g_stSceneautoLock);
    if (HI_TRUE == g_bSceneautoStart)
    {
        printf("SRDK SCENEAUTO Module is start already!\n");
        pthread_mutex_unlock(&g_stSceneautoLock);
        return HI_SUCCESS;
    }
    s32ViDev = g_stINIPara.stMpInfo.s32ViDev;
    s32IspDev = g_stINIPara.stMpInfo.s32IspDev;
    s32Ret = CommSceneautoGetDCIParam(s32ViDev, &stAdptDCIParam);
    if (HI_SUCCESS != s32Ret)
    {
        printf("CommSceneautoGetDCIParam failed\n");
        pthread_mutex_unlock(&g_stSceneautoLock);
        return HI_FAILURE;
    }
    stAdptDCIParam.bEnable = HI_TRUE;
    stAdptDCIParam.u32BlackGain = 32;
    stAdptDCIParam.u32ContrastGain = 32;
    stAdptDCIParam.u32LightGain = 32;
    s32Ret = CommSceneautoSetDCIParam(s32ViDev, &stAdptDCIParam);
    if (HI_SUCCESS != s32Ret)
    {
        printf("CommSceneautoSetDCIParam failed\n");
        pthread_mutex_unlock(&g_stSceneautoLock);
        return HI_FAILURE;
    }
    if (SRDK_SCENEAUTO_SPECIAL_SCENE_NONE == g_eSpecialScene)
    {
        s32Ret = Sceneauto_GetPreviousPara();
        if (HI_SUCCESS != s32Ret)
        {
            pthread_mutex_unlock(&g_stSceneautoLock);
            printf("Sceneauto_GetPreviousPara failed\n");
            return HI_FAILURE;
        }
    }
    s32Ret = SceneAuto_SetAERoute();
    if (HI_SUCCESS != s32Ret)
    {
        printf("SceneAuto_SetAERoute failed\n");
        pthread_mutex_unlock(&g_stSceneautoLock);
        return HI_FAILURE;
    }

    s32Ret = SceneAuto_SetDefaultGamma();
    if (HI_SUCCESS != s32Ret)
    {
        printf("SceneAuto_SetDefaultGamma failed\n");
        pthread_mutex_unlock(&g_stSceneautoLock);
        return HI_FAILURE;
    }

    s32Ret = CommSceneautoGetAEAttr(s32IspDev, &stAdptAEAttr);
    if (HI_SUCCESS != s32Ret)
    {
        printf("CommSceneautoGetAEAttr failed\n");
        pthread_mutex_unlock(&g_stSceneautoLock);
        return HI_FAILURE;
    }
    stAdptAEAttr.u8AERunInterval = g_stINIPara.stIniAE.u8AERunInterval;
    s32Ret = CommSceneautoSetAEAttr(s32IspDev, &stAdptAEAttr);
    if (HI_SUCCESS != s32Ret)
    {
        printf("CommSceneautoSetAEAttr failed\n");
        pthread_mutex_unlock(&g_stSceneautoLock);
        return HI_FAILURE;
    }
    g_bNormalThreadFlag = HI_TRUE;
    g_bSpecialThreadFlag = HI_TRUE;


    s32Ret = pthread_create(&g_pthSceneAutoNormal, NULL, SceneAuto_NormalThread, NULL);
    if (HI_SUCCESS != s32Ret)
    {
        printf("pthread_create SceneAuto_NormalThread failed \n");
        pthread_mutex_unlock(&g_stSceneautoLock);
        return HI_FAILURE;
    }

    s32Ret = pthread_create(&g_pthSceneAutoSpecial, NULL, SceneAuto_SpecialThread, NULL);
    if (HI_SUCCESS != s32Ret)
    {
        printf("pthread_create SceneAuto_SpecialThread failed \n");
        pthread_mutex_unlock(&g_stSceneautoLock);
        return HI_FAILURE;
    }

    if (HI_TRUE == g_stINIPara.stThreshValue.bIVEEnable)
    {
        s32Ret = Sceneauto_IVEStart();
        if (HI_SUCCESS != s32Ret)
        {
            printf("Sceneauto_IVEStart failed\n");
            pthread_mutex_unlock(&g_stSceneautoLock);
            return HI_FAILURE;
        }

    }
    g_bSceneautoStart = HI_TRUE;
    pthread_mutex_unlock(&g_stSceneautoLock);

    printf("SRDK SCENEAUTO Module has been started successfully!\n");

    return HI_SUCCESS;
}

HI_S32 HI_SRDK_SCENEAUTO_DeInit()
{

    pthread_mutex_lock(&g_stSceneautoLock);
    if (HI_FALSE == g_bSceneautoInit)
    {
        printf("SRDK SCENEAUTO Module has not been inited !\n");
        pthread_mutex_unlock(&g_stSceneautoLock);
        return HI_SUCCESS;
    }

    if (HI_TRUE == g_bNormalThreadFlag)
    {
        g_bNormalThreadFlag = HI_FALSE;
        (void)pthread_join(g_pthSceneAutoNormal, NULL);
    }
    if (HI_TRUE == g_bSpecialThreadFlag)
    {
        g_bSpecialThreadFlag = HI_FALSE;
        (void)pthread_join(g_pthSceneAutoSpecial, NULL);
    }
    Sceneauto_FreeDict();
    Sceneauto_FreeMem();
    g_bSceneautoInit = HI_FALSE;
    pthread_mutex_unlock(&g_stSceneautoLock);

    printf("SRDK SCENEAUTO Module has been deinited successfully!\n");

    return HI_SUCCESS;
}

HI_S32 HI_SRDK_SCENEAUTO_Init(const HI_CHAR* pszFileName)
{
    HI_S32 s32Ret = HI_SUCCESS;

    CHECK_NULL_PTR(pszFileName);

    pthread_mutex_lock(&g_stSceneautoLock);
    if (HI_TRUE == g_bSceneautoInit)
    {
        printf("SRDK SCENEAUTO Module has been inited already\n");
        pthread_mutex_unlock(&g_stSceneautoLock);
        return HI_SUCCESS;
    }

    s32Ret = Sceneauto_LoadFile(pszFileName);
    if (HI_SUCCESS != s32Ret)
    {
        printf("Sceneauto_LoadFile failed\n");
        Sceneauto_FreeDict();
        pthread_mutex_unlock(&g_stSceneautoLock);
        return HI_FAILURE;
    }

    s32Ret = Sceneauto_LoadINIPara();
    if (HI_SUCCESS != s32Ret)
    {
        printf("Sceneauto_LoadCommonPara failed\n");
        Sceneauto_FreeDict();
        Sceneauto_FreeMem();
        pthread_mutex_unlock(&g_stSceneautoLock);
        return HI_FAILURE;
    }

    g_bSceneautoInit = HI_TRUE;
    g_eSpecialScene = SRDK_SCENEAUTO_SPECIAL_SCENE_NONE;
    g_eVencRcMode = ADPT_SCENEAUTO_RCMODE_BUTT;
    pthread_mutex_unlock(&g_stSceneautoLock);

    printf("SRDK SCENUAUTO Module has been inited successfully!\n");

    return HI_SUCCESS;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */
