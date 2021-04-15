//os

//top
#include "inc/gsf.h"

//mod
#include "rtsps.h"

//myself
#include "msg_func.h"
#include "rtsp-st.h"
#include "cfg.h"


static rtsp_ctl(gsf_msg_t *req, int isize, gsf_msg_t *rsp, int *osize)
{
  char buf[2048] = {0};
  struct rtsp_st_ctl_t *ctl = (struct rtsp_st_ctl_t *)buf;
  
  ctl->id = req->id;
  memcpy(ctl->data, req->data, req->size);
  ctl->size = req->size;
  
  int ret = rtsp_st_ctl(ctl);
  if(ret == 0)
  {
    memcpy(rsp->data, ctl->data, ctl->size);
    rsp->size = ctl->size;
    rsp->err = ctl->err;
  }
  else
  {
    rsp->size = 0;
    rsp->err = -1;
  }
}

static void msg_func_cfg(gsf_msg_t *req, int isize, gsf_msg_t *rsp, int *osize)
{
  if(req->set)
  {
    rtsps_parm = *((gsf_rtsps_t*)req->data);
    json_parm_save(rtsps_parm_path, &rtsps_parm);
    rsp->err  = 0;
    rsp->size = 0;
  }
  else
  {
    *((gsf_rtsps_t*)rsp->data) = rtsps_parm;
    rsp->err  = 0;
    rsp->size = sizeof(gsf_rtsps_t);
  }
}

static void msg_func_open(gsf_msg_t *req, int isize, gsf_msg_t *rsp, int *osize)
{
  rtsp_ctl(req, isize, rsp, osize);
}

static void msg_func_close(gsf_msg_t *req, int isize, gsf_msg_t *rsp, int *osize)
{
  rtsp_ctl(req, isize, rsp, osize);
}

static void msg_func_list(gsf_msg_t *req, int isize, gsf_msg_t *rsp, int *osize)
{
  rtsp_ctl(req, isize, rsp, osize);
}

static void msg_func_clear(gsf_msg_t *req, int isize, gsf_msg_t *rsp, int *osize)
{
  rtsp_ctl(req, isize, rsp, osize);
}


static void msg_func_popen(gsf_msg_t *req, int isize, gsf_msg_t *rsp, int *osize)
{
  rtsp_ctl(req, isize, rsp, osize);
}

static void msg_func_pclose(gsf_msg_t *req, int isize, gsf_msg_t *rsp, int *osize)
{
  rtsp_ctl(req, isize, rsp, osize);
}

static void msg_func_plist(gsf_msg_t *req, int isize, gsf_msg_t *rsp, int *osize)
{
  rtsp_ctl(req, isize, rsp, osize);
}

static void msg_func_pclear(gsf_msg_t *req, int isize, gsf_msg_t *rsp, int *osize)
{
  rtsp_ctl(req, isize, rsp, osize);
}

static msg_func_t *msg_func[GSF_ID_RTSPS_END] = {
    [GSF_ID_MOD_CLI]        = NULL,
    [GSF_ID_RTSPS_CFG]      = msg_func_cfg,
    [GSF_ID_RTSPS_C_OPEN]   = msg_func_open,
    [GSF_ID_RTSPS_C_CLOSE]  = msg_func_close,
    [GSF_ID_RTSPS_C_LIST]   = msg_func_list,
    [GSF_ID_RTSPS_C_CLEAR]  = msg_func_clear,
    [GSF_ID_RTSPS_P_OPEN]   = msg_func_popen,
    [GSF_ID_RTSPS_P_CLOSE]  = msg_func_pclose,
    [GSF_ID_RTSPS_P_LIST]   = msg_func_plist,
    [GSF_ID_RTSPS_P_CLEAR]  = msg_func_pclear,
};


int msg_func_proc(gsf_msg_t *req, int isize, gsf_msg_t *rsp, int *osize)
{
    if(req->id < 0 || req->id >= GSF_ID_RTSPS_END)
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
