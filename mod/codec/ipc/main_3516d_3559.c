#if defined(GSF_CPU_3516d) || defined(GSF_CPU_3559)

#define _GNU_SOURCE
#include <unistd.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include "inc/gsf.h"
#include "fw/comm/inc/proc.h"
#include "fw/comm/inc/sstat.h"
#include "mod/bsp/inc/bsp.h"
#include "mod/svp/inc/svp.h"
#include "mod/rtsps/inc/rtsps.h"
#include "codec.h"
#include "cfg.h"
#include "msg_func.h"
#include "mpp.h"
#include "rgn.h"
#include "venc.h"
#include "lens.h"
#include "main_func.h"

//0: disable; 
//1: 1 PIP-main + 1 PIP-sub;
//2: 1 sensor + 1 thermal + 1 PIP;
static int avs = 0; // codec_ipc.vi.avs;

// if compile error, please add PIC_XXX to sample_comm.h:PIC_SIZE_E;
#ifndef HAVE_PIC_600P
#warning "PIC_600P => PIC_D1_PAL"
#define PIC_600P PIC_D1_PAL
#endif
#ifndef HAVE_PIC_288P
#warning "PIC_288P => PIC_CIF"
#define PIC_288P PIC_CIF
#endif
#ifndef HAVE_PIC_400P
#warning "PIC_400P => PIC_CIF"
#define PIC_400P PIC_CIF
#endif
#ifndef HAVE_PIC_512P
#warning "PIC_512P => PIC_D1_PAL"
#define PIC_512P PIC_D1_PAL
#endif
#ifndef HAVE_PIC_2048P
#warning "PIC_2448x2048 => PIC_2592x1944"
#define PIC_2448x2048 PIC_2592x1944
#endif
#ifndef HAVE_PIC_640P
#warning "PIC_640P => PIC_512P"
#define PIC_640P PIC_512P
#endif

#ifndef HAVE_PIC_1520P
#define PIC_2688x1520 PIC_2592x1520
#warning "PIC_2688x1520 => PIC_2592x1520"
#endif

#define PIC_WIDTH(w, h) \
          (w >= 7680)?PIC_7680x4320:\
          (w >= 3840)?PIC_3840x2160:\
          (w >= 2688 && h >= 1520)?PIC_2688x1520:\
          (w >= 2592 && h >= 1944)?PIC_2592x1944:\
          (w >= 2592 && h >= 1536)?PIC_2592x1536:\
          (w >= 2592 && h >= 1520)?PIC_2592x1520:\
          (w >= 2448 && h >= 2048)?PIC_2448x2048:\
          (w >= 1920)?PIC_1080P:\
          (w >= 1280)?PIC_720P: \
          (w >= 800)?PIC_600P:  \
          (w >= 720 && h >= 576)?PIC_D1_PAL: \
          (w >= 720 && h >= 480)?PIC_D1_NTSC: \
          (w >= 640 && h >= 640)?PIC_640P: \
          (w >= 640 && h >= 512)?PIC_512P: \
          (w >= 640 && h >= 360)?PIC_360P: \
          (w >= 400 && h >= 400)?PIC_400P: \
          (w >= 384 && h >= 288)?PIC_288P: \
          PIC_CIF

static gsf_resolu_t __pic_wh[PIC_BUTT] = {
      [PIC_7680x4320] = {0, 7680, 4320},
      [PIC_3840x2160] = {0, 3840, 2160},
      [PIC_2688x1520] = {0, 2688, 1520},
      [PIC_2592x1944] = {0, 2592, 1944},
      [PIC_2592x1536] = {0, 2592, 1536},
      [PIC_2592x1520] = {0, 2592, 1520},
      [PIC_2448x2048] = {0, 2448, 2048},
      [PIC_1080P]     = {0, 1920, 1080},
      [PIC_720P]      = {0, 1280, 720},
      [PIC_600P]      = {0, 800, 600},
      [PIC_D1_PAL]    = {0, 720, 576},
      [PIC_D1_NTSC]   = {0, 720, 480},
      [PIC_640P]      = {0, 640, 640},
      [PIC_512P]      = {0, 640, 512},
      [PIC_360P]      = {0, 640, 360},
      [PIC_400P]      = {0, 400, 400},
      [PIC_288P]      = {0, 384, 288},
      [PIC_CIF]       = {0, 352, 288},
};          
#define PIC_WH(e,_w,_h) do {\
  _w = __pic_wh[e].w; _h = __pic_wh[e].h;\
  }while(0)

#define PT_VENC(t) \
            (t == GSF_ENC_H264)? PT_H264:\
            (t == GSF_ENC_H265)? PT_H265:\
            (t == GSF_ENC_JPEG)? PT_JPEG:\
            (t == GSF_ENC_MJPEG)? PT_MJPEG:\
            PT_H264

//////////////////////////////////////////////////////////////////

//resolution;
static gsf_resolu_t vores;
#define vo_res_set(_w, _h) do{vores.w = _w; vores.h = _h;}while(0)
int vo_res_get(gsf_resolu_t *res) { *res = vores; return 0;}

//layout;
static gsf_layout_t voly;
int vo_ly_set(int ly) {voly.layout = ly; return 0;}
int vo_ly_get(gsf_layout_t *ly) { *ly = voly; return 0;}

static gsf_mpp_vpss_t *p_vpss = NULL;
static gsf_venc_ini_t *p_venc_ini = NULL;
static gsf_mpp_cfg_t  *p_cfg = NULL;

//second sdp hook, fixed venc cfg;
#define SECOND_WIDTH(second) ((second) == SECOND_BT656_PAL?720: \
                              (second) == SECOND_BT656_NTSC?720:\
                              (second) == SECOND_BT656_288P?384:\
                              (second) == SECOND_BT656_600P?800:\
                              640)
#define SECOND_HEIGHT(second) ((second) == SECOND_BT656_PAL?576: \
                               (second) == SECOND_BT656_NTSC?480:\
                               (second) == SECOND_BT656_288P?288:\
                               (second) == SECOND_BT656_600P?600:\
                              512)
#define SECOND_HIRES(second) ((second) == SECOND_BT656_PAL?PIC_D1_PAL:\
                              (second) == SECOND_BT656_NTSC?PIC_D1_NTSC:\
                              (second) == SECOND_BT656_288P?PIC_288P:\
                              (second) == SECOND_BT656_600P?PIC_600P:\
                              PIC_512P)

