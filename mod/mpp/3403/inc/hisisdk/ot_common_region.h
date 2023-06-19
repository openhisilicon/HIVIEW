/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */

#ifndef __OT_COMMON_REGION_H__
#define __OT_COMMON_REGION_H__

#include "ot_common.h"
#include "ot_common_video.h"
#include "ot_errno.h"
#include "ot_defines.h"

#define OT_RGN_MAX_BMP_UPDATE_NUM      16
#define OT_RGN_BATCH_HANDLE_MAX        24
#define OT_RGN_QUAD_POINT_NUM          4
#define OT_RGN_LINE_POINT_NUM          2
#define OT_RGN_CLUT_NUM                16
typedef td_u32 ot_rgn_handle;
typedef td_u32 ot_rgn_handle_grp;

/* type of video regions */
typedef enum {
    OT_RGN_OVERLAY = 0,
    OT_RGN_COVER,
    OT_RGN_OVERLAYEX,
    OT_RGN_COVEREX,
    OT_RGN_LINE,
    OT_RGN_MOSAIC,
    OT_RGN_MOSAICEX,
    OT_RGN_CORNER_RECTEX,
    OT_RGN_BUTT
} ot_rgn_type;

typedef enum  {
    OT_RGN_ATTACH_JPEG_MAIN = 0,
    OT_RGN_ATTACH_JPEG_MPF0,
    OT_RGN_ATTACH_JPEG_MPF1,
    OT_RGN_ATTACH_JPEG_BUTT
} ot_rgn_attach_dst;

typedef struct {
    td_bool        enable;
    td_bool        is_abs_qp;
    td_s32         qp_val;
} ot_rgn_overlay_qp_info;

typedef struct {
    ot_pixel_format        pixel_format;    /* bitmap pixel format now support clut2 and clut4 */
    td_u32                 bg_color;    /* background color, pixel format depends on "pixel_format" */
    /*
     * region size, width:[2, OT_RGN_OVERLAY_MAX_WIDTH], align:2,
     * height:[2, OT_RGN_OVERLAY_MAX_HEIGHT], align:2
     */
    ot_size                size;
    td_u32                 canvas_num;
    td_u32                 clut[OT_RGN_CLUT_NUM];      /* color look up table */
} ot_rgn_overlay_attr;

typedef struct {
    /*
     * x:[0, OT_RGN_OVERLAY_MAX_X], align:2,
     * y:[0, OT_RGN_OVERLAY_MAX_Y], align:2
     */
    ot_point                       point;
    td_u32                         fg_alpha;
    td_u32                         bg_alpha;
    td_u32                         layer;     /* OVERLAY region layer range depends on OVERLAY max num */
    ot_rgn_overlay_qp_info         qp_info;
    ot_rgn_attach_dst              dst;
} ot_rgn_overlay_chn_attr;

typedef struct {
    ot_pixel_format                pixel_format;
    td_u32                         bg_color;    /* background color, pixel format depends on "pixel_format" */
    /*
     * region size, width:[2, OT_RGN_OVERLAY_MAX_WIDTH], align:2,
     * height:[2, OT_RGN_OVERLAY_MAX_HEIGHT], align:2
     */
    ot_size                        size;
    td_u32                         canvas_num;
    td_u32                         clut[OT_RGN_CLUT_NUM];
} ot_rgn_overlayex_attr;

typedef struct {
    /*
     * x:[0, OT_RGN_OVERLAYEX_MAX_X], align:2,
     * y:[0, OT_RGN_OVERLAYEX_MAX_Y], align:2
     */
    ot_point                       point;
    td_u32                         fg_alpha;
    td_u32                         bg_alpha;
    td_u32                         layer;     /* OVERLAYEX region layer range depends on OVERLAYEX max num */
} ot_rgn_overlayex_chn_attr;

typedef struct {
    ot_cover                       cover;
    td_u32                         layer;  /* COVER region layer range depends on COVER max num */
    ot_coord                       coord;  /* ratio coordinate or abs coordinate */
} ot_rgn_cover_chn_attr;

