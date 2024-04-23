#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <string.h>
#include <sys/time.h>
#include <assert.h>
#include <errno.h>
#include <syslog.h>
#include <signal.h>
#include <stdarg.h>

#include "inc/gsf.h"  // "fw/comm/inc/list.h" define LIST_HEAD;
#include "fw/comm/inc/proc.h"
#include "fw/libflv/inc/flv-muxer.h"
#include "mongoose.h" // src/mongoose.h:2644:0: warning: "LIST_HEAD" redefined

#include "mod_call.h"
#include "gsf_urldec.h"

#include "cfg.h"
#include "webrtc.h"
#include "http.h"

#include "inc/frm.h"


#define FLV_TYPE_AUDIO 8
#define FLV_TYPE_VDIEO 9
#define FLV_TYPE_SCRIPT 18

typedef struct {
  char flag[32];  //session flag;
  struct cfifo_ex* video;
  struct cfifo_ex* audio;  
  unsigned char* data;
  flv_muxer_t* flv;
  unsigned int vpts, apts;
  int idr, chn, sid;
  struct mg_mgr *mgr;
  
  pthread_t thread_id;
  int thread_exit;
  
  int wsbuf_len[2];
  unsigned char *wsbuf[2];
  unsigned int wr, rd;
}lws_session_t;


static pthread_t gpid;
static int session_cnt = 0;
static sig_atomic_t s_received_signal = 0;

#if 1

static void on_flv_send(struct mg_connection *nc, int ev, void *p) {
  (void) ev;
  
  if(!p)
  {
    printf("%s => p == NULL.\n", __func__);
    return;
  }
  
  lws_session_t *sess = *((lws_session_t**)p);

  //printf("%s => >>>>>>>>nc:%p, user_data:%p, sess:%p\n", __func__, nc, nc->user_data,sess);
  
  struct mg_connection *c = nc;
  if(1)//for (c = mg_next(nc->mgr, NULL); c != NULL; c = mg_next(nc->mgr, c)) 
  {
    
    #ifdef GSF_CPU_3516e
    #define SBUF_MAX_SIZE 0.5*1024*1024 
    #else
    #define SBUF_MAX_SIZE 2*1024*1024
    #endif
    if (c->user_data != NULL) {
      if (c->user_data == sess) {
        //printf("%s => sess:%p, rd:%u, len:%d\n", __func__, sess, sess->rd, sess->wsbuf_len[(sess->rd)%2]);
        if (sess->wsbuf_len[(sess->rd)%2] == 0 || c->send_mbuf.len >= SBUF_MAX_SIZE ) {
          // EOF is received from stdin. Schedule the connection to close
          c->flags |= MG_F_SEND_AND_CLOSE;
          if (c->send_mbuf.len <= 0) {
            c->flags |= MG_F_CLOSE_IMMEDIATELY;
          }
        } else if (sess->wsbuf[(sess->rd)%2]){
          // A character is received from stdin. Send it to the connection.
          unsigned char *data = sess->wsbuf[(sess->rd)%2];
          mg_send_websocket_frame(c, WEBSOCKET_OP_BINARY, data, sess->wsbuf_len[(sess->rd)%2]);
        }
        sess->rd++;
      }
    }
    
  }
}

static void be_write_uint32(uint8_t* ptr, uint32_t val)
{
	ptr[0] = (uint8_t)((val >> 24) & 0xFF);
	ptr[1] = (uint8_t)((val >> 16) & 0xFF);
	ptr[2] = (uint8_t)((val >> 8) & 0xFF);
	ptr[3] = (uint8_t)(val & 0xFF);
}
static int flv_write_tag(uint8_t* tag, uint8_t type, uint32_t bytes, uint32_t timestamp)
{
    // TagType
	tag[0] = type & 0x1F;

	// DataSize
	tag[1] = (bytes >> 16) & 0xFF;
	tag[2] = (bytes >> 8) & 0xFF;
	tag[3] = bytes & 0xFF;

	// Timestamp
	tag[4] = (timestamp >> 16) & 0xFF;
	tag[5] = (timestamp >> 8) & 0xFF;
	tag[6] = (timestamp >> 0) & 0xFF;
	tag[7] = (timestamp >> 24) & 0xFF; // Timestamp Extended

	// StreamID(Always 0)
	tag[8] = 0;
	tag[9] = 0;
	tag[10] = 0;

	return 11;
}


static int flv_send_header(lws_session_t *sess)
{
  struct mg_mgr *mgr = sess->mgr;
  int len = 0;

  unsigned char *raw = sess->wsbuf[sess->wr%2];
  
	raw[0] = 'F'; // FLV signature
	raw[1] = 'L';
	raw[2] = 'V';
	raw[3] = 0x01; // File version
	raw[4] = 0x01 | 0x04; // Type flags (audio:0x04 & video:0x01)
	be_write_uint32(raw + 5, 9); // Data offset
	be_write_uint32(raw + 9, 0); // PreviousTagSize0(Always 0)
  len += (9+4);
  
  //printf("%s => sess:%p, wr:%u, len:%d\n", __func__, sess, sess->wr, len);

  sess->wsbuf_len[sess->wr%2] = len;
  //assert(sess->wr - sess->rd < 2);
  sess->wr++;
  mg_broadcast(mgr, on_flv_send, &sess, sizeof(sess)); 
  
  
  return 0;
}

