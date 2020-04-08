#include <assert.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h>
#include <sys/stat.h>
#include <unistd.h>

#include "nvc.h"
#include "nvt.h"

// for nvt callback;
nvt_t *g_nvt;
int get_dev_info(void) {return 0;};
int get_dev_profiles(int chs, onvif_profile_t *profile, int num, int flag) {return 0;};//get all profiles
int get_vsc_profiles(int chs, video_source_conf_t *info, int num) {return 0;} ;//video source configure
int get_asc_profiles(int chs, audio_source_conf_t *info, int num) {return 0;} ;//audio source configure
int get_vec_profiles(int chs, video_encoder_conf_t *info, int num) {return 0;};//video encoder configure
int get_aec_profiles(int chs, audio_encoder_conf_t *info, int num) {return 0;};//audio encoder configure
int get_meta_profiles(void) {return 0;};//metadata configure
int get_PTZ_profiles(void) {return 0;};//PTZ configure profiles
int get_dev_capability(device_capability_t *dev_cap, int streamtype) {return 0;};
int set_vsc_conf(int chs, video_source_conf_t *info) {return 0;};
int set_vec_conf(int chs, video_encoder_conf_t *info) {return 0;};
int set_aec_conf(int chs, audio_encoder_conf_t *info) {return 0;};
int set_imaging_conf(int chs, imaging_conf_t *img) {return 0;};
int get_recording_list(nvt_time_t starttime, nvt_time_t endtime) {return 0;};
int get_dev_time(systime_t *time) {return 0;};
int set_dev_time(systime_t *time) {return 0;};
int get_dev_netinfo(net_info_t *info) {return 0;};
int set_dev_netinfo(net_info_t *info) {return 0;};
int get_user_info(user_info_t *info) {return 0;};
int add_user_info(char *name, char *passwd) {return 0;};
int del_user_info(char *name) {return 0;};
int set_user_info(char *name, char *passwd) {return 0;};
int get_device_syslog(int type, char *syslog) {return 0;};
int device_reboot() {return 0;};
int get_dev_portinfo(port_info_t *info) {return 0;};
int set_dev_portinfo(port_info_t *info) {return 0;};
int get_curr_chs() {return 0;};
int get_imaging_conf(int chs, imaging_conf_t *img, int num) {return 0;};
int get_recording_info(recording_info_t *info, char *token) {return 0;};
int set_device_factorydefault(int mask) {return 0;};

// for nvc callback;
int nvc_snap_req(char *uri, int chs, int streamtype, void *msg){return 0;};



#if 0  // for nvr;

#include "comm.h"

#define MAX_DEST_LEN 1280

/************************************************************************************/
//static void *_nvc_dev_open(char *ip, uint16_t port, char *user, char *pass, int timeout);

static NVC_PTZ_Preset_t *Preset = NULL;

static int s_parse_WH2resolution(uint16_t width, uint16_t height);

/***********************************************************************************/
int32_t nvc_init(void)
{
	return 0;
}
int32_t nvc_uninit(void)
{
	return 0;
}

#if 1
int32_t nvc_probe(int type, int timeout, NVC_PROBER_CB *cb, void *user_args)
{
	int ret = 0;
	//NVC_discovery(type);
	if (cb == NULL)
	{
		printf("[%s][%s]----->>>> Error: Invalid parameter! cb = NULL\n", __FILE__, __func__);
		return -1;
	}
	NVC_Probe_t probe;
	memset(&probe, 0, sizeof(NVC_Probe_t));
	
	probe.type = type;
	ret = NVC_discovery(&probe, timeout, cb, user_args);
	if (ret != 0)
	{
		printf("[%s][%s]----->>>> Error: NVC_discovery return error! ret: %d\n", __FILE__, __func__, ret);
		return -1;
	}
   
	return 0;
}
#else
int32_t nvc_probe(int type, int timeout, NVC_PROBER_CB *cb, void *user_args)
{
	int ret = 0;
	//NVC_discovery(type);
	if (cb == NULL)
	{
		printf("[%s][%s]----->>>> Error: Invalid parameter! cb = NULL\n", __FILE__, __func__);
		return -1;
	}
	NVC_Probe_t probe;
	nvc_probe_item_t item;

	memset(&probe, 0, sizeof(NVC_Probe_t));
	memset(&item, 0, sizeof(nvc_probe_item_t));
	
	probe.type = type;
	ret = NVC_discovery(&probe, timeout);
	if (ret != 0)
	{
		printf("[%s][%s]----->>>> Error: NVC_discovery return error! ret: %d\n", __FILE__, __func__, ret);
		return -1;
	}
	item.type = probe.type;
	item.port = probe.port;
	strncpy(item.ip, probe.ip, MAX_IP_LEN);

	NVC_Dev_t *dev = _nvc_dev_open(item.ip, item.port, NULL, NULL, timeout);
	if (dev)
	{
		printf("==========>>>>>>>>>> ch_num: %d\n", dev->ch_num);
		item.ch_num = dev->ch_num;
		nvc_dev_close(dev);
	}
	else
	{
		printf("[%s][%d]=====>>> nvc_dev_open failed!\n", __func__, __LINE__);
		item.ch_num = 2;
	}
	
	cb(&item, user_args);
	return 0;
}

static void *_nvc_dev_open(char *ip, uint16_t port, char *user, char *pass, int timeout)
{
	printf("[%s]--->>>>  in\n", __func__);
	int ret = 0;
	NVC_Dev_t *dev_handle = (NVC_Dev_t *)calloc(1, sizeof(NVC_Dev_t));

	//dev_handle->profile_tn = (Profile_info_t *)calloc(2, sizeof(Profile_info_t));
	dev_handle->capa = (NVC_capa_t *)calloc(1, sizeof(NVC_capa_t));

	strcpy(dev_handle->ip, ip);
	dev_handle->port = port;
	if (NULL != user)
	{
		strcpy(dev_handle->user, user);
	}
	if (NULL != pass)
	{
		strcpy(dev_handle->passwd, pass);
	}

	dev_handle->timeout = timeout;

	dev_handle->capa->capa_type = All;	/**Get all capabilities*/
	dev_handle->capa->ana_capa = (Analytics_capa_t *)calloc(1, sizeof(Analytics_capa_t));
	dev_handle->capa->dev_capa = (Device_capa_t *)calloc(1, sizeof(Device_capa_t));
	dev_handle->capa->env_capa = (Events_capa_t *)calloc(1, sizeof(Events_capa_t));
	dev_handle->capa->img_capa = (Img_capa_t *)calloc(1, sizeof(Img_capa_t));
	dev_handle->capa->media_capa = (Media_capa_t *)calloc(1, sizeof(Media_capa_t));
	dev_handle->capa->PTZ_capa = (PTZ_capa_t *)calloc(1, sizeof(PTZ_capa_t));

	ret = NVC_Get_Capabilities(dev_handle);
	if (ret != 0)
	{
		printf("[%s][%s][%d]--->>> NVC_Get_capabilities failed!\n, ret: %d\n",__FILE__, __func__, __LINE__, ret);
		nvc_dev_close(dev_handle);
		return NULL;
	}
	
	ret = NVC_Get_profiles(dev_handle);
	if (ret != 0)
	{
		printf("[%s][%s][%d]--->>> NVC_Get_capabilities failed!\n, ret: %d\n",__FILE__, __func__, __LINE__, ret);
		nvc_dev_close(dev_handle);
		return NULL;
	}

	printf("[%s]--->>>>  out\n", __func__);
	return dev_handle;
}
#endif

int32_t nvc_net_parm_set(nvc_net_parm_t *net_parm, char *name, char *passwd)
{
    assert(net_parm);
    printf("[%s][%d]=====================>>> in\n", __func__, __LINE__);
    //printf("[%s][%d]=====================>>> dst_id: %s\n", __func__, __LINE__, net_parm->dst_id);
    int ret = -1;
    
    if (strlen(net_parm->dst_id) > 0)
    {
        NVC_Net_info_t net_info;
        memset(&net_info, 0, sizeof(NVC_Net_info_t));
        strncpy(net_info.dst_id, net_parm->dst_id, sizeof(net_info.dst_id));
        
        strcpy(net_info.ip_info.if_name, net_parm->ip_info.if_name);
        strcpy(net_info.ip_info.ip_addr, net_parm->ip_info.ip_addr);
        strcpy(net_info.ip_info.mask, net_parm->ip_info.mask);
        strcpy(net_info.ip_info.gateway, net_parm->ip_info.gateway);
        strcpy(net_info.ip_info.mac, net_parm->ip_info.mac);
        strcpy(net_info.ip_info.dns1, net_parm->ip_info.dns1);
        strcpy(net_info.ip_info.dns2, net_parm->ip_info.dns2);
        net_info.ip_info.dhcp_enable = net_parm->ip_info.enable_dhcp;
        net_info.ip_info.dns_auto_en = net_parm->ip_info.dns_auto_en;
        strcpy(net_info.ntp_server, net_parm->ntp_server);
        net_info.http_port = net_parm->http_port;
        net_info.data_port = net_parm->data_port;
        net_info.cmd_port = net_parm->cmd_port;
        ret = NVC_Net_info_set(&net_info, name, passwd);
        if (ret != 0)
        {
            printf("[%s][%s]----->>>> Error: nvc_net_parm_set return error! ret: %d\n", __FILE__, __func__, ret);
            return -1;
        }
    }
    else
    {
        printf("[%s][%s]----->>>> Error: nvc_net_parm_set return error! ret: %d\n", __FILE__, __func__, ret);
        return -1;
    }

    printf("[%s][%d]=====================>>> out OK!\n", __func__, __LINE__);
    return 0;
}

int32_t nvc_net_parm_get(nvc_net_parm_t *net_parm, char *name, char *passwd)
{
    assert(net_parm);
    //printf("[%s][%d]=====================>>> in\n", __func__, __LINE__);
    //printf("[%s][%d]=====================>>> dst_id: %s\n", __func__, __LINE__, net_parm->dst_id);
    int ret = -1;
    NVC_Net_info_t net_info;
    memset(&net_info, 0, sizeof(NVC_Net_info_t));

    strncpy(net_info.dst_id, net_parm->dst_id, sizeof(net_info.dst_id));
    fprintf(stderr, "[%s] name:%s passwd:%s\n", __FUNCTION__, name, passwd);
    ret = NVC_Net_info_get(&net_info, name, passwd);
    if (ret != 0)
    {
        printf("[%s][%s]----->>>> Error: nvc_net_parm_set return error! ret: %d\n", __FILE__, __func__, ret);
        return -1;
    }
    strcpy(net_parm->ip_info.if_name, net_info.ip_info.if_name);
    strcpy(net_parm->ip_info.ip_addr, net_info.ip_info.ip_addr);
    strcpy(net_parm->ip_info.mask, net_info.ip_info.mask);
    strcpy(net_parm->ip_info.gateway, net_info.ip_info.gateway);
    strcpy(net_parm->ip_info.mac, net_info.ip_info.mac);
    strcpy(net_parm->ip_info.dns1, net_info.ip_info.dns1);
    strcpy(net_parm->ip_info.dns2, net_info.ip_info.dns2);
    net_parm->ip_info.enable_dhcp = net_info.ip_info.dhcp_enable;
    net_parm->ip_info.dns_auto_en = net_info.ip_info.dns_auto_en;
    strcpy(net_parm->ntp_server, net_info.ntp_server);
    net_parm->http_port = net_info.http_port;
    net_parm->data_port = net_info.data_port;
    net_parm->cmd_port = net_info.cmd_port;

    printf("[%s][%d]=====================>>> out OK!\n", __func__, __LINE__);
    return 0;
}

