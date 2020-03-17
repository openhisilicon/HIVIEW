#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <time.h>
#include <string.h>

#ifndef WIN32
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#else
#include <winsock2.h>
#include <winsock.h>
#include <ws2tcpip.h> //mc
#define __inline__ __inline
#define socklen_t  int
#define EADDRINUSE WSAEADDRINUSE
#define close closesocket
#undef  errno
#define errno WSAGetLastError()
#endif

#include "rtp-socket.h"
#include "time64.h"



// must be bound/connected
int socket_getname(int sock, char ip[SOCKET_ADDRLEN], u_short* port)
{
  int socket_error = 0;
	struct sockaddr_storage addr;
	socklen_t addrlen = sizeof(addr);
	if(socket_error == getsockname(sock, (struct sockaddr*)&addr, &addrlen))
		return socket_error;

	return socket_addr_to((struct sockaddr*)&addr, addrlen, ip, port);
}

int socket_getpeername(int sock, char ip[SOCKET_ADDRLEN], u_short* port)
{
  int socket_error = 0;
	struct sockaddr_storage addr;
	socklen_t addrlen = sizeof(addr);
	if(socket_error == getpeername(sock, (struct sockaddr*)&addr, &addrlen))
		return socket_error;
	return socket_addr_to((struct sockaddr*)&addr, addrlen, ip, port);
}

int socket_isip(const char* ip)
{
	struct sockaddr_storage addr;
	if(1 != inet_pton(AF_INET, ip, &((struct sockaddr_in*)&addr)->sin_addr) 
		&& 1 != inet_pton(AF_INET6, ip, &((struct sockaddr_in6*)&addr)->sin6_addr))
		return -1;
	return 0;
}

int socket_ipv4(const char* ipv4_or_dns, char ip[SOCKET_ADDRLEN])
{
	int r;
	struct addrinfo hints, *addr;
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
  // hints.ai_flags = AI_ADDRCONFIG;
	r = getaddrinfo(ipv4_or_dns, NULL, &hints, &addr);
	if (0 != r)
		return r;

	assert(AF_INET == addr->ai_family);
	inet_ntop(AF_INET, &(((struct sockaddr_in*)addr->ai_addr)->sin_addr), ip, SOCKET_ADDRLEN);
	freeaddrinfo(addr);
	return 0;
}

int socket_ipv6(const char* ipv6_or_dns, char ip[SOCKET_ADDRLEN])
{
	int r;
	struct addrinfo hints, *addr;
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET6;
	hints.ai_flags = /*AI_ADDRCONFIG |*/ AI_V4MAPPED; // map ipv4 address to ipv6
	r = getaddrinfo(ipv6_or_dns, NULL, &hints, &addr);
	if (0 != r)
		return r;

	assert(AF_INET6 == addr->ai_family);
	inet_ntop(AF_INET6, &(((struct sockaddr_in6*)addr->ai_addr)->sin6_addr), ip, SOCKET_ADDRLEN);
	freeaddrinfo(addr);
	return 0;
}

int socket_addr_from_ipv4(struct sockaddr_in* addr4, const char* ipv4_or_dns, u_short port)
{
	int r;
	char portstr[16];
	struct addrinfo hints, *addr;
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
//	hints.ai_flags = AI_ADDRCONFIG;
	snprintf(portstr, sizeof(portstr), "%hu", port);
	r = getaddrinfo(ipv4_or_dns, portstr, &hints, &addr);
	if (0 != r)
		return r;

	// fixed ios getaddrinfo don't set port if node is ipv4 address
	socket_addr_setport(addr->ai_addr, addr->ai_addrlen, port);
	assert(sizeof(struct sockaddr_in) == addr->ai_addrlen);
	memcpy(addr4, addr->ai_addr, addr->ai_addrlen);
	freeaddrinfo(addr);
	return 0;
}

int socket_addr_from_ipv6(struct sockaddr_in6* addr6, const char* ipv6_or_dns, u_short port)
{
	int r;
	char portstr[16];
	struct addrinfo hints, *addr;
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET6;
	hints.ai_flags = AI_V4MAPPED /*| AI_ADDRCONFIG*/; // AI_ADDRCONFIG linux "ff00::" return -2
	snprintf(portstr, sizeof(portstr), "%hu", port);
	r = getaddrinfo(ipv6_or_dns, portstr, &hints, &addr);
	if (0 != r)
		return r;

	// fixed ios getaddrinfo don't set port if node is ipv4 address
	socket_addr_setport(addr->ai_addr, addr->ai_addrlen, port);
	assert(sizeof(struct sockaddr_in6) == addr->ai_addrlen);
	memcpy(addr6, addr->ai_addr, addr->ai_addrlen);
	freeaddrinfo(addr);
	return 0;
}

