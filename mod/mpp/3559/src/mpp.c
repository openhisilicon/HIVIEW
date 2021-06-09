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
                                                      /*sns-lan-wdr-res-fps*/
    {SONY_IMX327_MIPI_2M_30FPS_12BIT,                   "imx327-0-0-2-30", "libsns_imx327.so", "stSnsImx327Obj"},
    {SONY_IMX327_MIPI_2M_30FPS_12BIT_WDR2TO1,           "imx327-0-1-2-30", "libsns_imx327.so", "stSnsImx327Obj"},
    {SONY_IMX327_2L_MIPI_2M_30FPS_12BIT,                "imx327-2-0-2-30", "libsns_imx327_2l.so", "stSnsImx327_2l_Obj"},
    {SONY_IMX327_2L_MIPI_2M_30FPS_12BIT_WDR2TO1,        "imx327-2-1-2-30", "libsns_imx327_2l.so", "stSnsImx327_2l_Obj"},
    {SONY_IMX390_MIPI_2M_30FPS_12BIT,                   "imx390-0-0-2-30", "libsns_imx390.so", "stSnsImx390Obj"},
    {SONY_IMX307_MIPI_2M_30FPS_12BIT,                   "imx307-0-0-2-30", "libsns_imx307.so", "stSnsImx307Obj"},
    {SONY_IMX307_MIPI_2M_30FPS_12BIT_WDR2TO1,           "imx307-0-1-2-30", "libsns_imx307.so", "stSnsImx307Obj"},
    {SONY_IMX307_2L_MIPI_2M_30FPS_12BIT,                "imx307-2-0-2-30", "libsns_imx307_2l.so", "stSnsImx307_2l_Obj"},
    {SONY_IMX307_2L_MIPI_2M_30FPS_12BIT_WDR2TO1,        "imx307-2-1-2-30", "libsns_imx307_2l.so", "stSnsImx307_2l_Obj"},
    {SONY_IMX307_2L_SLAVE_MIPI_2M_30FPS_12BIT,          "imx307-3-0-2-30", "libsns_imx307_2l_slave.so", "g_stSnsImx307_2l_Slave_Obj"},
    {SONY_IMX307_2L_SLAVE_MIPI_2M_30FPS_12BIT_WDR2TO1,  "imx307-3-1-2-30", "libsns_imx307_2l_slave.so", "g_stSnsImx307_2l_Slave_Obj"},
    {SONY_IMX335_MIPI_5M_30FPS_12BIT,                   "imx335-0-0-5-30", "libsns_imx335.so", "stSnsImx335Obj"},
    {SONY_IMX335_MIPI_5M_30FPS_10BIT_WDR2TO1,           "imx335-0-1-5-30", "libsns_imx335.so", "stSnsImx335Obj"},
    {SONY_IMX335_MIPI_4M_30FPS_12BIT,                   "imx335-0-0-4-30", "libsns_imx335.so", "stSnsImx335Obj"},
    {SONY_IMX335_MIPI_4M_30FPS_10BIT_WDR2TO1,           "imx335-0-1-4-30", "libsns_imx335.so", "stSnsImx335Obj"},
    {SONY_IMX458_MIPI_8M_30FPS_10BIT,                   "imx458-0-0-8-30", "libsns_imx458.so", "stSnsImx458Obj"},
    {SONY_IMX458_MIPI_12M_20FPS_10BIT,                  "imx458-0-0-12-30","libsns_imx458.so", "stSnsImx458Obj"},
    {SONY_IMX458_MIPI_4M_60FPS_10BIT,                   "imx458-0-0-4-60", "libsns_imx458.so", "stSnsImx458Obj"},
    {SONY_IMX458_MIPI_4M_40FPS_10BIT,                   "imx458-0-0-4-40", "libsns_imx458.so", "stSnsImx458Obj"},
    {SONY_IMX458_MIPI_2M_90FPS_10BIT,                   "imx458-0-0-2-90", "libsns_imx458.so", "stSnsImx458Obj"},
    {SONY_IMX458_MIPI_1M_129FPS_10BIT,                  "imx458-0-0-1-129","libsns_imx458.so", "stSnsImx458Obj"},
    {SMART_SC4210_MIPI_3M_30FPS_12BIT,                  "sc4210-0-0-3-30", "libsns_sc4210.so", "stSnsSc4210Obj"},
    {SMART_SC4210_MIPI_3M_30FPS_10BIT_WDR2TO1,          "sc4210-0-1-3-30", "libsns_sc4210.so", "stSnsSc4210Obj"},
    {PANASONIC_MN34220_LVDS_2M_30FPS_12BIT,             "mn34220-0-0-2-30", "libsns_mn34220.so", "stSnsMn34220Obj"},
    {OMNIVISION_OS04B10_MIPI_4M_30FPS_10BIT,            "os04b10-0-0-4-30", "libsns_os04b10.so", "stSnsOs04b10_2lObj"},
    {OMNIVISION_OS05A_MIPI_4M_30FPS_12BIT,              "os05a-0-0-4-30", "libsns_os05a.so", "stSnsOs05aObj"},
    {OMNIVISION_OS05A_MIPI_4M_30FPS_10BIT_WDR2TO1,      "os05a-0-1-4-30", "libsns_os05a.so", "stSnsOs05aObj"},
    {OMNIVISION_OS05A_2L_MIPI_1M_30FPS_10BIT,           "os05a-2-0-1-30", "libsns_os05a_2l.so", "stSnsOs05a_2lObj"},
    {OMNIVISION_OS05A_2L_MIPI_4M_30FPS_12BIT,           "os05a-2-0-4-30", "libsns_os05a_2l.so", "stSnsOs05a_2lObj"},
    {OMNIVISION_OS05A_2L_MIPI_4M_30FPS_10BIT_WDR2TO1,   "os05a-2-1-4-30", "libsns_os05a_2l.so", "stSnsOs05a_2lObj"},
    {OMNIVISION_OS08A10_MIPI_8M_30FPS_10BIT,            "os08a10-0-0-8-30", "libsns_os08a10.so", "stSnsOS08A10Obj"},
    {GALAXYCORE_GC2053_MIPI_2M_30FPS_10BIT,             "gc2053-0-0-2-30", "libsns_gc2053.so", "stSnsGc2053Obj"},
    {OMNIVISION_OV12870_MIPI_1M_240FPS_10BIT,           "ov12870-0-0-1-240", "libsns_ov12870.so", "stSnsOv12870Obj"},
    {OMNIVISION_OV12870_MIPI_2M_120FPS_10BIT,           "ov12870-0-0-2-120", "libsns_ov12870.so", "stSnsOv12870Obj"},
    {OMNIVISION_OV12870_MIPI_8M_30FPS_10BIT,            "ov12870-0-0-8-30", "libsns_ov12870.so", "stSnsOv12870Obj"},
    {OMNIVISION_OV12870_MIPI_12M_30FPS_10BIT,           "ov12870-0-0-12-30", "libsns_ov12870.so", "stSnsOv12870Obj"},
    {OMNIVISION_OV9284_MIPI_1M_60FPS_10BIT,             "ov9284-0-0-1-60", "libsns_ov9284.so", "stSnsOv9284Obj"},
    {SONY_IMX415_MIPI_8M_30FPS_12BIT,                   "imx415-0-0-8-30", "libsns_imx415.so", "stSnsImx415Obj"},
    {SONY_IMX415_MIPI_8M_20FPS_12BIT,                   "imx415-0-0-8-20", "libsns_imx415.so", "stSnsImx415Obj"},
    {SONY_IMX415_MIPI_2M_60FPS_12BIT,                   "imx415-0-0-2-60", "libsns_imx415.so", "stSnsImx415Obj"},
    {SONY_IMX334_MIPI_8M_30FPS_12BIT,                   "imx334-0-0-8-30", "libsns_imx334.so", "stSnsImx334Obj"},
    {OMNIVISION_OV2775_MIPI_2M_30FPS_12BIT,             "ov2775-0-0-2-30", "libsns_ov2775.so", "stSnsOV2775Obj"},
    {OMNIVISION_OV2775_MIPI_2M_30FPS_12BIT_WDR2TO1,     "ov2775-0-1-2-30", "libsns_ov2775.so", "stSnsOV2775Obj"},
    {OMNIVISION_OV2775_MIPI_2M_30FPS_12BIT_WDR2TO1_HLCG,"ov2775-0-2-2-30", "libsns_ov2775.so", "stSnsOV2775Obj"},
    {OMNIVISION_OV2775_MIPI_2M_30FPS_12BIT_BUILTIN,     "ov2775-0-3-2-30", "libsns_ov2775.so", "stSnsOV2775Obj"},
    {OMNIVISION_OV2775_2L_MIPI_2M_30FPS_12BIT,          "ov2775-2-0-2-30", "libsns_ov2775.so", "stSnsOV2775Obj"},
    {OMNIVISION_OV2775_2L_MIPI_2M_30FPS_12BIT_WDR2TO1,  "ov2775-2-1-2-30", "libsns_ov2775.so", "stSnsOV2775Obj"},
    {PRIMESENSOR_PS5260_2L_MIPI_2M_30FPS_12BIT,         "ps5260-2-0-2-30", "libsns_ps5260_2l.so", "g_stSnsPs5260_2l_Obj"},
    {PRIMESENSOR_PS5260_2L_MIPI_2M_30FPS_12BIT_BUILTIN, "ps5260-2-3-2-30", "libsns_ps5260_2l.so", "g_stSnsPs5260_2l_Obj"},
    {MIPI_YUV_2M_60FPS_8BIT,                      "yuv422-0-0-2-60", NULL,                NULL},
    {MIPI_YUV_8M_30FPS_8BIT,                      "yuv422-0-0-8-30", NULL,                NULL},
    {MIPI_YUVPKG_2M_60FPS_8BIT,                   "pkg422-0-0-2-60", NULL,                NULL},
    {BT1120_YUV_2M_60FPS_8BIT,                    "bt1120-0-0-2-60", NULL,                NULL},
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
static int snscnt = 0;

