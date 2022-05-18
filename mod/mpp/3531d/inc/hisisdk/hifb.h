/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: hifb hal operation
 */

#ifndef __HIFB_H__
#define __HIFB_H__

#include <linux/fb.h>
#include <linux/ioctl.h>
#include "hi_type.h"
#include "hi_common.h"

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

#define IOC_TYPE_HIFB       'F'
/* To obtain the colorkey of an overlay layer */
#define FBIOGET_COLORKEY_HIFB       _IOR(IOC_TYPE_HIFB, 90, hi_fb_colorkey)
/* To set the colorkey of an overlay layer */
#define FBIOPUT_COLORKEY_HIFB       _IOW(IOC_TYPE_HIFB, 91, hi_fb_colorkey)
/* To get the alpha of an overlay layer */
#define FBIOGET_ALPHA_HIFB          _IOR(IOC_TYPE_HIFB, 92, hi_fb_alpha)
/* To set the alpha of an overlay layer */
#define FBIOPUT_ALPHA_HIFB          _IOW(IOC_TYPE_HIFB, 93, hi_fb_alpha)
/* To get the origin of an overlay layer on the screen */
#define FBIOGET_SCREEN_ORIGIN_HIFB  _IOR(IOC_TYPE_HIFB, 94, hi_fb_point)
/* To set the origin of an overlay layer on the screen */
#define FBIOPUT_SCREEN_ORIGIN_HIFB  _IOW(IOC_TYPE_HIFB, 95, hi_fb_point)
/* To obtain the anti-flicker setting of an overlay layer */
#define FBIOGET_DEFLICKER_HIFB       _IOR(IOC_TYPE_HIFB, 98, hi_fb_deflicker)
/* To set the anti-flicker setting of an overlay layer */
#define FBIOPUT_DEFLICKER_HIFB       _IOW(IOC_TYPE_HIFB, 99, hi_fb_deflicker)
/* To wait for the vertical blanking region of an overlay layer */
#define FBIOGET_VER_BLANK_HIFB         _IO(IOC_TYPE_HIFB, 100)
/* To set the display state of an overlay layer */
#define FBIOPUT_SHOW_HIFB           _IOW(IOC_TYPE_HIFB, 101, hi_bool)
/* To obtain the display state of an overlay layer */
#define FBIOGET_SHOW_HIFB           _IOR(IOC_TYPE_HIFB, 102, hi_bool)
/* to obtain the capability of an overlay layer */
#define FBIOGET_CAPABILITY_HIFB    _IOR(IOC_TYPE_HIFB, 103, hi_fb_capability)
/* set the screen output size */
#define FBIOPUT_SCREEN_SIZE          _IOW(IOC_TYPE_HIFB, 130, hi_fb_size)
/* get the screen output size */
#define FBIOGET_SCREEN_SIZE          _IOR(IOC_TYPE_HIFB, 131, hi_fb_size)
/* To display multiple surfaces in turn and set the alpha and colorkey attributes */
#define FBIOFLIP_SURFACE            _IOW(IOC_TYPE_HIFB, 132, hi_fb_surfaceex)
/* To set the compression function status of an overlay layer */
#define FBIOPUT_COMPRESSION_HIFB    _IOW(IOC_TYPE_HIFB, 133, hi_bool)
/* To obtain the compression function status of an overlay layer */
#define FBIOGET_COMPRESSION_HIFB    _IOR(IOC_TYPE_HIFB, 134, hi_bool)
/* To set the hdr function status of an overlay layer */
#define FBIOPUT_DYNAMIC_RANGE_HIFB  _IOW(IOC_TYPE_HIFB, 139, hi_fb_dynamic_range)
/* To get the hdr function status of an overlay layer */
#define FBIOGET_DYNAMIC_RANGE_HIFB  _IOR(IOC_TYPE_HIFB, 140, hi_fb_dynamic_range)
/* To create the layer */
#define FBIO_CREATE_LAYER           _IO(IOC_TYPE_HIFB, 149)
/* To destory the layer */
#define FBIO_DESTROY_LAYER          _IO(IOC_TYPE_HIFB, 150)

