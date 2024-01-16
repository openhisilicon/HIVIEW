#include "rtp-media.h"
#include "rtp.h"
#include "time64.h"

#include "inc/gsf.h"
#include "mod/codec/inc/codec.h"

#include "inc/frm.h"

#include <stddef.h> // container_of
#define CONTAINER_OF(ptr, struct_type, member) \
        (struct_type*)((char*)(ptr) - offsetof(struct_type, member))
        
enum {
  MEDIA_TRACK_VIDEO = 0, // a=control:video
  MEDIA_TRACK_AUDIO = 1, // a=control:audio
  MEDIA_TRACK_META  = 2, // a=control:meta
  MEDIA_TRACK_BUTT
};

#define MAX_UDP_PACKET (1450-16)

struct media_track_t
{
  unsigned char m_packet[MAX_UDP_PACKET+14];
  struct rtp_transport_t* m_transport;
  void* m_rtppacker; // rtp encoder
  void* m_rtp;       // rtp status
  struct cfifo_ex* m_reader;
  int m_evfd;
  int sendcnt;
  int senderr;
  uint32_t m_rtp_clock;
  time64_t m_rtcp_clock;
};

struct rtp_media_live_t
{
  struct rtp_media_t base;
  
  // private;
  int ch, st;
  int loop, exited;
  st_thread_t tid;
  struct media_track_t track[MEDIA_TRACK_BUTT];
  int cur_track;
  unsigned char *cur_frm;
};

int SendRTCP(struct media_track_t *t)
{
	// make sure have sent RTP packet

	time64_t clock = time64_now();
	int interval = rtp_rtcp_interval(t->m_rtp);
	if(0 == t->m_rtcp_clock || t->m_rtcp_clock + interval < clock)
	{
		char rtcp[1024] = {0};
		size_t n = rtp_rtcp_report(t->m_rtp, rtcp, sizeof(rtcp));

		t->m_transport->send(t->m_transport, 1, rtcp, n);

		t->m_rtcp_clock = clock;
	}
	
	return 0;
}

