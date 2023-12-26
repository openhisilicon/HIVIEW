#if defined(GSF_CPU_3531d) || defined(GSF_CPU_3519) || defined(GSF_CPU_3516e)

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

static int avs = 0; // codec_ipc.vi.avs;

// if compile error, please add PIC_XXX to sample_comm.h:PIC_SIZE_E;
#ifndef HAVE_PIC_288P
#warning "PIC_288P => PIC_CIF"
#define PIC_288P PIC_CIF
#endif

#ifndef HAVE_PIC_360P
#warning "PIC_360P => PIC_CIF"
#define PIC_360P PIC_CIF
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
int second_sdp(int i, gsf_sdp_t *sdp)
{
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
    if((!codec_ipc.venc[j].en) || (j >= p_venc_ini->st_num && j != GSF_CODEC_SNAP_IDX))
      continue;

    gsf_mpp_venc_t venc = {
      .VencChn    = i*GSF_CODEC_VENC_NUM+j,
      .srcModId   = HI_ID_VPSS,
      .VpssGrp    = i,
      .VpssChn    = (j<p_venc_ini->st_num)?j:0,
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
    
    if(j < p_venc_ini->st_num) // st_num+1(JPEG);
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
      

#if defined(GSF_CPU_3519)
void mpp_ini_3519(gsf_mpp_cfg_t *cfg, gsf_rgn_ini_t *rgn_ini, gsf_venc_ini_t *venc_ini, gsf_mpp_vpss_t *vpss)
{
  if(strstr(cfg->snsname, "imx290"))
  {
  	// imx290-0-0-2-30
    cfg->lane = 0; cfg->wdr = 0; cfg->res = 2; cfg->fps = 30;
    rgn_ini->ch_num = 1; rgn_ini->st_num = 2;
    venc_ini->ch_num = 1; venc_ini->st_num = 2;
    VPSS_BIND_VI(0, 0, 0, 0, 1, 1, PIC_1080P, PIC_720P);
  }
  else
  {
    // imx334-0-0-8-30
    cfg->lane = 0; cfg->wdr = 0; cfg->res = 8; cfg->fps = 30;
    rgn_ini->ch_num = 2; rgn_ini->st_num = 1;
    venc_ini->ch_num = 2; venc_ini->st_num = 1;
    VPSS_BIND_VI(0, 0, 0, 0, 1, 0, PIC_3840x2160, PIC_720P);
    VPSS_BIND_VI(1, 1, 0, 1, 1, 0, PIC_1080P, PIC_720P);
  }
}
#endif

#if defined(GSF_CPU_3516e)
void mpp_ini_3516e(gsf_mpp_cfg_t *cfg, gsf_rgn_ini_t *rgn_ini, gsf_venc_ini_t *venc_ini, gsf_mpp_vpss_t *vpss)
{
  // sc2335-0-0-2-30
  cfg->lane = 0; cfg->wdr = 0; cfg->res = 2; cfg->fps = 30;
  rgn_ini->ch_num = 1; rgn_ini->st_num = 2;
  venc_ini->ch_num = 1; venc_ini->st_num = 2;
  VPSS_BIND_VI(0, 0, 0, 0, 1, 1, PIC_1080P, PIC_720P); 
}
#endif

#if defined(GSF_CPU_3531d)
void mpp_ini_3531d(gsf_mpp_cfg_t *cfg, gsf_rgn_ini_t *rgn_ini, gsf_venc_ini_t *venc_ini, gsf_mpp_vpss_t *vpss)
{
  // 
  //cfg->lane = 0; cfg->wdr = 0; cfg->res = 2; cfg->fps = 60;
  rgn_ini->ch_num = 4; rgn_ini->st_num = 2;
  venc_ini->ch_num = 4; venc_ini->st_num = 2;

  VPSS_BIND_VI(0, 0, 0, 0, 1, 1, PIC_1080P, PIC_D1_NTSC);
  VPSS_BIND_VI(1, 1, 0, 1, 1, 1, PIC_1080P, PIC_D1_NTSC);
  VPSS_BIND_VI(2, 2, 0, 2, 1, 1, PIC_1080P, PIC_D1_NTSC);
  VPSS_BIND_VI(3, 3, 0, 3, 1, 1, PIC_1080P, PIC_D1_NTSC);
//VPSS_BIND_VI(4, 4, 0, 4, 1, 0, PIC_1080P, PIC_D1_NTSC);
//VPSS_BIND_VI(5, 5, 0, 5, 1, 0, PIC_1080P, PIC_D1_NTSC);
//VPSS_BIND_VI(6, 6, 0, 6, 1, 0, PIC_1080P, PIC_D1_NTSC);
//VPSS_BIND_VI(7, 7, 0, 7, 1, 0, PIC_1080P, PIC_D1_NTSC);
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
      #if defined(GSF_CPU_3516e)
      {
        mpp_ini_3516e(&cfg, &rgn_ini, &venc_ini, vpss);
      }
      #elif defined(GSF_CPU_3519)
      {
        mpp_ini_3519(&cfg, &rgn_ini, &venc_ini, vpss);
      }
      #elif defined(GSF_CPU_3531d)
      {
        mpp_ini_3531d(&cfg, &rgn_ini, &venc_ini, vpss);
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
        #if defined(GSF_CPU_3516e)
        .vpss_en = {1, 1,},
        .vpss_sz = {PIC_1080P, PIC_720P,},
        #endif
    };

    gsf_mpp_vi_start(&vi);
    
    #if 0
    {
      //ttyAMA2: Single channel baseboard, ttyAMA4: double channel baseboard;
      char uart_name[32] = {0};
	    sprintf(uart_name, "/dev/%s", codec_ipc.lenscfg.uart);
	    gsf_lens_start(0, uart_name);
  	}
  	#endif
    
    // vpss start;
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
      ;
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
    return 0;
}

int main_start(gsf_bsp_def_t *bsp_def)
{    
    mpp_start(bsp_def);

    venc_start(1);

    vo_start();

    extern int vdec_start();
    if(vdec_start() < 0)
    {
      ;
    }
    
    return 0;
}

int main_loop(void)
{
  usleep(10*1000);
  return 0;
}

#endif //#if defined(GSF_CPU_3531d) || defined(GSF_CPU_3519) || defined(GSF_CPU_3516e)