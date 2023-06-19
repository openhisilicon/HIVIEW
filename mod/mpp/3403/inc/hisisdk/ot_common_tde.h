/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */
#ifndef OT_COMMON_TDE_H
#define OT_COMMON_TDE_H

#include "ot_type.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/* tde start err no. */
#define OT_ERR_TDE_BASE ((td_s32)(((0x80UL + 0x20UL) << 24) | (100 << 16) | (4 << 13) | 1))

typedef enum {
    OT_ERR_TDE_DEV_NOT_OPEN = OT_ERR_TDE_BASE, /* tde device not open yet */
    OT_ERR_TDE_DEV_OPEN_FAILED,                /* open tde device failed */
    OT_ERR_TDE_NULL_PTR,                       /* input parameters contain null ptr */
    OT_ERR_TDE_NO_MEM,                         /* malloc failed  */
    OT_ERR_TDE_INVALID_HANDLE,                 /* invalid job handle */
    OT_ERR_TDE_INVALID_PARAM,                  /* invalid parameter */
    OT_ERR_TDE_NOT_ALIGNED,                    /* aligned error for position, stride, width */
    OT_ERR_TDE_MINIFICATION,                   /* invalid minification */
    OT_ERR_TDE_CLIP_AREA,                      /* clip area and operation area have no intersection */
    OT_ERR_TDE_JOB_TIMEOUT,                    /* blocked job wait timeout */
    OT_ERR_TDE_UNSUPPORTED_OPERATION,          /* unsupported operation */
    OT_ERR_TDE_QUERY_TIMEOUT,                  /* query time out */
    OT_ERR_TDE_INTERRUPT,                      /* blocked job was interrupted */
    OT_ERR_TDE_BUTT,
}ot_tde_err_code;

