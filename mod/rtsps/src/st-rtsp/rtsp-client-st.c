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

#include "ntp-time.h"
#include "rtsp-client.h"
#include "rtp-socket.h"
#include "cstringext.h"
#include "rtp-receiver.h"
#include "rtsp-client-st.h"

enum {
  RTSP_CLIENT_STAT_SETUP = 1,
  RTSP_CLIENT_STAT_PLAY = 2,
};


struct rtsp_client_test_t
{
  char host[64];
  char file[64];
  char user[64];
  char pwd[64];
  int  rtsp_port;
  int   stat;
	void* rtsp;
	st_netfd_t socket;
  int last_keeptime;
  int ses_timeout;
	int transport;
	st_netfd_t rtp[5][2];
	unsigned short port[5][2];
	struct st_rtsp_client_handler_t handler;
  int loop, exited;
  st_thread_t tid;
  struct rtp_context_t* receiver[5];
  // for pusher;
  int ch,st;
  char sdp[2048];
  struct rtp_media_t* media;
};

static int rtsp_client_send(void* param, const char* uri, const void* req, size_t bytes)
{
	//TODO: check uri and make socket
	//1. uri != rtsp describe uri(user input)
	//2. multi-uri if media_count > 1
	
	//lock;
	struct rtsp_client_test_t *ctx = (struct rtsp_client_test_t *)param;
	return st_write(ctx->socket, req, bytes, 2000*1000);
}

static int rtpport(void* param, int media, unsigned short *rtp)
{
	struct rtsp_client_test_t *ctx = (struct rtsp_client_test_t *)param;
	switch (ctx->transport)
	{
	case RTSP_TRANSPORT_RTP_UDP:
		assert(0 == rtp_socket_create(NULL, ctx->rtp[media], ctx->port[media]));
		*rtp = ctx->port[media][0];
		break;

	case RTSP_TRANSPORT_RTP_TCP:
		*rtp = 0;
		break;

	default:
		assert(0);
		return -1;
	}

	return 0;
}


static void onrtp(void* param, uint8_t channel, const void* data, uint16_t bytes)
{
	struct rtsp_client_test_t *ctx = (struct rtsp_client_test_t *)param;
	rtp_tcp_receiver_input(ctx->receiver[channel/2], channel, data, bytes);
}

static int ondescribe(void* param, const char* sdp)
{
	struct rtsp_client_test_t *ctx = (struct rtsp_client_test_t *)param;
	return rtsp_client_setup(ctx->rtsp, sdp);
}

static int onsetup(void* param)
{
	int i;
	uint64_t npt = 0;
	char ip[65];
	u_short rtspport;
	struct rtsp_client_test_t *ctx = (struct rtsp_client_test_t *)param;

	printf("%s => media_count:%d\n", __func__, rtsp_client_media_count(ctx->rtsp));
	
	// Session: 813629027556073;timeout=6
  const char *session = rtsp_client_get_header(ctx->rtsp, "Session");
  if(session)
  {
    char *p = strstr(session, "timeout=");
    int ses_timeout = (p)?(int)(atof(p+8)):0;
    ctx->ses_timeout = (ses_timeout >= 3 && ses_timeout <= 3*60)?ses_timeout:ctx->ses_timeout;
    printf("%s => ses_timeout:%d\n", __func__, ctx->ses_timeout);
  }
	
	ctx->stat = RTSP_CLIENT_STAT_SETUP;
	
	if(ctx->handler.onframe == NULL)
	{
  	assert(0 == rtsp_client_record(ctx->rtsp, &npt, NULL));
  	return 0;
  }
	
	assert(0 == rtsp_client_play(ctx->rtsp, &npt, NULL));
	
	
	for (i = 0; i < rtsp_client_media_count(ctx->rtsp); i++)
	{
		int payload, port[2];
		const char* encoding;
		const struct rtsp_header_transport_t* transport;
		transport = rtsp_client_get_media_transport(ctx->rtsp, i);
		encoding = rtsp_client_get_media_encoding(ctx->rtsp, i);
		payload = rtsp_client_get_media_payload(ctx->rtsp, i);
		
		printf("%s => i:%d, transport:%d, encoding:%s, payload:%d\n"
		      , __func__, i, transport->transport, encoding, payload);
		
		if (RTSP_TRANSPORT_RTP_UDP == transport->transport)
		{
			//assert(RTSP_TRANSPORT_RTP_UDP == transport->transport); // udp only
			assert(0 == transport->multicast); // unicast only
			assert(transport->rtp.u.client_port1 == ctx->port[i][0]);
			assert(transport->rtp.u.client_port2 == ctx->port[i][1]);

			port[0] = transport->rtp.u.server_port1;
			port[1] = transport->rtp.u.server_port2;

			if (*transport->source)
			{
        printf("%s => 111111 ctx:%p, localport[%d,%d], peer:%s, peerport[%d,%d]\n"
              , __func__, ctx, ctx->port[i][0], ctx->port[i][1], transport->source, port[0], port[1]);
				ctx->receiver[i] = rtp_udp_receiver_create(ctx->rtp[i], transport->source, port, payload, encoding, &ctx->handler);
			}
			else
			{
       // printf("%s => 222222 ctx:%p, localport[%d,%d], peer:%s, peerport[%d,%d]\n"
       //       , __func__, ctx, ctx->port[i][0], ctx->port[i][1], transport->source, port[0], port[1]);
				
				
				//socket_getpeername(st_netfd_fileno(ctx->socket), ip, &rtspport);
				//printf("%s => socket_getpeername ctx->socket:%d, ip:%s, port:%d\n", __func__, st_netfd_fileno(ctx->socket), ip, rtspport);
				ctx->receiver[i] = rtp_udp_receiver_create(ctx->rtp[i], ctx->host, port, payload, encoding, &ctx->handler);
			}
		}
		else if (RTSP_TRANSPORT_RTP_TCP == transport->transport)
		{
			//assert(transport->rtp.u.client_port1 == transport->interleaved1);
			//assert(transport->rtp.u.client_port2 == transport->interleaved2);
			ctx->receiver[i] = rtp_tcp_receiver_create(transport->interleaved1, transport->interleaved2, payload, encoding, &ctx->handler);
		}
		else
		{
			assert(0); // TODO
		}
	}

	return 0;
}

