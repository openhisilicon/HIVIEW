/*
  Copyright (c), 2001-2024, Shenshu Tech. Co., Ltd.
 */

#ifndef OT_BUFFER_DETAIL_H
#define OT_BUFFER_DETAIL_H

#include "securec.h"
#ifdef __KERNEL__
#include "ot_osal.h"
#endif
#include "ot_type.h"
#include "ot_common.h"
#include "ot_common_video.h"
#include "ot_math.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define OT_MAXI_NUM_LIMIT 30000

#define OT_SEG_RATIO_8BIT_LUMA      1300
#define OT_SEG_RATIO_8BIT_CHROMA    2000

#define OT_SEG_WIDTH_BIT_ALIGN 128
#define OT_FMU_MIN_BUF_LINE    32

static td_void ot_common_get_pic_buf_cfg(const ot_pic_buf_attr *buf_attr, ot_vb_calc_cfg *calc_cfg);

__inline static td_void ot_common_get_fmap_compress_buf_cfg(const ot_pic_buf_attr *buf_attr, ot_vb_calc_cfg *calc_cfg)
{
    td_u32 chn_num;
    td_u32 width, height;
    td_u32 main_stride, main_size, y_size, c_size;

    if (calc_cfg == TD_NULL) {
        return;
    }
    if ((buf_attr == TD_NULL) || (buf_attr->width > OT_MAXI_NUM_LIMIT) || (buf_attr->height > OT_MAXI_NUM_LIMIT)) {
        (td_void)memset_s(calc_cfg, sizeof(*calc_cfg), 0, sizeof(*calc_cfg));
        return;
    }

    if (buf_attr->pixel_format == OT_PIXEL_FORMAT_YUV_400) {
        chn_num = 4; /* 4 chn */
        width = buf_attr->width;
        height = buf_attr->height;
    } else if ((buf_attr->pixel_format >= OT_PIXEL_FORMAT_RGB_BAYER_8BPP) &&
               (buf_attr->pixel_format <= OT_PIXEL_FORMAT_RGB_BAYER_16BPP)) {
        chn_num = 8; /* 8 chn */
        width = buf_attr->width / 4;
        height = buf_attr->height;
    } else if ((buf_attr->pixel_format == OT_PIXEL_FORMAT_YVU_SEMIPLANAR_420) ||
               (buf_attr->pixel_format == OT_PIXEL_FORMAT_YUV_SEMIPLANAR_420)) {
        chn_num = 4; /* 4 chn */
        width = buf_attr->width / 4;
        height = buf_attr->height;
    } else { /* RGB */
        chn_num = 8; /* 8 chn */
        width = buf_attr->width / 2;
        height = buf_attr->height;
    }

    main_stride = 24 + chn_num * OT_DIV_UP(width, 32) + chn_num * width * 8 + (chn_num / 2) * 64;
    main_stride = OT_DIV_UP(main_stride, 128) * 16;

    y_size = main_stride * height;
    if ((buf_attr->pixel_format == OT_PIXEL_FORMAT_YVU_SEMIPLANAR_420) ||
        (buf_attr->pixel_format == OT_PIXEL_FORMAT_YUV_SEMIPLANAR_420)) {
        c_size = y_size / 2;
    } else {
        c_size = 0;
    }
    main_size = y_size + c_size;

    calc_cfg->vb_size = main_size;
    calc_cfg->head_stride = 0;
    calc_cfg->head_size = 0;
    calc_cfg->head_y_size = 0;
    calc_cfg->main_stride = main_stride;
    calc_cfg->main_size = main_size;
    calc_cfg->main_y_size = y_size;
}

__inline static td_void ot_common_get_tile32x4_yuv_buf_cfg(const ot_pic_buf_attr *buf_attr, ot_vb_calc_cfg *calc_cfg)
{
    td_u32 width, height;

    if (calc_cfg == TD_NULL) {
        return;
    }

    (td_void)memset_s(calc_cfg, sizeof(*calc_cfg), 0, sizeof(*calc_cfg));

    if ((buf_attr == TD_NULL) || (buf_attr->width > OT_MAXI_NUM_LIMIT) || (buf_attr->height > OT_MAXI_NUM_LIMIT)) {
        return;
    }

    width = buf_attr->width;
    height = buf_attr->height;

    calc_cfg->main_stride = OT_ALIGN_UP(width, 32);
    if (buf_attr->compress_mode == OT_COMPRESS_MODE_NONE) {
        calc_cfg->vb_size = calc_cfg->main_stride * OT_ALIGN_UP(height, 4) * 2;
        return;
    }

    calc_cfg->vb_size = calc_cfg->main_stride * OT_ALIGN_UP(height, 4);
}

