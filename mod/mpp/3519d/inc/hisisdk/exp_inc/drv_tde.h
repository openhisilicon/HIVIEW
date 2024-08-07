/*
  Copyright (c), 2001-2024, Shenshu Tech. Co., Ltd.
 */

#ifndef DRV_TDE_H
#define DRV_TDE_H

#include "ot_type.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/* tde start err no. */
#define DRV_ERR_TDE_BASE ((td_s32)(((0x80UL + 0x20UL) << 24) | (100 << 16) | (4 << 13) | 1))

typedef enum {
    DRV_ERR_TDE_DEV_NOT_OPEN = DRV_ERR_TDE_BASE, /* tde device not open yet */
    DRV_ERR_TDE_DEV_OPEN_FAILED, /* open tde device failed */
    DRV_ERR_TDE_NULL_PTR, /* input parameters contain null ptr */
    DRV_ERR_TDE_NO_MEM, /* malloc failed  */
    DRV_ERR_TDE_INVALID_HANDLE, /* invalid job handle */
    DRV_ERR_TDE_INVALID_PARA, /* invalid parameter */
    DRV_ERR_TDE_NOT_ALIGNED, /* aligned error for position, stride, width */
    DRV_ERR_TDE_MINIFICATION, /* invalid minification */
    DRV_ERR_TDE_CLIP_AREA, /* clip area and operation area have no intersection */
    DRV_ERR_TDE_JOB_TIMEOUT, /* blocked job wait timeout */
    DRV_ERR_TDE_UNSUPPORTED_OPERATION, /* unsupported operation */
    DRV_ERR_TDE_QUERY_TIMEOUT, /* query time out */
    DRV_ERR_TDE_INTERRUPT, /* blocked job was interrupted */
    DRV_ERR_TDE_BUTT,
}drv_tde_err_code;

typedef td_void (*drv_tde_func_callback)(const td_void *para1, const td_void *para2);

typedef enum {
    DRV_TDE_COLOR_FMT_RGB444 = 0, /* RGB444 format */
    DRV_TDE_COLOR_FMT_BGR444, /* BGR444 format */
    DRV_TDE_COLOR_FMT_RGB555, /* RGB555 format */
    DRV_TDE_COLOR_FMT_BGR555, /* BGR555 format */
    DRV_TDE_COLOR_FMT_RGB565, /* RGB565 format */
    DRV_TDE_COLOR_FMT_BGR565, /* BGR565 format */
    DRV_TDE_COLOR_FMT_RGB888, /* RGB888 format */
    DRV_TDE_COLOR_FMT_BGR888, /* BGR888 format */
    DRV_TDE_COLOR_FMT_ARGB4444, /* ARGB4444 format */
    DRV_TDE_COLOR_FMT_ABGR4444, /* ABGR4444 format */
    DRV_TDE_COLOR_FMT_RGBA4444, /* RGBA4444 format */
    DRV_TDE_COLOR_FMT_BGRA4444, /* BGRA4444 format */
    DRV_TDE_COLOR_FMT_ARGB1555, /* ARGB1555 format */
    DRV_TDE_COLOR_FMT_ABGR1555, /* ABGR1555 format */
    DRV_TDE_COLOR_FMT_RGBA1555, /* RGBA1555 format */
    DRV_TDE_COLOR_FMT_BGRA1555, /* BGRA1555 format */
    DRV_TDE_COLOR_FMT_ARGB8565, /* ARGB8565 format */
    DRV_TDE_COLOR_FMT_ABGR8565, /* ABGR8565 format */
    DRV_TDE_COLOR_FMT_RGBA8565, /* RGBA8565 format */
    DRV_TDE_COLOR_FMT_BGRA8565, /* BGRA8565 format */
    DRV_TDE_COLOR_FMT_ARGB8888, /* ARGB8888 format */
    DRV_TDE_COLOR_FMT_ABGR8888, /* ABGR8888 format */
    DRV_TDE_COLOR_FMT_RGBA8888, /* RGBA8888 format */
    DRV_TDE_COLOR_FMT_BGRA8888, /* BGRA8888 format */
    DRV_TDE_COLOR_FMT_RABG8888, /* RABG8888 format */
    DRV_TDE_COLOR_FMT_CLUT1, /* 1-bit palette format without alpha component. Each pixel occupies one bit. */
    DRV_TDE_COLOR_FMT_CLUT2, /* 2-bit palette format without alpha component. Each pixel occupies two bits. */
    /* 4-bit palette format without alpha component. Each pixel occupies four bits. */
    DRV_TDE_COLOR_FMT_CLUT4,
    /* 8-bit palette format without alpha component. Each pixel occupies eight bits. */
    DRV_TDE_COLOR_FMT_CLUT8,
    DRV_TDE_COLOR_FMT_ACLUT44, /* 1-bit palette format with alpha component. Each pixel occupies one bit. */
    DRV_TDE_COLOR_FMT_ACLUT88, /* 1-bit palette format with alpha component. Each pixel occupies one bit. */
    DRV_TDE_COLOR_FMT_A1, /* Alpha format. Each pixel occupies one bit. */
    DRV_TDE_COLOR_FMT_A8, /* Alpha format. Each pixel occupies eight bits. */
    DRV_TDE_COLOR_FMT_YCBCR888, /* YUV packet format without alpha component */
    DRV_TDE_COLOR_FMT_AYCBCR8888, /* YUV packet format with alpha component */
    DRV_TDE_COLOR_FMT_YCBCR422,           /* 35：PKYUYV */
    DRV_TDE_COLOR_FMT_PKGYVYU,            /* 36：PKYVYU */
    DRV_TDE_COLOR_FMT_PKGUYVY,            /* 37：PKUYVY */
    DRV_TDE_COLOR_FMT_PKGVYUY,            /* 38：PKVYUY */
    DRV_TDE_COLOR_FMT_PKGVUYY,            /* 39：PKVUYY */
    DRV_TDE_COLOR_FMT_PKGYYUV,            /* 40：PKYYUV */
    DRV_TDE_COLOR_FMT_PKGUVYY,            /* 41：PKUVYY */
    DRV_TDE_COLOR_FMT_PKGYYVU,            /* 42：PKYYVU */
    DRV_TDE_COLOR_FMT_JPG_YCBCR400MBP,    /* 43：SP400 */
    DRV_TDE_COLOR_FMT_JPG_YCBCR422MBHP,   /* 44：SP422h(1*2)(Pixel width is even) */
    DRV_TDE_COLOR_FMT_JPG_YCBCR422MBVP,   /* 45：SP422V(2*1)(Pixel height is even) */
    DRV_TDE_COLOR_FMT_MP1_YCBCR420MBP,    /* 46：SP420(cd hi The pixel width and height are even numbers.) */
    DRV_TDE_COLOR_FMT_MP2_YCBCR420MBP,    /* 47：SP420(cd hi The pixel width and height are even numbers.) */
    DRV_TDE_COLOR_FMT_MP2_YCBCR420MBI,    /* 48：SP420(cd hi The pixel width and height are even numbers.) */
    DRV_TDE_COLOR_FMT_JPG_YCBCR420MBP,    /* 49：SP420(cd hi The pixel width and height are even numbers.) */
    DRV_TDE_COLOR_FMT_JPG_YCBCR444MBP,    /* 50：SP444(cd hi The pixel width and height are even numbers.) */
    DRV_TDE_COLOR_FMT_MAX                 /* 51：End of enumeration */
} drv_tde_color_fmt;

