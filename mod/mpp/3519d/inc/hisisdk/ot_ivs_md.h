/*
  Copyright (c), 2001-2024, Shenshu Tech. Co., Ltd.
 */

#ifndef OT_IVS_MD_H
#define OT_IVS_MD_H

#include "ot_common_md.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */

/*
 *   Prototype    : ot_ivs_md_init
 *   Description  : Motion Detection(MD) initialization.
 *   Parameters   : td_void.
 *
 *   Return Value : TD_SUCCESS: Success;Error codes: Failure.
 *   Date         : 2019-06-30
 *   Author       : Multimedia software (SVP) group
 */
td_s32 ot_ivs_md_init(td_void);

/*
 *   Prototype    : ot_ivs_md_exit
 *   Description  : Motion Detection(MD) exit.
 *   Parameters   : td_void.
 *
 *   Return Value : TD_SUCCESS: Success;Error codes: Failure.
 *   Date         : 2019-06-30
 *   Author       : Multimedia software (SVP) group
 */
td_s32 ot_ivs_md_exit(td_void);

/*
 *   Prototype    : ot_ivs_md_create_chn
 *   Description  : Create Motion Detection(MD) Chn.
 *   Parameters   : ot_md_chn       md_chn      Md chn.
 *                  ot_md_attr      *md_attr    Md attribute parameters
 *   Return Value : TD_SUCCESS: Success;Error codes: Failure.
 *   Date         : 2019-06-30
 *   Author       : Multimedia software (SVP) group
 */
td_s32 ot_ivs_md_create_chn(ot_md_chn md_chn, const ot_md_attr *md_attr);

/*
 *   Prototype    : ot_ivs_md_destroy_chn
 *   Description  : Destroy Motion Detection(MD) chn.
 *   Parameters   : ot_md_chn   md_chn   Md chn that would be destroy.
 *
 *   Return Value : TD_SUCCESS: Success;Error codes: Failure.
 *   Date         : 2019-06-30
 *   Author       : Multimedia software (SVP) group
 */
td_s32 ot_ivs_md_destroy_chn(ot_md_chn md_chn);

/*
 *   Prototype    : ot_ivs_md_set_chn_attr
 *   Description  : Set Motion Detection(MD) chn attribute.
 *   Parameters   : ot_md_chn       md_chn      Md chn.
 *                  ot_md_attr      *md_attr    Md attribute parameters
 *   Return Value : TD_SUCCESS: Success;Error codes: Failure.
 *   Date         : 2019-06-30
 *   Author       : Multimedia software (SVP) group
 */
td_s32 ot_ivs_md_set_chn_attr(ot_md_chn md_chn, const ot_md_attr *md_attr);

/*
 *   Prototype    : ot_ivs_md_get_chn_attr
 *   Description  : Get Motion Detection(MD) chn attribute.
 *   Parameters   : ot_md_chn       md_chn      Md chn.
 *                  ot_md_attr      *md_attr    Md attribute parameters
 *   Return Value : TD_SUCCESS: Success;Error codes: Failure.
 *   Date         : 2019-06-30
 *   Author       : Multimedia software (SVP) group
 */
td_s32 ot_ivs_md_get_chn_attr(ot_md_chn md_chn, ot_md_attr *md_attr);

/*
 *   Prototype    : ot_ivs_md_get_bg
 *   Description  : Get Motion Detection(MD) background image.
 *   Parameters   : ot_md_chn           md_chn      Md chn.
 *                  ot_svp_dst_img    *pstBg      Output background image
 *   Return Value : TD_SUCCESS: Success;Error codes: Failure.
 *   Date         : 2019-06-30
 *   Author       : Multimedia software (SVP) group
 */
td_s32 ot_ivs_md_get_bg(ot_md_chn md_chn, const ot_svp_dst_img *bg);

/*
 *   Prototype    : OT_IVS_MD_Process
 *   Description  : Motion Detection(MD) process.
 *   Parameters   : ot_md_chn               md_chn      Md chn.
 *                  ot_svp_src_img        *cur        Current image
 *                  ot_svp_src_img        *ref        Reference image
 *                  ot_svp_dst_img        *sad        Output result of sad value
 *                  ot_svp_dst_mem_info     *blob       Output blob
 *   Return Value : TD_SUCCESS: Success;Error codes: Failure.
 *   Date         : 2019-06-30
 *   Author       : Multimedia software (SVP) group
 */
td_s32 ot_ivs_md_proc(ot_md_chn md_chn, const ot_svp_src_img *cur, const ot_svp_src_img *ref,
    const ot_svp_dst_img *sad, const ot_svp_dst_mem_info *blob);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
#endif /* OT_IVS_MD_H */
