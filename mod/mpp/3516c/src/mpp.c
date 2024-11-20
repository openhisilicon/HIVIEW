#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <poll.h>
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
#include <libgen.h>


#include "ot_scene.h"
#include "scene_loadparam.h"

#include "hi_mpi_ae.h"
#include "sample_comm.h"

#include "hi_audio_aac_adp.h"

#include "mpp.h"
//#include "mppex.h"

static hi_s32 aiisp = 0;

static int _audio_init = 0;
static gsf_mpp_aenc_t _adec, _aenc = {
  .AeChn = 0,
  .enPayLoad = PT_AAC,
  .adtype = 0, // 0:INNER, 1:I2S, 2:PCM;
  .stereo = 1, 
  .sp = 48000, //sampleRate
  .br = 0,    //bitRate;
  .uargs = NULL,
  .cb = NULL,
};


typedef struct {
  int   type;
  char* name;
  char* lib;
  char* obj;
}SAMPLE_MPP_SENSOR_T;

int SENSOR_TYPE;
int SENSOR0_TYPE;
int SENSOR1_TYPE;
hi_isp_sns_obj* SENSOR_OBJ = NULL;
hi_isp_sns_obj *sample_comm_isp_get_sns_obj(sample_sns_type sns_type)
{
  return SENSOR_OBJ;
}

static SAMPLE_MPP_SENSOR_T libsns[SNS_TYPE_BUTT] = {
    {SC4336P_MIPI_4M_30FPS_10BIT,             "sc4336p-0-0-4-30", "libsns_sc4336p.so",    "g_sns_sc4336p_obj"},
    {OS04D10_MIPI_4M_30FPS_10BIT,             "os04d10-0-0-4-30", "libsns_os04d10.so",    "g_sns_os04d10_obj"},
    {GC4023_MIPI_4M_30FPS_10BIT,              "gc4023-0-0-4-30", "libsns_gc4023.so",      "g_sns_gc4023_obj"},
    {SC431HAI_MIPI_4M_30FPS_10BIT,            "sc431hai-0-0-4-30", "libsns_sc431hai.so",  "g_sns_sc431hai_obj"},
    {SC431HAI_MIPI_4M_30FPS_10BIT_WDR2TO1,    "sc431hai-0-1-4-30", "libsns_sc431hai.so",  "g_sns_sc431hai_obj"},
    {SC450AI_MIPI_4M_30FPS_10BIT,             "sc450ai-0-0-4-30", "libsns_sc450ai.so",    "g_sns_sc450ai_obj"},
    {SC450AI_MIPI_4M_30FPS_10BIT_WDR2TO1,     "sc450ai-0-1-4-30", "libsns_sc450ai.so",    "g_sns_sc450ai_obj"},
    {SC500AI_MIPI_5M_30FPS_10BIT,             "sc500ai-0-0-5-30", "libsns_sc500ai.so",    "g_sns_sc500ai_obj"},
    {SC500AI_MIPI_5M_30FPS_10BIT_WDR2TO1,     "sc500ai-0-1-5-30", "libsns_sc500ai.so",    "g_sns_sc500ai_obj"},
    {OV_OS04A10_MIPI_4M_30FPS_12BIT,          "os04a10-0-0-4-30", "libsns_os04a10.so",    "g_sns_os04a10_obj"},
  };


static SAMPLE_MPP_SENSOR_T* SAMPLE_MPP_SERSOR_GET(char* name)
{
  int i = 0;
  for(i = 0; i < SNS_TYPE_BUTT; i++)
  {
    //printf("libsns[%d].name:%s, name:%s\n", i, libsns[i].name, name);
    if(strstr(libsns[i].name, name))
    {
      return &libsns[i];
    }
  }
  return NULL;
}


static void * dl = NULL;
static int snscnt = 0;

hi_void sample_venc_handle_sig2(hi_s32 signo)
{
  int ret = 0;
  
  signal(SIGINT, SIG_IGN);
  signal(SIGTERM, SIG_IGN);
  if (signo == SIGINT || signo == SIGTERM) 
  {
      //mppex_hook_destroy();
      ret = gsf_mpp_scene_stop();
      printf("gsf_mpp_scene_stop ret = %x\n", ret);
      ret = gsf_mpp_venc_dest();
      printf("gsf_mpp_venc_dest ret = %x\n", ret);
      
      for(int i = 0; i < 3*4; i++) // 4channels;
      {
        hi_mpp_chn src_chn;
        hi_mpp_chn dst_chn;
        dst_chn.mod_id = HI_ID_VENC;
        dst_chn.dev_id = 0;
        dst_chn.chn_id = i;
    
        if(hi_mpi_sys_get_bind_by_dst(&dst_chn, &src_chn) == 0)
        {
          sample_comm_vpss_un_bind_venc(src_chn.dev_id, src_chn.chn_id, dst_chn.chn_id);
          sample_comm_venc_stop(dst_chn.chn_id);
        }  
      }  
      
      //extern hi_s32 sample_audio_ai_aenc_stop(gsf_mpp_aenc_t *aenc);
      //ret = sample_audio_ai_aenc_stop(&_aenc);
      //printf("sample_audio_ai_aenc_stop ret = %x\n", ret);
      
      sample_comm_all_isp_stop();
      printf("sample_comm_all_isp_stop\n");
      
      sample_comm_sys_exit();
      printf("sample_comm_sys_exit\n");
  }
  exit(-1);
}