/* Definition of the semi-planar YUV format */
typedef enum {
    DRV_TDE_MB_COLOR_FMT_JPG_YCBCR400MBP = 0, /* Semi-planar YUV400 format, for JPG decoding */
    /* Semi-planar YUV422 format, horizontal sampling, for JPG decoding */
    DRV_TDE_MB_COLOR_FMT_JPG_YCBCR422MBHP,
    /* Semi-planar YUV422 format, vertical sampling, for JPG decoding */
    DRV_TDE_MB_COLOR_FMT_JPG_YCBCR422MBVP,
    DRV_TDE_MB_COLOR_FMT_MP1_YCBCR420MBP, /* Semi-planar YUV420 format */
    DRV_TDE_MB_COLOR_FMT_MP2_YCBCR420MBP, /* Semi-planar YUV420 format */
    DRV_TDE_MB_COLOR_FMT_MP2_YCBCR420MBI, /* Semi-planar YUV400 format */
    DRV_TDE_MB_COLOR_FMT_JPG_YCBCR420MBP, /* Semi-planar YUV400 format, for JPG pictures */
    DRV_TDE_MB_COLOR_FMT_JPG_YCBCR444MBP, /* Semi-planar YUV444 format, for JPG pictures */
    DRV_TDE_MB_COLOR_FMT_MAX
} drv_tde_mb_color_fmt;
/* Structure of the bitmap information set by customers */
typedef struct {
    td_phys_addr_t phys_addr; /* Header address of a bitmap or the Y component */
    td_u32 phys_len;
    drv_tde_color_fmt color_format; /* Color format */

    td_u32 height; /* Bitmap height */

    td_u32 width; /* Bitmap width */

    td_u32 stride; /* Stride of a bitmap or the Y component */

    td_bool is_ycbcr_clut; /* Whether the CLUT is in the YCbCr space. */

    td_bool alpha_max_is_255; /* The maximum alpha value of a bitmap is 255 or 128. */

    td_bool support_alpha_ex_1555; /* <Whether to enable the alpha extension of an ARGB1555 bitmap. */
    td_u8 alpha0; /* Values of alpha0 and alpha1, used as the ARGB1555 format */
    td_u8 alpha1; /* Values of alpha0 and alpha1, used as the ARGB1555 format */
    td_phys_addr_t cbcr_phys_addr; /* Address of the CbCr component, pilot */
    td_u32 cbcr_phys_len;
    td_u32 cbcr_stride; /* Stride of the CbCr component, pilot */
    /* Address of the color look-up table (CLUT), for color extension or color correction */
    td_phys_addr_t clut_phys_addr;
    td_u32 clut_phys_len;
} drv_tde_surface;

