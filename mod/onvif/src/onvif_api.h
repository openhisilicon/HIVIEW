#ifndef __onvif_api_h__
#define __onvif_api_h__

#include <stdint.h>

#include "onvif.h"

#define MAX_PRESET_NUM  512
#define ONVIF_PRESET_NAME_FMT  "PtzName_%d"


/* probe */
typedef struct nvc_probe_item_s {
    uint8_t  dst_id[32];/* 目标设备ID */
    uint8_t  ip[64];
    uint32_t type;	    /**Device_Type_E 0:Device 1:NetworkVideoTransmitter*/
    uint32_t ch_num;
    uint32_t port;
}nvc_probe_item_t;
typedef int (NVC_PROBER_CB)(nvc_probe_item_t *item, void *user_args);

/*------------------------------------------------------*/

int32_t nvc_init(void);
int32_t nvc_uninit(void);

/*------------------------------------------------------*/

int32_t nvc_probe(int type, int timeout, NVC_PROBER_CB *cb, void *user_args);

/*------------------------------------------------------*/

void *nvc_dev_open(char *url, int timeout /*EVENT_CB*/);
int32_t nvc_dev_close(void *dev);

typedef struct {
    uint8_t    trans;       /* 0: udp, 1: tcp, 2: rtsp 3: http*/
    uint8_t    proto;       /* 0: Unicast, 1: multicast */
    int8_t     url[256];    /* url */
}nvc_media_url_t;

int32_t nvc_media_url_get(void *dev, int ch, int st, nvc_media_url_t* media);

// NVC_PTZ_ID_E #include "onvif.h"
// nvc_ptz_ctl_t #include "onvif.h"
int32_t nvc_ptz_ctl(void *dev, int ch, int st, nvc_ptz_ctl_t *ctl);

/*
 * 预置点定义
 */
typedef struct {
    uint8_t enable;         //是否设置
    uint8_t no;             //预置点号
    uint8_t res[2];         //保留
    uint8_t name[32];       //预置点名称
}nvc_preset_t;
 
/*
 * 预置点集合(每个通道一个集合)
 */
typedef struct {
    uint8_t         num;                       //已设置的预置点个数
    uint8_t         res[3];                    //保留
    nvc_preset_t    preset_set[MAX_PRESET_NUM];//预置点
}nvc_preset_param_t;

// 查询预置位集合,以便于设置巡航 
int32_t nvc_ptz_presets_get(void *dev, int ch, int st, nvc_preset_param_t *preset);


typedef struct {
    uint8_t brightness;         //亮度
    uint8_t contrast;           //对比度
    uint8_t saturation;         //饱和度
    uint8_t hue;                //色度
    uint8_t sharpness;          //锐度
    uint8_t effect_mode;        //0:使用自定义值，1：使用默认值
    uint8_t res[2];
}nvc_image_attr_t;

int32_t nvc_img_attr_get(void *dev, int ch, int st, nvc_image_attr_t *attr);
int32_t nvc_img_attr_set(void *dev, int ch, int st, nvc_image_attr_t *attr);

int32_t nvc_snap_url_get(void *dev, int ch, int st, char *url);

typedef struct {
    uint32_t year;
    uint32_t mon;
    uint32_t day;
    uint32_t hour;
    uint32_t min;
    uint32_t sec;
}nvc_time_t; // == NVC_time_t;

//Time synchronization to the device
int32_t nvc_systime_set(void *dev, int ch, int st, nvc_time_t *time);

/*------------------------------------------------------*/

/*NVT参数定义*/
typedef struct nvt_parm_s {
  int ch_num;     //nvt设备通道数
  int auth;       //auth;
	uint16_t port;  //nvt监听端口
}nvt_parm_t;

typedef struct user_right_s {
    uint8_t user_name[32];
    uint8_t user_pwd[32];
    int32_t local_right;    //本地GUI权限
    uint32_t remote_right;  //远程权限
}user_right_t;


/* 初始化NVT */
int32_t nvt_init(nvt_parm_t *parm);
int32_t nvt_uninit(void);

/* 启动NVT服务 */
int32_t nvt_start(void);
int32_t nvt_stop(void);

/*------------------------------------------------------*/


#endif //__onvif_api_h__