/* crusor handle */
/* Attention:surface in cursor will be released by user */
#define FBIOPUT_CURSOR_INFO           _IOW(IOC_TYPE_HIFB, 104, hi_fb_cursor)
#define FBIOGET_CURSOR_INFO           _IOW(IOC_TYPE_HIFB, 105, hi_fb_cursor)

#define FBIOPUT_CURSOR_STATE          _IOW(IOC_TYPE_HIFB, 106, hi_bool)
#define FBIOGET_CURSOR_STATE          _IOW(IOC_TYPE_HIFB, 107, hi_bool)

#define FBIOPUT_CURSOR_POS            _IOW(IOC_TYPE_HIFB, 108, hi_fb_point)
#define FBIOGET_CURSOR_POS            _IOR(IOC_TYPE_HIFB, 109, hi_fb_point)

#define FBIOPUT_CURSOR_COLORKEY       _IOR(IOC_TYPE_HIFB, 110, hi_fb_colorkey)
#define FBIOGET_CURSOR_COLORKEY       _IOW(IOC_TYPE_HIFB, 111, hi_fb_colorkey)
#define FBIOPUT_CURSOR_ALPHA          _IOR(IOC_TYPE_HIFB, 112, hi_fb_alpha)
#define FBIOGET_CURSOR_ALPHA          _IOW(IOC_TYPE_HIFB, 113, hi_fb_alpha)

/* cursor will be separated from attached layer automatically if you attach cursor to another layer,that means
cursor can be attached to only one layer at any time */
#define FBIOPUT_CURSOR_ATTCH_CURSOR    _IOW(IOC_TYPE_HIFB, 114, hi_u32)
#define FBIOPUT_CURSOR_DETACH_CURSOR   _IOW(IOC_TYPE_HIFB, 115, hi_u32)
/* To set the layer information */
#define FBIOPUT_LAYER_INFO                _IOW(IOC_TYPE_HIFB, 120, hi_fb_layer_info)
/* To get the layer information */
#define FBIOGET_LAYER_INFO                _IOR(IOC_TYPE_HIFB, 121, hi_fb_layer_info)
/* To get canvas buf */
#define FBIOGET_CANVAS_BUF             _IOR(IOC_TYPE_HIFB, 123, hi_fb_buf)
/* To refresh the displayed contents in extended mode */
#define FBIO_REFRESH                      _IOW(IOC_TYPE_HIFB, 124, hi_fb_buf)
/* sync refresh */
#define FBIO_WAITFOR_FREFRESH_DONE        _IO(IOC_TYPE_HIFB, 125)
/* To set the mirror mode */
#define FBIOPUT_MIRROR_MODE            _IOW(IOC_TYPE_HIFB, 126, hi_fb_mirror_mode)
/* To get the mirror mode */
#define FBIOGET_MIRROR_MODE            _IOW(IOC_TYPE_HIFB, 127, hi_fb_mirror_mode)
/* To set the rotate mode */
#define FBIOPUT_ROTATE_MODE            _IOW(IOC_TYPE_HIFB, 128, hi_fb_rotate_mode)
/* To get the rotate mode */
#define FBIOGET_ROTATE_MODE            _IOW(IOC_TYPE_HIFB, 129, hi_fb_rotate_mode)
/* To set the DDR detect zone of an overlay layer */
#define FBIOPUT_MDDRDETECT_HIFB    _IOW(IOC_TYPE_HIFB, 135, hi_fb_ddr_zone)
/* To get the DDR detect zone of an overlay layer */
#define FBIOGET_MDDRDETECT_HIFB    _IOW(IOC_TYPE_HIFB, 136, hi_fb_ddr_zone)
/* To draw smart rect */
#define FBIO_DRAW_SMART_RECT    _IOW(IOC_TYPE_HIFB, 151, hi_fb_smart_rect_param)


