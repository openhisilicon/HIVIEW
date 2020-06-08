#include <assert.h>
#include <unistd.h>

#include "nvc.h"
#include "wsseapi.h"

#define MAX_INFO_LEN 100


#define SOAP struct soap *

#if 0

SOAP_NMAC struct Namespace probe_namespaces[] =
{
	{"SOAP-ENV", "http://www.w3.org/2003/05/soap-envelope", "http://schemas.xmlsoap.org/soap/envelope/", NULL},
	{"wsa", "http://schemas.xmlsoap.org/ws/2004/08/addressing", NULL, NULL},
	{"dndl", "http://www.onvif.org/ver10/network/wsdl/DiscoveryLookupBinding", NULL, NULL},
	{"dnrd", "http://www.onvif.org/ver10/network/wsdl/RemoteDiscoveryBinding", NULL, NULL},
	{"d", "http://schemas.xmlsoap.org/ws/2005/04/discovery", NULL, NULL},
	{"dn", "http://www.onvif.org/ver10/network/wsdl", NULL, NULL},
	{"tds", "http://www.onvif.org/ver10/device/wsdl", NULL, NULL},
	{NULL, NULL, NULL, NULL}
};

SOAP_NMAC struct Namespace probe_namespaces[] =
{
	{"SOAP-ENV", "http://www.w3.org/2003/05/soap-envelope", "http://schemas.xmlsoap.org/soap/envelope/", NULL},
	{"SOAP-ENC", "http://www.w3.org/2003/05/soap-encoding", "http://schemas.xmlsoap.org/soap/encoding/", NULL},
	{"xsi", "http://www.w3.org/2001/XMLSchema-instance", "http://www.w3.org/*/XMLSchema-instance", NULL},
	{"xsd", "http://www.w3.org/2001/XMLSchema", "http://www.w3.org/*/XMLSchema", NULL},
	{"wsa", "http://schemas.xmlsoap.org/ws/2004/08/addressing", NULL, NULL},
	{"dndl", "http://www.onvif.org/ver10/network/wsdl/DiscoveryLookupBinding", NULL, NULL},
	{"dnrd", "http://www.onvif.org/ver10/network/wsdl/RemoteDiscoveryBinding", NULL, NULL},
	{"d", "http://schemas.xmlsoap.org/ws/2005/04/discovery", NULL, NULL},
	{"dn", "http://www.onvif.org/ver10/network/wsdl", NULL, NULL},
	{"tds", "http://www.onvif.org/ver10/device/wsdl", NULL, NULL},
	{NULL, NULL, NULL, NULL}
};
#endif

SOAP_NMAC struct Namespace probe_namespaces[] =
{
	{"SOAP-ENV", "http://www.w3.org/2003/05/soap-envelope", "http://schemas.xmlsoap.org/soap/envelope/", NULL},
	{"xsi", "http://www.w3.org/2001/XMLSchema-instance", "http://www.w3.org/*/XMLSchema-instance", NULL},
	{"xsd", "http://www.w3.org/2001/XMLSchema", "http://www.w3.org/*/XMLSchema", NULL},
	{"wsa", "http://schemas.xmlsoap.org/ws/2004/08/addressing", NULL, NULL},
	{"d", "http://schemas.xmlsoap.org/ws/2005/04/discovery", NULL, NULL},
    {"tds", "http://www.onvif.org/ver10/device/wsdl", NULL, NULL},
    {"dn", "http://www.onvif.org/ver10/network/wsdl", NULL, NULL},
	{NULL, NULL, NULL, NULL}
};

SOAP_NMAC struct Namespace namespaces[] =
{
	{"SOAP-ENV", "http://www.w3.org/2003/05/soap-envelope", "http://schemas.xmlsoap.org/soap/envelope/", NULL},
	{"SOAP-ENC", "http://www.w3.org/2003/05/soap-encoding", "http://schemas.xmlsoap.org/soap/encoding/", NULL},
	{"xsi", "http://www.w3.org/2001/XMLSchema-instance", "http://www.w3.org/*/XMLSchema-instance", NULL},
	{"xsd", "http://www.w3.org/2001/XMLSchema", "http://www.w3.org/*/XMLSchema", NULL},
	{"wsa", "http://schemas.xmlsoap.org/ws/2004/08/addressing", NULL, NULL},
	{"wsa5", "http://www.w3.org/2005/08/addressing", "http://schemas.xmlsoap.org/ws/2004/08/addressing", NULL},
	{"c14n", "http://www.w3.org/2001/10/xml-exc-c14n#", NULL, NULL},
	{"wsu", "http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-wssecurity-utility-1.0.xsd", NULL, NULL},
	{"xenc", "http://www.w3.org/2001/04/xmlenc#", NULL, NULL},
	{"ds", "http://www.w3.org/2000/09/xmldsig#", NULL, NULL},
	{"wsse", "http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-wssecurity-secext-1.0.xsd", "http://docs.oasis-open.org/wss/oasis-wss-wssecurity-secext-1.1.xsd", NULL},
	{"xmime", "http://tempuri.org/xmime.xsd", NULL, NULL},
	{"xop", "http://www.w3.org/2004/08/xop/include", NULL, NULL},
	{"wsbf2", "http://docs.oasis-open.org/wsrf/bf-2", NULL, NULL},
	{"tt", "http://www.onvif.org/ver10/schema", NULL, NULL},
	{"wstop", "http://docs.oasis-open.org/wsn/t-1", NULL, NULL},
	{"wsr2", "http://docs.oasis-open.org/wsrf/r-2", NULL, NULL},
	{"daae", "http://www.onvif.org/ver20/analytics/wsdl/AnalyticsEngineBinding", NULL, NULL},
	{"dare", "http://www.onvif.org/ver20/analytics/wsdl/RuleEngineBinding", NULL, NULL},
	{"tan", "http://www.onvif.org/ver20/analytics/wsdl", NULL, NULL},
	{"decpp", "http://www.onvif.org/ver10/events/wsdl/CreatePullPointBinding", NULL, NULL},
	{"dee", "http://www.onvif.org/ver10/events/wsdl/EventBinding", NULL, NULL},
	{"denc", "http://www.onvif.org/ver10/events/wsdl/NotificationConsumerBinding", NULL, NULL},
	{"denf", "http://www.onvif.org/ver10/events/wsdl/NotificationProducerBinding", NULL, NULL},
	{"depp", "http://www.onvif.org/ver10/events/wsdl/PullPointBinding", NULL, NULL},
	{"depps", "http://www.onvif.org/ver10/events/wsdl/PullPointSubscriptionBinding", NULL, NULL},
	{"tev", "http://www.onvif.org/ver10/events/wsdl", NULL, NULL},
	{"depsm", "http://www.onvif.org/ver10/events/wsdl/PausableSubscriptionManagerBinding", NULL, NULL},
	{"wsnt", "http://docs.oasis-open.org/wsn/b-2", NULL, NULL},
	{"desm", "http://www.onvif.org/ver10/events/wsdl/SubscriptionManagerBinding", NULL, NULL},
	{"dndl", "http://www.onvif.org/ver10/network/wsdl/DiscoveryLookupBinding", NULL, NULL},
	{"dnrd", "http://www.onvif.org/ver10/network/wsdl/RemoteDiscoveryBinding", NULL, NULL},
	{"d", "http://schemas.xmlsoap.org/ws/2005/04/discovery", NULL, NULL},
	{"dn", "http://www.onvif.org/ver10/network/wsdl", NULL, NULL},
	{"tad", "http://www.onvif.org/ver10/analyticsdevice/wsdl", NULL, NULL},
	{"tae", "http://www.onvif.org/ver10/actionengine/wsdl", NULL, NULL},
	{"tds", "http://www.onvif.org/ver10/device/wsdl", NULL, NULL},
	{"timg", "http://www.onvif.org/ver20/imaging/wsdl", NULL, NULL},
	{"tls", "http://www.onvif.org/ver10/display/wsdl", NULL, NULL},
	{"tmd", "http://www.onvif.org/ver10/deviceIO/wsdl", NULL, NULL},
	{"tptz", "http://www.onvif.org/ver20/ptz/wsdl", NULL, NULL},
	{"trc", "http://www.onvif.org/ver10/recording/wsdl", NULL, NULL},
	{"trp", "http://www.onvif.org/ver10/replay/wsdl", NULL, NULL},
	{"trt", "http://www.onvif.org/ver10/media/wsdl", NULL, NULL},
	{"trv", "http://www.onvif.org/ver10/receiver/wsdl", NULL, NULL},
	{"tse", "http://www.onvif.org/ver10/search/wsdl", NULL, NULL},
	{NULL, NULL, NULL, NULL}
};


/***************************************************************************************************************************/
static int s_parse_a_enc_type(int is_set, int enc_type);
static int s_parse_v_enc_type(int is_set, int enc_type);
static int s_parse_v_enc_level(int is_set, int level);


/***************************************************************************************************************************/

char *_strcpy(char *deststr, char *srctstr)
{
    if (srctstr == NULL || deststr == NULL)
    {
        deststr = NULL;
        return deststr;
    }
    return strcpy(deststr, srctstr);
}

static SOAP creat_soap(struct SOAP_ENV__Header *header, const char *was_To, const char *was_Action, int timeout
                , int probe_flag, NVC_Dev_t *dev)
{
    struct soap *soap = NULL; 
    time_t time_n;
    struct tm *tm_t;
    unsigned char macaddr[MACH_ADDR_LENGTH] = {0};
    char _HwId[LARGE_INFO_LENGTH] = {0};

    soap = soap_new();
    //printf(" => soap: %p\n", soap);
    if(soap==NULL)
    {
      printf("[%s][%s][Line: %d]Error: soap = NULL\n", __FILE__, __func__, __LINE__);
      return NULL;
    }

		#if 1 /**test*/
		if (probe_flag == 1)
		{
			soap_set_namespaces(soap, probe_namespaces);
		}
		else
		{
      #if 0
      int k = 0;
      for (k = 0; k <45; k++)
      {
        printf(" => Namespace->id: \t%s; \tnamespace->ns: %s\n", namespaces[k].id, namespaces[k].ns);
      }      
      #endif
			soap_set_namespaces(soap, namespaces);
		}
		#endif
		//超过5秒钟没有数据就退出
		if (timeout > 0)
		{
			soap->recv_timeout    = timeout;
			soap->connect_timeout = timeout;
		}
		else
		{
			soap->recv_timeout    = 5;
			soap->connect_timeout = 5;
		}
        if (probe_flag == 1)
        {
    		soap_default_SOAP_ENV__Header(soap, header);
    		    		
    		net_get_hwaddr(ETH_NAME, macaddr);
        time_n = time(NULL);
        tm_t = localtime(&time_n);
        sprintf(_HwId, "uuid:1319d68a-%04d-%04d-%04d-%02x%02x%02x%02x%02x%02x",
                tm_t->tm_hour,tm_t->tm_min,tm_t->tm_sec,
                macaddr[0], macaddr[1], macaddr[2], macaddr[3], macaddr[4], macaddr[5]);

    		//sprintf(_HwId,"uuid:1419d68a-1dd2-11b2-a105-%02X%02X%02X%02X%02X%02X",macaddr[0], macaddr[1], macaddr[2], macaddr[3], macaddr[4], macaddr[5]);
    		header->wsa__MessageID =(char *)soap_malloc(soap, sizeof(char) * INFO_LENGTH);    
    		memset(header->wsa__MessageID, 0, sizeof(char) * INFO_LENGTH);
        strncpy(header->wsa__MessageID, _HwId, strlen(_HwId));		

      	header->wsa__ReplyTo = (struct wsa__EndpointReferenceType*)soap_malloc(soap, sizeof(struct wsa__EndpointReferenceType));
        memset(header->wsa__ReplyTo, 0, sizeof(struct wsa__EndpointReferenceType));
        header->wsa__ReplyTo->Address = (char *)soap_malloc(soap, sizeof(char) * INFO_LENGTH);
        memset(header->wsa__ReplyTo->Address, 0, sizeof(char)*INFO_LENGTH);
        strcpy(header->wsa__ReplyTo->Address, "http://schemas.xmlsoap.org/ws/2004/08/addressing/role/anonymous");

    		if (was_Action != NULL)
    		{
      			header->wsa__Action =(char *)soap_malloc(soap, sizeof(char) * LARGE_INFO_LENGTH);
      			memset(header->wsa__Action, 0, sizeof(char) * LARGE_INFO_LENGTH);
      			strncpy(header->wsa__Action, was_Action, LARGE_INFO_LENGTH);//"http://schemas.xmlsoap.org/ws/2005/04/discovery/Probe";
    		}
    		if (was_To != NULL)
    		{
      			header->wsa__To =(char *)soap_malloc(soap, sizeof(char) * LARGE_INFO_LENGTH);
      			memset(header->wsa__To, 0, sizeof(char) * LARGE_INFO_LENGTH);
      			strncpy(header->wsa__To,  was_To, LARGE_INFO_LENGTH);//"urn:schemas-xmlsoap-org:ws:2005:04:discovery";	
    		}
    		
    		soap->header = header;
        }
        else
        {
            soap->header = NULL;
            if(dev != NULL)
            {
                if(dev->need_verify && dev->get_tm == 0)
                {
                    fprintf(stderr, "[%s] Add username token digest!\n", __FUNCTION__);
                    NVC_time_t tm;
                    NVC_Get_SystemTime(dev, &tm);
                    char dev_time[64];
                    sprintf(dev_time, "%04d-%02d-%02dT%02d:%02d:%02dZ", tm);
                    soap_wsse_add_UsernameTokenDigest_c(soap, "", dev->user, dev->passwd, dev_time);//添加校验
                }
            }
        }

		//printf("was_Action: %s\n", soap->header->wsa__Action);
		//printf("was_To: %s\n", soap->header->wsa__To);

		return soap;
} 

static void destroy_soap(struct soap *soap)
{
    soap_destroy(soap); 
    soap_end(soap); 
    soap_done(soap);
    if(soap)
    {
      	//printf("[%s] => soap: %p\n", __func__, soap);
		#if 0 /* NOTE */
		soap_free(struct soap *soap)
		{
		  soap_done(soap);
          free(soap);
		}
		#endif
        free(soap);
        soap = NULL;
      	//printf("[%s] => soap: %p\n", __func__, soap);
    }
}

/******************************************************************************************
func: nvc_get_ip_port(NVC_Probe_t *probe, char *url)
param: 
	in: url: its type is something like "http://192.168.001.200:8000/onvif/device_service"
	out: ip and port
return:
	success: 0
	other: -1;
********************************************************************************************/
static int nvc_get_ip_port(NVC_Probe_t * probe, char *url)
{
	char tmp[256];
	if (url != NULL && strlen(url) != 0)
	{
		sscanf(url, "http://%s", tmp);
		
	}
	if (strstr(tmp, "/"))
	{
		char *host = strtok(tmp, "/");
		if (host)
		{
			//if (strstr(host, ":"))
			{
				char *ip = strsep(&host, ":");
				if (ip)
				{
					memset(probe->ip, 0, strlen(probe->ip));
					strncpy(probe->ip, ip, MAX_IP_LEN);
				}
				if (host)
				{
					sscanf(host, "%d", &(probe->port));
				}
                else
                {
                    probe->port = 80;
                }
			}
		}
	}
	
	return 0;
}

#if 1
SOAP_FMAC5 int SOAP_FMAC6 nvc_discovery_start(struct soap *soap, const char *soap_endpoint, const char *soap_action, struct d__ProbeType *d__Probe)  
{  
        struct __dndl__Probe soap_tmp___dndl__Probe;  
  
        if (!soap_action)  
                soap_action = "http://www.onvif.org/ver10/network/wsdl/Probe";  
        soap->encodingStyle = NULL;  
        soap_tmp___dndl__Probe.d__Probe = d__Probe;  
        soap_begin(soap);  
        soap_serializeheader(soap);  
        soap_serialize___dndl__Probe(soap, &soap_tmp___dndl__Probe);  
        if (soap_begin_count(soap))  
                return soap->error;  
  
        if (soap->mode & SOAP_IO_LENGTH)  
        {  
            if (soap_envelope_begin_out(soap)  
                            || soap_putheader(soap)  
                            || soap_body_begin_out(soap)  
                            || soap_put___dndl__Probe(soap, &soap_tmp___dndl__Probe, "-dndl:Probe", NULL)  
                            || soap_body_end_out(soap)  
                            || soap_envelope_end_out(soap))  
                    return soap->error;  
        }  
        if (soap_end_count(soap))  
                return soap->error;  
        if (soap_connect(soap, soap_endpoint, soap_action)  
                        || soap_envelope_begin_out(soap)  
                        || soap_putheader(soap)  
                        || soap_body_begin_out(soap)  
                        || soap_put___dndl__Probe(soap, &soap_tmp___dndl__Probe, "-dndl:Probe", NULL)  
                        || soap_body_end_out(soap)  
                        || soap_envelope_end_out(soap)  
                        || soap_end_send(soap))  
                return soap_closesock(soap);  
        return SOAP_OK;  
}  
  
SOAP_FMAC5 int SOAP_FMAC6 nvc_discovery_recv(struct soap *soap, struct d__ProbeMatchesType *d__ProbeMatches)  
{  
        if (!d__ProbeMatches)  
        {
                printf("[%s][%s][Line:%d] => param error: %d, %s, %s\n",__FILE__, __func__, __LINE__, soap->error, *soap_faultcode(soap), *soap_faultstring(soap));
                soap_closesock(soap);  
                return -1;  
        }
  
        soap_default_d__ProbeMatchesType(soap, d__ProbeMatches);  
  
        if (soap_begin_recv(soap)  
                        || soap_envelope_begin_in(soap)  
                        || soap_recv_header(soap)  
                        || soap_body_begin_in(soap))
        {  
                printf("[%s][%s][Line:%d] => begin error: %d, %s, %s\n",__FILE__, __func__, __LINE__, soap->error, *soap_faultcode(soap), *soap_faultstring(soap));
                return soap_closesock(soap);  
        }
        soap_get_d__ProbeMatchesType(soap, d__ProbeMatches, "d:ProbeMatches", "d:ProbeMatchesType");  
        if (soap->error)  
        {
                printf("[%s][%s][Line:%d] => analys error: %d, %s, %s\n",__FILE__, __func__, __LINE__, soap->error, *soap_faultcode(soap), *soap_faultstring(soap));
                soap_recv_fault(soap, 0);  
                return -1;  
        }  
        if (soap_body_end_in(soap)  
                        || soap_envelope_end_in(soap)  
                        || soap_end_recv(soap))  
        {
                printf("[%s][%s][Line:%d] => end error: %d, %s, %s\n",__FILE__, __func__, __LINE__, soap->error, *soap_faultcode(soap), *soap_faultstring(soap));
                soap_closesock(soap);  
                return -1;  
        }
        return SOAP_OK;  
}  
  
SOAP_FMAC5 int SOAP_FMAC6 nvc_discovery_stop(struct soap *soap)  
{  
        return soap_closesock(soap);  
} 

