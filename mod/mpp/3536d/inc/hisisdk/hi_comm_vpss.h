/******************************************************************************

  Copyright (C), 2001-2011, Hisilicon Tech. Co., Ltd.

 ******************************************************************************
  File Name     : hi_comm_vpss.h
  Version       : Initial Draft
  Author        : Hisilicon multimedia software group
  Created       : 
  Last Modified :
  Description   : common struct definition for vpss
  Function List :
  History       :
  1.Date        : 20130508
    Author      : l00183122
    Modification: Create
  
 
******************************************************************************/

#ifndef __HI_COMM_VPSS_H__
#define __HI_COMM_VPSS_H__


#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

#include "hi_type.h"
#include "hi_common.h"
#include "hi_errno.h"
#include "hi_comm_video.h"

#define HI_ERR_VPSS_NULL_PTR        HI_DEF_ERR(HI_ID_VPSS, EN_ERR_LEVEL_ERROR, EN_ERR_NULL_PTR)
#define HI_ERR_VPSS_NOTREADY        HI_DEF_ERR(HI_ID_VPSS, EN_ERR_LEVEL_ERROR, EN_ERR_SYS_NOTREADY)
#define HI_ERR_VPSS_INVALID_DEVID   HI_DEF_ERR(HI_ID_VPSS, EN_ERR_LEVEL_ERROR, EN_ERR_INVALID_DEVID)
#define HI_ERR_VPSS_INVALID_CHNID   HI_DEF_ERR(HI_ID_VPSS, EN_ERR_LEVEL_ERROR, EN_ERR_INVALID_CHNID)
#define HI_ERR_VPSS_EXIST           HI_DEF_ERR(HI_ID_VPSS, EN_ERR_LEVEL_ERROR, EN_ERR_EXIST)
#define HI_ERR_VPSS_UNEXIST         HI_DEF_ERR(HI_ID_VPSS, EN_ERR_LEVEL_ERROR, EN_ERR_UNEXIST)
#define HI_ERR_VPSS_NOT_SUPPORT     HI_DEF_ERR(HI_ID_VPSS, EN_ERR_LEVEL_ERROR, EN_ERR_NOT_SUPPORT)
#define HI_ERR_VPSS_NOT_PERM        HI_DEF_ERR(HI_ID_VPSS, EN_ERR_LEVEL_ERROR, EN_ERR_NOT_PERM)
#define HI_ERR_VPSS_NOMEM           HI_DEF_ERR(HI_ID_VPSS, EN_ERR_LEVEL_ERROR, EN_ERR_NOMEM)
#define HI_ERR_VPSS_NOBUF           HI_DEF_ERR(HI_ID_VPSS, EN_ERR_LEVEL_ERROR, EN_ERR_NOBUF)
#define HI_ERR_VPSS_ILLEGAL_PARAM   HI_DEF_ERR(HI_ID_VPSS, EN_ERR_LEVEL_ERROR, EN_ERR_ILLEGAL_PARAM)
#define HI_ERR_VPSS_BUSY            HI_DEF_ERR(HI_ID_VPSS, EN_ERR_LEVEL_ERROR, EN_ERR_BUSY)
#define HI_ERR_VPSS_BUF_EMPTY       HI_DEF_ERR(HI_ID_VPSS, EN_ERR_LEVEL_ERROR, EN_ERR_BUF_EMPTY)


typedef   HI_S32 VPSS_GRP;  /* [0, VPSS_MAX_GRP_NUM) */
typedef   HI_S32 VPSS_CHN;  /* [0, VPSS_MAX_CHN_NUM) */

/*Define 4 video frame*/
typedef enum hiVPSS_FRAME_WORK_E
{
    VPSS_FRAME_WORK_LEFT     =  0,
    VPSS_FRAME_WORK_RIGHT    =  1,
    VPSS_FRAME_WORK_BOTTOM   =  2,
    VPSS_FRAME_WORK_TOP      =  3,
    VPSS_FRAME_WORK_BUTT
}VPSS_FRAME_WORK_E;

/*Define de-interlace mode*/    
typedef enum  hiVPSS_DIE_MODE_E
{
    VPSS_DIE_MODE_NODIE     = 0,
    VPSS_DIE_MODE_AUTO      = 1,
    VPSS_DIE_MODE_DIE       = 2,
    VPSS_DIE_MODE_BUTT
}VPSS_DIE_MODE_E;

/*Define attributes of vpss channel*/
typedef struct hiVPSS_CHN_ATTR_S
{
    HI_BOOL bSpEn;    /*Sharpen enable*/
    HI_BOOL bUVInvert;  /* UV Invert enable*/
    HI_BOOL bBorderEn; /*Frame enable*/
    BORDER_S  stBorder;
}VPSS_CHN_ATTR_S;

typedef struct hiVPSS_GRP_PARAM_S
{
    HI_U32 u32Contrast;          /*strength of dymanic contrast improve*/
	HI_U32 u32DieStrength;       /*strength of de-interlace,not used now*/
    HI_U32 u32IeStrength;        /*strength of image enhance*/
    HI_U32 u32SfStrength;        /*strength of y space filter*/
    HI_U32 u32TfStrength;        /*strength of y time filter*/
    HI_U32 u32CfStrength;        /*strength of c space filter*/
	HI_U32 u32CTfStrength;       /*strength of c time filter*/
	HI_U32 u32CvbsStrength;      /*strength of cvbs*/
    HI_U32 u32DeMotionBlurring;  /*strength of de motion blurring,not used now*/
}VPSS_GRP_PARAM_S;