int gsf_mpp_cfg_sns(char *path, gsf_mpp_cfg_t *cfg)
{
  int i = 0;
  char snsstr[64];
  sprintf(snsstr, "%s-%d-%d-%d-%d"
          , cfg->snsname, cfg->lane, cfg->wdr, cfg->res, cfg->fps);
  
  SAMPLE_MPP_SENSOR_T* sns = SAMPLE_MPP_SERSOR_GET(snsstr);
  if(!sns)
  {
    printf("%s => snsstr:%s, unknow.\n", __func__, snsstr);
    return -1;
  } 
  
  snscnt = cfg->snscnt;
  aiisp = cfg->aiisp;
    
  char loadstr[256] = {0};
  char snsname[64] = {0};
 
  if(strstr(cfg->snsname, "os04a10"))
    strncpy(snsname, "os04d10", sizeof(snsname)-1);
  else 
    strncpy(snsname, cfg->snsname, sizeof(snsname)-1);
  
  sprintf(loadstr, "%s/ko/load3516cv610/load3516cv610_00s_debug -i -sensor0 %s", path, snsname);
  for(i = 1; i < snscnt; i++)
  {
    sprintf(loadstr, "%s -sensor%d %s", loadstr, i, snsname);
  }
  
  printf("%s => loadstr: %s\n", __func__, loadstr);
  system(loadstr);
  
  SENSOR_TYPE = SENSOR0_TYPE = SENSOR1_TYPE = sns->type;
  
  if(cfg->second && cfg->snscnt == 1)
  {
    SENSOR1_TYPE = SNS_TYPE_BUTT;
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
    
    SENSOR_OBJ = dlsym(dl, sns->obj);
    if(NULL != dlerror())
    {
        printf("err dlsym %s\n", sns->obj);
        goto __err;
    }
  }
  printf("%s => snsstr:%s, sensor_type:%d, load:%s\n"
        , __func__, snsstr, SENSOR_TYPE, sns->lib?:"");
  
  signal(SIGINT, sample_venc_handle_sig2);
  signal(SIGTERM, sample_venc_handle_sig2);
  
  return 0;
__err:
	if(dl)
	{
    dlclose(dl);
  }
  dlerror();
  return -1;
}

int gsf_mpp_cfg(char *path, gsf_mpp_cfg_t *cfg)
{
  return gsf_mpp_cfg_sns(path, cfg);
}

static sample_venc_vpss_chn_attr vpss_param;
static sample_venc_vb_attr vb_attr = {0};
static sample_vi_cfg vi_cfg[HI_VI_MAX_DEV_NUM];


//SAMPLE_COMM_VI_StartVi
int gsf_mpp_vi_start(gsf_mpp_vi_t *vi)
{
  hi_s32 ret = 0, i = 0;
  hi_size enc_size[CHN_NUM_MAX];
  hi_size vi_size = {0};
  hi_pic_size pic_size[CHN_NUM_MAX] = {PIC_1080P, PIC_D1_NTSC};

  for (i = 0; i < CHN_NUM_MAX; i++) {
      //check venc_pic_size;
      pic_size[i] = (vi->venc_pic_size[i])?vi->venc_pic_size[i]:pic_size[i];
    
      ret = sample_comm_sys_get_pic_size(pic_size[i], &enc_size[i]);
      if (ret != HI_SUCCESS) {
          sample_print("sample_comm_sys_get_pic_size failed!\n");
          return ret;
      }
  }

  // get vi param
  for(i = 0; i < snscnt; i++)
  {
    sample_comm_vi_get_default_vi_cfg(SENSOR0_TYPE, &vi_cfg[i]);
    //sample_comm_vi_get_mipi_info_by_dev_id(SENSOR0_TYPE, i, &vi_cfg[i].mipi_info);
  }
  
  //mppex_comm_vi_bb(snscnt, vi_cfg);
  
  // get vpss param
  vi_size = vi_cfg[0].dev_info.dev_attr.in_size;
  extern hi_void get_default_vpss_chn_attr(hi_size *vi_size, hi_size enc_size[], hi_s32 len,
            sample_venc_vpss_chn_attr *vpss_chan_attr);
  get_default_vpss_chn_attr(&vi_size, enc_size, CHN_NUM_MAX, &vpss_param);

  /******************************************
    step 1: init sys alloc common vb
  ******************************************/
  extern hi_void get_vb_attr(const hi_size *vi_size, const sample_venc_vpss_chn_attr *vpss_chn_attr,
            sample_venc_vb_attr *vb_attr);
  get_vb_attr(&vi_size, &vpss_param, &vb_attr);


  //HI_VB_SUPPLEMENT_BNR_MOT_MASK
  extern hi_s32 sample_venc_sys_init(sample_venc_vb_attr *vb_attr);
  if ((ret = sample_venc_sys_init(&vb_attr)) != HI_SUCCESS) {
      sample_print("Init SYS ret for %#x!\n", ret);
      return ret;
  }
  
  //vi_vpss_mode;
  sample_comm_vi_set_vi_vpss_mode(HI_VI_ONLINE_VPSS_ONLINE, HI_VI_AIISP_MODE_DEFAULT);
  
  sample_sns_type sns_type = SENSOR0_TYPE;
 
  for(i = 0; i < snscnt; i++)
  {
    extern hi_s32 sample_venc_vi_init(sample_vi_cfg *vi_cfg); //sample_comm_vi_start_vi(vi_cfg);
    {   
      ret = sample_venc_vi_init(&vi_cfg[i]);
      printf("sample_venc_vi_init ret:%d\n", ret);
    }
    if (ret != HI_SUCCESS) {
        sample_print("Init VI i:%d ret:%#x!\n", i, ret);
        goto EXIT_SYS_STOP;
    }
  }
  
  //ret = mppex_comm_vi_ee(snscnt, vi_cfg);
  
  return ret;

EXIT_SYS_STOP:
    sample_comm_sys_exit();
  return ret;
}

int gsf_mpp_vi_stop()
{
  int ret = 0, i = 0;
  
  extern hi_void sample_venc_vi_deinit(sample_vi_cfg *vi_cfg);//sample_comm_vi_stop_vi(vi_cfg);
  
  for(i = 0; i < snscnt; i++)
  {
    {  
      sample_venc_vi_deinit(&vi_cfg[i]);
    }  
  }

  //mppex_comm_vi_tt(snscnt, vi_cfg);

  sample_comm_sys_exit();
  return ret;
}
//HI_S32 HI_MPI_VI_GetPipeFrame(VI_PIPE ViPipe, VIDEO_FRAME_INFO_S *pstVideoFrame, HI_S32 s32MilliSec);
//HI_S32 HI_MPI_VI_GetChnFrame(VI_PIPE ViPipe, VI_CHN ViChn, VIDEO_FRAME_INFO_S *pstFrameInfo, HI_S32 s32MilliSec);
int gsf_mpp_vi_get(int ViPipe, int ViChn, VIDEO_FRAME_INFO_S *pstFrameInfo, int s32MilliSec)
{
  int ret = 0;
  return ret;
}

