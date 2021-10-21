#include "webs.h"
#include "mod/codec/inc/codec.h"


#include "webrtc.h"

#define MAX_FRAME_SIZE (800*1024)


#ifndef WEBRTC_ENABLE

#warning "!!! WEBRTC_ENABLE"
int rtc_init() {return 0;}
int rtc_uninit(){return 0;}

rtc_sess_t *rtc_sess_new(){return 0;}
int rtc_sess_free(rtc_sess_t *rtc_sess){return 0;}

int rtc_createOffer(rtc_sess_t *rtc_sess, char* sdp_json, int sdp_json_len){return 0;}
int rtc_setAnswer(rtc_sess_t *rtc_sess, char* sdp_json){return 0;}
int rtc_setCandidate(rtc_sess_t *rtc_sess, char* ice_json){return 0;}
char* rtc_getCandidate(rtc_sess_t *rtc_sess){return 0;}

#else

#warning "___ WEBRTC_ENABLE"
unsigned int cfifo_recsize(unsigned char *p1, unsigned int n1, unsigned char *p2);
unsigned int cfifo_rectag(unsigned char *p1, unsigned int n1, unsigned char *p2);
unsigned int cfifo_recgut(unsigned char *p1, unsigned int n1, unsigned char *p2, void *u);

struct list_head sess_list;
pthread_mutex_t sess_list_lock;
pthread_t once = PTHREAD_ONCE_INIT;
rtc_sess_t *__rtc_sess_new();
int __rtc_sess_free(rtc_sess_t *rtc_sess);

void* once_run(void* parm)  
{ 
  pthread_detach(pthread_self());
  
  int i = 0, cnt = (int)parm;
  for(i = 0; i < cnt; i++)
  {
    rtc_sess_t* rtc_sess = __rtc_sess_new();
    
    pthread_mutex_lock(&sess_list_lock);
    list_add(&rtc_sess->list, &sess_list);
    pthread_mutex_unlock(&sess_list_lock);
  }
  return NULL;
}  

int rtc_init()
{
  int ret = initKvsWebRtc();
  printf("initKvsWebRtc ret:%x\n", ret);
  
  INIT_LIST_HEAD(&sess_list);
  pthread_mutex_init(&sess_list_lock, NULL);
  return pthread_create(&once, NULL, once_run, (void*)2);
}
int rtc_uninit()
{
  return deinitKvsWebRtc();
}

int __rtc_sess_free(rtc_sess_t *rtc_sess)
{
  int ret = 0;
  rtc_sess->terminated = 1;
  
  if(rtc_sess->pid)
    pthread_join(rtc_sess->pid, NULL);
  
  if(rtc_sess->peer_)
  {  
    ret = closePeerConnection(rtc_sess->peer_);
    ret |= freePeerConnection(&rtc_sess->peer_);
  }
  printf("free rtc_sess:%p\n", rtc_sess);
  free(rtc_sess);
  return ret;
}

int rtc_sess_free(rtc_sess_t *rtc_sess)
{
  if(!rtc_sess)
    return -1;
  
  __rtc_sess_free(rtc_sess);
  
  return pthread_create(&once, NULL, once_run, (void*)1);
}


void* rtc_video_send_task(void *parm);


void rtc_RtcOnIceCandidate(UINT64 customData, PCHAR iceStr) 
{
  rtc_sess_t *rtc_sess = (rtc_sess_t*)customData;
  
  if(iceStr)
  {
    sprintf(rtc_sess->ice_json, "%s", "{\"type\":\"candidate\",");
    strncat(rtc_sess->ice_json, iceStr+1, sizeof(rtc_sess->ice_json)-strlen(rtc_sess->ice_json));

    #if 0
    if(rtc_sess->mgr)
      mg_broadcast(rtc_sess->mgr, on_rtc_send, rtc_sess, sizeof(rtc_sess));
    printf("send candidate:[%s]\n", rtc_sess->ice_json);
    #endif  
  }
}

void rtc_RtcOnMessage(UINT64 customData, PRtcDataChannel dc, BOOL isBinary, PBYTE msgData, UINT32 msgLen)
{
  rtc_sess_t *rtc_sess = (rtc_sess_t*)customData;
  
  printf("dc_label=%s isBinary=%d msgLen=%d\n", dc->name, isBinary, msgLen);
  if(!isBinary) 
  {
    printf("\t%.*s\n", msgLen, msgData);
  }
}

