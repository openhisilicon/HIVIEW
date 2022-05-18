/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: region common function for sample.
 * Author: Hisilicon multimedia software group
 * Create: 2019/11/29
 */

#include "sample_comm.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include "loadbmp.h"


#define OVERLAY_MIN_HANDLE 0
#define OVERLAYEX_MIN_HANDLE 20
#define COVER_MIN_HANDLE 40
#define COVEREX_MIN_HANDLE 60
#define LINE_MIN_HANDLE 80
#define MOSAIC_MIN_HANDLE 100
#define MOSAICEX_MIN_HANDLE 120
#define DEFAULT_CASNVAS_NUM 2

#define BITS_NUM_PER_BYTE 8
#define BYTE_PER_PIX_1555 2
#define BYTE_PER_PIX_8888 4
#define PIX_PER_BYTTE_CLUT2 4
#define PIX_PER_BYTTE_CLUT4 2
#define OFFSET_NUM0 200
#define OFFSET_NUM1 160
#define OFFSET_NUM2 100
#define OFFSET_NUM3 50
#define MAX_BIT_COUNT 32

#define RGN_ALPHA 128
#define RGN_ARGB8888_BLUE 0xff0000ff
#define RGN_RGB888_BLUE 0x0000ff

static osd_color_format region_mst_get_color_format_by_pixel_format(hi_pixel_format pixel_format)
{
    switch (pixel_format) {
        case HI_PIXEL_FORMAT_ARGB_CLUT2:
        /* is the same with argb1555 */
        case HI_PIXEL_FORMAT_ARGB_CLUT4:
        /* is the same with argb1555 */
        case HI_PIXEL_FORMAT_ARGB_1555:
            return OSD_COLOR_FORMAT_RGB1555;
        case HI_PIXEL_FORMAT_ARGB_4444:
            return OSD_COLOR_FORMAT_RGB4444;
        case HI_PIXEL_FORMAT_ARGB_8888:
            return OSD_COLOR_FORMAT_RGB8888;
        default:
            printf("pixel format is not support!\n");
            return OSD_COLOR_FORMAT_BUTT;
    }
}

static hi_u8 *region_mst_get_clut2_data_from_bmp_data(hi_bmp *bmp)
{
    hi_u8 *clut_data = NULL;
    hi_u16 *temp = HI_NULL;
    hi_u8 *ctemp = HI_NULL;
    hi_u32 i, j, k;
    hi_s32 value;
    hi_u8 value_temp;

    temp = (hi_u16 *)bmp->data;

    clut_data = malloc(bmp->height * bmp->width / PIX_PER_BYTTE_CLUT2);
    if (clut_data == NULL) {
        printf("malloc osd memroy err!\n");
        return HI_NULL;
    }

    ctemp = (hi_u8 *)clut_data;
    for (i = 0; i < bmp->height; i++) {
        for (j = 0; j < bmp->width / PIX_PER_BYTTE_CLUT2; j++) {
            value = 0;
            for (k = 0; k < PIX_PER_BYTTE_CLUT2; k++) {
                value_temp = ((*temp & 0x001f) * 28 + ((*temp >> 5) & 0x001f) * 58 + /* 0x001f:28:5:58:color modulus */
                    ((*temp >> 10) & 0x001f) * 14) / 800; /* 10:0x001f:14:800:color modulus */
                value_temp = value_temp << (2 * (PIX_PER_BYTTE_CLUT2 - k - 1)); /* 2:color modulus */
                value += value_temp;
                temp++;
            }
            *ctemp = value;
            ctemp++;
        }
    }

    return clut_data;
}

static hi_u8 *region_mst_get_clut4_data_from_bmp_data(hi_bmp *bmp)
{
    hi_u8 *clut_data = NULL;
    hi_u16 *temp = HI_NULL;
    hi_u8 *ctemp = HI_NULL;
    hi_u32 i, j, k;
    hi_s32 value;
    hi_u8 value_temp;

    temp = (hi_u16 *)bmp->data;

    clut_data = malloc(bmp->height * bmp->width / PIX_PER_BYTTE_CLUT4);
    if (clut_data == NULL) {
        printf("malloc osd memroy err!\n");
        return HI_NULL;
    }

    ctemp = (hi_u8 *)clut_data;
    for (i = 0; i < bmp->height; i++) {
        for (j = 0; j < bmp->width / PIX_PER_BYTTE_CLUT4; j++) {
            value = 0;
            for (k = j; k < j + PIX_PER_BYTTE_CLUT4; k++) {
                value_temp = ((*temp & 0x001f) * 28 + ((*temp >> 5) & 0x001f) * 58 + /* 0x001f:28:5:58:color modulus */
                    ((*temp >> 10) & 0x001f) * 14) / 200; /* 10:0x001f:14:200:color modulus */
                temp++;
                value = (value_temp << 4) + value_temp; /* 4:color modulus */
            }
            *ctemp = value;
            ctemp++;
        }
    }

    return clut_data;
}

hi_void fil_bmp(hi_bmp *bmp, hi_bool fil, hi_u32 fil_color)
{
    hi_u16 *temp = HI_NULL;
    hi_u32 i, j;

    if (!fil) {
        return;
    }

    temp = (hi_u16 *)bmp->data;
    for (i = 0; i < bmp->height; i++) {
        for (j = 0; j < bmp->width; j++) {
            if (*temp == fil_color) {
                *temp &= 0x7FFF;
            }
            temp++;
        }
    }
    return;
}

