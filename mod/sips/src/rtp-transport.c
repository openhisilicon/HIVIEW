#include "rtp-transport.h"

#include "rtp-header.h"
#include "rtp-util.h"

struct rtp_tcp_transport_t 
{
	struct rtp_transport_t base;
	int active;
	st_netfd_t m_socket[2];
	st_netfd_t m_child[2];
	char dst[64];
	unsigned short port[2];
	uint8_t m_packet[2 + (1 << 16)];
	int (*send)(void* rtsp, const void* data, size_t bytes);
};


static int rtp_tcp_transport_recv(struct rtp_transport_t* t, int rtcp, const void* data, size_t bytes)
{
  return 0;
}

static int rtp_tcp_transport_send(struct rtp_transport_t* t, int rtcp, const void* data, size_t bytes)
{
  struct rtp_tcp_transport_t* transport = (struct rtp_tcp_transport_t*)t; 
  
	assert(bytes < (1 << 16));
	if (bytes >= (1 << 16))
		return E2BIG;

	transport->m_packet[0] = (bytes >> 8) & 0xFF;
	transport->m_packet[1] = bytes & 0xff;
	memcpy(transport->m_packet + 2, data, bytes);
	
	return st_write((transport->m_child[0])?transport->m_child[0]:transport->m_socket[0], transport->m_packet, bytes + 2, ST_UTIME_NO_TIMEOUT);
}

#include <sys/types.h>  
#include <sys/socket.h>

static int rtp_tcp_transport_conn(struct rtp_transport_t* t)
{
  struct rtp_tcp_transport_t* transport = (struct rtp_tcp_transport_t*)t;
  
  struct sockaddr_in rmt_addr;
  rmt_addr.sin_family = AF_INET;
  rmt_addr.sin_port = htons(transport->port[0]);
  rmt_addr.sin_addr.s_addr = inet_addr(transport->dst);
  
  if(transport->active)
  {  
    if(st_connect(transport->m_socket[0], (struct sockaddr*)&rmt_addr, sizeof(rmt_addr), ST_UTIME_NO_TIMEOUT) < 0)
    {
      printf("ACTIVE connect dst[%s:%d] err.\n", transport->dst, transport->port[0]);
      return -1;
    }
  }
  else
  {
    int addrlen = 0;
    struct sockaddr_in  cli_addr;
    listen(st_netfd_fileno(transport->m_socket[0]), 12);
    transport->m_child[0] = st_accept(transport->m_socket[0], (struct sockaddr *)&cli_addr, &addrlen, ST_UTIME_NO_TIMEOUT);
    printf("PASSIVE accept peer_port:%d, m_child:%d\n", transport->port[0], transport->m_child[0]);
  }
  
  return 0;
}



static void rtp_tcp_transport_free(struct rtp_transport_t* t)
{
  struct rtp_tcp_transport_t* transport = (struct rtp_tcp_transport_t*)t;

  if(transport)
  {
    int i;
  	for (i = 0; i < 2; i++)
  	{
  		if (transport->m_socket[i])
  			st_netfd_close(transport->m_socket[i]);
  		transport->m_socket[i] = NULL;
  		
  		if (transport->m_child[i])
  			st_netfd_close(transport->m_child[i]);
  		transport->m_child[i] = NULL;
  	}
  	free(transport);
  }
}

struct rtp_transport_t*
    rtp_tcp_transport_new(const char* dst, unsigned short port[2], int active)
{
  struct rtp_tcp_transport_t* transport = calloc(1, sizeof(struct rtp_tcp_transport_t));
  
  transport->base.istcp= 1;
  transport->base.send = rtp_tcp_transport_send;
  transport->base.recv = rtp_tcp_transport_recv;
  transport->base.conn = rtp_tcp_transport_conn;
  transport->base.free = rtp_tcp_transport_free;
  transport->active = active;
  
  strncpy(transport->dst, dst, sizeof(transport->dst)-1);
  transport->port[0] = port[0];
  transport->port[1] = port[1];
  
	if(rtp_socket_create(NULL, SOCK_STREAM, transport->m_socket, port) < 0)
	{
	  free(transport);
	  return NULL;
	}
	
  return (struct rtp_transport_t*)transport;
}