//from sample_af.c;
extern int sample_af_main(gsf_mpp_af_t *af);


int gsf_mpp_af_start(gsf_mpp_af_t *af)
{
  return 0;//sample_af_main(af);
}


//vpss;
extern hi_void sample_venc_vpss_deinit(hi_vpss_grp vpss_grp, sample_venc_vpss_chn_attr *vpss_chan_cfg);

//SAMPLE_COMM_VI_Bind_VPSS
int gsf_mpp_vpss_start(gsf_mpp_vpss_t *vpss)
{
  int ret = 0;
  hi_vi_pipe vi_pipe  = vpss->ViPipe;
  hi_vi_chn vi_chn    = vpss->ViChn;
  hi_vpss_grp vpss_grp= vpss->VpssGrp;
    
  //vi_pipe = mppex_comm_vpss_bb(vpss, &vpss_param);
  
  extern hi_s32 sample_venc_vpss_init(hi_vpss_grp vpss_grp, sample_venc_vpss_chn_attr *vpss_chan_cfg);
  if ((ret = sample_venc_vpss_init(vpss_grp, &vpss_param)) != HI_SUCCESS) {
      sample_print("Init VPSS err for %#x!\n", ret);
      goto EXIT;
  }
  
  if ((ret = sample_comm_vi_bind_vpss(vi_pipe, vi_chn, vpss_grp, 0)) != HI_SUCCESS) 
  {
      sample_print("VI Bind VPSS err for %#x!\n", ret);
      goto EXIT_VPSS_STOP;
  }
  
  //mppex_comm_vpss_ee(vpss, &vpss_param);
EXIT:
  return ret;
  
EXIT_VPSS_STOP:
  sample_venc_vpss_deinit(vpss_grp, &vpss_param);
  return ret;
}

int gsf_mpp_vpss_stop(gsf_mpp_vpss_t *vpss)
{
  int ret = 0;
  hi_vi_pipe vi_pipe  = vpss->ViPipe;
  hi_vi_chn vi_chn    = vpss->ViChn;
  hi_vpss_grp vpss_grp= vpss->VpssGrp;

  sample_comm_vi_un_bind_vpss(vi_pipe, vi_chn, vpss_grp, 0);
  sample_venc_vpss_deinit(vpss_grp, &vpss_param);
  return ret;
}
//HI_S32 HI_MPI_VPSS_SendFrame VPSS_GRP VpssGrp, VPSS_GRP_PIPE VpssGrpPipe, const VIDEO_FRAME_INFO_S *pstVideoFrame , HI_S32 s32MilliSec);
int gsf_mpp_vpss_send(int VpssGrp, int VpssGrpPipe, VIDEO_FRAME_INFO_S *pstVideoFrame , int s32MilliSec)
{
  int ret = 0;
  return ret;
}

int gsf_mpp_vpss_ctl(int VpssGrp, int id, void *args)
{
  int ret = -1;
  switch(id)
  {
    case GSF_MPP_VPSS_CTL_PAUSE:
      ret = hi_mpi_vpss_stop_grp(VpssGrp);
      if(ret)
        printf("GSF_MPP_VPSS_CTL_PAUSE VpssGrp:%d, err 0x%x\n", VpssGrp, ret);    
      break;
    case GSF_MPP_VPSS_CTL_RESUM:  
      ret = hi_mpi_vpss_start_grp(VpssGrp);
      if(ret)  
        printf("GSF_MPP_VPSS_CTL_RESUM VpssGrp:%d, err 0x%x\n", VpssGrp, ret);
      break;
    case GSF_MPP_VPSS_CTL_CROP:
      ret = hi_mpi_vpss_set_grp_crop(VpssGrp, (hi_vpss_crop_info*)args);
      if(ret)
        printf("GSF_MPP_VPSS_CTL_CROP VpssGrp:%d, err 0x%x\n", VpssGrp, ret);
      break;
    case GSF_MPP_VPSS_CTL_ASPECT:
      {
        int i = 0;
        hi_vpss_chn_attr chn_attr;
    
        for(i = 0; i < 2; i++)
        {
          if(hi_mpi_vpss_get_chn_attr(VpssGrp, i, &chn_attr) == HI_SUCCESS)
          {
            chn_attr.aspect_ratio = *((hi_aspect_ratio*)args);
            ret = hi_mpi_vpss_set_chn_attr(VpssGrp, i, &chn_attr);
            if(ret)
              printf("GSF_MPP_VPSS_CTL_ASPECT VpssGrp:%d,VpssChn:%d err 0x%x\n", VpssGrp, i, ret);
          }
        }
      }
      break;
    case  GSF_MPP_VPCH_CTL_ENABLE:
      ret = hi_mpi_vpss_enable_chn(VpssGrp, (int)args);
      if(ret)
        printf("GSF_MPP_VPCH_CTL_ENABLE VpssGrp:%d,VpssChn:%d err 0x%x\n", VpssGrp, (int)args, ret); 
      break;
    case GSF_MPP_VPCH_CTL_DISABLE:
      ret = hi_mpi_vpss_disable_chn(VpssGrp, (int)args);
      if(ret)
        printf("GSF_MPP_VPCH_CTL_DISABLE VpssGrp:%d,VpssChn:%d err 0x%x\n", VpssGrp, (int)args, ret); 
      break;
    case GSF_MPP_VPSS_CTL_ATTR:
      ret = hi_mpi_vpss_get_grp_attr(VpssGrp, (hi_vpss_grp_attr*)args);
      if(ret)
        printf("GSF_MPP_VPSS_CTL_ATTR VpssGrp:%d, err 0x%x\n", VpssGrp, ret); 
      break;
  }
  return ret;
}

//启动编码通道