typedef struct {
    hi_u32  width;
    hi_u32  height;
} hi_fb_size;

typedef enum {
    HI_FB_DYNAMIC_RANGE_SDR8 = 0,
    HI_FB_DYNAMIC_RANGE_SDR10,
    HI_FB_DYNAMIC_RANGE_HDR10,
    HI_FB_DYNAMIC_RANGE_HLG,
    HI_FB_DYNAMIC_RANGE_SLF,
    HI_FB_DYNAMIC_RANGE_BUTT
} hi_fb_dynamic_range;

typedef struct {
    hi_bool enable;         /* colorkey enable flag */
    hi_u32 value;              /* colorkey value, maybe contains alpha */
} hi_fb_colorkey;

typedef struct {
    hi_s32 x;
    hi_s32 y;
    hi_s32 width;
    hi_s32 height;
} hi_fb_rect;

typedef struct {
    hi_s32 x_pos;         /* <  horizontal position */
    hi_s32 y_pos;         /* <  vertical position */
} hi_fb_point;

typedef struct {
    hi_u32  hor_deflicker_level;    /* <  horizontal deflicker level */
    hi_u32  ver_deflicker_level;    /* <  vertical deflicker level */
    hi_u8   ATTRIBUTE *hor_deflicker_coef;    /* <  horizontal deflicker coefficient */
    hi_u8   ATTRIBUTE *ver_deflicker_coef;    /* <  vertical deflicker coefficient */
} hi_fb_deflicker;

/* Alpha info */
typedef struct {
    hi_bool alpha_en;   /*  alpha enable flag */
    hi_bool alpha_chn_en;  /*  alpha channel enable flag */
    hi_u8 alpha0;         /*  alpha0 value, used in ARGB1555 */
    hi_u8 alpha1;         /*  alpha1 value, used in ARGB1555 */
    hi_u8 global_alpha;    /*  global alpha value */
    hi_u8 reserved;
} hi_fb_alpha;

typedef enum {
    HI_FB_FORMAT_RGB565 = 0,
    HI_FB_FORMAT_RGB888,              /*  RGB888 24bpp */

    HI_FB_FORMAT_KRGB444,       /*  RGB444 16bpp */
    HI_FB_FORMAT_KRGB555,       /*  RGB555 16bpp */
    HI_FB_FORMAT_KRGB888,       /*  RGB888 32bpp */

    HI_FB_FORMAT_ARGB4444,      /* ARGB4444 */
    HI_FB_FORMAT_ARGB1555,      /* ARGB1555 */
    HI_FB_FORMAT_ARGB8888,      /* ARGB8888 */
    HI_FB_FORMAT_ARGB8565,      /* ARGB8565 */

    HI_FB_FORMAT_RGBA4444,      /* ARGB4444 */
    HI_FB_FORMAT_RGBA5551,      /* RGBA5551 */
    HI_FB_FORMAT_RGBA5658,      /* RGBA5658 */
    HI_FB_FORMAT_RGBA8888,      /* RGBA8888 */

    HI_FB_FORMAT_BGR565,        /* BGR565 */
    HI_FB_FORMAT_BGR888,        /* BGR888 */
    HI_FB_FORMAT_ABGR4444,      /* ABGR4444 */
    HI_FB_FORMAT_ABGR1555,      /* ABGR1555 */
    HI_FB_FORMAT_ABGR8888,      /* ABGR8888 */
    HI_FB_FORMAT_ABGR8565,      /* ABGR8565 */
    HI_FB_FORMAT_KBGR444,       /* BGR444 16bpp */
    HI_FB_FORMAT_KBGR555,       /* BGR555 16bpp */
    HI_FB_FORMAT_KBGR888,       /* BGR888 32bpp */

    HI_FB_FORMAT_1BPP,          /* clut1 */
    HI_FB_FORMAT_2BPP,          /* clut2 */
    HI_FB_FORMAT_4BPP,          /* clut4 */
    HI_FB_FORMAT_8BPP,          /* clut8 */
    HI_FB_FORMAT_ACLUT44,       /* AClUT44 */
    HI_FB_FORMAT_ACLUT88,         /* ACLUT88 */
    HI_FB_FORMAT_PUYVY,         /* UYVY */
    HI_FB_FORMAT_PYUYV,         /* YUYV */
    HI_FB_FORMAT_PYVYU,         /* YVYU */
    HI_FB_FORMAT_YUV888,        /* YUV888 */
    HI_FB_FORMAT_AYUV8888,      /* AYUV8888 */
    HI_FB_FORMAT_YUVA8888,      /* YUVA8888 */
    HI_FB_FORMAT_BUTT
} hi_fb_color_format;

