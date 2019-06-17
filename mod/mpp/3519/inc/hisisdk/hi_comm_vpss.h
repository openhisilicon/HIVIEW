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


typedef   HI_S32 VPSS_GRP;
typedef   HI_S32 VPSS_GRP_PIPE;
typedef   HI_S32 VPSS_CHN;

#define VPSS_INVALID_FRMRATE  (-1UL)
#define VPSS_CHN0               0     
#define VPSS_CHN1               1
#define VPSS_CHN2               2
#define VPSS_CHN3               3
#define VPSS_INVALID_CHN       -1 

/*Define de-interlace mode*/    
typedef enum  hiVPSS_DIE_MODE_E
{
    VPSS_DIE_MODE_AUTO      = 0,
    VPSS_DIE_MODE_NODIE     = 1,
    VPSS_DIE_MODE_DIE       = 2,
    VPSS_DIE_MODE_BUTT
}VPSS_DIE_MODE_E;

/*Define attributes of vpss channel*/
typedef struct hiVPSS_CHN_ATTR_S
{
    HI_BOOL bSpEn;              /*Sharpen enable*/         
    HI_BOOL bBorderEn;          /*Frame enable*/
    HI_BOOL bMirror;            /*mirror enable*/
    HI_BOOL bFlip;              /*flip   enable*/
    HI_S32  s32SrcFrameRate;    /* source frame rate */
    HI_S32  s32DstFrameRate;    /* dest  frame rate */   
    BORDER_S  stBorder;     
}VPSS_CHN_ATTR_S;

typedef struct hiVPSS_GRP_PARAM_S
{
    HI_U32 u32Contrast;     /*strength of dymanic contrast improve*/
    
    HI_S32 s32GlobalStrength; /*strength of whole 3DNR*/
    HI_S32 s32IeStrength;     /*strength of image enhance*/
    HI_S32 s32YSFStrength;   /*strength of space filter*/
    HI_S32 s32YTFStrength;   /*strength of time filter*/
    HI_S32 s32CSFStrength;   /*strength of chroma filter*/
    HI_S32 s32CTFStrength;   /*strength of chroma filter*/    
    HI_S32 s32MotionLimen;   /*limen for motion*/    
}VPSS_GRP_PARAM_S;


/* for Hi3519V101 3DNR S interface */
typedef struct hiNRS_PARAM_V3_S
{
  HI_U8  sf0_ies,    sf0_ieb,  sf0_iet, _rB_;
  HI_U8  sf0_bright, sf0_dark;   
  HI_U8  sf1_bright, sf1_dark;
  HI_U16 tf1_md_thresh; 
  HI_U8  sf2_bright, sf2_dark;
  HI_U16 tf2_md_thresh;   
  HI_U8  sfk_bright, sfk_dark,  sf_ed_thresh; 
  
  HI_U8  sfk_profile : 3,  tf_mth_linear_flg : 1, _reserved_b_ : 4;
  HI_U8  sfk_ratio   : 5,  sf_bsc_freq : 3; 
  
  HI_U8  tf1_md_type : 4, tf1_still : 4; 
  HI_U8  tf2_md_type : 4, tf2_still : 4;   
  
  HI_U8  tf1_md_profile : 3; 
  HI_U8  tf2_md_profile : 3, tf_profile : 2; 
  
  HI_U8   csf_strength, ctf_range;
  HI_U16  ctf_strength : 6;
  HI_U16  tf1_moving   : 5;   
  HI_U16  tf2_moving   : 5; 
} NRS_PARAM_V3_S;


typedef struct hiSNAP_NRS_PARAM_S
{
  HI_U8  C0_TfClp : 1, C0_TfRng : 6, _rb0_ : 1;
  HI_U8  C0_SBS, C0_SDS;
  HI_U8  C1_SBS, C1_SDS;
  HI_U8  Y0_SBS, Y0_SDS, Y0_STH,  Y0_SBF : 2, Y0_RTO : 5, _rb1_ : 1;
  HI_U8  Y1_SBS, Y1_SDS, Y1_STH,  Y1_PRO : 3, Y1_RTO : 5, _rB_[ 3 ];
} SNAP_NRS_PARAM_S;

typedef struct hiSNAP_NRS_PARAM_AUTO_S
{
    HI_U32 u32ParamNum;
    HI_U32* pau32ISO;
    SNAP_NRS_PARAM_S*  pastNRSParam;
} SNAP_NRS_PARAM_AUTO_S;

typedef struct hiSNAP_NRS_PARAM_MANUAL_S
{
    SNAP_NRS_PARAM_S stNRSParam;
} SNAP_NRS_PARAM_MANUAL_S;