int venc_fixed_sdp(int ch, int i, gsf_sdp_t *sdp)
{
  if(ch != 1)
    return -1;
  
  if(p_cfg->second)
  {
    sdp->audio_shmid = -1;
    if(i == 0 || i == 2) // main 
    {
      sdp->venc.width = SECOND_WIDTH(p_cfg->second);
      sdp->venc.height = SECOND_HEIGHT(p_cfg->second);
      sdp->venc.bitrate = (SECOND_WIDTH(p_cfg->second)>=1920)?8000:2000;
      sdp->venc.fps = p_cfg->fps; // fps: venc = vi;
    }
    else // sub
    {
      sdp->venc.width = (SECOND_WIDTH(p_cfg->second)>=1920)?720:352;
      sdp->venc.height = (SECOND_WIDTH(p_cfg->second)>=1920)?480:288;
      sdp->venc.bitrate = (SECOND_WIDTH(p_cfg->second)>=1920)?2000:1000;
      sdp->venc.fps = p_cfg->fps; // fps: venc = vi;
    }
    return 0; 
  }
  return -1;
}

int venc_start(int start)
{
  int ret = 0, i = 0, j = 0, k = 0;
  
  gsf_mpp_recv_t st = {
    .s32Cnt = 0,  
    .VeChn = {0,},
    .uargs = NULL,
    .cb = gsf_venc_recv,
  };
  
  if(!start)
  {
    printf("stop >>> gsf_mpp_venc_dest()\n");
    gsf_mpp_venc_dest();
  }

  for(i = 0; i < p_venc_ini->ch_num; i++)
  for(j = 0; j < GSF_CODEC_VENC_NUM; j++)
  {
    if(!codec_ipc.venc[j].en)
      continue;

    gsf_mpp_venc_t venc = {
      .VencChn    = i*GSF_CODEC_VENC_NUM+j,
      .srcModId   = HI_ID_VPSS,
      .VpssGrp    = i,
      .VpssChn    = (p_vpss[i].enable[j])?j:0,
      .enPayLoad  = PT_VENC(codec_ipc.venc[j].type),
      .enSize     = PIC_WIDTH(codec_ipc.venc[j].width, codec_ipc.venc[j].height),
      .enRcMode   = codec_ipc.venc[j].rcmode,
      .u32Profile = codec_ipc.venc[j].profile,
      .bRcnRefShareBuf = HI_TRUE,
      .enGopMode  = VENC_GOPMODE_NORMALP,
      .u32FrameRate = codec_ipc.venc[j].fps,
      .u32Gop       = codec_ipc.venc[j].gop,
      .u32BitRate   = codec_ipc.venc[j].bitrate,
      .u32LowDelay   = codec_ipc.venc[j].lowdelay,
    };

    if(strcasestr(p_cfg->type, "3516a"))
    {
      if(strstr(p_cfg->snsname, "imx378")/*||strstr(p_cfg->snsname, "imx585")*/)
      {
        venc.VpssChn = 0;
      }
    }

    if(avs == 1)
    {  
      venc.srcModId   = HI_ID_VO;
      venc.VpssGrp    = -1;
      venc.VpssChn    = -1;
    }
    
    if(p_cfg->second && i == 1)
    {
      gsf_sdp_t sdp;
      if(venc_fixed_sdp(i, j, &sdp) == 0)
      {
        venc.enSize = PIC_WIDTH(sdp.venc.width, sdp.venc.height);
        venc.u32BitRate = sdp.venc.bitrate;
        venc.u32FrameRate = sdp.venc.fps;
      }
    }
    
    //third-channel;
    if(avs == 2 && i == 2)
    {
      venc.srcModId   = HI_ID_VO;
      venc.VpssGrp    = -1;
      venc.VpssChn    = -1;
      venc.enSize     = PIC_1080P;
      venc.u32BitRate = 4000;
      venc.u32FrameRate = 30;
    }

    int w = 0, h = 0;
    PIC_WH(venc.enSize, w, h);
      
    if(!start)
    {
      ret = gsf_mpp_venc_stop(&venc);
      printf("stop >>> ch:%d, st:%d, enSize:%d[%dx%d], ret:%d\n"
            , i, j, venc.enSize, w, h, ret);
    }
    else
    {
      ret = gsf_mpp_venc_start(&venc);
      printf("start >>> ch:%d, st:%d, enSize:%d[%dx%d], ret:%d\n"
            , i, j, venc.enSize, w, h, ret);
    }
    
    if(!start)
      continue;
    
    if(codec_ipc.venc[j].type != GSF_ENC_JPEG)
    {
      st.VeChn[st.s32Cnt] = venc.VencChn;
      st.s32Cnt++;
    }
    
    if(0 == j)//for each channel;
    {
      // refresh rgn for st_num+1(JPEG);
      for(k = 0; k < GSF_CODEC_OSD_NUM; k++)
      {
        gsf_rgn_osd_set(i, k, &codec_ipc.osd[k]);
        if(codec_ipc.osd[k].type == 1)
          gsf_venc_set_osd_time_idx(i, (codec_ipc.osd[k].en)?k:-1, codec_ipc.osd[k].point[0], codec_ipc.osd[k].point[1]);
      }  
      for(k = 0; k < GSF_CODEC_VMASK_NUM; k++)
      {
        gsf_rgn_vmask_set(i, k, &codec_ipc.vmask[k]);
      }
    }  
  }
  
  if(!start)
    return 0;
    
  // recv start;
  printf("start >>> gsf_mpp_venc_recv s32Cnt:%d\n", st.s32Cnt);
  gsf_mpp_venc_recv(&st);
  return 0;
}

#define VPSS_BIND_VI(_i, _vip, _vich, _vpg, _en0, _en1, _sz0, _sz1) do{ \
      vpss[_i].VpssGrp=_vpg; vpss[_i].ViPipe=_vip; vpss[_i].ViChn=_vich;\
      vpss[_i].enable[0]=_en0;vpss[_i].enable[1]=_en1;\
      vpss[_i].enSize[0]=_sz0;vpss[_i].enSize[1]=_sz1;}while(0)
      

