
/******************************************************************************
Copyright (C), 2017-2018, Hisilicon Tech. Co., Ltd.
******************************************************************************
File Name     : hi_avs_position_query.h
Version       : Version 1.0
Created       : 2018/1/24
Last Modified :
Description   : API for avs position query library.
Function List :
History       :

1.Date        : 2018/05/09
Modification  : First time to delivery
******************************************************************************/

#ifndef __HI_AVS_POSITION_QUERY_H__
#define __HI_AVS_POSITION_QUERY_H__


#ifdef __cplusplus
extern "C" {
#endif

#include "hi_comm_avs.h"

#define AVS_MAX_INPUT_NUMBER 6

/**Specification of the LUT accuracy*/
typedef enum hiAVS_QUERY_MODE_E
{
    AVS_DST_QUERY_SRC = 0,
    AVS_SRC_QUERY_DST = 1,
    AVS_QUERY_MODE_BUTT
}AVS_QUERY_MODE_E;

typedef struct hiAVS_CONFIG_S
{
    AVS_QUERY_MODE_E       enMeshMode;                           /* mesh generating mode. */
    HI_U32                 u32Camera;                            /* camera number*/
    SIZE_S                 stSrcSize;                            /* Range: Hi3559AV100ES = [256, 8192] | Hi3559AV100 = [256, 8192]   | Hi3556AV100 = [256, 8192]; Size of source image. */
    SIZE_S                 stDstSize;                            /* Range: Hi3559AV100ES = [256, 16384] | Hi3559AV100 = [256, 16384] | Hi3556AV100 = [256, 8192]; Size of target image. */
    AVS_PROJECTION_MODE_E  enPrjMode;                            /* Projection mode. */
    POINT_S                stCenter;                             /* Range: [-16383,16383]: Center point. */
    AVS_FOV_S              stFOV;                                /* Output FOV. */
    AVS_ROTATION_S         stORIRotation;                        /* Output original rotation. */
    AVS_ROTATION_S         stRotation;                           /* Output rotation. */
    HI_U32                 u32MeshWinSize;                       /* Range:[2,256], the window size of mesh data. */
    AVS_LUT_ACCURACY_E     enLutAccuracy;                        /* Accuracy of lut file. */
    HI_U64                 u64LutVirAddr[AVS_MAX_INPUT_NUMBER];  /* the virtual address of lut data. */
}AVS_CONFIG_S;

/**
Generates the lookup table about the position between output image and source image.
pstAvsConfig:     output image config
u64MeshVirAddr:  the virtual address of mesh data to save.
**/
HI_S32 HI_AVS_PosMeshGenerate(AVS_CONFIG_S *pstAvsConfig, HI_U64 u64MeshVirAddr[AVS_MAX_INPUT_NUMBER]);

/**
Query the position in source image space from the output image space
pstDstSize    : the resolution of destination image;
u32WindowSize : the windows size of position mesh data, should be same as generating the position mesh.
u64MeshVirAddr: the virtual address of position mesh data, the memory size should be same as the mesh file.
pstDstPointIn : the input position in destination image space.
pstSrcPointOut: the output position in source image space.
**/
HI_S32 HI_AVS_PosQueryDst2Src(SIZE_S *pstDstSize, HI_U32 u32WindowSize,HI_U64 u64MeshVirAddr,
                                     POINT_S *pstDstPointIn, POINT_S *pstSrcPointOut);

/**
Query the position in output stitch image space from the source image space
pstSrcSize:    the resolution of source image;
u32WindowSize: the windows size of position mesh data, should be same as generating the position mesh.
u64MeshVirAddr:the virtual address of position mesh data, the memory size should be same as the mesh file.
pstSrcPointIn: the input position in source image space.
pstDstPointOut:the output position in destination image space.
**/
HI_S32 HI_AVS_PosQuerySrc2Dst(SIZE_S *pstSrcSize, HI_U32 u32WindowSize, HI_U64 u64MeshVirAddr,
	                          POINT_S *pstSrcPointIn, POINT_S *pstDstPointOut);


#ifdef __cplusplus
}
#endif

#endif /*hi_avs_position_query.h*/
