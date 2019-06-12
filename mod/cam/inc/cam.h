#ifndef __cam_h__
#define  __cam_h__

#ifdef __cplusplus
extern "C" {
#endif

#include "inc/gsf.h"

//for json cfg;
#include "mod/cam/inc/sjb_cam.h"

#define GSF_IPC_CAM      "ipc:///tmp/cam_rep"

enum {
    GSF_ID_CAM_XXX1     = 1,  // gsf_sp_t
    GSF_ID_CAM_END
};

enum {
    GSF_CAM_ERR = -1,
};


#ifdef __cplusplus
}
#endif

#endif