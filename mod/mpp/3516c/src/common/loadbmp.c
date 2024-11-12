/*
  Copyright (c), 2001-2024, Shenshu Tech. Co., Ltd.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include "securec.h"
#include "loadbmp.h"

#define BITS_NUM_PER_BYTE 8
#define BYTE_PER_PIX_1555 2
#define BYTE_PER_PIX_8888 4
#define PIX_PER_BYTE_CLUT2 4
#define PIX_PER_BYTE_CLUT4 2
#define STRIDE_ALIGN 4
#define MAX_BIT_COUNT 32
#define MAX_OFF_BITS 118
#define MAX_WIDTH 16384
#define MAX_HEIGHT 8192
#ifndef align_up
#define align_up(x, a) ((((x) + ((a) - 1)) / (a)) * (a))
#endif

osd_component_info g_osd_comp_info[OSD_COLOR_FORMAT_BUTT] = {
    { 0, 4, 4, 4 }, /* RGB444 */
    { 4, 4, 4, 4 }, /* ARGB4444 */
    { 0, 5, 5, 5 }, /* RGB555 */
    { 0, 5, 6, 5 }, /* RGB565 */
    { 1, 5, 5, 5 }, /* ARGB1555 */
    { 0, 0, 0, 0 }, /* RESERVED */
    { 0, 8, 8, 8 }, /* RGB888 */
    { 8, 8, 8, 8 }  /* ARGB8888 */
};

hi_u16 osd_make_color_u16(hi_u8 r, hi_u8 g, hi_u8 b, osd_component_info comp_info)
{
    hi_u8 r1, g1, b1;
    hi_u16 pixel = 0;
    hi_u32 tmp = 15; /* 15bit color data */

    r1 = r >> (BITS_NUM_PER_BYTE - comp_info.r_len);
    g1 = g >> (BITS_NUM_PER_BYTE - comp_info.g_len);
    b1 = b >> (BITS_NUM_PER_BYTE - comp_info.b_len);
    while (comp_info.a_len) {
        pixel |= (1 << tmp);
        tmp--;
        comp_info.a_len--;
    }

    pixel |= (r1 | (g1 << comp_info.b_len) | (b1 << (comp_info.b_len + comp_info.g_len)));
    return pixel;
}

hi_s32 get_bmp_info(const hi_char *filename, osd_bit_map_file_header *bmp_file_header, osd_bit_map_info *bmp_info)
{
    FILE *file = HI_NULL;
    hi_u16 bf_type;
    hi_char *path = HI_NULL;

    if (filename == HI_NULL) {
        printf("osd_get_bmp: filename=HI_NULL\n");
        return HI_FAILURE;
    }
    if (strlen(filename) > PATH_MAX - 1) {
        printf("file name Extra long\n");
        return HI_FAILURE;
    }

    path = realpath(filename, HI_NULL);
    if (path == HI_NULL) {
        printf("file path is null\n");
        return HI_FAILURE;
    }

    file = fopen(path, "rb");
    if (file == HI_NULL) {
        printf("Open file failed:%s!\n", filename);
        goto read_bmp_failed;
    }

    (hi_void)fread(&bf_type, 1, sizeof(bf_type), file);
    if (bf_type != 0x4d42) { /* BM */
        printf("not bitmap file\n");
        (hi_void)fclose(file);
        goto read_bmp_failed;
    }

    (hi_void)fread(bmp_file_header, 1, sizeof(osd_bit_map_file_header), file);
    (hi_void)fread(bmp_info, 1, sizeof(osd_bit_map_info), file);

    (hi_void)fclose(file);
    free(path);
    return HI_SUCCESS;

read_bmp_failed:
    free(path);
    return HI_FAILURE;
}

