#ifndef _NVC_PLATFORM_H_
#define _NVC_PLATFROM_H_

#include "soapH.h"

#include "network.h"
#include "onvif_api.h"

/********** 宏定义 ******/

#ifndef ETH_NAME 
#define ETH_NAME "eth0" 
#endif
#ifndef MAX_IF_NAME_LEN 
#define MAX_IF_NAME_LEN 16
#endif
#ifndef MAX_IP_ADDR_LEN 
#define MAX_IP_ADDR_LEN 16
#endif
#ifndef MAX_MAC_ADDR_LEN 
#define MAX_MAC_ADDR_LEN 32
#endif
#ifndef MAX_PRESET_NUM  
#define MAX_PRESET_NUM  512
#endif 

#define MACH_ADDR_LENGTH 	  6
#define INFO_LENGTH         256
#define LARGE_INFO_LENGTH   1024
#define SMALL_INFO_LENGTH   128

#define MAX_NAME_LEN 64
#define MAX_PASSWD_LEN 32

#define MAX_URL_LEN 256
#define MAX_IP_LEN 64

#define MAX_CH_NUM 32

#define MAX_TYPE_LEN 16

#define MAX_TOKEN_LEN 64

/********** enum *********/
typedef enum _capabilities_type  
{
	All = 0,
	Analytics,
	Device,
	Events,
	Imaging,
	Media,
	PTZ,
}capabilities_type;

typedef enum _stream_type_protocol
{
	Unicast = 0,
	multicast = 1
}stream_type_protocol;

typedef enum _stream_type
{
	main_stream = 0,
	second_stream
}stream_type;

typedef enum _transport_protocol
{
	udp = 0,
	tcp,
	rtsp,
	http
}transport_protocol;

typedef enum _Video_Type
{
	  JPEG = 0
	, MPEG4
	, H264
}Video_Type;

typedef struct video_resolution_s
{
	uint16_t width;
	uint16_t height;
	
}video_resolution_t;

typedef struct Video_Enc_s
{
	uint16_t enc_type;        /* 视频编码格式0--h264 1--MJPEG 2--JPEG	*/
	video_resolution_t resolution;
	
    uint16_t frame_rate;      /*帧率*/
    uint16_t bitrate;         /*码率*/
	uint8_t pic_quilty;     //编码质量0-最好，1-次好，2-较好，3-一般，4-较差，5-差	
	uint8_t gop;			//I 帧间隔1-200
	uint8_t level;          //编码等级，h264: 0--baseline, 1--main, 2--high;
	uint8_t res[1];
}Video_Enc_t;

#ifndef _boolean_e
#define _boolean_e
enum _boolean_ {__false = 0, __true = 1};
#endif

typedef enum _Audio_Type
{
	 G711 = 0
	,G726
	,AAC
}Audio_Type;
typedef struct Audio_Enc_s
{
	uint16_t enc_type;        /*编码类型*/
}Audio_Enc_t;

typedef struct media_info_s
{
	Audio_Enc_t audio;
	Video_Enc_t video;
}media_info_t;

typedef struct Profile_info_s
{
	char profile_token[MAX_TOKEN_LEN];
	//char profile_name[MAX_TOKEN_LEN];

	char v_src_token[MAX_TOKEN_LEN];	/**Video source configuration*/
	char v_src_name[MAX_TOKEN_LEN];
	char v_src_sourcetoken[MAX_TOKEN_LEN];
	
	char a_src_token[MAX_TOKEN_LEN];	/**Audio source configuration*/
	char a_src_name[MAX_TOKEN_LEN];
	char a_src_sourcetoken[MAX_TOKEN_LEN];

	char v_enc_token[MAX_TOKEN_LEN];	/**Video encoder configuration*/
	char v_enc_name[MAX_TOKEN_LEN];
	
	#if 1
	uint16_t width;				/** Resolution */
	uint16_t height;				/** Resolution */
	uint8_t a_enc;			/** audio enc type */
	uint8_t v_enc;			/** video enc type */
	uint8_t frame_rate;  /** 帧率 */
	uint8_t res[1];		/**  保留字节,对齐*/
	#endif
	char a_enc_token[MAX_TOKEN_LEN];	/**Audio encoder configuration*/
	char a_enc_name[MAX_TOKEN_LEN];

	char v_ana_token[MAX_TOKEN_LEN];	/**Video analytics*/
	char v_ana_name[MAX_TOKEN_LEN];

	char PTZ_parm_token[MAX_TOKEN_LEN];	/**PTZ configuration*/
	//char PTZ_parm_name[MAX_TOKEN_LEN];
	char PTZ_parm_nodetoken[MAX_TOKEN_LEN];

	char m_parm_token[MAX_TOKEN_LEN];	/**Metadata configuration*/
	char m_parm_name[MAX_TOKEN_LEN];

	char ao_parm_token[MAX_TOKEN_LEN];	/**Audio Output configuration*/
	char ao_parm_name[MAX_TOKEN_LEN];
	char ao_parm_outtoken[MAX_TOKEN_LEN];

	char a_dec_token[MAX_TOKEN_LEN];	/**Audiio decoder configuration*/
	char a_dec_name[MAX_TOKEN_LEN];
}Profile_info_t;

