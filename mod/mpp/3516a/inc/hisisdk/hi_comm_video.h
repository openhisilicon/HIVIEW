/*******************************************************************************
  Copyright (C), 2001-2011, Hisilicon Tech. Co., Ltd.

 ******************************************************************************
 File Name     : hi_comm_video.h
 Version       : Initial Draft
 Author        : c42025
 Created       : 2006/11/09
 Description   :
 History       :
 1.Date        : 2006/11/03
   Author      : c42025
   Modification: Created file

 2.Date        : 2007/12/11
   Author      : c42025
   Modification: delelte all about digital watermark

 3.Date        :   2008/10/31
   Author      :   z44949
   Modification:   Translate the chinese comment
******************************************************************************/

#ifndef __HI_COMM_VIDEO_H__
#define __HI_COMM_VIDEO_H__

#include "hi_type.h"
#include "hi_defines.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

#define FISHEYE_MAX_REGION_NUM 	4
#define FISHEYE_MAX_OFFSET 		127
#define FISHEYE_MAX_PAN_VALUE   360
#define FISHEYE_MAX_TILT_VALUE	360
#define FISHEYE_MAX_ZOOM_VALUE  4095
#define FISHEYE_MIN_ZOOM_VALUE  1
#define FISHEYE_MIN_OUT_WIDTH  	960
#define FISHEYE_MIN_OUT_HEIGHT  360
#define FISHEYE_MAX_OUT_WIDTH  	4608
#define FISHEYE_MAX_OUT_HEIGHT  3456


typedef struct hiPOINT_S
{
    HI_S32 s32X;
    HI_S32 s32Y;
} POINT_S;

typedef struct hiSIZE_S
{
    HI_U32 u32Width;
    HI_U32 u32Height;
} SIZE_S;

typedef struct hiRECT_S
{
    HI_S32 s32X;
    HI_S32 s32Y;
    HI_U32 u32Width;
    HI_U32 u32Height;
} RECT_S;

typedef enum hiPIC_SIZE_E
{
    PIC_QCIF = 0,
    PIC_CIF,
    PIC_2CIF,
    PIC_HD1,
    PIC_D1,
    PIC_960H,

    PIC_QVGA,    /* 320 * 240 */
    PIC_VGA,     /* 640 * 480 */
    PIC_XGA,     /* 1024 * 768 */
    PIC_SXGA,    /* 1400 * 1050 */
    PIC_UXGA,    /* 1600 * 1200 */
    PIC_QXGA,    /* 2048 * 1536 */

    PIC_WVGA,    /* 854 * 480 */
    PIC_WSXGA,   /* 1680 * 1050 */
    PIC_WUXGA,   /* 1920 * 1200 */
    PIC_WQXGA,   /* 2560 * 1600 */

    PIC_HD720,   /* 1280 * 720 */
    PIC_HD1080,  /* 1920 * 1080 */
    PIC_2304x1296, /* 3M:2304 * 1296 */
    PIC_2592x1520, /* 4M:2592 * 1520 */
    PIC_5M,        /* 2592 * 1944 */
    PIC_UHD4K,     /* 3840 * 2160 */

    PIC_BUTT
} PIC_SIZE_E;

typedef enum hiVIDEO_NORM_E
{
    VIDEO_ENCODING_MODE_PAL = 0,
    VIDEO_ENCODING_MODE_NTSC,
    VIDEO_ENCODING_MODE_AUTO,
    VIDEO_ENCODING_MODE_BUTT
} VIDEO_NORM_E;

typedef enum hiVIDEO_CONTROL_MODE_E
{
    VIDEO_CONTROL_MODE_SLAVER = 0,
    VIDEO_CONTROL_MODE_MASTER,
    VIDEO_CONTROL_MODE_BUTT
} VIDEO_CONTROL_MODE_E;

