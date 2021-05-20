#include "cstring.h"
#include "sip-uac.h"
#include "sip-uas.h"
#include "sip-message.h"
#include "sip-transport.h"
#include "sip-timer.h"

#include "http-parser.h"
#include "http-header-auth.h"
#include "rtsp-media.h"

#include "uri-parse.h"
#include "cstringext.h"
#include "base64.h"
#include "sdp.h"
#include "md5.h"

#include "rtp-media.h"
#include "rtp-socket.h"
#include "fw/comm/inc/uthash.h"

struct sip_uas_app_t
{
	st_netfd_t udp;
	socklen_t addrlen;
	struct sockaddr_storage addr;

	http_parser_t* request;
	http_parser_t* response;
	struct sip_agent_t* sip;
};

struct sip_media_t
{
  UT_hash_handle hh;
	struct rtsp_media_t medias[3];
	int nmedia;

  struct rtp_media_t *media;
	unsigned short port[2];
};
static struct sip_media_t *mtabs;


static int sip_uas_transport_send(void* param, const struct cstring_t* protocol, const struct cstring_t* url, const struct cstring_t* received, int rport, const void* data, int bytes)
{
	struct sip_uas_app_t *app = (struct sip_uas_app_t *)param;

	printf("st_sendto => \n[%.*s]\n", (int)bytes, (const char*)data);
	int r = st_sendto(app->udp, data, bytes, (struct sockaddr*)&app->addr, app->addrlen, ST_UTIME_NO_TIMEOUT);
	
	return r == bytes ? 0 : -1;
}

static void* sip_uas_oninvite(void* param, const struct sip_message_t* req, struct sip_uas_transaction_t* t, struct sip_dialog_t* dialog, const void* data, int bytes)
{
  printf("%s => bytes:%d\n[%s]\n", __func__, bytes, data);
  
	char reply[1024];
	const struct cstring_t* h = sip_message_get_header_by_name(req, "Content-Type");
	if (0 == cstrcasecmp(h, "Application/SDP"))
	{
		socklen_t len = 0;
		
		struct sip_media_t* m = calloc(1, sizeof(struct sip_media_t));

		m->nmedia = rtsp_media_sdp((const char*)data, m->medias, sizeof(m->medias) / sizeof(m->medias[0]));
		assert(m->nmedia > 0);
		assert(0 == strcasecmp("IP4", m->medias[0].addrtype) || 0 == strcasecmp("IP6", m->medias[0].addrtype));

    m->media = rtp_media_live_new(0, 0);
		
		int i;
		for(i = 0; i < m->nmedia; i++)
		{
		  printf("i:%d, media:[%s], proto:[%s], address:[%s], nport:%d, port[%d,%d]\n"
		    , i
		    , m->medias[i].media, m->medias[i].proto, m->medias[i].address
		    , m->medias[i].nport, m->medias[i].port[0], m->medias[i].port[1]);
		    
		   if(strstr(m->medias[i].media, "video"))
		   {
	    		m->port[0] = m->medias[i].port[0];
	        m->port[1] = m->medias[i].nport > 1 ? m->medias[i].port[1] : (m->medias[i].port[0] + 1);
		      int ret = m->media->add_transport(m->media, "video", rtp_udp_transport_new(m->medias[i].address, m->port));  
		   }
		}
		
		
    // reply;
	  const char* pattern_live = "v=0\r\n"
    "o=HTRS 3780 2519 IN IP4 %s\r\n"
    "s=Talk\r\n"
    "c=IN IP4 %s\r\n"
    "t=0 0\r\n"
    ;
    
    char *name = "thomas";
    char *host = strstr(req->to.uri.host.p, "@");
    host = (host)?host+1:"0.0.0.0";
    
    char sdp[2048] = {0};
    snprintf(sdp, sizeof(sdp), pattern_live, host, host);
    
    //m->port[0-1];
    m->media->get_sdp(m->media, sdp);
    
    char contact[128];
    sprintf(contact, "sip:%s@%s", name, host);
		sip_uas_add_header(t, "Content-Type", "application/sdp");
		sip_uas_add_header(t, "Contact", contact);
		assert(0 == sip_uas_reply(t, 200, sdp, strlen(sdp)));
		
		HASH_ADD_PTR(mtabs, media, m);
		return m;
	}
	else
	{
		assert(0);
		return NULL;
	}
}

/// @param[in] code 0-ok, other-sip status code
/// @return 0-ok, other-error
static int sip_uas_onack(void* param, const struct sip_message_t* req, struct sip_uas_transaction_t* t, void* session, struct sip_dialog_t* dialog, int code, const void* data, int bytes)
{
	struct sip_media_t* m = (struct sip_media_t*)session;
  printf("%s => m:%p code:%d\n", __func__, m, code);
  
  // check m is alive;
  if(m)
  {
    HASH_FIND_PTR(mtabs, &m->media, m);
  }
  
	if (200 <= code && code < 300)
	{
	  if(m && m->media)
	  {
		  m->media->play(m->media);
		}
	}
	else
	{
	  if(m)
	  {
	    if(m->media)
	    {
		    rtp_media_live_free(m->media);
		    printf("rtp_media_live_free m->media:%p\n", m->media);
		  }
		  HASH_DEL(mtabs, m);
		  free(m);
		}
	}
	return 0;
}

