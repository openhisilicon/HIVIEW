#include <stdio.h>
#include <unistd.h>
#include <math.h>
#include <string.h>

//#include "main.h"
#include "pthread.h"
#include "onvif_api.h"



//#define TEST_SERVER
//#define TEST_SEARCH
//#define TEST_GET_URL
//#define TEST_ALL

int main()
{
  return 0;
}


#ifdef TEST_SERVER
void main()
{
    int res;
    nvt_parm_t *parm;
    
    parm = (nvt_parm_t*)calloc(1, sizeof(nvt_parm_t));
    
    parm->port = 80;
    //parm->msg_cb = _msg_dispatch;

    nvt_init(parm);
    res = nvt_start();
    if(res)
    {
        fprintf(stderr, "start nvt error !\n");
        nvt_uninit();
        nvt_stop();
        exit(-1);
    }
    while(1)
    {
        sleep(1);
    }
}
#endif  // TEST_SERVER


#ifdef TEST_SEARCH
static char ip[64];
static uint16_t port;
static int callback(nvc_probe_item_t *item, void *user_args)
{
	if (strlen(item->ip) > 1)
	{	
		fprintf(stderr, "%s==========>>>> probe: ip: %s, port: %d \n", __func__, item->ip, item->port);
		memset(ip, 0, 64);
		strcpy(ip, item->ip);
		port = item->port;
	}
	else
		fprintf(stderr, "%s==========>>> probe: ip: NULL\n", __func__);
	return 0;
}
void main()
{
    nvc_probe_item_t item;

    while(1)
    {
        nvc_probe(0, 5, callback, NULL);
        sleep(30);
    }
}
#endif // TEST_SEARCH




#ifdef TEST_GET_URL
void main()
{

    char ip[64] = "192.168.1.68";
    uint16_t port = 8080;

    nvc_init();


    void *dev = nvc_dev_open(ip, port, NULL, NULL, 0);
    
    
    char buf[1024];
	nvc_msg_t * pmsg1 = (nvc_msg_t *)buf;
	pmsg1->ch = 2;
	pmsg1->st_ch = 0;
	sdk_media_url_t *url = (sdk_media_url_t *)pmsg1->data;
	url->proto = 0;
	url->trans = 3;
    
    nvc_media_url_get(dev, pmsg1);
	printf("[%s] url: %s\n",__func__ ,url->url);

    nvc_dev_close(dev);
    
    
    
    nvc_uninit();

    return 0;
}
#endif // TEST_GET_URL