/* we ONLY define picture format used, all unused will be deleted!*/
typedef enum hiPIXEL_FORMAT_E
{
    PIXEL_FORMAT_RGB_1BPP = 0,
    PIXEL_FORMAT_RGB_2BPP,
    PIXEL_FORMAT_RGB_4BPP,
    PIXEL_FORMAT_RGB_8BPP,
    PIXEL_FORMAT_RGB_444,

    PIXEL_FORMAT_RGB_4444,
    PIXEL_FORMAT_RGB_555,
    PIXEL_FORMAT_RGB_565,
    PIXEL_FORMAT_RGB_1555,

    /*  9 reserved */
    PIXEL_FORMAT_RGB_888,
    PIXEL_FORMAT_RGB_8888,

    PIXEL_FORMAT_RGB_PLANAR_888,
    PIXEL_FORMAT_RGB_BAYER_8BPP,
    PIXEL_FORMAT_RGB_BAYER_10BPP,
    PIXEL_FORMAT_RGB_BAYER_12BPP,
    PIXEL_FORMAT_RGB_BAYER_14BPP,

    PIXEL_FORMAT_RGB_BAYER,         /* 16 bpp */

    PIXEL_FORMAT_YUV_A422,
    PIXEL_FORMAT_YUV_A444,

    PIXEL_FORMAT_YUV_PLANAR_422,
    PIXEL_FORMAT_YUV_PLANAR_420,

    PIXEL_FORMAT_YUV_PLANAR_444,

    PIXEL_FORMAT_YUV_SEMIPLANAR_422,
    PIXEL_FORMAT_YUV_SEMIPLANAR_420,
    PIXEL_FORMAT_YUV_SEMIPLANAR_444,

    PIXEL_FORMAT_UYVY_PACKAGE_422,
    PIXEL_FORMAT_YUYV_PACKAGE_422,
    PIXEL_FORMAT_VYUY_PACKAGE_422,
    PIXEL_FORMAT_YCbCr_PLANAR,

    PIXEL_FORMAT_SINGLE,

    PIXEL_FORMAT_BUTT
} PIXEL_FORMAT_E;

typedef struct hiVIDEO_VBI_INFO_S
{
    HI_U32 au32Data[VIU_MAX_VBI_LEN];
    HI_U32 u32Len;
} VIDEO_VBI_INFO_S;

typedef enum hiVIDEO_FIELD_E
{
    VIDEO_FIELD_TOP         = 0x1,    /* even field */
    VIDEO_FIELD_BOTTOM      = 0x2,    /* odd field */
    VIDEO_FIELD_INTERLACED  = 0x3,    /* two interlaced fields */
    VIDEO_FIELD_FRAME       = 0x4,    /* frame */

    VIDEO_FIELD_BUTT
} VIDEO_FIELD_E;

typedef enum hiVIDEO_FORMAT_E
{
    VIDEO_FORMAT_LINEAR		= 0x0,		/* nature video line */
    VIDEO_FORMAT_TILE		= 0x1,		/* tile cell: 256pixel x 16line, default tile mode */
    VIDEO_FORMAT_TILE64		= 0x2,		/* tile cell: 64pixel x 16line */

    VIDEO_FORMAT_BUTT
} VIDEO_FORMAT_E;

typedef enum hiCOMPRESS_MODE_E
{
    COMPRESS_MODE_NONE		= 0x0,		/* no compress */
    COMPRESS_MODE_SEG		= 0x1,		/* compress unit is 256 bytes as a segment, default seg mode */
    COMPRESS_MODE_SEG128	= 0x2,		/* compress unit is 128 bytes as a segment */
    COMPRESS_MODE_LINE		= 0x3,		/* compress unit is the whole line */
    COMPRESS_MODE_FRAME		= 0x4,		/* compress unit is the whole frame */

    COMPRESS_MODE_BUTT
} COMPRESS_MODE_E;

typedef enum hiVIDEO_DISPLAY_MODE_E
{
    VIDEO_DISPLAY_MODE_PREVIEW		= 0x0,
    VIDEO_DISPLAY_MODE_PLAYBACK		= 0x1,

    VIDEO_DISPLAY_MODE_BUTT
} VIDEO_DISPLAY_MODE_E;


#define DCF_DRSCRIPTION_LENGTH      32
#define DCF_CAPTURE_TIME_LENGTH     20

