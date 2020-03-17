#ifndef __rtsps_h__
#define  __rtsps_h__

#ifdef __cplusplus
extern "C" {
#endif

#include "inc/gsf.h"

//for json cfg;
#include "mod/rtsps/inc/sjb_rtsps.h"

#define GSF_IPC_RTSPS      "ipc:///tmp/rtsps_rep"

enum {
    GSF_ID_RTSPS_CFG = 1,
    GSF_ID_RTSPS_END
};

enum {
    GSF_RTSPS_ERR = -1,
};

#ifdef __cplusplus
}
#endif

#endif