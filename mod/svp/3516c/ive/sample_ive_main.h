/*
  Copyright (c), 2001-2024, Shenshu Tech. Co., Ltd.
 */

#ifndef SAMPLE_IVE_MAIN_H
#define SAMPLE_IVE_MAIN_H
#include "hi_type.h"
#include "securec.h"

/*
 * function : show Canny sample
 */
hi_void sample_ive_canny(hi_char canny_complete);

/*
 * function : show Gmm2 sample
 */
hi_void sample_ive_gmm2(hi_void);

/*
 * function : show Test Memory sample
 */
hi_void sample_ive_test_memory(hi_void);

/*
 * function : show Sobel sample
 */
hi_void sample_ive_sobel(hi_void);

/*
 * function : show St Lk sample
 */
hi_void sample_ive_st_lk(hi_void);

/*
 * function : show Occlusion detected sample
 */
hi_void sample_ive_od(hi_void);

/*
 * function : show Md sample
 */
hi_void sample_ive_md(hi_void);

/*
 * function : show PerspTrans sample
 */
hi_void sample_ive_warp_persp_trans(hi_void);

/*
 * function : show Kcf sample
 */
hi_void sample_ive_kcf(hi_void);

/*
 * function : show resize sample
 */
hi_void sample_ive_resize(hi_void);

/*
 * function :Canny sample signal handle
 */
hi_void sample_ive_canny_handle_sig(hi_void);

/*
 * function : Gmm2 sample signal handle
 */
hi_void sample_ive_gmm2_handle_sig(hi_void);


/*
 * function : TestMemory sample signal handle
 */
hi_void sample_ive_test_memory_handle_sig(hi_void);

/*
 * function : Sobel sample signal handle
 */
hi_void sample_ive_sobel_handle_sig(hi_void);

/*
 * function : St_Lk sample signal handle
 */
hi_void sample_ive_st_lk_handle_sig(hi_void);

/*
 * function : PerspTrans sample signal handle
 */
hi_void sample_ive_warp_persp_trans_handle_sig(hi_void);

/*
 * function : Od sample signal handle
 */
hi_void sample_ive_od_handle_sig(hi_void);

/*
 * function : Md sample signal handle
 */
hi_void sample_ive_md_handle_sig(hi_void);

/*
 * function :Resize sample signal handle
 */
hi_void sample_ive_resize_handle_sig(hi_void);

#endif