typedef struct hiISP_DCF_INFO_S
{
    HI_U8       au8ImageDescription[DCF_DRSCRIPTION_LENGTH];        /*Describes image*/
    HI_U8       au8Make[DCF_DRSCRIPTION_LENGTH];                    /*Shows manufacturer of digital cameras*/
    HI_U8       au8Model[DCF_DRSCRIPTION_LENGTH];                   /*Shows model number of digital cameras*/
    HI_U8       au8Software[DCF_DRSCRIPTION_LENGTH];                /*Shows firmware (internal software of digital cameras) version number*/

    HI_U16      u16ISOSpeedRatings;                                 /*CCD sensitivity equivalent to Ag-Hr film speedrate*/
    HI_U32      u32FNumber;                                         /*The actual F-number (F-stop) of lens when the image was taken*/
    HI_U32      u32MaxApertureValue;                                /*Maximum aperture value of lens.*/
    HI_U32      u32ExposureTime;                                    /*Exposure time (reciprocal of shutter speed).*/
    HI_U32      u32ExposureBiasValue;                               /*Exposure bias (compensation) value of taking picture*/
    HI_U8       u8ExposureProgram;                                  /*Exposure program that the camera used when image was taken. '1' means manual control, '2'
                                                                              program normal, '3' aperture priority, '4' shutter priority, '5' program creative (slow program),
                                                                              '6' program action(high-speed program), '7' portrait mode, '8' landscape mode*/
    HI_U8       u8MeteringMode;                                     /*Exposure metering method. '0' means unknown, '1' average, '2' center weighted average, '3'
                                                                              spot, '4' multi-spot, '5' multi-segment, '6' partial, '255' other*/
    HI_U8       u8LightSource;                                      /*Light source, actually this means white balance setting. '0' means unknown, '1' daylight, '2'
                                                                               fluorescent, '3' tungsten, '10' flash, '17' standard light A, '18' standard light B, '19' standard light
                                                                               C, '20' D55, '21' D65, '22' D75, '255' other*/
    HI_U32      u32FocalLength;                                     /*Focal length of lens used to take image. Unit is millimeter*/
    HI_U8       u8SceneType;                                        /*Indicates the type of scene. Value '0x01' means that the image was directly photographed.*/
    HI_U8       u8CustomRendered;                                   /*Indicates the use of special processing on image data, such as rendering geared to output.
                                                                               0 = Normal process  1 = Custom process   */
    HI_U8       u8ExposureMode;                                     /*Indicates the exposure mode set when the image was shot.
                                                                              0 = Auto exposure,1 = Manual exposure, 2 = Auto bracket*/
    HI_U8       u8WhiteBalance;                                     /* Indicates the white balance mode set when the image was shot.
                                                                                0 = Auto white balance ,1 = Manual white balance */
    HI_U8       u8FocalLengthIn35mmFilm;                            /*Indicates the equivalent focal length assuming a 35mm film camera, in mm*/
    HI_U8       u8SceneCaptureType;                                 /*Indicates the type of scene that was shot. 0 = Standard,1 = Landscape,2 = Portrait,3 = Night scene. */
    HI_U8       u8GainControl;                                      /*Indicates the degree of overall image gain adjustment. 0 = None,1 = Low gain up,2 = High gain up,3 = Low gain down,4 = High gain down. */
    HI_U8       u8Contrast;                                         /*Indicates the direction of contrast processing applied by the camera when the image was shot.
                                                                               0 = Normal,1 = Soft,2 = Hard */
    HI_U8       u8Saturation;                                       /*Indicates the direction of saturation processing applied by the camera when the image was shot.
                                                                              0 = Normal,1 = Low saturation,2 = High saturation*/
    HI_U8       u8Sharpness;                                        /*Indicates the direction of sharpness processing applied by the camera when the image was shot.
                                                                              0 = Normal,1 = Soft,2 = Hard .*/
} ISP_DCF_INFO_S;

typedef struct hiJPEG_DCF_S
{
    HI_U8           au8CaptureTime[DCF_CAPTURE_TIME_LENGTH];            /*The date and time when the picture data was generated*/
    HI_U8           bFlash;                                             /*whether the picture is captured when a flash lamp is on*/
    HI_U32          u32DigitalZoomRatio;                                /*Indicates the digital zoom ratio when the image was shot.
                                                                                   If the numerator of the recorded value is 0, this indicates that digital zoom was not used.*/
    ISP_DCF_INFO_S  stIspDCFInfo;

} JPEG_DCF_S;

