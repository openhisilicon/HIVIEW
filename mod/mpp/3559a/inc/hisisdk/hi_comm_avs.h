/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2016-2019. All rights reserved.
 * Description: AVS common header
 * Author: Hisilicon multimedia software group
 * Create: 2016-11-11
 */


#ifndef __HI_COMM_AVS_H__
#define __HI_COMM_AVS_H__

#include "hi_type.h"
#include "hi_common.h"
#include "hi_errno.h"
#include "hi_comm_video.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */

#define HI_ERR_AVS_NULL_PTR       HI_DEF_ERR(HI_ID_AVS, EN_ERR_LEVEL_ERROR, EN_ERR_NULL_PTR)
#define HI_ERR_AVS_NOTREADY       HI_DEF_ERR(HI_ID_AVS, EN_ERR_LEVEL_ERROR, EN_ERR_SYS_NOTREADY)
#define HI_ERR_AVS_INVALID_DEVID  HI_DEF_ERR(HI_ID_AVS, EN_ERR_LEVEL_ERROR, EN_ERR_INVALID_DEVID)
#define HI_ERR_AVS_INVALID_PIPEID HI_DEF_ERR(HI_ID_AVS, EN_ERR_LEVEL_ERROR, EN_ERR_INVALID_PIPEID)
#define HI_ERR_AVS_INVALID_CHNID  HI_DEF_ERR(HI_ID_AVS, EN_ERR_LEVEL_ERROR, EN_ERR_INVALID_CHNID)
#define HI_ERR_AVS_EXIST          HI_DEF_ERR(HI_ID_AVS, EN_ERR_LEVEL_ERROR, EN_ERR_EXIST)
#define HI_ERR_AVS_UNEXIST        HI_DEF_ERR(HI_ID_AVS, EN_ERR_LEVEL_ERROR, EN_ERR_UNEXIST)
#define HI_ERR_AVS_NOT_SUPPORT    HI_DEF_ERR(HI_ID_AVS, EN_ERR_LEVEL_ERROR, EN_ERR_NOT_SUPPORT)
#define HI_ERR_AVS_NOT_PERM       HI_DEF_ERR(HI_ID_AVS, EN_ERR_LEVEL_ERROR, EN_ERR_NOT_PERM)
#define HI_ERR_AVS_NOMEM          HI_DEF_ERR(HI_ID_AVS, EN_ERR_LEVEL_ERROR, EN_ERR_NOMEM)
#define HI_ERR_AVS_NOBUF          HI_DEF_ERR(HI_ID_AVS, EN_ERR_LEVEL_ERROR, EN_ERR_NOBUF)
#define HI_ERR_AVS_ILLEGAL_PARAM  HI_DEF_ERR(HI_ID_AVS, EN_ERR_LEVEL_ERROR, EN_ERR_ILLEGAL_PARAM)
#define HI_ERR_AVS_BUSY           HI_DEF_ERR(HI_ID_AVS, EN_ERR_LEVEL_ERROR, EN_ERR_BUSY)
#define HI_ERR_AVS_BUF_EMPTY      HI_DEF_ERR(HI_ID_AVS, EN_ERR_LEVEL_ERROR, EN_ERR_BUF_EMPTY)

typedef enum hiAVS_LUT_ACCURACY_E {
    AVS_LUT_ACCURACY_HIGH = 0, /* LUT high accuracy. */
    AVS_LUT_ACCURACY_LOW = 1, /* LUT low accuracy. */
    AVS_LUT_ACCURACY_BUTT
} AVS_LUT_ACCURACY_E;

typedef struct hiAVS_LUT_S {
    AVS_LUT_ACCURACY_E enAccuracy; /* RW; Range: [0, 1]; LUT accuracy. */
    HI_U64 u64PhyAddr[AVS_PIPE_NUM]; /* RW; Physical address of the LUT. */
} AVS_LUT_S;

typedef enum hiAVS_PROJECTION_MODE_E {
    AVS_PROJECTION_EQUIRECTANGULAR = 0, /* Equirectangular mode. */
    AVS_PROJECTION_RECTILINEAR = 1, /* Rectilinear mode. */
    AVS_PROJECTION_CYLINDRICAL = 2, /* Cylindrical mode. */
    AVS_PROJECTION_CUBE_MAP = 3, /* Cube map mode. */
    AVS_PROJECTION_BUTT
} AVS_PROJECTION_MODE_E;

typedef enum hiAVS_GAIN_MODE_E {
    AVS_GAIN_MODE_MANUAL = 0, /* Manual gain mode. */
    AVS_GAIN_MODE_AUTO = 1, /* Auto gain mode. */
    AVS_GAIN_MODE_BUTT
} AVS_GAIN_MODE_E;

