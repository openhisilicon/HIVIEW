#ifndef __codec_h__
#define  __codec_h__

#ifdef __cplusplus
extern "C" {
#endif

#include "inc/gsf.h"

//for json cfg;
#include "mod/codec/inc/sjb_codec.h"

#define GSF_IPC_CODEC      "ipc:///tmp/codec_rep"

enum {
    GSF_ID_CODEC_VENC     = 1,  // gsf_venc_t
    GSF_ID_CODEC_AENC     = 2,  // gsf_aenc_t
    GSF_ID_CODEC_STCTL    = 3,  // gsf_stctl_t
    GSF_ID_CODEC_OSD      = 4,  // gsf_osd_t
    GSF_ID_CODEC_VMASK    = 5,  // gsf_vmask_t
    GSF_ID_CODEC_END
};

enum {
    GSF_CODEC_ERR = -1,
};

typedef struct {
  int st;    // stream index;
  int ops;   // 1: open, 0: close;
  int shmid; // result;
}gsf_stctl_t;


#ifdef __cplusplus
}
#endif

#endif