/* Definition of the semi-planar YUV data */
typedef struct {
    drv_tde_mb_color_fmt mb_color_format; /* YUV format */
    td_phys_addr_t y_addr; /* Physical address of the Y component */
    td_u32 y_len;
    td_u32 y_width; /* Width of the Y component */
    td_u32 y_height; /* Height of the Y component */
    td_u32 y_stride; /*  Stride of the Y component, indicating bytes in each row */
    td_phys_addr_t cbcr_phys_addr; /* Width of the UV component */
    td_u32 cbcr_phys_len;
    td_u32 cbcr_stride; /* Stride of the UV component, indicating the bytes in each row */
} drv_tde_mb_surface;
/* Definition of the TDE rectangle */
typedef struct {
    td_s32 pos_x;   /* Horizontal coordinate */
    td_s32 pos_y;   /* Vertical coordinate */
    td_u32 width;  /* Width */
    td_u32 height; /* Height */
} drv_tde_rect;

/* dma module */
typedef struct {
    drv_tde_surface *dst_surface;
    drv_tde_rect *dst_rect;
} drv_tde_none_src;

/* single source */
typedef struct {
    drv_tde_surface *src_surface;
    drv_tde_surface *dst_surface;
    drv_tde_rect *src_rect;
    drv_tde_rect *dst_rect;
} drv_tde_single_src;

/* mb source */
typedef struct {
    drv_tde_mb_surface *mb_surface;
    drv_tde_surface *dst_surface;
    drv_tde_rect *src_rect;
    drv_tde_rect *dst_rect;
} drv_tde_mb_src;

/* double source */
typedef struct {
    drv_tde_surface *bg_surface;
    drv_tde_surface *fg_surface;
    drv_tde_surface *dst_surface;
    drv_tde_rect *bg_rect;
    drv_tde_rect *fg_rect;
    drv_tde_rect *dst_rect;
} drv_tde_double_src;

typedef enum {
    DRV_TDE_ALPHA_BLENDING_NONE = 0x0,
    DRV_TDE_ALPHA_BLENDING_BLEND = 0x1,
    /* Alpha blending */ /* <CNcomment:Alpha blend */
    DRV_TDE_ALPHA_BLENDING_ROP = 0x2,
    /* ROP blending */ /* <CNcomment:rop superimposing */
    DRV_TDE_ALPHA_BLENDING_COLORIZE = 0x4,
    /* Colorize operation */                                 /* <CNcomment:Colorize */
    DRV_TDE_ALPHA_BLENDING_MAX = 0x8 /* End of enumeration */ /* <CNcomment:End of enumeration */
} drv_tde_alpha_blending;

typedef enum {
    DRV_TDE_ROP_BLACK = 0,   /* Blackness */
    DRV_TDE_ROP_NOTMERGEPEN, /* ~(S2 | S1) */
    DRV_TDE_ROP_MASKNOTPEN,  /* ~S2&S1    */
    DRV_TDE_ROP_NOTCOPYPEN,  /* ~S2       */
    DRV_TDE_ROP_MASKPENNOT,  /* S2&~S1    */
    DRV_TDE_ROP_NOT,         /* ~S1       */
    DRV_TDE_ROP_XORPEN,      /* S2^S1     */
    DRV_TDE_ROP_NOTMASKPEN,  /* NOTMASKPEN ~(S2 & S1) */
    DRV_TDE_ROP_MASKPEN,     /* S2&S1     */
    DRV_TDE_ROP_NOTXORPEN,   /* ~(S2^S1)  */
    DRV_TDE_ROP_NOP,         /* S1        */
    DRV_TDE_ROP_MERGENOTPEN, /* ~S2|S1    */
    DRV_TDE_ROP_COPYPEN,     /* S2        */
    DRV_TDE_ROP_MERGEPENNOT, /* S2|~S1    */
    DRV_TDE_ROP_MERGEPEN,    /* S2|S1     */
    DRV_TDE_ROP_WHITE,       /* Whiteness */
    DRV_TDE_ROP_MAX
} drv_tde_rop_mode;

typedef enum {
    DRV_TDE_MIRROR_NONE = 0,
    /* No mirror */ /* <CNcomment:No mirror */
    DRV_TDE_MIRROR_HORIZONTAL,
    /* Horizontal mirror */ /* <CNcomment:Horizontal mirror */
    DRV_TDE_MIRROR_VERTICAL,
    /* Vertical mirror */ /* <CNcomment:Vertical mirror */
    DRV_TDE_MIRROR_BOTH,
    /* Horizontal and vertical mirror */ /* <CNcomment:Horizontal and vertical mirror */
    DRV_TDE_MIRROR_MAX
} drv_tde_mirror_mode;

