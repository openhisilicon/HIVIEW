/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */
#ifndef OT_COMMON_GDC_H
#define OT_COMMON_GDC_H

#include "ot_type.h"
#include "ot_common.h"
#include "ot_errno.h"
#include "ot_common_video.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

/* failure caused by malloc buffer */
#define OT_ERR_GDC_NO_BUF           OT_DEFINE_ERR(OT_ID_GDC, OT_ERR_LEVEL_ERROR, OT_ERR_NO_BUF)
#define OT_ERR_GDC_BUF_EMPTY        OT_DEFINE_ERR(OT_ID_GDC, OT_ERR_LEVEL_ERROR, OT_ERR_BUF_EMPTY)
#define OT_ERR_GDC_NULL_PTR         OT_DEFINE_ERR(OT_ID_GDC, OT_ERR_LEVEL_ERROR, OT_ERR_NULL_PTR)
#define OT_ERR_GDC_ILLEGAL_PARAM    OT_DEFINE_ERR(OT_ID_GDC, OT_ERR_LEVEL_ERROR, OT_ERR_ILLEGAL_PARAM)
#define OT_ERR_GDC_BUF_FULL         OT_DEFINE_ERR(OT_ID_GDC, OT_ERR_LEVEL_ERROR, OT_ERR_BUF_FULL)
#define OT_ERR_GDC_NOT_READY        OT_DEFINE_ERR(OT_ID_GDC, OT_ERR_LEVEL_ERROR, OT_ERR_NOT_READY)
#define OT_ERR_GDC_NOT_SUPPORT      OT_DEFINE_ERR(OT_ID_GDC, OT_ERR_LEVEL_ERROR, OT_ERR_NOT_SUPPORT)
#define OT_ERR_GDC_NOT_PERM         OT_DEFINE_ERR(OT_ID_GDC, OT_ERR_LEVEL_ERROR, OT_ERR_NOT_PERM)
#define OT_ERR_GDC_BUSY             OT_DEFINE_ERR(OT_ID_GDC, OT_ERR_LEVEL_ERROR, OT_ERR_BUSY)
#define OT_ERR_GDC_INVALID_CHN_ID   OT_DEFINE_ERR(OT_ID_GDC, OT_ERR_LEVEL_ERROR, OT_ERR_INVALID_CHN_ID)
#define OT_ERR_GDC_CHN_UNEXIST      OT_DEFINE_ERR(OT_ID_GDC, OT_ERR_LEVEL_ERROR, OT_ERR_UNEXIST)

#define OT_GDC_PRIVATE_DATA_NUM         4
#define OT_GDC_PMF_COEF_NUM             9

#define OT_FISHEYE_MAX_RGN_NUM          4
#define OT_FISHEYE_LMF_COEF_NUM         128

typedef td_u32 ot_gdc_handle;

typedef struct {
    ot_video_frame_info img_in; /* input picture */
    ot_video_frame_info img_out; /* output picture */
    td_u64 private_data[OT_GDC_PRIVATE_DATA_NUM]; /* RW; private data of task */
    td_u64 reserved; /* RW; debug information,state of current picture */
} ot_gdc_task_attr;

/* mount mode of device */
typedef enum {
    OT_FISHEYE_MOUNT_MODE_DESKTOP = 0, /* Desktop mount mode */
    OT_FISHEYE_MOUNT_MODE_CEILING = 1, /* Ceiling mount mode */
    OT_FISHEYE_MOUNT_MODE_WALL = 2, /* wall mount mode */

    OT_FISHEYE_MOUNT_MODE_BUTT
} ot_fisheye_mount_mode;

/* view mode of client */
typedef enum {
    OT_FISHEYE_VIEW_MODE_360_PANORAMA = 0, /* 360 panorama mode of gdc correction */
    OT_FISHEYE_VIEW_MODE_180_PANORAMA = 1, /* 180 panorama mode of gdc correction */
    OT_FISHEYE_VIEW_MODE_NORM = 2, /* normal mode of gdc correction */
    OT_FISHEYE_VIEW_MODE_NO_TRANS = 3, /* no gdc correction */

    OT_FISHEYE_VIEW_MODE_BUTT
} ot_fisheye_view_mode;

/* fisheye region correction attribute */
typedef struct {
    ot_fisheye_view_mode view_mode; /* RW; range: [0, 3];gdc view mode */
    td_u32 in_radius; /* RW; inner radius of gdc correction region */
    td_u32 out_radius; /* RW; out radius of gdc correction region */
    td_u32 pan; /* RW; range: [0, 360] */
    td_u32 tilt; /* RW; range: [0, 360] */
    td_u32 hor_zoom; /* RW; Range: [1, 5265]; horizontal zoom of correction region */
    td_u32 ver_zoom; /* RW; Range: [1, 4095]; vertical zoom of correction region */
    ot_rect out_rect; /* RW; out imge rectangle attribute */
} ot_fisheye_rgn_attr;

