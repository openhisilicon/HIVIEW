#ifndef __gsf_h__
#define  __gsf_h__

#ifdef __cplusplus
extern "C" {
#endif

#include "fw/nm/inc/nm.h"
#include "fw/cfifo/inc/timer.h"
#include "fw/cfifo/inc/shm.h"
#include "fw/cfifo/inc/cfifo.h"
#include "fw/comm/inc/uthash.h"
#include "fw/comm/inc/list.h"

#include "msg.h"

//command-line interface 
#define GSF_ID_MOD_CLI 0
enum {
    GSF_CLI_POWEROFF = 0,
    GSF_CLI_RESTART  = 1,
    GSF_CLI_REGISTER = 2,   ///< gsf_mod_reg_t
};


//mod id defined
enum {
    GSF_MOD_ID_BSP   = 0,
    GSF_MOD_ID_CODEC = 1,
    GSF_MOD_ID_WEBS  = 2,
    GSF_MOD_ID_RTSPS = 3,
    GSF_MOD_ID_REC   = 4,
    GSF_MOD_ID_ONVIF = 5,
    GSF_MOD_ID_SVP   = 6,
    GSF_MOD_ID_APP   = 7,
    GSF_MOD_ID_END   = 15
};

//mod register
#define GSF_URI_SIZE     64
#define GSF_RE_REG_TIME  6
typedef struct {
    char uri[GSF_URI_SIZE]; // uri;
    int  mid;               // mod id;
    int  pid;               // pid;
    int  init;              // init flags;
}gsf_mod_reg_t;

//media;
enum {
  GSF_FRM_VIDEO  = 0,
  GSF_FRM_AUDIO  = 1,
};

enum {
  GSF_FRM_FLAG_IDR  = (1 << 0), //IDR;
  GSF_FRM_FLAG_SHM  = (1 << 1), //SHM;
  GSF_FRM_FLAG_EOF  = (1 << 2), //EOF;
};


enum {
	GSF_FRM_VIDEO_P      = 0, /* P  */
	GSF_FRM_VIDEO_I      = 1, /* I  */
  GSF_FRM_VIDEO_B      = 2, /* B  */
  GSF_FRM_AUDIO_1      = 3, /* 1  */
  GSF_FRM_BUTT,
};

enum {
	GSF_ENC_H264       = 0,
	GSF_ENC_H265       = 1,
	GSF_ENC_MJPEG      = 2,
	GSF_ENC_MPEG4      = 3,
	GSF_ENC_ADPCM      = 4,
	GSF_ENC_G711A      = 5,
	GSF_ENC_G711U      = 6,
	GSF_ENC_G726       = 7,
	GSF_ENC_AAC        = 8,
	GSF_ENC_BUTT,
};


#define GSF_FRM_NAL_NUM 6

typedef struct {
  unsigned char  type;   // GSF_FRM_VIDEO_P - GSF_FRM_VIDEO_B
  unsigned char  encode; // GSF_ENC_H264 - GSF_ENC_MPEG4
  unsigned char  res[2]; // res;
  unsigned short width;  // picture width;
  unsigned short height; // picture height;
  unsigned int   nal[GSF_FRM_NAL_NUM]; // nal size;
}gsf_frm_video_t;

typedef struct {
  unsigned char encode; // GSF_ENC_ADPCM - GSF_ENC_AAC
  unsigned char sp;     // SamplesPerSec
  unsigned char bps;    // BitsPerSample
  unsigned char chn;    // ChannelNum
  unsigned char res[4]; // res;
  unsigned int  res2[6];// res;
}gsf_frm_audio_t;

typedef struct {
  unsigned char type;   // GSF_FRM_VIDEO - GSF_FRM_AUDIO
  unsigned char flag;   // GSF_FRM_FLAG_;
  unsigned short seq;   // sequence;
  unsigned int   utc;   // localtime ms;
  unsigned int   pts;   // timestamp ms;
  unsigned int   size;  // data size;
  union
  {
    gsf_frm_video_t video;
    gsf_frm_audio_t audio;
  };
  unsigned char data[0]; //line-data or shm_ptr_t;
}gsf_frm_t;


//mod;
#include "mod/bsp/inc/log.h"             
#include "mod/bsp/inc/bsp.h"
#include "mod/codec/inc/codec.h"
#include "mod/rec/inc/rec.h"

#ifdef __cplusplus
}
#endif

#endif