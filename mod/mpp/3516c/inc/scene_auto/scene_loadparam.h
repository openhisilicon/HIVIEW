/*
  Copyright (c), 2001-2024, Shenshu Tech. Co., Ltd.
 */

#ifndef SCENE_LOADPARAM_H
#define SCENE_LOADPARAM_H

#include "ot_scene.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


#define SCENE_MAX_VIDEOMODE 7

typedef struct {
    ot_scene_mode video_mode[SCENE_MAX_VIDEOMODE];
} ot_scene_video_mode;


/* *
 * @brief Get IniParam
 * @param[in]  dir_name : sensor ini dir
 * @param[in]  scene_param : SceneParam
 * @param[in]  video_mode : VideoMode Param
 * @return   0 success, non-zero error code.
 * @exception    None
 * @author    MobileCam Reference Develop Team
 * @date      2017/12/11
 */
hi_s32 ot_scene_create_param(const hi_char *dir_name, ot_scene_param *scene_param, ot_scene_video_mode *video_mode);


#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /* End of #ifndef HI_SCENE_LOADPARAM_H */
