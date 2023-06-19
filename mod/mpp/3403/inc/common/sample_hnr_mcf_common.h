/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */

#ifndef SAMPLE_HNR_MCF_COMMON_H
#define SAMPLE_HNR_MCF_COMMON_H
#include "hi_common.h"
#include "hi_math.h"
#include "hi_buffer.h"
#include "securec.h"
#include "hi_mipi_rx.h"
#include "hi_mipi_tx.h"
#include "hi_common_sys.h"
#include "hi_common_isp.h"
#include "hi_common_vpss.h"
#include "hi_common_aio.h"
#include "hi_common_hdmi.h"
#include "hi_mpi_sys.h"
#include "hi_mpi_vpss.h"
#include "hi_mpi_mcf.h"
#include "sample_comm.h"
#include "hi_mpi_mcf_vi.h"

typedef struct {
    hi_char *input_file_name;
    hi_size input_img_size;
    hi_char *output_file_name;
    hi_size output_img_size;
} sample_comm_mcf_scale_img_param;

#define safe_free(memory) do { \
    if ((memory) != HI_NULL) { \
        free(memory); \
        memory = HI_NULL; \
    } \
} while (0)

hi_void sample_hnr_common_mcf_get_default_grp_attr(hi_mcf_grp_attr *grp_attr, hi_size *mono_size, hi_size *color_size);
hi_s32 sample_hnr_common_vi_bind_mcf(hi_vi_pipe vi_pipe, hi_vi_chn vi_chn, hi_mcf_grp mcf_grp, hi_mcf_pipe mcf_pipe);
hi_s32 sample_hnr_common_vi_unbind_mcf(hi_vi_pipe vi_pipe, hi_vi_chn vi_chn, hi_mcf_grp mcf_grp, hi_mcf_pipe mcf_pipe);

hi_s32 sample_hnr_common_mcf_start(hi_mcf_grp grp, const hi_mcf_grp_attr *grp_attr, hi_crop_info *grp_crop_info,
                                   hi_bool *mcf_chn_en, hi_size *chn_out_size);

hi_s32 sample_hnr_common_mcf_stop_vpss(const hi_mcf_grp_attr *grp_attr);
hi_s32 sample_hnr_common_mcf_stop(hi_mcf_grp grp, const hi_bool *chn_enable, hi_u32 chn_array_size);
hi_s32 sample_hnr_common_mcf_start_vpss(const hi_mcf_grp_attr *mcf_grp_attr);
hi_s32 sample_hnr_common_mcf_bind_vpss(hi_mcf_grp mcf_grp, hi_mcf_chn mcf_chn, hi_vpss_grp vpss_grp,
    hi_vpss_chn vpss_chn);
hi_s32 sample_hnr_common_mcf_unbind_vpss(hi_mcf_grp mcf_grp, hi_mcf_chn mcf_chn, hi_vpss_grp vpss_grp,
    hi_vpss_chn vpss_chn);

/* functions for hnr */
hi_s32 sample_hnr_mcf_check_size(hi_size mono_size, hi_size color_size);
hi_void sample_hnr_mcf_start_mcf_vi_cfg(hi_mcf_grp mcf_grp, const hi_mcf_grp_attr *mcf_grp_attr);
hi_void sample_hnr_mcf_stop_mcf_vi_cfg(hi_mcf_grp mcf_grp);

hi_s32 sample_hnr_mcf_start_vpss(hi_vpss_grp grp, hi_size *in_size);
hi_void sample_hnr_mcf_stop_vpss(hi_vpss_grp grp);
hi_void sample_hnr_mcf_bind_vi(hi_mcf_grp mcf_grp, const hi_mcf_grp_attr *mcf_grp_attr, hi_vi_chn vi_chn);
hi_void sample_hnr_mcf_unbind_vi(hi_mcf_grp mcf_grp, const hi_mcf_grp_attr *mcf_grp_attr, hi_vi_chn vi_chn);

#endif