/*********************************************/
typedef struct MVC_Probe_s
{
	uint8_t  ip[MAX_IP_LEN];
	uint32_t port;
  uint32_t type;	/**NVC_Device_Type_E*/
  uint32_t ch_num;
}NVC_Probe_t;


/*************************** URL ***********************************/
typedef struct NVC_Stream_Uri_s
{
	stream_type_protocol protocol;
	transport_protocol trans_protocol;
	char url[MAX_URL_LEN];
	int res[2];
}NVC_Stream_Uri_t;

/************************** capabilities ****************************/
typedef struct Analytics_capa_s
{
	char url[MAX_URL_LEN];
	
}Analytics_capa_t;

typedef struct Device_capa_s
{
	char url[MAX_URL_LEN];
	
}Device_capa_t;

typedef struct Events_capa_s
{
	char url[MAX_URL_LEN];
	
}Events_capa_t;

typedef struct Img_capa_s
{
	char url[MAX_URL_LEN];
	
}Img_capa_t;

typedef struct Media_capa_s
{
	char url[MAX_URL_LEN];
	
}Media_capa_t;

typedef struct PTZ_capa_s
{
	char url[MAX_URL_LEN];
	
}PTZ_capa_t;

typedef struct NVC_capa_s
{
	capabilities_type capa_type;
	
	Analytics_capa_t *ana_capa;
	Device_capa_t *dev_capa;
	Events_capa_t *env_capa;
	Img_capa_t *img_capa;
	Media_capa_t *media_capa;
	PTZ_capa_t *PTZ_capa;
}NVC_capa_t;

/*********************** PTZ ************************/
typedef struct NVC_PTZ_Configution_s
{
    char ContinuousPanTiltSpace[LARGE_INFO_LENGTH]; /** PTZ Control: up, down, right, left will be used*/
    char ContinuousZoomSpace[LARGE_INFO_LENGTH]; /** PTZ Control: zoom add and subtract will be used*/
}NVC_PTZ_Configution_t; /**次结构体暂时先定义这两个用到的，后续如果PTZ有用到，可以在加进去*/

typedef struct NVC_PanTile_s
{
	float x;
	float y;
}NVC_PanTilt_t;

typedef struct NVC_Zoom_S
{
	float x;
}NVC_Zoom_t;

typedef struct NVC_PTZ_Speed_s
{
	NVC_PanTilt_t pantilt;
	NVC_Zoom_t  zoom;
}NVC_PTZ_Speed_t;

typedef struct NVC_PTZ_Stop_s
{
    enum _boolean_ en_pant;
    enum _boolean_ en_zoom;
}NVC_PTZ_Stop_t;

typedef struct PTZ_Preset_s
{
    char name[MAX_NAME_LEN];
    char token[MAX_TOKEN_LEN];
    NVC_PTZ_Speed_t speed;
    int  staytime;
}PTZ_Preset_t;

typedef struct NVC_PTZ_Preset_s
{
    int _size;
    PTZ_Preset_t _preset[MAX_PRESET_NUM];
    char ptz_tour_token[MAX_TOKEN_LEN];
}NVC_PTZ_Preset_t;

/********************** network info ***********************/
typedef struct NVC_IP_info_s
{
    uint8_t if_name[MAX_IF_NAME_LEN];
    uint8_t ip_addr[MAX_IP_ADDR_LEN];
    uint8_t mac[MAX_MAC_ADDR_LEN];
    
    uint8_t mask[MAX_IP_ADDR_LEN];
    uint8_t gateway[MAX_IP_ADDR_LEN];
    
    uint8_t dns1[MAX_IP_ADDR_LEN];
    uint8_t dns2[MAX_IP_ADDR_LEN];

    uint8_t dhcp_enable;
    uint8_t dns_auto_en;
    uint8_t res[2];
}NVC_IP_info_t;
typedef struct NVC_Net_info_s
{
    uint8_t dst_id[MAX_IP_ADDR_LEN];
    NVC_IP_info_t ip_info;
    uint8_t ntp_server[MAX_IP_ADDR_LEN];
    uint8_t res_server[MAX_IP_ADDR_LEN];//保留
    uint16_t http_port;         //web端口
    uint16_t data_port;         //媒体端口
    uint16_t cmd_port;          //probe 探测到的端口
    uint16_t res_port[1];          //保留
}NVC_Net_info_t;


