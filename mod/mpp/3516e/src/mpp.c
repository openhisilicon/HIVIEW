#include <stdio.h>
#include <dlfcn.h>
#include <string.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>

#include "hi_type.h"
#include "hi_scene.h"
#include "hi_scene_loadparam.h"
#include "hi_scenecomm_log.h"

#include "sample_comm.h"
#include "mpp.h"


////// from sample_venc.c;
#define VB_MAX_NUM 10
typedef struct hiSAMPLE_VPSS_ATTR_S
{
    SIZE_S            stMaxSize;
    DYNAMIC_RANGE_E   enDynamicRange;
    PIXEL_FORMAT_E    enPixelFormat;
    COMPRESS_MODE_E   enCompressMode[VPSS_MAX_PHY_CHN_NUM];
    SIZE_S            stOutPutSize[VPSS_MAX_PHY_CHN_NUM];
    FRAME_RATE_CTRL_S stFrameRate[VPSS_MAX_PHY_CHN_NUM];
    HI_BOOL           bMirror[VPSS_MAX_PHY_CHN_NUM];
    HI_BOOL           bFlip[VPSS_MAX_PHY_CHN_NUM];
    HI_BOOL           bChnEnable[VPSS_MAX_PHY_CHN_NUM];

    SAMPLE_SNS_TYPE_E enSnsType;
    HI_U32            BigStreamId;
    HI_U32            SmallStreamId;
    VI_VPSS_MODE_E    ViVpssMode;
    HI_BOOL           bWrapEn;
    HI_U32            WrapBufLine;
} SAMPLE_VPSS_CHN_ATTR_S;

typedef struct hiSAMPLE_VB_ATTR_S
{
    HI_U32            validNum;
    HI_U64            blkSize[VB_MAX_NUM];
    HI_U32            blkCnt[VB_MAX_NUM];
    HI_U32            supplementConfig;
} SAMPLE_VB_ATTR_S;

extern void SAMPLE_VENC_HandleSig(HI_S32 signo);
extern HI_S32 SAMPLE_VENC_SYS_Init(SAMPLE_VB_ATTR_S *pCommVbAttr);
extern HI_S32 SAMPLE_VENC_VI_Init( SAMPLE_VI_CONFIG_S *pstViConfig, VI_VPSS_MODE_E ViVpssMode);
extern HI_S32 SAMPLE_VENC_CheckSensor(SAMPLE_SNS_TYPE_E   enSnsType,SIZE_S  stSize);
extern HI_S32 SAMPLE_VENC_ModifyResolution(SAMPLE_SNS_TYPE_E   enSnsType,PIC_SIZE_E *penSize,SIZE_S *pstSize);
extern HI_S32 SAMPLE_VENC_VPSS_Init(VPSS_GRP VpssGrp, SAMPLE_VPSS_CHN_ATTR_S *pstParam);
extern HI_VOID SAMPLE_VENC_GetDefaultVpssAttr(SAMPLE_SNS_TYPE_E enSnsType, HI_BOOL *pChanEnable, SIZE_S stEncSize[], SAMPLE_VPSS_CHN_ATTR_S *pVpssAttr);
extern HI_VOID SAMPLE_VENC_GetCommVbAttr(const SAMPLE_SNS_TYPE_E enSnsType, const SAMPLE_VPSS_CHN_ATTR_S *pstParam, HI_BOOL bSupportDcf, SAMPLE_VB_ATTR_S * pstcommVbAttr);

////// end.



typedef struct {
  int   type;
  char* name;
  char* lib;
  char* obj;
}SAMPLE_MPP_SENSOR_T;

int SENSOR_TYPE  = 0;
int SENSOR0_TYPE = 0;
int SENSOR1_TYPE = 0;
SAMPLE_SNS_TYPE_E g_enSnsType[MAX_SENSOR_NUM];
ISP_SNS_OBJ_S* g_pstSnsObj[MAX_SENSOR_NUM];

