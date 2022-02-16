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
#include "lens.h"
#include "venc.h"

extern int venc_start(int start);
extern int vo_res_get(gsf_resolu_t *res);
extern int vo_ly_get(gsf_layout_t *ly);

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
  
  sdp->video_shmid = venc_mgr[req->ch*3 + req->sid].video_shmid;
  sdp->audio_shmid = audio_shmid;
  sdp->venc = codec_ipc.venc[req->sid];
  sdp->aenc = codec_ipc.aenc;
  
  if(req->ch == 1)
  {
    extern int second_sdp(int i, gsf_sdp_t *sdp);
    second_sdp(req->ch, sdp);
  }
  
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

static void msg_func_osd(gsf_msg_t *req, int isize, gsf_msg_t *rsp, int *osize)
{
  if(req->set && req->sid >= 0 && req->sid < GSF_CODEC_OSD_NUM)
  {
    gsf_osd_t *osd = (gsf_osd_t*)req->data;
    
    codec_ipc.osd[req->sid] = *osd;
    if(gsf_rgn_osd_set(req->ch, req->sid, osd) == 0)
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
  return;
}


static void msg_func_voly(gsf_msg_t *req, int isize, gsf_msg_t *rsp, int *osize)
{
  if(req->set)
  {
    ;
    rsp->err  = 0;
    rsp->size = 0;
  }
  else
  {
    gsf_layout_t *ly = (gsf_layout_t *)rsp->data;
    vo_ly_get(ly);
    rsp->err  = 0;
    rsp->size = sizeof(gsf_layout_t);
  }
}



static void msg_func_lens(gsf_msg_t *req, int isize, gsf_msg_t *rsp, int *osize)
{
  rsp->err  = 0;
  rsp->size = 0;
  rsp->ch = req->ch;
  rsp->sid = req->sid;

  gsf_lens_t *lens = (gsf_lens_t*)req->data;
  switch(lens->cmd)
  {
    case GSF_LENS_IRC:
      rsp->err = gsf_lens_ircut(req->ch, lens->arg1);
      break;
    case GSF_LENS_STOP:
      rsp->err = gsf_lens_stop(req->ch);
      break;
    case GSF_LENS_ZOOM:
      rsp->err = gsf_lens_zoom(req->ch, lens->arg1, lens->arg2);
      break;
    case GSF_LENS_FOCUS:
      rsp->err = gsf_lens_focus(req->ch, lens->arg1, lens->arg2);
      break;
    case GSF_LENS_CAL:
      rsp->err = gsf_lens_cal(req->ch);
      break;
  }
  return;
}

#if defined(GSF_CPU_3516d)
static void msg_func_imgall(gsf_msg_t *req, int isize, gsf_msg_t *rsp, int *osize)
{
  gsf_img_all_t *all = (gsf_img_all_t*)rsp->data;
  
  gsf_mpp_isp_ctl(0, GSF_MPP_ISP_CTL_IMG, all);
  rsp->err  = 0;
  rsp->size = sizeof(gsf_img_all_t);
  printf("get all\n");
  
}
static void msg_func_imgcsc(gsf_msg_t *req, int isize, gsf_msg_t *rsp, int *osize)
{
  if(req->set)
  {
    gsf_img_csc_t *csc = (gsf_img_csc_t*)req->data;
    
    //codec_ipc.csc = *csc;
    //json_parm_save(codec_parm_path, &codec_ipc);
    gsf_mpp_isp_ctl(0, GSF_MPP_ISP_CTL_CSC, csc);
    rsp->err  = 0;
    rsp->size = 0;
    printf("set csc\n");
  }
  else
  {
    gsf_img_csc_t *csc = (gsf_img_csc_t*)rsp->data;
    
    //*csc = codec_ipc.csc;
    rsp->err  = 0;
    rsp->size = sizeof(gsf_img_csc_t);
    printf("get csc\n");
  } 
}
static void msg_func_imgae(gsf_msg_t *req, int isize, gsf_msg_t *rsp, int *osize)
{
  if(req->set)
  {
    gsf_img_ae_t *ae = (gsf_img_ae_t*)req->data;
    
    //codec_ipc.ae = *ae;
    //json_parm_save(codec_parm_path, &codec_ipc);
    gsf_mpp_isp_ctl(0, GSF_MPP_ISP_CTL_AE, ae);
    rsp->err  = 0;
    rsp->size = 0;
    printf("set ae\n");
  }
  else
  {
    gsf_img_ae_t *ae = (gsf_img_ae_t*)rsp->data;
    
    //*ae = codec_ipc.ae;
    rsp->err  = 0;
    rsp->size = sizeof(gsf_img_ae_t);
    printf("get ae\n");
  } 
}

static void msg_func_imgdehaze(gsf_msg_t *req, int isize, gsf_msg_t *rsp, int *osize)
{
  if(req->set)
  {
    gsf_img_dehaze_t *dehaze = (gsf_img_dehaze_t*)req->data;
    
    //codec_ipc.dehaze = *dehaze;
    //json_parm_save(codec_parm_path, &codec_ipc);
    gsf_mpp_isp_ctl(0, GSF_MPP_ISP_CTL_DEHAZE, dehaze);
    rsp->err  = 0;
    rsp->size = 0;
    printf("set dehaze\n");
  }
  else
  {
    gsf_img_dehaze_t *dehaze = (gsf_img_dehaze_t*)rsp->data;
    
    //*dehaze = codec_ipc.dehaze;
    rsp->err  = 0;
    rsp->size = sizeof(gsf_img_dehaze_t);
    printf("get dehaze\n");
  } 
}
#endif

static void msg_func_lenscfg(gsf_msg_t *req, int isize, gsf_msg_t *rsp, int *osize)
{
  if(req->set)
  {
    gsf_lenscfg_t *lenscfg = (gsf_lenscfg_t*)req->data;
    
    codec_ipc.lenscfg = *lenscfg;
    json_parm_save(codec_parm_path, &codec_ipc);
    rsp->err  = 0;
    rsp->size = 0;
    printf("set lenscfg\n");
  }
  else
  {
    gsf_lenscfg_t *lenscfg = (gsf_lenscfg_t*)rsp->data;
    
    *lenscfg = codec_ipc.lenscfg;
    rsp->err  = 0;
    rsp->size = sizeof(gsf_lenscfg_t);
    printf("get lenscfg\n");
  } 
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
    [GSF_ID_CODEC_VORES]  = msg_func_vores,
    [GSF_ID_CODEC_VOLY]   = msg_func_voly,
    [GSF_ID_CODEC_LENS]   = msg_func_lens,
    [GSF_ID_CODEC_LENSCFG]= msg_func_lenscfg,
#if defined(GSF_CPU_3516d)    
    [GSF_ID_CODEC_IMGALL]= msg_func_imgall,
    [GSF_ID_CODEC_IMGCSC]= msg_func_imgcsc,
    [GSF_ID_CODEC_IMGAE]= msg_func_imgae,
    [GSF_ID_CODEC_IMGDEHAZE]= msg_func_imgdehaze,
#endif    
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
