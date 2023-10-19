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

//0: disable; 
//1: 1 PIP-main + 1 PIP-sub;
//2: 1 sensor + 1 thermal + 1 PIP;
static int avs = 0; // codec_ipc.vi.avs;

// if compile error, please add PIC_XXX to sample_comm.h:PIC_SIZE_E;
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

#if !defined(GSF_CPU_3403) && !defined(GSF_CPU_3519d)
  #ifndef HAVE_PIC_1520P
  #define PIC_2688x1520 PIC_2592x1520
  #warning "PIC_2688x1520 => PIC_2592x1520"
  #endif
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

GSF_LOG_GLOBAL_INIT("CODEC", 8*1024);

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
#if defined(GSF_CPU_3403) || defined(GSF_CPU_3519d)
#define SECOND_WIDTH(second) ((second) == 1?1920:(second) == 2?720:640)
#define SECOND_HEIGHT(second) ((second) == 1?1080:(second) == 2?480:512)
#define SECOND_HIRES(second) ((second) == 1?PIC_1080P:(second) == 2?PIC_D1_NTSC:PIC_512P)
#else
#define SECOND_WIDTH(second) ((second) == 1?720:(second) == 2?720:(second) == 4?384:640)
#define SECOND_HEIGHT(second) ((second) == 1?576:(second) == 2?480:(second) == 4?288:512)
#define SECOND_HIRES(second) ((second) == 1?PIC_D1_PAL:(second) == 2?PIC_D1_NTSC:(second) == 4?PIC_288P:PIC_512P)
#endif  

int second_sdp(int i, gsf_sdp_t *sdp)
{
  #if defined(GSF_CPU_3516d) || defined(GSF_CPU_3403) || defined(GSF_CPU_3519d)
  if(p_cfg->second)
  {
    sdp->audio_shmid = -1;
    if(i == 0 || i == 2) // main 
    {
      sdp->venc.width = SECOND_WIDTH(p_cfg->second);
      sdp->venc.height = SECOND_HEIGHT(p_cfg->second);
      sdp->venc.bitrate = (SECOND_WIDTH(p_cfg->second)>720)?8000:2000;
      sdp->venc.fps = p_cfg->fps; // fps: venc = vi;
    }
    else // sub
    {
      sdp->venc.width = (SECOND_WIDTH(p_cfg->second)>720)?720:352;
      sdp->venc.height = (SECOND_WIDTH(p_cfg->second)>720)?480:288;
      sdp->venc.bitrate = (SECOND_WIDTH(p_cfg->second)>720)?2000:1000;
      sdp->venc.fps = p_cfg->fps; // fps: venc = vi;
    }
    return 0; 
  }
  #endif
  
  return -1;
}

// mod msg;
static int req_recv(char *in, int isize, char *out, int *osize, int err)
{
    int ret = 0;
    gsf_msg_t *req = (gsf_msg_t*)in;
    gsf_msg_t *rsp = (gsf_msg_t*)out;

    *rsp  = *req;
    rsp->err  = 0;
    rsp->size = 0;

    ret = msg_func_proc(req, isize, rsp, osize);

    rsp->err = (ret == TRUE)?rsp->err:GSF_ERR_MSG;
    *osize = sizeof(gsf_msg_t)+rsp->size;
    printf("req->id:%d, set:%d, rsp->err:%d, size:%d\n", req->id, req->set, rsp->err, rsp->size);
    return 0;
}

// osd msg;
static int osd_recv(char *msg, int size, int err)
{
  gsf_osd_act_t *act = ( gsf_osd_act_t *)msg;
  gsf_rgn_canvas(0, 7, act);
  return 0;
}

