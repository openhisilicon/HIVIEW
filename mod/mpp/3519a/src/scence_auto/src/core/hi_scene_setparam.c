#include "hi_scene_setparam.h"
#include <unistd.h>
#include <string.h>
#include "mpi_awb.h"
#include "mpi_ae.h"
#include "mpi_isp.h"
#include "mpi_vi.h"
#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

static HI_U32 DRC_strfilter[3]={0};

extern HI_S32 HI_MPI_VPSS_SetGrpNRXParam(VPSS_GRP VpssGrp, VPSS_GRP_NRX_PARAM_S *pstVpssNrXParam);
extern HI_S32 HI_MPI_VPSS_GetGrpNRXParam(VPSS_GRP VpssGrp, VPSS_GRP_NRX_PARAM_S *pstVpssNrXParam);

extern HI_S32 HI_MPI_VPSS_GetGrpAttr(VPSS_GRP VpssGrp, VPSS_GRP_ATTR_S* pstGrpAttr);
extern HI_S32 HI_MPI_VPSS_SetGrpAttr(VPSS_GRP VpssGrp, VPSS_GRP_ATTR_S* pstGrpAttr);

#define SCENE_MAX(a, b) (((a) < (b)) ? (b) : (a))

#define SCENE_DIV_0TO1(a)  ((0 == (a)) ? 1 : (a))

#define ABS(a,b) ((a >= b) ? (a-b) : (b-a))

#define SCENE_SHADING_TRIGMODE_L2H            (1)

#define SCENE_SHADING_TRIGMODE_H2L            (2)

#define SCENE_MAX_ISP_PIPE_NUM                (6)

HI_BOOL g_bISPPause;
HI_SCENE_PIPE_PARAM_S g_astScenePipeParam[HI_SCENE_PIPETYPE_NUM];

#define CHECK_SCENE_PAUSE()\
    do{\
        if (HI_TRUE == g_bISPPause)\
        {\
            return HI_SUCCESS;\
        }\
    }while(0);\

#define CHECK_SCENE_RET(s32Ret)\
    do{\
        if (HI_SUCCESS != s32Ret)\
        {\
            printf("Failed at %s: LINE: %d with %#x!", __FUNCTION__, __LINE__, s32Ret);\
        }\
    }while(0);\

#define CHECK_SCENE_NULLPTR(ptr)\
    do{\
        if (NULL == ptr)\
        {\
            printf("NullPtr at %s: LINE: %d!", __FUNCTION__, __LINE__);\
            return HI_FAILURE;\
        }\
    }while(0);\

#define FREE_SCENE_PTR(ptr)\
    do{\
        if (NULL != ptr)\
        {\
            free(ptr);\
            ptr = NULL;\
        }\
    }while(0);\

static	HI_S32 iMax2(HI_S32 a, HI_S32 b) {{ if (a > b) b = a; }; return b; }

