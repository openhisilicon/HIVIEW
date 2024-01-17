#ifndef _rtsp_server_st_h_
#define _rtsp_server_st_h_
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

#define SO_REUSEPORT 15

#include "rtsp-server.h"
#include "rtp-over-rtsp.h"
#include "rtp-transport.h"
#include "rtp-media.h"

#if defined(__cplusplus)
extern "C" {
#endif


struct rtsp_server_listen_t
{
	st_netfd_t srv_nfd;
  int loop, exited;
  st_thread_t tid;
  struct st_rtsp_handler_t* handler;
  void *param;
};

void* st_rtsp_server_listen(const char* ip, int port);
int st_rtsp_server_unlisten(void* st);


struct rtsp_session_t;

struct st_rtsp_handler_t
{
	struct rtsp_handler_t base;
	void (*onerror)(struct rtsp_session_t* sess, int code);
	void (*onrtp)(struct rtsp_session_t* sess, uint8_t channel, const void* data, uint16_t bytes);
};

void* rtsp_server_listen(const char* ip, int port, struct st_rtsp_handler_t* handler, void* param);
int rtsp_server_unlisten(void* st);


struct rtsp_session_t
{
	st_netfd_t socket;
	socklen_t  addrlen;
	struct sockaddr_storage addr;
	st_mutex_t lock;
	
	struct rtp_over_rtsp_t rtp;
	
	int rtsp_need_more_data;
	uint8_t buffer[4*1024];
	struct rtsp_server_t *rtsp;

  int channel;
  struct rtp_media_t* media;
  
  struct st_rtsp_handler_t handler;
	void* param;
	int timeout, snderr;
	int loop, exited;
	st_thread_t tid;
};


int  rtsp_session_oncreate(struct rtsp_session_t *sess);
void rtsp_session_ondestroy(struct rtsp_session_t* sess);
void rtsp_session_onrecv(struct rtsp_session_t* sess, int code, size_t bytes);


#if defined(__cplusplus)
}
#endif
#endif /* !_rtsp_server_st_h_ */