void *nvc_dev_open(char *ip, uint16_t port, char *user, char *pass, int timeout)
{
	//printf("[%s]--->>>>  in\n", __func__);
	int ret = -1;
	NVC_Dev_t *dev_handle = (NVC_Dev_t *)calloc(1, sizeof(NVC_Dev_t));

	//dev_handle->profile_tn = (Profile_info_t *)calloc(2, sizeof(Profile_info_t));
	dev_handle->capa = (NVC_capa_t *)calloc(1, sizeof(NVC_capa_t));

	strcpy(dev_handle->ip, ip);
	dev_handle->port = port;
	if (NULL != user)
	{
		strcpy(dev_handle->user, user);
	}
	if (NULL != pass)
	{
		strcpy(dev_handle->passwd, pass);
	}

	dev_handle->timeout = timeout;

	dev_handle->capa->capa_type = All;	/**Get all capabilities*/
	dev_handle->capa->ana_capa = (Analytics_capa_t *)calloc(1, sizeof(Analytics_capa_t));
	dev_handle->capa->dev_capa = (Device_capa_t *)calloc(1, sizeof(Device_capa_t));
	dev_handle->capa->env_capa = (Events_capa_t *)calloc(1, sizeof(Events_capa_t));
	dev_handle->capa->img_capa = (Img_capa_t *)calloc(1, sizeof(Img_capa_t));
	dev_handle->capa->media_capa = (Media_capa_t *)calloc(1, sizeof(Media_capa_t));
	dev_handle->capa->PTZ_capa = (PTZ_capa_t *)calloc(1, sizeof(PTZ_capa_t));

	ret = NVC_Get_Capabilities(dev_handle);
	if (ret != 0)
	{
		//printf("[%s][%s][%d]--->>> NVC_Get_capabilities failed!\n, ret: %d\n",__FILE__, __func__, __LINE__, ret);
		nvc_dev_close(dev_handle);
		return NULL;
	}
	
	ret = NVC_Get_profiles(dev_handle);
	if (ret != 0)
	{
		//printf("[%s][%s][%d]--->>> NVC_Get_capabilities failed!\n, ret: %d\n",__FILE__, __func__, __LINE__, ret);
		nvc_dev_close(dev_handle);
		return NULL;
	}
    /**************** get ptz pantile's and zoom's space ****************/
    dev_handle->ptz_conf = calloc(1, sizeof(NVC_PTZ_Configution_t));
    ret = NVC_PTZ_Get_Configurations(dev_handle, dev_handle->ptz_conf);
    if (ret != 0)
    {
        //printf("[%s][%d]===> NVC_Get_Presets return Error! ret = %d\n", __func__, __LINE__, ret);
    }
    else
    {
        #if 0
        printf("[%s][%d]===>PTZ pantilt space: %s\n, zoom space: %s\n"
                , __func__, __LINE__
                , dev_handle->ptz_conf->ContinuousPanTiltSpace
                , dev_handle->ptz_conf->ContinuousZoomSpace);
        #endif
    }
    /**获取预置位*/
    dev_handle->Preset = calloc(1, sizeof(NVC_PTZ_Preset_t));
    //dev_handle->Preset->_preset = calloc(MAX_PRESET_NUM, sizeof(PTZ_Preset_t));
    ret = NVC_PTZ_Get_Presets(dev_handle, dev_handle->Preset);

    if (ret != 0)
    {
        //printf("[%s][%d]===> NVC_Get_Presets return Error! ret = %d\n", __func__, __LINE__, ret);
    }
    else
    {
        #if 0
        int i = 0;
        //printf("[%s][%d]===> preset->size: %d\n", __func__, __LINE__, dev_handle->Preset->_size);
        for (i = 0; i < dev_handle->Preset->_size; i++)
        {
            printf("[%s][%d]===> preset[%d].name: %s, token: %s\n"
                , __func__, __LINE__
                , i
                , dev_handle->Preset->_preset[i].name
                , dev_handle->Preset->_preset[i].token);

            printf("[%s][%d]===> preset[%d].speed ==> [ pantilt => x: %f, y: %f, zoom => x: %f ]\n"
                , __func__, __LINE__
                , i
                , dev_handle->Preset->_preset[i].speed.pantilt.x 
                , dev_handle->Preset->_preset[i].speed.pantilt.y
                , dev_handle->Preset->_preset[i].speed.zoom.x );
        }
        #endif
    }
#if 0
    ret = NVC_PTZ_Preset_tour(dev_handle, dev_handle->Preset);
    if(ret != 0)
    {
        printf("[%s][%d]===> NVC_PTZ_Preset_tour return Error! ret = %d\n", __func__, __LINE__, ret);
    }
#endif
    return dev_handle;
}
int32_t nvc_dev_close(void *dev)
{
	NVC_Dev_t *dev_handle = (NVC_Dev_t *)dev;
	if (NULL == dev_handle)
	{
		return 0;
	}
    #if 1
    if (dev_handle->Preset)
    {
        /*
        if (dev_handle->Preset->_preset)
        {
            free(dev_handle->Preset->_preset);
            dev_handle->Preset->_preset = NULL;
        }
        */
        free(dev_handle->Preset);
        dev_handle->Preset = NULL;
    }
    if (dev_handle->ptz_conf)
    {
        free(dev_handle->ptz_conf);
        dev_handle->ptz_conf = NULL;
    }
    #else
    if (Preset)
    {
        if (Preset->_preset)
        {
            free(Preset->_preset);
            Preset->_preset = NULL;
        }
        free(Preset);
        Preset = NULL;
    }
    #endif
	if (dev_handle->capa)
	{
		if (dev_handle->capa->ana_capa)
		{
			free(dev_handle->capa->ana_capa);
			dev_handle->capa->ana_capa = NULL;
		}
		if (dev_handle->capa->dev_capa)
		{
			free(dev_handle->capa->dev_capa);
			dev_handle->capa->dev_capa = NULL;
		}
		if (dev_handle->capa->env_capa)
		{
			free(dev_handle->capa->env_capa);
			dev_handle->capa->env_capa = NULL;
		}
		if (dev_handle->capa->img_capa)
		{
			free(dev_handle->capa->img_capa);
			dev_handle->capa->img_capa = NULL;
		}
		if (dev_handle->capa->media_capa)
		{
			free(dev_handle->capa->media_capa);
			dev_handle->capa->media_capa = NULL;
		}
		if (dev_handle->capa->PTZ_capa)
		{
			free(dev_handle->capa->PTZ_capa);
			dev_handle->capa->PTZ_capa = NULL;
		}
		free(dev_handle->capa);
		dev_handle->capa = NULL;
	}
	
	if (dev_handle->profile_tn)	/**此块空间在onvif.c中的getprofiles中calloc*/
	{
		free(dev_handle->profile_tn);
		dev_handle->profile_tn = NULL;
	}

	free(dev_handle);
	dev_handle = NULL;
	return 0;
}

int nvc_dev_profile_get(void *dev, nvc_msg_t *pmsg)
{
	//printf("[%s]--->>>>  in\n", __func__);
	NVC_Dev_t *dev_handle = (NVC_Dev_t *)dev;
	int st_type = pmsg->st_ch;

	if (dev == NULL || dev_handle->profile_tn == NULL)
	{
		printf("[%s][%s][%d]=====>>> Error: invalid parameter! dev: %p\n"
			, __FILE__, __func__, __LINE__, dev);
		return -1;
	}
	
	nvc_profile_enc_t *nvc_profile = (nvc_profile_enc_t *)pmsg->data;

	if (st_type == 0 || st_type == 1)
	{
		nvc_profile->audio_enc = dev_handle->profile_tn[st_type].a_enc;
		nvc_profile->video_enc = dev_handle->profile_tn[st_type].v_enc;
		nvc_profile->frame_rate = dev_handle->profile_tn[st_type].frame_rate;
		uint16_t width  = dev_handle->profile_tn[st_type].width;
		uint16_t height = dev_handle->profile_tn[st_type].height;
		nvc_profile->resolution = s_parse_WH2resolution(width, height);
	}
	//printf("[%s]--->>>>  out\n", __func__);
	
	return 0;
}


int32_t nvc_media_url_get(void *dev, nvc_msg_t *pmsg)
{
	NVC_Dev_t *dev_handle = (NVC_Dev_t *)dev;
	int retval = 0;
	int ch = pmsg->ch;
	int st_type = pmsg->st_ch;
	
	sdk_media_url_t *nvc_url = (sdk_media_url_t *)(pmsg->data);
	NVC_Stream_Uri_t nvc_stream_uri;
	
	if (dev == NULL)
	{
		//printf("[%s]----->>>>> Error: no such channel,ch: %d, dev: %p\n", __func__, ch, dev);
		return -1;	
	}

	dev_handle->st_type = st_type;
	nvc_stream_uri.protocol = nvc_url->proto;
	nvc_stream_uri.trans_protocol = nvc_url->trans;

	if (strlen(dev_handle->capa->media_capa->url)== 0 
		|| strlen(dev_handle->profile_tn[dev_handle->st_type].profile_token) == 0)
	{
		//printf("[%s][%s][%d]---->>> Error: do not know soap_endpoint or profile_token!\n", __FILE__, __func__, __LINE__);
		/*printf("[%s][%s][%d]---->>> soap_endpoint: %s, profile_token: %s\n"
			, __FILE__, __func__, __LINE__, dev_handle->capa->media_capa->url, dev_handle->profile_tn[dev_handle->st_type].profile_token);*/
		return -1;
	}
	
	retval = NVC_GetStreamUri(dev_handle, &nvc_stream_uri);
	if (retval == -1)
	{
		//printf("[%s][%s][%d]---->>>> Error: NVC_GetStreamUri Error! ret: %d\n",__FILE__, __func__, __LINE__, retval);
		return -1;
	}
	memset(nvc_url->url, 0, strlen(nvc_url->url));
	strncpy(nvc_url->url, nvc_stream_uri.url, strlen(nvc_stream_uri.url));
    
    return 0;
}

static int s_parse_WH2resolution(uint16_t width, uint16_t height)
{
	int ret = 0;
	if (width == 1920 && height == 1080)
	{
		ret = SDK_VIDEO_RESOLUTION_1080p;
	}
	else if (width == 1280 && height == 720)
	{
		ret = SDK_VIDEO_RESOLUTION_720p;
	}
	else if (width == 1280 && height == 960)
	{
		ret = SDK_VIDEO_RESOLUTION_960p;
	}
	else if ((width == 704 && height == 576)
		|| (width ==704 && height == 480))
	{
		ret = SDK_VIDEO_RESOLUTION_D1;
	}
	else if (width == 352 && height == 288)
	{
		ret = SDK_VIDEO_RESOLUTION_CIF;
	}
	else if (176 && height == 144)
	{
		ret = SDK_VIDEO_RESOLUTION_QCIF;
	}
	else
	{
		//printf("[%s][%s]--->>> no such resolution! input w: %d, h: %d\n", __FILE__, __func__, width, height);
		//printf("[%s][%s]--->>> get default resolution! resolution: D1!\n", __FILE__, __func__);
		//ret = -1;
		ret = SDK_VIDEO_RESOLUTION_D1;
	}
	return ret;
}

int32_t nvc_enc_parm_get(void *dev, nvc_msg_t *pmsg)
{
	int ret = 0;
	nvc_enc_parm_t *enc_param = (nvc_enc_parm_t *)pmsg->data;
	media_info_t media_info;

	if (NULL == dev)
	{
		//printf("[%s][%s]----->>>>> Error: no such dev,ch: %d, dev: %p\n", __FILE__, __func__, pmsg->st_ch, dev);
		return -1;	
	}
	//printf("[%s][%s]----->>>>> ch: %d, st_type: %d\n", __FILE__, __func__, pmsg->ch, pmsg->st_ch);
	if (pmsg->ch < 0 || pmsg->ch > MAX_CHANN_NUM)
	{
		//printf("[%s]----->>>>> Error: no such channel!\n", __func__);
		return -1;	
	}
	
	NVC_Dev_t *dev_handle = (NVC_Dev_t *)dev;
	dev_handle->st_type = pmsg->st_ch;
	
	//NVC_Get_Audio_info(dev_handle, &media_info.audio);
	ret = NVC_Get_Video_info(dev_handle, &media_info.video);
	if (ret != 0)
	{
		//printf("[%s][%s]----->>>>> Error: NVC_Get_Video_info return error!, ret: %d\n", __FILE__, __func__, ret);
		return -1;	
	}

	if (pmsg->st_ch == 0)
	{
		/**************************** video enc ***************************/
		enc_param->main.video_enc = media_info.video.enc_type;
		enc_param->main.resolution = s_parse_WH2resolution(media_info.video.resolution.width, media_info.video.resolution.height);
		enc_param->main.pic_quilty = media_info.video.pic_quilty;
		enc_param->main.frame_rate = media_info.video.frame_rate;
		enc_param->main.bitrate = media_info.video.bitrate;
		enc_param->main.gop = media_info.video.gop;
		enc_param->main.level = media_info.video.level;
		//enc_param->main.mix_type = (media_info.audio_enable == 1) ? 0: 1;
		/****************************** audio enc **************************/
		//enc_param->main.audio_enc = media_info.audio.enc_type;
	}
	else if (pmsg->st_ch == 1)
	{
		/**************************** video enc ***************************/
		enc_param->second.video_enc = media_info.video.enc_type;
		enc_param->second.resolution = s_parse_WH2resolution(media_info.video.resolution.width, media_info.video.resolution.height);
		enc_param->second.pic_quilty = media_info.video.pic_quilty;
		enc_param->second.frame_rate = media_info.video.frame_rate;
		enc_param->second.bitrate = media_info.video.bitrate;
		enc_param->second.gop = media_info.video.gop;
		enc_param->second.level = media_info.video.level;
		//enc_param->second.mix_type = (media_info.audio_enable == 1) ? 0: 1;
		/****************************** audio enc **************************/
		//enc_param->second.audio_enc = media_info.audio.enc_type;
	}

	return 0;
}

