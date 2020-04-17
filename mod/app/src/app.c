#include <unistd.h>
#include <stdlib.h>
#include <assert.h>

#include "inc/gsf.h"

#include "app.h"
#include "cfg.h"
#include "msg_func.h"
#include "nvr_live.h"

GSF_LOG_GLOBAL_INIT("APP", 8*1024);

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

  if(pmsg == GSF_EV_BSP_MOD_REG)
  {
    gsf_mod_reg_t *reg = (gsf_mod_reg_t*) pmsg->data;
    printf("GSF_EV_BSP_MOD_REG mid:%d, pid:%d, uri:%s\n"
          , reg->mid, reg->pid, reg->uri);
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

int vo_ly()
{
  int voch[GSF_CODEC_NVR_CHN] = {3, 2, 1, 0,};
  GSF_MSG_DEF(gsf_layout_t, ly, 8*1024);
  
  ly->layout = 4;
  live_get_shmid(ly->layout, voch, ly->shmid);
  int ret = GSF_MSG_SENDTO(GSF_ID_CODEC_VOLY, 0, SET, 0
                        , sizeof(gsf_layout_t)
                        , GSF_IPC_CODEC, 2000);
  return ret;
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
    
    #ifdef GSF_DEV_NVR
    live_mon();
    #endif
     
    //init listen;
    GSF_LOG_CONN(0, 100);
    void* rep = nm_rep_listen(GSF_IPC_APP
                    , NM_REP_MAX_WORKERS
                    , NM_REP_OSIZE_MAX
                    , req_recv);        
    reg2bsp();
    
    void* sub = nm_sub_conn(GSF_PUB_BSP, sub_recv);
       
       

       
                    
    while(1)
    {
      sleep(1);
    }
      
    GSF_LOG_DISCONN();

  return 0;
}