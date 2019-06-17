/******************************************************************************

  Copyright (C), 2001-2011, Hisilicon Tech. Co., Ltd.

 ******************************************************************************
  File Name     : hi_comm_vo.h
  Version       : Initial Draft
  Author        : Hisilicon multimedia software group
  Created       : 2009/03/18
  Description   :
  History       :
  1.Date        : 2009/03/18
    Author      : x00100808
    Modification: Created file

******************************************************************************/

#ifndef __HI_COMM_VO_H__
#define __HI_COMM_VO_H__

#include "hi_type.h"
#include "hi_common.h"
#include "hi_comm_video.h"

#define VO_DEF_CHN_BUF_LEN      8
#define VO_DEF_DISP_BUF_LEN		5
#define VO_DEF_VIRT_BUF_LEN		3

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* End of #ifdef __cplusplus */

typedef enum hiEN_VOU_ERR_CODE_E
{
    EN_ERR_VO_DEV_NOT_CONFIG	  = 0x40,
    EN_ERR_VO_DEV_NOT_ENABLE      = 0x41,
    EN_ERR_VO_DEV_HAS_ENABLED     = 0x42,
    EN_ERR_VO_DEV_HAS_BINDED      = 0x43,
    EN_ERR_VO_DEV_NOT_BINDED      = 0x44,

    ERR_VO_NOT_ENABLE             = 0x45,
    ERR_VO_NOT_DISABLE            = 0x46,
    ERR_VO_NOT_CONFIG             = 0x47,

    ERR_VO_CHN_NOT_DISABLE        = 0x48,
    ERR_VO_CHN_NOT_ENABLE         = 0x49,
    ERR_VO_CHN_NOT_CONFIG         = 0x4a,
    ERR_VO_CHN_NOT_ALLOC          = 0x4b,

    ERR_VO_CCD_INVALID_PAT        = 0x4c,
    ERR_VO_CCD_INVALID_POS        = 0x4d,

    ERR_VO_WAIT_TIMEOUT           = 0x4e,
    ERR_VO_INVALID_VFRAME         = 0x4f,
    ERR_VO_INVALID_RECT_PARA      = 0x50,
    ERR_VO_SETBEGIN_ALREADY       = 0x51,
    ERR_VO_SETBEGIN_NOTYET        = 0x52,
    ERR_VO_SETEND_ALREADY         = 0x53,
    ERR_VO_SETEND_NOTYET          = 0x54,

    ERR_VO_GRP_INVALID_ID         = 0x55,
    ERR_VO_GRP_NOT_CREATE         = 0x56,
    ERR_VO_GRP_HAS_CREATED        = 0x57,
    ERR_VO_GRP_NOT_DESTROY        = 0x58,
    ERR_VO_GRP_CHN_FULL           = 0x59,
    ERR_VO_GRP_CHN_EMPTY          = 0x5a,
    ERR_VO_GRP_CHN_NOT_EMPTY      = 0x5b,
    ERR_VO_GRP_INVALID_SYN_MODE   = 0x5c,
    ERR_VO_GRP_INVALID_BASE_PTS   = 0x5d,
    ERR_VO_GRP_NOT_START          = 0x5e,
    ERR_VO_GRP_NOT_STOP           = 0x5f,
    ERR_VO_GRP_INVALID_FRMRATE    = 0x60,
    ERR_VO_GRP_CHN_HAS_REG        = 0x61,
    ERR_VO_GRP_CHN_NOT_REG        = 0x62,
    ERR_VO_GRP_CHN_NOT_UNREG      = 0x63,
    ERR_VO_GRP_BASE_NOT_CFG       = 0x64,

    ERR_GFX_NOT_DISABLE           = 0x65,
    ERR_GFX_NOT_BIND              = 0x66,
    ERR_GFX_NOT_UNBIND            = 0x67,
    ERR_GFX_INVALID_ID            = 0x68,

    ERR_VO_WBC_NOT_DISABLE        = 0x69,
    ERR_VO_WBC_NOT_CONFIG         = 0x6a,

    ERR_VO_CHN_AREA_OVERLAP       = 0x6b,

    EN_ERR_INVALID_WBCID          = 0x6c,
    EN_ERR_INVALID_LAYERID        = 0x6d,
    EN_ERR_VO_VIDEO_HAS_BINDED    = 0x6e,
    EN_ERR_VO_VIDEO_NOT_BINDED    = 0x6f,
    ERR_VO_WBC_HAS_BIND           = 0x70,
    ERR_VO_WBC_HAS_CONFIG         = 0x71, 
    ERR_VO_WBC_NOT_BIND           = 0x72,

    /* new added */
    ERR_VO_BUTT

}EN_VOU_ERR_CODE_E;