hi_s32 region_mst_load_bmp(const char *filename, hi_bmp *bmp, hi_bool fil, hi_u32 fil_color, hi_pixel_format pixel_fmt)
{
    osd_surface surface;
    osd_bit_map_file_header bmp_file_header;
    osd_bit_map_info bmp_info;
    hi_u32 bpp;
    hi_u8 *clut_data = NULL;

    if (get_bmp_info(filename, &bmp_file_header, &bmp_info) < 0) {
        printf("get_bmp_info err!\n");
        return HI_FAILURE;
    }
    if (bmp_info.bmp_header.bi_bit_count > MAX_BIT_COUNT || bmp_info.bmp_header.bi_width > HI_RGN_OVERLAY_MAX_WIDTH ||
        bmp_info.bmp_header.bi_height > HI_RGN_OVERLAY_MAX_HEIGHT) {
        printf("bmp info error!");
        return HI_FAILURE;
    }
    surface.color_format = region_mst_get_color_format_by_pixel_format(pixel_fmt);
    if (surface.color_format == OSD_COLOR_FORMAT_BUTT) {
        return HI_FAILURE;
    }

    bpp = bmp_info.bmp_header.bi_bit_count / BITS_NUM_PER_BYTE;
    bmp->data = malloc(bmp_info.bmp_header.bi_width * bpp * abs(bmp_info.bmp_header.bi_height));
    if (bmp->data == NULL) {
        printf("malloc osd memroy err!\n");
        return HI_FAILURE;
    }

    create_surface_by_bit_map(filename, &surface, (hi_u8 *)(bmp->data));

    bmp->width = surface.width;
    bmp->height = surface.height;
    bmp->pixel_format = pixel_fmt;

    if (pixel_fmt == HI_PIXEL_FORMAT_ARGB_CLUT2) {
        clut_data = region_mst_get_clut2_data_from_bmp_data(bmp);
    } else if (pixel_fmt == HI_PIXEL_FORMAT_ARGB_CLUT4) {
        clut_data = region_mst_get_clut4_data_from_bmp_data(bmp);
    } else {
        fil_bmp(bmp, fil, fil_color);
    }
    if (clut_data != NULL) {
        free(bmp->data);
        bmp->data = clut_data;
    }
    return HI_SUCCESS;
}

hi_s32 region_mst_update_canvas(const char *filename, hi_bmp *bmp, hi_bool fil, hi_u32 fil_color, hi_size *size,
    hi_u32 stride, hi_pixel_format pixel_fmt)
{
    osd_surface surface;
    osd_bit_map_file_header bmp_file_header;
    osd_bit_map_info bmp_info;
    canvas_size_info canvas_size;

    if (get_bmp_info(filename, &bmp_file_header, &bmp_info) < 0) {
        printf("get_bmp_info err!\n");
        return HI_FAILURE;
    }

    if (HI_PIXEL_FORMAT_ARGB_1555 == pixel_fmt) {
        surface.color_format = OSD_COLOR_FORMAT_RGB1555;
    } else if (HI_PIXEL_FORMAT_ARGB_4444 == pixel_fmt) {
        surface.color_format = OSD_COLOR_FORMAT_RGB4444;
    } else if (HI_PIXEL_FORMAT_ARGB_8888 == pixel_fmt) {
        surface.color_format = OSD_COLOR_FORMAT_RGB8888;
    } else if (HI_PIXEL_FORMAT_ARGB_CLUT2 == pixel_fmt) {
        surface.color_format = OSD_COLOR_FORMAT_CLUT4;
    } else if (HI_PIXEL_FORMAT_ARGB_CLUT4 == pixel_fmt) {
        surface.color_format = OSD_COLOR_FORMAT_CLUT4;
    } else {
        printf("pixel format is not support!\n");
        return HI_FAILURE;
    }

    if (bmp->data == NULL) {
        printf("malloc osd memroy err!\n");
        return HI_FAILURE;
    }

    canvas_size.width = size->width;
    canvas_size.height = size->height;
    canvas_size.stride = stride;
    create_surface_by_canvas(filename, &surface, (hi_u8 *)(bmp->data), &canvas_size);

    bmp->width = surface.width;
    bmp->height = surface.height;

    if (HI_PIXEL_FORMAT_ARGB_1555 == pixel_fmt) {
        bmp->pixel_format = HI_PIXEL_FORMAT_ARGB_1555;
    } else if (HI_PIXEL_FORMAT_ARGB_4444 == pixel_fmt) {
        bmp->pixel_format = HI_PIXEL_FORMAT_ARGB_4444;
    } else if (HI_PIXEL_FORMAT_ARGB_8888 == pixel_fmt) {
        bmp->pixel_format = HI_PIXEL_FORMAT_ARGB_8888;
    } else if (HI_PIXEL_FORMAT_ARGB_CLUT2 == pixel_fmt) {
        bmp->pixel_format = HI_PIXEL_FORMAT_ARGB_CLUT2;
    } else if (HI_PIXEL_FORMAT_ARGB_CLUT4 == pixel_fmt) {
        bmp->pixel_format = HI_PIXEL_FORMAT_ARGB_CLUT4;
    }

    return HI_SUCCESS;
}

