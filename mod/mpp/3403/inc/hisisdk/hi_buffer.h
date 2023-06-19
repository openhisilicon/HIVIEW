/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */

#ifndef __HI_BUFFER_H__
#define __HI_BUFFER_H__

#include "hi_math.h"
#include "hi_type.h"
#include "hi_common.h"
#include "hi_common_video.h"
#include "ot_buffer.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef ot_pic_buf_attr hi_pic_buf_attr;
typedef ot_venc_buf_attr hi_venc_buf_attr;

__inline static hi_u32 hi_common_get_bit_width(hi_data_bit_width bit_width)
{
    return ot_common_get_bit_width(bit_width);
}

__inline static hi_void hi_copy_vb_calc_cfg(hi_vb_calc_cfg *dst_cfg, hi_vb_calc_cfg *src_cfg)
{
    return ot_copy_vb_calc_cfg(dst_cfg, src_cfg);
}

__inline static hi_bool hi_common_is_pixel_format_package_422(hi_pixel_format pixel_format)
{
    return ot_common_is_pixel_format_package_422(pixel_format);
}

__inline static hi_void hi_common_get_compress_none_pic_buf_cfg(hi_pic_buf_attr *buf_attr,
    hi_s32 bit_width, hi_vb_calc_cfg *calc_cfg)
{
    return ot_common_get_compress_none_pic_buf_cfg(buf_attr, bit_width, calc_cfg);
}

__inline static hi_void hi_common_get_compact_pic_buf_cfg(hi_pic_buf_attr *buf_attr, hi_mod_id mod_id,
    hi_u32 bit_width, hi_vb_calc_cfg *cfg)
{
    return ot_common_get_compact_pic_buf_cfg(buf_attr, mod_id, bit_width, cfg);
}

__inline static hi_void hi_common_get_compact_none_pic_buf_cfg(hi_pic_buf_attr *buf_attr, hi_vb_calc_cfg *cfg)
{
    return ot_common_get_compact_none_pic_buf_cfg(buf_attr, cfg);
}

__inline static hi_void hi_common_get_compress_pic_buf_cfg(hi_pic_buf_attr *buf_attr, hi_mod_id mod_id,
    hi_u32 bit_width, hi_vb_calc_cfg *calc_cfg)
{
    return ot_common_get_compress_pic_buf_cfg(buf_attr, mod_id, bit_width, calc_cfg);
}

__inline static hi_u32 hi_vi_get_raw_bit_width(hi_pixel_format pixel_format)
{
    return ot_vi_get_raw_bit_width(pixel_format);
}

__inline static hi_void hi_common_vi_get_raw_buf_cfg_with_compress_ratio(hi_pic_buf_attr *buf_attr,
    hi_u32 compress_ratio, hi_vb_calc_cfg *calc_cfg)
{
    return ot_common_vi_get_raw_buf_cfg_with_compress_ratio(buf_attr, compress_ratio, calc_cfg);
}

__inline static hi_void hi_common_vi_get_raw_buf_cfg(hi_pic_buf_attr *buf_attr, hi_vb_calc_cfg *calc_cfg)
{
    return ot_common_vi_get_raw_buf_cfg(buf_attr, calc_cfg);
}

__inline static hi_void hi_common_get_pic_buf_cfg(hi_pic_buf_attr *buf_attr, hi_vb_calc_cfg *calc_cfg)
{
    return ot_common_get_pic_buf_cfg(buf_attr, calc_cfg);
}

__inline static hi_void hi_common_get_vi_compact_seg_buf_size(hi_pic_buf_attr *buf_attr, hi_vb_calc_cfg *calc_cfg)
{
    return ot_common_get_vi_compact_seg_buf_size(buf_attr, calc_cfg);
}

__inline static hi_void hi_common_get_vpss_compact_seg_buf_size(hi_pic_buf_attr *buf_attr,
    hi_vb_calc_cfg *calc_cfg)
{
    return ot_common_get_vpss_compact_seg_buf_size(buf_attr, calc_cfg);
}

__inline static hi_u32 hi_common_get_pic_buf_size(hi_pic_buf_attr *buf_attr)
{
    return ot_common_get_pic_buf_size(buf_attr);
}

__inline static hi_u32 hi_hnr_size_align(hi_u32 size)
{
    return ot_hnr_heigh_align(size);
}

__inline static hi_u32 hi_hnr_get_pic_buf_size(hi_u32 width, hi_u32 height)
{
    return ot_hnr_get_pic_buf_size(width, height);
}

__inline static hi_bool hi_vdec_check_pic_size(hi_payload_type type, hi_u32 width, hi_u32 height)
{
    return ot_vdec_check_pic_size(type, width, height);
}

__inline static hi_u32 hi_vdec_get_pic_buf_size(hi_payload_type type, hi_pic_buf_attr *buf_attr)
{
    return ot_vdec_get_pic_buf_size(type, buf_attr);
}

__inline static hi_u32 hi_vdec_get_tmv_buf_size(hi_payload_type type, hi_u32 width, hi_u32 height)
{
    return ot_vdec_get_tmv_buf_size(type, width, height);
}

__inline static hi_bool hi_venc_check_pic_buf_size(hi_payload_type type, hi_venc_buf_attr *attr)
{
    return ot_venc_check_pic_buf_size(type, attr);
}

__inline static hi_u32 hi_venc_get_h264_pic_buf_size(hi_payload_type type, hi_venc_buf_attr *attr)
{
    return ot_venc_get_h264_pic_buf_size(type, attr);
}

__inline static hi_u32 hi_venc_get_h265_pic_buf_size(hi_payload_type type, hi_venc_buf_attr *attr)
{
    return ot_venc_get_h265_pic_buf_size(type, attr);
}

__inline static hi_u32 hi_venc_get_pic_buf_size(hi_payload_type type, hi_venc_buf_attr *attr)
{
    return ot_venc_get_pic_buf_size(type, attr);
}

__inline static hi_u32 hi_venc_get_pic_info_buf_size(hi_payload_type type, hi_venc_buf_attr *attr)
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

__inline static hi_bool hi_venc_is_mosaic_blk_valid(hi_u32 blk_size)
{
    return ot_venc_is_mosaic_blk_valid(blk_size);
}

__inline static hi_u32 hi_venc_get_mosaic_map_stride(hi_u32 width, hi_u32 blk_size)
{
    return ot_venc_get_mosaic_map_stride(width, blk_size);
}

__inline static hi_u32 hi_venc_get_mosaic_map_size(hi_u32 width, hi_u32 height, hi_u32 blk_size)
{
    return ot_venc_get_mosaic_map_size(width, height, blk_size);
}

__inline static hi_u32 hi_avs_get_buf_size(hi_u32 width, hi_u32 height)
{
    return ot_avs_get_buf_size(width, height);
}

__inline static hi_u32 hi_vpss_get_mcf_color_buf_size(hi_u32 width, hi_u32 height)
{
    return ot_vpss_get_mcf_color_buf_size(width, height);
}

__inline static hi_u32 hi_vpss_get_mcf_mono_buf_size(hi_u32 width, hi_u32 height)
{
    return ot_vpss_get_mcf_mono_buf_size(width, height);
}

#ifdef __cplusplus
}
#endif
#endif /* __HI_BUFFER_H__ */