static hi_bool is_support_bmp_file(const osd_bit_map_info *bmp_info, hi_u16 bpp_threshold)
{
    hi_u16 bpp;

    bpp = bmp_info->bmp_header.bi_bit_count / BITS_NUM_PER_BYTE;
    if (bpp_threshold != 0 && bpp < bpp_threshold) {
        printf("bitmap format not supported!\n");
        return HI_FALSE;
    }

    if (bmp_info->bmp_header.bi_compression != 0) {
        printf("not support compressed bitmap file!\n");
        return HI_FALSE;
    }

    if (bmp_info->bmp_header.bi_height < 0) {
        printf("bmp_info.bmp_header.bi_height < 0\n");
        return HI_FALSE;
    }

    return HI_TRUE;
}

static hi_s32 read_bmp_data(const osd_bit_map_file_header *bmp_file_header, const osd_bit_map_info *bmp_info,
    FILE *file, osd_logo *video_logo)
{
    hi_u8 *rgb_buf = video_logo->rgb_buf;
    hi_u8 *orig_bmp_buf = NULL;
    hi_u32 bmp_data_stride;
    hi_u32 bmp_data_size;
    hi_u16 bpp, dst_bpp;
    hi_u16 i, j;
    hi_s32 ret;

    bpp = bmp_info->bmp_header.bi_bit_count / BITS_NUM_PER_BYTE;
    dst_bpp = (bpp > 2) ? 4 : 2; /* RGB1555: 2byte, RGB8888: 4byte */

    video_logo->width = bmp_info->bmp_header.bi_width;
    video_logo->height = bmp_info->bmp_header.bi_height;
    if (video_logo->stride == 0) {
        video_logo->stride = video_logo->width * dst_bpp;
    }

    bmp_data_stride = video_logo->width * bpp;
    bmp_data_stride = align_up(bmp_data_stride, STRIDE_ALIGN);
    bmp_data_size = video_logo->height * bmp_data_stride;

    /* RGB8888 or RGB1555 */
    orig_bmp_buf = (hi_u8 *)malloc(bmp_data_size);
    if (orig_bmp_buf == NULL) {
        printf("not enough memory to malloc!\n");
        return HI_FAILURE;
    }

    (hi_void)fseek(file, bmp_file_header->bf_off_bits, 0);
    if (fread(orig_bmp_buf, 1, bmp_data_size, file) != bmp_data_size) {
        printf("fread error!line:%d\n", __LINE__);
        perror("fread:");
    }

    for (i = 0; i < video_logo->height; i++) {
        for (j = 0; j < video_logo->width; ++j) {
            ret = memcpy_s(rgb_buf + i * video_logo->stride + j * dst_bpp, bpp,
                orig_bmp_buf + ((video_logo->height - 1) - i) * bmp_data_stride + j * bpp, bpp);
            if (ret != EOK) {
                free(orig_bmp_buf);
                printf("copy bmp failed!line:%d\n", __LINE__);
                return HI_FAILURE;
            }
            if (dst_bpp == 4) {                                               /* 4: RGB8888 */
                *(rgb_buf + i * video_logo->stride + j * dst_bpp + 3) = 0x80; /* 3: alpha offset */
            }
        }
    }

    free(orig_bmp_buf);

    return HI_SUCCESS;
}