typedef struct hiNRS_PARAM_VIDEO_AUTO_S
{
    HI_U32 u32ParamNum;
    HI_U32* pau32ISO;
    NRS_PARAM_V3_S* pastNRSParam;
} NRS_PARAM_VIDEO_AUTO_S;

typedef struct hiNRS_PARAM_VIDEO_MANUAL_S
{
    NRS_PARAM_V3_S stNRSParam;
} NRS_PARAM_VIDEO_MANUAL_S;


typedef struct { HI_U8  IES, IET, IEB, _rB_; } tV19yIEy;

typedef struct 
{ 
  HI_U8     SBF0 : 1;
  HI_U8     SBF1 : 1, horPro : 2;
  HI_U8     SBF2 : 2, verPro : 2, kProDD  : 3, SSLP   : 5, _rB_;
  HI_U8     VRTO,     SFkRfw : 6, SFkType : 2, SFkBig : 1, _rb_ : 7;

  HI_U16    STH[3], SBS[3], SDS[3];  

} tV19ySFy;

typedef struct
{
  HI_U16    MATH : 10,  MATE : 4,  MATW : 2;
  HI_U8     MASW :  4,  MABW : 3,  MAXN : 1, _rB_;

} tV19yMDy;

typedef struct
{
  HI_U8     SFR[4], MTFR[4], TFR[4], STR, _rB_[3];
  
  HI_U16    MDDZ : 10, TFS  : 4,  TFT : 2;
  HI_U16    SDZ  : 10, TSS  : 5, _rb_ : 1;

} tV19yTFy;

typedef struct
{
   HI_U8  SFC;
   HI_U8  TFC : 6, _rsvd_b1_ : 2;
   HI_U8  TPC : 6, _rsvd_b2_ : 2;
   HI_U8  TRC;
}  tV19yNRc;


typedef struct hiVIDEO_NRX_PARAM_V1_S
{
    tV19yIEy  IEy;
    tV19ySFy  SFy[4];
    tV19yMDy  MDy[3];
    tV19yTFy  TFy[4];
    tV19yNRc  NRc;
} VIDEO_NRX_PARAM_V1_S;


typedef struct hiVIDEO_NRX_PARAM_V1_MANUAL_S
{
    VIDEO_NRX_PARAM_V1_S  stVideoNrXParamV1;
} VIDEO_NRX_PARAM_V1_MANUAL_S;

typedef enum hiVPSS_NR_OPT_TYPE_E
{
    VPSS_NRS_MANUAL  = 0,    /*Manual mode*/
    VPSS_NRS_AUTO    = 1,    /*Auto mode*/
    VPSS_NRS_BUTT    
}VPSS_NR_OPT_TYPE_E;

typedef struct hiSNAP_NRS_PARAM_V4_S
{
    VPSS_NR_OPT_TYPE_E         enOptType;		    /*Adaptive NR */
    SNAP_NRS_PARAM_MANUAL_S    stSnapNrManual;	    /*NRS param for manual snap */
    SNAP_NRS_PARAM_AUTO_S      stSnapNrAuto; 		/*NRS param for auto snap */
} SNAP_NRS_PARAM_V4_S;


typedef struct hiNRS_PARAM_V4_S
{
    VPSS_NR_OPT_TYPE_E          enOptType;		    /*Adaptive NR */
    NRS_PARAM_VIDEO_MANUAL_S    stVideoNrManual;	/*NRS param for manual video */
    NRS_PARAM_VIDEO_AUTO_S      stVideoNrAuto; 		/*NRS param for auto video */
} NRS_PARAM_V4_S;



typedef struct hiNRX_PARAM_V1_S
{
    VPSS_NR_OPT_TYPE_E          enOptType;		        /*Adaptive NR */
    VIDEO_NRX_PARAM_V1_MANUAL_S stVideoNrXV1Manual;	    /*NRX param for manual video */
} NRX_PARAM_V1_S;


typedef enum hiVPSS_NR_VER_E   
{
    VPSS_NR_V1 = 1, 
    VPSS_NR_V2 = 2, 
    VPSS_NR_V3 = 3,
    VPSS_NR_V4 = 4,
    VPSS_NR_BUTT  
}VPSS_NR_VER_E;

typedef enum hiVPSS_NR_INTF_E   
{
    VPSS_NR_INTF_S = 0, 
    VPSS_NR_INTF_X = 1, 
    VPSS_NR_INTF_B = 2,
    VPSS_NR_INTF_BUTT  
}VPSS_NR_INTF_E;


