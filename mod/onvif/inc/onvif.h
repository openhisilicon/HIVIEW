#ifndef __onvif_h__
#define  __onvif_h__

#ifdef __cplusplus
extern "C" {
#endif

#include "inc/gsf.h"

//for json cfg;
#include "mod/onvif/inc/sjb_onvif.h"

#define GSF_IPC_ONVIF      "ipc:///tmp/onvif_rep"

enum {
    GSF_ID_ONVIF_CFG       = 1,  // [gsf_onvif_cfg_t, gsf_onvif_cfg_t]
    GSF_ID_ONVIF_C_PROBE   = 2,  // [gsf_onvif_probe_t, gsf_onvif_probe_item_t ... N]
    GSF_ID_ONVIF_C_OPEN    = 3,  // [gsf_onvif_url_t, gsf_onvif_media_url_t]
    GSF_ID_ONVIF_C_CLOSE   = 4,  // [gsf_onvif_url_t, nil]
    GSF_ID_ONVIF_C_LIST    = 5,  // [nil, gsf_onvif_conn_t ... N]
    GSF_ID_ONVIF_C_CLEAR   = 6,  // [nil, nil]
    GSF_ID_ONVIF_C_PTZCTL  = 7,  // [gsf_onvif_ptz_ctl_t, nil]
    GSF_ID_ONVIF_C_PRESET  = 8,  // 
    GSF_ID_ONVIF_C_SETTIME = 9,  //
    GSF_ID_ONVIF_C_IMGATTR = 10, //
    GSF_ID_ONVIF_END
};

enum {
    GSF_ONVIF_ERR = -1,
};

typedef struct {
  int type;   /**Device_Type_E 0:Device 1:NetworkVideoTransmitter*/
  int timeout;
}gsf_onvif_probe_t;

typedef struct {
  char  dst_id[32];
  char  ip[64];
  int   type;	 /**Device_Type_E 0:Device 1:NetworkVideoTransmitter*/
  int   ch_num;
  int   port;
}gsf_onvif_probe_item_t;

typedef struct {
  char url[256]; // onvif://admin:12345@192.168.1.60:8000
  int  timeout;
}gsf_onvif_url_t;

typedef struct {
  char st1[256]; // rtsp://admin:12345@192.168.1.60:554/1
  char st2[256]; // rtsp://admin:12345@192.168.1.60:554/2
  char snap[256];// http://admin:12345@192.168.1.60:80/snap.jpg
}gsf_onvif_media_url_t;

typedef struct {
  int refcnt;
  gsf_onvif_url_t onvif_url;
  gsf_onvif_media_url_t media_url;
}gsf_onvif_conn_t;

/*
 * 云台控制消息ID定义
 */
typedef enum {
    NVC_PTZ_UP_START = 0x001
  , NVC_PTZ_UP_STOP
  , NVC_PTZ_DOWN_START
  , NVC_PTZ_DOWN_STOP
  , NVC_PTZ_LEFT_START
  , NVC_PTZ_LEFT_STOP
  , NVC_PTZ_RIGHT_START    
  , NVC_PTZ_RIGHT_STOP
  , NVC_PTZ_LEFT_UP_START
  , NVC_PTZ_LEFT_UP_STOP
  , NVC_PTZ_RIGHT_UP_START
  , NVC_PTZ_RIGHT_UP_STOP
  , NVC_PTZ_LEFT_DOWN_START
  , NVC_PTZ_LEFT_DOWN_STOP
  , NVC_PTZ_RIGHT_DOWN_START
  , NVC_PTZ_RITHT_DOWN_STOP
  , NVC_PTZ_PRESET_SET
  , NVC_PTZ_PRESET_CALL
  , NVC_PTZ_PRESET_DEL
  , NVC_PTZ_ZOOM_ADD_START
  , NVC_PTZ_ZOOM_ADD_STOP
  , NVC_PTZ_ZOOM_SUB_START
  , NVC_PTZ_ZOOM_SUB_STOP
  , NVC_PTZ_FOCUS_ADD_START
  , NVC_PTZ_FOCUS_ADD_STOP
  , NVC_PTZ_FOCUS_SUB_START
  , NVC_PTZ_FOCUS_SUB_STOP
  , NVC_PTZ_IRIS_ADD_START
  , NVC_PTZ_IRIS_ADD_STOP
  , NVC_PTZ_IRIS_SUB_START
  , NVC_PTZ_IRIS_SUB_STOP
  , NVC_PTZ_GOTO_ZERO_PAN
  , NVC_PTZ_FLIP_180
  , NVC_PTZ_SET_PATTERN_START
  , NVC_PTZ_SET_PATTERN_STOP
  , NVC_PTZ_RUN_PATTERN
  , NVC_PTZ_SET_AUXILIARY
  , NVC_PTZ_CLEAR_AUXILIARY
  , NVC_PTZ_AUTO_SCAN_START
  , NVC_PTZ_AUTO_SCAN_STOP
  , NVC_PTZ_RANDOM_SCAN_START
  , NVC_PTZ_RANDOM_SCAN_STOP
  , NVC_PTZ_LEFT_OFFSET
  , NVC_PTZ_RIGHT_OFFSET
  , NVC_PTZ_DOWN_OFFSET
  , NVC_PTZ_UP_OFFSET
  , NVC_PTZ_ZOOM_MULTIPLE
  , NVC_PTZ_POINT_CENTER
  , NVC_PTZ_VIEW_CENTER
  , NVC_PTZ_BUTT
}NVC_PTZ_ID_E;

typedef struct {
    unsigned char cmd;     /* NVC_PTZ_ID_E */
    unsigned char speed;   /* 1 - 100 */
    unsigned char res;     /* res */
    unsigned char val;     /* preset, track ...  */
    unsigned char name[32];/* preset, track name */
}nvc_ptz_ctl_t;

typedef struct {
  char url[256]; // onvif://admin:12345@192.168.1.60:8000
  nvc_ptz_ctl_t ctl; // nvc_ptz_ctl_t
}gsf_onvif_ptz_ctl_t;


#ifdef __cplusplus
}
#endif

#endif