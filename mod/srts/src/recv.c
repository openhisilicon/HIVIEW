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

#include "fw/h26xbits/inc/h26xbits.h"
#include "inc/frm.h"


#include "rtp-profile.h"
#include "rtp-payload.h"
#include "rtp.h"

typedef struct {
  char encoding[32];
  char host[64];
  int  port;
  int video_shmid;
  int audio_shmid;
  struct cfifo_ex *video_fifo;
  struct cfifo_ex *audio_fifo;
  gsf_frm_t *video_frm; // gsf_frm_t + __data_size__;
  int packet_cnt;
  int last_time;
} recv_ctx_t; 
static recv_ctx_t recv_ctx = {.video_shmid = -1, .audio_shmid = -1,};

int recv_shmid_get(gsf_shmid_t *shmid)
{
  shmid->video_shmid = recv_ctx.video_shmid;
  shmid->audio_shmid = recv_ctx.audio_shmid;
  printf("%s => video_shmid:%d, audio_shmid:%d\n", __func__, shmid->video_shmid, shmid->audio_shmid);
  return 0;
}


static unsigned int cfifo_recrel(unsigned char *p1, unsigned int n1, unsigned char *p2)
{
    return 0;
}
static unsigned int cfifo_recput(unsigned char *p1, unsigned int n1, unsigned char *p2, void *u)
{
  int i = 0, a = 0, l = 0, _n1 = n1;
  unsigned char *p = NULL, *_p1 = p1, *_p2 = p2;
  gsf_frm_t *rec = (gsf_frm_t*)u;

  p = (unsigned char*)(rec);
  a = sizeof(gsf_frm_t) + rec->size;
  
  l = CFIFO_MIN(a, _n1);
  memcpy(_p1, p, l);
  memcpy(_p2, p+l, a-l);
  _n1-=l;_p1+=l;_p2+=a-l;

  return 0;
}

