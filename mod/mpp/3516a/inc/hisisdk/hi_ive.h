/******************************************************************************

  Copyright (C), 2001-2014, Hisilicon Tech. Co., Ltd.

 ******************************************************************************
  File Name     : hi_comm_ive.h
  Version       : Initial Draft
  Author        : Hisilicon multimedia software (IVE) group
  Created       : 2014/08/05
  Description   : 
  History       :
  1.Date        : 2013/07/01~2014/08/05
    Author      : t00228657\c00211359\c00206215
    Modification: Created file
******************************************************************************/
#ifndef _HI_IVE_H_
#define _HI_IVE_H_

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif

#include "hi_comm_ive.h"
//if sdk environment,include hi_common.h,else typedef HI_S32 IVE_HANDLE
#include "hi_common.h"


#define IVE_HIST_NUM		     256
#define IVE_MAP_NUM		         256
#define IVE_MAX_REGION_NUM       254
#define IVE_ST_MAX_CORNER_NUM    200

/*
*DMA mode ,created by Chen Quanfu 2013-07-19
*/
typedef enum hiIVE_DMA_MODE_E
{
    IVE_DMA_MODE_DIRECT_COPY    =  0x0,
    IVE_DMA_MODE_INTERVAL_COPY  =  0x1,
    IVE_DMA_MODE_SET_3BYTE      =  0x2,
    IVE_DMA_MODE_SET_8BYTE      =  0x3,
    IVE_DMA_MODE_BUTT
}IVE_DMA_MODE_E;

/*
*DMA control parameter ,created by Chen Quanfu 2013-07-19
*/
typedef struct hiIVE_DMA_CTRL_S
{
    IVE_DMA_MODE_E enMode;
    HI_U64 u64Val;		    /*Used in memset mode*/
    HI_U8 u8HorSegSize;		/*Used in interval-copy mode, every row was segmented by u8HorSegSize bytes, restricted in values of 2,3,4,8,16*/
    HI_U8 u8ElemSize; 		/*Used in interval-copy mode, the valid bytes copied in front of every segment in a valid row, which 0<u8ElemSize<u8HorSegSize*/
    HI_U8 u8VerSegRows;		/*Used in interval-copy mode, copy one row in every u8VerSegRows*/
}IVE_DMA_CTRL_S;

/*
*Filter control parameters
*You need to set these parameters when using the filter operator.
*Modify template filter from 3x3 to 5x5 by Chen Quanfu 2013-07-11
*/
typedef struct hiIVE_FILTER_CTRL_S
{
    HI_S8 as8Mask[25];        /*Template parameter filter coefficient*/
    HI_U8 u8Norm;             /*Normalization parameter, by right shift*/
}IVE_FILTER_CTRL_S;

/* 
* CSC working mode.
* Modified by Tan Bing, 2013-7-22.
*/
typedef enum hiIVE_CSC_MODE_E
{
    IVE_CSC_MODE_VIDEO_BT601_YUV2RGB =  0x0,	/*CSC: YUV2RGB, video transfer mode, RGB value range [16, 235]*/    
    IVE_CSC_MODE_VIDEO_BT709_YUV2RGB =  0x1,	/*CSC: YUV2RGB, video transfer mode, RGB value range [16, 235]*/
    IVE_CSC_MODE_PIC_BT601_YUV2RGB   =  0x2,	/*CSC: YUV2RGB, picture transfer mode, RGB value range [0, 255]*/
    IVE_CSC_MODE_PIC_BT709_YUV2RGB   =  0x3,	/*CSC: YUV2RGB, picture transfer mode, RGB value range [0, 255]*/

	IVE_CSC_MODE_PIC_BT601_YUV2HSV   =  0x4,	/*CSC: YUV2HSV, picture transfer mode, HSV value range [0, 255]*/
	IVE_CSC_MODE_PIC_BT709_YUV2HSV   =  0x5,	/*CSC: YUV2HSV, picture transfer mode, HSV value range [0, 255]*/

	IVE_CSC_MODE_PIC_BT601_YUV2LAB   =  0x6,	/*CSC: YUV2LAB, picture transfer mode, Lab value range [0, 255]*/
	IVE_CSC_MODE_PIC_BT709_YUV2LAB   =  0x7,	/*CSC: YUV2LAB, picture transfer mode, Lab value range [0, 255]*/

	IVE_CSC_MODE_VIDEO_BT601_RGB2YUV =  0x8,   	/*CSC: RGB2YUV, video transfer mode, YUV value range [0, 255]*/
	IVE_CSC_MODE_VIDEO_BT709_RGB2YUV =  0x9,	/*CSC: RGB2YUV, video transfer mode, YUV value range [0, 255]*/
	IVE_CSC_MODE_PIC_BT601_RGB2YUV   =  0xa,   	/*CSC: RGB2YUV, picture transfer mode, Y:[16, 235],U\V:[16, 240]*/
	IVE_CSC_MODE_PIC_BT709_RGB2YUV   =  0xb,	/*CSC: RGB2YUV, picture transfer mode, Y:[16, 235],U\V:[16, 240]*/

    IVE_CSC_MODE_BUTT
}IVE_CSC_MODE_E;

