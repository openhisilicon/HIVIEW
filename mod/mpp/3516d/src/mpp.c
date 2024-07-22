#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/poll.h>
#include <sys/time.h>
#include <fcntl.h>
#include <errno.h>
#include <pthread.h>
#include <math.h>
#include <unistd.h>
#include <signal.h>
#include <sys/mman.h>
#include <sys/prctl.h>
#include <dlfcn.h>
#include <linux/fb.h>


#include "hi_type.h"
#include "hi_scene.h"
#include "hi_scene_loadparam.h"
#include "hi_scenecomm_log.h"

#include "sample_comm.h"
#include "sample_isp.h"
#include "hifb.h"
#include "mpp.h"

#include "mppex.h"

extern HI_S32 SAMPLE_VENC_VI_Init( SAMPLE_VI_CONFIG_S *pstViConfig, HI_BOOL bLowDelay, HI_U32 u32SupplementConfig);
extern HI_S32 SAMPLE_VENC_CheckSensor(SAMPLE_SNS_TYPE_E   enSnsType,SIZE_S  stSize);
extern HI_S32 SAMPLE_VENC_ModifyResolution(SAMPLE_SNS_TYPE_E   enSnsType,PIC_SIZE_E *penSize,SIZE_S *pstSize);
extern HI_S32 SAMPLE_VENC_VPSS_Init(VPSS_GRP VpssGrp, HI_BOOL* pabChnEnable, DYNAMIC_RANGE_E enDynamicRange,PIXEL_FORMAT_E enPixelFormat,SIZE_S stSize[],SAMPLE_SNS_TYPE_E enSnsType);

//from sample_comm_vdec.c;
extern VB_SOURCE_E  g_enVdecVBSource;
extern VB_POOL g_ahPicVbPool[VB_MAX_POOLS];
extern VB_POOL g_ahTmvVbPool[VB_MAX_POOLS];
extern HI_VOID SAMPLE_VDEC_HandleSig(HI_S32 signo);

//from sample_af.c;
extern int sample_af_main(gsf_mpp_af_t *af);

//from sample_ir_auto.c;
extern HI_S32 ISP_IrSwitchToIr(VI_PIPE ViPipe);
extern HI_S32 ISP_IrSwitchToNormal(VI_PIPE ViPipe);
extern HI_S32 ISP_IrSwitchAuto(VI_PIPE ViPipe);
extern HI_S32 ISP_IrMode(gsf_mpp_ir_t *ir);

//from sample_audio.c;
extern HI_S32 SAMPLE_AUDIO_AiAo(HI_VOID);


typedef struct {
  int   type;
  char* name;
  char* lib;
  char* obj;
}SAMPLE_MPP_SENSOR_T;

int SENSOR_TYPE;
int SENSOR0_TYPE;
int SENSOR1_TYPE;
SAMPLE_SNS_TYPE_E g_enSnsType[MAX_SENSOR_NUM];
ISP_SNS_OBJ_S* g_pstSnsObj[MAX_SENSOR_NUM];

static SAMPLE_MPP_SENSOR_T libsns[SAMPLE_SNS_TYPE_BUTT] = {
    {SONY_IMX327_MIPI_2M_30FPS_12BIT,           "imx327-0-0-2-30", "libsns_imx327.so", "stSnsImx327Obj"},
    {SONY_IMX327_MIPI_2M_30FPS_12BIT_WDR2TO1,   "imx327-0-1-2-30", "libsns_imx327.so", "stSnsImx327Obj"}, 
    {SONY_IMX327_2L_MIPI_2M_30FPS_12BIT,        "imx327-2-0-2-30", "libsns_imx327_2l.so", "stSnsImx327_2l_Obj"}, 
    {SONY_IMX327_2L_MIPI_2M_30FPS_12BIT_WDR2TO1,"imx327-2-1-2-30", "libsns_imx327_2l.so", "stSnsImx327_2l_Obj"}, 
    {SONY_IMX290_MIPI_2M_30FPS_12BIT,           "imx290-0-0-2-30", "libsns_imx290.so", "stSnsImx290Obj"},
    {SONY_IMX290_MIPI_2M_30FPS_12BIT_WDR2TO1,   "imx290-0-1-2-30", "libsns_imx290.so", "stSnsImx290Obj"},
    {SONY_IMX290_2L_MIPI_2M_30FPS_12BIT,        "imx290-2-0-2-30", "libsns_imx290_2l.so", "stSnsImx290_2l_Obj"}, 
    {SONY_IMX290_2L_MIPI_2M_30FPS_12BIT_WDR2TO1,"imx290-2-1-2-30", "libsns_imx290_2l.so", "stSnsImx290_2l_Obj"}, 
    {SONY_IMX307_MIPI_2M_30FPS_12BIT,           "imx307-0-0-2-30", "libsns_imx307.so", "stSnsImx307Obj"},
    {SONY_IMX307_MIPI_2M_60FPS_12BIT,           "imx307-0-0-2-50", "libsns_imx307.so", "stSnsImx307Obj"},
    {SONY_IMX307_MIPI_2M_30FPS_12BIT_WDR2TO1,   "imx307-0-1-2-30", "libsns_imx307.so", "stSnsImx307Obj"},
    {SONY_IMX307_2L_MIPI_2M_30FPS_12BIT,        "imx307-2-0-2-30", "libsns_imx307_2l.so", "stSnsImx307_2l_Obj"},
    {SONY_IMX307_2L_MIPI_2M_30FPS_12BIT_WDR2TO1,"imx307-2-1-2-30", "libsns_imx307_2l.so", "stSnsImx307_2l_Obj"},
    {SONY_IMX335_MIPI_5M_30FPS_12BIT,           "imx335-0-0-5-30", "libsns_imx335.so", "stSnsImx335Obj"},
    {SONY_IMX335_MIPI_5M_30FPS_10BIT_WDR2TO1,   "imx335-0-1-5-30", "libsns_imx335.so", "stSnsImx335Obj"},
    {SONY_IMX335_MIPI_4M_30FPS_12BIT,           "imx335-0-0-4-30", "libsns_imx335.so", "stSnsImx335Obj"},
    {SONY_IMX335_MIPI_4M_30FPS_10BIT_WDR2TO1,   "imx335-0-1-4-30", "libsns_imx335.so", "stSnsImx335Obj"},
    {SONY_IMX458_MIPI_8M_30FPS_10BIT,           "imx458-0-0-8-30",  "libsns_imx458.so", "stSnsImx458Obj"}, 
    {SONY_IMX458_MIPI_12M_20FPS_10BIT,          "imx458-0-0-12-30", "libsns_imx458.so", "stSnsImx458Obj"}, 
    {SONY_IMX458_MIPI_4M_60FPS_10BIT,           "imx458-0-0-4-60",  "libsns_imx458.so", "stSnsImx458Obj"}, 
    {SONY_IMX458_MIPI_4M_40FPS_10BIT,           "imx458-0-0-4-40",  "libsns_imx458.so", "stSnsImx458Obj"}, 
    {SONY_IMX458_MIPI_2M_90FPS_10BIT,           "imx458-0-0-2-90",  "libsns_imx458.so", "stSnsImx458Obj"}, 
    {SONY_IMX458_MIPI_1M_129FPS_10BIT,          "imx458-0-0-1-129",  "libsns_imx458.so", "stSnsImx458Obj"}, 
    {PANASONIC_MN34220_LVDS_2M_30FPS_12BIT,     "mn34220-0-0-2-30",   "libsns_mn34220.so", "stSnsMn34220Obj"},
    {OMNIVISION_OS04B10_MIPI_4M_30FPS_10BIT,      "os04b10-0-0-4-30", "libsns_os04b10.so", "stSnsOs04b10_2lObj"},
    {OMNIVISION_OS05A_MIPI_4M_30FPS_12BIT,        "os05a-0-0-4-30", "libsns_os05a.so", "stSnsOs05aObj"},
    {OMNIVISION_OS05A_MIPI_4M_30FPS_10BIT_WDR2TO1,"os05a-0-1-4-30", "libsns_os05a.so", "stSnsOs05aObj"},
    {OMNIVISION_OS08A10_MIPI_8M_30FPS_10BIT,      "os08a10-0-0-8-30", "libsns_os08a10.so", "stSnsOS08A10Obj"},
    {GALAXYCORE_GC2053_MIPI_2M_30FPS_10BIT,       "gc2053-0-0-2-30", "libsns_gc2053.so", "stSnsGc2053Obj"},
    {SMART_SC4210_MIPI_3M_30FPS_12BIT,            "sc4210-0-0-3-30", "libsns_sc4210.so", "stSnsSc4210Obj"},
    {SMART_SC4210_MIPI_3M_30FPS_10BIT_WDR2TO1,    "sc4210-0-1-3-30", "libsns_sc4210.so", "stSnsSc4210Obj"},
    {OMNIVISION_OV12870_MIPI_1M_240FPS_10BIT,     "ov12870-0-0-1-240", "libsns_ov12870.so", "stSnsOv12870Obj"},
    {OMNIVISION_OV12870_MIPI_2M_120FPS_10BIT,     "ov12870-0-0-2-120", "libsns_ov12870.so", "stSnsOv12870Obj"},
    {OMNIVISION_OV12870_MIPI_8M_30FPS_10BIT,      "ov12870-0-0-8-30", "libsns_ov12870.so", "stSnsOv12870Obj"},
    {OMNIVISION_OV12870_MIPI_12M_30FPS_10BIT,     "ov12870-0-0-12-30", "libsns_ov12870.so", "stSnsOv12870Obj"},
    {SONY_IMX415_MIPI_8M_30FPS_12BIT,             "imx415-0-0-8-30", "libsns_imx415.so", "stSnsImx415Obj"},
    {SONY_IMX415_MIPI_8M_20FPS_12BIT,             "imx415-0-0-8-20", "libsns_imx415.so", "stSnsImx415Obj"},
    {SONY_IMX415_MIPI_2M_60FPS_12BIT,             "imx415-0-0-2-60", "libsns_imx415.so", "stSnsImx415Obj"},
    {MIPI_YUV_1M_60FPS_8BIT,                      "yuv422-0-0-1-60", NULL,                NULL},
    {MIPI_YUV_2M_60FPS_8BIT,                      "yuv422-0-0-2-60", NULL,                NULL},
    {MIPI_YUV_8M_30FPS_8BIT,                      "yuv422-0-0-8-30", NULL,                NULL},
    {MIPI_YUVPKG_2M_60FPS_8BIT,                   "pkg422-0-0-2-60", NULL,                NULL},
    {BT1120_YUV_2M_60FPS_8BIT,                    "bt1120-0-0-2-60", NULL,                NULL},
    {BT1120_YUV_1M_60FPS_8BIT,                    "bt1120-0-0-1-60", NULL,                NULL},
    {SONY_IMX385_MIPI_2M_30FPS_12BIT,           "imx385-0-0-2-30", "libsns_imx385.so", "stSnsImx385Obj"},
    {SONY_IMX482_MIPI_2M_30FPS_12BIT,           "imx482-0-0-2-30", "libsns_imx482.so", "stSnsImx482Obj"},
    {OV426_OV6946_DC_0M_30FPS_12BIT,            "ov426-0-0-0-30", "libsns_ov426.so", "stSnsOv426Obj"},
    {SONY_IMX585_MIPI_2M_30FPS_12BIT,           "imx585-0-0-2-30", "libsns_imx585.so", "stSnsImx585Obj"},
    {SONY_IMX327_MIPI_2M_60FPS_12BIT,           "imx327-0-0-2-60", "libsns_imx327.so", "stSnsImx327Obj"},
  };


static SAMPLE_MPP_SENSOR_T* SAMPLE_MPP_SERSOR_GET(char* name)
{
  int i = 0;
  for(i = 0; i < SAMPLE_SNS_TYPE_BUTT; i++)
  {
    //printf("libsns[%d].name:%s, name:%s\n", i, libsns[i].name, name);
    if(strstr(libsns[i].name, name))
    {
      return &libsns[i];
    }
  }
  return NULL;
}


ISP_SNS_OBJ_S* SAMPLE_COMM_ISP_GetSnsObj(HI_U32 u32SnsId)
{
    SAMPLE_SNS_TYPE_E enSnsType;

    enSnsType = g_enSnsType[u32SnsId];
    return g_pstSnsObj[u32SnsId];
}

static void * dl = NULL;
static int snscnt = 0, wdr_mode = 0;

#include <signal.h>
void SAMPLE_VENC_HandleSig2(HI_S32 signo)
{
    signal(SIGINT, SIG_IGN);
    signal(SIGTERM, SIG_IGN);

    if (SIGINT == signo || SIGTERM == signo)
    {
        mppex_hook_destroy();
        gsf_mpp_scene_stop();
        SAMPLE_COMM_VENC_StopGetStream();
        SAMPLE_COMM_AUDIO_DestoryAllTrd();
        SAMPLE_COMM_All_ISP_Stop();
        SAMPLE_COMM_VO_HdmiStop();
        SAMPLE_COMM_SYS_Exit();
    }
    exit(-1);
}

int gsf_mpp_cfg_sns(char *path, gsf_mpp_cfg_t *cfg)
{
  int i = 0;
  char snsstr[64];
  sprintf(snsstr, "%s-%d-%d-%d-%d"
          , cfg->snsname, cfg->lane, cfg->wdr, cfg->res, cfg->fps);

  mppex_hook_sns(cfg);
  
  SAMPLE_MPP_SENSOR_T* sns = SAMPLE_MPP_SERSOR_GET(snsstr);
  if(!sns)
  {
    printf("%s => snsstr:%s, unknow.\n", __func__, snsstr);
    return -1;
  } 
  
  snscnt = cfg->snscnt;
  wdr_mode = cfg->wdr;
  char loadstr[256];
  if(strstr(cfg->snsname, "bt1120") || strstr(cfg->snsname, "ov426"))
    sprintf(loadstr, "%s/ko/load3516dv300 -i -yuv0 1", path); 
  else if(strstr(cfg->snsname, "imx385") || strstr(cfg->snsname, "imx482") || strstr(cfg->snsname, "imx585")) // 37.125Mclk && i2c;
    sprintf(loadstr, "%s/ko/load3516dv300 -i -sensor0 %s", path, "imx327");
  else
    sprintf(loadstr, "%s/ko/load3516dv300 -i -sensor0 %s", path, cfg->snsname);

  for(i = 1; i < snscnt; i++)
  {
    sprintf(loadstr, "%s -sensor%d %s", loadstr, i, cfg->snsname);
  }
  
  if(cfg->second && cfg->snscnt == 1 && !strstr(cfg->snsname, "bt1120"))
  {
    sprintf(loadstr, "%s -yuv0 2", loadstr);
  }
  
  printf("%s => loadstr: %s\n", __func__, loadstr);
  system(loadstr);
  
  SENSOR_TYPE = SENSOR0_TYPE = SENSOR1_TYPE = sns->type;
  
  if(cfg->second && cfg->snscnt == 1)
  {
    SENSOR1_TYPE = (cfg->second == SECOND_BT656_PAL)?BT656_YUV_0M_60FPS_8BIT: //PAL
                   (cfg->second == SECOND_BT656_NTSC)?BT656N_YUV_0M_60FPS_8BIT://NTSC
                   (cfg->second == SECOND_BT656_512P)?BT656_YUV_512P_60FPS_8BIT://640x512
                   (cfg->second == SECOND_BT656_288P)?BT656_YUV_288P_60FPS_8BIT://384x288
                                                      BT656_YUV_600P_60FPS_8BIT;//800x600
  }
  
  if(dl)
  {
    dlclose(dl);
    dlerror();
  }
  
  if(sns->lib)
  {
    dl = dlopen(sns->lib, RTLD_LAZY);
    if(dl == NULL)
    {
      printf("err dlopen %s\n", sns->lib);
      goto __err;
    }
    
    g_pstSnsObj[0] = g_pstSnsObj[1] = dlsym(dl, sns->obj);
    if(NULL != dlerror())
    {
        printf("err dlsym %s\n", sns->obj);
        goto __err;
    }
  }
  printf("%s => snsstr:%s, sensor_type:%d, load:%s\n"
        , __func__, snsstr, SENSOR_TYPE, sns->lib?:"");
  
  signal(SIGINT, SAMPLE_VENC_HandleSig2);
  signal(SIGTERM, SAMPLE_VENC_HandleSig2);
  
  return 0;
__err:
	if(dl)
	{
    dlclose(dl);
  }
  dlerror();
  return -1;
}

int gsf_mpp_cfg_vdec(char *path, gsf_mpp_cfg_t *cfg);

