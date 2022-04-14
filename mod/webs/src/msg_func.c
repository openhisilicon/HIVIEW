//os

//top
#include "inc/gsf.h"

//mod
#include "webs.h"

//myself
#include "cfg.h"
#include "msg_func.h"


static void msg_func_cfg(gsf_msg_t *req, int isize, gsf_msg_t *rsp, int *osize)
{
  if(req->set)
  {
    gsf_webs_cfg_t *cfg = (gsf_webs_cfg_t*)req->data;
    
    //http_close();
    
    webs_cfg = *cfg;
    //if(http_open(webs_cfg.port) == 0)
    {
      json_parm_save(webs_parm_path, &webs_cfg);
    }

    rsp->err  = 0;
    rsp->size = 0;
  }
  else
  {
    gsf_webs_cfg_t *cfg = (gsf_webs_cfg_t*)rsp->data;
    *cfg = webs_cfg;
    rsp->err  = 0;
    rsp->size = sizeof(gsf_webs_cfg_t);
  }
}


static msg_func_t *msg_func[GSF_ID_WEBS_END] = {
    [GSF_ID_MOD_CLI]      = NULL,
    [GSF_ID_WEBS_CFG]      = msg_func_cfg,
 };


int msg_func_proc(gsf_msg_t *req, int isize, gsf_msg_t *rsp, int *osize)
{
    if(req->id < 0 || req->id >= GSF_ID_WEBS_END)
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
