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

#include "rtp-socket.h"
#include "rtp-profile.h"
#include "rtp-payload.h"
#include "rtp.h"
#include "time64.h"

#include "rtp-receiver.h"

#if defined(WIN32)
#define strcasecmp _stricmp
#endif


static int rtp_read(struct rtp_context_t* ctx, st_netfd_t s)
{
	int r;
	socklen_t len;
	struct sockaddr_storage ss;
	len = sizeof(ss);

	//r = st_recvfrom(s, ctx->rtp_buffer, sizeof(ctx->rtp_buffer), (struct sockaddr*)&ss, &len, ST_UTIME_NO_TIMEOUT);
  //printf("%s => r:%d, recvform port:%d, ctx->port[0]:%d\n", __func__, r, ntohs(((struct sockaddr_in*)&ss)->sin_port), ctx->port[0]);
  do
  {
    r = recvfrom(st_netfd_fileno(s), ctx->rtp_buffer, sizeof(ctx->rtp_buffer), MSG_DONTWAIT, (struct sockaddr*)&ss, &len);
    if(r <= 0)
    {
      if (errno == EINTR)
        continue;
        
      if (!((errno == EAGAIN) || (errno == EWOULDBLOCK)))
        return -1;
      //empty;
      return 0;
    }
    #if 1
  	assert(AF_INET == ss.ss_family);
  	if(((struct sockaddr_in*)&ss)->sin_port != htons(ctx->port[0]))
    {
      
      char ip[SOCKET_ADDRLEN];
      u_short lport;
      
      socket_getname(st_netfd_fileno(s), ip, &lport);
      
      printf("%s @@@@@@ =>ctx:%p, local_port:%d, from_port:%d, cfg_port[0]:%d\n"
            , __func__, ctx, lport, ntohs(((struct sockaddr_in*)&ss)->sin_port), ctx->port[0]);
      
      printf("\033[0;31m" "\n\n  ctx:%p, exit.\n\n" "\033[0000m", ctx);
      
      exit(0);
    }
    
  	if(0)//if(memcmp(&((struct sockaddr_in*)&ss)->sin_addr, &((struct sockaddr_in*)&ctx->ss)->sin_addr, 4))
  	{
  	   char ssip[64] = {0};
  	   char ctxip[64] = {0};
  	   inet_ntop(AF_INET, &((struct sockaddr_in*)&ss)->sin_addr, ssip, sizeof(ssip));
  	   inet_ntop(AF_INET, &((struct sockaddr_in*)&ctx->ss)->sin_addr, ctxip, sizeof(ctxip));
  	   printf("%s => ctxip:%s, ssip:%s\n", __func__, ctxip, ssip);
  	}
  	rtp_payload_decode_input(ctx->payload, ctx->rtp_buffer, r);
  	rtp_onreceived(ctx->rtp, ctx->rtp_buffer, r);
    #endif
  }while(1);
	return r;
}

static int rtcp_read(struct rtp_context_t* ctx, st_netfd_t s)
{
	int r;
	socklen_t len;
	struct sockaddr_storage ss;
	len = sizeof(ss);
	
	//r = st_recvfrom(s, ctx->rtcp_buffer, sizeof(ctx->rtcp_buffer), (struct sockaddr*)&ss, &len, ST_UTIME_NO_TIMEOUT);
  //printf("%s => r:%d, recvform port:%d, ctx->port[0]:%d\n", __func__, r, ntohs(((struct sockaddr_in*)&ss)->sin_port), ctx->port[1]);
	do
	{
    r = recvfrom(st_netfd_fileno(s), ctx->rtcp_buffer, sizeof(ctx->rtcp_buffer), MSG_DONTWAIT, (struct sockaddr*)&ss, &len);
    if(r <= 0)
    {
      if (errno == EINTR)
        continue;
        
      if (!((errno == EAGAIN) || (errno == EWOULDBLOCK)))
        return -1;
      //empty;
      return 0;
    }
    #if 1
  	assert(AF_INET == ss.ss_family);
  	assert(((struct sockaddr_in*)&ss)->sin_port == htons(ctx->port[1]));
  	assert(0 == memcmp(&((struct sockaddr_in*)&ss)->sin_addr, &((struct sockaddr_in*)&ctx->ss)->sin_addr, 4));
  	r = rtp_onreceived_rtcp(ctx->rtp, ctx->rtcp_buffer, r);
  	#endif
  }while(1);
	return r;
}

