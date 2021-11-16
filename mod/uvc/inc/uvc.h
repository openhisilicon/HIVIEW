#ifndef __uvc_h__
#define  __uvc_h__


#ifdef __cplusplus
extern "C" {
#endif


#include "inc/gsf.h"

//for json cfg;
#include "mod/uvc/inc/sjb_uvc.h"


#define GSF_IPC_UVC "ipc:///tmp/uvc_rep"

enum {
    GSF_ID_UVC_CFG    = 1,  // [gsf_uvc_t, gsf_uvc_t]
        
    GSF_ID_UVC_END
};

enum {
  GSF_UVC_ERR = -1,
};


#ifdef __cplusplus
}
#endif

#endif