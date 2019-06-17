#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <sys/prctl.h>

#include "hi_sceneauto_comm.h"
#include "hi_type.h"
#include "hi_comm_vpss.h"
#include "mpi_vpss.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

void V19yPrintNRs(const HI_SCENEAUTO_3DNR_S *pS )
{
  printf("\n\n                             sizeof( S ) = %d ", sizeof(*pS) );
  printf("\n*******************************************************************************");
  
  printf("\n   sf0_bright  ( %3d )      sfx_bright    ( %3d,%3d )      sfk_bright ( %3d )",  pS->sf0_bright,  pS->sf1_bright,    pS->sf2_bright, pS->sfk_bright  );    
  printf("\n   sf0_dark    ( %3d )      sfx_dark      ( %3d,%3d )      sfk_dark   ( %3d )",  pS->sf0_dark  ,  pS->sf1_dark  ,    pS->sf2_dark  , pS->sfk_dark    );       
  printf("\n  ----------------------+                                  sfk_pro    ( %3d )",                                                      pS->sfk_profile );
  printf("\n                        |   tfx_moving    ( %3d,%3d )      sfk_ratio  ( %3d )",                   pS->tf1_moving,    pS->tf2_moving, pS->sfk_ratio   ); 
  printf("\n   csf_strength( %3d )  |   tfx_md_thresh ( %3d,%3d )                        ",  pS->csf_strength,pS->tf1_md_thresh, pS->tf2_md_thresh                  ); 
  printf("\n   ctf_strength( %3d )  |   tfx_md_profile( %3d,%3d )    sf_ed_thresh ( %3d )",  pS->ctf_strength,pS->tf1_md_profile,pS->tf2_md_profile,pS->sf_ed_thresh); 
  printf("\n   ctf_range   ( %3d )  |   tfx_md_type   ( %3d,%3d )    sf_bsc_freq  ( %3d )",  pS->ctf_range,   pS->tf1_md_type,   pS->tf2_md_type,   pS->sf_bsc_freq ); 
  printf("\n                        |   tfx_still     ( %3d,%3d )    tf_profile   ( %3d )",                   pS->tf1_still,     pS->tf2_still,     pS->tf_profile  ); 
  
  printf("\n*******************************************************************************\n\n");  
}

void V19yPrintVpssNRs(const VPSS_GRP_NRS_PARAM_S *pS )
{
  printf("\n\n                             sizeof( S ) = %d ", sizeof(*pS) );
  printf("\n*******************************************************************************");
  
  printf("\n   sf0_bright  ( %3d )      sfx_bright    ( %3d,%3d )      sfk_bright ( %3d )",  pS->stNRSParam_V3.sf0_bright,  pS->stNRSParam_V3.sf1_bright,    pS->stNRSParam_V3.sf2_bright, pS->stNRSParam_V3.sfk_bright  );    
  printf("\n   sf0_dark    ( %3d )      sfx_dark      ( %3d,%3d )      sfk_dark   ( %3d )",  pS->stNRSParam_V3.sf0_dark  ,  pS->stNRSParam_V3.sf1_dark  ,    pS->stNRSParam_V3.sf2_dark  , pS->stNRSParam_V3.sfk_dark    );       
  printf("\n  ----------------------+                                  sfk_pro    ( %3d )",                                                      pS->stNRSParam_V3.sfk_profile );
  printf("\n                        |   tfx_moving    ( %3d,%3d )      sfk_ratio  ( %3d )",                   pS->stNRSParam_V3.tf1_moving,    pS->stNRSParam_V3.tf2_moving, pS->stNRSParam_V3.sfk_ratio   ); 
  printf("\n   csf_strength( %3d )  |   tfx_md_thresh ( %3d,%3d )                        ",  pS->stNRSParam_V3.csf_strength,pS->stNRSParam_V3.tf1_md_thresh, pS->stNRSParam_V3.tf2_md_thresh                  ); 
  printf("\n   ctf_strength( %3d )  |   tfx_md_profile( %3d,%3d )    sf_ed_thresh ( %3d )",  pS->stNRSParam_V3.ctf_strength,pS->stNRSParam_V3.tf1_md_profile,pS->stNRSParam_V3.tf2_md_profile,pS->stNRSParam_V3.sf_ed_thresh); 
  printf("\n   ctf_range   ( %3d )  |   tfx_md_type   ( %3d,%3d )    sf_bsc_freq  ( %3d )",  pS->stNRSParam_V3.ctf_range,   pS->stNRSParam_V3.tf1_md_type,   pS->stNRSParam_V3.tf2_md_type,   pS->stNRSParam_V3.sf_bsc_freq ); 
  printf("\n                        |   tfx_still     ( %3d,%3d )    tf_profile   ( %3d )",                   pS->stNRSParam_V3.tf1_still,     pS->stNRSParam_V3.tf2_still,     pS->stNRSParam_V3.tf_profile  ); 
  
  printf("\n*******************************************************************************\n\n");  
}

