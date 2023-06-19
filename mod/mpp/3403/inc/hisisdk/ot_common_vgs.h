/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */

#ifndef OT_COMMON_VGS_H
#define OT_COMMON_VGS_H

#include "ot_type.h"
#include "ot_errno.h"
#include "ot_common_video.h"

#ifdef __cplusplus
extern "C" {
#endif

/* failure caused by malloc buffer */
#define OT_ERR_VGS_NO_BUF          OT_DEFINE_ERR(OT_ID_VGS, OT_ERR_LEVEL_ERROR, OT_ERR_NO_BUF)
#define OT_ERR_VGS_BUF_EMPTY       OT_DEFINE_ERR(OT_ID_VGS, OT_ERR_LEVEL_ERROR, OT_ERR_BUF_EMPTY)
#define OT_ERR_VGS_NULL_PTR        OT_DEFINE_ERR(OT_ID_VGS, OT_ERR_LEVEL_ERROR, OT_ERR_NULL_PTR)
#define OT_ERR_VGS_ILLEGAL_PARAM   OT_DEFINE_ERR(OT_ID_VGS, OT_ERR_LEVEL_ERROR, OT_ERR_ILLEGAL_PARAM)
#define OT_ERR_VGS_BUF_FULL        OT_DEFINE_ERR(OT_ID_VGS, OT_ERR_LEVEL_ERROR, OT_ERR_BUF_FULL)
#define OT_ERR_VGS_NOT_READY       OT_DEFINE_ERR(OT_ID_VGS, OT_ERR_LEVEL_ERROR, OT_ERR_NOT_READY)
#define OT_ERR_VGS_NOT_SUPPORT     OT_DEFINE_ERR(OT_ID_VGS, OT_ERR_LEVEL_ERROR, OT_ERR_NOT_SUPPORT)
#define OT_ERR_VGS_NOT_PERM        OT_DEFINE_ERR(OT_ID_VGS, OT_ERR_LEVEL_ERROR, OT_ERR_NOT_PERM)

#define OT_VGS_TASK_PRIVATE_DATA_LEN   4
#define OT_VGS_QUAD_POINT_NUM          4
#define OT_VGS_CLUT_LEN                16
#define OT_VGS_STITCH_NUM              4

typedef td_s32 ot_vgs_handle;

typedef struct {
    ot_video_frame_info        img_in;               /* RW; Input picture. */
    ot_video_frame_info        img_out;              /* RW; Output picture. */
    td_u64                     private_data[OT_VGS_TASK_PRIVATE_DATA_LEN];      /* RW; private data of task. */
    td_u32                     reserved;             /* RW; Debug information,state of current picture. */
} ot_vgs_task_attr;

typedef struct {
    ot_video_frame_info        img_in[OT_VGS_STITCH_NUM];                       /* RW; Input picture. */
    ot_video_frame_info        img_out;              /* RW; Output picture. */
    td_u64                     private_data[OT_VGS_TASK_PRIVATE_DATA_LEN];      /* RW; private data of task. */
    td_u32                     reserved;             /* RW; Debug information,state of current picture. */
} ot_vgs_stitch_task_attr;

typedef struct {
    ot_point                   start_point;          /* RW; Line start point. */
    ot_point                   end_point;            /* RW; Line end point. */
    td_u32                     thick;                /* RW; Width of line. */
    td_u32                     color;                /* RW; Range: [0,0xFFFFFF]; Color of line. */
} ot_vgs_line;

typedef enum {
    OT_VGS_OSD_INVERTED_COLOR_NONE = 0,              /* Not invert. */
    OT_VGS_OSD_INVERTED_COLOR_RGB,                   /* Invert rgb. */
    OT_VGS_OSD_INVERTED_COLOR_ALPHA,                 /* Invert alpha. */
    OT_VGS_OSD_INVERTED_COLOR_ALL,                   /* Invert rgb and alpha. */
    OT_VGS_OSD_INVERTED_COLOR_BUTT
} ot_vgs_osd_inverted_color;

typedef struct {
    ot_rect                    rect;                      /* RW; Osd area. */
    td_u32                     bg_color;                  /* RW; Reserved */
    ot_pixel_format            pixel_format;              /* RW; Pixel format of osd. */
    td_phys_addr_t             phys_addr;                 /* RW; Physical address of osd. */
    td_u32                     stride;                    /* RW; Range: [16,16384]; Stride of osd. */
    td_u32                     bg_alpha;                  /* RW; Range: [0,255]; Background alpha of osd. */
    td_u32                     fg_alpha;                  /* RW; Range: [0,255]; Foreground alpha of osd. */
    td_u32                     clut[OT_VGS_CLUT_LEN];     /* RW; Color look up table data. */
    ot_vgs_osd_inverted_color  osd_inverted_color;        /* RW; Osd inverted color mode. */
} ot_vgs_osd;

typedef enum {
    OT_VGS_SCALE_COEF_NORM = 0,                            /* Normal scale coefficient. */
    OT_VGS_SCALE_COEF_TAP2,                                /* Scale coefficient of 2 tap. */
    OT_VGS_SCALE_COEF_TAP4,                                /* Scale coefficient of 4 tap. */
    OT_VGS_SCALE_COEF_TAP6,                                /* Scale coefficient of 6 tap. */
    OT_VGS_SCALE_COEF_TAP8,                                /* Scale coefficient of 8 tap. */
    OT_VGS_SCALE_COEF_BUTT
} ot_vgs_scale_coef_mode;

typedef struct {
    td_bool             crop_en;
    ot_rect             crop_rect;
    td_u32              mosaic_num;
    ot_mosaic           mosaic[OT_VGS_MAX_MOSAIC_NUM];
    td_u32              cover_num;
    ot_cover            cover[OT_VGS_MAX_COVER_NUM];
    td_u32              corner_rect_num;
    ot_corner_rect_attr corner_rect_attr;
    ot_corner_rect      corner_rect[OT_VGS_MAX_CORNER_RECT_NUM];
    td_u32              osd_num;
    ot_vgs_osd          osd[OT_VGS_MAX_OSD_NUM];
    td_bool             mirror_en;
    td_bool             flip_en;
    ot_vgs_scale_coef_mode scale_coef_mode;
} ot_vgs_online;

#ifdef __cplusplus
}
#endif

#endif /* OT_COMMON_VGS_H */
