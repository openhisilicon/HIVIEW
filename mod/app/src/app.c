#include <unistd.h>
#include <stdlib.h>
#include <assert.h>

#include "inc/gsf.h"

#include "app.h"
#include "cfg.h"
#include "msg_func.h"
#include "nvr_live.h"
#include "kbd.h"

GSF_LOG_GLOBAL_INIT("APP", 8*1024);

extern int lvgl_stop(void);
extern int lvgl_start(int w, int h);
extern int lvgl_ctl(int cmd, void* args);

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

//default lt;
#if defined(GSF_CPU_3559a)
static int lt = 4;
#else
static int lt = 1;
#endif

int vo_ly(int num)
{
  static int voch[GSF_CODEC_NVR_CHN] = {0,1,2,3};
  GSF_MSG_DEF(gsf_layout_t, ly, 8*1024);
  
  if(num >= 1 && num <= 4)
  {
    lt = 1;
    voch[0] = num -1;
  }
  else if(num >= 5)
  {
    lt = 4;
    voch[0] = 0; voch[1] = 1; voch[2] = 2; voch[3] = 3;
  }
  else if (num <= 0)
  {
    ;//keep it;
  }
  printf("lt:%d, voch[%d,%d,%d,%d]\n", lt, voch[0], voch[1], voch[2], voch[3]);

  ly->layout = lt;
  
  #if defined(GSF_CPU_3559a)
  live_get_shmid(ly->layout, voch, 0, ly->shmid);
  #else
  live_get_shmid(ly->layout, voch, lt>1?1:0, ly->shmid);
  #endif
  
  int ret = GSF_MSG_SENDTO(GSF_ID_CODEC_VOLY, 0, SET, 0
                        , sizeof(gsf_layout_t)
                        , GSF_IPC_CODEC, 2000);

  if(ly->layout == 1 
    && voch[0] >= 0 && voch[0] <= GSF_CODEC_NVR_CHN)
  {
    kbd_set_url(app_nvr.chsrc[voch[0]].host);
  } 
                       
  return ret;
}



static int sub_recv(char *msg, int size, int err)
{
  gsf_msg_t *pmsg = (gsf_msg_t*)msg;

  if(pmsg->id == GSF_EV_BSP_MOD_REG)
  {
    gsf_mod_reg_t *reg = (gsf_mod_reg_t*) pmsg->data;
    
    printf("GSF_EV_BSP_MOD_REG mid:%d, pid:%d, uri:%s\n"
          , reg->mid, reg->pid, reg->uri);
          
    if(reg->mid == GSF_MOD_ID_RTSPS)
    {
      live_clear_rtsp();
    }
    else if(reg->mid == GSF_MOD_ID_ONVIF)
    {
      live_clear_onvif();
    }
    else if(reg->mid == GSF_MOD_ID_CODEC)
    {
      vo_ly(0);
    }
  }
  
  return 0;
}



static int reg2bsp()
{
  while(1)
  {
    //register To;
    GSF_MSG_DEF(gsf_mod_reg_t, reg, 8*1024);
    reg->mid = GSF_MOD_ID_APP;
    strcpy(reg->uri, GSF_IPC_APP);
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

static int vores_get(gsf_resolu_t *res)
{
  while(1)
  {
    //register To;
    GSF_MSG_DEF(gsf_resolu_t, _res, 8*1024);
    int ret = GSF_MSG_SENDTO(GSF_ID_CODEC_VORES, 0, GET, 0, 0, GSF_IPC_CODEC, 2000);
    printf("GSF_ID_CODEC_VORES To:%s, ret:%d, size:%d\n", GSF_IPC_CODEC, ret, __rsize);
    static int cnt = 6;
    if(ret == 0)
    {
      *res = *_res;
      break;
    }
    if(cnt-- < 0)
      return -1;
    sleep(1);
  }
  return 0;
}

static int voly_get(gsf_layout_t *ly)
{
  while(1)
  {
    //register To;
    GSF_MSG_DEF(gsf_layout_t, _ly, 8*1024);
    int ret = GSF_MSG_SENDTO(GSF_ID_CODEC_VOLY, 0, GET, 0, 0, GSF_IPC_CODEC, 2000);
    printf("GSF_ID_CODEC_VOLY To:%s, ret:%d, size:%d\n", GSF_IPC_CODEC, ret, __rsize);
    static int cnt = 6;
    if(ret == 0)
    {
      *ly = *_ly;
      break;
    }
    if(cnt-- < 0)
      return -1;
    sleep(1);
  }
  return 0;
}




int main(int argc, char *argv[])
{
    if(argc < 2)
    {
      printf("pls input: %s app_parm.json\n", argv[0]);
      return -1;
    }
    
    strncpy(app_parm_path, argv[1], sizeof(app_parm_path)-1);
    
    if(json_parm_load(app_parm_path, &app_nvr) < 0)
    {
      json_parm_save(app_parm_path, &app_nvr);
      json_parm_load(app_parm_path, &app_nvr);
    }
    
    // joint multiple mods to works;
    
    // get res;
    gsf_resolu_t res = {0};
    vores_get(&res);
    if(!res.w || !res.h)
    {
      printf("vores_get err.\n");
      return -1;
    }
    
    //get ly;
    gsf_layout_t ly = {0};
    voly_get(&ly);
    lt = ly.layout;
    lvgl_ctl(0, (void*)lt);
    
    //kbd_mon("/dev/ttyAMA2");
    lvgl_start(res.w, res.h);

    #ifdef GSF_DEV_NVR
    #warning "...... DDDDDDDD GSF_DEV_NVR DDDDDDDD ......"
    live_mon();
    #else
    #warning "...... UUUUUUUU GSF_DEV_NVR UUUUUUUU ......"
    #endif

    //init listen;
    GSF_LOG_CONN(1, 100);
    void* rep = nm_rep_listen(GSF_IPC_APP
                    , NM_REP_MAX_WORKERS
                    , NM_REP_OSIZE_MAX
                    , req_recv);        
    reg2bsp();
    
    void* sub = nm_sub_conn(GSF_PUB_BSP, sub_recv);

    while(1)
    {
      sleep(5);
    }
    
    GSF_LOG_DISCONN();

  return 0;
}