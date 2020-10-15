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



#define VI_2CH_3516d  0  //  1: 2 sensor input; 0: 1 sensor input;
#define AVS_4CH_3559a 0  //  1: 4 sensor => avs => 1 venc; 0: 4 sensor => 4 venc; 

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


/////////////////////////////////////

static int osd_keepalive[GSF_CODEC_OSD_NUM] = {3,3,3,3,3,3,3,3};

static int sub_recv(char *msg, int size, int err)
{
  gsf_msg_t *pmsg = (gsf_msg_t*)msg;
  
  printf("pmsg->id:%d\n", pmsg->id);
  
  if(pmsg->id == GSF_EV_SVP_MD)
  {
    gsf_svp_mds_t *mds = (gsf_svp_mds_t*) pmsg->data;
    
    int i = 0;
    for(i = 0; i < 4 && i < mds->cnt; i++)
    {
      
      osd_keepalive[i] = 3;
      
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
      
      osd_keepalive[i] = 3;
      
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

  return 0;
}

static int rgn_timer_func(void *u)
{

#if 0

  int i = 0;
  for(i = 0; i < GSF_CODEC_OSD_NUM; i++)
  {
    if(--osd_keepalive[i] > 0)
      continue;
    
    gsf_osd_t osd;
    memset(&osd, 0, sizeof(gsf_osd_t));          
    gsf_rgn_osd_set(0, i, &osd);
  }
  return 0;
  
#else
  
  static gsf_osd_t osd;
  if(!osd.en)
  {
    osd.en = 1;
    osd.type = 0;
    osd.fontsize = 0;
    osd.point[0] = 10;
    osd.point[1] = 10;
    osd.wh[0] = 100;
    osd.wh[1] = 300;
  }
  static gsf_vmask_t vmask;
  if(!vmask.en)
  {
    vmask.en = 1;
    vmask.color = 0x0000ffff;
    vmask.rect[0] = 100;
    vmask.rect[1] = 100;
    vmask.rect[2] = 200;
    vmask.rect[3] = 200;
  }
  
  time_t rawtime;
  time(&rawtime);
  struct tm *info = localtime(&rawtime);
  strftime(osd.text, 80, "%Y-%m-%d %H:%M:%S", info);
  
  osd.point[0] += 100;
  osd.point[0] %= 2000;
  osd.point[1] += 100;
  osd.point[1] %= 2000;
  printf("osd: x:%d,y:%d\n", osd.point[0], osd.point[1]);
  
  struct timespec ts1, ts2;  
  clock_gettime(CLOCK_MONOTONIC, &ts1);
  
  gsf_rgn_osd_set(0, 0, &osd);
  
  clock_gettime(CLOCK_MONOTONIC, &ts2);
  int cost = (ts2.tv_sec*1000 + ts2.tv_nsec/1000000) - (ts1.tv_sec*1000 + ts1.tv_nsec/1000000);
  printf("gsf_rgn_osd_set cost:%d ms\n", cost);
  
  vmask.rect[0] += 100;
  vmask.rect[0] %= 2000;
  vmask.rect[1] += 100;
  vmask.rect[1] %= 2000;
  printf("vmask: x:%d,y:%d,w:%d,h:%d\n", vmask.rect[0], vmask.rect[1], vmask.rect[2], vmask.rect[3]);
  gsf_rgn_vmask_set(0, 0, &vmask);
  
  return 0;
#endif
}

/////////////////////////////////////

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
  printf("GET GSF_ID_BSP_DEF To:%s, ret:%d, model:%s\n", GSF_IPC_BSP, ret, cfg->board.model);

  if(ret < 0)
    return ret;

  if(def) 
    *def = *cfg;
    
  return ret;
}

static gsf_venc_ini_t *p_venc_ini = NULL;

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
      
#if defined(GSF_CPU_3559a) && (AVS_4CH_3559a == 1)
      .srcModId   = HI_ID_AVS,
      .AVSGrp     = i,
      .AVSChn    = (j<p_venc_ini->st_num)?j:0,
#else
      .srcModId   = HI_ID_VPSS,
      .VpssGrp    = i,
      .VpssChn    = (j<p_venc_ini->st_num)?j:0,
#endif
      .enPayLoad  = PT_VENC(codec_ipc.venc[j].type),
      .enSize     = PIC_WIDTH(codec_ipc.venc[j].width),
      .enRcMode   = SAMPLE_RC_CBR,
      .u32Profile = 0,
      .bRcnRefShareBuf = HI_TRUE,
      .enGopMode  = VENC_GOPMODE_NORMALP,
      .u32FrameRate = codec_ipc.venc[j].fps,
      .u32Gop       = codec_ipc.venc[j].gop,
      .u32BitRate   = codec_ipc.venc[j].bitrate,
    };
    
    if(!start)
    {
      ret = gsf_mpp_venc_stop(&venc);
      printf("stop >>> ch:%d, st:%d, width:%d, ret:%d\n", i, j, codec_ipc.venc[j].width, ret);
    }
    else
    {
      ret = gsf_mpp_venc_start(&venc);
      printf("start >>> ch:%d, st:%d, width:%d, ret:%d\n", i, j, codec_ipc.venc[j].width, ret);
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



int main(int argc, char *argv[])
{
    int i = 0;
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
    info("parm.venc[0].type:%d, width:%d\n", codec_ipc.venc[0].type, codec_ipc.venc[0].width);

    // register to bsp && get bsp_def;
    #if 1
    if(reg2bsp() < 0)
      return -1;

    if(getdef(&bsp_def) < 0)
      return -1;
    #else
    sprintf(bsp_def.board.sensor[0], "%s", "sc2335");
    #endif

    GSF_LOG_CONN(0, 100);
    
    // -------------------  vi => vpss => venc  -------------------- //
    // channel: CH0        CH1        CH2         CH3         CH4    //
    // vi:      pipe0      pipe1      pipe2       pipe3       ...    //
    // vpss:    G0[0,1]    G1[0,1]    G2[0,1]     G3[0,1]     ...    //
    // venc:    V0 V1 S2   V3 V4 S5   V6 V7 S8    V9 V10 S11  ...    //
    // ------------------------------------------------------------- //
    
    // init mpp;
    #ifdef GSF_CPU_3559a
    
      #if (AVS_4CH_3559a == 0)
      // imx334-0-0-8-30
      gsf_mpp_cfg_t cfg = { .snsname = "imx334", .snscnt = 4, .lane = 0, .wdr  = 0, .res  = 8, .fps  = 30, };
      gsf_rgn_ini_t rgn_ini = {.ch_num = 4, .st_num = 2};
      gsf_venc_ini_t venc_ini = {.ch_num = 4, .st_num = 2};
      gsf_mpp_vpss_t vpss[GSF_CODEC_IPC_CHN] = {
              {.VpssGrp = 0, .ViPipe = 0, .ViChn = 0, .enable = {1, 1,}, .enSize = {PIC_3840x2160, PIC_720P,}},
              {.VpssGrp = 1, .ViPipe = 1, .ViChn = 0, .enable = {1, 1,}, .enSize = {PIC_3840x2160, PIC_720P,}},
              {.VpssGrp = 2, .ViPipe = 2, .ViChn = 0, .enable = {1, 1,}, .enSize = {PIC_3840x2160, PIC_720P,}},
              {.VpssGrp = 3, .ViPipe = 3, .ViChn = 0, .enable = {1, 1,}, .enSize = {PIC_3840x2160, PIC_720P,}},
            };
      #else
      // imx334-0-0-8-30
      gsf_mpp_cfg_t cfg = { .snsname = "imx334", .snscnt = 4, .lane = 0, .wdr  = 0, .res  = 8, .fps  = 30, };
      gsf_rgn_ini_t rgn_ini = {.ch_num = 1, .st_num = 1};
      gsf_venc_ini_t venc_ini = {.ch_num = 1, .st_num = 1};
      gsf_mpp_vpss_t vpss[GSF_CODEC_IPC_CHN] = {
              {.VpssGrp = 0, .ViPipe = 0, .ViChn = 0, .enable = {1, 0,}, .enSize = {PIC_1080P, PIC_720P,}},
              {.VpssGrp = 1, .ViPipe = 1, .ViChn = 0, .enable = {1, 0,}, .enSize = {PIC_1080P, PIC_720P,}},
              {.VpssGrp = 2, .ViPipe = 2, .ViChn = 0, .enable = {1, 0,}, .enSize = {PIC_1080P, PIC_720P,}},
              {.VpssGrp = 3, .ViPipe = 3, .ViChn = 0, .enable = {1, 0,}, .enSize = {PIC_1080P, PIC_720P,}},
            };
      #endif


    #elif defined(GSF_CPU_3519a)
    // imx334-0-0-8-60
    gsf_mpp_cfg_t cfg = { .snsname = "imx334", .snscnt = 1, .lane = 0, .wdr  = 0, .res  = 8, .fps  = 30, };
    gsf_rgn_ini_t rgn_ini = {.ch_num = 1, .st_num = 2};
    gsf_venc_ini_t venc_ini = {.ch_num = 1, .st_num = 2};
    gsf_mpp_vpss_t vpss[GSF_CODEC_IPC_CHN] = { 
            {.VpssGrp = 0, .ViPipe = 0, .ViChn = 0, .enable = {1, 1,}, .enSize = {PIC_3840x2160, PIC_720P,}},
          };
    
    #elif defined(GSF_CPU_3516d)
    
      #if (VI_2CH_3516d == 0)
      // imx335-0-0-4-30
      gsf_mpp_cfg_t cfg = { .snsname = "imx335", .snscnt = 1, .lane = 0, .wdr  = 0, .res  = 4, .fps  = 30, };
      gsf_rgn_ini_t rgn_ini = {.ch_num = 1, .st_num = 2};
      gsf_venc_ini_t venc_ini = {.ch_num = 1, .st_num = 2};
      gsf_mpp_vpss_t vpss[GSF_CODEC_IPC_CHN] = {
            {.VpssGrp = 0, .ViPipe = 0, .ViChn = 0, .enable = {1, 1,}, .enSize = {PIC_2592x1536, PIC_720P,}},
          };
      #else
      // imx327-2-0-2-30
      gsf_mpp_cfg_t cfg = { .snsname = "imx327", .snscnt = 2, .lane = 2, .wdr  = 0, .res  = 2, .fps  = 30, };
      gsf_rgn_ini_t rgn_ini = {.ch_num = 2, .st_num = 1};
      gsf_venc_ini_t venc_ini = {.ch_num = 2, .st_num = 1};
      gsf_mpp_vpss_t vpss[GSF_CODEC_IPC_CHN] = {
            {.VpssGrp = 0, .ViPipe = 0, .ViChn = 0, .enable = {1, 0,}, .enSize = {PIC_1080P, PIC_720P,}},
            {.VpssGrp = 1, .ViPipe = 1, .ViChn = 0, .enable = {1, 0,}, .enSize = {PIC_1080P, PIC_720P,}},
          };
      #endif
    
    #elif defined(GSF_CPU_3516e)
    
      #if 1
      // sc2335-0-0-2-30
      gsf_mpp_cfg_t cfg = { .snsname = "sc2335", .snscnt = 1, .lane = 0, .wdr  = 0, .res  = 2, .fps  = 30, };
      gsf_rgn_ini_t rgn_ini = {.ch_num = 1, .st_num = 2};
      gsf_venc_ini_t venc_ini = {.ch_num = 1, .st_num = 2};
      gsf_mpp_vpss_t vpss[GSF_CODEC_IPC_CHN] = {
            {.VpssGrp = 0, .ViPipe = 0, .ViChn = 0, .enable = {1, 1,}, .enSize = {PIC_1080P, PIC_720P,}},
          };

      #endif
    #elif defined(GSF_CPU_3519)
      // imx334-0-0-8-30
      gsf_mpp_cfg_t cfg = { .snsname = "imx334", .snscnt = 1, .lane = 0, .wdr  = 0, .res  = 8, .fps  = 30, };
      gsf_rgn_ini_t rgn_ini = {.ch_num = 2, .st_num = 1};
      gsf_venc_ini_t venc_ini = {.ch_num = 2, .st_num = 1};
      gsf_mpp_vpss_t vpss[GSF_CODEC_IPC_CHN] = {
            {.VpssGrp = 0, .ViPipe = 0, .ViChn = 0, .enable = {1, 0,}, .enSize = {PIC_3840x2160, PIC_720P,}},
            {.VpssGrp = 1, .ViPipe = 1, .ViChn = 0, .enable = {1, 0,}, .enSize = {PIC_1080P, PIC_720P,}},
          };
    #elif defined(GSF_CPU_3559)
      // imx458-0-0-8-30
      gsf_mpp_cfg_t cfg = { .snsname = "imx458", .snscnt = 1, .lane = 0, .wdr  = 0, .res  = 8, .fps  = 30, };
      gsf_rgn_ini_t rgn_ini = {.ch_num = 1, .st_num = 2};
      gsf_venc_ini_t venc_ini = {.ch_num = 1, .st_num = 2};
      gsf_mpp_vpss_t vpss[GSF_CODEC_IPC_CHN] = {
            {.VpssGrp = 0, .ViPipe = 0, .ViChn = 0, .enable = {1, 1,}, .enSize = {PIC_3840x2160, PIC_720P,}},
          };
    #else
      #error "error unknow gsf_mpp_cfg_t."
    #endif 

    //maohw strcpy(cfg.snsname, bsp_def.board.sensor[0]);
    p_venc_ini = &venc_ini;
    
    
    char home_path[256] = {0};
    proc_absolute_path(home_path);
    sprintf(home_path, "%s/../", home_path);
    printf("home_path:[%s]\n", home_path);
   
    gsf_mpp_cfg(home_path, &cfg);
    
    //internal-init rgn, venc;
    gsf_rgn_init(&rgn_ini);
    gsf_venc_init(&venc_ini);

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
    
    // vpss start;
    #if defined(GSF_CPU_3559a) && (AVS_4CH_3559a == 1)
    for(i = 0; i < 4; i++)
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


    venc_start(1);
    
    //init listen;
    void* rep = nm_rep_listen(GSF_IPC_CODEC, NM_REP_MAX_WORKERS, NM_REP_OSIZE_MAX, req_recv);
    
    //test osd;
    /////////////////////////////////////
    /// void *rgn_timer = timer_add(1000, rgn_timer_func, NULL);
    void* sub = nm_sub_conn(GSF_PUB_SVP, sub_recv);
    printf("nm_sub_conn sub:%p\n", sub);
    /////////////////////////////////////
    
    
    while(1)
    {
      sleep(1);
    }
      
    GSF_LOG_DISCONN();
    return 0;
}