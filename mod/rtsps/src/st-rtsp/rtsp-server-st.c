#include "ctypedef.h"
#include "cstringext.h"
#include "ntp-time.h"
#include "rtp-profile.h"
#include "rtsp-server.h"
#include "uri-parse.h"
#include "urlcodec.h"
#include "fw/libhttp/inc/http-header-auth.h"
#include "rtsp-server-st.h"
#include "cfg.h"

//#include "rtp-udp-transport.h"
//#include "rtp-tcp-transport.h"

static int rtsp_uri_parse(const char* uri, char* path)
{
	char path1[256];
	struct uri_t* r = uri_parse(uri, strlen(uri));
	if(!r)
		return -1;

	url_decode(r->path, strlen(r->path), path1, sizeof(path1));
	strncpy(path ,path1 ,256);
	uri_free(r);
	return 0;
}

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



static int rtsp_auth(struct rtsp_session_t *sess, rtsp_server_t* rtsp, const char* uri)
{  
  if(!rtsps_parm.auth)
    return 0;

  static gsf_user_t userlist[8];
  if(userlist[0].name[0] == '\0')
  {
    getuser(userlist, 8);
  }

  char* auth_hdr = rtsp_server_get_header(rtsp, "Authorization");
  if(!auth_hdr)
__401:
  {
    int n = 0;
  	char header[1024] = { 0 };
		n += snprintf(header + n, sizeof(header) - n, "WWW-Authenticate: Digest realm=\"8ce748e83087\", nonce=\"f54f0eb78282e0071eb3747eb278682b\", stale=\"FALSE\"\r\n");
    rtsp_server_reply2(rtsp, 401, header, NULL, 0);
    //printf("401:[%s]\n", header);
    return -1;
  }
  
  //only admin user for test;
  
  //printf("auth_hdr:[%s]\n", auth_hdr);
  char *response = strstr(auth_hdr, "response=");
  
  struct http_header_www_authenticate_t authorization;
  memset(&authorization, 0, sizeof(authorization));
  authorization.scheme = HTTP_AUTHENTICATION_DIGEST;
  
  snprintf(authorization.uri, sizeof(authorization.uri), "%s", uri);
  snprintf(authorization.username, sizeof(authorization.username), "%s", userlist[0].name);
  http_header_www_authenticate(auth_hdr, &authorization);
  
  char authorization_out[256] = {0};
  http_header_auth(&authorization, userlist[0].pwd, "DESCRIBE", NULL, 0, authorization_out, sizeof(authorization_out));
  
  //printf("response:[%s], authorization_out:[%s]\n", response, authorization_out);
  if(!response || !strstr(authorization_out, response))
  {
    goto __401;
  }
  
  
  return 0;
}



static int rtsp_ondescribe(void* ptr, rtsp_server_t* rtsp, const char* uri)
{
  struct rtsp_session_t *sess = (struct rtsp_session_t*)ptr;
  
  if(rtsp_auth(sess, rtsp, uri) != 0)
    return 0;
  
	static const char* pattern_live =
		"v=0\n"
		"o=- %llu %llu IN IP4 %s\n"
		"s=%s\n"
		"c=IN IP4 0.0.0.0\n"
		"t=0 0\n"
		"a=control:*\n";

  int ch = 0, sid = 0;
  char filename[256] = {0};
	rtsp_uri_parse(uri, filename);
	printf("%s => ptr:%p, uri:[%s], filename[%s]\n", __func__, ptr, uri, filename);
  sscanf(filename, "/%d/%d", &ch, &sid);
  ch  = (ch < 0 || ch > 255)?0:ch;
  sid = (sid < 0 || sid > 255)?0:sid; 
  
  if(!sess->media)
    sess->media = rtp_media_live_new(ch, sid);
  	
  char sdp[2048] = {0};
  snprintf(sdp, sizeof(sdp), pattern_live, ntp64_now(), ntp64_now(), "0.0.0.0", uri);

  sess->media->get_sdp(sess->media, sdp);

  return rtsp_server_reply_describe(rtsp, 200, sdp);
}

