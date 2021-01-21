#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <assert.h>

#include "inc/gsf.h"
#include "mod_call.h"


typedef void (sjb_cb)(void *c, int m, void *stru, int r1, int r2);
typedef struct {
  int  id;
  char *str;
  char *uri;      // mod uri;
  sjb_cb *req_serialize;
  sjb_cb *rsp_serialize;
  int reqs;       // req struct size;
  int rsps, rspn; // rsp struct size, rsp is array have N struct;
}sjb_cb_t;

#ifdef GSF_DEV_NVR
#warning "...... GSF_DEV_NVR defined ......"
#endif


sjb_cb_t sjb_maps[GSF_MOD_ID_END<<8|255] = {

//bsp;
  {GSF_ID_BSP_TIME, "GSF_ID_BSP_TIME", GSF_IPC_BSP,(sjb_cb*)sjb_bind_gsf_time_t, (sjb_cb*)sjb_bind_gsf_time_t, sizeof(gsf_time_t), sizeof(gsf_time_t),0},
  {GSF_ID_BSP_ETH,  "GSF_ID_BSP_ETH",  GSF_IPC_BSP,(sjb_cb*)sjb_bind_gsf_eth_t,  (sjb_cb*)sjb_bind_gsf_eth_t,  sizeof(gsf_eth_t),  sizeof(gsf_eth_t), 0},
  {GSF_ID_BSP_NTP,  "GSF_ID_BSP_NTP",  GSF_IPC_BSP,(sjb_cb*)sjb_bind_gsf_ntp_t,  (sjb_cb*)sjb_bind_gsf_ntp_t,  sizeof(gsf_ntp_t),  sizeof(gsf_ntp_t), 0},
  {GSF_ID_BSP_WIFI, "GSF_ID_BSP_WIFI", GSF_IPC_BSP,(sjb_cb*)sjb_bind_gsf_wifi_t, (sjb_cb*)sjb_bind_gsf_wifi_t, sizeof(gsf_wifi_t), sizeof(gsf_wifi_t),0},

//codec;
  {GSF_ID_CODEC_VENC,  "GSF_ID_CODEC_VENC", GSF_IPC_CODEC,(sjb_cb*)sjb_bind_gsf_venc_t,   (sjb_cb*)sjb_bind_gsf_venc_t,sizeof(gsf_venc_t), sizeof(gsf_venc_t),0},
  {GSF_ID_CODEC_OSD,   "GSF_ID_CODEC_OSD",  GSF_IPC_CODEC,(sjb_cb*)sjb_bind_gsf_osd_t,    (sjb_cb*)sjb_bind_gsf_osd_t,sizeof(gsf_osd_t), sizeof(gsf_osd_t),0},
  {GSF_ID_CODEC_VMASK, "GSF_ID_CODEC_VMASK", GSF_IPC_CODEC,(sjb_cb*)sjb_bind_gsf_vmask_t, (sjb_cb*)sjb_bind_gsf_vmask_t,sizeof(gsf_vmask_t), sizeof(gsf_vmask_t),0},

//rec;
  {GSF_ID_REC_CFG,  "GSF_ID_REC_CFG", GSF_IPC_REC  ,(sjb_cb*)sjb_bind_gsf_rec_cfg_t,(sjb_cb*)sjb_bind_gsf_rec_cfg_t,sizeof(gsf_rec_cfg_t),sizeof(gsf_rec_cfg_t),0},
  {GSF_ID_REC_QREC, "GSF_ID_REC_QREC", GSF_IPC_REC  ,(sjb_cb*)sjb_bind_gsf_rec_q_t,(sjb_cb*)sjb_bind_gsf_file_t,sizeof(gsf_rec_q_t),sizeof(gsf_file_t),1},

//app;
  {GSF_ID_APP_CHSRC, "GSF_ID_APP_CHSRC", GSF_IPC_APP  ,(sjb_cb*)sjb_bind_gsf_chsrc_t,(sjb_cb*)sjb_bind_gsf_chsrc_t,sizeof(gsf_chsrc_t),sizeof(gsf_chsrc_t),1},

};



int mod_call(char *str, char *args, char *in, char *out, int osize)
{
  int i = 0, j = -1, ret = -1;
  sjb_cb_t *cb = NULL;
  
  int channel = -1, gset = -1, sid = -1;
  
  sscanf(args, "G%dC%dS%d", &gset, &channel, &sid);
  
  for(i = 0; i < sizeof(sjb_maps)/sizeof(sjb_maps[0]); i++)
  {
    if(sjb_maps[i].id == 0)
      break;
    if(!strncmp(sjb_maps[i].str, str, strlen(sjb_maps[i].str)))
    {
      j = i;
      break;
    }
  }
  
  if(j < 0)
  {
    sprintf(out, "{\"code\":%d\r\n}", ret);
    printf("rsp => \n%s\n", out);
    return ret;
  }
  
  GSF_MSG_DEF(char, msgdata, sizeof(gsf_msg_t) + 80*1024);

  cb = &sjb_maps[j];
    
  if(in && cb->req_serialize)
  {
    cJSON* json = cJSON_Parse(in);
    cb->req_serialize(json, 1, msgdata, 0, 0);
    cJSON_Delete(json);
  }
  
  printf("send => id:%d, in:[%s]\n", cb->id, in);
  
  //gset;
  ret = GSF_MSG_SENDTO(cb->id, channel, gset, sid
                        , cb->reqs
                        , cb->uri
                        , 2000);

  printf("send => To:%s, ret:%d, size:%d\n"
        , cb->uri, ret, __pmsg->size);
  

  sprintf(out, "{\"code\":%d,\r\n", ret);
  strncat(out, "\"data\": ", osize-strlen(out)-1);

  if(cb->rspn)
    strncat(out, "[\r\n", osize-strlen(out)-1);
    
  if(ret == 0 && __pmsg->size > 0 && cb->rsp_serialize)
  {    
    for(i = 0; i < __pmsg->size/cb->rsps; i++)
    {
      cJSON* json2 = cJSON_CreateObject();
      cb->rsp_serialize(json2, 0, __pmsg->data + i*cb->rsps, 0, 0);
      char *rsp = cJSON_Print(json2);
      strncat(out, rsp, osize-strlen(out)-1);
      free(rsp);
      cJSON_Delete(json2);
      
      if(cb->rspn && i < __pmsg->size/cb->rsps - 1)
      {
        strncat(out, ",\r\n", osize-strlen(out)-1);
        //printf("osize:%d, strlen(out)-1:%d\n", osize, strlen(out)-1);
      }
    }
  }
  else
  {
    strncat(out, "{}\r\n", osize-strlen(out)-1);
  }
  
  if(cb->rspn)
    strncat(out, "]\r\n", osize-strlen(out)-1);
    
  strncat(out, "}", osize-strlen(out)-1);
  printf("rsp => \n%s\n", out);
  return ret;
}
