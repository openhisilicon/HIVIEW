//os
#include <errno.h>
#include <stdio.h>
#include <libgen.h>
 
//top
#include "inc/gsf.h"

//mod
#include "svp.h"

//myself
#include "cfg.h"
#include "msg_func.h"

static void msg_func_cfg(gsf_msg_t *req, int isize, gsf_msg_t *rsp, int *osize)
{
  if(req->set)
  {
    gsf_svp_t *svp = (gsf_svp_t*)req->data;
    
    svp_parm.svp = *svp;
    
    // change alg;
    json_parm_save(svp_parm_path, &svp_parm);
      
    rsp->err  = 0;
    rsp->size = 0;
  }
  else
  {
    gsf_svp_t *svp = (gsf_svp_t*)rsp->data;
    
    memcpy(svp, &svp_parm.svp, sizeof(svp_parm.svp));
    
    rsp->err  = 0;
    rsp->size = sizeof(svp_parm.svp);
  }
}


static void msg_func_md(gsf_msg_t *req, int isize, gsf_msg_t *rsp, int *osize)
{
  if(req->set)
  {
    gsf_svp_md_t *mdcfg = (gsf_svp_md_t*)req->data;
    
    // start/stop md_alg(ch);
    rsp->err  = 0;
    rsp->size = 0;
  }
  else
  {
    gsf_svp_md_t *mdcfg = (gsf_svp_md_t*)rsp->data;
    
    //get mdcfg(ch);
    
    rsp->err  = 0;
    rsp->size = sizeof(gsf_svp_md_t);
  }
}

static void msg_func_lpr(gsf_msg_t *req, int isize, gsf_msg_t *rsp, int *osize)
{
  if(req->set)
  {
    gsf_svp_lpr_t *lprcfg = (gsf_svp_lpr_t*)req->data;
    
    // start/stop lpr_alg(ch);  
    rsp->err  = 0;
    rsp->size = 0;
  }
  else
  {
    gsf_svp_lpr_t *lprcfg = (gsf_svp_lpr_t*)rsp->data;
    
    //get lprcfg(ch);
    
    rsp->err  = 0;
    rsp->size = sizeof(gsf_svp_lpr_t);
  }
}

static void msg_func_yolo(gsf_msg_t *req, int isize, gsf_msg_t *rsp, int *osize)
{
  if(req->set)
  {
    gsf_svp_yolo_t *yolocfg = (gsf_svp_yolo_t*)req->data;
    
    // start/stop yolo_alg(ch);  

    svp_parm.yolo = *yolocfg;
    json_parm_save(svp_parm_path, &svp_parm);
    
    rsp->err  = 0;
    rsp->size = 0;
  }
  else
  {
    gsf_svp_yolo_t *yolocfg = (gsf_svp_yolo_t*)rsp->data;
    

    memcpy(yolocfg, &svp_parm.yolo, sizeof(svp_parm.yolo));
    //get yolocfg(ch);
    
    rsp->err  = 0;
    rsp->size = sizeof(gsf_svp_yolo_t);
  }
}

static msg_func_t *msg_func[GSF_ID_SVP_END] = {
    [GSF_ID_SVP_CFG]    = msg_func_cfg,
    [GSF_ID_SVP_MD]     = msg_func_md,
    [GSF_ID_SVP_LPR]    = msg_func_lpr,
    [GSF_ID_SVP_YOLO]   = msg_func_yolo,
 };


int msg_func_proc(gsf_msg_t *req, int isize, gsf_msg_t *rsp, int *osize)
{
    if(req->id < 0 || req->id >= GSF_ID_SVP_END)
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
