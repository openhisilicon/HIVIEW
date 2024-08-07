/*
  Copyright (c), 2001-2024, Shenshu Tech. Co., Ltd.
 */

#ifndef OT_BUFFER_H
#define OT_BUFFER_H

#include "securec.h"
#include "ot_type.h"
#include "ot_common.h"
#include "ot_common_video.h"
#include "ot_math.h"
#include "ot_buffer_detail.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

__inline static td_void ot_common_get_pic_buf_cfg(const ot_pic_buf_attr *buf_attr, ot_vb_calc_cfg *calc_cfg)
{
    if (calc_cfg == TD_NULL) {
        return;
    }
    if (buf_attr == TD_NULL) {
        (td_void)memset_s(calc_cfg, sizeof(*calc_cfg), 0, sizeof(*calc_cfg));
        return;
    }

    /* fmap compress */
    if (buf_attr->compress_mode == OT_COMPRESS_MODE_FMAP) {
        ot_common_get_fmap_compress_buf_cfg(buf_attr, calc_cfg);
        return;
    }

    /* tile32x4, yuv */
    if (buf_attr->video_format == OT_VIDEO_FORMAT_TILE_32x4) {
        ot_common_get_tile32x4_yuv_buf_cfg(buf_attr, calc_cfg);
        return;
    }

    /* linear, raw */
    if (ot_common_is_pixel_format_rgb_bayer(buf_attr->pixel_format) == TD_TRUE) {
        ot_common_get_raw_buf_cfg(buf_attr, calc_cfg);
        return;
    }
    /* linear, yuv */
    ot_common_get_yuv_buf_cfg(buf_attr, calc_cfg);
}

__inline static td_u32 ot_common_get_pic_buf_size(const ot_pic_buf_attr *buf_attr)
{
    ot_vb_calc_cfg calc_cfg;

    ot_common_get_pic_buf_cfg(buf_attr, &calc_cfg);

    return calc_cfg.vb_size;
}

__inline static td_void ot_vi_get_raw_buf_cfg_with_compress_ratio(const ot_pic_buf_attr *buf_attr,
    td_u32 compress_ratio, ot_vb_calc_cfg *calc_cfg)
{
    if (calc_cfg == TD_NULL) {
        return;
    }
    if ((buf_attr == TD_NULL) || (buf_attr->compress_mode != OT_COMPRESS_MODE_FRAME)) {
        (td_void)memset_s(calc_cfg, sizeof(*calc_cfg), 0, sizeof(*calc_cfg));
        return;
    }

    /* specify frame compress ratio */
    ot_common_get_raw_buf_cfg_with_compress_ratio(buf_attr, compress_ratio, calc_cfg);
}

__inline static td_u32 ot_aibnr_get_pic_buf_size(td_u32 width, td_u32 height)
{
    td_u32 compress_none_buf_size;

    if ((width > OT_MAXI_NUM_LIMIT) || (height > OT_MAXI_NUM_LIMIT)) {
        return 0;
    }

    compress_none_buf_size = ot_aibnr_get_vb_size(width, height, OT_COMPRESS_MODE_NONE);

    return compress_none_buf_size;
}

__inline static td_u32 ot_aidrc_get_in_buf_size(td_u32 width, td_u32 height)
{
    td_u32 compress_none_buf_size, compress_line_buf_size;

    if ((width > OT_MAXI_NUM_LIMIT) || (height > OT_MAXI_NUM_LIMIT)) {
        return 0;
    }

    compress_none_buf_size = ot_aidrc_get_in_vb_size(width, height, OT_COMPRESS_MODE_NONE);
    compress_line_buf_size = ot_aidrc_get_in_vb_size(width, height, OT_COMPRESS_MODE_FMAP);

    return MAX2(compress_none_buf_size, compress_line_buf_size);
}

__inline static td_u32 ot_aidrc_get_out_buf_size(td_u32 width, td_u32 height, td_bool advanced_mode)
{
    ot_vb_calc_cfg calc_cfg = {0};

    if ((width > OT_MAXI_NUM_LIMIT) || (height > OT_MAXI_NUM_LIMIT)) {
        return 0;
    }

    ot_aidrc_get_detail_buf_cfg(width, height, &calc_cfg);

    if (advanced_mode == TD_FALSE) {
        return calc_cfg.vb_size;
    }

    return calc_cfg.vb_size + ot_aidrc_get_coef_buf_size(width, height);
}