hi_s32 sample_comm_region_get_min_handle(hi_rgn_type type)
{
    hi_s32 min_handle;
    switch (type) {
        case HI_RGN_OVERLAY:
            min_handle = OVERLAY_MIN_HANDLE;
            break;
        case HI_RGN_OVERLAYEX:
            min_handle = OVERLAYEX_MIN_HANDLE;
            break;
        case HI_RGN_COVER:
            min_handle = COVER_MIN_HANDLE;
            break;
        case HI_RGN_COVEREX:
            min_handle = COVEREX_MIN_HANDLE;
            break;
        case HI_RGN_LINE:
            min_handle = LINE_MIN_HANDLE;
            break;
        case HI_RGN_MOSAIC:
            min_handle = MOSAIC_MIN_HANDLE;
            break;
        case HI_RGN_MOSAICEX:
            min_handle = MOSAICEX_MIN_HANDLE;
            break;
        default:
            min_handle = -1;
            break;
    }
    return min_handle;
}

hi_s32 sample_region_create_overlay(hi_s32 handle_num)
{
    hi_s32 ret;
    hi_s32 i;
    hi_rgn_attr region;

    region.type = HI_RGN_OVERLAY;
    region.attr.overlay.pixel_format = HI_PIXEL_FORMAT_ARGB_CLUT2;
    region.attr.overlay.size.height = OFFSET_NUM0;
    region.attr.overlay.size.width = OFFSET_NUM0;
    region.attr.overlay.bg_color = 1;
    region.attr.overlay.canvas_num = DEFAULT_CASNVAS_NUM;
    for (i = 0; i < HI_RGN_CLUT_NUM; i++) {
        region.attr.overlay.clut[i] = 0x000fff0f * (i + 1) * (i + 1);
        region.attr.overlay.clut[i] |= 0xff000000;
    }
    if (handle_num == 2) { /* 2 :two region */
        for (i = OVERLAY_MIN_HANDLE; i < OVERLAY_MIN_HANDLE + handle_num; i++) {
            ret = hi_mpi_rgn_create(i, &region);
        }
    }
    if (handle_num == 8) {                                                         /* 8:eight region */
        for (i = OVERLAY_MIN_HANDLE; i < OVERLAY_MIN_HANDLE + handle_num - 6; i++) { /* 6:ARGBCLUT2 */
            ret = hi_mpi_rgn_create(i, &region);
        }
        for (i = OVERLAY_MIN_HANDLE + 2; i < OVERLAY_MIN_HANDLE + handle_num - 4; i++) { /* 2:4:ARGBCLUT4 */
            region.attr.overlay.pixel_format = HI_PIXEL_FORMAT_ARGB_CLUT4;
            region.attr.overlay.bg_color = 1;
            ret = hi_mpi_rgn_create(i, &region);
        }
        for (i = OVERLAY_MIN_HANDLE + 4; i < OVERLAY_MIN_HANDLE + handle_num - 2; i++) { /* 4:2:ARGB1555 */
            region.attr.overlay.pixel_format = HI_PIXEL_FORMAT_ARGB_1555;
            region.attr.overlay.bg_color = 0xffff;
            ret = hi_mpi_rgn_create(i, &region);
        }
        for (i = OVERLAY_MIN_HANDLE + 6; i < OVERLAY_MIN_HANDLE + handle_num; i++) { /* 6:ARGB4444 */
            region.attr.overlay.pixel_format = HI_PIXEL_FORMAT_ARGB_4444;
            region.attr.overlay.bg_color = 0xffff;
            ret = hi_mpi_rgn_create(i, &region);
        }
    } else {
        ret = HI_FALSE;
    }
    if (ret != HI_SUCCESS) {
        SAMPLE_PRT("hi_mpi_rgn_create failed with %#x!\n", ret);
        return HI_FAILURE;
    }
    return ret;
}

hi_s32 sample_region_create_overlayex(hi_s32 handle_num)
{
    hi_s32 ret;
    hi_s32 i;
    hi_rgn_attr region;

    region.type = HI_RGN_OVERLAYEX;
    region.attr.overlayex.pixel_format = HI_PIXEL_FORMAT_ARGB_1555;
    region.attr.overlayex.size.height = OFFSET_NUM0;
    region.attr.overlayex.size.width = OFFSET_NUM0;
    region.attr.overlayex.bg_color = 0x00ff;
    region.attr.overlayex.canvas_num = DEFAULT_CASNVAS_NUM;
    for (i = OVERLAYEX_MIN_HANDLE; i < OVERLAYEX_MIN_HANDLE + handle_num; i++) {
        ret = hi_mpi_rgn_create(i, &region);
        if (ret != HI_SUCCESS) {
            SAMPLE_PRT("hi_mpi_rgn_create failed with %#x!\n", ret);
            return HI_FAILURE;
        }
    }

    return ret;
}

hi_s32 sample_region_create_cover(hi_s32 handle_num)
{
    hi_s32 ret;
    hi_s32 i;
    hi_rgn_attr region;

    region.type = HI_RGN_COVER;

    for (i = COVER_MIN_HANDLE; i < COVER_MIN_HANDLE + handle_num; i++) {
        ret = hi_mpi_rgn_create(i, &region);
        if (ret != HI_SUCCESS) {
            SAMPLE_PRT("hi_mpi_rgn_create failed with %#x!\n", ret);
            return HI_FAILURE;
        }
    }

    return ret;
}

