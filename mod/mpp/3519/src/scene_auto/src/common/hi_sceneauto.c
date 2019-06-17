#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/prctl.h>
#include <unistd.h>

//#include "iniparser.h"
#include "hi_sceneauto_comm.h"
#include "hi_sceneauto_define_ext.h"
#include "hi_sceneauto_ext.h"
#include "hi_common.h"
#include "hi_comm_isp.h"
#include "hi_comm_vpss.h"
#include "hi_comm_venc.h"
#include "mpi_vi.h"
#include "mpi_ae.h"
#include "mpi_awb.h"
#include "mpi_isp.h"
#include "mpi_vpss.h"
#include "mpi_sys.h"
#include "hi_math.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

HI_SCENEAUTO_PARAM_S g_stSceneautoParam;
HI_SCENEAUTO_STATE_S g_stSceneautoState;
SCENEAUTO_SEPCIAL_SCENE_E g_eSpecialScene;
SCENEAUTO_FSWDR_MODE_E g_eFSWDRMode;

pthread_mutex_t g_stSceneautoLock = PTHREAD_MUTEX_INITIALIZER;



#ifdef _MSC_VER
#define MUTEX_INIT_LOCK(mutex) InitializeCriticalSection(&mutex)
#define MUTEX_LOCK(mutex) EnterCriticalSection(&mutex)
#define MUTEX_UNLOCK(mutex) LeaveCriticalSection(&mutex)
#define MUTEX_DESTROY(mutex) DeleteCriticalSection(&mutex)
#else
#define MUTEX_INIT_LOCK(mutex) \
do \
{ \
(void)pthread_mutex_init(&mutex, NULL); \
}while(0)
#define MUTEX_LOCK(mutex) \
do \
{ \
(void)pthread_mutex_lock(&mutex); \
}while(0)
#define MUTEX_UNLOCK(mutex) \
do \
{ \
(void)pthread_mutex_unlock(&mutex); \
}while(0)
#define MUTEX_DESTROY(mutex) \
do \
{ \
(void)pthread_mutex_destroy(&mutex); \
}while(0)
#endif




#define CHECK_SCENEAUTO_INIT()\
    do{\
        pthread_mutex_lock(&g_stSceneautoLock);\
        if (HI_FALSE == g_stSceneautoState.bSceneautoInit)\
        {\
            printf("func:%s,line:%d, please init sceneauto first!\n",__FUNCTION__,__LINE__);\
            pthread_mutex_unlock(&g_stSceneautoLock);\
            return HI_FAILURE;\
        }\
        pthread_mutex_unlock(&g_stSceneautoLock);\
    }while(0);

#define CHEKC_SCENEAUTO_START()\
    do{\
        pthread_mutex_lock(&g_stSceneautoLock);\
        if (HI_FALSE == g_stSceneautoState.bSceneautoStart)\
        {\
            printf("func:%s,line:%d, please start sceneauto first!\n",__FUNCTION__,__LINE__);\
            pthread_mutex_unlock(&g_stSceneautoLock);\
            return HI_FAILURE;\
        }\
        pthread_mutex_unlock(&g_stSceneautoLock);\
    }while(0);

extern HI_S32 Sceneauto_SetNrS(VPSS_GRP VpssGrp, const HI_SCENEAUTO_3DNR_S *pst3Nrs);
extern HI_S32 Sceneauto_SetNrX(VPSS_GRP VpssGrp, const HI_SCENEAUTO_3DNR_X_S *pst3Nrx);

static __inline int iClip2(int x, int b) {{ if (x < 0) x = 0; };{ if (x > b) x = b; }; return x; }
static __inline int iMin2(int a, int b) {{ if (a > b) a = b; }; return a; }
static __inline int iMax2(int a, int b) {{ if (a > b) b = a; }; return b; }  

static __inline HI_U32 MapISO(HI_U32 iso)
{
  HI_U32   j,  i = (iso >= 200);
  
  if (iso < 72) return (HI_U32)iMax2(iso, -3); 
  
  i += ( (iso >= (200 << 1)) + (iso >= (400 << 1)) + (iso >= (400 << 2)) + (iso >= (400 << 3)) + (iso >= (400 << 4)) );
  i += ( (iso >= (400 << 5)) + (iso >= (400 << 6)) + (iso >= (400 << 7)) + (iso >= (400 << 8)) + (iso >= (400 << 9)) );
  i += ( (iso >= (400 << 10))+ (iso >= (400 << 11))+ (iso >= (400 << 12))+ (iso >= (400 << 13))+ (iso >= (400 << 14))) ;
  j  = ( (iso >  (112 << i)) + (iso >  (125 << i)) + (iso >  (141 << i)) + (iso >  (158 << i)) + (iso >  (178 << i)) );
  
  return (i * 6 + j + (iso >= 80) + (iso >= 90) + (iso >= 100) - 3);  
}

static __inline HI_U32 Interpulate(HI_U32 u32Mid,HI_U32 u32Left, HI_U32 u32LValue, HI_U32 u32Right, HI_U32 u32RValue)
{
    HI_U32 u32Value;
    HI_U32 k;

    if (u32Mid <= u32Left)
    {
        u32Value = u32LValue;
        return u32Value;
    }
    if (u32Mid >= u32Right)
    {
        u32Value = u32RValue;
        return u32Value;
    }
    
    k = (u32Right - u32Left); 
    u32Value = (((u32Right - u32Mid) * (HI_U64)u32LValue + (u32Mid - u32Left) * (HI_U64)u32RValue + (k >> 1))/ k);

    return u32Value;
}

static __inline HI_U32 TimeFilter(HI_U32 u32Para0,HI_U32 u32Para1, HI_U32 u32TimeCnt, HI_U32 u32Index)
{
    HI_U64 u64Temp;
    HI_U32 u32Value;

    if (u32Para0 > u32Para1)
    {
        u64Temp = (HI_U64)(u32Para0 - u32Para1) << 8;
        u64Temp = (u64Temp * (u32Index + 1)) / u32TimeCnt >> 8;
        u32Value = u32Para0 - (HI_U32)u64Temp;
    }
    else
    {
        u64Temp = (HI_U64)(u32Para1 - u32Para0) << 8;
        u64Temp = (u64Temp * (u32Index + 1)) / u32TimeCnt >> 8;
        u32Value = u32Para0 + (HI_U32)u64Temp;
    }

    return u32Value;
}

static __inline HI_U32 GetLevelLtoH(HI_U32 u32Value, HI_U32 u32Level, HI_U32 u32Count, HI_U32 *pu32Thresh)
{
    for (u32Level = 0; u32Level < u32Count; u32Level++)
    {
        if (u32Value <= pu32Thresh[u32Level])
        {
            break;
        }
    }
    if (u32Level == u32Count)
    {
        u32Level = u32Count - 1;
    }

    return u32Level;
}

static __inline HI_U32 GetLevelHtoL(HI_U32 u32Value, HI_U32 u32Level, HI_U32 u32Count, HI_U32 *pu32Thresh)
{
    for (u32Level = u32Count; u32Level > 0; u32Level--)
    {
        if (u32Value > pu32Thresh[u32Level - 1])
        {
            break;
        }

    }
    if (u32Level > 0)
    {
        u32Level = u32Level - 1;
    }

    return u32Level;    
}

HI_S32 Sceneauto_SetVpssParam(VPSS_GRP VpssGrp, HI_SCENEAUTO_3DNRPARAM_S st3dnrParam)
{
	HI_S32 s32Ret = HI_SUCCESS;
	HI_S32 i;	
	VPSS_GRP_NRS_PARAM_S stVpssGrpNrS;

    //s32Ret = HI_MPI_VPSS_GetGrpNRSParam(VpssGrp, &stVpssGrpNrS);
    //if (HI_SUCCESS != s32Ret)
    //   {
    //       printf("HI_MPI_VPSS_GetGrpNRSParam failed\n");
    //       return HI_FAILURE;
    //   }
    stVpssGrpNrS.enNRVer = VPSS_NR_V4;
    stVpssGrpNrS.stNRSParam_V4.enOptType = VPSS_NRS_AUTO;
    stVpssGrpNrS.stNRSParam_V4.stVideoNrAuto.u32ParamNum = st3dnrParam.u323DnrIsoCount;
    stVpssGrpNrS.stNRSParam_V4.stVideoNrAuto.pau32ISO = (HI_U32*)malloc((st3dnrParam.u323DnrIsoCount) * sizeof(HI_U32));
    CHECK_NULL_PTR(stVpssGrpNrS.stNRSParam_V4.stVideoNrAuto.pau32ISO);
    stVpssGrpNrS.stNRSParam_V4.stVideoNrAuto.pastNRSParam = (NRS_PARAM_V3_S *)malloc((st3dnrParam.u323DnrIsoCount) * sizeof(NRS_PARAM_V3_S));
    CHECK_NULL_PTR(stVpssGrpNrS.stNRSParam_V4.stVideoNrAuto.pastNRSParam);
     
	for (i = 0; i < st3dnrParam.u323DnrIsoCount; i++)
	{
        stVpssGrpNrS.stNRSParam_V4.stVideoNrAuto.pau32ISO[i] = st3dnrParam.pu323DnrIsoThresh[i];
        
		stVpssGrpNrS.stNRSParam_V4.stVideoNrAuto.pastNRSParam[i].sf0_bright = st3dnrParam.pst3dnrParam[i].sf0_bright;
		stVpssGrpNrS.stNRSParam_V4.stVideoNrAuto.pastNRSParam[i].sf0_dark = st3dnrParam.pst3dnrParam[i].sf0_dark;
		
		stVpssGrpNrS.stNRSParam_V4.stVideoNrAuto.pastNRSParam[i].sf1_bright = st3dnrParam.pst3dnrParam[i].sf1_bright;
		stVpssGrpNrS.stNRSParam_V4.stVideoNrAuto.pastNRSParam[i].sf1_dark = st3dnrParam.pst3dnrParam[i].sf1_dark;
		stVpssGrpNrS.stNRSParam_V4.stVideoNrAuto.pastNRSParam[i].tf1_md_thresh = st3dnrParam.pst3dnrParam[i].tf1_md_thresh;

		stVpssGrpNrS.stNRSParam_V4.stVideoNrAuto.pastNRSParam[i].sf2_bright = st3dnrParam.pst3dnrParam[i].sf2_bright;
		stVpssGrpNrS.stNRSParam_V4.stVideoNrAuto.pastNRSParam[i].sf2_dark = st3dnrParam.pst3dnrParam[i].sf2_dark;
		stVpssGrpNrS.stNRSParam_V4.stVideoNrAuto.pastNRSParam[i].tf2_md_thresh = st3dnrParam.pst3dnrParam[i].tf2_md_thresh;
		
		stVpssGrpNrS.stNRSParam_V4.stVideoNrAuto.pastNRSParam[i].sfk_bright = st3dnrParam.pst3dnrParam[i].sfk_bright;
		stVpssGrpNrS.stNRSParam_V4.stVideoNrAuto.pastNRSParam[i].sfk_dark = st3dnrParam.pst3dnrParam[i].sfk_dark;
		stVpssGrpNrS.stNRSParam_V4.stVideoNrAuto.pastNRSParam[i].sf_ed_thresh = st3dnrParam.pst3dnrParam[i].sf_ed_thresh;

		stVpssGrpNrS.stNRSParam_V4.stVideoNrAuto.pastNRSParam[i].sfk_profile = st3dnrParam.pst3dnrParam[i].sfk_profile;
		stVpssGrpNrS.stNRSParam_V4.stVideoNrAuto.pastNRSParam[i]._reserved_b_ = st3dnrParam.pst3dnrParam[i]._reserved_b_;
		stVpssGrpNrS.stNRSParam_V4.stVideoNrAuto.pastNRSParam[i].sfk_ratio = st3dnrParam.pst3dnrParam[i].sfk_ratio;
		stVpssGrpNrS.stNRSParam_V4.stVideoNrAuto.pastNRSParam[i].sf_bsc_freq = st3dnrParam.pst3dnrParam[i].sf_bsc_freq;

    	stVpssGrpNrS.stNRSParam_V4.stVideoNrAuto.pastNRSParam[i].tf1_md_type = st3dnrParam.pst3dnrParam[i].tf1_md_type;
		stVpssGrpNrS.stNRSParam_V4.stVideoNrAuto.pastNRSParam[i].tf1_still = st3dnrParam.pst3dnrParam[i].tf1_still;
		stVpssGrpNrS.stNRSParam_V4.stVideoNrAuto.pastNRSParam[i].tf2_md_type = st3dnrParam.pst3dnrParam[i].tf2_md_type;
		stVpssGrpNrS.stNRSParam_V4.stVideoNrAuto.pastNRSParam[i].tf2_still = st3dnrParam.pst3dnrParam[i].tf2_still;

		stVpssGrpNrS.stNRSParam_V4.stVideoNrAuto.pastNRSParam[i].tf1_md_profile = st3dnrParam.pst3dnrParam[i].tf1_md_profile;
		stVpssGrpNrS.stNRSParam_V4.stVideoNrAuto.pastNRSParam[i].tf2_md_profile = st3dnrParam.pst3dnrParam[i].tf2_md_profile;
		stVpssGrpNrS.stNRSParam_V4.stVideoNrAuto.pastNRSParam[i].tf_profile = st3dnrParam.pst3dnrParam[i].tf_profile;

		stVpssGrpNrS.stNRSParam_V4.stVideoNrAuto.pastNRSParam[i].csf_strength = st3dnrParam.pst3dnrParam[i].csf_strength;
		stVpssGrpNrS.stNRSParam_V4.stVideoNrAuto.pastNRSParam[i].ctf_range = st3dnrParam.pst3dnrParam[i].ctf_range;
		stVpssGrpNrS.stNRSParam_V4.stVideoNrAuto.pastNRSParam[i].ctf_strength = st3dnrParam.pst3dnrParam[i].ctf_strength;
		stVpssGrpNrS.stNRSParam_V4.stVideoNrAuto.pastNRSParam[i].tf1_moving = st3dnrParam.pst3dnrParam[i].tf1_moving;
		stVpssGrpNrS.stNRSParam_V4.stVideoNrAuto.pastNRSParam[i].tf2_moving = st3dnrParam.pst3dnrParam[i].tf2_moving;
	}

	s32Ret = HI_MPI_VPSS_SetGrpNRSParam(VpssGrp, &stVpssGrpNrS);
	if (HI_SUCCESS != s32Ret)
    {
        printf("HI_MPI_VPSS_SetGrpNRSParam failed\n");
        FREE_PTR(stVpssGrpNrS.stNRSParam_V4.stVideoNrAuto.pastNRSParam);
        FREE_PTR(stVpssGrpNrS.stNRSParam_V4.stVideoNrAuto.pau32ISO);
        return HI_FAILURE;
    }

    FREE_PTR(stVpssGrpNrS.stNRSParam_V4.stVideoNrAuto.pastNRSParam);
    FREE_PTR(stVpssGrpNrS.stNRSParam_V4.stVideoNrAuto.pau32ISO);
    
	return HI_SUCCESS;
}

