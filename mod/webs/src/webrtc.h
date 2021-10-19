#ifndef __webrtc_h__
#define __webrtc_h__

#include "com/amazonaws/kinesis/video/webrtcclient/Include.h"

typedef struct {
  char flag[32];
  void *mgr;
  pthread_t pid;
  PRtcPeerConnection  peer_;
  PRtcDataChannel     dc_;
  PRtcRtpTransceiver  videoTransceiver_;
  PRtcRtpTransceiver  audioTransceiver_;
  RtcMediaStreamTrack video_track_;
  RtcMediaStreamTrack audio_track_;
  int  terminated;
  char ice_json[2048];
}rtc_sess_t;

int rtc_init();
int rtc_uninit();

rtc_sess_t *rtc_sess_new();
int rtc_sess_free(rtc_sess_t *rtc_sess);

int rtc_createOffer(rtc_sess_t *rtc_sess, char* sdp_json, int sdp_json_len);
int rtc_setAnswer(rtc_sess_t *rtc_sess, char* sdp_json);
int rtc_setCandidate(rtc_sess_t *rtc_sess, char* ice_json);
char* rtc_getCandidate(rtc_sess_t *rtc_sess);

#endif //__webrtc_h__