static	HI_U32 MapISO(HI_U32 iso)
{
  HI_U32   j,  i = (iso >= 200);

  if (iso < 72) return iMax2(iso, -3);

  i += ( (iso >= (200 << 1)) + (iso >= (400 << 1)) + (iso >= (400 << 2)) + (iso >= (400 << 3)) + (iso >= (400 << 4)) );
  i += ( (iso >= (400 << 5)) + (iso >= (400 << 6)) + (iso >= (400 << 7)) + (iso >= (400 << 8)) + (iso >= (400 << 9)) );
  i += ( (iso >= (400 << 10))+ (iso >= (400 << 11))+ (iso >= (400 << 12))+ (iso >= (400 << 13))+ (iso >= (400 << 14))) ;
  j  = ( (iso >  (112 << i)) + (iso >  (125 << i)) + (iso >  (141 << i)) + (iso >  (158 << i)) + (iso >  (178 << i)) );

  return (i * 6 + j + (iso >= 80) + (iso >= 90) + (iso >= 100) - 3);
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

static  HI_U32 SCENE_GetLevelLtoH(HI_U64 u64Value, HI_U32 u32Count, HI_U64* pu64Thresh)
{
    HI_U32 u32Level = 0;

    for (u32Level = 0; u32Level < u32Count; u32Level++)
    {
        if (u64Value <= pu64Thresh[u32Level])
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
static  HI_U32 SCENE_GetLevelHtoL(HI_U64 u64Value, HI_U32 u32Count, HI_U64* pu64Thresh)
{
    HI_U32 u32Level = 0;

    for (u32Level = u32Count; u32Level > 0; u32Level--)
    {
        if (u64Value > pu64Thresh[u32Level - 1])
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


static HI_U32 SCENE_GetLevelLtoH_U32(HI_U32 u32Value, HI_U32 u32Count, HI_U32* pu32Thresh)
{
    HI_U32 u32Level = 0;

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
static HI_U32 SCENE_Interpulate(HI_U32 u32Mid, HI_U32 u32Left, HI_U32 u32LValue, HI_U32 u32Right, HI_U32 u32RValue)
{
    HI_U32 u32Value = 0;
    HI_U32 k = 0;

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
    u32Value = (((u32Right - u32Mid) * u32LValue + (u32Mid - u32Left) * u32RValue + (k >> 1)) / k);
    return u32Value;
}

static HI_U32 SCENE_TimeFilter(HI_U32 u32Para0, HI_U32 u32Para1, HI_U32 u32TimeCnt, HI_U32 u32Index)
{
    HI_U64 u64Temp = 0;
    HI_U32 u32Value = 0;

    if (u32Para0 > u32Para1)
    {
        u64Temp = (HI_U64)(u32Para0 - u32Para1) << 8;
        u64Temp = (u64Temp * (u32Index + 1)) / SCENE_DIV_0TO1(u32TimeCnt) >> 8;
        u32Value = u32Para0 - (HI_U32)u64Temp - 1;
    }
    else if (u32Para0 < u32Para1)
    {
        u64Temp = (HI_U64)(u32Para1 - u32Para0) << 8;
        u64Temp = (u64Temp * (u32Index + 1)) / SCENE_DIV_0TO1(u32TimeCnt) >> 8;
        u32Value = u32Para0 + (HI_U32)u64Temp + 1;
    }
    else
    {
        u32Value = u32Para0;
    }


    return u32Value;
}

void PrintNRx(const HI_SCENE_3DNR_S *pX )
{
  const  tV56xIEy*  pi = pX->IEy;
  const  tV56xSFy*  ps = pX->SFy;
  const  tV56xMDy*  pm = pX->MDy;
  const  tV56xTFy*  pt = pX->TFy;

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

  printf("\n\n                             sizeof( X ) = %d ", sizeof(*pX) );
  printf("\n*******************************************************************************");
  printf("\n -en                %3d |             %3d |             %3d |             %3d",  _tmprt1_( ps, NRyEn ));
  printf("\n                        |                 |                 |                ");
  printf("\n -nXsf2     %3d:%3d:%3d |     %3d:%3d:%3d |     %3d:%3d:%3d |     %3d:%3d:%3d", _tmprt3_( ps, SFS2, SFT2, SBR2 ));
  printf("\n                        |                 |                 |                ");
  printf("\n -nXsf4     %3d:%3d:%3d |     %3d:%3d:%3d |     %3d:%3d:%3d |     %3d:%3d:%3d", _tmprt3_( ps, SFS4, SFT4, SBR4 ));
  printf("\n -bwsf4             %3d |             %3d |             %3d |             %3d", _tmprt1_( ps, BWSF4 ));
  printf("\n -kmsf4                 |                 |             %3d |             %3d", ps[2].kMode, ps[3].kMode);
  printf("\n                        |                 |                 |                ");
  printf("\n -nXsf5 %3d:%3d:%3d:%3d |                 |                 | %3d:%3d:%3d:%3d", _t4_( pi,0, IES0, IES1, IES2, IES3 ),_t4_( pi,1, IES0, IES1, IES2, IES3 ));
  printf("\n -dzsf5             %3d |                 |                 |             %3d",       pi[0].IEDZ, pi[1].IEDZ );
  printf("\n                        |                 |                 |                ");
  printf("\n -nXsf6     %3d:%3d:%3d |     %3d:%3d:%3d |     %3d:%3d:%3d |     %3d:%3d:%3d", _tmprt3_( ps, SPN6, SBN6, PBR6 ));
  printf("\n                        |                 |                 |                ");
  printf("\n -trith             %3d |             %3d |             %3d |             %3d", _tmprt1_( ps,TriTh ));
  printf("\n -nXsfn %3d:%3d:%3d:%3d | %3d:%3d:%3d:%3d | %3d:%3d:%3d:%3d | %3d:%3d:%3d:%3d", _tmprt4_( ps,SFN0, SFN1, SFN2, SFN3 ));
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
  printf("\n -mXid                  |     %3d:%3d:%3d |     %3d:%3d:%3d +-----------------",_tmprt3y( pm, MAI0,   MAI1,   MAI2   ));
  printf("\n                        |                 |                 |   -sfc      %3d",                            pX->NRc.SFC );
  printf("\n -mXdb                  |         %3d:%3d |         %3d:%3d |   -tfc      %3d", _tmprt2y( pm, MADK,MABR ), pX->NRc.TFC );
  printf("\n -madz                  |             %3d |             %3d |                ",  pm[0].MADZ, pm[1].MADZ                );
  printf("\n                        |                 |                 |   -csfs     %3d",                            pX->NRc.CSFS);
  printf("\n -math                  |             %3d |             %3d |   -csfr     %3d",  pm[0].MATH, pm[1].MATH,   pX->NRc.CSFR);
  printf("\n -mate                  |             %3d |             %3d |   -ciir     %3d",  pm[0].MATE, pm[1].MATE,   pX->NRc.CIIR);
  printf("\n -mabw                  |             %3d |             %3d |                ",  pm[0].MABW, pm[1].MABW                );
  printf("\n -matw                  |             %3d |             %3d |   -ctfs     %3d",  pm[0].MATW, pm[1].MATW,   pX->NRc.CTFS);
  printf("\n -masw                  |             %3d |             %3d |   -ctfp     %3d",  pm[0].MASW, pm[1].MASW,   pX->NRc.TFC );
  printf("\n -maxn                  |             %3d |             %3d |   -ctfr     %3d",  pm[0].MAXN, pm[1].MAXN,   pX->NRc.CTFR);
  printf("\n*******************************************************************************\n");
}

void PrintNRxVPSS( const VPSS_NRX_V1_S *pX )
{
  const  tV56aIEy*  pi = pX->IEy;
  const  tV56aSFy*  ps = pX->SFy;
  const  tV56aMDy*  pm = pX->MDy;
  const  tV56aTFy*  pt = pX->TFy;

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

  printf("\n\n                             sizeof( X ) = %d ", sizeof(*pX) );
  printf("\n*******************************************************************************");
  printf("\n -en                %3d |             %3d |             %3d |             %3d",  _tmprt1_( ps, NRyEn ));
  printf("\n                        |                 |                 |                ");
  printf("\n -nXsf2     %3d:%3d:%3d |     %3d:%3d:%3d |     %3d:%3d:%3d |     %3d:%3d:%3d", _tmprt3_( ps, SFS2, SFT2, SBR2 ));
  printf("\n                        |                 |                 |                ");
  printf("\n -nXsf4     %3d:%3d:%3d |     %3d:%3d:%3d |     %3d:%3d:%3d |     %3d:%3d:%3d", _tmprt3_( ps, SFS4, SFT4, SBR4 ));
  printf("\n -bwsf4             %3d |             %3d |             %3d |             %3d", _tmprt1_( ps, BWSF4 ));
  printf("\n -kmsf4                 |                 |             %3d |             %3d", ps[2].kMode, ps[3].kMode);
  printf("\n                        |                 |                 |                ");
  printf("\n -nXsf5 %3d:%3d:%3d:%3d |                 |                 | %3d:%3d:%3d:%3d", _t4_( pi,0, IES0, IES1, IES2, IES3 ),_t4_( pi,1, IES0, IES1, IES2, IES3 ));
  printf("\n -dzsf5             %3d |                 |                 |             %3d",       pi[0].IEDZ, pi[1].IEDZ );
  printf("\n                        |                 |                 |                ");
  printf("\n -nXsf6     %3d:%3d:%3d |     %3d:%3d:%3d |     %3d:%3d:%3d |     %3d:%3d:%3d", _tmprt3_( ps, SPN6, SBN6, PBR6 ));
  printf("\n                        |                 |                 |                ");
  printf("\n -trith             %3d |             %3d |             %3d |             %3d", _tmprt1_( ps,TriTh ));
  printf("\n -nXsfn %3d:%3d:%3d:%3d | %3d:%3d:%3d:%3d | %3d:%3d:%3d:%3d | %3d:%3d:%3d:%3d", _tmprt4_( ps,SFN0, SFN1, SFN2, SFN3 ));
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
  printf("\n -mXid                  |     %3d:%3d:%3d |     %3d:%3d:%3d +-----------------",_tmprt3y( pm, MAI0,   MAI1,   MAI2   ));
  printf("\n                        |                 |                 |   -sfc      %3d",                            pX->NRc.SFC );
  printf("\n -mXdb                  |         %3d:%3d |         %3d:%3d |   -tfc      %3d", _tmprt2y( pm, MADK,MABR ), pX->NRc.TFC );
  printf("\n -madz                  |             %3d |             %3d |                ",  pm[0].MADZ, pm[1].MADZ                );
  printf("\n                        |                 |                 |   -csfs     %3d",                            pX->NRc.CSFS);
  printf("\n -math                  |             %3d |             %3d |   -csfr     %3d",  pm[0].MATH, pm[1].MATH,   pX->NRc.CSFR);
  printf("\n -mate                  |             %3d |             %3d |   -ciir     %3d",  pm[0].MATE, pm[1].MATE,   pX->NRc.CIIR);
  printf("\n -mabw                  |             %3d |             %3d |                ",  pm[0].MABW, pm[1].MABW                );
  printf("\n -matw                  |             %3d |             %3d |   -ctfs     %3d",  pm[0].MATW, pm[1].MATW,   pX->NRc.CTFS);
  printf("\n -masw                  |             %3d |             %3d |   -ctfp     %3d",  pm[0].MASW, pm[1].MASW,   pX->NRc.TFC );
  printf("\n -maxn                  |             %3d |             %3d |   -ctfr     %3d",  pm[0].MAXN, pm[1].MAXN,   pX->NRc.CTFR);
  printf("\n*******************************************************************************\n");
}

HI_S32 HI_SCENE_SetAeWeightTab(VI_PIPE ViPipe, HI_U8 u8Index)
{
    HI_S32 i, j = 0;
    HI_S32 s32Ret = HI_SUCCESS;

    ISP_STATISTICS_CFG_S stStatisticsCfg;

    s32Ret = HI_MPI_ISP_GetStatisticsConfig(ViPipe, &stStatisticsCfg);
    CHECK_SCENE_RET(s32Ret);

    for (i = 0; i < AE_ZONE_ROW; i++)
    {
        for (j = 0; j < AE_ZONE_COLUMN; j++)
        {
            stStatisticsCfg.stAECfg.au8Weight[i][j] = g_astScenePipeParam[u8Index].stStaticStatistics.au8AEWeight[i][j];
        }
    }

    s32Ret = HI_MPI_ISP_SetStatisticsConfig(ViPipe, &stStatisticsCfg);
    CHECK_SCENE_RET(s32Ret);
    return HI_SUCCESS;
}

HI_S32 HI_SCENE_SetDynamicAE(VI_PIPE ViPipe, HI_U64 u64Exposure, HI_U64 u64LastExposure, HI_U8 u8Index)
{
    if (HI_TRUE != g_astScenePipeParam[u8Index].stModuleState.bDynamicAE)
    {
        return HI_SUCCESS;
    }

    HI_U32 u32ExpLevel = 0;
    HI_S32 s32Ret = HI_SUCCESS;
	ISP_PUB_ATTR_S stPubAttr;
    ISP_INNER_STATE_INFO_S stInnerStateInfo;
    ISP_EXP_INFO_S stIspExpInfo;

    if (u64Exposure != u64LastExposure)
    {
        ISP_EXPOSURE_ATTR_S stExposureAttr;
        CHECK_SCENE_PAUSE();

        s32Ret = HI_MPI_ISP_GetPubAttr(ViPipe, &stPubAttr);
        CHECK_SCENE_RET(s32Ret);


        s32Ret = HI_MPI_ISP_QueryInnerStateInfo(ViPipe, &stInnerStateInfo);
        CHECK_SCENE_RET(s32Ret);


    	s32Ret = HI_MPI_ISP_QueryExposureInfo(ViPipe, &stIspExpInfo);
        CHECK_SCENE_RET(s32Ret);

        if ((WDR_MODE_NONE != stPubAttr.enWDRMode)&&(stIspExpInfo.u32ISO<= 110) && (u32RefExpRatio >= g_astScenePipeParam[u8Index].stStaticWdrExposure.u32RefRatioDn)
			&& (u32RefExpRatio <= g_astScenePipeParam[u8Index].stStaticWdrExposure.u32RefRatioUp) && (u64Exposure <= g_astScenePipeParam[u8Index].stStaticWdrExposure.u32ExpTh))
        {
            s32Ret = HI_MPI_ISP_GetExposureAttr(ViPipe, &stExposureAttr);
            CHECK_SCENE_RET(s32Ret);

            if (u64Exposure > u64LastExposure)
            {
                u32ExpLevel = SCENE_GetLevelLtoH(u64Exposure, g_astScenePipeParam[u8Index].stDynamicWdrExposure.u8AEExposureCnt, g_astScenePipeParam[u8Index].stDynamicWdrExposure.au64ExpLtoHThresh);
            }
            else
            {
                u32ExpLevel = SCENE_GetLevelHtoL(u64Exposure, g_astScenePipeParam[u8Index].stDynamicWdrExposure.u8AEExposureCnt, g_astScenePipeParam[u8Index].stDynamicWdrExposure.au64ExpHtoLThresh);
            }

            if (u32ExpLevel == g_astScenePipeParam[u8Index].stDynamicWdrExposure.u8AEExposureCnt - 1)
            {
                stExposureAttr.stAuto.u8Compensation = g_astScenePipeParam[u8Index].stDynamicWdrExposure.ExpCompensation[u32ExpLevel];
                stExposureAttr.stAuto.u8MaxHistOffset = g_astScenePipeParam[u8Index].stDynamicWdrExposure.MaxHistOffset[u32ExpLevel];
            }
            else
            {
                stExposureAttr.stAuto.u8Compensation = SCENE_Interpulate(u64Exposure,
                                                       g_astScenePipeParam[u8Index].stDynamicWdrExposure.au64ExpLtoHThresh[u32ExpLevel],
                                                       g_astScenePipeParam[u8Index].stDynamicWdrExposure.ExpCompensation[u32ExpLevel],
                                                       g_astScenePipeParam[u8Index].stDynamicWdrExposure.au64ExpLtoHThresh[u32ExpLevel + 1],
                                                       g_astScenePipeParam[u8Index].stDynamicWdrExposure.ExpCompensation[u32ExpLevel + 1]);

                stExposureAttr.stAuto.u8MaxHistOffset = SCENE_Interpulate(u64Exposure,
                                                        g_astScenePipeParam[u8Index].stDynamicWdrExposure.au64ExpLtoHThresh[u32ExpLevel],
                                                        g_astScenePipeParam[u8Index].stDynamicWdrExposure.MaxHistOffset[u32ExpLevel],
                                                        g_astScenePipeParam[u8Index].stDynamicWdrExposure.au64ExpLtoHThresh[u32ExpLevel + 1],
                                                        g_astScenePipeParam[u8Index].stDynamicWdrExposure.MaxHistOffset[u32ExpLevel + 1]);

            }
        }
        else
        {
            s32Ret = HI_MPI_ISP_GetExposureAttr(ViPipe, &stExposureAttr);
            CHECK_SCENE_RET(s32Ret);

            if (u64Exposure > u64LastExposure)
            {
                u32ExpLevel = SCENE_GetLevelLtoH(u64Exposure, g_astScenePipeParam[u8Index].stDynamicAe.u8AEExposureCnt, g_astScenePipeParam[u8Index].stDynamicAe.au64ExpLtoHThresh);
            }
            else
            {
                u32ExpLevel = SCENE_GetLevelHtoL(u64Exposure, g_astScenePipeParam[u8Index].stDynamicAe.u8AEExposureCnt, g_astScenePipeParam[u8Index].stDynamicAe.au64ExpHtoLThresh);
            }

            if (u32ExpLevel == g_astScenePipeParam[u8Index].stDynamicAe.u8AEExposureCnt - 1)
            {
                stExposureAttr.stAuto.u8Compensation = g_astScenePipeParam[u8Index].stDynamicAe.au8AutoCompensation[u32ExpLevel];
                stExposureAttr.stAuto.u8MaxHistOffset = g_astScenePipeParam[u8Index].stDynamicAe.au8AutoMaxHistOffset[u32ExpLevel];
            }
            else
            {
                stExposureAttr.stAuto.u8Compensation = SCENE_Interpulate(u64Exposure,
                                                       g_astScenePipeParam[u8Index].stDynamicAe.au64ExpLtoHThresh[u32ExpLevel],
                                                       g_astScenePipeParam[u8Index].stDynamicAe.au8AutoCompensation[u32ExpLevel],
                                                       g_astScenePipeParam[u8Index].stDynamicAe.au64ExpLtoHThresh[u32ExpLevel + 1],
                                                       g_astScenePipeParam[u8Index].stDynamicAe.au8AutoCompensation[u32ExpLevel + 1]);

                stExposureAttr.stAuto.u8MaxHistOffset = SCENE_Interpulate(u64Exposure,
                                                        g_astScenePipeParam[u8Index].stDynamicAe.au64ExpLtoHThresh[u32ExpLevel],
                                                        g_astScenePipeParam[u8Index].stDynamicAe.au8AutoMaxHistOffset[u32ExpLevel],
                                                        g_astScenePipeParam[u8Index].stDynamicAe.au64ExpLtoHThresh[u32ExpLevel + 1],
                                                        g_astScenePipeParam[u8Index].stDynamicAe.au8AutoMaxHistOffset[u32ExpLevel + 1]);

            }
        }

        CHECK_SCENE_PAUSE();
        s32Ret = HI_MPI_ISP_SetExposureAttr(ViPipe, &stExposureAttr);
        CHECK_SCENE_RET(s32Ret);
    }

    return HI_SUCCESS;
}

HI_S32 HI_SCENE_SetStaticAE(VI_PIPE ViPipe, HI_U8 u8Index)
{
	if(HI_TRUE != g_astScenePipeParam[u8Index].stModuleState.bStaticAE)
	{
		return HI_SUCCESS;
	}

	HI_S32 i = 0;
    HI_S32 s32Ret = HI_SUCCESS;

    ISP_PUB_ATTR_S stPubAttr;
    ISP_EXPOSURE_ATTR_S stExposureAttr;
    ISP_AE_ROUTE_EX_S stAeRouteEx;

    s32Ret = HI_MPI_ISP_GetAERouteAttrEx(ViPipe, &stAeRouteEx);
    CHECK_SCENE_RET(s32Ret);

    stAeRouteEx.u32TotalNum = g_astScenePipeParam[u8Index].stStaticAeRouteEx.u32TotalNum;
    for (i = 0; i < stAeRouteEx.u32TotalNum; i++)
    {
        stAeRouteEx.astRouteExNode[i].u32IntTime = g_astScenePipeParam[u8Index].stStaticAeRouteEx.au32IntTime[i];
        stAeRouteEx.astRouteExNode[i].u32Again= g_astScenePipeParam[u8Index].stStaticAeRouteEx.au32Again[i];
        stAeRouteEx.astRouteExNode[i].u32Dgain= g_astScenePipeParam[u8Index].stStaticAeRouteEx.au32Dgain[i];
        stAeRouteEx.astRouteExNode[i].u32IspDgain= g_astScenePipeParam[u8Index].stStaticAeRouteEx.au32IspDgain[i];
	}

    s32Ret = HI_MPI_ISP_SetAERouteAttrEx(ViPipe, &stAeRouteEx);
    CHECK_SCENE_RET(s32Ret);

    s32Ret = HI_MPI_ISP_GetExposureAttr(ViPipe, &stExposureAttr);
    CHECK_SCENE_RET(s32Ret);

    s32Ret = HI_MPI_ISP_GetPubAttr(ViPipe, &stPubAttr);
    CHECK_SCENE_RET(s32Ret);

    i = g_astScenePipeParam[u8Index].stStaticAeRouteEx.u32TotalNum - 1;
    stExposureAttr.stAuto.stExpTimeRange.u32Max = g_astScenePipeParam[u8Index].stStaticAeRouteEx.au32IntTime[i];

    if (WDR_MODE_NONE != stPubAttr.enWDRMode)
    {
    	stExposureAttr.stAuto.stDGainRange.u32Max = g_astScenePipeParam[u8Index].stStaticAeRouteEx.au32Dgain[i];
        stExposureAttr.stAuto.stISPDGainRange.u32Max = g_astScenePipeParam[u8Index].stStaticAeRouteEx.au32IspDgain[i];
	}

    stExposureAttr.bAERouteExValid = g_astScenePipeParam[u8Index].stStaticAe.bAERouteExValid;
    stExposureAttr.u8AERunInterval = g_astScenePipeParam[u8Index].stStaticAe.u8AERunInterval;
    stExposureAttr.stAuto.stSysGainRange.u32Max = g_astScenePipeParam[u8Index].stStaticAe.u32AutoSysGainMax;
    stExposureAttr.stAuto.u8Speed = g_astScenePipeParam[u8Index].stStaticAe.u8AutoSpeed;
    stExposureAttr.stAuto.u8Tolerance = g_astScenePipeParam[u8Index].stStaticAe.u8AutoTolerance;
    stExposureAttr.stAuto.stAEDelayAttr.u16BlackDelayFrame = g_astScenePipeParam[u8Index].stStaticAe.u16AutoBlackDelayFrame;
    stExposureAttr.stAuto.stAEDelayAttr.u16WhiteDelayFrame = g_astScenePipeParam[u8Index].stStaticAe.u16AutoWhiteDelayFrame;

    s32Ret = HI_MPI_ISP_SetExposureAttr(ViPipe, &stExposureAttr);
    CHECK_SCENE_RET(s32Ret);

    if (g_astScenePipeParam[u8Index].stModuleState.bAeWeightTab)
    {
        s32Ret = HI_SCENE_SetAeWeightTab(u8Index, ViPipe);
        CHECK_SCENE_RET(s32Ret);
    }

    return HI_SUCCESS;
}

HI_S32 HI_SCENE_SetStaticAWB(VI_PIPE ViPipe, HI_U8 u8Index)
{
	if(HI_TRUE != g_astScenePipeParam[u8Index].stModuleState.bStaticAWB)
	{
		return HI_SUCCESS;
	}

    HI_S32 i = 0;
    HI_S32 s32Ret = HI_SUCCESS;

    ISP_WB_ATTR_S stWbAttr;

    s32Ret = HI_MPI_ISP_GetWBAttr(ViPipe, &stWbAttr);
    CHECK_SCENE_RET(s32Ret);

    for (i = 0; i < 4; i++)
    {
        stWbAttr.stAuto.au16StaticWB[i] = g_astScenePipeParam[u8Index].stStaticAwb.au16AutoStaticWB[i];
    }

    for (i = 0; i < 6; i++)
    {
        stWbAttr.stAuto.as32CurvePara[i] = g_astScenePipeParam[u8Index].stStaticAwb.as32AutoCurvePara[i];
    }

    stWbAttr.stAuto.u16Speed = g_astScenePipeParam[u8Index].stStaticAwb.u16AutoSpeed;
    stWbAttr.stAuto.u16LowColorTemp = g_astScenePipeParam[u8Index].stStaticAwb.u16AutoLowColorTemp;

    for (i = 0; i < ISP_AUTO_ISO_STRENGTH_NUM; i++)
    {
        stWbAttr.stAuto.stCbCrTrack.au16CrMax[i] = g_astScenePipeParam[u8Index].stStaticAwb.au16AutoCrMax[i];
        stWbAttr.stAuto.stCbCrTrack.au16CrMin[i] = g_astScenePipeParam[u8Index].stStaticAwb.au16AutoCrMin[i];
        stWbAttr.stAuto.stCbCrTrack.au16CbMax[i] = g_astScenePipeParam[u8Index].stStaticAwb.au16AutoCbMax[i];
        stWbAttr.stAuto.stCbCrTrack.au16CbMin[i] = g_astScenePipeParam[u8Index].stStaticAwb.au16AutoCbMin[i];
    }

    s32Ret = HI_MPI_ISP_SetWBAttr(ViPipe, &stWbAttr);
    CHECK_SCENE_RET(s32Ret);
    return HI_SUCCESS;
}

HI_S32 HI_SCENE_SetStaticAWBEX(VI_PIPE ViPipe, HI_U8 u8Index)
{
	if(HI_TRUE != g_astScenePipeParam[u8Index].stModuleState.bStaticAWBEx)
	{
		return HI_SUCCESS;
	}

    HI_S32 i = 0;
    HI_S32 s32Ret = HI_SUCCESS;

    ISP_AWB_ATTR_EX_S stAwbAttrEx;

    s32Ret = HI_MPI_ISP_GetAWBAttrEx(ViPipe, &stAwbAttrEx);
    CHECK_SCENE_RET(s32Ret);

    stAwbAttrEx.u8Tolerance = g_astScenePipeParam[u8Index].stStaticAwbEx.u8Tolerance;
    stAwbAttrEx.stInOrOut.u32OutThresh = g_astScenePipeParam[u8Index].stStaticAwbEx.u32OutThresh;
    stAwbAttrEx.stInOrOut.u16LowStop = g_astScenePipeParam[u8Index].stStaticAwbEx.u16LowStop;
    stAwbAttrEx.stInOrOut.u16HighStart = g_astScenePipeParam[u8Index].stStaticAwbEx.u16HighStart;
    stAwbAttrEx.stInOrOut.u16HighStop = g_astScenePipeParam[u8Index].stStaticAwbEx.u16HighStop;
    stAwbAttrEx.bMultiLightSourceEn = g_astScenePipeParam[u8Index].stStaticAwbEx.bMultiLightSourceEn;

    for (i = 0; i < 8; i++)
    {
        stAwbAttrEx.au16MultiCTWt[i] = g_astScenePipeParam[u8Index].stStaticAwbEx.au16MultiCTWt[i];
    }

    s32Ret = HI_MPI_ISP_SetAWBAttrEx(ViPipe, &stAwbAttrEx);
    CHECK_SCENE_RET(s32Ret);

    return HI_SUCCESS;
}

HI_S32 HI_SCENE_SetStaticGlobalCac(VI_PIPE ViPipe, HI_U8 u8Index)
{
	if(HI_TRUE != g_astScenePipeParam[u8Index].stModuleState.bStaticGlobalCac)
	{
		return HI_SUCCESS;
	}

    HI_S32 s32Ret = HI_SUCCESS;

    ISP_GLOBAL_CAC_ATTR_S stGlobalCac;

    s32Ret = HI_MPI_ISP_GetGlobalCacAttr(ViPipe, &stGlobalCac);
    CHECK_SCENE_RET(s32Ret);

    stGlobalCac.bEnable = g_astScenePipeParam[u8Index].stStaticGlobalCac.bEnable;
    stGlobalCac.u16VerCoordinate = g_astScenePipeParam[u8Index].stStaticGlobalCac.u16VerCoordinate;
    stGlobalCac.u16HorCoordinate = g_astScenePipeParam[u8Index].stStaticGlobalCac.u16HorCoordinate;
    stGlobalCac.s16ParamRedA = g_astScenePipeParam[u8Index].stStaticGlobalCac.s16ParamRedA;
    stGlobalCac.s16ParamRedB = g_astScenePipeParam[u8Index].stStaticGlobalCac.s16ParamRedB;
    stGlobalCac.s16ParamRedC = g_astScenePipeParam[u8Index].stStaticGlobalCac.s16ParamRedC;
    stGlobalCac.s16ParamBlueA = g_astScenePipeParam[u8Index].stStaticGlobalCac.s16ParamBlueA;
    stGlobalCac.s16ParamBlueB = g_astScenePipeParam[u8Index].stStaticGlobalCac.s16ParamBlueB;
    stGlobalCac.s16ParamBlueC = g_astScenePipeParam[u8Index].stStaticGlobalCac.s16ParamBlueC;
    stGlobalCac.u8VerNormShift = g_astScenePipeParam[u8Index].stStaticGlobalCac.u8VerNormShift;
    stGlobalCac.u8VerNormFactor = g_astScenePipeParam[u8Index].stStaticGlobalCac.u8VerNormFactor;
    stGlobalCac.u8HorNormShift = g_astScenePipeParam[u8Index].stStaticGlobalCac.u8HorNormShift;
    stGlobalCac.u8HorNormFactor = g_astScenePipeParam[u8Index].stStaticGlobalCac.u8HorNormFactor;
    stGlobalCac.u16CorVarThr = g_astScenePipeParam[u8Index].stStaticGlobalCac.u16CorVarThr;

    s32Ret = HI_MPI_ISP_SetGlobalCacAttr(ViPipe, &stGlobalCac);
    CHECK_SCENE_RET(s32Ret);

    return HI_SUCCESS;
}

HI_S32 HI_SCENE_SetStaticLocalCac(VI_PIPE ViPipe, HI_U8 u8Index)
{
	if(HI_TRUE != g_astScenePipeParam[u8Index].stModuleState.bStaticLocalCac)
	{
		return HI_SUCCESS;
	}

    HI_S32 s32Ret = HI_SUCCESS;
	ISP_LOCAL_CAC_ATTR_S  stLocalCac;

    s32Ret = HI_MPI_ISP_GetLocalCacAttr(ViPipe, &stLocalCac);
    CHECK_SCENE_RET(s32Ret);

    stLocalCac.bEnable = g_astScenePipeParam[u8Index].stStaticLocalCac.bEnable;
	stLocalCac.u16PurpleDetRange= g_astScenePipeParam[u8Index].stStaticLocalCac.u16PurpleDetRange;
	stLocalCac.u16VarThr= g_astScenePipeParam[u8Index].stStaticLocalCac.u16VarThr;

    s32Ret = HI_MPI_ISP_SetLocalCacAttr(ViPipe, &stLocalCac);
    CHECK_SCENE_RET(s32Ret);
    return HI_SUCCESS;
}


HI_S32 HI_SCENE_SetStaticWDRExposure(VI_PIPE ViPipe, HI_U8 u8Index)
{
	if(HI_TRUE != g_astScenePipeParam[u8Index].stModuleState.bStaticWdrExposure)
	{
		return HI_SUCCESS;
	}

    HI_S32 i = 0;
    HI_S32 s32Ret = HI_SUCCESS;

    ISP_WDR_EXPOSURE_ATTR_S stWdrExposureAttr;

    s32Ret = HI_MPI_ISP_GetWDRExposureAttr(ViPipe, &stWdrExposureAttr);
    CHECK_SCENE_RET(s32Ret);

    stWdrExposureAttr.enExpRatioType = (ISP_OP_TYPE_E)g_astScenePipeParam[u8Index].stStaticWdrExposure.u8ExpRatioType;
    stWdrExposureAttr.u32ExpRatioMax = g_astScenePipeParam[u8Index].stStaticWdrExposure.u32ExpRatioMax;
    stWdrExposureAttr.u32ExpRatioMin = g_astScenePipeParam[u8Index].stStaticWdrExposure.u32ExpRatioMin;

    for (i = 0; i < EXP_RATIO_NUM; i++)
    {
        stWdrExposureAttr.au32ExpRatio[i] = g_astScenePipeParam[u8Index].stStaticWdrExposure.au32ExpRatio[i];
    }

    s32Ret = HI_MPI_ISP_SetWDRExposureAttr(ViPipe, &stWdrExposureAttr);
    CHECK_SCENE_RET(s32Ret);
    return HI_SUCCESS;
}

HI_S32 HI_SCENE_SetDynamicWdrExposure(VI_PIPE ViPipe, HI_U64 u64Exposure, HI_U64 u64LastExposure, HI_U8 u8Index)
{
#if 0
	if(HI_TRUE != g_astScenePipeParam[u8Index].stModuleState.bDynamicWdrExposure)
	{
		return HI_SUCCESS;
	}

    HI_U32 u32ExpLevel = 0;
    HI_S32 s32Ret = HI_SUCCESS;

    if (u64Exposure != u64LastExposure)
    {
        ISP_WDR_EXPOSURE_ATTR_S stWdrExposureAttr;

        CHECK_SCENE_PAUSE();
        s32Ret = HI_MPI_ISP_GetWDRExposureAttr(ViPipe, &stWdrExposureAttr);
        CHECK_SCENE_RET(s32Ret);

        if (u64Exposure > u64LastExposure)
        {
            u32ExpLevel = SCENE_GetLevelLtoH(u64Exposure, g_astScenePipeParam[u8Index].stDynamicWdrExposure.u8AEExposureCnt, g_astScenePipeParam[u8Index].stDynamicWdrExposure.au64ExpLtoHThresh);
        }
        else
        {
            u32ExpLevel = SCENE_GetLevelHtoL(u64Exposure, g_astScenePipeParam[u8Index].stDynamicWdrExposure.u8AEExposureCnt, g_astScenePipeParam[u8Index].stDynamicWdrExposure.au64ExpHtoLThresh);
        }

        if (u32ExpLevel == g_astScenePipeParam[u8Index].stDynamicWdrExposure.u8AEExposureCnt - 1)
        {
            stWdrExposureAttr.u32ExpRatioMax = g_astScenePipeParam[u8Index].stDynamicWdrExposure.au32AutoExpRatioMax[u32ExpLevel];
            stWdrExposureAttr.u32ExpRatioMin = g_astScenePipeParam[u8Index].stDynamicWdrExposure.au32AutoExpRatioMin[u32ExpLevel];
        }
        else
        {
            stWdrExposureAttr.u32ExpRatioMax = SCENE_Interpulate(u64Exposure,
                                               g_astScenePipeParam[u8Index].stDynamicWdrExposure.au64ExpLtoHThresh[u32ExpLevel],
                                               g_astScenePipeParam[u8Index].stDynamicWdrExposure.au32AutoExpRatioMax[u32ExpLevel],
                                               g_astScenePipeParam[u8Index].stDynamicWdrExposure.au64ExpLtoHThresh[u32ExpLevel + 1],
                                               g_astScenePipeParam[u8Index].stDynamicWdrExposure.au32AutoExpRatioMax[u32ExpLevel + 1]);
            stWdrExposureAttr.u32ExpRatioMin = SCENE_Interpulate(u64Exposure,
                                               g_astScenePipeParam[u8Index].stDynamicWdrExposure.au64ExpLtoHThresh[u32ExpLevel],
                                               g_astScenePipeParam[u8Index].stDynamicWdrExposure.au32AutoExpRatioMin[u32ExpLevel],
                                               g_astScenePipeParam[u8Index].stDynamicWdrExposure.au64ExpLtoHThresh[u32ExpLevel + 1],
                                               g_astScenePipeParam[u8Index].stDynamicWdrExposure.au32AutoExpRatioMin[u32ExpLevel + 1]);
        }

        CHECK_SCENE_PAUSE();
        s32Ret = HI_MPI_ISP_SetWDRExposureAttr(ViPipe, &stWdrExposureAttr);
        CHECK_SCENE_RET(s32Ret);
    }
#endif
    return HI_SUCCESS;
}

HI_S32 HI_SCENE_SetDynamicFsWdr(VI_PIPE ViPipe, HI_U32 u32ISO, HI_U32 u32LastISO, HI_U8 u8Index, HI_U32 u32WdrRatio)
{
    if (HI_TRUE != g_astScenePipeParam[u8Index].stModuleState.bDynamicFSWDR)
    {
        return HI_SUCCESS;
    }

    HI_S32 s32Ret = HI_SUCCESS;

    ISP_WDR_FS_ATTR_S stFSWDRAttr;

    CHECK_SCENE_PAUSE();
    s32Ret = HI_MPI_ISP_GetFSWDRAttr(ViPipe, &stFSWDRAttr);
    CHECK_SCENE_RET(s32Ret);

    if (u32ISO < 1600)
    {
        stFSWDRAttr.stWDRCombine.bMotionComp = HI_FALSE;
		stFSWDRAttr.enWDRMergeMode = MERGE_FUSION_MODE;
        stFSWDRAttr.stBnr.au8GsigmaGain[0] = 12;
        stFSWDRAttr.stBnr.au8RBsigmaGain[0] = 3;

        if(u32ISO < 150)
        {
           stFSWDRAttr.enWDRMergeMode = MERGE_WDR_MODE;
		   stFSWDRAttr.stWDRCombine.bMotionComp = HI_TRUE;
        }
    }
    else
    {
        stFSWDRAttr.stWDRCombine.bMotionComp = HI_FALSE;
        stFSWDRAttr.enWDRMergeMode = MERGE_FUSION_MODE;

        if (u32ISO >= 19200)
        {
            stFSWDRAttr.stBnr.au8GsigmaGain[0] = 0;
            stFSWDRAttr.stBnr.au8RBsigmaGain[0] = 0;
        }
        else
        {
            stFSWDRAttr.stBnr.au8GsigmaGain[0] = (HI_U8)(12 - (u32ISO / 1600));
            stFSWDRAttr.stBnr.au8RBsigmaGain[0] = (HI_U8)((3 >= (u32ISO / 1600)) ? (3 - (u32ISO / 1600)) : 0);
        }
    }

    CHECK_SCENE_PAUSE();
    s32Ret = HI_MPI_ISP_SetFSWDRAttr(ViPipe, &stFSWDRAttr);
    CHECK_SCENE_RET(s32Ret);

    return HI_SUCCESS;
}

HI_S32 HI_SCENE_SetStaticDRC(VI_PIPE ViPipe, HI_U8 u8Index)
{
	if(HI_TRUE != g_astScenePipeParam[u8Index].stModuleState.bStaticDRC)
	{
		return HI_SUCCESS;
	}

    HI_S32 i = 0;
    HI_S32 s32Ret = HI_SUCCESS;

    ISP_DRC_ATTR_S stDrcAttr;

    s32Ret = HI_MPI_ISP_GetDRCAttr(ViPipe, &stDrcAttr);
    CHECK_SCENE_RET(s32Ret);

    stDrcAttr.bEnable = g_astScenePipeParam[u8Index].stStaticDrc.bEnable;
    stDrcAttr.enCurveSelect = (ISP_DRC_CURVE_SELECT_E)g_astScenePipeParam[u8Index].stStaticDrc.u8CurveSelect;
    stDrcAttr.enOpType = (ISP_OP_TYPE_E)g_astScenePipeParam[u8Index].stStaticDrc.u8DRCOpType;
    stDrcAttr.stAuto.u16Strength = g_astScenePipeParam[u8Index].stStaticDrc.u16AutoStrength;
    stDrcAttr.stAuto.u16StrengthMin = g_astScenePipeParam[u8Index].stStaticDrc.u16AutoStrengthMin;
    stDrcAttr.stAuto.u16StrengthMax = g_astScenePipeParam[u8Index].stStaticDrc.u16AutoStrengthMax;

    for (i = 0; i < HI_ISP_DRC_TM_NODE_NUM; i++)
    {
        stDrcAttr.au16ToneMappingValue[i] = g_astScenePipeParam[u8Index].stStaticDrc.au16ToneMappingValue[i];
    }

    s32Ret = HI_MPI_ISP_SetDRCAttr(ViPipe, &stDrcAttr);
    CHECK_SCENE_RET(s32Ret);
    return HI_SUCCESS;
}

#define HI_SCENE_REF_EXP_RATIO_FRM   (16)
HI_S32 HI_SCENE_SetDynamicDRC(VI_PIPE ViPipe, HI_U8 u8Index, HI_U32 u32WdrRatio, HI_U32 u32Iso, HI_U64 u64Exposure)
{
	if(HI_TRUE != g_astScenePipeParam[u8Index].stModuleState.bDynamicDrc)
	{
		return HI_SUCCESS;
	}

    HI_S32 i = 0;
    HI_S32 s32Ret = HI_SUCCESS;
    HI_U32 u32IsoLevel = 0;
    HI_U32 u32RatioLevel = 0;
    ISP_DRC_ATTR_S stIspDrcAttr;
    ISP_INNER_STATE_INFO_S stInnerStateInfo;
    ISP_AE_STATISTICS_S stAeStat;
    ISP_PUB_ATTR_S stPubAttr;
	ISP_SATURATION_ATTR_S stSaturationAttr;

    HI_S32 u32Ratiotmp1 = 0;//, u32LumAvertmp1 = 0;
	HI_U32 u32LumAver = 0, u32Ratiotmp = 0, u32Ratiotmp2 = 0, u32LumAvertmp2 = 0;
    float f32dfacotr = 0.0;
	float f32dfacotrSat = 0.0;

    static HI_U32 u32LastDrcStr[MAX_ISP_PIPE_NUM] = {0};
    static HI_U32 u32CurrDrcStr = 0;
    static HI_U8 u8LastDrcLocalMixingBrightMax[MAX_ISP_PIPE_NUM] = {0};
    static HI_U8 u8LastDrcLocalMixingBrightMin[MAX_ISP_PIPE_NUM] = {0};
    static HI_U8 u8LastLocalMixingDarkMax[MAX_ISP_PIPE_NUM] = {0};
    static HI_U8 u8LastLocalMixingDarkMin[MAX_ISP_PIPE_NUM] = {0};
    static HI_S8 s8LastDetailAdjustFactor[MAX_ISP_PIPE_NUM] = {0};
    static HI_U8 u8LastSpatialFltCoef[MAX_ISP_PIPE_NUM] = {0};
    static HI_U8 u8LastRangeFltCoef[MAX_ISP_PIPE_NUM] = {0};
    static HI_U8 u8LastGradRevMax[MAX_ISP_PIPE_NUM] = {0};
    static HI_U8 u8LastGradRevThr[MAX_ISP_PIPE_NUM] = {0};
    static HI_U8 u8LastCompress[MAX_ISP_PIPE_NUM] = {0};
    static HI_U8 u8LastStretch[MAX_ISP_PIPE_NUM] = {0};

    static HI_U8 u8CurLocalMixingBrightMax = 0;
    static HI_U8 u8CurLocalMixingBrightMin = 0;
    static HI_U8 u8CurLocalMixingDarkMax = 0;
    static HI_U8 u8CurLocalMixingDarkMin = 0;
    static HI_S8 s8CurDetailAdjustFactor = 0;
    static HI_U8 u8SpatialFltCoef = 0;
    static HI_U8 u8RangeFltCoef = 0;
    static HI_U8 u8GradRevMax = 0;
    static HI_U8 u8GradRevThr = 0;
    static HI_U8 u8CurCompress = 0;
    static HI_U8 u8CurStretch = 0;

    HI_U32 u32Interval = g_astScenePipeParam[u8Index].stDynamicDrc.u32Interval;

    CHECK_SCENE_PAUSE();
    s32Ret = HI_MPI_ISP_GetPubAttr(ViPipe, &stPubAttr);
    CHECK_SCENE_RET(s32Ret);

    CHECK_SCENE_PAUSE();
	s32Ret = HI_MPI_ISP_GetSaturationAttr(ViPipe, &stSaturationAttr);
    CHECK_SCENE_RET(s32Ret);

    if (WDR_MODE_NONE == stPubAttr.enWDRMode)
    {
        return HI_SUCCESS;
    }

    CHECK_SCENE_PAUSE();

    s32Ret = HI_MPI_ISP_GetAEStatistics(ViPipe, &stAeStat);
    CHECK_SCENE_RET(s32Ret);

    u32LumAver = (stAeStat.au16BEGlobalAvg[1] + stAeStat.au16BEGlobalAvg[2]) / 2;
    u32Ratiotmp1 = g_astScenePipeParam[u8Index].stStaticWdrExposure.u32ExpRatioMin - u32RefExpRatio;
    u32Ratiotmp = (u32Ratiotmp1 > 0) ? u32Ratiotmp1 : 0;

    for (i = 0; i < 2; i++)
    {
        DRC_strfilter[i] = DRC_strfilter[i + 1];
        u32Ratiotmp2 += DRC_strfilter[i];
    }

    DRC_strfilter[2] = u32Ratiotmp;

    u32Ratiotmp1 = (u32Ratiotmp2 + DRC_strfilter[2]) / 3;

    //printf("===DRC:LumAver=%d,u32RefExpRatio=%d,u32Ratiotmp1=%d,Expo = %lld ===\n", u32LumAver, u32RefExpRatio, u32Ratiotmp1,u64Exposure);

    s32Ret = HI_MPI_ISP_QueryInnerStateInfo(ViPipe, &stInnerStateInfo);
    CHECK_SCENE_RET(s32Ret);

    s32Ret = HI_MPI_ISP_GetDRCAttr(ViPipe, &stIspDrcAttr);
    CHECK_SCENE_RET(s32Ret);

    u8LastDrcLocalMixingBrightMax[ViPipe] = stIspDrcAttr.u8LocalMixingBrightMax;
    u8LastDrcLocalMixingBrightMin[ViPipe] = stIspDrcAttr.u8LocalMixingBrightMin;
    u8LastLocalMixingDarkMax[ViPipe] = stIspDrcAttr.u8LocalMixingDarkMax;
    u8LastLocalMixingDarkMin[ViPipe] = stIspDrcAttr.u8LocalMixingDarkMin;

    s8LastDetailAdjustFactor[ViPipe] = stIspDrcAttr.s8DetailAdjustFactor;
    u8LastSpatialFltCoef[ViPipe] = stIspDrcAttr.u8SpatialFltCoef;
    u8LastRangeFltCoef[ViPipe] = stIspDrcAttr.u8RangeFltCoef;
    u8LastGradRevMax[ViPipe] = stIspDrcAttr.u8GradRevMax;
    u8LastGradRevThr[ViPipe] = stIspDrcAttr.u8GradRevThr;
    u32LastDrcStr[ViPipe] = stIspDrcAttr.stAuto.u16Strength;
    u8LastCompress[ViPipe] = stIspDrcAttr.stAsymmetryCurve.u8Compress;
    u8LastStretch[ViPipe] = stIspDrcAttr.stAsymmetryCurve.u8Stretch;

    u32RatioLevel = SCENE_GetLevelLtoH_U32(u32Ratiotmp1, g_astScenePipeParam[u8Index].stDynamicDrc.u32RationCount, g_astScenePipeParam[u8Index].stDynamicDrc.au32RationLevel);
    u32IsoLevel = SCENE_GetLevelLtoH_U32(u32Iso, g_astScenePipeParam[u8Index].stDynamicDrc.u32ISOCount, g_astScenePipeParam[u8Index].stDynamicDrc.au32ISOLevel);

    u32CurrDrcStr = g_astScenePipeParam[u8Index].stDynamicDrc.u16AutoStrength[u32IsoLevel][u32RatioLevel];
	//printf("u32CurrDrcStr :%d +++++++ \n ",u32CurrDrcStr);

    //outdoor non-wdr scene, decrease DRC Str
#if 1
    f32dfacotr =  1.0;
	f32dfacotrSat = 1.2;

    if (u64Exposure < 9000)
    {
        //u32LumAvertmp1 = ((u32LumAver - 8200) > 0) ? (u32LumAver - 8200) : 0;

        //f32dfacotr = (float)((float)u32LumAvertmp1 / 8200);

		stSaturationAttr.stAuto.au8Sat[0] = 90 * f32dfacotrSat;
        
        u32LumAvertmp2 = (HI_U32)((float)u32CurrDrcStr * f32dfacotr);
		//printf("u32LumAvertmp2 : %d ++++++++\n",u32LumAvertmp2);
        u32CurrDrcStr = u32LumAvertmp2;
    }
	else if(u64Exposure >= 9000 && u64Exposure < 12000)
	{
	    u32CurrDrcStr = u32CurrDrcStr * (1.0 - f32dfacotr)/3000 * (u64Exposure - 9000) + u32CurrDrcStr * f32dfacotr;
		stSaturationAttr.stAuto.au8Sat[0] = 90 * f32dfacotrSat - 90 * (f32dfacotrSat - 1.0)/3000 * (u64Exposure - 9000);
	}
	else
	{
	    stSaturationAttr.stAuto.au8Sat[0] = 90;
	}
#endif

    s32Ret = HI_MPI_ISP_SetSaturationAttr(ViPipe, &stSaturationAttr);
    CHECK_SCENE_RET(s32Ret);

    if (u32IsoLevel == 0 || u32IsoLevel == g_astScenePipeParam[u8Index].stDynamicDrc.u32ISOCount - 1)
    {
        u8CurLocalMixingBrightMax = g_astScenePipeParam[u8Index].stDynamicDrc.au8LocalMixingBrightMax[u32IsoLevel];
        u8CurLocalMixingBrightMin = g_astScenePipeParam[u8Index].stDynamicDrc.au8LocalMixingBrightMin[u32IsoLevel];

        u8CurLocalMixingDarkMax = g_astScenePipeParam[u8Index].stDynamicDrc.au8LocalMixingDarkMax[u32IsoLevel];
        u8CurLocalMixingDarkMin = g_astScenePipeParam[u8Index].stDynamicDrc.au8LocalMixingDarkMin[u32IsoLevel];
        s8CurDetailAdjustFactor = g_astScenePipeParam[u8Index].stDynamicDrc.as8DetailAdjustFactor[u32IsoLevel];

        //non-wdr scene,s8CurDetailAdjustFactor adapt
        if (u32RefExpRatio < 400)
        {
            s8CurDetailAdjustFactor = s8CurDetailAdjustFactor - 3 ;
            s8CurDetailAdjustFactor = (s8CurDetailAdjustFactor > 0) ? s8CurDetailAdjustFactor : 0;

            if (u32LumAver > 12000)//outdoor
            {
                s8CurDetailAdjustFactor = s8CurDetailAdjustFactor - 3;
                s8CurDetailAdjustFactor = (s8CurDetailAdjustFactor > 0) ? s8CurDetailAdjustFactor : 0;
            }
        }

        u8SpatialFltCoef = g_astScenePipeParam[u8Index].stDynamicDrc.au8SpatialFltCoef[u32IsoLevel];
        u8RangeFltCoef = g_astScenePipeParam[u8Index].stDynamicDrc.au8RangeFltCoef[u32IsoLevel];
        u8GradRevMax = g_astScenePipeParam[u8Index].stDynamicDrc.au8GradRevMax[u32IsoLevel];
        u8GradRevThr = g_astScenePipeParam[u8Index].stDynamicDrc.au8GradRevThr[u32IsoLevel];
        u8CurCompress = g_astScenePipeParam[u8Index].stDynamicDrc.au8Compress[u32IsoLevel];
        u8CurStretch = g_astScenePipeParam[u8Index].stDynamicDrc.au8Stretch[u32IsoLevel];
    }
    else
    {
        u8CurLocalMixingBrightMax = SCENE_Interpulate(u32Iso , g_astScenePipeParam[u8Index].stDynamicDrc.au32ISOLevel[u32IsoLevel],
                                    g_astScenePipeParam[u8Index].stDynamicDrc.au8LocalMixingBrightMax[u32IsoLevel],
                                    g_astScenePipeParam[u8Index].stDynamicDrc.au32ISOLevel[u32IsoLevel + 1],
                                    g_astScenePipeParam[u8Index].stDynamicDrc.au8LocalMixingBrightMax[u32IsoLevel + 1]);
        u8CurLocalMixingBrightMin = SCENE_Interpulate(u32Iso , g_astScenePipeParam[u8Index].stDynamicDrc.au32ISOLevel[u32IsoLevel],
                                    g_astScenePipeParam[u8Index].stDynamicDrc.au8LocalMixingBrightMin[u32IsoLevel],
                                    g_astScenePipeParam[u8Index].stDynamicDrc.au32ISOLevel[u32IsoLevel + 1],
                                    g_astScenePipeParam[u8Index].stDynamicDrc.au8LocalMixingBrightMin[u32IsoLevel + 1]);
        u8CurLocalMixingDarkMax = SCENE_Interpulate(u32Iso , g_astScenePipeParam[u8Index].stDynamicDrc.au32ISOLevel[u32IsoLevel],
                                  g_astScenePipeParam[u8Index].stDynamicDrc.au8LocalMixingDarkMax[u32IsoLevel],
                                  g_astScenePipeParam[u8Index].stDynamicDrc.au32ISOLevel[u32IsoLevel + 1],
                                  g_astScenePipeParam[u8Index].stDynamicDrc.au8LocalMixingDarkMax[u32IsoLevel + 1]);
        u8CurLocalMixingDarkMin = SCENE_Interpulate(u32Iso , g_astScenePipeParam[u8Index].stDynamicDrc.au32ISOLevel[u32IsoLevel],
                                  g_astScenePipeParam[u8Index].stDynamicDrc.au8LocalMixingDarkMin[u32IsoLevel],
                                  g_astScenePipeParam[u8Index].stDynamicDrc.au32ISOLevel[u32IsoLevel + 1],
                                  g_astScenePipeParam[u8Index].stDynamicDrc.au8LocalMixingDarkMin[u32IsoLevel + 1]);
        s8CurDetailAdjustFactor = SCENE_Interpulate(u32Iso , g_astScenePipeParam[u8Index].stDynamicDrc.au32ISOLevel[u32IsoLevel],
                                  ABS(g_astScenePipeParam[u8Index].stDynamicDrc.as8DetailAdjustFactor[u32IsoLevel],0),
                                  g_astScenePipeParam[u8Index].stDynamicDrc.au32ISOLevel[u32IsoLevel + 1],
                                  ABS(g_astScenePipeParam[u8Index].stDynamicDrc.as8DetailAdjustFactor[u32IsoLevel + 1],0));

        if (u32RefExpRatio < 400 )
        {
            s8CurDetailAdjustFactor = s8CurDetailAdjustFactor - 3 ;
            s8CurDetailAdjustFactor = (s8CurDetailAdjustFactor > 0) ? s8CurDetailAdjustFactor : 0;

            if (u32LumAver > 12000)
            {
                s8CurDetailAdjustFactor = s8CurDetailAdjustFactor - 3;
                s8CurDetailAdjustFactor = (s8CurDetailAdjustFactor > 0) ? s8CurDetailAdjustFactor : 0;
            }
        }

        u8SpatialFltCoef = SCENE_Interpulate(u32Iso, g_astScenePipeParam[u8Index].stDynamicDrc.au32ISOLevel[u32IsoLevel],
                                             g_astScenePipeParam[u8Index].stDynamicDrc.au8SpatialFltCoef[u32IsoLevel],
                                             g_astScenePipeParam[u8Index].stDynamicDrc.au32ISOLevel[u32IsoLevel + 1],
                                             g_astScenePipeParam[u8Index].stDynamicDrc.au8SpatialFltCoef[u32IsoLevel + 1]);
        u8RangeFltCoef = SCENE_Interpulate(u32Iso, g_astScenePipeParam[u8Index].stDynamicDrc.au32ISOLevel[u32IsoLevel],
                                           g_astScenePipeParam[u8Index].stDynamicDrc.au8RangeFltCoef[u32IsoLevel],
                                           g_astScenePipeParam[u8Index].stDynamicDrc.au32ISOLevel[u32IsoLevel + 1],
                                           g_astScenePipeParam[u8Index].stDynamicDrc.au8RangeFltCoef[u32IsoLevel + 1]);
        u8GradRevMax = SCENE_Interpulate(u32Iso, g_astScenePipeParam[u8Index].stDynamicDrc.au32ISOLevel[u32IsoLevel],
                                         g_astScenePipeParam[u8Index].stDynamicDrc.au8GradRevMax[u32IsoLevel],
                                         g_astScenePipeParam[u8Index].stDynamicDrc.au32ISOLevel[u32IsoLevel + 1],
                                         g_astScenePipeParam[u8Index].stDynamicDrc.au8GradRevMax[u32IsoLevel + 1]);
        u8GradRevThr = SCENE_Interpulate(u32Iso, g_astScenePipeParam[u8Index].stDynamicDrc.au32ISOLevel[u32IsoLevel],
                                         g_astScenePipeParam[u8Index].stDynamicDrc.au8GradRevThr[u32IsoLevel],
                                         g_astScenePipeParam[u8Index].stDynamicDrc.au32ISOLevel[u32IsoLevel + 1],
                                         g_astScenePipeParam[u8Index].stDynamicDrc.au8GradRevThr[u32IsoLevel + 1]);
        u8CurCompress = SCENE_Interpulate(u32Iso, g_astScenePipeParam[u8Index].stDynamicDrc.au32ISOLevel[u32IsoLevel],
                                          g_astScenePipeParam[u8Index].stDynamicDrc.au8Compress[u32IsoLevel],
                                          g_astScenePipeParam[u8Index].stDynamicDrc.au32ISOLevel[u32IsoLevel + 1],
                                          g_astScenePipeParam[u8Index].stDynamicDrc.au8Compress[u32IsoLevel + 1]);
        u8CurStretch = SCENE_Interpulate(u32Iso, g_astScenePipeParam[u8Index].stDynamicDrc.au32ISOLevel[u32IsoLevel],
                                         g_astScenePipeParam[u8Index].stDynamicDrc.au8Stretch[u32IsoLevel],
                                         g_astScenePipeParam[u8Index].stDynamicDrc.au32ISOLevel[u32IsoLevel + 1],
                                         g_astScenePipeParam[u8Index].stDynamicDrc.au8Stretch[u32IsoLevel + 1]);

    }

    //printf("u32CurrDrcStr = %d u32ISO = %d IndexX = %d,IndexY=%d,LumAvertmp2=%d\n", u32CurrDrcStr, u32Iso, u32RatioLevel, u32IsoLevel, u32LumAvertmp2);

    if (u32Iso < 500 && u32WdrRatio >= g_astScenePipeParam[u8Index].stStaticWdrExposure.u32ExpRatioMin)
    {
        u8CurCompress = u8CurCompress + (HI_U8)((double)g_astScenePipeParam[u8Index].stStaticWdrExposure.u32ExpRatioMax / (double)(u32WdrRatio) * 4) ;
    }
    else
    {
        //stIspDrcAttr.u8LocalMixingDark = 64;
    }


    if (u8CurCompress < 0x64)
    {
        u8CurCompress = 0x64;
    }
    else if (u8CurCompress > 0xc8)
    {
        u8CurCompress = 0xc8;
    }

    if (u8CurStretch < 30)
    {
        u8CurStretch = 30;
    }
    else if (u8CurStretch > 60)
    {
        u8CurStretch = 60;
    }
	
	HI_U32 u32RefRatioLevel = 0;
	HI_U32 u32RefExpRatioAlpha = 0;
	HI_U16 au16ToneMappingValue[200] = {0};

	stIspDrcAttr.enCurveSelect = (ISP_DRC_CURVE_SELECT_E)g_astScenePipeParam[u8Index].stStaticDrc.u8CurveSelect;
	
	u32RefRatioLevel = SCENE_GetLevelLtoH_U32(stInnerStateInfo.u32WDRExpRatioActual[0], 
		g_astScenePipeParam[u8Index].stDynamicDrc.u32RefRatioCount, g_astScenePipeParam[u8Index].stDynamicDrc.au32RefRatioLtoH);
	if (u32RefRatioLevel == 0 || u32RefRatioLevel == g_astScenePipeParam[u8Index].stDynamicDrc.u32RefRatioCount - 1)
    {
        u32RefExpRatioAlpha = g_astScenePipeParam[u8Index].stDynamicDrc.u32RefRatioAlpha[u32RefRatioLevel];
    }
	else
    {
        u32RefExpRatioAlpha = SCENE_Interpulate(stInnerStateInfo.u32WDRExpRatioActual[0], g_astScenePipeParam[u8Index].stDynamicDrc.au32RefRatioLtoH[u32RefRatioLevel - 1],
                                 g_astScenePipeParam[u8Index].stDynamicDrc.u32RefRatioAlpha[u32RefRatioLevel - 1],
                                 g_astScenePipeParam[u8Index].stDynamicDrc.au32RefRatioLtoH[u32RefRatioLevel],
                                 g_astScenePipeParam[u8Index].stDynamicDrc.u32RefRatioAlpha[u32RefRatioLevel]);
    }

	printf("u32RefExpRatioAlpha = %d\n",u32RefExpRatioAlpha);

	for (i = 0 ; i < 200 ; i++)
    {
    	au16ToneMappingValue[i] = ((HI_U32)u32RefExpRatioAlpha * g_astScenePipeParam[u8Index].stDynamicDrc.au16ToneMappingValue2[i]
    		+ (HI_U32)(0x100 - u32RefExpRatioAlpha) * g_astScenePipeParam[u8Index].stDynamicDrc.au16ToneMappingValue1[i]) >> 8;
    }

    //for (i = 0; i < u32Interval; i++)
    i = u32Interval >> 3;
    {
        stIspDrcAttr.stAuto.u16Strength = SCENE_TimeFilter(u32LastDrcStr[ViPipe], u32CurrDrcStr, u32Interval, i);
        stIspDrcAttr.u8LocalMixingBrightMax = SCENE_TimeFilter(u8LastDrcLocalMixingBrightMax[ViPipe], u8CurLocalMixingBrightMax, u32Interval, i);
        stIspDrcAttr.u8LocalMixingBrightMin = SCENE_TimeFilter(u8LastDrcLocalMixingBrightMin[ViPipe], u8CurLocalMixingBrightMin, u32Interval, i);
        stIspDrcAttr.u8LocalMixingDarkMax = SCENE_TimeFilter(u8LastLocalMixingDarkMax[ViPipe], u8CurLocalMixingDarkMax, u32Interval, i);
        stIspDrcAttr.u8LocalMixingDarkMin = SCENE_TimeFilter(u8LastLocalMixingDarkMin[ViPipe], u8CurLocalMixingDarkMin, u32Interval, i);

        stIspDrcAttr.s8DetailAdjustFactor = SCENE_TimeFilter(ABS(s8LastDetailAdjustFactor[ViPipe], 0), ABS(s8CurDetailAdjustFactor, 0),
                                            u32Interval, i);
        stIspDrcAttr.u8SpatialFltCoef = SCENE_TimeFilter(u8LastSpatialFltCoef[ViPipe], u8SpatialFltCoef, u32Interval, i);
        stIspDrcAttr.u8RangeFltCoef = SCENE_TimeFilter(u8LastRangeFltCoef[ViPipe], u8RangeFltCoef, u32Interval, i);
        stIspDrcAttr.u8GradRevMax = SCENE_TimeFilter(u8LastGradRevMax[ViPipe], u8GradRevMax, u32Interval, i);
        stIspDrcAttr.u8GradRevThr = SCENE_TimeFilter(u8LastGradRevThr[ViPipe], u8GradRevThr, u32Interval, i);
        stIspDrcAttr.stAsymmetryCurve.u8Compress = SCENE_TimeFilter(u8LastCompress[ViPipe], u8CurCompress, u32Interval, i);
        stIspDrcAttr.stAsymmetryCurve.u8Stretch = SCENE_TimeFilter(u8LastStretch[ViPipe], u8CurStretch, u32Interval, i);
        stIspDrcAttr.stAuto.u16StrengthMin = 0;
        stIspDrcAttr.u8DetailDarkStep = 0;

        //printf("ini_s8DetailAjustFactor = %d\n",g_stSceneautoParam.pstIspParam[s32Index].stDrcParam.pstDRCAttr[IndexX].s8DetailAjustFactor);
        //printf("s8LastDetailAdjustFactor = %d s8DetailAdjustFactor = %d\n",s8LastDetailAdjustFactor[ViPipe],stIspDrcAttr.s8DetailAdjustFactor);
        HI_U32 j = 0;
        for(j = 0 ; j < 200 ; j++)
        {
            stIspDrcAttr.au16ToneMappingValue[j]  = SCENE_TimeFilter(stIspDrcAttr.au16ToneMappingValue[j],
        		                          au16ToneMappingValue[j], g_astScenePipeParam[u8Index].stDynamicDrc.u32Interval,i);
        }

		CHECK_SCENE_PAUSE();
        s32Ret = HI_MPI_ISP_SetDRCAttr(ViPipe, &stIspDrcAttr);
	    CHECK_SCENE_RET(s32Ret);
        usleep(40000);
    }

    u32LastDrcStr[ViPipe] = u32CurrDrcStr;
    u8LastDrcLocalMixingBrightMax[ViPipe] = g_astScenePipeParam[u8Index].stDynamicDrc.au8LocalMixingBrightMax[u32IsoLevel];
    u8LastDrcLocalMixingBrightMin[ViPipe] = g_astScenePipeParam[u8Index].stDynamicDrc.au8LocalMixingBrightMin[u32IsoLevel];
    u8LastLocalMixingDarkMax[ViPipe] = g_astScenePipeParam[u8Index].stDynamicDrc.au8LocalMixingDarkMax[u32IsoLevel];
    u8LastLocalMixingDarkMin[ViPipe] = g_astScenePipeParam[u8Index].stDynamicDrc.au8LocalMixingDarkMin[u32IsoLevel];
    s8LastDetailAdjustFactor[ViPipe] = g_astScenePipeParam[u8Index].stDynamicDrc.as8DetailAdjustFactor[u32IsoLevel];
    u8LastSpatialFltCoef[ViPipe] = g_astScenePipeParam[u8Index].stDynamicDrc.au8SpatialFltCoef[u32IsoLevel];
    u8LastRangeFltCoef[ViPipe] = g_astScenePipeParam[u8Index].stDynamicDrc.au8RangeFltCoef[u32IsoLevel];
    u8LastGradRevMax[ViPipe] = g_astScenePipeParam[u8Index].stDynamicDrc.au8GradRevMax[u32IsoLevel];
    u8LastGradRevThr[ViPipe] = g_astScenePipeParam[u8Index].stDynamicDrc.au8GradRevThr[u32IsoLevel];
    u8LastCompress[ViPipe] = g_astScenePipeParam[u8Index].stDynamicDrc.au8Compress[u32IsoLevel];
    u8LastStretch[ViPipe] = g_astScenePipeParam[u8Index].stDynamicDrc.au8Stretch[u32IsoLevel];

    return HI_SUCCESS;
}

HI_S32 HI_SCENE_SetStaticLDCI(VI_PIPE ViPipe, HI_U8 u8Index)
{
	if(HI_TRUE != g_astScenePipeParam[u8Index].stModuleState.bStaticLdci)
	{
		return HI_SUCCESS;
	}

    HI_S32 i = 0;
    HI_S32 s32Ret = HI_SUCCESS;

    ISP_LDCI_ATTR_S stLDCIAttr;

    s32Ret = HI_MPI_ISP_GetLDCIAttr(ViPipe, &stLDCIAttr);
    CHECK_SCENE_RET(s32Ret);

    stLDCIAttr.bEnable = g_astScenePipeParam[u8Index].stStaticLdci.bEnable;
    stLDCIAttr.enOpType = (ISP_OP_TYPE_E)g_astScenePipeParam[u8Index].stStaticLdci.u8LDCIOpType;
    stLDCIAttr.u8GaussLPFSigma = g_astScenePipeParam[u8Index].stStaticLdci.u8GaussLPFSigma;

    for (i = 0; i < ISP_AUTO_ISO_STRENGTH_NUM; i++)
    {
        stLDCIAttr.stAuto.au16BlcCtrl[i] = g_astScenePipeParam[u8Index].stStaticLdci.au16AutoBlcCtrl[i];
        stLDCIAttr.stAuto.astHeWgt[i].stHePosWgt.u8Sigma = g_astScenePipeParam[u8Index].stStaticLdci.au8AutoHePosSigma[i];
        stLDCIAttr.stAuto.astHeWgt[i].stHePosWgt.u8Wgt = g_astScenePipeParam[u8Index].stStaticLdci.au8AutoHePosWgt[i];
        stLDCIAttr.stAuto.astHeWgt[i].stHePosWgt.u8Mean = g_astScenePipeParam[u8Index].stStaticLdci.au8AutoHePosMean[i];
        stLDCIAttr.stAuto.astHeWgt[i].stHeNegWgt.u8Sigma = g_astScenePipeParam[u8Index].stStaticLdci.au8AutoHeNegSigma[i];
        stLDCIAttr.stAuto.astHeWgt[i].stHeNegWgt.u8Wgt = g_astScenePipeParam[u8Index].stStaticLdci.au8AutoHeNegWgt[i];
        stLDCIAttr.stAuto.astHeWgt[i].stHeNegWgt.u8Mean = g_astScenePipeParam[u8Index].stStaticLdci.au8AutoHeNegMean[i];
    }

    s32Ret = HI_MPI_ISP_SetLDCIAttr(ViPipe, &stLDCIAttr);
    CHECK_SCENE_RET(s32Ret);
    return HI_SUCCESS;
}

HI_S32 HI_SCENE_SetDynamicLDCI(VI_PIPE ViPipe, HI_U64 u64Exposure, HI_U64 u64LastExposure, HI_U8 u8Index)
{
	if(HI_TRUE != g_astScenePipeParam[u8Index].stModuleState.bDynamicLdci)
	{
		return HI_SUCCESS;
	}

    HI_U32 u32ExpLevel = 0;
    HI_S32 s32Ret = HI_SUCCESS;

    ISP_LDCI_ATTR_S stLDCIAttr;

    if (u64Exposure != u64LastExposure)
    {
        CHECK_SCENE_PAUSE();
        s32Ret = HI_MPI_ISP_GetLDCIAttr(ViPipe, &stLDCIAttr);
        CHECK_SCENE_RET(s32Ret);

        u32ExpLevel = SCENE_GetLevelLtoH(u64Exposure, g_astScenePipeParam[u8Index].stDynamicLDCI.u32EnableCnt, g_astScenePipeParam[u8Index].stDynamicLDCI.au64EnableExpThreshLtoH);
        stLDCIAttr.bEnable = g_astScenePipeParam[u8Index].stDynamicLDCI.au8Enable[u32ExpLevel];

        u32ExpLevel = SCENE_GetLevelLtoH(u64Exposure, g_astScenePipeParam[u8Index].stDynamicLDCI.u32EnableCnt, g_astScenePipeParam[u8Index].stDynamicLDCI.au64EnableExpThreshLtoH);
        stLDCIAttr.bEnable = g_astScenePipeParam[u8Index].stDynamicLDCI.au8Enable[u32ExpLevel];

        CHECK_SCENE_PAUSE();
        s32Ret = HI_MPI_ISP_SetLDCIAttr(ViPipe, &stLDCIAttr);
        CHECK_SCENE_RET(s32Ret);
    }

    return HI_SUCCESS;
}

HI_S32 HI_SCENE_SetStaticDeHaze(VI_PIPE ViPipe, HI_U8 u8Index)
{
	if(HI_TRUE != g_astScenePipeParam[u8Index].stModuleState.bStaticDehaze)
	{
		return HI_SUCCESS;
	}

    HI_S32 i = 0;
    HI_S32 s32Ret = HI_SUCCESS;

    ISP_DEHAZE_ATTR_S stDehazeAttr;

    s32Ret = HI_MPI_ISP_GetDehazeAttr(ViPipe, &stDehazeAttr);
    CHECK_SCENE_RET(s32Ret);

    stDehazeAttr.bEnable = g_astScenePipeParam[u8Index].stStaticDehaze.bEnable;
    stDehazeAttr.enOpType = (ISP_OP_TYPE_E)g_astScenePipeParam[u8Index].stStaticDehaze.u8DehazeOpType;
    stDehazeAttr.bUserLutEnable = g_astScenePipeParam[u8Index].stStaticDehaze.bUserLutEnable;

    for (i = 0; i < 256; i++)
    {
        stDehazeAttr.au8DehazeLut[i] = g_astScenePipeParam[u8Index].stStaticDehaze.au8DehazeLut[i];
    }

    s32Ret = HI_MPI_ISP_SetDehazeAttr(ViPipe, &stDehazeAttr);
    CHECK_SCENE_RET(s32Ret);
    return HI_SUCCESS;
}

HI_S32 HI_SCENE_SetDynamicDehaze(VI_PIPE ViPipe, HI_U64 u64Exposure, HI_U64 u64LastExposure, HI_U8 u8Index)
{
    if (HI_TRUE != g_astScenePipeParam[u8Index].stModuleState.bDynamicDehaze)
    {
        return HI_SUCCESS;
    }

//    static const HI_U8 au8UserDehazeLutDefault[256] =
//    {
//        255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
//        255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
//        255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
//        255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
//        255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
//        255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
//        255, 255, 255, 255, 255, 255, 255, 255, 254, 254, 254, 254, 253, 253, 253, 252, 252,
//        252, 251, 251, 250, 250, 249, 248, 248, 247, 246, 246, 245, 244, 243, 242, 242, 241,
//        240, 239, 238, 237, 236, 235, 234, 233, 232, 231, 229, 228, 227, 226, 225, 223, 222,
//        221, 219, 218, 217, 215, 214, 212, 211, 209, 208, 206, 205, 203, 202, 200, 198, 197,
//        195, 193, 192, 190, 188, 186, 185, 183, 181, 179, 177, 175, 174, 172, 170, 168, 166,
//        164, 162, 160, 158, 156, 154, 152, 149, 147, 145, 143, 141, 139, 137, 134, 132, 130,
//        128, 126, 123, 121, 119, 116, 114, 112, 109, 107, 105, 102, 100, 98, 95, 93, 90, 88,
//        85, 83, 80, 78, 76, 73, 70, 68, 65, 63, 60, 58, 55, 53, 50, 48, 45, 42, 40, 37, 35,
//        32, 29, 27, 24, 21, 19, 16, 13, 11, 8, 5, 3, 0
//    };
//
//    static const HI_U8 au8UserDehazeLut[256] =
//    {
//        57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 84, 85, 86, 87, 88, 89,
//        90, 91, 92, 93, 94, 95, 96, 97, 98, 99, 100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111, 112, 113, 114, 115, 116, 117,
//        118, 119, 120, 121, 122, 123, 124, 125, 126, 127, 128, 129, 130, 131, 132, 133, 134, 135, 136, 137, 138, 139, 140, 141, 142, 143,
//        144, 145, 146, 147, 148, 149, 150, 151, 152, 153, 154, 155, 156, 157, 158, 159, 160, 161, 162, 163, 164, 165, 166, 167, 168, 169, 170,
//        171, 172, 173, 174, 175, 176, 177, 178, 179, 180, 181, 182, 183, 184, 185, 186, 187, 188, 189, 190, 191, 192, 193, 194, 195, 196, 197,
//        198, 199, 200, 201, 202, 203, 204, 205, 206, 207, 208, 209, 210, 211, 212, 213, 214, 215, 216, 217, 218, 219, 220, 221, 222, 223, 224,
//        225, 226, 227, 228, 229, 230, 231, 232, 233, 234, 235, 236, 237, 238, 239, 240, 241, 242, 243, 244, 245, 246, 247, 248, 249, 250, 251,
//        252, 253, 254, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
//        255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
//        255, 255, 255, 255, 255, 255, 255
//    };

    HI_U32 i=0;//,j = 0;
    //HI_U32 u32ExpLevel = 0;
    HI_S32 s32Ret = HI_SUCCESS;
    HI_U32 u32Interval = 20;
    static HI_U8 u8Lastdehaze = 0;
    static HI_U8 u8Curdehaze = 0;
    //HI_U8 au8UserDehazeLutGet[256]={0};
    ISP_DEHAZE_ATTR_S stDehazeAttr;
    ISP_INNER_STATE_INFO_S stInnerStateInfo;
    ISP_PUB_ATTR_S stPubAttr;
    ISP_EXP_INFO_S stIspExpInfo;

    s32Ret = HI_MPI_ISP_QueryInnerStateInfo(ViPipe, &stInnerStateInfo);
    if (HI_SUCCESS != s32Ret)
    {
        printf("HI_MPI_ISP_QueryInnerStateInfo failed\n");
        return HI_FAILURE;
    }

    s32Ret = HI_MPI_ISP_QueryExposureInfo(ViPipe, &stIspExpInfo);
    if (HI_SUCCESS != s32Ret)
    {
        printf("HI_MPI_ISP_QueryExposureInfo failed\n");
        return HI_FAILURE;
    }

    s32Ret = HI_MPI_ISP_GetPubAttr(ViPipe, &stPubAttr);
    if (HI_SUCCESS != s32Ret)
    {
        printf("HI_MPI_ISP_GetPubAttr failed\n");
        return HI_FAILURE;
    }

    s32Ret = HI_MPI_ISP_GetDehazeAttr(ViPipe, &stDehazeAttr);
    if (HI_SUCCESS != s32Ret)
    {
        printf("HI_MPI_ISP_GetDehazeAttr failed\n");
        return HI_FAILURE;
    }

    u8Lastdehaze = stDehazeAttr.stManual.u8strength;
    //for (j = 0; j < 256; j++)
    //{
    //	au8UserDehazeLutGet[j]= stDehazeAttr.au8DehazeLut[j];
    //}

    if (WDR_MODE_NONE != stPubAttr.enWDRMode)
    {
        if (stIspExpInfo.u32ISO > 1600)
        {
            stDehazeAttr.bUserLutEnable = HI_FALSE;
            u8Curdehaze = 120;
        }
        else
        {
            stDehazeAttr.bUserLutEnable = HI_FALSE;
            u8Curdehaze = 126 - (HI_U8)((double)(stInnerStateInfo.u32WDRExpRatioActual[0] / (double)g_astScenePipeParam[u8Index].stStaticWdrExposure.u32ExpRatioMin) * 5);
        }

        stDehazeAttr.bEnable = HI_TRUE;
        stDehazeAttr.enOpType = OP_TYPE_MANUAL;

        for (i = 0; i < u32Interval; i++)
        {
            //if (stIspExpInfo.u32ISO > 1600)
            //{
            //    for (j = 0; j < 256; j++)
            //    {
            //        stDehazeAttr.au8DehazeLut[j] = SCENE_TimeFilter(au8UserDehazeLutGet[j], au8UserDehazeLut[j], u32Interval, i);
            //    }
            //}
            //else
            //{
            //    for (j = 0; j < 256; j++)
            //    {
            //        stDehazeAttr.au8DehazeLut[j] = SCENE_TimeFilter(au8UserDehazeLutGet[j], au8UserDehazeLutDefault[j], u32Interval, i);
            //    }
            //}
            stDehazeAttr.stManual.u8strength = SCENE_TimeFilter(u8Lastdehaze, u8Curdehaze, u32Interval, i);
            s32Ret = HI_MPI_ISP_SetDehazeAttr(ViPipe, &stDehazeAttr);
            CHECK_SCENE_RET(s32Ret);
            usleep(40000);
        }
    }
    else
    {
        stDehazeAttr.bEnable = HI_FALSE;
        CHECK_SCENE_PAUSE();
        s32Ret = HI_MPI_ISP_SetDehazeAttr(ViPipe, &stDehazeAttr);
        CHECK_SCENE_RET(s32Ret);

    }

    return HI_SUCCESS;
}

HI_S32 HI_SCENE_SetStatic3DNR(VI_PIPE ViPipe, HI_U8 u8Index)
{
	if(HI_TRUE != g_astScenePipeParam[u8Index].stModuleState.bStatic3DNR)
	{
		return HI_SUCCESS;
	}

	HI_S32 s32Ret = HI_SUCCESS;
    HI_S32 i, j = 0;

	VPSS_GRP_NRX_PARAM_S stNrx;

    stNrx.stNRXParam_V1.stNRXAuto.u32ParamNum = 10;
    stNrx.stNRXParam_V1.stNRXAuto.pau32ISO = (HI_U32*)malloc(sizeof(HI_U32) * 10);
    CHECK_SCENE_NULLPTR(stNrx.stNRXParam_V1.stNRXAuto.pau32ISO);
    stNrx.stNRXParam_V1.stNRXAuto.pastNRXParam = (VPSS_NRX_V1_S*)malloc(sizeof(VPSS_NRX_V1_S) * 10);
    CHECK_SCENE_NULLPTR(stNrx.stNRXParam_V1.stNRXAuto.pastNRXParam);

	stNrx.enNRVer = VPSS_NR_V1;
	stNrx.stNRXParam_V1.enOptMode = OPERATION_MODE_AUTO;

	s32Ret = HI_MPI_VPSS_GetGrpNRXParam(ViPipe, &stNrx);
    if (HI_SUCCESS != s32Ret)
    {
        printf("HI_MPI_VPSS_GetGrpNRXParam failed.");
        FREE_SCENE_PTR(stNrx.stNRXParam_V1.stNRXAuto.pau32ISO);
        FREE_SCENE_PTR(stNrx.stNRXParam_V1.stNRXAuto.pastNRXParam);
        return HI_FAILURE;
    }

    memcpy(stNrx.stNRXParam_V1.stNRXAuto.pau32ISO, g_astScenePipeParam[u8Index].stStaticThreeDNR.au32ThreeDNRIso,
           sizeof(HI_U32) * g_astScenePipeParam[u8Index].stStaticThreeDNR.u32ThreeDNRCount);

    stNrx.stNRXParam_V1.stNRXAuto.u32ParamNum = g_astScenePipeParam[u8Index].stStaticThreeDNR.u32ThreeDNRCount;

    for (i = 0; i < stNrx.stNRXParam_V1.stNRXAuto.u32ParamNum; i++)
    {

        /*IEy*/
        for (j = 0; j < 2; j++)
        {
            stNrx.stNRXParam_V1.stNRXAuto.pastNRXParam[i].IEy[j].IES0 = g_astScenePipeParam[u8Index].stStaticThreeDNR.astThreeDNRValue[i].IEy[j].IES0;
            stNrx.stNRXParam_V1.stNRXAuto.pastNRXParam[i].IEy[j].IES1 = g_astScenePipeParam[u8Index].stStaticThreeDNR.astThreeDNRValue[i].IEy[j].IES1;
            stNrx.stNRXParam_V1.stNRXAuto.pastNRXParam[i].IEy[j].IES2 = g_astScenePipeParam[u8Index].stStaticThreeDNR.astThreeDNRValue[i].IEy[j].IES2;
            stNrx.stNRXParam_V1.stNRXAuto.pastNRXParam[i].IEy[j].IES3 = g_astScenePipeParam[u8Index].stStaticThreeDNR.astThreeDNRValue[i].IEy[j].IES3;
            stNrx.stNRXParam_V1.stNRXAuto.pastNRXParam[i].IEy[j].IEDZ = g_astScenePipeParam[u8Index].stStaticThreeDNR.astThreeDNRValue[i].IEy[j].IEDZ;
        }

        /*SFy*/
        for (j = 0; j < 4; j++)
        {
            stNrx.stNRXParam_V1.stNRXAuto.pastNRXParam[i].SFy[j].NRyEn = g_astScenePipeParam[u8Index].stStaticThreeDNR.astThreeDNRValue[i].SFy[j].NRyEn;
            stNrx.stNRXParam_V1.stNRXAuto.pastNRXParam[i].SFy[j].SPN6 = g_astScenePipeParam[u8Index].stStaticThreeDNR.astThreeDNRValue[i].SFy[j].SPN6;
            stNrx.stNRXParam_V1.stNRXAuto.pastNRXParam[i].SFy[j].SFR = g_astScenePipeParam[u8Index].stStaticThreeDNR.astThreeDNRValue[i].SFy[j].SFR;

            stNrx.stNRXParam_V1.stNRXAuto.pastNRXParam[i].SFy[j].SBN6 = g_astScenePipeParam[u8Index].stStaticThreeDNR.astThreeDNRValue[i].SFy[j].SBN6;
            stNrx.stNRXParam_V1.stNRXAuto.pastNRXParam[i].SFy[j].PBR6 = g_astScenePipeParam[u8Index].stStaticThreeDNR.astThreeDNRValue[i].SFy[j].PBR6;

            stNrx.stNRXParam_V1.stNRXAuto.pastNRXParam[i].SFy[j].SFS2 = g_astScenePipeParam[u8Index].stStaticThreeDNR.astThreeDNRValue[i].SFy[j].SFS2;
            stNrx.stNRXParam_V1.stNRXAuto.pastNRXParam[i].SFy[j].SFT2 = g_astScenePipeParam[u8Index].stStaticThreeDNR.astThreeDNRValue[i].SFy[j].SFT2;
            stNrx.stNRXParam_V1.stNRXAuto.pastNRXParam[i].SFy[j].SBR2 = g_astScenePipeParam[u8Index].stStaticThreeDNR.astThreeDNRValue[i].SFy[j].SBR2;

            stNrx.stNRXParam_V1.stNRXAuto.pastNRXParam[i].SFy[j].SFS4 = g_astScenePipeParam[u8Index].stStaticThreeDNR.astThreeDNRValue[i].SFy[j].SFS4;
            stNrx.stNRXParam_V1.stNRXAuto.pastNRXParam[i].SFy[j].SFT4 = g_astScenePipeParam[u8Index].stStaticThreeDNR.astThreeDNRValue[i].SFy[j].SFT4;
            stNrx.stNRXParam_V1.stNRXAuto.pastNRXParam[i].SFy[j].SBR4 = g_astScenePipeParam[u8Index].stStaticThreeDNR.astThreeDNRValue[i].SFy[j].SBR4;

            stNrx.stNRXParam_V1.stNRXAuto.pastNRXParam[i].SFy[j].SFN0 = g_astScenePipeParam[u8Index].stStaticThreeDNR.astThreeDNRValue[i].SFy[j].SFN0;
            stNrx.stNRXParam_V1.stNRXAuto.pastNRXParam[i].SFy[j].SFN1 = g_astScenePipeParam[u8Index].stStaticThreeDNR.astThreeDNRValue[i].SFy[j].SFN1;
            stNrx.stNRXParam_V1.stNRXAuto.pastNRXParam[i].SFy[j].SFN2 = g_astScenePipeParam[u8Index].stStaticThreeDNR.astThreeDNRValue[i].SFy[j].SFN2;
            stNrx.stNRXParam_V1.stNRXAuto.pastNRXParam[i].SFy[j].SFN3 = g_astScenePipeParam[u8Index].stStaticThreeDNR.astThreeDNRValue[i].SFy[j].SFN3;

            stNrx.stNRXParam_V1.stNRXAuto.pastNRXParam[i].SFy[j].STH1 = g_astScenePipeParam[u8Index].stStaticThreeDNR.astThreeDNRValue[i].SFy[j].STH1;
            stNrx.stNRXParam_V1.stNRXAuto.pastNRXParam[i].SFy[j].STH2 = g_astScenePipeParam[u8Index].stStaticThreeDNR.astThreeDNRValue[i].SFy[j].STH2;
            stNrx.stNRXParam_V1.stNRXAuto.pastNRXParam[i].SFy[j].STH3 = g_astScenePipeParam[u8Index].stStaticThreeDNR.astThreeDNRValue[i].SFy[j].STH3;

            stNrx.stNRXParam_V1.stNRXAuto.pastNRXParam[i].SFy[j].kMode = g_astScenePipeParam[u8Index].stStaticThreeDNR.astThreeDNRValue[i].SFy[j].kMode;
            stNrx.stNRXParam_V1.stNRXAuto.pastNRXParam[i].SFy[j].BWSF4 = g_astScenePipeParam[u8Index].stStaticThreeDNR.astThreeDNRValue[i].SFy[j].BWSF4;
            stNrx.stNRXParam_V1.stNRXAuto.pastNRXParam[i].SFy[j].tEdge = g_astScenePipeParam[u8Index].stStaticThreeDNR.astThreeDNRValue[i].SFy[j].tEdge;
            stNrx.stNRXParam_V1.stNRXAuto.pastNRXParam[i].SFy[j].TriTh = g_astScenePipeParam[u8Index].stStaticThreeDNR.astThreeDNRValue[i].SFy[j].TriTh;
        }

        /*TFy*/
        for (j = 0; j < 3; j++)
        {
            if (0 == j)
            {
                stNrx.stNRXParam_V1.stNRXAuto.pastNRXParam[i].TFy[j].bRef = g_astScenePipeParam[u8Index].stStaticThreeDNR.astThreeDNRValue[i].TFy[j].bRef;
            }

            stNrx.stNRXParam_V1.stNRXAuto.pastNRXParam[i].TFy[j].TFS = g_astScenePipeParam[u8Index].stStaticThreeDNR.astThreeDNRValue[i].TFy[j].TFS;
            stNrx.stNRXParam_V1.stNRXAuto.pastNRXParam[i].TFy[j].TDZ = g_astScenePipeParam[u8Index].stStaticThreeDNR.astThreeDNRValue[i].TFy[j].TDZ;
            stNrx.stNRXParam_V1.stNRXAuto.pastNRXParam[i].TFy[j].TDX = g_astScenePipeParam[u8Index].stStaticThreeDNR.astThreeDNRValue[i].TFy[j].TDX;
            stNrx.stNRXParam_V1.stNRXAuto.pastNRXParam[i].TFy[j].TSS = g_astScenePipeParam[u8Index].stStaticThreeDNR.astThreeDNRValue[i].TFy[j].TSS;
            stNrx.stNRXParam_V1.stNRXAuto.pastNRXParam[i].TFy[j].TSI = g_astScenePipeParam[u8Index].stStaticThreeDNR.astThreeDNRValue[i].TFy[j].TSI;
            stNrx.stNRXParam_V1.stNRXAuto.pastNRXParam[i].TFy[j].SDZ = g_astScenePipeParam[u8Index].stStaticThreeDNR.astThreeDNRValue[i].TFy[j].SDZ;
            stNrx.stNRXParam_V1.stNRXAuto.pastNRXParam[i].TFy[j].STR = g_astScenePipeParam[u8Index].stStaticThreeDNR.astThreeDNRValue[i].TFy[j].STR;

            stNrx.stNRXParam_V1.stNRXAuto.pastNRXParam[i].TFy[j].TFR[0] = g_astScenePipeParam[u8Index].stStaticThreeDNR.astThreeDNRValue[i].TFy[j].TFR[0];
            stNrx.stNRXParam_V1.stNRXAuto.pastNRXParam[i].TFy[j].TFR[1] = g_astScenePipeParam[u8Index].stStaticThreeDNR.astThreeDNRValue[i].TFy[j].TFR[1];
            stNrx.stNRXParam_V1.stNRXAuto.pastNRXParam[i].TFy[j].TFR[2] = g_astScenePipeParam[u8Index].stStaticThreeDNR.astThreeDNRValue[i].TFy[j].TFR[2];
            stNrx.stNRXParam_V1.stNRXAuto.pastNRXParam[i].TFy[j].TFR[3] = g_astScenePipeParam[u8Index].stStaticThreeDNR.astThreeDNRValue[i].TFy[j].TFR[3];
            stNrx.stNRXParam_V1.stNRXAuto.pastNRXParam[i].TFy[j].TFR[4] = g_astScenePipeParam[u8Index].stStaticThreeDNR.astThreeDNRValue[i].TFy[j].TFR[4];
        }

        /*NRc*/
        stNrx.stNRXParam_V1.stNRXAuto.pastNRXParam[i].NRc.SFC = g_astScenePipeParam[u8Index].stStaticThreeDNR.astThreeDNRValue[i].NRc.SFC;
        stNrx.stNRXParam_V1.stNRXAuto.pastNRXParam[i].NRc.TFC = g_astScenePipeParam[u8Index].stStaticThreeDNR.astThreeDNRValue[i].NRc.TFC;
        stNrx.stNRXParam_V1.stNRXAuto.pastNRXParam[i].NRc.CSFS = g_astScenePipeParam[u8Index].stStaticThreeDNR.astThreeDNRValue[i].NRc.CSFS;
        stNrx.stNRXParam_V1.stNRXAuto.pastNRXParam[i].NRc.CSFR = g_astScenePipeParam[u8Index].stStaticThreeDNR.astThreeDNRValue[i].NRc.CSFR;
        stNrx.stNRXParam_V1.stNRXAuto.pastNRXParam[i].NRc.CIIR = g_astScenePipeParam[u8Index].stStaticThreeDNR.astThreeDNRValue[i].NRc.CIIR;
        stNrx.stNRXParam_V1.stNRXAuto.pastNRXParam[i].NRc.CTFS = g_astScenePipeParam[u8Index].stStaticThreeDNR.astThreeDNRValue[i].NRc.CTFS;
        stNrx.stNRXParam_V1.stNRXAuto.pastNRXParam[i].NRc.CTFR = g_astScenePipeParam[u8Index].stStaticThreeDNR.astThreeDNRValue[i].NRc.CTFR;

        /*MDy*/
        for (j = 0; j < 2; j++)
        {
            stNrx.stNRXParam_V1.stNRXAuto.pastNRXParam[i].MDy[j].MAI0 = g_astScenePipeParam[u8Index].stStaticThreeDNR.astThreeDNRValue[i].MDy[j].MAI0;
            stNrx.stNRXParam_V1.stNRXAuto.pastNRXParam[i].MDy[j].MAI1 = g_astScenePipeParam[u8Index].stStaticThreeDNR.astThreeDNRValue[i].MDy[j].MAI1;
            stNrx.stNRXParam_V1.stNRXAuto.pastNRXParam[i].MDy[j].MAI2 = g_astScenePipeParam[u8Index].stStaticThreeDNR.astThreeDNRValue[i].MDy[j].MAI2;

            stNrx.stNRXParam_V1.stNRXAuto.pastNRXParam[i].MDy[j].MADK = g_astScenePipeParam[u8Index].stStaticThreeDNR.astThreeDNRValue[i].MDy[j].MADK;
            stNrx.stNRXParam_V1.stNRXAuto.pastNRXParam[i].MDy[j].MABR = g_astScenePipeParam[u8Index].stStaticThreeDNR.astThreeDNRValue[i].MDy[j].MABR;

            stNrx.stNRXParam_V1.stNRXAuto.pastNRXParam[i].MDy[j].MADZ = g_astScenePipeParam[u8Index].stStaticThreeDNR.astThreeDNRValue[i].MDy[j].MADZ;

            stNrx.stNRXParam_V1.stNRXAuto.pastNRXParam[i].MDy[j].MATH = g_astScenePipeParam[u8Index].stStaticThreeDNR.astThreeDNRValue[i].MDy[j].MATH;
            stNrx.stNRXParam_V1.stNRXAuto.pastNRXParam[i].MDy[j].MATE = g_astScenePipeParam[u8Index].stStaticThreeDNR.astThreeDNRValue[i].MDy[j].MATE;

            stNrx.stNRXParam_V1.stNRXAuto.pastNRXParam[i].MDy[j].MABW = g_astScenePipeParam[u8Index].stStaticThreeDNR.astThreeDNRValue[i].MDy[j].MABW;
            stNrx.stNRXParam_V1.stNRXAuto.pastNRXParam[i].MDy[j].MATW = g_astScenePipeParam[u8Index].stStaticThreeDNR.astThreeDNRValue[i].MDy[j].MATW;

            stNrx.stNRXParam_V1.stNRXAuto.pastNRXParam[i].MDy[j].MASW = g_astScenePipeParam[u8Index].stStaticThreeDNR.astThreeDNRValue[i].MDy[j].MASW;
            stNrx.stNRXParam_V1.stNRXAuto.pastNRXParam[i].MDy[j].MAXN = g_astScenePipeParam[u8Index].stStaticThreeDNR.astThreeDNRValue[i].MDy[j].MAXN;
        }

        /*MDy*/
        for (j = 0; j < 2; j++)
        {
            stNrx.stNRXParam_V1.stNRXAuto.pastNRXParam[i].MDy[j].MAI0 = g_astScenePipeParam[u8Index].stStaticThreeDNR.astThreeDNRValue[i].MDy[j].MAI0;
            stNrx.stNRXParam_V1.stNRXAuto.pastNRXParam[i].MDy[j].MAI1 = g_astScenePipeParam[u8Index].stStaticThreeDNR.astThreeDNRValue[i].MDy[j].MAI1;
            stNrx.stNRXParam_V1.stNRXAuto.pastNRXParam[i].MDy[j].MAI2 = g_astScenePipeParam[u8Index].stStaticThreeDNR.astThreeDNRValue[i].MDy[j].MAI2;

            stNrx.stNRXParam_V1.stNRXAuto.pastNRXParam[i].MDy[j].MADK = g_astScenePipeParam[u8Index].stStaticThreeDNR.astThreeDNRValue[i].MDy[j].MADK;
            stNrx.stNRXParam_V1.stNRXAuto.pastNRXParam[i].MDy[j].MABR = g_astScenePipeParam[u8Index].stStaticThreeDNR.astThreeDNRValue[i].MDy[j].MABR;

            stNrx.stNRXParam_V1.stNRXAuto.pastNRXParam[i].MDy[j].MADZ = g_astScenePipeParam[u8Index].stStaticThreeDNR.astThreeDNRValue[i].MDy[j].MADZ;

            stNrx.stNRXParam_V1.stNRXAuto.pastNRXParam[i].MDy[j].MATH = g_astScenePipeParam[u8Index].stStaticThreeDNR.astThreeDNRValue[i].MDy[j].MATH;
            stNrx.stNRXParam_V1.stNRXAuto.pastNRXParam[i].MDy[j].MATE = g_astScenePipeParam[u8Index].stStaticThreeDNR.astThreeDNRValue[i].MDy[j].MATE;
            stNrx.stNRXParam_V1.stNRXAuto.pastNRXParam[i].MDy[j].MABW = g_astScenePipeParam[u8Index].stStaticThreeDNR.astThreeDNRValue[i].MDy[j].MABW;
            stNrx.stNRXParam_V1.stNRXAuto.pastNRXParam[i].MDy[j].MATW = g_astScenePipeParam[u8Index].stStaticThreeDNR.astThreeDNRValue[i].MDy[j].MATW;

            stNrx.stNRXParam_V1.stNRXAuto.pastNRXParam[i].MDy[j].MASW = g_astScenePipeParam[u8Index].stStaticThreeDNR.astThreeDNRValue[i].MDy[j].MASW;
            stNrx.stNRXParam_V1.stNRXAuto.pastNRXParam[i].MDy[j].MAXN = g_astScenePipeParam[u8Index].stStaticThreeDNR.astThreeDNRValue[i].MDy[j].MAXN;
        }

        for (j = 0; j < 32; j++)
        {
            stNrx.stNRXParam_V1.stNRXAuto.pastNRXParam[i].SBSk2[j] = 0;
            stNrx.stNRXParam_V1.stNRXAuto.pastNRXParam[i].SBSk3[j] = 0;
            stNrx.stNRXParam_V1.stNRXAuto.pastNRXParam[i].SDSk2[j] = 0;
            stNrx.stNRXParam_V1.stNRXAuto.pastNRXParam[i].SDSk3[j] = 0;
        }

		//PrintNRxVPSS(&stNrx.stNRXParam_V1.stNRXAuto.pastNRXParam[i]);
    }

	stNrx.enNRVer = VPSS_NR_V1;
	stNrx.stNRXParam_V1.enOptMode = OPERATION_MODE_AUTO;
	s32Ret = HI_MPI_VPSS_SetGrpNRXParam(ViPipe, &stNrx);
    if (HI_SUCCESS != s32Ret)
    {
        printf("HI_MPI_VPSS_SetGrpNRXParam failed.");
        FREE_SCENE_PTR(stNrx.stNRXParam_V1.stNRXAuto.pau32ISO);
        FREE_SCENE_PTR(stNrx.stNRXParam_V1.stNRXAuto.pastNRXParam);
        return HI_FAILURE;
    }

	return HI_SUCCESS;
}

HI_S32 HI_SCENE_Set3DNR(VI_PIPE ViPipe, const HI_SCENE_3DNR_S *pst3Nrx)
{
	HI_S32 s32Ret = HI_SUCCESS;
	HI_U32 i = 0;

	VPSS_GRP_NRX_PARAM_S stNrx, stViNrXGet;
	VPSS_GRP_ATTR_S stGrpAttr;

	stNrx.enNRVer = VPSS_NR_V1;
	stNrx.stNRXParam_V1.enOptMode = OPERATION_MODE_MANUAL;
	s32Ret = HI_MPI_VPSS_GetGrpNRXParam(ViPipe, &stNrx);
	CHECK_SCENE_RET(s32Ret);

	/*IEy*/
	for (i = 0; i < 2; i++)
	{
		stNrx.stNRXParam_V1.stNRXManual.stNRXParam.IEy[i].IES0 = pst3Nrx->IEy[i].IES0;
		stNrx.stNRXParam_V1.stNRXManual.stNRXParam.IEy[i].IES1 = pst3Nrx->IEy[i].IES1;
		stNrx.stNRXParam_V1.stNRXManual.stNRXParam.IEy[i].IES2 = pst3Nrx->IEy[i].IES2;
		stNrx.stNRXParam_V1.stNRXManual.stNRXParam.IEy[i].IES3 = pst3Nrx->IEy[i].IES3;
		stNrx.stNRXParam_V1.stNRXManual.stNRXParam.IEy[i].IEDZ = pst3Nrx->IEy[i].IEDZ;
	}

	/*SFy*/
	for (i = 0; i < 4; i++)
	{
		stNrx.stNRXParam_V1.stNRXManual.stNRXParam.SFy[i].NRyEn = pst3Nrx->SFy[i].NRyEn;
		stNrx.stNRXParam_V1.stNRXManual.stNRXParam.SFy[i].SPN6 = pst3Nrx->SFy[i].SPN6;
		stNrx.stNRXParam_V1.stNRXManual.stNRXParam.SFy[i].SFR = pst3Nrx->SFy[i].SFR;

		stNrx.stNRXParam_V1.stNRXManual.stNRXParam.SFy[i].SBN6 = pst3Nrx->SFy[i].SBN6;
		stNrx.stNRXParam_V1.stNRXManual.stNRXParam.SFy[i].PBR6 = pst3Nrx->SFy[i].PBR6;

		stNrx.stNRXParam_V1.stNRXManual.stNRXParam.SFy[i].SFS2 = pst3Nrx->SFy[i].SFS2;
		stNrx.stNRXParam_V1.stNRXManual.stNRXParam.SFy[i].SFT2 = pst3Nrx->SFy[i].SFT2;
		stNrx.stNRXParam_V1.stNRXManual.stNRXParam.SFy[i].SBR2 = pst3Nrx->SFy[i].SBR2;

		stNrx.stNRXParam_V1.stNRXManual.stNRXParam.SFy[i].SFS4 = pst3Nrx->SFy[i].SFS4;
		stNrx.stNRXParam_V1.stNRXManual.stNRXParam.SFy[i].SFT4 = pst3Nrx->SFy[i].SFT4;
		stNrx.stNRXParam_V1.stNRXManual.stNRXParam.SFy[i].SBR4 = pst3Nrx->SFy[i].SBR4;

		stNrx.stNRXParam_V1.stNRXManual.stNRXParam.SFy[i].SFN0 = pst3Nrx->SFy[i].SFN0;
		stNrx.stNRXParam_V1.stNRXManual.stNRXParam.SFy[i].SFN1 = pst3Nrx->SFy[i].SFN1;
		stNrx.stNRXParam_V1.stNRXManual.stNRXParam.SFy[i].SFN2 = pst3Nrx->SFy[i].SFN2;
		stNrx.stNRXParam_V1.stNRXManual.stNRXParam.SFy[i].SFN3 = pst3Nrx->SFy[i].SFN3;

		stNrx.stNRXParam_V1.stNRXManual.stNRXParam.SFy[i].STH1 = pst3Nrx->SFy[i].STH1;
		stNrx.stNRXParam_V1.stNRXManual.stNRXParam.SFy[i].STH2 = pst3Nrx->SFy[i].STH2;
		stNrx.stNRXParam_V1.stNRXManual.stNRXParam.SFy[i].STH3 = pst3Nrx->SFy[i].STH3;

		stNrx.stNRXParam_V1.stNRXManual.stNRXParam.SFy[i].kMode = pst3Nrx->SFy[i].kMode;
		stNrx.stNRXParam_V1.stNRXManual.stNRXParam.SFy[i].BWSF4 = pst3Nrx->SFy[i].BWSF4;
		stNrx.stNRXParam_V1.stNRXManual.stNRXParam.SFy[i].tEdge = pst3Nrx->SFy[i].tEdge;
		stNrx.stNRXParam_V1.stNRXManual.stNRXParam.SFy[i].TriTh = pst3Nrx->SFy[i].TriTh;
	}

	/*TFy*/
	for (i = 0; i < 3; i++)
	{
		if (0 == i)
		{
			stNrx.stNRXParam_V1.stNRXManual.stNRXParam.TFy[i].bRef = pst3Nrx->TFy[i].bRef;
		}

		stNrx.stNRXParam_V1.stNRXManual.stNRXParam.TFy[i].TFS = pst3Nrx->TFy[i].TFS;
		stNrx.stNRXParam_V1.stNRXManual.stNRXParam.TFy[i].TDZ = pst3Nrx->TFy[i].TDZ;
		stNrx.stNRXParam_V1.stNRXManual.stNRXParam.TFy[i].TDX = pst3Nrx->TFy[i].TDX;
		stNrx.stNRXParam_V1.stNRXManual.stNRXParam.TFy[i].TSS = pst3Nrx->TFy[i].TSS;
		stNrx.stNRXParam_V1.stNRXManual.stNRXParam.TFy[i].TSI = pst3Nrx->TFy[i].TSI;
		stNrx.stNRXParam_V1.stNRXManual.stNRXParam.TFy[i].SDZ = pst3Nrx->TFy[i].SDZ;
		stNrx.stNRXParam_V1.stNRXManual.stNRXParam.TFy[i].STR = pst3Nrx->TFy[i].STR;

		stNrx.stNRXParam_V1.stNRXManual.stNRXParam.TFy[i].TFR[0] = pst3Nrx->TFy[i].TFR[0];
		stNrx.stNRXParam_V1.stNRXManual.stNRXParam.TFy[i].TFR[1] = pst3Nrx->TFy[i].TFR[1];
		stNrx.stNRXParam_V1.stNRXManual.stNRXParam.TFy[i].TFR[2] = pst3Nrx->TFy[i].TFR[2];
		stNrx.stNRXParam_V1.stNRXManual.stNRXParam.TFy[i].TFR[3] = pst3Nrx->TFy[i].TFR[3];
		stNrx.stNRXParam_V1.stNRXManual.stNRXParam.TFy[i].TFR[4] = pst3Nrx->TFy[i].TFR[4];
	}

	/*NRc*/
	stNrx.stNRXParam_V1.stNRXManual.stNRXParam.NRc.SFC = pst3Nrx->NRc.SFC;
	stNrx.stNRXParam_V1.stNRXManual.stNRXParam.NRc.TFC = pst3Nrx->NRc.TFC;
	stNrx.stNRXParam_V1.stNRXManual.stNRXParam.NRc.CSFS = pst3Nrx->NRc.CSFS;
	stNrx.stNRXParam_V1.stNRXManual.stNRXParam.NRc.CSFR = pst3Nrx->NRc.CSFR;
	stNrx.stNRXParam_V1.stNRXManual.stNRXParam.NRc.CIIR = pst3Nrx->NRc.CIIR;
	stNrx.stNRXParam_V1.stNRXManual.stNRXParam.NRc.CTFS = pst3Nrx->NRc.CTFS;
	stNrx.stNRXParam_V1.stNRXManual.stNRXParam.NRc.CTFR = pst3Nrx->NRc.CTFR;

	/*MDy*/
	for (i = 0; i < 2; i++)
	{
		stNrx.stNRXParam_V1.stNRXManual.stNRXParam.MDy[i].MAI0 = pst3Nrx->MDy[i].MAI0;
		stNrx.stNRXParam_V1.stNRXManual.stNRXParam.MDy[i].MAI1 = pst3Nrx->MDy[i].MAI1;
		stNrx.stNRXParam_V1.stNRXManual.stNRXParam.MDy[i].MAI2 = pst3Nrx->MDy[i].MAI2;

		stNrx.stNRXParam_V1.stNRXManual.stNRXParam.MDy[i].MADK = pst3Nrx->MDy[i].MADK;
		stNrx.stNRXParam_V1.stNRXManual.stNRXParam.MDy[i].MABR = pst3Nrx->MDy[i].MABR;

		stNrx.stNRXParam_V1.stNRXManual.stNRXParam.MDy[i].MADZ = pst3Nrx->MDy[i].MADZ;

		stNrx.stNRXParam_V1.stNRXManual.stNRXParam.MDy[i].MATH = pst3Nrx->MDy[i].MATH;
		stNrx.stNRXParam_V1.stNRXManual.stNRXParam.MDy[i].MATE = pst3Nrx->MDy[i].MATE;

		stNrx.stNRXParam_V1.stNRXManual.stNRXParam.MDy[i].MABW = pst3Nrx->MDy[i].MABW;
		stNrx.stNRXParam_V1.stNRXManual.stNRXParam.MDy[i].MATW = pst3Nrx->MDy[i].MATW;

		stNrx.stNRXParam_V1.stNRXManual.stNRXParam.MDy[i].MASW = pst3Nrx->MDy[i].MASW;
		stNrx.stNRXParam_V1.stNRXManual.stNRXParam.MDy[i].MAXN = pst3Nrx->MDy[i].MAXN;
	}

	for (i = 0; i < 32; i++)
    {
        stNrx.stNRXParam_V1.stNRXManual.stNRXParam.SBSk2[i] = 0;
        stNrx.stNRXParam_V1.stNRXManual.stNRXParam.SBSk3[i] = 0;
        stNrx.stNRXParam_V1.stNRXManual.stNRXParam.SDSk2[i] = 0;
        stNrx.stNRXParam_V1.stNRXManual.stNRXParam.SDSk3[i] = 0;
    }

	stNrx.enNRVer = VPSS_NR_V1;
	stNrx.stNRXParam_V1.enOptMode = OPERATION_MODE_MANUAL;
	s32Ret = HI_MPI_VPSS_SetGrpNRXParam(ViPipe, &stNrx);
	CHECK_SCENE_RET(s32Ret);

	stViNrXGet.enNRVer = VPSS_NR_V1;
	stViNrXGet.stNRXParam_V1.enOptMode = OPERATION_MODE_MANUAL;
	s32Ret = HI_MPI_VPSS_GetGrpNRXParam(ViPipe, &stViNrXGet);
	CHECK_SCENE_RET(s32Ret);

	if (MCRTmp != MCREn)
	{
		s32Ret = HI_MPI_VPSS_GetGrpAttr(ViPipe, &stGrpAttr);
		CHECK_SCENE_RET(s32Ret);

		stGrpAttr.stNrAttr.enNrMotionMode = MCREn == 0 ? 0 : 1;
		MCRTmp = MCREn;

		s32Ret	=  HI_MPI_VPSS_SetGrpAttr(ViPipe, &stGrpAttr);
		CHECK_SCENE_RET(s32Ret);
	}

    PrintNRxVPSS(&stViNrXGet.stNRXParam_V1.stNRXManual.stNRXParam);
	return HI_SUCCESS;
}

HI_S32 HI_SCENE_SetDynamic3DNR(VI_PIPE ViPipe, HI_U32 u32Iso, HI_U8 u8Index)
{
	if(HI_TRUE != g_astScenePipeParam[u8Index].stModuleState.bDyanamic3DNR)
	{
		return HI_SUCCESS;
	}

	HI_S32 s32Ret = HI_SUCCESS;
	HI_U32 u32Mid = 0, u32Left = 0, u32Right = 0;
	HI_U32 u32IsoLevel = 0;
	HI_U32 i = 0;
	HI_SCENE_3DNR_S stNrx;
	HI_SCENE_STATIC_3DNR_S st3dnrparam =g_astScenePipeParam[u8Index].stStaticThreeDNR;
	HI_U32 u32Count = st3dnrparam.u32ThreeDNRCount;
	HI_U32* pu32Thresh = (HI_U32*)st3dnrparam.au32ThreeDNRIso;
	u32IsoLevel = GetLevelLtoH(u32Iso, u32IsoLevel, u32Count, pu32Thresh);
	if (0 == u32IsoLevel)
	{
		stNrx = st3dnrparam.astThreeDNRValue[0];
	}
	else
	{
		u32Mid = MapISO(u32Iso);
		u32Left = MapISO(pu32Thresh[u32IsoLevel - 1]);
		u32Right = MapISO(pu32Thresh[u32IsoLevel]);

		/*IEy*/
		for (i = 0; i < 2; i++)
		{
			stNrx.IEy[i].IES0 = SCENE_Interpulate(u32Mid, u32Left, st3dnrparam.astThreeDNRValue[u32IsoLevel - 1].IEy[i].IES0, u32Right, st3dnrparam.astThreeDNRValue[u32IsoLevel].IEy[i].IES0);
			stNrx.IEy[i].IES1 = SCENE_Interpulate(u32Mid, u32Left, st3dnrparam.astThreeDNRValue[u32IsoLevel - 1].IEy[i].IES1, u32Right, st3dnrparam.astThreeDNRValue[u32IsoLevel].IEy[i].IES1);
			stNrx.IEy[i].IES2 = SCENE_Interpulate(u32Mid, u32Left, st3dnrparam.astThreeDNRValue[u32IsoLevel - 1].IEy[i].IES2, u32Right, st3dnrparam.astThreeDNRValue[u32IsoLevel].IEy[i].IES2);
			stNrx.IEy[i].IES3 = SCENE_Interpulate(u32Mid, u32Left, st3dnrparam.astThreeDNRValue[u32IsoLevel - 1].IEy[i].IES3, u32Right, st3dnrparam.astThreeDNRValue[u32IsoLevel].IEy[i].IES3);
			stNrx.IEy[i].IEDZ = SCENE_Interpulate(u32Mid, u32Left, st3dnrparam.astThreeDNRValue[u32IsoLevel - 1].IEy[i].IEDZ, u32Right, st3dnrparam.astThreeDNRValue[u32IsoLevel].IEy[i].IEDZ);
		}

		/*SFy*/
		for (i = 0; i < 4; i++)
		{
			stNrx.SFy[i].NRyEn = st3dnrparam.astThreeDNRValue[u32IsoLevel - 1].SFy[i].NRyEn;

			stNrx.SFy[i].SBN6 = st3dnrparam.astThreeDNRValue[u32IsoLevel - 1].SFy[i].SBN6;
			stNrx.SFy[i].PBR6 = st3dnrparam.astThreeDNRValue[u32IsoLevel - 1].SFy[i].PBR6;
			stNrx.SFy[i].SPN6 = st3dnrparam.astThreeDNRValue[u32IsoLevel - 1].SFy[i].SPN6;

			stNrx.SFy[i].SFR  = SCENE_Interpulate(u32Mid, u32Left, st3dnrparam.astThreeDNRValue[u32IsoLevel - 1].SFy[i].SFR, u32Right, st3dnrparam.astThreeDNRValue[u32IsoLevel].SFy[i].SFR);
			stNrx.SFy[i].SFS2 = SCENE_Interpulate(u32Mid, u32Left, st3dnrparam.astThreeDNRValue[u32IsoLevel - 1].SFy[i].SFS2, u32Right, st3dnrparam.astThreeDNRValue[u32IsoLevel].SFy[i].SFS2);
			stNrx.SFy[i].SFT2 = SCENE_Interpulate(u32Mid, u32Left, st3dnrparam.astThreeDNRValue[u32IsoLevel - 1].SFy[i].SFT2, u32Right, st3dnrparam.astThreeDNRValue[u32IsoLevel].SFy[i].SFT2);
			stNrx.SFy[i].SBR2 = SCENE_Interpulate(u32Mid, u32Left, st3dnrparam.astThreeDNRValue[u32IsoLevel - 1].SFy[i].SBR2, u32Right, st3dnrparam.astThreeDNRValue[u32IsoLevel].SFy[i].SBR2);

			stNrx.SFy[i].SFS4 = SCENE_Interpulate(u32Mid, u32Left, st3dnrparam.astThreeDNRValue[u32IsoLevel - 1].SFy[i].SFS4, u32Right, st3dnrparam.astThreeDNRValue[u32IsoLevel].SFy[i].SFS4);
			stNrx.SFy[i].SFT4 = SCENE_Interpulate(u32Mid, u32Left, st3dnrparam.astThreeDNRValue[u32IsoLevel - 1].SFy[i].SFT4, u32Right, st3dnrparam.astThreeDNRValue[u32IsoLevel].SFy[i].SFT4);
			stNrx.SFy[i].SBR4 = SCENE_Interpulate(u32Mid, u32Left, st3dnrparam.astThreeDNRValue[u32IsoLevel - 1].SFy[i].SBR4, u32Right, st3dnrparam.astThreeDNRValue[u32IsoLevel].SFy[i].SBR4);

            stNrx.SFy[i].SFN0 = st3dnrparam.astThreeDNRValue[u32IsoLevel - 1].SFy[i].SFN0;
            stNrx.SFy[i].SFN1 = st3dnrparam.astThreeDNRValue[u32IsoLevel - 1].SFy[i].SFN1;
            stNrx.SFy[i].SFN2 = st3dnrparam.astThreeDNRValue[u32IsoLevel - 1].SFy[i].SFN2;
            stNrx.SFy[i].SFN3 = st3dnrparam.astThreeDNRValue[u32IsoLevel - 1].SFy[i].SFN3;

			stNrx.SFy[i].STH1 = SCENE_Interpulate(u32Mid, u32Left, st3dnrparam.astThreeDNRValue[u32IsoLevel - 1].SFy[i].STH1, u32Right, st3dnrparam.astThreeDNRValue[u32IsoLevel].SFy[i].STH1);
			stNrx.SFy[i].STH2 = SCENE_Interpulate(u32Mid, u32Left, st3dnrparam.astThreeDNRValue[u32IsoLevel - 1].SFy[i].STH2, u32Right, st3dnrparam.astThreeDNRValue[u32IsoLevel].SFy[i].STH2);
			stNrx.SFy[i].STH3 = SCENE_Interpulate(u32Mid, u32Left, st3dnrparam.astThreeDNRValue[u32IsoLevel - 1].SFy[i].STH3, u32Right, st3dnrparam.astThreeDNRValue[u32IsoLevel].SFy[i].STH3);

			stNrx.SFy[i].kMode = SCENE_Interpulate(u32Mid, u32Left, st3dnrparam.astThreeDNRValue[u32IsoLevel - 1].SFy[i].kMode, u32Right, st3dnrparam.astThreeDNRValue[u32IsoLevel].SFy[i].kMode);
			stNrx.SFy[i].BWSF4 = SCENE_Interpulate(u32Mid, u32Left, st3dnrparam.astThreeDNRValue[u32IsoLevel - 1].SFy[i].BWSF4, u32Right, st3dnrparam.astThreeDNRValue[u32IsoLevel].SFy[i].BWSF4);
			stNrx.SFy[i].tEdge = SCENE_Interpulate(u32Mid, u32Left, st3dnrparam.astThreeDNRValue[u32IsoLevel - 1].SFy[i].tEdge, u32Right, st3dnrparam.astThreeDNRValue[u32IsoLevel].SFy[i].tEdge);
			stNrx.SFy[i].TriTh = SCENE_Interpulate(u32Mid, u32Left, st3dnrparam.astThreeDNRValue[u32IsoLevel - 1].SFy[i].TriTh, u32Right, st3dnrparam.astThreeDNRValue[u32IsoLevel].SFy[i].TriTh);
		}

		/*TFy*/
		for (i = 0; i < 2; i++)
		{
			if (0 == i)
			{
				stNrx.TFy[i].bRef = st3dnrparam.astThreeDNRValue[u32IsoLevel - 1].TFy[i].bRef;
			}

			stNrx.TFy[i].TFS = SCENE_Interpulate(u32Mid, u32Left, st3dnrparam.astThreeDNRValue[u32IsoLevel - 1].TFy[i].TFS, u32Right, st3dnrparam.astThreeDNRValue[u32IsoLevel].TFy[i].TFS);
			stNrx.TFy[i].TDZ = SCENE_Interpulate(u32Mid, u32Left, st3dnrparam.astThreeDNRValue[u32IsoLevel - 1].TFy[i].TDZ, u32Right, st3dnrparam.astThreeDNRValue[u32IsoLevel].TFy[i].TDZ);
			stNrx.TFy[i].TDX = SCENE_Interpulate(u32Mid, u32Left, st3dnrparam.astThreeDNRValue[u32IsoLevel - 1].TFy[i].TDX, u32Right, st3dnrparam.astThreeDNRValue[u32IsoLevel].TFy[i].TDX);
			stNrx.TFy[i].TSS = SCENE_Interpulate(u32Mid, u32Left, st3dnrparam.astThreeDNRValue[u32IsoLevel - 1].TFy[i].TSS, u32Right, st3dnrparam.astThreeDNRValue[u32IsoLevel].TFy[i].TSS);
			stNrx.TFy[i].TSI = SCENE_Interpulate(u32Mid, u32Left, st3dnrparam.astThreeDNRValue[u32IsoLevel - 1].TFy[i].TSI, u32Right, st3dnrparam.astThreeDNRValue[u32IsoLevel].TFy[i].TSI);
			stNrx.TFy[i].SDZ = SCENE_Interpulate(u32Mid, u32Left, st3dnrparam.astThreeDNRValue[u32IsoLevel - 1].TFy[i].SDZ, u32Right, st3dnrparam.astThreeDNRValue[u32IsoLevel].TFy[i].SDZ);
			stNrx.TFy[i].STR = SCENE_Interpulate(u32Mid, u32Left, st3dnrparam.astThreeDNRValue[u32IsoLevel - 1].TFy[i].STR, u32Right, st3dnrparam.astThreeDNRValue[u32IsoLevel].TFy[i].STR);
			stNrx.TFy[i].TFR[0] = SCENE_Interpulate(u32Mid, u32Left, st3dnrparam.astThreeDNRValue[u32IsoLevel - 1].TFy[i].TFR[0], u32Right, st3dnrparam.astThreeDNRValue[u32IsoLevel].TFy[i].TFR[0]);
			stNrx.TFy[i].TFR[1] = SCENE_Interpulate(u32Mid, u32Left, st3dnrparam.astThreeDNRValue[u32IsoLevel - 1].TFy[i].TFR[1], u32Right, st3dnrparam.astThreeDNRValue[u32IsoLevel].TFy[i].TFR[1]);
			stNrx.TFy[i].TFR[2] = SCENE_Interpulate(u32Mid, u32Left, st3dnrparam.astThreeDNRValue[u32IsoLevel - 1].TFy[i].TFR[2], u32Right, st3dnrparam.astThreeDNRValue[u32IsoLevel].TFy[i].TFR[2]);
			stNrx.TFy[i].TFR[3] = SCENE_Interpulate(u32Mid, u32Left, st3dnrparam.astThreeDNRValue[u32IsoLevel - 1].TFy[i].TFR[3], u32Right, st3dnrparam.astThreeDNRValue[u32IsoLevel].TFy[i].TFR[3]);
			stNrx.TFy[i].TFR[4] = SCENE_Interpulate(u32Mid, u32Left, st3dnrparam.astThreeDNRValue[u32IsoLevel - 1].TFy[i].TFR[4], u32Right, st3dnrparam.astThreeDNRValue[u32IsoLevel].TFy[i].TFR[4]);
		}

		/*NRc*/
		stNrx.NRc.SFC = SCENE_Interpulate(u32Mid, u32Left, st3dnrparam.astThreeDNRValue[u32IsoLevel - 1].NRc.SFC, u32Right, st3dnrparam.astThreeDNRValue[u32IsoLevel].NRc.SFC);
		stNrx.NRc.TFC = SCENE_Interpulate(u32Mid, u32Left, st3dnrparam.astThreeDNRValue[u32IsoLevel - 1].NRc.TFC, u32Right, st3dnrparam.astThreeDNRValue[u32IsoLevel].NRc.TFC);
		stNrx.NRc.CSFS = SCENE_Interpulate(u32Mid, u32Left, st3dnrparam.astThreeDNRValue[u32IsoLevel - 1].NRc.CSFS, u32Right, st3dnrparam.astThreeDNRValue[u32IsoLevel].NRc.CSFS);
		stNrx.NRc.CSFR = SCENE_Interpulate(u32Mid, u32Left, st3dnrparam.astThreeDNRValue[u32IsoLevel - 1].NRc.CSFR, u32Right, st3dnrparam.astThreeDNRValue[u32IsoLevel].NRc.CSFR);
		stNrx.NRc.CIIR = SCENE_Interpulate(u32Mid, u32Left, st3dnrparam.astThreeDNRValue[u32IsoLevel - 1].NRc.CIIR, u32Right, st3dnrparam.astThreeDNRValue[u32IsoLevel].NRc.CIIR);
		stNrx.NRc.CTFS = SCENE_Interpulate(u32Mid, u32Left, st3dnrparam.astThreeDNRValue[u32IsoLevel - 1].NRc.CTFS, u32Right, st3dnrparam.astThreeDNRValue[u32IsoLevel].NRc.CTFS);
		stNrx.NRc.CTFR = SCENE_Interpulate(u32Mid, u32Left, st3dnrparam.astThreeDNRValue[u32IsoLevel - 1].NRc.CTFR, u32Right, st3dnrparam.astThreeDNRValue[u32IsoLevel].NRc.CTFR);

		/*MDy*/
		for (i = 0; i < 2; i++)
		{

			stNrx.MDy[i].MAI0 = SCENE_Interpulate(u32Mid, u32Left, st3dnrparam.astThreeDNRValue[u32IsoLevel - 1].MDy[i].MAI0, u32Right, st3dnrparam.astThreeDNRValue[u32IsoLevel].MDy[i].MAI0);
			stNrx.MDy[i].MAI1 = SCENE_Interpulate(u32Mid, u32Left, st3dnrparam.astThreeDNRValue[u32IsoLevel - 1].MDy[i].MAI1, u32Right, st3dnrparam.astThreeDNRValue[u32IsoLevel].MDy[i].MAI1);
			stNrx.MDy[i].MAI2 = SCENE_Interpulate(u32Mid, u32Left, st3dnrparam.astThreeDNRValue[u32IsoLevel - 1].MDy[i].MAI2, u32Right, st3dnrparam.astThreeDNRValue[u32IsoLevel].MDy[i].MAI2);

			stNrx.MDy[i].MADK = SCENE_Interpulate(u32Mid, u32Left, st3dnrparam.astThreeDNRValue[u32IsoLevel - 1].MDy[i].MADK, u32Right, st3dnrparam.astThreeDNRValue[u32IsoLevel].MDy[i].MADK);
			stNrx.MDy[i].MABR = SCENE_Interpulate(u32Mid, u32Left, st3dnrparam.astThreeDNRValue[u32IsoLevel - 1].MDy[i].MABR, u32Right, st3dnrparam.astThreeDNRValue[u32IsoLevel].MDy[i].MABR);

			stNrx.MDy[i].MADZ = SCENE_Interpulate(u32Mid, u32Left, st3dnrparam.astThreeDNRValue[u32IsoLevel - 1].MDy[i].MADZ, u32Right, st3dnrparam.astThreeDNRValue[u32IsoLevel].MDy[i].MADZ);

			stNrx.MDy[i].MATH = SCENE_Interpulate(u32Mid, u32Left, st3dnrparam.astThreeDNRValue[u32IsoLevel - 1].MDy[i].MATH, u32Right, st3dnrparam.astThreeDNRValue[u32IsoLevel].MDy[i].MATH);
			stNrx.MDy[i].MATE = SCENE_Interpulate(u32Mid, u32Left, st3dnrparam.astThreeDNRValue[u32IsoLevel - 1].MDy[i].MATE, u32Right, st3dnrparam.astThreeDNRValue[u32IsoLevel].MDy[i].MATE);

			stNrx.MDy[i].MABW = SCENE_Interpulate(u32Mid, u32Left, st3dnrparam.astThreeDNRValue[u32IsoLevel - 1].MDy[i].MABW, u32Right, st3dnrparam.astThreeDNRValue[u32IsoLevel].MDy[i].MABW);
			stNrx.MDy[i].MATW = SCENE_Interpulate(u32Mid, u32Left, st3dnrparam.astThreeDNRValue[u32IsoLevel - 1].MDy[i].MATW, u32Right, st3dnrparam.astThreeDNRValue[u32IsoLevel].MDy[i].MATW);

			stNrx.MDy[i].MASW = SCENE_Interpulate(u32Mid, u32Left, st3dnrparam.astThreeDNRValue[u32IsoLevel - 1].MDy[i].MASW, u32Right, st3dnrparam.astThreeDNRValue[u32IsoLevel].MDy[i].MASW);
			stNrx.MDy[i].MAXN = SCENE_Interpulate(u32Mid, u32Left, st3dnrparam.astThreeDNRValue[u32IsoLevel - 1].MDy[i].MAXN, u32Right, st3dnrparam.astThreeDNRValue[u32IsoLevel].MDy[i].MAXN);

		}
	}

	s32Ret = HI_SCENE_Set3DNR(ViPipe, &stNrx);
	CHECK_SCENE_RET(s32Ret);
    return HI_SUCCESS;
}

HI_S32 HI_SCENE_SetStaticSaturation(VI_PIPE ViPipe, HI_U8 u8Index)
{
	if(HI_TRUE != g_astScenePipeParam[u8Index].stModuleState.bStaticSaturation)
	{
		return HI_SUCCESS;
	}

    HI_S32 i = 0;
    HI_S32 s32Ret = HI_SUCCESS;

    ISP_SATURATION_ATTR_S stSaturationAttr;

    s32Ret = HI_MPI_ISP_GetSaturationAttr(ViPipe, &stSaturationAttr);
    CHECK_SCENE_RET(s32Ret);

    for (i = 0; i < ISP_AUTO_ISO_STRENGTH_NUM; i++)
    {
        stSaturationAttr.stAuto.au8Sat[i] = g_astScenePipeParam[u8Index].stStaticSaturation.au8AutoSat[i];
    }

    s32Ret = HI_MPI_ISP_SetSaturationAttr(ViPipe, &stSaturationAttr);
    CHECK_SCENE_RET(s32Ret);
    return HI_SUCCESS;
}

HI_S32 HI_SCENE_SetStaticCCM(VI_PIPE ViPipe, HI_U8 u8Index)
{
	if(HI_TRUE != g_astScenePipeParam[u8Index].stModuleState.bStaticCCM)
	{
		return HI_SUCCESS;
	}

    HI_S32 i = 0;
    HI_S32 j = 0;
    HI_S32 s32Ret = HI_SUCCESS;

    ISP_COLORMATRIX_ATTR_S stColormatrixAttr;

    s32Ret = HI_MPI_ISP_GetCCMAttr(ViPipe, &stColormatrixAttr);
    CHECK_SCENE_RET(s32Ret);

    stColormatrixAttr.stAuto.u16CCMTabNum = (HI_U16)g_astScenePipeParam[u8Index].stStaticCcm.u32TotalNum;

    for (i = 0; i < g_astScenePipeParam[u8Index].stStaticCcm.u32TotalNum; i++)
    {
        stColormatrixAttr.stAuto.astCCMTab[i].u16ColorTemp = g_astScenePipeParam[u8Index].stStaticCcm.au16AutoColorTemp[i];

        for (j = 0; j < CCM_MATRIX_SIZE; j++)
        {
            stColormatrixAttr.stAuto.astCCMTab[i].au16CCM[j] = g_astScenePipeParam[u8Index].stStaticCcm.au16AutoCCM[i][j];
        }
    }

    s32Ret = HI_MPI_ISP_SetCCMAttr(ViPipe, &stColormatrixAttr);
    CHECK_SCENE_RET(s32Ret);

    return HI_SUCCESS;
}

HI_S32 HI_SCENE_SetStaticCSC(VI_PIPE ViPipe, HI_U8 u8Index)
{
	if(HI_TRUE != g_astScenePipeParam[u8Index].stModuleState.bStaticCSC)
	{
		return HI_SUCCESS;
	}

    HI_S32 s32Ret = HI_SUCCESS;

    ISP_CSC_ATTR_S stCscAttr;

    s32Ret = HI_MPI_ISP_GetCSCAttr(ViPipe, &stCscAttr);
    CHECK_SCENE_RET(s32Ret);

    stCscAttr.bEnable = g_astScenePipeParam[u8Index].stStaticCsc.bEnable;
    stCscAttr.enColorGamut = g_astScenePipeParam[u8Index].stStaticCsc.enColorGamut;

    s32Ret = HI_MPI_ISP_SetCSCAttr(ViPipe, &stCscAttr);
    CHECK_SCENE_RET(s32Ret);
    return HI_SUCCESS;
}

HI_S32 HI_SCENE_SetStaticNr(VI_PIPE ViPipe, HI_U8 u8Index)
{
	if(HI_TRUE != g_astScenePipeParam[u8Index].stModuleState.bStaticNr)
	{
		return HI_SUCCESS;
	}

    HI_S32 s32Ret = HI_SUCCESS;

    ISP_NR_ATTR_S stNrAttr;

    s32Ret = HI_MPI_ISP_GetNRAttr(ViPipe, &stNrAttr);
    CHECK_SCENE_RET(s32Ret);

    stNrAttr.bEnable = g_astScenePipeParam[u8Index].stStaticNr.bEnable;

    s32Ret = HI_MPI_ISP_SetNRAttr(ViPipe, &stNrAttr);
    CHECK_SCENE_RET(s32Ret);
    return HI_SUCCESS;
}

HI_S32 HI_SCENE_SetDynamicNr(VI_PIPE ViPipe, HI_U64 u64Exposure, HI_U64 u64LastExposure, HI_U8 u8Index)
{
    if (HI_TRUE != g_astScenePipeParam[u8Index].stModuleState.bDynamicNr)
    {
        return HI_SUCCESS;
    }

    HI_U16  au16CoringRatio[HI_ISP_BAYERNR_LUT_LENGTH] =
    {
        10, 10, 10, 15, 20, 30, 40, 40, 50, 50, 70, 90, 100, 100, 100, 100, \
        100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100
    };

    HI_U16  au16CoringRatioDef[HI_ISP_BAYERNR_LUT_LENGTH] =
    {
        100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, \
        100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100
    };

    HI_S32 i = 0;
    HI_S32 s32Ret = HI_SUCCESS;
    ISP_NR_ATTR_S stNRAttr;
    ISP_EXP_INFO_S stIspExpInfo;
    ISP_PUB_ATTR_S stPubAttr;


    s32Ret = HI_MPI_ISP_GetPubAttr(ViPipe, &stPubAttr);
    CHECK_SCENE_RET(s32Ret);

    if (WDR_MODE_NONE != stPubAttr.enWDRMode)
    {
        s32Ret = HI_MPI_ISP_QueryExposureInfo(ViPipe, &stIspExpInfo);
        CHECK_SCENE_RET(s32Ret);

        s32Ret = HI_MPI_ISP_GetNRAttr(ViPipe, &stNRAttr);
        CHECK_SCENE_RET(s32Ret);

        if (stIspExpInfo.u32ISO >= 1600)
        {
            for (i = 0; i < HI_ISP_BAYERNR_LUT_LENGTH; i++)
            {
                stNRAttr.au16CoringRatio[i] = au16CoringRatio[i];
            }
        }
        else
        {
            for (i = 0; i < HI_ISP_BAYERNR_LUT_LENGTH; i++)
            {
                stNRAttr.au16CoringRatio[i] = au16CoringRatioDef[i];
            }
        }

        s32Ret = HI_MPI_ISP_SetNRAttr(ViPipe, &stNRAttr);
        CHECK_SCENE_RET(s32Ret);
    }
    else
    {
    }

    return HI_SUCCESS;
}

HI_S32 HI_SCENE_SetDynamicDpc(VI_PIPE ViPipe, HI_U64 u64Exposure, HI_U64 u64LastExposure, HI_U8 u8Index)
{
    if (HI_TRUE != g_astScenePipeParam[u8Index].stModuleState.bDynamicDpc)
    {
        return HI_SUCCESS;
    }

    HI_S32 s32Ret = HI_SUCCESS;
    HI_S32 i = 0;
    ISP_EXP_INFO_S stIspExpInfo;
    ISP_DP_DYNAMIC_ATTR_S stDPDynamicAttr;
    ISP_PUB_ATTR_S stPubAttr;

    //HI_U16  au16Strength[ISP_AUTO_ISO_STRENGTH_NUM] =   {0, 0, 0, 152, 220, 220, 150, 150, 150, 150, 152, 152, 152, 152, 152, 152};
    //HI_U16  au16BlendRatio[ISP_AUTO_ISO_STRENGTH_NUM] = {0, 0, 0,   0,   0,   0,  10,  20,  20,  25, 50, 50, 50, 50, 50, 50};

    HI_U16  au16StrengthWdr[ISP_AUTO_ISO_STRENGTH_NUM] =   {0, 0, 0, 152, 180, 200, 220, 240, 240, 240, 240, 240, 240, 240, 240, 240};
    HI_U16  au16BlendRatioWdr[ISP_AUTO_ISO_STRENGTH_NUM] = {0, 0, 0,   0,   0,   0,  0,  0,  0,  0, 0, 0, 0, 0, 0, 0};


    s32Ret = HI_MPI_ISP_QueryExposureInfo(ViPipe, &stIspExpInfo);
    CHECK_SCENE_RET(s32Ret);

    s32Ret = HI_MPI_ISP_GetDPDynamicAttr(ViPipe, &stDPDynamicAttr);
    CHECK_SCENE_RET(s32Ret);

    s32Ret = HI_MPI_ISP_GetPubAttr(ViPipe, &stPubAttr);
    CHECK_SCENE_RET(s32Ret);

    stDPDynamicAttr.u8SoftSlope = 19;

    if (stIspExpInfo.u32ISO >= 60000)
    {
        stDPDynamicAttr.bSupTwinkleEn = 0x1;
    }
    else
    {
        stDPDynamicAttr.bSupTwinkleEn = 0x0;
    }

    if (WDR_MODE_2To1_LINE == stPubAttr.enWDRMode)
    {
        if (stIspExpInfo.u32ISO >= 20000)
        {
            stDPDynamicAttr.bSupTwinkleEn = 0x1;
        }
        else
        {
            stDPDynamicAttr.bSupTwinkleEn = 0x0;
        }

        for (i = 0; i < ISP_AUTO_ISO_STRENGTH_NUM; i++)
        {
            stDPDynamicAttr.stAuto.au16BlendRatio[i] = au16BlendRatioWdr[i];
            stDPDynamicAttr.stAuto.au16Strength[i] = au16StrengthWdr[i];
        }
    }

    s32Ret = HI_MPI_ISP_SetDPDynamicAttr(ViPipe, &stDPDynamicAttr);
    CHECK_SCENE_RET(s32Ret);

    return HI_SUCCESS;
}

HI_S32 HI_SCENE_SetDynamicBlackLevel(VI_PIPE ViPipe, HI_U64 u64Exposure, HI_U64 u64LastExposure, HI_U8 u8Index)
{
    if (HI_TRUE != g_astScenePipeParam[u8Index].stModuleState.bDynamicBLC)
    {
        return HI_SUCCESS;
    }

    HI_S32 s32Ret = HI_SUCCESS;
    HI_S32 i = 0;
    ISP_BLACK_LEVEL_S stBlackLevel;
    ISP_EXP_INFO_S stIspExpInfo;
    ISP_PUB_ATTR_S stPubAttr;

    HI_U16 BlackLevelDef[4] = {240, 240, 240, 240};
    HI_U16 BlackLevel1[4] = {240, 240, 240, 242};
    HI_U16 BlackLevel2[4] = {240, 240, 240, 245};
    HI_U16 BlackLevel3[4] = {240, 240, 240, 247};


    s32Ret = HI_MPI_ISP_GetPubAttr(ViPipe, &stPubAttr);
    CHECK_SCENE_RET(s32Ret);

    if (WDR_MODE_2To1_LINE == stPubAttr.enWDRMode)
    {
        s32Ret = HI_MPI_ISP_QueryExposureInfo(ViPipe, &stIspExpInfo);
        CHECK_SCENE_RET(s32Ret);

        s32Ret = HI_MPI_ISP_GetBlackLevelAttr(ViPipe, &stBlackLevel);
        CHECK_SCENE_RET(s32Ret);

        if (stIspExpInfo.u32ISO >= 40000)
        {
            for (i = 0; i < 4; i++)
            {
                stBlackLevel.au16BlackLevel[i] = BlackLevel3[i];
            }
        }
        else if (stIspExpInfo.u32ISO >= 30000)
        {
            for (i = 0; i < 4; i++)
            {
                stBlackLevel.au16BlackLevel[i] = BlackLevel2[i];
            }
        }
        else if (stIspExpInfo.u32ISO >= 12000)
        {
            for (i = 0; i < 4; i++)
            {
                stBlackLevel.au16BlackLevel[i] = BlackLevel1[i];
            }
        }
        else
        {
            for (i = 0; i < 4; i++)
            {
                stBlackLevel.au16BlackLevel[i] = BlackLevelDef[i];
            }
        }

        s32Ret = HI_MPI_ISP_SetBlackLevelAttr(ViPipe, &stBlackLevel);
        CHECK_SCENE_RET(s32Ret);
    }
    else {}

    return HI_SUCCESS;
}

HI_S32 HI_SCENE_SetStaticShading(VI_PIPE ViPipe, HI_U8 u8Index)
{
	if(HI_TRUE != g_astScenePipeParam[u8Index].stModuleState.bStaticShading)
	{
		return HI_SUCCESS;
	}

    HI_S32 s32Ret = HI_SUCCESS;

    ISP_SHADING_ATTR_S stShadingAttr;

    s32Ret = HI_MPI_ISP_GetMeshShadingAttr(ViPipe, &stShadingAttr);
    CHECK_SCENE_RET(s32Ret);

    stShadingAttr.bEnable = g_astScenePipeParam[u8Index].stStaticShading.bEnable;

    s32Ret = HI_MPI_ISP_SetMeshShadingAttr(ViPipe, &stShadingAttr);
    CHECK_SCENE_RET(s32Ret);
    return HI_SUCCESS;
}

HI_S32 HI_SCENE_SetDynamicShading(VI_PIPE ViPipe, HI_U64 u64Exposure, HI_U64 u64LastExposure, HI_U8 u8Index)
{
	if(HI_TRUE != g_astScenePipeParam[u8Index].stModuleState.bDynamicShading)
	{
		return HI_SUCCESS;
	}

    HI_U32 u32ExpLevel = 0;
    HI_S32 s32Ret = HI_SUCCESS;

    ISP_SHADING_ATTR_S stShadingAttr;

    if (u64Exposure != u64LastExposure)
    {
        CHECK_SCENE_PAUSE();
        s32Ret = HI_MPI_ISP_GetMeshShadingAttr(ViPipe, &stShadingAttr);
        CHECK_SCENE_RET(s32Ret);

        u32ExpLevel = SCENE_GetLevelLtoH(u64Exposure, g_astScenePipeParam[u8Index].stDynamicShading.u32ExpThreshCnt, g_astScenePipeParam[u8Index].stDynamicShading.au64ExpThreshLtoH);

        if (u32ExpLevel == 0 || u32ExpLevel == g_astScenePipeParam[u8Index].stDynamicShading.u32ExpThreshCnt - 1)
        {
            stShadingAttr.u16MeshStr = g_astScenePipeParam[u8Index].stDynamicShading.au16ManualStrength[u32ExpLevel];
        }
        else
        {
            stShadingAttr.u16MeshStr = SCENE_Interpulate(u64Exposure,
                                       g_astScenePipeParam[u8Index].stDynamicShading.au64ExpThreshLtoH[u32ExpLevel - 1],
                                       g_astScenePipeParam[u8Index].stDynamicShading.au16ManualStrength[u32ExpLevel - 1],
                                       g_astScenePipeParam[u8Index].stDynamicShading.au64ExpThreshLtoH[u32ExpLevel],
                                       g_astScenePipeParam[u8Index].stDynamicShading.au16ManualStrength[u32ExpLevel]);
        }

        CHECK_SCENE_PAUSE();
        s32Ret = HI_MPI_ISP_SetMeshShadingAttr(ViPipe, &stShadingAttr);
        CHECK_SCENE_RET(s32Ret);
    }

    return HI_SUCCESS;
}

HI_S32 HI_SCENE_SetStaticSharpen(VI_PIPE ViPipe, HI_U8 u8Index)
{
	if(HI_TRUE != g_astScenePipeParam[u8Index].stModuleState.bStaticSharpen)
	{
		return HI_SUCCESS;
	}

    HI_S32 i, j = 0;
    HI_S32 s32Ret = HI_SUCCESS;

    ISP_SHARPEN_ATTR_S stSharpenAttr;

    s32Ret = HI_MPI_ISP_GetIspSharpenAttr(ViPipe, &stSharpenAttr);
    CHECK_SCENE_RET(s32Ret);

    stSharpenAttr.bEnable = g_astScenePipeParam[u8Index].stStaticSharpen.bEnable;

    for (i = 0; i < ISP_SHARPEN_GAIN_NUM; i++)
    {
        for (j = 0; j < ISP_AUTO_ISO_STRENGTH_NUM; j++)
        {
            stSharpenAttr.stAuto.au16TextureStr[i][j] = g_astScenePipeParam[u8Index].stStaticSharpen.au16AutoTextureStr[i][j];
            stSharpenAttr.stAuto.au16EdgeStr[i][j] = g_astScenePipeParam[u8Index].stStaticSharpen.au16AutoEdgeStr[i][j];
        }
    }

    for (i = 0; i < ISP_AUTO_ISO_STRENGTH_NUM; i++)
    {
        stSharpenAttr.stAuto.au8OverShoot[i] = g_astScenePipeParam[u8Index].stStaticSharpen.au8AutoOverShoot[i];
        stSharpenAttr.stAuto.au8UnderShoot[i] = g_astScenePipeParam[u8Index].stStaticSharpen.au8AutoUnderShoot[i];
        stSharpenAttr.stAuto.au8ShootSupStr[i] = g_astScenePipeParam[u8Index].stStaticSharpen.au8ShootSupStr[i];
        stSharpenAttr.stAuto.au16TextureFreq[i] = g_astScenePipeParam[u8Index].stStaticSharpen.au16AutoTextureFreq[i];
        stSharpenAttr.stAuto.au16EdgeFreq[i] = g_astScenePipeParam[u8Index].stStaticSharpen.au16AutoEdgeFreq[i];
        stSharpenAttr.stAuto.au8DetailCtrl[i] = g_astScenePipeParam[u8Index].stStaticSharpen.au8AutoDetailCtrl[i];
        stSharpenAttr.stAuto.au8EdgeFiltStr[i] = g_astScenePipeParam[u8Index].stStaticSharpen.au8EdgeFiltStr[i];
        stSharpenAttr.stAuto.au8RGain[i] = g_astScenePipeParam[u8Index].stStaticSharpen.au8RGain[i];
        stSharpenAttr.stAuto.au8BGain[i] = g_astScenePipeParam[u8Index].stStaticSharpen.au8BGain[i];
        stSharpenAttr.stAuto.au8SkinGain[i] = g_astScenePipeParam[u8Index].stStaticSharpen.au8SkinGain[i];
    }

    s32Ret = HI_MPI_ISP_SetIspSharpenAttr(ViPipe, &stSharpenAttr);
    CHECK_SCENE_RET(s32Ret);
    return HI_SUCCESS;
}


HI_S32 HI_SCENE_SetStaticCA(VI_PIPE ViPipe, HI_U8 u8Index)
{
	if(HI_TRUE != g_astScenePipeParam[u8Index].stModuleState.bStaticCa)
	{
		return HI_SUCCESS;
	}

    HI_S32 s32Ret = HI_SUCCESS;

    ISP_CA_ATTR_S stCAAttr;

    s32Ret = HI_MPI_ISP_GetCAAttr(ViPipe, &stCAAttr);
    CHECK_SCENE_RET(s32Ret);

    stCAAttr.bEnable = g_astScenePipeParam[u8Index].stStaticCa.bEnable;

    s32Ret = HI_MPI_ISP_SetCAAttr(ViPipe, &stCAAttr);
    CHECK_SCENE_RET(s32Ret);
    return HI_SUCCESS;
}

HI_S32 HI_SCENE_SetStaticCrosstalk(VI_PIPE ViPipe, HI_U8 u8Index)
{
	if(HI_TRUE != g_astScenePipeParam[u8Index].stModuleState.bStaticCrosstalk)
	{
		return HI_SUCCESS;
	}

    HI_S32 s32Ret = HI_SUCCESS;

    ISP_CR_ATTR_S stCRAttr;

    s32Ret = HI_MPI_ISP_GetCrosstalkAttr(ViPipe, &stCRAttr);
    CHECK_SCENE_RET(s32Ret);

    stCRAttr.bEnable = g_astScenePipeParam[u8Index].stStaticCrosstalk.bEnable;

    s32Ret = HI_MPI_ISP_SetCrosstalkAttr(ViPipe, &stCRAttr);
    CHECK_SCENE_RET(s32Ret);
    return HI_SUCCESS;
}

HI_S32 HI_SCENE_SetStaticClut(VI_PIPE ViPipe, HI_U8 u8Index)
{
	if(HI_TRUE != g_astScenePipeParam[u8Index].stModuleState.bStaticClut)
	{
		return HI_SUCCESS;
	}

    HI_S32 s32Ret = HI_SUCCESS;

    ISP_CLUT_ATTR_S stClutAttr;

    s32Ret = HI_MPI_ISP_GetClutAttr(ViPipe, &stClutAttr);
    CHECK_SCENE_RET(s32Ret);

    stClutAttr.bEnable = g_astScenePipeParam[u8Index].stStaticClut.bEnable;

    s32Ret = HI_MPI_ISP_SetClutAttr(ViPipe, &stClutAttr);
    CHECK_SCENE_RET(s32Ret);
    return HI_SUCCESS;
}

HI_S32 HI_SCENE_SetDynamicVideoGamma(VI_PIPE ViPipe, HI_U64 u64Exposure, HI_U64 u64LastExposure, HI_U8 u8Index)
{
	if(HI_TRUE != g_astScenePipeParam[u8Index].stModuleState.bDynamicGamma)
	{
		return HI_SUCCESS;
	}

    HI_U32 i, j = 0;
    HI_U32 u32ExpLevel = 0;
    static HI_U32 u32LastExpLevel;
    HI_S32 s32Ret = HI_SUCCESS;

    ISP_GAMMA_ATTR_S stIspGammaAttr;
    if (u64Exposure != u64LastExposure)
    {
        if (u64Exposure > u64LastExposure)
        {
            u32ExpLevel = SCENE_GetLevelLtoH(u64Exposure, g_astScenePipeParam[u8Index].stDynamicGamma.u32TotalNum, g_astScenePipeParam[u8Index].stDynamicGamma.au64ExpThreshLtoH);
        }
        else
        {
            u32ExpLevel = SCENE_GetLevelHtoL(u64Exposure, g_astScenePipeParam[u8Index].stDynamicGamma.u32TotalNum, g_astScenePipeParam[u8Index].stDynamicGamma.au64ExpThreshHtoL);
        }

        if (u32ExpLevel != u32LastExpLevel)
        {
            for (i = 0; i < g_astScenePipeParam[u8Index].stDynamicGamma.u32InterVal; i++)
            {
                CHECK_SCENE_PAUSE();
                s32Ret = HI_MPI_ISP_GetGammaAttr(ViPipe, &stIspGammaAttr);
                CHECK_SCENE_RET(s32Ret);

                for (j = 0; j < GAMMA_NODE_NUM; j++)
                {
                    stIspGammaAttr.u16Table[j] = SCENE_TimeFilter(g_astScenePipeParam[u8Index].stDynamicGamma.au16Table[u32LastExpLevel][j],
                                                 g_astScenePipeParam[u8Index].stDynamicGamma.au16Table[u32ExpLevel][j],
                                                 g_astScenePipeParam[u8Index].stDynamicGamma.u32InterVal, i);
                }

                stIspGammaAttr.enCurveType = ISP_GAMMA_CURVE_USER_DEFINE;
                CHECK_SCENE_PAUSE();
                s32Ret = HI_MPI_ISP_SetGammaAttr(ViPipe, &stIspGammaAttr);
                CHECK_SCENE_RET(s32Ret);
                usleep(30000);
            }
            u32LastExpLevel = u32ExpLevel;
        }

    }

    return HI_SUCCESS;
}

HI_S32 HI_SCENE_SetDynamicPhotoGamma(VI_PIPE ViPipe, HI_U64 u64Exposure, HI_U64 u64LastExposure, HI_U8 u8Index)
{
	if(HI_TRUE != g_astScenePipeParam[u8Index].stModuleState.bDynamicGamma)
	{
		return HI_SUCCESS;
	}

    HI_U32 i = 0;
    HI_U32 u32ExpLevel = 0;
    HI_S32 s32Ret = HI_SUCCESS;

    ISP_GAMMA_ATTR_S stIspGammaAttr;

    if (u64Exposure != u64LastExposure)
    {
        CHECK_SCENE_PAUSE();
        s32Ret = HI_MPI_ISP_GetGammaAttr(ViPipe, &stIspGammaAttr);
        CHECK_SCENE_RET(s32Ret);

        stIspGammaAttr.enCurveType = ISP_GAMMA_CURVE_USER_DEFINE;

        for (i = 0; i < GAMMA_NODE_NUM; i++)
        {
            stIspGammaAttr.u16Table[i] = g_astScenePipeParam[u8Index].stDynamicGamma.au16Table[u32ExpLevel][i];
        }

        CHECK_SCENE_PAUSE();
        s32Ret = HI_MPI_ISP_SetGammaAttr(ViPipe, &stIspGammaAttr);
        CHECK_SCENE_RET(s32Ret);
    }

    return HI_SUCCESS;
}


HI_S32 HI_SCENE_SetDynamicFalseColor(VI_PIPE ViPipe, HI_U64 u64Exposure, HI_U64 u64LastExposure, HI_U8 u8Index)
{
	if(HI_TRUE != g_astScenePipeParam[u8Index].stModuleState.bDynamicFalseColor)
	{
		return HI_SUCCESS;
	}

    HI_S32 s32Ret = HI_SUCCESS;

    ISP_ANTIFALSECOLOR_ATTR_S stAntiFalsecolorAttr;

    if (u64Exposure != u64LastExposure)
    {
        CHECK_SCENE_PAUSE();
        s32Ret = HI_MPI_ISP_GetAntiFalseColorAttr(ViPipe, &stAntiFalsecolorAttr);
        CHECK_SCENE_RET(s32Ret);

        HI_U32 u32ExpLevel = 0;
		u32ExpLevel = SCENE_GetLevelLtoH_U32(u64Exposure, g_astScenePipeParam[u8Index].stDynamicFalsecolor.u32TotalNum, g_astScenePipeParam[u8Index].stDynamicFalsecolor.au32FalsecolorExpThresh);

		stAntiFalsecolorAttr.stManual.u8AntiFalseColorStrength = g_astScenePipeParam[u8Index].stDynamicFalsecolor.au8ManualStrength[u32ExpLevel];

        CHECK_SCENE_PAUSE();
        s32Ret = HI_MPI_ISP_SetAntiFalseColorAttr(ViPipe, &stAntiFalsecolorAttr);
        CHECK_SCENE_RET(s32Ret);
    }

    return HI_SUCCESS;
}

HI_S32 HI_SCENE_SetDynamicDIS(VI_PIPE ViPipe, VI_CHN ViChn, HI_U32 u32Iso, HI_U32 u32LastIso, HI_U8 u8Index, HI_BOOL bEnable)
{
	if(HI_TRUE != g_astScenePipeParam[u8Index].stModuleState.bDynamicDIS)
	{
		return HI_SUCCESS;
	}

    HI_S32 s32Ret = HI_SUCCESS;

    DIS_ATTR_S stDisAttr;

    if (HI_TRUE != bEnable)
    {
        return HI_SUCCESS;
    }

    if (u32Iso != u32LastIso)
    {
        CHECK_SCENE_PAUSE();
        s32Ret = HI_MPI_VI_GetChnDISAttr(ViPipe, ViChn, &stDisAttr);
        CHECK_SCENE_RET(s32Ret);

		HI_U32 u32IsoLevel = 0;
		u32IsoLevel = SCENE_GetLevelLtoH_U32(u32Iso, g_astScenePipeParam[u8Index].stDynamicDis.u32TotalNum, g_astScenePipeParam[u8Index].stDynamicDis.au32DISIsoThresh);
		stDisAttr.u32MovingSubjectLevel = g_astScenePipeParam[u8Index].stDynamicDis.au32MovingSubjectLevel[u32IsoLevel];

        CHECK_SCENE_PAUSE();
        s32Ret = HI_MPI_VI_SetChnDISAttr(ViPipe, ViChn, &stDisAttr);
        CHECK_SCENE_RET(s32Ret);
    }

    return HI_SUCCESS;
}

HI_S32 HI_SCENE_SetPipeParam(const HI_SCENE_PIPE_PARAM_S* pstScenePipeParam, HI_U32 u32Num)
{
    if (HI_NULL == pstScenePipeParam)
    {
        printf("null pointer");
        return HI_FAILURE;
    }

    memcpy(g_astScenePipeParam, pstScenePipeParam, sizeof(HI_SCENE_PIPE_PARAM_S) * u32Num);

    return HI_SUCCESS;
}

HI_S32 HI_SCENE_SetPause(HI_BOOL bPause)
{
    g_bISPPause = bPause;
    return HI_SUCCESS;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
