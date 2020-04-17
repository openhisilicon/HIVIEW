#ifndef __rtsps_h__
#define  __rtsps_h__

#ifdef __cplusplus
extern "C" {
#endif

#include "inc/gsf.h"
#include "mod/codec/inc/codec.h"

//for json cfg;
#include "mod/rtsps/inc/sjb_rtsps.h"


#define GSF_IPC_RTSPS      "ipc:///tmp/rtsps_rep"

enum {
    GSF_ID_RTSPS_CFG    = 1,  // [gsf_rtsps_t, gsf_rtsps_t]
    GSF_ID_RTSPS_C_OPEN = 2,  // [gsf_rtsp_url_t, gsf_shmid_t]
    GSF_ID_RTSPS_C_CLOSE= 3,  // [gsf_rtsp_url_t, nil]
    GSF_ID_RTSPS_C_LIST = 4,  // [nil, gsf_rtsp_conn_t ... N]
    GSF_ID_RTSPS_C_CLEAR= 5,  // [nil, nil]
    GSF_ID_RTSPS_END
};

enum {
    GSF_RTSPS_ERR = -1,
};


typedef struct {
  int  transp;
  char url[256]; // rtsp://admin:12345@192.168.1.60:554/1
}gsf_rtsp_url_t;

typedef struct {
  gsf_rtsp_url_t rtsp_url;
  gsf_shmid_t    shmid;
  int            refcnt;
}gsf_rtsp_conn_t;


#ifdef __cplusplus
}
#endif

#endif