hi_s32 load_bmp(const hi_char *filename, osd_logo *video_logo)
{
    osd_bit_map_file_header bmp_file_header;
    osd_bit_map_info bmp_info;
    FILE *file = HI_NULL;
    hi_char *path = HI_NULL;

    if (filename == HI_NULL || video_logo == HI_NULL) {
        printf("load_bmp: null ptr args!\n");
        return HI_FAILURE;
    }
    if (strlen(filename) > PATH_MAX - 1) {
        printf("file name Extra long\n");
        return HI_FAILURE;
    }
    path = realpath(filename, HI_NULL);
    if (path == HI_NULL) {
        return HI_FAILURE;
    }
    if (get_bmp_info(path, &bmp_file_header, &bmp_info) < 0) {
        goto read_bmp_failed;
    }
    if (bmp_info.bmp_header.bi_bit_count > MAX_BIT_COUNT || bmp_info.bmp_header.bi_width > MAX_WIDTH ||
        bmp_info.bmp_header.bi_height > MAX_HEIGHT || bmp_file_header.bf_off_bits > MAX_OFF_BITS) {
        printf("bmp info error!");
        goto read_bmp_failed;
    }
    if (is_support_bmp_file(&bmp_info, 2) != HI_TRUE) { /* each pixel should takes 2 (or more) bytes */
        goto read_bmp_failed;
    }

    file = fopen(path, "rb");
    if (file == HI_NULL) {
        printf("Open file failed:%s!\n", filename);
        goto read_bmp_failed;
    }

    if (read_bmp_data(&bmp_file_header, &bmp_info, file, video_logo) != HI_SUCCESS) {
        (hi_void)fclose(file);
        goto read_bmp_failed;
    }
    (hi_void)fclose(file);
    free(path);
    return HI_SUCCESS;

read_bmp_failed:
    free(path);
    return HI_FAILURE;
}

static hi_void updata_osd_logo_size_info(const osd_bit_map_info *bmp_info, osd_color_format fmt, osd_logo *video_logo)
{
    video_logo->width = bmp_info->bmp_header.bi_width;
    video_logo->height = bmp_info->bmp_header.bi_height;

    switch (fmt) {
        case OSD_COLOR_FORMAT_RGB444:
        case OSD_COLOR_FORMAT_RGB555:
        case OSD_COLOR_FORMAT_RGB565:
        case OSD_COLOR_FORMAT_RGB1555:
        case OSD_COLOR_FORMAT_RGB4444:
            video_logo->stride = video_logo->width * BYTE_PER_PIX_1555;
            return;
        case OSD_COLOR_FORMAT_RGB888:
        case OSD_COLOR_FORMAT_RGB8888:
            video_logo->stride = video_logo->width * BYTE_PER_PIX_8888;
            return;
        case OSD_COLOR_FORMAT_CLUT2:
            video_logo->stride = video_logo->width / PIX_PER_BYTE_CLUT2;
            return;
        case OSD_COLOR_FORMAT_CLUT4:
            video_logo->stride = video_logo->width / PIX_PER_BYTE_CLUT4;
            return;
        default:
            printf("file(%s), line(%d), no such format!\n", __FILE__, __LINE__);
            return;
    }
}

static hi_s32 copy_original_bmp_data(hi_u16 bpp, const hi_u8 *data, const bmp_data_size_info *data_size_info,
    osd_logo *video_logo)
{
    hi_u32 i, j;
    hi_s32 ret;
    hi_u8 *rgb_buf = video_logo->rgb_buf;
    hi_u16 dest_bpp = bpp;

    for (i = 0; i < data_size_info->height; ++i) {
        for (j = 0; j < data_size_info->width; ++j) {
            ret = memcpy_s(rgb_buf + i * video_logo->stride + j * bpp, dest_bpp,
                data + ((data_size_info->height - 1) - i) * data_size_info->stride + j * bpp, bpp);
            if (ret != EOK) {
                printf("copy bmp failed!line:%d\n", __LINE__);
                return HI_FAILURE;
            }
        }
    }

    return HI_SUCCESS;
}

static hi_s32 copy_clut_bmp_data(hi_u16 bpp, osd_color_format fmt, const hi_u8 *data,
    const bmp_data_size_info *data_size_info, osd_logo *video_logo)
{
    hi_u32 i, j;
    hi_s32 ret;
    hi_u8 *rgb_buf = video_logo->rgb_buf;
    hi_u32 width = data_size_info->width;
    hi_u16 dest_bpp = bpp;

    if (fmt == OSD_COLOR_FORMAT_CLUT4) {
        width /= PIX_PER_BYTE_CLUT4;
    } else if (fmt == OSD_COLOR_FORMAT_CLUT2) {
        width /= PIX_PER_BYTE_CLUT2;
    } else {
    }

    for (i = 0; i < data_size_info->height; ++i) {
        for (j = 0; j < width; ++j) {
            ret = memcpy_s(rgb_buf + i * video_logo->stride + j, dest_bpp,
                data + ((data_size_info->height - 1) - i) * data_size_info->stride + j, bpp);
            if (ret != EOK) {
                printf("copy bmp failed!line:%d\n", __LINE__);
                return HI_FAILURE;
            }
        }
    }

    return HI_SUCCESS;
}