static void rtp_packet(void* param, const void *packet, int bytes, uint32_t time, int flags)
{
  const uint8_t start_code[] = { 0, 0, 0, 1 };
  recv_ctx_t *ctx = (recv_ctx_t *)param; 
  
  if(flags)
  {
    printf("%s => encoding:%s, time:%08u, flags:%08d, drop.\n", __func__, ctx->encoding, time, flags);
  }

	struct timespec _ts;  
  clock_gettime(CLOCK_MONOTONIC, &_ts);
  ctx->last_time = _ts.tv_sec;

	if (0 == strcmp("h264", ctx->encoding))
	{
		uint8_t type = *(uint8_t*)packet & 0x1f;

		if(!ctx->video_frm)
		{
		  ctx->video_frm = (gsf_frm_t*)malloc(sizeof(gsf_frm_t)+GSF_FRM_MAX_SIZE);
		  memset(ctx->video_frm, 0, sizeof(gsf_frm_t));
		  ctx->packet_cnt = 0;
		}
		
    if (type == 7) // sps;
		{
      h26x_parse_sps_wh((char*)packet
          , type
          , bytes
          , &ctx->video_frm->video.width
          , &ctx->video_frm->video.height);
      //printf("%s => encoding:%s, time:%08u, flags:%08d, bytes:%d\n", __func__, ctx->encoding, time, flags, bytes);
		}
    if(ctx->video_frm->size + 4 + bytes <= GSF_FRM_MAX_SIZE)
    {
      ctx->video_frm->data[ctx->video_frm->size+0] = 00;
      ctx->video_frm->data[ctx->video_frm->size+1] = 00;
      ctx->video_frm->data[ctx->video_frm->size+2] = 00;
      ctx->video_frm->data[ctx->video_frm->size+3] = 01;
      
	    memcpy(ctx->video_frm->data + ctx->video_frm->size + 4, packet, bytes);
	    ctx->video_frm->video.nal[ctx->packet_cnt] = bytes + 4;
	    ctx->video_frm->size += bytes + 4;
	    ctx->packet_cnt++;
	  }
	  else
	  {
	    ctx->video_frm->size = 0;
	    ctx->packet_cnt = 0;
      return;
	  }

		if(type == 5 || type == 1)
		{
      ctx->video_frm->type = GSF_FRM_VIDEO;
      ctx->video_frm->flag = (type == 5)?GSF_FRM_FLAG_IDR:0;
      ctx->video_frm->seq  = ctx->video_frm->seq + 1;
      ctx->video_frm->utc  = _ts.tv_sec*1000 + _ts.tv_nsec/1000000;
      ctx->video_frm->pts  = ctx->video_frm->utc;
      ctx->video_frm->video.encode = GSF_ENC_H264;

      int ret = cfifo_put(ctx->video_fifo,  
                      ctx->video_frm->size+sizeof(gsf_frm_t),
                      cfifo_recput, 
                      (unsigned char*)ctx->video_frm);
      if(0)                
      printf("%s => encoding:%s, time:%08u, type:%d, flags:%08d, w:%d, h:%d, packet_cnt:%d\n"
			      , __func__, ctx->encoding, time, type, flags
			      , ctx->video_frm->video.width, ctx->video_frm->video.height
			      , ctx->packet_cnt);

      ctx->video_frm->size = 0;
	    ctx->packet_cnt = 0;
		}
	}
	else if (0 == strcmp("h265", ctx->encoding))
	{
		uint8_t type = (*(uint8_t*)packet >> 1) & 0x3f;
		
		if(type >= 0 && type <= 9) //PB;
		{
		  //NAL_TRAIL_N = 0,
      //NAL_TRAIL_R = 1,
      //NAL_TSA_N = 2,
      //NAL_TSA_R = 3,
      //NAL_STSA_N = 4,
      //NAL_STSA_R = 5,
      //NAL_RADL_N = 6,
      //NAL_RADL_R = 7,
      //NAL_RASL_N = 8,
      //NAL_RASL_R = 9,
		  ;
		}
		else if(type >= 16 && type <= 21) //I;
		{
      //NAL_BLA_W_LP = 16,
      //NAL_BLA_W_RADL = 17,
      //NAL_BLA_N_LP = 18,
      //NAL_IDR_W_RADL = 19,
      //NAL_IDR_N_LP = 20,
      //NAL_CRA_NUT = 21,
      ;
		}
		else if (type >= 32) // !vcl;
		{
      //NAL_VPS = 32,
      //NAL_SPS = 33,
      //NAL_PPS = 34,
      //NAL_AUD = 35,
      //NAL_EOS_NUT = 36,
      //NAL_EOB_NUT = 37,
      //NAL_FD_NUT = 38,
      //NAL_SEI_PREFIX = 39,
      //NAL_SEI_SUFFIX = 40,
			//printf("%s => encoding:%s, time:%08u, flags:%08d, bytes:%d\n", __func__, ctx->encoding, time, flags, bytes);
		}
		
		if(!ctx->video_frm)
		{
		  ctx->video_frm = (gsf_frm_t*)malloc(sizeof(gsf_frm_t)+GSF_FRM_MAX_SIZE);
		  memset(ctx->video_frm, 0, sizeof(gsf_frm_t));
		  ctx->packet_cnt = 0;
		}
		
    if (type == 33) // sps;
		{
      h26x_parse_sps_wh((char*)packet
          , type
          , bytes
          , &ctx->video_frm->video.width
          , &ctx->video_frm->video.height);
		}

    if(ctx->video_frm->size + 4 + bytes <= GSF_FRM_MAX_SIZE)
    {
      ctx->video_frm->data[ctx->video_frm->size+0] = 00;
      ctx->video_frm->data[ctx->video_frm->size+1] = 00;
      ctx->video_frm->data[ctx->video_frm->size+2] = 00;
      ctx->video_frm->data[ctx->video_frm->size+3] = 01;
      
	    memcpy(ctx->video_frm->data + ctx->video_frm->size + 4, packet, bytes);
	    ctx->video_frm->video.nal[ctx->packet_cnt] = bytes + 4;
	    ctx->video_frm->size += bytes + 4;
	    ctx->packet_cnt++;
	  }
	  else
	  {
	    ctx->video_frm->size = 0;
	    ctx->packet_cnt = 0;
      return;
	  }

		if(type >= 0 && type <= 21)
		{
      ctx->video_frm->type = GSF_FRM_VIDEO;
      ctx->video_frm->flag = (type >= 16)?GSF_FRM_FLAG_IDR:0;
      ctx->video_frm->seq  = ctx->video_frm->seq + 1;
      ctx->video_frm->utc  = _ts.tv_sec*1000 + _ts.tv_nsec/1000000;
      ctx->video_frm->pts  = ctx->video_frm->utc;
      ctx->video_frm->video.encode = GSF_ENC_H265;

      int ret = cfifo_put(ctx->video_fifo,  
                      ctx->video_frm->size+sizeof(gsf_frm_t),
                      cfifo_recput, 
                      (unsigned char*)ctx->video_frm);
      if(0)                
      printf("%s => encoding:%s, time:%08u, type:%d, flags:%08d, w:%d, h:%d, packet_cnt:%d\n"
			      , __func__, ctx->encoding, time, type, flags
			      , ctx->video_frm->video.width, ctx->video_frm->video.height
			      , ctx->packet_cnt);
			      
      ctx->video_frm->size = 0;
	    ctx->packet_cnt = 0;
		}
	}
  return;
}

