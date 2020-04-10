#ifndef __NETWORK_H_
#define __NETWORK_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/ioctl.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <net/if.h>
#include <arpa/inet.h>

//#include "onvif.h"
#include <signal.h>
#include <sys/sem.h>

#ifndef ETH_NAME
#define ETH_NAME "eth0"
#endif

typedef union __NET_IPV4 {
	unsigned int  int32;
	unsigned char str[4];
} NET_IPV4;

int net_get_hwaddr(char *ifname, unsigned char *mac);
int bind_server_udp(int server_s, char *server_ip);
int set_nonblock_fd(int fd);
int create_server_socket_udp(void);
in_addr_t net_get_ifaddr(char *ifname);

#endif