int gsf_mpp_cfg(char *path, gsf_mpp_cfg_t *cfg)
{
  //extern HI_S32 cipher_check(void);
  //cipher_check();
  
  mppex_hook_register();
  	
  if(cfg && cfg->snscnt > 0)
    return gsf_mpp_cfg_sns(path, cfg);
  else  
    return gsf_mpp_cfg_vdec(path, cfg);
}



int gsf_mpp_vi_start(gsf_mpp_vi_t *vi)
{
  HI_S32 s32Ret;
  VI_DEV  ViDev = 0;
  VI_PIPE ViPipe = 0;
  VI_CHN  ViChn = 0;
  SAMPLE_VI_CONFIG_S stViConfig;
  memset(&stViConfig, 0, sizeof(stViConfig));
  SAMPLE_COMM_VI_GetSensorInfo(&stViConfig);
  
  stViConfig.s32WorkingViNum       = snscnt;
  // SAMPLE_COMM_VI_SetMipiHsMode(LANE_DIVIDE_MODE_0/LANE_DIVIDE_MODE_1)
  // Mode  DEV0   DEV1  DEV2  DEV3  DEV4   DEV5  DEV6   DEV7
  //  0    L0~L3   N     N     N      N      N     N     N
  //  1    L0L2    L1L3  N     N      N      N     N     N
  int i = 0;
  for(i = 0; i < snscnt; i++)
  {
    stViConfig.astViInfo[i].stDevInfo.ViDev     = i;//ViDev 0, 1, 2, 3;
    stViConfig.astViInfo[i].stPipeInfo.aPipe[0] = i;//ViPipe 0, 1, 2, 3;
    stViConfig.astViInfo[i].stChnInfo.ViChn     = 0;//ViChn  0, 0, 0, 0;
    stViConfig.astViInfo[i].stChnInfo.enDynamicRange = DYNAMIC_RANGE_SDR8;
    stViConfig.astViInfo[i].stChnInfo.enPixFormat    = PIXEL_FORMAT_YVU_SEMIPLANAR_420;//422
    stViConfig.astViInfo[i].stDevInfo.enWDRMode = wdr_mode?WDR_MODE_2To1_LINE:0;
  }

  mppex_hook_vi(&stViConfig, vi->bLowDelay);

  s32Ret = SAMPLE_VENC_VI_Init(&stViConfig, vi->bLowDelay,vi->u32SupplementConfig);
  if(s32Ret != HI_SUCCESS)
  {
    SAMPLE_PRT("Init VI err for %#x!\n", s32Ret);
    return HI_FAILURE;
  }
  
  return s32Ret;
}

int gsf_mpp_vi_stop()
{
  HI_S32 s32Ret;
  
  SAMPLE_VI_CONFIG_S stViConfig;
  memset(&stViConfig, 0, sizeof(stViConfig));
  SAMPLE_COMM_VI_GetSensorInfo(&stViConfig);
  SAMPLE_COMM_VI_StopVi(&stViConfig);
  
  SAMPLE_COMM_SYS_Exit();
  return s32Ret;
}

int gsf_mpp_vpss_send(int VpssGrp, int VpssGrpPipe, VIDEO_FRAME_INFO_S *pstVideoFrame , int s32MilliSec)
{
  return HI_MPI_VPSS_SendFrame(VpssGrp, VpssGrpPipe, pstVideoFrame, s32MilliSec);
}


int gsf_mpp_uvc_get(int ViPipe, int ViChn, VIDEO_FRAME_INFO_S *pstFrameInfo, int s32MilliSec)
{
  return mppex_uvc_get(ViPipe, pstFrameInfo, s32MilliSec);
}
int gsf_mpp_uvc_release(int ViPipe, int ViChn, VIDEO_FRAME_INFO_S *pstFrameInfo)
{
  return mppex_uvc_rel(ViPipe, pstFrameInfo);
}


int gsf_mpp_af_start(gsf_mpp_af_t *af)
{
  return sample_af_main(af);
}



int gsf_mpp_vpss_start(gsf_mpp_vpss_t *vpss)
{
  HI_S32 i, s32Ret;
  SIZE_S stSize[VPSS_MAX_PHY_CHN_NUM];

  SAMPLE_VI_CONFIG_S stViConfig;
  memset(&stViConfig, 0, sizeof(stViConfig));
  SAMPLE_COMM_VI_GetSensorInfo(&stViConfig);
 
  mppex_hook_vpss_bb(vpss);
  
  for(i = 0; i < VPSS_MAX_PHY_CHN_NUM; i++)
  {
      if(!vpss->enable[i])
      {
        continue;
      }
      
      s32Ret = SAMPLE_COMM_SYS_GetPicSize(vpss->enSize[i], &stSize[i]);
      if (HI_SUCCESS != s32Ret)
      {
          SAMPLE_PRT("SAMPLE_COMM_SYS_GetPicSize failed!\n");
          return s32Ret;
      }
  }

  s32Ret = SAMPLE_VENC_CheckSensor(stViConfig.astViInfo[0].stSnsInfo.enSnsType,stSize[0]);
  if(s32Ret != HI_SUCCESS)
  {
      s32Ret = SAMPLE_VENC_ModifyResolution(stViConfig.astViInfo[0].stSnsInfo.enSnsType,&vpss->enSize[0],&stSize[0]);
      if(s32Ret != HI_SUCCESS)
      {
          return HI_FAILURE;
      }
  }

  s32Ret = SAMPLE_VENC_VPSS_Init(vpss->VpssGrp,
                                vpss->enable,
                                DYNAMIC_RANGE_SDR8,
                                PIXEL_FORMAT_YVU_SEMIPLANAR_420,
                                stSize,
                                stViConfig.astViInfo[0].stSnsInfo.enSnsType);
  if (HI_SUCCESS != s32Ret)
  {
      SAMPLE_PRT("Init VPSS err for %#x!\n", s32Ret);
      goto EXIT_VI_STOP;
  }

  if(vpss->ViPipe >= 0)
  {
    s32Ret = SAMPLE_COMM_VI_Bind_VPSS(vpss->ViPipe, vpss->ViChn, vpss->VpssGrp);
    if(s32Ret != HI_SUCCESS)
    {
        SAMPLE_PRT("VI Bind VPSS err for %#x!\n", s32Ret);
        goto EXIT_VPSS_STOP;
    }
  }
  
  mppex_hook_vpss_ee(vpss);

  
  return s32Ret;
  
EXIT_VPSS_STOP:
    SAMPLE_COMM_VPSS_Stop(vpss->VpssGrp,vpss->enable);
EXIT_VI_STOP:
    //SAMPLE_COMM_VI_StopVi(&stViConfig);
  return s32Ret;
}

int gsf_mpp_vpss_stop(gsf_mpp_vpss_t *vpss)
{
  HI_S32 s32Ret = 0;
  

  
  //SAMPLE_VI_CONFIG_S stViConfig;
  //memset(&stViConfig, 0, sizeof(stViConfig));
  //SAMPLE_COMM_VI_GetSensorInfo(&stViConfig);

  if(vpss->ViPipe >= 0)
    SAMPLE_COMM_VI_UnBind_VPSS(vpss->ViPipe,vpss->ViChn,vpss->VpssGrp);
  
  SAMPLE_COMM_VPSS_Stop(vpss->VpssGrp,vpss->enable);
  //SAMPLE_COMM_VI_StopVi(&stViConfig);
  return s32Ret;
}


int gsf_mpp_vpss_ctl(int VpssGrp, int id, void *args)
{
  int ret = -1;
  switch(id)
  {
    case GSF_MPP_VPSS_CTL_PAUSE:
      ret = HI_MPI_VPSS_StopGrp(VpssGrp);
      if(ret)
        printf("GSF_MPP_VPSS_CTL_PAUSE VpssGrp:%d, err 0x%x\n", VpssGrp, ret);    
      break;
    case GSF_MPP_VPSS_CTL_RESUM:  
      ret = HI_MPI_VPSS_StartGrp(VpssGrp);
      if(ret)  
        printf("GSF_MPP_VPSS_CTL_RESUM VpssGrp:%d, err 0x%x\n", VpssGrp, ret);
      break;
    case GSF_MPP_VPSS_CTL_CROP:
      ret = HI_MPI_VPSS_SetGrpCrop(VpssGrp, (VPSS_CROP_INFO_S*)args);
      if(ret)
        printf("GSF_MPP_VPSS_CTL_CROP VpssGrp:%d, err 0x%x\n", VpssGrp, ret);
      break;
    case GSF_MPP_VPSS_CTL_ASPECT:
      {
        int i = 0;
        VPSS_CHN_ATTR_S stChnAttr;
    
        for(i = 0; i < 2; i++)
        {
          if(HI_MPI_VPSS_GetChnAttr(VpssGrp, i, &stChnAttr) == HI_SUCCESS)
          {
            stChnAttr.stAspectRatio = *((ASPECT_RATIO_S*)args);
            ret = HI_MPI_VPSS_SetChnAttr(VpssGrp, i, &stChnAttr);
            if(ret)
              printf("GSF_MPP_VPSS_CTL_ASPECT VpssGrp:%d,VpssChn:%d err 0x%x\n", VpssGrp, i, ret);
          }
        }
      }
      break;
    case  GSF_MPP_VPCH_CTL_ENABLE:
      ret = HI_MPI_VPSS_EnableChn(VpssGrp, (int)args);
      if(ret)
        printf("GSF_MPP_VPCH_CTL_ENABLE VpssGrp:%d,VpssChn:%d err 0x%x\n", VpssGrp, (int)args, ret); 
      break;
    case GSF_MPP_VPCH_CTL_DISABLE:
      ret = HI_MPI_VPSS_DisableChn(VpssGrp, (int)args);
      if(ret)
        printf("GSF_MPP_VPCH_CTL_DISABLE VpssGrp:%d,VpssChn:%d err 0x%x\n", VpssGrp, (int)args, ret); 
      break;
    case GSF_MPP_VPSS_CTL_ATTR:
      ret = HI_MPI_VPSS_GetGrpAttr(VpssGrp, (VPSS_GRP_ATTR_S*)args);
      if(ret)
        printf("GSF_MPP_VPSS_CTL_ATTR VpssGrp:%d, err 0x%x\n", VpssGrp, ret); 
      break;
  }
  return ret;
}


int gsf_mpp_venc_start(gsf_mpp_venc_t *venc)
{
  HI_S32 s32Ret;
  VENC_GOP_ATTR_S stGopAttr;
  
  mppex_hook_venc_bb(venc);
  
  s32Ret = SAMPLE_COMM_VENC_GetGopAttr(venc->enGopMode,&stGopAttr);
  if (HI_SUCCESS != s32Ret)
  {
      SAMPLE_PRT("Venc Get GopAttr for %#x!\n", s32Ret);
      goto EXIT_VI_VPSS_UNBIND;
  }

  SAMPLE_COMM_VENC_CreatAttr(venc->VencChn, venc->u32FrameRate, venc->u32Gop, venc->u32BitRate);

  s32Ret = SAMPLE_COMM_VENC_Start(venc->VencChn, 
                                venc->enPayLoad, 
                                venc->enSize, 
                                venc->enRcMode,
                                venc->u32Profile,
                                venc->bRcnRefShareBuf,
                                &stGopAttr);
  if (HI_SUCCESS != s32Ret)
  {
      SAMPLE_PRT("Venc Start failed for %#x!\n", s32Ret);
      goto EXIT_VI_VPSS_UNBIND;
  }

  if(venc->VpssGrp >= 0)
  {
    s32Ret = SAMPLE_COMM_VPSS_Bind_VENC(venc->VpssGrp, venc->VpssChn,venc->VencChn);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("Venc Get GopAttr failed for %#x!\n", s32Ret);
        goto EXIT_VENC_STOP;
    }
  }

  mppex_hook_venc_ee(venc);
  return s32Ret;

EXIT_VENC_UnBind:
    if(venc->VpssGrp >= 0)
      SAMPLE_COMM_VPSS_UnBind_VENC(venc->VpssGrp,venc->VpssChn,venc->VencChn);
EXIT_VENC_STOP:
    SAMPLE_COMM_VENC_Stop(venc->VencChn);
EXIT_VI_VPSS_UNBIND:
  return s32Ret;
}


int gsf_mpp_venc_stop(gsf_mpp_venc_t *venc)
{
  HI_S32 s32Ret;
  if(venc->VpssGrp >= 0)
    SAMPLE_COMM_VPSS_UnBind_VENC(venc->VpssGrp,venc->VpssChn,venc->VencChn);
  SAMPLE_COMM_VENC_Stop(venc->VencChn);
  return s32Ret;
}

int gsf_mpp_venc_send(int VeChn, VIDEO_FRAME_INFO_S *pstFrame, int s32MilliSec, gsf_mpp_venc_get_t *get)
{
  int ret = 0;
  
  if(!pstFrame)
    return -1;
    
  /*
  设置编码器属性中通道宽高属性时,需要首先停止编码通道接收输入图像;
  通道宽高设置后通道的优先级并不会恢复默认,
  编码通道的其它所有参数配置恢复默认值,关闭OSD,并且清空码流buffer和缓存图像队列;
  */

  VENC_CHN_ATTR_S stChnAttr;
  
  ret = HI_MPI_VENC_GetChnAttr(VeChn, &stChnAttr);
  if(ret != 0)
    return -1;

  if(pstFrame->stVFrame.u32Width > stChnAttr.stVencAttr.u32MaxPicWidth
    || pstFrame->stVFrame.u32Height > stChnAttr.stVencAttr.u32MaxPicHeight)
    return -1;
   
  if(get && get->cb)
  {
    stChnAttr.stVencAttr.u32PicWidth = pstFrame->stVFrame.u32Width;
    stChnAttr.stVencAttr.u32PicHeight = pstFrame->stVFrame.u32Height;
    ret = HI_MPI_VENC_SetChnAttr(VeChn, &stChnAttr);
    
    VENC_RECV_PIC_PARAM_S  stRecvParam;
    stRecvParam.s32RecvPicNum = 1;
    ret = HI_MPI_VENC_StartRecvFrame(VeChn, &stRecvParam);
 
    ret = HI_MPI_VENC_SendFrame(VeChn, pstFrame, s32MilliSec);
 
    VENC_STREAM_S stStream;
    VENC_PACK_S stPack[4];
    stStream.u32PackCount = 4;
    stStream.pstPack = &stPack;
    
    ret = HI_MPI_VENC_GetStream(VeChn, &stStream, s32MilliSec);
    if(ret == 0)
    {
      get->cb(&stStream, get->u);
      ret = HI_MPI_VENC_ReleaseStream(VeChn, &stStream);
    }
    
    ret = HI_MPI_VENC_StopRecvFrame(VeChn);
    return ret;
  }
    
  if(pstFrame->stVFrame.u32Width != stChnAttr.stVencAttr.u32PicWidth
    || pstFrame->stVFrame.u32Height != stChnAttr.stVencAttr.u32PicHeight)
  {
    ret = HI_MPI_VENC_StopRecvFrame(VeChn);
    
    stChnAttr.stVencAttr.u32PicWidth = pstFrame->stVFrame.u32Width;
    stChnAttr.stVencAttr.u32PicHeight = pstFrame->stVFrame.u32Height;
    ret = HI_MPI_VENC_SetChnAttr(VeChn, &stChnAttr);
    
    VENC_RECV_PIC_PARAM_S  stRecvParam;
    stRecvParam.s32RecvPicNum = -1;
    ret = HI_MPI_VENC_StartRecvFrame(VeChn, &stRecvParam);
  }
  ret = HI_MPI_VENC_SendFrame(VeChn, pstFrame, s32MilliSec);
  return ret;
}


int gsf_mpp_venc_recv(gsf_mpp_recv_t *recv)
{
  HI_S32 s32Ret;
  s32Ret = SAMPLE_COMM_VENC_StartGetStreamCb(recv->VeChn,recv->s32Cnt, recv->cb, recv->uargs);
  return s32Ret;
}

int gsf_mpp_venc_dest()
{
  SAMPLE_COMM_VENC_StopGetStream();
  return 0;  
}

int gsf_mpp_venc_snap(VENC_CHN VencChn, HI_U32 SnapCnt, int(*cb)(int i, VENC_STREAM_S* pstStream, void* u), void* u)
{
  return SAMPLE_COMM_VENC_SnapProcessCB(VencChn, SnapCnt, cb, u);
}

static int g_scenebEnable = 0;

