//os

//top
#include "inc/gsf.h"

//mod
#include "srts.h"

//myself
#include "msg_func.h"

extern int srts_open(int ch, int st, char *url);
extern int srts_close();

static void msg_func_cfg(gsf_msg_t *req, int isize, gsf_msg_t *rsp, int *osize)
{
  if(req->set)
  {
    srts_parm = *((gsf_srts_t*)req->data);
    json_parm_save(srts_parm_path, &srts_parm);
    rsp->err  = 0;
    rsp->size = 0;
  }
  else
  {
    *((gsf_srts_t*)rsp->data) = srts_parm;
    rsp->err  = 0;
    rsp->size = sizeof(gsf_srts_t);
  }
}

static void msg_func_ctl(gsf_msg_t *req, int isize, gsf_msg_t *rsp, int *osize)
{
  int ret = 0;
  
  if(req->set)
  {
    gsf_srts_ctl_t* ctl = (gsf_srts_ctl_t*)req->data;
    ret = srts_close();
    ret = srts_open(ctl->ch, ctl->st, ctl->dst);
    rsp->err  = ret;
    rsp->size = 0;
  }
  else
  {
    rsp->err  = 0;
    rsp->size = 0;
  }
}

static void msg_func_recv(gsf_msg_t *req, int isize, gsf_msg_t *rsp, int *osize)
{
  int ret = 0;
  
  extern int recv_open(int ch, int st, char *url);
  extern int recv_close();
  extern int recv_shmid_get(gsf_shmid_t *shmid);
  
  gsf_srts_ctl_t* ctl = (gsf_srts_ctl_t*)req->data;
  ret = recv_close();
  ret = recv_open(ctl->ch, ctl->st, ctl->dst);
  
  rsp->err  = ret;
  recv_shmid_get((gsf_shmid_t*)rsp->data);
  rsp->size = sizeof(gsf_shmid_t);
}


static msg_func_t *msg_func[GSF_ID_SRTS_END] = {
    [GSF_ID_MOD_CLI]       = NULL,
    [GSF_ID_SRTS_CFG]      = msg_func_cfg,
    [GSF_ID_SRTS_CTL]      = msg_func_ctl,
    [GSF_ID_SRTS_RECV]      = msg_func_recv,
 };


int msg_func_proc(gsf_msg_t *req, int isize, gsf_msg_t *rsp, int *osize)
{
    if(req->id < 0 || req->id >= GSF_ID_SRTS_END)
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
