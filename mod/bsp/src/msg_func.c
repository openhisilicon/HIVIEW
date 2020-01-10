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
  if(req->set)
  {
    rsp->size = 0;
    rsp->err = rtc_set((gsf_time_t*)req->data);
  }
  else
  {
    rsp->size = sizeof(gsf_time_t);
    rsp->err = rtc_get((gsf_time_t *)rsp->data);
  }
}

static void msg_func_eth(gsf_msg_t *req, int isize, gsf_msg_t *rsp, int *osize)
{
  if(req->set)
  {
    rsp->size = 0;
    rsp->err = netinf_eth_set((gsf_eth_t*)req->data);
    if(rsp->err == 0)
    {
      bsp_parm.eth = *((gsf_eth_t*)req->data);
      json_parm_save(bsp_parm_path, &bsp_parm);
    }
  }
  else
  {
    rsp->size = sizeof(gsf_eth_t);
    rsp->err = netinf_eth_get((gsf_eth_t *)rsp->data);
  }
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
  nm_pub_send(bsp_pub, (char*)&msg, sizeof(msg));
  return 0;
}

static void msg_func_upg(gsf_msg_t *req, int isize, gsf_msg_t *rsp, int *osize)
{
  int ret = 0;
  rsp->size = 0;
  
  rsp->err = upg_start(req->data, upg_cb, NULL);
}

static void msg_func_def(gsf_msg_t *req, int isize, gsf_msg_t *rsp, int *osize)
{
  gsf_bsp_def_t *cfg = (gsf_bsp_def_t*)rsp->data;
  
  *cfg = bsp_def;
  rsp->size = sizeof(gsf_bsp_def_t);
}


static void msg_func_ntp(gsf_msg_t *req, int isize, gsf_msg_t *rsp, int *osize)
{
  if(req->set)
  {
    rsp->size = 0;
    rsp->err = ntp_set((gsf_ntp_t*)req->data);
    if(rsp->err == 0)
    {
      bsp_parm.ntp = *((gsf_ntp_t*)req->data);
      json_parm_save(bsp_parm_path, &bsp_parm);
    }
  }
  else
  {
    gsf_ntp_t *ntp = (gsf_ntp_t*)rsp->data;
    *ntp = bsp_parm.ntp;
    rsp->size = sizeof(gsf_ntp_t);
    rsp->err = 0;
  }
}

static void msg_func_wifi(gsf_msg_t *req, int isize, gsf_msg_t *rsp, int *osize)
{
  if(req->set)
  {
    rsp->size = 0;
    rsp->err = netinf_wifi_set((gsf_wifi_t*)req->data);
    if(rsp->err == 0)
    {
      bsp_parm.wifi = *((gsf_wifi_t*)req->data);
      json_parm_save(bsp_parm_path, &bsp_parm);
    }
  }
  else
  {
    gsf_wifi_t *wifi = (gsf_wifi_t*)rsp->data;
    *wifi = bsp_parm.wifi;
    rsp->size = sizeof(gsf_wifi_t);
    rsp->err = 0;
  }
}

static void msg_func_wlist(gsf_msg_t *req, int isize, gsf_msg_t *rsp, int *osize)
{
  gsf_wifi_list_t *wls = (gsf_wifi_list_t*)rsp->data;
  
  int cnt = netinf_wifi_list(wls);
  
  if(cnt >= 0)
  {
    rsp->size = sizeof(gsf_wifi_list_t)*cnt;
    rsp->err = 0;
  }
  else
  {
    rsp->err = cnt;  
  }
}

static void msg_func_base(gsf_msg_t *req, int isize, gsf_msg_t *rsp, int *osize)
{
  if(req->set)
  {
    gsf_base_t *base = (gsf_base_t*)req->data;
    
    rsp->size = 0;
    rsp->err = zone_set(base->zone);
    if(rsp->err == 0)
    {
      bsp_parm.base = *base;
      json_parm_save(bsp_parm_path, &bsp_parm);
    }
  }
  else
  {
    gsf_base_t *base = (gsf_base_t*)rsp->data;
    *base = bsp_parm.base;
    rsp->size = sizeof(gsf_base_t);
    rsp->err = 0;
  }
}


static void msg_func_user(gsf_msg_t *req, int isize, gsf_msg_t *rsp, int *osize)
{
  if(req->set)
  {
    memcpy(bsp_parm.users, req->data, sizeof(bsp_parm.users));
    json_parm_save(bsp_parm_path, &bsp_parm);
    rsp->size = 0;
    rsp->err = 0;
  }
  else
  {
    memcpy(rsp->data, bsp_parm.users, sizeof(bsp_parm.users));
    rsp->size = sizeof(bsp_parm.users);
    rsp->err = 0;
  }
}


static msg_func_t *msg_func[GSF_ID_BSP_END] = {
    [GSF_ID_MOD_CLI]    = msg_func_cli,
    [GSF_ID_BSP_TIME]   = msg_func_time,
    [GSF_ID_BSP_ETH]    = msg_func_eth,
    [GSF_ID_BSP_UPG]    = msg_func_upg,
    [GSF_ID_BSP_DEF]    = msg_func_def,
    [GSF_ID_BSP_NTP]    = msg_func_ntp,
    [GSF_ID_BSP_WIFI]   = msg_func_wifi,
    [GSF_ID_BSP_WLIST]  = msg_func_wlist,
    [GSF_ID_BSP_BASE]   = msg_func_base,
    [GSF_ID_BSP_USER]   = msg_func_user,
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