/*
*CSC control parameters
*You need to set these parameters when using the CSC operator.
*Removed the CSC output format, replaced by setting the output image type.
*Modified by tanbing 2013-7-22.
*/
typedef struct hiIVE_CSC_CTRL_S
{
    IVE_CSC_MODE_E    enMode; /*Working mode*/
}IVE_CSC_CTRL_S;

/*
*Filter+CSC control parameters
*You need to set these parameters when using the filter+CSC operator.
*The control information about both the filter and CSC needs to be configured.
*Modify template filter from 3x3 to 5x5 by Tan Bing 2013-07-11
*/
typedef struct hiIVE_FILTER_AND_CSC_CTRL_S
{
    IVE_CSC_MODE_E		enMode;			/*CSC working mode*/
    HI_S8				as8Mask[25];	/*Template parameter filter coefficient*/
    HI_U8				u8Norm;			/*Normalization parameter, by right shift*/
}IVE_FILTER_AND_CSC_CTRL_S;

/*
*SOBEL output ctrl
*/
typedef enum hiIVE_SOBEL_OUT_CTRL_E
{
	IVE_SOBEL_OUT_CTRL_BOTH =  0x0, /*Output horizontal and vertical*/
	IVE_SOBEL_OUT_CTRL_HOR  =  0x1,	/*Output horizontal*/
	IVE_SOBEL_OUT_CTRL_VER  =  0x2,	/*Output vertical*/
	IVE_SOBEL_OUT_CTRL_BUTT
}IVE_SOBEL_OUT_CTRL_E;

/*
*SOBEL control parameter
*Add IVE_SOBEL_OUT_CTRL_E by Chen Quanfu 2013-07-11
*/
typedef struct hiIVE_SOBEL_CTRL_S
{
	IVE_SOBEL_OUT_CTRL_E enOutCtrl; /*Output format*/
    HI_S8 as8Mask[25];			    /*Template parameter*/
}IVE_SOBEL_CTRL_S;

/*
*Type of the magnitude and angle output results
*Modified by Chen Quanfu 2013-07-23
*/
typedef enum hiIVE_MAG_AND_ANG_OUT_CTRL_E
{
    IVE_MAG_AND_ANG_OUT_CTRL_MAG          =  0x0,      /*Only the magnitude is output.*/    
    IVE_MAG_AND_ANG_OUT_CTRL_MAG_AND_ANG  =  0x1,      /*The magnitude and angle are output.*/
    IVE_MAG_AND_ANG_OUT_CTRL_BUTT
}IVE_MAG_AND_ANG_OUT_CTRL_E;

/*
*Magnitude and angle control parameter
*/
typedef struct hiIVE_MAG_AND_ANG_CTRL_S
{
    IVE_MAG_AND_ANG_OUT_CTRL_E enOutCtrl;
	HI_U16 u16Thr;
    HI_S8  as8Mask[25];         /*Template parameter.*/
}IVE_MAG_AND_ANG_CTRL_S;

/*
*Dilate control parameters
*/
typedef struct hiIVE_DILATE_CTRL_S
{
    HI_U8 au8Mask[25];         /*The template parameter value must be 0 or 255.*/
}IVE_DILATE_CTRL_S;

/*
*Erode control parameter
*/
typedef struct hiIVE_ERODE_CTRL_S
{
    HI_U8 au8Mask[25];         /*The template parameter value must be 0 or 255.*/
}IVE_ERODE_CTRL_S;

