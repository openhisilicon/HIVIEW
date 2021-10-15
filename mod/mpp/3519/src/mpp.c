#include <stdio.h>
#include <dlfcn.h>
#include <string.h>
#include <signal.h>

#include "sample_comm.h"
#include "mpp.h"

extern void SAMPLE_VENC_HandleSig(HI_S32 signo);


typedef struct {
  int   type;
  char* name;
  char* lib;
  char* obj;
}SAMPLE_MPP_SENSOR_T;

int SENSOR_TYPE;
ISP_SNS_OBJ_S* g_pstSnsObj[2];

static SAMPLE_MPP_SENSOR_T libsns[64] = {
    {APTINA_AR0130_DC_720P_30FPS,            "ar0130-0-0-1-30",       NULL,                       ""},
    {APTINA_9M034_DC_720P_30FPS,             "9m034-0-0-1-30",        NULL,                       ""},
    {SONY_IMX122_DC_1080P_30FPS,             "imx122-0-0-2-30",       NULL,                       ""},
    {APTINA_MT9P006_DC_1080P_30FPS,          "mt9p006-0-0-2-30",      NULL,                       ""},
    {SAMPLE_VI_MODE_1_D1,                    "bt656-0-0-0-30",        NULL,                       ""},
    {SAMPLE_VI_MODE_BT1120_1080I,            "bt1120i-0-0-2-30",      NULL,                       ""},
    {SAMPLE_VI_MODE_BT1120_720P,             "bt1120-0-0-1-30",       NULL,                       ""},
    {SAMPLE_VI_MODE_BT1120_1080P,            "bt1120-0-0-2-30",       NULL,                       ""},
    {PANASONIC_MN34220_SUBLVDS_1080P_30FPS,  "mn34220-1-0-2-30",      NULL,                       "stSnsMn34220Obj"},
    {PANASONIC_MN34220_SUBLVDS_720P_120FPS,  "mn34220-1-0-1-120",     NULL,                       "stSnsMn34220Obj"},
    {SONY_IMX178_LVDS_1080P_30FPS,           "imx178-0-0-2-30",       NULL,                       ""},
    {SONY_IMX290_MIPI_1080P_30FPS,           "imx290-0-0-2-30",       "libsns_imx290.so",         "stSnsImx290Obj"},
    {PANASONIC_MN34220_MIPI_1080P_30FPS,     "mn34220-0-0-2-30",      NULL,                       "stSnsMn34220Obj"},
    {PANASONIC_MN34220_MIPI_720P_120FPS,     "mn34220-0-0-1-120",     NULL,                       "stSnsMn34220Obj"},
    {SONY_IMX178_LVDS_5M_30FPS,              "imx178-0-0-5-30",       NULL,                       ""},
    {SONY_IMX226_LVDS_4K_30FPS,              "imx226-0-0-8-30",       "libsns_imx226.so",         "stSnsImx226Obj"},
    {SONY_IMX226_LVDS_9M_30FPS,              "imx226-0-0-9-30",       "libsns_imx226.so",         "stSnsImx226Obj"},
    {SONY_IMX226_LVDS_12M_30FPS,             "imx226-0-0-12-30",      "libsns_imx226.so",         "stSnsImx226Obj"},
    {SONY_IMX226_LVDS_4K_60FPS,              "imx226-0-0-8-60",       "libsns_imx226.so",         "stSnsImx226Obj"},
    {SONY_IMX117_LVDS_4K_30FPS,              "imx117-0-0-8-30",       NULL,                       "stSnsImx117Obj"},
    {SONY_IMX117_LVDS_12M_28FPS,             "imx117-0-0-12-28",      NULL,                       "stSnsImx117Obj"},
    {SONY_IMX117_LVDS_1080P_120FPS,          "imx117-0-0-2-120",      NULL,                       "stSnsImx117Obj"},
    {SONY_IMX117_LVDS_720P_240FPS,           "imx117-0-0-1-240",      NULL,                       "stSnsImx117Obj"},
    {SONY_IMX377_MIPI_8M_30FPS_10BIT,        "imx337-0-0-8-30-10",    NULL,                       "stSnsImx377Obj"},
    {SONY_IMX377_MIPI_8M_60FPS_10BIT,        "imx337-0-0-8-60-10",    NULL,                       "stSnsImx377Obj"},
    {SONY_IMX377_MIPI_1080P_120FPS_12BIT,    "imx337-0-0-2-120-12",   NULL,                       "stSnsImx377Obj"},
    {SONY_IMX377_MIPI_720P_240FPS_10BIT,     "imx337-0-0-1-240-10",   NULL,                       "stSnsImx377Obj"},
    {SONY_IMX377_MIPI_12M_30FPS_12BIT,       "imx337-0-0-12-30-12",   NULL,                       "stSnsImx377Obj"},
    {SONY_IMX377_MIPI_8M_30FPS_12BIT,        "imx337-0-0-8-30-12",    NULL,                       "stSnsImx377Obj"},
    {SONY_IMX274_LVDS_4K_30FPS,              "imx274-0-0-8-30",       "libsns_imx274.so",         "stSnsImx274Obj"},
    {PANASONIC_MN34120_LVDS_16M_16P25FPS,    "mn34120-0-0-16-25",     NULL,                       "stSnsMn34120Obj"},
    {PANASONIC_MN34120_LVDS_4K_30FPS,        "mn34120-0-0-8-30",      NULL,                       "stSnsMn34120Obj"},
    {PANASONIC_MN34120_LVDS_1080P_60FPS,     "mn34120-0-0-2-60",      NULL,                       "stSnsMn34120Obj"},
    {APTINA_AR0230_HISPI_1080P_30FPS,        "ar0230-0-0-2-30",       NULL,                       ""},
    {APTINA_AR0330_MIPI_1080P_30FPS,         "ar0330-0-0-2-30",       NULL,                       ""},
    {APTINA_AR0330_MIPI_1536P_25FPS,         "ar0330-0-0-3-25",       NULL,                       ""},
    {APTINA_AR0330_MIPI_1296P_25FPS,         "ar0330-0-0-3-25",       NULL,                       ""},
    {OMNIVISION_OV4689_MIPI_4M_30FPS,        "ov4689-0-0-4-30",       "libsns_ov4689_slave.so",   "stSnsOv4689SlaveObj"},
    {OMNIVISION_OV4689_MIPI_1080P_30FPS,     "ov4689-0-0-2-30",       "libsns_ov4689_slave.so",   "stSnsOv4689SlaveObj"},
    {OMNIVISION_OV5658_MIPI_5M_30FPS,        "ov5658-0-0-5-30",       NULL,                       ""},
    {SONY_IMX326_MIPI_5M_30FPS,              "imx326-0-0-5-30",       "libsns_imx326.so",         "stSnsImx326Obj"},
    {OMNIVISION_OS05A_MIPI_5M_30FPS,         "os05a-0-0-5-30",        "libsns_os05a.so",          "stSnsOs05aObj"},
    {OMNIVISION_OS08A_MIPI_4K_30FPS,         "os08a-0-0-8-30",        "libsns_os08a10.so",        "stSnsOs08a10Obj"},
    {SONY_IMX334_MIPI_4K_30FPS,              "imx334-0-0-8-30",       "libsns_imx334.so",         "stSnsImx334Obj"},
  };

