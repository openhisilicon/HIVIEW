#include "rtsp-server-st.h"
#include "rtsp-client-st.h"

#include "rtsp-st.h"

static pthread_t rtsp_st_pid;
static void* st_rtsp_ctl_listen(char *ip, unsigned short port);

#include "fw/comm/inc/uthash.h"
struct rtsp_conn_ctx_t
{
  UT_hash_handle hh;
  char name[256];
  void* c;
  int video_shmid;
  int audio_shmid;
};
// muti-rtsps, conn_ctxs in shm memery;
static struct rtsp_conn_ctx_t *conn_ctxs;

static int onframe(void* param, const char*encoding, const void *packet, int bytes, uint32_t time, int flags)
{
  const uint8_t start_code[] = { 0, 0, 0, 1 };
  struct rtsp_conn_ctx_t *ctx = (struct rtsp_conn_ctx_t *)param; 

  if(flags)
  {
    //printf("%s => encoding:%s, time:%08u, flags:%08d, drop.\n", ctx->name, encoding, time, flags);
  }


	if (0 == strcmp("H264", encoding))
	{
		uint8_t type = *(uint8_t*)packet & 0x1f;
		if (type == 7) // !vcl;
		{
			//printf("%s => encoding:%s, time:%08u, flags:%08d, bytes:%d\n", ctx->name, encoding, time, flags, bytes);
		}
	}
	else if (0 == strcmp("H265", encoding))
	{
		uint8_t type = (*(uint8_t*)packet >> 1) & 0x3f;
		if (type > 32) // !vcl;
		{
			//printf("%s => encoding:%s, time:%08u, flags:%08d, bytes:%d\n", ctx->name, encoding, time, flags, bytes);
		}
	}
  
  return 0;
}


static void* rtsp_st_thread(void *param)
{
  int *run_flag = (int*)param;
  
  st_set_eventsys(ST_EVENTSYS_ALT);
  /* Initialize the ST library */
  if (st_init() < 0) {
    printf("st_init err.\n");
    *run_flag = -1;  
    return NULL;
  }
  
  void *l = st_rtsp_server_listen(NULL, 554);
  void *c = st_rtsp_ctl_listen("127.0.0.1", 127);

  *run_flag = 1;
  while(1)
  {
    st_sleep(1000);
  }

  return NULL; 
}


int rtsp_st_init()
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
  
  int ret = pthread_create(&rtsp_st_pid, NULL, rtsp_st_thread, &run_flag);
  if(ret < 0)
    return ret;
  
  while(!run_flag)
    usleep(10*1000);
    
  return ret;
}

int rtsp_st_ctl(struct rtsp_st_ctl_t *ctl)
{
  int ret = 0;
  struct sockaddr_in to_addr;
  memset(&to_addr, 0, sizeof(struct sockaddr_in));
  to_addr.sin_family = AF_INET;
  to_addr.sin_port = htons(127);
  to_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
  
  int sock = socket(PF_INET, SOCK_DGRAM, 0);
  
  struct timeval tv;
	tv.tv_sec  = 3;
	tv.tv_usec = 0;
	setsockopt(sock, SOL_SOCKET, SO_SNDTIMEO, &tv, sizeof(tv));
  setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
  
  ret = sendto(sock, ctl, sizeof(*ctl)+ctl->size, 0, (struct sockaddr*)&to_addr, sizeof(to_addr));
  printf("sendto ret:%d\n", ret);
  
  if(ret > 0)
  {
    struct sockaddr_in from_addr;
    int from_len = sizeof(struct sockaddr_in);
      
    char buffer[8*1024] = {0};
    ret = recvfrom(sock, buffer, sizeof(buffer), 0, (struct sockaddr*)&from_addr, &from_len);
    printf("recvfrom ret:%d\n", ret);
    close(sock);
    return 0;
  }
  close(sock);
  return -1;
}