#include <signal.h>
void SAMPLE_VENC_HandleSig2(HI_S32 signo)
{
    signal(SIGINT, SIG_IGN);
    signal(SIGTERM, SIG_IGN);

    if (SIGINT == signo || SIGTERM == signo)
    {
        mppex_hook_destroy();
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
  char loadstr[256];
  if(strstr(cfg->snsname, "bt1120"))
    sprintf(loadstr, "%s/ko/load3559v200 -i -yuv0 1", path);
  else if(strstr(cfg->snsname, "imx334")) // 24Mclk && i2c;
    sprintf(loadstr, "%s/ko/load3559v200 -i -sensor0 %s", path, "imx458");
  else
    sprintf(loadstr, "%s/ko/load3559v200 -i -sensor0 %s", path, cfg->snsname);

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
    SENSOR1_TYPE = BT656_YUV_0M_60FPS_8BIT;
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
    stViConfig.astViInfo[i].stChnInfo.enPixFormat    = PIXEL_FORMAT_YVU_SEMIPLANAR_420;
  }

  mppex_hook_vi(&stViConfig);

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

int gsf_mpp_venc_start(gsf_mpp_venc_t *venc)
{
  HI_S32 s32Ret;
  VENC_GOP_ATTR_S stGopAttr;
  
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

  mppex_hook_venc(venc);
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
  printf("The scene sample is end.");
  return s32ret;
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
  printf("VencChn:%d, id:%d, ret:%d\n", VencChn, id, ret);
  return ret;
}

int gsf_mpp_isp_ctl(int ViPipe, int id, void *args)
{
  int ret = -1;
  switch(id)
  {
    case GSF_MPP_ISP_CTL_IR:
      ret = ((int)args)?ISP_IrSwitchToIr(ViPipe):ISP_IrSwitchToNormal(ViPipe);
      break;
    default:
      break;
  }
  printf("ViPipe:%d, id:%d, ret:%d\n", ViPipe, id, ret);
  return ret;
}

int gsf_mpp_audio_start(gsf_mpp_aenc_t *aenc)
{
  static int init = 0;
  
  if(!init)
  {
    init = 1;
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


int gsf_mpp_cfg_vdec(char *path, gsf_mpp_cfg_t *cfg)
{
    char loadstr[64];
    sprintf(loadstr, "%s/ko/load3516dv300 -i", path);
    printf("%s => loadstr: %s\n", __func__, loadstr);
    system(loadstr);
    
    signal(SIGINT, SAMPLE_VDEC_HandleSig);
    signal(SIGTERM, SAMPLE_VDEC_HandleSig);
    
    
    HI_S32 i, s32Ret = HI_SUCCESS;
    HI_U32 u32VdecChnNum = 4;

    /************************************************
    step1:  init SYS, init common VB(for VPSS and VO)
    *************************************************/
    SIZE_S stDispSize;
    PIC_SIZE_E enDispPicSize = PIC_1080P;
    s32Ret =  SAMPLE_COMM_SYS_GetPicSize(enDispPicSize, &stDispSize);
    if(s32Ret != HI_SUCCESS)
    {
        SAMPLE_PRT("sys get pic size fail for %#x!\n", s32Ret);
        goto END1;
    }

	  stDispSize.u32Height = (stDispSize.u32Height == 1080)?1088:stDispSize.u32Height;

    VB_CONFIG_S stVbConfig;
    memset(&stVbConfig, 0, sizeof(VB_CONFIG_S));
    stVbConfig.u32MaxPoolCnt             = 128;
    stVbConfig.astCommPool[0].u32BlkCnt  = 10*u32VdecChnNum;
    stVbConfig.astCommPool[0].u64BlkSize = COMMON_GetPicBufferSize(stDispSize.u32Width, stDispSize.u32Height,
                                                PIXEL_FORMAT_YVU_SEMIPLANAR_420, DATA_BITWIDTH_8, COMPRESS_MODE_SEG, 0);
    s32Ret = SAMPLE_COMM_SYS_Init(&stVbConfig);
    if(s32Ret != HI_SUCCESS)
    {
        SAMPLE_PRT("init sys fail for %#x!\n", s32Ret);
        goto END1;
    }

    /************************************************
    step2:  init module VB or user VB(for VDEC)
    *************************************************/
    SAMPLE_VDEC_ATTR astSampleVdec[VDEC_MAX_CHN_NUM];
    for(i=0; i<u32VdecChnNum; i++)
    {
        astSampleVdec[i].enType                           = PT_H264;
        astSampleVdec[i].u32Width                         = 1920;
        astSampleVdec[i].u32Height                        = 1080;
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
  
    for(i = 0; i < VO_MAX_DEV_NUM; i++)
    {
      pthread_mutex_init(&vo_mng[i].lock, NULL);
      //pthread_mutex_destroy(&mutex);
    }
    return 0;
  
END2:
    SAMPLE_COMM_VDEC_ExitVBPool();
END1:
    SAMPLE_COMM_SYS_Exit();
  return s32Ret;
}





//启动视频输出设备;
int gsf_mpp_vo_start(int vodev, VO_INTF_TYPE_E type, VO_INTF_SYNC_E sync, int wbc)
{
    HI_S32 i, s32Ret = HI_SUCCESS;
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
    stVoConfig.enPixFormat           = PIXEL_FORMAT_YVU_SEMIPLANAR_420;
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



//创建图像层显示通道;
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
    SAMPLE_COMM_VPSS_UnBind_VO(vdev->layer[volayer].chs[i].src_grp, vdev->layer[volayer].chs[i].src_chn, volayer, i);
    if(vdev->layer[volayer].chs[i].src_type >= VO_SRC_VDVP)
    {
      SAMPLE_COMM_VDEC_UnBind_VPSS(i, i);
      HI_BOOL chen[VPSS_MAX_PHY_CHN_NUM] = {HI_TRUE,HI_TRUE};
      SAMPLE_COMM_VPSS_Stop(i, chen);
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
  
 
  if(0)
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
int gsf_mpp_vo_vsend(int volayer, int ch, char *data, gsf_mpp_frm_attr_t *attr)
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
    vdev->layer[volayer].chs[ch].src_grp = ch;
    vdev->layer[volayer].chs[ch].src_chn = VPSS_CHN0;
    // VOLAYER_HD0: [0 - VPSS_MAX_GRP_NUM/2]
    // VOLAYER_PIP: [VPSS_MAX_GRP_NUM/2 - VPSS_MAX_GRP_NUM];

    // unbind vo && vpss && vdec;
    SAMPLE_COMM_VPSS_UnBind_VO(ch, VPSS_CHN0, volayer, ch);
    SAMPLE_COMM_VDEC_UnBind_VPSS(ch, ch);
    
    // stop  vpss;
    #if 0
    s32Ret = HI_MPI_VPSS_StopGrp(ch);
    s32Ret = HI_MPI_VPSS_DisableChn(ch, VPSS_CHN0);
    s32Ret = HI_MPI_VPSS_DestroyGrp(ch);
    #else
    HI_BOOL chen[VPSS_MAX_PHY_CHN_NUM] = {HI_TRUE,HI_TRUE};
    s32Ret = SAMPLE_COMM_VPSS_Stop(ch, chen);
    #endif
    // stop  vdec;
    HI_MPI_VDEC_StopRecvStream(ch);
    HI_MPI_VDEC_DestroyChn(ch);
    
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
      stGrpAttr.u32MaxW = ALIGN_UP(attr->width,  16);
      stGrpAttr.u32MaxH = ALIGN_UP(attr->height, 16);
      stGrpAttr.stFrameRate.s32SrcFrameRate = -1;
      stGrpAttr.stFrameRate.s32DstFrameRate = -1;
      stGrpAttr.enDynamicRange = DYNAMIC_RANGE_SDR8;
      stGrpAttr.enPixelFormat  = PIXEL_FORMAT_YVU_SEMIPLANAR_420;
      stGrpAttr.bNrEn   = HI_FALSE;
      s32Ret = HI_MPI_VPSS_CreateGrp(ch, &stGrpAttr);

      
      /*** enable vpss chn, with frame ***/
      stChnAttr.u32Width                    = ALIGN_UP(attr->width,  16);
      stChnAttr.u32Height                   = ALIGN_UP(attr->height, 16);
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
      
      s32Ret = HI_MPI_VPSS_SetChnAttr(ch, VPSS_CHN0, &stChnAttr);
      s32Ret = HI_MPI_VPSS_EnableChn(ch, VPSS_CHN0);
      s32Ret = HI_MPI_VPSS_StartGrp(ch);
    }
    else
    {
      VPSS_GRP_ATTR_S stGrpAttr = {0};
      VPSS_CHN_ATTR_S stChnAttr[VPSS_MAX_PHY_CHN_NUM] = {0};
      
      /*** create vpss group ***/
      stGrpAttr.u32MaxW = ALIGN_UP(attr->width,  16);
      stGrpAttr.u32MaxH = ALIGN_UP(attr->height, 16);
      stGrpAttr.stFrameRate.s32SrcFrameRate = -1;
      stGrpAttr.stFrameRate.s32DstFrameRate = -1;
      stGrpAttr.enDynamicRange = DYNAMIC_RANGE_SDR8;
      stGrpAttr.enPixelFormat  = PIXEL_FORMAT_YVU_SEMIPLANAR_420;
      stGrpAttr.bNrEn   = HI_FALSE;
        
      /*** enable vpss chn, with frame ***/
      int i;
      for(i = 0; i < 2; i++)
      {
        stChnAttr[i].u32Width                    = ALIGN_UP((i == 0)?attr->width:640,  16);
        stChnAttr[i].u32Height                   = ALIGN_UP((i == 0)?attr->height:480, 16);
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
      s32Ret = SAMPLE_COMM_VPSS_Start(ch, chen, &stGrpAttr, stChnAttr);
    }
    // bind  vdec && vpss && vo;
    s32Ret = SAMPLE_COMM_VDEC_Bind_VPSS(ch, ch);
    s32Ret = SAMPLE_COMM_VPSS_Bind_VO(ch, VPSS_CHN0, volayer, ch);
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


int gsf_mpp_ao_asend(int aodev, int ch, char *data, gsf_mpp_frm_attr_t *attr)
{
  
  //audio dec bind vo;
  
  return 0;
}

#include "audio_aac_adp.h"
//from sample_audio.c;
extern HI_BOOL gs_bAioReSample;
extern AUDIO_SAMPLE_RATE_E enInSampleRate;

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
  
  stAioAttr.enSamplerate   = AUDIO_SAMPLE_RATE_48000;
  stAioAttr.enBitwidth     = AUDIO_BIT_WIDTH_16;
  stAioAttr.enWorkmode     = AIO_MODE_I2S_MASTER;
  stAioAttr.enSoundmode    = AUDIO_SOUND_MODE_STEREO;
  stAioAttr.u32EXFlag      = 0;
  stAioAttr.u32FrmNum      = 30;
  stAioAttr.u32PtNumPerFrm = AACLC_SAMPLES_PER_FRAME;
  stAioAttr.u32ChnCnt      = 2;
  stAioAttr.u32ClkSel      = 0;
  stAioAttr.enI2sType      = (AoDev == SAMPLE_AUDIO_INNER_AO_DEV)?AIO_I2STYPE_INNERCODEC:AIO_I2STYPE_INNERHDMI;

  s32AoChnCnt = stAioAttr.u32ChnCnt;
  s32Ret = SAMPLE_COMM_AUDIO_StartAo(AoDev, s32AoChnCnt, &stAioAttr, enInSampleRate, gs_bAioReSample);
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
  s32Ret = SAMPLE_COMM_AUDIO_StopAo(AoDev, s32AoChnCnt, gs_bAioReSample);
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
  SAMPLE_COMM_VPSS_UnBind_VO(vdev->layer[volayer].chs[ch].src_grp, vdev->layer[volayer].chs[ch].src_chn, volayer, ch);
  if(vdev->layer[volayer].chs[ch].src_type >= VO_SRC_VDVP)
  {
    // stop vpss;
    SAMPLE_COMM_VDEC_UnBind_VPSS(ch, ch);
    HI_BOOL chen[VPSS_MAX_PHY_CHN_NUM] = {HI_TRUE,HI_TRUE};
    SAMPLE_COMM_VPSS_Stop(ch, chen);
    //stop vdec;
    HI_MPI_VDEC_StopRecvStream(ch);
    HI_MPI_VDEC_DestroyChn(ch);
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
#if 1
    var.transp= s_a32;
    var.red   = s_r32;
    var.green = s_g32;
    var.blue  = s_b32;
    var.bits_per_pixel = 32;
#else
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