int32_t nvc_enc_parm_set(void *dev, nvc_msg_t *pmsg)
{
	int ret = 0;
	nvc_enc_parm_t *enc_param = (nvc_enc_parm_t *)pmsg->data;
	media_info_t media_info;

    //printf("[%s]==============>>> in\n", __func__);
    
	if (NULL == dev)
	{
		//printf("[%s]----->>>>> Error: no such dev,ch: %d, dev: %p\n", __func__, pmsg->st_ch, dev);
		return -1;	
	}

	//printf("[%s][%s]----->>>>> ch: %d, st_type: %d\n", __FILE__, __func__, pmsg->ch, pmsg->st_ch);
	if (pmsg->ch < 0 || pmsg->ch > MAX_CHANN_NUM)
	{
		//printf("[%s]----->>>>> Error: no such channel!\n", __func__);
		return -1;	
	}
	
	NVC_Dev_t *dev_handle = (NVC_Dev_t *)dev;
	dev_handle->st_type = pmsg->st_ch;
	
	if (pmsg->st_ch == 0)
	{
		/**************************** video enc ***************************/
		media_info.video.enc_type = enc_param->main.video_enc;
		switch (enc_param->main.resolution)
		{
			case SDK_VIDEO_RESOLUTION_1080p:
				media_info.video.resolution.width  = 1920;
				media_info.video.resolution.height = 1080;
				break;
			case SDK_VIDEO_RESOLUTION_720p:
				media_info.video.resolution.width  = 1280;
				media_info.video.resolution.height = 720;
				break;
            case SDK_VIDEO_RESOLUTION_960p:
                media_info.video.resolution.width  = 1280;
                media_info.video.resolution.height = 960;
                break;
			case SDK_VIDEO_RESOLUTION_D1:
				media_info.video.resolution.width  = 704;
				media_info.video.resolution.height = 576;
				break;
			case SDK_VIDEO_RESOLUTION_CIF:
				media_info.video.resolution.width  = 352;
				media_info.video.resolution.height = 288;
				break;
			case SDK_VIDEO_RESOLUTION_QCIF:
				media_info.video.resolution.width  = 176;
				media_info.video.resolution.height = 144;
				break;
			default:
				media_info.video.resolution.width  = 704;
				media_info.video.resolution.height = 576;
				break;
		}
        
		media_info.video.pic_quilty 	= enc_param->main.pic_quilty;
		media_info.video.frame_rate 	= enc_param->main.frame_rate;
		media_info.video.bitrate 		= enc_param->main.bitrate;
		media_info.video.gop 			= enc_param->main.gop;
		media_info.video.level 			= enc_param->main.level;
		//enc_param->main.mix_type = (media_info.audio_enable == 1) ? 0: 1;
		/****************************** audio enc **************************/
		 //media_info.audio.enc_type = enc_param->main.audio_enc;
	}
	else if (pmsg->st_ch == 1)
	{
		/**************************** video enc ***************************/
		media_info.video.enc_type = enc_param->second.video_enc;
		switch (enc_param->second.resolution)
		{
			case SDK_VIDEO_RESOLUTION_1080p:
				media_info.video.resolution.width  = 1920;
				media_info.video.resolution.height = 1080;
				break;
			case SDK_VIDEO_RESOLUTION_720p:
				media_info.video.resolution.width  = 1280;
				media_info.video.resolution.height = 720;
				break;
            case SDK_VIDEO_RESOLUTION_960p:
                media_info.video.resolution.width  = 1280;
                media_info.video.resolution.height = 960;
                break;
			case SDK_VIDEO_RESOLUTION_D1:
				media_info.video.resolution.width  = 704;
				media_info.video.resolution.height = 576;
				break;
			case SDK_VIDEO_RESOLUTION_CIF:
				media_info.video.resolution.width  = 352;
				media_info.video.resolution.height = 288;
				break;
			case SDK_VIDEO_RESOLUTION_QCIF:
				media_info.video.resolution.width  = 176;
				media_info.video.resolution.height = 144;
				break;
			default:
				media_info.video.resolution.width  = 704;
				media_info.video.resolution.height = 576;
				break;
		}
		media_info.video.pic_quilty 	= enc_param->second.pic_quilty;
		media_info.video.frame_rate  	= enc_param->second.frame_rate;
		media_info.video.bitrate	 	= enc_param->second.bitrate;
		media_info.video.gop 		 	= enc_param->second.gop;
		media_info.video.level 		 	= enc_param->second.level;
		//enc_param->main.mix_type = (media_info.audio_enable == 1) ? 0: 1;
		/****************************** audio enc **************************/
		 //media_info.audio.enc_type = enc_param->second.audio_enc;
	}
	
	//NVC_Set_Audio_info(dev_handle, &media_info.audio);
	ret = NVC_Set_Video_info(dev_handle, &media_info.video);
	if (ret != 0)
	{
		//printf("[%s][%s]----->>>>> Error: NVC_Set_Video_info return error!, ret: %d\n", __FILE__, __func__, ret);
		return -1;	
	}
    //printf("[%s]==============>>> out\n", __func__);
	return 0;
}

int32_t nvc_img_attr_get(void *dev, nvc_msg_t *pmsg)  //nvc_image_attr_t
{
    //printf("[%s][%d]=====================>>> in\n", __func__, __LINE__);
    int ret = 0;
	nvc_image_attr_t *img_parm = (nvc_image_attr_t *)pmsg->data;
    NVC_Img_attr_t nvc_img;
    memset(&nvc_img, 0, sizeof(NVC_Img_attr_t));

	if (NULL == dev)
	{
		//printf("[%s][%s]----->>>>> Error: no such dev,ch: %d, dev: %p\n", __FILE__, __func__, pmsg->st_ch, dev);
		return -1;	
	}
	//printf("[%s][%s]----->>>>> ch: %d, st_type: %d\n", __FILE__, __func__, pmsg->ch, pmsg->st_ch);
	if (pmsg->ch < 0 || pmsg->ch > MAX_CHANN_NUM)
	{
		//printf("[%s]----->>>>> Error: no such channel!\n", __func__);
		return -1;	
	}
	
	NVC_Dev_t *dev_handle = (NVC_Dev_t *)dev;
	dev_handle->st_type = pmsg->st_ch;
        
    ret = NVC_Get_Img_options(dev_handle, &nvc_img);

    ret = NVC_Get_Img_attr_info(dev_handle, &nvc_img);
    if (ret != 0)
	{
		//printf("[%s][%s]----->>>>> Error: NVC_Get_Img_attr_info return error!, ret: %d\n", __FILE__, __func__, ret);
		return -1;	
	}

    img_parm->brightness = nvc_img.brightness;
    img_parm->contrast   = nvc_img.contrast;
    img_parm->saturation = nvc_img.saturation;
    img_parm->hue        = nvc_img.hue;
    img_parm->sharpness  = nvc_img.sharpness;

#if 0
    fprintf(stderr, "[%s][%d]======>>> brightness: %d, contrast: %d, saturation: %d, hue: %d, sharpness: %d\n"
                ,__func__, __LINE__
                , img_parm->brightness
                , img_parm->contrast  
                , img_parm->saturation
                , img_parm->hue       
                , img_parm->sharpness);
#endif
    //printf("[%s][%d]=====================>>> out OK!\n", __func__, __LINE__);
    return 0;
}

int32_t nvc_img_attr_set(void *dev, nvc_msg_t *pmsg)  //nvc_image_attr_t
{
    //printf("[%s][%d]=====================>>> in\n", __func__, __LINE__);
    int ret = 0;
	nvc_image_attr_t *img_parm = (nvc_enc_parm_t *)pmsg->data;
    NVC_Img_attr_t nvc_img;
    memset(&nvc_img, 0, sizeof(NVC_Img_attr_t));

	if (NULL == dev)
	{
		//printf("[%s][%s]----->>>>> Error: no such dev,ch: %d, dev: %p\n", __FILE__, __func__, pmsg->st_ch, dev);
		return -1;	
	}
	//printf("[%s][%s]----->>>>> ch: %d, st_type: %d\n", __FILE__, __func__, pmsg->ch, pmsg->st_ch);
	if (pmsg->ch < 0 || pmsg->ch > MAX_CHANN_NUM)
	{
		//printf("[%s]----->>>>> Error: no such channel!\n", __func__);
		return -1;	
	}
	
	NVC_Dev_t *dev_handle = (NVC_Dev_t *)dev;
	dev_handle->st_type = pmsg->st_ch;

    nvc_img.brightness = img_parm->brightness;
    nvc_img.contrast   = img_parm->contrast;
    nvc_img.saturation = img_parm->saturation;
    nvc_img.hue        = img_parm->hue;
    nvc_img.sharpness  = img_parm->sharpness;
#if 0
    fprintf(stderr, "[%s][%d]======>>> brightness: %d, contrast: %d, saturation: %d, hue: %d, sharpness: %d\n"
                ,__func__, __LINE__
                , img_parm->brightness
                , img_parm->contrast  
                , img_parm->saturation
                , img_parm->hue       
                , img_parm->sharpness);
#endif
    ret = NVC_Get_Img_options(dev_handle, &nvc_img);
    
    ret = NVC_Set_Img_attr_info(dev_handle, &nvc_img);
    if (ret != 0)
	{
		//printf("[%s][%s]----->>>>> Error: NVC_Get_Img_attr_info return error!, ret: %d\n", __FILE__, __func__, ret);
		return -1;	
	}

    //printf("[%s][%d]=====================>>> out OK!\n", __func__, __LINE__);
    return 0;
}

int32_t nvc_ptz_presets_get(void *dev, nvc_msg_t *pmsg)
{
    //printf("[%s]==============>>> in\n", __func__);

    assert(pmsg);
    assert(pmsg->data);
    int ret = 0;
    sdk_preset_param_t  *preset = (sdk_preset_param_t *)pmsg->data;
    
    if (dev == NULL)
    {
        printf("[%s][%d]----->>>>> Error: No such device, ch: %d, dev: %p\n"
                , __func__
                , pmsg->st_ch,
                dev);
        return -1;
    }

	NVC_Dev_t *dev_handle = (NVC_Dev_t *)dev;
	dev_handle->st_type = pmsg->st_ch;
    
    ret = NVC_PTZ_Get_Presets(dev_handle, dev_handle->Preset);
    if (ret != 0)
    {
        return -1;
    }

    printf("[%s][%d]===================>>> preset_num: %d\n", __func__, __LINE__, dev_handle->Preset->_size);
    if (dev_handle->Preset->_size <= MAX_PRESET_NUM)
    {
	    int i = 0;    

	    fprintf(stderr, "cruise_token :%s\n", dev_handle->Preset->ptz_tour_token);
    	memset(preset, 0, sizeof(sdk_preset_param_t));
        for (i = 0; i < dev_handle->Preset->_size; i++)
        {
#if 0               
        	fprintf(stderr, "i : %d, get_preset_name :%s, token :%s\n", 
        		i, 
        		dev_handle->Preset->_preset[i].name, 
        		dev_handle->Preset->_preset[i].token);
#else        
            if (strlen(dev_handle->Preset->_preset[i].name) > 1)
            {
                preset->preset_set[preset->num].enable = 1;
                preset->preset_set[preset->num].no     = i + 1;
				sprintf(preset->preset_set[preset->num].name, ONVIF_PRESET_NAME_FMT, i+1);
                
                preset->num++;

            }            
#endif            
        }
    }
    
    //printf("[%s]==============>>> out\n", __func__);
    return 0;
}

