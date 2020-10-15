/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2016-2018. All rights reserved.
 * Description:
 * Author:
 * Create: 2016-09-19
 */

#ifndef __HI_MATH_ADAPT_H__
#define __HI_MATH_ADAPT_H__

#include "hi_type.h"
#include "hi_math.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

/******************************************************************************
** hi_abs(x)                 absolute value of x
** sign(x)                sign of x
** cmp(x,y)               0 if x==y; 1 if x>y; -1 if x<y
******************************************************************************/
#define hi_abs(x)          ((x) >= 0 ? (x) : (-(x)))
#define _sign(x)         ( (x) >= 0 ? 1 : -1 )
#define cmp(x,y)        (((x) == (y)) ? 0 : (((x) > (y)) ? 1 : -1))

/******************************************************************************
** max2(x,y)              maximum of x and y
** min2(x,y)              minimum of x and y
** max3(x,y,z)            maximum of x, y and z
** min3(x,y,z)            minimun of x, y and z
** median(x,y,z)          median of x,y,z
** mean2(x,y)             mean of x,y
******************************************************************************/
#define max2(x,y)       ( (x)>(y) ? (x):(y) )
#define min2(x,y)       ( (x)<(y) ? (x):(y) )
#define max3(x,y,z)     ( (x)>(y) ? max2(x,z) : max2(y,z) )
#define min3(x,y,z)     ( (x)<(y) ? min2(x,z) : min2(y,z) )
#define median(x,y,z)   (((x)+(y)+(z) - max3(x,y,z)) - min3(x,y,z) )
#define mean2(x,y)      (((x)+(y)) >> 1 )

/******************************************************************************
** clip3(x,min,max)       clip x within [min,max]
** wrap_max(x,max,min)    wrap to min if x equal max
** wrap_min(x,min,max)    wrap to max if x equal min
** value_between(x,min.max)   True if x is between [min,max] inclusively.
******************************************************************************/
#define clip_min(x,min)          (((x) >= min) ? (x) : min)
#define clip3(x,min,max)         ( (x)< (min) ? (min) : ((x)>(max)?(max):(x)) )
#define clip_max(x,max)          ((x)>(max)?(max):(x))
#define wrap_max(x,max,min)      ( (x)>=(max) ? (min) : (x) )
#define wrap_min(x,min,max)      ( (x)<=(min) ? (max) : (x) )
#define value_between(x,min,max) (((x)>=(min)) && ((x) <= (max)))

/******************************************************************************
** multi_of_2_power(x,a)  whether x is multiple of a(a must be power of 2)
** align_down(x,a)     floor x to multiple of a(a must be power of 2)
** align_up(x, a)            align x to multiple of a
**
** Example:
** align_up(5,4) = 8
** align_down(5,4)   = 4
******************************************************************************/
#define multi_of_2_power(x,a)    (!((x) & ((a) - 1) ) )
#define hiceiling(x, a)           (((x)+(a)-1)/(a))

#define align_up(x, a)           ( ( ((x) + ((a) - 1) ) / (a) ) * (a) )
#define align_down(x, a)         ( ( (x) / (a)) * (a) )

#define div_up(x, a)             ( ((x) + ((a) - 1) ) / (a) )

/******************************************************************************
** Get the span between two unsinged number, such as
** span(hi_u32, 100, 200) is 200 - 100 = 100
** span(hi_u32, 200, 100) is 0xFFFFFFFF - 200 + 100
** span(hi_u64, 200, 100) is 0xFFFFFFFFFFFFFFFF - 200 + 100
******************************************************************************/
#define span(type, begin, end)\
({                            \
   type b = (begin);          \
   type e = (end);            \
   (type)((b >= e) ? (b - e) : (b + ((~((type)0))-e))); \
})

/******************************************************************************
** endian32(x,y)              little endian <---> big endian
******************************************************************************/
#define  endian32( x )                        \
    (   ( (x) << 24 ) |                        \
        ( ( (x) & 0x0000ff00 ) << 8 ) |        \
        ( ( (x) & 0x00ff0000 ) >> 8 ) |        \
        ( ( (x) >> 24 ) & 0x000000ff )  )



/******************************************************************************
** endian16(x,y)              little endian <---> big endian
** is_little_end()            whether the system is little end mode
******************************************************************************/
#define  endian16( x )    ( (((x) << 8) & 0xff00) | (((x) >> 8) & 255) )
__inline static hi_bool is_little_end(void)
{
    union un_end_test_u {
        hi_char test[4];
        hi_u32 test_full;
    } un_end_test;

    un_end_test.test[0] = 0x01;
    un_end_test.test[1] = 0x02;
    un_end_test.test[2] = 0x03;
    un_end_test.test[3] = 0x04;

    return (un_end_test.test_full > 0x01020304) ? (HI_TRUE) : (HI_FALSE);
}

/******************************************************************************
** fraction32(de,nu)          fraction: nu(minator) / de(nominator).
** numerator32(x)              of x(x is fraction)
** denominator32(x)           Denominator of x(x is fraction)

** represent fraction in 32 bit. LSB 16 is numerator, MSB 16 is denominator
** It is integer if denominator is 0.
******************************************************************************/
#define fraction32(de,nu)       ( ((de) << 16) | (nu) )
#define numerator32(x)          ( (x) & 0xffff)
#define denominator32(x)        ( (x) >> 16 )

/******************************************************************************
** rgb(r,g,b)    assemble the r,g,b to 24bit color
** rgb_r(c)      get RED   from 24bit color
** rgb_g(c)      get GREEN from 24bit color
** rgb_b(c)      get BLUE  from 24bit color
******************************************************************************/
#define rgb(r,g,b) ((((r) & 0xff) << 16) | (((g) & 0xff) << 8) | ((b) & 0xff))
#define rgb_r(c)   ( ((c) & 0xff0000) >> 16)
#define rgb_g(c)   ( ((c) & 0xff00) >> 8)
#define rgb_b(c)   (  (c) & 0xff)

/******************************************************************************
** yuv(y,u,v)    assemble the y,u,v to 30bit color
** yuv_y(c)      get Y from 30bit color
** yuv_u(c)      get U from 30bit color
** yuv_v(c)      get V from 30bit color
******************************************************************************/
#define yuv(y,u,v) ((((y) & 0x03ff) << 20) | (((u) & 0x03ff) << 10) | ((v) & 0x03ff))
#define yuv_y(c)   ( ((c) & 0x3ff00000) >> 20)
#define yuv_u(c)   ( ((c) & 0x000ffc00) >> 10)
#define yuv_v(c)   (  (c) & 0x000003ff)

/******************************************************************************
** yuv_8bit(y,u,v)    assemble the y,u,v to 24bit color
** yuv_8bit_y(c)      get Y from 24bit color
** yuv_8bit_u(c)      get U from 24bit color
** yuv_8bit_v(c)      get V from 24bit color
******************************************************************************/
#define yuv_8bit(y,u,v) ((((y) & 0xff) << 16) | (((u) & 0xff) << 8) | ((v) & 0xff))
#define yuv_8bit_y(c)   ( ((c) & 0xff0000) >> 16)
#define yuv_8bit_u(c)   ( ((c) & 0xff00) >> 8)
#define yuv_8bit_v(c)   (  (c) & 0xff)
/******************************************************************************
** rgb2_yc(r, g, b, *y, *u, *u)    convert r,g,b to y,u,v
** rgb2_yuv(rgb)             convert rgb to yuv
******************************************************************************/
__inline static hi_void rgb2_yc(hi_u16 r, hi_u16 g, hi_u16 b, hi_u16 *py, hi_u16 *pcb, hi_u16 *pcr)
{
    /* Y */
    *py = (hi_u16)((((r * 66 + g * 129 + b * 25) >> 8) + 16) << 2);

    /* cb */
    *pcb = (hi_u16)(((((b * 112 - r * 38) - g * 74) >> 8) + 128) << 2);

    /* cr */
    *pcr = (hi_u16)(((((r * 112 - g * 94) - b * 18) >> 8) + 128) << 2);
}

__inline static hi_u32 rgb2_yuv(hi_u32 rgb)
{
    hi_u16 y, u, v;

    rgb2_yc(rgb_r(rgb), rgb_g(rgb), rgb_b(rgb), &y, &u, &v);

    return yuv(y, u, v);
}

__inline static hi_void rgb2_yc_full(hi_u16 r, hi_u16 g, hi_u16 b, hi_u16 *py, hi_u16 *pcb, hi_u16 *pcr)
{
    hi_u16 py_temp, pcb_temp, pcr_temp;

    py_temp = (hi_u16)(((r * 76 + g * 150 + b * 29) >> 8) * 4);
    pcb_temp = (hi_u16)(clip_min(((((b * 130 - r * 44) - g * 86) >> 8) + 128), 0) * 4);
    pcr_temp = (hi_u16)(clip_min(((((r * 130 - g * 109) - b * 21) >> 8) + 128), 0) * 4);

    *py = max2(min2(py_temp, 1023), 0);
    *pcb = max2(min2(pcb_temp, 1023), 0);
    *pcr = max2(min2(pcr_temp, 1023), 0);
}

__inline static hi_u32 rgb2_yuv_full(hi_u32 rgb)
{
    hi_u16 y, u, v;

    rgb2_yc_full(rgb_r(rgb), rgb_g(rgb), rgb_b(rgb), &y, &u, &v);

    return yuv(y, u, v);
}

/******************************************************************************
** rgb2_yc_8_bit(r, g, b, *y, *u, *u)    convert r,g,b to y,u,v
** rgb2_yuv_8_bit(rgb)                   convert rgb to yuv
******************************************************************************/
__inline static hi_void rgb2_yc_8_bit(hi_u8 r, hi_u8 g, hi_u8 b, hi_u8 *py, hi_u8 *pcb, hi_u8 *pcr)
{
    /* Y */
    *py = (hi_u8)(((r * 66 + g * 129 + b * 25) >> 8) + 16);

    /* cb */
    *pcb = (hi_u8)((((b * 112 - r * 38) - g * 74) >> 8) + 128);

    /* cr */
    *pcr = (hi_u8)((((r * 112 - g * 94) - b * 18) >> 8) + 128);
}

__inline static hi_u32 rgb2_yuv_8_bit(hi_u32 rgb)
{
    hi_u8 y, u, v;

    rgb2_yc_8_bit(rgb_r(rgb), rgb_g(rgb), rgb_b(rgb), &y, &u, &v);

    return yuv_8bit(y, u, v);
}

__inline static hi_void rgb2_yc_full_8_bit(hi_u8 r, hi_u8 g, hi_u8 b, hi_u8 *py, hi_u8 *pcb, hi_u8 *pcr)
{
    hi_s16 py_temp, pcb_temp, pcr_temp;

    py_temp = (r * 76 + g * 150 + b * 29) >> 8;
    pcb_temp = (((b * 130 - r * 44) - g * 86) >> 8) + 128;
    pcr_temp = (((r * 130 - g * 109) - b * 21) >> 8) + 128;

    *py = max2(min2(py_temp, 255), 0);
    *pcb = max2(min2(pcb_temp, 255), 0);
    *pcr = max2(min2(pcr_temp, 255), 0);
}

__inline static hi_u32 rgb2_yuv_full_8_bit(hi_u32 rgb)
{
    hi_u8 y, u, v;

    rgb2_yc_full_8_bit(rgb_r(rgb), rgb_g(rgb), rgb_b(rgb), &y, &u, &v);

    return yuv_8bit(y, u, v);
}

/*******************************************************************************
** fps_control useing sample:
**  fps_ctrl g_fps_ctrl;
**
**  take 12 frame uniform in 25.
**  init_fps(&g_fps_ctrl, 25, 12);
**
**  {
**       if(fps_control(&g_fps_ctrl)) printf("yes, this frmae should be token");
**  }
**
*******************************************************************************/
typedef struct {
    hi_u32 ffps; /* full frame rate    */
    hi_u32 tfps; /* target frame rate  */
    hi_u32 frm_key; /* update key frame   */
} fps_ctrl;

__inline static hi_void init_fps(fps_ctrl *frm_ctrl, hi_u32 full_fps,
         hi_u32 tag_fps)
{
    frm_ctrl->ffps   = full_fps;
    frm_ctrl->tfps   = tag_fps;
    frm_ctrl->frm_key = 0;
}

__inline static hi_bool fps_control(fps_ctrl *frm_ctrl)
{
    hi_bool ret = HI_FALSE;

    frm_ctrl->frm_key += frm_ctrl->tfps;
    if (frm_ctrl->frm_key >= frm_ctrl->ffps) {
        frm_ctrl->frm_key -= frm_ctrl->ffps;
        ret = HI_TRUE;
    }

    return ret;
}

__inline static hi_u32 get_low_addr(hi_u64 phyaddr)
{
    return (hi_u32)phyaddr;
}

__inline static hi_u32 get_high_addr(hi_u64 phyaddr)
{
    return (hi_u32)(phyaddr >> 32);
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __HI_ADAPT_MATH_H__ */

