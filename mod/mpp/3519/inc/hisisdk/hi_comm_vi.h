/******************************************************************************

Copyright (C), 2004-2020, Hisilicon Tech. Co., Ltd.

******************************************************************************
File Name     : hi_comm_vi.h
Version       : Initial Draft
Author        : Hisilicon multimedia software group
Created       : 2009/3/9
Last Modified :
Description   :
Function List :
History       :
1.Date        : 2010/11/16
  Author      : p00123320/w54723/n168968
  Modification: Created file

2.Date        : 2011/06/16
  Author      : w54723/l00181524/c00186004
  Modification: Created file


******************************************************************************/

#ifndef __HI_COMM_VI_H__
#define __HI_COMM_VI_H__

#include "hi_common.h"
#include "hi_errno.h"
#include "hi_comm_video.h"
#include "hi_comm_fisheye.h"


#ifdef __cplusplus
#if __cplusplus
extern "C"
{
#endif
#endif /* __cplusplus */

#define VI_INVALID_FRMRATE  (-1UL)
#define VIU_MAX_USER_FRAME_DEPTH 8
#define VIU_DEV_COMP_MASK_NUM         2
#define VIU_DEV_AD_CHN_NUM            4
#define VIU_DEV_VC_NUM                4
#define VIU_DEV_MAX_VC_ID             0xF

#define VIU_CSC_IDC_NUM               3
#define VIU_CSC_ODC_NUM               3
#define VIU_CSC_COEF_NUM              9




typedef enum hiEN_VIU_ERR_CODE_E
{
    ERR_VI_FAILED_NOTENABLE = 64,       /* device or channel not enable*/
    ERR_VI_FAILED_NOTDISABLE,           /* device not disable*/
    ERR_VI_FAILED_CHNOTDISABLE,         /* channel not disable*/
    ERR_VI_CFG_TIMEOUT,                 /* config timeout*/
    ERR_VI_NORM_UNMATCH,                /* video norm of ADC and VIU is unmatch*/
    ERR_VI_INVALID_WAYID,               /* invlalid way ID     */
    ERR_VI_INVALID_PHYCHNID,            /* invalid phychn id*/
    ERR_VI_FAILED_NOTBIND,              /* device or channel not bind */
    ERR_VI_FAILED_BINDED,               /* device or channel not unbind */
    ERR_VI_DIS_PROCESS_FAIL,			/* dis process failed */
} EN_VIU_ERR_CODE_E;

#define HI_ERR_VI_INVALID_PARA          HI_DEF_ERR(HI_ID_VIU, EN_ERR_LEVEL_ERROR, EN_ERR_ILLEGAL_PARAM)
#define HI_ERR_VI_INVALID_DEVID         HI_DEF_ERR(HI_ID_VIU, EN_ERR_LEVEL_ERROR, EN_ERR_INVALID_DEVID)
#define HI_ERR_VI_INVALID_CHNID         HI_DEF_ERR(HI_ID_VIU, EN_ERR_LEVEL_ERROR, EN_ERR_INVALID_CHNID)
#define HI_ERR_VI_INVALID_NULL_PTR      HI_DEF_ERR(HI_ID_VIU, EN_ERR_LEVEL_ERROR, EN_ERR_NULL_PTR)
#define HI_ERR_VI_FAILED_NOTCONFIG      HI_DEF_ERR(HI_ID_VIU, EN_ERR_LEVEL_ERROR, EN_ERR_NOT_CONFIG)
#define HI_ERR_VI_SYS_NOTREADY          HI_DEF_ERR(HI_ID_VIU, EN_ERR_LEVEL_ERROR, EN_ERR_SYS_NOTREADY)
#define HI_ERR_VI_BUF_EMPTY             HI_DEF_ERR(HI_ID_VIU, EN_ERR_LEVEL_ERROR, EN_ERR_BUF_EMPTY)
#define HI_ERR_VI_BUF_FULL              HI_DEF_ERR(HI_ID_VIU, EN_ERR_LEVEL_ERROR, EN_ERR_BUF_FULL)
#define HI_ERR_VI_NOMEM                 HI_DEF_ERR(HI_ID_VIU, EN_ERR_LEVEL_ERROR, EN_ERR_NOMEM)
#define HI_ERR_VI_NOT_SUPPORT           HI_DEF_ERR(HI_ID_VIU, EN_ERR_LEVEL_ERROR, EN_ERR_NOT_SUPPORT)
#define HI_ERR_VI_BUSY                  HI_DEF_ERR(HI_ID_VIU, EN_ERR_LEVEL_ERROR, EN_ERR_BUSY)
#define HI_ERR_VI_NOT_PERM              HI_DEF_ERR(HI_ID_VIU, EN_ERR_LEVEL_ERROR, EN_ERR_NOT_PERM)

#define HI_ERR_VI_FAILED_NOTENABLE      HI_DEF_ERR(HI_ID_VIU, EN_ERR_LEVEL_ERROR, ERR_VI_FAILED_NOTENABLE)/* 0xA0108040*/
#define HI_ERR_VI_FAILED_NOTDISABLE     HI_DEF_ERR(HI_ID_VIU, EN_ERR_LEVEL_ERROR, ERR_VI_FAILED_NOTDISABLE)/* 0xA0108041*/
#define HI_ERR_VI_FAILED_CHNOTDISABLE   HI_DEF_ERR(HI_ID_VIU, EN_ERR_LEVEL_ERROR, ERR_VI_FAILED_CHNOTDISABLE)/* 0xA0108042*/
#define HI_ERR_VI_CFG_TIMEOUT           HI_DEF_ERR(HI_ID_VIU, EN_ERR_LEVEL_ERROR, ERR_VI_CFG_TIMEOUT)/* 0xA0108043*/
#define HI_ERR_VI_NORM_UNMATCH          HI_DEF_ERR(HI_ID_VIU, EN_ERR_LEVEL_ERROR, ERR_VI_NORM_UNMATCH)/* 0xA0108044*/
#define HI_ERR_VI_INVALID_WAYID         HI_DEF_ERR(HI_ID_VIU, EN_ERR_LEVEL_ERROR, ERR_VI_INVALID_WAYID)/* 0xA0108045*/
#define HI_ERR_VI_INVALID_PHYCHNID      HI_DEF_ERR(HI_ID_VIU, EN_ERR_LEVEL_ERROR, ERR_VI_INVALID_PHYCHNID)/* 0xA0108046*/
#define HI_ERR_VI_FAILED_NOTBIND        HI_DEF_ERR(HI_ID_VIU, EN_ERR_LEVEL_ERROR, ERR_VI_FAILED_NOTBIND)/* 0xA0108047*/
#define HI_ERR_VI_FAILED_BINDED         HI_DEF_ERR(HI_ID_VIU, EN_ERR_LEVEL_ERROR, ERR_VI_FAILED_BINDED)/* 0xA0108048*/

#define HI_ERR_VI_DIS_PROCESS_FAIL      HI_DEF_ERR(HI_ID_VIU, EN_ERR_LEVEL_ERROR, ERR_VI_DIS_PROCESS_FAIL)

/*get the subchn index by main chn */
#define  SUBCHN(ViChn)   (ViChn + 16)

/* define cascade chn */
#define VI_CAS_CHN_1   32
#define VI_CAS_CHN_2   33


/* interface mode of video input */
typedef enum hiVI_INTF_MODE_E
{
    VI_MODE_BT656 = 0,              /* ITU-R BT.656 YUV4:2:2 */
    VI_MODE_BT601,                  /* ITU-R BT.601 YUV4:2:2 */
    VI_MODE_DIGITAL_CAMERA,         /* digatal camera mode */
    VI_MODE_BT1120_STANDARD,        /* BT.1120 progressive mode */
    VI_MODE_BT1120_INTERLEAVED,     /* BT.1120 interstage mode */
    VI_MODE_MIPI,                   /*MIPI mode*/
    VI_MODE_LVDS,
    VI_MODE_HISPI, 

    VI_MODE_BUTT
} VI_INTF_MODE_E;


/* Input mode */
typedef enum hiVI_INPUT_MODE_E
{
    VI_INPUT_MODE_BT656 = 0,        /* ITU-R BT.656 YUV4:2:2 */
    VI_INPUT_MODE_BT601,            /* ITU-R BT.601 YUV4:2:2 */
    VI_INPUT_MODE_DIGITAL_CAMERA,   /* digatal camera mode */
    VI_INPUT_MODE_INTERLEAVED,
    VI_INPUT_MODE_MIPI,             /*MIPI mode*/
    VI_INPUT_MODE_LVDS,
    VI_INPUT_MODE_HISPI,

    VI_INPUT_MODE_BUTT
} VI_INPUT_MODE_E;

typedef enum hiVI_WORK_MODE_E
{
    VI_WORK_MODE_1Multiplex = 0,    /* 1 Multiplex mode */
    VI_WORK_MODE_2Multiplex,        /* 2 Multiplex mode */
    VI_WORK_MODE_4Multiplex,        /* 4 Multiplex mode */

    VI_WORK_MODE_BUTT
} VI_WORK_MODE_E;



/* whether an input picture is interlaced or progressive */
typedef enum hiVI_SCAN_MODE_E
{
    VI_SCAN_INTERLACED  = 0,
    VI_SCAN_PROGRESSIVE,

    VI_SCAN_BUTT,
} VI_SCAN_MODE_E;

typedef enum hiVI_DATA_YUV_SEQ_E
{
   /*The input sequence of the second component(only contains u and v) in BT.1120 mode */
    VI_INPUT_DATA_VUVU = 0,
    VI_INPUT_DATA_UVUV,

   /* The input sequence for yuv */
    VI_INPUT_DATA_UYVY,
    VI_INPUT_DATA_VYUY,
    VI_INPUT_DATA_YUYV,
    VI_INPUT_DATA_YVYU,

    VI_DATA_YUV_BUTT
} VI_DATA_YUV_SEQ_E;

typedef enum hiVI_CLK_EDGE_E
{
    VI_CLK_EDGE_SINGLE_UP = 0,         /* single-edge mode and in rising edge */
    VI_CLK_EDGE_SINGLE_DOWN,           /* single-edge mode and in falling edge */
    //VI_CLK_EDGE_DOUBLE ,                 /* Double edge mode */

    VI_CLK_EDGE_BUTT
} VI_CLK_EDGE_E;

typedef enum hiVI_COMP_MODE_E
{
    VI_COMP_MODE_SINGLE = 0,           /* in single component mode */
    VI_COMP_MODE_DOUBLE = 1,           /* in double component mode */
    VI_COMP_MODE_BUTT,
}VI_COMP_MODE_E;

/* Y/C composite or separation mode */
typedef enum hiVI_COMBINE_MODE_E
{
    VI_COMBINE_COMPOSITE = 0,     /* Composite mode */
    VI_COMBINE_SEPARATE,          /* Separate mode */
    VI_COMBINE_BUTT,
} VI_COMBINE_MODE_E;

/* Attribute of the vertical synchronization signal */
typedef enum hiVI_VSYNC_E
{
    VI_VSYNC_FIELD = 0,           /* Field/toggle mode:a signal reversal means a new frame or a field */
    VI_VSYNC_PULSE,               /* Pusle/effective mode:a pusle or an effective signal means a new frame or a field */
} VI_VSYNC_E;

/* Polarity of the vertical synchronization signal */
typedef enum hiVI_VSYNC_NEG_E
{
    VI_VSYNC_NEG_HIGH = 0,        /*if VIU_VSYNC_E = VIU_VSYNC_FIELD,then the vertical synchronization signal of even field is high-level,
                                        if VIU_VSYNC_E = VIU_VSYNC_PULSE,then the vertical synchronization pulse is positive pulse.*/
    VI_VSYNC_NEG_LOW              /*if VIU_VSYNC_E = VIU_VSYNC_FIELD,then the vertical synchronization signal of even field is low-level,
                                        if VIU_VSYNC_E = VIU_VSYNC_PULSE,then the vertical synchronization pulse is negative pulse.*/
} VI_VSYNC_NEG_E;

/* Attribute of the horizontal synchronization signal */
typedef enum hiVI_HSYNC_E
{
    VI_HSYNC_VALID_SINGNAL = 0,   /* the horizontal synchronization is valid signal mode */
    VI_HSYNC_PULSE,               /* the horizontal synchronization is pulse mode, a new pulse means the beginning of a new line */
} VI_HSYNC_E;

/* Polarity of the horizontal synchronization signal */
typedef enum hiVI_HSYNC_NEG_E
{
    VI_HSYNC_NEG_HIGH = 0,        /*if VI_HSYNC_E = VI_HSYNC_VALID_SINGNAL,then the valid horizontal synchronization signal is high-level;
                                        if VI_HSYNC_E = VI_HSYNC_PULSE,then the horizontal synchronization pulse is positive pulse */
    VI_HSYNC_NEG_LOW              /*if VI_HSYNC_E = VI_HSYNC_VALID_SINGNAL,then the valid horizontal synchronization signal is low-level;
                                        if VI_HSYNC_E = VI_HSYNC_PULSE,then the horizontal synchronization pulse is negative pulse */
} VI_HSYNC_NEG_E;

/* Attribute of the valid vertical synchronization signal */
typedef enum hiVI_VSYNC_VALID_E
{
    VI_VSYNC_NORM_PULSE = 0,      /* the vertical synchronization is pusle mode, a pusle means a new frame or field  */
    VI_VSYNC_VALID_SINGAL,        /* the vertical synchronization is effective mode, a effective signal means a new frame or field */
} VI_VSYNC_VALID_E;

/* Polarity of the valid vertical synchronization signal */
typedef enum hiVI_VSYNC_VALID_NEG_E
{
    VI_VSYNC_VALID_NEG_HIGH = 0,  /*if VI_VSYNC_VALID_E = VI_VSYNC_NORM_PULSE,a positive pulse means vertical synchronization pulse;
                                        if VI_VSYNC_VALID_E = VI_VSYNC_VALID_SINGAL,the valid vertical synchronization signal is high-level */
    VI_VSYNC_VALID_NEG_LOW        /*if VI_VSYNC_VALID_E = VI_VSYNC_NORM_PULSE,a negative pulse means vertical synchronization pulse;
                                        if VI_VSYNC_VALID_E = VI_VSYNC_VALID_SINGAL,the valid vertical synchronization signal is low-level */
} VI_VSYNC_VALID_NEG_E;



/* Blank information of the input timing */
typedef struct hiVI_TIMING_BLANK_S
{
    HI_U32 u32HsyncHfb ;    /* Horizontal front blanking width */
    HI_U32 u32HsyncAct ;    /* Horizontal effetive width */
    HI_U32 u32HsyncHbb ;    /* Horizontal back blanking width */
    HI_U32 u32VsyncVfb ;    /* Vertical front blanking height of one frame or odd-field frame picture */
    HI_U32 u32VsyncVact ;   /* Vertical effetive width of one frame or odd-field frame picture */
    HI_U32 u32VsyncVbb ;    /* Vertical back blanking height of one frame or odd-field frame picture */
    HI_U32 u32VsyncVbfb ;   /* Even-field vertical front blanking height when input mode is interlace (invalid when progressive input mode) */
    HI_U32 u32VsyncVbact ;  /* Even-field vertical effetive width when input mode is interlace (invalid when progressive input mode) */
    HI_U32 u32VsyncVbbb ;   /* Even-field vertical back blanking height when input mode is interlace (invalid when progressive input mode) */
}VI_TIMING_BLANK_S;

/* synchronization information about the BT.601 or DC timing */
typedef struct hiVI_SYNC_CFG_S
{
    VI_VSYNC_E             enVsync;
    VI_VSYNC_NEG_E         enVsyncNeg;
    VI_HSYNC_E             enHsync;
    VI_HSYNC_NEG_E         enHsyncNeg;
    VI_VSYNC_VALID_E       enVsyncValid;
    VI_VSYNC_VALID_NEG_E   enVsyncValidNeg;
    VI_TIMING_BLANK_S      stTimingBlank;
} VI_SYNC_CFG_S;

/* the highest bit of the BT.656 timing reference code*/
typedef enum hiBT656_FIXCODE_E
{
    BT656_FIXCODE_1 = 0,       /* The highest bit of the EAV/SAV data over the BT.656 protocol is always 1.*/
    BT656_FIXCODE_0            /* The highest bit of the EAV/SAV data over the BT.656 protocol is always 0.*/
}BT656_FIXCODE_E;

/* Polarity of the field indicator bit (F) of the BT.656 timing reference code */
typedef enum hiBT656_FIELD_POLAR_E
{
    BT656_FIELD_POLAR_STD = 0, /* the standard BT.656 mode,the first filed F=0,the second filed F=1*/
    BT656_FIELD_POLAR_NSTD     /* the non-standard BT.656 mode,the first filed F=1,the second filed F=0*/
}BT656_FIELD_POLAR_E;

typedef struct hiVI_BT656_SYNC_CFG_S
{
    BT656_FIXCODE_E           enFixCode;
    BT656_FIELD_POLAR_E         enFieldPolar;
}VI_BT656_SYNC_CFG_S;

typedef enum hiVI_DATA_TYPE_E
{
    VI_DATA_TYPE_YUV = 0,
    VI_DATA_TYPE_RGB = 1,
    VI_DATA_TYPE_BUTT
} VI_DATA_TYPE_E;

typedef enum hiVI_DATA_PATH_E
{
    VI_PATH_BYPASS    = 0,        /* ISP bypass */
    VI_PATH_ISP       = 1,        /* ISP enable */
    VI_PATH_RAW       = 2,        /* Capture raw data, for debug */
    VI_PATH_BUTT
}VI_DATA_PATH_E;

typedef enum hiVI_REPHASE_MODE_E   
{
	VI_REPHASE_MODE_NONE 		= 0,
	VI_REPHASE_MODE_SKIP_1_2	= 1,		/*skip 1/2*/
	VI_REPHASE_MODE_SKIP_1_3	= 2,			/* skip 1/3 */
	VI_REPHASE_MODE_BINNING_1_2	= 3,  		/* binning 1/2*/
	VI_REPHASE_MODE_BINNING_1_3	= 4,		/* binning 1/3*/
	VI_REPHASE_MODE_BUTT
}VI_REPHASE_MODE_E;
typedef struct hiVI_BAS_REPHASE_ATTR_S
{	
	VI_REPHASE_MODE_E   enHRephaseMode;
	VI_REPHASE_MODE_E   enVRephaseMode;	
} VI_BAS_REPHASE_ATTR_S;
typedef struct hiVI_BAS_SCALE_ATTR_S
{
	SIZE_S				stBasSize; 			/* bayer scale size.*/
	HI_BOOL 			bCompress;			/*bayer scale compress.*/
} VI_BAS_SCALE_ATTR_S;
typedef struct hiVI_BAS_ATTR_S
{
	VI_BAS_SCALE_ATTR_S stSacleAttr;
	VI_BAS_REPHASE_ATTR_S stRephaseAttr;
} VI_BAS_ATTR_S;
/* the extended attributes of VI device */
typedef struct hiVI_DEV_ATTR_EX_S
{
    VI_INPUT_MODE_E     enInputMode;        /* Input mode */
    VI_WORK_MODE_E      enWorkMode;         /*1-, 2-, or 4-channel multiplexed work mode */

    VI_COMBINE_MODE_E   enCombineMode;      /* Y/C composite or separation mode */
    VI_COMP_MODE_E      enCompMode;         /* Component mode (single-component or dual-component) */
    VI_CLK_EDGE_E       enClkEdge;          /* Clock edge mode (sampling on the rising or falling edge) */

    HI_U32              au32CompMask[VIU_DEV_COMP_MASK_NUM];    /* Component mask */

    VI_SCAN_MODE_E      enScanMode;         /* Input scanning mode (progressive or interlaced) */
    HI_S32              s32AdChnId[VIU_DEV_AD_CHN_NUM];      /* AD channel ID. Typically, the default value -1 is recommended */

    VI_DATA_YUV_SEQ_E   enDataSeq;          /* Input data sequence (only the YUV format is supported) */
    VI_SYNC_CFG_S       stSynCfg;           /* Sync timing. This member must be configured in BT.601 mode or DC mode */

    VI_BT656_SYNC_CFG_S stBT656SynCfg;      /* Sync timing. This member must be configured in BT.656 mode */

    VI_DATA_PATH_E      enDataPath;         /* ISP enable or bypass */
    VI_DATA_TYPE_E      enInputDataType;    /* RGB: CSC-709 or CSC-601, PT YUV444 disable; YUV: default yuv CSC coef PT YUV444 enable. */

    HI_BOOL             bDataRev;           /* Data reverse */
    
    RECT_S              stDevRect;          /* Dev capture rect */
	VI_BAS_ATTR_S       stBasAttr;           /*BAS*/	
} VI_DEV_ATTR_EX_S;

/* the attributes of a VI device */
typedef struct hiVI_DEV_ATTR_S
{
    VI_INTF_MODE_E      enIntfMode;         /* Interface mode */
    VI_WORK_MODE_E      enWorkMode;         /*1-, 2-, or 4-channel multiplexed work mode */

    HI_U32              au32CompMask[VIU_DEV_COMP_MASK_NUM];    /* Component mask */
    VI_SCAN_MODE_E      enScanMode;         /* Input scanning mode (progressive or interlaced) */
    HI_S32              s32AdChnId[VIU_DEV_AD_CHN_NUM];      /* AD channel ID. Typically, the default value -1 is recommended */

    /* The below members must be configured in BT.601 mode or DC mode and are invalid in other modes */
    VI_DATA_YUV_SEQ_E   enDataSeq;          /* Input data sequence (only the YUV format is supported) */
    VI_SYNC_CFG_S       stSynCfg;           /* Sync timing. This member must be configured in BT.601 mode or DC mode */

    VI_DATA_PATH_E      enDataPath;         /* ISP enable or bypass */
    VI_DATA_TYPE_E      enInputDataType;    /* RGB: CSC-709 or CSC-601, PT YUV444 disable; YUV: default yuv CSC coef PT YUV444 enable. */

    HI_BOOL             bDataRev;           /* Data reverse */

    RECT_S              stDevRect;          /* Dev capture rect */
    VI_BAS_ATTR_S       stBasAttr;           /*BAS*/	
} VI_DEV_ATTR_S;


typedef struct hiVI_CHN_BIND_ATTR_S
{
    VI_DEV ViDev;
    VI_WAY ViWay;
} VI_CHN_BIND_ATTR_S;


/* captrue selection of video input */
typedef enum hiVI_CAPSEL_E
{
    VI_CAPSEL_TOP = 0,                  /* top field */
    VI_CAPSEL_BOTTOM,                   /* bottom field */
    VI_CAPSEL_BOTH,                     /* top and bottom field */
    VI_CAPSEL_BUTT
} VI_CAPSEL_E;
typedef struct hiVI_DEV_BIND_ATTR_S
{
	MIPI_DEV s32MipiDev;
}VI_DEV_BIND_ATTR_S;


/* the attributes of a VI channel */
typedef struct hiVI_CHN_ATTR_S
{
    RECT_S          stCapRect;          /* the capture rect (corresponding to the size of the picture captured by a VI device).
                                                For primary channels, the stCapRect's u32Width and u32Height are static attributes. That is,
                                                the value of them can be changed only after primary and secondary channels are disabled.
                                                For secondary channels, stCapRect is an invalid attribute */
    SIZE_S          stDestSize;         /* Target picture size.
                                                For primary channels, stDestSize must be equal to stCapRect's u32Width and u32Height,
                                                because primary channel doesn't have scale capability. Additionally, it is a static
                                                attribute, That is, the value of stDestSize can be changed only after primary and
                                                secondary channels are disabled.
                                                For secondary channels, stDestSize is a dynamic attribute */

    VI_CAPSEL_E     enCapSel;           /* Frame/field select. It is used only in interlaced mode.
                                                For primary channels, enCapSel is a static attribute */
    PIXEL_FORMAT_E  enPixFormat;        /* Pixel storage format. Only the formats semi-planar420 and semi-planar422 are supported */

    COMPRESS_MODE_E enCompressMode;     /* 256B Segment compress or no compress. */

    HI_BOOL         bMirror;            /* Whether to mirror */
    HI_BOOL         bFlip;              /* Whether to flip */
    HI_S32          s32SrcFrameRate;    /* Source frame rate. The value -1 indicates that the frame rate is not controlled */
    HI_S32          s32DstFrameRate;    /* Target frame rate. The value -1 indicates that the frame rate is not controlled */
} VI_CHN_ATTR_S;


typedef struct hiVI_CHN_STAT_S
{
    HI_BOOL bEnable;                    /* Whether this channel is enabled */
    HI_U32 u32IntCnt;                   /* The video frame interrupt count */
    HI_U32 u32FrmRate;                  /* current frame rate */
    HI_U32 u32LostInt;                  /* The interrupt is received but nobody care */
    HI_U32 u32VbFail;                   /* Video buffer malloc failure */
    HI_U32 u32PicWidth;                 /* curren pic width */
    HI_U32 u32PicHeight;                /* current pic height */
} VI_CHN_STAT_S;

typedef enum hi_VI_USERPIC_MODE_E
{
    VI_USERPIC_MODE_PIC = 0,            /* YUV picture */
    VI_USERPIC_MODE_BGC,                /* Background picture only with a color */
    VI_USERPIC_MODE_BUTT,
} VI_USERPIC_MODE_E;

typedef struct hiVI_USERPIC_BGC_S
{
    HI_U32          u32BgColor;
} VI_USERPIC_BGC_S;

typedef struct hiVI_USERPIC_ATTR_S
{
    HI_BOOL                 bPub;          /* Whether the user picture information is shared by all VI devices and channels*/
    VI_USERPIC_MODE_E       enUsrPicMode;  /* User picture mode */
    union
    {
        VIDEO_FRAME_INFO_S  stUsrPicFrm;   /* Information about a YUV picture */
        VI_USERPIC_BGC_S    stUsrPicBg;    /* Information about a background picture only with a color */
    }unUsrPic;
} VI_USERPIC_ATTR_S;

typedef enum hiVI_FLASH_MODE_E
{
    VI_FLASH_ONCE = 0,		            /* Flash one time */
    VI_FLASH_FREQ = 1,                  /* Flash frequently */
    VI_FLASH_MODE_BUTT
}VI_FLASH_MODE_E;

typedef struct hiVI_FlASH_CONFIG_S
{
    VI_FLASH_MODE_E enFlashMode;	    /* Flash one time, flash frequently*/

    HI_U32 u32StartTime;	            /* Flash start time,unit: sensor pix clk.*/
    HI_U32 u32Duration;                 /* Flash high duration, unit: sensor pix clk.*/
    HI_U32 u32CapFrmIndex;              /* Set which vframe will be bFlashed after flashing, default is 0. */
    HI_U32 u32Interval;                 /* Flash frequently interval, unit: frame*/
}VI_FLASH_CONFIG_S;

typedef struct hiVI_EXT_CHN_ATTR_S
{
    VI_CHN    s32BindChn;		        /* The channel num which extend channel will bind to*/
    SIZE_S    stDestSize;		        /* Target size*/

    HI_S32    s32SrcFrameRate;          /* Source frame rate. The value -1 indicates that the frame rate is not controlled */
    HI_S32    s32DstFrameRate;             /* Target frame rate. The value -1 indicates that the frame rate is not controlled */
    PIXEL_FORMAT_E  enPixFormat;        /* Pixel storage format. Only the formats semi-planar420 and semi-planar422 are supported */
    COMPRESS_MODE_E enCompressMode;
}VI_EXT_CHN_ATTR_S;


typedef struct hiVI_LDC_ATTR_S
{
    HI_BOOL bEnable;                    /* Whether LDC is enbale */
    LDC_ATTR_S stAttr;                  /* LDC Attribute */
}VI_LDC_ATTR_S;

typedef struct hiVI_ROTATE_EX_ATTR_S
{
    HI_BOOL   bEnable;                 /* Whether ROTATE_EX_S is enbale */
    ROTATE_EX_S stRotateEx;                  /* Rotate Attribute */
}VI_ROTATE_EX_ATTR_S;


typedef struct hiVI_CYLIND_ATTR_S
{
    HI_BOOL bEnable;                    /* Whether CYLIND is enbale */
    FISHEYE_CYLIND_ATTR_S stFisheyeCylindAttr;                  /*CYLIND Attribute */
}VI_CYLIND_ATTR_S;

#define PMF_COEF_MAX_NUM    9
typedef struct hiVI_PMF_ATTR_S
{
	HI_BOOL     				bEnable;	
	HI_S32      				as32PMFCoef[PMF_COEF_MAX_NUM];
	SIZE_S      				stDestSize;
}VI_PMF_ATTR_S;
typedef struct hiVI_STITCH_CORRECTION_ATTR_S
{
	VI_LDC_ATTR_S				stLdcAttr;
	VI_CYLIND_ATTR_S			stCylindAttr;
	VI_PMF_ATTR_S				stPMFAttr;
}VI_STITCH_CORRECTION_ATTR_S;

typedef struct hiVI_CHN_LUM_S
{
    HI_U32 u32Lum;                      /* Luma sum of current frame */
    HI_U64 u64Pts;                      /* PTS of current frame */
} VI_CHN_LUM_S;

typedef enum hiVI_CSC_TYPE_E
{
    VI_CSC_TYPE_601 = 0,                /* CSC Type: 601 */
    VI_CSC_TYPE_709,                    /* CSC Type: 709 */
    VI_CSC_TYPE_USER,                   /* CSC Type: user define */
    VI_CSC_TYPE_BUTT,
} VI_CSC_TYPE_E;

typedef struct hiVI_CSC_MATRIX_S
{
    HI_S32 s32CSCIdc[VIU_CSC_IDC_NUM];      /* input R G B direct current component offset: [-256~255] */
    HI_S32 s32CSCOdc[VIU_CSC_ODC_NUM];      /* output Y U V direct current component offset: [-256~255] */
    HI_S32 s32CSCCoef[VIU_CSC_COEF_NUM];    /* convert coefficient matrix: [-16384, 16383] */
}VI_CSC_MATRIX_S;

typedef struct hiVI_CSC_ATTR_S
{
    VI_CSC_TYPE_E enViCscType;          /* 601 or 709 */
    HI_U32 u32LumaVal;                  /* Luminance: [0 ~ 100] */
    HI_U32 u32ContrVal;                 /* Contrast: [0 ~ 100] */
    HI_U32 u32HueVal;                   /* Hue: [0 ~ 100] */
    HI_U32 u32SatuVal;                  /* Satuature: [0 ~ 100] */
    HI_BOOL bTVModeEn;                  /*TV Mode: true or false*/       
    VI_CSC_MATRIX_S stCSCMatrix;        /* The coefficient matrix of CSC */
} VI_CSC_ATTR_S;

#define RAW_READ_MAX_FRAME_NUM    30
typedef struct 
{
	VIDEO_FRAME_INFO_S stFrame;
}VI_RAW_DATA_INFO_S;

typedef struct hiVI_DCI_PARAM_S
{
    HI_BOOL     bEnable;
    HI_U32      u32BlackGain;       /*u32BlackGain : [0, 63]*/
    HI_U32      u32ContrastGain;    /*u32ContrastGain : [0, 63]*/
    HI_U32      u32LightGain;       /*u32LightGain : [0, 63]*/
    HI_BOOL     bManBlendEn;        /*0:auto blend; 1: manual blend*/
    HI_U32      u32BlendRatio;      /*blend ratio: [0, 127]*/
    HI_BOOL     bBlackStretchEn;    /*black stretch*/
} VI_DCI_PARAM_S;

typedef struct hiVI_ISP_WDR_ATTR_S
{
    WDR_MODE_E  enWDRMode;
    HI_BOOL bCompress;
}VI_WDR_ATTR_S;

typedef struct hiVI_VC_NUMBER_S
{
    HI_U32 au32VCNumber[VIU_DEV_VC_NUM];
}VI_VC_NUMBER_S;

typedef enum hiVI_DUMP_TYPE_E
{
    VI_DUMP_TYPE_RAW   = 0,
    VI_DUMP_TYPE_IR    = 1,
    VI_DUMP_TYPE_YUV   = 2,
    VI_DUMP_TYPE_RGB   = 3,
    VI_DUMP_TYPE_BUTT
}VI_DUMP_TYPE_E;

typedef enum hiVI_DUMP_SEL_E
{
	VI_DUMP_SEL_PORT   = 0,
	VI_DUMP_SEL_BAS    = 1,  
	VI_DUMP_SEL_BUTT
}VI_DUMP_SEL_E;

typedef struct hiVI_DUMP_ATTR_S
{
    VI_DUMP_TYPE_E enDumpType;
    PIXEL_FORMAT_E enPixelFormat;
    CROP_INFO_S    stCropInfo;
	VI_DUMP_SEL_E  enDumpSel;
}VI_DUMP_ATTR_S;

typedef enum hiVI_DIS_ACCURACY_E
{
	VI_DIS_ACCURACY_HIGH,
	VI_DIS_ACCURACY_MIDDLE,
	VI_DIS_ACCURACY_LOW,
	
	VI_DIS_ACCURACY_BUTT
}VI_DIS_ACCURACY_E;

typedef enum hiVI_DIS_CAMERA_MODE_E
{
	VI_DIS_CAMERA_MODE_NORMAL,
	VI_DIS_CAMERA_MODE_IPC,
	
	VI_DIS_CAMERA_MODE_BUTT	
}VI_DIS_CAMERA_MODE_E;

typedef enum hiVI_DIS_MOTION_TYPE_E
{
    VI_DIS_MOTION_4DOF_SOFT,
    VI_DIS_MOTION_6DOF_SOFT,
    VI_DIS_MOTION_6DOF_HYBRID,
    VI_DIS_MOTION_8DOF_HARD,

    VI_DIS_MOTION_BUTT,
}VI_DIS_MOTION_TYPE_E;

typedef enum hiVI_DIS_ANGLE_TYPE_E
{
    VI_DIS_ANGLE_TYPE_HORIZONTAL,
    VI_DIS_ANGLE_TYPE_VERTICAL,
    VI_DIS_ANGLE_TYPE_DIAGONAL,

    VI_DIS_ANGLE_TYPE_BUTT,
}VI_DIS_ANGLE_TYPE_E;

typedef struct hiVI_DIS_CONFIG_S
{
	VI_DIS_ACCURACY_E		enAccuracy;					/* Accuracy setting of motion detection */
	VI_DIS_CAMERA_MODE_E    enCameraMode;				/* DIS Camera mode */
	VI_DIS_MOTION_TYPE_E    enMotionType;				/* DIS Motion Type*/
	HI_U32  				u32FixLevel;				/* Level of how strong the screen is fixated. range:[0~7] */	
	HI_S32  				s32RollingShutterCoef;		/* Rolling shutter distortion correction coefficient. range:[-100~100]*/
	HI_U32					u32BufNum;					/* Buf num for DIS ,range:[5~8]*/
	HI_U32					u32CropRatio;				/* crop ratio of output image,[50~98]*/
	HI_S32					s32FrameRate;				/* the output framerate,[1~120]*/
	HI_BOOL					bScale;						/* Scale output image */
	HI_U32					u32DelayFrmNum;				/* delay frame number*/
	HI_U32					u32RetCenterStrength;		/* The Strength for returning the screen to the center position.*/
    HI_U32					u32GyroWeight;				/* the weight that depend on gyro data*/
}VI_DIS_CONFIG_S;

typedef struct hiVI_DIS_ATTR_S
{
    HI_BOOL         		bEnable;                    /* DIS enable */
    HI_U32          		u32MovingSubjectLevel;      /* Threshold level to determine moving subject in the screen. range:[0~7] */
    HI_U32          		u32NoMovementLevel;         /* Threshold level to determine that the screen is still. range:[0~7] */
    HI_U32          		u32TimeLag;                 /* Time lag between Frame and gyro data*/
    VI_DIS_ANGLE_TYPE_E     enAngleType;                /* View angle type [0~2] HORIZONTAL = 0, VERTICAL = 1, DIAGONAL = 2,*/
    HI_U32                  u32Vangle;                  /* View angle,[100,1380]*/
    HI_BOOL      			bStillCrop;					/* the stabilization will be not working ,but the output image still be cropped*/
}VI_DIS_ATTR_S;

typedef struct hiVI_DIS_GYRO_DATA_S
{
	HI_DOUBLE     *pdRotX;		/* Pointer to the x-axis angler velocity array */
	HI_DOUBLE     *pdRotY;		/* Pointer to the y-axis angler velocity array */
	HI_DOUBLE     *pdRotZ;		/* Pointer to the z-axis angler velocity array */
	HI_S64    	  *ps64Time;	/* Pointer to the time stamp array */
	HI_U32    	   u32Num;		/* Number of valid data */
}VI_DIS_GYRO_DATA_S;

typedef struct hiVI_DIS_CALLBACK_S
{
   HI_S32 (*pfnGetGyroDataCallback)(HI_U64 u64BeginPts , HI_U64 u64EndPts, VI_DIS_GYRO_DATA_S* pstDISGyroData);
   HI_S32 (*pfnReleaseGyroDataCallback)(HI_VOID);	
}VI_DIS_CALLBACK_S;

typedef struct hiVI_MOD_PARAM_S
{
    HI_BOOL bLumaExtendEn;      /*whether to use extended brightness adjustment range -128-128*/
    HI_BOOL bContrastModeEn;    /*whether brightness changing with contrast changes.*/
}VI_MOD_PARAM_S;
typedef struct hiVI_SNAP_NORMAL_ATTR_S
{
    /* -------- statical attrs -------- */
    HI_U32  u32FrameDepth;   /* buffer depth, range:[2,8] */
    HI_S32  s32SrcFrameRate; /* for raw frame rate control */
    HI_S32  s32DstFrameRate;
    HI_BOOL bZSL;

    /* -------- dynamical attrs -------- */
    HI_U32  u32RollbackMs;   /* rollback time, unit(ms), invalid when bZSL is HI_FALSE  */
    HI_U32  u32Interval;     /* for continuous capture, select frame every u32Intercal frames */
    HI_U32  u32FrameCnt;     /* set capture frame counts */
}VI_SNAP_NORMAL_ATTR_S;

typedef struct hiVI_SNAP_HDR_ATTR_S
{
    /* -------- statical attrs -------- */
    HI_U32  u32FrameDepth;  /* buffer depth, range:[2,8]*/
}VI_SNAP_PRO_ATTR_S;

typedef struct hiVI_SNAP_USER_ATTR_S
{
    HI_U32  u32FrameDepth;   /* buffer depth, range:[1,8] */
    HI_S32  s32SrcFrameRate; /* for raw frame rate control */
    HI_S32  s32DstFrameRate;   
}VI_SNAP_USER_ATTR_S;

typedef struct hiVI_SNAP_ATTR_S
{
    SNAP_TYPE_E enSnapType;
    ISP_DEV     IspDev;         /* which ISP is used */
    HI_U32      u32RefFrameNum; /* referencial fame numbers, must be in 
                                   accordance with 3NDR's referencial frame numbers */
    union
    {
        VI_SNAP_NORMAL_ATTR_S   stNormalAttr;
        VI_SNAP_PRO_ATTR_S      stProAttr;
		VI_SNAP_USER_ATTR_S     stUserAttr;
    }unAttr;
}VI_SNAP_ATTR_S;

typedef struct hiVI_RAW_FRAME_INFO_S
{
    VIDEO_FRAME_INFO_S 		stFrame;	
    ISP_CONFIG_INFO_S    	stIspInfo;  
	ISP_DCF_UPDATE_INFO_S  	stIspUpdateInfo;
}VI_RAW_FRAME_INFO_S;

typedef struct hiVI_BAYER_READ_ATTR_S
{
   HI_BOOL bGenTiming;
   HI_S32  s32FrmRate;
}VI_BAYER_READ_ATTR_S;

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* End of #ifndef__HI_COMM_VIDEO_IN_H__ */
