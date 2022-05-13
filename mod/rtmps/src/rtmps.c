#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <assert.h>
#include "sockutil.h"

#include "inc/gsf.h"

#include "proc.h"
#include "flv-muxer.h"
#include "rtmp-client.h"
#include "flv-reader.h"
#include "flv-proto.h"

#include "rtmps.h"
#include "cfg.h"
#include "msg_func.h"

#include "mod/bsp/inc/bsp.h"
#include "mod/codec/inc/codec.h"

GSF_LOG_GLOBAL_INIT("RTMPS", 8*1024);


#define MAX_FRAME_SIZE (1000*1024)

typedef struct {
  int idr;
  struct cfifo_ex* video;
  flv_muxer_t* flv;
  rtmp_client_t* rtmp;
  unsigned int pts;
  char* data;
}sess_t;


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
      
    assert(rec->data[0] == 00 && rec->data[1] == 00 && rec->data[2] == 00 && rec->data[3] == 01);

    return len;
}


static int flv_muxer_meta(sess_t *sess)
{
	struct flv_metadata_t metadata;
	metadata.audiocodecid = 4;
	metadata.audiodatarate = 16.1;
	metadata.audiosamplerate = 48000;
	metadata.audiosamplesize = 16;
	metadata.stereo = TRUE;
	metadata.videocodecid = 7;
	metadata.videodatarate = 64.0;
	metadata.framerate = 30;
	metadata.width = 1920;
	metadata.height = 1080;
	flv_muxer_metadata(sess->flv, &metadata);
  return 0;
}


static int on_flv_packet(void* _sess, int type, const void* data, size_t bytes, uint32_t timestamp)
{
  sess_t* sess = (sess_t*)_sess;
	int r = bytes;
	char *packet = (char*)data;

  int keyframe = 1 == ((packet[0] & 0xF0) >> 4);
  if((type==FLV_TYPE_VIDEO) && keyframe)
  printf("type:%02d [A:%d, V:%d, S:%d] key:%d\n"
        , type, FLV_TYPE_AUDIO, FLV_TYPE_VIDEO, FLV_TYPE_SCRIPT, (type==FLV_TYPE_VIDEO)?keyframe:0);

  if(!sess->rtmp)
    return 0;

	if (FLV_TYPE_AUDIO == type)
	{
		r = rtmp_client_push_audio(sess->rtmp, packet, r, timestamp);
	}
	else if (FLV_TYPE_VIDEO == type)
	{
		r = rtmp_client_push_video(sess->rtmp, packet, r, timestamp);
	}
	else if (FLV_TYPE_SCRIPT == type)
	{
		r = rtmp_client_push_script(sess->rtmp, packet, r, timestamp);
	}
	else
	{
		assert(0);
		r = 0; // ignore
	}

  return 0;
}

static int rtmp_client_send(void* param, const void* header, size_t len, const void* data, size_t bytes)
{
	socket_t* socket = (socket_t*)param;
	socket_bufvec_t vec[2];
	socket_setbufvec(vec, 0, (void*)header, len);
	socket_setbufvec(vec, 1, (void*)data, bytes);
	return socket_send_v_all_by_time(*socket, vec, bytes > 0 ? 2 : 1, 0, 5000);
}


typedef struct {
  socket_t socket;
  rtmp_client_t* rtmp;
  int _runing, _exit;
}rtmp_push_t;

static rtmp_push_t *g_push;

int rtmp_push_close(rtmp_push_t *p)
{
  printf("push stop ...\n");
  if(p && p->rtmp)
  {
    rtmp_client_destroy(p->rtmp);
    socket_close(p->socket);
    free(p);
    g_push = NULL;
    //socket_cleanup();
  }
  return 0;
}


rtmp_push_t* rtmp_push_open(char* url)
{
    //rtmp://push.zhuagewawa.com/record/w255?wsSecret=b06fe45a1640a18a2817cdfecfb8a563&wsABSTime=1649303438

    char host[128] = {0};
    char app[128] = {0};
    char stream[128] = {0};
    sscanf(url, "rtmp://%[^/]/%[^/]/%[^/]", host, app, stream);
    
    char packet[64*1024];
  	snprintf(packet, sizeof(packet), "rtmp://%s/%s", host, app);

    if(host[0] == '\0' || app[0] == '\0' || stream[0] == '\0')
      return NULL;

    printf("push url:[%s/%s]\n", packet, stream);

  	struct rtmp_client_handler_t handler;
  	memset(&handler, 0, sizeof(handler));
  	handler.send = rtmp_client_send;
    rtmp_client_t* rtmp = NULL;
    
    rtmp_push_t* p = malloc(sizeof(rtmp_push_t));
    
    static int _init = 0;
    if(!_init)
    {
      _init = 1;
      socket_init();
    } 
  	
  	p->socket = socket_connect_host(host, 1935, 2000);
  	socket_setnonblock(p->socket, 0);

  	p->rtmp = rtmp_client_create(app, stream, packet/*tcurl*/, &p->socket, &handler);
  	int r = rtmp_client_start(p->rtmp, 0);

  	while (4 != rtmp_client_getstate(p->rtmp) && (r = socket_recv(p->socket, packet, sizeof(packet), 0)) > 0)
  	{
  		if(0 != rtmp_client_input(p->rtmp, packet, r))
  		{
  		  goto __err;
  		}
  	}
  	return p;
__err:
    rtmp_push_close(p);
    return NULL;
}