static int rtp_receiver(struct rtp_context_t* ctx, st_netfd_t rtp[2], int timeout)
{
	int i, r;
	int interval;
	time64_t clock, mr_clock;
	struct pollfd fds[2];

	for (i = 0; i < 2; i++)
	{
		fds[i].fd = st_netfd_fileno(rtp[i]);
		fds[i].events = POLLIN;
	}

	clock = mr_clock = time64_now();
	while (ctx->loop)
	{
	  #if 0
		// RTCP report
		interval = rtp_rtcp_interval(ctx->rtp);
		if (clock + interval < time64_now())
		{
			r = rtp_rtcp_report(ctx->rtp, ctx->rtcp_buffer, sizeof(ctx->rtcp_buffer));
			r = st_write(rtp[1], ctx->rtcp_buffer, r, timeout*1000);
			clock = time64_now();
			printf("ctx:%p, interval:%d, clock:%lld, send RR.\n", ctx, interval, clock);
		}
		#endif

    fds[0].revents = 0;
    fds[1].revents = 0;

    if (st_poll(fds, 2, ST_UTIME_NO_TIMEOUT) <= 0) {
      printf("%s => st_poll err.\n", __func__);
      break;
    }
    
    if (fds[0].revents & POLLIN) {
      if(rtp_read(ctx, rtp[0]) < 0)
      {
        printf("%s => rtp_read err.\n", __func__);
        break;    
      }
    }

    if (fds[1].revents & POLLIN) {
      if(rtcp_read(ctx, rtp[1]) < 0)
      {
        printf("%s => rtcp_read err.\n", __func__);
        break;
      }
    }
    
    #if 0 // io-collect
    st_usleep(40*1000);
    #endif
	}
	return r;
}

static void rtp_packet(void* param, const void *packet, int bytes, uint32_t time, int flags)
{
	const uint8_t start_code[] = { 0, 0, 0, 1 };
	struct rtp_context_t* ctx;
	ctx = (struct rtp_context_t*)param;
	if (0 == strcmp("H264", ctx->encoding) || 0 == strcmp("H265", ctx->encoding))
	{
		//fwrite(start_code, 1, 4, ctx->fp);
	}
	else if (0 == strcasecmp("mpeg4-generic", ctx->encoding))
	{
		uint8_t adts[7];
		int len = bytes + 7;
		uint8_t profile = 2;
		uint8_t sampling_frequency_index = 4;
		uint8_t channel_configuration = 2;
		adts[0] = 0xFF; /* 12-syncword */
		adts[1] = 0xF0 /* 12-syncword */ | (0 << 3)/*1-ID*/ | (0x00 << 2) /*2-layer*/ | 0x01 /*1-protection_absent*/;
		adts[2] = ((profile - 1) << 6) | ((sampling_frequency_index & 0x0F) << 2) | ((channel_configuration >> 2) & 0x01);
		adts[3] = ((channel_configuration & 0x03) << 6) | ((len >> 11) & 0x03); /*0-original_copy*/ /*0-home*/ /*0-copyright_identification_bit*/ /*0-copyright_identification_start*/
		adts[4] = (uint8_t)(len >> 3);
		adts[5] = ((len & 0x07) << 5) | 0x1F;
		adts[6] = 0xFC | ((len / 1024) & 0x03);
		//fwrite(adts, 1, sizeof(adts), ctx->fp);
	}
	else if (0 == strcmp("MP4A-LATM", ctx->encoding))
	{
		// add ADTS header
	}
	
	//fwrite(packet, 1, bytes, ctx->fp);
	if(ctx->handler.onframe)
	{
	  ctx->handler.onframe(ctx->handler.param, ctx->encoding, packet, bytes, time, flags);
	}
}

static void rtp_on_rtcp(void* param, const struct rtcp_msg_t* msg)
{
	struct rtp_context_t* ctx;
	ctx = (struct rtp_context_t*)param;
	if (RTCP_MSG_BYE == msg->type)
	{
		printf("finished\n");
	}
}

static void* rtp_worker(void* param)
{
	struct rtp_context_t* ctx;
	ctx = (struct rtp_context_t*)param;

	rtp_receiver(ctx, ctx->socket, 2000);

  ctx->exited = 1; 
  printf("%s => ctx:%p, exit.\n", __func__, ctx);
  
  st_thread_exit(NULL);
	return NULL;
}

void  rtp_udp_receiver_destroy(struct rtp_context_t* ctx)
{
  if(!ctx)
    return;

  if(ctx->tid && !ctx->exited) //check tid is valid
  {
    st_thread_interrupt(ctx->tid);
    ctx->loop = 0;
    while(!ctx->exited)
      st_usleep(10*1000);
  }
  
  if(ctx->rtp)
    rtp_destroy(ctx->rtp);
	if(ctx->payload)
	  rtp_payload_decode_destroy(ctx->payload);
	if(ctx->socket[0])
	  st_netfd_close(ctx->socket[0]);
	if(ctx->socket[1])
	  st_netfd_close(ctx->socket[1]);
  free(ctx);
}