/*
* Type of the Thresh mode.
* Modified by Tan Bing, 2013-7-22
*/
typedef enum hiIVE_THRESH_MODE_E
{
    IVE_THRESH_MODE_BINARY       =  0x0,  /*srcVal <= lowThr, dstVal = minVal; srcVal > lowThr, dstVal = maxVal.*/
    IVE_THRESH_MODE_TRUNC        =  0x1,  /*srcVal <= lowThr, dstVal = srcVal; srcVal > lowThr, dstVal = maxVal.*/
    IVE_THRESH_MODE_TO_MINVAL    =  0x2,  /*srcVal <= lowThr, dstVal = minVal; srcVal > lowThr, dstVal = srcVal.*/
	
	IVE_THRESH_MODE_MIN_MID_MAX  =  0x3,  /*srcVal <= lowThr, dstVal = minVal;  lowThr < srcVal <= highThr, dstVal = midVal; srcVal > highThr, dstVal = maxVal.*/
	IVE_THRESH_MODE_ORI_MID_MAX  =  0x4,  /*srcVal <= lowThr, dstVal = srcVal;  lowThr < srcVal <= highThr, dstVal = midVal; srcVal > highThr, dstVal = maxVal.*/     
	IVE_THRESH_MODE_MIN_MID_ORI  =  0x5,  /*srcVal <= lowThr, dstVal = minVal;  lowThr < srcVal <= highThr, dstVal = midVal; srcVal > highThr, dstVal = srcVal.*/
	IVE_THRESH_MODE_MIN_ORI_MAX  =  0x6,  /*srcVal <= lowThr, dstVal = minVal;  lowThr < srcVal <= highThr, dstVal = srcVal; srcVal > highThr, dstVal = maxVal.*/
	IVE_THRESH_MODE_ORI_MID_ORI  =  0x7,  /*srcVal <= lowThr, dstVal = srcVal;  lowThr < srcVal <= highThr, dstVal = midVal; srcVal > highThr, dstVal = srcVal.*/

	IVE_THRESH_MODE_BUTT
}IVE_THRESH_MODE_E;

/*
* Thresh control parameters.
* Modified by Tan Bing, 2013-7-22
*/
typedef struct hiIVE_THRESH_CTRL_S
{
    IVE_THRESH_MODE_E enMode; 
    HI_U8 u8LowThr;			/*user-defined threshold,  0<=u8LowThr<=255 */
	HI_U8 u8HighThr;		/*user-defined threshold, if enMode<IVE_THRESH_MODE_MIN_MID_MAX, u8HighThr is not used, else 0<=u8LowThr<=u8HighThr<=255;*/
    HI_U8 u8MinVal;			/*Minimum value when tri-level thresholding*/
    HI_U8 u8MidVal;			/*Middle value when tri-level thresholding, if enMode<2, u32MidVal is not used; */
	HI_U8 u8MaxVal;			/*Maxmum value when tri-level thresholding*/
}IVE_THRESH_CTRL_S;

/*
*Type of the Sub output results
*/
typedef enum hiIVE_SUB_MODE_E
{
    IVE_SUB_MODE_ABS	=  0x0,	  /*Absolute value of the difference*/
    IVE_SUB_MODE_SHIFT  =  0x1,   /*The output result is obtained by shifting the result one digit right to reserve the signed bit.*/ 
    IVE_SUB_MODE_BUTT
}IVE_SUB_MODE_E;

/*
*Sub control parameters
*/
typedef struct hiIVE_SUB_CTRL_S
{
	IVE_SUB_MODE_E enMode;
}IVE_SUB_CTRL_S;

/*
*Type of the Integ output results, created by Tan Bing, 2013-03-18
* Modify by Chen Quanfu 2013-07-15
* Spec: add IVE_INTEG_OUT_CTRL_SQSUM output format
*/
typedef enum hiIVE_INTEG_OUT_CTRL_E
{
	IVE_INTEG_OUT_CTRL_COMBINE  =  0x0,
	IVE_INTEG_OUT_CTRL_SUM	    =  0x1,
	IVE_INTEG_OUT_CTRL_SQSUM    =  0x2,
	IVE_INTEG_OUT_CTRL_BUTT
}IVE_INTEG_OUT_CTRL_E;

/*
* Integ control parameters,Create by Chen Quanfu 2013-07-15
*/
typedef struct hiIVE_INTEG_CTRL_S 
{ 
	IVE_INTEG_OUT_CTRL_E enOutCtrl; 
}IVE_INTEG_CTRL_S;

/*
*Type of the Thresh_S16 mode
*/
typedef enum hiIVE_THRESH_S16_MODE_E
{
    IVE_THRESH_S16_MODE_S16_TO_S8_MIN_MID_MAX  =  0x0,
    IVE_THRESH_S16_MODE_S16_TO_S8_MIN_ORI_MAX  =  0x1,
    IVE_THRESH_S16_MODE_S16_TO_U8_MIN_MID_MAX  =  0x2,
    IVE_THRESH_S16_MODE_S16_TO_U8_MIN_ORI_MAX  =  0x3,

    IVE_THRESH_S16_MODE_BUTT
}IVE_THRESH_S16_MODE_E;