#ifdef TEST_ALL
static char ip[64];
static uint16_t port;
static int callback(nvc_probe_item_t *item, void *user_args)
{
	if (strlen(item->ip) > 1)
	{	
		printf("%s==========>>>> probe: ip: %s, port: %d \n", __func__, item->ip, item->port);
		memset(ip, 0, 64);
		strcpy(ip, item->ip);
		port = item->port;
	}
	else
		printf("%s==========>>> probe: ip: NULL\n", __func__);
	return 0;
}
int main(void)
{
	nvc_probe_item_t item;
	nvc_probe(0, 5, callback, NULL);
    //return 0;
	
    
    
    int i = 0;
    
#if 1
    char *ip = "192.168.1.11";
    int port = 8899;
    #if 0
    nvc_net_parm_t net_parm;
    memset(&net_parm, 0, sizeof(nvc_net_parm_t));
    strcpy(net_parm.dst_id, "192.168.3.39:80");

    nvc_net_parm_get(&net_parm);


   uint8_t ntp_server[MAX_IP_ADDR_LEN];//NTP服务IP
    uint8_t res_server[MAX_IP_ADDR_LEN];//保留
    uint16_t http_port;         //web端口
    uint16_t cmd_port;          //控制端口
    uint16_t data_port;         //媒体端口
    uint16_t res_port;          //保留

 uint8_t ip_addr[MAX_IP_ADDR_LEN];//IP ADDR
    uint8_t mask[MAX_IP_ADDR_LEN];   //IP MASK
    uint8_t gateway[MAX_IP_ADDR_LEN];//网关

    strcpy(net_parm.ip_info.ip_addr, "192.168.1.39");
    strcpy(net_parm.ip_info.mask, "255.255.255.0");
    strcpy(net_parm.ip_info.gateway, "192.168.1.1");
    strcpy(net_parm.ip_info.dns1, "192.168.1.254");
    net_parm.http_port = 8000;
    net_parm.data_port = 554;
    strcpy(net_parm.ntp_server, "211.1.11.1");
    
    nvc_net_parm_set(&net_parm);

    //strcpy(net_parm.dst_id, "192.168.1.229:8000");
    //nvc_net_parm_get(&net_parm);
    #endif
    
    void *dev = nvc_dev_open(ip, port, "admin", "12345", 0);
    
    #if 0
    nvc_image_attr_t *img_parm = pmsg->data;    
    pmsg->st_ch = 0;
    nvc_img_attr_get(dev, pmsg);
    img_parm->brightness = 125;
    img_parm->contrast = 125;
    img_parm->saturation = 135;
    img_parm->hue = 125;
    img_parm->sharpness = 125;
    
    nvc_img_attr_set(dev, pmsg);
    memset(img_parm, 0, sizeof(nvc_image_attr_t));
    nvc_img_attr_get(dev, pmsg);
    #endif
    #if 0
    pmsg->ch = 0;
	pmsg->st_ch = 0;
	pmsg->size = sizeof(sdk_ptz_ctl_t);

    sdk_ptz_ctl_t *ptz_ctl = (sdk_ptz_ctl_t*)pmsg->data;

 /*   ptz_ctl->cmd = SDK_PTZ_MSG_RIGHT_START;
    ptz_ctl->speed = 58;
    printf("=========>>> will RIGHT start!\n");
    nvc_ptz_ctl(dev, pmsg);
    sleep(1);
    ptz_ctl->cmd = SDK_PTZ_MSG_RIGHT_STOP;
    printf("==========>>> will RIGHT stop!\n");
    nvc_ptz_ctl(dev, pmsg);

    ptz_ctl->cmd = SDK_PTZ_MSG_LEFT_START;
    ptz_ctl->speed = 68;
    printf("=========>>> will LEFT_UP start!\n");
    nvc_ptz_ctl(dev, pmsg);
    sleep(1);
    ptz_ctl->cmd = SDK_PTZ_MSG_LEFT_STOP;
    printf("==========>>> will LEFT_UP stop!\n");
    nvc_ptz_ctl(dev, pmsg);

    /*ptz_ctl->cmd = SDK_PTZ_MSG_ZOOM_ADD_START;
    ptz_ctl->speed = 60;
    printf("=========>>> will ZOOM_ADD start!\n");
    nvc_ptz_ctl(dev, pmsg);
    sleep(1);
    ptz_ctl->cmd = SDK_PTZ_MSG_ZOOM_ADD_STOP;
    printf("==========>>> will ZOOM_ADD stop!\n");
    nvc_ptz_ctl(dev, pmsg);
    
    strcpy(ptz_ctl->name, "PTZ_Name");
    ptz_ctl->cmd = SDK_PTZ_MSG_PRESET_SET;
    printf("=========>>> will PRESET_SET!\n");
    nvc_ptz_ctl(dev, pmsg);
    sleep(1);

    /*ptz_ctl->cmd = SDK_PTZ_MSG_ZOOM_SUB_START;
    ptz_ctl->speed = 60;
    printf("=========>>> will ZOOM_SUB start!\n");
    nvc_ptz_ctl(dev, pmsg);
    sleep(1);
    ptz_ctl->cmd = SDK_PTZ_MSG_ZOOM_SUB_STOP;
    printf("==========>>> will ZOOM_SUB stop!\n");
    nvc_ptz_ctl(dev, pmsg);
    */
    ptz_ctl->cmd = SDK_PTZ_MSG_RIGHT_START;
    ptz_ctl->speed = 68;
    printf("=========>>> will RIGHT_DOWN start!\n");
    nvc_ptz_ctl(dev, pmsg);
    sleep(5);
    ptz_ctl->cmd = SDK_PTZ_MSG_RIGHT_STOP;
    printf("==========>>> will RIGHT_DOWN stop!\n");
    nvc_ptz_ctl(dev, pmsg);
    #if 0
    ptz_ctl->cmd = SDK_PTZ_MSG_PRESET_CALL;
    ptz_ctl->speed = 68;
    printf("=========>>> will PRESET_CALL start!\n");
    nvc_ptz_ctl(dev, pmsg);

    sleep(1);
    strcpy(ptz_ctl->name, "PTZ_Name");
    ptz_ctl->cmd = SDK_PTZ_MSG_PRESET_DEL;
    printf("=========>>> will PRESET_SET!\n");
    nvc_ptz_ctl(dev, pmsg);
    #endif
    #endif

		//nvc_probe(0, 5, callback, NULL);
		
	#if 0

		//char *ip = "192.168.1.229";
        //int port = 8000;
		printf("======> ip: %s, port: %d\n", ip, port);
		void *dev = nvc_dev_open(ip, port, NULL, NULL, 0);

	//char _buf[1024];
	nvc_msg_t *pmsg2 = (nvc_msg_t *)_buf;
	
	pmsg2->ch = 0;
	pmsg2->st_ch = 0;
	pmsg2->size = sizeof(nvc_profile_enc_t);

	nvc_profile_enc_t *prof_enc = (nvc_profile_enc_t *)pmsg2->data;
	nvc_dev_profile_get(dev, pmsg2);
	printf("main: resolu: %d, v_enc: %d, a_enc: %d, frame_rate: %d\n"
		, prof_enc->resolution, prof_enc->video_enc, prof_enc->audio_enc, prof_enc->frame_rate);

	pmsg2->st_ch = 1;
	nvc_dev_profile_get(dev, pmsg2);
		printf("second: resolu: %d, v_enc: %d, a_enc: %d, frame_rate: %d\n"
		, prof_enc->resolution, prof_enc->video_enc, prof_enc->audio_enc, prof_enc->frame_rate);
	
	char buf[1024];
	nvc_msg_t * pmsg1 = (nvc_msg_t *)buf;
	pmsg1->ch = 2;
	pmsg1->st_ch = 0;
	sdk_media_url_t *url = (sdk_media_url_t *)pmsg1->data;
	url->proto = 0;
	url->trans = 3;

		char __buf[1024];
		nvc_msg_t * pmsg3 = (nvc_msg_t *)__buf;
		pmsg3->ch = 0;
		pmsg3->st_ch = 0;
		nvc_enc_parm_t *enc_param = (nvc_enc_parm_t *)pmsg3->data;
		nvc_enc_parm_get(dev, pmsg3);
		printf(">>> 111 >>> enc_type: %d, resolu: %d, quality: %d, fram_rate: %d, bitrate: %d, gop: %d, level: %d\n"
			, enc_param->main.video_enc
			, enc_param->main.resolution
			, enc_param->main.pic_quilty
			, enc_param->main.frame_rate
			, enc_param->main.bitrate
			, enc_param->main.gop
			, enc_param->main.level);

        enc_param->main.resolution = 15;
        nvc_enc_parm_set(dev, pmsg3);
         nvc_net_parm_set(&net_parm);
         printf("11111111111111111111111111111111111111\n");
        nvc_enc_parm_get(dev, pmsg3);
        printf(">>> 33333333 >>> enc_type: %d, resolu: %d, quality: %d, fram_rate: %d, bitrate: %d, gop: %d, level: %d\n"
			, enc_param->main.video_enc
			, enc_param->main.resolution
			, enc_param->main.pic_quilty
			, enc_param->main.frame_rate
			, enc_param->main.bitrate
			, enc_param->main.gop
			, enc_param->main.level);

		pmsg3->st_ch = 1;
		nvc_enc_parm_get(dev, pmsg3);
		printf(">>>> 2222 >>>enc_type: %d, resolu: %d, quality: %d, fram_rate: %d, bitrate: %d, gop: %d, level: %d\n"
			, enc_param->second.video_enc
			, enc_param->second.resolution
			, enc_param->second.pic_quilty
			, enc_param->second.frame_rate
			, enc_param->second.bitrate
			, enc_param->second.gop
			, enc_param->second.level);
	nvc_media_url_get(dev, pmsg1);
	printf("[%s] url: %s\n",__func__ ,url->url);
	nvc_dev_close(dev);
		nvc_uninit();
	#endif
#endif
	return 0;
}
#endif // TEST_ALL