struct rtp_context_t* rtp_udp_receiver_create(st_netfd_t rtp[2], const char* peer, int peerport[2], int payload, const char* encoding, struct st_rtsp_client_handler_t* _handler)
{
	size_t n;
	pthread_t t;
	struct rtp_context_t* ctx;
	struct rtp_event_t evthandler;
	struct rtp_payload_t handler;
	const struct rtp_profile_t* profile;

	ctx = malloc(sizeof(*ctx));

	ctx->handler = *_handler;
	profile = rtp_profile_find(payload);
	
	handler.alloc = NULL;
	handler.free = NULL;
	handler.packet = rtp_packet;
	ctx->payload = rtp_payload_decode_create(payload, encoding, &handler, ctx);
	if (NULL == ctx->payload)
	{
	  goto __err; // ignore 
	}
	
	evthandler.on_rtcp = rtp_on_rtcp;
	ctx->rtp = rtp_create(&evthandler, &ctx, (uint32_t)(intptr_t)&ctx, (uint32_t)(intptr_t)&ctx, profile ? profile->frequency : 9000, 1*1024*1024, 0);

	assert(0 == socket_addr_from(&ctx->ss, &ctx->len, peer, (u_short)peerport[0]));
	//assert(0 == socket_addr_setport((struct sockaddr*)&ss, len, (u_short)peerport[0]));
	//assert(0 == connect(rtp[0], (struct sockaddr*)&ss, len));
	assert(0 == socket_addr_setport((struct sockaddr*)&ctx->ss, ctx->len, (u_short)peerport[1]));
	//assert(0 == connect(rtp[1], (struct sockaddr*)&ss, len));

	snprintf(ctx->encoding, sizeof(ctx->encoding), "%s", encoding);
	ctx->socket[0] = rtp[0];
	ctx->socket[1] = rtp[1];
	ctx->port[0] = (u_short)peerport[0];
	ctx->port[1] = (u_short)peerport[1];

  ctx->loop = 1; ctx->exited = !ctx->loop;
  if ((ctx->tid = st_thread_create(rtp_worker, ctx, 0, 0)) == NULL) {
    printf("st_thread_create err.\n");
    goto __err;
  }
  
  ctx->free = rtp_udp_receiver_destroy;
  return ctx;
__err:
  if(ctx->rtp)
    rtp_destroy(ctx->rtp);
	if(ctx->payload)
	  rtp_payload_decode_destroy(ctx->payload);
	if(rtp[0])
	  st_netfd_close(rtp[0]);
	if(rtp[1])
	  st_netfd_close(rtp[1]);
  free(ctx);
  return NULL;
}

void  rtp_tcp_receiver_destroy(struct rtp_context_t* ctx)
{
  if(!ctx)
    return;
  
  if(ctx->rtp)
    rtp_destroy(ctx->rtp);
	if(ctx->payload)
	  rtp_payload_decode_destroy(ctx->payload);

  free(ctx);
}


struct rtp_context_t* rtp_tcp_receiver_create(uint8_t interleave1, uint8_t interleave2, int payload, const char* encoding, struct st_rtsp_client_handler_t* _handler)
{
	struct rtp_context_t* ctx;
	struct rtp_event_t evthandler;
	struct rtp_payload_t handler;
	const struct rtp_profile_t* profile;

	ctx = malloc(sizeof(struct rtp_context_t));

  ctx->handler   = *_handler;

	profile = rtp_profile_find(payload);

	handler.alloc = NULL;
	handler.free = NULL;
	handler.packet = rtp_packet;
	ctx->payload = rtp_payload_decode_create(payload, encoding, &handler, ctx);
	if (NULL == ctx->payload)
	{
	  goto __err; // ignore
	}

	evthandler.on_rtcp = rtp_on_rtcp;
	ctx->rtp = rtp_create(&evthandler, &ctx, (uint32_t)(intptr_t)&ctx, (uint32_t)(intptr_t)&ctx, profile ? profile->frequency : 9000, 1*1024*1024, 0);

	snprintf(ctx->encoding, sizeof(ctx->encoding), "%s", encoding);
	ctx->free = rtp_tcp_receiver_destroy;
	return ctx;
	
__err:
  if(ctx->rtp)
    rtp_destroy(ctx->rtp);
	if(ctx->payload)
	  rtp_payload_decode_destroy(ctx->payload);
	  
	free(ctx);
  return NULL;
}


void rtp_tcp_receiver_input(struct rtp_context_t* ctx, uint8_t channel, const void* data, uint16_t bytes)
{
	if (0 == channel % 2)
	{
		rtp_payload_decode_input(ctx->payload, data, bytes);
		rtp_onreceived(ctx->rtp, data, bytes);
	}
	else
	{
		rtp_onreceived_rtcp(ctx->rtp, data, bytes);
	}
}