/* System define error code */
#define HI_ERR_VO_BUSY                  HI_DEF_ERR(HI_ID_VOU, EN_ERR_LEVEL_ERROR, EN_ERR_BUSY)
#define HI_ERR_VO_NO_MEM                HI_DEF_ERR(HI_ID_VOU, EN_ERR_LEVEL_ERROR, EN_ERR_NOMEM)
#define HI_ERR_VO_NULL_PTR              HI_DEF_ERR(HI_ID_VOU, EN_ERR_LEVEL_ERROR, EN_ERR_NULL_PTR)
#define HI_ERR_VO_SYS_NOTREADY          HI_DEF_ERR(HI_ID_VOU, EN_ERR_LEVEL_ERROR, EN_ERR_SYS_NOTREADY)
#define HI_ERR_VO_INVALID_DEVID         HI_DEF_ERR(HI_ID_VOU, EN_ERR_LEVEL_ERROR, EN_ERR_INVALID_DEVID)
#define HI_ERR_VO_INVALID_CHNID         HI_DEF_ERR(HI_ID_VOU, EN_ERR_LEVEL_ERROR, EN_ERR_INVALID_CHNID)
#define HI_ERR_VO_ILLEGAL_PARAM         HI_DEF_ERR(HI_ID_VOU, EN_ERR_LEVEL_ERROR, EN_ERR_ILLEGAL_PARAM)
#define HI_ERR_VO_NOT_SUPPORT          HI_DEF_ERR(HI_ID_VOU, EN_ERR_LEVEL_ERROR, EN_ERR_NOT_SUPPORT)
#define HI_ERR_VO_NOT_PERMIT            HI_DEF_ERR(HI_ID_VOU, EN_ERR_LEVEL_ERROR, EN_ERR_NOT_PERM)
#define HI_ERR_VO_INVALID_WBCID         HI_DEF_ERR(HI_ID_VOU, EN_ERR_LEVEL_ERROR, EN_ERR_INVALID_WBCID)
#define HI_ERR_VO_INVALID_LAYERID        HI_DEF_ERR(HI_ID_VOU, EN_ERR_LEVEL_ERROR, EN_ERR_INVALID_LAYERID)


/* device relative error code */
#define HI_ERR_VO_DEV_NOT_CONFIG        HI_DEF_ERR(HI_ID_VOU, EN_ERR_LEVEL_ERROR, EN_ERR_VO_DEV_NOT_CONFIG)
#define HI_ERR_VO_DEV_NOT_ENABLE        HI_DEF_ERR(HI_ID_VOU, EN_ERR_LEVEL_ERROR, EN_ERR_VO_DEV_NOT_ENABLE)
#define HI_ERR_VO_DEV_HAS_ENABLED       HI_DEF_ERR(HI_ID_VOU, EN_ERR_LEVEL_ERROR, EN_ERR_VO_DEV_HAS_ENABLED)
#define HI_ERR_VO_DEV_HAS_BINDED        HI_DEF_ERR(HI_ID_VOU, EN_ERR_LEVEL_ERROR, EN_ERR_VO_DEV_HAS_BINDED)
#define HI_ERR_VO_DEV_NOT_BINDED        HI_DEF_ERR(HI_ID_VOU, EN_ERR_LEVEL_ERROR, EN_ERR_VO_DEV_NOT_BINDED)

