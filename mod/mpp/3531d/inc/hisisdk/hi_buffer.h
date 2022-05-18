/*
* Copyright (c) Hisilicon Technologies Co., Ltd. 2011-2019. All rights reserved.
* Description:
* Author: Hisilicon multimedia software group
* Create: 2011/06/28
 */

#ifndef __HI_BUFFER_H__
#define __HI_BUFFER_H__

#include "hi_math.h"
#include "hi_type.h"
#include "hi_common.h"
#include "hi_common_video.h"

#ifdef __KERNEL__
#include "hi_osal.h"
#endif

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

#define HI_MAXI_NUM_LIMIT 100000

#define HI_SEG_RATIO_8BIT_LUMA 1600
#define HI_SEG_RATIO_8BIT_CHROMA 2000

#define HI_SEG_WIDTH_BIT_ALIGN 128

typedef struct {
    hi_u32            width;
    hi_u32            height;
    hi_u32            align;
    hi_data_bit_width bit_width;
    hi_pixel_format   pixel_format;
    hi_compress_mode  compress_mode;
} hi_pic_buf_attr;

typedef struct {
    hi_bool           share_buf_en;
    hi_u32            frame_buf_ratio;
    hi_pic_buf_attr   pic_buf_attr;
} hi_venc_buf_attr;

__inline static hi_u32 hi_common_get_bit_width(hi_data_bit_width bit_width)
{
    hi_u32 res_bit_width;

    switch (bit_width) {
        case HI_DATA_BIT_WIDTH_8: {
            res_bit_width = 8;
            break;
        }
        case HI_DATA_BIT_WIDTH_10: {
            res_bit_width = 10;
            break;
        }
        case HI_DATA_BIT_WIDTH_12: {
            res_bit_width = 12;
            break;
        }
        case HI_DATA_BIT_WIDTH_14: {
            res_bit_width = 14;
            break;
        }
        case HI_DATA_BIT_WIDTH_16: {
            res_bit_width = 16;
            break;
        }
        default:
        {
            res_bit_width = 0;
            break;
        }
    }
    return res_bit_width;
}

__inline static hi_void hi_copy_vb_calc_cfg(hi_vb_calc_cfg *dst_cfg, hi_vb_calc_cfg *src_cfg)
{
    if (dst_cfg == HI_NULL || src_cfg == HI_NULL) {
        return;
    }

    dst_cfg->vb_size     = src_cfg->vb_size;
    dst_cfg->head_stride = src_cfg->head_stride;
    dst_cfg->head_size   = src_cfg->head_size;
    dst_cfg->head_y_size = src_cfg->head_y_size;
    dst_cfg->main_stride = src_cfg->main_stride;
    dst_cfg->main_size   = src_cfg->main_size;
    dst_cfg->main_y_size = src_cfg->main_y_size;

    return;
}

__inline static hi_void hi_common_get_compress_none_pic_buf_cfg(hi_pic_buf_attr *buf_attr,
    hi_s32 bit_width, hi_vb_calc_cfg *calc_cfg)
{
    hi_u32 align_height;
    hi_vb_calc_cfg cfg          = {0};

    if (buf_attr == HI_NULL || calc_cfg == HI_NULL) {
        return;
    }

    align_height = HI_ALIGN_UP(buf_attr->height, 2);
    cfg.main_stride = HI_ALIGN_UP((buf_attr->width * (hi_u32)bit_width + 7) >> 3, buf_attr->align);
    cfg.main_y_size = cfg.main_stride * align_height;

    if ((buf_attr->pixel_format == HI_PIXEL_FORMAT_YVU_SEMIPLANAR_420) ||
        (buf_attr->pixel_format == HI_PIXEL_FORMAT_YUV_SEMIPLANAR_420)) {
        cfg.main_size = ((cfg.main_stride * align_height) * 3) >> 1;
    } else if ((buf_attr->pixel_format == HI_PIXEL_FORMAT_YVU_SEMIPLANAR_422) ||
        (buf_attr->pixel_format == HI_PIXEL_FORMAT_YUV_SEMIPLANAR_422)) {
        cfg.main_size = cfg.main_stride * align_height * 2;
    } else if ((buf_attr->pixel_format == HI_PIXEL_FORMAT_YUV_400) ||
        (buf_attr->pixel_format == HI_PIXEL_FORMAT_S16C1)) {
        cfg.main_size = cfg.main_stride * align_height;
    } else {
        cfg.main_size = cfg.main_stride * align_height * 3;
    }

    cfg.vb_size = cfg.main_size;

    hi_copy_vb_calc_cfg(calc_cfg, &cfg);
    return;
}

