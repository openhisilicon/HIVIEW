#ifndef _rtsp_client_st_h_
#define _rtsp_client_st_h_

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include "st.h"

#include "rtsp-client.h"
#include "rtp-socket.h"
#include "cstringext.h"

#if defined(__cplusplus)
extern "C" {
#endif


struct st_rtsp_client_handler_t
{
  int ch,st;
  void *param;
  int (*onframe)(void* param, const char*encoding, const void *packet, int bytes, uint32_t time, int flags);
  int (*onerr)(void* param, int err);
};

void* rtsp_client_connect(const char* url, int protol, struct st_rtsp_client_handler_t *_handler);

int rtsp_client_close(void* st);
int rtsp_client_keepalive(void* st, int media);

#if defined(__cplusplus)
}
#endif
#endif /* !_rtsp_client_st_h_ */