/* video relative error code */
#define HI_ERR_VO_VIDEO_NOT_ENABLE      HI_DEF_ERR(HI_ID_VOU, EN_ERR_LEVEL_ERROR, ERR_VO_NOT_ENABLE)
#define HI_ERR_VO_VIDEO_NOT_DISABLE     HI_DEF_ERR(HI_ID_VOU, EN_ERR_LEVEL_ERROR, ERR_VO_NOT_DISABLE)
#define HI_ERR_VO_VIDEO_NOT_CONFIG      HI_DEF_ERR(HI_ID_VOU, EN_ERR_LEVEL_ERROR, ERR_VO_NOT_CONFIG)
#define HI_ERR_VO_VIDEO_HAS_BINDED      HI_DEF_ERR(HI_ID_VOU, EN_ERR_LEVEL_ERROR, EN_ERR_VO_VIDEO_HAS_BINDED)
#define HI_ERR_VO_VIDEO_NOT_BINDED      HI_DEF_ERR(HI_ID_VOU, EN_ERR_LEVEL_ERROR, EN_ERR_VO_VIDEO_NOT_BINDED)

/*wbc error code*/
#define HI_ERR_VO_WBC_NOT_DISABLE     HI_DEF_ERR(HI_ID_VOU, EN_ERR_LEVEL_ERROR, ERR_VO_WBC_NOT_DISABLE)
#define HI_ERR_VO_WBC_NOT_CONFIG      HI_DEF_ERR(HI_ID_VOU, EN_ERR_LEVEL_ERROR, ERR_VO_WBC_NOT_CONFIG)
#define HI_ERR_VO_WBC_HAS_CONFIG      HI_DEF_ERR(HI_ID_VOU, EN_ERR_LEVEL_ERROR, ERR_VO_WBC_HAS_CONFIG)
#define HI_ERR_VO_WBC_NOT_BIND        HI_DEF_ERR(HI_ID_VOU, EN_ERR_LEVEL_ERROR, ERR_VO_WBC_NOT_BIND)
#define HI_ERR_VO_WBC_HAS_BIND        HI_DEF_ERR(HI_ID_VOU, EN_ERR_LEVEL_ERROR, ERR_VO_WBC_HAS_BIND)

/* channel relative error code */
#define HI_ERR_VO_CHN_NOT_DISABLE       HI_DEF_ERR(HI_ID_VOU, EN_ERR_LEVEL_ERROR, ERR_VO_CHN_NOT_DISABLE)
#define HI_ERR_VO_CHN_NOT_ENABLE        HI_DEF_ERR(HI_ID_VOU, EN_ERR_LEVEL_ERROR, ERR_VO_CHN_NOT_ENABLE)
#define HI_ERR_VO_CHN_NOT_CONFIG        HI_DEF_ERR(HI_ID_VOU, EN_ERR_LEVEL_ERROR, ERR_VO_CHN_NOT_CONFIG)
#define HI_ERR_VO_CHN_NOT_ALLOC         HI_DEF_ERR(HI_ID_VOU, EN_ERR_LEVEL_ERROR, ERR_VO_CHN_NOT_ALLOC)
#define HI_ERR_VO_CHN_AREA_OVERLAP      HI_DEF_ERR(HI_ID_VOU, EN_ERR_LEVEL_ERROR, ERR_VO_CHN_AREA_OVERLAP)


/* cascade relatvie error code */
#define HI_ERR_VO_INVALID_PATTERN       HI_DEF_ERR(HI_ID_VOU, EN_ERR_LEVEL_ERROR, ERR_VO_CCD_INVALID_PAT)
#define HI_ERR_VO_INVALID_POSITION      HI_DEF_ERR(HI_ID_VOU, EN_ERR_LEVEL_ERROR, ERR_VO_CCD_INVALID_POS)