// svp event;
static int sub_recv(char *msg, int size, int err)
{
  gsf_msg_t *pmsg = (gsf_msg_t*)msg;

  if(pmsg->id == GSF_EV_SVP_MD)
  {
    gsf_svp_mds_t *mds = (gsf_svp_mds_t*) pmsg->data;
    
    int i = 0;
    for(i = 0; i < 4 && i < mds->cnt; i++)
    {
      gsf_osd_t osd;

      osd.en = 1;
      osd.type = 0;
      osd.fontsize = 0;
      osd.point[0] = (unsigned int)((float)mds->result[i].rect[0]/(float)mds->w*codec_ipc.venc[0].width)&(-1);
      osd.point[1] = (unsigned int)((float)mds->result[i].rect[1]/(float)mds->h*codec_ipc.venc[0].height)&(-1);
      osd.wh[0]    = (unsigned int)((float)mds->result[i].rect[2]/(float)mds->w*codec_ipc.venc[0].width)&(-1);
      osd.wh[1]    = (unsigned int)((float)mds->result[i].rect[3]/(float)mds->h*codec_ipc.venc[0].height)&(-1);
      
      sprintf(osd.text, "ID: %d", i);
      
      printf("GSF_EV_SVP_MD idx: %d, osd: x:%d,y:%d,w:%d,h:%d\n"
            , i, osd.point[0], osd.point[1], osd.wh[0], osd.wh[1]);
            
      gsf_rgn_osd_set(0, i, &osd);
    }
  }
  else if(pmsg->id == GSF_EV_SVP_LPR)
  {
    gsf_svp_lprs_t *lprs = (gsf_svp_lprs_t*) pmsg->data;
    
    int i = 0;
    //lprs->cnt = 1;
    for(i = 0; i < 4 && i < lprs->cnt; i++)
    {
      gsf_osd_t osd;

      osd.en = 1;
      osd.type = 0;
      osd.fontsize = 1;

      osd.point[0] = (unsigned int)((float)lprs->result[i].rect[0]/(float)lprs->w*codec_ipc.venc[0].width)&(-1);
      osd.point[1] = (unsigned int)((float)lprs->result[i].rect[1]/(float)lprs->h*codec_ipc.venc[0].height)&(-1);
      osd.wh[0]    = (unsigned int)((float)lprs->result[i].rect[2]/(float)lprs->w*codec_ipc.venc[0].width)&(-1);
      osd.wh[1]    = (unsigned int)((float)lprs->result[i].rect[3]/(float)lprs->h*codec_ipc.venc[0].height)&(-1);
      
      char utf8str[32] = {0};
      gsf_gb2312_to_utf8(lprs->result[i].number, strlen(lprs->result[i].number), utf8str);
      sprintf(osd.text, "%s", utf8str);
      
      //printf("GSF_EV_SVP_LPR idx: %d, osd: rect: [%d,%d,%d,%d], utf8:[%s]\n"
      //      , i, osd.point[0], osd.point[1], osd.wh[0], osd.wh[1], osd.text);
      
      gsf_rgn_osd_set(0, i, &osd);
    }
  }
  else if(pmsg->id == GSF_EV_SVP_YOLO)
  {
    gsf_svp_yolos_t *yolos = (gsf_svp_yolos_t*) pmsg->data;
    
    int i = 0;
    gsf_rgn_rects_t rects = {0};

    float xr = 0, yr = 0, wr = 1, hr = 1;
    int chn = pmsg->ch;
    
    #if defined(GSF_CPU_3516d)
    if(avs == 1)
    {
      xr = 0 + pmsg->ch*(yolos->w/2.0);
      yr = yolos->h/4.0, wr = 2.0, hr = 2.0;
      chn = 0;
    }
    #endif
    
    rects.size = yolos->cnt;
    rects.w = yolos->w;
    rects.h = yolos->h;
    
    int j = 0;
    for(i = 0; i < yolos->cnt; i++)
    {
      rects.box[j].rect[0] = xr + yolos->box[i].rect[0]/wr;
      rects.box[j].rect[1] = yr + yolos->box[i].rect[1]/hr;
      rects.box[j].rect[2] = yolos->box[i].rect[2]/wr;
      rects.box[j].rect[3] = yolos->box[i].rect[3]/hr;
      gsf_gb2312_to_utf8(yolos->box[i].label, strlen(yolos->box[i].label), rects.box[j].label);
      
      j++;
    } rects.size = j;
    
    // osd to sub-stream if main-stream > 1080P;
    //printf("chn:%d, rects.size:%d\n", chn, rects.size);
    gsf_rgn_rect_set(chn, 0, &rects, (codec_ipc.venc[0].width>1920)?2:1);
    //gsf_rgn_nk_set(0, 0, &rects, (codec_ipc.venc[0].width>1920)?2:1);
    
  }
  else if(pmsg->id == GSF_EV_SVP_CFACE)
  {
    gsf_svp_cfaces_t *cfaces = (gsf_svp_cfaces_t*) pmsg->data;
    
    int i = 0;

    gsf_rgn_rects_t rects = {0};

    rects.size = cfaces->cnt;
    rects.w = cfaces->w;
    rects.h = cfaces->h;
    
    for(i = 0; i < cfaces->cnt; i++)
    {
      rects.box[i].rect[0] = cfaces->box[i].rect[0];
      rects.box[i].rect[1] = cfaces->box[i].rect[1];
      rects.box[i].rect[2] = cfaces->box[i].rect[2];
      rects.box[i].rect[3] = cfaces->box[i].rect[3];
      sprintf(rects.box[i].label, "%d", cfaces->box[i].id);
    }
    
    #if 0
    struct timespec ts1, ts2;  
    clock_gettime(CLOCK_MONOTONIC, &ts1);
    #endif
    
    // osd to sub-stream if main-stream > 1080P;
    gsf_rgn_rect_set(0, 0, &rects, (codec_ipc.venc[0].width>1920)?2:1);
    //gsf_rgn_nk_set(0, 0, &rects, (codec_ipc.venc[0].width>1920)?2:1);
    
  	#if 0
    clock_gettime(CLOCK_MONOTONIC, &ts2);
    int cost = (ts2.tv_sec*1000 + ts2.tv_nsec/1000000) - (ts1.tv_sec*1000 + ts1.tv_nsec/1000000);
    printf("gsf_rgn_rect_set cost:%d ms\n", cost);
  	#endif


  }
  
  return 0;
}

