#ifndef __sadp_h__
#define __sadp_h__

#include "bsp.h"

enum {
  GSF_SADP_VER_REQ = (1<<0), // requset-flag;
  GSF_SADP_VER_MC  = (1<<1), // require-multicast-response;
};

//peer ip;
typedef struct {
  char ipaddr[64];
  unsigned short port;
}gsf_sadp_peer_t;

//sadp_msg;
typedef struct {
  short ver;      // ver GSF_SADP_VER_;
  short modid;    // gsf.h
  int   devid;    // devid = dst when REQ = 1; devid = src when REQ = 0; devid = 0[all devices];
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