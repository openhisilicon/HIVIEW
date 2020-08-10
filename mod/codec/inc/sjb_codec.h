#ifndef __sjb_codec_h__
#define __sjb_codec_h__

#define GSF_CODEC_NVR_CHN   4 //16    // nvr channel num;
#define GSF_CODEC_IPC_CHN   4     // ipc channel num;
#define GSF_CODEC_ST_NUM    2     // stream num;
#define GSF_CODEC_OSD_NUM   8     // osd num;
#define GSF_CODEC_VMASK_NUM 8     // vmask num;
#define GSF_CODEC_SNAP_IDX  (GSF_CODEC_ST_NUM)
#define GSF_CODEC_VENC_NUM  (GSF_CODEC_ST_NUM)

#include "fw/cjson/inc/sjb_api.ih"
#include "mod/codec/inc/sjb_codec.ih"
#include "fw/cjson/inc/sjb_end.ih"


#endif //__sjb_codec_h__