/// on terminating a session(dialog)
static int sip_uas_onbye(void* param, const struct sip_message_t* req, struct sip_uas_transaction_t* t, void* session)
{
  struct sip_media_t* m = (struct sip_media_t*)session;
  printf("%s => m:%p\n", __func__, m);
  
  // check m is alive;
  if(m)
  {
    HASH_FIND_PTR(mtabs, &m->media, m);
  }
  
  if(m)
  {
    if(m->media)
    {
	    rtp_media_live_free(m->media);
	    printf("rtp_media_live_free m->media:%p\n", m->media);
	  }
	  HASH_DEL(mtabs, m);
	  free(m);  
  }
  
	return sip_uas_reply(t, 200, NULL, 0);
}

/// cancel a transaction(should be an invite transaction)
static int sip_uas_oncancel(void* param, const struct sip_message_t* req, struct sip_uas_transaction_t* t, void* session)
{
  struct sip_media_t* m = (struct sip_media_t*)session;
  printf("%s => m:%p\n", __func__, m);
	return sip_uas_reply(t, 200, NULL, 0);
}

/// @param[in] expires in seconds
static int sip_uas_onregister(void* param, const struct sip_message_t* req, struct sip_uas_transaction_t* t, const char* user, const char* location, int expires)
{
  printf("%s => user:%s\n", __func__, user);
	return sip_uas_reply(t, 200, NULL, 0);
}

static void sip_uas_loop(struct sip_uas_app_t *app)
{
	uint8_t buffer[2*1024];
	http_parser_t* parser;

	do
	{
		memset(buffer, 0, sizeof(buffer));
		app->addrlen = sizeof(app->addr);

    int r = st_recvfrom(app->udp, buffer, sizeof(buffer), (struct sockaddr*)&app->addr, &app->addrlen, 3*1000*1000);    
    if(r <= 4 )
      continue;
        
		//printf("st_recvfrom => r:%d, \n[%s]\n", r, buffer);
		parser = 0 == strncasecmp("SIP", (char*)buffer, 3) ? app->request: app->response;

    size_t n = r;
    assert(0 == http_parser_input(parser, buffer, &n));
    struct sip_message_t* msg = sip_message_create(parser==app->response? SIP_MESSAGE_REQUEST : SIP_MESSAGE_REPLY);
    assert(0 == sip_message_load(msg, parser));
    assert(0 == sip_agent_input(app->sip, msg));
    sip_message_destroy(msg);
    http_parser_clear(parser);
    
	} while (1);
}



st_netfd_t socket_listen_udp(unsigned short port)
{
  int opt, sock, n;
  struct sockaddr_in lcl_addr;
  st_netfd_t srv_nfd;

  memset(&lcl_addr, 0, sizeof(struct sockaddr_in));
  lcl_addr.sin_family = AF_INET;
  lcl_addr.sin_port = htons(port);
  lcl_addr.sin_addr.s_addr = INADDR_ANY;

  if ((sock = socket(PF_INET, SOCK_DGRAM, 0)) < 0) {
    printf("socket err.\n");
    return NULL;
  }
  
  n = 1;
  if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (char *)&n, sizeof(n)) < 0) {
    printf("setsockopt SO_REUSEADDR err.\n");
    return NULL;
  }
  
  if (setsockopt(sock, SOL_SOCKET, SO_REUSEPORT, (char *)&n, sizeof(n))<0) {
    printf("setsockopt SO_REUSEPORT err.\n");
    return NULL;
  }
  
  if (bind(sock, (struct sockaddr *)&lcl_addr, sizeof(lcl_addr)) < 0) {
    printf("bind err.");
    return NULL;
  }

  if ((srv_nfd = st_netfd_open_socket(sock)) == NULL) {
    printf("st_netfd_open err.");
    return NULL;
  }
  
  return srv_nfd;
}


void sip_uas_app(void)
{
	sip_timer_init();
	
	struct sip_uas_handler_t handler;
	handler.onregister = sip_uas_onregister;
	handler.oninvite = sip_uas_oninvite;
	handler.onack = sip_uas_onack;
	handler.onbye = sip_uas_onbye;
	handler.oncancel = sip_uas_oncancel;
	handler.send = sip_uas_transport_send;

	struct sip_uas_app_t app;
	
	app.sip = sip_agent_create(&handler, &app);
	app.request = http_parser_create(HTTP_PARSER_CLIENT);
	app.response = http_parser_create(HTTP_PARSER_SERVER);

	app.udp = socket_listen_udp(SIP_PORT);
	
	sip_uas_loop(&app);
	
	sip_agent_destroy(app.sip);
	
	st_netfd_close(app.udp);
	
	http_parser_destroy(app.request);
	http_parser_destroy(app.response);
	sip_timer_cleanup();
}
