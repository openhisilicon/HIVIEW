/*
* Copyright (c) Hisilicon Technologies Co., Ltd. 2017-2019. All rights reserved.
* Description:Initial Draft
* Author: Hisilicon multimedia software group
* Create: 2017/07/15
*/


#ifndef __HI_COMMON_VIDEO_H__
#define __HI_COMMON_VIDEO_H__

#include "hi_type.h"
#include "hi_common.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

typedef enum  {
    HI_OP_MODE_AUTO   = 0,
    HI_OP_MODE_MANUAL = 1,
    HI_OP_MODE_BUTT
} hi_op_mode;

typedef enum  {
    HI_VB_SRC_COMMON  = 0,
    HI_VB_SRC_MOD  = 1,
    HI_VB_SRC_PRIVATE = 2,
    HI_VB_SRC_USER    = 3,
    HI_VB_SRC_BUTT
} hi_vb_src;

typedef enum  {
    HI_ASPECT_RATIO_NONE   = 0,        /* full screen */
    HI_ASPECT_RATIO_AUTO   = 1,        /* ratio no change, 1:1*/
    HI_ASPECT_RATIO_MANUAL = 2,        /* ratio manual set */
    HI_ASPECT_RATIO_BUTT
} hi_aspect_ratio_type;


typedef enum  {
    HI_VIDEO_FIELD_TOP         = 1,    /* even field */
    HI_VIDEO_FIELD_BOTTOM      = 2,    /* odd field */
    HI_VIDEO_FIELD_INTERLACED  = 3,    /* two interlaced fields */
    HI_VIDEO_FIELD_FRAME       = 4,    /* frame */

    HI_VIDEO_FIELD_BUTT
} hi_video_field;

typedef enum  {
    HI_VIDEO_FORMAT_LINEAR = 0,       /* nature video line */
    HI_VIDEO_FORMAT_TILE_64x16,       /* tile cell: 64pixel x 16line */
    HI_VIDEO_FORMAT_BUTT
} hi_video_format;

typedef enum  {
    HI_COMPRESS_MODE_NONE = 0,      /* no compress */
    HI_COMPRESS_MODE_SEG,           /* compress unit is 256x1 bytes as a segment.*/
    HI_COMPRESS_MODE_SEG_COMPACT,   /* compact compress unit is 256x1 bytes as a segment.*/
    HI_COMPRESS_MODE_TILE,          /* compress unit is a tile.*/
    HI_COMPRESS_MODE_LINE,          /* compress unit is the whole line.*/
    HI_COMPRESS_MODE_FRAME,         /* compress unit is the whole frame. YUV for VPSS(3DNR) */

    HI_COMPRESS_MODE_BUTT
} hi_compress_mode;

typedef enum  {
    HI_VIDEO_DISPLAY_MODE_PREVIEW  = 0,
    HI_VIDEO_DISPLAY_MODE_PLAYBACK = 1,

    HI_VIDEO_DISPLAY_MODE_BUTT
} hi_video_display_mode;

typedef enum  {
    HI_FRAME_FLAG_SNAP_FLASH  = 0x1 << 0,
    HI_FRAME_FLAG_SNAP_CUR    = 0x1 << 1,
    HI_FRAME_FLAG_SNAP_REF    = 0x1 << 2,
    HI_FRAME_FLAG_SNAP_END    = 0x1 << 3,
    HI_FRAME_FLAG_MIRROR      = 0x1 << 4,
    HI_FRAME_FLAG_FLIP        = 0x1 << 5,
    HI_FRAME_FLAG_BUTT
} hi_frame_flag;


typedef enum  {
    HI_COLOR_GAMUT_BT601 = 0,
    HI_COLOR_GAMUT_BT709,
    HI_COLOR_GAMUT_BT2020,
    HI_COLOR_GAMUT_USER,
    HI_COLOR_GAMUT_BUTT
} hi_color_gamut;

