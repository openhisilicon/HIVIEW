//os

//top
#include "inc/gsf.h"

//mod
#include "codec.h"

//myself
#include "cfg.h"
#include "msg_func.h"
#include "mpp.h"
#include "live.h"

static void msg_func_vores(gsf_msg_t *req, int isize, gsf_msg_t *rsp, int *osize)
{
  rsp->err  = -1;
}

static void msg_func_voly(gsf_msg_t *req, int isize, gsf_msg_t *rsp, int *osize)
{
  gsf_layout_t *ly = (gsf_layout_t *)req->data;
  
  live_ly(ly);
}

static void msg_func_vomv(gsf_msg_t *req, int isize, gsf_msg_t *rsp, int *osize)
{
  rsp->err  = -1;
}

static msg_func_t *msg_func[GSF_ID_CODEC_END] = {
    [GSF_ID_MOD_CLI]      = NULL,
    [GSF_ID_CODEC_VORES]  = msg_func_vores,
    [GSF_ID_CODEC_VOLY]   = msg_func_voly,
    [GSF_ID_CODEC_VOMV]   = msg_func_vomv,
 };


int msg_func_proc(gsf_msg_t *req, int isize, gsf_msg_t *rsp, int *osize)
{
    if(req->id < 0 || req->id >= GSF_ID_CODEC_END)
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