#if defined(GSF_CPU_3559)
void mpp_ini_3559(gsf_mpp_cfg_t *cfg, gsf_rgn_ini_t *rgn_ini, gsf_venc_ini_t *venc_ini, gsf_mpp_vpss_t *vpss)
{
  #if 0
  // yuv422-0-0-2-60
  cfg->lane = 0; cfg->wdr = 0; cfg->res = 2; cfg->fps = 60;
  rgn_ini->ch_num = 1; rgn_ini->st_num = 2;
  venc_ini->ch_num = 1; venc_ini->st_num = 2;
  VPSS_BIND_VI(0, 0, 0, 0, 1, 1, PIC_1080P, PIC_D1_NTSC);
  return;
  #endif
  
  if(strstr(cfg->snsname, "imx335"))
  {
    // imx335-0-0-4-30
    cfg->lane = 0; cfg->wdr = 0; cfg->res = 4; cfg->fps = 30;
    rgn_ini->ch_num = 1; rgn_ini->st_num = 2;
    venc_ini->ch_num = 1; venc_ini->st_num = 2;
    VPSS_BIND_VI(0, 0, 0, 0, 1, 1, PIC_2592x1536, PIC_1080P);
    return;
  }
  else
  {
    // imx415-0-0-8-30
    cfg->lane = 0; cfg->wdr = 0; cfg->res = 8; cfg->fps = 30;
    rgn_ini->ch_num = 1; rgn_ini->st_num = 2;
    venc_ini->ch_num = 1; venc_ini->st_num = 2;
    VPSS_BIND_VI(0, 0, 0, 0, 1, 1, PIC_3840x2160, PIC_1080P);
    return;
  }
}
#endif
#if defined(GSF_CPU_3516d)
void mpp_ini_3516d(gsf_mpp_cfg_t *cfg, gsf_rgn_ini_t *rgn_ini, gsf_venc_ini_t *venc_ini, gsf_mpp_vpss_t *vpss)
{
  if (avs == 1)
  {
    // imx335-0-0-4-30
    if(strstr(cfg->snsname, "imx327") 
      || strstr(cfg->snsname, "imx290") || strstr(cfg->snsname, "imx307") || strstr(cfg->snsname, "imx385") || strstr(cfg->snsname, "imx482"))
    {
      cfg->lane = (cfg->second)?0:2; cfg->wdr = 0; cfg->res = 2; cfg->fps = (codec_ipc.vi.fps>0)?codec_ipc.vi.fps:30;
      rgn_ini->ch_num = 1; rgn_ini->st_num = 2;
      venc_ini->ch_num = 1; venc_ini->st_num = 2;
      VPSS_BIND_VI(0, 0, 0, 0, 1, 1, PIC_1080P, PIC_720P);
      
      VPSS_BIND_VI(1, 1, 0, 1, 1, 1, PIC_1080P, PIC_720P);
      if(cfg->second)
	      VPSS_BIND_VI(1, 1, 0, 1, 1, 1, SECOND_HIRES(cfg->second), PIC_CIF);
	      
    }
    else if(strstr(cfg->snsname, "imx415") || strstr(cfg->snsname, "imx334") || strstr(cfg->snsname, "imx378") || strstr(cfg->snsname, "imx585") || strstr(cfg->snsname, "imx678") )
    {
        if(strcasestr(cfg->type, "3516a"))
        {          
          cfg->lane = 0; cfg->wdr = 0; cfg->res = (codec_ipc.vi.res==2)?2:8; cfg->fps = (codec_ipc.vi.fps>0)?codec_ipc.vi.fps:30;
          rgn_ini->ch_num = 1; rgn_ini->st_num = 2;
          venc_ini->ch_num = 1; venc_ini->st_num = 2;
          VPSS_BIND_VI(0, 0, 0, 0, 1, 1-1, (codec_ipc.vi.res==2)?PIC_1080P:PIC_3840x2160, PIC_1080P);
        }
        else
        {
          cfg->lane = 0; cfg->wdr = 0; cfg->res = 2; cfg->fps = strstr(cfg->snsname, "imx415")?60:30;
          rgn_ini->ch_num = 1; rgn_ini->st_num = 2;
          venc_ini->ch_num = 1; venc_ini->st_num = 2;
          VPSS_BIND_VI(0, 0, 0, 0, 1, 1-1, PIC_1080P, PIC_D1_NTSC);
        }
        if(cfg->second)
      	  VPSS_BIND_VI(1, 1, 0, 1, 1, 1-1, SECOND_HIRES(cfg->second), PIC_CIF);
    }
    else if(strstr(cfg->snsname, "imx335"))
    {
      cfg->lane = (cfg->second)?0:2; cfg->wdr = 0; cfg->res = 4; cfg->fps = 30;
      rgn_ini->ch_num = 1; rgn_ini->st_num = 2;
      venc_ini->ch_num = 1; venc_ini->st_num = 2;
      VPSS_BIND_VI(0, 0, 0, 0, 1, 1, PIC_2592x1536, PIC_1080P);
      if(cfg->second)
      	VPSS_BIND_VI(1, 1, 0, 1, 1, 1, SECOND_HIRES(cfg->second), PIC_CIF);
    }
    return;
  }
  
  if(cfg->snscnt < 2)
  {
    if(strstr(cfg->snsname, "imx327") 
      || strstr(cfg->snsname, "imx290") || strstr(cfg->snsname, "imx307") || strstr(cfg->snsname, "imx385") || strstr(cfg->snsname, "imx482"))
    {
    	// imx327-0-0-2-30
    	#if 1
      cfg->lane = 0; cfg->wdr = 0; cfg->res = 2; cfg->fps = (codec_ipc.vi.fps>0)?codec_ipc.vi.fps:30;
      rgn_ini->ch_num = 1; rgn_ini->st_num = 2;
      venc_ini->ch_num = 1; venc_ini->st_num = 2;
      VPSS_BIND_VI(0, 0, 0, 0, 1, 1, PIC_1080P, PIC_720P);
      #else
      cfg->lane = 0; cfg->wdr = 0; cfg->res = 2; cfg->fps = 120;
      rgn_ini->ch_num = 1; rgn_ini->st_num = 2;
      venc_ini->ch_num = 1; venc_ini->st_num = 2;
      VPSS_BIND_VI(0, 0, 0, 0, 1, 1, PIC_1080P, PIC_720P);
      #endif
      if(cfg->second)
      {
        rgn_ini->ch_num = venc_ini->ch_num = 2;
        VPSS_BIND_VI(1, 1, 0, 1, 1, 1, SECOND_HIRES(cfg->second), PIC_CIF);
      }
    }
    else if(strstr(cfg->snsname, "imx415") || strstr(cfg->snsname, "imx334") || strstr(cfg->snsname, "imx378") 
      || strstr(cfg->snsname, "imx585") || strstr(cfg->snsname, "imx678"))
    {
        if(strcasestr(cfg->type, "3516a"))
        {
          cfg->lane = 0; cfg->wdr = 0; cfg->res = (codec_ipc.vi.res==2)?2:8; cfg->fps = (codec_ipc.vi.fps>0)?codec_ipc.vi.fps:30;
          rgn_ini->ch_num = 1; rgn_ini->st_num = (avs==2)?1:2;
          venc_ini->ch_num = 1; venc_ini->st_num = (avs==2)?1:2;
          VPSS_BIND_VI(0, 0, 0, 0, 1, 1, (codec_ipc.vi.res==2)?PIC_1080P:PIC_3840x2160, PIC_1080P);
            
          if(strstr(cfg->snsname, "imx415") && codec_ipc.vi.wdr)
          {
            cfg->wdr = codec_ipc.vi.wdr;
            cfg->fps = 20;
          }
          
          if(strstr(cfg->snsname, "imx585"))
          {
            VPSS_BIND_VI(0, 0, 0, 0, 1, 1, (codec_ipc.vi.res==2)?PIC_1080P:PIC_3840x2160, PIC_720P);
          }  

        }
        else
        {
          cfg->lane = 0; cfg->wdr = 0; cfg->res = 2; cfg->fps = strstr(cfg->snsname, "imx415")?60:30;
          rgn_ini->ch_num = 1; rgn_ini->st_num = (avs==2)?1:2;
          venc_ini->ch_num = 1; venc_ini->st_num = (avs==2)?1:2;
          VPSS_BIND_VI(0, 0, 0, 0, 1, 1, PIC_1080P, PIC_D1_NTSC);
        }
			  if(cfg->second)
			  {
			    rgn_ini->ch_num = venc_ini->ch_num = 2;
				  VPSS_BIND_VI(1, 1, 0, 1, 1, 1, SECOND_HIRES(cfg->second), PIC_CIF);
			  }
    }
    else if(strstr(cfg->snsname, "imx335"))
    {
      // imx335-0-0-4-30
      cfg->lane = 0; cfg->wdr = codec_ipc.vi.wdr; cfg->res = 4; cfg->fps = 30;
      rgn_ini->ch_num = 1; rgn_ini->st_num = 2;
      venc_ini->ch_num = 1; venc_ini->st_num = 2;
      VPSS_BIND_VI(0, 0, 0, 0, 1, 1, PIC_2592x1536, PIC_1080P);
      if(cfg->second)
      {
        rgn_ini->ch_num = venc_ini->ch_num = 2;
        VPSS_BIND_VI(1, 1, 0, 1, 1, 1, SECOND_HIRES(cfg->second), PIC_CIF);
      }
    }
    else if(strstr(cfg->snsname, "imx568"))
    {
      // imx335-0-0-4-30
      cfg->lane = 0; cfg->wdr = codec_ipc.vi.wdr; cfg->res = 5; cfg->fps = 30;
      rgn_ini->ch_num = 1; rgn_ini->st_num = 2;
      venc_ini->ch_num = 1; venc_ini->st_num = 2;
      VPSS_BIND_VI(0, 0, 0, 0, 1, 1, PIC_2448x2048, PIC_1080P);
      if(cfg->second)
      {
        rgn_ini->ch_num = venc_ini->ch_num = 2;
        VPSS_BIND_VI(1, 1, 0, 1, 1, 1, SECOND_HIRES(cfg->second), PIC_CIF);
      }
    }
    else if(strstr(cfg->snsname, "imx664"))
    {
      // imx335-0-0-4-30
      cfg->lane = 0; cfg->wdr = codec_ipc.vi.wdr; cfg->res = 4; cfg->fps = 60;
      rgn_ini->ch_num = 1; rgn_ini->st_num = 2;
      venc_ini->ch_num = 1; venc_ini->st_num = 2;
      VPSS_BIND_VI(0, 0, 0, 0, 1, 1, PIC_2688x1520, PIC_720P);
      if(cfg->second)
      {
        rgn_ini->ch_num = venc_ini->ch_num = 2;
        VPSS_BIND_VI(1, 1, 0, 1, 1, 1, SECOND_HIRES(cfg->second), PIC_CIF);
      }
    }    
    else if(strstr(cfg->snsname, "ov426"))
    {
      cfg->lane = 0; cfg->wdr = 0; cfg->res = 0; cfg->fps = 30;
      rgn_ini->ch_num = 1; rgn_ini->st_num = 1;
      venc_ini->ch_num = 1; venc_ini->st_num = 1;
      VPSS_BIND_VI(0, 0, 0, 0, 1, 0, PIC_400P, PIC_400P);
    }
    else if(strstr(cfg->snsname, "yuv422"))
    {
      if(codec_ipc.vi.res == 8)
      {
        // yuv422-0-0-8-30
        cfg->lane = 0; cfg->wdr = 0; cfg->res = 8; cfg->fps = 30;
        rgn_ini->ch_num = 1; rgn_ini->st_num = 2;
        venc_ini->ch_num = 1; venc_ini->st_num = 2;
        VPSS_BIND_VI(0, 0, 0, 0, 1, 1, PIC_3840x2160, PIC_1080P);

      }
      else if (codec_ipc.vi.res == 2)
      {
        // yuv422-0-0-2-60
        cfg->lane = 0; cfg->wdr = 0; cfg->res = 2; cfg->fps = 60;
        rgn_ini->ch_num = 1; rgn_ini->st_num = 2;
        venc_ini->ch_num = 1; venc_ini->st_num = 2;
        VPSS_BIND_VI(0, 0, 0, 0, 1, 1, PIC_1080P, PIC_D1_NTSC);
      }
      else if (codec_ipc.vi.res == 1)
      {
        // yuv422-0-0-1-60
        cfg->lane = 0; cfg->wdr = 0; cfg->res = 1; cfg->fps = 60;
        rgn_ini->ch_num = 1; rgn_ini->st_num = 2;
        venc_ini->ch_num = 1; venc_ini->st_num = 2;
        VPSS_BIND_VI(0, 0, 0, 0, 1, 1, PIC_720P, PIC_D1_NTSC);
      }
      else  // other;
      {
        // yuv422-0-0-2-60
        cfg->lane = 0; cfg->wdr = 0; cfg->res = 2; cfg->fps = 60;
        rgn_ini->ch_num = 1; rgn_ini->st_num = 2;
        venc_ini->ch_num = 1; venc_ini->st_num = 2;
        VPSS_BIND_VI(0, 0, 0, 0, 1, 1, PIC_1080P, PIC_D1_NTSC);
      }
    }
    else
    {
      if (codec_ipc.vi.res == 2)
      {
        cfg->lane = 0; cfg->wdr = 0; cfg->res = 2; cfg->fps = 60;
        rgn_ini->ch_num = 1; rgn_ini->st_num = 2;
        venc_ini->ch_num = 1; venc_ini->st_num = 2;
        VPSS_BIND_VI(0, 0, 0, 0, 1, 1, PIC_1080P, PIC_D1_NTSC);
      }   
      else if (codec_ipc.vi.res == 1)
      {
        cfg->lane = 0; cfg->wdr = 0; cfg->res = 1; cfg->fps = 60;
        rgn_ini->ch_num = 1; rgn_ini->st_num = 2;
        venc_ini->ch_num = 1; venc_ini->st_num = 2;
        VPSS_BIND_VI(0, 0, 0, 0, 1, 1, PIC_720P, PIC_D1_NTSC);
      }  
      else 
      {
        cfg->lane = 0; cfg->wdr = 0; cfg->res = 2; cfg->fps = 60;
        rgn_ini->ch_num = 1; rgn_ini->st_num = 2;
        venc_ini->ch_num = 1; venc_ini->st_num = 2;
        VPSS_BIND_VI(0, 0, 0, 0, 1, 1, PIC_1080P, PIC_D1_NTSC);
      }
    }
  }
  else
  {
   
    if(strstr(cfg->snsname, "imx335"))
    {
      // imx335-2-0-5-30
      cfg->lane = 2; cfg->wdr = 0; cfg->res = 5; cfg->fps = 30;
      rgn_ini->ch_num = 2; rgn_ini->st_num = 2;
      venc_ini->ch_num = 2; venc_ini->st_num = 2;
      VPSS_BIND_VI(0, 0, 0, 0, 1, 1, PIC_2592x1944, PIC_D1_NTSC);
      VPSS_BIND_VI(1, 1, 0, 1, 1, 1, PIC_2592x1944, PIC_D1_NTSC);
    }
    else  
    {
      // imx327-2-0-2-30
      cfg->lane = 2; cfg->wdr = 0; cfg->res = 2; cfg->fps = 30;
      rgn_ini->ch_num = 2; rgn_ini->st_num = 2;
      venc_ini->ch_num = 2; venc_ini->st_num = 2;
      VPSS_BIND_VI(0, 0, 0, 0, 1, 1, PIC_1080P, PIC_D1_NTSC);
      VPSS_BIND_VI(1, 1, 0, 1, 1, 1, PIC_1080P, PIC_D1_NTSC);
    }
  }
}
#endif