/* RGB and packet YUV formats and semi-planar YUV format */
typedef enum {
    OT_TDE_COLOR_FORMAT_RGB444 = 0,       /* RGB444 format */
    OT_TDE_COLOR_FORMAT_BGR444,           /* BGR444 format */
    OT_TDE_COLOR_FORMAT_RGB555,           /* RGB555 format */
    OT_TDE_COLOR_FORMAT_BGR555,           /* BGR555 format */
    OT_TDE_COLOR_FORMAT_RGB565,           /* RGB565 format */
    OT_TDE_COLOR_FORMAT_BGR565,           /* BGR565 format */
    OT_TDE_COLOR_FORMAT_RGB888,           /* RGB888 format */
    OT_TDE_COLOR_FORMAT_BGR888,           /* BGR888 format */
    OT_TDE_COLOR_FORMAT_ARGB4444,         /* ARGB4444 format */
    OT_TDE_COLOR_FORMAT_ABGR4444,         /* ABGR4444 format */
    OT_TDE_COLOR_FORMAT_RGBA4444,         /* RGBA4444 format */
    OT_TDE_COLOR_FORMAT_BGRA4444,         /* BGRA4444 format */
    OT_TDE_COLOR_FORMAT_ARGB1555,         /* ARGB1555 format */
    OT_TDE_COLOR_FORMAT_ABGR1555,         /* ABGR1555 format */
    OT_TDE_COLOR_FORMAT_RGBA1555,         /* RGBA1555 format */
    OT_TDE_COLOR_FORMAT_BGRA1555,         /* BGRA1555 format */
    OT_TDE_COLOR_FORMAT_ARGB8565,         /* ARGB8565 format */
    OT_TDE_COLOR_FORMAT_ABGR8565,         /* ABGR8565 format */
    OT_TDE_COLOR_FORMAT_RGBA8565,         /* RGBA8565 format */
    OT_TDE_COLOR_FORMAT_BGRA8565,         /* BGRA8565 format */
    OT_TDE_COLOR_FORMAT_ARGB8888,         /* ARGB8888 format */
    OT_TDE_COLOR_FORMAT_ABGR8888,         /* ABGR8888 format */
    OT_TDE_COLOR_FORMAT_RGBA8888,         /* RGBA8888 format */
    OT_TDE_COLOR_FORMAT_BGRA8888,         /* BGRA8888 format */
    OT_TDE_COLOR_FORMAT_RABG8888,         /* RABG8888 format */
    /* 1-bit palette format without alpha component. Each pixel occupies one bit. */
    OT_TDE_COLOR_FORMAT_CLUT1,
    /* 2-bit palette format without alpha component. Each pixel occupies two bits. */
    OT_TDE_COLOR_FORMAT_CLUT2,
    /* 4-bit palette format without alpha component. Each pixel occupies four bits. */
    OT_TDE_COLOR_FORMAT_CLUT4,
    /* 8-bit palette format without alpha component. Each pixel occupies eight bits. */
    OT_TDE_COLOR_FORMAT_CLUT8,
    /* 4-bit palette format with alpha component. Each pixel occupies 8 bit. */
    OT_TDE_COLOR_FORMAT_ACLUT44,
    /* 8-bit palette format with alpha component. Each pixel occupies 16 bit. */
    OT_TDE_COLOR_FORMAT_ACLUT88,
    OT_TDE_COLOR_FORMAT_A1,               /* Alpha format. Each pixel occupies one bit. */
    OT_TDE_COLOR_FORMAT_A8,               /* Alpha format. Each pixel occupies eight bits. */
    OT_TDE_COLOR_FORMAT_YCbCr888,         /* YUV packet format without alpha component */
    OT_TDE_COLOR_FORMAT_AYCbCr8888,       /* YUV packet format with alpha component */
    OT_TDE_COLOR_FORMAT_YCbCr422,         /* YUV packet422 format */
    OT_TDE_COLOR_FORMAT_PKGVYUY,          /* YUV packet422 format, VYUY format */
    OT_TDE_COLOR_FORMAT_BYTE,             /* Only for fast copy */
    OT_TDE_COLOR_FORMAT_HALFWORD,         /* Only for fast copy */
    OT_TDE_COLOR_FORMAT_JPG_YCbCr400MBP,  /* Semi-planar YUV400 format, for JPG decoding */
    OT_TDE_COLOR_FORMAT_JPG_YCbCr422MBHP, /* Semi-planar YUV422 format, horizontal sampling, for JPG decoding */
    OT_TDE_COLOR_FORMAT_JPG_YCbCr422MBVP, /* Semi-planar YUV422 format, vertical sampling, for JPG decoding */
    OT_TDE_COLOR_FORMAT_MP1_YCbCr420MBP,  /* Semi-planar YUV420 format */
    OT_TDE_COLOR_FORMAT_MP2_YCbCr420MBP,  /* Semi-planar YUV420 format */
    OT_TDE_COLOR_FORMAT_MP2_YCbCr420MBI,  /* Semi-planar YUV420 format */
    OT_TDE_COLOR_FORMAT_JPG_YCbCr420MBP,  /* Semi-planar YUV420 format, for JPG decoding */
    OT_TDE_COLOR_FORMAT_JPG_YCbCr444MBP,  /* Semi-planar YUV444 format */
    OT_TDE_COLOR_FORMAT_MAX               /* End of enumeration */
} ot_tde_color_format;

/* Definition of the semi-planar YUV format */
typedef enum {
    OT_TDE_MB_COLOR_FORMAT_JPG_YCbCr400MBP = 0, /* Semi-planar YUV400 format, for JPG decoding */
    /* Semi-planar YUV422 format, horizontal sampling, for JPG decoding */
    OT_TDE_MB_COLOR_FORMAT_JPG_YCbCr422MBHP,
    OT_TDE_MB_COLOR_FORMAT_JPG_YCbCr422MBVP, /* Semi-planar YUV422 format, vertical sampling, for JPG decoding */
    OT_TDE_MB_COLOR_FORMAT_MP1_YCbCr420MBP,  /* Semi-planar YUV420 format */
    OT_TDE_MB_COLOR_FORMAT_MP2_YCbCr420MBP,  /* Semi-planar YUV420 format */
    OT_TDE_MB_COLOR_FORMAT_MP2_YCbCr420MBI,  /* Semi-planar YUV420 format */
    OT_TDE_MB_COLOR_FORMAT_JPG_YCbCr420MBP,  /* Semi-planar YUV420 format, for JPG pictures */
    OT_TDE_MB_COLOR_FORMAT_JPG_YCbCr444MBP,  /* Semi-planar YUV444 format, for JPG pictures */
    OT_TDE_MB_COLOR_FORMAT_MAX
} ot_tde_mb_color_format;

