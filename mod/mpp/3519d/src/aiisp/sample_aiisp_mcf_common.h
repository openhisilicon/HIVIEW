/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */

#ifndef SAMPLE_AI3DNR_MCF_COMMON_H
#define SAMPLE_AI3DNR_MCF_COMMON_H

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
#include "hi_mpi_sys.h"
#include "hi_mpi_vpss.h"
#include "hi_mpi_mcf.h"
#include "sample_comm.h"

hi_void sample_aiisp_mcf_get_default_grp_attr(hi_mcf_grp_attr *grp_attr, hi_size *mono_size,
    hi_size *color_size);
hi_void sample_aiisp_mcf_get_large_size(hi_size mono_size, hi_size color_size, hi_size *large_size);
hi_void sample_aiisp_mcf_set_chn_defualt_outsize(hi_size out_size);
hi_s32 sample_aiisp_mcf_start_vi(const hi_mcf_grp_attr *mcf_grp_attr, sample_vi_cfg *vi_cfg1,
    sample_vi_cfg *vi_cfg0, hi_size *in_size);
hi_void sample_aiisp_mcf_stop_vi(hi_vi_pipe vi_color_pipe, sample_vi_cfg *vi_cfg1, sample_vi_cfg *vi_cfg0);
hi_s32 sample_aiisp_start_mcf_data_flow(hi_mcf_grp mcf_grp, hi_mcf_grp_attr *mcf_grp_attr,
    hi_size *out_size, hi_mcf_crop_info *grp_crop_info);
hi_void sample_aiisp_stop_mcf_data_flow(hi_mcf_grp mcf_grp, hi_mcf_grp_attr *mcf_grp_attr);
#endif
