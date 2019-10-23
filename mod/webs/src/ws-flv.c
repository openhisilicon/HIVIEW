#if 0

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

#include "inc/gsf.h"
#include "fw/libflv/inc/flv-muxer.h"
#include "fw/libwebsockets/inc/libwebsockets.h"
#include "fw/libwebsockets/inc/lws-sub.h"

extern unsigned int cfifo_recsize(unsigned char *p1, unsigned int n1, unsigned char *p2);
extern unsigned int cfifo_rectag(unsigned char *p1, unsigned int n1, unsigned char *p2);
extern unsigned int cfifo_recgut(unsigned char *p1, unsigned int n1, unsigned char *p2, void *u);

#define FLV_TYPE_AUDIO 8
#define FLV_TYPE_VDIEO 9
#define FLV_TYPE_SCRIPT 18

#define MAX_FRAME_SIZE (300*1024)
typedef struct {
  struct cfifo_ex* video;
  unsigned char data[MAX_FRAME_SIZE];
  unsigned char wsbuf[MAX_FRAME_SIZE+1024];
  flv_muxer_t* flv;
  unsigned int pts;
  struct lws *wsi;
  char url[256];
}lws_session_t;

static int force_exit = 0;
static int session_cnt = 0;
static pthread_t gpid;

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
  int len = 0;
  unsigned char *raw = &sess->wsbuf[LWS_SEND_BUFFER_PRE_PADDING];
	raw[0] = 'F'; // FLV signature
	raw[1] = 'L';
	raw[2] = 'V';
	raw[3] = 0x01; // File version
	raw[4] = 0x01;// | 0x04; // Type flags (audio:0x04 & video:0x01)
	be_write_uint32(raw + 5, 9); // Data offset
	be_write_uint32(raw + 9, 0); // PreviousTagSize0(Always 0)
  len += (9+4);
  
  int ret = lws_write(sess->wsi,
         raw,
         len,
         LWS_WRITE_BINARY);
  
  if( ret != len )
  {
    printf("lws_write >>>  ret:%d, len:%d\n",ret, len);
    return -1;
  }
  
  return 0;
}

static int flv_send_tag(lws_session_t *sess, int type, const void* data, size_t bytes, uint32_t timestamp)
{  
  int len = 0;
  unsigned char *raw = &sess->wsbuf[LWS_SEND_BUFFER_PRE_PADDING]; 
  flv_write_tag(raw+len, (uint8_t)type, (uint32_t)bytes, timestamp);
  len += 11;
  memcpy(raw+len, data, bytes);
  len += bytes;
  be_write_uint32(raw+len, (uint32_t)bytes + 11);
  len += 4;
  
  int ret = lws_write(sess->wsi,
         raw,
         len,
         LWS_WRITE_BINARY);
  
  if( ret != len )
  {
    printf("lws_write >>>  ret:%d, len:%d\n",ret, len);
    return -1;
  }
  
  return 0;
}

static int on_flv_packet(void* sess, int type, const void* data, size_t bytes, uint32_t timestamp)
{
	// write FLV Tag;
	return flv_send_tag(sess, type, data, bytes, timestamp);
}


