#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <assert.h>

#include "uvc.h"

#include "cfg.h"
#include "msg_func.h"
#include "camera.h"
#include "frame_cache.h"
#include "config_svc.h"

//from application.c
extern int create_cache(void);
extern void destroy_cache(void);
extern int run_camera(void);
extern void stop_camera(void);


GSF_LOG_GLOBAL_INIT("UVC", 8*1024);

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
      printf("pls input: %s uvc_parm.json\n", argv[0]);
      return -1;
    }
    
    strncpy(uvc_parm_path, argv[1], sizeof(uvc_parm_path)-1);
    
    if(json_parm_load(uvc_parm_path, &uvc_parm) < 0)
    {
      json_parm_save(uvc_parm_path, &uvc_parm);
      json_parm_load(uvc_parm_path, &uvc_parm);
    }
    info("uvc_parm => en:%d\n", uvc_parm.en);
    
    GSF_LOG_CONN(1, 100);
    void* rep = nm_rep_listen(GSF_IPC_UVC, NM_REP_MAX_WORKERS, NM_REP_OSIZE_MAX, req_recv);

    if(uvc_parm.en)
    {
      if (create_config_svc("") != 0)
      {
        printf("create_config_svc err.\n");
      }
      if (create_cache() != 0)
      {
        printf("create_config_svc err.\n");
      }
      if (run_camera() != 0)
      {
        printf("create_config_svc err.\n");
      }
    }
    
    while (1)
    {
        sleep(1);
    }

    destroy_cache();
    release_cofnig_svc();
    GSF_LOG_DISCONN();
    return 0;
}