int NVC_discovery(NVC_Probe_t *probe, int timeout, NVC_PROBER_CB *cb, void *user_args)
{
  struct soap *soap = NULL; 
  int  retval = 0; 
  struct d__ProbeType req;
  struct d__ProbeMatchesType resp;
  struct d__ScopesType sScope;
  char *was_To = "urn:schemas-xmlsoap-org:ws:2005:04:discovery";
  char *was_Action = "http://schemas.xmlsoap.org/ws/2005/04/discovery/Probe";
  char *soap_endpoint = "soap.udp://239.255.255.250:3702/";

  //printf(" => soap: %p\n", soap);
  //soap = (struct soap *)malloc(sizeof(struct soap));
  //memset(soap, 0, sizeof(struct soap));
  struct SOAP_ENV__Header header;
  NVC_Dev_t *dev;
  
  NET_IPV4 ip;
  char _IPAddr[LARGE_INFO_LENGTH];
  
  ip.int32 = net_get_ifaddr(ETH_NAME);
  sprintf(_IPAddr, "%d.%d.%d.%d", ip.str[0], ip.str[1], ip.str[2], ip.str[3]);
  printf("ipaddr: %s\n", _IPAddr);
  soap = creat_soap(&header, was_To, was_Action, timeout, 1, dev);
  if (soap == NULL)
  {
    printf("[%s][%d] => creat_soap failed!\n", __func__, __LINE__);
    return -1;
  }

 	soap_default_d__ScopesType(soap, &sScope);
	sScope.__item = "";
	//sScope.__item = "onvif://www.onvif.org";

 	soap_default_d__ProbeType(soap, &req);
	//req.d__Scopes = &sScope;
	req.d__Scopes = NULL;
	req.d__Types = (char **)soap_malloc(soap, sizeof(char *));
	memset(req.d__Types, 0, sizeof(char *));
	req.d__Types[0] = (char *)soap_malloc(soap, sizeof(char) * MAX_INFO_LEN);
	memset(req.d__Types[0], 0, sizeof(char) * MAX_INFO_LEN);
  if(probe->type == 0)
  {
    strncpy(req.d__Types[0], "tds:Device", MAX_INFO_LEN);
  }
  else if(probe->type == 1)
  {
      strncpy(req.d__Types[0], "dn:NetworkVideoTransmitter", MAX_INFO_LEN);
  }
  retval = nvc_discovery_start(soap, soap_endpoint, NULL, &req);

  nvc_probe_item_t item; /**回调函数的输出参数*/
  memset(&item, 0, sizeof(nvc_probe_item_t));
  while (retval == SOAP_OK)
  {
    retval = nvc_discovery_recv(soap, &resp);
    if (retval == SOAP_OK)
    {
      printf("retval == soap_ok\n");
      if (soap->error)
      {
        printf("[%s][%s][Line:%d] => soap error: %d, %s, %s\n",__FILE__, __func__, __LINE__, soap->error, *soap_faultcode(soap), *soap_faultstring(soap)); 
        retval = soap->error;
      }
      else
      {
        int i = 0;
        for (i = 0; i < resp.__sizeProbeMatch; i++)
  			{
  				if (resp.ProbeMatch != NULL && resp.ProbeMatch[i].d__XAddrs != NULL)
  				{
  					nvc_get_ip_port(probe, resp.ProbeMatch[i].d__XAddrs);
  				}
  				printf("soap_call___dndl__Probe     \r\n");
  				printf("Target Service Address  : %s\r\n", resp.ProbeMatch[i].d__XAddrs);	
  				printf("************************************\n\n");
             
          memset(&item, 0, sizeof(nvc_probe_item_t));
          item.type = probe->type;
          item.port = probe->port;
          strncpy(item.ip, probe->ip, MAX_IP_LEN);
          sprintf(item.dst_id, "%s:%d", probe->ip, probe->port);
          printf("[%s][%d] => item.type: %d, ip: %s, port: %d, dst_id: %s\n"
              , __func__, __LINE__
              , item.type, item.ip, item.port, item.dst_id);
#if 0 
          /**获取探测到的设备通道数目*/
          {
            struct _tds__GetCapabilities capaReq;
            struct _tds__GetCapabilitiesResponse capaResp;
            struct soap *_soap = creat_soap(&header, NULL, NULL, timeout, 0);
            char *_soap_endpoint = (char *)calloc(1, sizeof(char)*MAX_URL_LEN);//"soap.udp://239.255.255.250:3702/";
            sprintf(_soap_endpoint, "http://%s:%d/onvif/device_service", item.ip, item.port);
            const char *_soap_action = "http://www.onvif.org/ver10/device/wsdl/GetCapabilities";
            capaReq.Category = (enum tt__CapabilityCategory *)soap_malloc(_soap, sizeof(char)*INFO_LENGTH);
            memset(capaReq.Category, 0, sizeof(char)*INFO_LENGTH);
            *(capaReq.Category) = Media;
            capaReq.__sizeCategory = 1;
            printf("aaaaaaaa => capaReq.__sizeCategory: %d, Category: %d\n"
                , capaReq.__sizeCategory, *(capaReq.Category));
            soap_call___tds__GetCapabilities(_soap, _soap_endpoint, _soap_action, &capaReq, &capaResp);
            if (_soap_endpoint)
            {
              free(_soap_endpoint);
              _soap_endpoint = NULL;
            }
            if (_soap->error)
            {
          		printf("[%s][%s][Line:%d] => soap error: %d, %s, %s\n",__FILE__, __func__, __LINE__, _soap->error, *soap_faultcode(_soap), *soap_faultstring(_soap)); 
          		item.ch_num = 2;
            }
            else
            {                           
              if (capaResp.Capabilities 
                  && capaResp.Capabilities->Media
                  && strlen(capaResp.Capabilities->Media->XAddr))
              {
                char *__soap_endpoint = (char *)calloc(1, sizeof(char)*MAX_URL_LEN);//"soap.udp://239.255.255.250:3702/";
                strncpy(__soap_endpoint, capaResp.Capabilities->Media->XAddr, sizeof(char)*MAX_URL_LEN);
                const char *__soap_action = "http://www.onvif.org/ver10/media/wsdl/GetProfiles";
                struct _trt__GetProfiles profile_req;
                struct _trt__GetProfilesResponse profile_resp;
                memset(&profile_resp, 0, sizeof(struct _trt__GetProfilesResponse));
                soap_call___trt__GetProfiles(_soap, __soap_endpoint, __soap_action, &profile_req, &profile_resp);
                if (_soap->error)
                {
                  printf("[%s][%s][Line:%d] => soap error: %d, %s, %s\n",__FILE__, __func__, __LINE__, _soap->error, *soap_faultcode(_soap), *soap_faultstring(_soap)); 
                  item.ch_num = 2;
                }
                else
                {
                  if (profile_resp.__sizeProfiles > 0 
                      && profile_resp.Profiles[0].VideoSourceConfiguration)
                  {
                      item.ch_num = profile_resp.Profiles[0].VideoSourceConfiguration->UseCount;
                      item.ch_num = (item.ch_num == 0)?2:item.ch_num;
                  }
                  else
                  {
                      item.ch_num = 2;
                  }
                  printf("[%s][%d] => item.ch_num: %d\n",__func__, __LINE__, item.ch_num);
                }
                if (__soap_endpoint) 
                {
                  free(__soap_endpoint);
                  __soap_endpoint = NULL;
                }
              }
              else
              {
                printf("[%s][%s][Line:%d] => ch_num: %d\n",__FILE__, __func__, item.ch_num);
                item.ch_num = 2;
              }                            
            }
            destroy_soap(_soap);
          }
#endif
          item.ch_num = 2;
          printf("[%s][%d] => item.type: %d, ip: %s, port: %d, ch_num: %d\n"
              , __func__, __LINE__
              , item.type, item.ip, item.port, item.ch_num);
          if(strcmp(item.ip, _IPAddr))//NVR不显示在onvif的搜索结果列表中                 
            cb(&item, user_args);
  			}			
      }
    }
    else if (soap->error)  
    {  
      printf("retval == soap_error\n");
      printf("[%s][%s][Line:%d] => soap error: %d, %s, %s\n",__FILE__, __func__, __LINE__, soap->error, *soap_faultcode(soap), *soap_faultstring(soap)); 
      retval = soap->error;  
    }  
  }
  retval |= nvc_discovery_stop(soap);
  destroy_soap(soap);

  return retval;
}
#else
int NVC_discovery(NVC_Probe_t *probe, int timeout)
{
	struct soap *soap = NULL; 
	int  retval = 0; 
	struct d__ProbeType req;
	struct d__ProbeMatchesType resp;
	struct d__ScopesType sScope;
	char *was_To = "urn:schemas-xmlsoap-org:ws:2005:04:discovery";
	char *was_Action = "http://schemas.xmllocal_soap.org/ws/2005/04/discovery/Probe";
	char *soap_endpoint = "soap.udp://239.255.255.250:3702/";

   	//printf(" => soap: %p\n", soap);
  	/*soap = (struct soap *)malloc(sizeof(struct soap));
 	memset(soap, 0, sizeof(struct soap));*/
	struct SOAP_ENV__Header header;
	soap = creat_soap(&header, was_To, was_Action, timeout, 1);

 	soap_default_d__ScopesType(soap, &sScope);
	sScope.__item = "";
	//sScope.__item = "onvif://www.onvif.org";

 	soap_default_d__ProbeType(soap, &req);
	req.d__Scopes = &sScope;
	
	req.d__Types = (char **)soap_malloc(soap, sizeof(char *));
	memset(req.d__Types, 0, sizeof(char *));
	req.d__Types[0] = (char *)soap_malloc(soap, sizeof(char) * MAX_INFO_LEN);
	memset(req.d__Types[0], 0, sizeof(char) * MAX_INFO_LEN);

	strncpy(req.d__Types[0], "dn:NetworkVideoTransmitter", MAX_INFO_LEN);
	do
	{
		//printf("soap->version: %d\n", soap->version);
		//printf("soap: %p, was_Action: %s\n", soap, soap->header->wsa__Action);
		//printf("soap: %p, was_To: %s\n", soap, soap->header->wsa__To);

		soap_call___dndl__Probe(soap, soap_endpoint, NULL, &req, &resp); 
		if (soap->error)
		{ 
			printf("[%s][%s][Line:%d] => soap error: %d, %s, %s\n",__FILE__, __func__, __LINE__, soap->error, *soap_faultcode(soap), *soap_faultstring(soap)); 
			retval = soap->error; 
			break;
		} 
		else
		{
			int i = 0;
			for (i = 0; i < resp.__sizeProbeMatch; i++)
			{
				if (resp.ProbeMatch != NULL && resp.ProbeMatch[i].d__XAddrs != NULL)
				{
					nvc_get_ip_port(probe, resp.ProbeMatch[i].d__XAddrs);
				}
				#if 0
				printf("soap_call___dndl__Probe : %d\r\n", i);
				
				printf(" __sizeProbeMatch       : %d\r\n", resp.__sizeProbeMatch);
				printf("Target EP Address       : %s\r\n", resp.ProbeMatch[i].wsa__EndpointReference.Address);
				printf("Target Type             : %s\r\n", resp.ProbeMatch[i].d__Types[i]);
				printf("Target Service Address  : %s\r\n", resp.ProbeMatch[i].d__XAddrs);
				printf("Target Metadata Version : %d\r\n", resp.ProbeMatch[i].d__MetadataVersion);
				printf("Target Scopes Address   : %s\r\n", resp.ProbeMatch[i].d__Scopes->__item);
				#else
				printf("soap_call___dndl__Probe     \r\n");
				printf("Target Service Address  : %s\r\n", resp.ProbeMatch[i].d__XAddrs);	
				#endif
				printf("************************************\n\n");
			}			
		}
	}while(0);
	
	 destroy_soap(soap);
	
    return  retval; 
}
#endif


int NVC_Get_profiles(NVC_Dev_t *dev)
{
START:;
	int ret = 0;
  int do_again = 0;

	struct _trt__GetProfiles profile_req;
	struct _trt__GetProfilesResponse profile_resp;
	memset(&profile_resp, 0, sizeof(struct _trt__GetProfilesResponse));
    
	struct soap *soap = NULL;//(struct soap *)calloc(1, sizeof(struct soap));
	//const char *was_To = "";
	//const char *was_Action = "";
	
	struct SOAP_ENV__Header header;
	//soap = creat_soap(&header, was_To, was_Action, dev->timeout, 0);
    soap = creat_soap(&header, NULL, NULL, dev->timeout, 0, dev);
    if (soap == NULL)
    {
        printf("[%s][%d] => creat_soap failed!\n", __func__, __LINE__);
        return -1;
    }
    
	if (dev->capa == NULL 
        || dev->capa == NULL
        || dev->capa->media_capa == NULL 
        || dev->capa->media_capa->url == NULL
        || strlen(dev->capa->media_capa->url) < 1)
	{
		printf("[%s][%d] =>Error: Invaild parameter! dev->capa: %p, soap_endpoint: %p\n", __func__, __LINE__, dev->capa, dev->capa->media_capa->url);
		destroy_soap(soap);
		return -1;
	}
	
	char *soap_endpoint = (char *)calloc(1, sizeof(char)*MAX_URL_LEN);
	strncpy(soap_endpoint, dev->capa->media_capa->url, sizeof(char)*MAX_URL_LEN);

	const char *soap_action = "http://www.onvif.org/ver10/media/wsdl/GetProfiles";

	do
	{
		soap_call___trt__GetProfiles(soap, soap_endpoint, soap_action, &profile_req, &profile_resp);
		if (soap->error)
		{
			printf("[%s][%s][Line:%d] => soap error: %d, %s, %s\n",__FILE__, __func__, __LINE__, soap->error, *soap_faultcode(soap), *soap_faultstring(soap)); 
			ret = soap->error; 
      if((ret == SOAP_CLI_FAULT) || (ret == SOAP_FAULT) || (ret == 401))
      {
          fprintf(stderr, "[%s] Verify error! Send again!\n", __FUNCTION__);
          if(!dev->need_verify)
          {
              dev->need_verify = 1;
              do_again = 1;
          }    
      }
			break;
		}
		else 
		{
			int i = 0;
			dev->profile_tn = (Profile_info_t *)calloc(profile_resp.__sizeProfiles, sizeof(Profile_info_t));
			for (i = 0; i < profile_resp.__sizeProfiles; i++)
			{
				//printf(" => i: %d\n", i);
				if (profile_resp.Profiles != NULL)
				{
					memset(dev->profile_tn[i].profile_token, 0, MAX_TOKEN_LEN);
					//memset(dev->profile_tn[i].profile_name, 0, MAX_TOKEN_LEN);
					if (profile_resp.Profiles[i].token != NULL)
					{
						strncpy(dev->profile_tn[i].profile_token, profile_resp.Profiles[i].token, MAX_TOKEN_LEN);
					}
          #if 0
					if (profile_resp.Profiles[i].Name != NULL)
					{
						strncpy(dev->profile_tn[i].profile_name, profile_resp.Profiles[i].Name, MAX_TOKEN_LEN);
					}
          #endif
					if (profile_resp.Profiles[i].VideoSourceConfiguration != NULL)
					{
						memset(dev->profile_tn[i].v_src_token, 0, MAX_TOKEN_LEN);
						memset(dev->profile_tn[i].v_src_name, 0, MAX_TOKEN_LEN);
						memset(dev->profile_tn[i].v_src_sourcetoken, 0, MAX_TOKEN_LEN);
						if (profile_resp.Profiles[i].VideoSourceConfiguration->token != NULL)
						{
							strncpy(dev->profile_tn[i].v_src_token, profile_resp.Profiles[i].VideoSourceConfiguration->token, MAX_TOKEN_LEN);
						}
						if (profile_resp.Profiles[i].VideoSourceConfiguration->Name != NULL)
						{
							strncpy(dev->profile_tn[i].v_src_name, profile_resp.Profiles[i].VideoSourceConfiguration->Name, MAX_TOKEN_LEN);
						}
						if (profile_resp.Profiles[i].VideoSourceConfiguration->SourceToken != NULL)
						{
							strncpy(dev->profile_tn[i].v_src_sourcetoken, profile_resp.Profiles[i].VideoSourceConfiguration->SourceToken, MAX_TOKEN_LEN);
						}
						dev->ch_num = profile_resp.Profiles[0].VideoSourceConfiguration->UseCount;
					}
									
					if (profile_resp.Profiles[i].AudioSourceConfiguration != NULL)
					{
						memset(dev->profile_tn[i].a_src_token, 0, MAX_TOKEN_LEN);
						memset(dev->profile_tn[i].a_src_name, 0, MAX_TOKEN_LEN);
						memset(dev->profile_tn[i].a_src_sourcetoken, 0, MAX_TOKEN_LEN);
						if (profile_resp.Profiles[i].AudioSourceConfiguration->token != NULL)
						{
							strncpy(dev->profile_tn[i].a_src_token, profile_resp.Profiles[i].AudioSourceConfiguration->token, MAX_TOKEN_LEN);
						}
						if (profile_resp.Profiles[i].AudioSourceConfiguration->Name != NULL)
						{
							strncpy(dev->profile_tn[i].a_src_name, profile_resp.Profiles[i].AudioSourceConfiguration->Name, MAX_TOKEN_LEN);
						}
						if (profile_resp.Profiles[i].AudioSourceConfiguration->SourceToken != NULL)
						{
							strncpy(dev->profile_tn[i].a_src_sourcetoken, profile_resp.Profiles[i].AudioSourceConfiguration->SourceToken, MAX_TOKEN_LEN);
						}
					}
									
					if (profile_resp.Profiles[i].VideoEncoderConfiguration != NULL)
					{
						memset(dev->profile_tn[i].v_enc_token, 0, MAX_TOKEN_LEN);
						memset(dev->profile_tn[i].v_enc_name, 0, MAX_TOKEN_LEN);
						if (profile_resp.Profiles[i].VideoEncoderConfiguration->token != NULL)
						{
							strncpy(dev->profile_tn[i].v_enc_token, profile_resp.Profiles[i].VideoEncoderConfiguration->token, MAX_TOKEN_LEN);
						}
						if (profile_resp.Profiles[i].VideoEncoderConfiguration->Name != NULL)
						{
							strncpy(dev->profile_tn[i].v_enc_name, profile_resp.Profiles[i].VideoEncoderConfiguration->Name, MAX_TOKEN_LEN);
						}
						#if 1
						dev->profile_tn[i].v_enc = s_parse_v_enc_type(0, profile_resp.Profiles[i].VideoEncoderConfiguration->Encoding);
						if (profile_resp.Profiles[i].VideoEncoderConfiguration->RateControl != NULL)
						{
							dev->profile_tn[i].frame_rate = profile_resp.Profiles[i].VideoEncoderConfiguration->RateControl->FrameRateLimit;
						}
						if (profile_resp.Profiles[i].VideoEncoderConfiguration->Resolution != NULL)
						{
							dev->profile_tn[i].width  = profile_resp.Profiles[i].VideoEncoderConfiguration->Resolution->Width;
							dev->profile_tn[i].height = profile_resp.Profiles[i].VideoEncoderConfiguration->Resolution->Height;
						}
						else if (profile_resp.Profiles[i].VideoSourceConfiguration != NULL 
								&& profile_resp.Profiles[i].VideoSourceConfiguration->Bounds)
						{
							dev->profile_tn[i].width  = profile_resp.Profiles[i].VideoSourceConfiguration->Bounds->width;
							dev->profile_tn[i].height = profile_resp.Profiles[i].VideoSourceConfiguration->Bounds->height;
						}
						else 
						{
							dev->profile_tn[i].width  = 0;
							dev->profile_tn[i].height = 0;
						}
						#endif	
					}
					#if 1				
					if (profile_resp.Profiles[i].AudioEncoderConfiguration != NULL)
					{
						memset(dev->profile_tn[i].a_enc_token, 0, MAX_TOKEN_LEN);
						memset(dev->profile_tn[i].a_enc_name, 0, MAX_TOKEN_LEN);
						if (profile_resp.Profiles[i].AudioEncoderConfiguration->token != NULL)
						{
							strncpy(dev->profile_tn[i].a_enc_token, profile_resp.Profiles[i].AudioEncoderConfiguration->token, MAX_TOKEN_LEN);
						}
						if (profile_resp.Profiles[i].AudioEncoderConfiguration->Name != NULL)
						{
							strncpy(dev->profile_tn[i].a_enc_name, profile_resp.Profiles[i].AudioEncoderConfiguration->Name, MAX_TOKEN_LEN);
						}
						dev->profile_tn[i].a_enc = s_parse_a_enc_type(0, profile_resp.Profiles[i].AudioEncoderConfiguration->Encoding);
					}
					#endif								
					if (profile_resp.Profiles[i].VideoAnalyticsConfiguration != NULL)
					{
						memset(dev->profile_tn[i].v_ana_token, 0, MAX_TOKEN_LEN);
						memset(dev->profile_tn[i].v_ana_name, 0, MAX_TOKEN_LEN);
						if (profile_resp.Profiles[i].VideoAnalyticsConfiguration->token != NULL)
						{
							strncpy(dev->profile_tn[i].v_ana_token, profile_resp.Profiles[i].VideoAnalyticsConfiguration->token, MAX_TOKEN_LEN);
						}
						if (profile_resp.Profiles[i].VideoAnalyticsConfiguration->Name != NULL)
						{
							strncpy(dev->profile_tn[i].v_ana_name, profile_resp.Profiles[i].VideoAnalyticsConfiguration->Name, MAX_TOKEN_LEN);
						}
					}
					#if 1				
					if (profile_resp.Profiles[i].PTZConfiguration != NULL)
					{
						memset(dev->profile_tn[i].PTZ_parm_token, 0, MAX_TOKEN_LEN);
						//memset(dev->profile_tn[i].PTZ_parm_name, 0, MAX_TOKEN_LEN);
						memset(dev->profile_tn[i].PTZ_parm_nodetoken, 0, MAX_TOKEN_LEN);
						if (profile_resp.Profiles[i].PTZConfiguration->token != NULL)
						{
							strncpy(dev->profile_tn[i].PTZ_parm_token, profile_resp.Profiles[i].PTZConfiguration->token, MAX_TOKEN_LEN);
						}
            #if 0
						if (profile_resp.Profiles[i].PTZConfiguration->Name != NULL)
						{
							strncpy(dev->profile_tn[i].PTZ_parm_name, profile_resp.Profiles[i].PTZConfiguration->Name, MAX_TOKEN_LEN);
						}
            #endif
						if (profile_resp.Profiles[i].PTZConfiguration->NodeToken != NULL)
						{
							strncpy(dev->profile_tn[i].PTZ_parm_nodetoken, profile_resp.Profiles[i].PTZConfiguration->NodeToken, MAX_TOKEN_LEN);
						}
					}
					#endif
					if (profile_resp.Profiles[i].MetadataConfiguration != NULL)
					{
						memset(dev->profile_tn[i].m_parm_token, 0, MAX_TOKEN_LEN);
						memset(dev->profile_tn[i].m_parm_name, 0, MAX_TOKEN_LEN);
						if (profile_resp.Profiles[i].MetadataConfiguration->token != NULL)
						{
							strncpy(dev->profile_tn[i].m_parm_token, profile_resp.Profiles[i].MetadataConfiguration->token, MAX_TOKEN_LEN);
						}
						if (profile_resp.Profiles[i].MetadataConfiguration->Name != NULL)
						{
							strncpy(dev->profile_tn[i].m_parm_name, profile_resp.Profiles[i].MetadataConfiguration->Name, MAX_TOKEN_LEN);
						}
					}
					#if 1				
					if (profile_resp.Profiles[i].Extension != NULL)
					{
						if (profile_resp.Profiles[i].Extension->AudioOutputConfiguration != NULL)
						{
							memset(dev->profile_tn[i].ao_parm_token, 0, MAX_TOKEN_LEN);
							memset(dev->profile_tn[i].ao_parm_name, 0, MAX_TOKEN_LEN);
							memset(dev->profile_tn[i].ao_parm_outtoken, 0, MAX_TOKEN_LEN);
							if (profile_resp.Profiles[i].Extension->AudioOutputConfiguration->token != NULL)
							{
								strncpy(dev->profile_tn[i].ao_parm_token, profile_resp.Profiles[i].Extension->AudioOutputConfiguration->token, MAX_TOKEN_LEN);
							}
							if (profile_resp.Profiles[i].Extension->AudioOutputConfiguration->Name != NULL)
							{
								strncpy(dev->profile_tn[i].ao_parm_name, profile_resp.Profiles[i].Extension->AudioOutputConfiguration->Name, MAX_TOKEN_LEN);
							}
							if (profile_resp.Profiles[i].Extension->AudioOutputConfiguration->OutputToken != NULL)
							{
								strncpy(dev->profile_tn[i].ao_parm_outtoken, profile_resp.Profiles[i].Extension->AudioOutputConfiguration->OutputToken, MAX_TOKEN_LEN);
							}
						}
					#endif
						
						if (profile_resp.Profiles[i].Extension->AudioDecoderConfiguration != NULL)
						{
							memset(dev->profile_tn[i].a_dec_token, 0, MAX_TOKEN_LEN);
							memset(dev->profile_tn[i].a_dec_name, 0, MAX_TOKEN_LEN);
							if (profile_resp.Profiles[i].Extension->AudioDecoderConfiguration->token != NULL)
							{
								strncpy(dev->profile_tn[i].a_dec_token, profile_resp.Profiles[i].Extension->AudioDecoderConfiguration->token, MAX_TOKEN_LEN);
							}
							if (profile_resp.Profiles[i].Extension->AudioDecoderConfiguration->Name != NULL)
							{
								strncpy(dev->profile_tn[i].a_dec_name, profile_resp.Profiles[i].Extension->AudioDecoderConfiguration->Name, MAX_TOKEN_LEN);
							}
						}
					}
					
					printf("%s => i: %d, PTZ_parm_token: [%s]\n",     __func__, i, dev->profile_tn[i].PTZ_parm_token);
					printf("%s => i: %d, PTZ_parm_nodetoken: [%s]\n", __func__, i, dev->profile_tn[i].PTZ_parm_nodetoken);
					printf("%s => i: %d, profiletoken: [%s]\n",       __func__, i, dev->profile_tn[i].profile_token);
					
				}
				else
				{
					printf("[%s][%s][%d] => can not get profile, profile_resp.Profile = NULL\n", __FILE__, __func__, __LINE__);
				}
			}
			break;
		}
	}while(0);

	if (soap_endpoint) 
	{
		free(soap_endpoint);
		soap_endpoint = NULL;
	}
	destroy_soap(soap);
  fprintf(stderr, "[%s] do again=%d\n", __FUNCTION__, do_again);
  if(do_again)
  {
      goto START;
  }
	return ret;
}


