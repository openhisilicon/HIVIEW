/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */

#ifndef __OT_MATH_H__
#define __OT_MATH_H__

#include "ot_type.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

/*
 * ABS(x)                 absolute value of x
 * SIGN(x)                sign of x
 * CMP(x,y)               0 if x==y; 1 if x>y; -1 if x<y
 */
#define ABS(x)          ((x) >= 0 ? (x) : (-(x)))
#define _SIGN(x)         ((x) >= 0 ? 1 : (-1))
#define CMP(x, y)        (((x) == (y)) ? 0 : (((x) > (y)) ? 1 : (-1)))

/*
 * MAX2(x,y)              maximum of x and y
 * MIN2(x,y)              minimum of x and y
 * MAX3(x,y,z)            maximum of x, y and z
 * MIN3(x,y,z)            minimum of x, y and z
 */
#define MAX2(x, y)       ((x) > (y) ? (x) : (y))
#define MIN2(x, y)       ((x) < (y) ? (x) : (y))
#define MAX3(x, y, z)     ((x) > (y) ? MAX2(x, z) : MAX2(y, z))
#define MIN3(x, y, z)     ((x) < (y) ? MIN2(x, z) : MIN2(y, z))

/*
 * CLIP3(x,min,max)       clip x within [min,max]
 * value_between(x,min.max)   True if x is between [min,max] inclusively.
 */
#define clip_min(x, min)          (((x) >= (min)) ? (x) : (min))
#define clip3(x, min, max)         ((x) < (min) ? (min) : ((x) > (max) ? (max) : (x)))
#define clip_max(x, max)          ((x) > (max) ? (max) : (x))
#define value_between(x, min, max) (((x) >= (min)) && ((x) <= (max)))

/*
 * MULTI_OF_2_POWER(x,a)  whether x is multiple of a(a must be power of 2)
 * OT_ALIGN_DOWN(x,a)     floor x to multiple of a(a must be power of 2)
 * OT_ALIGN_UP(x, a)            align x to multiple of a
 *
 * Example:
 * ALIGN UP, OT_ALIGN_UP(5,4) = 8
 * ALIGN DOWN, OT_ALIGN_DOWN(5,4)   = 4
 */
#define MULTI_OF_2_POWER(x, a)    (!((x) & ((a) - 1)))
#define OT_CEILING(x, a)           (((x) + (a)-1) / (a))

#define OT_ALIGN_UP(x, a)           ((((x) + ((a) - 1)) / (a)) * (a))
#define OT_ALIGN_DOWN(x, a)         (((x) / (a)) * (a))

#define OT_DIV_UP(x, a)             (((x) + ((a) - 1)) / (a))

/*
 * FRACTION32(de,nu)          fraction: nu(minator) / de(nominator).
 * NUMERATOR32(x)              of x(x is fraction)
 * DENOMINATOR32(x)           Denominator of x(x is fraction)
 * represent fraction in 32 bit. LSB 16 is numerator, MSB 16 is denominator
 * It is integer if denominator is 0.
 */
#define FRACTION32(de, nu)       (((de) << 16) | (nu))
#define NUMERATOR32(x)          ((x) & 0xffff)
#define DENOMINATOR32(x)        ((x) >> 16)

/*
 * RGB(r,g,b)    assemble the r,g,b to 24bit color
 * RGB_R(c)      get RED   from 24bit color
 * RGB_G(c)      get GREEN from 24bit color
 * RGB_B(c)      get BLUE  from 24bit color
 */
#define RGB(r, g, b) ((((r) & 0xff) << 16) | (((g) & 0xff) << 8) | ((b) & 0xff))
#define RGB_R(c)   (((c) & 0xff0000) >> 16)
#define RGB_G(c)   (((c) & 0xff00) >> 8)
#define RGB_B(c)   ((c) & 0xff)

/*
 * YUV(y,u,v)    assemble the y,u,v to 30bit color
 * YUV_Y(c)      get Y from 30bit color
 * YUV_U(c)      get U from 30bit color
 * YUV_V(c)      get V from 30bit color
 */
#define YUV(y, u, v) ((((y) & 0x03ff) << 20) | (((u) & 0x03ff) << 10) | ((v) & 0x03ff))
#define YUV_Y(c)   (((c) & 0x3ff00000) >> 20)
#define YUV_U(c)   (((c) & 0x000ffc00) >> 10)
#define YUV_V(c)   ((c) & 0x000003ff)