typedef struct hiVPSS_GRP_NRX_PARAM_S
{
    VPSS_NR_VER_E enNRVer;
    union
    {
        NRX_PARAM_V1_S stNRXParam_V1;                   /* interface X V1 for Hi3519V101  */
    };
    
}VPSS_GRP_NRX_PARAM_S;




typedef enum hiVPSS_NR_TYPE_E   
{
    VPSS_NR_TYPE_VIDEO      = 0, 
    VPSS_NR_TYPE_SNAP       = 1, 
    VPSS_NR_TYPE_BUTT  
}VPSS_NR_TYPE_E;

typedef struct hiVPSS_GRP_SNAP_NRS_PARAM_S
{
    VPSS_NR_VER_E enNRVer;
    union
    {
        SNAP_NRS_PARAM_V4_S     stSnapNRSParam_V4;   /* interface S V4 for Hi3519V101 snap  */
    };    
} VPSS_GRP_SNAP_NRS_PARAM_S;


typedef struct hiVPSS_GRP_NRS_PARAM_S
{
    VPSS_NR_VER_E enNRVer;
    union
    {
        NRS_PARAM_V3_S          stNRSParam_V3;   /* interface S V3 for Hi3519V101   video*/
        NRS_PARAM_V4_S          stNRSParam_V4;   /* interface S V4 for Hi3519V101   video*/
    };    
}VPSS_GRP_NRS_PARAM_S;

typedef struct hiVPSS_GRP_NRS_AUTO_INFO_S
{
    HI_U32                          u32ISO;
    VPSS_NR_TYPE_E                  enNrType;           /*NR type*/
    union
    {
        NRS_PARAM_V3_S              stVideoNrAuto;      /*NRS param for auto video */
        SNAP_NRS_PARAM_S            stSnapNrAuto; 		/*NRS param for auto snap */
    };
    
} VPSS_GRP_NRS_AUTO_INFO_S;


typedef struct hiVPSS_GRP_QUERY_S
{
    VPSS_NR_TYPE_E                  enNrType;           /*NR type*/
    VPSS_NR_VER_E                   enNRVer;            /*Running version of NR */
    VPSS_NR_OPT_TYPE_E              enOptType;		    /*NR Adaptive*/
    VPSS_NR_INTF_E                  enNrIntfType;       /*NR Interface*/
} VPSS_GRP_QUERY_S;


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

typedef struct hiVPSS_LDC_ATTR_S
{
    HI_BOOL bEnable;                    /* Whether LDC is enbale */
    LDC_ATTR_S stAttr;                  /* LDC Attribute */
}VPSS_LDC_ATTR_S;

typedef enum hiVPSS_STITCH_BLEND_MODE_E
{
    VPSS_STITCH_BLEND_ADJUST = 0,
    VPSS_STITCH_BLEND_BUTT
}VPSS_STITCH_BLEND_MODE_E;

typedef struct hiVPSS_STITCH_BLEND_ATTR_S
{
    HI_U32          u32OutWidth;
    HI_U32          u32OutHeight;
    POINT_S         stOverlapPoint[4];    
}VPSS_STITCH_BLEND_ATTR_S;

typedef struct hiVPSS_STITCH_BLEND_ADJUST_PARAM_S
{
    HI_U32      u32BldPosition;   
    HI_U32      u32BldRange;      
}VPSS_STITCH_BLEND_ADJUST_PARAM_S;

typedef struct hiVPSS_STITCH_BLEND_PARAM_S
{
    union
    {
        VPSS_STITCH_BLEND_ADJUST_PARAM_S    stAdjustParam;
    };
}VPSS_STITCH_BLEND_PARAM_S;



typedef enum hiVPSS_NR_REF_SOURCE_E   
{
    VPSS_NR_REF_FROM_RFR       = 0, 
    VPSS_NR_REF_FROM_CHN0      = 1, 
    VPSS_NR_REF_FROM_SRC       = 2, 
    VPSS_NR_REF_FROM_BUTT   
}VPSS_NR_REF_SOURCE_E;


typedef enum hiVPSS_NR_OUTPUT_MODE_E   
{
    VPSS_NR_OUTPUT_NORMAL      = 0, 
    VPSS_NR_OUTPUT_DELAY       = 1, 
    VPSS_NR_OUTPUT_BUTT  
}VPSS_NR_OUTPUT_MODE_E;

typedef struct hiNR_SNAP_ATTR_S
{
}NR_SNAP_ATTR_S;

typedef struct hiNR_VIDEO_ATTR_S
{
    VPSS_NR_REF_SOURCE_E   enNrRefSource;
    VPSS_NR_OUTPUT_MODE_E  enNrOutputMode;
}NR_VIDEO_ATTR_S;

