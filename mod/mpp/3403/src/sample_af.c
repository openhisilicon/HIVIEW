
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
#include "hi_mpi_isp.h"
#include "hi_mpi_ae.h"
#include "sample_comm.h"

#define BLEND_SHIFT 6
#define ALPHA 64 // 1
#define BELTA 54 // 0.85
#define FEINFO 0
 
static int af_runing = 0;
static pthread_t af_tid;
static gsf_mpp_af_t af_ini = {0,};

#if 0 //maohw
static unsigned short  af_weight[HI_ISP_AE_ZONE_ROW][HI_ISP_AE_ZONE_COLUMN] =
{
  {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
  {1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1},
  {1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1},
  {1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1},
  {1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1},
  {1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1},
  {1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1},
  {1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1},
  {1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1},
  {1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1},
  {1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1},
  {1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1},
  {1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1},
  {1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1},
  {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
};
#else
static unsigned short  af_weight[HI_ISP_AE_ZONE_ROW][HI_ISP_AE_ZONE_COLUMN] =
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
#endif
int af_set_statistic(ot_vi_pipe vi_pipe)
{
	td_s32 ret = TD_SUCCESS;
  ot_isp_stats_cfg stats_cfg;
  
  ot_isp_focus_stats_cfg focus_cfg = {
    {1, 17, 15, 1, 0, {0, 0, 0, 1920, 1080}, {0, 0, 0, 1920, 1080}, 0/*OT_ISP_AF_STATS_AFTER_DGAIN*/, {0x0, 0x1, 0}, {1, 0x9bff}, 0xf0},
      
    {1, {1, 1, 1}, 15, {188, 414, -330, 486, -461, 400, -328}, {7, 0, 3, 1}, {1, 0, 255, 0, 240, 8, 14}, {127, 12, 2047}},
      
    {0, {1, 1, 0}, 2, {200, 200, -110, 461, -415, 0, 0}, {6, 0, 1, 0}, {0, 0, 0, 0, 0, 0, 0},{ 15, 12, 2047}},
    
    {{ 20, 16, 0, -16, -20}, {1, 0, 255, 0, 220, 8, 14}, {38, 12, 1800}},
    
    {{ -12, -24, 0, 24, 12}, {1, 0, 255, 0, 220, 8, 14}, {15, 12, 2047}},
    
    {4, {0, 0}, {1, 1}, 0}
  };

  ret = hi_mpi_isp_get_stats_cfg(vi_pipe, &stats_cfg);
  if (TD_SUCCESS != ret) {
    printf("get_stats_cfg error!(ret = 0x%x)\n", ret);
    return TD_FAILURE;
  }
  
  printf("%s => vi_pipe:%d, stats_cfg.key[%llx]\n", __func__, vi_pipe, stats_cfg.key);
  
  memcpy(&stats_cfg.focus_cfg, &focus_cfg, sizeof(ot_isp_focus_stats_cfg));
  ret = hi_mpi_isp_set_stats_cfg(vi_pipe, &stats_cfg);
  if (TD_SUCCESS != ret) {
    printf("set_stats_cfg error!(ret = 0x%x)\n", ret);
    return TD_FAILURE;
  }
	return TD_SUCCESS;
}

int af_get_vd(void)
{
  ot_vi_pipe vi_pipe = af_ini.ViPipe;
  td_s32 ret = hi_mpi_isp_get_vd_time_out(vi_pipe, HI_ISP_VD_FE_START, 5000);
  if(TD_SUCCESS != ret)
    printf("%s => vi_pipe:%d, get_vd_time_out ret:%d\n", __func__, vi_pipe, ret);
  return ret;
}

int af_get_value(HI_U32 FV[2])
{
  td_s32 ret = 0;
  ot_vi_pipe vi_pipe = af_ini.ViPipe;
  ot_isp_af_stats af_stats;
  ot_isp_focus_zone zone_metrics[OT_ISP_WDR_MAX_FRAME_NUM][OT_ISP_AF_ZONE_ROW][OT_ISP_AF_ZONE_COLUMN] = {0};
  
  ret = hi_mpi_isp_get_focus_stats(vi_pipe, &af_stats);
  if(TD_SUCCESS != ret)
    printf("%s => vi_pipe:%d, get_focus_stats ret:%d\n", __func__, vi_pipe, ret);
  
  #if FEINFO
  memcpy(zone_metrics, &af_stats.fe_af_stat, sizeof(ot_isp_focus_zone) * OT_ISP_WDR_MAX_FRAME_NUM * OT_ISP_AF_ZONE_ROW * OT_ISP_AF_ZONE_COLUMN);
  #else
  memcpy(zone_metrics[0], &af_stats.be_af_stat, sizeof(ot_isp_focus_zone) * OT_ISP_AF_ZONE_ROW * OT_ISP_AF_ZONE_COLUMN);
  #endif 
  
  td_u32 sum_fv1 = 0;
  td_u32 sum_fv2 = 0;
  td_u32 wgt_sum = 0;
  td_u32 fv1_n, fv2_n, fv1, fv2;
  
  int wdr_chn = 1;
  for (int k = 0; k < wdr_chn; k++) {
    for (int i = 0 ; i < HI_ISP_AE_ZONE_ROW; i++ ) {
      for (int j = 0 ; j < HI_ISP_AE_ZONE_COLUMN; j++ ) {
        td_u32 h1 = zone_metrics[k][i][j].h1;
        td_u32 h2 = zone_metrics[k][i][j].h2;
        td_u32 v1 = zone_metrics[k][i][j].v1;
        td_u32 v2 = zone_metrics[k][i][j].v2;
        fv1_n = (h1 * ALPHA + v1 * ((1 << BLEND_SHIFT) - ALPHA)) >> BLEND_SHIFT;
        fv2_n = (h2 * BELTA + v2 * ((1 << BLEND_SHIFT) - BELTA)) >> BLEND_SHIFT;
        sum_fv1 += af_weight[i][j] * fv1_n;
        sum_fv2 += af_weight[i][j] * fv2_n;
        wgt_sum += af_weight[i][j];
      }
    }
  }
  
  fv1 = sum_fv1;
  fv2 = sum_fv2;
  
  #if 0 //maohw
  fv1 /= wgt_sum;
  fv2 /= wgt_sum;
  #endif
  
  //printf("%4d %4d\n", fv1, fv2);
  
  FV[0] = fv1;
	FV[1] = fv2;
	
  return ret;
}

void* af_task(void * argv)
{	
	td_s32 ret = 0;
  td_s32 FV[2] = {0};
  td_s32 i = 0;
  ot_vi_pipe vi_pipe = af_ini.ViPipe;
  
	while (af_runing)
	{
		ret = af_get_vd();
		ret |= af_get_value(FV);
		if (TD_SUCCESS != ret)
		{
			printf("af_get_value error!(ret = 0x%x)\n", ret);
			sleep(5);
			continue;
		}

    ot_isp_exp_info exp_info = {0};
    ret = hi_mpi_isp_query_exposure_info(vi_pipe, &exp_info);
    if(ret < 0)
    {
  			printf("query_exposure_info err %0#x\n", ret);
  			sleep(3);
  			continue;
    }

		if(af_ini.cb)
		{
		  af_ini.cb(FV[0], FV[1], exp_info.a_gain, af_ini.uargs);
		}
	}
	printf("%s exit.\n", __func__);
}

int sample_af_main(gsf_mpp_af_t *af)
{
  if(af)
    af_ini = *af;
  
  if(af_set_statistic(af_ini.ViPipe) < 0)
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