/*
*Thresh_S16 control parameters
*/
typedef struct hiIVE_THRESH_S16_CTRL_S
{
    IVE_THRESH_S16_MODE_E enMode;
    HI_S16 s16LowThr;		/*User-defined threshold*/
    HI_S16 s16HighThr;		/*User-defined threshold*/
    IVE_8BIT_U un8MinVal;	/*Minimum value when tri-level thresholding*/
    IVE_8BIT_U un8MidVal;	/*Middle value when tri-level thresholding*/
    IVE_8BIT_U un8MaxVal;	/*Maxmum value when tri-level thresholding*/
}IVE_THRESH_S16_CTRL_S;

/*
*Type of the Thresh_U16 mode
*/
typedef enum hiIVE_THRESH_U16_MODE_E
{
    IVE_THRESH_U16_MODE_U16_TO_U8_MIN_MID_MAX  =  0x0,
    IVE_THRESH_U16_MODE_U16_TO_U8_MIN_ORI_MAX  =  0x1,

    IVE_THRESH_U16_MODE_BUTT
}IVE_THRESH_U16_MODE_E;

/*
*Thresh_U16 control parameters
*/
typedef struct hiIVE_THRESH_U16_CTRL_S
{
    IVE_THRESH_U16_MODE_E enMode;
    HI_U16 u16LowThr; 
    HI_U16 u16HighThr;
    HI_U8  u8MinVal; 
    HI_U8  u8MidVal;
    HI_U8  u8MaxVal; 
}IVE_THRESH_U16_CTRL_S;

/*
*Mode of 16BitTo8Bit
*/
typedef enum hiIVE_16BIT_TO_8BIT_MODE_E
{
	IVE_16BIT_TO_8BIT_MODE_S16_TO_S8		=  0x0,
	IVE_16BIT_TO_8BIT_MODE_S16_TO_U8_ABS	=  0x1,
	IVE_16BIT_TO_8BIT_MODE_S16_TO_U8_BIAS	=  0x2,
	IVE_16BIT_TO_8BIT_MODE_U16_TO_U8		=  0x3,

	IVE_16BIT_TO_8BIT_MODE_BUTT
}IVE_16BIT_TO_8BIT_MODE_E;

/*
*16BitTo8Bit control parameters
*/
typedef struct hiIVE_16BIT_TO_8BIT_CTRL_S
{
	IVE_16BIT_TO_8BIT_MODE_E enMode;
 	HI_U16 u16Denominator;
	HI_U8  u8Numerator; 
	HI_S8  s8Bias;
}IVE_16BIT_TO_8BIT_CTRL_S;

/*
*Type of the OrdStaFilter
*/
typedef enum hiIVE_ORD_STAT_FILTER_MODE_E 
{ 
    IVE_ORD_STAT_FILTER_MODE_MEDIAN  =  0x0,
    IVE_ORD_STAT_FILTER_MODE_MAX     =  0x1,
    IVE_ORD_STAT_FILTER_MODE_MIN     =  0x2,

    IVE_ORD_STAT_FILTER_MODE_BUTT 
}IVE_ORD_STAT_FILTER_MODE_E;

/*
*OrdStaFilter control parameters
*/
typedef struct hiIVE_ORD_STAT_FILTER_CTRL_S 
{ 
    IVE_ORD_STAT_FILTER_MODE_E enMode;

}IVE_ORD_STAT_FILTER_CTRL_S; 

/*
* Map LUT memory struct
*/
typedef struct hiIVE_MAP_LUT_MEM_S
{
    HI_U8  au8Map[IVE_MAP_NUM];
}IVE_MAP_LUT_MEM_S;

/*
* Equalizehist control member struct
*/
typedef struct hiIVE_EQUALIZE_HIST_CTRL_MEM_S
{
    HI_U32 au32Hist[IVE_HIST_NUM];
    HI_U8  au8Map[IVE_MAP_NUM];
}IVE_EQUALIZE_HIST_CTRL_MEM_S;

/*
*Equalizehist control parameters,created by Chen Quanfu 2013-07-17
*/
typedef struct hiIVE_EQUALIZE_HIST_CTRL_S
{
    IVE_MEM_INFO_S stMem;
}IVE_EQUALIZE_HIST_CTRL_S;

/*
*Add control parameters,created by Chen Quanfu 2013-07-17
*/
typedef struct hiIVE_ADD_CTRL_S
{
	HI_U0Q16 u0q16X;		 /*x of "xA+yB"*/
	HI_U0Q16 u0q16Y;         /*y of "xA+yB"*/
}IVE_ADD_CTRL_S;