int gsf_mpp_scene_start(char *path, int scenemode)
{
    HI_S32 s32ret = HI_SUCCESS;
    HI_SCENE_PARAM_S stSceneParam;
    HI_SCENE_VIDEO_MODE_S stVideoMode;
    
    s32ret = HI_SCENE_CreateParam(path, &stSceneParam, &stVideoMode);
    if (HI_SUCCESS != s32ret)
    {
        printf("SCENETOOL_CreateParam failed\n");
        return HI_FAILURE;
    }
    s32ret = HI_SCENE_Init(&stSceneParam);
    if (HI_SUCCESS != s32ret)
    {
        printf("HI_SCENE_Init failed\n");
        return HI_FAILURE;
    }

    s32ret = HI_SCENE_SetSceneMode(&(stVideoMode.astVideoMode[scenemode]));
    if (HI_SUCCESS != s32ret)
    {
        printf("HI_SRDK_SCENEAUTO_Start failed\n");
        return HI_FAILURE;
    }
    printf("The sceneauto is started.\n");
    g_scenebEnable = 1;
    return s32ret;
}
int gsf_mpp_scene_stop()
{
  HI_S32 s32ret = HI_SUCCESS;
  s32ret = HI_SCENE_Deinit();
  if (HI_SUCCESS != s32ret)
  {
      printf("HI_SCENE_Deinit failed\n");
      return HI_FAILURE;
  }
  return s32ret;
}

extern HI_SCENE_CTL_AE_S g_scene_ctl_ae[4];

int gsf_mpp_scene_ctl(int ViPipe, int id, void *args)
{
  int ret = -1;
  
  if(ViPipe < 0 || ViPipe >= 4)
    return ret;  

  switch(id)
  {
    case GSF_MPP_SCENE_CTL_ALL:
      {
        ret = 0;
        gsf_mpp_scene_all_t *all = (gsf_mpp_scene_all_t*)args;
        all->ae.compensation_mul = g_scene_ctl_ae[ViPipe].compensation_mul;
      }
      break;
    case GSF_MPP_SCENE_CTL_AE:
      {
        ret = 0;
        HI_SCENE_CTL_AE_S *ae = (HI_SCENE_CTL_AE_S*)args;
        
        g_scene_ctl_ae[ViPipe].compensation_mul = ae->compensation_mul;
        printf("g_scene_ctl_ae[%d]: %0.2f\n", ViPipe, g_scene_ctl_ae[ViPipe].compensation_mul);
      }
      break;
  }
  return ret;
}

int gsf_mpp_venc_ctl(int VencChn, int id, void *args)
{
  int ret = -1;
  switch(id)
  {
    case GSF_MPP_VENC_CTL_IDR:
      ret = HI_MPI_VENC_RequestIDR(VencChn, HI_TRUE);
      break;
    case GSF_MPP_VENC_CTL_RST:
      ret = HI_MPI_VENC_StopRecvFrame(VencChn);
      if (ret != HI_SUCCESS)
      {
        printf("HI_MPI_VENC_StopRecvFrame err 0x%x\n",ret);
        return HI_FAILURE;
      }
      ret = HI_MPI_VENC_ResetChn(VencChn);
      if (ret != HI_SUCCESS)
      {
        printf("HI_MPI_VENC_ResetChn err 0x%x\n",ret);
        return HI_FAILURE;
      }
      VENC_RECV_PIC_PARAM_S  stRecvParam;
      stRecvParam.s32RecvPicNum = -1;
      ret = HI_MPI_VENC_StartRecvFrame(VencChn,&stRecvParam);
      if (ret != HI_SUCCESS)
      {
        printf("HI_MPI_VENC_StartRecvFrame err 0x%x\n",ret);
        return HI_FAILURE;
      }
      break;
    default:
      break;
  }
  printf("%s => VencChn:%d, id:%d, ret:%d\n", __func__, VencChn, id, ret);
  return ret;
}