int gsf_mpp_venc_start(gsf_mpp_venc_t *venc)
{
  int ret = 0;
  hi_venc_gop_mode gop_mode;
  hi_venc_gop_attr gop_attr;
  
  hi_vpss_grp vpss_grp = venc->VpssGrp;
  hi_vpss_chn vpss_chn = venc->VpssChn;
  hi_venc_chn venc_chn = venc->VencChn;
  
  sample_comm_venc_chn_param venc_create_param = {0};

  gop_mode = HI_VENC_GOP_MODE_NORMAL_P;
  if ((ret = sample_comm_venc_get_gop_attr(gop_mode, &gop_attr)) != HI_SUCCESS) {
      sample_print("Venc Get GopAttr for %#x!\n", ret);
      return ret;
  }

  venc_create_param.frame_rate = 30; /* 30: is a number */
  venc_create_param.gop = 60; /* 60: is a number */
  venc_create_param.stats_time = 2; /* 2: is a number */
  venc_create_param.gop_attr                  = gop_attr;
  venc_create_param.type                      = venc->enPayLoad;// HI_PT_H265;
  venc_create_param.size                      = venc->enSize; // BIG_STREAM_SIZE//SMALL_STREAM_SIZE
  venc_create_param.rc_mode                   = SAMPLE_RC_CBR;//venc->enRcMode;//SAMPLE_RC_CBR;
  venc_create_param.profile                   = 0;
  venc_create_param.is_rcn_ref_share_buf      = HI_TRUE;
  venc_create_param.frame_rate                = venc->u32FrameRate; /* 30 is a number */
  venc_create_param.gop                       = venc->u32Gop; /* 30 is a number */
  venc_create_param.bitrate                   = venc->u32BitRate;
  
  /* encode h.264 */
  ret = sample_comm_venc_start(venc_chn, &venc_create_param);
  if (ret != HI_SUCCESS) {
      sample_print("Venc Start failed for %#x!\n", ret);
      goto EXIT;
  }
  if(vpss_grp >= 0)
  {
    if ((ret = sample_comm_vpss_bind_venc(vpss_grp, vpss_chn, venc_chn)) != HI_SUCCESS) {
        sample_print("sample_comm_vpss_bind_venc failed for %#x!\n", ret);
        goto EXIT_VENC_H264_STOP;
    }
  }
  
  printf("%s => venc_chn:%d, type:%d, vpss_grp:%d, vpss_chn:%d\n", __func__, venc_chn, venc_create_param.type, vpss_grp, vpss_chn);
  
  return ret;

EXIT_VENC_H264_STOP:
    sample_comm_venc_stop(venc_chn);
EXIT:
  return ret;
}
//停止编码通道
int gsf_mpp_venc_stop(gsf_mpp_venc_t *venc)
{
  int ret = 0;
  hi_venc_gop_mode gop_mode;
  hi_venc_gop_attr gop_attr;

  hi_vpss_grp vpss_grp = venc->VpssGrp;
  hi_vpss_chn vpss_chn = venc->VpssChn;
  hi_venc_chn venc_chn = venc->VencChn;
  if(vpss_grp >= 0)
    sample_comm_vpss_un_bind_venc(vpss_grp, vpss_chn, venc_chn);
  sample_comm_venc_stop(venc_chn);
  return ret;
}

int gsf_mpp_venc_ctl(int VencChn, int id, void *args)
{
  int ret = 0;
  
  #if 0
  hi_venc_chn_attr venc_attr;
  ret = hi_mpi_venc_get_chn_attr(VencChn, &venc_attr);
  ret = hi_mpi_venc_set_chn_attr(VencChn, &venc_attr);
  #endif
  
  return ret;
}

int gsf_mpp_venc_send(int VeChn, VIDEO_FRAME_INFO_S *pstFrame, int s32MilliSec, gsf_mpp_venc_get_t *get)
{
  int ret = 0;
  
  if(!pstFrame)
    return -1;
    
  hi_venc_chn_attr stChnAttr;
  
  ret = hi_mpi_venc_get_chn_attr(VeChn, &stChnAttr);
  if(ret != 0)
    return -1;

  if(pstFrame->video_frame.width > stChnAttr.venc_attr.max_pic_width
    || pstFrame->video_frame.height > stChnAttr.venc_attr.max_pic_height)
    return -1;
   
  if(get && get->cb)
  {
    stChnAttr.venc_attr.pic_width = pstFrame->video_frame.width;
    stChnAttr.venc_attr.pic_height = pstFrame->video_frame.height;
    ret = hi_mpi_venc_set_chn_attr(VeChn, &stChnAttr);
    
    hi_venc_start_param  stRecvParam;
    stRecvParam.recv_pic_num = 1;
    ret = hi_mpi_venc_start_chn(VeChn, &stRecvParam);
 
    ret = hi_mpi_venc_send_frame(VeChn, pstFrame, s32MilliSec);
 
    hi_venc_stream stStream;
    hi_venc_pack   packs[4];
    stStream.pack_cnt = 4;
    stStream.pack = packs;
    
    ret = hi_mpi_venc_get_stream(VeChn, &stStream, s32MilliSec);
    if(ret == 0)
    {
      get->cb(&stStream, get->u);
      ret = hi_mpi_venc_release_stream(VeChn, &stStream);
    }
    
    ret = hi_mpi_venc_stop_chn(VeChn);
    return ret;
  }
    
  if(pstFrame->video_frame.width != stChnAttr.venc_attr.pic_width
    || pstFrame->video_frame.height != stChnAttr.venc_attr.pic_height)
  {
    ret = hi_mpi_venc_stop_chn(VeChn);
    
    stChnAttr.venc_attr.pic_width = pstFrame->video_frame.width;
    stChnAttr.venc_attr.pic_height = pstFrame->video_frame.height;
    ret = hi_mpi_venc_set_chn_attr(VeChn, &stChnAttr);
    
    hi_venc_start_param  stRecvParam;
    stRecvParam.recv_pic_num = 1;
    ret = hi_mpi_venc_start_chn(VeChn, &stRecvParam);
  }
  ret = hi_mpi_venc_send_frame(VeChn, pstFrame, s32MilliSec);
  return ret;
}

//启动接收线程
int gsf_mpp_venc_recv(gsf_mpp_recv_t *recv)
{
  int ret = 0;

  if ((ret = sample_comm_venc_start_get_streamCB(recv->VeChn, recv->s32Cnt, recv->cb, recv->uargs)) != HI_SUCCESS) 
  {
      sample_print("Start Venc failed!\n");
  } 
  
  return ret;
}
//停止接收线程
int gsf_mpp_venc_dest()
{
  int ret = 0;
  sample_comm_venc_stop_get_stream(CHN_NUM_MAX);
  return ret;
}