void rtc_RtcOnConnectionStateChange(UINT64 customData, RTC_PEER_CONNECTION_STATE state)
{
  printf("@@@@@@@@@@@@@@@@@@ state: %d @@@@@@@@@@@@@@@@@@\n", state);
  
  rtc_sess_t *rtc_sess = (rtc_sess_t*)customData;
  
  if(state == RTC_PEER_CONNECTION_STATE_CONNECTED) 
  {
    RtcDataChannelInit dcInit;
    memset(&dcInit, 0, sizeof dcInit);
    dcInit.ordered = TRUE;
    createDataChannel(rtc_sess->peer_, (PCHAR)"op", &dcInit, &rtc_sess->dc_);
    dataChannelOnMessage(rtc_sess->dc_, customData, rtc_RtcOnMessage);
    
    //thread retStatus = writeFrame(rtc_sess->videoTransceiver_, &frame);
    pthread_create(&rtc_sess->pid, NULL, rtc_video_send_task, (void*)customData);
    
    //thread retStatus = writeFrame(rtc_sess->audioTransceiver_, &frame);
  }
  else if (state == RTC_PEER_CONNECTION_STATE_DISCONNECTED)
  {
    rtc_sess->terminated = 1;
  }
}

void rtc_RtcOnFrame(UINT64 customData, PFrame frame)
{
  rtc_sess_t *rtc_sess = (rtc_sess_t*)customData;
  
  if(frame)
  {
    printf("(audio) size=%d\n", frame->size);
  }
}

rtc_sess_t *__rtc_sess_new()
{
  int ret = 0;
  rtc_sess_t *rtc_sess = calloc(1, sizeof(rtc_sess_t));
  strcpy(rtc_sess->flag, "rtc");
  
  RtcConfiguration conf_;
  memset(&conf_, 0, sizeof conf_);
  conf_.iceTransportPolicy = ICE_TRANSPORT_POLICY_ALL;
  //conf_.kvsRtcConfiguration.generatedCertificateBits = 2048;
  conf_.kvsRtcConfiguration.generateRSACertificate = 1;
  //strcpy(conf_.iceServers[0].urls, "stun:77.72.169.213:3478");
  strcpy(conf_.iceServers[0].urls, "stun:208.91.197.54:3478");
  ret = createPeerConnection(&conf_, &rtc_sess->peer_);
  printf("createPeerConnection ret:%x\n", ret);

  ret = peerConnectionOnIceCandidate(rtc_sess->peer_, rtc_sess, rtc_RtcOnIceCandidate);
  printf("peerConnectionOnIceCandidate ret:%x\n", ret);
  ret = peerConnectionOnConnectionStateChange(rtc_sess->peer_, rtc_sess, rtc_RtcOnConnectionStateChange);
  printf("peerConnectionOnConnectionStateChange ret:%x\n", ret);

  rtc_sess->video_track_.codec = RTC_CODEC_H264_PROFILE_42E01F_LEVEL_ASYMMETRY_ALLOWED_PACKETIZATION_MODE;
  rtc_sess->video_track_.kind = MEDIA_STREAM_TRACK_KIND_VIDEO;
  strcpy(rtc_sess->video_track_.streamId, "0");
  strcpy(rtc_sess->video_track_.trackId, "0");
  RtcRtpTransceiverInit rtcRtpTransceiverInit_v_;
  rtcRtpTransceiverInit_v_.direction = RTC_RTP_TRANSCEIVER_DIRECTION_SENDONLY;
  addTransceiver(rtc_sess->peer_, &rtc_sess->video_track_, &rtcRtpTransceiverInit_v_, &rtc_sess->videoTransceiver_);

  rtc_sess->audio_track_.codec = RTC_CODEC_ALAW;
  rtc_sess->audio_track_.kind = MEDIA_STREAM_TRACK_KIND_AUDIO;
  strcpy(rtc_sess->audio_track_.streamId, "1");
  strcpy(rtc_sess->audio_track_.trackId, "1");
  RtcRtpTransceiverInit rtcRtpTransceiverInit_a_;
  rtcRtpTransceiverInit_a_.direction = RTC_RTP_TRANSCEIVER_DIRECTION_SENDRECV;
  addTransceiver(rtc_sess->peer_, &rtc_sess->audio_track_, &rtcRtpTransceiverInit_a_, &rtc_sess->audioTransceiver_);
  
  transceiverOnFrame(rtc_sess->audioTransceiver_, rtc_sess, rtc_RtcOnFrame);  
  printf("new rtc_sess:%p\n", rtc_sess);
  return rtc_sess;
}


rtc_sess_t *rtc_sess_new()
{
  rtc_sess_t *rtc_sess = NULL;
  
  pthread_mutex_lock(&sess_list_lock);
  
  if(!list_empty(&sess_list))
  {
    rtc_sess = list_entry(sess_list.next, rtc_sess_t, list); 
    list_del(&rtc_sess->list);
  }
  pthread_mutex_unlock(&sess_list_lock);
  
  return rtc_sess;
}


