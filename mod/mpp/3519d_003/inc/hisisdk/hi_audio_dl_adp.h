/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */

#ifndef HI_AUDIO_DL_ADP_H
#define HI_AUDIO_DL_ADP_H

#include "hi_type.h"

#ifdef __cplusplus
extern "C" {
#endif /* End of #ifdef __cplusplus */

hi_s32 hi_audio_dlpath(hi_char *lib_path);

hi_s32 hi_audio_dlopen(hi_void **lib_handle, hi_char *lib_name);

hi_s32 hi_audio_dlsym(hi_void **func_handle, hi_void *lib_handle, hi_char *func_name);

hi_s32 hi_audio_dlclose(hi_void *lib_handle);

#ifdef __cplusplus
}
#endif /* End of #ifdef __cplusplus */

#endif