/* Structure of the bitmap information set by customers */
typedef struct {
    td_phys_addr_t phys_addr; /* Header address of a bitmap or the Y component */
    td_u32 phys_len;
    ot_tde_color_format color_format; /* Color format */
    td_u32 height; /* Bitmap height */
    td_u32 width; /* Bitmap width */
    td_u32 stride; /* Stride of a bitmap or the Y component */
    td_bool is_ycbcr_clut; /* Whether the CLUT is in the YCbCr space. */
    td_bool alpha_max_is_255; /* The maximum alpha value of a bitmap is 255 or 128. */
    td_bool support_alpha_ex_1555; /* Whether to enable the alpha extension of an ARGB1555 bitmap. */
    td_u8 alpha0;        /* Values of alpha0 and alpha1, used as the ARGB1555 format */
    td_u8 alpha1;        /* Values of alpha0 and alpha1, used as the ARGB1555 format */
    td_phys_addr_t cbcr_phys_addr;    /* Address of the CbCr component, pilot */
    td_u32 cbcr_phys_len;
    td_u32 cbcr_stride;  /* Stride of the CbCr component, pilot */
    /* <Address of the color look-up table (CLUT), for color extension or color correction */
    td_phys_addr_t clut_phys_addr;
    td_u32 clut_phys_len;
} ot_tde_surface;

/* Definition of the semi-planar YUV data */
typedef struct {
    ot_tde_mb_color_format mb_color_format; /* YUV format */
    td_phys_addr_t y_addr;             /* Physical address of the Y component */
    td_u32 y_len;
    td_u32 y_width;            /* Width of the Y component */
    td_u32 y_height;           /* Height of the Y component */
    td_u32 y_stride;           /* Stride of the Y component, indicating bytes in each row */
    td_phys_addr_t cbcr_phys_addr;      /* Width of the UV component */
    td_u32 cbcr_phys_len;
    td_u32 cbcr_stride;        /* Stride of the UV component, indicating the bytes in each row */
} ot_tde_mb_surface;

/* Definition of the TDE rectangle */
typedef struct {
    td_s32 pos_x;   /* Horizontal coordinate */
    td_s32 pos_y;   /* Vertical coordinate */
    td_u32 width;  /* Width */
    td_u32 height; /* Height */
} ot_tde_rect;

/* dma module */
typedef struct {
    ot_tde_surface *dst_surface;
    ot_tde_rect *dst_rect;
} ot_tde_none_src;

/* single source */
typedef struct {
    ot_tde_surface *src_surface;
    ot_tde_surface *dst_surface;
    ot_tde_rect *src_rect;
    ot_tde_rect *dst_rect;
} ot_tde_single_src;

/* mb source */
typedef struct {
    ot_tde_mb_surface *mb_surface;
    ot_tde_surface *dst_surface;
    ot_tde_rect *src_rect;
    ot_tde_rect *dst_rect;
} ot_tde_mb_src;

/* double source */
typedef struct {
    ot_tde_surface *bg_surface;
    ot_tde_surface *fg_surface;
    ot_tde_surface *dst_surface;
    ot_tde_rect *bg_rect;
    ot_tde_rect *fg_rect;
    ot_tde_rect *dst_rect;
} ot_tde_double_src;

/* triple source */
typedef struct {
    ot_tde_surface *bg_surface;
    ot_tde_surface *fg_surface;
    ot_tde_surface *dst_surface;
    ot_tde_surface *mask_surface;
    ot_tde_rect *bg_rect;
    ot_tde_rect *fg_rect;
    ot_tde_rect *mask_rect;
    ot_tde_rect *dst_rect;
} ot_tde_triple_src;