// name: [cfg->snsname - cfg->lane - cfg->wdr - cfg->res - cfg->fps]
static SAMPLE_MPP_SENSOR_T libsns[SAMPLE_SNS_TYPE_BUTT] = {
    {SMART_SC4236_MIPI_3M_30FPS_10BIT,              "sc4236-0-0-3-30", "libsns_sc4236.so", "stSnsSc4236Obj"},
    {SMART_SC4236_MIPI_3M_20FPS_10BIT,              "sc4236-0-0-3-20", "libsns_sc4236.so", "stSnsSc4236Obj"}, 
    {SMART_SC2231_MIPI_2M_30FPS_10BIT,              "sc2231-0-0-2-30", "libsns_sc2231.so", "stSnsSc2231Obj"}, 
    {SMART_SC2335_MIPI_2M_30FPS_10BIT,              "sc2335-0-0-2-30", "libsns_sc2335.so", "stSnsSc2335Obj"},
    {SMART_SC3235_MIPI_3M_30FPS_10BIT,              "sc3235-0-0-3-30", "libsns_sc3235.so", "stSnsSc3235Obj"}, 
    {SMART_SC2235_DC_2M_30FPS_10BIT,                "sc2235-0-0-2-30", "libsns_sc2235.so", "stSnsSc2235Obj"}, 
    {GALAXYCORE_GC2053_MIPI_2M_30FPS_10BIT,         "gc2053-0-0-2-30", "libsns_gc2053.so", "stSnsGc2053Obj"}, 
    {GALAXYCORE_GC2053_MIPI_2M_30FPS_10BIT_FORCAR,  "gc2053-1-0-2-30", "libsns_gc2053_forcar.so", "stSnsGc2053ForCarObj"}, 
    {SONY_IMX327_MIPI_2M_30FPS_12BIT,               "imx327-0-0-2-30", "libsns_imx307.so", "stSnsImx327Obj"}, 
    {SONY_IMX327_MIPI_2M_30FPS_12BIT_WDR2TO1,       "imx327-0-1-2-30", "libsns_imx307.so", "stSnsImx327Obj"}, 
    {SONY_IMX327_2L_MIPI_2M_30FPS_12BIT,            "imx327-2-0-2-30", "libsns_imx307_2l.so", "stSnsImx327_2l_Obj"}, 
    {SONY_IMX327_2L_MIPI_2M_30FPS_12BIT_WDR2TO1,    "imx327-2-1-2-30",  "libsns_imx307_2l.so", "stSnsImx327_2l_Obj"}, 
    {SONY_IMX307_MIPI_2M_30FPS_12BIT,               "imx307-0-0-2-30", "libsns_imx307.so", "stSnsImx307Obj"}, 
    {SONY_IMX307_MIPI_2M_30FPS_12BIT_WDR2TO1,       "imx307-0-1-2-30",  "libsns_imx307.so", "stSnsImx307Obj"}, 
    {SONY_IMX307_2L_MIPI_2M_30FPS_12BIT,            "imx307-2-0-2-30",  "libsns_imx307_2l.so", "stSnsImx307_2l_Obj"}, 
    {SONY_IMX307_2L_MIPI_2M_30FPS_12BIT_WDR2TO1,    "imx307-2-1-2-30",  "libsns_imx307_2l.so", "stSnsImx307_2l_Obj"}, 
    {SONY_IMX335_MIPI_5M_30FPS_12BIT,               "imx335-0-0-5-30",  "libsns_imx335.so", "stSnsImx335Obj"}, 
    {SONY_IMX335_MIPI_5M_30FPS_10BIT_WDR2TO1,       "imx335-0-1-5-30",   "libsns_imx335.so", "stSnsImx335Obj"}, 
    {SONY_IMX335_MIPI_4M_30FPS_12BIT,               "imx335-0-0-4-30",   "libsns_imx335.so", "stSnsImx335Obj"}, 
    {SONY_IMX335_MIPI_4M_30FPS_10BIT_WDR2TO1,       "imx335-0-1-4-30",   "libsns_imx335.so", "stSnsImx335Obj"}, 
    {OMNIVISION_OS05A_MIPI_5M_30FPS_12BIT,          "os05a-0-0-5-30",   "libsns_os05a.so", "stSnsOs05aObj"}, 
    {OMNIVISION_OS05A_MIPI_4M_30FPS_12BIT,          "os05a-0-0-4-30",   "libsns_os05a.so", "stSnsOs05aObj"}, 
    {OMNIVISION_OS05A_MIPI_4M_30FPS_10BIT_WDR2TO1,  "os05a-0-1-4-30",   "libsns_os05a.so", "stSnsOs05aObj"}, 
    
    
  };