HI_S32 Sceneauto_SetTrafficIspParam(ISP_DEV IspDev)
{
	HI_S32 s32Ret = HI_SUCCESS;
	HI_S32 i;
	ISP_DRC_ATTR_S stIspDrcAttr;
	ISP_WDR_EXPOSURE_ATTR_S stIspWdrExposureAttr;
	ISP_GAMMA_ATTR_S stIspGammaAttr;
	ISP_YUV_SHARPEN_ATTR_S stIspYuvSharpenAttr;
	
	/*****set DRC param*****/
    s32Ret = HI_MPI_ISP_GetDRCAttr(IspDev, &stIspDrcAttr);    
    if (HI_SUCCESS != s32Ret)
    {
        printf("HI_MPI_ISP_GetDRCAttr failed\n");
        return HI_FAILURE;
    }
    stIspDrcAttr.bEnable = g_stSceneautoParam.stTrafficParam.bDRCEnable;
    if (HI_TRUE == g_stSceneautoParam.stTrafficParam.bDRCManulEnable)
    {
        stIspDrcAttr.enOpType = OP_TYPE_MANUAL;
        stIspDrcAttr.stManual.u32Strength = g_stSceneautoParam.stTrafficParam.u32DRCStrengthTarget;
    }
    else
    {
        stIspDrcAttr.enOpType = OP_TYPE_AUTO;
        stIspDrcAttr.stAuto.u32Strength = g_stSceneautoParam.stTrafficParam.u32DRCAutoStrength;
    }
    stIspDrcAttr.u32WhiteLevel = g_stSceneautoParam.stTrafficParam.u32WhiteLevel;
    stIspDrcAttr.u32BlackLevel = g_stSceneautoParam.stTrafficParam.u32BlackLevel;
    stIspDrcAttr.u32Asymmetry = g_stSceneautoParam.stTrafficParam.u32Asymmetry;
    stIspDrcAttr.u32BrightEnhance = g_stSceneautoParam.stTrafficParam.u32BrightEnhance;
    stIspDrcAttr.FilterMux = (ISP_DRC_CURVE_TYPE_E)g_stSceneautoParam.stTrafficParam.u8FilterMux;
    stIspDrcAttr.stDrcCurve.u32SlopeMax = g_stSceneautoParam.stTrafficParam.u32SlopeMax;
    stIspDrcAttr.stDrcCurve.u32SlopeMin = g_stSceneautoParam.stTrafficParam.u32SlopeMin;
    stIspDrcAttr.stDrcCurve.u32VarianceIntensity = g_stSceneautoParam.stTrafficParam.u32VarianceIntensity;
    stIspDrcAttr.stDrcCurve.u32VarianceSpace = g_stSceneautoParam.stTrafficParam.u32VarianceSpace;
    stIspDrcAttr.stDrcCurveEx.u32BrightPr = g_stSceneautoParam.stTrafficParam.u32BrightPr;
    stIspDrcAttr.stDrcCurveEx.u32Contrast = g_stSceneautoParam.stTrafficParam.u32Contrast;
    stIspDrcAttr.stDrcCurveEx.u32DarkEnhance = g_stSceneautoParam.stTrafficParam.u32DarkEnhance;
    stIspDrcAttr.stDrcCurveEx.u32Svariance = g_stSceneautoParam.stTrafficParam.u32Svariance;    
    s32Ret = HI_MPI_ISP_SetDRCAttr(IspDev, &stIspDrcAttr);    
    if (HI_SUCCESS != s32Ret)
    {
        printf("HI_MPI_ISP_SetDRCAttr failed\n");
        return HI_FAILURE;
    }

    /*****set WDR Exposure param*****/
    s32Ret = HI_MPI_ISP_GetWDRExposureAttr(IspDev, &stIspWdrExposureAttr);
    if (HI_SUCCESS != s32Ret)
    {
        printf("HI_MPI_ISP_GetWDRExposureAttr failed\n");
        return HI_FAILURE;
    }
    stIspWdrExposureAttr.enExpRatioType = (ISP_OP_TYPE_E)g_stSceneautoParam.stTrafficParam.u8ExpRatioType;
    for (i = 0; i < 3; i++)
    {
        stIspWdrExposureAttr.au32ExpRatio[i] = g_stSceneautoParam.stTrafficParam.au32ExpRatio[i];
    }
    stIspWdrExposureAttr.u32ExpRatioMax = g_stSceneautoParam.stTrafficParam.u32ExpRatioMax;
    stIspWdrExposureAttr.u32ExpRatioMin = g_stSceneautoParam.stTrafficParam.u32ExpRatioMin;
    s32Ret = HI_MPI_ISP_SetWDRExposureAttr(IspDev, &stIspWdrExposureAttr);
    if (HI_SUCCESS != s32Ret)
    {
        printf("HI_MPI_ISP_SetWDRExposureAttr failed\n");
        return HI_FAILURE;
    }

    /*****set Gamma param*****/
    s32Ret = HI_MPI_ISP_GetGammaAttr(IspDev, &stIspGammaAttr);
    if (HI_SUCCESS != s32Ret)
    {
        printf("HI_MPI_ISP_GetGammaAttr failed\n");
        return HI_FAILURE;
    }
    stIspGammaAttr.enCurveType = ISP_GAMMA_CURVE_USER_DEFINE;
    for (i = 0; i < 257 ; i++)
    {
        stIspGammaAttr.u16Table[i] = g_stSceneautoParam.stTrafficParam.u16GammaTable[i];
    }
    s32Ret = HI_MPI_ISP_SetGammaAttr(IspDev, &stIspGammaAttr);
    if (HI_SUCCESS != s32Ret)
    {
        printf("HI_MPI_ISP_SetGammaAttr failed\n");
        return HI_FAILURE;
    }

    /*****set YuvSharpen param*****/
    s32Ret = HI_MPI_ISP_GetYuvSharpenAttr(IspDev, &stIspYuvSharpenAttr);    
    if (HI_SUCCESS != s32Ret)
    {
        printf("HI_MPI_ISP_GetYuvSharpenAttr failed\n");
        return HI_FAILURE;
    }
    for (i = 0; i < EXPOSURE_LEVEL; i++)
    {
        stIspYuvSharpenAttr.stAuto.au8TextureSt[i] = g_stSceneautoParam.stTrafficParam.au8TextureSt[i];
        stIspYuvSharpenAttr.stAuto.au8EdgeSt[i] = g_stSceneautoParam.stTrafficParam.au8EdgeSt[i];
        stIspYuvSharpenAttr.stAuto.au8OverShoot[i] = g_stSceneautoParam.stTrafficParam.au8OverShoot[i];
        stIspYuvSharpenAttr.stAuto.au8UnderShoot[i] = g_stSceneautoParam.stTrafficParam.au8UnderShoot[i];
        stIspYuvSharpenAttr.stAuto.au8TextureThd[i] = g_stSceneautoParam.stTrafficParam.au8TextureThd[i];
        stIspYuvSharpenAttr.stAuto.au8EdgeThd[i] = g_stSceneautoParam.stTrafficParam.au8EdgeThd[i];
        stIspYuvSharpenAttr.stAuto.au8DetailCtrl[i] = g_stSceneautoParam.stTrafficParam.au8DetailCtrl[i];
    }
    s32Ret = HI_MPI_ISP_SetYuvSharpenAttr(IspDev, &stIspYuvSharpenAttr);    
    if (HI_SUCCESS != s32Ret)
    {
        printf("HI_MPI_ISP_SetYuvSharpenAttr failed\n");
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

HI_S32 Sceneauto_SetTrafficMode()
{
	HI_S32 s32Ret = HI_SUCCESS;
	HI_S32 s32Index;
	ISP_DEV IspDev;

	for (s32Index = 0; s32Index < g_stSceneautoParam.stCommParam.s32IspDevCount; s32Index++)
	{
		IspDev = g_stSceneautoParam.pstIspParam[s32Index].IspDev;
		s32Ret = Sceneauto_SetTrafficIspParam(IspDev);
		if (HI_SUCCESS != s32Ret)
		{
			printf("Sceneauto_SetHlcIspParam failed\n");
	        return HI_FAILURE;
		}
	}
	
	return HI_SUCCESS;
}

HI_S32 Sceneauto_SetFaceIspParam(ISP_DEV IspDev)
{
	HI_S32 s32Ret = HI_SUCCESS;
	HI_S32 i;
	ISP_YUV_SHARPEN_ATTR_S stIspYuvSharpenAttr;
    ISP_EXPOSURE_ATTR_S stExpAttr;
    ISP_GAMMA_ATTR_S stIspGammaAttr;

    /*****set AE param*****/
    s32Ret = HI_MPI_ISP_GetExposureAttr(IspDev, &stExpAttr);    
    if (HI_SUCCESS != s32Ret)
    {
        printf("HI_MPI_ISP_GetExposureAttr failed\n");
        return HI_FAILURE;
    }
    stExpAttr.stAuto.stExpTimeRange.u32Max = 8000; /* 8ms */
    s32Ret = HI_MPI_ISP_SetExposureAttr(IspDev, &stExpAttr);    
    if (HI_SUCCESS != s32Ret)
    {
        printf("HI_MPI_ISP_SetExposureAttr failed\n");
        return HI_FAILURE;
    }

    /*****set Gamma param*****/
    s32Ret = HI_MPI_ISP_GetGammaAttr(IspDev, &stIspGammaAttr);
    if (HI_SUCCESS != s32Ret)
    {
        printf("HI_MPI_ISP_GetGammaAttr failed\n");
        return HI_FAILURE;
    }
    stIspGammaAttr.enCurveType = ISP_GAMMA_CURVE_USER_DEFINE;
    for (i = 0; i < 257; i++)
    {
        stIspGammaAttr.u16Table[i] = g_stSceneautoParam.stFaceParam.u16GammaTable[i];
    }
    s32Ret = HI_MPI_ISP_SetGammaAttr(IspDev, &stIspGammaAttr);
    if (HI_SUCCESS != s32Ret)
    {
        printf("HI_MPI_ISP_SetGammaAttr failed\n");
        return HI_FAILURE;
    }
	
    /*****set YuvSharpen param*****/
    s32Ret = HI_MPI_ISP_GetYuvSharpenAttr(IspDev, &stIspYuvSharpenAttr);    
    if (HI_SUCCESS != s32Ret)
    {
        printf("HI_MPI_ISP_GetYuvSharpenAttr failed\n");
        return HI_FAILURE;
    }
    for (i = 0; i < EXPOSURE_LEVEL; i++)
    {
        stIspYuvSharpenAttr.stAuto.au8TextureSt[i] = g_stSceneautoParam.stFaceParam.au8TextureSt[i];
        stIspYuvSharpenAttr.stAuto.au8EdgeSt[i] = g_stSceneautoParam.stFaceParam.au8EdgeSt[i];
        stIspYuvSharpenAttr.stAuto.au8OverShoot[i] = g_stSceneautoParam.stFaceParam.au8OverShoot[i];
        stIspYuvSharpenAttr.stAuto.au8UnderShoot[i] = g_stSceneautoParam.stFaceParam.au8UnderShoot[i];
        stIspYuvSharpenAttr.stAuto.au8TextureThd[i] = g_stSceneautoParam.stFaceParam.au8TextureThd[i];
        stIspYuvSharpenAttr.stAuto.au8EdgeThd[i] = g_stSceneautoParam.stFaceParam.au8EdgeThd[i];
        stIspYuvSharpenAttr.stAuto.au8DetailCtrl[i] = g_stSceneautoParam.stFaceParam.au8DetailCtrl[i];
    }
    s32Ret = HI_MPI_ISP_SetYuvSharpenAttr(IspDev, &stIspYuvSharpenAttr);    
    if (HI_SUCCESS != s32Ret)
    {
        printf("HI_MPI_ISP_SetYuvSharpenAttr failed\n");
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

HI_S32 Sceneauto_SetFaceMode()
{
	HI_S32 s32Ret = HI_SUCCESS;
	HI_S32 s32Index;
	ISP_DEV IspDev;

	for (s32Index = 0; s32Index < g_stSceneautoParam.stCommParam.s32IspDevCount; s32Index++)
	{
		IspDev = g_stSceneautoParam.pstIspParam[s32Index].IspDev;
		s32Ret = Sceneauto_SetFaceIspParam(IspDev);
		if (HI_SUCCESS != s32Ret)
		{
			printf("Sceneauto_SetFaceIspParam failed\n");
	        return HI_FAILURE;
		}
	}
	
	return HI_SUCCESS;
}


HI_S32 Sceneauto_SetHlcViParam(VI_DEV ViDev)
{
	HI_S32 s32Ret = HI_SUCCESS;
	VI_DCI_PARAM_S stViDciParam;
	
	/*****set DCI param*****/
    s32Ret = HI_MPI_VI_GetDCIParam(ViDev, &stViDciParam);
    if (HI_SUCCESS != s32Ret)
    {
        printf("HI_MPI_VI_GetDCIParam failed\n");
        return HI_FAILURE;
    }
    stViDciParam.bEnable = g_stSceneautoParam.stHlcParam.bDCIEnable;
    stViDciParam.u32BlackGain = g_stSceneautoParam.stHlcParam.u32DCIBlackGain;
    stViDciParam.u32ContrastGain = g_stSceneautoParam.stHlcParam.u32DCIContrastGain;
    stViDciParam.u32LightGain = g_stSceneautoParam.stHlcParam.u32DCILightGain;
    s32Ret = HI_MPI_VI_SetDCIParam(ViDev, &stViDciParam);
    if (HI_SUCCESS != s32Ret)
    {
        printf("HI_MPI_VI_SetDCIParam failed\n");
        return HI_FAILURE;
    }
    
	return HI_SUCCESS;
}

HI_S32 Sceneauto_SetHlcIspParam(ISP_DEV IspDev)
{
	HI_S32 s32Ret = HI_SUCCESS;
    HI_S32 i;
    ISP_EXPOSURE_ATTR_S stIspExposureAttr;
    ISP_DRC_ATTR_S stIspDrcAttr;
    ISP_WDR_EXPOSURE_ATTR_S stIspWdrExposureAttr;
    ISP_SATURATION_ATTR_S stIspSaturationAttr;
    ISP_GAMMA_ATTR_S stIspGammaAttr;

    /*****set AE param*****/
    s32Ret = HI_MPI_ISP_GetExposureAttr(IspDev, &stIspExposureAttr);
    if (HI_SUCCESS != s32Ret)
    {
        printf("HI_MPI_ISP_GetExposureAttr failed\n");
        return HI_FAILURE;
    }
    stIspExposureAttr.stAuto.u8Compensation = g_stSceneautoParam.stHlcParam.u8ExpCompensation;
    stIspExposureAttr.stAuto.stAEDelayAttr.u16BlackDelayFrame = g_stSceneautoParam.stHlcParam.u16BlackDelayFrame;
    stIspExposureAttr.stAuto.stAEDelayAttr.u16WhiteDelayFrame = g_stSceneautoParam.stHlcParam.u16WhiteDelayFrame;
    stIspExposureAttr.stAuto.u8Speed = g_stSceneautoParam.stHlcParam.u8Speed;
    stIspExposureAttr.stAuto.u8Tolerance = g_stSceneautoParam.stHlcParam.u8Tolerance;
    stIspExposureAttr.stAuto.u16HistRatioSlope = g_stSceneautoParam.stHlcParam.u16HistRatioSlope;
    stIspExposureAttr.stAuto.u8MaxHistOffset = g_stSceneautoParam.stHlcParam.u8MaxHistOffset;
    s32Ret = HI_MPI_ISP_SetExposureAttr(IspDev, &stIspExposureAttr);
    if (HI_SUCCESS != s32Ret)
    {
        printf("HI_MPI_ISP_SetExposureAttr failed\n");
        return HI_FAILURE;
    }

    /*****set DRC param*****/
    s32Ret = HI_MPI_ISP_GetDRCAttr(IspDev, &stIspDrcAttr);    
    if (HI_SUCCESS != s32Ret)
    {
        printf("HI_MPI_ISP_GetDRCAttr failed\n");
        return HI_FAILURE;
    }
    stIspDrcAttr.bEnable = g_stSceneautoParam.stHlcParam.bDRCEnable;
    if (HI_TRUE == g_stSceneautoParam.stHlcParam.bDRCManulEnable)
    {
        stIspDrcAttr.enOpType = OP_TYPE_MANUAL;
        stIspDrcAttr.stManual.u32Strength = g_stSceneautoParam.stHlcParam.u32DRCStrengthTarget;
    }
    else
    {
        stIspDrcAttr.enOpType = OP_TYPE_AUTO;
        stIspDrcAttr.stAuto.u32Strength = g_stSceneautoParam.stHlcParam.u32DRCAutoStrength;
    }
	stIspDrcAttr.u32WhiteLevel = g_stSceneautoParam.stHlcParam.u32WhiteLevel;
	stIspDrcAttr.u32BlackLevel = g_stSceneautoParam.stHlcParam.u32BlackLevel;
	stIspDrcAttr.u32Asymmetry = g_stSceneautoParam.stHlcParam.u32Asymmetry;
	stIspDrcAttr.u32BrightEnhance = g_stSceneautoParam.stHlcParam.u32BrightEnhance;
	stIspDrcAttr.FilterMux = (ISP_DRC_CURVE_TYPE_E)g_stSceneautoParam.stHlcParam.u8FilterMux;
	stIspDrcAttr.stDrcCurve.u32SlopeMax = g_stSceneautoParam.stHlcParam.u32SlopeMax;
	stIspDrcAttr.stDrcCurve.u32SlopeMin = g_stSceneautoParam.stHlcParam.u32SlopeMin;
	stIspDrcAttr.stDrcCurve.u32VarianceIntensity = g_stSceneautoParam.stHlcParam.u32VarianceIntensity;
	stIspDrcAttr.stDrcCurve.u32VarianceSpace = g_stSceneautoParam.stHlcParam.u32VarianceSpace;
	stIspDrcAttr.stDrcCurveEx.u32BrightPr = g_stSceneautoParam.stHlcParam.u32BrightPr;
	stIspDrcAttr.stDrcCurveEx.u32Contrast = g_stSceneautoParam.stHlcParam.u32Contrast;
	stIspDrcAttr.stDrcCurveEx.u32DarkEnhance = g_stSceneautoParam.stHlcParam.u32DarkEnhance;
	stIspDrcAttr.stDrcCurveEx.u32Svariance = g_stSceneautoParam.stHlcParam.u32Svariance;    
    s32Ret = HI_MPI_ISP_SetDRCAttr(IspDev, &stIspDrcAttr);    
    if (HI_SUCCESS != s32Ret)
    {
        printf("HI_MPI_ISP_SetDRCAttr failed\n");
        return HI_FAILURE;
    }

	/*****set WDR Exposure param*****/
    s32Ret = HI_MPI_ISP_GetWDRExposureAttr(IspDev, &stIspWdrExposureAttr);
    if (HI_SUCCESS != s32Ret)
    {
        printf("HI_MPI_ISP_GetWDRExposureAttr failed\n");
        return HI_FAILURE;
    }
    stIspWdrExposureAttr.enExpRatioType = (ISP_OP_TYPE_E)g_stSceneautoParam.stHlcParam.u8ExpRatioType;
    for (i = 0; i < 3; i++)
    {
    	stIspWdrExposureAttr.au32ExpRatio[i] = g_stSceneautoParam.stHlcParam.au32ExpRatio[i];
    }
    stIspWdrExposureAttr.u32ExpRatioMax = g_stSceneautoParam.stHlcParam.u32ExpRatioMax;
    stIspWdrExposureAttr.u32ExpRatioMin = g_stSceneautoParam.stHlcParam.u32ExpRatioMin;
	s32Ret = HI_MPI_ISP_SetWDRExposureAttr(IspDev, &stIspWdrExposureAttr);
    if (HI_SUCCESS != s32Ret)
    {
        printf("HI_MPI_ISP_SetWDRExposureAttr failed\n");
        return HI_FAILURE;
    }

    /*****set Saturation param*****/
    s32Ret = HI_MPI_ISP_GetSaturationAttr(IspDev, &stIspSaturationAttr);
    if (HI_SUCCESS != s32Ret)
    {
        printf("HI_MPI_ISP_GetSaturationAttr failed\n");
        return HI_FAILURE;
    }
    for (i = 0; i < 16; i++)
    {
        stIspSaturationAttr.stAuto.au8Sat[i] = g_stSceneautoParam.stHlcParam.u8Saturation[i];
    }
    s32Ret = HI_MPI_ISP_SetSaturationAttr(IspDev, &stIspSaturationAttr);
    if (HI_SUCCESS != s32Ret)
    {
        printf("HI_MPI_ISP_SetSaturationAttr failed\n");
        return HI_FAILURE;
    }

    /*****set Gamma param*****/
    s32Ret = HI_MPI_ISP_GetGammaAttr(IspDev, &stIspGammaAttr);
    if (HI_SUCCESS != s32Ret)
    {
        printf("HI_MPI_ISP_GetGammaAttr failed\n");
        return HI_FAILURE;
    }
    stIspGammaAttr.enCurveType = ISP_GAMMA_CURVE_USER_DEFINE;
    for (i = 0; i < 257 ; i++)
    {
        stIspGammaAttr.u16Table[i] = g_stSceneautoParam.stHlcParam.u16GammaTable[i];
    }
    s32Ret = HI_MPI_ISP_SetGammaAttr(IspDev, &stIspGammaAttr);
    if (HI_SUCCESS != s32Ret)
    {
        printf("HI_MPI_ISP_SetGammaAttr failed\n");
        return HI_FAILURE;
    }
    
	return HI_SUCCESS;
}

HI_S32 Sceneauto_SetHLC()
{
	HI_S32 s32Ret = HI_SUCCESS;
	HI_S32 s32Index;
	ISP_DEV IspDev;
	VI_DEV ViDev;

	for (s32Index = 0; s32Index < g_stSceneautoParam.stCommParam.s32IspDevCount; s32Index++)
	{
		IspDev = g_stSceneautoParam.pstIspParam[s32Index].IspDev;
		s32Ret = Sceneauto_SetHlcIspParam(IspDev);
		if (HI_SUCCESS != s32Ret)
		{
			printf("Sceneauto_SetHlcIspParam failed\n");
	        return HI_FAILURE;
		}
	}

	for (s32Index = 0; s32Index < g_stSceneautoParam.stCommParam.s32ViDevCount; s32Index++)
	{
		ViDev = g_stSceneautoParam.pstViParam[s32Index].ViDev;
		s32Ret = Sceneauto_SetHlcViParam(ViDev);
		if (HI_SUCCESS != s32Ret)
		{
			printf("Sceneauto_SetHlcViParam failed\n");
	        return HI_FAILURE;
		}
	}
	
	return HI_SUCCESS;
}

HI_S32 Sceneauto_SetIrViParam(VI_DEV ViDev)
{
	HI_S32 s32Ret = HI_SUCCESS;
	VI_DCI_PARAM_S stViDciParam;
	
	/*****set DCI param*****/
    s32Ret = HI_MPI_VI_GetDCIParam(ViDev, &stViDciParam);
    if (HI_SUCCESS != s32Ret)
    {
        printf("HI_MPI_VI_GetDCIParam failed\n");
        return HI_FAILURE;
    }
    stViDciParam.bEnable = g_stSceneautoParam.stIrParam.bDCIEnable;
    stViDciParam.u32BlackGain = g_stSceneautoParam.stIrParam.u32DCIBlackGain;
    stViDciParam.u32ContrastGain = g_stSceneautoParam.stIrParam.u32DCIContrastGain;
    stViDciParam.u32LightGain = g_stSceneautoParam.stIrParam.u32DCILightGain;
    s32Ret = HI_MPI_VI_SetDCIParam(ViDev, &stViDciParam);
    if (HI_SUCCESS != s32Ret)
    {
        printf("HI_MPI_VI_SetDCIParam failed\n");
        return HI_FAILURE;
    }
    
	return HI_SUCCESS;
}

HI_S32 Sceneauto_SetIrIspParam(ISP_DEV IspDev)
{
	HI_S32 s32Ret = HI_SUCCESS;
	HI_S32 i,j;
	ISP_EXPOSURE_ATTR_S stIspExposureAttr;
	ISP_SATURATION_ATTR_S stIspSaturationAttr;
	ISP_WB_ATTR_S stIspWbAttr;
	ISP_COLORMATRIX_ATTR_S stIspCCMAttr;
	ISP_BAYER_SHARPEN_ATTR_S stIspBayerSharpenAttr;
	ISP_YUV_SHARPEN_ATTR_S stIspYuvSharpenAttr;
	ISP_GAMMA_ATTR_S stIspGammaAttr;
	ISP_DP_ATTR_S *pstIspDpAttr = HI_NULL;
	
	/*****set AE param*****/
    s32Ret = HI_MPI_ISP_GetExposureAttr(IspDev, &stIspExposureAttr);
    if (HI_SUCCESS != s32Ret)
    {
        printf("HI_MPI_ISP_GetExposureAttr failed\n");
        return HI_FAILURE;
    }
    stIspExposureAttr.stAuto.u16HistRatioSlope = g_stSceneautoParam.stIrParam.u16HistRatioSlope;
    stIspExposureAttr.stAuto.stAEDelayAttr.u16BlackDelayFrame = g_stSceneautoParam.stIrParam.u16BlackDelayFrame;
    stIspExposureAttr.stAuto.stAEDelayAttr.u16WhiteDelayFrame = g_stSceneautoParam.stIrParam.u16WhiteDelayFrame;
    stIspExposureAttr.stAuto.u8Speed = g_stSceneautoParam.stIrParam.u8Speed;
    stIspExposureAttr.stAuto.u8Tolerance = g_stSceneautoParam.stIrParam.u8Tolerance;
    for (i = 0; i < AE_WEIGHT_ROW; i++)
    {
        for (j = 0; j < AE_WEIGHT_COLUMN; j++)
        {
            stIspExposureAttr.stAuto.au8Weight[i][j] = g_stSceneautoParam.stIrParam.au8Weight[i][j];
        }
    }
    s32Ret = HI_MPI_ISP_SetExposureAttr(IspDev, &stIspExposureAttr);
    if (HI_SUCCESS != s32Ret)
    {
        printf("HI_MPI_ISP_SetExposureAttr failed\n");
        return HI_FAILURE;
    }

    /*****set Saturation param*****/
    s32Ret = HI_MPI_ISP_GetSaturationAttr(IspDev, &stIspSaturationAttr);
    if (HI_SUCCESS != s32Ret)
    {
        printf("HI_MPI_ISP_GetSaturationAttr failed\n");
        return HI_FAILURE;
    }
    stIspSaturationAttr.enOpType = OP_TYPE_MANUAL;
    stIspSaturationAttr.stManual.u8Saturation = 0;
    s32Ret = HI_MPI_ISP_SetSaturationAttr(IspDev, &stIspSaturationAttr);
    if (HI_SUCCESS != s32Ret)
    {
        printf("HI_MPI_ISP_SetSaturationAttr failed\n");
        return HI_FAILURE;
    }

    /*****set AWB param*****/
    s32Ret = HI_MPI_ISP_GetWBAttr(IspDev, &stIspWbAttr);
    if (HI_SUCCESS != s32Ret)
    {
        printf("HI_MPI_ISP_GetWBAttr failed\n");
        return HI_FAILURE;
    }
    stIspWbAttr.enOpType = OP_TYPE_MANUAL;
    stIspWbAttr.stManual.u16Bgain  = 0x100;
    stIspWbAttr.stManual.u16Gbgain = 0x100;
    stIspWbAttr.stManual.u16Grgain = 0x100;
    stIspWbAttr.stManual.u16Rgain  = 0x100;
    s32Ret = HI_MPI_ISP_SetWBAttr(IspDev, &stIspWbAttr);
    if (HI_SUCCESS != s32Ret)
    {
        printf("HI_MPI_ISP_SetWBAttr failed\n");
        return HI_FAILURE;
    }

    /*****set CCM param*****/
	s32Ret = HI_MPI_ISP_GetCCMAttr(IspDev, &stIspCCMAttr);
	if (HI_SUCCESS != s32Ret)
    {
        printf("HI_MPI_ISP_GetCCMAttr failed\n");
        return HI_FAILURE;
    }
    stIspCCMAttr.enOpType = OP_TYPE_MANUAL;
    stIspCCMAttr.stManual.au16CCM[0] = 0x100;
    stIspCCMAttr.stManual.au16CCM[1] = 0x0;
    stIspCCMAttr.stManual.au16CCM[2] = 0x0;
    stIspCCMAttr.stManual.au16CCM[3] = 0x0;
    stIspCCMAttr.stManual.au16CCM[4] = 0x100;
    stIspCCMAttr.stManual.au16CCM[5] = 0x0;
    stIspCCMAttr.stManual.au16CCM[6] = 0x0;
    stIspCCMAttr.stManual.au16CCM[7] = 0x0;
    stIspCCMAttr.stManual.au16CCM[8] = 0x100;
    s32Ret = HI_MPI_ISP_SetCCMAttr(IspDev, &stIspCCMAttr);
	if (HI_SUCCESS != s32Ret)
    {
        printf("HI_MPI_ISP_SetCCMAttr failed\n");
        return HI_FAILURE;
    }

    /*****set BayerSharpen param*****/
    s32Ret = HI_MPI_ISP_GetBayerSharpenAttr(IspDev, &stIspBayerSharpenAttr);
    if (HI_SUCCESS != s32Ret)
    {
        printf("HI_MPI_ISP_GetBayerSharpenAttr failed\n");
        return HI_FAILURE;
    }
    for (i = 0; i < EXPOSURE_LEVEL; i++)
    {
        stIspBayerSharpenAttr.stAuto.astShp[i].u16LumaThd = g_stSceneautoParam.stIrParam.au16LumThresh[i];
        stIspBayerSharpenAttr.stAuto.astShp[i].u8SharpenD = g_stSceneautoParam.stIrParam.au8SharpenD[i];
        stIspBayerSharpenAttr.stAuto.astShp[i].u8SharpenUd = g_stSceneautoParam.stIrParam.au8SharpenUd[i];
    }
    s32Ret = HI_MPI_ISP_SetBayerSharpenAttr(IspDev, &stIspBayerSharpenAttr);
    if (HI_SUCCESS != s32Ret)
    {
        printf("HI_MPI_ISP_SetBayerSharpenAttr failed\n");
        return HI_FAILURE;
    }

    /*****set YuvSharpen param*****/
    s32Ret = HI_MPI_ISP_GetYuvSharpenAttr(IspDev, &stIspYuvSharpenAttr);    
    if (HI_SUCCESS != s32Ret)
    {
        printf("HI_MPI_ISP_GetYuvSharpenAttr failed\n");
        return HI_FAILURE;
    }
    for (i = 0; i < EXPOSURE_LEVEL; i++)
    {
        stIspYuvSharpenAttr.stAuto.au8TextureSt[i] = g_stSceneautoParam.stIrParam.au8TextureSt[i];
        stIspYuvSharpenAttr.stAuto.au8EdgeSt[i] = g_stSceneautoParam.stIrParam.au8EdgeSt[i];
        stIspYuvSharpenAttr.stAuto.au8OverShoot[i] = g_stSceneautoParam.stIrParam.au8OverShoot[i];
        stIspYuvSharpenAttr.stAuto.au8UnderShoot[i] = g_stSceneautoParam.stIrParam.au8UnderShoot[i];
    }
    s32Ret = HI_MPI_ISP_SetYuvSharpenAttr(IspDev, &stIspYuvSharpenAttr);    
    if (HI_SUCCESS != s32Ret)
    {
        printf("HI_MPI_ISP_SetYuvSharpenAttr failed\n");
        return HI_FAILURE;
    }

    /*****set Gamma param*****/
    s32Ret = HI_MPI_ISP_GetGammaAttr(IspDev, &stIspGammaAttr);
    if (HI_SUCCESS != s32Ret)
    {
        printf("HI_MPI_ISP_GetGammaAttr failed\n");
        return HI_FAILURE;
    }
    stIspGammaAttr.enCurveType = ISP_GAMMA_CURVE_USER_DEFINE;
    for (i = 0; i < 257; i++)
    {
        stIspGammaAttr.u16Table[i] = g_stSceneautoParam.stIrParam.u16GammaTable[i];
    }
    s32Ret = HI_MPI_ISP_SetGammaAttr(IspDev, &stIspGammaAttr);
    if (HI_SUCCESS != s32Ret)
    {
        printf("HI_MPI_ISP_SetGammaAttr failed\n");
        return HI_FAILURE;
    }

	/*****set DP param*****/
	pstIspDpAttr = (ISP_DP_ATTR_S *)malloc(sizeof(ISP_DP_ATTR_S));
    if(HI_NULL == pstIspDpAttr)
    {
        printf("Malloc ISP_DP_ATTR_S failed\n");
        return HI_FAILURE;
    }
    s32Ret = HI_MPI_ISP_GetDPAttr(IspDev, pstIspDpAttr);
    if (HI_SUCCESS != s32Ret)
    {
        printf("HI_MPI_ISP_GetDPAttr failed\n");
        FREE_PTR(pstIspDpAttr);
        return HI_FAILURE;
    }
    pstIspDpAttr->stDynamicAttr.u16Slope = g_stSceneautoParam.stIrParam.u16Slope;
    s32Ret = HI_MPI_ISP_SetDPAttr(IspDev, pstIspDpAttr);
    if (HI_SUCCESS != s32Ret)
    {
        printf("HI_MPI_ISP_SetDPAttr failed\n");
        FREE_PTR(pstIspDpAttr);
        return HI_FAILURE;
    }

    FREE_PTR(pstIspDpAttr);
	return HI_SUCCESS;
}

HI_S32 Sceneauto_SetIR()
{
	HI_S32 s32Ret = HI_SUCCESS;
	HI_S32 s32Index;
	ISP_DEV IspDev;
	VI_DEV ViDev;

	for (s32Index = 0; s32Index < g_stSceneautoParam.stCommParam.s32IspDevCount; s32Index++)
	{
		IspDev = g_stSceneautoParam.pstIspParam[s32Index].IspDev;
		s32Ret = Sceneauto_SetIrIspParam(IspDev);
		if (HI_SUCCESS != s32Ret)
		{
			printf("Sceneauto_SetIrIspParam failed\n");
	        return HI_FAILURE;
		}
	}

	for (s32Index = 0; s32Index < g_stSceneautoParam.stCommParam.s32ViDevCount; s32Index++)
	{
		ViDev = g_stSceneautoParam.pstViParam[s32Index].ViDev;
		s32Ret = Sceneauto_SetIrViParam(ViDev);
		if (HI_SUCCESS != s32Ret)
		{
			printf("Sceneauto_SetIrViParam failed\n");
	        return HI_FAILURE;
		}
	}
		
	return HI_SUCCESS;
}

HI_S32 Sceneauto_SetIspDrcParam(HI_S32 s32Index)
{
	static HI_U32 au32LastDrcExpLevel[MAX_ISP_COUNT] = {0};
	SCENEAUTO_SEPCIAL_SCENE_E eSpecialScene = SCENEAUTO_SPECIAL_SCENE_NONE;
	SCENEAUTO_FSWDR_MODE_E eFSWDRMode = SCENEAUTO_FSWDR_NORMAL_MODE;
	HI_S32 s32Ret = HI_SUCCESS;
	HI_S32 i;
	HI_U32 u32Interval = 30;
	HI_U32 u32ExpLevel = 0;
	HI_U32 u32LastExpLevel = 0;
    HI_U32 u32Count = g_stSceneautoParam.pstIspParam[s32Index].stDrcParam.u32DRCount;
    HI_U32 *pu32Thresh = g_stSceneautoParam.pstIspParam[s32Index].stDrcParam.pu32DRThresh;
	ISP_DEV IspDev;
	ISP_WDR_MODE_S stIspWdrMode;
	ISP_EXP_INFO_S stIspExpInfo;
	ISP_DRC_ATTR_S stIspDrcAttr;
		
	IspDev = g_stSceneautoParam.pstIspParam[s32Index].IspDev;
	u32LastExpLevel = au32LastDrcExpLevel[s32Index];

	s32Ret = HI_MPI_ISP_GetWDRMode(IspDev, &stIspWdrMode);
    if (HI_SUCCESS != s32Ret)
    {
        printf("HI_MPI_ISP_GetWDRMode failed\n");
        return HI_FAILURE;
    }
    if ((WDR_MODE_NONE == stIspWdrMode.enWDRMode)||
    	(SCENEAUTO_SPECIAL_SCENE_HLC == g_eSpecialScene) ||
        (SCENEAUTO_SPECIAL_SCENE_TRAFFIC == g_eSpecialScene))
    {
    	return HI_SUCCESS;
    }

    s32Ret = HI_MPI_ISP_QueryExposureInfo(IspDev, &stIspExpInfo);
    if (HI_SUCCESS != s32Ret)
    {
        printf("HI_MPI_ISP_QueryExposureInfo failed\n");
        return HI_FAILURE;
    }
    u32ExpLevel = GetLevelLtoH(stIspExpInfo.u8AveLum, u32ExpLevel, u32Count, pu32Thresh);

    s32Ret = HI_MPI_ISP_GetDRCAttr(IspDev, &stIspDrcAttr);
    if (HI_SUCCESS != s32Ret)
    {
        printf("HI_MPI_ISP_GetDRCAttr failed\n");
		return HI_FAILURE;
    }

    if ((u32ExpLevel != u32LastExpLevel) ||
    	(eSpecialScene != g_eSpecialScene) ||
    	(eFSWDRMode != g_eFSWDRMode))
    {
    	if(((SCENEAUTO_SPECIAL_SCENE_MANUAL_LONG_FRAME == g_eSpecialScene) && (SCENEAUTO_FSWDR_LONG_FRAME_MODE == g_eFSWDRMode)) || 
               ((SCENEAUTO_SPECIAL_SCENE_AUTO_FSWDR == g_eSpecialScene) && (SCENEAUTO_FSWDR_LONG_FRAME_MODE == g_eFSWDRMode)))
        {
            hi_usleep(200000);
            stIspDrcAttr.stAuto.u32Strength = g_stSceneautoParam.stFswdrParam.u32DRCStrength;
        }
        else if((SCENEAUTO_SPECIAL_SCENE_MANUAL_NORMAL_WDR == g_eSpecialScene) && (SCENEAUTO_FSWDR_NORMAL_MODE == g_eFSWDRMode))
        {
            hi_usleep(200000);
            stIspDrcAttr.stAuto.u32Strength = 512;
        }
        else if ((SCENEAUTO_SPECIAL_SCENE_NONE == g_eSpecialScene) ||
                ((SCENEAUTO_SPECIAL_SCENE_AUTO_FSWDR == g_eSpecialScene) && (SCENEAUTO_FSWDR_NORMAL_MODE == g_eFSWDRMode)))
        {
            stIspDrcAttr.bEnable = g_stSceneautoParam.pstIspParam[s32Index].stDrcParam.pstDRCAttr[u32ExpLevel].bEnable;
            stIspDrcAttr.enOpType = (ISP_OP_TYPE_E)g_stSceneautoParam.pstIspParam[s32Index].stDrcParam.pstDRCAttr[u32ExpLevel].u8OpType;
            stIspDrcAttr.u32BlackLevel = g_stSceneautoParam.pstIspParam[s32Index].stDrcParam.pstDRCAttr[u32ExpLevel].u32BlackLevel;
            stIspDrcAttr.u32Asymmetry = g_stSceneautoParam.pstIspParam[s32Index].stDrcParam.pstDRCAttr[u32ExpLevel].u32Asymmetry;
            stIspDrcAttr.u32BrightEnhance = g_stSceneautoParam.pstIspParam[s32Index].stDrcParam.pstDRCAttr[u32ExpLevel].u32BrightEnhance;
            stIspDrcAttr.FilterMux = (ISP_DRC_CURVE_TYPE_E)g_stSceneautoParam.pstIspParam[s32Index].stDrcParam.pstDRCAttr[u32ExpLevel].u8FilterMux;
            stIspDrcAttr.stManual.u32Strength = g_stSceneautoParam.pstIspParam[s32Index].stDrcParam.pstDRCAttr[u32ExpLevel].u32Strength;
            stIspDrcAttr.stAuto.u32Strength = g_stSceneautoParam.pstIspParam[s32Index].stDrcParam.pstDRCAttr[u32ExpLevel].u32DRCAutoStrength;

            stIspDrcAttr.stDrcCurve.u32SlopeMax = g_stSceneautoParam.pstIspParam[s32Index].stDrcParam.pstDRCAttr[u32ExpLevel].u32SlopeMax;
            stIspDrcAttr.stDrcCurve.u32SlopeMin = g_stSceneautoParam.pstIspParam[s32Index].stDrcParam.pstDRCAttr[u32ExpLevel].u32SlopeMin;
            stIspDrcAttr.stDrcCurve.u32VarianceSpace = g_stSceneautoParam.pstIspParam[s32Index].stDrcParam.pstDRCAttr[u32ExpLevel].u32VarianceSpace;
            stIspDrcAttr.stDrcCurve.u32VarianceIntensity = g_stSceneautoParam.pstIspParam[s32Index].stDrcParam.pstDRCAttr[u32ExpLevel].u32VarianceIntensity;

            for (i = 0; i < u32Interval; i++)
            {
                stIspDrcAttr.u32WhiteLevel = TimeFilter(g_stSceneautoParam.pstIspParam[s32Index].stDrcParam.pstDRCAttr[u32LastExpLevel].u32WhiteLevel, \
                    g_stSceneautoParam.pstIspParam[s32Index].stDrcParam.pstDRCAttr[u32ExpLevel].u32WhiteLevel, u32Interval, i);

                stIspDrcAttr.stDrcCurveEx.u32Contrast = TimeFilter(g_stSceneautoParam.pstIspParam[s32Index].stDrcParam.pstDRCAttr[u32LastExpLevel].u32Contrast, \
                    g_stSceneautoParam.pstIspParam[s32Index].stDrcParam.pstDRCAttr[u32ExpLevel].u32Contrast, u32Interval, i);

                stIspDrcAttr.stDrcCurveEx.u32BrightPr = TimeFilter(g_stSceneautoParam.pstIspParam[s32Index].stDrcParam.pstDRCAttr[u32LastExpLevel].u32BrightPr, \
                    g_stSceneautoParam.pstIspParam[s32Index].stDrcParam.pstDRCAttr[u32ExpLevel].u32BrightPr, u32Interval, i);

                stIspDrcAttr.stDrcCurveEx.u32Svariance = TimeFilter(g_stSceneautoParam.pstIspParam[s32Index].stDrcParam.pstDRCAttr[u32LastExpLevel].u32Svariance, \
                    g_stSceneautoParam.pstIspParam[s32Index].stDrcParam.pstDRCAttr[u32ExpLevel].u32Svariance, u32Interval, i);

                stIspDrcAttr.stDrcCurveEx.u32DarkEnhance = TimeFilter(g_stSceneautoParam.pstIspParam[s32Index].stDrcParam.pstDRCAttr[u32LastExpLevel].u32DarkEnhance, \
                    g_stSceneautoParam.pstIspParam[s32Index].stDrcParam.pstDRCAttr[u32ExpLevel].u32DarkEnhance, u32Interval, i);

                s32Ret = HI_MPI_ISP_SetDRCAttr(IspDev, &stIspDrcAttr);
                if (HI_SUCCESS != s32Ret)
                {
                    printf("HI_MPI_ISP_SetDPAttr failed\n");
                    return HI_FAILURE;
                }
                hi_usleep(100000);
            }            
        }

		s32Ret = HI_MPI_ISP_SetDRCAttr(IspDev, &stIspDrcAttr);
	    if (HI_SUCCESS != s32Ret)
	    {
	        printf("HI_MPI_ISP_SetDRCAttr failed\n");
	        return HI_FAILURE;
	    }
	  //  u32LastExpLevel = u32ExpLevel;
        au32LastDrcExpLevel[s32Index] = u32ExpLevel;
     // eSpecialScene = g_eSpecialScene;
      //  eFSWDRMode = g_eFSWDRMode;
    }
    
	return HI_SUCCESS;		
}

HI_S32 Sceneauto_Set3DNR(VPSS_GRP VpssGrp, HI_U32 u32Iso, const HI_SCENEAUTO_3DNRPARAM_S st3dnrparam)
{
    HI_S32 s32Ret = HI_SUCCESS;
    HI_U32 u32Mid, u32Left, u32Right;
    HI_U32 u32IsoLevel = 0;
    HI_U32 u32Count = st3dnrparam.u323DnrIsoCount;
    HI_U32 *pu32Thresh = st3dnrparam.pu323DnrIsoThresh;
    HI_SCENEAUTO_3DNR_S stNrs;
    
    u32IsoLevel = GetLevelLtoH(u32Iso, u32IsoLevel, u32Count, pu32Thresh);

    if (0 == u32IsoLevel)
    {
        stNrs = st3dnrparam.pst3dnrParam[0];            
    }
    else 
    {
        u32Mid = MapISO(u32Iso);
        u32Left = MapISO(st3dnrparam.pu323DnrIsoThresh[u32IsoLevel - 1]);
        u32Right = MapISO(st3dnrparam.pu323DnrIsoThresh[u32IsoLevel]);

         stNrs.sf0_bright = Interpulate(u32Mid, 
            u32Left, st3dnrparam.pst3dnrParam[u32IsoLevel - 1].sf0_bright,
            u32Right, st3dnrparam.pst3dnrParam[u32IsoLevel].sf0_bright);
         stNrs.sf0_dark = Interpulate(u32Mid, 
            u32Left, st3dnrparam.pst3dnrParam[u32IsoLevel - 1].sf0_dark,
            u32Right, st3dnrparam.pst3dnrParam[u32IsoLevel].sf0_dark);
        
         stNrs.sf1_bright = Interpulate(u32Mid, 
            u32Left, st3dnrparam.pst3dnrParam[u32IsoLevel - 1].sf1_bright,
            u32Right, st3dnrparam.pst3dnrParam[u32IsoLevel].sf1_bright);
         stNrs.sf1_dark = Interpulate(u32Mid, 
            u32Left, st3dnrparam.pst3dnrParam[u32IsoLevel - 1].sf1_dark,
            u32Right, st3dnrparam.pst3dnrParam[u32IsoLevel].sf1_dark);
        
         stNrs.sf2_bright = Interpulate(u32Mid, 
            u32Left, st3dnrparam.pst3dnrParam[u32IsoLevel - 1].sf2_bright,
            u32Right, st3dnrparam.pst3dnrParam[u32IsoLevel].sf2_bright);
         stNrs.sf2_dark= Interpulate(u32Mid, 
            u32Left, st3dnrparam.pst3dnrParam[u32IsoLevel - 1].sf2_dark,
            u32Right, st3dnrparam.pst3dnrParam[u32IsoLevel].sf2_dark);
        
         stNrs.sfk_bright= Interpulate(u32Mid, 
            u32Left, st3dnrparam.pst3dnrParam[u32IsoLevel - 1].sfk_bright,
            u32Right, st3dnrparam.pst3dnrParam[u32IsoLevel].sfk_bright);
         stNrs.sfk_dark= Interpulate(u32Mid, 
            u32Left, st3dnrparam.pst3dnrParam[u32IsoLevel - 1].sfk_dark,
            u32Right, st3dnrparam.pst3dnrParam[u32IsoLevel].sfk_dark);

         stNrs.sfk_ratio = Interpulate(u32Mid, 
            u32Left, st3dnrparam.pst3dnrParam[u32IsoLevel - 1].sfk_ratio,
            u32Right, st3dnrparam.pst3dnrParam[u32IsoLevel].sfk_ratio);
         stNrs.sfk_profile = Interpulate(u32Mid, 
            u32Left, st3dnrparam.pst3dnrParam[u32IsoLevel - 1].sfk_profile,
            u32Right, st3dnrparam.pst3dnrParam[u32IsoLevel].sfk_profile);
         stNrs.sf_ed_thresh = Interpulate(u32Mid, 
            u32Left, st3dnrparam.pst3dnrParam[u32IsoLevel - 1].sf_ed_thresh,
            u32Right, st3dnrparam.pst3dnrParam[u32IsoLevel].sf_ed_thresh);

         stNrs.tf1_md_thresh = Interpulate(u32Mid, 
            u32Left, st3dnrparam.pst3dnrParam[u32IsoLevel - 1].tf1_md_thresh,
            u32Right, st3dnrparam.pst3dnrParam[u32IsoLevel].tf1_md_thresh);
         stNrs.tf2_md_thresh = Interpulate(u32Mid, 
            u32Left, st3dnrparam.pst3dnrParam[u32IsoLevel - 1].tf2_md_thresh,
            u32Right, st3dnrparam.pst3dnrParam[u32IsoLevel].tf2_md_thresh);

         stNrs.tf1_md_profile = Interpulate(u32Mid, 
            u32Left, st3dnrparam.pst3dnrParam[u32IsoLevel - 1].tf1_md_profile,
            u32Right, st3dnrparam.pst3dnrParam[u32IsoLevel].tf1_md_profile);
         stNrs.tf2_md_profile = Interpulate(u32Mid, 
            u32Left, st3dnrparam.pst3dnrParam[u32IsoLevel - 1].tf2_md_profile,
            u32Right, st3dnrparam.pst3dnrParam[u32IsoLevel].tf2_md_profile);
         stNrs.tf_profile = Interpulate(u32Mid, 
            u32Left, st3dnrparam.pst3dnrParam[u32IsoLevel - 1].tf_profile,
            u32Right, st3dnrparam.pst3dnrParam[u32IsoLevel].tf_profile);

         stNrs.tf1_still = Interpulate(u32Mid, 
            u32Left, st3dnrparam.pst3dnrParam[u32IsoLevel - 1].tf1_still,
            u32Right, st3dnrparam.pst3dnrParam[u32IsoLevel].tf1_still);
         stNrs.tf2_still = Interpulate(u32Mid, 
            u32Left, st3dnrparam.pst3dnrParam[u32IsoLevel - 1].tf2_still,
            u32Right, st3dnrparam.pst3dnrParam[u32IsoLevel].tf2_still);

         stNrs.tf1_md_type = Interpulate(u32Mid, 
            u32Left, st3dnrparam.pst3dnrParam[u32IsoLevel - 1].tf1_md_type,
            u32Right, st3dnrparam.pst3dnrParam[u32IsoLevel].tf1_md_type);
         stNrs.tf2_md_type = Interpulate(u32Mid, 
            u32Left, st3dnrparam.pst3dnrParam[u32IsoLevel - 1].tf2_md_type,
            u32Right, st3dnrparam.pst3dnrParam[u32IsoLevel].tf2_md_type);

         stNrs.tf1_moving = Interpulate(u32Mid, 
            u32Left, st3dnrparam.pst3dnrParam[u32IsoLevel - 1].tf1_moving,
            u32Right, st3dnrparam.pst3dnrParam[u32IsoLevel].tf1_moving);
         stNrs.tf2_moving = Interpulate(u32Mid, 
            u32Left, st3dnrparam.pst3dnrParam[u32IsoLevel - 1].tf2_moving,
            u32Right, st3dnrparam.pst3dnrParam[u32IsoLevel].tf2_moving);
         stNrs.sf_bsc_freq = Interpulate(u32Mid,
            u32Left, st3dnrparam.pst3dnrParam[u32IsoLevel - 1].sf_bsc_freq,
            u32Right, st3dnrparam.pst3dnrParam[u32IsoLevel].sf_bsc_freq);            
         stNrs.csf_strength = Interpulate(u32Mid, 
            u32Left, st3dnrparam.pst3dnrParam[u32IsoLevel - 1].csf_strength,
            u32Right, st3dnrparam.pst3dnrParam[u32IsoLevel].csf_strength);
         stNrs.ctf_range = Interpulate(u32Mid, 
            u32Left, st3dnrparam.pst3dnrParam[u32IsoLevel - 1].ctf_range,
            u32Right, st3dnrparam.pst3dnrParam[u32IsoLevel].ctf_range);
         stNrs.ctf_strength = Interpulate(u32Mid, 
            u32Left, st3dnrparam.pst3dnrParam[u32IsoLevel - 1].ctf_strength,
            u32Right, st3dnrparam.pst3dnrParam[u32IsoLevel].ctf_strength);

    }
    
	stNrs._reserved_b_ = 0;

    s32Ret = Sceneauto_SetNrS(VpssGrp, &stNrs);
    if (HI_SUCCESS != s32Ret)
    {
        printf("SetNrb failed\n");
        return HI_FAILURE;
    }
    
    return HI_SUCCESS;
}

HI_S32 Sceneauto_Set3DNRX(VPSS_GRP VpssGrp, HI_U32 u32Iso, const HI_SCENEAUTO_3DNRPARAM_X_S st3dnrxparam)
{
    HI_S32 s32Ret = HI_SUCCESS;
    HI_U32 i = 0, j = 0;
    HI_U32 u32Mid, u32Left, u32Right;
    HI_U32 u32IsoLevel = 0;
    HI_U32 u32Count = st3dnrxparam.u323DnrIsoCount;
    HI_U32 *pu32Thresh = st3dnrxparam.pu323DnrIsoThresh;
    HI_SCENEAUTO_3DNR_X_S stNrx;
    
    u32IsoLevel = GetLevelLtoH(u32Iso, u32IsoLevel, u32Count, pu32Thresh);

    if (0 == u32IsoLevel)
    {
        stNrx = st3dnrxparam.pst3dnrxParam[0];            
    }
    else 
    {
        u32Mid = MapISO(u32Iso);
        u32Left = MapISO(st3dnrxparam.pu323DnrIsoThresh[u32IsoLevel - 1]);
        u32Right = MapISO(st3dnrxparam.pu323DnrIsoThresh[u32IsoLevel]);
        
        /* IEy */
        stNrx.IEy.IES = Interpulate(u32Mid, 
            u32Left, st3dnrxparam.pst3dnrxParam[u32IsoLevel - 1].IEy.IES,
            u32Right, st3dnrxparam.pst3dnrxParam[u32IsoLevel].IEy.IES);
        stNrx.IEy.IET = Interpulate(u32Mid, 
            u32Left, st3dnrxparam.pst3dnrxParam[u32IsoLevel - 1].IEy.IET,
            u32Right, st3dnrxparam.pst3dnrxParam[u32IsoLevel].IEy.IET);
        stNrx.IEy.IEB = Interpulate(u32Mid, 
            u32Left, st3dnrxparam.pst3dnrxParam[u32IsoLevel - 1].IEy.IEB,
            u32Right, st3dnrxparam.pst3dnrxParam[u32IsoLevel].IEy.IEB);

        /* SFy[4] */
        for (i = 0; i < 4; i++)
        {
            stNrx.SFy[i].SBF0 = Interpulate(u32Mid, 
                u32Left, st3dnrxparam.pst3dnrxParam[u32IsoLevel - 1].SFy[i].SBF0,
                u32Right, st3dnrxparam.pst3dnrxParam[u32IsoLevel].SFy[i].SBF0);
            stNrx.SFy[i].SBF1 = Interpulate(u32Mid, 
                u32Left, st3dnrxparam.pst3dnrxParam[u32IsoLevel - 1].SFy[i].SBF1,
                u32Right, st3dnrxparam.pst3dnrxParam[u32IsoLevel].SFy[i].SBF1);
            stNrx.SFy[i].SBF2 = Interpulate(u32Mid, 
                u32Left, st3dnrxparam.pst3dnrxParam[u32IsoLevel - 1].SFy[i].SBF2,
                u32Right, st3dnrxparam.pst3dnrxParam[u32IsoLevel].SFy[i].SBF2);
            stNrx.SFy[i].VRTO = Interpulate(u32Mid, 
                u32Left, st3dnrxparam.pst3dnrxParam[u32IsoLevel - 1].SFy[i].VRTO,
                u32Right, st3dnrxparam.pst3dnrxParam[u32IsoLevel].SFy[i].VRTO);
            stNrx.SFy[i].horPro = Interpulate(u32Mid, 
                u32Left, st3dnrxparam.pst3dnrxParam[u32IsoLevel - 1].SFy[i].horPro,
                u32Right, st3dnrxparam.pst3dnrxParam[u32IsoLevel].SFy[i].horPro);
            stNrx.SFy[i].verPro = Interpulate(u32Mid, 
                u32Left, st3dnrxparam.pst3dnrxParam[u32IsoLevel - 1].SFy[i].verPro,
                u32Right, st3dnrxparam.pst3dnrxParam[u32IsoLevel].SFy[i].verPro);
            stNrx.SFy[i].SFkRfw = Interpulate(u32Mid, 
                u32Left, st3dnrxparam.pst3dnrxParam[u32IsoLevel - 1].SFy[i].SFkRfw,
                u32Right, st3dnrxparam.pst3dnrxParam[u32IsoLevel].SFy[i].SFkRfw);
            stNrx.SFy[i].kProDD = Interpulate(u32Mid, 
                u32Left, st3dnrxparam.pst3dnrxParam[u32IsoLevel - 1].SFy[i].kProDD,
                u32Right, st3dnrxparam.pst3dnrxParam[u32IsoLevel].SFy[i].kProDD);
            stNrx.SFy[i].SFkType = Interpulate(u32Mid, 
                u32Left, st3dnrxparam.pst3dnrxParam[u32IsoLevel - 1].SFy[i].SFkType,
                u32Right, st3dnrxparam.pst3dnrxParam[u32IsoLevel].SFy[i].SFkType);
            stNrx.SFy[i].SSLP = Interpulate(u32Mid, 
                u32Left, st3dnrxparam.pst3dnrxParam[u32IsoLevel - 1].SFy[i].SSLP,
                u32Right, st3dnrxparam.pst3dnrxParam[u32IsoLevel].SFy[i].SSLP);
            stNrx.SFy[i].SFkBig = Interpulate(u32Mid, 
                u32Left, st3dnrxparam.pst3dnrxParam[u32IsoLevel - 1].SFy[i].SFkBig,
                u32Right, st3dnrxparam.pst3dnrxParam[u32IsoLevel].SFy[i].SFkBig);
            for (j = 0; j < 3; j++)
            {
                stNrx.SFy[i].SBS[j] = Interpulate(u32Mid, 
                    u32Left, st3dnrxparam.pst3dnrxParam[u32IsoLevel - 1].SFy[i].SBS[j],
                    u32Right, st3dnrxparam.pst3dnrxParam[u32IsoLevel].SFy[i].SBS[j]);
                stNrx.SFy[i].SDS[j] = Interpulate(u32Mid, 
                    u32Left, st3dnrxparam.pst3dnrxParam[u32IsoLevel - 1].SFy[i].SDS[j],
                    u32Right, st3dnrxparam.pst3dnrxParam[u32IsoLevel].SFy[i].SDS[j]);
                stNrx.SFy[i].STH[j] = Interpulate(u32Mid, 
                    u32Left, st3dnrxparam.pst3dnrxParam[u32IsoLevel - 1].SFy[i].STH[j],
                    u32Right, st3dnrxparam.pst3dnrxParam[u32IsoLevel].SFy[i].STH[j]);
            }
        }

        /* MDy[3] */
        for (i = 0; i < 3; i++)
        {
            stNrx.MDy[i].MATH = Interpulate(u32Mid, 
                u32Left, st3dnrxparam.pst3dnrxParam[u32IsoLevel - 1].MDy[i].MATH,
                u32Right, st3dnrxparam.pst3dnrxParam[u32IsoLevel].MDy[i].MATH);
            stNrx.MDy[i].MATE = Interpulate(u32Mid, 
                u32Left, st3dnrxparam.pst3dnrxParam[u32IsoLevel - 1].MDy[i].MATE,
                u32Right, st3dnrxparam.pst3dnrxParam[u32IsoLevel].MDy[i].MATE);
            stNrx.MDy[i].MATW = Interpulate(u32Mid, 
                u32Left, st3dnrxparam.pst3dnrxParam[u32IsoLevel - 1].MDy[i].MATW,
                u32Right, st3dnrxparam.pst3dnrxParam[u32IsoLevel].MDy[i].MATW);
            stNrx.MDy[i].MASW = Interpulate(u32Mid, 
                u32Left, st3dnrxparam.pst3dnrxParam[u32IsoLevel - 1].MDy[i].MASW,
                u32Right, st3dnrxparam.pst3dnrxParam[u32IsoLevel].MDy[i].MASW);
            stNrx.MDy[i].MABW = Interpulate(u32Mid, 
                u32Left, st3dnrxparam.pst3dnrxParam[u32IsoLevel - 1].MDy[i].MABW,
                u32Right, st3dnrxparam.pst3dnrxParam[u32IsoLevel].MDy[i].MABW);
            stNrx.MDy[i].MAXN = Interpulate(u32Mid, 
                u32Left, st3dnrxparam.pst3dnrxParam[u32IsoLevel - 1].MDy[i].MAXN,
                u32Right, st3dnrxparam.pst3dnrxParam[u32IsoLevel].MDy[i].MAXN);
        }

        /* TFy[4] */
        for (i = 0; i < 4; i++)
        {
            stNrx.TFy[i].MDDZ = Interpulate(u32Mid, 
                u32Left, st3dnrxparam.pst3dnrxParam[u32IsoLevel - 1].TFy[i].MDDZ,
                u32Right, st3dnrxparam.pst3dnrxParam[u32IsoLevel].TFy[i].MDDZ);
            stNrx.TFy[i].SDZ = Interpulate(u32Mid, 
                u32Left, st3dnrxparam.pst3dnrxParam[u32IsoLevel - 1].TFy[i].SDZ,
                u32Right, st3dnrxparam.pst3dnrxParam[u32IsoLevel].TFy[i].SDZ);
            stNrx.TFy[i].TFS = Interpulate(u32Mid, 
                u32Left, st3dnrxparam.pst3dnrxParam[u32IsoLevel - 1].TFy[i].TFS,
                u32Right, st3dnrxparam.pst3dnrxParam[u32IsoLevel].TFy[i].TFS);
            stNrx.TFy[i].TSS = Interpulate(u32Mid, 
                u32Left, st3dnrxparam.pst3dnrxParam[u32IsoLevel - 1].TFy[i].TSS,
                u32Right, st3dnrxparam.pst3dnrxParam[u32IsoLevel].TFy[i].TSS);
            stNrx.TFy[i].TFT = Interpulate(u32Mid, 
                u32Left, st3dnrxparam.pst3dnrxParam[u32IsoLevel - 1].TFy[i].TFT,
                u32Right, st3dnrxparam.pst3dnrxParam[u32IsoLevel].TFy[i].TFT);
            stNrx.TFy[i].STR = Interpulate(u32Mid, 
                u32Left, st3dnrxparam.pst3dnrxParam[u32IsoLevel - 1].TFy[i].STR,
                u32Right, st3dnrxparam.pst3dnrxParam[u32IsoLevel].TFy[i].STR);
            for (j = 0; j < 4; j++)
            {
                stNrx.TFy[i].SFR[j] = Interpulate(u32Mid, 
                    u32Left, st3dnrxparam.pst3dnrxParam[u32IsoLevel - 1].TFy[i].SFR[j],
                    u32Right, st3dnrxparam.pst3dnrxParam[u32IsoLevel].TFy[i].SFR[j]);
                stNrx.TFy[i].MTFR[j] = Interpulate(u32Mid, 
                    u32Left, st3dnrxparam.pst3dnrxParam[u32IsoLevel - 1].TFy[i].MTFR[j],
                    u32Right, st3dnrxparam.pst3dnrxParam[u32IsoLevel].TFy[i].MTFR[j]);
                stNrx.TFy[i].TFR[j] = Interpulate(u32Mid, 
                    u32Left, st3dnrxparam.pst3dnrxParam[u32IsoLevel - 1].TFy[i].TFR[j],
                    u32Right, st3dnrxparam.pst3dnrxParam[u32IsoLevel].TFy[i].TFR[j]);
            }
        }

        /* NRc */
        stNrx.NRc.SFC = Interpulate(u32Mid, 
            u32Left, st3dnrxparam.pst3dnrxParam[u32IsoLevel - 1].NRc.SFC,
            u32Right, st3dnrxparam.pst3dnrxParam[u32IsoLevel].NRc.SFC);
        stNrx.NRc.TFC = Interpulate(u32Mid, 
            u32Left, st3dnrxparam.pst3dnrxParam[u32IsoLevel - 1].NRc.TFC,
            u32Right, st3dnrxparam.pst3dnrxParam[u32IsoLevel].NRc.TFC);
        stNrx.NRc.TPC = Interpulate(u32Mid, 
            u32Left, st3dnrxparam.pst3dnrxParam[u32IsoLevel - 1].NRc.TPC,
            u32Right, st3dnrxparam.pst3dnrxParam[u32IsoLevel].NRc.TPC);
        stNrx.NRc.TRC = Interpulate(u32Mid, 
            u32Left, st3dnrxparam.pst3dnrxParam[u32IsoLevel - 1].NRc.TRC,
            u32Right, st3dnrxparam.pst3dnrxParam[u32IsoLevel].NRc.TRC);
    }  

    //printf("ISO:%d, u32IsoLevel:%d, SBS:%d\n", u32Iso, u32IsoLevel, stNrx.SFy[0].SBS[0]);

    s32Ret = Sceneauto_SetNrX(VpssGrp, &stNrx);
    if (HI_SUCCESS != s32Ret)
    {
        printf("SetNrX failed\n");
        return HI_FAILURE;
    }
    
    return HI_SUCCESS;
}


void *SceneAuto_3DnrThread(void* pVoid)
{
    HI_S32 s32Ret = HI_SUCCESS;
    HI_U32 u32Iso[MAX_ISP_COUNT] = {0,0};
    HI_U32 u32LastIso[MAX_ISP_COUNT] = {0,0};
    HI_S32 s32IspIndex,s32VpssIndex;
    ISP_DEV IspDev;
    VPSS_GRP VpssGrp;
    ISP_EXP_INFO_S stIspExpInfo;
    
    SCENEAUTO_SEPCIAL_SCENE_E eSpecialScene;
    eSpecialScene = g_eSpecialScene;

    prctl(PR_SET_NAME, (unsigned long)"hi_ScnAuto_3Dnr", 0, 0, 0);

    while (HI_TRUE == g_stSceneautoState.stThread3Dnr.bThreadFlag)
    {
        for (s32IspIndex = 0 ; s32IspIndex < g_stSceneautoParam.stCommParam.s32IspDevCount; s32IspIndex++)
        {
            IspDev = g_stSceneautoParam.pstIspParam[s32IspIndex].IspDev;

            /* calculate iso value (iso = AGain * DGain * ISPDGain * 100 >> 30) */
            s32Ret = HI_MPI_ISP_QueryExposureInfo(IspDev, &stIspExpInfo);
            if (HI_SUCCESS != s32Ret)
            {
                printf("HI_MPI_ISP_QueryExposureInfo failed\n");
            }           
            u32Iso[IspDev] = stIspExpInfo.u32ISO;            

            //set 3dnr
            if ((u32Iso[IspDev] != u32LastIso[IspDev]) || (eSpecialScene != g_eSpecialScene))
            {
                for (s32VpssIndex = 0; s32VpssIndex < g_stSceneautoParam.stCommParam.s32VpssGrpCount; s32VpssIndex++)
                {
                    VpssGrp = g_stSceneautoParam.pstVpssParam[s32VpssIndex].VpssGrp; 
                    
                    if (SCENEAUTO_SPECIAL_SCENE_HLC == g_eSpecialScene)     
                    {
                        //set HLC 3DNR param 
                        s32Ret = Sceneauto_Set3DNR(VpssGrp, u32Iso[IspDev], g_stSceneautoParam.stHlcParam.stHlc3dnr);
                        if (HI_SUCCESS != s32Ret)
                        {
                            printf("Sceneauto_SetHlc3DNR failed\n");
                        }
                    }
                    else if(SCENEAUTO_SPECIAL_SCENE_IR == g_eSpecialScene)
                    {
                        //set IR 3DNR param
                        s32Ret = Sceneauto_Set3DNR(VpssGrp, u32Iso[IspDev], g_stSceneautoParam.stIrParam.stIr3dnr);
                        if (HI_SUCCESS != s32Ret)
                        {
                            printf("Sceneauto_SetIr3DNR failed\n");
                        }
                    }
                    else if(SCENEAUTO_SPECIAL_SCENE_TRAFFIC == g_eSpecialScene)
                    {
                        //set Traffic 3DNR param
                        s32Ret = Sceneauto_Set3DNR(VpssGrp, u32Iso[IspDev], g_stSceneautoParam.stTrafficParam.stTraffic3dnr);
                        if (HI_SUCCESS != s32Ret)
                        {
                            printf("Sceneauto_SetTraffic3DNR failed\n");
                        }
                    }
                    else if(SCENEAUTO_SPECIAL_SCENE_FACE == g_eSpecialScene)
                    {
                        //set Traffic 3DNR param
                        s32Ret = Sceneauto_Set3DNRX(VpssGrp, u32Iso[IspDev], g_stSceneautoParam.stFaceParam.stFace3dnrx);
                        if (HI_SUCCESS != s32Ret)
                        {
                            printf("Sceneauto_SetFace3DNR failed\n");
                        }
                    }
                    else if(((SCENEAUTO_SPECIAL_SCENE_AUTO_FSWDR == g_eSpecialScene) && (SCENEAUTO_FSWDR_LONG_FRAME_MODE == g_eFSWDRMode)) ||
                        (SCENEAUTO_SPECIAL_SCENE_MANUAL_LONG_FRAME == g_eSpecialScene))
                    {
                        //set LongFrame 3DNR param
                        s32Ret = Sceneauto_Set3DNR(VpssGrp, u32Iso[IspDev], g_stSceneautoParam.stFswdrParam.stFSWDR3dnr);
                        if (HI_SUCCESS != s32Ret)
                        {
                            printf("Sceneauto_SetFSWDR3DNR failed\n");
                        }
                    }
                    else
                    {
                        //set Normal 3DNR param
                        s32Ret = Sceneauto_Set3DNR(VpssGrp, u32Iso[IspDev], g_stSceneautoParam.pstVpssParam[s32VpssIndex].st3dnrParam);
                        if (HI_SUCCESS != s32Ret)
                        {
                            printf("Sceneauto_SetNormal3DNR failed\n");
                        }
                    }

                    u32LastIso[IspDev] = u32Iso[IspDev];
                    eSpecialScene = g_eSpecialScene;
                }
            }
	    }	
		hi_usleep(200000);
    }
    
    return NULL;
}

void *SceneAuto_DrcThread(void* pVoid)
{
	HI_S32 s32Ret = HI_SUCCESS;
	HI_S32 s32Index;

    prctl(PR_SET_NAME, (unsigned long)"hi_ScnAuto_Drc", 0, 0, 0);

	while (HI_TRUE == g_stSceneautoState.stThreadDrc.bThreadFlag)
    {
    	for (s32Index = 0 ; s32Index < g_stSceneautoParam.stCommParam.s32IspDevCount; s32Index++)
    	{
    		s32Ret = Sceneauto_SetIspDrcParam(s32Index);
    		if (HI_SUCCESS != s32Ret)
	        {
	            printf("Sceneauto_SetIspDrcParam failed\n");
	        }
    	}

    	hi_usleep(200000);
    }
	
	return NULL;
}

HI_S32 Sceneauto_SetIspSpecialParam(HI_S32 s32Index)
{
	static HI_U32 au32LastDisExposure[MAX_ISP_COUNT] = {0};
	static HI_U8 au8LastDisAveLum[MAX_ISP_COUNT] = {0};
	static HI_U8 au8DisCount[MAX_ISP_COUNT] = {0};
	HI_S32 s32Ret = HI_SUCCESS;
	HI_U32 u32Exposure = 0;
	HI_U32 u32DeltaExposure = 0;
	HI_U8 u8AveLum = 0;
	HI_U8 u8DelataAveLum = 0;
	ISP_DEV IspDev;
    HI_U32 u32OnlineMode = 0;
	ISP_EXP_INFO_S stIspExpInfo;
    ISP_DIS_ATTR_S stDisAttr;
	
	IspDev = g_stSceneautoParam.pstIspParam[s32Index].IspDev;

    s32Ret = HI_MPI_SYS_GetViVpssMode(&u32OnlineMode);
    if (s32Ret != HI_SUCCESS)
    {
        printf("HI_MPI_SYS_GetViVpssMode err: 0x%x.\n", s32Ret);
        return HI_TRUE;
    }

    if(1 == u32OnlineMode) /* Is OnlineMode */
    {
        return HI_SUCCESS;
    }

	s32Ret = HI_MPI_ISP_QueryExposureInfo(IspDev, &stIspExpInfo);
    if (HI_SUCCESS != s32Ret)
    {
        printf("HI_MPI_ISP_QueryExposureInfo failed\n");
        return HI_FAILURE;
    }
    u32Exposure = stIspExpInfo.u32Exposure;
    u8AveLum = stIspExpInfo.u8AveLum;
	if (au32LastDisExposure[s32Index] == 0)
	{
		au32LastDisExposure[s32Index] = u32Exposure;
		return HI_SUCCESS;
	}

	/*******disable DIS function when luminance and exposure changed instantly*********/
	if ((au32LastDisExposure[s32Index] != u32Exposure)||(au8LastDisAveLum[s32Index] != u8AveLum))
	{
		if (u32Exposure > au32LastDisExposure[s32Index])
		{
			u32DeltaExposure = (u32Exposure * 256 / au32LastDisExposure[s32Index]) - 256;
		}
		else
		{
			u32DeltaExposure = (au32LastDisExposure[s32Index] * 256 / u32Exposure) - 256;
		}

		if (u8AveLum > au8LastDisAveLum[s32Index])
		{
			u8DelataAveLum = u8AveLum - au8LastDisAveLum[s32Index];
		}
		else
		{
			u8DelataAveLum = au8LastDisAveLum[s32Index] - u8AveLum;
		}

		s32Ret = HI_MPI_ISP_GetDISAttr(IspDev, &stDisAttr);
        if (HI_SUCCESS != s32Ret)
        {
            printf("HI_MPI_ISP_GetDISAttr failed\n");
            return HI_FAILURE;
        }
        
		if ((u32DeltaExposure > g_stSceneautoParam.stCommParam.u32DeltaDisExpThresh)||(u8DelataAveLum > g_stSceneautoParam.stCommParam.u32AveLumThresh))
		{
			if (stDisAttr.bEnable == HI_TRUE)
            {
                stDisAttr.bEnable = HI_FALSE;
                s32Ret = HI_MPI_ISP_SetDISAttr(IspDev, &stDisAttr);
                if (HI_SUCCESS != s32Ret)
                {
                    printf("HI_MPI_ISP_SetDISAttr failed\n");
                }
                sleep(1);
            }
            au8DisCount[s32Index] = 0;
		}
		else
		{
			if (stDisAttr.bEnable == HI_FALSE)
			{
				au8DisCount[s32Index]++;
				if (au8DisCount[s32Index] > 7)
				{
                    stDisAttr.bEnable = HI_TRUE;
                    s32Ret = HI_MPI_ISP_SetDISAttr(IspDev, &stDisAttr);
                    if (HI_SUCCESS != s32Ret)
                    {
                        printf("HI_MPI_ISP_SetDISAttr failed\n");
                    }
                    au8DisCount[s32Index] = 0;
				}
			}
		}

		au32LastDisExposure[s32Index] = u32Exposure;
		au8LastDisAveLum[s32Index] = u8AveLum;
	}

	return HI_SUCCESS;	
}

void * SceneAuto_SpecialThread(void* pVoid)
{
	HI_S32 s32Ret = HI_SUCCESS;
	HI_S32 s32Index;
    
    prctl(PR_SET_NAME, (unsigned long)"hi_ScnAuto_Spe", 0, 0, 0);

	while (HI_TRUE == g_stSceneautoState.stThreadSpecial.bThreadFlag)
    {
    	for (s32Index = 0 ; s32Index < g_stSceneautoParam.stCommParam.s32IspDevCount; s32Index++)
    	{
    		s32Ret = Sceneauto_SetIspSpecialParam(s32Index);
    		if (HI_SUCCESS != s32Ret)
	        {
	            printf("Sceneauto_SetIspSpecialParam failed\n");
	        }           
    	}

    	hi_usleep(80000);
    }
    
	return NULL;
}

HI_S32 Sceneauto_SetAutoFSWDR(HI_S32 s32Index, ISP_DEV IspDev, HI_U32 u32Exposure)
{
	static HI_U32 au32WDRCount0[MAX_ISP_COUNT] = {0};
	static HI_U32 au32WDRCount1[MAX_ISP_COUNT] = {0};
	HI_S32 s32Ret = HI_SUCCESS;
	ISP_EXPOSURE_ATTR_S stIspExposureAttr;
	ISP_EXP_INFO_S stIspExpInfo;
    ISP_WDR_EXPOSURE_ATTR_S stWDRExpAttr;

    s32Ret = HI_MPI_ISP_QueryExposureInfo(IspDev, &stIspExpInfo);
    if (HI_SUCCESS != s32Ret)
    {
        printf("HI_MPI_ISP_QueryExposureInfo failed\n");
        return HI_FAILURE;
    }
    s32Ret = HI_MPI_ISP_GetWDRExposureAttr(IspDev, &stWDRExpAttr);
    if (HI_SUCCESS != s32Ret)
    {
        printf("HI_MPI_ISP_SetWDRExposureAttr failed\n");
        return HI_FAILURE;
    }
    s32Ret = HI_MPI_ISP_GetExposureAttr(IspDev, &stIspExposureAttr);
    if (HI_SUCCESS != s32Ret)
    {
        printf("HI_MPI_ISP_GetExposureAttr failed\n");
        return HI_FAILURE;
    }

	if(SCENEAUTO_SPECIAL_SCENE_MANUAL_LONG_FRAME == g_eSpecialScene)
    {
        g_eFSWDRMode = SCENEAUTO_FSWDR_LONG_FRAME_MODE;
        stIspExposureAttr.stAuto.enFSWDRMode = (ISP_FSWDR_MODE_E)g_eFSWDRMode;
        s32Ret = HI_MPI_ISP_SetExposureAttr(IspDev, &stIspExposureAttr);
        if (HI_SUCCESS != s32Ret)
        {
            printf("HI_MPI_ISP_SetExposureAttr failed\n");
            return HI_FAILURE;
        }
    }
    else if(SCENEAUTO_SPECIAL_SCENE_MANUAL_NORMAL_WDR == g_eSpecialScene)
    {
        g_eFSWDRMode = SCENEAUTO_FSWDR_NORMAL_MODE;
        stIspExposureAttr.stAuto.enFSWDRMode = (ISP_FSWDR_MODE_E)g_eFSWDRMode;
        s32Ret = HI_MPI_ISP_SetExposureAttr(IspDev, &stIspExposureAttr);
        if (HI_SUCCESS != s32Ret)
        {
            printf("HI_MPI_ISP_SetExposureAttr failed\n");
            return HI_FAILURE;
        }
    }
    else
    {
    	if ((u32Exposure > g_stSceneautoParam.stFswdrParam.u32ISOUpTh) && (stIspExpInfo.u32RefExpRatio < g_stSceneautoParam.stFswdrParam.u32RefExpRatioTh))
        {
            au32WDRCount0[s32Index]++;
        }
        else
        {
            au32WDRCount0[s32Index] = 0;
        }

        if ((u32Exposure < g_stSceneautoParam.stFswdrParam.u32ISODnTh))
        {
            au32WDRCount1[s32Index]++;
        }
        else
        {
            au32WDRCount1[s32Index] = 0;
        }

        if(au32WDRCount0[s32Index] > 3)
        {
            g_eFSWDRMode = SCENEAUTO_FSWDR_LONG_FRAME_MODE;
            stIspExposureAttr.stAuto.enFSWDRMode = (ISP_FSWDR_MODE_E)g_eFSWDRMode;
            s32Ret = HI_MPI_ISP_SetExposureAttr(IspDev, &stIspExposureAttr);
            if (HI_SUCCESS != s32Ret)
            {
                printf("HI_MPI_ISP_SetExposureAttr failed\n");
                return HI_FAILURE;
            }
        }
        else if(au32WDRCount1[s32Index] > 3)
        {
            g_eFSWDRMode = SCENEAUTO_FSWDR_NORMAL_MODE;
            stIspExposureAttr.stAuto.enFSWDRMode = (ISP_FSWDR_MODE_E)g_eFSWDRMode;
            s32Ret = HI_MPI_ISP_SetExposureAttr(IspDev, &stIspExposureAttr);
            if (HI_SUCCESS != s32Ret)
            {
                printf("HI_MPI_ISP_SetExposureAttr failed\n");
                return HI_FAILURE;
            }
        }
    }
	
	return HI_SUCCESS;
}

HI_S32 Sceneauto_SetSharpen(HI_S32 s32Index, ISP_DEV IspDev, HI_U32 u32Exposure)
{
	static HI_U32 au32SharpenLastExposure[MAX_ISP_COUNT] = {0};
	HI_S32 s32Ret = HI_SUCCESS;
    HI_U32 u32ExpLevel = 0;
    HI_U32 u32LastExpLevel = 0;
	HI_U32 u32Count = g_stSceneautoParam.pstIspParam[s32Index].stSharpenParam.u32ExpCount;
    HI_U32 *pu32Thresh = g_stSceneautoParam.pstIspParam[s32Index].stSharpenParam.pu32ExpThresh;
	ISP_BAYER_SHARPEN_ATTR_S stIspBayerSharpenAttr;
    ISP_YUV_SHARPEN_ATTR_S stIspYuvSharpenAttr;
	u32ExpLevel = GetLevelLtoH(u32Exposure, u32ExpLevel, u32Count, pu32Thresh);
	u32LastExpLevel = GetLevelLtoH(au32SharpenLastExposure[s32Index], u32LastExpLevel, u32Count, pu32Thresh);

    if ((SCENEAUTO_SPECIAL_SCENE_IR != g_eSpecialScene) && 
        (SCENEAUTO_SPECIAL_SCENE_TRAFFIC != g_eSpecialScene) && 
        (SCENEAUTO_SPECIAL_SCENE_FACE != g_eSpecialScene) && 
        (u32ExpLevel != u32LastExpLevel))
    {
        s32Ret = HI_MPI_ISP_GetBayerSharpenAttr(IspDev, &stIspBayerSharpenAttr);
        if (HI_SUCCESS != s32Ret)
        {
            printf("HI_MPI_ISP_GetBayerSharpenAttr failed\n");
            return HI_FAILURE;
        }
        s32Ret = HI_MPI_ISP_GetYuvSharpenAttr(IspDev, &stIspYuvSharpenAttr);
        if (HI_SUCCESS != s32Ret)
        {
            printf("HI_MPI_ISP_GetYuvSharpenAttr failed\n");
            return HI_FAILURE;
        }

        stIspBayerSharpenAttr.stAuto.astShp[0].u8SharpenD = g_stSceneautoParam.pstIspParam[s32Index].stSharpenParam.pstDynamicSharpen[u32ExpLevel].u8SharpenD;
        stIspBayerSharpenAttr.stAuto.astShp[0].u8SharpenUd = g_stSceneautoParam.pstIspParam[s32Index].stSharpenParam.pstDynamicSharpen[u32ExpLevel].u8SharpenUd;
        stIspYuvSharpenAttr.stAuto.au8TextureSt[0] = g_stSceneautoParam.pstIspParam[s32Index].stSharpenParam.pstDynamicSharpen[u32ExpLevel].u8TextureSt;
        stIspYuvSharpenAttr.stAuto.au8EdgeSt[0] = g_stSceneautoParam.pstIspParam[s32Index].stSharpenParam.pstDynamicSharpen[u32ExpLevel].u8EdgeSt;
        stIspYuvSharpenAttr.stAuto.au8OverShoot[0] = g_stSceneautoParam.pstIspParam[s32Index].stSharpenParam.pstDynamicSharpen[u32ExpLevel].u8OverShoot;
        stIspYuvSharpenAttr.stAuto.au8UnderShoot[0] = g_stSceneautoParam.pstIspParam[s32Index].stSharpenParam.pstDynamicSharpen[u32ExpLevel].u8UnderShoot;
        stIspYuvSharpenAttr.stAuto.au8DetailCtrl[0] = g_stSceneautoParam.pstIspParam[s32Index].stSharpenParam.pstDynamicSharpen[u32ExpLevel].u8DetailCtrl;

        s32Ret = HI_MPI_ISP_SetBayerSharpenAttr(IspDev, &stIspBayerSharpenAttr);
        if (HI_SUCCESS != s32Ret)
        {
            printf("HI_MPI_ISP_SetBayerSharpenAttr failed\n");
            return HI_FAILURE;
        }
        s32Ret = HI_MPI_ISP_SetYuvSharpenAttr(IspDev, &stIspYuvSharpenAttr);
        if (HI_SUCCESS != s32Ret)
        {
            printf("HI_MPI_ISP_SetYuvSharpenAttr failed\n");
            return HI_FAILURE;
        }
    } 

    au32SharpenLastExposure[s32Index] = u32Exposure;
    
	return HI_SUCCESS;  
}

HI_S32 Sceneauto_SetDemosaic(HI_S32 s32Index, ISP_DEV IspDev, HI_U32 u32Exposure)
{
	static HI_U32 au32DemosaicLastExposure[MAX_ISP_COUNT] = {0};
	HI_S32 s32Ret = HI_SUCCESS;
    HI_U32 u32ExpLevel = 0;
    HI_U32 u32LastExpLevel = 0;
    HI_U32 u32Count = g_stSceneautoParam.pstIspParam[s32Index].stDemosaicParam.u32ExpCount;
    HI_U32 *pu32Thresh = g_stSceneautoParam.pstIspParam[s32Index].stDemosaicParam.pu32ExpThresh;
	ISP_DEMOSAIC_ATTR_S stIspDemosaicAttr;
	u32ExpLevel = GetLevelLtoH(u32Exposure, u32ExpLevel, u32Count, pu32Thresh);
	u32LastExpLevel = GetLevelLtoH(au32DemosaicLastExposure[s32Index], u32LastExpLevel, u32Count, pu32Thresh);

	if (u32ExpLevel != u32LastExpLevel)
	{
		s32Ret = HI_MPI_ISP_GetDemosaicAttr(IspDev, &stIspDemosaicAttr);
	    if (HI_SUCCESS != s32Ret)
	    {
	        printf("HI_MPI_ISP_GetDemosaicAttr failed\n");
	        return HI_FAILURE;
	    }
	    stIspDemosaicAttr.u8UuSlope = g_stSceneautoParam.pstIspParam[s32Index].stDemosaicParam.pstDemosaic[u32ExpLevel].u8UuSlope;
	    stIspDemosaicAttr.u8VaSlope = g_stSceneautoParam.pstIspParam[s32Index].stDemosaicParam.pstDemosaic[u32ExpLevel].u8VaSlope;
	    stIspDemosaicAttr.u8AaSlope = g_stSceneautoParam.pstIspParam[s32Index].stDemosaicParam.pstDemosaic[u32ExpLevel].u8AaSlope;
	    stIspDemosaicAttr.u8VhSlope = g_stSceneautoParam.pstIspParam[s32Index].stDemosaicParam.pstDemosaic[u32ExpLevel].u8VhSlope;
	    s32Ret = HI_MPI_ISP_SetDemosaicAttr(IspDev, &stIspDemosaicAttr);
	    if (HI_SUCCESS != s32Ret)
	    {
	        printf("HI_MPI_ISP_SetDemosaicAttr failed\n");
	        return HI_FAILURE;
	    }
    }
    
    au32DemosaicLastExposure[s32Index] = u32Exposure;

    return HI_SUCCESS;    
}

HI_S32 Sceneauto_SetFalseColor(HI_S32 s32Index, ISP_DEV IspDev, HI_U32 u32Exposure)
{
	HI_S32 s32Ret = HI_SUCCESS;
    HI_U32 u32ExpLevel = 0;
    HI_U32 u32Count = g_stSceneautoParam.pstIspParam[s32Index].stFalseColorParam.u32ExpCount;
    HI_U32 *pu32Thresh = g_stSceneautoParam.pstIspParam[s32Index].stFalseColorParam.pu32ExpThresh;
    ISP_ANTI_FALSECOLOR_S stIspAntiFalsecolor;
    u32ExpLevel = GetLevelLtoH(u32Exposure, u32ExpLevel, u32Count, pu32Thresh);

    s32Ret = HI_MPI_ISP_GetAntiFalseColorAttr(IspDev, &stIspAntiFalsecolor);
    if (HI_SUCCESS != s32Ret)
    {
        printf("HI_MPI_ISP_GetAntiFalseColorAttr failed\n");
        return HI_FAILURE;
    }
    stIspAntiFalsecolor.u8Strength = g_stSceneautoParam.pstIspParam[s32Index].stFalseColorParam.pstFalseColor[u32ExpLevel].u8Strength;
    s32Ret = HI_MPI_ISP_SetAntiFalseColorAttr(IspDev, &stIspAntiFalsecolor);
    if (HI_SUCCESS != s32Ret)
    {
        printf("HI_MPI_ISP_SetAntiFalseColorAttr failed\n");
        return HI_FAILURE;
    }
    
	return HI_SUCCESS;
}

HI_S32 Sceneauto_SetDP(HI_S32 s32Index, ISP_DEV IspDev, HI_U32 u32Exposure)
{
	HI_S32 s32Ret = HI_SUCCESS;
	HI_U32 u32ExpLevel = 0;
    HI_U32 u32Count = g_stSceneautoParam.pstIspParam[s32Index].stDpParam.u32ExpCount;
    HI_U32 *pu32Thresh = g_stSceneautoParam.pstIspParam[s32Index].stDpParam.pu32ExpThresh;
    ISP_DP_ATTR_S *pstDPAttr = NULL;

    u32ExpLevel = GetLevelLtoH(u32Exposure, u32ExpLevel, u32Count, pu32Thresh);
	pstDPAttr = (ISP_DP_ATTR_S *)malloc(sizeof(ISP_DP_ATTR_S));
    if(HI_NULL == pstDPAttr)
    {
        printf("malloc ISP_DP_ATTR_S failed\n");
        return HI_FAILURE;
    }

    s32Ret = HI_MPI_ISP_GetDPAttr(IspDev, pstDPAttr);
    if (HI_SUCCESS != s32Ret)
    {
        printf("HI_MPI_ISP_GetDPAttr failed\n");
        FREE_PTR(pstDPAttr);
        return HI_FAILURE;
    }
    pstDPAttr->stDynamicAttr.u16Slope = g_stSceneautoParam.pstIspParam[s32Index].stDpParam.pstDPAttr[u32ExpLevel].u16Slope;
    pstDPAttr->stDynamicAttr.u16Thresh = g_stSceneautoParam.pstIspParam[s32Index].stDpParam.pstDPAttr[u32ExpLevel].u16Thresh;
    s32Ret = HI_MPI_ISP_SetDPAttr(IspDev, pstDPAttr);
    if (HI_SUCCESS != s32Ret)
    {
        printf("HI_MPI_ISP_SetDPAttr failed\n");
        FREE_PTR(pstDPAttr);
        return HI_FAILURE;
    }

    FREE_PTR(pstDPAttr);
    return HI_SUCCESS;
}

HI_S32 Sceneauto_SetAeRelatedExp(HI_S32 s32Index, ISP_DEV IspDev, HI_U32 u32Exposure)
{
	static HI_U32 au32AeLastExposure[MAX_ISP_COUNT] = {0};
	HI_S32 s32Ret = HI_SUCCESS;
	HI_U32 u32ExpLevel = 0;
	HI_U32 u32Count = 0;
    HI_U32 *pu32Thresh = NULL;
    ISP_EXPOSURE_ATTR_S stIspExposureAttr;
    HI_U32 u32LastExposure;
    
	u32LastExposure = au32AeLastExposure[s32Index];

	s32Ret = HI_MPI_ISP_GetExposureAttr(IspDev, &stIspExposureAttr);
    if (HI_SUCCESS != s32Ret)
    {
        printf("HI_MPI_ISP_GetExposureAttr failed\n");
        return HI_FAILURE;
    }

    if (SCENEAUTO_SPECIAL_SCENE_IR == g_eSpecialScene)
    {
        u32Count = g_stSceneautoParam.stIrParam.u32ExpCount;
        if (u32Exposure > u32LastExposure)
        {
            pu32Thresh = g_stSceneautoParam.stIrParam.pu32ExpThreshLtoH;
            u32ExpLevel = GetLevelLtoH(u32Exposure, u32ExpLevel, u32Count, pu32Thresh);
            stIspExposureAttr.stAuto.u8Compensation = g_stSceneautoParam.stIrParam.pu8ExpCompensation[u32ExpLevel];
            stIspExposureAttr.stAuto.u8MaxHistOffset = g_stSceneautoParam.stIrParam.pu8MaxHistOffset[u32ExpLevel];
        }
        else if (u32Exposure < u32LastExposure)
        {
            pu32Thresh = g_stSceneautoParam.stIrParam.pu32ExpThreshHtoL;
            u32ExpLevel = GetLevelHtoL(u32Exposure, u32ExpLevel, u32Count, pu32Thresh);
            stIspExposureAttr.stAuto.u8Compensation = g_stSceneautoParam.stIrParam.pu8ExpCompensation[u32ExpLevel];
            stIspExposureAttr.stAuto.u8MaxHistOffset = g_stSceneautoParam.stIrParam.pu8MaxHistOffset[u32ExpLevel];
        }       
    }
	else if(SCENEAUTO_SPECIAL_SCENE_TRAFFIC == g_eSpecialScene)
    {
        hi_usleep(200000);
        stIspExposureAttr.stAuto.u8Compensation = g_stSceneautoParam.stTrafficParam.u8ExpCompensation;
        stIspExposureAttr.stAuto.u8MaxHistOffset = g_stSceneautoParam.stTrafficParam.u8MaxHistoffset;
    }
    else if(((SCENEAUTO_SPECIAL_SCENE_AUTO_FSWDR == g_eSpecialScene) && (SCENEAUTO_FSWDR_LONG_FRAME_MODE == g_eFSWDRMode)) ||
        ((SCENEAUTO_SPECIAL_SCENE_MANUAL_LONG_FRAME == g_eSpecialScene) && (SCENEAUTO_FSWDR_LONG_FRAME_MODE == g_eFSWDRMode)))
    {
        hi_usleep(200000);        
        u32Count = g_stSceneautoParam.stFswdrParam.u32ExpCount;
        if (u32Exposure > u32LastExposure)
        {
            pu32Thresh = g_stSceneautoParam.stFswdrParam.pu32ExpThreshLtoH;
            u32ExpLevel = GetLevelLtoH(u32Exposure, u32ExpLevel, u32Count, pu32Thresh);
            stIspExposureAttr.stAuto.u8Compensation = g_stSceneautoParam.stFswdrParam.pu8ExpCompensation[u32ExpLevel];
            stIspExposureAttr.stAuto.u8MaxHistOffset = g_stSceneautoParam.stFswdrParam.pu8MaxHistOffset[u32ExpLevel];
        }
        else if (u32Exposure < u32LastExposure)
        {
            pu32Thresh = g_stSceneautoParam.stFswdrParam.pu32ExpThreshHtoL;
            u32ExpLevel = GetLevelHtoL(u32Exposure, u32ExpLevel, u32Count, pu32Thresh);
            stIspExposureAttr.stAuto.u8Compensation = g_stSceneautoParam.stFswdrParam.pu8ExpCompensation[u32ExpLevel];
            stIspExposureAttr.stAuto.u8MaxHistOffset = g_stSceneautoParam.stFswdrParam.pu8MaxHistOffset[u32ExpLevel];
        }       
    }
    else if ((SCENEAUTO_SPECIAL_SCENE_NONE == g_eSpecialScene) ||
        ((SCENEAUTO_SPECIAL_SCENE_AUTO_FSWDR == g_eSpecialScene) && (SCENEAUTO_FSWDR_NORMAL_MODE == g_eFSWDRMode)) ||
        ((SCENEAUTO_SPECIAL_SCENE_MANUAL_NORMAL_WDR == g_eSpecialScene) && (SCENEAUTO_FSWDR_NORMAL_MODE == g_eFSWDRMode)))
    {
        hi_usleep(200000);
        u32Count = g_stSceneautoParam.pstIspParam[s32Index].stAeParam.u32ExpCount;
        if (u32Exposure > u32LastExposure)
        {
            pu32Thresh = g_stSceneautoParam.pstIspParam[s32Index].stAeParam.pu32AEExpLtoHThresh;
            u32ExpLevel = GetLevelLtoH(u32Exposure, u32ExpLevel, u32Count, pu32Thresh);
            stIspExposureAttr.stAuto.u8Compensation = g_stSceneautoParam.pstIspParam[s32Index].stAeParam.pstAERelatedExp[u32ExpLevel].u8AECompesation;
            stIspExposureAttr.stAuto.u8MaxHistOffset = g_stSceneautoParam.pstIspParam[s32Index].stAeParam.pstAERelatedExp[u32ExpLevel].u8AEHistOffset;
        }
        else if (u32Exposure < u32LastExposure)
        {
            pu32Thresh = g_stSceneautoParam.pstIspParam[s32Index].stAeParam.pu32AEExpHtoLThresh;
            u32ExpLevel = GetLevelHtoL(u32Exposure, u32ExpLevel, u32Count, pu32Thresh);
            stIspExposureAttr.stAuto.u8Compensation = g_stSceneautoParam.pstIspParam[s32Index].stAeParam.pstAERelatedExp[u32ExpLevel].u8AECompesation;
            stIspExposureAttr.stAuto.u8MaxHistOffset = g_stSceneautoParam.pstIspParam[s32Index].stAeParam.pstAERelatedExp[u32ExpLevel].u8AEHistOffset;
        }
    }

    s32Ret = HI_MPI_ISP_SetExposureAttr(IspDev, &stIspExposureAttr);
    if (HI_SUCCESS != s32Ret)
    {
        printf("HI_MPI_ISP_SetExposureAttr failed\n");
        return HI_FAILURE;
    }

    //u32LastExposure = u32Exposure;
    au32AeLastExposure[s32Index] = u32Exposure;

    return HI_SUCCESS;    
}

HI_S32 Sceneauto_SetGamma(HI_S32 s32Index, ISP_DEV IspDev, HI_U32 u32Exposure)
{
    static HI_U32 au32GammaCount[MAX_ISP_COUNT] = {0};
    static HI_U32 au32GammaLastExpLevel[MAX_ISP_COUNT] = {0};
    static HI_U32 au32GammaLastExposure[MAX_ISP_COUNT] = {0};
    HI_U32 u32GammaCount;
    HI_U32 u32LastExpLevel;
    HI_U32 u32LastExposure;
    HI_S32 s32Ret = HI_SUCCESS;
    HI_U32 u32Temp;
    HI_S32 i, j;
    HI_U32 u32ExpLevel = 0;
    HI_U32 u32Count = g_stSceneautoParam.pstIspParam[s32Index].stGammaParam.u32ExpCount;
    HI_U32 *pu32Thresh = NULL;
    ISP_GAMMA_ATTR_S stIspGammaAttr;

	u32GammaCount = au32GammaCount[s32Index];
	u32LastExpLevel = au32GammaLastExpLevel[s32Index];
	u32LastExposure = au32GammaLastExposure[s32Index];
	
  	if ((u32Exposure != u32LastExposure) 
        && (SCENEAUTO_SPECIAL_SCENE_IR != g_eSpecialScene) 
        && (SCENEAUTO_SPECIAL_SCENE_HLC != g_eSpecialScene)
        && (SCENEAUTO_SPECIAL_SCENE_TRAFFIC != g_eSpecialScene)
        && (SCENEAUTO_SPECIAL_SCENE_FACE != g_eSpecialScene))
    {
        if (u32Exposure > u32LastExposure)
        {
            pu32Thresh = g_stSceneautoParam.pstIspParam[s32Index].stGammaParam.pu32ExpThreshLtoH;
            u32ExpLevel = GetLevelLtoH(u32Exposure, u32ExpLevel, u32Count, pu32Thresh);
        }
        else if (u32Exposure < u32LastExposure)
        {
            pu32Thresh = g_stSceneautoParam.pstIspParam[s32Index].stGammaParam.pu32ExpThreshHtoL;
            u32ExpLevel = GetLevelHtoL(u32Exposure, u32ExpLevel, u32Count, pu32Thresh);
        }
           
        if (u32ExpLevel != u32LastExpLevel)
        {
            u32GammaCount++;
            au32GammaCount[s32Index]++;
            if (u32GammaCount > g_stSceneautoParam.pstIspParam[s32Index].stGammaParam.u32DelayCount)
            {
                //set gamma
                for (j = 0; j < g_stSceneautoParam.pstIspParam[s32Index].stGammaParam.u32Interval; j++)
                {
                    s32Ret = HI_MPI_ISP_GetGammaAttr(IspDev, &stIspGammaAttr);
                    if (HI_SUCCESS != s32Ret)
                    {
                        printf("HI_MPI_ISP_GetGammaAttr failed\n");
                        return HI_FAILURE;
                    }
                    for (i = 0; i < 257; i++)
                    {
                        if (g_stSceneautoParam.pstIspParam[s32Index].stGammaParam.pstGamma[u32LastExpLevel].u16Table[i] > g_stSceneautoParam.pstIspParam[s32Index].stGammaParam.pstGamma[u32ExpLevel].u16Table[i])
                        {
                            u32Temp = ((HI_U32)(g_stSceneautoParam.pstIspParam[s32Index].stGammaParam.pstGamma[u32LastExpLevel].u16Table[i] - g_stSceneautoParam.pstIspParam[s32Index].stGammaParam.pstGamma[u32ExpLevel].u16Table[i])) << 8;
                            u32Temp = (u32Temp * (j + 1)) / g_stSceneautoParam.pstIspParam[s32Index].stGammaParam.u32Interval;
                            u32Temp = u32Temp >> 8;
                            stIspGammaAttr.u16Table[i] = g_stSceneautoParam.pstIspParam[s32Index].stGammaParam.pstGamma[u32LastExpLevel].u16Table[i] - (HI_U32)u32Temp;
                        }
                        else
                        {
                            u32Temp = ((HI_U32)(g_stSceneautoParam.pstIspParam[s32Index].stGammaParam.pstGamma[u32ExpLevel].u16Table[i] - g_stSceneautoParam.pstIspParam[s32Index].stGammaParam.pstGamma[u32LastExpLevel].u16Table[i])) << 8;
                            u32Temp = (u32Temp * (j + 1)) / g_stSceneautoParam.pstIspParam[s32Index].stGammaParam.u32Interval;
                            u32Temp = u32Temp >> 8;
                            stIspGammaAttr.u16Table[i] = g_stSceneautoParam.pstIspParam[s32Index].stGammaParam.pstGamma[u32LastExpLevel].u16Table[i] + (HI_U32)u32Temp;
                        }
                    }
                    stIspGammaAttr.enCurveType = ISP_GAMMA_CURVE_USER_DEFINE;
                    s32Ret = HI_MPI_ISP_SetGammaAttr(IspDev, &stIspGammaAttr);
                    if (HI_SUCCESS != s32Ret)
                    {
                        printf("HI_MPI_ISP_SetGammaAttr failed\n");
                        return HI_FAILURE;
                    }
                    hi_usleep(80000);
                }
                // u32LastExposure = u32Exposure;
                 au32GammaLastExposure[s32Index] = u32Exposure;
                // u32LastExpLevel = u32ExpLevel;
                 au32GammaLastExpLevel[s32Index] = u32ExpLevel;
                // u32GammaCount = 0;
                 au32GammaCount[s32Index] = 0;
            }
        }
        else
        {
            //u32GammaCount = 0;
            au32GammaCount[s32Index] = 0;
        }
    }
    
    return HI_SUCCESS;
}

HI_S32 Sceneauto_SetIspNoramlParam(HI_S32 s32Index)
{
	HI_S32 s32Ret = HI_SUCCESS;
	HI_U32 u32Exposure = 0;
	ISP_DEV IspDev;
	ISP_EXP_INFO_S stIspExpInfo;
	ISP_INNER_STATE_INFO_S stIspInnerStateInfo;
	ISP_DRC_ATTR_S stIspDrcAttr;
    ISP_WDR_MODE_S stIspWdrMode;
    

	IspDev = g_stSceneautoParam.pstIspParam[s32Index].IspDev;

	/*********************calculate exposure value*********************/
    s32Ret = HI_MPI_ISP_QueryExposureInfo(IspDev, &stIspExpInfo);
    if (HI_SUCCESS != s32Ret)
    {
        printf("HI_MPI_ISP_QueryExposureInfo failed\n");
        return HI_FAILURE;
    }

    s32Ret = HI_MPI_ISP_QueryInnerStateInfo(IspDev, &stIspInnerStateInfo);
    if (HI_SUCCESS != s32Ret)
    {
        printf("HI_MPI_ISP_QueryExposureInfo failed\n");
        return HI_FAILURE;
    }
    if (stIspInnerStateInfo.u32DRCStrengthActual < g_stSceneautoParam.stCommParam.u32DRCStrengthThresh)
    {
        stIspInnerStateInfo.u32DRCStrengthActual = g_stSceneautoParam.stCommParam.u32DRCStrengthThresh;
    }
    
    s32Ret = HI_MPI_ISP_GetDRCAttr(IspDev, &stIspDrcAttr);
    if (HI_SUCCESS != s32Ret)
    {
        printf("HI_MPI_ISP_GetDRCAttr failed\n");
        return HI_FAILURE;
    }

    s32Ret = HI_MPI_ISP_GetWDRMode(IspDev, &stIspWdrMode);
    if (HI_SUCCESS != s32Ret)
    {
        printf("HI_MPI_ISP_GetWDRMode failed\n");
        return HI_FAILURE;
    }
    
    if (WDR_MODE_NONE != stIspWdrMode.enWDRMode)
    {
        //in wdr mode, use iso as exposure value
        u32Exposure = stIspExpInfo.u32ISO;
    }
    else
    {
        u32Exposure = ((HI_U64)stIspExpInfo.u32AGain * (HI_U64)stIspExpInfo.u32DGain * (HI_U64)stIspExpInfo.u32ISPDGain) 
                      * ((HI_U64)stIspExpInfo.u32ExpTime)
                      >> 30;

        if (HI_TRUE == stIspDrcAttr.bEnable)
        {
            u32Exposure = (HI_U64)u32Exposure * (HI_U64)stIspInnerStateInfo.u32DRCStrengthActual / (HI_U64)g_stSceneautoParam.stCommParam.u32DRCStrengthThresh;
        }
    }

    //printf("\n ###################### u32Exposure = %d ####################\n",u32Exposure);
	/*********************set gamma releated with exposure*********************/
    s32Ret = Sceneauto_SetGamma(s32Index, IspDev, u32Exposure);
    if (HI_SUCCESS != s32Ret)
    {
        printf("Sceneauto_SetGamma failed\n");
    }

 	/*********************set ae param releated with exposure******************/
    s32Ret = Sceneauto_SetAeRelatedExp(s32Index, IspDev, u32Exposure);
    if (HI_SUCCESS != s32Ret)
    {
        printf("Sceneauto_SetAE failed\n");
    }

    /*********************set dp param releated with exposure******************/
    s32Ret = Sceneauto_SetDP(s32Index, IspDev, u32Exposure);
    if (HI_SUCCESS != s32Ret)
    {
        printf("Sceneauto_SetDP failed\n");
    }

    /*********************set falsecolor param releated with exposure******************/
    s32Ret = Sceneauto_SetFalseColor(s32Index, IspDev, u32Exposure);
    if (HI_SUCCESS != s32Ret)
    {
        printf("Sceneauto_SetDP failed\n");
    }

    /*********************set ae demosaic related with exposure*********************/
    s32Ret = Sceneauto_SetDemosaic(s32Index, IspDev, u32Exposure);
    if (HI_SUCCESS != s32Ret)
    {
        printf("Sceneauto_SetDemosaic failed\n");
    } 

	/*********************set sharpen related with exposure*********************/
    s32Ret = Sceneauto_SetSharpen(s32Index, IspDev, u32Exposure);
    if (HI_SUCCESS != s32Ret)
    {
        printf("Sceneauto_SetSharpen failed\n");
    } 

    if ((SCENEAUTO_SPECIAL_SCENE_AUTO_FSWDR == g_eSpecialScene) || 
    	(SCENEAUTO_SPECIAL_SCENE_MANUAL_LONG_FRAME == g_eSpecialScene) || 
       	(SCENEAUTO_SPECIAL_SCENE_MANUAL_NORMAL_WDR == g_eSpecialScene))
    {
        s32Ret = Sceneauto_SetAutoFSWDR(s32Index, IspDev, u32Exposure);
        if (HI_SUCCESS != s32Ret)
        {
            printf("Sceneauto_SetAutoFSWDR failed\n");
        }
    }

	return HI_SUCCESS;
}

void* SceneAuto_NormalThread(void* pVoid)
{
	HI_S32 s32Ret = HI_SUCCESS;
	HI_S32 s32Index;

    prctl(PR_SET_NAME, (unsigned long)"hi_ScnAuto_Nor", 0, 0, 0);

	while (HI_TRUE == g_stSceneautoState.stThreadNormal.bThreadFlag)
    {
    	for (s32Index = 0 ; s32Index < g_stSceneautoParam.stCommParam.s32IspDevCount; s32Index++)
    	{
    		s32Ret = Sceneauto_SetIspNoramlParam(s32Index);
    		if (HI_SUCCESS != s32Ret)
	        {
	            printf("Sceneauto_SetIspNoramlParam failed\n");
	        }
    	}

    	hi_usleep(1000000);
    }
    
	return NULL;
}

HI_S32 Sceneauto_SetViDefaultParam(HI_S32 s32Index)
{	
	HI_S32 s32Ret = HI_SUCCESS;
	VI_DEV ViDev;
	VI_DCI_PARAM_S stDciParam;

	ViDev = g_stSceneautoParam.pstViParam[s32Index].ViDev;
	
	/*********************set DCI*********************/
    s32Ret = HI_MPI_VI_GetDCIParam(ViDev, &stDciParam);
    if (HI_SUCCESS != s32Ret)
    {
        printf("HI_MPI_VI_GetDCIParam failed\n");
        return HI_FAILURE;
    } 
    stDciParam.bEnable = g_stSceneautoParam.pstViParam[s32Index].stDciParam.bDCIEnable;
    stDciParam.u32BlackGain = g_stSceneautoParam.pstViParam[s32Index].stDciParam.u32DCIBlackGain;
    stDciParam.u32ContrastGain = g_stSceneautoParam.pstViParam[s32Index].stDciParam.u32DCIContrastGain;
    stDciParam.u32LightGain = g_stSceneautoParam.pstViParam[s32Index].stDciParam.u32DCILightGain;
    s32Ret = HI_MPI_VI_SetDCIParam(ViDev, &stDciParam);
    if (HI_SUCCESS != s32Ret)
    {
        printf("HI_MPI_VI_SetDCIParam failed\n");
        return HI_FAILURE;
    }
    
	return HI_SUCCESS;
}

HI_S32 Sceneauto_SetViPreviousParam(HI_S32 s32Index)
{
	HI_S32 s32Ret = HI_SUCCESS;
	VI_DEV ViDev;
	VI_DCI_PARAM_S stViDciParam;

	ViDev = g_stSceneautoParam.pstViParam[s32Index].ViDev;

	//DCI
	s32Ret = HI_MPI_VI_GetDCIParam(ViDev, &stViDciParam);
    if (HI_SUCCESS != s32Ret)
    {
        printf("HI_MPI_VI_GetDCIParam failed\n");
        return HI_FAILURE;
    }
    stViDciParam.bEnable = g_stSceneautoState.pstPrevisouViParam[s32Index].stDci.bDCIEnable;
    stViDciParam.u32BlackGain = g_stSceneautoState.pstPrevisouViParam[s32Index].stDci.u32DCIBlackGain;
    stViDciParam.u32ContrastGain = g_stSceneautoState.pstPrevisouViParam[s32Index].stDci.u32DCIContrastGain;
    stViDciParam.u32LightGain = g_stSceneautoState.pstPrevisouViParam[s32Index].stDci.u32DCILightGain;
	s32Ret = HI_MPI_VI_SetDCIParam(ViDev, &stViDciParam);
    if (HI_SUCCESS != s32Ret)
    {
        printf("HI_MPI_VI_SetDCIParam failed\n");
        return HI_FAILURE;
    }
	
	return HI_SUCCESS;
}

HI_S32 Sceneauto_GetViPreviousParam(HI_S32 s32Index)
{
	HI_S32 s32Ret = HI_SUCCESS;
	VI_DEV ViDev;
	VI_DCI_PARAM_S stViDciParam = {0};

	ViDev = g_stSceneautoParam.pstViParam[s32Index].ViDev;

	//DCI
	s32Ret = HI_MPI_VI_GetDCIParam(ViDev, &stViDciParam);
    if (HI_SUCCESS != s32Ret)
    {
        printf("HI_MPI_VI_GetDCIParam failed\n");
        return HI_FAILURE;
    }
    g_stSceneautoState.pstPrevisouViParam[s32Index].stDci.bDCIEnable = stViDciParam.bEnable;
    g_stSceneautoState.pstPrevisouViParam[s32Index].stDci.u32DCIBlackGain = stViDciParam.u32BlackGain;
    g_stSceneautoState.pstPrevisouViParam[s32Index].stDci.u32DCIContrastGain = stViDciParam.u32ContrastGain;
    g_stSceneautoState.pstPrevisouViParam[s32Index].stDci.u32DCILightGain = stViDciParam.u32LightGain;
	
	return HI_SUCCESS;
}

HI_S32 Sceneauto_SetIspDefaultParam(HI_S32 s32Index)
{
	HI_S32 s32Ret = HI_SUCCESS;
	HI_S32 i;
	ISP_DEV IspDev;
	HI_U32 u32ExpLevel = 0;
	HI_U32 u32Exposure;
	ISP_AE_ROUTE_S stAeRoute;
	ISP_EXPOSURE_ATTR_S stExposureAttr;
	ISP_EXP_INFO_S stIspExpInfo;
	ISP_WDR_MODE_S stIspWdrMode;
	ISP_GAMMA_ATTR_S stGammaAttr;
	ISP_ANTI_FALSECOLOR_S stIspAntiFalsecolor;
	ISP_DEMOSAIC_ATTR_S stIspDemosaicAttr;
	ISP_BAYER_SHARPEN_ATTR_S stIspBayerSharpenAttr;
	ISP_YUV_SHARPEN_ATTR_S stIspYuvSharpenAttr;
	ISP_DP_ATTR_S *pstDPAttr = HI_NULL;

	IspDev = g_stSceneautoParam.pstIspParam[s32Index].IspDev;

	/*********************set ae route*********************/
	s32Ret = HI_MPI_ISP_GetAERouteAttr(IspDev, &stAeRoute);
	if (HI_SUCCESS != s32Ret)
	{
	    printf("HI_MPI_ISP_GetAERouteAttr failed\n");
	    return HI_FAILURE;
	}
	stAeRoute.u32TotalNum = g_stSceneautoParam.pstIspParam[s32Index].stAeParam.stAeRoute.u32TotalNum;
	for (i = 0; i < g_stSceneautoParam.pstIspParam[s32Index].stAeParam.stAeRoute.u32TotalNum; i++)
	{
	    stAeRoute.astRouteNode[i].u32IntTime = g_stSceneautoParam.pstIspParam[s32Index].stAeParam.stAeRoute.pstRuteNode[i].u32IntTime;
	    stAeRoute.astRouteNode[i].u32SysGain = g_stSceneautoParam.pstIspParam[s32Index].stAeParam.stAeRoute.pstRuteNode[i].u32SysGain;
	}
	s32Ret = HI_MPI_ISP_SetAERouteAttr(IspDev, &stAeRoute);
	if (HI_SUCCESS != s32Ret)
	{
	    printf("HI_MPI_ISP_SetAERouteAttr failed\n");
	    return HI_FAILURE;
	}

	/*********************set ae runintervel*********************/
    s32Ret = HI_MPI_ISP_GetExposureAttr(IspDev, &stExposureAttr);  
    if (HI_SUCCESS != s32Ret)
    {
        printf("HI_MPI_ISP_GetExposureAttr failed\n");
        return HI_FAILURE;
    }
    stExposureAttr.u8AERunInterval = g_stSceneautoParam.pstIspParam[s32Index].stAeParam.u8AERunInterval;
    s32Ret = HI_MPI_ISP_SetExposureAttr(IspDev, &stExposureAttr);  
    if (HI_SUCCESS != s32Ret)
    {
        printf("HI_MPI_ISP_SetExposureAttr failed\n");
        return HI_FAILURE;
    }

	s32Ret = HI_MPI_ISP_QueryExposureInfo(IspDev, &stIspExpInfo);
    if (HI_SUCCESS != s32Ret)
    {
        printf("HI_MPI_ISP_QueryExposureInfo failed\n");
        return HI_FAILURE;
    }
    s32Ret = HI_MPI_ISP_GetWDRMode(IspDev, &stIspWdrMode);
    if (HI_SUCCESS != s32Ret)
    {
        printf("HI_MPI_ISP_GetWDRMode failed\n");
        return HI_FAILURE;
    }

    if (WDR_MODE_NONE != stIspWdrMode.enWDRMode)
    {
        u32Exposure = stIspExpInfo.u32ISO;
    }
    else
    {
        u32Exposure = ((HI_U64)stIspExpInfo.u32AGain * (HI_U64)stIspExpInfo.u32DGain * (HI_U64)stIspExpInfo.u32ISPDGain >> 30) 
                          * ((HI_U64)stIspExpInfo.u32ExpTime);
    }

	/*********************set gamma*********************/
    u32ExpLevel = GetLevelLtoH(u32Exposure, u32ExpLevel, 
								g_stSceneautoParam.pstIspParam[s32Index].stGammaParam.u32ExpCount,
								g_stSceneautoParam.pstIspParam[s32Index].stGammaParam.pu32ExpThreshLtoH);

    s32Ret = HI_MPI_ISP_GetGammaAttr(IspDev, &stGammaAttr);  
    if (HI_SUCCESS != s32Ret)
    {
        printf("HI_MPI_ISP_GetGammaAttr failed\n");
        return HI_FAILURE;
    }
    stGammaAttr.enCurveType = ISP_GAMMA_CURVE_USER_DEFINE;
    for (i = 0; i < 257; i++)
    {
        stGammaAttr.u16Table[i] = g_stSceneautoParam.pstIspParam[s32Index].stGammaParam.pstGamma[u32ExpLevel].u16Table[i];
    }
    s32Ret = HI_MPI_ISP_SetGammaAttr(IspDev, &stGammaAttr);  
    if (HI_SUCCESS != s32Ret)
    {
        printf("HI_MPI_ISP_SetGammaAttr failed\n");
        return HI_FAILURE;
    }

    /*********************set falsecolor*********************/
    u32ExpLevel = GetLevelLtoH(u32Exposure, u32ExpLevel, 
							 	g_stSceneautoParam.pstIspParam[s32Index].stFalseColorParam.u32ExpCount,
							 	g_stSceneautoParam.pstIspParam[s32Index].stFalseColorParam.pu32ExpThresh);
    s32Ret = HI_MPI_ISP_GetAntiFalseColorAttr(IspDev, &stIspAntiFalsecolor);
    if (HI_SUCCESS != s32Ret)
    {
        printf("HI_MPI_ISP_GetAntiFalseColorAttr failed\n");
        return HI_FAILURE;
    }
    stIspAntiFalsecolor.u8Strength = g_stSceneautoParam.pstIspParam[s32Index].stFalseColorParam.pstFalseColor[u32ExpLevel].u8Strength;
    s32Ret = HI_MPI_ISP_SetAntiFalseColorAttr(IspDev, &stIspAntiFalsecolor);
    if (HI_SUCCESS != s32Ret)
    {
        printf("HI_MPI_ISP_SetAntiFalseColorAttr failed\n");
        return HI_FAILURE;
    }

	/*********************set demosaic*********************/
    u32ExpLevel = GetLevelLtoH(u32Exposure, u32ExpLevel, 
								g_stSceneautoParam.pstIspParam[s32Index].stDemosaicParam.u32ExpCount,
								g_stSceneautoParam.pstIspParam[s32Index].stDemosaicParam.pu32ExpThresh);
    s32Ret = HI_MPI_ISP_GetDemosaicAttr(IspDev, &stIspDemosaicAttr);
    if (HI_SUCCESS != s32Ret)
    {
        printf("HI_MPI_ISP_GetDemosaicAttr failed\n");
        return HI_FAILURE;
    }
    stIspDemosaicAttr.u8UuSlope = g_stSceneautoParam.pstIspParam[s32Index].stDemosaicParam.pstDemosaic[u32ExpLevel].u8UuSlope;
    stIspDemosaicAttr.u8VaSlope = g_stSceneautoParam.pstIspParam[s32Index].stDemosaicParam.pstDemosaic[u32ExpLevel].u8VaSlope;
    stIspDemosaicAttr.u8AaSlope = g_stSceneautoParam.pstIspParam[s32Index].stDemosaicParam.pstDemosaic[u32ExpLevel].u8AaSlope;
    stIspDemosaicAttr.u8VhSlope = g_stSceneautoParam.pstIspParam[s32Index].stDemosaicParam.pstDemosaic[u32ExpLevel].u8VhSlope;
    s32Ret = HI_MPI_ISP_SetDemosaicAttr(IspDev, &stIspDemosaicAttr);
    if (HI_SUCCESS != s32Ret)
    {
        printf("HI_MPI_ISP_SetDemosaicAttr failed\n");
        return HI_FAILURE;
    } 

    /*********************set sharpen*********************/
    u32ExpLevel = GetLevelLtoH(u32Exposure, u32ExpLevel,
    							g_stSceneautoParam.pstIspParam[s32Index].stSharpenParam.u32ExpCount,
    							g_stSceneautoParam.pstIspParam[s32Index].stSharpenParam.pu32ExpThresh);
	    
    s32Ret = HI_MPI_ISP_GetBayerSharpenAttr(IspDev, &stIspBayerSharpenAttr);
    if (HI_SUCCESS != s32Ret)
    {
        printf("HI_MPI_ISP_GetBayerSharpenAttr failed\n");
        return HI_FAILURE;
    }
    s32Ret = HI_MPI_ISP_GetYuvSharpenAttr(IspDev, &stIspYuvSharpenAttr);
    if (HI_SUCCESS != s32Ret)
    {
        printf("HI_MPI_ISP_GetYuvSharpenAttr failed\n");
        return HI_FAILURE;
    }
    for (i = 0; i < EXPOSURE_LEVEL; i++)
    {
    	if (0 ==  i)
    	{
    		// iso < 100, sharpen param related with exposure
    		stIspBayerSharpenAttr.stAuto.astShp[i].u8SharpenD = g_stSceneautoParam.pstIspParam[s32Index].stSharpenParam.pstDynamicSharpen[u32ExpLevel].u8SharpenD;
	        stIspBayerSharpenAttr.stAuto.astShp[i].u8SharpenUd = g_stSceneautoParam.pstIspParam[s32Index].stSharpenParam.pstDynamicSharpen[u32ExpLevel].u8SharpenUd;
	        stIspYuvSharpenAttr.stAuto.au8TextureSt[i] = g_stSceneautoParam.pstIspParam[s32Index].stSharpenParam.pstDynamicSharpen[u32ExpLevel].u8TextureSt;
	        stIspYuvSharpenAttr.stAuto.au8EdgeSt[i] = g_stSceneautoParam.pstIspParam[s32Index].stSharpenParam.pstDynamicSharpen[u32ExpLevel].u8EdgeSt;
	        stIspYuvSharpenAttr.stAuto.au8OverShoot[i] = g_stSceneautoParam.pstIspParam[s32Index].stSharpenParam.pstDynamicSharpen[u32ExpLevel].u8OverShoot;
	        stIspYuvSharpenAttr.stAuto.au8UnderShoot[i] = g_stSceneautoParam.pstIspParam[s32Index].stSharpenParam.pstDynamicSharpen[u32ExpLevel].u8UnderShoot;
	        stIspYuvSharpenAttr.stAuto.au8DetailCtrl[i] = g_stSceneautoParam.pstIspParam[s32Index].stSharpenParam.pstDynamicSharpen[u32ExpLevel].u8DetailCtrl;
    	}
    	else
    	{
    		stIspBayerSharpenAttr.stAuto.astShp[i].u8SharpenD = g_stSceneautoParam.pstIspParam[s32Index].stSharpenParam.astDefaultSharpen[i].u8SharpenD;
	        stIspBayerSharpenAttr.stAuto.astShp[i].u8SharpenUd = g_stSceneautoParam.pstIspParam[s32Index].stSharpenParam.astDefaultSharpen[i].u8SharpenUd;
	        stIspYuvSharpenAttr.stAuto.au8TextureSt[i] = g_stSceneautoParam.pstIspParam[s32Index].stSharpenParam.astDefaultSharpen[i].u8TextureSt;
	        stIspYuvSharpenAttr.stAuto.au8EdgeSt[i] = g_stSceneautoParam.pstIspParam[s32Index].stSharpenParam.astDefaultSharpen[i].u8EdgeSt;
	        stIspYuvSharpenAttr.stAuto.au8OverShoot[i] = g_stSceneautoParam.pstIspParam[s32Index].stSharpenParam.astDefaultSharpen[i].u8OverShoot;
	        stIspYuvSharpenAttr.stAuto.au8UnderShoot[i] = g_stSceneautoParam.pstIspParam[s32Index].stSharpenParam.astDefaultSharpen[i].u8UnderShoot;
	        stIspYuvSharpenAttr.stAuto.au8DetailCtrl[i] = g_stSceneautoParam.pstIspParam[s32Index].stSharpenParam.astDefaultSharpen[i].u8DetailCtrl;
    	}
    }
    s32Ret = HI_MPI_ISP_SetBayerSharpenAttr(IspDev, &stIspBayerSharpenAttr);
    if (HI_SUCCESS != s32Ret)
    {
        printf("HI_MPI_ISP_SetBayerSharpenAttr failed\n");
        return HI_FAILURE;
    }
    s32Ret = HI_MPI_ISP_SetYuvSharpenAttr(IspDev, &stIspYuvSharpenAttr);
    if (HI_SUCCESS != s32Ret)
    {
        printf("HI_MPI_ISP_SetYuvSharpenAttr failed\n");
        return HI_FAILURE;
    }

    /*********************set DPC*********************/
    pstDPAttr = (ISP_DP_ATTR_S *)malloc(sizeof(ISP_DP_ATTR_S));
	if(NULL == pstDPAttr)
	{
      printf("mallloc ISP_DP_ATTR_S failed\n");
	  return HI_FAILURE;
	}
    u32ExpLevel = GetLevelLtoH(u32Exposure, u32ExpLevel, 
								g_stSceneautoParam.pstIspParam[s32Index].stDpParam.u32ExpCount,
								g_stSceneautoParam.pstIspParam[s32Index].stDpParam.pu32ExpThresh);
    s32Ret = HI_MPI_ISP_GetDPAttr(IspDev, pstDPAttr);
    if (HI_SUCCESS != s32Ret)
    {
        printf("HI_MPI_ISP_GetDPAttr failed\n");
        FREE_PTR(pstDPAttr);
        return HI_FAILURE;
    }
    pstDPAttr->stDynamicAttr.u16Slope = g_stSceneautoParam.pstIspParam[s32Index].stDpParam.pstDPAttr[u32ExpLevel].u16Slope;
    pstDPAttr->stDynamicAttr.u16Thresh = g_stSceneautoParam.pstIspParam[s32Index].stDpParam.pstDPAttr[u32ExpLevel].u16Thresh;
    s32Ret = HI_MPI_ISP_SetDPAttr(IspDev, pstDPAttr);
    if (HI_SUCCESS != s32Ret)
    {
        printf("HI_MPI_ISP_SetDPAttr failed\n");
        FREE_PTR(pstDPAttr);
        return HI_FAILURE;
    }

    FREE_PTR(pstDPAttr); 

	return HI_SUCCESS;
}

HI_S32 Sceneauto_SetIspPreviousParam(HI_S32 s32Index)
{
	HI_S32 s32Ret = HI_SUCCESS;
	HI_S32 i,j;
	ISP_DEV IspDev;
    ISP_WB_ATTR_S stIspWbAttr;
	ISP_COLORMATRIX_ATTR_S stIspCCMAttr;    
	ISP_DEMOSAIC_ATTR_S stIspDemosaicAttr;
	ISP_BAYER_SHARPEN_ATTR_S stIspBayerSharpenAttr;
    ISP_YUV_SHARPEN_ATTR_S stIspYuvSharpenAttr;
    ISP_GAMMA_ATTR_S stIspGammaAttr;
    ISP_EXPOSURE_ATTR_S stIspExposureAttr;
    ISP_AE_ROUTE_S stIspAeRoute;
    ISP_SATURATION_ATTR_S stIspSaturationAttr;
    ISP_DRC_ATTR_S stIspDrcAttr;
    ISP_WDR_EXPOSURE_ATTR_S stIspWdrExposureAttr;
    ISP_DP_ATTR_S *pstIspDpAttr = HI_NULL;

	IspDev = g_stSceneautoParam.pstIspParam[s32Index].IspDev;

    //awb
    s32Ret = HI_MPI_ISP_GetWBAttr(IspDev, &stIspWbAttr);
    if (HI_SUCCESS != s32Ret)
    {
        printf("HI_MPI_ISP_GetWBAttr failed\n");
        return HI_FAILURE;
    }
    stIspWbAttr.enOpType = (ISP_OP_TYPE_E)g_stSceneautoState.pstPreviousIspParam[s32Index].stColor.u8AwbType;
    s32Ret = HI_MPI_ISP_SetWBAttr(IspDev, &stIspWbAttr);
    if (HI_SUCCESS != s32Ret)
    {
        printf("HI_MPI_ISP_SetWBAttr failed\n");
        return HI_FAILURE;
    }
    //ccm
	s32Ret = HI_MPI_ISP_GetCCMAttr(IspDev, &stIspCCMAttr);
	if (HI_SUCCESS != s32Ret)
    {
        printf("HI_MPI_ISP_GetCCMAttr failed\n");
        return HI_FAILURE;
    }
    stIspCCMAttr.enOpType = (ISP_OP_TYPE_E)g_stSceneautoState.pstPreviousIspParam[s32Index].stColor.u8CcmType;
	s32Ret = HI_MPI_ISP_SetCCMAttr(IspDev, &stIspCCMAttr);
	if (HI_SUCCESS != s32Ret)
    {
        printf("HI_MPI_ISP_SetCCMAttr failed\n");
        return HI_FAILURE;
    }

	//demosaic
	s32Ret = HI_MPI_ISP_GetDemosaicAttr(IspDev, &stIspDemosaicAttr);
    if (HI_SUCCESS != s32Ret)
    {
        printf("HI_MPI_ISP_GetDemosaicAttr failed\n");
        return HI_FAILURE;
    }
	stIspDemosaicAttr.u8UuSlope = g_stSceneautoState.pstPreviousIspParam[s32Index].stDemosaic.u8UuSlope;
    stIspDemosaicAttr.u8AaSlope = g_stSceneautoState.pstPreviousIspParam[s32Index].stDemosaic.u8AaSlope;
    stIspDemosaicAttr.u8VaSlope = g_stSceneautoState.pstPreviousIspParam[s32Index].stDemosaic.u8VaSlope;
    stIspDemosaicAttr.u8VhSlope = g_stSceneautoState.pstPreviousIspParam[s32Index].stDemosaic.u8VhSlope;
	s32Ret = HI_MPI_ISP_SetDemosaicAttr(IspDev, &stIspDemosaicAttr);
    if (HI_SUCCESS != s32Ret)
    {
        printf("HI_MPI_ISP_SetDemosaicAttr failed\n");
        return HI_FAILURE;
    }    

    //Sharpen
    s32Ret = HI_MPI_ISP_GetBayerSharpenAttr(IspDev, &stIspBayerSharpenAttr);
    if (HI_SUCCESS != s32Ret)
    {
        printf("HI_MPI_ISP_GetBayerSharpenAttr failed\n");
        return HI_FAILURE;
    }
    s32Ret = HI_MPI_ISP_GetYuvSharpenAttr(IspDev, &stIspYuvSharpenAttr);
    if (HI_SUCCESS != s32Ret)
    {
        printf("HI_MPI_ISP_GetYuvSharpenAttr failed\n");
        return HI_FAILURE;
    }
    for (i = 0; i < EXPOSURE_LEVEL; i++)
    {
    	stIspBayerSharpenAttr.stAuto.astShp[i].u8SharpenD = g_stSceneautoState.pstPreviousIspParam[s32Index].stSharpen[i].u8SharpenD;
		stIspBayerSharpenAttr.stAuto.astShp[i].u16LumaThd = g_stSceneautoState.pstPreviousIspParam[s32Index].stSharpen[i].u16LumThresh;
		stIspBayerSharpenAttr.stAuto.astShp[i].u8SharpenUd = g_stSceneautoState.pstPreviousIspParam[s32Index].stSharpen[i].u8SharpenUd;
		stIspYuvSharpenAttr.stAuto.au8TextureSt[i] = g_stSceneautoState.pstPreviousIspParam[s32Index].stSharpen[i].u8TextureSt;
		stIspYuvSharpenAttr.stAuto.au8EdgeSt[i] = g_stSceneautoState.pstPreviousIspParam[s32Index].stSharpen[i].u8EdgeSt;
        stIspYuvSharpenAttr.stAuto.au8OverShoot[i] = g_stSceneautoState.pstPreviousIspParam[s32Index].stSharpen[i].u8OverShoot;
        stIspYuvSharpenAttr.stAuto.au8UnderShoot[i] = g_stSceneautoState.pstPreviousIspParam[s32Index].stSharpen[i].u8UnderShoot;
        stIspYuvSharpenAttr.stAuto.au8EdgeThd[i] = g_stSceneautoState.pstPreviousIspParam[s32Index].stSharpen[i].u8EdgeThd;
        stIspYuvSharpenAttr.stAuto.au8TextureThd[i] = g_stSceneautoState.pstPreviousIspParam[s32Index].stSharpen[i].u8TextureThd;
        stIspYuvSharpenAttr.stAuto.au8DetailCtrl[i] = g_stSceneautoState.pstPreviousIspParam[s32Index].stSharpen[i].u8DetailCtrl;
    }
    s32Ret = HI_MPI_ISP_SetBayerSharpenAttr(IspDev, &stIspBayerSharpenAttr);
    if (HI_SUCCESS != s32Ret)
    {
        printf("HI_MPI_ISP_SetBayerSharpenAttr failed\n");
        return HI_FAILURE;
    }
    s32Ret = HI_MPI_ISP_SetYuvSharpenAttr(IspDev, &stIspYuvSharpenAttr);
    if (HI_SUCCESS != s32Ret)
    {
        printf("HI_MPI_ISP_SetYuvSharpenAttr failed\n");
        return HI_FAILURE;
    }

    //gamma
    s32Ret = HI_MPI_ISP_GetGammaAttr(IspDev, &stIspGammaAttr);
    if (HI_SUCCESS != s32Ret)
    {
        printf("HI_MPI_ISP_GetGammaAttr failed\n");
        return HI_FAILURE;
    }
    g_stSceneautoState.pstPreviousIspParam[s32Index].stGamma.u8CurveType = stIspGammaAttr.enCurveType;
    for (i = 0; i < 257; i++)
    {
        stIspGammaAttr.u16Table[i] = g_stSceneautoState.pstPreviousIspParam[s32Index].stGamma.u16Table[i];
    }
    s32Ret = HI_MPI_ISP_SetGammaAttr(IspDev, &stIspGammaAttr);
    if (HI_SUCCESS != s32Ret)
    {
        printf("HI_MPI_ISP_SetGammaAttr failed\n");
        return HI_FAILURE;
    }

    //ae
    s32Ret = HI_MPI_ISP_GetExposureAttr(IspDev, &stIspExposureAttr);
    if (HI_SUCCESS != s32Ret)
    {
        printf("HI_MPI_ISP_GetExposureAttr failed\n");
        return HI_FAILURE;
    }
    stIspExposureAttr.u8AERunInterval = g_stSceneautoState.pstPreviousIspParam[s32Index].stExposure.u8AERunInterval;
    stIspExposureAttr.stAuto.u8Speed = g_stSceneautoState.pstPreviousIspParam[s32Index].stExposure.u8Speed;
    stIspExposureAttr.stAuto.u8Tolerance = g_stSceneautoState.pstPreviousIspParam[s32Index].stExposure.u8Tolerance;
    stIspExposureAttr.stAuto.stAEDelayAttr.u16BlackDelayFrame = g_stSceneautoState.pstPreviousIspParam[s32Index].stExposure.u16BlackDelayFrame;
    stIspExposureAttr.stAuto.stAEDelayAttr.u16WhiteDelayFrame = g_stSceneautoState.pstPreviousIspParam[s32Index].stExposure.u16WhiteDelayFrame;
    stIspExposureAttr.stAuto.stSysGainRange.u32Max = g_stSceneautoState.pstPreviousIspParam[s32Index].stExposure.u32SysGainMax;
    stIspExposureAttr.stAuto.u8Compensation = g_stSceneautoState.pstPreviousIspParam[s32Index].stExposure.u8AECompesation;
    stIspExposureAttr.stAuto.u8MaxHistOffset = g_stSceneautoState.pstPreviousIspParam[s32Index].stExposure.u8AEHistOffset;
    stIspExposureAttr.stAuto.u16HistRatioSlope = g_stSceneautoState.pstPreviousIspParam[s32Index].stExposure.u16HistRatioSlope;
    stIspExposureAttr.stAuto.enFSWDRMode = (ISP_FSWDR_MODE_E)g_stSceneautoState.pstPreviousIspParam[s32Index].stExposure.u8FSWDRLFMode;
    for (i = 0; i < AE_WEIGHT_ROW; i++)
    {
        for (j = 0; j < AE_WEIGHT_COLUMN; j++)
        {
            stIspExposureAttr.stAuto.au8Weight[i][j] = g_stSceneautoState.pstPreviousIspParam[s32Index].stExposure.au8Weight[i][j];
        }
    }
    s32Ret = HI_MPI_ISP_SetExposureAttr(IspDev, &stIspExposureAttr);
    if (HI_SUCCESS != s32Ret)
    {
        printf("HI_MPI_ISP_SetExposureAttr failed\n");
        return HI_FAILURE;
    }
    s32Ret = HI_MPI_ISP_GetAERouteAttr(IspDev, &stIspAeRoute);
    if (HI_SUCCESS != s32Ret)
    {
        printf("HI_MPI_ISP_GetAERouteAttr failed\n");
        return HI_FAILURE;
    }
    stIspAeRoute.u32TotalNum = g_stSceneautoState.pstPreviousIspParam[s32Index].stExposure.s32TotalNum;
    for (i = 0; i < stIspAeRoute.u32TotalNum; i++)
    {
        stIspAeRoute.astRouteNode[i].u32IntTime = g_stSceneautoState.pstPreviousIspParam[s32Index].stExposure.astRouteNode[i].u32IntTime;
        stIspAeRoute.astRouteNode[i].u32SysGain = g_stSceneautoState.pstPreviousIspParam[s32Index].stExposure.astRouteNode[i].u32SysGain;
    }
    s32Ret = HI_MPI_ISP_SetAERouteAttr(IspDev, &stIspAeRoute);
    if (HI_SUCCESS != s32Ret)
    {
        printf("HI_MPI_ISP_SetAERouteAttr failed\n");
        return HI_FAILURE;
    }

    //saturation
    s32Ret = HI_MPI_ISP_GetSaturationAttr(IspDev, &stIspSaturationAttr);
    if (HI_SUCCESS != s32Ret)
    {
        printf("HI_MPI_ISP_GetSaturationAttr failed\n");
        return HI_FAILURE;
    }
    stIspSaturationAttr.enOpType = (ISP_OP_TYPE_E)g_stSceneautoState.pstPreviousIspParam[s32Index].stSaturation.u8OpType;
    for (i = 0; i < 16; i++)
    {
        stIspSaturationAttr.stAuto.au8Sat[i] = g_stSceneautoState.pstPreviousIspParam[s32Index].stSaturation.u8Saturation[i];
    }
    s32Ret = HI_MPI_ISP_SetSaturationAttr(IspDev, &stIspSaturationAttr);
    if (HI_SUCCESS != s32Ret)
    {
        printf("HI_MPI_ISP_SetSaturationAttr failed\n");
        return HI_FAILURE;
    }

    //DRC
    s32Ret = HI_MPI_ISP_GetDRCAttr(IspDev, &stIspDrcAttr);
    if (HI_SUCCESS != s32Ret)
    {
        printf("HI_MPI_ISP_GetDRCAttr failed\n");
        return HI_FAILURE;
    }
    stIspDrcAttr.bEnable = g_stSceneautoState.pstPreviousIspParam[s32Index].stDrc.bEnable;
    stIspDrcAttr.enOpType = (ISP_OP_TYPE_E)g_stSceneautoState.pstPreviousIspParam[s32Index].stDrc.u8OpType;
    stIspDrcAttr.stManual.u32Strength = g_stSceneautoState.pstPreviousIspParam[s32Index].stDrc.u32Strength;
    stIspDrcAttr.stAuto.u32Strength = g_stSceneautoState.pstPreviousIspParam[s32Index].stDrc.u32DRCAutoStrength;
    stIspDrcAttr.u32WhiteLevel = g_stSceneautoState.pstPreviousIspParam[s32Index].stDrc.u32WhiteLevel;
    stIspDrcAttr.u32BlackLevel = g_stSceneautoState.pstPreviousIspParam[s32Index].stDrc.u32BlackLevel;
    stIspDrcAttr.u32Asymmetry = g_stSceneautoState.pstPreviousIspParam[s32Index].stDrc.u32Asymmetry;
    stIspDrcAttr.u32BrightEnhance = g_stSceneautoState.pstPreviousIspParam[s32Index].stDrc.u32BrightEnhance;
    stIspDrcAttr.FilterMux = (ISP_DRC_CURVE_TYPE_E)g_stSceneautoState.pstPreviousIspParam[s32Index].stDrc.u8FilterMux;
    stIspDrcAttr.stDrcCurve.u32SlopeMax = g_stSceneautoState.pstPreviousIspParam[s32Index].stDrc.u32SlopeMax;
    stIspDrcAttr.stDrcCurve.u32SlopeMin = g_stSceneautoState.pstPreviousIspParam[s32Index].stDrc.u32SlopeMin;
    stIspDrcAttr.stDrcCurve.u32VarianceSpace = g_stSceneautoState.pstPreviousIspParam[s32Index].stDrc.u32VarianceSpace;
    stIspDrcAttr.stDrcCurve.u32VarianceIntensity = g_stSceneautoState.pstPreviousIspParam[s32Index].stDrc.u32VarianceIntensity;
    stIspDrcAttr.stDrcCurveEx.u32Contrast = g_stSceneautoState.pstPreviousIspParam[s32Index].stDrc.u32Contrast;
    stIspDrcAttr.stDrcCurveEx.u32BrightPr = g_stSceneautoState.pstPreviousIspParam[s32Index].stDrc.u32BrightPr;
    stIspDrcAttr.stDrcCurveEx.u32Svariance = g_stSceneautoState.pstPreviousIspParam[s32Index].stDrc.u32Svariance;
    stIspDrcAttr.stDrcCurveEx.u32DarkEnhance = g_stSceneautoState.pstPreviousIspParam[s32Index].stDrc.u32DarkEnhance;
	s32Ret = HI_MPI_ISP_SetDRCAttr(IspDev, &stIspDrcAttr);
    if (HI_SUCCESS != s32Ret)
    {
        printf("HI_MPI_ISP_SetDRCAttr failed\n");
        return HI_FAILURE;
    }
    
    //WDR Exposure attr
    s32Ret = HI_MPI_ISP_GetWDRExposureAttr(IspDev, &stIspWdrExposureAttr);
    if (HI_SUCCESS != s32Ret)
    {
        printf("HI_MPI_ISP_GetWDRExposureAttr failed\n");
        return HI_FAILURE;
    }
    stIspWdrExposureAttr.enExpRatioType = (ISP_OP_TYPE_E)g_stSceneautoState.pstPreviousIspParam[s32Index].stWdrExposureAttr.u8ExpRatioType; 
	stIspWdrExposureAttr.u32ExpRatioMax = g_stSceneautoState.pstPreviousIspParam[s32Index].stWdrExposureAttr.u32ExpRatioMax; 
	stIspWdrExposureAttr.u32ExpRatioMin = g_stSceneautoState.pstPreviousIspParam[s32Index].stWdrExposureAttr.u32ExpRatioMin; 
	for (i = 0; i < 3; i++)
	{
		stIspWdrExposureAttr.au32ExpRatio[i] = g_stSceneautoState.pstPreviousIspParam[s32Index].stWdrExposureAttr.au32ExpRatio[i];
	}
	s32Ret = HI_MPI_ISP_SetWDRExposureAttr(IspDev, &stIspWdrExposureAttr);
    if (HI_SUCCESS != s32Ret)
    {
        printf("HI_MPI_ISP_SetWDRExposureAttr failed\n");
        return HI_FAILURE;
    }

	//DP
	pstIspDpAttr = (ISP_DP_ATTR_S *)malloc(sizeof(ISP_DP_ATTR_S));
    if(HI_NULL == pstIspDpAttr)
    {
        printf("Malloc ISP_DP_ATTR_S failed\n");
        return HI_FAILURE;
    }
	s32Ret = HI_MPI_ISP_GetDPAttr(IspDev, pstIspDpAttr);
    if (HI_SUCCESS != s32Ret)
    {
        printf("HI_MPI_ISP_GetDPAttr failed\n");
        FREE_PTR(pstIspDpAttr);
        return HI_FAILURE;
    }
    pstIspDpAttr->stDynamicAttr.u16Slope = g_stSceneautoState.pstPreviousIspParam[s32Index].stDp.u16Slope;
    pstIspDpAttr->stDynamicAttr.u16Thresh = g_stSceneautoState.pstPreviousIspParam[s32Index].stDp.u16Thresh;
	s32Ret = HI_MPI_ISP_SetDPAttr(IspDev, pstIspDpAttr);
    if (HI_SUCCESS != s32Ret)
    {
        printf("HI_MPI_ISP_SetDPAttr failed\n");
        FREE_PTR(pstIspDpAttr);
        return HI_FAILURE;
    }

	FREE_PTR(pstIspDpAttr);

	return HI_SUCCESS;
}

HI_S32 Sceneauto_GetIspPreviousParam(HI_S32 s32Index)
{
	HI_S32 s32Ret = HI_SUCCESS;
	HI_S32 i,j;
	ISP_DEV IspDev;
    ISP_WB_ATTR_S stIspWbAttr;
	ISP_COLORMATRIX_ATTR_S stIspCCMAttr;
	ISP_DEMOSAIC_ATTR_S stIspDemosaicAttr;
	ISP_BAYER_SHARPEN_ATTR_S stIspBayerSharpenAttr;
    ISP_YUV_SHARPEN_ATTR_S stIspYuvSharpenAttr;
    ISP_GAMMA_ATTR_S stIspGammaAttr;
    ISP_EXPOSURE_ATTR_S stIspExposureAttr;
    ISP_AE_ROUTE_S stIspAeRoute;
    ISP_SATURATION_ATTR_S stIspSaturationAttr;
    ISP_DRC_ATTR_S stIspDrcAttr;
    ISP_WDR_EXPOSURE_ATTR_S stIspWdrExposureAttr;
    ISP_DP_ATTR_S *pstIspDpAttr = HI_NULL;

	IspDev = g_stSceneautoParam.pstIspParam[s32Index].IspDev;

    //awb
    s32Ret = HI_MPI_ISP_GetWBAttr(IspDev, &stIspWbAttr);
    if (HI_SUCCESS != s32Ret)
    {
        printf("HI_MPI_ISP_GetDemosaicAttr failed\n");
        return HI_FAILURE;
    }
    g_stSceneautoState.pstPreviousIspParam[s32Index].stColor.u8AwbType = (HI_U8)stIspWbAttr.enOpType;

    //ccm
	s32Ret = HI_MPI_ISP_GetCCMAttr(IspDev, &stIspCCMAttr);
	if (HI_SUCCESS != s32Ret)
    {
        printf("HI_MPI_ISP_GetCCMAttr failed\n");
        return HI_FAILURE;
    }
    g_stSceneautoState.pstPreviousIspParam[s32Index].stColor.u8CcmType = (HI_U8)stIspCCMAttr.enOpType;
    
	//demosaic
	s32Ret = HI_MPI_ISP_GetDemosaicAttr(IspDev, &stIspDemosaicAttr);
    if (HI_SUCCESS != s32Ret)
    {
        printf("HI_MPI_ISP_GetDemosaicAttr failed\n");
        return HI_FAILURE;
    }
    g_stSceneautoState.pstPreviousIspParam[s32Index].stDemosaic.u8UuSlope = stIspDemosaicAttr.u8UuSlope;
    g_stSceneautoState.pstPreviousIspParam[s32Index].stDemosaic.u8AaSlope = stIspDemosaicAttr.u8AaSlope;
    g_stSceneautoState.pstPreviousIspParam[s32Index].stDemosaic.u8VaSlope = stIspDemosaicAttr.u8VaSlope;
    g_stSceneautoState.pstPreviousIspParam[s32Index].stDemosaic.u8VhSlope = stIspDemosaicAttr.u8VhSlope;

    //Sharpen
    s32Ret = HI_MPI_ISP_GetBayerSharpenAttr(IspDev, &stIspBayerSharpenAttr);
    if (HI_SUCCESS != s32Ret)
    {
        printf("HI_MPI_ISP_GetBayerSharpenAttr failed\n");
        return HI_FAILURE;
    }
    s32Ret = HI_MPI_ISP_GetYuvSharpenAttr(IspDev, &stIspYuvSharpenAttr);
    if (HI_SUCCESS != s32Ret)
    {
        printf("HI_MPI_ISP_GetYuvSharpenAttr failed\n");
        return HI_FAILURE;
    }
    for (i = 0; i < EXPOSURE_LEVEL; i++)
    {
    	g_stSceneautoState.pstPreviousIspParam[s32Index].stSharpen[i].u8SharpenD = stIspBayerSharpenAttr.stAuto.astShp[i].u8SharpenD;
		g_stSceneautoState.pstPreviousIspParam[s32Index].stSharpen[i].u16LumThresh = stIspBayerSharpenAttr.stAuto.astShp[i].u16LumaThd;
		g_stSceneautoState.pstPreviousIspParam[s32Index].stSharpen[i].u8SharpenUd = stIspBayerSharpenAttr.stAuto.astShp[i].u8SharpenUd;
		g_stSceneautoState.pstPreviousIspParam[s32Index].stSharpen[i].u8TextureSt = stIspYuvSharpenAttr.stAuto.au8TextureSt[i];
		g_stSceneautoState.pstPreviousIspParam[s32Index].stSharpen[i].u8EdgeSt = stIspYuvSharpenAttr.stAuto.au8EdgeSt[i];
        g_stSceneautoState.pstPreviousIspParam[s32Index].stSharpen[i].u8OverShoot = stIspYuvSharpenAttr.stAuto.au8OverShoot[i];
        g_stSceneautoState.pstPreviousIspParam[s32Index].stSharpen[i].u8UnderShoot = stIspYuvSharpenAttr.stAuto.au8UnderShoot[i];
        g_stSceneautoState.pstPreviousIspParam[s32Index].stSharpen[i].u8EdgeThd = stIspYuvSharpenAttr.stAuto.au8EdgeThd[i];
        g_stSceneautoState.pstPreviousIspParam[s32Index].stSharpen[i].u8TextureThd = stIspYuvSharpenAttr.stAuto.au8TextureThd[i];
        g_stSceneautoState.pstPreviousIspParam[s32Index].stSharpen[i].u8DetailCtrl = stIspYuvSharpenAttr.stAuto.au8DetailCtrl[i];
    }

    //gamma
    s32Ret = HI_MPI_ISP_GetGammaAttr(IspDev, &stIspGammaAttr);
    if (HI_SUCCESS != s32Ret)
    {
        printf("HI_MPI_ISP_GetGammaAttr failed\n");
        return HI_FAILURE;
    }
    g_stSceneautoState.pstPreviousIspParam[s32Index].stGamma.u8CurveType = stIspGammaAttr.enCurveType;
    for (i = 0; i < 257; i++)
    {
        g_stSceneautoState.pstPreviousIspParam[s32Index].stGamma.u16Table[i] = stIspGammaAttr.u16Table[i];
    }

    //ae
    s32Ret = HI_MPI_ISP_GetExposureAttr(IspDev, &stIspExposureAttr);
    if (HI_SUCCESS != s32Ret)
    {
        printf("HI_MPI_ISP_GetExposureAttr failed\n");
        return HI_FAILURE;
    }
    g_stSceneautoState.pstPreviousIspParam[s32Index].stExposure.u8AERunInterval = stIspExposureAttr.u8AERunInterval;
    g_stSceneautoState.pstPreviousIspParam[s32Index].stExposure.u8Speed = stIspExposureAttr.stAuto.u8Speed;
    g_stSceneautoState.pstPreviousIspParam[s32Index].stExposure.u8Tolerance = stIspExposureAttr.stAuto.u8Tolerance;
    g_stSceneautoState.pstPreviousIspParam[s32Index].stExposure.u16BlackDelayFrame = stIspExposureAttr.stAuto.stAEDelayAttr.u16BlackDelayFrame;
    g_stSceneautoState.pstPreviousIspParam[s32Index].stExposure.u16WhiteDelayFrame = stIspExposureAttr.stAuto.stAEDelayAttr.u16WhiteDelayFrame;
    g_stSceneautoState.pstPreviousIspParam[s32Index].stExposure.u32SysGainMax = stIspExposureAttr.stAuto.stSysGainRange.u32Max;
    g_stSceneautoState.pstPreviousIspParam[s32Index].stExposure.u8AECompesation = stIspExposureAttr.stAuto.u8Compensation;
    g_stSceneautoState.pstPreviousIspParam[s32Index].stExposure.u8AEHistOffset = stIspExposureAttr.stAuto.u8MaxHistOffset;
    g_stSceneautoState.pstPreviousIspParam[s32Index].stExposure.u16HistRatioSlope = stIspExposureAttr.stAuto.u16HistRatioSlope;
    g_stSceneautoState.pstPreviousIspParam[s32Index].stExposure.u8FSWDRLFMode = (HI_U8)stIspExposureAttr.stAuto.enFSWDRMode;
    for (i = 0; i < AE_WEIGHT_ROW; i++)
    {
        for (j = 0; j < AE_WEIGHT_COLUMN; j++)
        {
            g_stSceneautoState.pstPreviousIspParam[s32Index].stExposure.au8Weight[i][j] = stIspExposureAttr.stAuto.au8Weight[i][j];
        }
    }
    s32Ret = HI_MPI_ISP_GetAERouteAttr(IspDev, &stIspAeRoute);
    if (HI_SUCCESS != s32Ret)
    {
        printf("HI_MPI_ISP_GetAERouteAttr failed\n");
        return HI_FAILURE;
    }
    g_stSceneautoState.pstPreviousIspParam[s32Index].stExposure.s32TotalNum = stIspAeRoute.u32TotalNum;
    for (i = 0; i < stIspAeRoute.u32TotalNum; i++)
    {
        g_stSceneautoState.pstPreviousIspParam[s32Index].stExposure.astRouteNode[i].u32IntTime = stIspAeRoute.astRouteNode[i].u32IntTime;
        g_stSceneautoState.pstPreviousIspParam[s32Index].stExposure.astRouteNode[i].u32SysGain = stIspAeRoute.astRouteNode[i].u32SysGain;
    }

    //saturation
    s32Ret = HI_MPI_ISP_GetSaturationAttr(IspDev, &stIspSaturationAttr);
    if (HI_SUCCESS != s32Ret)
    {
        printf("HI_MPI_ISP_GetSaturationAttr failed\n");
        return HI_FAILURE;
    }
    g_stSceneautoState.pstPreviousIspParam[s32Index].stSaturation.u8OpType = stIspSaturationAttr.enOpType;
    for (i = 0; i < 16; i++)
    {
        g_stSceneautoState.pstPreviousIspParam[s32Index].stSaturation.u8Saturation[i] = stIspSaturationAttr.stAuto.au8Sat[i];
    }

    //DRC
    s32Ret = HI_MPI_ISP_GetDRCAttr(IspDev, &stIspDrcAttr);
    if (HI_SUCCESS != s32Ret)
    {
        printf("HI_MPI_ISP_GetDRCAttr failed\n");
        return HI_FAILURE;
    }
    g_stSceneautoState.pstPreviousIspParam[s32Index].stDrc.bEnable = stIspDrcAttr.bEnable;
    g_stSceneautoState.pstPreviousIspParam[s32Index].stDrc.u8OpType = stIspDrcAttr.enOpType;
    g_stSceneautoState.pstPreviousIspParam[s32Index].stDrc.u32Strength = stIspDrcAttr.stManual.u32Strength;
    g_stSceneautoState.pstPreviousIspParam[s32Index].stDrc.u32DRCAutoStrength = stIspDrcAttr.stAuto.u32Strength;
    g_stSceneautoState.pstPreviousIspParam[s32Index].stDrc.u32WhiteLevel = stIspDrcAttr.u32WhiteLevel;
    g_stSceneautoState.pstPreviousIspParam[s32Index].stDrc.u32BlackLevel = stIspDrcAttr.u32BlackLevel;
    g_stSceneautoState.pstPreviousIspParam[s32Index].stDrc.u32Asymmetry = stIspDrcAttr.u32Asymmetry;
    g_stSceneautoState.pstPreviousIspParam[s32Index].stDrc.u32BrightEnhance = stIspDrcAttr.u32BrightEnhance;
    g_stSceneautoState.pstPreviousIspParam[s32Index].stDrc.u8FilterMux = stIspDrcAttr.FilterMux;
    g_stSceneautoState.pstPreviousIspParam[s32Index].stDrc.u32SlopeMax = stIspDrcAttr.stDrcCurve.u32SlopeMax;
    g_stSceneautoState.pstPreviousIspParam[s32Index].stDrc.u32SlopeMin = stIspDrcAttr.stDrcCurve.u32SlopeMin;
    g_stSceneautoState.pstPreviousIspParam[s32Index].stDrc.u32VarianceSpace = stIspDrcAttr.stDrcCurve.u32VarianceSpace;
    g_stSceneautoState.pstPreviousIspParam[s32Index].stDrc.u32VarianceIntensity = stIspDrcAttr.stDrcCurve.u32VarianceIntensity;
    g_stSceneautoState.pstPreviousIspParam[s32Index].stDrc.u32Contrast = stIspDrcAttr.stDrcCurveEx.u32Contrast;
    g_stSceneautoState.pstPreviousIspParam[s32Index].stDrc.u32BrightPr = stIspDrcAttr.stDrcCurveEx.u32BrightPr;
    g_stSceneautoState.pstPreviousIspParam[s32Index].stDrc.u32Svariance = stIspDrcAttr.stDrcCurveEx.u32Svariance;
    g_stSceneautoState.pstPreviousIspParam[s32Index].stDrc.u32DarkEnhance = stIspDrcAttr.stDrcCurveEx.u32DarkEnhance;

    //WDR Exposure attr
    s32Ret = HI_MPI_ISP_GetWDRExposureAttr(IspDev, &stIspWdrExposureAttr);
    if (HI_SUCCESS != s32Ret)
    {
        printf("HI_MPI_ISP_GetWDRExposureAttr failed\n");
        return HI_FAILURE;
    }
    g_stSceneautoState.pstPreviousIspParam[s32Index].stWdrExposureAttr.u8ExpRatioType = stIspWdrExposureAttr.enExpRatioType; 
	g_stSceneautoState.pstPreviousIspParam[s32Index].stWdrExposureAttr.u32ExpRatioMax = stIspWdrExposureAttr.u32ExpRatioMax; 
	g_stSceneautoState.pstPreviousIspParam[s32Index].stWdrExposureAttr.u32ExpRatioMin = stIspWdrExposureAttr.u32ExpRatioMin; 
	for (i = 0; i < 3; i++)
	{
		g_stSceneautoState.pstPreviousIspParam[s32Index].stWdrExposureAttr.au32ExpRatio[i] = stIspWdrExposureAttr.au32ExpRatio[i];
	}

	//DP
	pstIspDpAttr = (ISP_DP_ATTR_S *)malloc(sizeof(ISP_DP_ATTR_S));
    if(HI_NULL == pstIspDpAttr)
    {
        printf("Malloc ISP_DP_ATTR_S failed\n");
        return HI_FAILURE;
    }
	s32Ret = HI_MPI_ISP_GetDPAttr(IspDev, pstIspDpAttr);
    if (HI_SUCCESS != s32Ret)
    {
        printf("HI_MPI_ISP_GetDPAttr failed\n");
        FREE_PTR(pstIspDpAttr);
        return HI_FAILURE;
    }
    g_stSceneautoState.pstPreviousIspParam[s32Index].stDp.u16Slope = pstIspDpAttr->stDynamicAttr.u16Slope;
    g_stSceneautoState.pstPreviousIspParam[s32Index].stDp.u16Thresh = pstIspDpAttr->stDynamicAttr.u16Thresh;

	FREE_PTR(pstIspDpAttr);

	return HI_SUCCESS;
}

HI_S32 HI_SCENEAUTO_SetSpecialMode(const SCENEAUTO_SEPCIAL_SCENE_E* peSpecialScene)
{
	CHECK_SCENEAUTO_INIT();
	CHEKC_SCENEAUTO_START();
    CHECK_NULL_PTR(peSpecialScene);
    
    HI_S32 s32Ret = HI_SUCCESS;
    HI_S32 s32Index;
    
    if ((*peSpecialScene != SCENEAUTO_SPECIAL_SCENE_IR) &&
        (*peSpecialScene != SCENEAUTO_SPECIAL_SCENE_HLC) &&
        (*peSpecialScene != SCENEAUTO_SPECIAL_SCENE_NONE) &&
        (*peSpecialScene != SCENEAUTO_SPECIAL_SCENE_AUTO_FSWDR) && 
        (*peSpecialScene != SCENEAUTO_SPECIAL_SCENE_MANUAL_LONG_FRAME) && 
        (*peSpecialScene != SCENEAUTO_SPECIAL_SCENE_MANUAL_NORMAL_WDR) && 
        (*peSpecialScene != SCENEAUTO_SPECIAL_SCENE_TRAFFIC) &&
        (*peSpecialScene != SCENEAUTO_SPECIAL_SCENE_FACE))
    {
        printf("error input! unknown special scene!\n");
        return HI_FAILURE;
    }

	MUTEX_LOCK(g_stSceneautoLock);
    SCENEAUTO_SEPCIAL_SCENE_E eSpecialScene = *peSpecialScene;


    if (g_eSpecialScene != eSpecialScene)
    {		
        g_eSpecialScene = eSpecialScene;
        switch (eSpecialScene)
        {
            case SCENEAUTO_SPECIAL_SCENE_IR:
                s32Ret = Sceneauto_SetIR();
                if (HI_SUCCESS != s32Ret)
                {
                    printf("Sceneauto_SetIR failed\n");
                   MUTEX_UNLOCK(g_stSceneautoLock);
                    return HI_FAILURE;
                }
                break;
            case SCENEAUTO_SPECIAL_SCENE_HLC:
                hi_usleep(1000000);
                s32Ret = Sceneauto_SetHLC();
                if (HI_SUCCESS != s32Ret)
                {
                    printf("Sceneauto_SetHLC failed\n");
                    MUTEX_UNLOCK(g_stSceneautoLock);
                    return HI_FAILURE;
                }
                break;            
            case SCENEAUTO_SPECIAL_SCENE_TRAFFIC:
                hi_usleep(1000000);
                s32Ret = Sceneauto_SetTrafficMode();
                if (HI_SUCCESS != s32Ret)
                {
                    printf("Sceneauto_SetTrafficMode failed\n");
                   MUTEX_UNLOCK(g_stSceneautoLock);
                    return HI_FAILURE;
                }
                break;
            case SCENEAUTO_SPECIAL_SCENE_FACE:
                hi_usleep(1000000);
                s32Ret = Sceneauto_SetFaceMode();
                if (HI_SUCCESS != s32Ret)
                {
                    printf("Sceneauto_SetFaceMode failed\n");
                   MUTEX_UNLOCK(g_stSceneautoLock);
                    return HI_FAILURE;
                }
                break;
            case SCENEAUTO_SPECIAL_SCENE_MANUAL_LONG_FRAME:
            case SCENEAUTO_SPECIAL_SCENE_AUTO_FSWDR:
            case SCENEAUTO_SPECIAL_SCENE_MANUAL_NORMAL_WDR: 
                g_eFSWDRMode = SCENEAUTO_FSWDR_NORMAL_MODE;                
                break;
            case SCENEAUTO_SPECIAL_SCENE_NONE:
            	for (s32Index = 0; s32Index < g_stSceneautoParam.stCommParam.s32IspDevCount; s32Index++)
				{
					s32Ret = Sceneauto_SetIspPreviousParam(s32Index);
					if (HI_SUCCESS != s32Ret)
		            {
		               MUTEX_UNLOCK(g_stSceneautoLock);
		                printf("Sceneauto_SetIspPreviousParam failed\n");
		                return HI_FAILURE;
		            }

		            s32Ret = Sceneauto_SetIspDefaultParam(s32Index);
		            if (HI_SUCCESS != s32Ret)
		            {
		               
						MUTEX_UNLOCK(g_stSceneautoLock);
		                printf("Sceneauto_SetIspDefaultParam failed\n");
		                return HI_FAILURE;
		            }
				}
  				for (s32Index = 0; s32Index < g_stSceneautoParam.stCommParam.s32ViDevCount; s32Index++)
  				{
  					s32Ret = Sceneauto_SetViPreviousParam(s32Index);
					if (HI_SUCCESS != s32Ret)
		            {
		                MUTEX_UNLOCK(g_stSceneautoLock);
		                printf("Sceneauto_SetViPreviousParam failed\n");
		                return HI_FAILURE;
		            }

		            s32Ret = Sceneauto_SetViDefaultParam(s32Index);
		            if (HI_SUCCESS != s32Ret)
		            {
		               MUTEX_UNLOCK(g_stSceneautoLock);
		                printf("Sceneauto_SetViDefaultParam failed\n");
		                return HI_FAILURE;
		            }
  				}               
                break;              
            default:
                printf("unknown choice\n");
                break;
        }

    }
    MUTEX_UNLOCK(g_stSceneautoLock);

    
    return HI_SUCCESS;
}

HI_S32 HI_SCENEAUTO_GetSpecialMode(SCENEAUTO_SEPCIAL_SCENE_E* peSpecialScene)
{
	CHECK_SCENEAUTO_INIT();
	CHEKC_SCENEAUTO_START();
    CHECK_NULL_PTR(peSpecialScene);

	MUTEX_LOCK(g_stSceneautoLock);
    *peSpecialScene = g_eSpecialScene;
	MUTEX_UNLOCK(g_stSceneautoLock);

    return HI_SUCCESS;
}

HI_S32 HI_SCENEAUTO_Stop()
{
	CHECK_SCENEAUTO_INIT();
	
   
	MUTEX_LOCK(g_stSceneautoLock);
    if (HI_FALSE == g_stSceneautoState.bSceneautoStart)
    {
        printf("SCENEAUTO Module has been stopped already!\n");
		MUTEX_UNLOCK(g_stSceneautoLock);
        return HI_SUCCESS;
    }

    g_stSceneautoState.stThreadNormal.bThreadFlag = HI_FALSE;
    g_stSceneautoState.stThreadSpecial.bThreadFlag = HI_FALSE;
    g_stSceneautoState.stThread3Dnr.bThreadFlag = HI_FALSE;    
   	g_stSceneautoState.stThreadDrc.bThreadFlag = HI_FALSE;

    (void)pthread_join(g_stSceneautoState.stThreadNormal.pThread, NULL);
    (void)pthread_join(g_stSceneautoState.stThreadSpecial.pThread, NULL);
    (void)pthread_join(g_stSceneautoState.stThread3Dnr.pThread, NULL);
    (void)pthread_join(g_stSceneautoState.stThreadDrc.pThread, NULL);

    g_stSceneautoState.bSceneautoStart = HI_FALSE;
   
    MUTEX_UNLOCK(g_stSceneautoLock);
    printf("SCENEAUTO Module has been stopped successfully!\n");

    return HI_SUCCESS;
}

HI_S32 HI_SCENEAUTO_Start()
{
	CHECK_SCENEAUTO_INIT();
	
    HI_S32 s32Ret = HI_SUCCESS;
    HI_S32 s32Index;
    
	MUTEX_LOCK(g_stSceneautoLock);
    if (HI_TRUE == g_stSceneautoState.bSceneautoStart)
    {
        printf("SCENEAUTO Module is start already!\n");
        MUTEX_UNLOCK(g_stSceneautoLock);
        return HI_SUCCESS;
    }

	for (s32Index = 0; s32Index < g_stSceneautoParam.stCommParam.s32IspDevCount; s32Index++)
	{
		s32Ret = Sceneauto_GetIspPreviousParam(s32Index);
		if (HI_SUCCESS != s32Ret)
	    {
	        printf("Sceneauto_GetIspPreviousParam failed\n");
	        MUTEX_UNLOCK(g_stSceneautoLock);
	        return HI_FAILURE;
	    } 
		
		s32Ret = Sceneauto_SetIspDefaultParam(s32Index);
		if (HI_SUCCESS != s32Ret)
	    {
	        printf("Sceneauto_SetIspDefaultParam failed\n");
	        MUTEX_UNLOCK(g_stSceneautoLock);
	        return HI_FAILURE;
	    } 
	}

	for (s32Index = 0; s32Index < g_stSceneautoParam.stCommParam.s32ViDevCount; s32Index++)
	{
		s32Ret = Sceneauto_GetViPreviousParam(s32Index);
		if (HI_SUCCESS != s32Ret)
	    {
	        printf("Sceneauto_GetViPreviousParam failed\n");
	        MUTEX_UNLOCK(g_stSceneautoLock);
	        return HI_FAILURE;
	    } 
		
		s32Ret = Sceneauto_SetViDefaultParam(s32Index);
		if (HI_SUCCESS != s32Ret)
	    {
	        printf("Sceneauto_SetViDefaultParam failed\n");
	        MUTEX_UNLOCK(g_stSceneautoLock);
	        return HI_FAILURE;
	    } 
	}

   	g_stSceneautoState.stThreadNormal.bThreadFlag = HI_TRUE;
   	g_stSceneautoState.stThreadSpecial.bThreadFlag = HI_TRUE;
    g_stSceneautoState.stThread3Dnr.bThreadFlag = HI_TRUE;
   	g_stSceneautoState.stThreadDrc.bThreadFlag = HI_TRUE;

    s32Ret = pthread_create(&g_stSceneautoState.stThreadNormal.pThread, NULL, SceneAuto_NormalThread, NULL);
    if (HI_SUCCESS != s32Ret)
    {
        printf("pthread_create SceneAuto_NormalThread failed \n");
        MUTEX_UNLOCK(g_stSceneautoLock);
        return HI_FAILURE;
    }

    s32Ret = pthread_create(&g_stSceneautoState.stThreadSpecial.pThread, NULL, SceneAuto_SpecialThread, NULL);
    if (HI_SUCCESS != s32Ret)
    {
        printf("pthread_create SceneAuto_SpecialThread failed \n");
        MUTEX_UNLOCK(g_stSceneautoLock);
        return HI_FAILURE;
    }

    s32Ret = pthread_create(&g_stSceneautoState.stThread3Dnr.pThread, NULL, SceneAuto_3DnrThread, NULL);
    if (HI_SUCCESS != s32Ret)
    {
        printf("pthread_create SceneAuto_3DnrThread failed \n");
        MUTEX_UNLOCK(g_stSceneautoLock);
        return HI_FAILURE;
    }    

    s32Ret = pthread_create(&g_stSceneautoState.stThreadDrc.pThread, NULL, SceneAuto_DrcThread, NULL);
    if (HI_SUCCESS != s32Ret)
    {
        printf("pthread_create SceneAuto_DrcThread failed \n");
        MUTEX_UNLOCK(g_stSceneautoLock);
        return HI_FAILURE;
    }

    g_stSceneautoState.bSceneautoStart = HI_TRUE;
  
	MUTEX_UNLOCK(g_stSceneautoLock);

    printf("SCENEAUTO Module has been started successfully!\n");

    return HI_SUCCESS;
}


HI_S32 HI_SCENEAUTO_DeInit()
{
    MUTEX_LOCK(g_stSceneautoLock);
    if (HI_FALSE == g_stSceneautoState.bSceneautoInit)
    {
        printf("SCENEAUTO Module has not been inited !\n");
        MUTEX_UNLOCK(g_stSceneautoLock);
        return HI_SUCCESS;
    }

    if (HI_TRUE == g_stSceneautoState.stThreadNormal.bThreadFlag)
    {
        g_stSceneautoState.stThreadNormal.bThreadFlag = HI_FALSE;
        (void)pthread_join(g_stSceneautoState.stThreadNormal.pThread, NULL);
    }
    if (HI_TRUE == g_stSceneautoState.stThreadSpecial.bThreadFlag)
    {
        g_stSceneautoState.stThreadSpecial.bThreadFlag = HI_FALSE;
        (void)pthread_join(g_stSceneautoState.stThreadSpecial.pThread, NULL);
    }
    if (HI_TRUE == g_stSceneautoState.stThread3Dnr.bThreadFlag)
    {
        g_stSceneautoState.stThread3Dnr.bThreadFlag = HI_FALSE;
        (void)pthread_join(g_stSceneautoState.stThread3Dnr.pThread, NULL);
    }    
    if (HI_TRUE == g_stSceneautoState.stThreadDrc.bThreadFlag)
    {
        g_stSceneautoState.stThreadDrc.bThreadFlag = HI_FALSE;
        (void)pthread_join(g_stSceneautoState.stThreadDrc.pThread, NULL);
    }

	FREE_PTR(g_stSceneautoState.pstPrevisouViParam);
	FREE_PTR(g_stSceneautoState.pstPreviousIspParam);   
    Sceneauto_FreeDict();
    Sceneauto_FreeMem();
    g_stSceneautoState.bSceneautoInit = HI_FALSE;
    MUTEX_UNLOCK(g_stSceneautoLock);

    printf("SCENEAUTO Module has been deinited successfully!\n");

    return HI_SUCCESS;
}

HI_VOID PrintParam()
{
	HI_S32 i, j, k;
	
	printf("\n[common]\n");
	printf("IspDevCount = %d\n", g_stSceneautoParam.stCommParam.s32IspDevCount);
	printf("ViDevCount = %d\n", g_stSceneautoParam.stCommParam.s32ViDevCount);
	printf("VpssGrpCount = %d\n", g_stSceneautoParam.stCommParam.s32VpssGrpCount);
	printf("\n");
	printf("ave_lum_thresh = %d\n", g_stSceneautoParam.stCommParam.u32AveLumThresh);
	printf("delta_dis_expthresh = %d\n", g_stSceneautoParam.stCommParam.u32DeltaDisExpThresh);
	printf("u32DRCStrengthThresh = %d\n", g_stSceneautoParam.stCommParam.u32DRCStrengthThresh);
	printf("\n");
	for (i = 0; i < g_stSceneautoParam.stCommParam.s32IspDevCount; i++)
	{
		printf("[ISP_PARAM_%d]\n", i);
		printf("ISP_DEV = %d\n", g_stSceneautoParam.pstIspParam[i].IspDev);
		printf("aeRunInterval = %d\n", g_stSceneautoParam.pstIspParam[i].stAeParam.u8AERunInterval);
		printf("u32TotalNum = %d\n", g_stSceneautoParam.pstIspParam[i].stAeParam.stAeRoute.u32TotalNum);
		printf("u32IntTime = ");
		for (j = 0; j < g_stSceneautoParam.pstIspParam[i].stAeParam.stAeRoute.u32TotalNum; j++)
		{
			printf("%d|", g_stSceneautoParam.pstIspParam[i].stAeParam.stAeRoute.pstRuteNode[j].u32IntTime);
		}
		printf("\n");
		printf("u32SysGain = ");
		for (j = 0; j < g_stSceneautoParam.pstIspParam[i].stAeParam.stAeRoute.u32TotalNum; j++)
		{
			printf("%d|", g_stSceneautoParam.pstIspParam[i].stAeParam.stAeRoute.pstRuteNode[j].u32SysGain);
		}
		printf("\n");
		printf("aeBitrateCount = %d\n", g_stSceneautoParam.pstIspParam[i].stAeParam.u32BitrateCount);
		printf("aeBitrateThresh = ");
		for (j = 0; j < g_stSceneautoParam.pstIspParam[i].stAeParam.u32BitrateCount; j++)
		{
			printf("%d|", g_stSceneautoParam.pstIspParam[i].stAeParam.pu32BitrateThresh[j]);
		}
		printf("\n");
		printf("u8Speed = ");
		for (j = 0; j < g_stSceneautoParam.pstIspParam[i].stAeParam.u32BitrateCount; j++)
		{
			printf("%d|", g_stSceneautoParam.pstIspParam[i].stAeParam.pstAERelatedBit[j].u8Speed);
		}
		printf("\n");
		printf("u8Tolerance = ");
		for (j = 0; j < g_stSceneautoParam.pstIspParam[i].stAeParam.u32BitrateCount; j++)
		{
			printf("%d|", g_stSceneautoParam.pstIspParam[i].stAeParam.pstAERelatedBit[j].u8Tolerance);
		}
		printf("\n");
		printf("u16BlackDelayFrame = ");
		for (j = 0; j < g_stSceneautoParam.pstIspParam[i].stAeParam.u32BitrateCount; j++)
		{
			printf("%d|", g_stSceneautoParam.pstIspParam[i].stAeParam.pstAERelatedBit[j].u16BlackDelayFrame);
		}
		printf("\n");
		printf("u16WhiteDelayFrame = ");
		for (j = 0; j < g_stSceneautoParam.pstIspParam[i].stAeParam.u32BitrateCount; j++)
		{
			printf("%d|", g_stSceneautoParam.pstIspParam[i].stAeParam.pstAERelatedBit[j].u16WhiteDelayFrame);
		}
		printf("\n");
		printf("u32SysGainMax = ");
		for (j = 0; j < g_stSceneautoParam.pstIspParam[i].stAeParam.u32BitrateCount; j++)
		{
			printf("%d|", g_stSceneautoParam.pstIspParam[i].stAeParam.pstAERelatedBit[j].u32SysGainMax);
		}
		printf("\n");
		printf("aeExpCount = %d\n", g_stSceneautoParam.pstIspParam[i].stAeParam.u32ExpCount);
		printf("aeExpLtoHThresh = ");
		for (j = 0; j < g_stSceneautoParam.pstIspParam[i].stAeParam.u32ExpCount; j++)
		{
			printf("%d|", g_stSceneautoParam.pstIspParam[i].stAeParam.pu32AEExpLtoHThresh[j]);
		}
		printf("\n");
		printf("aeExpHtoLThresh = ");
		for (j = 0; j < g_stSceneautoParam.pstIspParam[i].stAeParam.u32ExpCount; j++)
		{
			printf("%d|", g_stSceneautoParam.pstIspParam[i].stAeParam.pu32AEExpHtoLThresh[j]);
		}
		printf("\n");
		printf("aeCompesation = ");
		for (j = 0; j < g_stSceneautoParam.pstIspParam[i].stAeParam.u32ExpCount; j++)
		{
			printf("%d|", g_stSceneautoParam.pstIspParam[i].stAeParam.pstAERelatedExp[j].u8AECompesation);
		}
		printf("\n");
		printf("aeHistOffset = ");
		for (j = 0; j < g_stSceneautoParam.pstIspParam[i].stAeParam.u32ExpCount; j++)
		{
			printf("%d|", g_stSceneautoParam.pstIspParam[i].stAeParam.pstAERelatedExp[j].u8AEHistOffset);
		}
		printf("\n");
		printf("drCount = %d\n", g_stSceneautoParam.pstIspParam[i].stDrcParam.u32DRCount);
		printf("drThresh = ");
		for (j = 0; j < g_stSceneautoParam.pstIspParam[i].stDrcParam.u32DRCount; j++)
		{
			printf("%d|", g_stSceneautoParam.pstIspParam[i].stDrcParam.pu32DRThresh[j]);
		}
		printf("\n");
		printf("bEnable = ");
		for (j = 0; j < g_stSceneautoParam.pstIspParam[i].stDrcParam.u32DRCount; j++)
		{
			printf("%d|", g_stSceneautoParam.pstIspParam[i].stDrcParam.pstDRCAttr[j].bEnable);
		}
		printf("\n");
		printf("u8OpType = ");
		for (j = 0; j < g_stSceneautoParam.pstIspParam[i].stDrcParam.u32DRCount; j++)
		{
			printf("%d|", g_stSceneautoParam.pstIspParam[i].stDrcParam.pstDRCAttr[j].u8OpType);
		}
		printf("\n");
		printf("u32Strength = ");
		for (j = 0; j < g_stSceneautoParam.pstIspParam[i].stDrcParam.u32DRCount; j++)
		{
			printf("%d|", g_stSceneautoParam.pstIspParam[i].stDrcParam.pstDRCAttr[j].u32Strength);
		}
		printf("\n");
		printf("u32DRCAutoStrength = ");
		for (j = 0; j < g_stSceneautoParam.pstIspParam[i].stDrcParam.u32DRCount; j++)
		{
			printf("%d|", g_stSceneautoParam.pstIspParam[i].stDrcParam.pstDRCAttr[j].u32DRCAutoStrength);
		}
		printf("\n");
		printf("u32WhiteLevel = ");
		for (j = 0; j < g_stSceneautoParam.pstIspParam[i].stDrcParam.u32DRCount; j++)
		{
			printf("%d|", g_stSceneautoParam.pstIspParam[i].stDrcParam.pstDRCAttr[j].u32WhiteLevel);
		}
		printf("\n");
		printf("u32BlackLevel = ");
		for (j = 0; j < g_stSceneautoParam.pstIspParam[i].stDrcParam.u32DRCount; j++)
		{
			printf("%d|", g_stSceneautoParam.pstIspParam[i].stDrcParam.pstDRCAttr[j].u32BlackLevel);
		}
		printf("\n");
		printf("u32Asymmetry = ");
		for (j = 0; j < g_stSceneautoParam.pstIspParam[i].stDrcParam.u32DRCount; j++)
		{
			printf("%d|", g_stSceneautoParam.pstIspParam[i].stDrcParam.pstDRCAttr[j].u32Asymmetry);
		}
		printf("\n");
		printf("u32BrightEnhance = ");
		for (j = 0; j < g_stSceneautoParam.pstIspParam[i].stDrcParam.u32DRCount; j++)
		{
			printf("%d|", g_stSceneautoParam.pstIspParam[i].stDrcParam.pstDRCAttr[j].u32BrightEnhance);
		}
		printf("\n");
		printf("u8FilterMux = ");
		for (j = 0; j < g_stSceneautoParam.pstIspParam[i].stDrcParam.u32DRCount; j++)
		{
			printf("%d|", g_stSceneautoParam.pstIspParam[i].stDrcParam.pstDRCAttr[j].u8FilterMux);
		}
		printf("\n");
		printf("u32SlopeMax = ");
		for (j = 0; j < g_stSceneautoParam.pstIspParam[i].stDrcParam.u32DRCount; j++)
		{
			printf("%d|", g_stSceneautoParam.pstIspParam[i].stDrcParam.pstDRCAttr[j].u32SlopeMax);
		}
		printf("\n");
		printf("u32SlopeMin = ");
		for (j = 0; j < g_stSceneautoParam.pstIspParam[i].stDrcParam.u32DRCount; j++)
		{
			printf("%d|", g_stSceneautoParam.pstIspParam[i].stDrcParam.pstDRCAttr[j].u32SlopeMin);
		}
		printf("\n");
		printf("u32VarianceSpace = ");
		for (j = 0; j < g_stSceneautoParam.pstIspParam[i].stDrcParam.u32DRCount; j++)
		{
			printf("%d|", g_stSceneautoParam.pstIspParam[i].stDrcParam.pstDRCAttr[j].u32VarianceSpace);
		}
		printf("\n");
		printf("u32VarianceIntensity = ");
		for (j = 0; j < g_stSceneautoParam.pstIspParam[i].stDrcParam.u32DRCount; j++)
		{
			printf("%d|", g_stSceneautoParam.pstIspParam[i].stDrcParam.pstDRCAttr[j].u32VarianceIntensity);
		}
		printf("\n");
		printf("u32Contrast = ");
		for (j = 0; j < g_stSceneautoParam.pstIspParam[i].stDrcParam.u32DRCount; j++)
		{
			printf("%d|", g_stSceneautoParam.pstIspParam[i].stDrcParam.pstDRCAttr[j].u32Contrast);
		}
		printf("\n");
		printf("u32BrightPr = ");
		for (j = 0; j < g_stSceneautoParam.pstIspParam[i].stDrcParam.u32DRCount; j++)
		{
			printf("%d|", g_stSceneautoParam.pstIspParam[i].stDrcParam.pstDRCAttr[j].u32BrightPr);
		}
		printf("\n");
		printf("u32Svariance = ");
		for (j = 0; j < g_stSceneautoParam.pstIspParam[i].stDrcParam.u32DRCount; j++)
		{
			printf("%d|", g_stSceneautoParam.pstIspParam[i].stDrcParam.pstDRCAttr[j].u32Svariance);
		}
		printf("\n");
		printf("u32DarkEnhance = ");
		for (j = 0; j < g_stSceneautoParam.pstIspParam[i].stDrcParam.u32DRCount; j++)
		{
			printf("%d|", g_stSceneautoParam.pstIspParam[i].stDrcParam.pstDRCAttr[j].u32DarkEnhance);
		}
		printf("\n");
		printf("demosaicExpCount = %d\n", g_stSceneautoParam.pstIspParam[i].stDemosaicParam.u32ExpCount);
		printf("demosaicExpThresh = ");
		for (j = 0; j < g_stSceneautoParam.pstIspParam[i].stDemosaicParam.u32ExpCount; j++)
		{
			printf("%d|", g_stSceneautoParam.pstIspParam[i].stDemosaicParam.pu32ExpThresh[j]);
		}
		printf("\n");
		printf("u8UuSlpoe = ");
		for (j = 0; j < g_stSceneautoParam.pstIspParam[i].stDemosaicParam.u32ExpCount; j++)
		{
			printf("%d|", g_stSceneautoParam.pstIspParam[i].stDemosaicParam.pstDemosaic[j].u8UuSlope);
		}
		printf("\n");
		printf("u8AaSlope = ");
		for (j = 0; j < g_stSceneautoParam.pstIspParam[i].stDemosaicParam.u32ExpCount; j++)
		{
			printf("%d|", g_stSceneautoParam.pstIspParam[i].stDemosaicParam.pstDemosaic[j].u8AaSlope);
		}
		printf("\n");
		printf("u8VaSlope = ");
		for (j = 0; j < g_stSceneautoParam.pstIspParam[i].stDemosaicParam.u32ExpCount; j++)
		{
			printf("%d|", g_stSceneautoParam.pstIspParam[i].stDemosaicParam.pstDemosaic[j].u8VaSlope);
		}
		printf("\n");
		printf("u8VhSlope = ");
		for (j = 0; j < g_stSceneautoParam.pstIspParam[i].stDemosaicParam.u32ExpCount; j++)
		{
			printf("%d|", g_stSceneautoParam.pstIspParam[i].stDemosaicParam.pstDemosaic[j].u8VhSlope);
		}
		printf("\n");
		printf("defSharpenD = ");
		for (j = 0; j < EXPOSURE_LEVEL; j++)
		{
			printf("%d|", g_stSceneautoParam.pstIspParam[i].stSharpenParam.astDefaultSharpen[j].u8SharpenD);
		}
		printf("\n");
		printf("defSharpenUd = ");
		for (j = 0; j < EXPOSURE_LEVEL; j++)
		{
			printf("%d|", g_stSceneautoParam.pstIspParam[i].stSharpenParam.astDefaultSharpen[j].u8SharpenUd);
		}
		printf("\n");
		printf("defTextureSt = ");
		for (j = 0; j < EXPOSURE_LEVEL; j++)
		{
			printf("%d|", g_stSceneautoParam.pstIspParam[i].stSharpenParam.astDefaultSharpen[j].u8TextureSt);
		}
		printf("\n");
		printf("defEdgeSt = ");
		for (j = 0; j < EXPOSURE_LEVEL; j++)
		{
			printf("%d|", g_stSceneautoParam.pstIspParam[i].stSharpenParam.astDefaultSharpen[j].u8EdgeSt);
		}
		printf("\n");
		printf("defOverShoot = ");
		for (j = 0; j < EXPOSURE_LEVEL; j++)
		{
			printf("%d|", g_stSceneautoParam.pstIspParam[i].stSharpenParam.astDefaultSharpen[j].u8OverShoot);
		}
		printf("\n");
		printf("defUnderShoot = ");
		for (j = 0; j < EXPOSURE_LEVEL; j++)
		{
			printf("%d|", g_stSceneautoParam.pstIspParam[i].stSharpenParam.astDefaultSharpen[j].u8UnderShoot);
		}
		printf("\n");
		printf("defDetailCtrl = ");
		for (j = 0; j < EXPOSURE_LEVEL; j++)
		{
			printf("%d|", g_stSceneautoParam.pstIspParam[i].stSharpenParam.astDefaultSharpen[j].u8DetailCtrl);
		}
		printf("\n");
		printf("sharpenExpCount = %d\n", g_stSceneautoParam.pstIspParam[i].stSharpenParam.u32ExpCount);
		printf("sharpenExpThresh = ");
		for (j = 0; j < g_stSceneautoParam.pstIspParam[i].stSharpenParam.u32ExpCount; j++)
		{
			printf("%d|", g_stSceneautoParam.pstIspParam[i].stSharpenParam.pu32ExpThresh[j]);
		}
		printf("\n");
		printf("dynSharpenD = ");
		for (j = 0; j < g_stSceneautoParam.pstIspParam[i].stSharpenParam.u32ExpCount; j++)
		{
			printf("%d|", g_stSceneautoParam.pstIspParam[i].stSharpenParam.pstDynamicSharpen[j].u8SharpenD);
		}
		printf("\n");
		printf("dynSharpenUd = ");
		for (j = 0; j < g_stSceneautoParam.pstIspParam[i].stSharpenParam.u32ExpCount; j++)
		{
			printf("%d|", g_stSceneautoParam.pstIspParam[i].stSharpenParam.pstDynamicSharpen[j].u8SharpenUd);
		}
		printf("\n");
		printf("dynTextureSt = ");
		for (j = 0; j < g_stSceneautoParam.pstIspParam[i].stSharpenParam.u32ExpCount; j++)
		{
			printf("%d|", g_stSceneautoParam.pstIspParam[i].stSharpenParam.pstDynamicSharpen[j].u8TextureSt);
		}
		printf("\n");
		printf("dynEdgeSt = ");
		for (j = 0; j < g_stSceneautoParam.pstIspParam[i].stSharpenParam.u32ExpCount; j++)
		{
			printf("%d|", g_stSceneautoParam.pstIspParam[i].stSharpenParam.pstDynamicSharpen[j].u8EdgeSt);
		}
		printf("\n");
		printf("dynOverShoot = ");
		for (j = 0; j < g_stSceneautoParam.pstIspParam[i].stSharpenParam.u32ExpCount; j++)
		{
			printf("%d|", g_stSceneautoParam.pstIspParam[i].stSharpenParam.pstDynamicSharpen[j].u8OverShoot);
		}
		printf("\n");
		printf("dynUnderShoot = ");
		for (j = 0; j < g_stSceneautoParam.pstIspParam[i].stSharpenParam.u32ExpCount; j++)
		{
			printf("%d|", g_stSceneautoParam.pstIspParam[i].stSharpenParam.pstDynamicSharpen[j].u8UnderShoot);
		}
		printf("\n");
		printf("dynDetailCtrl = ");
		for (j = 0; j < g_stSceneautoParam.pstIspParam[i].stSharpenParam.u32ExpCount; j++)
		{
			printf("%d|", g_stSceneautoParam.pstIspParam[i].stSharpenParam.pstDynamicSharpen[j].u8DetailCtrl);
		}
		printf("\n");
		printf("dpExpCount = %d\n", g_stSceneautoParam.pstIspParam[i].stDpParam.u32ExpCount);
		printf("dpExpThresh = ");
		for (j = 0; j < g_stSceneautoParam.pstIspParam[i].stDpParam.u32ExpCount; j++)
		{
			printf("%d|", g_stSceneautoParam.pstIspParam[i].stDpParam.pu32ExpThresh[j]);
		}
		printf("\n");
		printf("u16Slop = ");
		for (j = 0; j < g_stSceneautoParam.pstIspParam[i].stDpParam.u32ExpCount; j++)
		{
			printf("%d|", g_stSceneautoParam.pstIspParam[i].stDpParam.pstDPAttr[j].u16Slope);
		}
		printf("\n");
		printf("u16Thresh = ");
		for (j = 0; j < g_stSceneautoParam.pstIspParam[i].stDpParam.u32ExpCount; j++)
		{
			printf("%d|", g_stSceneautoParam.pstIspParam[i].stDpParam.pstDPAttr[j].u16Thresh);
		}
		printf("\n");
		printf("falsecolorExpCount = %d\n", g_stSceneautoParam.pstIspParam[i].stFalseColorParam.u32ExpCount);
		printf("falsecolorExpThresh = ");
		for (j = 0; j < g_stSceneautoParam.pstIspParam[i].stFalseColorParam.u32ExpCount; j++)
		{
			printf("%d|", g_stSceneautoParam.pstIspParam[i].stFalseColorParam.pu32ExpThresh[j]);
		}
		printf("\n");
		printf("u8Strength = ");
		for (j = 0; j < g_stSceneautoParam.pstIspParam[i].stFalseColorParam.u32ExpCount; j++)
		{
			printf("%d|", g_stSceneautoParam.pstIspParam[i].stFalseColorParam.pstFalseColor[j].u8Strength);
		}
		printf("\n");
		printf("gammaDelayCount = %d\n", g_stSceneautoParam.pstIspParam[i].stGammaParam.u32DelayCount);
		printf("gammaInterval = %d\n", g_stSceneautoParam.pstIspParam[i].stGammaParam.u32Interval);
		printf("gammaExpCount = %d\n", g_stSceneautoParam.pstIspParam[i].stGammaParam.u32ExpCount);
		printf("gammaExpThreshLtoH = ");
		for (j = 0; j < g_stSceneautoParam.pstIspParam[i].stGammaParam.u32ExpCount; j++)
		{
			printf("%d|", g_stSceneautoParam.pstIspParam[i].stGammaParam.pu32ExpThreshLtoH[j]);
		}
		printf("\n");
		printf("gammaExpThreshHtoL = ");
		for (j = 0; j < g_stSceneautoParam.pstIspParam[i].stGammaParam.u32ExpCount; j++)
		{
			printf("%d|", g_stSceneautoParam.pstIspParam[i].stGammaParam.pu32ExpThreshHtoL[j]);
		}
		printf("\n");
		for (j = 0; j < g_stSceneautoParam.pstIspParam[i].stGammaParam.u32ExpCount; j++)
		{
			printf("gamma_%d = ", j);
			for (k = 0; k < 257; k++)
			{
				printf("%d,", g_stSceneautoParam.pstIspParam[i].stGammaParam.pstGamma[j].u16Table[k]);
			}
			printf("\n");
		}
		printf("\n");
	}

	for (i = 0; i < g_stSceneautoParam.stCommParam.s32ViDevCount; i++)
	{
		printf("[VI_PARAM_%d]\n", i);
		printf("VI_DEV = %d\n", g_stSceneautoParam.pstViParam[i].ViDev);		
		printf("DCIEnable = %d\n", g_stSceneautoParam.pstViParam[i].stDciParam.bDCIEnable);
		printf("DCIBlackGain = %d\n", g_stSceneautoParam.pstViParam[i].stDciParam.u32DCIBlackGain);
		printf("DCIContrastGain = %d\n", g_stSceneautoParam.pstViParam[i].stDciParam.u32DCIContrastGain);
		printf("DCILightGain = %d\n", g_stSceneautoParam.pstViParam[i].stDciParam.u32DCILightGain);
		printf("\n");
	}

	for (i = 0; i < g_stSceneautoParam.stCommParam.s32VpssGrpCount; i++)
	{
		printf("[VPSS_PARAM_%d]\n", g_stSceneautoParam.pstVpssParam[i].VpssGrp);
		printf("3DnrIsoCount = %d\n", g_stSceneautoParam.pstVpssParam[i].st3dnrParam.u323DnrIsoCount);
		for (j = 0; j < g_stSceneautoParam.pstVpssParam[i].st3dnrParam.u323DnrIsoCount; j++)
		{
			printf("3DnrParam_%d = \\\n", j);

			 printf("   sf0_bright  ( %3d )      sfx_bright    ( %3d,%3d )      sfk_bright ( %3d )\\\n"
				    "   sf0_dark    ( %3d )      sfx_dark      ( %3d,%3d )      sfk_dark   ( %3d )\\\n"
				    "  ----------------------+                                  sfk_pro    ( %3d )\\\n"
				    "                        |   tfx_moving    ( %3d,%3d )      sfk_ratio  ( %3d )\\\n"
				    "   csf_strength( %3d )  |   tfx_md_thresh ( %3d,%3d )                        \\\n"
				    "   ctf_strength( %3d )  |   tfx_md_profile( %3d,%3d )    sf_ed_thresh ( %3d )\\\n"
				    "   ctf_range   ( %3d )  |   tfx_md_type   ( %3d,%3d )    sf_bsc_freq  ( %3d )\\\n"
				    "                        |   tfx_still     ( %3d,%3d )    tf_profile   ( %3d )\n",
				    g_stSceneautoParam.pstVpssParam[i].st3dnrParam.pst3dnrParam[j].sf0_bright,
				    g_stSceneautoParam.pstVpssParam[i].st3dnrParam.pst3dnrParam[j].sf1_bright,
				    g_stSceneautoParam.pstVpssParam[i].st3dnrParam.pst3dnrParam[j].sf2_bright,
				    g_stSceneautoParam.pstVpssParam[i].st3dnrParam.pst3dnrParam[j].sfk_bright,

				    g_stSceneautoParam.pstVpssParam[i].st3dnrParam.pst3dnrParam[j].sf0_dark,
				    g_stSceneautoParam.pstVpssParam[i].st3dnrParam.pst3dnrParam[j].sf1_dark,
				    g_stSceneautoParam.pstVpssParam[i].st3dnrParam.pst3dnrParam[j].sf2_dark,
				    g_stSceneautoParam.pstVpssParam[i].st3dnrParam.pst3dnrParam[j].sfk_dark,

				    g_stSceneautoParam.pstVpssParam[i].st3dnrParam.pst3dnrParam[j].sfk_profile,

				    g_stSceneautoParam.pstVpssParam[i].st3dnrParam.pst3dnrParam[j].tf1_moving,
				    g_stSceneautoParam.pstVpssParam[i].st3dnrParam.pst3dnrParam[j].tf2_moving,
				    g_stSceneautoParam.pstVpssParam[i].st3dnrParam.pst3dnrParam[j].sfk_ratio,

				    g_stSceneautoParam.pstVpssParam[i].st3dnrParam.pst3dnrParam[j].csf_strength,
				    g_stSceneautoParam.pstVpssParam[i].st3dnrParam.pst3dnrParam[j].tf1_md_thresh,
				    g_stSceneautoParam.pstVpssParam[i].st3dnrParam.pst3dnrParam[j].tf2_md_thresh,

					g_stSceneautoParam.pstVpssParam[i].st3dnrParam.pst3dnrParam[j].ctf_strength,
				    g_stSceneautoParam.pstVpssParam[i].st3dnrParam.pst3dnrParam[j].tf1_md_profile,
				    g_stSceneautoParam.pstVpssParam[i].st3dnrParam.pst3dnrParam[j].tf2_md_profile,
				    g_stSceneautoParam.pstVpssParam[i].st3dnrParam.pst3dnrParam[j].sf_ed_thresh,

				    g_stSceneautoParam.pstVpssParam[i].st3dnrParam.pst3dnrParam[j].ctf_range,
				    g_stSceneautoParam.pstVpssParam[i].st3dnrParam.pst3dnrParam[j].tf1_md_type,
				    g_stSceneautoParam.pstVpssParam[i].st3dnrParam.pst3dnrParam[j].tf2_md_type,
				    g_stSceneautoParam.pstVpssParam[i].st3dnrParam.pst3dnrParam[j].sf_bsc_freq,

				    g_stSceneautoParam.pstVpssParam[i].st3dnrParam.pst3dnrParam[j].tf1_still,
				    g_stSceneautoParam.pstVpssParam[i].st3dnrParam.pst3dnrParam[j].tf2_still,
				    g_stSceneautoParam.pstVpssParam[i].st3dnrParam.pst3dnrParam[j].tf_profile);
			
		}
		printf("\n");
	}

	printf("[IR]\n");
	printf("ExpCount = %d\n", g_stSceneautoParam.stIrParam.u32ExpCount);
	printf("ExpThreshLtoH = ");
	for (j = 0; j < g_stSceneautoParam.stIrParam.u32ExpCount; j++)
	{
		printf("%d|", g_stSceneautoParam.stIrParam.pu32ExpThreshLtoH[j]);
	}
	printf("\n");
	printf("ExpThreshHtoL = ");
	for (j = 0; j < g_stSceneautoParam.stIrParam.u32ExpCount; j++)
	{
		printf("%d|", g_stSceneautoParam.stIrParam.pu32ExpThreshHtoL[j]);
	}
	printf("\n");
	printf("ExpCompensation = ");
	for (j = 0; j < g_stSceneautoParam.stIrParam.u32ExpCount; j++)
	{
		printf("%d|", g_stSceneautoParam.stIrParam.pu8ExpCompensation[j]);
	}
	printf("\n");
	printf("MaxHistOffset = ");
	for (j = 0; j < g_stSceneautoParam.stIrParam.u32ExpCount; j++)
	{
		printf("%d|", g_stSceneautoParam.stIrParam.pu8MaxHistOffset[j]);
	}
	printf("\n");
	printf("u16HistRatioSlope = %d\n", g_stSceneautoParam.stIrParam.u16HistRatioSlope);
	printf("BlackDelayFrame = %d\n", g_stSceneautoParam.stIrParam.u16BlackDelayFrame);
	printf("WhiteDelayFrame = %d\n", g_stSceneautoParam.stIrParam.u16WhiteDelayFrame);
	printf("u8Tolerance = %d\n", g_stSceneautoParam.stIrParam.u8Tolerance);
	printf("u8Speed = %d\n", g_stSceneautoParam.stIrParam.u8Speed);
	for (j = 0; j < 15; j++)
	{
		printf("expweight_%2d =", j);
		for (k = 0; k < 17 ; k ++)
		{
			printf("%d,", g_stSceneautoParam.stIrParam.au8Weight[j][k]);
		}
		printf("\n");
	}
	printf("DCIEnable = %d\n", g_stSceneautoParam.stIrParam.bDCIEnable);
	printf("DCIBlackGain = %d\n", g_stSceneautoParam.stIrParam.u32DCIBlackGain);
	printf("DCIContrastGain = %d\n", g_stSceneautoParam.stIrParam.u32DCIContrastGain);
	printf("DCILightGain = %d\n", g_stSceneautoParam.stIrParam.u32DCILightGain);
	printf("IRu16Slope = %d\n", g_stSceneautoParam.stIrParam.u16Slope);
	printf("au16LumThresh = ");
	for (j = 0; j < EXPOSURE_LEVEL; j++)
	{
		printf("%d,", g_stSceneautoParam.stIrParam.au16LumThresh[j]);
	}
	printf("\n");
	printf("au8SharpenD = ");
	for (j = 0; j < EXPOSURE_LEVEL; j++)
	{
		printf("%d,", g_stSceneautoParam.stIrParam.au8SharpenD[j]);
	}
	printf("\n");
	printf("au8SharpenUd = ");
	for (j = 0; j < EXPOSURE_LEVEL; j++)
	{
		printf("%d,", g_stSceneautoParam.stIrParam.au8SharpenUd[j]);
	}
	printf("\n");
	printf("au8TextureSt = ");
	for (j = 0; j < EXPOSURE_LEVEL; j++)
	{
		printf("%d,", g_stSceneautoParam.stIrParam.au8TextureSt[j]);
	}
	printf("\n");
	printf("au8EdgeSt = ");
	for (j = 0; j < EXPOSURE_LEVEL; j++)
	{
		printf("%d,", g_stSceneautoParam.stIrParam.au8EdgeSt[j]);
	}
	printf("\n");
	printf("au8OverShoot = ");
	for (j = 0; j < EXPOSURE_LEVEL; j++)
	{
		printf("%d,", g_stSceneautoParam.stIrParam.au8OverShoot[j]);
	}
	printf("\n");
	printf("au8UnderShoot = ");
	for (j = 0; j < EXPOSURE_LEVEL; j++)
	{
		printf("%d,", g_stSceneautoParam.stIrParam.au8UnderShoot[j]);
	}
	printf("\n");
	printf("gamma_0 = ");
	for (j = 0; j < 257; j++)
	{
		printf("%d,", g_stSceneautoParam.stIrParam.u16GammaTable[j]);
	}
	printf("\n");
	printf("3DnrIsoCount = %d\n", g_stSceneautoParam.stIrParam.stIr3dnr.u323DnrIsoCount);
	for (j = 0; j < g_stSceneautoParam.stIrParam.stIr3dnr.u323DnrIsoCount; j++)
	{
		printf("3DnrParam_%d = \\\n", j);

		 printf("   sf0_bright  ( %3d )      sfx_bright    ( %3d,%3d )      sfk_bright ( %3d )\\\n"
			    "   sf0_dark    ( %3d )      sfx_dark      ( %3d,%3d )      sfk_dark   ( %3d )\\\n"
			    "  ----------------------+                                  sfk_pro    ( %3d )\\\n"
			    "                        |   tfx_moving    ( %3d,%3d )      sfk_ratio  ( %3d )\\\n"
			    "   csf_strength( %3d )  |   tfx_md_thresh ( %3d,%3d )                        \\\n"
			    "   ctf_strength( %3d )  |   tfx_md_profile( %3d,%3d )    sf_ed_thresh ( %3d )\\\n"
			    "   ctf_range   ( %3d )  |   tfx_md_type   ( %3d,%3d )    sf_bsc_freq  ( %3d )\\\n"
			    "                        |   tfx_still     ( %3d,%3d )    tf_profile   ( %3d )\n",
			    g_stSceneautoParam.stIrParam.stIr3dnr.pst3dnrParam[j].sf0_bright,
			    g_stSceneautoParam.stIrParam.stIr3dnr.pst3dnrParam[j].sf1_bright,
			    g_stSceneautoParam.stIrParam.stIr3dnr.pst3dnrParam[j].sf2_bright,
			    g_stSceneautoParam.stIrParam.stIr3dnr.pst3dnrParam[j].sfk_bright,

			    g_stSceneautoParam.stIrParam.stIr3dnr.pst3dnrParam[j].sf0_dark,
			    g_stSceneautoParam.stIrParam.stIr3dnr.pst3dnrParam[j].sf1_dark,
			    g_stSceneautoParam.stIrParam.stIr3dnr.pst3dnrParam[j].sf2_dark,
			    g_stSceneautoParam.stIrParam.stIr3dnr.pst3dnrParam[j].sfk_dark,

			    g_stSceneautoParam.stIrParam.stIr3dnr.pst3dnrParam[j].sfk_profile,

			    g_stSceneautoParam.stIrParam.stIr3dnr.pst3dnrParam[j].tf1_moving,
			    g_stSceneautoParam.stIrParam.stIr3dnr.pst3dnrParam[j].tf2_moving,
			    g_stSceneautoParam.stIrParam.stIr3dnr.pst3dnrParam[j].sfk_ratio,

			    g_stSceneautoParam.stIrParam.stIr3dnr.pst3dnrParam[j].csf_strength,
			    g_stSceneautoParam.stIrParam.stIr3dnr.pst3dnrParam[j].tf1_md_thresh,
			    g_stSceneautoParam.stIrParam.stIr3dnr.pst3dnrParam[j].tf2_md_thresh,

				g_stSceneautoParam.stIrParam.stIr3dnr.pst3dnrParam[j].ctf_strength,
			    g_stSceneautoParam.stIrParam.stIr3dnr.pst3dnrParam[j].tf1_md_profile,
			    g_stSceneautoParam.stIrParam.stIr3dnr.pst3dnrParam[j].tf2_md_profile,
			    g_stSceneautoParam.stIrParam.stIr3dnr.pst3dnrParam[j].sf_ed_thresh,

			    g_stSceneautoParam.stIrParam.stIr3dnr.pst3dnrParam[j].ctf_range,
			    g_stSceneautoParam.stIrParam.stIr3dnr.pst3dnrParam[j].tf1_md_type,
			    g_stSceneautoParam.stIrParam.stIr3dnr.pst3dnrParam[j].tf2_md_type,
			    g_stSceneautoParam.stIrParam.stIr3dnr.pst3dnrParam[j].sf_bsc_freq,

			    g_stSceneautoParam.stIrParam.stIr3dnr.pst3dnrParam[j].tf1_still,
			    g_stSceneautoParam.stIrParam.stIr3dnr.pst3dnrParam[j].tf2_still,
			    g_stSceneautoParam.stIrParam.stIr3dnr.pst3dnrParam[j].tf_profile);
		
	}
	printf("\n");
	printf("[HLC]\n");
	printf("u8ExpRatioType = %d\n", g_stSceneautoParam.stHlcParam.u8ExpRatioType);
	printf("au32ExpRatio = %d,%d,%d\n", g_stSceneautoParam.stHlcParam.au32ExpRatio[0],g_stSceneautoParam.stHlcParam.au32ExpRatio[1],g_stSceneautoParam.stHlcParam.au32ExpRatio[2]);
	printf("u32ExpRatioMax = %d\n", g_stSceneautoParam.stHlcParam.u32ExpRatioMax);
	printf("u32ExpRatioMin = %d\n", g_stSceneautoParam.stHlcParam.u32ExpRatioMin);	
	printf("ExpCompensation = %d\n", g_stSceneautoParam.stHlcParam.u8ExpCompensation);
	printf("WhiteDelayFrame = %d\n", g_stSceneautoParam.stHlcParam.u16WhiteDelayFrame);
	printf("BlackDelayFrame = %d\n", g_stSceneautoParam.stHlcParam.u16BlackDelayFrame);
	printf("u8Speed = %d\n", g_stSceneautoParam.stHlcParam.u8Speed);
	printf("HistRatioSlope = %d\n", g_stSceneautoParam.stHlcParam.u16HistRatioSlope);
	printf("MaxHistOffset = %d\n", g_stSceneautoParam.stHlcParam.u8MaxHistOffset);
	printf("u8Tolerance = %d\n", g_stSceneautoParam.stHlcParam.u8Tolerance);
	printf("DCIEnable = %d\n", g_stSceneautoParam.stHlcParam.bDCIEnable);
	printf("DCIBlackGain = %d\n", g_stSceneautoParam.stHlcParam.u32DCIBlackGain);
	printf("DCIContrastGain = %d\n", g_stSceneautoParam.stHlcParam.u32DCIContrastGain);
	printf("DCILightGain = %d\n", g_stSceneautoParam.stHlcParam.u32DCILightGain);
	printf("DRCEnable = %d\n", g_stSceneautoParam.stHlcParam.bDRCEnable);
	printf("DRCManulEnable = %d\n", g_stSceneautoParam.stHlcParam.bDRCManulEnable);
	printf("DRCStrengthTarget = %d\n", g_stSceneautoParam.stHlcParam.u32DRCStrengthTarget);
	printf("DRCAutoStrength = %d\n", g_stSceneautoParam.stHlcParam.u32DRCAutoStrength);
	printf("u32WhiteLevel = %d\n", g_stSceneautoParam.stHlcParam.u32WhiteLevel);
	printf("u32BlackLevel = %d\n", g_stSceneautoParam.stHlcParam.u32BlackLevel);
	printf("u32Asymmetry = %d\n", g_stSceneautoParam.stHlcParam.u32Asymmetry);
	printf("u32BrightEnhance = %d\n", g_stSceneautoParam.stHlcParam.u32BrightEnhance);
	printf("u8FilterMux = %d\n", g_stSceneautoParam.stHlcParam.u8FilterMux);
	printf("u32SlopeMax = %d\n", g_stSceneautoParam.stHlcParam.u32SlopeMax);
	printf("u32SlopeMin = %d\n", g_stSceneautoParam.stHlcParam.u32SlopeMin);
	printf("u32VarianceIntensity = %d\n", g_stSceneautoParam.stHlcParam.u32VarianceIntensity);
	printf("u32VarianceSpace = %d\n", g_stSceneautoParam.stHlcParam.u32VarianceSpace);
	printf("u32BrightPr = %d\n", g_stSceneautoParam.stHlcParam.u32BrightPr);
	printf("u32Contrast = %d\n", g_stSceneautoParam.stHlcParam.u32Contrast);
	printf("u32DarkEnhance = %d\n", g_stSceneautoParam.stHlcParam.u32DarkEnhance);
	printf("u32Svariance = %d\n", g_stSceneautoParam.stHlcParam.u32Svariance);
	printf("Saturation = ");
	for (j = 0; j < 16; j++)
	{
		printf("%d,", g_stSceneautoParam.stHlcParam.u8Saturation[j]);
	}
	printf("\n");
	printf("gamma_0 = ");
	for (j = 0; j < 257; j++)
	{
		printf("%d,", g_stSceneautoParam.stHlcParam.u16GammaTable[j]);
	}
	printf("\n");
	printf("3DnrIsoCount = %d\n", g_stSceneautoParam.stHlcParam.stHlc3dnr.u323DnrIsoCount);
	for (j = 0; j < g_stSceneautoParam.stHlcParam.stHlc3dnr.u323DnrIsoCount; j++)
	{
		printf("3DnrParam_%d = \\\n", j);

		 printf("   sf0_bright  ( %3d )      sfx_bright    ( %3d,%3d )      sfk_bright ( %3d )\\\n"
			    "   sf0_dark    ( %3d )      sfx_dark      ( %3d,%3d )      sfk_dark   ( %3d )\\\n"
			    "  ----------------------+                                  sfk_pro    ( %3d )\\\n"
			    "                        |   tfx_moving    ( %3d,%3d )      sfk_ratio  ( %3d )\\\n"
			    "   csf_strength( %3d )  |   tfx_md_thresh ( %3d,%3d )                        \\\n"
			    "   ctf_strength( %3d )  |   tfx_md_profile( %3d,%3d )    sf_ed_thresh ( %3d )\\\n"
			    "   ctf_range   ( %3d )  |   tfx_md_type   ( %3d,%3d )    sf_bsc_freq  ( %3d )\\\n"
			    "                        |   tfx_still     ( %3d,%3d )    tf_profile   ( %3d )\n",
			    g_stSceneautoParam.stHlcParam.stHlc3dnr.pst3dnrParam[j].sf0_bright,
			    g_stSceneautoParam.stHlcParam.stHlc3dnr.pst3dnrParam[j].sf1_bright,
			    g_stSceneautoParam.stHlcParam.stHlc3dnr.pst3dnrParam[j].sf2_bright,
			    g_stSceneautoParam.stHlcParam.stHlc3dnr.pst3dnrParam[j].sfk_bright,

			    g_stSceneautoParam.stHlcParam.stHlc3dnr.pst3dnrParam[j].sf0_dark,
			    g_stSceneautoParam.stHlcParam.stHlc3dnr.pst3dnrParam[j].sf1_dark,
			    g_stSceneautoParam.stHlcParam.stHlc3dnr.pst3dnrParam[j].sf2_dark,
			    g_stSceneautoParam.stHlcParam.stHlc3dnr.pst3dnrParam[j].sfk_dark,

			    g_stSceneautoParam.stHlcParam.stHlc3dnr.pst3dnrParam[j].sfk_profile,

			    g_stSceneautoParam.stHlcParam.stHlc3dnr.pst3dnrParam[j].tf1_moving,
			    g_stSceneautoParam.stHlcParam.stHlc3dnr.pst3dnrParam[j].tf2_moving,
			    g_stSceneautoParam.stHlcParam.stHlc3dnr.pst3dnrParam[j].sfk_ratio,

			    g_stSceneautoParam.stHlcParam.stHlc3dnr.pst3dnrParam[j].csf_strength,
			    g_stSceneautoParam.stHlcParam.stHlc3dnr.pst3dnrParam[j].tf1_md_thresh,
			    g_stSceneautoParam.stHlcParam.stHlc3dnr.pst3dnrParam[j].tf2_md_thresh,

				g_stSceneautoParam.stHlcParam.stHlc3dnr.pst3dnrParam[j].ctf_strength,
			    g_stSceneautoParam.stHlcParam.stHlc3dnr.pst3dnrParam[j].tf1_md_profile,
			    g_stSceneautoParam.stHlcParam.stHlc3dnr.pst3dnrParam[j].tf2_md_profile,
			    g_stSceneautoParam.stHlcParam.stHlc3dnr.pst3dnrParam[j].sf_ed_thresh,

			    g_stSceneautoParam.stHlcParam.stHlc3dnr.pst3dnrParam[j].ctf_range,
			    g_stSceneautoParam.stHlcParam.stHlc3dnr.pst3dnrParam[j].tf1_md_type,
			    g_stSceneautoParam.stHlcParam.stHlc3dnr.pst3dnrParam[j].tf2_md_type,
			    g_stSceneautoParam.stHlcParam.stHlc3dnr.pst3dnrParam[j].sf_bsc_freq,

			    g_stSceneautoParam.stHlcParam.stHlc3dnr.pst3dnrParam[j].tf1_still,
			    g_stSceneautoParam.stHlcParam.stHlc3dnr.pst3dnrParam[j].tf2_still,
			    g_stSceneautoParam.stHlcParam.stHlc3dnr.pst3dnrParam[j].tf_profile);
		
	}
	printf("\n");
	printf("[TRAFFIC]\n");
	printf("DRCEnable = %d\n", g_stSceneautoParam.stTrafficParam.bDRCEnable);
	printf("DRCManulEnable = %d\n", g_stSceneautoParam.stTrafficParam.bDRCManulEnable);
	printf("DRCStrengthTarget = %d\n", g_stSceneautoParam.stTrafficParam.u32DRCStrengthTarget);
	printf("DRCAutoStrength = %d\n", g_stSceneautoParam.stTrafficParam.u32DRCAutoStrength);
	printf("u32WhiteLevel = %d\n", g_stSceneautoParam.stTrafficParam.u32WhiteLevel);
	printf("u32BlackLevel = %d\n", g_stSceneautoParam.stTrafficParam.u32BlackLevel);
	printf("u32Asymmetry = %d\n", g_stSceneautoParam.stTrafficParam.u32Asymmetry);
	printf("u32BrightEnhance = %d\n", g_stSceneautoParam.stTrafficParam.u32BrightEnhance);
	printf("u8FilterMux = %d\n", g_stSceneautoParam.stTrafficParam.u8FilterMux);
	printf("u32SlopeMax = %d\n", g_stSceneautoParam.stTrafficParam.u32SlopeMax);
	printf("u32SlopeMin = %d\n", g_stSceneautoParam.stTrafficParam.u32SlopeMin);
	printf("u32VarianceIntensity = %d\n", g_stSceneautoParam.stTrafficParam.u32VarianceIntensity);
	printf("u32VarianceSpace = %d\n", g_stSceneautoParam.stTrafficParam.u32VarianceSpace);
	printf("u32BrightPr = %d\n", g_stSceneautoParam.stTrafficParam.u32BrightPr);
	printf("u32Contrast = %d\n", g_stSceneautoParam.stTrafficParam.u32Contrast);
	printf("u32DarkEnhance = %d\n", g_stSceneautoParam.stTrafficParam.u32DarkEnhance);
	printf("u32Svariance = %d\n", g_stSceneautoParam.stTrafficParam.u32Svariance);
	printf("u8ExpCompensation = %d\n", g_stSceneautoParam.stTrafficParam.u8ExpCompensation);
	printf("u8MaxHistoffset = %d\n", g_stSceneautoParam.stTrafficParam.u8MaxHistoffset);
	printf("u8ExpRatioType = %d\n", g_stSceneautoParam.stTrafficParam.u8ExpRatioType);
	printf("au32ExpRatio = %d,%d,%d\n", g_stSceneautoParam.stTrafficParam.au32ExpRatio[0],g_stSceneautoParam.stTrafficParam.au32ExpRatio[1],g_stSceneautoParam.stTrafficParam.au32ExpRatio[2]);
	printf("u32ExpRatioMax = %d\n", g_stSceneautoParam.stTrafficParam.u32ExpRatioMax);
	printf("u32ExpRatioMin = %d\n", g_stSceneautoParam.stTrafficParam.u32ExpRatioMin);
	printf("au8TextureSt = ");
	for (j = 0; j < 16; j++)
	{
		printf("%d|", g_stSceneautoParam.stTrafficParam.au8TextureSt[j]);
	}
	printf("\n");
	printf("au8EdgeSt = ");
	for (j = 0; j < 16; j++)
	{
		printf("%d|", g_stSceneautoParam.stTrafficParam.au8EdgeSt[j]);
	}
	printf("\n");
	printf("au8OverShoot = ");
	for (j = 0; j < 16; j++)
	{
		printf("%d|", g_stSceneautoParam.stTrafficParam.au8OverShoot[j]);
	}
	printf("\n");
	printf("au8UnderShoot = ");
	for (j = 0; j < 16; j++)
	{
		printf("%d|", g_stSceneautoParam.stTrafficParam.au8UnderShoot[j]);
	}
	printf("\n");
	printf("au8TextureThd = ");
	for (j = 0; j < 16; j++)
	{
		printf("%d|", g_stSceneautoParam.stTrafficParam.au8TextureThd[j]);
	}
	printf("\n");
	printf("au8EdgeThd = ");
	for (j = 0; j < 16; j++)
	{
		printf("%d|", g_stSceneautoParam.stTrafficParam.au8EdgeThd[j]);
	}
	printf("\n");
	printf("au8DetailCtrl = ");
	for (j = 0; j < 16; j++)
	{
		printf("%d|", g_stSceneautoParam.stTrafficParam.au8DetailCtrl[j]);
	}
	printf("\n");
	printf("gamma_0 = ");
	for (j = 0; j < 257; j++)
	{
		printf("%d,", g_stSceneautoParam.stTrafficParam.u16GammaTable[j]);
	}
	printf("\n");
	printf("3DnrIsoCount = %d\n", g_stSceneautoParam.stTrafficParam.stTraffic3dnr.u323DnrIsoCount);
	for (j = 0; j < g_stSceneautoParam.stTrafficParam.stTraffic3dnr.u323DnrIsoCount; j++)
	{
		printf("3DnrParam_%d = \\\n", j);

		 printf("   sf0_bright  ( %3d )      sfx_bright    ( %3d,%3d )      sfk_bright ( %3d )\\\n"
			    "   sf0_dark    ( %3d )      sfx_dark      ( %3d,%3d )      sfk_dark   ( %3d )\\\n"
			    "  ----------------------+                                  sfk_pro    ( %3d )\\\n"
			    "                        |   tfx_moving    ( %3d,%3d )      sfk_ratio  ( %3d )\\\n"
			    "   csf_strength( %3d )  |   tfx_md_thresh ( %3d,%3d )                        \\\n"
			    "   ctf_strength( %3d )  |   tfx_md_profile( %3d,%3d )    sf_ed_thresh ( %3d )\\\n"
			    "   ctf_range   ( %3d )  |   tfx_md_type   ( %3d,%3d )    sf_bsc_freq  ( %3d )\\\n"
			    "                        |   tfx_still     ( %3d,%3d )    tf_profile   ( %3d )\n",
			    g_stSceneautoParam.stTrafficParam.stTraffic3dnr.pst3dnrParam[j].sf0_bright,
			    g_stSceneautoParam.stTrafficParam.stTraffic3dnr.pst3dnrParam[j].sf1_bright,
			    g_stSceneautoParam.stTrafficParam.stTraffic3dnr.pst3dnrParam[j].sf2_bright,
			    g_stSceneautoParam.stTrafficParam.stTraffic3dnr.pst3dnrParam[j].sfk_bright,

			    g_stSceneautoParam.stTrafficParam.stTraffic3dnr.pst3dnrParam[j].sf0_dark,
			    g_stSceneautoParam.stTrafficParam.stTraffic3dnr.pst3dnrParam[j].sf1_dark,
			    g_stSceneautoParam.stTrafficParam.stTraffic3dnr.pst3dnrParam[j].sf2_dark,
			    g_stSceneautoParam.stTrafficParam.stTraffic3dnr.pst3dnrParam[j].sfk_dark,

			    g_stSceneautoParam.stTrafficParam.stTraffic3dnr.pst3dnrParam[j].sfk_profile,

			    g_stSceneautoParam.stTrafficParam.stTraffic3dnr.pst3dnrParam[j].tf1_moving,
			    g_stSceneautoParam.stTrafficParam.stTraffic3dnr.pst3dnrParam[j].tf2_moving,
			    g_stSceneautoParam.stTrafficParam.stTraffic3dnr.pst3dnrParam[j].sfk_ratio,

			    g_stSceneautoParam.stTrafficParam.stTraffic3dnr.pst3dnrParam[j].csf_strength,
			    g_stSceneautoParam.stTrafficParam.stTraffic3dnr.pst3dnrParam[j].tf1_md_thresh,
			    g_stSceneautoParam.stTrafficParam.stTraffic3dnr.pst3dnrParam[j].tf2_md_thresh,

				g_stSceneautoParam.stTrafficParam.stTraffic3dnr.pst3dnrParam[j].ctf_strength,
			    g_stSceneautoParam.stTrafficParam.stTraffic3dnr.pst3dnrParam[j].tf1_md_profile,
			    g_stSceneautoParam.stTrafficParam.stTraffic3dnr.pst3dnrParam[j].tf2_md_profile,
			    g_stSceneautoParam.stTrafficParam.stTraffic3dnr.pst3dnrParam[j].sf_ed_thresh,

			    g_stSceneautoParam.stTrafficParam.stTraffic3dnr.pst3dnrParam[j].ctf_range,
			    g_stSceneautoParam.stTrafficParam.stTraffic3dnr.pst3dnrParam[j].tf1_md_type,
			    g_stSceneautoParam.stTrafficParam.stTraffic3dnr.pst3dnrParam[j].tf2_md_type,
			    g_stSceneautoParam.stTrafficParam.stTraffic3dnr.pst3dnrParam[j].sf_bsc_freq,

			    g_stSceneautoParam.stTrafficParam.stTraffic3dnr.pst3dnrParam[j].tf1_still,
			    g_stSceneautoParam.stTrafficParam.stTraffic3dnr.pst3dnrParam[j].tf2_still,
			    g_stSceneautoParam.stTrafficParam.stTraffic3dnr.pst3dnrParam[j].tf_profile);
		
	}
	printf("\n");
	printf("[FSWDR]\n");
	printf("DRCStrength = %d\n", g_stSceneautoParam.stFswdrParam.u32DRCStrength);
	printf("ISOUpTh = %d\n", g_stSceneautoParam.stFswdrParam.u32ISOUpTh);
	printf("ISODnTh = %d\n", g_stSceneautoParam.stFswdrParam.u32ISODnTh);
	printf("RefExpRatioTh = %d\n", g_stSceneautoParam.stFswdrParam.u32RefExpRatioTh);
	printf("ExpCount = %d\n", g_stSceneautoParam.stFswdrParam.u32ExpCount);
	printf("ExpThreshLtoH = ");
	for (j = 0; j < g_stSceneautoParam.stFswdrParam.u32ExpCount; j++)
	{
		printf("%d|", g_stSceneautoParam.stFswdrParam.pu32ExpThreshLtoH[j]);
	}
	printf("\n");
	printf("ExpThreshHtoL = ");
	for (j = 0; j < g_stSceneautoParam.stFswdrParam.u32ExpCount; j++)
	{
		printf("%d|", g_stSceneautoParam.stFswdrParam.pu32ExpThreshHtoL[j]);
	}
	printf("\n");
	printf("ExpCompensation = ");
	for (j = 0; j < g_stSceneautoParam.stFswdrParam.u32ExpCount; j++)
	{
		printf("%d|", g_stSceneautoParam.stFswdrParam.pu8ExpCompensation[j]);
	}
	printf("\n");
	printf("MaxHistOffset = ");
	for (j = 0; j < g_stSceneautoParam.stFswdrParam.u32ExpCount; j++)
	{
		printf("%d|", g_stSceneautoParam.stFswdrParam.pu8MaxHistOffset[j]);
	}
	printf("\n");

	printf("3DnrIsoCount = %d\n", g_stSceneautoParam.stFswdrParam.stFSWDR3dnr.u323DnrIsoCount);
	for (j = 0; j < g_stSceneautoParam.stFswdrParam.stFSWDR3dnr.u323DnrIsoCount; j++)
	{
		printf("3DnrParam_%d = \\\n", j);

		 printf("   sf0_bright  ( %3d )      sfx_bright    ( %3d,%3d )      sfk_bright ( %3d )\\\n"
			    "   sf0_dark    ( %3d )      sfx_dark      ( %3d,%3d )      sfk_dark   ( %3d )\\\n"
			    "  ----------------------+                                  sfk_pro    ( %3d )\\\n"
			    "                        |   tfx_moving    ( %3d,%3d )      sfk_ratio  ( %3d )\\\n"
			    "   csf_strength( %3d )  |   tfx_md_thresh ( %3d,%3d )                        \\\n"
			    "   ctf_strength( %3d )  |   tfx_md_profile( %3d,%3d )    sf_ed_thresh ( %3d )\\\n"
			    "   ctf_range   ( %3d )  |   tfx_md_type   ( %3d,%3d )    sf_bsc_freq  ( %3d )\\\n"
			    "                        |   tfx_still     ( %3d,%3d )    tf_profile   ( %3d )\n",
			    g_stSceneautoParam.stFswdrParam.stFSWDR3dnr.pst3dnrParam[j].sf0_bright,
			    g_stSceneautoParam.stFswdrParam.stFSWDR3dnr.pst3dnrParam[j].sf1_bright,
			    g_stSceneautoParam.stFswdrParam.stFSWDR3dnr.pst3dnrParam[j].sf2_bright,
			    g_stSceneautoParam.stFswdrParam.stFSWDR3dnr.pst3dnrParam[j].sfk_bright,

			    g_stSceneautoParam.stFswdrParam.stFSWDR3dnr.pst3dnrParam[j].sf0_dark,
			    g_stSceneautoParam.stFswdrParam.stFSWDR3dnr.pst3dnrParam[j].sf1_dark,
			    g_stSceneautoParam.stFswdrParam.stFSWDR3dnr.pst3dnrParam[j].sf2_dark,
			    g_stSceneautoParam.stFswdrParam.stFSWDR3dnr.pst3dnrParam[j].sfk_dark,

			    g_stSceneautoParam.stFswdrParam.stFSWDR3dnr.pst3dnrParam[j].sfk_profile,

			    g_stSceneautoParam.stFswdrParam.stFSWDR3dnr.pst3dnrParam[j].tf1_moving,
			    g_stSceneautoParam.stFswdrParam.stFSWDR3dnr.pst3dnrParam[j].tf2_moving,
			    g_stSceneautoParam.stFswdrParam.stFSWDR3dnr.pst3dnrParam[j].sfk_ratio,

			    g_stSceneautoParam.stFswdrParam.stFSWDR3dnr.pst3dnrParam[j].csf_strength,
			    g_stSceneautoParam.stFswdrParam.stFSWDR3dnr.pst3dnrParam[j].tf1_md_thresh,
			    g_stSceneautoParam.stFswdrParam.stFSWDR3dnr.pst3dnrParam[j].tf2_md_thresh,

				g_stSceneautoParam.stFswdrParam.stFSWDR3dnr.pst3dnrParam[j].ctf_strength,
			    g_stSceneautoParam.stFswdrParam.stFSWDR3dnr.pst3dnrParam[j].tf1_md_profile,
			    g_stSceneautoParam.stFswdrParam.stFSWDR3dnr.pst3dnrParam[j].tf2_md_profile,
			    g_stSceneautoParam.stFswdrParam.stFSWDR3dnr.pst3dnrParam[j].sf_ed_thresh,

			    g_stSceneautoParam.stFswdrParam.stFSWDR3dnr.pst3dnrParam[j].ctf_range,
			    g_stSceneautoParam.stFswdrParam.stFSWDR3dnr.pst3dnrParam[j].tf1_md_type,
			    g_stSceneautoParam.stFswdrParam.stFSWDR3dnr.pst3dnrParam[j].tf2_md_type,
			    g_stSceneautoParam.stFswdrParam.stFSWDR3dnr.pst3dnrParam[j].sf_bsc_freq,

			    g_stSceneautoParam.stFswdrParam.stFSWDR3dnr.pst3dnrParam[j].tf1_still,
			    g_stSceneautoParam.stFswdrParam.stFSWDR3dnr.pst3dnrParam[j].tf2_still,
			    g_stSceneautoParam.stFswdrParam.stFSWDR3dnr.pst3dnrParam[j].tf_profile);
		
	}
	printf("\n");
}

HI_S32 HI_SCENEAUTO_Init(const HI_CHAR* pszFileName)
{
    HI_S32 s32Ret = HI_SUCCESS;

    CHECK_NULL_PTR(pszFileName);

    MUTEX_LOCK(g_stSceneautoLock);
    if (HI_TRUE == g_stSceneautoState.bSceneautoInit)
    {
        printf("SCENEAUTO Module has been inited already\n");
        MUTEX_UNLOCK(g_stSceneautoLock);
        return HI_SUCCESS;
    }

    s32Ret = Sceneauto_LoadFile(pszFileName);
    if (HI_SUCCESS != s32Ret)
    {
        printf("Sceneauto_LoadFile failed\n");
        Sceneauto_FreeDict();
		MUTEX_UNLOCK(g_stSceneautoLock);
        return HI_FAILURE;
    }

    s32Ret = Sceneauto_LoadINIPara();
    if (HI_SUCCESS != s32Ret)
    {
        printf("Sceneauto_LoadCommonPara failed\n");
        Sceneauto_FreeDict();
        Sceneauto_FreeMem();
        MUTEX_UNLOCK(g_stSceneautoLock);
        return HI_FAILURE;
    }

    //PrintParam();

	g_stSceneautoState.bSceneautoInit = HI_TRUE;
    g_eSpecialScene = SCENEAUTO_SPECIAL_SCENE_NONE; 
    g_stSceneautoState.pstPreviousIspParam = (HI_SCENEAUTO_PREISPPARAM_S*)malloc((g_stSceneautoParam.stCommParam.s32IspDevCount) * sizeof(HI_SCENEAUTO_PREISPPARAM_S));
	//CHECK_NULL_PTR(g_stSceneautoState.pstPreviousIspParam);
	if (NULL == g_stSceneautoState.pstPreviousIspParam)
	{
        printf("func:%s,line:%d, NULL pointer\n",__FUNCTION__,__LINE__);
        MUTEX_UNLOCK(g_stSceneautoLock);
        return HI_FAILURE;
    }
	memset(g_stSceneautoState.pstPreviousIspParam, 0, (g_stSceneautoParam.stCommParam.s32IspDevCount) * sizeof(HI_SCENEAUTO_PREISPPARAM_S));
    g_stSceneautoState.pstPrevisouViParam = (HI_SCENEAUTO_PREVIPARAM_S*)malloc((g_stSceneautoParam.stCommParam.s32ViDevCount) * sizeof(HI_SCENEAUTO_PREVIPARAM_S));
	//CHECK_NULL_PTR(g_stSceneautoState.pstPrevisouViParam);
	if (NULL == g_stSceneautoState.pstPrevisouViParam)
	{
        printf("func:%s,line:%d, NULL pointer\n",__FUNCTION__,__LINE__);
        MUTEX_UNLOCK(g_stSceneautoLock);
        return HI_FAILURE;
    }
	memset(g_stSceneautoState.pstPrevisouViParam, 0, (g_stSceneautoParam.stCommParam.s32ViDevCount) * sizeof(HI_SCENEAUTO_PREVIPARAM_S));
	
    MUTEX_UNLOCK(g_stSceneautoLock);

    printf("SCENUAUTO Module has been inited successfully!\n");

    return HI_SUCCESS;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */
