#include "cfg.h"

char app_parm_path[128] = {0};

gsf_app_nvr_t app_nvr = {
  .chsrc[0] = {
    .en = 1,
    .host = "onvif://admin:12345@192.168.1.2:8000",
    .transp = 0,
    .st1  = "",
    .st2  = "",
  },
  .chsrc[1] = {
    .en = 0,
    .host = "onvif://admin:admin@192.168.1.101:80",
    .transp = 0,
    .st1  = "",
    .st2  = "",
  },
};

int json_parm_load(char *filename, gsf_app_nvr_t *cfg)
{
  if(access(filename, 0)) return -1;
  FILE *f=fopen(filename,"rb");fseek(f,0,SEEK_END);long len=ftell(f);fseek(f,0,SEEK_SET);
	char *data=(char*)malloc(len+1);fread(data,1,len,f);fclose(f);
  cJSON* json = cJSON_Parse(data);
  free(data);
  if (json)
	{
    sjb_bind_gsf_app_nvr_t(json, 1, cfg, 0, 0);
    cJSON_Delete(json);
	}
  return 0;
}

int json_parm_save(char *filename, gsf_app_nvr_t *cfg)
{
  FILE *f=fopen(filename,"wb");
  cJSON* out = cJSON_CreateObject();
  sjb_bind_gsf_app_nvr_t(out, 0, cfg, 0, 0);
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