int32_t nvc_ptz_ctl(void *dev, nvc_msg_t *pmsg)
{
    fprintf(stderr, "[%s]==============>>> in\n", __func__);
    assert(pmsg);
    assert(pmsg->data);
	int ret = 0;
	sdk_ptz_ctl_t *ptz_ctl = (sdk_ptz_ctl_t*)pmsg->data;

	if (dev == NULL)
	{
		printf("[%s]----->>>>> Error: no such device,ch: %d, dev: %p\n", __func__, pmsg->st_ch, dev);
		return -1;	
	}

	NVC_Dev_t *dev_handle = (NVC_Dev_t *)dev;
	dev_handle->st_type = pmsg->st_ch;

    //fprintf(stderr, "[%s][%d]======>>> st_type: %d, cmd: 0x%03x, speed: %d\n"
    //    , __func__, __LINE__
    //    , dev_handle->st_type
    //    , ptz_ctl->cmd, ptz_ctl->speed);
    
	switch (ptz_ctl->cmd)
	{
		case SDK_PTZ_MSG_UP_START: /**0x001*/
        {
            NVC_PTZ_Speed_t nvc_ptz_speed;
			nvc_ptz_speed.pantilt.x = 0.0;
			nvc_ptz_speed.pantilt.y = 0.0 + (ptz_ctl->speed / 10.0);
			nvc_ptz_speed.zoom.x = 0.0; 
            ret = NVC_PTZ_ContinusMove(dev_handle, &nvc_ptz_speed);
        }
			break;
		case SDK_PTZ_MSG_UP_STOP:
        {
            NVC_PTZ_Stop_t ptz_stop;
            ptz_stop.en_pant = __true;
            ptz_stop.en_zoom = __false;
            ret = NVC_PTZ_Stop(dev_handle, &ptz_stop);
		}
            break;
		case SDK_PTZ_MSG_DOWN_START: /**0x003*/
        {
            NVC_PTZ_Speed_t nvc_ptz_speed;
			nvc_ptz_speed.pantilt.x = 0.0;
			nvc_ptz_speed.pantilt.y = 0.0 - (ptz_ctl->speed / 10.0);
			nvc_ptz_speed.zoom.x = 0.0;
			ret = NVC_PTZ_ContinusMove(dev_handle, &nvc_ptz_speed);
		}
            break;
		case SDK_PTZ_MSG_DOWN_STOP:
        {
            NVC_PTZ_Stop_t ptz_stop;
            ptz_stop.en_pant = __true;
            ptz_stop.en_zoom = __false;
            ret = NVC_PTZ_Stop(dev_handle, &ptz_stop);
		}
            break;
		case SDK_PTZ_MSG_LEFT_START: /**0x005*/
        {
            NVC_PTZ_Speed_t nvc_ptz_speed;
			nvc_ptz_speed.pantilt.x = 0.0 - (ptz_ctl->speed / 10.0);
			nvc_ptz_speed.pantilt.y = 0.0;
			nvc_ptz_speed.zoom.x = 0.0;
            ret = NVC_PTZ_ContinusMove(dev_handle, &nvc_ptz_speed);
		}
            break;
		case SDK_PTZ_MSG_LEFT_STOP:
        {
            NVC_PTZ_Stop_t ptz_stop;
            ptz_stop.en_pant = __true;
            ptz_stop.en_zoom = __false;
            ret = NVC_PTZ_Stop(dev_handle, &ptz_stop);
		}
            break;
		case SDK_PTZ_MSG_RIGHT_START: /**0x007*/
        {
            NVC_PTZ_Speed_t nvc_ptz_speed;
			nvc_ptz_speed.pantilt.x = 0.0 + (ptz_ctl->speed / 10.0);
			nvc_ptz_speed.pantilt.y = 0.0;
			nvc_ptz_speed.zoom.x = 0.0;
            ret = NVC_PTZ_ContinusMove(dev_handle, &nvc_ptz_speed);
		}
            break;
		case SDK_PTZ_MSG_RIGHT_STOP:
        {
            NVC_PTZ_Stop_t ptz_stop;
            ptz_stop.en_pant = __true;
            ptz_stop.en_zoom = __false;
            ret = NVC_PTZ_Stop(dev_handle, &ptz_stop);
		}
            break;
		case SDK_PTZ_MSG_LEFT_UP_START: /**0x009*/
        {
            NVC_PTZ_Speed_t nvc_ptz_speed;
			nvc_ptz_speed.pantilt.x = 0.0 - (ptz_ctl->speed / 10.0);
			nvc_ptz_speed.pantilt.y = 0.0 + (ptz_ctl->speed / 10.0);
			nvc_ptz_speed.zoom.x = 0.0;
            ret = NVC_PTZ_ContinusMove(dev_handle, &nvc_ptz_speed);
		}
            break;
		case SDK_PTZ_MSG_LEFT_UP_STOP:
        {
            NVC_PTZ_Stop_t ptz_stop;
            ptz_stop.en_pant = __true;
            ptz_stop.en_zoom = __false;
            ret = NVC_PTZ_Stop(dev_handle, &ptz_stop);
		}
            break;
		case SDK_PTZ_MSG_RIGHT_UP_START: /**0x00b*/
        {
            NVC_PTZ_Speed_t nvc_ptz_speed;
			nvc_ptz_speed.pantilt.x = 0.0 + (ptz_ctl->speed / 10.0);
			nvc_ptz_speed.pantilt.y = 0.0 - (ptz_ctl->speed / 10.0);
			nvc_ptz_speed.zoom.x = 0.0;
            ret = NVC_PTZ_ContinusMove(dev_handle, &nvc_ptz_speed);
		}
            break;
		case SDK_PTZ_MSG_RIGHT_UP_STOP:
        {
            NVC_PTZ_Stop_t ptz_stop;
            ptz_stop.en_pant = __true;
            ptz_stop.en_zoom = __false;
            ret = NVC_PTZ_Stop(dev_handle, &ptz_stop);
		}
            break;
		case SDK_PTZ_MSG_LEFT_DOWN_START: /**0x00d*/
        {
            NVC_PTZ_Speed_t nvc_ptz_speed;
			nvc_ptz_speed.pantilt.x = 0.0 - (ptz_ctl->speed / 10.0);
			nvc_ptz_speed.pantilt.y = 0.0 - (ptz_ctl->speed / 10.0);
			nvc_ptz_speed.zoom.x = 0.0;
            ret = NVC_PTZ_ContinusMove(dev_handle, &nvc_ptz_speed);
		}
            break;
		case SDK_PTZ_MSG_LEFT_DOWN_STOP:
        {
            NVC_PTZ_Stop_t ptz_stop;
            ptz_stop.en_pant = __true;
            ptz_stop.en_zoom = __false;
            ret = NVC_PTZ_Stop(dev_handle, &ptz_stop);
		}
            break;
		case SDK_PTZ_MSG_RIGHT_DOWN_START: /**0x00f*/
        {
            NVC_PTZ_Speed_t nvc_ptz_speed;
			nvc_ptz_speed.pantilt.x = 0.0 + (ptz_ctl->speed / 10.0);
			nvc_ptz_speed.pantilt.y = 0.0 - (ptz_ctl->speed / 10.0);
			nvc_ptz_speed.zoom.x = 0.0;
            ret = NVC_PTZ_ContinusMove(dev_handle, &nvc_ptz_speed);
		}
            break;
		case SDK_PTZ_MSG_RITHT_DOWN_STOP:
        {
            NVC_PTZ_Stop_t ptz_stop;
            ptz_stop.en_pant = __true;
            ptz_stop.en_zoom = __false;
            ret = NVC_PTZ_Stop(dev_handle, &ptz_stop);
		}
            break;
        case SDK_PTZ_MSG_PRESET_SET:
            {
                if (dev_handle->Preset->_size <= MAX_PRESET_NUM)
                {
                    PTZ_Preset_t _preset;
                    //memset(_preset.name, 0, sizeof(_preset.name));
                    memset(&_preset, 0, sizeof(PTZ_Preset_t));
                    strncpy(_preset.name,  ptz_ctl->name, sizeof(_preset.name));
                    _preset.name[MAX_NAME_LEN - 1 ] = 0;
					sprintf(_preset.token, "%d", ptz_ctl->val);
                    
                    ret = NVC_PTZ_Preset_set(dev_handle, &_preset);
                    ret |= NVC_PTZ_Get_Presets(dev_handle, dev_handle->Preset);
                }
                else
                {
                     ret = -1;
                }
            }
            break;
        case SDK_PTZ_MSG_PRESET_CALL:
            {
                int i = 0, nSet = 0;
                int preset_no = 0, preset_no_0 = 0, nRet = 0;
                char name_buf[MAX_NAME_LEN] = {0};
                for(i = 0; i < dev_handle->Preset->_size; i++)
                {
					nSet = 0;					
                    nSet = (strcmp(dev_handle->Preset->_preset[i].name, ptz_ctl->name) == 0) ? 1 : 0;
                    
					nRet = sscanf(dev_handle->Preset->_preset[i].name, "Preset%d", &preset_no);
					if (0 < nRet)
					{
						nRet = sscanf(ptz_ctl->name, "PtzName_%d", &preset_no_0);
						
						if (0 < nRet && (preset_no_0 == preset_no))
						{
							nSet = 1;
						}
					}
					
					
					if (1 == nSet)
                    {
                        ret = NVC_PTZ_Preset_call(dev_handle, &(dev_handle->Preset->_preset[i]));
                        break;
                    }
                }

                if (i == dev_handle->Preset->_size)
                {
                	PTZ_Preset_t preset_excp;
                	memset(&preset_excp, 0, sizeof(PTZ_Preset_t));
					nRet = sscanf(ptz_ctl->name, "PtzName_%d", &preset_no_0);
					if (nRet > 0 & preset_no_0 < MAX_PRESET_NUM)
					{
						sprintf(preset_excp.name, "PtzName_%d", preset_no_0);
						sprintf(preset_excp.token, "%d", preset_no_0);
						preset_excp.staytime = 0;
                    }
                }
            }
            break;
        case SDK_PTZ_MSG_PRESET_DEL:
            {
                if (dev_handle->Preset->_size > 0)
                {
                    int i = 0;
                    for(i = 0; i < dev_handle->Preset->_size; i++)
                    {
                        if (strcmp(dev_handle->Preset->_preset[i].name, ptz_ctl->name) == 0)
                        {
                            ret = NVC_PTZ_Preset_del(dev_handle, &(dev_handle->Preset->_preset[i]));
                            ret |= NVC_PTZ_Get_Presets(dev_handle, dev_handle->Preset);
                            break;
                        }
                    }
                }
                else
                {
                    printf("[%s][%d]===> Preset->size: %d, MAX_PRESET_NUM: %d, <= 0\n"
                        , __func__, __LINE__
                        , dev_handle->Preset->_size
                        , MAX_PRESET_NUM);
                    ret = -1;
                }
            }
            break;
		case SDK_PTZ_MSG_ZOOM_ADD_START: /**0x014*/
        {
            NVC_PTZ_Speed_t nvc_ptz_speed;
			nvc_ptz_speed.pantilt.x = 0.0;
			nvc_ptz_speed.pantilt.y = 0.0;
			nvc_ptz_speed.zoom.x = 0.0 + (ptz_ctl->speed / 10.0); 
            ret = NVC_PTZ_ContinusMove(dev_handle, &nvc_ptz_speed);
		}
            break;
		case SDK_PTZ_MSG_ZOOM_ADD_STOP:
        {
            NVC_PTZ_Stop_t ptz_stop;
            ptz_stop.en_pant = __false;
            ptz_stop.en_zoom = __true;
            ret = NVC_PTZ_Stop(dev_handle, &ptz_stop);
		}
            break;
		case SDK_PTZ_MSG_ZOOM_SUB_START: /**0x016*/
		{
            NVC_PTZ_Speed_t nvc_ptz_speed;
            nvc_ptz_speed.pantilt.x = 0.0;
			nvc_ptz_speed.pantilt.y = 0.0;
			nvc_ptz_speed.zoom.x = 0.0 - (ptz_ctl->speed / 10.0); 
            ret = NVC_PTZ_ContinusMove(dev_handle, &nvc_ptz_speed);
		}
            break;
		case SDK_PTZ_MSG_ZOOM_SUB_STOP:
        {
            NVC_PTZ_Stop_t ptz_stop;
            ptz_stop.en_pant = __false;
            ptz_stop.en_zoom = __true;
            ret = NVC_PTZ_Stop(dev_handle, &ptz_stop);
		}
            break;
		default:
			//printf("[%s][%s]--->>> do nothing now!\n", __FILE__, __func__);
			break;
	}
	
	if (ret != 0)
	{
		//printf("[%s][%s]----->>>>> Error: NVC_Set_Video_info return error!, ret: %d\n", __FILE__, __func__, ret);
		return -1;	
	}
    //printf("[%s]==============>>> out\n", __func__);
	return 0;
}

int32_t nvc_snap_url_get(void *dev, nvc_msg_t *pmsg)
{
    int ret = 0;
    NVC_Snapshot_t *snapshot = (NVC_Snapshot_t *)pmsg->data;
    NVC_Snapshot_t nvc_pic;
    printf("[%s]==============>>> in\n", __func__);
    
    if (dev == NULL)
    {
        printf("[%s][%d]----->>>>> Error: No such device, ch: %d, dev: %p\n"
                , __func__
                , pmsg->st_ch,
                dev);
        return -1;
    }

    NVC_Dev_t *dev_handle = (NVC_Dev_t *)dev;
    dev_handle->st_type = pmsg->st_ch;

	if (pmsg->ch < 0 || pmsg->ch > MAX_CHANN_NUM)
	{
        printf("[%s]==============>>> in\n", __func__);
		return -1;	
    }
    dev_handle->ch_num  = pmsg->ch;

    ret = NVC_Get_SnapshotUri(dev_handle, &nvc_pic, pmsg->data);
    if (ret != 0)
    {
        return -1;
    }
    
    printf("[%s]==============>>> out\n", __func__);
    return nvc_pic.size;
}

int nvc_snap_req(char *uri, int chs, int streamtype, void *msg)
{
    printf("[%s]==============>>> in\n", __func__);

    int sockfd, ret, i, pos, port;
    struct sockaddr_in srv_addr, cli_addr;
    char str[4096];
    unsigned char buf[1024 * 100]; 
    char ipaddr[32];
    char *_ip, *part;
    char *tmp_ptr = NULL;
    char *url = NULL; 
    fd_set   t_set1;  
    struct timeval  tv;  
    int buflen = 0;
    char dstbuf[1024*1000] = {0};
    FILE * fp=NULL;
    int isbegin = 0;
    int isend = 0;
    int tmplen = 0;
    int filesize = 0;
    char path[32];
    struct stat statbuf;
    
    if((chs < 0) || (streamtype < 0))
    {
        return -1;
    }
    //gxr 2014-03-28 修改:判断web目录是在/app or /var下面。
    if(0 == stat("/app/www/",&statbuf))
    {
        if(access("/app/www/snap/", F_OK) < 0)
        {
          system("mkdir /app/www/snap/");
        }
        sprintf(path, "/app/www/snap/snap_%d_%d.jpg", chs, streamtype);
    }
    else if(0 == stat("/var/www/",&statbuf))
    {
        if(access("/var/www/snap/", F_OK) < 0)
        {
          system("mkdir /var/www/snap/");
        }
        sprintf(path, "/var/www/snap/snap_%d_%d.jpg", chs, streamtype);
    }
    else
    {
        return -1;
    }
      
    tmp_ptr = strstr(uri, "//");
    if(tmp_ptr)
    {
        tmp_ptr = tmp_ptr + 2;
    }
    else
        return -1;

    char *tmp = strchr(tmp_ptr, '/');
    if(tmp)
    {
        pos = (int)(tmp - tmp_ptr);
        memset (ipaddr, 0, sizeof(ipaddr));
        strncpy(ipaddr, tmp_ptr, pos);  //ipaddr---> ip:port
    }
    else
        return -1;

    url = tmp_ptr + pos;
    if(!strchr(ipaddr, ':'))
    {
        _ip = ipaddr;
        if(strstr(uri, "http://"))
            port = 80;
        else
            port = 443;
    }
    else
    {
        _ip = strtok(ipaddr, ":");
        while((part = strtok(NULL, ":")))
        {
            port = atoi(part);
        }
    }
 
    //build socket
    if((sockfd = socket(AF_INET, SOCK_STREAM,  0 )) == -1)
    {
        fprintf(stderr, "[%s] create socket error!\n", __FUNCTION__);  
        return -1;
    }
    
    srv_addr.sin_family = AF_INET;
    srv_addr.sin_port = htons(port);
    srv_addr.sin_addr.s_addr = inet_addr(_ip);

    if(connect(sockfd, (struct sockaddr *)&srv_addr, sizeof(srv_addr) )== -1) 
    {
        fprintf(stderr, "[%s] connect error!\n", __FUNCTION__);  
        close(sockfd);  
        return -1;
    }

    //send packet  
    memset(str, 0, sizeof(str));  
    sprintf(str, "GET %s HTTP/1.1\r\nHOST: %s\r\nConnection: Close\r\n\r\n", url, _ip);

    ret = write(sockfd, str, strlen(str));  
    if (ret < 0) 
    {
        fprintf(stderr, "[%s] send packet error! errno id:%d errno:%s!\n", __FUNCTION__, errno, strerror(errno));  
        close(sockfd);  
        return -1;        
    }

    // recv packet
    FD_ZERO(&t_set1);  
    FD_SET(sockfd, &t_set1);    
    tv.tv_sec= 2;  
    tv.tv_usec= 0;  

    fp = fopen(path,"wb+");
    if(fp < 0)
    {
        return -1;
    }
    unsigned char jpgstart[2] = {0xFF, 0xD8};
    unsigned char jpgend[2]   = {0xFF, 0xD9};

    while(1)
    {  
        ret = select(sockfd +1, &t_set1, NULL, NULL, &tv);  
        if(ret < 0)
        {
            fprintf(stderr, "[%s] select < 0!\n", __FUNCTION__);      
            break;
        }
        else if(ret > 0)
        {
            memset(buf, 0, sizeof(buf));  
            i = read(sockfd, buf, sizeof(buf));
            int j;
            memcpy(dstbuf + buflen, buf, i);
            buflen = buflen + i; 
            tmplen = buflen;

            for(j = 0; j < tmplen; j++)
            {
                if(!memcmp(dstbuf + j, jpgstart, 2))
                {   //find FFD8
                    //fprintf(stderr, "find FFD8 j =%d tmplen = %d\n", j, tmplen);
                    isbegin = j;
                }

                if(!memcmp(dstbuf + j, jpgend, 2))
                {   //find FFD9
                    //fprintf(stderr, "find FFD9 j =%d\n", j);
                    isend = j;
                    fwrite(dstbuf + isbegin, 1, isend - isbegin + 2, fp);
                }
            }
            
            if(i == 0)
            {  
                close(sockfd);  
                break;
            }     
        }
    }

    if(fp)
    {
        fclose(fp);
    }

    struct stat statbuff;

    if(stat(path, &statbuff) < 0)
    {  
        fprintf(stderr, "[%s] error!\n", __FUNCTION__);
        return -1;
    }
    else
    {  
        filesize = statbuff.st_size; 
        if(filesize < 0)
            return -1;
        else
        {
            printf("file = %d\n", filesize);
            
            if(msg == NULL)
            {
              fprintf(stderr, "[%s][%d] msg error!\n", __FUNCTION__, __LINE__);
            }
#if 0            
            snap_pic_t *p = (snap_pic_t *)msg;
            unsigned long pts;
            pts = chs;
            pts = pts<<32 | p->snap_type;
            p->snap_cb(pts, dstbuf + isbegin, filesize);
 #endif           
            return filesize;
        }
    }
}
int32_t nvc_systime_set(void *dev, nvc_msg_t *pmsg, sdk_time_t *time_cfg)
{
    int ret;
    char msg_buf[MAX_MSG_SIZE] = {0};
    
    printf("[%s]==============>>> in\n", __func__);
    if (dev == NULL)
    {
        printf("[%s][%d]----->>>>> Error: No such device, ch: %d, dev: %p\n"
                , __func__
                , pmsg->st_ch,
                dev);
        return -1;
    }

    NVC_Dev_t *dev_handle = (NVC_Dev_t *)dev;

    ret = NVC_Set_SystemTime(dev_handle, time_cfg);
    if(ret != 0)
    {
        printf("[%s][%s]----->>>>> Error: NVC_Set_SystemTime return error!, ret: %d\n", __FILE__, __func__, ret);
        return -1;
    }
    printf("[%s]==============>>> out\n", __func__);

    return 0;
}