__inline static hi_void hi_common_get_compact_pic_buf_cfg(hi_pic_buf_attr *buf_attr,
    hi_u32 bit_width, hi_vb_calc_cfg *cfg)
{
    hi_u32 c_size;
    hi_u32 align_c_height;
    hi_u32 align_height;
    hi_u32 cmp_ratio_luma   = HI_SEG_RATIO_8BIT_LUMA;
    hi_u32 cmp_ratio_chroma = HI_SEG_RATIO_8BIT_CHROMA;

    if (buf_attr == HI_NULL || cfg == HI_NULL) {
        return;
    }

    align_height = HI_ALIGN_UP(buf_attr->height, 2);
    cfg->main_stride = HI_ALIGN_UP(buf_attr->width * bit_width, HI_SEG_WIDTH_BIT_ALIGN);

    if ((buf_attr->pixel_format == HI_PIXEL_FORMAT_YVU_SEMIPLANAR_420) ||
        (buf_attr->pixel_format == HI_PIXEL_FORMAT_YUV_SEMIPLANAR_420)) {
        align_c_height = align_height / 2;
    } else if ((buf_attr->pixel_format == HI_PIXEL_FORMAT_YVU_SEMIPLANAR_422) ||
        (buf_attr->pixel_format == HI_PIXEL_FORMAT_YUV_SEMIPLANAR_422)) {
        align_c_height = align_height;
    } else if (buf_attr->pixel_format == HI_PIXEL_FORMAT_YUV_400) {
        align_c_height = 0;
    } else {
        align_c_height = align_height * 2;
    }

#ifdef __KERNEL__
    cfg->main_y_size = osal_div64_u64(cfg->main_stride * align_height * 1000ULL, cmp_ratio_luma * bit_width);
    c_size = osal_div64_u64(cfg->main_stride * align_c_height * 1000ULL, cmp_ratio_chroma * bit_width);
#else
    cfg->main_y_size = cfg->main_stride * align_height * 1000ULL / (cmp_ratio_luma * bit_width);
    c_size = cfg->main_stride * align_c_height * 1000ULL / (cmp_ratio_chroma * bit_width);
#endif

    c_size = HI_ALIGN_UP(c_size, HI_DEFAULT_ALIGN);
    cfg->main_y_size = HI_ALIGN_UP(cfg->main_y_size, HI_DEFAULT_ALIGN);

    cfg->main_stride = cfg->main_stride >> 3;
    cfg->head_y_size = cfg->head_stride * align_height;
    cfg->head_size = cfg->head_stride * (align_height + align_c_height);
    cfg->main_size = cfg->main_y_size + c_size;

    return;
}

__inline static hi_void hi_common_get_compact_none_pic_buf_cfg(hi_pic_buf_attr *buf_attr,
    hi_vb_calc_cfg *cfg)
{
    hi_u32 align_height;

    if (buf_attr == HI_NULL || cfg == HI_NULL) {
        return;
    }

    align_height = HI_ALIGN_UP(buf_attr->height, 2);
    cfg->main_stride = HI_ALIGN_UP(buf_attr->width, buf_attr->align);
    cfg->head_y_size = cfg->head_stride * align_height;
    cfg->main_y_size = cfg->main_stride * align_height;

    if ((buf_attr->pixel_format == HI_PIXEL_FORMAT_YVU_SEMIPLANAR_420) ||
        (buf_attr->pixel_format == HI_PIXEL_FORMAT_YUV_SEMIPLANAR_420)) {
        cfg->head_size = (cfg->head_stride * align_height * 3) >> 1;
        cfg->main_size = (cfg->main_stride * align_height * 3) >> 1;
    } else if ((buf_attr->pixel_format == HI_PIXEL_FORMAT_YVU_SEMIPLANAR_422) ||
        (buf_attr->pixel_format == HI_PIXEL_FORMAT_YUV_SEMIPLANAR_422)) {
        cfg->head_size = cfg->head_stride * align_height * 2;
        cfg->main_size = cfg->main_stride * align_height * 2;
    } else if (buf_attr->pixel_format == HI_PIXEL_FORMAT_YUV_400) {
        cfg->head_size = cfg->head_stride * align_height;
        cfg->main_size = cfg->main_stride * align_height;
    } else {
        cfg->head_size = cfg->head_stride * align_height * 3;
        cfg->main_size = cfg->main_stride * align_height * 3;
    }

    return;
}