struct rtp_udp_transport_t
{
  struct rtp_transport_t base;
	st_netfd_t m_socket[2];
	socklen_t m_addrlen[2];
	struct sockaddr_storage m_addr[2];
};

static int rtp_udp_transport_recv(struct rtp_transport_t* t, int rtcp, const void* data, size_t bytes)
{
    struct rtp_udp_transport_t* transport = (struct rtp_udp_transport_t*)t; 
  
    int i = rtcp ? 1 : 0;
    if(i)
    {
      struct sockaddr from; int fromlen;
      return st_recvfrom(transport->m_socket[i], (void*)data, bytes, &from, &fromlen, ST_UTIME_NO_WAIT);
    }
    return 0;
}

static int rtp_udp_transport_conn(struct rtp_transport_t* t)
{
  return 0;
}

static int rtp_udp_transport_send(struct rtp_transport_t* t, int rtcp, const void* data, size_t bytes)
{
  struct rtp_udp_transport_t* transport = (struct rtp_udp_transport_t*)t; 
  
	int i = rtcp ? 1 : 0;
	
	//dump rtp-pkt; 
  #if 0
	if(i)
	{
  	char ip[SOCKET_ADDRLEN]; u_short port;
    socket_addr_to((struct sockaddr*)&transport->m_addr[i], transport->m_addrlen[i], ip, &port);
    printf("%s => RTCP pid:%d, i:%d, bytes:%d to [%s:%d]\n", __func__, getpid(), i, bytes, ip, port);
  }
  else
  {
    rtp_header_t rtp;
    const uint8_t *ptr = (const unsigned char *)data;
    uint32_t v = nbo_r32(ptr);
    
    printf("%s => RTP pid:%d, rtp[m:%d,pt:%d,seq:%d,ts:%u,ssrc:%u,bytes:%d]\n"
          , __func__, getpid(), RTP_M(v), RTP_PT(v), RTP_SEQ(v), nbo_r32(ptr+4), nbo_r32(ptr+8), bytes);
  }
  #endif
  
	return st_sendto(transport->m_socket[i], data, bytes, (struct sockaddr*)&transport->m_addr[i], transport->m_addrlen[i], ST_UTIME_NO_TIMEOUT);
}

static void rtp_udp_transport_free(struct rtp_transport_t* t)
{
  struct rtp_udp_transport_t *transport = (struct rtp_udp_transport_t *)t;
  if(transport)
  {
    int i;
  	for (i = 0; i < 2; i++)
  	{
  		if (transport->m_socket[i])
  			st_netfd_close(transport->m_socket[i]);
  		transport->m_socket[i] = NULL;
  	}
  	free(transport);
  }
}

struct rtp_transport_t* rtp_udp_transport_new(const char* ip, unsigned short port[2])
{
  return rtp_udp_transport_new2(NULL, ip, port);
}

struct rtp_transport_t*
    rtp_udp_transport_new2(st_netfd_t rtp[2], const char* ip, unsigned short port[2])
{
  struct rtp_udp_transport_t *transport = calloc(1, sizeof(struct rtp_udp_transport_t));
  
  transport->base.send = rtp_udp_transport_send;
  transport->base.recv = rtp_udp_transport_recv;
  transport->base.conn = rtp_udp_transport_conn;
  transport->base.free = rtp_udp_transport_free;
  
	int r1 = socket_addr_from(&transport->m_addr[0], &transport->m_addrlen[0], ip, port[0]);
	int r2 = socket_addr_from(&transport->m_addr[1], &transport->m_addrlen[1], ip, port[1]);
	if (0 != r1 || 0 != r2)
	{
	  free(transport);
	  return NULL;
	}
	
	if(rtp)
  {
  	transport->m_socket[0] = rtp[0];
  	transport->m_socket[1] = rtp[1];
	  return (struct rtp_transport_t*)transport;
	}
	
	if(rtp_socket_create(NULL, SOCK_DGRAM, transport->m_socket, port) < 0)
	{
	  free(transport);
	  return NULL;
	}
	
  return (struct rtp_transport_t*)transport;
}


