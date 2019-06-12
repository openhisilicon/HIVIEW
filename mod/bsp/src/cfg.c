#include <stdio.h>
#include <unistd.h>
#include "cfg.h"

gsf_bsp_def_t bsp_def = {
  .board = {
    .model = "IPCXXX",
    .sn    = "123456",
    .type  = "HI3519_E_4_2",
    .snscnt = 2,
    .sensor = {"BT1120_R", "BT1120_R", },
    .caps   = 0xFFFFFFFF,
    },
  .eth  = {
    .dhcp = 0,
    .ipv6 = 0,
    .ipaddr = "192.168.1.66",
    .netmask = "255.255.255.0",
    .gateway = "192.168.1.1",
  },
  .admin = {
    .name = "admin",
    .pwd  = "",
  },
};
    
gsf_bsp_parm_t bsp_parm;

int jscon_def_load(char *filename, gsf_bsp_def_t *cfg)
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

int jscon_def_save(char *filename, gsf_bsp_def_t *cfg)
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

int jscon_parm_load(char *filename, gsf_bsp_parm_t *cfg)
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

int jscon_parm_save(char *filename, gsf_bsp_parm_t *cfg)
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