__inline static hi_void hi_common_get_compress_pic_buf_cfg(hi_pic_buf_attr *buf_attr,
    hi_u32 bit_width, hi_vb_calc_cfg *calc_cfg)
{
    hi_vb_calc_cfg cfg          = {0};

    if (buf_attr == HI_NULL || calc_cfg == HI_NULL) {
        return;
    }

    if (buf_attr->width <= 4096) {
        cfg.head_stride = 16;
    } else if (buf_attr->width <= 8192) {
        cfg.head_stride = 32;
    } else {
        cfg.head_stride = 64;
    }

    if (bit_width == 8) {
        if (buf_attr->compress_mode == HI_COMPRESS_MODE_SEG_COMPACT && buf_attr->width > 256) { /* 256 compact width */
            hi_common_get_compact_pic_buf_cfg(buf_attr, bit_width, &cfg);
        } else {
            hi_common_get_compact_none_pic_buf_cfg(buf_attr, &cfg);
        }
    }

    cfg.head_size = HI_ALIGN_UP(cfg.head_size, buf_attr->align);

    cfg.vb_size = cfg.head_size + cfg.main_size;

    hi_copy_vb_calc_cfg(calc_cfg, &cfg);
    return;
}

__inline static hi_void hi_common_get_pic_buf_cfg(hi_pic_buf_attr *buf_attr, hi_vb_calc_cfg *calc_cfg)
{
    hi_u32 bit_width;

    if (buf_attr == HI_NULL || calc_cfg == HI_NULL) {
        return;
    }

    if ((buf_attr->width > HI_MAXI_NUM_LIMIT) || (buf_attr->height > HI_MAXI_NUM_LIMIT)) {
        calc_cfg->vb_size = 0;
    }

    /* align: 0 is automatic mode, alignment size following system. Non-0 for specified alignment size */
    if (buf_attr->align == 0) {
        buf_attr->align = HI_DEFAULT_ALIGN;
    } else if (buf_attr->align > HI_MAX_ALIGN) {
        buf_attr->align = HI_MAX_ALIGN;
    } else {
        buf_attr->align = (HI_ALIGN_UP(buf_attr->align, HI_DEFAULT_ALIGN));
    }

    bit_width = hi_common_get_bit_width(buf_attr->bit_width);

    if (buf_attr->compress_mode == HI_COMPRESS_MODE_NONE) {
        hi_common_get_compress_none_pic_buf_cfg(buf_attr, bit_width, calc_cfg);
    } else {
        hi_common_get_compress_pic_buf_cfg(buf_attr, bit_width, calc_cfg);
    }

    return;
}

__inline static hi_u32 hi_common_get_pic_buf_size(hi_pic_buf_attr *buf_attr)
{
    hi_vb_calc_cfg calc_cfg;

    if (buf_attr == HI_NULL) {
        return 0;
    }
    hi_common_get_pic_buf_cfg(buf_attr, &calc_cfg);

    return calc_cfg.vb_size;
}