int gsf_mpp_isp_ctl(int ViPipe, int id, void *args)
{
  int ret = -1;
  switch(id)
  {
    case GSF_MPP_ISP_CTL_IR:
      switch((int)args)
      {
        case 0:
          ISP_IrMode((gsf_mpp_ir_t*)NULL);
          ret = ISP_IrSwitchToNormal(ViPipe);
          break;
        case 1:
          ISP_IrMode((gsf_mpp_ir_t*)NULL);
          ret = ISP_IrSwitchToIr(ViPipe);
          break;
        default:
          ISP_IrMode((gsf_mpp_ir_t*)args);
          ret = ISP_IrSwitchAuto(ViPipe);
          break;
      }
      break;
    case GSF_MPP_ISP_CTL_IMG:
      {
        gsf_mpp_img_all_t *all = (gsf_mpp_img_all_t*)args;
        
        all->scene.bEnable = g_scenebEnable;
        
        ISP_CSC_ATTR_S stCSCFAttr;
        ret = HI_MPI_ISP_GetCSCAttr(ViPipe, &stCSCFAttr);
        
        all->csc.u8Hue  =  stCSCFAttr.u8Hue;
        all->csc.u8Luma =  stCSCFAttr.u8Luma;
        all->csc.u8Contr =  stCSCFAttr.u8Contr;
        all->csc.u8Satu =  stCSCFAttr.u8Satu;
        
        ISP_EXPOSURE_ATTR_S stExpAttr;
        HI_MPI_ISP_GetExposureAttr(ViPipe, &stExpAttr);

        all->ae.u8Speed = stExpAttr.stAuto.u8Speed;
        all->ae.u8Compensation = stExpAttr.stAuto.u8Compensation;
        all->ae.SysGainRangeMax = stExpAttr.stAuto.stSysGainRange.u32Max;
        all->ae.SysGainRangeMin = stExpAttr.stAuto.stSysGainRange.u32Min;
        all->ae.ExpTimeRangeMax = stExpAttr.stAuto.stExpTimeRange.u32Max;
        all->ae.ExpTimeRangeMin = stExpAttr.stAuto.stExpTimeRange.u32Min;
        
        
        ISP_DEHAZE_ATTR_S stDehazeAttr;
        HI_MPI_ISP_GetDehazeAttr(ViPipe, &stDehazeAttr);
        
        all->dehaze.u8strength = stDehazeAttr.stAuto.u8strength;
        
        ISP_SHARPEN_ATTR_S stIspShpAttr;
        ret = HI_MPI_ISP_GetIspSharpenAttr(ViPipe, &stIspShpAttr);
        
        all->sharpen.u16TextureFreq = stIspShpAttr.stManual.u16TextureFreq;
        all->sharpen.u16EdgeFreq = stIspShpAttr.stManual.u16EdgeFreq;
        all->sharpen.u8DetailCtrl = stIspShpAttr.stManual.u8DetailCtrl;
        
        ISP_HLC_ATTR_S stIspHlcAttr;
        ret = HI_MPI_ISP_GetIspHlcAttr(ViPipe, &stIspHlcAttr);
        
        all->hlc.u8LumaThr= stIspHlcAttr.u8LumaThr;
        all->hlc.u8LumaTarget = stIspHlcAttr.u8LumaTarget;
        
        ISP_GAMMA_ATTR_S stGammaAttr;
        ret = HI_MPI_ISP_GetGammaAttr(ViPipe, &stGammaAttr);
        
        all->gamma.enCurveType = stGammaAttr.enCurveType;
        
        ISP_DRC_ATTR_S stDRC;
        ret = HI_MPI_ISP_GetDRCAttr(ViPipe, &stDRC);
        
        all->drc.u16Strength = stDRC.stAuto.u16Strength;
        all->drc.u16StrengthMax = stDRC.stAuto.u16StrengthMax;
        all->drc.u16StrengthMin = stDRC.stAuto.u16StrengthMin;
        
        ISP_LDCI_ATTR_S stLDCIAttr;
        ret = HI_MPI_ISP_GetLDCIAttr(ViPipe, &stLDCIAttr);
        
        all->ldci.u16BlcCtrl = stLDCIAttr.stManual.u16BlcCtrl;
        all->ldci.stHePosWgt_u8Wgt = stLDCIAttr.stManual.stHeWgt.stHePosWgt.u8Wgt;
        all->ldci.stHeNegWgt_u8Mean = stLDCIAttr.stManual.stHeWgt.stHeNegWgt.u8Mean;
        
        VPSS_GRP VpssGrp = 0;        
        VPSS_GRP_ATTR_S stGrpAttr = {0};
        ret = HI_MPI_VPSS_GetGrpAttr(VpssGrp, &stGrpAttr);
        
        VI_LDC_ATTR_S stLDCAttr = {0};
        ret = HI_MPI_VI_GetChnLDCAttr(ViPipe, 0, &stLDCAttr);
        all->ldc.bEnable = stLDCAttr.bEnable;
        all->ldc.s32DistortionRatio = stLDCAttr.stAttr.s32DistortionRatio;
      }
      break;
    case GSF_MPP_ISP_CTL_CSC:
      {
        gsf_mpp_img_csc_t *csc = (gsf_mpp_img_csc_t*)args;
        if(!csc->bEnable)
        {
          ret = 0;  
          break;
        }
        ISP_CSC_ATTR_S stCSCFAttr;
        ret = HI_MPI_ISP_GetCSCAttr(ViPipe, &stCSCFAttr);
        
        stCSCFAttr.bEnable = 1;
        stCSCFAttr.u8Hue =    csc->u8Hue;
        stCSCFAttr.u8Luma =   csc->u8Luma;
        stCSCFAttr.u8Contr =  csc->u8Contr;
        stCSCFAttr.u8Satu =   csc->u8Satu;
        ret = HI_MPI_ISP_SetCSCAttr(ViPipe, &stCSCFAttr);
      }
      break;
    case GSF_MPP_ISP_CTL_AE:
      {
        gsf_mpp_img_ae_t *ae = (gsf_mpp_img_ae_t*)args;
        if(!ae->bEnable)
        {
          ret = 0;  
          break;
        }

        ISP_EXPOSURE_ATTR_S stExpAttr;
        ret = HI_MPI_ISP_GetExposureAttr(ViPipe, &stExpAttr);

        stExpAttr.bByPass = 0;
        stExpAttr.stAuto.u8Speed = ae->u8Speed;
        stExpAttr.stAuto.u8Compensation        = ae->u8Compensation;
        stExpAttr.stAuto.stSysGainRange.u32Max = ae->SysGainRangeMax;
        stExpAttr.stAuto.stSysGainRange.u32Min = ae->SysGainRangeMin;
        stExpAttr.stAuto.stExpTimeRange.u32Max = ae->ExpTimeRangeMax;
        stExpAttr.stAuto.stExpTimeRange.u32Min = ae->ExpTimeRangeMin;
        
        ret = HI_MPI_ISP_SetExposureAttr(ViPipe, &stExpAttr);
      }
      break;
      case GSF_MPP_ISP_CTL_DEHAZE:
      {
        gsf_mpp_img_dehaze_t *dehaze = (gsf_mpp_img_dehaze_t*)args;
        if(!dehaze->bEnable)
        {
          ret = 0;  
          break;
        }
          
        ISP_DEHAZE_ATTR_S stDehazeAttr;
        ret = HI_MPI_ISP_GetDehazeAttr(ViPipe, &stDehazeAttr);
        
        stDehazeAttr.bEnable = 1;
        stDehazeAttr.stAuto.u8strength = dehaze->u8strength;
        
        ret = HI_MPI_ISP_SetDehazeAttr(ViPipe, &stDehazeAttr);
      }
      break;
      
     case GSF_MPP_ISP_CTL_SCENE:
     {
        gsf_mpp_img_scene_t *scene = (gsf_mpp_img_scene_t*)args;
        g_scenebEnable = scene->bEnable;
        if(!scene->bEnable)
          ret = HI_SCENE_Pause(HI_TRUE);
        else 
          ret = HI_SCENE_Pause(HI_FAILURE);
     }
     break;
     
     case GSF_MPP_ISP_CTL_SHARPEN:
     {   
        gsf_mpp_img_sharpen_t *sharpen = (gsf_mpp_img_sharpen_t*)args;

        printf("sharpen->bEnable:%d,u16TextureFreq:%d\n", sharpen->bEnable, sharpen->u16TextureFreq);  
        ISP_SHARPEN_ATTR_S stIspShpAttr;
        ret = HI_MPI_ISP_GetIspSharpenAttr(ViPipe, &stIspShpAttr);
        
        //stIspShpAttr.bEnable = 1;
        stIspShpAttr.enOpType= (sharpen->bEnable)?OP_TYPE_MANUAL:OP_TYPE_AUTO;
        stIspShpAttr.stManual.u16TextureFreq = sharpen->u16TextureFreq;
        stIspShpAttr.stManual.u16EdgeFreq = sharpen->u16EdgeFreq;
        stIspShpAttr.stManual.u8DetailCtrl = sharpen->u8DetailCtrl;
        ret = HI_MPI_ISP_SetIspSharpenAttr(ViPipe, &stIspShpAttr);
     }
     break; 

     case GSF_MPP_ISP_CTL_HLC:
     {
        gsf_mpp_img_hlc_t *hlc = (gsf_mpp_img_hlc_t*)args;
        if(!hlc->bEnable)
        {
          ret = 0;  
          break;
        }
          
        ISP_HLC_ATTR_S stIspHlcAttr;
        ret = HI_MPI_ISP_GetIspHlcAttr(ViPipe, &stIspHlcAttr);
        
        stIspHlcAttr.bEnable = 1;
        stIspHlcAttr.u8LumaThr = hlc->u8LumaThr;
        stIspHlcAttr.u8LumaTarget = hlc->u8LumaTarget;
        
        ret = HI_MPI_ISP_SetIspHlcAttr(ViPipe, &stIspHlcAttr);
     } 
     break;
     case GSF_MPP_ISP_CTL_GAMMA:
     {
        gsf_mpp_img_gamma_t *gamma = (gsf_mpp_img_gamma_t*)args;
        if(!gamma->bEnable)
        {
          ret = 0;  
          break;
        }
          
        ISP_GAMMA_ATTR_S stGammaAttr;
        ret = HI_MPI_ISP_GetGammaAttr(ViPipe, &stGammaAttr);
        
        stGammaAttr.bEnable = 1;
        stGammaAttr.enCurveType = gamma->enCurveType;
        if(gamma->TableNo >= 0 && gamma->TableNo < GAMMATAB_MAX)
          memcpy(stGammaAttr.u16Table, gammaTab[gamma->TableNo], sizeof(stGammaAttr.u16Table));
        ret = HI_MPI_ISP_SetGammaAttr(ViPipe, &stGammaAttr);
     } 
     break;
     case GSF_MPP_ISP_CTL_DRC:
     {
        gsf_mpp_img_drc_t *drc = (gsf_mpp_img_drc_t*)args;
        if(!drc->bEnable)
        {
          ret = 0;  
          break;
        }
          
        ISP_DRC_ATTR_S stDRC;
        ret = HI_MPI_ISP_GetDRCAttr(ViPipe, &stDRC);
        
        stDRC.bEnable = 1;
        stDRC.enCurveSelect = DRC_CURVE_ASYMMETRY;
        stDRC.enOpType = OP_TYPE_AUTO;
        stDRC.stAuto.u16Strength = drc->u16Strength;
        stDRC.stAuto.u16StrengthMax = drc->u16StrengthMax;
        stDRC.stAuto.u16StrengthMin = drc->u16StrengthMin;        
        ret = HI_MPI_ISP_SetDRCAttr(ViPipe, &stDRC);
     }
     break;
     case GSF_MPP_ISP_CTL_LDCI:
     {
        gsf_mpp_img_ldci_t *ldci = (gsf_mpp_img_ldci_t*)args;
        if(!ldci->bEnable)
        {
          ret = 0;  
          break;
        }
          
        ISP_LDCI_ATTR_S stLDCIAttr;
        ret = HI_MPI_ISP_GetLDCIAttr(ViPipe, &stLDCIAttr);
        
        stLDCIAttr.bEnable = 1;
        stLDCIAttr.enOpType = OP_TYPE_MANUAL;
        stLDCIAttr.stManual.u16BlcCtrl = ldci->u16BlcCtrl;
        stLDCIAttr.stManual.stHeWgt.stHePosWgt.u8Wgt = ldci->stHePosWgt_u8Wgt;
        stLDCIAttr.stManual.stHeWgt.stHeNegWgt.u8Mean = ldci->stHeNegWgt_u8Mean;        
        ret = HI_MPI_ISP_SetLDCIAttr(ViPipe, &stLDCIAttr);
     }
     break;
     case GSF_MPP_ISP_CTL_3DNR:
     {
        /* 3DNR */

        //HI_S32 HI_MPI_VI_SetPipeNRXParam(VI_PIPE ViPipe, const VI_PIPE_NRX_PARAM_S *pstNrXParam);
        //HI_S32 HI_MPI_VI_GetPipeNRXParam(VI_PIPE ViPipe, VI_PIPE_NRX_PARAM_S *pstNrXParam);
              
        //HI_S32 HI_MPI_VPSS_SetGrpNRXParam(VPSS_GRP VpssGrp, const VPSS_GRP_NRX_PARAM_S *pstNRXParam);
        //HI_S32 HI_MPI_VPSS_GetGrpNRXParam(VPSS_GRP VpssGrp, VPSS_GRP_NRX_PARAM_S *pstNRXParam);

        //HI_S32 HI_MPI_ISP_SetDEAttr(VI_PIPE ViPipe, const ISP_DE_ATTR_S *pstDEAttr);
        //HI_S32 HI_MPI_ISP_GetDEAttr(VI_PIPE ViPipe, ISP_DE_ATTR_S *pstDEAttr);
        
        //HI_S32 HI_MPI_VI_SetPipeAttr(ViPipe, &stViPipeAttr); //stViPipeAttr.bNrEn = HI_TRUE;
        //HI_S32 HI_MPI_VPSS_SetGrpAttr(VpssGrp, &stGrpAttr ); //stGrpAttr.bNrEn = HI_TRUE;
        
        gsf_mpp_img_3dnr_t *_3dnr = (gsf_mpp_img_3dnr_t*)args;
        if(!_3dnr->bEnable)
        {
          ret = 0;  
          break;
        }

        VI_PIPE ViPipe = 0;
        VI_PIPE_ATTR_S stViPipeAttr = {0};
        VPSS_GRP VpssGrp = 0;
        VPSS_GRP_ATTR_S stGrpAttr = {0};
        
        VI_PIPE_NRX_PARAM_S stVINRXParam = {0};
        stVINRXParam.enNRVersion = VI_NR_V2;
        stVINRXParam.stNRXParamV2.enOptMode = OPERATION_MODE_MANUAL;
        ret = HI_MPI_VI_GetPipeNRXParam(ViPipe, &stVINRXParam);
        
        VPSS_GRP_NRX_PARAM_S stVPSSNRXParam = {0};
        stVPSSNRXParam.enNRVer = VPSS_NR_V2;
        stVPSSNRXParam.stNRXParam_V2.enOptMode = OPERATION_MODE_MANUAL;
        ret = HI_MPI_VPSS_GetGrpNRXParam(ViPipe, &stVPSSNRXParam);
        

        int i = 0, j = 0;
        HI_SCENE_3DNR_VI_S astThreeDNRVIValue;
        HI_SCENE_3DNR_VPSS_S astThreeDNRVPSSValue;
        
        _3dnr->u8strength = (_3dnr->u8strength >= 0 && _3dnr->u8strength <= 14)?_3dnr->u8strength:0;
        char* pszString = _3dnrParam[_3dnr->u8strength];
        {
            HI_SCENE_3DNR_VI_S *pVIX      = &astThreeDNRVIValue;
            HI_SCENE_3DNR_VPSS_S *pVPX    = &astThreeDNRVPSSValue;
            HI_SCENE_3DNR_VI_IEy *pViI    = &(pVIX->IEy);
            HI_SCENE_3DNR_VI_SFy *pViS    = &(pVIX->SFy);
            HI_SCENE_3DNR_VPSS_IEy  *pVpI = pVPX->IEy;
            HI_SCENE_3DNR_VPSS_SFy  *pVpS = pVPX->SFy;
            HI_SCENE_3DNR_VPSS_MDy  *pVpM = pVPX->MDy;
            HI_SCENE_3DNR_VPSS_TFy  *pVpT = pVPX->TFy;
            HI_SCENE_3DNR_VPSS_RFs  *pVpR = &pVPX->RFs;

            static HI_S32 g_enGmc = NR_MOTION_MODE_NORMAL;
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
        }
        
        /*VPSS 3dnr_level: 1, 2, 3 */
        for (i = 0; i < 3; i++)
        {
            stVPSSNRXParam.stNRXParam_V2.stNRXManual.stNRXParam.SFy[i].SFS1 = astThreeDNRVPSSValue.SFy[i].SFS1; //Interpulate
            stVPSSNRXParam.stNRXParam_V2.stNRXManual.stNRXParam.SFy[i].SFS2 = astThreeDNRVPSSValue.SFy[i].SFS2; //Interpulate
            stVPSSNRXParam.stNRXParam_V2.stNRXManual.stNRXParam.SFy[i].SFS4 = astThreeDNRVPSSValue.SFy[i].SFS4; //Interpulate
            stVPSSNRXParam.stNRXParam_V2.stNRXManual.stNRXParam.SFy[i].SFT1 = astThreeDNRVPSSValue.SFy[i].SFT1; //Interpulate
            stVPSSNRXParam.stNRXParam_V2.stNRXManual.stNRXParam.SFy[i].SFT2 = astThreeDNRVPSSValue.SFy[i].SFT2; //Interpulate
            stVPSSNRXParam.stNRXParam_V2.stNRXManual.stNRXParam.SFy[i].SFT4 = astThreeDNRVPSSValue.SFy[i].SFT4; //Interpulate
            stVPSSNRXParam.stNRXParam_V2.stNRXManual.stNRXParam.SFy[i].SBR1 = astThreeDNRVPSSValue.SFy[i].SBR1; //Interpulate
            stVPSSNRXParam.stNRXParam_V2.stNRXManual.stNRXParam.SFy[i].SBR2 = astThreeDNRVPSSValue.SFy[i].SBR2; //Interpulate
            stVPSSNRXParam.stNRXParam_V2.stNRXManual.stNRXParam.SFy[i].SBR4 = astThreeDNRVPSSValue.SFy[i].SBR4; //Interpulate
            stVPSSNRXParam.stNRXParam_V2.stNRXManual.stNRXParam.SFy[i].SFR  = astThreeDNRVPSSValue.SFy[i].SFR ; //Interpulate
            stVPSSNRXParam.stNRXParam_V2.stNRXManual.stNRXParam.IEy[i].IES0 = astThreeDNRVPSSValue.IEy[i].IES0; //Interpulate
            stVPSSNRXParam.stNRXParam_V2.stNRXManual.stNRXParam.IEy[i].IES1 = astThreeDNRVPSSValue.IEy[i].IES1; //Interpulate
            stVPSSNRXParam.stNRXParam_V2.stNRXManual.stNRXParam.IEy[i].IES2 = astThreeDNRVPSSValue.IEy[i].IES2; //Interpulate
            stVPSSNRXParam.stNRXParam_V2.stNRXManual.stNRXParam.IEy[i].IES3 = astThreeDNRVPSSValue.IEy[i].IES3; //Interpulate
            stVPSSNRXParam.stNRXParam_V2.stNRXManual.stNRXParam.IEy[i].IEDZ = astThreeDNRVPSSValue.IEy[i].IEDZ; //Interpulate

            for (j = 0; j < 3; j++)
            {
                stVPSSNRXParam.stNRXParam_V2.stNRXManual.stNRXParam.SFy[i].SFR6[j]  = astThreeDNRVPSSValue.SFy[i].SFR6[j];
            }

            stVPSSNRXParam.stNRXParam_V2.stNRXManual.stNRXParam.SFy[i].NRyEn = astThreeDNRVPSSValue.SFy[i].NRyEn;
            stVPSSNRXParam.stNRXParam_V2.stNRXManual.stNRXParam.SFy[i].BWSF4 = astThreeDNRVPSSValue.SFy[i].BWSF4;
            stVPSSNRXParam.stNRXParam_V2.stNRXManual.stNRXParam.SFy[i].SPN6  = astThreeDNRVPSSValue.SFy[i].SPN6;
            stVPSSNRXParam.stNRXParam_V2.stNRXManual.stNRXParam.SFy[i].SBN6  = astThreeDNRVPSSValue.SFy[i].SBN6;
            stVPSSNRXParam.stNRXParam_V2.stNRXManual.stNRXParam.SFy[i].PBR6  = astThreeDNRVPSSValue.SFy[i].PBR6;
            stVPSSNRXParam.stNRXParam_V2.stNRXManual.stNRXParam.SFy[i].JMODE = astThreeDNRVPSSValue.SFy[i].JMODE;
            stVPSSNRXParam.stNRXParam_V2.stNRXManual.stNRXParam.SFy[i].TriTh = astThreeDNRVPSSValue.SFy[i].TriTh;

            if (stVINRXParam.stNRXParamV2.stNRXManualV2.stNRXParamV2.SFy.TriTh)
            {
                stVPSSNRXParam.stNRXParam_V2.stNRXManual.stNRXParam.SFy[i].SFN0 = astThreeDNRVPSSValue.SFy[i].SFN0;
                stVPSSNRXParam.stNRXParam_V2.stNRXManual.stNRXParam.SFy[i].SFN1 = astThreeDNRVPSSValue.SFy[i].SFN1;
                stVPSSNRXParam.stNRXParam_V2.stNRXManual.stNRXParam.SFy[i].SFN2 = astThreeDNRVPSSValue.SFy[i].SFN2;
                stVPSSNRXParam.stNRXParam_V2.stNRXManual.stNRXParam.SFy[i].SFN3 = astThreeDNRVPSSValue.SFy[i].SFN3;
                stVPSSNRXParam.stNRXParam_V2.stNRXManual.stNRXParam.SFy[i].STH1 = astThreeDNRVPSSValue.SFy[i].STH1; //Interpulate
                stVPSSNRXParam.stNRXParam_V2.stNRXManual.stNRXParam.SFy[i].STH2 = astThreeDNRVPSSValue.SFy[i].STH2; //Interpulate
                stVPSSNRXParam.stNRXParam_V2.stNRXManual.stNRXParam.SFy[i].STH3 = astThreeDNRVPSSValue.SFy[i].STH3; //Interpulate
            }
            else
            {
                stVPSSNRXParam.stNRXParam_V2.stNRXManual.stNRXParam.SFy[i].SFN0 = astThreeDNRVPSSValue.SFy[i].SFN0;
                stVPSSNRXParam.stNRXParam_V2.stNRXManual.stNRXParam.SFy[i].SFN1 = astThreeDNRVPSSValue.SFy[i].SFN1;
                stVPSSNRXParam.stNRXParam_V2.stNRXManual.stNRXParam.SFy[i].SFN3 = astThreeDNRVPSSValue.SFy[i].SFN3;
                stVPSSNRXParam.stNRXParam_V2.stNRXManual.stNRXParam.SFy[i].STH1 = astThreeDNRVPSSValue.SFy[i].STH1; //Interpulate
                stVPSSNRXParam.stNRXParam_V2.stNRXManual.stNRXParam.SFy[i].STH3 = astThreeDNRVPSSValue.SFy[i].STH3; //Interpulate
            }
        }

        stVPSSNRXParam.stNRXParam_V2.stNRXManual.stNRXParam.SFy[1].kMode = astThreeDNRVPSSValue.SFy[1].kMode;
        stVPSSNRXParam.stNRXParam_V2.stNRXManual.stNRXParam.SFy[2].kMode = astThreeDNRVPSSValue.SFy[2].kMode;

        if (stVPSSNRXParam.stNRXParam_V2.stNRXManual.stNRXParam.SFy[1].kMode > 1)
        {
            for (j = 0; j < 32; j++)
            {
                stVPSSNRXParam.stNRXParam_V2.stNRXManual.stNRXParam.SFy[1].SBSk[j] = astThreeDNRVPSSValue.SFy[1].SBSk[j];
                stVPSSNRXParam.stNRXParam_V2.stNRXManual.stNRXParam.SFy[1].SDSk[j] = astThreeDNRVPSSValue.SFy[1].SDSk[j];
            }
        }

        if (stVPSSNRXParam.stNRXParam_V2.stNRXManual.stNRXParam.SFy[2].kMode > 1)
        {
            for (j = 0; j < 32; j++)
            {
                stVPSSNRXParam.stNRXParam_V2.stNRXManual.stNRXParam.SFy[2].SBSk[j] = astThreeDNRVPSSValue.SFy[2].SBSk[j];
                stVPSSNRXParam.stNRXParam_V2.stNRXManual.stNRXParam.SFy[2].SDSk[j] = astThreeDNRVPSSValue.SFy[2].SDSk[j];
            }
        }

        stVPSSNRXParam.stNRXParam_V2.stNRXManual.stNRXParam.TFy[0].tEdge = astThreeDNRVPSSValue.TFy[0].tEdge;
        stVPSSNRXParam.stNRXParam_V2.stNRXManual.stNRXParam.TFy[1].tEdge = astThreeDNRVPSSValue.TFy[1].tEdge;
        stVPSSNRXParam.stNRXParam_V2.stNRXManual.stNRXParam.TFy[0].bRef = astThreeDNRVPSSValue.TFy[0].bRef;
        //stVPSSNRXParam.stNRXParam_V2.stNRXManual.stNRXParam.GMC.GMEMode = astThreeDNRVPSSValue.GMC.GMEMode;
        stGrpAttr.stNrAttr.enNrMotionMode = astThreeDNRVPSSValue.GMC.GMEMode;
        stVPSSNRXParam.stNRXParam_V2.stNRXManual.stNRXParam.TFy[0].RFI = astThreeDNRVPSSValue.TFy[0].RFI;
        stVPSSNRXParam.stNRXParam_V2.stNRXManual.stNRXParam.TFy[0].DZMode0 = astThreeDNRVPSSValue.TFy[0].DZMode0;
        stVPSSNRXParam.stNRXParam_V2.stNRXManual.stNRXParam.TFy[0].DZMode1 = astThreeDNRVPSSValue.TFy[0].DZMode1;
        stVPSSNRXParam.stNRXParam_V2.stNRXManual.stNRXParam.TFy[1].DZMode0 = astThreeDNRVPSSValue.TFy[1].DZMode0;
        stVPSSNRXParam.stNRXParam_V2.stNRXManual.stNRXParam.TFy[1].DZMode1 = astThreeDNRVPSSValue.TFy[1].DZMode1;

        stVPSSNRXParam.stNRXParam_V2.stNRXManual.stNRXParam.RFs.RFUI = astThreeDNRVPSSValue.RFs.RFUI;
        stVPSSNRXParam.stNRXParam_V2.stNRXManual.stNRXParam.RFs.RFDZ = astThreeDNRVPSSValue.RFs.RFDZ;
        stVPSSNRXParam.stNRXParam_V2.stNRXManual.stNRXParam.RFs.RFSLP = astThreeDNRVPSSValue.RFs.RFSLP;
        stVPSSNRXParam.stNRXParam_V2.stNRXManual.stNRXParam.RFs.advMATH = astThreeDNRVPSSValue.RFs.advMATH;

        for (i = 0; i < 2; i++)
        {

            stVPSSNRXParam.stNRXParam_V2.stNRXManual.stNRXParam.MDy[i].MATH0 = astThreeDNRVPSSValue.MDy[i].MATH0; //Interpulate
            stVPSSNRXParam.stNRXParam_V2.stNRXManual.stNRXParam.MDy[i].MATH1 = astThreeDNRVPSSValue.MDy[i].MATH1; //Interpulate
            stVPSSNRXParam.stNRXParam_V2.stNRXManual.stNRXParam.MDy[i].MATE0 = astThreeDNRVPSSValue.MDy[i].MATE0; //Interpulate
            stVPSSNRXParam.stNRXParam_V2.stNRXManual.stNRXParam.MDy[i].MATE1 = astThreeDNRVPSSValue.MDy[i].MATE1; //Interpulate
            stVPSSNRXParam.stNRXParam_V2.stNRXManual.stNRXParam.MDy[i].MABW0 = astThreeDNRVPSSValue.MDy[i].MABW0; //Interpulate
            stVPSSNRXParam.stNRXParam_V2.stNRXManual.stNRXParam.MDy[i].MABW1 = astThreeDNRVPSSValue.MDy[i].MABW1; //Interpulate
            stVPSSNRXParam.stNRXParam_V2.stNRXManual.stNRXParam.MDy[i].MASW  = astThreeDNRVPSSValue.MDy[i].MASW ; //Interpulate
            stVPSSNRXParam.stNRXParam_V2.stNRXManual.stNRXParam.MDy[i].MADZ0 = astThreeDNRVPSSValue.MDy[i].MADZ0; //Interpulate
            stVPSSNRXParam.stNRXParam_V2.stNRXManual.stNRXParam.MDy[i].MADZ1 = astThreeDNRVPSSValue.MDy[i].MADZ1; //Interpulate 

            if ((stVPSSNRXParam.stNRXParam_V2.stNRXManual.stNRXParam.MDy[i].MADZ0 > 0) || (stVPSSNRXParam.stNRXParam_V2.stNRXManual.stNRXParam.MDy[i].MADZ1 > 0))
            {
                //for (j = 0; j > 16; j++)
                {
                    stVPSSNRXParam.stNRXParam_V2.stNRXManual.stNRXParam.MDy[i].MABR0 = astThreeDNRVPSSValue.MDy[i].MABR0;
                    stVPSSNRXParam.stNRXParam_V2.stNRXManual.stNRXParam.MDy[i].MABR1 = astThreeDNRVPSSValue.MDy[i].MABR1;
                }
            }

            stVPSSNRXParam.stNRXParam_V2.stNRXManual.stNRXParam.MDy[i].biPath = astThreeDNRVPSSValue.MDy[i].biPath;
            stVPSSNRXParam.stNRXParam_V2.stNRXManual.stNRXParam.MDy[i].MAI00 = astThreeDNRVPSSValue.MDy[i].MAI00;
            stVPSSNRXParam.stNRXParam_V2.stNRXManual.stNRXParam.MDy[i].MAI01 = astThreeDNRVPSSValue.MDy[i].MAI01;
            stVPSSNRXParam.stNRXParam_V2.stNRXManual.stNRXParam.MDy[i].MAI02 = astThreeDNRVPSSValue.MDy[i].MAI02;
            stVPSSNRXParam.stNRXParam_V2.stNRXManual.stNRXParam.MDy[i].MAI10 = astThreeDNRVPSSValue.MDy[i].MAI10;
            stVPSSNRXParam.stNRXParam_V2.stNRXManual.stNRXParam.MDy[i].MAI11 = astThreeDNRVPSSValue.MDy[i].MAI11;
            stVPSSNRXParam.stNRXParam_V2.stNRXManual.stNRXParam.MDy[i].MAI12 = astThreeDNRVPSSValue.MDy[i].MAI12;
            stVPSSNRXParam.stNRXParam_V2.stNRXManual.stNRXParam.MDy[i].MATW  = astThreeDNRVPSSValue.MDy[i].MATW;
            stVPSSNRXParam.stNRXParam_V2.stNRXManual.stNRXParam.MDy[i].MASW  = astThreeDNRVPSSValue.MDy[i].MASW;
        }

        for (i = 0; i < 2; i++)
        {
            stVPSSNRXParam.stNRXParam_V2.stNRXManual.stNRXParam.TFy[i].TSS0 = astThreeDNRVPSSValue.TFy[i].TSS0; //Interpulate  
            stVPSSNRXParam.stNRXParam_V2.stNRXManual.stNRXParam.TFy[i].TSS1 = astThreeDNRVPSSValue.TFy[i].TSS1; //Interpulate  
            stVPSSNRXParam.stNRXParam_V2.stNRXManual.stNRXParam.TFy[i].TFS0 = astThreeDNRVPSSValue.TFy[i].TFS0; //Interpulate  
            stVPSSNRXParam.stNRXParam_V2.stNRXManual.stNRXParam.TFy[i].TFS1 = astThreeDNRVPSSValue.TFy[i].TFS1; //Interpulate  

            for (j = 0; j < 6; j++)
            {
                stVPSSNRXParam.stNRXParam_V2.stNRXManual.stNRXParam.TFy[i].TFR0[j] = astThreeDNRVPSSValue.TFy[i].TFR0[j]; //Interpulate  
                stVPSSNRXParam.stNRXParam_V2.stNRXManual.stNRXParam.TFy[i].TFR0[j] = astThreeDNRVPSSValue.TFy[i].TFR0[j]; //Interpulate  
                stVPSSNRXParam.stNRXParam_V2.stNRXManual.stNRXParam.TFy[i].TFR1[j] = astThreeDNRVPSSValue.TFy[i].TFR1[j]; //Interpulate  
                stVPSSNRXParam.stNRXParam_V2.stNRXManual.stNRXParam.TFy[i].TFR1[j] = astThreeDNRVPSSValue.TFy[i].TFR1[j]; //Interpulate  
            }

            stVPSSNRXParam.stNRXParam_V2.stNRXManual.stNRXParam.TFy[i].STR0 = astThreeDNRVPSSValue.TFy[i].STR0;
            stVPSSNRXParam.stNRXParam_V2.stNRXManual.stNRXParam.TFy[i].STR1 = astThreeDNRVPSSValue.TFy[i].STR1;
            stVPSSNRXParam.stNRXParam_V2.stNRXManual.stNRXParam.TFy[i].SDZ0 = astThreeDNRVPSSValue.TFy[i].SDZ0;
            stVPSSNRXParam.stNRXParam_V2.stNRXManual.stNRXParam.TFy[i].SDZ1 = astThreeDNRVPSSValue.TFy[i].SDZ1;
            stVPSSNRXParam.stNRXParam_V2.stNRXManual.stNRXParam.TFy[i].TSI0 = astThreeDNRVPSSValue.TFy[i].TSI0;
            stVPSSNRXParam.stNRXParam_V2.stNRXManual.stNRXParam.TFy[i].TSI1 = astThreeDNRVPSSValue.TFy[i].TSI1;
            stVPSSNRXParam.stNRXParam_V2.stNRXManual.stNRXParam.TFy[i].TDZ0 = astThreeDNRVPSSValue.TFy[i].TDZ0;
            stVPSSNRXParam.stNRXParam_V2.stNRXManual.stNRXParam.TFy[i].TDZ1 = astThreeDNRVPSSValue.TFy[i].TDZ1;
            stVPSSNRXParam.stNRXParam_V2.stNRXManual.stNRXParam.TFy[i].TDX0 = astThreeDNRVPSSValue.TFy[i].TDX0;
            stVPSSNRXParam.stNRXParam_V2.stNRXManual.stNRXParam.TFy[i].TDX1 = astThreeDNRVPSSValue.TFy[i].TDX1;
        }

        stVPSSNRXParam.stNRXParam_V2.stNRXManual.stNRXParam.pNRc.SFC  = astThreeDNRVPSSValue.pNRc.SFC ; //Interpulate
        stVPSSNRXParam.stNRXParam_V2.stNRXManual.stNRXParam.pNRc.CTFS = astThreeDNRVPSSValue.pNRc.CTFS; //Interpulate
        stVPSSNRXParam.stNRXParam_V2.stNRXManual.stNRXParam.pNRc.TFC  = astThreeDNRVPSSValue.pNRc.TFC ; //Interpulate

        //stVINRXParam.enNRVersion = VI_NR_V2;
        //stVINRXParam.stNRXParamV2.enOptMode = OPERATION_MODE_MANUAL;
        //ret = HI_MPI_VI_SetPipeNRXParam(ViPipe, &stVINRXParam);
        
        stVPSSNRXParam.enNRVer = VPSS_NR_V2;
        stVPSSNRXParam.stNRXParam_V2.enOptMode = OPERATION_MODE_MANUAL;
        ret = HI_MPI_VPSS_SetGrpNRXParam(VpssGrp, &stVPSSNRXParam);

        ret = HI_MPI_VPSS_GetGrpAttr(VpssGrp, &stGrpAttr);
        stGrpAttr.bNrEn = HI_TRUE;
        ret = HI_MPI_VPSS_SetGrpAttr(VpssGrp, &stGrpAttr);
     }
     break; 
     case GSF_MPP_ISP_CTL_FLIP:
     {
      gsf_mpp_img_flip_t *flip = (gsf_mpp_img_flip_t*)args;
      
      VI_CHN_ATTR_S stChnAttr;
      ret = HI_MPI_VI_GetChnAttr(ViPipe, 0, &stChnAttr);
      printf("GET ret:0x%x, bFlip:%d, bMirror:%d\n", ret, stChnAttr.bFlip, stChnAttr.bMirror);
      
      stChnAttr.bFlip = flip->bFlip;
      stChnAttr.bMirror = flip->bMirror;
      
      ret = HI_MPI_VI_SetChnAttr(ViPipe, 0, &stChnAttr);
      printf("SET ret:0x%x, bFlip:%d, bMirror:%d\n", ret, stChnAttr.bFlip, stChnAttr.bMirror);  
      
     }
     break;
     case GSF_MPP_ISP_CTL_DIS:
     {
      gsf_mpp_img_dis_t *dis = (gsf_mpp_img_dis_t*)args;

      DIS_CONFIG_S stDISConfig     = {0};
      DIS_ATTR_S stDISAttr         = {0};
      
      HI_MPI_VI_GetChnDISAttr(ViPipe, 0, &stDISAttr);
      if(stDISAttr.bEnable)
      {
        stDISAttr.bEnable = HI_FALSE;
        HI_MPI_VI_SetChnDISAttr(ViPipe, 0, &stDISAttr);
      }      

      stDISConfig.enMode              = dis->enMode;//DIS_MODE_4_DOF_GME;//DIS_MODE_6_DOF_GME; //DIS_MODE_4_DOF_GME;
      stDISConfig.enMotionLevel       = DIS_MOTION_LEVEL_NORMAL;
      stDISConfig.u32CropRatio        = 80;
      stDISConfig.u32BufNum           = 5;
      stDISConfig.u32FrameRate        = 30;
      stDISConfig.enPdtType           = dis->enPdtType;//DIS_PDT_TYPE_DV;//DIS_PDT_TYPE_IPC; //DIS_PDT_TYPE_DV; DIS_PDT_TYPE_DRONE;
      stDISConfig.u32GyroOutputRange  = 0;
      stDISConfig.bScale              = HI_TRUE; //HI_FALSE;
      stDISConfig.bCameraSteady       = HI_FALSE;
      stDISConfig.u32GyroDataBitWidth = 0;

      stDISAttr.bEnable               = dis->bEnable; //HI_TRUE;
      stDISAttr.u32MovingSubjectLevel = 0;
      stDISAttr.s32RollingShutterCoef = 0;
      stDISAttr.s32Timelag            = 0;
      stDISAttr.u32ViewAngle          = 1000;
      stDISAttr.bStillCrop            = HI_FALSE;
      stDISAttr.u32HorizontalLimit    = 512;
      stDISAttr.u32VerticalLimit      = 512;
      stDISAttr.bGdcBypass            = HI_FALSE;
      stDISAttr.u32Strength           = 1024;
          
      stDISConfig.u32FrameRate        = 30;
      stDISAttr.s32Timelag            = 33333;

      ret = HI_MPI_VI_SetChnDISConfig(ViPipe, 0, &stDISConfig);
      printf("SET dis config ret:0x%x, enMode:%d, enPdtType:%d\n", ret, stDISConfig.enMode, stDISConfig.enPdtType);

      ret = HI_MPI_VI_SetChnDISAttr(ViPipe, 0, &stDISAttr);
      printf("SET dis attr ret:0x%x, bEnable:%d\n", ret, stDISAttr.bEnable);
      
     }
     break;
     case GSF_MPP_ISP_CTL_LDC:
     {
      gsf_mpp_img_ldc_t *ldc = (gsf_mpp_img_ldc_t*)args;
      
      VI_LDC_ATTR_S stLDCAttr = {0};
      stLDCAttr.bEnable = ldc->bEnable;
      stLDCAttr.stAttr.bAspect = 1;              /* RW;Range: [0, 1];Whether aspect ration  is keep */
      stLDCAttr.stAttr.s32XRatio = 100;          /* RW; Range: [0, 100]; field angle ration of  horizontal,valid when bAspect=0.*/
      stLDCAttr.stAttr.s32YRatio = 100;          /* RW; Range: [0, 100]; field angle ration of  vertical,valid when bAspect=0.*/
      stLDCAttr.stAttr.s32XYRatio = 100;         /* RW; Range: [0, 100]; field angle ration of  all,valid when bAspect=1.*/
      stLDCAttr.stAttr.s32CenterXOffset = 0;     /* RW; Range: [-511, 511]; horizontal offset of the image distortion center relative to image center.*/
      stLDCAttr.stAttr.s32CenterYOffset = 0;     /* RW; Range: [-511, 511]; vertical offset of the image distortion center relative to image center.*/
      stLDCAttr.stAttr.s32DistortionRatio = ldc->s32DistortionRatio;  /* RW; Range: [-300, 500]; LDC Distortion ratio.When spread on,s32DistortionRatio range should be [0, 500]*/
      
      ret = HI_MPI_VI_SetChnLDCAttr(ViPipe, 0, &stLDCAttr);
      printf("SetChnLDCAttr ret:0x%x, ViPipe:%d, bEnable:%d, s32DistortionRatio:%d\n"
            , ret, ViPipe, stLDCAttr.bEnable, stLDCAttr.stAttr.s32DistortionRatio);
      
      //HI_S32 HI_MPI_VI_SetChnSpreadAttr(VI_PIPE ViPipe, VI_CHN ViChn, const SPREAD_ATTR_S *pstSpreadAttr);
     }
     break;
     default:
     break;
  }
  printf("ViPipe:%d, id:%d, ret:%d\n", ViPipe, id, ret);
  return ret;
}

