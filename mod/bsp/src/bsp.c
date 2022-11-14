#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "inc/gsf.h"
#include "bsp.h"
#include "fw/comm/inc/proc.h"
#include "cfg.h"      // config
#include "netinf.h"   // netinf
#include "wdg.h"      // watchdog
#include "rtc.h"      // rtc
#include "sadp.h"     // discover
#include "upg.h"      // upgrade

#include "msg_func.h"
#include "fw/comm/inc/netcfg.h"

#define SADP_ADDR "238.238.238.238"

GSF_LOG_GLOBAL_INIT("BSP", 8*1024);

void* bsp_pub = NULL;

extern gsf_mod_reg_t gmods[GSF_MOD_ID_END];

char home_path[128] = {0};

static int log_recv(char *msg, int size, int err)
{
    static int cnt = 1;
    static int l = 0;
    static int f = 0;
    static char *logfile[3] = { "/tmp/log0.log", "/tmp/log1.log", "/tmp/log2.log"};
    
    if(!f)
    {
      f = open(logfile[1], O_RDWR|O_CREAT|O_TRUNC, 0766); close(f);
      f = open(logfile[2], O_RDWR|O_CREAT|O_TRUNC, 0766); close(f);
      f = open(logfile[l], O_RDWR|O_CREAT|O_TRUNC, 0766);
    }
    
    if(f)
    {
      //fprintf(stderr, "L[%s]", msg);
      write(f, msg, size);
      if(++cnt % 128 == 0)
      {
        struct stat st;
        if(fstat(f, &st) == 0 && st.st_size > 128*1024)
        {
          close(f);
          f = open(logfile[++l%3], O_RDWR|O_CREAT|O_TRUNC, 0766);
        }
      }
    }
    return 0;
}

static int req_recv(char *in, int isize, char *out, int *osize, int err)
{
    int ret = 0;
    gsf_msg_t *req = (gsf_msg_t*)in;
    gsf_msg_t *rsp = (gsf_msg_t*)out;

    *rsp  = *req;
    rsp->err  = 0;
    rsp->size = 0;

    printf("req->id:%d, isize:%d\n", req->id, isize);

    ret = msg_func_proc(req, isize, rsp, osize);

    rsp->err = (ret == TRUE)?rsp->err:GSF_ERR_MSG;
    *osize = sizeof(gsf_msg_t)+rsp->size;
    
    printf("req->id:%d, ret:%d, rsp->err:%d\n", req->id, ret, rsp->err);


    return 0;
}

static int sadp_recv_func(gsf_sadp_msg_t *in, gsf_msg_t* out
          , int *osize/*IN&OUT*/, gsf_sadp_peer_t* peer)
{
  int ret = 0;
  
  printf("in->ver:%04X, modid:%d, msg.size:%d, peer:[%s:%d]\n"
        , in->ver, in->modid, in->msg.size, peer->ipaddr, peer->port);

  if(!(in->ver&GSF_SADP_VER_REQ) || in->modid < 0 || in->modid >= GSF_MOD_ID_END)
  {
    printf("err, ver:%04X, modid:%d.\n", in->ver, in->modid);
    *osize = 0;
    return 0;
  }

  char _mac[6] = {0}; //gen devid;
  netcfg_get_mac_addr("eth0", _mac);
  int devid = _mac[5] << 24 | _mac[4] << 16 | _mac[3] << 8 | _mac[2] << 0;
  
  if(in->ver&GSF_SADP_VER_MC && in->devid && in->devid != devid)
  {
    printf("err, in->devid:0x%08x, myself:0x%08x\n", in->devid, devid);
    *osize = 0;
    return 0;
  }
  
  ret = nm_req_sendto(gmods[in->modid].uri, 3, (char*)&in->msg, sizeof(gsf_msg_t)+in->msg.size, (char*)out, osize);
  if(ret < 0)
  {
    printf("nm_req_sendto(%s) err.\n", gmods[in->modid].uri);
    *osize = 0;
    return 0;
  }

  if(in->ver & GSF_SADP_VER_MC)
  {
    char buf[16*1024] = {0};
    gsf_sadp_msg_t *req = (gsf_sadp_msg_t*)buf;
    gsf_sadp_peer_t dst = {SADP_ADDR, 8888};
    
    req->ver   = 0;
    req->modid = in->modid;
    req->devid = devid;

    memcpy(&req->msg, out, *osize);
    
    printf("send MC to %s:8888, modid:%d, msgid:%d\n", SADP_ADDR, in->modid, out->id);
    sadp_cu_gset(&dst, req, NULL, NULL, 0);
    
    *osize = 0;
    return 0;
  }

  printf("send UC to %s:%d, modid:%d, msgid:%d\n\n", peer->ipaddr, peer->port, in->modid, out->id);
  return 0;
}

