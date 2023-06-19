/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
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
#define CORNER_RECTEX_MIN_HANDLE 140

#define DEFAULT_CANVAS_NUM 2

#define BITS_NUM_PER_BYTE 8
#define BYTE_PER_PIX_1555 2
#define BYTE_PER_PIX_8888 4
#define PIX_PER_BYTE_CLUT2 4
#define PIX_PER_BYTE_CLUT4 2
#define OFFSET_NUM0 200
#define OFFSET_NUM1 160
#define OFFSET_NUM2 100
#define OFFSET_NUM3 50
#define OFFSET_NUM4 16

#define MAX_BIT_COUNT 32

#define RGN_DEFAULT_WIDTH 160
#define RGN_DEFAULT_HEIGHT 160
#define RGN_ALPHA 128
#define RGN_ARGB8888_BLUE 0xff0000ff
#define RGN_RGB888_BLUE 0x0000ff
#define RGN_RGB888_RED  0xff0000

typedef struct {
    const hi_char *filename;
    hi_bmp bmp;
    hi_bool fil;
    hi_u32 fil_color;
    hi_pixel_format pixel_fmt;
} rgn_load_bmp_info;

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
    hi_u8 *c_temp = HI_NULL;
    hi_u32 i, j, k;
    hi_s32 value;
    hi_u8 value_temp;

    temp = (hi_u16 *)bmp->data;

    clut_data = malloc(bmp->height * bmp->width / PIX_PER_BYTE_CLUT2);
    if (clut_data == NULL) {
        printf("malloc osd memory err!\n");
        return HI_NULL;
    }

    c_temp = (hi_u8 *)clut_data;
    for (i = 0; i < bmp->height; i++) {
        for (j = 0; j < bmp->width / PIX_PER_BYTE_CLUT2; j++) {
            value = 0;
            for (k = 0; k < PIX_PER_BYTE_CLUT2; k++) {
                value_temp = ((*temp & 0x001f) * 28 + ((*temp >> 5) & 0x001f) * 58 + /* 0x001f:28:5:58:color modulus */
                    ((*temp >> 10) & 0x001f) * 14) / 800; /* 10:0x001f:14:800:color modulus */
                value_temp = value_temp << (2 * (PIX_PER_BYTE_CLUT2 - k - 1)); /* 2:color modulus */
                value += value_temp;
                temp++;
            }
            *c_temp = value;
            c_temp++;
        }
    }

    return clut_data;
}

