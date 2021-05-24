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
    gsf_svp_t *cfg = (gsf_svp_t*)req->data;
    
    svp_cfg = *cfg;
    
    // change alg;
    json_parm_save(svp_parm_path, &svp_cfg);
      
    rsp->err  = 0;
    rsp->size = 0;
  }
  else
  {
    gsf_svp_t *cfg = (gsf_svp_t*)rsp->data;
    
    memcpy(cfg, &svp_cfg, sizeof(svp_cfg));
    
    rsp->err  = 0;
    rsp->size = sizeof(svp_cfg);
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


static void msg_func_face(gsf_msg_t *req, int isize, gsf_msg_t *rsp, int *osize)
{
  int ret = 0;
  rsp->size = 0;
  rsp->err = 0;
  
  if(strlen(req->data))
  {
    extern int person_face_add(char* filename);
    
    char oldname[256] = {0};
    char newname[256] = {0};
    strncpy(oldname, req->data, sizeof(oldname)-1);
    sprintf(newname, "/app/face/list/%s", basename(oldname));
    
    char cmd[256] = {0};
    snprintf(cmd, sizeof(cmd), "mv %s %s", req->data, newname);
    system(cmd);

    printf("mv oldname[%s] => newname:[%s]\n", req->data, newname);
    rsp->err = person_face_add(newname);
    if(rsp->err)
    {
      //unlink(newname);
    }
  }
}




static msg_func_t *msg_func[GSF_ID_SVP_END] = {
    [GSF_ID_SVP_CFG]    = msg_func_cfg,
    [GSF_ID_SVP_MD]     = msg_func_md,
    [GSF_ID_SVP_LPR]    = msg_func_lpr,
    [GSF_ID_SVP_FACE]   = msg_func_face,
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