/*
*NCC dst memory struct
*/
typedef struct hiIVE_NCC_DST_MEM_S
{
    HI_U64 u64Numerator;
    HI_U64 u64QuadSum1;
    HI_U64 u64QuadSum2;
    HI_U8  u8Reserved[8];
}IVE_NCC_DST_MEM_S;

/*
*Region struct
*/
typedef struct hiIVE_REGION_S
{
    HI_U32 u32Area;			   /*Represented by the pixel number*/
    HI_U16 u16Left;            /*Circumscribed rectangle left border*/
    HI_U16 u16Right;           /*Circumscribed rectangle right border*/
    HI_U16 u16Top;             /*Circumscribed rectangle top border*/
    HI_U16 u16Bottom;          /*Circumscribed rectangle bottom border*/
}IVE_REGION_S;

/*
*CCBLOB struct
*/
typedef struct hiIVE_CCBLOB_S
{
    HI_U16 u16CurAreaThr;                         /*Threshold of the result regions' area*/
    HI_S8  s8LabelStatus;                         /*-1: Labeled failed ; 0: Labeled successfully*/
    HI_U8  u8RegionNum;                           /*Number of valid region, non-continuous stored*/
    IVE_REGION_S astRegion[IVE_MAX_REGION_NUM];	  /*Valid regions with 'u32Area>0' and 'label = ArrayIndex+1'*/
}IVE_CCBLOB_S;

/*
*CCL control struct
*/
typedef struct hiIVE_CCL_CTRL_S
{
    HI_U16 u16InitAreaThr;    /*Init threshold of region area*/
    HI_U16 u16Step;           /*Increase area step for once*/
}IVE_CCL_CTRL_S;

/*
*GMM control struct
*/
typedef struct hiIVE_GMM_CTRL_S
{
    HI_U22Q10    u22q10NoiseVar;        /*Initial noise Variance*/
    HI_U22Q10    u22q10MaxVar;          /*Max  Variance*/
    HI_U22Q10    u22q10MinVar;          /*Min  Variance*/
    HI_U0Q16     u0q16LearnRate;        /*Learning rate*/
    HI_U0Q16     u0q16BgRatio;			/*Background ratio*/
    HI_U8Q8      u8q8VarThr;			/*Variance Threshold*/
    HI_U0Q16     u0q16InitWeight;       /*Initial Weight*/
    HI_U8        u8ModelNum;            /*Model number: 3 or 5*/
}IVE_GMM_CTRL_S;

/*
*CannyHysEdge control struct
*/
typedef struct hiIVE_CANNY_HYS_EDGE_CTRL_S
{
    IVE_MEM_INFO_S stMem;
    HI_U16 u16LowThr;
    HI_U16 u16HighThr;
    HI_S8 as8Mask[25];
} IVE_CANNY_HYS_EDGE_CTRL_S;

/*
*Canny stack size struct
*/
typedef struct hiIVE_CANNY_STACK_SIZE_S
{
    HI_U32 u32StackSize;   /*Stack size for output*/
    HI_U8 u8Reserved[12];  /*For 16 byte align*/
}IVE_CANNY_STACK_SIZE_S;

/*
*LBP compare mode
*/
typedef enum hiIVE_LBP_CMP_MODE_E
{
    IVE_LBP_CMP_MODE_NORMAL = 0x0,	/* P(x)-P(center)>= un8BitThr.s8Val, s(x)=1; else s(x)=0; */
    IVE_LBP_CMP_MODE_ABS = 0x1,		/* Abs(P(x)-P(center))>=un8BitThr.u8Val, s(x)=1; else s(x)=0; */

    IVE_LBP_CMP_MODE_BUTT
}IVE_LBP_CMP_MODE_E;

/*
*LBP control struct
*/
typedef struct hiIVE_LBP_CTRL_S 
{ 
    IVE_LBP_CMP_MODE_E enMode;
    IVE_8BIT_U un8BitThr;
}IVE_LBP_CTRL_S; 

/*
*Type of the GradientFilter output format
*/
typedef enum hiIVE_NORM_GRAD_OUT_CTRL_E
{
	IVE_NORM_GRAD_OUT_CTRL_HOR_AND_VER  =  0x0,
	IVE_NORM_GRAD_OUT_CTRL_HOR		    =  0x1,
	IVE_NORM_GRAD_OUT_CTRL_VER			=  0x2,
	IVE_NORM_GRAD_OUT_CTRL_COMBINE      =  0x3,

	IVE_NORM_GRAD_OUT_CTRL_BUTT
}IVE_NORM_GRAD_OUT_CTRL_E;

