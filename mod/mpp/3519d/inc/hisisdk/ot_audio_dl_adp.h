/*
  Copyright (c), 2001-2024, Shenshu Tech. Co., Ltd.
 */

#ifndef OT_AUDIO_DL_ADP_H
#define OT_AUDIO_DL_ADP_H

#include "ot_type.h"

td_s32 ot_audio_dlpath(td_char *lib_path);

td_s32 ot_audio_dlopen(td_void **lib_handle, td_char *lib_name);

td_s32 ot_audio_dlsym(td_void **func_handle, td_void *lib_handle, td_char *func_name);

td_s32 ot_audio_dlclose(td_void *lib_handle);

#endif