int socket_addr_from(struct sockaddr_storage* ss, socklen_t* len, const char* ipv4_or_ipv6_or_dns, u_short port)
{
	int r;
	char portstr[16];
	struct addrinfo *addr;
	snprintf(portstr, sizeof(portstr), "%hu", port);
	r = getaddrinfo(ipv4_or_ipv6_or_dns, portstr, NULL, &addr);
	if (0 != r)
		return r;

	// fixed ios getaddrinfo don't set port if node is ipv4 address
	socket_addr_setport(addr->ai_addr, addr->ai_addrlen, port);
	assert(addr->ai_addrlen <= sizeof(struct sockaddr_storage));
	memcpy(ss, addr->ai_addr, addr->ai_addrlen);
	*len = addr->ai_addrlen;
	freeaddrinfo(addr);
	return 0;
}

int socket_addr_to(const struct sockaddr* sa, socklen_t salen, char ip[SOCKET_ADDRLEN], u_short* port)
{
	if (AF_INET == sa->sa_family)
	{
		struct sockaddr_in* in = (struct sockaddr_in*)sa;
		assert(sizeof(struct sockaddr_in) == salen);
		inet_ntop(AF_INET, &in->sin_addr, ip, SOCKET_ADDRLEN);
		*port = ntohs(in->sin_port);
	}
	else if (AF_INET6 == sa->sa_family)
	{
		struct sockaddr_in6* in6 = (struct sockaddr_in6*)sa;
		assert(sizeof(struct sockaddr_in6) == salen);
		inet_ntop(AF_INET6, &in6->sin6_addr, ip, SOCKET_ADDRLEN);
		*port = ntohs(in6->sin6_port);
	}
	else
	{
		return -1; // unknown address family
	}

	return 0;
}

int socket_addr_setport(struct sockaddr* sa, socklen_t salen, u_short port)
{
	if (AF_INET == sa->sa_family)
	{
		struct sockaddr_in* in = (struct sockaddr_in*)sa;
		assert(sizeof(struct sockaddr_in) == salen);
		in->sin_port = htons(port);
	}
	else if (AF_INET6 == sa->sa_family)
	{
		struct sockaddr_in6* in6 = (struct sockaddr_in6*)sa;
		assert(sizeof(struct sockaddr_in6) == salen);
		in6->sin6_port = htons(port);
	}
	else
	{
		assert(0);
		return -1;
	}

	return 0;
}

int socket_addr_name(const struct sockaddr* sa, socklen_t salen, char* host, size_t hostlen)
{
	return getnameinfo(sa, salen, host, hostlen, NULL, 0, 0);
}

int socket_addr_is_multicast(const struct sockaddr* sa, socklen_t salen)
{
	if (AF_INET == sa->sa_family)
	{
		const struct sockaddr_in* in = (const struct sockaddr_in*)sa;
		assert(sizeof(struct sockaddr_in) == salen);
		return (ntohl(in->sin_addr.s_addr) & 0xf0000000) == 0xe0000000 ? 1 : 0;
	}
	else if (AF_INET6 == sa->sa_family)
	{
		const struct sockaddr_in6* in6 = (const struct sockaddr_in6*)sa;
		assert(sizeof(struct sockaddr_in6) == salen);
		return in6->sin6_addr.s6_addr[0] == 0xff ? 1 : 0;
	}
	else
	{
		assert(0);
	}

	return 0;
}

/// RECOMMAND: compare with struct sockaddr_storage
/// @return 0-equal, other-don't equal
int socket_addr_compare(const struct sockaddr* sa, const struct sockaddr* sb)
{
	if(sa->sa_family != sb->sa_family)
		return sa->sa_family - sb->sa_family;

	switch (sa->sa_family)
	{
	case AF_INET:	return memcmp(sa, sb, sizeof(struct sockaddr_in));
	case AF_INET6:	return memcmp(sa, sb, sizeof(struct sockaddr_in6));
  #if 1 // Windows build 17061 https://blogs.msdn.microsoft.com/commandline/2017/12/19/af_unix-comes-to-windows/
	case AF_UNIX:	return memcmp(sa, sb, sizeof(struct sockaddr_un));
  #endif
	default:		return -1;
	}
}


