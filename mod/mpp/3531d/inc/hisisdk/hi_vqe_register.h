/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2019-2020. All rights reserved.
 * Description: VQE module register
 * Author: Hisilicon multimedia software group
 * Create: 2019-02-25
 * History:
 *    1.Date        : 2019-02-25
 *      Modification: Created file
 */

#ifndef __HI_VQE_REGISTER_H__
#define __HI_VQE_REGISTER_H__

#include "hi_type.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

/* record */
hi_void *hi_vqe_record_get_handle(hi_void);
/* HPF */
hi_void *hi_vqe_hpf_get_handle(hi_void);
/* AEC */
hi_void *hi_vqe_aec_get_handle(hi_void);
/* ANR */
hi_void *hi_vqe_anr_get_handle(hi_void);
/* RNR */
hi_void *hi_vqe_rnr_get_handle(hi_void);
/* HDR */
hi_void *hi_vqe_hdr_get_handle(hi_void);
/* DRC */
hi_void *hi_vqe_drc_get_handle(hi_void);
/* PEQ */
hi_void *hi_vqe_peq_get_handle(hi_void);
/* AGC */
hi_void *hi_vqe_agc_get_handle(hi_void);
/* EQ */
hi_void *hi_vqe_eq_get_handle(hi_void);
/* resample */
hi_void *hi_vqe_resample_get_handle(hi_void);
/* GAIN */
hi_void *hi_vqe_gain_get_handle(hi_void);
/* talkv2 */
hi_void *hi_vqe_talkv2_get_handle(hi_void);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __HI_VQE_REGISTER_H__ */