static int reg2bsp()
{
  while(1)
  {
    //register To;
    GSF_MSG_DEF(gsf_mod_reg_t, reg, 8*1024);
    reg->mid = GSF_MOD_ID_CODEC;
    strcpy(reg->uri, GSF_IPC_CODEC);
    int ret = GSF_MSG_SENDTO(GSF_ID_MOD_CLI, 0, SET, GSF_CLI_REGISTER, sizeof(gsf_mod_reg_t), GSF_IPC_BSP, 2000);
    printf("GSF_CLI_REGISTER To:%s, ret:%d, size:%d\n", GSF_IPC_BSP, ret, __rsize);
    
    static int cnt = 3;
    if(ret == 0)
      break;
    if(cnt-- < 0)
      return -1;
    sleep(1);
  }
  return 0;
}

static int getdef(gsf_bsp_def_t *def)
{
  GSF_MSG_DEF(gsf_msg_t, msg, 4*1024);
  int ret = GSF_MSG_SENDTO(GSF_ID_BSP_DEF, 0, GET, 0, 0, GSF_IPC_BSP, 2000);
  gsf_bsp_def_t *cfg = (gsf_bsp_def_t*)__pmsg->data;
  printf("GET GSF_ID_BSP_DEF To:%s, ret:%d, type:%s, snscnt:%d, sensor:[%s]\n"
        , GSF_IPC_BSP, ret, cfg->board.type, cfg->board.snscnt, cfg->board.sensor[0]);

  if(ret < 0)
    return ret;

  if(def) 
    *def = *cfg;
    
  return ret;
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

    #if defined(GSF_CPU_3516d)
    if(strcasestr(p_cfg->type, "3516a"))
    {
      if(strstr(p_cfg->snsname, "imx378")/*||strstr(p_cfg->snsname, "imx585")*/)
      {
        venc.VpssChn = 0;
      }
    }
    #endif

    if(avs == 1)
    {  
      #if defined(GSF_CPU_3559a)
      venc.srcModId   = HI_ID_AVS;
      venc.AVSGrp     = i;
      venc.AVSChn    = (j<p_venc_ini->st_num)?j:0;
      #elif defined(GSF_CPU_3516d)
      venc.srcModId   = HI_ID_VO;
      venc.VpssGrp    = -1;
      venc.VpssChn    = -1;
      #endif
    }
    
    //only 3516D supported 
    #if defined(GSF_CPU_3516d) || defined(GSF_CPU_3403) || defined(GSF_CPU_3519d)
    if(p_cfg->second && i == 1)
    {
      gsf_sdp_t sdp;
      if(second_sdp(j, &sdp) == 0)
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
    
    #endif //GSF_CPU_3516d

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
  printf("start >>> gsf_mpp_venc_recv s32Cnt:%d, cb:%p\n", st.s32Cnt, st.cb);
  gsf_mpp_venc_recv(&st);
  return 0;
}

#define VPSS_BIND_VI(_i, _vip, _vich, _vpg, _en0, _en1, _sz0, _sz1) do{ \
      vpss[_i].VpssGrp=_vpg; vpss[_i].ViPipe=_vip; vpss[_i].ViChn=_vich;\
      vpss[_i].enable[0]=_en0;vpss[_i].enable[1]=_en1;\
      vpss[_i].enSize[0]=_sz0;vpss[_i].enSize[1]=_sz1;}while(0)
      