static hi_s32 copy_2byte_bmp(hi_u16 bpp, osd_color_format fmt, const hi_u8 *data,
    const bmp_data_size_info *data_size_info, osd_logo *video_logo)
{
    hi_u32 i, j;
    hi_u8 *rgb_buf = video_logo->rgb_buf;
    hi_u8 r, g, b;
    hi_u8 *start = HI_NULL;
    hi_u16 *dst = HI_NULL;

    /* start color convert */
    for (i = 0; i < data_size_info->height; ++i) {
        for (j = 0; j < data_size_info->width; ++j) {
            start = (hi_u8 *)(data + ((data_size_info->height - 1) - i) * data_size_info->stride + j * bpp);
            dst = (hi_u16 *)(rgb_buf + i * video_logo->stride + j * 2); /* 2 bytes */
            r = *(start);
            g = *(start + 1);
            b = *(start + 2); /* 2 bytes offset */
            *dst = osd_make_color_u16(r, g, b, g_osd_comp_info[fmt]);
        }
    }
    return HI_SUCCESS;
}

static hi_s32 copy_4byte_bmp(hi_u16 bpp, const hi_u8 *data, const bmp_data_size_info *data_size_info,
    osd_logo *video_logo)
{
    hi_u32 i, j;
    hi_s32 ret;
    hi_u8 *rgb_buf = video_logo->rgb_buf;
    hi_u16 dest_bpp = bpp;

    for (i = 0; i < data_size_info->height; ++i) {
        for (j = 0; j < data_size_info->width; ++j) {
            ret = memcpy_s(rgb_buf + i * video_logo->stride + j * 4, dest_bpp, /* offset 4 bytes */
                data + ((data_size_info->height - 1) - i) * data_size_info->stride + j * bpp, bpp);
            if (ret != EOK) {
                printf("copy bmp failed!line:%d\n", __LINE__);
                return HI_FAILURE;
            }
            *(rgb_buf + i * video_logo->stride + j * 4 + 3) = 0xff; /* 4 bytes data, alpha offset is 3 */
        }
    }
    return HI_SUCCESS;
}

static hi_s32 copy_original_bmp_data_and_fill_alpha(hi_u16 bpp, osd_color_format fmt, const hi_u8 *data,
    const bmp_data_size_info *data_size_info, osd_logo *video_logo)
{
    switch (fmt) {
        case OSD_COLOR_FORMAT_RGB444:
        case OSD_COLOR_FORMAT_RGB555:
        case OSD_COLOR_FORMAT_RGB565:
        case OSD_COLOR_FORMAT_RGB1555:
        case OSD_COLOR_FORMAT_RGB4444:
            return copy_2byte_bmp(bpp, fmt, data, data_size_info, video_logo);
        case OSD_COLOR_FORMAT_RGB888:
        case OSD_COLOR_FORMAT_RGB8888:
            return copy_4byte_bmp(bpp, data, data_size_info, video_logo);
        default:
            printf("file(%s), line(%d), no such format!\n", __FILE__, __LINE__);
            return HI_FAILURE;
    }
}