__inline static td_u32 ot_ai3dnr_get_pic_buf_size(td_u32 width, td_u32 height)
{
    td_u32 buf_size;

    if ((width > OT_MAXI_NUM_LIMIT) || (height > OT_MAXI_NUM_LIMIT)) {
        return 0;
    }

    buf_size = ot_ai3dnr_get_vb_size(width, height, OT_COMPRESS_MODE_NONE);

    return buf_size;
}

__inline static td_u32 ot_vdec_get_pic_buf_size(ot_payload_type type, const ot_pic_buf_attr *buf_attr)
{
    td_u32 align_width, align_height;
    td_u32 header_size;
    td_u32 header_stride;
    td_u32 extra_size = 0;
    td_u32 size;

    if (buf_attr == TD_NULL) {
        return 0;
    }

    if (ot_vdec_check_pic_size(type, buf_attr->width, buf_attr->height) == TD_FALSE) {
        return 0;
    }

    if ((type == OT_PT_H264) || (type == OT_PT_MP4VIDEO)) {
        align_width = OT_ALIGN_UP(buf_attr->width, OT_H264D_ALIGN_W);
        align_height = OT_ALIGN_UP(buf_attr->height, OT_H264D_ALIGN_H);
        if (buf_attr->width <= 8192) {
            header_stride = OT_ALIGN_UP(align_width, 2048) >> 6;
        } else {
            header_stride = OT_ALIGN_UP(align_width, 8192) >> 6;
        }
        header_size = header_stride * (OT_ALIGN_UP(align_height, OT_H264D_ALIGN_H) >> 2);
        size = ((header_size + align_width * align_height) * 3) >> 1;
    } else if (type == OT_PT_H265) {
        align_width = OT_ALIGN_UP(buf_attr->width, OT_H265D_ALIGN_W);
        align_height = OT_ALIGN_UP(buf_attr->height, OT_H265D_ALIGN_H);
        if (buf_attr->width <= 8192) {
            header_stride = OT_ALIGN_UP(align_width, 2048) >> 6;
        } else {
            header_stride = OT_ALIGN_UP(align_width, 8192) >> 6;
        }
        header_size = header_stride * (OT_ALIGN_UP(align_height, OT_H265D_ALIGN_H) >> 2);

        if (buf_attr->bit_width == OT_DATA_BIT_WIDTH_10) {
            extra_size = OT_ALIGN_UP((align_width * 2) >> 3, 16) * OT_ALIGN_UP(align_height, 32);
        }

        size = ((header_size + align_width * align_height + extra_size) * 3) >> 1;
    } else if ((type == OT_PT_JPEG) || (type == OT_PT_MJPEG)) {
        /* for OT_PIXEL_FORMAT_YVU_SEMIPLANAR_420 */
        align_width = OT_ALIGN_UP(buf_attr->width, OT_JPEGD_ALIGN_W);
        align_height = OT_ALIGN_UP(buf_attr->height, OT_JPEGD_ALIGN_H);
        if (buf_attr->pixel_format == OT_PIXEL_FORMAT_YVU_SEMIPLANAR_422) {
            size = align_width * align_height * 2; /* 2:Y+1/2U+1/2V */
        } else {
            size = (align_width * align_height * 3) >> 1; /* 3:Y+1/4U+1/4V */
        }
    } else {
        size = 0;
    }

    return size;
}

__inline static td_u32 ot_vdec_get_tmv_buf_size(ot_payload_type type, td_u32 width, td_u32 height)
{
    td_u32 width_in_mb, height_in_mb;
    td_u32 col_mb_size;
    td_u32 size;

    if (ot_vdec_check_pic_size(type, width, height) == TD_FALSE) {
        return 0;
    }

    if ((type == OT_PT_H264) || (type == OT_PT_MP4VIDEO)) {
        width_in_mb = OT_ALIGN_UP(width, 16) >> 4;
        height_in_mb = OT_ALIGN_UP(height, 16) >> 4;
        col_mb_size = 16 * 4;
        size = OT_ALIGN_UP((col_mb_size * width_in_mb * height_in_mb), 128);
    } else if (type == OT_PT_H265) {
        width_in_mb = OT_ALIGN_UP(width, 64) >> 4;
        height_in_mb = OT_ALIGN_UP(height, 64) >> 4;
        col_mb_size = 4 * 4;
        size = OT_ALIGN_UP((col_mb_size * width_in_mb * height_in_mb), 128);
    } else {
        size = 0;
    }

    return size;
}