int NVC_Get_Capabilities(NVC_Dev_t *dev)//, NVC_capa_t *capa)
{
START:;
	int retval = 0;
	struct soap *soap = NULL;
	struct _tds__GetCapabilities capaReq;
	struct _tds__GetCapabilitiesResponse capaResp;
  memset(&capaResp, 0, sizeof(struct _tds__GetCapabilitiesResponse));
  int do_again = 0;

	//const char *was_To = "";//"http://www.onvif.org/ver10/device/wsdl/Device/GetCapabilitiesResponse";
	//const char *was_Action = "";//"http://www.onvif.org/ver10/device/wsdl/Device/GetCapabilities";
	
	struct SOAP_ENV__Header header;
	//soap = creat_soap(&header, was_To, was_Action, dev->timeout, 0);
    soap = creat_soap(&header, NULL, NULL, dev->timeout, 0, dev);

    if (soap == NULL)
    {
        printf("[%s][%d] => creat_soap failed!\n", __func__, __LINE__);
        return -1;
    }

	char *soap_endpoint = (char *)calloc(1, sizeof(char)*MAX_URL_LEN);//"soap.udp://239.255.255.250:3702/";
	sprintf(soap_endpoint, "http://%s:%d/onvif/device_service", dev->ip, dev->port);
	printf("[%s][%d] =>soap_endpoint: %s\n", __func__,__LINE__, soap_endpoint);
	
	const char *soap_action = "http://www.onvif.org/ver10/device/wsdl/GetCapabilities";
	
	//printf("[%s][%d] =>get capabilities type: %d\n", __func__, __LINE__, dev->capa->capa_type);;
	
	if (dev->capa->capa_type < 0 || dev->capa->capa_type > 6)	/**capa_type is not one of enum _capabilities_type*/
	{
		capaReq.Category = NULL;
	}
	else
	{
		capaReq.Category = (enum tt__CapabilityCategory *)soap_malloc(soap, sizeof(char)*INFO_LENGTH);
		memset(capaReq.Category, 0, sizeof(char)*INFO_LENGTH);
        capaReq.__sizeCategory = 1;
		*(capaReq.Category) = dev->capa->capa_type;
	}
    
	do 
	{
		soap_call___tds__GetCapabilities(soap, soap_endpoint, soap_action, &capaReq, &capaResp);
		if (soap->error)
		{
                //soap error: 1, SOAP-ENV:Sender, Server authentication failed
                // soap error: 12, SOAP-ENV:Sender, The action requested requires authorization and the sender is not authorized
				printf("[%s][%s][Line:%d] => soap error: %d, %s, %s\n",__FILE__, __func__, __LINE__, soap->error, *soap_faultcode(soap), *soap_faultstring(soap)); 
				retval = soap->error;
                if((retval == SOAP_CLI_FAULT) || (retval == SOAP_FAULT))
                {
                    fprintf(stderr, "[%s] Verify error! Send again!\n", __FUNCTION__);
                    if(!dev->need_verify)
                    {
                        dev->need_verify = 1;
                        do_again = 1;
                    }    
                }
				break;
		}
		else
		{
			if (capaResp.Capabilities != NULL)
			{
				if ((capaResp.Capabilities->Analytics != NULL) 
					&& (capaResp.Capabilities->Analytics->XAddr != NULL))
				{
					memset(dev->capa->ana_capa->url, 0, strlen(dev->capa->ana_capa->url));
					strncpy(dev->capa->ana_capa->url, capaResp.Capabilities->Analytics->XAddr, MAX_URL_LEN);
				}
				if ((capaResp.Capabilities->Device != NULL) 
					&& (capaResp.Capabilities->Device->XAddr != NULL))
				{
					memset(dev->capa->dev_capa->url, 0, strlen(dev->capa->dev_capa->url));
					strncpy(dev->capa->dev_capa->url, capaResp.Capabilities->Device->XAddr, MAX_URL_LEN);
				}
				if ((capaResp.Capabilities->Events != NULL) 
					&& (capaResp.Capabilities->Events->XAddr != NULL))
				{
					memset(dev->capa->env_capa->url, 0, strlen(dev->capa->env_capa->url));
					strncpy(dev->capa->env_capa->url, capaResp.Capabilities->Events->XAddr, MAX_URL_LEN);
				}
				if ((capaResp.Capabilities->Imaging != NULL) 
					&& (capaResp.Capabilities->Imaging->XAddr != NULL))
				{
					memset(dev->capa->img_capa->url, 0, strlen(dev->capa->img_capa->url));
					strncpy(dev->capa->img_capa->url, capaResp.Capabilities->Imaging->XAddr, MAX_URL_LEN);
				}
				if ((capaResp.Capabilities->Media != NULL) 
					&& (capaResp.Capabilities->Media->XAddr != NULL))
				{
					memset(dev->capa->media_capa->url, 0, strlen(dev->capa->media_capa->url));
					strncpy(dev->capa->media_capa->url, capaResp.Capabilities->Media->XAddr, MAX_URL_LEN);
				}
				if ((capaResp.Capabilities->PTZ != NULL) 
					&& (capaResp.Capabilities->PTZ->XAddr != NULL))
				{
					memset(dev->capa->PTZ_capa->url, 0, strlen(dev->capa->PTZ_capa->url));
					strncpy(dev->capa->PTZ_capa->url, capaResp.Capabilities->PTZ->XAddr, MAX_URL_LEN);
				}
			}
      #if 1
      printf("[%s][%d] =>ana_capa->url: %s\n",__func__,__LINE__, dev->capa->ana_capa->url);
      printf("[%s][%d] =>dev_capa->url: %s\n",__func__,__LINE__, dev->capa->dev_capa->url);
      printf("[%s][%d] =>img_capa->url: %s\n",__func__,__LINE__, dev->capa->img_capa->url);
      printf("[%s][%d] =>media_capa->url: %s\n",__func__,__LINE__, dev->capa->media_capa->url);
      printf("[%s][%d] =>PTZ_capa->url: %s\n",__func__,__LINE__, dev->capa->PTZ_capa->url);
      #endif
		}
	}while(0);

  free(soap_endpoint);
	soap_endpoint = NULL;
	destroy_soap(soap);
  if(do_again)
    goto START;
	return retval;
}

int NVC_GetStreamUri(NVC_Dev_t *dev, NVC_Stream_Uri_t *nvc_stream_url)
{
	//NVC_Dev_t dev_handle = (NVC_Dev_t *)dev;
	struct soap *soap = NULL; 
  	int  retval = 0; 
	struct _trt__GetStreamUri StreamUri_req;
	struct _trt__GetStreamUriResponse StreamUri_resp;
    memset(&StreamUri_resp, 0, sizeof(struct _trt__GetStreamUriResponse));

	if (dev->profile_tn == NULL 
        || dev->profile_tn->profile_token == NULL
        || strlen(dev->profile_tn[dev->st_type].profile_token) < 1 
        || dev->capa == NULL 
        || dev->capa->media_capa == NULL
        || strlen(dev->capa->media_capa->url) < 1)
	{
		printf("[%s][%s] => Error: Invalid parameter!dev->profile_tn: %p, dev->profile_tn->profile_token: %s\n"
			, __FILE__, __func__, dev->profile_tn, dev->profile_tn[dev->st_type].profile_token);	
		return -1;
	}
		
	//const char * was_To = "";//"urn:www-onvif-org:ver10:media:wsdl";
	//const char *was_Action = "";
	
	//soap = (struct soap *)malloc(sizeof(struct soap));
  	//memset(soap, 0, sizeof(struct soap));
	struct SOAP_ENV__Header header;
	//soap = creat_soap(&header, was_To, was_Action, dev->timeout, 0);
    soap = creat_soap(&header, NULL, NULL, dev->timeout, 0, dev);
    if (soap == NULL)
    {
        printf("[%s][%d] => creat_soap failed!\n", __func__, __LINE__);
        return -1;
    }

	char *soap_endpoint = (char *)calloc(1, sizeof(char)*MAX_URL_LEN);//"soap.udp://239.255.255.250:3702/";
	strncpy(soap_endpoint, dev->capa->media_capa->url, sizeof(char)*MAX_URL_LEN);
	
	//printf("[%s][%d] => soap_endpoint: %s\n", __func__,__LINE__, soap_endpoint);

	const char *soap_action ="http://www.onvif.org/ver10/media/wsdl/GetStreamUri";

	StreamUri_req.StreamSetup = (struct tt__StreamSetup*)soap_malloc(soap, sizeof(struct tt__StreamSetup));
	memset(StreamUri_req.StreamSetup, 0, sizeof(struct tt__StreamSetup));
	StreamUri_req.StreamSetup->Stream = nvc_stream_url->protocol;
	StreamUri_req.StreamSetup->Transport = (struct tt__Transport*)soap_malloc(soap, sizeof(struct tt__Transport));
	memset(StreamUri_req.StreamSetup->Transport, 0, sizeof(struct tt__Transport));
	StreamUri_req.StreamSetup->Transport->Protocol = nvc_stream_url->trans_protocol;

	StreamUri_req.ProfileToken = (char *)soap_malloc(soap, sizeof(char) * INFO_LENGTH);
	memset(StreamUri_req.ProfileToken, 0, sizeof(char)*INFO_LENGTH);
	strncpy(StreamUri_req.ProfileToken, dev->profile_tn[dev->st_type].profile_token, sizeof(char)*INFO_LENGTH);

	//printf(" => profiletoken: %s\n", StreamUri_req.ProfileToken);
	
	if ((dev->st_type != 0) && (dev->st_type != 1))
	{
		printf("[%s][%s] => Error: Invalid st_type! do not support such st_type now! st_type: %d\n", __FILE__, __func__, dev->st_type);
		free(soap_endpoint);
		soap_endpoint = NULL;
		destroy_soap(soap);
		return -1;
	}

	do 
	{
		soap_call___trt__GetStreamUri(soap, soap_endpoint, soap_action, &StreamUri_req, &StreamUri_resp);
		if (soap->error)
		{
			printf("[%s][%s][Line:%d] => soap error: %d, %s, %s\n",__FILE__, __func__, __LINE__, soap->error, *soap_faultcode(soap), *soap_faultstring(soap)); 
			retval = soap->error; 
			break;
		}
		else
		{
			memset(nvc_stream_url->url, 0, strlen(nvc_stream_url->url));
			strncpy(nvc_stream_url->url, StreamUri_resp.MediaUri->Uri, MAX_URL_LEN);
			printf("soap_call___trt__GetStreamUri               : %s\r\n", StreamUri_resp.MediaUri->Uri);
			#if 0
			printf("MediaUri->InvalidAfterConnect               : %d\r\n", StreamUri_resp.MediaUri->InvalidAfterConnect);
			printf("MediaUri->InvalidAfterReboot                : %d\r\n", StreamUri_resp.MediaUri->InvalidAfterReboot);
			printf("MediaUri->Timeout                           : %llu\r\n", StreamUri_resp.MediaUri->Timeout);
			printf("MediaUri->__size                            : %d\r\n", StreamUri_resp.MediaUri->__size);
			#endif
			printf("MediaUri->Uri                               : %s\r\n", nvc_stream_url->url);
		}
	}while(0);

	free(soap_endpoint);
	soap_endpoint = NULL;
	destroy_soap(soap);
  return  retval; 
}

static int s_parse_a_enc_type(int is_set, int enc_type)
{
	int ret = -1;
	if (is_set)
	{
		switch (enc_type)
		{
			case 1:
			case 2:
				ret = tt__AudioEncoding__G711;
				break;
			case 4:
			case 5:
			case 6:
			case 7:
				ret = tt__AudioEncoding__G726;
				break;
			case 10:
				ret = tt__AudioEncoding__AAC;
				break;
			case 13:
			case 14:
			case 15:
			case 16:
				ret = tt__AudioEncoding__G726;
				break;
			default: 
				ret = tt__AudioEncoding__G711;
				break;
		}
	}
	else
	{
		switch (enc_type)
		{
			case tt__AudioEncoding__G711:
				ret = 1;
				break;
			case tt__AudioEncoding__G726:
				ret = 4;
				break;
			case tt__AudioEncoding__AAC:
				ret = 10;
				break;
			default: 
				ret = 0;
				break;
		}
	}
	if (ret < 0)
	{
		printf("[%s][%s] => do not support such encoding type now! enc_type: %d\n",__FILE__, __func__, enc_type);
	}
	return ret;
}

int NVC_Get_Audio_info(NVC_Dev_t *dev, Audio_Enc_t *audio)
{
	int ret = 0;
	struct soap *soap = NULL;

	struct _trt__GetAudioEncoderConfiguration a_enc_req;
	struct _trt__GetAudioEncoderConfigurationResponse a_enc_resp;
    memset(&a_enc_resp, 0, sizeof(struct _trt__GetAudioEncoderConfigurationResponse));

	if (dev->profile_tn == NULL 
        || strlen(dev->profile_tn[dev->st_type].a_enc_token) < 1
        || dev->capa == NULL
        || dev->capa->media_capa == NULL
        || strlen(dev->capa->media_capa->url) < 1)
	{
		printf("[%s][%s] => Error: Invalid parameter!dev->profile_tn: %p, dev->profile_tn->profile_token: %p\n"
			, __FILE__, __func__, dev->profile_tn, dev->profile_tn[dev->st_type].a_enc_token);	
		return -1;
	}
	
	/*soap = (struct soap *)malloc(sizeof(struct soap));
	memset(soap, 0, sizeof(sizeof(struct soap)));*/
	struct SOAP_ENV__Header header;
	//soap = creat_soap(&header, was_To, was_Action, dev->timeout, 0);
	soap = creat_soap(&header, NULL, NULL, dev->timeout, 0, dev);
    if (soap == NULL)
    {
        printf("[%s][%d] => creat_soap failed!\n", __func__, __LINE__);
        return -1;
    }

	char *soap_endpoint = (char *)calloc(1, sizeof(char)*MAX_URL_LEN);//"soap.udp://239.255.255.250:3702/";
	strncpy(soap_endpoint, dev->capa->media_capa->url, MAX_URL_LEN);

	const char *soap_action = "http://www.onvif.org/ver10/media/wsdl/GetAudioEncoderConfiguration";
	
	a_enc_req.ConfigurationToken = (char *)soap_malloc(soap, sizeof(char)*INFO_LENGTH);
	memset(a_enc_req.ConfigurationToken, 0, sizeof(char)*INFO_LENGTH);
	strncpy(a_enc_req.ConfigurationToken, dev->profile_tn[dev->st_type].a_enc_token, sizeof(char)*INFO_LENGTH);

	do
	{
		soap_call___trt__GetAudioEncoderConfiguration(soap, soap_endpoint, soap_action, &a_enc_req, &a_enc_resp);
		if (soap->error)
		{
			printf("[%s][%s][Line:%d] => soap error: %d, %s, %s\n",__FILE__, __func__, __LINE__, soap->error, *soap_faultcode(soap), *soap_faultstring(soap)); 
			ret = soap->error; 
			break;
		}
		else 
		{
		    if (a_enc_resp.Configuration)
            {
                audio->enc_type = s_parse_a_enc_type(0, a_enc_resp.Configuration->Encoding);
            }
            else
            {
                printf("[%s][%d] => soap_call___trt__GetAudioEncoderConfiguration: Configuration = NULL\n", __func__, __LINE__);
            }
			break;
		}
	}while(0);

	free(soap_endpoint);
	soap_endpoint = NULL;
	destroy_soap(soap);

	return ret;
}

int NVC_Set_Audio_info(NVC_Dev_t *dev, Audio_Enc_t *audio)
{
	int ret = 0;
	struct soap *soap = NULL;
	
	struct _trt__SetAudioEncoderConfiguration a_enc_req;
	struct _trt__SetAudioEncoderConfigurationResponse a_enc_resp;
    memset(&a_enc_resp, 0, sizeof(struct _trt__SetAudioEncoderConfigurationResponse));

	if (dev->profile_tn == NULL 
        || strlen(dev->profile_tn[dev->st_type].a_enc_token) < 1
        || dev->capa == NULL
        || dev->capa->media_capa == NULL
        || strlen(dev->capa->media_capa->url) < 1)
	{
		printf("[%s][%s] => Error: Invalid parameter!dev->profile_tn: %p, dev->profile_tn->profile_token: %p\n"
			, __FILE__, __func__, dev->profile_tn, dev->profile_tn[dev->st_type].a_enc_token);	
		return -1;
	}

	/*soap = (struct soap *)malloc(sizeof(struct soap));
	memset(soap, 0, sizeof(sizeof(struct soap)));*/
	struct SOAP_ENV__Header header;
	//soap = creat_soap(&header, was_To, was_Action, dev->timeout, 0);
	soap = creat_soap(&header, NULL, NULL, dev->timeout, 0, dev);
    if (soap == NULL)
    {
        printf("[%s][%d] => creat_soap failed!\n", __func__, __LINE__);
        return -1;
    }
    
	char *soap_endpoint = (char *)calloc(1, sizeof(char)*MAX_URL_LEN);//"soap.udp://239.255.255.250:3702/";
	strncpy(soap_endpoint, dev->capa->media_capa->url, sizeof(char)*MAX_URL_LEN);

	const char *soap_action = "http://www.onvif.org/ver10/media/wsdl/SetAudioEncoderConfiguration";

	/************ 有些字段上层没有,所以在Set之前先Get一下,便于某些字段的填充 ***************/
	/****************************** Get enc configuration ********************************/
	struct _trt__GetAudioEncoderConfiguration a_get_enc_req;
	struct _trt__GetAudioEncoderConfigurationResponse a_get_enc_resp;
    memset(&a_enc_resp, 0, sizeof(struct _trt__GetAudioEncoderConfigurationResponse));
	
	a_get_enc_req.ConfigurationToken = (char *)soap_malloc(soap, sizeof(char)*INFO_LENGTH);
	memset(a_get_enc_req.ConfigurationToken, 0, sizeof(char)*INFO_LENGTH);
	strncpy(a_get_enc_req.ConfigurationToken, dev->profile_tn[dev->st_type].a_enc_token, strlen(dev->profile_tn[dev->st_type].a_enc_token));

	do
	{
		soap_call___trt__GetAudioEncoderConfiguration(soap, soap_endpoint, soap_action, &a_get_enc_req, &a_get_enc_resp);
		if (soap->error)
		{
			printf("[%s][%s][Line:%d] => soap error: %d, %s, %s\n",__FILE__, __func__, __LINE__, soap->error, *soap_faultcode(soap), *soap_faultstring(soap)); 
			ret = soap->error;
			free(soap_endpoint);
			soap_endpoint = NULL;
			destroy_soap(soap);
			return ret;
		}
	}while(0);
		
	/***************************** Set enc configuration *********************************/
	a_enc_req.Configuration = (struct tt__AudioEncoderConfiguration *)soap_malloc(soap, sizeof(struct tt__AudioEncoderConfiguration));
	memset(a_enc_req.Configuration, 0, sizeof(struct tt__AudioEncoderConfiguration));

	a_enc_req.Configuration->Name = (char *)soap_malloc(soap, sizeof(char)*MAX_NAME_LEN);
	memset(a_enc_req.Configuration->Name, 0, sizeof(char)*MAX_NAME_LEN);
	strncpy(a_enc_req.Configuration->Name, dev->profile_tn[dev->st_type].a_enc_name, sizeof(char)*MAX_NAME_LEN);
	
	a_enc_req.Configuration->token= (char *)soap_malloc(soap, sizeof(char)*INFO_LENGTH);
	memset(a_enc_req.Configuration->token, 0, sizeof(char)*INFO_LENGTH);
	strncpy(a_enc_req.Configuration->token, dev->profile_tn[dev->st_type].a_enc_token, sizeof(char)*INFO_LENGTH);
	
	a_enc_req.Configuration->Encoding = s_parse_a_enc_type(1, audio->enc_type);
	#if 1 /**上层没有的用Get出来的填充*/
    if (a_get_enc_resp.Configuration 
        && a_get_enc_resp.Configuration->Multicast)
    {
    	a_enc_req.Configuration->UseCount = a_get_enc_resp.Configuration->UseCount;
    	a_enc_req.Configuration->Bitrate = a_get_enc_resp.Configuration->Bitrate;
    	a_enc_req.Configuration->SampleRate = a_get_enc_resp.Configuration->SampleRate;

    	a_enc_req.Configuration->Multicast = (struct tt__MulticastConfiguration *)soap_malloc(soap, sizeof(struct tt__MulticastConfiguration));
    	memset(a_enc_req.Configuration->Multicast, 0, sizeof(struct tt__MulticastConfiguration));
        if (a_get_enc_resp.Configuration->Multicast->Address)
        {
            a_enc_req.Configuration->Multicast->Address = (struct tt__IPAddress *)soap_malloc(soap, sizeof(struct tt__IPAddress));
        	memset(a_enc_req.Configuration->Multicast->Address, 0, sizeof(struct tt__IPAddress));
        	a_enc_req.Configuration->Multicast->Address->Type = a_get_enc_resp.Configuration->Multicast->Address->Type;
        	if (tt__IPType__IPv4 == a_enc_req.Configuration->Multicast->Address->Type)
        	{
        		a_enc_req.Configuration->Multicast->Address->IPv4Address = (char **)soap_malloc(soap, sizeof(char *));
        		memset(a_enc_req.Configuration->Multicast->Address->IPv4Address, 0, sizeof(char **));
        		a_enc_req.Configuration->Multicast->Address->IPv4Address[0] = (char *)soap_malloc(soap, sizeof(char) * LARGE_INFO_LENGTH);
        		memset(a_enc_req.Configuration->Multicast->Address->IPv4Address[0], 0, sizeof(char)*LARGE_INFO_LENGTH);
        		if (a_get_enc_resp.Configuration->Multicast->Address->IPv4Address 
                    && a_get_enc_resp.Configuration->Multicast->Address->IPv4Address[0])
                {
                    strcpy(a_enc_req.Configuration->Multicast->Address->IPv4Address[0], a_get_enc_resp.Configuration->Multicast->Address->IPv4Address[0]);
                }
        	}
        	else
        	{
        		a_enc_req.Configuration->Multicast->Address->IPv6Address = (char **)soap_malloc(soap, sizeof(char *));
        		memset(a_enc_req.Configuration->Multicast->Address->IPv6Address, 0, sizeof(char **));
        		a_enc_req.Configuration->Multicast->Address->IPv6Address[0] = (char *)soap_malloc(soap, sizeof(char) * LARGE_INFO_LENGTH);
        		memset(a_enc_req.Configuration->Multicast->Address->IPv6Address[0], 0, sizeof(char)*LARGE_INFO_LENGTH);
        		if (a_get_enc_resp.Configuration->Multicast->Address->IPv6Address 
                    && a_get_enc_resp.Configuration->Multicast->Address->IPv6Address[0])
                {
                    strcpy(a_enc_req.Configuration->Multicast->Address->IPv6Address[0], a_get_enc_resp.Configuration->Multicast->Address->IPv6Address[0]);
                }
        	}
        }
    	
    	a_enc_req.Configuration->Multicast->Port = a_enc_req.Configuration->Multicast->Port;
    	a_enc_req.Configuration->Multicast->TTL = a_enc_req.Configuration->Multicast->TTL;
    	a_enc_req.Configuration->Multicast->AutoStart = a_enc_req.Configuration->Multicast->AutoStart;

    	a_enc_req.Configuration->SessionTimeout = a_enc_req.Configuration->SessionTimeout;
    	a_enc_req.ForcePersistence = 1;	/**true*/
    }
	#endif

	do
	{
		soap_call___trt__SetAudioEncoderConfiguration(soap, soap_endpoint, soap_action, &a_enc_req, &a_enc_resp);
		if (soap->error)
		{
			printf("[%s][%s][Line:%d] => soap error: %d, %s, %s\n",__FILE__, __func__, __LINE__, soap->error, *soap_faultcode(soap), *soap_faultstring(soap)); 
			ret = soap->error; 
			break;
		}
		else 
		{
			break;
		}
	}while(0);

	free(soap_endpoint);
	soap_endpoint = NULL;
	destroy_soap(soap);

	return ret;
}

static int s_parse_v_enc_type(int is_set, int enc_type)
{
	int ret = -1;
	if (is_set)
	{
		switch (enc_type)
		{
			case 0:
				ret = tt__VideoEncoding__H264;
				break;
			case 1:
				ret = tt__VideoEncoding__MPEG4;
				break;
			#if 0
			case :
				ret = tt__VideoEncoding__MPEG4;
				break;
			#endif
			default:
				ret = tt__VideoEncoding__H264;
				break;
		}
	}
	else
	{
		switch (enc_type)
		{
			case tt__VideoEncoding__JPEG:
				ret = -1;
				break;
			case tt__VideoEncoding__MPEG4:
				ret = 1;
				break;
			case tt__VideoEncoding__H264:
				ret = 0;
				break;
			default:
				ret = -1;
				break;
		}
	}
	if (ret < 0)
	{
		printf("[%s][%s] => do not support such encoding type now! enc_type: %d\n",__FILE__, __func__, enc_type);
	}
	return ret;
}

static int s_parse_v_enc_level(int is_set, int level)
{
	int ret = -1;
	if (is_set)
	{
		switch (level)
		{
			case 0:
				ret = tt__H264Profile__Baseline;
				break;
			case 1:
				ret = tt__H264Profile__Main;
				break;
			case 2:
				ret = tt__H264Profile__High;
				break;
			default:
				ret = tt__H264Profile__Baseline;
				break;
		}
	}
	else
	{
		switch (level)
		{
			case tt__H264Profile__Baseline:
				ret = 0;
				break;
			case tt__H264Profile__Main:
				ret = 1;
				break;
			case tt__H264Profile__Extended:
				ret = 1;
				break;
			case tt__H264Profile__High:
				ret = 2;
				break;
			default:
				ret = 0;
				break;
		}
	}
	if (ret < 0)
	{
		printf("[%s][%s] => do not support such stream level! level: %d\n",__FILE__, __func__, level);
	}
	return ret;
}

