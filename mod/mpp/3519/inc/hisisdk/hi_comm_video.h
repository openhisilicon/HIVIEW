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

typedef struct hiAREA_S
{
    HI_S32 s32StartX;
    HI_S32 s32StartY;
    HI_S32 s32EndX;
    HI_S32 s32EndY;
} AREA_S;
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
    PIC_2688x1944,/* 2688 * 1944 */
    PIC_6M,        /* 3072 * 2160 */
    PIC_UHD4K,     /* 3840 * 2160 */
    PIC_3Kx3K,      /* 3000 * 3000 */
	PIC_12M,     	/* 4000 * 3000 */
	PIC_16M,     	/* 4608 * 3456 */

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

    PIXEL_FORMAT_YUV_400,

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



typedef struct hiUSER_SUPPLEMENT_INFO_S
{
    HI_U32          u32PhyAddr;                                             /*The physical address from user*/
    HI_VOID*        pVirAddr;                                               /*The virtual address from user*/
    HI_U32          u32Size;                                                /*size of user memory.                                                                         If the numerator of the recorded value is 0, this indicates that digital zoom was not used.*/
} USER_SUPPLEMENT_INFO_S;


#define WDR_FRAME_NUM_MAX 4
typedef struct hiFRAME_SUPPLEMENT_INFO_S
{
    HI_U32      u32ISO[WDR_FRAME_NUM_MAX]; 								            /*ISP internal ISO : Again*Dgain*ISPgain*/
    HI_U32      u32ExposureTime[WDR_FRAME_NUM_MAX];                                    /*Exposure time (reciprocal of shutter speed).*/
    HI_U32      u32IspDgain[WDR_FRAME_NUM_MAX];
    HI_U32      u32IspNrStrength[WDR_FRAME_NUM_MAX];
    USER_SUPPLEMENT_INFO_S stUserSupplementInfo;
    HI_U32      u32FNumber;                                         /*The actual F-number (F-stop) of lens when the image was taken*/
    HI_U32      u32SensorID;
    HI_U32      u32SensorMode;
    HI_U32      u32VcNum;
} FRAME_SUPPLEMENT_INFO_S;

typedef enum hiFRAME_FLASH_TYPE_E
{
    FRAME_FLASH_OFF  = 0,
    FRAME_FLASH_ON   = 1,
    FRAME_FLASH_BUTT,
} FRAME_FLASH_TYPE_E;

#define FRAME_FLAG_FRAME_TYPE_MASK  (0xF << 8)
#define FRAME_FLAG_REF_TYPE_MASK    (0xF << 4)
#define FRAME_FLAG_FLASH_TYPE_MASK  (0x1 << 0)

typedef enum hiFRAME_STATE_E
{
    FRAME_FLAG_FLASH_OFF     = 0,
    FRAME_FLAG_FLASH_ON      = 1,
    FRAME_FLAG_SNAP_0REF_CUR     = 0x4 << 4,
    FRAME_FLAG_SNAP_2REF_REF = 0x2 << 4,
    FRAME_FLAG_SNAP_2REF_CUR  = 0x6 << 4,
    FRAME_FLAG_SNAP_NORMAL      = 0x1 << 8,
    FRAME_FLAG_SNAP_PRO      = 0x2 << 8,
	FRAME_FLAG_SNAP_END      = 0x1 << 31,
    FRAME_FLAG_BUTT
} FRAME_FLAG_E;

typedef enum hiSNAP_ISP_STATE_E
{
    SNAP_STATE_NULL = 0,
    SNAP_STATE_CFG  = 1,
    SNAP_STATE_EFF  = 2,
    SNAP_STATE_BUTT
} SNAP_ISP_STATE_E;

typedef struct hiSNAP_ISP_INFO_S
{
    HI_U32  u32Iso;
    HI_U32  u32IspDgain;
    HI_U32  u32ExposureTime;
    HI_U32  au32WhiteBalanceGain[4];
    HI_U32  u32ColorTemp;
    HI_U16  au16CapCCM[9];
    SNAP_ISP_STATE_E enCapState;
} ISP_CONFIG_INFO_S;

typedef enum hiSNAP_TYPE_E
{
    SNAP_TYPE_NONE = 0, /* video mode */
    SNAP_TYPE_NORMAL,   /* ZSL and none-ZSL type */
    SNAP_TYPE_PRO,      /* Professional type support HDR, AEB, LongExposure */
    SNAP_TYPE_USER,     /* USER SEND RAW type */
    SNAP_TYPE_SYNC,    /* Quick response snap mode, the parameters of the preview and picture ISP pipe must be set the same */
	SNAP_TYPE_SYNC_PRO,   /*  double pipe online pro snap mode */ 
    SNAP_TYPE_BUTT
} SNAP_TYPE_E;

typedef struct hiISP_FRAME_INFO_S
{
    HI_U32 u32Iso;
    HI_U32 u32ExposureTime;
} ISP_FRAME_INFO_S;