__inline static td_u32 ot_venc_get_pic_buf_size(ot_payload_type type, const ot_venc_buf_attr *attr)
{
    if (ot_venc_check_pic_buf_size(type, attr) == TD_FALSE) {
        return 0;
    }

    if (type == OT_PT_H264) {
        return ot_venc_get_h264_pic_buf_size(type, attr);
    } else {
        return ot_venc_get_h265_pic_buf_size(type, attr);
    }
}

__inline static td_u32 ot_venc_get_pic_info_buf_size(ot_payload_type type, const ot_venc_buf_attr *attr)
{
    td_u32 pic_info_size;
    td_u32 tmv_size, pme_stride, pme_size, pme_info_size;
    td_u32 width, height;

    if (type != OT_PT_H264 && type != OT_PT_H265) {
        return 0;
    }

    if (attr == NULL) {
        return 0;
    }

    if (attr->share_buf_en != TD_FALSE && attr->share_buf_en != TD_TRUE) {
        return 0;
    }

    if (type == OT_PT_H264 &&
        (attr->pic_buf_attr.width > OT_VENC_H264_MAX_WIDTH || attr->pic_buf_attr.height > OT_VENC_H264_MAX_HEIGHT)) {
        return 0;
    }

    if (type == OT_PT_H265 &&
        (attr->pic_buf_attr.width > OT_VENC_H265_MAX_WIDTH || attr->pic_buf_attr.height > OT_VENC_H265_MAX_HEIGHT)) {
        return 0;
    }

    width = attr->pic_buf_attr.width;
    height = attr->pic_buf_attr.height;

    if (type == OT_PT_H264) {
        tmv_size = 0;
        pme_stride = OT_ALIGN_UP(width, 64) / 64;
        pme_size = pme_stride * (OT_ALIGN_UP(height, 16) / 16) * 64;
        pme_info_size = (OT_ALIGN_UP(width, 1024) / 16) * (OT_ALIGN_UP(height, 16) / 16) / 2;
    } else {
        tmv_size = ((OT_ALIGN_UP(width, 32) / 32) * (OT_ALIGN_UP(height, 32) / 32) + 1) / 2 * 2 * 32;
        pme_stride = OT_ALIGN_UP(width, 64) / 32;
        pme_size = pme_stride * (OT_ALIGN_UP(height, 32) / 32) * 64;
        pme_info_size = (OT_ALIGN_UP(width, 512) / 32) * (OT_ALIGN_UP(height, 32) / 32) * 2;
    }

    pic_info_size = tmv_size + pme_size + pme_info_size;

    return pic_info_size;
}

__inline static td_u32 ot_venc_get_qpmap_stride(td_u32 width)
{
    if (width > OT_MAXI_NUM_LIMIT) {
        return 0;
    }

    return OT_ALIGN_UP(width, 256) / 16;
}

__inline static td_u32 ot_venc_get_qpmap_size(ot_payload_type type, td_u32 width, td_u32 height)
{
    td_u32 stride, align_height;

    if ((width > OT_MAXI_NUM_LIMIT) || (height > OT_MAXI_NUM_LIMIT)) {
        return 0;
    }

    stride = ot_venc_get_qpmap_stride(width);
    if (type == OT_PT_H265) {
        align_height = OT_ALIGN_UP(height, 32) / 16;
    } else if (type == OT_PT_H264) {
        align_height = OT_ALIGN_UP(height, 16) / 16;
    } else {
        align_height = 0;
    }

    return stride * align_height;
}

__inline static td_u32 ot_venc_get_skip_weight_stride(ot_payload_type type, td_u32 width)
{
    td_u32 stride;

    if (width > OT_MAXI_NUM_LIMIT) {
        return 0;
    }

    if (type == OT_PT_H265) {
        stride = OT_ALIGN_UP(width, 1024) / 64;
    } else if (type == OT_PT_H264) {
        stride = OT_ALIGN_UP(width, 512) / 32;
    } else {
        stride = 0;
    }

    return stride;
}