static int rtsp_onsetup(void* ptr, rtsp_server_t* rtsp, const char* uri, const char* session, const struct rtsp_header_transport_t transports[], size_t num)
{
  struct rtsp_session_t *sess = (struct rtsp_session_t*)ptr;
  
	char rtsp_transport[128];
	const struct rtsp_header_transport_t *transport = NULL;
	
  char filename[256] = {0};
	rtsp_uri_parse(uri, filename);
  char* track = strrchr(filename, '/');
  track = track?track+1:"";
	
	printf("%s => ptr:%p, uri:[%s], filename[%s], track[%s], session:[%s]\n"
	      , __func__, ptr, uri, filename, track, session);
	
	if(sess->media == NULL)
	{
	  // 454 Session Not Found
	  return rtsp_server_reply_setup(rtsp, 454, NULL, NULL);
	}
    size_t i;	
	for(i = 0; i < num && !transport; i++)
	{
    printf("%s => ptr:%p, num:%d, transports[%d][transport:%d, multicast:%d]\n\n"
          , __func__, ptr, num, i, transports[i].transport, transports[i].multicast);
    
		if(RTSP_TRANSPORT_RTP_UDP == transports[i].transport)
		{
			// RTP/AVP/UDP
			transport = &transports[i];
		}
		else if(RTSP_TRANSPORT_RTP_TCP == transports[i].transport)
		{
			// RTP/AVP/TCP
			// 10.12 Embedded (Interleaved) Binary Data (p40)
			transport = &transports[i];
		}
		else if(transports[i].multicast)
		{
		  ;
		}
	}
	if(!transport)
	{
		// 461 Unsupported Transport
		return rtsp_server_reply_setup(rtsp, 461, NULL, NULL);
	}

	if (RTSP_TRANSPORT_RTP_TCP == transport->transport)
	{
		// 10.12 Embedded (Interleaved) Binary Data (p40)
		int interleaved[2];
		if (transport->interleaved1 == transport->interleaved2)
		{
			interleaved[0] = sess->channel++;
			interleaved[1] = sess->channel++;
		}
		else
		{
			interleaved[0] = transport->interleaved1;
			interleaved[1] = transport->interleaved2;
		}

    int ret = sess->media->add_transport(sess->media, track
                    , rtp_tcp_transport_new(rtsp, interleaved[0], interleaved[1]));
		if(ret < 0)
		{
		  // 451 Invalid parameter
	    return rtsp_server_reply_setup(rtsp, 451, NULL, NULL);
		}
		
		// RTP/AVP/TCP;interleaved=0-1
		snprintf(rtsp_transport, sizeof(rtsp_transport), "RTP/AVP/TCP;interleaved=%d-%d", interleaved[0], interleaved[1]);		
	}
	else if(transport->multicast)
	{
		// RFC 2326 1.6 Overall Operation p12
		// Multicast, client chooses address
		// Multicast, server chooses address
		assert(0);
		// 461 Unsupported Transport
		return rtsp_server_reply_setup(rtsp, 461, NULL, NULL);
	}
	else
	{
		// unicast
		assert(transport->rtp.u.client_port1 && transport->rtp.u.client_port2);
		unsigned short port[2] = { transport->rtp.u.client_port1, transport->rtp.u.client_port2 };
		const char *ip = transport->destination[0] ? transport->destination : rtsp_server_get_client(rtsp, NULL);

    int ret = sess->media->add_transport(sess->media, track, rtp_udp_transport_new(ip, port));
    
    if(ret < 0)
		{
		  // 451 Invalid parameter
	    return rtsp_server_reply_setup(rtsp, 451, NULL, NULL);
		}
    
		// RTP/AVP;unicast;client_port=4588-4589;server_port=6256-6257;destination=xxxx
		snprintf(rtsp_transport, sizeof(rtsp_transport), 
			"RTP/AVP;unicast;client_port=%hu-%hu;server_port=%hu-%hu%s%s", 
			transport->rtp.u.client_port1, transport->rtp.u.client_port2,
			port[0], port[1],
			transport->destination[0] ? ";destination=" : "",
			transport->destination[0] ? transport->destination : "");
	}
	// Session: 813629027556073;timeout=60
  char sessionid[128] = {0};
  sprintf(sessionid, "%p;timeout=%d", ptr, sess->timeout);
	return rtsp_server_reply_setup(rtsp, 200, sessionid, rtsp_transport);
}

static int rtsp_onplay(void* ptr, rtsp_server_t* rtsp, const char* uri, const char* session, const int64_t *npt, const double *scale)
{
  struct rtsp_session_t *sess = (struct rtsp_session_t*)ptr;
	printf("%s => ptr:%p, uri:[%s], session:[%s]\n", __func__, ptr, uri, session);
	
	if(sess->media == NULL)
	{
	  return rtsp_server_reply_play(rtsp, 454, NULL, NULL, NULL);
	}
	
	sess->media->play(sess->media);
	
	// RFC 2326 12.33 RTP-Info (p55)
	// 1. Indicates the RTP timestamp corresponding to the time value in the Range response header.
	// 2. A mapping from RTP timestamps to NTP timestamps (wall clock) is available via RTCP.
	char rtpinfo[512] = { 0 };
	sess->media->get_rtpinfo(sess->media, uri, rtpinfo, sizeof(rtpinfo));
  return rtsp_server_reply_play(rtsp, 200, npt, NULL, rtpinfo);
}