hi_s32 sample_region_create_coverex(hi_s32 handle_num)
{
    hi_s32 ret;
    hi_s32 i;
    hi_rgn_attr region;

    region.type = HI_RGN_COVEREX;

    for (i = COVEREX_MIN_HANDLE; i < COVEREX_MIN_HANDLE + handle_num; i++) {
        ret = hi_mpi_rgn_create(i, &region);
        if (ret != HI_SUCCESS) {
            SAMPLE_PRT("hi_mpi_rgn_create failed with %#x!\n", ret);
            return HI_FAILURE;
        }
    }

    return ret;
}

hi_s32 sample_region_create_line(hi_s32 handle_num)
{
    hi_s32 ret;
    hi_s32 i;
    hi_rgn_attr region;

    region.type = HI_RGN_LINE;

    for (i = LINE_MIN_HANDLE; i < LINE_MIN_HANDLE + handle_num; i++) {
        ret = hi_mpi_rgn_create(i, &region);
        if (ret != HI_SUCCESS) {
            SAMPLE_PRT("hi_mpi_rgn_create failed with %#x!\n", ret);
            return HI_FAILURE;
        }
    }

    return ret;
}


hi_s32 sample_region_create_mosaic(hi_s32 handle_num)
{
    hi_s32 ret;
    hi_s32 i;
    hi_rgn_attr region;

    region.type = HI_RGN_MOSAIC;

    for (i = MOSAIC_MIN_HANDLE; i < MOSAIC_MIN_HANDLE + handle_num; i++) {
        ret = hi_mpi_rgn_create(i, &region);
        if (ret != HI_SUCCESS) {
            SAMPLE_PRT("hi_mpi_rgn_create failed with %#x!\n", ret);
            return HI_FAILURE;
        }
    }

    return ret;
}

hi_s32 sample_region_create_mosaicex(hi_s32 handle_num)
{
    hi_s32 ret;
    hi_s32 i;
    hi_rgn_attr region;

    region.type = HI_RGN_MOSAICEX;

    for (i = MOSAICEX_MIN_HANDLE; i < MOSAICEX_MIN_HANDLE + handle_num; i++) {
        ret = hi_mpi_rgn_create(i, &region);
        if (ret != HI_SUCCESS) {
            SAMPLE_PRT("hi_mpi_rgn_create failed with %#x!\n", ret);
            return HI_FAILURE;
        }
    }

    return ret;
}

hi_s32 sample_region_destroy(hi_rgn_handle handle)
{
    hi_s32 ret;

    ret = hi_mpi_rgn_destroy(handle);
    if (ret != HI_SUCCESS) {
        SAMPLE_PRT("hi_mpi_rgn_destroy failed with %#x!\n", ret);
        return HI_FAILURE;
    }

    return ret;
}

hi_s32 sample_region_attach_to_chn(hi_rgn_handle handle, hi_mpp_chn *chn, hi_rgn_chn_attr *chn_attr)
{
    hi_s32 ret;
    ret = hi_mpi_rgn_attach_to_chn(handle, chn, chn_attr);
    if (ret != HI_SUCCESS) {
        SAMPLE_PRT("hi_mpi_rgn_attach_to_chn failed with %#x!\n", ret);
        return HI_FAILURE;
    }
    return ret;
}

hi_s32 sample_region_detach_from_chn(hi_rgn_handle handle, hi_mpp_chn *chn)
{
    hi_s32 ret;
    ret = hi_mpi_rgn_detach_from_chn(handle, chn);
    if (ret != HI_SUCCESS) {
        SAMPLE_PRT("hi_mpi_rgn_detach_from_chn failed with %#x!\n", ret);
        return HI_FAILURE;
    }
    return ret;
}

hi_s32 sample_region_set_bit_map(hi_rgn_handle handle, hi_bmp *bitmap)
{
    hi_s32 ret;
    ret = hi_mpi_rgn_set_bmp(handle, bitmap);
    if (ret != HI_SUCCESS) {
        SAMPLE_PRT("hi_mpi_rgn_set_bit_map failed with %#x!\n", ret);
        return HI_FAILURE;
    }
    return ret;
}

hi_s32 sample_region_get_up_canvas_info(hi_rgn_handle handle, hi_rgn_canvas_info *canvas_info)
{
    hi_s32 ret;
    ret = hi_mpi_rgn_get_canvas_info(handle, canvas_info);
    if (ret != HI_SUCCESS) {
        SAMPLE_PRT("hi_mpi_rgn_get_canvas_info failed with %#x!\n", ret);
        return HI_FAILURE;
    }

    ret = hi_mpi_rgn_update_canvas(handle);
    if (ret != HI_SUCCESS) {
        SAMPLE_PRT("hi_mpi_rgn_update_canvas failed with %#x!\n", ret);
        return HI_FAILURE;
    }
    return ret;
}

hi_s32 sample_comm_region_create(hi_s32 handle_num, hi_rgn_type type)
{
    hi_s32 ret = HI_SUCCESS;
    if (handle_num <= 0 || handle_num > 16) { /* 16:max_num */
        SAMPLE_PRT("handle_num is illegal %d!\n", handle_num);
        return HI_FAILURE;
    }
    if (type < 0 || type >= HI_RGN_BUTT) {
        SAMPLE_PRT("type is illegal %d!\n", type);
        return HI_FAILURE;
    }
    switch (type) {
        case HI_RGN_OVERLAY:
            ret = sample_region_create_overlay(handle_num);
            break;
        case HI_RGN_OVERLAYEX:
            ret = sample_region_create_overlayex(handle_num);
            break;
        case HI_RGN_COVER:
            ret = sample_region_create_cover(handle_num);
            break;
        case HI_RGN_COVEREX:
            ret = sample_region_create_coverex(handle_num);
            break;
        case HI_RGN_LINE:
            ret = sample_region_create_line(handle_num);
            break;
        case HI_RGN_MOSAIC:
            ret = sample_region_create_mosaic(handle_num);
            break;
        case HI_RGN_MOSAICEX:
            ret = sample_region_create_mosaicex(handle_num);
            break;
        default:
            break;
    }
    if (ret != HI_SUCCESS) {
        SAMPLE_PRT("sample_comm_region_create failed! handle_num%d,entype:%d!\n", handle_num, type);
        return HI_FAILURE;
    }
    return ret;
}

