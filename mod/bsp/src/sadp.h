#ifndef __sadp_h__
#define __sadp_h__

#include "bsp.h"

//peer ip;
typedef struct {
  char ipaddr[64];
  unsigned short port;
}gsf_sadp_peer_t;

//sadp_msg;
typedef struct {
  short ver;      // 0x0001
  short modid;    // gsf.h
  gsf_user_t user;// auth;
  gsf_msg_t  msg; // msg.h;
}gsf_sadp_msg_t;

// echo 5 > /proc/sys/net/ipv4/igmp_max_memberships
// route add -net 224.0.0.0 netmask 240.0.0.0 dev eth0
// route del -net 224.0.0.0 netmask 240.0.0.0

//pu listen mc&udp;
typedef struct {
  char ethname[64];
  char lcaddr[64];
  char mcaddr[64];      // default: 238.238.238.238;
  unsigned short mcport;// default: 8888;
  int(*cb)(gsf_sadp_msg_t *in, gsf_msg_t* out
          , int *osize/*IN&OUT*/, gsf_sadp_peer_t* peer);
}gsf_sadp_ini_t;

int sadp_pu_init(gsf_sadp_ini_t *ini);

//cu send mc||udp;
int sadp_cu_gset(gsf_sadp_peer_t* dst, gsf_sadp_msg_t *in
                , gsf_msg_t* out, int *osize/*IN&OUT*/, int to);

#endif