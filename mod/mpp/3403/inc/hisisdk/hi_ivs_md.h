/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */

#ifndef __HI_IVS_MD_H__
#define __HI_IVS_MD_H__

#include "hi_common_md.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 *   Prototype    : hi_ivs_md_init
 *   Description  : Motion Detection(MD) initialization.
 *   Parameters   : hi_void.
 *
 *   Return Value : HI_SUCCESS: Success;Error codes: Failure.
 *   Date         : 2019-06-30
 *   Author       : Multimedia software (SVP) group
 */
hi_s32 hi_ivs_md_init(hi_void);

/*
 *   Prototype    : hi_ivs_md_exit
 *   Description  : Motion Detection(MD) exit.
 *   Parameters   : hi_void.
 *
 *   Return Value : HI_SUCCESS: Success;Error codes: Failure.
 *   Date         : 2019-06-30
 *   Author       : Multimedia software (SVP) group
 */
hi_s32 hi_ivs_md_exit(hi_void);

/*
 *   Prototype    : hi_ivs_md_create_chn
 *   Description  : Create Motion Detection(MD) Chn.
 *   Parameters   : hi_md_chn       md_chn      Md chn.
 *                  hi_md_attr      *md_attr    Md attribute parameters
 *   Return Value : HI_SUCCESS: Success;Error codes: Failure.
 *   Date         : 2019-06-30
 *   Author       : Multimedia software (SVP) group
 */
hi_s32 hi_ivs_md_create_chn(hi_md_chn md_chn, const hi_md_attr *md_attr);

/*
 *   Prototype    : hi_ivs_md_destroy_chn
 *   Description  : Destroy Motion Detection(MD) chn.
 *   Parameters   : hi_md_chn   md_chn   Md chn that would be destroy.
 *
 *   Return Value : HI_SUCCESS: Success;Error codes: Failure.
 *   Date         : 2019-06-30
 *   Author       : Multimedia software (SVP) group
 */
hi_s32 hi_ivs_md_destroy_chn(hi_md_chn md_chn);

/*
 *   Prototype    : hi_ivs_md_set_chn_attr
 *   Description  : Set Motion Detection(MD) chn attribute.
 *   Parameters   : hi_md_chn       md_chn      Md chn.
 *                  hi_md_attr      *md_attr    Md attribute parameters
 *   Return Value : HI_SUCCESS: Success;Error codes: Failure.
 *   Date         : 2019-06-30
 *   Author       : Multimedia software (SVP) group
 */
hi_s32 hi_ivs_md_set_chn_attr(hi_md_chn md_chn, const hi_md_attr *md_attr);

/*
 *   Prototype    : hi_ivs_md_get_chn_attr
 *   Description  : Get Motion Detection(MD) chn attribute.
 *   Parameters   : hi_md_chn       md_chn      Md chn.
 *                  hi_md_attr      *md_attr    Md attribute parameters
 *   Return Value : HI_SUCCESS: Success;Error codes: Failure.
 *   Date         : 2019-06-30
 *   Author       : Multimedia software (SVP) group
 */
hi_s32 hi_ivs_md_get_chn_attr(hi_md_chn md_chn, hi_md_attr *md_attr);

/*
 *   Prototype    : hi_ivs_md_get_bg
 *   Description  : Get Motion Detection(MD) background image.
 *   Parameters   : hi_md_chn           md_chn      Md chn.
 *                  hi_svp_dst_img    *pstBg      Output background image
 *   Return Value : HI_SUCCESS: Success;Error codes: Failure.
 *   Date         : 2019-06-30
 *   Author       : Multimedia software (SVP) group
 */
hi_s32 hi_ivs_md_get_bg(hi_md_chn md_chn, const hi_svp_dst_img *bg);

/*
 *   Prototype    : hi_ivs_md_proc
 *   Description  : Motion Detection(MD) process.
 *   Parameters   : hi_md_chn               md_chn      Md chn.
 *                  hi_svp_src_img        *cur        Current image
 *                  hi_svp_src_img        *ref        Reference image
 *                  hi_svp_dst_img        *sad        Output result of sad value
 *                  hi_svp_dst_mem_info     *blob       Output blob
 *   Return Value : HI_SUCCESS: Success;Error codes: Failure.
 *   Date         : 2019-06-30
 *   Author       : Multimedia software (SVP) group
 */
hi_s32 hi_ivs_md_proc(hi_md_chn md_chn, const hi_svp_src_img *cur, const hi_svp_src_img *ref,
    const hi_svp_dst_img *sad, const hi_svp_dst_mem_info *blob);

#ifdef __cplusplus
}
#endif
#endif /* __HI_IVS_MD_H__ */