typedef struct {
    ot_fisheye_view_mode view_mode; /* RW; range: [0, 3];gdc view mode */
    td_u32 in_radius; /* RW; inner radius of gdc correction region */
    td_u32 out_radius; /* RW; out radius of gdc correction region */
    td_u32 x; /* RW; Range: [0, 8192); x-coordinate of the centre point of correction region */
    td_u32 y; /* RW; Range: [0, 8192); y-coordinate of the centre point of correction region */
    td_u32 hor_zoom; /* RW; Range: [1, 4095]; horizontal zoom of correction region */
    td_u32 ver_zoom; /* RW; Range: [1, 4095]; vertical zoom of correction region */
    ot_rect out_rect; /* RW; out imge rectangle attribute */
} ot_fisheye_rgn_attr_ex;

/* fisheye all regions correction attribute */
typedef struct {
     /*
      * RW; range: [0, 1];
      * whether gdc len's LMF coefficient is from user config or from default linear config
      */
    td_bool lmf_en;
    td_bool bg_color_en; /* RW; range: [0, 1];whether use background color or not */
    td_u32  bg_color; /* RW; range: [0,0xffffff];the background color RGB888 */

    /*
     * RW; range: [-511, 511];
     * the horizontal offset between image center and physical center of len
     */
    td_s32  hor_offset;
    /*
     * RW; range: [-511, 511];
     * the vertical offset between image center and physical center of len
     */
    td_s32  ver_offset;

    td_u32  trapezoid_coef; /* RW; range: [0, 32];strength coefficient of trapezoid correction */
    td_s32  fan_strength; /* RW; range: [-760, 760];strength coefficient of fan correction */

    ot_fisheye_mount_mode mount_mode; /* RW; range: [0, 2];gdc mount mode */

    td_u32 rgn_num; /* RW; range: [1, 4]; gdc correction region number */
    ot_fisheye_rgn_attr fisheye_rgn_attr[OT_FISHEYE_MAX_RGN_NUM]; /* RW; attribution of gdc correction region */
} ot_fisheye_attr;

typedef struct {
     /*
      * RW; range: [0, 1];
      * whether gdc len's LMF coefficient is from user config or from default linear config
      */
    td_bool lmf_en;
    td_bool bg_color_en; /* RW; range: [0, 1];whether use background color or not */
    td_u32 bg_color; /* RW; range: [0,0xffffff];the background color RGB888 */

    /*
     * RW; range: [-511, 511];
     * the horizontal offset between image center and physical center of len
     */
    td_s32 hor_offset;
    /*
     * RW; range: [-511, 511];
     * the vertical offset between image center and physical center of len
     */
    td_s32 ver_offset;

    td_u32 trapezoid_coef; /* RW; range: [0, 32];strength coefficient of trapezoid correction */
    td_s32 fan_strength; /* RW; range: [-760, 760];strength coefficient of fan correction */

    ot_fisheye_mount_mode mount_mode; /* RW; range: [0, 2];gdc mount mode */

    td_u32 rgn_num; /* RW; range: [1, 4]; gdc correction region number */
    ot_fisheye_rgn_attr_ex fisheye_rgn_attr[OT_FISHEYE_MAX_RGN_NUM]; /* RW; attribution of gdc correction region */
} ot_fisheye_attr_ex;

/* fisheye correction attribute in channel */
typedef struct {
    td_bool enable; /* RW; range: [0, 1]; */
    ot_fisheye_attr fisheye_attr;
    ot_size dst_size; /* RW; range: width:[640, 8192],height:[360, 8192]; */
} ot_fisheye_correction_attr;

/* fisheye config */
typedef struct {
    td_u16 lmf_coef[OT_FISHEYE_LMF_COEF_NUM]; /* RW;  LMF coefficient of gdc len */
} ot_fisheye_cfg;

/* fisheye job config */
typedef struct {
    td_phys_addr_t len_map_phys_addr; /* LMF coefficient physic addr */
} ot_gdc_fisheye_job_cfg;

/* gdc PMF attr */
typedef struct {
    td_s64 pmf_coef[OT_GDC_PMF_COEF_NUM]; /* W;  PMF coefficient of gdc */
} ot_gdc_pmf_attr;

/* Gdc FISHEYE POINT QUERY Attr */
typedef struct {
    td_u32 rgn_index;
    ot_fisheye_attr *fisheye_attr;
    td_u16 lmf_coef[OT_FISHEYE_LMF_COEF_NUM];
} ot_gdc_fisheye_point_query_attr;

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* OT_COMMON_GDC_H */