typedef struct {
    ot_cover                       coverex;
    td_u32                         layer;  /* COVEREX region layer range depends on COVEREX max num */
    ot_coord                       coord;  /* ratio coordinate or abs coordinate */
} ot_rgn_coverex_chn_attr;

typedef struct {
    ot_rect                        rect;              /* location of MOSAIC */
    ot_mosaic_blk_size             blk_size;          /* block size of MOSAIC */
    td_u32                         layer;             /* MOSAIC region layer range depends on MOSAIC max num */
} ot_rgn_mosaic_chn_attr;

typedef struct {
    ot_rect                        rect;              /* location of MOSAICEX */
    ot_mosaic_blk_size             blk_size;          /* block size of MOSAIC EX */
    td_u32                         layer;             /* MOSAICEX region layer range depends on MOSAICEX max num */
} ot_rgn_mosaicex_chn_attr;

typedef union {
    ot_rgn_overlay_attr           overlay; /* attribute of overlay region. AUTO:ot_rgn_type:OT_RGN_OVERLAY; */
    ot_rgn_overlayex_attr         overlayex; /* attribute of overlayex region. AUTO:ot_rgn_type:OT_RGN_OVERLAYEX; */
} ot_rgn_type_attr;

typedef struct {
    /* width of line range[OT_RGN_LINE_MIN_THICK, OT_RGN_LINE_MAX_THICK] */
    td_u32                         thick;
    td_u32                         color;                           /* color of line range [0, 0xffffff] */
    ot_point                       points[OT_RGN_LINE_POINT_NUM];   /* startpoint and endpoint of line */
} ot_rgn_line_chn_attr;

typedef struct {
    ot_corner_rect                 corner_rect;      /* location and size of corner rect */
    ot_corner_rect_attr            corner_rect_attr; /* attribute of corner rect region */
    td_u32                         layer;            /* CORNER_RECT region layer range depends on CORNER_RECT max num */
} ot_rgn_corner_rect_chn_attr;

typedef union {
    ot_rgn_overlay_chn_attr     overlay_chn; /* attribute of overlay region. AUTO:ot_rgn_type:OT_RGN_OVERLAY; */
    ot_rgn_cover_chn_attr       cover_chn; /* attribute of cover region. AUTO:ot_rgn_type: OT_RGN_COVER; */
    ot_rgn_overlayex_chn_attr   overlayex_chn; /* attribute of overlayex region. AUTO:ot_rgn_type: OT_RGN_OVERLAYEX; */
    ot_rgn_coverex_chn_attr     coverex_chn; /* attribute of coverex region. AUTO:ot_rgn_type:OT_RGN_COVEREX; */
    ot_rgn_line_chn_attr        line_chn; /* attribute of line region. AUTO:ot_rgn_type:OT_RGN_LINE; */
    ot_rgn_mosaic_chn_attr      mosaic_chn; /* attribute of mosaic region. AUTO:ot_rgn_type:OT_RGN_MOSAIC; */
    ot_rgn_mosaicex_chn_attr    mosaicex_chn; /* attribute of mosaicex region. AUTO:ot_rgn_type:OT_RGN_MOSAICEX; */
    /*
     * attribute of corner rectex region.
     * AUTO:ot_rgn_type:OT_RGN_CORNER_RECTEX;
     */
    ot_rgn_corner_rect_chn_attr corner_rectex_chn;
} ot_rgn_type_chn_attr;

/* attribute of a region */
typedef struct {
    ot_rgn_type                    type;
    ot_rgn_type_attr               attr; /* region attribute */
} ot_rgn_attr;

/* attribute of a region */
typedef struct {
    td_bool                        is_show;
    ot_rgn_type                    type;
    ot_rgn_type_chn_attr           attr;        /* region attribute */
} ot_rgn_chn_attr;

typedef struct {
    td_phys_addr_t                 phys_addr;
    ot_size                        size;
    td_u32                         stride;
    ot_pixel_format                pixel_format;
    td_void  ATTRIBUTE             *virt_addr;
} ot_rgn_canvas_info;

    /* invalid device ID */