/*
 * YUV_8BIT(y,u,v)    assemble the y,u,v to 24bit color
 * YUV_8BIT_Y(c)      get Y from 24bit color
 * YUV_8BIT_U(c)      get U from 24bit color
 * YUV_8BIT_V(c)      get V from 24bit color
 */
#define YUV_8BIT(y, u, v) ((((y) & 0xff) << 16) | (((u) & 0xff) << 8) | ((v) & 0xff))
#define YUV_8BIT_Y(c)   (((c) & 0xff0000) >> 16)
#define YUV_8BIT_U(c)   (((c) & 0xff00) >> 8)
#define YUV_8BIT_V(c)   ((c) & 0xff)

#define ot_usleep(usec) \
    do { \
        usleep(usec); \
    } while (0)

/*
 * Get the span between two unsigned number, such as
 * SPAN(td_u32, 100, 200) is 200 - 100 = 100
 * SPAN(td_u32, 200, 100) is 0xFFFFFFFF - 200 + 100
 * SPAN(td_u64, 200, 100) is 0xFFFFFFFFFFFFFFFF - 200 + 100
 */
#define SPAN(type, begin, end) \
({                             \
    type b = (begin);          \
    type e = (end);            \
    (type)((b >= e) ? (b - e) : (b + ((~((type)0)) - e))); \
})

/*
 * ENDIAN32(x,y)              little endian <---> big endian
 * IS_LITTLE_END()            whether the system is little end mode
 */
#define  ENDIAN32(x)                   \
    (((x) << 24) |                     \
    (((x) & 0x0000ff00) << 8) |        \
    (((x) & 0x00ff0000) >> 8) |        \
    (((x) >> 24) & 0x000000ff))


/*
 * ENDIAN16(x,y)              little endian <---> big endian
 * IS_LITTLE_END()            whether the system is little end mode
 */
#define  ENDIAN16(x)    ((((x) << 8) & 0xff00) | (((x) >> 8) & 255))

__inline static td_bool is_little_end(void)
{
    union end_test_u {
        td_char test[4];
        td_u32 test_full;
    } end_test;

    end_test.test[0] = 0x01;
    end_test.test[1] = 0x02;
    end_test.test[2] = 0x03;
    end_test.test[3] = 0x04;

    return (end_test.test_full > 0x01020304) ? (TD_TRUE) : (TD_FALSE);
}

#define address_out_32bit(addr, len) ((addr >= 0x100000000) || ((addr + len) >= 0x100000000))

/*
 * rgb_to_yc(r, g, b, *y, *u, *u)    convert r,g,b to y,u,v
 * rgb_to_yuv(rgb)             convert rgb to yuv
 * rgbfull to yuv601limit
 */
__inline static td_void rgb_to_yc(td_u16 r, td_u16 g, td_u16 b, td_u16 *y, td_u16 *cb, td_u16 *cr)
{
    /* Y */
    *y = (td_u16)((((r * 66 + g * 129 + b * 25) >> 8) + 16) << 2);

    /* cb */
    *cb = (td_u16)(((((b * 112 - r * 38) - g * 74) >> 8) + 128) << 2);

    /* cr */
    *cr = (td_u16)(((((r * 112 - g * 94) - b * 18) >> 8) + 128) << 2);
}

__inline static td_u32 rgb_to_yuv(td_u32 rgb)
{
    td_u16 y, u, v;

    rgb_to_yc(RGB_R(rgb), RGB_G(rgb), RGB_B(rgb), &y, &u, &v);

    return YUV(y, u, v);
}


/* rgbfull to yuv601full */
__inline static td_void rgb_to_yc_full(td_u16 r, td_u16 g, td_u16 b, td_u16 *y, td_u16 *cb, td_u16 *cr)
{
    td_u16 y_tmp, cb_tmp, cr_tmp;

    y_tmp = (td_u16)(((r * 76 + g * 150 + b * 29) >> 8) * 4);
    cb_tmp = (td_u16)(clip_min(((((b * 128 - r * 43) - g * 84) >> 8) + 128), 0) * 4);
    cr_tmp = (td_u16)(clip_min(((((r * 128 - g * 107) - b * 20) >> 8) + 128), 0) * 4);

    *y = MAX2(MIN2(y_tmp, 1023), 0);
    *cb = MAX2(MIN2(cb_tmp, 1023), 0);
    *cr = MAX2(MIN2(cr_tmp, 1023), 0);
}

