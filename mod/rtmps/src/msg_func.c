//os

//top
#include "inc/gsf.h"

//mod
#include "rtmps.h"

//myself
#include "msg_func.h"

static void msg_func_cfg(gsf_msg_t *req, int isize, gsf_msg_t *rsp, int *osize)
{
  if(req->set)
  {
    rtmps_parm = *((gsf_rtmps_t*)req->data);
    rtmp_push_stop(NULL);
    json_parm_save(rtmps_parm_path, &rtmps_parm);
    rsp->err  = 0;
    rsp->size = 0;
  }
  else
  {
    *((gsf_rtmps_t*)rsp->data) = rtmps_parm;
    rsp->err  = 0;
    rsp->size = sizeof(gsf_rtmps_t);
  }
}


static msg_func_t *msg_func[GSF_ID_RTMPS_END] = {
    [GSF_ID_MOD_CLI]      = NULL,
    [GSF_ID_RTMPS_CFG]      = msg_func_cfg,
 };


int msg_func_proc(gsf_msg_t *req, int isize, gsf_msg_t *rsp, int *osize)
{
    if(req->id < 0 || req->id >= GSF_ID_RTMPS_END)
    {
        return FALSE;
    }
    
    if(msg_func[req->id] == NULL)
    {
        return FALSE;
    }   
    
    msg_func[req->id](req, isize, rsp, osize);
    
    return TRUE;
}
