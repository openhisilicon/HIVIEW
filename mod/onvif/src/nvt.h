#ifndef __nvt_h__
#define __nvt_h__

#include <pthread.h>
#include "soapH.h"

#include "network.h"
#include "onvif_api.h"

#define DEFAULT_SCOPE   "onvif://www.onvif.org"

#ifndef ETH_NAME
#define ETH_NAME "eth0"
#endif

#define KEY (1492)
#define NUM_BROADCAST (4)        //number of times hello is broadcasted

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

#ifndef MAX_PRESET_NUM  
#define MAX_PRESET_NUM  512
#endif 


#ifndef _boolean_e
#define _boolean_e
enum _boolean_ {__false = 0, __true = 1};
#endif

#define MAX_THR (2)     // Size of thread pool
#define MAX_QUEUE (1000)// Max size of request queue

#define INFO_LEN        128
#define MAX_ENCODE      4
#define MAX_PROF        4
#define MIN_FRAMERATE   8
#define MAX_FRAMERATE   60
#define MIN_GOVLEN      1
#define MAX_GOVLEN      100
#define MIN_INTERVAL    0
#define MAX_INTERVAL    3
#ifdef  HICHIP_HI3535	
#define MAX_PROFILES    72
#else
#define MAX_PROFILES    64
#endif

#define ALL_CHS         128
#define MAX_BUF_LEN     1024*10
#define MAX_LOG_LEN     1024*128
#define REBOOT_SEC      15

#define MAX_RECORDING   200
#define VSC 0x001L
#define ASC 0x002L
#define VEC 0x004L
#define AEC 0x008L
//List of categories to retrieve capability information on.
#define ALL             0
#define ANALYTICS       1
#define DEVICE          2
#define EVENTS          3
#define IMAGING         4
#define MEDIA           5
#define PTZCAP          6
//End list.

#define MAIN_STREAM     0
#define SUB_STREAM      1

#define FALSE           0
#define TRUE            1

#define MAJOR           80/* major version number */
#define MINOR           81/* minor version number */

typedef enum{
      Manual = 0
    , NTP
}DateTimeType;

typedef struct{
    int     hour;
    int     minute;
    int     second;
    int     year;
    int     month;
    int     day;
}DateTime;

typedef struct systime_s{
    DateTimeType    datetype;
    int             daylightsaving; //0-->on   1-->off
    char            TZ[16];
    DateTime        UTCDateTime;
    DateTime        LocalDateTime;
}systime_t;

typedef struct imaging_conf_s{
    float   brightness;
    float   ColorSaturation;
    float   contrast;
    int     IrCtFilter; //Infrared Cutoff Filter settings.  0:ON 1:OFF 2:AUTO
    float   sharpness;
    /*not support!
    int              WideMode;
    float            WideLevel;
    int              WhiteBalanceMode;
    float            WhiteBalanceCrGain;
    float            WhiteBalanceCbGain;
    int              ExposureMode;    //0:AUTO  1:MANUAL
    int              priority;  //0:LowNoise 1:FrameRate
    exposure_window  window;
    float            MinExposureTime;    //Minimum value of exposure time range allowed to be used by the algorithm.
    float            MaxExposureTime;
    float            MinGain;      //Minimum value of the sensor gain range that is allowed to be used by the algorithm.
    float            MaxGain;
    float            MinIris;    //Minimum value of the iris range allowed to be used by the algorithm.
    float            MaxIris;
    float            ExposureTime; //The fixed exposure time used by the image sensor (μs).
    float            gain;   //The fixed gain used by the image sensor (dB).
    float            iris;      //The fixed attenuation of input light affected by the iris (dB). 0dB maps to a fully opened iris.

    int              BacklightMode;      // 0:OFF 1:ON
    float            BacklightLevel;

    //focus configuration
    int              AutoFocusMode; //0:AUTO  1:MANUAL
    float            DefaultSpeed;
    float            NearLimit;//Parameter to set autofocus near limit (unit: meter).
    float            FarLimit;*/
}imaging_conf_t;

typedef struct video_source_conf_s{
    char            vname[INFO_LEN];
    int             vcount;
    char            vtoken[INFO_LEN];
    char            vsourcetoken[INFO_LEN];
    int             x;
    int             y;
    int             width;
    int             heigth;
    char            uri[INFO_LEN];
}video_source_conf_t;

typedef struct audio_source_conf_s{
    char        aname[INFO_LEN];
    int         count;
    char        token[INFO_LEN];
    char        sourcetoken[INFO_LEN];
}audio_source_conf_t;

typedef enum{
     SP = 0
    ,ASP
}Mpeg4Profile;

typedef enum{
      Baseline = 0
    , Main
    , Extended
    , High
}H264Profile;

typedef enum {
      _JPEG   = 0
    , _MPEG4
    , _H264
}video_encoding;

typedef struct{
    int     type;       // ip type IPv4 or IPv6
    char    ipv4Addr[32];
    int     port;
    int     ttl;
    int     autostart;
}MulticastConf;

