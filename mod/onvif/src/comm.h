/*
 * 公用结构体定义
 *
 */

#ifndef __comm_h__
#define __comm_h__

//#include "log.h"
#include "main.h"

#include "nvc.h"
#include "nvt.h"

#define SEND_MSG_TO_MAIN(_id, _arg, _ext, _ch, buf) ({\
        int _ret;\
        sdk_msg_t *pmsg = (sdk_msg_t*)buf;\
        pmsg->id.src    = MODULE_ID_NET;\
        pmsg->id.dst    = MODULE_ID_MAIN;\
        pmsg->msg_id    = (_id);\
        pmsg->f_dir     = SDK_MSG_REQ;\
        pmsg->chann     = (_ch);\
        pmsg->args      = (_arg);\
        pmsg->extend    = (_ext);\
        _ret = (g_nvt->p.msg_cb != NULL)?(g_nvt->p.msg_cb(pmsg)):(-1);\
        })

#define SEND_MSG_TO_MAIN1(_id, _arg, _ext, _ch, pmsg) ({\
        int _ret;\
        pmsg->id.src    = MODULE_ID_NET;\
        pmsg->id.dst    = MODULE_ID_MAIN;\
        pmsg->msg_id    = (_id);\
        pmsg->f_dir     = SDK_MSG_REQ;\
        pmsg->chann     = (_ch);\
        pmsg->args      = (_arg);\
        pmsg->extend    = (_ext);\
        _ret = (g_nvt->p.msg_cb != NULL)?(g_nvt->p.msg_cb(pmsg)):(-1);\
        })


#endif //__comm_h__
