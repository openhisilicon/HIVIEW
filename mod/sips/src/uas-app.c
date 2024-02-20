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

#include "time64.h"
#include "fw/comm/inc/netcfg.h"

//for json cfg;
#include "mod/sips/inc/sjb_sips.h"
extern gsf_sips_t sips_parm;

#include "uas-device.h"


//for xml binding;
#include "sxb_sips.h"

static int listen_port = 5060;

struct sip_uac_transport_address_t
{
	st_netfd_t udp;
	st_netfd_t tcp;
	socklen_t addrlen;
	struct sockaddr_storage addr;
  char local[64];
};


struct sip_uas_app_t
{
  st_netfd_t tcp;
	st_netfd_t udp;
	socklen_t addrlen;
	struct sockaddr_storage addr;

	http_parser_t* request;
	http_parser_t* response;
	struct sip_agent_t* sip;
	
	int register_runing;
	int registed;
  int expired, keepalive;
  char usr[64];
  char pwd[64];
  char host[64];
  char from[64];
  struct sip_uac_transport_address_t transport; // for register
	struct http_header_www_authenticate_t auth;
	int nonce_count;
	char callid[64];
	int cseq;
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


static int sip_uac_transport_via(void* transport, const char* destination, char protocol[16], char local[128], char dns[128])
{
	int r;
	char ip[65];
	u_short port;
	struct uri_t* uri;

	struct sip_uac_transport_address_t *t = (struct sip_uac_transport_address_t *)transport;

	// rfc3263 4.1 Selecting a Transport Protocol
	// 1. If the URI specifies a transport protocol in the transport parameter,
	//    that transport protocol SHOULD be used. Otherwise, if no transport 
	//    protocol is specified, but the TARGET(maddr) is a numeric IP address, 
	//    the client SHOULD use UDP for a SIP URI, and TCP for a SIPS URI.
	// 2. if no transport protocol is specified, and the TARGET is not numeric, 
	//    but an explicit port is provided, the client SHOULD use UDP for a SIP URI, 
	//    and TCP for a SIPS URI
	// 3. Otherwise, if no transport protocol or port is specified, and the target 
	//    is not a numeric IP address, the client SHOULD perform a NAPTR query for 
	//    the domain in the URI.

	// The client SHOULD try the first record. If an attempt should fail, based on 
	// the definition of failure in Section 4.3, the next SHOULD be tried, and if 
	// that should fail, the next SHOULD be tried, and so on.

	t->addrlen = sizeof(t->addr);
	memset(&t->addr, 0, sizeof(t->addr));
	strcpy(protocol, NULL == t->tcp ? "UDP" : "TCP");

	uri = uri_parse(destination, strlen(destination));
	if (!uri)
		return -1; // invalid uri

	// rfc3263 4-Client Usage (p5)
	// once a SIP server has successfully been contacted (success is defined below), 
	// all retransmissions of the SIP request and the ACK for non-2xx SIP responses 
	// to INVITE MUST be sent to the same host.
	// Furthermore, a CANCEL for a particular SIP request MUST be sent to the same 
	// SIP server that the SIP request was delivered to.

	// TODO: sips port
	r = socket_addr_from(&t->addr, &t->addrlen, uri->host, uri->port ? uri->port : listen_port);
	if (0 == r)
	{
		socket_addr_to((struct sockaddr*)&t->addr, t->addrlen, ip, &port);
		socket_getname(NULL == t->tcp ? st_netfd_fileno(t->udp) : st_netfd_fileno(t->tcp), local, &port);
		sprintf(local, "%s", t->local);
		
		//maohw r = ip_route_get(ip, local);
		if (0 == r)
		{
			dns[0] = 0;
			struct sockaddr_storage ss;
			socklen_t len = sizeof(ss);
			if (0 == socket_addr_from(&ss, &len, local, port))
				socket_addr_name((struct sockaddr*)&ss, len, dns, 128);

			//if (listen_port != port)
				snprintf(local + strlen(local), 128 - strlen(local), ":%hu", port);

			//if (NULL == strchr(dns, '.'))
				snprintf(dns, 128, "%s", local); // don't have valid dns
		}
    
	}
	uri_free(uri);
	return r;
}

static int sip_uac_transport_send(void* transport, const void* data, size_t bytes)
{
	struct sip_uac_transport_address_t *t = (struct sip_uac_transport_address_t *)transport;

	//char p1[1024];
	//char p2[1024];
	((char*)data)[bytes] = 0;
	printf("%s=[\n%s]\n", __func__, (const char*)data);
	int r = st_sendto(NULL == t->tcp ? t->udp : t->tcp, data, bytes, (struct sockaddr*)&t->addr, t->addrlen, ST_UTIME_NO_TIMEOUT);
	return r == bytes ? 0 : -1;
}


static int sip_uac_onmessage(void* param, const struct sip_message_t* reply, struct sip_uac_transaction_t* t, int code)
{
  printf("%s => code:%d\n", __func__, code);
	return 0;
}

static int sip_uac_sendmessage(struct sip_uas_app_t *app, const char *msg)
{
	struct sip_uac_transaction_t  *t = sip_uac_message(app->sip, app->from, app->host, sip_uac_onmessage, app);
	
  sip_uac_add_header(t, "Content-Type", "Application/MANSCDP+xml");
	
  struct sip_transport_t transport = {
			sip_uac_transport_via,
			sip_uac_transport_send,
	};
	
	assert(0 == sip_uac_send(t, msg, strlen(msg), &transport, &app->transport));

	return sip_uac_transaction_release(t);
}


static int sip_uas_transport_send(void* param, const struct cstring_t* protocol, const struct cstring_t* url, const struct cstring_t* received, int rport, const void* data, int bytes)
{
	struct sip_uas_app_t *app = (struct sip_uas_app_t *)param;

	printf("st_sendto => \n[%.*s]\n", (int)bytes, (const char*)data);
	int r = st_sendto(app->udp, data, bytes, (struct sockaddr*)&app->addr, app->addrlen, ST_UTIME_NO_TIMEOUT);
	
	return r == bytes ? 0 : -1;
}

static int sip_uas_parse_chn(const struct sip_message_t* req)
{
  //34020000001310000001@192.168.0.13:5060
  for(int i = 0; i < sips_parm.chnum; i++)
  {
    char token[64];
    sprintf(token, "34020000001310%06d", i+1);
    if(strstr(req->u.c.uri.host.p, token))
      return i;
  }

  return 0; 
}


static void* sip_uas_oninvite(void* param, const struct sip_message_t* req, struct sip_uas_transaction_t* t, struct sip_dialog_t* dialog, const void* data, int bytes)
{
  int chn = sip_uas_parse_chn(req);
  printf("%s => t:%p, request:[%s], chn:%d\n", __func__, t, req->u.c.uri.host.p, chn);
  
	char reply[1024];
	const struct cstring_t* h = sip_message_get_header_by_name(req, "Content-Type");
	if (0 == cstrcasecmp(h, "Application/SDP"))
	{
		socklen_t len = 0;
		
		struct sip_media_t* m = calloc(1, sizeof(struct sip_media_t));

		m->nmedia = rtsp_media_sdp((const char*)data, m->medias, sizeof(m->medias) / sizeof(m->medias[0]));
		assert(m->nmedia > 0);
		assert(0 == strcasecmp("IP4", m->medias[0].addrtype) || 0 == strcasecmp("IP6", m->medias[0].addrtype));

    m->media = rtp_media_live_new(chn, 0);
		
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
	          
          //a=setup:active
          //a=connection:new
        	int active  = strstr(data, "a=setup:active")?0:1;
        	  
	        int ret = m->media->add_transport(m->media, "video"
	                  , strstr(m->medias[i].proto, "TCP")?rtp_tcp_transport_new(m->medias[i].address, m->port, active):
	                          rtp_udp_transport_new(m->medias[i].address, m->port));
	                          
		      printf("i:%d, media video local port[%d,%d]\n", i, m->port[0], m->port[1]);
		   }
		}
		
		
    /* SSRC */
  	char *_ssrc = strstr(data, "y=");
  	uint32_t ssrc = 0x5F5E4E9; //"y=0100001001"
  	if(_ssrc)
  	{
  		sscanf(_ssrc, "y=%d", &ssrc);
	  }
		

