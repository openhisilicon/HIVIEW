/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2019-2020. All rights reserved.
 * Description: Vgs mpi interface
 * Author: Hisilicon multimedia software group
 * Create: 2019-07-24
 */

#ifndef __HI_COMMON_VGS_H__
#define __HI_COMMON_VGS_H__

#include "hi_type.h"
#include "hi_common_video.h"

#ifdef __cplusplus
extern "C" {
#endif

#define HI_ERR_VGS_NO_BUF          HI_DEFINE_ERR(HI_ID_VGS, HI_ERR_LEVEL_ERROR, HI_ERR_NO_BUF)
#define HI_ERR_VGS_BUF_EMPTY       HI_DEFINE_ERR(HI_ID_VGS, HI_ERR_LEVEL_ERROR, HI_ERR_BUF_EMPTY)
#define HI_ERR_VGS_NULL_PTR        HI_DEFINE_ERR(HI_ID_VGS, HI_ERR_LEVEL_ERROR, HI_ERR_NULL_PTR)
#define HI_ERR_VGS_ILLEGAL_PARAM   HI_DEFINE_ERR(HI_ID_VGS, HI_ERR_LEVEL_ERROR, HI_ERR_ILLEGAL_PARAM)
#define HI_ERR_VGS_BUF_FULL        HI_DEFINE_ERR(HI_ID_VGS, HI_ERR_LEVEL_ERROR, HI_ERR_BUF_FULL)
#define HI_ERR_VGS_NOT_READY       HI_DEFINE_ERR(HI_ID_VGS, HI_ERR_LEVEL_ERROR, HI_ERR_NOT_READY)
#define HI_ERR_VGS_NOT_SUPPORT     HI_DEFINE_ERR(HI_ID_VGS, HI_ERR_LEVEL_ERROR, HI_ERR_NOT_SUPPORT)
#define HI_ERR_VGS_NOT_PERM        HI_DEFINE_ERR(HI_ID_VGS, HI_ERR_LEVEL_ERROR, HI_ERR_NOT_PERM)

#define HI_VGS_TASK_PRIVATE_DATA_LEN   4
#define HI_VGS_QUAD_POINT_NUM          4
#define HI_VGS_CLUT_LEN                16

typedef hi_s32 hi_vgs_handle;

typedef struct {
    hi_video_frame_info        img_in;               /* RW; Input picture. */
    hi_video_frame_info        img_out;              /* RW; Output picture. */
    hi_u64                     private_data[HI_VGS_TASK_PRIVATE_DATA_LEN];      /* RW; private data of task. */
    hi_u32                     reserved;             /* RW; Debug information,state of current picture. */
} hi_vgs_task_attr;

typedef struct {
    hi_point                   start_point;          /* RW; Line start point. */
    hi_point                   end_point;            /* RW; Line end point. */
    hi_u32                     thick;                /* RW; Width of line. */
    hi_u32                     color;                /* RW; Range: [0,0xFFFFFF]; Color of line. */
} hi_vgs_line;

typedef enum {
    HI_VGS_OSD_INVERTED_COLOR_NONE = 0,              /* Not invert. */
    HI_VGS_OSD_INVERTED_COLOR_RGB,                   /* Invert rgb. */
    HI_VGS_OSD_INVERTED_COLOR_ALPHA,                 /* Invert alpha. */
    HI_VGS_OSD_INVERTED_COLOR_ALL,                   /* Invert rgb and alpha. */
    HI_VGS_OSD_INVERTED_COLOR_BUTT
} hi_vgs_osd_inverted_color;

typedef struct {
    hi_rect                    rect;                      /* RW; Osd area. */
    /*
     * RW; Background color of osd, depends on pixel format of osd,ARGB8888:[0,0xFFFFFFFF],
     * ARGB4444:[0,0xFFFF], ARGB1555:[0,0x1FFF].
     */
    hi_u32                     bg_color;
    hi_pixel_format            pixel_format;              /* RW; Pixel format of osd. */
    hi_phys_addr_t             phys_addr;                 /* RW; Physical address of osd. */
    hi_u32                     stride;                    /* RW; Range: [16,16384]; Stride of osd. */
    hi_u32                     bg_alpha;                  /* RW; Range: [0,255]; Background alpha of osd. */
    hi_u32                     fg_alpha;                  /* RW; Range: [0,255]; Foreground alpha of osd. */
    hi_u32                     clut[HI_VGS_CLUT_LEN];     /* RW; Color look up table data. */
    hi_vgs_osd_inverted_color  osd_inverted_color;        /* RW; Osd inverted color mode. */
} hi_vgs_osd;

typedef enum {
    HI_VGS_SCALE_COEF_NORM = 0,                            /* Normal scale coefficient. */
    HI_VGS_SCALE_COEF_TAP2,                                /* Scale coefficient of 2 tap. */
    HI_VGS_SCALE_COEF_TAP4,                                /* Scale coefficient of 4 tap. */
    HI_VGS_SCALE_COEF_TAP6,                                /* Scale coefficient of 6 tap. */
    HI_VGS_SCALE_COEF_TAP8,                                /* Scale coefficient of 8 tap. */
    HI_VGS_SCALE_COEF_BUTT
} hi_vgs_scale_coef_mode;

#ifdef __cplusplus
}
#endif

#endif /* __HI_COMMON_VGS_H__ */