__inline static td_bool ot_common_is_pixel_format_rgb_bayer(ot_pixel_format pixel_format)
{
    if ((pixel_format == OT_PIXEL_FORMAT_RGB_BAYER_8BPP) || (pixel_format == OT_PIXEL_FORMAT_RGB_BAYER_10BPP) ||
        (pixel_format == OT_PIXEL_FORMAT_RGB_BAYER_12BPP) || (pixel_format == OT_PIXEL_FORMAT_RGB_BAYER_14BPP) ||
        (pixel_format == OT_PIXEL_FORMAT_RGB_BAYER_16BPP)) {
        return TD_TRUE;
    }
    return TD_FALSE;
}

__inline static td_u32 ot_common_get_bit_width(ot_data_bit_width bit_width)
{
    switch (bit_width) {
        case OT_DATA_BIT_WIDTH_8:
            return 8;
        case OT_DATA_BIT_WIDTH_10:
            return 10;
        case OT_DATA_BIT_WIDTH_12:
            return 12;
        case OT_DATA_BIT_WIDTH_14:
            return 14;
        case OT_DATA_BIT_WIDTH_16:
            return 16;
        case OT_DATA_BIT_WIDTH_32:
            return 32;
        case OT_DATA_BIT_WIDTH_64:
            return 64;
        default:
            return 0;
    }
}

__inline static td_u32 ot_vi_get_raw_bit_width(ot_pixel_format pixel_format)
{
    switch (pixel_format) {
        case OT_PIXEL_FORMAT_RGB_BAYER_8BPP:
            return 8; /* 8:single pixel width */
        case OT_PIXEL_FORMAT_RGB_BAYER_10BPP:
            return 10; /* 10:single pixel width */
        case OT_PIXEL_FORMAT_RGB_BAYER_12BPP:
            return 12; /* 12:single pixel width */
        case OT_PIXEL_FORMAT_RGB_BAYER_14BPP:
            return 14; /* 14:single pixel width */
        case OT_PIXEL_FORMAT_RGB_BAYER_16BPP:
            return 16; /* 16:single pixel width */
        default:
            return 0;
    }
}

__inline static td_u32 ot_common_get_valid_align(td_u32 align)
{
    /* align: 0 is automatic mode, alignment size following system. Non-0 for specified alignment size */
    if (align == 0) {
        return OT_DEFAULT_ALIGN;
    } else if (align > OT_MAX_ALIGN) {
        return OT_MAX_ALIGN;
    } else {
        return OT_ALIGN_UP(align, OT_DEFAULT_ALIGN);
    }
}

__inline static td_void ot_common_get_raw_buf_cfg_with_compress_ratio(const ot_pic_buf_attr *buf_attr,
    td_u32 compress_ratio, ot_vb_calc_cfg *calc_cfg)
{
    td_u32 width, height, bit_width;
    td_u32 align;
    td_u32 stride = 0;
    td_u32 size = 0;
    td_u32 raw_compress_ratio;

    if (calc_cfg == TD_NULL) {
        return;
    }
    if ((buf_attr == TD_NULL) || (buf_attr->width > OT_MAXI_NUM_LIMIT) || (buf_attr->height > OT_MAXI_NUM_LIMIT)) {
        (td_void)memset_s(calc_cfg, sizeof(*calc_cfg), 0, sizeof(*calc_cfg));
        return;
    }

    width = buf_attr->width;
    height = buf_attr->height;
    bit_width = ot_vi_get_raw_bit_width(buf_attr->pixel_format);
    align = ot_common_get_valid_align(buf_attr->align);

    if (buf_attr->compress_mode == OT_COMPRESS_MODE_NONE) {
        stride = OT_ALIGN_UP(OT_ALIGN_UP(width * bit_width, 8) / 8, align);
        size = stride * height;
    } else if (buf_attr->compress_mode == OT_COMPRESS_MODE_LINE) {
        if ((OT_DIV_UP(width, 32) * height) <= 4096) {
            stride = (16 + width * bit_width + (width + 31) / 32 + 127) / 128;
        } else {
            raw_compress_ratio = 1538;
            stride = (width * bit_width * 1000UL / raw_compress_ratio + 8192 + 127) / 128;
        }
        stride = OT_ALIGN_UP(OT_ALIGN_UP(stride, 2) * 16, align);
        size = stride * height;
    } else if (buf_attr->compress_mode == OT_COMPRESS_MODE_FRAME) {
        if ((OT_DIV_UP(width, 32) * height) <= 4096) {
            size = (height * width * bit_width * 3 / 2 + (width + 31) / 32 * height + 255) / 256 * 2 * 16;
        } else {
            raw_compress_ratio = (compress_ratio == 0) ? 2000 : compress_ratio;
            size = (height * width * bit_width * 1000UL / raw_compress_ratio + 255) / 256 * 2 * 16;
        }
        size = OT_ALIGN_UP(size, align);
    } else { /* not support */
    }

    calc_cfg->vb_size     = size;
    calc_cfg->head_stride = 0;
    calc_cfg->head_size   = 0;
    calc_cfg->head_y_size = 0;
    calc_cfg->main_stride = stride;
    calc_cfg->main_size   = size;
    calc_cfg->main_y_size = size;
}