int NVC_Get_Video_info(NVC_Dev_t *dev, Video_Enc_t *video)
{
	int ret = 0;
	struct soap *soap = NULL;

	struct _trt__GetVideoEncoderConfiguration v_enc_req;
	struct _trt__GetVideoEncoderConfigurationResponse v_enc_resp;
    memset(&v_enc_resp, 0, sizeof(struct _trt__GetVideoEncoderConfigurationResponse));

	if (dev->profile_tn == NULL 
        || strlen(dev->profile_tn[dev->st_type].v_enc_token) < 1 
        || dev->capa == NULL
        || dev->capa->media_capa == NULL
        || strlen(dev->capa->media_capa->url) < 1)
	{
		printf("[%s][%s] => Error: Invalid parameter!dev->profile_tn: %p, dev->profile_tn->profile_token: %p\n"
			, __FILE__, __func__, dev->profile_tn, dev->profile_tn[dev->st_type].v_enc_token);	
		return -1;
	}

	
	//const char *was_To 			= "";
	//const char *was_Action 	= "";
	
	/*soap = (struct soap *)malloc(sizeof(struct soap));
	memset(soap, 0, sizeof(sizeof(struct soap)));*/
	struct SOAP_ENV__Header header;
	//soap = creat_soap(&header, was_To, was_Action, dev->timeout, 0);
	soap = creat_soap(&header, NULL, NULL, dev->timeout, 0, dev);
    if (soap == NULL)
    {
        printf("[%s][%d] => creat_soap failed!\n", __func__, __LINE__);
        return -1;
    }


	char *soap_endpoint = (char *)calloc(1, sizeof(char)*MAX_URL_LEN);//"soap.udp://239.255.255.250:3702/";
	strncpy(soap_endpoint, dev->capa->media_capa->url, sizeof(char)*MAX_URL_LEN);

	const char *soap_action = "http://www.onvif.org/ver10/media/wsdl/GetVideoEncoderConfiguration";
	
	v_enc_req.ConfigurationToken = (char *)soap_malloc(soap, sizeof(char)*INFO_LENGTH);
	memset(v_enc_req.ConfigurationToken, 0, sizeof(char)*INFO_LENGTH);
	strncpy(v_enc_req.ConfigurationToken, dev->profile_tn[dev->st_type].v_enc_token, sizeof(char)*INFO_LENGTH);

  //fprintf(stderr, "[%s] configurationtoken=%s dev=%s\n", __FUNCTION__, v_enc_req.ConfigurationToken, dev->profile_tn[dev->st_type].v_enc_token);
	do
	{
		soap_call___trt__GetVideoEncoderConfiguration(soap, soap_endpoint, soap_action, &v_enc_req, &v_enc_resp);
		if (soap->error)
		{
			printf("[%s][%s][Line:%d] => soap error: %d, %s, %s\n",__FILE__, __func__, __LINE__, soap->error, *soap_faultcode(soap), *soap_faultstring(soap)); 
			ret = soap->error; 
			break;
		}
		else 
		{
		    if (v_enc_resp.Configuration)
            {
                video->enc_type = s_parse_v_enc_type(0, v_enc_resp.Configuration->Encoding);
                if (v_enc_resp.Configuration->Resolution)
                {
                    video->resolution.width = v_enc_resp.Configuration->Resolution->Width;
        			      video->resolution.height = v_enc_resp.Configuration->Resolution->Height;
                    //printf("[%s][%d] => width%d height%d\n", __func__, __LINE__, video->resolution.width, video->resolution.height);

                }
                else
                {
                    printf("[%s][%d] => soap_call___trt__GetVideoEncoderConfiguration: Resolution = NULL\n", __func__, __LINE__);
                }
    			//video->pic_quilty = ((v_enc_resp.Configuration->Quality * 6) / 101);
                video->pic_quilty = v_enc_resp.Configuration->Quality;
                if (v_enc_resp.Configuration->RateControl)
                {
                    video->frame_rate = v_enc_resp.Configuration->RateControl->FrameRateLimit;
        			video->bitrate = v_enc_resp.Configuration->RateControl->BitrateLimit;
                }
                else
                {
                    printf("[%s][%d] => soap_call___trt__GetVideoEncoderConfiguration: RateControl = NULL\n", __func__, __LINE__);
                }
                if (v_enc_resp.Configuration->H264)
                {
                    video->gop = v_enc_resp.Configuration->H264->GovLength;
                    video->level = s_parse_v_enc_level(0, v_enc_resp.Configuration->H264->H264Profile);
                }
                else
                {
                    printf("[%s][%d] => soap_call___trt__GetVideoEncoderConfiguration: H264 = NULL\n", __func__, __LINE__);
                }
            }
            else
            {
                printf("[%s][%d] => soap_call___trt__GetVideoEncoderConfiguration: Configuration = NULL\n", __func__, __LINE__);
            }
			break;
		}
	}while(0);

	free(soap_endpoint);
	soap_endpoint = NULL;
	destroy_soap(soap);

	return ret;
}

int NVC_Set_Video_info(NVC_Dev_t *dev, Video_Enc_t *video)
{
	int ret = 0;
	struct soap *soap = NULL;

	struct _trt__GetVideoEncoderConfiguration v_get_enc_req;
	struct _trt__GetVideoEncoderConfigurationResponse v_get_enc_resp;
    memset(&v_get_enc_resp, 0, sizeof(struct _trt__GetVideoEncoderConfigurationResponse));
	
	struct _trt__SetVideoEncoderConfiguration v_enc_req;
	struct _trt__SetVideoEncoderConfigurationResponse v_enc_resp;
    memset(&v_enc_resp, 0, sizeof(struct _trt__SetVideoEncoderConfigurationResponse));

	if (dev->profile_tn == NULL 
        || strlen(dev->profile_tn[dev->st_type].v_enc_token) < 1
        || dev->capa == NULL
        || dev->capa->media_capa == NULL
        || strlen(dev->capa->media_capa->url) < 1)
	{
		printf("[%s][%s] => Error: Invalid parameter!dev->profile_tn: %p, dev->profile_tn->profile_token: %p\n"
			, __FILE__, __func__, dev->profile_tn, dev->profile_tn[dev->st_type].v_enc_token);	
		return -1;
	}

	if (strlen(dev->profile_tn->v_enc_token) < 1)
	{
		printf("[%s][%s] => Error: Invalid parameter!input profile_token = NULL\n", __FILE__, __func__);	
		return -1;
	}
	
	//const char *was_To = "";
	//const char *was_Action = "";
	
	/*soap = (struct soap *)malloc(sizeof(struct soap));
	memset(soap, 0, sizeof(sizeof(struct soap)));*/
	struct SOAP_ENV__Header header;
	//soap = creat_soap(&header, was_To, was_Action, dev->timeout, 0);
    soap = creat_soap(&header, NULL, NULL, dev->timeout, 0, dev);
    if (soap == NULL)
    {
        printf("[%s][%d] => creat_soap failed!\n", __func__, __LINE__);
        return -1;
    }

	char *soap_endpoint = (char *)calloc(1, sizeof(char)*MAX_URL_LEN);//"soap.udp://239.255.255.250:3702/";
	strncpy(soap_endpoint, dev->capa->media_capa->url, sizeof(char)*MAX_URL_LEN);

	const char *soap_action = "http://www.onvif.org/ver10/media/wsdl/GetVideoEncoderConfiguration";

	/************ 有些字段上层没有,所以在Set之前先Get一下,便于某些字段的填充 ***************/
	/****************************** Get enc configuration ********************************/
	v_get_enc_req.ConfigurationToken = (char *)soap_malloc(soap, sizeof(char)*INFO_LENGTH);
	memset(v_get_enc_req.ConfigurationToken, 0, sizeof(char)*INFO_LENGTH);
	strncpy(v_get_enc_req.ConfigurationToken, dev->profile_tn[dev->st_type].v_enc_token, sizeof(char)*INFO_LENGTH);

	do
	{
		soap_call___trt__GetVideoEncoderConfiguration(soap, soap_endpoint, soap_action, &v_get_enc_req, &v_get_enc_resp);
		if (soap->error)
		{
			printf("[%s][%s][Line:%d] => soap error: %d, %s, %s\n",__FILE__, __func__, __LINE__, soap->error, *soap_faultcode(soap), *soap_faultstring(soap)); 
			ret = soap->error;
			free(soap_endpoint);
			soap_endpoint = NULL;
			destroy_soap(soap);
			return ret;
		}
	}while(0);
		
	/***************************** Set enc configuration *********************************/
	v_enc_req.Configuration = (struct tt__VideoEncoderConfiguration *)soap_malloc(soap, sizeof(struct tt__VideoEncoderConfiguration));
	memset(v_enc_req.Configuration, 0, sizeof(struct tt__VideoEncoderConfiguration));

	v_enc_req.Configuration->Name = (char *)soap_malloc(soap, sizeof(char)*MAX_NAME_LEN);
	memset(v_enc_req.Configuration->Name, 0, sizeof(char)*MAX_NAME_LEN);
	strncpy(v_enc_req.Configuration->Name, dev->profile_tn[dev->st_type].v_enc_name, sizeof(char)*MAX_NAME_LEN);

	v_enc_req.Configuration->token= (char *)soap_malloc(soap, sizeof(char)*INFO_LENGTH);
	memset(v_enc_req.Configuration->token, 0, sizeof(char)*INFO_LENGTH);
	strncpy(v_enc_req.Configuration->token, dev->profile_tn[dev->st_type].v_enc_token, sizeof(char)*INFO_LENGTH);
	
	v_enc_req.Configuration->UseCount = v_get_enc_resp.Configuration->UseCount;	/**上层没有的同Get出来的填充*/
	v_enc_req.Configuration->Encoding = s_parse_v_enc_type(1, video->enc_type);

	v_enc_req.Configuration->Resolution = (struct tt__VideoResolution *)soap_malloc(soap, sizeof(struct tt__VideoResolution));
	memset(v_enc_req.Configuration->Resolution, 0, sizeof(struct tt__VideoResolution));
	v_enc_req.Configuration->Resolution->Width = video->resolution.width;
	v_enc_req.Configuration->Resolution->Height = video->resolution.height;
   
	//v_enc_req.Configuration->Quality = (video->pic_quilty * 101) / 6;
    v_enc_req.Configuration->Quality = video->pic_quilty;
	v_enc_req.Configuration->RateControl = (struct tt__VideoRateControl *)soap_malloc(soap, sizeof(struct tt__VideoRateControl));
	memset(v_enc_req.Configuration->RateControl, 0, sizeof(struct tt__VideoRateControl));
	v_enc_req.Configuration->RateControl->FrameRateLimit = video->frame_rate;
	v_enc_req.Configuration->RateControl->EncodingInterval = v_get_enc_resp.Configuration->RateControl->EncodingInterval;	/**上层没有的用Get出来的填充*/
	v_enc_req.Configuration->RateControl->BitrateLimit = video->bitrate;

    v_enc_req.Configuration->H264 = (struct tt__H264Configuration *)soap_malloc(soap, sizeof(struct tt__H264Configuration));
	memset(v_enc_req.Configuration->H264, 0, sizeof(struct tt__H264Configuration));
	v_enc_req.Configuration->H264->GovLength = video->gop;
	v_enc_req.Configuration->H264->H264Profile = s_parse_v_enc_level(1, video->level);
        
	#if 1 /**上层没有的同Get出来的填充*/
    if (v_get_enc_resp.Configuration) 
    {
        fprintf(stderr, "[%s] have configuration!\n", __FUNCTION__);
        if (v_get_enc_resp.Configuration->MPEG4)
    	{
            fprintf(stderr, "[%s] have MPEG4!\n", __FUNCTION__);
        	v_enc_req.Configuration->MPEG4 = (struct tt__Mpeg4Configuration *)soap_malloc(soap, sizeof(struct tt__Mpeg4Configuration));
        	memset(v_enc_req.Configuration->MPEG4, 0, sizeof(struct tt__Mpeg4Configuration));
        	v_enc_req.Configuration->MPEG4->GovLength = v_get_enc_resp.Configuration->MPEG4->GovLength;
        	v_enc_req.Configuration->MPEG4->Mpeg4Profile = v_get_enc_resp.Configuration->MPEG4->Mpeg4Profile;
        }
        else
        {
            fprintf(stderr, "[%s] MPEG4 not exist!\n", __FUNCTION__);
            v_enc_req.Configuration->MPEG4 = NULL;
        }

        v_enc_req.Configuration->Multicast = v_get_enc_resp.Configuration->Multicast;
#if 0
        if (v_get_enc_resp.Configuration->Multicast 
            && v_get_enc_resp.Configuration->Multicast->Address)
        {
            fprintf(stderr, "[%s] have Multicast!\n", __FUNCTION__);
            v_enc_req.Configuration->Multicast = (struct tt__MulticastConfiguration *)soap_malloc(soap, sizeof(struct tt__MulticastConfiguration));
        	memset(v_enc_req.Configuration->Multicast, 0, sizeof(struct tt__MulticastConfiguration));
        	v_enc_req.Configuration->Multicast->Address = (struct tt__IPAddress *)soap_malloc(soap, sizeof(struct tt__IPAddress));
        	memset(v_enc_req.Configuration->Multicast->Address, 0, sizeof(struct tt__IPAddress));
        	v_enc_req.Configuration->Multicast->Address->Type = v_get_enc_resp.Configuration->Multicast->Address->Type;
        	if (tt__IPType__IPv4 == v_enc_req.Configuration->Multicast->Address->Type)
        	{
                if(v_get_enc_resp.Configuration->Multicast->Address->IPv4Address)
                {
                    
                    v_enc_req.Configuration->Multicast->Address->IPv4Address = NULL;
                }
                else
                {
            		v_enc_req.Configuration->Multicast->Address->IPv4Address = (char **)soap_malloc(soap, sizeof(char *));
            		memset(v_enc_req.Configuration->Multicast->Address->IPv4Address, 0, sizeof(char *));
            		v_enc_req.Configuration->Multicast->Address->IPv4Address[0] = (char *)soap_malloc(soap, sizeof(char) * LARGE_INFO_LENGTH);
            		memset(v_enc_req.Configuration->Multicast->Address->IPv4Address[0], 0, sizeof(char)*LARGE_INFO_LENGTH);
                    if (v_get_enc_resp.Configuration->Multicast->Address->IPv4Address 
                        && v_get_enc_resp.Configuration->Multicast->Address->IPv4Address[0])
                    {
                        strcpy(v_enc_req.Configuration->Multicast->Address->IPv4Address[0], v_get_enc_resp.Configuration->Multicast->Address->IPv4Address[0]);
                    }
                }    
        	}
        	else
        	{
        		v_enc_req.Configuration->Multicast->Address->IPv6Address = (char **)soap_malloc(soap, sizeof(char *));
        		memset(v_enc_req.Configuration->Multicast->Address->IPv6Address, 0, sizeof(char **));
        		v_enc_req.Configuration->Multicast->Address->IPv6Address[0] = (char *)soap_malloc(soap, sizeof(char) * LARGE_INFO_LENGTH);
        		memset(v_enc_req.Configuration->Multicast->Address->IPv6Address[0], 0, sizeof(char)*LARGE_INFO_LENGTH);
                if (v_get_enc_resp.Configuration->Multicast->Address->IPv6Address 
                    && v_get_enc_resp.Configuration->Multicast->Address->IPv6Address[0])
                {
                    strcpy(v_enc_req.Configuration->Multicast->Address->IPv6Address[0], v_get_enc_resp.Configuration->Multicast->Address->IPv6Address[0]);
                }
        	}

        	v_enc_req.Configuration->Multicast->Port = v_get_enc_resp.Configuration->Multicast->Port;
        	v_enc_req.Configuration->Multicast->TTL = v_get_enc_resp.Configuration->Multicast->TTL;
        	v_enc_req.Configuration->Multicast->AutoStart = v_get_enc_resp.Configuration->Multicast->AutoStart; /**false*/
        }
        else
        {
            fprintf(stderr, "[%s] Multicast not exist!\n", __FUNCTION__);
            v_enc_req.Configuration->Multicast = NULL;
            
        }
#endif

        v_enc_req.Configuration->SessionTimeout = v_get_enc_resp.Configuration->SessionTimeout;//12*60; /**hongmb: 12*60s, do not sure*/
        v_enc_req.ForcePersistence = 1; /**true*/
    }
	#endif

    fprintf(stderr, "[%s] Encoding:%d FrameRateLimit:%d BitrateLimit:%d GovLength:%d\n", __FUNCTION__, 
                    v_enc_req.Configuration->Encoding, 
                    v_enc_req.Configuration->RateControl->FrameRateLimit,
                    v_enc_req.Configuration->RateControl->BitrateLimit,
                    v_enc_req.Configuration->H264->GovLength);

    fprintf(stderr, "[%s] name:%s token:%s height:%d width:%d \n", __FUNCTION__, 
                    v_enc_req.Configuration->Name, 
                    v_enc_req.Configuration->token,
                    v_enc_req.Configuration->Resolution->Width,
                    v_enc_req.Configuration->Resolution->Height);

    const char *_soap_action = "http://www.onvif.org/ver10/media/wsdl/SetVideoEncoderConfiguration";
    
	do
	{
		soap_call___trt__SetVideoEncoderConfiguration(soap, soap_endpoint, _soap_action, &v_enc_req, &v_enc_resp);
		if (soap->error)
		{
			printf("[%s][%s][Line:%d] => soap error: %d, %s, %s\n",__FILE__, __func__, __LINE__, soap->error, *soap_faultcode(soap), *soap_faultstring(soap)); 
			ret = soap->error; 
			break;
		}
		else 
		{
			break;
		}
	}while(0);

	free(soap_endpoint);
	soap_endpoint = NULL;
	destroy_soap(soap);

	return ret;
}

/************************************* PTZ *******************************************/
int NVC_PTZ_Get_Configurations(NVC_Dev_t * dev, NVC_PTZ_Configution_t * ptz_conf)
{
    struct soap *soap = NULL;
	int retval = 0;
	struct _tptz__GetConfiguration  tptz_GetConfreq;
    struct _tptz__GetConfigurationResponse tptz_GetConfresp;
    memset(&tptz_GetConfresp, 0, sizeof(struct _tptz__GetConfigurationsResponse));
    
    if (dev == NULL 
        || dev->profile_tn == NULL 
        || strlen(dev->profile_tn[dev->st_type].PTZ_parm_token) < 1
        || dev->capa == NULL 
        || dev->capa->PTZ_capa == NULL 
        || strlen(dev->capa->PTZ_capa->url) < 1)        
	{
		printf("[%s][%s] => Error: Invalid parameter! dev->profile_tn:%p, dev->profile_tn->profile_token:[%s]\n"
			, __FILE__, __func__, dev->profile_tn, dev->profile_tn[dev->st_type].PTZ_parm_token);	
		return -1;
	}
	
	//const char *was_To = "";
	//const char *was_Action = "";
	
	/*soap = (struct soap *)malloc(sizeof(struct soap));
	memset(soap, 0, sizeof(sizeof(struct soap)));*/
	struct SOAP_ENV__Header header;
	//soap = creat_soap(&header, was_To, was_Action, dev->timeout, 0);
    soap = creat_soap(&header, NULL, NULL, dev->timeout, 0, dev);
    if (soap == NULL)
    {
        printf("[%s][%d] => creat_soap failed!\n", __func__, __LINE__);
        return -1;
    }
    


	char *soap_endpoint = (char *)calloc(1, sizeof(char)*MAX_URL_LEN);//"soap.udp://239.255.255.250:3702/";
	strncpy(soap_endpoint, dev->capa->PTZ_capa->url, sizeof(char)*MAX_URL_LEN);

    const char *soap_action = "http://www.onvif.org/ver20/ptz/wsdl/GetConfigurations";
    
    printf("[%s][%d] => profiletoken: %s\n", __func__, __LINE__, dev->profile_tn[dev->st_type].profile_token);
    tptz_GetConfreq.PTZConfigurationToken = (char *)soap_malloc(soap, sizeof(char)*INFO_LENGTH);
    memset(tptz_GetConfreq.PTZConfigurationToken, 0, sizeof(char)*INFO_LENGTH);
    strncpy(tptz_GetConfreq.PTZConfigurationToken, dev->profile_tn[dev->st_type].PTZ_parm_token, sizeof(char)*INFO_LENGTH);
 
   do
    {   
        soap_call___tptz__GetConfiguration(soap, soap_endpoint, soap_action, &tptz_GetConfreq, &tptz_GetConfresp);
        if (soap->error)
        {
            printf("[%s][%s][Line:%d] => soap error: %d, %s, %s\n",__FILE__, __func__, __LINE__, soap->error, *soap_faultcode(soap), *soap_faultstring(soap)); 
			retval = soap->error; 
			break;
		}
		else 
		{
		    if (tptz_GetConfresp.PTZConfiguration)
            {
                if (tptz_GetConfresp.PTZConfiguration->DefaultContinuousPanTiltVelocitySpace)
                {
                    strncpy(ptz_conf->ContinuousPanTiltSpace, tptz_GetConfresp.PTZConfiguration->DefaultContinuousPanTiltVelocitySpace, sizeof(ptz_conf->ContinuousPanTiltSpace));
                }
                if (tptz_GetConfresp.PTZConfiguration->DefaultContinuousZoomVelocitySpace)
                {
                    strncpy(ptz_conf->ContinuousZoomSpace, tptz_GetConfresp.PTZConfiguration->DefaultContinuousZoomVelocitySpace, sizeof(ptz_conf->ContinuousZoomSpace));
                }
            }      
			break;
		}
    }while(0);

    if (soap_endpoint)
    {
        free(soap_endpoint);
    	soap_endpoint = NULL;
	}
    destroy_soap(soap);
    
    return retval;
}