static int _audio_init = 0;
static gsf_mpp_aenc_t _aenc;

int gsf_mpp_audio_start(gsf_mpp_aenc_t *aenc)
{
  if(!_audio_init)
  {
    _audio_init = 1;
    extern HI_S32 sample_audio_init();
    sample_audio_init();
  }
  
  if(aenc == NULL)
  { //test ai->ao;
    static pthread_t aio_pid;
    extern HI_S32 SAMPLE_AUDIO_AiAo(HI_VOID);
    return pthread_create(&aio_pid, 0, (void*(*)(void*))SAMPLE_AUDIO_AiAo, NULL);
  }
  else
  {
    extern HI_S32 SAMPLE_AUDIO_AiAenc(gsf_mpp_aenc_t *aenc);
    
    _aenc = *aenc;
    return SAMPLE_AUDIO_AiAenc(aenc);
  }
  return 0;
}

int gsf_mpp_audio_stop(gsf_mpp_aenc_t *aenc)
{
  if(aenc == NULL)
  {
    ;  
  }
  else
  {
    extern HI_S32 SAMPLE_AUDIO_AiAencStop(gsf_mpp_aenc_t *aenc);
    return SAMPLE_AUDIO_AiAencStop(aenc);
  }
  return 0;
}


//HI_S32 HI_MPI_RGN_Create(RGN_HANDLE Handle,const RGN_ATTR_S *pstRegion);
//HI_S32 HI_MPI_RGN_Destroy(RGN_HANDLE Handle);
//HI_S32 HI_MPI_RGN_AttachToChn(RGN_HANDLE Handle, const MPP_CHN_S *pstChn,const RGN_CHN_ATTR_S *pstChnAttr);
//HI_S32 HI_MPI_RGN_DetachFromChn(RGN_HANDLE Handle,const MPP_CHN_S *pstChn);
//HI_S32 HI_MPI_RGN_SetAttr(RGN_HANDLE Handle,const RGN_ATTR_S *pstRegion);
//HI_S32 HI_MPI_RGN_GetAttr(RGN_HANDLE Handle, RGN_ATTR_S *pstRegion);
//HI_S32 HI_MPI_RGN_SetDisplayAttr(RGN_HANDLE Handle,const MPP_CHN_S *pstChn,const RGN_CHN_ATTR_S *pstChnAttr);
//HI_S32 HI_MPI_RGN_GetDisplayAttr(RGN_HANDLE Handle,const MPP_CHN_S *pstChn, RGN_CHN_ATTR_S *pstChnAttr);

int gsf_mpp_rgn_ctl(RGN_HANDLE Handle, int id, gsf_mpp_rgn_t *rgn)
{
  int ret = -1;
  switch(id)
  {
    case GSF_MPP_RGN_CREATE:
      ret = HI_MPI_RGN_Create(Handle, &rgn->stRegion);
      break;
    case GSF_MPP_RGN_DESTROY:
      ret = HI_MPI_RGN_Destroy(Handle);
      break;
    case GSF_MPP_RGN_SETATTR:
      ret = HI_MPI_RGN_SetAttr(Handle, &rgn->stRegion);
      break;
    case GSF_MPP_RGN_GETATTR:
      ret = HI_MPI_RGN_GetAttr(Handle, &rgn->stRegion);
      break;
    case GSF_MPP_RGN_ATTACH:
      ret = HI_MPI_RGN_AttachToChn(Handle, &rgn->stChn, &rgn->stChnAttr);
      break;
    case GSF_MPP_RGN_DETACH:
      ret = HI_MPI_RGN_DetachFromChn(Handle, &rgn->stChn);
      break;
    case GSF_MPP_RGN_SETDISPLAY:
      ret = HI_MPI_RGN_SetDisplayAttr(Handle, &rgn->stChn, &rgn->stChnAttr);
      break;
    case GSF_MPP_RGN_GETDISPLAY:
      ret = HI_MPI_RGN_GetDisplayAttr(Handle, &rgn->stChn, &rgn->stChnAttr);
      break;
  }
  //printf("handle:%d, id:%d, enType:%d, ret:%d\n", Handle, id, rgn->stRegion.enType, ret);
  return ret;
}