static hi_u8 *region_mst_get_clut4_data_from_bmp_data(hi_bmp *bmp)
{
    hi_u8 *clut_data = NULL;
    hi_u16 *temp = HI_NULL;
    hi_u8 *c_temp = HI_NULL;
    hi_u32 i, j, k;
    hi_s32 value;
    hi_u8 value_temp;

    temp = (hi_u16 *)bmp->data;

    clut_data = malloc(bmp->height * bmp->width / PIX_PER_BYTE_CLUT4);
    if (clut_data == NULL) {
        printf("malloc osd memory err!\n");
        return HI_NULL;
    }

    c_temp = (hi_u8 *)clut_data;
    for (i = 0; i < bmp->height; i++) {
        for (j = 0; j < bmp->width / PIX_PER_BYTE_CLUT4; j++) {
            value = 0;
            for (k = j; k < j + PIX_PER_BYTE_CLUT4; k++) {
                value_temp = ((*temp & 0x001f) * 28 + ((*temp >> 5) & 0x001f) * 58 + /* 0x001f:28:5:58:color modulus */
                    ((*temp >> 10) & 0x001f) * 14) / 200; /* 10:0x001f:14:200:color modulus */
                temp++;
                value = (value_temp << 4) + value_temp; /* 4:color modulus */
            }
            *c_temp = value;
            c_temp++;
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

hi_s32 region_mst_load_bmp(rgn_load_bmp_info *load_bmp_info)
{
    osd_surface surface;
    osd_bit_map_file_header bmp_file_header;
    osd_bit_map_info bmp_info;
    hi_u32 bpp;
    hi_u8 *clut_data = NULL;
    if (get_bmp_info(load_bmp_info->filename, &bmp_file_header, &bmp_info) < 0) {
        printf("get_bmp_info err!\n");
        return HI_FAILURE;
    }

    if (bmp_info.bmp_header.bi_bit_count > MAX_BIT_COUNT || bmp_info.bmp_header.bi_width > HI_RGN_OVERLAY_MAX_WIDTH ||
        bmp_info.bmp_header.bi_height > HI_RGN_OVERLAY_MAX_HEIGHT) {
        printf("bmp info error!");
        return HI_FAILURE;
    }
    surface.color_format = region_mst_get_color_format_by_pixel_format(load_bmp_info->pixel_fmt);
    if (surface.color_format == OSD_COLOR_FORMAT_BUTT) {
        return HI_FAILURE;
    }
    bpp = bmp_info.bmp_header.bi_bit_count / BITS_NUM_PER_BYTE;
    load_bmp_info->bmp.data = malloc(bmp_info.bmp_header.bi_width * bpp * abs(bmp_info.bmp_header.bi_height));
    if (load_bmp_info->bmp.data == NULL) {
        printf("malloc osd memory err!\n");
        return HI_FAILURE;
    }

    create_surface_by_bit_map(load_bmp_info->filename, &surface, (hi_u8 *)(load_bmp_info->bmp.data));
    load_bmp_info->bmp.width = surface.width;
    load_bmp_info->bmp.height = surface.height;
    load_bmp_info->bmp.pixel_format = load_bmp_info->pixel_fmt;

    if (load_bmp_info->pixel_fmt == HI_PIXEL_FORMAT_ARGB_CLUT2) {
        clut_data = region_mst_get_clut2_data_from_bmp_data(&load_bmp_info->bmp);
    } else if (load_bmp_info->pixel_fmt == HI_PIXEL_FORMAT_ARGB_CLUT4) {
        clut_data = region_mst_get_clut4_data_from_bmp_data(&load_bmp_info->bmp);
    } else {
        fil_bmp(&load_bmp_info->bmp, load_bmp_info->fil, load_bmp_info->fil_color);
    }
    if (clut_data != NULL) {
        free(load_bmp_info->bmp.data);
        load_bmp_info->bmp.data = clut_data;
    }
    return HI_SUCCESS;
}

hi_s32 region_mst_update_canvas(rgn_load_bmp_info *load_bmp_info, hi_size *size, hi_u32 stride)
{
    osd_surface surface;
    osd_bit_map_file_header bmp_file_header;
    osd_bit_map_info bmp_info;
    canvas_size_info canvas_size;

    if (get_bmp_info(load_bmp_info->filename, &bmp_file_header, &bmp_info) < 0) {
        printf("get_bmp_info err!\n");
        return HI_FAILURE;
    }

    if (HI_PIXEL_FORMAT_ARGB_1555 == load_bmp_info->pixel_fmt) {
        surface.color_format = OSD_COLOR_FORMAT_RGB1555;
    } else if (HI_PIXEL_FORMAT_ARGB_4444 == load_bmp_info->pixel_fmt) {
        surface.color_format = OSD_COLOR_FORMAT_RGB4444;
    } else if (HI_PIXEL_FORMAT_ARGB_8888 == load_bmp_info->pixel_fmt) {
        surface.color_format = OSD_COLOR_FORMAT_RGB8888;
    } else if (HI_PIXEL_FORMAT_ARGB_CLUT2 == load_bmp_info->pixel_fmt) {
        surface.color_format = OSD_COLOR_FORMAT_CLUT4;
    } else if (HI_PIXEL_FORMAT_ARGB_CLUT4 == load_bmp_info->pixel_fmt) {
        surface.color_format = OSD_COLOR_FORMAT_CLUT4;
    } else {
        printf("pixel format is not support!\n");
        return HI_FAILURE;
    }

    if (load_bmp_info->bmp.data == NULL) {
        printf("malloc osd memory err!\n");
        return HI_FAILURE;
    }

    canvas_size.width = size->width;
    canvas_size.height = size->height;
    canvas_size.stride = stride;
    create_surface_by_canvas(load_bmp_info->filename, &surface, (hi_u8 *)(load_bmp_info->bmp.data), &canvas_size);

    load_bmp_info->bmp.width = surface.width;
    load_bmp_info->bmp.height = surface.height;

    if (HI_PIXEL_FORMAT_ARGB_1555 == load_bmp_info->pixel_fmt) {
        load_bmp_info->bmp.pixel_format = HI_PIXEL_FORMAT_ARGB_1555;
    } else if (HI_PIXEL_FORMAT_ARGB_4444 == load_bmp_info->pixel_fmt) {
        load_bmp_info->bmp.pixel_format = HI_PIXEL_FORMAT_ARGB_4444;
    } else if (HI_PIXEL_FORMAT_ARGB_8888 == load_bmp_info->pixel_fmt) {
        load_bmp_info->bmp.pixel_format = HI_PIXEL_FORMAT_ARGB_8888;
    } else if (HI_PIXEL_FORMAT_ARGB_CLUT2 == load_bmp_info->pixel_fmt) {
        load_bmp_info->bmp.pixel_format = HI_PIXEL_FORMAT_ARGB_CLUT2;
    } else if (HI_PIXEL_FORMAT_ARGB_CLUT4 == load_bmp_info->pixel_fmt) {
        load_bmp_info->bmp.pixel_format = HI_PIXEL_FORMAT_ARGB_CLUT4;
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
        case HI_RGN_CORNER_RECTEX:
            min_handle = CORNER_RECTEX_MIN_HANDLE;
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
    region.attr.overlay.size.height = RGN_DEFAULT_WIDTH;
    region.attr.overlay.size.width = RGN_DEFAULT_HEIGHT;
    region.attr.overlay.bg_color = 1;
    region.attr.overlay.canvas_num = DEFAULT_CANVAS_NUM;
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
        sample_print("hi_mpi_rgn_create failed with %#x!\n", ret);
        return HI_FAILURE;
    }
    return HI_SUCCESS;
}

hi_s32 sample_region_create_overlayex(hi_s32 handle_num)
{
    hi_s32 ret;
    hi_s32 i;
    hi_rgn_attr region;

    region.type = HI_RGN_OVERLAYEX;
    region.attr.overlayex.pixel_format = HI_PIXEL_FORMAT_ARGB_1555;
    region.attr.overlayex.size.height = RGN_DEFAULT_HEIGHT;
    region.attr.overlayex.size.width = RGN_DEFAULT_WIDTH;
    region.attr.overlayex.bg_color = 0x00ff;
    region.attr.overlayex.canvas_num = DEFAULT_CANVAS_NUM;
    for (i = OVERLAYEX_MIN_HANDLE; i < OVERLAYEX_MIN_HANDLE + handle_num; i++) {
        ret = hi_mpi_rgn_create(i, &region);
        if (ret != HI_SUCCESS) {
            sample_print("hi_mpi_rgn_create failed with %#x!\n", ret);
            return HI_FAILURE;
        }
    }

    return HI_SUCCESS;
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
            sample_print("hi_mpi_rgn_create failed with %#x!\n", ret);
            return HI_FAILURE;
        }
    }

    return HI_SUCCESS;
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
            sample_print("hi_mpi_rgn_create failed with %#x!\n", ret);
            return HI_FAILURE;
        }
    }

    return HI_SUCCESS;
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
            sample_print("hi_mpi_rgn_create failed with %#x!\n", ret);
            return HI_FAILURE;
        }
    }

    return HI_SUCCESS;
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
            sample_print("hi_mpi_rgn_create failed with %#x!\n", ret);
            return HI_FAILURE;
        }
    }

    return HI_SUCCESS;
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
            sample_print("hi_mpi_rgn_create failed with %#x!\n", ret);
            return HI_FAILURE;
        }
    }

    return HI_SUCCESS;
}