int NVC_PTZ_ContinusMove(NVC_Dev_t *dev, NVC_PTZ_Speed_t *ptz_speed)
{
	struct soap *soap = NULL;
	int retval = 0;
	struct _tptz__ContinuousMove tptz_ContinuousMove_req; 
	struct _tptz__ContinuousMoveResponse tptz_ContinuousMove_resp;
    memset(&tptz_ContinuousMove_resp, 0, sizeof(struct _tptz__ContinuousMoveResponse));
    
	if (dev == NULL 
        || dev->profile_tn == NULL 
        || strlen(dev->profile_tn[dev->st_type].PTZ_parm_token) < 1
        || dev->ptz_conf == NULL
        || dev->capa == NULL 
        || dev->capa->PTZ_capa == NULL 
        || strlen(dev->capa->PTZ_capa->url) < 0)
	{
		printf("[%s][%s] => Error: Invalid parameter!dev->profile_tn:%p, dev->profile_tn->profile_token:[%s]\n"
			, __FILE__, __func__, dev->profile_tn, dev->profile_tn[dev->st_type].PTZ_parm_token);	
		return -1;
	}
	
	//const char *was_To = "";
	//const char *was_Action = "";
	
	/*soap = (struct soap *)malloc(sizeof(struct soap));
	memset(soap, 0, sizeof(sizeof(struct soap)));*/
	struct SOAP_ENV__Header header;
	//soap = creat_soap(&header, was_To, was_Action, dev->timeout, 0);
    soap = creat_soap(&header, NULL, NULL, dev->timeout, 0, dev);
    if (soap == NULL)
    {
        printf("[%s][%d] => creat_soap failed!\n", __func__, __LINE__);
        return -1;
    }
    


	char *soap_endpoint = (char *)calloc(1, sizeof(char)*MAX_URL_LEN);//"soap.udp://239.255.255.250:3702/";
	strncpy(soap_endpoint, dev->capa->PTZ_capa->url, sizeof(char)*MAX_URL_LEN);

	const char *soap_action = "http://www.onvif.org/ver20/ptz/wsdl/ContinuousMove";

  #if 1
  printf("[%s][%d] => soap_endpoint: %s\n", __func__, __LINE__, soap_endpoint);
  printf("[%s][%d] =>111 profiletoken: %s, pantilt->x: %f, pantile->y: %f, space: %s, zoom->x: %f, space: %s\n"
      , __func__, __LINE__
      , dev->profile_tn[dev->st_type].profile_token
      , ptz_speed->pantilt.x, ptz_speed->pantilt.y
      , dev->ptz_conf->ContinuousPanTiltSpace
      , ptz_speed->zoom.x
      , dev->ptz_conf->ContinuousZoomSpace);
  printf("====================================================================================\n");
  #endif
    
	tptz_ContinuousMove_req.ProfileToken = (char *)soap_malloc(soap, sizeof(char) * INFO_LENGTH);
	memset( tptz_ContinuousMove_req.ProfileToken, 0, sizeof(char) * INFO_LENGTH);
	strncpy( tptz_ContinuousMove_req.ProfileToken, dev->profile_tn[dev->st_type].profile_token, sizeof(char) * INFO_LENGTH);

    #if 1
    tptz_ContinuousMove_req.Timeout = NULL;
    #else
    tptz_ContinuousMove_req.Timeout = (LONG64 *)soap_malloc(soap, sizeof(LONG64));
	memset(tptz_ContinuousMove_req.Timeout, 0, sizeof(LONG64));
	*(tptz_ContinuousMove_req.Timeout) = 5*1000;
    printf(" => *(tptz_ContinuousMove_req.Timeout): %lld\n", *(tptz_ContinuousMove_req.Timeout));
    #endif
    tptz_ContinuousMove_req.Velocity = (struct tt__PTZSpeed *)soap_malloc(soap, sizeof(struct tt__PTZSpeed));
    memset(tptz_ContinuousMove_req.Velocity, 0, sizeof(struct tt__PTZSpeed));

    printf("[%s][%d] =>111  pantilt->x: %f, pantile->y: %f,zoom->x: %f\n"
        , __func__, __LINE__
        , fabs(ptz_speed->pantilt.x), fabs(ptz_speed->pantilt.y)
        , fabs(ptz_speed->zoom.x));

    if((fabs(ptz_speed->pantilt.x) > 0.0) || (fabs(ptz_speed->pantilt.y) > 0.0))
    {
        fprintf(stderr, "[%s]  pantilt \n", __FUNCTION__);
    	tptz_ContinuousMove_req.Velocity->PanTilt = (struct tt__Vector2D *)soap_malloc(soap, sizeof(struct tt__Vector2D));
    	memset(tptz_ContinuousMove_req.Velocity->PanTilt, 0, sizeof(struct tt__Vector2D));
    	tptz_ContinuousMove_req.Velocity->PanTilt->x = ptz_speed->pantilt.x;
    	tptz_ContinuousMove_req.Velocity->PanTilt->y = ptz_speed->pantilt.y;
    }
#if 0
	tptz_ContinuousMove_req.Velocity->PanTilt->space = (char *)soap_malloc(soap, sizeof(char)*MAX_INFO_LEN);
	memset(tptz_ContinuousMove_req.Velocity->PanTilt->space, 0, sizeof(char)*MAX_INFO_LEN);
	if (strlen(dev->ptz_conf->ContinuousPanTiltSpace) < 1)
    {
        tptz_ContinuousMove_req.Velocity->PanTilt->space = NULL;
    }
    else
    {
        strncpy(tptz_ContinuousMove_req.Velocity->PanTilt->space, dev->ptz_conf->ContinuousPanTiltSpace, sizeof(char)*MAX_INFO_LEN);
    }
#endif
#if 1
    if(fabs(ptz_speed->zoom.x) > 0.0)
    {
        fprintf(stderr, "[%s]  zoom \n", __FUNCTION__);
    	tptz_ContinuousMove_req.Velocity->Zoom = (struct tt__Vector1D *)soap_malloc(soap, sizeof(struct tt__Vector1D));
    	memset(tptz_ContinuousMove_req.Velocity->Zoom, 0, sizeof(struct tt__Vector1D));
    	tptz_ContinuousMove_req.Velocity->Zoom->x = ptz_speed->zoom.x;
    }
#endif
#if 0
	tptz_ContinuousMove_req.Velocity->Zoom->space = (char *)soap_malloc(soap, sizeof(char)*MAX_INFO_LEN);
	memset(tptz_ContinuousMove_req.Velocity->Zoom->space, 0, sizeof(char)*MAX_INFO_LEN);
    if (strlen(dev->ptz_conf->ContinuousZoomSpace) < 1)
    {
        tptz_ContinuousMove_req.Velocity->Zoom->space = NULL;
    }
    else
    {
        strncpy(tptz_ContinuousMove_req.Velocity->Zoom->space, dev->ptz_conf->ContinuousZoomSpace, sizeof(char)*MAX_INFO_LEN);
    }
#endif
    #if 0
    printf("####################################################################################\n");
    
    printf("[%s][%d] =>222 profiletoken: %s, pantilt->x: %f, pantile->y: %f, space: %s, zoom->x: %f, space: %s\n"
        , __func__, __LINE__
        , tptz_ContinuousMove_req.ProfileToken
        , tptz_ContinuousMove_req.Velocity->PanTilt->x, tptz_ContinuousMove_req.Velocity->PanTilt->y
        , tptz_ContinuousMove_req.Velocity->PanTilt->space
        , tptz_ContinuousMove_req.Velocity->Zoom->x
        , tptz_ContinuousMove_req.Velocity->Zoom->space);
    #endif

#if 0
    tptz_ContinuousMove_req.Velocity->PanTilt->space = NULL;
    tptz_ContinuousMove_req.Velocity->Zoom = NULL;
#endif
	do 
	{
		soap_call___tptz__ContinuousMove(soap, soap_endpoint, soap_action, &tptz_ContinuousMove_req, &tptz_ContinuousMove_resp);
		if (soap->error)
		{
			printf("[%s][%s][Line:%d] => soap error: %d, %s, %s\n",__FILE__, __func__, __LINE__, soap->error, *soap_faultcode(soap), *soap_faultstring(soap)); 
			retval = soap->error; 
			break;
		}
		else 
		{
			break;
		}
	}while(0);

	free(soap_endpoint);
	soap_endpoint = NULL;
	destroy_soap(soap);

	return 0;
}

int NVC_PTZ_Stop(NVC_Dev_t *dev, NVC_PTZ_Stop_t *ptz_stop)
{
    struct soap *soap = NULL;
	int retval = 0;
	struct _tptz__Stop tptz_Stop_req; 
	struct _tptz__StopResponse tptz_Stop_resp;
    memset(&tptz_Stop_resp, 0, sizeof(struct _tptz__StopResponse));
    
	if (dev == NULL 
        || dev->profile_tn == NULL 
        || strlen(dev->profile_tn[dev->st_type].PTZ_parm_token) < 1
        || dev->capa == NULL 
        || dev->capa->PTZ_capa == NULL 
        || strlen(dev->capa->PTZ_capa->url) < 1)
	{
		printf("[%s][%s] => Error: Invalid parameter!dev->profile_tn: %p, dev->profile_tn->profile_token:[%s]\n"
			, __FILE__, __func__, dev->profile_tn, dev->profile_tn[dev->st_type].PTZ_parm_token);	
		return -1;
	}
	
	//const char *was_To = "";
	//const char *was_Action = "";
	
	/*soap = (struct soap *)malloc(sizeof(struct soap));
	memset(soap, 0, sizeof(sizeof(struct soap)));*/
	struct SOAP_ENV__Header header;
	//soap = creat_soap(&header, was_To, was_Action, dev->timeout, 0);
    soap = creat_soap(&header, NULL, NULL, dev->timeout, 0, dev);
    if (soap == NULL)
    {
        printf("[%s][%d] => creat_soap failed!\n", __func__, __LINE__);
        return -1;
    }
  
  
	char *soap_endpoint = (char *)calloc(1, sizeof(char)*MAX_URL_LEN);//"soap.udp://239.255.255.250:3702/";
	strncpy(soap_endpoint, dev->capa->PTZ_capa->url, sizeof(char)*MAX_URL_LEN);

	const char *soap_action = "http://www.onvif.org/ver20/ptz/wsdl/Stop";

    #if 1
    printf("[%s][%d] =>111 profiletoken: %s, en_pant: %d, en_zoom: %d\n"
        , __func__, __LINE__
        , dev->profile_tn[dev->st_type].profile_token
        , ptz_stop->en_pant
        , ptz_stop->en_zoom);
    printf("====================================================================================\n");
    #endif
    
    tptz_Stop_req.ProfileToken = (char *)soap_malloc(soap, sizeof(char) * INFO_LENGTH);
	memset(tptz_Stop_req.ProfileToken, 0, sizeof(char) * INFO_LENGTH);
	strncpy(tptz_Stop_req.ProfileToken, dev->profile_tn[dev->st_type].profile_token, sizeof(char) * INFO_LENGTH);

    tptz_Stop_req.PanTilt = (enum xsd__boolean_ *)soap_malloc(soap, sizeof(enum xsd__boolean_));
	memset(tptz_Stop_req.PanTilt, 0, sizeof(enum xsd__boolean_));
    *(tptz_Stop_req.PanTilt) = ptz_stop->en_pant;

    tptz_Stop_req.Zoom = (enum xsd__boolean_ *)soap_malloc(soap, sizeof(enum xsd__boolean_));
	memset(tptz_Stop_req.Zoom, 0, sizeof(enum xsd__boolean_));
    *(tptz_Stop_req.Zoom) = ptz_stop->en_zoom;
    #if 1
    printf("####################################################################################\n");
    
    printf("[%s][%d] =>222 profiletoken: %s, PanTilt: %d, Zoom: %d\n"
        , __func__, __LINE__
        , tptz_Stop_req.ProfileToken
        , *(tptz_Stop_req.PanTilt)
        , *(tptz_Stop_req.Zoom));
    #endif
    
    do
    {
        soap_call___tptz__Stop(soap, soap_endpoint, soap_action, &tptz_Stop_req, &tptz_Stop_resp);
        if (soap->error)
        {
            printf("[%s][%s][Line:%d] => soap error: %d, %s, %s\n",__FILE__, __func__, __LINE__, soap->error, *soap_faultcode(soap), *soap_faultstring(soap)); 
			retval = soap->error; 
			break;
		}
		else 
		{
			break;
		}
    }while(0);

    free(soap_endpoint);
	soap_endpoint = NULL;
	destroy_soap(soap);

    return 0;
}

#if 0
int NVC_PTZ_Get_Presets(NVC_Dev_t * dev, NVC_PTZ_Preset_t * preset)
{
    struct soap *soap = NULL;
	int retval = 0;
	struct _tptz__GetNodes tptz__GetNodes;
    struct _tptz__GetNodesResponse tptz__GetNodesResponse;

	if (dev->profile_tn == NULL || strlen(dev->capa->PTZ_capa->url) < 2)
	{
		printf("[%s][%s] => Error: Invalid parameter!dev->profile_tn: %p, dev->profile_tn->profile_token: %p\n"
			, __FILE__, __func__, dev->profile_tn, dev->profile_tn->PTZ_parm_token);	
		return -1;
	}
	
	//const char *was_To = "";
	//const char *was_Action = "";
	
	/*soap = (struct soap *)malloc(sizeof(struct soap));
	memset(soap, 0, sizeof(sizeof(struct soap)));*/
	struct SOAP_ENV__Header header;
	//soap = creat_soap(&header, was_To, was_Action, dev->timeout, 0);
    soap = creat_soap(&header, NULL, NULL, dev->timeout, 0);
    
	if (dev->capa == NULL || dev->capa->PTZ_capa->url == NULL)
	{
		printf("[%s][%d] =>Error: Invaild parameter! dev->capa: %p, soap_endpoint: %p\n", __func__, __LINE__, dev->capa, dev->capa->PTZ_capa->url);
		destroy_soap(soap);
		return -1;
	}
	if (strlen(dev->capa->PTZ_capa->url) == 0)
	{
		printf("[%s][%d] =>Error: do not know soap_endpoint\n", __func__, __LINE__);
		destroy_soap(soap);
		return -1;
	}
	char *soap_endpoint = (char *)calloc(1, sizeof(char)*MAX_URL_LEN);//"soap.udp://239.255.255.250:3702/";
	strncpy(soap_endpoint, dev->capa->PTZ_capa->url, sizeof(char)*MAX_URL_LEN);

    const char *soap_action = "http://www.onvif.org/ver20/ptz/wsdl/GetNodes";

    do
    {        
        soap_call___tptz__GetNodes(soap, soap_endpoint, soap_action, &tptz__GetNodes, &tptz__GetNodesResponse);
        if (soap->error)
        {
            printf("[%s][%s][Line:%d] => soap error: %d, %s, %s\n",__FILE__, __func__, __LINE__, soap->error, *soap_faultcode(soap), *soap_faultstring(soap)); 
			retval = soap->error; 
			break;
		}
		else 
		{
		    int i = 0;
            preset->_size = tptz__GetNodesResponse.__sizePTZNode;
            for (i = 0; i < preset->_size; i++)
            {
                if (tptz__GetNodesResponse.PTZNode)
                {
                    if (tptz__GetNodesResponse.PTZNode[i].Name)
                    {
                        memset(preset->_preset[i].name, 0, sizeof(preset->_preset[i].name));
                        strncpy(preset->_preset[i].name, tptz__GetNodesResponse.PTZNode[i].Name, sizeof(preset->_preset[i].name));
                    }
                    if (tptz__GetNodesResponse.PTZNode[i].token)
                    {
                        memset(preset->_preset[i].token, 0, sizeof(preset->_preset[i].token));
                        strncpy(preset->_preset[i].token, tptz__GetNodesResponse.PTZNode[i].token, sizeof(preset->_preset[i].token));
                    }
                    preset->_preset[i].speed.pantilt.x = 1;
                    preset->_preset[i].speed.pantilt.y = 1;
                    preset->_preset[i].speed.zoom.x = 1;
                }
            }
			break;
		}
    }while(0);

    if (soap_endpoint)
    {
        free(soap_endpoint);
    	soap_endpoint = NULL;
	}
    destroy_soap(soap);
    
    return retval;
}
#else
int NVC_PTZ_Get_Presets(NVC_Dev_t * dev, NVC_PTZ_Preset_t * preset)
{
    struct soap *soap = NULL;
    int retval = 0;
    struct _tptz__GetPresets tptz_GetPresetreq;
    struct _tptz__GetPresetsResponse tptz_GetPresetresp;
    memset(&tptz_GetPresetreq,  0, sizeof(struct _tptz__GetPresets));
    memset(&tptz_GetPresetresp, 0, sizeof(struct _tptz__GetPresetsResponse));

    if (dev == NULL 
        || dev->profile_tn == NULL 
        || strlen(dev->profile_tn[dev->st_type].PTZ_parm_token) < 1
        || dev->capa == NULL 
        || dev->capa->PTZ_capa == NULL 
        || strlen(dev->capa->PTZ_capa->url) < 1)        
    {
        printf("[%s][%s] => Error: Invalid parameter!dev->profile_tn: %p, dev->profile_tn->profile_token:[%s]\n"
        	, __FILE__, __func__, dev->profile_tn, dev->profile_tn[dev->st_type].PTZ_parm_token);	
        return -1;
    }

  	struct SOAP_ENV__Header header;
    soap = creat_soap(&header, NULL, NULL, dev->timeout, 0, dev);
    if (soap == NULL)
    {
        printf("[%s][%d] => creat_soap failed!\n", __func__, __LINE__);
        return -1;
    }
    
    
  	char *soap_endpoint = (char *)calloc(1, sizeof(char)*MAX_URL_LEN);//"soap.udp://239.255.255.250:3702/";
  	strncpy(soap_endpoint, dev->capa->PTZ_capa->url, sizeof(char)*MAX_URL_LEN);
    const char *soap_action = "http://www.onvif.org/ver20/ptz/wsdl/GetPresets";
    
    printf("[%s][%d] => profiletoken: %s\n", __func__, __LINE__, dev->profile_tn[dev->st_type].profile_token);
    tptz_GetPresetreq.ProfileToken = (char *)soap_malloc(soap, sizeof(char)*INFO_LENGTH);
    memset(tptz_GetPresetreq.ProfileToken, 0, sizeof(char)*INFO_LENGTH);
    strncpy(tptz_GetPresetreq.ProfileToken, dev->profile_tn[dev->st_type].profile_token, sizeof(char)*INFO_LENGTH);

    do
    {        
        soap_call___tptz__GetPresets(soap, soap_endpoint, soap_action, &tptz_GetPresetreq, &tptz_GetPresetresp);
        if (soap->error)
        {
            fprintf(stderr, "[%s][%s][Line:%d] => soap error: %d, %s, %s\n",__FILE__, __func__, __LINE__, soap->error, *soap_faultcode(soap), *soap_faultstring(soap)); 
            retval = soap->error; 
            break;
        }
        else 
        {
            int i = 0;
            if(tptz_GetPresetresp.Preset != NULL)
            {
                preset->_size = (tptz_GetPresetresp.__sizePreset > MAX_PRESET_NUM)?MAX_PRESET_NUM:tptz_GetPresetresp.__sizePreset;
                  
                fprintf(stderr, "[%s][%s][Line:%d] => preset->_size: %d\n",__FILE__, __func__, __LINE__,preset->_size); 
                for (i = 0; i < tptz_GetPresetresp.__sizePreset; i++)
                {
                    memset(preset->_preset[i].name, 0, sizeof(preset->_preset[i].name));
                    if(tptz_GetPresetresp.Preset[i].Name)
                    {
                        strncpy(preset->_preset[i].name, tptz_GetPresetresp.Preset[i].Name, sizeof(preset->_preset[i].name));
                    }
                    memset(preset->_preset[i].token, 0, sizeof(preset->_preset[i].token));
                    if(tptz_GetPresetresp.Preset[i].token)
                    {
                        strncpy(preset->_preset[i].token, tptz_GetPresetresp.Preset[i].token, sizeof(preset->_preset[i].token));
                    }
                    if (tptz_GetPresetresp.Preset[i].PTZPosition)
                    {
                        if (tptz_GetPresetresp.Preset[i].PTZPosition->PanTilt)
                        {
                            preset->_preset[i].speed.pantilt.x = tptz_GetPresetresp.Preset[i].PTZPosition->PanTilt->x;
                            preset->_preset[i].speed.pantilt.y = tptz_GetPresetresp.Preset[i].PTZPosition->PanTilt->y;
                        }
                        else
                        {
                            preset->_preset[i].speed.pantilt.x = 1;
                            preset->_preset[i].speed.pantilt.y = 1;
                        }
                        if (tptz_GetPresetresp.Preset[i].PTZPosition->Zoom)
                        {
                            preset->_preset[i].speed.zoom.x = tptz_GetPresetresp.Preset[i].PTZPosition->Zoom->x;
                        }
                        else
                        {
                            preset->_preset[i].speed.zoom.x = 1;
                        }
                    }
                }
            }
            break;
        }
    }while(0);

    if (soap_endpoint)
    {
        free(soap_endpoint);
        soap_endpoint = NULL;
    }
    destroy_soap(soap);

    return retval;
}
#endif

int NVC_PTZ_Preset_set(NVC_Dev_t * dev, PTZ_Preset_t * preset)
{
    struct soap *soap = NULL;
	int retval = 0;
	struct _tptz__SetPreset tptz_SetPresetreq;
    struct _tptz__SetPresetResponse tptz_SetPresetresp;

	memset(&tptz_SetPresetreq,  0, sizeof(struct _tptz__SetPreset));
    memset(&tptz_SetPresetresp, 0, sizeof(struct _tptz__SetPresetResponse));
    
	if (dev == NULL 
        || dev->profile_tn == NULL 
        || strlen(dev->profile_tn[dev->st_type].PTZ_parm_token) < 1
        || dev->capa == NULL 
        || dev->capa->PTZ_capa == NULL 
        || strlen(dev->capa->PTZ_capa->url) < 1)
	{
		printf("[%s][%s] => Error: Invalid parameter!dev->profile_tn: %p, dev->profile_tn->profile_token:[%s]\n"
			, __FILE__, __func__, dev->profile_tn, dev->profile_tn[dev->st_type].PTZ_parm_token);	
		return -1;
	}
	
	//const char *was_To = "";
	//const char *was_Action = "";
	
	/*soap = (struct soap *)malloc(sizeof(struct soap));
	memset(soap, 0, sizeof(sizeof(struct soap)));*/
	struct SOAP_ENV__Header header;
	//soap = creat_soap(&header, was_To, was_Action, dev->timeout, 0);
    soap = creat_soap(&header, NULL, NULL, dev->timeout, 0, dev);
    if (soap == NULL)
    {
        printf("[%s][%d] => creat_soap failed!\n", __func__, __LINE__);
        return -1;
    }

	char *soap_endpoint = (char *)calloc(1, sizeof(char)*MAX_URL_LEN);//"soap.udp://239.255.255.250:3702/";
	strncpy(soap_endpoint, dev->capa->PTZ_capa->url, sizeof(char)*MAX_URL_LEN);

    const char *soap_action = "http://www.onvif.org/ver20/ptz/wsdl/SetPreset";
    
    printf("[%s][%d] => profiletoken: %s, preset => name: %s, token: %s\n"
        , __func__, __LINE__
        , dev->profile_tn[dev->st_type].profile_token
        , preset->name
        , preset->token);
    
    tptz_SetPresetreq.ProfileToken = (char *)soap_malloc(soap, sizeof(char)*INFO_LENGTH);
    memset(tptz_SetPresetreq.ProfileToken, 0, sizeof(char)*INFO_LENGTH);
    strncpy(tptz_SetPresetreq.ProfileToken, dev->profile_tn[dev->st_type].profile_token, sizeof(char)*INFO_LENGTH);

    tptz_SetPresetreq.PresetName = (char *)soap_malloc(soap, sizeof(char)*INFO_LENGTH);
    memset(tptz_SetPresetreq.PresetName, 0, sizeof(char)*INFO_LENGTH);
    strncpy(tptz_SetPresetreq.PresetName, preset->name, sizeof(char)*INFO_LENGTH);

    tptz_SetPresetreq.PresetToken = (char *)soap_malloc(soap, sizeof(char)*INFO_LENGTH);
    memset(tptz_SetPresetreq.PresetToken, 0, sizeof(char)*INFO_LENGTH);
    strncpy(tptz_SetPresetreq.PresetToken, preset->token, sizeof(char)*INFO_LENGTH);

    printf("[%s][%d] => tptz_SetPresetreq => profiletoken: %s, name: %s\n"    
        , __func__, __LINE__
        , tptz_SetPresetreq.ProfileToken
        , tptz_SetPresetreq.PresetName);

    do
    {
        soap_call___tptz__SetPreset(soap, soap_endpoint, soap_action, &tptz_SetPresetreq, &tptz_SetPresetresp);
        if (soap->error)
        {
            printf("[%s][%s][Line:%d] => soap error: %d, %s, %s\n",__FILE__, __func__, __LINE__, soap->error, *soap_faultcode(soap), *soap_faultstring(soap)); 
			retval = soap->error; 
			break;
		}
		else 
		{
		    #if 0
		    strncpy(tptz_SetPresetreq.PresetToken, tptz_SetPresetresp.PresetToken, sizeof(char)*INFO_LENGTH);
		    memset(preset->token, 0, sizeof(preset->token));
            strncpy(preset->token, tptz_SetPresetreq.PresetToken, sizeof(preset->token));
            #endif
			break;
		}
    }while(0);

    free(soap_endpoint);
	soap_endpoint = NULL;
	destroy_soap(soap);
    
    return 0;
}

