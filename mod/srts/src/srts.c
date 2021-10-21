#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <assert.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>

#include "inc/gsf.h"

#include "srts.h"
#include "cfg.h"
#include "msg_func.h"

#include "mod/bsp/inc/bsp.h"
#include "mod/codec/inc/codec.h"

GSF_LOG_GLOBAL_INIT("SRTS", 8*1024);

#define MAX_FRAME_SIZE (800*1024)


static int req_recv(char *in, int isize, char *out, int *osize, int err)
{
    int ret = 0;
    gsf_msg_t *req = (gsf_msg_t*)in;
    gsf_msg_t *rsp = (gsf_msg_t*)out;

    *rsp  = *req;
    rsp->err  = 0;
    rsp->size = 0;

    ret = msg_func_proc(req, isize, rsp, osize);

    rsp->err = (ret == TRUE)?rsp->err:GSF_ERR_MSG;
    *osize = sizeof(gsf_msg_t)+rsp->size;

    return 0;
}


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
    
    //printf("len:%d, l1:%d\n", len, l);
    
    char *p = (char*)u;
    memcpy(p, p1, l);
    memcpy(p+l, p2, len-l);

    gsf_frm_t *rec = (gsf_frm_t *)u;
  	struct timespec _ts;  
    clock_gettime(CLOCK_MONOTONIC, &_ts);
    int cost = (_ts.tv_sec*1000 + _ts.tv_nsec/1000000) - rec->utc;
    if(cost > 33)
      printf("get rec ok [delay:%d ms].\n", cost);

    return len;
}


#include "mpeg-ps.h"
#include "mpeg-ts.h"
#include "mpeg-ts-proto.h"
#include <assert.h>
#include <string.h>
#include <stdio.h>

#define MAX_FRAME_SIZE (800*1024)

static pthread_t gpid;
static int sendflag = 0;
static char destip[64];
static unsigned short destport;

static void* ts_alloc(void* param, size_t bytes)
{
	static char s_buffer[188];
	assert(bytes <= sizeof(s_buffer));
	return s_buffer;
}

static void ts_free(void* param, void* packet)
{
	return;
}

typedef struct {
  int sockfd;
  struct sockaddr_in *pdest_addr;
  int size;
  unsigned char sendbuf[1316];
}dest_t;

static int ts_write(void* param, const void* packet, size_t bytes)
{
  dest_t *dest = (dest_t*)param;
  
  memcpy(dest->sendbuf + dest->size, packet, bytes);
  dest->size += bytes;
  
  if(dest->size < sizeof(dest->sendbuf))
  {
    return 0;
  }
  
  int ret = sendto(dest->sockfd, dest->sendbuf, dest->size, 0, (struct sockaddr *)dest->pdest_addr, sizeof(struct sockaddr_in));
  dest->size = 0;
  
  return 0;
}