/* Logical operation type */
typedef enum {
    OT_TDE_ALPHA_BLENDING_NONE = 0x0,     /* No alpha and raster of operation (ROP) blending */
    OT_TDE_ALPHA_BLENDING_BLEND = 0x1,    /* Alpha blending */
    OT_TDE_ALPHA_BLENDING_ROP = 0x2,      /* ROP blending */
    OT_TDE_ALPHA_BLENDING_COLORIZE = 0x4, /* Colorize operation */
    OT_TDE_ALPHA_BLENDING_MAX = 0x8       /* End of enumeration */
} ot_tde_alpha_blending;

/* Definition of ROP codes */
typedef enum {
    OT_TDE_ROP_BLACK = 0,   /* Blackness */
    OT_TDE_ROP_NOTMERGEPEN, /* ~(S2 | S1) */
    OT_TDE_ROP_MASKNOTPEN,  /* ~S2&S1 */
    OT_TDE_ROP_NOTCOPYPEN,  /* ~S2 */
    OT_TDE_ROP_MASKPENNOT,  /* S2&~S1 */
    OT_TDE_ROP_NOT,         /* ~S1 */
    OT_TDE_ROP_XORPEN,      /* S2^S1 */
    OT_TDE_ROP_NOTMASKPEN,  /* ~(S2 & S1) */
    OT_TDE_ROP_MASKPEN,     /* S2&S1 */
    OT_TDE_ROP_NOTXORPEN,   /* ~(S2^S1) */
    OT_TDE_ROP_NOP,         /* S1 */
    OT_TDE_ROP_MERGENOTPEN, /* ~S2|S1 */
    OT_TDE_ROP_COPYPEN,     /* S2 */
    OT_TDE_ROP_MERGEPENNOT, /* S2|~S1 */
    OT_TDE_ROP_MERGEPEN,    /* S2|S1 */
    OT_TDE_ROP_WHITE,       /* Whiteness */
    OT_TDE_ROP_MAX
} ot_tde_rop_mode;

/* Definition of the blit mirror */
typedef enum {
    OT_TDE_MIRROR_NONE = 0,   /* No mirror */
    OT_TDE_MIRROR_HOR, /* Horizontal mirror */
    OT_TDE_MIRROR_VER,   /* Vertical mirror */
    OT_TDE_MIRROR_BOTH,       /* Horizontal and vertical mirror */
    OT_TDE_MIRROR_MAX
} ot_tde_mirror_mode;

/* Clip operation type */
typedef enum {
    OT_TDE_CLIP_MODE_NONE = 0, /* No clip */
    OT_TDE_CLIP_MODE_INSIDE,   /* Clip the data within the rectangle to output and discard others */
    OT_TDE_CLIP_MODE_OUTSIDE,  /* Clip the data outside the rectangle to output and discard others */
    OT_TDE_CLIP_MODE_MAX
} ot_tde_clip_mode;

/* Scaling mode for the macroblock */
typedef enum {
    OT_TDE_MB_RESIZE_NONE = 0,       /* No scaling */
    OT_TDE_MB_RESIZE_QUALITY_LOW,    /* Low-quality scaling */
    OT_TDE_MB_RESIZE_QUALITY_MIDDLE, /* Medium-quality scaling */
    OT_TDE_MB_RESIZE_QUALITY_HIGH,   /* High-quality scaling */
    OT_TDE_MB_RESIZE_MAX
} ot_tde_mb_resize;

/* Definition of fill colors */
typedef struct {
    ot_tde_color_format color_format; /* TDE pixel format */
    td_u32 color_value;         /* Fill colors that vary according to pixel formats */
} ot_tde_fill_color;

/* Definition of colorkey modes */
typedef enum {
    OT_TDE_COLORKEY_MODE_NONE = 0,   /* No colorkey */
    /* When performing the colorkey operation on the foreground bitmap,
    you need to perform this operation before the CLUT for color extension
    and perform this operation after the CLUT for color correction. */
    OT_TDE_COLORKEY_MODE_FG,
    OT_TDE_COLORKEY_MODE_BG, /* Perform the colorkey operation on the background bitmap */
    OT_TDE_COLORKEY_MODE_MAX
} ot_tde_colorkey_mode;

