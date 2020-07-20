#ifndef __cfg_h__
#define __cfg_h__

#include "codec.h"

typedef struct {
  int video_shmid;
  struct cfifo_ex* video_fifo;
  gsf_sdp_val_t val[4];
  int vst;
  void* pstStream;
}venc_mgr_t;

extern venc_mgr_t venc_mgr[GSF_CODEC_NVR_CHN*GSF_CODEC_VENC_NUM];

extern char codec_parm_path[128];
extern gsf_codec_nvr_t codec_nvr;

int json_parm_load(char *filename, gsf_codec_nvr_t *cfg);
int json_parm_save(char *filename, gsf_codec_nvr_t *cfg);

#endif