__inline static td_void ot_common_get_raw_buf_cfg(const ot_pic_buf_attr *buf_attr, ot_vb_calc_cfg *calc_cfg)
{
    /* use default frame compress ratio */
    ot_common_get_raw_buf_cfg_with_compress_ratio(buf_attr, 0, calc_cfg);
}

__inline static td_bool ot_common_is_pixel_format_package_422(ot_pixel_format pixel_format)
{
    if ((pixel_format == OT_PIXEL_FORMAT_YUYV_PACKAGE_422) || (pixel_format == OT_PIXEL_FORMAT_YVYU_PACKAGE_422) ||
        (pixel_format == OT_PIXEL_FORMAT_UYVY_PACKAGE_422) || (pixel_format == OT_PIXEL_FORMAT_VYUY_PACKAGE_422) ||
        (pixel_format == OT_PIXEL_FORMAT_YYUV_PACKAGE_422) || (pixel_format == OT_PIXEL_FORMAT_YYVU_PACKAGE_422) ||
        (pixel_format == OT_PIXEL_FORMAT_UVYY_PACKAGE_422) || (pixel_format == OT_PIXEL_FORMAT_VUYY_PACKAGE_422) ||
        (pixel_format == OT_PIXEL_FORMAT_VY1UY0_PACKAGE_422)) {
        return TD_TRUE;
    }
    return TD_FALSE;
}

__inline static td_void ot_common_get_uncompressed_yuv_buf_cfg(const ot_pic_buf_attr *buf_attr,
    ot_vb_calc_cfg *calc_cfg)
{
    td_u32 align_height, bit_width, size, y_size;
    td_u32 align, stride;

    if (calc_cfg == TD_NULL) {
        return;
    }
    if ((buf_attr == TD_NULL) || (buf_attr->width > OT_MAXI_NUM_LIMIT) || (buf_attr->height > OT_MAXI_NUM_LIMIT)) {
        (td_void)memset_s(calc_cfg, sizeof(*calc_cfg), 0, sizeof(*calc_cfg));
        return;
    }

    align_height = OT_ALIGN_UP(buf_attr->height, 2);
    bit_width = ot_common_get_bit_width(buf_attr->bit_width);
    align = ot_common_get_valid_align(buf_attr->align);

    stride = OT_ALIGN_UP((buf_attr->width * bit_width + 7) >> 3, align);
    y_size = stride * align_height;
    if ((buf_attr->pixel_format == OT_PIXEL_FORMAT_YVU_SEMIPLANAR_420) ||
        (buf_attr->pixel_format == OT_PIXEL_FORMAT_YUV_SEMIPLANAR_420)) {
        size = (y_size * 3) >> 1;
    } else if ((buf_attr->pixel_format == OT_PIXEL_FORMAT_YVU_SEMIPLANAR_422) ||
        (buf_attr->pixel_format == OT_PIXEL_FORMAT_YUV_SEMIPLANAR_422)) {
        size = y_size * 2;
    } else if ((buf_attr->pixel_format == OT_PIXEL_FORMAT_YUV_400) ||
        (buf_attr->pixel_format == OT_PIXEL_FORMAT_S8C1) || (buf_attr->pixel_format == OT_PIXEL_FORMAT_U8C1) ||
        (buf_attr->pixel_format == OT_PIXEL_FORMAT_S16C1) || (buf_attr->pixel_format == OT_PIXEL_FORMAT_U16C1) ||
        (buf_attr->pixel_format == OT_PIXEL_FORMAT_S32C1) || (buf_attr->pixel_format == OT_PIXEL_FORMAT_U32C1) ||
        (buf_attr->pixel_format == OT_PIXEL_FORMAT_S64C1) || (buf_attr->pixel_format == OT_PIXEL_FORMAT_U64C1)) {
        size = y_size;
    } else if (buf_attr->pixel_format == OT_PIXEL_FORMAT_UV_420) {
        size = y_size >> 1;
    } else if (ot_common_is_pixel_format_package_422(buf_attr->pixel_format) == TD_TRUE) {
        stride *= 2;
        size = y_size * 2;
    } else {
        size = y_size * 3;
    }

    calc_cfg->vb_size     = size;
    calc_cfg->head_stride = 0;
    calc_cfg->head_size   = 0;
    calc_cfg->head_y_size = 0;
    calc_cfg->main_stride = stride;
    calc_cfg->main_size   = size;
    calc_cfg->main_y_size = y_size;
}

