#ifndef __sips_h__
#define  __sips_h__


#ifdef __cplusplus
extern "C" {
#endif


#include "inc/gsf.h"

//for json cfg;
#include "mod/sips/inc/sjb_sips.h"


#define GSF_IPC_SIPS "ipc:///tmp/sips_rep"

enum {
    GSF_ID_SIPS_CFG    = 1,  // [gsf_sips_t, gsf_sips_t]
    GSF_ID_SIPS_END
};

enum {
  GSF_SIPS_ERR = -1,
};


#ifdef __cplusplus
}
#endif

#endif