int32_t nvc_cruise_get(void *dev, nvc_msg_t *pmsg)
{
    assert(pmsg);
    assert(pmsg->data);
    int ret = 0;
    //sdk_cruise_param_t *cruise = (sdk_cruise_param_t *)pmsg->data;
    
    if (dev == NULL)
    {
        printf("[%s][%d]----->>>>> Error: No such device, ch: %d, dev: %p\n"
                , __func__
                , pmsg->st_ch,
                dev);
        return -1;
    }

	NVC_Dev_t *dev_handle = (NVC_Dev_t *)dev;
	dev_handle->st_type = pmsg->st_ch;
    
    ret = NVC_PTZ_Preset_tour(dev_handle, dev_handle->Preset);
    if (ret != 0)
    {
        return -1;
    }

    printf("[%s][%d]===================>>> preset_num: %d\n", __func__, __LINE__, dev_handle->Preset->_size);
    if (dev_handle->Preset->_size <= MAX_PRESET_NUM)
    {
	    //int i = 0; 

	    fprintf(stderr, "cruise_token :%s\n", dev_handle->Preset->ptz_tour_token);
    }
    
    return 0;
}

int32_t nvc_cruise_start(void *dev, int32_t cruise_id, int32_t st_ch)
{
	int ret = 0;
	NVC_Dev_t *dev_handle = (NVC_Dev_t *)dev;
    NVC_PTZ_Preset_t _preset;

	dev_handle->st_type = st_ch;
	memset(&_preset, 0, sizeof(NVC_PTZ_Preset_t));
	sprintf(_preset.ptz_tour_token, "%d", cruise_id);
    
   	ret = NVC_PTZ_PresetTour_start(dev_handle, &_preset);
   	if (0 != ret)
   	{
		fprintf(stderr, "[%s][%s] cruiset start error, ret :%d\n", __FILE__, __func__, ret);   			
		return -1;
   	}
   	
   	return 0;
}


int32_t nvc_cruise_stop(void *dev, int32_t cruise_id, int32_t st_ch)
{
	int ret = 0;
	NVC_Dev_t *dev_handle = (NVC_Dev_t *)dev;
    NVC_PTZ_Preset_t _preset;

	dev_handle->st_type = st_ch;
	memset(&_preset, 0, sizeof(NVC_PTZ_Preset_t));
	sprintf(_preset.ptz_tour_token, "%d", cruise_id);
    
   	ret = NVC_PTZ_PresetTour_stop(dev_handle, &_preset);
   	if (0 != ret)
   	{
   		fprintf(stderr, "[%s][%s] cruiset stop error, ret :%d\n", __FILE__, __func__, ret);
		return -1;
   	}
   	
   	return 0;   	
}

///////////////////////////////////////////////////////////////

nvt_t *g_nvt;

///////////////////////////////////////////////////////////////

int32_t nvt_start(void)
{
    system("route add -net 239.255.255.0 netmask 255.255.255.0 dev eth0");
    if (pthread_create(&g_nvt->discovery_id, NULL, start_nvt_discovery, NULL) < 0) 
    {
        fprintf(stderr, "[%s]  ====> discovery service failed.........\n", __FUNCTION__);
            return -1;
    } 
    else 
    {
        fprintf(stderr, "[%s]  ====> discovery service starting.........\n", __FUNCTION__); 
    }   
#if 1
    if(pthread_create(&g_nvt->serve_id, NULL, start_nvt_serve, NULL) < 0)
    {
        fprintf(stderr, "[%s]  ====> NVT service failed.........\n", __FUNCTION__);
    }
    else
    {
        fprintf(stderr, "[%s]  ====> NVT service starting.........\n", __FUNCTION__);   
    }
#endif
    return 0;
}

int32_t nvt_stop(void)
{
    stop_nvt_discovery();
    stop_nvt_serve();
}


int32_t nvt_init(nvt_parm_t *parm)
{
    int i;
    g_nvt = (nvt_t*)calloc(1, sizeof(nvt_t));
    if(!g_nvt)
        return -1;
    
    memcpy(&g_nvt->p, parm, sizeof(nvt_parm_t));

    g_nvt->t.dev = (device_info_t*)calloc(1, sizeof(device_info_t));
    
    g_nvt->t.nprofiles = MAX_PROFILES;
    g_nvt->t.profiles = (onvif_profile_t*)calloc(1, g_nvt->t.nprofiles * sizeof(onvif_profile_t));

    for(i = 0; i < MAX_PROFILES; i++)
    {
        g_nvt->t.profiles[i].vsc = (video_source_conf_t *)calloc(1, sizeof(video_source_conf_t));
        g_nvt->t.profiles[i].asc = (audio_source_conf_t *)calloc(1, sizeof(audio_source_conf_t));
        g_nvt->t.profiles[i].vec = (video_encoder_conf_t *)calloc(1, sizeof(video_encoder_conf_t));
        g_nvt->t.profiles[i].aec = (audio_encoder_conf_t *)calloc(1, sizeof(audio_encoder_conf_t));
        g_nvt->t.profiles[i].img = (imaging_conf_t *)calloc(1, sizeof(imaging_conf_t)); 

        //add by lyx
    	g_nvt->t.profiles[i].PTZc = (PTZ_conf_t *)calloc(1, sizeof(PTZ_conf_t));
    }
    //get_dev_profiles(ALL_CHS,  g_nvt->t.profiles, 2, VSC | ASC | VEC | AEC);
    //get_dev_info();
    setDefaultNameToken();
    return 0;
}
int32_t nvt_uninit(void)
{
    int i;    
    if(g_nvt)   
    {         
        for(i = 0; i < MAX_PROFILES; i++)   
        {   
            if(g_nvt->t.profiles[i].img) free(g_nvt->t.profiles[i].img);   
            if(g_nvt->t.profiles[i].vsc) free(g_nvt->t.profiles[i].vsc);     
            if(g_nvt->t.profiles[i].asc) free(g_nvt->t.profiles[i].asc);     
            if(g_nvt->t.profiles[i].vec) free(g_nvt->t.profiles[i].vec);      
            if(g_nvt->t.profiles[i].aec) free(g_nvt->t.profiles[i].aec);
        }       
        if(g_nvt->t.profiles) free(g_nvt->t.profiles);
        
        if(g_nvt->t.dev) free(g_nvt->t.dev);

        free(g_nvt);        
        g_nvt = NULL;   
    }
}

///////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////
int setDefaultNameToken()
{
    int num, i;
    char VSCname[SMALL_INFO_LENGTH];
    char VSCtoken[SMALL_INFO_LENGTH];
    char sourcetoken[SMALL_INFO_LENGTH];
    char VECname[SMALL_INFO_LENGTH];
    char VECtoken[SMALL_INFO_LENGTH];
    char pname[SMALL_INFO_LENGTH];
    char ptoken[SMALL_INFO_LENGTH];
    
    memset(VSCname, 0, SMALL_INFO_LENGTH);
    memset(VSCtoken, 0, SMALL_INFO_LENGTH);
    memset(sourcetoken, 0, SMALL_INFO_LENGTH);
    memset(VECname, 0, SMALL_INFO_LENGTH);
    memset(VECtoken, 0, SMALL_INFO_LENGTH);
    memset(pname, 0, SMALL_INFO_LENGTH);
    memset(ptoken, 0, SMALL_INFO_LENGTH);
    
    num = get_curr_chs();
    num = num * 2;
    for(i = 0; i < num; i++)
    {
        sprintf(sourcetoken, "CH%02dVideoSourceToken", i/2);
        sprintf(VSCname, "CH%02dVideoConfigureName", i/2);
        sprintf(VSCtoken, "CH%02dVideoConfigureToken", i/2);

        if(i%2 == 0)
        {        
            sprintf(pname, "CH%02dProfileName", i/2);
            sprintf(ptoken, "CH%02dProfileToken", i/2);
            sprintf(VECname, "CH%02dVideoEncodeName", i/2);
            sprintf(VECtoken, "CH%02dVideoEncodeToken", i/2);
        }
        else
        {
            sprintf(pname, "CH%02dSubProfileName", i/2);
            sprintf(ptoken, "CH%02dSubProfileToken", i/2);
            sprintf(VECname, "CH%02dSubVideoEncodeName", i/2);
            sprintf(VECtoken, "CH%02dSubVideoEncodeToken", i/2);
        }

        strcpy(g_nvt->t.profiles[i].vsc->vname, VSCname);
        strcpy(g_nvt->t.profiles[i].vsc->vtoken, VSCtoken);
        strcpy(g_nvt->t.profiles[i].vsc->vsourcetoken, sourcetoken);
        strcpy(g_nvt->t.profiles[i].vec->name, VECname);
        strcpy(g_nvt->t.profiles[i].vec->token, VECtoken);
        strcpy(g_nvt->t.profiles[i].pname, pname);
        strcpy(g_nvt->t.profiles[i].ptoken, ptoken);
    }
    return 0;
}

int get_dev_info()
{
    int ret;
    char msg_buf[MAX_MSG_SIZE] = {0};
    ret = SEND_MSG_TO_MAIN(SDK_MAIN_MSG_SYS_CFG, SDK_PARAM_GET, 0, 0, msg_buf);
    if(ret == 0)
    {
        sdk_sys_cfg_t *sys_cfg = ((sdk_msg_t*)msg_buf)->data;
#if 1
        fprintf(stderr, "[NVT][%s] devtype=[%d] hardware_ver=[%s] serial_no=[%s] software_ver=[%s]\n"
                        ,__FUNCTION__
                        ,sys_cfg->dev_type
                        ,sys_cfg->hardware_ver
                        ,sys_cfg->serial_no
                        ,sys_cfg->software_ver);
#endif
        strncpy(g_nvt->t.dev->hwid       , sys_cfg->hardware_ver , INFO_LEN);
        strncpy(g_nvt->t.dev->serial     , sys_cfg->serial_no    , INFO_LEN);
        strncpy(g_nvt->t.dev->sw         , sys_cfg->software_ver , INFO_LEN);
    }
    else
    {
        fprintf(stderr, "[NVT][%s] Get device info error!\n", __FUNCTION__);
    }
}

