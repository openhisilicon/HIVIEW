#ifndef __sample_h__
#define  __sample_h__


#ifdef __cplusplus
extern "C" {
#endif

//top inc;
#include "inc/gsf.h"

//for json cfg;
#include "mod/sample/inc/sjb_sample.h"


#define GSF_IPC_SAMPLE "ipc:///tmp/sample_rep"

enum {
    GSF_ID_SAMPLE_CFG    = 1,  // [gsf_sample_t, gsf_sample_t]
    
    GSF_ID_SAMPLE_END
};

enum {
  GSF_SAMPLE_ERR = -1,
};

#ifdef __cplusplus
}
#endif

#endif