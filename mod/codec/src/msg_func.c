//os

//top
#include "inc/gsf.h"

//mod
#include "codec.h"

//myself
#include "msg_func.h"

static void msg_func_venc(gsf_msg_t *req, int isize, gsf_msg_t *rsp, int *osize)
{
}

static void msg_func_aenc(gsf_msg_t *req, int isize, gsf_msg_t *rsp, int *osize)
{
}

static void msg_func_stctl(gsf_msg_t *req, int isize, gsf_msg_t *rsp, int *osize)
{
}


static msg_func_t *msg_func[GSF_ID_CODEC_END] = {
    [GSF_ID_MOD_CLI]      = NULL,
    [GSF_ID_CODEC_VENC]   = msg_func_venc,
    [GSF_ID_CODEC_AENC]   = msg_func_aenc,
    [GSF_ID_CODEC_STCTL]  = msg_func_stctl,
    [GSF_ID_CODEC_OSD]    = NULL,
    [GSF_ID_CODEC_VMASK]  = NULL,
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