int get_curr_chs()
{
    int chnum;
    int ret;
    char msg_buf[MAX_MSG_SIZE] = {0};
    ret = SEND_MSG_TO_MAIN(SDK_MAIN_MSG_SYS_CFG, SDK_PARAM_GET, 0, 0, msg_buf);
   
    if(ret == 0)
    {
        sdk_sys_cfg_t *sys_cfg = ((sdk_msg_t*)msg_buf)->data;
        chnum = sys_cfg->ana_chan_num + sys_cfg->ip_chan_num;
    }
 
    return chnum;
}

int encoding_interval = 1;

int get_vsc_profiles(int chs, video_source_conf_t *info, int num)
{
    char name[SMALL_INFO_LENGTH];
    char token[SMALL_INFO_LENGTH];
    char sourcetoken[SMALL_INFO_LENGTH];
    int ret;
    char msg_buf[MAX_MSG_SIZE] = {0};
    uint8_t resolution;
    sdk_encode_t *videoconf;
    NET_IPV4 ip;
    char _IPAddr[LARGE_INFO_LENGTH] = {0};

    ip.int32 = net_get_ifaddr(ETH_NAME);
    sprintf(_IPAddr, "%d.%d.%d.%d", ip.str[0], ip.str[1], ip.str[2], ip.str[3]);

    //sprintf(_IPAddr,"127.0.0.1");
    memset(name, 0, sizeof(SMALL_INFO_LENGTH));
    memset(token, 0, sizeof(SMALL_INFO_LENGTH));
    memset(sourcetoken, 0, sizeof(SMALL_INFO_LENGTH));

    ret = SEND_MSG_TO_MAIN(SDK_MAIN_MSG_ENCODE_CFG, SDK_PARAM_GET, 0, chs/2, msg_buf);
                     
    if(ret == 0)
    {
        videoconf = ((sdk_msg_t*)msg_buf)->data;
        if(chs%2 == 0)
            resolution = videoconf->main.resolution;
        else
            resolution = videoconf->second.resolution;
    }
    else
    {
        //set default value
        if(chs%2 == 0)
            resolution = SDK_VIDEO_RESOLUTION_720p;
        else
            resolution = SDK_VIDEO_RESOLUTION_D1;
    }
    port_info_t port_info;
    ret = get_dev_portinfo(&port_info);
    if(ret < 0)
    {
        fprintf(stderr, "[get_vsc_profiles]====> not get port\n\n");
        port_info.rtsp_port = 554;//set default value
    }
    
    sprintf(sourcetoken, "CH%02dVideoSourceToken", chs/2);
    sprintf(name, "CH%02dVideoConfigureName", chs/2);
    sprintf(token, "CH%02dVideoConfigureToken", chs/2);
    strcpy(info->vname, name);
    strcpy(info->vtoken, token);
    strcpy(info->vsourcetoken, sourcetoken);

    if(chs%2 == 0)
    {
        sprintf(info->uri, "rtsp://%s:%d/0/%d/0", _IPAddr, port_info.rtsp_port, chs/2);
    }
    else
    {
        sprintf(info->uri, "rtsp://%s:%d/0/%d/1",_IPAddr, port_info.rtsp_port, chs/2);
    }

    info->vcount = num;
    info->x = 0;
    info->y = 0;

    //parse resolution
    switch(resolution)
    {
        case SDK_VIDEO_RESOLUTION_CIF:
            {
                info->width = 352;
                info->heigth = 288;
            }
            break;
        case SDK_VIDEO_RESOLUTION_D1:
            {
                info->width = 704;
                info->heigth = 576;
            }
            break;
        case SDK_VIDEO_RESOLUTION_720p:
            {
                info->width = 1280;
                info->heigth = 720;           
            }
            break;        
        case SDK_VIDEO_RESOLUTION_1080p:
            {
                info->width = 1920;
                info->heigth = 1080;                
            }
            break;
        case SDK_VIDEO_RESOLUTION_960p:
            {
                info->width = 1280;
                info->heigth = 960;
            }
            break;
        default:
            {
                info->width = 704;
                info->heigth = 576;
            }
            break;    
    }    
}

int get_imaging_conf(int chs, imaging_conf_t *img, int num)
{
    sdk_image_attr_t *imgconf;
    char msg_buf[MAX_MSG_SIZE] = {0};
    int ret;

    memset(msg_buf, 0, sizeof(msg_buf));  
    ret = SEND_MSG_TO_MAIN(SDK_MAIN_MSG_IMAGE_ATTR_CFG, SDK_PARAM_GET, 0, chs/2, msg_buf);

    if(ret == 0)
    {
        imgconf = ((sdk_msg_t*)msg_buf)->data;
        img->brightness = imgconf->brightness;
        img->ColorSaturation = imgconf->saturation;
        img->contrast = imgconf->contrast;
        img->sharpness = imgconf->sharpness;
       /*fprintf(stderr, "\n\nxxxxxxxxxxx[SDK_MAIN_MSG_IMAGE_ATTR_CFG] brightness=[%d] ColorSaturation=[%d] contrast=[%d] sharpness=[%d]\n\n"
                        ,imgconf->brightness
                        ,imgconf->saturation
                        ,imgconf->contrast
                        ,imgconf->sharpness);*/
    }
    else
    {
        img->brightness = 100.00;
        img->ColorSaturation = 100.00;
        img->contrast = 100.00;
        img->sharpness = 100.00;
    }
    img->IrCtFilter = 0;
    /*fprintf(stderr, "\n\nxxxxxxxxxxx[SDK_MAIN_MSG_IMAGE_ATTR_CFG][chs=%d] brightness=[%f] ColorSaturation=[%f] contrast=[%f] IrCtFilter=[%d] sharpness=[%f]\n\n"
                    ,chs
                    ,img->brightness
                    ,img->ColorSaturation
                    ,img->contrast
                    ,img->IrCtFilter
                    ,img->sharpness);*/

}

int get_asc_profiles(int chs, audio_source_conf_t *info, int num)
{
    char name[SMALL_INFO_LENGTH];
    char token[SMALL_INFO_LENGTH];
    char sourcetoken[SMALL_INFO_LENGTH];

    memset(name, 0, sizeof(SMALL_INFO_LENGTH));
    memset(token, 0, sizeof(SMALL_INFO_LENGTH));
    memset(sourcetoken, 0, sizeof(SMALL_INFO_LENGTH));

    sprintf(name, "CH%02dAudioConfigureName", chs/2);
    sprintf(token, "CH%02dAudioConfigureToken", chs/2);
    sprintf(sourcetoken, "CH%02dAudioSourceToken", chs/2);
    strcpy(info->aname, name);
    strcpy(info->token, token);
    strcpy(info->sourcetoken, sourcetoken);
    info->count = num;
}

int get_vec_profiles(int chs, video_encoder_conf_t *info, int num)
{
    char name[SMALL_INFO_LENGTH];
    char token[SMALL_INFO_LENGTH];
    int ret;
    char msg_buf[MAX_MSG_SIZE] = {0};
    sdk_encode_t *videoconf;
    uint8_t resolution;

    memset(name, 0, sizeof(SMALL_INFO_LENGTH));
    memset(token, 0, sizeof(SMALL_INFO_LENGTH));

    ret = SEND_MSG_TO_MAIN(SDK_MAIN_MSG_ENCODE_CFG, SDK_PARAM_GET, 0, chs/2, msg_buf);
    
    if(ret == 0)
    {
        videoconf = ((sdk_msg_t*)msg_buf)->data;
        if(chs%2 == 0)
        {          
            info->BitrateLimit = videoconf->main.bitrate;
            info->gLength = videoconf->main.gop;
            info->hProfile = videoconf->main.level;
            info->quality = videoconf->main.pic_quilty;
            info->FrameRateLimit = videoconf->main.frame_rate;
            resolution = videoconf->main.resolution;
        }
        else
        {
            info->BitrateLimit = videoconf->second.bitrate;
            info->gLength = videoconf->second.gop;
            info->hProfile = videoconf->second.level;
            info->quality = videoconf->second.pic_quilty;
            info->FrameRateLimit = videoconf->second.frame_rate;
            resolution = videoconf->second.resolution;
        }
    }
    else
    {
        //fprintf(stderr, "\n\nxxxxxxxxxxxxxxxxxx[get_vec_profiles][SDK_MAIN_MSG_ENCODE_CFG] ===> not get\n\n");
        //set default value
        info->BitrateLimit = 2048;
        info->gLength = 1;
        info->hProfile = 0;
        info->quality = 1;
        if(chs%2 == 0)
            resolution = SDK_VIDEO_RESOLUTION_720p;
        else
            resolution = SDK_VIDEO_RESOLUTION_D1;
    }

#if 0
    if(chs%2 == 0)
    {
        sprintf(name, "CH%02dVideoEncodeName", chs/2);
        sprintf(token, "CH%02dVideoEncodeToken", chs/2);
    }
    else
    {
        sprintf(name, "CH%02dSubVideoEncodeName", chs/2);
        sprintf(token, "CH%02dSubVideoEncodeToken", chs/2);
    }
    strcpy(info->name, name);
    strcpy(info->token, token);
#endif

    info->encoding = _H264;
    info->usecount = num;
    info->EncodingInterval = encoding_interval;
    info->MultiConf.autostart = 0;
    info->MultiConf.type = 0;
    info->MultiConf.port = g_nvt->p.port;
    info->MultiConf.ttl = 0;
    info->SessionTimeout = 720000;
    info->GovLength = 1;
    info->mProfile = SP;
    //parse resolution
    switch(resolution)
    {
        case SDK_VIDEO_RESOLUTION_CIF:
            {
                info->width = 352;
                info->height = 288;
            }
            break;
        case SDK_VIDEO_RESOLUTION_D1:
            {
                info->width = 704;
                info->height = 576;
            }
            break;
        case SDK_VIDEO_RESOLUTION_720p:
            {
                info->width = 1280;
                info->height = 720;           
            }
            break;        
        case SDK_VIDEO_RESOLUTION_1080p:
            {
                info->width = 1920;
                info->height = 1080;                
            }
            break;
        case SDK_VIDEO_RESOLUTION_960p:
            {
                info->width = 1280;
                info->height = 960;
            }
            break;
        default:
            {
                info->width = 704;
                info->height = 576;
            }
            break;    
    }    
}

int audio_samplerate = 1;
int audio_bitrate = 64;
int get_aec_profiles(int chs, audio_encoder_conf_t *info, int num)
{
    char name[SMALL_INFO_LENGTH];
    char token[SMALL_INFO_LENGTH];

    memset(name, 0, sizeof(SMALL_INFO_LENGTH));
    memset(token, 0, sizeof(SMALL_INFO_LENGTH));

    sprintf(name, "CH%02dAudioEncodeName", chs/2);
    sprintf(token, "CH%02dAudioEncodeToken", chs/2);

    strcpy(info->name, name);
    strcpy(info->token, token);
    info->encoding = _G711;
    info->bitrate = audio_bitrate;
    info->samplerate = audio_samplerate;
    info->MultiConf.autostart = 0;
    info->MultiConf.type = 0;
    info->MultiConf.port = g_nvt->p.port;
    info->MultiConf.ttl = 0;
    info->SessionTimeout = 72000;
    info->usecount = num;
}

#if 1
static int firstGetParamFlag = 1;
static time_t lastGetParamTm;
#endif

int get_dev_profiles(int chs, onvif_profile_t *profile, int num, int flag)
{
    int i, t;
    char name[SMALL_INFO_LENGTH];
    char token[SMALL_INFO_LENGTH];
    time_t currTime;
    memset(name, 0, sizeof(SMALL_INFO_LENGTH));
    memset(token, 0, sizeof(SMALL_INFO_LENGTH));
    
    printf("[%s]=========> in \n", __FUNCTION__);

    printf("[%s]=========> in  firstGetParamFlag=%d\n", __FUNCTION__, firstGetParamFlag);
    
#if 1
    if(chs == ALL_CHS)
    {
        if(firstGetParamFlag == 1)
        {
            lastGetParamTm = time(((time_t *)NULL));
            firstGetParamFlag = 0;
        }
        else 
        {
            currTime = time(((time_t *)NULL));
            
            printf("[%s]=========> (currTime-lastGetParamTm)=%d \n",  __FUNCTION__, (currTime - lastGetParamTm));
            if((currTime - lastGetParamTm) < 6*60)
            {
                return 0;
            }
            else
            {
                lastGetParamTm = currTime;
            }
        }
    }
    
    printf("[%s]=========> qqqqqqqq\n", __FUNCTION__);
    
#endif
    
    if(chs == ALL_CHS)
    {
        num = get_curr_chs();
        num = num * 2;
        for(i = 0; i < num; i++)
        {
            profile[i].pfixed = __false;
            if(flag&VSC)
            {
                get_vsc_profiles(i, profile[i].vsc, num);
            }
            if(flag&ASC)
            {
                get_asc_profiles(i, profile[i].asc, num);
            }
            if(flag&VEC)
            {
                get_vec_profiles(i, profile[i].vec, num);
            }
            if(flag&AEC)
            {
                get_aec_profiles(i, profile[i].aec, num);
            }
        }
    }
    else
    {
        if(chs%2 == 0)
        {
            sprintf(name, "CH%02dProfileName", chs/2);
            sprintf(token, "CH%02dProfileToken", chs/2);
        }
        else
        {
            sprintf(name, "CH%02dSubProfileName", chs/2);
            sprintf(token, "CH%02dSubProfileToken", chs/2);
        }
    
        strcpy(profile->pname, name);
        strcpy(profile->ptoken, token);

        profile->pfixed = __false;
        get_vsc_profiles(chs, profile->vsc, num);
        get_asc_profiles(chs, profile->asc, num);
        get_vec_profiles(chs, profile->vec, num);
        get_aec_profiles(chs, profile->aec, num);
    }
    printf("[%s]=========> out\n", __FUNCTION__);
    return 0;
}