/* misc */
#define HI_ERR_VO_WAIT_TIMEOUT          HI_DEF_ERR(HI_ID_VOU, EN_ERR_LEVEL_ERROR, ERR_VO_WAIT_TIMEOUT)
#define HI_ERR_VO_INVALID_VFRAME        HI_DEF_ERR(HI_ID_VOU, EN_ERR_LEVEL_ERROR, ERR_VO_INVALID_VFRAME)
#define HI_ERR_VO_INVALID_RECT_PARA     HI_DEF_ERR(HI_ID_VOU, EN_ERR_LEVEL_ERROR, ERR_VO_INVALID_RECT_PARA)
#define HI_ERR_VO_SETBEGIN_ALREADY      HI_DEF_ERR(HI_ID_VOU, EN_ERR_LEVEL_ERROR, ERR_VO_SETBEGIN_ALREADY)
#define HI_ERR_VO_SETBEGIN_NOTYET       HI_DEF_ERR(HI_ID_VOU, EN_ERR_LEVEL_ERROR, ERR_VO_SETBEGIN_NOTYET)
#define HI_ERR_VO_SETEND_ALREADY        HI_DEF_ERR(HI_ID_VOU, EN_ERR_LEVEL_ERROR, ERR_VO_SETEND_ALREADY)
#define HI_ERR_VO_SETEND_NOTYET         HI_DEF_ERR(HI_ID_VOU, EN_ERR_LEVEL_ERROR, ERR_VO_SETEND_NOTYET)

/* sync group relative error code */
#define HI_ERR_VO_GRP_INVALID_ID        HI_DEF_ERR(HI_ID_VOU, EN_ERR_LEVEL_ERROR, ERR_VO_GRP_INVALID_ID)
#define HI_ERR_VO_GRP_NOT_CREATE        HI_DEF_ERR(HI_ID_VOU, EN_ERR_LEVEL_ERROR, ERR_VO_GRP_NOT_CREATE)
#define HI_ERR_VO_GRP_HAS_CREATED       HI_DEF_ERR(HI_ID_VOU, EN_ERR_LEVEL_ERROR, ERR_VO_GRP_HAS_CREATED)
#define HI_ERR_VO_GRP_NOT_DESTROY       HI_DEF_ERR(HI_ID_VOU, EN_ERR_LEVEL_ERROR, ERR_VO_GRP_NOT_DESTROY)
#define HI_ERR_VO_GRP_CHN_FULL          HI_DEF_ERR(HI_ID_VOU, EN_ERR_LEVEL_ERROR, ERR_VO_GRP_CHN_FULL)
#define HI_ERR_VO_GRP_CHN_EMPTY         HI_DEF_ERR(HI_ID_VOU, EN_ERR_LEVEL_ERROR, ERR_VO_GRP_CHN_EMPTY)
#define HI_ERR_VO_GRP_CHN_NOT_EMPTY     HI_DEF_ERR(HI_ID_VOU, EN_ERR_LEVEL_ERROR, ERR_VO_GRP_CHN_NOT_EMPTY)
#define HI_ERR_VO_GRP_INVALID_SYN_MODE  HI_DEF_ERR(HI_ID_VOU, EN_ERR_LEVEL_ERROR, ERR_VO_GRP_INVALID_SYN_MODE)
#define HI_ERR_VO_GRP_INVALID_BASE_PTS  HI_DEF_ERR(HI_ID_VOU, EN_ERR_LEVEL_ERROR, ERR_VO_GRP_INVALID_BASE_PTS)
#define HI_ERR_VO_GRP_NOT_START         HI_DEF_ERR(HI_ID_VOU, EN_ERR_LEVEL_ERROR, ERR_VO_GRP_NOT_START)
#define HI_ERR_VO_GRP_NOT_STOP          HI_DEF_ERR(HI_ID_VOU, EN_ERR_LEVEL_ERROR, ERR_VO_GRP_NOT_STOP)
#define HI_ERR_VO_GRP_INVALID_FRMRATE   HI_DEF_ERR(HI_ID_VOU, EN_ERR_LEVEL_ERROR, ERR_VO_GRP_INVALID_FRMRATE)
#define HI_ERR_VO_GRP_CHN_HAS_REG       HI_DEF_ERR(HI_ID_VOU, EN_ERR_LEVEL_ERROR, ERR_VO_GRP_CHN_HAS_REG)
#define HI_ERR_VO_GRP_CHN_NOT_REG       HI_DEF_ERR(HI_ID_VOU, EN_ERR_LEVEL_ERROR, ERR_VO_GRP_CHN_NOT_REG)
#define HI_ERR_VO_GRP_CHN_NOT_UNREG     HI_DEF_ERR(HI_ID_VOU, EN_ERR_LEVEL_ERROR, ERR_VO_GRP_CHN_NOT_UNREG)
#define HI_ERR_VO_GRP_BASE_NOT_CFG      HI_DEF_ERR(HI_ID_VOU, EN_ERR_LEVEL_ERROR, ERR_VO_GRP_BASE_NOT_CFG)


