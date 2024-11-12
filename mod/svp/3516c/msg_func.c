//os
#include <errno.h>
#include <stdio.h>
#include <libgen.h>
 
//top
#include "inc/gsf.h"

//mod
#include "svp.h"
#include "mod/app/inc/app.h"

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
    
    if(1)//sync lines to app.exe;
    {    
      GSF_MSG_DEF(gsf_polygons_t, lines, 8*1024);
      lines->polygon_num = 1;
      lines->polygons[0].point_num = yolocfg->det_polygon.polygons[0].point_num;
      memcpy(lines->polygons[0].points, yolocfg->det_polygon.polygons[0].points, sizeof(lines->polygons[0].points));
      int ret = GSF_MSG_SENDTO(GSF_ID_APP_IOU, 0, SET, 0, sizeof(gsf_polygons_t), GSF_IPC_APP, 2000);
    }
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


static void msg_func_face(gsf_msg_t *req, int isize, gsf_msg_t *rsp, int *osize)
{
  int ret = 0;
  rsp->size = 0;
  rsp->err = 0;
  #if 0
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
  #endif
}




static msg_func_t *msg_func[GSF_ID_SVP_END] = {
    [GSF_ID_SVP_CFG]    = msg_func_cfg,
    [GSF_ID_SVP_MD]     = msg_func_md,
    [GSF_ID_SVP_LPR]    = msg_func_lpr,
    [GSF_ID_SVP_YOLO]   = msg_func_yolo,
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
