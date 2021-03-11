#include "netinf.h"
#include "cfg.h"
#include "wifi.h"
#include "fw/comm/inc/netcfg.h"

int vpn_init(gsf_vpn_t *vpn);

int netinf_init(void)
{
  //init driver;
  wifi_init();
   
  //init ipaddr;
  gsf_eth_t *eth = &bsp_parm.eth;
  netinf_eth_set(eth);
  
  //init vpn;
  vpn_init(&bsp_parm.vpn);
  
  return 0;
}

int netinf_eth_set(gsf_eth_t *eth)
{
  dhcpc_stop("eth0");
  
  int ret = netcfg_set_mac_addr("eth0", eth->mac);
  printf("ret:%d, eth->mac:[%s]\n", ret, eth->mac);
  
  if(!eth->dhcp)
  {
    netcfg_set_ip_addr("eth0", eth->ipaddr);
    netcfg_set_mask_addr("eth0", eth->netmask); 
    netcfg_set_def_gw_addr("eth0", eth->gateway);
    netcfg_update_dns("/etc/resolv.conf", eth->dns1, eth->dns2);
  }
  else
  {
     dhcpc_start("eth0");
  }
  
  return 0;
}

int netinf_eth_get(gsf_eth_t *eth)
{
  char *name = "eth0";
  gsf_eth_t *_eth = &bsp_parm.eth;
  
  eth->dhcp = _eth->dhcp;
  eth->ipv6 = _eth->ipv6;
  netcfg_get_ip_addr(name,   eth->ipaddr);
  netcfg_get_gw_addr(name,   eth->gateway);
  netcfg_get_mask_addr(name, eth->netmask);
  netcfg_get_dns("/etc/resolv.conf", eth->dns1, eth->dns2);
  char _mac[6] = {0};
  netcfg_get_mac_addr(name, _mac);
  sprintf(eth->mac, "%02X:%02X:%02X:%02X:%02X:%02X", _mac[0], _mac[1], _mac[2], _mac[3], _mac[4], _mac[5]);
  printf("eth->mac:[%s]\n", eth->mac);
  return 0;
}


int netinf_wifi_set(gsf_wifi_t *wifi)
{
  return wifi_set(wifi);
}

int netinf_wifi_list(gsf_wifi_list_t list[128])
{
  return wifi_list(list);
}

extern int sadp_init();
static pthread_t once;
static void* once_run(void* parm)
{
  pthread_detach(pthread_self());
  
  while(1)
  {
    char str[128];
    sprintf(str, "dmesg | grep \"%s\" ", "crng init done");
    FILE* fd = popen(str, "r");
    if(fd)
    {
      if (fgets(str, sizeof(str), fd))
      {
        printf("%s => %s\n", __func__, str);
        if(strstr(str, "crng init done"))
        {
          system("zerotier-one -d");
          sadp_init();
          fclose(fd);
          break;
        }
      }
      fclose(fd);
    }
    sleep(1);
  }
  printf("%s => exit.\n", __func__);
  return NULL;
}

int vpn_init(gsf_vpn_t *vpn)
{
  return pthread_create(&once, NULL, once_run, NULL);
}

int netinf_vpn_set(gsf_vpn_t *vpn)
{
  char str[128] = {0};

  // leave;
  if(strlen(vpn->nwid))
  {
    sprintf(str, "zerotier-cli leave %s", vpn->nwid);
    system(str);
    sleep(1);
  }

  // join;
  if(vpn->en && strlen(vpn->nwid))
  {
    sprintf(str, "zerotier-cli join %s", vpn->nwid);
    system(str);
  }
    
  return 0;
}

int netinf_vpn_stat(gsf_vpn_stat_t *st)
{
  int line = 0;
  char str[128];
  sprintf(str, "%s", "zerotier-cli listnetworks");
  FILE* fd = popen(str, "r");
  while(fd)
  {
    if (fgets(str, sizeof(str), fd))
    {
      if(line++)
        strncpy(st->network, str, sizeof(st->network)-1);
    }
    else
    {
      break;
    }
  }
  
  if(fd)
    fclose(fd);
    
  return 0;
}