static int onteardown(void* param)
{
	return 0;
}

static int onplay(void* param, int media, const uint64_t *nptbegin, const uint64_t *nptend, const double *scale, const struct rtsp_rtp_info_t* rtpinfo, int count)
{
  struct rtsp_client_test_t *ctx = (struct rtsp_client_test_t *)param;
  ctx->stat = RTSP_CLIENT_STAT_PLAY;
	return 0;
}

static int onpause(void* param)
{
	return 0;
}

static int onannounce(void* param)
{
	struct rtsp_client_test_t *ctx = (struct rtsp_client_test_t *)param;
	return rtsp_client_setup(ctx->rtsp, ctx->sdp);
}


int onrecord(void* param, int media, const uint64_t *nptbegin, const uint64_t *nptend, const double *scale, const struct rtsp_rtp_info_t* rtpinfo, int count)
{
  int i = 0;
  struct rtsp_client_test_t *ctx = (struct rtsp_client_test_t *)param;
  ctx->stat = RTSP_CLIENT_STAT_PLAY;

  printf("%s => media_count:%d\n", __func__, rtsp_client_media_count(ctx->rtsp));
#if 1
	for (i = 0; i < rtsp_client_media_count(ctx->rtsp); i++)
	{
		int payload;
		unsigned short port[2];
		const char* encoding;
		const struct rtsp_header_transport_t* transport;
		transport = rtsp_client_get_media_transport(ctx->rtsp, i);
		encoding = rtsp_client_get_media_encoding(ctx->rtsp, i);
		payload = rtsp_client_get_media_payload(ctx->rtsp, i);
		if (RTSP_TRANSPORT_RTP_UDP == transport->transport)
		{
			//assert(RTSP_TRANSPORT_RTP_UDP == transport->transport); // udp only
			assert(0 == transport->multicast); // unicast only
			assert(transport->rtp.u.client_port1 == ctx->port[i][0]);
			assert(transport->rtp.u.client_port2 == ctx->port[i][1]);

			port[0] = transport->rtp.u.server_port1;
			port[1] = transport->rtp.u.server_port2;

			if (*transport->source)
			{
        printf("%s => UDP have source ctx:%p, localport[%d,%d], peer:%s, peerport[%d,%d]\n"
              , __func__, ctx, ctx->port[i][0], ctx->port[i][1], transport->source, port[0], port[1]); 
                 
        int ret = ctx->media->add_transport(ctx->media, 
                  (strstr(encoding, "H264")||strstr(encoding, "H265"))?"video":"audio",
                  rtp_udp_transport_new2(ctx->rtp[i], transport->source, port));
			}
			else
			{
        printf("%s => UDP used host ctx:%p, localport[%d,%d], peer:%s, peerport[%d,%d]\n"
              , __func__, ctx, ctx->port[i][0], ctx->port[i][1], ctx->host, port[0], port[1]);

        int ret = ctx->media->add_transport(ctx->media, 
                  (strstr(encoding, "H264")||strstr(encoding, "H265"))?"video":"audio",
                  rtp_udp_transport_new2(ctx->rtp[i], ctx->host, port));
			}
		}
		else if (RTSP_TRANSPORT_RTP_TCP == transport->transport)
		{
			//assert(transport->rtp.u.client_port1 == transport->interleaved1);
			//assert(transport->rtp.u.client_port2 == transport->interleaved2);          
      int ret = ctx->media->add_transport(ctx->media,
                (strstr(encoding, "H264")||strstr(encoding, "H265"))?"video":"audio",
                rtp_tcp_transport_new2(ctx->rtsp, transport->interleaved1, transport->interleaved2, (int(*)(void*,const void*,size_t))rtsp_client_send_interleaved_data));
		}
		else
		{
			assert(0); // TODO
		}
	}
	ctx->media->play(ctx->media);
#endif

  return 0;
}