static SAMPLE_MPP_SENSOR_T* SAMPLE_MPP_SERSOR_GET(char* name)
{
  int i = 0;
  for(i = 0; i < sizeof(libsns)/sizeof(libsns[0]); i++)
  {
    if(strstr(libsns[i].name, name))
    {
      return &libsns[i];
    }
  }
  return NULL;
}

static void * dl = NULL;

int gsf_mpp_cfg(char *path, gsf_mpp_cfg_t *cfg)
{
  char snsstr[64];
  sprintf(snsstr, "%s-%d-%d-%d-%d"
          , cfg->snsname, cfg->lane, cfg->wdr, cfg->res, cfg->fps);

  SAMPLE_MPP_SENSOR_T* sns = SAMPLE_MPP_SERSOR_GET(snsstr);
  if(!sns)
  {
    printf("%s => snsstr:%s, unknow.\n", __func__, snsstr);
    return -1;
  } 

  char loadstr[256];
  //sprintf(loadstr, "%s/ko/load3519v101 -i -sensor0 %s -offline", path, cfg->snsname);
  // fixed vi0=>snsname, vi1=>bt1120;
  sprintf(loadstr, "%s/ko/load3519v101 -a -sensor0 %s -sensor1 bt1120 -total 1024 -osmem 512 -offline -workmode single_pipe", path, cfg->snsname);
  printf("%s => loadstr: %s\n", __func__, loadstr);
  system(loadstr);
  
  signal(SIGINT, SAMPLE_VENC_HandleSig);
  signal(SIGTERM, SAMPLE_VENC_HandleSig);
  
  
  SENSOR_TYPE = sns->type;
  
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
    PIC_SIZE_E enSize[2] = {PIC_UHD4K, PIC_HD1080};	
	
    VB_CONF_S stVbConf;
    
    SAMPLE_VI_CONFIG_S stViConfig = {0};
    SAMPLE_VI_CONFIG_S stViConfig2 = {0};
    
    HI_S32 s32Ret = HI_SUCCESS;
    HI_U32 u32BlkSize;

    /******************************************
     step  1: init sys variable 
    ******************************************/
    memset(&stVbConf,0,sizeof(VB_CONF_S));
	
    SAMPLE_COMM_VI_GetSizeBySensor(&enSize[0]);
  
    stVbConf.u32MaxPoolCnt = 128;

    /*video buffer*/ 
	  if(1)
    {
	    u32BlkSize = SAMPLE_COMM_SYS_CalcPicVbBlkSize(VIDEO_ENCODING_MODE_PAL,\
	                enSize[0], SAMPLE_PIXEL_FORMAT, SAMPLE_SYS_ALIGN_WIDTH);
	    stVbConf.astCommPool[0].u32BlkSize = u32BlkSize;
	    stVbConf.astCommPool[0].u32BlkCnt = 10*2;
	  }
	  
    if(2) 
    {
	    u32BlkSize = SAMPLE_COMM_SYS_CalcPicVbBlkSize(VIDEO_ENCODING_MODE_PAL,\
	                enSize[1], SAMPLE_PIXEL_FORMAT, SAMPLE_SYS_ALIGN_WIDTH);
	    stVbConf.astCommPool[1].u32BlkSize = u32BlkSize;
	    stVbConf.astCommPool[1].u32BlkCnt =10*2;
	  }

    /******************************************
     step 2: mpp system init. 
    ******************************************/
    s32Ret = SAMPLE_COMM_SYS_Init(&stVbConf);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("system init failed with %d!\n", s32Ret);
        goto END_VENC_1080P_CLASSIC_0;
    }

    /******************************************
     step 3: start vi dev & chn to capture
    ******************************************/
    stViConfig.enViMode   = SENSOR_TYPE;
    stViConfig.enRotate   = ROTATE_NONE;
    stViConfig.enNorm     = VIDEO_ENCODING_MODE_AUTO;
    stViConfig.enViChnSet = (SENSOR_TYPE == SONY_IMX334_MIPI_4K_30FPS)?VI_CHN_SET_FLIP:VI_CHN_SET_NORMAL; // maohw VI_CHN_SET_FLIP for sml imx334;
    stViConfig.enWDRMode  = WDR_MODE_NONE;
    s32Ret = SAMPLE_COMM_VI_StartVi(&stViConfig);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("start vi failed!\n");
        goto END_VENC_1080P_CLASSIC_1;
    }
    
    //start-vi2;
    stViConfig2.enViMode   = SAMPLE_VI_MODE_BT1120_1080P;
    stViConfig2.enRotate   = ROTATE_NONE;
    stViConfig2.enNorm     = VIDEO_ENCODING_MODE_AUTO;
    stViConfig2.enViChnSet = VI_CHN_SET_NORMAL;
	  stViConfig2.enWDRMode  = WDR_MODE_NONE;
    s32Ret = SAMPLE_COMM_VI_StartVi2(&stViConfig2);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("start vi2 failed!\n");
        goto END_VENC_1080P_CLASSIC_1;
    }


  return s32Ret;
  
