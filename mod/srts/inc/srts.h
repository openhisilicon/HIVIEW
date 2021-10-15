#ifndef __srts_h__
#define  __srts_h__


#ifdef __cplusplus
extern "C" {
#endif


#include "inc/gsf.h"

//for json cfg;
#include "mod/srts/inc/sjb_srts.h"


#define GSF_IPC_SRTS "ipc:///tmp/srts_rep"

enum {
    GSF_ID_SRTS_CFG    = 1,  // [gsf_srts_t, gsf_srts_t]
    
    GSF_ID_SRTS_END
};

enum {
  GSF_SRTS_ERR = -1,
};

#ifdef __cplusplus
}
#endif

#endif