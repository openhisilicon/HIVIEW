/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */

#ifndef __AUDIO_DL_ADP_H__
#define __AUDIO_DL_ADP_H__

#include "hi_type.h"

hi_s32 audio_dlpath(hi_char *lib_path);

hi_s32 audio_dlopen(hi_void **lib_handle, hi_char *lib_name);

hi_s32 audio_dlsym(hi_void **func_handle, hi_void *lib_handle, hi_char *func_name);

hi_s32 audio_dlclose(hi_void *lib_handle);

#endif
