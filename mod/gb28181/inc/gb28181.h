#ifndef  __gb28181_h__
#define  __gb28181_h__

#ifdef __cplusplus
extern "C" {
#endif

#include "inc/gsf.h"

//for json cfg;
#include "mod/gb28181/inc/sjb_gb28181.h"


#define GSF_IPC_GB28181      "ipc:///tmp/gb28181_rep"

enum {
    //GSF_ID_UAV_CFG    = 1,  // [gsf_uav_t, gsf_uav_t]
    //GSF_ID_UAV_CMD    = 2,  // [gsf_rtsp_url_t, gsf_shmid_t]

    GSF_ID_GB28181_END
};



#ifdef __cplusplus
}
#endif

#endif