void* udp_send_task(void *parm)
{
  printf("start.\n");
  int i = 0, ret = 0;
  struct cfifo_ex* video_fifo = NULL;
  struct cfifo_ex* audio_fifo = NULL;
  int ep = cfifo_ep_alloc(1);
  unsigned char buf[sizeof(gsf_frm_t)+MAX_FRAME_SIZE];
  
  int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
  int reuse = 1;
  setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (char *)&reuse, sizeof(reuse));
  int send_size = 1316;
  struct sockaddr_in dest_addr;
	memset(&dest_addr, 0, sizeof(dest_addr));
	dest_addr.sin_family = AF_INET;
	dest_addr.sin_port   = htons(destport);
	dest_addr.sin_addr.s_addr = inet_addr(destip);
  
  dest_t dest = {
    .sockfd = sockfd,
    .pdest_addr = &dest_addr,
    .size  = 0
  };
  
  int trackid = 0;
  struct mpeg_ts_func_t tshandler;
	tshandler.alloc = ts_alloc;
	tshandler.write = ts_write;
	tshandler.free = ts_free;
  void* ts = mpeg_ts_create(&tshandler, &dest);
  
  GSF_MSG_DEF(gsf_sdp_t, sdp, sizeof(gsf_msg_t)+sizeof(gsf_sdp_t));
  do
  {
    //wait codec.exe;
    sdp->video_shmid = sdp->audio_shmid = -1;
    ret = GSF_MSG_SENDTO(GSF_ID_CODEC_SDP, 0, GET, 0
                          , sizeof(gsf_sdp_t)
                          , GSF_IPC_CODEC
                          , 2000);
  }while(destport && ret < 0);
  
  if(ret == 0)
  {
    video_fifo = cfifo_shmat(cfifo_recsize, cfifo_rectag, sdp->video_shmid);
    //audio_fifo = cfifo_shmat(cfifo_recsize, cfifo_rectag, sdp->audio_shmid);
    
    cfifo_ep_ctl(ep, CFIFO_EP_ADD, video_fifo);
    unsigned int video_utc = cfifo_newest(video_fifo, 1);
     
    if(audio_fifo)
    {
      cfifo_ep_ctl(ep, CFIFO_EP_ADD, audio_fifo);
      unsigned int audio_utc = cfifo_oldest(audio_fifo, video_utc);
      printf("video_utc:%u, audio_utc:%u\n", video_utc, audio_utc);
    }
  }
  while(destport)
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
          //printf("rec->type:%d, seq:%d, utc:%u, size:%d\n", rec->type, rec->seq, rec->utc, rec->size);
          
          if(0)
          {
            // UDP-RAW;
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
      		else if(rec->type == GSF_FRM_VIDEO && rec->video.encode == GSF_ENC_H264)
      		{
      		  // MPEG-TS
      		  if(trackid <= 0)
      		  {
      		    trackid = mpeg_ts_add_stream(ts, PSI_STREAM_H264, NULL, 0);
      		  }
      		  if(trackid > 0)
      		  {
      		    int ret = mpeg_ts_write(ts, trackid, (rec->flag&GSF_FRM_FLAG_IDR)?1:0, rec->pts * 90, rec->pts * 90, rec->data, rec->size);
      		    printf("mpeg_ts_write size:%d, ret:%d\n", rec->size, ret);
      		  }
      		}
      }
    }
  }
  
  if(ep)
    cfifo_ep_free(ep);
  
  if(ts)
    mpeg_ts_destroy(ts);
  
  if(video_fifo)
    cfifo_free(video_fifo);
    
  if(audio_fifo)
    cfifo_free(audio_fifo);
  
  if(sockfd > 0)
    close(sockfd);
    
  printf("exit.\n");
  return NULL;
}


int rawudp_open(char *ip, unsigned short port)
{
  if(!strlen(srts_parm.url) || gpid)
    return -1;
    
  char cmd[256];
  sprintf(cmd, "srt-live-transmit -q udp://:%d %s &", port, srts_parm.url);
  system(cmd);
  
  strcpy(destip, ip);
  destport = port;
  return pthread_create(&gpid, NULL, udp_send_task, (void*)NULL);
}

int rawudp_close(void)
{
  if(!gpid)
    return -1;

  system("killall -9 srt-live-transmit");
  
  destport = 0;
  pthread_join(gpid, NULL);
  gpid = 0;
}


int main(int argc, char *argv[])
{
    if(argc < 2)
    {
      printf("pls input: %s srts_parm.json\n", argv[0]);
      return -1;
    }
    
    strncpy(srts_parm_path, argv[1], sizeof(srts_parm_path)-1);
    
    if(json_parm_load(srts_parm_path, &srts_parm) < 0)
    {
      json_parm_save(srts_parm_path, &srts_parm);
      json_parm_load(srts_parm_path, &srts_parm);
    }
    info("srts_parm => en:%d, url:[%s]\n", srts_parm.en, srts_parm.url);
    
    GSF_LOG_CONN(1, 100);
    void* rep = nm_rep_listen(GSF_IPC_SRTS, NM_REP_MAX_WORKERS, NM_REP_OSIZE_MAX, req_recv);

    int en = 0;

    while(1)
    {
      if(en != srts_parm.en)
      {
        if(en == 0 && srts_parm.en > 0)
          rawudp_open("127.0.0.1", 5555);
        else if(en > 0 && srts_parm.en == 0)
          rawudp_close();

        en = srts_parm.en;
      }
      sleep(1);
    }
    

    GSF_LOG_DISCONN();
    return 0;
}