/* we ONLY define picture format used, all unused will be deleted! */
typedef enum {
    HI_PIXEL_FORMAT_RGB_444 = 0,
    HI_PIXEL_FORMAT_RGB_555,
    HI_PIXEL_FORMAT_RGB_565,
    HI_PIXEL_FORMAT_RGB_888,

    HI_PIXEL_FORMAT_BGR_444,
    HI_PIXEL_FORMAT_BGR_555,
    HI_PIXEL_FORMAT_BGR_565,
    HI_PIXEL_FORMAT_BGR_888,

    HI_PIXEL_FORMAT_ARGB_1555,
    HI_PIXEL_FORMAT_ARGB_4444,
    HI_PIXEL_FORMAT_ARGB_8565,
    HI_PIXEL_FORMAT_ARGB_8888,
    HI_PIXEL_FORMAT_ARGB_2BPP,
    HI_PIXEL_FORMAT_ARGB_CLUT2,
    HI_PIXEL_FORMAT_ARGB_CLUT4,

    HI_PIXEL_FORMAT_ABGR_1555,
    HI_PIXEL_FORMAT_ABGR_4444,
    HI_PIXEL_FORMAT_ABGR_8565,
    HI_PIXEL_FORMAT_ABGR_8888,

    HI_PIXEL_FORMAT_RGB_BAYER_8BPP,
    HI_PIXEL_FORMAT_RGB_BAYER_10BPP,
    HI_PIXEL_FORMAT_RGB_BAYER_12BPP,
    HI_PIXEL_FORMAT_RGB_BAYER_14BPP,
    HI_PIXEL_FORMAT_RGB_BAYER_16BPP,

    HI_PIXEL_FORMAT_YVU_PLANAR_422,
    HI_PIXEL_FORMAT_YVU_PLANAR_420,
    HI_PIXEL_FORMAT_YVU_PLANAR_444,

    HI_PIXEL_FORMAT_YVU_SEMIPLANAR_422,
    HI_PIXEL_FORMAT_YVU_SEMIPLANAR_420,
    HI_PIXEL_FORMAT_YVU_SEMIPLANAR_444,

    HI_PIXEL_FORMAT_YUV_SEMIPLANAR_422,
    HI_PIXEL_FORMAT_YUV_SEMIPLANAR_420,
    HI_PIXEL_FORMAT_YUV_SEMIPLANAR_444,

    HI_PIXEL_FORMAT_YUYV_PACKAGE_422,
    HI_PIXEL_FORMAT_YVYU_PACKAGE_422,
    HI_PIXEL_FORMAT_UYVY_PACKAGE_422,
    HI_PIXEL_FORMAT_VYUY_PACKAGE_422,
    HI_PIXEL_FORMAT_YYUV_PACKAGE_422,
    HI_PIXEL_FORMAT_YYVU_PACKAGE_422,
    HI_PIXEL_FORMAT_UVYY_PACKAGE_422,
    HI_PIXEL_FORMAT_VUYY_PACKAGE_422,
    HI_PIXEL_FORMAT_VY1UY0_PACKAGE_422,

    HI_PIXEL_FORMAT_YUV_400,
    HI_PIXEL_FORMAT_UV_420,

    /* SVP data format */
    HI_PIXEL_FORMAT_BGR_888_PLANAR,
    HI_PIXEL_FORMAT_HSV_888_PACKAGE,
    HI_PIXEL_FORMAT_HSV_888_PLANAR,
    HI_PIXEL_FORMAT_LAB_888_PACKAGE,
    HI_PIXEL_FORMAT_LAB_888_PLANAR,
    HI_PIXEL_FORMAT_S8C1,
    HI_PIXEL_FORMAT_S8C2_PACKAGE,
    HI_PIXEL_FORMAT_S8C2_PLANAR,
    HI_PIXEL_FORMAT_S8C3_PLANAR,
    HI_PIXEL_FORMAT_S16C1,
    HI_PIXEL_FORMAT_U8C1,
    HI_PIXEL_FORMAT_U16C1,
    HI_PIXEL_FORMAT_S32C1,
    HI_PIXEL_FORMAT_U32C1,
    HI_PIXEL_FORMAT_U64C1,
    HI_PIXEL_FORMAT_S64C1,

    HI_PIXEL_FORMAT_BUTT
} hi_pixel_format;