__inline static hi_u32 hi_vdec_get_pic_buf_size(hi_payload_type type,
    hi_pic_buf_attr *buf_attr)
{
    hi_u32 align_width, align_height;
    hi_u32 header_size = 0;
    hi_u32 header_stride = 0;
    hi_u32 extra_size = 0;
    hi_u32 size = 0;

    if (buf_attr == HI_NULL) {
        return 0;
    }

    if ((buf_attr->width > HI_MAXI_NUM_LIMIT) || (buf_attr->height > HI_MAXI_NUM_LIMIT)) {
        return 0;
    }

    if (type == HI_PT_H264) {
        align_width = HI_ALIGN_UP(buf_attr->width, HI_H264D_ALIGN_W);
        align_height = HI_ALIGN_UP(buf_attr->height, HI_H264D_ALIGN_H);
        if (buf_attr->width <= 8192) {
            header_stride = HI_ALIGN_UP(align_width, 2048) >> 6;
        } else {
            header_stride = HI_ALIGN_UP(align_width, 8192) >> 6;
        }
        header_size = header_stride * (HI_ALIGN_UP(align_height, HI_H264D_ALIGN_H) >> 2);
        size = ((header_size + align_width * align_height) * 3) >> 1;
    } else if (type == HI_PT_H265) {
        align_width = HI_ALIGN_UP(buf_attr->width, HI_H265D_ALIGN_W);
        align_height = HI_ALIGN_UP(buf_attr->height, HI_H265D_ALIGN_H);
        if (buf_attr->width <= 8192) {
            header_stride = HI_ALIGN_UP(align_width, 2048) >> 6;
        } else {
            header_stride = HI_ALIGN_UP(align_width, 8192) >> 6;
        }
        header_size = header_stride * (HI_ALIGN_UP(align_height, HI_H265D_ALIGN_H) >> 2);

        if (buf_attr->bit_width == HI_DATA_BIT_WIDTH_10) {
            extra_size = HI_ALIGN_UP((align_width * 2) >> 3, 16) * HI_ALIGN_UP(align_height, 32);
        }

        size = ((header_size + align_width * align_height + extra_size) * 3) >> 1;
    } else if ((type == HI_PT_JPEG) || (type == HI_PT_MJPEG)) {
        /* for HI_PIXEL_FORMAT_YVU_SEMIPLANAR_420 */
        align_width = HI_ALIGN_UP(buf_attr->width, HI_JPEGD_ALIGN_W);
        align_height = HI_ALIGN_UP(buf_attr->height, HI_JPEGD_ALIGN_H);
        size = (align_width * align_height * 3) >> 1;
    } else {
        size = 0;
    }

    return size;
}

__inline static hi_u32 hi_vdec_get_tmv_buf_size(hi_payload_type type, hi_u32 width, hi_u32 height)
{
    hi_u32 width_in_mb, height_in_mb;
    hi_u32 col_mb_size;
    hi_u32 size = 0;

    if ((width > HI_MAXI_NUM_LIMIT) || (height > HI_MAXI_NUM_LIMIT)) {
        return 0;
    }

    if (type == HI_PT_H264) {
        width_in_mb = HI_ALIGN_UP(width, 16) >> 4;
        height_in_mb = HI_ALIGN_UP(height, 16) >> 4;
        col_mb_size = 16 * 4;
        size = HI_ALIGN_UP((col_mb_size * width_in_mb * height_in_mb), 128);
    } else if (type == HI_PT_H265) {
        width_in_mb = HI_ALIGN_UP(width, 64) >> 4;
        height_in_mb = HI_ALIGN_UP(height, 64) >> 4;
        col_mb_size = 4 * 4;
        size = HI_ALIGN_UP((col_mb_size * width_in_mb * height_in_mb), 128);
    } else {
        size = 0;
    }

    return size;
}