/* graphics relative error code */
#define HI_ERR_VO_GFX_NOT_DISABLE     HI_DEF_ERR(HI_ID_VOU, EN_ERR_LEVEL_ERROR, ERR_GFX_NOT_DISABLE)
#define HI_ERR_VO_GFX_NOT_BIND        HI_DEF_ERR(HI_ID_VOU, EN_ERR_LEVEL_ERROR, ERR_GFX_NOT_BIND)
#define HI_ERR_VO_GFX_NOT_UNBIND      HI_DEF_ERR(HI_ID_VOU, EN_ERR_LEVEL_ERROR, ERR_GFX_NOT_UNBIND)
#define HI_ERR_VO_GFX_INVALID_ID      HI_DEF_ERR(HI_ID_VOU, EN_ERR_LEVEL_ERROR, ERR_GFX_INVALID_ID)

/* vo inteface type */
#define VO_INTF_CVBS        (0x01L<<0)
#define VO_INTF_YPBPR       (0x01L<<1)
#define VO_INTF_VGA         (0x01L<<2)
#define VO_INTF_BT656       (0x01L<<3)
#define VO_INTF_BT1120      (0x01L<<4)
#define VO_INTF_HDMI        (0x01L<<5)
#define VO_INTF_LCD         (0x01L<<6)
#define VO_INTF_BT656_H     (0x01L<<7)
#define VO_INTF_BT656_L     (0x01L<<8)
#define VO_INTF_LCD_6BIT    (0x01L<<9)
#define VO_INTF_LCD_8BIT    (0x01L<<10)
#define VO_INTF_LCD_16BIT   (0x01L<<11)
#define VO_INTF_LCD_24BIT   (0x01L<<12)





#define VO_DEFAULT_CHN      -1          /* use vo buffer as pip buffer */

/*****************************************************************************
 * 3520 ADDed
 *****************************************************************************/
typedef HI_U32 VO_INTF_TYPE_E;



