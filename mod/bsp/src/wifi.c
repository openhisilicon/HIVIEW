#include "wifi.h"
#include "cfg.h"

#include "fw/comm/inc/netcfg.h"

int wifi_init(void)
{
  gsf_wifi_t *wifi = &bsp_parm.wifi;

  // load driver;
  
  // hostapd
  // wpa_supplicant

  return 0;
}

int wifi_set(gsf_wifi_t *wifi)
{
  
  // hostapd
  // wpa_supplicant
  
  return 0;
}

int wifi_list(gsf_wifi_list_t list[128])
{
  int cnt = 0;
  // iw (https://mirrors.edge.kernel.org/pub/software/network)
  // -- WirelessTools (iwconfig, iwlist, iwspy, iwpriv, ifrename)
  
  return cnt;
}


int dhcpc_start(char *name)
{
  //sprintf(szStr, "/sbin/udhcpc -i %s -b -s /usr/share/udhcpc/default.script &", pNetName);
  return 0;
}

int dhcpc_stop(char *name)
{
  // killall udhcpc;
  return 0;
}


int dhcpd_start(char *name)
{
  return 0;
}
int dhcpd_stop(char *name)
{
  return 0;
}