void* handle_connect(void *arg)
{
  struct rtsp_client_test_t *ctx = (struct rtsp_client_test_t *)arg;
  int r;
	struct rtsp_client_handler_t handler;
  int packet_size = 1*1024*1024;
  char *packet = NULL;
  
  int sock = 0;
  if ((sock = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
    printf("socket err.\n");
    goto __exit;
  }
  if ((ctx->socket = st_netfd_open_socket(sock)) == NULL) {
    printf("st_netfd_open_socket err.\n");
    close(sock);
    goto __exit;
  }

	packet = malloc(packet_size);

	handler.send = rtsp_client_send;
	handler.rtpport = rtpport;
	handler.ondescribe = ondescribe;
	handler.onsetup = onsetup;
	handler.onplay = onplay;
	handler.onpause = onpause;
	handler.onteardown = onteardown;
	handler.onrtp = onrtp;
	handler.onannounce = onannounce;
	handler.onrecord = onrecord;
	snprintf(packet, packet_size, "rtsp://%s/%s", ctx->host, ctx->file); // url
  
  struct sockaddr_in rmt_addr;
  rmt_addr.sin_family = AF_INET;
  rmt_addr.sin_port = htons(ctx->rtsp_port);
  rmt_addr.sin_addr.s_addr = inet_addr(ctx->host);
  
  if(st_connect(ctx->socket, (struct sockaddr*)&rmt_addr, sizeof(rmt_addr), ST_UTIME_NO_TIMEOUT) < 0)
  {
    printf("st_connect err:%s, peer:%s, port:%d\n", strerror(errno), ctx->host, ctx->rtsp_port);
    goto __exit;
  }
	
	ctx->rtsp = rtsp_client_create(packet, ctx->user, ctx->pwd, &handler, ctx);
	assert(ctx->rtsp);
	
	if(ctx->handler.onframe == NULL)
  {

    static const char* pattern_live =
  		"v=0\n"
  		"o=- %llu %llu IN IP4 %s\n"
  		"s=%s\n"
  		"c=IN IP4 0.0.0.0\n"
  		"t=0 0\n"
  		"a=control:*\n";

    ctx->media = rtp_media_live_new(ctx->handler.ch, ctx->handler.st);
    snprintf(ctx->sdp, sizeof(ctx->sdp), pattern_live, ntp64_now(), ntp64_now(), "0.0.0.0", "hiview");
    ctx->media->get_sdp(ctx->media, ctx->sdp);
    assert(0 == rtsp_client_announce(ctx->rtsp, ctx->sdp));
  }
  else
 	{
	  assert(0 == rtsp_client_describe(ctx->rtsp));
  }

	r = st_read(ctx->socket, packet, packet_size, ST_UTIME_NO_TIMEOUT);
	while(r > 0)
	{
	  if((r = rtsp_client_input(ctx->rtsp, packet, r)) != 0)
	  {
	     printf("rtsp_client_input err r:%d packet[%s]\n", r, packet);
	  }
		r = st_read(ctx->socket, packet, packet_size, ST_UTIME_NO_TIMEOUT);		
		
		#if 0 // io-collect
		if(ctx->stat == RTSP_CLIENT_STAT_PLAY 
		  && ctx->transport == RTSP_TRANSPORT_RTP_TCP)
		{
		  if(r > 0 && r < 8*1024)
		  {
		    st_usleep(40*1000);
		  }
		}
		#endif

	}
  
  if(ctx->stat >= RTSP_CLIENT_STAT_SETUP)
    rtsp_client_teardown(ctx->rtsp);
	
	rtsp_client_destroy(ctx->rtsp);
	
__exit:
  printf("%s => exit ctx:%p\n", __func__, ctx);
  if(packet)
    free(packet);
	if(ctx->socket)
	  st_netfd_close(ctx->socket);
  
  if(ctx->handler.onerr)
  {
    ctx->handler.onerr(ctx->handler.param, -1);
  }

  ctx->exited = 1;
  st_thread_exit(NULL);
  return NULL;
}

char *rtsp_url_parse(char *in, char *host, int* port, char* path, char *user, char *pwd)
{   
    char *p = strstr(in, "rtsp://");

    if(!p) return in;
       
    p += strlen("rtsp://");

    /* get rtsp://xxx/ */
    char *h = strsep(&p, "/");
  
    if(!h) return in; 
    
    char *ip = NULL;   
    /* goto @ after */
    if(ip = strrchr(h, '@'))
    {
        if(user && pwd)
        {        
            *ip = '\0';
             sscanf(h, "%[^:]:%s", user, pwd);
        }
        h = ip+strlen("@");
    }
    /* get ip */
    ip = strsep(&h, ":");
    
    if(ip){ if(host) strcpy(host, ip);}
   
    /* get port */
    if(h) 
        if(port) *port = atoi(h);
    else
        if(port) *port = 554;

    /* append /path */
    if(p){ if(path) strcpy(path, p);}

    return in;
}

void* rtsp_client_connect(const char* url, int protol, struct st_rtsp_client_handler_t *_handler)
{
  struct rtsp_client_test_t *ctx = calloc(1, sizeof(struct rtsp_client_test_t));

  char _url[256] = {0};
  strcpy(_url, url);
  rtsp_url_parse(_url, ctx->host, &ctx->rtsp_port, ctx->file, ctx->user, ctx->pwd);

  ctx->handler = *_handler;
	ctx->transport = protol;
	//ctx->transport = RTSP_TRANSPORT_RTP_UDP;
  //ctx->transport = RTSP_TRANSPORT_RTP_TCP;
  
  //init last_keeptime, ses_timeout;
  struct timespec _ts;
	clock_gettime(CLOCK_MONOTONIC, &_ts);
  ctx->last_keeptime = _ts.tv_sec;
  ctx->ses_timeout = 60;
  //printf("%s => ses_timeout:%d\n", __func__, ctx->ses_timeout);
  
  ctx->loop = 1; ctx->exited = !ctx->loop;
  if ((ctx->tid = st_thread_create(handle_connect, ctx, 0, 0)) == NULL) {
    free(ctx);
    printf("st_thread_create err.\n");
    return NULL;
  }

  printf("%s => new ctx:%p\n", __func__, ctx);
  return (void*)ctx;  
}


int rtsp_client_close(void* st)
{
  struct rtsp_client_test_t *ctx = (struct rtsp_client_test_t*)st;
  if(!ctx)
    return -1;

  int i = 0;
  for(i = 0; i < 5; i++)
  {
    if(ctx->receiver[i])
      ctx->receiver[i]->free(ctx->receiver[i]);
  }
  
  if(ctx->media)
    rtp_media_live_free(ctx->media);

  if(ctx->tid && !ctx->exited) //check tid is valid
  {
    st_thread_interrupt(ctx->tid);
    ctx->loop = 0;
    while(!ctx->exited)
      st_usleep(10*1000);
  }
  printf("%s => interrupt && free ctx:%p\n", __func__, ctx);
  free(ctx);
  return 0;
}

int rtsp_client_options(rtsp_client_t *rtsp, const char* commands);
int rtsp_client_keepalive(void* st, int media)
{
  struct rtsp_client_test_t *ctx = (struct rtsp_client_test_t*)st;

  if(!ctx || !ctx->rtsp)
    return -1;
    
  struct timespec _ts;
	clock_gettime(CLOCK_MONOTONIC, &_ts);
	if(_ts.tv_sec - ctx->last_keeptime >= ctx->ses_timeout*0.8)
	{
	  ctx->last_keeptime = _ts.tv_sec; 
    printf("ctx:%p, ses_timeout:%d, GET_PARAMETER.\n", ctx, ctx->ses_timeout);
	  extern int rtsp_client_get_parameter(struct rtsp_client_t *rtsp, int media, const char* parameter);
		//rtsp_client_get_parameter(ctx->rtsp, 2, NULL);
		rtsp_client_get_parameter(ctx->rtsp, media, NULL);
	}
	else
	{
	  //printf("ctx:%p, ses_timeout:%d, after:%d\n", ctx, ctx->ses_timeout, _ts.tv_sec - ctx->last_keeptime);
	}
  return 0;
}