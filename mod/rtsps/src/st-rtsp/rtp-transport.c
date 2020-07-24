#include "rtp-transport.h"


struct rtp_tcp_transport_t 
{
	struct rtp_transport_t base;
	uint8_t m_rtp;
	uint8_t m_rtcp;
	void* m_rtsp;
	uint8_t m_packet[4 + (1 << 16)];
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

	transport->m_packet[0] = '$';
	transport->m_packet[1] = rtcp ? transport->m_rtcp : transport->m_rtp;
	transport->m_packet[2] = (bytes >> 8) & 0xFF;
	transport->m_packet[3] = bytes & 0xff;
	memcpy(transport->m_packet + 4, data, bytes);
	int r = transport->send(transport->m_rtsp, transport->m_packet, bytes + 4);
	return 0 == r ? bytes : r;
}

static void rtp_tcp_transport_free(struct rtp_transport_t* t)
{
  struct rtp_tcp_transport_t* transport = (struct rtp_tcp_transport_t*)t;
  if(transport)
    free(transport);
}

struct rtp_transport_t* rtp_tcp_transport_new(rtsp_server_t* rtsp, uint8_t rtp, uint8_t rtcp)
{
  return rtp_tcp_transport_new2(rtsp, rtp, rtcp, rtsp_server_send_interleaved_data);
}


struct rtp_transport_t*
    rtp_tcp_transport_new2(void* rtsp, uint8_t rtp, uint8_t rtcp,
                           int (*send)(void* rtsp, const void* data, size_t bytes))
{
  struct rtp_tcp_transport_t* transport = calloc(1, sizeof(struct rtp_tcp_transport_t));
  
  transport->base.istcp= 1;
  transport->base.send = rtp_tcp_transport_send;
  transport->base.recv = rtp_tcp_transport_recv;
  transport->base.free = rtp_tcp_transport_free;
  transport->m_rtsp = rtsp;
  transport->m_rtp  = rtp;
  transport->m_rtcp = rtcp;
  transport->send = send;
  
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

static int rtp_udp_transport_send(struct rtp_transport_t* t, int rtcp, const void* data, size_t bytes)
{
  struct rtp_udp_transport_t* transport = (struct rtp_udp_transport_t*)t; 
  
	int i = rtcp ? 1 : 0;
	if(i)
	{
  	char ip[SOCKET_ADDRLEN]; u_short port;
    socket_addr_to((struct sockaddr*)&transport->m_addr[i], transport->m_addrlen[i], ip, &port);
    printf("%s => pid:%d, i:%d, bytes:%d to[%s:%d]\n", __func__, getpid(), i, bytes, ip, port);
  }
  
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
	
	if(rtp_socket_create(NULL, transport->m_socket, port) < 0)
	{
	  free(transport);
	  return NULL;
	}
	
  return (struct rtp_transport_t*)transport;
}


