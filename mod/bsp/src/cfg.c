#include <stdio.h>
#include <unistd.h>
#include "cfg.h"

char bsp_parm_path[128] = {0};
char bsp_def_path[128] = {0};

gsf_bsp_def_t bsp_def = {
  .board = {
    .model = "IPC123",
    .sn    = "123456",
    .type  = "HI3516DV300_E_4_2",
    .snscnt = 1,
    .sensor = {"imx327", "", },
    .caps   = 0xFFFFFFFF,
    },
  .base = {
    .name = "gsfIpc",
    .language = 0,
    .zone     = -8*60,
  },
  .eth  = {
    .dhcp = 0,
    .ipv6 = 0,
    .ipaddr = "10.6.5.134",
    .netmask = "255.255.255.0",
    .gateway = "10.6.5.1",
    .dns1    = "202.96.134.133",
    .dns2    = "202.96.128.86",
  },
  .wifi = {
    .en = 0,
  },
  .ntp = {
    .prog = 600,
    .server1 = "server 0.cn.pool.ntp.org",
    .server2 = "server 1.cn.pool.ntp.org",
  },
  .admin = {
    .name = "admin",
    .pwd  = "",
  },
};
    
gsf_bsp_parm_t bsp_parm;

int json_def_load(char *filename, gsf_bsp_def_t *cfg)
{
  if(access(filename, 0)) return -1;
  FILE *f=fopen(filename,"rb");fseek(f,0,SEEK_END);long len=ftell(f);fseek(f,0,SEEK_SET);
	char *data=(char*)malloc(len+1);fread(data,1,len,f);fclose(f);
  cJSON* json = cJSON_Parse(data);
  free(data);
  if (json)
	{
    sjb_bind_gsf_bsp_def_t(json, 1, cfg, 0, 0);
    cJSON_Delete(json);
	}
  return 0;
}

int json_def_save(char *filename, gsf_bsp_def_t *cfg)
{
  FILE *f=fopen(filename,"wb");
  cJSON* out = cJSON_CreateObject();
  sjb_bind_gsf_bsp_def_t(out, 0, cfg, 0, 0);
  char* print = cJSON_Print(out);
  if(print)
  {
	  fprintf(f, "%s", print);
	  fclose(f);
	  free(print);
  }
  cJSON_Delete(out);
  return 0;
}

int json_parm_load(char *filename, gsf_bsp_parm_t *cfg)
{
  if(access(filename, 0)) return -1;
  FILE *f=fopen(filename,"rb");fseek(f,0,SEEK_END);long len=ftell(f);fseek(f,0,SEEK_SET);
	char *data=(char*)malloc(len+1);fread(data,1,len,f);fclose(f);
  cJSON* json = cJSON_Parse(data);
  free(data);
  if (json)
	{
    sjb_bind_gsf_bsp_parm_t(json, 1, cfg, 0, 0);
    cJSON_Delete(json);
	}
  return 0;
}

int json_parm_save(char *filename, gsf_bsp_parm_t *cfg)
{
  FILE *f=fopen(filename,"wb");
  cJSON* out = cJSON_CreateObject();
  sjb_bind_gsf_bsp_parm_t(out, 0, cfg, 0, 0);
  char* print = cJSON_Print(out);
  if(print)
  {
	  fprintf(f, "%s", print);
	  fclose(f);
	  free(print);
  }
  cJSON_Delete(out);
  return 0;
}