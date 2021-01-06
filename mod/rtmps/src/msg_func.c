//os

//top
#include "inc/gsf.h"

//mod
#include "rtmps.h"

//myself
#include "msg_func.h"


static msg_func_t *msg_func[GSF_ID_RTMPS_END] = {
    [GSF_ID_MOD_CLI]      = NULL,
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