END_VENC_1080P_CLASSIC_1:	//vi stop
    SAMPLE_COMM_VI_StopVi(&stViConfig);
END_VENC_1080P_CLASSIC_0:	//system exit
    SAMPLE_COMM_SYS_Exit();
  return s32Ret;
}
int gsf_mpp_vi_stop()
{
  SAMPLE_COMM_SYS_Exit();
  return 0;
}


int gsf_mpp_vpss_start(gsf_mpp_vpss_t *vpss)
{
    PIC_SIZE_E enSize[2] = {PIC_UHD4K, PIC_HD1080};
    HI_S32   ViPipe = vpss->ViPipe;
    VPSS_GRP VpssGrp = vpss->VpssGrp;
    VPSS_CHN VpssChn = 0;
    VPSS_GRP_ATTR_S stVpssGrpAttr = {0};
    VPSS_CHN_ATTR_S stVpssChnAttr = {0};
    VPSS_CHN_MODE_S stVpssChnMode;

    HI_S32 s32Ret = HI_SUCCESS;
    SIZE_S stSize;

    /******************************************
     step  1: init sys variable 
    ******************************************/

    SAMPLE_COMM_VI_GetSizeBySensor(&enSize[0]);
    
    /******************************************
     step 4: start vpss and vi bind vpss
    ******************************************/
    s32Ret = SAMPLE_COMM_SYS_GetPicSize(VIDEO_ENCODING_MODE_PAL, enSize[0], &stSize);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("SAMPLE_COMM_SYS_GetPicSize failed!\n");
        goto END_VENC_1080P_CLASSIC_1;
    }
    
    //sensor wh size;
    stVpssGrpAttr.u32MaxW = stSize.u32Width;
    stVpssGrpAttr.u32MaxH = stSize.u32Height;
    stVpssGrpAttr.bIeEn = HI_FALSE;
    stVpssGrpAttr.bNrEn = HI_TRUE;
    stVpssGrpAttr.stNrAttr.enNrType = VPSS_NR_TYPE_VIDEO;
    stVpssGrpAttr.stNrAttr.stNrVideoAttr.enNrRefSource = VPSS_NR_REF_FROM_RFR;
    stVpssGrpAttr.stNrAttr.u32RefFrameNum = 2;
    stVpssGrpAttr.bHistEn = HI_FALSE;
    stVpssGrpAttr.bDciEn = HI_FALSE;
    stVpssGrpAttr.enDieMode = VPSS_DIE_MODE_NODIE;
    stVpssGrpAttr.enPixFmt = SAMPLE_PIXEL_FORMAT;
    stVpssGrpAttr.bStitchBlendEn   = HI_FALSE;

    s32Ret = SAMPLE_COMM_VPSS_StartGroup(VpssGrp, &stVpssGrpAttr);
    if (HI_SUCCESS != s32Ret)
    {
	    SAMPLE_PRT("Start Vpss failed!\n");
	    goto END_VENC_1080P_CLASSIC_2;
    }

    s32Ret = SAMPLE_COMM_VI_BindVpss2(SENSOR_TYPE, VpssGrp, ViPipe);
    if (HI_SUCCESS != s32Ret)
    {
	    SAMPLE_PRT("Vi bind Vpss failed!\n");
	    goto END_VENC_1080P_CLASSIC_3;
    }

    int i;
    for(i = 0; i < VPSS_MAX_PHY_CHN_NUM; i++)
    {
      if(!vpss->enable[i])
      {
        continue;
      }
      
  		s32Ret = SAMPLE_COMM_SYS_GetPicSize(VIDEO_ENCODING_MODE_PAL, vpss->enSize[i], &stSize);
  		if (HI_SUCCESS != s32Ret)
  		{
  			SAMPLE_PRT(	"SAMPLE_COMM_SYS_GetPicSize failed!\n");
  			goto END_VENC_1080P_CLASSIC_4;
  		}
  		
	    VpssChn = i;
	    stVpssChnMode.enChnMode      = VPSS_CHN_MODE_USER;
	    stVpssChnMode.bDouble        = HI_FALSE;
	    stVpssChnMode.enPixelFormat  = SAMPLE_PIXEL_FORMAT;
	    stVpssChnMode.u32Width       = stSize.u32Width;
	    stVpssChnMode.u32Height      = stSize.u32Height;
	    stVpssChnMode.enCompressMode = COMPRESS_MODE_SEG;
	    memset(&stVpssChnAttr, 0, sizeof(stVpssChnAttr));
	    stVpssChnAttr.s32SrcFrameRate = -1;
	    stVpssChnAttr.s32DstFrameRate = -1;
	    s32Ret = SAMPLE_COMM_VPSS_EnableChn(VpssGrp, VpssChn, &stVpssChnAttr, &stVpssChnMode, HI_NULL);
	    if (HI_SUCCESS != s32Ret)
	    {
		    SAMPLE_PRT("Enable vpss chn failed!\n");
		    goto END_VENC_1080P_CLASSIC_4;
	    }
    }
  
    return s32Ret;
    
