/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2019. All rights reserved.
 * Description:Common defination  struct of region
 * Author: Hisilicon multimedia software group
 * Create: 2019/06/15
 */

#ifndef __HI_COMMON_REGION_H__
#define __HI_COMMON_REGION_H__

#include "hi_common.h"
#include "hi_common_video.h"
#include "hi_errno.h"
#include "hi_defines.h"


#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* End of #ifdef __cplusplus */

#define HI_RGN_MAX_BMP_UPDATE_NUM      16
#define HI_RGN_BATCH_HANDLE_MAX        24
#define HI_RGN_QUAD_POINT_NUM          4
#define HI_RGN_LINE_POINT_NUM          2
#define HI_RGN_CLUT_NUM                16
typedef hi_u32 hi_rgn_handle;
typedef hi_u32 hi_rgn_handle_grp;

/* type of video regions */
typedef enum {
    HI_RGN_OVERLAY = 0,
    HI_RGN_COVER,
    HI_RGN_OVERLAYEX,
    HI_RGN_COVEREX,
    HI_RGN_LINE,
    HI_RGN_MOSAIC,
    HI_RGN_MOSAICEX,
    HI_RGN_CORNER_RECTEX,
    HI_RGN_BUTT
} hi_rgn_type;

typedef enum  {
    HI_RGN_ATTACH_JPEG_MAIN = 0,
    HI_RGN_ATTACH_JPEG_MPF0,
    HI_RGN_ATTACH_JPEG_MPF1,
    HI_RGN_ATTACH_JPEG_BUTT
} hi_rgn_attach_dst;

typedef struct {
    hi_bool        enable;
    hi_bool        is_abs_qp;
    hi_s32         qp_val;
} hi_rgn_overlay_qp_info;

typedef struct {
    hi_pixel_format        pixel_format;    /* bitmap pixel format now support clut2 and clut4 */
    hi_u32                 bg_color;    /* background color, pixel format depends on "pixel_format" */
    /*
     * region size, W:[2, HI_RGN_OVERLAY_MAX_WIDTH], align:2,
     * H:[2, HI_RGN_OVERLAY_MAX_HEIGHT], align:2
     */
    hi_size                size;
    hi_u32                 canvas_num;
    hi_u32                 clut[HI_RGN_CLUT_NUM];      /* color look up table */
} hi_rgn_overlay_attr;

typedef struct {
    hi_point                       point;    /* X:[0, OVERLAY_MAX_X_VENC], align:2, Y:[0,OVERLAY_MAX_Y_VENC], align:2 */
    hi_u32                         fg_alpha;
    hi_u32                         bg_alpha;
    hi_u32                         layer;     /* OVERLAY region layer range:[0,7] */
    hi_rgn_overlay_qp_info         qp_info;
    hi_rgn_attach_dst              dst;
} hi_rgn_overlay_chn_attr;

typedef struct {
    hi_pixel_format                pixel_format;
    hi_u32                         bg_color;    /* background color, pixel format depends on "pixel_format" */
    /*
     * region size, W:[2, HI_RGN_OVERLAY_MAX_WIDTH], align:2,
     * H:[2, HI_RGN_OVERLAY_MAX_HEIGHT], align:2
     */
    hi_size                        size;
    hi_u32                         canvas_num;
    hi_u32                         clut[HI_RGN_CLUT_NUM];
} hi_rgn_overlayex_attr;

typedef struct {
    /*
     * X:[0, HI_RGN_OVERLAY_MAX_X], align:2,
     * Y:[0, HI_RGN_OVERLAY_MAX_Y], align:2
     */
    hi_point                       point;
    hi_u32                         fg_alpha;
    hi_u32                         bg_alpha;
    hi_u32                         layer;     /* OVERLAYEX region layer range:[0,15] */
} hi_rgn_overlayex_chn_attr;

typedef struct {
    hi_cover                       cover;
    hi_u32                         layer;      /* COVER region layer */
    hi_coord                       coord;  /* ratio coordiante or abs coord */
} hi_rgn_cover_chn_attr;