typedef enum {
    DRV_TDE_CLIP_MODE_NONE = 0,
    DRV_TDE_CLIP_MODE_INSIDE,
    DRV_TDE_CLIP_MODE_OUTSIDE,
    DRV_TDE_CLIP_MODE_MAX
} drv_tde_clip_mode;

typedef enum {
    DRV_TDE_MB_RESIZE_NONE = 0,
    DRV_TDE_MB_RESIZE_QUALITY_LOW,
    DRV_TDE_MB_RESIZE_QUALITY_MIDDLE,
    DRV_TDE_MB_RESIZE_QUALITY_HIGH,
    DRV_TDE_MB_RESIZE_MAX
} drv_tde_mb_resize;

typedef struct {
    td_u32 color_value;
    drv_tde_color_fmt color_format;
} drv_tde_fill_color;

/* Definition of corner_rect */
typedef struct {
    td_u32 width;
    td_u32 height;
    td_u32 inner_color;
    td_u32 outer_color;
} drv_tde_corner_rect_info;

typedef struct {
    td_s32 start_x;
    td_s32 start_y;
    td_s32 end_x;
    td_s32 end_y;
    td_u32 thick;
    td_u32 color;
} drv_tde_line;

typedef enum {
    DRV_TDE_COLOR_KEY_MODE_NONE = 0,
    DRV_TDE_COLOR_KEY_MODE_FOREGROUND,
    DRV_TDE_COLOR_KEY_MODE_BACKGROUND,
    DRV_TDE_COLOR_KEY_MODE_MAX
} drv_tde_color_key_mode;

typedef struct {
    td_u8 component_min; /* Minimum value of a component */
    td_u8 component_max; /* Maximum value of a component */
    td_u8 is_component_out;
    td_u8 is_component_ignore; /* Whether to ignore a component */
    td_u8 component_mask; /* Component mask */
} drv_tde_color_key_comp;

typedef union {
    struct {
        drv_tde_color_key_comp alpha; /* alpha component */
        drv_tde_color_key_comp red; /* red component */
        drv_tde_color_key_comp green; /* green component */
        drv_tde_color_key_comp blue; /* blue component */
    } argb_colorkey;
    struct {
        drv_tde_color_key_comp alpha; /* alpha component */
        drv_tde_color_key_comp y; /* y component */
        drv_tde_color_key_comp cb; /* cb component */
        drv_tde_color_key_comp cr; /* cr component */
    } ycbcr_colorkey;
    struct {
        drv_tde_color_key_comp alpha; /* alpha component */
        drv_tde_color_key_comp clut; /* clut component */
    } clut_colorkey;
} drv_tde_color_key;

typedef enum {
    DRV_TDE_OUT_ALPHA_FROM_NORM = 0,
    DRV_TDE_OUT_ALPHA_FROM_BACKGROUND,
    /* Output from the background bitmap */
    DRV_TDE_OUT_ALPHA_FROM_FOREGROUND,
    /* Output from the foreground bitmap */
    DRV_TDE_OUT_ALPHA_FROM_GLOBALALPHA,
    /* Output from the global alpha */
    DRV_TDE_OUT_ALPHA_FROM_MAX
} drv_tde_out_alpha_from;

typedef enum {
    DRV_TDE_FILTER_MODE_COLOR = 0,
    /* Filter the color */
    DRV_TDE_FILTER_MODE_ALPHA,
    /* Filter the alpha channel */
    DRV_TDE_FILTER_MODE_BOTH,
    /* Filter the color and alpha channel */
    DRV_TDE_FILTER_MODE_NONE,
    /* No filter */
    DRV_TDE_FILTER_MODE_MAX
} drv_tde_filter_mode;

/* blend mode */
typedef enum {
    DRV_TDE_BLEND_ZERO = 0x0,
    DRV_TDE_BLEND_ONE,
    DRV_TDE_BLEND_SRC2COLOR,
    DRV_TDE_BLEND_INVSRC2COLOR,
    DRV_TDE_BLEND_SRC2ALPHA,
    DRV_TDE_BLEND_INVSRC2ALPHA,
    DRV_TDE_BLEND_SRC1COLOR,
    DRV_TDE_BLEND_INVSRC1COLOR,
    DRV_TDE_BLEND_SRC1ALPHA,
    DRV_TDE_BLEND_INVSRC1ALPHA,
    DRV_TDE_BLEND_SRC2ALPHASAT,
    DRV_TDE_BLEND_MAX
} drv_tde_blend_mode;