typedef enum  {
    HI_DYNAMIC_RANGE_SDR8 = 0,
    HI_DYNAMIC_RANGE_SDR10,
    HI_DYNAMIC_RANGE_HDR10,
    HI_DYNAMIC_RANGE_HLG,
    HI_DYNAMIC_RANGE_SLF,
    HI_DYNAMIC_RANGE_XDR,
    HI_DYNAMIC_RANGE_BUTT
} hi_dynamic_range;


typedef enum  {
    HI_DATA_BIT_WIDTH_8 = 0,
    HI_DATA_BIT_WIDTH_10,
    HI_DATA_BIT_WIDTH_12,
    HI_DATA_BIT_WIDTH_14,
    HI_DATA_BIT_WIDTH_16,
    HI_DATA_BIT_WIDTH_BUTT
} hi_data_bit_width;


typedef struct {
    hi_u32 top_width;
    hi_u32 bottom_width;
    hi_u32 left_width;
    hi_u32 right_width;
    hi_u32 color;
} hi_border;

typedef struct {
    hi_s32 x;
    hi_s32 y;
} hi_point;

typedef struct {
    hi_u32 width;
    hi_u32 height;
} hi_size;

typedef struct {
    hi_s32 x;
    hi_s32 y;
    hi_u32 width;
    hi_u32 height;
} hi_rect;

typedef enum {
    HI_COORD_ABS = 0,                          /* Absolute coordinate. */
    HI_COORD_RATIO,                            /* Ratio coordinate. */
    HI_COORD_BUTT
} hi_coord;

typedef enum {
    HI_COVER_RECT = 0,                         /* Rectangle cover. */
    HI_COVER_QUAD,                             /* Quadrangle cover. */
    HI_COVER_BUTT
} hi_cover_type;

typedef struct {
    hi_bool        is_solid;                   /* RW; Solid or hollow cover. */
    hi_u32         thick;                      /* RW; Thick of the hollow quadrangle. */
    hi_point       point[HI_QUAD_POINT_NUM];   /* RW; Four points of the quadrangle. */
} hi_quad_cover;

typedef struct {
    hi_cover_type      type;   /* RW; Cover type. */
    union {
        hi_rect        rect;   /* RW; The rectangle area. AUTO:hi_cover_type:HI_COVER_RECT; */
        hi_quad_cover  quad;   /* RW; The quadrangle area.  AUTO:hi_cover_type:HI_COVER_QUAD; */
    };
    hi_u32             color;  /* RW; Range: [0,0xFFFFFF]; Color of cover. */
} hi_cover;

typedef enum  {
    HI_MOSAIC_BLK_SIZE_4 = 0, /* block size 4 * 4 of MOSAIC */
    HI_MOSAIC_BLK_SIZE_8,     /* block size 8 * 8 of MOSAIC */
    HI_MOSAIC_BLK_SIZE_16,    /* block size 16 * 16 of MOSAIC */
    HI_MOSAIC_BLK_SIZE_32,    /* block size 32 * 32 of MOSAIC */
    HI_MOSAIC_BLK_SIZE_64,    /* block size 64 * 64 of MOSAIC */
    HI_MOSAIC_BLK_SIZE_128,   /* block size 128 * 128 of MOSAIC */
    HI_MOSAIC_BLK_SIZE_BUTT
} hi_mosaic_blk_size;

typedef struct {
    hi_mosaic_blk_size blk_size;
    hi_rect            rect;   /* RW; The rectangle area. */
} hi_mosaic;

typedef struct {
    hi_bool enable;
    hi_rect  rect;
} hi_crop_info;

typedef struct {
    hi_s32  src_frame_rate;        /* RW; source frame rate */
    hi_s32  dst_frame_rate;        /* RW; dest frame rate */
} hi_frame_rate_ctrl;

