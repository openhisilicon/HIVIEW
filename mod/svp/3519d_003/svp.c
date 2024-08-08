#include <unistd.h>
#include <stdlib.h>
#include <assert.h>

#include "fw/comm/inc/proc.h"
#include "mod/codec/inc/codec.h"
#include "svp.h"
#include "cfg.h"
#include "msg_func.h"

#include "yolov5.h"

void* svp_pub = NULL;
GSF_LOG_GLOBAL_INIT("SVP", 8*1024);

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
static int reg2bsp()
{
  while(1)
  {
    //register To;
    GSF_MSG_DEF(gsf_mod_reg_t, reg, 8*1024);
    reg->mid = GSF_MOD_ID_SVP;
    strcpy(reg->uri, GSF_IPC_SVP);
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

#include <signal.h>

static void _handle_sig(int signo)
{
    if (signo == SIGINT || signo == SIGTERM) 
    {
       extern int yolo_stop(); yolo_stop();
    }
    exit(-1);
}

int sample_svp_reg_signal()
{
  struct sigaction sa;
  sa.sa_handler = _handle_sig;
  sa.sa_flags = 0;
  sigaction(SIGINT, &sa, NULL);
  sigaction(SIGTERM, &sa, NULL);
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


int main(int argc, char *argv[])
{
    if(argc < 2)
    {
      printf("pls input: %s svp_parm.json\n", argv[0]);
      return -1;
    }
     
    strncpy(svp_parm_path, argv[1], sizeof(svp_parm_path)-1);
    
    if(json_parm_load(svp_parm_path, &svp_parm) < 0)
    {
      json_parm_save(svp_parm_path, &svp_parm);
      json_parm_load(svp_parm_path, &svp_parm);
    }
 
    svp_pub = nm_pub_listen(GSF_PUB_SVP);
     
    //wait codec.exe startup
    gsf_resolu_t res = {0};
    vores_get(&res); 
     
    char home_path[256] = {0};
    proc_absolute_path(home_path);
    sprintf(home_path, "%s/../", home_path);
    printf("home_path:[%s]\n", home_path);
    
    sample_svp_reg_signal();
    printf("init algorithm library...\n");
    
    extern int yolo_start(char *home_path, int v8); 
    extern int yolo_stop();
    // for test yolo_start(home_path, 1/*v8*/);
    
    //init listen;
    GSF_LOG_CONN(0, 100);
    void* rep = nm_rep_listen(GSF_IPC_SVP
                    , NM_REP_MAX_WORKERS
                    , NM_REP_OSIZE_MAX
                    , req_recv);
    //reg2bsp();
    
    int yolo_alg = 0;
    while(1)
    {                        
      if(yolo_alg != svp_parm.svp.yolo_alg)
      {
        if(yolo_alg == 0 && svp_parm.svp.yolo_alg > 0)
        {
          yolo_start(home_path, 0/*v8*/);
        }
        else if(yolo_alg > 0 && svp_parm.svp.yolo_alg == 0)
        {
          yolo_stop();
        }
        yolo_alg = svp_parm.svp.yolo_alg;  
      }
      sleep(1);
    }
    
    GSF_LOG_DISCONN();

    return 0;
}