__inline static td_u32 ot_common_get_yuv_head_stride(td_u32 width, ot_compress_mode compress_mode)
{
    if (compress_mode == OT_COMPRESS_MODE_SEG_COMPACT) {
        return 32;
    }
    if (width <= 4096) {
        return 16;
    } else if (width <= 8192) {
        return 32;
    } else {
        return 64;
    }
}

__inline static td_u32 ot_common_get_seg_compress_align_height_chroma(td_u32 align_height,
    ot_pixel_format pixel_format)
{
    if ((pixel_format == OT_PIXEL_FORMAT_YVU_SEMIPLANAR_420) ||
        (pixel_format == OT_PIXEL_FORMAT_YUV_SEMIPLANAR_420)) {
        return (align_height >> 1);
    } else if ((pixel_format == OT_PIXEL_FORMAT_YVU_SEMIPLANAR_422) ||
        (pixel_format == OT_PIXEL_FORMAT_YUV_SEMIPLANAR_422)) {
        return align_height;
    } else if (pixel_format == OT_PIXEL_FORMAT_YUV_400) {
        return 0;
    } else {
        return (align_height * 2);
    }
}

__inline static td_void ot_common_get_compact_seg_compress_yuv_buf_cfg(const ot_pic_buf_attr *buf_attr,
    ot_vb_calc_cfg *calc_cfg)
{
    td_u32 align_height, align_c_height, bit_width, head_size, head_y_size, y_size, c_size;
    td_u32 align, head_stride, stride;
    const td_u32 cmp_ratio_luma = OT_SEG_RATIO_8BIT_LUMA;
    const td_u32 cmp_ratio_chroma = OT_SEG_RATIO_8BIT_CHROMA;

    if (calc_cfg == TD_NULL) {
        return;
    }
    if ((buf_attr == TD_NULL) || (buf_attr->width > OT_MAXI_NUM_LIMIT) || (buf_attr->height > OT_MAXI_NUM_LIMIT)) {
        (td_void)memset_s(calc_cfg, sizeof(*calc_cfg), 0, sizeof(*calc_cfg));
        return;
    }

    align_height = OT_ALIGN_UP(buf_attr->height, 2);
    align_c_height = ot_common_get_seg_compress_align_height_chroma(align_height, buf_attr->pixel_format);
    bit_width = ot_common_get_bit_width(buf_attr->bit_width);
    align = ot_common_get_valid_align(buf_attr->align);

    head_stride = ot_common_get_yuv_head_stride(buf_attr->width, buf_attr->compress_mode);
    head_y_size = head_stride * align_height;
    head_size = OT_ALIGN_UP(head_stride * (align_height + align_c_height), align);

    stride = OT_ALIGN_UP(buf_attr->width * bit_width, OT_SEG_WIDTH_BIT_ALIGN);
#ifdef __KERNEL__
    y_size = OT_ALIGN_UP(osal_div64_u64(stride * align_height * 1000ULL, cmp_ratio_luma * 8), align);
    c_size = OT_ALIGN_UP(osal_div64_u64(stride * align_c_height * 1000ULL, cmp_ratio_chroma * 8), align);
#else
    y_size = OT_ALIGN_UP(stride * align_height * 1000ULL / (cmp_ratio_luma * 8), align);
    c_size = OT_ALIGN_UP(stride * align_c_height * 1000ULL / (cmp_ratio_chroma * 8), align);
#endif

    calc_cfg->vb_size     = head_size + y_size + c_size;
    calc_cfg->head_stride = head_stride;
    calc_cfg->head_size   = head_size;
    calc_cfg->head_y_size = head_y_size;
    calc_cfg->main_stride = stride >> 3;
    calc_cfg->main_size   = y_size + c_size;
    calc_cfg->main_y_size = y_size;
}