hi_s32 sample_region_create_corner_rectex(hi_s32 handle_num)
{
    hi_s32 ret;
    hi_s32 i;
    hi_rgn_attr region;

    region.type = HI_RGN_CORNER_RECTEX;

    for (i = CORNER_RECTEX_MIN_HANDLE; i < CORNER_RECTEX_MIN_HANDLE + handle_num; i++) {
        ret = hi_mpi_rgn_create(i, &region);
        if (ret != HI_SUCCESS) {
            sample_print("hi_mpi_rgn_create failed with %#x!\n", ret);
            return HI_FAILURE;
        }
    }

    return HI_SUCCESS;
}

hi_s32 sample_region_destroy(hi_rgn_handle handle)
{
    hi_s32 ret;

    ret = hi_mpi_rgn_destroy(handle);
    if (ret != HI_SUCCESS) {
        sample_print("hi_mpi_rgn_destroy failed with %#x!\n", ret);
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

hi_s32 sample_region_attach_to_chn(hi_rgn_handle handle, hi_mpp_chn *chn, hi_rgn_chn_attr *chn_attr)
{
    hi_s32 ret;

    ret = hi_mpi_rgn_attach_to_chn(handle, chn, chn_attr);
    if (ret != HI_SUCCESS) {
        sample_print("hi_mpi_rgn_attach_to_chn failed with %#x!\n", ret);
        return HI_FAILURE;
    }
    return HI_SUCCESS;
}

hi_s32 sample_region_detach_from_chn(hi_rgn_handle handle, hi_mpp_chn *chn)
{
    hi_s32 ret;
    ret = hi_mpi_rgn_detach_from_chn(handle, chn);
    if (ret != HI_SUCCESS) {
        sample_print("hi_mpi_rgn_detach_from_chn failed with %#x!\n", ret);
        return HI_FAILURE;
    }
    return HI_SUCCESS;
}

hi_s32 sample_region_set_bit_map(hi_rgn_handle handle, hi_bmp *bitmap)
{
    hi_s32 ret;
    ret = hi_mpi_rgn_set_bmp(handle, bitmap);
    if (ret != HI_SUCCESS) {
        sample_print("hi_mpi_rgn_set_bit_map failed with %#x!\n", ret);
        return HI_FAILURE;
    }
    return HI_SUCCESS;
}

hi_s32 sample_region_get_up_canvas_info(hi_rgn_handle handle, hi_rgn_canvas_info *canvas_info)
{
    hi_s32 ret;
    ret = hi_mpi_rgn_get_canvas_info(handle, canvas_info);
    if (ret != HI_SUCCESS) {
        sample_print("hi_mpi_rgn_get_canvas_info failed with %#x!\n", ret);
        return HI_FAILURE;
    }

    ret = hi_mpi_rgn_update_canvas(handle);
    if (ret != HI_SUCCESS) {
        sample_print("hi_mpi_rgn_update_canvas failed with %#x!\n", ret);
        return HI_FAILURE;
    }
    return HI_SUCCESS;
}

hi_s32 sample_comm_region_create(hi_s32 handle_num, hi_rgn_type type)
{
    hi_s32 ret = HI_SUCCESS;
    if (handle_num <= 0 || handle_num > 16) { /* 16:max_num */
        sample_print("handle_num is illegal %d!\n", handle_num);
        return HI_FAILURE;
    }
    if (type < 0 || type >= HI_RGN_BUTT) {
        sample_print("type is illegal %d!\n", type);
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
        case HI_RGN_CORNER_RECTEX:
            ret = sample_region_create_corner_rectex(handle_num);
            break;
        default:
            break;
    }
    if (ret != HI_SUCCESS) {
        sample_print("sample_comm_region_create failed! handle_num%d,type:%d!\n", handle_num, type);
        return HI_FAILURE;
    }
    return ret;
}

hi_s32 sample_comm_region_destroy(hi_s32 handle_num, hi_rgn_type type)
{
    hi_s32 i;
    hi_s32 ret;
    hi_s32 min_handle;

    if (handle_num <= 0 || handle_num > 16) { /* 16 max_num */
        sample_print("handle_num is illegal %d!\n", handle_num);
        return HI_FAILURE;
    }
    if (type < 0 || type >= HI_RGN_BUTT) {
        sample_print("type is illegal %d!\n", type);
        return HI_FAILURE;
    }
    min_handle = sample_comm_region_get_min_handle(type);
    for (i = min_handle; i < min_handle + handle_num; i++) {
        ret = sample_region_destroy(i);
        if (ret != HI_SUCCESS) {
            sample_print("sample_comm_region_destroy failed!\n");
        }
    }
    return HI_SUCCESS;
}

hi_s32 sample_comm_region_attach_check(hi_s32 handle_num, hi_rgn_type type, hi_mpp_chn *mpp_chn)
{
    if (handle_num <= 0 || handle_num > 16) { /* 16 max_num */
        sample_print("handle_num is illegal %d!\n", handle_num);
        return HI_FAILURE;
    }
    if (type < 0 || type >= HI_RGN_BUTT) {
        sample_print("type is illegal %d!\n", type);
        return HI_FAILURE;
    }
    if (mpp_chn == HI_NULL) {
        sample_print("mpp_chn is NULL !\n");
        return HI_FAILURE;
    }
    return HI_SUCCESS;
}

#define rgn_check_handle_min_ret(handle, min_value) \
    do { \
        if ((handle) < (min_value)) { \
            sample_print("handle(%d) invalid\n", (handle)); \
            return; \
        } \
    } while (0)

static hi_void sample_region_get_overlay_chn_attr(hi_s32 handle, hi_rgn_overlay_chn_attr *overlay_chn)
{
    rgn_check_handle_min_ret(handle, OVERLAY_MIN_HANDLE);

    overlay_chn->bg_alpha = RGN_ALPHA;
    overlay_chn->fg_alpha = RGN_ALPHA;
    overlay_chn->qp_info.enable = HI_TRUE;
    overlay_chn->qp_info.is_abs_qp = HI_TRUE;
    overlay_chn->qp_info.qp_val = 0;
    overlay_chn->dst = HI_RGN_ATTACH_JPEG_MAIN;
    overlay_chn->point.x = OFFSET_NUM0 * (handle - OVERLAY_MIN_HANDLE) + OFFSET_NUM0;
    overlay_chn->point.y = OFFSET_NUM2 * (handle - OVERLAY_MIN_HANDLE);
    overlay_chn->layer = handle - OVERLAY_MIN_HANDLE;
}

hi_s32 sample_region_set_overlay_chn_attr(hi_s32 handle_num,
    hi_rgn_chn_attr *chn_attr, hi_mpp_chn *mpp_chn)
{
    hi_s32 i;
    hi_s32 ret;

    rgn_check_handle_num_return(handle_num);

    chn_attr->type = HI_RGN_OVERLAY;

    for (i = OVERLAY_MIN_HANDLE; i < OVERLAY_MIN_HANDLE + handle_num; i++) {
        sample_region_get_overlay_chn_attr(i, &chn_attr->attr.overlay_chn);
        ret = sample_region_attach_to_chn(i, mpp_chn, chn_attr);
        if (ret != HI_SUCCESS) {
            sample_print("sample_region_attach_to_chn failed!\n");
            sample_comm_region_detach_frm_chn(i - OVERLAY_MIN_HANDLE + 1, HI_RGN_OVERLAY, mpp_chn);
            return ret;
        }
    }
    return HI_SUCCESS;
}

static hi_void sample_region_get_overlayex_chn_attr(hi_s32 handle,
    hi_rgn_overlayex_chn_attr *overlayex_chn)
{
    rgn_check_handle_min_ret(handle, OVERLAYEX_MIN_HANDLE);

    overlayex_chn->bg_alpha = RGN_ALPHA;
    overlayex_chn->fg_alpha = RGN_ALPHA;
    overlayex_chn->point.x = OFFSET_NUM0 * (handle - OVERLAYEX_MIN_HANDLE) + OFFSET_NUM0;
    overlayex_chn->point.y = OFFSET_NUM2 * (handle - OVERLAYEX_MIN_HANDLE);
    overlayex_chn->layer = handle - OVERLAYEX_MIN_HANDLE;
}

hi_s32 sample_region_set_overlayex_chn_attr(hi_s32 handle_num,
    hi_rgn_chn_attr *chn_attr, hi_mpp_chn *mpp_chn)
{
    hi_s32 i;
    hi_s32 ret;

    rgn_check_handle_num_return(handle_num);

    chn_attr->type = HI_RGN_OVERLAYEX;

    for (i = OVERLAYEX_MIN_HANDLE; i < OVERLAYEX_MIN_HANDLE + handle_num; i++) {
        sample_region_get_overlayex_chn_attr(i, &chn_attr->attr.overlayex_chn);
        ret = sample_region_attach_to_chn(i, mpp_chn, chn_attr);
        if (ret != HI_SUCCESS) {
            sample_print("sample_region_attach_to_chn failed!\n");
            sample_comm_region_detach_frm_chn(i - OVERLAYEX_MIN_HANDLE + 1, HI_RGN_OVERLAYEX, mpp_chn);
            return ret;
        }
    }
    return HI_SUCCESS;
}

static hi_void sample_region_get_quad_cover_chn_attr(hi_s32 handle, hi_rgn_cover_chn_attr *cover_chn)
{
    rgn_check_handle_min_ret(handle, COVER_MIN_HANDLE);

    cover_chn->coord = HI_COORD_ABS;
    cover_chn->layer = handle - COVER_MIN_HANDLE;
    cover_chn->cover.type = HI_COVER_QUAD;
    cover_chn->cover.color = RGN_RGB888_RED;
    cover_chn->cover.quad.is_solid = handle % 2; /* 2:solid or not */
    cover_chn->cover.quad.thick = HI_RGN_COVER_MIN_THICK + (handle - COVER_MIN_HANDLE) %
        (HI_RGN_COVER_MAX_THICK - HI_RGN_COVER_MIN_THICK + 1);
    cover_chn->cover.quad.point[0].x = OFFSET_NUM0 * (handle - COVER_MIN_HANDLE);
    cover_chn->cover.quad.point[0].y = OFFSET_NUM3 + OFFSET_NUM3;
    cover_chn->cover.quad.point[1].x = OFFSET_NUM0 * (handle - COVER_MIN_HANDLE) + OFFSET_NUM2;
    cover_chn->cover.quad.point[1].y = OFFSET_NUM3;
    cover_chn->cover.quad.point[2].x = OFFSET_NUM0 * (handle - COVER_MIN_HANDLE) + OFFSET_NUM0; /* 2:point num */
    cover_chn->cover.quad.point[2].y = OFFSET_NUM3 + OFFSET_NUM3;                               /* 2:point num */
    cover_chn->cover.quad.point[3].x = OFFSET_NUM0 * (handle - COVER_MIN_HANDLE) + OFFSET_NUM2; /* 3:point num */
    cover_chn->cover.quad.point[3].y = OFFSET_NUM3 + OFFSET_NUM2;                               /* 3:point num */
}

static hi_void sample_region_get_rect_cover_chn_attr(hi_s32 handle, hi_rgn_cover_chn_attr *cover_chn)
{
    rgn_check_handle_min_ret(handle, COVER_MIN_HANDLE);

    cover_chn->coord = HI_COORD_ABS;
    cover_chn->layer = handle - COVER_MIN_HANDLE;

    cover_chn->cover.type = HI_COVER_RECT;
    cover_chn->cover.color = RGN_RGB888_BLUE;
    cover_chn->cover.rect.height = RGN_DEFAULT_HEIGHT;
    cover_chn->cover.rect.width = RGN_DEFAULT_WIDTH;
    cover_chn->cover.rect.x = OFFSET_NUM0 * (handle - COVER_MIN_HANDLE) + OFFSET_NUM0;
    cover_chn->cover.rect.y = OFFSET_NUM0;
}

hi_s32 sample_region_set_cover_chn_attr(hi_s32 handle_num,
    hi_rgn_chn_attr *chn_attr, hi_mpp_chn *mpp_chn)
{
    hi_s32 i;
    hi_s32 ret;

    rgn_check_handle_num_return(handle_num);

    chn_attr->type = HI_RGN_COVER;

    for (i = COVER_MIN_HANDLE; i < COVER_MIN_HANDLE + handle_num; i++) {
        if (i - COVER_MIN_HANDLE < 4) { /* 4:quad num */
            sample_region_get_quad_cover_chn_attr(i, &chn_attr->attr.cover_chn);
        } else {
            sample_region_get_rect_cover_chn_attr(i, &chn_attr->attr.cover_chn);
        }
        ret = sample_region_attach_to_chn(i, mpp_chn, chn_attr);
        if (ret != HI_SUCCESS) {
            sample_print("sample_region_attach_to_chn failed!\n");
            sample_comm_region_detach_frm_chn(i - COVER_MIN_HANDLE + 1, HI_RGN_COVER, mpp_chn);
            return ret;
        }
    }
    return HI_SUCCESS;
}

static hi_void sample_region_get_quad_coverex_chn_attr(hi_s32 handle,
    hi_rgn_coverex_chn_attr *coverex_chn)
{
    rgn_check_handle_min_ret(handle, COVEREX_MIN_HANDLE);

    coverex_chn->coord = HI_COORD_ABS;
    coverex_chn->layer = handle - COVEREX_MIN_HANDLE;
    coverex_chn->coverex.type = HI_COVER_QUAD;
    coverex_chn->coverex.color = RGN_RGB888_RED;
    coverex_chn->coverex.quad.is_solid = handle % 2; /* 2:solid or not */
    coverex_chn->coverex.quad.thick = HI_RGN_COVEREX_MIN_THICK + (handle - COVEREX_MIN_HANDLE) %
        (HI_RGN_COVEREX_MAX_THICK - HI_RGN_COVEREX_MIN_THICK + 1);
    coverex_chn->coverex.quad.point[0].x = OFFSET_NUM0 * (handle - COVEREX_MIN_HANDLE);
    coverex_chn->coverex.quad.point[0].y = OFFSET_NUM3 + OFFSET_NUM3;
    coverex_chn->coverex.quad.point[1].x = OFFSET_NUM0 * (handle - COVEREX_MIN_HANDLE) + OFFSET_NUM2;
    coverex_chn->coverex.quad.point[1].y = OFFSET_NUM3;
    coverex_chn->coverex.quad.point[2].x = OFFSET_NUM0 * (handle - COVEREX_MIN_HANDLE) + OFFSET_NUM0; /* 2:point num */
    coverex_chn->coverex.quad.point[2].y = OFFSET_NUM3 + OFFSET_NUM3;                                 /* 2:point num */
    coverex_chn->coverex.quad.point[3].x = OFFSET_NUM0 * (handle - COVEREX_MIN_HANDLE) + OFFSET_NUM2; /* 3:point num */
    coverex_chn->coverex.quad.point[3].y = OFFSET_NUM3 + OFFSET_NUM2;                                 /* 3:point num */
}

static hi_void sample_region_get_rect_coverex_chn_attr(hi_s32 handle,
    hi_rgn_coverex_chn_attr *coverex_chn)
{
    rgn_check_handle_min_ret(handle, COVEREX_MIN_HANDLE);

    coverex_chn->coord = HI_COORD_ABS;
    coverex_chn->layer = handle - COVEREX_MIN_HANDLE;
    coverex_chn->coverex.type = HI_COVER_RECT;
    coverex_chn->coverex.color = RGN_RGB888_BLUE;
    coverex_chn->coverex.rect.height = RGN_DEFAULT_HEIGHT;
    coverex_chn->coverex.rect.width = RGN_DEFAULT_WIDTH;
    coverex_chn->coverex.rect.x = OFFSET_NUM0 * (handle - COVEREX_MIN_HANDLE) + OFFSET_NUM0;
    coverex_chn->coverex.rect.y = OFFSET_NUM0;
}

hi_s32 sample_region_set_coverex_chn_attr(hi_s32 handle_num,
    hi_rgn_chn_attr *chn_attr, hi_mpp_chn *mpp_chn)
{
    hi_s32 i;
    hi_s32 ret;

    rgn_check_handle_num_return(handle_num);

    chn_attr->type = HI_RGN_COVEREX;

    for (i = COVEREX_MIN_HANDLE; i < COVEREX_MIN_HANDLE + handle_num; i++) {
        if (i - COVEREX_MIN_HANDLE < 4) { /* 4:quad num */
            sample_region_get_quad_coverex_chn_attr(i, &chn_attr->attr.coverex_chn);
        } else {
            sample_region_get_rect_coverex_chn_attr(i, &chn_attr->attr.coverex_chn);
        }
        ret = sample_region_attach_to_chn(i, mpp_chn, chn_attr);
        if (ret != HI_SUCCESS) {
            sample_print("sample_region_attach_to_chn failed!\n");
            sample_comm_region_detach_frm_chn(i - COVEREX_MIN_HANDLE + 1, HI_RGN_COVEREX, mpp_chn);
            return ret;
        }
    }
    return HI_SUCCESS;
}

static hi_void sample_region_get_corner_rectex_chn_attr(hi_s32 handle,
    hi_rgn_corner_rect_chn_attr *corner_rectex_chn)
{
    rgn_check_handle_min_ret(handle, CORNER_RECTEX_MIN_HANDLE);

    corner_rectex_chn->corner_rect.rect.height = RGN_DEFAULT_HEIGHT;
    corner_rectex_chn->corner_rect.rect.width = RGN_DEFAULT_WIDTH;
    corner_rectex_chn->corner_rect.thick = HI_RGN_CORNER_RECTEX_MIN_THICK;
    corner_rectex_chn->corner_rect.hor_len = OFFSET_NUM4;
    corner_rectex_chn->corner_rect.ver_len = OFFSET_NUM4;
    corner_rectex_chn->corner_rect_attr.color = RGN_RGB888_RED;
    corner_rectex_chn->corner_rect_attr.corner_rect_type = HI_CORNER_RECT_TYPE_CORNER;
    corner_rectex_chn->layer = handle - CORNER_RECTEX_MIN_HANDLE;
    corner_rectex_chn->corner_rect.rect.x = OFFSET_NUM0 * (handle - CORNER_RECTEX_MIN_HANDLE) + OFFSET_NUM3;
    corner_rectex_chn->corner_rect.rect.y = OFFSET_NUM2 * (handle - CORNER_RECTEX_MIN_HANDLE) + OFFSET_NUM3;
}

hi_s32 sample_region_set_corner_rectex_chn_attr(hi_s32 handle_num,
    hi_rgn_chn_attr *chn_attr, hi_mpp_chn *mpp_chn)
{
    hi_s32 i;
    hi_s32 ret;

    rgn_check_handle_num_return(handle_num);

    chn_attr->type = HI_RGN_CORNER_RECTEX;

    for (i = CORNER_RECTEX_MIN_HANDLE; i < CORNER_RECTEX_MIN_HANDLE + handle_num; i++) {
        sample_region_get_corner_rectex_chn_attr(i, &chn_attr->attr.corner_rectex_chn);
        if (i - CORNER_RECTEX_MIN_HANDLE < 4) { /* 4:corner type number */
            chn_attr->attr.corner_rectex_chn.corner_rect_attr.corner_rect_type = HI_CORNER_RECT_TYPE_CORNER;
        } else {
            chn_attr->attr.corner_rectex_chn.corner_rect_attr.corner_rect_type = HI_CORNER_RECT_TYPE_FULL_LINE;
        }
        ret = sample_region_attach_to_chn(i, mpp_chn, chn_attr);
        if (ret != HI_SUCCESS) {
            sample_print("sample_region_attach_to_chn failed!\n");
            sample_comm_region_detach_frm_chn(i - CORNER_RECTEX_MIN_HANDLE + 1, HI_RGN_CORNER_RECTEX, mpp_chn);
            return ret;
        }
    }
    return HI_SUCCESS;
}

static hi_void sample_region_get_line_chn_attr(hi_s32 handle, hi_rgn_line_chn_attr *line_chn)
{
    rgn_check_handle_min_ret(handle, LINE_MIN_HANDLE);

    line_chn->color = RGN_RGB888_BLUE;
    line_chn->points[0].x = OFFSET_NUM0 + OFFSET_NUM2 * (handle - LINE_MIN_HANDLE);
    line_chn->points[0].y = OFFSET_NUM0 + OFFSET_NUM0 * (handle - LINE_MIN_HANDLE);
    line_chn->points[1].x = OFFSET_NUM3 + OFFSET_NUM0 * (handle - LINE_MIN_HANDLE);
    line_chn->points[1].y = OFFSET_NUM3 + OFFSET_NUM2 * (handle - LINE_MIN_HANDLE);
    line_chn->thick = (handle - LINE_MIN_HANDLE + HI_RGN_LINE_MIN_THICK) % HI_RGN_LINE_MAX_THICK;
}

static hi_s32 sample_region_set_line_chn_attr(hi_s32 handle_num,
    hi_rgn_chn_attr *chn_attr, hi_mpp_chn *mpp_chn)
{
    hi_s32 i;
    hi_s32 ret;

    rgn_check_handle_num_return(handle_num);

    chn_attr->type = HI_RGN_LINE;

    for (i = LINE_MIN_HANDLE; i < LINE_MIN_HANDLE + handle_num; i++) {
        sample_region_get_line_chn_attr(i, &chn_attr->attr.line_chn);
        ret = sample_region_attach_to_chn(i, mpp_chn, chn_attr);
        if (ret != HI_SUCCESS) {
            sample_print("sample_region_attach_to_chn failed!\n");
            sample_comm_region_detach_frm_chn(i - LINE_MIN_HANDLE + 1, HI_RGN_LINE, mpp_chn);
            return ret;
        }
    }
    return HI_SUCCESS;
}

static hi_void sample_region_get_mosaic_chn_attr(hi_s32 handle, hi_rgn_mosaic_chn_attr *mosaic_chn)
{
    rgn_check_handle_min_ret(handle, MOSAIC_MIN_HANDLE);

    mosaic_chn->blk_size = HI_MOSAIC_BLK_SIZE_32;
    mosaic_chn->rect.height = OFFSET_NUM0;
    mosaic_chn->rect.width = OFFSET_NUM0;
    mosaic_chn->rect.x = OFFSET_NUM0 * (handle - MOSAIC_MIN_HANDLE);
    mosaic_chn->rect.y = OFFSET_NUM0 * (handle - MOSAIC_MIN_HANDLE);
    mosaic_chn->layer = handle - MOSAIC_MIN_HANDLE;
}

static hi_s32 sample_region_set_mosaic_chn_attr(hi_s32 handle_num,
    hi_rgn_chn_attr *chn_attr, hi_mpp_chn *mpp_chn)
{
    hi_s32 i;
    hi_s32 ret;

    rgn_check_handle_num_return(handle_num);

    chn_attr->type = HI_RGN_MOSAIC;

    for (i = MOSAIC_MIN_HANDLE; i < MOSAIC_MIN_HANDLE + handle_num; i++) {
        sample_region_get_mosaic_chn_attr(i, &chn_attr->attr.mosaic_chn);
        ret = sample_region_attach_to_chn(i, mpp_chn, chn_attr);
        if (ret != HI_SUCCESS) {
            sample_print("sample_region_attach_to_chn failed!\n");
            sample_comm_region_detach_frm_chn(i - MOSAIC_MIN_HANDLE + 1, HI_RGN_MOSAIC, mpp_chn);
            return ret;
        }
    }
    return HI_SUCCESS;
}

static hi_void sample_region_get_mosaicex_chn_attr(hi_s32 handle, hi_rgn_mosaicex_chn_attr *mosaicex_chn)
{
    rgn_check_handle_min_ret(handle, MOSAICEX_MIN_HANDLE);

    mosaicex_chn->blk_size = HI_MOSAIC_BLK_SIZE_16 + (handle - MOSAICEX_MIN_HANDLE) %
        (HI_MOSAIC_BLK_SIZE_64 - HI_MOSAIC_BLK_SIZE_16 + 1);
    mosaicex_chn->rect.height = OFFSET_NUM0;
    mosaicex_chn->rect.width = OFFSET_NUM0;
    mosaicex_chn->rect.x = OFFSET_NUM0 * (handle - MOSAICEX_MIN_HANDLE);
    mosaicex_chn->rect.y = OFFSET_NUM0 * (handle - MOSAICEX_MIN_HANDLE);
    mosaicex_chn->layer = handle - MOSAICEX_MIN_HANDLE;
}

hi_s32 sample_region_set_mosaicex_chn_attr(hi_s32 handle_num,
    hi_rgn_chn_attr *chn_attr, hi_mpp_chn *mpp_chn)
{
    hi_s32 i;
    hi_s32 ret;

    rgn_check_handle_num_return(handle_num);

    chn_attr->type = HI_RGN_MOSAICEX;

    for (i = MOSAICEX_MIN_HANDLE; i < MOSAICEX_MIN_HANDLE + handle_num; i++) {
        sample_region_get_mosaicex_chn_attr(i, &chn_attr->attr.mosaicex_chn);
        ret = sample_region_attach_to_chn(i, mpp_chn, chn_attr);
        if (ret != HI_SUCCESS) {
            sample_print("sample_region_attach_to_chn failed!\n");
            sample_comm_region_detach_frm_chn(i - MOSAICEX_MIN_HANDLE + 1, MOSAICEX_MIN_HANDLE, mpp_chn);
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
            ret = sample_region_set_overlay_chn_attr(handle_num, chn_attr, chn);
            break;
        case HI_RGN_OVERLAYEX:
            ret = sample_region_set_overlayex_chn_attr(handle_num, chn_attr, chn);
            break;
        case HI_RGN_COVER:
            ret = sample_region_set_cover_chn_attr(handle_num, chn_attr, chn);
            break;
        case HI_RGN_COVEREX:
            ret = sample_region_set_coverex_chn_attr(handle_num, chn_attr, chn);
            break;
        case HI_RGN_LINE:
            ret = sample_region_set_line_chn_attr(handle_num, chn_attr, chn);
            break;
        case HI_RGN_MOSAIC:
            ret = sample_region_set_mosaic_chn_attr(handle_num, chn_attr, chn);
            break;
        case HI_RGN_MOSAICEX:
            ret = sample_region_set_mosaicex_chn_attr(handle_num, chn_attr, chn);
            break;
        case HI_RGN_CORNER_RECTEX:
            ret = sample_region_set_corner_rectex_chn_attr(handle_num, chn_attr, chn);
            break;
        default:
            break;
    }
    return ret;
}

hi_s32 sample_comm_region_attach_to_chn(hi_s32 handle_num, hi_rgn_type type, hi_mpp_chn *mpp_chn)
{
    hi_s32 ret;
    hi_rgn_chn_attr chn_attr = {0};
    ret = sample_comm_region_attach_check(handle_num, type, mpp_chn);
    if (ret != HI_SUCCESS) {
        return ret;
    }
    /* set the chn config */
    chn_attr.is_show = HI_TRUE;
    ret = sample_region_set_chn_attr(handle_num, type, &chn_attr, mpp_chn);
    if (ret != HI_SUCCESS) {
        sample_print("sample_region_attach_to_chn failed!\n");
    }
    return ret;
}

hi_s32 sample_comm_check_min(hi_s32 min_handle)
{
    if ((min_handle <= HI_INVALID_HANDLE) || (min_handle > HI_RGN_HANDLE_MAX)) {
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

hi_s32 sample_comm_region_detach_frm_chn(hi_s32 handle_num, hi_rgn_type type, hi_mpp_chn *mpp_chn)
{
    hi_s32 i;
    hi_s32 ret;
    hi_s32 min_handle;

    if (handle_num <= 0 || handle_num > 16) { /* 16:max region num */
        sample_print("handle_num is illegal %d!\n", handle_num);
        return HI_FAILURE;
    }
    if (type < 0 || type >= HI_RGN_BUTT) {
        sample_print("type is illegal %d!\n", type);
        return HI_FAILURE;
    }
    if (mpp_chn == HI_NULL) {
        sample_print("mpp_chn is NULL !\n");
        return HI_FAILURE;
    }
    min_handle = sample_comm_region_get_min_handle(type);
    if (sample_comm_check_min(min_handle) != HI_SUCCESS) {
        sample_print("min_handle(%d) should be in [0, %d).\n", min_handle, HI_RGN_HANDLE_MAX);
        return HI_FAILURE;
    }
    for (i = min_handle; i < min_handle + handle_num; i++) {
        ret = sample_region_detach_from_chn(i, mpp_chn);
        if (ret != HI_SUCCESS) {
            sample_print("sample_region_detach_from_chn failed! handle:%d\n", i);
        }
    }
    return HI_SUCCESS;
}

hi_s32 sample_comm_region_set_bit_map(hi_rgn_handle handle, const hi_char *bmp_path)
{
    hi_s32 ret;
    rgn_load_bmp_info load_bmp_info = {0};

    load_bmp_info.filename = bmp_path;
    load_bmp_info.fil = HI_FALSE;
    load_bmp_info.fil_color = 0;
    if (handle < OVERLAY_MIN_HANDLE + 2) { /* 2: create clut2 region */
        load_bmp_info.pixel_fmt = HI_PIXEL_FORMAT_ARGB_CLUT2;
        ret = region_mst_load_bmp(&load_bmp_info);
        if (ret != HI_SUCCESS) {
            sample_print("region_mst_load_bmp failed!handle\n");
        }
        ret = sample_region_set_bit_map(handle, &load_bmp_info.bmp);
    } else if (handle < OVERLAY_MIN_HANDLE + 4 && handle >= OVERLAY_MIN_HANDLE + 2) { /* 4:2:create argbclut4 region */
        load_bmp_info.pixel_fmt = HI_PIXEL_FORMAT_ARGB_CLUT4;
        ret = region_mst_load_bmp(&load_bmp_info);
        if (ret != HI_SUCCESS) {
            sample_print("region_mst_load_bmp failed!handle\n");
        }
        ret = sample_region_set_bit_map(handle, &load_bmp_info.bmp);
    } else if (handle < OVERLAY_MIN_HANDLE + 6 && handle >= OVERLAY_MIN_HANDLE + 4) { /* 6:4:create argb1555 region */
        load_bmp_info.pixel_fmt = HI_PIXEL_FORMAT_ARGB_1555;
        ret = region_mst_load_bmp(&load_bmp_info);
        if (ret != HI_SUCCESS) {
            sample_print("region_mst_load_bmp failed!handle\n");
        }
        ret = sample_region_set_bit_map(handle, &load_bmp_info.bmp);
    } else if (handle < OVERLAY_MIN_HANDLE + 8 && handle >= OVERLAY_MIN_HANDLE + 6) { /* 8:6:create argb4444 region */
        load_bmp_info.pixel_fmt = HI_PIXEL_FORMAT_ARGB_4444;
        ret = region_mst_load_bmp(&load_bmp_info);
        if (ret != HI_SUCCESS) {
            sample_print("region_mst_load_bmp failed!handle\n");
        }
        ret = sample_region_set_bit_map(handle, &load_bmp_info.bmp);
    } else {
        ret = HI_FALSE;
    }
    if (ret != HI_SUCCESS) {
        sample_print("sample_region_set_bit_map failed!handle:%d\n", handle);
    }
    free(load_bmp_info.bmp.data);
    return ret;
}

hi_s32 sample_comm_region_get_up_canvas(hi_rgn_handle handle, const hi_char *bmp_path)
{
    hi_s32 ret;
    hi_size size;
    hi_rgn_canvas_info canvas_info;
    rgn_load_bmp_info load_bmp_info = {0};

    ret = hi_mpi_rgn_get_canvas_info(handle, &canvas_info);
    if (ret != HI_SUCCESS) {
        sample_print("hi_mpi_rgn_get_canvas_info failed with %#x!\n", ret);
        return HI_FAILURE;
    }

    size.width = canvas_info.size.width;
    size.height = canvas_info.size.height;
    load_bmp_info.bmp.data = canvas_info.virt_addr;
    load_bmp_info.pixel_fmt = canvas_info.pixel_format;
    load_bmp_info.fil = HI_FALSE;
    load_bmp_info.fil_color = 0;
    load_bmp_info.filename = bmp_path;
    ret = region_mst_update_canvas(&load_bmp_info, &size, canvas_info.stride);
    if (ret != HI_SUCCESS) {
        sample_print("region_mst_update_canvas failed with %#x!\n", ret);
        return HI_FAILURE;
    }

    ret = hi_mpi_rgn_update_canvas(handle);
    if (ret != HI_SUCCESS) {
        sample_print("hi_mpi_rgn_update_canvas failed with %#x!\n", ret);
        return HI_FAILURE;
    }

    return ret;
}