int gsf_mpp_rgn_bitmap(RGN_HANDLE Handle, BITMAP_S *bitmap)
{
  return HI_MPI_RGN_SetBitMap(Handle, bitmap);
}

int gsf_mpp_rgn_canvas_get(RGN_HANDLE Handle, RGN_CANVAS_INFO_S *pstRgnCanvasInfo)
{
  int s32Ret = HI_MPI_RGN_GetCanvasInfo(Handle, pstRgnCanvasInfo);
  if(HI_SUCCESS != s32Ret)
  {
      SAMPLE_PRT("HI_MPI_RGN_GetCanvasInfo failed with %#x!\n", s32Ret);
      return HI_FAILURE;
  }
  return s32Ret;
}
int gsf_mpp_rgn_canvas_update(RGN_HANDLE Handle)
{
 return HI_MPI_RGN_UpdateCanvas(Handle); 
}


typedef struct {
  int adec;
  int ao;
  // ...
}vo_audio_t;

typedef struct {
  int en;
  int depth;
  VO_WBC_MODE_E   mode;
  VO_WBC_ATTR_S   attr;
  VO_WBC_SOURCE_S src;
  // ...
}vo_wbc_t;

typedef enum {
  VO_SRC_VP = 0,
  VO_SRC_VDVP,
}VO_SRC_E;

typedef struct {
  int etype;
  int width;
  int height;
  VO_SRC_E src_type;
  int src_grp;
  int src_chn;
  // ...
}vo_ch_t;

typedef struct {
  RECT_S     rect;
  VO_LAYOUT_E cnt;
  vo_ch_t    chs[VO_MAX_CHN_NUM];
  // ...
}vo_layer_t;

typedef struct {
  // lock;
  pthread_mutex_t lock;
  // vodev;
  VO_INTF_TYPE_E intf;
  VO_INTF_SYNC_E sync;
  
  vo_wbc_t wbc;
  vo_audio_t audio;
  
  // volayer;
  vo_layer_t layer[VOLAYER_BUTT];
  
  // ...
}vo_mng_t;

static vo_mng_t vo_mng[VO_MAX_DEV_NUM];

static int layer2vdev[VO_MAX_LAYER_NUM] = {
  [VOLAYER_HD0] = VODEV_HD0,
};

static int SAMPLE_VDEC_INIT(HI_U32 u32VdecChnNum)
{
    HI_S32 i, s32Ret = HI_SUCCESS;
    /************************************************
    step2:  init module VB or user VB(for VDEC)
    *************************************************/
    SAMPLE_VDEC_ATTR astSampleVdec[VDEC_MAX_CHN_NUM];
    for(i=0; i<u32VdecChnNum; i++)
    {
        astSampleVdec[i].enType                           = PT_H264;
        astSampleVdec[i].u32Width                         = u32VdecChnNum>1?1920:3840;
        astSampleVdec[i].u32Height                        = u32VdecChnNum>1?1080:2160;
        astSampleVdec[i].enMode                           = VIDEO_MODE_FRAME;
        astSampleVdec[i].stSapmleVdecVideo.enDecMode      = VIDEO_DEC_MODE_IP;//VIDEO_DEC_MODE_IPB;
        astSampleVdec[i].stSapmleVdecVideo.enBitWidth     = DATA_BITWIDTH_8;
        astSampleVdec[i].stSapmleVdecVideo.u32RefFrameNum = 3;
        astSampleVdec[i].u32DisplayFrameNum               = 2;
        astSampleVdec[i].u32FrameBufCnt = astSampleVdec[i].stSapmleVdecVideo.u32RefFrameNum + astSampleVdec[i].u32DisplayFrameNum + 1;
    }
    s32Ret = SAMPLE_COMM_VDEC_InitVBPool(u32VdecChnNum, &astSampleVdec[0]);
    if(s32Ret != HI_SUCCESS)
    {
        SAMPLE_PRT("init mod common vb fail for %#x!\n", s32Ret);
        goto END2;
    }
  
    VDEC_MOD_PARAM_S stModParam;
  
    CHECK_RET(HI_MPI_VDEC_GetModParam(&stModParam), "HI_MPI_VDEC_GetModParam");
    stModParam.enVdecVBSource = g_enVdecVBSource;
    CHECK_RET(HI_MPI_VDEC_SetModParam(&stModParam), "HI_MPI_VDEC_GetModParam");  
    return 0;
    
END2:
    SAMPLE_COMM_VDEC_ExitVBPool();
    return s32Ret;
}

int gsf_mpp_cfg_vdec(char *path, gsf_mpp_cfg_t *cfg)
{
    char loadstr[64];
    sprintf(loadstr, "%s/ko/load3516dv300 -i", path);
    printf("%s => loadstr: %s\n", __func__, loadstr);
    system(loadstr);
    
    signal(SIGINT, SAMPLE_VDEC_HandleSig);
    signal(SIGTERM, SAMPLE_VDEC_HandleSig);
    
    
    HI_S32 i, s32Ret = HI_SUCCESS;

    /************************************************
    step1:  init SYS, init common VB(for VPSS and VO)
    *************************************************/
    SIZE_S stDispSize;
    PIC_SIZE_E enDispPicSize = (cfg->res<=2)?PIC_1080P:PIC_3840x2160; 
    s32Ret =  SAMPLE_COMM_SYS_GetPicSize(enDispPicSize, &stDispSize);
    if(s32Ret != HI_SUCCESS)
    {
        SAMPLE_PRT("sys get pic size fail for %#x!\n", s32Ret);
        goto END1;
    }

	  //stDispSize.u32Height = (stDispSize.u32Height == 1080)?1088:stDispSize.u32Height;

    VB_CONFIG_S stVbConfig;
    memset(&stVbConfig, 0, sizeof(VB_CONFIG_S));
    stVbConfig.u32MaxPoolCnt             = 128;
    stVbConfig.astCommPool[0].u32BlkCnt  = (enDispPicSize == PIC_1080P)?10*4:10*1;
    stVbConfig.astCommPool[0].u64BlkSize = COMMON_GetPicBufferSize(stDispSize.u32Width, stDispSize.u32Height,
                                                PIXEL_FORMAT_YVU_SEMIPLANAR_420, DATA_BITWIDTH_8, COMPRESS_MODE_SEG, 0);
    s32Ret = SAMPLE_COMM_SYS_Init(&stVbConfig);
    if(s32Ret != HI_SUCCESS)
    {
        SAMPLE_PRT("init sys fail for %#x!\n", s32Ret);
        goto END1;
    }

    //moveto gsf_mpp_vo_start SAMPLE_VDEC_INIT(4);
    return 0;
  
END1:
    SAMPLE_COMM_SYS_Exit();
  return s32Ret;
}


static int vo_csc()
{
  if(vo_mng[0].intf == VO_INTF_MIPI)
  {
    VO_CSC_S stGhCSC = {0};
    int s32Ret = HI_MPI_VO_GetGraphicLayerCSC(0, &stGhCSC);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("HI_MPI_VO_GetGraphicLayerCSC failed s32Ret:0x%x!\n", s32Ret);
    }
    stGhCSC.enCscMatrix = VO_CSC_MATRIX_IDENTITY;
    s32Ret = HI_MPI_VO_SetGraphicLayerCSC(0, &stGhCSC);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("HI_MPI_VO_SetGraphicLayerCSC failed s32Ret:0x%x!\n", s32Ret);
    }
  }
}


//启动视频输出设备;
int gsf_mpp_vo_start(int vodev, VO_INTF_TYPE_E type, VO_INTF_SYNC_E sync, int wbc)
{
    HI_S32 i, j, s32Ret = HI_SUCCESS;
    SIZE_S stDispSize;
    PIC_SIZE_E enDispPicSize = PIC_1080P;
    SAMPLE_VO_CONFIG_S stVoConfig;
    
    HI_U32 u32VoFrmRate = 0;
    s32Ret = SAMPLE_COMM_VO_GetWH(sync, &stDispSize.u32Width,
                              &stDispSize.u32Height, &u32VoFrmRate);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("get vo width and height failed with %d!\n", s32Ret);
        return s32Ret;
    }
    /************************************************
    step5:  start VO
    *************************************************/
    stVoConfig.VoDev                 = vodev;
    stVoConfig.enVoIntfType          = type;
    stVoConfig.enIntfSync            = sync;
    stVoConfig.enPicSize             = enDispPicSize; // unused;
    stVoConfig.u32BgColor            = COLOR_RGB_BLACK;//COLOR_RGB_BLUE;
    stVoConfig.u32DisBufLen          = 3;
    stVoConfig.enDstDynamicRange     = DYNAMIC_RANGE_SDR8;
    stVoConfig.enVoMode              = VO_MODE_1MUX;
    stVoConfig.enPixFormat           = PIXEL_FORMAT_YVU_SEMIPLANAR_420; //422
    stVoConfig.stDispRect.s32X       = 0;
    stVoConfig.stDispRect.s32Y       = 0;
    stVoConfig.stDispRect.u32Width   = stDispSize.u32Width;
    stVoConfig.stDispRect.u32Height  = stDispSize.u32Height;
    stVoConfig.stImageSize.u32Width  = stDispSize.u32Width;
    stVoConfig.stImageSize.u32Height = stDispSize.u32Height;
    stVoConfig.enVoPartMode          = VO_PART_MODE_SINGLE;//VO_PART_MODE_MULTI;

    
    s32Ret = SAMPLE_COMM_VO_StartVO(&stVoConfig);
    if(s32Ret != HI_SUCCESS)
    {
        SAMPLE_PRT("start VO fail for %#x!\n", s32Ret);
        SAMPLE_COMM_VO_StopVO(&stVoConfig);
    }
    
    vo_mng[vodev].intf = stVoConfig.enVoIntfType;
    vo_mng[vodev].sync = stVoConfig.enIntfSync;
    printf("vodev:%d, intf:%d, sync:%d, u32Width:%d, u32Height:%d\n"
            , vodev, vo_mng[vodev].intf, vo_mng[vodev].sync, stDispSize.u32Width, stDispSize.u32Height);         

    for(i = 0; i < VO_MAX_DEV_NUM; i++)
    {
      pthread_mutex_init(&vo_mng[i].lock, NULL);
      //pthread_mutex_destroy(&mutex);
      for(j = 0; j < VOLAYER_BUTT; j++)
      {
        int k = 0;
        for(k = 0; k < VO_MAX_CHN_NUM; k++)
          vo_mng[i].layer[j].chs[k].src_grp = vo_mng[i].layer[j].chs[k].src_chn = -1;
      
        vo_mng[i].layer[j].rect.s32X = vo_mng[i].layer[j].rect.s32Y = 0;
        vo_mng[i].layer[j].rect.u32Width = stVoConfig.stDispRect.u32Width;
        vo_mng[i].layer[j].rect.u32Height = stVoConfig.stDispRect.u32Height;
      }
    }

    SAMPLE_VDEC_INIT((sync==VO_OUTPUT_1080P60)?4:1);

    mppex_hook_vo(sync);
    
    return s32Ret;
}

//停止视频输出设备
int gsf_mpp_vo_stop(int vodev)
{
    vo_mng_t *vdev = &vo_mng[vodev];
    
    VO_LAYER VoLayer = (vodev == VODEV_HD0)?VOFB_GUI:VOFB_GUI1;
    SAMPLE_VO_MODE_E enVoMode   = vdev->layer[VoLayer].cnt;
    VO_INTF_TYPE_E enVoIntfType = vdev->intf;

    SAMPLE_COMM_VO_HdmiStop();
    SAMPLE_COMM_VO_StopChn(VoLayer, enVoMode);
    SAMPLE_COMM_VO_StopLayer(VoLayer);
    SAMPLE_COMM_VO_StopDev(vodev);
    
    return 0;
}

//创建视频层显示通道;
int gsf_mpp_vo_layout(int volayer, VO_LAYOUT_E layout, RECT_S *rect)
{
  int i = 0;
  int err = 0;
  HI_S32 s32Ret = HI_SUCCESS;
   
  vo_mng_t *vdev = &vo_mng[layer2vdev[volayer]];
  
  if(vdev->layer[volayer].cnt == layout)
  {
    return 0;
  }
  
  pthread_mutex_lock(&vdev->lock);
  
  for(i = 0; i < vdev->layer[volayer].cnt; i++)
  {
    // unbind vo && vpss && vdec;
    
    int vpss_grp = vdev->layer[volayer].chs[i].src_grp;
    int vpss_chn = vdev->layer[volayer].chs[i].src_chn;
    
    if(vpss_grp != -1)
    {
      SAMPLE_COMM_VPSS_UnBind_VO(vpss_grp, vpss_chn, volayer, i);
      if(vdev->layer[volayer].chs[i].src_type >= VO_SRC_VDVP)
      {
        SAMPLE_COMM_VDEC_UnBind_VPSS(i, vpss_grp);
        HI_BOOL chen[VPSS_MAX_PHY_CHN_NUM] = {HI_TRUE,HI_TRUE};
        SAMPLE_COMM_VPSS_Stop(vpss_grp, chen);
      }
    }
    // only set width = height = 0, tell gsf_mpp_vo_vsend to recreate vdec&vpss;
    vdev->layer[volayer].chs[i].width = vdev->layer[volayer].chs[i].height = 0;
  }

  if(vdev->layer[volayer].cnt)
    SAMPLE_COMM_VO_StopChn(volayer, vdev->layer[volayer].cnt);

  
  if(vdev->layer[volayer].chs[i].src_type >= VO_SRC_VDVP)
  {
    SAMPLE_COMM_VDEC_Stop(vdev->layer[volayer].cnt);
  }
  
  vdev->layer[volayer].cnt = layout;
  SAMPLE_COMM_VO_StartChn(volayer, vdev->layer[volayer].cnt);
  
  pthread_mutex_unlock(&vdev->lock);
  
  if(0) //test RGN for VO;
  {
      HI_S32             s32Ret;
      HI_S32             HandleNum;
      RGN_TYPE_E         enType;
      MPP_CHN_S          stChn;
      
      HandleNum = 1;
      enType = COVEREX_RGN;
      
      for(i = 0; i < 2; i++)
      {
        stChn.enModId = HI_ID_VO;
        stChn.s32DevId = 0;
        stChn.s32ChnId = i;
        SAMPLE_COMM_REGION_DetachFrmChn(HandleNum, enType, &stChn);
      }
      
      SAMPLE_COMM_REGION_Destroy(HandleNum, enType);
      
      s32Ret = SAMPLE_COMM_REGION_Create(HandleNum,enType);
      if(HI_SUCCESS != s32Ret)
      {
          SAMPLE_PRT("SAMPLE_COMM_REGION_Create failed!\n");
      }
      
      for(i = 0; i < 2; i++)
      {
        stChn.enModId = HI_ID_VO;
        stChn.s32DevId = 0;
        stChn.s32ChnId = i;
        
        s32Ret = SAMPLE_COMM_REGION_AttachToChn(HandleNum,enType,&stChn);
        if(HI_SUCCESS != s32Ret)
        {
            SAMPLE_PRT("SAMPLE_COMM_REGION_AttachToChn failed!\n");
        }
      }   
  }
 
  return err;
}


