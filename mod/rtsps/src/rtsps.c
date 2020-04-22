#include <unistd.h>
#include <stdlib.h>
#include "inc/gsf.h"

#include "rtsps.h"
#include "cfg.h"
#include "msg_func.h"
#include "rtsp-st.h"

#include "mod/bsp/inc/bsp.h"

GSF_LOG_GLOBAL_INIT("RTSPS", 8*1024);

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

#include <signal.h>
static void main_exit(int signal)
{
  //TODO;
	exit(0);
}

static int multi_processes()
{
  int i = 0, pid = 0;
  for (i = 0; i < 2; i++) // connect in multiple processes when process > 1;
  {
    if ((pid = fork()) < 0) 
    {
      printf("WARN: can't create process: fork, i:%d\n", i);
      break;
    }
    if (pid == 0)
    {
      printf("CHILD: process: i:%d, pid:%d\n", i, getpid());
      rtsp_st_init();
      
      while(1) sleep(1000);
      return 0;
    }
  }
  sleep(1); //wait child runing;
  printf("PARENT: process: i:%d, pid:%d\n", i, getpid());
  return 0;
}

int main(int argc, char *argv[])
{
  signal(SIGPIPE, SIG_IGN);
  signal(SIGTERM, main_exit);
  signal(SIGINT,  main_exit);


  if(argc < 2)
  {
    printf("pls input: %s rtsps_parm.json\n", argv[0]);
    return -1;
  }
  
  if(json_parm_load(argv[1], &rtsps_parm) < 0)
  {
    json_parm_save(argv[1], &rtsps_parm);
    json_parm_load(argv[1], &rtsps_parm);
  }

  #if 0
  //multi_processes();
  #else
  rtsp_st_init();
  #endif
  
  
  while(1)
  {
    //register To;
    GSF_MSG_DEF(gsf_mod_reg_t, reg, 8*1024);
    reg->mid = GSF_MOD_ID_RTSPS;
    strcpy(reg->uri, GSF_IPC_RTSPS);
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
  void* rep = nm_rep_listen(GSF_IPC_RTSPS
                        , NM_REP_MAX_WORKERS
                        , NM_REP_OSIZE_MAX
                        , req_recv);
  
  while(1)
  {
    if(0)
    {
      GSF_MSG_DEF(gsf_rtsp_url_t, ru, 8*1024);
      sprintf(ru->url, "%s", "rtsp://admin:12345@192.168.1.2:554/1");
      int ret = GSF_MSG_SENDTO(GSF_ID_RTSPS_C_OPEN, 0, SET, 0, sizeof(gsf_rtsp_url_t), GSF_IPC_RTSPS, 2000);
      printf("GSF_ID_RTSPS_C_OPEN ret:%d, size:%d\n", ret, __rsize);
    }
    
    sleep(6);
    
    if(0)
    {
      GSF_MSG_DEF(gsf_rtsp_url_t, ru, 8*1024);
      sprintf(ru->url, "%s", "rtsp://admin:12345@192.168.1.2:554/1");
      int ret = GSF_MSG_SENDTO(GSF_ID_RTSPS_C_CLOSE, 0, SET, 0, sizeof(gsf_rtsp_url_t), GSF_IPC_RTSPS, 2000);
      printf("GSF_ID_RTSPS_C_CLOSE ret:%d, size:%d\n", ret, __rsize);
    }
    
  }
  
  GSF_LOG_DISCONN();
  return 0;
}
