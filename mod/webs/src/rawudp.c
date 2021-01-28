#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <assert.h>

#include "inc/gsf.h"
#include "mod_call.h"

unsigned int cfifo_recsize(unsigned char *p1, unsigned int n1, unsigned char *p2)
{
    unsigned int size = sizeof(gsf_frm_t);

    if(n1 >= size)
    {
        gsf_frm_t *rec = (gsf_frm_t*)p1;
        return  sizeof(gsf_frm_t) + rec->size;
    }
    else
    {
        gsf_frm_t rec;
        char *p = (char*)(&rec);
        memcpy(p, p1, n1);
        memcpy(p+n1, p2, size-n1);
        return  sizeof(gsf_frm_t) + rec.size;
    }
    
    return 0;
}

unsigned int cfifo_rectag(unsigned char *p1, unsigned int n1, unsigned char *p2)
{
    unsigned int size = sizeof(gsf_frm_t);

    if(n1 >= size)
    {
        gsf_frm_t *rec = (gsf_frm_t*)p1;
        return (rec->flag & GSF_FRM_FLAG_IDR)?rec->utc:0;
    }
    else
    {
        gsf_frm_t rec;
        char *p = (char*)(&rec);
        memcpy(p, p1, n1);
        memcpy(p+n1, p2, size-n1);
        return (rec.flag & GSF_FRM_FLAG_IDR)?rec.utc:0;
    }
    
    return 0;
}

unsigned int cfifo_recgut(unsigned char *p1, unsigned int n1, unsigned char *p2, void *u)
{
    unsigned int len = cfifo_recsize(p1, n1, p2);
    unsigned int l = CFIFO_MIN(len, n1);
    
    //printf("len:%d, l1:%d\n", len, l);
    
    char *p = (char*)u;
    memcpy(p, p1, l);
    memcpy(p+l, p2, len-l);

    gsf_frm_t *rec = (gsf_frm_t *)u;
  	struct timespec _ts;  
    clock_gettime(CLOCK_MONOTONIC, &_ts);
    int cost = (_ts.tv_sec*1000 + _ts.tv_nsec/1000000) - rec->utc;
    if(cost > 33)
      printf("get rec->type:%d ok [delay:%d ms].\n", rec->type, cost);

    return len;
}

static pthread_t gpid;
static char destip[64];
static unsigned short destport;

void* udp_send_task(void *parm)
{
  int i = 0, ret = 0;
  
  struct cfifo_ex* video_fifo = NULL;
  struct cfifo_ex* audio_fifo = NULL;
  int ep = cfifo_ep_alloc(1);
  unsigned char buf[sizeof(gsf_frm_t)+500*1024];
  
  int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
  int send_size = 1300;
  struct sockaddr_in dest_addr;
	memset(&dest_addr, 0, sizeof(dest_addr));
	dest_addr.sin_family = AF_INET;
	dest_addr.sin_port   = htons(destport);
	dest_addr.sin_addr.s_addr = inet_addr(destip);
  
 
  GSF_MSG_DEF(gsf_sdp_t, sdp, sizeof(gsf_msg_t)+sizeof(gsf_sdp_t));
  sdp->video_shmid = sdp->audio_shmid = -1;
  ret = GSF_MSG_SENDTO(GSF_ID_CODEC_SDP, 0, GET, 0
                        , sizeof(gsf_sdp_t)
                        , GSF_IPC_CODEC
                        , 2000);
  if(ret == 0)
  {
    video_fifo = cfifo_shmat(cfifo_recsize, cfifo_rectag, sdp->video_shmid);
    audio_fifo = cfifo_shmat(cfifo_recsize, cfifo_rectag, sdp->audio_shmid);
    
	cfifo_ep_ctl(ep, CFIFO_EP_ADD, video_fifo);
	unsigned int video_utc = cfifo_newest(video_fifo, 1);
	 
	if(audio_fifo)
	{
	  cfifo_ep_ctl(ep, CFIFO_EP_ADD, audio_fifo);
	  unsigned int audio_utc = cfifo_oldest(audio_fifo, video_utc);
	  printf("video_utc:%u, audio_utc:%u\n", video_utc, audio_utc);
	}
    
  }
  while(1)
  {
    int cnt = 0;
    struct cfifo_ex* result[255];
    cnt = cfifo_ep_wait(ep, 2000, result, 255);
    if(cnt <= 0)
    {
       printf("cfifo_ep_wait err cnt:%d\n", cnt);
    }
    
    for(i = 0; i < cnt; i++)
    {
      struct cfifo_ex* fifo = result[i];
      
      while(ret >= 0)
      {
          ret = cfifo_get(fifo, cfifo_recgut, (unsigned char*)buf);
          if(ret < 0)
          {
              printf("cfifo err ret:%d\n", ret);
          }
          else if (ret == 0)
          {
              //printf("cfifo empty \n");
              break;
          }
          else
          {
              //cfifo_info(fifo);
          }
          // TODO;
          gsf_frm_t *rec = (gsf_frm_t *)buf;
          printf("rec->type:%d, seq:%d, utc:%u, size:%d\n"
                , rec->type, rec->seq, rec->utc, rec->size);

          if(0)
          {
        		char *pbuf = rec->data;
        		int  left = rec->size;
        		int  cnt = 0;
        		while(1)
        		{
        			if (left <= send_size)
        			{
        				ret = sendto(sockfd, pbuf, left, 0, (struct sockaddr *)&dest_addr, sizeof(dest_addr));
        				assert(ret == left);
        				break;
        			}

        			ret = sendto(sockfd, pbuf, send_size, 0, (struct sockaddr *)&dest_addr, sizeof(dest_addr));
        			assert(ret == send_size);
        			pbuf += send_size;
        			left -= send_size;
        			cnt  += send_size;
        			
        			if(cnt > 10*1300)
        			{
        			  cnt = 0;
        			  usleep(1);
        			}
        		}
      		}
      }
    }
  }
  return NULL;
}


int rawudp_open(char *ip, unsigned short port)
{
  strcpy(destip, ip);
  destport = port;
  
  return pthread_create(&gpid, NULL, udp_send_task, (void*)NULL);
}