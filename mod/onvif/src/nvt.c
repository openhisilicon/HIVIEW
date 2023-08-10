#include <time.h>
#include <string.h>
#include <assert.h>
#include <sys/select.h>
#include <unistd.h>
#include <signal.h>
#include "nvt.h"
#include "wsseapi.h"

#include <sys/syscall.h>

#define gettid() syscall(__NR_gettid) 

int nvt_wsse_auth(struct soap *soap)
{
	if(g_nvt->p.auth == 0)
	{
	  printf("%s => check flag auth:%d.\n", __func__, g_nvt->p.auth);
		return 0;
	}
	
	if(soap && soap->header && soap->header->wsse__Security)
	{
  	  if(soap->header->wsse__Security->UsernameToken == NULL 
      	|| soap->header->wsse__Security->UsernameToken->Nonce == NULL 
      	|| soap->header->wsse__Security->UsernameToken->wsu__Created == NULL 
      	|| soap->header->wsse__Security->UsernameToken->Password == NULL 
      	|| soap->header->wsse__Security->UsernameToken->Password->Type == NULL)
    	{
    		int ret = soap_sender_fault_subcode(soap
    		            , "wsse:FailedAuthentication"
    		            , "The request requires authorization."
    		            , NULL);
        printf("%s => check UsernameToken param err, ret:%d.\n", __func__, ret);
        return -1;
    	}

      // find pwd: soap->header->wsse__Security->UsernameToken->Username
      printf("%s => check find Username:[%s] ...\n", __func__, soap->header->wsse__Security->UsernameToken->Username);
      user_info_t userlist = {0,};
      user_right_t *ur = (user_right_t*)userlist.buf;
      
      get_user_info(&userlist);
      if(!userlist.size || strcmp(ur->user_name, soap->header->wsse__Security->UsernameToken->Username))
      {
        printf("%s => check find Username:[%s], err.\n", __func__, soap->header->wsse__Security->UsernameToken->Username);
        return -1;
      }
      printf("%s => check find ok Username:[%s], pwd[%s].\n", __func__, ur->user_name, ur->user_pwd);
      char *pwd =  ur->user_pwd;
  		{
  			if(soap_wsse_verify_Password(soap, pwd) == SOAP_OK)
  			{
  			  printf("%s => check username:[%s] soap_wsse_verify_Password ok.\n", __func__, soap->header->wsse__Security->UsernameToken->Username);
  				return 0;
  			}
  			else
  			{
    			printf("%s => check username:[%s] soap_wsse_verify_Password err.\n", __func__, soap->header->wsse__Security->UsernameToken->Username);
        	return -1;
  			}
  		}
	}
	
	int ret = soap_sender_fault_subcode(soap
	            , "wsse:FailedAuthentication"
	            , "The request requires authorization."
	            , NULL);
	printf("%s => check wsse__Security param err, ret:%d.\n", __func__, ret);
	return -1;
}

char *generate_uuid(int type)
{
    time_t time_n;
    struct tm *tm_t;
    static char uuid[LARGE_INFO_LENGTH];
    unsigned char macaddr[MACH_ADDR_LENGTH];

    net_get_hwaddr(ETH_NAME, macaddr);
    if(type == 1)
    {
        time_n = time(NULL);
        tm_t = localtime(&time_n);
        sprintf(uuid,"urn:uuid:1319d68a-%d%d%d-%d%d-%d%d-%02X%02X%02X%02X%02X%02X",
                tm_t->tm_wday,tm_t->tm_mday,tm_t->tm_mon,tm_t->tm_year,
                tm_t->tm_hour,tm_t->tm_min,tm_t->tm_sec,
                macaddr[0], macaddr[1], macaddr[2], macaddr[3], macaddr[4], macaddr[5]);
    }
    else if(type == 0)
    {
        sprintf(uuid,"urn:uuid:1319d68a-3b10-43a5-95c2-%02X%02X%02X%02X%02X%02X",
                macaddr[0], macaddr[1], macaddr[2], macaddr[3], macaddr[4], macaddr[5]);  
    }
    return uuid;
}

int __init__Hello(struct soap *soap, struct d__HelloType *p__Hello)
{
    char _IPAddr[INFO_LENGTH];
    char *_HwId;
    int i = 0;
    NET_IPV4 ip;

    ip.int32 = net_get_ifaddr(ETH_NAME);
    _HwId = generate_uuid(0);

    sprintf(_IPAddr, "http://%03d.%03d.%03d.%03d:%d/onvif/device_service", ip.str[0], ip.str[1], ip.str[2], ip.str[3], g_nvt->p.port);

    p__Hello->wsa__EndpointReference.Address= (char *)soap_malloc(soap, sizeof(char)*LARGE_INFO_LENGTH);
    strcpy(p__Hello->wsa__EndpointReference.Address, _HwId);
    p__Hello->wsa__EndpointReference.PortType = NULL;
    p__Hello->wsa__EndpointReference.ServiceName = NULL;
    p__Hello->wsa__EndpointReference.ReferenceProperties = NULL;
    p__Hello->wsa__EndpointReference.ReferenceParameters = NULL;
    p__Hello->wsa__EndpointReference.__size = 0;
    p__Hello->wsa__EndpointReference.__any = NULL;
    p__Hello->d__Scopes = (struct d__ScopesType*)soap_malloc(soap, sizeof(struct d__ScopesType));
    p__Hello->d__Scopes->__item = (char*)soap_malloc(soap, sizeof(char)*LARGE_INFO_LENGTH);
    memset(p__Hello->d__Scopes->__item,0,sizeof(p__Hello->d__Scopes->__item));
    strcat(p__Hello->d__Scopes->__item, "onvif://www.onvif.org/type/Network_Video_Transmitter");

    p__Hello->d__XAddrs = (char *)soap_malloc(soap, sizeof(char) * INFO_LENGTH);
    strcpy(p__Hello->d__XAddrs, _IPAddr);
    p__Hello->d__Scopes->MatchBy = NULL;
    p__Hello->d__Types = soap_malloc(soap, sizeof(char*)*LARGE_INFO_LENGTH);
    p__Hello->d__Types[0] = soap_malloc(soap, sizeof(char)*LARGE_INFO_LENGTH);
    strcpy(p__Hello->d__Types[0], "dn:NetworkVideoTransmitter");
    p__Hello->d__MetadataVersion = 1;
    return 0;
}

int udp_hello()
{
    int itr= 0;
    unsigned char macaddr[MACH_ADDR_LENGTH];
    char *_HwId;

    struct soap *soaph;
    struct SOAP_ENV__Header header;
    struct wsa__EndpointReferenceType wsa__EndpointReference;
    struct d__HelloType d__Hello;
    unsigned char loop = 1;

    net_get_hwaddr(ETH_NAME, macaddr);
    _HwId = generate_uuid(1);
    /* HELLO */
    while(itr < NUM_BROADCAST)
    {
        //int fd = create_server_socket_udp();
        soaph=soap_new();
        soaph->connect_flags= SO_BROADCAST;
        soap_init1(soaph,SOAP_IO_UDP | SO_BROADCAST);
        soaph->send_timeout = 1;
        soaph->recv_timeout = 1;
        //soaph->master = *server_udp;
        //soaph->socket = *server_udp;
        soaph->errmode = 0;
        soaph->bind_flags = SO_REUSEADDR;

        if(!soap_valid_socket(soap_bind(soaph, NULL, 3702, 128)))
        {
            fprintf(stderr, "[%s][%d] bind port error!\n", __FUNCTION__, __LINE__);
            soap_print_fault(soaph, stderr);
            goto err;
        }  
        setsockopt(soaph->master, IPPROTO_IP, IP_MULTICAST_LOOP, &loop, sizeof(loop));

        soap_set_omode(soaph, SOAP_ENC_ZLIB);
        soap_default_wsa__EndpointReferenceType(soaph, &wsa__EndpointReference);
        soap_default_SOAP_ENV__Header(soaph, &header);

        header.wsa__MessageID =(char *)soap_malloc(soaph, sizeof(char) * INFO_LENGTH);
        strcpy(header.wsa__MessageID, _HwId);
        header.wsa__To = "urn:schemas-xmlsoap-org:ws:2005:04:discovery";
        header.wsa__Action = "http://schemas.xmlsoap.org/ws/2005/04/discovery/Hello";

        soaph->header = &header;

        soap_default_d__HelloType(soaph, &d__Hello);
        if (__init__Hello(soaph, &d__Hello))
        {
            goto err;
        }
        if (soap_call___dnrd__Hello(soaph, "soap.udp://239.255.255.250:3702/", NULL, &d__Hello, NULL))
        {
            soap_print_fault(soaph, stderr);
            goto err;
        }

        soap_destroy(soaph);
        soap_end(soaph);
        soap_free(soaph);
        itr++;
        usleep(4000);
    }
    
    return 0;
err:
    soap_destroy(soaph);
    soap_end(soaph);
    soap_free(soaph);
    return -1;
    /* End-HELLO */
}

int onvif_hello(void)
{
    udp_hello();
    return 0;
}

int onvif_bye(void)
{
    char ip[32];
    char _IPAddr[INFO_LENGTH];
    char *_HwId;
    struct soap *soapb;
    struct SOAP_ENV__Header header;
    struct wsa__EndpointReferenceType wsa__EndpointReference;

    soapb=soap_new();
    soapb->connect_flags= SO_BROADCAST;
    soap_init1(soapb,SO_BROADCAST);
    soapb->send_timeout = 1;
    soapb->recv_timeout = 1;
    soapb->errmode = 0;
    soapb->bind_flags = SO_REUSEADDR;
    soap_set_omode(soapb, SOAP_ENC_ZLIB);
    soap_default_wsa__EndpointReferenceType(soapb, &wsa__EndpointReference);
    soap_default_SOAP_ENV__Header(soapb, &header);
    soapb->header = &header;

    header.wsa__MessageID =(char *)soap_malloc(soapb, sizeof(char) * INFO_LENGTH);
    _HwId = generate_uuid(1);
    strcpy(header.wsa__MessageID, _HwId);
    header.wsa__To = "urn:schemas-xmlsoap-org:ws:2005:04:discovery";
    header.wsa__Action = "http://schemas.xmlsoap.org/ws/2005/04/discovery/Bye";

    //fill d_Bye
    struct d__ByeType d__Bye;
    soap_default_d__ByeType(soapb, &d__Bye);

    sprintf(_IPAddr, "http://%s:%d/onvif/device_service", ip, 8000);

    d__Bye.wsa__EndpointReference.Address = (char *)soap_malloc(soapb, sizeof(char) * LARGE_INFO_LENGTH);
    strcpy(d__Bye.wsa__EndpointReference.Address, _HwId);

    d__Bye.wsa__EndpointReference.PortType = (char **)soap_malloc(soapb, sizeof(char *));
    d__Bye.wsa__EndpointReference.PortType[0]= (char *)soap_malloc(soapb, sizeof(char) * SMALL_INFO_LENGTH);
    strcpy(d__Bye.wsa__EndpointReference.PortType[0], "ttl");

    d__Bye.wsa__EndpointReference.ServiceName=NULL;
    d__Bye.wsa__EndpointReference.ReferenceProperties=NULL;
    d__Bye.wsa__EndpointReference.ReferenceParameters=NULL;
    d__Bye.wsa__EndpointReference.__size = 0;
    d__Bye.wsa__EndpointReference.__any = (char **)soap_malloc(soapb, sizeof(char *));
    d__Bye.wsa__EndpointReference.__any[0] = (char *)soap_malloc(soapb, sizeof(char) * SMALL_INFO_LENGTH);
    strcpy(d__Bye.wsa__EndpointReference.__any[0], "any");

    d__Bye.wsa__EndpointReference.__anyAttribute = NULL;

    d__Bye.d__Scopes = (struct d__ScopesType*)soap_malloc(soapb, sizeof(struct d__ScopesType));
    d__Bye.d__Scopes->__item = "onvif://www.onvif.org/type/Network_Video_Transmitter";
    d__Bye.d__Scopes->MatchBy = NULL;
    d__Bye.d__XAddrs= (char *) soap_malloc(soapb, sizeof(char) * INFO_LENGTH);
    strcpy(d__Bye.d__XAddrs, _IPAddr);
    d__Bye.d__Types= (char **) soap_malloc(soapb, sizeof(char *));
    d__Bye.d__Types[0] = "dn:NetworkVideoTransmitter";
    d__Bye.d__MetadataVersion = NULL;

    if (soap_call___dnrd__Bye(soapb, "soap.udp://239.255.255.250:3702/", NULL, &d__Bye, NULL))
    {
        soap_print_fault(soapb, stderr);
    }

    soap_destroy(soapb);
    soap_end(soapb);
    soap_done(soapb);
    free(soapb);
    return 0;
}

SOAP_FMAC5 int SOAP_FMAC6 __dndl__Probe(struct soap*soap,
                        struct d__ProbeType *d__Probe,
                        struct d__ProbeMatchesType *d__ProbeMatches)
{
    int flag = 0;
    char *_HwId;
    char ipaddr[64];
    NET_IPV4 ip;

    if(g_nvt->t.discovery_mode == 1)
        return SOAP_FAULT;

    if(d__Probe->d__Scopes != NULL)
    {
        if(d__Probe->d__Scopes->MatchBy && strstr(d__Probe->d__Scopes->MatchBy, "InvalidMatchRule"))
        {
            flag = 0;
        }
        else
            flag = 1;
    }
    else
        flag = 1;
 
    if(flag == 1)
    {
        ip.int32 =  net_get_ifaddr(ETH_NAME);
        _HwId = generate_uuid(0);
        sprintf(ipaddr, "http://%d.%d.%d.%d:%d/onvif/device_service", ip.str[0], ip.str[1], ip.str[2], ip.str[3], g_nvt->p.port);

        d__ProbeMatches->__sizeProbeMatch = 1;
        d__ProbeMatches->__size = 0;
        d__ProbeMatches->__any= NULL;
        d__ProbeMatches->__anyAttribute = NULL;
        
        d__ProbeMatches->ProbeMatch = (struct d__ProbeMatchType *)soap_malloc(soap, sizeof(struct d__ProbeMatchType));        
        memset(d__ProbeMatches->ProbeMatch, 0, sizeof(struct d__ProbeMatchType));
        d__ProbeMatches->ProbeMatch->d__XAddrs = (char *)soap_malloc(soap, sizeof(char) * INFO_LENGTH);
        d__ProbeMatches->ProbeMatch->d__Types = (char **)soap_malloc(soap, sizeof(char*) * LARGE_INFO_LENGTH);
        d__ProbeMatches->ProbeMatch->d__Types[0] = (char *)soap_malloc(soap, sizeof(char) * LARGE_INFO_LENGTH);
        memset(d__ProbeMatches->ProbeMatch->d__Types[0], 0, sizeof(char)*LARGE_INFO_LENGTH);
        d__ProbeMatches->ProbeMatch->d__Scopes = (struct d__ScopesType*)soap_malloc(soap,sizeof(struct d__ScopesType));
        memset(d__ProbeMatches->ProbeMatch->d__Scopes, 0, sizeof(struct d__ScopesType));
        d__ProbeMatches->ProbeMatch->__size = 0;
        d__ProbeMatches->ProbeMatch->__any= NULL;
        d__ProbeMatches->ProbeMatch->__anyAttribute = NULL;
        d__ProbeMatches->ProbeMatch->d__MetadataVersion = 1;
        strcpy(d__ProbeMatches->ProbeMatch->d__XAddrs, ipaddr);
        
        d__ProbeMatches->ProbeMatch->wsa__EndpointReference.ReferenceProperties = NULL;
        d__ProbeMatches->ProbeMatch->wsa__EndpointReference.ReferenceParameters = NULL;
        d__ProbeMatches->ProbeMatch->wsa__EndpointReference.ServiceName = NULL;
        d__ProbeMatches->ProbeMatch->wsa__EndpointReference.PortType = (char **)soap_malloc(soap, sizeof(char *) * SMALL_INFO_LENGTH);
        d__ProbeMatches->ProbeMatch->wsa__EndpointReference.__any = (char **)soap_malloc(soap, sizeof(char*) * SMALL_INFO_LENGTH);
        d__ProbeMatches->ProbeMatch->wsa__EndpointReference.__anyAttribute = NULL;
        d__ProbeMatches->ProbeMatch->wsa__EndpointReference.Address = (char *)soap_malloc(soap, sizeof(char) * INFO_LENGTH);
 
        /*--------------------------------------------------------*/
        d__ProbeMatches->ProbeMatch->d__Scopes->__item =(char *)soap_malloc(soap, LARGE_INFO_LENGTH);
        memset(d__ProbeMatches->ProbeMatch->d__Scopes->__item, 0, sizeof(char)*LARGE_INFO_LENGTH);
        
        strcpy(d__ProbeMatches->ProbeMatch->d__Scopes->__item, "onvif://www.onvif.org/NetworkVideoTransmitter");
        strcat(d__ProbeMatches->ProbeMatch->d__Scopes->__item, " ");
        strcat(d__ProbeMatches->ProbeMatch->d__Scopes->__item, "onvif://www.onvif.org/name/NVR");
        strcat(d__ProbeMatches->ProbeMatch->d__Scopes->__item, " ");
        strcat(d__ProbeMatches->ProbeMatch->d__Scopes->__item, "onvif://www.onvif.org/hardware/");
        strcat(d__ProbeMatches->ProbeMatch->d__Scopes->__item, g_nvt->t.dev->hwid);
        strcat(d__ProbeMatches->ProbeMatch->d__Scopes->__item, " ");
        
        /*--------------------------------------------------------*/
        
        
        d__ProbeMatches->ProbeMatch->d__Scopes->MatchBy = NULL;
        
        if (!d__Probe->d__Types || !strcmp(d__Probe->d__Types[0], ""))
            strcpy(d__ProbeMatches->ProbeMatch->d__Types[0], "dn:NetworkVideoTransmitter");// dn:NetworkVideoTransmitter
        else
        {
            fprintf(stderr, "[%s]------->type=%s\n", __FUNCTION__, d__Probe->d__Types[0]);
            strcpy(d__ProbeMatches->ProbeMatch->d__Types[0], d__Probe->d__Types[0]);
        }
        d__ProbeMatches->ProbeMatch->wsa__EndpointReference.PortType[0] = (char *)soap_malloc(soap, sizeof(char) * SMALL_INFO_LENGTH);
        strcpy(d__ProbeMatches->ProbeMatch->wsa__EndpointReference.PortType[0], "ttl");
        d__ProbeMatches->ProbeMatch->wsa__EndpointReference.__any[0] = (char *)soap_malloc(soap, sizeof(char) * SMALL_INFO_LENGTH);
        strcpy(d__ProbeMatches->ProbeMatch->wsa__EndpointReference.__any[0], "Any");
        d__ProbeMatches->ProbeMatch->wsa__EndpointReference.__size = 0;
        strcpy(d__ProbeMatches->ProbeMatch->wsa__EndpointReference.Address, _HwId);
    }
    else
    {
        return SOAP_FAULT;
    }
    if(soap->header != NULL)
    {
        soap->header->wsa__RelatesTo = (struct wsa__Relationship*)soap_malloc(soap, sizeof(struct wsa__Relationship));
        soap->header->wsa__RelatesTo->__item =(char *)soap_malloc(soap, sizeof(char) * INFO_LENGTH);
        strcpy(soap->header->wsa__RelatesTo->__item, soap->header->wsa__MessageID);
        soap->header->wsa__RelatesTo->RelationshipType = NULL;
        soap->header->wsa__RelatesTo->__anyAttribute = NULL;
        soap->header->wsa5__MessageID = NULL;
        soap->header->wsa5__To= NULL;
        soap->header->wsa5__Action = NULL;
        soap->header->wsa__To =(char *)soap_malloc(soap, sizeof(char) * INFO_LENGTH);
        strcpy(soap->header->wsa__To, "http://schemas.xmlsoap.org/ws/2004/08/addressing/role/anonymous");
        soap->header->wsa__Action =(char *)soap_malloc(soap, sizeof(char) * INFO_LENGTH);
        strcpy(soap->header->wsa__Action, "http://schemas.xmlsoap.org/ws/2005/04/discovery/ProbeMatches");
        soap->header->wsa__MessageID =(char *)soap_malloc(soap, sizeof(char) * INFO_LENGTH);
        strcpy(soap->header->wsa__MessageID,_HwId);
    }
    return SOAP_OK;
}
SOAP_FMAC5 int SOAP_FMAC6 __tds__GetSystemDateAndTime(struct soap *soap, struct _tds__GetSystemDateAndTime *GetTimeReq, struct _tds__GetSystemDateAndTimeResponse *GetTimeResp)
{
    int ret;
    systime_t *time = (systime_t *)calloc(1, sizeof(systime_t));
    memset(time, 0, sizeof(systime_t));

    ret = get_dev_time(time);
    if(ret < 0)
    {
        fprintf(stderr, "[%s][%d] get device time error!\n", __FUNCTION__, __LINE__);
        if(time) 
        {
            free(time); 
            time = NULL;
        }
        return SOAP_FAULT;
    }

    GetTimeResp->SystemDateAndTime = (struct tt__SystemDateTime*)soap_malloc(soap, sizeof(struct tt__SystemDateTime));
    memset(GetTimeResp->SystemDateAndTime, 0, sizeof(struct tt__SystemDateTime));

    GetTimeResp->SystemDateAndTime->DateTimeType = time->datetype; 
    GetTimeResp->SystemDateAndTime->DaylightSavings = time->daylightsaving;
    GetTimeResp->SystemDateAndTime->Extension = NULL;
    GetTimeResp->SystemDateAndTime->TimeZone = (struct tt__TimeZone*)soap_malloc(soap, sizeof(struct tt__TimeZone));
    GetTimeResp->SystemDateAndTime->TimeZone->TZ = (char *)soap_malloc(soap, sizeof(char) * INFO_LENGTH);
    strcpy(GetTimeResp->SystemDateAndTime->TimeZone->TZ, time->TZ);

    GetTimeResp->SystemDateAndTime->UTCDateTime = (struct tt__DateTime*)soap_malloc(soap, sizeof(struct tt__DateTime));
    GetTimeResp->SystemDateAndTime->UTCDateTime->Time = (struct tt__Time*)soap_malloc(soap, sizeof(struct tt__Time));
    GetTimeResp->SystemDateAndTime->UTCDateTime->Date = (struct tt__Date*)soap_malloc(soap, sizeof(struct tt__Date)); 
   
    GetTimeResp->SystemDateAndTime->UTCDateTime->Date->Year = time->UTCDateTime.year;
    GetTimeResp->SystemDateAndTime->UTCDateTime->Date->Month = time->UTCDateTime.month;
    GetTimeResp->SystemDateAndTime->UTCDateTime->Date->Day = time->UTCDateTime.day;

    GetTimeResp->SystemDateAndTime->UTCDateTime->Time->Hour = time->UTCDateTime.hour;
    GetTimeResp->SystemDateAndTime->UTCDateTime->Time->Minute = time->UTCDateTime.minute;
    GetTimeResp->SystemDateAndTime->UTCDateTime->Time->Second = time->UTCDateTime.second;

    GetTimeResp->SystemDateAndTime->LocalDateTime = (struct tt__DateTime*)soap_malloc(soap, sizeof(struct tt__DateTime));
    GetTimeResp->SystemDateAndTime->LocalDateTime->Time = (struct tt__Time*)soap_malloc(soap, sizeof(struct tt__Time));
    GetTimeResp->SystemDateAndTime->LocalDateTime->Date = (struct tt__Date*)soap_malloc(soap, sizeof(struct tt__Date));
    
    GetTimeResp->SystemDateAndTime->LocalDateTime->Time->Hour = time->LocalDateTime.hour;
    GetTimeResp->SystemDateAndTime->LocalDateTime->Time->Minute = time->LocalDateTime.minute;
    GetTimeResp->SystemDateAndTime->LocalDateTime->Time->Second = time->LocalDateTime.second;
    GetTimeResp->SystemDateAndTime->LocalDateTime->Date->Year = time->LocalDateTime.year;
    GetTimeResp->SystemDateAndTime->LocalDateTime->Date->Month = time->LocalDateTime.month;
    GetTimeResp->SystemDateAndTime->LocalDateTime->Date->Day = time->LocalDateTime.day;

    GetTimeResp->SystemDateAndTime->Extension = NULL;
    GetTimeResp->SystemDateAndTime->__anyAttribute = NULL;

    if(time)
    {
        free(time);
        time = NULL;
    }
    return SOAP_OK;
}

char *_TZ_name[25] = {
	"GMT-12",// Eniwetok, Kwajalein", 
	"GMT-11",// Midway Island, Samoa",
	"GMT-10",// Hawaii", // HAW10
	"GMT-09",// Alaska", // AKST9AKDT,M3.2.0,M11.1.0
	"GMT-08",// Pacific Time (US & Canada), Tijuana", // PST8PDT,M3.2.0,M11.1.0
	"GMT-07",// Mountain Time (US & Canada), Arizona", // MST7
	"GMT-06",// Central Time (US & Canada), Mexico City, Tegucigalpa, Saskatchewan", // CST6CDT,M3.2.0,M11.1.0
	"GMT-05",// Eastern Time (US & Canada), Indiana(East), Bogota, Lima", // EST5EDT,M3.2.0,M11.1.0
	"GMT-04",// Atlantic Time (Canada), Caracas, La Paz", // AST4ADT,M4.1.0/00:01:00,M10.5.0/00:01:00
	"GMT-03",// Brasilia, Buenos Aires, Georgetown",
	"GMT-02",// Mid-Atlantic",
	"GMT-01",// Azores, Cape Verdes Is.",
	"GMT+00",// GMT, Dublin, Edinburgh, London, Lisbon, Monrovia, Casablanca", // GMT+0BST-1,M3.5.0/01:00:00,M10.5.0/02:00:00
	"GMT+01",// Berlin, Stockholm, Rome, Bern, Brussels, Vienna, Paris, Madrid, Amsterdam, Prague, Warsaw, Budapest", // CET-1CEST-2,M3.5.0/02:00:00,M10.5.0/03:00:00
	"GMT+02",// Athens, Helsinki, Istanbul, Cairo, Eastern Europe, Harare, Pretoria, Israel", // EET-2EEST-3,M3.5.0/03:00:00,M10.5.0/04:00:00
	"GMT+03",// Baghdad, Kuwait, Nairobi, Riyadh, Moscow, St. Petersburg, Kazan, Volgograd", // MSK-3MSD,M3.5.0/2,M10.5.0/3
	"GMT+04",// Abu Dhabi, Muscat, Tbilisi",
	"GMT+05",// Islamabad, Karachi, Ekaterinburg, Tashkent",
	"GMT+06",// Alma Ata, Dhaka",
	"GMT+07",// Bangkok, Jakarta, Hanoi",
	"GMT+08",// Taipei, Beijing, Chongqing, Urumqi, Hong Kong, Perth, Singapore",
	"GMT+09",// Tokyo, Osaka, Sapporo, Seoul, Yakutsk",
	"GMT+10",// Brisbane, Melbourne, Sydney, Guam, Port Moresby, Vladivostok, Hobart",
	"GMT+11",// Magadan, Solomon Is., New Caledonia",
	"GMT+12",// Fiji, Kamchatka, Marshall Is., Wellington, Auckland"
};

int get_timezone_num(char *TZ)
{
    int i = 0; 
    for (; i < 25; ++i)
    {
        if (!strncmp(_TZ_name[i], TZ, 6))
        {
            return i;
        }
    }
    if (!strcmp(TZ, "ChinaStandardTime-8"))
    {
        return 20;
    }
        
	if (strncmp(TZ, "IDLW", 4)==0)return 0;//-12
	else if (strncmp(TZ, "NT", 2)==0)return 1;//-11
	else if ((strncmp(TZ, "AHST", 4)==0)||(strncmp(TZ, "CAT", 3)==0)||(strncmp(TZ, "HST", 3)==0)||(strncmp(TZ, "HDT", 3)==0))return 2;//-10
	else if ((strncmp(TZ, "YST", 3)==0)||(strncmp(TZ, "YDT", 3)==0))return 3;//-9
	else if ((strncmp(TZ, "PST", 3)==0)||(strncmp(TZ, "PDT", 3)==0))return 4;//-8
	else if ((strncmp(TZ, "MST", 3)==0)||(strncmp(TZ, "MDT", 3)==0))return 5;//-7
	else if ((strncmp(TZ, "CST", 3)==0)||(strncmp(TZ, "CDT", 3)==0))return 6;//-6
	else if ((strncmp(TZ, "EST", 3)==0)||(strncmp(TZ, "EDT", 3)==0))return 7;//-5
	else if ((strncmp(TZ, "AST", 3)==0)||(strncmp(TZ, "ADT", 3)==0))return 8;//-4
	else if (strncmp(TZ, "GMT-03", 6)==0)return 9;//-3
	else if (strncmp(TZ, "AT", 2)==0)return 10;//-2
	else if (strncmp(TZ, "WAT", 3)==0)return 11;//-1
	else if ((strncmp(TZ, "GMT", 3)==0)||(strncmp(TZ, "UT", 2)==0)||(strncmp(TZ, "UTC", 3)==0)||(strncmp(TZ, "BST", 3)==0))return 12;//-0
	else if ((strncmp(TZ, "CET", 3)==0)||(strncmp(TZ, "FWT", 3)==0)||(strncmp(TZ, "MET", 3)==0)\
		||(strncmp(TZ, "MEWT", 4)==0)||(strncmp(TZ, "SWT", 3)==0)||(strncmp(TZ, "MEST", 4)==0)\
		||(strncmp(TZ, "MESZ", 4)==0)||(strncmp(TZ, "SST", 3)==0)||(strncmp(TZ, "FST", 3)==0))return 13;//1
	else if (strncmp(TZ, "EET", 3)==0)return 14;//2
	else if (strncmp(TZ, "BT", 2)==0)return 15;//3
	else if (strncmp(TZ, "ZP4", 3)==0)return 16;//4
	else if (strncmp(TZ, "ZP5", 3)==0)return 17;//5
	else if (strncmp(TZ, "ZP6", 3)==0)return 18;//6
	else if (strncmp(TZ, "ZP7", 3)==0)return 19;//7
	else if (strncmp(TZ, "WAST", 4)==0)return 20;//8
	else if (strncmp(TZ, "JST", 3)==0)return 21;//9
	else if (strncmp(TZ, "ACT", 3)==0)return 22;//10
	else if (strncmp(TZ, "EAST", 4)==0)return 23;//11
	else if (strncmp(TZ, "IDLE", 4)==0)return 24;//12

    return -1;//ERROR    
}

SOAP_FMAC5 int SOAP_FMAC6 __tds__SetSystemDateAndTime(struct soap *soap, struct _tds__SetSystemDateAndTime *SetTimeReq, struct _tds__SetSystemDateAndTimeResponse *SetTimeResp)
{

	int ret = 0;
	char *_TZ = NULL;

    systime_t *_time = (systime_t *)calloc(1, sizeof(systime_t));
    memset(_time, 0, sizeof(systime_t));

    _time->datetype = SetTimeReq->DateTimeType; //Manual = 0, NTP = 1
#if 1 //maohw
	/* TimeZone */
	if(SetTimeReq->TimeZone)
	{
		_TZ  = SetTimeReq->TimeZone->TZ;
                
		//maohw ret = get_timezone_num(_TZ);
		if(ret == -1)
		{
			return SOAP_FAULT;
		}             
	}
#endif
	_time->daylightsaving = SetTimeReq->DaylightSavings;
	if(_TZ)
	  strncpy(_time->TZ, _TZ, sizeof(_time->TZ)-1);
	
	if(_time->datetype == 1) // NTP
	{
    fprintf(stderr, "[%s][%d] using ntp sync!\n", __FUNCTION__, __LINE__);
		return SOAP_OK;
	}
#if 1	

	if(SetTimeReq->UTCDateTime)
	{
		_time->UTCDateTime.year = SetTimeReq->UTCDateTime->Date->Year;
		_time->UTCDateTime.month = SetTimeReq->UTCDateTime->Date->Month;
		_time->UTCDateTime.day = SetTimeReq->UTCDateTime->Date->Day;
    _time->UTCDateTime.hour = SetTimeReq->UTCDateTime->Time->Hour;
    _time->UTCDateTime.minute = SetTimeReq->UTCDateTime->Time->Minute;
		_time->UTCDateTime.second = SetTimeReq->UTCDateTime->Time->Second;
                
		if((_time->UTCDateTime.hour > 24) 
                || (_time->UTCDateTime.minute > 60) 
                || (_time->UTCDateTime.second > 60) 
                || (_time->UTCDateTime.month  > 12) 
                || (_time->UTCDateTime.day    > 31))
		{		
			return SOAP_FAULT;
		}
	}

#endif

    ret = set_dev_time(_time);

    if(_time)
    {
        free(_time);
        _time = NULL;
    }
    //fprintf(stderr, "[%s] set systime ok! ret=%d\n", __FUNCTION__, ret);

	return SOAP_OK;
}

SOAP_FMAC5 int SOAP_FMAC6 __tds__GetDeviceInformation(struct soap *soap, struct _tds__GetDeviceInformation *tds__GetDeviceInformation, struct _tds__GetDeviceInformationResponse *DeviceInfo)
{
    get_dev_info();
    DeviceInfo->Manufacturer = (char *)soap_malloc(soap, sizeof(char) * LARGE_INFO_LENGTH);
    DeviceInfo->Model = (char *)soap_malloc(soap, sizeof(char) * LARGE_INFO_LENGTH);
    DeviceInfo->FirmwareVersion = (char *)soap_malloc(soap, sizeof(char) * LARGE_INFO_LENGTH);
    DeviceInfo->SerialNumber = (char *)soap_malloc(soap, sizeof(char) * LARGE_INFO_LENGTH);
    DeviceInfo->HardwareId = (char *)soap_malloc(soap, sizeof(char) * LARGE_INFO_LENGTH);

    strcpy(DeviceInfo->Manufacturer, "HIVIEW"/*"Shenzhen HIVIEW Technology Co., LTD."*/);
    strcpy(DeviceInfo->Model,  "ONVIF");
    strcpy(DeviceInfo->FirmwareVersion, g_nvt->t.dev->sw);
    strcpy(DeviceInfo->SerialNumber, g_nvt->t.dev->serial);
    strcpy(DeviceInfo->HardwareId, g_nvt->t.dev->hwid);
    /*fprintf(stderr, "devtype=[%d] sw=[%s] serial=[%s] hwid=[%s]\n"
                        ,DeviceInfo->Model
                        ,DeviceInfo->FirmwareVersion
                        ,DeviceInfo->SerialNumber
                        ,DeviceInfo->HardwareId);*/
    return SOAP_OK;
}

SOAP_FMAC5 int SOAP_FMAC6 __tds__GetCapabilities(struct soap *soap, struct _tds__GetCapabilities *tds__GetCapabilities, struct _tds__GetCapabilitiesResponse *GetCap)
{
    int _Category;
    char _IPv4Address[LARGE_INFO_LENGTH];

    NET_IPV4 ip;

    if(tds__GetCapabilities->Category == NULL)
    {
        tds__GetCapabilities->Category = (enum tt__CapabilityCategory *)soap_malloc(soap, sizeof(enum tt__CapabilityCategory));
        *tds__GetCapabilities->Category =  ALL;
        _Category = ALL;
    }
    else
    {
        _Category = *tds__GetCapabilities->Category;       
    }

    ip.int32 =  net_get_ifaddr(ETH_NAME);

    sprintf(_IPv4Address, "http://%d.%d.%d.%d:%d/onvif/services", ip.str[0], ip.str[1], ip.str[2], ip.str[3], g_nvt->p.port);

    //init Capabilities
    GetCap->Capabilities = (struct tt__Capabilities*)soap_malloc(soap, sizeof(struct tt__Capabilities));
    GetCap->Capabilities->Analytics = NULL;
    GetCap->Capabilities->Device = NULL;
    GetCap->Capabilities->Events = NULL;
    GetCap->Capabilities->Imaging = NULL;
    GetCap->Capabilities->Media = NULL;
    GetCap->Capabilities->PTZ = NULL;

    //malloc for capabilities extension
    GetCap->Capabilities->Extension = (struct tt__CapabilitiesExtension*)soap_malloc(soap, sizeof(struct tt__CapabilitiesExtension));
    memset(GetCap->Capabilities->Extension, 0, sizeof(struct tt__CapabilitiesExtension));

    //init Extension_device_io
    GetCap->Capabilities->Extension->DeviceIO = (struct tt__DeviceIOCapabilities*)soap_malloc(soap, sizeof(struct tt__DeviceIOCapabilities));
    memset(GetCap->Capabilities->Extension->DeviceIO, 0, sizeof(struct tt__DeviceIOCapabilities));
    GetCap->Capabilities->Extension->DeviceIO->XAddr = (char *)soap_malloc(soap, sizeof(char) * INFO_LENGTH);
    strcpy(GetCap->Capabilities->Extension->DeviceIO->XAddr,_IPv4Address);
    GetCap->Capabilities->Extension->DeviceIO->VideoSources = TRUE;
    GetCap->Capabilities->Extension->DeviceIO->VideoOutputs = TRUE;
    GetCap->Capabilities->Extension->DeviceIO->AudioSources = TRUE;
    GetCap->Capabilities->Extension->DeviceIO->AudioOutputs = TRUE;
    GetCap->Capabilities->Extension->DeviceIO->RelayOutputs = TRUE;
    GetCap->Capabilities->Extension->DeviceIO->__any = NULL;
    GetCap->Capabilities->Extension->DeviceIO->__anyAttribute = NULL;
    GetCap->Capabilities->Extension->DeviceIO->__size = 1;

    //init Extension_others
#if 0
    GetCap->Capabilities->Extension->Replay = NULL;
#else    
    GetCap->Capabilities->Extension->Replay = (struct tt__ReplayCapabilities*)soap_malloc(soap, sizeof(struct tt__ReplayCapabilities));
    GetCap->Capabilities->Extension->Replay->XAddr = (char *)soap_malloc(soap, sizeof(char) * LARGE_INFO_LENGTH);
    strcpy(GetCap->Capabilities->Extension->Replay->XAddr, _IPv4Address);
    GetCap->Capabilities->Extension->Replay->__any = NULL;
    GetCap->Capabilities->Extension->Replay->__anyAttribute = NULL;
    GetCap->Capabilities->Extension->Replay->__size = 1;
#endif
    
#if 0
    GetCap->Capabilities->Extension->Search = NULL;
#else
    GetCap->Capabilities->Extension->Search = (struct tt__SearchCapabilities*)soap_malloc(soap, sizeof(struct tt__SearchCapabilities));
    GetCap->Capabilities->Extension->Search->MetadataSearch = _false;
    GetCap->Capabilities->Extension->Search->XAddr = (char *)soap_malloc(soap, sizeof(char) * LARGE_INFO_LENGTH);
    strcpy(GetCap->Capabilities->Extension->Search->XAddr, _IPv4Address);
    GetCap->Capabilities->Extension->Search->__any = NULL;
    GetCap->Capabilities->Extension->Search->__anyAttribute = NULL;
    GetCap->Capabilities->Extension->Search->__size = 1;
#endif

    GetCap->Capabilities->Extension->Recording = NULL;
    GetCap->Capabilities->Extension->Receiver = NULL;
    GetCap->Capabilities->Extension->Display = NULL;
    GetCap->Capabilities->Extension->AnalyticsDevice = NULL;
    GetCap->Capabilities->Extension->Extensions = NULL;
    GetCap->Capabilities->Extension->__any = NULL;
    GetCap->Capabilities->Extension->__size = 1;

    if((_Category == ALL) || (_Category == ANALYTICS))
    {
        GetCap->Capabilities->Analytics = (struct tt__AnalyticsCapabilities*)soap_malloc(soap, sizeof(struct tt__AnalyticsCapabilities));
        memset(GetCap->Capabilities->Analytics, 0, sizeof(struct tt__AnalyticsCapabilities));
        GetCap->Capabilities->Analytics->XAddr = (char *)soap_malloc(soap, sizeof(char) * LARGE_INFO_LENGTH);
        strcpy(GetCap->Capabilities->Analytics->XAddr, _IPv4Address);
        GetCap->Capabilities->Analytics->RuleSupport = FALSE;
        GetCap->Capabilities->Analytics->AnalyticsModuleSupport = FALSE;
    }

    if((_Category == ALL) || (_Category == DEVICE))
    {
        GetCap->Capabilities->Device = (struct tt__DeviceCapabilities*)soap_malloc(soap, sizeof(struct tt__DeviceCapabilities));
        memset(GetCap->Capabilities->Device, 0, sizeof(struct tt__DeviceCapabilities));
        GetCap->Capabilities->Device->XAddr = (char *)soap_malloc(soap, sizeof(char) * LARGE_INFO_LENGTH);
        strcpy(GetCap->Capabilities->Device->XAddr, _IPv4Address);
        GetCap->Capabilities->Device->Extension = NULL;
        GetCap->Capabilities->Device->Network = (struct tt__NetworkCapabilities*)soap_malloc(soap, sizeof(struct tt__NetworkCapabilities));
        GetCap->Capabilities->Device->Network->IPFilter = (enum xsd__boolean_ *)soap_malloc(soap, sizeof(int));
        GetCap->Capabilities->Device->Network->ZeroConfiguration = (enum xsd__boolean_ *)soap_malloc(soap, sizeof(int));
        GetCap->Capabilities->Device->Network->IPVersion6 = (enum xsd__boolean_ *)soap_malloc(soap, sizeof(int));
        GetCap->Capabilities->Device->Network->DynDNS = (enum xsd__boolean_ *)soap_malloc(soap, sizeof(int));
        //set value
        *GetCap->Capabilities->Device->Network->IPFilter = _false;
        *GetCap->Capabilities->Device->Network->ZeroConfiguration = _false;
        *GetCap->Capabilities->Device->Network->IPVersion6 = _false;
        *GetCap->Capabilities->Device->Network->DynDNS = _false;
        GetCap->Capabilities->Device->Network->Extension = (
        struct tt__NetworkCapabilitiesExtension*)soap_malloc(soap, sizeof(struct tt__NetworkCapabilitiesExtension));
        memset(GetCap->Capabilities->Device->Network->Extension, 0, sizeof(struct tt__NetworkCapabilitiesExtension));
        GetCap->Capabilities->Device->Network->Extension->Dot11Configuration = (enum xsd__boolean_ *)soap_malloc(soap, sizeof(int));
        *GetCap->Capabilities->Device->Network->Extension->Dot11Configuration = _false;
        GetCap->Capabilities->Device->Network->Extension->Extension = NULL;

        GetCap->Capabilities->Device->System = (struct tt__SystemCapabilities*)soap_malloc(soap, sizeof(struct tt__SystemCapabilities));
        GetCap->Capabilities->Device->System->DiscoveryResolve = FALSE;
        GetCap->Capabilities->Device->System->DiscoveryBye = FALSE;
        GetCap->Capabilities->Device->System->RemoteDiscovery = FALSE;
        GetCap->Capabilities->Device->System->SystemBackup = FALSE;
        GetCap->Capabilities->Device->System->SystemLogging = TRUE;
        GetCap->Capabilities->Device->System->FirmwareUpgrade = TRUE;
        GetCap->Capabilities->Device->System->__sizeSupportedVersions = TRUE;
        GetCap->Capabilities->Device->System->SupportedVersions = (struct tt__OnvifVersion*)soap_malloc(soap, sizeof(struct tt__OnvifVersion));
        GetCap->Capabilities->Device->System->SupportedVersions->Major = 2;
        GetCap->Capabilities->Device->System->SupportedVersions->Minor = 1;
        GetCap->Capabilities->Device->System->Extension = (struct tt__SystemCapabilitiesExtension*)soap_malloc(soap, sizeof(struct tt__SystemCapabilitiesExtension));
        GetCap->Capabilities->Device->System->Extension->HttpSystemBackup = (enum xsd__boolean_ *)soap_malloc(soap, sizeof(int));
        GetCap->Capabilities->Device->System->Extension->__size = 0;
        GetCap->Capabilities->Device->System->Extension->__any = NULL;
        *GetCap->Capabilities->Device->System->Extension->HttpSystemBackup = _false;
        GetCap->Capabilities->Device->System->Extension->HttpFirmwareUpgrade = (enum xsd__boolean_ *)soap_malloc(soap, sizeof(int));
        *GetCap->Capabilities->Device->System->Extension->HttpFirmwareUpgrade = _true;
        GetCap->Capabilities->Device->System->Extension->HttpSystemLogging = (enum xsd__boolean_ *)soap_malloc(soap, sizeof(int));
        *GetCap->Capabilities->Device->System->Extension->HttpSystemLogging = _true;
        GetCap->Capabilities->Device->System->Extension->HttpSupportInformation = (enum xsd__boolean_ *)soap_malloc(soap, sizeof(int));
        *GetCap->Capabilities->Device->System->Extension->HttpSupportInformation = _true;
        GetCap->Capabilities->Device->System->Extension->Extension = NULL;

        GetCap->Capabilities->Device->IO = NULL;
#if 0
        GetCap->Capabilities->Device->IO = (struct tt__IOCapabilities*)soap_malloc(soap, sizeof(struct tt__IOCapabilities));
        GetCap->Capabilities->Device->IO->InputConnectors = (int *)soap_malloc(soap, sizeof(int));
        *GetCap->Capabilities->Device->IO->InputConnectors = 0;
        GetCap->Capabilities->Device->IO->RelayOutputs = (int *)soap_malloc(soap, sizeof(int));
        *GetCap->Capabilities->Device->IO->RelayOutputs = 1;
        GetCap->Capabilities->Device->IO->Extension = NULL;
#endif
        GetCap->Capabilities->Device->Security = (struct tt__SecurityCapabilities*)soap_malloc(soap, sizeof(struct tt__SecurityCapabilities));
        memset(GetCap->Capabilities->Device->Security, 0, sizeof(struct tt__SecurityCapabilities));
        GetCap->Capabilities->Device->Security->TLS1_x002e1 = FALSE;
        GetCap->Capabilities->Device->Security->TLS1_x002e2 = FALSE;
        GetCap->Capabilities->Device->Security->OnboardKeyGeneration = FALSE;
        GetCap->Capabilities->Device->Security->AccessPolicyConfig = FALSE;
        GetCap->Capabilities->Device->Security->X_x002e509Token = FALSE;
        GetCap->Capabilities->Device->Security->SAMLToken = FALSE;
        GetCap->Capabilities->Device->Security->KerberosToken = FALSE;
        GetCap->Capabilities->Device->Security->RELToken = FALSE;
        GetCap->Capabilities->Device->Security->Extension = NULL;
    }

    if((_Category == ALL) || (_Category == EVENTS))
    {
        GetCap->Capabilities->Events = (struct tt__EventCapabilities*)soap_malloc(soap, sizeof(struct tt__EventCapabilities));
        memset(GetCap->Capabilities->Events, 0, sizeof(struct tt__EventCapabilities));
        GetCap->Capabilities->Events->XAddr = (char *)soap_malloc(soap, sizeof(char) * LARGE_INFO_LENGTH);
        strcpy(GetCap->Capabilities->Events->XAddr, _IPv4Address);
        GetCap->Capabilities->Events->WSSubscriptionPolicySupport = TRUE;
        GetCap->Capabilities->Events->WSPullPointSupport = TRUE;
        GetCap->Capabilities->Events->WSPausableSubscriptionManagerInterfaceSupport = TRUE;
    }

    if((_Category == ALL) || (_Category == IMAGING))
    {
        GetCap->Capabilities->Imaging = (struct tt__ImagingCapabilities*)soap_malloc(soap, sizeof(struct tt__ImagingCapabilities));
        GetCap->Capabilities->Imaging->XAddr = (char *)soap_malloc(soap, sizeof(char) * LARGE_INFO_LENGTH);
        strcpy(GetCap->Capabilities->Imaging->XAddr, _IPv4Address);
    }

    if((_Category == ALL) || (_Category == MEDIA))
    {
        GetCap->Capabilities->Media = (struct tt__MediaCapabilities*)soap_malloc(soap, sizeof(struct tt__MediaCapabilities));
        memset(GetCap->Capabilities->Media, 0, sizeof(struct tt__MediaCapabilities));
        GetCap->Capabilities->Media->XAddr = (char *)soap_malloc(soap, sizeof(char) * LARGE_INFO_LENGTH);
        strcpy(GetCap->Capabilities->Media->XAddr, _IPv4Address);
        GetCap->Capabilities->Media->StreamingCapabilities = (struct tt__RealTimeStreamingCapabilities*)soap_malloc(soap, sizeof(struct tt__RealTimeStreamingCapabilities));
        GetCap->Capabilities->Media->StreamingCapabilities->RTPMulticast = (enum xsd__boolean_ *)soap_malloc(soap, sizeof(int));
        *GetCap->Capabilities->Media->StreamingCapabilities->RTPMulticast = _false;
        GetCap->Capabilities->Media->StreamingCapabilities->RTP_USCORETCP = (enum xsd__boolean_ *)soap_malloc(soap, sizeof(int));
        *GetCap->Capabilities->Media->StreamingCapabilities->RTP_USCORETCP = _true;
        GetCap->Capabilities->Media->StreamingCapabilities->RTP_USCORERTSP_USCORETCP = (enum xsd__boolean_ *)soap_malloc(soap, sizeof(int));
        *GetCap->Capabilities->Media->StreamingCapabilities->RTP_USCORERTSP_USCORETCP = _true;
        GetCap->Capabilities->Media->StreamingCapabilities->Extension = NULL;
        GetCap->Capabilities->Media->Extension = NULL;
    }

    if((_Category == ALL) || (_Category == PTZCAP))
    {
        GetCap->Capabilities->PTZ = (struct tt__PTZCapabilities*)soap_malloc(soap, sizeof(struct tt__PTZCapabilities));
        memset(GetCap->Capabilities->PTZ, 0, sizeof(struct tt__PTZCapabilities));
        GetCap->Capabilities->PTZ->XAddr = (char *)soap_malloc(soap, sizeof(char) * LARGE_INFO_LENGTH);
        strcpy(GetCap->Capabilities->PTZ->XAddr, _IPv4Address);
    }

    return SOAP_OK;
}

SOAP_FMAC5 int SOAP_FMAC6 __tds__GetServices(struct soap *soap, struct _tds__GetServices *tds__GetServices, struct _tds__GetServicesResponse *GetSevs)
{
#if 1
    char _IPAddr[INFO_LENGTH];
    NET_IPV4 ip;

    ip.int32 =  net_get_ifaddr(ETH_NAME);
    sprintf(_IPAddr, "http://%d.%d.%d.%d/onvif/services", ip.str[0], ip.str[1], ip.str[2], ip.str[3]);

    if(!tds__GetServices->IncludeCapability)
    {
        GetSevs->__sizeService = 7;
        GetSevs->Service = (struct tds__Service *)soap_malloc(soap, GetSevs->__sizeService * sizeof(struct tds__Service));
        memset(GetSevs->Service, 0, GetSevs->__sizeService * sizeof(struct tds__Service));
        GetSevs->Service[0].XAddr = (char *)soap_malloc(soap, sizeof(char) * INFO_LENGTH);
        GetSevs->Service[0].Namespace = (char *)soap_malloc(soap, sizeof(char) * INFO_LENGTH);
        strcpy(GetSevs->Service[0].Namespace, "http://www.onvif.org/ver10/events/wsdl");
        strcpy(GetSevs->Service[0].XAddr, _IPAddr);
        GetSevs->Service[0].Capabilities = NULL;
        GetSevs->Service[0].Version = (struct tt__OnvifVersion *)soap_malloc(soap, sizeof(struct tt__OnvifVersion));
        GetSevs->Service[0].Version->Major = 0;
        GetSevs->Service[0].Version->Minor = 3;
        GetSevs->Service[0].__any = NULL;
        GetSevs->Service[0].__size = 0;
        GetSevs->Service[0].__anyAttribute = NULL;

        GetSevs->Service[1].XAddr = (char *)soap_malloc(soap, sizeof(char) * INFO_LENGTH);
        GetSevs->Service[1].Namespace = (char *)soap_malloc(soap, sizeof(char) * INFO_LENGTH);
        strcpy(GetSevs->Service[1].Namespace, "http://www.onvif.org/ver10/receiver/wsdl");
        strcpy(GetSevs->Service[1].XAddr, _IPAddr);
        GetSevs->Service[1].Capabilities = NULL;
        GetSevs->Service[1].Version = (struct tt__OnvifVersion *)soap_malloc(soap, sizeof(struct tt__OnvifVersion));
        GetSevs->Service[1].Version->Major = 0;
        GetSevs->Service[1].Version->Minor = 3;
        GetSevs->Service[1].__any = NULL;
        GetSevs->Service[1].__size = 0;
        GetSevs->Service[1].__anyAttribute = NULL;

        GetSevs->Service[2].XAddr = (char *)soap_malloc(soap, sizeof(char) * INFO_LENGTH);
        GetSevs->Service[2].Namespace = (char *)soap_malloc(soap, sizeof(char) * INFO_LENGTH);
        strcpy(GetSevs->Service[2].Namespace, "http://www.onvif.org/ver10/device/wsdl");
        strcpy(GetSevs->Service[2].XAddr, _IPAddr);
        GetSevs->Service[2].Capabilities = NULL;
        GetSevs->Service[2].Version = (struct tt__OnvifVersion *)soap_malloc(soap, sizeof(struct tt__OnvifVersion));
        GetSevs->Service[2].Version->Major = 0;
        GetSevs->Service[2].Version->Minor = 3;
        GetSevs->Service[2].__any = NULL;
        GetSevs->Service[2].__size = 0;
        GetSevs->Service[2].__anyAttribute = NULL;

        GetSevs->Service[3].XAddr = (char *)soap_malloc(soap, sizeof(char) * INFO_LENGTH);
        GetSevs->Service[3].Namespace = (char *)soap_malloc(soap, sizeof(char) * INFO_LENGTH);
        strcpy(GetSevs->Service[3].Namespace, "http://www.onvif.org/ver10/recording/wsdl");
        strcpy(GetSevs->Service[3].XAddr, _IPAddr);
        GetSevs->Service[3].Capabilities = NULL;
        GetSevs->Service[3].Version = (struct tt__OnvifVersion *)soap_malloc(soap, sizeof(struct tt__OnvifVersion));
        GetSevs->Service[3].Version->Major = 0;
        GetSevs->Service[3].Version->Minor = 3;
        GetSevs->Service[3].__any = NULL;
        GetSevs->Service[3].__size = 0;
        GetSevs->Service[3].__anyAttribute = NULL;

        GetSevs->Service[4].XAddr = (char *)soap_malloc(soap, sizeof(char) * INFO_LENGTH);
        GetSevs->Service[4].Namespace = (char *)soap_malloc(soap, sizeof(char) * INFO_LENGTH);
        strcpy(GetSevs->Service[4].Namespace, "http://www.onvif.org/ver10/search/wsdl");
        strcpy(GetSevs->Service[4].XAddr, _IPAddr);
        GetSevs->Service[4].Capabilities = NULL;
        GetSevs->Service[4].Version = (struct tt__OnvifVersion *)soap_malloc(soap, sizeof(struct tt__OnvifVersion));
        GetSevs->Service[4].Version->Major = 0;
        GetSevs->Service[4].Version->Minor = 3;
        GetSevs->Service[4].__any = NULL;
        GetSevs->Service[4].__size = 0;
        GetSevs->Service[4].__anyAttribute = NULL;

        GetSevs->Service[5].XAddr = (char *)soap_malloc(soap, sizeof(char) * INFO_LENGTH);
        GetSevs->Service[5].Namespace = (char *)soap_malloc(soap, sizeof(char) * INFO_LENGTH);
        strcpy(GetSevs->Service[5].Namespace, "http://www.onvif.org/ver10/replay/wsdl");
        strcpy(GetSevs->Service[5].XAddr, _IPAddr);
        GetSevs->Service[5].Capabilities = NULL;
        GetSevs->Service[5].Version = (struct tt__OnvifVersion *)soap_malloc(soap, sizeof(struct tt__OnvifVersion));
        GetSevs->Service[5].Version->Major = 0;
        GetSevs->Service[5].Version->Minor = 3;
        GetSevs->Service[5].__any = NULL;
        GetSevs->Service[5].__size = 0;
        GetSevs->Service[5].__anyAttribute = NULL;
        
        GetSevs->Service[6].XAddr = (char *)soap_malloc(soap, sizeof(char) * INFO_LENGTH);
        GetSevs->Service[6].Namespace = (char *)soap_malloc(soap, sizeof(char) * INFO_LENGTH);
        strcpy(GetSevs->Service[6].Namespace, "http://www.onvif.org/ver10/media/wsdl");
        strcpy(GetSevs->Service[6].XAddr, _IPAddr);
        GetSevs->Service[6].Capabilities = NULL;
        GetSevs->Service[6].Version = (struct tt__OnvifVersion *)soap_malloc(soap, sizeof(struct tt__OnvifVersion));
        GetSevs->Service[6].Version->Major = 0;
        GetSevs->Service[6].Version->Minor = 3;
        GetSevs->Service[6].__any = NULL;
        GetSevs->Service[6].__size = 0;
        GetSevs->Service[6].__anyAttribute = NULL;        
    }
    else
    { 
        GetSevs->__sizeService = 7;
        GetSevs->Service = (struct tds__Service *)soap_malloc(soap, (GetSevs->__sizeService) * sizeof(struct tds__Service));
        memset(GetSevs->Service, 0, (GetSevs->__sizeService) * sizeof(struct tds__Service));
        GetSevs->Service[0].XAddr = (char *)soap_malloc(soap, sizeof(char) * INFO_LENGTH);
        GetSevs->Service[0].Namespace = (char *)soap_malloc(soap, sizeof(char) * INFO_LENGTH);
        strcpy(GetSevs->Service[0].XAddr, _IPAddr);
        strcpy(GetSevs->Service[0].Namespace, "http://www.onvif.org/ver10/event/wsdl");
        GetSevs->Service[0].Capabilities = NULL;
        GetSevs->Service[0].Version = (struct tt__OnvifVersion *)soap_malloc(soap, sizeof(struct tt__OnvifVersion));
        GetSevs->Service[0].Version->Major = 0;
        GetSevs->Service[0].Version->Minor = 3;
        GetSevs->Service[0].__size = 1;
        //GetSevs->Service[0].__any = NULL;
        GetSevs->Service[0].__any = (char **)soap_malloc(soap, sizeof(char *));
        GetSevs->Service[0].__any[0] = (char *)soap_malloc(soap, sizeof(char) * INFO_LENGTH);
        strcpy(GetSevs->Service[0].__any[0], "tev:GetServiceCapabilities");
        GetSevs->Service[0].__anyAttribute = NULL;
        
        GetSevs->Service[1].XAddr = (char *)soap_malloc(soap, sizeof(char) * INFO_LENGTH);
        GetSevs->Service[1].Namespace = (char *)soap_malloc(soap, sizeof(char) * INFO_LENGTH);
        strcpy(GetSevs->Service[1].Namespace, "http://www.onvif.org/ver10/receiver/wsdl");
        strcpy(GetSevs->Service[1].XAddr, _IPAddr);
        GetSevs->Service[1].Capabilities = NULL;
        GetSevs->Service[1].Version = (struct tt__OnvifVersion *)soap_malloc(soap, sizeof(struct tt__OnvifVersion));
        GetSevs->Service[1].Version->Major = 0;
        GetSevs->Service[1].Version->Minor = 3;
        //GetSevs->Service[1].__any = NULL;
        GetSevs->Service[1].__any = (char **)soap_malloc(soap, sizeof(char *));
        GetSevs->Service[1].__any[0] = (char *)soap_malloc(soap, sizeof(char) * INFO_LENGTH);
        strcpy(GetSevs->Service[1].__any[0], "trv:GetServiceCapabilities");
        GetSevs->Service[1].__size = 1;
        GetSevs->Service[1].__anyAttribute = NULL;
        
        GetSevs->Service[2].XAddr = (char *)soap_malloc(soap, sizeof(char) * INFO_LENGTH);
        GetSevs->Service[2].Namespace = (char *)soap_malloc(soap, sizeof(char) * INFO_LENGTH);
        strcpy(GetSevs->Service[2].Namespace, "http://www.onvif.org/ver10/device/wsdl");
        strcpy(GetSevs->Service[2].XAddr, _IPAddr);
        GetSevs->Service[2].Capabilities = NULL;
        GetSevs->Service[2].Version = (struct tt__OnvifVersion *)soap_malloc(soap, sizeof(struct tt__OnvifVersion));
        GetSevs->Service[2].Version->Major = 0;
        GetSevs->Service[2].Version->Minor = 3;
        //GetSevs->Service[2].__any = NULL;
        GetSevs->Service[2].__any = (char **)soap_malloc(soap, sizeof(char *));
        GetSevs->Service[2].__any[0] = (char *)soap_malloc(soap, sizeof(char) * INFO_LENGTH);
        strcpy(GetSevs->Service[2].__any[0], "tls:GetServiceCapabilities");
        GetSevs->Service[2].__size = 1;
        GetSevs->Service[2].__anyAttribute = NULL;
        
        GetSevs->Service[3].XAddr = (char *)soap_malloc(soap, sizeof(char) * INFO_LENGTH);
        GetSevs->Service[3].Namespace = (char *)soap_malloc(soap, sizeof(char) * INFO_LENGTH);
        strcpy(GetSevs->Service[3].Namespace, "http://www.onvif.org/ver10/recording/wsdl");
        strcpy(GetSevs->Service[3].XAddr, _IPAddr);
        GetSevs->Service[3].Capabilities = NULL;
        GetSevs->Service[3].Version = (struct tt__OnvifVersion *)soap_malloc(soap, sizeof(struct tt__OnvifVersion));
        GetSevs->Service[3].Version->Major = 0;
        GetSevs->Service[3].Version->Minor = 3;
        //GetSevs->Service[3].__any = NULL;
        GetSevs->Service[3].__any = (char **)soap_malloc(soap, sizeof(char *));
        GetSevs->Service[3].__any[0] = (char *)soap_malloc(soap, sizeof(char) * INFO_LENGTH);
        strcpy(GetSevs->Service[3].__any[0], "trc:GetServiceCapabilities");
        GetSevs->Service[3].__size = 1;
        GetSevs->Service[3].__anyAttribute = NULL;
        
        GetSevs->Service[4].XAddr = (char *)soap_malloc(soap, sizeof(char) * INFO_LENGTH);
        GetSevs->Service[4].Namespace = (char *)soap_malloc(soap, sizeof(char) * INFO_LENGTH);
        strcpy(GetSevs->Service[4].Namespace, "http://www.onvif.org/ver10/search/wsdl");
        strcpy(GetSevs->Service[4].XAddr, _IPAddr);
        GetSevs->Service[4].Capabilities = NULL;
        GetSevs->Service[4].Version = (struct tt__OnvifVersion *)soap_malloc(soap, sizeof(struct tt__OnvifVersion));
        GetSevs->Service[4].Version->Major = 0;
        GetSevs->Service[4].Version->Minor = 3;
        //GetSevs->Service[4].__any = NULL;
        GetSevs->Service[4].__any = (char **)soap_malloc(soap, sizeof(char *));
        GetSevs->Service[4].__any[0] = (char *)soap_malloc(soap, sizeof(char) * INFO_LENGTH);
        strcpy(GetSevs->Service[4].__any[0], "tse:GetServiceCapabilities");
        GetSevs->Service[4].__size = 1;
        GetSevs->Service[4].__anyAttribute = NULL;

        GetSevs->Service[5].XAddr = (char *)soap_malloc(soap, sizeof(char) * INFO_LENGTH);
        GetSevs->Service[5].Namespace = (char *)soap_malloc(soap, sizeof(char) * INFO_LENGTH);
        strcpy(GetSevs->Service[5].Namespace, "http://www.onvif.org/ver10/replay/wsdl");
        strcpy(GetSevs->Service[5].XAddr, _IPAddr);
        GetSevs->Service[5].Capabilities = NULL;
        GetSevs->Service[5].Version = (struct tt__OnvifVersion *)soap_malloc(soap, sizeof(struct tt__OnvifVersion));
        GetSevs->Service[5].Version->Major = 0;
        GetSevs->Service[5].Version->Minor = 3;
        //GetSevs->Service[5].__any = NULL;
        GetSevs->Service[5].__any = (char **)soap_malloc(soap, sizeof(char *));
        GetSevs->Service[5].__any[0] = (char *)soap_malloc(soap, sizeof(char) * INFO_LENGTH);
        strcpy(GetSevs->Service[5].__any[0], "trp:GetServiceCapabilities");
        GetSevs->Service[5].__size = 1;
        GetSevs->Service[5].__anyAttribute = NULL;

        GetSevs->Service[6].XAddr = (char *)soap_malloc(soap, sizeof(char) * INFO_LENGTH);
        GetSevs->Service[6].Namespace = (char *)soap_malloc(soap, sizeof(char) * INFO_LENGTH);
        strcpy(GetSevs->Service[6].Namespace, "http://www.onvif.org/ver10/media/wsdl");
        strcpy(GetSevs->Service[6].XAddr, _IPAddr);
        GetSevs->Service[6].Capabilities = NULL;
        GetSevs->Service[6].Version = (struct tt__OnvifVersion *)soap_malloc(soap, sizeof(struct tt__OnvifVersion));
        GetSevs->Service[6].Version->Major = 0;
        GetSevs->Service[6].Version->Minor = 3;
        GetSevs->Service[6].__any = (char **)soap_malloc(soap, sizeof(char *));
        GetSevs->Service[6].__any[0] = (char *)soap_malloc(soap, sizeof(char) * INFO_LENGTH);
        strcpy(GetSevs->Service[6].__any[0], "trp:GetServiceCapabilities");
        GetSevs->Service[6].__size = 1;
        GetSevs->Service[6].__anyAttribute = NULL;         
    }
#endif
    return SOAP_OK;
}

SOAP_FMAC5 int SOAP_FMAC6 __tds__GetServiceCapabilities(struct soap *soap, struct _tds__GetServiceCapabilities *tds__GetServiceCapabilities, struct _tds__GetServiceCapabilitiesResponse *tds__GetServiceCapabilitiesResponse)
{
	tds__GetServiceCapabilitiesResponse->Capabilities = (struct tds__DeviceServiceCapabilities *)soap_malloc(soap, sizeof(struct tds__DeviceServiceCapabilities));
	memset(tds__GetServiceCapabilitiesResponse->Capabilities, 0, sizeof(struct tds__DeviceServiceCapabilities));
	/* NETWORK */
	tds__GetServiceCapabilitiesResponse->Capabilities->Network = (struct tds__NetworkCapabilities *)soap_malloc(soap, sizeof(struct tds__NetworkCapabilities));
	memset(tds__GetServiceCapabilitiesResponse->Capabilities->Network, 0, sizeof(struct tds__NetworkCapabilities));
	tds__GetServiceCapabilitiesResponse->Capabilities->Network->IPFilter = (enum xsd__boolean_ *)soap_malloc(soap, sizeof(int));
	tds__GetServiceCapabilitiesResponse->Capabilities->Network->ZeroConfiguration = (enum xsd__boolean_ *)soap_malloc(soap, sizeof(int));
	tds__GetServiceCapabilitiesResponse->Capabilities->Network->IPVersion6 = (enum xsd__boolean_ *)soap_malloc(soap, sizeof(int));
	tds__GetServiceCapabilitiesResponse->Capabilities->Network->DynDNS = (enum xsd__boolean_ *)soap_malloc(soap, sizeof(int));
	tds__GetServiceCapabilitiesResponse->Capabilities->Network->Dot11Configuration = (enum xsd__boolean_ *)soap_malloc(soap, sizeof(int));
	tds__GetServiceCapabilitiesResponse->Capabilities->Network->HostnameFromDHCP= (enum xsd__boolean_ *)soap_malloc(soap, sizeof(int));
	tds__GetServiceCapabilitiesResponse->Capabilities->Network->NTP= (int *)soap_malloc(soap, sizeof(int));
	*tds__GetServiceCapabilitiesResponse->Capabilities->Network->IPFilter = FALSE;
	*tds__GetServiceCapabilitiesResponse->Capabilities->Network->ZeroConfiguration = FALSE;
	*tds__GetServiceCapabilitiesResponse->Capabilities->Network->IPVersion6 = FALSE;
	*tds__GetServiceCapabilitiesResponse->Capabilities->Network->DynDNS = FALSE;
	*tds__GetServiceCapabilitiesResponse->Capabilities->Network->Dot11Configuration = FALSE;
	*tds__GetServiceCapabilitiesResponse->Capabilities->Network->HostnameFromDHCP= FALSE;
	*tds__GetServiceCapabilitiesResponse->Capabilities->Network->NTP= 3;
	tds__GetServiceCapabilitiesResponse->Capabilities->Network->__anyAttribute = NULL;

	/* SYSTEM */
	tds__GetServiceCapabilitiesResponse->Capabilities->System = (struct tds__SystemCapabilities *)soap_malloc(soap, sizeof(struct tds__SystemCapabilities));
	memset(tds__GetServiceCapabilitiesResponse->Capabilities->System, 0, sizeof(struct tds__SystemCapabilities));
	tds__GetServiceCapabilitiesResponse->Capabilities->System->DiscoveryResolve = (enum xsd__boolean_ *)soap_malloc(soap, sizeof(int));
	tds__GetServiceCapabilitiesResponse->Capabilities->System->DiscoveryBye = (enum xsd__boolean_ *)soap_malloc(soap, sizeof(int));
	tds__GetServiceCapabilitiesResponse->Capabilities->System->RemoteDiscovery = (enum xsd__boolean_ *)soap_malloc(soap, sizeof(int));
	tds__GetServiceCapabilitiesResponse->Capabilities->System->SystemBackup = (enum xsd__boolean_ *)soap_malloc(soap, sizeof(int));
	tds__GetServiceCapabilitiesResponse->Capabilities->System->FirmwareUpgrade = (enum xsd__boolean_ *)soap_malloc(soap, sizeof(int));
	tds__GetServiceCapabilitiesResponse->Capabilities->System->SystemLogging = (enum xsd__boolean_ *)soap_malloc(soap, sizeof(int));
	tds__GetServiceCapabilitiesResponse->Capabilities->System->HttpSystemBackup = (enum xsd__boolean_ *)soap_malloc(soap, sizeof(int));
	tds__GetServiceCapabilitiesResponse->Capabilities->System->HttpFirmwareUpgrade = (enum xsd__boolean_ *)soap_malloc(soap, sizeof(int));
	tds__GetServiceCapabilitiesResponse->Capabilities->System->HttpSystemLogging = (enum xsd__boolean_ *)soap_malloc(soap, sizeof(int));
	tds__GetServiceCapabilitiesResponse->Capabilities->System->HttpSupportInformation = (enum xsd__boolean_ *)soap_malloc(soap, sizeof(int));
    *tds__GetServiceCapabilitiesResponse->Capabilities->System->DiscoveryResolve = FALSE;
    *tds__GetServiceCapabilitiesResponse->Capabilities->System->DiscoveryBye = FALSE;
    *tds__GetServiceCapabilitiesResponse->Capabilities->System->RemoteDiscovery = FALSE;
    *tds__GetServiceCapabilitiesResponse->Capabilities->System->SystemBackup = FALSE;
    *tds__GetServiceCapabilitiesResponse->Capabilities->System->FirmwareUpgrade = TRUE;
    *tds__GetServiceCapabilitiesResponse->Capabilities->System->SystemLogging = TRUE;
    *tds__GetServiceCapabilitiesResponse->Capabilities->System->HttpSystemBackup = FALSE;
    *tds__GetServiceCapabilitiesResponse->Capabilities->System->HttpFirmwareUpgrade = TRUE;
    *tds__GetServiceCapabilitiesResponse->Capabilities->System->HttpSystemLogging = TRUE;
    *tds__GetServiceCapabilitiesResponse->Capabilities->System->HttpSupportInformation = TRUE;
	tds__GetServiceCapabilitiesResponse->Capabilities->System->__anyAttribute = NULL;

	/* SECURITY */
	tds__GetServiceCapabilitiesResponse->Capabilities->Security = (struct tds__SecurityCapabilities *)soap_malloc(soap, sizeof(struct tds__SecurityCapabilities));
	memset(tds__GetServiceCapabilitiesResponse->Capabilities->Security, 0, sizeof(struct tds__SecurityCapabilities));
	tds__GetServiceCapabilitiesResponse->Capabilities->Security->TLS1_x002e0 = (enum xsd__boolean_ *)soap_malloc(soap, sizeof(int));
	tds__GetServiceCapabilitiesResponse->Capabilities->Security->TLS1_x002e1 = (enum xsd__boolean_ *)soap_malloc(soap, sizeof(int));
	tds__GetServiceCapabilitiesResponse->Capabilities->Security->TLS1_x002e2 = (enum xsd__boolean_ *)soap_malloc(soap, sizeof(int));
	tds__GetServiceCapabilitiesResponse->Capabilities->Security->OnboardKeyGeneration = (enum xsd__boolean_ *)soap_malloc(soap, sizeof(int));
	tds__GetServiceCapabilitiesResponse->Capabilities->Security->AccessPolicyConfig = (enum xsd__boolean_ *)soap_malloc(soap, sizeof(int));
	tds__GetServiceCapabilitiesResponse->Capabilities->Security->Dot1X = (enum xsd__boolean_ *)soap_malloc(soap, sizeof(int));
	tds__GetServiceCapabilitiesResponse->Capabilities->Security->RemoteUserHandling = (enum xsd__boolean_ *)soap_malloc(soap, sizeof(int));
	tds__GetServiceCapabilitiesResponse->Capabilities->Security->X_x002e509Token = (enum xsd__boolean_ *)soap_malloc(soap, sizeof(int));
	tds__GetServiceCapabilitiesResponse->Capabilities->Security->SAMLToken = (enum xsd__boolean_ *)soap_malloc(soap, sizeof(int));
	tds__GetServiceCapabilitiesResponse->Capabilities->Security->KerberosToken = (enum xsd__boolean_ *)soap_malloc(soap, sizeof(int));
	tds__GetServiceCapabilitiesResponse->Capabilities->Security->UsernameToken = (enum xsd__boolean_ *)soap_malloc(soap, sizeof(int));
	tds__GetServiceCapabilitiesResponse->Capabilities->Security->HttpDigest = (enum xsd__boolean_ *)soap_malloc(soap, sizeof(int));
	tds__GetServiceCapabilitiesResponse->Capabilities->Security->RELToken = (enum xsd__boolean_ *)soap_malloc(soap, sizeof(int));
    *tds__GetServiceCapabilitiesResponse->Capabilities->Security->TLS1_x002e0 = _false;
    *tds__GetServiceCapabilitiesResponse->Capabilities->Security->TLS1_x002e1 = _false;
    *tds__GetServiceCapabilitiesResponse->Capabilities->Security->TLS1_x002e2 = _false;
    *tds__GetServiceCapabilitiesResponse->Capabilities->Security->OnboardKeyGeneration = _false;
    *tds__GetServiceCapabilitiesResponse->Capabilities->Security->AccessPolicyConfig = _false;
    *tds__GetServiceCapabilitiesResponse->Capabilities->Security->Dot1X = _false;
    *tds__GetServiceCapabilitiesResponse->Capabilities->Security->RemoteUserHandling = _false;
    *tds__GetServiceCapabilitiesResponse->Capabilities->Security->X_x002e509Token = _false;
    *tds__GetServiceCapabilitiesResponse->Capabilities->Security->SAMLToken = _false;
    *tds__GetServiceCapabilitiesResponse->Capabilities->Security->KerberosToken = _false;
    *tds__GetServiceCapabilitiesResponse->Capabilities->Security->UsernameToken = _false;
    *tds__GetServiceCapabilitiesResponse->Capabilities->Security->HttpDigest = _false;
    *tds__GetServiceCapabilitiesResponse->Capabilities->Security->RELToken = _false;
	tds__GetServiceCapabilitiesResponse->Capabilities->Security->SupportedEAPMethods = NULL;
	tds__GetServiceCapabilitiesResponse->Capabilities->Security->__anyAttribute = NULL;
    return SOAP_OK;
}

SOAP_FMAC5 int SOAP_FMAC6 __tds__GetScopes(struct soap *soap, struct _tds__GetScopes *input, struct _tds__GetScopesResponse *GetScopes)
{
    int size = 5;
    int i = 0;

    GetScopes->__sizeScopes = size;

    GetScopes->Scopes = (struct tt__Scope*)soap_malloc(soap, (size *sizeof(struct tt__Scope)));
    memset(GetScopes->Scopes, 0, sizeof(struct tt__Scope));

    GetScopes->Scopes[i].ScopeDef = tt__ScopeDefinition__Fixed;
    GetScopes->Scopes[i].ScopeItem = (char *)soap_malloc(soap, (sizeof(char) * INFO_LENGTH));
    strcpy(GetScopes->Scopes[i].ScopeItem, "onvif://www.onvif.org/type/Network_Video_Transmitter");
    i++;

    GetScopes->Scopes[i].ScopeDef = tt__ScopeDefinition__Fixed;
    GetScopes->Scopes[i].ScopeItem = (char *)soap_malloc(soap, (sizeof(char) * INFO_LENGTH));
    strcpy(GetScopes->Scopes[i].ScopeItem, "onvif://www.onvif.org/Profile/Streaming");
    i++;

    GetScopes->Scopes[i].ScopeDef = tt__ScopeDefinition__Fixed;
    GetScopes->Scopes[i].ScopeItem = (char *)soap_malloc(soap, (sizeof(char) * INFO_LENGTH));
    strcpy(GetScopes->Scopes[i].ScopeItem, "onvif://www.onvif.org/name/github.com/openhisilicon");
    i++;

    GetScopes->Scopes[i].ScopeDef = tt__ScopeDefinition__Fixed;
    GetScopes->Scopes[i].ScopeItem = (char *)soap_malloc(soap, (sizeof(char) * INFO_LENGTH));
    snprintf(GetScopes->Scopes[i].ScopeItem, INFO_LENGTH, "onvif://www.onvif.org/hardware/%s", g_nvt->t.dev->hwid);
    i++;

    GetScopes->Scopes[i].ScopeDef = tt__ScopeDefinition__Fixed;
    GetScopes->Scopes[i].ScopeItem = (char *)soap_malloc(soap, (sizeof(char) * INFO_LENGTH));
    strcpy(GetScopes->Scopes[i].ScopeItem, "onvif://www.onvif.org/location/SHENZHEN/CHINA");

    return SOAP_OK;
}

SOAP_FMAC5 int SOAP_FMAC6 __tds__GetDNS(struct soap *soap, struct _tds__GetDNS *input, struct _tds__GetDNSResponse *output)
{
#if 1
    int ret;
    net_info_t *info;

    info = (net_info_t *)calloc(1, sizeof(net_info_t));
    memset(info, 0, sizeof(net_info_t));

    ret = get_dev_netinfo(info);

    if(ret < 0)
    {
        fprintf(stderr, "[%s][%d] Get device network information error!\n", __FUNCTION__, __LINE__);
        return SOAP_FAULT;
    }

    output->DNSInformation = (struct tt__DNSInformation *)soap_malloc(soap, sizeof(struct tt__DNSInformation));
    
    output->DNSInformation->FromDHCP = info->dhcp_status;

    if(output->DNSInformation->FromDHCP == _false)
    {
        
        output->DNSInformation->__sizeDNSManual = 2;
        output->DNSInformation->DNSManual = (struct tt__IPAddress *)soap_malloc(soap, 2 * sizeof(struct tt__IPAddress));
        output->DNSInformation->DNSManual[0].Type = tt__IPType__IPv4;
        output->DNSInformation->DNSManual[0].IPv4Address = (char **)soap_malloc(soap, sizeof(char *));
        if(info->dns1)
        {
          output->DNSInformation->DNSManual[0].IPv4Address[0] = (char *)soap_malloc(soap, sizeof(char) * INFO_LENGTH); 
          strcpy(output->DNSInformation->DNSManual[0].IPv4Address[0], info->dns1);
        }
        else
          output->DNSInformation->DNSManual->IPv4Address[0] = NULL;
          
        output->DNSInformation->DNSManual[0].IPv6Address = NULL;
        
        output->DNSInformation->DNSManual[1].Type = tt__IPType__IPv4;
        output->DNSInformation->DNSManual[1].IPv4Address = (char **)soap_malloc(soap, sizeof(char *));
        if(info->dns2)
        {
          output->DNSInformation->DNSManual[1].IPv4Address[0] = (char *)soap_malloc(soap, sizeof(char) * INFO_LENGTH); 
          strcpy(output->DNSInformation->DNSManual[1].IPv4Address[0], info->dns2);
        }
        else
          output->DNSInformation->DNSManual[1].IPv4Address[0] = NULL;
          
        output->DNSInformation->DNSManual[1].IPv6Address = NULL;
        output->DNSInformation->DNSFromDHCP = NULL;
        output->DNSInformation->__sizeDNSFromDHCP = 0;
    }
    else if(output->DNSInformation->FromDHCP == _true)
    {
        output->DNSInformation->__sizeDNSFromDHCP = 1;
        output->DNSInformation->DNSManual = NULL;
        output->DNSInformation->__sizeDNSManual = 0;
        output->DNSInformation->DNSFromDHCP = (struct tt__IPAddress *)soap_malloc(soap, sizeof(struct tt__IPAddress));
        output->DNSInformation->DNSFromDHCP->IPv4Address = (char **)soap_malloc(soap, sizeof(char *));
        if(info->dns1)
        {
          output->DNSInformation->DNSFromDHCP->IPv4Address[0] = (char *)soap_malloc(soap, sizeof(char) * INFO_LENGTH); 
          strcpy(output->DNSInformation->DNSFromDHCP->IPv4Address[0], info->dns1);
        }
        else
          output->DNSInformation->DNSFromDHCP->IPv4Address[0] = NULL;
          
        output->DNSInformation->DNSFromDHCP->IPv6Address = NULL;
        output->DNSInformation->DNSFromDHCP->Type = tt__IPType__IPv4;
    }

    output->DNSInformation->Extension = NULL;
    output->DNSInformation->__sizeSearchDomain = 0;
    output->DNSInformation->SearchDomain = NULL;
    output->DNSInformation->__anyAttribute = NULL;
    if(info)
    {
        free(info);
        info = NULL;
    }
#endif
    return SOAP_OK;
}

int check_ipaddress (char *str) 
{
	int segs = 0;   /* Segment count. */     
	int chcnt = 0;  /* Character count within segment. */     
	int accum = 0;  /* Accumulator for segment. */      
	/* Catch NULL pointer. */      
	if (str == NULL) return 0;      
	/* Process every character in string. */      
	while (*str != '\0') 
	{         
		/* Segment changeover. */          
		if (*str == '.') 
		{             
			/* Must have some digits in segment. */              
			if (chcnt == 0) return 0;              
			/* Limit number of segments. */              
			if (++segs == 4) return 0;              
			/* Reset segment values and restart loop. */              
			chcnt = accum = 0;             
			str++;             
			continue;         
		}  

		/* Check numeric. */          
		if ((*str < '0') || (*str > '9')) return 0;
		/* Accumulate and check segment. */          
		if ((accum = accum * 10 + *str - '0') > 255) return 0;
		/* Advance other segment specific stuff and continue loop. */          
		chcnt++;         
		str++;     
	}      
	/* Check enough segments and enough characters in last segment. */      
	if (segs != 3) return 0;      
	if (chcnt == 0) return 0;      
	/* Address okay. */      
	return 1; 
} 

SOAP_FMAC5 int SOAP_FMAC6 __tds__SetDNS(struct soap *soap, struct _tds__SetDNS *input, struct _tds__SetDNSResponse *output)
{
#if 1
    int ret;
	int dhcp_status = 0;
	int value = input->FromDHCP;
	char _IPv4Address[SMALL_INFO_LENGTH];

    net_info_t *info = (net_info_t *)calloc(1, sizeof(net_info_t));
    memset(info, 0, sizeof(net_info_t)); 

    get_dev_netinfo(info);

	if (dhcp_status != value) 
    {
		if(value == _true) 
        {
			return SOAP_OK;
		}
	}

	if(input->DNSManual == NULL)
	{
		return SOAP_OK;
	}

    if(input->__sizeDNSManual == 1)
    {
        if(check_ipaddress(input->DNSManual->IPv4Address[0]) == 0)
        {
		    return SOAP_FAULT;            
        }
        strcpy(info->dns1, input->DNSManual->IPv4Address[0]); 
        
    }
    else if(input->__sizeDNSManual == 2)
    {

        if((check_ipaddress(input->DNSManual[0].IPv4Address[0]) == 0) 
                || (check_ipaddress(input->DNSManual[1].IPv4Address[0]) == 0))
        {
		    return SOAP_FAULT;            
        }        
        strcpy(info->dns1, input->DNSManual[0].IPv4Address[0]); 
        strcpy(info->dns2, input->DNSManual[1].IPv4Address[0]);         
    }
    ret = set_dev_netinfo(info);
    if(info)
    {
        free(info);
        info = NULL;
    }	
#endif
    return SOAP_OK;
}

SOAP_FMAC5 int SOAP_FMAC6 __tds__GetNetworkInterfaces(struct soap *soap, struct _tds__GetNetworkInterfaces *tds__GetNetworkInterfaces, struct _tds__GetNetworkInterfacesResponse *GetNetwork)
{
    unsigned char mac[MACH_ADDR_LENGTH];
    char _mac[LARGE_INFO_LENGTH];
    char _IPAddr[LARGE_INFO_LENGTH];
    NET_IPV4 ip;
    int ret;
    net_info_t *info;

    info = (net_info_t *)calloc(1, sizeof(net_info_t));
    memset(info, 0, sizeof(net_info_t));

    ret = get_dev_netinfo(info);

    if(ret < 0)
    {
        fprintf(stderr, "[%s][%d] Get device network information error!\n", __FUNCTION__, __LINE__);
        return SOAP_FAULT;
    }
    
    net_get_hwaddr(ETH_NAME, mac);
    ip.int32 =  net_get_ifaddr(ETH_NAME);

    sprintf(_mac, "%02X:%02X:%02X:%02X:%02X:%02X", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
    sprintf(_IPAddr, "%d.%d.%d.%d", ip.str[0], ip.str[1], ip.str[2], ip.str[3]);

    GetNetwork->__sizeNetworkInterfaces = 1;
    GetNetwork->NetworkInterfaces = (struct tt__NetworkInterface *)soap_malloc(soap, sizeof(struct tt__NetworkInterface));

    GetNetwork->NetworkInterfaces->token = (char *)soap_malloc(soap, sizeof(char) * LARGE_INFO_LENGTH);
    strcpy(GetNetwork->NetworkInterfaces->token, ETH_NAME);
    GetNetwork->NetworkInterfaces->Enabled = TRUE;

    GetNetwork->NetworkInterfaces->Info = (struct tt__NetworkInterfaceInfo *)soap_malloc(soap, sizeof(struct tt__NetworkInterfaceInfo));
    GetNetwork->NetworkInterfaces->Info->Name = (char *)soap_malloc(soap, sizeof(char) * LARGE_INFO_LENGTH);
    strcpy(GetNetwork->NetworkInterfaces->Info->Name, ETH_NAME);
    GetNetwork->NetworkInterfaces->Info->HwAddress = (char *)soap_malloc(soap, sizeof(char) * LARGE_INFO_LENGTH);
    strcpy(GetNetwork->NetworkInterfaces->Info->HwAddress,_mac);
    GetNetwork->NetworkInterfaces->Info->MTU = (int *)soap_malloc(soap, sizeof(int));
    GetNetwork->NetworkInterfaces->Info->MTU[0] = 1500;

    GetNetwork->NetworkInterfaces->Link = NULL;
    GetNetwork->NetworkInterfaces->IPv4 = (struct tt__IPv4NetworkInterface *)soap_malloc(soap, sizeof(struct tt__IPv4NetworkInterface));
    GetNetwork->NetworkInterfaces->IPv4->Enabled = TRUE;
    GetNetwork->NetworkInterfaces->IPv4->Config = (struct tt__IPv4Configuration *)soap_malloc(soap, sizeof(struct tt__IPv4Configuration));
    memset(GetNetwork->NetworkInterfaces->IPv4->Config, 0, sizeof(struct tt__IPv4Configuration));
    GetNetwork->NetworkInterfaces->IPv4->Config->__sizeManual = 1;
    GetNetwork->NetworkInterfaces->IPv4->Config->Manual = (struct tt__PrefixedIPv4Address *)soap_malloc(soap, sizeof(struct tt__PrefixedIPv4Address));
    GetNetwork->NetworkInterfaces->IPv4->Config->Manual->Address = (char *)soap_malloc(soap, sizeof(char) * 40);
    strcpy(GetNetwork->NetworkInterfaces->IPv4->Config->Manual->Address,_IPAddr);
    GetNetwork->NetworkInterfaces->IPv4->Config->Manual->PrefixLength = 24;

    GetNetwork->NetworkInterfaces->IPv4->Config->LinkLocal = NULL;
    GetNetwork->NetworkInterfaces->IPv4->Config->FromDHCP = (struct tt__PrefixedIPv4Address *)soap_malloc(soap, sizeof(struct tt__PrefixedIPv4Address));
    GetNetwork->NetworkInterfaces->IPv4->Config->FromDHCP->Address = (char *)soap_malloc(soap, sizeof(char) * LARGE_INFO_LENGTH);
    strcpy(GetNetwork->NetworkInterfaces->IPv4->Config->FromDHCP->Address, _IPAddr);
    GetNetwork->NetworkInterfaces->IPv4->Config->FromDHCP->PrefixLength = 24;
    GetNetwork->NetworkInterfaces->IPv4->Config->DHCP = info->dhcp_status;
    GetNetwork->NetworkInterfaces->IPv6 = NULL;
    GetNetwork->NetworkInterfaces->Extension = NULL;

    return SOAP_OK;
    if(info)
    {
        free(info);
        info = NULL;
    }
}

SOAP_FMAC5 int SOAP_FMAC6 __tds__SetNetworkInterfaces(struct soap *soap, struct _tds__SetNetworkInterfaces *input, struct _tds__SetNetworkInterfacesResponse *output)
{
    int local_dhcp_status = 0;
    net_info_t *info = (net_info_t *)calloc(1, sizeof(net_info_t));
    int ret;
    
    ret = get_dev_netinfo(info);
    if(ret < 0)
    {
        fprintf(stderr, "[%s][%d] get device network information error!\n", __FUNCTION__, __LINE__);
    }

    if(input->InterfaceToken != NULL)
    {
        if(strcmp(input->InterfaceToken, ETH_NAME))
        {
        	return SOAP_FAULT;
        }
    }

    if(input->NetworkInterface->IPv6 != NULL)
    {
        if(*input->NetworkInterface->IPv6->Enabled == 1)
        {	
        	return SOAP_FAULT;
        }
    }	
    if(input->NetworkInterface->IPv4 == NULL)
    {
        return SOAP_FAULT;
    }

    printf("NetworkInterface => IPv4->Enabled:%p, IPv4->Manual:%p, IPv4->DHCP:%p\n", input->NetworkInterface->IPv4->Enabled, input->NetworkInterface->IPv4->Manual, input->NetworkInterface->IPv4->DHCP);

    if(input->NetworkInterface->IPv4->Enabled != NULL)
    {
        if(input->NetworkInterface->IPv4->Manual != NULL)
        {
        	if(input->NetworkInterface->IPv4->Manual->Address != NULL)
        	{
        		if(check_ipaddress(input->NetworkInterface->IPv4->Manual->Address) == 0) // Check IP address
        		{
        			return SOAP_FAULT;
        		}
        		info->dhcp_status = 0; //maohw;
            if(strcmp(info->ip_addr, input->NetworkInterface->IPv4->Manual->Address))
            {
                strcpy(info->ip_addr, input->NetworkInterface->IPv4->Manual->Address);
            }
            else
            {
                //maohw return SOAP_OK;
            }
        	}                                            
        }
        else
        {
        	if(input->NetworkInterface->IPv4->DHCP != NULL)
        	{	
                if(*(input->NetworkInterface->IPv4->DHCP) != info->dhcp_status)
                {
                    info->dhcp_status = *(input->NetworkInterface->IPv4->DHCP);
                }
        	}
        }
    }
    
    output->RebootNeeded = FALSE;
    set_dev_netinfo(info);
    if(info)
    {
        free(info);
        info = NULL;
    }
    return SOAP_OK;    
}


SOAP_FMAC5 int SOAP_FMAC6 __tds__GetWsdlUrl(struct soap *soap, struct _tds__GetWsdlUrl *tds__GetWsdlUrl, struct _tds__GetWsdlUrlResponse *tds__GetWsdlUrlResponse)
{
	tds__GetWsdlUrlResponse->WsdlUrl = "http://www.onvif.org/Documents/Specifications.aspx";
  return SOAP_OK;
}

SOAP_FMAC5 int SOAP_FMAC6 __tds__GetNTP(struct soap *soap, struct _tds__GetNTP *tds__GetNTP, struct _tds__GetNTPResponse *tds__GetNTPResponse)
{
  
  tds__GetNTPResponse->NTPInformation = (struct tt__NTPInformation *)soap_malloc(soap, sizeof(struct tt__NTPInformation));
  
  tds__GetNTPResponse->NTPInformation->FromDHCP = _false;
  tds__GetNTPResponse->NTPInformation->__sizeNTPManual = 2;
  tds__GetNTPResponse->NTPInformation->NTPManual = (struct tt__NetworkHost *)soap_malloc(soap, 2 * sizeof(struct tt__NetworkHost));
  tds__GetNTPResponse->NTPInformation->NTPManual[0].Type = tt__NetworkHostType__IPv4;
  tds__GetNTPResponse->NTPInformation->NTPManual[0].IPv4Address = (char **)soap_malloc(soap, sizeof(char *));
  tds__GetNTPResponse->NTPInformation->NTPManual[0].IPv4Address[0] = (char *)soap_malloc(soap, sizeof(char) * INFO_LENGTH);
  
  tds__GetNTPResponse->NTPInformation->NTPManual[1].Type = tt__NetworkHostType__IPv4;
  tds__GetNTPResponse->NTPInformation->NTPManual[1].IPv4Address = (char **)soap_malloc(soap, sizeof(char *));
  tds__GetNTPResponse->NTPInformation->NTPManual[1].IPv4Address[0] = (char *)soap_malloc(soap, sizeof(char) * INFO_LENGTH);
  
  char ntp[2][INFO_LENGTH];
  ntp[0][0] = ntp[1][0] = '\0'; 
  get_ntp_info(ntp[0], ntp[1]);

  strcpy(tds__GetNTPResponse->NTPInformation->NTPManual[0].IPv4Address[0], ntp[0]);
  strcpy(tds__GetNTPResponse->NTPInformation->NTPManual[1].IPv4Address[0], ntp[1]);

  return SOAP_OK;
}

SOAP_FMAC5 int SOAP_FMAC6 __tds__SetNTP(struct soap *soap, struct _tds__SetNTP *tds__SetNTP, struct _tds__SetNTPResponse *tds__SetNTPResponse)
{ 
  char ntp[2][INFO_LENGTH];
  ntp[0][0] = ntp[1][0] = '\0'; 
  
  for(int i = 0; i < tds__SetNTP->__sizeNTPManual && i < 2; i++)
  {
    if(tds__SetNTP->NTPManual[i].IPv4Address)
    {
      printf("%s => i:%d, [%s]\n", __func__, i, tds__SetNTP->NTPManual[i].IPv4Address[0]);
      strncpy(ntp[i], tds__SetNTP->NTPManual[i].IPv4Address[0], INFO_LENGTH-1);
    }
  }
  
  set_ntp_info(ntp[0], ntp[1]);
  
  return SOAP_OK;
}

SOAP_FMAC5 int SOAP_FMAC6 __tds__GetHostname(struct soap *soap, struct _tds__GetHostname *tds__GetHostname, struct _tds__GetHostnameResponse *tds__GetHostnameResponse)
{
	tds__GetHostnameResponse->HostnameInformation = (struct tt__HostnameInformation *)soap_malloc(soap, sizeof(struct tt__HostnameInformation));
	memset(tds__GetHostnameResponse->HostnameInformation, 0, sizeof(struct tt__HostnameInformation));

    tds__GetHostnameResponse->HostnameInformation->Name = (char *) soap_malloc(soap, sizeof(char) * LARGE_INFO_LENGTH); 
	tds__GetHostnameResponse->HostnameInformation->Extension = NULL;
    tds__GetHostnameResponse->HostnameInformation->__anyAttribute = NULL;
    tds__GetHostnameResponse->HostnameInformation->FromDHCP = _false;

    strcpy(tds__GetHostnameResponse->HostnameInformation->Name, "NVR");
    return SOAP_OK;    
}

SOAP_FMAC5 int SOAP_FMAC6 __tds__SetHostname(struct soap *soap, struct _tds__SetHostname *input, struct _tds__SetHostnameResponse *output)
{
    if(input->Name == NULL)
    {
        return SOAP_FAULT;
    }
    return SOAP_OK;
}

SOAP_FMAC5 int SOAP_FMAC6 __tds__GetNetworkDefaultGateway(struct soap *soap, struct _tds__GetNetworkDefaultGateway *input, struct _tds__GetNetworkDefaultGatewayResponse *output)
{
    int ret;
    net_info_t *info;
    info = (net_info_t *)calloc(1, sizeof(net_info_t));
    memset(info, 0, sizeof(net_info_t));

    ret = get_dev_netinfo(info);
    if(ret < 0)
    {
        fprintf(stderr, "[%s][%d] get network information error!\n", __FUNCTION__, __LINE__);
        return SOAP_FAULT;
    }

    output->NetworkGateway = (struct tt__NetworkGateway*)soap_malloc(soap, sizeof(struct tt__NetworkGateway));
    output->NetworkGateway->IPv4Address = (char **)soap_malloc(soap, sizeof(char *));
    output->NetworkGateway->IPv4Address[0] = (char *)soap_malloc(soap, sizeof(char) * INFO_LENGTH);
    strcpy(output->NetworkGateway->IPv4Address[0], info->gateway);
    output->NetworkGateway->IPv6Address = NULL;
    output->NetworkGateway->__sizeIPv4Address = 1;
    output->NetworkGateway->__sizeIPv6Address = 0;

    if(info)
    {
        free(info);
        info = NULL;
    }
    return SOAP_OK;
}

SOAP_FMAC5 int SOAP_FMAC6 __tds__SetNetworkDefaultGateway(struct soap *soap, struct _tds__SetNetworkDefaultGateway *input, struct _tds__SetNetworkDefaultGatewayResponse *output)
{
    int ret;
    net_info_t *info = (net_info_t *)calloc(1, sizeof(net_info_t));
    ret = get_dev_netinfo(info);
    if(ret < 0)
    {
        fprintf(stderr, "[%s][%d] get network information error!\n", __FUNCTION__, __LINE__);
        return SOAP_FAULT;
    }
	
	if(input->__sizeIPv6Address)
	{
		return SOAP_FAULT;
	}
	
	if(!input->__sizeIPv4Address)
	{
		return SOAP_FAULT;
	}
	
	if(check_ipaddress(*input->IPv4Address) == 0) // Check IP address
	{
		return SOAP_FAULT;
	}

	strcpy(info->gateway, *input->IPv4Address);
	
    set_dev_netinfo(info);
    if(info)
    {
        free(info);
        info = NULL;
    }
    return SOAP_OK;

}

SOAP_FMAC5 int SOAP_FMAC6 __tds__GetNetworkProtocols(struct soap *soap, struct _tds__GetNetworkProtocols *input, struct _tds__GetNetworkProtocolsResponse *output)
{
    int ret;
    port_info_t *info = (port_info_t *)calloc(1, sizeof(port_info_t));

    ret = get_dev_portinfo(info);
    if(ret < 0)
    {
        fprintf(stderr, "[%s][%d] get network information error!\n", __FUNCTION__, __LINE__);
        return SOAP_FAULT;
    }
    
    output->__sizeNetworkProtocols = 2;

    output->NetworkProtocols = (struct tt__NetworkProtocol *)soap_malloc(soap, 2 * sizeof(struct tt__NetworkProtocol));
    memset(output->NetworkProtocols, 0, 2 * sizeof(struct tt__NetworkProtocol));

    output->NetworkProtocols[0].Enabled = info->http_enable;
    output->NetworkProtocols[0].Extension = NULL;
    output->NetworkProtocols[0].Name = tt__NetworkProtocolType__HTTP;
    output->NetworkProtocols[0].Port = (int *)soap_malloc(soap, sizeof(int));
    *output->NetworkProtocols[0].Port = info->http_port;
    output->NetworkProtocols[0].__anyAttribute = NULL;
    output->NetworkProtocols[0].__sizePort = 1;

    output->NetworkProtocols[1].Enabled = info->rtsp_enable;
    output->NetworkProtocols[1].Extension = NULL;
    output->NetworkProtocols[1].Name = tt__NetworkProtocolType__RTSP;
    output->NetworkProtocols[1].Port = (int *)soap_malloc(soap, sizeof(int));
    *output->NetworkProtocols[1].Port = info->rtsp_port;
    output->NetworkProtocols[1].__anyAttribute = NULL;
    output->NetworkProtocols[1].__sizePort = 1;

    if(info)
    {
        free(info);
        info = NULL;
    }
    return SOAP_OK;
}

SOAP_FMAC5 int SOAP_FMAC6 __tds__SetNetworkProtocols(struct soap *soap, struct _tds__SetNetworkProtocols *input, struct _tds__SetNetworkProtocolsResponse *output)
{
    int ret;
    port_info_t *info = (port_info_t *)calloc(1, sizeof(port_info_t));

    ret = get_dev_portinfo(info);
    if(ret < 0)
    {
        fprintf(stderr, "[%s][%d] get network information error!\n", __FUNCTION__, __LINE__);
        return SOAP_FAULT;
    }

    int i;
    for(i = 0; i < input->__sizeNetworkProtocols; i++)  
    {
        if(input->NetworkProtocols[i].Name == tt__NetworkProtocolType__HTTP)
        {
            info->http_port = *input->NetworkProtocols[i].Port;
            info->http_enable = input->NetworkProtocols[i].Enabled;
        }
        else if(input->NetworkProtocols[i].Name == tt__NetworkProtocolType__RTSP)
        {
            info->rtsp_port = *input->NetworkProtocols[i].Port;
            info->rtsp_enable = input->NetworkProtocols[i].Enabled;
        }
    }

    set_dev_portinfo(info);
    if(info)
    {
        free(info);
        info = NULL;
    }
    return SOAP_OK;
}

SOAP_FMAC5 int SOAP_FMAC6 __tds__GetDiscoveryMode(struct soap *soap, struct _tds__GetDiscoveryMode *input, struct _tds__GetDiscoveryModeResponse *output)
{
    output->DiscoveryMode = g_nvt->t.discovery_mode;
    return SOAP_OK;
}

SOAP_FMAC5 int SOAP_FMAC6 __tds__SetDiscoveryMode(struct soap *soap, struct _tds__SetDiscoveryMode *input, struct _tds__SetDiscoveryModeResponse *output)
{
	unsigned char  mode = input->DiscoveryMode;
	if(mode == 0 || mode == 1)
	{
		g_nvt->t.discovery_mode = mode;
	}
    else
    {
        return SOAP_FAULT;
    }
    return SOAP_OK;
}

SOAP_FMAC5 int SOAP_FMAC6 __tds__GetUsers(struct soap *soap, struct _tds__GetUsers *input, struct _tds__GetUsersResponse *output)
{
    int i;
    user_info_t *users;
    user_right_t  *user_info = NULL;

    users = (user_info_t *)calloc(1, sizeof(user_info_t));
    get_user_info(users);
    
    output->User = (struct tt__User *)soap_malloc(soap, sizeof(struct tt__User) * users->size);
    memset(output->User, 0, sizeof(struct tt__User) * users->size);
    for(i = 0; i < users->size; i++)
    {
        user_info = (user_right_t *)(users->buf + sizeof(user_right_t) * i);
        output->User[i].Username = (char *) soap_malloc(soap, sizeof(char) * INFO_LEN);
        strcpy(output->User[i].Username, user_info->user_name);
        if(!strcmp(output->User[i].Username, "admin"))
        {
            output->User[i].UserLevel = tt__UserLevel__Administrator;
        }
        else
        {
            output->User[i].UserLevel = tt__UserLevel__User;
        }
        if(output->User[i].Username == NULL)
        {
            break;    
        }
        output->User[i].__anyAttribute = NULL;
        output->User[i].Extension = NULL;
    }
    output->__sizeUser = i;
    if(users)
    {
        free(users);
        users = NULL;
    }
    return SOAP_OK;
}

int check_user_exist(char *name)
{
    int i;
    user_info_t *users;
    user_right_t  *user_info = NULL;

    users = (user_info_t *)calloc(1, sizeof(user_info_t));
    get_user_info(users);

    for(i = 0; i < users->size; i++)
    {
        user_info = (user_right_t *)(users->buf + sizeof(user_right_t) * i);
        if(strcmp(user_info->user_name, name))
        {
            return 0;
        }
    }
    return 1;
}

SOAP_FMAC5 int SOAP_FMAC6 __tds__CreateUsers(struct soap *soap, struct _tds__CreateUsers *input, struct _tds__CreateUsersResponse *output)
{
    user_right_t  user_info;
    int level;
    char name[INFO_LEN];
    char passwd[INFO_LEN];

    if((input->User->Username == NULL) || (input->User->Password == NULL))
    {
        return SOAP_FAULT;
    }

    if(check_user_exist(input->User->Username))
    {
        fprintf(stderr, "[%s][%d] user is exist!\n", __FUNCTION__, __LINE__);
        return SOAP_FAULT;
    }
    memset(name, 0, sizeof(name));
    memset(passwd, 0, sizeof(passwd));
    strcpy(name, input->User->Username);
    strcpy(passwd, input->User->Password);
    level = input->User->UserLevel;
    
    add_user_info(name, passwd);
    return SOAP_OK;
}

SOAP_FMAC5 int SOAP_FMAC6 __tds__DeleteUsers(struct soap *soap, struct _tds__DeleteUsers *input, struct _tds__DeleteUsersResponse *output)
{
    char name[INFO_LEN];
    if(*input->Username == NULL)
    {
        return SOAP_FAULT;
    }

    if(!strcmp(*input->Username, "admin"))
    {
        return SOAP_FAULT;
    }

    del_user_info(*input->Username);
    return SOAP_OK;
}

SOAP_FMAC5 int SOAP_FMAC6 __tds__SetUser(struct soap *soap, struct _tds__SetUser *input, struct _tds__SetUserResponse *output)
{
    user_right_t  user_info;
    int level;
    char name[INFO_LEN];
    char passwd[INFO_LEN];

    if((input->User->Username == NULL) || (input->User->Password == NULL))
    {
        return SOAP_FAULT;
    }
    
    if(check_user_exist(input->User->Username))
    {
        return SOAP_FAULT;
    }

    memset(name, 0, sizeof(name));
    memset(passwd, 0, sizeof(passwd));
    strcpy(name, input->User->Username);
    strcpy(passwd, input->User->Password);
    level = input->User->UserLevel;
    
    set_user_info(name, passwd);
    return SOAP_OK;
}

SOAP_FMAC5 int SOAP_FMAC6 __tds__GetSystemLog(struct soap *soap, struct _tds__GetSystemLog *input, struct _tds__GetSystemLogResponse *output)
{
#if 1
    int ret;
    if(input->LogType == 0)
    {
        return SOAP_FAULT;
    }

    char syslog[MAX_LOG_LEN];
    ret = get_device_syslog(input->LogType, syslog);

    output->SystemLog = (struct tt__SystemLog *)soap_malloc(soap, sizeof(struct tt__SystemLog));
    output->SystemLog->String = (char *)soap_malloc(soap, sizeof(char) * MAX_LOG_LEN);
    memset(output->SystemLog->String, 0, sizeof(char) * MAX_LOG_LEN);
    output->SystemLog->Binary = NULL;
    sprintf(output->SystemLog->String, "%s", syslog);
#endif
    return SOAP_OK;
}
SOAP_FMAC5 int SOAP_FMAC6 __tds__SetSystemFactoryDefault(struct soap *soap, struct _tds__SetSystemFactoryDefault *input, struct _tds__SetSystemFactoryDefaultResponse *output)
{
    //soft: All parameters value are set to factory default, expect basic network setting
    //hard: All parameters value are set to factory default
    int mask;
    if(input->FactoryDefault == tt__FactoryDefaultType__Soft)
    {
        fprintf(stderr, "[%s] default type tt__FactoryDefaultType__Soft\n", __FUNCTION__);
        mask = 1;
    }
    else if(input->FactoryDefault == tt__FactoryDefaultType__Hard)
    {
        fprintf(stderr, "[%s] default type tt__FactoryDefaultType__Hard\n", __FUNCTION__);
        mask = 0;
    }
    else
    {
        return SOAP_FAULT;
    }
    set_device_factorydefault(mask);
    return SOAP_OK;
}

SOAP_FMAC5 int SOAP_FMAC6 __tds__StartFirmwareUpgrade(struct soap *soap, struct _tds__StartFirmwareUpgrade *input, struct _tds__StartFirmwareUpgradeResponse *output)
{
#if 1
    NET_IPV4 ip;
    ip.int32 =  net_get_ifaddr(ETH_NAME);
    output->ExpectedDownTime = 36000;
    output->UploadDelay = 60;
    output->UploadUri = (char *)soap_malloc(soap, sizeof(char) * INFO_LEN);
    memset(output->UploadUri, 0, sizeof(output->UploadUri));
    sprintf(output->UploadUri, "http://%d.%d.%d.%d:8080", ip.str[0], ip.str[1], ip.str[2], ip.str[3]);
#endif
    return SOAP_OK;
}

SOAP_FMAC5 int SOAP_FMAC6 __tds__UpgradeSystemFirmware(struct soap *soap, struct _tds__UpgradeSystemFirmware *input, struct _tds__UpgradeSystemFirmwareResponse *output)
{
    return SOAP_OK;
}

SOAP_FMAC5 int SOAP_FMAC6 __tds__SystemReboot(struct soap *soap, struct _tds__SystemReboot *input, struct _tds__SystemRebootResponse *output)
{
    char buf[256] = {0};
    sprintf(buf, "Rebooting in %d seconds.", REBOOT_SEC);
    output->Message = (char *)soap_malloc(soap, sizeof(char) * 256);
    memset(output->Message, 0, sizeof(char) * 256);
    sprintf(output->Message, "%s", buf);
    return SOAP_OK;
}


int ptz_cfg_get(struct soap* soap, int chs, struct tt__PTZConfiguration *pPtz)
{
    pPtz->UseCount = 0;
  	pPtz->Name = (char *)soap_malloc(soap, sizeof(char)*SMALL_INFO_LENGTH);
  	strcpy(pPtz->Name, "Ptz");
  	pPtz->token = (char *)soap_malloc(soap, sizeof(char)*SMALL_INFO_LENGTH);
  	strcpy(pPtz->token, "PtzToken");
  	pPtz->NodeToken = (char *)soap_malloc(soap, sizeof(char)*SMALL_INFO_LENGTH);
  	strcpy(pPtz->NodeToken, "PtzNodeToken");

  	pPtz->DefaultAbsolutePantTiltPositionSpace = (char *)soap_malloc(soap, sizeof(char)*SMALL_INFO_LENGTH);
  	strcpy(pPtz->DefaultAbsolutePantTiltPositionSpace, "http://www.onvif.org/ver10/tptz/PanTiltSpaces/PositionGenericSpace");
  	pPtz->DefaultAbsoluteZoomPositionSpace = (char *)soap_malloc(soap, sizeof(char)*SMALL_INFO_LENGTH);
  	strcpy(pPtz->DefaultAbsoluteZoomPositionSpace, "http://www.onvif.org/ver10/tptz/ZoomSpaces/PositionGenericSpace");
  	pPtz->DefaultRelativePanTiltTranslationSpace = (char *)soap_malloc(soap, sizeof(char)*SMALL_INFO_LENGTH);
  	strcpy(pPtz->DefaultRelativePanTiltTranslationSpace, "http://www.onvif.org/ver10/tptz/PanTiltSpaces/TranslationGenericSpace");
  	pPtz->DefaultRelativeZoomTranslationSpace = (char *)soap_malloc(soap, sizeof(char)*SMALL_INFO_LENGTH);
  	strcpy(pPtz->DefaultRelativeZoomTranslationSpace, "http://www.onvif.org/ver10/tptz/ZoomSpaces/TranslationGenericSpace");
  	pPtz->DefaultContinuousPanTiltVelocitySpace = (char *)soap_malloc(soap, sizeof(char)*SMALL_INFO_LENGTH);
  	strcpy(pPtz->DefaultContinuousPanTiltVelocitySpace, "http://www.onvif.org/ver10/tptz/PanTiltSpaces/VelocityGenericSpace");
  	pPtz->DefaultContinuousZoomVelocitySpace = (char *)soap_malloc(soap, sizeof(char)*SMALL_INFO_LENGTH);
  	strcpy(pPtz->DefaultContinuousZoomVelocitySpace, "http://www.onvif.org/ver10/tptz/ZoomSpaces/VelocityGenericSpace");

  	pPtz->DefaultPTZSpeed = (struct tt__PTZSpeed *)soap_malloc(soap, sizeof(struct tt__PTZSpeed));
  	memset(pPtz->DefaultPTZSpeed, 0, sizeof(struct tt__PTZSpeed));
  	pPtz->DefaultPTZSpeed->PanTilt = (struct tt__Vector2D *)soap_malloc(soap, sizeof(struct tt__Vector2D));
  	pPtz->DefaultPTZSpeed->Zoom = (struct tt__Vector1D *)soap_malloc(soap, sizeof(struct tt__Vector1D));
  	memset(pPtz->DefaultPTZSpeed->PanTilt, 0, sizeof(struct tt__Vector2D));
  	memset(pPtz->DefaultPTZSpeed->Zoom, 0, sizeof(struct tt__Vector1D));
  	pPtz->DefaultPTZSpeed->PanTilt->space = (char *)soap_malloc(soap, sizeof(char)*SMALL_INFO_LENGTH);
  	pPtz->DefaultPTZSpeed->Zoom->space = (char *)soap_malloc(soap, sizeof(char)*SMALL_INFO_LENGTH);
  	memset(pPtz->DefaultPTZSpeed->PanTilt->space, 0, sizeof(char)*SMALL_INFO_LENGTH);
  	memset(pPtz->DefaultPTZSpeed->Zoom->space, 0, sizeof(char)*SMALL_INFO_LENGTH);
  	pPtz->DefaultPTZSpeed->PanTilt->x = 0.5;
  	pPtz->DefaultPTZSpeed->PanTilt->y = 0.5;
  	pPtz->DefaultPTZSpeed->Zoom->x = 0.5;
  	strcpy(pPtz->DefaultPTZSpeed->PanTilt->space, "http://www.onvif.org/ver10/tptz/PanTiltSpaces/GenericSpeedSpace");
  	strcpy(pPtz->DefaultPTZSpeed->Zoom->space, "http://www.onvif.org/ver10/tptz/ZoomSpaces/ZoomGenericSpeedSpace");

  	pPtz->DefaultPTZTimeout = (LONG64 *)soap_malloc(soap, sizeof(LONG64));
  	*pPtz->DefaultPTZTimeout = 5000;

  	pPtz->PanTiltLimits = (struct tt__PanTiltLimits *)soap_malloc(soap, sizeof(struct tt__PanTiltLimits));
  	memset(pPtz->PanTiltLimits, 0, sizeof(struct tt__PanTiltLimits));
  	pPtz->PanTiltLimits->Range = (struct tt__Space2DDescription *)soap_malloc(soap, sizeof(struct tt__Space2DDescription));
  	memset(pPtz->PanTiltLimits->Range, 0, sizeof(struct tt__Space2DDescription));
  	pPtz->PanTiltLimits->Range->URI = (char *)soap_malloc(soap, sizeof(char)*SMALL_INFO_LENGTH);
  	strcpy(pPtz->PanTiltLimits->Range->URI, "http://www.onvif.org/ver10/tptz/PanTiltSpaces/PositionGenericSpace");
  	pPtz->PanTiltLimits->Range->XRange = (struct tt__FloatRange *)soap_malloc(soap, sizeof(struct tt__FloatRange));
  	pPtz->PanTiltLimits->Range->YRange = (struct tt__FloatRange *)soap_malloc(soap, sizeof(struct tt__FloatRange));
  	pPtz->PanTiltLimits->Range->XRange->Max = 1.0;
  	pPtz->PanTiltLimits->Range->XRange->Min = -1.0;
  	pPtz->PanTiltLimits->Range->YRange->Max = 1.0;
  	pPtz->PanTiltLimits->Range->YRange->Min = -1.0;

  	pPtz->ZoomLimits = (struct tt__ZoomLimits *)soap_malloc(soap, sizeof(struct tt__ZoomLimits));
  	memset(pPtz->ZoomLimits, 0, sizeof(struct tt__PanTiltLimits));
  	pPtz->ZoomLimits->Range = (struct tt__Space1DDescription *)soap_malloc(soap, sizeof(struct tt__Space1DDescription));
  	memset(pPtz->ZoomLimits->Range, 0, sizeof(struct tt__Space1DDescription));
  	pPtz->ZoomLimits->Range->URI = (char *)soap_malloc(soap, sizeof(char)*SMALL_INFO_LENGTH);
  	strcpy(pPtz->ZoomLimits->Range->URI, "http://www.onvif.org/ver10/tptz/ZoomSpaces/PositionGenericSpace");
  	pPtz->ZoomLimits->Range->XRange = (struct tt__FloatRange *)soap_malloc(soap, sizeof(struct tt__FloatRange));
  	pPtz->ZoomLimits->Range->XRange->Max = 1.0;
  	pPtz->ZoomLimits->Range->XRange->Min = -1.0;   
  	
  	return SOAP_OK;
}



SOAP_FMAC5 int SOAP_FMAC6 __trt__GetProfile(struct soap *soap, struct _trt__GetProfile *input, struct _trt__GetProfileResponse *GetProf)
{
    int i;
    char _IPAddr[LARGE_INFO_LENGTH];
    NET_IPV4 ip;
    int flag = -1;
    
    if((input->ProfileToken == NULL) || (strcmp(input->ProfileToken, "") == 0))
        return SOAP_FAULT;

    for(i = 0; i < g_nvt->t.nprofiles; ++i)
    {
        if(strcmp(input->ProfileToken, g_nvt->t.profiles[i].ptoken) == 0)
        {
            flag++;
            break;
       }
    }

    if(flag < 0)
        return SOAP_FAULT;

    ip.int32 =  net_get_ifaddr(ETH_NAME);
    sprintf(_IPAddr, "%d.%d.%d.%d", ip.str[0], ip.str[1], ip.str[2], ip.str[3]);
#if 1  
    onvif_profile_t *_profile = (onvif_profile_t*)calloc(1, sizeof(onvif_profile_t));
    _profile->aec = (audio_encoder_conf_t *)calloc(1, sizeof(audio_encoder_conf_t));
    _profile->asc = (audio_source_conf_t *)calloc(1, sizeof(audio_source_conf_t));
    _profile->vec = (video_encoder_conf_t *)calloc(1, sizeof(video_encoder_conf_t));
    _profile->vsc = (video_source_conf_t *)calloc(1, sizeof(video_source_conf_t));
    get_dev_profiles(i, _profile, 2, VSC | ASC | VEC |AEC);
    
    //malloc and init Profile
    GetProf->Profile =(struct tt__Profile *)soap_malloc(soap, sizeof(struct tt__Profile));
    memset(GetProf->Profile, 0, sizeof(struct tt__Profile));

    //init profile's name token and fixed member
    GetProf->Profile->Name = (char *)soap_malloc(soap, sizeof(char) * SMALL_INFO_LENGTH);
    strncpy(GetProf->Profile->Name, _profile->pname, SMALL_INFO_LENGTH);
    GetProf->Profile->token = (char *)soap_malloc(soap, sizeof(char) * SMALL_INFO_LENGTH);
    strncpy(GetProf->Profile->token, _profile->ptoken, SMALL_INFO_LENGTH);
    GetProf->Profile->fixed = (enum xsd__boolean_ *)soap_malloc(soap, sizeof(int));
    *GetProf->Profile->fixed = _profile->pfixed;

    // malloc and init for Profile VideoSourceConfiguration
    GetProf->Profile->VideoSourceConfiguration = (struct tt__VideoSourceConfiguration *)soap_malloc(soap, sizeof(struct tt__VideoSourceConfiguration));
    memset(GetProf->Profile->VideoSourceConfiguration, 0, sizeof(struct tt__VideoSourceConfiguration));
    GetProf->Profile->VideoSourceConfiguration->Name = (char *)soap_malloc(soap, sizeof(char) * SMALL_INFO_LENGTH);
    strncpy(GetProf->Profile->VideoSourceConfiguration->Name, _profile->vsc->vname, SMALL_INFO_LENGTH);
    GetProf->Profile->VideoSourceConfiguration->token = (char *)soap_malloc(soap, sizeof(char) * SMALL_INFO_LENGTH);
    strncpy(GetProf->Profile->VideoSourceConfiguration->token, _profile->vsc->vtoken, SMALL_INFO_LENGTH);
    GetProf->Profile->VideoSourceConfiguration->SourceToken = (char *)soap_malloc(soap, sizeof(char) * SMALL_INFO_LENGTH);
    strncpy(GetProf->Profile->VideoSourceConfiguration->SourceToken, _profile->vsc->vsourcetoken, SMALL_INFO_LENGTH);
    GetProf->Profile->VideoSourceConfiguration->UseCount = _profile->vsc->vcount;
    GetProf->Profile->VideoSourceConfiguration->Bounds = (struct tt__IntRectangle *)soap_malloc(soap, sizeof(struct tt__IntRectangle));
    memset(GetProf->Profile->VideoSourceConfiguration->Bounds, 0, sizeof(struct tt__IntRectangle));
    GetProf->Profile->VideoSourceConfiguration->Bounds->y = _profile->vsc->x;
    GetProf->Profile->VideoSourceConfiguration->Bounds->x = _profile->vsc->y;
    GetProf->Profile->VideoSourceConfiguration->Bounds->width  = _profile->vsc->width;
    GetProf->Profile->VideoSourceConfiguration->Bounds->height = _profile->vsc->heigth;

    // malloc and init for Profile  AudioSourceConfiguration
    GetProf->Profile->AudioSourceConfiguration = (struct tt__AudioSourceConfiguration *)soap_malloc(soap, sizeof(struct tt__AudioSourceConfiguration));
    memset(GetProf->Profile->AudioSourceConfiguration, 0, sizeof(struct tt__AudioSourceConfiguration));
    GetProf->Profile->AudioSourceConfiguration->Name = (char *)soap_malloc(soap, sizeof(char) * SMALL_INFO_LENGTH);
    strncpy(GetProf->Profile->AudioSourceConfiguration->Name, _profile->asc->aname, SMALL_INFO_LENGTH);
    GetProf->Profile->AudioSourceConfiguration->token = (char *)soap_malloc(soap, sizeof(char) * SMALL_INFO_LENGTH);
    strncpy(GetProf->Profile->AudioSourceConfiguration->token, _profile->asc->token, SMALL_INFO_LENGTH);
    GetProf->Profile->AudioSourceConfiguration->SourceToken = (char *)soap_malloc(soap, sizeof(char) * SMALL_INFO_LENGTH);
    strncpy(GetProf->Profile->AudioSourceConfiguration->SourceToken, _profile->asc->sourcetoken, SMALL_INFO_LENGTH);
    GetProf->Profile->AudioSourceConfiguration->UseCount = _profile->asc->count;

    // malloc and init for Profile  VideoEncoderConfiguration
    GetProf->Profile->VideoEncoderConfiguration = (struct tt__VideoEncoderConfiguration *)soap_malloc(soap, sizeof(struct tt__VideoEncoderConfiguration));
    memset(GetProf->Profile->VideoEncoderConfiguration, 0, sizeof(struct tt__VideoEncoderConfiguration));
    GetProf->Profile->VideoEncoderConfiguration->Name = (char *)soap_malloc(soap, sizeof(char) * SMALL_INFO_LENGTH);
    strncpy(GetProf->Profile->VideoEncoderConfiguration->Name, _profile->vec->name, SMALL_INFO_LENGTH);
    GetProf->Profile->VideoEncoderConfiguration->token = (char *)soap_malloc(soap, sizeof(char) * SMALL_INFO_LENGTH);
    strncpy(GetProf->Profile->VideoEncoderConfiguration->token, _profile->vec->token, SMALL_INFO_LENGTH);
    GetProf->Profile->VideoEncoderConfiguration->UseCount = _profile->vec->usecount;
    GetProf->Profile->VideoEncoderConfiguration->Encoding = _profile->vec->encoding; //JPEG = 0, MPEG4 = 1, H264 = 2;
    GetProf->Profile->VideoEncoderConfiguration->Resolution = (struct tt__VideoResolution *)soap_malloc(soap, sizeof(struct tt__VideoResolution));
    GetProf->Profile->VideoEncoderConfiguration->Resolution->Width = _profile->vec->width;
    GetProf->Profile->VideoEncoderConfiguration->Resolution->Height =_profile->vec->height;
    GetProf->Profile->VideoEncoderConfiguration->Quality = _profile->vec->quality;
    GetProf->Profile->VideoEncoderConfiguration->RateControl = (struct tt__VideoRateControl *)soap_malloc(soap, sizeof(struct tt__VideoRateControl));
    GetProf->Profile->VideoEncoderConfiguration->RateControl->FrameRateLimit = _profile->vec->FrameRateLimit;
    GetProf->Profile->VideoEncoderConfiguration->RateControl->EncodingInterval = _profile->vec->EncodingInterval;
    GetProf->Profile->VideoEncoderConfiguration->RateControl->BitrateLimit = _profile->vec->BitrateLimit;
    GetProf->Profile->VideoEncoderConfiguration->MPEG4 = (struct tt__Mpeg4Configuration *)soap_malloc(soap, sizeof(struct tt__Mpeg4Configuration));
    GetProf->Profile->VideoEncoderConfiguration->MPEG4->GovLength = _profile->vec->GovLength;
    GetProf->Profile->VideoEncoderConfiguration->MPEG4->Mpeg4Profile = _profile->vec->mProfile; //SP = 0, ASP = 1
    GetProf->Profile->VideoEncoderConfiguration->H264 = (struct tt__H264Configuration *)soap_malloc(soap, sizeof(struct tt__H264Configuration));
    GetProf->Profile->VideoEncoderConfiguration->H264->GovLength = _profile->vec->gLength;
    GetProf->Profile->VideoEncoderConfiguration->H264->H264Profile = _profile->vec->hProfile;//Baseline = 0, Main = 1, High = 3
    GetProf->Profile->VideoEncoderConfiguration->Multicast = (struct tt__MulticastConfiguration *)soap_malloc(soap, sizeof(struct tt__MulticastConfiguration));
    memset(GetProf->Profile->VideoEncoderConfiguration->Multicast, 0, sizeof(struct tt__MulticastConfiguration));
    GetProf->Profile->VideoEncoderConfiguration->Multicast->Port = 80;//nvt.t.profiles[i].vec->MultiConf.port;
    GetProf->Profile->VideoEncoderConfiguration->Multicast->TTL = _profile->vec->MultiConf.ttl;
    GetProf->Profile->VideoEncoderConfiguration->Multicast->AutoStart = _profile->vec->MultiConf.autostart;//false
    GetProf->Profile->VideoEncoderConfiguration->Multicast->Address  = (struct tt__IPAddress *)soap_malloc(soap, sizeof(struct tt__IPAddress));
    GetProf->Profile->VideoEncoderConfiguration->Multicast->Address->Type = _profile->vec->MultiConf.type;
    GetProf->Profile->VideoEncoderConfiguration->Multicast->Address->IPv4Address = (char **)soap_malloc(soap, sizeof(char *));
    GetProf->Profile->VideoEncoderConfiguration->Multicast->Address->IPv4Address[0] = (char *)soap_malloc(soap, sizeof(char) * INFO_LENGTH);
    strcpy(*GetProf->Profile->VideoEncoderConfiguration->Multicast->Address->IPv4Address, _IPAddr);
    GetProf->Profile->VideoEncoderConfiguration->Multicast->Address->IPv6Address = NULL;
    GetProf->Profile->VideoEncoderConfiguration->SessionTimeout = _profile->vec->SessionTimeout;

    // malloc and init for Profile  AudioSourceConfiguration GetProf->Profile->
    GetProf->Profile->AudioEncoderConfiguration = (struct tt__AudioEncoderConfiguration *)soap_malloc(soap, sizeof(struct tt__AudioEncoderConfiguration));
    memset(GetProf->Profile->AudioEncoderConfiguration, 0, sizeof(struct tt__AudioEncoderConfiguration));
    GetProf->Profile->AudioEncoderConfiguration->Name = (char *)soap_malloc(soap, sizeof(char) * SMALL_INFO_LENGTH);
    strncpy(GetProf->Profile->AudioEncoderConfiguration->Name, _profile->aec->name, SMALL_INFO_LENGTH);
    GetProf->Profile->AudioEncoderConfiguration->token = (char *)soap_malloc(soap, sizeof(char) * SMALL_INFO_LENGTH);
    strncpy(GetProf->Profile->AudioEncoderConfiguration->token, _profile->aec->token, SMALL_INFO_LENGTH);
    GetProf->Profile->AudioEncoderConfiguration->UseCount = _profile->aec->usecount;
    GetProf->Profile->AudioEncoderConfiguration->Encoding = _profile->aec->encoding;
    GetProf->Profile->AudioEncoderConfiguration->Bitrate = _profile->aec->bitrate;
    GetProf->Profile->AudioEncoderConfiguration->SampleRate= _profile->aec->samplerate;
    GetProf->Profile->AudioEncoderConfiguration->SessionTimeout = _profile->aec->SessionTimeout;
    GetProf->Profile->AudioEncoderConfiguration->Multicast = (struct tt__MulticastConfiguration *)soap_malloc(soap, sizeof(struct tt__MulticastConfiguration));
    memset(GetProf->Profile->AudioEncoderConfiguration->Multicast, 0, sizeof(struct tt__MulticastConfiguration));
    GetProf->Profile->AudioEncoderConfiguration->Multicast->Port = 80;
    GetProf->Profile->AudioEncoderConfiguration->Multicast->TTL = _profile->aec->MultiConf.ttl;
    GetProf->Profile->AudioEncoderConfiguration->Multicast->AutoStart = _profile->aec->MultiConf.autostart;
    GetProf->Profile->AudioEncoderConfiguration->Multicast->Address  = (struct tt__IPAddress *)soap_malloc(soap, sizeof(struct tt__IPAddress));
    GetProf->Profile->AudioEncoderConfiguration->Multicast->Address->Type = _profile->aec->MultiConf.type;
    GetProf->Profile->AudioEncoderConfiguration->Multicast->Address->IPv4Address = (char **)soap_malloc(soap, sizeof(char *));
    GetProf->Profile->AudioEncoderConfiguration->Multicast->Address->IPv4Address[0] = (char *)soap_malloc(soap, sizeof(char) * INFO_LENGTH);
    strcpy(*GetProf->Profile->AudioEncoderConfiguration->Multicast->Address->IPv4Address, _IPAddr);
    GetProf->Profile->AudioEncoderConfiguration->Multicast->Address->IPv6Address = NULL;

    // malloc and init for Profile  VideoAnalyticsConfiguration
    GetProf->Profile->VideoAnalyticsConfiguration = (struct tt__VideoAnalyticsConfiguration *) soap_malloc(soap, sizeof(struct tt__VideoAnalyticsConfiguration));
    memset(GetProf->Profile->VideoAnalyticsConfiguration, 0, sizeof(struct tt__VideoAnalyticsConfiguration));
    GetProf->Profile->VideoAnalyticsConfiguration->Name = (char *)soap_malloc(soap, sizeof(char) * SMALL_INFO_LENGTH);
    strncpy(GetProf->Profile->VideoAnalyticsConfiguration->Name, _profile->vec->name, SMALL_INFO_LENGTH);
    GetProf->Profile->VideoAnalyticsConfiguration->UseCount = _profile->vec->usecount;
    GetProf->Profile->VideoAnalyticsConfiguration->token = (char *)soap_malloc(soap, sizeof(char) * SMALL_INFO_LENGTH);
    strncpy(GetProf->Profile->VideoAnalyticsConfiguration->token, _profile->vec->token, SMALL_INFO_LENGTH);
    GetProf->Profile->VideoAnalyticsConfiguration->AnalyticsEngineConfiguration = (struct tt__AnalyticsEngineConfiguration *)soap_malloc(soap, sizeof(struct tt__AnalyticsEngineConfiguration));
    GetProf->Profile->VideoAnalyticsConfiguration->AnalyticsEngineConfiguration->__sizeAnalyticsModule = 0;
    GetProf->Profile->VideoAnalyticsConfiguration->AnalyticsEngineConfiguration->AnalyticsModule = NULL;
    GetProf->Profile->VideoAnalyticsConfiguration->AnalyticsEngineConfiguration->Extension = NULL;
    GetProf->Profile->VideoAnalyticsConfiguration->RuleEngineConfiguration = (struct tt__RuleEngineConfiguration *)soap_malloc(soap, sizeof(struct tt__RuleEngineConfiguration));
    GetProf->Profile->VideoAnalyticsConfiguration->RuleEngineConfiguration->__sizeRule = 0;
    GetProf->Profile->VideoAnalyticsConfiguration->RuleEngineConfiguration->Rule = NULL;
    GetProf->Profile->VideoAnalyticsConfiguration->RuleEngineConfiguration->Extension = NULL;

    // malloc and init for Profile  PTZConfiguration
    GetProf->Profile->PTZConfiguration = (struct tt__PTZConfiguration *)soap_malloc(soap, sizeof(struct tt__PTZConfiguration));
    memset(GetProf->Profile->PTZConfiguration, 0, sizeof(struct tt__PTZConfiguration));
    ptz_cfg_get(soap, i, GetProf->Profile->PTZConfiguration);
    
    //extension
    GetProf->Profile->Extension = NULL;
    // malloc and init for Profile  MetadataConfiguration
    GetProf->Profile->MetadataConfiguration = NULL;
#if 0
    GetProf->Profile->MetadataConfiguration = (struct tt__MetadataConfiguration *)soap_malloc(soap,sizeof(struct tt__MetadataConfiguration));
    memset(GetProf->Profile->MetadataConfiguration, 0, sizeof(struct tt__MetadataConfiguration));
    GetProf->Profile->MetadataConfiguration->Name = _profile->metac->name;
    GetProf->Profile->MetadataConfiguration->UseCount = _profile->metac->usecount;
    GetProf->Profile->MetadataConfiguration->token = _profile->metac->token;
    GetProf->Profile->MetadataConfiguration->Analytics = (enum xsd__boolean_ *)soap_malloc(soap, sizeof(char) * sizeof(enum xsd__boolean_));
    *GetProf->Profile->MetadataConfiguration->Analytics = _profile->metac->analytics;
    GetProf->Profile->MetadataConfiguration->SessionTimeout = _profile->metac->sessionTimeout;
    GetProf->Profile->MetadataConfiguration->PTZStatus = NULL;
    GetProf->Profile->MetadataConfiguration->Events = NULL;
    GetProf->Profile->MetadataConfiguration->Multicast = (struct tt__MulticastConfiguration *)soap_malloc(soap, sizeof(struct tt__MulticastConfiguration));
    memset(GetProf->Profile->MetadataConfiguration->Multicast, 0, sizeof(struct tt__MulticastConfiguration));
    GetProf->Profile->MetadataConfiguration->Multicast->Port = _profile->metac->multiconf.port;
    GetProf->Profile->MetadataConfiguration->Multicast->TTL = _profile->metac->multiconf.ttl;
    GetProf->Profile->MetadataConfiguration->Multicast->AutoStart = _profile->metac->multiconf.autostart;
    GetProf->Profile->MetadataConfiguration->Multicast->Address  = (struct tt__IPAddress *)soap_malloc(soap, sizeof(struct tt__IPAddress));
    memset(GetProf->Profile->MetadataConfiguration->Multicast->Address, 0, sizeof(struct tt__IPAddress));
    GetProf->Profile->MetadataConfiguration->Multicast->Address->Type = tt__IPType__IPv4;
    GetProf->Profile->MetadataConfiguration->Multicast->Address->IPv4Address = (char **)soap_malloc(soap, sizeof(char) * sizeof(char *));
    GetProf->Profile->MetadataConfiguration->Multicast->Address->IPv4Address[0] = (char *)soap_malloc(soap, sizeof(char) * LARGE_INFO_LENGTH);
    strcpy(*GetProf->Profile->MetadataConfiguration->Multicast->Address->IPv4Address, _IPAddr);
    GetProf->Profile->MetadataConfiguration->Multicast->Address->IPv6Address = NULL;
#endif

    if(_profile)
    {
        if(_profile->aec) free(_profile->aec);
        if(_profile->asc) free(_profile->asc);
        if(_profile->vec) free(_profile->vec);
        if(_profile->vsc) free(_profile->vsc);
        free(_profile);
        _profile = NULL;
    }
#else
    get_dev_profiles(ALL_CHS, g_nvt->t.profiles, 2, VSC | ASC | VEC |AEC);

    //malloc and init Profile
    GetProf->Profile =(struct tt__Profile *)soap_malloc(soap, sizeof(struct tt__Profile));
    memset(GetProf->Profile, 0, sizeof(struct tt__Profile));

    //init profile's name token and fixed member
    GetProf->Profile->Name = (char *)soap_malloc(soap, sizeof(char) * SMALL_INFO_LENGTH);
    strncpy(GetProf->Profile->Name, g_nvt->t.profiles[i].pname, SMALL_INFO_LENGTH);
    GetProf->Profile->token = (char *)soap_malloc(soap, sizeof(char) * SMALL_INFO_LENGTH);
    strncpy(GetProf->Profile->token, g_nvt->t.profiles[i].ptoken, SMALL_INFO_LENGTH);
    GetProf->Profile->fixed = (enum xsd__boolean_ *)soap_malloc(soap, sizeof(int));
    *GetProf->Profile->fixed = g_nvt->t.profiles[i].pfixed;

    // malloc and init for Profile VideoSourceConfiguration
    GetProf->Profile->VideoSourceConfiguration = (struct tt__VideoSourceConfiguration *)soap_malloc(soap, sizeof(struct tt__VideoSourceConfiguration));
    memset(GetProf->Profile->VideoSourceConfiguration, 0, sizeof(struct tt__VideoSourceConfiguration));
    GetProf->Profile->VideoSourceConfiguration->Name = (char *)soap_malloc(soap, sizeof(char) * SMALL_INFO_LENGTH);
    strncpy(GetProf->Profile->VideoSourceConfiguration->Name, g_nvt->t.profiles[i].vsc->vname, SMALL_INFO_LENGTH);
    GetProf->Profile->VideoSourceConfiguration->token = (char *)soap_malloc(soap, sizeof(char) * SMALL_INFO_LENGTH);
    strncpy(GetProf->Profile->VideoSourceConfiguration->token, g_nvt->t.profiles[i].vsc->vtoken, SMALL_INFO_LENGTH);
    GetProf->Profile->VideoSourceConfiguration->SourceToken = (char *)soap_malloc(soap, sizeof(char) * SMALL_INFO_LENGTH);
    strncpy(GetProf->Profile->VideoSourceConfiguration->SourceToken, g_nvt->t.profiles[i].vsc->vsourcetoken, SMALL_INFO_LENGTH);
    GetProf->Profile->VideoSourceConfiguration->UseCount = g_nvt->t.profiles[i].vsc->vcount;
    GetProf->Profile->VideoSourceConfiguration->Bounds = (struct tt__IntRectangle *)soap_malloc(soap, sizeof(struct tt__IntRectangle));
    memset(GetProf->Profile->VideoSourceConfiguration->Bounds, 0, sizeof(struct tt__IntRectangle));
    GetProf->Profile->VideoSourceConfiguration->Bounds->y = g_nvt->t.profiles[i].vsc->x;
    GetProf->Profile->VideoSourceConfiguration->Bounds->x = g_nvt->t.profiles[i].vsc->y;
    GetProf->Profile->VideoSourceConfiguration->Bounds->width  = g_nvt->t.profiles[i].vsc->width;
    GetProf->Profile->VideoSourceConfiguration->Bounds->height = g_nvt->t.profiles[i].vsc->heigth;

    // malloc and init for Profile  AudioSourceConfiguration
    GetProf->Profile->AudioSourceConfiguration = (struct tt__AudioSourceConfiguration *)soap_malloc(soap, sizeof(struct tt__AudioSourceConfiguration));
    memset(GetProf->Profile->AudioSourceConfiguration, 0, sizeof(struct tt__AudioSourceConfiguration));
    GetProf->Profile->AudioSourceConfiguration->Name = (char *)soap_malloc(soap, sizeof(char) * SMALL_INFO_LENGTH);
    strncpy(GetProf->Profile->AudioSourceConfiguration->Name, g_nvt->t.profiles[i].asc->aname, SMALL_INFO_LENGTH);
    GetProf->Profile->AudioSourceConfiguration->token = (char *)soap_malloc(soap, sizeof(char) * SMALL_INFO_LENGTH);
    strncpy(GetProf->Profile->AudioSourceConfiguration->token, g_nvt->t.profiles[i].asc->token, SMALL_INFO_LENGTH);
    GetProf->Profile->AudioSourceConfiguration->SourceToken = (char *)soap_malloc(soap, sizeof(char) * SMALL_INFO_LENGTH);
    strncpy(GetProf->Profile->AudioSourceConfiguration->SourceToken, g_nvt->t.profiles[i].asc->sourcetoken, SMALL_INFO_LENGTH);
    GetProf->Profile->AudioSourceConfiguration->UseCount = g_nvt->t.profiles[i].asc->count;

    // malloc and init for Profile  VideoEncoderConfiguration
    GetProf->Profile->VideoEncoderConfiguration = (struct tt__VideoEncoderConfiguration *)soap_malloc(soap, sizeof(struct tt__VideoEncoderConfiguration));
    memset(GetProf->Profile->VideoEncoderConfiguration, 0, sizeof(struct tt__VideoEncoderConfiguration));
    GetProf->Profile->VideoEncoderConfiguration->Name = (char *)soap_malloc(soap, sizeof(char) * SMALL_INFO_LENGTH);
    strncpy(GetProf->Profile->VideoEncoderConfiguration->Name, g_nvt->t.profiles[i].vec->name, SMALL_INFO_LENGTH);
    GetProf->Profile->VideoEncoderConfiguration->token = (char *)soap_malloc(soap, sizeof(char) * SMALL_INFO_LENGTH);
    strncpy(GetProf->Profile->VideoEncoderConfiguration->token, g_nvt->t.profiles[i].vec->token, SMALL_INFO_LENGTH);
    GetProf->Profile->VideoEncoderConfiguration->UseCount = g_nvt->t.profiles[i].vec->usecount;
    GetProf->Profile->VideoEncoderConfiguration->Encoding = g_nvt->t.profiles[i].vec->encoding; //JPEG = 0, MPEG4 = 1, H264 = 2;
    GetProf->Profile->VideoEncoderConfiguration->Resolution = (struct tt__VideoResolution *)soap_malloc(soap, sizeof(struct tt__VideoResolution));
    GetProf->Profile->VideoEncoderConfiguration->Resolution->Width = g_nvt->t.profiles[i].vec->width;
    GetProf->Profile->VideoEncoderConfiguration->Resolution->Height =g_nvt->t.profiles[i].vec->height;
    GetProf->Profile->VideoEncoderConfiguration->Quality = g_nvt->t.profiles[i].vec->quality;
    GetProf->Profile->VideoEncoderConfiguration->RateControl = (struct tt__VideoRateControl *)soap_malloc(soap, sizeof(struct tt__VideoRateControl));
    GetProf->Profile->VideoEncoderConfiguration->RateControl->FrameRateLimit = g_nvt->t.profiles[i].vec->FrameRateLimit;
    GetProf->Profile->VideoEncoderConfiguration->RateControl->EncodingInterval = g_nvt->t.profiles[i].vec->EncodingInterval;
    GetProf->Profile->VideoEncoderConfiguration->RateControl->BitrateLimit = g_nvt->t.profiles[i].vec->BitrateLimit;
    GetProf->Profile->VideoEncoderConfiguration->MPEG4 = (struct tt__Mpeg4Configuration *)soap_malloc(soap, sizeof(struct tt__Mpeg4Configuration));
    GetProf->Profile->VideoEncoderConfiguration->MPEG4->GovLength = g_nvt->t.profiles[i].vec->GovLength;
    GetProf->Profile->VideoEncoderConfiguration->MPEG4->Mpeg4Profile = g_nvt->t.profiles[i].vec->mProfile; //SP = 0, ASP = 1
    GetProf->Profile->VideoEncoderConfiguration->H264 = (struct tt__H264Configuration *)soap_malloc(soap, sizeof(struct tt__H264Configuration));
    GetProf->Profile->VideoEncoderConfiguration->H264->GovLength = g_nvt->t.profiles[i].vec->gLength;
    GetProf->Profile->VideoEncoderConfiguration->H264->H264Profile = g_nvt->t.profiles[i].vec->hProfile;//Baseline = 0, Main = 1, High = 3
    GetProf->Profile->VideoEncoderConfiguration->Multicast = (struct tt__MulticastConfiguration *)soap_malloc(soap, sizeof(struct tt__MulticastConfiguration));
    memset(GetProf->Profile->VideoEncoderConfiguration->Multicast, 0, sizeof(struct tt__MulticastConfiguration));
    GetProf->Profile->VideoEncoderConfiguration->Multicast->Port = 80;//nvt.t.profiles[i].vec->MultiConf.port;
    GetProf->Profile->VideoEncoderConfiguration->Multicast->TTL = g_nvt->t.profiles[i].vec->MultiConf.ttl;
    GetProf->Profile->VideoEncoderConfiguration->Multicast->AutoStart = g_nvt->t.profiles[i].vec->MultiConf.autostart;//false
    GetProf->Profile->VideoEncoderConfiguration->Multicast->Address  = (struct tt__IPAddress *)soap_malloc(soap, sizeof(struct tt__IPAddress));
    GetProf->Profile->VideoEncoderConfiguration->Multicast->Address->Type = g_nvt->t.profiles[i].vec->MultiConf.type;
    GetProf->Profile->VideoEncoderConfiguration->Multicast->Address->IPv4Address = (char **)soap_malloc(soap, sizeof(char *));
    GetProf->Profile->VideoEncoderConfiguration->Multicast->Address->IPv4Address[0] = (char *)soap_malloc(soap, sizeof(char) * INFO_LENGTH);
    strcpy(*GetProf->Profile->VideoEncoderConfiguration->Multicast->Address->IPv4Address, _IPAddr);
    GetProf->Profile->VideoEncoderConfiguration->Multicast->Address->IPv6Address = NULL;
    GetProf->Profile->VideoEncoderConfiguration->SessionTimeout = g_nvt->t.profiles[i].vec->SessionTimeout;

    // malloc and init for Profile  AudioSourceConfiguration GetProf->Profile->
    GetProf->Profile->AudioEncoderConfiguration = (struct tt__AudioEncoderConfiguration *)soap_malloc(soap, sizeof(struct tt__AudioEncoderConfiguration));
    memset(GetProf->Profile->AudioEncoderConfiguration, 0, sizeof(struct tt__AudioEncoderConfiguration));
    GetProf->Profile->AudioEncoderConfiguration->Name = (char *)soap_malloc(soap, sizeof(char) * SMALL_INFO_LENGTH);
    strncpy(GetProf->Profile->AudioEncoderConfiguration->Name, g_nvt->t.profiles[i].aec->name, SMALL_INFO_LENGTH);
    GetProf->Profile->AudioEncoderConfiguration->token = (char *)soap_malloc(soap, sizeof(char) * SMALL_INFO_LENGTH);
    strncpy(GetProf->Profile->AudioEncoderConfiguration->token, g_nvt->t.profiles[i].aec->token, SMALL_INFO_LENGTH);
    GetProf->Profile->AudioEncoderConfiguration->UseCount = g_nvt->t.profiles[i].aec->usecount;
    GetProf->Profile->AudioEncoderConfiguration->Encoding = g_nvt->t.profiles[i].aec->encoding;
    GetProf->Profile->AudioEncoderConfiguration->Bitrate = g_nvt->t.profiles[i].aec->bitrate;
    GetProf->Profile->AudioEncoderConfiguration->SampleRate= g_nvt->t.profiles[i].aec->samplerate;
    GetProf->Profile->AudioEncoderConfiguration->SessionTimeout = g_nvt->t.profiles[i].aec->SessionTimeout;
    GetProf->Profile->AudioEncoderConfiguration->Multicast = (struct tt__MulticastConfiguration *)soap_malloc(soap, sizeof(struct tt__MulticastConfiguration));
    memset(GetProf->Profile->AudioEncoderConfiguration->Multicast, 0, sizeof(struct tt__MulticastConfiguration));
    GetProf->Profile->AudioEncoderConfiguration->Multicast->Port = 80;
    GetProf->Profile->AudioEncoderConfiguration->Multicast->TTL = g_nvt->t.profiles[i].aec->MultiConf.ttl;
    GetProf->Profile->AudioEncoderConfiguration->Multicast->AutoStart = g_nvt->t.profiles[i].aec->MultiConf.autostart;
    GetProf->Profile->AudioEncoderConfiguration->Multicast->Address  = (struct tt__IPAddress *)soap_malloc(soap, sizeof(struct tt__IPAddress));
    GetProf->Profile->AudioEncoderConfiguration->Multicast->Address->Type = g_nvt->t.profiles[i].aec->MultiConf.type;
    GetProf->Profile->AudioEncoderConfiguration->Multicast->Address->IPv4Address = (char **)soap_malloc(soap, sizeof(char *));
    GetProf->Profile->AudioEncoderConfiguration->Multicast->Address->IPv4Address[0] = (char *)soap_malloc(soap, sizeof(char) * INFO_LENGTH);
    strcpy(*GetProf->Profile->AudioEncoderConfiguration->Multicast->Address->IPv4Address, _IPAddr);
    GetProf->Profile->AudioEncoderConfiguration->Multicast->Address->IPv6Address = NULL;

    // malloc and init for Profile  VideoAnalyticsConfiguration
    GetProf->Profile->VideoAnalyticsConfiguration = (struct tt__VideoAnalyticsConfiguration *) soap_malloc(soap, sizeof(struct tt__VideoAnalyticsConfiguration));
    memset(GetProf->Profile->VideoAnalyticsConfiguration, 0, sizeof(struct tt__VideoAnalyticsConfiguration));
    GetProf->Profile->VideoAnalyticsConfiguration->Name = (char *)soap_malloc(soap, sizeof(char) * SMALL_INFO_LENGTH);
    strncpy(GetProf->Profile->VideoAnalyticsConfiguration->Name, g_nvt->t.profiles[i].vec->name, SMALL_INFO_LENGTH);
    GetProf->Profile->VideoAnalyticsConfiguration->UseCount = g_nvt->t.profiles[i].vec->usecount;
    GetProf->Profile->VideoAnalyticsConfiguration->token = (char *)soap_malloc(soap, sizeof(char) * SMALL_INFO_LENGTH);
    strncpy(GetProf->Profile->VideoAnalyticsConfiguration->token, g_nvt->t.profiles[i].vec->token, SMALL_INFO_LENGTH);
    GetProf->Profile->VideoAnalyticsConfiguration->AnalyticsEngineConfiguration = (struct tt__AnalyticsEngineConfiguration *)soap_malloc(soap, sizeof(struct tt__AnalyticsEngineConfiguration));
    GetProf->Profile->VideoAnalyticsConfiguration->AnalyticsEngineConfiguration->__sizeAnalyticsModule = 0;
    GetProf->Profile->VideoAnalyticsConfiguration->AnalyticsEngineConfiguration->AnalyticsModule = NULL;
    GetProf->Profile->VideoAnalyticsConfiguration->AnalyticsEngineConfiguration->Extension = NULL;
    GetProf->Profile->VideoAnalyticsConfiguration->RuleEngineConfiguration = (struct tt__RuleEngineConfiguration *)soap_malloc(soap, sizeof(struct tt__RuleEngineConfiguration));
    GetProf->Profile->VideoAnalyticsConfiguration->RuleEngineConfiguration->__sizeRule = 0;
    GetProf->Profile->VideoAnalyticsConfiguration->RuleEngineConfiguration->Rule = NULL;
    GetProf->Profile->VideoAnalyticsConfiguration->RuleEngineConfiguration->Extension = NULL;

    // malloc and init for Profile  PTZConfiguration
    GetProf->Profile->PTZConfiguration = NULL;
    //extension
    GetProf->Profile->Extension = NULL;
    // malloc and init for Profile  MetadataConfiguration
    GetProf->Profile->MetadataConfiguration = NULL;

#endif

    return SOAP_OK;
}



SOAP_FMAC5 int SOAP_FMAC6 __trt__GetProfiles(struct soap *soap, struct _trt__GetProfiles *trt__GetProfiles, struct _trt__GetProfilesResponse *GetProfs)
{
    int i, num;
    char _IPAddr[LARGE_INFO_LENGTH];
    NET_IPV4 ip;
    char name[SMALL_INFO_LENGTH];
    char token[SMALL_INFO_LENGTH];
    char sourcetoken[SMALL_INFO_LENGTH];

    memset(name, 0, sizeof(SMALL_INFO_LENGTH));
    memset(token, 0, sizeof(SMALL_INFO_LENGTH));
    memset(sourcetoken, 0, sizeof(SMALL_INFO_LENGTH));
    
    get_dev_profiles(ALL_CHS, g_nvt->t.profiles, 2, ASC | VEC |AEC);

    num = get_curr_chs();
    num = num*2;

    ip.int32 = net_get_ifaddr(ETH_NAME);
    sprintf(_IPAddr, "%d.%d.%d.%d", ip.str[0], ip.str[1], ip.str[2], ip.str[3]);

    //malloc and init Profiles
    GetProfs->Profiles =(struct tt__Profile *)soap_malloc(soap, sizeof(struct tt__Profile) * num);
    memset(GetProfs->Profiles, 0, sizeof(struct tt__Profile) * num);
    printf("=====================>[__trt__GetProfiles] num=%d\n", num);

    GetProfs->__sizeProfiles = num;
    for(i = 0; i < num; ++i)
    {
        //init profile's name token and fixed member
        GetProfs->Profiles[i].Name = (char *)soap_malloc(soap, sizeof(char) * SMALL_INFO_LENGTH);
        GetProfs->Profiles[i].token = (char *)soap_malloc(soap, sizeof(char) * SMALL_INFO_LENGTH);
        strcpy(GetProfs->Profiles[i].Name, g_nvt->t.profiles[i].pname);
        strcpy(GetProfs->Profiles[i].token, g_nvt->t.profiles[i].ptoken);
        GetProfs->Profiles[i].fixed = (enum xsd__boolean_ *)soap_malloc(soap, sizeof(int));
        *GetProfs->Profiles[i].fixed = g_nvt->t.profiles[i].pfixed;

        // malloc and init for Profiles VideoSourceConfiguration     
        sprintf(sourcetoken, "CH%02dVideoSourceToken", i/2);
        sprintf(name, "CH%02dVideoConfigureName", i/2);
        sprintf(token, "CH%02dVideoConfigureToken", i/2);
        GetProfs->Profiles[i].VideoSourceConfiguration = (struct tt__VideoSourceConfiguration *)soap_malloc(soap, sizeof(struct tt__VideoSourceConfiguration));
        memset(GetProfs->Profiles[i].VideoSourceConfiguration, 0, sizeof(struct tt__VideoSourceConfiguration));
        GetProfs->Profiles[i].VideoSourceConfiguration->Name = (char *)soap_malloc(soap, sizeof(char) * SMALL_INFO_LENGTH);
        GetProfs->Profiles[i].VideoSourceConfiguration->token = (char *)soap_malloc(soap, sizeof(char) * SMALL_INFO_LENGTH);
        GetProfs->Profiles[i].VideoSourceConfiguration->SourceToken = (char *)soap_malloc(soap, sizeof(char) * SMALL_INFO_LENGTH);
        strcpy(GetProfs->Profiles[i].VideoSourceConfiguration->Name, name);
        strcpy(GetProfs->Profiles[i].VideoSourceConfiguration->token, token);
        strcpy(GetProfs->Profiles[i].VideoSourceConfiguration->SourceToken, sourcetoken);
        GetProfs->Profiles[i].VideoSourceConfiguration->UseCount = num;
        GetProfs->Profiles[i].VideoSourceConfiguration->Bounds = (struct tt__IntRectangle *)soap_malloc(soap, sizeof(struct tt__IntRectangle));
        memset(GetProfs->Profiles[i].VideoSourceConfiguration->Bounds, 0, sizeof(struct tt__IntRectangle));
        GetProfs->Profiles[i].VideoSourceConfiguration->Bounds->y = 0;
        GetProfs->Profiles[i].VideoSourceConfiguration->Bounds->x = 0;
        GetProfs->Profiles[i].VideoSourceConfiguration->Bounds->width  = 1280;
        GetProfs->Profiles[i].VideoSourceConfiguration->Bounds->height = 720;


        GetProfs->Profiles[i].AudioSourceConfiguration = (struct tt__AudioSourceConfiguration *)soap_malloc(soap, sizeof(struct tt__AudioSourceConfiguration));
        memset(GetProfs->Profiles[i].AudioSourceConfiguration, 0, sizeof(struct tt__AudioSourceConfiguration));
        GetProfs->Profiles[i].AudioSourceConfiguration->Name = (char *)soap_malloc(soap, sizeof(char) * SMALL_INFO_LENGTH);
        GetProfs->Profiles[i].AudioSourceConfiguration->token = (char *)soap_malloc(soap, sizeof(char) * SMALL_INFO_LENGTH);
        GetProfs->Profiles[i].AudioSourceConfiguration->SourceToken = (char *)soap_malloc(soap, sizeof(char) * SMALL_INFO_LENGTH);
        strcpy(GetProfs->Profiles[i].AudioSourceConfiguration->Name, g_nvt->t.profiles[i].asc->aname);
        strcpy(GetProfs->Profiles[i].AudioSourceConfiguration->token, g_nvt->t.profiles[i].asc->token);
        strcpy(GetProfs->Profiles[i].AudioSourceConfiguration->SourceToken, g_nvt->t.profiles[i].asc->sourcetoken);
        GetProfs->Profiles[i].AudioSourceConfiguration->UseCount = g_nvt->t.profiles[i].asc->count;

        // malloc and init for Profiles  VideoEncoderConfiguration
        //GetProfs->Profiles[i].VideoEncoderConfiguration = NULL;
#if 1
        GetProfs->Profiles[i].VideoEncoderConfiguration = (struct tt__VideoEncoderConfiguration *)soap_malloc(soap, sizeof(struct tt__VideoEncoderConfiguration));
        memset(GetProfs->Profiles[i].VideoEncoderConfiguration, 0, sizeof(struct tt__VideoEncoderConfiguration));
        GetProfs->Profiles[i].VideoEncoderConfiguration->Name = (char *)soap_malloc(soap, sizeof(char) * SMALL_INFO_LENGTH);
        GetProfs->Profiles[i].VideoEncoderConfiguration->token = (char *)soap_malloc(soap, sizeof(char) * SMALL_INFO_LENGTH);
        strcpy(GetProfs->Profiles[i].VideoEncoderConfiguration->Name, g_nvt->t.profiles[i].vec->name);
        strcpy(GetProfs->Profiles[i].VideoEncoderConfiguration->token, g_nvt->t.profiles[i].vec->token);
        GetProfs->Profiles[i].VideoEncoderConfiguration->UseCount = g_nvt->t.profiles[i].vec->usecount;
        GetProfs->Profiles[i].VideoEncoderConfiguration->Encoding = g_nvt->t.profiles[i].vec->encoding; //JPEG = 0, MPEG4 = 1, H264 = 2;
        GetProfs->Profiles[i].VideoEncoderConfiguration->Resolution = (struct tt__VideoResolution *)soap_malloc(soap, sizeof(struct tt__VideoResolution));
        GetProfs->Profiles[i].VideoEncoderConfiguration->Resolution->Width = g_nvt->t.profiles[i].vec->width;
        GetProfs->Profiles[i].VideoEncoderConfiguration->Resolution->Height =g_nvt->t.profiles[i].vec->height;
        GetProfs->Profiles[i].VideoEncoderConfiguration->Quality = g_nvt->t.profiles[i].vec->quality;
        GetProfs->Profiles[i].VideoEncoderConfiguration->RateControl = (struct tt__VideoRateControl *)soap_malloc(soap, sizeof(struct tt__VideoRateControl));
        GetProfs->Profiles[i].VideoEncoderConfiguration->RateControl->FrameRateLimit = g_nvt->t.profiles[i].vec->FrameRateLimit;
        GetProfs->Profiles[i].VideoEncoderConfiguration->RateControl->EncodingInterval = g_nvt->t.profiles[i].vec->EncodingInterval;
        GetProfs->Profiles[i].VideoEncoderConfiguration->RateControl->BitrateLimit = g_nvt->t.profiles[i].vec->BitrateLimit;
        GetProfs->Profiles[i].VideoEncoderConfiguration->MPEG4 = (struct tt__Mpeg4Configuration *)soap_malloc(soap, sizeof(struct tt__Mpeg4Configuration));
        GetProfs->Profiles[i].VideoEncoderConfiguration->MPEG4->GovLength = g_nvt->t.profiles[i].vec->GovLength;
        GetProfs->Profiles[i].VideoEncoderConfiguration->MPEG4->Mpeg4Profile = g_nvt->t.profiles[i].vec->mProfile;
        GetProfs->Profiles[i].VideoEncoderConfiguration->H264 = (struct tt__H264Configuration *)soap_malloc(soap, sizeof(struct tt__H264Configuration));
        GetProfs->Profiles[i].VideoEncoderConfiguration->H264->GovLength = g_nvt->t.profiles[i].vec->gLength;
        GetProfs->Profiles[i].VideoEncoderConfiguration->H264->H264Profile = g_nvt->t.profiles[i].vec->hProfile;//Baseline = 0, Main = 1, High = 3
        GetProfs->Profiles[i].VideoEncoderConfiguration->Multicast = (struct tt__MulticastConfiguration *)soap_malloc(soap, sizeof(struct tt__MulticastConfiguration));
        memset(GetProfs->Profiles[i].VideoEncoderConfiguration->Multicast, 0, sizeof(struct tt__MulticastConfiguration));
        GetProfs->Profiles[i].VideoEncoderConfiguration->Multicast->Port = g_nvt->p.port;//nvt.t.profiles[i].vec->MultiConf.port;
        GetProfs->Profiles[i].VideoEncoderConfiguration->Multicast->TTL = g_nvt->t.profiles[i].vec->MultiConf.ttl;
        GetProfs->Profiles[i].VideoEncoderConfiguration->Multicast->AutoStart = g_nvt->t.profiles[i].vec->MultiConf.autostart;//false
        GetProfs->Profiles[i].VideoEncoderConfiguration->Multicast->Address  = (struct tt__IPAddress *)soap_malloc(soap, sizeof(struct tt__IPAddress));
        GetProfs->Profiles[i].VideoEncoderConfiguration->Multicast->Address->Type = g_nvt->t.profiles[i].vec->MultiConf.type;
        GetProfs->Profiles[i].VideoEncoderConfiguration->Multicast->Address->IPv4Address = (char **)soap_malloc(soap, sizeof(char *));
        GetProfs->Profiles[i].VideoEncoderConfiguration->Multicast->Address->IPv4Address[0] = (char *)soap_malloc(soap, sizeof(char) * INFO_LENGTH);
        strcpy(*GetProfs->Profiles[i].VideoEncoderConfiguration->Multicast->Address->IPv4Address, _IPAddr);
        GetProfs->Profiles[i].VideoEncoderConfiguration->Multicast->Address->IPv6Address = NULL;
        GetProfs->Profiles[i].VideoEncoderConfiguration->SessionTimeout = g_nvt->t.profiles[i].vec->SessionTimeout;
#endif
        // malloc and init for Profiles  AudioSourceConfiguration
        GetProfs->Profiles[i].AudioEncoderConfiguration = (struct tt__AudioEncoderConfiguration *)soap_malloc(soap, sizeof(struct tt__AudioEncoderConfiguration));
        memset(GetProfs->Profiles[i].AudioEncoderConfiguration, 0, sizeof(struct tt__AudioEncoderConfiguration));
        GetProfs->Profiles[i].AudioEncoderConfiguration->Name = (char *)soap_malloc(soap, sizeof(char) * SMALL_INFO_LENGTH);
        GetProfs->Profiles[i].AudioEncoderConfiguration->token = (char *)soap_malloc(soap, sizeof(char) * SMALL_INFO_LENGTH);
        strcpy(GetProfs->Profiles[i].AudioEncoderConfiguration->Name, g_nvt->t.profiles[i].aec->name);
        strcpy(GetProfs->Profiles[i].AudioEncoderConfiguration->token, g_nvt->t.profiles[i].aec->token);
        GetProfs->Profiles[i].AudioEncoderConfiguration->UseCount = g_nvt->t.profiles[i].aec->usecount;
        GetProfs->Profiles[i].AudioEncoderConfiguration->Encoding = g_nvt->t.profiles[i].aec->encoding;
        GetProfs->Profiles[i].AudioEncoderConfiguration->Bitrate = g_nvt->t.profiles[i].aec->bitrate;
        GetProfs->Profiles[i].AudioEncoderConfiguration->SampleRate= g_nvt->t.profiles[i].aec->samplerate;
        GetProfs->Profiles[i].AudioEncoderConfiguration->SessionTimeout = g_nvt->t.profiles[i].aec->SessionTimeout;
        GetProfs->Profiles[i].AudioEncoderConfiguration->Multicast = (struct tt__MulticastConfiguration *)soap_malloc(soap, sizeof(struct tt__MulticastConfiguration));
        memset(GetProfs->Profiles[i].AudioEncoderConfiguration->Multicast, 0, sizeof(struct tt__MulticastConfiguration));
        GetProfs->Profiles[i].AudioEncoderConfiguration->Multicast->Port = g_nvt->p.port;
        GetProfs->Profiles[i].AudioEncoderConfiguration->Multicast->TTL = g_nvt->t.profiles[i].aec->MultiConf.ttl;
        GetProfs->Profiles[i].AudioEncoderConfiguration->Multicast->AutoStart = g_nvt->t.profiles[i].aec->MultiConf.autostart;
        GetProfs->Profiles[i].AudioEncoderConfiguration->Multicast->Address  = (struct tt__IPAddress *)soap_malloc(soap, sizeof(struct tt__IPAddress));
        GetProfs->Profiles[i].AudioEncoderConfiguration->Multicast->Address->Type = g_nvt->t.profiles[i].aec->MultiConf.type;
        GetProfs->Profiles[i].AudioEncoderConfiguration->Multicast->Address->IPv4Address = (char **)soap_malloc(soap, sizeof(char *));
        GetProfs->Profiles[i].AudioEncoderConfiguration->Multicast->Address->IPv4Address[0] = (char *)soap_malloc(soap, sizeof(char) * INFO_LENGTH);
        strcpy(*GetProfs->Profiles[i].AudioEncoderConfiguration->Multicast->Address->IPv4Address, _IPAddr);
        GetProfs->Profiles[i].AudioEncoderConfiguration->Multicast->Address->IPv6Address = NULL;
        
        // malloc and init for Profiles  VideoAnalyticsConfiguration
        GetProfs->Profiles[i].VideoAnalyticsConfiguration = NULL;
#if 0
        GetProfs->Profiles[i].VideoAnalyticsConfiguration = (struct tt__VideoAnalyticsConfiguration *) soap_malloc(soap, sizeof(struct tt__VideoAnalyticsConfiguration));
        memset(GetProfs->Profiles[i].VideoAnalyticsConfiguration, 0, sizeof(struct tt__VideoAnalyticsConfiguration));
        GetProfs->Profiles[i].VideoAnalyticsConfiguration->Name = (char *)soap_malloc(soap, sizeof(char) * SMALL_INFO_LENGTH);
        strncpy(GetProfs->Profiles[i].VideoAnalyticsConfiguration->Name, g_nvt->t.profiles[i].vec->name, SMALL_INFO_LENGTH);
        GetProfs->Profiles[i].VideoAnalyticsConfiguration->UseCount = g_nvt->t.profiles[i].vec->usecount;
        GetProfs->Profiles[i].VideoAnalyticsConfiguration->token = (char *)soap_malloc(soap, sizeof(char) * SMALL_INFO_LENGTH);
        strncpy(GetProfs->Profiles[i].VideoAnalyticsConfiguration->token, g_nvt->t.profiles[i].vec->token, SMALL_INFO_LENGTH);
        GetProfs->Profiles[i].VideoAnalyticsConfiguration->AnalyticsEngineConfiguration = (struct tt__AnalyticsEngineConfiguration *)soap_malloc(soap, sizeof(struct tt__AnalyticsEngineConfiguration));
        GetProfs->Profiles[i].VideoAnalyticsConfiguration->AnalyticsEngineConfiguration->__sizeAnalyticsModule = 0;
        GetProfs->Profiles[i].VideoAnalyticsConfiguration->AnalyticsEngineConfiguration->AnalyticsModule = NULL;
        GetProfs->Profiles[i].VideoAnalyticsConfiguration->AnalyticsEngineConfiguration->Extension = NULL;
        GetProfs->Profiles[i].VideoAnalyticsConfiguration->RuleEngineConfiguration = (struct tt__RuleEngineConfiguration *)soap_malloc(soap, sizeof(struct tt__RuleEngineConfiguration));
        GetProfs->Profiles[i].VideoAnalyticsConfiguration->RuleEngineConfiguration->__sizeRule = 0;
        GetProfs->Profiles[i].VideoAnalyticsConfiguration->RuleEngineConfiguration->Rule = NULL;
        GetProfs->Profiles[i].VideoAnalyticsConfiguration->RuleEngineConfiguration->Extension = NULL;
#endif
        // malloc and init for Profiles  PTZConfiguration
        GetProfs->Profiles[i].PTZConfiguration = (struct tt__PTZConfiguration *)soap_malloc(soap, sizeof(struct tt__PTZConfiguration));
        memset(GetProfs->Profiles[i].PTZConfiguration, 0, sizeof(struct tt__PTZConfiguration));
        ptz_cfg_get(soap, i, GetProfs->Profiles[i].PTZConfiguration);
        
        // malloc and init for Profiles  MetadataConfiguration
        GetProfs->Profiles[i].MetadataConfiguration = NULL;
#if 0
        GetProfs->Profiles[i].MetadataConfiguration = (struct tt__MetadataConfiguration *)soap_malloc(soap,sizeof(struct tt__MetadataConfiguration));
        memset(GetProfs->Profiles[i].MetadataConfiguration, 0, sizeof(struct tt__MetadataConfiguration));
        GetProfs->Profiles[i].MetadataConfiguration->Name = g_nvt->t.profiles[i].metac->name;
        GetProfs->Profiles[i].MetadataConfiguration->UseCount = g_nvt->t.profiles[i].metac->usecount;
        GetProfs->Profiles[i].MetadataConfiguration->token = g_nvt->t.profiles[i].metac->token;
        GetProfs->Profiles[i].MetadataConfiguration->Analytics = (enum xsd__boolean_ *)soap_malloc(soap, sizeof(char) * sizeof(enum xsd__boolean_));
        *GetProfs->Profiles[i].MetadataConfiguration->Analytics = g_nvt->t.profiles[i].metac->analytics;
        GetProfs->Profiles[i].MetadataConfiguration->SessionTimeout = g_nvt->t.profiles[i].metac->sessionTimeout;
        GetProfs->Profiles[i].MetadataConfiguration->PTZStatus = NULL;
        GetProfs->Profiles[i].MetadataConfiguration->Events = NULL;
        GetProfs->Profiles[i].MetadataConfiguration->Multicast = (struct tt__MulticastConfiguration *)soap_malloc(soap, sizeof(struct tt__MulticastConfiguration));
        memset(GetProfs->Profiles[i].MetadataConfiguration->Multicast, 0, sizeof(struct tt__MulticastConfiguration));
        GetProfs->Profiles[i].MetadataConfiguration->Multicast->Port = g_nvt->t.profiles[i].metac->multiconf.port;
        GetProfs->Profiles[i].MetadataConfiguration->Multicast->TTL = g_nvt->t.profiles[i].metac->multiconf.ttl;
        GetProfs->Profiles[i].MetadataConfiguration->Multicast->AutoStart = g_nvt->t.profiles[i].metac->multiconf.autostart;
        GetProfs->Profiles[i].MetadataConfiguration->Multicast->Address  = (struct tt__IPAddress *)soap_malloc(soap, sizeof(struct tt__IPAddress));
        memset(GetProfs->Profiles[i].MetadataConfiguration->Multicast->Address, 0, sizeof(struct tt__IPAddress));
        GetProfs->Profiles[i].MetadataConfiguration->Multicast->Address->Type = tt__IPType__IPv4;
        GetProfs->Profiles[i].MetadataConfiguration->Multicast->Address->IPv4Address = (char **)soap_malloc(soap, sizeof(char) * sizeof(char *));
        GetProfs->Profiles[i].MetadataConfiguration->Multicast->Address->IPv4Address[0] = (char *)soap_malloc(soap, sizeof(char) * LARGE_INFO_LENGTH);
        strcpy(*GetProfs->Profiles[i].MetadataConfiguration->Multicast->Address->IPv4Address, _IPAddr);
        GetProfs->Profiles[i].MetadataConfiguration->Multicast->Address->IPv6Address = NULL;
#endif
        //extension
        GetProfs->Profiles[i].Extension = NULL;
    }

    printf("=====================>[__trt__GetProfiles] SOAP_OK\n");
    return SOAP_OK;
}

SOAP_FMAC5 int SOAP_FMAC6 __trt__CreateProfile(struct soap *soap, struct _trt__CreateProfile *input, struct _trt__CreateProfileResponse *CreateProfile)
{
    int i;
    int num = 0;
    char name[SMALL_INFO_LENGTH];
    char token[SMALL_INFO_LENGTH];

    if(input->Token)
    {
        for(i = 0; i < g_nvt->t.nprofiles; i++)
        {
            if(strcmp(input->Token, g_nvt->t.profiles[i].ptoken)==0)
            {
                return SOAP_FAULT;
            }
        }
    }
    num = g_nvt->t.nprofiles + 1;

    if(num > MAX_PROFILES)
    {
        return SOAP_FAULT;
    }

#if 0
    if (!input->Token)
    {
        memset(token, 0, sizeof(token));
        sprintf(token,  "TOKEN_%d", num);
        g_nvt->t.profiles[num-1].ptoken = token;
    }
    else
    {
        g_nvt->t.profiles[num-1].ptoken = input->Token;
    }

    if(input->Name)
    {
        g_nvt->t.profiles[num-1].pname = input->Name;
    }
    else
    {
        memset(name, 0, sizeof(name));
        sprintf(name,  "NAME_%d", num);
        g_nvt->t.profiles[num-1].pname = name;
    }
#endif
    strcpy(g_nvt->t.profiles[num-1].ptoken, "token");
    strcpy(g_nvt->t.profiles[num-1].pname, input->Name);
    //g_nvt->t.profiles[num-1].ptoken = "token";
    //g_nvt->t.profiles[num-1].pname = input->Name;

    CreateProfile->Profile = (struct tt__Profile *)soap_malloc(soap, sizeof(struct tt__Profile));
    memset(CreateProfile->Profile, 0, sizeof(struct tt__Profile));

    CreateProfile->Profile->Name = input->Name;//g_nvt->t.profiles[num-1].pname;
    CreateProfile->Profile->token = "token";//g_nvt->t.profiles[num-1].ptoken;
    CreateProfile->Profile->VideoSourceConfiguration = NULL;
    CreateProfile->Profile->AudioSourceConfiguration = NULL;
    CreateProfile->Profile->VideoEncoderConfiguration = NULL;
    CreateProfile->Profile->AudioEncoderConfiguration = NULL;
    CreateProfile->Profile->VideoAnalyticsConfiguration = NULL;
    CreateProfile->Profile->PTZConfiguration = NULL;
    CreateProfile->Profile->MetadataConfiguration = NULL;
    CreateProfile->Profile->Extension = NULL;
    CreateProfile->Profile->fixed = NULL;
    CreateProfile->Profile->__anyAttribute = NULL;

    ++g_nvt->t.nprofiles;
    return SOAP_OK;
}

SOAP_FMAC5 int SOAP_FMAC6 __trt__GetStreamUri(struct soap *soap, struct _trt__GetStreamUri *trt__GetStreamUri, struct _trt__GetStreamUriResponse *StreamUri)
{
    int i = 0;
    char _IPAddr[INFO_LENGTH];
    char _Path[INFO_LENGTH];
    int flag = 0;

    if(nvt_wsse_auth(soap) < 0)
    {
      return SOAP_FAULT;
    }

    //get_dev_profiles(ALL_CHS, g_nvt->t.profiles, 2, VSC | ASC | VEC |AEC);

    for(i = 0; i < g_nvt->t.nprofiles; i++)
    {
        //printf("[%s]=========> g_nvt->t.profiles[%d].ptoken=%s\n", __FUNCTION__, i, g_nvt->t.profiles[i].ptoken);
        if(strcmp(trt__GetStreamUri->ProfileToken, g_nvt->t.profiles[i].ptoken) != 0)
        {
            continue;
        }
        else
        {
            flag = 1;
            break;
        }
    }

    if(flag == 0)
    {
        return SOAP_FAULT;
    }

    video_source_conf_t *info;
    info = (video_source_conf_t *)calloc(1, sizeof(video_source_conf_t));

    get_vsc_profiles(i, info, 2);

    if (trt__GetStreamUri->StreamSetup == NULL || trt__GetStreamUri->StreamSetup->Stream == tt__StreamType__RTP_Unicast)
    {
        sprintf(_IPAddr, "%s", info->uri);
    }
    else
    {
        if(trt__GetStreamUri->StreamSetup->Stream == tt__StreamType__RTP_Multicast)
        {
            if (NULL != trt__GetStreamUri->StreamSetup->Transport
                    &&trt__GetStreamUri->StreamSetup->Transport->Protocol == 0)
            {
                sprintf(_IPAddr, "%s", info->uri);
            }
            else
            {
                return SOAP_FAULT;
            }
        }
    }

    /* Response */
    StreamUri->MediaUri = (struct tt__MediaUri *)soap_malloc(soap, sizeof(struct tt__MediaUri));
    memset(StreamUri->MediaUri, 0, sizeof(struct tt__MediaUri));
    StreamUri->MediaUri->Uri = (char *)soap_malloc(soap, sizeof(char) * LARGE_INFO_LENGTH);
    strcpy(StreamUri->MediaUri->Uri, _IPAddr);
    StreamUri->MediaUri->InvalidAfterConnect = __false;
    StreamUri->MediaUri->InvalidAfterReboot = __false;
    StreamUri->MediaUri->Timeout = 0;
    StreamUri->MediaUri->__size = 1;

    if(info)
    {
        free(info);
        info = NULL;
    }
    printf("[%s]=========> out\n", __FUNCTION__);
    return SOAP_OK;
}

SOAP_FMAC5 int SOAP_FMAC6 __trt__GetVideoSources(struct soap *soap, struct _trt__GetVideoSources *trt__GetVideoSources, struct _trt__GetVideoSourcesResponse *GetVideoS)
{
    int i, t, p;
    char sourcetoken[SMALL_INFO_LENGTH];
    memset(sourcetoken, 0, sizeof(SMALL_INFO_LENGTH));
    //get_dev_profiles(ALL_CHS, g_nvt->t.profiles, 2, VSC);
    t = get_curr_chs();

    GetVideoS->VideoSources = (struct tt__VideoSource *)soap_malloc(soap, (t) * sizeof(struct tt__VideoSource));
    memset(GetVideoS->VideoSources, 0, (t) * sizeof(struct tt__VideoSource));
    GetVideoS->__sizeVideoSources = t;
    t = t*2;
    for(i = 0; i < t; i++)
    {
#if 1
        if(i%2)
            continue;
#endif
        p = i/2;
        GetVideoS->VideoSources[p].token = (char *)soap_malloc(soap, sizeof(char) * SMALL_INFO_LENGTH);
        sprintf(sourcetoken, "CH%02dVideoSourceToken", p);
        strcpy(GetVideoS->VideoSources[p].token, sourcetoken);
        GetVideoS->VideoSources[p].Framerate = 30;
        GetVideoS->VideoSources[p].Resolution = (struct tt__VideoResolution *)soap_malloc(soap, sizeof(struct tt__VideoResolution));
        GetVideoS->VideoSources[p].Resolution->Width = 1280;
        GetVideoS->VideoSources[p].Resolution->Height = 720;
        GetVideoS->VideoSources[p].Imaging = NULL;
        GetVideoS->VideoSources[p].Extension = NULL;
        GetVideoS->VideoSources[p].__anyAttribute = NULL;
    }

    return SOAP_OK;
}

SOAP_FMAC5 int SOAP_FMAC6 __trt__GetVideoSourceConfigurations(struct soap *soap, struct _trt__GetVideoSourceConfigurations *trt__GetVideoSourceConfigurations, struct _trt__GetVideoSourceConfigurationsResponse *GetVSCs)
{
    int i, t;
    char name[SMALL_INFO_LENGTH];
    char token[SMALL_INFO_LENGTH];
    char sourcetoken[SMALL_INFO_LENGTH];

    memset(name, 0, sizeof(SMALL_INFO_LENGTH));
    memset(token, 0, sizeof(SMALL_INFO_LENGTH));
    memset(sourcetoken, 0, sizeof(SMALL_INFO_LENGTH));

    //get_dev_profiles(ALL_CHS, g_nvt->t.profiles, 2, VSC);
    t = get_curr_chs();
    t = t*2;

    GetVSCs->Configurations = (struct tt__VideoSourceConfiguration*)soap_malloc(soap,(t * sizeof(struct tt__VideoSourceConfiguration)));
    memset(GetVSCs->Configurations, 0, t * sizeof(struct tt__VideoSourceConfiguration));

    for(i = 0; i < t; i++)
    {
        sprintf(sourcetoken, "CH%2d_VideoSourceToken", i/2);
        sprintf(name, "CH%02dVideoConfigureName", i/2);
        sprintf(token, "CH%02dVideoConfigureToken", i/2);
        GetVSCs->Configurations[i].Name = (char *)soap_malloc(soap, sizeof(char) * SMALL_INFO_LENGTH);
        GetVSCs->Configurations[i].token = (char *)soap_malloc(soap, sizeof(char) * SMALL_INFO_LENGTH);
        GetVSCs->Configurations[i].SourceToken = (char *)soap_malloc(soap, sizeof(char) * SMALL_INFO_LENGTH);        

        strcpy(GetVSCs->Configurations[i].Name, name);
        strcpy(GetVSCs->Configurations[i].token, token);
        strcpy(GetVSCs->Configurations[i].SourceToken, sourcetoken);   
        GetVSCs->Configurations[i].UseCount = t;
        GetVSCs->Configurations[i].Bounds = (struct tt__IntRectangle *)soap_malloc(soap, sizeof(struct tt__IntRectangle));
        GetVSCs->Configurations[i].Bounds->x = 0;
        GetVSCs->Configurations[i].Bounds->y = 0;
        GetVSCs->Configurations[i].Bounds->width = 1280;
        GetVSCs->Configurations[i].Bounds->height = 720;
    }
    GetVSCs->__sizeConfigurations = i;

    return SOAP_OK;
}

SOAP_FMAC5 int SOAP_FMAC6 __trt__GetVideoSourceConfiguration(struct soap *soap, struct _trt__GetVideoSourceConfiguration *input, struct _trt__GetVideoSourceConfigurationResponse *GetVSC)
{
    int i;
    int index = -1;
    int tokens = 0;

    for(i = 0; i < g_nvt->t.nprofiles; i++)
    {
        if(strcmp(input->ConfigurationToken, g_nvt->t.profiles[i].vsc->vtoken) == 0)
        {
            index = i;
            tokens++;
        }
    }
    if(i < 0)
        return SOAP_FAULT;

    video_source_conf_t *info;
    info = (video_source_conf_t *)calloc(1, sizeof(video_source_conf_t));
    get_vsc_profiles(index, info, 2);
    GetVSC->Configuration = (struct tt__VideoSourceConfiguration*)soap_malloc(soap,sizeof(struct tt__VideoSourceConfiguration));
    memset(GetVSC->Configuration, 0, sizeof(struct tt__VideoSourceConfiguration));
    GetVSC->Configuration->Name = (char *)soap_malloc(soap, sizeof(char) * SMALL_INFO_LENGTH);
    strncpy(GetVSC->Configuration->Name, info->vname, SMALL_INFO_LENGTH);
    GetVSC->Configuration->UseCount = info->vcount;
    GetVSC->Configuration->token = (char *)soap_malloc(soap, sizeof(char) * SMALL_INFO_LENGTH);
    strncpy(GetVSC->Configuration->token, info->vtoken, SMALL_INFO_LENGTH);
    GetVSC->Configuration->SourceToken = (char *)soap_malloc(soap, sizeof(char) * SMALL_INFO_LENGTH);
    strncpy(GetVSC->Configuration->SourceToken, info->vsourcetoken, SMALL_INFO_LENGTH);
    GetVSC->Configuration->Bounds = (struct tt__IntRectangle *)soap_malloc(soap, sizeof(struct tt__IntRectangle));
    GetVSC->Configuration->Bounds->x = info->x;
    GetVSC->Configuration->Bounds->y = info->y;
    GetVSC->Configuration->Bounds->width = info->width;
    GetVSC->Configuration->Bounds->height = info->heigth;
    if(info)
    {
        free(info);
        info = NULL;
    }
    return SOAP_OK;
}

static int firstGetVECFlag;
static time_t lastGetVECTm;

SOAP_FMAC5 int SOAP_FMAC6 __trt__GetVideoEncoderConfigurations(struct soap *soap, struct _trt__GetVideoEncoderConfigurations *trt__GetVideoEncoderConfigurations, struct _trt__GetVideoEncoderConfigurationsResponse *GetVECs)
{
    int i;
    char _IPAddr[LARGE_INFO_LENGTH];
    NET_IPV4 ip;

    ip.int32 =  net_get_ifaddr(ETH_NAME);
    sprintf(_IPAddr, "%d.%d.%d.%d", ip.str[0], ip.str[1], ip.str[2], ip.str[3]);

#if 1    
        time_t currTime;
        if(firstGetVECFlag == 1)
        {
            lastGetVECTm = time(((time_t *)NULL));
            firstGetVECFlag = 0;
            get_dev_profiles(ALL_CHS, g_nvt->t.profiles, 2, VEC);
        }
        else
        {
            currTime = time(((time_t *)NULL));      
            if((currTime - lastGetVECTm) > 6*60)
            {
                lastGetVECTm = currTime;
                get_dev_profiles(ALL_CHS, g_nvt->t.profiles, 2, VEC);
            }
        }
#endif 
    //get_dev_profiles(ALL_CHS, g_nvt->t.profiles, 2, VEC);

    int t = get_curr_chs();
    t = t*2;

    GetVECs->Configurations = (struct tt__VideoEncoderConfiguration*)soap_malloc(soap, (t * sizeof(struct tt__VideoEncoderConfiguration)));
    memset(GetVECs->Configurations, 0, t * sizeof(struct tt__VideoEncoderConfiguration));

    for(i = 0; i < t; i++)
    {
        GetVECs->Configurations[i].Name = (char *)soap_malloc(soap, sizeof(char) * SMALL_INFO_LENGTH);
        strcpy(GetVECs->Configurations[i].Name, g_nvt->t.profiles[i].vec->name);
        GetVECs->Configurations[i].UseCount = g_nvt->t.profiles[i].vec->usecount;
        GetVECs->Configurations[i].token = (char *)soap_malloc(soap, sizeof(char) * SMALL_INFO_LENGTH);
        strcpy(GetVECs->Configurations[i].token, g_nvt->t.profiles[i].vec->token);
        GetVECs->Configurations[i].Encoding = g_nvt->t.profiles[i].vec->encoding;
        GetVECs->Configurations[i].H264 = (struct tt__H264Configuration *)soap_malloc(soap, sizeof(struct tt__H264Configuration));
        GetVECs->Configurations[i].H264->GovLength = g_nvt->t.profiles[i].vec->gLength;
        GetVECs->Configurations[i].H264->H264Profile = g_nvt->t.profiles[i].vec->hProfile;
        GetVECs->Configurations[i].Quality = g_nvt->t.profiles[i].vec->quality;
        GetVECs->Configurations[i].RateControl = (struct tt__VideoRateControl *)soap_malloc(soap, sizeof(struct tt__VideoRateControl));
        GetVECs->Configurations[i].RateControl->BitrateLimit = g_nvt->t.profiles[i].vec->BitrateLimit;
        GetVECs->Configurations[i].RateControl->EncodingInterval = g_nvt->t.profiles[i].vec->EncodingInterval;
        GetVECs->Configurations[i].RateControl->FrameRateLimit = g_nvt->t.profiles[i].vec->FrameRateLimit;

        GetVECs->Configurations[i].Resolution = (struct tt__VideoResolution *)soap_malloc(soap, sizeof(struct tt__VideoResolution));
        GetVECs->Configurations[i].Resolution->Width = g_nvt->t.profiles[i].vec->width;
        GetVECs->Configurations[i].Resolution->Height = g_nvt->t.profiles[i].vec->height;

        GetVECs->Configurations[i].Multicast = (struct tt__MulticastConfiguration *)soap_malloc(soap, sizeof(struct tt__MulticastConfiguration));
        memset(GetVECs->Configurations[i].Multicast, 0, sizeof(struct tt__MulticastConfiguration));
        GetVECs->Configurations[i].Multicast->Port= g_nvt->t.profiles[i].vec->MultiConf.port;
        GetVECs->Configurations[i].Multicast->TTL= g_nvt->t.profiles[i].vec->MultiConf.ttl;
        GetVECs->Configurations[i].Multicast->AutoStart = g_nvt->t.profiles[i].vec->MultiConf.autostart;
        GetVECs->Configurations[i].Multicast->Address  = (struct tt__IPAddress *)soap_malloc(soap, sizeof(struct tt__IPAddress));
        GetVECs->Configurations[i].Multicast->Address->Type = 0;
        GetVECs->Configurations[i].Multicast->Address->IPv4Address = (char **)soap_malloc(soap, sizeof(char *));
        GetVECs->Configurations[i].Multicast->Address->IPv4Address[0] = (char *)soap_malloc(soap, sizeof(char) * INFO_LENGTH);
        strcpy(GetVECs->Configurations[i].Multicast->Address->IPv4Address[0], _IPAddr);
        GetVECs->Configurations[i].Multicast->Address->IPv6Address = NULL;

        GetVECs->Configurations[i].MPEG4 = NULL;
    }
    GetVECs->__sizeConfigurations = i;

    return SOAP_OK;
}

SOAP_FMAC5 int SOAP_FMAC6 __trt__GetVideoEncoderConfiguration(struct soap *soap, struct _trt__GetVideoEncoderConfiguration *input, struct _trt__GetVideoEncoderConfigurationResponse *GetVEC)
{
    int i;
    char _IPAddr[LARGE_INFO_LENGTH];
    NET_IPV4 ip;
    int index = -1;
    int tokens = 0;

    for(i = 0; i < g_nvt->t.nprofiles; i++)
    {
        assert(g_nvt->t.profiles[i].vec->token != NULL);
        if(strcmp(input->ConfigurationToken, g_nvt->t.profiles[i].vec->token) == 0)
        {
            tokens++;
            index = i;
        }
    }

    if(i < 0)
        return SOAP_FAULT;

    ip.int32 =  net_get_ifaddr(ETH_NAME);
    sprintf(_IPAddr, "%d.%d.%d.%d", ip.str[0], ip.str[1], ip.str[2], ip.str[3]);

    video_encoder_conf_t * info = (video_encoder_conf_t *)calloc(1, sizeof(video_encoder_conf_t));
    get_vec_profiles(index, info, 2);
    GetVEC->Configuration = (struct tt__VideoEncoderConfiguration*)soap_malloc(soap, sizeof(struct tt__VideoEncoderConfiguration));
    memset(GetVEC->Configuration, 0, sizeof(struct tt__VideoEncoderConfiguration));
    GetVEC->Configuration->Name = (char *)soap_malloc(soap, sizeof(char) * SMALL_INFO_LENGTH);
    strncpy(GetVEC->Configuration->Name, info->name, SMALL_INFO_LENGTH);
    GetVEC->Configuration->UseCount = tokens;
    GetVEC->Configuration->token = (char *)soap_malloc(soap, sizeof(char) * SMALL_INFO_LENGTH);
    strncpy(GetVEC->Configuration->token, info->token, SMALL_INFO_LENGTH);
    GetVEC->Configuration->Encoding = info->encoding;
    GetVEC->Configuration->H264 = (struct tt__H264Configuration *)soap_malloc(soap, sizeof(struct tt__H264Configuration));
    GetVEC->Configuration->H264->GovLength = info->gLength;
    GetVEC->Configuration->H264->H264Profile = info->hProfile;
    GetVEC->Configuration->Quality = info->quality;
    GetVEC->Configuration->RateControl = (struct tt__VideoRateControl *)soap_malloc(soap, sizeof(struct tt__VideoRateControl));
    GetVEC->Configuration->RateControl->BitrateLimit = info->BitrateLimit;
    GetVEC->Configuration->RateControl->EncodingInterval = info->EncodingInterval;
    GetVEC->Configuration->RateControl->FrameRateLimit = info->FrameRateLimit;
    GetVEC->Configuration->Resolution = (struct tt__VideoResolution *)soap_malloc(soap, sizeof(struct tt__VideoResolution));
    GetVEC->Configuration->Resolution->Width = info->width;
    GetVEC->Configuration->Resolution->Height = info->height;
    GetVEC->Configuration->Multicast = (struct tt__MulticastConfiguration *)soap_malloc(soap, sizeof(struct tt__MulticastConfiguration));
    memset(GetVEC->Configuration->Multicast, 0, sizeof(struct tt__MulticastConfiguration));
    GetVEC->Configuration->Multicast->Port= info->MultiConf.port;
    GetVEC->Configuration->Multicast->TTL= info->MultiConf.ttl;
    GetVEC->Configuration->Multicast->AutoStart = info->MultiConf.autostart;
    GetVEC->Configuration->Multicast->Address  = (struct tt__IPAddress *)soap_malloc(soap, sizeof(struct tt__IPAddress));
    GetVEC->Configuration->Multicast->Address->Type = 0;
    GetVEC->Configuration->Multicast->Address->IPv4Address = (char **)soap_malloc(soap, sizeof(char *));
    GetVEC->Configuration->Multicast->Address->IPv4Address[0] = (char *)soap_malloc(soap, sizeof(char) * INFO_LENGTH);
    strcpy(GetVEC->Configuration->Multicast->Address->IPv4Address[0], _IPAddr);
    GetVEC->Configuration->Multicast->Address->IPv6Address = NULL;
    GetVEC->Configuration->MPEG4 = NULL;

    return SOAP_OK;
}

SOAP_FMAC5 int SOAP_FMAC6 __trt__GetVideoEncoderConfigurationOptions(struct
soap *soap, struct _trt__GetVideoEncoderConfigurationOptions *input, struct _trt__GetVideoEncoderConfigurationOptionsResponse *GetVECops)
{
    int i, stream_num = 0;
    int encodetype = 2; //set default value is H264
    char PToken[SMALL_INFO_LENGTH];
    char CToken[SMALL_INFO_LENGTH];
    int streamtype = 0; //set default value is MAIN FRAME

    GetVECops->Options = (struct tt__VideoEncoderConfigurationOptions*)soap_malloc(soap,sizeof(struct tt__VideoEncoderConfigurationOptions));
    memset(GetVECops->Options, 0,  sizeof(struct tt__VideoEncoderConfigurationOptions));
    GetVECops->Options->H264 = NULL;
    GetVECops->Options->JPEG = NULL;
    GetVECops->Options->MPEG4 = NULL;
    GetVECops->Options->QualityRange = (struct tt__IntRange*)soap_malloc(soap, sizeof(struct tt__IntRange));
    GetVECops->Options->QualityRange->Max = 5;
    GetVECops->Options->QualityRange->Min = 0;

    if(input->ProfileToken)
    {
        strcpy(PToken, input->ProfileToken);
    }

    if(input->ConfigurationToken)
    {
        strcpy(CToken, input->ConfigurationToken);
    }
    
    int t = get_curr_chs();
    //get_dev_profiles(ALL_CHS, g_nvt->t.profiles, 2, VEC);
    t = t*2;
    for(i  = 0; i < t; i++)
    {
    #if 0
        printf("[%s]==========> g_nvt->t.profiles[%d].ptoken=%s g_nvt->t.profiles[%d].vec->token=%s\n"
                ,__FUNCTION__
                ,i
                ,g_nvt->t.profiles[i].ptoken
                ,i
                ,g_nvt->t.profiles[i].vec->token);
    #endif           
        if((strcmp(PToken, g_nvt->t.profiles[i].ptoken) == 0)
                ||(strcmp(CToken, g_nvt->t.profiles[i].vec->token) ==0))
        {
            if(strcmp(PToken, g_nvt->t.profiles[i].ptoken) == 0)
                stream_num = i;
            break;
        }
    }
#if 1
    printf("[%s]==========> stream_num=%d\n",__FUNCTION__,stream_num);
#endif

    if(strstr(PToken, "Sub") != NULL)
        streamtype = SUB_STREAM;
    else
        streamtype = MAIN_STREAM;
#if 1
    video_encoder_conf_t *info = (video_encoder_conf_t *)calloc(1, sizeof(video_encoder_conf_t));
    get_vec_profiles(stream_num, info, 2);
    encodetype = info->encoding;
#endif
    
    //encodetype = g_nvt->t.profiles[stream_num].vec->encoding;
    printf("[%s]==========> g_nvt->t.profiles[stream_num].vec->encoding=%d\n", __FUNCTION__, encodetype);

    if(encodetype == 0)
    {
        GetVECops->Options->JPEG = (struct tt__JpegOptions *)soap_malloc(soap, sizeof(struct tt__JpegOptions));
        GetVECops->Options->JPEG->__sizeResolutionsAvailable = 3;
        GetVECops->Options->JPEG->ResolutionsAvailable = (struct tt__VideoResolution *)soap_malloc(soap, 3 * sizeof(struct tt__VideoResolution));
        GetVECops->Options->JPEG->ResolutionsAvailable[0].Width  = 1600;
        GetVECops->Options->JPEG->ResolutionsAvailable[0].Height = 1200;
        GetVECops->Options->JPEG->ResolutionsAvailable[1].Width  = 2048;
        GetVECops->Options->JPEG->ResolutionsAvailable[1].Height = 1536;
        GetVECops->Options->JPEG->ResolutionsAvailable[2].Width  = 2592;
        GetVECops->Options->JPEG->ResolutionsAvailable[2].Height = 1920;
        GetVECops->Options->JPEG->FrameRateRange = (struct tt__IntRange*)soap_malloc(soap, sizeof(struct tt__IntRange));
        GetVECops->Options->JPEG->FrameRateRange->Min = MIN_FRAMERATE;
        GetVECops->Options->JPEG->FrameRateRange->Max = MAX_FRAMERATE;
        GetVECops->Options->JPEG->EncodingIntervalRange = (struct tt__IntRange*)soap_malloc(soap, sizeof(struct tt__IntRange));
        GetVECops->Options->JPEG->EncodingIntervalRange->Min= MIN_INTERVAL;
        GetVECops->Options->JPEG->EncodingIntervalRange->Max = MAX_INTERVAL;
    }

    if(encodetype == 1)
    {
        GetVECops->Options->MPEG4 = (struct tt__Mpeg4Options*)soap_malloc(soap, sizeof(struct tt__Mpeg4Options));
        GetVECops->Options->MPEG4->__sizeResolutionsAvailable = 4;
        GetVECops->Options->MPEG4->ResolutionsAvailable = (struct tt__VideoResolution*)soap_malloc(soap, sizeof(struct tt__VideoResolution) *4);
        GetVECops->Options->MPEG4->ResolutionsAvailable[0].Width = 1280;
        GetVECops->Options->MPEG4->ResolutionsAvailable[0].Height = 720;
        GetVECops->Options->MPEG4->ResolutionsAvailable[1].Width = 720;
        GetVECops->Options->MPEG4->ResolutionsAvailable[1].Height = 480;
        GetVECops->Options->MPEG4->ResolutionsAvailable[2].Width = 1280;
        GetVECops->Options->MPEG4->ResolutionsAvailable[2].Height = 960;
        GetVECops->Options->MPEG4->ResolutionsAvailable[3].Width = 1920;
        GetVECops->Options->MPEG4->ResolutionsAvailable[3].Height = 1080;
        GetVECops->Options->MPEG4->GovLengthRange = (struct tt__IntRange*)soap_malloc(soap, sizeof(struct tt__IntRange));
        GetVECops->Options->MPEG4->GovLengthRange->Max = MAX_GOVLEN;
        GetVECops->Options->MPEG4->GovLengthRange->Min = MIN_GOVLEN;
        GetVECops->Options->MPEG4->FrameRateRange = (struct tt__IntRange*)soap_malloc(soap, sizeof(struct tt__IntRange));
        GetVECops->Options->MPEG4->FrameRateRange->Max = MAX_FRAMERATE;
        GetVECops->Options->MPEG4->FrameRateRange->Min = MIN_FRAMERATE;
        GetVECops->Options->MPEG4->EncodingIntervalRange= (struct tt__IntRange*)soap_malloc(soap, sizeof(struct tt__IntRange));
        GetVECops->Options->MPEG4->EncodingIntervalRange->Max = MAX_INTERVAL;
        GetVECops->Options->MPEG4->EncodingIntervalRange->Min = MIN_INTERVAL;
        GetVECops->Options->MPEG4->__sizeMpeg4ProfilesSupported= 1;
        GetVECops->Options->MPEG4->Mpeg4ProfilesSupported = tt__Mpeg4Profile__SP;
    }

    if(encodetype == 2)
    {
        g_nvt->t.device_cap = (device_capability_t *)calloc(1, sizeof(device_capability_t));
        get_dev_capability(g_nvt->t.device_cap, streamtype);

        int i, j;
        for(j = 0; j < 4/*maohw max resolution num*/; j++)
        {
            if(!g_nvt->t.device_cap->videostream[stream_num].reswidth[0][j])
                break;
        }

        GetVECops->Options->H264 = (struct tt__H264Options*)soap_malloc(soap, sizeof(struct tt__H264Options));
        GetVECops->Options->H264->__sizeResolutionsAvailable = j;
        GetVECops->Options->H264->ResolutionsAvailable = (struct tt__VideoResolution*)soap_malloc(soap, j * sizeof(struct tt__VideoResolution));

        for(i = 0; i < j; ++i)
        {
            GetVECops->Options->H264->ResolutionsAvailable[i].Width = g_nvt->t.device_cap->videostream[stream_num].reswidth[0][i];
            GetVECops->Options->H264->ResolutionsAvailable[i].Height = g_nvt->t.device_cap->videostream[stream_num].resheight[0][i];
        }

        GetVECops->Options->H264->GovLengthRange = (struct tt__IntRange*)soap_malloc(soap, sizeof(struct tt__IntRange));
        GetVECops->Options->H264->GovLengthRange->Max = g_nvt->t.device_cap->videostream[stream_num].maxgovlen[0];
        GetVECops->Options->H264->GovLengthRange->Min = g_nvt->t.device_cap->videostream[stream_num].mingovlen[0];
        GetVECops->Options->H264->FrameRateRange = (struct tt__IntRange*)soap_malloc(soap, sizeof(struct tt__IntRange));
        GetVECops->Options->H264->FrameRateRange->Max = g_nvt->t.device_cap->videostream[stream_num].maxframe[0];
        GetVECops->Options->H264->FrameRateRange->Min = g_nvt->t.device_cap->videostream[stream_num].minframe[0];
        GetVECops->Options->H264->EncodingIntervalRange= (struct tt__IntRange*)soap_malloc(soap, sizeof(struct tt__IntRange));
        GetVECops->Options->H264->EncodingIntervalRange->Max = g_nvt->t.device_cap->videostream[stream_num].maxinterval[0];
        GetVECops->Options->H264->EncodingIntervalRange->Min = g_nvt->t.device_cap->videostream[stream_num].mininterval[0];
        GetVECops->Options->H264->__sizeH264ProfilesSupported = g_nvt->t.device_cap->videostream[stream_num].profileslevelnums;
        GetVECops->Options->H264->H264ProfilesSupported = (enum tt__H264Profile *)soap_malloc(soap,  g_nvt->t.device_cap->videostream[stream_num].profileslevelnums * sizeof(int));;
        GetVECops->Options->H264->H264ProfilesSupported = tt__H264Profile__Baseline;
        /*
              for(i = 0; i< g_nvt->t.device_cap->videostream[stream_num].profileslevelnums; ++i)
              {
                  GetVECops->Options->H264->H264ProfilesSupported[i] = g_nvt->t.device_cap->videostream[stream_num].profileslevelvalue[i];
              }
            */
       if(g_nvt->t.device_cap)
       {
            free(g_nvt->t.device_cap);
            g_nvt->t.device_cap = NULL;
       }    
    }

    GetVECops->Options->Extension = NULL;
    printf("[%s]=========> out\n", __FUNCTION__);    
    return SOAP_OK;
}

SOAP_FMAC5 int SOAP_FMAC6 __trt__AddVideoEncoderConfiguration(struct soap *soap, struct _trt__AddVideoEncoderConfiguration *input, struct _trt__AddVideoEncoderConfigurationResponse *addVEC)
{
    int i;
    int ptoken = 0;
    video_encoder_conf_t *videoencoder;

    for(i = 0; i < g_nvt->t.nprofiles; i++)
    {
        if(strcmp(input->ProfileToken, g_nvt->t.profiles[i].ptoken) == 0)
        {
            ptoken = 1;
            break;
        }
    }

    if(!ptoken)
        return SOAP_FAULT;

    videoencoder = (video_encoder_conf_t*)calloc(1, sizeof(video_encoder_conf_t));
    sprintf(videoencoder->token, "%s", input->ProfileToken);

    //set default value
    videoencoder->usecount = g_nvt->t.nprofiles;
    videoencoder->encoding = _H264;
    videoencoder->width = 1280;
    videoencoder->height = 720;
    videoencoder->FrameRateLimit = 25;
    videoencoder->EncodingInterval = 1;
    videoencoder->BitrateLimit = 2048;
    videoencoder->gLength = 25;
    videoencoder->hProfile = Main;

    videoencoder->MultiConf.autostart = 0;
    videoencoder->MultiConf.type = 0;
    videoencoder->MultiConf.port = g_nvt->p.port;
    videoencoder->MultiConf.ttl = 0;
    videoencoder->SessionTimeout = 720000;

    g_nvt->t.profiles[i].vec = (video_encoder_conf_t*)calloc(1, sizeof(video_encoder_conf_t));
    memcpy(g_nvt->t.profiles[i].vec, videoencoder, sizeof(video_source_conf_t));

    free(videoencoder);
    videoencoder = NULL;
    return SOAP_OK;
}

SOAP_FMAC5 int SOAP_FMAC6 __trt__RemoveVideoEncoderConfiguration(struct soap *soap, struct _trt__RemoveVideoEncoderConfiguration *input, struct _trt__RemoveVideoEncoderConfigurationResponse *removeVEC)
{
    int i;

    for(i = 0; i < g_nvt->t.nprofiles; i++)
    {
        if(strcmp(input->ProfileToken, g_nvt->t.profiles[i].ptoken) == 0)
        {
            if(g_nvt->t.profiles[i].vec)   //remove video encoder configuration
                memset(g_nvt->t.profiles[i].vec, 0, sizeof(video_encoder_conf_t));
            break;
        }
    }
    return SOAP_OK;
}

SOAP_FMAC5 int SOAP_FMAC6 __trt__SetVideoEncoderConfiguration(struct soap *soap, struct _trt__SetVideoEncoderConfiguration *input, struct _trt__SetVideoEncoderConfigurationResponse *setVEC)
{
    int i, width, height;
    int ptoken = 0;
    int govlen = 15;

    video_encoder_conf_t videoencoder;

    if(input->Configuration)
    {

        for(i = 0; i < g_nvt->t.nprofiles; i++)
        {
            if(strcmp(input->Configuration->token, g_nvt->t.profiles[i].vec->token) == 0)
            {
                ptoken = 1;
                break;
            }
        }

        if(!ptoken)
        {
            return SOAP_FAULT;
        }
#if 0
        if(input->Configuration->Resolution)
        {
            width = input->Configuration->Resolution->Width;
            height = input->Configuration->Resolution->Height;
        }

        if(((width % 2) != 0) ||( (height % 2) != 0) ||((width * height) % 16 != 0))
        {
            return SOAP_FAULT;
        }
#endif
        videoencoder.width = input->Configuration->Resolution->Width;
        videoencoder.height = input->Configuration->Resolution->Height;

        videoencoder.usecount = input->Configuration->UseCount;
        videoencoder.encoding = input->Configuration->Encoding;
        videoencoder.quality = input->Configuration->Quality;
        videoencoder.SessionTimeout = 72000;
        strcpy(videoencoder.token, input->Configuration->token);
        strcpy(videoencoder.name, input->Configuration->Name);

        if(input->Configuration->RateControl)
        {
            videoencoder.BitrateLimit = input->Configuration->RateControl->BitrateLimit;
            videoencoder.EncodingInterval = input->Configuration->RateControl->EncodingInterval;
            videoencoder.FrameRateLimit = input->Configuration->RateControl->FrameRateLimit;
        }
        else
        {
            videoencoder.BitrateLimit = 0;
            videoencoder.EncodingInterval = 0;
            videoencoder.FrameRateLimit = 0;
        }

        if(input->Configuration->H264)
        {
            videoencoder.gLength = input->Configuration->H264->GovLength;
            videoencoder.hProfile = input->Configuration->H264->H264Profile;
        }
        else
        {
            videoencoder.gLength = g_nvt->t.profiles[i].vec->gLength;
            videoencoder.hProfile = g_nvt->t.profiles[i].vec->hProfile;
        }

        if(input->Configuration->MPEG4)
        {
            videoencoder.GovLength = input->Configuration->MPEG4->GovLength;
            videoencoder.mProfile =  input->Configuration->MPEG4->Mpeg4Profile;
        }
        else
        {
            videoencoder.GovLength = g_nvt->t.profiles[i].vec->GovLength;
            videoencoder.mProfile = g_nvt->t.profiles[i].vec->mProfile;
        }

        if(input->Configuration->Multicast)
        {
            videoencoder.MultiConf.type = input->Configuration->Multicast->Address->Type;
            strcpy(videoencoder.MultiConf.ipv4Addr, *input->Configuration->Multicast->Address->IPv4Address);
            videoencoder.MultiConf.port = input->Configuration->Multicast->Port;
            videoencoder.MultiConf.ttl = input->Configuration->Multicast->TTL;
            videoencoder.MultiConf.autostart = input->Configuration->Multicast->AutoStart;
        }
    }
    else
        return SOAP_FAULT;
    set_vec_conf(i, &videoencoder);
    memcpy(g_nvt->t.profiles[i].vec, &videoencoder, sizeof(video_encoder_conf_t));
    return SOAP_OK;
}

SOAP_FMAC5 int SOAP_FMAC6 __trt__GetCompatibleVideoEncoderConfigurations(struct soap *soap, struct _trt__GetCompatibleVideoEncoderConfigurations *input, struct _trt__GetCompatibleVideoEncoderConfigurationsResponse *GetVECs)
{
    int i;
    int flag = 0;
    char _IPAddr[INFO_LENGTH];
    NET_IPV4 ip;
    video_encoder_conf_t *video_encode_info;

    ip.int32 =  net_get_ifaddr(ETH_NAME);
    sprintf(_IPAddr, "http://%d.%d.%d.%d/onvif/services", ip.str[0], ip.str[1], ip.str[2], ip.str[3]);

    for(i = 0; i < g_nvt->t.nprofiles; i++)
    {
        if(strcmp(input->ProfileToken, g_nvt->t.profiles[i].ptoken) == 0)
        {
            flag = 1;
            break;
        }
    }

    if(!flag)
    {
        return SOAP_FAULT;
    }
    video_encode_info = (video_encoder_conf_t *)calloc(1, sizeof(video_encoder_conf_t));
    get_vec_profiles(i, video_encode_info, 2);
    int bitrate = video_encode_info->BitrateLimit;
    int framerate = video_encode_info->FrameRateLimit;
    int ratecontrol = video_encode_info->EncodingInterval;
    int width = video_encode_info->width;
    int height = video_encode_info->height;
    int govlength = video_encode_info->GovLength;

    GetVECs->__sizeConfigurations = 1;
    GetVECs->Configurations = (struct tt__VideoEncoderConfiguration *)soap_malloc(soap, sizeof(struct  tt__VideoEncoderConfiguration));
    memset(GetVECs->Configurations, 0, sizeof(struct tt__VideoEncoderConfiguration));

    GetVECs->Configurations->Name = (char *)soap_malloc(soap, sizeof(char) * SMALL_INFO_LENGTH);
    strncpy(GetVECs->Configurations->Name, video_encode_info->name, SMALL_INFO_LENGTH);
    GetVECs->Configurations->UseCount = 1;
    GetVECs->Configurations->token = (char *)soap_malloc(soap, sizeof(char) * SMALL_INFO_LENGTH);
    strncpy(GetVECs->Configurations->token, video_encode_info->token, SMALL_INFO_LENGTH);
    GetVECs->Configurations->Encoding = 0;// {tt__VideoEncoding__JPEG = 0, onv__VideoEncoding__MPEG4 = 1,tt__VideoEncoding__H264 = 2}
    GetVECs->Configurations->Quality = video_encode_info->quality;
    GetVECs->Configurations->SessionTimeout = 60;
    GetVECs->Configurations->Resolution = (struct tt__VideoResolution *)soap_malloc(soap, sizeof(struct tt__VideoResolution));
    GetVECs->Configurations->Resolution->Width = width;
    GetVECs->Configurations->Resolution->Height = height;
    GetVECs->Configurations->RateControl = (struct tt__VideoRateControl *)soap_malloc(soap, sizeof(struct tt__VideoRateControl));
    GetVECs->Configurations->RateControl->FrameRateLimit = framerate;
    GetVECs->Configurations->RateControl->EncodingInterval = ratecontrol;
    GetVECs->Configurations->RateControl->BitrateLimit = bitrate;
    GetVECs->Configurations->MPEG4 = (struct tt__Mpeg4Configuration *)soap_malloc(soap, sizeof(struct tt__Mpeg4Configuration));
    GetVECs->Configurations->MPEG4->GovLength = govlength;
    GetVECs->Configurations->MPEG4->Mpeg4Profile = 0;//{onv__Mpeg4Profile__SP = 0, onv__Mpeg4Profile__ASP = 1}
    GetVECs->Configurations->H264 = (struct tt__H264Configuration *)soap_malloc(soap, sizeof(struct tt__H264Configuration));
    GetVECs->Configurations->H264->GovLength = govlength;
    GetVECs->Configurations->H264->H264Profile = 0;//Baseline = 0, Main = 1, High = 3
    GetVECs->Configurations->Multicast = (struct tt__MulticastConfiguration *)soap_malloc(soap, sizeof(struct tt__MulticastConfiguration));
    memset(GetVECs->Configurations->Multicast, 0, sizeof(struct tt__MulticastConfiguration));
    GetVECs->Configurations->Multicast->Port = 80;
    GetVECs->Configurations->Multicast->TTL = 1;
    GetVECs->Configurations->Multicast->AutoStart = 0;
    GetVECs->Configurations->Multicast->Address  = (struct tt__IPAddress *)soap_malloc(soap, sizeof(struct tt__IPAddress));
    GetVECs->Configurations->Multicast->Address->Type = 0;
    GetVECs->Configurations->Multicast->Address->IPv4Address = (char **)soap_malloc(soap, sizeof(char *));
    GetVECs->Configurations->Multicast->Address->IPv4Address[0] = (char *)soap_malloc(soap, sizeof(char) * INFO_LENGTH);
    strcpy(*GetVECs->Configurations->Multicast->Address->IPv4Address, _IPAddr);
    GetVECs->Configurations->Multicast->Address->IPv6Address = NULL;

    if(video_encode_info)
    {
        free(video_encode_info);
        video_encode_info = NULL;
    }
    return SOAP_OK;
}

SOAP_FMAC5 int SOAP_FMAC6 __trt__RemoveVideoSourceConfiguration(struct soap *soap, struct _trt__RemoveVideoSourceConfiguration *input, struct _trt__RemoveVideoSourceConfigurationResponse *removeVS)
{
    int i;

    for(i = 0; i < g_nvt->t.nprofiles; i++)
    {
        if(strcmp(input->ProfileToken, g_nvt->t.profiles[i].ptoken) == 0)
        {
            if(g_nvt->t.profiles[i].vsc)
            {
                memset(g_nvt->t.profiles[i].vsc, 0, sizeof(video_source_conf_t)); //remove video source
            }
            break;
        }
    }
    return SOAP_OK;
}

SOAP_FMAC5 int SOAP_FMAC6 __trt__AddVideoSourceConfiguration(struct soap *soap, struct _trt__AddVideoSourceConfiguration *input, struct _trt__AddVideoSourceConfigurationResponse *AddVideoSource)
{
    int i;
    int ptoken = 0;
    video_source_conf_t *videosource;

    if(input->ProfileToken)
    {
        for(i = 0; i < g_nvt->t.nprofiles; i++)
        {
            if(strcmp(input->ProfileToken, g_nvt->t.profiles[i].ptoken) == 0)
            {
                ptoken = 1;
                break;
            }
        }

        if(!ptoken)
            return SOAP_FAULT;

        videosource = (video_source_conf_t*)calloc(1, sizeof(video_source_conf_t));

        videosource->vcount = 1;
        if(input->ConfigurationToken)
        {
            sprintf(videosource->vtoken, "%s", input->ConfigurationToken);
        }
        else
            return SOAP_FAULT;

        //set default value
        videosource->heigth = 720;
        videosource->width = 1028;
        videosource->x = 100;
        videosource->y = 200;
        strcpy(videosource->vsourcetoken, g_nvt->t.profiles[i].vsc->vsourcetoken);
        videosource->vcount = g_nvt->t.profiles[i].vsc->vcount;
    }
    else
        return SOAP_FAULT;

    g_nvt->t.profiles[i].vsc = (video_source_conf_t*)calloc(1, sizeof(video_source_conf_t));

    memcpy(g_nvt->t.profiles[i].vsc, videosource, sizeof(video_source_conf_t));
    free(videosource);
    videosource = NULL;
    return SOAP_OK;
}

SOAP_FMAC5 int SOAP_FMAC6 __trt__SetVideoSourceConfiguration(struct soap *soap, struct _trt__SetVideoSourceConfiguration *input, struct _trt__SetVideoSourceConfigurationResponse *SetVSC)
{
    int i, width, height;
    int ptoken = 0;
    video_source_conf_t *videosource;

    if(input->Configuration)
    {
        for(i = 0; i < g_nvt->t.nprofiles; i++)
        {
            if(strcmp(input->Configuration->token, g_nvt->t.profiles[i].ptoken) == 0)
            {
                ptoken = 1;
                break;
            }
        }
        if(!ptoken)
        {
            return SOAP_FAULT;
        }
        if(input->Configuration->Bounds)
        {
            width = input->Configuration->Bounds->width;
            height = input->Configuration->Bounds->height;
            if(! ((width >= 320 && width <= 1960) && (height >= 192 && height <= 1080)))
            {
                return SOAP_FAULT;
            }
            videosource = (video_source_conf_t*)calloc(1, sizeof(video_source_conf_t));
            videosource->width = width;
            videosource->heigth= height;
            videosource->x = input->Configuration->Bounds->x;
            videosource->y = input->Configuration->Bounds->y;
        }

        sprintf(videosource->vname, "%s", input->Configuration->Name);
        sprintf(videosource->vtoken, "%s", input->Configuration->token);
        sprintf(videosource->vsourcetoken, "%s", input->Configuration->SourceToken);
        videosource->vcount = input->Configuration->UseCount;
    }

    set_vsc_conf(i, videosource);
    memcpy(g_nvt->t.profiles[i].vsc, videosource, sizeof(video_source_conf_t));
    if(videosource)
    {
        free(videosource);
        videosource = NULL;
    }
    return SOAP_OK;
}

SOAP_FMAC5 int SOAP_FMAC6 __trt__GetCompatibleVideoSourceConfigurations(struct soap *soap, struct _trt__GetCompatibleVideoSourceConfigurations *input, struct _trt__GetCompatibleVideoSourceConfigurationsResponse *GetVSCs)
{
    int i;
    int flag = 0;

    for(i=0; i < g_nvt->t.nprofiles; i++)
    {
        if(strcmp(input->ProfileToken, g_nvt->t.profiles[i].ptoken) == 0)
        {
            flag = -1;
            break;
        }
    }
    if(!flag)
    {
        return SOAP_FAULT;
    }

    GetVSCs->__sizeConfigurations = 1;
    GetVSCs->Configurations = (struct tt__VideoSourceConfiguration *)soap_malloc(soap, 3 * sizeof(struct tt__VideoSourceConfiguration));
    memset(GetVSCs->Configurations, 0, 3 * sizeof(struct tt__VideoSourceConfiguration));
    GetVSCs->Configurations[0].Name = (char *)soap_malloc(soap, sizeof(char) * SMALL_INFO_LENGTH);
    strcpy(GetVSCs->Configurations[0].Name, g_nvt->t.profiles[i].vsc->vname);
    GetVSCs->Configurations[0].UseCount = 1;
    GetVSCs->Configurations[0].token = (char *)soap_malloc(soap, sizeof(char) * SMALL_INFO_LENGTH);
    strcpy(GetVSCs->Configurations[0].token, g_nvt->t.profiles[i].vsc->vtoken);
    GetVSCs->Configurations[0].SourceToken = (char *)soap_malloc(soap, sizeof(char) * SMALL_INFO_LENGTH);
    strcpy(GetVSCs->Configurations[0].SourceToken, g_nvt->t.profiles[i].vsc->vsourcetoken);
    GetVSCs->Configurations[0].Bounds = (struct tt__IntRectangle *)soap_malloc(soap, sizeof(struct tt__IntRectangle));
    GetVSCs->Configurations[0].Bounds->x = 0;
    GetVSCs->Configurations[0].Bounds->y = 0;
    GetVSCs->Configurations[0].Bounds->width = 1920;
    GetVSCs->Configurations[0].Bounds ->height = 1080;

    GetVSCs->Configurations[1].Name = (char *)soap_malloc(soap, sizeof(char) * SMALL_INFO_LENGTH);
    strcpy(GetVSCs->Configurations[1].Name, g_nvt->t.profiles[i].vsc->vname);
    GetVSCs->Configurations[1].UseCount = 1;
    GetVSCs->Configurations[1].token = (char *)soap_malloc(soap, sizeof(char) * SMALL_INFO_LENGTH);    
    strcpy(GetVSCs->Configurations[1].token, g_nvt->t.profiles[i].vsc->vtoken);
    GetVSCs->Configurations[1].SourceToken = (char *)soap_malloc(soap, sizeof(char) * SMALL_INFO_LENGTH);    
    strcpy(GetVSCs->Configurations[1].SourceToken, g_nvt->t.profiles[i].vsc->vsourcetoken);
    GetVSCs->Configurations[1].Bounds = (struct tt__IntRectangle *)soap_malloc(soap, sizeof(struct tt__IntRectangle));
    GetVSCs->Configurations[1].Bounds->x = 0;
    GetVSCs->Configurations[1].Bounds->y = 0;
    GetVSCs->Configurations[1].Bounds ->width = 1280;
    GetVSCs->Configurations[1].Bounds ->height = 720;
    
    GetVSCs->Configurations[2].Name = (char *)soap_malloc(soap, sizeof(char) * SMALL_INFO_LENGTH);
    strcpy(GetVSCs->Configurations[2].Name, g_nvt->t.profiles[i].vsc->vname);
    GetVSCs->Configurations[2].UseCount = 1;
    GetVSCs->Configurations[2].token = (char *)soap_malloc(soap, sizeof(char) * SMALL_INFO_LENGTH);
    strcpy(GetVSCs->Configurations[2].token, g_nvt->t.profiles[i].vsc->vtoken);
    GetVSCs->Configurations[2].SourceToken = (char *)soap_malloc(soap, sizeof(char) * SMALL_INFO_LENGTH);
    strcpy(GetVSCs->Configurations[2].SourceToken, g_nvt->t.profiles[i].vsc->vsourcetoken);
    GetVSCs->Configurations[2].Bounds = (struct tt__IntRectangle *)soap_malloc(soap, sizeof(struct tt__IntRectangle));
    GetVSCs->Configurations[2].Bounds->x = 0;
    GetVSCs->Configurations[2].Bounds->y = 0;
    GetVSCs->Configurations[2].Bounds->width = 1280;
    GetVSCs->Configurations[2].Bounds->height = 960;

    return SOAP_OK;
}

SOAP_FMAC5 int SOAP_FMAC6 __trt__GetVideoSourceConfigurationOptions(struct soap *soap, struct _trt__GetVideoSourceConfigurationOptions *input, struct _trt__GetVideoSourceConfigurationOptionsResponse *GetVSCops)
{
    int i;
    int num_token = 0;
    int j;
    int flag = 0;
    int flg = 0;
    int index = 0;
    int num = 0;

    for(i = 0; i < g_nvt->t.nprofiles; i++)
    {
        for(j = 0; j <= i; j++)
        {
            if(strcmp(g_nvt->t.profiles[j].vsc->vtoken, g_nvt->t.profiles[i].vsc->vtoken) == 0)
            {
                flg = 1;
                break;
            }
        }
        if(flg == 0)
        {
            num_token++;
        }
    }
    if(input->ConfigurationToken != NULL)
    {
        for(i = 0; i < g_nvt->t.nprofiles; i++)
        {
            if(strcmp(input->ConfigurationToken, g_nvt->t.profiles[i].vsc->vtoken) == 0);
            {
                flag = 1;
                index = j;
                break;
            }
        }
    }

    if(input->ProfileToken != NULL)
    {
        for(i = 0; i < g_nvt->t.nprofiles; i++)
        {
            if(strcmp(input->ProfileToken, g_nvt->t.profiles[i].ptoken)==0)
            {
                flag = 1;
                index = j;
                break;
            }
        }
    }

    if(!flag)
    {
        return SOAP_FAULT;
    }
    else
    {
        GetVSCops->Options = (struct tt__VideoSourceConfigurationOptions *)soap_malloc(soap, sizeof(struct tt__VideoSourceConfigurationOptions));
        memset(GetVSCops->Options, 0, sizeof(struct tt__VideoSourceConfigurationOptions));

        GetVSCops->Options->BoundsRange = (struct tt__IntRectangleRange *)soap_malloc(soap, sizeof(struct tt__IntRectangleRange));
        GetVSCops->Options->BoundsRange->XRange = (struct tt__IntRange *)soap_malloc(soap, sizeof(struct tt__IntRange));
        GetVSCops->Options->BoundsRange->XRange->Min = 0;
        GetVSCops->Options->BoundsRange->XRange->Max = 0;
        GetVSCops->Options->BoundsRange->YRange = (struct tt__IntRange *)soap_malloc(soap, sizeof(struct tt__IntRange));
        GetVSCops->Options->BoundsRange->YRange->Min = 0;
        GetVSCops->Options->BoundsRange->YRange->Max = 0;
        GetVSCops->Options->BoundsRange->WidthRange = (struct tt__IntRange *)soap_malloc(soap, sizeof(struct tt__IntRange));
        GetVSCops->Options->BoundsRange->WidthRange->Min = 320;
        GetVSCops->Options->BoundsRange->WidthRange->Max = 1920;
        GetVSCops->Options->BoundsRange->HeightRange = (struct tt__IntRange *)soap_malloc(soap, sizeof(struct tt__IntRange));
        GetVSCops->Options->BoundsRange->HeightRange->Min = 192;
        GetVSCops->Options->BoundsRange->HeightRange->Max = 1080;
        GetVSCops->Options->VideoSourceTokensAvailable = (char **)soap_malloc(soap, sizeof(char *) * 1);

        if(input->ProfileToken == NULL && input->ConfigurationToken == NULL)
        {
            GetVSCops->Options->__sizeVideoSourceTokensAvailable = num_token;
            for(i = 0;i < g_nvt->t.nprofiles; i++)
            {
                for(j = 0; j < i; j++)
                {
                    if(strcmp(g_nvt->t.profiles[j].vsc->vtoken, g_nvt->t.profiles[i].vsc->vtoken) == 0)
                    {
                    flg = 1;
                    break;
                    }
                }
                if(flg == 0)
                {
                    GetVSCops->Options->VideoSourceTokensAvailable[num] = (char *)soap_malloc(soap, sizeof(char) * SMALL_INFO_LENGTH);
                    strcpy(GetVSCops->Options->VideoSourceTokensAvailable[num], g_nvt->t.profiles[index].vsc->vsourcetoken);
                    num++;
                }
                flg = 0;
            }
        }
        else
        {
            GetVSCops->Options->__sizeVideoSourceTokensAvailable = 1;
            GetVSCops->Options->VideoSourceTokensAvailable[0] = (char *)soap_malloc(soap, sizeof(char) * SMALL_INFO_LENGTH);
            strcpy(GetVSCops->Options->VideoSourceTokensAvailable[0], g_nvt->t.profiles[index].vsc->vsourcetoken);
        }
    }


    return SOAP_OK;

}

SOAP_FMAC5 int SOAP_FMAC6 __trt__GetAudioSources(struct soap *soap, struct _trt__GetAudioSources *trt__GetAudioSources, struct _trt__GetAudioSourcesResponse *GetAudioSources)
{
    int i, j, flag = 0, num_token = 0;

    for(i = 0; i < g_nvt->t.nprofiles; i++)
    {
        for(j = 0; j < i; j++)
        {
            if(!strcmp(g_nvt->t.profiles[i].asc->token, "") ||strcmp(g_nvt->t.profiles[j].asc->token,  g_nvt->t.profiles[i].asc->token)==0)
            {
                flag = 1;
                break;
            }
        }
        if(flag == 0)
        {
            num_token++;
        }
        flag = 0;
    }

    GetAudioSources->__sizeAudioSources = num_token;
    GetAudioSources->AudioSources = (struct tt__AudioSource *)soap_malloc(soap, sizeof(struct tt__AudioSource)* GetAudioSources->__sizeAudioSources);
    memset(GetAudioSources->AudioSources, 0, sizeof(struct tt__AudioSource)* GetAudioSources->__sizeAudioSources);

    for(i = 0; i < g_nvt->t.nprofiles; i++)
    {
        flag = -1;
        for(j = 0; j < i; j++)
        {

            if(!strcmp(g_nvt->t.profiles[i].asc->token, "")||strcmp(g_nvt->t.profiles[j].asc->token, g_nvt->t.profiles[i].asc->token)==0)
            {
                flag = 1;
                break;
            }
        }

        if(!flag)
        {
            GetAudioSources->AudioSources[i].token = (char *)soap_malloc(soap,  sizeof(char) * INFO_LENGTH);
            strcpy(GetAudioSources->AudioSources[i].token, g_nvt->t.profiles[i].asc->token);
            GetAudioSources->AudioSources[i].Channels = 1;
        }
    }
    return SOAP_OK;
}

SOAP_FMAC5 int SOAP_FMAC6 __trt__GetAudioSourceConfigurations(struct soap *soap, struct _trt__GetAudioSourceConfigurations *trt__GetAudioSourceConfigurations, struct _trt__GetAudioSourceConfigurationsResponse *GetASC)
{
    int i;

    get_dev_profiles(ALL_CHS, g_nvt->t.profiles, 2, ASC);
    GetASC->Configurations = (struct tt__AudioSourceConfiguration*)soap_malloc(soap,(g_nvt->t.nprofiles* sizeof(struct tt__AudioSourceConfiguration)));
    memset(GetASC->Configurations, 0, g_nvt->t.nprofiles * sizeof(struct tt__AudioSourceConfiguration));

    for(i = 0; i < g_nvt->t.nprofiles; i++)
    {
        GetASC->Configurations[i].Name = (char *)soap_malloc(soap,  sizeof(char) * INFO_LENGTH);
        GetASC->Configurations[i].SourceToken = (char *)soap_malloc(soap,  sizeof(char) * INFO_LENGTH);
        GetASC->Configurations[i].token = (char *)soap_malloc(soap,  sizeof(char) * INFO_LENGTH);
        strcpy(GetASC->Configurations[i].Name, g_nvt->t.profiles[i].asc->aname);
        strcpy(GetASC->Configurations[i].SourceToken, g_nvt->t.profiles[i].asc->sourcetoken);
        strcpy(GetASC->Configurations[i].token, g_nvt->t.profiles[i].asc->token);
        GetASC->Configurations[i].UseCount = g_nvt->t.profiles[i].asc->count;
    }
    GetASC->__sizeConfigurations = i;

    return SOAP_OK;
}

SOAP_FMAC5 int SOAP_FMAC6 __trt__GetAudioSourceConfiguration(struct soap *soap, struct _trt__GetAudioSourceConfiguration *input, struct _trt__GetAudioSourceConfigurationResponse *GetASC)
{
    int i = 0;
    int profile = 0;

    for(i = 0; i < g_nvt->t.nprofiles; i++)
    {
        if(strcmp(input->ConfigurationToken, g_nvt->t.profiles[i].asc->token) == 0)
        {
            profile = 1;
            break;
        }
    }

    if(!profile)
    {
        return SOAP_FAULT;
    }
    audio_source_conf_t *info = (audio_source_conf_t *)calloc(1, sizeof(audio_source_conf_t));
    GetASC->Configuration = (struct tt__AudioSourceConfiguration *)soap_malloc(soap, sizeof(struct tt__AudioSourceConfiguration));
    memset(GetASC->Configuration, 0, sizeof(struct tt__AudioSourceConfiguration));
    GetASC->Configuration->Name = (char *)soap_malloc(soap, sizeof(char) * SMALL_INFO_LENGTH);
    strncpy(GetASC->Configuration->Name, info->aname, SMALL_INFO_LENGTH);
    GetASC->Configuration->SourceToken = (char *)soap_malloc(soap, sizeof(char) * SMALL_INFO_LENGTH);
    strncpy(GetASC->Configuration->SourceToken, info->sourcetoken, SMALL_INFO_LENGTH);
    GetASC->Configuration->token = (char *)soap_malloc(soap, sizeof(char) * SMALL_INFO_LENGTH);
    strncpy(GetASC->Configuration->token, info->token, SMALL_INFO_LENGTH);
    GetASC->Configuration->UseCount = info->count;

    if(info)
    {
        free(info);
        info = NULL;
    }
    return SOAP_OK;

}

SOAP_FMAC5 int SOAP_FMAC6 __trt__GetAudioEncoderConfigurations(struct soap *soap, struct _trt__GetAudioEncoderConfigurations *inputs, struct _trt__GetAudioEncoderConfigurationsResponse *GetAEC)
{
    int i;
    char _IPAddr[LARGE_INFO_LENGTH];
    NET_IPV4 ip;

    ip.int32 =  net_get_ifaddr(ETH_NAME);
    sprintf(_IPAddr, "%d.%d.%d.%d", ip.str[0], ip.str[1], ip.str[2], ip.str[3]);

    get_dev_profiles(ALL_CHS, g_nvt->t.profiles, 2, AEC);

    GetAEC->Configurations = (struct tt__AudioEncoderConfiguration*)soap_malloc(soap, (g_nvt->t.nprofiles* sizeof(struct tt__AudioEncoderConfiguration)));
    memset(GetAEC->Configurations, 0, g_nvt->t.nprofiles * sizeof(struct tt__AudioEncoderConfiguration));

    for(i = 0; i < g_nvt->t.nprofiles; i++)
    {
        GetAEC->Configurations[i].Bitrate = g_nvt->t.profiles[i].aec->bitrate;
        GetAEC->Configurations[i].Encoding = g_nvt->t.profiles[i].aec->encoding;
        GetAEC->Configurations[i].Multicast = (struct tt__MulticastConfiguration *)soap_malloc(soap, sizeof(struct tt__MulticastConfiguration));
        memset(GetAEC->Configurations[i].Multicast, 0, sizeof(struct tt__MulticastConfiguration));

        GetAEC->Configurations[i].Multicast->AutoStart = g_nvt->t.profiles[i].aec->MultiConf.autostart;
        GetAEC->Configurations[i].Multicast->Port = g_nvt->t.profiles[i].aec->MultiConf.port;
        GetAEC->Configurations[i].Multicast->TTL = g_nvt->t.profiles[i].aec->MultiConf.ttl;
        GetAEC->Configurations[i].Multicast->Address  = (struct tt__IPAddress *)soap_malloc(soap, sizeof(struct tt__IPAddress));
        GetAEC->Configurations[i].Multicast->Address->Type = 0;
        GetAEC->Configurations[i].Multicast->Address->IPv4Address = (char **)soap_malloc(soap, sizeof(char *));
        GetAEC->Configurations[i].Multicast->Address->IPv4Address[0] = (char *)soap_malloc(soap, sizeof(char) * INFO_LENGTH);
        strcpy(GetAEC->Configurations[i].Multicast->Address->IPv4Address[0], _IPAddr);
        GetAEC->Configurations[i].Multicast->Address->IPv6Address = NULL;

        GetAEC->Configurations[i].Name = (char *)soap_malloc(soap, sizeof(char) * SMALL_INFO_LENGTH);

        strcpy(GetAEC->Configurations[i].Name, g_nvt->t.profiles[i].aec->name);
        GetAEC->Configurations[i].SampleRate = g_nvt->t.profiles[i].aec->samplerate;
        GetAEC->Configurations[i].SessionTimeout = g_nvt->t.profiles[i].aec->SessionTimeout;
        GetAEC->Configurations[i].token = (char *)soap_malloc(soap, sizeof(char) * SMALL_INFO_LENGTH);
        strcpy(GetAEC->Configurations[i].token, g_nvt->t.profiles[i].aec->token);
        GetAEC->Configurations[i].UseCount = g_nvt->t.profiles[i].aec->usecount;
    }
    GetAEC->__sizeConfigurations = i;
    return SOAP_OK;
}

SOAP_FMAC5 int SOAP_FMAC6 __trt__AddAudioSourceConfiguration(struct soap *soap, struct _trt__AddAudioSourceConfiguration *input, struct _trt__AddAudioSourceConfigurationResponse *AddASC)
{
    int i;
    int ptoken = 0;
    char ctoken[SMALL_INFO_LENGTH] = "";

    audio_source_conf_t  *audiosource;
    for(i = 0; i < g_nvt->t.nprofiles; i++)
    {
        if(strcmp(input->ProfileToken, g_nvt->t.profiles[i].ptoken) == 0)
        {
            ptoken = 1;
            break;
        }
    }

    strcpy(ctoken, input->ConfigurationToken);
    if(strcmp(ctoken, "G711") && strcmp(ctoken, "G726") && strcmp(ctoken, "AAC"))
    {
        return SOAP_FAULT;
    }

    if(!ptoken)
    {
        return SOAP_FAULT;
    }
    audiosource = (audio_source_conf_t*)calloc(1, sizeof(audio_source_conf_t));
    sprintf(audiosource->token, "%s", input->ConfigurationToken);

    //set default value
    strcpy(audiosource->aname, g_nvt->t.profiles[i].asc->aname);
    strcpy(audiosource->sourcetoken, g_nvt->t.profiles[i].asc->sourcetoken);
    audiosource->count = g_nvt->t.profiles[i].asc->count;

    g_nvt->t.profiles[i].asc = (audio_source_conf_t*)calloc(1, sizeof(audio_source_conf_t));
    memcpy(g_nvt->t.profiles[i].asc, audiosource, sizeof(audio_source_conf_t));
    if(audiosource)
    {
        free(audiosource);
        audiosource = NULL;
    }

    return SOAP_OK;
}

SOAP_FMAC5 int SOAP_FMAC6 __trt__GetAudioEncoderConfigurationOptions(struct soap *soap, struct _trt__GetAudioEncoderConfigurationOptions *input, struct _trt__GetAudioEncoderConfigurationOptionsResponse *GetAECops)
{
    int i;
    int flag = 0;

    if(input->ConfigurationToken != NULL)
    {
        for(i = 0; i < g_nvt->t.nprofiles; i++)
        {
            if(strcmp(input->ConfigurationToken, g_nvt->t.profiles[i].aec->token) == 0)
            {
                flag = 1;
                break;
            }
        }
    }
      
    if(flag == 0)
    {
        return SOAP_FAULT;
    } 
    
    audio_encoder_conf_t *info = (audio_encoder_conf_t*)calloc(1, sizeof(audio_encoder_conf_t));
    
    get_aec_profiles(i, info, 2);

    GetAECops->Options = (struct tt__AudioEncoderConfigurationOptions *)soap_malloc( soap, sizeof(struct tt__AudioEncoderConfigurationOptions));
    memset(GetAECops->Options, 0, sizeof(struct tt__AudioEncoderConfigurationOptions));

    GetAECops->Options->__sizeOptions = 1;
    GetAECops->Options->Options = (struct tt__AudioEncoderConfigurationOption *)soap_malloc(soap, sizeof(struct tt__AudioEncoderConfigurationOption));
    memset(GetAECops->Options->Options, 0, sizeof(struct tt__AudioEncoderConfigurationOption));
    GetAECops->Options->Options[0].Encoding = info->encoding;  // "G711";
    GetAECops->Options->Options[0].BitrateList = (struct tt__IntList *)soap_malloc(soap, sizeof(struct tt__IntList));
    GetAECops->Options->Options[0].BitrateList->__sizeItems = 1;
    GetAECops->Options->Options[0].BitrateList->Items = (int*)soap_malloc(soap, sizeof(int));
    *GetAECops->Options->Options[0].BitrateList->Items = info->bitrate;
    GetAECops->Options->Options[0].SampleRateList = (struct tt__IntList*)soap_malloc(soap, sizeof(struct tt__IntList));
    GetAECops->Options->Options[0].SampleRateList->__sizeItems = 1;
    GetAECops->Options->Options[0].SampleRateList->Items = (int*)soap_malloc(soap, sizeof(int));
    *GetAECops->Options->Options[0].SampleRateList->Items = info->samplerate;

    if(info)
    {
        free(info);
        info = NULL;
    }
    return SOAP_OK;
}


SOAP_FMAC5 int SOAP_FMAC6 __trt__RemoveAudioSourceConfiguration(struct soap *soap, struct _trt__RemoveAudioSourceConfiguration *input, struct _trt__RemoveAudioSourceConfigurationResponse *RemoveASC)
{
    int i;

    for(i = 0; i < g_nvt->t.nprofiles; i++)
    {
        if(strcmp(input->ProfileToken, g_nvt->t.profiles[i].ptoken) == 0)
        {
            if(g_nvt->t.profiles[i].asc)
            {
                memset(g_nvt->t.profiles[i].asc, 0, sizeof(audio_source_conf_t)); //remove video source
            }
            break;
        }
    }
    return SOAP_OK;
}

SOAP_FMAC5 int SOAP_FMAC6 __trt__SetAudioSourceConfiguration(struct soap *soap, struct _trt__SetAudioSourceConfiguration *input, struct _trt__SetAudioSourceConfigurationResponse *trt__SetAudioSourceConfigurationResponse)
{
    int i;
    int flag = 0;
    int count;
    char _Token[SMALL_INFO_LENGTH];
    char _Name[SMALL_INFO_LENGTH];
    char _SourceToken[SMALL_INFO_LENGTH];
    audio_source_conf_t *audio_conf;

    for(i = 0; i < g_nvt->t.nprofiles; i++)
    {
        if(strcmp(_SourceToken, g_nvt->t.profiles[i].asc->sourcetoken) == 0)
        {
            flag = 1;
            break;
        }
    }
    if(!flag)
    {
        return SOAP_FAULT;
    }

    strcpy(_SourceToken, input->Configuration->SourceToken);
    strcpy(_Token, input->Configuration->token);
    strcpy(_Name, input->Configuration->Name);
    count = input->Configuration->UseCount;

    audio_conf = (audio_source_conf_t *)calloc(1, sizeof(audio_source_conf_t));
    memcpy(g_nvt->t.profiles[i].asc, audio_conf, sizeof(audio_conf));
    if(audio_conf)
    {
        free(audio_conf);
        audio_conf = NULL;
    }
    return SOAP_OK;
}

SOAP_FMAC5 int SOAP_FMAC6 __trt__GetCompatibleAudioSourceConfigurations(struct soap *soap, struct _trt__GetCompatibleAudioSourceConfigurations *trt__GetCompatibleAudioSourceConfigurations, struct _trt__GetCompatibleAudioSourceConfigurationsResponse *GetASC)
{
    GetASC->__sizeConfigurations = 3;
    GetASC->Configurations = (struct tt__AudioSourceConfiguration*)soap_malloc(soap, GetASC->__sizeConfigurations * sizeof(struct tt__AudioSourceConfiguration));
    memset(GetASC->Configurations, 0, GetASC->__sizeConfigurations * sizeof(struct tt__AudioSourceConfiguration));

    GetASC->Configurations[0].Name = (char *)soap_malloc(soap, sizeof(char) * SMALL_INFO_LENGTH);
    strcpy(GetASC->Configurations[0].Name, "G711");
    GetASC->Configurations[0].UseCount = 1;
    GetASC->Configurations[0].token = (char *)soap_malloc(soap, sizeof(char) * SMALL_INFO_LENGTH);
    strcpy(GetASC->Configurations[0].token, "G711");
    GetASC->Configurations[0].SourceToken = (char *)soap_malloc(soap, sizeof(char) * SMALL_INFO_LENGTH);
    strcpy(GetASC->Configurations[0].SourceToken, "G711");
    GetASC->Configurations[1].Name = (char *)soap_malloc(soap, sizeof(char) * SMALL_INFO_LENGTH);
    strcpy(GetASC->Configurations[1].Name, "G726");
    GetASC->Configurations[1].UseCount = 1;
    GetASC->Configurations[1].token = (char *)soap_malloc(soap, sizeof(char) * SMALL_INFO_LENGTH);
    strcpy(GetASC->Configurations[1].token, "G726");
    GetASC->Configurations[1].SourceToken = (char *)soap_malloc(soap, sizeof(char) * SMALL_INFO_LENGTH);;
    strcpy(GetASC->Configurations[1].SourceToken, "G726");
    GetASC->Configurations[2].Name = (char *)soap_malloc(soap, sizeof(char) * SMALL_INFO_LENGTH);
    strcpy(GetASC->Configurations[2].Name, "AAC");
    GetASC->Configurations[2].UseCount = 1;
    GetASC->Configurations[2].token = (char *)soap_malloc(soap, sizeof(char) * SMALL_INFO_LENGTH);
    strcpy(GetASC->Configurations[2].token, "AAC");
    GetASC->Configurations[2].SourceToken = (char *)soap_malloc(soap, sizeof(char) * SMALL_INFO_LENGTH);;
    strcpy(GetASC->Configurations[2].SourceToken, "AAC");

    return SOAP_OK;
}

SOAP_FMAC5 int SOAP_FMAC6 __trt__GetAudioEncoderConfiguration(struct soap *soap, struct _trt__GetAudioEncoderConfiguration *input, struct _trt__GetAudioEncoderConfigurationResponse *GetAEC)
{
    int i, j;
    char IPAddr[INFO_LENGTH];
    NET_IPV4 ip;
    int flag = 0;
    int num = 0;

    for(i = 0; i <  g_nvt->t.nprofiles; i++)
    {
            if(strcmp(input->ConfigurationToken, g_nvt->t.profiles[i].aec->token) == 0)
            {
                    flag = 1;
                    break;
            }
    }
    if(!flag)
    {
            return SOAP_FAULT;
    }

    for(j = 0; j < g_nvt->t.nprofiles; j++)
    {
            if(strcmp(input->ConfigurationToken, g_nvt->t.profiles[j].aec->token) == 0)
            {
                    num++;
            }
    }
    audio_encoder_conf_t * info = (audio_encoder_conf_t *)calloc(1, sizeof(audio_encoder_conf_t));
    get_aec_profiles(i, info, 2);

    ip.int32 =  net_get_ifaddr(ETH_NAME);
    sprintf(IPAddr, "http://%03d.%03d.%03d.%03d/onvif/services", ip.str[0], ip.str[1], ip.str[2], ip.str[3]);

    GetAEC->Configuration = (struct tt__AudioEncoderConfiguration *)soap_malloc(soap, sizeof(struct tt__AudioEncoderConfiguration));
    memset(GetAEC->Configuration, 0, sizeof(struct tt__AudioEncoderConfiguration));

    GetAEC->Configuration->Name = (char *) soap_malloc(soap, sizeof(char) * SMALL_INFO_LENGTH);
    strcpy(GetAEC->Configuration->Name, info->name);
    GetAEC->Configuration->UseCount = num;
    GetAEC->Configuration->token = (char *) soap_malloc(soap, sizeof(char) * SMALL_INFO_LENGTH);
    strcpy(GetAEC->Configuration->token, info->token);
    GetAEC->Configuration->Encoding = info->encoding;
    GetAEC->Configuration->Bitrate = info->bitrate;
    GetAEC->Configuration->SampleRate = info->samplerate;
    GetAEC->Configuration->Multicast = (struct tt__MulticastConfiguration *)soap_malloc(soap, sizeof(struct tt__MulticastConfiguration));
    memset(GetAEC->Configuration->Multicast, 0, sizeof(struct tt__MulticastConfiguration));
    GetAEC->Configuration->Multicast->Address = (struct tt__IPAddress*)soap_malloc(soap, sizeof(struct tt__IPAddress));
    GetAEC->Configuration->Multicast->Address->Type = info->MultiConf.type;
    GetAEC->Configuration->Multicast->Address->IPv4Address = (char **)soap_malloc(soap, sizeof(char *));
    GetAEC->Configuration->Multicast->Address->IPv4Address[0] = (char *)soap_malloc(soap, sizeof(char) * INFO_LENGTH);
    strcpy(*GetAEC->Configuration->Multicast->Address->IPv4Address, IPAddr);
    GetAEC->Configuration->Multicast->Address->IPv6Address = NULL;
    GetAEC->Configuration->Multicast->Port = info->MultiConf.port;
    GetAEC->Configuration->Multicast->TTL = info->MultiConf.ttl;
    GetAEC->Configuration->Multicast->AutoStart = info->MultiConf.autostart;
    GetAEC->Configuration->SessionTimeout = info->SessionTimeout;

    if(info)
    {
        free(info);
        info = NULL;
    }
    return SOAP_OK;
}

SOAP_FMAC5 int SOAP_FMAC6 __trt__AddAudioEncoderConfiguration(struct soap *soap, struct _trt__AddAudioEncoderConfiguration *input, struct _trt__AddAudioEncoderConfigurationResponse *AddAEC)
{
    int i;
    int ptoken = 0;

    audio_encoder_conf_t  *audioencoder;
    for(i = 0; i < g_nvt->t.nprofiles; i++)
    {
        if(strcmp(input->ProfileToken, g_nvt->t.profiles[i].ptoken) == 0)
        {
            ptoken = 1;
            break;
        }
    }

    if(!ptoken)
    {
        return SOAP_FAULT;
    }
    audioencoder = (audio_encoder_conf_t*)calloc(1, sizeof(audio_encoder_conf_t));
    strcpy(audioencoder->token, input->ConfigurationToken);
    memcpy(g_nvt->t.profiles[i].aec, audioencoder, sizeof(audio_encoder_conf_t));
    if(audioencoder)
    {
        free(audioencoder);
        audioencoder = NULL;
    }
    return SOAP_OK;
}

SOAP_FMAC5 int SOAP_FMAC6 __trt__RemoveAudioEncoderConfiguration(struct soap *soap, struct _trt__RemoveAudioEncoderConfiguration *input, struct _trt__RemoveAudioEncoderConfigurationResponse *RemoveAEC)
{
    int i;

    for(i = 0; i < g_nvt->t.nprofiles; i++)
    {
        if(strcmp(input->ProfileToken, g_nvt->t.profiles[i].ptoken) == 0)
        {
            if(g_nvt->t.profiles[i].aec)
            {
                memset(g_nvt->t.profiles[i].aec, 0, sizeof(audio_encoder_conf_t)); //remove video source
            }
            break;
        }
    }
    return SOAP_OK;
}

SOAP_FMAC5 int SOAP_FMAC6 __trt__SetAudioEncoderConfiguration(struct soap *soap, struct _trt__SetAudioEncoderConfiguration *input, struct _trt__SetAudioEncoderConfigurationResponse *trt__SetAudioEncoderConfigurationResponse)
{
#if 0
    int i;
    int flag = 0;
    char _Bitrate = input->Configuration->Bitrate;
    char _SampleRate = input->Configuration->SampleRate;
    char _Encoding = input->Configuration->Encoding;
    char _Enable = 1;
    char _Mode = 2;//0 - Mic, 1-Speaker ,2-Both
    audio_encoder_conf_t *add_audio_encoder;

    for(i = 0; i < g_nvt->t.nprofiles; i++)
    {
        if(strcmp(input->Configuration->token, g_nvt->t.profiles[i].aec->token) == 0)
        {
            flag = 1;
            break;
        }
    }
    if(!flag)
    {
        return SOAP_FAULT;
    }

    if(_Encoding == 0 || _Encoding == 1) //G711 || G726
    {
        if(_Bitrate != 64 && _Bitrate != 128)
        {
            return SOAP_FAULT;
        }
    }
    else if(_Encoding == 2) //AAC LC
    {
        if(_Bitrate != 64)
        {
            return SOAP_FAULT;
        }
    }

    if(_SampleRate != 8 && _SampleRate != 16)
    {
        return SOAP_FAULT;
    }

    add_audio_encoder = (audio_encoder_conf_t *)calloc(1, sizeof(audio_encoder_conf_t));
    strcpy(add_audio_encoder->name, input->Configuration->Name);
    strcpy(add_audio_encoder->token, input->Configuration->token);
    add_audio_encoder->usecount = input->Configuration->UseCount;
    add_audio_encoder->bitrate = input->Configuration->Bitrate;
    add_audio_encoder->samplerate = input->Configuration->SampleRate;
    add_audio_encoder->encoding = input->Configuration->Encoding;

    set_aec_conf(i, add_audio_encoder);
    memcpy(g_nvt->t.profiles[i].aec, add_audio_encoder, sizeof(add_audio_encoder));
    if(add_audio_encoder)
    {
        free(add_audio_encoder);
        add_audio_encoder = NULL;
    }
#endif
    return SOAP_OK;
}

SOAP_FMAC5 int SOAP_FMAC6 __trt__GetCompatibleAudioEncoderConfigurations(struct soap *soap, struct _trt__GetCompatibleAudioEncoderConfigurations *trt__GetCompatibleAudioEncoderConfigurations, struct _trt__GetCompatibleAudioEncoderConfigurationsResponse *GetAEC)
{
    int i;
    int flag = 0;
    char _IPAddr[INFO_LENGTH];
    NET_IPV4 ip;

    for(i = 0; i < g_nvt->t.nprofiles; i++)
    {
        if(strcmp(trt__GetCompatibleAudioEncoderConfigurations->ProfileToken, g_nvt->t.profiles[i].ptoken) == 0)
        {
            flag = 1;
            break;
        }
    }
    if(!flag)
    {
        return SOAP_FAULT;
    }

    audio_encoder_conf_t *info;
    info = (audio_encoder_conf_t *)calloc(1, sizeof(audio_encoder_conf_t));
    get_aec_profiles(i, info, 2);

    ip.int32 =  net_get_ifaddr(ETH_NAME);
    sprintf(_IPAddr, "http://%d.%d.%d.%d/onvif/services", ip.str[0], ip.str[1], ip.str[2], ip.str[3]);

    GetAEC->__sizeConfigurations = 1;

    GetAEC->Configurations = (struct tt__AudioEncoderConfiguration *)soap_malloc(soap, sizeof(struct tt__AudioEncoderConfiguration));
    memset(GetAEC->Configurations, 0, sizeof(struct tt__AudioEncoderConfiguration));
    GetAEC->Configurations->Name = NULL;
    GetAEC->Configurations->UseCount = 1;
    GetAEC->Configurations->token = (char *)soap_malloc(soap, sizeof(char) * SMALL_INFO_LENGTH);info->token;//"audio_encoder_config";
    strncpy(GetAEC->Configurations->token, info->token, SMALL_INFO_LENGTH);
    GetAEC->Configurations->Encoding = info->encoding;

    GetAEC->Configurations->Bitrate = info->bitrate;
    GetAEC->Configurations->SampleRate = info->samplerate;
    GetAEC->Configurations->SessionTimeout = 60;
    GetAEC->Configurations->Multicast = (struct tt__MulticastConfiguration *)soap_malloc(soap, sizeof(struct tt__MulticastConfiguration));
    memset(GetAEC->Configurations->Multicast, 0, sizeof(struct tt__MulticastConfiguration));
    GetAEC->Configurations->Multicast->Port = g_nvt->p.port;
    GetAEC->Configurations->Multicast->TTL = 1;
    GetAEC->Configurations->Multicast->AutoStart = 0;
    GetAEC->Configurations->Multicast->Address  = (struct tt__IPAddress *)soap_malloc(soap, sizeof(struct tt__IPAddress));
    GetAEC->Configurations->Multicast->Address->Type = 0;
    GetAEC->Configurations->Multicast->Address->IPv4Address = (char **)soap_malloc(soap, sizeof(char *));
    GetAEC->Configurations->Multicast->Address->IPv4Address[0] = (char *)soap_malloc(soap, sizeof(char) * INFO_LENGTH);
    strcpy(*GetAEC->Configurations->Multicast->Address->IPv4Address, _IPAddr);
    GetAEC->Configurations->Multicast->Address->IPv6Address = NULL;

    if(info)
    {
        free(info);
        info = NULL;
    }
    return SOAP_OK;
}

SOAP_FMAC5 int SOAP_FMAC6 __trt__GetSnapshotUri(struct soap* soap, struct _trt__GetSnapshotUri *input, struct _trt__GetSnapshotUriResponse *SnapUri)
{
    char _IPAddr[INFO_LENGTH];
    NET_IPV4 ip;
    int i;
    int flag = 0;
    int chs = 0;
    int streamtype = 0;
    char *tmp;
    char _tmp[4];
    char *pos;
    
    if(strlen(input->ProfileToken) == 0)
    {
        fprintf(stderr, "[%s] prifiletoken is NULL!\n", __FUNCTION__);
        return SOAP_FAULT;
    }

    ip.int32 =  net_get_ifaddr(ETH_NAME);
    
    for(i = 0; i < g_nvt->t.nprofiles; i++)
    {
        if(strcmp(input->ProfileToken, g_nvt->t.profiles[i].ptoken) == 0)
        {
            flag = 1;
            if(strstr(input->ProfileToken, "Sub"))
            {
                streamtype = 1;
            }
            //parse chs number
            #if 0
            tmp = input->ProfileToken + 2;
            pos = strchr(tmp, '_');
            chs = (int)(pos - tmp);
            memset(_tmp, 0, sizeof(_tmp));
            strncpy(_tmp, tmp, chs);
            chs = atoi(_tmp);
            #else
            sscanf(input->ProfileToken, "CH%02d", &chs);
            #endif
            
            break;
        }
    }

    if(flag)
    {
        //sprintf(_IPAddr, "http://%d.%d.%d.%d:80/snap/snap_%d_%d.jpg", ip.str[0], ip.str[1], ip.str[2], ip.str[3], chs, streamtype);
        sprintf(_IPAddr, "http://%d.%d.%d.%d:80/snap?args=G0C%dS%d", ip.str[0], ip.str[1], ip.str[2], ip.str[3], chs, streamtype);
        printf("flag:%d, _IPAddr:%s\n", flag, _IPAddr);
        soap->http_version = "1.0";
        SnapUri->MediaUri = (struct tt__MediaUri *)soap_malloc(soap, sizeof(struct tt__MediaUri));
        memset(SnapUri->MediaUri, 0, sizeof(struct tt__MediaUri));
        SnapUri->MediaUri->Uri = (char *) soap_malloc(soap, sizeof(char) * INFO_LENGTH);
        strcpy(SnapUri->MediaUri->Uri, _IPAddr);
        SnapUri->MediaUri->InvalidAfterConnect = __false;
        SnapUri->MediaUri->InvalidAfterReboot = __false;
        SnapUri->MediaUri->Timeout = 0;
    }
    else
    {
        //sprintf(_IPAddr, "http://%d.%d.%d.%d:80/snap/snap_%d_%d.jpg", ip.str[0], ip.str[1], ip.str[2], ip.str[3], chs, streamtype);
        sprintf(_IPAddr, "http://%d.%d.%d.%d:80/snap?args=G0C%dS%d", ip.str[0], ip.str[1], ip.str[2], ip.str[3], chs, streamtype);
        printf("flag:%d, _IPAddr:%s\n", flag, _IPAddr);
        soap->http_version = "1.0";
        SnapUri->MediaUri = (struct tt__MediaUri *)soap_malloc(soap, sizeof(struct tt__MediaUri));
        memset(SnapUri->MediaUri, 0, sizeof(struct tt__MediaUri));
        SnapUri->MediaUri->Uri = (char *) soap_malloc(soap, sizeof(char) * INFO_LENGTH);
        strcpy(SnapUri->MediaUri->Uri, _IPAddr);
        SnapUri->MediaUri->InvalidAfterConnect = __false;
        SnapUri->MediaUri->InvalidAfterReboot = __false;
        SnapUri->MediaUri->Timeout = 0;
    }

    return SOAP_OK;
}

/* Get the ImagingConfiguration for the requested VideoSource */
SOAP_FMAC5 int SOAP_FMAC6 __timg__GetImagingSettings(struct soap *soap, struct _timg__GetImagingSettings *timg__GetImagingSettings, struct _timg__GetImagingSettingsResponse *GetImgSetting)
{
    int i;
    int flag = 0;

    for(i = 0; i < g_nvt->t.nprofiles; i++)
    {
        if(strcmp(timg__GetImagingSettings->VideoSourceToken, g_nvt->t.profiles[i].vsc->vsourcetoken) == 0)
        {
            flag = 1;
            break;
        }
    }

    if(!flag)
    {
        return SOAP_FAULT;
    }

    get_imaging_conf(i, g_nvt->t.profiles[i].img, 2);

    GetImgSetting->ImagingSettings = (struct tt__ImagingSettings20 *)soap_malloc(soap, sizeof(struct tt__ImagingSettings20));
    memset(GetImgSetting->ImagingSettings, 0, sizeof(struct tt__ImagingSettings20));

#if 0
    GetImgSetting->ImagingSettings->WideDynamicRange = (struct tt__WideDynamicRange20 *)soap_malloc(soap, sizeof(struct tt__WideDynamicRange20));
    GetImgSetting->ImagingSettings->WideDynamicRange->Mode = 0;
    GetImgSetting->ImagingSettings->WideDynamicRange->Level = 0;

    GetImgSetting->ImagingSettings->WhiteBalance = (struct tt__WhiteBalance20 *)soap_malloc(soap, sizeof(struct tt__WhiteBalance20));
    GetImgSetting->ImagingSettings->WhiteBalance->CrGain = (float *) soap_malloc(soap, sizeof(float));
    GetImgSetting->ImagingSettings->WhiteBalance->CbGain = (float *) soap_malloc(soap, sizeof(float));
    GetImgSetting->ImagingSettings->WhiteBalance->Mode = 0;
    *GetImgSetting->ImagingSettings->WhiteBalance->CrGain = 0;
    *GetImgSetting->ImagingSettings->WhiteBalance->CbGain = 0;
    GetImgSetting->ImagingSettings->WhiteBalance->Extension = NULL;

    GetImgSetting->ImagingSettings->BacklightCompensation = (struct tt__BacklightCompensation20 *)soap_malloc(soap, sizeof(struct tt__BacklightCompensation20));
    GetImgSetting->ImagingSettings->BacklightCompensation->Level =(float *) soap_malloc(soap, sizeof(float));
    GetImgSetting->ImagingSettings->BacklightCompensation->Mode = 0;
    *GetImgSetting->ImagingSettings->BacklightCompensation->Level = 0;
#endif

    GetImgSetting->ImagingSettings->Brightness = (float *)soap_malloc(soap, sizeof(float));
    *GetImgSetting->ImagingSettings->Brightness = g_nvt->t.profiles[i].img->brightness;
    GetImgSetting->ImagingSettings->ColorSaturation = (float *)soap_malloc(soap, sizeof(float));
    *GetImgSetting->ImagingSettings->ColorSaturation = g_nvt->t.profiles[i].img->ColorSaturation;
    GetImgSetting->ImagingSettings->Contrast = (float *)soap_malloc(soap, sizeof(float));
    *GetImgSetting->ImagingSettings->Contrast = g_nvt->t.profiles[i].img->contrast;
    GetImgSetting->ImagingSettings->Sharpness = (float *)soap_malloc(soap, sizeof(float));
    *GetImgSetting->ImagingSettings->Sharpness = g_nvt->t.profiles[i].img->sharpness;
    GetImgSetting->ImagingSettings->IrCutFilter = (enum tt__IrCutFilterMode *)soap_malloc(soap, sizeof(enum tt__IrCutFilterMode));
    *GetImgSetting->ImagingSettings->IrCutFilter = g_nvt->t.profiles[i].img->IrCtFilter;

    GetImgSetting->ImagingSettings->WhiteBalance = NULL;
    GetImgSetting->ImagingSettings->BacklightCompensation = NULL;
    GetImgSetting->ImagingSettings->WideDynamicRange = NULL;
    GetImgSetting->ImagingSettings->WideDynamicRange = NULL;
    GetImgSetting->ImagingSettings->Exposure = NULL;
    GetImgSetting->ImagingSettings->Focus = NULL;
    GetImgSetting->ImagingSettings->Extension = NULL;

    return SOAP_OK;
}

SOAP_FMAC5 int SOAP_FMAC6 __timg__SetImagingSettings(struct soap *soap, struct _timg__SetImagingSettings *input, struct _timg__SetImagingSettingsResponse *output)
{
    int i;
    int flag = 0;

    imaging_conf_t *img;

    //fprintf(stderr, "[%s][%d] nprofiles:%d !\n", __FUNCTION__, __LINE__, g_nvt->t.nprofiles);
    for(i = 0; i < g_nvt->t.nprofiles; i++)
    {
        if(strcmp(input->VideoSourceToken, g_nvt->t.profiles[i].vsc->vsourcetoken) == 0)
        {
            flag = 1;
            break;
        }
    }
    if(!flag)
    {
        return SOAP_FAULT;
    }

    img = (imaging_conf_t *)calloc(1, sizeof(imaging_conf_t));

    img->brightness = (input->ImagingSettings->Brightness)?(*input->ImagingSettings->Brightness):0;
    img->ColorSaturation = (input->ImagingSettings->ColorSaturation)?(*input->ImagingSettings->ColorSaturation):0;
    img->contrast = (input->ImagingSettings->Contrast)?(*input->ImagingSettings->Contrast):0;
    img->IrCtFilter = (input->ImagingSettings->IrCutFilter)?(*input->ImagingSettings->IrCutFilter):0;
    img->sharpness = (input->ImagingSettings->Sharpness)?(*input->ImagingSettings->Sharpness):0;

    memcpy(g_nvt->t.profiles[i].img, img, sizeof(imaging_conf_t));
    set_imaging_conf(i, img);
    if(img)
    {
        free(img);
        img = NULL;
    }
    return SOAP_OK;
}

SOAP_FMAC5 int SOAP_FMAC6 __timg__GetOptions(struct soap *soap, struct _timg__GetOptions *timg__GetOptions, struct _timg__GetOptionsResponse *output)
{
    int i;
    int flag = 0;

    for(i = 0; i < g_nvt->t.nprofiles; i++)
    {
        if(strcmp(timg__GetOptions->VideoSourceToken, g_nvt->t.profiles[i].vsc->vsourcetoken) == 0)
        {
            flag = 1;
            break;
        }
    }
    if(!flag)
    {
        return SOAP_FAULT;
    }

    output->ImagingOptions =  (struct tt__ImagingOptions20 *)soap_malloc(soap, sizeof(struct tt__ImagingOptions20));
#if 0
    output->ImagingOptions->BacklightCompensation = (struct tt__BacklightCompensationOptions20 *)soap_malloc(soap, sizeof(struct tt__BacklightCompensationOptions20));
    output->ImagingOptions->BacklightCompensation->__sizeMode = 1;
    output->ImagingOptions->BacklightCompensation->Mode = (enum tt__BacklightCompensationMode *) soap_malloc(soap, sizeof(int));
    output->ImagingOptions->BacklightCompensation->Mode = NULL;
    output->ImagingOptions->BacklightCompensation->Level = NULL;
#endif
    output->ImagingOptions->BacklightCompensation = NULL;

    output->ImagingOptions->Brightness = (struct tt__FloatRange *)soap_malloc(soap, sizeof(struct tt__FloatRange));
    output->ImagingOptions->Brightness->Min = 1;
    output->ImagingOptions->Brightness->Max = 100;

    output->ImagingOptions->Contrast = (struct tt__FloatRange *)soap_malloc(soap, sizeof(struct tt__FloatRange));
    output->ImagingOptions->Contrast->Min = 1;
    output->ImagingOptions->Contrast->Max = 100;

    output->ImagingOptions->Sharpness = (struct tt__FloatRange *)soap_malloc(soap, sizeof(struct tt__FloatRange));
    output->ImagingOptions->Sharpness->Min = 1;
    output->ImagingOptions->Sharpness->Max = 100;

    output->ImagingOptions->ColorSaturation = (struct tt__FloatRange *)soap_malloc(soap, sizeof(struct tt__FloatRange));
    output->ImagingOptions->ColorSaturation->Min = 1;
    output->ImagingOptions->ColorSaturation->Max = 100;
    output->ImagingOptions->__sizeIrCutFilterModes = 1;

    output->ImagingOptions->IrCutFilterModes = NULL;
    output->ImagingOptions->Exposure = NULL;
    output->ImagingOptions->Focus = NULL;
    output->ImagingOptions->WideDynamicRange = NULL;
    output->ImagingOptions->WhiteBalance = NULL;
    output->ImagingOptions->Extension = NULL;

    return SOAP_OK;
}

SOAP_FMAC5 int SOAP_FMAC6 __timg__GetMoveOptions(struct soap *soap, struct _timg__GetMoveOptions *timg__GetMoveOptions, struct _timg__GetMoveOptionsResponse *timg__GetMoveOptionsResponse)
{
    return SOAP_FAULT;
}

/**************************************************
 *
 *
 * NVS 
 *
 *
\**************************************************/

//Recording search

nvt_time_t get_recording_starttime()
{
    struct tm now;
    time_t cur_time = time(NULL);
    gmtime_r(&cur_time, &now);
    nvt_time_t starttm;
    now.tm_mday = 8;
    now.tm_year = 113;
    now.tm_mon = 6;
    now.tm_hour = 22;
    
    starttm.day = now.tm_mday - 2;
    starttm.hour = 0;
    starttm.min = 0;
    starttm.year = now.tm_year + 1900;
    starttm.sec = 0;
    starttm.mon = now.tm_mon + 1;

    return starttm;
}

nvt_time_t get_recording_stoptime()
{
    struct tm now;
    time_t cur_time = time(NULL);
    gmtime_r(&cur_time, &now);
    nvt_time_t stoptm;
    
    now.tm_mday = 8;
    now.tm_year = 113;
    now.tm_mon = 6;
    now.tm_hour = 22;
    
    stoptm.day = now.tm_mday;
    stoptm.hour = now.tm_hour;
    stoptm.min = now.tm_min;
    stoptm.mon = now.tm_mon + 1;
    stoptm.sec = now.tm_sec;
    stoptm.year = now.tm_year + 1900;
    return stoptm;
}

int check_recordingtoken_exist(char *token)
{ 
    int i, j, k = 0;
    int ret;
    int token_exist = -1;
    char *_token;
    nvt_time_t starttime = get_recording_starttime();
    nvt_time_t stoptime = get_recording_stoptime();

    get_recording_list(starttime, stoptime);
    int sum = g_nvt->t.nrecordings;
    int chs = get_curr_chs();

    if(sum)
    {
        for(i = 0; i < chs; i++)
        {
            for(j = 0; j < sum; j++)
            {
                if(strlen(g_nvt->t.record[i][j].token) > 0)
                {
                    _token = g_nvt->t.record[i][j].token;
                    if(strcmp(_token, token) == 0)
                    {
                        token_exist = 1;
                    }
                }
                else
                    break;
            }
            if(token_exist)
                break;  
        }
    }

    return token_exist;
}


SOAP_FMAC5 int SOAP_FMAC6 __tse__GetServiceCapabilities(struct soap *soap, struct _tse__GetServiceCapabilities *input, struct _tse__GetServiceCapabilitiesResponse *output)
{
    output->Capabilities = (struct tse__Capabilities*)soap_malloc(soap, sizeof(struct tse__Capabilities));
    output->Capabilities->MetadataSearch= (enum xsd__boolean_ *)soap_malloc(soap, sizeof(int));
    *output->Capabilities->MetadataSearch = FALSE;
    output->Capabilities->__any = NULL;
    output->Capabilities->__anyAttribute = NULL;
    output->Capabilities->__size = 1;
    return SOAP_OK;
}

SOAP_FMAC5 int SOAP_FMAC6 __tse__GetRecordingSummary(struct soap *soap, struct _tse__GetRecordingSummary *input, struct _tse__GetRecordingSummaryResponse *output)
{
    time_t earliest;
    time_t recent;
    int i, j, k = 0;

    nvt_time_t starttime = get_recording_starttime();
    nvt_time_t stoptime = get_recording_stoptime();

    get_recording_list(starttime, stoptime);
    int sum = g_nvt->t.nrecordings;
    int chs = get_curr_chs();

    output->Summary = (struct tt__RecordingSummary*)soap_malloc(soap, sizeof(struct tt__RecordingSummary));
    if(sum)
    {
        for(i = 0; i < chs; i++)
        {
            for(j = 0; j < sum; j++)
            {
                if(strlen(g_nvt->t.record[i][j].token) > 0)
                {
                    if(k == 0)
                    {
                        earliest = g_nvt->t.record[i][j].start_time;
                        recent = g_nvt->t.record[i][j].stop_time;
                    }
                    k = 1;
                    if(earliest > g_nvt->t.record[i][j].start_time)
                    {
                        earliest = g_nvt->t.record[i][j].start_time;
                    }   
                    else if(recent < g_nvt->t.record[i][j].stop_time)
                    {
                        recent = g_nvt->t.record[i][j].stop_time;
                    }
                }
                else
                    break;
            }
        }
        output->Summary->DataFrom = earliest;//The earliest point in time where there is recorded data on the device.
        output->Summary->DataUntil = recent;//The most recent point in time where there is recorded data on the device.
    }
    else
    {
        output->Summary->DataFrom = time(&earliest);
        output->Summary->DataUntil = time(&recent);
    }
    output->Summary->NumberRecordings = g_nvt->t.nrecordings;
    output->Summary->__any = NULL;
    output->Summary->__anyAttribute = NULL;
    output->Summary->__size = 1;
    return SOAP_OK;
}

SOAP_FMAC5 int SOAP_FMAC6 __trc__GetRecordings(struct soap *soap, struct _trc__GetRecordings *input, struct _trc__GetRecordingsResponse *GetRecordingResp)
{
#if 1
    char _IPAddr[LARGE_INFO_LENGTH];
    NET_IPV4 ip;
    int sum, i, j;
    int chs;
    int k = 0;
    char *token;

    nvt_time_t starttime = get_recording_starttime();
    nvt_time_t stoptime = get_recording_stoptime();

    ip.int32 =  net_get_ifaddr(ETH_NAME);
    sprintf(_IPAddr, "http://%d.%d.%d.%d/onvif/media", ip.str[0], ip.str[1], ip.str[2], ip.str[3]);

#if 1
    get_recording_list(starttime, stoptime);
    sum = g_nvt->t.nrecordings;
    chs = get_curr_chs();

    GetRecordingResp->__sizeRecordingItem = sum;
    GetRecordingResp->RecordingItem = (struct tt__GetRecordingsResponseItem *)soap_malloc(soap, sum * sizeof(struct tt__GetRecordingsResponseItem));
    memset(GetRecordingResp->RecordingItem, 0, sum * sizeof(struct tt__GetRecordingsResponseItem));  

    if(sum)
    {
        for(i = 0; i < chs; i++)
        {
            for(j = 0; j < MAX_RECORDING; j++)
            {
                if(strlen(g_nvt->t.record[i][j].token) > 0)
                {
                    GetRecordingResp->RecordingItem[k].RecordingToken = (char *)soap_malloc(soap, sizeof(char) * INFO_LENGTH);
                    memset(GetRecordingResp->RecordingItem[k].RecordingToken, 0, sizeof(GetRecordingResp->RecordingItem[k].RecordingToken));
                    token = g_nvt->t.record[i][j].token;
                    strcpy(GetRecordingResp->RecordingItem[k].RecordingToken, token);    
                    k++;
                }
                else
                {
                    break;
                }
                
            }
        }
    }

#else
    sum = 1;
    GetRecordingResp->__sizeRecordingItem = sum;
    GetRecordingResp->RecordingItem = (struct tt__GetRecordingsResponseItem *)soap_malloc(soap, sum * sizeof(struct tt__GetRecordingsResponseItem));
    memset(GetRecordingResp->RecordingItem, 0, sum * sizeof(struct tt__GetRecordingsResponseItem));  
#endif
    for(i = 0; i < sum; ++i)
    {
#if 0
        GetRecordingResp->RecordingItem[i].RecordingToken = (char *)soap_malloc(soap, sizeof(char) * INFO_LENGTH);
        memset(GetRecordingResp->RecordingItem[i].RecordingToken, 0, sizeof(GetRecordingResp->RecordingItem[i].RecordingToken));
        strcpy(GetRecordingResp->RecordingItem[i].RecordingToken, "test token");
#endif
        GetRecordingResp->RecordingItem[i].Configuration = (struct tt__RecordingConfiguration *)soap_malloc(soap, sizeof(struct tt__RecordingConfiguration));
        memset(GetRecordingResp->RecordingItem[i].Configuration, 0, sizeof(struct tt__RecordingConfiguration));
        GetRecordingResp->RecordingItem[i].Tracks = (struct tt__GetTracksResponseList *)soap_malloc(soap, sizeof(struct tt__GetTracksResponseList));
        memset(GetRecordingResp->RecordingItem[i].Tracks, 0, sizeof(struct tt__GetTracksResponseList));
#if 1
        GetRecordingResp->RecordingItem[i].__any = NULL;
        GetRecordingResp->RecordingItem[i].__size = 1;
        GetRecordingResp->RecordingItem[i].__anyAttribute = NULL;
#endif
        GetRecordingResp->RecordingItem[i].Configuration->Content = (char *)soap_malloc(soap, sizeof(char) * INFO_LENGTH);
        strcpy(GetRecordingResp->RecordingItem[i].Configuration->Content, "Content");//Informative description of the source.
        GetRecordingResp->RecordingItem[i].Configuration->MaximumRetentionTime = 72000;

        GetRecordingResp->RecordingItem[i].Configuration->Source = (struct tt__RecordingSourceInformation*)soap_malloc(soap, sizeof(struct tt__RecordingSourceInformation));
        GetRecordingResp->RecordingItem[i].Configuration->Source->Address = (char *)soap_malloc(soap, sizeof(char) * INFO_LENGTH);
        strncpy(GetRecordingResp->RecordingItem[i].Configuration->Source->Address, _IPAddr, INFO_LENGTH);
        GetRecordingResp->RecordingItem[i].Configuration->Source->Description = (char *)soap_malloc(soap, sizeof(char) * INFO_LENGTH);
        strncpy(GetRecordingResp->RecordingItem[i].Configuration->Source->Description, "Description", INFO_LENGTH);
        GetRecordingResp->RecordingItem[i].Configuration->Source->Location = (char *)soap_malloc(soap, sizeof(char) * INFO_LENGTH);
        strncpy(GetRecordingResp->RecordingItem[i].Configuration->Source->Location, "Location", INFO_LENGTH);
        GetRecordingResp->RecordingItem[i].Configuration->Source->Name = (char *)soap_malloc(soap, sizeof(char) * INFO_LENGTH);
        strncpy(GetRecordingResp->RecordingItem[i].Configuration->Source->Name, "Name", INFO_LENGTH);
        GetRecordingResp->RecordingItem[i].Configuration->Source->SourceId = (char *)soap_malloc(soap, sizeof(char) * INFO_LENGTH);
        strncpy(GetRecordingResp->RecordingItem[i].Configuration->Source->SourceId, "VIDEO001", INFO_LENGTH);
#if 1
        GetRecordingResp->RecordingItem[i].Configuration->Source->__any = NULL;
        GetRecordingResp->RecordingItem[i].Configuration->Source->__anyAttribute = NULL;
        GetRecordingResp->RecordingItem[i].Configuration->Source->__size = 1;

        GetRecordingResp->RecordingItem[i].Configuration->__any = NULL;
        GetRecordingResp->RecordingItem[i].Configuration->__size = 1;
        GetRecordingResp->RecordingItem[i].Configuration->__anyAttribute = NULL;
#endif
        GetRecordingResp->RecordingItem[i].Tracks->Track = (struct tt__GetTracksResponseItem *)soap_malloc(soap, sizeof(struct tt__GetTracksResponseItem));
        memset(GetRecordingResp->RecordingItem[i].Tracks->Track, 0, sizeof(struct tt__GetTracksResponseItem));
        GetRecordingResp->RecordingItem[i].Tracks->Track->__size = 1; 
        GetRecordingResp->RecordingItem[i].Tracks->Track->Configuration = (struct tt__TrackConfiguration*)soap_malloc(soap, sizeof(struct tt__TrackConfiguration));
        memset(GetRecordingResp->RecordingItem[i].Tracks->Track->Configuration, 0, sizeof(struct tt__TrackConfiguration));
        GetRecordingResp->RecordingItem[i].Tracks->Track->Configuration->Description = (char *)soap_malloc(soap, sizeof(char) * LARGE_INFO_LENGTH);
        strcpy(GetRecordingResp->RecordingItem[i].Tracks->Track->Configuration->Description, "VIDEO001");
        GetRecordingResp->RecordingItem[i].Tracks->Track->Configuration->TrackType = tt__TrackType__Video;
        GetRecordingResp->RecordingItem[i].Tracks->Track->Configuration->__any = NULL;
        GetRecordingResp->RecordingItem[i].Tracks->Track->Configuration->__anyAttribute = NULL;
        GetRecordingResp->RecordingItem[i].Tracks->Track->Configuration->__size = 1;
        GetRecordingResp->RecordingItem[i].Tracks->Track->TrackToken = (char *)soap_malloc(soap, sizeof(char) * LARGE_INFO_LENGTH);
        strcpy(GetRecordingResp->RecordingItem[i].Tracks->Track->TrackToken, "VIDEO001");
#if 1
        GetRecordingResp->RecordingItem[i].Tracks->Track->__any = NULL;
        GetRecordingResp->RecordingItem[i].Tracks->Track->__anyAttribute = NULL;
        GetRecordingResp->RecordingItem[i].Tracks->Track->__size = 1;

        GetRecordingResp->RecordingItem[i].Tracks->__anyAttribute = NULL;
        GetRecordingResp->RecordingItem[i].Tracks->__sizeTrack = 1;
#endif
    }
#endif
    return SOAP_OK;
}

/*
    FindRecordings starts a search session, looking for recordings that matches the scope (See 20.2.4) defined in the request. 
    Results from the search are acquired using the GetRecordingSearchResults request, specifying the search token returned from this request. 
    The device shall continue searching until one of the following occurs:
    ■The entire time range from StartPoint to EndPoint has been searched through.
    ■The total number of matches has been found, defined by the MaxMatches parameter.
    ■The session has been ended by a client EndSession request.
    ■The session has been ended because KeepAliveTime since the last request related to this session has expired.
    The order of the results is undefined, to allow the device to return results in any order they are found. This operation is mandatory to support for a device implementing the recording search service.
*/
SOAP_FMAC5 int SOAP_FMAC6 __tse__FindRecordings(struct soap *soap, struct _tse__FindRecordings *input, struct _tse__FindRecordingsResponse *output)
{
    output->SearchToken = (char *)soap_malloc(soap, sizeof(char) * INFO_LENGTH);
#if 1
    if(input->Scope->IncludedSources != NULL)
    {
        if(input->Scope->IncludedSources->Token)
        {
            strcpy(output->SearchToken, input->Scope->IncludedSources->Token);
        }  
    }
    else
    {
        int sum = 0;
        int chs;
        int i, j;
        char *token;
        int flag = 0;

        nvt_time_t starttime = get_recording_starttime();
        nvt_time_t stoptime = get_recording_stoptime();

        get_recording_list(starttime, stoptime);
        sum = g_nvt->t.nrecordings;
        chs = get_curr_chs();
        if(sum)
        {
            for(i = 0; i < chs; i++)
            {
                for(j = 0; j < sum; j++)
                {
                    if(strlen(g_nvt->t.record[i][j].token) > 0)
                    {
                        flag = 1;
                        token = g_nvt->t.record[i][j].token;
                        break;
                    }
                    else
                        break;
                }
                if(flag)
                    break;
            }
        }
        else
        {
            return SOAP_FAULT;
        }

        strcpy(output->SearchToken, token);
    }
#else
    int sum = 0;
    int chs;
    int i, j;
    char *token;
    int flag = 0;
    int k = 0;

    if(input->Scope->IncludedSources == NULL) //return all recordings
    {
        nvt_time_t starttime = get_recording_starttime();
        nvt_time_t stoptime = get_recording_stoptime();

        get_recording_list(starttime, stoptime);
        sum = g_nvt->t.nrecordings;
        chs = get_curr_chs();

        output->SearchToken = (char *)soap_malloc(soap, sizeof(char) * INFO_LENGTH);
        if(sum)
        {
            for(i = 0; i < chs; i++)
            {
                for(j = 0; j < sum; j++)
                {
                    if(strlen(g_nvt->t.record[i][j].token) > 0)
                    {
                        token = g_nvt->t.record[i][j].token;
                        strcpy(output->SearchToken[k], token);
                        k++;
                    }
                    else
                        break;
                }
            }
        }
        else
        {
            return SOAP_FAULT;
        }
    }
    else
    {
        output->SearchToken = (char *)soap_malloc(soap, sizeof(char) * INFO_LENGTH);

        if(input->Scope->IncludedSources->Token)
        {
            strcpy(output->SearchToken, input->Scope->IncludedSources->Token);
        }
    }
#endif

    return SOAP_OK;
}

SOAP_FMAC5 int SOAP_FMAC6 __tse__EndSearch(struct soap *soap, struct _tse__EndSearch *input, struct _tse__EndSearchResponse *output)
{
    if(!input->SearchToken)
    {
        return SOAP_FAULT;
    }
    else if(check_recordingtoken_exist(input->SearchToken) < 0)
    {
        return SOAP_FAULT;
    }

    time_t now;
    time(&now);
    output->Endpoint = now;//stop now!
    return SOAP_OK;
}

SOAP_FMAC5 int SOAP_FMAC6 __tse__GetRecordingSearchResults(struct soap *soap, struct _tse__GetRecordingSearchResults *input, struct _tse__GetRecordingSearchResultsResponse *output)
{
    if(!input->SearchToken)
    {
        return SOAP_FAULT;
    }

    nvt_time_t starttime = get_recording_starttime();
    nvt_time_t stoptime = get_recording_stoptime();
    int i, j;
    get_recording_list(starttime, stoptime);
    int chs = get_curr_chs();
    char *_token;
    int sum = g_nvt->t.nrecordings;
    int token_exist = 0;
    int k = 0;
#if 0
    output->ResultList = (struct tt__FindRecordingResultList*)soap_malloc(soap, sizeof(struct tt__FindRecordingResultList));
    memset(output->ResultList, 0, sizeof(struct tt__FindRecordingResultList));

    output->ResultList->__sizeRecordingInformation = sum;

    output->ResultList->RecordingInformation = (struct tt__RecordingInformation *)soap_malloc(soap, sum * sizeof(struct tt__RecordingInformation));
    memset(output->ResultList->RecordingInformation, 0, sum * sizeof(struct tt__RecordingInformation));
#endif

    for(i = 0; i < chs; i++)
    {
        for(j = 0; j < sum; j++)
        {
            if(strlen(g_nvt->t.record[i][j].token) > 0)
            {
                _token = g_nvt->t.record[i][j].token;    
#if 0    
                output->ResultList->RecordingInformation[k].RecordingToken = (char *)soap_malloc(soap, sizeof(char) * INFO_LEN);
                strcpy(output->ResultList->RecordingInformation[k].RecordingToken, _token);
                output->ResultList->RecordingInformation[k].EarliestRecording = (time_t *)soap_malloc(soap, sizeof(time_t));
                *output->ResultList->RecordingInformation[k].EarliestRecording = g_nvt->t.record[i][j].start_time;
                output->ResultList->RecordingInformation[k].LatestRecording = (time_t *)soap_malloc(soap, sizeof(time_t));
                *output->ResultList->RecordingInformation[k].LatestRecording = g_nvt->t.record[i][j].stop_time;
                k++;
#endif
                if(!token_exist)
                {
                    if(strcmp(_token, input->SearchToken) == 0)
                    {
                        token_exist = 1;
                    }
                }
            }
            else
                break;
        }
    }

    if(!token_exist)
    {
       return SOAP_FAULT; //input token is wrong
    }

    char _IPAddr[LARGE_INFO_LENGTH];
    NET_IPV4 ip;
    ip.int32 =  net_get_ifaddr(ETH_NAME);
    sprintf(_IPAddr, "%d.%d.%d.%d", ip.str[0], ip.str[1], ip.str[2], ip.str[3]);

 #if 1   
    recording_info_t *info = (recording_info_t *)calloc(1, sizeof(recording_info_t));
    get_recording_info(info, input->SearchToken);

    output->ResultList = (struct tt__FindRecordingResultList*)soap_malloc(soap, sizeof(struct tt__FindRecordingResultList));
    memset(output->ResultList, 0, sizeof(struct tt__FindRecordingResultList));

    output->ResultList->RecordingInformation = (struct tt__RecordingInformation *)soap_malloc(soap, sizeof(struct tt__RecordingInformation));
    memset(output->ResultList->RecordingInformation, 0, sizeof(struct tt__RecordingInformation));

    output->ResultList->__sizeRecordingInformation = 1;

    output->ResultList->RecordingInformation->RecordingToken = (char *)soap_malloc(soap, sizeof(char) * INFO_LEN);
    strcpy(output->ResultList->RecordingInformation->RecordingToken, input->SearchToken);
    output->ResultList->RecordingInformation->Content = (char *)soap_malloc(soap, sizeof(char) * INFO_LEN);
    strcpy(output->ResultList->RecordingInformation->Content, "Content");
    output->ResultList->RecordingInformation->EarliestRecording = (time_t *)soap_malloc(soap, sizeof(time_t));
    *output->ResultList->RecordingInformation->EarliestRecording = info->start_time;
    output->ResultList->RecordingInformation->LatestRecording = (time_t *)soap_malloc(soap, sizeof(time_t));
    *output->ResultList->RecordingInformation->LatestRecording = info->stop_time;
    output->ResultList->RecordingInformation->RecordingStatus = tt__RecordingStatus__Stopped;

    output->ResultList->RecordingInformation->Source = (struct tt__RecordingSourceInformation*)soap_malloc(soap, sizeof(struct tt__RecordingSourceInformation));
    memset(output->ResultList->RecordingInformation->Source, 0, sizeof(struct tt__RecordingSourceInformation));
    output->ResultList->RecordingInformation->Source->Address = (char *)soap_malloc(soap, sizeof(char) * INFO_LEN);
    strcpy(output->ResultList->RecordingInformation->Source->Address, _IPAddr);
    output->ResultList->RecordingInformation->Source->Description = (char *)soap_malloc(soap, sizeof(char) * INFO_LEN);
    strcpy(output->ResultList->RecordingInformation->Source->Description, "Description");
    output->ResultList->RecordingInformation->Source->Location = (char *)soap_malloc(soap, sizeof(char) * INFO_LEN);
    strcpy(output->ResultList->RecordingInformation->Source->Location, "Location");
    output->ResultList->RecordingInformation->Source->Name = (char *)soap_malloc(soap, sizeof(char) * INFO_LEN);
    strcpy(output->ResultList->RecordingInformation->Source->Name, "Name");
    output->ResultList->RecordingInformation->Source->SourceId = (char *)soap_malloc(soap, sizeof(char) * INFO_LEN);
    strcpy(output->ResultList->RecordingInformation->Source->SourceId, "VIDEO001");
    output->ResultList->RecordingInformation->Source->__any = NULL;
    output->ResultList->RecordingInformation->Source->__anyAttribute= NULL;
    output->ResultList->RecordingInformation->Source->__size = 1;

    output->ResultList->RecordingInformation->Track = (struct tt__TrackInformation*)soap_malloc(soap, sizeof(struct tt__TrackInformation));
    memset(output->ResultList->RecordingInformation->Track, 0, sizeof(struct tt__TrackInformation));
    output->ResultList->RecordingInformation->Track->DataFrom = info->start_time;//The start date and time of the oldest recorded data in the track.
    output->ResultList->RecordingInformation->Track->DataTo = info->stop_time;//The stop date and time of the newest recorded data in the track.
    output->ResultList->RecordingInformation->Track->Description =(char *)soap_malloc(soap, sizeof(char) * INFO_LEN);
    strcpy(output->ResultList->RecordingInformation->Track->Description, "VIDEO001"); 
    output->ResultList->RecordingInformation->Track->TrackToken = (char *)soap_malloc(soap, sizeof(char) * INFO_LEN);
    strcpy(output->ResultList->RecordingInformation->Track->TrackToken, "VIDEO001"); 
    output->ResultList->RecordingInformation->Track->TrackType = tt__TrackType__Video;//tt__TrackType__Video = 0, tt__TrackType__Audio = 1, tt__TrackType__Metadata = 2, tt__TrackType__Extended = 3
    output->ResultList->RecordingInformation->Track->__any = NULL;
    output->ResultList->RecordingInformation->Track->__anyAttribute = NULL;
    output->ResultList->RecordingInformation->Track->__size = 1;

    output->ResultList->RecordingInformation->__any = NULL;
    output->ResultList->RecordingInformation->__anyAttribute = NULL;
    output->ResultList->RecordingInformation->__size = 1;
    output->ResultList->RecordingInformation->__sizeTrack = 1;
    if(!strcmp(input->SearchToken, _token))
    {
        output->ResultList->SearchState = tt__SearchState__Completed; //find last recording
    }
    else
        output->ResultList->SearchState = tt__SearchState__Searching;

    if(info)
    {
        free(info);
        info = NULL;
    }
#else
  
    for(i = 0; i < sum; i++)
    {
        output->ResultList->RecordingInformation[i].Content = (char *)soap_malloc(soap, sizeof(char) * INFO_LEN);
        strcpy(output->ResultList->RecordingInformation[i].Content, "Content");

        output->ResultList->RecordingInformation[i].RecordingStatus = tt__RecordingStatus__Stopped;

        output->ResultList->RecordingInformation[i].Source = (struct tt__RecordingSourceInformation*)soap_malloc(soap, sizeof(struct tt__RecordingSourceInformation));
        memset(output->ResultList->RecordingInformation[i].Source, 0, sizeof(struct tt__RecordingSourceInformation));
        output->ResultList->RecordingInformation[i].Source->Address = (char *)soap_malloc(soap, sizeof(char) * INFO_LEN);
        strcpy(output->ResultList->RecordingInformation[i].Source->Address, _IPAddr);
        output->ResultList->RecordingInformation[i].Source->Description = (char *)soap_malloc(soap, sizeof(char) * INFO_LEN);
        strcpy(output->ResultList->RecordingInformation[i].Source->Description, "Description");
        output->ResultList->RecordingInformation[i].Source->Location = (char *)soap_malloc(soap, sizeof(char) * INFO_LEN);
        strcpy(output->ResultList->RecordingInformation[i].Source->Location, "Location");
        output->ResultList->RecordingInformation[i].Source->Name = (char *)soap_malloc(soap, sizeof(char) * INFO_LEN);
        strcpy(output->ResultList->RecordingInformation[i].Source->Name, "Location");
        output->ResultList->RecordingInformation[i].Source->SourceId = (char *)soap_malloc(soap, sizeof(char) * INFO_LEN);
        strcpy(output->ResultList->RecordingInformation[i].Source->SourceId, "VIDEO001");
        output->ResultList->RecordingInformation[i].Source->__any = NULL;
        output->ResultList->RecordingInformation[i].Source->__anyAttribute= NULL;
        output->ResultList->RecordingInformation[i].Source->__size = 1;

        output->ResultList->RecordingInformation[i].Track = (struct tt__TrackInformation*)soap_malloc(soap, sizeof(struct tt__TrackInformation));
        memset(output->ResultList->RecordingInformation[i].Track, 0, sizeof(struct tt__TrackInformation));
        output->ResultList->RecordingInformation[i].Track->DataFrom = output->ResultList->RecordingInformation[k].EarliestRecording;
        output->ResultList->RecordingInformation[i].Track->DataTo = output->ResultList->RecordingInformation[k].LatestRecording;
        output->ResultList->RecordingInformation[i].Track->Description =(char *)soap_malloc(soap, sizeof(char) * INFO_LEN);
        strcpy(output->ResultList->RecordingInformation[i].Track->Description, "VIDEO001"); 
        output->ResultList->RecordingInformation[i].Track->TrackToken = (char *)soap_malloc(soap, sizeof(char) * INFO_LEN);
        strcpy(output->ResultList->RecordingInformation[i].Track->TrackToken, "VIDEO001"); 
        output->ResultList->RecordingInformation[i].Track->TrackType = tt__TrackType__Video;//tt__TrackType__Video = 0, tt__TrackType__Audio = 1, tt__TrackType__Metadata = 2, tt__TrackType__Extended = 3
        output->ResultList->RecordingInformation[i].Track->__any = NULL;
        output->ResultList->RecordingInformation[i].Track->__anyAttribute = NULL;
        output->ResultList->RecordingInformation[i].Track->__size = 1;

        output->ResultList->RecordingInformation[i].__any = NULL;
        output->ResultList->RecordingInformation[i].__anyAttribute = NULL;
        output->ResultList->RecordingInformation[i].__size = 1;
        output->ResultList->RecordingInformation[i].__sizeTrack = 1;
    }
    output->ResultList->SearchState = tt__SearchState__Completed;
#endif

    return SOAP_OK;
}

SOAP_FMAC5 int SOAP_FMAC6 __tse__GetRecordingInformation(struct soap *soap, struct _tse__GetRecordingInformation *input, struct _tse__GetRecordingInformationResponse *output)
{
#if 0
    if(!input->RecordingToken)
    {
        return SOAP_FAULT;
    }
    else if(check_recordingtoken_exist(input->RecordingToken) < 0)
    {
        return SOAP_FAULT;
    }
#endif
    int sum;
    int i, j, k;
    char _IPAddr[LARGE_INFO_LENGTH];
    NET_IPV4 ip;
    ip.int32 =  net_get_ifaddr(ETH_NAME);
    sprintf(_IPAddr, "%d.%d.%d.%d", ip.str[0], ip.str[1], ip.str[2], ip.str[3]);

    recording_info_t *info = (recording_info_t *)calloc(1, sizeof(recording_info_t));
    memset(info, 0, sizeof(recording_info_t));
    get_recording_info(info, input->RecordingToken);


    output->RecordingInformation = (struct tt__RecordingInformation *)soap_malloc(soap, sizeof(struct tt__RecordingInformation));
    memset(output->RecordingInformation, 0, sizeof(struct tt__RecordingInformation));
    output->RecordingInformation->Content = (char *)soap_malloc(soap, sizeof(char) * INFO_LEN);
    strcpy(output->RecordingInformation->Content, "Content");
    output->RecordingInformation->EarliestRecording = (time_t *)soap_malloc(soap, sizeof(time_t));
    *output->RecordingInformation->EarliestRecording = info->start_time;
    output->RecordingInformation->LatestRecording = (time_t *)soap_malloc(soap, sizeof(time_t));
    *output->RecordingInformation->LatestRecording = info->stop_time;
    output->RecordingInformation->RecordingStatus = tt__RecordingStatus__Initiated;//tt__RecordingStatus__Initiated = 0, tt__RecordingStatus__Recording = 1, tt__RecordingStatus__Stopped = 2, tt__RecordingStatus__Removing = 3, tt__RecordingStatus__Removed = 4, tt__RecordingStatus__Unknown = 5
    output->RecordingInformation->RecordingToken = (char *)soap_malloc(soap, sizeof(char) * INFO_LEN);
    strcpy(output->RecordingInformation->RecordingToken, input->RecordingToken);
    output->RecordingInformation->Source = (struct tt__RecordingSourceInformation*)soap_malloc(soap, sizeof(struct tt__RecordingSourceInformation));
    memset(output->RecordingInformation->Source, 0, sizeof(struct tt__RecordingSourceInformation));
    output->RecordingInformation->Source->Address = (char *)soap_malloc(soap, sizeof(char) * INFO_LEN);
    strcpy(output->RecordingInformation->Source->Address, _IPAddr);
    output->RecordingInformation->Source->Description = (char *)soap_malloc(soap, sizeof(char) * INFO_LEN);
    strcpy(output->RecordingInformation->Source->Description, "Description");
    output->RecordingInformation->Source->Location = (char *)soap_malloc(soap, sizeof(char) * INFO_LEN);
    strcpy(output->RecordingInformation->Source->Location, "Location");
    output->RecordingInformation->Source->Name = (char *)soap_malloc(soap, sizeof(char) * INFO_LEN);
    strcpy(output->RecordingInformation->Source->Name, "Name");
    output->RecordingInformation->Source->SourceId = (char *)soap_malloc(soap, sizeof(char) * INFO_LEN);
    strcpy(output->RecordingInformation->Source->SourceId, "VIDEO001");
    output->RecordingInformation->Source->__any = NULL;
    output->RecordingInformation->Source->__anyAttribute= NULL;
    output->RecordingInformation->Source->__size = 1;

    output->RecordingInformation->Track = (struct tt__TrackInformation*)soap_malloc(soap, sizeof(struct tt__TrackInformation));
    memset(output->RecordingInformation->Track, 0, sizeof(struct tt__TrackInformation));
    output->RecordingInformation->Track->DataFrom = info->start_time;//The start date and time of the oldest recorded data in the track.
    output->RecordingInformation->Track->DataTo = info->stop_time;//The stop date and time of the newest recorded data in the track.
    output->RecordingInformation->Track->Description = (char *)soap_malloc(soap, sizeof(char) * INFO_LEN);
    strcpy(output->RecordingInformation->Track->Description, "VIDEO001");
    output->RecordingInformation->Track->TrackToken = (char *)soap_malloc(soap, sizeof(char) * INFO_LEN);
    strcpy(output->RecordingInformation->Track->TrackToken, "VIDEO001");
    output->RecordingInformation->Track->TrackType = tt__TrackType__Video;//tt__TrackType__Video = 0, tt__TrackType__Audio = 1, tt__TrackType__Metadata = 2, tt__TrackType__Extended = 3
    output->RecordingInformation->Track->__any = NULL;
    output->RecordingInformation->Track->__anyAttribute = NULL;
    output->RecordingInformation->Track->__size = 1;

    output->RecordingInformation->__any = NULL;
    output->RecordingInformation->__anyAttribute = NULL;
    output->RecordingInformation->__size = 1;
    output->RecordingInformation->__sizeTrack = 1;

    if(info)
    {
        free(info); 
        info = NULL;
    }

    return SOAP_OK;
}

SOAP_FMAC5 int SOAP_FMAC6 __tse__GetSearchState(struct soap *soap, struct _tse__GetSearchState *input, struct _tse__GetSearchStateResponse *output)
{
    return SOAP_OK;
}

SOAP_FMAC5 int SOAP_FMAC6 __tse__FindEvents(struct soap *soap, struct _tse__FindEvents *input, struct _tse__FindEventsResponse *output)
{
#if 0
    if(input->StartPoint > input->EndPoint)
    {
        return SOAP_FAULT;
    }
 #endif
    struct tm stime;
    struct tm etime;
    nvt_time_t starttime;
    nvt_time_t endtime;
    int sum;
    int chs;
    int i, j;
    char *token;

    gmtime_r(&input->StartPoint, &stime);
    gmtime_r(input->EndPoint, &etime);

    starttime.day   = stime.tm_mday;
    starttime.hour  = stime.tm_hour;
    starttime.min   = stime.tm_min;
    starttime.mon   = stime.tm_mon + 1;
    starttime.sec   = stime.tm_sec;
    starttime.year  = stime.tm_year + 1900;
    
    endtime.day     = etime.tm_mday;
    endtime.hour    = etime.tm_hour;
    endtime.min     = etime.tm_min;
    endtime.mon     = etime.tm_mon + 1;
    endtime.sec     = etime.tm_sec;
    endtime.year    = etime.tm_year + 1900;

    get_recording_list(starttime, endtime);
    chs = get_curr_chs();
    sum = g_nvt->t.nrecordings;
    
    for(i = 0; i < chs; i++)
    {
        for(j = 0; j < sum; j++)
        {
            if(strlen(g_nvt->t.record[i][j].token) > 0)
            {
                token = g_nvt->t.record[i][j].token;
            }
            else
                break;
        }
    }
    //need return token list!
    output->SearchToken = (char *)soap_malloc(soap, sizeof(char) * INFO_LEN);
    strcpy(output->SearchToken, token); 
    return SOAP_OK;
}

SOAP_FMAC5 int SOAP_FMAC6 __tse__GetEventSearchResults(struct soap *soap, struct _tse__GetEventSearchResults *input, struct _tse__GetEventSearchResultsResponse *output)
{
#if 0
    if(!input->SearchToken)
    {
        return SOAP_FAULT;
    }
    else if(check_recordingtoken_exist(input->SearchToken) < 0)
    {
        return SOAP_FAULT;
    }

    int list = 1;
    recording_info_t *info = (recording_info_t *)soap_malloc(soap, sizeof(recording_info_t));
    get_recording_info(info, input->SearchToken);
    
    output->ResultList = (struct tt__FindEventResultList*)soap_malloc(soap, list * sizeof(struct tt__FindEventResultList));
    memset(output->ResultList, 0, sizeof(struct tt__FindEventResultList) * list);
    output->ResultList->Result = (struct tt__FindEventResult*)soap_malloc(soap, sizeof(struct tt__FindEventResult));
    memset(output->ResultList->Result, 0, sizeof(struct tt__FindEventResult));
    //output->ResultList->Result->Event = NULL;
#if 1
    output->ResultList->Result->Event = (struct wsnt__NotificationMessageHolderType*)soap_malloc(soap, sizeof(struct wsnt__NotificationMessageHolderType)); //The description of the event.
    memset(output->ResultList->Result->Event, 0, sizeof(struct wsnt__NotificationMessageHolderType));
    output->ResultList->Result->Event->Message = NULL;
    output->ResultList->Result->Event->wsnt__ProducerReference = NULL;
    output->ResultList->Result->Event->wsnt__SubscriptionReference = NULL;
    output->ResultList->Result->Event->wsnt__Topic = NULL;
#endif
    output->ResultList->Result->RecordingToken = (char *)soap_malloc(soap, sizeof(char) * INFO_LEN);
    output->ResultList->Result->TrackToken = (char *)soap_malloc(soap, sizeof(char) * INFO_LEN);
    strcpy(output->ResultList->Result->RecordingToken, input->SearchToken);
    strcpy(output->ResultList->Result->TrackToken, "VIDEO001");
    output->ResultList->Result->StartStateEvent = _true;
    output->ResultList->Result->Time = info->start_time; //The time when the event occured.
    output->ResultList->Result->__any = NULL;
    output->ResultList->Result->__anyAttribute = NULL;
    output->ResultList->Result->__size = 1;
    output->ResultList->__sizeResult = list;
    output->ResultList->SearchState = tt__SearchState__Completed;//Queued = 0, Searching = 1,Completed = 2, Unknown = 3

    if(info)
    {
        free(info);
        info = NULL;
    }
#endif
    return SOAP_OK;
}

SOAP_FMAC5 int SOAP_FMAC6 __tse__FindMetadata(struct soap *soap, struct _tse__FindMetadata *tse__FindMetadata, struct _tse__FindMetadataResponse *tse__FindMetadataResponse)
{
    return SOAP_FAULT;
}

SOAP_FMAC5 int SOAP_FMAC6 __tse__GetMetadataSearchResults(struct soap *soap, struct _tse__GetMetadataSearchResults *tse__GetMetadataSearchResults, struct _tse__GetMetadataSearchResultsResponse *tse__GetMetadataSearchResultsResponse)
{
    return SOAP_FAULT;
}

SOAP_FMAC5 int SOAP_FMAC6 __tse__FindPTZPosition(struct soap *soap, struct _tse__FindPTZPosition *tse__FindPTZPosition, struct _tse__FindPTZPositionResponse *tse__FindPTZPositionResponse)
{
    return SOAP_FAULT;    
}

SOAP_FMAC5 int SOAP_FMAC6 __tse__GetPTZPositionSearchResults(struct soap *soap, struct _tse__GetPTZPositionSearchResults *tse__GetPTZPositionSearchResults, struct _tse__GetPTZPositionSearchResultsResponse *tse__GetPTZPositionSearchResultsResponse)
{
    return SOAP_FAULT; 
}

//Replay control
/*
    Requests a URI that can be used to initiate playback of a recorded stream using RTSP as the control protocol. 
    The URI is valid only as it is specified in the response

*/
SOAP_FMAC5 int SOAP_FMAC6 __trp__GetReplayUri(struct soap *soap, struct _trp__GetReplayUri *input, struct _trp__GetReplayUriResponse *output)
{
#if 1

    if(!input->RecordingToken)
    {
        return SOAP_FAULT;
    }
    else if(check_recordingtoken_exist(input->RecordingToken) < 0)
    {
        return SOAP_FAULT;
    }


    int chs;
    char _uri[256];
    char _IPAddr[LARGE_INFO_LENGTH];
    NET_IPV4 ip;
    ip.int32 =  net_get_ifaddr(ETH_NAME);
    sprintf(_IPAddr, "%d.%d.%d.%d", ip.str[0], ip.str[1], ip.str[2], ip.str[3]);
    
    recording_info_t *info = (recording_info_t *)calloc(1, sizeof(recording_info_t));
    memset(info, 0, sizeof(recording_info_t));
    chs = get_recording_info(info, input->RecordingToken);

    char starttime[64];
    char endtime[64];
    struct tm *time;
    time = gmtime(&info->start_time);
    memset(starttime, 0, sizeof(starttime));
    memset(endtime, 0, sizeof(endtime));
    sprintf(starttime, "%d%02d%02d%02d%02d%02d", (1900+time->tm_year), (1+time->tm_mon), time->tm_mday, time->tm_hour, time->tm_min, time->tm_sec);
    time = gmtime(&info->stop_time);
    sprintf(endtime, "%d%02d%02d%02d%02d%02d", (1900+time->tm_year), (1+time->tm_mon), time->tm_mday, time->tm_hour, time->tm_min, time->tm_sec);

    memset(_uri, 0, sizeof(_uri));
    sprintf(_uri, "rtsp://%s:554/1/%d/0/recordType=-1&startTime=%s&endTime=%s", _IPAddr, chs, starttime, endtime);
    
    output->Uri = (char *)soap_malloc(soap, sizeof(char) * LARGE_INFO_LENGTH);
    strcpy(output->Uri, _uri);

    if(info)
    {
        free(info);
        info = NULL;
    }
#endif

    return SOAP_OK;
}

SOAP_FMAC5 int SOAP_FMAC6 __trp__SetReplayConfiguration(struct soap *soap, struct _trp__SetReplayConfiguration *input, struct _trp__SetReplayConfigurationResponse *output)
{    
    return SOAP_OK;
}

SOAP_FMAC5 int SOAP_FMAC6 __trp__GetReplayConfiguration(struct soap *soap, struct _trp__GetReplayConfiguration *input, struct _trp__GetReplayConfigurationResponse *output)
{
    output->Configuration = (struct tt__ReplayConfiguration*)soap_malloc(soap, sizeof(struct tt__ReplayConfiguration));
    output->Configuration->SessionTimeout = 72000;//The RTSP session timeout.
    output->Configuration->__any = NULL;
    output->Configuration->__anyAttribute = NULL;
    output->Configuration->__size = 1;
    return SOAP_OK;
}

SOAP_FMAC5 int SOAP_FMAC6 __trp__GetServiceCapabilities(struct soap *soap, struct _trp__GetServiceCapabilities *input, struct _trp__GetServiceCapabilitiesResponse *output)
{
    output->Capabilities = (struct trp__Capabilities*)soap_malloc(soap, sizeof(struct trp__Capabilities));
    output->Capabilities->ReversePlayback = _true;
    output->Capabilities->SessionTimeoutRange = (char *)soap_malloc(soap, sizeof(char) * INFO_LENGTH);//The list contains two elements defining the minimum and maximum valid values supported as session timeout in seconds. 
    strcpy(output->Capabilities->SessionTimeoutRange, "72000");
    output->Capabilities->__any = NULL;
    output->Capabilities->__anyAttribute = NULL;
    output->Capabilities->__size = 1;
    return SOAP_OK;
}

//Recording control
SOAP_FMAC5 int SOAP_FMAC6 __trc__CreateRecordingJob(struct soap *soap, struct _trc__CreateRecordingJob *trc__CreateRecordingJob, struct _trc__CreateRecordingJobResponse *trc__CreateRecordingJobResponse)
{
    return SOAP_FAULT;
}

SOAP_FMAC5 int SOAP_FMAC6 __trc__GetRecordingJobs(struct soap *soap, struct _trc__GetRecordingJobs *trc__GetRecordingJobs, struct _trc__GetRecordingJobsResponse *trc__GetRecordingJobsResponse)
{
    return SOAP_FAULT;
}

SOAP_FMAC5 int SOAP_FMAC6 __trc__DeleteRecordingJob(struct soap *soap, struct _trc__DeleteRecordingJob *trc__DeleteRecordingJob, struct _trc__DeleteRecordingJobResponse *trc__DeleteRecordingJobResponse)
{
    return SOAP_FAULT;
}

SOAP_FMAC5 int SOAP_FMAC6 __trc__GetRecordingJobState(struct soap *soap, struct _trc__GetRecordingJobState *trc__GetRecordingJobState, struct _trc__GetRecordingJobStateResponse *trc__GetRecordingJobStateResponse)
{
    return SOAP_FAULT;
}

SOAP_FMAC5 int SOAP_FMAC6 __trv__ConfigureReceiver(struct soap *soap, struct _trv__ConfigureReceiver *trv__ConfigureReceiver, struct _trv__ConfigureReceiverResponse *trv__ConfigureReceiverResponse)
{
    return SOAP_FAULT;
}

SOAP_FMAC5 int SOAP_FMAC6 __trv__GetReceivers(struct soap *soap, struct _trv__GetReceivers *trv__GetReceivers, struct _trv__GetReceiversResponse *trv__GetReceiversResponse)
{
    return SOAP_FAULT;
}

SOAP_FMAC5 int SOAP_FMAC6 __trc__SetRecordingJobMode(struct soap *soap, struct _trc__SetRecordingJobMode *trc__SetRecordingJobMode, struct _trc__SetRecordingJobModeResponse *trc__SetRecordingJobModeResponse)
{
    return SOAP_FAULT;
}

SOAP_FMAC5 int SOAP_FMAC6 __trc__CreateTrack(struct soap *soap, struct _trc__CreateTrack *trc__CreateTrack, struct _trc__CreateTrackResponse *trc__CreateTrackResponse)
{
    return SOAP_FAULT;
}

SOAP_FMAC5 int SOAP_FMAC6 __trc__DeleteTrack(struct soap *soap, struct _trc__DeleteTrack *trc__DeleteTrack, struct _trc__DeleteTrackResponse *trc__DeleteTrackResponse)
{
    return SOAP_FAULT;
}

SOAP_FMAC5 int SOAP_FMAC6 __trc__GetRecordingConfiguration(struct soap *soap, struct _trc__GetRecordingConfiguration *trc__GetRecordingConfiguration, struct _trc__GetRecordingConfigurationResponse *trc__GetRecordingConfigurationResponse)
{
    return SOAP_FAULT;
}

SOAP_FMAC5 int SOAP_FMAC6 __trc__GetRecordingJobConfiguration(struct soap *soap, struct _trc__GetRecordingJobConfiguration *trc__GetRecordingJobConfiguration, struct _trc__GetRecordingJobConfigurationResponse *trc__GetRecordingJobConfigurationResponse)
{
    return SOAP_FAULT;
}

SOAP_FMAC5 int SOAP_FMAC6 __trc__CreateRecording(struct soap *soap, struct _trc__CreateRecording *trc__CreateRecording, struct _trc__CreateRecordingResponse *CRecordingResp)
{
    return SOAP_FAULT;
}

SOAP_FMAC5 int SOAP_FMAC6 __trc__GetTrackConfiguration(struct soap *soap, struct _trc__GetTrackConfiguration *trc__GetTrackConfiguration, struct _trc__GetTrackConfigurationResponse *trc__GetTrackConfigurationResponse)
{
    return SOAP_FAULT;
}

SOAP_FMAC5 int SOAP_FMAC6 __trc__GetServiceCapabilities(struct soap *soap, struct _trc__GetServiceCapabilities *trc__GetServiceCapabilities, struct _trc__GetServiceCapabilitiesResponse *GetRecordingCap)
{
    return SOAP_FAULT;    
}

/////////////////// add ptz part ///////////////////

SOAP_FMAC5 int SOAP_FMAC6 __tptz__GetServiceCapabilities(struct soap* soap, struct _tptz__GetServiceCapabilities *tptz__GetServiceCapabilities, struct _tptz__GetServiceCapabilitiesResponse *tptz__GetServiceCapabilitiesResponse)
{
	tptz__GetServiceCapabilitiesResponse->Capabilities = (struct tptz__Capabilities *)soap_malloc(soap, sizeof(struct tptz__Capabilities));
	memset(tptz__GetServiceCapabilitiesResponse->Capabilities, 0, sizeof(struct tptz__Capabilities));

	tptz__GetServiceCapabilitiesResponse->Capabilities->EFlip = (enum xsd__boolean_ *)soap_malloc(soap, sizeof(enum xsd__boolean_));
	tptz__GetServiceCapabilitiesResponse->Capabilities->Reverse = (enum xsd__boolean_ *)soap_malloc(soap, sizeof(enum xsd__boolean_));

	*tptz__GetServiceCapabilitiesResponse->Capabilities->EFlip = _false;
	*tptz__GetServiceCapabilitiesResponse->Capabilities->Reverse = _false;

	return SOAP_OK;  
}

SOAP_FMAC5 int SOAP_FMAC6 __tptz__GetConfigurations(struct soap* soap, struct _tptz__GetConfigurations *tptz__GetConfigurations, struct _tptz__GetConfigurationsResponse *tptz__GetConfigurationsResponse)
{
	tptz__GetConfigurationsResponse->__sizePTZConfiguration = 1;
	tptz__GetConfigurationsResponse->PTZConfiguration = (struct tt__PTZConfiguration *)soap_malloc(soap, sizeof(struct tt__PTZConfiguration)*tptz__GetConfigurationsResponse->__sizePTZConfiguration);
	memset(tptz__GetConfigurationsResponse->PTZConfiguration, 0, sizeof(struct tt__PTZConfiguration)*tptz__GetConfigurationsResponse->__sizePTZConfiguration);
  
	int index = 0;
	for(index = 0; index < tptz__GetConfigurationsResponse->__sizePTZConfiguration; index++)
	{
		ptz_cfg_get(soap, index, &tptz__GetConfigurationsResponse->PTZConfiguration[index]);
	}

	return SOAP_OK;
}

SOAP_FMAC5 int SOAP_FMAC6 __tptz__GetPresets(struct soap* soap, struct _tptz__GetPresets *tptz__GetPresets, struct _tptz__GetPresetsResponse *tptz__GetPresetsResponse)
{
  int index = -1;//tptz__GetPresets->ProfileToken
  
  int PresetNum = 0;
  
  //TODO;
  PresetNum = 10;

  tptz__GetPresetsResponse->__sizePreset = PresetNum;
	tptz__GetPresetsResponse->Preset = (struct tt__PTZPreset *)soap_malloc(soap, sizeof(struct tt__PTZPreset)*tptz__GetPresetsResponse->__sizePreset);
	memset(tptz__GetPresetsResponse->Preset, 0, sizeof(struct tt__PTZPreset)*tptz__GetPresetsResponse->__sizePreset);

	int i = 0;
	for(i = 0; i < tptz__GetPresetsResponse->__sizePreset; i++)
	{
		tptz__GetPresetsResponse->Preset[i].Name = (char *)soap_malloc(soap, sizeof(char)*SMALL_INFO_LENGTH);
		tptz__GetPresetsResponse->Preset[i].token= (char *)soap_malloc(soap, sizeof(char)*SMALL_INFO_LENGTH);

		sprintf(tptz__GetPresetsResponse->Preset[i].token, "Preset%d", i);
		sprintf(tptz__GetPresetsResponse->Preset[i].Name, "Preset%d", i);
	}

	return SOAP_OK;   
}

SOAP_FMAC5 int SOAP_FMAC6 __tptz__SetPreset(struct soap* soap, struct _tptz__SetPreset *tptz__SetPreset, struct _tptz__SetPresetResponse *tptz__SetPresetResponse)
{
  int index = -1;//tptz__SetPreset->ProfileToken
  int presetIndex = -1;//tptz__SetPreset->PresetToken, tptz__SetPreset->PresetName
  
	tptz__SetPresetResponse->PresetToken = (char *)soap_malloc(soap, sizeof(char)*SMALL_INFO_LENGTH);
	memset(tptz__SetPresetResponse->PresetToken, 0, sizeof(char)*SMALL_INFO_LENGTH);
	strcpy(tptz__SetPresetResponse->PresetToken, tptz__SetPreset->PresetToken);

  printf("%s => index:%d, presetIndex:%d\n", __func__, index, presetIndex);

  return SOAP_OK;   
}

SOAP_FMAC5 int SOAP_FMAC6 __tptz__RemovePreset(struct soap* soap, struct _tptz__RemovePreset *tptz__RemovePreset, struct _tptz__RemovePresetResponse *tptz__RemovePresetResponse)
{
    int index = -1;//tptz__RemovePreset->ProfileToken
    int presetIndex = -1;//tptz__RemovePreset->PresetToken
    
    printf("%s => index:%d, presetIndex:%d\n", __func__, index, presetIndex);
    
    return SOAP_OK;  
}

SOAP_FMAC5 int SOAP_FMAC6 __tptz__GotoPreset(struct soap* soap, struct _tptz__GotoPreset *tptz__GotoPreset, struct _tptz__GotoPresetResponse *tptz__GotoPresetResponse)
{
    int index = -1;//tptz__GotoPreset->ProfileToken
    int presetIndex = -1;//tptz__GotoPreset->PresetToken
    
    printf("%s => index:%d, presetIndex:%d\n", __func__, index, presetIndex);
    
    return SOAP_OK;    
}

SOAP_FMAC5 int SOAP_FMAC6 __tptz__GetStatus(struct soap* soap, struct _tptz__GetStatus *tptz__GetStatus, struct _tptz__GetStatusResponse *tptz__GetStatusResponse)
{
    return SOAP_FAULT;    
}

SOAP_FMAC5 int SOAP_FMAC6 __tptz__GetConfiguration(struct soap* soap, struct _tptz__GetConfiguration *tptz__GetConfiguration, struct _tptz__GetConfigurationResponse *tptz__GetConfigurationResponse)
{
  int index = 0; //tptz__GetConfiguration->PTZConfigurationToken
	tptz__GetConfigurationResponse->PTZConfiguration = (struct tt__PTZConfiguration *)soap_malloc(soap, sizeof(struct tt__PTZConfiguration));
	memset(tptz__GetConfigurationResponse->PTZConfiguration, 0, sizeof(struct tt__PTZConfiguration));

	ptz_cfg_get(soap, index, tptz__GetConfigurationResponse->PTZConfiguration);

	return SOAP_OK;   
}


int ptz_node_get(struct soap *soap, int chs, struct tt__PTZNode *pPtzNode)
{

	pPtzNode->Name = (char *)soap_malloc(soap, sizeof(char)*SMALL_INFO_LENGTH);
	strcpy(pPtzNode->Name, "PtzNode");
	pPtzNode->token = (char *)soap_malloc(soap, sizeof(char)*SMALL_INFO_LENGTH);
	strcpy(pPtzNode->token, "PtzNodeToken");

	pPtzNode->SupportedPTZSpaces = (struct tt__PTZSpaces *)soap_malloc(soap, sizeof(struct tt__PTZSpaces));
	memset(pPtzNode->SupportedPTZSpaces, 0, sizeof(struct tt__PTZSpaces));

	pPtzNode->SupportedPTZSpaces->__sizeAbsolutePanTiltPositionSpace = 1;
	pPtzNode->SupportedPTZSpaces->AbsolutePanTiltPositionSpace = (struct tt__Space2DDescription *)soap_malloc(soap, sizeof(struct tt__Space2DDescription));
	memset(pPtzNode->SupportedPTZSpaces->AbsolutePanTiltPositionSpace, 0, sizeof(struct tt__Space2DDescription));
	pPtzNode->SupportedPTZSpaces->AbsolutePanTiltPositionSpace->URI = (char *)soap_malloc(soap, sizeof(char)*SMALL_INFO_LENGTH);
	memset(pPtzNode->SupportedPTZSpaces->AbsolutePanTiltPositionSpace->URI, 0, sizeof(char)*SMALL_INFO_LENGTH);
	strcpy(pPtzNode->SupportedPTZSpaces->AbsolutePanTiltPositionSpace->URI, "http://www.onvif.org/ver10/tptz/PanTiltSpaces/PositionGenericSpace");
	pPtzNode->SupportedPTZSpaces->AbsolutePanTiltPositionSpace->XRange = (struct tt__FloatRange *)soap_malloc(soap, sizeof(struct tt__FloatRange));
	memset(pPtzNode->SupportedPTZSpaces->AbsolutePanTiltPositionSpace->XRange, 0, sizeof(struct tt__FloatRange));
	pPtzNode->SupportedPTZSpaces->AbsolutePanTiltPositionSpace->XRange->Max = 1.0;
	pPtzNode->SupportedPTZSpaces->AbsolutePanTiltPositionSpace->XRange->Min = -1.0;
	pPtzNode->SupportedPTZSpaces->AbsolutePanTiltPositionSpace->YRange = (struct tt__FloatRange *)soap_malloc(soap, sizeof(struct tt__FloatRange));
	memset(pPtzNode->SupportedPTZSpaces->AbsolutePanTiltPositionSpace->YRange, 0, sizeof(struct tt__FloatRange));
	pPtzNode->SupportedPTZSpaces->AbsolutePanTiltPositionSpace->YRange->Max = 1.0;
	pPtzNode->SupportedPTZSpaces->AbsolutePanTiltPositionSpace->YRange->Min = -1.0;

	pPtzNode->SupportedPTZSpaces->__sizeAbsoluteZoomPositionSpace = 1;
	pPtzNode->SupportedPTZSpaces->AbsoluteZoomPositionSpace = (struct tt__Space1DDescription *)soap_malloc(soap, sizeof(struct tt__Space1DDescription));
	memset(pPtzNode->SupportedPTZSpaces->AbsoluteZoomPositionSpace, 0, sizeof(struct tt__Space1DDescription));
	pPtzNode->SupportedPTZSpaces->AbsoluteZoomPositionSpace->URI = (char *)soap_malloc(soap, sizeof(char)*SMALL_INFO_LENGTH);
	memset(pPtzNode->SupportedPTZSpaces->AbsoluteZoomPositionSpace->URI, 0, sizeof(char)*SMALL_INFO_LENGTH);
	strcpy(pPtzNode->SupportedPTZSpaces->AbsoluteZoomPositionSpace->URI, "http://www.onvif.org/ver10/tptz/ZoomSpaces/PositionGenericSpace");
	pPtzNode->SupportedPTZSpaces->AbsoluteZoomPositionSpace->XRange = (struct tt__FloatRange *)soap_malloc(soap, sizeof(struct tt__FloatRange));
	memset(pPtzNode->SupportedPTZSpaces->AbsoluteZoomPositionSpace->XRange, 0, sizeof(struct tt__FloatRange));
	pPtzNode->SupportedPTZSpaces->AbsoluteZoomPositionSpace->XRange->Max = 1.0;
	pPtzNode->SupportedPTZSpaces->AbsoluteZoomPositionSpace->XRange->Min = -1.0;

	pPtzNode->SupportedPTZSpaces->__sizeRelativePanTiltTranslationSpace = 1;
	pPtzNode->SupportedPTZSpaces->RelativePanTiltTranslationSpace = (struct tt__Space2DDescription *)soap_malloc(soap, sizeof(struct tt__Space2DDescription));
	memset(pPtzNode->SupportedPTZSpaces->RelativePanTiltTranslationSpace, 0, sizeof(struct tt__Space2DDescription));
	pPtzNode->SupportedPTZSpaces->RelativePanTiltTranslationSpace->URI = (char *)soap_malloc(soap, sizeof(char)*SMALL_INFO_LENGTH);
	memset(pPtzNode->SupportedPTZSpaces->RelativePanTiltTranslationSpace->URI, 0, sizeof(char)*SMALL_INFO_LENGTH);
	strcpy(pPtzNode->SupportedPTZSpaces->RelativePanTiltTranslationSpace->URI, "http://www.onvif.org/ver10/tptz/PanTiltSpaces/TranslationGenericSpace");
	pPtzNode->SupportedPTZSpaces->RelativePanTiltTranslationSpace->XRange = (struct tt__FloatRange *)soap_malloc(soap, sizeof(struct tt__FloatRange));
	memset(pPtzNode->SupportedPTZSpaces->RelativePanTiltTranslationSpace->XRange, 0, sizeof(struct tt__FloatRange));
	pPtzNode->SupportedPTZSpaces->RelativePanTiltTranslationSpace->XRange->Max = 1.0;
	pPtzNode->SupportedPTZSpaces->RelativePanTiltTranslationSpace->XRange->Min = -1.0;
	pPtzNode->SupportedPTZSpaces->RelativePanTiltTranslationSpace->YRange = (struct tt__FloatRange *)soap_malloc(soap, sizeof(struct tt__FloatRange));
	memset(pPtzNode->SupportedPTZSpaces->RelativePanTiltTranslationSpace->YRange, 0, sizeof(struct tt__FloatRange));
	pPtzNode->SupportedPTZSpaces->RelativePanTiltTranslationSpace->YRange->Max = 1.0;
	pPtzNode->SupportedPTZSpaces->RelativePanTiltTranslationSpace->YRange->Min = -1.0;

	pPtzNode->SupportedPTZSpaces->__sizeRelativeZoomTranslationSpace = 1;
	pPtzNode->SupportedPTZSpaces->RelativeZoomTranslationSpace = (struct tt__Space1DDescription *)soap_malloc(soap, sizeof(struct tt__Space1DDescription));
	memset(pPtzNode->SupportedPTZSpaces->RelativeZoomTranslationSpace, 0, sizeof(struct tt__Space1DDescription));
	pPtzNode->SupportedPTZSpaces->RelativeZoomTranslationSpace->URI = (char *)soap_malloc(soap, sizeof(char)*SMALL_INFO_LENGTH);
	memset(pPtzNode->SupportedPTZSpaces->RelativeZoomTranslationSpace->URI, 0, sizeof(char)*SMALL_INFO_LENGTH);
	strcpy(pPtzNode->SupportedPTZSpaces->RelativeZoomTranslationSpace->URI, "http://www.onvif.org/ver10/tptz/ZoomSpaces/TranslationGenericSpace");
	pPtzNode->SupportedPTZSpaces->RelativeZoomTranslationSpace->XRange = (struct tt__FloatRange *)soap_malloc(soap, sizeof(struct tt__FloatRange));
	memset(pPtzNode->SupportedPTZSpaces->RelativeZoomTranslationSpace->XRange, 0, sizeof(struct tt__FloatRange));
	pPtzNode->SupportedPTZSpaces->RelativeZoomTranslationSpace->XRange->Max = 1.0;
	pPtzNode->SupportedPTZSpaces->RelativeZoomTranslationSpace->XRange->Min = -1.0;

	pPtzNode->SupportedPTZSpaces->__sizeContinuousPanTiltVelocitySpace = 1;
	pPtzNode->SupportedPTZSpaces->ContinuousPanTiltVelocitySpace = (struct tt__Space2DDescription *)soap_malloc(soap, sizeof(struct tt__Space2DDescription));
	memset(pPtzNode->SupportedPTZSpaces->ContinuousPanTiltVelocitySpace, 0, sizeof(struct tt__Space2DDescription));
	pPtzNode->SupportedPTZSpaces->ContinuousPanTiltVelocitySpace->URI = (char *)soap_malloc(soap, sizeof(char)*SMALL_INFO_LENGTH);
	memset(pPtzNode->SupportedPTZSpaces->ContinuousPanTiltVelocitySpace->URI, 0, sizeof(char)*SMALL_INFO_LENGTH);
	strcpy(pPtzNode->SupportedPTZSpaces->ContinuousPanTiltVelocitySpace->URI, "http://www.onvif.org/ver10/tptz/PanTiltSpaces/VelocityGenericSpace");
	pPtzNode->SupportedPTZSpaces->ContinuousPanTiltVelocitySpace->XRange = (struct tt__FloatRange *)soap_malloc(soap, sizeof(struct tt__FloatRange));
	memset(pPtzNode->SupportedPTZSpaces->ContinuousPanTiltVelocitySpace->XRange, 0, sizeof(struct tt__FloatRange));
	pPtzNode->SupportedPTZSpaces->ContinuousPanTiltVelocitySpace->XRange->Max = 1.0;
	pPtzNode->SupportedPTZSpaces->ContinuousPanTiltVelocitySpace->XRange->Min = -1.0;
	pPtzNode->SupportedPTZSpaces->ContinuousPanTiltVelocitySpace->YRange = (struct tt__FloatRange *)soap_malloc(soap, sizeof(struct tt__FloatRange));
	memset(pPtzNode->SupportedPTZSpaces->ContinuousPanTiltVelocitySpace->YRange, 0, sizeof(struct tt__FloatRange));
	pPtzNode->SupportedPTZSpaces->ContinuousPanTiltVelocitySpace->YRange->Max = 1.0;
	pPtzNode->SupportedPTZSpaces->ContinuousPanTiltVelocitySpace->YRange->Min = -1.0;

	pPtzNode->SupportedPTZSpaces->__sizeContinuousZoomVelocitySpace = 1;
	pPtzNode->SupportedPTZSpaces->ContinuousZoomVelocitySpace = (struct tt__Space1DDescription *)soap_malloc(soap, sizeof(struct tt__Space1DDescription));
	memset(pPtzNode->SupportedPTZSpaces->ContinuousZoomVelocitySpace, 0, sizeof(struct tt__Space1DDescription));
	pPtzNode->SupportedPTZSpaces->ContinuousZoomVelocitySpace->URI = (char *)soap_malloc(soap, sizeof(char)*SMALL_INFO_LENGTH);
	memset(pPtzNode->SupportedPTZSpaces->ContinuousZoomVelocitySpace->URI, 0, sizeof(char)*SMALL_INFO_LENGTH);
	strcpy(pPtzNode->SupportedPTZSpaces->ContinuousZoomVelocitySpace->URI, "http://www.onvif.org/ver10/tptz/ZoomSpaces/VelocityGenericSpace");
	pPtzNode->SupportedPTZSpaces->ContinuousZoomVelocitySpace->XRange = (struct tt__FloatRange *)soap_malloc(soap, sizeof(struct tt__FloatRange));
	memset(pPtzNode->SupportedPTZSpaces->ContinuousZoomVelocitySpace->XRange, 0, sizeof(struct tt__FloatRange));
	pPtzNode->SupportedPTZSpaces->ContinuousZoomVelocitySpace->XRange->Max = 1.0;
	pPtzNode->SupportedPTZSpaces->ContinuousZoomVelocitySpace->XRange->Min = -1.0;

	pPtzNode->SupportedPTZSpaces->__sizePanTiltSpeedSpace = 1;
	pPtzNode->SupportedPTZSpaces->PanTiltSpeedSpace = (struct tt__Space1DDescription *)soap_malloc(soap, sizeof(struct tt__Space1DDescription));
	memset(pPtzNode->SupportedPTZSpaces->PanTiltSpeedSpace, 0, sizeof(struct tt__Space1DDescription));
	pPtzNode->SupportedPTZSpaces->PanTiltSpeedSpace->URI = (char *)soap_malloc(soap, sizeof(char)*SMALL_INFO_LENGTH);
	memset(pPtzNode->SupportedPTZSpaces->PanTiltSpeedSpace->URI, 0, sizeof(char)*SMALL_INFO_LENGTH);
	strcpy(pPtzNode->SupportedPTZSpaces->PanTiltSpeedSpace->URI, "http://www.onvif.org/ver10/tptz/PanTiltSpaces/GenericSpeedSpace");
	pPtzNode->SupportedPTZSpaces->PanTiltSpeedSpace->XRange = (struct tt__FloatRange *)soap_malloc(soap, sizeof(struct tt__FloatRange));
	memset(pPtzNode->SupportedPTZSpaces->PanTiltSpeedSpace->XRange, 0, sizeof(struct tt__FloatRange));
	pPtzNode->SupportedPTZSpaces->PanTiltSpeedSpace->XRange->Max = 1.0;
	pPtzNode->SupportedPTZSpaces->PanTiltSpeedSpace->XRange->Min = -1.0;

	pPtzNode->SupportedPTZSpaces->__sizeZoomSpeedSpace = 1;
	pPtzNode->SupportedPTZSpaces->ZoomSpeedSpace = (struct tt__Space1DDescription *)soap_malloc(soap, sizeof(struct tt__Space1DDescription));
	memset(pPtzNode->SupportedPTZSpaces->ZoomSpeedSpace, 0, sizeof(struct tt__Space1DDescription));
	pPtzNode->SupportedPTZSpaces->ZoomSpeedSpace->URI = (char *)soap_malloc(soap, sizeof(char)*SMALL_INFO_LENGTH);
	memset(pPtzNode->SupportedPTZSpaces->ZoomSpeedSpace->URI, 0, sizeof(char)*SMALL_INFO_LENGTH);
	strcpy(pPtzNode->SupportedPTZSpaces->ZoomSpeedSpace->URI, "http://www.onvif.org/ver10/tptz/ZoomSpaces/ZoomGenericSpeedSpace");
	pPtzNode->SupportedPTZSpaces->ZoomSpeedSpace->XRange = (struct tt__FloatRange *)soap_malloc(soap, sizeof(struct tt__FloatRange));
	memset(pPtzNode->SupportedPTZSpaces->ZoomSpeedSpace->XRange, 0, sizeof(struct tt__FloatRange));
	pPtzNode->SupportedPTZSpaces->ZoomSpeedSpace->XRange->Max = 1.0;
	pPtzNode->SupportedPTZSpaces->ZoomSpeedSpace->XRange->Min = -1.0;

	pPtzNode->MaximumNumberOfPresets = MAX_PRESET_NUM;
	pPtzNode->HomeSupported = _true;

	pPtzNode->FixedHomePosition = (enum xsd__boolean_ *)soap_malloc(soap, sizeof(enum xsd__boolean_));
	*pPtzNode->FixedHomePosition = _false;

	pPtzNode->__sizeAuxiliaryCommands = 0;

	return SOAP_OK;
}


SOAP_FMAC5 int SOAP_FMAC6 __tptz__GetNodes(struct soap* soap, struct _tptz__GetNodes *tptz__GetNodes, struct _tptz__GetNodesResponse *tptz__GetNodesResponse)
{
	tptz__GetNodesResponse->__sizePTZNode = 1;
	tptz__GetNodesResponse->PTZNode = (struct tt__PTZNode *)soap_malloc(soap, sizeof(struct tt__PTZNode)*tptz__GetNodesResponse->__sizePTZNode);
	memset(tptz__GetNodesResponse->PTZNode, 0, sizeof(struct tt__PTZNode)*tptz__GetNodesResponse->__sizePTZNode);

	int index = 0;
	for(index = 0; index < tptz__GetNodesResponse->__sizePTZNode; index++)
	{
		ptz_node_get(soap, index, &tptz__GetNodesResponse->PTZNode[index]);
	}
	
	return SOAP_OK;   
}

SOAP_FMAC5 int SOAP_FMAC6 __tptz__GetNode(struct soap* soap, struct _tptz__GetNode *tptz__GetNode, struct _tptz__GetNodeResponse *tptz__GetNodeResponse)
{
	int index = 0; // tptz__GetNode->NodeToken
	
	tptz__GetNodeResponse->PTZNode = (struct tt__PTZNode *)soap_malloc(soap, sizeof(struct tt__PTZNode));
	memset(tptz__GetNodeResponse->PTZNode, 0, sizeof(struct tt__PTZNode));

	ptz_node_get(soap, index, tptz__GetNodeResponse->PTZNode);

	return SOAP_OK;   
}

SOAP_FMAC5 int SOAP_FMAC6 __tptz__SetConfiguration(struct soap* soap, struct _tptz__SetConfiguration *tptz__SetConfiguration, struct _tptz__SetConfigurationResponse *tptz__SetConfigurationResponse)
{
    return SOAP_FAULT;    
}

SOAP_FMAC5 int SOAP_FMAC6 __tptz__GetConfigurationOptions(struct soap* soap, struct _tptz__GetConfigurationOptions *tptz__GetConfigurationOptions, struct _tptz__GetConfigurationOptionsResponse *tptz__GetConfigurationOptionsResponse)
{
  // tptz__GetConfigurationOptions->ConfigurationToken
  
	tptz__GetConfigurationOptionsResponse->PTZConfigurationOptions = (struct tt__PTZConfigurationOptions *)soap_malloc(soap, sizeof(struct tt__PTZConfigurationOptions));
	memset(tptz__GetConfigurationOptionsResponse->PTZConfigurationOptions, 0, sizeof(struct tt__PTZConfigurationOptions));

  struct tt__PTZConfigurationOptions *pPtzOptions = tptz__GetConfigurationOptionsResponse->PTZConfigurationOptions;
	pPtzOptions->Spaces = (struct tt__PTZSpaces *)soap_malloc(soap, sizeof(struct tt__PTZSpaces));
	memset(pPtzOptions->Spaces, 0, sizeof(struct tt__PTZSpaces));

	pPtzOptions->Spaces->__sizeAbsolutePanTiltPositionSpace = 1;
	pPtzOptions->Spaces->AbsolutePanTiltPositionSpace = (struct tt__Space2DDescription *)soap_malloc(soap, sizeof(struct tt__Space2DDescription));
	memset(pPtzOptions->Spaces->AbsolutePanTiltPositionSpace, 0, sizeof(struct tt__Space2DDescription));
	pPtzOptions->Spaces->AbsolutePanTiltPositionSpace->URI = (char *)soap_malloc(soap, sizeof(char)*SMALL_INFO_LENGTH);
	memset(pPtzOptions->Spaces->AbsolutePanTiltPositionSpace->URI, 0, sizeof(char)*SMALL_INFO_LENGTH);
	strcpy(pPtzOptions->Spaces->AbsolutePanTiltPositionSpace->URI, "http://www.onvif.org/ver10/tptz/PanTiltSpaces/PositionGenericSpace");
	pPtzOptions->Spaces->AbsolutePanTiltPositionSpace->XRange = (struct tt__FloatRange *)soap_malloc(soap, sizeof(struct tt__FloatRange));
	memset(pPtzOptions->Spaces->AbsolutePanTiltPositionSpace->XRange, 0, sizeof(struct tt__FloatRange));
	pPtzOptions->Spaces->AbsolutePanTiltPositionSpace->XRange->Max = 1.0;
	pPtzOptions->Spaces->AbsolutePanTiltPositionSpace->XRange->Min = -1.0;
	pPtzOptions->Spaces->AbsolutePanTiltPositionSpace->YRange = (struct tt__FloatRange *)soap_malloc(soap, sizeof(struct tt__FloatRange));
	memset(pPtzOptions->Spaces->AbsolutePanTiltPositionSpace->YRange, 0, sizeof(struct tt__FloatRange));
	pPtzOptions->Spaces->AbsolutePanTiltPositionSpace->YRange->Max = 1.0;
	pPtzOptions->Spaces->AbsolutePanTiltPositionSpace->YRange->Min = -1.0;

	pPtzOptions->Spaces->__sizeAbsoluteZoomPositionSpace = 1;
	pPtzOptions->Spaces->AbsoluteZoomPositionSpace = (struct tt__Space1DDescription *)soap_malloc(soap, sizeof(struct tt__Space1DDescription));
	memset(pPtzOptions->Spaces->AbsoluteZoomPositionSpace, 0, sizeof(struct tt__Space1DDescription));
	pPtzOptions->Spaces->AbsoluteZoomPositionSpace->URI = (char *)soap_malloc(soap, sizeof(char)*SMALL_INFO_LENGTH);
	memset(pPtzOptions->Spaces->AbsoluteZoomPositionSpace->URI, 0, sizeof(char)*SMALL_INFO_LENGTH);
	strcpy(pPtzOptions->Spaces->AbsoluteZoomPositionSpace->URI, "http://www.onvif.org/ver10/tptz/ZoomSpaces/PositionGenericSpace");
	pPtzOptions->Spaces->AbsoluteZoomPositionSpace->XRange = (struct tt__FloatRange *)soap_malloc(soap, sizeof(struct tt__FloatRange));
	memset(pPtzOptions->Spaces->AbsoluteZoomPositionSpace->XRange, 0, sizeof(struct tt__FloatRange));
	pPtzOptions->Spaces->AbsoluteZoomPositionSpace->XRange->Max = 1.0;
	pPtzOptions->Spaces->AbsoluteZoomPositionSpace->XRange->Min = -1.0;

	pPtzOptions->Spaces->__sizeRelativePanTiltTranslationSpace = 1;
	pPtzOptions->Spaces->RelativePanTiltTranslationSpace = (struct tt__Space2DDescription *)soap_malloc(soap, sizeof(struct tt__Space2DDescription));
	memset(pPtzOptions->Spaces->RelativePanTiltTranslationSpace, 0, sizeof(struct tt__Space2DDescription));
	pPtzOptions->Spaces->RelativePanTiltTranslationSpace->URI = (char *)soap_malloc(soap, sizeof(char)*SMALL_INFO_LENGTH);
	memset(pPtzOptions->Spaces->RelativePanTiltTranslationSpace->URI, 0, sizeof(char)*SMALL_INFO_LENGTH);
	strcpy(pPtzOptions->Spaces->RelativePanTiltTranslationSpace->URI, "http://www.onvif.org/ver10/tptz/PanTiltSpaces/TranslationGenericSpace");
	pPtzOptions->Spaces->RelativePanTiltTranslationSpace->XRange = (struct tt__FloatRange *)soap_malloc(soap, sizeof(struct tt__FloatRange));
	memset(pPtzOptions->Spaces->RelativePanTiltTranslationSpace->XRange, 0, sizeof(struct tt__FloatRange));
	pPtzOptions->Spaces->RelativePanTiltTranslationSpace->XRange->Max = 1.0;
	pPtzOptions->Spaces->RelativePanTiltTranslationSpace->XRange->Min = -1.0;
	pPtzOptions->Spaces->RelativePanTiltTranslationSpace->YRange = (struct tt__FloatRange *)soap_malloc(soap, sizeof(struct tt__FloatRange));
	memset(pPtzOptions->Spaces->RelativePanTiltTranslationSpace->YRange, 0, sizeof(struct tt__FloatRange));
	pPtzOptions->Spaces->RelativePanTiltTranslationSpace->YRange->Max = 1.0;
	pPtzOptions->Spaces->RelativePanTiltTranslationSpace->YRange->Min = -1.0;

	pPtzOptions->Spaces->__sizeRelativeZoomTranslationSpace = 1;
	pPtzOptions->Spaces->RelativeZoomTranslationSpace = (struct tt__Space1DDescription *)soap_malloc(soap, sizeof(struct tt__Space1DDescription));
	memset(pPtzOptions->Spaces->RelativeZoomTranslationSpace, 0, sizeof(struct tt__Space1DDescription));
	pPtzOptions->Spaces->RelativeZoomTranslationSpace->URI = (char *)soap_malloc(soap, sizeof(char)*SMALL_INFO_LENGTH);
	memset(pPtzOptions->Spaces->RelativeZoomTranslationSpace->URI, 0, sizeof(char)*SMALL_INFO_LENGTH);
	strcpy(pPtzOptions->Spaces->RelativeZoomTranslationSpace->URI, "http://www.onvif.org/ver10/tptz/ZoomSpaces/TranslationGenericSpace");
	pPtzOptions->Spaces->RelativeZoomTranslationSpace->XRange = (struct tt__FloatRange *)soap_malloc(soap, sizeof(struct tt__FloatRange));
	memset(pPtzOptions->Spaces->RelativeZoomTranslationSpace->XRange, 0, sizeof(struct tt__FloatRange));
	pPtzOptions->Spaces->RelativeZoomTranslationSpace->XRange->Max = 1.0;
	pPtzOptions->Spaces->RelativeZoomTranslationSpace->XRange->Min = -1.0;

	pPtzOptions->Spaces->__sizeContinuousPanTiltVelocitySpace = 1;
	pPtzOptions->Spaces->ContinuousPanTiltVelocitySpace = (struct tt__Space2DDescription *)soap_malloc(soap, sizeof(struct tt__Space2DDescription));
	memset(pPtzOptions->Spaces->ContinuousPanTiltVelocitySpace, 0, sizeof(struct tt__Space2DDescription));
	pPtzOptions->Spaces->ContinuousPanTiltVelocitySpace->URI = (char *)soap_malloc(soap, sizeof(char)*SMALL_INFO_LENGTH);
	memset(pPtzOptions->Spaces->ContinuousPanTiltVelocitySpace->URI, 0, sizeof(char)*SMALL_INFO_LENGTH);
	strcpy(pPtzOptions->Spaces->ContinuousPanTiltVelocitySpace->URI, "http://www.onvif.org/ver10/tptz/PanTiltSpaces/VelocityGenericSpace");
	pPtzOptions->Spaces->ContinuousPanTiltVelocitySpace->XRange = (struct tt__FloatRange *)soap_malloc(soap, sizeof(struct tt__FloatRange));
	memset(pPtzOptions->Spaces->ContinuousPanTiltVelocitySpace->XRange, 0, sizeof(struct tt__FloatRange));
	pPtzOptions->Spaces->ContinuousPanTiltVelocitySpace->XRange->Max = 1.0;
	pPtzOptions->Spaces->ContinuousPanTiltVelocitySpace->XRange->Min = -1.0;
	pPtzOptions->Spaces->ContinuousPanTiltVelocitySpace->YRange = (struct tt__FloatRange *)soap_malloc(soap, sizeof(struct tt__FloatRange));
	memset(pPtzOptions->Spaces->ContinuousPanTiltVelocitySpace->YRange, 0, sizeof(struct tt__FloatRange));
	pPtzOptions->Spaces->ContinuousPanTiltVelocitySpace->YRange->Max = 1.0;
	pPtzOptions->Spaces->ContinuousPanTiltVelocitySpace->YRange->Min = -1.0;

	pPtzOptions->Spaces->__sizeContinuousZoomVelocitySpace = 1;
	pPtzOptions->Spaces->ContinuousZoomVelocitySpace = (struct tt__Space1DDescription *)soap_malloc(soap, sizeof(struct tt__Space1DDescription));
	memset(pPtzOptions->Spaces->ContinuousZoomVelocitySpace, 0, sizeof(struct tt__Space1DDescription));
	pPtzOptions->Spaces->ContinuousZoomVelocitySpace->URI = (char *)soap_malloc(soap, sizeof(char)*SMALL_INFO_LENGTH);
	memset(pPtzOptions->Spaces->ContinuousZoomVelocitySpace->URI, 0, sizeof(char)*SMALL_INFO_LENGTH);
	strcpy(pPtzOptions->Spaces->ContinuousZoomVelocitySpace->URI, "http://www.onvif.org/ver10/tptz/ZoomSpaces/VelocityGenericSpace");
	pPtzOptions->Spaces->ContinuousZoomVelocitySpace->XRange = (struct tt__FloatRange *)soap_malloc(soap, sizeof(struct tt__FloatRange));
	memset(pPtzOptions->Spaces->ContinuousZoomVelocitySpace->XRange, 0, sizeof(struct tt__FloatRange));
	pPtzOptions->Spaces->ContinuousZoomVelocitySpace->XRange->Max = 1.0;
	pPtzOptions->Spaces->ContinuousZoomVelocitySpace->XRange->Min = -1.0;

	pPtzOptions->Spaces->__sizePanTiltSpeedSpace = 1;
	pPtzOptions->Spaces->PanTiltSpeedSpace = (struct tt__Space1DDescription *)soap_malloc(soap, sizeof(struct tt__Space1DDescription));
	memset(pPtzOptions->Spaces->PanTiltSpeedSpace, 0, sizeof(struct tt__Space1DDescription));
	pPtzOptions->Spaces->PanTiltSpeedSpace->URI = (char *)soap_malloc(soap, sizeof(char)*SMALL_INFO_LENGTH);
	memset(pPtzOptions->Spaces->PanTiltSpeedSpace->URI, 0, sizeof(char)*SMALL_INFO_LENGTH);
	strcpy(pPtzOptions->Spaces->PanTiltSpeedSpace->URI, "http://www.onvif.org/ver10/tptz/PanTiltSpaces/GenericSpeedSpace");
	pPtzOptions->Spaces->PanTiltSpeedSpace->XRange = (struct tt__FloatRange *)soap_malloc(soap, sizeof(struct tt__FloatRange));
	memset(pPtzOptions->Spaces->PanTiltSpeedSpace->XRange, 0, sizeof(struct tt__FloatRange));
	pPtzOptions->Spaces->PanTiltSpeedSpace->XRange->Max = 1.0;
	pPtzOptions->Spaces->PanTiltSpeedSpace->XRange->Min = -1.0;

	pPtzOptions->Spaces->__sizeZoomSpeedSpace = 1;
	pPtzOptions->Spaces->ZoomSpeedSpace = (struct tt__Space1DDescription *)soap_malloc(soap, sizeof(struct tt__Space1DDescription));
	memset(pPtzOptions->Spaces->ZoomSpeedSpace, 0, sizeof(struct tt__Space1DDescription));
	pPtzOptions->Spaces->ZoomSpeedSpace->URI = (char *)soap_malloc(soap, sizeof(char)*SMALL_INFO_LENGTH);
	memset(pPtzOptions->Spaces->ZoomSpeedSpace->URI, 0, sizeof(char)*SMALL_INFO_LENGTH);
	strcpy(pPtzOptions->Spaces->ZoomSpeedSpace->URI, "http://www.onvif.org/ver10/tptz/ZoomSpaces/ZoomGenericSpeedSpace");
	pPtzOptions->Spaces->ZoomSpeedSpace->XRange = (struct tt__FloatRange *)soap_malloc(soap, sizeof(struct tt__FloatRange));
	memset(pPtzOptions->Spaces->ZoomSpeedSpace->XRange, 0, sizeof(struct tt__FloatRange));
	pPtzOptions->Spaces->ZoomSpeedSpace->XRange->Max = 1.0;
	pPtzOptions->Spaces->ZoomSpeedSpace->XRange->Min = -1.0;

	pPtzOptions->PTZTimeout = (struct tt__DurationRange *)soap_malloc(soap, sizeof(struct tt__DurationRange));
	memset(pPtzOptions->PTZTimeout, 0, sizeof(struct tt__DurationRange));
	pPtzOptions->PTZTimeout->Max = 5000;
	pPtzOptions->PTZTimeout->Min = 1000;

  return SOAP_OK; 
}

SOAP_FMAC5 int SOAP_FMAC6 __tptz__GotoHomePosition(struct soap* soap, struct _tptz__GotoHomePosition *tptz__GotoHomePosition, struct _tptz__GotoHomePositionResponse *tptz__GotoHomePositionResponse)
{
    return SOAP_FAULT;    
}

SOAP_FMAC5 int SOAP_FMAC6 __tptz__SetHomePosition(struct soap* soap, struct _tptz__SetHomePosition *tptz__SetHomePosition, struct _tptz__SetHomePositionResponse *tptz__SetHomePositionResponse)
{
    return SOAP_FAULT;    
}

SOAP_FMAC5 int SOAP_FMAC6 __tptz__ContinuousMove(struct soap* soap, struct _tptz__ContinuousMove *tptz__ContinuousMove, struct _tptz__ContinuousMoveResponse *tptz__ContinuousMoveResponse)
{
  #define VISCA_MAX_SPEED 8
  int index = -1; //tptz__ContinuousMove->ProfileToken
  //tptz__ContinuousMove->Timeout
  printf("%s => ProfileToken:[%s]\n", __func__, tptz__ContinuousMove->ProfileToken);
  
	float xSpeed = 0, ySpeed = 0, zSpeed = 0;
  int action = 0, speed = 0;
  
	if(tptz__ContinuousMove->Velocity)
	{
		if(tptz__ContinuousMove->Velocity->PanTilt)
		{
			xSpeed = tptz__ContinuousMove->Velocity->PanTilt->x * VISCA_MAX_SPEED;
			ySpeed = tptz__ContinuousMove->Velocity->PanTilt->y * VISCA_MAX_SPEED;

			if(xSpeed)
			{
				action = (xSpeed > 0)?NVC_PTZ_RIGHT_START:NVC_PTZ_LEFT_START;
			}

			if(ySpeed)
			{
				if(ySpeed > 0)
				{
				  action = (action == NVC_PTZ_RIGHT_START)?NVC_PTZ_RIGHT_UP_START:
				           (action == NVC_PTZ_LEFT_START)?NVC_PTZ_LEFT_UP_START:
				           NVC_PTZ_UP_START;
				}
				else
				{
				  action = (action == NVC_PTZ_RIGHT_START)?NVC_PTZ_RIGHT_DOWN_START:
				           (action == NVC_PTZ_LEFT_START)?NVC_PTZ_LEFT_DOWN_START:
				           NVC_PTZ_DOWN_START;
				}
			}
      
      if(action)
      {
        xSpeed = (xSpeed < 0)?-xSpeed:xSpeed;
        ySpeed = (ySpeed < 0)?-ySpeed:ySpeed;

  			speed = xSpeed > ySpeed ? xSpeed : ySpeed;
        printf("%s => PanTilt action:%d, speed:%d, ret:%d\n", __func__, action, speed, ptz_ctl(0, action, speed));
      }
		}

		if(tptz__ContinuousMove->Velocity->Zoom)
		{
			zSpeed = tptz__ContinuousMove->Velocity->Zoom->x * VISCA_MAX_SPEED;
			
			if(zSpeed)
			{
			  action = (zSpeed > 0)?NVC_PTZ_ZOOM_ADD_START:NVC_PTZ_ZOOM_SUB_START;
			}

      if(action)
      {
        speed = (zSpeed < 0)?-zSpeed:zSpeed;
        printf("%s => Zoom action:%d, speed:%d, ret:%d\n", __func__, action, speed, ptz_ctl(0, action, speed));
      }
		}
	}
	
	return SOAP_OK; 
}

SOAP_FMAC5 int SOAP_FMAC6 __tptz__RelativeMove(struct soap* soap, struct _tptz__RelativeMove *tptz__RelativeMove, struct _tptz__RelativeMoveResponse *tptz__RelativeMoveResponse)
{
    return SOAP_FAULT;    
}

SOAP_FMAC5 int SOAP_FMAC6 __tptz__SendAuxiliaryCommand(struct soap* soap, struct _tptz__SendAuxiliaryCommand *tptz__SendAuxiliaryCommand, struct _tptz__SendAuxiliaryCommandResponse *tptz__SendAuxiliaryCommandResponse)
{
    return SOAP_FAULT;    
}

SOAP_FMAC5 int SOAP_FMAC6 __tptz__AbsoluteMove(struct soap* soap, struct _tptz__AbsoluteMove *tptz__AbsoluteMove, struct _tptz__AbsoluteMoveResponse *tptz__AbsoluteMoveResponse)
{
    return SOAP_FAULT;    
}

SOAP_FMAC5 int SOAP_FMAC6 __tptz__Stop(struct soap* soap, struct _tptz__Stop *tptz__Stop, struct _tptz__StopResponse *tptz__StopResponse)
{
  int index = -1;//tptz__Stop->ProfileToken
  int action = 0, speed = 0;
  printf("%s => ProfileToken:[%s]\n", __func__, tptz__Stop->ProfileToken);
  
	if(tptz__Stop->PanTilt || tptz__Stop->Zoom)
	{
		if(*tptz__Stop->PanTilt == _true)
		{
			action = NVC_PTZ_UP_STOP;
		}

		if(*tptz__Stop->Zoom == _true)
		{
			action = NVC_PTZ_ZOOM_ADD_STOP;
		}
    printf("%s => Stop action:%d, speed:%d, ret:%d\n", __func__, action, speed, ptz_ctl(0, action, speed));
	}

	return SOAP_OK;   
}

SOAP_FMAC5 int SOAP_FMAC6 __tptz__GetPresetTours(struct soap* soap, struct _tptz__GetPresetTours *tptz__GetPresetTours, struct _tptz__GetPresetToursResponse *tptz__GetPresetToursResponse)
{
    return SOAP_FAULT;    
}

SOAP_FMAC5 int SOAP_FMAC6 __tptz__GetPresetTour(struct soap* soap, struct _tptz__GetPresetTour *tptz__GetPresetTour, struct _tptz__GetPresetTourResponse *tptz__GetPresetTourResponse)
{
    return SOAP_FAULT;    
}

SOAP_FMAC5 int SOAP_FMAC6 __tptz__GetPresetTourOptions(struct soap* soap, struct _tptz__GetPresetTourOptions *tptz__GetPresetTourOptions, struct _tptz__GetPresetTourOptionsResponse *tptz__GetPresetTourOptionsResponse)
{
    return SOAP_FAULT;    
}

SOAP_FMAC5 int SOAP_FMAC6 __tptz__CreatePresetTour(struct soap* soap, struct _tptz__CreatePresetTour *tptz__CreatePresetTour, struct _tptz__CreatePresetTourResponse *tptz__CreatePresetTourResponse)
{
    return SOAP_FAULT;    
}

SOAP_FMAC5 int SOAP_FMAC6 __tptz__ModifyPresetTour(struct soap* soap, struct _tptz__ModifyPresetTour *tptz__ModifyPresetTour, struct _tptz__ModifyPresetTourResponse *tptz__ModifyPresetTourResponse)
{
    return SOAP_FAULT;    
}

SOAP_FMAC5 int SOAP_FMAC6 __tptz__OperatePresetTour(struct soap* soap, struct _tptz__OperatePresetTour *tptz__OperatePresetTour, struct _tptz__OperatePresetTourResponse *tptz__OperatePresetTourResponse)
{
    return SOAP_FAULT;    
}

SOAP_FMAC5 int SOAP_FMAC6 __tptz__RemovePresetTour(struct soap* soap, struct _tptz__RemovePresetTour *tptz__RemovePresetTour, struct _tptz__RemovePresetTourResponse *tptz__RemovePresetTourResponse)
{
    return SOAP_FAULT;    
}

////////////////////////////////////////////

void* start_nvt_discovery(void* parm)
{
START:;
    int ret = -1;
    unsigned char loop = 0;
    struct soap soap;
    struct ip_mreq mcast;
    int error_flag = 0;
    int count = 0;

    if(g_nvt->t.discovery_mode == tt__DiscoveryMode__Discoverable)
    {
		int count = 0; 
		while(count++ < 1) 
        {
			onvif_hello();
			usleep(200000);
		}
    }

    soap_init1(&soap, SOAP_IO_UDP);
    g_nvt->discovery_sock = soap.master;

    soap.bind_flags = SO_REUSEADDR;
    if(!soap_valid_socket(soap_bind(&soap, NULL, 3702, 128)))
    {
        fprintf(stderr, "[%s][%d] bind port error!\n", __FUNCTION__, __LINE__);
        soap_print_fault(&soap, stderr);
        return NULL;
    }
    mcast.imr_multiaddr.s_addr = inet_addr("239.255.255.250");
    mcast.imr_interface.s_addr = htonl(INADDR_ANY);

    if(setsockopt(soap.master, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char*)&mcast, sizeof(mcast)) < 0)
    {
        fprintf(stderr, "[%s][%d] add multicast error!\n", __FUNCTION__, __LINE__);
    }

    setsockopt(soap.master, IPPROTO_IP, IP_MULTICAST_LOOP, &loop, sizeof(loop));
    g_nvt->thread_discovery_flag = 0;

    while(!g_nvt->thread_discovery_flag)
    {       
        if((ret = soap_serve(&soap)))
        {
            fprintf(stderr, "[%s][%d] soap_serve error! ret=[%d]\n", __FUNCTION__, __LINE__, ret);
            soap_print_fault(&soap, stderr);
            count++;
        }
        
        usleep(1000);//2013-09-28 add for cpu usage 100%.
        soap_destroy(&soap);
        soap_end(&soap);
        
        if(0)//if(count > 10)
        {
            error_flag = 1;
            soap_done(&soap);
            fprintf(stderr, "[%s][%d] restart discovery!\n", __FUNCTION__, __LINE__);
            goto START;
        }
    }
    soap_done(&soap);
    pthread_exit(NULL);
    return NULL;
}

#if 0
void *process_queue(void *soap)
{
    struct soap *tsoap = (struct soap*)soap;
    int ret;
    fd_set r_set; 
    struct timeval tv = {.tv_sec = 5, .tv_usec = 0};
    for (;;)
    {
        tsoap->socket = dequeue();
        if (!soap_valid_socket(tsoap->socket))
        {
            fprintf(stderr, "[%s] invalid socket!\n", __FUNCTION__);
            break;
        }
#if 1
        FD_ZERO(&r_set);
        FD_SET(tsoap->socket, &r_set);
        ret = select(tsoap->socket+1, &r_set, NULL, NULL, &tv);
        if (ret > 0) 
        {
            if ((ret = soap_serve(tsoap)) != SOAP_OK) 
            {
                fprintf(stderr, "[%s] soap_serve error:%d!\n", ret);
            }
        }
        else
        {
            fprintf(stderr, "[%s] select timeout or has errno:%d\n", __FUNCTION__, errno);
        }
#endif       
#if 0
        soap_serve(tsoap);
#endif
        soap_destroy(tsoap);
        soap_end(tsoap);
    }
    return NULL;
}

int enqueue(SOAP_SOCKET sock)
{
    int status = SOAP_OK;
    int next;

    pthread_mutex_lock(&g_nvt->queue_cs);

    next = g_nvt->tail + 1;
    if (next >= MAX_QUEUE)
        next = 0;
    if (next == g_nvt->head)
        status = SOAP_EOM;
    else
    {
        g_nvt->queue[g_nvt->tail] = sock;
        g_nvt->tail = next;
    }

    pthread_cond_signal(&g_nvt->queue_cv);
    pthread_mutex_unlock(&g_nvt->queue_cs);
    return status;
}

SOAP_SOCKET dequeue()
{
    SOAP_SOCKET sock;
    pthread_mutex_lock(&g_nvt->queue_cs);

    while (g_nvt->head == g_nvt->tail)
        pthread_cond_wait(&g_nvt->queue_cv, &g_nvt->queue_cs);

    sock = g_nvt->queue[g_nvt->head++];

    if (g_nvt->head >= MAX_QUEUE)
        g_nvt->head = 0;

    pthread_mutex_unlock(&g_nvt->queue_cs);
    return sock;
}

void* start_nvt_serve(void *parm)
{
START:;
    struct soap soap;
    soap_init(&soap);
    int count = 0;
    int error_flag = 0;
    struct soap *soap_thr[MAX_THR]; // each thread needs a runtime context
    pthread_t tid[MAX_THR];
    SOAP_SOCKET m, s;
    int i;
    int ret; 

    soap.bind_flags = SO_REUSEADDR;
    m = soap_bind(&soap, NULL, g_nvt->p.port, 128);

    while(!soap_valid_socket(m))
    {
        fprintf(stderr, "[%s][%d] bind port error! rebind port!\n", __FUNCTION__, __LINE__);
        soap_print_fault(&soap, stderr);
        m = soap_bind(&soap, NULL, g_nvt->p.port, 128);
    }

    fprintf(stderr, "Socket connection successful %d\n", m);

    pthread_mutex_init(&g_nvt->queue_cs, NULL);
    pthread_cond_init(&g_nvt->queue_cv, NULL);

    for (i = 0; i < MAX_THR; i++)
    {
        soap_thr[i] = soap_copy(&soap);
        soap_thr[i]->recv_timeout = 5;
        fprintf(stderr, "Starting thread %d\n", i);
        ret = pthread_create(&tid[i], NULL, (void*(*)(void*))process_queue, (void*)soap_thr[i]);
        usleep(1000);
        if(ret != 0)
        {
            fprintf(stderr, "\n\n[%s]create pthread faild\n\n", __FUNCTION__);
        }
    }

    for (;;)
    {
        fprintf(stderr, "\n[%s] accpet request begin.\n", __FUNCTION__);
        s = soap_accept(&soap);
        fprintf(stderr, "\n[%s] accpet request end.\n", __FUNCTION__);
        if (!soap_valid_socket(s))
        {
            if (soap.errnum)
            {
                soap_print_fault(&soap, stderr);
                fprintf(stderr, "soap.errnum : [%d] retry\n", soap.errnum);
                usleep(1000);
                count++;
                if(count > 10)
                {
                    error_flag = 1;
                    break;
                }
                continue; // retry
            }
            else
            {
                fprintf(stderr, "Server timed out\n");
                break;
            }
        }
        fprintf(stderr, "Thread %d accepts socket %d connection from IP %d.%d.%d.%d\n", i, s, (soap.ip >> 24)&0xFF, (soap.ip >> 16)&0xFF, (soap.ip >> 8)&0xFF, soap.ip&0xFF);
        //请求的socket入队，如果队列已满则循环等待
        while (enqueue(s) == SOAP_EOM)
        {
            fprintf(stderr, "\n queue is full!\n");
            sleep(1);
       }
    }

    for (i = 0; i < MAX_THR; i++)
    {
        while (enqueue(SOAP_INVALID_SOCKET) == SOAP_EOM)
        {
            sleep(1);
        }
    }

    for (i = 0; i < MAX_THR; i++)
    {
        fprintf(stderr, "Waiting for thread %d to terminate... ", i);
        pthread_join(tid[i], NULL);
        fprintf(stderr, "terminated\n");
        soap_done(soap_thr[i]);
        free(soap_thr[i]);
    }

    pthread_mutex_destroy(&g_nvt->queue_cs);
    pthread_cond_destroy(&g_nvt->queue_cv);

    soap_done(&soap);
    if(error_flag = 1)
    {
        goto START;
    }
    return NULL;
}
#endif

#if 1

void *do_work(void *tsoap)
{
    int ret = -1;
    fd_set r_set; 
    struct timeval tv = {.tv_sec = 3, .tv_usec = 0};
    
    pthread_mutex_lock(&g_nvt->queue_cs);
    pthread_detach(pthread_self());

    struct soap *soap = (struct soap*)tsoap;
#if 0
    struct soap *soap = calloc(1, sizeof(struct soap));
    memcpy(soap, (char *)arg, sizeof(struct soap));
#endif
    soap->recv_timeout = 5;

    pthread_cond_signal(&g_nvt->queue_cv);
    pthread_mutex_unlock(&g_nvt->queue_cs); 

    FD_ZERO(&r_set);
    FD_SET(soap->socket, &r_set);

    ret = select(soap->socket+1, &r_set, NULL, NULL, &tv);
    if (ret > 0) 
    {
        if((ret = soap_serve(soap)) != SOAP_OK) 
        {
            fprintf(stderr, "[%s][%d] soap_serve error : %d!!!\n", __FUNCTION__, __LINE__, ret);
            soap_print_fault(soap, stderr);
        }
    }       
    usleep(1000);
    soap_destroy(soap);
    soap_end(soap);  
    soap_free(soap);
    
    pthread_exit(NULL);
}

void* start_nvt_serve(void *parm)
{
START:;
    pthread_t tid;
    struct timeval  now;
    struct timespec timeout;
    struct soap soap;
    soap_init(&soap);	
    SOAP_SOCKET m;
    int error_flag = 0;

    soap.bind_flags = SO_REUSEADDR;
    m = soap_bind(&soap, NULL, g_nvt->p.port/*5566*/, 128);
    g_nvt->thread_serve_flag = 0;

    while(!soap_valid_socket(m))
    {
        fprintf(stderr, "[%s][%d] bind port error! rebind port!\n", __FUNCTION__, __LINE__);
        soap_print_fault(&soap, stderr);
        m = soap_bind(&soap, NULL, 5566, 128);
    }

    g_nvt->serve_sock = soap.master;

    pthread_mutex_init(&g_nvt->queue_cs, NULL);
    pthread_cond_init(&g_nvt->queue_cv, NULL);

    while (!g_nvt->thread_serve_flag)  
    {
        if (!soap_valid_socket(soap_accept(&soap))) 
        {
        	soap_print_fault(&soap, stderr);
            error_flag = 1;
        	break;
        }
        usleep(1000);
            
        gettimeofday(&now, NULL);
        timeout.tv_sec  = now.tv_sec + 3;
        timeout.tv_nsec = now.tv_usec;
        pthread_mutex_lock(&g_nvt->queue_cs);
        struct soap *tsoap;
        tsoap = soap_copy(&soap);
        if (pthread_create(&tid, NULL, do_work, (void *)tsoap) < 0) 
        {
            fprintf(stderr, "[%s][%d] pthread_create do_work failed!\n", __FUNCTION__, __LINE__);	
        }

        int ret = pthread_cond_timedwait(&g_nvt->queue_cv, &g_nvt->queue_cs, &timeout); 
        if(ret == ETIMEDOUT) 
        {
            fprintf(stderr, "[%s][%d] wait do_work timeout!!!\n", __FUNCTION__, __LINE__);	   
        }
        
        pthread_mutex_unlock(&g_nvt->queue_cs);
    }

    soap_done(&soap);

    if(error_flag)
    {
        fprintf(stderr, "[%s][%d] error occur, restart nvt serve!\n", __FUNCTION__, __LINE__);
        goto START;
    }
    return 0;
}
#endif

void stop_nvt_discovery(void)
{
    g_nvt->thread_discovery_flag = 1;

    if(g_nvt->thread_discovery_flag >= 0)
    {
        onvif_bye();
        close(g_nvt->discovery_sock);
        pthread_kill(g_nvt->discovery_id, SIGTERM);
    }
}

void stop_nvt_serve(void)
{
    g_nvt->thread_serve_flag = 1;
    if(g_nvt->thread_serve_flag >= 0)
    {
        close(g_nvt->serve_sock);
        pthread_kill(g_nvt->serve_id, SIGTERM);        
    }
}