__inline static hi_u32 hi_venc_get_pic_buf_size(hi_payload_type type, hi_venc_buf_attr *attr)
{
    hi_u32 size = 0;
    hi_u32 align_width, align_height, y_header_size, c_header_size, y_size, c_size, blk_y, blk_c;

    if (attr == HI_NULL || (attr->share_buf_en != HI_FALSE && attr->share_buf_en != HI_TRUE) ||
        (attr->pic_buf_attr.width > HI_MAXI_NUM_LIMIT) || (attr->pic_buf_attr.height > HI_MAXI_NUM_LIMIT) ||
        (attr->pic_buf_attr.bit_width != HI_DATA_BIT_WIDTH_8)) {
        return 0;
    }

    if (type == HI_PT_H265) {
        align_height = HI_ALIGN_UP(attr->pic_buf_attr.height, 32);

        y_header_size = 32 * align_height / 4;
        c_header_size = y_header_size;

        align_width = HI_ALIGN_UP(attr->pic_buf_attr.width, 64);
        align_height = HI_ALIGN_UP(attr->pic_buf_attr.height, 16);
        y_size = (align_width * align_height * 8) >> 3;
        c_size = y_size >> 1;

        size = y_header_size + c_header_size + y_size + c_size;
        if (attr->share_buf_en == HI_TRUE) {
            blk_y = ((20 * 4 + 16 + 32 + 15) >> 4) << 4;  /* 4, 16, 15: algorithm param */
            blk_c = blk_y / 2;
            y_size += align_width * blk_y;
            c_size += align_width * blk_c;
            size = 2 * y_header_size + 2 * c_header_size + y_size + c_size;

            size += HI_ALIGN_UP(attr->pic_buf_attr.width, 32) * HI_ALIGN_UP(attr->pic_buf_attr.height, 32) / 16 +
                (attr->pic_buf_attr.width + 31) / 32 * 64;
        }
    } else if (type == HI_PT_H264) {
        align_height = HI_ALIGN_UP(attr->pic_buf_attr.height, 16);

        y_header_size = 32 * align_height / 4;
        c_header_size = y_header_size;

        align_width = HI_ALIGN_UP(attr->pic_buf_attr.width, 64);
        align_height = HI_ALIGN_UP(attr->pic_buf_attr.height, 16);
        y_size = align_width * align_height;
        c_size = y_size >> 1;

        size = y_header_size + c_header_size + y_size + c_size;
        if (attr->share_buf_en == HI_TRUE) {
            blk_y = ((16 * 4 + 16 + 16 + 15) >> 4) << 4;  /* 4, 16, 15: algorithm param */
            blk_c = blk_y / 2;
            y_size += align_width * blk_y;
            c_size += align_width * blk_c;
            size = 2 * y_header_size + 2 * c_header_size + y_size + c_size;

            size += HI_ALIGN_UP(attr->pic_buf_attr.width, 64) * HI_ALIGN_UP(attr->pic_buf_attr.height, 16) / 16 +
                HI_ALIGN_UP(attr->pic_buf_attr.width, 64);
        }
    } else {
        size = 0;
    }

    return size;
}

__inline static hi_u32 hi_venc_get_pic_info_buf_size(hi_payload_type type, hi_venc_buf_attr *attr)
{
    hi_u32 size;
    hi_u32 align_width, align_height;
    hi_u32 tmv_size, pme_size, pme_info_size;

    if (attr == HI_NULL || (attr->share_buf_en != HI_FALSE && attr->share_buf_en != HI_TRUE) ||
        (attr->pic_buf_attr.width > HI_MAXI_NUM_LIMIT) || (attr->pic_buf_attr.height > HI_MAXI_NUM_LIMIT)) {
        return 0;
    }

    if (type == HI_PT_H265) {
        align_width = HI_ALIGN_UP(attr->pic_buf_attr.width, 32) / 32;
        align_height = HI_ALIGN_UP(attr->pic_buf_attr.height, 32) / 32;
        tmv_size = (align_width * align_height + 1) / 2 * 2 * 16;

        align_width = HI_ALIGN_UP(attr->pic_buf_attr.width, 32);
        align_height = HI_ALIGN_UP(attr->pic_buf_attr.height, 32);
        pme_size = (align_width / 4) * (align_height / 4);

        align_width = HI_ALIGN_UP(attr->pic_buf_attr.width, 512) / 32;
        align_height = HI_ALIGN_UP(attr->pic_buf_attr.height, 32) / 32;
        pme_info_size = align_width * align_height * 2;

        size = tmv_size + pme_info_size;
        if (attr->share_buf_en == HI_FALSE) {
            size = tmv_size + pme_info_size + pme_size;
        }
    } else if (type == HI_PT_H264) {
        tmv_size = 0;

        align_width = HI_ALIGN_UP(attr->pic_buf_attr.width, 64) / 4;
        align_height = HI_ALIGN_UP(attr->pic_buf_attr.height, 16) / 4;
        pme_size = align_width  * align_height;

        align_width = HI_ALIGN_UP(attr->pic_buf_attr.width, 1024) / 16;
        align_height = HI_ALIGN_UP(attr->pic_buf_attr.height, 16) / 16;
        pme_info_size = align_width * align_height / 2;

        size = tmv_size + pme_info_size;
        if (attr->share_buf_en == HI_FALSE) {
            size = tmv_size + pme_info_size + pme_size;
        }
    } else {
        size = 0;
    }

    return size;
}

