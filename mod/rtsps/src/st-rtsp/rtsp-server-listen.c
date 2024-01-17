#include "rtsp-server-st.h"

#include <stdlib.h>

#include "cfg.h"

static void *handle_request(void *arg)
{
  struct rtsp_session_t* s = (struct rtsp_session_t*)arg;
  rtsp_session_oncreate(s);

  int tcnt = 0;
  #define READ_TIMEOUT 10
  
  while(s->loop)
  {
    int bytes = 0;
    int code = st_read(s->socket, s->buffer, sizeof(s->buffer), READ_TIMEOUT*1000000LL/*ST_UTIME_NO_TIMEOUT*/);
    
    if (code <= 0)
  	{
  	  if(errno == ETIME)
  	  {
  	    tcnt += READ_TIMEOUT;
  	    if(!s->snderr && (tcnt < s->timeout*1.1)) //check snderr;
  	    {
  	      printf("session:%p, st_read snderr:%d, tcnt:%d < timeout:%d sec.\n", s, s->snderr, tcnt, s->timeout);
  	      continue;
  	    }
  	    printf("session:%p, st_read snderr:%d, tcnt:%d > timeout:%d exit.\n", s, s->snderr, tcnt, s->timeout);
  	  }
  	  bytes = 0;
      code = -1;
  	}
  	else
  	{
  	  bytes = code;
  	  code = 0;
  	}
    
    tcnt = 0;
    rtsp_session_onrecv(s, code, bytes);
    
    if(code < 0)
    {
      break;
    }
  }
  
  rtsp_session_ondestroy(s);
  printf("st_netfd_close sess:%p, cli_nfd:%d, timeout:%d\n", s, s->socket, s->timeout);
  st_netfd_close(s->socket);
  st_mutex_destroy(s->lock);
  free(s);
  s->exited = 1;
  st_thread_exit(NULL);
  return NULL;
}

static void *handle_accept(void *arg)
{  
  int n;
  struct sockaddr_in  cli_addr;
  st_netfd_t cli_nfd, srv_nfd;
  struct st_rtsp_handler_t* handler;
  
  struct rtsp_server_listen_t *rtsp = (struct rtsp_server_listen_t*)arg;
  void *param = rtsp->param;
  srv_nfd = rtsp->srv_nfd;
  handler = rtsp->handler;
  printf("handle_accept rtsp:%p\n", rtsp);
  while(rtsp->loop) 
  {
    n = sizeof(cli_addr);
    cli_nfd = st_accept(srv_nfd, (struct sockaddr *)&cli_addr, &n,
     ST_UTIME_NO_TIMEOUT);
    if (cli_nfd == NULL) {
      printf("st_accept err.\n");
      break;
    }
    struct rtsp_session_t* sess;
  	sess = (struct rtsp_session_t*)calloc(1, sizeof(*sess));
  	  	
  	sess->socket = cli_nfd;
  	sess->addrlen = n;
  	memcpy(&sess->addr, &cli_addr, sess->addrlen);
    sess->lock = st_mutex_new();

  	memcpy(&sess->handler, handler, sizeof(sess->handler));
  	sess->param = param;
  	sess->timeout = 60; // default 60 sec;
    printf("st_accept sess:%p, cli_nfd:%d, timeout:%d\n", sess, cli_nfd, sess->timeout);
    sess->loop = 1; sess->exited = !sess->loop;
    if ((sess->tid = st_thread_create(handle_request, sess, 0, 0)) == NULL) {
      sess->loop = 0; sess->exited = !sess->loop;
      st_netfd_close(sess->socket);
      free(sess);
      printf("st_thread_create err.\n");
    }
  }
  rtsp->exited = 1;
  st_thread_exit(NULL);
  return NULL;
}

void* rtsp_server_listen(const char* ip, int port, struct st_rtsp_handler_t* handler, void* param)
{
  int opt, sock, n;
  struct sockaddr_in lcl_addr;
  st_netfd_t srv_nfd;

  memset(&lcl_addr, 0, sizeof(struct sockaddr_in));
  lcl_addr.sin_family = AF_INET;
  lcl_addr.sin_port = htons(port);
  lcl_addr.sin_addr.s_addr = ip==NULL?INADDR_ANY:inet_addr(ip);

  if ((sock = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
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
  listen(sock, 128);
  if ((srv_nfd = st_netfd_open_socket(sock)) == NULL) {
    printf("st_netfd_open err.");
    return NULL;
  }

  struct rtsp_server_listen_t *rtsp = calloc(1, sizeof(struct rtsp_server_listen_t));
  rtsp->srv_nfd = srv_nfd;
  rtsp->handler = handler;
  rtsp->param = param;
  rtsp->loop = 1; rtsp->exited = !rtsp->loop;
  if ((rtsp->tid = st_thread_create(handle_accept, rtsp, 0, 0)) == NULL) {
    st_netfd_close(rtsp->srv_nfd);
    free(rtsp);
    printf("st_thread_create err.\n");
    return NULL;
  }
  printf("%s => ok rtsp:%p\n", __func__, rtsp);
  return (void*)rtsp;
}

int rtsp_server_unlisten(void* st)
{
  struct rtsp_server_listen_t *l = (struct rtsp_server_listen_t *)st;
  
  if(l->tid && !l->exited) //check tid is valid
  {
    st_thread_interrupt(l->tid);
    l->loop = 0;
    while(!l->exited)
      st_usleep(10*1000);
  }
  // each close sess;
  st_netfd_close(l->srv_nfd);
  free(l);
    
  return 0;
}
