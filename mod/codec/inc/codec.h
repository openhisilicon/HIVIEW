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
    GSF_ID_CODEC_VENC     = 1,  // ch, sid, gsf_venc_t
    GSF_ID_CODEC_AENC     = 2,  // ch, sid, gsf_aenc_t
    GSF_ID_CODEC_SDP      = 3,  // ch, sid, gsf_sdp_t;
    GSF_ID_CODEC_IDR      = 4,  // ch, sid, nil;
    GSF_ID_CODEC_OSD      = 5,  // ch, sid, gsf_osd_t
    GSF_ID_CODEC_VMASK    = 6,  // ch, sid, gsf_vmask_t
    GSF_ID_CODEC_SNAP     = 7,  // ch, sid, nil;
    GSF_ID_CODEC_VORES    = 8,  // ch, sid, gsf_resolu_t;
    GSF_ID_CODEC_VOLY     = 9,  // ch, sid, gsf_layout_t;
    GSF_ID_CODEC_VOMV     = 10, // ch, sid, gsf_rect_t;
    GSF_ID_CODEC_END
};

enum {
    GSF_CODEC_ERR = -1,
};

typedef struct {
  int resolu;
}gsf_resolu_t;

typedef struct {
  int video_shmid;
  int audio_shmid;
}gsf_shmid_t;

typedef struct {
  int layout;
  gsf_shmid_t shmid[GSF_CODEC_NVR_CHN];
}gsf_layout_t;

typedef struct {
  int x, y;
  int w, h;
}gsf_rect_t;

typedef struct {
  int size;
  unsigned char data[128];
}gsf_sdp_val_t;

enum {
  GSF_SDP_VAL_SPS  = 0,
  GSF_SDP_VAL_PPS  = 1,
  GSF_SDP_VAL_VPS  = 2,
  GSF_SDP_VAL_SEI  = 3,
};

typedef struct {
  int video_shmid;
  int audio_shmid;
  gsf_aenc_t aenc;
  gsf_venc_t venc;
  gsf_sdp_val_t val[4];
}gsf_sdp_t;

#ifdef __cplusplus
}
#endif

#endif