static int live_sendto(struct rtp_media_live_t* m)
{
  int j = m->cur_track;
  gsf_frm_t *rec = (gsf_frm_t*)m->cur_frm;
  
  int sp = (rec->type == GSF_FRM_VIDEO)?90:
            (rec->type == GSF_FRM_AUDIO)?rec->audio.sp:
            1;
            
  //sync video;
  if(rec->type != GSF_FRM_VIDEO)
  {
    if(m->track[MEDIA_TRACK_VIDEO].m_rtp_clock == 0)
      return 0;  
  }
  
  if(m->track[j].m_rtp_clock == 0)
  {
    if(rec->flag & GSF_FRM_FLAG_IDR)
      m->track[j].m_rtp_clock = rec->pts;
    else
      return 0;
  }
  
  m->track[j].sendcnt = 0;
  
  //if(rec->type == GSF_FRM_VIDEO)
  //  printf("rec->seq:%u, rec->utc:%u\n", rec->seq, rec->utc);
    
  uint32_t timestamp = rec->pts - m->track[j].m_rtp_clock;
    
  int skip = 0;
  if(rec->type == GSF_FRM_AUDIO 
    && (rec->audio.encode == GSF_ENC_G711A || rec->audio.encode == GSF_ENC_G711U))
  {
    skip = 4;
  }
 
  if(rec->flag & GSF_FRM_FLAG_PHY)
  {
    #ifdef __FRM_PHY__
    int i = 0;
    venc_phyaddr_t* phyaddr = (venc_phyaddr_t*)rec->data;
    
    venc_phyaddr_mmap(phyaddr);
    
    #if 1
    gsf_frm_t *rec = (gsf_frm_t *)m->cur_frm;
  	struct timespec _ts;  
    clock_gettime(CLOCK_MONOTONIC, &_ts);
    int cost = (_ts.tv_sec*1000 + _ts.tv_nsec/1000000) - rec->utc;
    if(cost > 10)
      printf("m:%p, get rec phy size:%d, [delay:%d ms].\n", m, phyaddr->u32FrameSize, cost);
    #endif

    for(i = 0; i < phyaddr->u32PackCount; i++)
    {
      int nals = 0;
      unsigned char *nalu = NULL;
      
      if(phyaddr->stPack[i].u8Addr2)
      {
        nalu = rec->data + sizeof(venc_phyaddr_t);
        
        memcpy(nalu, phyaddr->stPack[i].u8Addr, phyaddr->stPack[i].u32Len);
        nals += phyaddr->stPack[i].u32Len;
        
        memcpy(nalu + nals, phyaddr->stPack[i].u8Addr2, phyaddr->stPack[i].u32Len2);
        nals += phyaddr->stPack[i].u32Len2;
        //printf("i:%d, u64PhyAddr2:%llx, nalu:[%02x %02x %02x %02x %02x], nals:%d\n", i, phyaddr->stPack[i].u64PhyAddr2, nalu[0], nalu[1], nalu[2], nalu[3], nalu[4], nals);
        rtp_payload_encode_input(m->track[j].m_rtppacker, nalu + 4, nals - 4, timestamp * sp /*kHz*/);  
      }
      else 
      {
        nals = phyaddr->stPack[i].u32Len;
        nalu = phyaddr->stPack[i].u8Addr;
        //printf("i:%d, u64PhyAddr:%llx, nalu:[%02x %02x %02x %02x %02x], nals:%d\n", i, phyaddr->stPack[i].u64PhyAddr, nalu[0], nalu[1], nalu[2], nalu[3], nalu[4], nals);
        rtp_payload_encode_input(m->track[j].m_rtppacker, nalu + 4, nals - 4, timestamp * sp /*kHz*/);  
      }
    }
    venc_phyaddr_munmap(phyaddr);
    #else
    printf("%s => error unknow GSF_FRM_FLAG_PHY\n", __func__);
    #endif
  }
  else   
  {
    #if 1  
    gsf_frm_t *rec = (gsf_frm_t *)m->cur_frm;
  	struct timespec _ts;  
    clock_gettime(CLOCK_MONOTONIC, &_ts);
    int cost = (_ts.tv_sec*1000 + _ts.tv_nsec/1000000) - rec->utc;
    if(cost > 10)
      printf("m:%p, get rec data size:%d, [delay:%d ms].\n", m, rec->size, cost);
    #endif 
 
    rtp_payload_encode_input(m->track[j].m_rtppacker, rec->data+skip, rec->size-skip, timestamp * sp /*kHz*/); 
    // SendRTP() used st_writev();
    // SendRTCP(&m->track[j]); 
  }
  
  if(m->track[j].senderr)
  {
    m->loop = 0;
    printf("m:%p, loop = %d\n", m, m->loop);
  }  
  return 0;
}

static unsigned int cfifo_recgut_me(unsigned char *p1, unsigned int n1, unsigned char *p2, void *u)
{
    unsigned int len = cfifo_recsize(p1, n1, p2);
    unsigned int l = CFIFO_MIN(len, n1);
    
    struct rtp_media_live_t* m = (struct rtp_media_live_t*)u;
    
    //copy video frame;
    char *p = (char*)m->cur_frm;
    memcpy(p, p1, l);
    memcpy(p+l, p2, len-l);
    
    #ifdef __FRM_PHY__
    #warning "DEFINED __FRM_PHY__ DEFINED"
    //when the network is unavailable, cfifo is blocked;
    //About __DMA_COPY__, please refrence /mod/webs/src/http.c;
    live_sendto(m);
    #endif
    
    return len;
}




