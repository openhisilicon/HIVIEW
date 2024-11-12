/*
  Copyright (c), 2001-2024, Shenshu Tech. Co., Ltd.
 */

#ifndef OT_VQE_REGISTER_H
#define OT_VQE_REGISTER_H

#include "ot_type.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* record */
td_void *ot_vqe_record_get_handle(td_void);
/* HPF */
td_void *ot_vqe_hpf_get_handle(td_void);
/* AEC */
td_void *ot_vqe_aec_get_handle(td_void);
/* ANR */
td_void *ot_vqe_anr_get_handle(td_void);
/* RNR */
td_void *ot_vqe_rnr_get_handle(td_void);
/* HDR */
td_void *ot_vqe_hdr_get_handle(td_void);
/* DRC */
td_void *ot_vqe_drc_get_handle(td_void);
/* PEQ */
td_void *ot_vqe_peq_get_handle(td_void);
/* AGC */
td_void *ot_vqe_agc_get_handle(td_void);
/* EQ */
td_void *ot_vqe_eq_get_handle(td_void);
/* resample */
td_void *ot_vqe_resample_get_handle(td_void);
/* GAIN */
td_void *ot_vqe_gain_get_handle(td_void);
/* talkv2 */
td_void *ot_vqe_talkv2_get_handle(td_void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* OT_VQE_REGISTER_H */
