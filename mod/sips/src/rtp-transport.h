#ifndef _rtp_transport_h_
#define _rtp_transport_h_

#include "rtp-socket.h"

struct rtp_transport_t
{
  int istcp;
  int(*send)(struct rtp_transport_t *t, int rtcp, const void* data, size_t bytes);
  int(*recv)(struct rtp_transport_t *t, int rtcp, const void* data, size_t bytes);
  int(*conn)(struct rtp_transport_t *t);
  void(*free)(struct rtp_transport_t *t);
};

struct rtp_transport_t*
    rtp_udp_transport_new(const char* ip, unsigned short port[2]);

struct rtp_transport_t*
    rtp_udp_transport_new2(st_netfd_t rtp[2], const char* ip, unsigned short port[2]);

struct rtp_transport_t*
    rtp_tcp_transport_new(const char* dst, unsigned short port[2], int active);


#endif /* !_rtp_transport_h_ */
