#ifndef _rtp_receiver_h_
#define _rtp_receiver_h_

struct rtp_context_t
{
  char encoding[64];
	u_short    port[2];
	st_netfd_t socket[2];
	struct sockaddr_storage ss;
	socklen_t len;

	char rtp_buffer[32*1024];
	char rtcp_buffer[32*1024];

	void* payload;
	void* rtp;
	
  int loop, exited;
  st_thread_t tid;
  void (*free)(struct rtp_context_t *ctx);
  
  void *onparam;
  int (*onframe)(void* param, const char*encoding, const void *packet, int bytes, uint32_t time, int flags);
};

struct rtp_context_t* rtp_udp_receiver_create2(st_netfd_t rtp[2], const char* peer, int peerport[2], int payload, const char* encoding, void* onframe, void* onparam);
struct rtp_context_t* rtp_tcp_receiver_create2(uint8_t interleave1, uint8_t interleave2, int payload, const char* encoding, void* onframe, void* onparam);


void  rtp_tcp_receiver_input(struct rtp_context_t* ctx, uint8_t channel, const void* data, uint16_t bytes);


#endif //!_rtp_receiver_h_