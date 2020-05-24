#include "rtp-media.h"
#include "rtp.h"
#include "time64.h"


#include "inc/gsf.h"
#include "mod/codec/inc/codec.h"


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
  int pollcnt;
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



static unsigned int cfifo_recsize(unsigned char *p1, unsigned int n1, unsigned char *p2)
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

static unsigned int cfifo_rectag(unsigned char *p1, unsigned int n1, unsigned char *p2)
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

static unsigned int cfifo_recgut(unsigned char *p1, unsigned int n1, unsigned char *p2, void *u)
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
    if(cost > 30*2)
      printf("u:%p, get rec ok [delay:%d ms].\n", u, cost);
      
    assert(rec->data[0] == 00 && rec->data[1] == 00 && rec->data[2] == 00 && rec->data[3] == 01);

    return len;
}



static void *live_send_task(void *arg)
{
  int i = 0, ret = 0;
  unsigned char* ptr = malloc(512*1024);
  uint32_t timestamp = 0;
  struct rtp_media_live_t* m = (struct rtp_media_live_t*)arg;
  
  if(m->track[MEDIA_TRACK_VIDEO].m_reader)
  {
    #if 1
    cfifo_newest(m->track[MEDIA_TRACK_VIDEO].m_reader, 0);
    GSF_MSG_DEF(char, msgdata, sizeof(gsf_msg_t));
    GSF_MSG_SENDTO(GSF_ID_CODEC_IDR, 0, SET, m->st
                    , 0
                    , GSF_IPC_CODEC
                    , 2000);
    #else
    cfifo_newest(m->track[MEDIA_TRACK_VIDEO].m_reader, 1);
    #endif
  }
  
  st_netfd_t m_evfd = NULL;
  
  if(m->track[MEDIA_TRACK_VIDEO].m_evfd > 0)
  {
    m_evfd = st_netfd_open(m->track[MEDIA_TRACK_VIDEO].m_evfd);
  }
  
  while(m->loop)
  {
    //get stream;
    if(m->track[MEDIA_TRACK_VIDEO].m_transport)
    {
      if(i%60 == 0)
      {
        char rr[1024] = {0};
        int r = m->track[MEDIA_TRACK_VIDEO].m_transport->recv(m->track[MEDIA_TRACK_VIDEO].m_transport, 1, rr, sizeof(rr));
      }
      
      struct timeval tv1, tv2;
      
      // wait data;
      if(m_evfd)
      {
        //gettimeofday(&tv1, NULL);
        int ret = st_netfd_poll(m_evfd, POLLIN|POLLET, 1000*1000);
        //gettimeofday(&tv2, NULL);
        //if(m->track[MEDIA_TRACK_VIDEO].pollcnt++%30 == 0)
        //  printf("pid:%d, m:%p => st_netfd_poll ret:%d, cost:%d\n", getpid(), m, ret, (tv2.tv_sec*1000+tv2.tv_usec/1000)-(tv1.tv_sec*1000+tv1.tv_usec/1000));
      }
      // get data;
      do{	
        //gettimeofday(&tv1, NULL);
      
        gsf_frm_t *rec = (gsf_frm_t*)ptr;
        int ret = cfifo_get(m->track[MEDIA_TRACK_VIDEO].m_reader, cfifo_recgut, (unsigned char*)ptr);
        if(ret <= 0)
          break;
        if(m->track[MEDIA_TRACK_VIDEO].m_rtp_clock == 0)
        {
          if(rec->flag & GSF_FRM_FLAG_IDR)
            m->track[MEDIA_TRACK_VIDEO].m_rtp_clock = rec->pts;
          else
            continue;
        }
        //gettimeofday(&tv2, NULL);
        //u_int32_t cost = (tv2.tv_sec*1000+tv2.tv_usec/1000)-(tv1.tv_sec*1000+tv1.tv_usec/1000);
        //if( cost > 30)
        //  printf("%u => warning run cfifo_get cost:%d\n", ptr, cost);
        
        m->track[MEDIA_TRACK_VIDEO].sendcnt = 0;
        timestamp = rec->pts - m->track[MEDIA_TRACK_VIDEO].m_rtp_clock;
        //printf("send MEDIA_TRACK_VIDEO idr:%d, timestamp:%u, size:%d\n", rec->flag & GSF_FRM_FLAG_IDR, timestamp, rec->size);
        rtp_payload_encode_input(m->track[MEDIA_TRACK_VIDEO].m_rtppacker, rec->data, rec->size, timestamp * 90 /*kHz*/);
        
        // SendRTP() used st_writev();
        // SendRTCP(&m->track[MEDIA_TRACK_VIDEO]);
      }while(1);
    }
    
    //get stream;
    if(m->track[MEDIA_TRACK_AUDIO].m_transport)
    {
      if(i%60 == 0)
      {
        char rr[1024] = {0};
        int r = m->track[MEDIA_TRACK_AUDIO].m_transport->recv(m->track[MEDIA_TRACK_AUDIO].m_transport, 1, rr, sizeof(rr));
      }
      // get data;
      // send data;  
      //printf("send MEDIA_TRACK_AUDIO.\n");
    }

    //get stream;
    if(m->track[MEDIA_TRACK_META].m_transport)
    {
      if(i%60 == 0)
      {
        char rr[1024] = {0};
        int r = m->track[MEDIA_TRACK_META].m_transport->recv(m->track[MEDIA_TRACK_META].m_transport, 1, rr, sizeof(rr));
      }
      
      // get data;
      // send data;  
      printf("send MEDIA_TRACK_META.\n");
    }
    
    i++;
    //st_usleep(5*1000);
  }
  free(ptr);
  m->exited = 1;
  if(m_evfd)
  {
    st_netfd_poll(m_evfd, POLLIN, 1);
    st_netfd_close(m_evfd);
  }
  printf("%s => exit m:%p .\n", __func__, m);
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
	
	int r = t->m_transport->send(t->m_transport, 0, packet, bytes);
	// r <= 0 when peer close; assert(r == (int)bytes);
	
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
    "m=video 0 RTP/AVP %d\n"
    "a=rtpmap:%d H264/90000\n"
    "a=range:npt=now-\n"
    "a=recvonly\n"
    "a=control:video\n"
    "a=fmtp:%d profile-level-id=%02X%02X%02X;"
    "packetization-mode=1;"
    "sprop-parameter-sets=";

  struct rtp_payload_t s_rtpfunc = {
    RTPAlloc,
    RTPFree,
    RTPPacket,
  };
  
  extern uint32_t rtp_ssrc(void);
  uint32_t ssrc = rtp_ssrc();
  m->track[MEDIA_TRACK_VIDEO].m_rtppacker = 
      rtp_payload_encode_create(RTP_PAYLOAD_H264, "H264", (uint16_t)ssrc, ssrc, &s_rtpfunc, &m->track[MEDIA_TRACK_VIDEO]);

	struct rtp_event_t event;
	event.on_rtcp = NULL;
	m->track[MEDIA_TRACK_VIDEO].m_rtp = rtp_create(&event, NULL, ssrc, ssrc, 90000, 4*1024, 1);
	rtp_set_info(m->track[MEDIA_TRACK_VIDEO].m_rtp, "RTSPServer", "live.h264");

  GSF_MSG_DEF(gsf_sdp_t, gsf_sdp, sizeof(gsf_msg_t)+sizeof(gsf_sdp_t));
  gsf_sdp->video_shmid = -1;
  gsf_sdp->audio_shmid = -1;
  if(GSF_MSG_SENDTO(GSF_ID_CODEC_SDP, 0, GET, m->st
                        , 0
                        , GSF_IPC_CODEC
                        , 2000) < 0)
  {
    printf("%s => get SDP err.\n" ,__func__);
    return 0;
  }
  
  m->track[MEDIA_TRACK_VIDEO].m_reader = cfifo_shmat(cfifo_recsize, cfifo_rectag, gsf_sdp->video_shmid);
  m->track[MEDIA_TRACK_VIDEO].m_evfd   = cfifo_take_fd(m->track[MEDIA_TRACK_VIDEO].m_reader);
  
  printf("%s => get SDP ok. st:%d, video_shmid:%d, m_reader:%p, m_evfd:%d\n" 
        , __func__
        , m->st, gsf_sdp->video_shmid
        , m->track[MEDIA_TRACK_VIDEO].m_reader
        , m->track[MEDIA_TRACK_VIDEO].m_evfd);
        
  if(m->track[MEDIA_TRACK_VIDEO].m_reader == NULL 
    || m->track[MEDIA_TRACK_VIDEO].m_evfd < 0)
  {
    return 0;
  }
  
  char media[1024] = {0};
  snprintf(media, sizeof(media), pattern_video, RTP_PAYLOAD_H264, RTP_PAYLOAD_H264,
          RTP_PAYLOAD_H264, 0,0,0);
  //media += parameters;
  //media += '\n';
  strcat(media, "\n");
  strcat(sdp, media);
  
  
	static const char* pattern_audio =
  	"m=audio 0 RTP/AVP %d\n"
  	"a=rtpmap:%d PCMU/%d/%d\n"
    "a=range:npt=now-\n"
    "a=recvonly\n"
  	"a=control:audio\n";
  	
  snprintf(media, sizeof(media), pattern_audio, RTP_PAYLOAD_PCMU, RTP_PAYLOAD_PCMU, 8000, 1);
  strcat(sdp, media);

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
    if(ml->tid)
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