#define ISP_AE_ZONE_ROW                 (15)
#define ISP_AE_ZONE_COLUMN              (17)
#define ISP_AWB_ZONE_ROW                (32)
#define ISP_AWB_ZONE_COLUMN             (32)
#define ISP_AWB_ZONE_NUM                (ISP_AWB_ZONE_ROW * ISP_AWB_ZONE_COLUMN) 
#define ISP_BAYER_CHN   (4)
typedef struct hiISP_AE_STAT_INFO_S
{
	HI_U16	au16ZoneAvg[ISP_AE_ZONE_ROW][ISP_AE_ZONE_COLUMN][ISP_BAYER_CHN];	
}ISP_AE_STAT_INFO_S;
typedef struct hiISP_AWB_STAT_INFO_S
{
	HI_U16 au16ZoneAvgR[ISP_AWB_ZONE_NUM];            /*RO, Zone Average R, Range: [0x0, 0xFFFF]*/   
	HI_U16 au16ZoneAvgG[ISP_AWB_ZONE_NUM];            /*RO, Zone Average G, Range: [0x0, 0xFFFF]*/   
	HI_U16 au16ZoneAvgB[ISP_AWB_ZONE_NUM];            /*RO, Zone Average B, Range: [0x0, 0xFFFF]*/   
	HI_U16 au16ZoneCountAll[ISP_AWB_ZONE_NUM];        /*RO, normalized number of Gray points, Range: [0x0, 0xFFFF]*/   
	HI_U16 au16ZoneCountMin[ISP_AWB_ZONE_NUM];        /*RO, normalized number of pixels under BlackLevel, Range: [0x0, 0xFFFF]*/   
	HI_U16 au16ZoneCountMax[ISP_AWB_ZONE_NUM];        /*RO, normalized number of pixels above Whitelevel, Range: [0x0, 0xFFFF]*/ 
} ISP_AWB_STAT_INFO_S;
#define HIST_1024_NUM   (1024)
typedef struct hiISP_AE_HISTOGRAM_S
{    
    HI_U32  au32AeHistogramStat[HIST_1024_NUM];
} ISP_AE_HISTOGRAM_S;
typedef struct hiISP_RAW_STAT_INFO_S
{
	ISP_AE_STAT_INFO_S	stIspAeInfo;	
	ISP_AE_HISTOGRAM_S 	stAeHistogram;
	ISP_AWB_STAT_INFO_S	stIspAwbInfo;
} ISP_RAW_STAT_INFO_S;
typedef struct hiVIDEO_SUPPLEMENT_S
{
    HI_U32   enFlashType;
    HI_U32   u32JpegDcfPhyAddr;
    HI_VOID* pJpegDcfVirAddr;
    HI_U32   u32FrameSupplementPhyAddr;
    HI_VOID* pFrameSupplementVirAddr;

    HI_U64   u64RawPts;
    HI_U32   u32IspInfoPhyAddr;
    HI_VOID* pIspInfoVirAddr;
	HI_U32   u32IspStatPhyAddr;
	HI_VOID* pIspStatVirAddr;
    HI_U32   u32LowDelayPhyAddr;
    HI_VOID* pLowDelayVirAddr;
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
    HI_S32 s32CenterXOffset;        /* Horizontal offset of the image distortion center relative to image center.*/
    HI_S32 s32CenterYOffset;        /* Vertical offset of the image distortion center relative to image center.*/
    HI_S32 s32Ratio;                /* Distortion ratio.*/
	HI_S32 s32MinRatio;
} LDC_ATTR_S;

typedef enum hiROTATION_VIEW_TYPE_E
{
    ROTATION_VIEW_TYPE_ALL   = 0,    /* View all source Image,no lose*/
    ROTATION_VIEW_TYPE_TYPICAL  = 1, /* View from rotation Image with source size,same lose*/
    ROTATION_VIEW_TYPE_INSIDE  = 2,      /* View with no black section,all  in dest Image*/
    ROTATION_VIEW_TYPE_BUTT,
} ROTATION_VIEW_TYPE_E;

typedef struct hiROTATE_EX_S
{
    ROTATION_VIEW_TYPE_E enViewType;       /*Rotation mode*/
    HI_U32               u32Angle;         /*Rotation Angle:[0,360]*/
    HI_S32               s32CenterXOffset; /* Horizontal offset of the image distortion center relative to image center.*/
    HI_S32               s32CenterYOffset; /* Vertical offset of the image distortion center relative to image center.*/
    SIZE_S               stDestSize;       /*Dest size of any angle rotation*/
} ROTATE_EX_S;

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

typedef struct hiISP_DCF_UPDATE_INFO_S
{
    HI_U16      u16ISOSpeedRatings;                                 /*CCD sensitivity equivalent to Ag-Hr film speedrate*/
    HI_U32      u32ExposureTime;                                    /*Exposure time (reciprocal of shutter speed).*/
    HI_U32      u32ExposureBiasValue;                               /*Exposure bias (compensation) value of taking picture*/
    HI_U8       u8ExposureProgram;                                  /*Exposure program that the camera used when image was taken. '1' means manual control, '2'
                                                                              program normal, '3' aperture priority, '4' shutter priority, '5' program creative (slow program),
                                                                              '6' program action(high-speed program), '7' portrait mode, '8' landscape mode*/
    HI_U32      u32FNumber;                                         /*The actual F-number (F-stop) of lens when the image was taken*/
    HI_U32      u32MaxApertureValue;                                /*Maximum aperture value of lens.*/
    HI_U8       u8ExposureMode;                                     /*Indicates the exposure mode set when the image was shot.
                                                                              0 = Auto exposure,1 = Manual exposure, 2 = Auto bracket*/
    HI_U8       u8WhiteBalance;                                     /* Indicates the white balance mode set when the image was shot.
                                                                                0 = Auto white balance ,1 = Manual white balance */
} ISP_DCF_UPDATE_INFO_S;


typedef enum hiVB_REMAP_MODE_E
{
    VB_REMAP_MODE_NONE   	= 0,	                                /* no remap */
    VB_REMAP_MODE_NOCACHE 	= 1,	                                /* no cache remap */
    VB_REMAP_MODE_CACHED 	= 2,	                                /* cache remap, if you use this mode, you should flush cache by yourself */
    VB_REMAP_MODE_BUTT    
} VB_REMAP_MODE_E;


typedef struct hiVB_PARAM_MOD_S
{
    VB_REMAP_MODE_E enCommVbRemapMode;
}VB_MOD_PARAM_S;
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* _HI_COMM_VIDEO_H_ */

