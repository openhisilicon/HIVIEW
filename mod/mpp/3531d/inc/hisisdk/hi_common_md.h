/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: definition of hi_common_md.h
 * Author: Hisilicon multimedia software (SVP) group
 * Create: 2019-06-10
 */
#ifndef _HI_COMMON_MD_H_
#define _HI_COMMON_MD_H_

#include "hi_common_ive.h"
#include "hi_debug.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/* ODT error code */
/* ODT Invalid channel ID: 0xA0308003 */
#define HI_ERR_ODT_INVALID_CHN_ID HI_DEFINE_ERR(HI_ID_OD, HI_ERR_LEVEL_ERROR, HI_ERR_INVALID_CHN_ID)
/* ODT exist: 0xA0308008 */
#define HI_ERR_ODT_EXIST HI_DEFINE_ERR(HI_ID_OD, HI_ERR_LEVEL_ERROR, HI_ERR_EXIST)
/* ODT unexist: 0xA0308009 */
#define HI_ERR_ODT_UNEXIST HI_DEFINE_ERR(HI_ID_OD, HI_ERR_LEVEL_ERROR, HI_ERR_UNEXIST)
/* ODT The operation, changing static attributes for example, is not permitted: 0xA030800d */
#define HI_ERR_ODT_NOT_PERM HI_DEFINE_ERR(HI_ID_OD, HI_ERR_LEVEL_ERROR, HI_ERR_NOT_PERM)
/* ODT the system is not ready because it may be not initialized: 0xA0308018 */
#define HI_ERR_ODT_NOT_READY HI_DEFINE_ERR(HI_ID_OD, HI_ERR_LEVEL_ERROR, HI_ERR_NOT_READY)
/* ODT busy: 0xA0308022 */
#define HI_ERR_ODT_BUSY HI_DEFINE_ERR(HI_ID_OD, HI_ERR_LEVEL_ERROR, HI_ERR_BUSY)

/*
 * Definition md algorithm mode
 */
typedef enum {
    HI_MD_ALG_MODE_BG = 0x0,  /* Base on background image */
    HI_MD_ALG_MODE_REF = 0x1, /* Base on reference image */

    HI_MD_ALG_MODE_BUTT
} hi_md_alg_mode;

typedef struct {
    hi_md_alg_mode alg_mode;          /* Md algorithm mode */
    hi_ive_sad_mode sad_mode;         /* Sad mode */
    hi_ive_sad_out_ctrl sad_out_ctrl; /* Sad output ctrl */
    hi_u32 width;                     /* Image width */
    hi_u32 height;                    /* Image height */
    hi_u16 sad_threshold;             /* Sad threshold */
    hi_ive_ccl_ctrl ccl_ctrl;         /* Ccl ctrl */
    hi_ive_add_ctrl add_ctrl;         /* Add ctrl */
} hi_md_attr;

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* _HI_COMMON_MD_H_ */
