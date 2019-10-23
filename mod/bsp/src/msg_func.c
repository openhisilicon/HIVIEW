//os

//top
#include "inc/gsf.h"

//mod
#include "bsp.h"
#include "cfg.h"
#include "netinf.h"   // netinf
#include "wdg.h"      // watchdog
#include "rtc.h"      // rtc
#include "sadp.h"     // discover
#include "upg.h"      // upgrade
//myself
#include "msg_func.h"

gsf_mod_reg_t gmods[GSF_MOD_ID_END] = {
  [GSF_MOD_ID_BSP] = {.uri = GSF_IPC_BSP},
  };

static void msg_func_cli(gsf_msg_t *req, int isize, gsf_msg_t *rsp, int *osize)
{
  int ret = 0;
  switch(req->sid)
  {
    case GSF_CLI_POWEROFF:
      {
        // sendto all mod, poweroff;
        // halt system;
      }
      break;
    case GSF_CLI_RESTART:
      {
        // sendto all mod, reboot;
        // reboot system;
      }
      break;
    case GSF_CLI_REGISTER:
      {
        // record mod url to gmods;
        gsf_mod_reg_t *reg = (gsf_mod_reg_t*)req->data;
        printf("REGISTER [mid:%d, uri:%s]\n", reg->mid, reg->uri);
        gmods[reg->mid] = *reg;
        rsp->size = sizeof(gmods);
        memcpy(rsp->data, gmods, sizeof(gmods));
      }
      break;
    default:
      break;
  }
}

static void msg_func_time(gsf_msg_t *req, int isize, gsf_msg_t *rsp, int *osize)
{
  int ret = 0;
  rsp->size = 0;
  rsp->err = rtc_set((gsf_time_t*)req->data);
}

static void msg_func_eth(gsf_msg_t *req, int isize, gsf_msg_t *rsp, int *osize)
{
  int ret = 0;
  rsp->size = 0;
  rsp->err = netinf_eth_set((gsf_eth_t*)req->data);
}

extern void* bsp_pub;

int upg_cb(int progress, void *u)
{
  gsf_msg_t msg;
  memset(&msg, 0, sizeof(msg));
  msg.id = GSF_EV_UPG;
  msg.ts = time(NULL)*1000;
  msg.sid = 0;
  msg.err = progress;
  msg.size = 0;
  nm_pub_send(bsp_pub, &msg, sizeof(msg));
  return 0;
}

static void msg_func_upg(gsf_msg_t *req, int isize, gsf_msg_t *rsp, int *osize)
{
  int ret = 0;
  rsp->size = 0;
  
  rsp->err = upg_start(req->data, upg_cb, NULL);
}

static void msg_func_cfg(gsf_msg_t *req, int isize, gsf_msg_t *rsp, int *osize)
{
  gsf_bsp_def_t *cfg = (gsf_bsp_def_t*)rsp->data;
  
  *cfg = bsp_def;
  rsp->size = sizeof(gsf_bsp_def_t);
}

static msg_func_t *msg_func[GSF_ID_BSP_END] = {
    [GSF_ID_MOD_CLI]    = msg_func_cli,
    [GSF_ID_BSP_TIME]   = msg_func_time,
    [GSF_ID_BSP_ETH]    = msg_func_eth,
    [GSF_ID_BSP_UPG]    = msg_func_upg,
    [GSF_ID_BSP_DEF]    = msg_func_cfg,
};


int msg_func_proc(gsf_msg_t *req, int isize, gsf_msg_t *rsp, int *osize)
{
    if(req->id < 0 || req->id >= GSF_ID_BSP_END)
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