typedef struct {
    hi_cover                       coverex;
    hi_u32                         layer; /* COVEREX region layer range */
    hi_coord                       coord;  /* ratio coordiante or abs coord */
} hi_rgn_coverex_chn_attr;


typedef struct {
    hi_rect                        rect;              /* location of MOSAIC */
    hi_mosaic_blk_size             blk_size;          /* block size of MOSAIC */
    hi_u32                         layer;             /* MOSAIC region layer range:[0,3] */
} hi_rgn_mosaic_chn_attr;

typedef struct {
    hi_rect                        rect;              /* location of MOSAICEX */
    hi_mosaic_blk_size             blk_size;          /* block size of MOSAIC EX */
    hi_u32                         layer;             /* MOSAICEX region layer range:[0,3] */
} hi_rgn_mosaicex_chn_attr;

typedef union {
    hi_rgn_overlay_attr           overlay; /* attribute of overlay region. AUTO:hi_rgn_type:HI_RGN_OVERLAY; */
    hi_rgn_overlayex_attr         overlayex; /* attribute of overlayex region. AUTO:hi_rgn_type:HI_RGN_OVERLAYEX; */
} hi_rgn_type_attr;

typedef struct {
    hi_u32                         thick;                        /* width of line range[0,7] */
    hi_u32                         color;                        /* color of line */
    hi_point                       points[HI_RGN_LINE_POINT_NUM];    /* startpoint and endpoint of line */
} hi_rgn_line_chn_attr;

typedef struct {
    hi_corner_rect                 corner_rect;         /* location and size of conrer rect */
    hi_corner_rect_attr            corner_rect_attr;    /* attribute of corner rect region */
    hi_u32                         layer;               /* corner_rect region layer */
} hi_rgn_corner_rect_chn_attr;

typedef union {
    hi_rgn_overlay_chn_attr     overlay_chn; /* attribute of overlay region. AUTO:hi_rgn_type:HI_RGN_OVERLAY; */
    hi_rgn_cover_chn_attr       cover_chn; /* attribute of cover region. AUTO:hi_rgn_type: HI_RGN_COVER; */
    hi_rgn_overlayex_chn_attr   overlayex_chn; /* attribute of overlayex region. AUTO:hi_rgn_type: HI_RGN_OVERLAYEX; */
    hi_rgn_coverex_chn_attr     coverex_chn; /* attribute of coverex region. AUTO:hi_rgn_type:HI_RGN_COVEREX; */
    hi_rgn_line_chn_attr        line_chn; /* attribute of line region. AUTO:hi_rgn_type:HI_RGN_LINE; */
    hi_rgn_mosaic_chn_attr      mosaic_chn; /* attribute of mosic region. AUTO:hi_rgn_type:HI_RGN_MOSAIC; */
    hi_rgn_mosaicex_chn_attr    mosaicex_chn; /* attribute of mosicex region. AUTO:hi_rgn_type:HI_RGN_MOSAICEX; */
    /*
     * attribute of corner rectex region.
     * AUTO:hi_rgn_type:HI_RGN_CORNER_RECTEX;
     */
    hi_rgn_corner_rect_chn_attr corner_rectex_chn;
} hi_rgn_type_chn_attr;

/* attribute of a region */
typedef struct {
    hi_rgn_type                    type; /* region type */
    hi_rgn_type_attr               attr; /* region attribute */
} hi_rgn_attr;

/* attribute of a region */
typedef struct {
    hi_bool                        is_show;
    hi_rgn_type                    type;        /* region type */
    hi_rgn_type_chn_attr           attr;        /* region attribute */
} hi_rgn_chn_attr;

typedef struct {
    hi_phys_addr_t                 phys_addr;
    hi_size                        size;
    hi_u32                         stride;
    hi_pixel_format                pixel_format;
    hi_void  ATTRIBUTE             *virt_addr;
} hi_rgn_canvas_info;

    /* invlalid device ID */