static hi_s32 read_bmp_data_ex(const osd_bit_map_file_header *bmp_file_header, const osd_bit_map_info *bmp_info,
    FILE *file, osd_logo *video_logo, osd_color_format fmt)
{
    hi_u8 *orig_bmp_buf = NULL;
    hi_u16 bpp = bmp_info->bmp_header.bi_bit_count / BITS_NUM_PER_BYTE;
    hi_u32 bmp_data_stride;
    hi_u32 bmp_data_size;
    hi_s32 ret = HI_SUCCESS;
    bmp_data_size_info data_size_info;

    updata_osd_logo_size_info(bmp_info, fmt, video_logo);

    bmp_data_stride = (bpp == 0) ? video_logo->stride : (video_logo->width * bpp);
    bmp_data_stride = align_up(bmp_data_stride, STRIDE_ALIGN);
    bmp_data_size = video_logo->height * bmp_data_stride;

    data_size_info.width = video_logo->width;
    data_size_info.height = video_logo->height;
    data_size_info.stride = bmp_data_stride;

    orig_bmp_buf = (hi_u8 *)malloc(bmp_data_size);
    if (orig_bmp_buf == NULL) {
        printf("not enough memory to malloc!\n");
        return HI_FAILURE;
    }

    (hi_void)fseek(file, bmp_file_header->bf_off_bits, 0);
    if (fread(orig_bmp_buf, 1, bmp_data_size, file) != bmp_data_size) {
        printf("fread error!line:%d\n", __LINE__);
        perror("fread:");
    }

    /* copy bmp data to rgb_buf according bpp (and fmt) */
    if (bpp == 2 || bpp == 4) { /* each pixel takes 2 (or 4) bytes */
        ret = copy_original_bmp_data(bpp, orig_bmp_buf, &data_size_info, video_logo);
        goto copy_over_exit;
    }

    if (bpp <= 1) { /* such as clut2 or clut 4, or 2BPP... */
        ret = copy_clut_bmp_data(bpp, fmt, orig_bmp_buf, &data_size_info, video_logo);
        goto copy_over_exit;
    }

    /* bpp should equal to 3 here */
    ret = copy_original_bmp_data_and_fill_alpha(bpp, fmt, orig_bmp_buf, &data_size_info, video_logo);

copy_over_exit:
    free(orig_bmp_buf);
    return ret;
}

hi_s32 load_bmp_ex(const hi_char *filename, osd_logo *video_logo, osd_color_format fmt)
{
    osd_bit_map_file_header bmp_file_header;
    osd_bit_map_info bmp_info;
    FILE *file = HI_NULL;
    hi_char *path = HI_NULL;

    if (filename == HI_NULL || video_logo == HI_NULL) {
        printf("load_bmp_ex: null ptr args!\n");
        return HI_FAILURE;
    }
    if (strlen(filename) > PATH_MAX - 1) {
        printf("file name Extra long\n");
        return HI_FAILURE;
    }
    path = realpath(filename, HI_NULL);
    if (path == HI_NULL) {
        return HI_FAILURE;
    }
    if (get_bmp_info(path, &bmp_file_header, &bmp_info) < 0) {
        goto read_bmp_failed;
    }
    if (bmp_info.bmp_header.bi_bit_count > MAX_BIT_COUNT || bmp_info.bmp_header.bi_width > MAX_WIDTH ||
        bmp_info.bmp_header.bi_height > MAX_HEIGHT || bmp_file_header.bf_off_bits > MAX_OFF_BITS) {
        printf("bmp info error!");
        goto read_bmp_failed;
    }
    if (is_support_bmp_file(&bmp_info, 0) != HI_TRUE) {
        goto read_bmp_failed;
    }

    file = fopen(path, "rb");
    if (file == HI_NULL) {
        printf("Open file failed:%s!\n", filename);
        goto read_bmp_failed;
    }

    if (read_bmp_data_ex(&bmp_file_header, &bmp_info, file, video_logo, fmt) != HI_SUCCESS) {
        (hi_void)fclose(file);
        goto read_bmp_failed;
    }
    (hi_void)fclose(file);
    free(path);
    return HI_SUCCESS;

read_bmp_failed:
    free(path);
    return HI_FAILURE;
}