typedef struct {
    hi_bool is_key_rgb;
    hi_bool is_key_alpha;      /* whether support colorkey alpha */
    hi_bool is_global_alpha;   /* whether support global alpha */
    hi_bool is_cmap;          /* whether support color map */
    hi_bool has_cmap_reg;    /* whether has color map register */
    hi_bool is_color_format[HI_FB_FORMAT_BUTT]; /* support which color format */
    hi_bool is_vo_scale;       /* support vo scale */
    /* whether support a certain layer */
    hi_bool is_layer_support;
    hi_u32  max_width;    /* the max pixels per line */
    hi_u32  max_height;   /* the max lines */
    hi_u32  min_width;    /* the min pixels per line */
    hi_u32  min_height;   /* the min lines */
    hi_u32  ver_deflicker_level;   /* vertical deflicker level, 0 means vertical deflicker is unsupported */
    hi_u32  hor_deflicker_level;   /* horizontal deflicker level, 0 means horizontal deflicker is unsupported  */
    hi_bool  is_decompress;
    hi_bool  is_premul;
    hi_bool  is_ghdr;         /* new feature. is ghdr supported. */
    hi_bool  is_osb;   /* new feature. is smart rect supported */
} hi_fb_capability;

/* refresh mode */
typedef enum {
    HI_FB_LAYER_BUF_DOUBLE = 0x0,       /* 2 display buf in fb */
    HI_FB_LAYER_BUF_ONE    = 0x1,       /* 1 display buf in fb */
    HI_FB_LAYER_BUF_NONE   = 0x2, /* no display buf in fb,the buf user refreshed will be directly set to VO */
    HI_FB_LAYER_BUF_DOUBLE_IMMEDIATE = 0x3, /* 2 display buf in fb, each refresh will be displayed */
    HI_FB_LAYER_BUF_BUTT
} hi_fb_layer_buf;

/* surface info */
typedef struct {
    hi_phys_addr_t  phys_addr;     /* start physical address */
    hi_u32  width;       /* width pixels */
    hi_u32  height;      /* height pixels */
    hi_u32  pitch;       /* line pixels */
    hi_fb_color_format format; /* color format */
    hi_fb_dynamic_range  dynamic_range;    /* dynamic range. */
} hi_fb_surface;

typedef struct {
    hi_phys_addr_t phys_addr;
    hi_fb_alpha alpha;
    hi_fb_colorkey colorkey;
} hi_fb_surfaceex;

/* refresh surface info */
typedef struct {
    hi_fb_surface canvas;
    hi_fb_rect update_rect;       /* refresh region */
} hi_fb_buf;

/* cursor info */
typedef struct {
    hi_fb_surface cursor;
    hi_fb_point hot_pos;
} hi_fb_cursor;

/* DDR detect zone info */
typedef struct {
    hi_u32 start_section;
    hi_u32 zone_nums;
} hi_fb_ddr_zone;