END_VENC_1080P_CLASSIC_4:	//vpss stop
  for(i = 0; i < VPSS_MAX_PHY_CHN_NUM; i++)
  {
    if(!vpss->enable[i])
    {
      continue;
    }
    VpssGrp = 0;
    VpssChn = i;
    SAMPLE_COMM_VPSS_DisableChn(VpssGrp, VpssChn);
  }
END_VENC_1080P_CLASSIC_3:    //vpss stop       
    SAMPLE_COMM_VI_UnBindVpss(SENSOR_TYPE);
END_VENC_1080P_CLASSIC_2:    //vpss stop   
    SAMPLE_COMM_VPSS_StopGroup(VpssGrp);
END_VENC_1080P_CLASSIC_1:	//vi stop
    return s32Ret;
}

int gsf_mpp_vpss_stop(gsf_mpp_vpss_t *vpss)
{
  HI_S32 s32Ret = HI_SUCCESS;
  VPSS_GRP VpssGrp;
  VPSS_CHN VpssChn;  

  int i;
  for(i = 0; i < VPSS_MAX_PHY_CHN_NUM; i++)
  {
    if(!vpss->enable[i])
    {
      continue;
    }
    VpssGrp = 0;
    VpssChn = i;
    SAMPLE_COMM_VPSS_DisableChn(VpssGrp, VpssChn);
  }
  SAMPLE_COMM_VI_UnBindVpss(SENSOR_TYPE);
  SAMPLE_COMM_VPSS_StopGroup(VpssGrp);
  return s32Ret;
}