int gsf_mpp_venc_snap(VENC_CHN VencChn, HI_U32 SnapCnt, int(*cb)(int i, VENC_STREAM_S* pstStream, void* u), void* u)
{
  return sample_comm_venc_snap_processCB(VencChn, SnapCnt, cb, u);
}

static int g_scenebEnable = 0;

int gsf_mpp_scene_start(char *path, int scenemode)
{
    char _path[256] = {0};
    
    strncpy(_path, path, sizeof(_path)-1);
    strcat(_path, (snscnt==2)?".2ch":"");
    
    char *dir_name = dirname(_path);
    
    sample_sns_type sns_type = SENSOR0_TYPE;  

    HI_S32 ret = HI_SUCCESS;
    ot_scene_param stSceneParam;
    ot_scene_video_mode stVideoMode;
    
    set_dir_name(dir_name); // for FPN_frame_xxx.raw file;
    
    ret = ot_scene_create_param(path, &stSceneParam, &stVideoMode);
    if (ret != HI_SUCCESS) {
        printf("SCENETOOL_CreateParam failed\n");
        return HI_FAILURE;
    }

    ret = ot_scene_init(&stSceneParam);
    if (ret != HI_SUCCESS) {
        printf("ot_scene_init failed\n");
        return HI_FAILURE;
    }

    ret = ot_scene_set_scene_mode(&(stVideoMode.video_mode[scenemode]));
    if (ret != HI_SUCCESS) {
        printf("HI_SRDK_SCENEAUTO_Start failed\n");
        return HI_FAILURE;
    }

    printf("The sceneauto is started\n");
    g_scenebEnable = 1;
    return ret;
}
int gsf_mpp_scene_stop()
{
  hi_s32 ret = ot_scene_deinit();
  if (ret != HI_SUCCESS) {
      printf("ot_scene_deinit failed\n");
  }


  printf("The scene sceneauto is stop\n");
  return ret;
}


//from sample_ir_auto.c;
extern hi_s32 isp_ir_switch_to_ir(hi_vi_pipe vi_pipe);
extern hi_s32 isp_ir_switch_to_normal(hi_vi_pipe vi_pipe);
extern hi_s32 isp_ir_switch_to_auto(hi_vi_pipe vi_pipe);
extern hi_s32 isp_ir_mode(gsf_mpp_ir_t *ir);

