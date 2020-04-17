//os

//top
#include "inc/gsf.h"

//mod
#include "rtsps.h"

//myself
#include "msg_func.h"
#include "rtsp-st.h"

static void msg_func_cfg(gsf_msg_t *req, int isize, gsf_msg_t *rsp, int *osize)
{
  ;
}


static void msg_func_open(gsf_msg_t *req, int isize, gsf_msg_t *rsp, int *osize)
{
  gsf_rtsp_url_t *ru = (gsf_rtsp_url_t*)req->data;
  gsf_shmid_t  *shmid = (gsf_shmid_t*)rsp->data;
  
  char buf[1024] = {0};
  struct rtsp_st_ctl_t *ctl = (struct rtsp_st_ctl_t *)buf;
  ctl->id = GSF_ID_RTSPS_C_OPEN;
  sprintf(ctl->data, "%s", ru->url);
  ctl->size = strlen(ctl->data)+1;
  rtsp_st_ctl(ctl);
  if(ctl->size > 0)
  {
    *shmid = *((gsf_shmid_t*)ctl->data);
    printf("url:%s, shmid->video_shmid:%d\n", ru->url, shmid->video_shmid);
    rsp->size = sizeof(gsf_shmid_t);
  }
  else
  {
    rsp->size = 0;
    rsp->err = -1;
  }
}

static void msg_func_close(gsf_msg_t *req, int isize, gsf_msg_t *rsp, int *osize)
{
  gsf_rtsp_url_t *ru = (gsf_rtsp_url_t*)req->data;
  
  char buf[1024] = {0};
  struct rtsp_st_ctl_t *ctl = (struct rtsp_st_ctl_t *)buf;
  ctl->id = GSF_ID_RTSPS_C_CLOSE;
  sprintf(ctl->data, "%s", ru->url);
  ctl->size = strlen(ctl->data)+1;
  rtsp_st_ctl(ctl);
  rsp->size = 0;
}

static void msg_func_list(gsf_msg_t *req, int isize, gsf_msg_t *rsp, int *osize)
{
  //gsf_rtsp_conn_t*;
}

static void msg_func_clear(gsf_msg_t *req, int isize, gsf_msg_t *rsp, int *osize)
{
  ;
}

static msg_func_t *msg_func[GSF_ID_RTSPS_END] = {
    [GSF_ID_MOD_CLI]        = NULL,
    [GSF_ID_RTSPS_CFG]      = msg_func_cfg,
    [GSF_ID_RTSPS_C_OPEN]   = msg_func_open,
    [GSF_ID_RTSPS_C_CLOSE]  = msg_func_close,
    [GSF_ID_RTSPS_C_LIST]   = msg_func_list,
    [GSF_ID_RTSPS_C_CLEAR]  = msg_func_clear,
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