typedef struct video_encoder_conf_s{
    char            name[INFO_LEN];
    int             usecount;
    char            token[INFO_LEN];
    video_encoding  encoding;
    int             width;
    int             height;
    float           quality;
    /*video rate control*/
    int             FrameRateLimit;
    int             EncodingInterval;
    int             BitrateLimit;
    /*Mpeg4 configuration*/
    int             GovLength;
    Mpeg4Profile    mProfile;
    /*H264 configuration*/
    int             gLength;            //group of video frames length
    H264Profile     hProfile;
    MulticastConf   MultiConf;
    long            SessionTimeout;
}video_encoder_conf_t;

typedef enum{
      _G711 = 0
    , _G726
    , _AAC
} AudioEncoding;

typedef struct audio_encoder_conf_s{
    char            name[INFO_LEN];
    int             usecount;
    char            token[INFO_LEN];
    AudioEncoding   encoding;
    int             bitrate;
    int             samplerate;
    MulticastConf   MultiConf;
    long            SessionTimeout;
}audio_encoder_conf_t;

typedef struct{
    float       x;
    float       y;
    char        *space;
}PanTilt;

typedef struct{
    float       x;
    char        *space;
}zoom;

typedef struct{
    PanTilt     pantilts;   //pan and tilt speed
    zoom        zooms;      // zoom speed
}DefPTZSpeed;

typedef struct{
    float       Min;
    float       Max;
}Range;

typedef struct{
    char        *uri;
    Range       x;
    Range       y;
}PanTiltLimits;

typedef struct{
    char        *uri;
    Range       x;
}ZoomLimits;

typedef enum{
      OFF = 0
    , ON
    , Extended0
}EMode;

typedef enum{
      OFF1 = 0
    , ON1
    , AUTO1
    , Extended1
}RMode;

typedef struct{
    EMode       eflip;
    RMode       revs;
    char        *extension;
}PTControlDirection;

typedef struct{
    PTControlDirection  PTConDirect;
    char                *extn;
}PTZextension;

typedef struct PTZ_conf_s{
    char            name[INFO_LEN];
    int             usecount;
    char            token[INFO_LEN];
    char            nodetoken[INFO_LEN];
    char            *DefAbsolutePanTiltPositionSpace;
    char            *DefAbsoluteZoomPositionSpace;
    char            *DefRelativePanTiltTranslationSpace;
    char            *DefRelativeZoomTranslationSpace;
    char            *DefContinuousPanTiltVelocitySpace;
    char            *DefContinuousZoomVelocitySpace;
    DefPTZSpeed     PTZSpeed;
    long            defPTZtimeout;
    PanTiltLimits   panlimit;
    ZoomLimits      zoomlimit;
    PTZextension    PTZext;
}PTZ_conf_t;

typedef struct metadata_conf_s{
    char            name[INFO_LEN];
    int             usecount;
    char            token[INFO_LEN];
    int             status;
    int             position;
    int             analytics;
    MulticastConf   multiconf;
    long            sessionTimeout;
    char            *extension;
}metadata_conf_t;

typedef struct onvif_profile_s{
    char                     pname[INFO_LEN];
    video_source_conf_t     *vsc;
    audio_source_conf_t     *asc;
    video_encoder_conf_t    *vec;
    audio_encoder_conf_t    *aec;
    PTZ_conf_t              *PTZc;
    metadata_conf_t         *metac;
    imaging_conf_t          *img;
    char                    ptoken[INFO_LEN];
    int                     pfixed;
}onvif_profile_t;

typedef struct onvif_scopes_s{
    char type[100];
    char name[50];
    char location[100];
    char hardware[50];
    char ex_scopes[100];
}onvif_scopes_t;

typedef enum{
      HTTP = 0
    , HTTPS
    , RTPS
}NetworkProtocols;

typedef struct network_protocol_s{
    NetworkProtocols    name;
    int                 enabled;
    int                 port;
    int                 nport;
}network_protocol_t;

typedef struct{
    char    *xaddr;
    int     Nipfilter;
    int     NdynDNS;
    int     Nntp;
    int     Sdiscoverybye;
    int     Sfirmwareupgrade;
    int     Shttplogging;
    int     Shttpinfo;
}device_cap;

#if 0
typedef struct{
    char    *xaddr;
    int     rulesupport;
    int     amodulesupport;
}analytics_cap;

typedef struct{
    char    *xaddr;
    int     wspolicy;
    int     wspoint;
    int     wsmanager;
}events_cap;

typedef struct{
    char    *xaddr;
}imaging_cap;

typedef struct{
    char    *xaddr;
    int     rtpmulticast;
    int     rtptcp;
}media_cap;

typedef struct{
    char    *xaddr;
}PTZ_cap;
#endif

