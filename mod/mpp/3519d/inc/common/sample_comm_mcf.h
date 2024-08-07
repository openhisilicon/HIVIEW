/*
  Copyright (c), 2001-2024, Shenshu Tech. Co., Ltd.
 */

#ifndef __SAMPLE_COMM_MCF_H__
#define __SAMPLE_COMM_MCF_H__
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
#include "hi_common_mcf_calibration.h"

typedef struct {
    hi_char *input_file_name;
    hi_size input_img_size;
    hi_char *output_file_name;
    hi_size output_img_size;
} sample_comm_mcf_scale_img_param;

typedef struct {
    hi_u32 color_cnt;
    hi_u32 mono_cnt;
} sample_yuv_cnt;

#define VI_MONO_PIPE    0
#define VI_COLOR_PIPE   1
#define MCF_VPSS_MAX_WIDTH   2688
#define MCF_VPSS_MAX_HEIGHT   1520

hi_void sample_common_mcf_get_default_grp_attr(hi_mcf_grp_attr *grp_attr, hi_size *mono_size, hi_size *color_size);
hi_s32 sample_common_vi_bind_mcf(hi_vi_pipe vi_pipe, hi_vi_chn vi_chn, hi_mcf_grp mcf_grp, hi_mcf_pipe mcf_pipe);
hi_s32 sample_common_vi_unbind_mcf(hi_vi_pipe vi_pipe, hi_vi_chn vi_chn, hi_mcf_grp mcf_grp, hi_mcf_pipe mcf_pipe);
hi_s32 sample_common_mcf_bind_vo(hi_mcf_grp mcf_grp, hi_mcf_chn mcf_chn, hi_vo_layer vo_layer, hi_vo_chn vo_chn);
hi_s32 sample_common_mcf_unbind_vo(hi_mcf_grp mcf_grp, hi_mcf_chn mcf_chn, hi_vo_layer vo_layer, hi_vo_chn vo_chn);
hi_s32 sample_common_mcf_bind_venc(hi_mcf_grp mcf_grp, hi_mcf_chn mcf_chn, hi_venc_chn venc_chn);
hi_s32 sample_common_mcf_unbind_venc(hi_mcf_grp mcf_grp, hi_mcf_chn mcf_chn, hi_venc_chn venc_chn);

hi_s32 sample_common_mcf_start(hi_mcf_grp grp, const hi_mcf_grp_attr *grp_attr, hi_mcf_crop_info *grp_crop_info,
                               hi_bool *mcf_chn_en, hi_size *chn_out_size);
hi_void sample_common_mcf_get_default_chn_attr(hi_mcf_chn_attr *chn_attr, hi_size *out_size);

hi_s32 sample_common_mcf_stop_vpss_one_grp(hi_vpss_grp grp);
hi_s32 sample_common_mcf_stop_vpss(const hi_mcf_grp_attr *grp_attr);
hi_s32 sample_common_mcf_stop(hi_mcf_grp grp, const hi_bool *chn_enable, hi_u32 chn_array_size);
hi_s32 sample_common_mcf_start_vpss(const hi_mcf_grp_attr *mcf_grp_attr, hi_u32 color_cnt, hi_u32 mono_cnt,
    hi_fov_attr *fov_correction_attr);
hi_s32 sample_common_mcf_bind_vpss(hi_mcf_grp mcf_grp, hi_mcf_chn mcf_chn, hi_vpss_grp vpss_grp, hi_vpss_chn vpss_chn);
hi_s32 sample_common_mcf_unbind_vpss(hi_mcf_grp mcf_grp, hi_mcf_chn mcf_chn,
    hi_vpss_grp vpss_grp, hi_vpss_chn vpss_chn);
hi_void sample_common_mcf_set_default_vpss_attr(hi_vpss_grp_attr *vpss_grp_attr,
                                                const hi_mcf_grp_attr *mcf_grp_attr, hi_bool is_mono_pipe);
#endif