typedef enum {
    DRV_TDE_BLEND_CMD_NONE = 0x0, /* fs: sa      fd: 1.0-sa */
    DRV_TDE_BLEND_CMD_CLEAR,      /* fs: 0.0     fd: 0.0    */
    DRV_TDE_BLEND_CMD_SRC,        /* fs: 1.0     fd: 0.0    */
    DRV_TDE_BLEND_CMD_SRCOVER,    /* fs: 1.0     fd: 1.0-sa */
    DRV_TDE_BLEND_CMD_DSTOVER,    /* fs: 1.0-da  fd: 1.0    */
    DRV_TDE_BLEND_CMD_SRCIN,      /* fs: da      fd: 0.0    */
    DRV_TDE_BLEND_CMD_DSTIN,      /* fs: 0.0     fd: sa     */
    DRV_TDE_BLEND_CMD_SRCOUT,     /* fs: 1.0-da  fd: 0.0    */
    DRV_TDE_BLEND_CMD_DSTOUT,     /* fs: 0.0     fd: 1.0-sa */
    DRV_TDE_BLEND_CMD_SRCATOP,    /* fs: da      fd: 1.0-sa */
    DRV_TDE_BLEND_CMD_DSTATOP,    /* fs: 1.0-da  fd: sa     */
    DRV_TDE_BLEND_CMD_ADD,        /* fs: 1.0     fd: 1.0    */
    DRV_TDE_BLEND_CMD_XOR,        /* fs: 1.0-da  fd: 1.0-sa */
    DRV_TDE_BLEND_CMD_DST,        /* fs: 0.0     fd: 1.0    */
    DRV_TDE_BLEND_CMD_CONFIG,
    /* You can set the parameteres */
    DRV_TDE_BLEND_CMD_MAX
} drv_tde_blend_cmd;
/* Options for the alpha blending operation */
typedef struct {
    td_bool global_alpha_en; /* Global alpha enable */
    td_bool pixel_alpha_en; /* Pixel alpha enable */
    td_bool src1_alpha_premulti; /* Src1 alpha premultiply enable */
    td_bool src2_alpha_premulti; /* Src2 alpha premultiply enable */
    drv_tde_blend_cmd blend_cmd; /* Alpha blending command */
    drv_tde_blend_mode src1_blend_mode;
    drv_tde_blend_mode src2_blend_mode;
} drv_tde_blend_opt;
/* CSC parameter option */
typedef struct {
    td_bool src_csc_user_en; /* User-defined ICSC parameter enable */
    td_bool src_csc_param_reload_en; /* User-defined ICSC parameter reload enable */
    td_bool dst_csc_user_en; /* User-defined OCSC parameter enable */
    td_bool dst_csc_param_reload_en; /* User-defined OCSC parameter reload enable */
    td_phys_addr_t src_csc_param_addr; /* ICSC parameter address. The address must be 128-bit aligned. */
    td_s32 src_csc_param_len;
    td_phys_addr_t dst_csc_param_addr; /* OCSC parameter address. The address must be 128-bit aligned. */
    td_s32 dst_csc_param_len;
} drv_tde_csc_opt;
/* Definition of blit operation options */
typedef struct {
    drv_tde_alpha_blending alpha_blending_cmd; /* Logical operation type */

    drv_tde_rop_mode rop_color; /* ROP type of the color space */

    drv_tde_rop_mode rop_alpha; /* ROP type of the alpha component */

    drv_tde_color_key_mode colorkey_mode; /* Colorkey mode */

    drv_tde_color_key colorkey_value; /* Colorkey value */

    drv_tde_clip_mode clip_mode; /* Perform the clip operation within or beyond the area */

    drv_tde_rect clip_rect; /* Definition of the clipping area */

    td_bool resize; /* Whether to scale */

    drv_tde_filter_mode filter_mode; /* Filtering mode during scaling */

    drv_tde_mirror_mode mirror; /* Mirror type */

    td_bool clut_reload; /* Whether to reload the CLUT */

    td_u8 global_alpha; /* Global alpha value */

    drv_tde_out_alpha_from out_alpha_from; /* Source of the output alpha */

    td_u32 color_resize; /* Colorize value */

    drv_tde_blend_opt blend_opt;

    drv_tde_csc_opt csc_opt;
    td_bool is_compress;
    td_bool is_decompress;
} drv_tde_opt;
/* Definition of macroblock operation options */
typedef struct {
    drv_tde_clip_mode clip_mode; /* Clip mode */
    drv_tde_rect clip_rect; /* Definition of the clipping area */
    drv_tde_mb_resize resize_en; /* Scaling information */
    /* If the alpha value is not set, the maximum alpha value is output by default. */
    td_bool is_set_out_alpha;
    td_u8 out_alpha; /* Global alpha for operation */
} drv_tde_mb_opt;
/* Definition of the pattern filling operation */
typedef struct {
    drv_tde_alpha_blending alpha_blending_cmd; /* Logical operation type */

    drv_tde_rop_mode rop_color; /* ROP type of the color space */

    drv_tde_rop_mode rop_alpha; /* ROP type of the alpha component */

    drv_tde_color_key_mode colorkey_mode; /* Colorkey mode */

    drv_tde_color_key colorkey_value; /* Colorkey value */

    drv_tde_clip_mode clip_mode; /* Clip mode */

    drv_tde_rect clip_rect; /* Clipping area */

    td_bool clut_reload; /* Whether to reload the CLUT */

    td_u8 global_alpha; /* Global alpha */

    drv_tde_out_alpha_from out_alpha_from; /* Source of the output alpha */

    td_u32 color_resize; /* Colorize value */

    drv_tde_blend_opt blend_opt; /* Options of the blending operation */

    drv_tde_csc_opt csc_opt; /* CSC parameter option */
} drv_tde_pattern_fill_opt;
/* Definition of rotation directions */
typedef enum {
    DRV_TDE_ROTATE_CLOCKWISE_90 = 0, /* Rotate 90 degree clockwise */
    DRV_TDE_ROTATE_CLOCKWISE_180,    /* Rotate 180 degree clockwise */
    DRV_TDE_ROTATE_CLOCKWISE_270,    /* Rotate 270 degree clockwise */
    DRV_TDE_ROTATE_MAX
} drv_tde_rotate_angle;

