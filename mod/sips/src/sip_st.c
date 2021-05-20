#include "cfg.h"
#include "uas-app.h"
#include "rtp-socket.h"


static pthread_t sip_st_pid;
static void* sip_st_thread(void *param)
{
  int *run_flag = (int*)param;
  
  st_set_eventsys(ST_EVENTSYS_ALT);
  
  /* Initialize the ST library */
  if (st_init() < 0) {
    printf("st_init err.\n");
    *run_flag = -1;  
    return NULL;
  }
  *run_flag = 1;
  
  
  //TODO;
  sip_uas_app();
  
  
  return NULL; 
}


int sip_st_init()
{
  //lo;
  system("ifconfig lo 127.0.0.1");
  
  //set keepalive parm
	system("echo 60 > /proc/sys/net/ipv4/tcp_keepalive_time");
	system("echo 10 > /proc/sys/net/ipv4/tcp_keepalive_intvl");
	system("echo 5 > /proc/sys/net/ipv4/tcp_keepalive_probes");

	//set SO_SNDBUF 800k;
	system("echo  819200 > /proc/sys/net/core/wmem_max");
	system("echo  819200 > /proc/sys/net/core/wmem_default");
	system("echo  819200 > /proc/sys/net/core/rmem_max");
	system("echo  819200 > /proc/sys/net/core/rmem_default");
  
  int run_flag = 0;
  
  int ret = pthread_create(&sip_st_pid, NULL, sip_st_thread, &run_flag);
  if(ret < 0)
    return ret;
  
  while(!run_flag)
    usleep(10*1000);
    
  return ret;
}