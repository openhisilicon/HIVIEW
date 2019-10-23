#ifndef __sjb_codec_h__
#define __sjb_codec_h__

#define GSF_CODEC_NVR_CHN  16 // nvr channel num;
#define GSF_CODEC_IPC_CHN   2 // ipc channel num;
#define GSF_CODEC_VENC_NUM  3 // venc num;
#define GSF_CODEC_OSD_NUM   8 // osd num;
#define GSF_CODEC_VMASK_NUM 8 // vmask num;


enum {
  GSF_VENC_TYPE_H264 = 0,
  GSF_VENC_TYPE_H265,
  GSF_VENC_TYPE_MJPEG,
  GSF_VENC_TYPE_MPEG4,
  GSF_VENC_TYPE_BUTT
};

enum {
  GSF_AENC_TYPE_PCM = 0,
  GSF_AENC_TYPE_G711A,
  GSF_AENC_TYPE_G711U,
  GSF_AENC_TYPE_G726,
  GSF_AENC_TYPE_AACLC,
  GSF_AENC_TYPE_EAAC,
  GSF_AENC_TYPE_EAACPLUS,
  GSF_AENC_TYPE_AMR_WB,
  GSF_AENC_TYPE_BUTT
};

#include "fw/comm/sjb_api.ih"
#include "mod/codec/inc/sjb_codec.ih"
#include "fw/comm/sjb_end.ih"


#endif //__sjb_codec_h__