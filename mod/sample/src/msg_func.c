//os

//top
#include "inc/gsf.h"

//mod
#include "sample.h"

//myself
#include "msg_func.h"

static void msg_func_cfg(gsf_msg_t *req, int isize, gsf_msg_t *rsp, int *osize)
{
  if(req->set)
  {
    sample_parm = *((gsf_sample_t*)req->data);
    json_parm_save(sample_parm_path, &sample_parm);
    rsp->err  = 0;
    rsp->size = 0;
  }
  else
  {
    *((gsf_sample_t*)rsp->data) = sample_parm;
    rsp->err  = 0;
    rsp->size = sizeof(gsf_sample_t);
  }
}


static msg_func_t *msg_func[GSF_ID_SAMPLE_END] = {
    [GSF_ID_MOD_CLI]      = NULL,
    [GSF_ID_SAMPLE_CFG]      = msg_func_cfg,
 };


int msg_func_proc(gsf_msg_t *req, int isize, gsf_msg_t *rsp, int *osize)
{
    if(req->id < 0 || req->id >= GSF_ID_SAMPLE_END)
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
