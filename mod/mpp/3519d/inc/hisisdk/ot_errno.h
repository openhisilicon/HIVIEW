/*
  Copyright (c), 2001-2024, Shenshu Tech. Co., Ltd.
 */

#ifndef OT_ERRNO_H
#define OT_ERRNO_H

#include "ot_debug.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* end of #ifdef __cplusplus */

/*
 * 1010 0000b
 * VTOP use APPID from 0~31
 * so, vendor use APPID based on 32
 */
#define OT_ERR_APP_ID (0x80000000L + 0x20000000L)

/*
 * |----------------------------------------------------------------|
 * | 1 |   APP_ID   |   MOD_ID    | ERR_LEVEL |   ERR_ID            |
 * |----------------------------------------------------------------|
 * |<--><--7bits----><----8bits---><--3bits---><------13bits------->|
 */
#define OT_DEFINE_ERR(mod, level, err_id) \
    ((td_s32)((OT_ERR_APP_ID) | ((mod) << 16) | ((level) << 13) | (err_id)))

/*
 * NOTE! the following defined all common error code,
 * all module must reserved 0~63 for their common error code
 */
typedef enum {
    OT_ERR_LEVEL_DEBUG = 0,  /* debug-level                                  */
    OT_ERR_LEVEL_INFO,       /* informational                                */
    OT_ERR_LEVEL_NOTICE,     /* normal but significant condition             */
    OT_ERR_LEVEL_WARNING,    /* warning conditions                           */
    OT_ERR_LEVEL_ERROR,      /* error conditions                             */
    OT_ERR_LEVEL_CRIT,       /* critical conditions                          */
    OT_ERR_LEVEL_ALERT,      /* action must be taken immediately             */
    OT_ERR_LEVEL_FATAL,      /* just for compatibility with previous version */
    OT_ERR_LEVEL_BUTT
} ot_err_level;

typedef enum {
    OT_ERR_INVALID_DEV_ID  =  0x1, /* invalid device ID                          */
    OT_ERR_INVALID_PIPE_ID =  0x2, /* invalid pipe ID                           */
    OT_ERR_INVALID_CHN_ID  =  0x3, /* invalid channel ID                          */
    OT_ERR_INVALID_LAYER_ID = 0x4, /* invalid channel ID                          */
    OT_ERR_INVALID_GRP_ID   = 0x5, /* invalid group ID                          */

    OT_ERR_ILLEGAL_PARAM   = 0x7, /* at lease one parameter is illegal
                               * eg, an illegal enumeration value             */
    OT_ERR_EXIST           = 0x8, /* resource exists                              */
    OT_ERR_UNEXIST         = 0x9, /* resource not exists                            */
    OT_ERR_NULL_PTR        = 0xa, /* using a NULL pointer                           */
    OT_ERR_NOT_CFG         = 0xb, /* try to enable or initialize system, device
                              ** or channel, before configing attribute       */
    OT_ERR_NOT_SUPPORT     = 0xc, /* operation or type is not supported by NOW    */
    OT_ERR_NOT_PERM        = 0xd, /* operation is not permitted
                              ** eg, try to change static attribute           */
    OT_ERR_NOT_ENABLE      = 0x10,
    OT_ERR_NOT_DISABLE     = 0x11,
    OT_ERR_NOT_START       = 0x12,
    OT_ERR_NOT_STOP        = 0x13,
    OT_ERR_NO_MEM          = 0x14, /* failure caused by malloc memory              */
    OT_ERR_NO_BUF          = 0x15, /* failure caused by malloc buffer              */
    OT_ERR_BUF_EMPTY       = 0x16, /* no data in buffer                            */
    OT_ERR_BUF_FULL        = 0x17, /* no buffer for new data                       */
    OT_ERR_NOT_READY       = 0x18, /* System is not ready, maybe not initialed or
                              ** loaded. Returning the error code when opening
                              ** a device file failed.                        */
    OT_ERR_TIMEOUT         = 0x20,
    OT_ERR_BAD_ADDR        = 0x21, /* bad address,
                              ** eg. used for copy_from_user & copy_to_user   */
    OT_ERR_BUSY            = 0x22, /* resource is busy,
                              ** eg. destroy a venc chn without unregister it */
    OT_ERR_SIZE_NOT_ENOUGH = 0x23, /* buffer size is smaller than the actual size required */

    OT_ERR_NOT_BINDED      = 0x24,
    OT_ERR_BINDED          = 0x25,
    OT_ERR_BUTT            = 0x3f, /* maximum code, private error code of all modules
                              ** must be greater than it                      */
} ot_errno;

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* OT_ERRNO_H */