__inline static td_u32 ot_venc_get_skip_weight_size(ot_payload_type type, td_u32 width, td_u32 height)
{
    td_u32 stride, align_height;

    if ((width > OT_MAXI_NUM_LIMIT) || (height > OT_MAXI_NUM_LIMIT)) {
        return 0;
    }

    stride = ot_venc_get_skip_weight_stride(type, width);

    if (type == OT_PT_H265) {
        align_height = OT_ALIGN_UP(height, 32) / 32;
    } else if (type == OT_PT_H264) {
        align_height = OT_ALIGN_UP(height, 16) / 16;
    } else {
        align_height = 0;
    }

    return stride * align_height;
}

__inline static td_u32 ot_venc_get_roimap_stride(ot_payload_type type, td_u32 width)
{
    td_u32 stride;

    if (width > OT_MAXI_NUM_LIMIT) {
        return 0;
    }

    if (type == OT_PT_JPEG || type == OT_PT_MJPEG) {
        stride = OT_ALIGN_UP(width, 1024) / 64; // 1024 64: hardware align requirement
    } else {
        stride = 0;
    }

    return stride;
}

__inline static td_u32 ot_venc_get_roimap_size(ot_payload_type type, td_u32 width, td_u32 height)
{
    td_u32 stride, align_height;

    if ((width > OT_MAXI_NUM_LIMIT) || (height > OT_MAXI_NUM_LIMIT)) {
        return 0;
    }

    stride = ot_venc_get_roimap_stride(type, width);

    if (type == OT_PT_JPEG || type == OT_PT_MJPEG) {
        align_height = OT_ALIGN_UP(height, 16) / 16; // 16: hardware align requirement
    } else {
        align_height = 0;
    }

    return stride * align_height;
}

__inline static td_u32 ot_venc_get_mosaic_map_stride(td_u32 width, td_u32 blk_size)
{
    td_u32 blk_width;

    if (width > OT_VENC_H265_MAX_WIDTH || !ot_venc_is_mosaic_blk_valid(blk_size)) {
        return 0;
    }

    blk_width = OT_ALIGN_UP(width, blk_size) / blk_size;

    return OT_ALIGN_UP(blk_width, 128) / 8; // 128, 8: hardware align requirement
}

__inline static td_u32 ot_venc_get_mosaic_map_size(td_u32 width, td_u32 height, td_u32 blk_size)
{
    td_u32 stride, blk_height;

    if (width > OT_VENC_H265_MAX_WIDTH || height > OT_VENC_H265_MAX_HEIGHT || !ot_venc_is_mosaic_blk_valid(blk_size)) {
        return 0;
    }

    stride = ot_venc_get_mosaic_map_stride(width, blk_size);
    blk_height = OT_ALIGN_UP(height, blk_size) / blk_size;

    return stride * blk_height;
}

__inline static td_u32 ot_vpss_get_mcf_color_buf_size(td_u32 width, td_u32 height)
{
    td_u32 stride;
    td_u32 size;

    stride = OT_ALIGN_UP(width, OT_DEFAULT_ALIGN * 4); /* 4: align requirement */
    height = OT_ALIGN_UP(height, 8); /* 8: align requirement */
    size = stride * height * 29 / 16;

    return size;
}

__inline static td_u32 ot_vpss_get_mcf_mono_buf_size(td_u32 width, td_u32 height)
{
    td_u32 stride;
    td_u32 size;

    stride = OT_ALIGN_UP(width, OT_DEFAULT_ALIGN * 4); /* 4: align requirement */
    height = OT_ALIGN_UP(height, 8); /* 8: align requirement */

    if (width > 3072) { /* 3072: line buffer */
        stride += 128;  /* 128: for split */
    }

    size = stride * height * 21 / 16;

    return size;
}

__inline static td_u32 ot_gdc_get_lut_buf_stride(td_u32 width, ot_lut_cell_size cell_size)
{
    td_u32 stride;

    if (cell_size >= OT_LUT_CELL_SIZE_BUTT) {
        return 0;
    }

    stride = OT_ALIGN_UP(((OT_DIV_UP(width, (16 << cell_size)) + 1) * 8), 16); // 16 8: hardware align requirement
    return stride;
}

__inline static td_u32 ot_common_get_fmu_wrap_page_num(ot_fmu_id id, td_u32 width, td_u32 height)
{
    switch (id) {
        case OT_FMU_ID_VI: {
            return 0;
        }
        case OT_FMU_ID_VPSS: {
            return ot_vpss_get_fmu_wrap_page_num(width, height);
        }
        default: {
            return 0;
        }
    }
}

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* OT_BUFFER_H */

