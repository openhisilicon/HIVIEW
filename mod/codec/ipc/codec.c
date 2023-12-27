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

//////////////////////////////////////////////////////////////////

GSF_LOG_GLOBAL_INIT("CODEC", 8*1024);

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
    if(codec_ipc.vi.avs == 1)
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
    
    #warning "...... @@@@@@@@ GSF_DEV_IPC @@@@@@@@ ......"
    printf("\n...... @@@@@@@@ GSF_DEV_IPC @@@@@@@@ ......\n");

    // register to bsp && get bsp_def;
    if(reg2bsp() < 0)
      return -1;

    if(getdef(&bsp_def) < 0)
      return -1;

    GSF_LOG_CONN(0, 100);
    
    main_start(&bsp_def);
    
    //init listen;
    void* rep = nm_rep_listen(GSF_IPC_CODEC
                      , NM_REP_MAX_WORKERS
                      , NM_REP_OSIZE_MAX
                      , req_recv);
                      
    void* osd_pull = nm_pull_listen(GSF_IPC_OSD, osd_recv);
    
    void* sub = nm_sub_conn(GSF_PUB_SVP, sub_recv);
    printf("nm_sub_conn sub:%p\n", sub);

    
    while(1)
    {
      main_loop();
    }

    GSF_LOG_DISCONN();
    return 0;
}