/*Define detailed NR params for grp image process*/
typedef struct hiVPSS_NR_ADVANCED_PARAM_S
{
	HI_U32 u32Mdz;
    HI_U32 u32HTfRelaStrength;   /*relation strength of hard NR chn */

	/*only for weak nr chn*/
	HI_U32 u32Edz;
	HI_U32 u32WTfRelaStrength;   /*relation strength of weak NR chn */
}VPSS_NR_ADVANCED_PARAM_S;


/* Define image feild select mode */
typedef enum hiVPSS_CAPSEL_E
{
    VPSS_CAPSEL_BOTH = 0,             /* top and bottom field */
    VPSS_CAPSEL_TOP,                  /* top field */
    VPSS_CAPSEL_BOTTOM,               /* bottom field */
    
    VPSS_CAPSEL_BUTT
} VPSS_CAPSEL_E;

/*Define coordinate mode*/
typedef enum hiVPSS_CROP_COORDINATE_E   
{
    VPSS_CROP_RATIO_COOR = 0,   /*Ratio coordinate*/
    VPSS_CROP_ABS_COOR          /*Absolute coordinate*/
}VPSS_CROP_COORDINATE_E;

/*Define attributes of CLIP function*/
typedef struct hiVPSS_CROP_INFO_S
{
    HI_BOOL bEnable;        /*CROP enable*/
    VPSS_CROP_COORDINATE_E  enCropCoordinate;   /*Coordinate mode of the crop start point*/ 
    RECT_S  stCropRect;     /*CROP rectangular*/   
}VPSS_CROP_INFO_S;

/*Define attributes of vpss GROUP*/
typedef struct hiVPSS_GRP_ATTR_S
{
    /*statistic attributes*/
    HI_U32  u32MaxW;  /*MAX width of the group*/
    HI_U32  u32MaxH;  /*MAX height of the group*/
    PIXEL_FORMAT_E enPixFmt; /*Pixel format*/
    
    HI_BOOL bIeEn;    /*Image enhance enable*/
    HI_BOOL bDciEn;   /*Dynamic contrast Improve enable*/
    HI_BOOL bNrEn;    /*Noise reduce enable*/ 
    HI_BOOL bHistEn;  /*Hist enable*/
    HI_BOOL bEsEn;      /*Edge smooth enable*/
    VPSS_DIE_MODE_E enDieMode; /*De-interlace enable*/
}VPSS_GRP_ATTR_S;

/*Define vpss frame control info*/
typedef struct hiVPSS_FRAME_RATE_S
{
    HI_S32  s32SrcFrmRate;        /* Input frame rate of a  group*/
    HI_S32  s32DstFrmRate;        /* Output frame rate of a channel group */
} VPSS_FRAME_RATE_S;

/*Define vpss channel's work mode*/
typedef enum hiVPSS_CHN_MODE_E   
{
    VPSS_CHN_MODE_AUTO = 0, /*Auto mode*/
    VPSS_CHN_MODE_USER  /*User mode*/
}VPSS_CHN_MODE_E;

/*Define attributes of vpss channel's work mode*/
typedef struct hiVPSS_CHN_MODE_S
{
    VPSS_CHN_MODE_E  enChnMode;   /*Vpss channel's work mode*/
    HI_U32 u32Width;              /*Width of target image*/
    HI_U32 u32Height;             /*Height of target image*/
    HI_BOOL bDouble;              /*Field-frame transfer, only valid for VPSS_CHN2*/
    VPSS_FRAME_RATE_S stFrameRate;
    ASPECT_RATIO_S stAspectRatio;
    PIXEL_FORMAT_E  enPixelFormat;/*Pixel format of target image*/
    COMPRESS_MODE_E enCompressMode;   /*Compression mode of the output*/
}VPSS_CHN_MODE_S;


/*Define detailed params for channel image process*/
typedef struct hiVPSS_CHN_PARAM_S
{
    HI_U32 u32SpStrength;
    HI_U32 u32SfStrength;
    HI_U32 u32TfStrength;
    HI_U32 u32CfStrength;
    HI_U32 u32DeMotionBlurring;  
} VPSS_CHN_PARAM_S;

/*Define vpss prescale info*/
typedef struct hiVPSS_PRESCALE_INFO_S
{
    HI_BOOL bPreScale;       /*prescale enable*/
    SIZE_S  stDestSize;      /*destination size*/
}VPSS_PRESCALE_INFO_S;

/*Define vpss filter info*/
typedef struct hiVPSS_SIZER_INFO_S
{
    HI_BOOL bSizer;
    SIZE_S  stSize;
}VPSS_SIZER_INFO_S;

typedef struct hiVPSS_REGION_INFO_S
{
    RECT_S *pstRegion;    /*region attribute*/
    HI_U32 u32RegionNum;       /*count of the region*/
}VPSS_REGION_INFO_S;

typedef enum hiVPSS_PRESCALE_MODE_E
{
    VPSS_PRESCALE_MODE_DEFAULT = 0,  /*use vpss to prescale*/
    VPSS_PRESCALE_MODE_OTHER,        /*use vgs to prescale*/
    VPSS_PRESCALE_MODE_BUTT
}VPSS_PRESCALE_MODE_E;

typedef struct hiVPSS_MOD_PARAM_S
{
    HI_U32  u32VpssVbSource;
	HI_U32  u32VpssEnRatio;
} VPSS_MOD_PARAM_S;


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */
#endif /* __HI_COMM_VPSS_H__ */