hi_s32 sample_comm_region_destroy(hi_s32 handle_num, hi_rgn_type type)
{
    hi_s32 i;
    hi_s32 ret = HI_SUCCESS;
    hi_s32 min_handle;

    if (handle_num <= 0 || handle_num > 16) { /* 16:max_num */
        SAMPLE_PRT("handle_num is illegal %d!\n", handle_num);
        return HI_FAILURE;
    }
    if (type < 0 || type >= HI_RGN_BUTT) {
        SAMPLE_PRT("type is illegal %d!\n", type);
        return HI_FAILURE;
    }
    min_handle = sample_comm_region_get_min_handle(type);
    for (i = min_handle; i < min_handle + handle_num; i++) {
        ret = sample_region_destroy(i);
        if (ret != HI_SUCCESS) {
            SAMPLE_PRT("sample_comm_region_destroy failed!\n");
        }
    }
    return ret;
}

hi_s32 sample_comm_region_attach_check(hi_s32 handle_num, hi_rgn_type type, hi_mpp_chn *mpp_chn)
{
    if (handle_num <= 0 || handle_num > 16) { /* 16:max_num */
        SAMPLE_PRT("handle_num is illegal %d!\n", handle_num);
        return HI_FAILURE;
    }
    if (type < 0 || type >= HI_RGN_BUTT) {
        SAMPLE_PRT("type is illegal %d!\n", type);
        return HI_FAILURE;
    }
    if (mpp_chn == HI_NULL) {
        SAMPLE_PRT("mpp_chn is NULL !\n");
        return HI_FAILURE;
    }
    return HI_SUCCESS;
}

hi_s32 sample_region_get_overlay_chn_attr(hi_s32 handle_num,
    hi_rgn_chn_attr *chn_attr, hi_mpp_chn *mpp_chn)
{
    hi_s32 i, ret;

    chn_attr->type = HI_RGN_OVERLAY;

    chn_attr->attr.overlay_chn.bg_alpha = RGN_ALPHA;
    chn_attr->attr.overlay_chn.fg_alpha = RGN_ALPHA;

    chn_attr->attr.overlay_chn.qp_info.enable = HI_TRUE;
    chn_attr->attr.overlay_chn.qp_info.is_abs_qp = HI_TRUE;
    chn_attr->attr.overlay_chn.qp_info.qp_val = 0;

    chn_attr->attr.overlay_chn.dst = HI_RGN_ATTACH_JPEG_MAIN;

    for (i = OVERLAY_MIN_HANDLE; i < OVERLAY_MIN_HANDLE + handle_num; i++) {
        if (i < 4 + OVERLAY_MIN_HANDLE) { /* 4:diffreent color */
            chn_attr->attr.overlay_chn.point.x = OVERLAY_MIN_HANDLE + OFFSET_NUM1 * (i - OVERLAY_MIN_HANDLE);
            chn_attr->attr.overlay_chn.point.y = OVERLAY_MIN_HANDLE + OFFSET_NUM1 * (i - OVERLAY_MIN_HANDLE);
            chn_attr->attr.overlay_chn.layer = i - OVERLAY_MIN_HANDLE;
        } else {
            chn_attr->attr.overlay_chn.point.x = OFFSET_NUM0 * (i - 4 - OVERLAY_MIN_HANDLE) + OFFSET_NUM0; /* 4:color */
            chn_attr->attr.overlay_chn.point.y = OFFSET_NUM1 * (i - 4 - OVERLAY_MIN_HANDLE); /* 4:diffreent color */
            chn_attr->attr.overlay_chn.layer = i - OVERLAY_MIN_HANDLE;
        }
        ret = sample_region_attach_to_chn(i, mpp_chn, chn_attr);
        if (ret != HI_SUCCESS) {
            SAMPLE_PRT("sample_region_attach_to_chn failed!\n");
            sample_comm_region_detach_frm_chn(i - OVERLAY_MIN_HANDLE + 1, HI_RGN_OVERLAY, mpp_chn);
            return ret;
        }
    }
    return HI_SUCCESS;
}

