#include <unistd.h>
#include <stdlib.h>
#include <assert.h>

#include "inc/gsf.h"
#include "fw/comm/inc/proc.h"
#include "mod/bsp/inc/bsp.h"
#include "mod/svp/inc/svp.h"
#include "codec.h"
#include "cfg.h"
#include "msg_func.h"
#include "mpp.h"
#include "live.h"
#include "rgn.h"
#include "venc.h"

#ifndef PIC_VGA
#define PIC_VGA PIC_D1_NTSC
#endif

#define PIC_WIDTH(w) \
            (w >= 3840)?PIC_3840x2160:\
            (w >= 1920)?PIC_1080P:\
            (w >= 1280)?PIC_720P: \
            (w >= 720)?PIC_D1_NTSC: \
            PIC_VGA
            
#define PT_VENC(t) \
            (t == GSF_ENC_H264)? PT_H264:\
            (t == GSF_ENC_H265)? PT_H265:\
            (t == GSF_ENC_JPEG)? PT_JPEG:\
            (t == GSF_ENC_MJPEG)? PT_MJPEG:\
            PT_H264


GSF_LOG_GLOBAL_INIT("CODEC", 8*1024);

//resolution;
static gsf_resolu_t vores;
#define vo_res_set(_w, _h) do{vores.w = _w; vores.h = _h;}while(0)
int vo_res_get(gsf_resolu_t *res) { *res = vores; return 0;}

//layout;
static gsf_layout_t voly;
int vo_ly_set(int ly) {voly.layout = ly; return 0;}
int vo_ly_get(gsf_layout_t *ly) { *ly = voly; return 0;}

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



static int sub_recv(char *msg, int size, int err)
{
  gsf_msg_t *pmsg = (gsf_msg_t*)msg;
  
  //printf("pmsg->id:%d\n", pmsg->id);
  if(pmsg->id == GSF_EV_SVP_YOLO)
  {
    gsf_svp_yolos_t *yolos = (gsf_svp_yolos_t*) pmsg->data;
    
    int i = 0;
    gsf_rgn_rects_t rects = {0};

    float xr = 0, yr = 0, wr = 1, hr = 1;
    int chn = pmsg->ch;
        
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
    gsf_rgn_rect_set(chn, 0, &rects, (codec_nvr.venc[0].width>1920)?2:1);
    //gsf_rgn_nk_set(0, 0, &rects, (codec_nvr.venc[0].width>1920)?2:1);
    
  }

  return 0;
}

static gsf_venc_ini_t *p_venc_ini = NULL;
static gsf_rgn_ini_t *p_rgn_ini = NULL;

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
    if((!codec_nvr.venc[j].en) || (j >= p_venc_ini->st_num && j != GSF_CODEC_SNAP_IDX))
      continue;
    
    gsf_mpp_venc_t venc = {
      .VencChn    = i*GSF_CODEC_VENC_NUM+j,
      .srcModId   = HI_ID_VPSS,
      .VpssGrp    = i, // grp;
      .VpssChn    = 0, // ch;
      .enPayLoad  = PT_VENC(codec_nvr.venc[j].type),
      .enSize     = PIC_WIDTH(codec_nvr.venc[j].width),
      .enRcMode   = SAMPLE_RC_CBR,
      .u32Profile = 0,
      .bRcnRefShareBuf = HI_TRUE,
      .enGopMode  = VENC_GOPMODE_NORMALP,
      .u32FrameRate = codec_nvr.venc[j].fps,
      .u32Gop       = codec_nvr.venc[j].gop,
      .u32BitRate   = codec_nvr.venc[j].bitrate,
      };
    
    if(!start)
    {
      ret = gsf_mpp_venc_stop(&venc);
      printf("stop >>> VencChn:%d, ch:%d, st:%d, width:%d, ret:%d\n"
            , venc.VencChn, i, j, codec_nvr.venc[j].width, ret);
    }
    else
    {
      ret = gsf_mpp_venc_start(&venc);
      printf("start >>> VencChn:%d, ch:%d, st:%d, width:%d, ret:%d\n"
            , venc.VencChn, i, j, codec_nvr.venc[j].width, ret);
    }
    
    if(!start)
      continue;  
    
    if(j < 2) // st_num+1(JPEG);
    {
      st.VeChn[st.s32Cnt] = venc.VencChn;
      st.s32Cnt++;
    }
  }
  
  if(!start)
    return 0;

  // recv start;
  printf("start >>> gsf_mpp_venc_recv s32Cnt:%d, cb:%p\n", st.s32Cnt, st.cb);
  gsf_mpp_venc_recv(&st);
  return 0;
}