int gsf_mpp_isp_ctl(int ViPipe, int id, void *args)
{
  int ret = -1;
  
  switch(id)
  {
  case GSF_MPP_ISP_CTL_IR:
    #if 1
    switch((int)args)
    {
      case 0:
          isp_ir_mode((gsf_mpp_ir_t*)NULL);
          ret = isp_ir_switch_to_normal(ViPipe);
        break;
      case 1:
          isp_ir_mode((gsf_mpp_ir_t*)NULL);
          ret = isp_ir_switch_to_ir(ViPipe);
        break;
      default:
          isp_ir_mode((gsf_mpp_ir_t*)args);
          ret = isp_ir_switch_to_auto(ViPipe);
        break;
    }
    #endif
    printf("GSF_MPP_ISP_CTL_IR, args:%p\n", args);
    break;
  case GSF_MPP_ISP_CTL_IMG:
    {
      gsf_mpp_img_all_t *all = (gsf_mpp_img_all_t*)args;
      
      all->scene.bEnable = g_scenebEnable;
      
      hi_isp_exposure_attr exp_attr;
      ret = hi_mpi_isp_get_exposure_attr(ViPipe, &exp_attr);
      all->ae.u8Speed         = exp_attr.auto_attr.speed;
      all->ae.u8Compensation  = exp_attr.auto_attr.compensation;
      all->ae.SysGainRangeMax = exp_attr.auto_attr.sys_gain_range.max;
      all->ae.SysGainRangeMin = exp_attr.auto_attr.sys_gain_range.min;
      all->ae.ExpTimeRangeMax = exp_attr.auto_attr.exp_time_range.max;
      all->ae.ExpTimeRangeMin = exp_attr.auto_attr.exp_time_range.min;

      hi_isp_sharpen_attr shp_attr;
      ret = hi_mpi_isp_get_sharpen_attr(ViPipe, &shp_attr);
      all->sharpen.u16TextureFreq = shp_attr.manual_attr.texture_freq;
      all->sharpen.u16EdgeFreq    = shp_attr.manual_attr.edge_freq;   
      all->sharpen.u8DetailCtrl   = shp_attr.manual_attr.detail_ctrl; 
      
      hi_ldc_attr ldc_attr;
      ret = hi_mpi_vpss_get_grp_ldc(0, &ldc_attr);
      all->ldc.bEnable = ldc_attr.enable;
      all->ldc.s32DistortionRatio = ldc_attr.ldc_v1_attr.distortion_ratio;

      hi_isp_ldci_attr ldci_attr;
      ret = hi_mpi_isp_get_ldci_attr(ViPipe, &ldci_attr);
      
      all->ldci.u16BlcCtrl = ldci_attr.manual_attr.blc_ctrl;
      all->ldci.stHePosWgt_u8Wgt = ldci_attr.manual_attr.he_wgt.he_pos_wgt.wgt;
      all->ldci.stHeNegWgt_u8Mean = ldci_attr.manual_attr.he_wgt.he_neg_wgt.mean;

      printf("all->scene.bEnable:%d\n", all->scene.bEnable);
    }
    break;
  case GSF_MPP_ISP_CTL_AE:
    {
      gsf_mpp_img_ae_t *ae = (gsf_mpp_img_ae_t*)args;
      
      printf("ae->bEnable:%d, u8Compensation:%d\n", ae->bEnable, ae->u8Compensation);  
      if(!ae->bEnable)
      {
        ret = 0;  
        break;
      }

      hi_isp_exposure_attr exp_attr;
      ret = hi_mpi_isp_get_exposure_attr(ViPipe, &exp_attr);

      //exp_attr.bypass = 0;
      exp_attr.auto_attr.speed              = ae->u8Speed;
      exp_attr.auto_attr.compensation       = ae->u8Compensation;
      exp_attr.auto_attr.sys_gain_range.max = ae->SysGainRangeMax;
      exp_attr.auto_attr.sys_gain_range.min = ae->SysGainRangeMin;
      exp_attr.auto_attr.exp_time_range.max = ae->ExpTimeRangeMax;
      exp_attr.auto_attr.exp_time_range.min = ae->ExpTimeRangeMin;
      
      ret = hi_mpi_isp_set_exposure_attr(ViPipe, &exp_attr);
    }
    break;
  case GSF_MPP_ISP_CTL_SHARPEN:
    {
      gsf_mpp_img_sharpen_t *sharpen = (gsf_mpp_img_sharpen_t*)args;

      printf("sharpen->bEnable:%d,u16TextureFreq:%d\n", sharpen->bEnable, sharpen->u16TextureFreq);  

      hi_isp_sharpen_attr shp_attr;
      ret = hi_mpi_isp_get_sharpen_attr(ViPipe, &shp_attr);
      
      //shp_attr.en = HI_TRUE;
      shp_attr.op_type = (sharpen->bEnable)?HI_OP_MODE_MANUAL:HI_OP_MODE_AUTO;
      shp_attr.manual_attr.texture_freq   = sharpen->u16TextureFreq;
      shp_attr.manual_attr.edge_freq      = sharpen->u16EdgeFreq;
      shp_attr.manual_attr.detail_ctrl    = sharpen->u8DetailCtrl;
      ret = hi_mpi_isp_set_sharpen_attr(ViPipe, &shp_attr);
    }
    break;    
  case GSF_MPP_ISP_CTL_FLIP:
    {
      gsf_mpp_img_flip_t *flip = (gsf_mpp_img_flip_t*)args;

      hi_vi_chn_attr chn_attr;
      ret = hi_mpi_vi_get_chn_attr(ViPipe, 0, &chn_attr);
      printf("GET ret:0x%x, flip_en:%d, mirror_en:%d\n", ret, chn_attr.flip_en, chn_attr.mirror_en);

      chn_attr.flip_en = flip->bFlip;
      chn_attr.mirror_en = flip->bMirror;

      ret = hi_mpi_vi_set_chn_attr(ViPipe, 0, &chn_attr);
      printf("SET ret:0x%x, flip_en:%d, mirror_en:%d\n", ret, chn_attr.flip_en, chn_attr.mirror_en);
    }
    break;
  case GSF_MPP_ISP_CTL_LDC:
    {
      gsf_mpp_img_ldc_t *ldc = (gsf_mpp_img_ldc_t*)args;

      hi_ldc_attr ldc_attr;

      ldc_attr.enable                       = ldc->bEnable;
      ldc_attr.ldc_version                  = HI_LDC_V1;
      ldc_attr.ldc_v1_attr.aspect           = 1;
      ldc_attr.ldc_v1_attr.x_ratio          = 100; /* 100: x ratio */
      ldc_attr.ldc_v1_attr.y_ratio          = 100; /* 100: y ratio */
      ldc_attr.ldc_v1_attr.xy_ratio         = 100; /* 100: x y ratio */
      ldc_attr.ldc_v1_attr.center_x_offset  = 0;
      ldc_attr.ldc_v1_attr.center_y_offset  = 0;
      ldc_attr.ldc_v1_attr.distortion_ratio = ldc->s32DistortionRatio; /* 500: distortion ratio */

      ret = hi_mpi_vpss_set_grp_ldc(0, &ldc_attr);
      printf("vi_set_chn_ldc_attr ret:0x%x, ViPipe:%d, bEnable:%d, s32DistortionRatio:%d\n"
          , ret, ViPipe, ldc_attr.enable, ldc_attr.ldc_v1_attr.distortion_ratio);
    }
    break;
  case GSF_MPP_ISP_CTL_DIS:
    {
      gsf_mpp_img_dis_t *dis = (gsf_mpp_img_dis_t*)args;

      hi_dis_cfg dis_cfg    = {0};
      hi_dis_attr dis_attr  = {0};

      hi_mpi_vi_get_chn_dis_attr(ViPipe, 0, &dis_attr);
      if(dis_attr.enable)
      {
        dis_attr.enable = HI_FALSE;
        hi_mpi_vi_set_chn_dis_attr(ViPipe, 0, &dis_attr);
      }

      dis_cfg.mode              = dis->enMode;//DIS_MODE_4_DOF_GME;//DIS_MODE_6_DOF_GME; //DIS_MODE_4_DOF_GME;
      dis_cfg.motion_level       = HI_DIS_MOTION_LEVEL_NORM;
      dis_cfg.crop_ratio        = 80;
      dis_cfg.buf_num           = 5;
      dis_cfg.frame_rate        = 30;
      dis_cfg.pdt_type          = dis->enPdtType;//DIS_PDT_TYPE_DV;//DIS_PDT_TYPE_IPC; //DIS_PDT_TYPE_DV; DIS_PDT_TYPE_DRONE;
      dis_cfg.scale             = HI_TRUE; //HI_FALSE;
      dis_cfg.camera_steady     = HI_FALSE;

      dis_attr.enable               = dis->bEnable; //HI_TRUE;
      dis_attr.moving_subject_level = 0;
      dis_attr.rolling_shutter_coef = 0;
      dis_attr.timelag              = 0;
      dis_attr.still_crop           = HI_FALSE;
      dis_attr.hor_limit            = 512;
      dis_attr.ver_limit            = 512;
      dis_attr.gdc_bypass           = HI_FALSE;
      dis_attr.strength             = 1024;
          
      dis_cfg.frame_rate  = 30;
      dis_attr.timelag    = 33333;

      ret = hi_mpi_vi_set_chn_dis_cfg(ViPipe, 0, &dis_cfg);
      printf("SET dis config ret:0x%x, enMode:%d, enPdtType:%d\n", ret, dis_cfg.mode, dis_cfg.pdt_type);

      ret = hi_mpi_vi_set_chn_dis_attr(ViPipe, 0, &dis_attr);
      printf("SET dis attr ret:0x%x, bEnable:%d\n", ret, dis_attr.enable);
    }
    break;
  case GSF_MPP_ISP_CTL_LDCI:
    {
      gsf_mpp_img_ldci_t *ldci = (gsf_mpp_img_ldci_t*)args;

      hi_isp_ldci_attr ldci_attr;
      ret = hi_mpi_isp_get_ldci_attr(ViPipe, &ldci_attr);
            
      ldci_attr.enable = 1; //ldci->bEnable;
      ldci_attr.op_type = (ldci->bEnable)?HI_OP_MODE_MANUAL:HI_OP_MODE_AUTO;
      ldci_attr.manual_attr.blc_ctrl = ldci->u16BlcCtrl;
      ldci_attr.manual_attr.he_wgt.he_pos_wgt.wgt = ldci->stHePosWgt_u8Wgt;
      ldci_attr.manual_attr.he_wgt.he_neg_wgt.mean = ldci->stHeNegWgt_u8Mean;        
      ret = hi_mpi_isp_set_ldci_attr(ViPipe, &ldci_attr);
    }
    break;
  default:
    break;
  }
  return ret;
}