static int flv_send_tag(lws_session_t *sess, int type, const void* data, size_t bytes, uint32_t timestamp)
{
  struct mg_mgr *mgr = sess->mgr;
  int len = 0;

  unsigned char *raw = sess->wsbuf[sess->wr%2];
  
  flv_write_tag(raw+len, (uint8_t)type, (uint32_t)bytes, timestamp);
  len += 11;
  memcpy(raw+len, data, bytes);
  len += bytes;
  be_write_uint32(raw+len, (uint32_t)bytes + 11);
  len += 4;
  
  //printf("%s => sess:%p, wr:%u, len:%d\n", __func__, sess, sess->wr, len);
  
  sess->wsbuf_len[sess->wr%2] = len;
  //assert(sess->wr - sess->rd < 2);
  sess->wr++;
  mg_broadcast(mgr, on_flv_send, &sess, sizeof(sess)); 
  
  
  return 0;
}

static int on_flv_packet(void* sess, int type, const void* data, size_t bytes, uint32_t timestamp)
{
  #if 0
  char *_data = (char*)data;
	printf("FLV type:%d, bytes:%d [%02X,%02X,%02X,%02X,%02X,%02X]\n"
	      , type, bytes, _data[0], _data[1], _data[2], _data[3], _data[4], _data[5]);
	#endif
	return flv_send_tag(sess, type, data, bytes, timestamp);
}

#endif



static void *send_thread_func(void *param) {
  
  int i = 0;
  lws_session_t *sess = (lws_session_t*) param;
  struct mg_mgr *mgr = sess->mgr;
  pthread_detach(pthread_self());

  int ep = cfifo_ep_alloc(1);
  
  if(sess->video)
  {
    cfifo_ep_ctl(ep, CFIFO_EP_ADD, sess->video);
	  cfifo_newest(sess->video, 0);
  }
  if(sess->audio)
  {
  	cfifo_ep_ctl(ep, CFIFO_EP_ADD, sess->audio);
  	cfifo_newest(sess->audio, 0);
  }
  #if 1
  GSF_MSG_DEF(char, msgdata, sizeof(gsf_msg_t));
  GSF_MSG_SENDTO(GSF_ID_CODEC_IDR, sess->chn, SET, sess->sid
                  , 0
                  , GSF_IPC_CODEC
                  , 2000);
  #endif
  
  while(!sess->thread_exit)
  {
    
    int cnt = 0;
    struct cfifo_ex* result[255];
    cnt = cfifo_ep_wait(ep, 1000, result, 255);
    if(cnt <= 0)
    {
       printf("cfifo_ep_wait err cnt:%d\n", cnt);
       continue;
    }

    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    //printf("%s => cfifo_ep_wait ts:%llu ms\n", __func__, (ts.tv_sec*1000 + ts.tv_nsec/1000000));
    
    for(i = 0; i < cnt; i++)
    {
      struct cfifo_ex* fifo = result[i];
      
      while(!sess->thread_exit)
      {
        gsf_frm_t *rec = (gsf_frm_t *)sess->data;
        int ret = cfifo_get(fifo, cfifo_recgut, (unsigned char*)sess->data);
        sess->idr = (sess->idr)?:(rec->flag&GSF_FRM_FLAG_IDR);
               
        if(ret < 0)
        {
            printf("cfifo err ret:%d\n", ret);
            break;
        }
        else if (ret == 0)
        {
          //printf("cfifo empty ret:%d\n", ret);
          break;
        }
        else if (ret > 0 && sess->idr)
        {
          //printf("cfifo frame ret:%d\n", ret);
          if(rec->type == GSF_FRM_VIDEO /*&& rec->video.encode == GSF_ENC_H264*/)
          {
            if(!sess->vpts)
            {
              printf("V rec->type:%d, seq:%d, utc:%u, size:%d\n", rec->type, rec->seq, rec->utc, rec->size);
              flv_send_header(sess);
              sess->vpts = rec->pts;
            }
            
            //struct timespec ts1, ts2;  
            //clock_gettime(CLOCK_MONOTONIC, &ts1);
            
            if(rec->video.encode == GSF_ENC_H264)
            {
              if(flv_muxer_avc(sess->flv // sps-pps-vcl
                      , rec->data
                      , rec->size
                      , rec->pts - sess->vpts
                      , rec->pts - sess->vpts) < 0)
              {
                printf("flv_muxer_avc err.\n");
              }
            }
            else 
            {
              // https://github.com/ksvc/FFmpeg/wiki
              // https://ott.dolby.com/codec_test/index.html
              if(flv_muxer_hevc(sess->flv // sps-pps-vcl
                      , rec->data
                      , rec->size
                      , rec->pts - sess->vpts
                      , rec->pts - sess->vpts) < 0)
              {
                printf("flv_muxer_hevc err.\n");
              }
            }
             
            //clock_gettime(CLOCK_MONOTONIC, &ts2);
            //int cost = (ts2.tv_sec*1000 + ts2.tv_nsec/1000000) - (ts1.tv_sec*1000 + ts1.tv_nsec/1000000);
            //if(cost > 6)
            //{ 
            //  printf("flv_muxer_avc size:%d, cost:%d ms\n", rec->size, cost);
            //}
          }
          else if(rec->type == GSF_FRM_AUDIO && rec->audio.encode == GSF_ENC_AAC)
          {
            if(!sess->vpts)
              continue;
            
            if(!sess->apts)
            {
              printf("A rec->type:%d, seq:%d, utc:%u, size:%d\n", rec->type, rec->seq, rec->utc, rec->size);
              sess->apts = rec->pts;
            }
            
            //struct timespec ts1, ts2;  
            //clock_gettime(CLOCK_MONOTONIC, &ts1);
            
            
            if(flv_muxer_aac(sess->flv // sps-pps-vcl
                    , rec->data
                    , rec->size
                    , rec->pts - sess->apts
                    , rec->pts - sess->apts) < 0)
            {
              printf("flv_muxer_aac err.\n");
            }
            
            //clock_gettime(CLOCK_MONOTONIC, &ts2);
            //int cost = (ts2.tv_sec*1000 + ts2.tv_nsec/1000000) - (ts1.tv_sec*1000 + ts1.tv_nsec/1000000);
            //if(cost > 3)
            //{ 
            //  printf("flv_muxer_aac size:%d, cost:%d ms\n", rec->size, cost);
            //}
            
          }
        }
      }
    }

  }
  
  cfifo_ep_free(ep);
  
  if(sess)
  {
    if(sess->data)
    {
      printf("sess:%p, free data:%p\n", sess, sess->data);
      #ifdef __DMA_COPY__
      venc_viraddr_free(sess->data);
      #else
      free(sess->data);
      #endif
    }    
    for(i = 0; i < 2; i++)
    { 
      if(sess->wsbuf[i])
      {
        free(sess->wsbuf[i]);
        sess->wsbuf_len[i] = 0;
        printf("sess:%p, free wsbuf:%p\n", sess, sess->wsbuf[i]);
      }
    }
    if(sess->video)
    {
      printf("close video:%p \n", sess->video);
      session_cnt--;
      cfifo_free(sess->video);
    }
    
    if(sess->audio)
    {
      printf("close audio:%p \n", sess->audio);
      cfifo_free(sess->audio);
    }
    
    if(sess->flv)
    {
       printf("flv_muxer_destroy:%p \n", sess->flv);
      flv_muxer_destroy(sess->flv);
    }
    free(sess);
  }
  
  printf("%s => exit.\n", __func__);
  return NULL;
}