static int callback_movie(struct lws *wsi,
                          enum lws_callback_reasons reason, void *user,
                          void *in, size_t len){
    int ret = 0;
    lws_session_t *sess = (lws_session_t*)user;

    switch (reason){
        case LWS_CALLBACK_CLOSED:
            lwsl_notice("LWS_CALLBACK_CLOSED lws:%p, user:%p\n", wsi, user);
            if(sess)
            {
              if(sess->video)
              {
                lwsl_notice("close video:%p \n", sess->video);
                session_cnt--;
                cfifo_free(sess->video);
              }
              if(sess->flv)
              {
                flv_muxer_destroy(sess->flv);
              }
            }
            break;
        case LWS_CALLBACK_ESTABLISHED:
            lwsl_notice("LWS_CALLBACK_ESTABLISHED lws:%p, user:%p\n", wsi, user);
            if(sess)
            {
              int channel = 0;
              sscanf(sess->url, "/flv%d", &channel);
              channel = (channel-1 > 0)?(channel-1):0;
              session_cnt++;
              sess->wsi = wsi;
              
              GSF_MSG_DEF(gsf_sdp_t, sdp, sizeof(gsf_msg_t)+sizeof(gsf_sdp_t));
              sdp->video_shmid = -1;
              #if 1
              ret = GSF_MSG_SENDTO(GSF_ID_CODEC_SDP, 0, GET, 0
                                    , sizeof(gsf_sdp_t)
                                    , GSF_IPC_CODEC
                                    , 2000);
              #else
              ret = -1;
              #endif                         
                                    
              if(ret == 0)
              {
                sess->video = cfifo_shmat(cfifo_recsize, cfifo_rectag, sdp->video_shmid);
                cfifo_newest(sess->video, 1);
                sess->flv = flv_muxer_create(on_flv_packet, (void*)sess);
                lwsl_notice("open ok video:%p, flv:%p, url:%s\n", sess->video, sess->flv, sess->url);
              }
              else
              {
                sess->video = sess->flv = NULL;
                lwsl_notice("open err video:%p, flv:%p, url:%s\n", sess->video, sess->flv, sess->url);
                return -1;
              }
            }
            break;
        case LWS_CALLBACK_PROTOCOL_DESTROY:
            lwsl_notice("LWS_CALLBACK_PROTOCOL_DESTROY lws:%p, user:%p\n", wsi, user);
            break;
        case LWS_CALLBACK_SERVER_WRITEABLE: 
            //lwsl_notice("LWS_CALLBACK_SERVER_WRITEABLE lws:%p, user:%p\n", wsi, user);
            if(sess && sess->video)
            {
              gsf_frm_t *rec = (gsf_frm_t *)sess->data;
              ret = cfifo_get(sess->video, cfifo_recgut, (unsigned char*)sess->data);
              //printf("sess:%p, sess->video:%p, ret:%d\n", sess, sess->video, ret);
              if(ret < 0)
              {
                  //printf("cfifo err ret:%d\n", ret);
              }
              else if (ret == 0)
              {
                //printf("cfifo empty ret:%d\n", ret);
              }
              else if (ret > 0 )
              {
                //printf("cfifo frame ret:%d\n", ret);
                if(rec->video.nal[0] /*&& rec->video.encode == 0*/)
                {
                  if(!sess->pts)
                  {
                    flv_send_header(sess);
                    sess->pts = rec->pts;
                  }
                  
                  int i = 0;
                  unsigned int  *nal   = rec->video.nal;
                  unsigned char *offset = rec->data;
                  for (i = 0; i < GSF_FRM_NAL_NUM; i++)
                  {
                    if(nal[i] == 0)
                      break;
                    #if 0
                    printf("nal[%d]:%d [%02x %02x %02x %02x %02x %02x]\n", i, nal[i]
                          , offset[0],offset[1],offset[2],offset[3],offset[4],offset[5]);
                    #endif
                    if(flv_muxer_h264_nalu(sess->flv
                            , offset+4
                            , nal[i]-4
                            , rec->pts - sess->pts
                            , rec->pts - sess->pts) < 0)
                      {
                        //printf("flv_muxer_h264_nalu err.\n");
                      }
                      offset += nal[i];
                  }
                }
              }
            }
            else
            {
              return -1;
            }
            break;
        case LWS_CALLBACK_RECEIVE:
            lwsl_notice("LWS_CALLBACK_RECEIVE lws:%p, [%s]\n", wsi, (const char*)in);
            break;
        case LWS_CALLBACK_FILTER_PROTOCOL_CONNECTION:
            dump_handshake_info(wsi);
            if(sess)
              lws_hdr_copy(wsi, sess->url, sizeof(sess->url), WSI_TOKEN_GET_URI);
            break;
        default:
            break;
    }
    
    return 0;
}

static struct lws_protocols protocols[] = {
    {
        "movie",		          /* name */
        callback_movie,		    /* callback */
        sizeof(lws_session_t),/* per_session_data_size */
        1000000,		          /* max frame size / rx buffer */
	      0,			              /* id */
	      NULL,			            /* user */
	      1000000			          /* tx_packet_size */
    },
    { NULL, NULL, 0, 0 }      /* terminator */
};


void sighandler(int sig){
    force_exit = 1;
}


int ws_flv_close()
{
  sighandler(0);
  return pthread_join(gpid, NULL);
}

static void* ws_task(void* parm);
int ws_flv_open(int port)
{
  force_exit = 0;
  if(pthread_create(&gpid, NULL, ws_task, (void*)port) != 0)
  {
  	printf("create ws_task error%d(%s)\n",errno, strerror(errno));
    return -1;
  }
  return 0;
}

static void* ws_task(void* parm)
{
    int n = 0;
    struct lws_context *context;
    struct lws_context_creation_info info;
    { // set info
        memset(&info, 0, sizeof info);
        info.port = (int)parm;
        info.iface = NULL;
        info.protocols = protocols;
        info.ssl_cert_filepath = NULL;
        info.ssl_private_key_filepath = NULL;
        info.gid = -1;
        info.uid = -1;
        info.options = 0;
    }

    int syslog_options = LOG_PID | LOG_PERROR;
    int debug_level = 7;
    { // log setting
        lws_set_log_level(debug_level, lwsl_emit_syslog);
        lwsl_notice("lws chat server -\n");
        setlogmask(LOG_UPTO (LOG_DEBUG));
        openlog("lwsts", syslog_options, LOG_DAEMON);
    }
    
    context = lws_create_context(&info);
    if (context == NULL) {
        lwsl_err("lws init failed\n");
        return NULL;
    }
	
    n = 0;
    while (n >= 0 && !force_exit) {
        struct timeval tv;
        gettimeofday(&tv, NULL);
        n = lws_service(context, 50);

        if(session_cnt > 0)
          lws_callback_on_writable_all_protocol(context, protocols);
  
        usleep(10*1000); // about 30fps
    }

    lws_context_destroy(context);
    lwsl_notice("lws-test-server exited cleanly\n");
    return NULL;
}

#endif