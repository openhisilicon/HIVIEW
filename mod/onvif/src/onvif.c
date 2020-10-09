#include <unistd.h>
#include <stdlib.h>
#include <assert.h>

#include "inc/gsf.h"

#include "onvif.h"
#include "cfg.h"
#include "msg_func.h"
#include "onvif_api.h"

GSF_LOG_GLOBAL_INIT("ONVIF", 8*1024);

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
    reg->mid = GSF_MOD_ID_ONVIF;
    strcpy(reg->uri, GSF_IPC_ONVIF);
    int ret = GSF_MSG_SENDTO(GSF_ID_MOD_CLI, 0, SET, GSF_CLI_REGISTER
                            , sizeof(gsf_mod_reg_t), GSF_IPC_BSP, 2000);
                            
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

int main(int argc, char *argv[])
{

    if(argc < 2)
    {
      printf("pls input: %s onvif_parm.json\n", argv[0]);
      return -1;
    }
    
    strncpy(onvif_parm_path, argv[1], sizeof(onvif_parm_path)-1);
    
    if(json_parm_load(onvif_parm_path, &onvif_cfg) < 0)
    {
      json_parm_save(onvif_parm_path, &onvif_cfg);
      json_parm_load(onvif_parm_path, &onvif_cfg);
    }
    
    GSF_LOG_CONN(0, 100);
    void* rep = nm_rep_listen(GSF_IPC_ONVIF
                    , NM_REP_MAX_WORKERS
                    , NM_REP_OSIZE_MAX
                    , req_recv);        
    reg2bsp();
    

    {
      nvt_parm_t* parm = (nvt_parm_t*)calloc(1, sizeof(nvt_parm_t));
      parm->port = onvif_cfg.port;
      parm->ch_num = onvif_cfg.chnum;
      //parm->msg_cb = NULL;
      nvt_init(parm);
      nvt_start();
      
      nvc_init();
    }

                    
    while(1)
    {
      sleep(1);
    }
      
    GSF_LOG_DISCONN();

  return 0;
}