int rtc_createOffer(rtc_sess_t *rtc_sess, char* sdp_json, int sdp_json_len)
{
  if(!rtc_sess)
    return -1;
    
  RtcSessionDescriptionInit offer;
  createOffer(rtc_sess->peer_, &offer);
  setLocalDescription(rtc_sess->peer_, &offer);
  int ret = serializeSessionDescriptionInit(&offer, sdp_json, &sdp_json_len);
  printf("serializeSessionDescriptionInit ret:0x%x\n", ret);
  return ret;
}

          
int rtc_setAnswer(rtc_sess_t *rtc_sess, char* sdp_json)
{
  if(!rtc_sess)
    return -1;
    
  RtcSessionDescriptionInit answer;
  int ret = deserializeSessionDescriptionInit(sdp_json, strlen(sdp_json), &answer);
  printf("deserializeSessionDescriptionInit ret:0x%x\n", ret);
  if(ret == 0) 
  {
    ret = setRemoteDescription(rtc_sess->peer_, &answer);
    printf("setRemoteDescription ret:0x%x\n", ret);
  }
  return ret;
}

int rtc_setCandidate(rtc_sess_t *rtc_sess, char* ice_json)
{
  if(!rtc_sess)
    return -1;
    
  RtcIceCandidateInit ice;
  
  int ret = deserializeRtcIceCandidateInit(ice_json, strlen(ice_json), &ice);
  printf("deserializeRtcIceCandidateInit ret:0x%x\n", ret);
  if(ret == 0) 
  {
    ret = addIceCandidate(rtc_sess->peer_, ice.candidate);
    printf("addIceCandidate ret:0x%x\n", ret);
  }
  return ret;
}

char* rtc_getCandidate(rtc_sess_t *rtc_sess)
{
  if(!rtc_sess)
    return NULL;
    
  return rtc_sess->ice_json;
}



void* rtc_video_send_task(void *parm)
{
  printf("start.\n");
  int i = 0, ret = 0;
  struct cfifo_ex* video_fifo = NULL;
  struct cfifo_ex* audio_fifo = NULL;
  int ep = cfifo_ep_alloc(1);
  unsigned char buf[sizeof(gsf_frm_t)+MAX_FRAME_SIZE];

  rtc_sess_t *rtc_sess = (rtc_sess_t*)parm;
  
  GSF_MSG_DEF(gsf_sdp_t, sdp, sizeof(gsf_msg_t)+sizeof(gsf_sdp_t));
  sdp->video_shmid = sdp->audio_shmid = -1;
  ret = GSF_MSG_SENDTO(GSF_ID_CODEC_SDP, 0, GET, 0
                        , sizeof(gsf_sdp_t)
                        , GSF_IPC_CODEC
                        , 2000);

  if(ret == 0)
  {
    video_fifo = cfifo_shmat(cfifo_recsize, cfifo_rectag, sdp->video_shmid);
    //audio_fifo = cfifo_shmat(cfifo_recsize, cfifo_rectag, sdp->audio_shmid);
    
  	cfifo_ep_ctl(ep, CFIFO_EP_ADD, video_fifo);
  	unsigned int video_utc = cfifo_newest(video_fifo, 1);
  	 
  	if(audio_fifo)
  	{
  	  cfifo_ep_ctl(ep, CFIFO_EP_ADD, audio_fifo);
  	  unsigned int audio_utc = cfifo_oldest(audio_fifo, video_utc);
  	  printf("video_utc:%u, audio_utc:%u\n", video_utc, audio_utc);
  	}
  }
  
  Frame frame = {0};
  
  while(!rtc_sess->terminated)
  {
    int cnt = 0;
    struct cfifo_ex* result[255];
    cnt = cfifo_ep_wait(ep, 2000, result, 255);
    if(cnt <= 0)
    {
       printf("cfifo_ep_wait err cnt:%d\n", cnt);
    }
    
    for(i = 0; i < cnt; i++)
    {
      struct cfifo_ex* fifo = result[i];
      
      while(ret >= 0)
      {
          ret = cfifo_get(fifo, cfifo_recgut, (unsigned char*)buf);
          if(ret < 0)
          {
              printf("cfifo err ret:%d\n", ret);
          }
          else if (ret == 0)
          {
              //printf("cfifo empty \n");
              break;
          }
          else
          {
              //cfifo_info(fifo);
          }
          
          gsf_frm_t *rec = (gsf_frm_t *)buf;
          //printf("rec->type:%d, seq:%d, utc:%u, size:%d\n", rec->type, rec->seq, rec->utc, rec->size);
          
          frame.frameData = (PBYTE)rec->data;
          frame.size = rec->size;
          frame.presentationTs += (((10LL * 1000LL) * 1000LL) / 30);
          ret = writeFrame(rtc_sess->videoTransceiver_, &frame);
          if(ret != 0)
            printf("writeFrame(video) ret:0x%x\n", ret);
      }
    }
  }
  
  cfifo_ep_free(ep);

  if(video_fifo)
  {
    printf("close video:%p \n", video_fifo);
    cfifo_free(video_fifo);
  }
  
  if(audio_fifo)
  {
    printf("close audio:%p \n", audio_fifo);
    cfifo_free(audio_fifo);
  }
  
  printf("exit.\n");
  return NULL;
}




#endif //WEBRTC_ENABLE