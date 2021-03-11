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
  sprintf(cmd, "killall -9 udhcpc udhcpd wpa_supplicant hostapd"); system(cmd);
  
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
  
  //gsf_eth_t for wifi ipaddr;
  extern int netinf_eth_set(gsf_eth_t *eth);
  gsf_eth_t *eth = &bsp_parm.eth;
  netinf_eth_set(eth);
  
  return 0;
}

int wifi_list(gsf_wifi_list_t list[128])
{
  int cnt = 0;    
  char cmd[256] = {0};
  sprintf(cmd, "%s/wifi/wpa_cli -i eth0 scan", home_path); system(cmd);
  
  int line = 0;
  sprintf(cmd, "%s/wifi/wpa_cli -i eth0 scan_result", home_path);
  FILE* fd = popen(cmd, "r");
  while(fd)
  {
    if (fgets(cmd, sizeof(cmd), fd))
    {
      if(line++ == 0)
        continue;
      
      if(cnt >= 128)
        break;
      
      int t = 0;
      char *left = cmd;
      char *token = NULL;
      
      enum {
        TOKEN_BSSID = 0, TOKEN_FREQ = 1, TOKEN_SIGNAL = 2, TOKEN_FLAGS = 3, TOKEN_SSID = 4
      };
      
      cmd[strlen(cmd)-1] = '\0';
      //printf("cnt:%d, cmd:[%02x, %02x]\n", cnt, cmd[strlen(cmd)-2], cmd[strlen(cmd)-1]);
      
      list[cnt].quality = 0;
      list[cnt].frequency = 0;
      list[cnt].channle = 0;
      
      while(token = strsep(&left, "\t"))
      {
        if(!strlen(token))
          continue;
          
        //printf("token:[%s]\n", token);
        switch(t++)
        {
          case TOKEN_BSSID:
            strncpy(list[cnt].mac, token, sizeof(list[cnt].mac)-1);
            break;
          case TOKEN_FREQ:
            break;
          case TOKEN_SIGNAL:
            break;
          case TOKEN_FLAGS:
            list[cnt].encrypt = strstr(token, "WPA")?1:0;
            break;
          case TOKEN_SSID:
            strncpy(list[cnt].ssid, token, sizeof(list[cnt].ssid)-1);
            break;
        }
      }
      cnt++;
    }
    else
    {
      break;
    }
  }
  
  if(fd)
    fclose(fd);
    
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