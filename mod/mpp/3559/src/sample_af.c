
#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>
#include "hi_type.h"
#include "mpi_isp.h"

#include "sample_comm.h"

#define BLEND_SHIFT 6
#define ALPHA 64 // 1
#define BELTA 54 // 0.85

static int FEAF = 0;
static int af_runing = 0;
static pthread_t af_tid;
static gsf_mpp_af_t af_ini;

#define LDM_LENS 1 //ldm;

#if LDM_LENS
static unsigned short  AFWeight[AE_ZONE_ROW][AE_ZONE_COLUMN] =
{
	{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, 
	{0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0}, 
	{0, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 1, 1, 0}, 
	{0, 1, 1, 1, 2, 4, 4, 4, 4, 4, 4, 4, 2, 1, 1, 1, 0}, 
	{0, 1, 1, 2, 4, 8, 8, 8, 8, 8, 8, 8, 4, 2, 1, 1, 0}, 
	{0, 1, 1, 2, 4, 8, 8,16,16,16, 8, 8, 4, 4, 2, 1, 0}, 
	{0, 1, 2, 4, 8, 8,16,16,48,16,16, 8, 8, 4, 2, 1, 0}, 
	{0, 1, 2, 4, 8, 8,16,48,96,48,16, 8, 8, 4, 2, 1, 0}, 
	{0, 1, 2, 4, 8, 8,16,16,48,16,16, 8, 8, 4, 2, 1, 0}, 
	{0, 1, 1, 2, 4, 8, 8,16,16,16, 8, 8, 4, 2, 1, 1, 0}, 
	{0, 1, 1, 2, 4, 8, 8, 8, 8, 8, 8, 8, 4, 2, 1, 1, 0}, 
	{0, 1, 1, 1, 2, 4, 4, 4, 4, 4, 4, 4, 2, 1, 1, 1, 0}, 
	{0, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2, 2, 1, 1, 1, 1, 0}, 
	{0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0}, 
	{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0} 

};
#else //ori
static int AFWeight[AE_ZONE_ROW][AE_ZONE_COLUMN] =
{
	{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
	{1, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1},
	{1, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1},
	{1, 1, 2, 2, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 2, 2, 1},
	{1, 1, 2, 2, 4, 4, 4, 4, 4, 4, 4, 2, 4, 4, 2, 2, 1},
	{1, 1, 2, 2, 4, 4, 4, 6, 6, 6, 6, 4, 4, 4, 2, 2, 1},
	{1, 1, 2, 2, 4, 4, 4, 6, 9, 9, 6, 4, 4, 4, 2, 2, 1},
	{1, 1, 2, 2, 4, 4, 4, 6, 9, 9, 6, 4, 4, 4, 2, 2, 1},
	{1, 1, 2, 2, 4, 4, 4, 6, 9, 9, 6, 4, 4, 4, 2, 2, 1},
	{1, 1, 2, 2, 4, 4, 4, 6, 6, 6, 6, 4, 4, 4, 2, 2, 1},
	{1, 1, 2, 2, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 2, 2, 1},
	{1, 1, 2, 2, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 2, 2, 1},
	{1, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1},
	{1, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1},
	{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
};

#endif

int af_set_statistic(VI_PIPE ViPipe)
{
	HI_S32 s32Ret = HI_SUCCESS;
	ISP_STATISTICS_CFG_S stIspStaticsCfg;
	ISP_PUB_ATTR_S stPubattr;
	//ISP_PUB_ATTR_S stPubattr;
  ISP_FOCUS_STATISTICS_CFG_S  stFocusCfg =
  {
    {1, 17, 15, 3840, 2160, 1, 0, {0, 0, 0, 3840, 2160}, 0, {0x2, 0x4, 0}, {1, 0x9bff}, 0xf0},
    {1, {1, 1, 1}, 15, {188, 414, -330, 486, -461, 400, -328}, {7, 0, 3, 1}, {1, 0, 255, 0, 220, 8, 14}, {127, 12, 2047} },
    {0, {1, 1, 0}, 2, {200, 200, -110, 461, -415, 0, 0}, {6, 0, 1, 0}, {0, 0, 0, 0, 0, 0, 0 }, {15, 12, 2047} },
    {{20, 16, 0, -16, -20}, {1, 0, 255, 0, 220, 8, 14}, {38, 12, 1800} },
    {{ -12, -24, 0, 24, 12}, {1, 0, 255, 0, 220, 8, 14}, {15, 12, 2047} },
    {4, {0, 0}, {1, 1}, 0}
  };

	s32Ret = HI_MPI_ISP_GetStatisticsConfig(ViPipe, &stIspStaticsCfg);
	s32Ret |= HI_MPI_ISP_GetPubAttr(ViPipe, &stPubattr);
	stFocusCfg.stConfig.u16Vsize = stFocusCfg.stConfig.stCrop.u16H = stPubattr.stWndRect.u32Height;
	stFocusCfg.stConfig.u16Hsize = stFocusCfg.stConfig.stCrop.u16W = stPubattr.stWndRect.u32Width;
	printf("u32Width:%d, u32Height:%d, bit1FEAfStat:%d, bit1BEAfStat:%d\n"
	      , stFocusCfg.stConfig.u16Hsize, stFocusCfg.stConfig.u16Vsize
	      , stIspStaticsCfg.unKey.bit1FEAfStat, stIspStaticsCfg.unKey.bit1BEAfStat);
	if (HI_SUCCESS != s32Ret)
	{
		printf("HI_MPI_ISP_GetStatisticsConfig error!(s32Ret = 0x%x)\n", s32Ret);
		return HI_FAILURE;
	}
	memcpy(&stIspStaticsCfg.stFocusCfg, &stFocusCfg, sizeof(ISP_FOCUS_STATISTICS_CFG_S));
	s32Ret = HI_MPI_ISP_SetStatisticsConfig(ViPipe, & stIspStaticsCfg);
	if (HI_SUCCESS != s32Ret)
	{
		printf("HI_MPI_ISP_SetStatisticsConfig error!(s32Ret = 0x%x)\n", s32Ret);
		return HI_FAILURE;
	}
	return HI_SUCCESS;
}

int af_get_vd(void)
{
  HI_S32 s32Ret = 0;
  s32Ret = HI_MPI_ISP_GetVDTimeOut(0, ISP_VD_FE_START, 5000);
  return s32Ret;
}

int af_get_value(HI_U32 FV[2])
{
	ISP_AF_STATISTICS_S stIspStatics;
	HI_S32 s32Ret = 0;
	HI_U32 u32SumFv1 = 0, u32SumFv2 = 0, u32WgtSum = 0;
	HI_U32 u32Fv1_n = 0, u32Fv2_n = 0;
	int i = 0, j = 0;

	s32Ret = HI_MPI_ISP_GetFocusStatistics(0, &stIspStatics);
	if (HI_SUCCESS != s32Ret)
	{
		printf("HI_MPI_ISP_GetStatistics error!(s32Ret = 0x%x)\n", s32Ret);
		return s32Ret;
	}
	
	u32SumFv1 = u32SumFv2 = u32WgtSum = 0;
	for (i = 0; i < AF_ZONE_ROW; i++)
	{
		for (j = 0; j < AF_ZONE_COLUMN; j++)
		{
			HI_U32 u32H1, u32H2, u32V1, u32V2;
			
			if(FEAF == 1)
			{
				u32H1 = stIspStatics.stFEAFStat.stZoneMetrics[0][i][j].u16h1;
				u32H2 = stIspStatics.stFEAFStat.stZoneMetrics[0][i][j].u16h2;
				u32V1 = stIspStatics.stFEAFStat.stZoneMetrics[0][i][j].u16v1;
				u32V2 = stIspStatics.stFEAFStat.stZoneMetrics[0][i][j].u16v2;
			}
			else
			{
				u32H1 = stIspStatics.stBEAFStat.stZoneMetrics[i][j].u16h1;
				u32H2 = stIspStatics.stBEAFStat.stZoneMetrics[i][j].u16h2;
				u32V1 = stIspStatics.stBEAFStat.stZoneMetrics[i][j].u16v1;
				u32V2 = stIspStatics.stBEAFStat.stZoneMetrics[i][j].u16v2;	
			}
			
			u32Fv1_n = (u32H1 * ALPHA + u32V1 * ((1<<BLEND_SHIFT) - ALPHA)) >> BLEND_SHIFT;
			u32Fv2_n = (u32H2 * BELTA + u32V2 * ((1<<BLEND_SHIFT) - BELTA)) >> BLEND_SHIFT;

			u32SumFv1 += AFWeight[i][j] * u32Fv1_n;
			u32SumFv2 += AFWeight[i][j] * u32Fv2_n;
			u32WgtSum += AFWeight[i][j];
		}
	}
	
  #if LDM_LENS
  FV[0] = u32SumFv1;
	FV[1] = u32SumFv2;
	#else //ori
	FV[0] = u32SumFv1 / u32WgtSum;
	FV[1] = u32SumFv2 / u32WgtSum;
  #endif

  return s32Ret;
}

void* af_task(void * argv)
{	
	HI_S32 s32Ret = 0;
  HI_U32 FV[2] = {0};
  HI_S32 i = 0, u32Gain = 0;

	while (af_runing)
	{
		s32Ret = af_get_vd();
		s32Ret |= af_get_value(FV);
		if (HI_SUCCESS != s32Ret)
		{
			printf("HI_MPI_ISP_GetStatistics error!(s32Ret = 0x%x)\n", s32Ret);
			sleep(5);
			continue;
		}
		
		// ISP_EXP_INFO_S
		ISP_EXP_INFO_S stExp;
		if ((i = HI_MPI_ISP_QueryExposureInfo(0, &stExp)) != HI_SUCCESS)
		{
			printf("HI_MPI_ISP_QueryExposureInfo err %0#x\n", i);
			sleep(3);
			continue;
		}

		u32Gain = stExp.u32AGain; // DGain

		if(af_ini.cb)
		{
		  af_ini.cb(FV[0], FV[1], u32Gain, af_ini.uargs);
		}
	}
	printf("%s exit.\n", __func__);
}

int sample_af_main(gsf_mpp_af_t *af)
{
  if(af)
    af_ini = *af;
  
  if(af_set_statistic(0) < 0)
  {
    printf("af_set_statistic err.\n");
    return -1;
  }
  
  if(af_ini.cb == NULL)
  {
    printf("af.cb = NULL.\n"); 
    return 0;
  }
  
  af_runing = 1;
	if (0 != pthread_create(&af_tid, 0, af_task, NULL))
  {
    printf("pthread_create af_task err.\n");
    return -1;
  }
  return 0;	
}

int sample_af_stop()
{
  if(af_runing == 1)
  {
    af_runing = 0;
    pthread_join(af_tid, 0); 
  }
  return 0;
}


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */
