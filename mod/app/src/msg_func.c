//os

//top
#include "inc/gsf.h"

//mod
#include "app.h"

//myself
#include "cfg.h"
#include "msg_func.h"

static void msg_func_chsrc(gsf_msg_t *req, int isize, gsf_msg_t *rsp, int *osize)
{
  if(req->set && req->ch >= 0 && req->ch < GSF_CODEC_NVR_CHN)
  {
    gsf_chsrc_t *chsrc = (gsf_chsrc_t*)req->data;
    
    app_nvr.chsrc[req->ch] = *chsrc;
    
    // add chsrc to live-mon-task;
    
    json_parm_save(app_parm_path, &app_nvr);
      
    rsp->err  = 0;
    rsp->size = 0;
  }
  else
  {
    gsf_chsrc_t *chsrc = (gsf_chsrc_t*)rsp->data;
    
    memcpy(chsrc, app_nvr.chsrc, sizeof(gsf_chsrc_t)*GSF_CODEC_NVR_CHN);
    
    rsp->err  = 0;
    rsp->size = sizeof(gsf_chsrc_t)*GSF_CODEC_NVR_CHN;
  }
}


static void msg_func_gui(gsf_msg_t *req, int isize, gsf_msg_t *rsp, int *osize)
{
  if(req->set)
  {
    gsf_gui_t *gui = (gsf_gui_t*)req->data;
    app_nvr.gui = *gui;
    json_parm_save(app_parm_path, &app_nvr);
    rsp->err  = 0;
    rsp->size = 0;
  }
  else
  {
    gsf_gui_t *gui = (gsf_gui_t*)rsp->data;
    *gui = app_nvr.gui;
    rsp->err  = 0;
    rsp->size = sizeof(gsf_gui_t);
  }
}



static msg_func_t *msg_func[GSF_ID_APP_END] = {
    [GSF_ID_MOD_CLI]      = NULL,
    [GSF_ID_APP_CHSRC]    = msg_func_chsrc,
    [GSF_ID_APP_GUI]      = msg_func_gui,
 };


int msg_func_proc(gsf_msg_t *req, int isize, gsf_msg_t *rsp, int *osize)
{
    if(req->id < 0 || req->id >= GSF_ID_APP_END)
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