struct mg_serve_http_opts s_http_server_opts;

int webrtc_proc(struct mg_connection *nc, rtc_sess_t **rtc_sess, struct websocket_message *wm)
{
  int ret = 0;
  
  if(!strncmp(wm->data, "{\"type\":\"hello", strlen("{\"type\":\"hello")))
  {
    char sdp_json[4096] = {0};
    unsigned int sdp_json_len = sizeof(sdp_json);
    memcpy(sdp_json, wm->data, wm->size);
    
    int new_flag = 0;
    
    // new session;
    if(!(*rtc_sess))
    {
      *rtc_sess = rtc_sess_new();
      if(*rtc_sess)
      {
        (*rtc_sess)->mgr = nc->mgr;
        char *dst = strstr(sdp_json, ",\"src\":\"");
        sscanf(dst, ",\"src\":\"%127[^\"]", (*rtc_sess)->dst);
        warn("rtc_sess_new nc:%p, (*rtc_sess):%p, src:[%s]\n", nc, *rtc_sess, (*rtc_sess)->dst);
        ret = WEBRTC_NEW;
      }
    }
    
    
    
    if(!(*rtc_sess))
    {
      char dst[128] = {0};
      char *pdst = strstr(sdp_json, ",\"src\":\"");
      sscanf(pdst, ",\"src\":\"%127[^\"]", dst);
      
      sprintf(sdp_json, "{\"type\":\"close\",\"dst\":\"%s\",\"msg\":\"%s\"}"
            , dst, "conncet too much.");
      mg_send_websocket_frame(nc, WEBSOCKET_OP_TEXT, sdp_json, strlen(sdp_json));
      return 0;
    }
    
    sdp_json[0] = '\0';
    rtc_createOffer((*rtc_sess), sdp_json, sdp_json_len);
    
    //dst = src;
    sdp_json[strlen(sdp_json)-1] = ',';
    char dst_token[128]; sprintf(dst_token,"\"dst\":\"%s\"}", (*rtc_sess)->dst);
    strcat(sdp_json, dst_token);
    
    printf("send createOffer:[%s]\n\n", sdp_json);
    mg_send_websocket_frame(nc, WEBSOCKET_OP_TEXT, sdp_json, strlen(sdp_json));
    
    return ret;
  }
  else if(!strncmp(wm->data, "{\"type\":\"leave", strlen("{\"type\":\"leave")))
  {
    printf("LEAVE [%.*s].\n", wm->size, wm->data);
    return ret = WEBRTC_DEL;
  }
  else if(!strncmp(wm->data, "{\"type\":\"answer", strlen("{\"type\":\"answer")))
  {
    char sdp_json[4096] = {0};
    
    memcpy(sdp_json, wm->data, wm->size);
    printf("recv createAnswe:[%s]\n\n", sdp_json);

    rtc_setAnswer((*rtc_sess), sdp_json);
  }
  else if(!strncmp(wm->data, "{\"candidate\":\"candidate", strlen("{\"candidate\":\"candidate")))
  {
    do
    {
      char *local_ice = rtc_getCandidate((*rtc_sess));
      if(!local_ice)
        break;
      {
        //dst = src;
        local_ice[strlen(local_ice)-1] = ',';
        char dst_token[128]; sprintf(dst_token,"\"dst\":\"%s\"}", (*rtc_sess)->dst);
        strcat(local_ice, dst_token);
        
        warn("send candidate:[%s]\n\n", local_ice);
        mg_send_websocket_frame(nc, WEBSOCKET_OP_TEXT, local_ice, strlen(local_ice));
      }
    }while(1);
   
    char ice_json[4096] = {0};
    memcpy(ice_json, wm->data, wm->size);
    printf("recv candidate:[%s]\n\n", ice_json);
    rtc_setCandidate((*rtc_sess), ice_json);
  }
  
  return ret;
}