/* composed surface info */
typedef struct {
    drv_tde_surface src_surface;
    drv_tde_rect src_rect;
    drv_tde_rect dst_rect;
    drv_tde_opt option;
} drv_tde_composer;

/* composed surface list */
typedef struct {
    td_u32 surface_num;
    drv_tde_surface *dst_surface;
    drv_tde_composer *composor;
} drv_tde_surface_list;

typedef struct {
    td_s32 handle; /* TDE handle */
    drv_tde_surface bg_surface; /* background surface */
    drv_tde_rect bg_rect; /* background surface operating rect */
    drv_tde_surface fg_surface; /* foreground surface */
    drv_tde_rect fg_rect; /* foreground surface operating rect    */
    drv_tde_surface dst_surface; /* target surface */
    drv_tde_rect dst_rect; /* target surface operating rect */
    drv_tde_opt option; /* operating option */
    td_u32 null_indicator; /* flag of mask , 1: valid ,0: invalid */
} drv_tde_bitblit_cmd;

typedef struct {
    td_s32 handle; /* TDE handle */
    drv_tde_surface fg_surface; /* background surface */
    drv_tde_rect fg_rect; /* background surface operating rect */
    drv_tde_surface dst_surface; /* foreground surface */
    drv_tde_rect dst_rect; /* foreground surface operating rect */
    drv_tde_fill_color fill_color; /* fill color */
    drv_tde_opt option; /* operating option */
    td_u32 null_indicator; /* flag of mask , 1: valid ,0: invalid */
} drv_tde_solid_draw_cmd;

typedef struct {
    td_s32 handle; /* TDE handle */
    drv_tde_surface src_surface; /* src surface */
    drv_tde_rect src_rect; /* src surface rect */
    drv_tde_surface dst_surface; /* target surface */
    drv_tde_rect dst_rect; /* target rect */
} drv_tde_quick_copy_cmd;

typedef struct {
    td_s32 handle; /* TDE handle */
    drv_tde_surface dst_surface; /* target surface */
    drv_tde_rect dst_rect; /* target surface rect */
    td_u32 fill_data;  /* fill color */
} drv_tde_quick_fill_cmd;

/* multi draw rect info */
typedef struct {
    td_s32 handle; /* < TDE handle */
    td_u32 corner_rect_num;
    drv_tde_surface dst_surface; /* < target surface */
    td_u64 corner_rect_region; /* < target surface rect */
    td_u64 corner_rect_info; /* target corner rect */
} drv_tde_multi_draw_cmd;

typedef struct {
    td_s32 handle;
    td_u32 line_num;
    drv_tde_surface dst_surface;
    td_u64 line;
} drv_tde_draw_line_cmd;

typedef struct {
    td_s32 handle; /* TDE handle */
    drv_tde_surface src_surface; /* src surface */
    drv_tde_rect src_rect; /* src rect */
    drv_tde_surface dst_surface; /* target surface */
    drv_tde_rect dst_rect; /* target rect */
} drv_tde_quick_resize_cmd;

typedef struct {
    td_s32 handle; /* TDE handle */
    drv_tde_mb_surface mb_src_surface; /* src surface */
    drv_tde_rect mb_src_rect; /* src rect */
    drv_tde_surface dst_surface; /* target surface */
    drv_tde_rect dst_rect; /* target rect */
    drv_tde_mb_opt mb_option; /* operating option */
} drv_tde_mb_blit_cmd;

typedef struct {
    td_s32 handle; /* TDE handle */
    td_bool is_sync; /* weather sync */
    td_bool is_block; /* weather block */
    td_u32 time_out; /* time out(ms) */
} drv_tde_end_job_cmd;

typedef struct {
    td_s32 handle; /* TDE handle */
    drv_tde_surface bg_surface; /* bk surface */
    drv_tde_rect bg_rect; /* bk rect */
    drv_tde_surface fg_surface; /* fore surface */
    drv_tde_rect fg_rect; /* fore rect */
    drv_tde_surface mask_surface; /* mask surface  */
    drv_tde_rect mask_rect; /* mask surface rect */
    drv_tde_surface dst_surface; /* target surface */
    drv_tde_rect dst_rect; /* target rect */
    drv_tde_rop_mode rop_color; /* RGB rop type */
    drv_tde_rop_mode rop_alpha; /* alpha rop type */
} drv_tde_bitmap_maskrop_cmd;

