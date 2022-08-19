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

#include "inc/frm.h"

#include "mpeg-ps.h"
#include "mpeg-ts.h"
#include "mpeg-ts-proto.h"
#include <assert.h>
#include <string.h>
#include <stdio.h>

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

#define UDP_SEND_SIZE 1316
typedef struct {
  int sockfd;
  struct sockaddr_in *pdest_addr;
  int size;
  unsigned char sendbuf[4096];
}dest_t;

static int ts_write(void* param, const void* packet, size_t bytes)
{
  dest_t *dest = (dest_t*)param;
  
  memcpy(dest->sendbuf + dest->size, packet, bytes);
  dest->size += bytes;
  
  if(dest->size < UDP_SEND_SIZE)
  {
    return 0;
  }
  
  int ret = sendto(dest->sockfd, dest->sendbuf, dest->size, 0, (struct sockaddr *)dest->pdest_addr, sizeof(struct sockaddr_in));
  dest->size = 0;
  
  return 0;
}

static void* raw_create(dest_t *dest)
{
  return dest;
}

static int raw_send(void *raw, gsf_frm_t *rec)
{
  int ret = 0;
	char *pbuf = rec->data;
	int  left = rec->size;
	int  cnt = 0;
	
	dest_t *dest = (dest_t*)raw;
	
	while(1)
	{
		if (left <= UDP_SEND_SIZE)
		{
			ret = sendto(dest->sockfd, pbuf, left, 0, (struct sockaddr *)dest->pdest_addr, sizeof(struct sockaddr_in));
			assert(ret == left);
			break;
		}

		ret = sendto(dest->sockfd, pbuf, UDP_SEND_SIZE, 0, (struct sockaddr *)dest->pdest_addr, sizeof(struct sockaddr_in));
		assert(ret == UDP_SEND_SIZE);
		pbuf += UDP_SEND_SIZE;
		left -= UDP_SEND_SIZE;
		cnt  += UDP_SEND_SIZE;
		
		if(0)//if(cnt >= 10*UDP_SEND_SIZE)
		{
		  cnt = 0;
		  usleep(0);
		}
	}
	return 0;
}

#include "rtp-profile.h"
#include "rtp-payload.h"
#include "rtp.h"

#ifdef SRTP_ENABLE
#include "srtp.h"
srtp_t srtp;
#endif

enum {PROTOL_UDP  // udp://239.0.0.1:6666
    , PROTOL_SRT  // srt://:6666
    , PROTOL_RTP  // rtp://239.0.0.1:6666
    , PROTOL_SRTP // srtp://239.0.0.1:6666
    };
int protol = PROTOL_SRT;

void* RTPAlloc(void* param, int bytes)
{
	dest_t *dest = (dest_t*)param;
	assert(bytes <= sizeof(dest->sendbuf));
	return dest->sendbuf;
}

void RTPFree(void* param, void *packet)
{
	dest_t *dest = (dest_t*)param;
	assert(dest->sendbuf == packet);
}



void RTPPacket(void* param, const void *packet, int bytes, uint32_t timestamp, int flags)
{
	dest_t *dest = (dest_t*)param;
	assert(dest->sendbuf == packet);
	
	int len = bytes;
	
#ifdef SRTP_ENABLE
	if(protol == PROTOL_SRTP)
	{
    srtp_protect(srtp, (void*)packet, &len);
  }
#endif

	int ret = sendto(dest->sockfd, packet, len, 0, (struct sockaddr *)dest->pdest_addr, sizeof(struct sockaddr_in));
	assert(ret == len);
	// r <= 0 when peer close; assert(r == (int)len);
  //printf("sendto ret:%d, bytes:%d\n", ret, bytes);
}


void* udp_send_task(void *parm)
{
  printf("start.\n");
  int i = 0, ret = 0;
  struct cfifo_ex* video_fifo = NULL;
  struct cfifo_ex* audio_fifo = NULL;
  int ep = cfifo_ep_alloc(1);
  unsigned char buf[sizeof(gsf_frm_t)+GSF_FRM_MAX_SIZE];
  
  int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
  int reuse = 1;
  setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (char *)&reuse, sizeof(reuse));

  struct sockaddr_in dest_addr;
	memset(&dest_addr, 0, sizeof(dest_addr));
	dest_addr.sin_family = AF_INET;
	dest_addr.sin_port   = htons(destport);
	dest_addr.sin_addr.s_addr = inet_addr(destip);
  
  int mc1 = 0;
  sscanf(destip, "%d.%*s", &mc1);
  mc1 = (mc1 >= 224 && mc1 <= 239)?1:0;
  if(mc1)
  {
    struct in_addr localInterface;
    localInterface.s_addr = inet_addr("0.0.0.0");
    if(setsockopt(sockfd, IPPROTO_IP, IP_MULTICAST_IF, (char *)&localInterface, sizeof(localInterface)) < 0)
    {
      printf("setsockopt IP_MULTICAST_IF err.\n");
    }
  }
  printf("destip[%s], mc1:%d\n", destip, mc1);
  
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
  
  void* raw = raw_create(&dest);

  struct rtp_event_t event;
	event.on_rtcp = NULL;
  extern uint32_t rtp_ssrc(void);
  uint32_t ssrc = rtp_ssrc();

  struct rtp_payload_t s_rtpfunc = {
    RTPAlloc,
    RTPFree,
    RTPPacket,
  };
  //rtp_packet_setsize(1300);
  
  void* rtp = rtp_payload_encode_create(RTP_PAYLOAD_H264, "H264", (uint16_t)ssrc, ssrc, &s_rtpfunc, &dest);
  