static void ev_handler(struct mg_connection *nc, int ev, void *p) {
  switch (ev) {

    case MG_EV_HTTP_REQUEST: {
      mg_serve_http(nc, p, s_http_server_opts);
      break;
    }

    case MG_EV_WEBSOCKET_HANDSHAKE_REQUEST: {
      struct http_message *hm = (struct http_message *) p;      
      printf("MG_EV_WEBSOCKET_HANDSHAKE_REQUEST [%.*s], nc:%p, user_data:%p\n"
            , (int) hm->uri.len, hm->uri.p, nc, nc->user_data);
            
      if(webs_cfg.auth)
        if (!mg_http_is_authorized(hm, hm->uri, s_http_server_opts.auth_domain, s_http_server_opts.global_auth_file,
                                 MG_AUTH_FLAG_IS_GLOBAL_PASS_FILE)) {
        printf("%s => MG_EV_WEBSOCKET_HANDSHAKE_REQUEST !mg_http_is_authorized\n", __func__);
        mg_http_send_digest_auth_request(nc, s_http_server_opts.auth_domain);
        return;
        }
    
      if(!nc->user_data && !strncmp(hm->uri.p, "/flv", strlen("/flv")))
      {
        int ret = 0;
        int channel = 0, sid = 0;
        sscanf(hm->uri.p, "/flv%d", &channel);
        channel = (channel-1 > 0)?(channel-1):0;
                   
        GSF_MSG_DEF(gsf_sdp_t, sdp, sizeof(gsf_msg_t)+sizeof(gsf_sdp_t));
        
        //The main stream is preferred;
        sdp->video_shmid = sdp->audio_shmid = -1;
        ret = GSF_MSG_SENDTO(GSF_ID_CODEC_SDP, channel, GET, sid = 0
                              , sizeof(gsf_sdp_t)
                              , GSF_IPC_CODEC
                              , 2000);
        //When the width of the main stream is greater than 2592, the second stream is used;
        if(sdp->venc.width > 2688)
        {
          sdp->video_shmid = sdp->audio_shmid = -1;
          ret = GSF_MSG_SENDTO(GSF_ID_CODEC_SDP, channel, GET, sid = 1
                                , sizeof(gsf_sdp_t)
                                , GSF_IPC_CODEC
                                , 2000);
        }

        if(ret != 0)
        {
          printf("open err uri:%.*s, channel:%d\n",(int) hm->uri.len, hm->uri.p, channel);
          break;
        }

        session_cnt++;
                 
        lws_session_t *sess = (lws_session_t*)calloc(1, sizeof(lws_session_t));
        strcpy(sess->flag, "flv");
        #ifdef __DMA_COPY__ 
        sess->data = venc_viraddr_alloc(GSF_FRM_MAX_SIZE);
        #else
        sess->data     = malloc(GSF_FRM_MAX_SIZE);
        #endif
        sess->wsbuf[0] = malloc(GSF_FRM_MAX_SIZE);
        sess->wsbuf[1] = malloc(GSF_FRM_MAX_SIZE);

        printf("sess:%p, malloc:%p\n", sess, sess->wsbuf[0]);
        printf("sess:%p, malloc:%p\n", sess, sess->wsbuf[1]);
        assert(sess->wsbuf[0]);
        assert(sess->wsbuf[1]);
        
        sess->chn = channel;
        sess->sid = sid;
        sess->mgr = nc->mgr;
        sess->video = cfifo_shmat(cfifo_recsize, cfifo_rectag, sdp->video_shmid);
        sess->audio = cfifo_shmat(cfifo_recsize, cfifo_rectag, sdp->audio_shmid);
        
        sess->flv = flv_muxer_create(on_flv_packet, (void*)sess);
        
        pthread_create(&sess->thread_id, NULL, send_thread_func, sess);
        nc->user_data = sess;
        printf("SET nc:%p, user_data:%p, thread_id:%p\n", nc, nc->user_data, sess->thread_id);
        printf("open ok video_shmid:%d, audio_shmid:%d, flv:%p, uri:%.*s\n"
            , sdp->video_shmid, sdp->audio_shmid, sess->flv, (int) hm->uri.len, hm->uri.p);
      }
      else if(!nc->user_data && !strncmp(hm->uri.p, "/webrtc", strlen("/webrtc")))
      {
        // new session;
        rtc_sess_t *rtc_sess = rtc_sess_new();
        if(rtc_sess)
        {  
          rtc_sess->mgr = nc->mgr;
        }
        nc->user_data = rtc_sess;
        printf("SET rtc_sess nc:%p, user_data:%p\n", nc, nc->user_data);
      }
      break;
    }
    
    case MG_EV_WEBSOCKET_FRAME: {
      struct websocket_message *wm = (struct websocket_message *) p;
      
      #if 0
      printf("MG_EV_WEBSOCKET_FRAME [%s:%d], nc:%p, user_data:%p\n"
            , wm->data, wm->size, nc, nc->user_data);
      #endif
      
      if(nc->user_data && !strncmp(nc->user_data, "rtc", 3))
      {
        webrtc_proc(nc, (rtc_sess_t**)&nc->user_data, wm);
      }
      
      break;
    }
    
    case MG_EV_WEBSOCKET_HANDSHAKE_DONE: {
      printf("MG_EV_WEBSOCKET_HANDSHAKE_DONE [%s], nc:%p, user_data:%p\n", "", nc, nc->user_data);
      break;
    }

    case MG_EV_CLOSE:{
      if(nc->user_data)
      {
        if(!strncmp(nc->user_data, "flv", 3))
        {
          printf("MG_EV_CLOSE [%s], nc:%p, user_data:%p\n", "flv", nc, nc->user_data);
          
          lws_session_t *sess = (lws_session_t*)nc->user_data;
          if(sess->thread_id)
          {
            sess->thread_exit = 1;
          }
        }
        else if(!strncmp(nc->user_data, "rtc", 3))
        {
          printf("MG_EV_CLOSE [%s], nc:%p, user_data:%p\n", "rtc", nc, nc->user_data);
          
          // free sesson;
          rtc_sess_t *rtc_sess = (rtc_sess_t*)nc->user_data;
          rtc_sess_free(rtc_sess);
        }
      }
      break;
    }

    default:
      break;
  }
}

