#include "wifi.h"
#include "cfg.h"

#include "fw/comm/inc/netcfg.h"

int wifi_init(void)
{
  gsf_wifi_t *wifi = &bsp_parm.wifi;
  char cmd[256] = {0};
  
  // load driver;
  if(wifi->en)
  {
    if(strlen(wifi->ssid))
    {
      sprintf(cmd, "sed -i '/^ssid=/{h;s/=.*/=\"%s\"/}' %s/wifi/wpa.conf", wifi->ssid, home_path); system(cmd);
      sprintf(cmd, "sed -i '/^psk=/{h;s/=.*/=\"%s\"/}' %s/wifi/wpa.conf", wifi->pwd, home_path); system(cmd);
      //sprintf(cmd, "sed -i '/^ssid=/{h;s/=.*/=%s/}' %s/wifi/hostapd.conf", wifi->ssid, home_path); system(cmd);
      //sprintf(cmd, "sed -i '/^wpa_passphrase=/{h;s/=.*/=%s/}' %s/wifi/hostapd.conf", wifi->pwd, home_path); system(cmd);
    }
    sprintf(cmd, "%s/wifi/%s", home_path, (wifi->ap)?"ap.sh":"wifi.sh"); system(cmd);
  }
  return 0;
}

int wifi_set(gsf_wifi_t *wifi)
{
  char cmd[256] = {0};
  sprintf(cmd, "killall udhcpc udhcpd wpa_supplicant hostapd"); system(cmd);
  
  if(wifi->en)
  {
    if(strlen(wifi->ssid))
    {
      sprintf(cmd, "sed -i '/^ssid=/{h;s/=.*/=\"%s\"/}' %s/wifi/wpa.conf", wifi->ssid, home_path); system(cmd);
      sprintf(cmd, "sed -i '/^psk=/{h;s/=.*/=\"%s\"/}' %s/wifi/wpa.conf", wifi->pwd, home_path); system(cmd);
      //sprintf(cmd, "sed -i '/^ssid=/{h;s/=.*/=%s/}' %s/wifi/hostapd.conf", wifi->ssid, home_path); system(cmd);
      //sprintf(cmd, "sed -i '/^wpa_passphrase=/{h;s/=.*/=%s/}' %s/wifi/hostapd.conf", wifi->pwd, home_path); system(cmd);
    }
    sprintf(cmd, "%s/wifi/%s", home_path, (wifi->ap)?"ap_reset.sh":"wifi_reset.sh"); system(cmd);
  }
  
  //gsf_eth_t for wifi ipaddr;
  extern int netinf_eth_set(gsf_eth_t *eth);
  gsf_eth_t *eth = &bsp_parm.eth;
  netinf_eth_set(eth);
  
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
  char cmd[256] = {0};
  sprintf(cmd, "udhcpc -i %s -b -s %s/wifi/simple.script &", name, home_path); system(cmd);
  return 0;
}

int dhcpc_stop(char *name)
{
  char cmd[256] = {0};
  sprintf(cmd, "killall udhcpc"); system(cmd);
  return 0;
}


int dhcpd_start(char *name)
{
  char cmd[256] = {0};
  sprintf(cmd, "udhcpd %s/wifi/udhcpd.conf &", home_path); system(cmd);
  return 0;
}
int dhcpd_stop(char *name)
{
  char cmd[256] = {0};
  sprintf(cmd, "killall udhcpd"); system(cmd);
  return 0;;
}