#ifndef __srts_h__
#define  __srts_h__


#ifdef __cplusplus
extern "C" {
#endif


#include "inc/gsf.h"
#include "mod/codec/inc/codec.h"

//for json cfg;
#include "mod/srts/inc/sjb_srts.h"


#define GSF_IPC_SRTS "ipc:///tmp/srts_rep"

enum {
    GSF_ID_SRTS_CFG    = 1,  // [gsf_srts_t, gsf_srts_t]
    GSF_ID_SRTS_CTL    = 2,  // [gsf_srts_ctl_t]
    GSF_ID_SRTS_RECV   = 3,  // [gsf_srts_ctl_t, gsf_shmid_t]
    GSF_ID_SRTS_END
};

enum {
  GSF_SRTS_ERR = -1,
};


typedef struct {
  int ch, st;   //channel, stream
  char dst[256];// udp://226.0.0.22:7980
}gsf_srts_ctl_t;


#ifdef __cplusplus
}
#endif

#endif