typedef struct {
    hi_aspect_ratio_type mode;          /* aspect ratio mode: none/auto/manual */
    hi_u32         bg_color;      /* background color, RGB 888 */
    hi_rect         video_rect;     /* valid in ASPECT_RATIO_MANUAL mode */

} hi_aspect_ratio;

typedef struct {
    hi_s16              offset_top;           /* top offset of show area */
    hi_s16              offset_left;          /* left offset of show area */
    hi_u32              max_luma;             /* for HDR */
    hi_u32              min_luma;             /* for HDR */
    hi_u64              private_data[HI_MAX_PRIVATE_DATA_NUM];
} hi_video_supplement_misc;

typedef struct {
    hi_phys_addr_t   misc_info_phys_addr;        /* default allocated buffer */
    hi_phys_addr_t   jpeg_dcf_phys_addr;
    hi_phys_addr_t   isp_info_phys_addr;
    hi_phys_addr_t   low_delay_phys_addr;
    hi_phys_addr_t   frame_dng_phys_addr;

    hi_void* ATTRIBUTE misc_info_virt_addr;       /* misc info */
    hi_void* ATTRIBUTE jpeg_dcf_virt_addr;        /* jpeg_dcf, used in JPEG DCF */
    hi_void* ATTRIBUTE isp_info_virt_addr;        /* isp_frame_info, used in ISP debug, when get raw and send raw */
    hi_void* ATTRIBUTE low_delay_virt_addr;       /* used in low delay */
    hi_void* ATTRIBUTE frame_dng_virt_addr;
} hi_video_supplement;

typedef struct {
    hi_u32              width;
    hi_u32              height;
    hi_video_field      field;
    hi_pixel_format     pixel_format;
    hi_video_format     video_format;
    hi_compress_mode    compress_mode;
    hi_dynamic_range    dynamic_range;
    hi_color_gamut      color_gamut;

    hi_u32              header_stride[HI_MAX_COLOR_COMPONENT];
    hi_u32              stride[HI_MAX_COLOR_COMPONENT];

    hi_phys_addr_t      header_phys_addr[HI_MAX_COLOR_COMPONENT];
    hi_phys_addr_t      phys_addr[HI_MAX_COLOR_COMPONENT];
    hi_void* ATTRIBUTE  header_virt_addr[HI_MAX_COLOR_COMPONENT];
    hi_void* ATTRIBUTE  virt_addr[HI_MAX_COLOR_COMPONENT];

    hi_u32              time_ref;
    hi_u64              pts;

    hi_u64              user_data[HI_MAX_USER_DATA_NUM];
    hi_u32              frame_flag;     /* frame_flag, can be OR operation. */
    hi_video_supplement supplement;
} hi_video_frame;

typedef struct {
    hi_video_frame video_frame;
    hi_u32        pool_id;
    hi_mod_id      mod_id;
} hi_video_frame_info;

typedef struct {
    hi_u32 vb_size;

    hi_u32 head_stride;
    hi_u32 head_size;
    hi_u32 head_y_size;

    hi_u32 main_stride;
    hi_u32 main_size;
    hi_u32 main_y_size;
} hi_vb_calc_cfg;

typedef struct {
    hi_pixel_format pixel_format;  /* bitmap's pixel format */
    hi_u32 width;               /* bitmap's width */
    hi_u32 height;              /* bitmap's height */
    hi_void* ATTRIBUTE data;      /* address of bitmap's data */
} hi_bmp;

/* Angle of rotation */
typedef enum  {
    HI_ROTATION_0   = 0,
    HI_ROTATION_90  = 1,
    HI_ROTATION_180 = 2,
    HI_ROTATION_270 = 3,
    HI_ROTATION_BUTT
} hi_rotation;

typedef enum {
    HI_ROTATION_VIEW_TYPE_ALL      = 0, /* View all source Image,no lose*/
    HI_ROTATION_VIEW_TYPE_TYPICAL  = 1, /* View from rotation Image with source size,same lose*/
    HI_ROTATION_VIEW_TYPE_INSIDE   = 2, /* View with no black section,all  in dest Image*/
    HI_ROTATION_VIEW_TYPE_BUTT,
} hi_rotation_view_type;

