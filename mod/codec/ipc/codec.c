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
#include "rgn.h"
#include "venc.h"
#include "lens.h"

#define XYKJ 0

#define AVS_4CH_3559a 0  //  1: 4 sensor => avs => 1 venc; 0: 4 sensor => 4 venc; 
#define AVS_2CH_3516d 0  //  1: 2 sensor => vo => 1 venc;  0: 2 sensor => 2 venc;
#define SECOND_CHANNEL XYKJ // second channel;

#ifndef PIC_VGA
#define PIC_VGA PIC_CIF
#endif
#ifndef PIC_7680x4320
#define PIC_7680x4320 PIC_3840x2160
#endif



#define PIC_WIDTH(w, h) \
            (w >= 7680)?PIC_7680x4320:\
            (w >= 3840)?PIC_3840x2160:\
            (w >= 2592 && h >= 1944)?PIC_2592x1944:\
            (w >= 2592 && h >= 1536)?PIC_2592x1536:\
            (w >= 1920)?PIC_1080P:\
            (w >= 1280)?PIC_720P: \
            (w >= 720 && h >= 576)?PIC_D1_PAL: \
            (w >= 720 && h >= 480)?PIC_D1_NTSC: \
            PIC_VGA
            
#define PT_VENC(t) \
            (t == GSF_ENC_H264)? PT_H264:\
            (t == GSF_ENC_H265)? PT_H265:\
            (t == GSF_ENC_JPEG)? PT_JPEG:\
            (t == GSF_ENC_MJPEG)? PT_MJPEG:\
            PT_H264

#define PIC_WH(e,w,h) \
            if(e == PIC_7680x4320){ w = 7680; h = 4320;}\
            else if(e == PIC_3840x2160){ w = 3840; h = 2160;}\
            else if (e == PIC_3840x2160){ w = 3840; h = 2160;}\
            else if (e == PIC_2592x1944){ w = 2592; h = 1944;}\
            else if (e == PIC_2592x1536){ w = 2592; h = 1536;}\
            else if (e == PIC_1080P){ w = 1920; h = 1080;}\
            else if (e == PIC_720P){ w = 1280; h = 720;}\
            else if (e == PIC_D1_PAL){ w = 720; h = 576;}\
            else if (e == PIC_D1_NTSC){ w = 720; h = 480;}\
            else { w = 352; h = 288;}


GSF_LOG_GLOBAL_INIT("CODEC", 8*1024);


static gsf_resolu_t vores;
#define vo_res_set(_w, _h) do{vores.w = _w; vores.h = _h;}while(0)
int vo_res_get(gsf_resolu_t *res)
{
  *res = vores;
  return 0;
}

static gsf_layout_t voly;
#define vo_ly_set(ly) do{voly.layout = ly;}while(0)
int vo_ly_get(gsf_layout_t *ly)
{
  *ly = voly;
  return 0;
}