typedef struct {
    td_s32 handle; /* TDE handle */
    drv_tde_surface bg_surface; /* bk surface */
    drv_tde_rect bg_rect; /* bk rect */
    drv_tde_surface fg_surface; /* fore surface */
    drv_tde_rect fg_rect; /* fore rect */
    drv_tde_surface mask_surface; /* mask surface  */
    drv_tde_rect mask_rect; /* mask surface rect */
    drv_tde_surface dst_surface; /* target surface */
    drv_tde_rect dst_rect; /* target rect */
    td_u8 alpha; /* global alpha */
    td_u8 reserved0;
    td_u8 reserved1;
    td_u8 reserved2;
    drv_tde_alpha_blending blend_mode; /* blend opt */
} drv_tde_bitmap_maskblend_cmd;

typedef struct {
    td_s32 handle; /* TDE handle */
    drv_tde_surface bg_surface; /* bk surface */
    drv_tde_rect bg_rect; /* bk rect */
    drv_tde_surface fg_surface; /* fore surface */
    drv_tde_rect fg_rect; /* fore rect */
    drv_tde_surface dst_surface; /* target surface */
    drv_tde_rect dst_rect; /* target rect */
    drv_tde_pattern_fill_opt option; /* option */
    td_u32 null_indicator; /* flag of mask , 1: valid ,0: invalid */
} drv_tde_pattern_fill_cmd;

typedef struct {
    td_s32 handle;
    drv_tde_surface src_surface;
    drv_tde_rect src_rect;
    drv_tde_surface dst_surface;
    drv_tde_rect dst_rect;
    drv_tde_rotate_angle rotate_angle;
} drv_tde_rotate_cmd;

typedef enum {
    IOC_TDE_BEGIN_JOB = 0,
    IOC_TDE_BIT_BLIT,
    IOC_TDE_SOLID_DRAW,
    IOC_TDE_QUICK_COPY,
    IOC_TDE_QUICK_RESIZE,
    IOC_TDE_QUICK_FILL,
    IOC_TDE_MB_BITBLT,
    IOC_TDE_END_JOB,
    IOC_TDE_WAITFORDONE,
    IOC_TDE_CANCEL_JOB,
    IOC_TDE_BITMAP_MASKROP,
    IOC_TDE_BITMAP_MASKBLEND,
    IOC_TDE_WAITALLDONE,
    IOC_TDE_RESET,
    IOC_TDE_SET_ALPHATHRESHOLD_VALUE,
    IOC_TDE_GET_ALPHATHRESHOLD_VALUE,
    IOC_TDE_SET_ALPHATHRESHOLD_STATE,
    IOC_TDE_GET_ALPHATHRESHOLD_STATE,
    IOC_TDE_PATTERN_FILL,
    IOC_TDE_ROTATE,
    IOC_TDE_DRAW_MULTI_RECT,
    IOC_TDE_DRAW_LINE,
    IOC_TDE_BUTT
} ioc_tde;

#define TDE_IOC_MAGIC 't'
#define TDE_BEGIN_JOB                _IOR(TDE_IOC_MAGIC, IOC_TDE_BEGIN_JOB, td_s32)
#define TDE_BIT_BLIT                 _IOW(TDE_IOC_MAGIC, IOC_TDE_BIT_BLIT, drv_tde_bitblit_cmd)
#define TDE_SOLID_DRAW               _IOW(TDE_IOC_MAGIC, IOC_TDE_SOLID_DRAW, drv_tde_solid_draw_cmd)
#define TDE_QUICK_COPY               _IOW(TDE_IOC_MAGIC, IOC_TDE_QUICK_COPY, drv_tde_quick_copy_cmd)
#define TDE_QUICK_RESIZE             _IOW(TDE_IOC_MAGIC, IOC_TDE_QUICK_RESIZE, drv_tde_quick_resize_cmd)
#define TDE_QUICK_FILL               _IOW(TDE_IOC_MAGIC, IOC_TDE_QUICK_FILL, drv_tde_quick_fill_cmd)
#define TDE_MB_BITBLT                _IOW(TDE_IOC_MAGIC, IOC_TDE_MB_BITBLT, drv_tde_mb_blit_cmd)
#define TDE_END_JOB                  _IOW(TDE_IOC_MAGIC, IOC_TDE_END_JOB, drv_tde_end_job_cmd)
#define TDE_WAITFORDONE              _IOW(TDE_IOC_MAGIC, IOC_TDE_WAITFORDONE, td_s32)
#define TDE_CANCEL_JOB               _IOW(TDE_IOC_MAGIC, IOC_TDE_CANCEL_JOB, td_s32)
#define TDE_WAITALLDONE              _IO(TDE_IOC_MAGIC, IOC_TDE_WAITALLDONE)
#define TDE_SET_ALPHATHRESHOLD_VALUE _IOW(TDE_IOC_MAGIC, IOC_TDE_SET_ALPHATHRESHOLD_VALUE, td_u8)
#define TDE_GET_ALPHATHRESHOLD_VALUE _IOR(TDE_IOC_MAGIC, IOC_TDE_GET_ALPHATHRESHOLD_VALUE, td_u8)
#define TDE_SET_ALPHATHRESHOLD_STATE _IOW(TDE_IOC_MAGIC, IOC_TDE_SET_ALPHATHRESHOLD_STATE, td_bool)
#define TDE_GET_ALPHATHRESHOLD_STATE _IOR(TDE_IOC_MAGIC, IOC_TDE_GET_ALPHATHRESHOLD_STATE, td_bool)
#define TDE_PATTERN_FILL             _IOW(TDE_IOC_MAGIC, IOC_TDE_PATTERN_FILL, drv_tde_pattern_fill_cmd)
#define TDE_ROTATE                   _IOW(TDE_IOC_MAGIC, IOC_TDE_ROTATE, drv_tde_rotate_cmd)
#define TDE_DRAW_MULTI_RECT          _IOW(TDE_IOC_MAGIC, IOC_TDE_DRAW_MULTI_RECT, drv_tde_multi_draw_cmd)
#define TDE_DRAW_LINE                _IOW(TDE_IOC_MAGIC, IOC_TDE_DRAW_LINE, drv_tde_draw_line_cmd)