int rtmp_push_stop(rtmp_push_t *p)
{
  p = p?p:g_push;
  if(!p)
    return 0;
    
  p->_runing = 0;
  while(!p->_exit)
    usleep(100*1000);
  return 0;
}

int rtmp_push_start(rtmp_push_t *p)
{
    p->_exit = !(p->_runing = 1);
    printf("push start ...\n");
    
    int ret = 0;
    int channel = 0, sid = 0;
    
    sess_t _sess;
    sess_t *sess = &_sess;
    sess->data = malloc(MAX_FRAME_SIZE);
    sess->idr  = 0;
    sess->pts  = 0;
    
    {
      GSF_MSG_DEF(gsf_sdp_t, sdp, sizeof(gsf_msg_t)+sizeof(gsf_sdp_t));
      sdp->video_shmid = -1;

      ret = GSF_MSG_SENDTO(GSF_ID_CODEC_SDP, channel, GET, sid
                          , sizeof(gsf_sdp_t)
                          , GSF_IPC_CODEC
                          , 2000);
      sess->video = cfifo_shmat(cfifo_recsize, cfifo_rectag, sdp->video_shmid);
      sess->flv  = flv_muxer_create(on_flv_packet, (void*)sess);
      sess->rtmp = p->rtmp;
      cfifo_newest(sess->video, 0);
    }
    
    {
      GSF_MSG_DEF(char, msgdata, sizeof(gsf_msg_t));
      GSF_MSG_SENDTO(GSF_ID_CODEC_IDR, channel, SET, sid
                      , 0
                      , GSF_IPC_CODEC
                      , 2000);
    }
    while(p->_runing)
    {
      gsf_frm_t *rec = (gsf_frm_t *)sess->data;
      int ret = cfifo_get(sess->video, cfifo_recgut, (unsigned char*)sess->data);
      sess->idr = (sess->idr)?:(rec->flag&GSF_FRM_FLAG_IDR);
      
      if(ret < 0)
      {
          //printf("cfifo err ret:%d\n", ret);
      }
      else if (ret == 0)
      {
        //printf("cfifo empty ret:%d\n", ret);
      }
      else if (ret > 0 && sess->idr)
      {
        //printf("cfifo frame ret:%d\n", ret);
        if(rec->video.nal[0] /*&& rec->video.encode == 0*/)
        {
          if(!sess->pts)
          {
            flv_muxer_meta(sess);
            sess->pts = rec->pts;
          }
          
          if(flv_muxer_avc(sess->flv // sps-pps-vcl
                  , rec->data
                  , rec->size
                  , rec->pts - sess->pts
                  , rec->pts - sess->pts) < 0)
          {
            printf("flv_muxer_avc err.\n");
          }
        }
      }
      usleep(5*1000);
    }

    if(sess->video) cfifo_free(sess->video);
    if(sess->flv) flv_muxer_destroy(sess->flv);
    if(sess->data) free(sess->data);
    p->_exit = 1;
    return 0;
}


int main(int argc, char *argv[])
{
    if(argc < 2)
    {
      printf("pls input: %s rtmps_parm.json\n", argv[0]);
      return -1;
    }
    
    strncpy(rtmps_parm_path, argv[1], sizeof(rtmps_parm_path)-1);
    
    if(json_parm_load(rtmps_parm_path, &rtmps_parm) < 0)
    {
      json_parm_save(rtmps_parm_path, &rtmps_parm);
      json_parm_load(rtmps_parm_path, &rtmps_parm);
    }
    info("rtmps_parm => auth:%d, port:%d\n", rtmps_parm.auth, rtmps_parm.port);
    
    GSF_LOG_CONN(1, 100);
    void* rep = nm_rep_listen(GSF_IPC_RTMPS, NM_REP_MAX_WORKERS, NM_REP_OSIZE_MAX, req_recv);

    while(1)
    {
      //char *url = "rtmp://push.zhuagewawa.com/record/w255?wsSecret=b06fe45a1640a18a2817cdfecfb8a563&wsABSTime=1649303438";
      g_push = rtmp_push_open(rtmps_parm.purl);
      if(!g_push)
      {
        sleep(3);
        continue;
      }
      rtmp_push_start(g_push);
      rtmp_push_close(g_push);
    }
    

    GSF_LOG_DISCONN();
    return 0;
}