/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */

#ifndef HI_BUFFER_H
#define HI_BUFFER_H

#include "hi_math.h"
#include "hi_type.h"
#include "hi_common.h"
#include "hi_common_video.h"
#include "ot_buffer.h"

#ifdef __cplusplus
extern "C" {
#endif

__inline static hi_void hi_common_get_pic_buf_cfg(const hi_pic_buf_attr *buf_attr, hi_vb_calc_cfg *calc_cfg)
{
    return ot_common_get_pic_buf_cfg(buf_attr, calc_cfg);
}

__inline static hi_u32 hi_common_get_pic_buf_size(const hi_pic_buf_attr *buf_attr)
{
    return ot_common_get_pic_buf_size(buf_attr);
}

__inline static hi_void hi_vi_get_raw_buf_cfg_with_compress_ratio(const hi_pic_buf_attr *buf_attr,
    hi_u32 compress_ratio, hi_vb_calc_cfg *calc_cfg)
{
    return ot_vi_get_raw_buf_cfg_with_compress_ratio(buf_attr, compress_ratio, calc_cfg);
}

__inline static hi_u32 hi_aibnr_get_pic_buf_size(hi_u32 width, hi_u32 height)
{
    return ot_aibnr_get_pic_buf_size(width, height);
}

__inline static hi_u32 hi_aidrc_get_in_buf_size(hi_u32 width, hi_u32 height)
{
    return ot_aidrc_get_in_buf_size(width, height);
}

__inline static hi_u32 hi_aidrc_get_out_buf_size(hi_u32 width, hi_u32 height, hi_bool advanced_mode)
{
    return ot_aidrc_get_out_buf_size(width, height, advanced_mode);
}

__inline static hi_u32 hi_ai3dnr_get_pic_buf_size(hi_u32 width, hi_u32 height)
{
    return ot_ai3dnr_get_pic_buf_size(width, height);
}

__inline static hi_u32 hi_vdec_get_pic_buf_size(hi_payload_type type, const hi_pic_buf_attr *buf_attr)
{
    return ot_vdec_get_pic_buf_size(type, buf_attr);
}

__inline static hi_u32 hi_vdec_get_tmv_buf_size(hi_payload_type type, hi_u32 width, hi_u32 height)
{
    return ot_vdec_get_tmv_buf_size(type, width, height);
}

__inline static hi_u32 hi_venc_get_pic_buf_size(hi_payload_type type, const hi_venc_buf_attr *attr)
{
    return ot_venc_get_pic_buf_size(type, attr);
}

__inline static hi_u32 hi_venc_get_pic_info_buf_size(hi_payload_type type, const hi_venc_buf_attr *attr)
{
    return ot_venc_get_pic_info_buf_size(type, attr);
}

__inline static hi_u32 hi_venc_get_qpmap_stride(hi_u32 width)
{
    return ot_venc_get_qpmap_stride(width);
}

__inline static hi_u32 hi_venc_get_qpmap_size(hi_payload_type type, hi_u32 width, hi_u32 height)
{
    return ot_venc_get_qpmap_size(type, width, height);
}

__inline static hi_u32 hi_venc_get_skip_weight_stride(hi_payload_type type, hi_u32 width)
{
    return ot_venc_get_skip_weight_stride(type, width);
}

__inline static hi_u32 hi_venc_get_skip_weight_size(hi_payload_type type, hi_u32 width, hi_u32 height)
{
    return ot_venc_get_skip_weight_size(type, width, height);
}

__inline static hi_u32 hi_venc_get_roimap_stride(hi_payload_type type, hi_u32 width)
{
    return ot_venc_get_roimap_stride(type, width);
}

__inline static hi_u32 hi_venc_get_roimap_size(hi_payload_type type, hi_u32 width, hi_u32 height)
{
    return ot_venc_get_roimap_size(type, width, height);
}

__inline static hi_u32 hi_venc_get_mosaic_map_stride(hi_u32 width, hi_u32 blk_size)
{
    return ot_venc_get_mosaic_map_stride(width, blk_size);
}

__inline static hi_u32 hi_venc_get_mosaic_map_size(hi_u32 width, hi_u32 height, hi_u32 blk_size)
{
    return ot_venc_get_mosaic_map_size(width, height, blk_size);
}

__inline static hi_u32 hi_vpss_get_mcf_color_buf_size(hi_u32 width, hi_u32 height)
{
    return ot_vpss_get_mcf_color_buf_size(width, height);
}

__inline static hi_u32 hi_vpss_get_mcf_mono_buf_size(hi_u32 width, hi_u32 height)
{
    return ot_vpss_get_mcf_mono_buf_size(width, height);
}

__inline static hi_u32 hi_gdc_get_lut_buf_stride(hi_u32 width, hi_lut_cell_size cell_size)
{
    return ot_gdc_get_lut_buf_stride(width, cell_size);
}

__inline static hi_u32 hi_common_get_fmu_wrap_page_num(hi_fmu_id id, hi_u32 width, hi_u32 height)
{
    return ot_common_get_fmu_wrap_page_num(id, width, height);
}

#ifdef __cplusplus
}
#endif
#endif /* HI_BUFFER_H */