static pthread_t recv_pid;
static int recvflag = 0;
void* recv_task(void *parm)
{
  int ret = 0;
  int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
  
  if(sockfd < 0)
  {
    printf("%s => socket error\n", __func__);
    return NULL;
  }
  
  int reuse = 1;
  setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (char *)&reuse, sizeof(reuse));

  struct timeval tv;
	tv.tv_sec  = 1;
	tv.tv_usec = 0;
	setsockopt(sockfd, SOL_SOCKET, SO_SNDTIMEO, &tv, sizeof(tv));
  setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));

  struct sockaddr_in local_addr;
	memset(&local_addr, 0, sizeof(local_addr));
	local_addr.sin_family = AF_INET;
	local_addr.sin_port   = htons(recv_ctx.port);
	local_addr.sin_addr.s_addr = INADDR_ANY;
  
  if(bind(sockfd, (struct sockaddr*)&local_addr, sizeof(local_addr)))
  {
    printf("bind error recv_ctx.port:%d\n", recv_ctx.port);
    return NULL;
  }
  
  int mc1 = 0;
  sscanf(recv_ctx.host, "%d.%*s", &mc1);
  mc1 = (mc1 >= 224 && mc1 <= 239)?1:0;
  if(mc1)
  {
    struct ip_mreq group;
    memset(&group, 0, sizeof(group));
    group.imr_multiaddr.s_addr = inet_addr(recv_ctx.host);
    group.imr_interface.s_addr = htonl(INADDR_ANY);
    printf("ip_mreq imr_interface INADDR_ANY\n");
    if(setsockopt(sockfd, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char *)&group, sizeof(group)) < 0)
    {
      perror("setsockopt IP_ADD_MEMBERSHIP");
      return NULL;
    }
    unsigned char loop = 0;
    setsockopt(sockfd, IPPROTO_IP, IP_MULTICAST_LOOP, &loop, sizeof(loop));
  }
  
  struct rtp_payload_t handler;
	handler.alloc = NULL;
	handler.free = NULL;
	handler.packet = rtp_packet;
	
  void* payload = strstr(recv_ctx.encoding, "h264")?
                  rtp_payload_decode_create(97, recv_ctx.encoding, &handler, &recv_ctx):
                  rtp_payload_decode_create(98, recv_ctx.encoding, &handler, &recv_ctx);
  while(recvflag)
  {
    struct sockaddr_in cli;
    int n = sizeof(cli);
    char buf[16*1024];
    
    //recv rtp; //recv rtcp;
    if((ret = recvfrom(sockfd, buf, sizeof(buf), 0, (struct sockaddr *)&cli, (socklen_t*)&n)) <= 0)
    {
      //printf("recvfrom timeout ret:%d\n", ret);
      continue;
    }
    //printf("recvfrom cli[%s,%d] ret:%d\n", inet_ntoa(cli.sin_addr), ntohs(cli.sin_port), ret);
    
    rtp_payload_decode_input(payload, buf, ret);
  }
  
  rtp_payload_decode_destroy(payload);
  close(sockfd);
  return NULL;
}

int recv_open(int ch, int st, char *url)
{
  if(recv_pid || !url || !strlen(url))
    return -1;
  
  sscanf(url, "%[^:]:%[^:]:%d", recv_ctx.encoding, recv_ctx.host, &recv_ctx.port); 
  printf("recv url:[%s], encoding:[%d], host:[%s], port:[%d]\n", url, recv_ctx.encoding, recv_ctx.host, recv_ctx.port);
  
  if(recv_ctx.video_shmid == -1)
  {   
    recv_ctx.audio_shmid = -1;
    recv_ctx.video_fifo = cfifo_alloc(2*GSF_FRM_MAX_SIZE,
              cfifo_recsize, 
              cfifo_rectag, 
              cfifo_recrel, 
              &recv_ctx.video_shmid,
              0);
  }
  recvflag = 1;
  return pthread_create(&recv_pid, NULL, recv_task, (void*)NULL);
}

int recv_close()
{
  if(!recv_pid)
    return -1;
    
  recvflag = 0;
  pthread_join(recv_pid, NULL);
  recv_pid = 0;
  printf("recv close encoding:[%d], host:[%s], port:[%d]\n", recv_ctx.encoding, recv_ctx.host, recv_ctx.port);  
  return 0;
}