int sadp_init()
{
      // sadp;
    gsf_sadp_ini_t puini = {
      .ethname = "eth0",
      .lcaddr = "0.0.0.0",
      .mcaddr = SADP_ADDR,
      .mcport = 8888,
      .cb = sadp_recv_func,
    };
    if(bsp_parm.base.mcastdev[0])
    {
      strncpy(puini.ethname, bsp_parm.base.mcastdev, sizeof(puini.ethname)-1); // "ztyxa66jmd"
    }

    int ret = sadp_pu_init(&puini);
    printf("sadp_pu_init ret:%d\n", ret);
}

int main(int argc, char *argv[])
{
    void* log_pull = nm_pull_listen(GSF_IPC_LOG, log_recv); 
    bsp_pub = nm_pub_listen(GSF_PUB_BSP);
    
    if(argc != 3)
    {
      printf("pls input: %s bsp_def.json bsp_parm.json\n", argv[0]);
      return -1;
    }
    
    strncpy(bsp_def_path, argv[1], sizeof(bsp_def_path)-1);
    strncpy(bsp_parm_path, argv[2], sizeof(bsp_parm_path)-1);
    
    GSF_LOG_CONN(1, 100);

    if(json_def_load(bsp_def_path, &bsp_def) < 0)
    {
      json_def_save(bsp_def_path, &bsp_def);
      json_def_load(bsp_def_path, &bsp_def);
    }
    info("def.model:%s\n", bsp_def.board.model);
    
    if(json_parm_load(bsp_parm_path, &bsp_parm) < 0)
    {
      bsp_parm.base = bsp_def.base;
      bsp_parm.eth = bsp_def.eth;
      bsp_parm.wifi = bsp_def.wifi;
      bsp_parm.ntp = bsp_def.ntp;
            
      bsp_parm.users[0] = bsp_def.admin;
      
      json_parm_save(bsp_parm_path, &bsp_parm);
      json_parm_load(bsp_parm_path, &bsp_parm);
    }
    info("init ipaddr:%s\n", bsp_parm.eth.ipaddr);

    //home_path;
    proc_absolute_path(home_path);
    sprintf(home_path, "%s/..", home_path);
    printf("home_path:[%s]\n", home_path);

    //wdg_open();
    
    netinf_init();
    //save random mac addr when the device is first powered on;
    if(!strlen(bsp_parm.eth.mac))
    {
      netinf_eth_get(&bsp_parm.eth);
      json_parm_save(bsp_parm_path, &bsp_parm);
      info("save random mac:[%s]\n", bsp_parm.eth.mac);
    }
    
    rtc_init();
    zone_set(bsp_parm.base.zone);
    ntp_set(&bsp_parm.ntp);

    void* rep = nm_rep_listen(GSF_IPC_BSP
                    , NM_REP_MAX_WORKERS
                    , NM_REP_OSIZE_MAX
                    , req_recv);
    
    sadp_init();
    
    while(1)
    {
        sleep(6);
        
        #if 0 //test get netlink status;
        int stat = netcfg_netlink_status("eth0");
        printf("eth0 stat:%d\n", stat);
        
        stat = netcfg_netlink_status("eth1");
        printf("eth1 stat:%d\n", stat);
        
        stat = netcfg_netlink_status("wlan0");
        printf("wlan0 stat:%d\n", stat);
        
        #endif
        
        
        #if 0 // test wifi scan;
        int i = 0;
        GSF_MSG_DEF(gsf_msg_t, msg, sizeof(gsf_wifi_list_t)*128);
        int ret = GSF_MSG_SENDTO(GSF_ID_BSP_WLIST, 0, GET, 0, 0, GSF_IPC_BSP, 2000);
        gsf_wifi_list_t *wlist = (gsf_wifi_list_t*)__pmsg->data;
        for(i = 0; i < __pmsg->size/sizeof(gsf_wifi_list_t); i++)
        {
          printf("wlist[%d].mac:%s, encrypt:%d, ssid:%s\n", i, wlist[i].mac, wlist[i].encrypt, wlist[i].ssid);
        }
        #endif
    }
    
    nm_rep_close(rep);
    GSF_LOG_DISCONN();
    nm_pull_close(log_pull);
    return 0;
}