static hi_s32 read_bmp_canvas(const osd_bit_map_file_header *bmp_file_header, const osd_bit_map_info *bmp_info,
    FILE *file, osd_logo *video_logo, osd_color_format fmt)
{
    hi_u8 *orig_bmp_buf = NULL;
    hi_u16 bpp = bmp_info->bmp_header.bi_bit_count / BITS_NUM_PER_BYTE;
    hi_u32 bmp_data_size;
    hi_s32 ret = HI_SUCCESS;
    bmp_data_size_info data_size_info;

    data_size_info.width = bmp_info->bmp_header.bi_width;
    data_size_info.height = bmp_info->bmp_header.bi_height;

    if (bpp == 0) {
        if (fmt == OSD_COLOR_FORMAT_CLUT2) {
            data_size_info.stride = data_size_info.width / PIX_PER_BYTE_CLUT2;
        } else if (fmt == OSD_COLOR_FORMAT_CLUT4) {
            data_size_info.stride = data_size_info.width / PIX_PER_BYTE_CLUT4;
        } else {
            data_size_info.stride = video_logo->stride;
        }
    } else {
        data_size_info.stride = data_size_info.width * bpp;
        data_size_info.stride = align_up(data_size_info.stride, STRIDE_ALIGN);
    }

    bmp_data_size = data_size_info.height * data_size_info.stride;
    orig_bmp_buf = (hi_u8 *)malloc(bmp_data_size);
    if (orig_bmp_buf == NULL) {
        printf("not enough memory to malloc!\n");
        return HI_FAILURE;
    }

    (hi_void)fseek(file, bmp_file_header->bf_off_bits, 0);
    if (fread(orig_bmp_buf, 1, bmp_data_size, file) != bmp_data_size) {
        printf("fread error!line:%d\n", __LINE__);
        perror("fread:");
    }

    /* copy bmp data to rgb_buf according bpp (and fmt) */
    if (bpp == 2 || bpp == 4) { /* each pixel takes 2 (or 4) bytes */
        ret = copy_original_bmp_data(bpp, orig_bmp_buf, &data_size_info, video_logo);
        goto copy_over_exit;
    }

    if (bpp <= 1) { /* such as clut2 or clut 4, or 2BPP... */
        ret = copy_clut_bmp_data(bpp, fmt, orig_bmp_buf, &data_size_info, video_logo);
        goto copy_over_exit;
    }

    /* bpp should equal to 3 here */
    ret = copy_original_bmp_data_and_fill_alpha(bpp, fmt, orig_bmp_buf, &data_size_info, video_logo);

copy_over_exit:
    free(orig_bmp_buf);
    return ret;
}

hi_s32 load_bmp_canvas(const hi_char *filename, osd_logo *video_logo, osd_color_format fmt)
{
    osd_bit_map_file_header bmp_file_header;
    osd_bit_map_info bmp_info;
    FILE *file = HI_NULL;
    hi_char *path = HI_NULL;

    if (filename == HI_NULL || video_logo == HI_NULL) {
        printf("load_bmp_canvas: null ptr args!\n");
        return HI_FAILURE;
    }
    if (strlen(filename) > PATH_MAX - 1) {
        printf("file name Extra long\n");
        return HI_FAILURE;
    }
    path = realpath(filename, HI_NULL);
    if (path == HI_NULL) {
        return HI_FAILURE;
    }
    if (get_bmp_info(path, &bmp_file_header, &bmp_info) < 0) {
        goto read_bmp_failed;
    }
    if (bmp_info.bmp_header.bi_bit_count > MAX_BIT_COUNT || bmp_info.bmp_header.bi_width > MAX_WIDTH ||
        bmp_info.bmp_header.bi_height > MAX_HEIGHT || bmp_file_header.bf_off_bits > MAX_OFF_BITS) {
        printf("bmp info error!");
        goto read_bmp_failed;
    }
    if (is_support_bmp_file(&bmp_info, 0) != HI_TRUE) {
        goto read_bmp_failed;
    }

    file = fopen(path, "rb");
    if (file == HI_NULL) {
        printf("Open file failed:%s!\n", filename);
        goto read_bmp_failed;
    }

    if (read_bmp_canvas(&bmp_file_header, &bmp_info, file, video_logo, fmt) != HI_SUCCESS) {
        (hi_void)fclose(file);
        goto read_bmp_failed;
    }
    (hi_void)fclose(file);
    free(path);
    return HI_SUCCESS;

read_bmp_failed:
    free(path);
    return HI_FAILURE;
}

