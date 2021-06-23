#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <assert.h>

#include "cfg.h"
#include "msg_func.h"


GSF_LOG_GLOBAL_INIT("SIPS", 8*1024);



#include "mod/bsp/inc/bsp.h"
#include "mod/codec/inc/codec.h"

#define MAX_FRAME_SIZE (800*1024)

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

int main(int argc, char *argv[])
{
    if(argc < 2)
    {
      printf("pls input: %s sips_parm.json\n", argv[0]);
      return -1;
    }
    
    strncpy(sips_parm_path, argv[1], sizeof(sips_parm_path)-1);
    
    if(json_parm_load(sips_parm_path, &sips_parm) < 0)
    {
      json_parm_save(sips_parm_path, &sips_parm);
      json_parm_load(sips_parm_path, &sips_parm);
    }
    info("sips_parm => auth:%d, port:%d\n", sips_parm.auth, sips_parm.port);
    
    GSF_LOG_CONN(0, 100);
    void* rep = nm_rep_listen(GSF_IPC_SIPS, NM_REP_MAX_WORKERS, NM_REP_OSIZE_MAX, req_recv);

    sip_st_init();


    while(1)
    {
      sleep(3);
    }

    GSF_LOG_DISCONN();
    return 0;
}