#define HI_ERR_RGN_INVALID_DEV_ID HI_DEFINE_ERR(HI_ID_RGN, HI_ERR_LEVEL_ERROR, HI_ERR_INVALID_DEV_ID)
    /* invlalid channel ID */
#define HI_ERR_RGN_INVALID_CHN_ID HI_DEFINE_ERR(HI_ID_RGN, HI_ERR_LEVEL_ERROR, HI_ERR_INVALID_CHN_ID)
    /* at lease one parameter is illagal ,eg, an illegal enumeration value */
#define HI_ERR_RGN_ILLEGAL_PARAM HI_DEFINE_ERR(HI_ID_RGN, HI_ERR_LEVEL_ERROR, HI_ERR_ILLEGAL_PARAM)
    /* channel exists */
#define HI_ERR_RGN_EXIST HI_DEFINE_ERR(HI_ID_RGN, HI_ERR_LEVEL_ERROR, HI_ERR_EXIST)
    /* UN exist */
#define HI_ERR_RGN_UNEXIST HI_DEFINE_ERR(HI_ID_RGN, HI_ERR_LEVEL_ERROR, HI_ERR_UNEXIST)
    /* using a NULL point */
#define HI_ERR_RGN_NULL_PTR HI_DEFINE_ERR(HI_ID_RGN, HI_ERR_LEVEL_ERROR, HI_ERR_NULL_PTR)
    /* try to enable or initialize system,device or channel, before configing attribute */
#define HI_ERR_RGN_NOT_CFG HI_DEFINE_ERR(HI_ID_RGN, HI_ERR_LEVEL_ERROR, HI_ERR_NOT_CFG)
    /* operation is not supported by NOW */
#define HI_ERR_RGN_NOT_SUPPORT HI_DEFINE_ERR(HI_ID_RGN, HI_ERR_LEVEL_ERROR, HI_ERR_NOT_SUPPORT)
    /* operation is not permitted ,eg, try to change stati attribute */
#define HI_ERR_RGN_NOT_PERM HI_DEFINE_ERR(HI_ID_RGN, HI_ERR_LEVEL_ERROR, HI_ERR_NOT_PERM)
    /* failure caused by malloc memory */
#define HI_ERR_RGN_NO_MEM HI_DEFINE_ERR(HI_ID_RGN, HI_ERR_LEVEL_ERROR, HI_ERR_NO_MEM)
    /* failure caused by malloc buffer */
#define HI_ERR_RGN_NO_BUF HI_DEFINE_ERR(HI_ID_RGN, HI_ERR_LEVEL_ERROR, HI_ERR_NO_BUF)
    /* no data in buffer */
#define HI_ERR_RGN_BUF_EMPTY HI_DEFINE_ERR(HI_ID_RGN, HI_ERR_LEVEL_ERROR, HI_ERR_BUF_EMPTY)
    /* no buffer for new data */
#define HI_ERR_RGN_BUF_FULL HI_DEFINE_ERR(HI_ID_RGN, HI_ERR_LEVEL_ERROR, HI_ERR_BUF_FULL)
    /* bad address, eg. used for copy_from_user & copy_to_user */
#define HI_ERR_RGN_BAD_ADDR HI_DEFINE_ERR(HI_ID_RGN, HI_ERR_LEVEL_ERROR, HI_ERR_BAD_ADDR)
    /* resource is busy, eg. destroy a venc chn without unregistering it */
#define HI_ERR_RGN_BUSY HI_DEFINE_ERR(HI_ID_RGN, HI_ERR_LEVEL_ERROR, HI_ERR_BUSY)
    /*
     * System is not ready,maybe not initialed or loaded.
     * Returning the error code when opening a device file failed.
     */
#define HI_ERR_RGN_NOT_READY HI_DEFINE_ERR(HI_ID_RGN, HI_ERR_LEVEL_ERROR, HI_ERR_NOT_READY)

#ifdef __cplusplus
#if __cplusplus
}

#endif
#endif /* __cplusplus */
#endif /* __HI_COMMON_REGION_H__ */