static int rtsp_onpause(void* ptr, rtsp_server_t* rtsp, const char* uri, const char* session, const int64_t* npt)
{
	  printf("%s => ptr:%p, uri:[%s], session:[%s]\n", __func__, ptr, uri, session);
	// 457 Invalid Range
    return rtsp_server_reply_pause(rtsp, 200);
}

static int rtsp_onteardown(void* ptr, rtsp_server_t* rtsp, const char* uri, const char* session)
{
	printf("%s => ptr:%p, uri:[%s], session:[%s]\n", __func__, ptr, uri, session);
	struct rtsp_session_t *sess = (struct rtsp_session_t*)ptr;
	
	if(sess->media)
	{
	  rtp_media_live_free(sess->media);
	  sess->media = NULL;
	}
	
	return rtsp_server_reply_teardown(rtsp, 200);
}

static int rtsp_onannounce(void* ptr, rtsp_server_t* rtsp, const char* uri, const char* sdp)
{
    printf("%s => ptr:%p, uri:[%s]\n", __func__, ptr, uri);
    return rtsp_server_reply_announce(rtsp, 200);
}

static int rtsp_onrecord(void* ptr, rtsp_server_t* rtsp, const char* uri, const char* session, const int64_t *npt, const double *scale)
{
	  printf("%s => ptr:%p, uri:[%s], session:[%s]\n", __func__, ptr, uri, session);
    return rtsp_server_reply_record(rtsp, 200, NULL, NULL);
}

static int rtsp_onoptions(void* ptr, rtsp_server_t* rtsp, const char* uri)
{
	printf("%s => ptr:%p, uri:[%s]\n", __func__, ptr, uri);
	const char* require = rtsp_server_get_header(rtsp, "Require");
	return rtsp_server_reply_options(rtsp, 200);
}

static int rtsp_ongetparameter(void* ptr, rtsp_server_t* rtsp, const char* uri, const char* session, const void* content, int bytes)
{
	printf("%s => ptr:%p, uri:[%s], session:[%s]\n", __func__, ptr, uri, session);
	const char* ctype = rtsp_server_get_header(rtsp, "Content-Type");
	const char* encoding = rtsp_server_get_header(rtsp, "Content-Encoding");
	const char* language = rtsp_server_get_header(rtsp, "Content-Language");
	return rtsp_server_reply_get_parameter(rtsp, 200, NULL, 0);
}

static int rtsp_onsetparameter(void* ptr, rtsp_server_t* rtsp, const char* uri, const char* session, const void* content, int bytes)
{
	printf("%s => ptr:%p, uri:[%s], session:[%s]\n", __func__, ptr, uri, session);
	const char* ctype = rtsp_server_get_header(rtsp, "Content-Type");
	const char* encoding = rtsp_server_get_header(rtsp, "Content-Encoding");
	const char* language = rtsp_server_get_header(rtsp, "Content-Language");
	return rtsp_server_reply_set_parameter(rtsp, 200);
}


static void rtsp_onerror(struct rtsp_session_t* sess, int code)
{
	printf("rtsp_onerror => ptr:%p, code:%d\n", sess, code);
}

void* st_rtsp_server_listen(const char* ip, int port)
{
	struct st_rtsp_handler_t *handler = calloc(1, sizeof(struct st_rtsp_handler_t));

	handler->base.ondescribe = rtsp_ondescribe;
  handler->base.onsetup = rtsp_onsetup;
  handler->base.onplay = rtsp_onplay;
  handler->base.onpause = rtsp_onpause;
  handler->base.onteardown = rtsp_onteardown;
  handler->base.onannounce = rtsp_onannounce;
  handler->base.onrecord = rtsp_onrecord;
  handler->base.onoptions = rtsp_onoptions;
  handler->base.ongetparameter = rtsp_ongetparameter;
  handler->base.onsetparameter = rtsp_onsetparameter;
  handler->onerror = rtsp_onerror;
  
	void* l = rtsp_server_listen(ip, port, handler, NULL);
	if(l == NULL)
	{
	  free(handler);
	}
	
	return l;
}

int st_rtsp_server_unlisten(void* st)
{
  struct rtsp_server_listen_t *l = (struct rtsp_server_listen_t*)st;
  struct st_rtsp_handler_t *handler = l->handler;
  
  rtsp_server_unlisten(st);
  
  free(handler);
  return 0;
}