typedef enum hiFRAME_FLASH_TYPE_E
{
    FRAME_FLASH_OFF  = 0,
    FRAME_FLASH_ON   = 1,
    FRAME_FLASH_BUTT,
} FRAME_FLASH_TYPE_E;

typedef struct hiVIDEO_SUPPLEMENT_S
{
    FRAME_FLASH_TYPE_E enFlashType;
    HI_U32   u32JpegDcfPhyAddr;
    HI_VOID* pJpegDcfVirAddr;

} VIDEO_SUPPLEMENT_S;

typedef struct hiVIDEO_FRAME_S
{
    HI_U32          u32Width;
    HI_U32          u32Height;
    VIDEO_FIELD_E   u32Field;
    PIXEL_FORMAT_E  enPixelFormat;

    VIDEO_FORMAT_E  enVideoFormat;
    COMPRESS_MODE_E	enCompressMode;

    HI_U32          u32PhyAddr[3];
    HI_VOID*        pVirAddr[3];
    HI_U32          u32Stride[3];

    HI_U32          u32HeaderPhyAddr[3];
    HI_VOID*        pHeaderVirAddr[3];
    HI_U32          u32HeaderStride[3];

    HI_S16          s16OffsetTop;		/* top offset of show area */
    HI_S16          s16OffsetBottom;	/* bottom offset of show area */
    HI_S16          s16OffsetLeft;		/* left offset of show area */
    HI_S16          s16OffsetRight;		/* right offset of show area */

    HI_U64          u64pts;
    HI_U32          u32TimeRef;

    HI_U32          u32PrivateData;
    VIDEO_SUPPLEMENT_S stSupplement;
} VIDEO_FRAME_S;

typedef struct hiVIDEO_FRAME_INFO_S
{
    VIDEO_FRAME_S stVFrame;
    HI_U32 u32PoolId;
} VIDEO_FRAME_INFO_S;

typedef struct hiBITMAP_S
{
    PIXEL_FORMAT_E enPixelFormat;  /* Bitmap's pixel format */
    HI_U32 u32Width;               /* Bitmap's width */
    HI_U32 u32Height;              /* Bitmap's height */
    HI_VOID* pData;                /* Address of Bitmap's data */
} BITMAP_S;

/* VI Mix-Capture info. */
typedef struct hiVI_MIXCAP_STAT_S
{
    HI_BOOL bMixCapMode;    /* In mix-capture mode or not. */
    HI_BOOL bHasDownScale;  /* VI Frame is downscaled or not. */
} VI_MIXCAP_STAT_S;

/* VI output frame info. */
typedef struct hiVI_FRAME_INFO_S
{
    VI_MIXCAP_STAT_S stMixCapState; /* VI Mix-Capture info. */
    VIDEO_FRAME_INFO_S stViFrmInfo; /* Video frame info. */
} VI_FRAME_INFO_S;

typedef enum hiLDC_VIEW_TYPE_E
{
    LDC_VIEW_TYPE_ALL 	= 0,  	/* View scale all but x and y independtly, this will keep both x and y axis ,but corner maybe lost*/
    LDC_VIEW_TYPE_CROP	= 1,	/* Not use view scale, this will lost some side and corner */

    LDC_VIEW_TYPE_BUTT,
} LDC_VIEW_TYPE_E;

typedef struct hiLDC_ATTR_S
{
    LDC_VIEW_TYPE_E enViewType;
    HI_S32 s32CenterXOffset;        /* Horizontal offset of the image distortion center relative to image center. [-28,28]. */
    HI_S32 s32CenterYOffset;        /* Vertical offset of the image distortion center relative to image center. [-14,14]. */
    HI_S32 s32Ratio;                /* Distortion ratio. Hi3519 range [0, 500], other chip range [0, 511]. */
} LDC_ATTR_S;

typedef enum hiWDR_MODE_E
{
    WDR_MODE_NONE = 0,
    WDR_MODE_BUILT_IN,

    WDR_MODE_2To1_LINE,
    WDR_MODE_2To1_FRAME,
    WDR_MODE_2To1_FRAME_FULL_RATE,

    WDR_MODE_3To1_LINE,
    WDR_MODE_3To1_FRAME,
    WDR_MODE_3To1_FRAME_FULL_RATE,

    WDR_MODE_4To1_LINE,
    WDR_MODE_4To1_FRAME,
    WDR_MODE_4To1_FRAME_FULL_RATE,

    WDR_MODE_BUTT,
} WDR_MODE_E;

