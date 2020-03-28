/**
* @file    hi_scene_param.h
* @brief   scene module struct and interface declaration
*
* Copyright (c) 2012 Huawei Tech.Co.,Ltd
*
* @author    HiMobileCam Reference Develop Team
* @date       2017/12/13
* @version   1.0

*/
#include <pthread.h>
#include "hi_scene.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

/**For HDR Ver.1, HDRDarkRatio = 64, as noticed */
#define SCENE_HDR_DARKRATIO (64)

typedef enum tagSCENE_THREAD_TYPE_E
{
    SCENE_THREAD_TYPE_NORMAL = 0,
    SCENE_THREAD_TYPE_LUMINANCE,
    SCENE_THREAD_TYPE_NOTLINEAR,
    SCENE_THREAD_PIPE_TYPE_BUTT
} SCENE_THREAD_TYPE_E;

typedef enum tagSCENE_DYNAMIC_CHANGE_TYPE_E
{
    SCENE_DYNAMIC_CHANGE_TYPE_EXPOSURE = 0,
    SCENE_DYNAMIC_CHANGE_TYPE_ISO,
    SCENE_DYNAMIC_PIPE_TYPE_BUTT
} SCENE_DYNAMIC_CHANGE_TYPE_E;


typedef struct tagSCENE_THREAD_S
{
    HI_BOOL bThreadFlag;
    pthread_t pThread;
} SCENE_THREAD_S;

/**mainIsp state*/
typedef struct tagSCENE_MAINPIPE_STATE_S
{
    HI_HANDLE MainPipeHdl;
    HI_U32 u32SubPipeNum;
    HI_HANDLE aSubPipeHdl[HI_SCENE_PIPE_MAX_NUM];

    HI_BOOL bMetryFixed;
    HI_BOOL bDISEnable;
    HI_BOOL bLongExp;

    HI_U32 u32LongExpIso;
    HI_U64 u64LastNormalExposure;
    HI_U32 u32LastNormalIso;
    HI_U64 u64LastLuminanceExposure;
    HI_U32 u32LastLuminanceISO;
    HI_U64 u64LastNotLinearExposure;
    HI_U32 u32LastNotLinearISO;
    HI_U32 u32Iso;
    HI_U64 u64Exposure;
    HI_U32 u32WdrRatio;
} SCENE_MAINPIPE_STATE_S;

/**scene state*/
typedef struct tagSCENE_STATE_S
{
    HI_BOOL bSceneInit;
    HI_BOOL bSupportPhotoProcessing;
    HI_BOOL bPause;
    SCENE_THREAD_S stThreadNormal;
    SCENE_THREAD_S stThreadLuminance;
    SCENE_THREAD_S stThreadNotLinear;
    pthread_attr_t stThreadnormattr;
    pthread_attr_t stThreadluminanceattr;
    pthread_attr_t stThreadnotlinearattr;
    HI_U32 u32MainPipeNum;
    SCENE_MAINPIPE_STATE_S astMainPipe[HI_SCENE_PIPE_MAX_NUM];
} SCENE_STATE_S;

typedef HI_S32(*SCENE_SET_DYNAMIC_BYPARAM_CALLBACK_FN_PTR)(VI_PIPE ViPipe, HI_U64 u64Param, HI_U64 u64LastParam, HI_U8 u8Index);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