/* Definition of colorkey range */
typedef struct {
    td_u8 min_component;   /* Minimum value of a component */
    td_u8 max_component;   /* Maximum value of a component */
    td_u8 is_component_out;    /* The colorkey of a component is within or beyond the range. */
    td_u8 is_component_ignore; /* Whether to ignore a component. */
    td_u8 component_mask;  /* Component mask */
    td_u8 component_reserved;
    td_u8 component_reserved1;
    td_u8 component_reserved2;
} ot_tde_colorkey_component;

/* Definition of colorkey values */
typedef union {
    struct {
        ot_tde_colorkey_component alpha; /* Alpha component */
        ot_tde_colorkey_component red;   /* Red component */
        ot_tde_colorkey_component green; /* Green component */
        ot_tde_colorkey_component blue;  /* Blue component */
    } argb_colorkey;                     /* AUTO:ot_tde_colorkey_mode:OT_TDE_COLORKEY_MODE_NONE; */
    struct {
        ot_tde_colorkey_component alpha; /* Alpha component */
        ot_tde_colorkey_component y;     /* Y component */
        ot_tde_colorkey_component cb;    /* Cb component */
        ot_tde_colorkey_component cr;    /* Cr component */
    } ycbcr_colorkey;                    /* AUTO:ot_tde_colorkey_mode:OT_TDE_COLORKEY_MODE_FG; */
    struct {
        ot_tde_colorkey_component alpha; /* Alpha component */
        ot_tde_colorkey_component clut;  /* Palette component */
    } clut_colorkey;                     /* AUTO:ot_tde_colorkey_mode:OT_TDE_COLORKEY_MODE_BG; */
} ot_tde_colorkey;

/* Definition of alpha output sources */
typedef enum {
    OT_TDE_OUT_ALPHA_FROM_NORM = 0,    /* Output from the result of alpha blending or anti-flicker */
    OT_TDE_OUT_ALPHA_FROM_BG,  /* Output from the background bitmap */
    OT_TDE_OUT_ALPHA_FROM_FG,  /* Output from the foreground bitmap */
    OT_TDE_OUT_ALPHA_FROM_GLOBALALPHA, /* Output from the global alpha */
    OT_TDE_OUT_ALPHA_FROM_MAX
} ot_tde_out_alpha_from;

/* Definition of filtering */
typedef enum {
    OT_TDE_FILTER_MODE_COLOR = 0, /* Filter the color */
    OT_TDE_FILTER_MODE_ALPHA,     /* Filter the alpha channel */
    OT_TDE_FILTER_MODE_BOTH,      /* Filter the color and alpha channel */
    OT_TDE_FILTER_MODE_NONE,      /* No filter */
    OT_TDE_FILTER_MODE_MAX
} ot_tde_filter_mode;

/* Configuration of the anti-flicker channel */
typedef enum {
    OT_TDE_DEFLICKER_LEVEL_MODE_NONE = 0, /* No anti-flicker */
    OT_TDE_DEFLICKER_LEVEL_MODE_RGB,      /* Perform anti-flicker on the RGB component */
    OT_TDE_DEFLICKER_LEVEL_MODE_BOTH,     /* Perform anti-flicker on the alpha component */
    OT_TDE_DEFLICKER_LEVEL_MODE_MAX
} ot_tde_deflicker_mode;

/* blend mode */
typedef enum {
    OT_TDE_BLEND_ZERO = 0x0,
    OT_TDE_BLEND_ONE,
    OT_TDE_BLEND_SRC2COLOR,
    OT_TDE_BLEND_INVSRC2COLOR,
    OT_TDE_BLEND_SRC2ALPHA,
    OT_TDE_BLEND_INVSRC2ALPHA,
    OT_TDE_BLEND_SRC1COLOR,
    OT_TDE_BLEND_INVSRC1COLOR,
    OT_TDE_BLEND_SRC1ALPHA,
    OT_TDE_BLEND_INVSRC1ALPHA,
    OT_TDE_BLEND_SRC2ALPHASAT,
    OT_TDE_BLEND_MAX
} ot_tde_blend_mode;