typedef struct NVC_time_s{
    uint32_t year;
    uint32_t mon;
    uint32_t day;
    uint32_t hour;
    uint32_t min;
    uint32_t sec;
    uint32_t zone;
}NVC_time_t;


/******************* Device handle *******************/
typedef struct NVC_Dev_s
{
	char ip[MAX_IP_LEN];
	uint16_t port;
	uint16_t ch_num;
	char user[MAX_NAME_LEN];
	char passwd[MAX_PASSWD_LEN];

	uint32_t st_type;	/**码流类型*/
	Profile_info_t *profile_tn;

	NVC_capa_t *capa;

  NVC_PTZ_Configution_t *ptz_conf;
  NVC_PTZ_Preset_t *Preset;
    
	int timeout;
  int need_verify;
  
  int get_tm;
  
}NVC_Dev_t;

/*************** img_attr ****************/
typedef struct NVC_Img_attr_s
{
    uint8_t max_brihtness;
    uint8_t min_brihtness;
    uint8_t brightness;         //亮度
    
    uint8_t max_contrast;
    uint8_t min_contrast;
    uint8_t contrast;           //对比度
    
    uint8_t max_saturation;
    uint8_t min_saturation;
    uint8_t saturation;         //饱和度

    uint8_t max_hue;
    uint8_t min_hue;
    uint8_t hue;                //色度

    uint8_t max_sharpness;
    uint8_t min_sharpness;
    uint8_t sharpness;          //锐度
    
    uint8_t res[1];
}NVC_Img_attr_t;

typedef struct NVC_Snapshot_s
{
    char url[MAX_URL_LEN];
}NVC_Snapshot_t;


/*********** func *******/
//static SOAP  creat_soap_header(const char *was_To, const char *was_Action);
//typedef int (*call_probe_func)(NVC_Probe_t *probe);

//int NVC_discovery(NVC_Probe_t *probe, int timeout);
int NVC_discovery(NVC_Probe_t *probe, int timeout, NVC_PROBER_CB *cb, void *user_args);

int NVC_Get_Capabilities(NVC_Dev_t *dev);//, NVC_capa_t *capa);
int NVC_Get_profiles(NVC_Dev_t *dev);

int NVC_GetStreamUri(NVC_Dev_t *dev, NVC_Stream_Uri_t *nvc_stream_url);

int NVC_Get_Audio_info(NVC_Dev_t *dev, Audio_Enc_t *audio);
int NVC_Set_Audio_info(NVC_Dev_t *dev, Audio_Enc_t *audio);

int NVC_Get_Video_info(NVC_Dev_t *dev, Video_Enc_t *video);
int NVC_Set_Video_info(NVC_Dev_t *dev, Video_Enc_t *video);

int NVC_Get_Img_options(NVC_Dev_t *dev, NVC_Img_attr_t *img_attr);
int NVC_Get_Img_attr_info(NVC_Dev_t *dev, NVC_Img_attr_t *img_attr);
int NVC_Set_Img_attr_info(NVC_Dev_t *dev, NVC_Img_attr_t *img_attr);

int NVC_PTZ_Get_Configurations(NVC_Dev_t * dev, NVC_PTZ_Configution_t * ptz_conf);
int NVC_PTZ_ContinusMove(NVC_Dev_t *dev, NVC_PTZ_Speed_t *ptz_speed);
int NVC_PTZ_Stop(NVC_Dev_t *dev, NVC_PTZ_Stop_t *ptz_stop);

int NVC_PTZ_Get_Presets(NVC_Dev_t * dev, NVC_PTZ_Preset_t * preset);
int NVC_PTZ_Preset_set(NVC_Dev_t *dev, PTZ_Preset_t *preset);
int NVC_PTZ_Preset_del(NVC_Dev_t *dev, PTZ_Preset_t *preset);
int NVC_PTZ_Preset_call(NVC_Dev_t *dev, PTZ_Preset_t *preset);
int NVC_PTZ_Preset_tour(NVC_Dev_t *dev, NVC_PTZ_Preset_t *preset);
int NVC_Set_SystemTime(NVC_Dev_t *dev, NVC_time_t *time);
int NVC_Get_SnapshotUri(NVC_Dev_t *dev, NVC_Snapshot_t *snap);
int NVC_Get_SystemTime(NVC_Dev_t *dev, NVC_time_t *time);
///
extern int nvc_snap_req(char *uri, int chs, int streamtype, void *msg);



#endif