typedef enum hiVO_INTF_SYNC_E
{
    VO_OUTPUT_PAL = 0,
    VO_OUTPUT_NTSC,
    
    VO_OUTPUT_1080P24,
    VO_OUTPUT_1080P25,
    VO_OUTPUT_1080P30,
    
    VO_OUTPUT_720P50, 
    VO_OUTPUT_720P60,   
    VO_OUTPUT_1080I50,
    VO_OUTPUT_1080I60,    
    VO_OUTPUT_1080P50,
    VO_OUTPUT_1080P60,            

    VO_OUTPUT_576P50,
    VO_OUTPUT_480P60,

    VO_OUTPUT_800x600_60,            /* VESA 800 x 600 at 60 Hz (non-interlaced) */
    VO_OUTPUT_1024x768_60,           /* VESA 1024 x 768 at 60 Hz (non-interlaced) */
    VO_OUTPUT_1280x1024_60,          /* VESA 1280 x 1024 at 60 Hz (non-interlaced) */
    VO_OUTPUT_1366x768_60,           /* VESA 1366 x 768 at 60 Hz (non-interlaced) */
    VO_OUTPUT_1440x900_60,           /* VESA 1440 x 900 at 60 Hz (non-interlaced) CVT Compliant */
    VO_OUTPUT_1280x800_60,           /* 1280*800@60Hz VGA@60Hz*/
    VO_OUTPUT_1600x1200_60,          /* VESA 1600 x 1200 at 60 Hz (non-interlaced) */
    VO_OUTPUT_1680x1050_60,          /* VESA 1680 x 1050 at 60 Hz (non-interlaced) */
    VO_OUTPUT_1920x1200_60,          /* VESA 1920 x 1600 at 60 Hz (non-interlaced) CVT (Reduced Blanking)*/
    VO_OUTPUT_640x480_60,            /* VESA 640 x 480 at 60 Hz (non-interlaced) CVT */
    VO_OUTPUT_960H_PAL,              /* ITU-R BT.1302 960 x 576 at 50 Hz (interlaced)*/
    VO_OUTPUT_960H_NTSC,             /* ITU-R BT.1302 960 x 480 at 60 Hz (interlaced)*/
    VO_OUTPUT_320X240_60,            /* For ota5182 at 60 Hz (8bit) */
    VO_OUTPUT_320X240_50,            /* For ili9342 at 50 Hz (6bit) */
    VO_OUTPUT_240X320_50,            /* For ili9341 at 50 Hz (6bit) */
    VO_OUTPUT_240X320_60,            /* For ili9341 at 60 Hz (16bit) */
    VO_OUTPUT_800X600_50,            /* For LCD at 50 Hz (24bit) */
    VO_OUTPUT_3840x2160_30,          /* 3840x2160_30 */
    VO_OUTPUT_USER,
    VO_OUTPUT_BUTT

} VO_INTF_SYNC_E;

typedef enum hiVO_DISPLAY_FIELD_E
{
    VO_FIELD_TOP,                 /* top field*/
    VO_FIELD_BOTTOM,              /* bottom field*/
    VO_FIELD_BOTH,                /* top and bottom field*/
    VO_FIELD_BUTT
} VO_DISPLAY_FIELD_E;

typedef enum hiVOU_ZOOM_IN_E
{
    VOU_ZOOM_IN_RECT = 0,       /* zoom in by rect */
    VOU_ZOOM_IN_RATIO,          /* zoom in by ratio */
    VOU_ZOOM_IN_BUTT
} VOU_ZOOM_IN_E;

typedef enum hiVO_CSC_MATRIX_E
{
    VO_CSC_MATRIX_IDENTITY = 0,
    
    VO_CSC_MATRIX_BT601_TO_BT709,
    VO_CSC_MATRIX_BT709_TO_BT601,

    VO_CSC_MATRIX_BT601_TO_RGB_PC,
    VO_CSC_MATRIX_BT709_TO_RGB_PC,

    VO_CSC_MATRIX_RGB_TO_BT601_PC,
    VO_CSC_MATRIX_RGB_TO_BT709_PC,

    VO_CSC_MATRIX_BUTT
} VO_CSC_MATRIX_E;

typedef struct hiVO_CHN_ATTR_S
{
    HI_U32  u32Priority;                /* video out overlay pri */
    RECT_S  stRect;                     /* rect of video out chn */
    HI_BOOL bDeflicker;                 /* deflicker or not */
}VO_CHN_ATTR_S;

typedef struct hiVO_BORDER_S
{    
    HI_BOOL bBorderEn;                   /*do Frame or not*/  
    BORDER_S stBorder;
}VO_BORDER_S;


typedef struct hiVO_QUERY_STATUS_S
{
    HI_U32 u32ChnBufUsed;       /* channel buffer that been occupied */
} VO_QUERY_STATUS_S;

