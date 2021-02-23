#include "netinf.h"
#include "cfg.h"
#include "wifi.h"
#include "fw/comm/inc/netcfg.h"


int netinf_init(void)
{
  //init driver;
  wifi_init();
  system("zerotier-one -d");  
  
  //init ipaddr;
  gsf_eth_t *eth = &bsp_parm.eth;
  return netinf_eth_set(eth);
}

int netinf_eth_set(gsf_eth_t *eth)
{
  dhcpc_stop("eth0");
   
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

  //netcfg_get_mac_addr(name,  eth->mac);

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
  return 0;
}