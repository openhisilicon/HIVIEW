#include "rtp-media.h"
#include "rtp.h"
#include "time64.h"

#include "mpeg-ps.h"
#include "mpeg-ts.h"
#include "mpeg-ts-proto.h"

#include "inc/gsf.h"
#include "mod/codec/inc/codec.h"

#include "inc/frm.h"

#include <stddef.h> // container_of
#define CONTAINER_OF(ptr, struct_type, member) \
        (struct_type*)((char*)(ptr) - offsetof(struct_type, member))

#define _AUDIO_ENABLE_   0
#define RTP_PAYLOAD_H264 98  // H264;
#define RTP_PAYLOAD_PS   96  // PS;
        
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
  unsigned int timestamp; // current frame ts;
  
  struct ps_muxer_t* ps; // ps encoder
  int psvid, psaid, audio_shmid;
  unsigned char ps_packet[GSF_FRM_MAX_SIZE];
  
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
		
		printf("SendRTCP: track ts:%d\n", t->timestamp);
	}
	
	return 0;
}


static void *live_send_task(void *arg)
{
  int i = 0, ret = 0;
  st_netfd_t m_evfd = NULL;
  unsigned char* ptr = malloc(GSF_FRM_MAX_SIZE);
  struct rtp_media_live_t* m = (struct rtp_media_live_t*)arg;
  
  struct cfifo_ex* a_reader = NULL;
  #if _AUDIO_ENABLE_
  a_reader = cfifo_shmat(cfifo_recsize, cfifo_rectag, m->track[MEDIA_TRACK_VIDEO].audio_shmid);
  #endif

  if(m->track[MEDIA_TRACK_VIDEO].m_transport)
  {
    int r = m->track[MEDIA_TRACK_VIDEO].m_transport->conn(m->track[MEDIA_TRACK_VIDEO].m_transport);
    printf("MEDIA_TRACK_VIDEO conn r:%d\n", r);
    if(r < 0)
      goto __exit;
  }
  
  if(m->track[MEDIA_TRACK_VIDEO].m_reader)
  {
    cfifo_newest(m->track[MEDIA_TRACK_VIDEO].m_reader, 0);
    if(a_reader)
      cfifo_newest(a_reader, 0);
    
    GSF_MSG_DEF(char, msgdata, sizeof(gsf_msg_t));
    GSF_MSG_SENDTO(GSF_ID_CODEC_IDR, m->ch, SET, m->st
                    , 0
                    , GSF_IPC_CODEC
                    , 2000);
    m->track[MEDIA_TRACK_VIDEO].timestamp = 0;
  }
  
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
        #if defined(GSF_CPU_3519d)
        int ret = st_netfd_poll(m_evfd, POLLIN, 1000*1000);
        cfifo_fd_resume(m->track[MEDIA_TRACK_VIDEO].m_evfd);
        #else
        int ret = st_netfd_poll(m_evfd, POLLIN|POLLET, 1000*1000);
        #endif
        //gettimeofday(&tv2, NULL);
        //if(++m->track[MEDIA_TRACK_VIDEO].pollcnt%30 == 0)
        //  printf("pid:%d, m:%p => st_netfd_poll ret:%d, cost:%d\n", getpid(), m, ret, (tv2.tv_sec*1000+tv2.tv_usec/1000)-(tv1.tv_sec*1000+tv1.tv_usec/1000));
      }
      // get data;
      if(m->track[MEDIA_TRACK_VIDEO].m_reader)
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
          {
            if(a_reader)
              cfifo_newest(a_reader, 0);
            continue;
          }
        }
        //gettimeofday(&tv2, NULL);
        //u_int32_t cost = (tv2.tv_sec*1000+tv2.tv_usec/1000)-(tv1.tv_sec*1000+tv1.tv_usec/1000);
        //if( cost > 30)
        //  printf("%u => warning run cfifo_get cost:%d\n", ptr, cost);
        
        m->track[MEDIA_TRACK_VIDEO].sendcnt = 0;
        m->track[MEDIA_TRACK_VIDEO].timestamp = rec->pts - m->track[MEDIA_TRACK_VIDEO].m_rtp_clock;
        
        if(m->track[MEDIA_TRACK_VIDEO].ps)
        {
          ps_muxer_input(m->track[MEDIA_TRACK_VIDEO].ps, m->track[MEDIA_TRACK_VIDEO].psvid
                        , ((rec->flag & GSF_FRM_FLAG_IDR)?0x0001:0x0000) | 0x8000 //H264_H265_WITH_AUD
                        , m->track[MEDIA_TRACK_VIDEO].timestamp * 90, m->track[MEDIA_TRACK_VIDEO].timestamp * 90
                        , rec->data, rec->size);            
          //printf("ps_muxer_input: VVV ts:%d, size:%d\n", m->track[MEDIA_TRACK_VIDEO].timestamp, rec->size);
        }
        else
        {
          //printf("send MEDIA_TRACK_VIDEO idr:%d, timestamp:%u, size:%d\n", rec->flag & GSF_FRM_FLAG_IDR, m->track[MEDIA_TRACK_VIDEO].timestamp, rec->size);
          rtp_payload_encode_input(m->track[MEDIA_TRACK_VIDEO].m_rtppacker, rec->data, rec->size, m->track[MEDIA_TRACK_VIDEO].timestamp * 90 /*kHz*/);
        }
        
        // SendRTP() used st_writev();
        // SendRTCP(&m->track[MEDIA_TRACK_VIDEO]);
        
      }while(1);
      
      //a_reader
      if(a_reader)
      do{
        gsf_frm_t *rec = (gsf_frm_t*)ptr;
        int ret = cfifo_get(a_reader, cfifo_recgut, (unsigned char*)ptr);
        if(ret <= 0)
          break;
        if(m->track[MEDIA_TRACK_VIDEO].ps)
        {
          ps_muxer_input(m->track[MEDIA_TRACK_VIDEO].ps, m->track[MEDIA_TRACK_VIDEO].psaid
                        , 0x00
                        , m->track[MEDIA_TRACK_VIDEO].timestamp * 90, m->track[MEDIA_TRACK_VIDEO].timestamp * 90
                        , rec->data + 4, rec->size - 4);
         //printf("ps_muxer_input: AAA ts:%d, size:%d\n", m->track[MEDIA_TRACK_VIDEO].timestamp, rec->size - 4);
        }
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
  
__exit:
  free(ptr);
  if(a_reader)
    cfifo_free(a_reader);
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


static void* ps_alloc(void* param, size_t bytes)
{
	struct media_track_t *t = (struct media_track_t*)param;
	assert(bytes <= sizeof(t->ps_packet));
	return t->ps_packet;
}

static void ps_free(void* param, void* packet)
{
    return;
}

static int ps_write(void* param, int stream, void* packet, size_t bytes)
{
    //return 1 == fwrite(packet, bytes, 1, (FILE*)param) ? 0 : ferror((FILE*)param);
    
    struct media_track_t *t = (struct media_track_t*)param;
    
    return rtp_payload_encode_input(t->m_rtppacker, packet, bytes, t->timestamp * 90 /*kHz*/);
}





static int rtp_live_get_sdp(struct rtp_media_t* _m, char *sdp, int fmt, int ssrc)
{
  char media[1024] = {0};
  struct rtp_media_live_t *m = (struct rtp_media_live_t*)_m;
  
	static const char* pattern_audio =
  	//"m=audio 9002 RTP/AVP %d\r\n"
  	//"a=rtpmap:%d PCMU/%d/%d\r\n"
    "m=audio 9002 RTP/AVP 0 0\r\n"
    //"a=rtpmap:0 telephone-event/8000\r\n" 	
    //"a=range:npt=now-\n"
    //"a=recvonly\n"
  	//"a=control:audio\n"
  	;
  snprintf(media, sizeof(media), pattern_audio, RTP_PAYLOAD_PCMU, RTP_PAYLOAD_PCMU, 8000, 1);
  if(sdp)
    strcat(sdp, media);
  
  static const char* pattern_video =
    "m=video 9000 RTP/AVP %d\r\n"
    "a=rtpmap:%d %s/90000\r\n"
    //"a=range:npt=now-\n"
    //"a=recvonly\n"
    //"a=control:video\n"
    //"a=fmtp:%d profile-level-id=%02X%02X%02X;"
    //"packetization-mode=1;"
    //"sprop-parameter-sets=\r\n"
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
  
  //extern uint32_t rtp_ssrc(void);
  //uint32_t ssrc = rtp_ssrc();
  
  //void rtp_packet_setsize(int bytes);
  //rtp_packet_setsize(1440);

  if(!fmt)
  {
    if(gsf_sdp->venc.type == GSF_ENC_H265)
      m->track[MEDIA_TRACK_VIDEO].m_rtppacker = 
          rtp_payload_encode_create(RTP_PAYLOAD_H265, "H265", (uint16_t)ssrc, ssrc, &s_rtpfunc, &m->track[MEDIA_TRACK_VIDEO]);
    else
      m->track[MEDIA_TRACK_VIDEO].m_rtppacker = 
          rtp_payload_encode_create(RTP_PAYLOAD_H264, "H264", (uint16_t)ssrc, ssrc, &s_rtpfunc, &m->track[MEDIA_TRACK_VIDEO]);
  }
  else
  {
    struct ps_muxer_func_t handler;
    handler.alloc = ps_alloc;
    handler.write = ps_write;
    handler.free = ps_free;
    m->track[MEDIA_TRACK_VIDEO].ps = ps_muxer_create(&handler, &m->track[MEDIA_TRACK_VIDEO]);
    m->track[MEDIA_TRACK_VIDEO].psvid = ps_muxer_add_stream(m->track[MEDIA_TRACK_VIDEO].ps, PSI_STREAM_H264, NULL, 0);
    #if _AUDIO_ENABLE_
    m->track[MEDIA_TRACK_VIDEO].psaid = ps_muxer_add_stream(m->track[MEDIA_TRACK_VIDEO].ps, PSI_STREAM_AUDIO_G711A, NULL, 0);
    #endif
    m->track[MEDIA_TRACK_VIDEO].m_rtppacker = 
          rtp_payload_encode_create(RTP_PAYLOAD_PS, "MP2P", 1/*(uint16_t)ssrc*/, ssrc, &s_rtpfunc, &m->track[MEDIA_TRACK_VIDEO]);
  }
  
	struct rtp_event_t event;
	event.on_rtcp = NULL;
	m->track[MEDIA_TRACK_VIDEO].m_rtp = rtp_create(&event, NULL, ssrc, ssrc, 90000, 4*1024, 1);
	
	if(gsf_sdp->venc.type == GSF_ENC_H265)
	  rtp_set_info(m->track[MEDIA_TRACK_VIDEO].m_rtp, "RTSPServer", "live.h265");
	else
	  rtp_set_info(m->track[MEDIA_TRACK_VIDEO].m_rtp, "RTSPServer", "live.h264");
  
  m->track[MEDIA_TRACK_VIDEO].m_reader = cfifo_shmat(cfifo_recsize, cfifo_rectag, gsf_sdp->video_shmid);
  m->track[MEDIA_TRACK_VIDEO].m_evfd   = cfifo_take_fd(m->track[MEDIA_TRACK_VIDEO].m_reader);
  m->track[MEDIA_TRACK_VIDEO].audio_shmid = gsf_sdp->audio_shmid;
  
  printf("%s => get SDP ok. ch:%d, st:%d, video_shmid:%d, m_reader:%p, m_evfd:%d\n" 
        , __func__
        , m->ch, m->st, gsf_sdp->video_shmid
        , m->track[MEDIA_TRACK_VIDEO].m_reader
        , m->track[MEDIA_TRACK_VIDEO].m_evfd);
        
  if(m->track[MEDIA_TRACK_VIDEO].m_reader == NULL 
    || m->track[MEDIA_TRACK_VIDEO].m_evfd < 0)
  {
    return 0;
  }
  
  if(!fmt)
  {
    if(gsf_sdp->venc.type == GSF_ENC_H265)
      snprintf(media, sizeof(media), pattern_video, RTP_PAYLOAD_H265, RTP_PAYLOAD_H265, "H265",
              RTP_PAYLOAD_H265, 0,0,0);
    else
      snprintf(media, sizeof(media), pattern_video, RTP_PAYLOAD_H264, RTP_PAYLOAD_H264, "H264",
              RTP_PAYLOAD_H264, 0,0,0);
  }
  else
  {
    snprintf(media, sizeof(media), pattern_video, RTP_PAYLOAD_PS, RTP_PAYLOAD_PS, "PS",
            RTP_PAYLOAD_PS, 0,0,0);
  }
  if(sdp)          
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

      if(ml->track[i].ps)
      {
        ps_muxer_destroy(ml->track[i].ps);
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