int mpp_start(gsf_bsp_def_t *def)
{
    // init mpp;
    // -------------------  vi => vpss => venc  -------------------- //
    // channel: CH0        CH1        CH2         CH3         CH4    //
    // vi:      pipe0      pipe1      pipe2       pipe3       ...    //
    // vpss:    G0[0,1]    G1[0,1]    G2[0,1]     G3[0,1]     ...    //
    // venc:    V0 V1 S2   V3 V4 S5   V6 V7 S8    V9 V10 S11  ...    //
    // ------------------------------------------------------------- //
    int i = 0;
    static gsf_mpp_cfg_t cfg;
    static gsf_rgn_ini_t rgn_ini;
    static gsf_venc_ini_t venc_ini;
    static gsf_lens_ini_t lens_ini;
    static gsf_mpp_vpss_t vpss[GSF_CODEC_IPC_CHN];
    
    //only used sensor[0];
    strcpy(cfg.snsname, def->board.sensor[0]);
    cfg.snscnt = def->board.snscnt;
    
    avs = codec_ipc.vi.avs;

    do{
      #if defined(GSF_CPU_3516d)
      {
        // cpu type try sstat_chipid(), otherwise use board.type;
        char *chipid = sstat_chipid(); // [3516a300 3516d300 35590200]
        printf("chipid[%s]\n", chipid);
        strcpy(cfg.type, def->board.type);
        chipid = !strlen(chipid)?NULL:strcpy(cfg.type, chipid);
        //second channel from bsp_def.json; mpp.h:SECOND_TYPE;
        cfg.second = (cfg.snscnt > 1)?0:
                     (def->board.second <= 0)?0:def->board.second;
        printf("BOARD [type:%s, second:%d]\n", cfg.type, cfg.second);
        
        mpp_ini_3516d(&cfg, &rgn_ini, &venc_ini, vpss);
      }
      #elif defined(GSF_CPU_3559)
      {
        mpp_ini_3559(&cfg, &rgn_ini, &venc_ini, vpss);
      }
      #else
      {
        #error "error unknow gsf_mpp_cfg_t."
      }
      #endif
    }while(0);
    
    p_venc_ini = &venc_ini;
    p_vpss     = vpss;
    p_cfg      = &cfg;
    
    char home_path[256] = {0};
    proc_absolute_path(home_path);
    sprintf(home_path, "%s/../", home_path);
    printf("home_path:[%s]\n", home_path);

    gsf_mpp_cfg(home_path, &cfg);
    
    lens_ini.ch_num = 1;  // lens number;
    strncpy(lens_ini.sns, cfg.snsname, sizeof(lens_ini.sns)-1);
    strncpy(lens_ini.lens, "LENS-NAME", sizeof(lens_ini.lens)-1);
    gsf_lens_init(&lens_ini);

    // vi start;
    printf("vi.lowdelay:%d\n", codec_ipc.vi.lowdelay);
    gsf_mpp_vi_t vi = {
        .bLowDelay = codec_ipc.vi.lowdelay,//HI_TRUE,//HI_FALSE,
        .u32SupplementConfig = 0,
    };
    
    gsf_mpp_vi_start(&vi);
    
    {
      //ttyAMA2: Single channel baseboard, ttyAMA4: double channel baseboard;
      char uart_name[32] = {0};
	    sprintf(uart_name, "/dev/%s", codec_ipc.lenscfg.uart);
	    gsf_lens_start(0, uart_name);
  	}

    // vpss start;
    if(avs == 1)
    {
      for(i = 0; i < 2; i++)
      {
        gsf_mpp_vpss_start(&vpss[i]);
      }
    }
    else 
    {
      for(i = 0; i < venc_ini.ch_num; i++)
      {
        gsf_mpp_vpss_start(&vpss[i]);
      }
    }
    
    // scene start;
    char scene_ini[128] = {0};
    proc_absolute_path(scene_ini);

    sprintf(scene_ini, "%s/../cfg/%s.ini", scene_ini, cfg.snsname);
   
    if(gsf_mpp_scene_start(scene_ini, codec_ipc.vi.wdr) <= 0)  //enable img;
    {
      #define GET_IMG(ch) \
        gsf_img_all_t *_img = (ch == 0)?&codec_ipc.img:\
        (ch == 1)?&codec_ipc.img1:\
        (ch == 2)?&codec_ipc.img2:&codec_ipc.img3;
      
      for(i = 0; i < p_cfg->snscnt; i++)
      {
        GET_IMG(i);
        printf("setimg i:%d, magic:0x%x to isp.\n", i, _img->magic);
        
        if(_img->magic == 0x55aa)
        {
          gsf_mpp_isp_ctl(i, GSF_MPP_ISP_CTL_CSC, &_img->csc);
          gsf_mpp_isp_ctl(i, GSF_MPP_ISP_CTL_AE, &_img->ae);
          gsf_mpp_isp_ctl(i, GSF_MPP_ISP_CTL_DEHAZE, &_img->dehaze);
          gsf_mpp_isp_ctl(i, GSF_MPP_ISP_CTL_SHARPEN, &_img->sharpen);
          gsf_mpp_isp_ctl(i, GSF_MPP_ISP_CTL_HLC, &_img->hlc);
          gsf_mpp_isp_ctl(i, GSF_MPP_ISP_CTL_GAMMA, &_img->gamma);
          gsf_mpp_isp_ctl(i, GSF_MPP_ISP_CTL_DRC, &_img->drc);
          gsf_mpp_isp_ctl(i, GSF_MPP_ISP_CTL_LDCI, &_img->ldci);
          gsf_mpp_isp_ctl(i, GSF_MPP_ISP_CTL_3DNR, &_img->_3dnr);
          gsf_mpp_isp_ctl(i, GSF_MPP_ISP_CTL_LDC, &_img->ldc);
        }
        else 
        {
          _img->magic = 0x55aa;
          gsf_mpp_isp_ctl(i, GSF_MPP_ISP_CTL_IMG, _img);
        }
        
        printf("setscene i:%d, magic:0x%x to isp.\n", i, codec_ipc.scene[i].magic);
        if(codec_ipc.scene[i].magic == 0x55aa)
        {
          gsf_mpp_scene_ctl(i, GSF_MPP_SCENE_CTL_AE, &codec_ipc.scene[i].ae);
        }
        else 
        {
          codec_ipc.scene[i].magic = 0x55aa;
          gsf_mpp_scene_ctl(i, GSF_MPP_SCENE_CTL_ALL, &codec_ipc.scene[i]);
        }
      }
    }

    //third-channel;
    if(avs == 2)
    {
      rgn_ini.ch_num++;
      venc_ini.ch_num++;
    }
    
    //internal-init rgn, venc;
    gsf_rgn_init(&rgn_ini);
    gsf_venc_init(&venc_ini);
    
    return 0;
}


