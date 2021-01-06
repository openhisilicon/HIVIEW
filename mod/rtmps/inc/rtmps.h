#ifndef __rtmps_h__
#define  __rtmps_h__


#ifdef __cplusplus
extern "C" {
#endif


#include "inc/gsf.h"

//for json cfg;
#include "mod/rtmps/inc/sjb_rtmps.h"


#define GSF_IPC_RTMPS "ipc:///tmp/rtmps_rep"

enum {
    GSF_ID_RTMPS_CFG    = 1,  // [gsf_rtmps_t, gsf_rtmps_t]
    GSF_ID_RTMPS_P_OPEN = 2,  // [gsf_rtmp_purl_t, nil]
    GSF_ID_RTMPS_P_CLOSE= 3,  // [gsf_rtmp_purl_t, nil]
    GSF_ID_RTMPS_P_LIST = 4,  // [nil, gsf_rtmp_push_t ... N]
    GSF_ID_RTMPS_P_CLEAR= 5,  // [nil, nil]
    
    GSF_ID_RTMPS_END
};

enum {
  GSF_RTMPS_ERR = -1,
};

typedef struct {
  int  ch, st;   // channle,stream;
  int  transp;
  char url[256]; // rtmp://admin:12345@192.168.1.60:554/1
}gsf_rtmp_purl_t;

typedef struct {
  gsf_rtmp_purl_t rtmp_purl;
  int            refcnt;
}gsf_rtmp_push_t;

#ifdef __cplusplus
}
#endif

#endif