    // reply;
	  const char* pattern_live = "v=0\r\n"
    "o=34020000001110000001 0 0 IN IP4 %s\r\n"
    "s=Talk\r\n"
    "c=IN IP4 %s\r\n"
    "t=0 0\r\n"
    ;

    char *name = "34020000001110000001";
    char *_host = strstr(req->to.uri.host.p, "@");
    char *_port = (_host)?strstr(_host, ":"):NULL;
    
    char host[128] = {0};
    char port[16] = {0};
    if(_host && _port)
      sscanf(_host, "@%127[^:]", host);
    if(!strlen(host))
    {
      strcpy(host, "0.0.0.0");
    }
    if(_port)
      sscanf(_port, ":%15[^>]", port);
    if(!strlen(port))
    {
      strcpy(port, "5060");
    }
    
    char sdp[2048] = {0};
    snprintf(sdp, sizeof(sdp), pattern_live, host, host);
    
    #if 0 //m->port[0-1];
    m->media->get_sdp(m->media, sdp, strstr(data, "PS/90000")?1:0, ssrc);
    #else
    {
      m->media->get_sdp(m->media, NULL, strstr(data, "PS/90000")?1:0, ssrc);
        
      const char* pattern_video =
        "m=video %d RTP/AVP 96\r\n"
        "a=sendonly\r\n"
        "a=rtpmap:96 PS/90000\r\n"
        "y=%d\r\n"
        ;
      char sdp_video[256] = {0};  
      snprintf(sdp_video, sizeof(sdp_video), pattern_video, m->port[0], ssrc);
      strcat(sdp, sdp_video);
    }
    #endif
    