/* Alpha blending command. You can set parameters or select Porter or Duff. */
/* pixel = (source * fs + destination * fd),
   sa = source alpha,
   da = destination alpha */
typedef enum {
    OT_TDE_BLEND_CMD_NONE = 0x0, /* fs: sa      fd: 1.0-sa */
    OT_TDE_BLEND_CMD_CLEAR,      /* fs: 0.0     fd: 0.0 */
    OT_TDE_BLEND_CMD_SRC,        /* fs: 1.0     fd: 0.0 */
    OT_TDE_BLEND_CMD_SRCOVER,    /* fs: 1.0     fd: 1.0-sa */
    OT_TDE_BLEND_CMD_DSTOVER,    /* fs: 1.0-da  fd: 1.0 */
    OT_TDE_BLEND_CMD_SRCIN,      /* fs: da      fd: 0.0 */
    OT_TDE_BLEND_CMD_DSTIN,      /* fs: 0.0     fd: sa */
    OT_TDE_BLEND_CMD_SRCOUT,     /* fs: 1.0-da  fd: 0.0 */
    OT_TDE_BLEND_CMD_DSTOUT,     /* fs: 0.0     fd: 1.0-sa */
    OT_TDE_BLEND_CMD_SRCATOP,    /* fs: da      fd: 1.0-sa */
    OT_TDE_BLEND_CMD_DSTATOP,    /* fs: 1.0-da  fd: sa */
    OT_TDE_BLEND_CMD_ADD,        /* fs: 1.0     fd: 1.0 */
    OT_TDE_BLEND_CMD_XOR,        /* fs: 1.0-da  fd: 1.0-sa */
    OT_TDE_BLEND_CMD_DST,        /* fs: 0.0     fd: 1.0 */
    OT_TDE_BLEND_CMD_CONFIG,     /* You can set the parameteres. */
    OT_TDE_BLEND_CMD_MAX
} ot_tde_blend_cmd;

/* Options for the alpha blending operation */
typedef struct {
    td_bool global_alpha_en;       /* Global alpha enable */
    td_bool pixel_alpha_en;        /* Pixel alpha enable */
    td_bool src1_alpha_premulti;       /* Src1 alpha premultiply enable */
    td_bool src2_alpha_premulti;       /* Src2 alpha premultiply enable */
    ot_tde_blend_cmd blend_cmd;        /* Alpha blending command */
    /* Src1 blending mode select. It is valid when blend_cmd is set to OT_TDE_BLEND_CMD_CONFIG. */
    ot_tde_blend_mode src1_blend_mode;
    /* Src2 blending mode select. It is valid when blend_cmd is set to OT_TDE_BLEND_CMD_CONFIG. */
    ot_tde_blend_mode src2_blend_mode;
} ot_tde_blend_opt;

/* CSC parameter option */
typedef struct {
    td_bool src_csc_user_en;  /* User-defined ICSC parameter enable */
    td_bool src_csc_param_reload_en; /* User-defined ICSC parameter reload enable */
    td_bool dst_csc_user_en;  /* User-defined OCSC parameter enable */
    td_bool dst_csc_param_reload_en; /* User-defined OCSC parameter reload enable */
    td_phys_addr_t src_csc_param_addr; /* ICSC parameter address. The address must be 128-bit aligned. */
    td_s32 src_csc_param_len;
    td_phys_addr_t dst_csc_param_addr; /* OCSC parameter address. The address must be 128-bit aligned. */
    td_s32 dst_csc_param_len;
} ot_tde_csc_opt;