typedef struct{
    int     encodeformat;
    int     reswidth[MAX_ENCODE][32];//0:H264 1:JPEG 2:MJPEG 3:MPEG4
    int     resheight[MAX_ENCODE][32];
    int     minframe[MAX_ENCODE];
    int     maxframe[MAX_ENCODE];
    int     mininterval[MAX_ENCODE];
    int     maxinterval[MAX_ENCODE];
    int     mingovlen[MAX_ENCODE];
    int     maxgovlen[MAX_ENCODE];
    int     profileslevelnums;
    int     profileslevelvalue[4];
}videostream_cap;

typedef struct{
    int     encodeformat;
}audiostream_cap;

typedef struct net_info_s{
    uint8_t if_name[32];
    uint8_t ip_addr[32];
    uint8_t mask[32];
    uint8_t gateway[32];
    uint8_t mac[32];
    uint8_t dns1[32];
    uint8_t dns2[32];
    uint8_t dhcp_status;
    uint8_t dns_auto;    
}net_info_t;

typedef struct port_info_s{
    int     http_port;
    int     rtsp_port;
    int     http_enable;
    int     rtsp_enable;
}port_info_t;

typedef struct device_capability_s{
    int               chnums;
    int               streamnums;
    videostream_cap   videostream[MAX_PROFILES];
    audiostream_cap   audiostream[MAX_PROFILES];
}device_capability_t;

typedef struct device_info_s{
    char        sw[INFO_LEN];
    uint32_t    devtype;
    char        hwid[INFO_LEN];
    char        serial[INFO_LEN];
}device_info_t;

typedef struct recording_info_s{
    time_t  start_time;
    time_t  stop_time;
    char    token[INFO_LEN];
}recording_info_t;

typedef struct user_info_s{
    int  size;
    char buf[MAX_BUF_LEN];
}user_info_t;


typedef struct nvt_time_s{
    uint32_t year;
    uint32_t mon;
    uint32_t day;
    uint32_t hour;
    uint32_t min;
    uint32_t sec;
}nvt_time_t;



typedef struct onvif_s{
    unsigned int            discovery_mode;
    onvif_profile_t         *profiles;
    int                     nprofiles;
    //onvif_scopes_t      *scopes;
    device_capability_t     *device_cap;
    device_info_t           *dev;
    recording_info_t        record[16][MAX_RECORDING];
    int                     nrecordings;
}onvif_t;

typedef struct nvt_s{
    onvif_t         t;
    int             thread_discovery_flag;
    int             thread_serve_flag;
    pthread_t       discovery_id;
    int             discovery_sock;
    int             serve_sock;
    pthread_t       serve_id;
    pthread_mutex_t queue_cs;
    pthread_cond_t  queue_cv;
#if 0
    int             head;
    int             tail;
    int             queue[MAX_QUEUE];
#endif
    nvt_parm_t      p;
}nvt_t;


/* NVT 接口 */
void* start_nvt_discovery(void* parm);
void stop_nvt_discovery(void);

void* start_nvt_serve(void *parm);
void stop_nvt_serve(void);

/* NVT 上层实现，获取设备配置 */
extern nvt_t *g_nvt;
extern int get_dev_info(void);
extern int get_dev_profiles(int chs, onvif_profile_t *profile, int num, int flag);//get all profiles
extern int get_vsc_profiles(int chs, video_source_conf_t *info, int num);//video source configure
extern int get_asc_profiles(int chs, audio_source_conf_t *info, int num);//audio source configure
extern int get_vec_profiles(int chs, video_encoder_conf_t *info, int num);//video encoder configure
extern int get_aec_profiles(int chs, audio_encoder_conf_t *info, int num);//audio encoder configure
extern int get_meta_profiles(int chs, metadata_conf_t *info, int num);//metadata configure
extern int get_ptz_profiles(int chs, PTZ_conf_t *info, int num);//PTZ configure profiles
extern int get_dev_capability(device_capability_t *dev_cap, int streamtype);
extern int set_vsc_conf(int chs, video_source_conf_t *info);
extern int set_vec_conf(int chs, video_encoder_conf_t *info);
extern int set_aec_conf(int chs, audio_encoder_conf_t *info);
extern int set_imaging_conf(int chs, imaging_conf_t *img);
extern int get_recording_list(nvt_time_t starttime, nvt_time_t endtime);
extern int get_dev_time(systime_t *time);
extern int set_dev_time(systime_t *time);
extern int get_ntp_info(char *ntp1, char *ntp2);
extern int set_ntp_info(char *ntp1, char *ntp2);
extern int get_dev_netinfo(net_info_t *info);
extern int set_dev_netinfo(net_info_t *info);
extern int get_user_info(user_info_t *info);
extern int add_user_info(char *name, char *passwd);
extern int del_user_info(char *name);
extern int set_user_info(char *name, char *passwd);
extern int get_device_syslog(int type, char *syslog);
extern int device_reboot();

extern int get_dev_portinfo(port_info_t *info);
extern int set_dev_portinfo(port_info_t *info);
extern int get_curr_chs();
extern int get_imaging_conf(int chs, imaging_conf_t *img, int num);
extern int get_recording_info(recording_info_t *info, char *token);
extern int set_device_factorydefault(int mask);

extern int ptz_ctl(int chs, int action, int speed);


#endif