__inline static td_void ot_common_get_seg_compress_yuv_buf_cfg(const ot_pic_buf_attr *buf_attr,
    ot_vb_calc_cfg *calc_cfg)
{
    td_u32 align_height, bit_width, head_size, head_y_size, size, y_size;
    td_u32 align, head_stride, stride;

    if (calc_cfg == TD_NULL) {
        return;
    }
    if ((buf_attr == TD_NULL) || (buf_attr->width > OT_MAXI_NUM_LIMIT) || (buf_attr->height > OT_MAXI_NUM_LIMIT)) {
        (td_void)memset_s(calc_cfg, sizeof(*calc_cfg), 0, sizeof(*calc_cfg));
        return;
    }

    align_height = OT_ALIGN_UP(buf_attr->height, 2);
    bit_width = ot_common_get_bit_width(buf_attr->bit_width);
    align = ot_common_get_valid_align(buf_attr->align);

    head_stride = ot_common_get_yuv_head_stride(buf_attr->width, buf_attr->compress_mode);
    head_y_size = head_stride * align_height;
    stride = OT_ALIGN_UP((buf_attr->width * bit_width + 7) >> 3, align);
    y_size = stride * align_height;
    if ((buf_attr->pixel_format == OT_PIXEL_FORMAT_YVU_SEMIPLANAR_420) ||
        (buf_attr->pixel_format == OT_PIXEL_FORMAT_YUV_SEMIPLANAR_420)) {
        head_size = (head_y_size * 3) >> 1;
        size = (y_size * 3) >> 1;
    } else if ((buf_attr->pixel_format == OT_PIXEL_FORMAT_YVU_SEMIPLANAR_422) ||
        (buf_attr->pixel_format == OT_PIXEL_FORMAT_YUV_SEMIPLANAR_422)) {
        head_size = head_y_size * 2;
        size = y_size * 2;
    } else if (buf_attr->pixel_format == OT_PIXEL_FORMAT_YUV_400) {
        head_size = head_y_size;
        size = y_size;
    } else {
        head_size = head_y_size * 3;
        size = y_size * 3;
    }
    head_size = OT_ALIGN_UP(head_size, align);

    calc_cfg->vb_size     = head_size + size;
    calc_cfg->head_stride = head_stride;
    calc_cfg->head_size   = head_size;
    calc_cfg->head_y_size = head_y_size;
    calc_cfg->main_stride = stride;
    calc_cfg->main_size   = size;
    calc_cfg->main_y_size = y_size;
}

__inline static td_void ot_common_get_yuv_buf_cfg(const ot_pic_buf_attr *buf_attr, ot_vb_calc_cfg *calc_cfg)
{
    if (calc_cfg == TD_NULL) {
        return;
    }
    if (buf_attr == TD_NULL) {
        (td_void)memset_s(calc_cfg, sizeof(*calc_cfg), 0, sizeof(*calc_cfg));
        return;
    }

    if (buf_attr->compress_mode == OT_COMPRESS_MODE_NONE) {
        ot_common_get_uncompressed_yuv_buf_cfg(buf_attr, calc_cfg);
    } else if ((buf_attr->compress_mode == OT_COMPRESS_MODE_SEG_COMPACT) && (buf_attr->width > 256)) {
        /* compact seg compress */
        ot_common_get_compact_seg_compress_yuv_buf_cfg(buf_attr, calc_cfg);
    } else {
        /* seg compress */
        ot_common_get_seg_compress_yuv_buf_cfg(buf_attr, calc_cfg);
    }
}

__inline static td_u32 ot_aibnr_get_vb_size(td_u32 width, td_u32 height, ot_compress_mode compress_mode)
{
    ot_pic_buf_attr buf_attr = {0};
    ot_vb_calc_cfg calc_cfg = {0};

    buf_attr.width         = OT_ALIGN_UP(width, 4); /* align up to 4 */
    buf_attr.height        = OT_ALIGN_UP(height, 4); /* align up to 4 */
    buf_attr.align         = OT_DEFAULT_ALIGN;
    buf_attr.bit_width     = OT_DATA_BIT_WIDTH_8;
    buf_attr.pixel_format  = OT_PIXEL_FORMAT_RGB_BAYER_12BPP;
    buf_attr.compress_mode = compress_mode;
    buf_attr.video_format  = OT_VIDEO_FORMAT_LINEAR;
    ot_common_get_pic_buf_cfg(&buf_attr, &calc_cfg);

    return calc_cfg.vb_size;
}

__inline static td_u32 ot_aidrc_get_in_vb_size(td_u32 width, td_u32 height, ot_compress_mode compress_mode)
{
    ot_pic_buf_attr buf_attr = {0};
    ot_vb_calc_cfg calc_cfg = {0};

    buf_attr.width         = OT_ALIGN_UP(width, 4); /* align up to 4 */
    buf_attr.height        = OT_ALIGN_UP(height, 4); /* align up to 4 */
    buf_attr.align         = OT_DEFAULT_ALIGN;
    buf_attr.bit_width     = OT_DATA_BIT_WIDTH_8;
    buf_attr.pixel_format  = OT_PIXEL_FORMAT_RGB_BAYER_16BPP;
    buf_attr.compress_mode = compress_mode;
    buf_attr.video_format  = OT_VIDEO_FORMAT_LINEAR;
    ot_common_get_pic_buf_cfg(&buf_attr, &calc_cfg);

    return calc_cfg.vb_size;
}

