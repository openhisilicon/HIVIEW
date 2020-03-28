/**
* @file    hi_scene.h
* @brief   scene module struct and interface declaration
*
* Copyright (c) 2012 Huawei Tech.Co.,Ltd
*
* @author    HiMobileCam Reference Develop Team
* @date       2017/12/13
* @version   1.0

*/

#ifndef _HI_SCENE_LOADPARAM_H_
#define _HI_SCENE_LOADPARAM_H_

#include "hi_scene.h"


#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

/** \addtogroup     SCENE_LOADPARAM */
/** @{ */  /** <!-- [SCENE_LOADPARAM] */

#define SCENE_MAX_VIDEOMODE           (7)

typedef struct hiSCENE_VIDEO_MODE_S
{
    HI_SCENE_MODE_S astVideoMode[SCENE_MAX_VIDEOMODE];
} HI_SCENE_VIDEO_MODE_S;

/**
 * @brief Get IniParam
 * @param[in]  s32Index : sensor index
 * @param[in]  pstSceneParam : SceneParam
 * @param[in]  pstVideoMode : VideoMode Param
 * @return   0 success, non-zero error code.
 * @exception    None
 * @author    HiMobileCam Reference Develop Team
 * @date      2017/12/11
 */
HI_S32 HI_SCENE_CreateParam(HI_CHAR *pdirname, HI_SCENE_PARAM_S *pstSceneParam, HI_SCENE_VIDEO_MODE_S *pstVideoMode);


/** @}*/  /** <!-- ==== SCENE_LOADPARAM End ====*/

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


#endif /* End of #ifndef _HI_SCENE_LOADPARAM_H_ */