int set_vsc_conf(int chs, video_source_conf_t *info)
{
    int ret;
    sdk_encode_t *videoconf;
    char msg_buf[MAX_MSG_SIZE] = {0};

    ret = SEND_MSG_TO_MAIN(SDK_MAIN_MSG_ENCODE_CFG, SDK_PARAM_GET, 0, chs/2, msg_buf);
    if(ret == 0)
    {
       videoconf = ((sdk_msg_t*)msg_buf)->data;
       if(chs%2 == 0)
       {  
           if((info->width == 704) && (info->heigth == 576))
           {
               videoconf->second.resolution = SDK_VIDEO_RESOLUTION_D1;          
           }
           else if((info->width == 1280) && (info->heigth == 720))
           {
               videoconf->second.resolution = SDK_VIDEO_RESOLUTION_720p;
           }
           else if((info->width == 1920) && (info->heigth == 1080))
           {
               videoconf->second.resolution = SDK_VIDEO_RESOLUTION_1080p;
           }
           else if((info->width == 1280) && (info->heigth == 960))
           {
               videoconf->second.resolution = SDK_VIDEO_RESOLUTION_960p;
           }
       }
       else
       {
           if((info->width == 352) && (info->heigth == 288))
           {
               videoconf->second.resolution = SDK_VIDEO_RESOLUTION_CIF;
           }
           else if((info->width == 704) && (info->heigth == 576))
           {
               videoconf->second.resolution = SDK_VIDEO_RESOLUTION_D1;        
           }
       }
       ret = SEND_MSG_TO_MAIN(SDK_MAIN_MSG_ENCODE_CFG, SDK_PARAM_SET, 0, chs/2, msg_buf);
    }
    else
    {
        fprintf(stderr, "[%s] chs %d has no video stream\n", __FUNCTION__, chs/2);
    }
 
    return ret;
}

int set_vec_conf(int chs, video_encoder_conf_t *info)
{
    int ret;
    sdk_encode_t *videoconf;
    char msg_buf[MAX_MSG_SIZE] = {0};

    ret = SEND_MSG_TO_MAIN(SDK_MAIN_MSG_ENCODE_CFG, SDK_PARAM_GET, 0, chs/2, msg_buf);
    if(ret == 0)
    {
        videoconf = ((sdk_msg_t*)msg_buf)->data;
        if(chs%2 == 0)
        {  
            videoconf->main.bitrate = info->BitrateLimit;
            videoconf->main.frame_rate = info->FrameRateLimit;
            videoconf->main.gop = info->gLength;
            videoconf->main.pic_quilty = info->quality;
            videoconf->main.level = info->hProfile;
            if((info->width == 1280) && (info->height == 720))
            {
                videoconf->main.resolution = SDK_VIDEO_RESOLUTION_720p;
            }
            else if((info->width == 1920) && (info->height == 1080))
            {
                videoconf->main.resolution = SDK_VIDEO_RESOLUTION_1080p;
            }
        }
        else
        {
            videoconf->second.bitrate = info->BitrateLimit;
            videoconf->second.frame_rate = info->FrameRateLimit;
            videoconf->second.gop = info->gLength;
            videoconf->second.pic_quilty = info->quality;
            videoconf->second.level = info->hProfile;
            if((info->width == 352) && (info->height == 288))
            {
                videoconf->second.resolution = SDK_VIDEO_RESOLUTION_CIF;
            }
            else if((info->width == 704) && (info->height == 576))
            {
                videoconf->second.resolution = SDK_VIDEO_RESOLUTION_D1;        
            }
        }
        encoding_interval = info->EncodingInterval;
        ret = SEND_MSG_TO_MAIN(SDK_MAIN_MSG_ENCODE_CFG, SDK_PARAM_SET, 0, chs/2, msg_buf);
        if(ret == 0)
        {
            memset(msg_buf, 0, sizeof(msg_buf));
            ret = SEND_MSG_TO_MAIN(SDK_MAIN_MSG_ENCODE_CFG, SDK_PARAM_GET, 0, chs/2, msg_buf);
            if(ret == 0)
            {
                videoconf = ((sdk_msg_t*)msg_buf)->data;
            }
        }
    }
    else
    {
        fprintf(stderr, "[%s] chs %d has no video stream\n", __FUNCTION__, chs/2);
    }

    return ret;
}

int set_aec_conf(int chs, audio_encoder_conf_t *info)
{
    int ret;
    sdk_encode_t *videoconf;
    char msg_buf[MAX_MSG_SIZE] = {0};
    
    videoconf = ((sdk_msg_t*)msg_buf)->data;
    videoconf->main.audio_enc = info->encoding;
    audio_bitrate = info->bitrate;
    audio_samplerate = info->samplerate;
    ret = SEND_MSG_TO_MAIN(SDK_MAIN_MSG_ENCODE_CFG, SDK_PARAM_SET, 0, chs/2, msg_buf);
    return ret;   
}

int set_imaging_conf(int chs, imaging_conf_t *img)
{
    int ret;
    sdk_image_attr_t *imgconf;
    char msg_buf[MAX_MSG_SIZE] = {0};

    imgconf = ((sdk_msg_t*)msg_buf)->data;
    imgconf->brightness = img->brightness;
    imgconf->contrast = img->contrast;
    imgconf->hue = img->IrCtFilter;
    imgconf->saturation = img->ColorSaturation;
    imgconf->sharpness = img->sharpness;
    
    ret = SEND_MSG_TO_MAIN(SDK_MAIN_MSG_IMAGE_ATTR_CFG, SDK_PARAM_SET, 0, chs/2, msg_buf);
    return ret;
}

int get_dev_time(systime_t *time)
{
    int ret;
    char msg_buf[MAX_MSG_SIZE] = {0};
    sdk_time_t *time_cfg;

    ret = SEND_MSG_TO_MAIN(SDK_MAIN_MSG_SYSTIME, SDK_PARAM_GET, 0, 0, msg_buf);
    if(ret == 0)
    {
        time_cfg = ((sdk_msg_t*)msg_buf)->data;
    }
    else
    {
        fprintf(stderr, "[%s][%d] get device time error!\n", __FUNCTION__, __LINE__);
        return -1;
    }

    time->datetype = Manual;
    strcpy(time->TZ, "GMT+08");
    time->daylightsaving = 0;
    time->UTCDateTime.year   = time_cfg->year;
    time->UTCDateTime.month  = time_cfg->mon;
    time->UTCDateTime.day    = time_cfg->day;
    time->UTCDateTime.hour   = time_cfg->hour;
    time->UTCDateTime.minute = time_cfg->min;
    time->UTCDateTime.second = time_cfg->sec;

    time->LocalDateTime = time->UTCDateTime;
    return 0;
}

int set_dev_time(systime_t *time)
{
    int ret;
    char msg_buf[MAX_MSG_SIZE] = {0};
    sdk_time_t *time_cfg;

    ret = SEND_MSG_TO_MAIN(SDK_MAIN_MSG_SYSTIME, SDK_PARAM_GET, 0, 0, msg_buf);
    if(ret == 0)
    {
        time_cfg = ((sdk_msg_t*)msg_buf)->data;
    }
    else
    {
        fprintf(stderr, "[%s][%d] get device time error!\n", __FUNCTION__, __LINE__);
        return -1;
    }


    time_cfg->year = time->UTCDateTime.year;
    time_cfg->mon  = time->UTCDateTime.month;
    time_cfg->day  = time->UTCDateTime.day;
    time_cfg->hour = time->UTCDateTime.hour;
    time_cfg->min  = time->UTCDateTime.minute;
    time_cfg->sec  = time->UTCDateTime.second;

    ret = SEND_MSG_TO_MAIN(SDK_MAIN_MSG_SYSTIME, SDK_PARAM_SET, 0, 0, msg_buf);
    if(ret < 0)
    {
        fprintf(stderr, "[%s][%d] SDK_MAIN_MSG_SYSTIME set error!\n", __FUNCTION__, __LINE__);
    }
    return 0;
}

int get_dev_netinfo(net_info_t *info)
{
    int ret;
    char msg_buf[MAX_MSG_SIZE] = {0};
  	sdk_ip_info_t *ip_info;

    ret = SEND_MSG_TO_MAIN(SDK_MAIN_MSG_NET_LINK_CFG, SDK_PARAM_GET, 0, 0, msg_buf);
    if(ret == 0)
    {
        ip_info = ((sdk_msg_t*)msg_buf)->data;
    }
    else
    {
        fprintf(stderr, "[%s][%d] SDK_MAIN_MSG_NET_LINK_CFG get error!\n", __FUNCTION__, __LINE__);
        return -1;
    }
    
    info->dhcp_status = ip_info->enable_dhcp;
    strncpy(info->dns1, ip_info->dns1, sizeof(info->dns1));
    strncpy(info->dns2, ip_info->dns2, sizeof(info->dns2));
    strncpy(info->gateway, ip_info->gateway, sizeof(info->gateway));
    strncpy(info->if_name, ip_info->if_name, sizeof(info->if_name));
    strncpy(info->ip_addr, ip_info->ip_addr, sizeof(info->ip_addr));
    strncpy(info->mac, ip_info->mac, sizeof(info->mac));
    strncpy(info->mask, ip_info->mask, sizeof(info->mask));
    info->dns_auto = ip_info->dns_auto_en;

    return 0;
}

int get_dev_portinfo(port_info_t *info)
{
    int ret;
    char msg_buf[MAX_MSG_SIZE] = {0};
	sdk_net_mng_cfg_t *net_info;

    ret = SEND_MSG_TO_MAIN(SDK_MAIN_MSG_NET_MNG_CFG, SDK_PARAM_GET, 0, 0, msg_buf);
    if(ret == 0)
    {
        net_info = ((sdk_msg_t*)msg_buf)->data;
    }
    else
    {
        fprintf(stderr, "[%s][%d] SDK_MAIN_MSG_NET_MNG_CFG get error!\n", __FUNCTION__, __LINE__);
        return -1;
    }
    
    info->http_port = net_info->http_port;
    info->rtsp_port = net_info->dvr_data_port;
    info->http_enable = TRUE;
    info->rtsp_enable = TRUE;
    return 0;
}

int set_dev_netinfo(net_info_t *info)
{

    int ret;
    char msg_buf[MAX_MSG_SIZE] = {0};
  	sdk_ip_info_t *ip_info;
    ip_info = ((sdk_msg_t*)msg_buf)->data;

    ip_info->enable_dhcp = info->dhcp_status;
    strncpy(ip_info->gateway, info->gateway, sizeof(info->gateway));
    strncpy(ip_info->if_name, info->if_name, sizeof(info->if_name));
    strncpy(ip_info->ip_addr, info->ip_addr, sizeof(info->ip_addr));
    strncpy(ip_info->mac, info->mac, sizeof(info->mac));
    strncpy(ip_info->mask, info->mask, sizeof(info->mask));
    ip_info->dns_auto_en = info->dns_auto;
    sprintf(ip_info->dns1, "%s", info->dns1);
    sprintf(ip_info->dns2, "%s", info->dns2);

    ret = SEND_MSG_TO_MAIN(SDK_MAIN_MSG_NET_LINK_CFG, SDK_PARAM_SET, 0, 0, msg_buf);
    if(ret != 0)
    {
        fprintf(stderr, "[%s][%d] SDK_MAIN_MSG_NET_LINK_CFG set error!\n", __FUNCTION__, __LINE__);
        return -1;
    }
    get_dev_netinfo(info);
    return 0;
}

int set_dev_portinfo(port_info_t *info)
{
    int ret;
    char msg_buf[MAX_MSG_SIZE] = {0};
	sdk_net_mng_cfg_t *net_info;     

    
    ret = SEND_MSG_TO_MAIN(SDK_MAIN_MSG_NET_MNG_CFG, SDK_PARAM_GET, 0, 0, msg_buf);
    if(ret == 0)
    {
        net_info = ((sdk_msg_t*)msg_buf)->data;
    }
    else
    {
        fprintf(stderr, "[%s][%d] SDK_MAIN_MSG_NET_MNG_CFG get error!\n", __FUNCTION__, __LINE__);
        return -1;
    }

    net_info->http_port = info->http_port;
    net_info->dvr_data_port = info->rtsp_port;

    ret = SEND_MSG_TO_MAIN(SDK_MAIN_MSG_NET_MNG_CFG, SDK_PARAM_SET, 0, 0, msg_buf);
    if(ret != 0)
    {
        fprintf(stderr, "[%s][%d] SDK_MAIN_MSG_NET_LINK_CFG set error!\n", __FUNCTION__, __LINE__);
        return -1;
    }

    return 0;
}

int get_user_info(user_info_t * info)
{
    int ret;
    char msg_buf[MAX_MSG_SIZE] = {0};

    ret = SEND_MSG_TO_MAIN(SDK_MAIN_MSG_USER, SDK_USER_QUERY, 0, 0, msg_buf);
    if(ret == 0)
    {
        memset(info->buf, 0, sizeof(info->buf));
        memcpy(info->buf, ((sdk_msg_t*)msg_buf)->data, sizeof(info->buf));
        info->size = (((sdk_msg_t*)msg_buf)->size)/sizeof(sdk_user_right_t);
    }
    else
    {
        fprintf(stderr, "[%s][%d] SDK_MAIN_MSG_USER get error!\n", __FUNCTION__, __LINE__);
        return -1;
    }

    return 0;
}

