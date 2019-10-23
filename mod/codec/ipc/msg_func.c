//os

//top
#include "inc/gsf.h"

//mod
#include "codec.h"
#include "cfg.h"

//myself
#include "msg_func.h"
#include "mpp.h"

static void msg_func_venc(gsf_msg_t *req, int isize, gsf_msg_t *rsp, int *osize)
{
  gsf_venc_t *venc = (gsf_venc_t*)rsp->data;
  
  *venc = codec_ipc.venc[0];

  rsp->err  = 0;
  rsp->size = sizeof(gsf_venc_t);
}

static void msg_func_aenc(gsf_msg_t *req, int isize, gsf_msg_t *rsp, int *osize)
{
  rsp->err  = -1;
}


static void msg_func_sdp(gsf_msg_t *req, int isize, gsf_msg_t *rsp, int *osize)
{
  gsf_sdp_t *sdp = (gsf_sdp_t*)rsp->data;
  
  sdp->video_shmid = venc_mgr[req->sid].video_shmid;
  sdp->audio_shmid = -1;
  sdp->venc = codec_ipc.venc[req->sid];
  sdp->aenc = codec_ipc.aenc;
  sdp->val[0]  = venc_mgr[req->sid].val[0];
  sdp->val[1]  = venc_mgr[req->sid].val[1];
  sdp->val[2]  = venc_mgr[req->sid].val[2];
  sdp->val[3]  = venc_mgr[req->sid].val[3];
  printf("sid:%d, val.size[%d,%d,%d,%d]\n", req->sid,
        venc_mgr[req->sid].val[0].size,
        venc_mgr[req->sid].val[1].size,
        venc_mgr[req->sid].val[2].size,
        venc_mgr[req->sid].val[3].size);
  rsp->size = sizeof(gsf_sdp_t);
  rsp->err  = 0;
}

static void msg_func_idr(gsf_msg_t *req, int isize, gsf_msg_t *rsp, int *osize)
{
  rsp->size = 0;
  rsp->err  = gsf_mpp_venc_ctl(req->sid, GSF_MPP_VENC_CTL_IDR, NULL);
}


static msg_func_t *msg_func[GSF_ID_CODEC_END] = {
    [GSF_ID_MOD_CLI]      = NULL,
    [GSF_ID_CODEC_VENC]   = msg_func_venc,
    [GSF_ID_CODEC_AENC]   = msg_func_aenc,
    [GSF_ID_CODEC_SDP]    = msg_func_sdp,
    [GSF_ID_CODEC_IDR]    = msg_func_idr,
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