int NVC_PTZ_Preset_call(NVC_Dev_t * dev, PTZ_Preset_t * preset)
{
     struct soap *soap = NULL;
	int retval = 0;
	struct _tptz__GotoPreset tptz_GotoPresetreq;
    struct _tptz__GotoPresetResponse tptz_GotoPresetresp;
    memset(&tptz_GotoPresetresp, 0, sizeof(struct _tptz__GotoPresetResponse));

	if (dev == NULL 
        || dev->profile_tn == NULL 
        || strlen(dev->profile_tn[dev->st_type].PTZ_parm_token) < 1
        || dev->capa == NULL 
        || dev->capa->PTZ_capa == NULL 
        || strlen(dev->capa->PTZ_capa->url) < 1)
	{
		printf("[%s][%s] => Error: Invalid parameter!dev->profile_tn: %p, dev->profile_tn->profile_token:[%p]\n"
			, __FILE__, __func__, dev->profile_tn, dev->profile_tn[dev->st_type].PTZ_parm_token);	
		return -1;
	}
	
	//const char *was_To = "";
	//const char *was_Action = "";
	
	/*soap = (struct soap *)malloc(sizeof(struct soap));
	memset(soap, 0, sizeof(sizeof(struct soap)));*/
	struct SOAP_ENV__Header header;
	//soap = creat_soap(&header, was_To, was_Action, dev->timeout, 0);
    soap = creat_soap(&header, NULL, NULL, dev->timeout, 0, dev);
    if (soap == NULL)
    {
        printf("[%s][%d] => creat_soap failed!\n", __func__, __LINE__);
        return -1;
    }

	char *soap_endpoint = (char *)calloc(1, sizeof(char)*MAX_URL_LEN);//"soap.udp://239.255.255.250:3702/";
	strncpy(soap_endpoint, dev->capa->PTZ_capa->url, sizeof(char)*MAX_URL_LEN);

    const char *soap_action = "http://www.onvif.org/ver20/ptz/wsdl/GotoPreset";
    
    printf("[%s][%d] => profiletoken: %s\n", __func__, __LINE__, dev->profile_tn[dev->st_type].profile_token);

    tptz_GotoPresetreq.ProfileToken = (char *)soap_malloc(soap, sizeof(char)*INFO_LENGTH);
    memset(tptz_GotoPresetreq.ProfileToken, 0, sizeof(char)*INFO_LENGTH);
    strncpy(tptz_GotoPresetreq.ProfileToken, dev->profile_tn[dev->st_type].profile_token, sizeof(char)*INFO_LENGTH);

    tptz_GotoPresetreq.PresetToken = (char *)soap_malloc(soap, sizeof(char)*INFO_LENGTH);
    memset(tptz_GotoPresetreq.PresetToken, 0, sizeof(char)*INFO_LENGTH);
    strncpy(tptz_GotoPresetreq.PresetToken, preset->token, sizeof(char)*INFO_LENGTH);

    tptz_GotoPresetreq.Speed = (struct tt__PTZSpeed *)soap_malloc(soap, sizeof(struct tt__PTZSpeed));
    memset(tptz_GotoPresetreq.Speed, 0, sizeof(struct tt__PTZSpeed));

    tptz_GotoPresetreq.Speed->PanTilt = (struct tt__Vector2D *)soap_malloc(soap, sizeof(struct tt__Vector2D));
    memset(tptz_GotoPresetreq.Speed->PanTilt, 0, sizeof(struct tt__Vector2D));
    tptz_GotoPresetreq.Speed->PanTilt->space = (char *)soap_malloc(soap, sizeof(char)*INFO_LENGTH);
    memset(tptz_GotoPresetreq.Speed->PanTilt->space, 0, sizeof(char)*INFO_LENGTH);
    tptz_GotoPresetreq.Speed->PanTilt->x = preset->speed.pantilt.x;
    tptz_GotoPresetreq.Speed->PanTilt->y = preset->speed.pantilt.y;
    strcpy(tptz_GotoPresetreq.Speed->PanTilt->space, "http://www.onvif.org/ver10/schema");
    
    tptz_GotoPresetreq.Speed->Zoom = (struct tt__Vector1D *)soap_malloc(soap, sizeof(struct tt__Vector1D));
    memset(tptz_GotoPresetreq.Speed->Zoom, 0, sizeof(struct tt__Vector1D));
    tptz_GotoPresetreq.Speed->Zoom->space = (char *)soap_malloc(soap, sizeof(char)*INFO_LENGTH);
    memset(tptz_GotoPresetreq.Speed->Zoom->space, 0, sizeof(char)*INFO_LENGTH);
    tptz_GotoPresetreq.Speed->Zoom->x = preset->speed.zoom.x;
    strcpy(tptz_GotoPresetreq.Speed->Zoom->space, "http://www.onvif.org/ver10/schema");

    do
    {
        soap_call___tptz__GotoPreset(soap, soap_endpoint, soap_action, &tptz_GotoPresetreq, &tptz_GotoPresetresp);
        if (soap->error)
        {
            printf("[%s][%s][Line:%d] => soap error: %d, %s, %s\n",__FILE__, __func__, __LINE__, soap->error, *soap_faultcode(soap), *soap_faultstring(soap)); 
			retval = soap->error; 
			break;
		}
		else 
		{
			break;
		}
    }while(0);

    free(soap_endpoint);
	soap_endpoint = NULL;
	destroy_soap(soap);
    
    return retval;
}

int NVC_PTZ_Preset_del(NVC_Dev_t * dev, PTZ_Preset_t * preset)
{
    struct soap *soap = NULL;
	int retval = 0;
	struct _tptz__RemovePreset tptz_RemovePresetreq;
    struct _tptz__RemovePresetResponse tptz_RemovePresetresp;
    memset(&tptz_RemovePresetresp, 0, sizeof(struct _tptz__RemovePresetResponse));
    
	if (dev == NULL 
        || dev->profile_tn == NULL 
        || strlen(dev->profile_tn[dev->st_type].PTZ_parm_token) < 1
        || dev->capa == NULL 
        || dev->capa->PTZ_capa == NULL 
        || strlen(dev->capa->PTZ_capa->url) < 1)
	{
		printf("[%s][%s] => Error: Invalid parameter!dev->profile_tn: %p, dev->profile_tn->profile_token:[%s]\n"
			, __FILE__, __func__, dev->profile_tn, dev->profile_tn[dev->st_type].PTZ_parm_token);	
		return -1;
	}
	
	//const char *was_To = "";
	//const char *was_Action = "";
	
	/*soap = (struct soap *)malloc(sizeof(struct soap));
	memset(soap, 0, sizeof(sizeof(struct soap)));*/
	struct SOAP_ENV__Header header;
	//soap = creat_soap(&header, was_To, was_Action, dev->timeout, 0);
    soap = creat_soap(&header, NULL, NULL, dev->timeout, 0, dev);
    if (soap == NULL)
    {
        printf("[%s][%d] => creat_soap failed!\n", __func__, __LINE__);
        return -1;
    }
    
	char *soap_endpoint = (char *)calloc(1, sizeof(char)*MAX_URL_LEN);//"soap.udp://239.255.255.250:3702/";
	strncpy(soap_endpoint, dev->capa->PTZ_capa->url, sizeof(char)*MAX_URL_LEN);

    const char *soap_action = "http://www.onvif.org/ver20/ptz/wsdl/RemovePreset";
    
    printf("[%s][%d] => profiletoken: %s, preset => name: %s, token: %s\n"
        , __func__, __LINE__
        , dev->profile_tn[dev->st_type].profile_token
        , preset->name
        , preset->token);
    
    tptz_RemovePresetreq.ProfileToken = (char *)soap_malloc(soap, sizeof(char)*INFO_LENGTH);
    memset(tptz_RemovePresetreq.ProfileToken, 0, sizeof(char)*INFO_LENGTH);
    strncpy(tptz_RemovePresetreq.ProfileToken, dev->profile_tn[dev->st_type].profile_token, sizeof(char)*INFO_LENGTH);

    tptz_RemovePresetreq.PresetToken = (char *)soap_malloc(soap, sizeof(char)*INFO_LENGTH);
    memset(tptz_RemovePresetreq.PresetToken, 0, sizeof(char)*INFO_LENGTH);
    strncpy(tptz_RemovePresetreq.PresetToken, preset->token, sizeof(char)*INFO_LENGTH);

    printf("[%s][%d] => tptz_RemovePresetreq => profiletoken: %s, token: %s\n"    
        , __func__, __LINE__
        , tptz_RemovePresetreq.ProfileToken
        , tptz_RemovePresetreq.PresetToken);

    do
    {
        soap_call___tptz__RemovePreset(soap, soap_endpoint, soap_action, &tptz_RemovePresetreq, &tptz_RemovePresetresp);
        if (soap->error)
        {
            printf("[%s][%s][Line:%d] => soap error: %d, %s, %s\n",__FILE__, __func__, __LINE__, soap->error, *soap_faultcode(soap), *soap_faultstring(soap)); 
    		retval = soap->error; 
    		break;
    	}
    	else 
    	{
    		break;
    	}
    }while(0);

    free(soap_endpoint);
	soap_endpoint = NULL;
	destroy_soap(soap);
    
    return retval;
}
#if 1
int NVC_PTZ_Preset_tour(NVC_Dev_t * dev, NVC_PTZ_Preset_t *preset)
{
    struct soap *soap = NULL;
	int retval = 0;
	struct SOAP_ENV__Header header;
    const char *soap_action = NULL;
    char *soap_endpoint;
    int needCreate = 0;
    int i;
    char name[MAX_NAME_LEN];

	if (dev == NULL 
        || dev->profile_tn == NULL 
        || strlen(dev->profile_tn[dev->st_type].PTZ_parm_token) < 1
        || dev->capa == NULL 
        || dev->capa->PTZ_capa == NULL 
        || strlen(dev->capa->PTZ_capa->url) < 1)
	{
		printf("[%s][%s] => Error: Invalid parameter!dev->profile_tn: %p, dev->profile_tn->profile_token:[%s]\n"
			, __FILE__, __func__, dev->profile_tn, dev->profile_tn[dev->st_type].PTZ_parm_token);	
		return -1;
	}
    
    soap = creat_soap(&header, NULL, NULL, dev->timeout, 0, dev);
    if (soap == NULL)
    {
        printf("[%s][%d] => creat_soap failed!\n", __func__, __LINE__);
        return -1;
    }

    struct _tptz__GetPresetTours GetPresetTours;
    struct _tptz__GetPresetToursResponse GetPresetToursResp;

    memset(&GetPresetTours, 0, sizeof(struct _tptz__GetPresetTours));
    memset(&GetPresetToursResp, 0, sizeof(struct _tptz__GetPresetToursResponse));
    GetPresetTours.ProfileToken = (char *)soap_malloc(soap, sizeof(char)*INFO_LENGTH);
    memset(GetPresetTours.ProfileToken, 0, sizeof(char)*INFO_LENGTH);
    strncpy(GetPresetTours.ProfileToken, dev->profile_tn[dev->st_type].profile_token, sizeof(char)*INFO_LENGTH);

    soap_endpoint = (char *)calloc(1, sizeof(char)*MAX_URL_LEN);//"soap.udp://239.255.255.250:3702/";
	strncpy(soap_endpoint, dev->capa->PTZ_capa->url, sizeof(char)*MAX_URL_LEN);

    do
     {
        soap_call___tptz__GetPresetTours(soap, soap_endpoint, soap_action, &GetPresetTours, &GetPresetToursResp);
        if (soap->error)
        {
            printf("[%s][%s][Line:%d] => soap error: %d, %s, %s\n",__FILE__, __func__, __LINE__, soap->error, *soap_faultcode(soap), *soap_faultstring(soap)); 
            retval = soap->error; 
            break;
        }
        else 
        {
            printf("[%s][%s][Line:%d] => size:%d \n",__FILE__, __func__, __LINE__, GetPresetToursResp.__sizePresetTour); 
            printf("[%s][%s][Line:%d] => ok\n",__FILE__, __func__, __LINE__); 
            if(GetPresetToursResp.__sizePresetTour > 0)
            {
                memset(preset->ptz_tour_token, 0, sizeof(preset->ptz_tour_token));
                strcpy(preset->ptz_tour_token, GetPresetToursResp.PresetTour[0].token);
            }
            else
            {
                needCreate = 1;
            }

            printf("GetPresetToursResp.PresetTour :%p\n", GetPresetToursResp.PresetTour);
#if 0            
            if(GetPresetToursResp.PresetTour[0].Name)
            {
                memset(name, 0, sizeof(name));  
                strncpy(name, GetPresetToursResp.PresetTour[0].Name, sizeof(name));
            }
#endif            
        }
     }while(0);


#if 0
    if(needCreate)
    {
    	destroy_soap(soap);
        soap = creat_soap(&header, NULL, NULL, dev->timeout, 0, dev);
        if (soap == NULL)
        {
            printf("[%s][%d] => creat_soap failed!\n", __func__, __LINE__);
            free(soap_endpoint);
            soap_endpoint = NULL;         
            return -1;
        }

        struct _tptz__CreatePresetTour CreatePresetTour;
        struct _tptz__CreatePresetTourResponse CreatePresetTourResp;

        memset(&CreatePresetTour, 0, sizeof(struct _tptz__CreatePresetTour));
        memset(&CreatePresetTourResp, 0, sizeof(struct _tptz__CreatePresetTourResponse));
        CreatePresetTour.ProfileToken = (char *)soap_malloc(soap, sizeof(char)*INFO_LENGTH);
        memset(CreatePresetTour.ProfileToken, 0, sizeof(char)*INFO_LENGTH);
        strncpy(CreatePresetTour.ProfileToken, dev->profile_tn[dev->st_type].profile_token, sizeof(char)*INFO_LENGTH);

        do
         {
            soap_call___tptz__CreatePresetTour(soap, soap_endpoint, soap_action, &CreatePresetTour, &CreatePresetTourResp);
            if (soap->error)
            {
                printf("[%s][%s][Line:%d] => soap error: %d, %s, %s\n",__FILE__, __func__, __LINE__, soap->error, *soap_faultcode(soap), *soap_faultstring(soap)); 
                retval = soap->error; 
                break;
            }
            else 
            {
                printf("[%s][%s][Line:%d] => token%s\n",__FILE__, __func__, __LINE__, CreatePresetTourResp.PresetTourToken); 
                if(CreatePresetTourResp.PresetTourToken)
                {
                    memset(preset->ptz_tour_token, 0, sizeof(preset->ptz_tour_token));
                    strcpy(preset->ptz_tour_token, CreatePresetTourResp.PresetTourToken);
                }
            }
         }while(0);
    }
 
    if(strlen(preset->ptz_tour_token) > 1)
    {
        destroy_soap(soap);
        soap = creat_soap(&header, NULL, NULL, dev->timeout, 0, dev);
        if (soap == NULL)
        {
            printf("[%s][%d] => creat_soap failed!\n", __func__, __LINE__);
            free(soap_endpoint);
            soap_endpoint = NULL;         
            return -1;
        }
        struct _tptz__ModifyPresetTour ModifyPresetTour;
        struct _tptz__ModifyPresetTourResponse ModifyPresetTourResp;
        memset(&ModifyPresetTourResp, 0, sizeof(struct _tptz__ModifyPresetTourResponse));
        ModifyPresetTour.ProfileToken = (char *)soap_malloc(soap, sizeof(char)*INFO_LENGTH);
        memset(ModifyPresetTour.ProfileToken, 0, sizeof(char)*INFO_LENGTH);
        strncpy(ModifyPresetTour.ProfileToken, dev->profile_tn[dev->st_type].profile_token, sizeof(char)*INFO_LENGTH);
        ModifyPresetTour.PresetTour = (struct tt__PresetTour*)soap_malloc(soap, sizeof(struct tt__PresetTour));
        ModifyPresetTour.PresetTour->Name = (char *)soap_malloc(soap, sizeof(char)*INFO_LENGTH);
        memset(ModifyPresetTour.PresetTour->Name, 0, sizeof(char)*INFO_LENGTH);
        if(strlen(name) > 1)
        {
            strncpy(ModifyPresetTour.PresetTour->Name, name, sizeof(ModifyPresetTour.PresetTour->Name));
        }
        else
        {
            sprintf(ModifyPresetTour.PresetTour->Name, "%s", preset->ptz_tour_token);
        }
        ModifyPresetTour.PresetTour->AutoStart = _false;
        ModifyPresetTour.PresetTour->Extension = NULL;
        ModifyPresetTour.PresetTour->StartingCondition = NULL;
        ModifyPresetTour.PresetTour->Status = NULL;
        ModifyPresetTour.PresetTour->__anyAttribute = NULL;
        ModifyPresetTour.PresetTour->token = (char *)soap_malloc(soap, sizeof(char)*INFO_LENGTH);
        memset(ModifyPresetTour.PresetTour->token, 0, sizeof(char)*INFO_LENGTH);
        strncpy(ModifyPresetTour.PresetTour->token, preset->ptz_tour_token, sizeof(char)*INFO_LENGTH);
        ModifyPresetTour.PresetTour->__sizeTourSpot = preset->_size;
        for(i = 0; i < preset->_size; i++)
        {
            ModifyPresetTour.PresetTour->TourSpot = (struct tt__PTZPresetTourSpot*)soap_malloc(soap, sizeof(struct tt__PTZPresetTourSpot)*preset->_size);
            ModifyPresetTour.PresetTour->TourSpot[i].Extension = NULL;
            ModifyPresetTour.PresetTour->TourSpot[i].PresetDetail = NULL;
            ModifyPresetTour.PresetTour->TourSpot[i].__anyAttribute = NULL;
            ModifyPresetTour.PresetTour->TourSpot[i].Speed = (struct tt__PTZSpeed*)soap_malloc(soap, sizeof(struct tt__PTZSpeed));
            ModifyPresetTour.PresetTour->TourSpot[i].Speed->PanTilt = (struct tt__Vector2D*)soap_malloc(soap, sizeof(struct tt__Vector2D));
            ModifyPresetTour.PresetTour->TourSpot[i].Speed->PanTilt->x = preset->_preset[i].speed.pantilt.x;
            ModifyPresetTour.PresetTour->TourSpot[i].Speed->PanTilt->y = preset->_preset[i].speed.pantilt.y;
            ModifyPresetTour.PresetTour->TourSpot[i].Speed->PanTilt->space = NULL;
            ModifyPresetTour.PresetTour->TourSpot[i].Speed->Zoom = (struct tt__Vector1D*)soap_malloc(soap, sizeof(struct tt__Vector1D));
            ModifyPresetTour.PresetTour->TourSpot[i].Speed->Zoom->x = preset->_preset[i].speed.zoom.x;
            ModifyPresetTour.PresetTour->TourSpot[i].Speed->Zoom->space = NULL;
            ModifyPresetTour.PresetTour->TourSpot[i].StayTime = (LONG64 *)soap_malloc(soap, sizeof(LONG64));
        	memset(ModifyPresetTour.PresetTour->TourSpot[i].StayTime, 0, sizeof(LONG64));
        	*(ModifyPresetTour.PresetTour->TourSpot[i].StayTime) = preset->_preset[i].staytime;
        }

        do
         {
            soap_call___tptz__ModifyPresetTour(soap, soap_endpoint, soap_action, &ModifyPresetTour, &ModifyPresetTourResp);
            if (soap->error)
            {
                printf("[%s][%s][Line:%d] => soap error: %d, %s, %s\n",__FILE__, __func__, __LINE__, soap->error, *soap_faultcode(soap), *soap_faultstring(soap)); 
                retval = soap->error; 
                break;
            }
      }while(0);
    }
#endif

    free(soap_endpoint);
    soap_endpoint = NULL;
    destroy_soap(soap);
    return retval;
}

int NVC_PTZ_PresetTour_start(NVC_Dev_t * dev, NVC_PTZ_Preset_t *preset)
{
    struct soap *soap = NULL;
	int retval = 0;
	struct SOAP_ENV__Header header;
    const char *soap_action = NULL;
    char *soap_endpoint;
    
	if (dev == NULL 
        || dev->profile_tn == NULL 
        || strlen(dev->profile_tn[dev->st_type].PTZ_parm_token) < 1
        || dev->capa == NULL 
        || dev->capa->PTZ_capa == NULL 
        || strlen(dev->capa->PTZ_capa->url) < 1)
	{
		printf("[%s][%s] => Error: Invalid parameter!dev->profile_tn: %p, dev->profile_tn->profile_token:[%s]\n"
			, __FILE__, __func__, dev->profile_tn, dev->profile_tn[dev->st_type].PTZ_parm_token);	
		return -1;
	}
    
    if(strlen(preset->ptz_tour_token) < 1)
    {
        printf("[%s][%d] => PTZ preset tour token is NULL!\n", __func__, __LINE__);
        return -1;
    }

    soap = creat_soap(&header, NULL, NULL, dev->timeout, 0, dev);
    if (soap == NULL)
    {
        printf("[%s][%d] => creat_soap failed!\n", __func__, __LINE__);
        return -1;
    }
    struct _tptz__OperatePresetTour OperatePresetTour;
    struct _tptz__OperatePresetTourResponse OperatePresetTourResp;
    memset(&OperatePresetTourResp, 0, sizeof(struct _tptz__OperatePresetTourResponse));

    OperatePresetTour.Operation = tt__PTZPresetTourOperation__Start;
    OperatePresetTour.PresetTourToken = (char *)soap_malloc(soap, sizeof(char)*INFO_LENGTH);
    memset(OperatePresetTour.PresetTourToken, 0, sizeof(char)*INFO_LENGTH);
    strncpy(OperatePresetTour.PresetTourToken, preset->ptz_tour_token, sizeof(char)*INFO_LENGTH);
    OperatePresetTour.ProfileToken = (char *)soap_malloc(soap, sizeof(char)*INFO_LENGTH);
    memset(OperatePresetTour.ProfileToken, 0, sizeof(char)*INFO_LENGTH);
    strncpy(OperatePresetTour.ProfileToken, dev->profile_tn[dev->st_type].profile_token, sizeof(char)*INFO_LENGTH);

    soap_endpoint = (char *)calloc(1, sizeof(char)*MAX_URL_LEN);//"soap.udp://239.255.255.250:3702/";
	strncpy(soap_endpoint, dev->capa->PTZ_capa->url, sizeof(char)*MAX_URL_LEN);
    do
     {
		
         soap_call___tptz__OperatePresetTour(soap, soap_endpoint, soap_action, &OperatePresetTour, &OperatePresetTourResp);
         
         if (soap->error)
         {
             printf("[%s][%s][Line:%d][profiletoken :%s, tour_token :%s] => soap error: %d, %s, %s, %s\n",
             	__FILE__, __func__, __LINE__, OperatePresetTour.ProfileToken, 
             	OperatePresetTour.PresetTourToken, soap->error, *soap_faultcode(soap), 
             	*soap_faultstring(soap), *soap_faultdetail(soap)); 
             retval = soap->error; 
             break;
         }
     }while(0);

    free(soap_endpoint);
    soap_endpoint = NULL;
    destroy_soap(soap);
    return retval;
}

