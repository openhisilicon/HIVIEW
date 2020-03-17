//os

//top
#include "inc/gsf.h"

//mod
#include "rtsps.h"

//myself
#include "msg_func.h"

static void msg_func_cfg(gsf_msg_t *req, int isize, gsf_msg_t *rsp, int *osize)
{
}


static msg_func_t *msg_func[GSF_ID_RTSPS_END] = {
    [GSF_ID_MOD_CLI]      = NULL,
    [GSF_ID_RTSPS_CFG]    = msg_func_cfg,
 };


int msg_func_proc(gsf_msg_t *req, int isize, gsf_msg_t *rsp, int *osize)
{
    if(req->id < 0 || req->id >= GSF_ID_RTSPS_END)
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