#ifdef SRTP_ENABLE  
  // 00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F 10 11 12 13 14 15 16 17 18 19 1A 1B 1C 1D
  // AAECAwQFBgcICQoLDA0ODxAREhMUFRYXGBkaGxwd
  uint8_t key[30] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
                     0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D
                    };
  
  srtp_policy_t policy;
  memset(&policy, 0x0, sizeof(srtp_policy_t));

  int status = srtp_init();
  if (status)
    printf("error: srtp initialization failed with error code %d\n", status);
    
  srtp_crypto_policy_set_rtp_default(&policy.rtp);
  srtp_crypto_policy_set_rtcp_default(&policy.rtcp); 
  
  policy.key = key;

  //policy.ssrc.value = ssrc;
  //policy.ssrc.type = ssrc_specific;
  policy.ssrc.value = 0;
  policy.ssrc.type = ssrc_any_outbound;
  
  policy.next = NULL;

  policy.window_size = 128;
  policy.allow_repeat_tx = 0;

  policy.rtp.sec_serv = sec_serv_conf_and_auth;
  policy.rtcp.sec_serv = sec_serv_none; /* we don't do RTCP anyway */

  srtp_create(&srtp, &policy);
#endif

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
          
          // only test h264;
          if(rec->type != GSF_FRM_VIDEO || rec->video.encode != GSF_ENC_H264)
            continue;
            
          switch(protol)
          {
            case PROTOL_UDP:
              ret = raw_send(raw, rec);
              //printf("raw_send size:%d, ret:%d\n", rec->size, ret);
              break;
            case PROTOL_RTP:
            case PROTOL_SRTP:
              ret = rtp_payload_encode_input(rtp, rec->data, rec->size, rec->pts * 90);
              //printf("rtp_payload_encode_input size:%d, ret:%d\n", rec->size, ret);
              break;
            case PROTOL_SRT:
        		  // MPEG-TS
        		  if(trackid <= 0)
        		  {
        		    trackid = mpeg_ts_add_stream(ts, PSI_STREAM_H264, NULL, 0);
        		  }
        		  if(trackid > 0)
        		  {
        		    int ret = mpeg_ts_write(ts, trackid, (rec->flag&GSF_FRM_FLAG_IDR)?1:0, rec->pts * 90, rec->pts * 90, rec->data, rec->size);
        		    //printf("mpeg_ts_write size:%d, ret:%d\n", rec->size, ret);
        		  }
              break;
          }
      }
    }
  }
  
  if(ep)
    cfifo_ep_free(ep);
  
  if(ts)
    mpeg_ts_destroy(ts);
  
  if(rtp)
    rtp_payload_encode_destroy(rtp);
    
#ifdef SRTP_ENABLE
  if(srtp)  
    srtp_dealloc(srtp);
    
  srtp_shutdown();
#endif
  
  if(video_fifo)
    cfifo_free(video_fifo);
    
  if(audio_fifo)
    cfifo_free(audio_fifo);
  
  if(sockfd > 0)
    close(sockfd);
    
  printf("exit.\n");
  return NULL;
}


int udp_open(char *ip, unsigned short port)
{
  if(!strlen(srts_parm.url) || gpid)
    return -1;
    
  if(protol == PROTOL_SRT)
  { 
    // local udp => srt-live-transmit srts_parm.url;
    strcpy(ip, "127.0.0.1");
    port = 6666;
    char cmd[256];
    sprintf(cmd, "srt-live-transmit -q udp://:%d %s &", port, srts_parm.url);
    system(cmd);
  }
  
  strcpy(destip, ip);
  destport = port;
  return pthread_create(&gpid, NULL, udp_send_task, (void*)NULL);
}

int udp_close(void)
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

    GSF_LOG_CONN(1, 100);
    void* rep = nm_rep_listen(GSF_IPC_SRTS, NM_REP_MAX_WORKERS, NM_REP_OSIZE_MAX, req_recv);

    int en = 0;

    while(1)
    {
      if(en != srts_parm.en)
      {
        if(en == 0 && srts_parm.en > 0)
        {
          char pt[64] = {0};
          char host[64] = {0};
          int  port = 0;
          sscanf(srts_parm.url, "%[^:]:%[^:]:%d", pt, host, &port);
          
          protol = !strcmp(pt, "udp")?PROTOL_UDP:
                   !strcmp(pt, "srt")?PROTOL_SRT:
                   !strcmp(pt, "rtp")?PROTOL_RTP:PROTOL_SRTP;

          printf("url:[%s], protol:[%d], host:[%s], port:[%d]\n", srts_parm.url, protol, host, port);            
          udp_open(&host[2], port);
        }
        else if(en > 0 && srts_parm.en == 0)
        {
          udp_close();
        }
        en = srts_parm.en;
      }
      sleep(1);
    }
    

    GSF_LOG_DISCONN();
    return 0;
}