__inline static td_void ot_aidrc_get_detail_buf_cfg(td_u32 width, td_u32 height, ot_vb_calc_cfg *calc_cfg)
{
    ot_pic_buf_attr buf_attr = {0};

    buf_attr.width         = OT_ALIGN_UP(width, 2); /* align up to 2 */
    buf_attr.height        = OT_ALIGN_UP(height, 2); /* align up to 2 */
    buf_attr.align         = OT_DEFAULT_ALIGN;
    buf_attr.bit_width     = OT_DATA_BIT_WIDTH_8;
    buf_attr.pixel_format  = OT_PIXEL_FORMAT_RGB_BAYER_8BPP;
    buf_attr.compress_mode = OT_COMPRESS_MODE_NONE;
    buf_attr.video_format  = OT_VIDEO_FORMAT_LINEAR;

    ot_common_get_pic_buf_cfg(&buf_attr, calc_cfg);
}

__inline static td_u32 ot_aidrc_get_coef_buf_size(td_u32 width, td_u32 height)
{
    td_u32 coef_size, coef_min_size;
    const td_u32 down_sample_factor = 32;
    const td_u32 channels = 112;

    width = OT_ALIGN_UP(width, 2); /* align up to 2 */
    height = OT_ALIGN_UP(height, 2); /* align up to 2 */
    coef_size = OT_DIV_UP(width, down_sample_factor) * OT_DIV_UP(height, down_sample_factor) * channels;
    coef_min_size = 60 * 34 * channels; /* min size: 60 x 34 */

    return MAX2(coef_size, coef_min_size);
}

__inline static td_u32 ot_ai3dnr_get_vb_size(td_u32 width, td_u32 height, ot_compress_mode compress_mode)
{
    ot_pic_buf_attr buf_attr = {0};
    ot_vb_calc_cfg calc_cfg = {0};

    buf_attr.width         = OT_ALIGN_UP(width, 4); /* align up to 4 */
    buf_attr.height        = OT_ALIGN_UP(height, 4); /* align up to 4 */
    buf_attr.align         = OT_DEFAULT_ALIGN;
    buf_attr.bit_width     = OT_DATA_BIT_WIDTH_8;
    buf_attr.pixel_format  = OT_PIXEL_FORMAT_YVU_SEMIPLANAR_420;
    buf_attr.compress_mode = compress_mode;
    buf_attr.video_format  = OT_VIDEO_FORMAT_LINEAR;
    ot_common_get_pic_buf_cfg(&buf_attr, &calc_cfg);

    return calc_cfg.vb_size;
}

__inline static td_bool ot_vdec_check_pic_size(ot_payload_type type, td_u32 width, td_u32 height)
{
    if ((type == OT_PT_H265) && (width <= OT_H265D_MAX_WIDTH) && (height <= OT_H265D_MAX_HEIGHT)) {
        return TD_TRUE;
    }
    if ((type == OT_PT_H264) && (width <= OT_H264D_MAX_WIDTH) && (height <= OT_H264D_MAX_HEIGHT)) {
        return TD_TRUE;
    }
    if ((type == OT_PT_JPEG || type == OT_PT_MJPEG) &&
        (width <= OT_JPEGD_MAX_WIDTH) && (height <= OT_JPEGD_MAX_HEIGHT)) {
        return TD_TRUE;
    }
    return TD_FALSE;
}

__inline static td_bool ot_venc_check_pic_buf_size(ot_payload_type type, const ot_venc_buf_attr *attr)
{
    if (type != OT_PT_H264 && type != OT_PT_H265) {
        return TD_FALSE;
    }

    if (attr == NULL) {
        return TD_FALSE;
    }

    if (attr->share_buf_en != TD_FALSE && attr->share_buf_en != TD_TRUE) {
        return TD_FALSE;
    }

    if (attr->frame_buf_ratio < 70 || attr->frame_buf_ratio > 100) { /* 70 100: algorithm param */
        return TD_FALSE;
    }

    if (type == OT_PT_H264 &&
        (attr->pic_buf_attr.width > OT_VENC_H264_MAX_WIDTH || attr->pic_buf_attr.height > OT_VENC_H264_MAX_HEIGHT)) {
        return TD_FALSE;
    }

    if (type == OT_PT_H265 &&
        (attr->pic_buf_attr.width > OT_VENC_H265_MAX_WIDTH || attr->pic_buf_attr.height > OT_VENC_H265_MAX_HEIGHT)) {
        return TD_FALSE;
    }

    if (attr->pic_buf_attr.bit_width != OT_DATA_BIT_WIDTH_8) {
        return TD_FALSE;
    }

    return TD_TRUE;
}

