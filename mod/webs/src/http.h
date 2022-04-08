#ifndef __http_h__
#define __http_h__

int http_open(int port);
int http_close();

enum {
  WEBRTC_NEW = 1,
  WEBRTC_DEL = 2
};
int webrtc_proc(struct mg_connection *nc, rtc_sess_t **rtc_sess, struct websocket_message *wm);

#endif //__http_h__