typedef struct hiVPSS_NR_ATTR_S
{
    VPSS_NR_TYPE_E          enNrType;
    HI_U32                  u32RefFrameNum;
    union
    {
        NR_VIDEO_ATTR_S       stNrVideoAttr;
        NR_SNAP_ATTR_S        stNrSnapAttr;        
    };
}VPSS_NR_ATTR_S;


/*Define attributes of vpss GROUP*/
typedef struct hiVPSS_GRP_ATTR_S
{
    /*statistic attributes*/
    HI_U32                  u32MaxW;                /*MAX width of the group*/                    
    HI_U32                  u32MaxH;                /*MAX height of the group*/
    PIXEL_FORMAT_E          enPixFmt;               /*Pixel format*/
    
    HI_BOOL                 bIeEn;                  /*Image enhance enable*/
    HI_BOOL                 bDciEn;                 /*Dynamic contrast Improve enable*/
    
    HI_BOOL                 bNrEn;                  /*Noise reduce enable*/
    VPSS_NR_ATTR_S          stNrAttr;
    
    HI_BOOL                 bHistEn;                /*Hist enable*/
    VPSS_DIE_MODE_E         enDieMode;              /*De-interlace enable*/

    HI_BOOL                    bStitchBlendEn;               /*whether enable blend*/
    VPSS_STITCH_BLEND_MODE_E   enStitchBlendMode;
    VPSS_STITCH_BLEND_ATTR_S   stStitchBlendAttr;
} VPSS_GRP_ATTR_S;

/*Define vpss channel's work mode*/
typedef enum hiVPSS_CHN_MODE_E   
{
    VPSS_CHN_MODE_AUTO  = 0,        /*Auto mode*/
    VPSS_CHN_MODE_USER  = 1         /*User mode*/
}VPSS_CHN_MODE_E;

/*Define attributes of vpss channel's work mode*/
typedef struct hiVPSS_CHN_MODE_S
{
    VPSS_CHN_MODE_E     enChnMode;              /*Vpss channel's work mode*/
    HI_U32              u32Width;               /*Width of target image*/
    HI_U32              u32Height;              /*Height of target image*/
    HI_BOOL             bDouble;                /*Field-frame transfer, only valid for VPSS_PRE0_CHN*/
    PIXEL_FORMAT_E      enPixelFormat;          /*Pixel format of target image*/
    COMPRESS_MODE_E     enCompressMode;         /*Compression mode of the output*/
}VPSS_CHN_MODE_S;

typedef struct hiVPSS_ROTATE_EX_ATTR_S
{
    HI_BOOL bEnable;
    ROTATE_EX_S stRotateEx;
}VPSS_ROTATE_EX_ATTR_S;

/*Define detailed params for channel image process*/
typedef struct hiVPSS_CHN_PARAM_S
{
    HI_U32 u32SpStrength;
} VPSS_CHN_PARAM_S;

/*Define vpss frame control info*/
typedef struct hiVPSS_FRAME_RATE_S
{
    HI_S32  s32SrcFrmRate;        /* Input frame rate of a  group*/
    HI_S32  s32DstFrmRate;        /* Output frame rate of a channel group */
} VPSS_FRAME_RATE_S;

/*Define attributes of vpss extend channel*/
typedef struct hiVPSS_EXT_CHN_ATTR_S
{
    VPSS_CHN        s32BindChn;             /*channel bind to*/
    HI_U32          u32Width;               /*Width of target image*/
    HI_U32          u32Height;              /*Height of target image*/
    HI_S32          s32SrcFrameRate;        /*Frame rate of source*/
    HI_S32          s32DstFrameRate;        /*Frame rate of extend chn input&output*/
    PIXEL_FORMAT_E  enPixelFormat;          /*Pixel format of target image*/    
    COMPRESS_MODE_E enCompressMode;   /*Compression mode of the output*/
}VPSS_EXT_CHN_ATTR_S;

typedef struct hiVPSS_REGION_INFO_S
{
    RECT_S      *pstRegion;         /*region attribute*/
    HI_U32      u32RegionNum;       /*count of the region*/
}VPSS_REGION_INFO_S;


typedef struct hiVPSS_LOW_DELAY_INFO_S
{
    HI_BOOL bEnable;
    HI_U32 u32LineCnt;

}VPSS_LOW_DELAY_INFO_S;

typedef struct hiVPSS_PARAM_MOD_S
{
    HI_BOOL bOneBufForLowDelay;
}VPSS_MOD_PARAM_S;

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */
#endif /* __HI_COMM_VPSS_H__ */