typedef struct {
    hi_rotation_view_type view_type;       /*RW;range: [0, 2];rotation mode*/
    hi_u32    angle;         /*RW;range: [0,360];rotation angle:[0,360]*/
    hi_s32    center_x_offset; /*RW;range: [-511, 511];horizontal offset of the image distortion center relative to image center.*/
    hi_s32    center_y_offset; /*RW;range: [-511, 511];vertical offset of the image distortion center relative to image center.*/
    hi_size   dst_size;       /*RW;dest size of any angle rotation*/
} hi_free_rotation_attr;

typedef enum {
    HI_ROTATION_ANG_FIXED = 0,             /* Fixed angle. */
    HI_ROTATION_ANG_FREE,                  /* Free angle. */
    HI_ROTATION_ANG_BUTT
} hi_rotation_type;

typedef struct {
    hi_bool             enable;              /* RW; Rotate enable. */
    hi_rotation_type    rotation_type;       /* RW; Rotate type. */
    union {
        /*
         * RW; Fixed rotation attribute.
         * AUTO:hi_rotation_type:HI_ROTATION_ANG_FIXED;
         */
        hi_rotation     rotation_fixed;
        /*
         * RW; Free rotation attribute.
         * AUTO:hi_rotation_type:HI_ROTATION_ANG_FREE;
         */
        hi_free_rotation_attr          rotation_free;
    };
} hi_rotation_attr;

typedef enum  {
    HI_FRAME_INTERRUPT_START,
    HI_FRAME_INTERRUPT_EARLY,
    HI_FRAME_INTERRUPT_EARLY_END,
    HI_FRAME_INTERRUPT_EARLY_EARLY,
    HI_FRAME_INTERRUPT_BUTT,
} hi_frame_interrupt_type;

typedef struct {
    hi_frame_interrupt_type interrupt_type;
    hi_u32 early_line;
} hi_frame_interrupt_attr;

typedef enum {
    HI_DATA_RATE_X1 = 0, /* RW; output 1 pixel per clock */
    HI_DATA_RATE_X2,     /* RW; output 2 pixel per clock */

    HI_DATA_RATE_BUTT
} hi_data_rate;

typedef enum {
    HI_WDR_MODE_NONE = 0,
    HI_WDR_MODE_BUILT_IN,
    HI_WDR_MODE_QUDRA,

    HI_WDR_MODE_2To1_LINE,
    HI_WDR_MODE_2To1_FRAME,

    HI_WDR_MODE_3To1_LINE,
    HI_WDR_MODE_3To1_FRAME,

    HI_WDR_MODE_4To1_LINE,
    HI_WDR_MODE_4To1_FRAME,

    HI_WDR_MODE_BUTT,
} hi_wdr_mode;

typedef enum {
    HI_EXT_CHN_SRC_TAIL,
    HI_EXT_CHN_SRC_BEFORE_FISHEYE,
    HI_EXT_CHN_SRC_HEAD,

    HI_EXT_CHN_SRC_BUTT
} hi_ext_chn_src;

typedef enum {
    HI_CORNER_RECT_TYPE_CORNER = 0,
    HI_CORNER_RECT_TYPE_FULL_LINE,
    HI_CORNER_RECT_TYPE_BUTT
}hi_corner_rect_type;

typedef struct {
    hi_rect      rect;
    hi_u32       hor_len;
    hi_u32       ver_len;
    hi_u32       thick;
} hi_corner_rect;

typedef struct {
    hi_corner_rect_type    corner_rect_type;
    hi_u32                 color;
} hi_corner_rect_attr;

typedef enum {
    HI_SCHEDULE_NORMAL = 0,
    HI_SCHEDULE_QUICK,
    HI_SCHEDULE_BUTT
} hi_schedule_mode;

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __HI_COMMON_VIDEO_H__ */