int NVC_PTZ_PresetTour_stop(NVC_Dev_t * dev, NVC_PTZ_Preset_t *preset)
{
    struct soap *soap = NULL;
	int retval = 0;
	struct SOAP_ENV__Header header;
    const char *soap_action = NULL;
    char *soap_endpoint;
    

	if (dev == NULL 
        || dev->profile_tn == NULL 
        || strlen(dev->profile_tn[dev->st_type].PTZ_parm_token) < 1
        || dev->capa == NULL 
        || dev->capa->PTZ_capa == NULL 
        || strlen(dev->capa->PTZ_capa->url) < 1)
	{
		printf("[%s][%s] => Error: Invalid parameter!dev->profile_tn: %p, dev->profile_tn->profile_token:[%s]\n"
			, __FILE__, __func__, dev->profile_tn, dev->profile_tn[dev->st_type].PTZ_parm_token);	
		return -1;
	}
    
    if(strlen(preset->ptz_tour_token) < 1)
    {
        printf("[%s][%d] => PTZ preset tour token is NULL!\n", __func__, __LINE__);
        return -1;
    }

    soap = creat_soap(&header, NULL, NULL, dev->timeout, 0, dev);
    if (soap == NULL)
    {
        printf("[%s][%d] => creat_soap failed!\n", __func__, __LINE__);
        return -1;
    }
    struct _tptz__OperatePresetTour OperatePresetTour;
    struct _tptz__OperatePresetTourResponse OperatePresetTourResp;
    memset(&OperatePresetTourResp, 0, sizeof(struct _tptz__OperatePresetTourResponse));

    OperatePresetTour.Operation = tt__PTZPresetTourOperation__Stop;
    OperatePresetTour.PresetTourToken = (char *)soap_malloc(soap, sizeof(char)*INFO_LENGTH);
    memset(OperatePresetTour.PresetTourToken, 0, sizeof(char)*INFO_LENGTH);
    strncpy(OperatePresetTour.PresetTourToken, preset->ptz_tour_token, sizeof(char)*INFO_LENGTH);
    OperatePresetTour.ProfileToken = (char *)soap_malloc(soap, sizeof(char)*INFO_LENGTH);
    memset(OperatePresetTour.ProfileToken, 0, sizeof(char)*INFO_LENGTH);
    strncpy(OperatePresetTour.ProfileToken, dev->profile_tn[dev->st_type].profile_token, sizeof(char)*INFO_LENGTH);

    soap_endpoint = (char *)calloc(1, sizeof(char)*MAX_URL_LEN);//"soap.udp://239.255.255.250:3702/";
	strncpy(soap_endpoint, dev->capa->PTZ_capa->url, sizeof(char)*MAX_URL_LEN);
    do
     {
         soap_call___tptz__OperatePresetTour(soap, soap_endpoint, soap_action, &OperatePresetTour, &OperatePresetTourResp);
         if (soap->error)
         {
             printf("[%s][%s][Line:%d] => soap error: %d, %s, %s\n",__FILE__, __func__, __LINE__, soap->error, *soap_faultcode(soap), *soap_faultstring(soap)); 
             retval = soap->error; 
             break;
         }
     }while(0);

    free(soap_endpoint);
    soap_endpoint = NULL;
    destroy_soap(soap);
    return retval;
}

int NVC_PTZ_PresetTour_remove(NVC_Dev_t * dev, NVC_PTZ_Preset_t *preset)
{
    struct soap *soap = NULL;
	int retval = 0;
	struct SOAP_ENV__Header header;
    const char *soap_action = NULL;
    char *soap_endpoint;
    

	if (dev == NULL 
        || dev->profile_tn == NULL 
        || strlen(dev->profile_tn[dev->st_type].PTZ_parm_token) < 1
        || dev->capa == NULL 
        || dev->capa->PTZ_capa == NULL 
        || strlen(dev->capa->PTZ_capa->url) < 1)
	{
		printf("[%s][%s] => Error: Invalid parameter!dev->profile_tn: %p, dev->profile_tn->profile_token:[%s]\n"
			, __FILE__, __func__, dev->profile_tn, dev->profile_tn[dev->st_type].PTZ_parm_token);	
		return -1;
	}
    
    if(strlen(preset->ptz_tour_token) < 1)
    {
        printf("[%s][%d] => PTZ preset tour token is NULL!\n", __func__, __LINE__);
        return -1;
    }

    soap = creat_soap(&header, NULL, NULL, dev->timeout, 0, dev);
    if (soap == NULL)
    {
        printf("[%s][%d] => creat_soap failed!\n", __func__, __LINE__);
        return -1;
    }

    struct _tptz__GetPresetTour GetPresetTour;
    struct _tptz__GetPresetTourResponse GetPresetTourResp;
    memset(&GetPresetTourResp, 0, sizeof(struct _tptz__GetPresetTourResponse));
    GetPresetTour.PresetTourToken = (char *)soap_malloc(soap, sizeof(char)*INFO_LENGTH);
    memset(GetPresetTour.PresetTourToken, 0, sizeof(char)*INFO_LENGTH);
    strncpy(GetPresetTour.PresetTourToken, preset->ptz_tour_token, sizeof(char)*INFO_LENGTH);
    GetPresetTour.ProfileToken = (char *)soap_malloc(soap, sizeof(char)*INFO_LENGTH);
    memset(GetPresetTour.ProfileToken, 0, sizeof(char)*INFO_LENGTH);
    strncpy(GetPresetTour.ProfileToken, dev->profile_tn[dev->st_type].profile_token, sizeof(char)*INFO_LENGTH);

    soap_endpoint = (char *)calloc(1, sizeof(char)*MAX_URL_LEN);//"soap.udp://239.255.255.250:3702/";
	strncpy(soap_endpoint, dev->capa->PTZ_capa->url, sizeof(char)*MAX_URL_LEN);
    do
     {
         soap_call___tptz__GetPresetTour(soap, soap_endpoint, soap_action, &GetPresetTour, &GetPresetTourResp);
         if (soap->error)
         {
             printf("[%s][%s][Line:%d] => soap error: %d, %s, %s\n",__FILE__, __func__, __LINE__, soap->error, *soap_faultcode(soap), *soap_faultstring(soap)); 
             retval = soap->error; 
             break;
         }
     }while(0);
    retval = -1;
    if(retval >= 0)
    {
        destroy_soap(soap);
        soap = creat_soap(&header, NULL, NULL, dev->timeout, 0, dev);
        if (soap == NULL)
        {
            printf("[%s][%d] => creat_soap failed!\n", __func__, __LINE__);
            return -1;
        }
        struct _tptz__RemovePresetTour RemovePresetTour;
        struct _tptz__RemovePresetTourResponse RemovePresetTourResp;
        memset(&RemovePresetTourResp, 0, sizeof(struct _tptz__RemovePresetTourResponse));
        RemovePresetTour.PresetTourToken = (char *)soap_malloc(soap, sizeof(char)*INFO_LENGTH);
        memset(RemovePresetTour.PresetTourToken, 0, sizeof(char)*INFO_LENGTH);
        strncpy(RemovePresetTour.PresetTourToken, preset->ptz_tour_token, sizeof(char)*INFO_LENGTH);
        RemovePresetTour.ProfileToken = (char *)soap_malloc(soap, sizeof(char)*INFO_LENGTH);
        memset(RemovePresetTour.ProfileToken, 0, sizeof(char)*INFO_LENGTH);
        strncpy(RemovePresetTour.ProfileToken, dev->profile_tn[dev->st_type].profile_token, sizeof(char)*INFO_LENGTH);
        do
         {
            soap_call___tptz__RemovePresetTour(soap, soap_endpoint, soap_action, &RemovePresetTour, &RemovePresetTourResp);
             if (soap->error)
             {
                 printf("[%s][%s][Line:%d] => soap error: %d, %s, %s\n",__FILE__, __func__, __LINE__, soap->error, *soap_faultcode(soap), *soap_faultstring(soap)); 
                 retval = soap->error; 
                 break;
             }
         }while(0);
    }
    free(soap_endpoint);
    soap_endpoint = NULL;
    destroy_soap(soap);
    return retval;
}
#endif

int get_submask_from_masklen(char *submask, int len) /**masklen为子网掩码的长度*/
{
    if (len < 0 || len > 32)
    {
        printf("[%s][%d] => Invalid parameter!!! len: %d, is not between 0~32\n", __func__, __LINE__, len);
        return -1;
    }
    
    int tmplen = 32 - len;
    unsigned int mask = (0xFFFFFFFF << tmplen);
    unsigned int mask1 = (mask >> 24) & 0xFF ;
    unsigned int mask2 = (mask >> 16) & 0xFF;
    unsigned int mask3 = (mask >> 8) & 0xFF;
    unsigned int mask4 = mask & 0xFF;
    
    sprintf(submask, "%d.%d.%d.%d", mask1, mask2, mask3, mask4);
 
    return 0;
}

int get_masklen_from_submask(char *submask)
{
    if (submask == NULL ||
        strlen(submask) == 0)
    {
        printf("[%s][%d] => Invalid parameter!!! sunmask: %s, is not between 0~32\n", __func__, __LINE__, submask);
        return -1;
    }
    unsigned int mask[4] = {0};
    sscanf(submask, "%d.%d.%d.%d", &mask[0], &mask[1], &mask[2], &mask[3]);
    unsigned int _mask = (mask[0] << 24) | (mask[1] << 16) | (mask[2] << 8) | mask[3];/**必须为unsigned int类型,防止后面_mask>>=1出问题*/
    int len = 0;
    while (_mask != 0)
    {
        if ((_mask & 1) != 0)
        {
            len++;
        }
        _mask >>= 1;
    }
    return len;
}


/***************************************** Img attr *******************************************/
int NVC_Get_Img_options(NVC_Dev_t *dev, NVC_Img_attr_t *img_attr)
{
    if (dev->profile_tn == NULL 
        || strlen(dev->profile_tn[dev->st_type].v_src_sourcetoken) < 1 
        || dev->capa == NULL 
        || dev->capa->img_capa == NULL
        || strlen(dev->capa->img_capa->url) < 1)
	{
		printf("[%s][%s] => Error: Invalid parameter!dev->profile_tn: %p, dev->profile_tn->profile_token: %s\n"
			, __FILE__, __func__, dev->profile_tn, dev->profile_tn[dev->st_type].profile_token);	
		return -1;
	}
    
    int retval = -1;
    struct _timg__GetOptions img_GetOptionsReq;
    struct _timg__GetOptionsResponse img_GetOptionsResp;
    memset(&img_GetOptionsResp, 0, sizeof(struct _timg__GetOptionsResponse));
    
    struct soap *soap = NULL;
    struct SOAP_ENV__Header header;
    soap = creat_soap(&header, NULL, NULL, 5, 0, dev);
    if (soap == NULL)
    {
        printf("[%s][%d] => creat_soap failed!\n", __func__, __LINE__);
        return -1;
    }

    const char *soap_action = "http://www.onvif.org/ver20/imaging/wsdl/GetOptions";
    char *soap_endpoint = (char *)calloc(1, sizeof(char)*MAX_URL_LEN);
	strncpy(soap_endpoint, dev->capa->img_capa->url, sizeof(char)*MAX_URL_LEN);

    img_GetOptionsReq.VideoSourceToken = (char *)soap_malloc(soap, sizeof(char)*INFO_LENGTH);
    memset(img_GetOptionsReq.VideoSourceToken, 0, sizeof(char)*INFO_LENGTH);
    strncpy(img_GetOptionsReq.VideoSourceToken, dev->profile_tn[dev->st_type].v_src_sourcetoken, sizeof(char)*INFO_LENGTH);
    //printf("[%s][%d] => VideoSourceToken: %s\n", __func__, __LINE__, GetImgReq.VideoSourceToken);
    //printf("[%s][%d] => soap_endpoint: %s\n", __func__, __LINE__, soap_endpoint);

    soap_call___timg__GetOptions(soap, soap_endpoint, soap_action, &img_GetOptionsReq, &img_GetOptionsResp);
    if (soap->error)
    {
        printf("[%s][%s][Line:%d] => soap error: %d, %s, %s\n",__FILE__, __func__, __LINE__, soap->error, *soap_faultcode(soap), *soap_faultstring(soap)); 
    	  retval = soap->error;
    }
    else
    {
        if (img_GetOptionsResp.ImagingOptions)
        {
            if (img_GetOptionsResp.ImagingOptions->Brightness)
            {
                img_attr->max_brihtness = img_GetOptionsResp.ImagingOptions->Brightness->Max;
                img_attr->min_brihtness = img_GetOptionsResp.ImagingOptions->Brightness->Min;
            }
            if (img_GetOptionsResp.ImagingOptions->Contrast)
            {
                img_attr->max_contrast = img_GetOptionsResp.ImagingOptions->Contrast->Max;
                img_attr->min_contrast = img_GetOptionsResp.ImagingOptions->Contrast->Min;
            }
            if (img_GetOptionsResp.ImagingOptions->ColorSaturation)
            {
                img_attr->max_saturation = img_GetOptionsResp.ImagingOptions->ColorSaturation->Max;
                img_attr->min_saturation = img_GetOptionsResp.ImagingOptions->ColorSaturation->Min;
            }
            if (img_GetOptionsResp.ImagingOptions->Sharpness)
            {
                img_attr->max_sharpness = img_GetOptionsResp.ImagingOptions->Sharpness->Max;
                img_attr->min_sharpness = img_GetOptionsResp.ImagingOptions->Sharpness->Min;
            }
             img_attr->max_hue = 255;/**onvif 抓包未发现*/
             img_attr->min_hue = 0;/**onvif 抓包未发现*/
        }
    }
    #if 1
    printf("[%s][%d] => brightness: %d--%d, contrast: %d--%d, saturation: %d--%d, sharpness: %d--%d\n"
                ,__func__, __LINE__
                , img_attr->min_brihtness
                , img_attr->max_brihtness
                , img_attr->min_contrast
                , img_attr->max_contrast
                , img_attr->min_saturation
                , img_attr->max_saturation
                , img_attr->min_sharpness
                , img_attr->max_sharpness);
    #endif
    if (soap_endpoint)
    {
        free(soap_endpoint);
        soap_endpoint = NULL;
    }
    destroy_soap(soap);
    return 0;
}


int NVC_Get_Img_attr_info(NVC_Dev_t *dev, NVC_Img_attr_t *img_attr)
{    
    if (dev->profile_tn == NULL 
        || strlen(dev->profile_tn[dev->st_type].v_src_sourcetoken) < 1 
        || dev->capa == NULL 
        || dev->capa->img_capa == NULL
        || strlen(dev->capa->img_capa->url) < 1)
	{
		printf("[%s][%s] => Error: Invalid parameter!dev->profile_tn: %p, dev->profile_tn->profile_token: %s\n"
			, __FILE__, __func__, dev->profile_tn, dev->profile_tn[dev->st_type].profile_token);	
		return -1;
	}
    
    int retval = -1;
    struct _timg__GetImagingSettings GetImgReq;
    struct _timg__GetImagingSettingsResponse GetImgResp;
    memset(&GetImgResp, 0, sizeof(struct _timg__GetImagingSettingsResponse));
    
    struct soap *soap = NULL;
    struct SOAP_ENV__Header header;
    soap = creat_soap(&header, NULL, NULL, 5, 0, dev);
    if (soap == NULL)
    {
        printf("[%s][%d] => creat_soap failed!\n", __func__, __LINE__);
        return -1;
    }

    const char *soap_action =  "http://www.onvif.org/ver20/imaging/wsdl/GetImagingSettings";
    char *soap_endpoint = (char *)calloc(1, sizeof(char)*MAX_URL_LEN);
	strncpy(soap_endpoint, dev->capa->img_capa->url, sizeof(char)*MAX_URL_LEN);

    GetImgReq.VideoSourceToken = (char *)soap_malloc(soap, sizeof(char)*INFO_LENGTH);
    memset(GetImgReq.VideoSourceToken, 0, sizeof(char)*INFO_LENGTH);
    strncpy(GetImgReq.VideoSourceToken, dev->profile_tn[dev->st_type].v_src_sourcetoken, sizeof(char)*INFO_LENGTH);
    //printf("[%s][%d] => VideoSourceToken: %s\n", __func__, __LINE__, GetImgReq.VideoSourceToken);
    //printf("[%s][%d] => soap_endpoint: %s\n", __func__, __LINE__, soap_endpoint);

    soap_call___timg__GetImagingSettings(soap, soap_endpoint, soap_action, &GetImgReq, &GetImgResp);
    if (soap->error)
    {
        printf("[%s][%s][Line:%d] => soap error: %d, %s, %s\n",__FILE__, __func__, __LINE__, soap->error, *soap_faultcode(soap), *soap_faultstring(soap)); 
    	  retval = soap->error;
    }
    else
    {
        if (GetImgResp.ImagingSettings)
        {
            if (GetImgResp.ImagingSettings->Brightness)
            {
                if(img_attr->max_brihtness == 0)
                {
                    img_attr->max_brihtness = 1;
                }
                img_attr->brightness = (255 * (*(GetImgResp.ImagingSettings->Brightness) / (1.0 * img_attr->max_brihtness)));
            }
            if (GetImgResp.ImagingSettings->Contrast)
            {
                if(img_attr->max_contrast == 0)
                {
                    img_attr->max_contrast = 1;
                }
                img_attr->contrast = (255 * (*(GetImgResp.ImagingSettings->Contrast) / (1.0 * img_attr->max_contrast)));
            }
            if (GetImgResp.ImagingSettings->ColorSaturation)
            {
                if(img_attr->max_saturation == 0)
                {
                    img_attr->max_saturation = 1;
                }
                img_attr->saturation = (255 * (*(GetImgResp.ImagingSettings->ColorSaturation) / (1.0 *img_attr->max_saturation)));
            }
            if (GetImgResp.ImagingSettings->Sharpness)
            {
                if(img_attr->max_sharpness == 0)
                {
                    img_attr->max_sharpness = 0;
                }
                img_attr->sharpness = (255 * (*(GetImgResp.ImagingSettings->Sharpness) / (1.0 * img_attr->max_sharpness)));
            }
             img_attr->hue = 0;/**onvif 抓包未发现*/
        }
    }

    if (soap_endpoint)
    {
        free(soap_endpoint);
        soap_endpoint = NULL;
    }
    destroy_soap(soap);
    return 0;
}

int NVC_Set_Img_attr_info(NVC_Dev_t *dev, NVC_Img_attr_t *img_attr)
{
   if (dev->profile_tn == NULL 
      || strlen(dev->profile_tn[dev->st_type].v_src_sourcetoken) < 1 
      || dev->capa == NULL 
      || dev->capa->img_capa == NULL
      || strlen(dev->capa->img_capa->url) < 1)
  	{
  		printf("[%s][%s] => Error: Invalid parameter!dev->profile_tn: %p, dev->profile_tn->profile_token: %s\n"
  			, __FILE__, __func__, dev->profile_tn, dev->profile_tn[dev->st_type].profile_token);	
  		return -1;
  	}
       
    int retval = -1;
    struct _timg__SetImagingSettings SetImgReq;
    struct _timg__SetImagingSettingsResponse SetImgResp;
    memset(&SetImgReq, 0, sizeof(struct _timg__SetImagingSettings));
    
    struct soap *soap = NULL;
    struct SOAP_ENV__Header header;
    soap = creat_soap(&header, NULL, NULL, 5, 0, dev);
    if (soap == NULL)
    {
        printf("[%s][%d] => creat_soap failed!\n", __func__, __LINE__);
        return -1;
    }

    char *soap_endpoint = (char *)calloc(1, sizeof(char)*MAX_URL_LEN);
	strncpy(soap_endpoint, dev->capa->img_capa->url, sizeof(char)*MAX_URL_LEN);
    
    /************** before set, get img attr,防止某些字段为空致使设置失败 ***************/
    struct _timg__GetImagingSettings GetImgReq;
    struct _timg__GetImagingSettingsResponse GetImgResp;
    
    memset(&GetImgResp, 0, sizeof(struct _timg__GetImagingSettingsResponse));
    GetImgReq.VideoSourceToken = (char *)soap_malloc(soap, sizeof(char)*INFO_LENGTH);
    memset(GetImgReq.VideoSourceToken, 0, sizeof(char)*INFO_LENGTH);
    strncpy(GetImgReq.VideoSourceToken, dev->profile_tn[dev->st_type].v_src_sourcetoken, sizeof(char)*INFO_LENGTH);
    //printf("[%s][%d] => VideoSourceToken: %s\n", __func__, __LINE__, GetImgReq.VideoSourceToken);
    //printf("[%s][%d] => soap_endpoint: %s\n", __func__, __LINE__, soap_endpoint);
    const char *_soap_action =  "http://www.onvif.org/ver20/imaging/wsdl/GetImagingSettings";

    soap_call___timg__GetImagingSettings(soap, soap_endpoint, _soap_action, &GetImgReq, &GetImgResp);
    if (soap->error)
    {
        printf("[%s][%s][Line:%d] => soap error: %d, %s, %s\n",__FILE__, __func__, __LINE__, soap->error, *soap_faultcode(soap), *soap_faultstring(soap)); 
    	retval = soap->error;
        printf("[%s][%d] => before set, get Img attr, get failed!!!, return %d\n", __func__, __LINE__, retval);
        if (soap_endpoint)
        {
            free(soap_endpoint);
            soap_endpoint = NULL;
        }
        destroy_soap(soap);
        return retval;
    }

    /***************************** set img attr *****************************************/
    const char *soap_action =  "http://www.onvif.org/ver20/imaging/wsdl/SetImagingSettings";
    SetImgReq.VideoSourceToken = (char *)soap_malloc(soap, sizeof(char)*INFO_LENGTH);
    memset(SetImgReq.VideoSourceToken, 0, sizeof(char)*INFO_LENGTH);
    strncpy(SetImgReq.VideoSourceToken, dev->profile_tn[dev->st_type].v_src_sourcetoken, sizeof(char)*INFO_LENGTH);
    //printf("[%s][%d] => VideoSourceToken: %s\n", __func__, __LINE__, SetImgReq.VideoSourceToken);
    
    SetImgReq.ForcePersistence_x0020 = (enum xsd__boolean_ *)soap_malloc(soap, sizeof(enum xsd__boolean_));
    *(SetImgReq.ForcePersistence_x0020) = _true;
    
    SetImgReq.ImagingSettings = (struct tt__ImagingSettings20 *)soap_malloc(soap, sizeof(struct tt__ImagingSettings20));
    memset(SetImgReq.ImagingSettings, 0, sizeof(SetImgReq.ImagingSettings));

    SetImgReq.ImagingSettings->Brightness = (float *)soap_malloc(soap, sizeof(float));
    memset(SetImgReq.ImagingSettings->Brightness, 0, sizeof(float));
    //*(SetImgReq.ImagingSettings->Brightness) = (img_attr->max_brihtness - (((img_attr->max_brihtness - img_attr->min_brihtness) * (255 - img_attr->brightness)) / 255));
    *(SetImgReq.ImagingSettings->Brightness) = ((img_attr->max_brihtness - img_attr->min_brihtness) * (img_attr->brightness / 255.0)) + 0.5;

    SetImgReq.ImagingSettings->Contrast = (float *)soap_malloc(soap, sizeof(float));
    memset(SetImgReq.ImagingSettings->Contrast, 0, sizeof(float));
    *(SetImgReq.ImagingSettings->Contrast) = ((img_attr->max_contrast - img_attr->min_contrast) * ( img_attr->contrast / 255.0)) + 0.5;

    SetImgReq.ImagingSettings->ColorSaturation = (float *)soap_malloc(soap, sizeof(float));
    memset(SetImgReq.ImagingSettings->ColorSaturation, 0, sizeof(float));
    *(SetImgReq.ImagingSettings->ColorSaturation) = ((img_attr->max_saturation - img_attr->min_saturation) * (img_attr->saturation / 255.0)) + 0.5;

    SetImgReq.ImagingSettings->Sharpness = (float *)soap_malloc(soap, sizeof(float));
    memset(SetImgReq.ImagingSettings->Sharpness, 0, sizeof(float));
    *(SetImgReq.ImagingSettings->Sharpness) = ((img_attr->max_sharpness - img_attr->min_sharpness) * (img_attr->sharpness / 255.0)) + 0.5;
        
    #if 1
     fprintf(stderr, "[%s][%d] => brightness: %f, contrast: %f, saturation: %f, sharpness: %f\n"
                ,__func__, __LINE__
                , *(SetImgReq.ImagingSettings->Brightness)
                , *(SetImgReq.ImagingSettings->Contrast)
                , *(SetImgReq.ImagingSettings->ColorSaturation)
                , *(SetImgReq.ImagingSettings->Sharpness));
    #endif


    if (GetImgResp.ImagingSettings)
    {
#if 0
        if (GetImgResp.ImagingSettings->BacklightCompensation)
        {
            SetImgReq.ImagingSettings->BacklightCompensation = (struct tt__BacklightCompensation20 *)soap_malloc(soap, sizeof(struct tt__BacklightCompensation20));
            memset(SetImgReq.ImagingSettings->BacklightCompensation, 0, sizeof(struct tt__BacklightCompensation20));
            SetImgReq.ImagingSettings->BacklightCompensation->Mode = GetImgResp.ImagingSettings->BacklightCompensation->Mode;
            if (GetImgResp.ImagingSettings->BacklightCompensation->Level)
            {
                SetImgReq.ImagingSettings->BacklightCompensation->Level = (float *)soap_malloc(soap, sizeof(float));
                memset(SetImgReq.ImagingSettings->BacklightCompensation->Level, 0, sizeof(float));
                *(SetImgReq.ImagingSettings->BacklightCompensation->Level) = *(GetImgResp.ImagingSettings->BacklightCompensation->Level);
            }
        }
        else
            SetImgReq.ImagingSettings->BacklightCompensation = NULL;
        
        SetImgReq.ImagingSettings->Focus = NULL;
        SetImgReq.ImagingSettings->Extension = NULL;
        SetImgReq.ImagingSettings->__anyAttribute = NULL;

        if (GetImgResp.ImagingSettings->Exposure)
        {
            SetImgReq.ImagingSettings->Exposure = GetImgResp.ImagingSettings->Exposure;
        }
        else
        {
            SetImgReq.ImagingSettings->Exposure = NULL;
        }
        
        if (GetImgResp.ImagingSettings->Focus)
        {
            SetImgReq.ImagingSettings->Focus = GetImgResp.ImagingSettings->Focus;
        }
        else
        {
            SetImgReq.ImagingSettings->Focus = NULL;
        }
     
        SetImgReq.ImagingSettings->IrCutFilter = (enum tt__IrCutFilterMode *)soap_malloc(soap, sizeof(enum tt__IrCutFilterMode));
        memset(SetImgReq.ImagingSettings->IrCutFilter, 0, sizeof(enum tt__IrCutFilterMode));   
        if (GetImgResp.ImagingSettings->IrCutFilter)
        {
            *(SetImgReq.ImagingSettings->IrCutFilter) = *(GetImgResp.ImagingSettings->IrCutFilter);
        }
        else
            *(SetImgReq.ImagingSettings->IrCutFilter) = tt__IrCutFilterMode__ON;

        if (GetImgResp.ImagingSettings->WideDynamicRange)
        {

            SetImgReq.ImagingSettings->WideDynamicRange = (struct tt__WideDynamicRange20 *)soap_malloc(soap, sizeof(struct tt__WideDynamicRange20));
            memset(SetImgReq.ImagingSettings->WideDynamicRange, 0, sizeof(struct tt__WideDynamicRange20));
            SetImgReq.ImagingSettings->WideDynamicRange->Mode = GetImgResp.ImagingSettings->WideDynamicRange->Mode;
            if (GetImgResp.ImagingSettings->WideDynamicRange->Level)
            {
                SetImgReq.ImagingSettings->WideDynamicRange->Level = (float *)soap_malloc(soap, sizeof(float));
                memset(SetImgReq.ImagingSettings->WideDynamicRange->Level, 0, sizeof(float));
                *(SetImgReq.ImagingSettings->WideDynamicRange->Level) = *(GetImgResp.ImagingSettings->WideDynamicRange->Level);
            }
        }
        else
           SetImgReq.ImagingSettings->WideDynamicRange = NULL;


        if (GetImgResp.ImagingSettings->WhiteBalance)
        {
            SetImgReq.ImagingSettings->WhiteBalance = (struct tt__WhiteBalance20 *)soap_malloc(soap, sizeof(struct tt__WhiteBalance20));
            memset(SetImgReq.ImagingSettings->WhiteBalance, 0, sizeof(struct tt__WhiteBalance20));
            SetImgReq.ImagingSettings->WhiteBalance->Mode = GetImgResp.ImagingSettings->WhiteBalance->Mode;
            if (GetImgResp.ImagingSettings->WhiteBalance->CrGain)
            {
                SetImgReq.ImagingSettings->WhiteBalance->CrGain = (float *)soap_malloc(soap, sizeof(float));
                memset(SetImgReq.ImagingSettings->WhiteBalance->CrGain, 0, sizeof(float));
                *(SetImgReq.ImagingSettings->WhiteBalance->CrGain) = *(GetImgResp.ImagingSettings->WhiteBalance->CrGain);
            }
            if (GetImgResp.ImagingSettings->WhiteBalance->CbGain)
            {
                SetImgReq.ImagingSettings->WhiteBalance->CbGain = (float *)soap_malloc(soap, sizeof(float));
                memset(SetImgReq.ImagingSettings->WhiteBalance->CbGain, 0, sizeof(float));
                *(SetImgReq.ImagingSettings->WhiteBalance->CbGain) = *(GetImgResp.ImagingSettings->WhiteBalance->CbGain);
            }            
            SetImgReq.ImagingSettings->WhiteBalance->Extension = NULL;
            SetImgReq.ImagingSettings->WhiteBalance->__anyAttribute = NULL;
        }
        else
            SetImgReq.ImagingSettings->WhiteBalance = NULL;
#else
        SetImgReq.ImagingSettings->BacklightCompensation = GetImgResp.ImagingSettings->BacklightCompensation;
        SetImgReq.ImagingSettings->Exposure         = GetImgResp.ImagingSettings->Exposure;
        SetImgReq.ImagingSettings->Focus            = GetImgResp.ImagingSettings->Focus;
        SetImgReq.ImagingSettings->IrCutFilter      = GetImgResp.ImagingSettings->IrCutFilter;
        SetImgReq.ImagingSettings->WideDynamicRange = GetImgResp.ImagingSettings->WideDynamicRange;
        SetImgReq.ImagingSettings->WhiteBalance     = GetImgResp.ImagingSettings->WhiteBalance;
        SetImgReq.ImagingSettings->Extension        = GetImgResp.ImagingSettings->Extension;
        SetImgReq.ImagingSettings->__anyAttribute   = GetImgResp.ImagingSettings->__anyAttribute;

#endif
    }

    soap_call___timg__SetImagingSettings(soap, soap_endpoint, soap_action, &SetImgReq, &SetImgResp);
    if (soap->error)
    {
        printf("[%s][%s][Line:%d] => soap error: %d, %s, %s\n",__FILE__, __func__, __LINE__, soap->error, *soap_faultcode(soap), *soap_faultstring(soap)); 
    	retval = soap->error;
    }

    if (soap_endpoint)
    {
        free(soap_endpoint);
        soap_endpoint = NULL;
    }
    destroy_soap(soap);
    if (retval == 0)
    {
        printf("[%s][%d] => out OK!\n", __func__, __LINE__);
    }
    return 0;
}