int socket_c(int l4)
{
	int sock, err;

	sock = socket(AF_INET, l4, 0);
	if (sock < 0)
	{
		err = -errno;
		printf("Create socket failed.\n");
		return err;
	}
  #ifdef WIN32
  if (l4 != SOCK_STREAM)
  {
    int nSize = 2*1024 * 1024;
    setsockopt(sock, SOL_SOCKET, SO_RCVBUF, (char*)&nSize, sizeof(nSize));
  }
  #else
  {
    int nSize = 800 * 1024;
    setsockopt(sock, SOL_SOCKET, SO_RCVBUF, (char*)&nSize, sizeof(nSize));
  }
  #endif
  {
    int nSize = 0;
    socklen_t len = sizeof(nSize);
    getsockopt(sock, SOL_SOCKET, SO_RCVBUF, (char*)&nSize, &len);
    //printf("sock:%d, SO_RCVBUF = '%d'.\n", sock, nSize);
  }
  #ifdef WIN32
	{
		u_long flags = 1;
		ioctlsocket(sock, FIONBIO, (u_long*)&flags);
	}
  #else 
	/* other flags was reset */
	fcntl(sock, F_SETFL, O_NONBLOCK);
  #endif
	return sock;
}
int socket_b(int sock, const char *host, int port, int reuse)
{
	struct sockaddr_in sin;
	int err;

  if(reuse)
  {
    if(setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (char *)&reuse, sizeof(reuse)) < 0)
      printf("socket Setting SO_REUSEADDR failed socket '%d'.\n", sock);
  }

	memset(&sin, 0, sizeof(sin));
	sin.sin_family = AF_INET;
	sin.sin_port = htons(port);
	sin.sin_addr.s_addr = host ? inet_addr(host) : INADDR_ANY;

	if (bind(sock, (struct sockaddr*)&sin, sizeof(sin)))
	{
		err = -errno;
		printf( "\033[1;33m" "Bind socket failed on port '%d'.\n"  "\033[0000m", port);
		close(sock);
		return err;
	}
	
	return 0;
}
int socket_mcast_addr(int sock, const char *host)
{
	int err = -1;
	int loop = 1;
	err = setsockopt(sock, IPPROTO_IP, IP_MULTICAST_LOOP, (const char*)&loop, sizeof(loop));
	if(err < 0)
	{
		printf("setsockopt():IP_MULTICAST_LOOP\n");
		return -errno;
	}
	
	struct ip_mreq mreq;
	mreq.imr_multiaddr.s_addr = inet_addr(host);	/*广播地址*/
	mreq.imr_interface.s_addr = htonl(INADDR_ANY);  /*网络接口为默认*/
	/*将本机加入广播组*/
	err = setsockopt(sock, IPPROTO_IP, IP_ADD_MEMBERSHIP, (const char*)&mreq, sizeof(mreq));
	if (err < 0)
	{
		printf("setsockopt():IP_ADD_MEMBERSHIP\n");
		return -errno;
	}

	return 0;
}
int socket_bind(const char *host, int port, int l4, int reuse)
{
	struct sockaddr_in sin;
	int sock, err;

	sock = socket_c(l4);
	if (sock < 0)
	{
		return sock;
	}
	err = socket_b(sock, host, port, reuse);
	if(err < 0)
	{
		return err;
	}
	return sock;
}

static unsigned short s_base_port = 30000;
static unsigned short s_port_num = 30000;

static char s_multicast_ip[SOCKET_ADDRLEN];
static unsigned int s_multicast_num = 250;

int rtp_socket_create(const char* ip, st_netfd_t rtp[2], unsigned short port[2])
{
	unsigned short i;
	int sock[2];
	assert(0 == s_base_port % 2);
	
	//srand((unsigned int)time(NULL));
	static int randseq;
  struct timeval tv;	
	gettimeofday(&tv, NULL);
  srand((unsigned int)(tv.tv_sec*1000000+tv.tv_usec + getpid() + randseq++));
  
	do
	{
		i = rand() % 30000;
		i = i/2*2 + s_base_port;

		sock[0] = socket_bind(ip, i, SOCK_DGRAM, 0);
		if(sock[0] < 0)
			continue;

		sock[1] = socket_bind(ip, i + 1, SOCK_DGRAM, 0);
		if(sock[1] < 0)
		{
			close(sock[0]);
			continue;
		}
    printf("%s => sock[%d,%d], port[%d,%d]\n", __func__, sock[0], sock[1], i, i+1);
		rtp[0] = st_netfd_open_socket(sock[0]);
		rtp[1] = st_netfd_open_socket(sock[1]);
		port[0] = i;
		port[1] = i+1;
		return 0;

	} while(sock[0] < 0 || sock[1] < 0);

	return -1;
}

void rtp_socket_set_port_range(unsigned short base, unsigned short num)
{
	s_base_port = base;
	s_port_num = num;
}

void rtp_socket_get_port_range(unsigned short *base, unsigned short *num)
{
	*base = s_base_port;
	*num = s_port_num;
}

void rtp_socket_set_multicast_range(const char* multicast, unsigned int num)
{
	memset(s_multicast_ip, 0, sizeof(s_multicast_ip));
	snprintf(s_multicast_ip, sizeof(s_multicast_ip), "%s", multicast);
	s_multicast_num = num;
}

void rtp_socket_get_multicast_range(char multicast[SOCKET_ADDRLEN], unsigned int *num)
{
	snprintf(multicast, SOCKET_ADDRLEN, "%s", s_multicast_ip);
	*num = s_multicast_num;
}