//发送视频数据到指定ch;
int gsf_mpp_vo_vsend(int volayer, int ch, int flag, char *data, gsf_mpp_frm_attr_t *attr)
{
  int err = 0;
  HI_S32 s32Ret = HI_SUCCESS;
  
  vo_mng_t *vdev = &vo_mng[layer2vdev[volayer]];
  
  if(vdev->layer[volayer].cnt == VO_LAYOUT_NONE
    || attr->width == 0
    || attr->height == 0
    || (attr->etype != PT_H264 
        && attr->etype != PT_H265 
        && attr->etype != PT_MP4VIDEO
        && attr->etype != PT_MJPEG
        && attr->etype != PT_JPEG))
  {
    return -1;
  }
  
  pthread_mutex_lock(&vdev->lock);
  
  if(vdev->layer[volayer].chs[ch].width != attr->width 
    || vdev->layer[volayer].chs[ch].height != attr->height
    || vdev->layer[volayer].chs[ch].etype != attr->etype)
  {
    vdev->layer[volayer].chs[ch].width  = attr->width;
    vdev->layer[volayer].chs[ch].height = attr->height;
    vdev->layer[volayer].chs[ch].etype  = attr->etype;
    vdev->layer[volayer].chs[ch].src_type = VO_SRC_VDVP;
    
    int vpss_grp = vdev->layer[volayer].chs[ch].src_grp;
    int vpss_chn = vdev->layer[volayer].chs[ch].src_chn;
    
    // VOLAYER_HD0: [0 - VPSS_MAX_GRP_NUM/2]
    // VOLAYER_PIP: [VPSS_MAX_GRP_NUM/2 - VPSS_MAX_GRP_NUM];
    vdev->layer[volayer].chs[ch].src_grp = VPSS_MAX_GRP_NUM/2 + ch;
    vdev->layer[volayer].chs[ch].src_chn = VPSS_CHN0;
    
    // unbind vo && vpss && vdec;
    if(vpss_grp != -1)
    {
      SAMPLE_COMM_VPSS_UnBind_VO(vpss_grp, VPSS_CHN0, volayer, ch);
      SAMPLE_COMM_VDEC_UnBind_VPSS(ch, vpss_grp);
      
      // stop  vpss;
      HI_BOOL chen[VPSS_MAX_PHY_CHN_NUM] = {HI_TRUE,HI_TRUE};
      s32Ret = SAMPLE_COMM_VPSS_Stop(vpss_grp, chen);

      // stop  vdec;
      HI_MPI_VDEC_StopRecvStream(ch);
      HI_MPI_VDEC_DestroyChn(ch);
    }
    // reset vpss_grp,vpss_chn;
    vpss_grp = vdev->layer[volayer].chs[ch].src_grp;
    vpss_chn = vdev->layer[volayer].chs[ch].src_chn;
    
    // start vdec;
    if(1)
    {
    
    printf("%s => start vdec ch:%d, etype:%d, width:%d, height:%d\n", __func__, ch, attr->etype, attr->width, attr->height);
    
    VDEC_CHN_ATTR_S stChnAttr;
    VDEC_CHN_POOL_S stPool;
    VDEC_CHN_PARAM_S stChnParam;

    stChnAttr.enType           = attr->etype;
    stChnAttr.enMode           = VIDEO_MODE_FRAME;
    stChnAttr.u32PicWidth      = attr->width;
    stChnAttr.u32PicHeight     = attr->height;
    stChnAttr.u32StreamBufSize = attr->width * attr->height;
    stChnAttr.u32FrameBufCnt   = 3 /*u32RefFrameNum*/ + 2 /*u32DisplayFrameNum*/ + 1;

    if (PT_H264 == stChnAttr.enType || PT_H265 ==stChnAttr.enType)
    {
        stChnAttr.stVdecVideoAttr.u32RefFrameNum     = 3;
        stChnAttr.stVdecVideoAttr.bTemporalMvpEnable = 1; // VIDEO_DEC_MODE_IPB
        stChnAttr.u32FrameBufSize  = VDEC_GetPicBufferSize(stChnAttr.enType
                                          , stChnAttr.u32PicWidth
                                          , stChnAttr.u32PicHeight
                                          , PIXEL_FORMAT_YVU_SEMIPLANAR_420
                                          , DATA_BITWIDTH_8, 0);
    }
    else if (PT_JPEG == stChnAttr.enType || PT_MJPEG == stChnAttr.enType)
    {
        stChnAttr.enMode  = VIDEO_MODE_FRAME;
        stChnAttr.u32FrameBufSize  = VDEC_GetPicBufferSize(stChnAttr.enType
                                          , stChnAttr.u32PicWidth
                                          , stChnAttr.u32PicHeight
                                          , PIXEL_FORMAT_YVU_SEMIPLANAR_420
                                          , DATA_BITWIDTH_8, 0);
    }

    CHECK_CHN_RET(HI_MPI_VDEC_CreateChn(ch, &stChnAttr), ch, "HI_MPI_VDEC_CreateChn");

    if (VB_SOURCE_USER == g_enVdecVBSource)
    {
        stPool.hPicVbPool = g_ahPicVbPool[ch];
        stPool.hTmvVbPool = g_ahTmvVbPool[ch];
        CHECK_CHN_RET(HI_MPI_VDEC_AttachVbPool(ch, &stPool), ch, "HI_MPI_VDEC_AttachVbPool");
    }

    CHECK_CHN_RET(HI_MPI_VDEC_GetChnParam(ch, &stChnParam), ch, "HI_MPI_VDEC_GetChnParam");
    if (PT_H264 == stChnAttr.enType || PT_H265 == stChnAttr.enType)
    {
        stChnParam.stVdecVideoParam.enDecMode         = VIDEO_DEC_MODE_IP;//16dv300 !!!VIDEO_DEC_MODE_IPB;
        stChnParam.stVdecVideoParam.enCompressMode    = COMPRESS_MODE_NONE;//16dv300 !!!COMPRESS_MODE_TILE;
        stChnParam.stVdecVideoParam.enVideoFormat     = VIDEO_FORMAT_TILE_64x16;
        stChnParam.stVdecVideoParam.enOutputOrder     = VIDEO_OUTPUT_ORDER_DISP; //VIDEO_OUTPUT_ORDER_DEC
    }
    else
    {
        stChnParam.stVdecPictureParam.enPixelFormat   = PIXEL_FORMAT_YVU_SEMIPLANAR_420;
        stChnParam.stVdecPictureParam.u32Alpha        = 255;
    }
    stChnParam.u32DisplayFrameNum = 2;
    CHECK_CHN_RET(HI_MPI_VDEC_SetChnParam(ch, &stChnParam), ch, "HI_MPI_VDEC_SetChnParam");
    CHECK_CHN_RET(HI_MPI_VDEC_StartRecvStream(ch), ch, "HI_MPI_VDEC_StartRecvStream");
    
    // 
    HI_MPI_VDEC_SetDisplayMode(ch, VIDEO_DISPLAY_MODE_PREVIEW);
    }

    // start vpss;
    if(0)
    {
      VPSS_GRP_ATTR_S stGrpAttr = {0};
      VPSS_CHN_ATTR_S stChnAttr = {0};
      
      /*** create vpss group ***/
      stGrpAttr.u32MaxW = ALIGN_UP(attr->width,  2);
      stGrpAttr.u32MaxH = ALIGN_UP(attr->height, 2);
      stGrpAttr.stFrameRate.s32SrcFrameRate = -1;
      stGrpAttr.stFrameRate.s32DstFrameRate = -1;
      stGrpAttr.enDynamicRange = DYNAMIC_RANGE_SDR8;
      stGrpAttr.enPixelFormat  = PIXEL_FORMAT_YVU_SEMIPLANAR_420;
      stGrpAttr.bNrEn   = HI_FALSE;
      s32Ret = HI_MPI_VPSS_CreateGrp(ch, &stGrpAttr);

      
      /*** enable vpss chn, with frame ***/
      stChnAttr.u32Width                    = ALIGN_UP(attr->width,  2);
      stChnAttr.u32Height                   = ALIGN_UP(attr->height, 2);
      stChnAttr.enChnMode                   = VPSS_CHN_MODE_USER;//VPSS_CHN_MODE_AUTO; //VPSS_CHN_MODE_USER - 1920x1088; //
      stChnAttr.enCompressMode              = COMPRESS_MODE_NONE;//; //COMPRESS_MODE_SEG // HI_MPI_VPSS_GetChnFrame;
      stChnAttr.enDynamicRange              = DYNAMIC_RANGE_SDR8;
      stChnAttr.enPixelFormat               = PIXEL_FORMAT_YVU_SEMIPLANAR_420;
      stChnAttr.stFrameRate.s32SrcFrameRate = -1;
      stChnAttr.stFrameRate.s32DstFrameRate = -1;
      stChnAttr.u32Depth                    = 1; // 0; HI_MPI_VPSS_GetChnFrame;
      stChnAttr.bMirror                     = HI_FALSE;
      stChnAttr.bFlip                       = HI_FALSE;
      stChnAttr.stAspectRatio.enMode        = ASPECT_RATIO_NONE;
      stChnAttr.enVideoFormat               = VIDEO_FORMAT_LINEAR;
      
      s32Ret = HI_MPI_VPSS_SetChnAttr(vpss_grp, vpss_chn, &stChnAttr);
      s32Ret = HI_MPI_VPSS_EnableChn(vpss_grp, vpss_chn);
      s32Ret = HI_MPI_VPSS_StartGrp(vpss_grp);
    }
    else
    {
      VPSS_GRP_ATTR_S stGrpAttr = {0};
      VPSS_CHN_ATTR_S stChnAttr[VPSS_MAX_PHY_CHN_NUM] = {0};
      
      /*** create vpss group ***/
      stGrpAttr.u32MaxW = ALIGN_UP(attr->width,  2);
      stGrpAttr.u32MaxH = ALIGN_UP(attr->height, 2);
      stGrpAttr.stFrameRate.s32SrcFrameRate = -1;
      stGrpAttr.stFrameRate.s32DstFrameRate = -1;
      stGrpAttr.enDynamicRange = DYNAMIC_RANGE_SDR8;
      stGrpAttr.enPixelFormat  = PIXEL_FORMAT_YVU_SEMIPLANAR_420;
      stGrpAttr.bNrEn   = HI_FALSE;
        
      /*** enable vpss chn, with frame ***/
      int i;
      for(i = 0; i < 2; i++)
      {
        stChnAttr[i].u32Width                    = ALIGN_UP((i == 0)?attr->width:640,  2);
        stChnAttr[i].u32Height                   = ALIGN_UP((i == 0)?attr->height:480, 2);
        stChnAttr[i].enChnMode                   = VPSS_CHN_MODE_USER; //VPSS_CHN_MODE_AUTO; //VPSS_CHN_MODE_USER - 1920x1088; //
        stChnAttr[i].enCompressMode              = COMPRESS_MODE_NONE;//; //COMPRESS_MODE_SEG // HI_MPI_VPSS_GetChnFrame;
        stChnAttr[i].enDynamicRange              = DYNAMIC_RANGE_SDR8;
        stChnAttr[i].enPixelFormat               = PIXEL_FORMAT_YVU_SEMIPLANAR_420;
        stChnAttr[i].stFrameRate.s32SrcFrameRate = -1;
        stChnAttr[i].stFrameRate.s32DstFrameRate = -1;
        stChnAttr[i].u32Depth                    = 1; // 0; HI_MPI_VPSS_GetChnFrame;
        stChnAttr[i].bMirror                     = HI_FALSE;
        stChnAttr[i].bFlip                       = HI_FALSE;
        stChnAttr[i].stAspectRatio.enMode        = ASPECT_RATIO_NONE;
        stChnAttr[i].enVideoFormat               = VIDEO_FORMAT_LINEAR;
      }
      
      HI_BOOL chen[VPSS_MAX_PHY_CHN_NUM] = {HI_TRUE,HI_TRUE};
      s32Ret = SAMPLE_COMM_VPSS_Start(vpss_grp, chen, &stGrpAttr, stChnAttr);
      
      if(1) //VPSS-LowDelay
      {
        VPSS_LOW_DELAY_INFO_S  stLowDelayInfo;
        stLowDelayInfo.bEnable = HI_TRUE;//HI_FALSE;
        stLowDelayInfo.u32LineCnt = 16*30;
        s32Ret = HI_MPI_VPSS_SetLowDelayAttr(vpss_grp, 0, &stLowDelayInfo);
        if(s32Ret != HI_SUCCESS)
        {
            SAMPLE_PRT("HI_MPI_VPSS_SetLowDelayAttr failed %#x!\n", s32Ret);
        }
      }
    }
    // bind  vdec && vpss && vo;
    s32Ret = SAMPLE_COMM_VDEC_Bind_VPSS(ch, vpss_grp);
    s32Ret = SAMPLE_COMM_VPSS_Bind_VO(vpss_grp, vpss_chn, volayer, ch);
  }
  
  pthread_mutex_unlock(&vdev->lock);
  
  // send vdec;
  VDEC_STREAM_S stStream = {0};
  stStream.u64PTS  = attr->pts;//0;
  stStream.pu8Addr = data;
  stStream.u32Len  = attr->size;
  stStream.bEndOfFrame  = HI_TRUE;
  stStream.bEndOfStream = HI_FALSE;
  stStream.bDisplay = 1;
  s32Ret = HI_MPI_VDEC_SendStream(ch, &stStream, 0);
  //printf("HI_MPI_VDEC_SendStream ch:%d, ret:0x%x\n", ch, s32Ret);
  return err;
}


int gsf_mpp_ao_asend(int aodev, int ch, int flag, char *data, gsf_mpp_frm_attr_t *attr)
{
  if(!_audio_init)
  {
    _audio_init = 1;
    extern HI_S32 sample_audio_init();
    sample_audio_init();
  }
  
  HI_S32 s32Ret = 0;
  HI_S32 adch = 0;
    
  static gsf_mpp_frm_attr_t _attr = {.etype = PT_BUTT, };
  
  if(_attr.etype != attr->etype)
  {
    //reset;
    _attr = *attr;
    SAMPLE_AUDIO_AdecAo(attr);
  }
  
  if(attr->size == 0)
  {
    return HI_MPI_ADEC_SendEndOfStream(adch, HI_FALSE);
  }
  
  AUDIO_STREAM_S stAudioStream;
  stAudioStream.pStream = data;
  stAudioStream.u32Len = attr->size;
  stAudioStream.u64TimeStamp = 0;
  stAudioStream.u32Seq = 0;
  
  /* here only demo adec streaming sending mode, but pack sending mode is commended */
  s32Ret = HI_MPI_ADEC_SendStream(adch, &stAudioStream, flag);
  if (HI_SUCCESS != s32Ret)
  {
    printf("%s: HI_MPI_ADEC_SendStream(%d) failed \n", \
           __FUNCTION__, adch, stAudioStream.u32Len);
  }
  else
  {
    //printf("%s: HI_MPI_ADEC_SendStream(%d) OK u32Len:%d, u64TimeStamp:%llu\n", \
    //       __FUNCTION__, adch, stAudioStream.u32Len, stAudioStream.u64TimeStamp);
  }

  return s32Ret;
}

#include "audio_aac_adp.h"

int gsf_mpp_ao_bind(int aodev, int ch, int aidev, int aich)
{
  //ao bind ai;
  HI_S32      s32Ret;
  AI_CHN      AiChn = aich;
  AO_CHN      AoChn = ch;
  HI_S32      s32AoChnCnt;

  AIO_ATTR_S stAioAttr;
  
  AUDIO_DEV   AiDev = aidev;//SAMPLE_AUDIO_INNER_AI_DEV;
  AUDIO_DEV   AoDev = aodev;//SAMPLE_AUDIO_INNER_AO_DEV; SAMPLE_AUDIO_INNER_HDMI_AO_DEV;

  HI_BOOL bAioReSample = (AoDev == SAMPLE_AUDIO_INNER_AO_DEV)?0:
                         (_aenc.sp != AUDIO_SAMPLE_RATE_48000)?1:0;
  AUDIO_SAMPLE_RATE_E enInSampleRate = _aenc.sp;
  
  if(bAioReSample && AoDev != SAMPLE_AUDIO_INNER_AO_DEV)
    return 0;

  stAioAttr.enSamplerate   = (AoDev == SAMPLE_AUDIO_INNER_AO_DEV)?_aenc.sp:AUDIO_SAMPLE_RATE_48000;
  stAioAttr.enBitwidth     = AUDIO_BIT_WIDTH_16;
  stAioAttr.enWorkmode     = AIO_MODE_I2S_MASTER;
  stAioAttr.enSoundmode    = _aenc.stereo;//AUDIO_SOUND_MODE_STEREO;
  stAioAttr.u32EXFlag      = 0;
  stAioAttr.u32FrmNum      = 30;
  stAioAttr.u32PtNumPerFrm = (_aenc.enPayLoad==PT_AAC)?AACLC_SAMPLES_PER_FRAME:80*6;//AACLC_SAMPLES_PER_FRAME;
  stAioAttr.u32ChnCnt      = 1<<_aenc.stereo;
  stAioAttr.u32ClkSel      = 0;
  stAioAttr.enI2sType      = (AoDev == SAMPLE_AUDIO_INNER_AO_DEV)?AIO_I2STYPE_INNERCODEC:AIO_I2STYPE_INNERHDMI;

  s32AoChnCnt = stAioAttr.u32ChnCnt;
  s32Ret = SAMPLE_COMM_AUDIO_StartAo(AoDev, s32AoChnCnt, &stAioAttr, enInSampleRate, bAioReSample);
  if (s32Ret != HI_SUCCESS)
  {
    printf("SAMPLE_COMM_AUDIO_StartAo err:%d, AoDev:%d, s32AoChnCnt:%d\n", s32Ret, AoDev, s32AoChnCnt);
    return s32Ret;
  }
  
  s32Ret = SAMPLE_COMM_AUDIO_AoBindAi(AiDev, AiChn, AoDev, AoChn);
  if (s32Ret != HI_SUCCESS)
  {
    printf("SAMPLE_COMM_AUDIO_AoBindAi err:%d, AoDev:%d, AiDev:%d\n", s32Ret, AoDev, AiDev);
    goto __err;
  }

  return 0;
  
__err:
  s32Ret = SAMPLE_COMM_AUDIO_StopAo(AoDev, s32AoChnCnt, bAioReSample);
  if (s32Ret != HI_SUCCESS)
  {
    printf("SAMPLE_COMM_AUDIO_StopAo err:%d, AoDev:%d, s32AoChnCnt:%d\n", s32Ret, AoDev, s32AoChnCnt);
  }
  return -1;
}