hi_s32 sample_region_get_overlayex_chn_attr(hi_s32 handle_num,
    hi_rgn_chn_attr *chn_attr, hi_mpp_chn *mpp_chn)
{
    hi_s32 i, ret;

    chn_attr->type = HI_RGN_OVERLAYEX;

    chn_attr->attr.overlayex_chn.bg_alpha = RGN_ALPHA;
    chn_attr->attr.overlayex_chn.fg_alpha = RGN_ALPHA;

    for (i = OVERLAYEX_MIN_HANDLE; i < OVERLAYEX_MIN_HANDLE + handle_num; i++) {
        chn_attr->attr.overlayex_chn.point.x = OVERLAYEX_MIN_HANDLE + OFFSET_NUM2 * (i - OVERLAYEX_MIN_HANDLE);
        chn_attr->attr.overlayex_chn.point.y = OVERLAYEX_MIN_HANDLE + OFFSET_NUM3 * (i - OVERLAYEX_MIN_HANDLE);
        chn_attr->attr.overlayex_chn.layer = i - OVERLAYEX_MIN_HANDLE;
        ret = sample_region_attach_to_chn(i, mpp_chn, chn_attr);
        if (ret != HI_SUCCESS) {
            SAMPLE_PRT("sample_region_attach_to_chn failed!\n");
            sample_comm_region_detach_frm_chn(i - OVERLAY_MIN_HANDLE + 1, HI_RGN_OVERLAYEX, mpp_chn);
            return ret;
        }
    }
    return HI_SUCCESS;
}

hi_s32 sample_region_get_cover_chn_attr(hi_s32 handle_num,
    hi_rgn_chn_attr *chn_attr, hi_mpp_chn *mpp_chn)
{
    hi_s32 i, ret;

    chn_attr->type = HI_RGN_COVER;
    chn_attr->attr.cover_chn.cover.type = HI_COVER_RECT;

    chn_attr->attr.cover_chn.cover.rect.height = OFFSET_NUM0;
    chn_attr->attr.cover_chn.cover.rect.width = OFFSET_NUM0;

    chn_attr->attr.cover_chn.cover.color = RGN_RGB888_BLUE;
    chn_attr->attr.cover_chn.coord = HI_COORD_ABS;

    for (i = COVER_MIN_HANDLE; i < COVER_MIN_HANDLE + handle_num; i++) {
        chn_attr->attr.cover_chn.cover.rect.x = OFFSET_NUM0 * (i - COVER_MIN_HANDLE);
        chn_attr->attr.cover_chn.cover.rect.y = OFFSET_NUM0 * (i - COVER_MIN_HANDLE);
        chn_attr->attr.cover_chn.layer = i - COVER_MIN_HANDLE;
        ret = sample_region_attach_to_chn(i, mpp_chn, chn_attr);
        if (ret != HI_SUCCESS) {
            SAMPLE_PRT("sample_region_attach_to_chn failed!\n");
            sample_comm_region_detach_frm_chn(i - COVER_MIN_HANDLE + 1, HI_RGN_COVER, mpp_chn);
            return ret;
        }
    }
    return HI_SUCCESS;
}

hi_s32 sample_region_get_coverex_chn_attr(hi_s32 handle_num,
    hi_rgn_chn_attr *chn_attr, hi_mpp_chn *mpp_chn)
{
    hi_s32 i, ret;

    chn_attr->type = HI_RGN_COVEREX;
    chn_attr->attr.coverex_chn.coverex.type = HI_COVER_RECT;

    chn_attr->attr.coverex_chn.coverex.rect.height = OFFSET_NUM0;
    chn_attr->attr.coverex_chn.coverex.rect.width = OFFSET_NUM0;

    chn_attr->attr.coverex_chn.coverex.color = RGN_RGB888_BLUE;

    for (i = COVEREX_MIN_HANDLE; i < COVEREX_MIN_HANDLE + handle_num; i++) {
        chn_attr->attr.coverex_chn.coverex.rect.x = OFFSET_NUM0 * (i - COVEREX_MIN_HANDLE);
        chn_attr->attr.coverex_chn.coverex.rect.y = OFFSET_NUM0 * (i - COVEREX_MIN_HANDLE);
        chn_attr->attr.coverex_chn.layer = i - COVEREX_MIN_HANDLE;
        ret = sample_region_attach_to_chn(i, mpp_chn, chn_attr);
        if (ret != HI_SUCCESS) {
            SAMPLE_PRT("sample_region_attach_to_chn failed!\n");
            sample_comm_region_detach_frm_chn(i - COVEREX_MIN_HANDLE + 1, HI_RGN_COVEREX, mpp_chn);
            return ret;
        }
    }
    return HI_SUCCESS;
}

hi_s32 sample_region_get_line_chn_attr(hi_s32 handle_num,
    hi_rgn_chn_attr *chn_attr, hi_mpp_chn *mpp_chn)
{
    hi_s32 i, ret;

    chn_attr->type = HI_RGN_LINE;
    chn_attr->attr.line_chn.thick = 4; /* 4:thick */

    chn_attr->attr.line_chn.points[0].x = 0;
    chn_attr->attr.line_chn.points[0].y = 0;
    chn_attr->attr.line_chn.points[1].x = OFFSET_NUM2;
    chn_attr->attr.line_chn.points[1].y = OFFSET_NUM2;

    chn_attr->attr.line_chn.color = RGN_RGB888_BLUE;
    for (i = LINE_MIN_HANDLE; i < LINE_MIN_HANDLE + handle_num; i++) {
        chn_attr->attr.line_chn.points[0].x = OFFSET_NUM0 + OFFSET_NUM2 * (i - LINE_MIN_HANDLE);
        chn_attr->attr.line_chn.points[0].y = OFFSET_NUM0 + OFFSET_NUM0 * (i - LINE_MIN_HANDLE);
        chn_attr->attr.line_chn.points[1].x = OFFSET_NUM3 + OFFSET_NUM0 * (i - LINE_MIN_HANDLE);
        chn_attr->attr.line_chn.points[1].y = OFFSET_NUM3 + OFFSET_NUM2 * (i - LINE_MIN_HANDLE);
        chn_attr->attr.line_chn.thick = i % HI_RGN_LINE_MAX_THICK;
        ret = sample_region_attach_to_chn(i, mpp_chn, chn_attr);
        if (ret != HI_SUCCESS) {
            SAMPLE_PRT("sample_region_attach_to_chn failed!\n");
            sample_comm_region_detach_frm_chn(i - LINE_MIN_HANDLE + 1, HI_RGN_LINE, mpp_chn);
            return ret;
        }
    }
    return HI_SUCCESS;
}