static void *live_send_task(void *arg)
{
  int i = 0, j = 0, ret = 0;

  struct rtp_media_live_t* m = (struct rtp_media_live_t*)arg;
  
  m->cur_frm = malloc(GSF_FRM_MAX_SIZE);
  
  struct pollfd fds[MEDIA_TRACK_BUTT] = {0};
  st_netfd_t m_evfd[MEDIA_TRACK_BUTT] = {NULL};
 
  if(m->track[MEDIA_TRACK_VIDEO].m_reader)
  {
    cfifo_newest(m->track[MEDIA_TRACK_VIDEO].m_reader, 0);
    GSF_MSG_DEF(char, msgdata, sizeof(gsf_msg_t));
    GSF_MSG_SENDTO(GSF_ID_CODEC_IDR, m->ch, SET, m->st
                    , 0
                    , GSF_IPC_CODEC
                    , 2000);
                    
    fds[j].fd = m->track[MEDIA_TRACK_VIDEO].m_evfd;
	#if defined(GSF_CPU_3519d) 
    fds[j].events = POLLIN|(cfifo_fd_et(fds[j].fd)?POLLET:0);
	#else
	fds[j].events = POLLIN|POLLET;
	#endif
    m_evfd[MEDIA_TRACK_VIDEO] = st_netfd_open(fds[j].fd);
    j++;
  }
  if(m->track[MEDIA_TRACK_AUDIO].m_reader)
  {
    cfifo_newest(m->track[MEDIA_TRACK_AUDIO].m_reader, 0);
    fds[j].fd = m->track[MEDIA_TRACK_AUDIO].m_evfd;
	#if defined(GSF_CPU_3519d) 
    fds[j].events = POLLIN|(cfifo_fd_et(fds[j].fd)?POLLET:0);
	#else
	fds[j].events = POLLIN|POLLET;
	#endif
    m_evfd[MEDIA_TRACK_AUDIO] = st_netfd_open(fds[j].fd);
    j++;
  }
  
  int fdcnt = j;
  
  while(m->loop)
  {
    //get stream;
    
    fds[MEDIA_TRACK_VIDEO].revents = 0;
    fds[MEDIA_TRACK_AUDIO].revents = 0;
    fds[MEDIA_TRACK_META].revents  = 0;

    if (st_poll(fds, fdcnt, 1000*1000) <= 0)
    {
      printf("%s => st_poll err.\n", __func__);
      continue;
    }
    //printf("%s => st_poll ts:%llu ms\n", __func__, time64_now());

    for(j = 0; j < MEDIA_TRACK_BUTT; j++)
    {
      if ((fds[j].revents & POLLIN)
          && m->track[j].m_transport)
      {
        #if defined(GSF_CPU_3519d)
        cfifo_fd_resume(fds[j].fd);
        #endif

        if(i%60 == 0)
        {
          char rr[1024] = {0};
          int r = m->track[j].m_transport->recv(m->track[j].m_transport, 1, rr, sizeof(rr));
        }
        
        // get data;
        m->cur_track = j;
        while(m->loop)
        {
          int ret = cfifo_get(m->track[j].m_reader, cfifo_recgut_me, (void*)m);
          if(ret <= 0)
          {
            break;
          }  
          #ifndef __FRM_PHY__
          //The video frame has been copied to cur_frm;
          live_sendto(m);
          #endif
        }
      }
    }
    
    i++;
  }

  for(j = 0; j < MEDIA_TRACK_BUTT; j++)
  {
    if(m_evfd[j])
    {
      st_netfd_poll(m_evfd[j], POLLIN, 1);
      st_netfd_close(m_evfd[j]);
    }
  }
  
  if(m->cur_frm)
    free(m->cur_frm);
  
  printf("%s => exit m:%p .\n", __func__, m);
  m->exited = 1;
  st_thread_exit(NULL);
  return NULL;
}


void* RTPAlloc(void* param, int bytes)
{
	struct media_track_t *t = (struct media_track_t*)param;
	assert(bytes <= sizeof(t->m_packet));
	return t->m_packet;
}

void RTPFree(void* param, void *packet)
{
	struct media_track_t *t = (struct media_track_t*)param;
	assert(t->m_packet == packet);
}

void RTPPacket(void* param, const void *packet, int bytes, uint32_t timestamp, int flags)
{
	struct media_track_t *t = (struct media_track_t*)param;
	assert(t->m_packet == packet);
	
	if(t->senderr)
	{  
	  return;
	}
	
	int r = t->m_transport->send(t->m_transport, 0, packet, bytes);
	// r <= 0 when peer close; assert(r == (int)bytes);
	if(r <= 0)
	{
	  printf("send t:%p, r:%d\n", t, r);
	  t->senderr = 1;
	}
	
	#if 0
	if(!t->m_transport->istcp && ++t->sendcnt > 50)
	{
	   st_usleep(5*1000);
	   t->sendcnt = 0;
	}
	#endif
	
	rtp_onsend(t->m_rtp, packet, bytes);
}