__inline static td_u32 ot_venc_get_h264_pic_buf_size(ot_payload_type type, const ot_venc_buf_attr *attr)
{
    td_u32 y_header_stride, c_header_stride, y_stride, c_stride;
    td_u32 y_header_size, c_header_size;
    td_u32 y_size, c_size, y_blk_size, c_blk_size;
    td_u32 blk_height_y, blk_height_c;
    td_u32 width, height;
    td_bool share_buf;
    td_u32 frame_buf_ratio, frame_buf_ratio_y, frame_buf_ratio_c;
    td_u32 pic_size;

    width = attr->pic_buf_attr.width;
    height = attr->pic_buf_attr.height;
    share_buf = attr->share_buf_en;
    frame_buf_ratio = attr->frame_buf_ratio;

    if (width <= 2048) { /* 2048: pic width */
        y_header_stride = 128; /* 128: header stride */
    } else if (width <= 4096) { /* 4096: pic width */
        y_header_stride = 256; /* 256: header stride */
    } else if (width <= 6144) { /* 6144: pic width */
        y_header_stride = 384; /* 384: header stride */
    } else {
        y_header_stride = 512; /* 512: header stride */
    }

    if (frame_buf_ratio >= 70 && frame_buf_ratio < 100) { /* 70 100: buf ratio */
        frame_buf_ratio_y = frame_buf_ratio + 10; /* 10: buf ratio */
        frame_buf_ratio_c = frame_buf_ratio - 20; /* 20: buf ratio */
    } else if (frame_buf_ratio >= 60 && frame_buf_ratio < 70) { /* 60 70: buf ratio */
        frame_buf_ratio_y = frame_buf_ratio + 5; /* 5: buf ratio */
        frame_buf_ratio_c = frame_buf_ratio - 10; /* 10: buf ratio */
    } else {
        frame_buf_ratio_y = frame_buf_ratio;
        frame_buf_ratio_c = frame_buf_ratio;
    }

    c_header_stride = y_header_stride;

    y_header_size = y_header_stride * (OT_ALIGN_UP(height, 16) / 16); /* 16: algorithm param */
    c_header_size = c_header_stride * (OT_ALIGN_UP(height, 16) / 16); /* 16: algorithm param */

    blk_height_y = ((16 * 4 * 2 + 16 + 16 + 15) / 16) * 16; /* 16 15 4 2: algorithm param */
    blk_height_c = blk_height_y / 2; /* 2: algorithm param */

    y_stride = OT_ALIGN_UP(width, 64); /* 64: align */
    y_size = OT_ALIGN_UP((y_stride * height * frame_buf_ratio_y / 100), 256); /* 100 256: align */
    y_blk_size = OT_ALIGN_UP((y_stride * blk_height_y), 256); /* 256: align */

    c_stride = y_stride;
    c_size = OT_ALIGN_UP((c_stride * (height / 2) * frame_buf_ratio_c / 100), 256); /* 100 256: align */
    c_blk_size = OT_ALIGN_UP((c_stride * blk_height_c), 256); /* 256: align */
    if (share_buf == TD_FALSE) {
        pic_size = y_header_size + c_header_size + y_size + c_size;
    } else {
        /* 2: algorithm param */
        pic_size = (y_header_size + c_header_size) * 2 + y_size + c_size + y_blk_size + c_blk_size;
    }
    return pic_size;
}