typedef enum hiAVS_MODE_E {
    AVS_MODE_BLEND = 0, /* Stitching in blend mode. */
    AVS_MODE_NOBLEND_VER = 1, /* Vertical stitching, no blending. */
    AVS_MODE_NOBLEND_HOR = 2, /* Horizontal stitching, no blending. */
    AVS_MODE_NOBLEND_QR = 3, /* Quarter stitching, no blending. */
    AVS_MODE_BUTT
} AVS_MODE_E;

typedef struct hiAVS_GAIN_ATTR_S {
    AVS_GAIN_MODE_E enMode; /* RW; Range: [0, 1]; Gain mode. */
    HI_S32 s32Coef[AVS_PIPE_NUM]; /* RW; Range: [0, 65535]; Gain coef. */
} AVS_GAIN_ATTR_S;

typedef struct hiAVS_ROTATION_S {
    HI_S32 s32Yaw; /* RW; Range: [-18000, 18000]; Yaw angle. */
    HI_S32 s32Pitch; /* RW; Range: [-18000, 18000]; Pitch angle. */
    HI_S32 s32Roll; /* RW; Range: [-18000, 18000]; Roll angle. */
} AVS_ROTATION_S;

typedef struct hiAVS_FOV_S {
    HI_U32 u32FOVX; /* RW; Range: [1000, 36000]; Horizontal FOV. */
    HI_U32 u32FOVY; /* RW; Range: [1000, 18000]; Vertical FOV. */
} AVS_FOV_S;

typedef struct hiAVS_SPLIT_ATTR_S {
    HI_U32 u32PipeNum; /* RW; Range: [2, 6]; Pipe number in the split. */
    AVS_PIPE AVSPipe[AVS_SPLIT_PIPE_NUM]; /* RW; Range: [0, 7]; Pipe IDs in the split. */
} AVS_SPLIT_ATTR_S;

typedef struct hiAVS_CUBE_MAP_ATTR_S {
    HI_BOOL bBgColor; /* RW; whether use background color or not. */
    HI_U32 u32BgColor; /* RW; Range: [0, 0xFFFFFF]; Backgroud color. */
    HI_U32 u32SurfaceLength; /* RW; Range: [256, 4096]; Surface length. */
    POINT_S stStartPoint[AVS_CUBE_MAP_SURFACE_NUM]; /* RW; Start point of each surface. */
} AVS_CUBE_MAP_ATTR_S;

typedef struct hiAVS_OUTPUT_ATTR_S {
    AVS_PROJECTION_MODE_E enPrjMode; /* RW; Range: [0, 3]; Projection mode. */
    POINT_S stCenter; /* Center point. */
    AVS_FOV_S stFOV; /* Output FOV. */
    AVS_ROTATION_S stORIRotation; /* Output original rotation. */
    AVS_ROTATION_S stRotation; /* Output rotation. */
    AVS_SPLIT_ATTR_S stSplitAttr[AVS_SPLIT_NUM]; /* Split attribute for 7 or 8 inputs stitching. */
    AVS_CUBE_MAP_ATTR_S stCubeMapAttr; /* Cube map attribute. */
} AVS_OUTPUT_ATTR_S;

typedef struct hiAVS_GRP_ATTR_S {
    AVS_MODE_E enMode; /* RW; Range: [0, 3]; Group work mode */
    HI_U32 u32PipeNum; /* RW; Range: Hi3559AV100 = [1, 8] | Hi3519AV100 = [1, 4]; Pipe number. */
    HI_BOOL bSyncPipe; /* RW; Whether sync pipe image. */

    AVS_LUT_S stLUT; /* Look up table. */
    AVS_GAIN_ATTR_S stGainAttr; /* Gain attribute. */
    HI_U64 u64BBoxPhyAddr[AVS_PIPE_NUM]; /* RW; Physical address of bounding box data. */

    AVS_OUTPUT_ATTR_S stOutAttr; /* Output attribute. */

    FRAME_RATE_CTRL_S stFrameRate; /* Frame rate control info. */
} AVS_GRP_ATTR_S;

typedef struct hiAVS_CHN_ATTR_S {
    HI_U32 u32Width; /* RW; Range: Hi3559AV100 = [256, 16384] | Hi3519AV100 = [256, 8192]; Width of target image. */
    HI_U32 u32Height; /* RW; Range: [256, 8192]; Height of target image. */
    COMPRESS_MODE_E enCompressMode; /* RW; Compression mode of the output. */
    DYNAMIC_RANGE_E enDynamicRange; /* RW; Dynamic range. */
    HI_U32 u32Depth; /* RW; Range: [0, 8]; Chn user list depth. */
    FRAME_RATE_CTRL_S stFrameRate; /* Frame rate control info. */
} AVS_CHN_ATTR_S;

typedef struct hiAVS_MOD_PARAM_S {
    HI_U32 u32WorkingSetSize; /* RW; Range: [1, 4000000]; Working set size;  */
} AVS_MOD_PARAM_S;

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif /* End of #ifndef __HI_COMM_AVS_H__ */