#define AU_PT(gsf) \
        (gsf == GSF_ENC_AAC)?PT_AAC:\
        (gsf == GSF_ENC_G711A)?PT_G711A:\
        PT_G711U

int vo_start()
{
    //aenc;
    if( codec_ipc.aenc.en)
    {
      gsf_mpp_aenc_t aenc = {
        .AeChn = 0,
        .enPayLoad = AU_PT(codec_ipc.aenc.type),
        .adtype = 0, // 0:INNER, 1:I2S, 2:PCM;
        .stereo = (codec_ipc.aenc.type == GSF_ENC_AAC)?1:0, //codec_ipc.aenc.stereo
        .sp = codec_ipc.aenc.sprate*1000, //sampleRate
        .br = 0,    //bitRate;
        .uargs = NULL,
        .cb = gsf_aenc_recv,
      };
      
      gsf_mpp_audio_start(&aenc);
    }

    if(codec_ipc.vo.intf < 0)
    {
      printf("vo.intf: %d\n", codec_ipc.vo.intf);
      return 0;
    }
    
    // start vo;
    //int mipi_800x1280 = (access("/app/mipi", 0)!= -1)?1:0;
    //---- 800x640 -----//
    //---- 800x640 -----//
    int mipi_800x1280 = codec_ipc.vo.intf;
    if(mipi_800x1280)
    {
      gsf_mpp_vo_start(VODEV_HD0, VO_INTF_MIPI, VO_OUTPUT_USER, 0);
      gsf_mpp_fb_start(VOFB_GUI, VO_OUTPUT_USER, 0);
      
      gsf_mpp_vo_layout(VOLAYER_HD0, VO_LAYOUT_2MUX, NULL);
      vo_ly_set(VO_LAYOUT_2MUX);
      
      gsf_mpp_vo_src_t src0 = {0, 0};
      gsf_mpp_vo_bind(VOLAYER_HD0, 0, &src0);
      gsf_mpp_vo_src_t src1 = {1, 0};
      gsf_mpp_vo_bind(VOLAYER_HD0, 1, &src1);
      
      //HI_MPI_VO_SetChnRotation(VOLAYER_HD0, 0, ROTATION_90);
      //HI_MPI_VO_SetChnRotation(VOLAYER_HD0, 1, ROTATION_90);
      vo_res_set(800, 1280);
    }
    else
    {
      int inf = VO_INTF_HDMI;
      int sync = (codec_ipc.vo.sync == 3)?VO_OUTPUT_7680x4320_30:
            (codec_ipc.vo.sync == 2)?VO_OUTPUT_3840x2160_60:
            (codec_ipc.vo.sync == 1)?VO_OUTPUT_3840x2160_30:
            VO_OUTPUT_1080P60;
      printf("codec_ipc.vo.sync:%d\n", codec_ipc.vo.sync);
      gsf_mpp_vo_start(VODEV_HD0, inf, sync, 0);
      
      int ly = VO_LAYOUT_1MUX;
 
      ly = (p_cfg->second || avs == 1)?VO_LAYOUT_2MUX:p_cfg->snscnt;
      
      gsf_mpp_vo_layout(VOLAYER_HD0, ly, NULL);
      vo_ly_set(ly);
      
      gsf_mpp_fb_start(VOFB_GUI, sync, 0);
      
      //win0;
      gsf_mpp_vo_src_t src0 = {0, 0};
      gsf_mpp_vo_bind(VOLAYER_HD0, 0, &src0);
      
      //ov426 aspect;
      if(strstr(p_cfg->snsname, "ov426"))
      {
        RECT_S rect = {(1920-800)/2, (1080-800)/2, 800, 800};
        gsf_mpp_vo_aspect(VOLAYER_HD0, 0, &rect);
      }
      
      if(ly == VO_LAYOUT_2MUX)
      {
        //win1;
        if(p_cfg->second)
    	  {
    	    gsf_sdp_t sdp;
          RECT_S rect = {20, 0, 720, 576};
          if(venc_fixed_sdp(1, 0, &sdp) == 0)
          {
            rect_w = sdp.venc.width;
            rect_h = sdp.venc.height;
          }
          gsf_mpp_vo_aspect(VOLAYER_HD0, 1, &rect);
        }
        gsf_mpp_vo_src_t src1 = {1, 0};
        gsf_mpp_vo_bind(VOLAYER_HD0, 1, &src1);
      }
      else if(ly > VO_LAYOUT_2MUX)
      for(int i = 1; i < ly; i++)
      {
        //win1 win2 win3;
        gsf_mpp_vo_src_t src1 = {i, 0};
        gsf_mpp_vo_bind(VOLAYER_HD0, i, &src1);
      }
      
      if(sync == VO_OUTPUT_1080P60)
        vo_res_set(1920, 1080);
      else
        vo_res_set(3840, 2160);
    }
    return 0;
}
//test bt1120 user_pic;
//#define __USER_PIC__ 