#define OT_ERR_RGN_INVALID_DEV_ID OT_DEFINE_ERR(OT_ID_RGN, OT_ERR_LEVEL_ERROR, OT_ERR_INVALID_DEV_ID)
    /* invalid channel ID */
#define OT_ERR_RGN_INVALID_CHN_ID OT_DEFINE_ERR(OT_ID_RGN, OT_ERR_LEVEL_ERROR, OT_ERR_INVALID_CHN_ID)
    /* at least one parameter is illegal, e.g. an illegal enumeration value */
#define OT_ERR_RGN_ILLEGAL_PARAM OT_DEFINE_ERR(OT_ID_RGN, OT_ERR_LEVEL_ERROR, OT_ERR_ILLEGAL_PARAM)
    /* channel exists */
#define OT_ERR_RGN_EXIST OT_DEFINE_ERR(OT_ID_RGN, OT_ERR_LEVEL_ERROR, OT_ERR_EXIST)
    /* not exist */
#define OT_ERR_RGN_UNEXIST OT_DEFINE_ERR(OT_ID_RGN, OT_ERR_LEVEL_ERROR, OT_ERR_UNEXIST)
    /* using a NULL pointer */
#define OT_ERR_RGN_NULL_PTR OT_DEFINE_ERR(OT_ID_RGN, OT_ERR_LEVEL_ERROR, OT_ERR_NULL_PTR)
    /* try to enable or initialize system, device or channel, before configuring attribute */
#define OT_ERR_RGN_NOT_CFG OT_DEFINE_ERR(OT_ID_RGN, OT_ERR_LEVEL_ERROR, OT_ERR_NOT_CFG)
    /* operation is not supported */
#define OT_ERR_RGN_NOT_SUPPORT OT_DEFINE_ERR(OT_ID_RGN, OT_ERR_LEVEL_ERROR, OT_ERR_NOT_SUPPORT)
    /* operation is not permitted, e.g. try to change static attribute */
#define OT_ERR_RGN_NOT_PERM OT_DEFINE_ERR(OT_ID_RGN, OT_ERR_LEVEL_ERROR, OT_ERR_NOT_PERM)
    /* failure caused by malloc memory */
#define OT_ERR_RGN_NO_MEM OT_DEFINE_ERR(OT_ID_RGN, OT_ERR_LEVEL_ERROR, OT_ERR_NO_MEM)
    /* failure caused by malloc buffer */
#define OT_ERR_RGN_NO_BUF OT_DEFINE_ERR(OT_ID_RGN, OT_ERR_LEVEL_ERROR, OT_ERR_NO_BUF)
    /* no data in buffer */
#define OT_ERR_RGN_BUF_EMPTY OT_DEFINE_ERR(OT_ID_RGN, OT_ERR_LEVEL_ERROR, OT_ERR_BUF_EMPTY)
    /* no buffer for new data */
#define OT_ERR_RGN_BUF_FULL OT_DEFINE_ERR(OT_ID_RGN, OT_ERR_LEVEL_ERROR, OT_ERR_BUF_FULL)
    /* bad address, e.g. used for copy_from_user & copy_to_user */
#define OT_ERR_RGN_BAD_ADDR OT_DEFINE_ERR(OT_ID_RGN, OT_ERR_LEVEL_ERROR, OT_ERR_BAD_ADDR)
    /* resource is busy, e.g. destroy a venc chn without unregistering it */
#define OT_ERR_RGN_BUSY OT_DEFINE_ERR(OT_ID_RGN, OT_ERR_LEVEL_ERROR, OT_ERR_BUSY)
    /*
     * System is not ready, maybe not initialized or loaded.
     * Returning the error code when opening a device file failed.
     */
#define OT_ERR_RGN_NOT_READY OT_DEFINE_ERR(OT_ID_RGN, OT_ERR_LEVEL_ERROR, OT_ERR_NOT_READY)

#endif /* __OT_COMMON_REGION_H__ */
