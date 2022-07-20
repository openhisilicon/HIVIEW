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
    GSF_CLI_VERSION  = 3,
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
    GSF_MOD_ID_SIPS  = 8,
    GSF_MOD_ID_RTMPS = 9,
    GSF_MOD_ID_SRTS  = 10,
    //user custom mod;
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


enum {
	GSF_ENC_H264       = 0,
	GSF_ENC_H265       = 1,
	GSF_ENC_MJPEG      = 2,
	GSF_ENC_JPEG       = 3,
	GSF_ENC_MPEG4      = 4,
	GSF_ENC_ADPCM      = 5,
	GSF_ENC_G711A      = 6,
	GSF_ENC_G711U      = 7,
	GSF_ENC_G726       = 8,
	GSF_ENC_AAC        = 9,
	GSF_ENC_BUTT,
};


//only include bsp mod;
#include "mod/bsp/inc/log.h"             
#include "mod/bsp/inc/bsp.h"
//other mod have been removed;

#ifdef __cplusplus
}
#endif

#endif