int http_close()
{
  rtc_uninit();
  
  s_received_signal = 1;
  return pthread_join(gpid, NULL);
}




void on_rtc_send(struct mg_connection *nc, int ev, void *p) {
  (void) ev;
  
  if(!p)
  {
    printf("%s => p == NULL.\n", __func__);
    return;
  }
  
  rtc_sess_t *rtc_sess = (rtc_sess_t*)p;

  printf("%s => p != NULL.\n", __func__);
  
  struct mg_connection *c = nc;
  
  if(1)
  {
    if (c->user_data != NULL) {
      if(c->user_data == rtc_sess)
      {
        char *local_ice = rtc_getCandidate(rtc_sess);
        if(local_ice)
        {
          //dst = src;
          local_ice[strlen(local_ice)-1] = ',';
          char dst_token[128]; sprintf(dst_token,"\"dst\":\"%s\"}", rtc_sess->dst);
          strcat(local_ice, dst_token);
          
          printf("send candidate:[%s]\n\n", local_ice);
          mg_send_websocket_frame(nc, WEBSOCKET_OP_TEXT, local_ice, strlen(local_ice));
        }
      }
    }
  }
}



static void* ws_task(void* parm);
int http_open(int port)
{
  signal(SIGPIPE, SIG_IGN);
  
  system("ifconfig lo 127.0.0.1");

  rtc_init();

  s_received_signal = 0;
  if(pthread_create(&gpid, NULL, ws_task, (void*)port) != 0)
  {
  	printf("create ws_task error%d(%s)\n",errno, strerror(errno));
    return -1;
  }
  
  if(webs_cfg.webrtc_en)
  {  
    return signaling_init(webs_cfg.webrtc_url);
  }
  return 0;
}

struct file_writer_data {
  FILE *fp;
  size_t bytes_written;
};

extern int upg_stat;
extern int upg_progress;

void handle_upload(struct mg_connection *nc, int ev, void *p) {
  struct file_writer_data *data = (struct file_writer_data *) nc->user_data;
  struct mg_http_multipart_part *mp = (struct mg_http_multipart_part *) p;

  switch (ev) {
    case MG_EV_HTTP_MULTIPART_REQUEST: {
      struct http_message *hm = (struct http_message *) p;
      printf("%s => [%.*s], nc:%p, user_data:%p\n"
            ,__func__, (int) hm->uri.len, hm->uri.p, nc, nc->user_data);
      if(webs_cfg.auth)
        if (!mg_http_is_authorized(hm, hm->uri, s_http_server_opts.auth_domain, s_http_server_opts.global_auth_file,
                                 MG_AUTH_FLAG_IS_GLOBAL_PASS_FILE)) {
        mg_http_send_digest_auth_request(nc, s_http_server_opts.auth_domain);
        return;
        }
      break;
    }
    case MG_EV_HTTP_PART_BEGIN: {
      
      if (data == NULL) {
        
        printf("var_name:%s, file_name:%s\n",mp->var_name, mp->file_name);
        
        if(strstr(mp->file_name, ".upg"))
        {
          if(upg_stat == 1)
          {
            mg_printf(nc,
                "HTTP/1.1 200 OK\r\n"
                "Content-Type: text/plain\r\n"
                "Connection: close\r\n\r\n"
                "Device is upgrading \n\n");
            nc->flags |= MG_F_SEND_AND_CLOSE;
            return;
          }
          upg_stat = 1;
        }
        char lpath[256] = {0};
        data = calloc(1, sizeof(struct file_writer_data));
        snprintf(lpath, sizeof(lpath)-1, "/tmp/%s", mp->file_name);
        data->fp = fopen(lpath, "w");//tmpfile();
        data->bytes_written = 0;

        if (data->fp == NULL) {
          mg_printf(nc, "%s",
                    "HTTP/1.1 500 Failed to open a file\r\n"
                    "Content-Length: 0\r\n\r\n");
          nc->flags |= MG_F_SEND_AND_CLOSE;
          free(data);
          return;
        }
        nc->user_data = (void *) data;
      }
      break;
    }
    case MG_EV_HTTP_PART_DATA: {
      
      if(!data)
      {
        nc->flags |= MG_F_SEND_AND_CLOSE;
        return;
      }
      
      if (fwrite(mp->data.p, 1, mp->data.len, data->fp) != mp->data.len) {
        mg_printf(nc, "%s",
                  "HTTP/1.1 500 Failed to write to a file\r\n"
                  "Content-Length: 0\r\n\r\n");
        nc->flags |= MG_F_SEND_AND_CLOSE;
        
        return;
      }
      data->bytes_written += mp->data.len;
      break;
    }
    case MG_EV_HTTP_PART_END: {
      
      if(!data)
      {
        nc->flags |= MG_F_SEND_AND_CLOSE;
        return;
      }
      
      int ret = 0;
      if(strstr(mp->file_name, ".jpg"))
      {
        GSF_MSG_DEF(char, face_path, sizeof(gsf_msg_t)+256);
        snprintf(face_path, 256-1, "/tmp/%s", mp->file_name);
        ret = GSF_MSG_SENDTO(GSF_ID_SVP_FACE, 0, SET, 0
                        , strlen(face_path)+1
                        , GSF_IPC_SVP
                        , 2000);
        mg_printf(nc,
          "HTTP/1.1 200 OK\r\n"
          "Content-Type: text/plain\r\n"
          "Connection: close\r\n\r\n"
          "face file size:%ld ret:%d\n\n",
          (long) ftell(data->fp), ret);
      }
      else
      {
        mg_printf(nc,
          "HTTP/1.1 200 OK\r\n"
          "Content-Type: text/plain\r\n"
          "Connection: close\r\n\r\n"
          "write file size:%ld to /tmp\n\n",
          (long) ftell(data->fp));
      }
      
      nc->flags |= MG_F_SEND_AND_CLOSE;
      fclose(data->fp);
      free(data);
      nc->user_data = NULL;
      
      if(strstr(mp->file_name, ".upg"))
      {
        GSF_MSG_DEF(char, upg_path, sizeof(gsf_msg_t)+256);
        snprintf(upg_path, 256-1, "/tmp/%s", mp->file_name);
        GSF_MSG_SENDTO(GSF_ID_BSP_UPG, 0, SET, 0
                        , strlen(upg_path)+1
                        , GSF_IPC_BSP
                        , 2000);
      }
      break;
    }
  }
}

