#ifndef __sjb_codec_h__
#define __sjb_codec_h__

#define GSF_CODEC_NVR_CHN   16    // nvr channel num;
#define GSF_CODEC_OSD_NUM   8     // osd num;
#define GSF_CODEC_VMASK_NUM 8     // vmask num;

#if defined(GSF_CPU_3519d)
#define GSF_CODEC_IPC_CHN   (6) // ipc channel num;
#define GSF_CODEC_ST_NUM    (2) // stream num;
#define GSF_CODEC_SNAP_IDX  (GSF_CODEC_ST_NUM)
#define GSF_CODEC_VENC_NUM  (GSF_CODEC_ST_NUM)
#else
#define GSF_CODEC_IPC_CHN   (4)   // ipc channel num;
#define GSF_CODEC_ST_NUM    (2)   // stream num;
#define GSF_CODEC_SNAP_IDX  (GSF_CODEC_ST_NUM)
#define GSF_CODEC_VENC_NUM  (GSF_CODEC_ST_NUM+1)
#endif


typedef enum {
  GSF_LENS_IRC   = 1, // arg1: 0-day, 1-night
  GSF_LENS_STOP  = 2, // stop;
  GSF_LENS_ZOOM  = 3, // arg1: 1-++, 0---, arg2: speed;
  GSF_LENS_FOCUS = 4, // arg1: 1-++, 0---, arg2: speed;
  GSF_LENS_CAL   = 5, // lens calibration;
  GSF_PTZ_STOP   = 6, // arg1: 0, arg2: 0 
  GSF_PTZ_UP     = 7, // arg1: 0, arg2: speed 
  GSF_PTZ_DOWN   = 8, // arg1: 0, arg2: speed 
  GSF_PTZ_LEFT   = 9, // arg1: 0, arg2: speed  
  GSF_PTZ_RIGHT  = 10,// arg1: 0, arg2: speed  
}GSF_LENS_CMD_E;


#include "fw/cjson/inc/sjb_api.ih"
#include "mod/codec/inc/sjb_codec.ih"
#include "fw/cjson/inc/sjb_end.ih"


#endif //__sjb_codec_h__