/* Definition of blit operation options */
typedef struct {
    ot_tde_alpha_blending alpha_blending_cmd; /* Logical operation type */

    ot_tde_rop_mode rop_color; /* ROP type of the color space */

    ot_tde_rop_mode rop_alpha; /* ROP type of the alpha component */

    ot_tde_colorkey_mode colorkey_mode; /* Colorkey mode */

    ot_tde_colorkey colorkey_value; /* Colorkey value */

    ot_tde_clip_mode clip_mode; /* Perform the clip operation within or beyond the area */

    ot_tde_rect clip_rect; /* Definition of the clipping area */

    ot_tde_deflicker_mode deflicker_mode; /* Anti-flicker mode */

    td_bool resize; /* Whether to scale */

    ot_tde_filter_mode filter_mode; /* Filtering mode during scaling */

    ot_tde_mirror_mode mirror; /* Mirror type */

    td_bool clut_reload; /* Whether to reload the CLUT */

    td_u8 global_alpha; /* Global alpha value */

    ot_tde_out_alpha_from out_alpha_from; /* Source of the output alpha */

    td_u32 color_resize; /* Colorize value */

    ot_tde_blend_opt blend_opt;

    ot_tde_csc_opt csc_opt;
    td_bool is_compress;
    td_bool is_decompress;
} ot_tde_opt;

/* Definition of macroblock operation options */
typedef struct {
    ot_tde_clip_mode clip_mode; /* Clip mode */

    ot_tde_rect clip_rect; /* Definition of the clipping area */

    td_bool is_deflicker; /* Whether to perform anti-flicker */

    ot_tde_mb_resize resize_en; /* Scaling information */

    /* If the alpha value is not set, the maximum alpha value is output by default. */
    td_bool is_set_out_alpha;

    td_u8 out_alpha; /* Global alpha for operation */
} ot_tde_mb_opt;

/* Definition of the pattern filling operation */
typedef struct {
    ot_tde_alpha_blending alpha_blending_cmd; /* Logical operation type */

    ot_tde_rop_mode rop_color; /* ROP type of the color space */

    ot_tde_rop_mode rop_alpha; /* ROP type of the alpha component */

    ot_tde_colorkey_mode colorkey_mode; /* Colorkey mode */

    ot_tde_colorkey colorkey_value; /* Colorkey value */

    ot_tde_clip_mode clip_mode; /* Clip mode */

    ot_tde_rect clip_rect; /* Clipping area */

    td_bool clut_reload; /* Whether to reload the CLUT */

    td_u8 global_alpha; /* Global alpha */

    ot_tde_out_alpha_from out_alpha_from; /* Source of the output alpha */

    td_u32 color_resize; /* Colorize value */

    ot_tde_blend_opt blend_opt; /* Options of the blending operation */

    ot_tde_csc_opt csc_opt; /* CSC parameter option */
} ot_tde_pattern_fill_opt;

/* Definition of rotation directions */
typedef enum {
    OT_TDE_ROTATE_CLOCKWISE_90 = 0, /* Rotate 90 degree clockwise */
    OT_TDE_ROTATE_CLOCKWISE_180,    /* Rotate 180 degree clockwise */
    OT_TDE_ROTATE_CLOCKWISE_270,    /* Rotate 270 degree clockwise */
    OT_TDE_ROTATE_MAX
} ot_tde_rotate_angle;

/* Definition of anti-flicker levels */
typedef enum {
    OT_TDE_DEFLICKER_LEVEL_AUTO = 0, /* Adaptation. The anti-flicker coefficient is selected by the TDE. */
    OT_TDE_DEFLICKER_LEVEL_LOW,      /* Low-level anti-flicker */
    OT_TDE_DEFLICKER_LEVEL_MIDDLE,   /* Medium-level anti-flicker */
    OT_TDE_DEFLICKER_LEVEL_HIGH,     /* High-level anti-flicker */
    OT_TDE_DEFLICKER_LEVEL_MAX
} ot_tde_deflicker_level;

/* Definition of corner_rect */
typedef struct {
    td_u32 width;
    td_u32 height;
    td_u32 inner_color;
    td_u32 outer_color;
} ot_tde_corner_rect_info;

typedef struct {
    ot_tde_rect *corner_rect_region;
    ot_tde_corner_rect_info *corner_rect_info;
} ot_tde_corner_rect;

typedef struct {
    td_s32 start_x;
    td_s32 start_y;
    td_s32 end_x;
    td_s32 end_y;
    td_u32 thick;
    td_u32 color;
} ot_tde_line;

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */
#endif /* OT_COMMON_TDE_H */
