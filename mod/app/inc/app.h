#ifndef __app_h__
#define  __app_h__

#ifdef __cplusplus
extern "C" {
#endif

#include "inc/gsf.h"

//for json cfg;
#include "mod/codec/inc/sjb_codec.h"
#include "mod/app/inc/sjb_app.h"

#define GSF_IPC_APP      "ipc:///tmp/app_rep"

enum {
    GSF_ID_APP_CHSRC  = 1,  // [ gsf_chsrc_t, gsf_chsrc_t .... N]
    GSF_ID_APP_GUI    = 2,  // gsf_gui_t
    GSF_ID_APP_END
};

enum {
    GSF_APP_ERR = -1,
};




#ifdef __cplusplus
}
#endif

#endif