/*
*GradientFilter control parameters
*/
typedef struct hiIVE_NORM_GRAD_CTRL_S
{
	IVE_NORM_GRAD_OUT_CTRL_E enOutCtrl;
	HI_S8 as8Mask[25];
	HI_U8 u8Norm;
}IVE_NORM_GRAD_CTRL_S;

/*
* LKOpticalFlow  movement
*/
typedef struct hiIVE_MV_S9Q7_S
{
    HI_S32      s32Status;         /*Result of tracking: 0-success; -1-failure*/
    HI_S9Q7     s9q7Dx;           /*X-direction component of the movement*/
    HI_S9Q7     s9q7Dy;           /*Y-direction component of the movement*/
}IVE_MV_S9Q7_S;

typedef struct hiIVE_LK_OPTICAL_FLOW_CTRL_S
{
    HI_U16		u16CornerNum;		/*Number of the feature points,<200*/ 
    HI_U0Q8     u0q8MinEigThr;		/*Minimum eigenvalue threshold*/
    HI_U8	    u8IterCount;        /*Maximum iteration times*/
    HI_U0Q8     u0q8Epsilon;        /*Threshold of iteration for dx^2 + dy^2 < u0q8Epsilon */
}IVE_LK_OPTICAL_FLOW_CTRL_S;

typedef struct hiIVE_ST_MAX_EIG_S
{
    HI_U16 u16MaxEig;           /*Shi-Tomasi second step output MaxEig*/
    HI_U8  u8Reserved[14];      /*For 16 byte align*/
}IVE_ST_MAX_EIG_S;

/*
*ShiTomasi control parameters
*/
typedef struct hiIVE_ST_CANDI_CORNER_CTRL_S
{
	IVE_MEM_INFO_S stMem;
	HI_U0Q8 u0q8QualityLevel;
}IVE_ST_CANDI_CORNER_CTRL_S;

typedef struct hiIVE_ST_CORNER_INFO_S
{
    HI_U16 u16CornerNum;
    IVE_POINT_U16_S astCorner[IVE_ST_MAX_CORNER_NUM];
}IVE_ST_CORNER_INFO_S;

typedef struct hiIVE_ST_CORNER_CTRL_S
{
	HI_U16 u16MaxCornerNum;
	HI_U16 u16MinDist;
}IVE_ST_CORNER_CTRL_S;

/*
* GradFg mode enum
*/
typedef enum hiIVE_GRAD_FG_MODE_E
{
    IVE_GRAD_FG_MODE_USE_CUR_GRAD  =  0x0, 
    IVE_GRAD_FG_MODE_FIND_MIN_GRAD =  0x1, 

    IVE_GRAD_FG_MODE_BUTT
}IVE_GRAD_FG_MODE_E;

/*
* GradFg ctrl struct
*/
typedef struct hiIVE_GRAD_FG_CTRL_S
{
    IVE_GRAD_FG_MODE_E enMode;		/*Calculation mode*/
    HI_U16 u16EdwFactor;			/*Edge width adjustment factor (range: 500 to 2000; default: 1000)*/
    HI_U8 u8CrlCoefThr;				/*Gradient vector correlation coefficient threshold (ranges: 50 to 100; default: 80)*/
    HI_U8 u8MagCrlThr;				/*Gradient amplitude threshold (range: 0 to 20; default: 4)*/
    HI_U8 u8MinMagDiff;				/*Gradient magnitude difference threshold (range: 2 to 8; default: 2)*/
    HI_U8 u8NoiseVal;				/*Gradient amplitude noise threshold (range: 1 to 8; default: 1)*/
    HI_U8 u8EdwDark;				/*Black pixels enable flag (range: 0 (no), 1 (yes); default: 1)*/
}IVE_GRAD_FG_CTRL_S;

typedef struct hiIVE_CANDI_BG_PIX_S
{
    HI_U8Q4F4 u8q4f4Mean;			/*Candidate background grays value */
    HI_U16 u16StartTime;			/*Candidate Background start time */
    HI_U16 u16SumAccessTime;		/*Candidate Background cumulative access time */
    HI_U16 u16ShortKeepTime;		/*Candidate background short hold time*/
    HI_U8 u8ChgCond;				/*Time condition for candidate background into the changing state*/
    HI_U8 u8PotenBgLife;			/*Potential background cumulative access time */
}IVE_CANDI_BG_PIX_S;

typedef struct hiIVE_WORK_BG_PIX_S
{
    HI_U8Q4F4 u8q4f4Mean;			/*0# background grays value */
    HI_U16 u16AccTime;				/*Background cumulative access time */
    HI_U8 u8PreGray;				/*Gray value of last pixel */
    HI_U5Q3 u5q3DiffThr;			/*Differential threshold */
    HI_U8 u8AccFlag;				/*Background access flag */
    HI_U8 u8BgGray[3];				/*1# ~ 3# background grays value */
}IVE_WORK_BG_PIX_S;

