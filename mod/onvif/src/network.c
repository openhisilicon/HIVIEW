#include <unistd.h>
#include "network.h"


char *server_ip = "192.168.1.251";
int server_udp;
int server_hello;

int net_get_hwaddr(char *ifname, unsigned char *mac)
{
	struct ifreq ifr;
	int skfd;

	if ( (skfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) {
		//printf("socket error");
		return -1;
	}

	strncpy(ifr.ifr_name, ifname, IFNAMSIZ);
	if (ioctl(skfd, SIOCGIFHWADDR, &ifr) < 0) {
		//printf("net_get_hwaddr: ioctl SIOCGIFHWADDR");
		close(skfd);
		return -1;
	}
	close(skfd);

	memcpy(mac, ifr.ifr_ifru.ifru_hwaddr.sa_data, IFHWADDRLEN);
	return 0;
}

in_addr_t net_get_ifaddr(char *ifname)
{
	struct ifreq ifr;
	int skfd;
	struct sockaddr_in *saddr;

	if ( (skfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) {
		//printf("socket error");
		return -1;
	}

	strncpy(ifr.ifr_name, ifname, IFNAMSIZ);
	if (ioctl(skfd, SIOCGIFADDR, &ifr) < 0) {
	//	printf("net_get_ifaddr: ioctl SIOCGIFADDR");
		close(skfd);
		return -1;
	}
	close(skfd);

	saddr = (struct sockaddr_in *) &ifr.ifr_addr;
	return saddr->sin_addr.s_addr;
}

int bind_server_udp(int server_s, char *server_ip)
{
	int server_port_udp = 3702;
#if 0
#ifdef INET6
	struct sockaddr_in6 server_sockaddr;
	server_sockaddr.sin6_family = AF_INET6;
	memcpy(&server_sockaddr.sin6_addr, &in6addr_any, sizeof (in6addr_any));
	server_sockaddr.sin6_port = htons(server_port_udp);
#else
	struct sockaddr_in server_sockaddr;
	memset(&server_sockaddr, 0, sizeof server_sockaddr);
#ifdef HAVE_SIN_LEN             /* uncomment for BSDs */
	server_sockaddr.sin_len = sizeof server_sockaddr;
#endif
	server_sockaddr.sin_family = AF_INET;
	if (server_ip != NULL) {
		inet_aton(server_ip, &server_sockaddr.sin_addr);
	} else {
		server_sockaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	}
	server_sockaddr.sin_port = htons(server_port_udp);
#endif
#endif
	struct sockaddr_in server_sockaddr;
	memset(&server_sockaddr, 0, sizeof server_sockaddr);	
	server_sockaddr.sin_family = AF_INET;
	server_sockaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	server_sockaddr.sin_port = htons(server_port_udp);

	return bind(server_s, (struct sockaddr *) &server_sockaddr,
			sizeof (server_sockaddr));
}

int set_nonblock_fd(int fd)
{
	int flags;

	flags = fcntl(fd, F_GETFL);
	if (flags == -1)
		return -1;

	flags |= O_NONBLOCK;
	flags = fcntl(fd, F_SETFL, flags);
	return flags;
}

int create_server_socket_udp(void)
{
	int server_udp;
	int sock_opt = 1;
	unsigned char one = 1;

#ifdef SERVER_SSL
	if (do_ssl) {
		if (InitSSLStuff() != 1) {
			/*TO DO - emit warning the SSL stuff will not work*/
			//			syslog(LOG_ALERT, "Failure initialising SSL support - ");
			if (do_sock == 0) {
				//				syslog(LOG_ALERT, "    normal sockets disabled, so exiting");fflush(NULL);
				return 0;
			} else {
				//				syslog(LOG_ALERT, "    supporting normal (unencrypted) sockets only");fflush(NULL);
				do_sock = 2;
			}
		}
	} else
		do_sock = 2;
#endif /*SERVER_SSL*/

#ifdef SERVER_SSL
	if(do_sock){
#endif /*SERVER_SSL*/
		server_udp = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
		if (server_udp == -1) {
			//printf("unable to create socket");
		}

		/* server socket is nonblocking */
		if (set_nonblock_fd(server_udp) == -1) {
			//printf("fcntl: unable to set server socket to nonblocking");
		}

		/* close server socket on exec so cgi's can't write to it */
		if (fcntl(server_udp, F_SETFD, 1) == -1) {
			//printf("can't set close-on-exec on server socket!");
		}

		/* reuse socket addr */
		if ((setsockopt(server_udp, SOL_SOCKET, SO_REUSEADDR, (void *) &sock_opt,
						sizeof (sock_opt))) == -1) {
		//	printf("setsockopt");
		}
		if ((setsockopt(server_udp, IPPROTO_IP, IP_MULTICAST_LOOP,
						&one, sizeof (unsigned char))) == -1) {
			//printf("setsockopt");
		}
		char server_ip[16];
		NET_IPV4 _ip;

		_ip.int32 = net_get_ifaddr(ETH_NAME);
		sprintf(server_ip, "%03d.%03d.%03d.%03d",_ip.str[0], _ip.str[1], _ip.str[2], _ip.str[3]);
		/* internet family-specific code encapsulated in bind_server()  */
		if (bind_server_udp(server_udp, server_ip) == -1)
		{
			//printf("unable to bind ip:%s\n", server_ip);
		}

		return server_udp;
#ifdef SERVER_SSL
	}
	else
		return -1;
#endif /*SERVER_SSL*/
}



