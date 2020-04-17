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
    GSF_ID_ONVIF_C_PTZCTL  = 7,  //
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



#ifdef __cplusplus
}
#endif

#endif