HI_S32 Sceneauto_SetNrS(VPSS_GRP VpssGrp, const HI_SCENEAUTO_3DNR_S *pst3Nrs)
{
    HI_S32 s32Ret = HI_SUCCESS;
	VPSS_GRP_NRS_PARAM_S stVpssGrpNrS , stVpssGrpNrSGet;

    stVpssGrpNrS.enNRVer = VPSS_NR_V3;
	s32Ret = HI_MPI_VPSS_GetGrpNRSParam(VpssGrp, &stVpssGrpNrS);
	if (HI_SUCCESS != s32Ret)
    {
        printf("HI_MPI_VPSS_GetNRSParam failed\n");
        return HI_FAILURE;
    }

    stVpssGrpNrS.stNRSParam_V3.sf0_bright =  pst3Nrs->sf0_bright;
    stVpssGrpNrS.stNRSParam_V3.sf1_bright =  pst3Nrs->sf1_bright;
    stVpssGrpNrS.stNRSParam_V3.sf2_bright =  pst3Nrs->sf2_bright;
    stVpssGrpNrS.stNRSParam_V3.sfk_bright =  pst3Nrs->sfk_bright;

    stVpssGrpNrS.stNRSParam_V3.sf0_dark =  pst3Nrs->sf0_dark;
    stVpssGrpNrS.stNRSParam_V3.sf1_dark =  pst3Nrs->sf1_dark;
    stVpssGrpNrS.stNRSParam_V3.sf2_dark =  pst3Nrs->sf2_dark;
    stVpssGrpNrS.stNRSParam_V3.sfk_dark =  pst3Nrs->sfk_dark;

    stVpssGrpNrS.stNRSParam_V3.sf_ed_thresh =  pst3Nrs->sf_ed_thresh;
    stVpssGrpNrS.stNRSParam_V3.sfk_profile =  pst3Nrs->sfk_profile;
    stVpssGrpNrS.stNRSParam_V3.sfk_ratio =  pst3Nrs->sfk_ratio;
    stVpssGrpNrS.stNRSParam_V3.sf_bsc_freq =  pst3Nrs->sf_bsc_freq;

    stVpssGrpNrS.stNRSParam_V3.tf1_md_thresh =  pst3Nrs->tf1_md_thresh;
    stVpssGrpNrS.stNRSParam_V3.tf2_md_thresh =  pst3Nrs->tf2_md_thresh;
    stVpssGrpNrS.stNRSParam_V3.tf1_still =  pst3Nrs->tf1_still;
    stVpssGrpNrS.stNRSParam_V3.tf2_still =  pst3Nrs->tf2_still;
    stVpssGrpNrS.stNRSParam_V3.tf1_md_type =  pst3Nrs->tf1_md_type;
    stVpssGrpNrS.stNRSParam_V3.tf2_md_type =  pst3Nrs->tf2_md_type;
    stVpssGrpNrS.stNRSParam_V3.tf1_moving =  pst3Nrs->tf1_moving;
    stVpssGrpNrS.stNRSParam_V3.tf2_moving =  pst3Nrs->tf2_moving;
    stVpssGrpNrS.stNRSParam_V3.tf1_md_profile =  pst3Nrs->tf1_md_profile;
    stVpssGrpNrS.stNRSParam_V3.tf2_md_profile =  pst3Nrs->tf2_md_profile;
    stVpssGrpNrS.stNRSParam_V3.tf_profile =  pst3Nrs->tf_profile;

    stVpssGrpNrS.stNRSParam_V3.csf_strength =  pst3Nrs->csf_strength;
    stVpssGrpNrS.stNRSParam_V3.ctf_strength =  pst3Nrs->ctf_strength;
    stVpssGrpNrS.stNRSParam_V3.ctf_range =  pst3Nrs->ctf_range;
    
    //printf("\nVpssNR_Set S:\n");
    //V19yPrintVpssNRs(&stVpssGrpNrS);
    
    s32Ret = HI_MPI_VPSS_SetGrpNRSParam(VpssGrp, &stVpssGrpNrS);
	if (HI_SUCCESS != s32Ret)
    {
        printf("HI_MPI_VPSS_SetNRSParam failed\n");
        return HI_FAILURE;
    }

    stVpssGrpNrSGet.enNRVer = VPSS_NR_V3;
    s32Ret = HI_MPI_VPSS_GetGrpNRSParam(VpssGrp, &stVpssGrpNrSGet);
	if (HI_SUCCESS != s32Ret)
    {
        printf("HI_MPI_VPSS_GetGrpNRSParam failed\n");
        return HI_FAILURE;
    }
    //printf("\nVpssNR_Get S:\n");
    //V19yPrintVpssNRs(&stVpssGrpNrSGet);

    return HI_SUCCESS;
}

