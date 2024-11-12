/*
  Copyright (c), 2001-2024, Shenshu Tech. Co., Ltd.
 */

#ifndef HI_SCENE_INNER_H
#define HI_SCENE_INNER_H
#include <pthread.h>
#include "ot_scene.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* For HDR Ver.1, HDRDarkRatio = 64, as noticed */
#define SCENE_HDR_DARKRATIO 64

typedef enum {
    SCENE_THREAD_TYPE_NORMAL = 0,
    SCENE_THREAD_TYPE_LUMINANCE,
    SCENE_THREAD_TYPE_NOTLINEAR,
    SCENE_THREAD_PIPE_TYPE_BUTT
} scene_thread_type;

typedef enum {
    SCENE_DYNAMIC_CHANGE_TYPE_EXPOSURE = 0,
    SCENE_DYNAMIC_CHANGE_TYPE_ISO,
    SCENE_DYNAMIC_PIPE_TYPE_BUTT
} scene_dynamic_change_type;


typedef struct {
    hi_bool thread_flag;
    pthread_t thread;
} scene_thread;

/* mainIsp state */
typedef struct {
    hi_vi_pipe main_pipe_hdl;
    hi_u32 sub_pipe_num;
    hi_vi_pipe sub_pipe_hdl[HI_SCENE_PIPE_MAX_NUM];

    hi_bool metry_fixed;
    hi_bool long_exp;

    hi_u32 long_exp_iso;
    hi_u64 last_normal_exposure;
    hi_u32 last_normal_iso;
    hi_u64 last_luminance_exposure;
    hi_u32 last_luminance_iso;
    hi_u64 last_not_linear_exposure;
    hi_u32 last_not_linear_iso;
    hi_u32 iso;
    hi_u64 exposure;
    hi_u32 wdr_ratio;
} scene_main_pipe_state;

/* scene state */
typedef struct {
    hi_bool scene_init;
    hi_bool support_photo_processing;
    hi_bool pause;
    scene_thread thread_normal;
    scene_thread thread_luminance;
    scene_thread thread_not_linear;
    scene_thread thread_venc;
    pthread_attr_t thread_normal_attr;
    pthread_attr_t thread_luminance_attr;
    pthread_attr_t thread_not_linear_attr;
    pthread_attr_t thread_venc_attr;
    hi_u32 main_pipe_num;
    scene_main_pipe_state main_pipe[HI_SCENE_PIPE_MAX_NUM];
} scene_state;

typedef hi_s32 (*scene_set_dynamic_by_param_cb)(hi_vi_pipe vi_pipe, hi_u64 param, hi_u64 last_param, hi_u8 index);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* End of #ifndef HI_SCENE_INNER_H */