int main(int argc, char *argv[])
{
    gsf_bsp_def_t bsp_def;  
    
    if(argc < 2)
    {
      printf("pls input: %s codec_parm.json\n", argv[0]);
      return -1;
    }
    if(json_parm_load(argv[1], &codec_nvr) < 0)
    {
      json_parm_save(argv[1], &codec_nvr);
      json_parm_load(argv[1], &codec_nvr);
    }
    
    if(1)
    {
      //get bsp_def;
      GSF_MSG_DEF(gsf_msg_t, msg, 4*1024);
      int ret = GSF_MSG_SENDTO(GSF_ID_BSP_DEF, 0, GET, 0, 0, GSF_IPC_BSP, 2000);
      gsf_bsp_def_t *def = (gsf_bsp_def_t*)__pmsg->data;
      printf("GET GSF_ID_BSP_DEF To:%s, ret:%d, model:%s\n", GSF_IPC_BSP, ret, def->board.model);
      if(ret < 0)
        return 0;
        
      bsp_def = *def;
    }
    
    #warning "...... @@@@@@@@ GSF_DEV_NVR @@@@@@@@ ......"
    printf("\n...... @@@@@@@@ GSF_DEV_NVR @@@@@@@@ ......\n");
    
    char home_path[256] = {0};
    proc_absolute_path(home_path);
    sprintf(home_path, "%s/../", home_path);
    printf("home_path:[%s]\n", home_path);
    
    int mipi_800x1280 = codec_nvr.vo.intf;
    int sync = codec_nvr.vo.sync?VO_OUTPUT_3840x2160_30:VO_OUTPUT_1080P60;
    
    //mpp_cfg;
    gsf_mpp_cfg_t cfg = {0};
    cfg.res = (sync == VO_OUTPUT_1080P60)?2:8;
    gsf_mpp_cfg(home_path, &cfg);
    
    #if defined(GSF_CPU_3559a) || defined(GSF_CPU_3403)
    {
      //gsf_mpp_vo_start(VODEV_HD0, VO_INTF_VGA|VO_INTF_HDMI, sync, 0);
      gsf_mpp_vo_start(VODEV_HD0, VO_INTF_HDMI, sync, 0);
      gsf_mpp_fb_start(VOFB_GUI, sync, 0);
      //gsf_mpp_vo_layout(VOLAYER_HD0, VO_LAYOUT_1MUX, NULL);
      //vo_ly_set(VO_LAYOUT_1MUX);
      
      if(sync == VO_OUTPUT_1080P60)
        vo_res_set(1920, 1080);
      else
        vo_res_set(3840, 2160);
          
      gsf_rgn_ini_t rgn_ini = {.ch_num = 9, .st_num = 1};
      gsf_venc_ini_t venc_ini = {.ch_num = 9, .st_num = 1};
      p_venc_ini = &venc_ini;
      p_rgn_ini  = &rgn_ini;
    }
    #else
    {
      if(mipi_800x1280)
      {
        gsf_mpp_vo_start(VODEV_HD0, VO_INTF_MIPI, VO_OUTPUT_USER, 0);
        gsf_mpp_fb_start(VOFB_GUI, VO_OUTPUT_USER, 0);
        
        //---- 800x640 -----//
        //---- 800x640 -----//
        gsf_mpp_vo_layout(VOLAYER_HD0, VO_LAYOUT_2MUX, NULL);
        vo_ly_set(VO_LAYOUT_2MUX);
        vo_res_set(800, 1280);
      }
      else
      {
        gsf_mpp_vo_start(VODEV_HD0, VO_INTF_HDMI, sync, 0);
        gsf_mpp_fb_start(VOFB_GUI, sync, 0);
        
        gsf_mpp_vo_layout(VOLAYER_HD0, VO_LAYOUT_1MUX, NULL);
        vo_ly_set(VO_LAYOUT_1MUX);
        
        if(sync == VO_OUTPUT_1080P60)
          vo_res_set(1920, 1080);
        else
          vo_res_set(3840, 2160);
      }

      gsf_rgn_ini_t rgn_ini = {.ch_num = 2, .st_num = 1};
      gsf_venc_ini_t venc_ini = {.ch_num = 2, .st_num = 1};
      p_venc_ini = &venc_ini;
      p_rgn_ini  = &rgn_ini;
    }
    #endif
  
    //start live;
    live_mon();
  
    //internal-init rgn, venc;
    gsf_rgn_init(p_rgn_ini);
    gsf_venc_init(p_venc_ini);
  
    //vdec => vpss => venc;
    venc_start(1);
    
    // regtister;
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
    
    GSF_LOG_CONN(0, 100);
    void* rep = nm_rep_listen(GSF_IPC_CODEC, NM_REP_MAX_WORKERS, NM_REP_OSIZE_MAX, req_recv);
    
    void* sub = nm_sub_conn(GSF_PUB_SVP, sub_recv);
    printf("nm_sub_conn sub:%p\n", sub);
    
    while(1)
    {
      sleep(1);
    }
    
    gsf_mpp_fb_hide(VOFB_GUI, 1);
    gsf_mpp_vo_layout(VOLAYER_HD0, VO_LAYOUT_NONE, NULL);
    gsf_mpp_vo_stop(VODEV_HD0);
    
    GSF_LOG_DISCONN();
    return 0;
}