static gsf_venc_ini_t *p_venc_ini = NULL;
static gsf_mpp_cfg_t  *p_cfg = NULL;

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

    return 0;
}

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
      osd.point[0] = (unsigned int)((float)mds->result[i].rect[0]/(float)mds->w*1920)&(-1);
      osd.point[1] = (unsigned int)((float)mds->result[i].rect[1]/(float)mds->h*1080)&(-1);
      osd.wh[0]    = (unsigned int)((float)mds->result[i].rect[2]/(float)mds->w*1920)&(-1);
      osd.wh[1]    = (unsigned int)((float)mds->result[i].rect[3]/(float)mds->h*1080)&(-1);
      
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

      osd.point[0] = 0;//(unsigned int)((float)lprs->result[i].rect[0]/(float)lprs->w*1920)&(-1);
      osd.point[1] = 800+i*100;//(unsigned int)((float)lprs->result[i].rect[1]/(float)lprs->h*1080)&(-1);
      osd.wh[0]    = (unsigned int)((float)lprs->result[i].rect[2]/(float)lprs->w*1920)&(-1);
      osd.wh[1]    = (unsigned int)((float)lprs->result[i].rect[3]/(float)lprs->h*1080)&(-1);
      
      char utf8str[32] = {0};
      gsf_gb2312_to_utf8(lprs->result[i].number, strlen(lprs->result[i].number), utf8str);
      sprintf(osd.text, "%s", utf8str);
      
      printf("GSF_EV_SVP_LPR idx: %d, osd: rect: [%d,%d,%d,%d], utf8:[%s]\n"
            , i, osd.point[0], osd.point[1], osd.wh[0], osd.wh[1], osd.text);
            
      gsf_rgn_osd_set(0, i, &osd);
    }
  }
  else if(pmsg->id == GSF_EV_SVP_YOLO)
  {
    gsf_svp_yolos_t *yolos = (gsf_svp_yolos_t*) pmsg->data;
    
    int i = 0;
    gsf_rgn_rects_t rects = {0};

    #if(AVS_2CH_3516d == 1)
    float xr = 0 + pmsg->ch*(yolos->w/2.0), yr = yolos->h/4.0, wr = 2.0, hr = 2.0;
    int chn = 0;
    #else
    float xr = 0, yr = 0, wr = 1, hr = 1;
    int chn = pmsg->ch;
    #endif

    rects.size = yolos->cnt;
    rects.w = yolos->w;
    rects.h = yolos->h;
    
    int j = 0;
    for(i = 0; i < yolos->cnt; i++)
    {
      //person filter;
      #if XYKJ
      if(!strstr(yolos->box[i].label, "person"))
        continue;
      #endif
        
      rects.box[j].rect[0] = xr + yolos->box[i].rect[0]/wr;
      rects.box[j].rect[1] = yr + yolos->box[i].rect[1]/hr;
      rects.box[j].rect[2] = yolos->box[i].rect[2]/wr;
      rects.box[j].rect[3] = yolos->box[i].rect[3]/hr;
      gsf_gb2312_to_utf8(yolos->box[i].label, strlen(yolos->box[i].label), rects.box[j].label);
      
      j++;
    } rects.size = j;
    
    
    int width  = codec_ipc.venc[0].width;
    int height = codec_ipc.venc[0].height;
    
    #if defined(GSF_CPU_3516d)
    if(chn && p_cfg->second)
    {
      //second osd;
      codec_ipc.venc[0].width = 720;
      codec_ipc.venc[0].height = 576;
    }
    #endif
    
    // osd to sub-stream if main-stream > 1080P;
    //printf("chn:%d, rects.size:%d\n", chn, rects.size);
    gsf_rgn_rect_set(chn, 0, &rects, (codec_ipc.venc[0].width>1920)?2:1);
    //gsf_rgn_nk_set(0, 0, &rects, (codec_ipc.venc[0].width>1920)?2:1);

    #if defined(GSF_CPU_3516d)
    if(chn && p_cfg->second)
    {
      //second osd;
      codec_ipc.venc[0].width = width;
      codec_ipc.venc[0].height = height;
    }
    #endif
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
      .enRcMode   = SAMPLE_RC_CBR,
      .u32Profile = 0,
      .bRcnRefShareBuf = HI_TRUE,
      .enGopMode  = VENC_GOPMODE_NORMALP,
      .u32FrameRate = codec_ipc.venc[j].fps,
      .u32Gop       = codec_ipc.venc[j].gop,
      .u32BitRate   = codec_ipc.venc[j].bitrate,
    };

    #if defined(GSF_CPU_3559a) && (AVS_4CH_3559a == 1)
    venc.srcModId   = HI_ID_AVS;
    venc.AVSGrp     = i;
    venc.AVSChn    = (j<p_venc_ini->st_num)?j:0;
    #elif defined(GSF_CPU_3516d) && (AVS_2CH_3516d == 1)
    venc.srcModId   = HI_ID_VO;
    venc.VpssGrp    = -1;
    venc.VpssChn    = -1;
    #endif

    //only 3516D supported 
    #if defined(GSF_CPU_3516d)
    if(p_cfg->second && i == 1)
    {
      if(j == 0 || j == 2)
        venc.enSize = PIC_WIDTH(720, 576);
      else
        venc.enSize = PIC_WIDTH(352, 288);
    }
    #endif

    int w = 0, h = 0;
    PIC_WH(venc.enSize, w, h);
      
    if(!start)
    {
      ret = gsf_mpp_venc_stop(&venc);
      printf("stop >>> ch:%d, st:%d, [%dx%d], ret:%d\n"
            , i, j, w, h, ret);
    }
    else
    {
      ret = gsf_mpp_venc_start(&venc);
      printf("start >>> ch:%d, st:%d, [%dx%d], ret:%d\n"
            , i, j, w, h, ret);
    }
    
    if(!start)
      continue;
    
    if(j < p_venc_ini->st_num) // st_num+1(JPEG);
    {
      st.VeChn[st.s32Cnt] = venc.VencChn;
      st.s32Cnt++;
    }
    
    // refresh rgn for st_num+1(JPEG);
    for(k = 0; k < GSF_CODEC_OSD_NUM; k++)
      gsf_rgn_osd_set(i, k, &codec_ipc.osd[k]);
      
    for(k = 0; k < GSF_CODEC_VMASK_NUM; k++)
      gsf_rgn_vmask_set(i, k, &codec_ipc.vmask[k]);
  }
  
  if(!start)
    return 0;
    
  // recv start;
  printf("start >>> gsf_mpp_venc_recv s32Cnt:%d, cb:%p\n", st.s32Cnt, st.cb);
  gsf_mpp_venc_recv(&st);
  return 0;
}


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
    
    #define VPSS(_i, _g, _p, _ch, _en0, _en1, _sz0, _sz1) do{ \
      vpss[_i].VpssGrp=_g; vpss[_i].ViPipe=_p; vpss[_i].ViChn=_ch;\
      vpss[_i].enable[0]=_en0;vpss[_i].enable[1]=_en1;\
      vpss[_i].enSize[0]=_sz0;vpss[_i].enSize[1]=_sz1;}while(0)
    
    //only used sensor[0];
    strcpy(cfg.snsname, def->board.sensor[0]);
    cfg.snscnt = def->board.snscnt;

    do{
      #ifdef GSF_CPU_3559a
      {
        #if (AVS_4CH_3559a == 1)
        {
          // imx334-0-0-8-30
          cfg.lane = 0; cfg.wdr = 0; cfg.res = 8; cfg.fps = 30;
          rgn_ini.ch_num = 1; rgn_ini.st_num = 1;
          venc_ini.ch_num = 1; venc_ini.st_num = 1;
          VPSS(0, 0, 0, 0, 1, 0, PIC_1080P, PIC_720P);
          VPSS(1, 1, 1, 0, 1, 0, PIC_1080P, PIC_720P);
          VPSS(2, 2, 2, 0, 1, 0, PIC_1080P, PIC_720P);
          VPSS(3, 3, 3, 0, 1, 0, PIC_1080P, PIC_720P);   
        }
        #else
        {
          if(strstr(cfg.snsname, "sharp8k"))
          {
            //sharp8k-0-0-16-30
            cfg.lane = 0; cfg.wdr = 0; cfg.res = 16; cfg.fps = 30;
            rgn_ini.ch_num = 1; rgn_ini.st_num = 2;
            venc_ini.ch_num = 1; venc_ini.st_num = 2;
            VPSS(0, 0, 0, 0, 1, 1, PIC_7680x4320, PIC_3840x2160);
          }
          else
          {
            // imx334-0-0-8-30
            cfg.lane = 0; cfg.wdr = 0; cfg.res = 8; cfg.fps = 30;
            rgn_ini.ch_num = 4; rgn_ini.st_num = 2;
            venc_ini.ch_num = 4; venc_ini.st_num = 2;
            VPSS(0, 0, 0, 0, 1, 1, PIC_3840x2160, PIC_720P);
            VPSS(1, 1, 1, 0, 1, 1, PIC_3840x2160, PIC_720P);
            VPSS(2, 2, 2, 0, 1, 1, PIC_3840x2160, PIC_720P);
            VPSS(3, 3, 3, 0, 1, 1, PIC_3840x2160, PIC_720P);
          }

            
        }
        #endif

      }
      #elif defined(GSF_CPU_3519a)
      {
        // imx334-0-0-8-60
        cfg.lane = 0; cfg.wdr = 0; cfg.res = 8; cfg.fps = 30;
        rgn_ini.ch_num = 1; rgn_ini.st_num = 2;
        venc_ini.ch_num = 1; venc_ini.st_num = 2;
        VPSS(0, 0, 0, 0, 1, 1, PIC_3840x2160, PIC_720P);
      }
      #elif defined(GSF_CPU_3516d)
      {
        //cpu type; 
        strcpy(cfg.type, def->board.type);
        
        //second channel;
        cfg.second = (cfg.snscnt > 1)?0:SECOND_CHANNEL;

        #if(AVS_2CH_3516d == 1)
        {
          // imx335-0-0-4-30
          if(strstr(cfg.snsname, "imx327") 
            || strstr(cfg.snsname, "imx290"))
          {
            cfg.lane = (cfg.second)?0:2; cfg.wdr = 0; cfg.res = 2; cfg.fps = 30;
            rgn_ini.ch_num = 1; rgn_ini.st_num = 2;
            venc_ini.ch_num = 1; venc_ini.st_num = 2;
            VPSS(0, 0, 0, 0, 1, 1, PIC_1080P, PIC_720P);
            
			      VPSS(1, 1, 1, 0, 1, 1, PIC_1080P, PIC_720P);
            if(cfg.second)
				      VPSS(1, 1, 1, 0, 1, 1, PIC_D1_PAL, PIC_CIF);
          }
          else if(strstr(cfg.snsname, "imx335"))
          {
            cfg.lane = (cfg.second)?0:2; cfg.wdr = 0; cfg.res = 4; cfg.fps = 30;
            rgn_ini.ch_num = 1; rgn_ini.st_num = 2;
            venc_ini.ch_num = 1; venc_ini.st_num = 2;
            VPSS(0, 0, 0, 0, 1, 1, PIC_2592x1536, PIC_1080P);
            if(cfg.second)
            	VPSS(1, 1, 1, 0, 1, 1, PIC_D1_PAL, PIC_CIF);
          }
          break;
        }
        #endif
        
        
        if(cfg.snscnt < 2)
        {
          if(strstr(cfg.snsname, "imx327") 
            || strstr(cfg.snsname, "imx290"))
          {
          	// imx327-0-0-2-30
            cfg.lane = 0; cfg.wdr = 0; cfg.res = 2; cfg.fps = 30;
            rgn_ini.ch_num = 1; rgn_ini.st_num = 2;
            venc_ini.ch_num = 1; venc_ini.st_num = 2;
            VPSS(0, 0, 0, 0, 1, 1, PIC_1080P, PIC_720P);
            
            if(cfg.second)
            {
              rgn_ini.ch_num = venc_ini.ch_num = 2;
              VPSS(1, 1, 1, 0, 1, 1, PIC_D1_PAL, PIC_CIF);
            }
          }
          else if(strstr(cfg.snsname, "imx415"))
          {
              if(strstr(cfg.type, "3516AV300"))
              {
                cfg.lane = 0; cfg.wdr = 0; cfg.res = 8; cfg.fps = 30;
                rgn_ini.ch_num = 1; rgn_ini.st_num = 2;
                venc_ini.ch_num = 1; venc_ini.st_num = 2;
                VPSS(0, 0, 0, 0, 1, 1, PIC_3840x2160, PIC_1080P);
              }
              else
              {
                cfg.lane = 0; cfg.wdr = 0; cfg.res = 2; cfg.fps = 60;
                rgn_ini.ch_num = 1; rgn_ini.st_num = 2;
                venc_ini.ch_num = 1; venc_ini.st_num = 2;
                VPSS(0, 0, 0, 0, 1, 1, PIC_1080P, PIC_D1_NTSC);
              }
          }
          else if(strstr(cfg.snsname, "imx335"))
          {
            // imx335-0-0-4-30
            cfg.lane = 0; cfg.wdr = 0; cfg.res = 4; cfg.fps = 30;
            rgn_ini.ch_num = 1; rgn_ini.st_num = 2;
            venc_ini.ch_num = 1; venc_ini.st_num = 2;
            VPSS(0, 0, 0, 0, 1, 1, PIC_2592x1536, PIC_1080P);
            if(cfg.second)
            {
              rgn_ini.ch_num = venc_ini.ch_num = 2;
              VPSS(1, 1, 1, 0, 1, 1, PIC_D1_PAL, PIC_CIF);
            }
          }
          else if(strstr(cfg.snsname, "yuv422"))
          {
            // yuv422-0-0-8-30
            if(strstr(cfg.type, "3516AV300"))
            {
              cfg.lane = 0; cfg.wdr = 0; cfg.res = 8; cfg.fps = 30;
              rgn_ini.ch_num = 1; rgn_ini.st_num = 2;
              venc_ini.ch_num = 1; venc_ini.st_num = 2;
              VPSS(0, 0, 0, 0, 1, 1, PIC_3840x2160, PIC_1080P);
            }
            else
            {
              // yuv422-0-0-2-60
              cfg.lane = 0; cfg.wdr = 0; cfg.res = 2; cfg.fps = 60;
              rgn_ini.ch_num = 1; rgn_ini.st_num = 2;
              venc_ini.ch_num = 1; venc_ini.st_num = 2;
              VPSS(0, 0, 0, 0, 1, 1, PIC_1080P, PIC_D1_NTSC);
            }
          }
          else
          {
            cfg.lane = 0; cfg.wdr = 0; cfg.res = 2; cfg.fps = 60;
            rgn_ini.ch_num = 1; rgn_ini.st_num = 2;
            venc_ini.ch_num = 1; venc_ini.st_num = 2;
            VPSS(0, 0, 0, 0, 1, 1, PIC_1080P, PIC_D1_NTSC);
          }
        }
        else
        {
          // imx327-2-0-2-30
          cfg.lane = 2; cfg.wdr = 0; cfg.res = 2; cfg.fps = 30;
          rgn_ini.ch_num = 2; rgn_ini.st_num = 2;
          venc_ini.ch_num = 2; venc_ini.st_num = 2;
          VPSS(0, 0, 0, 0, 1, 1, PIC_1080P, PIC_D1_NTSC);
          VPSS(1, 1, 1, 0, 1, 1, PIC_1080P, PIC_D1_NTSC);
        }
      }
      #elif defined(GSF_CPU_3516e)
      {
        // sc2335-0-0-2-30
        cfg.lane = 0; cfg.wdr = 0; cfg.res = 2; cfg.fps = 30;
        rgn_ini.ch_num = 1; rgn_ini.st_num = 2;
        venc_ini.ch_num = 1; venc_ini.st_num = 2;
        VPSS(0, 0, 0, 0, 1, 1, PIC_1080P, PIC_720P); 
      }
      #elif defined(GSF_CPU_3519)
      {
        // imx334-0-0-8-30
        cfg.lane = 0; cfg.wdr = 0; cfg.res = 8; cfg.fps = 30;
        rgn_ini.ch_num = 2; rgn_ini.st_num = 1;
        venc_ini.ch_num = 2; venc_ini.st_num = 1;
        VPSS(0, 0, 0, 0, 1, 0, PIC_3840x2160, PIC_720P);
        VPSS(1, 1, 1, 0, 1, 0, PIC_1080P, PIC_720P);
      }
      #elif defined(GSF_CPU_3559)
      {
        #if 0
        // yuv422-0-0-2-60
        cfg.lane = 0; cfg.wdr = 0; cfg.res = 2; cfg.fps = 60;
        rgn_ini.ch_num = 1; rgn_ini.st_num = 2;
        venc_ini.ch_num = 1; venc_ini.st_num = 2;
        VPSS(0, 0, 0, 0, 1, 1, PIC_1080P, PIC_D1_NTSC);
        break;
        #endif
        
        #if 1
        // imx415-0-0-8-30
        cfg.lane = 0; cfg.wdr = 0; cfg.res = 8; cfg.fps = 30;
        rgn_ini.ch_num = 1; rgn_ini.st_num = 2;
        venc_ini.ch_num = 1; venc_ini.st_num = 2;
        VPSS(0, 0, 0, 0, 1, 1, PIC_3840x2160, PIC_1080P);
        break;
        #endif
        
        #if 0
        // imx335-0-0-4-30
        cfg.lane = 0; cfg.wdr = 0; cfg.res = 4; cfg.fps = 30;
        rgn_ini.ch_num = 1; rgn_ini.st_num = 2;
        venc_ini.ch_num = 1; venc_ini.st_num = 2;
        VPSS(0, 0, 0, 0, 1, 1, PIC_2592x1536, PIC_1080P);
        break;
        #endif
      }
      
      #elif defined(GSF_CPU_3531d)
      {
        // 
        //cfg.lane = 0; cfg.wdr = 0; cfg.res = 8; cfg.fps = 30;
        rgn_ini.ch_num = 2; rgn_ini.st_num = 1;
        venc_ini.ch_num = 2; venc_ini.st_num = 1;
        VPSS(0, 0, 0, 0, 1, 0, PIC_3840x2160, PIC_720P);
        VPSS(1, 1, 1, 0, 1, 0, PIC_1080P, PIC_720P);
      }
      #else
      {
        #error "error unknow gsf_mpp_cfg_t."
      }
      #endif
    }while(0);
    
    p_venc_ini = &venc_ini;
    p_cfg      = &cfg;
    
    char home_path[256] = {0};
    proc_absolute_path(home_path);
    sprintf(home_path, "%s/../", home_path);
    printf("home_path:[%s]\n", home_path);
   
    gsf_mpp_cfg(home_path, &cfg);
    
    //internal-init rgn, venc;
    gsf_rgn_init(&rgn_ini);
    gsf_venc_init(&venc_ini);
    
    lens_ini.ch_num = 1;  // lens number;
    strncpy(lens_ini.sns, cfg.snsname, sizeof(lens_ini.sns)-1);
    strncpy(lens_ini.lens, "LENS-NAME", sizeof(lens_ini.lens)-1);
    gsf_lens_init(&lens_ini);

    // vi start;
    #if defined(GSF_CPU_3559a) && (AVS_4CH_3559a == 1)
    gsf_mpp_vi_t vi = {
        .bLowDelay = HI_FALSE,
        .u32SupplementConfig = 0,
        .stStitchGrpAttr = {
          .bStitch = HI_TRUE,
          .enMode  = VI_STITCH_ISP_CFG_NORMAL,
          .u32PipeNum = 4,
          .PipeId[0] = 0,
          .PipeId[1] = 1,
          .PipeId[2] = 2,
          .PipeId[3] = 3,
        }
    };
    #else
    gsf_mpp_vi_t vi = {
        .bLowDelay = HI_FALSE,
        .u32SupplementConfig = 0,
        #ifdef GSF_CPU_3516e
        .vpss_en = {1, 1,},
        .vpss_sz = {PIC_1080P, PIC_720P,},
        #endif
    };
    #endif
    
    gsf_mpp_vi_start(&vi);
    
    //start af;
    gsf_lens_af_start(0);
    
    // vpss start;
    #if defined(GSF_CPU_3559a) && (AVS_4CH_3559a == 1)
    for(i = 0; i < 4; i++)
    #elif defined(GSF_CPU_3516d) && (AVS_2CH_3516d == 1)
    for(i = 0; i < 2; i++)
    #else
    for(i = 0; i < venc_ini.ch_num; i++)
    #endif
    {
      gsf_mpp_vpss_start(&vpss[i]);
    }
    
    // scene start;
    char scene_ini[128] = {0};
    proc_absolute_path(scene_ini);
    
    #if defined(GSF_CPU_3559a) && (AVS_4CH_3559a == 1)
    sprintf(scene_ini, "%s/../cfg/%savs.ini", scene_ini, cfg.snsname);
    #else
    sprintf(scene_ini, "%s/../cfg/%s.ini", scene_ini, cfg.snsname);
    #endif
    gsf_mpp_scene_start(scene_ini, 0);

    #if defined(GSF_CPU_3559a) && (AVS_4CH_3559a == 1)
    gsf_mpp_avs_t avs = {
      .AVSGrp = 0,
      .u32OutW = 1920*4,
      .u32OutH = 1080,
      .u32PipeNum = 4,
      .enMode = AVS_MODE_NOBLEND_HOR,
      .benChn1 = 0,
    };
    gsf_mpp_avs_start(&avs);
    #endif
    
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
    
    #if defined(GSF_CPU_3516d) || defined(GSF_CPU_3559)
	
    //aenc;
    gsf_mpp_aenc_t aenc = {
      .AeChn = 0,
      .enPayLoad = PT_AAC,
      .uargs = NULL,
      .cb = gsf_aenc_recv,
    };
    gsf_mpp_audio_start(&aenc);
    //gsf_mpp_audio_start(NULL);
    
    // test vo;
    //int mipi_800x1280 = (access("/app/mipi", 0)!= -1)?1:0;
    int mipi_800x1280 = codec_ipc.vo.intf;

    if(mipi_800x1280)
    {
      gsf_mpp_vo_start(VODEV_HD0, VO_INTF_MIPI, VO_OUTPUT_USER, 0);
      gsf_mpp_vo_layout(VOLAYER_HD0, VO_LAYOUT_2MUX, NULL);
      gsf_mpp_fb_start(VOFB_GUI, VO_OUTPUT_USER, 0);
      
      //---- 800x640 -----//
      //---- 800x640 -----//
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
      int sync = codec_ipc.vo.sync?VO_OUTPUT_3840x2160_30:VO_OUTPUT_1080P60;
      //gsf_mpp_vo_start(VODEV_HD0, VO_INTF_HDMI, VO_OUTPUT_1080P60, 0); // 10
      
      gsf_mpp_vo_start(VODEV_HD0, VO_INTF_HDMI, sync, 0);
      
      int ly = VO_LAYOUT_1MUX;
      
      #if defined(GSF_CPU_3516d)
        if(p_cfg->second || p_cfg->snscnt > 1)
          ly = VO_LAYOUT_2MUX;
          
        #if(AVS_2CH_3516d == 1)
        ly = VO_LAYOUT_2MUX;
        #endif
      #endif
      
      gsf_mpp_vo_layout(VOLAYER_HD0, ly, NULL);
      vo_ly_set(ly);
      
      gsf_mpp_fb_start(VOFB_GUI, sync, 0);
      
      gsf_mpp_vo_src_t src0 = {0, 0};
      gsf_mpp_vo_bind(VOLAYER_HD0, 0, &src0);
      
      #if defined(GSF_CPU_3516d)
      if(ly == VO_LAYOUT_2MUX)
      {
        if(p_cfg->second)
    	  {
          RECT_S rect = {20, 0, 720, 576};
          gsf_mpp_vo_aspect(VOLAYER_HD0, 1, &rect);
        }
        gsf_mpp_vo_src_t src1 = {1, 0};
        gsf_mpp_vo_bind(VOLAYER_HD0, 1, &src1);
      }
      #endif
      
      if(sync == VO_OUTPUT_1080P60)
        vo_res_set(1920, 1080);
      else
        vo_res_set(3840, 2160);
    }

    gsf_mpp_ao_bind(SAMPLE_AUDIO_INNER_AO_DEV, 0, SAMPLE_AUDIO_INNER_AI_DEV, 0);
    gsf_mpp_ao_bind(SAMPLE_AUDIO_INNER_HDMI_AO_DEV, 0, SAMPLE_AUDIO_INNER_AI_DEV, 0);
  
    #endif

    //init listen;
    void* rep = nm_rep_listen(GSF_IPC_CODEC
                      , NM_REP_MAX_WORKERS
                      , NM_REP_OSIZE_MAX
                      , req_recv);
    
    void* sub = nm_sub_conn(GSF_PUB_SVP, sub_recv);
    printf("nm_sub_conn sub:%p\n", sub);
    
    while(1)
    {
      sleep(1);
    }
      
    GSF_LOG_DISCONN();
    return 0;
}