static int rtp_live_get_sdp(struct rtp_media_t* _m, char *sdp)
{
  struct rtp_media_live_t *m = (struct rtp_media_live_t*)_m;
  
  static const char* pattern_video =
    "m=video 0 RTP/AVP %d\r\n"
    "a=rtpmap:%d %s/90000\r\n"
    "a=range:npt=now-\r\n"
    "a=recvonly\r\n"
    "a=control:video\r\n"
    "a=fmtp:%d profile-level-id=%02X%02X%02X;"
    //"packetization-mode=1;"
    //"sprop-parameter-sets="
    ;


  GSF_MSG_DEF(gsf_sdp_t, gsf_sdp, sizeof(gsf_msg_t)+sizeof(gsf_sdp_t));
  gsf_sdp->video_shmid = -1;
  gsf_sdp->audio_shmid = -1;
  if(GSF_MSG_SENDTO(GSF_ID_CODEC_SDP, m->ch, GET, m->st
                        , 0
                        , GSF_IPC_CODEC
                        , 2000) < 0)
  {
    printf("%s => get SDP err.\n" ,__func__);
    return 0;
  }

  struct rtp_payload_t s_rtpfunc = {
    RTPAlloc,
    RTPFree,
    RTPPacket,
  };
  
  struct rtp_event_t event;
	event.on_rtcp = NULL;
  
  extern uint32_t rtp_ssrc(void);
  uint32_t ssrc = rtp_ssrc();
  
  if(gsf_sdp->venc.type == GSF_ENC_H265)
    m->track[MEDIA_TRACK_VIDEO].m_rtppacker = 
        rtp_payload_encode_create(RTP_PAYLOAD_H265, "H265", (uint16_t)ssrc, ssrc, &s_rtpfunc, &m->track[MEDIA_TRACK_VIDEO]);
  else
    m->track[MEDIA_TRACK_VIDEO].m_rtppacker = 
        rtp_payload_encode_create(RTP_PAYLOAD_H264, "H264", (uint16_t)ssrc, ssrc, &s_rtpfunc, &m->track[MEDIA_TRACK_VIDEO]);

	m->track[MEDIA_TRACK_VIDEO].m_rtp = rtp_create(&event, NULL, ssrc, ssrc, 90000, 4*1024, 1);
	
	if(gsf_sdp->venc.type == GSF_ENC_H265)
	  rtp_set_info(m->track[MEDIA_TRACK_VIDEO].m_rtp, "RTSPServer", "live.h265");
	else
	  rtp_set_info(m->track[MEDIA_TRACK_VIDEO].m_rtp, "RTSPServer", "live.h264");
  
  m->track[MEDIA_TRACK_VIDEO].m_reader = cfifo_shmat(cfifo_recsize, cfifo_rectag, gsf_sdp->video_shmid);
  m->track[MEDIA_TRACK_VIDEO].m_evfd   = cfifo_take_fd(m->track[MEDIA_TRACK_VIDEO].m_reader);
  
  printf("%s => get SDP ok. ch:%d, st:%d, type:%d, video[shmid:%d, reader:%p, evfd:%d]\n" 
        , __func__
        , m->ch, m->st, gsf_sdp->venc.type
        , gsf_sdp->video_shmid
        , m->track[MEDIA_TRACK_VIDEO].m_reader
        , m->track[MEDIA_TRACK_VIDEO].m_evfd);
        
  if(m->track[MEDIA_TRACK_VIDEO].m_reader == NULL 
    || m->track[MEDIA_TRACK_VIDEO].m_evfd < 0)
  {
    return 0;
  }

  uint32_t ssrc2 = rtp_ssrc();
  
  if(gsf_sdp->aenc.type == GSF_ENC_AAC)
  {  
    m->track[MEDIA_TRACK_AUDIO].m_rtppacker = 
        rtp_payload_encode_create(RTP_PAYLOAD_MP4A, "mpeg4-generic", (uint16_t)ssrc2, ssrc2, &s_rtpfunc, &m->track[MEDIA_TRACK_AUDIO]);
    m->track[MEDIA_TRACK_AUDIO].m_rtp = rtp_create(&event, NULL, ssrc2, ssrc2, 48000, 4*1024, 1);
    rtp_set_info(m->track[MEDIA_TRACK_AUDIO].m_rtp, "RTSPServer", "live.aac");
  }
  else if(gsf_sdp->aenc.type == GSF_ENC_G711A)
  {
    m->track[MEDIA_TRACK_AUDIO].m_rtppacker =
        rtp_payload_encode_create(RTP_PAYLOAD_PCMA, "pcma", (uint16_t)ssrc2, ssrc2, &s_rtpfunc, &m->track[MEDIA_TRACK_AUDIO]);
    m->track[MEDIA_TRACK_AUDIO].m_rtp = rtp_create(&event, NULL, ssrc2, ssrc2, 8000, 4*1024, 1);
    rtp_set_info(m->track[MEDIA_TRACK_AUDIO].m_rtp, "RTSPServer", "live.pcma");
  }
  else
  {  
    m->track[MEDIA_TRACK_AUDIO].m_rtppacker = 
        rtp_payload_encode_create(RTP_PAYLOAD_PCMU, "pcmu", (uint16_t)ssrc2, ssrc2, &s_rtpfunc, &m->track[MEDIA_TRACK_AUDIO]);
    m->track[MEDIA_TRACK_AUDIO].m_rtp = rtp_create(&event, NULL, ssrc2, ssrc2, 8000, 4*1024, 1);
    rtp_set_info(m->track[MEDIA_TRACK_AUDIO].m_rtp, "RTSPServer", "live.pcmu");
  }
  
  m->track[MEDIA_TRACK_AUDIO].m_reader = cfifo_shmat(cfifo_recsize, cfifo_rectag, gsf_sdp->audio_shmid);
  m->track[MEDIA_TRACK_AUDIO].m_evfd   = cfifo_take_fd(m->track[MEDIA_TRACK_AUDIO].m_reader);
  
  printf("%s => get SDP ok. ch:%d, st:%d, audio[shmid:%d, reader:%p, evfd:%d]\n" 
        , __func__
        , m->ch, m->st
        , gsf_sdp->audio_shmid
        , m->track[MEDIA_TRACK_AUDIO].m_reader
        , m->track[MEDIA_TRACK_AUDIO].m_evfd);
  
  char media[1024] = {0};
  if(gsf_sdp->venc.type == GSF_ENC_H265)
    snprintf(media, sizeof(media), pattern_video, RTP_PAYLOAD_H265, RTP_PAYLOAD_H265, "H265",
            RTP_PAYLOAD_H265, 0,0,0);
  else
    snprintf(media, sizeof(media), pattern_video, RTP_PAYLOAD_H264, RTP_PAYLOAD_H264, "H264",
            RTP_PAYLOAD_H264, 0,0,0);

  //media += parameters;
  //media += '\n';
  strcat(media, "\r\n");
  strcat(sdp, media);
  
  if(m->track[MEDIA_TRACK_AUDIO].m_reader)
  {
  	static const char* pattern_audio =
    	"m=audio 0 RTP/AVP %d\r\n"
    	"a=rtpmap:%d %s/%d/%d\r\n"
      "a=range:npt=now-\r\n"
      "a=recvonly\r\n"
    	"a=control:audio\r\n";
    
    if(gsf_sdp->aenc.type == GSF_ENC_AAC)
      snprintf(media, sizeof(media), pattern_audio, RTP_PAYLOAD_MP4A, RTP_PAYLOAD_MP4A, "mpeg4-generic",48000, 2);
    else if(gsf_sdp->aenc.type == GSF_ENC_G711A)
      snprintf(media, sizeof(media), pattern_audio, RTP_PAYLOAD_PCMA, RTP_PAYLOAD_PCMU, "PCMA", 8000, 1);
    else 	
      snprintf(media, sizeof(media), pattern_audio, RTP_PAYLOAD_PCMU, RTP_PAYLOAD_PCMU, "PCMU", 8000, 1);

    strcat(sdp, media);
    
    if(gsf_sdp->aenc.type == GSF_ENC_AAC)
    {
      char fmtp[256];
      sprintf(fmtp, "a=fmtp:%d streamType=5;profile-level-id=1;mode=AAC-hbr;sizelength=13;indexlength=3;indexdeltalength=3\r\n", RTP_PAYLOAD_MP4A);
      strcat(sdp, fmtp);
    }
  }
  return 0;
}

