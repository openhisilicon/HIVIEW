#include "inc/gsf.h"

#include "mod/bsp/inc/bsp.h"
#include "mod/codec/inc/codec.h"

GSF_LOG_GLOBAL_INIT("WEBS", 8*1024);


typedef void (sjb_serialize_cb)(void *c, int m, void *stru, int r1, int r2);
typedef struct {
  int stru_size;
  sjb_serialize_cb *serialize;
  char *uri;
}sjb_serialize_cb_t;

sjb_serialize_cb_t sjb_maps[GSF_MOD_ID_END<<8|255] = {
  [GSF_MOD_ID_CODEC << 8 | GSF_ID_CODEC_VENC] = {sizeof(gsf_venc_t), (sjb_serialize_cb*)sjb_bind_gsf_venc_t, GSF_IPC_CODEC},
};


int rep_recv(char *in, int isize, char *out, int *osize, int err)
{ 
  int ret = 0;
  int modid = -1, msgid = -1, channel = -1, gset = -1;
  
  char *pin = (char*)malloc(isize+1);
  memcpy(pin, in, isize);
  pin[isize] = '\0';
  
  char *p = pin;
  p = strstr(p, "modid:");
  if(p)modid = (p+strlen("modid:"))?atoi(p+strlen("modid:")):-1;
  if(modid == -1)
      goto __err;
  p = strstr(p, "msgid:");
  if(p)msgid = (p+strlen("msgid:"))?atoi(p+strlen("msgid:")):-1;
  if(msgid == -1)
      goto __err;
  p = strstr(p, "channel:");
  if(p)channel = (p+strlen("channel:"))?atoi(p+strlen("channel:")):-1;
  if(channel == -1)
      goto __err;
  p = strstr(p, "gset:");
  if(p)gset = (p+strlen("gset:"))?atoi(p+strlen("gset:")):-1;
  if(gset == -1)
      goto __err;

  p = strstr(p, "data:");
  if(p)p+=strlen("data:");
  
  printf("parse => modid:%d, msgid:%d, channel:%d, gset:%d, data:\n%s\n"
        , modid, msgid, channel, gset, p);
  
  if(modid < 0 || modid >= GSF_MOD_ID_END || msgid < 0 || msgid >= 255)
    goto __err;

  GSF_MSG_DEF(gsf_msg_t, msg, 8*1024);

  if(p && sjb_maps[modid<<8|msgid].serialize)
  {
    cJSON* json = cJSON_Parse(p);
    sjb_maps[modid<<8|msgid].serialize(json, 1, msg->buf, 0, 0);
    cJSON_Delete(json);
  }
  
  //gset;
  ret = GSF_MSG_SENDTO(msgid, channel, gset, 0
                        , sjb_maps[modid<<8|msgid].stru_size
                        , sjb_maps[modid<<8|msgid].uri
                        , 2000);

  printf("GSF_MSG_SENDTO => To:%s, ret:%d, err:%d, size:%d\n"
        , sjb_maps[modid<<8|msgid].uri, ret, __pmsg->err, __pmsg->size);
  
  int code = ret?ret:__pmsg->err;
  sprintf(out, "code:%d%s\r\n", code, code?"[GSF_MSG_SENDTO err.]":"");
      
  if(code == 0 && __pmsg->size > 0 && sjb_maps[modid<<8|msgid].serialize)
  {
    cJSON* json2 = cJSON_CreateObject();
    sjb_maps[modid<<8|msgid].serialize(json2, 0, __pmsg->buf, 0, 0);
    char *rsp = cJSON_Print(json2);
    strncat(out, rsp, *osize-strlen(out)-1);
    free(rsp);
    cJSON_Delete(json2);
  }
  printf("rsp => \n%s\n", out);
  *osize = strlen(out);
  free(pin);
  return 0;
  
__err:
  printf("err => modid:%d, msgid:%d, channel:%d, gset:%d, data:\n%s\n"
        , modid, msgid, channel, gset, p);
  sprintf(out, "code:-2[input parm err.]\r\n");
  *osize = strlen(out);
  free(pin);
  return 0;
}

int main(void)
{
  // You can use chrome to open index.html to test json sending and receiving;
  void* pub = nm_pub_listen("ws://*:7789");
  void* req = nm_rep_listen("ws://*:7790"
                          , NM_REP_MAX_WORKERS
                          , NM_REP_OSIZE_MAX
                          , rep_recv);
  while(1)
  {
    static int cnt = 0;
    char str[16];
    sprintf(str, "PUB:%08d", cnt++);
    nm_pub_send(pub, str, strlen(str));
    usleep(100*1000);
  }
  
  
  return 0;
}