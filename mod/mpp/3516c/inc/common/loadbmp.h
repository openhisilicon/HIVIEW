/*
  Copyright (c), 2001-2024, Shenshu Tech. Co., Ltd.
 */

#ifndef LOAD_BMP_H
#define LOAD_BMP_H

#include "hi_type.h"

/* the color format OSD supported */
typedef enum {
    OSD_COLOR_FORMAT_RGB444 = 0,
    OSD_COLOR_FORMAT_RGB4444 = 1,
    OSD_COLOR_FORMAT_RGB555 = 2,
    OSD_COLOR_FORMAT_RGB565 = 3,
    OSD_COLOR_FORMAT_RGB1555 = 4,
    OSD_COLOR_FORMAT_RGB888 = 6,
    OSD_COLOR_FORMAT_RGB8888 = 7,
    OSD_COLOR_FORMAT_CLUT2 = 8,
    OSD_COLOR_FORMAT_CLUT4 = 9,
    OSD_COLOR_FORMAT_BUTT
} osd_color_format;

typedef struct {
    hi_u8 r;
    hi_u8 g;
    hi_u8 b;
    hi_u8 reserved;
} osd_rgb;

typedef struct {
    osd_color_format color_format; /* color format */
    hi_u16 height;                 /* operation height */
    hi_u16 width;                  /* operation width */
    hi_u16 stride;                 /* surface stride */
    hi_u16 reserved;
    hi_u8 *virt_addr; /* virtual address */
} osd_surface;

typedef struct {
    hi_u32 width;   /* out */
    hi_u32 height;  /* out */
    hi_u32 stride;  /* in */
    hi_u8 *rgb_buf; /* in/out */
} osd_logo;

typedef struct {
    hi_u16 bi_size;
    hi_u32 bi_width;
    hi_s32 bi_height;
    hi_u16 bi_planes;
    hi_u16 bi_bit_count;
    hi_u32 bi_compression;
    hi_u32 bi_size_image;
    hi_u32 bi_x_pels_per_meter;
    hi_u32 bi_y_pels_per_meter;
    hi_u32 bi_clr_used;
    hi_u32 bi_clr_important;
} osd_bit_map_info_header;

typedef struct {
    hi_u32 bf_size;
    hi_u16 bf_reserved1;
    hi_u16 bf_reserved2;
    hi_u32 bf_off_bits;
} osd_bit_map_file_header;

typedef struct {
    hi_u8 blue;
    hi_u8 green;
    hi_u8 red;
    hi_u8 reserved;
} osd_rgb_quad;

typedef struct {
    osd_bit_map_info_header bmp_header;
    osd_rgb_quad bmp_colors[1];
} osd_bit_map_info;

typedef struct {
    hi_u8 a_len;
    hi_u8 r_len;
    hi_u8 g_len;
    hi_u8 b_len;
} osd_component_info;

typedef struct {
    hi_u32 width;
    hi_u32 height;
    hi_u32 stride;
} canvas_size_info;

typedef canvas_size_info bmp_data_size_info;

#ifdef __cplusplus
extern "C" {
#endif

hi_s32 load_image(const hi_char *filename, osd_logo *video_logo);
hi_s32 load_bit_map_to_surface(const hi_char *file_name, const osd_surface *surface, hi_u8 *virt);
hi_s32 create_surface_by_bit_map(const hi_char *file_name, osd_surface *surface, hi_u8 *virt);
hi_s32 create_surface_by_canvas(const hi_char *file_name, osd_surface *surface, hi_u8 *virt,
    const canvas_size_info *canvas_size);
hi_s32 get_bmp_info(const hi_char *filename, osd_bit_map_file_header *bmp_file_header, osd_bit_map_info *bmp_info);

#ifdef __cplusplus
}
#endif

#endif /* End of #ifndef LOAD_BMP_H */