static void *handle_ctl(void *arg)
{
  char buffer[8*1024];
  st_netfd_t srv_nfd = (st_netfd_t)arg;
  
  while(1)
  {
    struct sockaddr_in from_addr;
    int from_len = sizeof(struct sockaddr_in);
    memset(buffer, 0, sizeof(buffer));
    int ret = st_recvfrom(srv_nfd, buffer, sizeof(buffer), (struct sockaddr*)&from_addr, &from_len, ST_UTIME_NO_TIMEOUT);
    if(ret <= 0)
    {
      printf("%s => err ret:%d\n", __func__, ret);
      break;
    }
    
    struct rtsp_st_ctl_t *ctl = (struct rtsp_st_ctl_t*)buffer;
    switch(ctl->id)
    {
      case RTSP_CTL_S_START: {
          printf("%s => id: RTSP_CTL_S_START \n", __func__);
          break;
        }
      case RTSP_CTL_S_STOP: {
          printf("%s => id: RTSP_CTL_S_STOP \n", __func__);
          break;
        }
      case RTSP_CTL_C_OPEN: {
          char *name = ctl->data;
          printf("%s => id: RTSP_CTL_C_OPEN [%s]\n", __func__, name);
          struct rtsp_conn_ctx_t *ctx = NULL;
          if(strlen(name) == 0 || strlen(name) > 255)
          {
            printf("%s => id: RTSP_CTL_C_OPEN err name [%s]\n", __func__, name);
            break;
          }
          HASH_FIND_STR(conn_ctxs, name, ctx);
          if (ctx)
          {
            printf("%s => id: RTSP_CTL_C_OPEN err already [%s]\n", __func__, name);
            break;
          }
          
          ctx = calloc(1, sizeof(*ctx));
          ctx->video_shmid = -1;
          ctx->audio_shmid = -1;
          
          struct st_rtsp_client_handler_t handler;
          handler.onframe = onframe;
          handler.param   = (void*)ctx;
          strncpy(ctx->name, name, sizeof(ctx->name)-1);          
          ctx->c = rtsp_client_connect(name, 0, &handler);
          HASH_ADD_STR(conn_ctxs, name, ctx);
          
          break;
        }
      case RTSP_CTL_C_CLOSE: {
          char *name = ctl->data;
          struct rtsp_conn_ctx_t *ctx = NULL;
          if(strlen(name) == 0 || strlen(name) > 255)
          {
            break;
          }
          HASH_FIND_STR(conn_ctxs, name, ctx);
          if (ctx)
          {
            HASH_DEL(conn_ctxs, ctx);
            
            if(ctx->c)
            {
              printf("%s => id: RTSP_CTL_C_CLOSE [%s]\n", __func__, name);
              rtsp_client_close(ctx->c);
            }
            if(ctx->video_shmid != -1)
            {
              ;
            }
            if(ctx->audio_shmid != -1)
            {
              ;
            }
            free(ctx);
          }
          break;
        }
      case RTSP_CTL_C_LIST: {
          printf("%s => id: RTSP_CTL_C_LIST \n", __func__);
          struct rtsp_conn_ctx_t *ctx, *tmp;
          HASH_ITER(hh, conn_ctxs, ctx, tmp)
          {
            //HASH_DEL(conn_ctxs, ctx);
            //free(ctx);
            printf("ctx->name:[%s]\n", ctx->name);
          }
          break;
        }
    }
    st_sendto(srv_nfd, ctl, sizeof(*ctl)+ctl->size, (struct sockaddr*)&from_addr, from_len, ST_UTIME_NO_TIMEOUT);
  }
  st_netfd_close(srv_nfd);
}



static void* st_rtsp_ctl_listen(char *ip, unsigned short port)
{
  int sock, n;
  struct sockaddr_in lcl_addr;
  st_netfd_t srv_nfd;
  memset(&lcl_addr, 0, sizeof(struct sockaddr_in));
  lcl_addr.sin_family = AF_INET;
  lcl_addr.sin_port = htons(port);
  lcl_addr.sin_addr.s_addr = inet_addr(ip);

  if ((sock = socket(PF_INET, SOCK_DGRAM, 0)) < 0) {
    printf("socket err.\n");
    return NULL;
  }
  n = 1;
  if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (char *)&n, sizeof(n)) < 0) {
    printf("setsockopt SO_REUSEADDR err.\n");
    return NULL;
  }

  if (setsockopt(sock, SOL_SOCKET, SO_REUSEPORT, (char *)&n, sizeof(n))<0) {
    printf("setsockopt SO_REUSEPORT err.\n");
    return NULL;
  }
  
  if (bind(sock, (struct sockaddr *)&lcl_addr, sizeof(lcl_addr)) < 0) {
    printf("bind err.");
    return NULL;
  }
  if ((srv_nfd = st_netfd_open_socket(sock)) == NULL) {
    printf("st_netfd_open err.");
    return NULL;
  }
  st_thread_t tid;
  if ((tid = st_thread_create(handle_ctl, srv_nfd, 0, 0)) == NULL) {
    st_netfd_close(srv_nfd);
    printf("st_thread_create err.\n");
    return NULL;
  }
  printf("%s => ok tid:%p\n", __func__, tid);
  
  return (void*)tid;
}