typedef struct hiIVE_BG_LIFE_S
{
    HI_U8 u8WorkBgLife[3];			/*1# ~ 3# background vitality */
    HI_U8 u8CandiBgLife;			/*Candidate background vitality */
}IVE_BG_LIFE_S;

typedef struct hiIVE_BG_MODEL_PIX_S
{
    IVE_WORK_BG_PIX_S	stWorkBgPixel;	/*Working background */
    IVE_CANDI_BG_PIX_S	stCandiPixel;	/*Candidate background */
    IVE_BG_LIFE_S		stBgLife;		/*Background vitality */
}IVE_BG_MODEL_PIX_S;

typedef struct hiIVE_FG_STAT_DATA_S
{
    HI_U32 u32PixNum;			
    HI_U32 u32SumLum;
    HI_U8  u8Reserved[8];
}IVE_FG_STAT_DATA_S;

typedef struct hiIVE_BG_STAT_DATA_S
{
    HI_U32 u32PixNum;			
    HI_U32 u32SumLum;
    HI_U8  u8Reserved[8];
}IVE_BG_STAT_DATA_S;

typedef struct hiIVE_MATCH_BG_MODEL_CTRL_S
{	
    HI_U32 u32CurFrmNum;		/*Current frame timestamp, in frame units */
    HI_U32 u32PreFrmNum;		/*Previous frame timestamp, in frame units */
    HI_U16 u16TimeThr;			/*Potential background replacement time threshold (range: 2 to 100 frames; default: 20) */

    HI_U8 u8DiffThrCrlCoef;		/*Correlation coefficients between differential threshold and gray value (range: 0 to 5; default: 0) */
    HI_U8 u8DiffMaxThr;			/*Maximum of background differential threshold (range: 3 to 15; default: 6) */
    HI_U8 u8DiffMinThr;			/*Minimum of background differential threshold (range: 3 to 15; default: 4) */
    HI_U8 u8DiffThrInc;			/*Dynamic Background differential threshold increment (range: 0 to 6; default: 0) */
    HI_U8 u8FastLearnRate;		/*Quick background learning rate (range: 0 to 4; default: 2) */
    HI_U8 u8DetChgRegion;		/*Whether to detect change region (range: 0 (no), 1 (yes); default: 0) */
}IVE_MATCH_BG_MODEL_CTRL_S;

typedef struct hiIVE_UPDATE_BG_MODEL_CTRL_S
{
    HI_U32 u32CurFrmNum;			/*Current frame timestamp, in frame units */
    HI_U32 u32PreChkTime;			/*The last time when background status is checked */
    HI_U32 u32FrmChkPeriod;			/*Background status checking period (range: 0 to 2000 frames; default: 50) */

    HI_U32 u32InitMinTime;			/*Background initialization shortest time (range: 20 to 6000 frames; default: 100)*/
    HI_U32 u32StyBgMinBlendTime;	/*Steady background integration shortest time (range: 20 to 6000 frames; default: 200)*/
    HI_U32 u32StyBgMaxBlendTime;	/*Steady background integration longest time (range: 20 to 40000 frames; default: 1500)*/
    HI_U32 u32DynBgMinBlendTime;	/*Dynamic background integration shortest time (range: 0 to 6000 frames; default: 0)*/
    HI_U32 u32StaticDetMinTime;		/*Still detection shortest time (range: 20 to 6000 frames; default: 80)*/
    HI_U16 u16FgMaxFadeTime;		/*Foreground disappearing longest time (range: 1 to 255 seconds; default: 15)*/
    HI_U16 u16BgMaxFadeTime;		/*Background disappearing longest time (range: 1 to 255  seconds ; default: 60)*/

    HI_U8 u8StyBgAccTimeRateThr;	/*Steady background access time ratio threshold (range: 10 to 100; default: 80)*/
    HI_U8 u8ChgBgAccTimeRateThr;	/*Change background access time ratio threshold (range: 10 to 100; default: 60)*/
    HI_U8 u8DynBgAccTimeThr;		/*Dynamic background access time ratio threshold (range: 0 to 50; default: 0)*/
    HI_U8 u8DynBgDepth;				/*Dynamic background depth (range: 0 to 3; default: 3)*/
    HI_U8 u8BgEffStaRateThr;		/*Background state time ratio threshold when initializing (range: 90 to 100; default: 90)*/

    HI_U8 u8AcceBgLearn;			/*Whether to accelerate background learning (range: 0 (no), 1 (yes); default: 0)*/
    HI_U8 u8DetChgRegion;			/*Whether to detect change region (range: 0 (no), 1 (yes); default: 0)*/
} IVE_UPDATE_BG_MODEL_CTRL_S;