void handle_config(struct mg_connection *nc, int ev, void *pp) 
{
  struct http_message* hm = (struct http_message*)pp;

  if(ev == MG_EV_CLOSE)
    return;
 
  if(hm->query_string.len == 0)
    return;

  if(webs_cfg.auth)
    if (!mg_http_is_authorized(hm, hm->uri, s_http_server_opts.auth_domain, s_http_server_opts.global_auth_file,
                             MG_AUTH_FLAG_IS_GLOBAL_PASS_FILE)) {
    mg_http_send_digest_auth_request(nc, s_http_server_opts.auth_domain);
    return;
    }

  char req[128] = {0};
  char args[128] = {0};
  char *p = NULL, *body = NULL;
  char *post_body = NULL;
  char *qs = (char*)malloc(hm->query_string.len + 1);
  sprintf(qs, "%.*s", hm->query_string.len, hm->query_string.p);
  
  
  // GET /config?id=GSF_ID_CODEC_VENC&args=G0C0S0&body=
  
  if(p = strstr(qs, "id="))
    sscanf(p, "id=%127[^&]", req);
  if(p = strstr(qs, "args="))
    sscanf(p, "args=%127[^&]", args);
 
  printf("qs >>> id:[%s], args:[%s]\n", req, args);
 
  if(mg_vcmp(&hm->method, "POST") == 0)
  {
    /*
      curl -X POST -d '{"a" : "aaaaa", "b" : "bbbbb"}' 'http://192.168.1.2/config?id=GSF_ID_CODEC_VENC&args=G0C0S0'
      POST /config?id=GSF_ID_CODEC_VENC&args=G0C0S0
      Content-Length: xx
      
      {"a" : "aaaaa", "b" : "bbbbb"}
    */
    printf("POST body >>> [%.*s]\n", hm->body.len, hm->body.p);
    if(hm->body.len > 0)
    {
      post_body = (char*)malloc(hm->body.len + 1);
      sprintf(post_body, "%.*s", hm->body.len, hm->body.p);
      body = post_body;
    }
  }
  else
  {
    /*
      curl -G  "http://192.168.1.2/config?id=GSF_ID_CODEC_VENC&args=G0C0S0" --data-urlencode 'body={"a":"aaaaa","b" : "bbbbbb"}'
      GET /config?id=GSF_ID_CODEC_VENC&args=G0C0S0&body=%7B%22a%22%3A%22aaaaa%22%2C%22b%22%20%3A%20%22bbbbbb%22%7D
    */
    body = strstr(qs, "&body=");
    body = (body)?body+strlen("&body="):NULL;
    if(body)
      gsf_url_decode(body, strlen(body));
    printf("GET body >>> [%s]\n", body);
  }
  
  
  // call process;
  char out[80*1024] = {0};
  mod_call(req, args, body, out, sizeof(out));
  
  if(qs)
    free(qs);
  
  if(post_body)
    free(post_body);

  mg_printf(nc, "HTTP/1.1 200 OK\r\n");
  mg_printf(nc, "Content-Length: %lu\r\n\r\n", strlen(out));
  mg_printf(nc, out);
  
  //响应完毕，关闭连接
  nc->flags |= MG_F_SEND_AND_CLOSE;
}