hi_s32 sample_region_get_mosaic_chn_attr(hi_s32 handle_num,
    hi_rgn_chn_attr *chn_attr, hi_mpp_chn *mpp_chn)
{
    hi_s32 i, ret;

    chn_attr->type = HI_RGN_MOSAIC;
    chn_attr->attr.mosaic_chn.blk_size = HI_MOSAIC_BLK_SIZE_32;
    chn_attr->attr.mosaic_chn.rect.height = OFFSET_NUM0;
    chn_attr->attr.mosaic_chn.rect.width = OFFSET_NUM0;

    for (i = MOSAIC_MIN_HANDLE; i < MOSAIC_MIN_HANDLE + handle_num; i++) {
        chn_attr->attr.mosaic_chn.rect.x = OFFSET_NUM0 * (i - MOSAIC_MIN_HANDLE);
        chn_attr->attr.mosaic_chn.rect.y = OFFSET_NUM0 * (i - MOSAIC_MIN_HANDLE);
        chn_attr->attr.mosaic_chn.layer = i - MOSAIC_MIN_HANDLE;
        ret = sample_region_attach_to_chn(i, mpp_chn, chn_attr);
        if (ret != HI_SUCCESS) {
            SAMPLE_PRT("sample_region_attach_to_chn failed!\n");
            sample_comm_region_detach_frm_chn(i - MOSAIC_MIN_HANDLE + 1, HI_RGN_MOSAIC, mpp_chn);
            return ret;
        }
    }
    return HI_SUCCESS;
}

hi_s32 sample_region_get_mosaicex_chn_attr(hi_s32 handle_num,
    hi_rgn_chn_attr *chn_attr, hi_mpp_chn *mpp_chn)
{
    hi_s32 i, ret;

    chn_attr->type = HI_RGN_MOSAICEX;
    chn_attr->attr.mosaicex_chn.blk_size = HI_MOSAIC_BLK_SIZE_32;
    chn_attr->attr.mosaicex_chn.rect.height = OFFSET_NUM0;
    chn_attr->attr.mosaicex_chn.rect.width = OFFSET_NUM0;

    for (i = MOSAICEX_MIN_HANDLE; i < MOSAICEX_MIN_HANDLE + handle_num; i++) {
        chn_attr->attr.mosaicex_chn.rect.x = OFFSET_NUM0 * (i - MOSAICEX_MIN_HANDLE);
        chn_attr->attr.mosaicex_chn.rect.y = OFFSET_NUM0 * (i - MOSAICEX_MIN_HANDLE);
        chn_attr->attr.mosaicex_chn.layer = i - MOSAICEX_MIN_HANDLE;
        chn_attr->attr.mosaicex_chn.blk_size = (i - MOSAICEX_MIN_HANDLE) % 2 + HI_MOSAIC_BLK_SIZE_8; /* 2:blk_size */
        ret = sample_region_attach_to_chn(i, mpp_chn, chn_attr);
        if (ret != HI_SUCCESS) {
            SAMPLE_PRT("sample_region_attach_to_chn failed!\n");
            sample_comm_region_detach_frm_chn(i - MOSAICEX_MIN_HANDLE + 1, HI_RGN_MOSAICEX, mpp_chn);
            return ret;
        }
    }
    return HI_SUCCESS;
}

hi_s32 sample_region_set_chn_attr(hi_s32 handle_num, hi_rgn_type type, hi_rgn_chn_attr *chn_attr,
    hi_mpp_chn *chn)
{
    hi_s32 ret = HI_SUCCESS;
    switch (type) {
        case HI_RGN_OVERLAY:
            ret = sample_region_get_overlay_chn_attr(handle_num, chn_attr, chn);
            break;
        case HI_RGN_OVERLAYEX:
            ret = sample_region_get_overlayex_chn_attr(handle_num, chn_attr, chn);
            break;
        case HI_RGN_COVER:
            ret = sample_region_get_cover_chn_attr(handle_num, chn_attr, chn);
            break;
        case HI_RGN_COVEREX:
            ret = sample_region_get_coverex_chn_attr(handle_num, chn_attr, chn);
            break;
        case HI_RGN_LINE:
            ret = sample_region_get_line_chn_attr(handle_num, chn_attr, chn);
            break;
        case HI_RGN_MOSAIC:
            ret = sample_region_get_mosaic_chn_attr(handle_num, chn_attr, chn);
            break;
        case HI_RGN_MOSAICEX:
            ret = sample_region_get_mosaicex_chn_attr(handle_num, chn_attr, chn);
            break;
        default:
            break;
    }
    return ret;
}

hi_s32 sample_comm_region_attach_to_chn(hi_s32 handle_num, hi_rgn_type type, hi_mpp_chn *mpp_chn)
{
    hi_s32 ret;
    hi_rgn_chn_attr chn_attr;
    ret = sample_comm_region_attach_check(handle_num, type, mpp_chn);
    if (ret != HI_SUCCESS) {
        return ret;
    }
    /* set the chn config */
    chn_attr.is_show = HI_TRUE;
    ret = sample_region_set_chn_attr(handle_num, type, &chn_attr, mpp_chn);
    if (ret != HI_SUCCESS) {
        SAMPLE_PRT("sample_region_attach_to_chn failed!\n");
        return ret;
    }
    return ret;
}

