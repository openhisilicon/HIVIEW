#include <stdio.h>
#include <unistd.h>
#include "cfg.h"

gsf_codec_nvr_t codec_nvr = {
  .vo = {0, 0},
  .venc = {
    [0] = {
        .en     = 1,
        .type   = 0,
        .width  = 1920,
        .height = 1080,
        .fps    = 30,
        .gop    = 30,
        .flow   = 0,
        .bitrate= 4000,
        .profile= 0,
        .qp     = 0,
    },
    [1] = {
        .en     = 1,
        .type   = 0,
        .width  = 640,
        .height = 480,
        .fps    = 30,
        .gop    = 30,
        .flow   = 0,
        .bitrate= 2000,
        .profile= 0,
        .qp     = 0,
    },
    [2] = {
        .en     = 1,
        .type   = 3,
        .width  = 1920,
        .height = 1080,
        .fps    = 30,
        .gop    = 30,
        .flow   = 0,
        .bitrate= 2000,
        .profile= 0,
        .qp     = 0,
    }
  },
};

int json_parm_load(char *filename, gsf_codec_nvr_t *cfg)
{
  if(access(filename, 0)) return -1;
  FILE *f=fopen(filename,"rb");fseek(f,0,SEEK_END);long len=ftell(f);fseek(f,0,SEEK_SET);
	char *data=(char*)malloc(len+1);fread(data,1,len,f);fclose(f);
  cJSON* json = cJSON_Parse(data);
  free(data);
  if (json)
	{
    sjb_bind_gsf_codec_nvr_t(json, 1, cfg, 0, 0);
    cJSON_Delete(json);
	}
  return 0;
}

int json_parm_save(char *filename, gsf_codec_nvr_t *cfg)
{
  FILE *f=fopen(filename,"wb");
  cJSON* out = cJSON_CreateObject();
  sjb_bind_gsf_codec_nvr_t(out, 0, cfg, 0, 0);
  char* print = cJSON_Print(out);
  if(print)
  {
	  fprintf(f, "%s", print);
	  fflush(f);fdatasync(fileno(f));fclose(f);
	  free(print);
  }
  cJSON_Delete(out);
  return 0;
}