#include <unistd.h>
#include <stdlib.h>
#include "inc/gsf.h"
#include "bsp.h"

#include "cfg.h"      // config
#include "netinf.h"   // netinf
#include "wdg.h"      // watchdog
#include "rtc.h"      // rtc
#include "sadp.h"     // discover
#include "upg.h"      // upgrade

#include "msg_func.h"

GSF_LOG_GLOBAL_INIT("BSP", 8*1024);

void* bsp_pub = NULL;

extern gsf_mod_reg_t gmods[GSF_MOD_ID_END];

static int log_recv(char *msg, int size, int err)
{
    fprintf(stderr, "L%s", msg);
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

    if(1)
    {
    	struct timespec _ts;
      clock_gettime(CLOCK_MONOTONIC, &_ts);
      printf("id:%d, delay:%d ms\n", req->id, _ts.tv_sec*1000 + _ts.tv_nsec/1000000 - req->ts);     
    }

    ret = msg_func_proc(req, isize, rsp, osize);

    rsp->err = (ret == TRUE)?rsp->err:GSF_ERR_MSG;
    *osize = sizeof(gsf_msg_t)+rsp->size;

    return 0;
}

#include <math.h>
static int same_subnet(char *sub_mask, char *ip_a, char *ip_b)
{ 
  int i;
  double cnt_m = 0.0;
  double cnt_a = 0.0;
  double cnt_b = 0.0;
  double tmp = 0.0;
 
  char *token = NULL;
 
  i = 3;
  for (token = strtok (sub_mask, "."); token != NULL; token = strtok (NULL, ".")) {
    tmp = atoi (token);
    tmp = tmp * pow (256, i--);  /* what dose this mean? do you understand */
    cnt_m += tmp;
  }
  
  i = 3;
  for (token = strtok (ip_a, "."); token != NULL; token = strtok (NULL, ".")) {
    tmp = atoi (token);
    tmp = tmp * pow (256, i--);  /* what dose this mean? do you understand */
    cnt_a += tmp;
  }
 
  i = 3; /* reset i */
  for (token = strtok (ip_b, "."); token != NULL; token = strtok (NULL, ".")) {
    tmp = atoi (token);
    tmp = tmp * pow (256, i--);
    cnt_b += tmp;
  }
 
  unsigned long mask   = (unsigned long)cnt_m;
  unsigned long mask_a = (unsigned long)cnt_a;
  unsigned long mask_b = (unsigned long)cnt_b;

  mask_a &= mask;
  mask_b &= mask;
 
  if (mask_a == mask_b)
    return 0;
  else
    return -1;
}

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <arpa/inet.h>
#include <net/if.h>
static int ip_addr(char* ethname, char *ipaddr, size_t len)
{
    int fd;
    char buffer[20];
    struct ifreq ifr;
    struct sockaddr_in *addr;

    if ((fd = socket(AF_INET, SOCK_DGRAM, 0)) >= 0)
    {
        strncpy(ifr.ifr_name, ethname, IFNAMSIZ);
        ifr.ifr_name[IFNAMSIZ - 1] = '\0';
        if (ioctl(fd, SIOCGIFADDR, &ifr) == 0)
        {
            addr = (struct sockaddr_in *) & (ifr.ifr_addr);
            inet_ntop(AF_INET, &addr->sin_addr, buffer, 20);
        }
        else
        {
            close(fd);
            return(-1);
        }
    }
    else
    {
        return(-1);
    }

    if (strlen(buffer) > len - 1)
    {
		    close(fd);
        return(-1);
    }
    strncpy(ipaddr, buffer, len);
    close(fd);
    return(0);
}

static int net_mask(char* ethname, char *netmask, size_t len)
{
    int fd;
    char buffer[20];
    struct ifreq ifr;
    struct sockaddr_in *addr;

    if ((fd = socket (AF_INET, SOCK_DGRAM, 0)) >= 0)
    {
        strncpy(ifr.ifr_name, ethname, IFNAMSIZ);
        ifr.ifr_name[IFNAMSIZ - 1] = '\0';
        if (ioctl(fd, SIOCGIFNETMASK, &ifr) == 0)
        {
            addr = (struct sockaddr_in *) & (ifr.ifr_addr);
            inet_ntop(AF_INET, &addr->sin_addr, buffer, 20);
        }
        else
        {
            close(fd);
            return(-1);
        }
    }
    else
    {
        return(-1);
    }

    if (strlen(buffer) > len - 1)
    {
		    close(fd);
        return(-1);
    }
    strncpy(netmask, buffer, len);

    close(fd);
    return(0);
}


static int sadp_recv_func(gsf_sadp_msg_t *in, gsf_msg_t* out
          , int *osize/*IN&OUT*/, gsf_sadp_peer_t* peer)
{
  int ret = 0;
  
  printf("in->ver:%04X, modid:%d, msg.size:%d, peer:[%s:%d]\n"
        , in->ver, in->modid, in->msg.size, peer->ipaddr, peer->port);
  
  if(in->modid < 0 || in->modid >= GSF_MOD_ID_END)
  {
    printf("modid err.\n", in->modid);
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
  
  if(1)
  {
    //for test mcast resp;
    char lcaddr[64];
    char netmask[64];
    ip_addr("eth0", lcaddr, sizeof(lcaddr));
    net_mask("eth0", netmask, sizeof(netmask));
    
    if(same_subnet(netmask, peer->ipaddr, lcaddr))
    {
      printf("not same subnet sendto 238.238.238.238\n");
      gsf_sadp_peer_t dst = {"238.238.238.238", 8888};
      gsf_sadp_msg_t in = {.modid = GSF_MOD_ID_END, .msg = {0,}};
      sadp_cu_gset(&dst, &in, NULL, osize, 0);
      *osize = 0;
      return 0;
    }
  }

  printf("nm_req_sendto(modid:%d) => osize:%d\n\n", in->modid, *osize);
  return 0;
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
    
    GSF_LOG_CONN(1, 100);
    
    if(json_def_load(argv[1], &bsp_def) < 0)
    {
      json_def_save(argv[1], &bsp_def);
      json_def_load(argv[1], &bsp_def);
    }
    info("def.model:%s\n", bsp_def.board.model);
    
    if(json_parm_load(argv[2], &bsp_parm) < 0)
    {
      bsp_parm.eth = bsp_def.eth;
      bsp_parm.users[0] = bsp_def.admin;
      
      json_parm_save(argv[2], &bsp_parm);
      json_parm_load(argv[2], &bsp_parm);
    }
    info("parm.ipaddr:%s\n", bsp_parm.eth.ipaddr);

    
    wdg_open(15);
    netinf_init();
    
    
    gsf_sadp_ini_t puini = {
      .ethname = "eth0",
      .lcaddr = "0.0.0.0",
      .mcaddr = "238.238.238.238",
      .mcport = 8888,
      .cb = sadp_recv_func,
    };
    int ret = sadp_pu_init(&puini);
    printf("sadp_pu_init ret:%d\n", ret);
    
    void* rep = nm_rep_listen(GSF_IPC_BSP
                    , NM_REP_MAX_WORKERS
                    , NM_REP_OSIZE_MAX
                    , req_recv);
    
    while(1)
    {
        sleep(1);
    }
    
    nm_rep_close(rep);
    GSF_LOG_DISCONN();
    nm_pull_close(log_pull);
    return 0;
}