int main_start(gsf_bsp_def_t *bsp_def)
{
    mpp_start(bsp_def);

    #ifdef __USER_PIC__
    if(strstr(p_cfg->snsname, "bt1120"))
    {
      VI_PIPE ViPipe = 2;
      VI_PIPE_ATTR_S stPipeAttr;
      if(HI_MPI_VI_GetPipeAttr(ViPipe, &stPipeAttr) == 0)
      {
        stPipeAttr.enPixFmt = PIXEL_FORMAT_YVU_SEMIPLANAR_420;
        HI_MPI_VI_SetPipeAttr(ViPipe, &stPipeAttr);
      }  
    }
    #endif
    
    venc_start(1);

    vo_start();

    #ifdef __EYEM__
    extern int eyem_start();
    eyem_start(); 
    #endif
    
    #ifdef __GUIDE__
    extern int guide_start();
    if(p_cfg->second == 5)
    {
      guide_start();
    }
    #endif
    
    if(1) //test file=>vdec=>vo;
    {
      extern int file_vdec(void);
      file_vdec();
    }
    
    extern int vdec_start();
    if(vdec_start() < 0)
    {
      gsf_mpp_ao_bind(SAMPLE_AUDIO_INNER_AO_DEV, 0, SAMPLE_AUDIO_INNER_AI_DEV, 0);
      gsf_mpp_ao_bind(SAMPLE_AUDIO_INNER_HDMI_AO_DEV, 0, SAMPLE_AUDIO_INNER_AI_DEV, 0);
    }

    //flip&mirror;
    gsf_mpp_img_flip_t flip;    
    flip.bFlip = codec_ipc.vi.flip;
    flip.bMirror = codec_ipc.vi.flip;
    gsf_mpp_isp_ctl(0, GSF_MPP_ISP_CTL_FLIP, &flip);
    
    return 0;
}