#if defined(GSF_CPU_3559a)
void mpp_ini_3559a(gsf_mpp_cfg_t *cfg, gsf_rgn_ini_t *rgn_ini, gsf_venc_ini_t *venc_ini, gsf_mpp_vpss_t *vpss)
{
  if (avs == 1)
  {
    // imx334-0-0-8-30
    cfg->lane = 0; cfg->wdr = 0; cfg->res = 8; cfg->fps = 30;
    rgn_ini->ch_num = 1; rgn_ini->st_num = 1;
    venc_ini->ch_num = 1; venc_ini->st_num = 1;
    VPSS_BIND_VI(0, 0, 0, 0, 1, 0, PIC_1080P, PIC_720P);
    VPSS_BIND_VI(1, 1, 0, 1, 1, 0, PIC_1080P, PIC_720P);
    VPSS_BIND_VI(2, 2, 0, 2, 1, 0, PIC_1080P, PIC_720P);
    VPSS_BIND_VI(3, 3, 0, 3, 1, 0, PIC_1080P, PIC_720P);   
  }
  else
  {
    if(strstr(cfg->snsname, "sharp8k"))
    {
      //sharp8k-0-0-16-30
      cfg->lane = 0; cfg->wdr = 0; cfg->res = 16; cfg->fps = 30;
      rgn_ini->ch_num = 1; rgn_ini->st_num = 2;
      venc_ini->ch_num = 1; venc_ini->st_num = 2;
      VPSS_BIND_VI(0, 0, 0, 0, 1, 1, PIC_7680x4320, PIC_3840x2160);
    }
    else
    {
      // imx334-0-0-8-30
      cfg->lane = 0; cfg->wdr = 0; cfg->res = 8; cfg->fps = 30;
      rgn_ini->ch_num = 4; rgn_ini->st_num = 2;
      venc_ini->ch_num = 4; venc_ini->st_num = 2;
      VPSS_BIND_VI(0, 0, 0, 0, 1, 1, PIC_3840x2160, PIC_720P);
      VPSS_BIND_VI(1, 1, 0, 1, 1, 1, PIC_3840x2160, PIC_720P);
      VPSS_BIND_VI(2, 2, 0, 2, 1, 1, PIC_3840x2160, PIC_720P);
      VPSS_BIND_VI(3, 3, 0, 3, 1, 1, PIC_3840x2160, PIC_720P);
    } 
  }
}
#endif
#if defined(GSF_CPU_3519a)
void mpp_ini_3519a(gsf_mpp_cfg_t *cfg, gsf_rgn_ini_t *rgn_ini, gsf_venc_ini_t *venc_ini, gsf_mpp_vpss_t *vpss)
{
  // imx334-0-0-8-60
  cfg->lane = 0; cfg->wdr = 0; cfg->res = 8; cfg->fps = 30;
  rgn_ini->ch_num = 1; rgn_ini->st_num = 2;
  venc_ini->ch_num = 1; venc_ini->st_num = 2;
  VPSS_BIND_VI(0, 0, 0, 0, 1, 1, PIC_3840x2160, PIC_720P);
}
#endif
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
      cfg->lane = 0; cfg->wdr = 0; cfg->res = 2; cfg->fps = 60;
      rgn_ini->ch_num = 1; rgn_ini->st_num = 2;
      venc_ini->ch_num = 1; venc_ini->st_num = 2;
      VPSS_BIND_VI(0, 0, 0, 0, 1, 1, PIC_1080P, PIC_D1_NTSC);
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


#if defined(GSF_CPU_3403)
void mpp_ini_3403(gsf_mpp_cfg_t *cfg, gsf_rgn_ini_t *rgn_ini, gsf_venc_ini_t *venc_ini, gsf_mpp_vpss_t *vpss)
{
   if(strstr(cfg->snsname, "imx482"))
   {
    // imx482-0-0-2-30
    cfg->lane = 0; cfg->wdr = 0; cfg->res = 2; cfg->fps = 30;
    rgn_ini->ch_num = 1; rgn_ini->st_num = 2;
    venc_ini->ch_num = 1; venc_ini->st_num = 2;
    VPSS_BIND_VI(0, 0, 0, 0, 1, 1, PIC_1080P, PIC_1080P);
    if(cfg->snscnt > 1)
    {
        // imx482-0-0-2-30
        VPSS_BIND_VI(1, 1, 0, 1, 1, 1, PIC_1080P, PIC_1080P);
        rgn_ini->ch_num++;
        venc_ini->ch_num++;
    } 
    else if(cfg->second)
    {
        rgn_ini->ch_num = venc_ini->ch_num = 2;
        VPSS_BIND_VI(1, 1, 0, 1, 1, 1, PIC_1080P, PIC_D1_NTSC);
    }
    return;
   }
   
   if(strstr(cfg->snsname, "os04a10"))
   {
    // os04a10-0-0-4-30
    //cfg->lane = (cfg->snscnt>1)?2:0; cfg->wdr = 0; cfg->res = 4; cfg->fps = 30;
    cfg->lane = 2; cfg->wdr = 0; cfg->res = 4; cfg->fps = 30;  
    rgn_ini->ch_num = 1; rgn_ini->st_num = 2;
    venc_ini->ch_num = 1; venc_ini->st_num = 2;
    VPSS_BIND_VI(0, 0, 0, 0, 1, 1, PIC_2688x1520, PIC_720P);
    
    if(cfg->snscnt > 1)
    for(int i = 1; i < cfg->snscnt; i++)
    {
        VPSS_BIND_VI(i, i, 0, i, 1, 1, PIC_2688x1520, PIC_720P);
        rgn_ini->ch_num++;
        venc_ini->ch_num++;
    } 
    else if(cfg->second)
    {
        rgn_ini->ch_num = venc_ini->ch_num = 2;
        VPSS_BIND_VI(1, 1, 0, 1, 1, 1, PIC_1080P, PIC_D1_NTSC);
    }
    return;
   }
  
  // os08a20-0-0-8-30
  cfg->lane = 0; cfg->wdr = 0; cfg->res = 8; cfg->fps = (codec_ipc.vi.fps>0)?codec_ipc.vi.fps:30;
  rgn_ini->ch_num = 1; rgn_ini->st_num = 2;
  venc_ini->ch_num = 1; venc_ini->st_num = 2;
  VPSS_BIND_VI(0, 0, 0, 0, 1, 1, PIC_3840x2160, PIC_640P);
  if(cfg->snscnt > 1)
  {
      // os08a20-0-0-8-30
      VPSS_BIND_VI(1, 1, 0, 1, 1, 1, PIC_3840x2160, PIC_640P);
      rgn_ini->ch_num++;
      venc_ini->ch_num++;
  }
  else if(cfg->second)
  {
      rgn_ini->ch_num = venc_ini->ch_num = 2;
      rgn_ini->st_num = venc_ini->st_num = 2;
      VPSS_BIND_VI(1, 1, 0, 1, 1, 1, PIC_1080P, PIC_640P);
  }
}
#endif

