#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <net/if.h>
#include <stdio.h>
#include <stdlib.h>

#include "sadp.h"
#include "cfg.h"

typedef struct {
  int fd, runing;
  pthread_t pid;
  gsf_sadp_ini_t ini;
}gsf_sadp_pu_t;

static gsf_sadp_pu_t __pu;

void* pu_recv_task(void *parm)
{
  int ret = 0, osize = 0;
  char buf[16*1024];
  char out[16*1024];
  struct sockaddr_in cli;
  gsf_sadp_pu_t *pu = (gsf_sadp_pu_t*)parm;
  
  while(pu->runing)
  {
    int n = sizeof(cli);
    if((ret = recvfrom(pu->fd, buf, sizeof(buf), 0, (struct sockaddr *)&cli, (socklen_t*)&n)) <= 0)
    {
      continue;
    }
   
    if(pu->ini.cb)
    {
      osize = sizeof(out);
      gsf_sadp_peer_t peer;
      strcpy(peer.ipaddr, inet_ntoa(cli.sin_addr));
      peer.port = ntohs(cli.sin_port);

      pu->ini.cb((gsf_sadp_msg_t*)buf, (gsf_msg_t*)out, &osize, &peer);
      if(osize > 0)
        sendto(pu->fd, out, osize, 0, (struct sockaddr*)&cli, sizeof(cli));
    }
  }
  
  printf("%s => exit.\n", __func__);
  return NULL;
}

int sadp_pu_init(gsf_sadp_ini_t *ini)
{
  gsf_sadp_pu_t *pu = &__pu;
  
  char cmdstr[64];
  //set keepalive parm;
  system("echo 60 > /proc/sys/net/ipv4/tcp_keepalive_time");
	system("echo 10 > /proc/sys/net/ipv4/tcp_keepalive_intvl");
	system("echo 5 > /proc/sys/net/ipv4/tcp_keepalive_probes");

	//set SO_SNDBUF 800k;
	system("echo  819200 > /proc/sys/net/core/wmem_max");
	system("echo  819200 > /proc/sys/net/core/wmem_default");
	system("echo  819200 > /proc/sys/net/core/rmem_max");
	system("echo  819200 > /proc/sys/net/core/rmem_default");
	//set mc;
  system("echo 5 > /proc/sys/net/ipv4/igmp_max_memberships");
  sprintf(cmdstr, "route add -net 224.0.0.0 netmask 240.0.0.0 dev %s", ini->ethname);
  system(cmdstr);
  
  
  if(pu->runing)
  {
    pu->runing = 0;
    close(pu->fd);
    pthread_join(pu->pid, NULL);
  }
  
  pu->ini = *ini;
  int fd = socket(AF_INET, SOCK_DGRAM, 0);
  if(fd < 0)
  {
    return -1;
  }
  
  int reuse = 1;
  if(setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (char *)&reuse, sizeof(reuse)) < 0)
  {
    close(fd);
    return -2;
  } 

  struct sockaddr_in localSock;
  memset((char *) &localSock, 0, sizeof(localSock));
  localSock.sin_family = AF_INET;
  localSock.sin_port = htons(pu->ini.mcport);
  localSock.sin_addr.s_addr = INADDR_ANY;
  if(bind(fd, (struct sockaddr*)&localSock, sizeof(localSock)))
  {
    close(fd);
    return -3;
  }
  
  #if 1
  struct ip_mreq group;
  memset(&group, 0, sizeof(group));
  group.imr_multiaddr.s_addr = inet_addr(pu->ini.mcaddr);
  group.imr_interface.s_addr = inet_addr("0.0.0.0");
  #else
  struct ip_mreqn group;
  memset(&group, 0, sizeof(group));
	group.imr_address.s_addr = htonl(INADDR_ANY);
	group.imr_ifindex = if_nametoindex(ini->ethname);
  group.imr_multiaddr.s_addr = inet_addr(pu->ini.mcaddr);
  #endif
  
  if(setsockopt(fd, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char *)&group, sizeof(group)) < 0)
  {
    perror("setsockopt IP_ADD_MEMBERSHIP");
    close(fd);
    return -4;
  }
  
  pu->fd = fd;
  pu->runing = 1;
  if((pthread_create(&pu->pid, NULL, pu_recv_task, (void*)pu)) != 0)
  {
    close(fd);
    return -5;
  }
  return 0;
}

///////////////////////////////////////////////


//cu;
int sadp_cu_gset(gsf_sadp_peer_t* dst, gsf_sadp_msg_t *in
                , gsf_msg_t* out, int *osize/*IN&OUT*/, int to)
{
  int fd = socket(AF_INET, SOCK_DGRAM, 0);
  if(fd < 0)
  {
    return -1;
  }
  
  struct timeval tv;
  tv.tv_sec  = to;
  tv.tv_usec = 0;
  setsockopt(fd,SOL_SOCKET,SO_RCVTIMEO,&tv, sizeof(tv));
   
  struct sockaddr_in groupSock;
  memset((char *) &groupSock, 0, sizeof(groupSock));
  groupSock.sin_family = AF_INET;
  groupSock.sin_addr.s_addr = inet_addr(dst->ipaddr);
  groupSock.sin_port = htons(dst->port);

  struct in_addr localInterface;
  localInterface.s_addr = inet_addr("0.0.0.0");
  if(setsockopt(fd, IPPROTO_IP, IP_MULTICAST_IF, (char *)&localInterface, sizeof(localInterface)) < 0)
  {
    close(fd);
    return -2;
  }

  if(sendto(fd, in, sizeof(gsf_sadp_msg_t)+in->msg.size, 0, (struct sockaddr*)&groupSock, sizeof(groupSock)) < 0)
  {
    close(fd);
    return -3;
  }

  if(!to)
  {
    close(fd);
    return 0;
  }

  int ret = 0, mc1 = 0;
  struct sockaddr_in cli;
  int left = *osize; *osize = 0;
  char *pout = (char*)out; 
  sscanf(dst->ipaddr, "%d.%*s", &mc1);
  mc1 = (mc1 >= 224 && mc1 <= 239)?1:0;
  struct timespec ts1, ts2;
  clock_gettime(CLOCK_MONOTONIC, &ts1);
  while(1)
  {
    int n = sizeof(cli);
    if((ret = recvfrom(fd, pout, left, 0, (struct sockaddr *)&cli, (socklen_t*)&n)) <= 0)
    {
      break;
    }
    
    *osize += ret;
    pout += ret;
    
    if(!mc1)
      break;

    clock_gettime(CLOCK_MONOTONIC, &ts2);
    if((ts2.tv_sec*1000 + ts2.tv_nsec/1000000) - (ts1.tv_sec*1000 + ts1.tv_nsec/1000000) > to*1000)
    {
      break;
    }
  }
  close(fd);
  return 0;
}