typedef td_s32 (*tde_module_open)(td_void);
typedef td_s32 (*tde_module_close)(td_void);
typedef td_s32 (*tde_module_begin_job)(td_s32 *);
typedef td_s32 (*tde_module_end_job)(drv_tde_end_job_cmd *, drv_tde_func_callback, td_void *);
typedef td_s32 (*tde_module_cancel_job)(td_s32);
typedef td_s32 (*tde_module_wait_for_done)(td_s32, td_u32);
typedef td_s32 (*tde_module_wait_all_done)(td_bool);
typedef td_s32 (*tde_module_quick_copy)(td_s32, const drv_tde_single_src *);
typedef td_s32 (*tde_module_quick_fill)(td_s32, drv_tde_surface *, drv_tde_rect *, td_u32);
typedef td_s32 (*tde_module_quick_resize)(td_s32, drv_tde_surface *, drv_tde_rect *, drv_tde_surface *, drv_tde_rect *);
typedef td_s32 (*tde_module_blit)(td_s32, drv_tde_double_src *, const drv_tde_opt *);
typedef td_s32 (*tde_module_mbblit)(td_s32, drv_tde_mb_src *, drv_tde_mb_opt *);
typedef td_s32 (*tde_module_solid_draw)(td_s32, const drv_tde_single_src *, drv_tde_fill_color *, const drv_tde_opt *);
typedef td_s32 (*tde_module_rotate)(td_s32, drv_tde_single_src *, drv_tde_rotate_angle);

typedef td_s32 (*tde_module_set_alpha_threshold_value)(td_u8);
typedef td_s32 (*tde_module_set_alpha_threshold_state)(td_bool);
typedef td_s32 (*tde_module_get_alpha_threshold_state)(td_bool *);
typedef td_s32 (*tde_module_calc_scale_rect)(const drv_tde_rect *, const drv_tde_rect *, drv_tde_rect *,
    drv_tde_rect *);
typedef td_s32 (*tde_module_set_gfbg_mmz_addr)(td_phys_addr_t, td_u32, td_u32);

typedef struct {
    tde_module_open drv_tde_module_open;
    tde_module_close drv_tde_module_close;
    tde_module_begin_job drv_tde_module_begin_job;
    tde_module_end_job drv_tde_module_end_job;
    tde_module_cancel_job drv_tde_module_cancel_job;
    tde_module_wait_for_done drv_tde_module_wait_for_done;
    tde_module_wait_all_done drv_tde_module_wait_all_done;
    tde_module_quick_copy drv_tde_module_quick_copy;
    tde_module_quick_fill drv_tde_module_quick_fill;
    tde_module_quick_resize drv_tde_module_quick_resize;
    tde_module_blit drv_tde_module_blit;
    tde_module_mbblit drv_tde_module_mb_blit;
    tde_module_solid_draw drv_tde_module_solid_draw;
    tde_module_rotate drv_tde_module_rotate;
    tde_module_set_alpha_threshold_value drv_tde_module_set_alpha_threshold_value;
    tde_module_set_alpha_threshold_state drv_tde_module_set_alpha_threshold_state;
    tde_module_get_alpha_threshold_state drv_tde_module_get_alpha_threshold_state;
    tde_module_calc_scale_rect drv_tde_module_cal_scale_rect;
    tde_module_set_gfbg_mmz_addr drv_tde_module_set_gfbg_mmz_addr;
} ot_tde_export_func;


#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif /* DRV_TDE_H */
