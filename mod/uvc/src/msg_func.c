//os

//top
#include "inc/gsf.h"

//mod
#include "uvc.h"

//myself
#include "msg_func.h"

static void msg_func_cfg(gsf_msg_t *req, int isize, gsf_msg_t *rsp, int *osize)
{
  if(req->set)
  {
    uvc_parm = *((gsf_uvc_t*)req->data);
    //TODO
    json_parm_save(uvc_parm_path, &uvc_parm);
    rsp->err  = 0;
    rsp->size = 0;
  }
  else
  {
    *((gsf_uvc_t*)rsp->data) = uvc_parm;
    rsp->err  = 0;
    rsp->size = sizeof(gsf_uvc_t);
  }
}


static msg_func_t *msg_func[GSF_ID_UVC_END] = {
    [GSF_ID_MOD_CLI]      = NULL,
    [GSF_ID_UVC_CFG]      = msg_func_cfg,
 };


int msg_func_proc(gsf_msg_t *req, int isize, gsf_msg_t *rsp, int *osize)
{
    if(req->id < 0 || req->id >= GSF_ID_UVC_END)
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