void handle_snap(struct mg_connection *nc, int ev, void *pp)
{
  struct http_message* hm = (struct http_message*)pp;

  if(ev == MG_EV_CLOSE)
    return;
 
  if(hm->query_string.len == 0)
    return;
  
  if(webs_cfg.auth)
    if (!mg_http_is_authorized(hm, hm->uri, s_http_server_opts.auth_domain, s_http_server_opts.global_auth_file,
                             MG_AUTH_FLAG_IS_GLOBAL_PASS_FILE)) {
    mg_http_send_digest_auth_request(nc, s_http_server_opts.auth_domain);
    return;
    }

  // GET /snap?args=G0C0S0
  int ret = 0;
  int channel = -1, gset = -1, sid = -1;
  sscanf(hm->query_string.p, "args=G%dC%dS%d", &gset, &channel, &sid);
  
  GSF_MSG_DEF(char, msgdata, sizeof(gsf_msg_t) + 4*1024);

  //gset;
  ret = GSF_MSG_SENDTO(GSF_ID_CODEC_SNAP, channel, gset, sid
                        , 0
                        , GSF_IPC_CODEC
                        , 3000);
                        
  printf("send => To:%s, ret:%d, msg->size:%d\n", GSF_IPC_CODEC, ret, __pmsg->size);
  
  char qs_buf[256] = {0};
  int qs_len = (hm->query_string.len < sizeof(qs_buf)-1)?hm->query_string.len:(sizeof(qs_buf)-1);
  sprintf(qs_buf, "%.*s", qs_len, hm->query_string.p);
  char *target = strstr(qs_buf, "target=");
  
  if(target) //GSF_ID_SVP_FEATURE  
  {
    GSF_MSG_DEF(char, msgdata, sizeof(gsf_msg_t) + 256);
    target += strlen("target=");
    strcpy(msgdata, target);
    
    ret = GSF_MSG_SENDTO(GSF_ID_SVP_FEATURE, channel, 1, sid
                          , strlen(target)+1
                          , GSF_IPC_SVP
                          , 1000);
    printf("GSF_ID_SVP_FEATURE target=%s, ret:%d\n", target, ret);
  }
  
  
  if(ret == 0 && __pmsg->size > 0)
  {
    FILE* fpJpg = NULL;
    fpJpg = fopen(__pmsg->data, "rb");
    if(fpJpg)
    {
      struct stat stStat;
      memset(&stStat, 0, sizeof(struct stat));
      
      int fd = fileno(fpJpg);
      fstat(fd, &stStat);
      
      mg_printf(nc, "HTTP/1.1 200 OK\r\n");
    	mg_printf(nc, "Content-length: %d\r\n",  stStat.st_size);
    	mg_printf(nc, "Content-type: image/jpeg\r\n\r\n");

      char buf[BUFSIZ];
      size_t n;
      while ((n = mg_fread(buf, 1, sizeof(buf), fpJpg)) > 0) 
      {
        mg_send(nc, buf, n);
      }
      fclose(fpJpg);
      
      //响应完毕，关闭连接
      nc->flags |= MG_F_SEND_AND_CLOSE;
      return;
    }
  }
  
  mg_printf(nc, "HTTP/1.1 404 Not Found\r\n");
  
  //响应完毕，关闭连接
  nc->flags |= MG_F_SEND_AND_CLOSE;
  return;
}

void handle_stat(struct mg_connection *nc, int ev, void *pp)
{
  struct http_message* hm = (struct http_message*)pp;

  if(ev == MG_EV_CLOSE)
    return;
 
  if(hm->query_string.len == 0)
    return;

  char req[128] = {0};
  char args[128] = {0};
  char *p = NULL;

  char *qs = (char*)malloc(hm->query_string.len + 1);
  sprintf(qs, "%.*s", hm->query_string.len, hm->query_string.p);
  
  // GET /stat?id=GSF_ID_BSP_UPG&args=G0C0S0&body=
  
  if(p = strstr(qs, "id="))
    sscanf(p, "id=%127[^&]", req);
  if(p = strstr(qs, "args="))
    sscanf(p, "args=%127[^&]", args);
 
  printf("qs >>> id:[%s], args:[%s]\n", req, args);
 
  char out[8*1024] = {0};
  
  //TODO;
  if(strstr(req, "GSF_ID_BSP_UPG"))
  {
    sprintf(out, "{\"progress\": %d}", upg_progress);
  }
  else if(strstr(req, "GSF_ID_BSP_SSTAT"))
  {
    float cpu, temp, mem, ddr;
    extern int sys_stat(float *cpu, float *temp, float *mem, float *ddr);
    
    sys_stat(&cpu, &temp, &mem, &ddr);
    printf("cpu:%0.2f, temp:%0.2f, mem:%0.2f, ddr:%0.2f\n", cpu, temp, mem, ddr);
    sprintf(out, "{"
                  "\"cpu\": %.2f,"
                  "\"temp\": %.2f,"
                  "\"mem\": %.2f," 
                  "\"ddr\": %.2f" 
                  "}", cpu, temp, mem, ddr);
  }
  
  
  if(qs)
    free(qs);

  mg_printf(nc, "HTTP/1.1 200 OK\r\n");
  mg_printf(nc, "Content-Length: %lu\r\n\r\n", strlen(out));
  mg_printf(nc, out);
  
  nc->flags |= MG_F_SEND_AND_CLOSE;
}