int gsf_mpp_venc_start(gsf_mpp_venc_t *venc)
{
  HI_S32 s32Ret = HI_SUCCESS;

  VENC_GOP_ATTR_S stGopAttr;
  s32Ret = SAMPLE_COMM_VENC_GetGopAttr(venc->enGopMode,&stGopAttr,VIDEO_ENCODING_MODE_PAL);
  if (HI_SUCCESS != s32Ret)
  {
      SAMPLE_PRT("Get GopAttr failed!\n");
      goto END_VENC_1080P_CLASSIC_5;
  }		

  s32Ret = SAMPLE_COMM_VENC_Start(venc->VencChn
                                , venc->enPayLoad
                                , VIDEO_ENCODING_MODE_PAL
                                , venc->enSize
                                , venc->enRcMode
                                , venc->u32Profile
                                , &stGopAttr);
  if (HI_SUCCESS != s32Ret)
  {
      SAMPLE_PRT("Start Venc failed!\n");
      goto END_VENC_1080P_CLASSIC_5;
  }

  s32Ret = SAMPLE_COMM_VENC_BindVpss(venc->VencChn, venc->VpssGrp, venc->VpssChn);
  if (HI_SUCCESS != s32Ret)
  {
      SAMPLE_PRT("Start Venc failed!\n");
      goto END_VENC_1080P_CLASSIC_5;
  }

	return s32Ret;

END_VENC_1080P_CLASSIC_5:
  SAMPLE_COMM_VENC_UnBindVpss(venc->VencChn, venc->VpssGrp, venc->VpssChn);
  SAMPLE_COMM_VENC_Stop(venc->VencChn);
  return s32Ret;
}
int gsf_mpp_venc_stop(gsf_mpp_venc_t *venc)
{
  SAMPLE_COMM_VENC_UnBindVpss(venc->VencChn, venc->VpssGrp, venc->VpssChn);
  SAMPLE_COMM_VENC_Stop(venc->VencChn); 
  return 0;
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
  int ret = -1;
  
  //path;
  ret = HI_SCENEAUTO_Init(path);
  if (ret != 0)
  {
      printf("HI_SCENEAUTO_Init failed\n");
      return -1;
  }
  
  //scenemode;
  ret = HI_SCENEAUTO_Start();
  
  return ret;
}
int gsf_mpp_scene_stop()
{
  HI_SCENEAUTO_DeInit();
  return 0;
}

int gsf_mpp_venc_ctl(int VencChn, int id, void *args)
{
  return 0;
}
int gsf_mpp_isp_ctl(int ch, int id, void *args)
{
  return 0;
}

int gsf_mpp_audio_start(gsf_mpp_aenc_t *aenc)
{
  return 0;
}
int gsf_mpp_audio_stop(gsf_mpp_aenc_t  *aenc)
{
  return 0;
}


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