int gsf_mpp_scene_ctl(int ViPipe, int id, void *args)
{
  int ret = -1;
  
  if(ViPipe < 0 || ViPipe >= 4)
    return ret;  
#if 0
  extern HI_SCENE_CTL_AE_S g_scene_ctl_ae[4];
  switch(id)
  {
    case GSF_MPP_SCENE_CTL_ALL:
      {
        ret = 0;
        gsf_mpp_scene_all_t *all = (gsf_mpp_scene_all_t*)args;
        
        // default g_scene_ctl_ae[ViPipe].compensation_mul = 1.0
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
#endif  
  return ret;
}


int gsf_mpp_audio_start(gsf_mpp_aenc_t *aenc)
{
  if(!_audio_init)
  {
    _audio_init = 1;
    extern hi_s32 sample_audio_init();
    sample_audio_init();
  }
 
  if(aenc == NULL)
  {
    printf("sample_audio_ai_ao start \n");
    _adec = _aenc;
    static pthread_t aio_pid;
    extern hi_s32 sample_audio_ai_ao(hi_void);
    return pthread_create(&aio_pid, 0, (void*(*)(void*))sample_audio_ai_ao, NULL);
  }
  else
  {
    printf("sample_audio_ai_aenc start \n");
    _adec = _aenc = *aenc;
    extern hi_s32 sample_audio_ai_aenc(gsf_mpp_aenc_t *aenc);
    return sample_audio_ai_aenc(aenc);
  }
  return 0;
}

int gsf_mpp_audio_stop(gsf_mpp_aenc_t  *aenc)
{
  int ret = 0;
  if(aenc == NULL)
  {
    ;  
  }
  else
  {
    extern hi_s32 sample_audio_ai_aenc_stop(gsf_mpp_aenc_t *aenc);
    return sample_audio_ai_aenc_stop(aenc);
  }
  return ret;
}


int gsf_mpp_rgn_ctl(RGN_HANDLE Handle, int id, gsf_mpp_rgn_t *rgn)
{
  int ret = 0;
  switch(id)
  {
    case GSF_MPP_RGN_CREATE:
      ret = hi_mpi_rgn_create(Handle, &rgn->stRegion);
      break;
    case GSF_MPP_RGN_DESTROY:
      ret = hi_mpi_rgn_destroy(Handle);
      break;
    case GSF_MPP_RGN_SETATTR:
      ret = hi_mpi_rgn_set_attr(Handle, &rgn->stRegion);
      break;
    case GSF_MPP_RGN_GETATTR:
      ret = hi_mpi_rgn_get_attr(Handle, &rgn->stRegion);
      break;
    case GSF_MPP_RGN_ATTACH:
      ret = hi_mpi_rgn_attach_to_chn(Handle, &rgn->stChn, &rgn->stChnAttr);
      break;
    case GSF_MPP_RGN_DETACH:
      ret = hi_mpi_rgn_detach_from_chn(Handle, &rgn->stChn);
      break;
    case GSF_MPP_RGN_SETDISPLAY:
      ret = hi_mpi_rgn_set_chn_display_attr(Handle, &rgn->stChn, &rgn->stChnAttr);
      break;
    case GSF_MPP_RGN_GETDISPLAY:
      ret = hi_mpi_rgn_get_chn_display_attr(Handle, &rgn->stChn, &rgn->stChnAttr);
      break;
  }
  //printf("id:%d, Handle:%d, ret:0x%x\n", id, Handle, ret);  
  return ret;
}
int gsf_mpp_rgn_bitmap(RGN_HANDLE Handle, BITMAP_S *bitmap)
{
  int ret = 0;
  ret = hi_mpi_rgn_set_bmp(Handle, bitmap);
  //printf("bitmap Handle:%d, ret:0x%x\n", Handle, ret);
  return ret;
}
int gsf_mpp_rgn_canvas_get(RGN_HANDLE Handle, RGN_CANVAS_INFO_S *pstRgnCanvasInfo)
{
  int ret = 0;
  return ret;
}
int gsf_mpp_rgn_canvas_update(RGN_HANDLE Handle)
{
  int ret = 0;
  return ret;
}


int gsf_mpp_ao_send_pcm(int aodev, int ch, int flag, unsigned char *data, int size)
{
  int ret = 0;
  static hi_s16 g_ao_data[2][HI_MAX_AO_POINT_NUM] = {0}; /* 2: stereo */
  static hi_audio_frame ao_frame = {0};

  printf("gsf_mpp_ao_send_pcm size:%d\n", size);

  ao_frame.bit_width = HI_AUDIO_BIT_WIDTH_16;
  ao_frame.snd_mode = HI_AUDIO_SOUND_MODE_STEREO;

  ao_frame.virt_addr[0] = (hi_u8 *)&(g_ao_data[0][0]);
  ao_frame.virt_addr[1] = (hi_u8 *)&(g_ao_data[1][0]);
  ao_frame.len = size;

  memcpy(ao_frame.virt_addr[0], data, size); 
  memcpy(ao_frame.virt_addr[1], data, size);

  if(size > 0)
  {
    ret = hi_mpi_ao_send_frame(aodev, ch, &ao_frame, 1000); /* 1000:1000ms */
  }
  return ret;
}


int gsf_mpp_ao_asend(int aodev, int ch, int flag, char *data, gsf_mpp_frm_attr_t *attr)
{
  if(!_audio_init)
  {
    _audio_init = 1;
    extern hi_s32 sample_audio_init();
    sample_audio_init();
  }
  
  HI_S32 s32Ret = 0;
  HI_S32 adch = 0;
    
  static gsf_mpp_frm_attr_t _attr = {.etype = PT_BUTT, };
  
  if(_attr.etype != attr->etype)
  {
    //reset;
    _attr = *attr;
    extern hi_s32 sample_audio_adec_ao(gsf_mpp_frm_attr_t *attr);
    sample_audio_adec_ao(attr);
  }
  
  if(attr->size == 0)
  {
    return hi_mpi_adec_send_end_of_stream(adch, HI_FALSE);
  }
  
  hi_audio_stream stAudioStream = {0};
  stAudioStream.stream = data;
  stAudioStream.len = attr->size;
  stAudioStream.time_stamp = 0;
  stAudioStream.seq = 0;
  
  /* here only demo adec streaming sending mode, but pack sending mode is commended */
  s32Ret = hi_mpi_adec_send_stream(adch, &stAudioStream, flag);
  if (HI_SUCCESS != s32Ret)
  {
    printf("%s: HI_MPI_ADEC_SendStream(%d) failed \n", \
           __FUNCTION__, adch, stAudioStream.len);
  }
  else
  {
    //printf("%s: HI_MPI_ADEC_SendStream(%d) OK len:%d, time_stamp:%llu\n", \
    //       __FUNCTION__, adch, stAudioStream.len, stAudioStream.time_stamp);
  }

  return s32Ret;
}

//audio ao_bind_ai;
int gsf_mpp_ao_bind(int aodev, int ch, int aidev, int aich)
{
  //ao bind ai;
  hi_s32    s32Ret;
  hi_ai_chn AiChn = aich;
  hi_ao_chn AoChn = ch;
  hi_s32    s32AoChnCnt;

  hi_aio_attr stAioAttr;
  
  hi_audio_dev   AiDev = aidev;//SAMPLE_AUDIO_INNER_AI_DEV;
  hi_audio_dev   AoDev = aodev;//SAMPLE_AUDIO_INNER_AO_DEV; SAMPLE_AUDIO_EXTERN_AO_DEV;

  if(aidev < 0)
  {
    printf("AoDev:%d, AoChn:%d, reset sp:%d & stereo:%d\n", AoDev, AoChn, _adec.sp, _adec.stereo);
  }

  hi_bool bAioReSample = (AoDev == SAMPLE_AUDIO_INNER_AO_DEV)?0:
                         (_adec.sp != HI_AUDIO_SAMPLE_RATE_48000)?1:0;
  hi_audio_sample_rate enInSampleRate = _adec.sp;
  
  if(bAioReSample && AoDev != SAMPLE_AUDIO_INNER_AO_DEV)
    return 0;

  stAioAttr.sample_rate   = (AoDev == SAMPLE_AUDIO_INNER_AO_DEV)?_adec.sp:HI_AUDIO_SAMPLE_RATE_48000;
  stAioAttr.bit_width     = HI_AUDIO_BIT_WIDTH_16;
  stAioAttr.work_mode     = HI_AIO_MODE_I2S_MASTER;
  stAioAttr.snd_mode      = _adec.stereo;//HI_AUDIO_SOUND_MODE_STEREO;
  stAioAttr.expand_flag   = 0;
  stAioAttr.frame_num     = SAMPLE_AUDIO_AI_USER_FRAME_DEPTH;
  stAioAttr.point_num_per_frame = (_adec.enPayLoad==PT_AAC)?HI_AACLC_SAMPLES_PER_FRAME:SAMPLE_AUDIO_POINT_NUM_PER_FRAME;//HI_AACLC_SAMPLES_PER_FRAME;
  stAioAttr.chn_cnt       = 1<<_adec.stereo;
  stAioAttr.clk_share     = 1;//(aidev<0)?0:1;
  stAioAttr.i2s_type      = (AoDev == SAMPLE_AUDIO_INNER_AO_DEV)?HI_AIO_I2STYPE_INNERCODEC:HI_AIO_I2STYPE_EXTERN;

  s32AoChnCnt = stAioAttr.chn_cnt;
  s32Ret = sample_comm_audio_start_ao(AoDev, s32AoChnCnt, &stAioAttr, enInSampleRate, bAioReSample);
  if (s32Ret != HI_SUCCESS)
  {
    printf("sample_comm_audio_start_ao err:%d, AoDev:%d, s32AoChnCnt:%d\n", s32Ret, AoDev, s32AoChnCnt);
    return s32Ret;
  }
  
  #if 1
  int aoVol = -5;
  s32Ret = hi_mpi_ao_set_volume(AoDev, aoVol); //[-121, 6] 值越大音量越大
  if (s32Ret != HI_SUCCESS)
  {
    printf("hi_mpi_ao_set_volume err:%d, AoDev:%d, aoVol:%d\n", s32Ret, AoDev, aoVol);
  }
  #endif
  
  if(AiDev < 0)
  {
    printf("AoDev:%d, AoChn:%d, ao not bind ai\n", AoDev, AoChn);
    return 0;
  }
  
  s32Ret = sample_comm_audio_ao_bind_ai(AiDev, AiChn, AoDev, AoChn);
  if (s32Ret != HI_SUCCESS)
  {
    printf("sample_comm_audio_ao_bind_ai err:%d, AoDev:%d, AiDev:%d\n", s32Ret, AoDev, AiDev);
    goto __err;
  }

  return 0;
  
__err:
  s32Ret = sample_comm_audio_stop_ao(AoDev, s32AoChnCnt, bAioReSample);
  if (s32Ret != HI_SUCCESS)
  {
    printf("sample_comm_audio_stop_ao err:%d, AoDev:%d, s32AoChnCnt:%d\n", s32Ret, AoDev, s32AoChnCnt);
  }
  return -1;
}