__inline static td_u32 rgb_to_yuv_full(td_u32 rgb)
{
    td_u16 y, u, v;

    rgb_to_yc_full(RGB_R(rgb), RGB_G(rgb), RGB_B(rgb), &y, &u, &v);

    return YUV(y, u, v);
}

/*
 * rgb_to_yc_8bit(r, g, b, *y, *u, *u)    convert r,g,b to y,u,v
 * rgb_to_yuv_8bit(rgb)                   convert rgb to yuv
 * rgbfull to yuv601limit
 */
__inline static td_void rgb_to_yc_8bit(td_u8 r, td_u8 g, td_u8 b, td_u8 *y, td_u8 *cb, td_u8 *cr)
{
    /* Y */
    *y = (td_u8)(((r * 66 + g * 129 + b * 25) >> 8) + 16);

    /* cb */
    *cb = (td_u8)((((b * 112 - r * 38) - g * 74) >> 8) + 128);

    /* cr */
    *cr = (td_u8)((((r * 112 - g * 94) - b * 18) >> 8) + 128);
}

__inline static td_u32 rgb_to_yuv_8bit(td_u32 rgb)
{
    td_u8 y, u, v;

    rgb_to_yc_8bit(RGB_R(rgb), RGB_G(rgb), RGB_B(rgb), &y, &u, &v);

    return YUV_8BIT(y, u, v);
}

/* rgbfull to yuv601full */
__inline static td_void rgb_to_yc_full_8bit(td_u8 r, td_u8 g, td_u8 b, td_u8 *y, td_u8 *cb, td_u8 *cr)
{
    td_s16 y_tmp, cb_tmp, cr_tmp;

    y_tmp = (r * 76 + g * 150 + b * 29) >> 8;
    cb_tmp = (((b * 128 - r * 43) - g * 84) >> 8) + 128;
    cr_tmp = (((r * 128 - g * 107) - b * 20) >> 8) + 128;

    *y = MAX2(MIN2(y_tmp, 255), 0);
    *cb = MAX2(MIN2(cb_tmp, 255), 0);
    *cr = MAX2(MIN2(cr_tmp, 255), 0);
}

__inline static td_u32 rgb2_yuv_full_8_bit(td_u32 rgb)
{
    td_u8 y, u, v;

    rgb_to_yc_full_8bit(RGB_R(rgb), RGB_G(rgb), RGB_B(rgb), &y, &u, &v);

    return YUV_8BIT(y, u, v);
}

/*
 * fps_control using sample:
 *  fps_ctrl g_fps_ctrl;
 *
 *  take 12 frame uniform in 25.
 *  usage: init_fps(&g_fps_ctrl, 25, 12);
 *
 *  {
 *       if(fps_control(&g_fps_ctrl)) printf("yes, this frame should be token");
 *  }
 */
typedef struct {
    td_u32 full_fps; /* full frame rate    */
    td_u32 target_fps; /* target frame rate  */
    td_u32 frame_key; /* update key frame   */
} fps_ctrl;

__inline static td_void init_fps(fps_ctrl *frame_ctrl, td_u32 full_fps, td_u32 tag_fps)
{
    frame_ctrl->full_fps = full_fps;
    frame_ctrl->target_fps = tag_fps;
    frame_ctrl->frame_key = 0;
}

__inline static td_bool do_fps_ctrl(fps_ctrl *frame_ctrl)
{
    td_bool ret = TD_FALSE;

    frame_ctrl->frame_key += frame_ctrl->target_fps;
    if (frame_ctrl->frame_key >= frame_ctrl->full_fps) {
        frame_ctrl->frame_key -= frame_ctrl->full_fps;
        ret = TD_TRUE;
    }

    return ret;
}

__inline static td_u32 get_low_addr(td_phys_addr_t phys_addr)
{
    return (td_u32)phys_addr;
}

#ifdef CONFIG_PHYS_ADDR_BIT_WIDTH_64
__inline static td_u32 get_high_addr(td_phys_addr_t phys_addr)
{
    return (td_u32)(phys_addr >> 32); /* 32bit low addr */
}
#else
__inline static td_u32 get_high_addr(td_phys_addr_t phys_addr)
{
    return 0;
}
#endif

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __OT_MATH_H__ */