//清除解码显示BUFF
int gsf_mpp_vo_clear(int volayer, int ch)
{
  int err = 0;
  
  #if 0
  /* send the flag of stream end */
  VDEC_STREAM_S stStream = {0};
  stStream.bEndOfStream = HI_TRUE;
  err = HI_MPI_VDEC_SendStream(ch, &stStream, -1);
  printf("HI_MPI_VDEC_SendStream err:%d, ch:%d\n", err, ch);
  #endif

  // reset vdec;
  err = HI_MPI_VDEC_StopRecvStream(ch); 
  err = HI_MPI_VDEC_ResetChn(ch);
  err = HI_MPI_VDEC_StartRecvStream(ch);
  
  // clear vo;
  HI_BOOL bClearAll = HI_TRUE;
  err = HI_MPI_VO_ClearChnBuf(volayer, ch, bClearAll);
  printf("HI_MPI_VO_ClearChnBuffer err:%d, ch:%d\n", err, ch);
  
  return err;
}


// VO-BIND-VPSS;
int gsf_mpp_vo_bind(int volayer, int ch, gsf_mpp_vo_src_t *src)
{
  HI_S32 s32Ret = HI_SUCCESS;
  vo_mng_t *vdev = &vo_mng[layer2vdev[volayer]];
   
  pthread_mutex_lock(&vdev->lock);

  // unbind vo && vpss && vdec;
  int vpss_grp = vdev->layer[volayer].chs[ch].src_grp;
  int vpss_chn = vdev->layer[volayer].chs[ch].src_chn;
  
  if(vpss_grp != -1)
  {
    SAMPLE_COMM_VPSS_UnBind_VO(vpss_grp, vpss_chn, volayer, ch);
    if(vdev->layer[volayer].chs[ch].src_type >= VO_SRC_VDVP)
    {
      // stop vpss;
      SAMPLE_COMM_VDEC_UnBind_VPSS(ch, vpss_grp);
      HI_BOOL chen[VPSS_MAX_PHY_CHN_NUM] = {HI_TRUE,HI_TRUE};
      SAMPLE_COMM_VPSS_Stop(vpss_grp, chen);
      //stop vdec;
      HI_MPI_VDEC_StopRecvStream(ch);
      HI_MPI_VDEC_DestroyChn(ch);
    }
  }
  // only set width = height = 0, tell gsf_mpp_vo_vsend to recreate vdec&vpss;
  vdev->layer[volayer].chs[ch].width = vdev->layer[volayer].chs[ch].height = 0;
  vdev->layer[volayer].chs[ch].src_grp = src->VpssGrp;
  vdev->layer[volayer].chs[ch].src_chn = src->VpssChn;
  vdev->layer[volayer].chs[ch].src_type = VO_SRC_VP;
  s32Ret = SAMPLE_COMM_VPSS_Bind_VO(src->VpssGrp, src->VpssChn, volayer, ch);
  
  pthread_mutex_unlock(&vdev->lock);
  return 0;
}



//裁剪通道源图像区域到显示通道(局部放大)
int gsf_mpp_vo_crop(int volayer, int ch, RECT_S *rect)
{
  HI_S32 s32Ret = HI_SUCCESS;
  VO_ZOOM_ATTR_S stZoomWindow;
  #if 0
  stZoomWindow.enZoomType = VO_ZOOM_IN_RECT;
  stZoomWindow.stZoomRect.s32X =  ALIGN_UP(rect->s32X, 2);
  stZoomWindow.stZoomRect.s32Y =  ALIGN_UP(rect->s32Y, 2);
  stZoomWindow.stZoomRect.u32Width = ALIGN_UP(rect->u32Width, 2);
  stZoomWindow.stZoomRect.u32Height = ALIGN_UP(rect->u32Height, 2);
  #else
  stZoomWindow.enZoomType = VO_ZOOM_IN_RATIO;
  stZoomWindow.stZoomRatio.u32XRatio = rect->s32X;
  stZoomWindow.stZoomRatio.u32YRatio = rect->s32Y;
  stZoomWindow.stZoomRatio.u32WRatio = rect->u32Width;
  stZoomWindow.stZoomRatio.u32HRatio = rect->u32Height;
  #endif
  
  /* set zoom window */
  s32Ret = HI_MPI_VO_SetZoomInWindow(volayer, ch, &stZoomWindow);
  if (s32Ret != HI_SUCCESS)
  {
    printf("Set zoom attribute, s32Ret:%#x, u32XRatio[%d,%d,%d,%d]\n"
        , s32Ret
        , stZoomWindow.stZoomRatio.u32XRatio, stZoomWindow.stZoomRatio.u32YRatio
        , stZoomWindow.stZoomRatio.u32WRatio, stZoomWindow.stZoomRatio.u32HRatio);
    return HI_FAILURE;
  }

  return s32Ret;
}


int gsf_mpp_vo_aspect(int volayer, int ch, RECT_S *rect)
{
    HI_S32 s32Ret = HI_SUCCESS;
    VO_CHN_ATTR_S stChnAttr;
  
    s32Ret = HI_MPI_VO_GetChnAttr(volayer, ch, &stChnAttr);
    if (s32Ret != HI_SUCCESS)
    {
      printf("%s(%d):failed with %#x!\n", \
             __FUNCTION__, __LINE__,  s32Ret);
      return HI_FAILURE;
    }
    
    int l = stChnAttr.stRect.s32X;
    int t = stChnAttr.stRect.s32Y;
    int r = stChnAttr.stRect.s32X + stChnAttr.stRect.u32Width;
    int b = stChnAttr.stRect.s32Y + stChnAttr.stRect.u32Height;
    
    
    rect->s32X = (l+rect->s32X < r)?l+rect->s32X:l;
    rect->s32Y = (t+rect->s32Y < b)?t+rect->s32Y:t;
    rect->u32Width = (rect->s32X+rect->u32Width < r)?rect->u32Width:r-rect->s32X;
    rect->u32Height= (rect->s32Y+rect->u32Height < b)?rect->u32Height:b-rect->s32Y;
    
    stChnAttr.stRect.s32X       = rect->s32X;
    stChnAttr.stRect.s32Y       = rect->s32Y;
    stChnAttr.stRect.u32Width   = rect->u32Width;
    stChnAttr.stRect.u32Height  = rect->u32Height;
    stChnAttr.u32Priority       = 0;
    stChnAttr.bDeflicker        = HI_FALSE;

    s32Ret = HI_MPI_VO_SetChnAttr(volayer, ch, &stChnAttr);
    if (s32Ret != HI_SUCCESS)
    {
        printf("%s(%d):failed with %#x!\n", \
               __FUNCTION__, __LINE__,  s32Ret);
        return HI_FAILURE;
    }
    
    return s32Ret;
}

int gsf_mpp_vo_rect(int volayer, int ch, RECT_S *rect, int priority)
{
    HI_S32 s32Ret = HI_SUCCESS;
    VO_CHN_ATTR_S stChnAttr;
 
    if(rect->u32Width == 0 || rect->u32Height == 0)
    {
      s32Ret = HI_MPI_VO_HideChn(volayer, ch);
      return s32Ret;
    }
    else 
    {
      s32Ret = HI_MPI_VO_ShowChn(volayer, ch);
    }

    vo_mng_t *vdev = &vo_mng[layer2vdev[volayer]];
    
    int l = vdev->layer[volayer].rect.s32X;
    int t = vdev->layer[volayer].rect.s32Y;
    int r = vdev->layer[volayer].rect.u32Width;
    int b = vdev->layer[volayer].rect.u32Height;
    
    rect->s32X = (l+rect->s32X < r)?l+rect->s32X:l;
    rect->s32Y = (t+rect->s32Y < b)?t+rect->s32Y:t;
    rect->u32Width = (rect->s32X+rect->u32Width < r)?rect->u32Width:r-rect->s32X;
    rect->u32Height= (rect->s32Y+rect->u32Height < b)?rect->u32Height:b-rect->s32Y;
    
    stChnAttr.stRect.s32X       = rect->s32X;
    stChnAttr.stRect.s32Y       = rect->s32Y;
    stChnAttr.stRect.u32Width   = rect->u32Width;
    stChnAttr.stRect.u32Height  = rect->u32Height;
    stChnAttr.u32Priority       = priority;
    stChnAttr.bDeflicker        = HI_FALSE;

    s32Ret = HI_MPI_VO_SetChnAttr(volayer, ch, &stChnAttr);
    if (s32Ret != HI_SUCCESS)
    {
        printf("%s(%d):failed with %#x!\n", \
               __FUNCTION__, __LINE__,  s32Ret);
        return HI_FAILURE;
    }
    
    return s32Ret;
}


static struct fb_bitfield s_r16 = {10, 5, 0};
static struct fb_bitfield s_g16 = {5, 5, 0};
static struct fb_bitfield s_b16 = {0, 5, 0};
static struct fb_bitfield s_a16 = {15, 1, 0};

static struct fb_bitfield s_a32 = {24,8,0};
static struct fb_bitfield s_r32 = {16,8,0};
static struct fb_bitfield s_g32 = {8,8,0};
static struct fb_bitfield s_b32 = {0,8,0};


static int vo_fd[VOFB_BUTT];


int gsf_mpp_fb_start(int vofb, VO_INTF_SYNC_E sync, int hide)
{
    int i = 0, j = 0, x = 0, y = 0;
    HI_CHAR file[32] = "/dev/fb0";
    
    switch (vofb)
    {
        case VOFB_GUI:
            strcpy(file, "/dev/fb0");
            break;
        case VOFB_GUI1:
            strcpy(file, "/dev/fb1");
            break;
        case VOFB_MOUSE:
            strcpy(file, "/dev/fb2");
            break;
        default:
            strcpy(file, "/dev/fb0");
            break;
    }
  
    /* 1. open framebuffer device overlay 0 */
    int fd = open(file, O_RDWR, 0);
    if(fd < 0)
    {
        SAMPLE_PRT("open %s failed!\n",file);
        return -1;
    } 

    vo_csc();

    HI_BOOL bShow = HI_FALSE;
    if (ioctl(fd, FBIOPUT_SHOW_HIFB, &bShow) < 0)
    {
        SAMPLE_PRT("FBIOPUT_SHOW_HIFB failed!\n");
        return -1;
    }
    #if 0
    HIFB_CAPABILITY_S stCap;
    if ( ioctl(fd, FBIOGET_CAPABILITY_HIFB, &stCap) < 0)
    {
        SAMPLE_PRT("FBIOGET_CAPABILITY_HIFB failed!\n");
        return -1;
    }
    #endif
    /* 2. set the screen original position */
    HIFB_POINT_S stPoint = {0, 0};
    stPoint.s32XPos = 0;
    stPoint.s32YPos = 0;

    if (ioctl(fd, FBIOPUT_SCREEN_ORIGIN_HIFB, &stPoint) < 0)
    {
        SAMPLE_PRT("set screen original show position failed!\n");
        close(fd);
        return -1;
    }

    /* 3. get the variable screen info */
    struct fb_var_screeninfo var;
    if (ioctl(fd, FBIOGET_VSCREENINFO, &var) < 0)
    {
        SAMPLE_PRT("Get variable screen info failed!\n");
		    close(fd);
        return -1;
    }
    
    int u32Width = 1280;
    int u32Height = 720;
    int u32VoFrmRate = 60;
    SAMPLE_COMM_VO_GetWH(sync, &u32Width, &u32Height, &u32VoFrmRate);
    printf("vofb:%d, u32Width:%d, u32Height:%d\n", vofb, u32Width, u32Height);
    switch (vofb)
    {
        case VOFB_GUI:
        case VOFB_GUI1:
            var.xres_virtual = u32Width;
            var.yres_virtual = u32Height;//u32Height*2;
            var.xres = u32Width;
            var.yres = u32Height;
            break;
        case VOFB_MOUSE:
            var.xres_virtual = 48;
            var.yres_virtual = 48;
            var.xres = 48;
            var.yres = 48;
            break;
        default:
            break;
    }
#if 0 //maohw;
    #warning "FB_COLOR_DEPTH == 32"
    var.transp= s_a32;
    var.red   = s_r32;
    var.green = s_g32;
    var.blue  = s_b32;
    var.bits_per_pixel = 32;
#else
    #warning "FB_COLOR_DEPTH == 16"
    var.transp= s_a16;
    var.red   = s_r16;
    var.green = s_g16;
    var.blue  = s_b16;
    var.bits_per_pixel = 16;
#endif
    var.activate = FB_ACTIVATE_NOW;
    
    /* 5. set the variable screeninfo */
    if (ioctl(fd, FBIOPUT_VSCREENINFO, &var) < 0)
    {
        SAMPLE_PRT("Put variable screen info failed!\n");
		    close(fd);
        return -1;
    }
     
    /* 6. get the fix screen info */
    struct fb_fix_screeninfo fix;
    if (ioctl(fd, FBIOGET_FSCREENINFO, &fix) < 0)
    {
        SAMPLE_PRT("Get fix screen info failed!\n");
		    close(fd);
        return -1;
    }
    int u32FixScreenStride = fix.line_length;   /*fix screen stride*/
    
    HIFB_ALPHA_S stAlpha={0};
    if (ioctl(fd, FBIOGET_ALPHA_HIFB,  &stAlpha))
    {
        SAMPLE_PRT("Get alpha failed!\n");
        close(fd);
        return -1;
    }
    stAlpha.bAlphaEnable = HI_TRUE;
    stAlpha.bAlphaChannel = HI_FALSE;
    stAlpha.u8Alpha0 = 0x0;//0xff; // 当最高位为0时,选择该值作为Alpha
    stAlpha.u8Alpha1 = 0xff;//0x0; // 当最高位为1时,选择该值作为Alpha
    stAlpha.u8GlobalAlpha = 0x0;//在Alpha通道使能时起作用
    
    if (ioctl(fd, FBIOPUT_ALPHA_HIFB,  &stAlpha) < 0)
    {
        SAMPLE_PRT("Set alpha failed!\n");
        close(fd);
        return -1;
    }

    HIFB_COLORKEY_S stColorKey;
    stColorKey.bKeyEnable = HI_FALSE;
    stColorKey.u32Key = 0x0000;
    if (ioctl(fd, FBIOPUT_COLORKEY_HIFB, &stColorKey) < 0)
    {
        SAMPLE_PRT("FBIOPUT_COLORKEY_HIFB failed!\n");
        close(fd);
        return -1;
    }

    /* 7. map the physical video memory for user use */
    HI_U8 *pShowScreen = mmap(HI_NULL, fix.smem_len, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
    if(MAP_FAILED == pShowScreen)
    {
        SAMPLE_PRT("mmap framebuffer failed!\n");
		    close(fd);
        return -1;
    }
    
    //vo_csc();
    memset(pShowScreen, 0x00, fix.smem_len);
    
    #if 0
    void *ptemp;
    ptemp = (pShowScreen + 100*u32FixScreenStride);
    for (y = 0; y < 100; y++)
    {
        for (x = 0; x < 800; x++)
        {
          *((HI_U32*)ptemp + y*u32FixScreenStride/4 + x) = 0xFFff0000;
        }
    }
    
    ptemp = (pShowScreen + 200*u32FixScreenStride);
    for (y = 0; y < 100; y++)
    {
        for (x = 0; x < 800; x++)
        {
          *((HI_U32*)ptemp + y*u32FixScreenStride/4 + x) = 0xFF00ff00;
        }
    }
    
    ptemp = (pShowScreen + 300*u32FixScreenStride);
    for (y = 0; y < 100; y++)
    {
        for (x = 0; x < 800; x++)
        {
          *((HI_U32*)ptemp + y*u32FixScreenStride/4 + x) = 0xFF0000ff;
        }
    }
    #endif
    
    munmap(pShowScreen, fix.smem_len);
    
    if(!hide)
    {
      bShow = HI_TRUE;
      if (ioctl(fd, FBIOPUT_SHOW_HIFB, &bShow) < 0)
      {
          SAMPLE_PRT("FBIOPUT_SHOW_HIFB failed!\n");
          close(fd);
          return -1;
      }
    }
    
    vo_fd[vofb] = fd;//close(fd);
    return 0;
}