hi_char *get_ext_name(const hi_char *filename)
{
    hi_char *pret = HI_NULL;
    size_t len;

    if (filename == HI_NULL) {
        printf("filename can't be null!");
        return HI_NULL;
    }

    len = strlen(filename);
    while (len) {
        pret = (hi_char *)(filename + len);
        if (*pret == '.') {
            return (pret + 1);
        }

        len--;
    }

    return pret;
}

hi_s32 load_image(const hi_char *filename, osd_logo *video_logo)
{
    hi_char *ext = get_ext_name(filename);
    if (ext == HI_NULL) {
        printf("get_ext_name error!\n");
        return -1;
    }

    if (strcmp(ext, "bmp") == 0) {
        if (load_bmp(filename, video_logo) != 0) {
            printf("load_bmp error!\n");
            return -1;
        }
    } else {
        printf("not supported image file!\n");
        return -1;
    }

    return 0;
}

hi_s32 load_image_ex(const hi_char *filename, osd_logo *video_logo, osd_color_format fmt)
{
    hi_char *ext = get_ext_name(filename);
    if (ext == HI_NULL) {
        printf("load_image_ex error!\n");
        return -1;
    }

    if (strcmp(ext, "bmp") == 0) {
        if (load_bmp_ex(filename, video_logo, fmt) != 0) {
            printf("load_bmp_ex error!\n");
            return -1;
        }
    } else {
        printf("not supported image file!\n");
        return -1;
    }

    return 0;
}

hi_s32 load_canvas_ex(const hi_char *filename, osd_logo *video_logo, osd_color_format fmt)
{
    hi_char *ext = get_ext_name(filename);
    if (ext == HI_NULL) {
        printf("load_canvas_ex error!\n");
        return -1;
    }

    if (strcmp(ext, "bmp") == 0) {
        if (load_bmp_canvas(filename, video_logo, fmt) != 0) {
            printf("OSD_LoadBMP error!\n");
            return -1;
        }
    } else {
        printf("not supported image file!\n");
        return -1;
    }

    return 0;
}

hi_s32 load_bit_map_to_surface(const hi_char *file_name, const osd_surface *surface, hi_u8 *virt)
{
    osd_logo logo;
    logo.stride = surface->stride;
    logo.rgb_buf = virt;

    return load_image(file_name, &logo);
}

hi_s32 create_surface_by_bit_map(const hi_char *file_name, osd_surface *surface, hi_u8 *virt)
{
    osd_logo logo;

    logo.rgb_buf = virt;
    if (load_image_ex(file_name, &logo, surface->color_format) < 0) {
        printf("load bmp error!\n");
        return -1;
    }

    surface->height = logo.height;
    surface->width = logo.width;
    surface->stride = logo.stride;

    return 0;
}

hi_s32 create_surface_by_canvas(const hi_char *file_name, osd_surface *surface, hi_u8 *virt,
    const canvas_size_info *canvas_size)
{
    osd_logo logo;

    logo.rgb_buf = virt;
    logo.width = canvas_size->width;
    logo.height = canvas_size->height;
    logo.stride = canvas_size->stride;
    if (load_canvas_ex(file_name, &logo, surface->color_format) < 0) {
        printf("load bmp error!\n");
        return -1;
    }

    surface->height = canvas_size->height;
    surface->width = canvas_size->width;
    surface->stride = canvas_size->stride;

    return 0;
}
