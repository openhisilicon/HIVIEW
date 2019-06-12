#include <unistd.h>
#include <stdlib.h>
#include "inc/gsf.h"
#include "bsp.h"

#include "cfg.h"      // config
#include "netinf.h"   // netinf
#include "wdg.h"      // watchdog
#include "rtc.h"      // rtc
#include "dio.h"      // gpio in/out
#include "mod/bsp/lib/sadp/inc/sadp.h"  // discover
#include "mod/bsp/lib/upg/inc/upg.h"    // upgrade

#include "msg_func.h"


GSF_LOG_GLOBAL_INIT("BSP", 8*1024);

static int log_recv(char *msg, int size, int err)
{
    fprintf(stderr, "L%s", msg);
    return 0;
}

static int req_recv(char *in, int isize, char *out, int *osize, int err)
{
    int ret = 0;
    gsf_msg_t *req = (gsf_msg_t*)in;
    gsf_msg_t *rsp = (gsf_msg_t*)out;

    *rsp  = *req;
    rsp->err  = 0;
    rsp->size = 0;

    if(1)
    {
    	struct timespec _ts;
      clock_gettime(CLOCK_MONOTONIC, &_ts);
      printf("id:%d, delay:%d ms\n", req->id, _ts.tv_sec*1000 + _ts.tv_nsec/1000000 - req->ts);     
    }

    ret = msg_func_proc(req, isize, rsp, osize);

    rsp->err = (ret == TRUE)?rsp->err:GSF_ERR_MSG;
    *osize = sizeof(gsf_msg_t)+rsp->size;

    return 0;
}


int main(int argc, char *argv[])
{
    void* log_pull = nm_pull_listen(GSF_IPC_LOG, log_recv); 
    
    if(argc != 3)
    {
      printf("pls input: %s bsp_def.json bsp_parm.json\n", argv[0]);
      return -1;
    }
    
    GSF_LOG_CONN(1, 100);
    
    if(jscon_def_load(argv[1], &bsp_def) < 0)
    {
      jscon_def_save(argv[1], &bsp_def);
      jscon_def_load(argv[1], &bsp_def);
    }
    info("def.model:%s\n", bsp_def.board.model);
    
    if(jscon_parm_load(argv[2], &bsp_parm) < 0)
    {
      bsp_parm.eth = bsp_def.eth;
      bsp_parm.users[0] = bsp_def.admin;
      
      jscon_parm_save(argv[2], &bsp_parm);
      jscon_parm_load(argv[2], &bsp_parm);
    }
    info("parm.ipaddr:%s\n", bsp_parm.eth.ipaddr);

    
    wdg_init();
    rtc_init();
    dio_init();
    netinf_init();
    upg_init(&bsp_def.board);
    sadp_init(&bsp_def.board);
    
    
    void* rep = nm_rep_listen(GSF_IPC_BSP
                    , NM_REP_MAX_WORKERS
                    , NM_REP_OSIZE_MAX
                    , req_recv);
    
    while(1)
    {
        sleep(1);
    }
    
    nm_rep_close(rep);
    GSF_LOG_DISCONN();
    nm_pull_close(log_pull);
    return 0;
}