//typedef enum hiFISHEYE_DIVISION_MODE_E
//{
//    FISHEYE_180_MODE_UP   	= 0,
//    FISHEYE_180_MODE_LEFT		= 1,
//    FISHEYE_180_MODE_DOWN		= 2,
//    FISHEYE_180_MODE_RIGHT 	= 3,
//    FISHEYE_180_MODE_BUTT
//}FISHEYE_DIVISION_MODE_E;


typedef enum hiFISHEYE_MOUNT_MODE_E
{		
	
	FISHEYE_DESKTOP_MOUNT 	= 0,		/* desktop mount mode */
	FISHEYE_CEILING_MOUNT	= 1,		/* ceiling mount mode */
	FISHEYE_WALL_MOUNT   	= 2,		/* wall mount mode */
	
    FISHEYE_MOUNT_MODE_BUTT
}FISHEYE_MOUNT_MODE_E;


typedef enum hiFISHEYE_VIEW_MODE_E
{
	FISHEYE_VIEW_360_PANORAMA   = 0, 	/* 360 panorama mode of fisheye correction */
	FISHEYE_VIEW_180_PANORAMA	= 1,	/* 180 panorama mode of fisheye correction */	
	FISHEYE_VIEW_NORMAL   		= 2, 	/* normal mode of fisheye correction */
	FISHEYE_NO_TRANSFORMATION 	= 3, 	/* no fisheye correction */
	
    FISHEYE_VIEW_MODE_BUTT
}FISHEYE_VIEW_MODE_E;

typedef struct hiFISHEYE_REGION_ATTR_S
{
	FISHEYE_VIEW_MODE_E 	enViewMode;		/* fisheye view mode */
	HI_U32 					u32InRadius;    /* inner radius of fisheye correction region [0, u32OutRadius) */
	HI_U32 					u32OutRadius;   /* out radius of fisheye correction region [1, max(width/2 of input picture, height/2 of input picture)] */
	HI_U32 					u32Pan;			/* [0, 360] */
	HI_U32 					u32Tilt;		/* [0, 360] */
	HI_U32 					u32HorZoom;		/* [1, 4095] */
	HI_U32 					u32VerZoom;		/* [1, 4095] */
	RECT_S                  stOutRect; 		/* output image info after fisheye correction range of width [960, 4608], 
											   rang of height [360, 3456], rang of x [0, 4608), rang of y [0, 3456) */
}FISHEYE_REGION_ATTR_S;


typedef struct hiFISHEYE_ATTR_S
{
	HI_BOOL 				bEnable;			/* whether enable fisheye correction or not */
	HI_BOOL 				bLMF; 				/* whether fisheye len's LMF coefficient is from user config or from default linear config */
	HI_BOOL                 bBgColor;       	/* whether use background color or not */
	HI_U32                  u32BgColor;			/* the background color ARGB8888 [0, 0xFFFFFF] */

	HI_S32 					s32HorOffset;   	/* the horizontal offset between image center and physical center of len [-127, 127] */
	HI_S32 					s32VerOffset;		/* the vertical offset between image center and physical center of len [-127, 127] */
	
	HI_U32                  u32TrapezoidCoef;	/* strength coefficient of trapezoid correction */
	
	FISHEYE_MOUNT_MODE_E    enMountMode;		/* fisheye mount mode */	
	
	HI_U32               	u32RegionNum;       /* fisheye correction region number [1, FISHEYE_MAX_REGION_NUM] */
	FISHEYE_REGION_ATTR_S 	astFisheyeRegionAttr[FISHEYE_MAX_REGION_NUM];/* attribution of fisheye correction region */	 
}FISHEYE_ATTR_S;


typedef struct hiFISHEYE_CONFIG_S
{
	HI_U16               	au16LMFCoef[128]; 	/* LMF coefficient of fisheye len */
}FISHEYE_CONFIG_S;


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* _HI_COMM_VIDEO_H_ */