__inline static td_u32 ot_venc_get_h265_pic_buf_size(ot_payload_type type, const ot_venc_buf_attr *attr)
{
    td_u32 y_header_stride, c_header_stride, y_stride, c_stride;
    td_u32 y_header_size, c_header_size;
    td_u32 y_size, c_size, y_blk_size, c_blk_size;
    td_u32 blk_height_y, blk_height_c;
    td_u32 width, height;
    td_bool share_buf;
    td_u32 frame_buf_ratio, frame_buf_ratio_y, frame_buf_ratio_c;
    td_u32 pic_size;

    width = attr->pic_buf_attr.width;
    height = attr->pic_buf_attr.height;
    share_buf = attr->share_buf_en;
    frame_buf_ratio = attr->frame_buf_ratio;

    if (width <= 2048) { /* 2048: pic width */
        y_header_stride = 256; /* 256: header stride */
    } else if (width <= 4096) { /* 4096: pic width */
        y_header_stride = 512; /* 512: header stride */
    } else if (width <= 6144) { /* 6144: pic width */
        y_header_stride = 768; /* 768: header stride */
    } else {
        y_header_stride = 1024; /* 1024: header stride */
    }

    if (frame_buf_ratio >= 70 && frame_buf_ratio < 100) { /* 70 100: buf ratio */
        frame_buf_ratio_y = frame_buf_ratio + 10; /* 10: buf ratio */
        frame_buf_ratio_c = frame_buf_ratio - 20; /* 20: buf ratio */
    } else if (frame_buf_ratio >= 60 && frame_buf_ratio < 70) { /* 60 70: buf ratio */
        frame_buf_ratio_y = frame_buf_ratio + 5; /* 5: buf ratio */
        frame_buf_ratio_c = frame_buf_ratio - 10; /* 10: buf ratio */
    } else {
        frame_buf_ratio_y = frame_buf_ratio;
        frame_buf_ratio_c = frame_buf_ratio;
    }

    c_header_stride = y_header_stride / 2; /* 2: algorithm param */

    y_header_size = y_header_stride * (OT_ALIGN_UP(height, 32) / 32); /* 32: algorithm param */
    c_header_size = c_header_stride * (OT_ALIGN_UP(height, 32) / 32); /* 32: algorithm param */

    blk_height_y = ((28 * 4 * 2 + 32 + 32 + 15) / 16) * 16; /* 32 28 16 15 4 2: algorithm param */
    blk_height_c = blk_height_y / 2; /* 2: algorithm param */
    y_stride = OT_ALIGN_UP(width, 64); /* 64: align */
    y_size = OT_ALIGN_UP((y_stride * height * frame_buf_ratio_y / 100), 256); /* 100 256: align */
    y_blk_size = OT_ALIGN_UP((y_stride * blk_height_y), 256); /* 256: align */
    c_stride = y_stride;
    c_size = OT_ALIGN_UP((c_stride * (height / 2) * frame_buf_ratio_c / 100), 256); /* 100 256: align */
    c_blk_size = OT_ALIGN_UP((c_stride * blk_height_c), 256); /* 256: align */

    if (share_buf == TD_FALSE) {
        pic_size = y_header_size + c_header_size + y_size + c_size;
    } else {
        /* 2: algorithm param */
        pic_size = (y_header_size + c_header_size) * 2 + y_size + c_size + y_blk_size + c_blk_size;
    }

    return pic_size;
}

__inline static td_bool ot_venc_is_mosaic_blk_valid(td_u32 blk_size)
{
    if (blk_size == 4 || blk_size == 8 || blk_size == 16 || /* 4, 8, 16: blk size */
        blk_size == 32 || blk_size == 64 || blk_size == 128) { /* 32, 64, 128: blk size */
        return TD_TRUE;
    }

    return TD_FALSE;
}

__inline static td_void ot_common_get_fmu_align_size(td_u32 *width, td_u32 *height)
{
    *height = OT_ALIGN_UP(*height, 4); /* 4: height align */
    *width = OT_ALIGN_UP(*width, OT_DEFAULT_ALIGN);
}

__inline static td_u32 ot_vi_get_fmu_wrap_page_num(td_u32 width, td_u32 height)
{
    const td_u32 page_size = 0x8000; /* 0x8000, page_size */
    td_u32 total_size;
    td_u32 page_num;

    ot_common_get_fmu_align_size(&width, &height);

    total_size = width * ((height >> 1) + OT_FMU_MIN_BUF_LINE);
    page_num = OT_ALIGN_UP(total_size, page_size) / page_size + 2; /* 2: ext */

    page_num = clip3(page_num, OT_FMU_MIN_PAGE_NUM, OT_FMU_MAX_Y_PAGE_NUM);

    return page_num;
}

__inline static td_u32 ot_vpss_get_fmu_wrap_page_num(td_u32 width, td_u32 height)
{
    const td_u32 page_size = 0x8000; /* 0x8000, page_size */
    td_u32 y_size, c_size;
    td_u32 y_page_num, c_page_num;

    ot_common_get_fmu_align_size(&width, &height);

    y_size = width * (OT_FMU_MIN_BUF_LINE << 2); /* 2: pre */
    c_size = y_size >> 1;
    y_page_num = OT_ALIGN_UP(y_size, page_size) / page_size + 3; /* 3: ext */
    c_page_num = OT_ALIGN_UP(c_size, page_size) / page_size + 3; /* 3: ext */
    y_page_num = clip3(y_page_num, OT_FMU_MIN_PAGE_NUM, OT_FMU_MAX_Y_PAGE_NUM);
    c_page_num = clip3(c_page_num, OT_FMU_MIN_PAGE_NUM, OT_FMU_MAX_C_PAGE_NUM);

    return y_page_num + c_page_num;
}

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* OT_BUFFER_DETAIL_H */