__inline static hi_u32 hi_venc_get_ref_pic_info_buf_size(hi_payload_type type,
    hi_u32 width, hi_u32 height, hi_u32 align)
{
    hi_venc_buf_attr attr;
    attr.pic_buf_attr.width = width;
    attr.pic_buf_attr.height = height;
    attr.pic_buf_attr.align = align;
    attr.pic_buf_attr.bit_width = HI_DATA_BIT_WIDTH_8;
    attr.frame_buf_ratio = 100; /* 100 : param */
    attr.share_buf_en = HI_FALSE;

    return hi_venc_get_pic_info_buf_size(type, &attr);
}

__inline static hi_u32 hi_venc_get_ref_buf_size(hi_payload_type type, hi_u32 width, hi_u32 height,
    hi_data_bit_width bit_width_enum, hi_u32 align)
{
    hi_venc_buf_attr attr;
    attr.pic_buf_attr.width = width;
    attr.pic_buf_attr.height = height;
    attr.pic_buf_attr.align = align;
    attr.pic_buf_attr.bit_width = HI_DATA_BIT_WIDTH_8;
    attr.frame_buf_ratio = 100; /* 100 : param */
    attr.share_buf_en = HI_FALSE;

    return hi_venc_get_pic_buf_size(type, &attr);
}

__inline static hi_u32 hi_venc_get_qpmap_stride(hi_u32 width)
{
    if (width > HI_MAXI_NUM_LIMIT) {
        return 0;
    }

    return HI_ALIGN_UP(width, 256) / 16;
}

__inline static hi_u32 hi_venc_get_qpmap_size(hi_payload_type type, hi_u32 width, hi_u32 height)
{
    hi_u32 stride, align_height;

    if ((width > HI_MAXI_NUM_LIMIT) || (height > HI_MAXI_NUM_LIMIT)) {
        return 0;
    }

    stride = hi_venc_get_qpmap_stride(width);
    if (type == HI_PT_H265) {
        align_height = HI_ALIGN_UP(height, 32) / 16;
    } else if (type == HI_PT_H264) {
        align_height = HI_ALIGN_UP(height, 16) / 16;
    } else {
        align_height = 0;
    }

    return stride * align_height;
}

__inline static hi_u32 hi_venc_get_skip_weight_stride(hi_payload_type type, hi_u32 width)
{
    hi_u32 stride;

    if (width > HI_MAXI_NUM_LIMIT) {
        return 0;
    }

    if (type == HI_PT_H265) {
        stride = HI_ALIGN_UP(width, 1024) / 64;
    } else if (type == HI_PT_H264) {
        stride = HI_ALIGN_UP(width, 512) / 32;
    } else {
        stride = 0;
    }

    return stride;
}

__inline static hi_u32 hi_venc_get_skip_weight_size(hi_payload_type type, hi_u32 width, hi_u32 height)
{
    hi_u32 stride, align_height;

    if ((width > HI_MAXI_NUM_LIMIT) || (height > HI_MAXI_NUM_LIMIT)) {
        return 0;
    }

    stride = hi_venc_get_skip_weight_stride(type, width);

    if (type == HI_PT_H265) {
        align_height = HI_ALIGN_UP(height, 32) / 32;
    } else if (type == HI_PT_H264) {
        align_height = HI_ALIGN_UP(height, 16) / 16;
    } else {
        align_height = 0;
    }

    return stride * align_height;
}

__inline static hi_u32 hi_venc_get_roimap_stride(hi_payload_type type, hi_u32 width)
{
    hi_u32 stride;

    if (width > HI_MAXI_NUM_LIMIT) {
        return 0;
    }

    if (type == HI_PT_JPEG || type == HI_PT_MJPEG) {
        stride = HI_ALIGN_UP(width, 1024) / 64; // 1024 64: hardware align requirement
    } else {
        stride = 0;
    }

    return stride;
}

__inline static hi_u32 hi_venc_get_roimap_size(hi_payload_type type, hi_u32 width, hi_u32 height)
{
    hi_u32 stride, align_height;

    if ((width > HI_MAXI_NUM_LIMIT) || (height > HI_MAXI_NUM_LIMIT)) {
        return 0;
    }

    stride = hi_venc_get_roimap_stride(type, width);

    if (type == HI_PT_JPEG || type == HI_PT_MJPEG) {
        align_height = HI_ALIGN_UP(height, 16) / 16; // 16: hardware align requirement
    } else {
        align_height = 0;
    }

    return stride * align_height;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __HI_BUFFER_H__ */

