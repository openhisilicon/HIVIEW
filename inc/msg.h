#ifndef __msg_h__
#define  __msg_h__

#ifdef __cplusplus
extern "C" {
#endif

#include <time.h>

enum {
    GET = 0, SET = 1
};
enum {
    FALSE = 0, TRUE = 1
};

enum {
    GSF_ERR_MSG = -10000
};

typedef struct {
    short ver;              // version;
    short id;               // msgid;
    char  ch,set;           // channel, set/get;
    char  sid,nsave;        // subid, not-save-set;
    int   ts;               // timestamp;
    int   err;              // errno;
    int   size;             // msgsize;
    char  data[0];          // msgdata;
}gsf_msg_t;

#define GSF_MSG_DEF(_stru, _var, _csize)\
        char __msg_buf[_csize] = {0};\
        gsf_msg_t *__pmsg = (gsf_msg_t*)__msg_buf;\
        _stru* _var = (_stru *)__pmsg->data;\
        int __rsize = sizeof(__msg_buf);

#define GSF_MSG_SENDTO(_id, _ch, _set, _sid, _size, _uri, _to) ({\
		int _ret   = 0; \
  	struct timespec _ts; \
    clock_gettime(CLOCK_MONOTONIC, &_ts);\
    __pmsg->ver	=  0; \
		__pmsg->id	= _id;\
		__pmsg->ch 	= _ch;\
		__pmsg->set = _set;\
		__pmsg->sid = _sid;\
		__pmsg->ts  = _ts.tv_sec*1000 + _ts.tv_nsec/1000000;\
		__pmsg->err = 0;\
		__pmsg->size= _size;\
    _ret = nm_req_sendto(_uri, _to, (char*)__pmsg, sizeof(gsf_msg_t)+__pmsg->size, (char*)__pmsg, &__rsize);\
    _ret = (_ret < 0)?_ret:__pmsg->err;\
		})


#ifdef __cplusplus
}
#endif

#endif