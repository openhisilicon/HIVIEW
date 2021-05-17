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

extern int vo_res_get(gsf_resolu_t *res);

static void msg_func_vores(gsf_msg_t *req, int isize, gsf_msg_t *rsp, int *osize)
{
  rsp->err  = 0;
  rsp->size = 0;
  
  if(req->set)
  {
    ;
    rsp->err  = 0;
    rsp->size = 0;
  }
  else
  {
    gsf_resolu_t *res = (gsf_resolu_t*)rsp->data;
    vo_res_get(res);
    rsp->err  = 0;
    rsp->size = sizeof(gsf_resolu_t);
  }
}

static void msg_func_voly(gsf_msg_t *req, int isize, gsf_msg_t *rsp, int *osize)
{
  if(req->set)
  {  
    gsf_layout_t *ly = (gsf_layout_t *)req->data;
    live_ly(ly);
  }
  else
  {
    ;
    rsp->err  = 0;
    rsp->size = 0;
  }
}

static void msg_func_vomv(gsf_msg_t *req, int isize, gsf_msg_t *rsp, int *osize)
{
  rsp->err  = -1;
}



static void msg_func_sdp(gsf_msg_t *req, int isize, gsf_msg_t *rsp, int *osize)
{
  gsf_sdp_t *sdp = (gsf_sdp_t*)rsp->data;
  
  sdp->video_shmid = venc_mgr[req->ch*3 + req->sid].video_shmid;
  sdp->audio_shmid = -1;
  sdp->venc = codec_nvr.venc[req->sid];

  sdp->val[0]  = venc_mgr[req->ch*GSF_CODEC_VENC_NUM + req->sid].val[0];
  sdp->val[1]  = venc_mgr[req->ch*GSF_CODEC_VENC_NUM + req->sid].val[1];
  sdp->val[2]  = venc_mgr[req->ch*GSF_CODEC_VENC_NUM + req->sid].val[2];
  sdp->val[3]  = venc_mgr[req->ch*GSF_CODEC_VENC_NUM + req->sid].val[3];
  printf("sid:%d, val.size[%d,%d,%d,%d]\n", req->sid,
        venc_mgr[req->ch*GSF_CODEC_VENC_NUM + req->sid].val[0].size,
        venc_mgr[req->ch*GSF_CODEC_VENC_NUM + req->sid].val[1].size,
        venc_mgr[req->ch*GSF_CODEC_VENC_NUM + req->sid].val[2].size,
        venc_mgr[req->ch*GSF_CODEC_VENC_NUM + req->sid].val[3].size);
  rsp->size = sizeof(gsf_sdp_t);
  rsp->err  = 0;
}

static void msg_func_idr(gsf_msg_t *req, int isize, gsf_msg_t *rsp, int *osize)
{
  rsp->size = 0;
  rsp->err  = gsf_mpp_venc_ctl(req->sid, GSF_MPP_VENC_CTL_IDR, NULL);
}

static int snap_cb(int idx, VENC_STREAM_S* pstStream, void* u)
{
  int i = 0;
  gsf_msg_t *rsp = (gsf_msg_t*)u;
  char name[64] = {0};
  sprintf(name, "/tmp/snap_%02d_%01d.jpg", rsp->ch, rsp->sid);
  FILE* pFile = fopen(name, "wb");
  
  for (i = 0; i < pstStream->u32PackCount; i++)
  {
    char *src = pstStream->pstPack[i].pu8Addr + pstStream->pstPack[i].u32Offset;
    int len = pstStream->pstPack[i].u32Len - pstStream->pstPack[i].u32Offset;
    if(pFile)
    {
      //printf("len:%d, rsp->size:%d\n", len, rsp->size);
      //memcpy(rsp->data+rsp->size, src, len);
      //rsp->size += len;
      fwrite(src, len, 1, pFile);
      fflush(pFile);
    }
  }
  
  if(pFile)
    fclose(pFile);
  
  strcpy(rsp->data, name);
  rsp->size = strlen(name)+1;
  printf("rsp->size:%d, rsp->data:[%s]\n", rsp->size, rsp->data);
  
  return 0;
}

static void msg_func_snap(gsf_msg_t *req, int isize, gsf_msg_t *rsp, int *osize)
{
  rsp->err  = 0;
  rsp->size = 0;
  rsp->ch = req->ch;
  rsp->sid = req->sid;
  gsf_mpp_venc_snap(rsp->ch*GSF_CODEC_VENC_NUM+GSF_CODEC_SNAP_IDX, 1, snap_cb, rsp);
  
  return;
}


static msg_func_t *msg_func[GSF_ID_CODEC_END] = {
    [GSF_ID_MOD_CLI]      = NULL,
    [GSF_ID_CODEC_VORES]  = msg_func_vores,
    [GSF_ID_CODEC_VOLY]   = msg_func_voly,
    [GSF_ID_CODEC_VOMV]   = msg_func_vomv,
    [GSF_ID_CODEC_SDP]    = msg_func_sdp,
    [GSF_ID_CODEC_IDR]    = msg_func_idr,
    [GSF_ID_CODEC_SNAP]   = msg_func_snap,
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
