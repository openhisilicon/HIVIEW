/*
  Copyright (c), 2001-2024, Shenshu Tech. Co., Ltd.
 */
#ifndef OT_COMMON_MD_H
#define OT_COMMON_MD_H

#include "ot_common_ive.h"
#include "ot_debug.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/* ODT error code */
/* ODT Invalid channel ID: 0xA0308003 */
#define OT_ERR_ODT_INVALID_CHN_ID OT_DEFINE_ERR(OT_ID_OD, OT_ERR_LEVEL_ERROR, OT_ERR_INVALID_CHN_ID)
/* ODT exist: 0xA0308008 */
#define OT_ERR_ODT_EXIST OT_DEFINE_ERR(OT_ID_OD, OT_ERR_LEVEL_ERROR, OT_ERR_EXIST)
/* ODT unexist: 0xA0308009 */
#define OT_ERR_ODT_UNEXIST OT_DEFINE_ERR(OT_ID_OD, OT_ERR_LEVEL_ERROR, OT_ERR_UNEXIST)
/* ODT The operation, changing static attributes for example, is not permitted: 0xA030800d */
#define OT_ERR_ODT_NOT_PERM OT_DEFINE_ERR(OT_ID_OD, OT_ERR_LEVEL_ERROR, OT_ERR_NOT_PERM)
/* ODT the system is not ready because it may be not initialized: 0xA0308018 */
#define OT_ERR_ODT_NOT_READY OT_DEFINE_ERR(OT_ID_OD, OT_ERR_LEVEL_ERROR, OT_ERR_NOT_READY)
/* ODT busy: 0xA0308022 */
#define OT_ERR_ODT_BUSY OT_DEFINE_ERR(OT_ID_OD, OT_ERR_LEVEL_ERROR, OT_ERR_BUSY)

/*
 * Definition md algorithm mode
 */
typedef enum {
    OT_MD_ALG_MODE_BG = 0x0,  /* Base on background image */
    OT_MD_ALG_MODE_REF = 0x1, /* Base on reference image */

    OT_MD_ALG_MODE_BUTT
} ot_md_alg_mode;

typedef struct {
    ot_md_alg_mode alg_mode;          /* Md algorithm mode */
    ot_ive_sad_mode sad_mode;         /* Sad mode */
    ot_ive_sad_out_ctrl sad_out_ctrl; /* Sad output ctrl */
    td_u32 width;                     /* Image width */
    td_u32 height;                    /* Image height */
    td_u16 sad_threshold;             /* Sad threshold */
    ot_ive_ccl_ctrl ccl_ctrl;         /* Ccl ctrl */
    ot_ive_add_ctrl add_ctrl;         /* Add ctrl */
} ot_md_attr;

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* OT_COMMON_MD_H */