int NVC_Get_SnapshotUri(NVC_Dev_t *dev, NVC_Snapshot_t *snap)
{
  int ret = 0;
  struct soap *soap = NULL;
  struct _trt__GetSnapshotUri SnapshotUriReq;
  struct _trt__GetSnapshotUriResponse SnapshotUriResp;
  memset(&SnapshotUriResp, 0, sizeof(struct _trt__GetSnapshotUriResponse));

	if (dev->profile_tn == NULL 
        || strlen(dev->profile_tn[dev->st_type].v_enc_token) < 1 
        || dev->capa == NULL
        || dev->capa->media_capa == NULL
        || strlen(dev->capa->media_capa->url) < 1)
	{
		printf("[%s][%s] => Error: Invalid parameter!dev->profile_tn: %p, dev->profile_tn->profile_token: %p\n"
			, __FILE__, __func__, dev->profile_tn, dev->profile_tn[dev->st_type].v_enc_token);	
		return -1;
	}

	struct SOAP_ENV__Header header;
	soap = creat_soap(&header, NULL, NULL, dev->timeout, 0, dev);
  if (soap == NULL)
  {
      printf("[%s][%d] => creat_soap failed!\n", __func__, __LINE__);
      return -1;
  }

	SnapshotUriReq.ProfileToken = (char *)soap_malloc(soap, sizeof(char)*INFO_LENGTH);
	memset(SnapshotUriReq.ProfileToken, 0, sizeof(char)*INFO_LENGTH);
  strncpy(SnapshotUriReq.ProfileToken, dev->profile_tn[dev->st_type].profile_token, sizeof(char)*INFO_LENGTH);
	
  fprintf(stderr, "[%s] token:%s\n", __FUNCTION__, SnapshotUriReq.ProfileToken);
	const char *soap_action = "http://www.onvif.org/ver10/media/wsdl/GetSnapshotUri";
 
  char *soap_endpoint = (char *)calloc(1, sizeof(char)*MAX_URL_LEN);//"soap.udp://239.255.255.250:3702/";
  strncpy(soap_endpoint, dev->capa->media_capa->url, sizeof(char)*MAX_URL_LEN);

  do
  {
    soap_call___trt__GetSnapshotUri(soap, soap_endpoint, soap_action, &SnapshotUriReq, &SnapshotUriResp);
    if (soap->error)
    {
    	printf("[%s][%s][Line:%d] => soap error: %d, %s, %s\n",__FILE__, __func__, __LINE__, soap->error, *soap_faultcode(soap), *soap_faultstring(soap)); 
    	ret = soap->error; 
    	break;
    }
    else 
    {
      printf("[%s][%d]soap_call___trt__GetSnapshotUri ok\n", __func__, __LINE__);
      if (SnapshotUriResp.MediaUri)
      {
          if(SnapshotUriResp.MediaUri->Uri)
          {
              printf("[%s][%d] st_type=%d, url:[%s]\n", __func__, __LINE__, dev->st_type, SnapshotUriResp.MediaUri->Uri);
              strncpy(snap->url, SnapshotUriResp.MediaUri->Uri, sizeof(snap->url)-1);
          }
          else
          {
              printf("[%s][%d] => GetSnapshotUri: Uri = NULL\n", __func__, __LINE__); 
          }
      }
  	  break;
    }
  }while(0);

	free(soap_endpoint);
	soap_endpoint = NULL;
	destroy_soap(soap);
  return 0;
}


int NVC_Get_SystemTime(NVC_Dev_t *dev, NVC_time_t *time)
{
  int retval = -1;
  struct soap *soap = NULL;
  struct SOAP_ENV__Header header;
  const char *soap_action = NULL;
  
  dev->get_tm = 1;
  soap = creat_soap(&header, NULL, NULL, 5, 0, dev);
  if (soap == NULL)
  {
    printf("[%s][%d] => creat_soap failed!\n", __func__, __LINE__);
    return -1;
  }
  dev->get_tm = 0;
  
  if (strlen(dev->ip) < 1 || dev->port == 0)
	{
		printf("[%s][%s] => Error: Invalid parameter! ip:[%s], port:[%d]\n"
			, __FILE__, __func__, dev->ip, dev->port);	
		return -1;
	}
    
  char *soap_endpoint = (char *)calloc(1, sizeof(char)*MAX_URL_LEN);
	sprintf(soap_endpoint, "http://%s:%d/onvif/device_service", dev->ip, dev->port);

#if 1
  struct _tds__GetSystemDateAndTime GetSystemTime;
  struct _tds__GetSystemDateAndTimeResponse GetSystemTimeResp;
  memset(&GetSystemTimeResp, 0, sizeof(struct _tds__GetSystemDateAndTimeResponse));
  do
  {
    soap_call___tds__GetSystemDateAndTime_(soap, soap_endpoint, soap_action, &GetSystemTime, &GetSystemTimeResp);
    if (soap->error)
    {
      printf("[%s][%s][Line:%d] => soap error: %d, %s, %s\n",__FILE__, __func__, __LINE__, soap->error, *soap_faultcode(soap), *soap_faultstring(soap)); 
      retval = soap->error;
      free(soap_endpoint);
      soap_endpoint = NULL;
      destroy_soap(soap);
      return retval;
    }
    else
    {
      if(GetSystemTimeResp.SystemDateAndTime)
      {
       if(GetSystemTimeResp.SystemDateAndTime->TimeZone)
       { 
          if(GetSystemTimeResp.SystemDateAndTime->TimeZone->TZ != NULL)
          {
            printf("[%s] => TZ=[%s]\n", __func__, GetSystemTimeResp.SystemDateAndTime->TimeZone->TZ);
          }
       }
       
       if(GetSystemTimeResp.SystemDateAndTime->UTCDateTime)
       {
        
          if(GetSystemTimeResp.SystemDateAndTime->UTCDateTime->Date)
          {
            time->year = GetSystemTimeResp.SystemDateAndTime->UTCDateTime->Date->Year;
            time->mon  = GetSystemTimeResp.SystemDateAndTime->UTCDateTime->Date->Month;
            time->day  = GetSystemTimeResp.SystemDateAndTime->UTCDateTime->Date->Day;
          }
          if(GetSystemTimeResp.SystemDateAndTime->UTCDateTime->Time)
          {
            time->hour = GetSystemTimeResp.SystemDateAndTime->UTCDateTime->Time->Hour;
            time->min = GetSystemTimeResp.SystemDateAndTime->UTCDateTime->Time->Minute;
            time->sec = GetSystemTimeResp.SystemDateAndTime->UTCDateTime->Time->Second;
          }
       }
       
      }

    }
  }while(0);
#endif
  
  printf("[%s][%d]  => [%d-%d-%dT%d:%d:%dZ] TZ[%d]\n", __func__, __LINE__
        , time->year, time->mon, time->day
        , time->hour, time->min, time->sec
        , time->zone);
  
  if (soap_endpoint)
  {
    free(soap_endpoint);
    soap_endpoint = NULL;
  }
  destroy_soap(soap);
  return 0;
}



int NVC_Set_SystemTime(NVC_Dev_t *dev, NVC_time_t *time)
{
  int retval = -1;
  struct soap *soap = NULL;
  struct SOAP_ENV__Header header;
  const char *soap_action = NULL;

  soap = creat_soap(&header, NULL, NULL, 5, 0, dev);
  if (soap == NULL)
  {
    printf("[%s][%d] => creat_soap failed!\n", __func__, __LINE__);
    return -1;
  }

  if (strlen(dev->ip) < 1 || dev->port == 0)
	{
		printf("[%s][%s] => Error: Invalid parameter! ip:[%s], port:[%d]\n"
			, __FILE__, __func__, dev->ip, dev->port);	
		return -1;
	}
    
  char *soap_endpoint = (char *)calloc(1, sizeof(char)*MAX_URL_LEN);
	sprintf(soap_endpoint, "http://%s:%d/onvif/device_service", dev->ip, dev->port);

  struct _tds__SetSystemDateAndTime SetSystemDateAndTime;
  struct _tds__SetSystemDateAndTimeResponse SetSystemDateAndTimeResp;
  memset(&SetSystemDateAndTimeResp, 0, sizeof(struct _tds__SetSystemDateAndTimeResponse));
  SetSystemDateAndTime.TimeZone           = (struct tt__TimeZone*)soap_malloc(soap, sizeof(struct tt__TimeZone));
  SetSystemDateAndTime.TimeZone->TZ       = (char *)soap_malloc(soap, sizeof(char)*INFO_LENGTH);
  memset(SetSystemDateAndTime.TimeZone->TZ, 0, sizeof(char)*INFO_LENGTH);
  SetSystemDateAndTime.UTCDateTime        = (struct tt__DateTime*)soap_malloc(soap, sizeof(struct tt__DateTime));
  SetSystemDateAndTime.UTCDateTime->Date  = (struct tt__Date*)soap_malloc(soap, sizeof(struct tt__Date));
  SetSystemDateAndTime.UTCDateTime->Time  = (struct tt__Time*)soap_malloc(soap, sizeof(struct tt__Time));
  SetSystemDateAndTime.DateTimeType       = tt__SetDateTimeType__Manual;
  SetSystemDateAndTime.DaylightSavings    = _false;
  SetSystemDateAndTime.UTCDateTime->Date->Year    = time->year;
  SetSystemDateAndTime.UTCDateTime->Date->Month   = time->mon;
  SetSystemDateAndTime.UTCDateTime->Date->Day     = time->day;
  SetSystemDateAndTime.UTCDateTime->Time->Minute  = time->min;
  SetSystemDateAndTime.UTCDateTime->Time->Second  = time->sec;

  if((int)(time->hour - 8) < 0)
  {
    SetSystemDateAndTime.UTCDateTime->Time->Hour = 24 + time->hour - 8;//assume that beijing time   
    SetSystemDateAndTime.UTCDateTime->Date->Day  = SetSystemDateAndTime.UTCDateTime->Date->Day - 1;
    if(SetSystemDateAndTime.UTCDateTime->Date->Day == 0)
      SetSystemDateAndTime.UTCDateTime->Date->Day = 1;
  }
  else
  {
    SetSystemDateAndTime.UTCDateTime->Time->Hour = time->hour - 8;//assume that beijing time   
  }  
#if 1
  struct _tds__GetSystemDateAndTime GetSystemTime;
  struct _tds__GetSystemDateAndTimeResponse GetSystemTimeResp;
  memset(&GetSystemTimeResp, 0, sizeof(struct _tds__GetSystemDateAndTimeResponse));
  do
  {
    soap_call___tds__GetSystemDateAndTime_(soap, soap_endpoint, soap_action, &GetSystemTime, &GetSystemTimeResp);
    if (soap->error)
    {
      printf("[%s][%s][Line:%d] => soap error: %d, %s, %s\n",__FILE__, __func__, __LINE__, soap->error, *soap_faultcode(soap), *soap_faultstring(soap)); 
      retval = soap->error;
      free(soap_endpoint);
      soap_endpoint = NULL;
      destroy_soap(soap);
      return retval;
    }
    else
    {
      if(GetSystemTimeResp.SystemDateAndTime)
      {
       if(GetSystemTimeResp.SystemDateAndTime->TimeZone)
       { 
          if(GetSystemTimeResp.SystemDateAndTime->TimeZone->TZ != NULL)
          {
            printf("[%s] => IPC TZ=[%s]\n", GetSystemTimeResp.SystemDateAndTime->TimeZone->TZ);
            strcpy(SetSystemDateAndTime.TimeZone->TZ, GetSystemTimeResp.SystemDateAndTime->TimeZone->TZ);
          }
          else
            SetSystemDateAndTime.TimeZone->TZ = "GMT+08";
       }
       else
         SetSystemDateAndTime.TimeZone->TZ = "GMT+08";
      }
      else   
        SetSystemDateAndTime.TimeZone->TZ = "GMT+08";
    }
  }while(0);
#endif
  printf("-->> time: %d-%d-%d %d:%d:%d\n"
      ,SetSystemDateAndTime.UTCDateTime->Date->Year
      ,SetSystemDateAndTime.UTCDateTime->Date->Month
      ,SetSystemDateAndTime.UTCDateTime->Date->Day
      ,SetSystemDateAndTime.UTCDateTime->Time->Hour
      ,SetSystemDateAndTime.UTCDateTime->Time->Minute
      ,SetSystemDateAndTime.UTCDateTime->Time->Second);
  
  soap_call___tds__SetSystemDateAndTime_(soap, soap_endpoint, soap_action, &SetSystemDateAndTime, &SetSystemDateAndTimeResp);
  if (soap->error)
  {
    printf("[%s][%s][Line:%d] => soap error: %d, %s, %s\n",__FILE__, __func__, __LINE__, soap->error, *soap_faultcode(soap), *soap_faultstring(soap)); 
    retval = soap->error;
  }
  else
  {
    printf("[%s][%s][Line:%d] =>Set system time OK!\n", __FILE__, __func__, __LINE__); 
  }

  if (soap_endpoint)
  {
    free(soap_endpoint);
    soap_endpoint = NULL;
  }
  destroy_soap(soap);
  return 0;
}

#if 0
int NVC_PTZ_Cruise_Begin(NVC_Dev_t * dev, PTZ_Preset_t * preset)
{

  struct soap *soap = NULL;
	int retval = 0;
	struct _tptz__SetPreset tptz_SetPresetreq;
  struct _tptz__SetPresetResponse tptz_SetPresetresp;

	memset(&tptz_SetPresetreq,  0, sizeof(struct _tptz__SetPreset));
  memset(&tptz_SetPresetresp, 0, sizeof(struct _tptz__SetPresetResponse));


	if (dev == NULL 
        || dev->profile_tn == NULL 
        || strlen(dev->profile_tn[dev->st_type].PTZ_parm_token) < 1
        || dev->capa == NULL 
        || dev->capa->PTZ_capa == NULL 
        || strlen(dev->capa->PTZ_capa->url) < 1)
	{
		printf("[%s][%s] => Error: Invalid parameter!dev->profile_tn: %p, dev->profile_tn->profile_token:[%s]\n"
			, __FILE__, __func__, dev->profile_tn, dev->profile_tn[dev->st_type].PTZ_parm_token);	
		return -1;
	}
	
	struct SOAP_ENV__Header header;

    soap = creat_soap(&header, NULL, NULL, dev->timeout, 0, dev);
    if (soap == NULL)
    {
        printf("[%s][%d] => creat_soap failed!\n", __func__, __LINE__);
        return -1;
    }

	char *soap_endpoint = (char *)calloc(1, sizeof(char)*MAX_URL_LEN);//"soap.udp://239.255.255.250:3702/";
	strncpy(soap_endpoint, dev->capa->PTZ_capa->url, sizeof(char)*MAX_URL_LEN);

    const char *soap_action = "http://www.onvif.org/ver20/ptz/wsdl/SetPreset";
    
    printf("[%s][%d] => profiletoken: %s, preset => name: %s, token: %s\n"
        , __func__, __LINE__
        , dev->profile_tn[dev->st_type].profile_token
        , preset->name
        , preset->token);
    
    tptz_SetPresetreq.ProfileToken = (char *)soap_malloc(soap, sizeof(char)*INFO_LENGTH);
    memset(tptz_SetPresetreq.ProfileToken, 0, sizeof(char)*INFO_LENGTH);
    strncpy(tptz_SetPresetreq.ProfileToken, dev->profile_tn[dev->st_type].profile_token, sizeof(char)*INFO_LENGTH);

    tptz_SetPresetreq.PresetName = (char *)soap_malloc(soap, sizeof(char)*INFO_LENGTH);
    memset(tptz_SetPresetreq.PresetName, 0, sizeof(char)*INFO_LENGTH);
    strncpy(tptz_SetPresetreq.PresetName, preset->name, sizeof(char)*INFO_LENGTH);

    tptz_SetPresetreq.PresetToken = (char *)soap_malloc(soap, sizeof(char)*INFO_LENGTH);
    memset(tptz_SetPresetreq.PresetToken, 0, sizeof(char)*INFO_LENGTH);
    strncpy(tptz_SetPresetreq.PresetToken, preset->token, sizeof(char)*INFO_LENGTH);

    printf("[%s][%d] => tptz_SetPresetreq => profiletoken: %s, name: %s\n"    
        , __func__, __LINE__
        , tptz_SetPresetreq.ProfileToken
        , tptz_SetPresetreq.PresetName);

    do
    {
      soap_call___tptz__SetPreset(soap, soap_endpoint, soap_action, &tptz_SetPresetreq, &tptz_SetPresetresp);
      if (soap->error)
      {
          printf("[%s][%s][Line:%d] => soap error: %d, %s, %s\n",__FILE__, __func__, __LINE__, soap->error, *soap_faultcode(soap), *soap_faultstring(soap)); 
    			retval = soap->error; 
    			break;
  		}
  		else 
  		{
  		    #if 0
  		    strncpy(tptz_SetPresetreq.PresetToken, tptz_SetPresetresp.PresetToken, sizeof(char)*INFO_LENGTH);
  		    memset(preset->token, 0, sizeof(preset->token));
              strncpy(preset->token, tptz_SetPresetreq.PresetToken, sizeof(preset->token));
          #endif
  			  break;
  		}
    }while(0);

    free(soap_endpoint);
	  soap_endpoint = NULL;
	  destroy_soap(soap);
    return 0;
}
#endif