    char contact[128];
    sprintf(contact, "sip:%s@%s:%s", name, host, port);
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
  printf("%s => t:%p, m:%p\n", __func__, t, m);
  
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

static int sip_uas_onmessage(void* param, const struct sip_message_t* req, struct sip_uas_transaction_t* t, void* session, const void* payload, int bytes)
{
  struct sip_uas_app_t *app = (struct sip_uas_app_t *)param;
  printf("%s => payload[\n%s]\n", __func__, req, t, payload);
	sip_uas_reply(t, 200, NULL, 0);
	
	if(!payload || !strlen(payload))
    return 0;
    
  char buf[64*1024];
  int len = sizeof(buf);
  
  if(strstr(payload, "<CmdType>DeviceInfo</CmdType>"))
  {
    mxml_node_t* xml = mxmlNewXML("1.0");
    mxml_node_t* root = mxmlNewElement(xml, "Response");

    DeviceInfo info = {
      .CmdType = "DeviceInfo",
      .SN = 17430,
      .DeviceID = "34020000001110000001",
      .Result = "OK",
      .DeviceName = "GB28181Device",
      .Manufacturer = "HIVIEW-TECH.CN",
      .Model = "Model",
      .Firmware = "Firmware",
    };
    sxb_bind_DeviceInfo(root, 0, &info, 0, 0);
    mxmlSaveString(xml, buf, len, whitespace_cb);
  	mxmlDelete(xml);
    sip_uac_sendmessage(app, buf);
  }
  else if(strstr(payload, "<CmdType>Catalog</CmdType>"))
  {
    mxml_node_t* xml = mxmlNewXML("1.0");
    mxml_node_t* root = mxmlNewElement(xml, "Response");
    
    //四个码段共20位十进制数字字符构成,即系统编码 =中心编码(8) + 行业编码(2) + 类型编码(3) + 序号(7)
    //类型编码(3): 111 - DVR, 131 - camera, 134 - alarm-in, 135 - alarm-out;
    
    Catalog catalog;
    memset(&catalog, 0, sizeof(catalog));
    strncpy(catalog.CmdType, "Catalog", sizeof(catalog.CmdType)-1);
    catalog.SN = 2;
    strncpy(catalog.DeviceID, "34020000001110000001", sizeof(catalog.DeviceID)-1);
    
    catalog.SumNum = sips_parm.chnum+1;
    ELE_NUM_SET(catalog.DeviceList) = sips_parm.chnum+1;
    int i = 0;
    for(i = 0; i < sips_parm.chnum+1; i++)
    {
      if(i==0)
      {
        snprintf(catalog.DeviceList[i].DeviceID, sizeof(catalog.DeviceList[i].DeviceID), "%s", "34020000001340000001");
        snprintf(catalog.DeviceList[i].Name, sizeof(catalog.DeviceList[i].Name), "%s", "Alarm");
      }
      else
      {
        snprintf(catalog.DeviceList[i].DeviceID, sizeof(catalog.DeviceList[i].DeviceID), "34020000001310%06d", i);
        snprintf(catalog.DeviceList[i].Name, sizeof(catalog.DeviceList[i].Name), "IPC%02d", i);
      }
      
      strncpy(catalog.DeviceList[i].Manufacturer, "HIVIEW-TECH.CN", sizeof(catalog.DeviceList[i].Manufacturer)-1);
      strncpy(catalog.DeviceList[i].Model, "Model", sizeof(catalog.DeviceList[i].Model)-1);
      strncpy(catalog.DeviceList[i].Owner, "Owner", sizeof(catalog.DeviceList[i].Owner)-1);
      strncpy(catalog.DeviceList[i].CivilCode, "34020000", sizeof(catalog.DeviceList[i].CivilCode)-1);
      strncpy(catalog.DeviceList[i].Block, "Block", sizeof(catalog.DeviceList[i].Block)-1);
      strncpy(catalog.DeviceList[i].Address, "Address", sizeof(catalog.DeviceList[i].Address)-1);
      
      strncpy(catalog.DeviceList[i].Parental, "0", sizeof(catalog.DeviceList[i].Parental)-1);
      strncpy(catalog.DeviceList[i].ParentID, "34020000001110000001", sizeof(catalog.DeviceList[i].ParentID)-1);
      strncpy(catalog.DeviceList[i].EndTime, "2021-07-15T09:47:40.782", sizeof(catalog.DeviceList[i].EndTime)-1);
      strncpy(catalog.DeviceList[i].Status, "ON", sizeof(catalog.DeviceList[i].Status)-1);
      
      catalog.DeviceList[i].Secrecy = 0;
      catalog.DeviceList[i].RegisterWay = 1;
    }
    sxb_bind_Catalog(root, 0, &catalog, 0, 0);
    mxmlSaveString(xml, buf, len, whitespace_cb);
  	mxmlDelete(xml);
    sip_uac_sendmessage(app, buf);
  }
  else if(strstr(payload, "<CmdType>DeviceControl</CmdType>"))
  {
    #define CTRL_RESP		"<?xml version=\"1.0\"?>\r\n" \
						"<Response>\r\n" \
						"<CmdType>DeviceControl</CmdType>\r\n" \
						"<SN>%s</SN>\r\n" \
						"<DeviceID>%s</DeviceID>\r\n" \
						"<Result>%s</Result>\r\n" \
						"</Response>\r\n"
    
    char *p = NULL;
    
    if(p = strstr(payload, "<SN>"))
    {
      ;
    }
    if(p = strstr(payload, "<DeviceID>"))
    {
      ;
    }
    
    if(p = strstr(payload, "<PTZCmd>"))
    {
      ptzcmd_ctl(0, p+strlen("<PTZCmd>"));
      //ptz and reboot is do not sendmsg;  
    }
  }  
  return 0;
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
        
		printf("st_recvfrom => r:%d, \n[%s]\n", r, buffer);
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


static void sip_uac_keepalive_do(struct sip_uas_app_t *app)
{
	const char* msg = "<?xml version=\"1.0\" encoding=\"GB2312\"?>"
						"<Notify>"
						"<CmdType>Keepalive</CmdType>"
						"<SN>1</SN>"
						"<DeviceID>34020000001110000001</DeviceID>"
						"<Status>OK</Status>"
						"</Notify>";

	sip_uac_sendmessage(app, msg);
}



static void sip_uac_register_do(struct sip_uas_app_t *app);
static int sip_uac_onregister(void* param, const struct sip_message_t* reply, struct sip_uac_transaction_t* t, int code)
{
	struct sip_uas_app_t *app = (struct sip_uas_app_t *)param;

	const struct cstring_t* h;
	if (0 == app->callid[0] && reply)
	{
		h = sip_message_get_header_by_name(reply, "Call-ID");
		if (h)
		{
			snprintf(app->callid, sizeof(app->callid), "%.*s", (int)h->n, h->p);
			h = sip_message_get_header_by_name(reply, "CSeq");
			app->cseq = atoi(h->p);
		}
	}

	if (200 <= code && code < 300)
	{
		printf("Register OK\n");
		app->registed = 1;
	}
	else if (401 == code)
	{
		printf("Unauthorized\n");
		memset(&app->auth, 0, sizeof(app->auth));
		h = sip_message_get_header_by_name(reply, "WWW-Authenticate");
		assert(0 == http_header_www_authenticate(h->p, &app->auth));
		app->nonce_count = 0;
		switch (app->auth.scheme)
		{
		case HTTP_AUTHENTICATION_DIGEST:
			sip_uac_register_do(app);
			break;

		case HTTP_AUTHENTICATION_BASIC:
			assert(0);
			break;

		default:
			assert(0);
		}
	}
	else
	{
	}
	return 0;
}

static void sip_uac_register_do(struct sip_uas_app_t *app)
{
	char buffer[256];
	
	//printf("%s => host:[%s], from[%s]\n", __func__, app->host, app->from);
	
	//t = sip_uac_register(uac, "Bob <sip:bob@biloxi.com>", "sip:registrar.biloxi.com", 7200, sip_uac_message_onregister, app);
	struct sip_uac_transaction_t *t = sip_uac_register(app->sip, app->from, app->host, app->expired, sip_uac_onregister, app);
  
  app->registed = 0;
  
  if(1)
  {
    char contact[128];
    sprintf(contact, "sip:%s@%s:%d", app->usr, app->transport.local, listen_port);
		sip_uac_add_header(t, "Contact", contact);
  }
  
	if (app->callid[0])
	{
		// All registrations from a UAC SHOULD use the same Call-ID
		sip_uac_add_header(t, "Call-ID", app->callid);

		snprintf(buffer, sizeof(buffer), "%u REGISTER", ++app->cseq);
		sip_uac_add_header(t, "CSeq", buffer);
	}

	if (HTTP_AUTHENTICATION_DIGEST == app->auth.scheme)
	{
		// https://blog.csdn.net/yunlianglinfeng/article/details/81109380
		// http://www.voidcn.com/article/p-oqqbqgvd-bgn.html
		++app->auth.nc;
		snprintf(app->auth.uri, sizeof(app->auth.uri), "%s", app->host);
		snprintf(app->auth.username, sizeof(app->auth.username), "%s", app->usr);
		http_header_auth(&app->auth, app->pwd, "REGISTER", NULL, 0, buffer, sizeof(buffer));
		sip_uac_add_header(t, "Authorization", buffer);
	}

	struct sip_transport_t transport = {
			sip_uac_transport_via,
			sip_uac_transport_send,
	};
	assert(0 == sip_uac_send(t, NULL, 0, &transport, &app->transport));
	
	sip_uac_transaction_release(t);
}

static void *register_task(void *arg)
{
  uint64_t regclock = 0, aliveclock = 0;
  struct sip_uas_app_t *app = (struct sip_uas_app_t*)arg;
  
  while(app->register_runing)
  {
    uint64_t now = time64_now();
		
		
		if (regclock + app->expired * 1000 < now)
		{
			regclock = now;
			sip_uac_register_do(app);
		}
		
    if (app->registed && aliveclock + app->keepalive * 1000 < now)
		{
			aliveclock = now;
			sip_uac_keepalive_do(app);
		}
		
    st_sleep(1);
  }
  
  printf("%s => exit app:%p .\n", __func__, app);
  st_thread_exit(NULL);
  return NULL;
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
  handler.onmessage = sip_uas_onmessage;

	struct sip_uas_app_t app;
	memset(&app, 0, sizeof(app));
	app.sip = sip_agent_create(&handler, &app);
	app.request = http_parser_create(HTTP_PARSER_CLIENT);
	app.response = http_parser_create(HTTP_PARSER_SERVER);

  app.tcp = NULL;
  listen_port = sips_parm.port?sips_parm.port:listen_port;
	app.udp = socket_listen_udp(listen_port);
	
	app.register_runing = 1;
  app.transport.udp = app.udp;
	app.transport.tcp = app.tcp;
	sprintf(app.transport.local, "192.168.0.2");
	netcfg_get_ip_addr("eth0",  app.transport.local);
	
	sprintf(app.usr, "%s", strlen(sips_parm.device)?sips_parm.device:"34020000001110000001");
	sprintf(app.pwd, "%s", strlen(sips_parm.password)?sips_parm.password:"12345678");
	sprintf(app.host,"%s", strlen(sips_parm.host)?sips_parm.host:"sip:34020000002000000001@192.168.0.166:5061");
	  
	app.expired = sips_parm.expired?sips_parm.expired:3600;
	app.keepalive = sips_parm.keepalive?sips_parm.keepalive:60;
	  
  sprintf(app.from, "sip:%s@3402000000", app.usr);
	
	if(sips_parm.enable)
	{
	  st_thread_t tid = st_thread_create(register_task, &app, 0, 0);
	}
	
	sip_uas_loop(&app);
	
	sip_agent_destroy(app.sip);
	
	st_netfd_close(app.udp);
	
	http_parser_destroy(app.request);
	http_parser_destroy(app.response);
	sip_timer_cleanup();
}
