#include "hi_scene_setparam.h"
#include <unistd.h>
#include <string.h>
#include "mpi_awb.h"
#include "mpi_ae.h"
#include "mpi_isp.h"
#include "mpi_vi.h"
#include "mpi_venc.h"
#include "hi_comm_rc.h"
#include "hi_comm_venc.h"
#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define PIC_WIDTH_1080P   1920
#define PIC_HEIGHT_1080P  1080
#define LCU_ALIGN_H265 64
#define MB_ALIGN_H264 16

extern HI_S32 HI_MPI_VPSS_SetGrpNRXParam(VPSS_GRP VpssGrp, VPSS_GRP_NRX_PARAM_S* pstVpssNrXParam);
extern HI_S32 HI_MPI_VPSS_GetGrpNRXParam(VPSS_GRP VpssGrp, VPSS_GRP_NRX_PARAM_S* pstVpssNrXParam);

extern HI_S32 HI_MPI_VPSS_GetGrpAttr(VPSS_GRP VpssGrp, VPSS_GRP_ATTR_S* pstGrpAttr);
extern HI_S32 HI_MPI_VPSS_SetGrpAttr(VPSS_GRP VpssGrp, VPSS_GRP_ATTR_S* pstGrpAttr);

HI_SCENE_FPS_S g_stSceneAutoFps = {0};
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

static	HI_S32 iMax2(HI_S32 a, HI_S32 b) { { if (a > b) { b = a; } }; return b; }

static	HI_U32 MapISO(HI_U32 iso)
{
    HI_U32   j,  i = (iso >= 200);

    if (iso < 72) { return iMax2(iso, -3); }

    i += ( (iso >= (200 << 1)) + (iso >= (400 << 1)) + (iso >= (400 << 2)) + (iso >= (400 << 3)) + (iso >= (400 << 4)) );
    i += ( (iso >= (400 << 5)) + (iso >= (400 << 6)) + (iso >= (400 << 7)) + (iso >= (400 << 8)) + (iso >= (400 << 9)) );
    i += ( (iso >= (400 << 10)) + (iso >= (400 << 11)) + (iso >= (400 << 12)) + (iso >= (400 << 13)) + (iso >= (400 << 14))) ;
    j  = ( (iso >  (112 << i)) + (iso >  (125 << i)) + (iso >  (141 << i)) + (iso >  (158 << i)) + (iso >  (178 << i)) );

    return (i * 6 + j + (iso >= 80) + (iso >= 90) + (iso >= 100) - 3);
}

static __inline HI_U32 GetLevelLtoH(HI_U32 u32Value, HI_U32 u32Level, HI_U32 u32Count, HI_U32* pu32Thresh)
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

static __inline HI_U32 GetLevelHtoL(HI_U32 u32Value, HI_U32 u32Level, HI_U32 u32Count, HI_U32* pu32Thresh)
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
        u32Level = u32Count - 1;
    }

    return u32Level;
}

#if 0
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
#endif

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
        u32Value = u32Para0 - (HI_U32)u64Temp;
    }

    else
    {
        u64Temp = (HI_U64)(u32Para1 - u32Para0) << 8;
        u64Temp = (u64Temp * (u32Index + 1)) / SCENE_DIV_0TO1(u32TimeCnt) >> 8;
        u32Value = u32Para0 + (HI_U32)u64Temp;
    }

    return u32Value;
}

void PrintNRxVPSS( const VPSS_NRX_V3_S* pX )
{
    const tV200_VPSS_IEy  *pi = pX->IEy;
    const tV200_VPSS_SFy  *ps = pX->SFy;
    const tV200_VPSS_MDy  *pm = pX->MDy;
    const tV200_VPSS_TFy  *pt = pX->TFy;

#undef  _tmprt1_5
#define _tmprt1_5( ps,    X )  ps[0].X, ps[1].X, ps[2].X, ps[3].X, ps[4].X
#undef  _tmprt1y
#define _tmprt1y( ps,    X )  ps[1].X, ps[2].X
#undef  _tmprt1z
#define _tmprt1z( ps,a,b,X )  ps[a].X, ps[b].X

#undef  _tmprt2_5
#define _tmprt2_5( ps,    X,Y )  ps[0].X, ps[0].Y,  ps[1].X,ps[1].Y,  ps[2].X,ps[2].Y,  ps[3].X, ps[3].Y,  ps[4].X, ps[4].Y

#undef  _tmprt2y
#define _tmprt2y( ps,    X,Y )  ps[0].X, ps[0].Y,  ps[1].X,ps[1].Y,  ps[2].X,ps[2].Y
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

#undef  _tmprt3_5
#define _tmprt3_5( ps,    X,Y,Z )  _t3_( ps,0,  X,Y,Z ),_t3_( ps,1,  X,Y,Z ),_t3_( ps,2,  X,Y,Z ), _t3_( ps,3,  X,Y,Z ), _t3_( ps,4,  X,Y,Z )


#undef  _tmprt3x_
#define _tmprt3x_( ps,    X,Y,Z )  ps[0].X, ps[0].Y, ps[0].Z ,_t3_( ps,1,  X,Y,Z ),_t3_( ps,2,  X,Y,Z ),_t3_( ps,3,  X,Y,Z )

#undef  _tmprt4_5
#define _tmprt4_5( ps,  K,X,Y,Z )  _t4_( ps,0,K,X,Y,Z ),_t4_( ps,1,K,X,Y,Z ),_t4_( ps,2,K,X,Y,Z ),_t4_( ps,3,K,X,Y,Z ), _t4_( ps,4,K,X,Y,Z )

#undef  _tmprt4x_
#define _tmprt4x_( ps,  K,X,Y,Z )  ps[0].K,ps[0].X,ps[0].Y,ps[0].Z,_t4_( ps,1,K,X,Y,Z ),_t4_( ps,2,K,X,Y,Z ),_t4_( ps,3,K,X,Y,Z )


    printf("\n**********N0************|*******N1*******|*******N2*******|**************N3***************");
    printf("\n -nXsf1     %3d:%3d:%3d |    %3d:%3d:%3d |    %3d:%3d:%3d |         %3d:%3d:%3d           ",  _tmprt3_(ps, SFS1, SFT1, SBR1));
    printf("\n -nXsf2     %3d:%3d:%3d |    %3d:%3d:%3d |    %3d:%3d:%3d |         %3d:%3d:%3d           ",  _tmprt3_(ps, SFS2, SFT2, SBR2));
    printf("\n -nXsf4     %3d:%3d:%3d |    %3d:%3d:%3d |    %3d:%3d:%3d |         %3d:%3d:%3d           ",  _tmprt3_(ps, SFS4, SFT4, SBR4));
    printf("\n -SelRt         %3d:%3d |                | -kmode     %3d |                 %3d           ", ps[0].SRT0,  ps[0].SRT1, ps[2].kMode, ps[3].kMode);
    printf("\n -DeRt          %3d:%3d |                |                |                               ", ps[0].DeRate, ps[0].DeIdx);
    printf("\n -sfs5                  |                |                |         %3d:%3d:%3d           ",  ps[4].SFS1, ps[4].SFS2, ps[4].SFS4    );
    printf("\n -nXsf5 %3d:%3d:%3d:%3d |%3d:%3d:%3d:%3d |%3d:%3d:%3d:%3d |%3d:%3d:%3d:%3d|%3d:%3d:%3d:%3d",  _tmprt4_5(pi, IES0, IES1, IES2, IES3) );
    printf("\n -dzsf5             %3d |            %3d |            %3d |            %3d|            %3d",  pi[0].IEDZ, pi[1].IEDZ, pi[2].IEDZ, pi[3].IEDZ, pi[4].IEDZ);
    printf("\n -nXsf6 %3d:%3d:%3d:%3d |%3d:%3d:%3d:%3d |%3d:%3d:%3d:%3d |%3d:%3d:%3d:%3d|%3d:%3d:%3d:%3d",  _tmprt4_5( ps, SPN6, SBN6, PBR6, JMODE ));
    printf("\n -nXsfr6%3d:%3d:%3d:%3d |%3d:%3d:%3d:%3d |%3d:%3d:%3d:%3d |%3d:%3d:%3d:%3d|%3d:%3d:%3d:%3d",  _tmprt4_5( ps, SFR6[0], SFR6[1], SFR6[2], SFR6[3]));
    printf("\n -nXsbr6        %3d:%3d |        %3d:%3d |        %3d:%3d |        %3d:%3d|        %3d:%3d",  _tmprt2_5( ps, SBR6[0], SBR6[1]));
    printf("\n                        |                |                |               |               ");
    printf("\n -nXsfn     %3d:%3d:%3d |    %3d:%3d:%3d |    %3d:%3d:%3d |    %3d:%3d:%3d|    %3d:%3d:%3d",  _tmprt3_5( ps,SFN0, SFN1,  SFN2 ));
    printf("\n -nXsth         %3d:%3d |        %3d:%3d |        %3d:%3d |        %3d:%3d|        %3d:%3d",  _tmprt2_5( ps,      STH1,  STH2 ));
    printf("\n -nXsthd        %3d:%3d |        %3d:%3d |        %3d:%3d |        %3d:%3d|        %3d:%3d",  _tmprt2_5( ps,      STHd1, STHd2));
    printf("\n -sfr    (0)    %3d     |        %3d     |        %3d     |        %3d    |        %3d    ",  _tmprt1_5( ps, SFR ));
    printf("\n                        |                |                |                               ");
    printf("\n -ref           %3d     |        %3d     |                |                               ", pt[0].bRef, pt[1].bRef);
    printf("\n -tedge                 |        %3d     |        %3d     | -mXmath      %3d              ", pt[1].TED, pt[2].TED, pm[1].MATH1);
    printf("\n                        |                |                | -mXmathd     %3d              ", pm[1].MATHd1);
    printf("\n -nXstr  (1)    %3d     |        %3d:%3d |        %3d     | -mXmate      %3d              ", pt[0].STR0, pt[1].STR0, pt[1].STR1, pt[2].STR0, pm[1].MATE1);
    printf("\n -nXsdz         %3d     |        %3d:%3d |        %3d     | -mXmabw      %3d              ", pt[0].SDZ0, pt[1].SDZ0, pt[1].SDZ1, pt[2].SDZ0, pm[1].MABW1);
    printf("\n                        |                |                |                               "  );
    printf("\n -nXtss         %3d     |        %3d:%3d |        %3d     |                               ", pt[0].TSS0, pt[1].TSS0, pt[1].TSS1, pt[2].TSS0);
    printf("\n -nXtsi         %3d     |        %3d:%3d |        %3d     |                               ", pt[0].TSI0, pt[1].TSI0, pt[1].TSI1, pt[2].TSI0);
    printf("\n -nXtfs         %3d     |        %3d:%3d |        %3d     |                               ", pt[0].TFS0, pt[1].TFS0, pt[1].TFS1, pt[2].TFS0);
    printf("\n -nXtdz  (3)    %3d     |        %3d:%3d |        %3d     |**************NRc**************", pt[0].TDZ0, pt[1].TDZ0, pt[1].TDZ1, pt[2].TDZ0);
    printf("\n -nXtdx         %3d     |        %3d:%3d |        %3d     |                               ", pt[0].TDX0, pt[1].TDX0, pt[1].TDX1, pt[2].TDX0);
    printf("\n -nXtfrs        %3d     |                |                |                               ", pt[0].TFRS);
    printf("\n -nXtfr0 (2)%3d:%3d:%3d |    %3d:%3d:%3d |    %3d:%3d:%3d | -sfc         %3d              ", _t3_(pt, 0, TFR0[0], TFR0[1], TFR0[2]), _tmprt3z(pt, 1, 2, TFR0[0], TFR0[1], TFR0[2]), pX->NRc.SFC );
    printf("\n            %3d:%3d:%3d |    %3d:%3d:%3d |    %3d:%3d:%3d | -tfc         %3d              ", _t3_(pt, 0, TFR0[3], TFR0[4], TFR0[5]), _tmprt3z(pt, 1, 2, TFR0[3], TFR0[4], TFR0[5]), pX->NRc.TFC);
    printf("\n -nXtfr1 (2)            |    %3d:%3d:%3d |                | -tpc         %3d              ", _t3_(pt, 1, TFR1[0], TFR1[1], TFR1[2]), pX->NRc.TPC);
    printf("\n                        |    %3d:%3d:%3d |                | -trc         %3d              ", _t3_(pt, 1, TFR1[3], TFR1[4], TFR1[5]), pX->NRc.TRC);
    printf("\n                        |                |                |                               ");
    printf("\n -mXid0                 |    %3d:%3d:%3d |    %3d:%3d:%3d |                               ", _tmprt3z(pm, 0, 1, MAI00, MAI01, MAI02));
    printf("\n -mXid1                 |    %3d:%3d:%3d |                |                               ", pm[0].MAI10, pm[0].MAI11, pm[0].MAI12 );
    printf("\n -mXmabr                |        %3d:%3d |        %3d     |                               ", pm[0].MABR0, pm[0].MABR1, pm[1].MABR0 );
    printf("\n -AdvMath               |        %3d     |                |                               ", pm[0].AdvMATH );
    printf("\n -AdvTh                 |        %3d     |                |                               ", pm[0].AdvTH   );
    printf("\n -mXmath                |        %3d:%3d |        %3d     |                               ", pm[0].MATH0, pm[0].MATH1, pm[1].MATH0 );
    printf("\n -mXmathd               |        %3d:%3d |        %3d     |                               ", pm[0].MATHd0,pm[0].MATHd1,pm[1].MATHd0);
    printf("\n -mXmate                |        %3d:%3d |        %3d     |                               ", pm[0].MATE0, pm[0].MATE1, pm[1].MATE0 );
    printf("\n -mXmabw                |        %3d:%3d |        %3d     |                               ", pm[0].MABW0, pm[0].MABW1, pm[1].MABW0 );
    printf("\n -mXmatw                |            %3d |        %3d     |                               ", pm[0].MATW,  pm[1].MATW );
    printf("\n******************************************************************************************\n");

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

HI_S32 HI_SCENE_SetStaticAE(VI_PIPE ViPipe, HI_U8 u8Index)
{
    if (HI_TRUE != g_astScenePipeParam[u8Index].stModuleState.bStaticAE)
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
        stAeRouteEx.astRouteExNode[i].u32Again = g_astScenePipeParam[u8Index].stStaticAeRouteEx.au32Again[i];
        stAeRouteEx.astRouteExNode[i].u32Dgain = g_astScenePipeParam[u8Index].stStaticAeRouteEx.au32Dgain[i];
        stAeRouteEx.astRouteExNode[i].u32IspDgain = g_astScenePipeParam[u8Index].stStaticAeRouteEx.au32IspDgain[i];
    }

    s32Ret = HI_MPI_ISP_SetAERouteAttrEx(ViPipe, &stAeRouteEx);
    CHECK_SCENE_RET(s32Ret);

    s32Ret = HI_MPI_ISP_GetExposureAttr(ViPipe, &stExposureAttr);
    CHECK_SCENE_RET(s32Ret);

    s32Ret = HI_MPI_ISP_GetPubAttr(ViPipe, &stPubAttr);
    CHECK_SCENE_RET(s32Ret);

    i = g_astScenePipeParam[u8Index].stStaticAeRouteEx.u32TotalNum - 1;
    //stExposureAttr.stAuto.stExpTimeRange.u32Max = g_astScenePipeParam[u8Index].stStaticAeRouteEx.au32IntTime[i];

    if (WDR_MODE_NONE != stPubAttr.enWDRMode)
    {
        //stExposureAttr.stAuto.stDGainRange.u32Max = g_astScenePipeParam[u8Index].stStaticAeRouteEx.au32Dgain[i];
        //stExposureAttr.stAuto.stISPDGainRange.u32Max = g_astScenePipeParam[u8Index].stStaticAeRouteEx.au32IspDgain[i];
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
        s32Ret = HI_SCENE_SetAeWeightTab(ViPipe, u8Index);
        CHECK_SCENE_RET(s32Ret);
    }

    return HI_SUCCESS;
}