HI_S32 Sceneauto_SetNrX(VPSS_GRP VpssGrp, const HI_SCENEAUTO_3DNR_X_S *pst3Nrx)
{
    HI_U32 i = 0, j = 0;
    HI_S32 s32Ret = HI_SUCCESS;
    VPSS_GRP_NRX_PARAM_S stVpssGrpNrX;
    stVpssGrpNrX.enNRVer = VPSS_NR_V1;
    stVpssGrpNrX.stNRXParam_V1.enOptType = VPSS_NRS_MANUAL;

	s32Ret = HI_MPI_VPSS_GetGrpNRXParam(VpssGrp, &stVpssGrpNrX);
	if (HI_SUCCESS != s32Ret)
    {
        printf("HI_MPI_VPSS_GetGrpNRXParam failed\n");
        return HI_FAILURE;
    }

    /* IEy */
    stVpssGrpNrX.stNRXParam_V1.stVideoNrXV1Manual.stVideoNrXParamV1.IEy.IES = pst3Nrx->IEy.IES;
    stVpssGrpNrX.stNRXParam_V1.stVideoNrXV1Manual.stVideoNrXParamV1.IEy.IET = pst3Nrx->IEy.IET;
    stVpssGrpNrX.stNRXParam_V1.stVideoNrXV1Manual.stVideoNrXParamV1.IEy.IEB = pst3Nrx->IEy.IEB;

    /* SFy[4] */
    for (i = 0; i < 4; i++)
    {
        stVpssGrpNrX.stNRXParam_V1.stVideoNrXV1Manual.stVideoNrXParamV1.SFy[i].SBF0 = pst3Nrx->SFy[i].SBF0;
        stVpssGrpNrX.stNRXParam_V1.stVideoNrXV1Manual.stVideoNrXParamV1.SFy[i].SBF1 = pst3Nrx->SFy[i].SBF1;
        stVpssGrpNrX.stNRXParam_V1.stVideoNrXV1Manual.stVideoNrXParamV1.SFy[i].SBF2 = pst3Nrx->SFy[i].SBF2;
        stVpssGrpNrX.stNRXParam_V1.stVideoNrXV1Manual.stVideoNrXParamV1.SFy[i].VRTO = pst3Nrx->SFy[i].VRTO;
        stVpssGrpNrX.stNRXParam_V1.stVideoNrXV1Manual.stVideoNrXParamV1.SFy[i].horPro = pst3Nrx->SFy[i].horPro;
        stVpssGrpNrX.stNRXParam_V1.stVideoNrXV1Manual.stVideoNrXParamV1.SFy[i].verPro = pst3Nrx->SFy[i].verPro;
        stVpssGrpNrX.stNRXParam_V1.stVideoNrXV1Manual.stVideoNrXParamV1.SFy[i].SFkRfw = pst3Nrx->SFy[i].SFkRfw;
        stVpssGrpNrX.stNRXParam_V1.stVideoNrXV1Manual.stVideoNrXParamV1.SFy[i].kProDD = pst3Nrx->SFy[i].kProDD;
        stVpssGrpNrX.stNRXParam_V1.stVideoNrXV1Manual.stVideoNrXParamV1.SFy[i].SFkType = pst3Nrx->SFy[i].SFkType;
        stVpssGrpNrX.stNRXParam_V1.stVideoNrXV1Manual.stVideoNrXParamV1.SFy[i].SSLP = pst3Nrx->SFy[i].SSLP;
        stVpssGrpNrX.stNRXParam_V1.stVideoNrXV1Manual.stVideoNrXParamV1.SFy[i].SFkBig = pst3Nrx->SFy[i].SFkBig;
        for (j = 0; j < 3; j++)
        {
            stVpssGrpNrX.stNRXParam_V1.stVideoNrXV1Manual.stVideoNrXParamV1.SFy[i].SBS[j] = pst3Nrx->SFy[i].SBS[j];
            stVpssGrpNrX.stNRXParam_V1.stVideoNrXV1Manual.stVideoNrXParamV1.SFy[i].SDS[j] = pst3Nrx->SFy[i].SDS[j];
            stVpssGrpNrX.stNRXParam_V1.stVideoNrXV1Manual.stVideoNrXParamV1.SFy[i].STH[j] = pst3Nrx->SFy[i].STH[j];
        }
    }

    /* MDy[3] */
    for (i = 0; i < 3; i++)
    {
        stVpssGrpNrX.stNRXParam_V1.stVideoNrXV1Manual.stVideoNrXParamV1.MDy[i].MATH = pst3Nrx->MDy[i].MATH;
        stVpssGrpNrX.stNRXParam_V1.stVideoNrXV1Manual.stVideoNrXParamV1.MDy[i].MATE = pst3Nrx->MDy[i].MATE;
        stVpssGrpNrX.stNRXParam_V1.stVideoNrXV1Manual.stVideoNrXParamV1.MDy[i].MATW = pst3Nrx->MDy[i].MATW;
        stVpssGrpNrX.stNRXParam_V1.stVideoNrXV1Manual.stVideoNrXParamV1.MDy[i].MASW = pst3Nrx->MDy[i].MASW;
        stVpssGrpNrX.stNRXParam_V1.stVideoNrXV1Manual.stVideoNrXParamV1.MDy[i].MABW = pst3Nrx->MDy[i].MABW;
        stVpssGrpNrX.stNRXParam_V1.stVideoNrXV1Manual.stVideoNrXParamV1.MDy[i].MAXN = pst3Nrx->MDy[i].MAXN;
    }

    /* TFy[4] */
    for (i = 0; i < 4; i++)
    {
        stVpssGrpNrX.stNRXParam_V1.stVideoNrXV1Manual.stVideoNrXParamV1.TFy[i].MDDZ = pst3Nrx->TFy[i].MDDZ;
        stVpssGrpNrX.stNRXParam_V1.stVideoNrXV1Manual.stVideoNrXParamV1.TFy[i].SDZ = pst3Nrx->TFy[i].SDZ;
        stVpssGrpNrX.stNRXParam_V1.stVideoNrXV1Manual.stVideoNrXParamV1.TFy[i].TFS = pst3Nrx->TFy[i].TFS;
        stVpssGrpNrX.stNRXParam_V1.stVideoNrXV1Manual.stVideoNrXParamV1.TFy[i].TSS = pst3Nrx->TFy[i].TSS;
        stVpssGrpNrX.stNRXParam_V1.stVideoNrXV1Manual.stVideoNrXParamV1.TFy[i].TFT = pst3Nrx->TFy[i].TFT;
        stVpssGrpNrX.stNRXParam_V1.stVideoNrXV1Manual.stVideoNrXParamV1.TFy[i].STR = pst3Nrx->TFy[i].STR;
        for (j = 0; j < 4; j++)
        {
            stVpssGrpNrX.stNRXParam_V1.stVideoNrXV1Manual.stVideoNrXParamV1.TFy[i].SFR[j] = pst3Nrx->TFy[i].SFR[j];
            stVpssGrpNrX.stNRXParam_V1.stVideoNrXV1Manual.stVideoNrXParamV1.TFy[i].MTFR[j] = pst3Nrx->TFy[i].MTFR[j];
            stVpssGrpNrX.stNRXParam_V1.stVideoNrXV1Manual.stVideoNrXParamV1.TFy[i].TFR[j] = pst3Nrx->TFy[i].TFR[j];
        }
    }

    /* NRc */
    stVpssGrpNrX.stNRXParam_V1.stVideoNrXV1Manual.stVideoNrXParamV1.NRc.SFC = pst3Nrx->NRc.SFC;
    stVpssGrpNrX.stNRXParam_V1.stVideoNrXV1Manual.stVideoNrXParamV1.NRc.TFC = pst3Nrx->NRc.TFC;
    stVpssGrpNrX.stNRXParam_V1.stVideoNrXV1Manual.stVideoNrXParamV1.NRc.TPC = pst3Nrx->NRc.TPC;
    stVpssGrpNrX.stNRXParam_V1.stVideoNrXV1Manual.stVideoNrXParamV1.NRc.TRC = pst3Nrx->NRc.TRC;

    //printf("\nVpssNR_Set S:\n");
    //V19yPrintVpssNRs(&stVpssGrpNrS);
    
    s32Ret = HI_MPI_VPSS_SetGrpNRXParam(VpssGrp, &stVpssGrpNrX);
	if (HI_SUCCESS != s32Ret)
    {
        printf("HI_MPI_VPSS_SetGrpNRXParam failed\n");
        return HI_FAILURE;
    }
#if 0
    stVpssGrpNrSGet.enNRVer = VPSS_NR_V3;
    s32Ret = HI_MPI_VPSS_GetGrpNRSParam(VpssGrp, &stVpssGrpNrSGet);
	if (HI_SUCCESS != s32Ret)
    {
        printf("HI_MPI_VPSS_GetGrpNRSParam failed\n");
        return HI_FAILURE;
    }
#endif    
    //printf("\nVpssNR_Get S:\n");
    //V19yPrintVpssNRs(&stVpssGrpNrSGet);

    return HI_SUCCESS;
}


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */
