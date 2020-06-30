/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2012-2018. All rights reserved.
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

    rgb2_yc(RGB_R(rgb), RGB_G(rgb), RGB_B(rgb), &y, &u, &v);

    return YUV(y, u, v);
}

__inline static hi_void rgb2_yc_full(hi_u16 r, hi_u16 g, hi_u16 b, hi_u16 *py, hi_u16 *pcb, hi_u16 *pcr)
{
    hi_u16 py_temp, pcb_temp, pcr_temp;

    py_temp = (hi_u16)(((r * 76 + g * 150 + b * 29) >> 8) * 4);
    pcb_temp = (hi_u16)(CLIP_MIN(((((b * 130 - r * 44) - g * 86) >> 8) + 128), 0) * 4);
    pcr_temp = (hi_u16)(CLIP_MIN(((((r * 130 - g * 109) - b * 21) >> 8) + 128), 0) * 4);

    *py = MAX2(MIN2(py_temp, 1023), 0);
    *pcb = MAX2(MIN2(pcb_temp, 1023), 0);
    *pcr = MAX2(MIN2(pcr_temp, 1023), 0);
}

__inline static hi_u32 rgb2_yuv_full(hi_u32 rgb)
{
    hi_u16 y, u, v;

    rgb2_yc_full(RGB_R(rgb), RGB_G(rgb), RGB_B(rgb), &y, &u, &v);

    return YUV(y, u, v);
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

    rgb2_yc_8_bit(RGB_R(rgb), RGB_G(rgb), RGB_B(rgb), &y, &u, &v);

    return YUV_8BIT(y, u, v);
}

__inline static hi_void rgb2_yc_full_8_bit(hi_u8 r, hi_u8 g, hi_u8 b, hi_u8 *py, hi_u8 *pcb, hi_u8 *pcr)
{
    hi_s16 py_temp, pcb_temp, pcr_temp;

    py_temp = (r * 76 + g * 150 + b * 29) >> 8;
    pcb_temp = (((b * 130 - r * 44) - g * 86) >> 8) + 128;
    pcr_temp = (((r * 130 - g * 109) - b * 21) >> 8) + 128;

    *py = MAX2(MIN2(py_temp, 255), 0);
    *pcb = MAX2(MIN2(pcb_temp, 255), 0);
    *pcr = MAX2(MIN2(pcr_temp, 255), 0);
}

__inline static hi_u32 rgb2_yuv_full_8_bit(hi_u32 rgb)
{
    hi_u8 y, u, v;

    rgb2_yc_full_8_bit(RGB_R(rgb), RGB_G(rgb), RGB_B(rgb), &y, &u, &v);

    return YUV_8BIT(y, u, v);
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