HI_S32 HI_SCENE_SetStaticWDRExposure(VI_PIPE ViPipe, HI_U8 u8Index)
{
    if (HI_TRUE != g_astScenePipeParam[u8Index].stModuleState.bStaticWdrExposure)
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

HI_S32 HI_SCENE_SetStaticFSWDR(VI_PIPE ViPipe, HI_U8 u8Index)
{
    if (HI_TRUE != g_astScenePipeParam[u8Index].stModuleState.bStaticFsWdr)
    {
        return HI_SUCCESS;
    }

    HI_S32 s32Ret = HI_SUCCESS;
    ISP_WDR_FS_ATTR_S stFSWDRAttr;

    CHECK_SCENE_PAUSE();
    s32Ret = HI_MPI_ISP_GetFSWDRAttr(ViPipe, &stFSWDRAttr);
    CHECK_SCENE_RET(s32Ret);

    stFSWDRAttr.enWDRMergeMode = g_astScenePipeParam[u8Index].stStaticFsWdr.enWDRMergeMode;

    CHECK_SCENE_PAUSE();
    s32Ret = HI_MPI_ISP_SetFSWDRAttr(ViPipe, &stFSWDRAttr);
    CHECK_SCENE_RET(s32Ret);

    return HI_SUCCESS;
}

HI_S32 HI_SCENE_SetStaticAWB(VI_PIPE ViPipe, HI_U8 u8Index)
{
    if (HI_TRUE != g_astScenePipeParam[u8Index].stModuleState.bStaticAWB)
    {
        return HI_SUCCESS;
    }

    HI_S32 i = 0;
    HI_S32 s32Ret = HI_SUCCESS;

    ISP_WB_ATTR_S stWbAttr;
    ISP_STATISTICS_CFG_S stStatisticsCfg;

    s32Ret = HI_MPI_ISP_GetWBAttr(ViPipe, &stWbAttr);
    CHECK_SCENE_RET(s32Ret);
    s32Ret = HI_MPI_ISP_GetStatisticsConfig(ViPipe, &stStatisticsCfg);
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
    stWbAttr.stAuto.stLumaHist.bEnable = g_astScenePipeParam[u8Index].stStaticAwb.u16LumaHistEnable;

    stStatisticsCfg.stWBCfg.enAWBSwitch = g_astScenePipeParam[u8Index].stStaticAwb.u16AWBSwitch;
    s32Ret = HI_MPI_ISP_SetWBAttr(ViPipe, &stWbAttr);
    CHECK_SCENE_RET(s32Ret);
    s32Ret = HI_MPI_ISP_SetStatisticsConfig(ViPipe, &stStatisticsCfg);
    CHECK_SCENE_RET(s32Ret);
    return HI_SUCCESS;
}

HI_S32 HI_SCENE_SetStaticAWBEX(VI_PIPE ViPipe, HI_U8 u8Index)
{
    if (HI_TRUE != g_astScenePipeParam[u8Index].stModuleState.bStaticAWBEx)
    {
        return HI_SUCCESS;
    }

    HI_S32 i = 0;
    HI_S32 s32Ret = HI_SUCCESS;

    ISP_WB_ATTR_S stWBAttr;
    ISP_AWB_ATTR_EX_S stAwbAttrEx;
    s32Ret = HI_MPI_ISP_GetWBAttr(ViPipe, &stWBAttr);
    CHECK_SCENE_RET(s32Ret);
    stWBAttr.bByPass = g_astScenePipeParam[u8Index].stStaticAwbEx.bBypass;
    s32Ret = HI_MPI_ISP_SetWBAttr(ViPipe, &stWBAttr);
    CHECK_SCENE_RET(s32Ret);

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

HI_S32 HI_SCENE_SetStaticSaturation(VI_PIPE ViPipe, HI_U8 u8Index)
{
    if (HI_TRUE != g_astScenePipeParam[u8Index].stModuleState.bStaticSaturation)
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
    if (HI_TRUE != g_astScenePipeParam[u8Index].stModuleState.bStaticCCM)
    {
        return HI_SUCCESS;
    }

    HI_S32 i = 0;
    HI_S32 j = 0;
    HI_S32 s32Ret = HI_SUCCESS;

    ISP_COLORMATRIX_ATTR_S stColormatrixAttr;

    s32Ret = HI_MPI_ISP_GetCCMAttr(ViPipe, &stColormatrixAttr);
    CHECK_SCENE_RET(s32Ret);
    stColormatrixAttr.enOpType = (ISP_OP_TYPE_E)g_astScenePipeParam[u8Index].stStaticCcm.u8CCMOpType;
    for (i = 0; i < CCM_MATRIX_SIZE; i++)
    {
        stColormatrixAttr.stManual.au16CCM[i] = g_astScenePipeParam[u8Index].stStaticCcm.au16ManCCM[i];
    }
    stColormatrixAttr.stAuto.bISOActEn = g_astScenePipeParam[u8Index].stStaticCcm.bAutoISOActEn;
    stColormatrixAttr.stAuto.bTempActEn= g_astScenePipeParam[u8Index].stStaticCcm.bAutoTempActEn;

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

HI_S32 HI_SCENE_SetStaticGlobalCac(VI_PIPE ViPipe, HI_U8 u8Index)
{
    if (HI_TRUE != g_astScenePipeParam[u8Index].stModuleState.bStaticGlobalCac)
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
    if (HI_TRUE != g_astScenePipeParam[u8Index].stModuleState.bStaticLocalCac)
    {
        return HI_SUCCESS;
    }

    HI_S32 i = 0;
    HI_S32 s32Ret = HI_SUCCESS;

    ISP_LOCAL_CAC_ATTR_S stLocalCac;

    s32Ret = HI_MPI_ISP_GetLocalCacAttr(ViPipe, &stLocalCac);
    CHECK_SCENE_RET(s32Ret);

    stLocalCac.bEnable = g_astScenePipeParam[u8Index].stStaticLocalCac.bEnable;
    stLocalCac.u16PurpleDetRange = g_astScenePipeParam[u8Index].stStaticLocalCac.u16PurpleDetRange;
    stLocalCac.u16VarThr = g_astScenePipeParam[u8Index].stStaticLocalCac.u16VarThr;

    for (i = 0; i < HI_SCENE_ISO_STRENGTH_NUM; i++)
    {
        stLocalCac.stAuto.au8DePurpleCrStr[i] = g_astScenePipeParam[u8Index].stStaticLocalCac.au8DePurpleCrStr[i];
        stLocalCac.stAuto.au8DePurpleCbStr[i] = g_astScenePipeParam[u8Index].stStaticLocalCac.au8DePurpleCbStr[i];
    }

    s32Ret = HI_MPI_ISP_SetLocalCacAttr(ViPipe, &stLocalCac);
    CHECK_SCENE_RET(s32Ret);

    return HI_SUCCESS;
}

HI_S32 HI_SCENE_SetStaticDPC(VI_PIPE ViPipe, HI_U8 u8Index)
{
    if (HI_TRUE != g_astScenePipeParam[u8Index].stModuleState.bStaticDPC)
    {
        return HI_SUCCESS;
    }

    HI_S32 i = 0;
    HI_S32 s32Ret = HI_SUCCESS;

    ISP_DP_DYNAMIC_ATTR_S stDpDynamic;

    s32Ret = HI_MPI_ISP_GetDPDynamicAttr(ViPipe, &stDpDynamic);
    CHECK_SCENE_RET(s32Ret);

    stDpDynamic.bEnable = g_astScenePipeParam[u8Index].stStaticDPC.bEnable;

    for (i = 0; i < HI_SCENE_ISO_STRENGTH_NUM; i++)
    {
        stDpDynamic.stAuto.au16Strength[i]   = g_astScenePipeParam[u8Index].stStaticDPC.au16Strength[i];
        stDpDynamic.stAuto.au16BlendRatio[i] = g_astScenePipeParam[u8Index].stStaticDPC.au16BlendRatio[i];
    }

    s32Ret = HI_MPI_ISP_SetDPDynamicAttr(ViPipe, &stDpDynamic);
    CHECK_SCENE_RET(s32Ret);

    return HI_SUCCESS;
}
HI_S32 HI_SCENE_SetStaticDRC(VI_PIPE ViPipe, HI_U8 u8Index)
{
    if (HI_TRUE != g_astScenePipeParam[u8Index].stModuleState.bStaticDRC)
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

HI_S32 HI_SCENE_SetStaticLDCI(VI_PIPE ViPipe, HI_U8 u8Index)
{
    if (HI_TRUE != g_astScenePipeParam[u8Index].stModuleState.bStaticLdci)
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

HI_S32 HI_SCENE_SetStaticDeHaze(VI_PIPE ViPipe, HI_U8 u8Index)
{
    if (HI_TRUE != g_astScenePipeParam[u8Index].stModuleState.bStaticDehaze)
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

HI_S32 HI_SCENE_SetStaticCSC(VI_PIPE ViPipe, HI_U8 u8Index)
{
    if (HI_TRUE != g_astScenePipeParam[u8Index].stModuleState.bStaticCSC)
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
    if (HI_TRUE != g_astScenePipeParam[u8Index].stModuleState.bStaticNr)
    {
        return HI_SUCCESS;
    }

    HI_S32 s32Ret = HI_SUCCESS;
    HI_S32 i = 0;

    ISP_NR_ATTR_S stNrAttr;

    s32Ret = HI_MPI_ISP_GetNRAttr(ViPipe, &stNrAttr);
    CHECK_SCENE_RET(s32Ret);

    stNrAttr.bEnable = g_astScenePipeParam[u8Index].stStaticNr.bEnable;
    for(i = 0; i < ISP_AUTO_ISO_STRENGTH_NUM; i++)
    {
        stNrAttr.stAuto.au8FineStr[i]    = g_astScenePipeParam[u8Index].stStaticNr.au8FineStr[i];
        stNrAttr.stAuto.au16CoringWgt[i] = g_astScenePipeParam[u8Index].stStaticNr.au16CoringWgt[i];
    }

    s32Ret = HI_MPI_ISP_SetNRAttr(ViPipe, &stNrAttr);
    CHECK_SCENE_RET(s32Ret);
    return HI_SUCCESS;
}

HI_S32 HI_SCENE_SetStaticShading(VI_PIPE ViPipe, HI_U8 u8Index)
{
    if (HI_TRUE != g_astScenePipeParam[u8Index].stModuleState.bStaticShading)
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

HI_S32 HI_SCENE_SetStaticSharpen(VI_PIPE ViPipe, HI_U8 u8Index)
{
    if (HI_TRUE != g_astScenePipeParam[u8Index].stModuleState.bStaticSharpen)
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
            stSharpenAttr.stAuto.au8LumaWgt[i][j] = g_astScenePipeParam[u8Index].stStaticSharpen.au8LumaWgt[i][j];
            stSharpenAttr.stAuto.au16TextureStr[i][j] = g_astScenePipeParam[u8Index].stStaticSharpen.au16AutoTextureStr[i][j];
            stSharpenAttr.stAuto.au16EdgeStr[i][j] = g_astScenePipeParam[u8Index].stStaticSharpen.au16AutoEdgeStr[i][j];
        }
    }

    for (i = 0; i < ISP_AUTO_ISO_STRENGTH_NUM; i++)
    {
        stSharpenAttr.stAuto.au8OverShoot[i] = g_astScenePipeParam[u8Index].stStaticSharpen.au8AutoOverShoot[i];
        stSharpenAttr.stAuto.au8UnderShoot[i] = g_astScenePipeParam[u8Index].stStaticSharpen.au8AutoUnderShoot[i];
        stSharpenAttr.stAuto.au8ShootSupStr[i] = g_astScenePipeParam[u8Index].stStaticSharpen.au8ShootSupStr[i];
        stSharpenAttr.stAuto.au8ShootSupAdj[i] = g_astScenePipeParam[u8Index].stStaticSharpen.au8ShootSupAdj[i];
        stSharpenAttr.stAuto.au16TextureFreq[i] = g_astScenePipeParam[u8Index].stStaticSharpen.au16AutoTextureFreq[i];
        stSharpenAttr.stAuto.au16EdgeFreq[i] = g_astScenePipeParam[u8Index].stStaticSharpen.au16AutoEdgeFreq[i];
        stSharpenAttr.stAuto.au8DetailCtrl[i] = g_astScenePipeParam[u8Index].stStaticSharpen.au8AutoDetailCtrl[i];
        stSharpenAttr.stAuto.au8EdgeFiltStr[i] = g_astScenePipeParam[u8Index].stStaticSharpen.au8EdgeFiltStr[i];
        stSharpenAttr.stAuto.au8EdgeFiltMaxCap[i] = g_astScenePipeParam[u8Index].stStaticSharpen.au8EdgeFiltMaxCap[i];
        stSharpenAttr.stAuto.au8RGain[i] = g_astScenePipeParam[u8Index].stStaticSharpen.au8RGain[i];
        stSharpenAttr.stAuto.au8BGain[i] = g_astScenePipeParam[u8Index].stStaticSharpen.au8BGain[i];
        stSharpenAttr.stAuto.au8GGain[i] = g_astScenePipeParam[u8Index].stStaticSharpen.au8GGain[i];
        stSharpenAttr.stAuto.au8SkinGain[i] = g_astScenePipeParam[u8Index].stStaticSharpen.au8SkinGain[i];
        stSharpenAttr.stAuto.au16MaxSharpGain[i] = g_astScenePipeParam[u8Index].stStaticSharpen.au16MaxSharpGain[i];
    }

    s32Ret = HI_MPI_ISP_SetIspSharpenAttr(ViPipe, &stSharpenAttr);
    CHECK_SCENE_RET(s32Ret);
    return HI_SUCCESS;
}

HI_S32 HI_SCENE_SetStaticCA(VI_PIPE ViPipe, HI_U8 u8Index)
{
    int i;
    if (HI_TRUE != g_astScenePipeParam[u8Index].stModuleState.bStaticCa)
    {
        return HI_SUCCESS;
    }

    HI_S32 s32Ret = HI_SUCCESS;

    ISP_CA_ATTR_S stCAAttr;

    s32Ret = HI_MPI_ISP_GetCAAttr(ViPipe, &stCAAttr);
    CHECK_SCENE_RET(s32Ret);

    stCAAttr.bEnable = g_astScenePipeParam[u8Index].stStaticCa.bEnable;
    for(i =0;i<ISP_AUTO_ISO_CA_NUM;i++)
    {
        stCAAttr.stCA.as32ISORatio[i] = g_astScenePipeParam[u8Index].stStaticCa.au16IsoRatio[i];
    }
    s32Ret = HI_MPI_ISP_SetCAAttr(ViPipe, &stCAAttr);
    CHECK_SCENE_RET(s32Ret);
    return HI_SUCCESS;
}

HI_S32 HI_SCENE_SetStaticVENC(VI_PIPE ViPipe, HI_U8 u8Index)
{
    HI_S32 s32Ret = HI_SUCCESS;

    if (HI_TRUE != g_astScenePipeParam[u8Index].stModuleState.bStaticVENC)
    {

        return HI_SUCCESS;
    }

    VENC_RC_PARAM_S  stRcParam;
    VENC_CHN_ATTR_S stVencAttr;
    //only support channel 0 settings
    //only supprot h265  cvbr and avbr param settings
    s32Ret = HI_MPI_VENC_GetRcParam(0, &stRcParam);
    CHECK_SCENE_RET(s32Ret);

    s32Ret =  HI_MPI_VENC_GetChnAttr(0, &stVencAttr);
    CHECK_SCENE_RET(s32Ret);

    if(stVencAttr.stRcAttr.enRcMode == VENC_RC_MODE_H265AVBR)
    {

        stRcParam.stParamH265AVbr.s32ChangePos = g_astScenePipeParam[u8Index].stStaticVencAttr.VENC_PARAM_H265_AVBR_S.s32ChangePos;
        stRcParam.stParamH265AVbr.u32MinIprop  = g_astScenePipeParam[u8Index].stStaticVencAttr.VENC_PARAM_H265_AVBR_S.u32MinIprop;
        stRcParam.stParamH265AVbr.u32MaxIprop  = g_astScenePipeParam[u8Index].stStaticVencAttr.VENC_PARAM_H265_AVBR_S.u32MaxIprop;
        stRcParam.stParamH265AVbr.s32MaxReEncodeTimes  = g_astScenePipeParam[u8Index].stStaticVencAttr.VENC_PARAM_H265_AVBR_S.s32MaxReEncodeTimes;
        stRcParam.stParamH265AVbr.s32MinStillPercent  = g_astScenePipeParam[u8Index].stStaticVencAttr.VENC_PARAM_H265_AVBR_S.s32MinStillPercent;
        stRcParam.stParamH265AVbr.u32MaxStillQP  = g_astScenePipeParam[u8Index].stStaticVencAttr.VENC_PARAM_H265_AVBR_S.u32MaxStillQP;
        stRcParam.stParamH265AVbr.u32MinStillPSNR  = g_astScenePipeParam[u8Index].stStaticVencAttr.VENC_PARAM_H265_AVBR_S.u32MinStillPSNR;
        stRcParam.stParamH265AVbr.u32MaxQp  = g_astScenePipeParam[u8Index].stStaticVencAttr.VENC_PARAM_H265_AVBR_S.u32MaxQp;
        stRcParam.stParamH265AVbr.u32MinQp  = g_astScenePipeParam[u8Index].stStaticVencAttr.VENC_PARAM_H265_AVBR_S.u32MinQp;
        stRcParam.stParamH265AVbr.u32MaxIQp  = g_astScenePipeParam[u8Index].stStaticVencAttr.VENC_PARAM_H265_AVBR_S.u32MaxIQp;
        stRcParam.stParamH265AVbr.u32MinIQp  = g_astScenePipeParam[u8Index].stStaticVencAttr.VENC_PARAM_H265_AVBR_S.u32MinIQp;
        stRcParam.stParamH265AVbr.u32MinQpDelta  = g_astScenePipeParam[u8Index].stStaticVencAttr.VENC_PARAM_H265_AVBR_S.u32MinQpDelta;
        stRcParam.stParamH265AVbr.u32MotionSensitivity  = g_astScenePipeParam[u8Index].stStaticVencAttr.VENC_PARAM_H265_AVBR_S.u32MotionSensitivity;
        stRcParam.stParamH265AVbr.bQpMapEn  = g_astScenePipeParam[u8Index].stStaticVencAttr.VENC_PARAM_H265_AVBR_S.bQpMapEn;
        stRcParam.stParamH265AVbr.enQpMapMode  = g_astScenePipeParam[u8Index].stStaticVencAttr.VENC_PARAM_H265_AVBR_S.enQpMapMode;

    }
    else if(stVencAttr.stRcAttr.enRcMode == VENC_RC_MODE_H265CVBR)
    {


        stRcParam.stParamH265CVbr.u32MinIprop  = g_astScenePipeParam[u8Index].stStaticVencAttr.VENC_PARAM_H265_CVBR_S.u32MinIprop;
        stRcParam.stParamH265CVbr.u32MaxIprop  = g_astScenePipeParam[u8Index].stStaticVencAttr.VENC_PARAM_H265_CVBR_S.u32MaxIprop;
        stRcParam.stParamH265CVbr.s32MaxReEncodeTimes  = g_astScenePipeParam[u8Index].stStaticVencAttr.VENC_PARAM_H265_CVBR_S.s32MaxReEncodeTimes;
        stRcParam.stParamH265CVbr.u32MaxQp  = g_astScenePipeParam[u8Index].stStaticVencAttr.VENC_PARAM_H265_CVBR_S.u32MaxQp;
        stRcParam.stParamH265CVbr.u32MinQp  = g_astScenePipeParam[u8Index].stStaticVencAttr.VENC_PARAM_H265_CVBR_S.u32MinQp;
        stRcParam.stParamH265CVbr.u32MaxIQp  = g_astScenePipeParam[u8Index].stStaticVencAttr.VENC_PARAM_H265_CVBR_S.u32MaxIQp;
        stRcParam.stParamH265CVbr.u32MinIQp  = g_astScenePipeParam[u8Index].stStaticVencAttr.VENC_PARAM_H265_CVBR_S.u32MinIQp;
        stRcParam.stParamH265CVbr.u32MinQpDelta  = g_astScenePipeParam[u8Index].stStaticVencAttr.VENC_PARAM_H265_CVBR_S.u32MinQpDelta;
        stRcParam.stParamH265CVbr.bQpMapEn  = g_astScenePipeParam[u8Index].stStaticVencAttr.VENC_PARAM_H265_CVBR_S.bQpMapEn;
        stRcParam.stParamH265CVbr.enQpMapMode  = g_astScenePipeParam[u8Index].stStaticVencAttr.VENC_PARAM_H265_CVBR_S.enQpMapMode;
        stRcParam.stParamH265CVbr.u32MaxQpDelta  = g_astScenePipeParam[u8Index].stStaticVencAttr.VENC_PARAM_H265_CVBR_S.u32MaxQpDelta;
        stRcParam.stParamH265CVbr.u32ExtraBitPercent  = g_astScenePipeParam[u8Index].stStaticVencAttr.VENC_PARAM_H265_CVBR_S.u32ExtraBitPercent;
        stRcParam.stParamH265CVbr.u32LongTermStatTimeUnit  = g_astScenePipeParam[u8Index].stStaticVencAttr.VENC_PARAM_H265_CVBR_S.u32LongTermStatTimeUnit;

    }
    else
    {
        printf("Not support Venc Format %d Param settings yet\n",stVencAttr.stRcAttr.enRcMode);
    }
    s32Ret = HI_MPI_VENC_SetRcParam(0, &stRcParam);
    CHECK_SCENE_RET(s32Ret);

    s32Ret = HI_MPI_VENC_SetChnAttr(0, &stVencAttr);
    CHECK_SCENE_RET(s32Ret);

    return HI_SUCCESS;
}
HI_S32 HI_SCENE_SetStaticCrosstalk(VI_PIPE ViPipe, HI_U8 u8Index)
{
    if (HI_TRUE != g_astScenePipeParam[u8Index].stModuleState.bStaticCrosstalk)
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
    if (HI_TRUE != g_astScenePipeParam[u8Index].stModuleState.bStaticClut)
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

HI_S32 HI_SCENE_SetStatic3DNR(VI_PIPE ViPipe, HI_U8 u8Index)
{
#if 0

    if (HI_TRUE != g_astScenePipeParam[u8Index].stModuleState.bStatic3DNR)
    {
        printf("Jump HI_SCENE_SetStatic3DNR\n");
        return HI_SUCCESS;
    }

    HI_S32 i, j, k = 0;
    HI_S32 s32Ret = HI_SUCCESS;

    VI_PIPE_NRX_PARAM_S stNrx;

    stNrx.stNRXParamV1.stNRXAutoV1.u32ParamNum = 12;
    stNrx.stNRXParamV1.stNRXAutoV1.pau32ISO = (HI_U32*)malloc(sizeof(HI_U32) * 12);
    CHECK_SCENE_NULLPTR(stNrx.stNRXParamV1.stNRXAutoV1.pau32ISO);
    stNrx.stNRXParamV1.stNRXAutoV1.pastNRXParamV1 = (VI_PIPE_NRX_PARAM_V1_S*)malloc(sizeof(VI_PIPE_NRX_PARAM_V1_S) * 12);
    CHECK_SCENE_NULLPTR(stNrx.stNRXParamV1.stNRXAutoV1.pastNRXParamV1);

    stNrx.enNRVersion = VI_NR_V1;
    stNrx.stNRXParamV1.enOptMode = OPERATION_MODE_AUTO;

    s32Ret = HI_MPI_VI_GetPipeNRXParam(ViPipe, &stNrx);

    if (HI_SUCCESS != s32Ret)
    {
        printf("HI_MPI_VI_GetPipeNRXParam failed.");
        FREE_SCENE_PTR(stNrx.stNRXParamV1.stNRXAutoV1.pau32ISO);
        FREE_SCENE_PTR(stNrx.stNRXParamV1.stNRXAutoV1.pastNRXParamV1);
        return HI_FAILURE;
    }

    memcpy(stNrx.stNRXParamV1.stNRXAutoV1.pau32ISO, g_astScenePipeParam[u8Index].stStaticThreeDNR.au32ThreeDNRIso,
           sizeof(HI_U32) * g_astScenePipeParam[u8Index].stStaticThreeDNR.u32ThreeDNRCount);

    stNrx.stNRXParamV1.stNRXAutoV1.u32ParamNum = g_astScenePipeParam[u8Index].stStaticThreeDNR.u32ThreeDNRCount;

    for (i = 0; i < stNrx.stNRXParamV1.stNRXAutoV1.u32ParamNum; i++)
    {
        stNrx.stNRXParamV1.stNRXAutoV1.pastNRXParamV1[i].IEy.IES = g_astScenePipeParam[u8Index].stStaticThreeDNR.astThreeDNRValue[i].IEy.IES;
        stNrx.stNRXParamV1.stNRXAutoV1.pastNRXParamV1[i].IEy.IESS = g_astScenePipeParam[u8Index].stStaticThreeDNR.astThreeDNRValue[i].IEy.IESS;
        stNrx.stNRXParamV1.stNRXAutoV1.pastNRXParamV1[i].IEy.IEDZ = g_astScenePipeParam[u8Index].stStaticThreeDNR.astThreeDNRValue[i].IEy.IEDZ;

        for (j = 0; j < 5; j++)
        {
            stNrx.stNRXParamV1.stNRXAutoV1.pastNRXParamV1[i].SFy[j].SBF = g_astScenePipeParam[u8Index].stStaticThreeDNR.astThreeDNRValue[i].SFy[j].SBF;
            stNrx.stNRXParamV1.stNRXAutoV1.pastNRXParamV1[i].SFy[j].STR = g_astScenePipeParam[u8Index].stStaticThreeDNR.astThreeDNRValue[i].SFy[j].STR;
            stNrx.stNRXParamV1.stNRXAutoV1.pastNRXParamV1[i].SFy[j].STHp = g_astScenePipeParam[u8Index].stStaticThreeDNR.astThreeDNRValue[i].SFy[j].STHp;
            stNrx.stNRXParamV1.stNRXAutoV1.pastNRXParamV1[i].SFy[j].SFT = g_astScenePipeParam[u8Index].stStaticThreeDNR.astThreeDNRValue[i].SFy[j].SFT;
            stNrx.stNRXParamV1.stNRXAutoV1.pastNRXParamV1[i].SFy[j].kPro = g_astScenePipeParam[u8Index].stStaticThreeDNR.astThreeDNRValue[i].SFy[j].kPro;

            for (k = 0; k < 3; k++)
            {
                stNrx.stNRXParamV1.stNRXAutoV1.pastNRXParamV1[i].SFy[j].STH[k] = g_astScenePipeParam[u8Index].stStaticThreeDNR.astThreeDNRValue[i].SFy[j].STH[k];
                stNrx.stNRXParamV1.stNRXAutoV1.pastNRXParamV1[i].SFy[j].SBS[k] = g_astScenePipeParam[u8Index].stStaticThreeDNR.astThreeDNRValue[i].SFy[j].SBS[k];
                stNrx.stNRXParamV1.stNRXAutoV1.pastNRXParamV1[i].SFy[j].SDS[k] = g_astScenePipeParam[u8Index].stStaticThreeDNR.astThreeDNRValue[i].SFy[j].SDS[k];
            }
        }

        for (j = 0; j < 2; j++)
        {
            stNrx.stNRXParamV1.stNRXAutoV1.pastNRXParamV1[i].MDy[j].MATH = g_astScenePipeParam[u8Index].stStaticThreeDNR.astThreeDNRValue[i].MDy[j].MATH;
            stNrx.stNRXParamV1.stNRXAutoV1.pastNRXParamV1[i].MDy[j].MATE = g_astScenePipeParam[u8Index].stStaticThreeDNR.astThreeDNRValue[i].MDy[j].MATE;
            stNrx.stNRXParamV1.stNRXAutoV1.pastNRXParamV1[i].MDy[j].MATW = g_astScenePipeParam[u8Index].stStaticThreeDNR.astThreeDNRValue[i].MDy[j].MATW;
            stNrx.stNRXParamV1.stNRXAutoV1.pastNRXParamV1[i].MDy[j].MASW = g_astScenePipeParam[u8Index].stStaticThreeDNR.astThreeDNRValue[i].MDy[j].MASW;
            stNrx.stNRXParamV1.stNRXAutoV1.pastNRXParamV1[i].MDy[j].MABW = g_astScenePipeParam[u8Index].stStaticThreeDNR.astThreeDNRValue[i].MDy[j].MABW;
            stNrx.stNRXParamV1.stNRXAutoV1.pastNRXParamV1[i].MDy[j].MAXN = g_astScenePipeParam[u8Index].stStaticThreeDNR.astThreeDNRValue[i].MDy[j].MAXN;
        }

        for (j = 0; j < 2; j++)
        {
            for (k = 0; k < 4; k++)
            {
                stNrx.stNRXParamV1.stNRXAutoV1.pastNRXParamV1[i].TFy[j].TFR[k] = g_astScenePipeParam[u8Index].stStaticThreeDNR.astThreeDNRValue[i].TFy[j].TFR[k];
            }

            stNrx.stNRXParamV1.stNRXAutoV1.pastNRXParamV1[i].TFy[j].TDZ = g_astScenePipeParam[u8Index].stStaticThreeDNR.astThreeDNRValue[i].TFy[j].TDZ;
            stNrx.stNRXParamV1.stNRXAutoV1.pastNRXParamV1[i].TFy[j].TDX = g_astScenePipeParam[u8Index].stStaticThreeDNR.astThreeDNRValue[i].TFy[j].TDX;
            stNrx.stNRXParamV1.stNRXAutoV1.pastNRXParamV1[i].TFy[j].TFS = g_astScenePipeParam[u8Index].stStaticThreeDNR.astThreeDNRValue[i].TFy[j].TFS;
        }

        stNrx.stNRXParamV1.stNRXAutoV1.pastNRXParamV1[i].HdgType = g_astScenePipeParam[u8Index].stStaticThreeDNR.astThreeDNRValue[i].HdgType;
        stNrx.stNRXParamV1.stNRXAutoV1.pastNRXParamV1[i].BriType = g_astScenePipeParam[u8Index].stStaticThreeDNR.astThreeDNRValue[i].BriType;
        stNrx.stNRXParamV1.stNRXAutoV1.pastNRXParamV1[i].HdgMode = g_astScenePipeParam[u8Index].stStaticThreeDNR.astThreeDNRValue[i].HdgMode;
        stNrx.stNRXParamV1.stNRXAutoV1.pastNRXParamV1[i].kTab2 = g_astScenePipeParam[u8Index].stStaticThreeDNR.astThreeDNRValue[i].kTab2;
        stNrx.stNRXParamV1.stNRXAutoV1.pastNRXParamV1[i].HdgWnd = g_astScenePipeParam[u8Index].stStaticThreeDNR.astThreeDNRValue[i].HdgWnd;
        stNrx.stNRXParamV1.stNRXAutoV1.pastNRXParamV1[i].kTab3 = g_astScenePipeParam[u8Index].stStaticThreeDNR.astThreeDNRValue[i].kTab3;
        stNrx.stNRXParamV1.stNRXAutoV1.pastNRXParamV1[i].HdgSFR = g_astScenePipeParam[u8Index].stStaticThreeDNR.astThreeDNRValue[i].HdgSFR;
        stNrx.stNRXParamV1.stNRXAutoV1.pastNRXParamV1[i].nOut = g_astScenePipeParam[u8Index].stStaticThreeDNR.astThreeDNRValue[i].nOut;
        stNrx.stNRXParamV1.stNRXAutoV1.pastNRXParamV1[i].HdgIES = g_astScenePipeParam[u8Index].stStaticThreeDNR.astThreeDNRValue[i].HdgIES;
        stNrx.stNRXParamV1.stNRXAutoV1.pastNRXParamV1[i].nRef = g_astScenePipeParam[u8Index].stStaticThreeDNR.astThreeDNRValue[i].nRef;

        for (j = 0; j < 4; j++)
        {
            stNrx.stNRXParamV1.stNRXAutoV1.pastNRXParamV1[i].SFRi[j] = g_astScenePipeParam[u8Index].stStaticThreeDNR.astThreeDNRValue[i].SFRi[j];
            stNrx.stNRXParamV1.stNRXAutoV1.pastNRXParamV1[i].SFRk[j] = g_astScenePipeParam[u8Index].stStaticThreeDNR.astThreeDNRValue[i].SFRk[j];
        }

        for (j = 0; j < 32; j++)
        {
            stNrx.stNRXParamV1.stNRXAutoV1.pastNRXParamV1[i].SBSk2[j] = g_astScenePipeParam[u8Index].stStaticThreeDNR.astThreeDNRValue[i].SBSk2[j];
            stNrx.stNRXParamV1.stNRXAutoV1.pastNRXParamV1[i].SBSk3[j] = g_astScenePipeParam[u8Index].stStaticThreeDNR.astThreeDNRValue[i].SBSk3[j];
            stNrx.stNRXParamV1.stNRXAutoV1.pastNRXParamV1[i].SDSk2[j] = g_astScenePipeParam[u8Index].stStaticThreeDNR.astThreeDNRValue[i].SDSk2[j];
            stNrx.stNRXParamV1.stNRXAutoV1.pastNRXParamV1[i].SDSk3[j] = g_astScenePipeParam[u8Index].stStaticThreeDNR.astThreeDNRValue[i].SDSk3[j];
        }

        for (j = 0; j < 16; j++)
        {
            stNrx.stNRXParamV1.stNRXAutoV1.pastNRXParamV1[i].BriThr[j] = g_astScenePipeParam[u8Index].stStaticThreeDNR.astThreeDNRValue[i].BriThr[j];
        }

        stNrx.stNRXParamV1.stNRXAutoV1.pastNRXParamV1[i].NRc.SFC = g_astScenePipeParam[u8Index].stStaticThreeDNR.astThreeDNRValue[i].NRc.SFC;
        stNrx.stNRXParamV1.stNRXAutoV1.pastNRXParamV1[i].NRc.TFC = g_astScenePipeParam[u8Index].stStaticThreeDNR.astThreeDNRValue[i].NRc.TFC;
        stNrx.stNRXParamV1.stNRXAutoV1.pastNRXParamV1[i].NRc.CSFS = g_astScenePipeParam[u8Index].stStaticThreeDNR.astThreeDNRValue[i].NRc.CSFS;
        stNrx.stNRXParamV1.stNRXAutoV1.pastNRXParamV1[i].NRc.CSFk = g_astScenePipeParam[u8Index].stStaticThreeDNR.astThreeDNRValue[i].NRc.CSFk;
        stNrx.stNRXParamV1.stNRXAutoV1.pastNRXParamV1[i].NRc.CTFS = g_astScenePipeParam[u8Index].stStaticThreeDNR.astThreeDNRValue[i].NRc.CTFS;
        stNrx.stNRXParamV1.stNRXAutoV1.pastNRXParamV1[i].NRc.CIIR = g_astScenePipeParam[u8Index].stStaticThreeDNR.astThreeDNRValue[i].NRc.CIIR;
        stNrx.stNRXParamV1.stNRXAutoV1.pastNRXParamV1[i].NRc.CTFR = g_astScenePipeParam[u8Index].stStaticThreeDNR.astThreeDNRValue[i].NRc.CTFR;
    }

    stNrx.enNRVersion = VI_NR_V1;
    stNrx.stNRXParamV1.enOptMode = OPERATION_MODE_AUTO;

    s32Ret = HI_MPI_VI_SetPipeNRXParam(ViPipe, &stNrx);

    if (HI_SUCCESS != s32Ret)
    {
        printf("HI_MPI_VI_SetPipeNRXParam failed.");
        FREE_SCENE_PTR(stNrx.stNRXParamV1.stNRXAutoV1.pau32ISO);
        FREE_SCENE_PTR(stNrx.stNRXParamV1.stNRXAutoV1.pastNRXParamV1);
        return HI_FAILURE;
    }

    FREE_SCENE_PTR(stNrx.stNRXParamV1.stNRXAutoV1.pau32ISO);
    FREE_SCENE_PTR(stNrx.stNRXParamV1.stNRXAutoV1.pastNRXParamV1);
#endif
    return HI_SUCCESS;
}

HI_S32 HI_SCENE_SetDynamicWdrExposure(VI_PIPE ViPipe, HI_U64 u64Exposure, HI_U64 u64LastExposure, HI_U8 u8Index)
{
#if 0

    if (HI_TRUE != g_astScenePipeParam[u8Index].stModuleState.bDynamicWdrExposure)
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

HI_S32 HI_SCENE_SetDynamicFps(VI_PIPE ViPipe, HI_U64 u64Exposure, HI_U64 u64LastExposure, HI_U8 u8Index)
{
    if (HI_TRUE != g_astScenePipeParam[u8Index].stModuleState.bDynamicFps)
    {
        return HI_SUCCESS;
    }

    static HI_U32 u32LastExpLevelLtH[MAX_ISP_PIPE_NUM] =  { [0 ...(ISP_MAX_PIPE_NUM - 1)] = 0xFFFF };//Max number
    static HI_U32 u32LastExpLevelHtL[MAX_ISP_PIPE_NUM] =  { [0 ...(ISP_MAX_PIPE_NUM - 1)] = 1 };
    HI_U32 u32ExpLevel = 0;
    HI_S32 s32Ret = HI_SUCCESS;
    ISP_PUB_ATTR_S stPubAttr;
    ISP_EXPOSURE_ATTR_S stExposureAttr;
    VENC_CHN_ATTR_S stVencChnAttr;
    VENC_CHN VeChn = 0;
    HI_FLOAT f32FrameRate = 0;
	HI_U32 u32Gop = 0;

    u32ExpLevel = SCENE_GetLevelLtoH_U32(u64Exposure, g_astScenePipeParam[u8Index].stDynamicFps.u8FPSExposureCnt, g_astScenePipeParam[u8Index].stDynamicFps.au32ExposureLtoHThresh);

    f32FrameRate = g_astScenePipeParam[u8Index].stDynamicFps.Fps[u32ExpLevel];
	u32Gop = g_astScenePipeParam[u8Index].stDynamicFps.u32VencGop[u32ExpLevel];

    if(HI_TRUE == g_stSceneAutoFps.bEnable)
    {
        if(HI_SCENE_OP_TYPE_AUTO == g_stSceneAutoFps.enOpType)
        {
            f32FrameRate = (g_stSceneAutoFps.stAuto.u32FpsMax < g_astScenePipeParam[u8Index].stDynamicFps.Fps[u32ExpLevel])?g_stSceneAutoFps.stAuto.u32FpsMax:f32FrameRate;
       		u32Gop = (g_stSceneAutoFps.stAuto.u32FpsMax < g_astScenePipeParam[u8Index].stDynamicFps.Fps[u32ExpLevel])?f32FrameRate*2:u32Gop;
		}
        else
        {
            f32FrameRate = g_stSceneAutoFps.stManual.u32Fps;
			u32Gop = f32FrameRate*2;
        }
    }

    if(u64Exposure > u64LastExposure)
    {
    //     u32ExpLevel = SCENE_GetLevelLtoH_U32(u64Exposure, g_astScenePipeParam[u8Index].stDynamicFps.u8FPSExposureCnt, g_astScenePipeParam[u8Index].stDynamicFps.au32ExposureLtoHThresh);
        if(u32ExpLevel == u32LastExpLevelLtH[ViPipe])
        {
            return HI_SUCCESS;
        }
        else
        {
            //Pub
            s32Ret = HI_MPI_ISP_GetPubAttr(ViPipe, &stPubAttr);
            CHECK_SCENE_RET(s32Ret);
            stPubAttr.f32FrameRate = f32FrameRate;
            s32Ret = HI_MPI_ISP_SetPubAttr(ViPipe, &stPubAttr);
            CHECK_SCENE_RET(s32Ret);
            //AE
            s32Ret = HI_MPI_ISP_GetExposureAttr(ViPipe, &stExposureAttr);
            CHECK_SCENE_RET(s32Ret);
            stExposureAttr.stAuto.stExpTimeRange.u32Max = g_astScenePipeParam[u8Index].stDynamicFps.au32AEMaxExpTime[u32ExpLevel];
            s32Ret = HI_MPI_ISP_SetExposureAttr(ViPipe, &stExposureAttr);
            CHECK_SCENE_RET(s32Ret);
            //VENC
            s32Ret = HI_MPI_VENC_GetChnAttr(VeChn,&stVencChnAttr);
            CHECK_SCENE_RET(s32Ret);
            if(stVencChnAttr.stRcAttr.enRcMode == VENC_RC_MODE_H265CBR)
            {
                stVencChnAttr.stRcAttr.stH265Cbr.fr32DstFrameRate = f32FrameRate;
                stVencChnAttr.stRcAttr.stH265Cbr.u32SrcFrameRate = f32FrameRate;
                stVencChnAttr.stRcAttr.stH265Cbr.u32Gop = u32Gop;
            }
            else if(stVencChnAttr.stRcAttr.enRcMode == VENC_RC_MODE_H265AVBR)
            {
                stVencChnAttr.stRcAttr.stH265AVbr.fr32DstFrameRate = f32FrameRate;
                stVencChnAttr.stRcAttr.stH265AVbr.u32SrcFrameRate = f32FrameRate;
                stVencChnAttr.stRcAttr.stH265AVbr.u32Gop = u32Gop;
            }
            else if(stVencChnAttr.stRcAttr.enRcMode == VENC_RC_MODE_H265CVBR)
            {
                stVencChnAttr.stRcAttr.stH265CVbr.fr32DstFrameRate = f32FrameRate;
                stVencChnAttr.stRcAttr.stH265CVbr.u32SrcFrameRate = f32FrameRate;
                stVencChnAttr.stRcAttr.stH265CVbr.u32Gop = u32Gop;
            }
            else
            {
                printf("Warning:SceneAuto now only support H265CBR and H265AVBR FPS settings.\n");
            }
            s32Ret = HI_MPI_VENC_SetChnAttr(VeChn,&stVencChnAttr);
            CHECK_SCENE_RET(s32Ret);

            u32LastExpLevelLtH[ViPipe] = u32ExpLevel;
        }
    }
    else if(u64LastExposure > u64Exposure)
        {
        // u32ExpLevel = SCENE_GetLevelLtoH_U32(u64Exposure, g_astScenePipeParam[u8Index].stDynamicFps.u8FPSExposureCnt, g_astScenePipeParam[u8Index].stDynamicFps.au32ExposureLtoHThresh);
            if(u32ExpLevel == u32LastExpLevelHtL[ViPipe])
            {
                return HI_SUCCESS;
            }
            else
            {
                //public
                s32Ret = HI_MPI_ISP_GetPubAttr(ViPipe, &stPubAttr);
                CHECK_SCENE_RET(s32Ret);
                stPubAttr.f32FrameRate = f32FrameRate;
                s32Ret = HI_MPI_ISP_SetPubAttr(ViPipe, &stPubAttr);
                CHECK_SCENE_RET(s32Ret);

                //AE
                s32Ret = HI_MPI_ISP_GetExposureAttr(ViPipe, &stExposureAttr);
                CHECK_SCENE_RET(s32Ret);
                stExposureAttr.stAuto.stExpTimeRange.u32Max = g_astScenePipeParam[u8Index].stDynamicFps.au32AEMaxExpTime[u32ExpLevel];
                s32Ret = HI_MPI_ISP_SetExposureAttr(ViPipe, &stExposureAttr);
                CHECK_SCENE_RET(s32Ret);
                //VENC
                s32Ret = HI_MPI_VENC_GetChnAttr(VeChn,&stVencChnAttr);
                CHECK_SCENE_RET(s32Ret);
                if(stVencChnAttr.stRcAttr.enRcMode == VENC_RC_MODE_H265CBR)
                {
                    stVencChnAttr.stRcAttr.stH265Cbr.fr32DstFrameRate = f32FrameRate;
                    stVencChnAttr.stRcAttr.stH265Cbr.u32SrcFrameRate = f32FrameRate;
                    stVencChnAttr.stRcAttr.stH265Cbr.u32Gop = u32Gop;
                }
                else if(stVencChnAttr.stRcAttr.enRcMode == VENC_RC_MODE_H265AVBR)
                {
                    stVencChnAttr.stRcAttr.stH265AVbr.fr32DstFrameRate = f32FrameRate;
                    stVencChnAttr.stRcAttr.stH265AVbr.u32SrcFrameRate = f32FrameRate;
                    stVencChnAttr.stRcAttr.stH265AVbr.u32Gop = u32Gop;
                }
                else if(stVencChnAttr.stRcAttr.enRcMode == VENC_RC_MODE_H265CVBR)
                {
                    stVencChnAttr.stRcAttr.stH265CVbr.fr32DstFrameRate = f32FrameRate;
                    stVencChnAttr.stRcAttr.stH265CVbr.u32SrcFrameRate = f32FrameRate;
                    stVencChnAttr.stRcAttr.stH265CVbr.u32Gop = u32Gop;
                }
                else
                {
                    printf("Warning:SceneAuto now only support H265CBR and H265AVBR FPS settings.\n");
                }

                u32LastExpLevelHtL[ViPipe] = u32ExpLevel;
            }
        }

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
	ISP_SMART_EXPOSURE_ATTR_S stSmartExpAttr;


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

        if ((WDR_MODE_NONE != stPubAttr.enWDRMode) && (stIspExpInfo.u32ISO <= 110) && (u32RefExpRatio >= g_astScenePipeParam[u8Index].stStaticWdrExposure.u32RefRatioDn)
            && (u32RefExpRatio <= g_astScenePipeParam[u8Index].stStaticWdrExposure.u32RefRatioUp) && (u64Exposure <= g_astScenePipeParam[u8Index].stStaticWdrExposure.u32ExpTh))
        {
            s32Ret = HI_MPI_ISP_GetExposureAttr(ViPipe, &stExposureAttr);
            CHECK_SCENE_RET(s32Ret);

            u32ExpLevel = SCENE_GetLevelLtoH(u64Exposure, g_astScenePipeParam[u8Index].stDynamicWdrExposure.u8AEExposureCnt, g_astScenePipeParam[u8Index].stDynamicWdrExposure.au64ExpLtoHThresh);

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

            u32ExpLevel = SCENE_GetLevelLtoH(u64Exposure, g_astScenePipeParam[u8Index].stDynamicAe.u8AEExposureCnt, g_astScenePipeParam[u8Index].stDynamicAe.au64ExpLtoHThresh);

            if (u32ExpLevel == 0)
            {
                stExposureAttr.stAuto.u8Compensation = g_astScenePipeParam[u8Index].stDynamicAe.au8AutoCompensation[u32ExpLevel];
                stExposureAttr.stAuto.u8MaxHistOffset = g_astScenePipeParam[u8Index].stDynamicAe.au8AutoMaxHistOffset[u32ExpLevel];
            }
            else
            {
                stExposureAttr.stAuto.u8Compensation = SCENE_Interpulate(u64Exposure,
                                                       g_astScenePipeParam[u8Index].stDynamicAe.au64ExpLtoHThresh[u32ExpLevel - 1],
                                                       g_astScenePipeParam[u8Index].stDynamicAe.au8AutoCompensation[u32ExpLevel - 1],
                                                       g_astScenePipeParam[u8Index].stDynamicAe.au64ExpLtoHThresh[u32ExpLevel],
                                                       g_astScenePipeParam[u8Index].stDynamicAe.au8AutoCompensation[u32ExpLevel]);

                stExposureAttr.stAuto.u8MaxHistOffset = SCENE_Interpulate(u64Exposure,
                                                        g_astScenePipeParam[u8Index].stDynamicAe.au64ExpLtoHThresh[u32ExpLevel - 1],
                                                        g_astScenePipeParam[u8Index].stDynamicAe.au8AutoMaxHistOffset[u32ExpLevel - 1],
                                                        g_astScenePipeParam[u8Index].stDynamicAe.au64ExpLtoHThresh[u32ExpLevel],
                                                        g_astScenePipeParam[u8Index].stDynamicAe.au8AutoMaxHistOffset[u32ExpLevel]);
            }
        }

        CHECK_SCENE_PAUSE();
        s32Ret = HI_MPI_ISP_SetExposureAttr(ViPipe, &stExposureAttr);
        CHECK_SCENE_RET(s32Ret);
    }

	CHECK_SCENE_PAUSE();
	s32Ret =HI_MPI_ISP_GetSmartExposureAttr(ViPipe, &stSmartExpAttr);
	CHECK_SCENE_RET(s32Ret);

    if(stSmartExpAttr.bEnable)
	{
	    ISP_WB_ATTR_S stWbAttr;

		CHECK_SCENE_PAUSE();
	    s32Ret = HI_MPI_ISP_GetWBAttr(ViPipe, &stWbAttr);
	    CHECK_SCENE_RET(s32Ret);
		s32Ret = HI_MPI_ISP_QueryExposureInfo(ViPipe, &stIspExpInfo);
        CHECK_SCENE_RET(s32Ret);

		if(stWbAttr.bByPass == HI_TRUE)
		{
		    stSmartExpAttr.bIRMode = HI_TRUE;
            stSmartExpAttr.u16ExpCoefMin = SCENE_Interpulate(stIspExpInfo.u32ISO, 100, 512, 1000, 400);
			stSmartExpAttr.u16ExpCoefMax = 1024;
		}
		else
		{
            if(stIspExpInfo.u32ISO > 150)
            {
               stSmartExpAttr.u16ExpCoefMax = SCENE_Interpulate(stIspExpInfo.u32ISO, 200, 1600, 2000, 1024);
            }
            else if(stIspExpInfo.u32Exposure > 10000)
            {
                stSmartExpAttr.u16ExpCoefMax = SCENE_Interpulate(stIspExpInfo.u32Exposure, 10000, 2048, 90000, 1600);
            }
            else
            {
                stSmartExpAttr.u16ExpCoefMax = SCENE_Interpulate(stIspExpInfo.u32Exposure, 2000, 2300, 10000, 2048);
            }
			stSmartExpAttr.u16ExpCoefMin = 1024;
		}

        CHECK_SCENE_PAUSE();
	    s32Ret =HI_MPI_ISP_SetSmartExposureAttr(ViPipe, &stSmartExpAttr);
	    CHECK_SCENE_RET(s32Ret);
	}

    return HI_SUCCESS;
}

HI_S32 HI_SCENE_SetDynamicFsWdr(VI_PIPE ViPipe, HI_U32 u32ISO, HI_U32 u32LastISO, HI_U8 u8Index, HI_U32 u32WdrRatio)
{
    if (HI_TRUE != g_astScenePipeParam[u8Index].stModuleState.bDynamicFSWDR)// || u32LastISO == u32ISO)
    {
        return HI_SUCCESS;
    }

    HI_S32 s32Ret = HI_SUCCESS;
    HI_U32 u32RatioLevel = 0;
    HI_U32 u32MidRatio, u32LeftRatio, u32RightRatio;
    HI_U32 i = 0;
    HI_U32* pu32Thresh = (HI_U32*)g_astScenePipeParam[u8Index].stDynamicFSWDR.au32RatioLtoHThresh;
    ISP_WDR_FS_ATTR_S stFSWDRAttr;

    CHECK_SCENE_PAUSE();
    s32Ret = HI_MPI_ISP_GetFSWDRAttr(ViPipe, &stFSWDRAttr);
    CHECK_SCENE_RET(s32Ret);

    u32RatioLevel = GetLevelLtoH(u32WdrRatio, u32RatioLevel, HI_SCENE_RATIO_STRENGTH_NUM, pu32Thresh);

    if (0 == u32RatioLevel)
    {
        for (i = 0; i < HI_SCENE_ISO_STRENGTH_NUM; i++)
        {
            stFSWDRAttr.stWDRCombine.stWDRMdt.stAuto.au8MdThrLowGain[i] = g_astScenePipeParam[u8Index].stDynamicFSWDR.stWDRCombine.au8MdThrLowGain[0][i];
            stFSWDRAttr.stWDRCombine.stWDRMdt.stAuto.au8MdThrHigGain[i] = g_astScenePipeParam[u8Index].stDynamicFSWDR.stWDRCombine.au8MdThrHigGain[0][i];
        }
    }
    else
    {
        u32MidRatio = u32WdrRatio;
        u32LeftRatio = pu32Thresh[u32RatioLevel - 1];
        u32RightRatio = pu32Thresh[u32RatioLevel];

        for (i = 0; i < HI_SCENE_ISO_STRENGTH_NUM; i++)
        {
            stFSWDRAttr.stWDRCombine.stWDRMdt.stAuto.au8MdThrLowGain[i] = SCENE_Interpulate(u32MidRatio,
                    u32LeftRatio, g_astScenePipeParam[u8Index].stDynamicFSWDR.stWDRCombine.au8MdThrLowGain[u32RatioLevel - 1][i],
                    u32RightRatio, g_astScenePipeParam[u8Index].stDynamicFSWDR.stWDRCombine.au8MdThrLowGain[u32RatioLevel][i]);
            stFSWDRAttr.stWDRCombine.stWDRMdt.stAuto.au8MdThrHigGain[i] = SCENE_Interpulate(u32MidRatio,
                    u32LeftRatio, g_astScenePipeParam[u8Index].stDynamicFSWDR.stWDRCombine.au8MdThrHigGain[u32RatioLevel - 1][i],
                    u32RightRatio, g_astScenePipeParam[u8Index].stDynamicFSWDR.stWDRCombine.au8MdThrHigGain[u32RatioLevel][i]);
        }
    }

    CHECK_SCENE_PAUSE();
    s32Ret = HI_MPI_ISP_SetFSWDRAttr(ViPipe, &stFSWDRAttr);
    CHECK_SCENE_RET(s32Ret);

    return HI_SUCCESS;
}

HI_S32 HI_SCENE_SetDynamicDRC(VI_PIPE ViPipe, HI_U8 u8Index, HI_U32 u32WdrRatio, HI_U32 u32Iso)
{
    if (HI_TRUE != g_astScenePipeParam[u8Index].stModuleState.bDynamicDrc)
    {
        return HI_SUCCESS;
    }

    HI_S32 i = 0;
    HI_U32 j = 0;
    HI_S32 s32Ret = HI_SUCCESS;
    HI_U32 u32IsoLevel = 0;
    HI_U32 u32RatioLevel = 0;
    HI_U32 u32Interval = g_astScenePipeParam[u8Index].stDynamicDrc.u32Interval;
    HI_U32 u32RationCount = g_astScenePipeParam[u8Index].stDynamicDrc.u32RationCount;
    HI_U32 *pu32RatioLevelThresh = g_astScenePipeParam[u8Index].stDynamicDrc.au32RationLevel;
    HI_U32 u32ISOCount = g_astScenePipeParam[u8Index].stDynamicDrc.u32ISOCount;
    HI_U32 *pu32IsoLevelThresh = g_astScenePipeParam[u8Index].stDynamicDrc.au32ISOLevel;

    static HI_U32 au32RefExpRatio[HI_SCENE_PIPETYPE_NUM][HI_SCENE_REF_EXP_RATIO_FRM] = {0};
    HI_U32 u32CurRefExpRatio = 0;
    HI_U32 u32RefRatioLevel = 0;
    HI_U32 u32RefExpRatioAlpha = 0;
    HI_U16 au16ToneMappingValue[200] = {0};

    HI_U8 u8LocalMixingBrightMax = 0;
    HI_U8 u8LocalMixingBrightMin = 0;
    HI_U8 u8LocalMixingDarkMax = 0;
    HI_U8 u8LocalMixingDarkMin = 0;
    HI_U8 u8DarkGainLmtY = 0;
    HI_U8 u8DarkGainLmtC = 0;
    HI_S8 s8DetailAdjustFactor = 0;
    HI_U8 u8SpatialFltCoef = 0;
    HI_U8 u8RangeFltCoef = 0;
    HI_U8 u8GradRevMax = 0;
    HI_U8 u8GradRevThr = 0;
    HI_U8 u8Compress = 0;
    HI_U8 u8Stretch = 0;
    HI_U16 u16Strength = 0;

    ISP_DRC_ATTR_S stIspDrcAttr;
    ISP_PUB_ATTR_S stPubAttr;
    ISP_EXP_INFO_S stExpInfo;

    CHECK_SCENE_PAUSE();
    s32Ret = HI_MPI_ISP_GetPubAttr(ViPipe, &stPubAttr);
    CHECK_SCENE_RET(s32Ret);

    if (WDR_MODE_NONE == stPubAttr.enWDRMode)
    {
        return HI_SUCCESS;
    }

    CHECK_SCENE_PAUSE();
    s32Ret = HI_MPI_ISP_GetDRCAttr(ViPipe, &stIspDrcAttr);
    CHECK_SCENE_RET(s32Ret);

    s32Ret = HI_MPI_ISP_QueryExposureInfo(ViPipe, &stExpInfo);
    CHECK_SCENE_RET(s32Ret);
    if(WDR_MODE_2To1_FRAME == stPubAttr.enWDRMode)
    {
        u32RatioLevel = SCENE_GetLevelLtoH_U32(stExpInfo.u32RefExpRatio>>6, u32RationCount, pu32RatioLevelThresh);
    }
    else
    {
        u32RatioLevel = SCENE_GetLevelLtoH_U32(u32WdrRatio, u32RationCount, pu32RatioLevelThresh);
    }
    u32IsoLevel = SCENE_GetLevelLtoH_U32(u32Iso, u32ISOCount, pu32IsoLevelThresh);

    if (u32IsoLevel == 0 || u32IsoLevel == u32ISOCount - 1)
    {
        u8LocalMixingBrightMax = g_astScenePipeParam[u8Index].stDynamicDrc.au8LocalMixingBrightMax[u32IsoLevel];
        u8LocalMixingBrightMin = g_astScenePipeParam[u8Index].stDynamicDrc.au8LocalMixingBrightMin[u32IsoLevel];
        u8LocalMixingDarkMax = g_astScenePipeParam[u8Index].stDynamicDrc.au8LocalMixingDarkMax[u32IsoLevel];
        u8LocalMixingDarkMin = g_astScenePipeParam[u8Index].stDynamicDrc.au8LocalMixingDarkMin[u32IsoLevel];
        u8DarkGainLmtY = g_astScenePipeParam[u8Index].stDynamicDrc.au8DarkGainLmtY[u32IsoLevel];
        u8DarkGainLmtC = g_astScenePipeParam[u8Index].stDynamicDrc.au8DarkGainLmtC[u32IsoLevel];
        s8DetailAdjustFactor = g_astScenePipeParam[u8Index].stDynamicDrc.as8DetailAdjustFactor[u32IsoLevel];
        u8SpatialFltCoef = g_astScenePipeParam[u8Index].stDynamicDrc.au8SpatialFltCoef[u32IsoLevel];
        u8RangeFltCoef = g_astScenePipeParam[u8Index].stDynamicDrc.au8RangeFltCoef[u32IsoLevel];
        u8GradRevMax = g_astScenePipeParam[u8Index].stDynamicDrc.au8GradRevMax[u32IsoLevel];
        u8GradRevThr = g_astScenePipeParam[u8Index].stDynamicDrc.au8GradRevThr[u32IsoLevel];
        u8Compress = g_astScenePipeParam[u8Index].stDynamicDrc.au8Compress[u32IsoLevel];
        u8Stretch = g_astScenePipeParam[u8Index].stDynamicDrc.au8Stretch[u32IsoLevel];
    }
    else
    {
        u8LocalMixingBrightMax = SCENE_Interpulate(u32Iso,
                                 pu32IsoLevelThresh[u32IsoLevel - 1], g_astScenePipeParam[u8Index].stDynamicDrc.au8LocalMixingBrightMax[u32IsoLevel - 1],
                                 pu32IsoLevelThresh[u32IsoLevel], g_astScenePipeParam[u8Index].stDynamicDrc.au8LocalMixingBrightMax[u32IsoLevel]);

        u8LocalMixingBrightMin = SCENE_Interpulate(u32Iso,
                                 pu32IsoLevelThresh[u32IsoLevel - 1], g_astScenePipeParam[u8Index].stDynamicDrc.au8LocalMixingBrightMin[u32IsoLevel - 1],
                                 pu32IsoLevelThresh[u32IsoLevel], g_astScenePipeParam[u8Index].stDynamicDrc.au8LocalMixingBrightMin[u32IsoLevel]);

        u8LocalMixingDarkMax = SCENE_Interpulate(u32Iso,
                               pu32IsoLevelThresh[u32IsoLevel - 1], g_astScenePipeParam[u8Index].stDynamicDrc.au8LocalMixingDarkMax[u32IsoLevel - 1],
                               pu32IsoLevelThresh[u32IsoLevel], g_astScenePipeParam[u8Index].stDynamicDrc.au8LocalMixingDarkMax[u32IsoLevel]);

        u8LocalMixingDarkMin = SCENE_Interpulate(u32Iso,
                               pu32IsoLevelThresh[u32IsoLevel - 1], g_astScenePipeParam[u8Index].stDynamicDrc.au8LocalMixingDarkMin[u32IsoLevel - 1],
                               pu32IsoLevelThresh[u32IsoLevel], g_astScenePipeParam[u8Index].stDynamicDrc.au8LocalMixingDarkMin[u32IsoLevel]);

        u8DarkGainLmtY = SCENE_Interpulate(u32Iso,
                         pu32IsoLevelThresh[u32IsoLevel - 1],g_astScenePipeParam[u8Index].stDynamicDrc.au8DarkGainLmtY[u32IsoLevel - 1],
                         pu32IsoLevelThresh[u32IsoLevel], g_astScenePipeParam[u8Index].stDynamicDrc.au8DarkGainLmtY[u32IsoLevel]);

        u8DarkGainLmtC = SCENE_Interpulate(u32Iso,
                         pu32IsoLevelThresh[u32IsoLevel - 1], g_astScenePipeParam[u8Index].stDynamicDrc.au8DarkGainLmtC[u32IsoLevel - 1],
                         pu32IsoLevelThresh[u32IsoLevel], g_astScenePipeParam[u8Index].stDynamicDrc.au8DarkGainLmtC[u32IsoLevel]);

        s8DetailAdjustFactor = SCENE_Interpulate(u32Iso,
                               pu32IsoLevelThresh[u32IsoLevel - 1], g_astScenePipeParam[u8Index].stDynamicDrc.as8DetailAdjustFactor[u32IsoLevel - 1],
                               pu32IsoLevelThresh[u32IsoLevel], g_astScenePipeParam[u8Index].stDynamicDrc.as8DetailAdjustFactor[u32IsoLevel]);

        u8SpatialFltCoef = SCENE_Interpulate(u32Iso,
                           pu32IsoLevelThresh[u32IsoLevel - 1], g_astScenePipeParam[u8Index].stDynamicDrc.au8SpatialFltCoef[u32IsoLevel - 1],
                           pu32IsoLevelThresh[u32IsoLevel], g_astScenePipeParam[u8Index].stDynamicDrc.au8SpatialFltCoef[u32IsoLevel]);

        u8RangeFltCoef = SCENE_Interpulate(u32Iso,
                         pu32IsoLevelThresh[u32IsoLevel - 1], g_astScenePipeParam[u8Index].stDynamicDrc.au8RangeFltCoef[u32IsoLevel - 1],
                         pu32IsoLevelThresh[u32IsoLevel], g_astScenePipeParam[u8Index].stDynamicDrc.au8RangeFltCoef[u32IsoLevel]);

        u8GradRevMax = SCENE_Interpulate(u32Iso,
                       pu32IsoLevelThresh[u32IsoLevel - 1], g_astScenePipeParam[u8Index].stDynamicDrc.au8GradRevMax[u32IsoLevel - 1],
                       pu32IsoLevelThresh[u32IsoLevel], g_astScenePipeParam[u8Index].stDynamicDrc.au8GradRevMax[u32IsoLevel]);

        u8GradRevThr = SCENE_Interpulate(u32Iso,
                       pu32IsoLevelThresh[u32IsoLevel - 1], g_astScenePipeParam[u8Index].stDynamicDrc.au8GradRevThr[u32IsoLevel - 1],
                       pu32IsoLevelThresh[u32IsoLevel], g_astScenePipeParam[u8Index].stDynamicDrc.au8GradRevThr[u32IsoLevel]);

        u8Compress = SCENE_Interpulate(u32Iso,
                     pu32IsoLevelThresh[u32IsoLevel - 1], g_astScenePipeParam[u8Index].stDynamicDrc.au8Compress[u32IsoLevel - 1],
                     pu32IsoLevelThresh[u32IsoLevel], g_astScenePipeParam[u8Index].stDynamicDrc.au8Compress[u32IsoLevel]);

        u8Stretch = SCENE_Interpulate(u32Iso,
                    pu32IsoLevelThresh[u32IsoLevel - 1], g_astScenePipeParam[u8Index].stDynamicDrc.au8Stretch[u32IsoLevel - 1],
                    pu32IsoLevelThresh[u32IsoLevel], g_astScenePipeParam[u8Index].stDynamicDrc.au8Stretch[u32IsoLevel]);
    }

    u16Strength = g_astScenePipeParam[u8Index].stDynamicDrc.u16AutoStrength[u32IsoLevel][u32RatioLevel];
    stIspDrcAttr.u8DetailDarkStep = 0;
    stIspDrcAttr.enCurveSelect = DRC_CURVE_USER;
    stIspDrcAttr.bEnable = g_astScenePipeParam[u8Index].stDynamicDrc.bEnable;

    for (i = 0; i < HI_SCENE_REF_EXP_RATIO_FRM - 1; i++)
    {
        au32RefExpRatio[ViPipe][i] = au32RefExpRatio[ViPipe][i+1];
    }
    au32RefExpRatio[ViPipe][i] = stExpInfo.u32RefExpRatio;

    for (i = 0; i < HI_SCENE_REF_EXP_RATIO_FRM; i++)
    {
        u32CurRefExpRatio += au32RefExpRatio[ViPipe][i];
    }
    u32CurRefExpRatio = u32CurRefExpRatio / HI_SCENE_REF_EXP_RATIO_FRM;
    u32RefRatioLevel = SCENE_GetLevelLtoH_U32(u32CurRefExpRatio,
                            g_astScenePipeParam[u8Index].stDynamicDrc.u32RefRatioCount,
                            g_astScenePipeParam[u8Index].stDynamicDrc.au32RefRatioLtoH);

    if (u32RefRatioLevel == 0 || u32RefRatioLevel == g_astScenePipeParam[u8Index].stDynamicDrc.u32RefRatioCount - 1)
    {
        u32RefExpRatioAlpha = g_astScenePipeParam[u8Index].stDynamicDrc.u32RefRatioAlpha[u32RefRatioLevel];
    }
    else
    {
        u32RefExpRatioAlpha = SCENE_Interpulate(u32CurRefExpRatio,
                                 g_astScenePipeParam[u8Index].stDynamicDrc.au32RefRatioLtoH[u32RefRatioLevel - 1],
                                 g_astScenePipeParam[u8Index].stDynamicDrc.u32RefRatioAlpha[u32RefRatioLevel - 1],
                                 g_astScenePipeParam[u8Index].stDynamicDrc.au32RefRatioLtoH[u32RefRatioLevel],
                                 g_astScenePipeParam[u8Index].stDynamicDrc.u32RefRatioAlpha[u32RefRatioLevel]);
    }

    for (i = 0 ; i < 200 ; i++)
    {
    	au16ToneMappingValue[i] = ((HI_U32)u32RefExpRatioAlpha * g_astScenePipeParam[u8Index].stDynamicDrc.au16TMValueLow[i]
    		+ (HI_U32)(0x100 - u32RefExpRatioAlpha) * g_astScenePipeParam[u8Index].stDynamicDrc.au16TMValueHigh[i]) >> 8;
    }

    i = u32Interval >> 3;
    {
        stIspDrcAttr.u8LocalMixingBrightMax = SCENE_TimeFilter(stIspDrcAttr.u8LocalMixingBrightMax,
                                              u8LocalMixingBrightMax, g_astScenePipeParam[u8Index].stDynamicDrc.u32Interval, i);
        stIspDrcAttr.u8LocalMixingBrightMin = SCENE_TimeFilter(stIspDrcAttr.u8LocalMixingBrightMin,
                                              u8LocalMixingBrightMin, g_astScenePipeParam[u8Index].stDynamicDrc.u32Interval, i);
        stIspDrcAttr.u8LocalMixingDarkMax = SCENE_TimeFilter(stIspDrcAttr.u8LocalMixingDarkMax,
                                            u8LocalMixingDarkMax, g_astScenePipeParam[u8Index].stDynamicDrc.u32Interval, i);
        stIspDrcAttr.u8LocalMixingDarkMin = SCENE_TimeFilter(stIspDrcAttr.u8LocalMixingDarkMin,
                                            u8LocalMixingDarkMin, g_astScenePipeParam[u8Index].stDynamicDrc.u32Interval, i);
        stIspDrcAttr.u8DarkGainLmtY = SCENE_TimeFilter(stIspDrcAttr.u8DarkGainLmtY,
                                            u8DarkGainLmtY, g_astScenePipeParam[u8Index].stDynamicDrc.u32Interval, i);
        stIspDrcAttr.u8DarkGainLmtC = SCENE_TimeFilter(stIspDrcAttr.u8DarkGainLmtC,
                                            u8DarkGainLmtC, g_astScenePipeParam[u8Index].stDynamicDrc.u32Interval, i);
        stIspDrcAttr.s8DetailAdjustFactor = SCENE_TimeFilter(stIspDrcAttr.s8DetailAdjustFactor,
                                            s8DetailAdjustFactor, g_astScenePipeParam[u8Index].stDynamicDrc.u32Interval, i);
        stIspDrcAttr.u8SpatialFltCoef = SCENE_TimeFilter(stIspDrcAttr.u8SpatialFltCoef,
                                        u8SpatialFltCoef, g_astScenePipeParam[u8Index].stDynamicDrc.u32Interval, i);
        stIspDrcAttr.u8RangeFltCoef = SCENE_TimeFilter(stIspDrcAttr.u8RangeFltCoef,
                                      u8RangeFltCoef, g_astScenePipeParam[u8Index].stDynamicDrc.u32Interval, i);
        stIspDrcAttr.u8GradRevMax = SCENE_TimeFilter(stIspDrcAttr.u8GradRevMax,
                                    u8GradRevMax, g_astScenePipeParam[u8Index].stDynamicDrc.u32Interval, i);
        stIspDrcAttr.u8GradRevThr = SCENE_TimeFilter(stIspDrcAttr.u8GradRevThr,
                                    u8GradRevThr, g_astScenePipeParam[u8Index].stDynamicDrc.u32Interval, i);
        stIspDrcAttr.stAsymmetryCurve.u8Compress = SCENE_TimeFilter(stIspDrcAttr.stAsymmetryCurve.u8Compress,
                u8Compress, g_astScenePipeParam[u8Index].stDynamicDrc.u32Interval, i);
        stIspDrcAttr.stAsymmetryCurve.u8Stretch = SCENE_TimeFilter(stIspDrcAttr.stAsymmetryCurve.u8Stretch,
                u8Stretch, g_astScenePipeParam[u8Index].stDynamicDrc.u32Interval, i);
	    if(0 == stIspDrcAttr.enOpType)
		{
        stIspDrcAttr.stAuto.u16Strength = SCENE_TimeFilter(stIspDrcAttr.stAuto.u16Strength,
                                               u16Strength, g_astScenePipeParam[u8Index].stDynamicDrc.u32Interval, i);
		}
		else
		{
			stIspDrcAttr.stManual.u16Strength = SCENE_TimeFilter(stIspDrcAttr.stManual.u16Strength,
                                               u16Strength, g_astScenePipeParam[u8Index].stDynamicDrc.u32Interval, i);
		}
        for(j = 0 ; j < 200 ; j++)
        {
            stIspDrcAttr.au16ToneMappingValue[j]  = SCENE_TimeFilter(stIspDrcAttr.au16ToneMappingValue[j],
        		                          au16ToneMappingValue[j], g_astScenePipeParam[u8Index].stDynamicDrc.u32Interval,i);
        }
        CHECK_SCENE_PAUSE();
        s32Ret = HI_MPI_ISP_SetDRCAttr(ViPipe, &stIspDrcAttr);
        CHECK_SCENE_RET(s32Ret);
    }

    return HI_SUCCESS;
}

HI_S32 HI_SCENE_SetDynamicLinearDRC(VI_PIPE ViPipe, HI_U64 u64Iso, HI_U64 u64LastIso, HI_U8 u8Index)
{
    HI_S32 s32Ret = HI_SUCCESS;
	ISP_SMART_EXPOSURE_ATTR_S stSmartExpAttr;

    CHECK_SCENE_PAUSE();
    s32Ret =HI_MPI_ISP_GetSmartExposureAttr(ViPipe, &stSmartExpAttr);
    CHECK_SCENE_RET(s32Ret);

    if ((!stSmartExpAttr.bEnable) && (HI_TRUE != g_astScenePipeParam[u8Index].stModuleState.bDynamicLinearDrc || u64Iso == u64LastIso))
    {
        return HI_SUCCESS;
    }

    HI_U32 u32IsoLevel = 0;
    HI_U32 u32ISOCount = g_astScenePipeParam[u8Index].stDynamicLinearDrc.u32ISOCount;
    HI_U32 *pu32IsoLevelThresh = g_astScenePipeParam[u8Index].stDynamicLinearDrc.au32ISOLevel;

    HI_U8 u8LocalMixingBrightMax = 0;
    HI_U8 u8LocalMixingBrightMin = 0;
    HI_U8 u8LocalMixingDarkMax = 0;
    HI_U8 u8LocalMixingDarkMin = 0;
    HI_U8 u8BrightGainLmt = 0;
    HI_U8 u8BrightGainLmtStep = 0;
    HI_U8 u8DarkGainLmtY = 0;
    HI_U8 u8DarkGainLmtC = 0;
    HI_U8 u8FltScaleCoarse = 0;
    HI_U8 u8FltScaleFine = 0;
    HI_U8 u8ContrastControl = 0;
    HI_S8 s8DetailAdjustFactor = 0;
    HI_U8 u8Asymmetry = 0;
    HI_U8 u8SecondPole = 0;
    HI_U8 u8Compress = 0;
    HI_U8 u8Stretch = 0;
    HI_U16 u16Strength = 0;

    ISP_DRC_ATTR_S stIspDrcAttr;

    CHECK_SCENE_PAUSE();
    s32Ret = HI_MPI_ISP_GetDRCAttr(ViPipe, &stIspDrcAttr);
    CHECK_SCENE_RET(s32Ret);

    stIspDrcAttr.bEnable = g_astScenePipeParam[u8Index].stDynamicLinearDrc.bEnable;

    u32IsoLevel = SCENE_GetLevelLtoH_U32(u64Iso, u32ISOCount, pu32IsoLevelThresh);
    if (u32IsoLevel == 0 || u32IsoLevel == u32ISOCount - 1)
    {
        u8LocalMixingBrightMax = g_astScenePipeParam[u8Index].stDynamicLinearDrc.au8LocalMixingBrightMax[u32IsoLevel];
        u8LocalMixingBrightMin = g_astScenePipeParam[u8Index].stDynamicLinearDrc.au8LocalMixingBrightMin[u32IsoLevel];
        u8LocalMixingDarkMax = g_astScenePipeParam[u8Index].stDynamicLinearDrc.au8LocalMixingDarkMax[u32IsoLevel];
        u8LocalMixingDarkMin = g_astScenePipeParam[u8Index].stDynamicLinearDrc.au8LocalMixingDarkMin[u32IsoLevel];
        u8BrightGainLmt = g_astScenePipeParam[u8Index].stDynamicLinearDrc.au8BrightGainLmt[u32IsoLevel];
        u8BrightGainLmtStep = g_astScenePipeParam[u8Index].stDynamicLinearDrc.au8BrightGainLmtStep[u32IsoLevel];
        u8DarkGainLmtY = g_astScenePipeParam[u8Index].stDynamicLinearDrc.au8DarkGainLmtY[u32IsoLevel];
        u8DarkGainLmtC = g_astScenePipeParam[u8Index].stDynamicLinearDrc.au8DarkGainLmtC[u32IsoLevel];
        u8FltScaleCoarse = g_astScenePipeParam[u8Index].stDynamicLinearDrc.au8FltScaleCoarse[u32IsoLevel];
        u8FltScaleFine = g_astScenePipeParam[u8Index].stDynamicLinearDrc.au8FltScaleFine[u32IsoLevel];
        u8ContrastControl = g_astScenePipeParam[u8Index].stDynamicLinearDrc.au8ContrastControl[u32IsoLevel];
        s8DetailAdjustFactor = g_astScenePipeParam[u8Index].stDynamicLinearDrc.as8DetailAdjustFactor[u32IsoLevel];
        u8Asymmetry = g_astScenePipeParam[u8Index].stDynamicLinearDrc.au8Asymmetry[u32IsoLevel];
        u8SecondPole = g_astScenePipeParam[u8Index].stDynamicLinearDrc.au8SecondPole[u32IsoLevel];
        u8Compress = g_astScenePipeParam[u8Index].stDynamicLinearDrc.au8Compress[u32IsoLevel];
        u8Stretch = g_astScenePipeParam[u8Index].stDynamicLinearDrc.au8Stretch[u32IsoLevel];
        u16Strength = g_astScenePipeParam[u8Index].stDynamicLinearDrc.au16Strength[u32IsoLevel];
    }
    else
    {
        u8LocalMixingBrightMax = SCENE_Interpulate(u64Iso,
                                 pu32IsoLevelThresh[u32IsoLevel - 1], g_astScenePipeParam[u8Index].stDynamicLinearDrc.au8LocalMixingBrightMax[u32IsoLevel - 1],
                                 pu32IsoLevelThresh[u32IsoLevel], g_astScenePipeParam[u8Index].stDynamicLinearDrc.au8LocalMixingBrightMax[u32IsoLevel]);

        u8LocalMixingBrightMin = SCENE_Interpulate(u64Iso,
                                 pu32IsoLevelThresh[u32IsoLevel - 1], g_astScenePipeParam[u8Index].stDynamicLinearDrc.au8LocalMixingBrightMin[u32IsoLevel - 1],
                                 pu32IsoLevelThresh[u32IsoLevel], g_astScenePipeParam[u8Index].stDynamicLinearDrc.au8LocalMixingBrightMin[u32IsoLevel]);

        u8LocalMixingDarkMax = SCENE_Interpulate(u64Iso,
                               pu32IsoLevelThresh[u32IsoLevel - 1], g_astScenePipeParam[u8Index].stDynamicLinearDrc.au8LocalMixingDarkMax[u32IsoLevel - 1],
                               pu32IsoLevelThresh[u32IsoLevel], g_astScenePipeParam[u8Index].stDynamicLinearDrc.au8LocalMixingDarkMax[u32IsoLevel]);

        u8LocalMixingDarkMin = SCENE_Interpulate(u64Iso,
                               pu32IsoLevelThresh[u32IsoLevel - 1], g_astScenePipeParam[u8Index].stDynamicLinearDrc.au8LocalMixingDarkMin[u32IsoLevel - 1],
                               pu32IsoLevelThresh[u32IsoLevel], g_astScenePipeParam[u8Index].stDynamicLinearDrc.au8LocalMixingDarkMin[u32IsoLevel]);

        u8BrightGainLmt = SCENE_Interpulate(u64Iso,
                         pu32IsoLevelThresh[u32IsoLevel - 1],g_astScenePipeParam[u8Index].stDynamicLinearDrc.au8BrightGainLmt[u32IsoLevel - 1],
                         pu32IsoLevelThresh[u32IsoLevel], g_astScenePipeParam[u8Index].stDynamicLinearDrc.au8BrightGainLmt[u32IsoLevel]);

        u8BrightGainLmtStep = SCENE_Interpulate(u64Iso,
                         pu32IsoLevelThresh[u32IsoLevel - 1], g_astScenePipeParam[u8Index].stDynamicLinearDrc.au8BrightGainLmtStep[u32IsoLevel - 1],
                         pu32IsoLevelThresh[u32IsoLevel], g_astScenePipeParam[u8Index].stDynamicLinearDrc.au8BrightGainLmtStep[u32IsoLevel]);
        u8DarkGainLmtY = SCENE_Interpulate(u64Iso,
                         pu32IsoLevelThresh[u32IsoLevel - 1],g_astScenePipeParam[u8Index].stDynamicLinearDrc.au8DarkGainLmtY[u32IsoLevel - 1],
                         pu32IsoLevelThresh[u32IsoLevel], g_astScenePipeParam[u8Index].stDynamicLinearDrc.au8DarkGainLmtY[u32IsoLevel]);
        u8DarkGainLmtC = SCENE_Interpulate(u64Iso,
                         pu32IsoLevelThresh[u32IsoLevel - 1], g_astScenePipeParam[u8Index].stDynamicLinearDrc.au8DarkGainLmtC[u32IsoLevel - 1],
                         pu32IsoLevelThresh[u32IsoLevel], g_astScenePipeParam[u8Index].stDynamicLinearDrc.au8DarkGainLmtC[u32IsoLevel]);

        u8FltScaleCoarse = SCENE_Interpulate(u64Iso,
                           pu32IsoLevelThresh[u32IsoLevel - 1], g_astScenePipeParam[u8Index].stDynamicLinearDrc.au8FltScaleCoarse[u32IsoLevel - 1],
                           pu32IsoLevelThresh[u32IsoLevel], g_astScenePipeParam[u8Index].stDynamicLinearDrc.au8FltScaleCoarse[u32IsoLevel]);

        u8FltScaleFine = SCENE_Interpulate(u64Iso,
                         pu32IsoLevelThresh[u32IsoLevel - 1], g_astScenePipeParam[u8Index].stDynamicLinearDrc.au8FltScaleFine[u32IsoLevel - 1],
                         pu32IsoLevelThresh[u32IsoLevel], g_astScenePipeParam[u8Index].stDynamicLinearDrc.au8FltScaleFine[u32IsoLevel]);

        u8ContrastControl = SCENE_Interpulate(u64Iso,
                       pu32IsoLevelThresh[u32IsoLevel - 1], g_astScenePipeParam[u8Index].stDynamicLinearDrc.au8ContrastControl[u32IsoLevel - 1],
                       pu32IsoLevelThresh[u32IsoLevel], g_astScenePipeParam[u8Index].stDynamicLinearDrc.au8ContrastControl[u32IsoLevel]);

        s8DetailAdjustFactor = SCENE_Interpulate(u64Iso,
                               pu32IsoLevelThresh[u32IsoLevel - 1], g_astScenePipeParam[u8Index].stDynamicLinearDrc.as8DetailAdjustFactor[u32IsoLevel - 1],
                               pu32IsoLevelThresh[u32IsoLevel], g_astScenePipeParam[u8Index].stDynamicLinearDrc.as8DetailAdjustFactor[u32IsoLevel]);

        u8Asymmetry = SCENE_Interpulate(u64Iso,
                       pu32IsoLevelThresh[u32IsoLevel - 1], g_astScenePipeParam[u8Index].stDynamicLinearDrc.au8Asymmetry[u32IsoLevel - 1],
                       pu32IsoLevelThresh[u32IsoLevel], g_astScenePipeParam[u8Index].stDynamicLinearDrc.au8Asymmetry[u32IsoLevel]);

        u8SecondPole = SCENE_Interpulate(u64Iso,
                       pu32IsoLevelThresh[u32IsoLevel - 1], g_astScenePipeParam[u8Index].stDynamicLinearDrc.au8SecondPole[u32IsoLevel - 1],
                       pu32IsoLevelThresh[u32IsoLevel], g_astScenePipeParam[u8Index].stDynamicLinearDrc.au8SecondPole[u32IsoLevel]);

        u8Compress = SCENE_Interpulate(u64Iso,
                     pu32IsoLevelThresh[u32IsoLevel - 1], g_astScenePipeParam[u8Index].stDynamicLinearDrc.au8Compress[u32IsoLevel - 1],
                     pu32IsoLevelThresh[u32IsoLevel], g_astScenePipeParam[u8Index].stDynamicLinearDrc.au8Compress[u32IsoLevel]);

        u8Stretch = SCENE_Interpulate(u64Iso,
                    pu32IsoLevelThresh[u32IsoLevel - 1], g_astScenePipeParam[u8Index].stDynamicLinearDrc.au8Stretch[u32IsoLevel - 1],
                    pu32IsoLevelThresh[u32IsoLevel], g_astScenePipeParam[u8Index].stDynamicLinearDrc.au8Stretch[u32IsoLevel]);

        u16Strength = SCENE_Interpulate(u64Iso,
                    pu32IsoLevelThresh[u32IsoLevel - 1], g_astScenePipeParam[u8Index].stDynamicLinearDrc.au16Strength[u32IsoLevel - 1],
                    pu32IsoLevelThresh[u32IsoLevel], g_astScenePipeParam[u8Index].stDynamicLinearDrc.au16Strength[u32IsoLevel]);
    }

    if(stSmartExpAttr.bEnable && (HI_TRUE == stSmartExpAttr.bIRMode))
    {
        u16Strength += SCENE_Interpulate(stSmartExpAttr.u16ExpCoef, 500, 100, 1024, 0);
		u16Strength = u16Strength > 512 ? 512 : u16Strength;
    }

    stIspDrcAttr.u8LocalMixingBrightMax = u8LocalMixingBrightMax;
    stIspDrcAttr.u8LocalMixingBrightMin = u8LocalMixingBrightMin;
    stIspDrcAttr.u8LocalMixingDarkMax = u8LocalMixingDarkMax;
    stIspDrcAttr.u8LocalMixingDarkMin = u8LocalMixingDarkMin;
    stIspDrcAttr.u8BrightGainLmt = u8BrightGainLmt;
    stIspDrcAttr.u8BrightGainLmtStep = u8BrightGainLmtStep;
    stIspDrcAttr.u8DarkGainLmtY = u8DarkGainLmtY;
    stIspDrcAttr.u8DarkGainLmtC = u8DarkGainLmtC;
    stIspDrcAttr.u8FltScaleCoarse = u8FltScaleCoarse;
    stIspDrcAttr.u8FltScaleFine = u8FltScaleFine;
    stIspDrcAttr.u8ContrastControl = u8ContrastControl;
    stIspDrcAttr.s8DetailAdjustFactor = s8DetailAdjustFactor;
    stIspDrcAttr.stAsymmetryCurve.u8Asymmetry = u8Asymmetry;
    stIspDrcAttr.stAsymmetryCurve.u8SecondPole = u8SecondPole;
    stIspDrcAttr.stAsymmetryCurve.u8Compress = u8Compress;
    stIspDrcAttr.stAsymmetryCurve.u8Stretch = u8Stretch;
    if(0 == stIspDrcAttr.enOpType)
    {
        stIspDrcAttr.stAuto.u16Strength = u16Strength;
    }
    else
    {
        stIspDrcAttr.stManual.u16Strength = u16Strength;
    }

    CHECK_SCENE_PAUSE();
    s32Ret = HI_MPI_ISP_SetDRCAttr(ViPipe, &stIspDrcAttr);
    CHECK_SCENE_RET(s32Ret);

    return HI_SUCCESS;
}
HI_S32 HI_SCENE_SetDynamicLDCI(VI_PIPE ViPipe, HI_U64 u64Exposure, HI_U64 u64LastExposure, HI_U8 u8Index)
{
    if (HI_TRUE != g_astScenePipeParam[u8Index].stModuleState.bDynamicLdci || u64LastExposure == u64Exposure)
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

HI_S32 HI_SCENE_SetDynamicDehaze(VI_PIPE ViPipe, HI_U64 u64Exposure, HI_U64 u64LastExposure, HI_U8 u8Index)
{
    if (HI_TRUE != g_astScenePipeParam[u8Index].stModuleState.bDynamicDehaze || u64LastExposure == u64Exposure)
    {
        return HI_SUCCESS;
    }

    HI_S32 s32Ret = HI_SUCCESS;
    ISP_DEHAZE_ATTR_S stDehazeAttr;
    HI_U32 u32ExpLevel = 0;

    if (u64Exposure != u64LastExposure)
    {
        CHECK_SCENE_PAUSE();
        s32Ret = HI_MPI_ISP_GetDehazeAttr(ViPipe, &stDehazeAttr);
        CHECK_SCENE_RET(s32Ret);

        u32ExpLevel = SCENE_GetLevelLtoH(u64Exposure, g_astScenePipeParam[u8Index].stDynamicDehaze.u32ExpThreshCnt, g_astScenePipeParam[u8Index].stDynamicDehaze.au64ExpThreshLtoH);
        if(0 == stDehazeAttr.enOpType)
        {
            if (u32ExpLevel == 0 || u32ExpLevel == g_astScenePipeParam[u8Index].stDynamicDehaze.u32ExpThreshCnt - 1)
            {
                stDehazeAttr.stAuto.u8strength = g_astScenePipeParam[u8Index].stDynamicDehaze.au8ManualStrength[u32ExpLevel];
            }
            else
            {
                stDehazeAttr.stAuto.u8strength = SCENE_Interpulate(u64Exposure,
                                                   g_astScenePipeParam[u8Index].stDynamicDehaze.au64ExpThreshLtoH[u32ExpLevel - 1],
                                                   g_astScenePipeParam[u8Index].stDynamicDehaze.au8ManualStrength[u32ExpLevel - 1],
                                                   g_astScenePipeParam[u8Index].stDynamicDehaze.au64ExpThreshLtoH[u32ExpLevel],
                                                   g_astScenePipeParam[u8Index].stDynamicDehaze.au8ManualStrength[u32ExpLevel]);
            }
        }
        else if(1 == stDehazeAttr.enOpType)
        {
            if (u32ExpLevel == 0 || u32ExpLevel == g_astScenePipeParam[u8Index].stDynamicDehaze.u32ExpThreshCnt - 1)
            {
                stDehazeAttr.stManual.u8strength = g_astScenePipeParam[u8Index].stDynamicDehaze.au8ManualStrength[u32ExpLevel];
            }
            else
            {
                stDehazeAttr.stManual.u8strength = SCENE_Interpulate(u64Exposure,
                                                   g_astScenePipeParam[u8Index].stDynamicDehaze.au64ExpThreshLtoH[u32ExpLevel - 1],
                                                   g_astScenePipeParam[u8Index].stDynamicDehaze.au8ManualStrength[u32ExpLevel - 1],
                                                   g_astScenePipeParam[u8Index].stDynamicDehaze.au64ExpThreshLtoH[u32ExpLevel],
                                                   g_astScenePipeParam[u8Index].stDynamicDehaze.au8ManualStrength[u32ExpLevel]);
            }
        }

        CHECK_SCENE_PAUSE();
        s32Ret = HI_MPI_ISP_SetDehazeAttr(ViPipe, &stDehazeAttr);
        CHECK_SCENE_RET(s32Ret);
    }

    return HI_SUCCESS;
}

HI_S32 HI_SCENE_SetDynamicNr(VI_PIPE ViPipe, HI_U64 u64Iso, HI_U64 u64LastIso, HI_U8 u8Index)
{
    if (HI_TRUE != g_astScenePipeParam[u8Index].stModuleState.bDynamicNr || u64LastIso == u64Iso)
    {
        return HI_SUCCESS;
    }

    HI_S32 s32Ret = HI_SUCCESS;
    HI_U32 i = 0;
    HI_U32 u32IsoLevel = 0;
    HI_U32 u32Count = g_astScenePipeParam[u8Index].stDynamicNr.u32CoringRatioCount;
    HI_U32 u32MidISO = 0, u32LeftISO = 0, u32RightISO = 0;
    HI_U32  u32LeftValue = 0, u32RightValue = 0;
    HI_U32* pu32Thresh = (HI_U32*)g_astScenePipeParam[u8Index].stDynamicNr.au32CoringRatioIso;
    ISP_NR_ATTR_S stNRAttr;

    u32IsoLevel = GetLevelHtoL(u64Iso, u32IsoLevel, u32Count, pu32Thresh);

    s32Ret = HI_MPI_ISP_GetNRAttr(ViPipe, &stNRAttr);
    CHECK_SCENE_RET(s32Ret);

    if (u32IsoLevel == (u32Count - 1))
    {
        for (i = 0; i < HI_SCENE_NR_LUT_LENGTH; i++)
        {
            stNRAttr.au16CoringRatio[i] = g_astScenePipeParam[u8Index].stDynamicNr.au16CoringRatio[u32IsoLevel][i];
        }
        stNRAttr.stWdr.au8FusionFrameStr[0] = g_astScenePipeParam[u8Index].stDynamicNr.au8FrameShortStr[u32IsoLevel];
        stNRAttr.stWdr.au8FusionFrameStr[1] = g_astScenePipeParam[u8Index].stDynamicNr.au8FrameLongStr[u32IsoLevel];
        stNRAttr.stWdr.au8WDRFrameStr[0] = g_astScenePipeParam[u8Index].stDynamicNr.au8FrameShortStr[u32IsoLevel];
        stNRAttr.stWdr.au8WDRFrameStr[1] = g_astScenePipeParam[u8Index].stDynamicNr.au8FrameLongStr[u32IsoLevel];
    }
    else
    {
        u32MidISO = (HI_U32)u64Iso;
        u32LeftISO = pu32Thresh[u32IsoLevel];
        u32RightISO = pu32Thresh[u32IsoLevel + 1];

        for (i = 0; i < HI_ISP_BAYERNR_LUT_LENGTH; i++)
        {
            u32LeftValue = g_astScenePipeParam[u8Index].stDynamicNr.au16CoringRatio[u32IsoLevel][i];
            u32RightValue = g_astScenePipeParam[u8Index].stDynamicNr.au16CoringRatio[u32IsoLevel + 1][i];
            stNRAttr.au16CoringRatio[i] = SCENE_Interpulate(u32MidISO, u32LeftISO, u32LeftValue, u32RightISO, u32RightValue);
        }
        stNRAttr.stWdr.au8FusionFrameStr[0] = g_astScenePipeParam[u8Index].stDynamicNr.au8FrameShortStr[u32IsoLevel];
        stNRAttr.stWdr.au8FusionFrameStr[1] = g_astScenePipeParam[u8Index].stDynamicNr.au8FrameLongStr[u32IsoLevel];
        stNRAttr.stWdr.au8WDRFrameStr[0] = g_astScenePipeParam[u8Index].stDynamicNr.au8FrameShortStr[u32IsoLevel];
        stNRAttr.stWdr.au8WDRFrameStr[1] = g_astScenePipeParam[u8Index].stDynamicNr.au8FrameLongStr[u32IsoLevel];
    }

    s32Ret = HI_MPI_ISP_SetNRAttr(ViPipe, &stNRAttr);
    CHECK_SCENE_RET(s32Ret);

    return HI_SUCCESS;
}

HI_S32 HI_SCENE_SetDynamicDpc(VI_PIPE ViPipe, HI_U64 u64Exposure, HI_U64 u64LastExposure, HI_U8 u8Index)
{
    if (HI_TRUE != g_astScenePipeParam[u8Index].stModuleState.bDynamicDpc || u64LastExposure == u64Exposure)
    {
        return HI_SUCCESS;
    }

    HI_S32 s32Ret = HI_SUCCESS;
    ISP_EXP_INFO_S stIspExpInfo;
    ISP_DP_DYNAMIC_ATTR_S stDPDynamicAttr;
    ISP_PUB_ATTR_S stPubAttr;

    //HI_U16  au16Strength[ISP_AUTO_ISO_STRENGTH_NUM] =   {0, 0, 0, 152, 220, 220, 150, 150, 150, 150, 152, 152, 152, 152, 152, 152};
    //HI_U16  au16BlendRatio[ISP_AUTO_ISO_STRENGTH_NUM] = {0, 0, 0,   0,   0,   0,  10,  20,  20,  25, 50, 50, 50, 50, 50, 50};



    s32Ret = HI_MPI_ISP_QueryExposureInfo(ViPipe, &stIspExpInfo);
    CHECK_SCENE_RET(s32Ret);

    s32Ret = HI_MPI_ISP_GetDPDynamicAttr(ViPipe, &stDPDynamicAttr);
    CHECK_SCENE_RET(s32Ret);

    s32Ret = HI_MPI_ISP_GetPubAttr(ViPipe, &stPubAttr);
    CHECK_SCENE_RET(s32Ret);

    stDPDynamicAttr.u8SoftSlope = 19;


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
    }
    else
    {
        if (stIspExpInfo.u32ISO >= 60000)
        {
            stDPDynamicAttr.bSupTwinkleEn = 0x1;
        }
        else
        {
            stDPDynamicAttr.bSupTwinkleEn = 0x0;
        }
    }

    s32Ret = HI_MPI_ISP_SetDPDynamicAttr(ViPipe, &stDPDynamicAttr);
    CHECK_SCENE_RET(s32Ret);

    return HI_SUCCESS;
}


HI_S32 HI_SCENE_SetDynamicShading(VI_PIPE ViPipe, HI_U64 u64Exposure, HI_U64 u64LastExposure, HI_U8 u8Index)
{
    if (HI_TRUE != g_astScenePipeParam[u8Index].stModuleState.bDynamicShading || u64LastExposure == u64Exposure)
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

HI_S32 HI_SCENE_SetDynamicCA(VI_PIPE ViPipe, HI_U64 u64Iso, HI_U64 u64LastIso, HI_U8 u8Index)
{
    if (HI_TRUE != g_astScenePipeParam[u8Index].stModuleState.bDynamicCA || u64LastIso == u64Iso)
    {
        return HI_SUCCESS;
    }
    int i = 0;
    HI_U32 u32ISOLevel = 0;
    HI_S32 s32Ret = HI_SUCCESS;

    ISP_CA_ATTR_S stCAAttr;

    if (u64Iso != u64LastIso)
    {
        CHECK_SCENE_PAUSE();
        s32Ret = HI_MPI_ISP_GetCAAttr(ViPipe, &stCAAttr);
        CHECK_SCENE_RET(s32Ret);

        u32ISOLevel = GetLevelLtoH(u64Iso,u32ISOLevel, g_astScenePipeParam[u8Index].stDynamicCA.u32CACount, g_astScenePipeParam[u8Index].stDynamicCA.au32IsoThresh);

        if (u32ISOLevel == 0 || u64Iso >= g_astScenePipeParam[u8Index].stDynamicCA.au32IsoThresh[g_astScenePipeParam[u8Index].stDynamicCA.u32CACount - 1])
        {
            for(i = 0;i<HI_ISP_CA_YRATIO_LUT_LENGTH;i++)
            {
                stCAAttr.stCA.au32YRatioLut[i] = g_astScenePipeParam[u8Index].stDynamicCA.au32CAYRatioLut[u32ISOLevel][i];
            }
        }
        else
        {
            for(i = 0;i<HI_ISP_CA_YRATIO_LUT_LENGTH;i++)
            {
                stCAAttr.stCA.au32YRatioLut[i] = SCENE_Interpulate(u64Iso,
                            g_astScenePipeParam[u8Index].stDynamicCA.au32IsoThresh[u32ISOLevel-1],g_astScenePipeParam[u8Index].stDynamicCA.au32CAYRatioLut[u32ISOLevel-1][i],
                            g_astScenePipeParam[u8Index].stDynamicCA.au32IsoThresh[u32ISOLevel],g_astScenePipeParam[u8Index].stDynamicCA.au32CAYRatioLut[u32ISOLevel][i]);
            }

        }

        CHECK_SCENE_PAUSE();
        s32Ret = HI_MPI_ISP_SetCAAttr(ViPipe, &stCAAttr);
        CHECK_SCENE_RET(s32Ret);
    }

    return HI_SUCCESS;
}


HI_S32 HI_SCENE_SetDynamicBLC(VI_PIPE ViPipe, HI_U64 u64Iso, HI_U64 u64LastIso, HI_U8 u8Index)
{
    if (HI_TRUE != g_astScenePipeParam[u8Index].stModuleState.bDynamicBLC || u64LastIso == u64Iso)
    {
        return HI_SUCCESS;
    }
    int i = 0;
    HI_U32 u32ISOLevel = 0;
    HI_S32 s32Ret = HI_SUCCESS;

    ISP_BLACK_LEVEL_S stBLCAttr;

    if (u64Iso != u64LastIso)
    {
        CHECK_SCENE_PAUSE();
        s32Ret = HI_MPI_ISP_GetBlackLevelAttr(ViPipe, &stBLCAttr);
        CHECK_SCENE_RET(s32Ret);

        u32ISOLevel = GetLevelLtoH(u64Iso,u32ISOLevel, g_astScenePipeParam[u8Index].stDynamicBLC.u32BLCCount, g_astScenePipeParam[u8Index].stDynamicBLC.au32IsoThresh);
        //printf("2086=%d\n",u32ISOLevel);
        if (u32ISOLevel == 0 || u64Iso >= g_astScenePipeParam[u8Index].stDynamicBLC.au32IsoThresh[g_astScenePipeParam[u8Index].stDynamicBLC.u32BLCCount - 1])
        {
            for(i = 0;i<4;i++)
            {
                stBLCAttr.au16BlackLevel[i] = g_astScenePipeParam[u8Index].stDynamicBLC.au32BLCValue[u32ISOLevel][i];
            }
        }
        else
        {
            for(i = 0;i<4;i++)
            {
                stBLCAttr.au16BlackLevel[i] = SCENE_Interpulate(u64Iso,
                            g_astScenePipeParam[u8Index].stDynamicBLC.au32IsoThresh[u32ISOLevel-1],g_astScenePipeParam[u8Index].stDynamicBLC.au32BLCValue[u32ISOLevel-1][i],
                            g_astScenePipeParam[u8Index].stDynamicBLC.au32IsoThresh[u32ISOLevel],g_astScenePipeParam[u8Index].stDynamicBLC.au32BLCValue[u32ISOLevel][i]);
            }

        }

        CHECK_SCENE_PAUSE();
        stBLCAttr.enOpType = OP_TYPE_MANUAL;
        s32Ret = HI_MPI_ISP_SetBlackLevelAttr(ViPipe, &stBLCAttr);
        CHECK_SCENE_RET(s32Ret);
    }

    return HI_SUCCESS;
}



HI_S32 HI_SCENE_SetDynamicVencBitrate(VI_PIPE ViPipe, HI_U64 u64Iso, HI_U64 u64LastIso, HI_U8 u8Index)
{
     if(HI_TRUE != g_astScenePipeParam[u8Index].stModuleState.bDynamicIsoVenc)
     {
     	return HI_SUCCESS;
     }

    HI_U32 u32ExpLevel = 0;
    HI_S32 s32Ret = HI_SUCCESS;
    HI_U32 u32Iso = u64Iso;
    HI_U32 u32LastIso = u64LastIso;

    VENC_CHN_ATTR_S stVencChnAttr;
    VENC_RC_PARAM_S stRcParam;

    PAYLOAD_TYPE_E  enType;

    if (u32Iso != u32LastIso)
    {
        CHECK_SCENE_PAUSE();
        s32Ret = HI_MPI_VENC_GetChnAttr(ViPipe, &stVencChnAttr);
	CHECK_SCENE_RET(s32Ret);
	enType  = stVencChnAttr.stVencAttr.enType;

        s32Ret = HI_MPI_VENC_GetRcParam(ViPipe, &stRcParam);
        CHECK_SCENE_RET(s32Ret);

        u32ExpLevel = SCENE_GetLevelLtoH_U32(u32Iso, g_astScenePipeParam[u8Index].stDynamicVencBitrate.u32IsoThreshCnt, g_astScenePipeParam[u8Index].stDynamicVencBitrate.au32IsoThreshLtoH);
        //printf("u32ExpThreshCnt = %d\n",g_astScenePipeParam[u8Index].stDynamicVencBitrate.u32ExpThreshCnt);
        if (u32ExpLevel == 0 || u32ExpLevel == g_astScenePipeParam[u8Index].stDynamicVencBitrate.u32IsoThreshCnt - 1)
        {
            if(enType == PT_H264 )
            {
                stRcParam.stParamH264AVbr.s32ChangePos = g_astScenePipeParam[u8Index].stDynamicVencBitrate.au16ManualPercent[u32ExpLevel];
            }
	    else
            {
                stRcParam.stParamH265AVbr.s32ChangePos = g_astScenePipeParam[u8Index].stDynamicVencBitrate.au16ManualPercent[u32ExpLevel];
            }

        }
        else
        {
            if(enType == PT_H264 )
            {
                 stRcParam.stParamH264AVbr.s32ChangePos = SCENE_Interpulate(u32Iso,
                            g_astScenePipeParam[u8Index].stDynamicVencBitrate.au32IsoThreshLtoH[u32ExpLevel - 1],
                            g_astScenePipeParam[u8Index].stDynamicVencBitrate.au16ManualPercent[u32ExpLevel - 1],
                            g_astScenePipeParam[u8Index].stDynamicVencBitrate.au32IsoThreshLtoH[u32ExpLevel],
                            g_astScenePipeParam[u8Index].stDynamicVencBitrate.au16ManualPercent[u32ExpLevel]);
            }
	    else
            {
                 stRcParam.stParamH265AVbr.s32ChangePos = SCENE_Interpulate(u32Iso,
                            g_astScenePipeParam[u8Index].stDynamicVencBitrate.au32IsoThreshLtoH[u32ExpLevel - 1],
                            g_astScenePipeParam[u8Index].stDynamicVencBitrate.au16ManualPercent[u32ExpLevel - 1],
                            g_astScenePipeParam[u8Index].stDynamicVencBitrate.au32IsoThreshLtoH[u32ExpLevel],
                            g_astScenePipeParam[u8Index].stDynamicVencBitrate.au16ManualPercent[u32ExpLevel]);
            }

        }

        CHECK_SCENE_PAUSE();

        s32Ret =  HI_MPI_VENC_SetRcParam(ViPipe, &stRcParam);
        CHECK_SCENE_RET(s32Ret);
    }

    return HI_SUCCESS;
}

HI_S32 HI_SCENE_SetDynamicVideoGamma(VI_PIPE ViPipe, HI_U64 u64Exposure, HI_U64 u64LastExposure, HI_U8 u8Index)
{
    if (HI_TRUE != g_astScenePipeParam[u8Index].stModuleState.bDynamicGamma)
    {
        return HI_SUCCESS;
    }

    HI_U32 i, j = 0;
    HI_U32 u32ExpLevel = 0;
    static HI_U32 u32LastExpLevel = 0;
    static HI_BOOL bFirst = HI_TRUE;
    HI_S32 s32Ret = HI_SUCCESS;

    ISP_GAMMA_ATTR_S stIspGammaAttr;

    if (u64Exposure != u64LastExposure || HI_TRUE == bFirst)
    {
        u32ExpLevel = SCENE_GetLevelLtoH(u64Exposure, g_astScenePipeParam[u8Index].stDynamicGamma.u32TotalNum, g_astScenePipeParam[u8Index].stDynamicGamma.au64ExpThreshHtoL);
        //printf("u32ExpLevel = %d\n", u32ExpLevel);
        if (u32ExpLevel != u32LastExpLevel || HI_TRUE == bFirst)
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
            bFirst = HI_FALSE;
        }
    }

    return HI_SUCCESS;
}

HI_S32 HI_SCENE_SetDynamicPhotoGamma(VI_PIPE ViPipe, HI_U64 u64Exposure, HI_U64 u64LastExposure, HI_U8 u8Index)
{
    if (HI_TRUE != g_astScenePipeParam[u8Index].stModuleState.bDynamicGamma)
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
    if (HI_TRUE != g_astScenePipeParam[u8Index].stModuleState.bDynamicFalseColor || u64LastExposure == u64Exposure)
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

HI_S32 HI_SCENE_Set3DNR(VI_PIPE ViPipe, const HI_SCENE_3DNR_S* pst3Nrx, HI_U8 u8Index)
{
    HI_S32 s32Ret = HI_SUCCESS;
    HI_U32 i = 0, j = 0;

    VPSS_GRP_NRX_PARAM_S stNrx, stViNrXGet;
    VPSS_GRP_ATTR_S stGrpAttr;

    stNrx.enNRVer = VPSS_NR_V3;
    stNrx.stNRXParam_V3.enOptMode = OPERATION_MODE_MANUAL;
    s32Ret = HI_MPI_VPSS_GetGrpNRXParam(ViPipe, &stNrx);
    CHECK_SCENE_RET(s32Ret);

    //IEy
    for (i = 0; i < 5; i++)
    {
        stNrx.stNRXParam_V3.stNRXManual.stNRXParam.IEy[i].IES0 = pst3Nrx->IEy[i].IES0;
        stNrx.stNRXParam_V3.stNRXManual.stNRXParam.IEy[i].IES1 = pst3Nrx->IEy[i].IES1;
        stNrx.stNRXParam_V3.stNRXManual.stNRXParam.IEy[i].IES2 = pst3Nrx->IEy[i].IES2;
        stNrx.stNRXParam_V3.stNRXManual.stNRXParam.IEy[i].IES3 = pst3Nrx->IEy[i].IES3;
        stNrx.stNRXParam_V3.stNRXManual.stNRXParam.IEy[i].IEDZ = pst3Nrx->IEy[i].IEDZ;
    }


    //SFy
    for (i = 0; i < 5; i++) // 5 levels
    {
        stNrx.stNRXParam_V3.stNRXManual.stNRXParam.SFy[i].SPN6  = pst3Nrx->SFy[i].SPN6;
        stNrx.stNRXParam_V3.stNRXManual.stNRXParam.SFy[i].SBN6  = pst3Nrx->SFy[i].SBN6;
        stNrx.stNRXParam_V3.stNRXManual.stNRXParam.SFy[i].PBR6  = pst3Nrx->SFy[i].PBR6;
        stNrx.stNRXParam_V3.stNRXManual.stNRXParam.SFy[i].JMODE = pst3Nrx->SFy[i].JMODE;//sf6

        for (j = 0; j < 4; j++)//SFR6[4]
        {
            stNrx.stNRXParam_V3.stNRXManual.stNRXParam.SFy[i].SFR6[j] = pst3Nrx->SFy[i].SFR6[j];//sfr6
        }

        stNrx.stNRXParam_V3.stNRXManual.stNRXParam.SFy[i].SBR6[0] = pst3Nrx->SFy[i].SBR6[0];
        stNrx.stNRXParam_V3.stNRXManual.stNRXParam.SFy[i].SBR6[1] = pst3Nrx->SFy[i].SBR6[1];//sbr6

        stNrx.stNRXParam_V3.stNRXManual.stNRXParam.SFy[i].SFN0 = pst3Nrx->SFy[i].SFN0;
        stNrx.stNRXParam_V3.stNRXManual.stNRXParam.SFy[i].SFN1 = pst3Nrx->SFy[i].SFN1;
        stNrx.stNRXParam_V3.stNRXManual.stNRXParam.SFy[i].SFN2 = pst3Nrx->SFy[i].SFN2;//sfn

        stNrx.stNRXParam_V3.stNRXManual.stNRXParam.SFy[i].STH1 = pst3Nrx->SFy[i].STH1;
        stNrx.stNRXParam_V3.stNRXManual.stNRXParam.SFy[i].STH2 = pst3Nrx->SFy[i].STH2;//sth

        stNrx.stNRXParam_V3.stNRXManual.stNRXParam.SFy[i].STHd1 = pst3Nrx->SFy[i].STHd1;
        stNrx.stNRXParam_V3.stNRXManual.stNRXParam.SFy[i].STHd2 = pst3Nrx->SFy[i].STHd2;//sthd

        stNrx.stNRXParam_V3.stNRXManual.stNRXParam.SFy[i].SFR   = pst3Nrx->SFy[i].SFR;//sfr
    }

    for (i = 0; i < 4; i++) // 4 levels
    {
        stNrx.stNRXParam_V3.stNRXManual.stNRXParam.SFy[i].SFS1 = pst3Nrx->SFy[i].SFS1;
        stNrx.stNRXParam_V3.stNRXManual.stNRXParam.SFy[i].SFT1 = pst3Nrx->SFy[i].SFT1;
        stNrx.stNRXParam_V3.stNRXManual.stNRXParam.SFy[i].SBR1 = pst3Nrx->SFy[i].SBR1;

        stNrx.stNRXParam_V3.stNRXManual.stNRXParam.SFy[i].SFS2 = pst3Nrx->SFy[i].SFS2;
        stNrx.stNRXParam_V3.stNRXManual.stNRXParam.SFy[i].SFT2 = pst3Nrx->SFy[i].SFT2;
        stNrx.stNRXParam_V3.stNRXManual.stNRXParam.SFy[i].SBR2 = pst3Nrx->SFy[i].SBR2;

        stNrx.stNRXParam_V3.stNRXManual.stNRXParam.SFy[i].SFS4 = pst3Nrx->SFy[i].SFS4;
        stNrx.stNRXParam_V3.stNRXManual.stNRXParam.SFy[i].SFT4 = pst3Nrx->SFy[i].SFT4;
        stNrx.stNRXParam_V3.stNRXManual.stNRXParam.SFy[i].SBR4 = pst3Nrx->SFy[i].SBR4;//sf1 sf2 sf4
    }

    stNrx.stNRXParam_V3.stNRXManual.stNRXParam.SFy[0].SRT0  = pst3Nrx->SFy[0].SRT0;
    stNrx.stNRXParam_V3.stNRXManual.stNRXParam.SFy[0].SRT1  = pst3Nrx->SFy[0].SRT1;//SelRt
    stNrx.stNRXParam_V3.stNRXManual.stNRXParam.SFy[2].kMode = pst3Nrx->SFy[2].kMode;
    stNrx.stNRXParam_V3.stNRXManual.stNRXParam.SFy[3].kMode = pst3Nrx->SFy[3].kMode;//kMode

    stNrx.stNRXParam_V3.stNRXManual.stNRXParam.SFy[0].DeIdx = pst3Nrx->SFy[0].DeIdx;
    stNrx.stNRXParam_V3.stNRXManual.stNRXParam.SFy[0].DeRate = pst3Nrx->SFy[0].DeRate;//DeRt

    stNrx.stNRXParam_V3.stNRXManual.stNRXParam.SFy[4].SFS1 = pst3Nrx->SFy[4].SFS1;
    stNrx.stNRXParam_V3.stNRXManual.stNRXParam.SFy[4].SFS2 = pst3Nrx->SFy[4].SFS2;
    stNrx.stNRXParam_V3.stNRXManual.stNRXParam.SFy[4].SFS4 = pst3Nrx->SFy[4].SFS4;//sfs5


    //TFy
    for (i = 0; i < 3; i++)
    {
        if (2 != i)
        {
            stNrx.stNRXParam_V3.stNRXManual.stNRXParam.TFy[i].bRef = pst3Nrx->TFy[i].bRef;//ref
        }
        if (0 != i)
        {
            stNrx.stNRXParam_V3.stNRXManual.stNRXParam.TFy[i].TED   = pst3Nrx->TFy[i].TED;//tedge
        }
        stNrx.stNRXParam_V3.stNRXManual.stNRXParam.TFy[i].STR0 = pst3Nrx->TFy[i].STR0;
        stNrx.stNRXParam_V3.stNRXManual.stNRXParam.TFy[1].STR1 = pst3Nrx->TFy[1].STR1;//str

        stNrx.stNRXParam_V3.stNRXManual.stNRXParam.TFy[i].SDZ0 = pst3Nrx->TFy[i].SDZ0;
        stNrx.stNRXParam_V3.stNRXManual.stNRXParam.TFy[1].SDZ1 = pst3Nrx->TFy[i].SDZ1;//sdz

        stNrx.stNRXParam_V3.stNRXManual.stNRXParam.TFy[i].TSS0 = pst3Nrx->TFy[i].TSS0;
        stNrx.stNRXParam_V3.stNRXManual.stNRXParam.TFy[1].TSS1 = pst3Nrx->TFy[1].TSS1;//tss

        stNrx.stNRXParam_V3.stNRXManual.stNRXParam.TFy[i].TSI0 = pst3Nrx->TFy[i].TSI0;
        stNrx.stNRXParam_V3.stNRXManual.stNRXParam.TFy[1].TSI1 = pst3Nrx->TFy[1].TSI1;//tsi

        stNrx.stNRXParam_V3.stNRXManual.stNRXParam.TFy[i].TFS0 = pst3Nrx->TFy[i].TFS0;
        stNrx.stNRXParam_V3.stNRXManual.stNRXParam.TFy[1].TFS1 = pst3Nrx->TFy[1].TFS1;//tfs

        stNrx.stNRXParam_V3.stNRXManual.stNRXParam.TFy[i].TDZ0 = pst3Nrx->TFy[i].TDZ0;
        stNrx.stNRXParam_V3.stNRXManual.stNRXParam.TFy[1].TDZ1 = pst3Nrx->TFy[1].TDZ1;//tdz

        stNrx.stNRXParam_V3.stNRXManual.stNRXParam.TFy[i].TDX0 = pst3Nrx->TFy[i].TDX0;
        stNrx.stNRXParam_V3.stNRXManual.stNRXParam.TFy[1].TDX1 = pst3Nrx->TFy[1].TDX1;//tdx

        for (j = 0; j < 6; j++)
        {
            stNrx.stNRXParam_V3.stNRXManual.stNRXParam.TFy[i].TFR0[j] = pst3Nrx->TFy[i].TFR0[j];//tfr0
        }

        for (j = 0; j < 6; j++)
        {
            stNrx.stNRXParam_V3.stNRXManual.stNRXParam.TFy[1].TFR1[j] = pst3Nrx->TFy[1].TFR1[j];//tfr1
        }

    }

    stNrx.stNRXParam_V3.stNRXManual.stNRXParam.TFy[0].TFRS = pst3Nrx->TFy[0].TFRS;//tfrs

    //MDy
    for (i = 0; i < 2; i++)
    {
        /* PATH0 */
        stNrx.stNRXParam_V3.stNRXManual.stNRXParam.MDy[i].MAI00 = pst3Nrx->MDy[i].MAI00;
        stNrx.stNRXParam_V3.stNRXManual.stNRXParam.MDy[i].MAI01 = pst3Nrx->MDy[i].MAI01;
        stNrx.stNRXParam_V3.stNRXManual.stNRXParam.MDy[i].MAI02 = pst3Nrx->MDy[i].MAI02;//id0

        stNrx.stNRXParam_V3.stNRXManual.stNRXParam.MDy[i].MAI10 = pst3Nrx->MDy[i].MAI10;
        stNrx.stNRXParam_V3.stNRXManual.stNRXParam.MDy[i].MAI11 = pst3Nrx->MDy[i].MAI11;
        stNrx.stNRXParam_V3.stNRXManual.stNRXParam.MDy[i].MAI12 = pst3Nrx->MDy[i].MAI12;//id1

        stNrx.stNRXParam_V3.stNRXManual.stNRXParam.MDy[i].MABR0  = pst3Nrx->MDy[i].MABR0;
        stNrx.stNRXParam_V3.stNRXManual.stNRXParam.MDy[0].MABR1 = pst3Nrx->MDy[0].MABR1;//mabr

        stNrx.stNRXParam_V3.stNRXManual.stNRXParam.MDy[0].AdvMATH = pst3Nrx->MDy[0].AdvMATH;//AdvMath
        stNrx.stNRXParam_V3.stNRXManual.stNRXParam.MDy[0].AdvTH = pst3Nrx->MDy[0].AdvTH;//AdvTh

        stNrx.stNRXParam_V3.stNRXManual.stNRXParam.MDy[i].MATH0 = pst3Nrx->MDy[i].MATH0;
        stNrx.stNRXParam_V3.stNRXManual.stNRXParam.MDy[i].MATH1 = pst3Nrx->MDy[i].MATH1;//math

        stNrx.stNRXParam_V3.stNRXManual.stNRXParam.MDy[i].MATHd0 = pst3Nrx->MDy[i].MATHd0;
        stNrx.stNRXParam_V3.stNRXManual.stNRXParam.MDy[i].MATHd1 = pst3Nrx->MDy[i].MATHd1;//mathd

        stNrx.stNRXParam_V3.stNRXManual.stNRXParam.MDy[i].MATE0 = pst3Nrx->MDy[i].MATE0;
        stNrx.stNRXParam_V3.stNRXManual.stNRXParam.MDy[i].MATE1 = pst3Nrx->MDy[i].MATE1;//mate


        stNrx.stNRXParam_V3.stNRXManual.stNRXParam.MDy[i].MABW0 = pst3Nrx->MDy[i].MABW0;
        stNrx.stNRXParam_V3.stNRXManual.stNRXParam.MDy[i].MABW1  = pst3Nrx->MDy[i].MABW1;//mabw

        stNrx.stNRXParam_V3.stNRXManual.stNRXParam.MDy[i].MATW = pst3Nrx->MDy[i].MATW;//matw


    }

    //NRc
    {
        stNrx.stNRXParam_V3.stNRXManual.stNRXParam.NRc.SFC = pst3Nrx->NRc.SFC;
        stNrx.stNRXParam_V3.stNRXManual.stNRXParam.NRc.TFC = pst3Nrx->NRc.TFC;
        stNrx.stNRXParam_V3.stNRXManual.stNRXParam.NRc.TPC = pst3Nrx->NRc.TPC;
        stNrx.stNRXParam_V3.stNRXManual.stNRXParam.NRc.TRC = pst3Nrx->NRc.TRC;
    }
//
//    {
//        printf("1============================================================\n");
//        PrintNRxVPSS(&stNrx.stNRXParam_V3.stNRXManual.stNRXParam);
//    }

    stNrx.enNRVer = VPSS_NR_V3;
    stNrx.stNRXParam_V3.enOptMode = OPERATION_MODE_MANUAL;
    s32Ret = HI_MPI_VPSS_SetGrpNRXParam(ViPipe, &stNrx);
    CHECK_SCENE_RET(s32Ret);

    stViNrXGet.enNRVer = VPSS_NR_V3;
    stViNrXGet.stNRXParam_V3.enOptMode = OPERATION_MODE_MANUAL;
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

    if(1 == g_astScenePipeParam[u8Index].stModuleState.bDebug)
    {
        PrintNRxVPSS(&stViNrXGet.stNRXParam_V3.stNRXManual.stNRXParam);
    }

    return HI_SUCCESS;
}

HI_S32 HI_SCENE_SetDynamic3DNR(VI_PIPE ViPipe, HI_U32 u32Iso, HI_U8 u8Index)
{
    if (HI_TRUE != g_astScenePipeParam[u8Index].stModuleState.bDynamic3DNR)
    {
        return HI_SUCCESS;
    }
    HI_S32 s32Ret = HI_SUCCESS;
    HI_U32 u32Mid = 0, u32Left = 0, u32Right = 0;
    HI_U32 u32IsoLevel = 0;
    HI_U32 i = 0, j = 0;
    HI_SCENE_3DNR_S stNrx;
    HI_SCENE_STATIC_3DNR_S st3dnrparam = g_astScenePipeParam[u8Index].stStaticThreeDNR;
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
        for (i = 0; i < 5; i++)
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
            //sf1
            stNrx.SFy[i].SFS1 = SCENE_Interpulate(u32Mid, u32Left, st3dnrparam.astThreeDNRValue[u32IsoLevel - 1].SFy[i].SFS1, u32Right, st3dnrparam.astThreeDNRValue[u32IsoLevel].SFy[i].SFS1);
            stNrx.SFy[i].SFT1 = SCENE_Interpulate(u32Mid, u32Left, st3dnrparam.astThreeDNRValue[u32IsoLevel - 1].SFy[i].SFT1, u32Right, st3dnrparam.astThreeDNRValue[u32IsoLevel].SFy[i].SFT1);
            stNrx.SFy[i].SBR1 = SCENE_Interpulate(u32Mid, u32Left, st3dnrparam.astThreeDNRValue[u32IsoLevel - 1].SFy[i].SBR1, u32Right, st3dnrparam.astThreeDNRValue[u32IsoLevel].SFy[i].SBR1);
            //sf2
            stNrx.SFy[i].SFS2 = SCENE_Interpulate(u32Mid, u32Left, st3dnrparam.astThreeDNRValue[u32IsoLevel - 1].SFy[i].SFS2, u32Right, st3dnrparam.astThreeDNRValue[u32IsoLevel].SFy[i].SFS2);
            stNrx.SFy[i].SFT2 = SCENE_Interpulate(u32Mid, u32Left, st3dnrparam.astThreeDNRValue[u32IsoLevel - 1].SFy[i].SFT2, u32Right, st3dnrparam.astThreeDNRValue[u32IsoLevel].SFy[i].SFT2);
            stNrx.SFy[i].SBR2 = SCENE_Interpulate(u32Mid, u32Left, st3dnrparam.astThreeDNRValue[u32IsoLevel - 1].SFy[i].SBR2, u32Right, st3dnrparam.astThreeDNRValue[u32IsoLevel].SFy[i].SBR2);
            //sf4
            stNrx.SFy[i].SFS4 = SCENE_Interpulate(u32Mid, u32Left, st3dnrparam.astThreeDNRValue[u32IsoLevel - 1].SFy[i].SFS4, u32Right, st3dnrparam.astThreeDNRValue[u32IsoLevel].SFy[i].SFS4);
            stNrx.SFy[i].SFT4 = SCENE_Interpulate(u32Mid, u32Left, st3dnrparam.astThreeDNRValue[u32IsoLevel - 1].SFy[i].SFT4, u32Right, st3dnrparam.astThreeDNRValue[u32IsoLevel].SFy[i].SFT4);
            stNrx.SFy[i].SBR4 = SCENE_Interpulate(u32Mid, u32Left, st3dnrparam.astThreeDNRValue[u32IsoLevel - 1].SFy[i].SBR4, u32Right, st3dnrparam.astThreeDNRValue[u32IsoLevel].SFy[i].SBR4);
        }
        //sfs5
        stNrx.SFy[4].SFS1 = SCENE_Interpulate(u32Mid, u32Left, st3dnrparam.astThreeDNRValue[u32IsoLevel - 1].SFy[4].SFS1, u32Right, st3dnrparam.astThreeDNRValue[u32IsoLevel].SFy[4].SFS1);
        stNrx.SFy[4].SFS2 = SCENE_Interpulate(u32Mid, u32Left, st3dnrparam.astThreeDNRValue[u32IsoLevel - 1].SFy[4].SFS2, u32Right, st3dnrparam.astThreeDNRValue[u32IsoLevel].SFy[4].SFS2);
        stNrx.SFy[4].SFS4 = SCENE_Interpulate(u32Mid, u32Left, st3dnrparam.astThreeDNRValue[u32IsoLevel - 1].SFy[4].SFS4, u32Right, st3dnrparam.astThreeDNRValue[u32IsoLevel].SFy[4].SFS4);

        for (i = 0; i < 5; i++)
        {

            //sf6
            stNrx.SFy[i].SPN6 = st3dnrparam.astThreeDNRValue[u32IsoLevel - 1].SFy[i].SPN6;
            stNrx.SFy[i].SBN6 = st3dnrparam.astThreeDNRValue[u32IsoLevel - 1].SFy[i].SBN6;
            stNrx.SFy[i].PBR6 = st3dnrparam.astThreeDNRValue[u32IsoLevel - 1].SFy[i].PBR6;
            stNrx.SFy[i].JMODE = st3dnrparam.astThreeDNRValue[u32IsoLevel - 1].SFy[i].JMODE;//sf6
            //sfr6
            for (j = 0; j < 4; j++)
            {
                stNrx.SFy[i].SFR6[j] = SCENE_Interpulate(u32Mid, u32Left, st3dnrparam.astThreeDNRValue[u32IsoLevel - 1].SFy[i].SFR6[j],
                                        u32Right, st3dnrparam.astThreeDNRValue[u32IsoLevel].SFy[i].SFR6[j]);
            }
            //sbr6
            stNrx.SFy[i].SBR6[0] = SCENE_Interpulate(u32Mid, u32Left, st3dnrparam.astThreeDNRValue[u32IsoLevel - 1].SFy[i].SBR6[0],
                                    u32Right, st3dnrparam.astThreeDNRValue[u32IsoLevel].SFy[i].SBR6[0]);
            stNrx.SFy[i].SBR6[1] = SCENE_Interpulate(u32Mid, u32Left, st3dnrparam.astThreeDNRValue[u32IsoLevel - 1].SFy[i].SBR6[1],
                                    u32Right, st3dnrparam.astThreeDNRValue[u32IsoLevel].SFy[i].SBR6[1]);
            //sfn
            stNrx.SFy[i].SFN0 = st3dnrparam.astThreeDNRValue[u32IsoLevel - 1].SFy[i].SFN0;
            stNrx.SFy[i].SFN1 = st3dnrparam.astThreeDNRValue[u32IsoLevel - 1].SFy[i].SFN1;
            stNrx.SFy[i].SFN2 = st3dnrparam.astThreeDNRValue[u32IsoLevel - 1].SFy[i].SFN2;
            //sth
            stNrx.SFy[i].STH1 = SCENE_Interpulate(u32Mid, u32Left, st3dnrparam.astThreeDNRValue[u32IsoLevel - 1].SFy[i].STH1,
                            u32Right, st3dnrparam.astThreeDNRValue[u32IsoLevel].SFy[i].STH1);
            stNrx.SFy[i].STH2 = SCENE_Interpulate(u32Mid, u32Left, st3dnrparam.astThreeDNRValue[u32IsoLevel - 1].SFy[i].STH2,
                            u32Right, st3dnrparam.astThreeDNRValue[u32IsoLevel].SFy[i].STH2);

            //sthd
            stNrx.SFy[i].STHd1 = SCENE_Interpulate(u32Mid, u32Left, st3dnrparam.astThreeDNRValue[u32IsoLevel - 1].SFy[i].STHd1,
                            u32Right, st3dnrparam.astThreeDNRValue[u32IsoLevel].SFy[i].STHd1);
            stNrx.SFy[i].STHd2 = SCENE_Interpulate(u32Mid, u32Left, st3dnrparam.astThreeDNRValue[u32IsoLevel - 1].SFy[i].STHd2,
                            u32Right, st3dnrparam.astThreeDNRValue[u32IsoLevel].SFy[i].STHd2);
            //sfr
            stNrx.SFy[i].SFR  = SCENE_Interpulate(u32Mid, u32Left, st3dnrparam.astThreeDNRValue[u32IsoLevel - 1].SFy[i].SFR,
                            u32Right, st3dnrparam.astThreeDNRValue[u32IsoLevel].SFy[i].SFR);


        }
        {
            //SelRt
            stNrx.SFy[0].SRT0 = st3dnrparam.astThreeDNRValue[u32IsoLevel - 1].SFy[0].SRT0;
            stNrx.SFy[0].SRT1 = st3dnrparam.astThreeDNRValue[u32IsoLevel - 1].SFy[0].SRT1;
            //DeRt
            stNrx.SFy[0].DeRate = st3dnrparam.astThreeDNRValue[u32IsoLevel - 1].SFy[0].DeRate;
            stNrx.SFy[0].DeIdx = st3dnrparam.astThreeDNRValue[u32IsoLevel - 1].SFy[0].DeIdx;
            //kMode
            stNrx.SFy[2].kMode = st3dnrparam.astThreeDNRValue[u32IsoLevel - 1].SFy[2].kMode;
            stNrx.SFy[3].kMode = st3dnrparam.astThreeDNRValue[u32IsoLevel - 1].SFy[3].kMode;
        }
        /*TFy*/
        for (i = 0; i < 3; i++)
        {
            //ref
            if (2 != i)
            {
                stNrx.TFy[i].bRef = st3dnrparam.astThreeDNRValue[u32IsoLevel - 1].TFy[i].bRef;
            }

            //str
            stNrx.TFy[i].STR0 = SCENE_Interpulate(u32Mid, u32Left, st3dnrparam.astThreeDNRValue[u32IsoLevel - 1].TFy[i].STR0,
                                                  u32Right, st3dnrparam.astThreeDNRValue[u32IsoLevel].TFy[i].STR0);
            stNrx.TFy[1].STR1 = SCENE_Interpulate(u32Mid, u32Left, st3dnrparam.astThreeDNRValue[u32IsoLevel - 1].TFy[1].STR1,
                                                  u32Right, st3dnrparam.astThreeDNRValue[u32IsoLevel].TFy[1].STR1);
            //sdz
            stNrx.TFy[i].SDZ0 = SCENE_Interpulate(u32Mid, u32Left, st3dnrparam.astThreeDNRValue[u32IsoLevel - 1].TFy[i].SDZ0,
                                                u32Right, st3dnrparam.astThreeDNRValue[u32IsoLevel].TFy[i].SDZ0);
            stNrx.TFy[1].SDZ1 = SCENE_Interpulate(u32Mid, u32Left, st3dnrparam.astThreeDNRValue[u32IsoLevel - 1].TFy[1].SDZ1,
                                                u32Right, st3dnrparam.astThreeDNRValue[u32IsoLevel].TFy[1].SDZ1);
            //tss
            stNrx.TFy[i].TSS0 = SCENE_Interpulate(u32Mid, u32Left, st3dnrparam.astThreeDNRValue[u32IsoLevel - 1].TFy[i].TSS0,
                                                u32Right, st3dnrparam.astThreeDNRValue[u32IsoLevel].TFy[i].TSS0);
            stNrx.TFy[1].TSS1 = SCENE_Interpulate(u32Mid, u32Left, st3dnrparam.astThreeDNRValue[u32IsoLevel - 1].TFy[1].TSS1,
                                                u32Right, st3dnrparam.astThreeDNRValue[u32IsoLevel].TFy[1].TSS1);
            //tsi
            stNrx.TFy[i].TSI0 = SCENE_Interpulate(u32Mid, u32Left, st3dnrparam.astThreeDNRValue[u32IsoLevel - 1].TFy[i].TSI0,
                                                u32Right, st3dnrparam.astThreeDNRValue[u32IsoLevel].TFy[i].TSI0);
            stNrx.TFy[1].TSI1 = SCENE_Interpulate(u32Mid, u32Left, st3dnrparam.astThreeDNRValue[u32IsoLevel - 1].TFy[1].TSI1,
                                                u32Right, st3dnrparam.astThreeDNRValue[u32IsoLevel].TFy[1].TSI1);
            //tfs
            stNrx.TFy[i].TFS0 = SCENE_Interpulate(u32Mid, u32Left, st3dnrparam.astThreeDNRValue[u32IsoLevel - 1].TFy[i].TFS0,
                                                u32Right, st3dnrparam.astThreeDNRValue[u32IsoLevel].TFy[i].TFS0);
            stNrx.TFy[1].TFS1 = SCENE_Interpulate(u32Mid, u32Left, st3dnrparam.astThreeDNRValue[u32IsoLevel - 1].TFy[1].TFS1,
                                                u32Right, st3dnrparam.astThreeDNRValue[u32IsoLevel].TFy[1].TFS1);
            //tdz
            stNrx.TFy[i].TDZ0 = SCENE_Interpulate(u32Mid, u32Left, st3dnrparam.astThreeDNRValue[u32IsoLevel - 1].TFy[i].TDZ0,
                                                u32Right, st3dnrparam.astThreeDNRValue[u32IsoLevel].TFy[i].TDZ0);
            stNrx.TFy[1].TDZ1 = SCENE_Interpulate(u32Mid, u32Left, st3dnrparam.astThreeDNRValue[u32IsoLevel - 1].TFy[1].TDZ1,
                                                u32Right, st3dnrparam.astThreeDNRValue[u32IsoLevel].TFy[1].TDZ1);
             //tdx
            stNrx.TFy[i].TDX0 = SCENE_Interpulate(u32Mid, u32Left, st3dnrparam.astThreeDNRValue[u32IsoLevel - 1].TFy[i].TDX0,
                                                u32Right, st3dnrparam.astThreeDNRValue[u32IsoLevel].TFy[i].TDX0);
            stNrx.TFy[1].TDX1 = SCENE_Interpulate(u32Mid, u32Left, st3dnrparam.astThreeDNRValue[u32IsoLevel - 1].TFy[1].TDX1,
                                                u32Right, st3dnrparam.astThreeDNRValue[u32IsoLevel].TFy[1].TDX1);
            //tfr0
            for (j = 0; j < 6; j++)
            {
                stNrx.TFy[i].TFR0[j] = SCENE_Interpulate(u32Mid, u32Left, st3dnrparam.astThreeDNRValue[u32IsoLevel - 1].TFy[i].TFR0[j],
                                u32Right, st3dnrparam.astThreeDNRValue[u32IsoLevel].TFy[i].TFR0[j]);
            }
            //tfr1
            for (j = 0; j < 6; j++)
            {
                stNrx.TFy[1].TFR1[j] = SCENE_Interpulate(u32Mid, u32Left, st3dnrparam.astThreeDNRValue[u32IsoLevel - 1].TFy[1].TFR1[j],
                                u32Right, st3dnrparam.astThreeDNRValue[u32IsoLevel].TFy[1].TFR1[j]);
            }

        }
        {
            //tedge
            stNrx.TFy[1].TED = SCENE_Interpulate(u32Mid, u32Left, st3dnrparam.astThreeDNRValue[u32IsoLevel - 1].TFy[1].TED,
                                u32Right, st3dnrparam.astThreeDNRValue[u32IsoLevel].TFy[1].TED);
            stNrx.TFy[2].TED = SCENE_Interpulate(u32Mid, u32Left, st3dnrparam.astThreeDNRValue[u32IsoLevel - 1].TFy[2].TED,
                                u32Right, st3dnrparam.astThreeDNRValue[u32IsoLevel].TFy[2].TED);

            //tfrs
            stNrx.TFy[0].TFRS = SCENE_Interpulate(u32Mid, u32Left, st3dnrparam.astThreeDNRValue[u32IsoLevel - 1].TFy[0].TFRS,
                                                u32Right, st3dnrparam.astThreeDNRValue[u32IsoLevel].TFy[0].TFRS);
        }

        /*MDy*/
        for (i = 0; i < 2; i++)
        {
            //id0
            stNrx.MDy[i].MAI00 = SCENE_Interpulate(u32Mid, u32Left, st3dnrparam.astThreeDNRValue[u32IsoLevel - 1].MDy[i].MAI00, u32Right, st3dnrparam.astThreeDNRValue[u32IsoLevel].MDy[i].MAI00);
            stNrx.MDy[i].MAI01 = SCENE_Interpulate(u32Mid, u32Left, st3dnrparam.astThreeDNRValue[u32IsoLevel - 1].MDy[i].MAI01, u32Right, st3dnrparam.astThreeDNRValue[u32IsoLevel].MDy[i].MAI01);
            stNrx.MDy[i].MAI02 = SCENE_Interpulate(u32Mid, u32Left, st3dnrparam.astThreeDNRValue[u32IsoLevel - 1].MDy[i].MAI02, u32Right, st3dnrparam.astThreeDNRValue[u32IsoLevel].MDy[i].MAI02);
            //id1
            stNrx.MDy[0].MAI10 = SCENE_Interpulate(u32Mid, u32Left, st3dnrparam.astThreeDNRValue[u32IsoLevel - 1].MDy[0].MAI10,
                                u32Right, st3dnrparam.astThreeDNRValue[u32IsoLevel].MDy[0].MAI10);
            stNrx.MDy[0].MAI11 = SCENE_Interpulate(u32Mid, u32Left, st3dnrparam.astThreeDNRValue[u32IsoLevel - 1].MDy[0].MAI11,
                                u32Right, st3dnrparam.astThreeDNRValue[u32IsoLevel].MDy[0].MAI11);
            stNrx.MDy[0].MAI12 = SCENE_Interpulate(u32Mid, u32Left, st3dnrparam.astThreeDNRValue[u32IsoLevel - 1].MDy[0].MAI12,
                                u32Right, st3dnrparam.astThreeDNRValue[u32IsoLevel].MDy[0].MAI12);
            //mabr0
            stNrx.MDy[i].MABR0 = SCENE_Interpulate(u32Mid, u32Left, st3dnrparam.astThreeDNRValue[u32IsoLevel - 1].MDy[i].MABR0,
                                u32Right, st3dnrparam.astThreeDNRValue[u32IsoLevel].MDy[i].MABR0);
            stNrx.MDy[0].MABR1 = SCENE_Interpulate(u32Mid, u32Left, st3dnrparam.astThreeDNRValue[u32IsoLevel - 1].MDy[0].MABR1,
                                u32Right, st3dnrparam.astThreeDNRValue[u32IsoLevel].MDy[0].MABR1);
            //math
            stNrx.MDy[i].MATH0 = SCENE_Interpulate(u32Mid, u32Left, st3dnrparam.astThreeDNRValue[u32IsoLevel - 1].MDy[i].MATH0,
                                u32Right, st3dnrparam.astThreeDNRValue[u32IsoLevel].MDy[i].MATH0);
            stNrx.MDy[i].MATH1 = SCENE_Interpulate(u32Mid, u32Left, st3dnrparam.astThreeDNRValue[u32IsoLevel - 1].MDy[i].MATH1,
                                u32Right, st3dnrparam.astThreeDNRValue[u32IsoLevel].MDy[i].MATH1);
            //mathd
            stNrx.MDy[i].MATHd0 = SCENE_Interpulate(u32Mid, u32Left, st3dnrparam.astThreeDNRValue[u32IsoLevel - 1].MDy[i].MATHd0,
                                u32Right, st3dnrparam.astThreeDNRValue[u32IsoLevel].MDy[i].MATHd0);
            stNrx.MDy[i].MATHd1 = SCENE_Interpulate(u32Mid, u32Left, st3dnrparam.astThreeDNRValue[u32IsoLevel - 1].MDy[i].MATHd1,
                                u32Right, st3dnrparam.astThreeDNRValue[u32IsoLevel].MDy[i].MATHd1);
            //mate
            stNrx.MDy[i].MATE0 = SCENE_Interpulate(u32Mid, u32Left, st3dnrparam.astThreeDNRValue[u32IsoLevel - 1].MDy[i].MATE0,
                                u32Right, st3dnrparam.astThreeDNRValue[u32IsoLevel].MDy[i].MATE0);
            stNrx.MDy[i].MATE1 = SCENE_Interpulate(u32Mid, u32Left, st3dnrparam.astThreeDNRValue[u32IsoLevel - 1].MDy[i].MATE1,
                                u32Right, st3dnrparam.astThreeDNRValue[u32IsoLevel].MDy[i].MATE1);
            //matw
            stNrx.MDy[i].MATW = SCENE_Interpulate(u32Mid, u32Left, st3dnrparam.astThreeDNRValue[u32IsoLevel - 1].MDy[i].MATW,
                                u32Right, st3dnrparam.astThreeDNRValue[u32IsoLevel].MDy[i].MATW);

            //mabw
            stNrx.MDy[i].MABW0 = SCENE_Interpulate(u32Mid, u32Left, st3dnrparam.astThreeDNRValue[u32IsoLevel - 1].MDy[i].MABW0,
                                u32Right, st3dnrparam.astThreeDNRValue[u32IsoLevel].MDy[i].MABW0);
            stNrx.MDy[i].MABW1  = SCENE_Interpulate(u32Mid, u32Left, st3dnrparam.astThreeDNRValue[u32IsoLevel - 1].MDy[i].MABW1,
                                u32Right, st3dnrparam.astThreeDNRValue[u32IsoLevel].MDy[i].MABW1);
            //AdvMath
            stNrx.MDy[0].AdvMATH  = SCENE_Interpulate(u32Mid, u32Left, st3dnrparam.astThreeDNRValue[u32IsoLevel - 1].MDy[0].AdvMATH,
                                            u32Right, st3dnrparam.astThreeDNRValue[u32IsoLevel].MDy[0].AdvMATH);
            //AdvTh
            stNrx.MDy[0].AdvTH  = SCENE_Interpulate(u32Mid, u32Left, st3dnrparam.astThreeDNRValue[u32IsoLevel - 1].MDy[0].AdvTH,
                                            u32Right, st3dnrparam.astThreeDNRValue[u32IsoLevel].MDy[0].AdvTH);

        }

        /*NRc*/
        stNrx.NRc.SFC = SCENE_Interpulate(u32Mid, u32Left, st3dnrparam.astThreeDNRValue[u32IsoLevel - 1].NRc.SFC,
                        u32Right, st3dnrparam.astThreeDNRValue[u32IsoLevel].NRc.SFC);
        stNrx.NRc.TFC = SCENE_Interpulate(u32Mid, u32Left, st3dnrparam.astThreeDNRValue[u32IsoLevel - 1].NRc.TFC,
                        u32Right, st3dnrparam.astThreeDNRValue[u32IsoLevel].NRc.TFC);
        stNrx.NRc.TPC = SCENE_Interpulate(u32Mid, u32Left, st3dnrparam.astThreeDNRValue[u32IsoLevel - 1].NRc.TPC,
                        u32Right, st3dnrparam.astThreeDNRValue[u32IsoLevel].NRc.TPC);
        stNrx.NRc.TRC = SCENE_Interpulate(u32Mid, u32Left, st3dnrparam.astThreeDNRValue[u32IsoLevel - 1].NRc.TRC,
                        u32Right, st3dnrparam.astThreeDNRValue[u32IsoLevel].NRc.TRC);

    }

    s32Ret = HI_SCENE_Set3DNR(ViPipe, &stNrx, u8Index);
    CHECK_SCENE_RET(s32Ret);

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

HI_S32 HI_SCENE_SetQP(HI_U32  u32PicWidth, HI_U32  u32PicHeight,HI_U32 u32MaxBitRate,PAYLOAD_TYPE_E  enType,VENC_RC_PARAM_S * pstRcParam)
{
     HI_U32 u32AlignPicHeight;
     HI_U32 u32AlignPicWidht;
     HI_U32 u32AlignPicHeight_1080P;
     HI_U32 u32AlignPicWidht_1080P;

     u32AlignPicHeight = ((u32PicHeight + MB_ALIGN_H264 - 1) / MB_ALIGN_H264);
     u32AlignPicWidht  = ((u32PicWidth + MB_ALIGN_H264- 1) / MB_ALIGN_H264);
     u32AlignPicHeight_1080P = ((PIC_HEIGHT_1080P + MB_ALIGN_H264- 1) / MB_ALIGN_H264);
     u32AlignPicWidht_1080P  = ((PIC_WIDTH_1080P + MB_ALIGN_H264- 1) / MB_ALIGN_H264);
     u32MaxBitRate = (u32AlignPicHeight_1080P *u32AlignPicWidht_1080P*u32MaxBitRate)/(u32AlignPicHeight*u32AlignPicWidht);

    if(enType == PT_H265)
    {

        if(u32MaxBitRate <= 768)
        {
             pstRcParam->stParamH265AVbr.u32MinIQp = 30;
	      pstRcParam->stParamH265AVbr.u32MinQp  = 32;
	      pstRcParam->stParamH265AVbr.u32MaxIQp = 48;
	      pstRcParam->stParamH265AVbr.u32MaxQp = 48;
        }
	 else if(u32MaxBitRate > 768 && u32MaxBitRate <= 1536)
	 {
	      pstRcParam->stParamH265AVbr.u32MinIQp = 29;
	      pstRcParam->stParamH265AVbr.u32MinQp  = 31;
	      pstRcParam->stParamH265AVbr.u32MaxIQp = 44;
	      pstRcParam->stParamH265AVbr.u32MaxQp = 44;
        }
	 else if(u32MaxBitRate > 1536 && u32MaxBitRate <= 2560)
	 {
	      pstRcParam->stParamH265AVbr.u32MinIQp = 26;
	      pstRcParam->stParamH265AVbr.u32MinQp  = 28;
	      pstRcParam->stParamH265AVbr.u32MaxIQp = 42;
	      pstRcParam->stParamH265AVbr.u32MaxQp = 42;
        }
	 else if(u32MaxBitRate > 2560 && u32MaxBitRate <= 3584)
	 {
	      pstRcParam->stParamH265AVbr.u32MinIQp = 25;
	      pstRcParam->stParamH265AVbr.u32MinQp  = 27;
	      pstRcParam->stParamH265AVbr.u32MaxIQp = 42;
	      pstRcParam->stParamH265AVbr.u32MaxQp = 42;
        }
	 else if(u32MaxBitRate > 3584 && u32MaxBitRate <= 4096)
	 {
	      pstRcParam->stParamH265AVbr.u32MinIQp = 24;
	      pstRcParam->stParamH265AVbr.u32MinQp  = 26;
	      pstRcParam->stParamH265AVbr.u32MaxIQp = 42;
	      pstRcParam->stParamH265AVbr.u32MaxQp = 42;
        }
	 else if(u32MaxBitRate > 4096)
	 {
	      pstRcParam->stParamH265AVbr.u32MinIQp = 23;
	      pstRcParam->stParamH265AVbr.u32MinQp  = 25;
	      pstRcParam->stParamH265AVbr.u32MaxIQp = 42;
	      pstRcParam->stParamH265AVbr.u32MaxQp = 42;
        }
       pstRcParam->stParamH265AVbr.u32MinIprop = 1;
       pstRcParam->stParamH265AVbr.u32MaxIprop  = 100;
       pstRcParam->stParamH265AVbr.s32MaxReEncodeTimes = 2;

       pstRcParam->stParamH265AVbr.s32MinStillPercent = 25;
	pstRcParam->stParamH265AVbr.u32MaxStillQP = 35;
       pstRcParam->stParamH265AVbr.u32MinStillPSNR = 0;
    }
    if(enType == PT_H264)
    {

        if(u32MaxBitRate <= 768)
        {
             pstRcParam->stParamH264AVbr.u32MinIQp = 30;
	      pstRcParam->stParamH264AVbr.u32MinQp  = 32;
	      pstRcParam->stParamH264AVbr.u32MaxIQp = 48;
	      pstRcParam->stParamH264AVbr.u32MaxQp = 48;
        }
	 else if(u32MaxBitRate > 768 && u32MaxBitRate <= 1536)
	 {
	      pstRcParam->stParamH264AVbr.u32MinIQp = 29;
	      pstRcParam->stParamH264AVbr.u32MinQp  = 31;
	      pstRcParam->stParamH264AVbr.u32MaxIQp = 44;
	      pstRcParam->stParamH264AVbr.u32MaxQp = 44;
        }
	 else if(u32MaxBitRate > 1536 && u32MaxBitRate <= 3072)
	 {
	      pstRcParam->stParamH264AVbr.u32MinIQp = 27;
	      pstRcParam->stParamH264AVbr.u32MinQp  = 29;
	      pstRcParam->stParamH264AVbr.u32MaxIQp = 42;
	      pstRcParam->stParamH264AVbr.u32MaxQp = 42;
        }
	 else if(u32MaxBitRate > 3072 && u32MaxBitRate <= 5120)
	 {
	      pstRcParam->stParamH264AVbr.u32MinIQp = 26;
	      pstRcParam->stParamH264AVbr.u32MinQp  = 28;
	      pstRcParam->stParamH264AVbr.u32MaxIQp = 42;
	      pstRcParam->stParamH264AVbr.u32MaxQp = 42;
        }
	 else if(u32MaxBitRate > 5120 && u32MaxBitRate <= 7168)
	 {
	      pstRcParam->stParamH264AVbr.u32MinIQp = 25;
	      pstRcParam->stParamH264AVbr.u32MinQp  = 27;
	      pstRcParam->stParamH264AVbr.u32MaxIQp = 42;
	      pstRcParam->stParamH264AVbr.u32MaxQp = 42;
        }
	 else if(u32MaxBitRate > 7168)
	 {
	      pstRcParam->stParamH264AVbr.u32MinIQp = 23;
	      pstRcParam->stParamH264AVbr.u32MinQp  = 25;
	      pstRcParam->stParamH264AVbr.u32MaxIQp = 42;
	      pstRcParam->stParamH264AVbr.u32MaxQp = 42;
        }
       pstRcParam->stParamH264AVbr.u32MinIprop = 1;
       pstRcParam->stParamH264AVbr.u32MaxIprop  = 100;
       pstRcParam->stParamH264AVbr.s32MaxReEncodeTimes = 2;

       pstRcParam->stParamH264AVbr.s32MinStillPercent = 25;
	pstRcParam->stParamH264AVbr.u32MaxStillQP = 35;
       pstRcParam->stParamH264AVbr.u32MinStillPSNR = 0;
    }
    return HI_SUCCESS;
}
HI_S32 HI_SCENE_SetRCParam(VI_PIPE ViPipe,HI_U8 u8Index)
{
    HI_S32 s32Ret = HI_SUCCESS;
    ISP_EXP_INFO_S stIspExpInfo;
    VENC_CHN_ATTR_S stVencChnAttr;
    VENC_CHN_STATUS_S stStatEx;
    VENC_RC_PARAM_S stRcParam;

    HI_U32 i,u32MinQP,u32Iso;
    HI_U32 u32MeanQp;								/*the start Qp of encoded frames*/
    HI_S32 s32Percent=90;
    HI_U32 u32SumMeanQP=0;
    HI_U32 u32SumIso = 0;
    HI_U32 u32MaxBitRate;
    PAYLOAD_TYPE_E  enType;
    static HI_U32  u32IsoInfo[8] = {100,100,100,100,100,100,100,100,};
    static HI_U32  u32MeanQpInfo[8] ={30,30,30,30,30,30,30,30};
    HI_U32  u32PicWidth;
    HI_U32  u32PicHeight;
    HI_U32  u32Index = 0;

    if(HI_TRUE != g_astScenePipeParam[u8Index].stModuleState.bDynamicIsoVenc)
    {
       // printf("HI_SCENE_SetRCParam will not use scencauto !\n");
    	return HI_SUCCESS;
    }

    s32Ret = HI_MPI_VENC_GetChnAttr(ViPipe, &stVencChnAttr);
    CHECK_SCENE_RET(s32Ret);


    if(stVencChnAttr.stRcAttr.enRcMode != VENC_RC_MODE_H264AVBR && stVencChnAttr.stRcAttr.enRcMode != VENC_RC_MODE_H265AVBR)
    {
        return HI_SUCCESS;
    }

    s32Ret = HI_MPI_ISP_QueryExposureInfo(ViPipe, &stIspExpInfo);
    CHECK_SCENE_RET(s32Ret);

    u32Iso = stIspExpInfo.u32ISO;

    s32Ret = HI_MPI_VENC_QueryStatus(ViPipe,&stStatEx);
    CHECK_SCENE_RET(s32Ret);

    enType  = stVencChnAttr.stVencAttr.enType;
    //u32StartQp = stStatEx.stVencStrmInfo.u32StartQp;
    u32MeanQp = stStatEx.stVencStrmInfo.u32MeanQp;
    u32MinQP = u32MeanQpInfo[0];
    u32MaxBitRate = (enType == PT_H264)?stVencChnAttr.stRcAttr.stH264AVbr.u32MaxBitRate : stVencChnAttr.stRcAttr.stH265AVbr.u32MaxBitRate;
    u32PicWidth = (enType == PT_H264)?stVencChnAttr.stVencAttr.u32PicWidth  : stVencChnAttr.stVencAttr.u32PicWidth;
    u32PicHeight = (enType == PT_H264)?stVencChnAttr.stVencAttr.u32PicHeight  : stVencChnAttr.stVencAttr.u32PicHeight;
   //printf("--1-- u32Iso = %d,u32MeanQp = %d,enType = %d\n",u32Iso,u32MeanQp,enType);

    for (i = 0; i < 7; i++)
    {
	 u32IsoInfo[i]= u32IsoInfo[i+1];
	 u32MeanQpInfo[i]=u32MeanQpInfo[i+1];
    }
    u32IsoInfo[i]= u32Iso;
    u32MeanQpInfo[i]=u32MeanQp;

    for (i=0; i< 8; i++)
    {
        if(u32MinQP < u32MeanQpInfo[i]) u32MinQP = u32MeanQpInfo[i];
        u32SumMeanQP += u32MeanQpInfo[i];
	    u32SumIso += u32IsoInfo[i];
    }
    u32SumMeanQP = u32SumMeanQP-u32MinQP;
    u32SumMeanQP = u32SumMeanQP/7;
    u32SumIso = u32SumIso/8;

    for(i =0;i < g_astScenePipeParam[u8Index].stDynamicVencBitrate.u32IsoThreshCnt;i++)
    {
         if( u32SumIso > g_astScenePipeParam[u8Index].stDynamicVencBitrate.au32IsoThreshLtoH[i]) u32Index++;
    }

    s32Ret= HI_MPI_VENC_GetRcParam(ViPipe, &stRcParam);
    CHECK_SCENE_RET(s32Ret);
    if(enType == PT_H264)
    {
        stRcParam.stParamH264AVbr.s32ChangePos =  g_astScenePipeParam[u8Index].stDynamicVencBitrate.au16ManualPercent[u32Index -1];
        s32Percent = stRcParam.stParamH264AVbr.s32ChangePos;
    }
    if(enType == PT_H265)
    {
        stRcParam.stParamH265AVbr.s32ChangePos =  g_astScenePipeParam[u8Index].stDynamicVencBitrate.au16ManualPercent[u32Index -1];
        s32Percent = stRcParam.stParamH265AVbr.s32ChangePos;
    }
//   printf("--2-- u32Iso = %d,u32SumMeanQP = %d,enType = %d,au32IsoThreshLtoH = %d\n",u32Iso,u32SumMeanQP,enType,g_astScenePipeParam[u8Index].stDynamicVencBitrate.au16ManualStrength[2]);
    if(u32SumIso >  g_astScenePipeParam[u8Index].stDynamicVencBitrate.au32IsoThreshLtoH[2])
    {
          //printf("--2-- u32Iso = %d,u32SumMeanQP = %d,enType = %d,s32ChangePos = %d\n",u32Iso,u32SumMeanQP,enType,stRcParam.stParamH265AVbr.s32ChangePos);

          if(u32SumMeanQP <= 33 && u32SumMeanQP >= 31)
          {
         	  s32Percent = 19 * s32Percent/20;
          }
          else if(u32SumMeanQP <= 30 && u32SumMeanQP >= 29)
   	      {
   	          s32Percent = 9 * s32Percent/10;
   	      }
	      else if(u32SumMeanQP < 29 )
   	      {
   	          s32Percent = 8 * s32Percent/10;
   	      }

          if(enType == PT_H264)
          {
               stRcParam.stParamH264AVbr.s32ChangePos = s32Percent;
	           stRcParam.stParamH264AVbr.u32MinQpDelta = 2;
          }
	      if(enType == PT_H265)
          {
               stRcParam.stParamH265AVbr.s32ChangePos = s32Percent;
	           stRcParam.stParamH265AVbr.u32MinQpDelta = 2;
          }
	   //printf("--4-- u32Iso = %d,s32ChangePos = %d,u32MinQpDelta = %d,s32MinStillPercent = %d\n",u32SumIso,stRcParam.stParamH265AVbr.s32ChangePos,stRcParam.stParamH265AVbr.u32MinQpDelta,stRcParam.stParamH265AVbr.s32MinStillPercent);


    }
    else
    {

	  //printf("--3-- u32Iso = %d,s32ChangePos = %d,u32MinQpDelta = %d,s32MinStillPercent = %d\n",u32SumIso,stRcParam.stParamH265AVbr.s32ChangePos,stRcParam.stParamH265AVbr.u32MinQpDelta,stRcParam.stParamH265AVbr.s32MinStillPercent);
	   if(stVencChnAttr.stGopAttr.enGopMode == VENC_GOPMODE_SMARTP)
      	   {
	      	   if(enType == PT_H264)
	          {
	               //stRcParam.stParamH264AVbr.s32ChangePos = s32Percent;
		        stRcParam.stParamH264AVbr.u32MinQpDelta = 3;
	          }
		      if(enType == PT_H265)
	          {
	               //stRcParam.stParamH265AVbr.s32ChangePos = s32Percent;
		        stRcParam.stParamH265AVbr.u32MinQpDelta = 3;
	          }
          }
	   else
	   {
		      if(enType == PT_H264)
	          {
	              // stRcParam.stParamH264AVbr.s32ChangePos = s32Percent;
		        stRcParam.stParamH264AVbr.u32MinQpDelta = 3;
	          }
		      if(enType == PT_H265)
	          {
	               //sstRcParam.stParamH265AVbr.s32ChangePos = s32Percent;
		        stRcParam.stParamH265AVbr.u32MinQpDelta = 3;
	          }
	   }
    }

    s32Ret = HI_SCENE_SetQP(u32PicWidth,u32PicHeight,u32MaxBitRate, enType, &stRcParam);
    CHECK_SCENE_RET(s32Ret);

    s32Ret= HI_MPI_VENC_SetRcParam(ViPipe, &stRcParam);
    CHECK_SCENE_RET(s32Ret);

    return HI_SUCCESS;
}
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
