#include <unistd.h>
#include <stdlib.h>
#include <assert.h>

#include "inc/gsf.h"
#include "mod/bsp/inc/bsp.h"

#include "codec.h"
#include "cfg.h"
#include "msg_func.h"
#include "mpp.h"
#include "rgn.h"
#include "venc.h"

#define PIC_WIDTH(w) \
            (w >= 3840)?PIC_3840x2160:\
            (w >= 1920)?PIC_1080P:\
            (w >= 1280)?PIC_720P: \
            PIC_D1_NTSC
            
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

static int rgn_timer_func(void *u)
{
  static gsf_osd_t osd;
  if(!osd.en)
  {
    osd.en = 1;
    osd.type = 0;
    osd.fontsize = 0;
    osd.point[0] = 10;
    osd.point[1] = 10;
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
  gsf_rgn_osd_set(0, 0, &osd);
  
  vmask.rect[0] += 100;
  vmask.rect[0] %= 2000;
  vmask.rect[1] += 100;
  vmask.rect[1] %= 2000;
  printf("vmask: x:%d,y:%d,w:%d,h:%d\n", vmask.rect[0], vmask.rect[1], vmask.rect[2], vmask.rect[3]);
  gsf_rgn_vmask_set(0, 0, &vmask);
  
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
  printf("GET GSF_ID_BSP_DEF To:%s, ret:%d, model:%s\n", GSF_IPC_BSP, ret, cfg->board.model);

  if(ret < 0)
    return ret;

  if(def) 
    *def = *cfg;
    
  return ret;
}


int venc_start(int start)
{
  // venc start;
  
  if(!start)
  {
    printf("stop >>> gsf_mpp_venc_dest()\n");
    gsf_mpp_venc_dest();
  }

  int i = 0;
  for(i = 0; i < GSF_CODEC_VENC_NUM; i++)
  {
    if(!codec_ipc.venc[i].en)
      break;

    gsf_mpp_venc_t venc = {
      .VencChn = i,
      .VpssGrp = 0,
      .VpssChn = (i>=2)?0:i, //vpss_ch;
      .enPayLoad  = PT_VENC(codec_ipc.venc[i].type),
      .enSize     = PIC_WIDTH(codec_ipc.venc[i].width), //vpss.enSize[0],
      .enRcMode   = SAMPLE_RC_CBR,
      .u32Profile = 0,
      .bRcnRefShareBuf = HI_TRUE,
      .enGopMode = VENC_GOPMODE_NORMALP,
      .u32FrameRate = codec_ipc.venc[i].fps,
      .u32Gop       = codec_ipc.venc[i].gop,
      .u32BitRate   = codec_ipc.venc[i].bitrate,
    };
    printf("start >>> i:%d, width:%d\n", i, codec_ipc.venc[i].width);
    if(!start)
      gsf_mpp_venc_stop(&venc);
    else
      gsf_mpp_venc_start(&venc);
  }
  
  if(!start)
    return 0;
    
  // recv start;
  gsf_mpp_recv_t st = {
    .s32Cnt = (i<=2)?i:2,  // 3 for JPEG;
    .VeChn = {0,1},
    .uargs = NULL,
    .cb = gsf_venc_recv,
  };
  printf("start >>> gsf_mpp_venc_recv s32Cnt:%d\n", st.s32Cnt);
  gsf_mpp_venc_recv(&st);

  // rgn;
  for(i = 0; i < GSF_CODEC_OSD_NUM; i++)
    gsf_rgn_osd_set(0, i, &codec_ipc.osd[i]);
    
  for(i = 0; i < GSF_CODEC_VMASK_NUM; i++)
    gsf_rgn_vmask_set(0, i, &codec_ipc.vmask[i]);

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
    //init listen;
    GSF_LOG_CONN(0, 100);
    void* rep = nm_rep_listen(GSF_IPC_CODEC, NM_REP_MAX_WORKERS, NM_REP_OSIZE_MAX, req_recv);

    gsf_rgn_init(NULL);
    gsf_venc_init(NULL);

    // init mpp;
    gsf_mpp_cfg_t cfg = {
        .snsname = "xxx",
        .snscnt = 1,
        .lane = 0,
        .wdr  = 0,
        #ifdef GSF_CPU_3519a // imx334-0-0-8-60
        .res  = 8,
        .fps  = 60,
        #elif defined(GSF_CPU_3516d)
        .res  = 4,          // imx335-0-0-4-30
        .fps  = 30,
        #else
        .res  = 2,          // sc2335-0-0-2-30
        .fps  = 30,
        #endif
    };
    
    strcpy(cfg.snsname, bsp_def.board.sensor[0]);
    
    gsf_mpp_cfg(NULL, &cfg);
    
    // vi start;
    gsf_mpp_vi_t vi = {
        .bLowDelay = HI_FALSE,
        .u32SupplementConfig = 0,
        #ifdef GSF_CPU_3516e
        .vpss_en = {1, 1,},
        .vpss_sz = {PIC_1080P, PIC_720P,},
        #endif
    };
    gsf_mpp_vi_start(&vi);
    
    // vpss start;
    gsf_mpp_vpss_t vpss = {
        .VpssGrp = 0,
        .ViPipe  = 0,
        .ViChn   = 0, 
        .enable  = {1, 1,},
        #ifdef GSF_CPU_3519a          // 4K
        .enSize  = {PIC_3840x2160, PIC_720P,},
        #elif defined(GSF_CPU_3516d)  // 4M
        .enSize  = {PIC_2592x1536, PIC_720P,},
        #else
        .enSize  = {PIC_1080P, PIC_720P,},
        #endif
    };
    gsf_mpp_vpss_start(&vpss);

    // scene start;
    char scene_ini[128];
    sprintf(scene_ini, "/var/app/cfg/%s.ini", bsp_def.board.sensor[0]);
    gsf_mpp_scene_start(scene_ini, 0);

    venc_start(1);
    
    // rgn test;
    //void *rgn_timer = timer_add(1000, rgn_timer_func, NULL);

    while(1)
    {
      sleep(1);
    }
      
    GSF_LOG_DISCONN();
    return 0;
}