static int rtp_live_get_duration(struct rtp_media_t* _m, int64_t* duration)
{
  return -1;
}

static int rtp_live_play(struct rtp_media_t* _m)
{
  struct rtp_media_live_t *m = (struct rtp_media_live_t*)_m;

  //start thread;
  m->loop = 1; m->exited = !m->loop;
  if ((m->tid = st_thread_create(live_send_task, m, 0, 0)) == NULL) {
    m->loop = 0; m->exited = !m->loop;
    printf("st_thread_create err.\n");
    return -1;
  }
  return 0;
}

static int rtp_live_pause(struct rtp_media_t* _m)
{
  return -1;
}

static int rtp_live_set_speed(struct rtp_media_t* _m, double speed)
{
  return -1;
}

static int rtp_live_seek(struct rtp_media_t* _m, int64_t pos)
{
  return -1;
}

static int rtp_live_get_rtpinfo(struct rtp_media_t* _m, const char* uri, char *rtpinfo, size_t bytes)
{
  struct rtp_media_live_t *m = (struct rtp_media_live_t*)_m;
	uint16_t seq = 0;
	uint32_t timestamp = 0;
	if(m->track[MEDIA_TRACK_VIDEO].m_rtppacker)
	{
	  rtp_payload_encode_getinfo(m->track[MEDIA_TRACK_VIDEO].m_rtppacker, &seq, &timestamp);
  }
	// url=rtsp://video.example.com/twister/video;seq=12312232;rtptime=78712811
	snprintf(rtpinfo, bytes, "url=%s;seq=%hu;rtptime=%u", uri, seq, timestamp);
	return 0;
}