typedef struct tagVO_SYNC_INFO_S
{
    HI_BOOL  bSynm;     /* sync mode(0:timing,as BT.656; 1:signal,as LCD) */
    HI_BOOL  bIop;      /* interlaced or progressive display(0:i; 1:p) */
    HI_U8    u8Intfb;   /* interlace bit width while output */

    HI_U16   u16Vact ;  /* vertical active area */
    HI_U16   u16Vbb;    /* vertical back blank porch */
    HI_U16   u16Vfb;    /* vertical front blank porch */

    HI_U16   u16Hact;   /* herizontal active area */
    HI_U16   u16Hbb;    /* herizontal back blank porch */
    HI_U16   u16Hfb;    /* herizontal front blank porch */
    HI_U16   u16Hmid;   /* bottom herizontal active area */

    HI_U16   u16Bvact;  /* bottom vertical active area */
    HI_U16   u16Bvbb;   /* bottom vertical back blank porch */
    HI_U16   u16Bvfb;   /* bottom vertical front blank porch */

    HI_U16   u16Hpw;    /* horizontal pulse width */
    HI_U16   u16Vpw;    /* vertical pulse width */

    HI_BOOL  bIdv;      /* inverse data valid of output */
    HI_BOOL  bIhs;      /* inverse horizontal synch signal */
    HI_BOOL  bIvs;      /* inverse vertical synch signal */

} VO_SYNC_INFO_S;

typedef struct hiVO_PUB_ATTR_S
{
    HI_U32                   u32BgColor;          /* Background color of a device, in RGB format. */
    VO_INTF_TYPE_E           enIntfType;          /* Type of a VO interface */
    VO_INTF_SYNC_E           enIntfSync;          /* Type of a VO interface timing */
    VO_SYNC_INFO_S           stSyncInfo;          /* Information about VO interface timings */
} VO_PUB_ATTR_S;


typedef enum hiVO_PART_MODE_E
{
    VO_PART_MODE_SINGLE	= 0,		/* single partition, which use software to make multi-picture in one hardware cell */
	VO_PART_MODE_MULTI	= 1,		/* muliti partition, each partition is a hardware cell */
	VO_PART_MODE_BUTT
	
} VO_PART_MODE_E;

typedef struct hiVO_VIDEO_LAYER_ATTR_S
{
    RECT_S stDispRect;                  /* Display resolution */
    SIZE_S stImageSize;                 /* Canvas size of the video layer */
    HI_U32 u32DispFrmRt;                /* Display frame rate */
    PIXEL_FORMAT_E enPixFormat;         /* Pixel format of the video layer */
    HI_BOOL bDoubleFrame;               /* Whether to double frames */  
    HI_BOOL bClusterMode;               /* Whether to take Cluster way to use memory*/ 
} VO_VIDEO_LAYER_ATTR_S;


typedef struct hiVO_ZOOM_RATIO_S
{
    HI_U32 u32XRatio;
    HI_U32 u32YRatio;
    HI_U32 u32WRatio;
    HI_U32 u32HRatio;    
} VO_ZOOM_RATIO_S;

typedef struct hiVO_ZOOM_ATTR_S
{
    VOU_ZOOM_IN_E   enZoomType;         /* choose the type of zoom in */
    union
    {
        RECT_S          stZoomRect;     /* zoom in by rect */
        VO_ZOOM_RATIO_S stZoomRatio;    /* zoom in by ratio */
    };
} VO_ZOOM_ATTR_S;

typedef struct hiVO_CSC_S
{
    VO_CSC_MATRIX_E enCscMatrix;
    HI_U32 u32Luma;                     /* luminance:   0 ~ 100 default: 50 */
    HI_U32 u32Contrast;                 /* contrast :   0 ~ 100 default: 50 */
    HI_U32 u32Hue;                      /* hue      :   0 ~ 100 default: 50 */
    HI_U32 u32Satuature;                /* satuature:   0 ~ 100 default: 50 */
} VO_CSC_S;


typedef struct hiVO_REGION_INFO_S
{
    RECT_S *pstRegion;    /*region attribute*/
    HI_U32 u32RegionNum;       /*count of the region*/
}VO_REGION_INFO_S;


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif /* End of #ifndef __HI_COMM_VO_H__ */

