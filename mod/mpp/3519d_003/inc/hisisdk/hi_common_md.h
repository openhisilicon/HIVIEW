/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */

#ifndef HI_COMMON_MD_H
#define HI_COMMON_MD_H

#include "hi_common_ive.h"
#include "hi_debug.h"
#include "ot_common_md.h"

#ifdef __cplusplus
extern "C" {
#endif

#define HI_ERR_ODT_INVALID_CHN_ID OT_ERR_ODT_INVALID_CHN_ID
#define HI_ERR_ODT_EXIST OT_ERR_ODT_EXIST
#define HI_ERR_ODT_UNEXIST OT_ERR_ODT_UNEXIST
#define HI_ERR_ODT_NOT_PERM OT_ERR_ODT_NOT_PERM
#define HI_ERR_ODT_NOT_READY OT_ERR_ODT_NOT_READY
#define HI_ERR_ODT_BUSY OT_ERR_ODT_BUSY
#define HI_MD_ALG_MODE_BG OT_MD_ALG_MODE_BG
#define HI_MD_ALG_MODE_REF OT_MD_ALG_MODE_REF
#define HI_MD_ALG_MODE_BUTT OT_MD_ALG_MODE_BUTT
typedef ot_md_alg_mode hi_md_alg_mode;
typedef ot_md_attr hi_md_attr;

#ifdef __cplusplus
}
#endif
#endif /* HI_COMMON_MD_H */
