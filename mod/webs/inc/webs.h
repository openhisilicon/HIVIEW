#ifndef __webs_h__
#define  __webs_h__

#ifdef __cplusplus
extern "C" {
#endif


#include "inc/gsf.h"

//for json cfg;
#include "mod/webs/inc/sjb_webs.h"


#define GSF_IPC_WEBS "ipc:///tmp/webs_rep"

enum {
  GSF_ID_WEBS_CFG = 1,   // gsf_webs_cfg_t;
  GSF_ID_WEBS_USER= 2,   // gsf_user_t[N];
  GSF_ID_WEBS_END
};

enum {
  GSF_WEBS_ERR = -1,
};


#ifdef __cplusplus
}
#endif

#endif