/* antiflicker level */
/* Auto means fb will choose a appropriate antiflicker level automatically according to the color info of map */
typedef enum {
    HI_FB_LAYER_ANTIFLICKER_NONE = 0x0,  /* no antiflicker */
    HI_FB_LAYER_ANTIFLICKER_LOW = 0x1,   /* low level */
    HI_FB_LAYER_ANTIFLICKER_MID = 0x2, /* middle level */
    HI_FB_LAYER_ANTIFLICKER_HIGH = 0x3, /* high level */
    HI_FB_LAYER_ANTIFLICKER_AUTO = 0x4, /* auto */
    HI_FB_LAYER_ANTIFLICKER_BUTT
} hi_fb_layer_antiflicker_level;

/* mirror mode  */
typedef enum {
    HI_FB_MIRROR_NONE = 0x0,
    HI_FB_MIRROR_HOR = 0x1,
    HI_FB_MIRROR_VER = 0x2,
    HI_FB_MIRROR_BOTH = 0x3,
    HI_FB_MIRROR_BUTT
} hi_fb_mirror_mode;

/* rotate mode */
typedef enum {
    HI_FB_ROTATE_NONE = 0x0,
    HI_FB_ROTATE_90 = 0x1,
    HI_FB_ROTATE_180 = 0x2,
    HI_FB_ROTATE_270 = 0x3,
    HI_FB_ROTATE_BUTT
} hi_fb_rotate_mode;

/* layer info maskbit */
typedef enum {
    HI_FB_LAYER_MASK_BUF_MODE = 0x1,           /* buf mode bitmask */
    HI_FB_LAYER_MASK_ANTIFLICKER_MODE = 0x2,  /* antiflicker mode bitmask */
    HI_FB_LAYER_MASK_POS = 0x4,               /* the position bitmask */
    HI_FB_LAYER_MASK_CANVAS_SIZE = 0x8,      /* canvassize bitmask */
    HI_FB_LAYER_MASK_DISPLAY_SIZE = 0x10,       /* displaysize bitmask */
    HI_FB_LAYER_MASK_SCREEN_SIZE = 0x20,     /* screensize bitmask */
    HI_FB_LAYER_MASK_MUL = 0x40,           /* pre-mult bitmask */
    HI_FB_LAYER_MASK_BUTT
} hi_fb_layer_info_maskbit;

/* layer info */
typedef struct {
    hi_fb_layer_buf buf_mode;
    hi_fb_layer_antiflicker_level antiflicker_level;
    hi_s32 x_pos;           /*  the x pos of origion point in screen */
    hi_s32 y_pos;           /*  the y pos of origion point in screen */
    hi_u32 canvas_width;    /*  the width of canvas buffer */
    hi_u32 canvas_height;   /*  the height of canvas buffer */
    /* the width of display buf in fb.for 0 buf, there is no display buf in fb, so it's effectless */
    hi_u32 display_width;
    hi_u32 display_height;  /*  the height of display buf in fb. */
    hi_u32 screen_width;    /*  the width of screen */
    hi_u32 screen_height;   /*  the height of screen */
    hi_bool is_premul;      /*  The data drawed in buf is premul data or not */
    hi_u32 mask;           /*  param modify mask bit */
} hi_fb_layer_info;

/* smart rect mode  */
typedef enum {
    HI_FB_SMART_RECT_NONE = 0x0, /* disable smart rect */
    HI_FB_SMART_RECT_SOLID = 0x1,
    HI_FB_SMART_RECT_FILLED = 0x2,
    HI_FB_SMART_RECT_CORNER = 0x3,
    HI_FB_SMART_RECT_BUTT
} hi_fb_smart_rect_mode;

/* smart rect */
typedef struct {
    hi_fb_smart_rect_mode mode;
    hi_fb_rect rect;
    hi_u32 corner_length; /* max 255 only use for HI_FB_SMART_RECT_CORNER mode */
    hi_u32 thick; /* max 32 */
    hi_u32 color_value;
} hi_fb_smart_rect;

typedef struct {
    hi_u32 num; /* max 128 */
    hi_fb_smart_rect *rect_start;
} hi_fb_smart_rect_param;
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


#endif /* __HIFB_H__ */