static SAMPLE_MPP_SENSOR_T* SAMPLE_MPP_SERSOR_GET(char* name)
{
  int i = 0;
  for(i = 0; i < SAMPLE_SNS_TYPE_BUTT; i++)
  {
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




int gsf_mpp_cfg(char *path, gsf_mpp_cfg_t *cfg)
{
  
  signal(SIGINT, SAMPLE_VENC_HandleSig);
  signal(SIGTERM, SAMPLE_VENC_HandleSig);
  
  char snsstr[64];
  sprintf(snsstr, "%s-%d-%d-%d-%d"
          , cfg->snsname, cfg->lane, cfg->wdr, cfg->res, cfg->fps);

  SAMPLE_MPP_SENSOR_T* sns = SAMPLE_MPP_SERSOR_GET(snsstr);
  if(!sns)
  {
    printf("%s => snsstr:%s, unknow.\n", __func__, snsstr);
    return -1;
  } 
  
  char loadstr[128];
  if(strstr(cfg->snsname, "sc2335"))
    sprintf(loadstr, "%s/ko/load3516ev200 -i -sensor0 %s", path, "sc2231");
  else
    sprintf(loadstr, "%s/ko/load3516ev200 -i -sensor0 %s", path, cfg->snsname);
  
  printf("%s => loadstr: %s\n", __func__, loadstr);
  system(loadstr);
  
  
  SENSOR_TYPE = SENSOR0_TYPE = SENSOR1_TYPE = sns->type;
  printf("%s => SENSOR0_TYPE: %d\n", __func__, SENSOR0_TYPE);
  
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
    
    g_pstSnsObj[0] = dlsym(dl, sns->obj);
    //g_pstSnsObj[1] = g_pstSnsObj[0];
    
    if(NULL != dlerror())
    {
        printf("err dlsym %s\n", sns->obj);
        goto __err;
    }
  }
  printf("%s => snsstr:%s, sensor_type:%d, load:%s\n"
        , __func__, snsstr, SENSOR_TYPE, sns->lib?:"");
  
  return 0;
__err:
	if(dl)
	{
    dlclose(dl);
  }
  dlerror();
  return -1;
}





int gsf_mpp_vi_start(gsf_mpp_vi_t *vi)
{
  HI_S32 i, s32Ret;
  VI_DEV  ViDev = 0;
  VI_PIPE ViPipe = 0;
  VI_CHN  ViChn = 0;
  SAMPLE_VI_CONFIG_S stViConfig;
  memset(&stViConfig, 0, sizeof(stViConfig));
  SAMPLE_COMM_VI_GetSensorInfo(&stViConfig);
  
  SIZE_S stSize[VPSS_MAX_PHY_CHN_NUM];

  for(i = 0; i < VPSS_MAX_PHY_CHN_NUM; i++)
  {
    if(!vi->vpss_en[i])
      continue;

    s32Ret = SAMPLE_COMM_SYS_GetPicSize(vi->vpss_sz[i], &stSize[i]);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("SAMPLE_COMM_SYS_GetPicSize failed!\n");
        return s32Ret;
    }
  }
  
  s32Ret = SAMPLE_VENC_CheckSensor(stViConfig.astViInfo[0].stSnsInfo.enSnsType, stSize[0]);
  if(s32Ret != HI_SUCCESS)
  {
      s32Ret = SAMPLE_VENC_ModifyResolution(stViConfig.astViInfo[0].stSnsInfo.enSnsType,&vi->vpss_sz[0],&stSize[0]);
      if(s32Ret != HI_SUCCESS)
      {
          return HI_FAILURE;
      }
  }
  
  SAMPLE_VPSS_CHN_ATTR_S stParam;
  SAMPLE_VENC_GetDefaultVpssAttr(stViConfig.astViInfo[0].stSnsInfo.enSnsType, vi->vpss_en, stSize, &stParam);

  SAMPLE_VB_ATTR_S commVbAttr;
  memset(&commVbAttr, 0, sizeof(commVbAttr));
  commVbAttr.supplementConfig = HI_FALSE;
  SAMPLE_VENC_GetCommVbAttr(stViConfig.astViInfo[0].stSnsInfo.enSnsType, &stParam, HI_FALSE, &commVbAttr);

  s32Ret = SAMPLE_VENC_SYS_Init(&commVbAttr);
  if(s32Ret != HI_SUCCESS)
  {
      SAMPLE_PRT("Init SYS err for %#x!\n", s32Ret);
      return s32Ret;
  }
  
  stViConfig.s32WorkingViNum       = 1;
  stViConfig.astViInfo[0].stDevInfo.ViDev     = ViDev;
  stViConfig.astViInfo[0].stPipeInfo.aPipe[0] = ViPipe;
  stViConfig.astViInfo[0].stChnInfo.ViChn     = ViChn;
  stViConfig.astViInfo[0].stChnInfo.enDynamicRange = DYNAMIC_RANGE_SDR8;
  stViConfig.astViInfo[0].stChnInfo.enPixFormat    = PIXEL_FORMAT_YVU_SEMIPLANAR_420;

  s32Ret = SAMPLE_VENC_VI_Init(&stViConfig, stParam.ViVpssMode);
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


int gsf_mpp_vpss_start(gsf_mpp_vpss_t *vpss)
{
  HI_S32 i, s32Ret;
  SIZE_S stSize[VPSS_MAX_PHY_CHN_NUM];

  SAMPLE_VI_CONFIG_S stViConfig;
  memset(&stViConfig, 0, sizeof(stViConfig));
  SAMPLE_COMM_VI_GetSensorInfo(&stViConfig);

  for(i = 0; i < VPSS_MAX_PHY_CHN_NUM; i++)
  {
      if(!vpss->enable[i])
        continue;
      
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

  SAMPLE_VPSS_CHN_ATTR_S stParam;
  SAMPLE_VENC_GetDefaultVpssAttr(stViConfig.astViInfo[0].stSnsInfo.enSnsType, vpss->enable, stSize, &stParam);
  
  s32Ret = SAMPLE_VENC_VPSS_Init(vpss->VpssGrp, &stParam);
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

  s32Ret = SAMPLE_COMM_VPSS_Bind_VENC(venc->VpssGrp, venc->VpssChn,venc->VencChn);
  if (HI_SUCCESS != s32Ret)
  {
      SAMPLE_PRT("Venc Get GopAttr failed for %#x!\n", s32Ret);
      goto EXIT_VENC_STOP;
  }

  return s32Ret;

EXIT_VENC_UnBind:
    SAMPLE_COMM_VPSS_UnBind_VENC(venc->VpssGrp,venc->VpssChn,venc->VencChn);
EXIT_VENC_STOP:
    SAMPLE_COMM_VENC_Stop(venc->VencChn);
EXIT_VI_VPSS_UNBIND:
  return s32Ret;
}


int gsf_mpp_venc_stop(gsf_mpp_venc_t *venc)
{
  HI_S32 s32Ret;
  
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
    default:
      break;
  }
  printf("VencChn:%d, id:%d, ret:%d\n", VencChn, id, ret);
  return ret;
}

int gsf_mpp_isp_ctl(int ch, int id, void *args)
{
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