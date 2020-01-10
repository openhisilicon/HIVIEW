//os

//top
#include "inc/gsf.h"

//mod
#include "codec.h"
#include "cfg.h"

//myself
#include "msg_func.h"
#include "mpp.h"
#include "rgn.h"

extern int venc_start(int start);
static void msg_func_venc(gsf_msg_t *req, int isize, gsf_msg_t *rsp, int *osize)
{
  if(req->set && req->sid >= 0 && req->sid < GSF_CODEC_VENC_NUM)
  {
    gsf_venc_t *venc = (gsf_venc_t*)req->data;
    
    venc_start(0);
    codec_ipc.venc[req->sid] = *venc;
    if(venc_start(1) == 0)
      json_parm_save(codec_parm_path, &codec_ipc);
    rsp->err  = 0;
    rsp->size = 0;
  }
  else if(req->sid >= 0 && req->sid < GSF_CODEC_VENC_NUM)
  {
    gsf_venc_t *venc = (gsf_venc_t*)rsp->data;
    *venc = codec_ipc.venc[req->sid];
    rsp->err  = 0;
    rsp->size = sizeof(gsf_venc_t);
  }
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

static void msg_func_osd(gsf_msg_t *req, int isize, gsf_msg_t *rsp, int *osize)
{
  if(req->set && req->sid >= 0 && req->sid < GSF_CODEC_OSD_NUM)
  {
    gsf_osd_t *osd = (gsf_osd_t*)req->data;
    
    codec_ipc.osd[req->sid] = *osd;
    if(gsf_rgn_osd_set(0, req->sid, osd) == 0)
      json_parm_save(codec_parm_path, &codec_ipc);
    rsp->err  = 0;
    rsp->size = 0;
  }
  else if(req->sid >= 0 && req->sid < GSF_CODEC_OSD_NUM)
  {
    gsf_osd_t *osd = (gsf_osd_t*)rsp->data;
    *osd = codec_ipc.osd[req->sid];
    rsp->err  = 0;
    rsp->size = sizeof(gsf_osd_t);
  }
}

static void msg_func_vmask(gsf_msg_t *req, int isize, gsf_msg_t *rsp, int *osize)
{
  if(req->set && req->sid >= 0 && req->sid < GSF_CODEC_VMASK_NUM)
  {
    gsf_vmask_t *vmask = (gsf_vmask_t*)req->data;
    codec_ipc.vmask[req->sid] = *vmask;
    if(gsf_rgn_vmask_set(0, req->sid, vmask) == 0)
      json_parm_save(codec_parm_path, &codec_ipc);
    rsp->err  = 0;
    rsp->size = 0;
  }
  else if(req->sid >= 0 && req->sid < GSF_CODEC_VMASK_NUM)
  {
    gsf_vmask_t *vmask = (gsf_vmask_t*)rsp->data;
    *vmask = codec_ipc.vmask[req->sid];
    rsp->err  = 0;
    rsp->size = sizeof(gsf_vmask_t);
  }
}


static int snap_cb(int idx, VENC_STREAM_S* pstStream, void* u)
{
  int i = 0;
  gsf_msg_t *rsp = (gsf_msg_t*)u;
  char *name = "/tmp/snap.jpg";
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
  gsf_mpp_venc_snap(2, 1, snap_cb, rsp);
}




static msg_func_t *msg_func[GSF_ID_CODEC_END] = {
    [GSF_ID_MOD_CLI]      = NULL,
    [GSF_ID_CODEC_VENC]   = msg_func_venc,
    [GSF_ID_CODEC_AENC]   = msg_func_aenc,
    [GSF_ID_CODEC_SDP]    = msg_func_sdp,
    [GSF_ID_CODEC_IDR]    = msg_func_idr,
    [GSF_ID_CODEC_OSD]    = msg_func_osd,
    [GSF_ID_CODEC_VMASK]  = msg_func_vmask,
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