#if defined(GSF_CPU_3519d)

static char* _chipid()
{
  static char chipid[64] = {0};
  sprintf(chipid, "%s", "3519d500");
  
  char str[128];
  sprintf(str, "%s", "bspmd.l 0x011020EE0 | grep 3516d500");
  FILE* fd = popen(str, "r");
  if(!fd)
    return chipid;

  while(1)
  {
    if (fgets(str, sizeof(str), fd))
  	{
  		if(strstr(str, "3516d500"))
        sprintf(chipid, "%s", "3516d500");
  		else if(strstr(str, "3519d500"))
  		  sprintf(chipid, "%s", "3519d500");
  	}
  	else
  	{
  	  break;
  	}
  }
  pclose(fd);
  return chipid;
}


void mpp_ini_3519d(gsf_mpp_cfg_t *cfg, gsf_rgn_ini_t *rgn_ini, gsf_venc_ini_t *venc_ini, gsf_mpp_vpss_t *vpss)
{
   sprintf(cfg->type, "%s", _chipid());
  
   if(strstr(cfg->snsname, "os04a10"))
   {
    // os04a10-0-0-4-30
    cfg->lane = (cfg->snscnt>1)?2:0; cfg->wdr = 0; cfg->res = 4; cfg->fps = 30;
    rgn_ini->ch_num = 1; rgn_ini->st_num = 2;
    venc_ini->ch_num = 1; venc_ini->st_num = 2;
    VPSS_BIND_VI(0, 0, 0, 0, 1, 1, PIC_2688x1520, PIC_640P);
    if(cfg->snscnt > 1)
    {
        VPSS_BIND_VI(1, 1, 0, 1, 1, 1, PIC_2688x1520, PIC_640P);
        rgn_ini->ch_num++;
        venc_ini->ch_num++;
    } 
    else if(cfg->second)
    {
        rgn_ini->ch_num = venc_ini->ch_num = 2;
        VPSS_BIND_VI(1, 1, 0, 1, 1, 1, PIC_1080P, PIC_640P);
    }
    return;
   }  
  
  // os08a20-2-0-2-30
  if(codec_ipc.vi.res==2)
  {
    cfg->lane = 2; cfg->wdr = 0; cfg->res = 2; cfg->fps = (codec_ipc.vi.fps>0)?codec_ipc.vi.fps:30;
    rgn_ini->ch_num = 1; rgn_ini->st_num = 2;
    venc_ini->ch_num = 1; venc_ini->st_num = 2;
    VPSS_BIND_VI(0, 0, 0, 0, 1, 1, PIC_1080P, PIC_640P);
    if(cfg->snscnt > 1)
    {
        VPSS_BIND_VI(1, 1, 0, 1, 1, 1, PIC_1080P, PIC_640P);
        rgn_ini->ch_num++;
        venc_ini->ch_num++;
    }
    else if(cfg->second)
    {
        rgn_ini->ch_num = venc_ini->ch_num = 2;
        rgn_ini->st_num = venc_ini->st_num = 2;
        VPSS_BIND_VI(1, 1, 0, 1, 1, 1, PIC_1080P, PIC_640P);
    }
    return;
  }
  
  // os08a20-0-0-8-30
  cfg->lane = 0; cfg->wdr = 0; cfg->res = 8; cfg->fps = (codec_ipc.vi.fps>0)?codec_ipc.vi.fps:30;
  rgn_ini->ch_num = 1; rgn_ini->st_num = 2;
  venc_ini->ch_num = 1; venc_ini->st_num = 2;
  VPSS_BIND_VI(0, 0, 0, 0, 1, 1, PIC_3840x2160, PIC_640P);
  if(cfg->snscnt > 1)
  {
      // os08a20-0-0-8-30
      VPSS_BIND_VI(1, 1, 0, 1, 1, 1, PIC_3840x2160, PIC_640P);
      rgn_ini->ch_num++;
      venc_ini->ch_num++;
  }
  else if(cfg->second)
  {
      rgn_ini->ch_num = venc_ini->ch_num = 2;
      rgn_ini->st_num = venc_ini->st_num = 2;
      VPSS_BIND_VI(1, 1, 0, 1, 1, 1, PIC_1080P, PIC_640P);
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
      #if defined(GSF_CPU_3559a)
      {
        mpp_ini_3559a(&cfg, &rgn_ini, &venc_ini, vpss);
      }
      #elif defined(GSF_CPU_3519a)
      {
        mpp_ini_3519a(&cfg, &rgn_ini, &venc_ini, vpss);
      }
      #elif defined(GSF_CPU_3516d)
      {
        // cpu type try sstat_chipid(), otherwise use board.type;
        char *chipid = sstat_chipid(); // [3516a300 3516d300 35590200]
        printf("chipid[%s]\n", chipid);
        strcpy(cfg.type, def->board.type);
        chipid = !strlen(chipid)?NULL:strcpy(cfg.type, chipid);
        //second channel from bsp_def.json; [0: disable, 1: BT656.PAL, 2:BT656.NTSC, 3: BT656.512P, 4:BT656.288P, 5:USB.GD]
        cfg.second = (cfg.snscnt > 1)?0:
                     (def->board.second <= 0)?0:def->board.second;
        printf("BOARD [type:%s, second:%d]\n", cfg.type, cfg.second);
        
        mpp_ini_3516d(&cfg, &rgn_ini, &venc_ini, vpss);
      }
      #elif defined(GSF_CPU_3516e)
      {
        mpp_ini_3516e(&cfg, &rgn_ini, &venc_ini, vpss);
      }
      #elif defined(GSF_CPU_3519)
      {
        mpp_ini_3519(&cfg, &rgn_ini, &venc_ini, vpss);
      }
      #elif defined(GSF_CPU_3559)
      {
        mpp_ini_3559(&cfg, &rgn_ini, &venc_ini, vpss);
      }
      #elif defined(GSF_CPU_3531d)
      {
        mpp_ini_3531d(&cfg, &rgn_ini, &venc_ini, vpss);
      }
      #elif defined(GSF_CPU_3403)
      {
        cfg.second = def->board.second;
        cfg.hnr = codec_ipc.vi.hnr;
        mpp_ini_3403(&cfg, &rgn_ini, &venc_ini, vpss);
      }
      #elif defined(GSF_CPU_3519d)
      {
        cfg.second = def->board.second;
        cfg.aiisp = codec_ipc.vi.hnr;
        mpp_ini_3519d(&cfg, &rgn_ini, &venc_ini, vpss);
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
        #elif defined(GSF_CPU_3403) || defined(GSF_CPU_3519d)
        .venc_pic_size = {PIC_3840x2160, PIC_1080P},
        #endif
    };
    //re-set vpss output size;
    #if defined(GSF_CPU_3403) || defined(GSF_CPU_3519d)
    vi.venc_pic_size[0] = p_vpss[0].enSize[0];
    vi.venc_pic_size[1] = p_vpss[0].enSize[1];
    #endif
    
    #if defined(GSF_CPU_3559a)
    if(avs == 1)
    {
        vi.stStitchGrpAttr.bStitch = HI_TRUE;
        vi.stStitchGrpAttr.enMode  = VI_STITCH_ISP_CFG_NORMAL;
        vi.stStitchGrpAttr.u32PipeNum = 4;
        vi.stStitchGrpAttr.PipeId[0] = 0;
        vi.stStitchGrpAttr.PipeId[1] = 1;
        vi.stStitchGrpAttr.PipeId[2] = 2;
        vi.stStitchGrpAttr.PipeId[3] = 3;
    }
    #endif
  
    gsf_mpp_vi_start(&vi);
    
    #if defined(GSF_CPU_3559a)
    if(1)
    {
    	struct timespec _ts;  
      clock_gettime(CLOCK_REALTIME, &_ts);
      HI_U64 u64PTSBase = _ts.tv_sec;
      u64PTSBase = _ts.tv_sec*1000000 + _ts.tv_nsec/1000;
      int ret = HI_MPI_SYS_InitPTSBase(u64PTSBase);//建议在媒体业务没有启动时调用这个接口
      //HI_S32 HI_MPI_SYS_SyncPTS(u64PTSBase);  //建议一秒钟进行一次时间戳微调;
      HI_U64 u64CurPTS;
      ret |= HI_MPI_SYS_GetCurPTS(&u64CurPTS);       
      printf("HI_MPI_SYS_InitPTSBase ret:%d, u64PTSBase:%llu, u64CurPTS:%llu\n", ret, u64PTSBase, u64CurPTS);
    }
    #endif
    
    #if defined(GSF_CPU_3516d) || defined(GSF_CPU_3559)
    {
      //ttyAMA2: Single channel baseboard, ttyAMA4: double channel baseboard;
      char uart_name[32] = {0};
	    sprintf(uart_name, "/dev/%s", codec_ipc.lenscfg.uart);
	    gsf_lens_start(0, uart_name);
  	}
  	#endif
    
    // vpss start;
    if(avs == 1)
    {
      #if defined(GSF_CPU_3559a)
      for(i = 0; i < 4; i++)
      #elif defined(GSF_CPU_3516d)
      for(i = 0; i < 2; i++)
      #endif
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

    #if defined(GSF_CPU_3559a)
      sprintf(scene_ini, "%s/../cfg/%s%s.ini", scene_ini, cfg.snsname, (avs)?"avs":"");
    #elif defined(GSF_CPU_3403)
      sprintf(scene_ini, "%s/../cfg/%s%s.ini", scene_ini, cfg.snsname, (cfg.hnr)?"_hnr":"");
    #elif defined(GSF_CPU_3519d)
      sprintf(scene_ini, "%s/../cfg/%s%s.ini", scene_ini, cfg.snsname, (cfg.aiisp)?"_hnr":"");
    #else
      sprintf(scene_ini, "%s/../cfg/%s.ini", scene_ini, cfg.snsname);
    #endif
    
   
    if(gsf_mpp_scene_start(scene_ini, codec_ipc.vi.wdr) <= 0)  //enable img;
    {
      // First tested on Hi3516X;
      #if defined(GSF_CPU_3516d) || defined(GSF_CPU_3559) ||  defined(GSF_CPU_3403)
      printf("scene_auto is bEnable=0, set img(magic:0x%x) to isp.\n", codec_ipc.img.magic);
      if(codec_ipc.img.magic == 0x55aa) 
      {
        gsf_mpp_isp_ctl(0, GSF_MPP_ISP_CTL_CSC, &codec_ipc.img.csc);
        gsf_mpp_isp_ctl(0, GSF_MPP_ISP_CTL_AE, &codec_ipc.img.ae);
        gsf_mpp_isp_ctl(0, GSF_MPP_ISP_CTL_DEHAZE, &codec_ipc.img.dehaze);
        //gsf_mpp_isp_ctl(0, GSF_MPP_ISP_CTL_SCENE, &codec_ipc.img.scene);
        gsf_mpp_isp_ctl(0, GSF_MPP_ISP_CTL_SHARPEN, &codec_ipc.img.sharpen);
        gsf_mpp_isp_ctl(0, GSF_MPP_ISP_CTL_HLC, &codec_ipc.img.hlc);
        gsf_mpp_isp_ctl(0, GSF_MPP_ISP_CTL_GAMMA, &codec_ipc.img.gamma);
        gsf_mpp_isp_ctl(0, GSF_MPP_ISP_CTL_DRC, &codec_ipc.img.drc);
        gsf_mpp_isp_ctl(0, GSF_MPP_ISP_CTL_LDCI, &codec_ipc.img.ldci);
        gsf_mpp_isp_ctl(0, GSF_MPP_ISP_CTL_3DNR, &codec_ipc.img._3dnr);
      }
      else 
      {
        codec_ipc.img.magic = 0x55aa;
        gsf_mpp_isp_ctl(0, GSF_MPP_ISP_CTL_IMG, &codec_ipc.img);
      }
      #endif
    }

    #if defined(GSF_CPU_3559a)
    if(avs == 1)
    {  
      gsf_mpp_avs_t avs = {
        .AVSGrp = 0,
        .u32OutW = 1920*4,
        .u32OutH = 1080,
        .u32PipeNum = 4,
        .enMode = AVS_MODE_NOBLEND_HOR,
        .benChn1 = 0,
      };
      gsf_mpp_avs_start(&avs);
    }
    #endif
    
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
    #if defined(GSF_CPU_3516d) || defined(GSF_CPU_3559) || defined(GSF_CPU_3559a) || defined(GSF_CPU_3403) || defined(GSF_CPU_3519d)
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
      
      #if defined(GSF_CPU_3403) || defined(GSF_CPU_3519d)
      gsf_mpp_audio_start(NULL);
      #else
      gsf_mpp_audio_start(&aenc);
      #endif
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
      #if defined(GSF_CPU_3519d)
      inf = HI_VO_INTF_BT1120;
      #endif
      
      int sync = (codec_ipc.vo.sync == 3)?VO_OUTPUT_7680x4320_30:
            (codec_ipc.vo.sync == 2)?VO_OUTPUT_3840x2160_60:
            (codec_ipc.vo.sync == 1)?VO_OUTPUT_3840x2160_30:
            VO_OUTPUT_1080P60;
      printf("codec_ipc.vo.sync:%d\n", codec_ipc.vo.sync);
      gsf_mpp_vo_start(VODEV_HD0, inf, sync, 0);
      
      int ly = VO_LAYOUT_1MUX;
      #if defined(GSF_CPU_3516d) || defined(GSF_CPU_3403) || defined(GSF_CPU_3519d)
      ly = (p_cfg->second || avs == 1)?VO_LAYOUT_2MUX:p_cfg->snscnt;
      #endif
      
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
      
      #if defined(GSF_CPU_3516d) || defined(GSF_CPU_3403) || defined(GSF_CPU_3519d)
      if(ly == VO_LAYOUT_2MUX)
      {
        //win1;
        if(p_cfg->second)
    	  {
    	    gsf_sdp_t sdp;
          RECT_S rect = {20, 0, 720, 576};
          if(second_sdp(0, &sdp) == 0)
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
      for(int i = 2; i < ly; i++)
      {
        //win2 win3 win4 ...;
        gsf_mpp_vo_src_t src1 = {i, 0};
        gsf_mpp_vo_bind(VOLAYER_HD0, i, &src1);
      }      
      #endif
      
      if(sync == VO_OUTPUT_1080P60)
        vo_res_set(1920, 1080);
      else
        vo_res_set(3840, 2160);
    }
    #endif // defined(GSF_CPU_3516d) || defined(GSF_CPU_3559)
    
    return 0;
}


int dzoom_plus = 0;

int main(int argc, char *argv[])
{
    gsf_bsp_def_t bsp_def;
    if(argc < 2)
    {
      printf("pls input: %s codec_parm.json\n", argv[0]);
      return -1;
    }
    
    strncpy(codec_parm_path, argv[1], sizeof(codec_parm_path)-1);
    
    if(json_parm_load(codec_parm_path, &codec_ipc) < 0)
    {
      json_parm_save(codec_parm_path, &codec_ipc);
      json_parm_load(codec_parm_path, &codec_ipc);
    }
    
    info("parm.venc[0].type:%d, width:%d\n"
          , codec_ipc.venc[0].type
          , codec_ipc.venc[0].width);
          
    // register to bsp && get bsp_def;
    if(reg2bsp() < 0)
      return -1;

    if(getdef(&bsp_def) < 0)
      return -1;

    GSF_LOG_CONN(1, 100);

    mpp_start(&bsp_def);

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
    
    extern int vdec_start();
    if(vdec_start() < 0)
    {
      gsf_mpp_ao_bind(SAMPLE_AUDIO_INNER_AO_DEV, 0, SAMPLE_AUDIO_INNER_AI_DEV, 0);
      #ifndef GSF_CPU_3519d
      gsf_mpp_ao_bind(SAMPLE_AUDIO_INNER_HDMI_AO_DEV, 0, SAMPLE_AUDIO_INNER_AI_DEV, 0);
      #endif
    }
    
    //init listen;
    void* rep = nm_rep_listen(GSF_IPC_CODEC
                      , NM_REP_MAX_WORKERS
                      , NM_REP_OSIZE_MAX
                      , req_recv);
                      
    void* osd_pull = nm_pull_listen(GSF_IPC_OSD, osd_recv);
    
    void* sub = nm_sub_conn(GSF_PUB_SVP, sub_recv);
    printf("nm_sub_conn sub:%p\n", sub);

    #if defined(GSF_CPU_3516d) || defined(GSF_CPU_3403)
    //flip&mirror;
    gsf_mpp_img_flip_t flip;    
    flip.bFlip = codec_ipc.vi.flip;
    flip.bMirror = codec_ipc.vi.flip;
    gsf_mpp_isp_ctl(0, GSF_MPP_ISP_CTL_FLIP, &flip);
    #endif
    
    while(1)
    {
      usleep(10*1000);
      
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
      
      #if defined(GSF_CPU_3516d)
      //if(strstr(p_cfg->snsname, "imx482") || strstr(p_cfg->snsname, "imx585"))
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
      #endif
    }

    GSF_LOG_DISCONN();
    return 0;
}