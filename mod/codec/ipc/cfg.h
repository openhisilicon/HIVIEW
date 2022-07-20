#ifndef __cfg_h__
#define __cfg_h__

#include "codec.h"

typedef struct {
  int video_shmid;
  struct cfifo_ex* video_fifo;
  gsf_sdp_val_t val[4];
  int vst;
  void* pstStream;  //VENC_STREAM_S*
  void* pstStreamBufInfo; //VENC_STREAM_BUF_INFO_S*
  char usei[256];
  int  usei_len;
  int frameSize;
}venc_mgr_t;

extern int audio_shmid;
extern venc_mgr_t venc_mgr[GSF_CODEC_IPC_CHN*GSF_CODEC_VENC_NUM];
extern gsf_codec_ipc_t codec_ipc;
extern char codec_parm_path[128];

int json_parm_load(char *filename, gsf_codec_ipc_t *cfg);
int json_parm_save(char *filename, gsf_codec_ipc_t *cfg);

#endif