int add_user_info(char *name, char *passwd)
{
    int ret;
    char msg_buf[MAX_MSG_SIZE] = {0};
    sdk_user_right_t *info;
    info = ((sdk_msg_t*)msg_buf)->data;
    
    strcpy(info->user.user_name, name); 
    strcpy(info->user.user_pwd, passwd);

    ret = SEND_MSG_TO_MAIN(SDK_MAIN_MSG_USER, SDK_USER_ADD, 0, 0, msg_buf);
    if(ret < 0)
    {
        fprintf(stderr, "[%s][%d] SDK_MAIN_MSG_USER add error!\n", __FUNCTION__, __LINE__);
    }
    return 0;
}

int del_user_info(char *name)
{
    int ret;
    sdk_user_t *info;
    char msg_buf[MAX_MSG_SIZE] = {0};
    int i;
    user_info_t *users;
    sdk_user_right_t  *user_info = NULL;

    info = ((sdk_msg_t*)msg_buf)->data;
    users = (user_info_t *)calloc(1, sizeof(user_info_t));
    get_user_info(users);

    for(i = 0; i < users->size; i++)
    {
        user_info = (sdk_user_right_t *)(users->buf + sizeof(sdk_user_right_t) * i);
        if(!strcmp(user_info->user.user_name, name))
        {
            strcpy(info->user_pwd, user_info->user.user_pwd);
            break;
        }
    }

    strcpy(info->user_name, name);

    ret = SEND_MSG_TO_MAIN(SDK_MAIN_MSG_USER, SDK_USER_DEL, 0, 0, msg_buf);
    if(ret < 0)
    {
        fprintf(stderr, "[%s][%d] SDK_MAIN_MSG_USER del error!\n", __FUNCTION__, __LINE__);
    }
    return 0;
}

int set_user_info(char *name, char *passwd)
{
    int ret;
    char msg_buf[MAX_MSG_SIZE] = {0};
    sdk_user_right_t *info;
    info = ((sdk_msg_t*)msg_buf)->data;
    
    strcpy(info->user.user_name, name); 
    strcpy(info->user.user_pwd, passwd);

    ret = SEND_MSG_TO_MAIN(SDK_MAIN_MSG_USER, SDK_USER_MODI, 0, 0, msg_buf);
    if(ret < 0)
    {
        fprintf(stderr, "[%s][%d] SDK_MAIN_MSG_USER add error!\n", __FUNCTION__, __LINE__);
    }
    return 0;
}

int get_device_syslog(int type, char *syslog)
{
    char msg_buf[MAX_MSG_SIZE] = {0};
    char msg[MAX_MSG_SIZE] = {0};
    int ret;
    sdk_log_cond_t *log = ((sdk_msg_t*)msg)->data;
    sdk_time_t *time_cfg;
    sdk_log_item_t *log_store;
    char tmpinfo[64];
    int i;
    char loginfo[128];

    ret = SEND_MSG_TO_MAIN(SDK_MAIN_MSG_SYSTIME, SDK_PARAM_GET, 0, 0, msg_buf);
    if(ret == 0)
    {
        time_cfg = ((sdk_msg_t*)msg_buf)->data;
    }
    else
    {
        fprintf(stderr, "[%s][%d] get device time error!\n", __FUNCTION__, __LINE__);
        return -1;
    }

    //query one day's syslog
    log->begin_time.year = time_cfg->year;
    log->begin_time.mon  = time_cfg->mon;
    log->begin_time.day  = time_cfg->day;
    log->begin_time.hour = 0;
    log->begin_time.min  = 0;
    log->begin_time.sec  = 0;
    log->end_time.year   = time_cfg->year;
    log->end_time.mon    = time_cfg->mon;
    log->end_time.day    = time_cfg->day;
    log->end_time.hour   = 23;
    log->end_time.min    = 59;
    log->end_time.sec    = 59;

    if(type == 0)
    {//system
        log->type = 1;
    }
    else if(type == 1)
    {//access
        log->type = 4;
    }
    memcpy(log->user, "admin", sizeof(log->user));
    ret = SEND_MSG_TO_MAIN(SDK_MAIN_MSG_LOG, SDK_LOG_QUERY, 0, 0, msg);
    if(ret == 0)
    {
        log_store = ((sdk_msg_t*)msg)->data;
        int size = (((sdk_msg_t*)msg)->size)/(sizeof(sdk_log_item_t));
        //convent struct to string 
        for(i = 0; i < size; i++)
        {
             sprintf(tmpinfo,"LogTime:%04d-%02d-%02d %02d:%02d:%02d, Username:%s, IP:%s, loginfo:"
                    ,log_store[i].time.year
                    ,log_store[i].time.mon
                    ,log_store[i].time.day
                    ,log_store[i].time.hour
                    ,log_store[i].time.min
                    ,log_store[i].time.sec
                    ,log_store[i].user
                    ,log_store[i].ip_addr);
            memcpy(loginfo, log_store[i].desc, sizeof(log_store[i].desc));
            strcat(syslog, tmpinfo);
            strcat(syslog, "\n");
        }
    }
    else
    {
        fprintf(stderr, "[%s] SDK_MAIN_MSG_LOG get error!\n", __FUNCTION__);
    }

    return 0;
}

int device_reboot()
{
    fprintf(stderr, "[%s] reboot device...\n", __FUNCTION__);
    int ret;
    char msg[MAX_MSG_SIZE] = {0};
    ret = SEND_MSG_TO_MAIN(SDK_MAIN_MSG_CLOSE_SYSTEM, SDK_CLOSE_SYS_REBOOT, 0, 0, msg);
    if(ret != 0)
    {
        fprintf(stderr, "[%s] SDK_MAIN_MSG_CLOSE_SYSTEM reboot failed!\n", __FUNCTION__);
    }
}


int set_device_factorydefault(int mask)
{
    char msg_buf[MAX_MSG_SIZE] = {0};
    sdk_default_param_t *def;
    int ret;
    int i;
	int mask_id = 0;

    def = ((sdk_msg_t*)msg_buf)->data;
    
    if(mask == 1)//soft
    {
        for(i = 0; i < SDK_PARAM_MASK_BUIT; i++)
        {
            if (mask_id == SDK_PARAM_MASK_CHANNEL || mask_id == SDK_PARAM_MASK_NET)
                continue;               
            memset(def, 0, sizeof(def));
            def->param_mask |= 1<<mask_id;
            ret = SEND_MSG_TO_MAIN(SDK_MAIN_MSG_PARAM_DEFAULT, SDK_PARAM_GET, 0, 0, msg_buf);          
        }
    }
    else//hard
    {
        def->param_mask = PARAM_MASK_ALL;
        ret = SEND_MSG_TO_MAIN(SDK_MAIN_MSG_PARAM_DEFAULT, SDK_PARAM_SET, 0, 0, msg_buf);
    }

    if(ret != 0)
    {
        fprintf(stderr, "[%s] SDK_MAIN_MSG_PARAM_DEFAULT  set error!\n", __FUNCTION__);
    }

    return 0;
}

int fill_resolutions(device_capability_t *dev_cap, int stream_type, int i, int index)
{
    switch(stream_type)
    {
        case SUB_STREAM:
            {
                dev_cap->videostream[i].resheight[index][0] = 576;
                dev_cap->videostream[i].reswidth[index][0]  = 704;
                dev_cap->videostream[i].resheight[index][1] = 288;
                dev_cap->videostream[i].reswidth[index][1]  = 352;
            }
            break;
        case MAIN_STREAM:
        case SDK_DEV_TYPE_NVR_4_3520D:
        case SDK_DEV_TYPE_NVR_9_3520D:
		case SDK_DEV_TYPE_NVR_16_3520D:
            {
                dev_cap->videostream[i].resheight[index][0] = 720;
                dev_cap->videostream[i].reswidth[index][0]  = 1280;
                dev_cap->videostream[i].resheight[index][1] = 1080;
                dev_cap->videostream[i].reswidth[index][1]  = 1920;
            }
            break;
        default:
            break;
    }
}

int get_dev_capability(device_capability_t *dev_cap, int stream_type)
{
    int encodetype;
    int i;
 
    dev_cap->streamnums = get_curr_chs();
    dev_cap->streamnums = dev_cap->streamnums*2;
    encodetype = _H264;
    for(i = 0; i < dev_cap->streamnums; i++)
    {
        if(encodetype & _H264)
        {
            dev_cap->videostream[i].encodeformat |= _H264;
            dev_cap->videostream[i].maxframe[0] = 25;
            dev_cap->videostream[i].minframe[0] = 1;
            dev_cap->videostream[i].maxgovlen[0] = 200;
            dev_cap->videostream[i].mingovlen[0] = 1;
            dev_cap->videostream[i].maxinterval[0] = 3;
            dev_cap->videostream[i].mininterval[0] = 0;

            dev_cap->videostream[i].profileslevelnums = 3;
            dev_cap->videostream[i].profileslevelvalue[0] = 0;
            dev_cap->videostream[i].profileslevelvalue[1] = 1;
            dev_cap->videostream[i].profileslevelvalue[2] = 3;
            fill_resolutions(dev_cap, stream_type, i, 0);
        }
        else if(encodetype & _JPEG)
        {
            dev_cap->videostream[i].encodeformat |= _JPEG;
            dev_cap->videostream[i].maxframe[1] = 25;
            dev_cap->videostream[i].minframe[1] = 1;
            dev_cap->videostream[i].maxgovlen[1] = 200;
            dev_cap->videostream[i].mingovlen[1] = 1;
            dev_cap->videostream[i].maxinterval[1] = 3;
            dev_cap->videostream[i].mininterval[1] = 0;
            fill_resolutions(dev_cap, stream_type, i, 1);

        }
        else if(encodetype & _MPEG4)
        {
            dev_cap->videostream[i].encodeformat |= _MPEG4;
            dev_cap->videostream[i].maxframe[2] = 25;
            dev_cap->videostream[i].minframe[2] = 1;
            dev_cap->videostream[i].maxgovlen[2] = 200;
            dev_cap->videostream[i].mingovlen[2] = 1;
            dev_cap->videostream[i].maxinterval[2] = 3;
            dev_cap->videostream[i].mininterval[2] = 0;
            fill_resolutions(dev_cap, stream_type, i, 2);
        }
    }
}

time_t convert_time(sdk_time_t time)
{
    time_t to;
    struct tm from;

    from.tm_hour    = time.hour;
    from.tm_mday    = time.day;
    from.tm_min     = time.min;
    from.tm_year    = time.year - 1900;
    from.tm_sec     = time.sec;
    from.tm_mon     = time.mon - 1;

    to = mktime(&from);
    if(to < 0) 
    {
    	printf("mktime error! errno:%s\n", strerror(errno));
        return 0;
    }
    return to;
}   

int get_recording_list(sdk_time_t starttime, sdk_time_t endtime)
{
    char msg_buf[MAX_MSG_SIZE] = {0};
    sdk_record_item_t *record = NULL;
    int ret, chs;
    int i, j;
    int sum = 0;
    int count = 0;
    int res = 0;
    memset(g_nvt->t.record, 0, sizeof(g_nvt->t.record));
    sdk_msg_t *msg = (sdk_msg_t *)msg_buf;
    sdk_record_cond_t *cond = (sdk_record_cond_t *)msg->data;

    chs = get_curr_chs();
    
    for(i = 0; i < chs; i++)
    {
        memset(msg_buf, 0, sizeof(msg_buf));
        cond->is_locked = 0;
        cond->record_type = -1;
        memcpy(&cond->start_time, &starttime, sizeof(sdk_time_t ));
        memcpy(&cond->stop_time, &endtime, sizeof (sdk_time_t));

        ret = SEND_MSG_TO_MAIN1(SDK_MAIN_MSG_RECORD_QUERY, SDK_PARAM_GET, 0, i, msg);
        while(msg->ack == SDK_ERR_BUSY)
        {
            fprintf(stderr, "[%s] Disk is busy now!\n", __FUNCTION__);
            ret = SEND_MSG_TO_MAIN1(SDK_MAIN_MSG_RECORD_QUERY, SDK_PARAM_GET, 0, i, msg);
            count++;
            if(count > 10)
                break;
        }

        if(ret == 0)
        {
            sum = (msg->size)/sizeof(sdk_record_item_t);

            if(sum > MAX_RECORDING) 
                sum = MAX_RECORDING;

            if(sum != 0)
            {
                for(j = 0; j < sum; j++)
                {
                    record = (sdk_record_item_t *)(msg->data + sizeof(sdk_record_item_t)*j);
                    g_nvt->t.record[i][j].start_time = convert_time(record->start_time);
                    g_nvt->t.record[i][j].stop_time = convert_time(record->stop_time);
                    sprintf(g_nvt->t.record[i][j].token, "chs_%d_%d_%d", i, g_nvt->t.record[i][j].start_time, g_nvt->t.record[i][j].stop_time);
                }  
                res = sum + res;  
                sum = 0;

            }
            else
            {
                continue;
            } 
            g_nvt->t.nrecordings = res;
        }
        else if(ret < 0)
        {
            fprintf(stderr, "\n[%s] cannot get SDK_MAIN_MSG_RECORD_QUERY\n", __FUNCTION__);
        }
    }
    return 0;
}

int get_recording_info(recording_info_t *info, char *token)
{
    time_t starttm;
    time_t stoptm;
    int chs;
    char *_token = NULL;
    char *part = NULL;
    int i = 0;

    _token = strdup(token);
    if(!strchr(_token, '_'))
        chs = -1;
    part = strtok(_token, "_");
	while((part = strtok(NULL, "_")))
	{
        if(i == 0)
        {
            chs = atoi(part);
        }
        else if(i == 1)
        {
            info->start_time = atoi(part);
        }
        else if(i == 2)
        {
            info->stop_time = atoi(part);
        }
        i++;
	}
	free(_token);
    return chs;
}

#endif