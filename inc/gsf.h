#ifndef __gsf_h__
#define  __gsf_h__

#ifdef __cplusplus
extern "C" {
#endif

#include "fw/nm/inc/nm.h"
#include "fw/cfifo/inc/timer.h"
#include "fw/cfifo/inc/shm.h"
#include "fw/cfifo/inc/cfifo.h"
#include "fw/comm/uthash.h"

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
    GSF_MOD_ID_SYS   = 0,
    GSF_MOD_ID_CODEC = 1,
    GSF_MOD_ID_CAM   = 2,
    GSF_MOD_ID_REC   = 3,
    GSF_MOD_ID_JOINT = 4,
    GSF_MOD_ID_USER  = 16,
    GSF_MOD_ID_END   = 32
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

//mod;
#include "mod/bsp/inc/log.h"             
#include "mod/bsp/inc/bsp.h"
#include "mod/codec/inc/codec.h"
#include "mod/rec/inc/rec.h"

#ifdef __cplusplus
}
#endif

#endif