#ifndef __venc_h__
#define __venc_h__

#include "codec.h"
#include "mpp.h"
#include "cfg.h"
#include "msg_func.h"

typedef struct {
  int ch_num;
  int st_num;
}gsf_venc_ini_t;

int gsf_venc_init(gsf_venc_ini_t *ini);

int gsf_venc_recv(VENC_CHN VeChn, PAYLOAD_TYPE_E PT, VENC_STREAM_S* pstStream, void* uargs);

int gsf_aenc_recv(int AeChn, PAYLOAD_TYPE_E PT, AUDIO_STREAM_S* pstStream, void* uargs);


#endif // __venc_h__