void handle_control(struct mg_connection *nc, int ev, void *pp)
{
  struct http_message* hm = (struct http_message*)pp;

  if(ev == MG_EV_CLOSE)
    return;
 
  if(hm->query_string.len == 0)
    return;

  char req[128] = {0};
  char args[128] = {0};
  char *p = NULL;

  char *qs = (char*)malloc(hm->query_string.len + 1);
  sprintf(qs, "%.*s", hm->query_string.len, hm->query_string.p);
  
  // GET /control?id=GSF_ID_BSP_UPG&args=G0C0S1&body=
  
  if(p = strstr(qs, "id="))
    sscanf(p, "id=%127[^&]", req);
  if(p = strstr(qs, "args="))
    sscanf(p, "args=%127[^&]", args);
 
  printf("qs >>> id:[%s], args:[%s]\n", req, args);

  int channel = -1, gset = -1, sid = -1;
  sscanf(args, "G%dC%dS%d", &gset, &channel, &sid);
  
  char out[8*1024] = {0};
  
  //TODO;
  int ret = -1;
  if(strstr(req, "GSF_ID_BSP_UPG"))
  {
    GSF_MSG_DEF(gsf_msg_t, msg, 4*1024);
    ret = GSF_MSG_SENDTO(GSF_ID_BSP_UPG, channel, gset, sid
                        , 0
                        , GSF_IPC_BSP
                        , 2000);
  }
  
  sprintf(out, "{\"code\":%d}", ret);
  
  if(qs)
    free(qs);

  mg_printf(nc, "HTTP/1.1 200 OK\r\n");
  mg_printf(nc, "Content-Length: %lu\r\n\r\n", strlen(out));
  mg_printf(nc, out);
  
  nc->flags |= MG_F_SEND_AND_CLOSE;
}




#define MSG_HANDER_REG(c) do { \
  mg_register_http_endpoint(c, "/upload", handle_upload MG_UD_ARG(NULL));\
  mg_register_http_endpoint(c, "/config", handle_config MG_UD_ARG(NULL));\
  mg_register_http_endpoint(c, "/snap", handle_snap MG_UD_ARG(NULL));    \
  mg_register_http_endpoint(c, "/stat", handle_stat MG_UD_ARG(NULL));    \
  mg_register_http_endpoint(c, "/control", handle_control MG_UD_ARG(NULL)); \
}while(0)


static int getuser(gsf_user_t *user, int num)
{
  GSF_MSG_DEF(gsf_msg_t, msg, 4*1024);
  int ret = GSF_MSG_SENDTO(GSF_ID_BSP_USER, 0, GET, 0, 0, GSF_IPC_BSP, 2000);
  gsf_user_t *userlist = (gsf_user_t*)__pmsg->data;
  int userlist_num = __pmsg->size/sizeof(gsf_user_t);
  printf("GET GSF_ID_BSP_USER To:%s, ret:%d, userlist num:%d\n"
        , GSF_IPC_BSP, ret, __pmsg->size/sizeof(gsf_user_t));

  if(ret < 0)
    return ret;

  if(user)
    memcpy(user, userlist, ((userlist_num > num)?num:userlist_num)*sizeof(gsf_user_t));

  return ret;
}


static void* ws_task(void* parm)
{
  // https://github.com/cesanta/mongoose-os-docs/blob/master/mongoose-os/userguide/security.md
  // ./openssl req  -nodes -new -x509  -keyout key.pem -out cert.pem
  // https://www.cnblogs.com/ylaoda/p/12032992.html
   
  char address[64];
  //sprintf(address, "%d", (int)parm);
  sprintf(address, "0.0.0.0:%d", (int)parm);
  
  struct mg_mgr mgr;
  struct mg_connection *c, *https;
  const char *err = NULL;

  mg_mgr_init(&mgr, NULL);
    
  struct mg_bind_opts opts;
  memset(&opts, 0, sizeof(opts));
  opts.error_string = &err;
  
  char www_path[128] = {0};
  proc_absolute_path(www_path);
  
  sprintf(www_path, "%s/../www", www_path);
  printf("www_path:[%s]\n", www_path);
  s_http_server_opts.document_root = www_path;  // Serve current directory
  
  char auth_file[128];
  sprintf(auth_file, "%s/.htpasswd", www_path);
  s_http_server_opts.global_auth_file = NULL;
  
  if(access(auth_file, 0) != -1)
    unlink(auth_file);
 
  if(webs_cfg.auth)
  {
    s_http_server_opts.auth_domain = "MyRealm";
    s_http_server_opts.global_auth_file = auth_file;
    
    gsf_user_t userlist[8];
    memset(userlist, 0, sizeof(userlist));
    getuser(userlist, 8);
    //only admin user for test;
    if(strstr(userlist[0].name, "admin"))
    {
      char cmdstr[256] = {0};
      sprintf(cmdstr, "printf %s:MyRealm: > %s;printf %s:MyRealm:%s | md5sum | awk '{print $1}' >> %s;"
                      , userlist[0].name, auth_file, userlist[0].name, userlist[0].pwd, auth_file);
      system(cmdstr);
    }
  }

  if ((c = mg_bind_opt(&mgr, address, ev_handler, opts)) == NULL) {
    fprintf(stderr, "mg_bind(%s) failed: %s\n", address, err);
    return NULL;
  }

  // Set up HTTP server parameters
  mg_set_protocol_http_websocket(c);
  MSG_HANDER_REG(c);
  
  char ssl_cert[128];
  char ssl_key[128];
  sprintf(ssl_cert, "%s/cert.pem", www_path);
  sprintf(ssl_key, "%s/key.pem", www_path);
  opts.ssl_cert = ssl_cert;
  opts.ssl_key  = ssl_key;
  https = mg_bind_opt(&mgr, "443", ev_handler, opts);
  if(https == NULL)
  {
    fprintf(stderr, "mg_bind(%s) failed: %s\n", "443", err);
    return NULL;
  }
  
  mg_set_protocol_http_websocket(https);
  MSG_HANDER_REG(https);
  
  while (s_received_signal == 0) {
    mg_mgr_poll(&mgr, 1000);
  }
  
  mg_mgr_free(&mgr);
  printf("%s => exit.\n", __func__);
  return NULL;
}

