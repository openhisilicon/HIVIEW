#ifndef _rtp_transport_h_
#define _rtp_transport_h_

#include "rtsp-server-st.h"
#include "rtp-socket.h"

struct rtp_transport_t
{
  int istcp;
  int(*send)(struct rtp_transport_t *t, int rtcp, const void* data, size_t bytes);
  int(*recv)(struct rtp_transport_t *t, int rtcp, const void* data, size_t bytes);
  void(*free)(struct rtp_transport_t *t);
};

struct rtp_transport_t*
    rtp_udp_transport_new(const char* ip, unsigned short port[2]);
struct rtp_transport_t*
    rtp_tcp_transport_new(rtsp_server_t* rtsp, uint8_t rtp, uint8_t rtcp);

struct rtp_transport_t*
    rtp_udp_transport_new2(st_netfd_t rtp[2], const char* ip, unsigned short port[2]);

struct rtp_transport_t*
    rtp_tcp_transport_new2(void* rtsp, uint8_t rtp, uint8_t rtcp,
                           int (*send)(void* rtsp, const void* data, size_t bytes));


#endif /* !_rtp_transport_h_ */
