#include "mod/bsp/inc/bsp.h"
#include "mod/codec/inc/codec.h"

GSF_LOG_GLOBAL_INIT("WEBS", 8*1024);

int main(void)
{
  // You'll see that the structure in sjb_xxx.h is automatically serialized to JSON;
  // You can easily communicate using HTTP+JSON without writing too much code;
  char *http_body1 = "{                       "
                  	"		\"vst\":	[{          "
                  	"				\"type\":	0,      "
                  	"				\"width\":	100,  "
                  	"				\"height\":	50,   "
                  	"				\"gop\":	50,     "
                  	"				\"flow\":	0,      "
                  	"				\"bitrate\":	0,  "
                  	"				\"profile\":	0,  "
                  	"				\"qp\":	0         "
                  	"			}]                  "
                  	"	}";
  printf("\n======================================\n");
  gsf_venc_t venc;
  memset(&venc, 0, sizeof(venc));
  cJSON* json = cJSON_Parse(http_body1);
  sjb_bind_gsf_venc_t(json, 1, &venc, 0, 0);
  cJSON_Delete(json);
  printf("venc.vst[0].width:%d, height:%d\n"
        , venc.vst[0].width, venc.vst[0].height);
  // GSF_MSG_SENDTO(GSF_IPC_CODEC);
  printf("\n======================================\n");
  // GSF_MSG_SENDTO(GSF_IPC_CODEC);
  cJSON* out = cJSON_CreateObject();
  sjb_bind_gsf_venc_t(out, 0, &venc, 0, 0);
  char* http_body2 = cJSON_Print(out);
  printf("webs_write:\n %s \n", http_body2);
  free(http_body2);
  cJSON_Delete(out);
  printf("\n======================================\n");
  return 0;
}