hi_s32 sample_comm_region_detach_frm_chn(hi_s32 handle_num, hi_rgn_type type, hi_mpp_chn *mpp_chn)
{
    hi_s32 i;
    hi_s32 ret = HI_SUCCESS;
    hi_s32 min_handle;

    if (handle_num <= 0 || handle_num > 16) { /* 16:max region num */
        SAMPLE_PRT("handle_num is illegal %d!\n", handle_num);
        return HI_FAILURE;
    }
    if (type < 0 || type >= HI_RGN_BUTT) {
        SAMPLE_PRT("type is illegal %d!\n", type);
        return HI_FAILURE;
    }
    if (mpp_chn == HI_NULL) {
        SAMPLE_PRT("mpp_chn is NULL !\n");
        return HI_FAILURE;
    }
    min_handle = sample_comm_region_get_min_handle(type);
    for (i = min_handle; i < min_handle + handle_num; i++) {
        ret = sample_region_detach_from_chn(i, mpp_chn);
        if (ret != HI_SUCCESS) {
            SAMPLE_PRT("sample_region_detach_from_chn failed! handle:%d\n", i);
        }
    }
    return ret;
}

hi_s32 sample_comm_region_set_bit_map(hi_rgn_handle handle, const hi_char *bmp_path)
{
    hi_s32 ret;
    hi_bmp bitmap;

    (hi_void)memset_s(&bitmap, sizeof(hi_bmp), 0, sizeof(hi_bmp));
    if (handle < OVERLAY_MIN_HANDLE + 2) { /* 2: create clut2 region */
        ret = region_mst_load_bmp(bmp_path, &bitmap, HI_FALSE, 0, HI_PIXEL_FORMAT_ARGB_CLUT2);
        if (ret != HI_SUCCESS) {
            SAMPLE_PRT("region_mst_load_bmp failed!handle\n");
        }
        ret = sample_region_set_bit_map(handle, &bitmap);
    } else if (handle < OVERLAY_MIN_HANDLE + 4 && handle >= OVERLAY_MIN_HANDLE + 2) { /* 4:2:create argbclut4 region */
        ret = region_mst_load_bmp(bmp_path, &bitmap, HI_FALSE, 0, HI_PIXEL_FORMAT_ARGB_CLUT4);
        if (ret != HI_SUCCESS) {
            SAMPLE_PRT("region_mst_load_bmp failed!handle\n");
        }
        ret = sample_region_set_bit_map(handle, &bitmap);
    } else if (handle < OVERLAY_MIN_HANDLE + 6 && handle >= OVERLAY_MIN_HANDLE + 4) { /* 6:4:create argb1555 region */
        ret = region_mst_load_bmp(bmp_path, &bitmap, HI_FALSE, 0, HI_PIXEL_FORMAT_ARGB_1555);
        if (ret != HI_SUCCESS) {
            SAMPLE_PRT("region_mst_load_bmp failed!handle\n");
        }
        ret = sample_region_set_bit_map(handle, &bitmap);
    } else if (handle < OVERLAY_MIN_HANDLE + 8 && handle >= OVERLAY_MIN_HANDLE + 6) { /* 8:6:create argb4444 region */
        ret = region_mst_load_bmp(bmp_path, &bitmap, HI_FALSE, 0, HI_PIXEL_FORMAT_ARGB_4444);
        if (ret != HI_SUCCESS) {
            SAMPLE_PRT("region_mst_load_bmp failed!handle\n");
        }
        ret = sample_region_set_bit_map(handle, &bitmap);
    } else {
        ret = HI_FALSE;
    }
    if (ret != HI_SUCCESS) {
        SAMPLE_PRT("sample_region_set_bit_map failed!handle:%d\n", handle);
    }
    free(bitmap.data);
    return ret;
}

hi_s32 sample_comm_region_get_up_canvas(hi_rgn_handle handle, const hi_char *bmp_path)
{
    hi_s32 ret;
    hi_size size;
    hi_bmp bmp;
    hi_rgn_canvas_info canvas_info;
    hi_pixel_format pixel_format;

    ret = hi_mpi_rgn_get_canvas_info(handle, &canvas_info);
    if (ret != HI_SUCCESS) {
        SAMPLE_PRT("hi_mpi_rgn_get_canvas_info failed with %#x!\n", ret);
        return HI_FAILURE;
    }

    pixel_format = canvas_info.pixel_format;
    bmp.data = canvas_info.virt_addr;
    size.width = canvas_info.size.width;
    size.height = canvas_info.size.height;
    ret = region_mst_update_canvas(bmp_path, &bmp, HI_FALSE, 0, &size, canvas_info.stride, pixel_format);
    if (ret != HI_SUCCESS) {
        SAMPLE_PRT("region_mst_update_canvas failed with %#x!\n", ret);
        return HI_FAILURE;
    }

    ret = hi_mpi_rgn_update_canvas(handle);
    if (ret != HI_SUCCESS) {
        SAMPLE_PRT("hi_mpi_rgn_update_canvas failed with %#x!\n", ret);
        return HI_FAILURE;
    }

    return ret;
}