static int rtp_live_add_transport(struct rtp_media_t* _m, const char* track, struct rtp_transport_t* transport)
{
  struct rtp_media_live_t *m = (struct rtp_media_live_t*)_m;
  
  int id = strstr(track, "video")?MEDIA_TRACK_VIDEO:
           strstr(track, "audio")?MEDIA_TRACK_AUDIO:
           strstr(track, "meta")?MEDIA_TRACK_META:-1;

  if(id < 0 || m->track[id].m_transport)
  {
    transport->free(transport);
    return -1;
  }
  
  m->track[id].m_transport = transport;
  return 0;
}


struct rtp_media_t* rtp_media_live_new(int ch, int st)
{
  struct rtp_media_live_t *ml = calloc(1, sizeof(struct rtp_media_live_t));
 
  ml->ch = ch;
  ml->st = st;
  ml->base.get_sdp        = rtp_live_get_sdp;      
  ml->base.get_duration   = rtp_live_get_duration;
  ml->base.play           = rtp_live_play;
  ml->base.pause          = rtp_live_pause;        
  ml->base.set_speed      = rtp_live_set_speed;
  ml->base.seek           = rtp_live_seek;   
  ml->base.get_rtpinfo    = rtp_live_get_rtpinfo;
  ml->base.add_transport  = rtp_live_add_transport;
  
  // private;
  
  return (struct rtp_media_t*)ml;
}

int rtp_media_live_free(struct rtp_media_t* m)
{
  int i = 0;
  struct rtp_media_live_t *ml = (struct rtp_media_live_t*)m;
  if(ml)
  {
    if(ml->tid && !ml->exited) //check tid is valid
    {
      // stop thread;
      st_thread_interrupt(ml->tid);
      ml->loop = 0;
      while(!ml->exited)
        st_usleep(10*1000);
    }
    
    for(i = 0; i < MEDIA_TRACK_BUTT; i++)
    {
      if(ml->track[i].m_transport)
      {
        ml->track[i].m_transport->free(ml->track[i].m_transport);
      }
      if(ml->track[i].m_rtppacker)
      {
        rtp_payload_encode_destroy(ml->track[i].m_rtppacker);
      }
      if(ml->track[i].m_rtp)
      {
        rtp_destroy(ml->track[i].m_rtp);
      }
      if(ml->track[i].m_reader)
      {
        cfifo_free(ml->track[i].m_reader);
      }
    }
    printf("%s => free m:%p\n", __func__, ml);
    free(ml); 
  }
  
  return 0;
}