int dzoom_plus = 0;
int main_loop(void)
{
    usleep(10*1000);
    
    //SECOND-UVC
    if(p_cfg->second == 6)
    {
      static int find_uvc = 0;
      if(!find_uvc)
      {
        for(int i = 0; i < 10; i++)
        {
          if(access("/dev/video0", 0) == 0)
          {
            find_uvc = 1;
            printf("@@@ find_uvc ok @@@\n");
            sleep(3);
            break;
          }
          sleep(1);
        }
      }

      if(find_uvc)
      {
        VIDEO_FRAME_INFO_S stFrameInfo;
        
        static float cnt = 0;
        static struct timespec ts1, ts2;  
        clock_gettime(CLOCK_MONOTONIC, &ts2);
        
        if(gsf_mpp_uvc_get(0, 0, &stFrameInfo, -1) == 0)
        {
          cnt++;
          
          if(1)
          {
            struct timespec ts1, ts2;  
            clock_gettime(CLOCK_MONOTONIC, &ts1);

            gsf_mpp_vpss_send(1, 0, &stFrameInfo, 0);
            gsf_mpp_uvc_release(0, 0, &stFrameInfo);
            
            clock_gettime(CLOCK_MONOTONIC, &ts2);
            int cost = (ts2.tv_sec*1000 + ts2.tv_nsec/1000000) - (ts1.tv_sec*1000 + ts1.tv_nsec/1000000);
            if(cost > 20)
            {
              printf("gsf_mpp_vpss_send cost:%d ms\n", cost);
            }
          }  
        }
        
        float cost = (ts2.tv_sec*1000 + ts2.tv_nsec/1000000) - (ts1.tv_sec*1000 + ts1.tv_nsec/1000000);
        if(cost >= 1000)
        {
          float fps = 1000.0/(cost/cnt);
          printf("%s uvc_fps: %0.2f\n", (fps<20)?"@@@":"", fps);
          ts1 = ts2;
          cnt = 0;
        }
        
      }
    }
    

    #ifdef __USER_PIC__
    {
    static VI_PIPE ViPipe = -1;
    static HI_S32 s32Ret = 0;
    static SAMPLE_VI_FRAME_INFO_S stViFrameInfo;
    static VI_USERPIC_ATTR_S stUsrPic;
    
    if(ViPipe == -1)
    {
      ViPipe = 2;
      stUsrPic.enUsrPicMode = VI_USERPIC_MODE_PIC;
      stUsrPic.unUsrPic.stUsrPicFrm.stVFrame.u32Width = 1920;
      stUsrPic.unUsrPic.stUsrPicFrm.stVFrame.u32Height = 1080;
      stUsrPic.unUsrPic.stUsrPicFrm.stVFrame.enPixelFormat = PIXEL_FORMAT_YVU_SEMIPLANAR_420;

      s32Ret = SAMPLE_COMM_VI_Load_UserPic("/app/cfg/UsePic_1920_1080_420.yuv", &stUsrPic, &stViFrameInfo);
      printf("SAMPLE_COMM_VI_Load_UserPic with %#x!\n", s32Ret);

      //stUsrPic.enUsrPicMode = VI_USERPIC_MODE_BGC;
      //stUsrPic.unUsrPic.stUsrPicBg.u32BgColor = 0xff22cc;
      assert(stUsrPic.unUsrPic.stUsrPicFrm.stVFrame.enPixelFormat == PIXEL_FORMAT_YVU_SEMIPLANAR_420);
      assert(stViFrameInfo.stVideoFrameInfo.stVFrame.enPixelFormat == PIXEL_FORMAT_YVU_SEMIPLANAR_420);
      assert(stViFrameInfo.stVideoFrameInfo.stVFrame.enCompressMode == COMPRESS_MODE_NONE);
      s32Ret = HI_MPI_VI_SetUserPic(ViPipe, &stUsrPic);
      printf("HI_MPI_VI_SetUserPic with %#x!\n", s32Ret);

    }

    s32Ret = HI_MPI_VI_EnableUserPic(ViPipe);
    printf("HI_MPI_VI_EnableUserPic with %#x!\n", s32Ret);
    sleep(10);
    
    s32Ret = HI_MPI_VI_DisableUserPic(ViPipe);
    printf("HI_MPI_VI_DisableUserPic with %#x!\n", s32Ret);
    sleep(10);
    
    //SAMPLE_COMM_VI_Release_UserPic(&stViFrameInfo);
    }
    #endif
    
    #if defined(__TEST_ASPECT__)
    ASPECT_RATIO_S aspect;
    aspect.enMode = ASPECT_RATIO_AUTO;
    aspect.u32BgColor = 0xff0000;
    gsf_mpp_vpss_ctl(0, GSF_MPP_VPSS_CTL_ASPECT, &aspect);
    printf("GSF_MPP_VPSS_CTL_ASPECT ASPECT_RATIO_AUTO\n");
    sleep(6);
    
    aspect.enMode = ASPECT_RATIO_NONE;
    aspect.u32BgColor = 0xff0000;
    printf("GSF_MPP_VPSS_CTL_ASPECT ASPECT_RATIO_NONE\n");
    gsf_mpp_vpss_ctl(0, GSF_MPP_VPSS_CTL_ASPECT, &aspect);
    sleep(6);
    #endif

    //#define __TEST_DIS__
    #if defined(__TEST_DIS__)
    gsf_mpp_img_dis_t dis;
    
    dis.bEnable = 0;
    dis.enMode = DIS_MODE_6_DOF_GME;
    dis.enPdtType = 0;
    gsf_mpp_isp_ctl(0, GSF_MPP_ISP_CTL_DIS, &dis);
    sleep(6);
    
    dis.bEnable = 1;
    dis.enMode = DIS_MODE_6_DOF_GME;
    dis.enPdtType = 1;
    gsf_mpp_isp_ctl(0, GSF_MPP_ISP_CTL_DIS, &dis);
    sleep(6);
    #endif
    
    if(1) //dzoom;
    {
      #define ZOOM_STEPS 100
      #define STEP_X  (MAX_W-MIN_W)/ZOOM_STEPS/2
      #define STEP_Y  (MAX_H-MIN_H)/ZOOM_STEPS/2
      
      static int MAX_W = 0;//3840;
      static int MAX_H = 0;//2160;
      static int MIN_W = 0;//MAX_W/4;
      static int MIN_H = 0;//MAX_H/4;
      static int cnt = 0, vpssGrp = 0;
      if(!MAX_W)
      {
        VPSS_GRP_ATTR_S stVpssGrpAttr = {0};
        gsf_mpp_vpss_ctl(vpssGrp, GSF_MPP_VPSS_CTL_ATTR, &stVpssGrpAttr);
        MAX_W = stVpssGrpAttr.u32MaxW;
        MAX_H = stVpssGrpAttr.u32MaxH;
        MIN_W = MAX_W/3.0;
        MIN_H = MAX_H/3.0;
      }
      
      if(dzoom_plus > 0)
      {
        if(cnt < ZOOM_STEPS-1)
          cnt++;
      }
      else if(dzoom_plus < 0)
      {
        if(cnt > 0)
          --cnt;
      }
      if(dzoom_plus)
      {
        int step = cnt%ZOOM_STEPS;
        VPSS_CROP_INFO_S stcrop;
        stcrop.bEnable = step?1:0;
        stcrop.enCropCoordinate = VPSS_CROP_ABS_COOR;
        stcrop.stCropRect.s32X = ALIGN_UP((int)step*STEP_X, 2);
        stcrop.stCropRect.s32Y = ALIGN_UP((int)step*STEP_Y, 2);
        stcrop.stCropRect.u32Width = ALIGN_UP((int)(MAX_W/2 - stcrop.stCropRect.s32X)*2, 2);
        stcrop.stCropRect.u32Height = ALIGN_UP((int)(MAX_H/2 - stcrop.stCropRect.s32Y)*2, 2);
        gsf_mpp_vpss_ctl(vpssGrp, GSF_MPP_VPSS_CTL_CROP, &stcrop);
      }
    }
}

#endif //#if defined(GSF_CPU_3516d) || defined(GSF_CPU_3559)