typedef enum hiIVE_ANN_MLP_ACTIV_FUNC_E
{
    IVE_ANN_MLP_ACTIV_FUNC_IDENTITY     = 0x0,
    IVE_ANN_MLP_ACTIV_FUNC_SIGMOID_SYM  = 0x1,
    IVE_ANN_MLP_ACTIV_FUNC_GAUSSIAN     = 0x2,

    IVE_ANN_MLP_ACTIV_FUNC_BUTT
}IVE_ANN_MLP_ACTIV_FUNC_E;

typedef struct hiIVE_ANN_MLP_MODEL_S
{
    IVE_ANN_MLP_ACTIV_FUNC_E enActivFunc;
    IVE_MEM_INFO_S stWeight;
    HI_U32 u32TotalWeightSize;

    HI_U16 au16LayerCount[8];    /*8 layers, including input and output layer, every layerCount<=256*/
    HI_U16 u16MaxCount;          /*MaxCount<=256*/
    HI_U8 u8LayerNum;		     /*2<layerNum<=8*/
}IVE_ANN_MLP_MODEL_S;

typedef enum hiIVE_SVM_TYPE_E 
{ 
    IVE_SVM_TYPE_C_SVC   = 0x0,
    IVE_SVM_TYPE_NU_SVC  = 0x1,

    IVE_SVM_TYPE_BUTT
}IVE_SVM_TYPE_E;

typedef enum hiIVE_SVM_KERNEL_TYPE_E 
{ 
    IVE_SVM_KERNEL_TYPE_LINEAR  = 0x0,
    IVE_SVM_KERNEL_TYPE_POLY    = 0x1,
    IVE_SVM_KERNEL_TYPE_RBF     = 0x2,
    IVE_SVM_KERNEL_TYPE_SIGMOID = 0x3,

    IVE_SVM_KERNEL_TYPE_BUTT
}IVE_SVM_KERNEL_TYPE_E;

typedef struct hiIVE_SVM_MODEL_S
{
    IVE_SVM_TYPE_E enType;
    IVE_SVM_KERNEL_TYPE_E enKernelType;

    IVE_MEM_INFO_S  stSv;       /*SV memory*/
    IVE_MEM_INFO_S  stDf;       /*Decision functions memory*/
    HI_U32 u32TotalDfSize;      /*All decision functions coef size in byte*/

    HI_U16 u16FeatureDim;	
    HI_U16 u16SvTotal;
    HI_U8  u8ClassCount;
}IVE_SVM_MODEL_S;

/*
* Sad mode
*/
typedef enum hiIVE_SAD_MODE_E
{
	IVE_SAD_MODE_MB_4X4		= 0x0, /*4x4*/
	IVE_SAD_MODE_MB_8X8		= 0x1, /*8x8*/
	IVE_SAD_MODE_MB_16X16	= 0x2, /*16x16*/
	
	IVE_SAD_MODE_BUTT
}IVE_SAD_MODE_E;
/*
*Sad output ctrl
*/
typedef enum hiIVE_SAD_OUT_CTRL_E
{
	IVE_SAD_OUT_CTRL_16BIT_BOTH	= 0x0, /*Output 16 bit sad and thresh*/
	IVE_SAD_OUT_CTRL_8BIT_BOTH	= 0x1, /*Output 8 bit sad and thresh*/
	IVE_SAD_OUT_CTRL_16BIT_SAD	= 0x2, /*Output 16 bit sad*/
	IVE_SAD_OUT_CTRL_8BIT_SAD	= 0x3, /*Output 8 bit sad*/
	IVE_SAD_OUT_CTRL_THRESH		= 0x4, /*Output thresh,16 bits sad */

	IVE_SAD_OUT_CTRL_BUTT
}IVE_SAD_OUT_CTRL_E;
/*
* Sad ctrl param
*/
typedef struct hiIVE_SAD_CTRL_S
{
	IVE_SAD_MODE_E enMode;
	IVE_SAD_OUT_CTRL_E enOutCtrl;
	HI_U16 u16Thr;				/*srcVal <= u16Thr, dstVal = minVal; srcVal > u16Thr, dstVal = maxVal.*/
	HI_U8 u8MinVal;				/*Min value*/
	HI_U8 u8MaxVal;				/*Max value*/
}IVE_SAD_CTRL_S; 

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
#endif/*_HI_IVE_H_*/