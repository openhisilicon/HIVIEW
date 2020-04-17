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
        return rec->flag & GSF_FRM_FLAG_IDR;
    }
    else
    {
        gsf_frm_t rec;
        char *p = (char*)(&rec);
        memcpy(p, p1, n1);
        memcpy(p+n1, p2, size-n1);
        return rec.flag & GSF_FRM_FLAG_IDR;
    }
    
    return 0;
}

unsigned int cfifo_recgut(unsigned char *p1, unsigned int n1, unsigned char *p2, void *u)
{
    unsigned int len = cfifo_recsize(p1, n1, p2);
    unsigned int l = CFIFO_MIN(len, n1);
    
    char *p = (char*)u;
    memcpy(p, p1, l);
    memcpy(p+l, p2, len-l);

    gsf_frm_t *rec = (gsf_frm_t *)u;
  	struct timespec _ts;  
    clock_gettime(CLOCK_MONOTONIC, &_ts);
    int cost = (_ts.tv_sec*1000 + _ts.tv_nsec/1000000) - rec->utc;
    if(cost > 33)
      printf("get rec ok [delay:%d ms].\n", cost);
      
    assert(rec->data[0] == 00 && rec->data[1] == 00 && rec->data[2] == 00 && rec->data[3] == 01);

    return len;
}

static pthread_t gpid;
static char destip[64];
static unsigned short destport;

void* udp_send_task(void *parm)
{
  int ret = 0;
  
  struct cfifo_ex* video_fifo = NULL;
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
  sdp->video_shmid = -1;
  ret = GSF_MSG_SENDTO(GSF_ID_CODEC_SDP, 0, GET, 0
                        , sizeof(gsf_sdp_t)
                        , GSF_IPC_CODEC
                        , 2000);
  if(ret == 0)
  {
    video_fifo = cfifo_shmat(cfifo_recsize, cfifo_rectag, sdp->video_shmid);
    cfifo_ep_ctl(ep, CFIFO_EP_ADD, video_fifo);
    cfifo_newest(video_fifo, 1);
  }
  while(1)
  {
    
    struct cfifo_ex* result[255];
    ret = cfifo_ep_wait(ep, 2000, result, 255);
    if(ret <= 0)
    {
       printf("cfifo_ep_wait err ret:%d\n", ret);
    }
    struct cfifo_ex* fifo = result[0];
    while(ret > 0)
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
  return NULL;
}


int rawudp_open(char *ip, unsigned short port)
{
  strcpy(destip, ip);
  destport = port;
  
  return pthread_create(&gpid, NULL, udp_send_task, (void*)NULL);
}