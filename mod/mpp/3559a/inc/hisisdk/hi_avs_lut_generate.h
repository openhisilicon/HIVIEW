/******************************************************************************
Copyright (C), 2017-2018, Hisilicon Tech. Co., Ltd.
******************************************************************************
File Name     : hi_avs_lut_generate.h
Version       : Version 4.1 (for Hi3559A)
Created       : 2017/7/5
Last Modified :
Description   : API for mesh(LUT) library in ARM.
Function List :
History       :

1.Date        : 2018/02/26
Modification  : First time to delivery
******************************************************************************/

#ifndef __HI_AVS_LUT_GENERATE_H__
#define __HI_AVS_LUT_GENERATE_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "hi_type.h"
#include "hi_comm_avs.h"

#define AVS_MAX_CAMERA_NUMBER 8
#define AVS_LUT_SIZE  0x400000   //4MB
#define AVS_CALIBRATION_FILE_LENGTH 0x2800 //10KB


/* the status of avs interface returned */
typedef enum hiAVS_STATUS_E
{
	AVS_STATUS_EOF = -1,	/*internal error codes*/
	AVS_STATUS_OK = 0,		/*success*/

	/* error statuses*/
	AVS_STATUS_FILE_READ_ERROR,
	AVS_STATUS_FILE_WRITE_ERROR,
	AVS_STATUS_FILE_INVALID,
	AVS_STATUS_ALLOC_FAILED,
	AVS_STATUS_INVALID_PARAM,

	AVS_STATUS_BUTT
}AVS_STATUS_E;

/*
Specification of the mask type
AVS_MASK_SHAPE_RECT:    build a rect shape mask;
AVS_MASK_SHAPE_ELLIPSE: build a ecllipse shape mask, including circl shape;
AVS_MASK_SHAPE_BUTT:    reserved.
*/
typedef enum hiAVS_MASK_SHAPE_E
{
	AVS_MASK_SHAPE_RECT = 0,
	AVS_MASK_SHAPE_ELLIPSE = 1,
	AVS_MASK_SHAPE_BUTT
}AVS_MASK_SHAPE_E;


/*
Specification of calibration file type
AVS_TYPE_AVSP:  calibration file come from AVSP calibration
AVS_TYPE_PTGUI: calibration file come from PTGUI calibration
AVS_TYPE_BUTT:  reserved.
*/
typedef enum hiAVS_TYPE_E
{
	AVS_TYPE_AVSP  = 0,
	AVS_TYPE_PTGUI = 1,
	AVS_TYPE_BUTT
}AVS_TYPE_E;

/*
Specification of the mask define.
enMaskShape : The mask shape type
s32OffsetH   : The offset of center in x direction
s32OffsetV   : The offset of center in y direction
u32HalfMajorAxis : The half of long axis in X direction
u32HalfMinorAxis : The half of short axis in Y direction
*/
typedef struct hiAVS_MASK_DEFINE_S
{
	AVS_MASK_SHAPE_E enMaskShape;
	HI_S32 s32OffsetH;
	HI_S32 s32OffsetV;
	HI_U32 u32HalfMajorAxis;
	HI_U32 u32HalfMinorAxis;
}AVS_MASK_DEFINE_S;  

/*
Specification of each lut mask input.
bSameMask: If true, all the mask using mask[0] parameters, otherwise using eack mask parameters;
bInputYuvMask:  If ture, using the yuv400 mask directly; else creat a new yuv400 by using stMaskDefine parameters.
stMaskDefine    : Mask define for each camera   
u64MaskVirAddr  : Virtual memory for saving each mask, the memory size for each one should be sizeof(HI_U16)*u32Width*u32Height, which u32Width and u32Height are the input resolution.
*/
typedef struct hiAVS_LUT_MASK_S
{
	HI_BOOL bSameMask;
	HI_BOOL bInputYuvMask;
	AVS_MASK_DEFINE_S stMaskDefine[AVS_MAX_CAMERA_NUMBER];
	HI_U64 u64MaskVirAddr[AVS_MAX_CAMERA_NUMBER];
}AVS_LUT_MASK_S;

/*
Generates mesh(LUT) files.
enType:               input calibration file type
u64FileInputVirAddr:  Virtual memory for saving input calibration file, the max memory size is 10KB;
pstMask :          The input parameters which define the mask
fStitchDistance:  The radius of the sphere used to create points to sample from. Stitching will be performed correctly at this distance.  (Ignored if calibration from PTGui.)
enLutAccuracy:    The accuracy of the lookup table. Valid values are 0 or 1.
u64LutOutputVirAddr:  Virtual memory for saving each output lookup tables, the memory size for each one should be 4MB. 
*/
AVS_STATUS_E HI_AVS_LutGenerate(AVS_TYPE_E enType,
                                      HI_U64 u64FileInputVirAddr,
                                      AVS_LUT_MASK_S *pstMask,
                                      HI_FLOAT fStitchDistance,
                                      AVS_LUT_ACCURACY_E enLutAccuracy,
                                      HI_U64 u64LutOutputVirAddr[AVS_MAX_CAMERA_NUMBER]
                                      );

#ifdef __cplusplus
}
#endif

#endif // __hi_avs_lut_generate_H__